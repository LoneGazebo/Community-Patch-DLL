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
	PrereqTech = 'TECH_MATHEMATICS',
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_COMPOSITE_BOWMAN'
			)
		)
	)
WHERE Type = 'UNIT_MAYAN_ATLATLIST';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_MAYAN_ATLATLIST', 'PROMOTION_INDIRECT_FIRE');

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
	('IMPROVEMENT_KUNA', 'TECH_THEOLOGY', 'YIELD_SCIENCE', 2),
	('IMPROVEMENT_KUNA', 'TECH_ASTRONOMY', 'YIELD_SCIENCE', 2),
	('IMPROVEMENT_KUNA', 'TECH_ARCHAEOLOGY', 'YIELD_SCIENCE', 2),
	('IMPROVEMENT_KUNA', 'TECH_FLIGHT', 'YIELD_CULTURE', 2);
