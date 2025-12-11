local lua_pcall = pcall
local lua_setmetatable = setmetatable
local civ_db_create_query = DB.CreateQuery

--------------------------------------------------------------------------------

local GiAsm = CPK.DB.GiAsm

local ArgsEach = CPK.Args.Each
local ArgsUnpack = CPK.Args.Unpack

local AssertError = CPK.Assert.Error
local AssertIsString = CPK.Assert.IsString

--------------------------------------------------------------------------------

local qry = civ_db_create_query([[
	SELECT NULL FROM `sqlite_master`
		WHERE type = 'table' AND name = ? AND name NOT LIKE 'sqlite%'
]])

--- Checks if GameInfo table exists
--- @param tbl_name string
--- @return boolean
local function gi_tbl_exist(tbl_name)
	AssertIsString(tbl_name)

	local row = qry(tbl_name)()

	return row ~= nil
end

--------------------------------------------------------------------------------

--- Gi is a lazy, cached registry of database-backed GameInfo tables.
---
--- It provides two access patterns:
---
---  - Property-style access:
---    - `Gi.Units` - Returns a single GiTbl instance for the given table name.
---       The table is assembled on first access and cached globally.
---       Throws if specified table (e.g. `Units`) does not exist
---  - Call-style access:
---    - `Gi("Units", "Buildings", "Technologies")` -
---      Returns multiple `GiTbl` instances as multiple return values,
---      in the same order as requested. Missing or non-string arguments
---      are ignored.
---
--- The underlying database is treated as static. Once a table is
--- assembled, it is never rebuilt or invalidated for the lifetime
--- of the process.
---
--- Gi performs existence checks against sqlite_master and throws
--- a descriptive error if a requested table does not exist
--- **(only for property-style access)**
---
--- @class GiRegistry
--- @field [string] GiTbl
--- @overload fun(...: string): GiTbl?, GiTbl?, GiTbl?, GiTbl?, ...
local GiRegistry = {}

lua_setmetatable(GiRegistry --[[@as table]], {
	__index = function(_, tbl_name)
		if not gi_tbl_exist(tbl_name) then
			AssertError(
				tbl_name,
				'existing table name',
				'Table "' .. tbl_name .. '" not found!'
			)
		end

		local tbl = GiAsm.Assemble(tbl_name)

		GiRegistry[tbl_name] = tbl

		return tbl
	end,
	__call = function(_, ...)
		local tbls = {}
		local n = 0

		ArgsEach(function(tbl_name)
			n = n + 1

			if tbl_name ~= nil then
				AssertIsString(tbl_name, 'Table name must be string!')

				local success, tbl = lua_pcall(function()
					return GiRegistry[tbl_name]
				end)

				if success then
					tbls[n] = tbl
				end
			end
		end, ...)

		tbls.n = n

		return ArgsUnpack(tbls)
	end
})

CPK.DB.Gi = GiRegistry
