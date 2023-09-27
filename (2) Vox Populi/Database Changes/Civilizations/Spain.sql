----------------------------------------------------------
-- Unique Ability: Reconquista
----------------------------------------------------------
UPDATE Traits
SET
	NaturalWonderFirstFinderGold = 0,
	NaturalWonderSubsequentFinderGold = 0,
	NaturalWonderYieldModifier = 0,
	NaturalWonderHappinessModifier = 0,
	FreeUnitOnConquest = 'UNIT_SPAIN_INQUISITOR',
	CanPurchaseNavalUnitsFaith = 1
WHERE Type = 'TRAIT_SEVEN_CITIES';

CREATE TEMP TABLE Helper (
	YieldType TEXT,
	Yield INTEGER
);

INSERT INTO Helper
VALUES
	('YIELD_GOLD', 10),
	('YIELD_FAITH', 4);

INSERT INTO Trait_YieldFromTileSettle
	(TraitType, TerrainType, YieldType, Yield)
SELECT
	'TRAIT_SEVEN_CITIES', a.Type, b.YieldType, b.Yield
FROM Terrains a, Helper b;

INSERT INTO Trait_YieldFromTilePurchase
	(TraitType, YieldType, Yield)
SELECT
	'TRAIT_SEVEN_CITIES', YieldType, Yield
FROM Helper;

INSERT INTO Trait_YieldFromTileEarn
	(TraitType, YieldType, Yield)
SELECT
	'TRAIT_SEVEN_CITIES', YieldType, Yield
FROM Helper;

INSERT INTO Trait_YieldFromTileConquest
	(TraitType, TerrainType, YieldType, Yield)
SELECT
	'TRAIT_SEVEN_CITIES', a.Type, b.YieldType, b.Yield
FROM Terrains a, Helper b;

INSERT INTO Trait_YieldFromTileCultureBomb
	(TraitType, TerrainType, YieldType, Yield)
SELECT
	'TRAIT_SEVEN_CITIES', a.Type, b.YieldType, b.Yield
FROM Terrains a, Helper b;

INSERT INTO Trait_YieldFromTileStealCultureBomb
	(TraitType, TerrainType, YieldType, Yield)
SELECT
	'TRAIT_SEVEN_CITIES', a.Type, b.YieldType, b.Yield
FROM Terrains a, Helper b;

DROP TABLE Helper;

----------------------------------------------------------
-- Unique Unit: Spanish Inquisitor
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_SPAIN', 'UNITCLASS_INQUISITOR', 'UNIT_SPAIN_INQUISITOR');

UPDATE Units
SET
	RequiresEnhancedReligion = 0,
	NoMaintenance = 1
WHERE Type = 'UNIT_SPAIN_INQUISITOR';

----------------------------------------------------------
-- Unique Unit: Conquistador (Explorer)
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_EXPLORER' WHERE UnitType = 'UNIT_SPANISH_CONQUISTADOR';

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_EXPLORER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_EXPLORER') + 6,
	DefaultUnitAI = 'UNITAI_FAST_ATTACK',
	Found = 1,
	FoundMid = 1,
	FoundAbroad = 1
WHERE Type = 'UNIT_SPANISH_CONQUISTADOR';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_SPANISH_CONQUISTADOR', 'PROMOTION_SHOCK_4'),
	('UNIT_SPANISH_CONQUISTADOR', 'PROMOTION_COASTAL_TERROR');

INSERT INTO Unit_BuildOnFound
	(UnitType, BuildingClassType)
SELECT
	'UNIT_SPANISH_CONQUISTADOR', BuildingClassType
FROM Unit_BuildOnFound
WHERE UnitType = 'UNIT_PIONEER';

INSERT INTO Unit_BuildOnFound
	(UnitType, BuildingClassType)
