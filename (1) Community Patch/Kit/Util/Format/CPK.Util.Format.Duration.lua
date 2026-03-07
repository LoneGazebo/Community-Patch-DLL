local lua_math_abs = math.abs
local lua_string_format = string.format

local AssertIsNumber = CPK.Assert.IsNumber

local SEC_PER_MS = 1e-3
local SEC_PER_US = 1e-6

local MS_PER_SEC = 1e3
local US_PER_SEC = 1e6
local NS_PER_SEC = 1e9

--- @param seconds number
--- @param digits? number
--- @return string
local function FormatDuration(seconds, digits)
	if digits == nil then
		digits = 2
	end

	AssertIsNumber(seconds)
	AssertIsNumber(digits)

	if seconds == 0 then
		return '0'
	end

	local abs = lua_math_abs(seconds)
	local val = nil
	local unit = nil

	if abs >= 1 then
		val, unit = seconds, 's'
	elseif abs >= SEC_PER_MS then
		val, unit = seconds * MS_PER_SEC, 'ms'
	elseif abs >= SEC_PER_US then
		val, unit = seconds * US_PER_SEC, 'us'
	else
		val, unit = seconds * NS_PER_SEC, 'ns'
	end

	return lua_string_format('%.' .. digits .. 'f %s', val, unit)
end

CPK.Util.Format.Duration = FormatDuration
