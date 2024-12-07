-- NOTE: In NewBuildings.xml and PreBuildingChanges.sql, we already made sure all unique buildings have the same stats and properties as their counterparts.
-- We modify buildings based on BuildingClass here to keep them the same still. The Civilization files only have deltas between their UB and the base building.

-- Helper temp tables because CTE isn't a thing in this SQLite version
CREATE TEMP TABLE Helper (
	BuildingClassType TEXT,
	RequiredBuildingClass TEXT,
	TerrainType TEXT,
	FeatureType TEXT,
	ResourceType TEXT,
	ImprovementType TEXT,
	SpecialistType TEXT,
	YieldType TEXT,
	Yield INTEGER,
	Description TEXT,
	RequiresOwner BOOLEAN,
	RequiresAnyButOwner BOOLEAN,
	SameEra BOOLEAN,
	ConsecutiveEras BOOLEAN,
	UniqueEras BOOLEAN
);

CREATE TEMP TABLE Helper2 (
	YieldType TEXT
);

----------------------------------------------------------------------------
-- Palace
----------------------------------------------------------------------------

-- Unlocks Great People birth, Era advance, World Wonder construction and war victory Historic Events
UPDATE Buildings
SET
	EventTourism = 7,
	NoUnhappfromXSpecialists = 1,
	VassalLevyEra = 2
WHERE BuildingClass = 'BUILDINGCLASS_PALACE';

INSERT INTO Helper
	(YieldType, Yield)
VALUES
	('YIELD_PRODUCTION', 3),
	('YIELD_GOLD', 5),
	('YIELD_SCIENCE', 6),
	('YIELD_CULTURE', 1);

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, b.Yield
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_PALACE';

DELETE FROM Helper;

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 34
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_PALACE';

----------------------------------------------------------------------------
-- Guild support/theming line (which starts at Monument for some reason)
----------------------------------------------------------------------------

-- Monument
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_MONUMENT';

-- Amphitheater
UPDATE Buildings
SET Help = 'TXT_KEY_BUILDING_AMPHITHEATER_HELP'
WHERE Type = 'BUILDING_AMPHITHEATER';

UPDATE Buildings
SET
	ThemingBonusHelp = 'TXT_KEY_AMPHITHEATER_THEMING_BONUS_HELP',
	GreatWorkCount = 2
WHERE BuildingClass = 'BUILDINGCLASS_AMPHITHEATER';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_AMPHITHEATER';

INSERT INTO Building_SpecificGreatPersonRateModifier
	(BuildingType, SpecialistType, Modifier)
SELECT
	Type, 'SPECIALIST_WRITER', 33
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_AMPHITHEATER';

-- Bonus and AIPriority to be swept in ThemingSweeps.sql
INSERT INTO Helper
	(Description, RequiresOwner, RequiresAnyButOwner)
VALUES
	('TXT_KEY_THEMING_BONUS_AMPHITHEATER_NATIONAL', 1, NULL),
	('TXT_KEY_THEMING_BONUS_AMPHITHEATER_GLOBAL', NULL, 1);

INSERT INTO Building_ThemingBonuses
	(BuildingType, Description, RequiresOwner, RequiresAnyButOwner)
SELECT a.Type, b.Description, b.RequiresOwner, b.RequiresAnyButOwner
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_AMPHITHEATER';

DELETE FROM Helper;

-- Opera House
UPDATE Buildings
SET Help = 'TXT_KEY_BUILDING_OPERA_HOUSE_HELP'
WHERE Type = 'BUILDING_OPERA_HOUSE';

UPDATE Buildings
SET CultureRateModifier = 5
WHERE BuildingClass = 'BUILDINGCLASS_OPERA_HOUSE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 3
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_OPERA_HOUSE';

INSERT INTO Building_SpecificGreatPersonRateModifier
	(BuildingType, SpecialistType, Modifier)
SELECT
	Type, 'SPECIALIST_MUSICIAN', 33
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_OPERA_HOUSE';

-- Gallery
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_GALLERY';

INSERT INTO Building_SpecificGreatPersonRateModifier
	(BuildingType, SpecialistType, Modifier)
SELECT
	Type, 'SPECIALIST_ARTIST', 33
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_GALLERY';

-- Museum
UPDATE Buildings
SET
	Help = 'TXT_KEY_BUILDING_MUSEUM_HELP',
	IconAtlas = 'COMMUNITY_3_ATLAS',
	PortraitIndex = 7
WHERE Type = 'BUILDING_MUSEUM';

UPDATE Buildings
SET BoredomFlatReduction = 1
WHERE BuildingClass = 'BUILDINGCLASS_MUSEUM';

INSERT INTO Helper
	(YieldType)
VALUES
	('YIELD_CULTURE'),
	('YIELD_TOURISM');

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, 25
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_MUSEUM';

DELETE FROM Helper;

-- Broadcast Tower
UPDATE Buildings
SET
	CultureRateModifier = 0,
	GreatWorkCount = 2,
	ThemingBonusHelp = 'TXT_KEY_BROADCAST_TOWER_THEMING_BONUS_HELP',
	BoredomFlatReduction = 1
WHERE BuildingClass = 'BUILDINGCLASS_BROADCAST_TOWER';

INSERT INTO Helper
	(YieldType)
VALUES
	('YIELD_CULTURE'),
	('YIELD_TOURISM');

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, 50
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_BROADCAST_TOWER';

DELETE FROM Helper;

-- Any two Great Work of Music can be themed
-- Bonus and AIPriority to be swept in ThemingSweeps.sql
INSERT INTO Helper
	(Description, SameEra, ConsecutiveEras, UniqueEras)
VALUES
	('TXT_KEY_THEMING_BONUS_BROADCAST_TOWER_NATIONAL', 1, NULL, NULL),
	('TXT_KEY_THEMING_BONUS_BROADCAST_TOWER_CLASSICS', NULL, 1, NULL),
	('TXT_KEY_THEMING_BONUS_BROADCAST_TOWER_ALL', NULL, NULL, 1);

INSERT INTO Building_ThemingBonuses
	(BuildingType, Description, SameEra, ConsecutiveEras, UniqueEras)
SELECT a.Type, b.Description, b.SameEra, b.ConsecutiveEras, b.UniqueEras
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_BROADCAST_TOWER';

DELETE FROM Helper;

----------------------------------------------------------------------------
-- Faith line
----------------------------------------------------------------------------

-- Shrine
UPDATE Buildings
SET PrereqTech = 'TECH_AGRICULTURE'
WHERE BuildingClass = 'BUILDINGCLASS_SHRINE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_FAITH', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_SHRINE';

-- Temple
UPDATE Buildings
SET Help = 'TXT_KEY_BUILDING_TEMPLE_HELP'
WHERE Type = 'BUILDING_TEMPLE';

UPDATE Buildings
SET
	ReligiousPressureModifier = 25,
	GreatWorkSlotType = 'GREAT_WORK_SLOT_MUSIC',
	GreatWorkCount = 1,
	ReligiousUnrestFlatReduction = 1
WHERE BuildingClass = 'BUILDINGCLASS_TEMPLE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_FAITH', 3
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_TEMPLE';

----------------------------------------------------------------------------
-- Food line
----------------------------------------------------------------------------

-- Granary
UPDATE Buildings
SET FoodKept = 15
WHERE BuildingClass = 'BUILDINGCLASS_GRANARY';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_FOOD', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_GRANARY';

INSERT INTO Building_InstantYield
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_FOOD', 25
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_GRANARY';

-- Aqueduct
UPDATE Buildings
SET
	FoodKept = 15,
	PovertyFlatReduction = 1
WHERE BuildingClass = 'BUILDINGCLASS_AQUEDUCT';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_FOOD', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_AQUEDUCT';

INSERT INTO Building_LakePlotYieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_FOOD', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_AQUEDUCT';

INSERT INTO Building_FeatureYieldChanges
	(BuildingType, FeatureType, YieldType, Yield)
SELECT
	Type, 'FEATURE_OASIS', 'YIELD_FOOD', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_AQUEDUCT';

