--- @meta

--- @class City
--- @field [string] function
local City = {}

--- Gets city owner player id.
--- @return PlayerId
function City:GetOwner() end

--- Get the type ID of the "best" (or a weighted random one out of the "best" 5) combat unit for City owner satisfying the given conditions<br>
--- The unit is assumed to be spawned in City, so all city restrictions apply
--- @param bIncludeRanged boolean? whether a ranged unit can be selected, default false
--- @param bIncludeShips boolean? whether a naval unit can be selected, default false
--- @param bIncludeRecon boolean? whether a recon unit (based on default unit AI) can be selected, default false
--- @param bIncludeUUs boolean? whether a unique unit can be selected, default false
--- @param bNoResource boolean? whether a unit requiring resource(s) may NOT be selected, default false
--- @param bMinorCivGift boolean? whether the unit is supposed to be a city state gift (not NoMinorCivGift units), default false
--- @param bRandom boolean? if true, the unit is picked from the top 5 units in the pool in random (weighted), default false
--- @param tUnitCombatIDs integer[]? if specified, only units of these UnitCombat types can be selected
--- @return UnitType
function City:GetCompetitiveSpawnUnitType(bIncludeRanged, bIncludeShips, bIncludeRecon, bIncludeUUs, bNoResource, bMinorCivGift, bRandom, tUnitCombatIDs) end

--- Get the type (unit/building/project/process) and details of an item in production queue
--- @param iQueueIndex integer The position of the order, starting at 0
--- @return OrderType eOrder
--- @return integer iData1 Database ID of the item
--- @return integer iData2 Unit AI of the item (only used for units)
--- @return boolean bSave Whether the item is on repeat (unused)
--- @return boolean bRush Whether the item is rushed (used by AI only to determine whether to chop trees)
function City:GetOrderFromQueue(iQueueIndex) end

--- @param eBuildingClass BuildingClassType
--- @param iSlotId integer
--- @return GreatWorkId eGreatWork # ID of the great work if exists, or -1 otherwise
function City:GetBuildingGreatWork(eBuildingClass, iSlotId) end

--- @param eSpecialist SpecialistType
--- @param bTooltip boolean? Whether a tooltip should be returned, default false
--- @return integer # The specialist rate (GPP) of the specified specialist type, **multiplied by 100**
--- @return string # The breakdown of sources of GPP and modifiers, if `bTooltip` is true
function City:GetSpecialistRate(eSpecialist, bTooltip) end
