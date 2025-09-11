local lua_next = next
local lua_table_insert = table.insert

--- Returns an array of keys from the given table.
--- The order of keys is not guaranteed!
---
--- @generic K
--- @param tbl table<K, any> # Input table
--- @return K[] # List of keys present in the table
local function TableKeys(tbl)
	local res = {}

	for key in lua_next, tbl do
		lua_table_insert(res, key)
	end

	return res
end

CPK.Table.Keys = TableKeys
