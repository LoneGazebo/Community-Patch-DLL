local IsTable = CPK.Type.IsTable

--- Checks if lua debug library is available.
--- <br>
--- To enable lua debug library set `EnableLuaDebugLibrary = 1` in `config.ini`
local function LuaDebugEnabled()
	return IsTable(debug)
end

CPK.Misc.LuaDebugEnabled = LuaDebugEnabled
