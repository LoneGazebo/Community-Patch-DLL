local _lua_select = select

local function ArgsSize(...)
	return _lua_select('#', ...)
end

CPK.Args.Size = ArgsSize
