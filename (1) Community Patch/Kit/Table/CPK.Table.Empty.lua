local lua_next = next

local AssertIsTable = CPK.Assert.IsTable

--- Checks whether a table is empty (contains no key–value pairs).
---
--- Iterates once over the table to determine if any element exists.
--- Returns immediately on the first element found.
---
--- ```lua
--- -- Examples
--- local TableEmpty = CPK.Table.Empty
---
--- -- empty table
--- local t1 = {}
--- print(TableEmpty(t1)) -- true
---
--- -- non-empty table
--- local t2 = { a = 1 }
--- print(TableEmpty(t2)) -- false
--- ```
---
--- @param tbl table # The table to check if it is empty.
--- @return boolean # `true` if the table is empty, `false` otherwise.
--- @nodiscard
local function TableEmpty(tbl)
	AssertIsTable(tbl)

	-- Iterate over the table to check if it has any elements
	for _, _ in lua_next, tbl, nil do
		return false
	end

	return true
end

CPK.Table.Empty = TableEmpty
