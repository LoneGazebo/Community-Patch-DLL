--------------------------------------------------------------------------------
-- GameInfo Assembler (GiAsm)
--
-- This module builds an immutable, memory-optimized, read-only view of SQLite
-- tables. The database is assumed to be STATIC:
--   - Tables do not change after load
--   - Rows are immutable
--   - Schema is stable
--
-- Goals:
--   * Minimize memory usage
--   * Avoid per-row Lua tables where possible
--   * Provide O(1) indexed access by primary key
--   * Provide fast iteration and filtering
--
-- Core ideas:
--   * Column-wise encoding (CONST / SCALAR / BOOL / PINT / DICT / SPARSE)
--   * Bit-packing for dense numeric and boolean columns
--   * Dictionary interning across tables
--   * Row materialization is lazy and cached
--
-- High-level pipeline:
--   1. Fetch schema and rows
--   2. Analyze column statistics
--   3. Select optimal encoding per column
--   4. Build indices and dictionaries
--   5. Encode all row data into flat arrays
--   6. Expose a read-only table-like API
--------------------------------------------------------------------------------

local lua_next = next
local lua_type = type
local lua_error = error
local lua_select = select
local lua_setmetatable = setmetatable

local lua_string_format = string.format

local civ_db_query = DB.Query

local civ_table_fill = table.fill
local lua_table_sort = table.sort
local lua_table_concat = table.concat

--------------------------------------------------------------------------------

local Void = CPK.FP.Void
local Bind = CPK.FP.Bind
local Always = CPK.FP.Always

local ArgsUnpack = CPK.Args.Unpack
local ArgsSize = CPK.Args.Size

local IsTable = CPK.Type.IsTable
local IsNumber = CPK.Type.IsNumber
local IsString = CPK.Type.IsString
local IsInteger = CPK.Type.IsInteger
local IsBoolean = CPK.Type.IsBoolean
local IsFunction = CPK.Type.IsFunction

local AssertError = CPK.Assert.Error
local AssertIsTable = CPK.Assert.IsTable
local AssertIsNumber = CPK.Assert.IsNumber
local AssertIsString = CPK.Assert.IsString
local AssertIsInteger = CPK.Assert.IsInteger

local NormalizeAffinity = CPK.DB.Sqlite.NormalizeAffinity

local BitLayoutAssign32 = CPK.Bit.Layout.Assign32

local BitsForCapacity = CPK.Bit.Math.BitsForCapacity
local BitsForRange = CPK.Bit.Math.BitsForRange

local BitFieldSetBit = CPK.Bit.Field.SetBit
local BitFieldGetBit = CPK.Bit.Field.GetBit

local BitFieldSetInt = CPK.Bit.Field.SetInt
local BitFieldGetInt = CPK.Bit.Field.GetInt

--------------------------------------------------------------------------------

--- NIL sentinel
--- SQLite NULL values are normalized into a unique sentinel object so they can:
---  * Be used as table keys
---  * Participate in frequency analysis
---  * Be stored in dictionaries
---
--- The sentinel is decoded back to Lua nil at read time.
--- @class GiNull
local NIL = { 'NIL_SENTINEL' }

--- 0.95 gives best result (tested 0.9, 0.8, 0.7, 0.6)
local SPARSE_DOMINANCE = 0.95

--- Column encoding kinds
--- ```text
--- CONST  : Single value for entire column
--- SCALAR : One Lua slot per row
--- BOOL   : 1-bit packed boolean
--- PINT   : Packed integer range
--- DICT   : Packed dictionary index
--- SPARSE : Default value + sparse overrides
--- ```
--- @enum GiColKind
local GiColKind = {
	BOOL = 'BOOL',
	PINT = 'PINT',
	DICT = 'DICT',
	CONST = 'CONST',
	SCALAR = 'SCALAR',
	SPARSE = 'SPARSE',
}

--- Empty table instance
--- Returned when a table has zero rows.
--- Behaves like an empty, immutable GameInfo table.
--- @type GiTbl
local GI_TBL_EMPTY = lua_setmetatable({}, {
	__len = Always(0),
	__call = Always(Void),
	__index = Void,
	__newindex = Void,
})

--------------------------------------------------------------------------------

-- ABI registries
--
-- These intern ABI descriptors so identical CONST or SCALAR columns
-- share the same Lua table, reducing memory and comparisons.

--- @type table<GiType, GiColAbiConst>
local GI_COL_ABI_REGISTRY_CONST = lua_setmetatable({}, {
	__mode = 'v',
	__index = function(self, default)
		local col_abi = { GiColKind.CONST, default }

		self[default] = col_abi

		return col_abi
	end
})

--- @type table<integer, GiColAbiScalar>
local GI_COL_ABI_REGISTRY_SCALAR = lua_setmetatable({}, {
	__mode = 'v',
	__index = function(self, slot)
		local col_abi = { GiColKind.SCALAR, slot }

		self[slot] = col_abi

		return col_abi
	end
})

--- Global dictionary ABI pool
---
--- Dictionary value arrays are deduplicated across ALL tables.
--- This is safe because the database is static.
--- @type GiType[][]
local GI_COL_ABI_MAPPING_SET_DICT = {}

--------------------------------------------------------------------------------

-- Row index helpers
-- Rows are addressed by a "base" offset into the flat row_data array.
-- base = row_index * row_len

