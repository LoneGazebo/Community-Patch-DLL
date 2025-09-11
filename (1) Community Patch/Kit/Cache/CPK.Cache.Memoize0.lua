local lua_setmetatable = setmetatable

--- Creates a memoized version of a nullary (zero-argument) function.
--- <br> The result of the first call is cached and reused on subsequent calls.
--- @generic Fn : fun(): any
--- @param fn Fn # The function to memoize. Must take no arguments.
--- @param cache table? # Optional table to use as cache. If omitted, a weak-valued table is used.
--- @return Fn # A memoized version of the original function.
local function Memoize0(fn, cache)
	local c = cache or lua_setmetatable({}, { __mode = 'v' })

	return function(...)
		local val = c[1]

		if val ~= nil then
			return val
		end

		val = fn(...)
		c[1] = val

		return val
	end
end

CPK.Cache.Memoize0 = Memoize0
