--- Linearly interpolates between two numbers.
---
--- Returns `from` when `fact` is 0 and `to` when `t` is 1.
--- Values of `fact` outside `[0, 1]` extrapolate beyond the range.
---
--- ```lua
--- -- Examples
--- local MathLerp = CPK.Math.Lerp
---
--- print(MathLerp(0, 10, 0.5)) -- 5
--- print(MathLerp(0, 10, 0))   -- 0
--- print(MathLerp(0, 10, 1))   -- 10
--- print(MathLerp(0, 10, 1.5)) -- 15
--- ```
---
--- @param from number # Start value (returned when `t` is 0).
--- @param to number # End value (returned when `t` is 1).
--- @param fact number # Interpolation factor.
--- @return number # Interpolated value.
--- @nodiscard
local function MathLerp(from, to, fact)
	return from + (to - from) * fact
end

CPK.Math.Lerp = MathLerp
