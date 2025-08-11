local Prepare = CPK.Assert.Prepare
local IsFunction = CPK.Type.IsFunction

--- Asserts that the type of the specified value is a function.
--- @type fun(val: any, mes?: string, lvl?: integer): nil
CPK.Assert.IsFunction = Prepare('function', IsFunction)
