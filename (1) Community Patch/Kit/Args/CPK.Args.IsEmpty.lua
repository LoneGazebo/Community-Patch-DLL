local _lua_select = select

--- Checks whether no arguments were passed.
--- @param ... any # Zero or more arguments
--- @return boolean # true if no arguments were passed, false otherwise
--- @nodiscard
local function ArgsIsEmpty(...)
	return _lua_select('#', ...) == 0
end

CPK.Args.IsEmpty = ArgsIsEmpty
