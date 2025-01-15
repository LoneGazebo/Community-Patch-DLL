----------------------------------------------------------
-- Unique Ability: Father Governs Children
----------------------------------------------------------
UPDATE Traits SET CityStateBonusModifier = 100 WHERE Type = 'TRAIT_CITY_STATE_BONUSES';

----------------------------------------------------------
-- Unique Unit: Naresuan's Elephant (Knight)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_KNIGHT'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_KNIGHT') + 2,
	Moves = (SELECT Moves FROM Units WHERE Type = 'UNIT_KNIGHT') - 1
WHERE Type = 'UNIT_SIAMESE_WARELEPHANT';

DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_SIAMESE_WARELEPHANT';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_SIAMESE_WARELEPHANT', 'PROMOTION_GENERAL_STACKING'),
	('UNIT_SIAMESE_WARELEPHANT', 'PROMOTION_ANTI_MOUNTED_I'),
	('UNIT_SIAMESE_WARELEPHANT', 'PROMOTION_FEARED_ELEPHANT');

----------------------------------------------------------
-- Unique Unit: Suea Mop (Field Gun)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_SIAM', 'UNITCLASS_FIELD_GUN', 'UNIT_SUEA_MOP');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_FIELD_GUN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_FIELD_GUN') + 2,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_FIELD_GUN') + 5
WHERE Type = 'UNIT_SUEA_MOP';

DELETE FROM Unit_FreePromotions WHERE UnitType = 'UNIT_SUEA_MOP' AND PromotionType = 'PROMOTION_CITY_SIEGE';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_SUEA_MOP', 'PROMOTION_COASTAL_ARTILLERY'),
	('UNIT_SUEA_MOP', 'PROMOTION_CROUCHING_TIGER');

----------------------------------------------------------
-- Unique Building: Wat
----------------------------------------------------------
UPDATE Civilization_BuildingClassOverrides SET BuildingClassType = 'BUILDINGCLASS_WAT' WHERE BuildingType = 'BUILDING_WAT';

UPDATE Buildings
SET
	PrereqTech = 'TECH_THEOLOGY',
	DiplomatInfluenceBoost = 10,
	InstantReligiousPressure = 10000,
	ReligiousUnrestFlatReduction = 1
WHERE Type = 'BUILDING_WAT';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_WAT', 'YIELD_SCIENCE', 2),
	('BUILDING_WAT', 'YIELD_CULTURE', 1),
	('BUILDING_WAT', 'YIELD_FAITH', 3);

INSERT INTO Building_BuildingClassLocalYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_WAT', 'BUILDINGCLASS_SHRINE', 'YIELD_SCIENCE', 3),
	('BUILDING_WAT', 'BUILDINGCLASS_TEMPLE', 'YIELD_SCIENCE', 3);

----------------------------------------------------------
-- Unique Building: Elephant Camp (Workshop)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_SIAM', 'BUILDINGCLASS_WORKSHOP', 'BUILDING_ELEPHANT_CAMP');

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_WORKSHOP' AND YieldType = 'YIELD_PRODUCTION') + 1
WHERE BuildingType = 'BUILDING_ELEPHANT_CAMP' AND YieldType = 'YIELD_PRODUCTION';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_ELEPHANT_CAMP', 'YIELD_CULTURE', 2),
	('BUILDING_ELEPHANT_CAMP', 'YIELD_FAITH', 1);

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
VALUES
	('BUILDING_ELEPHANT_CAMP', 'IMPROVEMENT_LUMBERMILL', 'YIELD_CULTURE', 1);

INSERT INTO Building_YieldChangesFromXCityStateStrategicResource
	(BuildingType, YieldType, Yield, NumRequired)
VALUES
	('BUILDING_ELEPHANT_CAMP', 'YIELD_PRODUCTION', 1, 2),
	('BUILDING_ELEPHANT_CAMP', 'YIELD_GOLD', 1, 2);