--- Zero-based numeric key [0..N-1]
--- @param tbl_meta GiTblMeta
--- @param row_key integer
--- @param _ any
--- @return nil | GiRow
local GI_ROW_IDX_ZERO_BASED = function(tbl_meta, row_key, _)
	AssertIsInteger(row_key)

	if row_key < 0 or row_key >= tbl_meta.row_cnt then
		return nil
	end

	return tbl_meta.row_memo[row_key * tbl_meta.row_len]
end

--- One-based numeric key [1..N]
--- @param tbl_meta GiTblMeta
--- @param row_key integer
--- @param _ any
--- @return nil | GiRow
local GI_ROW_IDX_ONE_BASED = function(tbl_meta, row_key, _)
	AssertIsInteger(row_key)

	if row_key < 1 or row_key > tbl_meta.row_cnt then
		return nil
	end

	return tbl_meta.row_memo[(row_key - 1) * tbl_meta.row_len]
end

--- Mapped key (dictionary index)
--- @param tbl_meta GiTblMeta
--- @param row_key string | integer
--- @param row_idx table<string | integer, integer>
--- @return nil | GiRow
local GI_ROW_IDX_MAPPED = function(tbl_meta, row_key, row_idx)
	local base = row_idx[row_key]

	if base == nil then
		return nil
	end

	return tbl_meta.row_memo[base]
end

--------------------------------------------------------------------------------

-- Helper functions

--- Counts rows in a table
--- @param tbl_name string
--- @return integer
local function gi_row_count(tbl_name)
	AssertIsString(tbl_name)

	local tmp = 'SELECT COUNT(*) as Count FROM `%s`'
	local sql = lua_string_format(tmp, tbl_name)
	local row = civ_db_query(sql)()

	AssertIsTable(row)
	AssertIsNumber(row.Count)

	return row.Count
end

--- Decode NIL sentinel back to Lua nil or return input
--- @param val any
--- @return any
local function gi_decode_nil(val)
	if val == NIL then
		return nil
	end

	return val
end

--- Dictionary value sorting
---
--- Sorting ensures:
---   * Deterministic dictionary layout
---   * Stable deduplication across tables
---
--- Order:
---   * NIL < number < string < boolean
---   * Then natural ordering inside type
--- @param vals GiType[]
local function gi_sort_mapping(vals)
	local TYPE_RANK = {
		number = 1,
		string = 2,
		boolean = 3,
	}

	local function rank(v)
		if v == NIL then
			return 0
		end
		return TYPE_RANK[lua_type(v)]
	end

	lua_table_sort(vals, function(a, b)
		if a == b then
			return false
		end

		local ra = rank(a)
		local rb = rank(b)

		if ra ~= rb then
			return ra < rb
		end

		if ra == 1 then -- number
			return a < b
		end

		if ra == 2 then -- string
			return a < b
		end

		if ra == 3 then -- boolean
			return (a and 1 or 0) < (b and 1 or 0)
		end

		-- Should not happen
		return false
	end)
end

--------------------------------------------------------------------------------

-- Types

--- Represents a normalized cell value stored internally.
--- Lua nil is never stored directly; missing values are replaced by GiNull.
--- @alias GiType number | string | boolean | GiNull

--- Temporary mutable row representation used during assembly.
--- Backed directly by SQLite query results.
--- All missing values are normalized to GiNull.
--- Keys are column names, values are GiType.
--- @class GiRowTmp
--- @field [string] GiType

--- Public immutable row view exposed to users.
--- Values are decoded from internal storage.
--- GiNull is converted back to Lua nil.
--- Rows are read-only.
--- ```lua
--- -- Examples
---
--- -- Gets row column value (Throws if column is not defined)
--- Gi.Policies[0].Description
---
--- -- Throws because column I_DO_NOT_EXIST is not defined
--- Gi.Policies[0].I_DO_NOT_EXIST
---
--- -- Checks if specified column exists
--- is_column_defined = Gi.Policies.POLICY_LIBERTY('I_DO_NOT_EXIST')
---
--- -- Checks if multiple specified columns exist
--- t, d, s = Gi.Policies.POLICY_LIBERTY('Type', 'Description', 'Unknown')
---
--- ```
--- @class GiRow
--- @field [string] number | string | boolean | nil
--- @overload fun(): table<string, number | string | boolean | nil>
--- @overload fun(...: string): string | number | boolean | nil, string | number | boolean | nil, string | number | boolean | nil, ...

--- Public immutable table object.
--- Supports indexing, iteration and length operator.
--- Entirely read-only.
--- ```lua
--- -- Examples
---
--- -- Gets row by primary number key
--- Gi.Policies[0]
---
--- -- Gets row by primary string key
--- Gi.Defines['START_YEAR']
---
--- -- Gets row by string column Type
--- -- (only if there is also primary number key)
--- Gi.Policies['POLICY_LIBERTY']
---
--- ```
--- @class GiTbl
--- @field [string] nil | GiRow
--- @field [number] nil | GiRow
--- @operator len(): integer
--- @operator call(nil | string | table<string, number | string | boolean>): fun(): nil | GiRow
--- @overload fun(filter: nil | string | table<string, number | string | boolean>, ...: string | number | boolean): fun(): nil | GiRow

--------------------------------------------------------------------------------

-- Column ABI (encoding descriptors)

--- Column ABI for constant columns.
--- Column has exactly one distinct value.
--- No per-row storage is used.
--- @class GiColAbiConst
--- @field [1] GiColKind.CONST
--- @field [2] GiType # Default

--- Column ABI for scalar columns.
--- One Lua slot per row, no bit packing.
--- @class GiColAbiScalar
--- @field [1] GiColKind.SCALAR
--- @field [2] integer # Slot

