local lua_math_huge = math.huge

local IsNumber = CPK.Type.IsNumber

--- Checks if the type of the specified value is `integer`.
--- @param val any # The value to check.
--- @return boolean # `true` if the value is a integer, `false` otherwise.
--- @nodiscard
local function IsInteger(val)
	return IsNumber(val)
			and val % 1 == 0
			and val ~= lua_math_huge
			and val ~= -lua_math_huge
end

CPK.Type.IsInteger = IsInteger
