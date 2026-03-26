-- Delete things we replace later
DELETE FROM Building_YieldChanges;
DELETE FROM Building_YieldModifiers;
DELETE FROM Building_PrereqBuildingClasses;
DELETE FROM Building_ClassesNeededInCity;
DELETE FROM Building_LocalResourceAnds;
DELETE FROM Building_LocalResourceOrs;
DELETE FROM Building_ResourceYieldChanges;
DELETE FROM Building_SeaResourceYieldChanges;

-- Delete National Visitor Center
DELETE FROM Civilization_BuildingClassOverrides WHERE BuildingClassType = 'BUILDINGCLASS_TOURIST_CENTER';
DELETE FROM Policy_BuildingClassHappiness WHERE BuildingClassType = 'BUILDINGCLASS_TOURIST_CENTER';
DELETE FROM BuildingClasses WHERE Type = 'BUILDINGCLASS_TOURIST_CENTER';
DELETE FROM Buildings WHERE Type = 'BUILDING_TOURIST_CENTER';

-- Delete Paper Maker
DELETE FROM Civilization_BuildingClassOverrides WHERE BuildingType = 'BUILDING_PAPER_MAKER';
DELETE FROM Building_YieldChangesPerPop WHERE BuildingType = 'BUILDING_PAPER_MAKER';
DELETE FROM Buildings WHERE Type = 'BUILDING_PAPER_MAKER';

-- Delete Pyramid
DELETE FROM Civilization_BuildingClassOverrides WHERE BuildingType = 'BUILDING_MAYA_PYRAMID';
DELETE FROM Buildings WHERE Type = 'BUILDING_MAYA_PYRAMID';

-- Delete Floating Gardens
DELETE FROM Civilization_BuildingClassOverrides WHERE BuildingType = 'BUILDING_FLOATING_GARDENS';
DELETE FROM Building_LakePlotYieldChanges WHERE BuildingType = 'BUILDING_FLOATING_GARDENS';
DELETE FROM Buildings WHERE Type = 'BUILDING_FLOATING_GARDENS';

-- Delete Coffee House
DELETE FROM Civilization_BuildingClassOverrides WHERE BuildingType = 'BUILDING_COFFEE_HOUSE';
DELETE FROM Buildings WHERE Type = 'BUILDING_COFFEE_HOUSE';

-- Delete Burial Tomb
DELETE FROM Civilization_BuildingClassOverrides WHERE BuildingType = 'BUILDING_BURIAL_TOMB';
DELETE FROM Buildings WHERE Type = 'BUILDING_BURIAL_TOMB';

-- Delete Parthenon BuildingClass
DELETE FROM Civilization_BuildingClassOverrides WHERE BuildingClassType = 'BUILDINGCLASS_PARTHENON';
DELETE FROM BuildingClasses WHERE Type = 'BUILDINGCLASS_PARTHENON';

-- Make sure existing unique buildings have the correct BuildingClass first
UPDATE Buildings SET BuildingClass = 'BUILDINGCLASS_NATIONAL_COLLEGE' WHERE Type = 'BUILDING_ROYAL_LIBRARY';
UPDATE Buildings SET BuildingClass = 'BUILDINGCLASS_CIRCUS' WHERE Type = 'BUILDING_CEILIDH_HALL';
UPDATE Buildings SET BuildingClass = 'BUILDINGCLASS_MINT' WHERE Type = 'BUILDING_HANSE';
UPDATE Buildings SET BuildingClass = 'BUILDINGCLASS_SCRIVENERS_OFFICE' WHERE Type = 'BUILDING_PARTHENON';
UPDATE Buildings SET BuildingClass = 'BUILDINGCLASS_HERBALIST' WHERE Type = 'BUILDING_LONGHOUSE';
UPDATE Buildings SET BuildingClass = 'BUILDINGCLASS_COURTHOUSE' WHERE Type = 'BUILDING_SATRAPS_COURT';
UPDATE Buildings SET BuildingClass = 'BUILDINGCLASS_BASTION_FORT' WHERE Type = 'BUILDING_KREPOST';
UPDATE Buildings SET BuildingClass = 'BUILDINGCLASS_WAT' WHERE Type = 'BUILDING_WAT';
UPDATE Buildings SET BuildingClass = 'BUILDINGCLASS_STONE_WORKS' WHERE Type = 'BUILDING_MUD_PYRAMID_MOSQUE';

