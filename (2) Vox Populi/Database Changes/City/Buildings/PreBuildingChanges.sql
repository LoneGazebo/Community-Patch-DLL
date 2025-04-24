-- Delete things we replace later
DELETE FROM Building_YieldChanges;
DELETE FROM Building_YieldModifiers;
DELETE FROM Building_PrereqBuildingClasses;
DELETE FROM Building_ClassesNeededInCity;
DELETE FROM Building_LocalResourceAnds;
DELETE FROM Building_ResourceYieldChanges;
DELETE FROM Building_SeaResourceYieldChanges;

-- Delete National Visitor Center
DELETE FROM Civilization_BuildingClassOverrides WHERE BuildingClassType = 'BUILDINGCLASS_TOURIST_CENTER';
DELETE FROM Policy_BuildingClassHappiness WHERE BuildingClassType = 'BUILDINGCLASS_TOURIST_CENTER';
DELETE FROM Building_Flavors WHERE BuildingType = 'BUILDING_TOURIST_CENTER';
DELETE FROM BuildingClasses WHERE Type = 'BUILDINGCLASS_TOURIST_CENTER';
DELETE FROM Buildings WHERE Type = 'BUILDING_TOURIST_CENTER';

-- Delete Paper Maker
DELETE FROM Civilization_BuildingClassOverrides WHERE BuildingType = 'BUILDING_PAPER_MAKER';
DELETE FROM Building_Flavors WHERE BuildingType = 'BUILDING_PAPER_MAKER';
DELETE FROM Building_YieldChangesPerPop WHERE BuildingType = 'BUILDING_PAPER_MAKER';
DELETE FROM Buildings WHERE Type = 'BUILDING_PAPER_MAKER';

-- Delete Mughal Fort
DELETE FROM Civilization_BuildingClassOverrides WHERE BuildingType = 'BUILDING_MUGHAL_FORT';
DELETE FROM Building_Flavors WHERE BuildingType = 'BUILDING_MUGHAL_FORT';
DELETE FROM Buildings WHERE Type = 'BUILDING_MUGHAL_FORT';

-- Delete Pyramid
DELETE FROM Civilization_BuildingClassOverrides WHERE BuildingType = 'BUILDING_MAYA_PYRAMID';
DELETE FROM Buildings WHERE Type = 'BUILDING_MAYA_PYRAMID';

-- Make sure existing unique buildings have the correct BuildingClass first
UPDATE Buildings SET BuildingClass = 'BUILDINGCLASS_NATIONAL_COLLEGE' WHERE Type = 'BUILDING_ROYAL_LIBRARY';
UPDATE Buildings SET BuildingClass = 'BUILDINGCLASS_GROCER' WHERE Type = 'BUILDING_COFFEE_HOUSE';
UPDATE Buildings SET BuildingClass = 'BUILDINGCLASS_CIRCUS' WHERE Type = 'BUILDING_CEILIDH_HALL';
UPDATE Buildings SET BuildingClass = 'BUILDINGCLASS_CARAVANSARY' WHERE Type = 'BUILDING_BURIAL_TOMB';
UPDATE Buildings SET BuildingClass = 'BUILDINGCLASS_MINT' WHERE Type = 'BUILDING_HANSE';
UPDATE Buildings SET BuildingClass = 'BUILDINGCLASS_HERBALIST' WHERE Type = 'BUILDING_LONGHOUSE';
UPDATE Buildings SET BuildingClass = 'BUILDINGCLASS_COURTHOUSE' WHERE Type = 'BUILDING_SATRAPS_COURT';
UPDATE Buildings SET BuildingClass = 'BUILDINGCLASS_FORTRESS' WHERE Type = 'BUILDING_KREPOST';
UPDATE Buildings SET BuildingClass = 'BUILDINGCLASS_CONSTABLE' WHERE Type = 'BUILDING_WAT';
UPDATE Buildings SET BuildingClass = 'BUILDINGCLASS_STONE_WORKS' WHERE Type = 'BUILDING_MUD_PYRAMID_MOSQUE';

-- Make sure unique buildings have the same stats and properties as their vanilla base buildings
-- This may seem like an extra step, but it is one-time work and should never need to change again, while making things easier to balance

-- America: Royal Collection
-- Themes are supposed to be different between buildings. No need to insert it here.

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

-- Aztec: Water Mill
UPDATE Buildings SET River = 1, FreshWater = 0 WHERE Type = 'BUILDING_FLOATING_GARDENS';
DELETE FROM Building_LakePlotYieldChanges WHERE BuildingType = 'BUILDING_FLOATING_GARDENS';

-- Babylon: Walls

-- Byzantium: Temple

-- Carthage: East India COmpany

-- Celts: Circus
UPDATE Buildings
SET
	PrereqTech = 'TECH_TRAPPING',
	Happiness = 2,
	GreatWorkSlotType = NULL,
	GreatWorkCount = 0,
	ArtDefineTag = 'ART_DEF_BUILDING_CIRCUS'
WHERE Type = 'BUILDING_CEILIDH_HALL';

-- Denmark: Lighthouse
INSERT INTO Building_SeaPlotYieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_JELLING_STONES', 'YIELD_FOOD', 1);

