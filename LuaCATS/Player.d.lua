---@meta

---@class Player
local Player = {}

--- @type table<PlayerId, Player>
Players = {}

---Get player ID
---@return PlayerId
function Player:GetID() end

---Get the type ID of the "best" (or a weighted random one out of the "best" 5) combat unit for Player satisfying the given conditions
---@param bIncludeRanged boolean? whether a ranged unit can be selected, default false
---@param bIncludeShips boolean? whether a naval unit can be selected, default false
---@param bIncludeRecon boolean? whether a recon unit (based on default unit AI) can be selected, default false
---@param bIncludeUUs boolean? whether a unique unit can be selected, default false
---@param bNoResource boolean? whether a unit requiring resource(s) may NOT be selected, default false
---@param bMinorCivGift boolean? whether the unit is supposed to be a city state gift (not NoMinorCivGift units), default false
---@param bRandom boolean? if true, the unit is picked from the top 5 units in the pool in random (weighted), default false
---@param tUnitCombatIDs integer[]? if specified, only units of these UnitCombat types can be selected
---@return UnitTypes
function Player:GetCompetitiveSpawnUnitType(
		bIncludeRanged,
		bIncludeShips,
		bIncludeRecon,
		bIncludeUUs,
		bNoResource,
		bMinorCivGift,
		bRandom,
		tUnitCombatIDs
)
end

--- Checks if player had already researched specified technology
--- @param techId TechId
--- @return boolean
function Player:HasTech(techId) end

--- Gets player's research cost of specified technology
--- @param techId TechId
--- @return integer
function Player:GetResearchCost(techId) end

--- Gets player's research progress of specified technology
--- @param techId TechId
--- @return integer
function Player:GetResearchProgress(techId) end