-- Make sure unique buildings have the same stats and properties as their vanilla base buildings
-- This may seem like an extra step, but it is one-time work and should never need to change again, while making things easier to balance

-- Arabia: Market
UPDATE Buildings SET ExtraLuxuries = 0 WHERE Type = 'BUILDING_BAZAAR';
DELETE FROM Building_FeatureYieldChanges WHERE BuildingType = 'BUILDING_BAZAAR';

-- Assyria: School of Philosophy
UPDATE Buildings
SET
	PrereqTech = 'TECH_PHILOSOPHY',
	FreeStartEra = NULL,
	GreatWorkSlotType = NULL,
	GreatWorkCount = 0,
	ArtDefineTag = 'ART_DEF_BUILDING_NATIONAL_COLLEGE'
WHERE Type = 'BUILDING_ROYAL_LIBRARY';

DELETE FROM Building_YieldChangesPerPop WHERE BuildingType = 'BUILDING_ROYAL_LIBRARY';
DELETE FROM Building_DomainFreeExperiencePerGreatWork WHERE BuildingType = 'BUILDING_ROYAL_LIBRARY';

-- Babylon: Walls

-- Celts: Circus
UPDATE Buildings
SET
	PrereqTech = 'TECH_TRAPPING',
	Happiness = 2,
	GreatWorkSlotType = NULL,
	GreatWorkCount = 0,
	ArtDefineTag = 'ART_DEF_BUILDING_CIRCUS'
WHERE Type = 'BUILDING_CEILIDH_HALL';

-- Ethiopia: Monument

-- Germany: Customs House
UPDATE Buildings
SET
	PrereqTech = 'TECH_CURRENCY',
	SpecialistType = NULL,
	SpecialistCount = 0,
	TradeRouteRecipientBonus = 0,
	TradeRouteTargetBonus = 0,
	CityStateTradeRouteProductionModifier = 0,
	FreeStartEra = NULL
WHERE Type = 'BUILDING_HANSE';

-- India: Castle
UPDATE Buildings
SET
	TechEnhancedTourism = 0,
	ArtDefineTag = 'CASTLE',
	DisplayPosition = 1
WHERE Type = 'BUILDING_MUGHAL_FORT';

-- Indonesia: Garden
UPDATE Buildings SET FreshWater = 1 WHERE Type = 'BUILDING_CANDI';

DELETE FROM Building_YieldChangesPerReligion WHERE BuildingType = 'BUILDING_CANDI';

-- Persia: Courthouse
UPDATE Buildings
SET
	PrereqTech = 'TECH_MATHEMATICS',
	FreeStartEra = NULL,
	Happiness = 0,
	SpecialistType = NULL,
	SpecialistCount = 0,
	TradeRouteRecipientBonus = 0,
	TradeRouteTargetBonus = 0,
	NoOccupiedUnhappiness = 1,
	ArtDefineTag = 'COURTHOUSE'
WHERE Type = 'BUILDING_SATRAPS_COURT';

DELETE FROM Building_ClassesNeededInCity WHERE BuildingType = 'BUILDING_SATRAPS_COURT';

-- Poland: Stable
DELETE FROM Building_UnitCombatFreeExperiences WHERE BuildingType = 'BUILDING_DUCAL_STABLE';

-- Songhai: Stone Works
UPDATE Buildings
SET
	PrereqTech = 'TECH_CALENDAR',
	Happiness = 1,
	MaxStartEra = NULL
WHERE Type = 'BUILDING_MUD_PYRAMID_MOSQUE';

-- Zulu: Barracks
UPDATE Buildings SET TrainedFreePromotion = NULL WHERE Type = 'BUILDING_IKANDA';