INSERT INTO Building_GrowthExtraYield
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_PRODUCTION', 25
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_AQUEDUCT';

-- Garden
UPDATE Buildings
SET
	FreshWater = 0,
	NoUnhappfromXSpecialists = 1
WHERE BuildingClass = 'BUILDINGCLASS_GARDEN';

INSERT INTO Building_FeatureYieldChanges
	(BuildingType, FeatureType, YieldType, Yield)
SELECT
	Type, 'FEATURE_OASIS', 'YIELD_GOLD', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_GARDEN';

-- Grocer
INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_FOOD', 20
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_GROCER';

-- Agribusiness
INSERT INTO Building_ResourceQuantityRequirements
	(BuildingType, ResourceType, Cost)
SELECT
	Type, 'RESOURCE_HORSE', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_STOCKYARD';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_FOOD', 5
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_STOCKYARD';

INSERT INTO Helper
	(ImprovementType, YieldType)
VALUES
	('IMPROVEMENT_FARM', 'YIELD_PRODUCTION'),
	('IMPROVEMENT_FARM', 'YIELD_GOLD'),
	('IMPROVEMENT_PASTURE', 'YIELD_PRODUCTION'),
	('IMPROVEMENT_PASTURE', 'YIELD_GOLD');

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
SELECT
	a.Type, b.ImprovementType, b.YieldType, 2
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_STOCKYARD';

DELETE FROM Helper;

INSERT INTO Building_YieldFromYieldPercent
	(BuildingType, YieldIn, YieldOut, Value)
SELECT
	Type, 'YIELD_FOOD', 'YIELD_GOLD', 10
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_STOCKYARD';

-- Hospital
UPDATE Buildings
SET Help = 'TXT_KEY_BUILDING_HOSPITAL_HELP'
WHERE Type = 'BUILDING_HOSPITAL';

UPDATE Buildings
SET
	AlwaysHeal = 15,
	PovertyFlatReduction = 1,
	NoUnhappfromXSpecialists = 2
WHERE BuildingClass = 'BUILDINGCLASS_HOSPITAL';

INSERT INTO Building_SpecialistYieldChangesLocal
	(BuildingType, SpecialistType, YieldType, Yield)
SELECT
	a.Type, b.Type, 'YIELD_FOOD', 1
FROM Buildings a, Specialists b
WHERE a.BuildingClass = 'BUILDINGCLASS_HOSPITAL'
AND b.GreatPeopleUnitClass IS NOT NULL;

INSERT INTO Helper
	(SpecialistType, YieldType)
VALUES
	('SPECIALIST_SCIENTIST', 'YIELD_SCIENCE'),
	('SPECIALIST_ENGINEER', 'YIELD_PRODUCTION'),
	('SPECIALIST_MERCHANT', 'YIELD_GOLD');

INSERT INTO Building_SpecialistYieldChangesLocal
	(BuildingType, SpecialistType, YieldType, Yield)
SELECT
	a.Type, b.SpecialistType, b.YieldType, 2
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_HOSPITAL';

DELETE FROM Helper;

-- Medical Lab
UPDATE Buildings
SET
	PopulationChange = 2,
	FoodKept = 15
WHERE BuildingClass = 'BUILDINGCLASS_MEDICAL_LAB';

INSERT INTO Building_YieldFromBirth
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 50
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_MEDICAL_LAB';

INSERT INTO Building_YieldFromBirthRetroactive
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 50
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_MEDICAL_LAB';

----------------------------------------------------------------------------
-- Science line
----------------------------------------------------------------------------

-- Council
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_GROVE';

INSERT INTO Building_YieldFromBirth
	(BuildingType, YieldType, Yield, IsEraScaling)
SELECT
	Type, 'YIELD_SCIENCE', 5, 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_GROVE';

-- Library
UPDATE Buildings
SET
	SpecialistType = 'SPECIALIST_SCIENTIST',
	SpecialistCount = 1,
	IlliteracyFlatReduction = 1,
	NoUnhappfromXSpecialists = 1
WHERE BuildingClass = 'BUILDINGCLASS_LIBRARY';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_LIBRARY';

DELETE FROM Building_YieldChangesPerPop
WHERE BuildingType IN (
	SELECT Type FROM Buildings
	WHERE BuildingClass = 'BUILDINGCLASS_LIBRARY'
);

-- University
UPDATE Buildings
SET
	SpecialistCount = 1,
	IlliteracyFlatReduction = 1
WHERE BuildingClass = 'BUILDINGCLASS_UNIVERSITY';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 3
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_UNIVERSITY';

UPDATE Building_FeatureYieldChanges
SET Yield = 1
WHERE BuildingType IN (
	SELECT Type FROM Buildings
	WHERE BuildingClass = 'BUILDINGCLASS_UNIVERSITY'
);

INSERT INTO Building_TerrainYieldChanges
	(BuildingType, TerrainType, YieldType, Yield)
SELECT
	Type, 'TERRAIN_SNOW', 'YIELD_SCIENCE', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_UNIVERSITY';

INSERT INTO Building_GrowthExtraYield
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 25
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_UNIVERSITY';

-- Public School
UPDATE Buildings
SET IlliteracyFlatReduction = 1
WHERE BuildingClass = 'BUILDINGCLASS_PUBLIC_SCHOOL';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 3
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_PUBLIC_SCHOOL';

-- Research Lab
UPDATE Buildings
SET IlliteracyFlatReduction = 1
WHERE BuildingClass = 'BUILDINGCLASS_LABORATORY';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 4
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_LABORATORY';

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
SELECT
	Type, 'IMPROVEMENT_ACADEMY', 'YIELD_SCIENCE', 4
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_LABORATORY';

INSERT INTO Helper
	(BuildingClassType)
VALUES
	('BUILDINGCLASS_HOSPITAL'),
	('BUILDINGCLASS_MEDICAL_LAB'),
	('BUILDINGCLASS_FACTORY');

INSERT INTO Building_BuildingClassLocalYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
SELECT
	a.Type, b.BuildingClassType, 'YIELD_SCIENCE', 4
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_LABORATORY';

DELETE FROM Helper;

INSERT INTO Building_SpecificGreatPersonRateModifier
	(BuildingType, SpecialistType, Modifier)
SELECT
	Type, 'SPECIALIST_SCIENTIST', 33
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_LABORATORY';

----------------------------------------------------------------------------
-- Gold line
----------------------------------------------------------------------------

-- Market
UPDATE Buildings
SET Help = 'TXT_KEY_BUILDING_MARKET_HELP'
WHERE Type = 'BUILDING_MARKET';

UPDATE Buildings
SET PrereqTech = 'TECH_HORSEBACK_RIDING' -- Trade
WHERE BuildingClass = 'BUILDINGCLASS_MARKET';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_GOLD', 3
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_MARKET';

-- Caravansary
UPDATE Buildings
SET
	PrereqTech = 'TECH_CURRENCY',
	FinishLandTRTourism = 10,
	TradeRouteLandGoldBonus = 300
WHERE BuildingClass = 'BUILDINGCLASS_CARAVANSARY';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_GOLD', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_CARAVANSARY';

INSERT INTO Helper
	(TerrainType, YieldType)
VALUES
	('TERRAIN_DESERT', 'YIELD_FOOD'),
	('TERRAIN_DESERT', 'YIELD_GOLD'),
	('TERRAIN_TUNDRA', 'YIELD_FOOD'),
	('TERRAIN_TUNDRA', 'YIELD_GOLD');

INSERT INTO Building_YieldPerXTerrainTimes100
	(BuildingType, TerrainType, YieldType, Yield)
SELECT
	a.Type, b.TerrainType, b.YieldType, 50
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_CARAVANSARY';

DELETE FROM Helper;

-- Customs House
DELETE FROM Building_LocalResourceOrs
WHERE BuildingType IN (
	SELECT Type FROM Buildings
	WHERE BuildingClass = 'BUILDINGCLASS_MINT'
);

