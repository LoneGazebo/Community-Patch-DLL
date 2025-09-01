----------------------------------------------------------
-- Unique Ability: The Great Warpath
----------------------------------------------------------
INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
SELECT
	'TRAIT_IGNORE_TERRAIN_IN_FOREST', Type, 'PROMOTION_WOODSMAN'
FROM UnitCombatInfos
WHERE IsMilitary = 1 AND IsNaval = 0 AND IsAerial = 0;

INSERT INTO Trait_GreatPersonProgressFromPolicyUnlock
	(TraitType, GreatPersonType, Value)
VALUES
	('TRAIT_IGNORE_TERRAIN_IN_FOREST', 'GREATPERSON_DIPLOMAT', 15);

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
-- Unique Unit: Prowler (Musketman)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_IROQUOIS', 'UNITCLASS_MUSKETMAN', 'UNIT_PROWLER');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_MUSKETMAN'
			)
		)
	),
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_MUSKETMAN') + 2
WHERE Type = 'UNIT_PROWLER';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_PROWLER', 'PROMOTION_CAN_MOVE_AFTER_ATTACKING'),
	('UNIT_PROWLER', 'PROMOTION_INFILTRATORS');

----------------------------------------------------------
-- Unique Unit: Tadodaho (Great Diplomat)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_IROQUOIS', 'UNITCLASS_GREAT_DIPLOMAT', 'UNIT_TADODAHO');

UPDATE Units
SET
	RestingPointChange = (SELECT RestingPointChange FROM Units WHERE Type = 'UNIT_GREAT_DIPLOMAT') + 15,
	CopyYieldsFromExpendTile = 1
WHERE Type = 'UNIT_TADODAHO';


INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_TADODAHO', 'BUILD_EMBASSY');

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
