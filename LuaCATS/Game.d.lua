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

--- Load screen only, from SequenceGameInitComplete and then each update.
--- Lays out the next batch of map graphics the DLL deferred, once the last
--- one has been dispatched. False when done (or after 10 minutes)
--- @return boolean bPending
function Game.ContinueDeferredLayout() end

--- Load screen only. Progress of the deferred map layout, in estimated KB
--- @return integer iDone
--- @return integer iTotal
function Game.GetDeferredLayoutProgress() end
