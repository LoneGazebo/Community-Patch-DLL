local _lua_select = select

--- Composes the specified functions.
--- Creates a function that executes the specified functions from last to first,
--- passing the result from each function to the previous one.
---
--- ```lua
--- -- Example
--- local Compose = CPK.FP.Compose
--- local A = function(...) return 'a', ... end
--- local B = function(...) return 'b', ... end
--- local C = function(...) return 'c', ... end
--- local fun = Compose(A, B, C) -- function(...) return A(B(C(...))) end
--- local a, b, c, one, two, three = fun(1, 2, 3) -- 'a', 'b', 'c', 1, 2, 3
--- ```
---
--- @param ... function # The functions to compose. They will be executed from last to first.
--- @return function # A new function that applies the composed functions in sequence.
--- @nodiscard
local function Compose(...)
	local fns = { ... }
	local len = _lua_select('#', ...)

	local function Call(i, ...)
		local fn = fns[i]

		if i <= 1 then
			return fn(...)
		end

		return Call(i - 1, fn(...))
	end

	return function(...)
		return Call(len, ...)
	end
end

CPK.FP.Compose = Compose
