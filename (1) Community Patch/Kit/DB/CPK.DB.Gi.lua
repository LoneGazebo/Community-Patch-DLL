local lua_next = next
local lua_type = type
local lua_error = error
local lua_tostring = tostring
local lua_setmetatable = setmetatable
local lua_collectgarbage = collectgarbage

local lua_math_log = math.log
local lua_math_max = math.max
local lua_math_ceil = math.ceil
local lua_math_floor = math.floor

local lua_table_sort = table.sort
local lua_table_concat = table.concat
local lua_string_format = string.format

local lua_coroutine_wrap = coroutine.wrap
local lua_coroutine_yield = coroutine.yield

local civ_db_query = DB.Query
local civ_table_fill = table.fill -- Non standard lua
local civ_db_create_query = DB.CreateQuery

local bit_bor = CPK.Bit.bor
local bit_bnot = CPK.Bit.bnot
local bit_band = CPK.Bit.band
local bit_lshift = CPK.Bit.lshift
local bit_rshift = CPK.Bit.rshift

local IsTable = CPK.Type.IsTable
local IsNumber = CPK.Type.IsNumber
local IsBoolean = CPK.Type.IsBoolean

local TableKeys = CPK.Table.Keys

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

--- @param int integer
--- @param pos integer # Bit position (0-based)
--- @return boolean
local function gi_int_getbool(int, pos)
	return bit_band(int, bit_lshift(1, pos)) ~= 0
end

--- @param int integer
--- @param pos integer # Bit position (0-based)
--- @param val boolean # Bit value to set
--- @return integer
local function gi_int_setbool(int, pos, val)
	local msk = bit_lshift(1, pos)

	if val then
		return bit_bor(int, msk)
	end

	return bit_band(int, bit_bnot(msk))
end

--- @param int integer
--- @param pos integer # Bit offset (0-based)
--- @param len integer # Number of bits to read
--- @return integer
local function gi_int_getnumb(int, pos, len)
	return bit_band(bit_rshift(int, pos), bit_lshift(1, len) - 1)
end

--- @param int integer
--- @param pos integer # Bit offset (0-based)
--- @param len integer # Number of bits to set
--- @param val integer # Number that must fit into specified bits
--- @return integer
local function gi_int_setnumb(int, pos, len, val)
	local msk = bit_lshift(1, len) - 1

	-- clear target bits
	int = bit_band(int, bit_bnot(bit_lshift(msk, pos)))

	-- write new value
	return bit_bor(int, bit_lshift(bit_band(val, msk), pos))
end

local gi_tbl_exists = (function()
	local query = civ_db_create_query([[
		SELECT NULL FROM sqlite_master
		WHERE type = 'table' AND name = ? AND name NOT LIKE 'sqlite%'
	]])

	--- @param name string # Table name
	--- @return boolean
	return function(name)
		return query(name)() ~= nil
	end
end)()

local gi_tbl_info = (function()
	local tmp = 'PRAGMA table_info(%s)'

	--- @param name string # Table name
	--- @return fun(): { pk: 1 | 0, notnull: 1 | 0, name: string, type: string, dflt_value: any }
	return function(name)
		local sql = lua_string_format(tmp, name)
		return civ_db_query(sql)
	end
end)()

local gi_tbl_size = (function()
	local tmp = 'SELECT COUNT() as Count FROM %s'

	--- @param name string # Table name
	--- @return integer
	return function(name)
		local sql = lua_string_format(tmp, name)
		local row = civ_db_query(sql)()

		return row.Count
	end
end)()

local gi_col_card = (function()
	local tmp = [[
		SELECT (COUNT(DISTINCT %s) + (SUM(%s IS NULL) > 0)) as Cardinality FROM %s
	]]

	--- Calculates the number of distinct values for specific column
	--- (Column cardinality)
	--- @param tbl_name string
	--- @param col_name string
	--- @return integer
	return function(tbl_name, col_name)
		local sql = lua_string_format(tmp, col_name, col_name, tbl_name)
		local row = civ_db_query(sql)()

		return row.Cardinality
	end
end)()

