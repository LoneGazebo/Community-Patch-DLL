--- Checks if the type of the specified value is `"nil"`.
---
--- ```lua
--- -- Example
--- local IsNil = CPK.Type.IsNil
--- local nilValue = nil
--- local numberValue = 123
---
--- print(IsNil(nilValue)) -- true
--- print(IsNil(numberValue)) -- false
--- ```
---
--- @param val any # The value to check.
--- @return boolean # `true` if the value is `nil`, `false` otherwise.
--- @nodiscard
local function IsNil(val)
	return val == nil
end

CPK.Type.IsNil = IsNil
