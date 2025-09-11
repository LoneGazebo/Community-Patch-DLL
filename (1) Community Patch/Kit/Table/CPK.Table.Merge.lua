local lua_next = next

local ArgsEach = CPK.Args.Each
local ArgsEmpty = CPK.Args.Empty

local AssertError = CPK.Assert.Error
local AssertIsTable = CPK.Assert.IsTable

--- Merges multiple tables into a new table
--- by shallow copying key-value pairs from left to right.
---
--- If multiple tables contain the same key, the value from the rightmost table is used.
---
--- ```lua
--- -- Example
--- local TableMerge = CPK.Table.Merge
---
--- local tbl1 = { a = 1, b = 2 }
--- local tbl2 = { b = 10, c = 20 }
--- local tbl3 = { a = 100, d = 30 }
---
--- local merged = TableMerge(tbl1, tbl2, tbl3)
--- -- merged: { a = 100, b = 10, c = 20, d = 30 }
--- ```
---
--- @param ... table
--- @return table
--- @nodiscard
local function TableMerge(...)
	if ArgsEmpty(...) then
		AssertError(
			'0 arguments',
			'at least 1 argument of type table',
			'Cannot merge tables because none were specified'
		)
	end

	--- @type table
	local res = {}
	local idx = 0

	ArgsEach(function(tbl)
		idx = idx + 1

		AssertIsTable(tbl, 'Argument at #' .. idx .. ' is not table')

		for key, val in lua_next, tbl, nil do
			res[key] = val
		end
	end, ...)

	return res
end

CPK.Table.Merge = TableMerge
