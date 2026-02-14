----------------------------------------------------------
-- Unique Ability: Reconquista
----------------------------------------------------------
UPDATE Traits
SET
	NaturalWonderFirstFinderGold = 0,
	NaturalWonderSubsequentFinderGold = 0,
	NaturalWonderYieldModifier = 0,
	NaturalWonderHappinessModifier = 0,
	CanPurchaseNavalUnitsFaith = 1,
	NewCitiesStartWithCapitalReligion = 1
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
-- Unique Unit: Spanish Inquisition (Inquisitor)
-- Exactly the same as a regular Inquisitor except for different text. No effect on balance.
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_SPAIN', 'UNITCLASS_INQUISITOR', 'UNIT_SPAIN_INQUISITOR');

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
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_EXPLORER') + 5,
	DefaultUnitAI = 'UNITAI_FAST_ATTACK',
	Found = 1,
	FoundMid = 1,
	NoMinorCivGift = 1
WHERE Type = 'UNIT_SPANISH_CONQUISTADOR';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_SPANISH_CONQUISTADOR', 'PROMOTION_OVERRUN'),
	('UNIT_SPANISH_CONQUISTADOR', 'PROMOTION_CIBOLA');

INSERT INTO Unit_BuildOnFound
	(UnitType, BuildingClassType)
SELECT
	'UNIT_SPANISH_CONQUISTADOR', BuildingClassType
FROM Unit_BuildOnFound
WHERE UnitType = 'UNIT_PIONEER';

INSERT INTO Unit_AITypes
	(UnitType, UnitAIType)
VALUES
	('UNIT_SPANISH_CONQUISTADOR', 'UNITAI_FAST_ATTACK'),
	('UNIT_SPANISH_CONQUISTADOR', 'UNITAI_SETTLE');

----------------------------------------------------------
-- Unique Unit: Armada (Corvette)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_SPAIN', 'UNITCLASS_PRIVATEER', 'UNIT_ARMADA');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_PRIVATEER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_PRIVATEER') + 2
WHERE Type = 'UNIT_ARMADA';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_ARMADA', 'PROMOTION_SANTA_MARIA'),
	('UNIT_ARMADA', 'PROMOTION_INVINCIBLE');

----------------------------------------------------------
-- Unique Improvement: Hacienda
----------------------------------------------------------
UPDATE Builds
SET
	PrereqTech = 'TECH_STEEL',
	Time = 700
WHERE Type = 'BUILD_HACIENDA';

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_WORKER', 'BUILD_HACIENDA');

INSERT INTO BuildFeatures
	(BuildType, FeatureType, Time, Remove)
VALUES
	('BUILD_HACIENDA', 'FEATURE_FOREST', 400, 1),
	('BUILD_HACIENDA', 'FEATURE_JUNGLE', 500, 1),
	('BUILD_HACIENDA', 'FEATURE_MARSH', 500, 1);

UPDATE Improvements
SET NoTwoAdjacent = 1
WHERE Type = 'IMPROVEMENT_HACIENDA';

INSERT INTO Improvement_ValidTerrains
	(ImprovementType, TerrainType)
VALUES
	('IMPROVEMENT_HACIENDA', 'TERRAIN_GRASS'),
	('IMPROVEMENT_HACIENDA', 'TERRAIN_PLAINS'),
	('IMPROVEMENT_HACIENDA', 'TERRAIN_DESERT'),
	('IMPROVEMENT_HACIENDA', 'TERRAIN_TUNDRA'),
	('IMPROVEMENT_HACIENDA', 'TERRAIN_SNOW');

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_HACIENDA', 'YIELD_CULTURE', 1);

INSERT INTO Improvement_AdjacentResourceYieldChanges
	(ImprovementType, ResourceType, YieldType, Yield)
SELECT
	'IMPROVEMENT_HACIENDA', Type, 'YIELD_FOOD', 3
FROM Resources
WHERE ResourceUsage = 0;

INSERT INTO Improvement_AdjacentResourceYieldChanges
	(ImprovementType, ResourceType, YieldType, Yield)
SELECT
	'IMPROVEMENT_HACIENDA', Type, 'YIELD_PRODUCTION', 3
FROM Resources
WHERE ResourceUsage = 1;

INSERT INTO Improvement_AdjacentResourceYieldChanges
	(ImprovementType, ResourceType, YieldType, Yield)
SELECT
	'IMPROVEMENT_HACIENDA', Type, 'YIELD_GOLD', 3
FROM Resources
WHERE ResourceUsage = 2;

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_HACIENDA', 'TECH_ARCHITECTURE', 'YIELD_CULTURE', 2),
	('IMPROVEMENT_HACIENDA', 'TECH_FERTILIZER', 'YIELD_FOOD', 1),
	('IMPROVEMENT_HACIENDA', 'TECH_FERTILIZER', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_HACIENDA', 'TECH_FERTILIZER', 'YIELD_GOLD', 1);

----------------------------------------------------------
-- Unique Building: Bullring (Zoo)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_SPAIN', 'BUILDINGCLASS_THEATRE', 'BUILDING_BULLRING');

DELETE FROM Building_ClassesNeededInCity WHERE BuildingType = 'BUILDING_BULLRING';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_THEATRE' AND YieldType = 'YIELD_CULTURE') + 2
WHERE BuildingType = 'BUILDING_BULLRING' AND YieldType = 'YIELD_CULTURE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BULLRING', 'YIELD_FAITH', 4),
	('BUILDING_BULLRING', 'YIELD_TOURISM', 4);

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
VALUES
	('BUILDING_BULLRING', 'IMPROVEMENT_PASTURE', 'YIELD_CULTURE', 2),
	('BUILDING_BULLRING', 'IMPROVEMENT_PASTURE', 'YIELD_TOURISM', 2);

UPDATE Building_FeatureYieldChanges
SET
	Yield = (
		SELECT Yield FROM Building_FeatureYieldChanges a
		WHERE a.BuildingType = 'BUILDING_THEATRE'
		AND a.FeatureType = Building_FeatureYieldChanges.FeatureType
		AND a.YieldType = Building_FeatureYieldChanges.YieldType
	) + 1
WHERE BuildingType = 'BUILDING_BULLRING';

INSERT INTO Building_InstantYieldFromWLTKDStart
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BULLRING', 'YIELD_GOLD', 100),
	('BUILDING_BULLRING', 'YIELD_CULTURE', 100),
	('BUILDING_BULLRING', 'YIELD_FAITH', 100);

INSERT INTO Building_WLTKDFromProject
	(BuildingType, ProjectType, Turns)
VALUES
	('BUILDING_BULLRING', 'PROJECT_PUBLIC_WORKS', 10);
