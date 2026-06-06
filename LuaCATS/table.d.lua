--- @meta

--- Create a new table/array with its first `size` entries being `value`.<br>
--- Note that Lua arrays are 1-based.
--- @generic T
--- @param value T
--- @param size integer
--- @return T[]
function table.fill(value, size) end

--- Fill/Overwrite the first `size` entries of the given table with `value`, and return the table.<br>
--- Note that Lua arrays are 1-based.
--- @param t table
--- @param value any
--- @param size integer
--- @return table
function table.fill(t, value, size) end

--- Count the number of non-`nil` entries in a table.<br>
--- Unlike the `#` operator, entries with non-positive-integer keys are counted.
--- @param t table
--- @return integer
function table.count(t) end
