----------------------------------------------------------------------------
-- Policy buildings
-- From here on, the buildings won't have replacements for sure,
-- so updated directly by Type
----------------------------------------------------------------------------
UPDATE Buildings
SET
	UnlockedByBelief = 0,
	Description = 'TXT_KEY_BUILDING_MONASTERY',
	PolicyType = 'POLICY_PIETY'
WHERE Type = 'BUILDING_MONASTERY';

UPDATE Buildings
SET
	PolicyType = 'POLICY_SECULARISM',
	Mountain = 0,
	SpecialistType = 'SPECIALIST_SCIENTIST',
	SpecialistCount = 2
WHERE Type = 'BUILDING_OBSERVATORY';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_PALACE_TREASURY', 'YIELD_GOLD', 4),
	('BUILDING_PALACE_GARDEN', 'YIELD_FOOD', 5),
	('BUILDING_CAPITAL_ENGINEER', 'YIELD_PRODUCTION', 3),
	('BUILDING_PALACE_COURT_CHAPEL', 'YIELD_FAITH', 3),
	('BUILDING_PALACE_ASTROLOGER', 'YIELD_SCIENCE', 3),
	('BUILDING_MONASTERY', 'YIELD_FOOD', 3),
	('BUILDING_MONASTERY', 'YIELD_SCIENCE', 3),
	('BUILDING_MONASTERY', 'YIELD_FAITH', 2),
	('BUILDING_OBSERVATORY', 'YIELD_SCIENCE', 6),
	('BUILDING_PALACE_SCIENCE_CULTURE', 'YIELD_CULTURE', 4),
	('BUILDING_FINANCE_CENTER', 'YIELD_CULTURE', 4),
	('BUILDING_EHRENHALLE', 'YIELD_CULTURE', 4),
	('BUILDING_EHRENHALLE', 'YIELD_TOURISM', 15);

INSERT INTO Building_YieldModifiers
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_THRONE_ROOM', 'YIELD_FOOD', 10),
	('BUILDING_THRONE_ROOM', 'YIELD_PRODUCTION', 10),
	('BUILDING_THRONE_ROOM', 'YIELD_GOLD', 10),
	('BUILDING_THRONE_ROOM', 'YIELD_SCIENCE', 10),
	('BUILDING_THRONE_ROOM', 'YIELD_FAITH', 10),
	('BUILDING_PALACE_SCIENCE_CULTURE', 'YIELD_SCIENCE', 5),
	('BUILDING_FINANCE_CENTER', 'YIELD_FOOD', 5),
	('BUILDING_FINANCE_CENTER', 'YIELD_GOLD', 5),
	('BUILDING_EHRENHALLE', 'YIELD_PRODUCTION', 5);

INSERT INTO Building_YieldPerXTerrainTimes100
	(BuildingType, TerrainType, YieldType, Yield)
VALUES
	('BUILDING_OBSERVATORY', 'TERRAIN_MOUNTAIN', 'YIELD_SCIENCE', 100);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_PALACE_TREASURY', 'BUILDINGCLASS_GARDEN', 'YIELD_CULTURE', 2),
	('BUILDING_PALACE_TREASURY', 'BUILDINGCLASS_BATH', 'YIELD_CULTURE', 2),
	('BUILDING_PALACE_TREASURY', 'BUILDINGCLASS_MONUMENT', 'YIELD_CULTURE', 2),
	('BUILDING_PALACE_ASTROLOGER', 'BUILDINGCLASS_GROVE', 'YIELD_SCIENCE', 1),
	('BUILDING_PALACE_ASTROLOGER', 'BUILDINGCLASS_LODGE', 'YIELD_SCIENCE', 1),
	('BUILDING_PALACE_ASTROLOGER', 'BUILDINGCLASS_HERBALIST', 'YIELD_SCIENCE', 1);

----------------------------------------------------------------------------
-- Religious Buildings
----------------------------------------------------------------------------

-- Cathedral
UPDATE Buildings
SET
	Help = 'TXT_KEY_BUILDING_CATHEDRAL_HELP',
	Happiness = 0,
	GreatWorkSlotType = NULL,
	GreatWorkCount = 0,
	ReligiousPressureModifier = 25,
	PovertyFlatReduction = 1
WHERE Type = 'BUILDING_CATHEDRAL';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CATHEDRAL', 'YIELD_GOLD', 2),
	('BUILDING_CATHEDRAL', 'YIELD_FAITH', 2);

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
VALUES
	('BUILDING_CATHEDRAL', 'IMPROVEMENT_FARM', 'YIELD_GOLD', 1),
	('BUILDING_CATHEDRAL', 'IMPROVEMENT_PASTURE', 'YIELD_GOLD', 1),
	('BUILDING_CATHEDRAL', 'IMPROVEMENT_QUARRY', 'YIELD_GOLD', 1);

INSERT INTO Building_YieldFromBorderGrowth
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CATHEDRAL', 'YIELD_GOLD', 10);

-- Mosque
UPDATE Buildings
SET
	Help = 'TXT_KEY_BUILDING_MOSQUE_HELP',
	Happiness = 0,
	ReligiousPressureModifier = 25,
	IlliteracyFlatReduction = 1
WHERE Type = 'BUILDING_MOSQUE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MOSQUE', 'YIELD_SCIENCE', 2),
	('BUILDING_MOSQUE', 'YIELD_FAITH', 3);

INSERT INTO Building_GoldenAgeYieldMod
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MOSQUE', 'YIELD_CULTURE', 15);

-- Pagoda
UPDATE Buildings
SET
	Help = 'TXT_KEY_BUILDING_PAGODA_HELP',
	Happiness = 0,
	BoredomFlatReduction = 1,
	ReligiousUnrestFlatReduction = 2
WHERE Type = 'BUILDING_PAGODA';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_PAGODA', 'YIELD_FAITH', 2);

