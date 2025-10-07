local locale = Locale

-- Formats a number with comma as thousands separator and with exactly two decimal places
--- Example:
--- ```lua
--- FormatDecimal(1234)  --> 1,234.00
--- FormatDecimal(0.7)   --> 0.70
--- ```
---
--- @param f number # Number
--- @return string # Formatted output
local function FormatDecimal(f)
	return locale.ToNumber(f, "#,##0.00")
end

CPK.Text.FormatDecimal = FormatDecimal
