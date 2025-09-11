local lua_next = next
local lua_type = type
local lua_setmetatable = setmetatable
local lua_getmetatable = getmetatable

local IsNil = CPK.Type.IsNil
local IsTable = CPK.Type.IsTable
local IsBoolean = CPK.Type.IsBoolean
local AssertError = CPK.Assert.Error
local AssertIsTable = CPK.Assert.IsTable

--- Creates a shallow copy of a table. Iterates over all key–value pairs
--- in `tbl` and assigns them into a new table.
--- Optionally applies a metatable to the copy:
--- - If `meta` is a table, it will be used as the new metatable.
--- - If `meta` is `true`, the metatable of the original table will be copied.
--- - If `meta` is `nil` or `false`, no metatable will be applied.
---
--- Note: This function performs a **shallow copy**.
--- Nested tables are copied by reference, not duplicated.
---
--- ```lua
--- -- Example 1: simple shallow copy
--- local TableCopy = CPK.Table.Copy
--- local orig = { a = 1, b = 2 }
--- local copy = TableCopy(orig)
--- local same = orig == copy -- false
--- print(copy.a) -- 1
--- print(copy.b) -- 2
---
--- -- Example 2: copy with explicit metatable
--- local mt = { __index = { c = 3 } }
--- local orig = { a = 1, b = 2 }
--- local copy = TableCopy(orig, mt)
--- print(copy.c) -- 3
---
--- -- Example 3: copy and inherit original metatable
--- local mt = { __index = { d = 4 } }
--- local orig = setmetatable({ a = 1 }, mt)
--- local copy = TableCopy(orig, true)
--- print(copy.d) -- 4
--- ```
---
--- @generic T : table
--- @param tbl T # The source table to copy.
--- @param meta? nil | boolean | table # How the metatable is applied.
--- @return T # A shallow copy of `tbl`.
--- @nodiscard
local function TableCopy(tbl, meta)
	AssertIsTable(tbl)

	local copy = {}

	for key, val in lua_next, tbl, nil do
		copy[key] = val
	end

	if IsNil(meta) then
		return copy
	end

	if IsTable(meta) then
		return lua_setmetatable(copy, meta --[[@as metatable]])
	end

	if IsBoolean(meta) then
		return meta
				and lua_setmetatable(copy, lua_getmetatable(tbl))
				or copy
	end

	return AssertError(lua_type(meta), 'boolean, table, nil')
end

CPK.Table.Copy = TableCopy
