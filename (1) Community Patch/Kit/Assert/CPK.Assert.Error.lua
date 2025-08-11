local _lua_error = error

--- Throws type assertion error.
--- @param got string # Got
--- @param exp string # Expected
--- @param mes? string # Extra Message
--- @param lvl? integer # Error level
--- @return nil
local function AssertError(got, exp, mes, lvl)
	got = got == nil and 'unknown' or got
	exp = exp == nil and 'unknown' or exp
	mes = mes == nil and '' or (' ' .. mes)

	local str = 'Got ' .. got .. ' where ' .. exp .. ' expected.' .. mes

	_lua_error(str, lvl)
end

CPK.Assert.Error = AssertError