--- Column ABI for sparse columns.
--- Dominant value stored once, deviations stored in a sparse map.
--- @class GiColAbiSparse
--- @field [1] GiColKind.SPARSE
--- @field [2] GiType # Default
--- @field [3] table<integer, GiType> # Mapping base -> value

--- Column ABI for packed integer columns.
--- Integer values stored as offsets from minimum value.
--- Packed into bit fields.
--- @class GiColAbiPint
--- @field [1] GiColKind.PINT
--- @field [2] integer # Slot
--- @field [3] integer # Bit Position
--- @field [4] integer # Bit Length
--- @field [5] integer # Minimum value

--- Column ABI for dictionary-encoded columns.
--- Distinct values mapped to dense integer ids.
--- Ids packed into bit fields.
--- Dictionary arrays may be shared globally.
--- @class GiColAbiDict
--- @field [1] GiColKind.DICT
--- @field [2] integer # Slot
--- @field [3] integer # Bit Position
--- @field [4] integer # Bit Length
--- @field [5] table<integer, GiType> # Mapping

--- Column ABI for boolean columns.
--- Exactly one bit per row.
--- @class GiColAbiBool
--- @field [1] GiColKind.BOOL
--- @field [2] integer # Slot
--- @field [3] integer # Bit position

--- Union of all column ABI variants.
--- @alias GiColAbi
--- | GiColAbiBool
--- | GiColAbiConst
--- | GiColAbiDict
--- | GiColAbiPint
--- | GiColAbiScalar
--- | GiColAbiSparse

--------------------------------------------------------------------------------

-- Row ABI

--- Row ABI mapping.
--- Maps column name to its column ABI descriptor.
--- Used to decode values from row storage.
--- @class GiRowAbi
--- @field [string] GiColAbi

--- Temporary column descriptor used during analysis and planning.
--- Accumulates statistics and encoding decisions.
--- Discarded after assembly completes.
--- @class GiColTmp
--- @field name string # Column name
--- @field primary boolean # Is column primary
--- @field affinity SqliteAffinity # Column affinity
--- @field kind? GiColKind # Column kind
--- @field slot? integer # Column slot(index) inside row_data
--- @field frequencies? table<GiType, integer> # Value frequencies
--- @field cardinality? integer # Distinct value count
--- @field dominant_cnt? integer # Dominant value frequency
--- @field dominant_val? GiType | nil # Dominant value
--- @field default? GiType # Default value
--- @field bit_pos? integer # Bit position
--- @field bit_len? integer # Bit length
--- @field min_val? integer # Minimum integer value
--- @field max_val? integer # Maximum integer value
--- @field mapping_vals? table<integer, GiType> # Dictionary values
--- @field mapping_idxs? table<GiType, integer> # Reverse dictionary
--- @field mapping_size? integer # Amount of dictionary values
--- @field mapping? table<integer, GiType> # Dictionary values
--- @field is_pint boolean # Is packed integer candidate

--- Internal immutable table metadata.
--- Owns row storage, ABI, indices and metamethods.
--- Shared by all row objects.
--- @class GiTblMeta
--- @field tbl_name string
--- @field row_abi GiRowAbi
--- @field row_cnt integer
--- @field row_len integer
--- @field row_data GiType[]
--- @field row_meta metatable
--- @field row_memo table<integer, table>
--- @field row_num_key? string
--- @field row_str_key? string
--- @field row_num_idx? function | table<number, integer>
--- @field row_str_idx? function | table<string, integer>
--- @field __len fun(): integer
--- @field __call fun(filter: nil | string | table<string, number | string | boolean>, ...): fun(): GiRow
--- @field __index fun(_, key: string | number): GiRow
--- @field __newindex fun(): nil

--- Table assembler.
--- Converts a SQLite table into a compact immutable in-memory representation.
--- Exists only during assembly.
--- @class GiAsm
--- @field tbl_name string
--- @field tbl_rows? { [string]: GiType }[]
--- @field tbl_cols? table<string, GiColTmp>
--- @field tbl_meta? GiTblMeta
--- @field tbl_inst? GiTbl
--- @field row_data? GiType[]
--- @field row_meta? metatable
--- @field row_memo? table<integer, table>
--- @field row_abi GiRowAbi
--- @field row_cnt integer
--- @field row_len integer
--- @field row_num_key? string
--- @field row_str_key? string
--- @field row_num_idx? function | table<number, integer>
--- @field row_str_idx? function | table<string, integer>
local GiAsmImpl = {}

--------------------------------------------------------------------------------

local GiAsm = {}

GiAsm.__index = GiAsmImpl

--- Assemble a table by name.
--- Convenience entry point.
--- @param tbl_name string
--- @return GiTbl
function GiAsm.Assemble(tbl_name)
	return GiAsm.New(tbl_name):Assemble()
end

--- @package
--- @param tbl_name string
--- @return GiAsm
function GiAsm.New(tbl_name)
	AssertIsString(tbl_name)

	local this = {
		tbl_name = tbl_name,
	}

	return lua_setmetatable(this, GiAsm)
end

--- Run full assembly pipeline.
--- Produces a public immutable table.
--- @return GiTbl
function GiAsmImpl:Assemble()
	self.row_cnt = gi_row_count(self.tbl_name)

	if self.row_cnt == 0 then
		return GI_TBL_EMPTY
	end

	self:FetchCols()
	self:FetchRows()
	self:SetupKeys()
	self:Analyze()
	self:SetupCols()
	self:BuildIndices()
	self:DedupIndices()
	self:BuildRowAbi()
	self:BuildRowData()

	self:Finalize()

	return self.tbl_inst
end