UPDATE Buildings
SET
	PrereqTech = 'TECH_GUILDS',
	TradeRouteRecipientBonus = 2,
	TradeRouteTargetBonus = 2,
	SpecialistType = 'SPECIALIST_MERCHANT',
	SpecialistCount = 1,
	PovertyFlatReduction = 1
WHERE BuildingClass = 'BUILDINGCLASS_MINT';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_MINT';

-- Bank
UPDATE Buildings
SET
	TradeRouteRecipientBonus = 0,
	TradeRouteTargetBonus = 0
WHERE BuildingClass = 'BUILDINGCLASS_BANK';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_GOLD', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_BANK';

INSERT INTO Helper
	(BuildingClassType)
VALUES
	('BUILDINGCLASS_CARAVANSARY'),
	('BUILDINGCLASS_MINT');

INSERT INTO Building_BuildingClassLocalYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
SELECT a.Type, b.BuildingClassType, 'YIELD_GOLD', 3
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_BANK';

DELETE FROM Helper;

INSERT INTO Building_YieldFromPurchase
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 15
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_BANK';

-- Stock Exchange
UPDATE Buildings
SET SpecialistCount = 1
WHERE BuildingClass = 'BUILDINGCLASS_STOCK_EXCHANGE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_GOLD', 3
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_STOCK_EXCHANGE';

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_GOLD', 50
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_STOCK_EXCHANGE';

INSERT INTO Building_HurryModifiersLocal
	(BuildingType, HurryType, HurryCostModifier)
SELECT
	Type, 'HURRY_GOLD', -20
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_STOCK_EXCHANGE';

INSERT INTO Helper
	(ImprovementType)
VALUES
	('IMPROVEMENT_CUSTOMS_HOUSE'),
	('IMPROVEMENT_TRADING_POST');

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
SELECT a.Type, b.ImprovementType, 'YIELD_GOLD', 2
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_STOCK_EXCHANGE';

DELETE FROM Helper;

----------------------------------------------------------------------------
-- Military line
----------------------------------------------------------------------------

-- Barracks
UPDATE Buildings
SET
	PrereqTech = 'TECH_ARCHERY', -- Military Strategy
	CitySupplyFlat = 1,
	DistressFlatReduction = 1
WHERE BuildingClass = 'BUILDINGCLASS_BARRACKS';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_BARRACKS';

-- Armory
UPDATE Buildings
SET
	CitySupplyFlat = 1,
	DistressFlatReduction = 1
WHERE BuildingClass = 'BUILDINGCLASS_ARMORY';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_ARMORY';

UPDATE Building_DomainFreeExperiences
SET Experience = 20
WHERE BuildingType IN (
	SELECT Type FROM Buildings
	WHERE BuildingClass = 'BUILDINGCLASS_ARMORY'
);

-- Military Academy
UPDATE Buildings
SET
	CitySupplyFlat = 1,
	DistressFlatReduction = 1
WHERE BuildingClass = 'BUILDINGCLASS_MILITARY_ACADEMY';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_MILITARY_ACADEMY';

UPDATE Building_DomainFreeExperiences
SET Experience = 25
WHERE BuildingType IN (
	SELECT Type FROM Buildings
	WHERE BuildingClass = 'BUILDINGCLASS_MILITARY_ACADEMY'
);

INSERT INTO Building_DomainProductionModifiers
	(BuildingType, DomainType, Modifier)
SELECT
	Type, 'DOMAIN_LAND', 15
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_MILITARY_ACADEMY';

----------------------------------------------------------------------------
-- Defense line
----------------------------------------------------------------------------

-- Walls
UPDATE Buildings
SET Help = 'TXT_KEY_BUILDING_WALLS_HELP'
WHERE Type = 'BUILDING_WALLS';

UPDATE Buildings
SET
	CityRangedStrikeRange = 1,
	CitySupplyModifier = 5,
	EmpireSizeModifierReduction = -5
WHERE BuildingClass = 'BUILDINGCLASS_WALLS';

-- Castle
UPDATE Buildings
SET
	CitySupplyModifier = 5,
	GreatWorkSlotType = 'GREAT_WORK_SLOT_ART_ARTIFACT',
	GreatWorkCount = 1,
	EmpireSizeModifierReduction = -5
WHERE BuildingClass = 'BUILDINGCLASS_CASTLE';

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
SELECT
	Type, 'IMPROVEMENT_QUARRY', 'YIELD_PRODUCTION', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_CASTLE';

-- Bastion Fort

-- Arsenal
UPDATE Buildings
SET
	PrereqTech = 'TECH_COMBUSTION',
	CityRangedStrikeRange = 1,
	CitySupplyModifier = 5,
	HealRateChange = 5,
	EmpireSizeModifierReduction = -5
WHERE BuildingClass = 'BUILDINGCLASS_ARSENAL';

-- Military Base
UPDATE Buildings
SET
	PrereqTech = 'TECH_RADAR',
	RangedStrikeModifier = 10,
	CitySupplyModifier = 5,
	HealRateChange = 20,
	DistressFlatReduction = 1,
	EmpireSizeModifierReduction = -5,
	CityAirStrikeDefense = 15
WHERE BuildingClass = 'BUILDINGCLASS_MILITARY_BASE';

INSERT INTO Building_DomainProductionModifiers
	(BuildingType, DomainType, Modifier)
SELECT
	Type, 'DOMAIN_AIR', 25
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_MILITARY_BASE';

----------------------------------------------------------------------------
-- Production line
----------------------------------------------------------------------------

-- Forge
UPDATE Buildings
SET
	PrereqTech = 'TECH_BRONZE_WORKING',
	SpecialistType = 'SPECIALIST_ENGINEER',
	SpecialistCount = 1
WHERE BuildingClass = 'BUILDINGCLASS_FORGE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_FORGE';

DELETE FROM Building_DomainProductionModifiers
WHERE BuildingType IN (
	SELECT Type FROM Buildings
	WHERE BuildingClass = 'BUILDINGCLASS_FORGE'
);

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
SELECT
	Type, 'IMPROVEMENT_MINE', 'YIELD_PRODUCTION', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_FORGE';

-- Workshop
UPDATE Buildings
SET PrereqTech = 'TECH_CIVIL_SERVICE'
WHERE BuildingClass = 'BUILDINGCLASS_WORKSHOP';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_PRODUCTION', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_WORKSHOP';

INSERT INTO Building_FeatureYieldChanges
	(BuildingType, FeatureType, YieldType, Yield)
SELECT
	Type, 'FEATURE_FOREST', 'YIELD_PRODUCTION', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_WORKSHOP';

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_PRODUCTION', 25
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_WORKSHOP';

-- Factory
UPDATE Buildings
SET
	XBuiltTriggersIdeologyChoice = 0,
	SpecialistCount = 1,
	NoUnhappfromXSpecialists = 1
WHERE BuildingClass = 'BUILDINGCLASS_FACTORY';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_PRODUCTION', 5
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_FACTORY';

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_PRODUCTION', 25
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_FACTORY';

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
SELECT
	Type, 'IMPROVEMENT_MANUFACTORY', 'YIELD_PRODUCTION', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_FACTORY';

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
SELECT
	Type, 'BUILDINGCLASS_FACTORY', 'YIELD_PRODUCTION', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_FACTORY';

-- Spaceship Factory
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_PRODUCTION', 3
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_SPACESHIP_FACTORY';

INSERT INTO Building_YieldModifiers
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 25
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_SPACESHIP_FACTORY';

----------------------------------------------------------------------------
-- Coastal line
----------------------------------------------------------------------------

-- Lighthouse
UPDATE Buildings
SET AllowsWaterRoutes = 1
WHERE BuildingClass = 'BUILDINGCLASS_LIGHTHOUSE';

-- Already has +1 food from vanilla
INSERT INTO Building_SeaPlotYieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_GOLD', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_LIGHTHOUSE';

-- Harbor
UPDATE Buildings
SET
	AllowsWaterRoutes = 0,
	FinishSeaTRTourism = 10,
	TradeRouteSeaGoldBonus = 200,
	CitySupplyFlat = 1
WHERE BuildingClass = 'BUILDINGCLASS_HARBOR';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_GOLD', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_HARBOR';

