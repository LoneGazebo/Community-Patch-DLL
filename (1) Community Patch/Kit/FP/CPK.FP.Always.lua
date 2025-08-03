--- Creates a function that always returns the specified value.
--- Useful for creating constant functions.
--- @generic T
--- @param val T # The value to always return
--- @return fun(): T # A function that returns the specified value
--- @nodiscard
local function Always(val)
	return function()
		return val
	end
end

CPK.FP.Always = Always
