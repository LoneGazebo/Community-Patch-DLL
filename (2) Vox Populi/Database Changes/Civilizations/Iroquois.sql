----------------------------------------------------------
-- Unique Ability: The Great Warpath
----------------------------------------------------------
INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
SELECT
	'TRAIT_IGNORE_TERRAIN_IN_FOREST', Type, 'PROMOTION_WOODSMAN'
FROM UnitCombatInfos
WHERE IsMilitary = 1 AND IsNaval = 0 AND IsAerial = 0;

----------------------------------------------------------
-- Unique Unit: Mohawk Warrior (Swordsman)
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
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_SWORDSMAN') + 1
WHERE Type = 'UNIT_IROQUOIAN_MOHAWKWARRIOR';

DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_IROQUOIAN_MOHAWKWARRIOR';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_IROQUOIAN_MOHAWKWARRIOR', 'PROMOTION_WOODSMAN'),
	('UNIT_IROQUOIAN_MOHAWKWARRIOR', 'PROMOTION_MOHAWK');

----------------------------------------------------------
-- Unique Building: Longhouse (Herbalist)
----------------------------------------------------------
UPDATE Civilization_BuildingClassOverrides
SET BuildingClassType = 'BUILDINGCLASS_HERBALIST'
WHERE BuildingType = 'BUILDING_LONGHOUSE';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_HERBALIST' AND YieldType = 'YIELD_FOOD') + 1
WHERE BuildingType = 'BUILDING_LONGHOUSE' AND YieldType = 'YIELD_FOOD';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_LONGHOUSE', 'YIELD_CULTURE', 1);

DELETE FROM Building_YieldPerXFeatureTimes100 WHERE BuildingType = 'BUILDING_LONGHOUSE';

INSERT INTO Building_FeatureYieldChanges
	(BuildingType, FeatureType, YieldType, Yield)
VALUES
--	('BUILDING_LONGHOUSE', 'FEATURE_MARSH', 'YIELD_PRODUCTION', 1),
	('BUILDING_LONGHOUSE', 'FEATURE_FOREST', 'YIELD_FOOD', 1),
	('BUILDING_LONGHOUSE', 'FEATURE_FOREST', 'YIELD_PRODUCTION', 1),
	('BUILDING_LONGHOUSE', 'FEATURE_JUNGLE', 'YIELD_FOOD', 1),
	('BUILDING_LONGHOUSE', 'FEATURE_JUNGLE', 'YIELD_PRODUCTION', 1),
	('BUILDING_LONGHOUSE', 'FEATURE_MARSH', 'YIELD_FOOD', 1);

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
VALUES
--	('BUILDING_LONGHOUSE', 'IMPROVEMENT_PLANTATION', 'YIELD_PRODUCTION', 1),
	('BUILDING_LONGHOUSE', 'IMPROVEMENT_PLANTATION', 'YIELD_FOOD', 1);
