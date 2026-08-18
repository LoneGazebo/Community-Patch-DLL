local lua_math_floor = math.floor

--- Rounds a number to the nearest integer.
---
--- Halfway values round up (towards positive infinity).
---
--- ```lua
--- -- Examples
--- local MathRound = CPK.Math.Round
---
--- print(MathRound(1.4))  -- 1
--- print(MathRound(1.5))  -- 2
--- print(MathRound(-1.5)) -- -1
--- ```
---
--- @param value number # The number to round.
--- @return integer # Nearest integer.
--- @nodiscard
local function MathRound(value)
	return lua_math_floor(value + 0.5)
end

CPK.Math.Round = MathRound
