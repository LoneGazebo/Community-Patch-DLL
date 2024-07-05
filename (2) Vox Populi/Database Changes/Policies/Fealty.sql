-- Tree structure
UPDATE Policies
SET GridX = 5, GridY = 2
WHERE Type = 'POLICY_ORGANIZED_RELIGION';

UPDATE Policies
SET GridX = 3, GridY = 1
WHERE Type = 'POLICY_MANDATE_OF_HEAVEN';

DELETE FROM Policy_PrereqPolicies
WHERE PolicyType IN (
	'POLICY_MANDATE_OF_HEAVEN',
	'POLICY_ORGANIZED_RELIGION',
	'POLICY_FREE_RELIGION',
	'POLICY_THEOCRACY'
);

INSERT INTO Policy_PrereqPolicies
	(PolicyType, PrereqPolicy)
VALUES
	('POLICY_ORGANIZED_RELIGION', 'POLICY_MANDATE_OF_HEAVEN'),
	('POLICY_FREE_RELIGION', 'POLICY_MANDATE_OF_HEAVEN'),
	('POLICY_THEOCRACY', 'POLICY_MANDATE_OF_HEAVEN');

-- Opener (now Fealty)
DELETE FROM Policy_BuildingClassProductionModifiers
WHERE PolicyType = 'POLICY_PIETY';

UPDATE Policies
SET FaithCostModifier = -25
WHERE Type = 'POLICY_PIETY';

-- Mandate of Heaven (now Nobility)
UPDATE Policies
SET FaithCostModifier = 0
WHERE Type = 'POLICY_MANDATE_OF_HEAVEN';

INSERT INTO Policy_BuildingClassHappiness
	(PolicyType, BuildingClassType, Happiness)
VALUES
	('POLICY_MANDATE_OF_HEAVEN', 'BUILDINGCLASS_CASTLE', 1);

INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
VALUES
	('POLICY_MANDATE_OF_HEAVEN', 'BUILDINGCLASS_CASTLE', 'YIELD_GOLD', 2),
	('POLICY_MANDATE_OF_HEAVEN', 'BUILDINGCLASS_ARMORY', 'YIELD_GOLD', 2);

INSERT INTO Policy_BuildingClassProductionModifiers
	(PolicyType, BuildingClassType, ProductionModifier)
VALUES
	('POLICY_MANDATE_OF_HEAVEN', 'BUILDINGCLASS_CASTLE', 100),
	('POLICY_MANDATE_OF_HEAVEN', 'BUILDINGCLASS_ARMORY', 100);

-- Theocracy (now Divine Right)
DELETE FROM Policy_ImprovementYieldChanges
WHERE PolicyType = 'POLICY_THEOCRACY';

DELETE FROM Policy_BuildingClassYieldModifiers
WHERE PolicyType = 'POLICY_THEOCRACY';

UPDATE Policies
SET
	InternalTradeRouteYieldModifier = 33,
	InternalTRTourism = 1,
	BoredomFlatReductionGlobal = 1
WHERE Type = 'POLICY_THEOCRACY';

-- Religious Tolerance (now Serfdom)
UPDATE Policies
SET
	SecondReligionPantheon = 0,
	PortraitIndex = 46
WHERE Type = 'POLICY_FREE_RELIGION';

INSERT INTO Policy_YieldFromNonSpecialistCitizens
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_FREE_RELIGION', 'YIELD_CULTURE', 25),
	('POLICY_FREE_RELIGION', 'YIELD_FAITH', 25);

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_FREE_RELIGION', 'IMPROVEMENT_PASTURE', 'YIELD_PRODUCTION', 2),
	('POLICY_FREE_RELIGION', 'IMPROVEMENT_PASTURE', 'YIELD_GOLD', 1);

-- Organized Religion
DELETE FROM Policy_BuildingClassYieldChanges
WHERE PolicyType = 'POLICY_ORGANIZED_RELIGION';

UPDATE Policies
SET
	PressureMod = 25,
	VotesPerFollowingCityTimes100 = 10
WHERE Type = 'POLICY_ORGANIZED_RELIGION';

INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
VALUES
	('POLICY_ORGANIZED_RELIGION', 'BUILDINGCLASS_SHRINE', 'YIELD_CULTURE', 2),
	('POLICY_ORGANIZED_RELIGION', 'BUILDINGCLASS_TEMPLE', 'YIELD_CULTURE', 2);

-- Reformation (now Fiefdoms)
UPDATE Policies
SET
	AddReformationBelief = 0,
	DoubleBorderGrowthWLTKD = 1,
	HappyPerMilitaryUnit = 10,
	PortraitIndex = 47
WHERE Type = 'POLICY_REFORMATION';

INSERT INTO Policy_WLTKDYieldMod
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_REFORMATION', 'YIELD_PRODUCTION', 15);

-- Finisher
DELETE FROM Policy_FreeUnitClasses
WHERE PolicyType = 'POLICY_PIETY_FINISHER';

DELETE FROM Policy_ImprovementCultureChanges
WHERE PolicyType = 'POLICY_PIETY_FINISHER';

INSERT INTO Policy_FaithPurchaseUnitClasses
	(PolicyType, UnitClassType)
VALUES
	('POLICY_PIETY_FINISHER', 'UNITCLASS_ARTIST');

UPDATE Policies
SET
	SharedReligionTourismModifier = 50,
	IncludesOneShotFreeUnits = 0
WHERE Type = 'POLICY_PIETY_FINISHER';

INSERT INTO Policy_ReligionYieldMod
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_PIETY_FINISHER', 'YIELD_PRODUCTION', 3),
	('POLICY_PIETY_FINISHER', 'YIELD_GOLD', 3),
	('POLICY_PIETY_FINISHER', 'YIELD_SCIENCE', 3),
	('POLICY_PIETY_FINISHER', 'YIELD_CULTURE', 3);

-- Scaler
INSERT INTO Policy_CityYieldChanges
	(PolicyType, YieldType, Yield)
SELECT Type, 'YIELD_FAITH', 1
FROM Policies
WHERE PolicyBranchType = 'POLICY_BRANCH_PIETY';

UPDATE Policies
SET DefenseBoostAllCities = 100
WHERE PolicyBranchType = 'POLICY_BRANCH_PIETY';
