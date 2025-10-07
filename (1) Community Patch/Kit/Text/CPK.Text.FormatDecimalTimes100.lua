local locale = Locale

-- Divides a number by 100 and formats it with comma as thousands separator and with exactly two decimal places
--- Example:
--- ```lua
--- FormatDecimalTimes100(123400)  --> 1,234.00
--- FormatDecimalTimes100(100)   --> 1.00
--- FormatDecimalTimes100(75)   --> 0.75
--- ```
---
--- @param f number # Number
--- @return string # Formatted output
local function FormatDecimalTimes100(f)
	return locale.ToNumber(f / 100, "#,##0.00")
end

CPK.Text.FormatDecimalTimes100 = FormatDecimalTimes100