INSERT INTO Building_SeaPlotYieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_FOOD', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_HARBOR';

INSERT INTO Building_SeaResourceYieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_PRODUCTION', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_HARBOR';

INSERT INTO Building_DomainProductionModifiers
	(BuildingType, DomainType, Modifier)
SELECT
	Type, 'DOMAIN_SEA', 15
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_HARBOR';

-- Seaport
UPDATE Buildings
SET
	PrereqTech = 'TECH_STEAM_POWER',
	CitySupplyModifier = 10,
	AllowsIndustrialWaterRoutes = 1
WHERE BuildingClass = 'BUILDINGCLASS_SEAPORT';

INSERT INTO Helper
	(YieldType)
VALUES
	('YIELD_PRODUCTION'),
	('YIELD_GOLD');

INSERT INTO Building_SeaPlotYieldChanges
	(BuildingType, YieldType, Yield)
SELECT a.Type, b.YieldType, 1
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_SEAPORT';

INSERT INTO Building_SeaResourceYieldChanges
	(BuildingType, YieldType, Yield)
SELECT a.Type, b.YieldType, 2
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_SEAPORT';

DELETE FROM Helper;

-- Mine Field
INSERT INTO Building_ResourceQuantityRequirements
	(BuildingType, ResourceType, Cost)
SELECT
	Type, 'RESOURCE_IRON', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_MINEFIELD';

----------------------------------------------------------------------------
-- Instant yield line (which starts at Arena)
----------------------------------------------------------------------------

-- Arena
UPDATE Buildings
SET Help = 'TXT_KEY_BUILDING_COLOSSEUM_HELP'
WHERE Type = 'BUILDING_COLOSSEUM';

UPDATE Buildings
SET
	Happiness = 0,
	BoredomFlatReduction = 1
WHERE BuildingClass = 'BUILDINGCLASS_COLOSSEUM';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_COLOSSEUM';

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_TOURISM', 25
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_COLOSSEUM';

INSERT INTO Helper
	(BuildingClassType)
VALUES
	('BUILDINGCLASS_BARRACKS'),
	('BUILDINGCLASS_FORGE'),
	('BUILDINGCLASS_ARMORY');

INSERT INTO Building_BuildingClassLocalYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
SELECT a.Type, b.BuildingClassType, 'YIELD_PRODUCTION', 2
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_COLOSSEUM';

DELETE FROM Helper;

-- Circus
DELETE FROM Building_LocalResourceOrs
WHERE BuildingType IN (
	SELECT Type FROM Buildings
	WHERE BuildingClass = 'BUILDINGCLASS_CIRCUS'
);

UPDATE Buildings
SET
	PrereqTech = 'TECH_PHYSICS',
	WLTKDTurns = 10,
	Happiness = 1,
	BoredomFlatReduction = 1
WHERE BuildingClass = 'BUILDINGCLASS_CIRCUS';

INSERT INTO Building_InstantYield
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 100
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_CIRCUS';

-- Zoo
UPDATE Buildings
SET
	PrereqTech = 'TECH_SCIENTIFIC_THEORY',
	Happiness = 0,
	FinishLandTRTourism = 5,
	FinishSeaTRTourism = 5,
	BoredomFlatReduction = 1
WHERE BuildingClass = 'BUILDINGCLASS_THEATRE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_THEATRE';

INSERT INTO Building_InstantYield
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 500
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_THEATRE';

INSERT INTO Helper
	(FeatureType, YieldType)
VALUES
	('FEATURE_FOREST', 'YIELD_CULTURE'),
	('FEATURE_FOREST', 'YIELD_TOURISM'),
	('FEATURE_JUNGLE', 'YIELD_CULTURE'),
	('FEATURE_JUNGLE', 'YIELD_TOURISM');

INSERT INTO Building_FeatureYieldChanges
	(BuildingType, FeatureType, YieldType, Yield)
SELECT
	a.Type, b.FeatureType, b.YieldType, 1
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_THEATRE';

DELETE FROM Helper;

-- Stadium
UPDATE BuildingClasses SET Description = 'TXT_KEY_BUILDING_STADIUM_DESC' WHERE Type = 'BUILDINGCLASS_STADIUM';

UPDATE Buildings SET
	PrereqTech = 'TECH_ATOMIC_THEORY',
	Happiness = 0,
	MinorFriendshipChange = 20,
	GreatWorksTourismModifier = 50
WHERE BuildingClass = 'BUILDINGCLASS_STADIUM';

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_GOLDEN_AGE_POINTS', 50
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_STADIUM';

----------------------------------------------------------------------------
-- Espionage line
----------------------------------------------------------------------------

-- Constabulary
UPDATE Buildings
SET
	EspionageModifier = 0,
	SpySecurityModifier = 20,
	SpySecurityModifierPerXPop = 180, -- ESPIONAGE_SECURITY_PER_POPULATION_BUILDING_SCALER = 360, so 180/360 gives 1 per 2 population in city
	DistressFlatReduction = 1
WHERE BuildingClass = 'BUILDINGCLASS_CONSTABLE';

-- Police Station
UPDATE Buildings
SET
	PrereqTech = 'TECH_ELECTRONICS',
	EspionageModifier = 0,
	SpySecurityModifier = 10,
	SpySecurityModifierPerXPop = 180, -- ESPIONAGE_SECURITY_PER_POPULATION_BUILDING_SCALER = 360, so 180/360 gives 1 per 2 population in city
	DistressFlatReduction = 1,
	PovertyFlatReduction = 1,
	IlliteracyFlatReduction = 1,
	BoredomFlatReduction = 1
WHERE BuildingClass = 'BUILDINGCLASS_POLICE_STATION';

----------------------------------------------------------------------------
-- Tourism line
----------------------------------------------------------------------------

-- Hotel
UPDATE Buildings
SET
	PrereqTech = 'TECH_RAILROAD',
	LandmarksTourismPercent = 25,
	GreatWorksTourismModifier = 25
WHERE BuildingClass = 'BUILDINGCLASS_HOTEL';

-- Interpretive Center
INSERT INTO Helper
	(YieldType)
VALUES
	('YIELD_CULTURE'),
	('YIELD_TOURISM');

INSERT INTO Building_ImprovementYieldChangesGlobal
	(BuildingType, ImprovementType, YieldType, Yield)
SELECT
	a.Type, 'IMPROVEMENT_LANDMARK', b.YieldType, 1
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_INTERPRETIVE_CENTER';

DELETE FROM Helper;

----------------------------------------------------------------------------
-- Diplo line
----------------------------------------------------------------------------

-- Chancery
INSERT INTO Building_UnitCombatProductionModifiers
	(BuildingType, UnitCombatType, Modifier)
SELECT
	Type, 'UNITCOMBAT_DIPLOMACY', 10
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_CHANCERY';

INSERT INTO Building_YieldPerAlly
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_PRODUCTION', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_CHANCERY';

-- Wire Service
INSERT INTO Building_UnitCombatProductionModifiers
	(BuildingType, UnitCombatType, Modifier)
SELECT
	Type, 'UNITCOMBAT_DIPLOMACY', 20
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_WIRE_SERVICE';

INSERT INTO Building_YieldPerFriend
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_WIRE_SERVICE';

INSERT INTO Helper
	(YieldType)
VALUES
	('YIELD_SCIENCE'),
	('YIELD_CULTURE');

INSERT INTO Building_YieldPerAlly
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, 1
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_WIRE_SERVICE';

DELETE FROM Helper;

----------------------------------------------------------------------------
-- Guilds
----------------------------------------------------------------------------

UPDATE BuildingClasses
SET MaxPlayerInstances = 3
WHERE Type IN (
	'BUILDINGCLASS_WRITERS_GUILD',
	'BUILDINGCLASS_ARTISTS_GUILD',
	'BUILDINGCLASS_MUSICIANS_GUILD'
);

-- Writers' Guild
UPDATE Buildings
SET
	GreatPeopleRateChange = 3,
	NoUnhappfromXSpecialists = 1
WHERE BuildingClass = 'BUILDINGCLASS_WRITERS_GUILD';

