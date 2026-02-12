-- Tree structure
UPDATE Policies
SET GridX = 2, GridY = 1,
PortraitIndex = 53
WHERE Type = 'POLICY_SECULARISM';

UPDATE Policies
SET GridX = 3, GridY = 2
WHERE Type = 'POLICY_SOVEREIGNTY';

UPDATE Policies
SET GridX = 5, GridY = 1,
PortraitIndex = 49
WHERE Type = 'POLICY_HUMANISM';

UPDATE Policies
SET GridX = 1, GridY = 2,
PortraitIndex = 52
WHERE Type = 'POLICY_FREE_THOUGHT';

UPDATE Policies
SET GridX = 4, GridY = 3,
PortraitIndex = 51
WHERE Type = 'POLICY_SCIENTIFIC_REVOLUTION';

DELETE FROM Policy_PrereqPolicies WHERE PolicyType = 'POLICY_FREE_THOUGHT';

INSERT INTO Policy_PrereqPolicies
	(PolicyType, PrereqPolicy)
VALUES
	('POLICY_FREE_THOUGHT', 'POLICY_SECULARISM'),
	('POLICY_SCIENTIFIC_REVOLUTION', 'POLICY_HUMANISM');

-- Opener
UPDATE Policies 
SET 
	GreatScientistRateModifier = 33,
	CityGrowthMod = 10
WHERE Type = 'POLICY_RATIONALISM';

INSERT INTO Policy_SpecialistYieldChanges
	(PolicyType, SpecialistType, YieldType, Yield)
VALUES
	('POLICY_RATIONALISM', 'SPECIALIST_SCIENTIST', 'YIELD_SCIENCE', 1),	
	('POLICY_RATIONALISM', 'SPECIALIST_SCIENTIST', 'YIELD_CULTURE', 1);

-- Secularism (now Liberalism)
DELETE FROM Policy_SpecialistExtraYields
WHERE PolicyType = 'POLICY_SECULARISM';

INSERT INTO Policy_SpecialistExtraYields
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_SECULARISM', 'YIELD_PRODUCTION', 1),
	('POLICY_SECULARISM', 'YIELD_GOLD', 2);

INSERT INTO Policy_GoldenAgeGreatPersonRateModifier
	(PolicyType, GreatPersonType, Modifier)
SELECT
	'POLICY_SECULARISM', Type, 25
FROM GreatPersons
WHERE Specialist IS NOT NULL;

-- Humanism (now Mass Education)
UPDATE Policies
SET
	GreatScientistRateModifier = 0,
	NumFreeTechs = 1,
	OneShot = 1
WHERE Type = 'POLICY_HUMANISM';

INSERT INTO Policy_BuildingClassHappiness
	(PolicyType, BuildingClassType, Happiness)
VALUES
	('POLICY_HUMANISM', 'BUILDINGCLASS_PUBLIC_SCHOOL', 1);

INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
VALUES
	('POLICY_HUMANISM', 'BUILDINGCLASS_PUBLIC_SCHOOL', 'YIELD_CULTURE', 2),
	('POLICY_HUMANISM', 'BUILDINGCLASS_PUBLIC_SCHOOL', 'YIELD_FOOD', 3);

INSERT INTO Policy_BuildingClassProductionModifiers
	(PolicyType, BuildingClassType, ProductionModifier)
VALUES
	('POLICY_HUMANISM', 'BUILDINGCLASS_PUBLIC_SCHOOL', 50);

-- Sovereignty (now Emancipation)
DELETE FROM Policy_BuildingClassYieldChanges
WHERE PolicyType = 'POLICY_SOVEREIGNTY';

INSERT INTO Policy_BuildingClassHappiness
	(PolicyType, BuildingClassType, Happiness)
VALUES
	('POLICY_SOVEREIGNTY', 'BUILDINGCLASS_THEATRE', 1),
	('POLICY_SOVEREIGNTY', 'BUILDINGCLASS_HOTEL', 1),
    ('POLICY_SOVEREIGNTY', 'BUILDINGCLASS_MUSEUM', 1);

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_SOVEREIGNTY', 'IMPROVEMENT_TRADING_POST', 'YIELD_PRODUCTION', 2),
	('POLICY_SOVEREIGNTY', 'IMPROVEMENT_TRADING_POST', 'YIELD_GOLD', 2);

INSERT INTO Policy_YieldFromBirthRetroactive
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_SOVEREIGNTY', 'YIELD_CULTURE', 5),
	('POLICY_SOVEREIGNTY', 'YIELD_GOLDEN_AGE_POINTS', 5);

INSERT INTO Policy_GoldenAgeYieldMod
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_SOVEREIGNTY', 'YIELD_FOOD', 10);

-- Free Thought (now Secularism, unhelpful I know)
DELETE FROM Policy_ImprovementYieldChanges
WHERE PolicyType = 'POLICY_FREE_THOUGHT';

DELETE FROM Policy_BuildingClassYieldModifiers
WHERE PolicyType = 'POLICY_FREE_THOUGHT';

INSERT INTO Policy_GoldenAgeYieldMod
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_FREE_THOUGHT', 'YIELD_SCIENCE', 10);

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_FREE_THOUGHT', 'IMPROVEMENT_ACADEMY', 'YIELD_PRODUCTION', 3),
	('POLICY_FREE_THOUGHT', 'IMPROVEMENT_ACADEMY', 'YIELD_GOLD', 3);

UPDATE Policies
SET
	ReligiousUnrestFlatReduction = 5,
	GreatScientistRateModifier = 0,
	GreatScientistBeakerModifier = 0
WHERE Type = 'POLICY_FREE_THOUGHT';

-- Scientific Revolution (now Employment Law)
UPDATE Policies
SET
	OneShot = 0,
	MedianTechPercentChange = 0
	HappinessToCulture = 50,
	SpecialistFoodChange = -1
WHERE Type = 'POLICY_SCIENTIFIC_REVOLUTION';

INSERT INTO Policy_YieldFromNonSpecialistCitizens
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_SCIENTIFIC_REVOLUTION', 'YIELD_PRODUCTION', 100);

INSERT INTO Policy_SpecialistExtraYields
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_SCIENTIFIC_REVOLUTION', 'YIELD_PRODUCTION', 1);

-- Finisher
UPDATE Policies
SET
	OneShot = 0,
	NumFreeTechs = 0,
	GreatScientistBeakerModifier = 25,
	SpySecurityModifier = 12
WHERE Type = 'POLICY_RATIONALISM_FINISHER';

-- Scaler
UPDATE Policies SET CityGrowthMod = 5 WHERE PolicyBranchType = 'POLICY_BRANCH_RATIONALISM';