VALUES
	('UNIT_SPANISH_CONQUISTADOR', 'BUILDINGCLASS_LIGHTHOUSE'),
	('UNIT_SPANISH_CONQUISTADOR', 'BUILDINGCLASS_ARMORY'),
	('UNIT_SPANISH_CONQUISTADOR', 'BUILDINGCLASS_D_FOR_SPAIN_MISSION');

----------------------------------------------------------
-- Unique Building: Mission
----------------------------------------------------------

-- Need the dummy to build
INSERT INTO Building_ClassesNeededInCity
	(BuildingType, BuildingClassType)
VALUES
	('BUILDING_SPAIN_MISSION', 'BUILDINGCLASS_D_FOR_SPAIN_MISSION');

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SPAIN_MISSION', 'YIELD_FAITH', 2);

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
VALUES
	('BUILDING_SPAIN_MISSION', 'IMPROVEMENT_SPAIN_HACIENDA', 'YIELD_FAITH', 1);

----------------------------------------------------------
-- Unique Improvement: Hacienda
----------------------------------------------------------
UPDATE Builds
SET
	PrereqTech = 'TECH_MACHINERY',
	Time = 800
WHERE Type = 'BUILD_SPAIN_HACIENDA';

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_WORKER', 'BUILD_SPAIN_HACIENDA');

INSERT INTO BuildFeatures
	(BuildType, FeatureType, Time, Remove)
VALUES
	('BUILD_SPAIN_HACIENDA', 'FEATURE_FOREST', 300, 1),
	('BUILD_SPAIN_HACIENDA', 'FEATURE_JUNGLE', 400, 1),
	('BUILD_SPAIN_HACIENDA', 'FEATURE_MARSH', 400, 1);

UPDATE Improvements
SET NoTwoAdjacent = 1
WHERE Type = 'IMPROVEMENT_SPAIN_HACIENDA';

INSERT INTO Improvement_ValidTerrains
	(ImprovementType, TerrainType)
VALUES
	('IMPROVEMENT_SPAIN_HACIENDA', 'TERRAIN_GRASS'),
	('IMPROVEMENT_SPAIN_HACIENDA', 'TERRAIN_PLAINS'),
	('IMPROVEMENT_SPAIN_HACIENDA', 'TERRAIN_DESERT'),
	('IMPROVEMENT_SPAIN_HACIENDA', 'TERRAIN_TUNDRA'),
	('IMPROVEMENT_SPAIN_HACIENDA', 'TERRAIN_SNOW');

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_SPAIN_HACIENDA', 'YIELD_CULTURE', 1);

INSERT INTO Improvement_AdjacentResourceYieldChanges
	(ImprovementType, ResourceType, YieldType, Yield)
SELECT
	'IMPROVEMENT_SPAIN_HACIENDA', Type, 'YIELD_FOOD', 3
FROM Resources
WHERE ResourceUsage = 0;

INSERT INTO Improvement_AdjacentResourceYieldChanges
	(ImprovementType, ResourceType, YieldType, Yield)
SELECT
	'IMPROVEMENT_SPAIN_HACIENDA', Type, 'YIELD_PRODUCTION', 3
FROM Resources
WHERE ResourceUsage = 1;

INSERT INTO Improvement_AdjacentResourceYieldChanges
	(ImprovementType, ResourceType, YieldType, Yield)
SELECT
	'IMPROVEMENT_SPAIN_HACIENDA', Type, 'YIELD_GOLD', 3
FROM Resources
WHERE ResourceUsage = 2;

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_SPAIN_HACIENDA', 'TECH_ARCHITECTURE', 'YIELD_CULTURE', 2),
	('IMPROVEMENT_SPAIN_HACIENDA', 'TECH_FERTILIZER', 'YIELD_FOOD', 1),
	('IMPROVEMENT_SPAIN_HACIENDA', 'TECH_FERTILIZER', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_SPAIN_HACIENDA', 'TECH_FERTILIZER', 'YIELD_GOLD', 1);
