local lua_select = select

--- Calls the specified function on each variadic argument.
--- Uses two strategies internally:
--- - For <= 8 arguments, iterates with `select` (O(n^2), negligible for small n).
--- - For > 8 arguments, packs into a table first (O(n), slightly more memory).
---
--- @generic Arg
--- @param fn fun(arg: Arg): any # Function to apply to each argument
--- @param ... Arg # Zero or more arguments
--- @return Arg ... # Returns the original arguments
local function ArgsEach(fn, ...)
	local len = lua_select('#', ...)

	if len <= 8 then
		local tmp = nil
		-- O(n^2), acceptable for small length
		for i = 1, len do
			-- Very crucial to not do `fn(select(i, ...))`
			-- because select returns multiple values
			tmp = lua_select(i, ...)
			fn(tmp)
		end
	else
		local tmp = { ... }
		-- O(n), faster but allocates a temporary table
		for i = 1, len do
			fn(tmp[i])
		end
	end

	return ...
end

CPK.Args.Each = ArgsEach
