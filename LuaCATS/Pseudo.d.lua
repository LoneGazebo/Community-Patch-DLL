--- @meta

--- Below refer to the ingame instance IDs
--- @alias PlayerId integer
--- @alias LandmassId integer
--- @alias ContinentId integer
--- @alias AreaId integer
--- @alias CityId integer
--- @alias PlotId integer
--- @alias TeamId integer
--- @alias UnitId integer
--- @alias GreatWorkId integer

--- Below refer to the database row IDs
--- @alias WorldSizeType integer
--- @alias UnitType integer
--- @alias TechType integer
--- @alias YieldType integer
--- @alias BuildingType integer
--- @alias BuildingClassType integer
--- @alias PlotType integer
--- @alias ResourceType integer
--- @alias TerrainType integer
--- @alias ImprovementType integer
--- @alias FeatureType integer
--- @alias PromotionType integer
--- @alias AccomplishmentType integer
--- @alias EventType integer
--- @alias CityEventType integer
--- @alias CivilizationType integer
--- @alias TraitType integer
--- @alias LeaderType integer
--- @alias BeliefType integer
--- @alias ReligionType integer
--- @alias PolicyType integer
--- @alias ProjectType integer
--- @alias ProcessType integer
--- @alias GreatWorkType integer
--- @alias SpecialistType integer
--- @alias GreatPersonType integer
--- @alias RegionType integer
--- @alias NaturalWonderType integer # This corresponds to the Natural_Wonder_Placement table

--- Usually acceptable values for DLL calls, but may not be valid database row IDs

--- @type table<string, BeliefType>
BeliefTypes = {
	NO_BELIEF = -1,
}

--- @type table<string, ReligionType>
ReligionTypes = {
	NO_RELIGION = -1,
	RELIGION_PANTHEON = 0,
}

--- Pseudo tables

--- The field content differs by popup type when set in DLL. Refer to `CvNotifications::Activate()` and the `AddPopup` functions for details.<br>
--- You may also pass your own constructed `PopupInfo` on Lua side to trigger events like `SerialEventGameMessagePopup`.
--- @class PopupInfo: table
--- @field Data1 integer?
--- @field Data2 integer?
--- @field Data3 integer?
--- @field Data4 integer?
--- @field Data5 integer?
--- @field Option1 boolean?
--- @field Option2 boolean?
--- @field Type ButtonPopupType?

--- @alias BeliefInfoWithScore {
--- 	ID: BeliefType,
--- 	Name: string,
--- 	Description: string,
--- 	Tooltip: string,
--- 	Score: integer,
--- }

--- See CvDllPathFinderUpdate.cpp and CvDllGameContext::TEMPCalculatePathFinderUpdates()
--- @alias PathNode {
--- 	x: integer,
--- 	y: integer,
--- 	turn: integer,
--- }

--- Used in AssignStartingPlots
--- @alias ResourceToPlaceArray {
--- 	[1]: ResourceType,
--- 	[2]: integer,
--- 	[3]: integer,
--- 	[4]: integer,
--- 	[5]: integer,
--- }

--- Used in AssignStartingPlots
--- @alias ResourceToPlace {
--- 	ID: ResourceType,
--- 	Quantity: integer,
--- 	MinRadius: integer,
--- 	MaxRadius: integer,
--- }

--- Used in AssignStartingPlots
--- @alias PlotYieldPotential {
--- 	Food: {Two: integer, Three: integer, Four: integer, Five: integer},
--- 	Prod: {One: integer, Two: integer, Three: integer},
--- 	NativeTwoFood: integer,
--- }

--- Used in AssignStartingPlots
--- @alias RegionId integer
