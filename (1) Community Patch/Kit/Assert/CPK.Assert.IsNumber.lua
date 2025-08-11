local Prepare = CPK.Assert.Prepare

local IsNumber = CPK.Type.IsNumber

--- Asserts that the type of the specified value is a number.
--- @type fun(val: any, mes?: string, lvl?: integer): nil
CPK.Assert.IsNumber = Prepare('number', IsNumber)
