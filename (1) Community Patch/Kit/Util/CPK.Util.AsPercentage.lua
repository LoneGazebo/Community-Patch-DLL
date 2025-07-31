local _lua_math_floor = math.floor

--- Converts a ratio (e.g., 0.25) into a display-friendly percentage (e.g. 25).
--- Rounds down to whole number if no decimal part is needed,
--- otherwise keeps one decimal place.
---
--- Example:
--- ```lua
--- AsPercent(0.3354)  --> 33.5
--- AsPercent(0.25)    --> 25
--- AsPercent(0.9999)  --> 99.9
--- AsPercent(125.789) --> 12578.9
--- ```
---
--- @param ratio number # Ratio (e.g., 0.25 = 25%)
--- @return number # Integer or 1-decimal-place float
local function AsPercentage(ratio)
	if _lua_math_floor(ratio * 1000) / 10 % 1 == 0 then
		return _lua_math_floor(ratio * 100)
	end

	return _lua_math_floor(ratio * 1000) / 10
end

CPK.Util.AsPercentage = AsPercentage
