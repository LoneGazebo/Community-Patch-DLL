local Prepare = CPK.Assert.Prepare
local IsNil = CPK.Type.IsNil

--- Asserts that the type of the specified value is nil.
--- @type fun(val: any, mes?: string, lvl?: integer): nil
CPK.Assert.IsNil = Prepare('nil', IsNil)