-- Egypt: Caravansary
UPDATE Buildings
SET
	PrereqTech = 'TECH_HORSEBACK_RIDING',
	Happiness = 0,
	CapturePlunderModifier = 0,
	TradeRouteLandDistanceModifier = 50,
	TradeRouteLandGoldBonus = 200,
	MaxStartEra = NULL,
	ArtInfoEraVariation = 1
WHERE Type = 'BUILDING_BURIAL_TOMB';

-- England: Ironworks

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

INSERT INTO Building_LocalResourceOrs
	(BuildingType, ResourceType)
VALUES
	('BUILDING_HANSE', 'RESOURCE_GOLD'),
	('BUILDING_HANSE', 'RESOURCE_SILVER');

-- Greece: Amphitheater

-- India: Aqueduct

-- Indonesia: Garden
UPDATE Buildings SET FreshWater = 1 WHERE Type = 'BUILDING_CANDI';

DELETE FROM Building_YieldChangesPerReligion WHERE BuildingType = 'BUILDING_CANDI';

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
	('BUILDING_SIEGE_WORKSHOP', 'DOMAIN_LAND', 15);

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

-- Rome: Arena

-- Siam: Constabulary
DELETE FROM Building_ClassesNeededInCity WHERE BuildingType = 'BUILDING_WAT';

UPDATE Buildings
SET
	PrereqTech = 'TECH_BANKING',
	UnlockedByBelief = 0,
	SpecialistType = NULL,
	SpecialistCount = 0,
	Espionage = 1,
	EspionageModifier = -25
WHERE Type = 'BUILDING_WAT';

DELETE FROM Building_FeatureYieldChanges WHERE BuildingType = 'BUILDING_WAT';

-- Songhai: Stone Works
UPDATE Buildings
SET
	PrereqTech = 'TECH_CALENDAR',
	Happiness = 1,
	MaxStartEra = NULL
WHERE Type = 'BUILDING_MUD_PYRAMID_MOSQUE';

INSERT INTO Building_LocalResourceOrs
	(BuildingType, ResourceType)
VALUES
	('BUILDING_MUD_PYRAMID_MOSQUE', 'RESOURCE_MARBLE'),
	('BUILDING_MUD_PYRAMID_MOSQUE', 'RESOURCE_STONE');

-- Sweden: Public School
INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SKOLA', 'YIELD_SCIENCE', 50);

-- Venice: National Monument

-- Zulu: Barracks
UPDATE Buildings SET TrainedFreePromotion = NULL WHERE Type = 'BUILDING_IKANDA';

-- Existing unique buildings with new base buildings
-- Set to follow NewBuildings.xml, delete from all other tables
-- These need to be edited every time a new column value is added to/removed from the base building in NewBuildings.xml

-- Austria: Grocer
UPDATE Buildings
SET
	PrereqTech = (SELECT PrereqTech FROM Buildings WHERE Type = 'BUILDING_GROCER'),
	FreeStartEra = (SELECT FreeStartEra FROM Buildings WHERE Type = 'BUILDING_GROCER'),
	Happiness = (SELECT Happiness FROM Buildings WHERE Type = 'BUILDING_GROCER'),
	PovertyFlatReduction = (SELECT PovertyFlatReduction FROM Buildings WHERE Type = 'BUILDING_GROCER'),
	FoodKept = (SELECT FoodKept FROM Buildings WHERE Type = 'BUILDING_GROCER'),
	GreatPeopleRateModifier = (SELECT GreatPeopleRateModifier FROM Buildings WHERE Type = 'BUILDING_GROCER'),
	SpecialistType = (SELECT SpecialistType FROM Buildings WHERE Type = 'BUILDING_GROCER'),
	SpecialistCount = (SELECT SpecialistCount FROM Buildings WHERE Type = 'BUILDING_GROCER'),
	ArtDefineTag = (SELECT ArtDefineTag FROM Buildings WHERE Type = 'BUILDING_GROCER')
WHERE Type = 'BUILDING_COFFEE_HOUSE';

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
	PrereqTech = (SELECT PrereqTech FROM Buildings WHERE Type = 'BUILDING_FORTRESS'),
	FreeStartEra = (SELECT FreeStartEra FROM Buildings WHERE Type = 'BUILDING_FORTRESS'),
	CityIndirectFire = (SELECT CityIndirectFire FROM Buildings WHERE Type = 'BUILDING_FORTRESS'),
	CitySupplyModifier = (SELECT CitySupplyModifier FROM Buildings WHERE Type = 'BUILDING_FORTRESS'),
	HealRateChange = (SELECT HealRateChange FROM Buildings WHERE Type = 'BUILDING_FORTRESS'),
	EmpireSizeModifierReduction = (SELECT EmpireSizeModifierReduction FROM Buildings WHERE Type = 'BUILDING_FORTRESS'),
	PlotCultureCostModifier = (SELECT PlotCultureCostModifier FROM Buildings WHERE Type = 'BUILDING_FORTRESS'),
	PlotBuyCostModifier = (SELECT PlotBuyCostModifier FROM Buildings WHERE Type = 'BUILDING_FORTRESS'),
	ArtDefineTag = (SELECT ArtDefineTag FROM Buildings WHERE Type = 'BUILDING_FORTRESS')
WHERE Type = 'BUILDING_KREPOST';

DELETE FROM Building_DomainFreeExperiences WHERE BuildingType = 'BUILDING_KREPOST';
