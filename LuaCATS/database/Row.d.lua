--- @meta

--- The row returned by [GameInfo](lua://GameInfo) or [DB](lua://DB) iterators
--- @class Row
--- @field [string] unknown
Row = {}

--- The row returned by [GameInfo](lua://GameInfo) or [DB](lua://DB) iterators or key access on primary tables<br>
--- Partial versions of some of these are prefetched in the [VP](lua://VP) table for faster access
--- @class Info: Row
--- @field ID integer
--- @field Type string
--- @field Description string
--- @field Help string
--- @field IconIndex integer
--- @field PortraitIndex integer
--- @field [string] unknown
Info = {}
