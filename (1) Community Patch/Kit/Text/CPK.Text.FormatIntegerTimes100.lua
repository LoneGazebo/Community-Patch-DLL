local lua_math_floor = math.floor
local locale = Locale

-- Divides a number by 100 and formats it with comma as thousands separator. Decimal numbers are rounded down to the nearest integer
--- Example:
--- ```lua
--- FormatIntegerTimes100(123400)  --> 1,234
--- FormatIntegerTimes100(100)   --> 1
--- FormatIntegerTimes100(75)   --> 0
--- ```
---
--- @param f number # Number
--- @return string # Formatted output
local function FormatIntegerTimes100(f)
	return locale.ToNumber(lua_math_floor(f / 100), "#,##0")
end

CPK.Text.FormatIntegerTimes100 = FormatIntegerTimes100
