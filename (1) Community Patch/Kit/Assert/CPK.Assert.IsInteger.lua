local Prepare = CPK.Assert.Prepare
local IsInteger = CPK.Type.IsInteger

--- Asserts that the specified value is a integer.
--- @type fun(val: any, mes?: string, lvl?: integer): nil
CPK.Assert.IsInteger = Prepare('integer', IsInteger)