INSERT INTO Building_YieldChangesPerReligion
	(BuildingType, YieldType, Yield)
SELECT
	'BUILDING_PAGODA', Type, 100
FROM Yields
WHERE ID < 6; -- "All" yields

-- Stupa
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_STUPA', 'YIELD_FAITH', 3),
	('BUILDING_STUPA', 'YIELD_GOLDEN_AGE_POINTS', 3),
	('BUILDING_STUPA', 'YIELD_TOURISM', 5);

-- Church
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CHURCH', 'YIELD_CULTURE', 2),
	('BUILDING_CHURCH', 'YIELD_FAITH', 4);

INSERT INTO Building_GreatWorkYieldChangesLocal
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CHURCH', 'YIELD_FAITH', 1);

-- Mandir
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MANDIR', 'YIELD_FOOD', 2),
	('BUILDING_MANDIR', 'YIELD_FAITH', 3);

INSERT INTO Building_YieldFromBirth
	(BuildingType, YieldType, Yield, IsEraScaling)
SELECT
	'BUILDING_MANDIR', Type, 5, 1
FROM Yields
WHERE ID < 6; -- "All" yields

-- Synagogue
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SYNAGOGUE', 'YIELD_PRODUCTION', 3),
	('BUILDING_SYNAGOGUE', 'YIELD_FAITH', 2);

INSERT INTO Building_WLTKDYieldMod
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SYNAGOGUE', 'YIELD_SCIENCE', 10);

-- Order
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_ORDER', 'YIELD_GOLD', 3),
	('BUILDING_ORDER', 'YIELD_FAITH', 2);

INSERT INTO Building_YieldFromUnitProduction
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_ORDER', 'YIELD_FAITH', 10);

-- Teocalli
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TEOCALLI', 'YIELD_FAITH', 2);

INSERT INTO Building_YieldFromVictoryGlobal
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TEOCALLI', 'YIELD_FAITH', 5);

INSERT INTO Building_DomainFreeExperiences
	(BuildingType, DomainType, Experience)
VALUES
	('BUILDING_TEOCALLI', 'DOMAIN_LAND', 15),
	('BUILDING_TEOCALLI', 'DOMAIN_SEA', 15),
	('BUILDING_TEOCALLI', 'DOMAIN_AIR', 15);

-- Gurdwara
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_GURDWARA', 'YIELD_FOOD', 3),
	('BUILDING_GURDWARA', 'YIELD_FAITH', 2);

INSERT INTO Building_YieldModifiers
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_GURDWARA', 'YIELD_FOOD', 10);

----------------------------------------------------------------------------
-- Reformation Buildings
----------------------------------------------------------------------------

-- Common
UPDATE Buildings
SET
	HolyCity = 1,
	GlobalFollowerPopRequired = 15,
	UnlockedByBelief = 1,
	ReligiousPressureModifier = 25,
	ConversionModifier = -20,
	ReligiousUnrestFlatReduction = 1,
	FaithToVotes = 10
WHERE IsReformation = 1;

INSERT INTO Building_ImprovementYieldChangesGlobal
	(BuildingType, ImprovementType, YieldType, Yield)
VALUES
	('BUILDING_MAUSOLEUM', 'IMPROVEMENT_HOLY_SITE', 'YIELD_FAITH', 5),
	('BUILDING_HEAVENLY_THRONE', 'IMPROVEMENT_HOLY_SITE', 'YIELD_GOLDEN_AGE_POINTS', 5),
	('BUILDING_GREAT_ALTAR', 'IMPROVEMENT_HOLY_SITE', 'YIELD_PRODUCTION', 5),
	('BUILDING_RELIGIOUS_LIBRARY', 'IMPROVEMENT_HOLY_SITE', 'YIELD_SCIENCE', 5),
	('BUILDING_DIVINE_COURT', 'IMPROVEMENT_HOLY_SITE', 'YIELD_CULTURE', 5),
	('BUILDING_SACRED_GARDEN', 'IMPROVEMENT_HOLY_SITE', 'YIELD_FOOD', 5),
	('BUILDING_HOLY_COUNCIL', 'IMPROVEMENT_HOLY_SITE', 'YIELD_SCIENCE', 5),
	('BUILDING_APOSTOLIC_PALACE', 'IMPROVEMENT_HOLY_SITE', 'YIELD_TOURISM', 5),
	('BUILDING_GRAND_OSSUARY', 'IMPROVEMENT_HOLY_SITE', 'YIELD_GOLD', 5);

-- Mausoleum
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MAUSOLEUM', 'YIELD_FAITH', 5);

INSERT INTO Building_YieldFromDeath
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MAUSOLEUM', 'YIELD_FAITH', 30);

-- Celestial Throne
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_HEAVENLY_THRONE', 'YIELD_FOOD', 2),
	('BUILDING_HEAVENLY_THRONE', 'YIELD_PRODUCTION', 2),
	('BUILDING_HEAVENLY_THRONE', 'YIELD_GOLD', 2),
	('BUILDING_HEAVENLY_THRONE', 'YIELD_SCIENCE', 2),
	('BUILDING_HEAVENLY_THRONE', 'YIELD_CULTURE', 2),
	('BUILDING_HEAVENLY_THRONE', 'YIELD_FAITH', 2);

-- Great Altar
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_GREAT_ALTAR', 'YIELD_FAITH', 5);

-- Chartarium
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_RELIGIOUS_LIBRARY', 'YIELD_SCIENCE', 4);

INSERT INTO Building_ThemingBonuses
	(BuildingType, Description, RequiresSamePlayer)
VALUES
	('BUILDING_RELIGIOUS_LIBRARY', 'TXT_KEY_THEMING_BONUS_RELIGIOUS_LIBRARY', 1);

-- Divine Court
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_DIVINE_COURT', 'YIELD_GOLD', 6),
	('BUILDING_DIVINE_COURT', 'YIELD_FAITH', 3);

