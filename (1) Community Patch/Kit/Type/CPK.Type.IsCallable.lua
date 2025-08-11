local _lua_type = type
local _lua_getmetatable = getmetatable

--- Checks if the specified value is callable.
--- A value is considered callable if it is a function or if it is a table with a `__call` method in its metatable.
---
--- ```lua
--- -- Example
--- local IsCallable = CPK.Type.IsCallable
--- local function func() end
--- local obj = setmetatable({}, { __call = function() end })
--- local tbl = {}
---
--- print(IsCallable(func))        -- Output: true
--- print(IsCallable(obj))         -- Output: true
--- print(IsCallable(tbl))         -- Output: false
--- ```
---
--- @param val any # The value to check.
--- @return boolean # `true` if the value is callable, `false` otherwise.
--- @nodiscard
local function IsCallable(val)
	local t = _lua_type(val)

	if t == 'function' then
			return true
	end

	if t == 'table' then
			local mt = _lua_getmetatable(val)

			return _lua_type(mt) == 'table' and _lua_type(mt.__call) == 'function'
	end

	return false
end

CPK.Type.IsCallable = IsCallable
