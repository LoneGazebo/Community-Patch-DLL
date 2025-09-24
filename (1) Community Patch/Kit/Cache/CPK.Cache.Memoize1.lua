local lua_setmetatable = setmetatable

local Identity = CPK.FP.Identity
local Memoize = CPK.Cache.Memoize

--- Creates a memoized version of a function basing on it's first argument.
--- The result is cached based on the provided key from the first argument.
--- Optionally, you can pass a custom cache or/and a function to provide keys.
--- @generic Fn : fun(arg: any, ...: any): any
--- @param fn Fn # The function to be memoized
--- @param cache? table # An optional cache table (default is weak-value table)
--- @param hash? fun(arg: any): any # A function to provide cache keys based on the arg (defaults to Identity of firest arg)
--- @return Fn # A memoized version of the function
local function Memoize1(fn, cache, hash)
	return Memoize(
		fn,
		cache or lua_setmetatable({}, { __mode = 'v' }),
		hash or Identity
	)
end

CPK.Cache.Memoize1 = Memoize1