-- New unique buildings with existing base buildings
-- Only need to insert into non-main tables here; others have been done in NewBuildings.xml
--
-- Relevant tables:
-- Building_DomainFreeExperiences
-- Building_DomainProductionModifiers
-- Building_ResourceQuantity
-- Building_ResourceQuantityRequirements
-- Building_RiverPlotYieldChanges
-- Building_SeaPlotYieldChanges
-- Building_FeatureYieldChanges
-- Building_UnitCombatProductionModifiers
-- Building_YieldChangesPerPop
--
-- Building_ThemingBonuses: Themes are supposed to be different between buildings. No need to insert it here.

-- America: Stable
INSERT INTO Building_UnitCombatProductionModifiers
	(BuildingType, UnitCombatType, Modifier)
VALUES
	('BUILDING_HOMESTEAD', 'UNITCOMBAT_MOUNTED', 15);

-- Arabia: University
INSERT INTO Building_FeatureYieldChanges
	(BuildingType, FeatureType, YieldType, Yield)
VALUES
	('BUILDING_BIMARISTAN', 'FEATURE_JUNGLE', 'YIELD_SCIENCE', 2);

-- Austria: Military Academy
INSERT INTO Building_DomainFreeExperiences
	(BuildingType, DomainType, Experience)
VALUES
	('BUILDING_SCHUTZENSTAND', 'DOMAIN_LAND', 15),
	('BUILDING_SCHUTZENSTAND', 'DOMAIN_SEA', 15),
	('BUILDING_SCHUTZENSTAND', 'DOMAIN_AIR', 15);

-- Aztec: Library
INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TELPOCHCALLI', 'YIELD_SCIENCE', 50);

-- Denmark: Lighthouse
INSERT INTO Building_SeaPlotYieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_RUNESTONE', 'YIELD_FOOD', 1);

-- England: Factory
INSERT INTO Building_ResourceQuantityRequirements
	(BuildingType, ResourceType, Cost)
VALUES
	('BUILDING_STEAM_MILL', 'RESOURCE_COAL', 1);

-- Japan: Armory
INSERT INTO Building_DomainFreeExperiences
	(BuildingType, DomainType, Experience)
VALUES
	('BUILDING_DOJO', 'DOMAIN_LAND', 15),
	('BUILDING_DOJO', 'DOMAIN_SEA', 15),
	('BUILDING_DOJO', 'DOMAIN_AIR', 15);

-- Korea: University
INSERT INTO Building_FeatureYieldChanges
	(BuildingType, FeatureType, YieldType, Yield)
VALUES
	('BUILDING_SEOWON', 'FEATURE_JUNGLE', 'YIELD_SCIENCE', 2);

-- Ottoman: Forge
INSERT INTO Building_DomainProductionModifiers
	(BuildingType, DomainType, Modifier)
VALUES
	('BUILDING_SIEGE_FOUNDRY', 'DOMAIN_LAND', 15);

-- Poland: Armory
INSERT INTO Building_DomainFreeExperiences
	(BuildingType, DomainType, Experience)
VALUES
	('BUILDING_BARBICAN', 'DOMAIN_LAND', 15),
	('BUILDING_BARBICAN', 'DOMAIN_SEA', 15),
	('BUILDING_BARBICAN', 'DOMAIN_AIR', 15);

-- Existing unique buildings with new base buildings
-- Set to follow NewBuildings.xml, delete from all other tables
-- These need to be edited every time a new column value is added to/removed from the base building in NewBuildings.xml

