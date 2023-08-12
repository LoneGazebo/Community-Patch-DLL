-- Mosque

DELETE FROM Building_YieldChanges
WHERE BuildingType = 'BUILDING_MOSQUE' AND YieldType = 'YIELD_CULTURE';

UPDATE Building_YieldChanges
SET Yield = '3'
WHERE BuildingType = 'BUILDING_MOSQUE' AND YieldType = 'YIELD_FAITH';

UPDATE Buildings
SET ReligiousPressureModifier = '25'
WHERE Type = 'BUILDING_MOSQUE';

UPDATE Buildings
SET Happiness = '0'
WHERE Type = 'BUILDING_MOSQUE';

UPDATE Buildings
SET Help = 'TXT_KEY_BUILDING_MOSQUE_HELP'
WHERE Type = 'BUILDING_MOSQUE';

-- Cathedral

DELETE FROM Building_YieldChanges
WHERE BuildingType = 'BUILDING_CATHEDRAL' AND YieldType = 'YIELD_CULTURE';

UPDATE Building_YieldChanges
SET Yield = '2'
WHERE BuildingType = 'BUILDING_CATHEDRAL' AND YieldType = 'YIELD_FAITH';

UPDATE Buildings
SET Happiness = '0'
WHERE Type = 'BUILDING_CATHEDRAL';

UPDATE Buildings
SET ReligiousPressureModifier = '25'
WHERE Type = 'BUILDING_CATHEDRAL';

UPDATE Buildings
SET Help = 'TXT_KEY_BUILDING_CATHEDRAL_HELP'
WHERE Type = 'BUILDING_CATHEDRAL';

-- Pagoda

DELETE FROM Building_YieldChanges
WHERE BuildingType = 'BUILDING_PAGODA';

UPDATE Buildings
SET Happiness = '0'
WHERE Type = 'BUILDING_PAGODA';

UPDATE Buildings
SET Help = 'TXT_KEY_BUILDING_PAGODA_HELP'
WHERE Type = 'BUILDING_PAGODA';

-- New Building Yields
INSERT INTO Building_YieldModifiers
		(BuildingType, 			YieldType, 		Yield)
VALUES	('BUILDING_GURDWARA', 	'YIELD_FOOD', 	10);

INSERT INTO Building_WLTKDYieldMod
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SYNAGOGUE', 'YIELD_SCIENCE', 10);

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_STUPA', 'YIELD_FAITH', 3),
	('BUILDING_STUPA', 'YIELD_GOLDEN_AGE_POINTS', 3),
	('BUILDING_STUPA', 'YIELD_TOURISM', 5),
	('BUILDING_CHURCH', 'YIELD_FAITH', 4),
	('BUILDING_CHURCH', 'YIELD_CULTURE', 2),
	('BUILDING_MANDIR', 'YIELD_FAITH', 3),
	('BUILDING_MANDIR', 'YIELD_FOOD', 2),
	('BUILDING_SYNAGOGUE', 'YIELD_FAITH', 2),
	('BUILDING_SYNAGOGUE', 'YIELD_PRODUCTION', 3),
	('BUILDING_MOSQUE', 'YIELD_SCIENCE', 2),
	('BUILDING_CATHEDRAL', 'YIELD_GOLD', 2),
	('BUILDING_ORDER', 'YIELD_FAITH', 2),
	('BUILDING_ORDER', 'YIELD_GOLD', 3),
	('BUILDING_TEOCALLI', 'YIELD_FAITH', 2),
	('BUILDING_GURDWARA', 'YIELD_FAITH', 2),
	('BUILDING_GURDWARA', 'YIELD_FOOD', 3),
	('BUILDING_PAGODA', 'YIELD_FAITH', 2);

INSERT INTO Building_YieldChangesPerReligion
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_PAGODA', 'YIELD_FOOD', 100),
	('BUILDING_PAGODA', 'YIELD_GOLD', 100),
	('BUILDING_PAGODA', 'YIELD_CULTURE', 100),
	('BUILDING_PAGODA', 'YIELD_SCIENCE', 100),
	('BUILDING_PAGODA', 'YIELD_FAITH', 100),
	('BUILDING_PAGODA', 'YIELD_PRODUCTION', 100);

INSERT INTO Building_YieldFromBirth
		(BuildingType, 			YieldType, 				Yield)
VALUES	('BUILDING_MANDIR', 	'YIELD_FOOD', 			5),
		('BUILDING_MANDIR', 	'YIELD_SCIENCE', 		5),
		('BUILDING_MANDIR', 	'YIELD_CULTURE', 		5),
		('BUILDING_MANDIR', 	'YIELD_PRODUCTION', 	5),
		('BUILDING_MANDIR', 	'YIELD_FAITH', 			5),
		('BUILDING_MANDIR', 	'YIELD_GOLD', 			5);

INSERT INTO Building_GoldenAgeYieldMod
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MOSQUE', 'YIELD_CULTURE', 15);

INSERT INTO Building_YieldFromBorderGrowth
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CATHEDRAL', 'YIELD_GOLD', 10);

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
VALUES
	('BUILDING_CATHEDRAL', 'IMPROVEMENT_FARM', 'YIELD_GOLD', 1),
	('BUILDING_CATHEDRAL', 'IMPROVEMENT_PASTURE', 'YIELD_GOLD', 1),
	('BUILDING_CATHEDRAL', 'IMPROVEMENT_QUARRY', 'YIELD_GOLD', 1);

INSERT INTO Building_YieldFromVictoryGlobal
	(BuildingType, YieldType, Yield, IsEraScaling)
VALUES
	('BUILDING_TEOCALLI', 'YIELD_FAITH', 5, 'false');

INSERT INTO Building_GreatWorkYieldChangesLocal
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CHURCH', 'YIELD_FAITH', 1);

INSERT INTO Building_DomainFreeExperiences
	(BuildingType, DomainType, Experience)
VALUES
	('BUILDING_TEOCALLI', 'DOMAIN_LAND', 15),
	('BUILDING_TEOCALLI', 'DOMAIN_AIR', 15),
	('BUILDING_TEOCALLI', 'DOMAIN_SEA', 15);

INSERT INTO Building_YieldFromUnitProduction
		(BuildingType, YieldType, Yield)
VALUES	('BUILDING_ORDER', 'YIELD_FAITH', 10);

INSERT INTO Building_YieldFromFaithPurchase
		(BuildingType,		YieldType,			Yield)
VALUES	('BUILDING_ORDER',	'YIELD_PRODUCTION',	10);