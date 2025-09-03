local ArgsPack = CPK.Args.Pack
local ArgsUnpack = CPK.Args.Unpack

--- Creates a curried version of the specified function.
--- A curried function takes a specified number of arguments. If fewer arguments are provided,
--- the function returns a new function that takes the remaining arguments.
--- The curried function counts all specified arguments, including `nil`. Empty values are not counted.
---
--- ```lua
--- -- Example
--- local Curry = CPK.FP.Curry
--- local Add = function(a, b, c) return a + b + c end
--- local CurriedAdd = Curry(3, Add)
--- print(CurriedAdd(1)(2)(3)) -- Output: 6
--- ```
---
--- @generic Fn : fun(...: any): any
--- @param len integer # The number of arguments required to execute the function.
--- @param fn Fn # The function to be curried.
--- @param args? { n: integer, [any]: any } # Initial bound arguments. If its size is greater than or equal to `len`, `fn` is executed immediately.
--- @return function # A curried version of the specified function.
--- @nodiscard
local function Curry(len, fn, args)
	local prevArgs = args or ArgsPack()
	local prevSize = prevArgs.n or #prevArgs

	return function(...)
		local currArgs = ArgsPack(...)
		local currSize = currArgs.n

		local nextArgs = { ArgsUnpack(prevArgs) }
		local nextSize = prevSize + currSize

		nextArgs.n = nextSize

		for i = 1, currSize do
			nextArgs[i + prevSize] = currArgs[i]
		end

		if nextSize >= len then
			return fn(ArgsUnpack(nextArgs))
		end

		return Curry(len, fn, nextArgs)
	end
end

CPK.FP.Curry = Curry
