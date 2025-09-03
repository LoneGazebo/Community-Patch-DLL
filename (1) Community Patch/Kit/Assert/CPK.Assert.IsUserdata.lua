local Prepare = CPK.Assert.Prepare
local IsUserdata = CPK.Type.IsUserdata

--- Asserts that the type of the specified value is a userdata.
--- @type fun(val: any, mes?: string, lvl?: integer): nil
CPK.Assert.IsUserdata = Prepare('userdata', IsUserdata)
