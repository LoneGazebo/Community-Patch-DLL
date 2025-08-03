--- Creates a memoized version of a function that can take multiple arguments.
--- The result is cached based on the provided key from the arguments.
--- You have to pass a cache and a function to provide keys.
--- @generic Fn : fun(...: any): any
--- @param fn Fn # The function to be memoized
--- @param cache table # A cache table to store computed values
--- @param hash fun(...: any): any # A function to provide cache keys based on the arguments
--- @return Fn # A memoized version of the function
local function Memoize(fn, cache, hash)
	return function(...)
		local key = hash(...)

		if key == nil then
			return fn(...)
		end

		local val = cache[key]

		if val == nil then
			val = fn(...)

			if val ~= nil then
				cache[key] = val
			end
		end

		return val
	end
end

CPK.Cache.Memoize = Memoize
