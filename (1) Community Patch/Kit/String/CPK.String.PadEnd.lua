local lua_string_rep = string.rep

--- @param str string
--- @param len number
--- @param pad? string
function CPK.String.PadEnd(str, len, pad)
	if #str >= len then return str end

	if pad == nil then pad = ' ' end

	return str .. lua_string_rep(pad, len - #str)
end
