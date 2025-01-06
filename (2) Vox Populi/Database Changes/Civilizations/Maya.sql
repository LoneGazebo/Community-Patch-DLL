----------------------------------------------------------
-- Unique Ability: The Long Count
----------------------------------------------------------
UPDATE Traits
SET PrereqTech = 'TECH_MATHEMATICS'
WHERE Type = 'TRAIT_LONG_COUNT';

----------------------------------------------------------
-- Unique Unit: Atlatlist (Composite Bowman)
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_COMPOSITE_BOWMAN' WHERE UnitType = 'UNIT_MAYAN_ATLATLIST';

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_COMPOSITE_BOWMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_COMPOSITE_BOWMAN') + 1
WHERE Type = 'UNIT_MAYAN_ATLATLIST';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_MAYAN_ATLATLIST', 'PROMOTION_INDIRECT_FIRE');

----------------------------------------------------------
-- Unique Unit: Holkan (Pathfinder)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_MAYA', 'UNITCLASS_PATHFINDER', 'UNIT_HOLKAN');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_SHOSHONE_PATHFINDER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_SHOSHONE_PATHFINDER') + 2
WHERE Type = 'UNIT_HOLKAN';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_HOLKAN', 'PROMOTION_TRAILBLAZER_1'),
	('UNIT_HOLKAN', 'PROMOTION_LOST_CODEX');

----------------------------------------------------------
-- Unique Improvement: Kuna
----------------------------------------------------------
UPDATE Builds
SET
	PrereqTech = 'TECH_MASONRY', -- Construction
	Time = 600
WHERE Type = 'BUILD_KUNA';

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_WORKER', 'BUILD_KUNA');

UPDATE Improvements
SET
	RequiresFeature = 1,
	NoTwoAdjacent = 1
WHERE Type = 'IMPROVEMENT_KUNA';

INSERT INTO Improvement_ValidFeatures
	(ImprovementType, FeatureType)
VALUES
	('IMPROVEMENT_KUNA', 'FEATURE_FOREST'),
	('IMPROVEMENT_KUNA', 'FEATURE_JUNGLE');

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_KUNA', 'YIELD_SCIENCE', 1),
	('IMPROVEMENT_KUNA', 'YIELD_FAITH', 2);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_KUNA', 'TECH_MATHEMATICS', 'YIELD_SCIENCE', 1),
	('IMPROVEMENT_KUNA', 'TECH_THEOLOGY', 'YIELD_SCIENCE', 1),
	('IMPROVEMENT_KUNA', 'TECH_ASTRONOMY', 'YIELD_SCIENCE', 2),
	('IMPROVEMENT_KUNA', 'TECH_ARCHAEOLOGY', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_KUNA', 'TECH_ARCHAEOLOGY', 'YIELD_SCIENCE', 2),
	('IMPROVEMENT_KUNA', 'TECH_FLIGHT', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_KUNA', 'TECH_FLIGHT', 'YIELD_CULTURE', 2);

----------------------------------------------------------
-- Unique Building: Pitz Court (Arena)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_MAYA', 'BUILDINGCLASS_COLOSSEUM', 'BUILDING_PITZ_COURT');

INSERT INTO Building_YieldFromLongCount
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_PITZ_COURT', 'YIELD_SCIENCE', 10),
	('BUILDING_PITZ_COURT', 'YIELD_FAITH', 10);

INSERT INTO Building_YieldChangesFromAccomplishments
	(BuildingType, AccomplishmentType, YieldType, Yield)
VALUES
	('BUILDING_PITZ_COURT', 'ACCOMPLISHMENT_LONGCOUNT_PROPHET', 'YIELD_FAITH', 1),
	('BUILDING_PITZ_COURT', 'ACCOMPLISHMENT_LONGCOUNT_WRITER', 'YIELD_CULTURE', 1),
	('BUILDING_PITZ_COURT', 'ACCOMPLISHMENT_LONGCOUNT_ARTIST', 'YIELD_GOLDEN_AGE_POINTS', 2),
	('BUILDING_PITZ_COURT', 'ACCOMPLISHMENT_LONGCOUNT_ENGINEER', 'YIELD_PRODUCTION', 2),
	('BUILDING_PITZ_COURT', 'ACCOMPLISHMENT_LONGCOUNT_MERCHANT', 'YIELD_FOOD', 1),
	('BUILDING_PITZ_COURT', 'ACCOMPLISHMENT_LONGCOUNT_MERCHANT', 'YIELD_GOLD', 1),
	('BUILDING_PITZ_COURT', 'ACCOMPLISHMENT_LONGCOUNT_SCIENTIST', 'YIELD_SCIENCE', 1);

INSERT INTO Building_BonusFromAccomplishments
	(BuildingType, AccomplishmentType, Happiness)
VALUES
	('BUILDING_PITZ_COURT', 'ACCOMPLISHMENT_LONGCOUNT_MUSICIAN', 1);

INSERT INTO Building_BonusFromAccomplishments
	(BuildingType, AccomplishmentType, DomainType, DomainXP)
VALUES
	('BUILDING_PITZ_COURT', 'ACCOMPLISHMENT_LONGCOUNT_GENERAL', 'DOMAIN_LAND', 5),
	('BUILDING_PITZ_COURT', 'ACCOMPLISHMENT_LONGCOUNT_ADMIRAL', 'DOMAIN_SEA', 5);

INSERT INTO Building_BonusFromAccomplishments
	(BuildingType, AccomplishmentType, UnitCombatType, UnitProductionModifier)
VALUES
	('BUILDING_PITZ_COURT', 'ACCOMPLISHMENT_LONGCOUNT_DIPLOMAT', 'UNITCOMBAT_DIPLOMACY', 5);
