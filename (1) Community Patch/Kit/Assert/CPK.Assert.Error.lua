local lua_error = error
local lua_tostring = tostring

--- Throws assertion error: `Got <got> where <exp> expected. <mes>`
--- @param got string | number | boolean # Actual type or value name.
--- @param exp string # Expected type or value description.
--- @param mes? string # Extra message for context.
--- @param lvl? integer # Error level (adjusted automatically).
local function AssertError(got, exp, mes, lvl)
	got = got == nil and 'unknown' or lua_tostring(got)
	exp = exp == nil and 'unknown' or lua_tostring(exp)
	mes = mes == nil and '' or (' ' .. lua_tostring(mes))
	lvl = lvl == nil and 1 or lvl

	local str = 'Got ' .. got .. ' where ' .. exp .. ' expected.' .. mes

	lua_error(str, lvl + 1)
end

CPK.Assert.Error = AssertError
