local _lua_type = type

--- Checks if the type of the specified value is `"thread"`.
---
--- ```lua
--- -- Example
--- local IsThread = CPK.Type.IsThread
--- local threadValue = coroutine.create(function() end)
--- local numberValue = 123
---
--- print(IsThread(threadValue)) -- true
--- print(IsThread(numberValue)) -- false
--- ```
---
--- @param val any # The value to check.
--- @return boolean # `true` if the value is a thread, `false` otherwise.
--- @nodiscard
local function IsThread(val)
	return _lua_type(val) == "thread"
end

CPK.Type.IsThread = IsThread
