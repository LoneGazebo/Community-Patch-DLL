----------------------------------------------------------
-- Unique Ability: Mandate of Heaven
----------------------------------------------------------
UPDATE Traits
SET
	GreatGeneralRateModifier = 0,
	GreatGeneralExtraBonus = 0,
	GreatWorkWLTKD = 1,
	ExpansionWLTKD = 1,
	PermanentYieldsDecreaseEveryEra = 1
WHERE Type = 'TRAIT_ART_OF_WAR';

INSERT INTO Trait_PermanentYieldChangeWLTKD
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_ART_OF_WAR', 'YIELD_FOOD', 2);

----------------------------------------------------------
-- Unique Unit: Chu-Ko-Nu (Crossbowman)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CROSSBOWMAN'
			)
		)
	),
	Combat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_CROSSBOWMAN') + 1
WHERE Type = 'UNIT_CHINESE_CHUKONU';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_CHINESE_CHUKONU', 'PROMOTION_LOGISTICS');

----------------------------------------------------------
-- Unique Unit: Treasure Ship (Frigate)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_CHINA', 'UNITCLASS_FRIGATE', 'UNIT_TREASURE_SHIP');

UPDATE Units
SET
	PrereqTech = 'TECH_ASTRONOMY',
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_FRIGATE'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_FRIGATE') + 3,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_FRIGATE') + 2
WHERE Type = 'UNIT_TREASURE_SHIP';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_TREASURE_SHIP', 'PROMOTION_COASTAL_RAIDER_3'),
	('UNIT_TREASURE_SHIP', 'PROMOTION_GAIN_EXPERIENCE'),
	('UNIT_TREASURE_SHIP', 'PROMOTION_KOWTOW');

----------------------------------------------------------
-- Unique Improvement: Siheyuan
----------------------------------------------------------
UPDATE Builds
SET
	PrereqTech = 'TECH_CURRENCY',
	Time = 600
WHERE Type = 'BUILD_SIHEYUAN';

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_WORKER', 'BUILD_SIHEYUAN');

INSERT INTO BuildFeatures
	(BuildType, FeatureType, Time, Remove)
VALUES
	('BUILD_SIHEYUAN', 'FEATURE_FOREST', 400, 1),
	('BUILD_SIHEYUAN', 'FEATURE_JUNGLE', 500, 1),
	('BUILD_SIHEYUAN', 'FEATURE_MARSH', 500, 1);

UPDATE Improvements
SET
	RequiresFlatlands = 1,
	Cityside = 1, -- Makes valid
	XSameAdjacentMakesValid = 2
WHERE Type = 'IMPROVEMENT_SIHEYUAN';

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_SIHEYUAN', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_SIHEYUAN', 'YIELD_GOLD', 1),
	('IMPROVEMENT_SIHEYUAN', 'YIELD_SCIENCE', 1);

INSERT INTO Improvement_WLTKDYields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_SIHEYUAN', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_SIHEYUAN', 'YIELD_GOLD', 1),
	('IMPROVEMENT_SIHEYUAN', 'YIELD_SCIENCE', 1);

INSERT INTO Improvement_GoldenAgeYields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_SIHEYUAN', 'YIELD_GREAT_GENERAL_POINTS', 1),
	('IMPROVEMENT_SIHEYUAN', 'YIELD_GREAT_ADMIRAL_POINTS', 1);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_SIHEYUAN', 'TECH_CIVIL_SERVICE', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_SIHEYUAN', 'TECH_ARCHITECTURE', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_SIHEYUAN', 'TECH_ARCHITECTURE', 'YIELD_SCIENCE', 1),
	('IMPROVEMENT_SIHEYUAN', 'TECH_ELECTRICITY', 'YIELD_GOLD', 1),
	('IMPROVEMENT_SIHEYUAN', 'TECH_ELECTRICITY', 'YIELD_CULTURE', 1);

----------------------------------------------------------
-- Unique Building: Examination Hall (Chancery)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_CHINA', 'BUILDINGCLASS_CHANCERY', 'BUILDING_EXAMINATION_HALL');

UPDATE Buildings
SET GPPOnCitizenBirth = 15
WHERE Type = 'BUILDING_EXAMINATION_HALL';

INSERT INTO Building_GreatWorkYieldChangesLocal
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_EXAMINATION_HALL', 'YIELD_GOLD', 2);
