local _lua_type = type

--- Checks if the type of the specified value is `"string"`.
---
--- ```lua
--- -- Example
--- local IsString = CPK.Type.IsString
--- local stringValue = "hello"
--- local numberValue = 123
---
--- print(IsString(stringValue)) -- true
--- print(IsString(numberValue)) -- false
--- ```
---
--- @param val any # The value to check.
--- @return boolean # `true` if the value is a string, `false` otherwise.
--- @nodiscard
local function IsString(val)
	return _lua_type(val) == "string"
end

CPK.Type.IsString = IsString
