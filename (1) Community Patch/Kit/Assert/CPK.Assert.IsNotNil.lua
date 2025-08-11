local Prepare = CPK.Assert.Prepare
local IsNotNil = CPK.Type.IsNotNil

--- Asserts that the type of the specified value is not nil.
--- @type fun(val: any, mes?: string, lvl?: integer): nil
CPK.Assert.IsNotNil = Prepare('not nil', IsNotNil)
