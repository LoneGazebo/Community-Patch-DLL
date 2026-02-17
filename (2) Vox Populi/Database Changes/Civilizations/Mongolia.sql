----------------------------------------------------------
-- Unique Ability: Mongol Terror
----------------------------------------------------------
UPDATE Traits SET CityStateCombatModifier = 0 WHERE Type = 'TRAIT_TERROR';

DELETE FROM Trait_MovesChangeUnitCombats WHERE TraitType = 'TRAIT_TERROR';

INSERT INTO Trait_FreePromotionUnitClass
	(TraitType, UnitClassType, PromotionType)
SELECT
	'TRAIT_TERROR', Type, 'PROMOTION_LOGISTICS'
FROM UnitClasses
WHERE DefaultUnit IN (
	SELECT Type FROM Units WHERE IsMounted = 1
);

INSERT INTO Trait_YieldFromMinorDemand
	(TraitType, YieldType, Yield)
SELECT
	'TRAIT_TERROR', Type, 25
FROM Yields WHERE Type IN ('YIELD_PRODUCTION', 'YIELD_SCIENCE');

----------------------------------------------------------
-- Unique Unit: Khan (Great General)
----------------------------------------------------------
UPDATE Units
SET
	Moves = (SELECT Moves FROM Units WHERE Type = 'UNIT_GREAT_GENERAL') + 3,
	UnitFlagAtlas = 'UNIT_FLAG_ATLAS_VP_1',
	UnitFlagIconOffset = 60
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
	('UNIT_MONGOLIAN_KHAN', 'BUILD_ORDO');

INSERT INTO BuildFeatures
	(BuildType, FeatureType, Remove)
VALUES
	('BUILD_ORDO', 'FEATURE_FOREST', 1),
	('BUILD_ORDO', 'FEATURE_JUNGLE', 1),
	('BUILD_ORDO', 'FEATURE_MARSH', 1);

UPDATE Improvements
SET
	InAdjacentFriendly = 1,
	BuildableOnResources = 1,
	ConnectsAllResources = 1,
	NoTwoAdjacent = 1,
	CultureBombRadius = 1,
	MakesPassable = 1,
	FreeMoveAcross = 1,
	DefenseModifier = 50,
	NoFollowUp = 1
WHERE Type = 'IMPROVEMENT_ORDO';

INSERT INTO Improvement_ValidTerrains
	(ImprovementType, TerrainType)
VALUES
	('IMPROVEMENT_ORDO', 'TERRAIN_GRASS'),
	('IMPROVEMENT_ORDO', 'TERRAIN_PLAINS'),
	('IMPROVEMENT_ORDO', 'TERRAIN_DESERT'),
	('IMPROVEMENT_ORDO', 'TERRAIN_TUNDRA'),
	('IMPROVEMENT_ORDO', 'TERRAIN_SNOW');

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_ORDO', 'YIELD_FOOD', 1),
	('IMPROVEMENT_ORDO', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_ORDO', 'YIELD_GOLD', 1),
	('IMPROVEMENT_ORDO', 'YIELD_CULTURE', 1);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_ORDO', 'TECH_CHEMISTRY', 'YIELD_CULTURE', 2),
	('IMPROVEMENT_ORDO', 'TECH_MILITARY_SCIENCE', 'YIELD_PRODUCTION', 2),
	('IMPROVEMENT_ORDO', 'TECH_ELECTRONICS', 'YIELD_PRODUCTION', 4),
	('IMPROVEMENT_ORDO', 'TECH_STEALTH', 'YIELD_CULTURE', 4);

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_NAVAL_TRADITION', 'IMPROVEMENT_ORDO', 'YIELD_SCIENCE', 2),
	('POLICY_NAVAL_TRADITION', 'IMPROVEMENT_ORDO', 'YIELD_CULTURE', 1),
	('POLICY_NEW_DEAL', 'IMPROVEMENT_ORDO', 'YIELD_PRODUCTION', 6);

----------------------------------------------------------
-- Unique Unit: Black Tug (Knight)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_MONGOL', 'UNITCLASS_KNIGHT', 'UNIT_BLACK_TUG');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_KNIGHT'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_KNIGHT') + 1
WHERE Type = 'UNIT_BLACK_TUG';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_BLACK_TUG', 'PROMOTION_CHARGE'),
	('UNIT_BLACK_TUG', 'PROMOTION_MINGGHAN');

----------------------------------------------------------
-- Unique Building: Ger (Smokehouse)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_MONGOL', 'BUILDINGCLASS_SMOKEHOUSE', 'BUILDING_GER');

UPDATE Buildings
SET
	PlotBuyCostModifier = -25,
	BorderGrowthRateIncrease = 40
WHERE Type = 'BUILDING_GER';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
--	('BUILDING_GER', 'YIELD_CULTURE_LOCAL', 2),
	('BUILDING_GER', 'YIELD_FAITH', 1);

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
VALUES
--	('BUILDING_GER', 'IMPROVEMENT_CAMP', 'YIELD_PRODUCTION', 1),
	('BUILDING_GER', 'IMPROVEMENT_PASTURE', 'YIELD_PRODUCTION', 1);

INSERT INTO Building_YieldFromBorderGrowth
	(BuildingType, YieldType, Yield)
VALUES
--	('BUILDING_GER', 'YIELD_FOOD', 5),
	('BUILDING_GER', 'YIELD_PRODUCTION', 5);

----------------------------------------------------------
-- Unique Project: Adopt the Yassa
----------------------------------------------------------
UPDATE Projects
SET
	Cost = 200,
	EmpireSizeModifierPerCityMod = -40,
	TechPrereq = 'TECH_PHILOSOPHY'
WHERE Type = 'PROJECT_YASSA';

INSERT INTO Project_UnitCombatProductionModifiersGlobal
	(ProjectType, UnitCombatType, Modifier)
VALUES
	('PROJECT_YASSA', 'UNITCOMBAT_ARCHER', 20),
	('PROJECT_YASSA', 'UNITCOMBAT_SIEGE', 20),
	('PROJECT_YASSA', 'UNITCOMBAT_MOUNTED', 20),
	('PROJECT_YASSA', 'UNITCOMBAT_ARMOR', 20);

INSERT INTO Project_YieldFromConquestAllCities
	(ProjectType, YieldType, Yield)
VALUES
	('PROJECT_YASSA', 'YIELD_GOLD', 10),
	('PROJECT_YASSA', 'YIELD_SCIENCE', 10),
	('PROJECT_YASSA', 'YIELD_CULTURE', 10);
