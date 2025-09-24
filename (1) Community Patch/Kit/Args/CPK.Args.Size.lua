local lua_select = select

local function ArgsSize(...)
	return lua_select('#', ...)
end

CPK.Args.Size = ArgsSize
