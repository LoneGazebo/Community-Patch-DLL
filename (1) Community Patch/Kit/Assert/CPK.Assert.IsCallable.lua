local Prepare = CPK.Assert.Prepare
local IsCallable = CPK.Type.IsCallable

--- Asserts that the type of the specified value is a callable (function or a table with `__call` metamethod).
--- @type fun(val: any, mes?: string, lvl?: integer): nil
CPK.Assert.IsCallable = Prepare('callable', IsCallable)
