local lua_select = select

--- Packs variable arguments into a table.
--- <br> Stores arguments in numeric keys and adds field `n` with the count.
--- <br> Compatibility replacement for `table.pack` (Lua 5.1).
--- <br>
--- @generic T
--- @param ... T
--- @return { n: integer, [integer]: T }
--- @nodiscard
--- @see table.pack
local function ArgsPack(...)
	return { n = lua_select('#', ...), ... }
end

CPK.Args.Pack = ArgsPack