-- Artists' Guild
UPDATE Buildings
SET
	GreatPeopleRateChange = 4,
	GreatWorkSlotType = 'GREAT_WORK_SLOT_ART_ARTIFACT',
	GreatWorkCount = 1,
	NoUnhappfromXSpecialists = 1
WHERE BuildingClass = 'BUILDINGCLASS_ARTISTS_GUILD';

-- Musicians' Guild
UPDATE Buildings
SET
	GreatPeopleRateChange = 5,
	NoUnhappfromXSpecialists = 1
WHERE BuildingClass = 'BUILDINGCLASS_MUSICIANS_GUILD';

----------------------------------------------------------------------------
-- Power Plants
----------------------------------------------------------------------------

-- Common
INSERT INTO Building_ResourceQuantityRequirements
	(BuildingType, ResourceType, Cost)
SELECT
	Type, 'RESOURCE_ALUMINUM', 1
FROM Buildings
WHERE BuildingClass IN (
	('BUILDINGCLASS_SOLAR_PLANT'),
	('BUILDINGCLASS_WIND_PLANT'),
	('BUILDINGCLASS_TIDAL_PLANT')
);

INSERT INTO Helper
	(BuildingClassType, Yield)
VALUES
	('BUILDINGCLASS_HYDRO_PLANT', 10),
	('BUILDINGCLASS_SOLAR_PLANT', 10),
	('BUILDINGCLASS_WIND_PLANT', 10),
	('BUILDINGCLASS_TIDAL_PLANT', 10),
	('BUILDINGCLASS_NUCLEAR_PLANT', 20);

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, 'YIELD_PRODUCTION', b.Yield
FROM Buildings a, Helper b
WHERE a.BuildingClass = b.BuildingClassType;

DELETE FROM Helper;

INSERT INTO Helper
	(BuildingClassType, Yield)
VALUES
	('BUILDINGCLASS_HYDRO_PLANT', 15),
	('BUILDINGCLASS_SOLAR_PLANT', 15),
	('BUILDINGCLASS_WIND_PLANT', 15),
	('BUILDINGCLASS_TIDAL_PLANT', 15),
	('BUILDINGCLASS_NUCLEAR_PLANT', 33);

INSERT INTO Building_YieldModifiers
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, 'YIELD_PRODUCTION', b.Yield
FROM Buildings a, Helper b
WHERE a.BuildingClass = b.BuildingClassType;

DELETE FROM Helper;

-- Boost all future potential yield conversion processes too
INSERT INTO Building_YieldFromProcessModifier
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.Type, 10
FROM Buildings a, Yields b
WHERE a.BuildingClass IN (
	('BUILDINGCLASS_HYDRO_PLANT'),
	('BUILDINGCLASS_SOLAR_PLANT'),
	('BUILDINGCLASS_WIND_PLANT'),
	('BUILDINGCLASS_TIDAL_PLANT'),
	('BUILDINGCLASS_NUCLEAR_PLANT')
) AND b.ID < 8;

-- Hydroelectric Power Plant - Yields to Lake and River
UPDATE Buildings
SET
	PrereqTech = 'TECH_ECOLOGY',
	River = 0,
	FreshWater = 1
WHERE BuildingClass = 'BUILDINGCLASS_HYDRO_PLANT';

DELETE FROM Building_RiverPlotYieldChanges
WHERE BuildingType IN (
	SELECT Type FROM Buildings
	WHERE BuildingClass = 'BUILDINGCLASS_HYDRO_PLANT'
);

INSERT INTO Helper
	(YieldType)
VALUES
	('YIELD_FOOD'),
	('YIELD_PRODUCTION'),
	('YIELD_SCIENCE');

INSERT INTO Building_RiverPlotYieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, 3
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_HYDRO_PLANT';

INSERT INTO Building_LakePlotYieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, 3
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_HYDRO_PLANT';

DELETE FROM Helper;

-- Solar Power Plant - Yields to every Land tile, but 3x on Desert
INSERT INTO Helper
	(TerrainType, Yield)
VALUES
	('TERRAIN_GRASS', 1),
	('TERRAIN_PLAINS', 1),
	('TERRAIN_TUNDRA', 1),
	('TERRAIN_SNOW', 1),
	('TERRAIN_DESERT', 3);

INSERT INTO Helper2
	(YieldType)
VALUES
	('YIELD_PRODUCTION'),
	('YIELD_SCIENCE');

INSERT INTO Building_TerrainYieldChanges
	(BuildingType, TerrainType, YieldType, Yield)
SELECT
	a.Type, b.TerrainType, c.YieldType, b.Yield
FROM Buildings a, Helper b, Helper2 c
WHERE a.BuildingClass = 'BUILDINGCLASS_SOLAR_PLANT';

-- Wind Power Plant - Small Yields to every non-Mountain tile
INSERT INTO Helper2
	(YieldType)
VALUES
	('YIELD_CULTURE');

INSERT INTO Building_TerrainYieldChanges
	(BuildingType, TerrainType, YieldType, Yield)
SELECT
	a.Type, b.TerrainType, c.YieldType, 1
FROM Buildings a, Helper b, Helper2 c
WHERE a.BuildingClass = 'BUILDINGCLASS_WIND_PLANT';

INSERT INTO Building_LakePlotYieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, 1
FROM Buildings a, Helper2 b
WHERE a.BuildingClass = 'BUILDINGCLASS_WIND_PLANT';

INSERT INTO Building_SeaPlotYieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, 1
FROM Buildings a, Helper2 b
WHERE a.BuildingClass = 'BUILDINGCLASS_WIND_PLANT';

DELETE FROM Helper;
DELETE FROM Helper2;

-- Tidal Power Plant - Yields to all sea tiles
INSERT INTO Helper
	(YieldType)
VALUES
	('YIELD_FOOD'),
	('YIELD_PRODUCTION'),
	('YIELD_GOLD'),
	('YIELD_SCIENCE');

INSERT INTO Building_SeaPlotYieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, 2
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_TIDAL_PLANT';

DELETE FROM Helper;

-- Nuclear Power Plant - boosts Specialists instead of tiles
UPDATE Buildings
SET
	PrereqTech = 'TECH_ECOLOGY',
	GreatPeopleRateModifier = 25
WHERE BuildingClass = 'BUILDINGCLASS_NUCLEAR_PLANT';

INSERT INTO Helper
	(YieldType)
VALUES
	('YIELD_PRODUCTION'),
	('YIELD_GOLD'),
	('YIELD_SCIENCE'),
	('YIELD_CULTURE');

INSERT INTO Building_SpecialistYieldChangesLocal
	(BuildingType, SpecialistType, YieldType, Yield)
SELECT
	a.Type, c.Type, b.YieldType, 1
FROM Buildings a, Helper b, Specialists c
WHERE a.BuildingClass = 'BUILDINGCLASS_NUCLEAR_PLANT'
AND c.GreatPeopleUnitClass IS NOT NULL;

DELETE FROM Helper;

----------------------------------------------------------------------------
-- Other buildings
----------------------------------------------------------------------------

-- Well
INSERT INTO Helper
	(YieldType, Yield)
VALUES
	('YIELD_FOOD', 1),
	('YIELD_PRODUCTION', 2);

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, b.Yield
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_WELL';

DELETE FROM Helper;

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_PRODUCTION', 20
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_WELL';

-- Water Mill
UPDATE Buildings
SET PrereqTech = 'TECH_CONSTRUCTION' -- Masonry
WHERE BuildingClass = 'BUILDINGCLASS_WATERMILL';

INSERT INTO Helper
	(YieldType, Yield)
VALUES
	('YIELD_FOOD', 2),
	('YIELD_PRODUCTION', 3);

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, b.Yield
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_WATERMILL';

DELETE FROM Helper;

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_PRODUCTION', 25
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_WATERMILL';

-- Stone Works
UPDATE Buildings
SET
	PrereqTech = 'TECH_MASONRY', -- Construction
	ProhibitedCityTerrain = NULL,
	AllowsProductionTradeRoutes = 1,
	Happiness = 0
