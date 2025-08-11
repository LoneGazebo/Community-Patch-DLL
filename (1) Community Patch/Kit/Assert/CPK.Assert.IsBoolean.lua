local Prepare = CPK.Assert.Prepare

local IsBoolean = CPK.Type.IsBoolean

--- Asserts that the type of the specified value is a boolean.
--- @type fun(val: any, mes?: string, lvl?: integer): nil
CPK.Assert.IsBoolean = Prepare('boolean', IsBoolean)
