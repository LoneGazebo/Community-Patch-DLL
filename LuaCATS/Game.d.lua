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
