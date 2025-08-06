
--- @meta

DB = {}

--- Executes an SQL query with optional parameters and returns an iterator over the result rows
--- @param sql string
--- @param ... unknown
--- @return fun(): Row
function DB.Query(sql, ...) end

--- Prepares an SQL query iterator for repeated execution with different parameters
--- @param sql string
--- @return fun(...: unknown): fun(): Row
function DB.CreateQuery(sql) end
