local lua_select = select

--- Calls the specified function on each variadic argument.
--- @generic Arg
--- @param fn fun(arg: Arg): any # Function to apply to each argument
--- @param ... Arg # Zero or more arguments
--- @return Arg ... # Returns the original arguments
local function ArgsEach(fn, ...)
	local len = lua_select('#', ...)

	for i = 1, len do
		-- Very crucial to not do `fn(select(i, ...))`
		-- because select returns multiple values
		local tmp = lua_select(i, ...)
		fn(tmp)
	end

	return ...
end

CPK.Args.Each = ArgsEach
