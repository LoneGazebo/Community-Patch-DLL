local lua_type = type
local AssertError = CPK.Assert.Error

--- Creates a function that asserts the value matches the specified Lua type using a provided test function.
---
--- ```lua
--- -- Example
--- local IsString = CPK.Type.IsString
--- local AssertPrepare = CPK.Assert.Prepare
--- local AssertIsString = AssertPrepare("string", IsString)
--- AssertIsString("Hello") -- Passes
--- AssertIsString(123)     -- Throws an error
--- ```
---
--- @param exp string # Type that is included in error as expected.
--- @param tst fun(val: any): boolean # A function that tests whether the value matches the expected type.
--- @return fun(val: any, mes?: string, lvl?: integer) # A function that asserts the value against test function and throws an error if test fails.
local function AssertPrepare(exp, tst)
	--- Asserts that the specified value matches the expected type using provided function.
	--- Throws an error if the assertion fails.
	---
	--- @param val any # The value to check.
	--- @param mes? string # An optional error message to include if the assertion fails.
	--- @param lvl? integer # An optional level of error. For common asserts level should be 3.
	return function(val, mes, lvl)
		if not tst(val) then
			AssertError(lua_type(val), exp, mes, lvl and (lvl + 1) or 2)
		end
	end
end

CPK.Assert.Prepare = AssertPrepare
