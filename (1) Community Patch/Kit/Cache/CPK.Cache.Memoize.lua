--- Creates a memoized version of a function that can take multiple arguments.
--- The result is cached based on the provided key from the arguments.
--- You have to pass a cache and a function to provide keys.
--- @generic Fn : fun(...: any): any
--- @param fn Fn # The function to be memoized
--- @param cache table # A cache table to store computed values
--- @param hash fun(...: any): any # A function to provide cache keys based on the arguments
--- @return Fn # A memoized version of the function
local function Memoize(fn, cache, hash)
	local c = cache
	local h = hash

	return function(...)
		local key = h(...)

		if key == nil then
			return fn(...)
		end

		local val = c[key]

		if val == nil then
			val = fn(...)
			c[key] = val
		end

		return val
	end
end

CPK.Cache.Memoize = Memoize
