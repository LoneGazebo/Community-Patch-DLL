--- @meta

--- @class Player
--- @field [string] function
local Player = {}

--- @type table<PlayerId, Player>
Players = {}

--- Get player ID
--- @return PlayerId
function Player:GetID() end

--- Get the modifier to trade bonuses on buildings from Player's trait(s)
--- @return integer
function Player:GetTradeBuildingModifier() end

--- Get the type ID of the "best" (or a weighted random one out of the "best" 5) combat unit for Player satisfying the given conditions
--- @param bIncludeRanged boolean? whether a ranged unit can be selected, default false
--- @param bIncludeShips boolean? whether a naval unit can be selected, default false
--- @param bIncludeRecon boolean? whether a recon unit (based on default unit AI) can be selected, default false
--- @param bIncludeUUs boolean? whether a unique unit can be selected, default false
--- @param bNoResource boolean? whether a unit requiring resource(s) may NOT be selected, default false
--- @param bMinorCivGift boolean? whether the unit is supposed to be a city state gift (not NoMinorCivGift units), default false
--- @param bRandom boolean? if true, the unit is picked from the top 5 units in the pool in random (weighted), default false
--- @param tUnitCombatIDs integer[]? if specified, only units of these UnitCombat types can be selected
--- @return UnitType
function Player:GetCompetitiveSpawnUnitType(bIncludeRanged, bIncludeShips, bIncludeRecon, bIncludeUUs, bNoResource, bMinorCivGift, bRandom, tUnitCombatIDs) end

--- Checks if player had already researched specified technology
--- @param eTech TechType
--- @return boolean
function Player:HasTech(eTech) end

--- Gets player's research cost of specified technology
--- @param eTech TechType
--- @return integer
function Player:GetResearchCost(eTech) end

--- Gets player's research progress of specified technology
--- @param eTech TechType
--- @return integer
function Player:GetResearchProgressTimes100(eTech) end

--- @param eGreatPerson GreatPersonType Caller is responsible for validating this
--- @return integer # The cost of the next great person for this player
function Player:GetNextGreatPersonCost(eGreatPerson) end

--- Fast retrieval
--- @return integer # The modifier to great person points for all great people *in all cities* (thus not affecting Generals and Admirals), including traits, policies, and buildings
function Player:GetGreatPeopleRateModifier() end

--- Fast retrieval
--- @param eGreatPerson GreatPersonType Caller is responsible for validating this
--- @return integer # The modifier to great person points for this specific great person, including traits, policies, buildings, and resolutions. Does not affect Great General Point and Great Admiral Point from yields.
function Player:GetGreatPersonRateModifier(eGreatPerson) end

--- Fast retrieval
--- @param eGreatPerson GreatPersonType Caller is responsible for validating this
--- @return integer # The modifier to great person points for this specific great person during golden ages from traits *in all cities* (thus not affecting Generals and Admirals)
function Player:GetGoldenAgeGreatPersonRateModifierFromTrait(eGreatPerson) end