WHERE BuildingClass = 'BUILDINGCLASS_STONE_WORKS';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_PRODUCTION', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_STONE_WORKS';

INSERT INTO Helper
	(ResourceType)
VALUES
	('RESOURCE_SALT'),
	('RESOURCE_LAPIS'),
	('RESOURCE_AMBER'),
	('RESOURCE_JADE'),
	('RESOURCE_URANIUM');

INSERT INTO Building_LocalResourceOrs
	(BuildingType, ResourceType)
SELECT
	a.Type, b.ResourceType
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_STONE_WORKS';

DELETE FROM Helper;

-- Smokehouse
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE_LOCAL', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_LODGE';

INSERT INTO Building_YieldFromBorderGrowth
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_FOOD', 5
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_LODGE';

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
SELECT
	Type, 'IMPROVEMENT_CAMP', 'YIELD_PRODUCTION', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_LODGE';

-- Herbalist
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_FOOD', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_HERBALIST';

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
SELECT
	Type, 'IMPROVEMENT_PLANTATION', 'YIELD_PRODUCTION', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_HERBALIST';

INSERT INTO Building_FeatureYieldChanges
	(BuildingType, FeatureType, YieldType, Yield)
SELECT
	Type, 'FEATURE_MARSH', 'YIELD_PRODUCTION', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_HERBALIST';

INSERT INTO Helper
	(FeatureType)
VALUES
	('FEATURE_FOREST'),
	('FEATURE_JUNGLE');

INSERT INTO Building_YieldPerXFeatureTimes100
	(BuildingType, FeatureType, YieldType, Yield)
SELECT
	a.Type, b.FeatureType, 'YIELD_FOOD', 50
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_HERBALIST';

DELETE FROM Helper;

-- Stable
UPDATE Buildings
SET
	PrereqTech = 'TECH_CHIVALRY',
	CitySupplyFlat = 1
WHERE BuildingClass = 'BUILDINGCLASS_STABLE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_PRODUCTION', 3
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_STABLE';

UPDATE Building_UnitCombatProductionModifiers
SET Modifier = 33
WHERE BuildingType IN (
	SELECT Type FROM Buildings
	WHERE BuildingClass = 'BUILDINGCLASS_STABLE'
);

-- Windmill
UPDATE Buildings
SET
	Flat = 0,
	BuildingProductionModifier = 15
WHERE BuildingClass = 'BUILDINGCLASS_WINDMILL';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_PRODUCTION', 3
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_WINDMILL';

INSERT INTO Helper
	(YieldType)
VALUES
	('YIELD_PRODUCTION'),
	('YIELD_GOLD');

INSERT INTO Building_LakePlotYieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, 2
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_WINDMILL';

INSERT INTO Building_FeatureYieldChanges
	(BuildingType, FeatureType, YieldType, Yield)
SELECT
	a.Type, 'FEATURE_MARSH', b.YieldType, 2
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_WINDMILL';

DELETE FROM Helper;

INSERT INTO Helper
	(BuildingClassType)
VALUES
	('BUILDINGCLASS_GRANARY'),
	('BUILDINGCLASS_GROCER');

INSERT INTO Building_BuildingClassLocalYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
SELECT
	a.Type, b.BuildingClassType, 'YIELD_FOOD', 1
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_WINDMILL';

DELETE FROM Helper;

-- Baths
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_BATH';

INSERT INTO Helper
	(BuildingClassType)
VALUES
	('BUILDINGCLASS_AMPHITHEATER'),
	('BUILDINGCLASS_TEMPLE'),
	('BUILDINGCLASS_GARDEN');

INSERT INTO Helper2
	(YieldType)
VALUES
	('YIELD_GOLD'),
	('YIELD_CULTURE');

INSERT INTO Building_BuildingClassLocalYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
SELECT
	a.Type, b.BuildingClassType, c.YieldType, 1
FROM Buildings a, Helper b, Helper2 c
WHERE a.BuildingClass = 'BUILDINGCLASS_BATH';

DELETE FROM Helper;
DELETE FROM Helper2;

INSERT INTO Building_GoldenAgeYieldMod
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 10
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_BATH';

-- Coaling Station
INSERT INTO Building_ResourceQuantityRequirements
	(BuildingType, ResourceType, Cost)
SELECT
	Type, 'RESOURCE_COAL', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_COALING_STATION';

INSERT INTO Helper
	(YieldType, Yield)
VALUES
	('YIELD_PRODUCTION', 20),
	('YIELD_GOLD', 10);

INSERT INTO Building_YieldModifiers
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, b.Yield
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_COALING_STATION';

DELETE FROM Helper;

-- Refinery
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_PRODUCTION', 3
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_COAL_PLANT';

INSERT INTO Helper
	(ResourceType)
VALUES
	('RESOURCE_IRON'),
	('RESOURCE_COAL'),
	('RESOURCE_OIL');

INSERT INTO Building_ResourceQuantity
	(BuildingType, ResourceType, Quantity)
SELECT
	a.Type, b.ResourceType, 1
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_COAL_PLANT';

DELETE FROM Helper;

-- Recycling Center
UPDATE Buildings
SET PrereqTech = 'TECH_ELECTRONICS'
WHERE BuildingClass = 'BUILDINGCLASS_RECYCLING_CENTER';

-- Strategic Defense System
UPDATE Buildings
SET
	PrereqTech = 'TECH_NUCLEAR_FISSION',
	NukeInterceptionChance = 50,
	CityAirStrikeDefense = 10
WHERE BuildingClass = 'BUILDINGCLASS_BOMB_SHELTER';

-- Airport
UPDATE Buildings
SET
	PrereqTech = 'TECH_ROCKETRY',
	LandmarksTourismPercent = 0,
	GreatWorksTourismModifier = 0,
	CityAirStrikeDefense = 10
WHERE BuildingClass = 'BUILDINGCLASS_AIRPORT';

-- Courthouse
UPDATE Buildings
SET PrereqTech = 'TECH_PHILOSOPHY'
WHERE BuildingClass = 'BUILDINGCLASS_COURTHOUSE';

----------------------------------------------------------------------------
-- National Wonders
----------------------------------------------------------------------------

-- National Monument
UPDATE Buildings
SET GreatWorkSlotType = 'GREAT_WORK_SLOT_ART_ARTIFACT'
WHERE BuildingClass = 'BUILDINGCLASS_NATIONAL_EPIC';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_NATIONAL_EPIC';

INSERT INTO Building_YieldFromBirth
	(BuildingType, YieldType, Yield, IsEraScaling)
SELECT
	Type, 'YIELD_CULTURE', 15, 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_NATIONAL_EPIC';

INSERT INTO Building_YieldFromPolicyUnlock
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_GOLDEN_AGE_POINTS', 50
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_NATIONAL_EPIC';

-- Scrivener's Office
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_COURT_SCRIBE';

INSERT INTO Building_FreeUnits
	(BuildingType, UnitType, NumUnits)
SELECT
	Type, 'UNIT_EMISSARY', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_COURT_SCRIBE';

INSERT INTO Building_ResourceQuantity
	(BuildingType, ResourceType, Quantity)
SELECT
	Type, 'RESOURCE_PAPER', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_COURT_SCRIBE';

INSERT INTO Building_UnitCombatProductionModifiers
	(BuildingType, UnitCombatType, Modifier)
SELECT
	Type, 'UNITCOMBAT_DIPLOMACY', 10
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_COURT_SCRIBE';

INSERT INTO Building_YieldPerFriend
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_FOOD', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_COURT_SCRIBE';

INSERT INTO Helper
	(YieldType)
VALUES
	('YIELD_FOOD'),
	('YIELD_FAITH');

INSERT INTO Building_YieldPerAlly
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, 1
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_COURT_SCRIBE';

DELETE FROM Helper;

-- Heroic Epic
UPDATE Buildings
SET CitySupplyModifier = 10
WHERE BuildingClass = 'BUILDINGCLASS_HEROIC_EPIC';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_HEROIC_EPIC';

INSERT INTO Building_FreeUnits
	(BuildingType, UnitType, NumUnits)
