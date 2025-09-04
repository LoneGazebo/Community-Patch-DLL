---@diagnostic disable-next-line: deprecated
local lua_unpack = table.unpack or unpack

--- Unpacks specified table using it's property `n` or length
--- @generic T
--- @param args { n?: integer, [integer]: T }
--- @return T ...
--- @nodiscard
--- @see unpack
--- @see table.unpack
local function ArgsUnpack(args)
	return lua_unpack(args, 1, args.n or #args)
end

CPK.Args.Unpack = ArgsUnpack
