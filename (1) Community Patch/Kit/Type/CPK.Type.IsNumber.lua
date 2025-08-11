local _lua_type = type

--- Checks if the type of the specified value is `"number"`.
---
--- ```lua
--- -- Example
--- local IsNumber = CPK.Type.IsNumber
--- local numberValue = 123
--- local stringValue = "hello"
---
--- print(IsNumber(numberValue)) -- true
--- print(IsNumber(stringValue)) -- false
--- ```
---
--- @param val any # The value to check.
--- @return boolean # `true` if the value is a number, `false` otherwise.
--- @nodiscard
local function IsNumber(val)
	return _lua_type(val) == "number"
end

CPK.Type.IsNumber = IsNumber