--- Sentinel to represent nil values
local NIL = { 'Sentinel that represents nil' }

--- @param b any
--- @return boolean
local gi_hlp_boolify = function(b)
	if IsBoolean(b) then return b end

	if IsNumber(b) then
		if b ~= 0 and b ~= 1 then
			AssertError(lua_tostring(b), '1 | 0')
		end

		return b == 1
	end

	return b ~= NIL and b ~= nil
end

local gi_col_data = (function()
	local tmp = 'SELECT (DISTINCT %s) as %s FROM %s ORDER BY %s IS NOT NULL, %s'

	--- @param tbl_name string
	--- @param col_name string
	--- @return { toidx: table<string, integer>, toval: table<integer, string | number> }
	return function(tbl_name, col_name)
		local toidx = {}
		local toval = {}
		local idx = 0

		local sql = lua_string_format(
			tmp,
			col_name,
			col_name,
			tbl_name,
			col_name,
			col_name
		)

		for row in civ_db_query(sql) do
			local val = row[col_name]

			if val == nil then
				val = NIL
			end

			if toidx[val] == nil then
				idx = idx + 1

				toidx[val] = idx
				toval[idx] = val
			end
		end

		return { toidx = toidx, toval = toval }
	end
end)()

--- @class GiCol
--- @field name string # Column name
--- @field prim boolean # Primary column
--- @field null boolean # Nullable column
--- @field dflt unknown # Default value for column
--- @field type 'number' | 'string' | 'boolean'
--- @field bits? integer # Bits required to store column as dictionary

local gi_tbl_cols = (function()
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
	local function bits_for_card(card)
		return lua_math_max(
			1,
			lua_math_ceil(
				lua_math_log(card) / lua_math_log(2)
			)
		)
	end

	--- @param name string # Table name
	--- @return table<string, GiCol[]> # Table columns
	--- @return GiCol? # Primary index column
	--- @return GiCol? # String index column
	return function(name)
		local cols = {}
		local main = nil
		local supp = nil

		for r in gi_tbl_info(name) do
			local c = {
				name = r.name,
				prim = gi_hlp_boolify(r.pk),
				type = normalize_type(r.type),
				dflt = normalize_dflt(r.dflt_value),
				null = not gi_hlp_boolify(r.notnull),
			}

			if c.prim then
				if c.type == 'number' then main = c end
				if c.type == 'string' then supp = c end
			elseif c.type == 'string' and c.name == 'Type' then
				supp = c
			elseif c.type == 'boolean' then
				c.bits = 1
			else
				c.bits = bits_for_card(gi_col_card(name, c.name))
			end

			cols[c.name] = c
		end

		return cols, main, supp
	end
end)()

