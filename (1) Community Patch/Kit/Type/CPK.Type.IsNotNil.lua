--- Checks if the type of the specified value is not nil.
--- @param val any # The value to check.
--- @return boolean # `true` if the value is not `nil`, `false` otherwise.
--- @nodiscard
local function IsNotNil(val)
	return val ~= nil
end

CPK.Type.IsNil = IsNotNil