-- Sacred Garden
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SACRED_GARDEN', 'YIELD_CULTURE', 5),
	('BUILDING_SACRED_GARDEN', 'YIELD_FAITH', 3);

-- Holy Council
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_HOLY_COUNCIL', 'YIELD_FOOD', 5),
	('BUILDING_HOLY_COUNCIL', 'YIELD_FAITH', 4);

-- Apostolic Palace
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_APOSTOLIC_PALACE', 'YIELD_FAITH', 4),
	('BUILDING_APOSTOLIC_PALACE', 'YIELD_GOLDEN_AGE_POINTS', 4);

-- Grand Ossuary
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_GRAND_OSSUARY', 'YIELD_FAITH', 10);

----------------------------------------------------------------------------
-- World Wonders
----------------------------------------------------------------------------

-- Stonehenge
UPDATE Buildings
SET
	Help = 'TXT_KEY_BUILDING_STONEHENGE_HELP',
	PrereqTech = 'TECH_THE_WHEEL',
	FreeBuildingThisCity = 'BUILDINGCLASS_GROVE'
WHERE Type = 'BUILDING_STONEHENGE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_STONEHENGE', 'YIELD_CULTURE', 1),
	('BUILDING_STONEHENGE', 'YIELD_FAITH', 2);

INSERT INTO Building_InstantYield
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_STONEHENGE', 'YIELD_FAITH', 60);

-- Pyramids
UPDATE Buildings
SET
	Help = 'TXT_KEY_WONDER_PYRAMIDS_HELP',
	PolicyBranchType = NULL,
	PrereqTech = 'TECH_MINING',
	WorkerSpeedModifier = 0
WHERE Type = 'BUILDING_PYRAMID';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_PYRAMID', 'YIELD_CULTURE', 1);

UPDATE Building_FreeUnits
SET UnitType = 'UNIT_SETTLER', NumUnits = 1 -- Venice gets a Merchant of Venice instead
WHERE BuildingType = 'BUILDING_PYRAMID';

INSERT INTO Building_YieldFromGPExpend
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_PYRAMID', 'YIELD_GOLDEN_AGE_POINTS', 50);

-- Petra
UPDATE Buildings
SET PrereqTech = 'TECH_HORSEBACK_RIDING' -- Trade
WHERE Type = 'BUILDING_PETRA';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_PETRA', 'YIELD_CULTURE', 2);

DELETE FROM Building_TerrainYieldChanges
WHERE BuildingType = 'BUILDING_PETRA';

DELETE FROM Building_FeatureYieldChanges
WHERE BuildingType = 'BUILDING_PETRA';

INSERT INTO Building_TerrainYieldChanges
	(BuildingType, TerrainType, YieldType, Yield)
VALUES
	('BUILDING_PETRA', 'TERRAIN_DESERT', 'YIELD_GOLD', 1);

-- Temple of Artemis
UPDATE Buildings
SET
	PrereqTech = 'TECH_CALENDAR',
	FreeBuildingThisCity = 'BUILDINGCLASS_HERBALIST',
	NoUnhappfromXSpecialists = 2
WHERE Type = 'BUILDING_TEMPLE_ARTEMIS';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TEMPLE_ARTEMIS', 'YIELD_CULTURE', 1);

UPDATE Building_UnitCombatProductionModifiers
SET Modifier = 25
WHERE BuildingType = 'BUILDING_TEMPLE_ARTEMIS';

-- Mausoleum of Halicarnassus
UPDATE Buildings
SET
	GreatPersonExpendGold = 0,
	FreeBuildingThisCity = 'BUILDINGCLASS_STONE_WORKS',
	WLTKDTurns = 20
WHERE Type = 'BUILDING_MAUSOLEUM_HALICARNASSUS';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MAUSOLEUM_HALICARNASSUS', 'YIELD_CULTURE', 2);

INSERT INTO Building_WLTKDYieldMod
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MAUSOLEUM_HALICARNASSUS', 'YIELD_PRODUCTION', 10),
	('BUILDING_MAUSOLEUM_HALICARNASSUS', 'YIELD_GOLD', 10),
	('BUILDING_MAUSOLEUM_HALICARNASSUS', 'YIELD_SCIENCE', 10);

-- Statue of Zeus
UPDATE Buildings
SET
	PolicyBranchType = NULL,
	FreeBuildingThisCity = 'BUILDINGCLASS_BARRACKS',
	DistressFlatReduction = 1
WHERE Type = 'BUILDING_STATUE_ZEUS';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_STATUE_ZEUS', 'YIELD_CULTURE', 1),
	('BUILDING_STATUE_ZEUS', 'YIELD_FAITH', 1);

-- Great Lighthouse
UPDATE Buildings
SET FreePromotion = 'PROMOTION_NAVIGATOR_2'
WHERE Type = 'BUILDING_GREAT_LIGHTHOUSE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_GREAT_LIGHTHOUSE', 'YIELD_PRODUCTION', 2),
	('BUILDING_GREAT_LIGHTHOUSE', 'YIELD_CULTURE', 1);

-- Great Library
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_GREAT_LIBRARY', 'YIELD_CULTURE', 1),
	('BUILDING_GREAT_LIBRARY', 'YIELD_SCIENCE', 3);

-- Hanging Gardens
UPDATE Buildings SET PolicyBranchType = NULL WHERE Type = 'BUILDING_HANGING_GARDEN';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_HANGING_GARDEN', 'YIELD_FOOD', 10),
	('BUILDING_HANGING_GARDEN', 'YIELD_CULTURE', 1);

-- Roman Forum
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_FORUM', 'YIELD_CULTURE', 1);

INSERT INTO Building_FreeUnits
	(BuildingType, UnitType, NumUnits)
VALUES
	('BUILDING_FORUM', 'UNIT_GREAT_DIPLOMAT', 1);

