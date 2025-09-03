local _lua_type = type

--- Checks if the type of the specified value is `"userdata"`.
--- @param val any # The value to check.
--- @return boolean # `true` if the value is userdata, `false` otherwise.
--- @nodiscard
local function IsUserdata(val)
	return _lua_type(val) == "userdata"
end

CPK.Type.IsUserdata = IsUserdata
