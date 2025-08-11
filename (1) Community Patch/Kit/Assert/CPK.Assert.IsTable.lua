local Prepare = CPK.Assert.Prepare
local IsTable = CPK.Type.IsTable

--- Asserts that the type of the specified value is a table.
--- @type fun(val: any, mes?: string, lvl?: integer): nil
CPK.Assert.IsTable = Prepare('table', IsTable)
