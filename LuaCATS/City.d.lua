--- @meta

--- @class City
local City = {}

--- Gets city owner player id.
--- @return PlayerID
function City:GetOwner() end

---Get the type ID of the "best" (or a weighted random one out of the "best" 5) combat unit for City owner satisfying the given conditions<br>
---The unit is assumed to be spawned in City, so all city restrictions apply
---@param bIncludeRanged boolean? whether a ranged unit can be selected, default false
---@param bIncludeShips boolean? whether a naval unit can be selected, default false
---@param bIncludeRecon boolean? whether a recon unit (based on default unit AI) can be selected, default false
---@param bIncludeUUs boolean? whether a unique unit can be selected, default false
---@param bNoResource boolean? whether a unit requiring resource(s) may NOT be selected, default false
---@param bMinorCivGift boolean? whether the unit is supposed to be a city state gift (not NoMinorCivGift units), default false
---@param bRandom boolean? if true, the unit is picked from the top 5 units in the pool in random (weighted), default false
---@param tUnitCombatIDs integer[]? if specified, only units of these UnitCombat types can be selected
---@return UnitTypes
function City:GetCompetitiveSpawnUnitType(bIncludeRanged, bIncludeShips, bIncludeRecon, bIncludeUUs, bNoResource, bMinorCivGift, bRandom, tUnitCombatIDs) end
