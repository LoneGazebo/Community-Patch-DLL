----------------------------------------------------------
-- Unique Ability: Achaemenid Legacy
----------------------------------------------------------
UPDATE Traits
SET
	GoldToGAP = 15,
	GoldenAgeCombatModifier = 15
WHERE Type = 'TRAIT_ENHANCED_GOLDEN_AGES';

----------------------------------------------------------
-- Unique Unit: Immortal (Spearman)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_SPEARMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_SPEARMAN') + 1
WHERE Type = 'UNIT_PERSIAN_IMMORTAL';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_PERSIAN_IMMORTAL', 'PROMOTION_FASTER_HEAL'),
	('UNIT_PERSIAN_IMMORTAL', 'PROMOTION_ARMOR_PLATING_1');

----------------------------------------------------------
-- Unique Unit: Qizilbash (Lancer)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_PERSIA', 'UNITCLASS_LANCER', 'UNIT_QIZILBASH');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_LANCER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_LANCER') + 4
WHERE Type = 'UNIT_QIZILBASH';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_QIZILBASH', 'PROMOTION_SAFAVI_AGITATOR');

----------------------------------------------------------
-- Unique Improvement: Charbagh
----------------------------------------------------------
UPDATE Builds
SET
	PrereqTech = 'TECH_THEOLOGY',
	Time = 600
WHERE Type = 'BUILD_CHARBAGH';

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_WORKER', 'BUILD_CHARBAGH');

INSERT INTO BuildFeatures
	(BuildType, FeatureType, Time, Remove)
VALUES
	('BUILD_CHARBAGH', 'FEATURE_FOREST', 400, 1),
	('BUILD_CHARBAGH', 'FEATURE_JUNGLE', 500, 1),
	('BUILD_CHARBAGH', 'FEATURE_MARSH', 500, 1);

UPDATE Improvements
SET
	FreshWaterMakesValid = 1,
	NoTwoAdjacent = 1,
	GreatPersonRateModifier = 5
WHERE Type = 'IMPROVEMENT_CHARBAGH';

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_CHARBAGH', 'YIELD_FOOD', 2),
	('IMPROVEMENT_CHARBAGH', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_CHARBAGH', 'YIELD_GOLD', 1),
	('IMPROVEMENT_CHARBAGH', 'YIELD_CULTURE', 1);

INSERT INTO Improvement_GoldenAgeYields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_CHARBAGH', 'YIELD_FOOD', 1),
	('IMPROVEMENT_CHARBAGH', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_CHARBAGH', 'YIELD_CULTURE', 1);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_CHARBAGH', 'TECH_BANKING', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_CHARBAGH', 'TECH_BANKING', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_CHARBAGH', 'TECH_FERTILIZER', 'YIELD_FOOD', 2),
	('IMPROVEMENT_CHARBAGH', 'TECH_FLIGHT', 'YIELD_GOLD', 2);

----------------------------------------------------------
-- Unique Building: Satrap's Court (Courthouse)
----------------------------------------------------------
UPDATE Civilization_BuildingClassOverrides
SET BuildingClassType = 'BUILDINGCLASS_COURTHOUSE'
WHERE BuildingType = 'BUILDING_SATRAPS_COURT';

UPDATE Buildings
SET
	BuildAnywhere = 1,
	Happiness = 1,
	SpecialistType = 'SPECIALIST_MERCHANT',
	SpecialistCount = 1
WHERE Type = 'BUILDING_SATRAPS_COURT';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SATRAPS_COURT', 'YIELD_GOLD', 1);

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SATRAPS_COURT', 'YIELD_GOLD', 34);

INSERT INTO Building_SpecialistYieldChangesLocal
	(BuildingType, SpecialistType, YieldType, Yield)
VALUES
	('BUILDING_SATRAPS_COURT', 'SPECIALIST_SCIENTIST', 'YIELD_GOLD', 1),
	('BUILDING_SATRAPS_COURT', 'SPECIALIST_MERCHANT', 'YIELD_GOLD', 1),
	('BUILDING_SATRAPS_COURT', 'SPECIALIST_ENGINEER', 'YIELD_GOLD', 1);
