local Prepare = CPK.Assert.Prepare
local IsString = CPK.Type.IsString

--- Asserts that the type of the specified value is a string.
--- @type fun(val: any, mes?: string, lvl?: integer): nil
CPK.Assert.IsString = Prepare('string', IsString)