INSERT INTO Building_ResourceQuantity
	(BuildingType, ResourceType, Quantity)
VALUES
	('BUILDING_FORUM', 'RESOURCE_PAPER', 1);

INSERT INTO Building_UnitCombatProductionModifiers
	(BuildingType, UnitCombatType, Modifier)
VALUES
	('BUILDING_FORUM', 'UNITCOMBAT_DIPLOMACY', 20);

-- Terracotta Army
UPDATE Buildings
SET
	InstantMilitaryIncrease = 0,
	WorkerSpeedModifier = 25,
	CitySupplyFlat = 5
WHERE Type = 'BUILDING_TERRACOTTA_ARMY';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TERRACOTTA_ARMY', 'YIELD_CULTURE', 1);

INSERT INTO Building_YieldFromVictoryGlobal
	(BuildingType, YieldType, Yield, IsEraScaling)
VALUES
	('BUILDING_TERRACOTTA_ARMY', 'YIELD_CULTURE', 10, 1);

-- Parthenon
UPDATE Buildings
SET
	GreatWorkCount = 2,
	ThemingBonusHelp = 'TXT_KEY_PARTHENON_THEMING_BONUS_HELP',
	BoredomFlatReduction = 1
WHERE Type = 'BUILDING_PARTHENON';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_PARTHENON', 'YIELD_CULTURE', 2);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_PARTHENON', 'BUILDINGCLASS_AMPHITHEATER', 'YIELD_SCIENCE', 1),
	('BUILDING_PARTHENON', 'BUILDINGCLASS_AMPHITHEATER', 'YIELD_CULTURE', 1);

INSERT INTO Building_ThemingBonuses
	(BuildingType, Description, RequiresOwner)
VALUES
	('BUILDING_PARTHENON', 'TXT_KEY_THEMING_BONUS_BUILDING_PARTHENON', 1);

-- Oracle
UPDATE Buildings
SET
	FreePolicies = 0,
	FreeBuildingThisCity = 'BUILDINGCLASS_TEMPLE',
	IlliteracyFlatReduction = 1
WHERE Type = 'BUILDING_ORACLE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_ORACLE', 'YIELD_SCIENCE', 1),
	('BUILDING_ORACLE', 'YIELD_CULTURE', 1);

INSERT INTO Building_InstantYield
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_ORACLE', 'YIELD_SCIENCE', 400),
	('BUILDING_ORACLE', 'YIELD_CULTURE', 400);

-- Angkor Wat
UPDATE Buildings
SET
	PrereqTech = 'TECH_CURRENCY',
	GlobalPlotCultureCostModifier = 0,
	BorderGrowthRateIncreaseGlobal = 40,
	FreeBuildingThisCity = 'BUILDINGCLASS_MANDIR'
WHERE Type = 'BUILDING_ANGKOR_WAT';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_ANGKOR_WAT', 'YIELD_CULTURE', 1),
	('BUILDING_ANGKOR_WAT', 'YIELD_FAITH', 1);

-- Great Wall
UPDATE Buildings
SET
	ObsoleteTech = 'TECH_GUNPOWDER',
	CitySupplyFlat = 3
WHERE Type = 'BUILDING_GREAT_WALL';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_GREAT_WALL', 'YIELD_CULTURE', 1);

INSERT INTO Building_FreeUnits
	(BuildingType, UnitType, NumUnits)
VALUES
	('BUILDING_GREAT_WALL', 'UNIT_GREAT_GENERAL', 1);

-- Colossus
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_COLOSSUS', 'YIELD_GOLD', 5),
	('BUILDING_COLOSSUS', 'YIELD_CULTURE', 1);

-- University of Sankore
UPDATE Buildings
SET
	PolicyBranchType = NULL,
	PolicyType = 'POLICY_TRADITION_FINISHER',
	PrereqTech = 'TECH_EDUCATION',
	ExtraMissionarySpreads = 0,
	SpecialistType = 'SPECIALIST_SCIENTIST'
WHERE Type = 'BUILDING_MOSQUE_OF_DJENNE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MOSQUE_OF_DJENNE', 'YIELD_SCIENCE', 1),
	('BUILDING_MOSQUE_OF_DJENNE', 'YIELD_CULTURE', 1),
	('BUILDING_MOSQUE_OF_DJENNE', 'YIELD_FAITH', 1);

INSERT INTO Building_YieldFromGPExpend
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MOSQUE_OF_DJENNE', 'YIELD_SCIENCE', 50);

-- Hagia Sophia
UPDATE Buildings
SET
	FreeBuildingThisCity = 'BUILDINGCLASS_CHURCH',
	ExtraMissionaryStrengthGlobal = 25
WHERE Type = 'BUILDING_HAGIA_SOPHIA';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_HAGIA_SOPHIA', 'YIELD_CULTURE', 2),
	('BUILDING_HAGIA_SOPHIA', 'YIELD_FAITH', 1);

-- Borobudur
UPDATE Buildings
SET
	FreeBuildingThisCity = 'BUILDINGCLASS_STUPA',
	ExtraMissionarySpreadsGlobal = 1
WHERE Type = 'BUILDING_BOROBUDUR';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BOROBUDUR', 'YIELD_CULTURE', 1),
	('BUILDING_BOROBUDUR', 'YIELD_FAITH', 5);

UPDATE Building_FreeUnits SET NumUnits = 2 WHERE BuildingType = 'BUILDING_BOROBUDUR';

-- Alhambra
UPDATE Buildings
SET
	PolicyType = 'POLICY_HONOR_FINISHER',
	TrainedFreePromotion = NULL,
	CultureRateModifier = 10,
	FreePromotion = 'PROMOTION_ALHAMBRA'
WHERE Type = 'BUILDING_ALHAMBRA';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_ALHAMBRA', 'YIELD_CULTURE', 1);

-- Machu Picchu
UPDATE Buildings
SET
	PrereqTech = 'TECH_PHYSICS',
	CityConnectionTradeRouteModifier = 15
