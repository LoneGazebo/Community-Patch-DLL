local lua_tostring = tostring

local AssertError = CPK.Assert.Error
local AssertIsNumber = CPK.Assert.IsNumber
local AssertIsString = CPK.Assert.IsString
local AssertIsFunction = CPK.Assert.IsFunction

--- Returns a formatted traceback string.
--- @param mes string | nil
--- @param lvl integer | nil
--- @return string
local function Trace(mes, lvl)
	local trace = traceback or (debug and debug.traceback)

	-- `traceback` function exists in every state except `Main State`
	-- `debug` should exist in `Main State`
	-- `debug` exists everywhere if EnableLuaDebugLibrary = 1 is set in config.ini
	AssertIsFunction(trace, 'Traceback function missing. This should not happen!')

	mes = mes or ''
	lvl = lvl or 1

	AssertIsString(mes)
	AssertIsNumber(lvl)

	if lvl < 1 then
		AssertError(lua_tostring(lvl), 'number >= 1')
	end

	-- lvl is adjusted to omit current function in trace
	local res = trace(mes, lvl + 1)

	-- If no message, remove leading newline
	return #mes == 0 and res:sub(2) or res
end

CPK.Debug.Trace = Trace
