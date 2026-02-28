--- Binds the first argument of the specified function
--- @generic Self
--- @generic Args
--- @param fn fun(self: Self, ...: Args): any
--- @param this Self
--- @return fun(...: Args): any
local function Bind(fn, this)
	return function(...)
		return fn(this, ...)
	end
end

CPK.FP.Bind = Bind