SELECT
	Type, 'UNIT_WRITER', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_HEROIC_EPIC';

-- School of Philosophy
UPDATE Buildings
SET
	GreatWorkSlotType = 'GREAT_WORK_SLOT_LITERATURE',
	GreatWorkCount = 1,
	SpecialistType = 'SPECIALIST_SCIENTIST',
	GreatPeopleRateChange = 2,
	NoUnhappfromXSpecialists = 1,
	IlliteracyFlatReductionGlobal = 1
WHERE BuildingClass = 'BUILDINGCLASS_NATIONAL_COLLEGE';

INSERT INTO Helper
	(YieldType, Yield)
VALUES
	('YIELD_SCIENCE', 5),
	('YIELD_CULTURE', 1);

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, b.Yield
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_NATIONAL_COLLEGE';

DELETE FROM Helper;

INSERT INTO Building_GoldenAgeYieldMod
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 20
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_NATIONAL_COLLEGE';

-- Circus Maximus
UPDATE Buildings
SET
	PrereqTech = 'TECH_METAL_CASTING',
	UnmoddedHappiness = 0,
	Happiness = 2,
	BoredomFlatReduction = 1
WHERE BuildingClass = 'BUILDINGCLASS_CIRCUS_MAXIMUS';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_CIRCUS_MAXIMUS';

INSERT INTO Building_ResourceQuantity
	(BuildingType, ResourceType, Quantity)
VALUES
	('BUILDING_CIRCUS_MAXIMUS', 'RESOURCE_HORSE', 2);

INSERT INTO Helper
	(YieldType)
VALUES
	('YIELD_GOLD'),
	('YIELD_CULTURE');

INSERT INTO Building_WLTKDYieldMod
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, 10
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_CIRCUS_MAXIMUS';

DELETE FROM Helper;

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
SELECT
	Type, 'BUILDINGCLASS_COLOSSEUM', 'YIELD_GOLD', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_CIRCUS_MAXIMUS';

-- Oxford University
UPDATE Buildings
SET
	FreeTechs = 0,
	IlliteracyFlatReduction = 1
WHERE BuildingClass = 'BUILDINGCLASS_OXFORD_UNIVERSITY';

INSERT INTO Helper
	(YieldType, Yield)
VALUES
	('YIELD_SCIENCE', 3),
	('YIELD_CULTURE', 3);

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, b.Yield
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_OXFORD_UNIVERSITY';

DELETE FROM Helper;

INSERT INTO Building_FreeUnits
	(BuildingType, UnitType, NumUnits)
SELECT
	Type, 'UNIT_SCIENTIST', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_OXFORD_UNIVERSITY';

INSERT INTO Building_YieldFromTech
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 50
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_OXFORD_UNIVERSITY';

-- Grand Temple
UPDATE Buildings
SET
	HolyCity = 0,
	ReligiousPressureModifier = 0,
	GreatWorkSlotType = 'GREAT_WORK_SLOT_MUSIC',
	GreatWorkCount = 1,
	ReligiousUnrestFlatReduction = 1,
	ReligiousUnrestFlatReductionGlobal = 1
WHERE BuildingClass = 'BUILDINGCLASS_GRAND_TEMPLE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_FAITH', 6
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_GRAND_TEMPLE';

INSERT INTO Helper
	(YieldType, Yield)
VALUES
	('YIELD_GOLD', 1),
	('YIELD_FAITH', 2);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
SELECT
	a.Type, 'BUILDINGCLASS_TEMPLE', b.YieldType, b.Yield
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_GRAND_TEMPLE';

DELETE FROM Helper;

-- East India Company
UPDATE Buildings
SET
	ExtraLuxuries = 1,
	ResourceDiversityModifier = 25,
	PovertyFlatReductionGlobal = 1
WHERE BuildingClass = 'BUILDINGCLASS_NATIONAL_TREASURY';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_GOLD', 4
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_NATIONAL_TREASURY';

-- Ironworks
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_PRODUCTION', 10
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_IRONWORKS';

INSERT INTO Building_ResourceQuantity
	(BuildingType, ResourceType, Quantity)
SELECT
	Type, 'RESOURCE_IRON', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_IRONWORKS';

INSERT INTO Building_YieldFromConstruction
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 25
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_IRONWORKS';

-- Printing Press
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_PRINTING_PRESS';

INSERT INTO Building_ResourceQuantity
	(BuildingType, ResourceType, Quantity)
SELECT
	Type, 'RESOURCE_PAPER', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_PRINTING_PRESS';

INSERT INTO Building_UnitCombatProductionModifiers
	(BuildingType, UnitCombatType, Modifier)
SELECT
	Type, 'UNITCOMBAT_DIPLOMACY', 20
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_PRINTING_PRESS';

-- Royal Collection
UPDATE Buildings
SET
	CultureRateModifier = 10,
	BoredomFlatReductionGlobal = 1
WHERE BuildingClass = 'BUILDINGCLASS_HERMITAGE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 2
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_HERMITAGE';

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 25
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_HERMITAGE';

-- Foreign Bureau
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_FOREIGN_OFFICE';

INSERT INTO Building_ResourceQuantity
	(BuildingType, ResourceType, Quantity)
SELECT
	Type, 'RESOURCE_PAPER', 1
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_FOREIGN_OFFICE';

INSERT INTO Building_UnitCombatProductionModifiers
	(BuildingType, UnitCombatType, Modifier)
SELECT
	Type, 'UNITCOMBAT_DIPLOMACY', 20
FROM Buildings
WHERE BuildingClass = 'BUILDINGCLASS_FOREIGN_OFFICE';

-- National Intelligence Agency
UPDATE Buildings
SET
	PrereqTech = 'TECH_COMPUTERS',
	GlobalEspionageModifier = 0,
	SpyRankChange = 0,
	InstantSpyRankChange = 0,
	SpySecurityModifier = 10
WHERE BuildingClass = 'BUILDINGCLASS_INTELLIGENCE_AGENCY';

INSERT INTO Helper
	(YieldType)
VALUES
	('YIELD_GOLD'),
	('YIELD_SCIENCE');

INSERT INTO Building_YieldFromSpyAttack
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, 100
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_INTELLIGENCE_AGENCY';

INSERT INTO Building_YieldFromSpyDefenseOrID
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, 100
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_INTELLIGENCE_AGENCY';

INSERT INTO Building_YieldFromSpyRigElection
	(BuildingType, YieldType, Yield)
SELECT
	a.Type, b.YieldType, 50
FROM Buildings a, Helper b
WHERE a.BuildingClass = 'BUILDINGCLASS_INTELLIGENCE_AGENCY';

DELETE FROM Helper;

----------------------------------------------------------------------------
-- Common inserts and updates for BuildingClass
----------------------------------------------------------------------------

INSERT INTO Helper
	(BuildingClassType, Yield)
VALUES
	('BUILDINGCLASS_OPERA_HOUSE', 1),
	('BUILDINGCLASS_GALLERY', 1),
	('BUILDINGCLASS_MUSEUM', 1),
	('BUILDINGCLASS_BROADCAST_TOWER', 2);

INSERT INTO Building_GreatWorkYieldChangesLocal
	(BuildingType, YieldType, Yield)
SELECT a.Type, 'YIELD_TOURISM', b.Yield
FROM Buildings a, Helper b
WHERE a.BuildingClass = b.BuildingClassType;

DELETE FROM Helper;

INSERT INTO Helper
	(BuildingClassType, YieldType, Yield)
VALUES
	('BUILDINGCLASS_LIGHTHOUSE', 'YIELD_FOOD', 4),
	('BUILDINGCLASS_COALING_STATION', 'YIELD_FOOD', 6),
	('BUILDINGCLASS_STABLE', 'YIELD_PRODUCTION', 2),
	('BUILDINGCLASS_WORKSHOP', 'YIELD_PRODUCTION', 4);

INSERT INTO Building_YieldFromInternalTR
	(BuildingType, YieldType, Yield)
SELECT a.Type, b.YieldType, b.Yield
FROM Buildings a, Helper b
WHERE a.BuildingClass = b.BuildingClassType;

