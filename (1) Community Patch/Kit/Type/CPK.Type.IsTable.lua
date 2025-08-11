local _lua_type = type

--- Checks if the type of the specified value is `"table"`.
---
--- ```lua
--- -- Example
--- local IsTable = CPK.Type.IsTable
--- local tableValue = {}
--- local numberValue = 123
---
--- print(IsTable(tableValue)) -- true
--- print(IsTable(numberValue)) -- false
--- ```
---
--- @param val any # The value to check.
--- @return boolean # `true` if the value is a table, `false` otherwise.
--- @nodiscard
local function IsTable(val)
	return _lua_type(val) == "table"
end

CPK.Type.IsTable = IsTable
