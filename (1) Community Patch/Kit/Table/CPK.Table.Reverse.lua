local lua_table_insert = table.insert

--- Returns a new table containing the elements of the input table in reverse order.
--- The original table is not modified.
---
--- @generic T
--- @param tbl T[] # Input table
--- @return T[] # New table with elements in reversed order
local function TableReverse(tbl)
	local res = {}

	for i = #tbl, 1, -1 do
		lua_table_insert(res, tbl[i])
	end

	return res
end

CPK.Table.Reverse = TableReverse
