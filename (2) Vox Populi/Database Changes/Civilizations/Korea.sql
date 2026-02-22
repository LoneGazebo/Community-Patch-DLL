----------------------------------------------------------
-- Unique Ability: Scholars of the Jade Hall
----------------------------------------------------------
UPDATE Traits
SET
	TechBoostFromCapitalScienceBuildings = 0,
	IsOddEraScaler = 1
WHERE Type = 'TRAIT_SCHOLARS_JADE_HALL';

DELETE FROM Trait_ImprovementYieldChanges WHERE TraitType = 'TRAIT_SCHOLARS_JADE_HALL';
DELETE FROM Trait_SpecialistYieldChanges WHERE TraitType = 'TRAIT_SCHOLARS_JADE_HALL';

INSERT INTO Trait_SpecialistYieldChanges
	(TraitType, SpecialistType, YieldType, Yield)
SELECT
	'TRAIT_SCHOLARS_JADE_HALL', Type, 'YIELD_SCIENCE', 1
FROM Specialists
WHERE GreatPeopleUnitClass IS NOT NULL;

INSERT INTO Trait_GoldenAgeYieldModifiers
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_SCHOLARS_JADE_HALL', 'YIELD_SCIENCE', 20);

INSERT INTO Trait_GreatPersonBornYield
	(TraitType, GreatPersonType, YieldType, Yield)
SELECT
	'TRAIT_SCHOLARS_JADE_HALL', Type, 'YIELD_GOLDEN_AGE_POINTS', 50
FROM GreatPersons;

----------------------------------------------------------
-- Unique Unit: Hwach'a (Trebuchet)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_TREBUCHET'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_TREBUCHET') + 5,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_TREBUCHET') + 1,
	BaseSightRange = (SELECT BaseSightRange FROM Units WHERE Type = 'UNIT_TREBUCHET') + 1,
	DefaultUnitAI = 'UNITAI_RANGED'
WHERE Type = 'UNIT_KOREAN_HWACHA';

-- Not really a siege unit
DELETE FROM Unit_AITypes WHERE UnitType = 'UNIT_KOREAN_HWACHA' AND UnitAIType = 'UNITAI_CITY_BOMBARD';

DELETE FROM Unit_FreePromotions
WHERE UnitType = 'UNIT_KOREAN_HWACHA'
AND PromotionType IN (
	'PROMOTION_CITY_SIEGE',
	'PROMOTION_SIEGE_INACCURACY'
);

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_KOREAN_HWACHA', 'PROMOTION_SPLASH_1'),
	('UNIT_KOREAN_HWACHA', 'PROMOTION_LOGISTICS');

----------------------------------------------------------
-- Unique Unit: Turtle Ship (Caravel)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CARAVEL'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_CARAVEL') + 7
WHERE Type = 'UNIT_KOREAN_TURTLE_SHIP';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_KOREAN_TURTLE_SHIP', 'PROMOTION_SHALLOW_DRAFT'),
	('UNIT_KOREAN_TURTLE_SHIP', 'PROMOTION_DECK_SPIKES');

----------------------------------------------------------
-- Unique Building: Seowon (University)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_KOREA', 'BUILDINGCLASS_UNIVERSITY', 'BUILDING_SEOWON');

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
--	('BUILDING_SEOWON', 'YIELD_SCIENCE', 3),
	('BUILDING_SEOWON', 'YIELD_CULTURE', 2);

INSERT INTO Building_GreatWorkYieldChangesLocal
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SEOWON', 'YIELD_PRODUCTION', 1),
	('BUILDING_SEOWON', 'YIELD_SCIENCE', 1);

CREATE TEMP TABLE Helper (
	YieldType TEXT
);

INSERT INTO Helper
VALUES
	('YIELD_PRODUCTION'),
	('YIELD_SCIENCE');

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
SELECT
	'BUILDING_SEOWON', a.Type, b.YieldType, 1
FROM Improvements a, Helper b
WHERE a.CreatedByGreatPerson = 1;

DROP TABLE Helper;

UPDATE Building_GrowthExtraYield
SET Yield = (SELECT Yield FROM Building_GrowthExtraYield WHERE BuildingType = 'BUILDING_UNIVERSITY' AND YieldType = 'YIELD_SCIENCE') * 2
WHERE BuildingType = 'BUILDING_SEOWON' AND YieldType = 'YIELD_SCIENCE';

----------------------------------------------------------
-- Unique Building: Chaebol (Stock Exchange)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_KOREA', 'BUILDINGCLASS_STOCK_EXCHANGE', 'BUILDING_CHAEBOL');

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_STOCK_EXCHANGE' AND YieldType = 'YIELD_GOLD') + 2
WHERE BuildingType = 'BUILDING_CHAEBOL' AND YieldType = 'YIELD_GOLD';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CHAEBOL', 'YIELD_PRODUCTION', 4);

INSERT INTO Building_BuildingClassLocalYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_CHAEBOL', 'BUILDINGCLASS_FACTORY', 'YIELD_PRODUCTION', 4);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_CHAEBOL', 'BUILDINGCLASS_STOCK_EXCHANGE', 'YIELD_GOLD', 2),
	('BUILDING_CHAEBOL', 'BUILDINGCLASS_STOCK_EXCHANGE', 'YIELD_SCIENCE', 2),
	('BUILDING_CHAEBOL', 'BUILDINGCLASS_STOCK_EXCHANGE', 'YIELD_CULTURE', 2);

UPDATE Building_HurryModifiersLocal
SET HurryCostModifier = (SELECT HurryCostModifier FROM Building_HurryModifiersLocal WHERE BuildingType = 'BUILDING_STOCK_EXCHANGE') - 5
WHERE BuildingType = 'BUILDING_CHAEBOL';
