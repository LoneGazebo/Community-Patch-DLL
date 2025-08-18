local Curry = CPK.FP.Curry

--- Retrieves the value associated with the specified key in the given table.
--- This function is curried and requires two arguments: `key` and `tbl`.
---
--- ```lua
--- -- Example
--- local Prop = CPK.Table.Prop
--- local myTable = { a = 1, b = 2, c = 3 }
--- print(Prop('b', myTable))  -- Output: 2
---
--- -- Example using currying
--- local GetPropB = Prop('b')
--- print(GetPropB(myTable))  -- Output: 2
--- ```
--- @overload fun(key: any, tbl: table): any
--- @overload fun(key: any): (fun(tbl: table): any)
--- @nodiscard
local TableProp = Curry(2, function(key, tbl)
	return tbl[key]
end)

CPK.Table.Prop = TableProp
