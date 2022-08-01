-- Great Writers come from Liberty

INSERT INTO Policy_FaithPurchaseUnitClasses (PolicyType, UnitClassType)
VALUES ('POLICY_LIBERTY_FINISHER' , 'UNITCLASS_WRITER');

-- Change Liberty Arrangement

UPDATE Policies
SET GridX = '1', GridY = '2', PolicyCostModifier = '0'
WHERE Type = 'POLICY_REPUBLIC';

UPDATE Policies
SET GridX = '1', GridY = '1'
WHERE Type = 'POLICY_COLLECTIVE_RULE';

INSERT INTO Policy_PrereqPolicies (PolicyType, PrereqPolicy)
VALUES ('POLICY_REPUBLIC' , 'POLICY_COLLECTIVE_RULE');

DELETE FROM Policy_PrereqPolicies
WHERE PolicyType = 'POLICY_COLLECTIVE_RULE';

-- Liberty Opener
UPDATE Policies
SET CulturePerCity = '0'
WHERE Type = 'POLICY_LIBERTY';

-- Collective Rule (Now City Planning)
UPDATE Policies
SET CapitalSettlerProductionModifier = '0', ExtraMoves = '1', PortraitIndex = '29', IconAtlas = 'POLICY_ATLAS', IconAtlasAchieved = 'POLICY_A_ATLAS'
WHERE Type = 'POLICY_COLLECTIVE_RULE';

DELETE FROM Policy_FreeUnitClasses
WHERE PolicyType = 'POLICY_COLLECTIVE_RULE';

INSERT INTO Policy_UnitClassProductionModifiers
	(PolicyType, UnitClassType, ProductionModifier)
VALUES
	('POLICY_COLLECTIVE_RULE', 'UNITCLASS_WORKER', 25),
	('POLICY_COLLECTIVE_RULE', 'UNITCLASS_CARGO_SHIP', 25),
	('POLICY_COLLECTIVE_RULE', 'UNITCLASS_CARAVAN', 25);

-- Citizenship (Now Free Labor)
UPDATE Policies
SET WorkerSpeedModifier = '25'
WHERE Type = 'POLICY_CITIZENSHIP';

-- Republic (Now Civil Engineers)
UPDATE Policies
SET BuildingProductionModifier = '15'
WHERE Type = 'POLICY_REPUBLIC';

DELETE FROM Policy_CityYieldChanges
WHERE PolicyType = 'POLICY_REPUBLIC';

-- Representation 
UPDATE Policies
SET GoldenAgeTurns = '0', NumCitiesPolicyCostDiscount = '0', HappinessPerXPopulationGlobal = '15', ExtraHappinessPerCity = '1'
WHERE Type = 'POLICY_REPRESENTATION';

-- Meritocracy
UPDATE Policies
SET HappinessPerTradeRoute = '0', UnhappinessMod = '0', PortraitIndex = '1', IconAtlas = 'POLICY_ATLAS_EXP2', IconAtlasAchieved = 'POLICY_A_ATLAS_EXP2'
WHERE Type = 'POLICY_MERITOCRACY';

-- Finisher
UPDATE Policies
SET NumFreeGreatPeople = '0', IncludesOneShotFreeUnits = '0'
WHERE Type = 'POLICY_LIBERTY_FINISHER';

--UPDATE Policies
--SET UnlocksPolicyBranchEra = 'ERA_MEDIEVAL'
--WHERE Type = 'POLICY_LIBERTY_FINISHER';

-- Finisher
--UPDATE Policies
--SET IdeologyPoint = '1'
--WHERE Type = 'POLICY_LIBERTY_FINISHER';

-- NEW

INSERT INTO Policy_YieldFromTech
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_LIBERTY', 'YIELD_CULTURE', 15),
	('POLICY_COLLECTIVE_RULE', 'YIELD_CULTURE', 15),
	('POLICY_MERITOCRACY', 'YIELD_CULTURE', 15),
	('POLICY_CITIZENSHIP', 'YIELD_CULTURE', 15),
	('POLICY_REPRESENTATION', 'YIELD_CULTURE', 15),
	('POLICY_REPUBLIC', 'YIELD_CULTURE', 15);

INSERT INTO Policy_YieldFromConstruction
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_REPUBLIC', 'YIELD_CULTURE', 10);

INSERT INTO Policy_YieldChangeTradeRoute
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_MERITOCRACY', 'YIELD_SCIENCE', 3);

INSERT INTO Policy_CityYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_MERITOCRACY', 'YIELD_FOOD', 3),
	('POLICY_COLLECTIVE_RULE', 'YIELD_PRODUCTION', 3),
	('POLICY_CITIZENSHIP', 'YIELD_GOLD', 3);

INSERT INTO Policy_YieldFromBirth
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_LIBERTY_FINISHER', 'YIELD_GOLD', 25);

INSERT INTO Policy_YieldFromBirthCapitalRetroactive
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_LIBERTY', 'YIELD_SCIENCE', 15);

INSERT INTO Policy_YieldFromTechRetroactive
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_LIBERTY', 'YIELD_CULTURE', 15);

INSERT INTO Policy_YieldFromBirthCapital
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_LIBERTY', 'YIELD_SCIENCE', 20);