-- Opener
UPDATE Policies
SET HappinessToScience = 0
WHERE Type = 'POLICY_RATIONALISM';

INSERT INTO Policy_ResourceYieldChanges
	(PolicyType, ResourceType, YieldType, Yield)
SELECT
	'POLICY_RATIONALISM', Type, 'YIELD_PRODUCTION', 2
FROM Resources
WHERE ResourceUsage = 1;

INSERT INTO Policy_ResourceYieldChanges
	(PolicyType, ResourceType, YieldType, Yield)
SELECT
	'POLICY_RATIONALISM', Type, 'YIELD_SCIENCE', 3
FROM Resources
WHERE ResourceUsage = 1;

INSERT INTO Policy_YieldModifiers
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_RATIONALISM', 'YIELD_SCIENCE', 5);

-- Secularism (now Scientific Revolution)
DELETE FROM Policy_SpecialistExtraYields
WHERE PolicyType = 'POLICY_SECULARISM';

INSERT INTO Policy_FeatureYieldChanges
	(PolicyType, FeatureType, YieldType, Yield)
VALUES
	('POLICY_SECULARISM', 'FEATURE_JUNGLE', 'YIELD_SCIENCE', 2);

INSERT INTO Policy_TerrainYieldChanges
	(PolicyType, TerrainType, YieldType, Yield)
VALUES
	('POLICY_SECULARISM', 'TERRAIN_SNOW', 'YIELD_SCIENCE', 2);

-- Humanism (now Enlightenment)
UPDATE Policies
SET
	GreatScientistRateModifier = 0,
	NumFreeTechs = 1,
	OneShot = 1
WHERE Type = 'POLICY_HUMANISM';

INSERT INTO Policy_BuildingClassHappiness
	(PolicyType, BuildingClassType, Happiness)
VALUES
	('POLICY_HUMANISM', 'BUILDINGCLASS_UNIVERSITY', 1);

-- Sovereignty (now Rights of Man)
DELETE FROM Policy_BuildingClassYieldChanges
WHERE PolicyType = 'POLICY_SOVEREIGNTY';

UPDATE Policies
SET
	DistressFlatReductionGlobal = 1,
	PovertyFlatReductionGlobal = 1,
	IlliteracyFlatReductionGlobal = 1,
	BoredomFlatReductionGlobal = 1,
	ReligiousUnrestFlatReductionGlobal = 1
WHERE Type = 'POLICY_SOVEREIGNTY';

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_SOVEREIGNTY', 'IMPROVEMENT_TRADING_POST', 'YIELD_PRODUCTION', 2),
	('POLICY_SOVEREIGNTY', 'IMPROVEMENT_TRADING_POST', 'YIELD_GOLD', 2);

INSERT INTO Policy_GoldenAgeYieldMod
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_SOVEREIGNTY', 'YIELD_SCIENCE', 10);

-- Free Thought
DELETE FROM Policy_ImprovementYieldChanges
WHERE PolicyType = 'POLICY_FREE_THOUGHT';

DELETE FROM Policy_BuildingClassYieldModifiers
WHERE PolicyType = 'POLICY_FREE_THOUGHT';

UPDATE Policies
SET
	ReligiousUnrestFlatReductionGlobal = 5,
	GreatScientistRateModifier = 33,
	GreatScientistBeakerModifier = 25
WHERE Type = 'POLICY_FREE_THOUGHT';

-- Scientific Revolution (now Empiricism)
UPDATE Policies
SET
	OneShot = 0,
	MedianTechPercentChange = 0,
	CityGrowthMod = 25
WHERE Type = 'POLICY_SCIENTIFIC_REVOLUTION';

INSERT INTO Policy_YieldFromNonSpecialistCitizens
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_SCIENTIFIC_REVOLUTION', 'YIELD_FOOD', 100);

INSERT INTO Policy_SpecialistExtraYields
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_SCIENTIFIC_REVOLUTION', 'YIELD_FOOD', 1);

INSERT INTO Policy_YieldModifierFromGreatWorks
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_SCIENTIFIC_REVOLUTION', 'YIELD_SCIENCE', 3);

-- Finisher
UPDATE Policies
SET
	OneShot = 0,
	NumFreeTechs = 0,
	DistressFlatReductionGlobal = 1,
	PovertyFlatReductionGlobal = 1,
	IlliteracyFlatReductionGlobal = 1,
	BoredomFlatReductionGlobal = 1,
	ReligiousUnrestFlatReductionGlobal = 1,
	SpySecurityModifier = 12
WHERE Type = 'POLICY_RATIONALISM_FINISHER';

-- Scaler
INSERT INTO Policy_YieldModifiers
	(PolicyType, YieldType, Yield)
SELECT
	Type, 'YIELD_SCIENCE', 2
FROM Policies
WHERE PolicyBranchType = 'POLICY_BRANCH_RATIONALISM';