DELETE FROM Helper;

-- Building prereq
INSERT INTO Helper
	(BuildingClassType, RequiredBuildingClass)
VALUES
	('BUILDINGCLASS_AMPHITHEATER', 'BUILDINGCLASS_MONUMENT'),
	('BUILDINGCLASS_OPERA_HOUSE', 'BUILDINGCLASS_AMPHITHEATER'),
	('BUILDINGCLASS_GALLERY', 'BUILDINGCLASS_AMPHITHEATER'),
	('BUILDINGCLASS_MUSEUM', 'BUILDINGCLASS_GALLERY'),
	('BUILDINGCLASS_BROADCAST_TOWER', 'BUILDINGCLASS_OPERA_HOUSE'),
	('BUILDINGCLASS_TEMPLE', 'BUILDINGCLASS_SHRINE'),
	('BUILDINGCLASS_AQUEDUCT', 'BUILDINGCLASS_GRANARY'),
	('BUILDINGCLASS_GARDEN', 'BUILDINGCLASS_AQUEDUCT'),
	('BUILDINGCLASS_GROCER', 'BUILDINGCLASS_AQUEDUCT'),
	('BUILDINGCLASS_STOCKYARD', 'BUILDINGCLASS_GROCER'),
	('BUILDINGCLASS_HOSPITAL', 'BUILDINGCLASS_GROCER'),
	('BUILDINGCLASS_MEDICAL_LAB', 'BUILDINGCLASS_HOSPITAL'),
	('BUILDINGCLASS_LIBRARY', 'BUILDINGCLASS_GROVE'),
	('BUILDINGCLASS_UNIVERSITY', 'BUILDINGCLASS_LIBRARY'),
	('BUILDINGCLASS_PUBLIC_SCHOOL', 'BUILDINGCLASS_UNIVERSITY'),
	('BUILDINGCLASS_LABORATORY', 'BUILDINGCLASS_PUBLIC_SCHOOL'),
	('BUILDINGCLASS_CARAVANSARY', 'BUILDINGCLASS_MARKET'),
	('BUILDINGCLASS_MINT', 'BUILDINGCLASS_MARKET'),
	('BUILDINGCLASS_BANK', 'BUILDINGCLASS_MARKET'),
	('BUILDINGCLASS_STOCK_EXCHANGE', 'BUILDINGCLASS_BANK'),
	('BUILDINGCLASS_ARMORY', 'BUILDINGCLASS_BARRACKS'),
	('BUILDINGCLASS_MILITARY_ACADEMY', 'BUILDINGCLASS_ARMORY'),
	('BUILDINGCLASS_CASTLE', 'BUILDINGCLASS_WALLS'),
	('BUILDINGCLASS_FORTRESS', 'BUILDINGCLASS_CASTLE'),
	('BUILDINGCLASS_ARSENAL', 'BUILDINGCLASS_FORTRESS'),
	('BUILDINGCLASS_MILITARY_BASE', 'BUILDINGCLASS_ARSENAL'),
	('BUILDINGCLASS_WORKSHOP', 'BUILDINGCLASS_FORGE'),
	('BUILDINGCLASS_FACTORY', 'BUILDINGCLASS_WORKSHOP'),
	('BUILDINGCLASS_SPACESHIP_FACTORY', 'BUILDINGCLASS_FACTORY'),
	('BUILDINGCLASS_POLICE_STATION', 'BUILDINGCLASS_CONSTABLE'),
	('BUILDINGCLASS_HARBOR', 'BUILDINGCLASS_LIGHTHOUSE'),
	('BUILDINGCLASS_SEAPORT', 'BUILDINGCLASS_HARBOR'),
	('BUILDINGCLASS_CIRCUS', 'BUILDINGCLASS_COLOSSEUM'),
	('BUILDINGCLASS_THEATRE', 'BUILDINGCLASS_CIRCUS'),
	('BUILDINGCLASS_STADIUM', 'BUILDINGCLASS_THEATRE'),
	('BUILDINGCLASS_WIRE_SERVICE', 'BUILDINGCLASS_CHANCERY'),
	-- National Wonders
	('BUILDINGCLASS_NATIONAL_EPIC', 'BUILDINGCLASS_MONUMENT'),
	('BUILDINGCLASS_HEROIC_EPIC', 'BUILDINGCLASS_BARRACKS'),
	('BUILDINGCLASS_NATIONAL_COLLEGE', 'BUILDINGCLASS_LIBRARY'),
	('BUILDINGCLASS_IRONWORKS', 'BUILDINGCLASS_FORGE'),
	('BUILDINGCLASS_CIRCUS_MAXIMUS', 'BUILDINGCLASS_COLOSSEUM'),
	('BUILDINGCLASS_NATIONAL_TREASURY', 'BUILDINGCLASS_MINT'),
	('BUILDINGCLASS_GRAND_TEMPLE', 'BUILDINGCLASS_TEMPLE'),
	('BUILDINGCLASS_OXFORD_UNIVERSITY', 'BUILDINGCLASS_UNIVERSITY'),
	('BUILDINGCLASS_PRINTING_PRESS', 'BUILDINGCLASS_CHANCERY'),
	('BUILDINGCLASS_HERMITAGE', 'BUILDINGCLASS_GALLERY'),
	('BUILDINGCLASS_FOREIGN_OFFICE', 'BUILDINGCLASS_WIRE_SERVICE'),
	('BUILDINGCLASS_INTELLIGENCE_AGENCY', 'BUILDINGCLASS_POLICE_STATION'),
	('BUILDINGCLASS_PALACE_SCIENCE_CULTURE', 'BUILDINGCLASS_PUBLIC_SCHOOL'),
	('BUILDINGCLASS_FINANCE_CENTER', 'BUILDINGCLASS_STOCK_EXCHANGE'),
	('BUILDINGCLASS_EHRENHALLE', 'BUILDINGCLASS_MILITARY_ACADEMY');

INSERT INTO Building_ClassesNeededInCity
	(BuildingType, BuildingClassType)
SELECT
	a.Type, b.RequiredBuildingClass
FROM Buildings a, Helper b
WHERE a.BuildingClass = b.BuildingClassType;

DELETE FROM Helper;

-- Defense, HP and Damage Reduction
CREATE TEMP TABLE BuildingsTemp (
	BuildingClassTemp TEXT,
	DefenseTemp INTEGER,
	ExtraCityHitPointsTemp INTEGER,
	DamageReductionFlatTemp INTEGER
);

INSERT INTO BuildingsTemp
VALUES
	('BUILDINGCLASS_WALLS', 600, 100, 0),
	('BUILDINGCLASS_CASTLE', 800, 0, 2),
	('BUILDINGCLASS_FORTRESS', 1000, 0, 2),
	('BUILDINGCLASS_ARSENAL', 1200, 300, 0),
	('BUILDINGCLASS_MILITARY_BASE', 2400, 200, 0),
	('BUILDINGCLASS_BOMB_SHELTER', 500, 0, 0),
	-- Capital
	('BUILDINGCLASS_PALACE', 400, 0, 0),
	-- Tradition
	('BUILDINGCLASS_CAPITAL_ENGINEER', 200, 50, 0),
	-- World Wonders
	('BUILDINGCLASS_RED_FORT', 600, 100, 0),
	-- Beliefs
	('BUILDINGCLASS_ORDER', 300, 0, 0);

UPDATE Buildings
SET
	Defense = (SELECT DefenseTemp FROM BuildingsTemp WHERE BuildingClassTemp = BuildingClass),
	ExtraCityHitPoints = (SELECT ExtraCityHitPointsTemp FROM BuildingsTemp WHERE BuildingClassTemp = BuildingClass),
	DamageReductionFlat = (SELECT DamageReductionFlatTemp FROM BuildingsTemp WHERE BuildingClassTemp = BuildingClass)
WHERE EXISTS (SELECT 1 FROM BuildingsTemp WHERE BuildingClassTemp = BuildingClass);

DROP TABLE BuildingsTemp;
DROP TABLE Helper;
DROP TABLE Helper2;
