--- @meta

--- @class Game
--- @field [string] function
Game = {}

--- Result is different for each player! Don't use in functions that affect the game state or it will desync!
--- @return PlayerId
function Game.GetActivePlayer() end

--- Result is different for each player! Don't use in functions that affect the game state or it will desync!
--- @return TeamId
function Game.GetActiveTeam() end

--- Given an in-game great work ID, return its database row ID
--- @param eGreatWork GreatWorkId
--- @return GreatWorkType eGreatWorkType
function Game.GetGreatWorkType(eGreatWork) end