WHERE Type = 'BUILDING_MACHU_PICHU';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MACHU_PICHU', 'YIELD_GOLD', 5),
	('BUILDING_MACHU_PICHU', 'YIELD_CULTURE', 1),
	('BUILDING_MACHU_PICHU', 'YIELD_FAITH', 2);

INSERT INTO Building_YieldPerXTerrainTimes100
	(BuildingType, TerrainType, YieldType, Yield)
VALUES
	('BUILDING_MACHU_PICHU', 'TERRAIN_MOUNTAIN', 'YIELD_FOOD', 100),
	('BUILDING_MACHU_PICHU', 'TERRAIN_MOUNTAIN', 'YIELD_PRODUCTION', 100),
	('BUILDING_MACHU_PICHU', 'TERRAIN_MOUNTAIN', 'YIELD_CULTURE', 100),
	('BUILDING_MACHU_PICHU', 'TERRAIN_MOUNTAIN', 'YIELD_FAITH', 100);

-- Forbidden Palace
UPDATE Buildings
SET
	PolicyBranchType = NULL,
	UnhappinessModifier = 0,
	SingleLeagueVotes = 0,
	PrereqTech = 'TECH_CIVIL_SERVICE',
	PolicyType = 'POLICY_LIBERTY_FINISHER',
	PovertyFlatReduction = 1
WHERE Type = 'BUILDING_FORBIDDEN_PALACE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_FORBIDDEN_PALACE', 'YIELD_CULTURE', 1);

INSERT INTO Building_HurryModifiers
	(BuildingType, HurryType, HurryCostModifier)
VALUES
	('BUILDING_FORBIDDEN_PALACE', 'HURRY_GOLD', -15);

-- Karlstejn
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_KARLSTEJN', 'YIELD_CULTURE', 3),
	('BUILDING_KARLSTEJN', 'YIELD_FAITH', 4);

-- Notre Dame
UPDATE Buildings
SET
	PrereqTech = 'TECH_MACHINERY',
	Help = 'TXT_KEY_BUILDING_NOTRE_DAME_HELP',
	UnmoddedHappiness = 0,
	Happiness = 1,
	ThemingBonusHelp = 'TXT_KEY_BUILDING_NOTRE_DAME_THEMING_BONUS_HELP',
	GreatWorkSlotType = 'GREAT_WORK_SLOT_ART_ARTIFACT',
	GreatWorkCount = 2,
	FreeBuildingThisCity = 'BUILDINGCLASS_CATHEDRAL',
	BasePressureModifierGlobal = 30
WHERE Type = 'BUILDING_NOTRE_DAME';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_NOTRE_DAME', 'YIELD_CULTURE', 1),
	('BUILDING_NOTRE_DAME', 'YIELD_FAITH', 4);

INSERT INTO Building_ThemingBonuses
	(BuildingType, Description, RequiresUniquePlayers, RequiresAnyButOwner)
VALUES
	('BUILDING_NOTRE_DAME', 'TXT_KEY_THEMING_BONUS_BUILDING_NOTRE_DAME', 1, 1);

-- Globe Theatre
UPDATE Buildings
SET
	BoredomFlatReduction = 1,
	NoUnhappfromXSpecialistsGlobal = 1
WHERE Type = 'BUILDING_GLOBE_THEATER';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_GLOBE_THEATER', 'YIELD_CULTURE', 2);

INSERT INTO Building_SpecialistYieldChanges
	(BuildingType, SpecialistType, YieldType, Yield)
VALUES
	('BUILDING_GLOBE_THEATER', 'SPECIALIST_WRITER', 'YIELD_GOLDEN_AGE_POINTS', 2),
	('BUILDING_GLOBE_THEATER', 'SPECIALIST_ARTIST', 'YIELD_GOLDEN_AGE_POINTS', 2),
	('BUILDING_GLOBE_THEATER', 'SPECIALIST_MUSICIAN', 'YIELD_GOLDEN_AGE_POINTS', 2);

-- Chichen Itza
UPDATE Buildings
SET
	Help = 'TXT_KEY_WONDER_CHICHEN_ITZA_HELP',
	PrereqTech = 'TECH_ASTRONOMY',
	Happiness = 0,
	EmpireSizeModifierReductionGlobal = -10,
	FreeBuildingThisCity = 'BUILDINGCLASS_TEOCALLI'
WHERE Type = 'BUILDING_CHICHEN_ITZA';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CHICHEN_ITZA', 'YIELD_CULTURE', 1);

-- Himeji Castle
UPDATE Buildings
SET
	FreeBuildingThisCity = 'BUILDINGCLASS_FORTRESS',
	GlobalDefenseMod = 10,
	CitySupplyFlat = 5
WHERE Type = 'BUILDING_HIMEJI_CASTLE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_HIMEJI_CASTLE', 'YIELD_CULTURE', 1);

-- Leaning Tower of Pisa
UPDATE Buildings
SET
	PrereqTech = 'TECH_CHEMISTRY',
	GlobalGreatPeopleRateModifier = 10,
	GreatPeopleRateModifier = 15
WHERE Type = 'BUILDING_LEANING_TOWER';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_LEANING_TOWER', 'YIELD_CULTURE', 1);

-- Porcelain Tower
UPDATE Buildings
SET
	PolicyBranchType = NULL,
	IlliteracyFlatReduction = 1,
	MedianTechPercentChange = 0
WHERE Type = 'BUILDING_PORCELAIN_TOWER';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_PORCELAIN_TOWER', 'YIELD_CULTURE', 1);

INSERT INTO Building_YieldModifiers
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_PORCELAIN_TOWER', 'YIELD_SCIENCE', 10);

-- Taj Mahal
UPDATE Buildings
SET
	Help = 'TXT_KEY_WONDER_TAJ_MAHAL_HELP',
	Happiness = 0