--- @param cols GiCol[] # Table columns to pack
--- @param inc fun(): integer # Function that increments row size and returns result
--- @return table<string, table>
local function gi_bfd_pack(cols, inc)
	-- Sort columns by descending bit width (Best-Fit Decreasing prerequisite)
	lua_table_sort(cols, function(a, b)
		return a.bits > b.bits
	end)

	-- bins: list of active 32-bit containers { slot, used }
	-- jmps: resulting mapping col.name -> { slot, bit_offset, bit_length }
	local bins = {}
	local jmps = {}

	-- Process each column in descending size order
	for i = 1, #cols do
		local col = cols[i]
		local req = col.bits -- bits required by this column

		local best_bin = nil
		local best_lft = nil

		-- Find the bin with the smallest leftover space that can still fit req
		for j = 1, #bins do
			local bin = bins[j]
			local lft = 32 - bin.used

			if req <= lft then
				if not best_lft or lft < best_lft then
					best_bin = bin
					best_lft = lft
				end
			end
		end

		-- If no existing bin fits, allocate a new 32-bit slot
		if not best_bin then
			best_bin = { widx = inc(), used = 0 }
			bins[#bins + 1] = best_bin
		end

		-- Record where this column is packed:
		-- word index, starting bit offset, and bit length
		jmps[col.name] = {
			best_bin.widx,
			best_bin.used,
			req,
		}

		-- Advance used bits in the selected bin
		best_bin.used = best_bin.used + req
	end

	return jmps
end

--- @param cols table<string, GiCol>
--- @param main GiCol?
--- @param supp GiCol?
local function gi_row_abi(cols, main, supp)
	local jumps = {}
	local width = 0

	local function inc()
		width = width + 1
		return width
	end

	if main then
		jumps[main.name] = inc()
	end

	if supp then
		jumps[supp.name] = inc()
	end

	local topack_cols = {}
	local simple_cols = {}

	for _, col in lua_next, cols do
		if col ~= main and col ~= supp then
			if col.bits <= 8 then
				topack_cols[#topack_cols + 1] = col
			else
				simple_cols[#simple_cols + 1] = col
			end
		end
	end

	for col_name, col_jmp in lua_next, gi_bfd_pack(topack_cols, inc) do
		jumps[col_name] = col_jmp
	end

	for i = 1, #simple_cols do
		jumps[simple_cols[i].name] = inc()
	end

	return width, jumps
end

local function SetTblRow()
	lua_error('GameInfo tables are read-only')
end

local function SetRowVal()
	lua_error('GameInfo rows are read-only')
end

--- @class GiTblMeta
--- @field tbl_name string # Table name
--- @field tbl_size integer # The amount of rows in the table
--- @field tbl_cols table<string, GiCol> # Table columns
--- @field tbl_data any[] # The flat array of all rows data
--- @field col_main? GiCol # Primary column
--- @field col_supp? GiCol # Support column
--- @field col_jmps table<string, integer | table> # TODO
--- @field row_meta metatable # Row metatable
--- @field row_size integer # Amount of slots that each row occupies in tbl_data
--- @field idx_main? table # TODO
--- @field idx_supp? table # TODO
--- @field idx_back? table # TODO
local GiTblMetaImpl = {}
local GiTblMeta = {}
GiTblMeta.__index = GiTblMetaImpl

function GiTblMeta.New(tbl_name)
	local size = gi_tbl_size(tbl_name)

	local this = {
		tbl_name = tbl_name,
		tbl_size = size,
		tbl_cols = nil,
		tbl_data = nil,

		col_main = nil,
		col_supp = nil,
		col_jmps = nil,

		row_meta = nil,
		row_size = nil,

		idx_main = nil,
		idx_supp = nil,
		idx_back = nil,

		__newindex = nil,
		__index = nil,
		__len = function() return size end
	}

	return lua_setmetatable(this, GiTblMeta)
end

function GiTblMetaImpl:FetchTblCols()
	local cols, main, supp = gi_tbl_cols(self.tbl_name)

	self.tbl_cols = cols
	self.col_main = main
	self.col_supp = supp

	if main then
		self.idx_main = {}
	end

	if supp then
		self.idx_supp = {}
	end

	if not main and not supp then
		self.idx_back = {}
	end

	return self
end

function GiTblMetaImpl:BuildRowAbi()
	local width, jumps = gi_row_abi(
		self.tbl_cols,
		self.col_main,
		self.col_supp
	)

	self.row_size = width
	self.row_jmps = jumps

	return self
end

function GiTblMetaImpl:FetchColData()
	self.col_data = {}

	for col_name, col_jump in lua_next, self.row_jmps do
		if IsTable(col_jump) and col_jump[3] then
			self.col_data[col_name] = gi_col_data(self.tbl_name, col_name)
		end
	end

	return self
end

function GiTblMetaImpl:BuildTblMeta()
	local this = self
	local main = self.col_main
	local supp = self.col_supp

	local function GetTblRow(_, key)
		local keyType = lua_type(key)

		if main and main.type == keyType then
			return this.idx_main[key]
		end

		if supp and supp.type == keyType then
			return this.idx_supp[key]
		end

		local tmp = 'Table "%s" does not support indexing by %s type'
		local mes = lua_string_format(tmp, this.tbl_name, keyType)
		local dat = { main and main.type, supp and supp.type }
		local uni = lua_table_concat(TableKeys(dat), ' | ')
		uni = uni == '' and 'nothing' or uni
		AssertError(keyType, uni, mes)
	end


	self.__newindex = SetTblRow
	self.__index = GetTblRow

	self.__call = function(_, flt)
		if flt == nil then
			local i = 0

			return function()

			end
		end
	end


	return self
end

function GiTblMetaImpl:BuildRowMeta()
	local this = self

	local function GetRowVal(row, key)
		AssertIsString(key, 'Columns are of string type')

		local dat = self.tbl_data
		local jmp = this.row_jmps[key]
		local idx = (row[1] - 1) * this.row_size

		if jmp == nil then
			return nil
		end

		if IsNumber(jmp) then
			return dat[idx + jmp]
		end

		local val = dat[idx + jmp[1]]

		if #jmp == 2 then
			return gi_int_getbool(val, jmp[2])
		end

		return this.col_data[key].toval[gi_int_getnumb(val, jmp[2], jmp[3])]
	end

	self.row_meta = {
		__index = GetRowVal,
		__newindex = SetRowVal,
	}

	return self
end

function GiTblMetaImpl:FetchTblRows()
	local row_size = self.row_size
	local row_meta = self.row_meta
	local row_jmps = self.row_jmps

	local col_data = self.col_data
	local col_main = self.col_main
	local col_supp = self.col_supp

	local idx_main = self.idx_main
	local idx_supp = self.idx_supp
	local idx_back = self.idx_back

	local tbl_data = civ_table_fill({}, self.tbl_size * row_size)

	self.tbl_data  = tbl_data

	local data     = self.tbl_data
	local tmp      = 'SELECT _ROWID_ as _ROWID_, * FROM %s ORDER BY _ROWID_'
	local sql      = lua_string_format(tmp, self.tbl_name)
	local qry      = civ_db_create_query(sql)
	local i        = 0

	for row in qry() do
		lua_setmetatable(row, nil)
		i = i + 1
		local base = (i - 1) * row_size
		data[base + 1] = i

		local r = lua_setmetatable({ i }, row_meta)

		if col_main and idx_main then
			idx_main[row[col_main.name] or NIL] = r
		end

		if col_supp and idx_supp then
			idx_supp[row[col_supp.name] or NIL] = r
		end

		for col_name, col_jump in lua_next, row_jmps do
			local val = row[col_name]

			if IsNumber(row_slot) then
				data[base + row_slot] = val
			elseif #row_slot == 2 then
				if val then
					local idx = base + row_slot[1]
					local int = data[idx] or 0

					data[idx] = bit_bor(int, bit_lshift(1, row_slot[2]))
				end
			else
				local idx = base + row_slot[1]
				local int = data[idx] or 0
				local msk = bit_lshift(1, row_slot[3]) - 1
				local enc = col_data[col_name].toidx[val or NIL]

				int = bit_band(int, bit_bnot(bit_lshift(msk, row_slot[2])))

				data[idx] = bit_bor(int, bit_lshift(enc, row_slot[2]))
			end
		end
	end

	return self
end

local Gi = lua_setmetatable({}, {
	__index = function(self, tbl_name)
		AssertIsString(tbl_name)

		if not gi_tbl_exists(tbl_name) then
			AssertError('"' .. tbl_name .. '"', 'existing table name')
		end

		local meta = GiTblMeta.New(tbl_name)
				:FetchTblCols()
				:BuildRowAbi()
				:FetchColData()
				:BuildTblMeta()
				:BuildRowMeta()
				:FetchTblRows()

		local tbl = lua_setmetatable({}, meta)

		self[tbl_name] = tbl

		return tbl
	end
})

CPK.DB.Gi = Gi
