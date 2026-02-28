local lua_math_log = math.log
local lua_math_ceil = math.ceil

local AssertError = CPK.Assert.Error
local AssertIsInteger = CPK.Assert.IsInteger

local LOG2 = lua_math_log(2)


--- Compute the minimum number of bits required to encode `n` distinct values.
--- @param n integer # Number of representable states (n >= 1)
--- @return integer # Required bit width (>= 1)
local function BitsForCapacity(n)
	AssertIsInteger(n)

	if n < 1 then
		AssertError(n, '>=1')
	end

	if n == 1 then
		return 1
	end

	return lua_math_ceil(lua_math_log(n) / LOG2)
end

--- Compute the minimum number of bits required to encode an inclusive integer range.
--- @param min integer
--- @param max integer
--- @return integer # Required bit width (>= 1)
local function BitsForRange(min, max)
	AssertIsInteger(min)
	AssertIsInteger(max)

	if min > max then
		AssertError(min .. ' > ' .. max, 'min <= max')
	end

	return BitsForCapacity((max - min) + 1)
end

CPK.Bit.Math = {
	BitsForCapacity = BitsForCapacity,
	BitsForRange = BitsForRange,
}
