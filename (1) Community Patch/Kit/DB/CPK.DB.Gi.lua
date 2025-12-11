local lua_next = next
local lua_type = type
local lua_error = error
local lua_tostring = tostring

local lua_setmetatable = setmetatable

local lua_table_concat = table.concat
local lua_string_format = string.format

local lua_coroutine_wrap = coroutine.wrap
local lua_coroutine_yield = coroutine.yield

local civ_db_query = DB.Query
local civ_db_create_query = DB.CreateQuery

local IsTable = CPK.Type.IsTable
local IsNumber = CPK.Type.IsNumber
local IsString = CPK.Type.IsString
local IsBoolean = CPK.Type.IsBoolean

local TableKeys = CPK.Table.Keys
local TableEmpty = CPK.Table.Empty

local AssertError = CPK.Assert.Error
local AssertIsString = CPK.Assert.IsString

--- @class GiRow
--- @field [string] nil | string | number | boolean

--- @class GiRowMeta
--- @field __index fun(row: GiRow, colName: string): nil | string | number | boolean
--- @field __newindex fun(row: GiRow, colName: string, colData: any)

--- @class GiTblCol
--- @field name string # Column name
--- @field null boolean # Is column nullable
--- @field prim boolean # Is column primary
--- @field type 'string' | 'number' | 'boolean'

--- @class GiTblInfo
--- @field tblName string # Table name
--- @field tblCols table<string, GiTblCol> # Table columns
--- @field tblBols table<string, GiTblCol> # Table boolean columns
--- @field tblNils table<string, GiTblCol> # Table nullable columns
--- @field tblIdxs { string: nil | GiTblCol, number: nil | GiTblCol }

--- @alias GiTblData
--- | { number: table<number, GiRow> }
--- | { string: table<string, GiRow> }
--- | { number: table<number, GiRow>, string: table<string, GiRow> }

--- @class GiTbl
--- @field [string | number] nil | GiRow
--- @operator len(): number
--- @operator call(string | table<string, number | string | boolean>): fun(): nil | GiRow

--- @class Gi
--- @field [string] GiTbl

--- @param rowType string
--- @return boolean
local function SqliteIsBooleanAffinity(rowType)
	return rowType:lower():match('^bool') ~= nil
end

--- @param rowType string
--- @return boolean
local function SqliteIsNumericAffinity(rowType)
	local t = rowType:lower()
	local r = t:match('^int')
			or t:match('^doub')
			or t:match('^real')
			or t:match('^float')

	return r ~= nil
end

--- @param val number | boolean | nil
--- @return boolean
local function Boolify(val)
	if IsBoolean(val) then
		return val --[[@as boolean]]
	end

	if IsNumber(val) then
		if val ~= 0 and val ~= 1 then
			AssertError(lua_type(val) .. ' ' .. lua_tostring(val), '1 | 0')
		end

		return val == 1
	end

	return val ~= nil
end

--- Checks if specified value type is allowed
--- @param val any
--- @return boolean # Of specified value allowed
--- @return type # Type of specified value
local function Allowed(val)
	local type = lua_type(val)
	local allow = type == 'number'
			or type == 'string'
			or type == 'boolean'

	return allow, type
end

local ExistsGiTbl = (function()
	local query = civ_db_create_query([[
		SELECT NULL FROM sqlite_master
		WHERE type = 'table' AND name = ?
	]])

	--- Checks if DB table exists by table name
	--- @param tblName string # DB Table name
	--- @return boolean
	return function(tblName)
		return query(tblName)() ~= nil
	end
end)()

local SelectGiTblSize = (function()
	local tmp = 'SELECT COUNT() as Count FROM %s'

	--- Counts DB table rows by table name
	--- @param tblName string # DB Table name
	--- @return integer
	return function(tblName)
		local sql = lua_string_format(tmp, tblName)
		local row = civ_db_query(sql)()

		return row.Count
	end
end)()

local PragmaGiTblInfo = (function()
	local tmp = 'PRAGMA table_info(%s)'

	--- @param tblName string
	--- @return fun(): { pk: 1 | 0, notnull: 1 | 0, name: string, type: string }
	return function(tblName)
		local sql = lua_string_format(tmp, tblName)
		return civ_db_query(sql)
	end
end)()

local function PrepareQueryAll(tblName)
	local tmp = 'SELECT _ROWID_ as _ROWID_, * FROM %s'
	local sql = lua_string_format(tmp, tblName)
	local qry = civ_db_create_query(sql)

	--- @return fun(): nil | GiRow
	return function()
		return qry()
	end
end

