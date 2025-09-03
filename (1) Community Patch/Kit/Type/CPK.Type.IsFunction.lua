local _lua_type = type

--- Checks if the type of the specified value is `"function"`.
---
--- ```lua
--- -- Example
--- local IsFunction = CPK.Type.IsFunction
--- local funcValue = function() end
--- local numberValue = 123
---
--- print(IsFunction(funcValue)) -- true
--- print(IsFunction(numberValue)) -- false
--- ```
---
--- @param val any # The value to check.
--- @return boolean # `true` if the value is a function, `false` otherwise.
--- @nodiscard
local function IsFunction(val)
	return _lua_type(val) == 'function'
end

CPK.Type.IsFunction = IsFunction
