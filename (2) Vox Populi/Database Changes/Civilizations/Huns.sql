----------------------------------------------------------
-- Unique Ability: Scourge of God
----------------------------------------------------------
UPDATE Traits
SET
	RazeSpeedModifier = 0,
	WarWearinessModifier = 50,
	EnemyWarWearinessModifier = 100,
	MultipleAttackBonus = 10
WHERE Type = 'TRAIT_RAZE_AND_HORSES';

DELETE FROM Civilization_FreeTechs WHERE CivilizationType = 'CIVILIZATION_HUNS' AND TechType = 'TECH_ANIMAL_HUSBANDRY';
DELETE FROM Trait_ImprovementYieldChanges WHERE TraitType = 'TRAIT_RAZE_AND_HORSES';

INSERT INTO Trait_YieldFromCityDamageTimes100
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_RAZE_AND_HORSES', 'YIELD_GOLD', 50),
	('TRAIT_RAZE_AND_HORSES', 'YIELD_CULTURE', 50);

----------------------------------------------------------
-- Unique Unit: Horse Archer (Skirmisher)
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_HORSE_ARCHER' WHERE UnitType = 'UNIT_HUN_HORSE_ARCHER';

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_MONGOLIAN_KESHIK'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_MONGOLIAN_KESHIK') + 1,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_MONGOLIAN_KESHIK') + 1
WHERE Type = 'UNIT_HUN_HORSE_ARCHER';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_HUN_HORSE_ARCHER', 'PROMOTION_BARRAGE_1'),
	('UNIT_HUN_HORSE_ARCHER', 'PROMOTION_FOCUS_FIRE');

----------------------------------------------------------
-- Unique Improvement: Eki
----------------------------------------------------------
UPDATE Builds
SET
	PrereqTech = 'TECH_ARCHERY', -- Military Strategy
	Time = 600
WHERE Type = 'BUILD_EKI';

INSERT INTO Build_TechTimeChanges
	(BuildType, TechType, TimeChange)
VALUES
	('BUILD_EKI', 'TECH_CHIVALRY', -100);

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_WORKER', 'BUILD_EKI');

INSERT INTO BuildFeatures
	(BuildType, FeatureType, Time, Remove)
VALUES
	('BUILD_EKI', 'FEATURE_FOREST', 400, 1),
	('BUILD_EKI', 'FEATURE_JUNGLE', 500, 1),
	('BUILD_EKI', 'FEATURE_MARSH', 500, 1);

UPDATE Improvements
SET
	RequiresFlatlands = 1,
	NoFreshWater = 1,
	InAdjacentFriendly = 1,
	NewOwner = 1
WHERE Type = 'IMPROVEMENT_EKI';

INSERT INTO Improvement_ValidTerrains
	(ImprovementType, TerrainType)
VALUES
	('IMPROVEMENT_EKI', 'TERRAIN_GRASS'),
	('IMPROVEMENT_EKI', 'TERRAIN_PLAINS'),
	('IMPROVEMENT_EKI', 'TERRAIN_DESERT'),
	('IMPROVEMENT_EKI', 'TERRAIN_TUNDRA'),
	('IMPROVEMENT_EKI', 'TERRAIN_SNOW');

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_EKI', 'YIELD_FOOD', 1),
	('IMPROVEMENT_EKI', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_EKI', 'YIELD_CULTURE', 1);

INSERT INTO Improvement_YieldPerXAdjacentImprovement
	(ImprovementType, OtherImprovementType, YieldType, Yield, NumRequired)
VALUES
	('IMPROVEMENT_EKI', 'IMPROVEMENT_EKI', 'YIELD_PRODUCTION', 1, 2),
	('IMPROVEMENT_EKI', 'IMPROVEMENT_EKI', 'YIELD_GOLD', 1, 2),
	('IMPROVEMENT_EKI', 'IMPROVEMENT_MANUFACTORY', 'YIELD_PRODUCTION', 1, 2),
	('IMPROVEMENT_EKI', 'IMPROVEMENT_MANUFACTORY', 'YIELD_GOLD', 1, 2);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_EKI', 'TECH_CHIVALRY', 'YIELD_FOOD', 1),
	('IMPROVEMENT_EKI', 'TECH_CHIVALRY', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_EKI', 'TECH_FERTILIZER', 'YIELD_FOOD', 1),
	('IMPROVEMENT_EKI', 'TECH_ROBOTICS', 'YIELD_PRODUCTION', 3);