local function DefaultColSetter()
	lua_error('Row can not be changed, make a copy instead')
end

--- @param info GiTblInfo # Table info
local function PrepareColGetter(info)
	local tblName = info.tblName
	local tblCols = info.tblCols
	local tblNils = info.tblNils

	--- @param _ GiRow
	--- @param colName string
	--- @return nil
	return function(_, colName)
		AssertIsString(colName, 'Column name can be only string')

		if not tblCols[colName] then
			local tmp = 'Column "%s.%s" is not defined'
			local mes = lua_string_format(tmp, tblName, colName)
			AssertError(colName, 'column name', mes)
		end

		if not tblNils[colName] then
			local tmp = 'Column "%s.%s" is not nullable'
			local mes = lua_string_format(tmp, tblName, colName)
			AssertError('nil', 'string | number', mes)
		end

		return nil
	end
end

--- @param info GiTblInfo # Table info
--- @param fltr table<string, string | number | boolean> # Filter table
local function PrepareColFilter(info, fltr)
	local tblName = info.tblName
	local tblCols = info.tblCols
	local tblBols = info.tblBols

	for key, val in lua_next, fltr do
		local col = tblCols[key]

		if not col then
			local tmp = 'Column "%s.%s" does not exist, but filter specifies it'
			lua_error(lua_string_format(tmp, tblName, key))
		end

		local allowed, type = Allowed(val)

		if not allowed then
			local tmp = 'Column "%s.%s" can not be filtered by type %s'
			local mes = lua_string_format(tmp, tblName, key, type)
			AssertError(type, 'number | string | boolean', mes)
		end

		if type == 'number' and col.type == 'boolean' then
			if val ~= 0 and val ~= 1 then
				AssertError(lua_tostring(val), '1 | 0 | boolean')
			end
		elseif type ~= col.type then
			local tmp = 'Column "%s.%s is %s, but filter specifies %s'
			local mes = lua_string_format(tmp, tblName, key, col.type, type)
			AssertError(type, col.type, mes)
		end
	end

	--- @param row GiRow
	--- @return boolean
	return function(row)
		for key, val in lua_next, fltr do
			if tblBols[key] then
				val = Boolify(val --[[@as number | boolean]])
			end

			if row[key] ~= val then
				return false
			end
		end

		return true
	end
end

local GiTblMeta = {}

--- @class GiTblMeta : GiTblInfo
--- @field tblName string
--- @field tblSize integer
--- @field tblData GiTblData
--- @field rowMeta GiRowMeta
--- @field __len fun(self: GiTbl): integer
--- @field __call fun(self: GiTbl, flt: nil | string | table<string, number | string | boolean>): fun(): GiRow
--- @field __index fun(self: GiTbl, key: string | number): nil | GiRow
local GiTblMetaImpl = {}
GiTblMeta.__index = GiTblMetaImpl

--- @param tblName string
--- @return GiTblInfo
local function SelectGiTblInfo(tblName)
	local colms = {}
	local idxes = {}
	local bools = {}
	local nulls = {}

	for row in PragmaGiTblInfo(tblName) do
		local name = row.name
		local prim = Boolify(row.pk)
		local null = not Boolify(row.notnull)
		local type = (SqliteIsBooleanAffinity(row.type) and 'boolean')
				or (SqliteIsNumericAffinity(row.type) and 'number')
				or 'string'

		local col = {
			name = name,
			null = null,
			prim = prim,
			type = type,
		} --[[@as GiTblCol]]

		colms[name] = col

		if type == 'boolean' then
			bools[name] = col
		end

		if null then
			nulls[name] = col
		end

		if prim then
			if type == 'number' then
				idxes.number = col
			end
			if type == 'string' then
				idxes.string = col
			end
		else
			if name == 'Type' and type == 'string' then
				idxes.string = col
			end
		end
	end

	return {
		tblName = tblName,
		tblCols = colms,
		tblIdxs = idxes,
		tblNils = nulls,
		tblBols = bools,
	} --[[@as GiTblInfo]]
end

--- @param row GiRow
--- @return number
--- @return GiRow
function GiTblMetaImpl:Hydrate(row)
	lua_setmetatable(row, nil)
	local rowId = row._ROWID_ --[[@as number]]
	row._ROWID_ = nil

	return rowId, lua_setmetatable(row, self.rowMeta) --[[@as GiRow]]
end

