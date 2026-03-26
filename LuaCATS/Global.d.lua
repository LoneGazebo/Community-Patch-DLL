--- @meta

--- A super global table that persists across all Lua contexts, useful for sharing data between them
--- @type table
MapModData = {}

--- Loads specified `filename` from VFS. It replaces lua's `require` function in Civ5.
--- See [Specifics Of Lua Implementation in Civ5](http://modiki.civfanatics.com/index.php?title=Specificities_of_the_Lua_implementation_in_Civ5)
--- for more information
--- @see require
--- @param filename string # Filename to load, or regex to load files
--- @param useRegex? boolean #
--- @return nil | string[]
function include(filename, useRegex) end

--- TODO docs
--- @param vector2 Vector2
--- @return Vector2
function ToHexFromGrid(vector2) end

--- TODO docs
--- @param x integer
--- @param y integer
--- @return integer, integer #
function ToGridFromHex(x, y) end

--- TODO docs
--- @param vector2 Vector2
--- @return Vector3
function HexToWorld(vector2) end

--- TODO docs
--- @param x integer
--- @param y integer
--- @return integer, integer, integer
function GridToWorld(x, y) end

--- Require `include("SupportFunctions")` to work<br>
--- Truncate a given string if necessary so that the resultant string (with ellipsis and the given suffix) fits just within the target size.<br>
--- The resultant string is set as the given control's text.
--- @param control Control The control that the text should fit in. Font type and size may affect the result.
--- @param iTargetSize integer The target width of the truncated text (+ ellipsis and suffix).
--- @param strOriginal string? The string to be truncated. The control's current text will be used if this is nil.
--- @param strSuffix string? The optional string to be appended to the truncated text. This counts towards the target size.
function TruncateString(control, iTargetSize, strOriginal, strSuffix) end

--- Alias for `debug.traceback`
--- that's available in every state except `Main State`
--- even if EnableLuaDebugLibrary = 0 in config.ini
traceback = debug.traceback
