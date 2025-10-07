local lua_math_floor = math.floor
local locale = Locale

-- Formats a number with comma as thousands separator. Decimal numbers are rounded down to the nearest integer
--- Example:
--- ```lua
--- FormatInteger(1234)  --> 1,234
--- FormatInteger(0.7)   --> 0
--- ```
---
--- @param f number # Number
--- @return string # Formatted output
local function FormatInteger(f)
	return locale.ToNumber(lua_math_floor(f), "#,##0")
end

CPK.Text.FormatInteger = FormatInteger
