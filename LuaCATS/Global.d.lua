--- @meta

--- Loads specified `filename` from VFS. It replaces lua's `require` function in Civ5.
--- See [Specifics Of Lua Implementation in Civ5](http://modiki.civfanatics.com/index.php?title=Specificities_of_the_Lua_implementation_in_Civ5)
--- for more information
--- @see require
--- @param filename string # Filename to load, or regex to load files
--- @param useRegex? boolean #
--- @return nil | string[]
function include(filename, useRegex) end

--- TODO docs
--- @param vector2 { x: number, y: number }
--- @return integer #
function ToHexFromGrid(vector2) end

--- TODO docs
--- @param x integer
--- @param y integer
--- @return integer, integer #
function ToGridFromHex(x, y) end
