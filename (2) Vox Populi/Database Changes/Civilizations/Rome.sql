----------------------------------------------------------
-- Unique Ability: The Glory of Rome
----------------------------------------------------------
UPDATE Traits
SET
	CapitalBuildingModifier = 15,
	CityStateCombatModifier = 30,
	AnnexedCityStatesGiveYields = 1
WHERE Type = 'TRAIT_CAPITAL_BUILDINGS_CHEAPER';

----------------------------------------------------------
-- Unique Unit: Legion (Swordsman)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_SWORDSMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_SWORDSMAN') + 2,
	WorkRate = 100
WHERE Type = 'UNIT_ROMAN_LEGION';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_ROMAN_LEGION', 'PROMOTION_COVER_1'),
	('UNIT_ROMAN_LEGION', 'PROMOTION_PILUM'),
	('UNIT_ROMAN_LEGION', 'PROMOTION_PRAEFECTUS_CASTRORUM');

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_ROMAN_LEGION', 'BUILD_ROAD'),
	('UNIT_ROMAN_LEGION', 'BUILD_FORT');

INSERT INTO Unit_AITypes
	(UnitType, UnitAIType)
VALUES
	('UNIT_ROMAN_LEGION', 'UNITAI_WORKER');

----------------------------------------------------------
-- Unique Unit: Ballista (Catapult)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CATAPULT'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_CATAPULT') + 3,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_CATAPULT') + 1,
	BaseSightRange = (SELECT BaseSightRange FROM Units WHERE Type = 'UNIT_CATAPULT') + 1
WHERE Type = 'UNIT_ROMAN_BALLISTA';

DELETE FROM Unit_FreePromotions WHERE UnitType = 'UNIT_ROMAN_BALLISTA' AND PromotionType = 'PROMOTION_SIEGE_INACCURACY';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_ROMAN_BALLISTA', 'PROMOTION_LEGATUS_LEGIONIS');

----------------------------------------------------------
-- Unique Improvement: Villa
----------------------------------------------------------
UPDATE Builds
SET
	PrereqTech = 'TECH_ENGINEERING',
	Time = 600
WHERE Type = 'BUILD_VILLA';

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_WORKER', 'BUILD_VILLA');

INSERT INTO BuildFeatures
	(BuildType, FeatureType, Time, Remove)
VALUES
	('BUILD_VILLA', 'FEATURE_FOREST', 400, 1),
	('BUILD_VILLA', 'FEATURE_JUNGLE', 500, 1),
	('BUILD_VILLA', 'FEATURE_MARSH', 500, 1);

UPDATE Improvements
SET
	NoAdjacentCity = 1,
	NoTwoAdjacent = 1,
	SpawnsAdjacentResource = 'RESOURCE_FIGS'
WHERE Type = 'IMPROVEMENT_VILLA';

INSERT INTO Improvement_ValidTerrains
	(ImprovementType, TerrainType)
VALUES
	('IMPROVEMENT_VILLA', 'TERRAIN_GRASS'),
	('IMPROVEMENT_VILLA', 'TERRAIN_PLAINS'),
	('IMPROVEMENT_VILLA', 'TERRAIN_DESERT'),
	('IMPROVEMENT_VILLA', 'TERRAIN_TUNDRA'),
	('IMPROVEMENT_VILLA', 'TERRAIN_SNOW');

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_VILLA', 'YIELD_CULTURE', 3);

INSERT INTO Improvement_YieldPerXAdjacentImprovement
	(ImprovementType, OtherImprovementType, YieldType, Yield, NumRequired)
VALUES
	('IMPROVEMENT_FARM', 'IMPROVEMENT_VILLA', 'YIELD_FOOD', 1, 1),
	('IMPROVEMENT_FARM', 'IMPROVEMENT_VILLA', 'YIELD_GOLD', 1, 1),
	('IMPROVEMENT_PLANTATION', 'IMPROVEMENT_VILLA', 'YIELD_GOLD', 1, 1),
	('IMPROVEMENT_PLANTATION', 'IMPROVEMENT_VILLA', 'YIELD_CULTURE', 1, 1);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_VILLA', 'TECH_CIVIL_SERVICE', 'YIELD_FOOD', 1),
	('IMPROVEMENT_VILLA', 'TECH_CIVIL_SERVICE', 'YIELD_GOLD', 1),
	('IMPROVEMENT_VILLA', 'TECH_ECONOMICS', 'YIELD_GOLD', 1),
	('IMPROVEMENT_VILLA', 'TECH_ECONOMICS', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_VILLA', 'TECH_FERTILIZER', 'YIELD_FOOD', 1),
	('IMPROVEMENT_VILLA', 'TECH_FERTILIZER', 'YIELD_CULTURE', 1);

INSERT INTO Building_YieldPerXImprovementGlobal
	(BuildingType, ImprovementType, YieldType, Yield, NumRequired)
VALUES
	('BUILDING_PALACE', 'IMPROVEMENT_VILLA', 'YIELD_FOOD', 2, 1);

----------------------------------------------------------
-- Unique Building: Fornix (Heroic Epic)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_ROME', 'BUILDINGCLASS_HEROIC_EPIC', 'BUILDING_FORNIX');

UPDATE Buildings
SET WLTKDTurns = 10
WHERE Type = 'BUILDING_FORNIX';

INSERT INTO Building_ExtraPlayerInstancesFromAccomplishments
	(BuildingType, AccomplishmentType, ExtraInstances)
VALUES
	('BUILDING_FORNIX', 'ACCOMPLISHMENT_ELIMINATE_PLAYER', 1);

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_HEROIC_EPIC' AND YieldType = 'YIELD_CULTURE') + 1
WHERE BuildingType = 'BUILDING_FORNIX' AND YieldType = 'YIELD_CULTURE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_FORNIX', 'YIELD_TOURISM', 2);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_FORNIX', 'BUILDINGCLASS_AQUEDUCT', 'YIELD_GOLD', 1),
	('BUILDING_FORNIX', 'BUILDINGCLASS_AQUEDUCT', 'YIELD_TOURISM', 1);

INSERT INTO Building_BuildingClassLocalYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_FORNIX', 'BUILDINGCLASS_AQUEDUCT', 'YIELD_GOLD', 1),
	('BUILDING_FORNIX', 'BUILDINGCLASS_AQUEDUCT', 'YIELD_TOURISM', 1);
