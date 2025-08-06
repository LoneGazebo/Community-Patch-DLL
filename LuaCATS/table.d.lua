--- @meta

--- Creates a new table/array with its first `size` entries being `value`.
--- Note that Lua arrays are 1-based.
--- @generic T
--- @param value T
--- @param size integer
--- @return T[]
function table.fill(value, size) end;

--- Counts the number of non-`nil` entries in a table.
--- Unlike the `#` operator, entries with non-positive-integer keys are counted.
--- @param t table
--- @return integer
function table.count(t) end;