--- Fetch all rows from SQLite.
--- Rows are ordered by ROWID.
--- @return nil
function GiAsmImpl:FetchRows()
	self.tbl_rows = {}
	local tbl_rows = self.tbl_rows

	local tmp = 'SELECT _ROWID_ as _ROWID_, * FROM `%s` ORDER BY _ROWID_'
	local sql = lua_string_format(tmp, self.tbl_name)

	for row in civ_db_query(sql) do
		-- detach builtin metatable to supress warnings on nonexistent column access
		tbl_rows[#tbl_rows + 1] = lua_setmetatable(row, nil)
	end
end

--- Fetch column schema information.
--- Initializes analysis metadata.
--- @return nil
function GiAsmImpl:FetchCols()
	self.tbl_cols = {}
	local tbl_cols = self.tbl_cols

	local tmp = 'PRAGMA table_info(`%s`)'
	local sql = lua_string_format(tmp, self.tbl_name)

	for row in civ_db_query(sql) do
		tbl_cols[row.name] = {
			name = row.name,
			primary = row.pk > 0,
			null = row.notnull == 0,
			affinity = NormalizeAffinity(row.type),
			frequencies = {},
			cardinality = 0,
			dominant_cnt = 0,
			dominant_val = nil,
			is_pint = true,
		}
	end
end

--- Determine primary key strategy and index type.
--- Detects numeric sequences and string keys.
--- @return nil
function GiAsmImpl:SetupKeys()
	local row_cnt = self.row_cnt
	local tbl_cols = self.tbl_cols
	local tbl_rows = self.tbl_rows

	--- @type GiColTmp[]
	local primaries = {}

	for _, col in lua_next, tbl_cols do
		if col.primary then
			primaries[#primaries + 1] = col
		elseif col.name == 'Type' and col.affinity == 'string' then
			self.row_str_key = col.name
		end
	end

	if #primaries == 0 or #primaries > 1 then
		self.row_num_key = '_ROWID_'
		self.row_str_key = nil
		self.row_str_idx = nil
	else
		local col = primaries[1]

		if col.affinity == 'number' then
			self.row_num_key = col.name
		elseif col.affinity == 'string' then
			self.row_str_key = col.name
		else
			self.row_num_key = '_ROWID_'
		end
	end

	if self.row_num_key then
		local from_0 = true
		local from_1 = true

		for i = 1, row_cnt do
			--[[@cast tbl_rows GiRowTmp[] ]]
			local row = tbl_rows[i]
			local key = row[self.row_num_key]

			if key ~= (i - 1) then
				from_0 = false
			end

			if key ~= i then
				from_1 = false
			end

			if not from_0 and not from_1 then
				break
			end
		end

		if from_0 then
			self.row_num_idx = GI_ROW_IDX_ZERO_BASED
		elseif from_1 then
			self.row_num_idx = GI_ROW_IDX_ONE_BASED
		end
	end
end

--- Analyze column statistics.
--- Computes frequencies, cardinality, dominant values and integer ranges.
--- @return nil
function GiAsmImpl:Analyze()
	local tbl_rows = self.tbl_rows
	local tbl_cols = self.tbl_cols

	for _, row in lua_next, tbl_rows do
		for _, col in lua_next, tbl_cols do
			local key = col.name
			local val = row[key]

			if val == nil then
				val = NIL
				row[key] = NIL
			end

			local frq = col.frequencies[val]

			-- frequency / cardinality
			if frq == nil then
				frq = 1
				col.cardinality = col.cardinality + 1
				col.frequencies[val] = frq
			else
				frq = frq + 1
				col.frequencies[val] = frq
			end

			-- dominant value
			if frq > col.dominant_cnt then
				col.dominant_cnt = frq
				col.dominant_val = val
			end

			-- numeric analysis
			if val ~= NIL and col.is_pint then
				if not IsNumber(val) or val % 1 ~= 0 then
					col.is_pint = false
					col.min_val = nil
					col.max_val = nil
				else
					if col.min_val == nil or val < col.min_val then
						col.min_val = val --[[@as number]]
					end

					if col.max_val == nil or val > col.max_val then
						col.max_val = val --[[@as number]]
					end
				end
			else
				col.is_pint = false
				col.min_val = nil
				col.max_val = nil
			end
		end
	end
end

--- Choose optimal encoding for each column.
--- Assigns bit layouts and row slots.
--- @return nil
function GiAsmImpl:SetupCols()
	local row_cnt = self.row_cnt

	-- Columns that require bit packing (BOOL / PNUM / DICT)
	local packs = {}

	--- @param col GiColTmp
	local decide_kind = function(col)
		-- IMPORTANT:
		-- Order of conditions is semantically significant.
		-- Earlier branches represent strictly cheaper / more constrained encodings.
		-- Do not reorder unless the cost model changes.

		-- Empty table: no data, treat as scalar placeholder
		if row_cnt == 0 then
			col.kind = GiColKind.SCALAR

			return
		end

		-- Single distinct value: store once, no per-row storage
		if col.cardinality == 1 then
			col.kind = GiColKind.CONST
			col.default = col.dominant_val

			return
		end

		-- Boolean columns: 1 bit per row, always optimal
		if col.affinity == 'boolean' then
			col.kind = GiColKind.BOOL
			col.bit_len = 1

			packs[#packs + 1] = col

			return
		end

		-- PNUM candidate, column must contain only integers
		if col.is_pint then
			-- Bits required to encode value offset (val - min_val)
			local pnum_bit_len = BitsForRange(col.min_val, col.max_val)

			-- Bits required for DICT index
			local dict_bit_len = BitsForCapacity(col.cardinality)

			-- Estimated memory cost
			local dict_tbl_over = (col.cardinality * 32) -- dictionary table overhead
			local pnum_mem_cost = (row_cnt * pnum_bit_len)
			local dict_mem_cost = (row_cnt * dict_bit_len) + dict_tbl_over

			-- PNUM is chosen only if
			-- 1) Fits in a single 32-bit word slot
			-- 2) Cheaper than DICT for this column
			if pnum_bit_len <= 32 and pnum_mem_cost < dict_mem_cost then
				col.kind = GiColKind.PINT
				col.bit_len = pnum_bit_len

				packs[#packs + 1] = col

				return
			end
		end

		-- Dominant value stored as default, only deviations stored
		if (col.dominant_cnt / row_cnt) >= SPARSE_DOMINANCE then
			col.kind = GiColKind.SPARSE
			col.default = col.dominant_val
			col.mapping = {}

			return
		end

		-- Small cardinality, value remapped to dense integer IDs
		if col.cardinality <= 256 then
			col.kind = GiColKind.DICT
			col.bit_len = BitsForCapacity(col.cardinality)
			col.mapping_idxs = {}
			col.mapping_vals = {}
			col.mapping_size = 0
			packs[#packs + 1] = col

			return
		end

		-- Stored directly in flat array, no packing
		col.kind = GiColKind.SCALAR
	end

	for _, col in lua_next, self.tbl_cols do
		decide_kind(col)
	end

	local row_len, _ = BitLayoutAssign32(packs)

	for _, col in lua_next, self.tbl_cols do
		if col.kind == GiColKind.SCALAR then
			row_len = row_len + 1
			col.slot = row_len
		end
	end

	self.row_len = row_len
end

--- Build primary key indices and column mappings.
--- @return nil
function GiAsmImpl:BuildIndices()
	local tbl_rows = self.tbl_rows --[[@as GiRowTmp[] ]]
	local tbl_cols = self.tbl_cols
	local row_cnt = self.row_cnt
	local row_len = self.row_len

	local num_key = self.row_num_key
	local str_key = self.row_str_key

	if num_key and not IsFunction(self.row_num_idx) then
		self.row_num_idx = {}
	end

	if str_key then
		self.row_str_idx = {}
	end

	for i = 1, row_cnt do
		local row = tbl_rows[i]
		local base = row_len * (i - 1)

		local num_val = num_key and row[num_key]
		local str_val = str_key and row[str_key]

		if num_val and not IsFunction(self.row_num_idx) then
			self.row_num_idx[num_val] = base
		end

		if str_val then
			self.row_str_idx[str_val] = base
		end

		for _, col in lua_next, tbl_cols do
			local val = row[col.name]
			local kind = col.kind

			if kind == GiColKind.SPARSE and val ~= col.default then
				col.mapping[base] = val
			elseif kind == GiColKind.DICT then
				local idx = col.mapping_idxs[val]

				if idx == nil then
					idx = 1 + col.mapping_size

					col.mapping_size = idx
					col.mapping_idxs[val] = idx
					col.mapping_vals[idx] = val
				end
			end
		end
	end
end

--- Deduplicate dictionary mappings across columns and tables.
--- @return nil
function GiAsmImpl:DedupIndices()
	local tbl_cols = self.tbl_cols

	for _, col in lua_next, tbl_cols do
		if col.kind == GiColKind.DICT then
			col.mapping_idxs = {}
			local vals = col.mapping_vals --[[@as table]]

			gi_sort_mapping(vals)

			for idx, val in lua_next, vals do
				col.mapping_idxs[val] = idx
			end

			local len = #vals
			local found = false

			for _, ref in lua_next, GI_COL_ABI_MAPPING_SET_DICT do
				if #ref == len then
					local eql = true

					for idx, val in lua_next, vals do
						if ref[idx] ~= val then
							eql = false
							break;
						end
					end

					if eql then
						col.mapping_vals = ref
						found = true
						break
					end
				end
			end

			if not found then
				GI_COL_ABI_MAPPING_SET_DICT[#GI_COL_ABI_MAPPING_SET_DICT + 1] = vals
			end
		end
	end
end

--- Build final row ABI from column metadata.
--- @return nil
function GiAsmImpl:BuildRowAbi()
	local tbl_cols = self.tbl_cols

	self.row_abi = {}
	local row_abi = self.row_abi

	for _, col in lua_next, tbl_cols do
		local name = col.name
		local kind = col.kind

		if kind == GiColKind.CONST then
			row_abi[name] = GI_COL_ABI_REGISTRY_CONST[col.default]
		elseif kind == GiColKind.SCALAR then
			row_abi[name] = GI_COL_ABI_REGISTRY_SCALAR[col.slot]
		else
			local col_abi = { kind }
			row_abi[name] = col_abi

			if kind == GiColKind.BOOL then
				col_abi[2] = col.slot
				col_abi[3] = col.bit_pos
			elseif kind == GiColKind.PINT then
				col_abi[2] = col.slot
				col_abi[3] = col.bit_pos
				col_abi[4] = col.bit_len
				col_abi[5] = col.min_val
			elseif kind == GiColKind.DICT then
				col_abi[2] = col.slot
				col_abi[3] = col.bit_pos
				col_abi[4] = col.bit_len
				col_abi[5] = col.mapping_vals
			elseif kind == GiColKind.SPARSE then
				col_abi[2] = col.default
				col_abi[3] = col.mapping
			end
		end
	end
end

--- Encode all rows into flat row storage.
--- @return nil
function GiAsmImpl:BuildRowData()
	local tbl_rows = self.tbl_rows --[[@as GiRowTmp[] ]]
	local tbl_cols = self.tbl_cols --[[@as table<string, GiColTmp>]]
	local row_cnt = self.row_cnt
	local row_len = self.row_len
	local row_abi = self.row_abi

	self.row_data = civ_table_fill(0, row_cnt * row_len)
	local row_data = self.row_data --[[@as GiType[] ]]

	for i = 1, row_cnt do
		local row = tbl_rows[i]
		local base = row_len * (i - 1)

		for col_name, col_abi in lua_next, row_abi do
			local val = row[col_name]
			local col_kind = col_abi[1]

			if col_kind == GiColKind.SCALAR then
				local slot = col_abi[2]
				local pos = base + slot

				row_data[pos] = val
			elseif col_kind == GiColKind.BOOL then
				local slot, bit_pos = col_abi[2], col_abi[3]

				local pos = base + slot
				local wrd = row_data[pos] --[[@as integer]]

				if val == NIL then
					val = false
				end

				row_data[pos] = BitFieldSetBit(wrd, bit_pos, val --[[@as boolean]])
			elseif col_kind == GiColKind.PINT then
				local slot = col_abi[2]
				local bit_pos, bit_len, min_val = col_abi[3], col_abi[4], col_abi[5]

				local pos = base + slot
				local wrd = row_data[pos] --[[@as integer]]
				local int = val - min_val

				row_data[pos] = BitFieldSetInt(wrd, bit_pos, bit_len, int)
			elseif col_kind == GiColKind.DICT then
				local slot, bit_pos, bit_len = col_abi[2], col_abi[3], col_abi[4]

				local pos = base + slot
				local wrd = row_data[pos] --[[@as integer]]
				local int = tbl_cols[col_name].mapping_idxs[val] - 1

				row_data[pos] = BitFieldSetInt(wrd, bit_pos, bit_len, int)
			end
		end
	end
end

--- Decode a single column value from internal row storage.
--- This is the lowest-level value accessor.
---
--- The function assumes that:
--- - `tbl_meta` is a fully initialized table metadata object
--- - `col_abi` is a valid column ABI descriptor
--- - `base` is a valid row base offset (row_len * row_index)
---
--- No column existence checks are performed.
--- Bounds are validated for the base offset only.
---
--- `GiNull` values are decoded back to Lua `nil`.
---
--- @param tbl_meta GiTblMeta
--- @param base integer
--- @param col_abi GiColAbi
--- @return number | string | boolean | nil
local gi_row_get_val_unsafe = function(tbl_meta, base, col_abi)
	local kind = col_abi[1] --[[@as GiColKind]]

	local max = tbl_meta.row_len * (tbl_meta.row_cnt - 1)

	if not IsInteger(base) or base < 0 or base > max then
		AssertError(base, '>0 and <=' .. max, 'Invalid base!')
	end

	if kind == GiColKind.CONST then
		return gi_decode_nil(col_abi[2])
	end

	if kind == GiColKind.SCALAR then
		local pos = base + col_abi[2]

		return gi_decode_nil(tbl_meta.row_data[pos])
	end

	if kind == GiColKind.SPARSE then
		local val = col_abi[3][base]

		if val ~= nil then
			return gi_decode_nil(val)
		end

		return gi_decode_nil(col_abi[2])
	end

	local slot = col_abi[2] --[[@as integer]]
	local bit_pos = col_abi[3] --[[@as integer]]
	local bit_len = col_abi[4] --[[@as integer]]

	if kind == GiColKind.BOOL then
		local wrd = tbl_meta.row_data[base + slot] --[[@as integer]]

		return BitFieldGetBit(wrd, bit_pos)
	end

	if kind == GiColKind.PINT then
		local wrd = tbl_meta.row_data[base + slot] --[[@as integer]]
		local int = BitFieldGetInt(wrd, bit_pos, bit_len)

		return int + col_abi[5]
	end

	if kind == GiColKind.DICT then
		local wrd = tbl_meta.row_data[base + slot] --[[@as integer]]
		local int = BitFieldGetInt(wrd, bit_pos, bit_len)
		local idx = int + 1

		return gi_decode_nil(col_abi[5][idx])
	end

	AssertError(kind, 'known column kind', 'This should not happen')
end

--- Read a column value from a row.
--- This is a strict accessor.
---
--- If the column does not exist in the table schema,
--- an error is raised.
---
--- @param tbl_meta GiTblMeta
--- @param row GiRowTmp
--- @param key string | number
--- @return number | string | boolean | nil
local gi_row_get_val_strict = function(tbl_meta, row, key)
	AssertIsString(key, 'Column key must be string!')

	local col_abi = tbl_meta.row_abi[key]

	if not col_abi then
		local tmp = 'Column "%s.%s" is not defined! '
				.. "To dynamically check if column is defined use form: `a, b = Gi[tbl][key](col_a, col_b)`"
		local mes = lua_string_format(tmp, tbl_meta.tbl_name, key)
		AssertError(key, 'existing column', mes)
	end

	return gi_row_get_val_unsafe(tbl_meta, row[1], col_abi)
end

--- Disallow row mutation.
--- Assigned as __newindex for row objects.
---
--- Rows are immutable views over static data.
--- Any attempt to modify a row raises an error.
---
local gi_row_set_val = function()
	lua_error(
		'Gi row is readonly! To make modifiable copy: Gi[tbl][key]()'
	)
end

--- @param tbl_meta GiTblMeta
--- @return boolean
local gi_col_exist = function(tbl_meta, col_name)
	if col_name ~= nil then
		AssertIsString(col_name, 'Column key must be string!')

		return tbl_meta.row_abi[col_name] ~= nil
	end

	return false
end

--- Creates row shallow copy if no arguments specified.
--- Checks for column existence per each argument specified.
---
--- This function behaves differently depending on whether column names
--- are provided via varargs:
---
--- - **No varargs:** returns a shallow copy of the row as a table
--- - **With varargs:** checks if specified column names exist in table in requested order
--- - _For up to 4 columns, values are returned directly (no table allocation)_
--- - _For more than 4 columns, values are collected and unpacked_
--- @param tbl_meta GiTblMeta
--- @param row GiRowTmp
--- @param ... nil | string
--- @return ... # Either row copy if no arguments specified, or column existence checks
local gi_row_call = function(tbl_meta, row, ...)
	local row_abi = tbl_meta.row_abi
	local args_size = ArgsSize(...)

	if args_size == 0 then
		local copy = {}

		for col_name, _ in lua_next, row_abi do
			copy[col_name] = gi_row_get_val_strict(tbl_meta, row, col_name)
		end

		return copy
	end

	if args_size <= 4 then
		local a, b, c, d = ...

		if args_size == 4 then
			return gi_col_exist(tbl_meta, a),
					gi_col_exist(tbl_meta, b),
					gi_col_exist(tbl_meta, c),
					gi_col_exist(tbl_meta, d)
		end

		if args_size == 3 then
			return gi_col_exist(tbl_meta, a),
					gi_col_exist(tbl_meta, b),
					gi_col_exist(tbl_meta, c)
		end

		if args_size == 2 then
			return gi_col_exist(tbl_meta, a), gi_col_exist(tbl_meta, b)
		end

		return gi_col_exist(tbl_meta, a)
	end

	local res = { n = args_size }

	for i = 1, args_size do
		local col_name = lua_select(i, ...)

		if col_name ~= nil then
			res[i] = gi_col_exist(tbl_meta, col_name)
		end
	end

	return ArgsUnpack(res)
end

--- Table length operator.
--- Returns number of rows in the table.
---
--- @param meta GiTblMeta
--- @return integer
local gi_tbl_get_len = function(meta, _)
	return meta.row_cnt
end

--- @param meta GiTblMeta # table metadata
--- @param _ any # unused
--- @param key string | number # row key
--- @return GiRow | nil # row object or nil if not found
local gi_tbl_get_row_unsafe = function(meta, _, key)
	if IsNumber(key) and meta.row_num_key then
		local row_num_idx = meta.row_num_idx --[[@as table<integer, integer> ]]

		if IsFunction(row_num_idx) then
			return row_num_idx(meta, key, nil)
		end

		return GI_ROW_IDX_MAPPED(meta, key, row_num_idx)
	end

	if IsString(key) and meta.row_str_key then
		return GI_ROW_IDX_MAPPED(meta, key, meta.row_str_idx --[[@as table]])
	end

	local exp = {}

	if meta.row_num_key then
		exp[#exp + 1] = 'number'
	end

	if meta.row_str_key then
		exp[#exp + 1] = 'string'
	end

	if #exp == 0 then
		AssertError(
			lua_type(key),
			'nothing',
			'Specified table "' .. meta.tbl_name .. '" does not support indexes!'
		)
	else
		local type = lua_type(key)
		local tmp = 'Specified table "%s" does not support indexing by %s!'
		local mes = lua_string_format(tmp, meta.tbl_name, type)
		AssertError(lua_type(key), lua_table_concat(exp, ' or '), mes)
	end
end

--- Table index operator.
--- Retrieves a row by primary key.
---
--- Supported keys:
--- - numeric key if numeric primary key exists
--- - string key if string primary key exists
---
--- If the key type or indexing mode is unsupported,
--- an error is raised.
---
--- @param meta GiTblMeta # table metadata
--- @param _ any # unused
--- @param key string | number # row key
--- @return GiRow | nil # row object or nil if not found
local gi_tbl_get_row_strict = function(meta, _, key)
	if meta.row_num_key == '_ROWID_' and IsNumber(key) then
		local type = lua_type(key)
		local exp = meta.row_str_key and 'string' or 'nothing'
		local tmp = 'Specified table "%s" does not support indexing by %s!'
		local mes = lua_string_format(tmp, meta.tbl_name, type)
		AssertError(lua_type(key), exp, mes)
	end

	return gi_tbl_get_row_unsafe(meta, _, key)
end

--- Disallow table mutation.
--- Assigned as __newindex for table objects.
---
--- Tables are immutable.
---
--- @param meta GiTblMeta
local gi_tbl_set_row = function(meta, _)
	lua_error('Table "' .. meta.tbl_name .. '" is read only!')
end

--- Build a row predicate function from a filter table.
---
--- The filter table maps column names to expected values.
--- Supported filter value types are number, string and boolean.
---
--- Validation is performed eagerly:
--- - column existence
--- - supported value types
---
--- Returned predicate operates on decoded row values.
--- @param meta GiTblMeta
--- @param cond table<string, GiType> # Filter conditions
local gi_cond_validate = function(meta, cond)
	local tbl_name = meta.tbl_name
	local row_abi = meta.row_abi

	for key, val in lua_next, cond do
		local col_abi = row_abi[key]

		if not col_abi then
			local tmp = 'Column "%s.%s" does not exist, but filter specifies it'
			lua_error(lua_string_format(tmp, tbl_name, key))
		end

		local val_type = lua_type(val)

		if val_type ~= 'number'
				and val_type ~= 'string'
				and val_type ~= 'boolean'
		then
			local tmp = 'Column "%s.%s" can not be filtered by type %s'
			local mes = lua_string_format(tmp, tbl_name, key, val_type)
			AssertError(val_type, 'number or string or boolean', mes)
		end
	end
end

--- @param meta GiTblMeta
--- @param cond table<string, GiType> # Filter conditions
--- @param base integer
local gi_cond_matches = function(meta, cond, base)
	for key, exp in lua_next, cond do
		local val = gi_row_get_val_unsafe(meta, base, meta.row_abi[key])

		if IsBoolean(val) and IsNumber(exp) then
			if exp ~= 1 and exp ~= 0 then
				local tmp = 'Non-ambigous value %s specified for column "%s.%s" filter'
				local mes = lua_string_format(tmp, exp, meta.tbl_name, key)
				AssertError(exp --[[@as any]], '1 or 0 or boolean', mes)
			end

			if val ~= (exp == 1) then
				return false
			end
		elseif val ~= exp then
			return false
		end
	end

	return true
end

--- Table call operator.
--- Produces an iterator over rows.
---
--- Supported modes:
--- - nil: iterate over all rows
--- - string: SQL WHERE clause (delegated to SQLite)
--- - table: in-memory filter using column equality
---
--- Iteration always yields immutable row objects.
---
--- @param meta GiTblMeta
--- @param _ any # unused
--- @param cond nil | string | table<string, GiType> # Filter
--- @param ... any # SQL bind parameters
--- @return fun(): GiRow | nil # Iterator function
local gi_tbl_iterate = function(meta, _, cond, ...)
	local row_cnt = meta.row_cnt
	local row_len = meta.row_len
	local row_memo = meta.row_memo

	if cond == nil then
		local idx = 0

		return function()
			if idx >= row_cnt then
				return nil
			end

			local base = idx * row_len
			idx = idx + 1

			return row_memo[base]
		end
	end

	if IsString(cond) then
		local row_key = meta.row_num_key or meta.row_str_key
		local tmp = "SELECT `%s` as `1` FROM `%s` WHERE %s"
		local sql = lua_string_format(tmp, row_key, meta.tbl_name, cond)
		local arg_size = ArgsSize(...)
		local nxt = nil

		if arg_size == 0 then
			nxt = DB.Query(sql)
		else
			local args = { n = arg_size }

			for i = 1, arg_size do
				local arg = lua_select(i, ...)

				if IsBoolean(arg) then
					arg = arg and 1 or 0
				end

				args[i] = arg
			end

			nxt = DB.Query(sql, ArgsUnpack(args))
		end

		return function()
			local ent = nxt()

			if ent == nil then
				return nil
			end

			return gi_tbl_get_row_unsafe(meta, nil, ent[1])
		end
	end

	if IsTable(cond) then
		--[[@cast cond table]]

		gi_cond_validate(meta, cond)

		local row_num_key = meta.row_num_key
		local row_str_key = meta.row_str_key

		local key = nil

		if row_num_key ~= nil and cond[row_num_key] ~= nil then
			key = cond[row_num_key]
		elseif row_str_key ~= nil and cond[row_str_key] ~= nil then
			key = cond[row_str_key]
		end

		if key ~= nil then
			local row = gi_tbl_get_row_unsafe(meta, nil, key --[[@as string | number]])
			local done = false

			if row == nil then return Void end

			return function()
				if done then return nil end
				done = true

				if gi_cond_matches(meta, cond, row[1]) then
					return row
				end
			end
		end

		local idx = 0

		return function()
			while idx < row_cnt do
				local base = idx * row_len
				idx = idx + 1

				if gi_cond_matches(meta, cond, base) then
					return row_memo[base]
				end
			end
		end
	end


	AssertError(
		lua_type(cond),
		'nil or table or string',
		'Unsupported filter specified!'
	--- @diagnostic disable-next-line: missing-return
	)
end

--- Finalize table metadata and public table object.
--- Installs all metamethods.
--- @return nil
function GiAsmImpl:Finalize()
	local tbl_meta = {
		tbl_name = self.tbl_name,
		row_abi = self.row_abi,
		row_cnt = self.row_cnt,
		row_len = self.row_len,
		row_data = self.row_data,
		row_num_key = self.row_num_key,
		row_num_idx = self.row_num_idx,
		row_str_key = self.row_str_key,
		row_str_idx = self.row_str_idx,
		row_meta = {},
		row_memo = {},
	}

	lua_setmetatable(tbl_meta.row_memo, {
		__mode = 'v',
		__index = function(self, base)
			local row = lua_setmetatable({ base }, tbl_meta.row_meta)
			self[base] = row
			return row
		end,
	})

	tbl_meta.row_meta.__call = Bind(gi_row_call, tbl_meta)
	tbl_meta.row_meta.__index = Bind(gi_row_get_val_strict, tbl_meta)
	tbl_meta.row_meta.__newindex = gi_row_set_val

	tbl_meta.__len = Bind(gi_tbl_get_len, tbl_meta)
	tbl_meta.__call = Bind(gi_tbl_iterate, tbl_meta)
	tbl_meta.__index = Bind(gi_tbl_get_row_strict, tbl_meta)
	tbl_meta.__newindex = Bind(gi_tbl_set_row, tbl_meta)

	self.tbl_meta = tbl_meta
	self.tbl_inst = lua_setmetatable({}, tbl_meta)
end

CPK.DB.GiAsm = GiAsm
