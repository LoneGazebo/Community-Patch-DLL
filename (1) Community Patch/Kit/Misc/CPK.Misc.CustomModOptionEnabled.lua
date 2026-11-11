local IsFunction = CPK.Type.IsFunction

local civ_db_create_query = DB.CreateQuery

local query = civ_db_create_query([[
	SELECT NULL
	FROM CustomModOptions
	WHERE Name = ? AND Value = 1
	LIMIT 1
]])

--- Checks if a custom mod option exists and is enabled by the given name.
--- Uses `Game.IsCustomModOption` if available, otherwise falls back to an SQL query.
--- @param name string # The option name, e.g., "BALANCE_VP".
--- @return boolean # True if the option exists in the database and has value 1, false otherwise.
local function CustomModOptionEnabled(name)
	if Game and IsFunction(Game.IsCustomModOption) then
		return Game.IsCustomModOption(name)
	end

	-- Using a direct SQL query should be faster 
	-- than querying via GameInfo({ Name = name })().Value == 1,
	-- The GameInfo call creates more intermediate tables for both
	-- the query and the results and do not leverage cache
	-- because of the table structure.
	-- Step the query to the end: a statement left on its first row stays open and
	-- makes the next database rebuild fail with "database table is locked".
	local bEnabled = false
	for _ in query(name) do
		bEnabled = true
	end
	return bEnabled
end

CPK.Misc.CustomModOptionEnabled = CustomModOptionEnabled