-- Greece: Scrivener's Office
-- Keeping the on-map building model for now
UPDATE Buildings
SET
	PrereqTech = (SELECT PrereqTech FROM Buildings WHERE Type = 'BUILDING_SCRIVENERS_OFFICE'),
	MaxStartEra = (SELECT MaxStartEra FROM Buildings WHERE Type = 'BUILDING_SCRIVENERS_OFFICE'),
	SpecialistType = (SELECT SpecialistType FROM Buildings WHERE Type = 'BUILDING_SCRIVENERS_OFFICE'),
	SpecialistCount = (SELECT SpecialistCount FROM Buildings WHERE Type = 'BUILDING_SCRIVENERS_OFFICE'),
	GreatPeopleRateChange = (SELECT GreatPeopleRateChange FROM Buildings WHERE Type = 'BUILDING_SCRIVENERS_OFFICE'),
	GreatWorkSlotType = (SELECT GreatWorkSlotType FROM Buildings WHERE Type = 'BUILDING_SCRIVENERS_OFFICE'),
	GreatWorkCount = (SELECT GreatWorkCount FROM Buildings WHERE Type = 'BUILDING_SCRIVENERS_OFFICE'),
	FreeGreatWork = (SELECT FreeGreatWork FROM Buildings WHERE Type = 'BUILDING_SCRIVENERS_OFFICE'),
	WonderSplashImage = NULL,
	WonderSplashAudio = NULL
WHERE Type = 'BUILDING_PARTHENON';

-- Iroquois: Herbalist
UPDATE Buildings
SET
	PrereqTech = (SELECT PrereqTech FROM Buildings WHERE Type = 'BUILDING_HERBALIST'),
	SpecialistType = (SELECT SpecialistType FROM Buildings WHERE Type = 'BUILDING_HERBALIST'),
	SpecialistCount = (SELECT SpecialistCount FROM Buildings WHERE Type = 'BUILDING_HERBALIST'),
	AllowsProductionTradeRoutes = (SELECT AllowsProductionTradeRoutes FROM Buildings WHERE Type = 'BUILDING_HERBALIST'),
	ArtDefineTag = (SELECT ArtDefineTag FROM Buildings WHERE Type = 'BUILDING_HERBALIST')
WHERE Type = 'BUILDING_LONGHOUSE';

DELETE FROM Building_FeatureYieldChanges WHERE BuildingType = 'BUILDING_LONGHOUSE';

-- Russia: Bastion Fort
UPDATE Buildings
SET
	PrereqTech = (SELECT PrereqTech FROM Buildings WHERE Type = 'BUILDING_BASTION_FORT'),
	FreeStartEra = (SELECT FreeStartEra FROM Buildings WHERE Type = 'BUILDING_BASTION_FORT'),
	CityIndirectFire = (SELECT CityIndirectFire FROM Buildings WHERE Type = 'BUILDING_BASTION_FORT'),
	RangedStrikeModifier = (SELECT RangedStrikeModifier FROM Buildings WHERE Type = 'BUILDING_BASTION_FORT'),
	CitySupplyModifier = (SELECT CitySupplyModifier FROM Buildings WHERE Type = 'BUILDING_BASTION_FORT'),
	HealRateChange = (SELECT HealRateChange FROM Buildings WHERE Type = 'BUILDING_BASTION_FORT'),
	EmpireSizeModifierReduction = (SELECT EmpireSizeModifierReduction FROM Buildings WHERE Type = 'BUILDING_BASTION_FORT'),
	PlotCultureCostModifier = (SELECT PlotCultureCostModifier FROM Buildings WHERE Type = 'BUILDING_BASTION_FORT'),
	PlotBuyCostModifier = (SELECT PlotBuyCostModifier FROM Buildings WHERE Type = 'BUILDING_BASTION_FORT'),
	ArtDefineTag = (SELECT ArtDefineTag FROM Buildings WHERE Type = 'BUILDING_BASTION_FORT')
WHERE Type = 'BUILDING_KREPOST';

DELETE FROM Building_DomainFreeExperiences WHERE BuildingType = 'BUILDING_KREPOST';

-- Existing unique buildings with no base buildings
-- Set everything back to default values (as if the buildings do nothing)
-- These should never need to be changed in the future

-- Siam: Wat
UPDATE Buildings
SET
	PrereqTech = NULL,
	UnlockedByBelief = 0,
	SpecialistType = NULL,
	SpecialistCount = 0
WHERE Type = 'BUILDING_WAT';

DELETE FROM Building_FeatureYieldChanges WHERE BuildingType = 'BUILDING_WAT';
