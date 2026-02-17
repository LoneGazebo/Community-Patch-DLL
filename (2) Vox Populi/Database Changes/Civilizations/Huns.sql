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
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_SKIRMISHER' WHERE UnitType = 'UNIT_HUN_HORSE_ARCHER';

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
-- Unique Unit: Tarkhan (Horseman)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_HUNS', 'UNITCLASS_HORSEMAN', 'UNIT_TARKHAN');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_HORSEMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_HORSEMAN') + 1
WHERE Type = 'UNIT_TARKHAN';

DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_TARKHAN';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_TARKHAN', 'PROMOTION_BELLUM_ALET');

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
	('IMPROVEMENT_EKI', 'IMPROVEMENT_ACADEMY', 'YIELD_PRODUCTION', 1, 2),
	('IMPROVEMENT_EKI', 'IMPROVEMENT_ACADEMY', 'YIELD_GOLD', 1, 2);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_EKI', 'TECH_CHIVALRY', 'YIELD_FOOD', 1),
	('IMPROVEMENT_EKI', 'TECH_CHIVALRY', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_EKI', 'TECH_FERTILIZER', 'YIELD_FOOD', 1),
	('IMPROVEMENT_EKI', 'TECH_ROBOTICS', 'YIELD_PRODUCTION', 3);

----------------------------------------------------------
-- Unique Unit: Ulticur (Circus Maximus)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_HUNS', 'BUILDINGCLASS_CIRCUS_MAXIMUS', 'BUILDING_ULTICUR');

UPDATE Buildings
SET
	GlobalHappinessPerMajorWar = 2,
	GlobalMilitaryProductionModPerMajorWar = 5
WHERE Type = 'BUILDING_ULTICUR';

UPDATE Building_ResourceQuantity
SET Quantity = (SELECT Quantity FROM Building_ResourceQuantity WHERE BuildingType = 'BUILDING_CIRCUS_MAXIMUS') * 2
WHERE BuildingType = 'BUILDING_ULTICUR';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_CIRCUS_MAXIMUS' AND YieldType = 'YIELD_CULTURE') + 2
WHERE BuildingType = 'BUILDING_ULTICUR' AND YieldType = 'YIELD_CULTURE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_ULTICUR', 'YIELD_PRODUCTION', 3);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_ULTICUR', 'BUILDINGCLASS_COLOSSEUM', 'YIELD_PRODUCTION', 3),
--	('BUILDING_ULTICUR', 'BUILDINGCLASS_COLOSSEUM', 'YIELD_GOLD', 2),
	('BUILDING_ULTICUR', 'BUILDINGCLASS_STABLE', 'YIELD_PRODUCTION', 2),
	('BUILDING_ULTICUR', 'BUILDINGCLASS_STABLE', 'YIELD_GOLD', 2);
