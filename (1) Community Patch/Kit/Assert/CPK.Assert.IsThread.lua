local Prepare = CPK.Assert.Prepare
local IsThread = CPK.Type.IsThread

--- Asserts that the type of the specified value is a thread.
--- @type fun(val: any, mes?: string, lvl?: integer): nil
CPK.Assert.IsThread = Prepare('thread', IsThread)
