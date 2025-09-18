local lua_select = select

--- Pipes the specified functions, creating a function that executes them in sequence.
--- Each function's output is passed as input to the next function.
---
--- ```lua
--- -- Example
--- local Pipe = CPK.FP.Pipe
--- local A = function(...) return 'a', ... end
--- local B = function(...) return 'b', ... end
--- local C = function(...) return 'c', ... end
--- local fun = Pipe(A, B, C) -- function(...) return C(B(A(...))) end
--- local c, b, a, one, two, three = fun(1, 2, 3) -- 'c', 'b', 'a', 1, 2, 3
--- ```
---
--- @param ... function # The functions to pipe. Each function receives the results of the previous function.
--- @return function # A function that applies the piped functions in sequence.
--- @nodiscard
local function Pipe(...)
	local fns = { ... }
	local len = lua_select('#', ...)

	local function Call(i, ...)
		local fn = fns[i]

		if i >= len then
			return fn(...)
		end

		return Call(i + 1, fn(...))
	end

	return function(...)
		return Call(1, ...)
	end
end

CPK.FP.Pipe = Pipe
