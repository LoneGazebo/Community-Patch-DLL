local lua_math_abs = math.abs
local lua_string_format = string.format

local AssertError = CPK.Assert.Error
local AssertIsNumber = CPK.Assert.IsNumber

local ONE_KiB = 1024
local ONE_MiB = ONE_KiB ^ 2
local ONE_GiB = ONE_KiB ^ 3

--- @param bytes number
--- @param digits? number
local function FormatByteSize(bytes, digits)
	if digits == nil then
		digits = 2
	end

	AssertIsNumber(bytes)
	AssertIsNumber(digits)

	if digits < 0 then
		AssertError(digits, 'nil or >=0')
	end

	local abs = lua_math_abs(bytes)
	local tmp = nil
	local val = nil

	if abs < ONE_KiB then
		tmp, val = '%d B', bytes
	elseif abs < ONE_MiB then
		tmp, val = ('%.' .. digits .. 'f KiB'), (bytes / ONE_KiB)
	elseif abs < ONE_GiB then
		tmp, val = ('%.' .. digits .. 'f MiB'), (bytes / ONE_MiB)
	else
		tmp, val = ('%.' .. digits .. 'f GiB'), (bytes / ONE_GiB)
	end

	return lua_string_format(tmp, val)
end

CPK.Util.Format.ByteSize = FormatByteSize
