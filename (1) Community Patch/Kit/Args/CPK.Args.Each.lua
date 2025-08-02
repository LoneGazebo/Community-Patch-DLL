local _lua_select = select

--- Calls specified function on each variadic argument.
--- @generic Arg
--- @param fn fun(Arg): any # Function to apply
--- @param ... Arg # Arguments to pass to `fn`
local function ArgsEach(fn, ...)
	local len = _lua_select('#', ...)

	if len == 0 then
		return
	end

	local tbl = { ... }

	for i = 1, len do
		fn(tbl[i])
	end
end

CPK.Args.Each = ArgsEach