function GiTblMetaImpl:Populate()
	local qry = PrepareQueryAll(self.tblName)
	local tblData = self.tblData
	local tblIdxs = self.tblIdxs

	local numColName = tblIdxs.number and tblIdxs.number.name
	local strColName = tblIdxs.string and tblIdxs.string.name

	local fallback = not numColName and not strColName

	for rawRow in qry() do
		local rowId, row = self:Hydrate(rawRow)

		if fallback then
			tblData.number[rowId + 1] = row
		end

		if numColName then
			tblData.number[row[numColName] + 1] = row
		end

		if strColName then
			tblData.string[row[strColName]] = row
		end
	end
end

--- @return fun(): nil | GiRow
function GiTblMetaImpl:SelectAll()
	local src = self.tblData.number or self.tblData.string
	local key = nil
	local row = nil

	return function()
		key, row = lua_next(src, key)

		return row
	end
end

--- @param fltr table<string, string | number | boolean>
--- @return fun(): nil | GiRow
function GiTblMetaImpl:SelectAllByFilterTbl(fltr)
	local src = self.tblData.number or self.tblData.string
	local flt = PrepareColFilter(self, fltr)

	local key = nil
	local row = nil

	return function()
		repeat
			key, row = lua_next(src, key)

			if row and flt(row) then
				return row
			end
		until row == nil
	end
end

--- @param fltr string
--- @return fun(): nil | GiRow
function GiTblMetaImpl:SelectAllByFilterStr(fltr)
	local tblName = self.tblName
	local tblIdxs = self.tblIdxs
	local colName = '_ROWID_'
	local colType = 'number'

	if tblIdxs.number then
		colName = tblIdxs.number.name
	elseif tblIdxs.string then
		colName = tblIdxs.string.name
		colType = 'string'
	end

	local src = self.tblData[colType]
	local tmp = 'SELECT %s as %s FROM %s WHERE %s'
	local sql = lua_string_format(tmp, colName, colName, tblName, fltr)
	local qry = civ_db_create_query(sql)

	return lua_coroutine_wrap(function()
		for row in qry() do
			local key = row[colName]

			if key ~= nil then
				if colType == 'number' then
					key = key + 1
				end

				lua_coroutine_yield(src[key])
			end
		end
	end)
end

--- @param tblName string
--- @return GiTblMeta
function GiTblMeta.New(tblName)
	local this = SelectGiTblInfo(tblName)

	--[[@cast this GiTblMeta]]
	this.tblSize = SelectGiTblSize(tblName)
	this.rowMeta = {
		__newindex = DefaultColSetter,
		__index = PrepareColGetter(this),
	}
	this.tblData = {}

	if this.tblIdxs.number then
		this.tblData.number = {}
	end

	if this.tblIdxs.string then
		this.tblData.string = {}
	end

	-- Fallback to rowid stored as number
	if TableEmpty(this.tblData) then
		this.tblData.number = {}
	end

	lua_setmetatable(this, GiTblMeta)

	this.__len = function()
		return this.tblSize
	end

	this.__index = function(_, key)
		local keyType = lua_type(key)
		local typeCol = this.tblIdxs[keyType]

		if not typeCol then
			local tmp = 'Table "%s" does not support indexing by %s type'
			local mes = lua_string_format(tmp, this.tblName, keyType)
			local uni = lua_table_concat(TableKeys(this.tblIdxs), ' | ')
			AssertError(keyType, uni == '' and 'nothing' or uni, mes)
		end

		if keyType == 'number' then
			key = key + 1
		end

		return this.tblData[keyType][key]
	end

	this.__call = function(_, fltr)
		if fltr == nil then
			return this:SelectAll()
		end

		if IsTable(fltr) then
			--[[@cast fltr table]]

			if TableEmpty(fltr) then
				AssertError('empty table', 'table with at least one property')
			end

			return this:SelectAllByFilterTbl(fltr)
		end

		if IsString(fltr) then
			--[[@cast fltr string]]

			if fltr == '' then
				AssertError('empty string', 'string that contains sql where part')
			end

			return this:SelectAllByFilterStr(fltr --[[@as string]])
		end

		---@diagnostic disable-next-line: missing-return
		AssertError(lua_type(fltr), 'nil | table | string')
	end

	return this
end

CPK.DB.Gi = lua_setmetatable({}, {
	__index = function(gi, tblName)
		AssertIsString(tblName)

		local exists = ExistsGiTbl(tblName)

		if not exists then
			AssertError('"' .. tblName .. '"', 'existing table name')
		end

		local giTblMeta = GiTblMeta.New(tblName)

		giTblMeta:Populate()

		local giTbl = lua_setmetatable({}, giTblMeta)

		gi[tblName] = giTbl

		return giTbl
	end
})
