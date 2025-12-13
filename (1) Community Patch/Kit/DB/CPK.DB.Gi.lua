local lua_next = next
local lua_type = type
local lua_error = error
local lua_tostring = tostring
local lua_setmetatable = setmetatable
local lua_collectgarbage = collectgarbage

local lua_math_log = math.log
local lua_math_max = math.max
local lua_math_ceil = math.ceil

local lua_table_sort = table.sort
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

--- @param t string # Row type
--- @return boolean
local function sqlite_affinity_is_boolean(t)
	return t:lower():match('^bool') ~= nil
end

--- @param t string # Row type
--- @return boolean
local function sqlite_affinity_is_numeric(t)
	t = t:lower()

	local r = t:match('^int')
			or t:match('^doub')
			or t:match('^real')
			or t:match('^float')

	return r ~= nil
end

--- Sentinel to represent nil values
local NIL = {}

--- @param b any
--- @return boolean
local gi_boolify = function(b)
	if IsBoolean(b) then return b end

	if IsNumber(b) then
		if b ~= 0 and b ~= 1 then
			AssertError(lua_tostring(b), '1 | 0')
		end

		return b == 1
	end

	return b ~= NIL and b ~= nil
end

local function gi_gc_memory()
	return lua_collectgarbage('count') / 1024
end

local function gi_gc_collect()
	return lua_collectgarbage('collect')
end

local gi_tbl_info = (function()
	local tmp = 'PRAGMA table_info(%s)'

	--- @param name string
	--- @return fun(): { pk: 1 | 0, notnull: 1 | 0, name: string, type: string, dflt_value: any }
	return function(name)
		local sql = lua_string_format(tmp, name)
		return civ_db_query(sql)
	end
end)()

local gi_tbl_count = (function()
	local tmp = 'SELECT COUNT() as Count FROM %s'

	--- @type fun(name: string): integer
	return function(name)
		local sql = lua_string_format(tmp, name)
		local row = civ_db_query(sql)()

		return row.Count
	end
end)()

local gi_tbl_exists = (function()
	local query = civ_db_create_query([[
		SELECT NULL FROM sqlite_master
		WHERE type = 'table' AND name = ? AND name ~= 'sqlite%'
	]])

	--- @type fun(name: string): boolean
	return function(name)
		return query(name)() ~= nil
	end
end)()

local gi_col_cardinality = (function()
	local tmp = [[
		SELECT COUNT(DISTINCT %s) + (SUM(%s IS NULL) > 0) as Cardinality FROM %s
	]]

	--- @param tbl_name string
	--- @param col_name string
	--- @return integer
	return function(tbl_name, col_name)
		local sql = lua_string_format(tmp, col_name, col_name, tbl_name)
		local row = civ_db_query(sql)()

		return row.Cardinality
	end
end)()

--- @class GiCol
--- @field name string # Column name
--- @field prim boolean # Primary column
--- @field null boolean # Nullable column
--- @field dflt unknown # Default value for column
--- @field type 'number' | 'string' | 'boolean'
--- @field bits? integer # Bits required to pack

local gi_tbl_columns = (function()
	local fixs = {
		['-1'] = -1,
		['"-1"'] = -1,
		["'-1'"] = -1,
		['false'] = false,
		['"false"'] = false,
		["'false'"] = false,
		['NULL'] = NIL,
		['"NULL"'] = NIL,
		["'NULL'"] = NIL,
	}

	--- @type fun(d: any): any
	local function normalize_dflt(d)
		if d == nil then
			return NIL
		end

		if fixs[d] ~= nil then
			return fixs[d]
		end

		return d
	end

	--- @type fun(t: string): 'number' | 'string' | 'boolean'
	local function normalize_type(t)
		return (sqlite_affinity_is_boolean(t) and 'boolean')
				or (sqlite_affinity_is_numeric(t) and 'number')
				or 'string'
	end

	--- Calculate the number of bits required to store cardinality
	--- @param card integer # Cardinality
	--- @return integer # Number of bits to represent
	local function bits_for_cardinality(card)
		return lua_math_max(
			1,
			lua_math_ceil(
				lua_math_log(card) / lua_math_log(2)
			)
		)
	end

	--- @type fun(bits: integer): boolean
	local function bits_packable(bits)
		return bits <= 8
	end

	--- @param name string # Table name
	--- @return table<string, GiCol[]> # Table columns
	--- @return GiCol? # Primary index column
	--- @return GiCol? # Support index column
	return function(name)
		local cols = {}
		local main = nil
		local supp = nil

		for r in gi_tbl_info(name) do
			local c = {
				name = r.name,
				prim = gi_boolify(r.pk),
				type = normalize_type(r.type),
				dflt = normalize_dflt(r.dflt_value),
				null = not gi_boolify(r.notnull),
			}

			if c.prim then
				if c.type == 'number' then main = c end
				if c.type == 'string' then main = c end
			elseif c.type == 'boolean' then
				-- do nothing
			elseif c.type == 'string' and c.name == 'Type' then
				supp = c
			else
				local card = gi_col_cardinality(name, c.name)
				local bits = bits_for_cardinality(card)

				if bits_packable(bits) then
					c.bits = bits
				end
			end

			cols[c.name] = c
		end

		return cols, main, supp
	end
end)()

--- @param cols table<string, GiCol>
--- @param main GiCol?
--- @param supp GiCol?
local function gi_row_layout(cols, main, supp)
	local bools = {}
	local packs = {}
	local other = {}

	for _, col in lua_next, cols do
		if col ~= main and col ~= supp then
			if col.type == 'boolean' then
				bools[#bools + 1] = col
			elseif col.bits then
				packs[#packs + 1] = col
			else
				other[#other + 1] = col
			end
		end
	end

	lua_table_sort(packs, function(a, b)
		return a.bits > b.bits
	end)

	local width = 0
	local slots = {}

	if main then
		width = width + 1
		slots[main.name] = width
	end

	if supp then
		width = width + 1
		slots[supp.name] = width
	end

	local bool_slot, bool_nbit = nil, 0

	for i = 1, #bools do
		local col = bools[i]

		if bool_nbit == 0 then
			width = width + 1
			bool_slot = width
		end

		slots[col.name] = { bool_slot, bool_nbit }

		bool_nbit = bool_nbit + 1

		if bool_nbit == 32 then
			bool_slot = nil
			bool_nbit = 0
		end
	end

	local pack_slot, pack_nbit = nil, 0

	for i = 1, #packs do
		local col = packs[i]
		local bits = col.bits

		if not pack_slot or (bits + pack_nbit) > 32 then
			width = width + 1
			pack_slot = width
			pack_nbit = 0
		end

		slots[col.name] = { pack_slot, pack_nbit, bits }

		pack_nbit = pack_nbit + bits
	end

	for i = 1, #other do
		width = width + 1
		slots[other[i].name] = width
	end

	return width, slots
end



--- @param tblName string
local function PrepareQueryAll(tblName)
	local tmp = 'SELECT _ROWID_ as _ROWID_, * FROM %s'
	local sql = lua_string_format(tmp, tblName)

	return civ_db_create_query(sql) --[[@as fun(): nil | GiRow]]
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

		local type = lua_type(val)
		local isok = type == 'number'
				or type == 'string'
				or type == 'boolean'

		if not isok then
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
	this.tblSize = GiCountTblRows(tblName)
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

		local exists = GiExistsTbl(tblName)

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

local function Flags(names)

end