WHERE Type = 'BUILDING_TAJ_MAHAL';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TAJ_MAHAL', 'YIELD_CULTURE', 2);

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TAJ_MAHAL', 'YIELD_GOLDEN_AGE_POINTS', 50);

INSERT INTO Building_YieldChangesPerReligion
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TAJ_MAHAL', 'YIELD_SCIENCE', 300),
	('BUILDING_TAJ_MAHAL', 'YIELD_CULTURE', 300),
	('BUILDING_TAJ_MAHAL', 'YIELD_FAITH', 300);

-- Uffizi
UPDATE Buildings
SET
	PolicyBranchType = NULL,
	PrereqTech = 'TECH_ECONOMICS',
	FreeBuildingThisCity = 'BUILDINGCLASS_GALLERY',
	NoUnhappfromXSpecialistsGlobal = 1
WHERE Type = 'BUILDING_UFFIZI';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_UFFIZI', 'YIELD_CULTURE', 2);

INSERT INTO Building_GreatWorkYieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_UFFIZI', 'YIELD_CULTURE', 1);

-- Sistine Chapel
UPDATE Buildings SET GlobalCultureRateModifier = 10 WHERE Type = 'BUILDING_SISTINE_CHAPEL';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SISTINE_CHAPEL', 'YIELD_CULTURE', 1);

-- Summer Palace
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SUMMER_PALACE', 'YIELD_CULTURE', 1);

INSERT INTO Building_ResourceQuantity
	(BuildingType, ResourceType, Quantity)
VALUES
	('BUILDING_SUMMER_PALACE', 'RESOURCE_PAPER', 1);

INSERT INTO Building_UnitCombatProductionModifiersGlobal
	(BuildingType, UnitCombatType, Modifier)
VALUES
	('BUILDING_SUMMER_PALACE', 'UNITCOMBAT_DIPLOMACY', 20);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_SUMMER_PALACE', 'BUILDINGCLASS_CHANCERY', 'YIELD_GOLD', 3),
	('BUILDING_SUMMER_PALACE', 'BUILDINGCLASS_CHANCERY', 'YIELD_CULTURE', 1);

-- Red Fort
UPDATE Buildings
SET
	PolicyType = 'POLICY_PIETY_FINISHER',
	GlobalDefenseMod = 0,
	FreeBuildingThisCity = 'BUILDINGCLASS_ARSENAL',
	RangedStrikeModifier = 10,
	CityRangedStrikeRange = 1,
	CitySupplyModifierGlobal = 5
WHERE Type = 'BUILDING_RED_FORT';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_RED_FORT', 'YIELD_CULTURE', 1);

INSERT INTO Building_FreeUnits
	(BuildingType, UnitType, NumUnits)
VALUES
	('BUILDING_RED_FORT', 'UNIT_ENGINEER', 1);

-- Neuschwanstein
UPDATE Buildings SET Happiness = 0 WHERE Type = 'BUILDING_NEUSCHWANSTEIN';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_NEUSCHWANSTEIN', 'YIELD_GOLD', 6),
	('BUILDING_NEUSCHWANSTEIN', 'YIELD_CULTURE', 4);

UPDATE Building_BuildingClassYieldChanges
SET YieldChange = 1 -- Gold, Culture
WHERE BuildingType = 'BUILDING_NEUSCHWANSTEIN';

-- Soho Foundry
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SOHO_FOUNDRY', 'YIELD_PRODUCTION', 5),
	('BUILDING_SOHO_FOUNDRY', 'YIELD_CULTURE', 1);

INSERT INTO Building_ResourceQuantity
	(BuildingType, ResourceType, Quantity)
VALUES
	('BUILDING_SOHO_FOUNDRY', 'RESOURCE_COAL', 4);

-- Louvre
UPDATE Buildings
SET
	PolicyBranchType = NULL,
	PrereqTech = 'TECH_ARCHAEOLOGY',
	PolicyType = 'POLICY_AESTHETICS_FINISHER',
	FreeBuildingThisCity = 'BUILDINGCLASS_MUSEUM'
WHERE Type = 'BUILDING_LOUVRE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_LOUVRE', 'YIELD_CULTURE', 2);

INSERT INTO Building_FreeUnits
	(BuildingType, UnitType, NumUnits)
VALUES
	('BUILDING_LOUVRE', 'UNIT_ARCHAEOLOGIST', 2);

-- Palace of Westminster
UPDATE Buildings
SET
	PolicyBranchType = NULL,
	PolicyType = 'POLICY_PATRONAGE_FINISHER',
	ExtraLeagueVotes = 4,
	PovertyFlatReduction = 1
WHERE Type = 'BUILDING_BIG_BEN';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BIG_BEN', 'YIELD_GOLD', 4),
	('BUILDING_BIG_BEN', 'YIELD_CULTURE', 1);

DELETE FROM Building_HurryModifiers WHERE BuildingType = 'BUILDING_BIG_BEN';

INSERT INTO Building_FreeUnits
	(BuildingType, UnitType, NumUnits)
VALUES
	('BUILDING_BIG_BEN', 'UNIT_MERCHANT', 1);

-- Eiffel Tower
UPDATE Buildings
SET
	UnmoddedHappiness = 0,
	EnhancedYieldTech = NULL,
	TechEnhancedTourism = 0,
	PrereqTech = 'TECH_DYNAMITE',
	PolicyCostModifier = -10
WHERE Type = 'BUILDING_EIFFEL_TOWER';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_EIFFEL_TOWER', 'YIELD_CULTURE', 1);

-- Brandenburg Gate
UPDATE Buildings SET CitySupplyFlat = 10 WHERE Type = 'BUILDING_BRANDENBURG_GATE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BRANDENBURG_GATE', 'YIELD_CULTURE', 1);

DELETE FROM Building_DomainFreeExperiences WHERE BuildingType = 'BUILDING_BRANDENBURG_GATE';

