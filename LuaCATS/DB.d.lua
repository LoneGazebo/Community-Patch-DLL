--- @meta

DB = {}

--- Executes an SQL query with optional parameters
--- and returns an iterator over the result rows.
--- @generic Row
--- @param sql string
--- @param ... unknown
--- @return fun(): Row
function DB.Query(sql, ...) end

--- Prepares an SQL query for repeated execution with different parameters.
--- @generic Row
--- @param sql string
--- @return fun(...: unknown): fun(): Row
function DB.CreateQuery(sql) end
