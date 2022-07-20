-- Unlock Time
UPDATE PolicyBranchTypes
SET EraPrereq = 'ERA_MEDIEVAL'
WHERE Type = 'POLICY_BRANCH_PIETY';

-- Update Piety Shape

UPDATE Policies
SET GridX = '5', GridY = '2'
WHERE Type = 'POLICY_ORGANIZED_RELIGION';

UPDATE Policies
SET GridX = '3', GridY = '1'
WHERE Type = 'POLICY_MANDATE_OF_HEAVEN';


-- Great Artists come from Piety

INSERT INTO Policy_FaithPurchaseUnitClasses (PolicyType, UnitClassType)
VALUES ('POLICY_PIETY_FINISHER' , 'UNITCLASS_ARTIST');

-- Piety Opener
DELETE FROM Policy_BuildingClassProductionModifiers
WHERE PolicyType = 'POLICY_PIETY';

UPDATE Policies
SET FaithCostModifier = '-25'
WHERE Type = 'POLICY_PIETY';

-- Organized Religion

UPDATE Policies
SET FaithCostModifier = '0', PressureMod = '50'
WHERE Type = 'POLICY_ORGANIZED_RELIGION';

DELETE FROM Policy_BuildingClassYieldChanges
WHERE PolicyType = 'POLICY_ORGANIZED_RELIGION' AND BuildingClassType = 'BUILDINGCLASS_SHRINE';

INSERT INTO Policy_SpecialistExtraYields
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_ORGANIZED_RELIGION', 'YIELD_FAITH', 1);

DELETE FROM Policy_BuildingClassYieldChanges
WHERE PolicyType = 'POLICY_ORGANIZED_RELIGION';

-- Mandate of Heaven (Now Trade Fairs)
UPDATE Policies
SET FaithCostModifier = '0'
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

-- Theocracy (Now Divine Right)

DELETE FROM Policy_BuildingClassYieldModifiers
WHERE PolicyType = 'POLICY_THEOCRACY';
	
UPDATE Policies
SET HappinessToCulture = '25'
WHERE Type = 'POLICY_THEOCRACY';

-- Reformation (Now Monasticism)
UPDATE Policies
SET AddReformationBelief = '0', DoubleBorderGrowthGA = '1', DoubleBorderGrowthWLTKD = '1', NoUnhappfromXSpecialists = '1', PortraitIndex = '47'
WHERE Type = 'POLICY_REFORMATION';

INSERT INTO Policy_WLTKDYieldMod
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_REFORMATION', 'YIELD_PRODUCTION', 15);

INSERT INTO Policy_GoldenAgeYieldMod
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_REFORMATION', 'YIELD_PRODUCTION', 15);

UPDATE Buildings
SET PolicyType = 'POLICY_PIETY', UnlockedByBelief = '0', FaithCost = '200'
WHERE Type = 'BUILDING_MONASTERY';

DELETE FROM Building_ResourceYieldChanges
WHERE BuildingType = 'BUILDING_MONASTERY';

DELETE FROM Building_YieldChanges
WHERE BuildingType = 'BUILDING_MONASTERY' AND YieldType = 'YIELD_CULTURE';

UPDATE Building_YieldChanges
SET Yield = '2'
WHERE BuildingType = 'BUILDING_MONASTERY' AND YieldType = 'YIELD_FAITH';

UPDATE Buildings
SET Description = 'TXT_KEY_BUILDING_MONASTERY'
WHERE Type = 'BUILDING_MONASTERY';

-- Free Religion (Now Iconography)
UPDATE Policies
SET SecondReligionPantheon = '0', InternalTradeRouteYieldModifier = '33', IncludesOneShotFreeUnits = '0', PortraitIndex = '46'
WHERE Type = 'POLICY_FREE_RELIGION';

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_FREE_RELIGION', 'IMPROVEMENT_PASTURE', 'YIELD_PRODUCTION', 2),
	('POLICY_FREE_RELIGION', 'IMPROVEMENT_PASTURE', 'YIELD_GOLD', 1);

INSERT INTO Policy_YieldFromNonSpecialistCitizens
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_FREE_RELIGION', 'YIELD_CULTURE', 25);

-- Finisher

DELETE FROM Policy_FreeUnitClasses
WHERE PolicyType = 'POLICY_PIETY_FINISHER';

DELETE FROM Policy_ImprovementYieldChanges
WHERE PolicyType = 'POLICY_PIETY_FINISHER';

DELETE FROM Policy_ImprovementCultureChanges
WHERE PolicyType = 'POLICY_PIETY_FINISHER';

UPDATE Policies
SET SharedReligionTourismModifier = '25'
WHERE Type = 'POLICY_PIETY_FINISHER';


DELETE FROM Policy_ImprovementYieldChanges
WHERE PolicyType = 'POLICY_THEOCRACY';

--UPDATE Policies
--SET UnlocksPolicyBranchEra = 'ERA_RENAISSANCE'
--WHERE Type = 'POLICY_PIETY_FINISHER';

-- Finisher
--UPDATE Policies
--SET IdeologyPoint = '1'
--WHERE Type = 'POLICY_PIETY_FINISHER';

-- New Data
DELETE FROM Policy_PrereqPolicies
WHERE PolicyType = 'POLICY_MANDATE_OF_HEAVEN';

DELETE FROM Policy_PrereqPolicies
WHERE PolicyType = 'POLICY_ORGANIZED_RELIGION';

DELETE FROM Policy_PrereqPolicies
WHERE PolicyType = 'POLICY_FREE_RELIGION';

DELETE FROM Policy_PrereqPolicies
WHERE PolicyType = 'POLICY_THEOCRACY';

INSERT INTO Policy_PrereqPolicies
	(PolicyType, PrereqPolicy)
VALUES
	('POLICY_ORGANIZED_RELIGION', 'POLICY_MANDATE_OF_HEAVEN'),
	('POLICY_FREE_RELIGION', 'POLICY_MANDATE_OF_HEAVEN'),
	('POLICY_THEOCRACY', 'POLICY_MANDATE_OF_HEAVEN');

INSERT INTO Policy_CityYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_ORGANIZED_RELIGION', 'YIELD_FOOD', 1),
	('POLICY_MANDATE_OF_HEAVEN', 'YIELD_FOOD', 1),
	('POLICY_THEOCRACY', 'YIELD_FOOD', 1),
	('POLICY_FREE_RELIGION', 'YIELD_FOOD', 1),
	('POLICY_REFORMATION', 'YIELD_FOOD', 1);


UPDATE Policies
SET DefenseBoostAllCities = '100'
WHERE Type IN 
('POLICY_ORGANIZED_RELIGION',
'POLICY_MANDATE_OF_HEAVEN',
'POLICY_THEOCRACY',
'POLICY_FREE_RELIGION',
'POLICY_REFORMATION');


INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MONASTERY', 'YIELD_SCIENCE', 3),
	('BUILDING_MONASTERY', 'YIELD_FOOD', 3);

INSERT INTO Policy_ReligionYieldMod
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_PIETY_FINISHER', 'YIELD_FAITH', 3),
	('POLICY_PIETY_FINISHER', 'YIELD_SCIENCE', 3),
	('POLICY_PIETY_FINISHER', 'YIELD_PRODUCTION', 3),
	('POLICY_PIETY_FINISHER', 'YIELD_CULTURE', 3);
