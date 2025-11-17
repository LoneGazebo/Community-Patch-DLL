local lua_string_format = string.format
local lua_string_byte = string.byte

local AssertIsString = CPK.Assert.IsString

local map = {
	['\\'] = '\\\\',
	['%'] = '%%',
	['\n'] = '\\n',
	['\t'] = '\\t',
	['\v'] = '\\v',
	['\r'] = '\\r',
	['\f'] = '\\f',
	["'"] = "\\'",
	['"'] = '\\"',
}

--- Returns a string with special characters escaped.
--- This function escapes special characters such as newlines, tabs, and quotes,
--- as well as unprintable and extended ASCII characters.
--- @param str string # The input string to escape.
--- @return string # Escaped string.
--- @nodiscard
local function StringEscape(str)
	AssertIsString(str)

	if str == '' then return str end

	local res = str
			:gsub('[\\%%\n\t\v\r\f\'"]', map)
			:gsub('[\000-\031\128-\255]', function(char)
				return lua_string_format('\\%03d', lua_string_byte(char))
			end)

	return res
end

CPK.String.Escape = StringEscape
