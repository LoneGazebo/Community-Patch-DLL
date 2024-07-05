-- Tree structure
UPDATE Policies
SET GridX = 1, GridY = 1
WHERE Type = 'POLICY_LEGALISM';

UPDATE Policies
SET GridX = 3, GridY = 1
WHERE Type = 'POLICY_ARISTOCRACY';

UPDATE Policies
SET GridX = 5, GridY = 1
WHERE Type = 'POLICY_OLIGARCHY';

UPDATE Policies
SET GridX = 2, GridY = 2
WHERE Type = 'POLICY_LANDED_ELITE';

UPDATE Policies
SET GridX = 4, GridY = 2
WHERE Type = 'POLICY_MONARCHY';

DELETE FROM Policy_PrereqPolicies
WHERE PolicyType IN (
	'POLICY_MONARCHY',
	'POLICY_LEGALISM'
);

INSERT INTO Policy_PrereqPolicies
	(PolicyType, PrereqPolicy)
VALUES
	('POLICY_MONARCHY', 'POLICY_ARISTOCRACY'),
	('POLICY_MONARCHY', 'POLICY_OLIGARCHY'),
	('POLICY_LANDED_ELITE', 'POLICY_ARISTOCRACY');

-- Opener
DELETE FROM Policy_BuildingClassCultureChanges
WHERE PolicyType = 'POLICY_TRADITION';

UPDATE Policies
SET
	PlotCultureExponentModifier = 0,
	FreePopulationCapital = 2,
	OneShot = 1
WHERE Type = 'POLICY_TRADITION';

INSERT INTO Policy_CapitalYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_TRADITION', 'YIELD_FOOD', 3);

INSERT INTO Policy_CapitalYieldPerPopChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_TRADITION', 'YIELD_CULTURE', 50);

INSERT INTO Policy_BuildingClassHappiness
	(PolicyType, BuildingClassType, Happiness)
VALUES
	('POLICY_TRADITION', 'BUILDINGCLASS_PALACE', 2);

-- Aristocracy (now Justice)
UPDATE Policies
SET
	HappinessPerXPopulation = 0,
	WonderProductionModifier = 0,
	GarrisonedCityRangeStrikeModifier = 25,
	PortraitIndex = 57
WHERE Type = 'POLICY_ARISTOCRACY';

INSERT INTO Policy_CityYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_ARISTOCRACY', 'YIELD_PRODUCTION', 1);

INSERT INTO Policy_FreeBuilding
	(PolicyType, BuildingClassType, Count)
VALUES
	('POLICY_ARISTOCRACY', 'BUILDINGCLASS_CAPITAL_ENGINEER', 1);

-- Oligarchy (now Sovereignty)
UPDATE Policies
SET
	GarrisonedCityRangeStrikeModifier = 0,
	GarrisonFreeMaintenance = 0,
	PlotCultureExponentModifier = -20,
	PortraitIndex = 59
WHERE Type = 'POLICY_OLIGARCHY';

INSERT INTO Policy_FreeBuilding
	(PolicyType, BuildingClassType, Count)
VALUES
	('POLICY_OLIGARCHY', 'BUILDINGCLASS_PALACE_COURT_CHAPEL', 1);

-- Legalism (now Ceremony)
UPDATE Policies
SET
	NumCitiesFreeCultureBuilding = 0,
	PortraitIndex = 55
WHERE Type = 'POLICY_LEGALISM';

INSERT INTO Policy_FreeBuilding
	(PolicyType, BuildingClassType, Count)
VALUES
	('POLICY_LEGALISM', 'BUILDINGCLASS_PALACE_ASTROLOGER', 1);

-- +1 happiness to National Wonders with building requirement
INSERT INTO Policy_BuildingClassHappiness
	(PolicyType, BuildingClassType, Happiness)
SELECT
	'POLICY_LEGALISM', Type, 1
FROM BuildingClasses
WHERE MaxPlayerInstances = 1 AND (
	EXISTS (
		SELECT 1 FROM Building_ClassesNeededInCity
		WHERE BuildingType = DefaultBuilding
	) OR EXISTS (
		SELECT 1 FROM Building_ClassNeededAnywhere
		WHERE BuildingType = DefaultBuilding
	)
);

-- +25% production to build National Wonders with building requirement
INSERT INTO Policy_BuildingClassProductionModifiers
	(PolicyType, BuildingClassType, ProductionModifier)
SELECT
	'POLICY_LEGALISM', Type, 25
FROM BuildingClasses
WHERE MaxPlayerInstances = 1 AND (
	EXISTS (
		SELECT 1 FROM Building_ClassesNeededInCity
		WHERE BuildingType = DefaultBuilding
	) OR EXISTS (
		SELECT 1 FROM Building_ClassNeededAnywhere
		WHERE BuildingType = DefaultBuilding
	)
);

-- Monarchy (now Majesty)
DELETE FROM Policy_CapitalYieldPerPopChanges
WHERE PolicyType = 'POLICY_MONARCHY';

UPDATE Policies
SET
	CapitalUnhappinessMod = 0,
	HalfSpecialistFoodCapital = 1,
	PortraitIndex = 58
WHERE Type = 'POLICY_MONARCHY';

INSERT INTO Policy_FreeBuilding
	(PolicyType, BuildingClassType, Count)
VALUES
	('POLICY_MONARCHY', 'BUILDINGCLASS_PALACE_GARDEN', 1);

-- Landed Elite (now Splendor)
DELETE FROM Policy_CapitalYieldChanges
WHERE PolicyType = 'POLICY_LANDED_ELITE';

UPDATE Policies
SET
	CapitalGrowthMod = 0,
	PortraitIndex = 56
WHERE Type = 'POLICY_LANDED_ELITE';

INSERT INTO Policy_YieldGPExpend
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_LANDED_ELITE', 'YIELD_CULTURE', 50);

INSERT INTO Policy_FreeBuilding
	(PolicyType, BuildingClassType, Count)
VALUES
	('POLICY_LANDED_ELITE', 'BUILDINGCLASS_PALACE_TREASURY', 1);

-- Finisher
UPDATE Policies
SET
	NumCitiesFreeFoodBuilding = 0,
	CityGrowthMod = 0
WHERE Type = 'POLICY_TRADITION_FINISHER';

CREATE TEMP TABLE Helper (
	YieldType TEXT
);

INSERT INTO Helper
	(YieldType)
VALUES
	('YIELD_FOOD'),
	('YIELD_PRODUCTION');

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
SELECT
	'POLICY_TRADITION_FINISHER', a.Type, b.YieldType, 1
FROM Improvements a, Helper b
WHERE a.CreatedByGreatPerson = 1 OR a.Type = 'IMPROVEMENT_LANDMARK';

DROP TABLE Helper;

INSERT INTO Policy_FreeBuilding
	(PolicyType, BuildingClassType, Count)
VALUES
	('POLICY_TRADITION_FINISHER', 'BUILDINGCLASS_PALACE_THRONE_ROOM', 1);

-- Scaler
INSERT INTO Policy_CapitalYieldChanges
	(PolicyType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 1
FROM Policies
WHERE PolicyBranchType = 'POLICY_BRANCH_TRADITION';

INSERT INTO Policy_CityYieldChanges
	(PolicyType, YieldType, Yield)
SELECT
	Type, 'YIELD_FOOD', 1
FROM Policies
WHERE PolicyBranchType = 'POLICY_BRANCH_TRADITION';