INSERT INTO Building_DomainFreeExperiencesGlobal
	(BuildingType, DomainType, Experience)
VALUES
	('BUILDING_BRANDENBURG_GATE', 'DOMAIN_LAND', 15),
	('BUILDING_BRANDENBURG_GATE', 'DOMAIN_SEA', 15),
	('BUILDING_BRANDENBURG_GATE', 'DOMAIN_AIR', 15);

-- Statue of Liberty
UPDATE Buildings SET PrereqTech = 'TECH_BIOLOGY' WHERE Type = 'BUILDING_STATUE_OF_LIBERTY';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_STATUE_OF_LIBERTY', 'YIELD_CULTURE', 1);

DELETE FROM Building_SpecialistYieldChanges WHERE BuildingType = 'BUILDING_STATUE_OF_LIBERTY';

INSERT INTO Building_SpecialistYieldChanges
	(BuildingType, SpecialistType, YieldType, Yield)
SELECT
	'BUILDING_STATUE_OF_LIBERTY', Type, 'YIELD_PRODUCTION', 1
FROM Specialists
WHERE GreatPeopleUnitClass IS NOT NULL;

-- Empire State Building
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_EMPIRE_STATE_BUILDING', 'YIELD_CULTURE', 1);

INSERT INTO Building_SpecialistYieldChanges
	(BuildingType, SpecialistType, YieldType, Yield)
SELECT
	'BUILDING_EMPIRE_STATE_BUILDING', Type, 'YIELD_GOLD', 1
FROM Specialists
WHERE GreatPeopleUnitClass IS NOT NULL;

-- Kremlin
UPDATE Buildings
SET
	PrereqTech = 'TECH_REPLACEABLE_PARTS',
	DistressFlatReduction = 1
WHERE Type = 'BUILDING_KREMLIN';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_KREMLIN', 'YIELD_CULTURE', 1);

UPDATE Building_UnitCombatProductionModifiers
SET Modifier = 33
WHERE BuildingType = 'BUILDING_KREMLIN';

-- Cristo Redentor
UPDATE Buildings SET PolicyCostModifier = 0 WHERE Type = 'BUILDING_CRISTO_REDENTOR';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CRISTO_REDENTOR', 'YIELD_CULTURE', 5),
	('BUILDING_CRISTO_REDENTOR', 'YIELD_TOURISM', 25);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_CRISTO_REDENTOR', 'BUILDINGCLASS_HOTEL', 'YIELD_CULTURE', 2),
	('BUILDING_CRISTO_REDENTOR', 'BUILDINGCLASS_HOTEL', 'YIELD_TOURISM', 2),
	('BUILDING_CRISTO_REDENTOR', 'BUILDINGCLASS_HOTEL', 'YIELD_GOLDEN_AGE_POINTS', 2);

-- Broadway
UPDATE Buildings SET PolicyType = 'POLICY_COMMERCE_FINISHER' WHERE Type = 'BUILDING_BROADWAY';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BROADWAY', 'YIELD_CULTURE', 2);

INSERT INTO Building_YieldFromConstruction
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BROADWAY', 'YIELD_CULTURE', 250);

-- Prora
UPDATE Buildings
SET
	UnmoddedHappiness = 0,
	Happiness = 1
WHERE Type = 'BUILDING_PRORA_RESORT';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_PRORA_RESORT', 'YIELD_CULTURE', 1);

INSERT INTO Building_YieldFromPillageGlobal
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_PRORA_RESORT', 'YIELD_CULTURE', 25);

-- Pentagon
UPDATE Buildings
SET
	PolicyType = 'POLICY_EXPLORATION_FINISHER',
	UnitUpgradeCostMod = 0,
	CitySupplyFlatGlobal = 1,
	AirModifierGlobal = 2
WHERE Type = 'BUILDING_PENTAGON';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_PENTAGON', 'YIELD_CULTURE', 1);

INSERT INTO Building_FreeUnits
	(BuildingType, UnitType, NumUnits)
VALUES
	('BUILDING_PENTAGON', 'UNIT_JET_FIGHTER', 2);

INSERT INTO Building_DomainProductionModifiers
	(BuildingType, DomainType, Modifier)
VALUES
	('BUILDING_PENTAGON', 'DOMAIN_AIR', 50);

INSERT INTO Building_DomainFreeExperiences
	(BuildingType, DomainType, Experience)
VALUES
	('BUILDING_PENTAGON', 'DOMAIN_AIR', 20);

-- The Motherland Calls
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MOTHERLAND_STATUE', 'YIELD_CULTURE', 1);

-- Bletchley Park
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BLETCHLEY_PARK', 'YIELD_SCIENCE', 5),
	('BUILDING_BLETCHLEY_PARK', 'YIELD_CULTURE', 1);

INSERT INTO Building_SpecialistYieldChanges
	(BuildingType, SpecialistType, YieldType, Yield)
VALUES
	('BUILDING_BLETCHLEY_PARK', 'SPECIALIST_SCIENTIST', 'YIELD_SCIENCE', 1);

-- CN Tower
UPDATE Buildings
SET
	FreeBuilding = NULL,
	GlobalPopulationChange = 0,
	FreeBuildingThisCity = 'BUILDINGCLASS_BROADCAST_TOWER',
	LandmarksTourismPercent = 25,
	GreatWorksTourismModifier = 25,
	GlobalLandmarksTourismPercent = 25,
	GlobalGreatWorksTourismModifier = 25
WHERE Type = 'BUILDING_CN_TOWER';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CN_TOWER', 'YIELD_CULTURE', 1);

INSERT INTO Building_GreatWorkYieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CN_TOWER', 'YIELD_GOLD', 2),
	('BUILDING_CN_TOWER', 'YIELD_TOURISM', 2);

-- Sydney Opera House
UPDATE Buildings
SET
	PrereqTech = 'TECH_SATELLITES',
	CultureRateModifier = 25
