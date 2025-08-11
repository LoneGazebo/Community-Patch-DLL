local _lua_type = type

--- Checks if the type of the specified value is `"boolean"`.
---
--- ```lua
--- -- Example
--- local IsBoolean = CPK.Type.IsBoolean
--- local booleanValue = true
--- local numberValue = 123
---
--- print(IsBoolean(booleanValue)) -- true
--- print(IsBoolean(numberValue)) -- false
--- ```
---
--- @param val any # The value to check.
--- @return boolean # `true` if the value is a boolean, `false` otherwise.
--- @nodiscard
local function IsBoolean(val)
	return _lua_type(val) == 'boolean'
end

CPK.Type.IsBoolean = IsBoolean
