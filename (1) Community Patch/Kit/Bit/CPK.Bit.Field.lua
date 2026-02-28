local lua_tostring = tostring

local AssertError = CPK.Assert.Error

local bor = CPK.Bit.Polyfill.bor
local bnot = CPK.Bit.Polyfill.bnot
local band = CPK.Bit.Polyfill.band
local lshift = CPK.Bit.Polyfill.lshift
local rshift = CPK.Bit.Polyfill.rshift

local BitField = {}

--- Set or clear a single-bit boolean flag inside a packed word.
---
--- The bit at position `pos` is set when `bit` is `true` or `1`,
--- and cleared when `bit` is `false` or `0`.
---
--- @param wrd integer # Original word
--- @param pos integer # Bit position (0-based, 0 = LSB)
--- @param bit boolean|0|1 # Value to store
--- @return integer # Updated word
local function SetBit(wrd, pos, bit)
	local mask = lshift(1, pos)

	if bit == 1 or bit == true then
		return bor(wrd, mask)
	elseif bit == 0 or bit == false then
		return band(wrd, bnot(mask))
	else
		AssertError(lua_tostring(bit), 'boolean | 0 | 1')
		--- @diagnostic disable-next-line: missing-return
	end
end

--- Read a single-bit boolean flag from a packed word.
---
--- @param wrd integer # Word to read from
--- @param pos integer # Bit position (0-based, 0 = LSB)
--- @return boolean # True if the bit is set, false otherwise
local function GetBit(wrd, pos)
	return band(wrd, lshift(1, pos)) ~= 0
end

--- Insert an unsigned integer value into a bit-field inside a packed word.
---
--- The value is masked to `len` bits and written starting at bit position `pos`.
--- Any existing bits in the target range are cleared before insertion.
---
--- @param wrd integer # Original word
--- @param pos integer # Starting bit position (0-based, 0 = LSB)
--- @param len integer # Field width in bits
--- @param int integer # Unsigned integer value to store
--- @return integer # Updated word
local function SetInt(wrd, pos, len, int)
	local field_mask = lshift(1, len) - 1
	local shift_mask = lshift(field_mask, pos)

	wrd = band(wrd, bnot(shift_mask))

	return bor(wrd, lshift(band(int, field_mask), pos))
end

--- Extract an unsigned integer value from a bit-field inside a packed word.
---
--- @param wrd integer # Word to read from
--- @param pos integer # Starting bit position (0-based, 0 = LSB)
--- @param len integer # Field width in bits
--- @return integer # Extracted unsigned integer value
local function GetInt(wrd, pos, len)
	return band(rshift(wrd, pos), lshift(1, len) - 1)
end

BitField.SetInt = SetInt
BitField.GetInt = GetInt
BitField.SetBit = SetBit
BitField.GetBit = GetBit

CPK.Bit.Field = BitField
