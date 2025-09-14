--- Converts an array-style table of values into a "hash set" for `O(1)` lookups.
--- Each value in the input table 
--- becomes a key in the result table with value `true`.
--- <br>
--- ```lua
--- -- Example
--- local reserved = CPK.Table.Hashify({ "found", "test", "aaa" })
--- print(reserved["found"])     --> true
--- print(reserved["notfound"])  --> nil
--- ```
--- @generic T
--- @param tbl T[] # Input table of values
--- @return table<T, true> # A lookup table with all values mapped to `true`.
local function TableHashify(tbl)
	local res = {}
	local len = #tbl

	for i = 1, len do
		local val = tbl[i]

		if val ~= nil then
			res[val] = true
		end
	end

	return res
end

CPK.Table.Hashify = TableHashify