WHERE Type = 'BUILDING_SYDNEY_OPERA_HOUSE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SYDNEY_OPERA_HOUSE', 'YIELD_CULTURE', 1);

-- Hubble Space Telescope
UPDATE Buildings SET PrereqTech = 'TECH_ROBOTICS' WHERE Type = 'BUILDING_HUBBLE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_HUBBLE', 'YIELD_CULTURE', 1);

UPDATE Building_FreeUnits SET NumUnits = 1 WHERE BuildingType = 'BUILDING_HUBBLE';

-- Great Firewall
UPDATE Buildings
SET
	PrereqTech = 'TECH_INTERNET',
	EspionageModifier = 0,
	GlobalEspionageModifier = 0,
	SpySecurityModifier = 50
WHERE Type = 'BUILDING_GREAT_FIREWALL';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_GREAT_FIREWALL', 'YIELD_CULTURE', 1);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_GREAT_FIREWALL', 'BUILDINGCLASS_LABORATORY', 'YIELD_SCIENCE', 10);

-- CERN
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CERN', 'YIELD_SCIENCE', 10),
	('BUILDING_CERN', 'YIELD_CULTURE', 1);

----------------------------------------------------------------------------
-- Corporation Buildings
----------------------------------------------------------------------------

-- Common
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 1
FROM Buildings
WHERE IsCorporation = 1 AND WonderSplashImage IS NOT NULL;

-- Trader Sid's
INSERT INTO Building_YieldPerFranchise
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TRADER_SIDS', 'YIELD_GOLD', 4);

-- Centaurus Extractors
INSERT INTO Building_YieldPerFranchise
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_LANDSEA_EXTRACTORS', 'YIELD_PRODUCTION', 3);

INSERT INTO Building_SeaPlotYieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_LANDSEA_EXTRACTORS', 'YIELD_PRODUCTION', 1);

-- Hexxon Refinery
INSERT INTO Building_ResourceQuantityPerXFranchises
	(BuildingType, ResourceType, NumFranchises)
VALUES
	('BUILDING_HEXXON_REFINERY', 'RESOURCE_COAL', 3),
	('BUILDING_HEXXON_REFINERY', 'RESOURCE_OIL', 3);

-- Giorgio Armeier
INSERT INTO Building_YieldPerFranchise
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_GIORGIO_ARMEIER', 'YIELD_CULTURE', 2);

-- Firaxite Materials
INSERT INTO Building_YieldPerFranchise
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_FIRAXITE_MATERIALS', 'YIELD_SCIENCE', 2);

-- TwoKay Foods
INSERT INTO Building_YieldPerFranchise
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TWOKAY_FOODS', 'YIELD_FOOD', 3);

INSERT INTO Building_YieldModifiers
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TWOKAY_FOODS', 'YIELD_FOOD', 10);

-- Civilized Jewelers

----------------------------------------------------------------------------
-- World Congress Wonders
----------------------------------------------------------------------------

-- Grand Canal
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_GRAND_CANAL', 'YIELD_CULTURE', 1);

-- Crystal Palace
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CRYSTAL_PALACE', 'YIELD_CULTURE', 1);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_CRYSTAL_PALACE', 'BUILDINGCLASS_FACTORY', 'YIELD_PRODUCTION', 2),
	('BUILDING_CRYSTAL_PALACE', 'BUILDINGCLASS_FACTORY', 'YIELD_SCIENCE', 2);

-- Menin Gate
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MENIN_GATE', 'YIELD_CULTURE', 2);

INSERT INTO Building_YieldFromDeath
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MENIN_GATE', 'YIELD_CULTURE', 2),
	('BUILDING_MENIN_GATE', 'YIELD_GOLDEN_AGE_POINTS', 2);

INSERT INTO Building_ImprovementYieldChangesGlobal
	(BuildingType, ImprovementType, YieldType, Yield)
VALUES
	('BUILDING_MENIN_GATE', 'IMPROVEMENT_FORT', 'YIELD_CULTURE', 2),
	('BUILDING_MENIN_GATE', 'IMPROVEMENT_FORT', 'YIELD_GOLDEN_AGE_POINTS', 2),
	('BUILDING_MENIN_GATE', 'IMPROVEMENT_CITADEL', 'YIELD_CULTURE', 2),
	('BUILDING_MENIN_GATE', 'IMPROVEMENT_CITADEL', 'YIELD_GOLDEN_AGE_POINTS', 2),
	('BUILDING_MENIN_GATE', 'IMPROVEMENT_MONGOLIA_ORDO', 'YIELD_CULTURE', 2),
	('BUILDING_MENIN_GATE', 'IMPROVEMENT_MONGOLIA_ORDO', 'YIELD_GOLDEN_AGE_POINTS', 2),
	('BUILDING_MENIN_GATE', 'IMPROVEMENT_LANDMARK', 'YIELD_CULTURE', 2),
	('BUILDING_MENIN_GATE', 'IMPROVEMENT_LANDMARK', 'YIELD_GOLDEN_AGE_POINTS', 2);

-- Olympic Village
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_OLYMPIC_VILLAGE', 'YIELD_CULTURE', 3);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_OLYMPIC_VILLAGE', 'BUILDINGCLASS_STADIUM', 'YIELD_GOLD', 2),
	('BUILDING_OLYMPIC_VILLAGE', 'BUILDINGCLASS_STADIUM', 'YIELD_TOURISM', 2);

-- International Space Station
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_INTERNATIONAL_SPACE_STATION', 'YIELD_CULTURE', 1);

INSERT INTO Building_YieldFromTech
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_INTERNATIONAL_SPACE_STATION', 'YIELD_PRODUCTION', 100);

-- United Nations
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_UN', 'YIELD_CULTURE', 1);

-- Consulate
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CONSULATE', 'YIELD_CULTURE', 1);
