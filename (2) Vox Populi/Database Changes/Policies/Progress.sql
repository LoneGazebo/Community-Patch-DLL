-- Tree structure
UPDATE Policies
SET GridX = 1, GridY = 2, PolicyCostModifier = 0
WHERE Type = 'POLICY_REPUBLIC';

UPDATE Policies
SET GridX = 1, GridY = 1
WHERE Type = 'POLICY_COLLECTIVE_RULE';

DELETE FROM Policy_PrereqPolicies
WHERE PolicyType = 'POLICY_COLLECTIVE_RULE';

INSERT INTO Policy_PrereqPolicies
	(PolicyType, PrereqPolicy)
VALUES
	('POLICY_REPUBLIC', 'POLICY_COLLECTIVE_RULE');

-- Opener (now Progress)
UPDATE Policies
SET
	CulturePerCity = 0,
	OneShot = 1
WHERE Type = 'POLICY_LIBERTY';

INSERT INTO Policy_YieldFromBirthCapital
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_LIBERTY', 'YIELD_SCIENCE', 20);

INSERT INTO Policy_YieldFromBirthCapitalRetroactive
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_LIBERTY', 'YIELD_SCIENCE', 15);

INSERT INTO Policy_YieldFromTech
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_LIBERTY', 'YIELD_CULTURE', 15);

INSERT INTO Policy_YieldFromTechRetroactive
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_LIBERTY', 'YIELD_CULTURE', 15);

-- Collective Rule (now Organization)
DELETE FROM Policy_FreeUnitClasses
WHERE PolicyType = 'POLICY_COLLECTIVE_RULE';

UPDATE Policies
SET
	CapitalSettlerProductionModifier = 0,
	IncludesOneShotFreeUnits = 0,
	PortraitIndex = 29,
	IconAtlas = 'POLICY_ATLAS',
	IconAtlasAchieved = 'POLICY_A_ATLAS'
WHERE Type = 'POLICY_COLLECTIVE_RULE';

INSERT INTO Policy_CityYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_COLLECTIVE_RULE', 'YIELD_PRODUCTION', 2);

INSERT INTO Policy_UnitClassProductionModifiers
	(PolicyType, UnitClassType, ProductionModifier)
VALUES
	('POLICY_COLLECTIVE_RULE', 'UNITCLASS_WORKER', 25),
	('POLICY_COLLECTIVE_RULE', 'UNITCLASS_CARGO_SHIP', 25),
	('POLICY_COLLECTIVE_RULE', 'UNITCLASS_CARAVAN', 25);

INSERT INTO Policy_FreePromotions
	(PolicyType, PromotionType)
VALUES
	('POLICY_COLLECTIVE_RULE', 'PROMOTION_FASTER_WORKER');

-- Citizenship (now Liberty)
UPDATE Policies
SET WorkerSpeedModifier = 25
WHERE Type = 'POLICY_CITIZENSHIP';

INSERT INTO Policy_CityYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_CITIZENSHIP', 'YIELD_GOLD', 2);

-- Republic (now Expertise)
DELETE FROM Policy_CityYieldChanges
WHERE PolicyType = 'POLICY_REPUBLIC';

UPDATE Policies
SET BuildingProductionModifier = 15
WHERE Type = 'POLICY_REPUBLIC';

INSERT INTO Policy_YieldFromConstruction
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_REPUBLIC', 'YIELD_CULTURE', 10);

-- Representation (now Equality)
UPDATE Policies
SET
	GoldenAgeTurns = 0,
	NumCitiesPolicyCostDiscount = 0,
	HappinessPerXPopulationGlobal = 15,
	ExtraHappinessPerCity = 1,
	DistressFlatReductionGlobal = 1,
	PovertyFlatReductionGlobal = 1,
	IlliteracyFlatReductionGlobal = 1,
	BoredomFlatReductionGlobal = 1
WHERE Type = 'POLICY_REPRESENTATION';

-- Meritocracy (now Fraternity)
UPDATE Policies
SET
	HappinessPerTradeRoute = 0,
	UnhappinessMod = 0,
	PortraitIndex = 1,
	IconAtlas = 'POLICY_ATLAS_EXP2',
	IconAtlasAchieved = 'POLICY_A_ATLAS_EXP2'
WHERE Type = 'POLICY_MERITOCRACY';

INSERT INTO Policy_CityYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_MERITOCRACY', 'YIELD_FOOD', 3);

INSERT INTO Policy_YieldChangeTradeRoute
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_MERITOCRACY', 'YIELD_SCIENCE', 3);

-- Finisher
INSERT INTO Policy_FaithPurchaseUnitClasses
	(PolicyType, UnitClassType)
VALUES
	('POLICY_LIBERTY_FINISHER', 'UNITCLASS_WRITER');

UPDATE Policies
SET
	NumFreeGreatPeople = 0,
	IncludesOneShotFreeUnits = 0
WHERE Type = 'POLICY_LIBERTY_FINISHER';

INSERT INTO Policy_YieldFromBirth
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_LIBERTY_FINISHER', 'YIELD_GOLD', 25);

-- Scaler
INSERT INTO Policy_YieldFromTech
	(PolicyType, YieldType, Yield)
SELECT
	Type, 'YIELD_CULTURE', 15
FROM Policies
WHERE PolicyBranchType = 'POLICY_BRANCH_LIBERTY';
