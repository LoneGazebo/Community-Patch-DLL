local lua_string_rep = string.rep

local AssertIsString = CPK.Assert.IsString
local AssertIsNumber = CPK.Assert.IsNumber

--- @param str string
--- @param len number
--- @param pad? string
function CPK.String.PadStart(str, len, pad)
	AssertIsString(str)
	AssertIsNumber(len)

	if #str >= len then return str end

	if pad == nil then pad = ' ' end

	return lua_string_rep(pad, len - #str) .. str
end
