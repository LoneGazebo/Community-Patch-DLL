local lua_math_floor = math.floor

--- Converts a ratio (e.g., 0.25) into a display-friendly percentage (e.g. 25).
--- Rounds down to whole number if no decimal part is needed,
--- otherwise keeps one decimal place.
---
--- Example:
--- ```lua
--- AsPercentage(0.3354)  --> 33.5
--- AsPercentage(0.25)    --> 25
--- AsPercentage(0.9999)  --> 99.9
--- AsPercentage(125.789) --> 12578.9
--- ```
---
--- @param ratio number # Ratio (e.g., 0.25 = 25%)
--- @return number # Integer or 1-decimal-place float
local function AsPercentage(ratio)
	local num = lua_math_floor(ratio * 1000) / 10

	return num % 1 == 0
			and lua_math_floor(num)
			or num
end

CPK.Misc.AsPercentage = AsPercentage
