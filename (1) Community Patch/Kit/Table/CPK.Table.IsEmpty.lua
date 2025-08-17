local _lua_next = next

local AssertIsTable = CPK.Assert.IsTable

--- Checks whether a table is empty (contains no key–value pairs).
---
--- Iterates once over the table to determine if any element exists.
--- Returns immediately on the first element found.
---
--- ```lua
--- -- Examples
--- local IsEmpty = CPK.Table.IsEmpty
---
--- -- empty table
--- local t1 = {}
--- print(IsEmpty(t1)) -- true
---
--- -- non-empty table
--- local t2 = { a = 1 }
--- print(IsEmpty(t2)) -- false
--- ```
---
--- @param tbl table # The table to check if it is empty.
--- @return boolean # `true` if the table is empty, `false` otherwise.
--- @nodiscard
local function IsEmpty(tbl)
	AssertIsTable(tbl)

	-- Iterate over the table to check if it has any elements
	for _, _ in _lua_next, tbl, nil do
		return false
	end

	return true
end

CPK.Table.IsEmpty = IsEmpty
