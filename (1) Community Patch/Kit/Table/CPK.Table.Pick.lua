local Curry = CPK.FP.Curry
local AssertIsTable = CPK.Assert.IsTable

--- Creates a new table containing only the specified keys from the original table.
--- Iterates over the `keys` array and copies the corresponding values from `tbl`.
---
--- Note: This is a shallow copy; nested tables are copied by reference.
---
--- ```lua
--- local Pick = CPK.Table.Pick
--- local orig = { a = 1, b = 2, c = 3 }
---
--- -- Example 1: pick a single key
--- local t1 = Pick({ "a" }, orig)
--- print(t1.a) -- 1
--- print(t1.b) -- nil
---
--- -- Example 2: pick multiple keys
--- local t2 = Pick({ "b", "c" }, orig)
--- print(t2.b) -- 2
--- print(t2.c) -- 3
--- print(t2.a) -- nil
--- ```
---
--- @overload fun(keys: any[], tbl: table): table
--- @overload fun(keys: any[]): (fun(tbl: table): table)
--- @nodiscard
local TablePick = Curry(2, function(keys, tbl)
	AssertIsTable(keys)
	AssertIsTable(tbl)

	local len = #keys
	local res = {}

	for i = 1, len do
		local key = keys[i]

		res[key] = tbl[key]
	end

	return res
end)

CPK.Table.Pick = TablePick
