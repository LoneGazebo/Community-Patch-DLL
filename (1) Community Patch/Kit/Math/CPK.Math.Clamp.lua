--- Clamps a number to the inclusive range `[min, max]`.
---
--- ```lua
--- -- Examples
--- local MathClamp = CPK.Math.Clamp
---
--- print(MathClamp(5, 0, 10))   -- 5
--- print(MathClamp(-3, 0, 10))  -- 0
--- print(MathClamp(42, 0, 10))  -- 10
--- ```
---
--- @param val number # The number to clamp.
--- @param min number # Lower bound of the range.
--- @param max number # Upper bound of the range.
--- @return number # `val` limited to the range `[min, max]`.
--- @nodiscard
local function MathClamp(val, min, max)
	if val < min then return min end
	if val > max then return max end

	return val
end

CPK.Math.Clamp = MathClamp
