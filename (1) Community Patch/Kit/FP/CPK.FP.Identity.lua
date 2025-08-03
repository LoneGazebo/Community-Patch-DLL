--- Returns the value passed to it unchanged.
--- Useful as a default function or for functional programming purposes.
--- @generic T
--- @param val T # The value to return unchanged
--- @return T # The same value passed as input
--- @nodiscard
local function Identity(val)
	return val
end

CPK.FP.Identity = Identity
