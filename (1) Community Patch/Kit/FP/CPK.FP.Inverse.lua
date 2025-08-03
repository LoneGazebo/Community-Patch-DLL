--- Creates a function that inverts the result of the specified predicate.
--- This function returns a new predicate function that returns the opposite boolean value
--- of the original predicate function.
---
--- ```lua
--- -- Example
--- local Inverse = CPK.FP.Inverse
--- local IsEven = function(num) return num % 2 == 0 end
--- local IsOdd = Inverse(IsEven)
--- print(IsOdd(2)) -- Output: false
--- print(IsOdd(3)) -- Output: true
--- ```
---
--- @generic T
--- @param predicate fun(...: T): boolean # The predicate function whose result will be inverted.
--- @return fun(...: T): boolean # A new function that returns the negation of the original predicate's result.
--- @nodiscard
local function Inverse(predicate)
	return function(...)
		return not predicate(...)
	end
end

CPK.FP.Inverse = Inverse
