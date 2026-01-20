local lua_string_rep = string.rep

--- @param str string
--- @param len number
--- @param pad? string
function CPK.String.PadStart(str, len, pad)
	if #str >= len then return str end
	if pad == nil then pad = ' ' end

	return lua_string_rep(pad, len - #str) .. str
end
