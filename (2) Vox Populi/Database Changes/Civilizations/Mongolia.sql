----------------------------------------------------------
-- Unique Ability: Mongol Terror
----------------------------------------------------------
UPDATE Traits SET CityStateCombatModifier = 0 WHERE Type = 'TRAIT_TERROR';

DELETE FROM Trait_MovesChangeUnitCombats WHERE TraitType = 'TRAIT_TERROR';

INSERT INTO Trait_FreePromotionUnitClass
	(TraitType, UnitClassType, PromotionType)
SELECT
	'TRAIT_TERROR', Type, 'PROMOTION_MONGOL_TERROR'
FROM UnitClasses
WHERE DefaultUnit IN (
	SELECT Type FROM Units WHERE IsMounted = 1
);

INSERT INTO Trait_YieldFromMinorDemand
	(TraitType, YieldType, Yield)
SELECT
	'TRAIT_TERROR', Type, 20
FROM Yields
WHERE ID < 6; -- "All" yields

----------------------------------------------------------
-- Unique Unit: Khan (Great General)
----------------------------------------------------------
UPDATE Units
SET
	Moves = (SELECT Moves FROM Units WHERE Type = 'UNIT_GREAT_GENERAL') + 3,
	UnitFlagAtlas = 'UNIT_KHAN_FLAG_ATLAS',
	UnitFlagIconOffset = 0
WHERE Type = 'UNIT_MONGOLIAN_KHAN';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_MONGOLIAN_KHAN', 'PROMOTION_MEDIC_GENERAL'),
	('UNIT_MONGOLIAN_KHAN', 'PROMOTION_MEDIC'),
	('UNIT_MONGOLIAN_KHAN', 'PROMOTION_MEDIC_II');

----------------------------------------------------------
-- Unique Improvement: Ordo
----------------------------------------------------------
DELETE FROM Unit_Builds WHERE UnitType = 'UNIT_MONGOLIAN_KHAN' AND BuildType = 'BUILD_CITADEL';

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_MONGOLIAN_KHAN', 'BUILD_MONGOLIA_ORDO');

INSERT INTO BuildFeatures
	(BuildType, FeatureType, Remove)
VALUES
	('BUILD_MONGOLIA_ORDO', 'FEATURE_FOREST', 1),
	('BUILD_MONGOLIA_ORDO', 'FEATURE_JUNGLE', 1),
	('BUILD_MONGOLIA_ORDO', 'FEATURE_MARSH', 1);

UPDATE Improvements
SET
	InAdjacentFriendly = 1,
	BuildableOnResources = 1,
	ConnectsAllResources = 1,
	NoTwoAdjacent = 1,
	CreatedByGreatPerson = 1,
	CultureBombRadius = 1,
	MakesPassable = 1,
	RestoreMoves = 1,
	DefenseModifier = 50,
	NoFollowUp = 1
WHERE Type = 'IMPROVEMENT_MONGOLIA_ORDO';

INSERT INTO Improvement_ValidTerrains
	(ImprovementType, TerrainType)
VALUES
	('IMPROVEMENT_MONGOLIA_ORDO', 'TERRAIN_GRASS'),
	('IMPROVEMENT_MONGOLIA_ORDO', 'TERRAIN_PLAINS'),
	('IMPROVEMENT_MONGOLIA_ORDO', 'TERRAIN_DESERT'),
	('IMPROVEMENT_MONGOLIA_ORDO', 'TERRAIN_TUNDRA'),
	('IMPROVEMENT_MONGOLIA_ORDO', 'TERRAIN_SNOW');

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_MONGOLIA_ORDO', 'YIELD_FOOD', 1),
	('IMPROVEMENT_MONGOLIA_ORDO', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_MONGOLIA_ORDO', 'YIELD_GOLD', 1),
	('IMPROVEMENT_MONGOLIA_ORDO', 'YIELD_CULTURE', 1);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_MONGOLIA_ORDO', 'TECH_CHEMISTRY', 'YIELD_CULTURE', 2),
	('IMPROVEMENT_MONGOLIA_ORDO', 'TECH_MILITARY_SCIENCE', 'YIELD_PRODUCTION', 2),
	('IMPROVEMENT_MONGOLIA_ORDO', 'TECH_ELECTRONICS', 'YIELD_PRODUCTION', 4),
	('IMPROVEMENT_MONGOLIA_ORDO', 'TECH_STEALTH', 'YIELD_CULTURE', 4);

----------------------------------------------------------
-- Unique Building: Ger (Smokehouse)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_MONGOL', 'BUILDINGCLASS_LODGE', 'BUILDING_YURT');

UPDATE Buildings
SET
	PlotBuyCostModifier = -25,
	BorderGrowthRateIncrease = 40
WHERE Type = 'BUILDING_YURT';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
--	('BUILDING_YURT', 'YIELD_CULTURE_LOCAL', 2),
	('BUILDING_YURT', 'YIELD_FAITH', 1);

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
VALUES
--	('BUILDING_YURT', 'IMPROVEMENT_CAMP', 'YIELD_PRODUCTION', 1),
	('BUILDING_YURT', 'IMPROVEMENT_PASTURE', 'YIELD_PRODUCTION', 1);

INSERT INTO Building_YieldFromBorderGrowth
	(BuildingType, YieldType, Yield)
VALUES
--	('BUILDING_YURT', 'YIELD_FOOD', 5),
	('BUILDING_YURT', 'YIELD_PRODUCTION', 5);
