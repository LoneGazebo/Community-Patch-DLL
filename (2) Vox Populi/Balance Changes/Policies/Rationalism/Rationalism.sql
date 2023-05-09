-- Unlock Time

UPDATE PolicyBranchTypes
SET EraPrereq = 'ERA_INDUSTRIAL'
WHERE Type = 'POLICY_BRANCH_RATIONALISM';


-- Opener

UPDATE Policies
SET
	HappinessToScience = 0
WHERE Type = 'POLICY_RATIONALISM';

INSERT INTO Policy_ResourceYieldChanges
	(PolicyType, ResourceType, YieldType, Yield)
VALUES
	('POLICY_RATIONALISM', 'RESOURCE_IRON', 'YIELD_SCIENCE', 3),
	('POLICY_RATIONALISM', 'RESOURCE_IRON', 'YIELD_PRODUCTION', 2),
	('POLICY_RATIONALISM', 'RESOURCE_HORSE', 'YIELD_SCIENCE', 3),
	('POLICY_RATIONALISM', 'RESOURCE_HORSE', 'YIELD_PRODUCTION', 2),
	('POLICY_RATIONALISM', 'RESOURCE_COAL', 'YIELD_SCIENCE', 3),
	('POLICY_RATIONALISM', 'RESOURCE_COAL', 'YIELD_PRODUCTION', 2),
	('POLICY_RATIONALISM', 'RESOURCE_OIL', 'YIELD_SCIENCE', 3),
	('POLICY_RATIONALISM', 'RESOURCE_OIL', 'YIELD_PRODUCTION', 2),
	('POLICY_RATIONALISM', 'RESOURCE_ALUMINUM', 'YIELD_SCIENCE', 3),
	('POLICY_RATIONALISM', 'RESOURCE_ALUMINUM', 'YIELD_PRODUCTION', 2),
	('POLICY_RATIONALISM', 'RESOURCE_URANIUM', 'YIELD_SCIENCE', 3),
	('POLICY_RATIONALISM', 'RESOURCE_URANIUM', 'YIELD_PRODUCTION', 2);


-- Humanism (now Enlightenment)

UPDATE Policies
SET
	GreatScientistRateModifier = 0,
	GoldenAgeTurns = 0,
	NumFreeTechs = 1
WHERE Type = 'POLICY_HUMANISM';

INSERT INTO Policy_BuildingClassHappiness
	(PolicyType, BuildingClassType, Happiness)
VALUES
	('POLICY_HUMANISM', 'BUILDINGCLASS_UNIVERSITY', 1);

INSERT INTO Policy_GoldenAgeYieldMod
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_HUMANISM', 'YIELD_SCIENCE', 10);


-- Scientific Revolution (now Rights of Man)

UPDATE Policies
SET
	OneShot = 0,
	MedianTechPercentChange = 0
WHERE Type = 'POLICY_SCIENTIFIC_REVOLUTION';

INSERT INTO Policy_YieldFromNonSpecialistCitizens
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_SCIENTIFIC_REVOLUTION', 'YIELD_FOOD', 50);

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_SCIENTIFIC_REVOLUTION', 'IMPROVEMENT_TRADING_POST', 'YIELD_PRODUCTION', 2),
	('POLICY_SCIENTIFIC_REVOLUTION', 'IMPROVEMENT_TRADING_POST', 'YIELD_GOLD', 2);


-- Secularism (now Scientific Revolution)

DELETE FROM Policy_SpecialistExtraYields
WHERE PolicyType = 'POLICY_SECULARISM';

UPDATE Buildings
SET
	PolicyType = 'POLICY_SECULARISM'
WHERE Type = 'BUILDING_OBSERVATORY';

INSERT INTO Policy_YieldModifierFromGreatWorks
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_SECULARISM', 'YIELD_SCIENCE', 3);

INSERT INTO Policy_FeatureYieldChanges
	(PolicyType, FeatureType, YieldType, Yield)
VALUES
	('POLICY_SECULARISM', 'FEATURE_JUNGLE', 'YIELD_SCIENCE', 2);

INSERT INTO Policy_TerrainYieldChanges
	(PolicyType, TerrainType, YieldType, Yield)
VALUES
	('POLICY_SECULARISM', 'TERRAIN_SNOW', 'YIELD_SCIENCE', 2);


-- Sovereignty (now Empiricism)

DELETE FROM Policy_BuildingClassYieldChanges
WHERE PolicyType = 'POLICY_SOVEREIGNTY';

UPDATE Policies
SET
	HappinessPerTradeRoute = 0,
	ExtraHappiness = 0,
	TechCostXCitiesMod = 0,
	EspionageModifier = -34,
	SpecialistFoodChange = -1
WHERE Type = 'POLICY_SOVEREIGNTY';


-- Free Thought

DELETE FROM Policy_BuildingClassYieldModifiers
WHERE PolicyType = 'POLICY_FREE_THOUGHT';

DELETE FROM Policy_ImprovementYieldChanges
WHERE PolicyType = 'POLICY_FREE_THOUGHT' AND YieldType = 'YIELD_SCIENCE';

UPDATE Policies
SET
	GreatEngineerHurryModifier = 0,
	GreatScientistBeakerModifier = 25
WHERE Type = 'POLICY_FREE_THOUGHT';


-- Finisher

UPDATE Policies
SET
	OneShot = 0,
	NumFreeTechs = 0,
	GreatScientistRateModifier = 33,
	CityGrowthMod = 25
WHERE Type = 'POLICY_RATIONALISM_FINISHER';



----------------------
-- Combined Insertions
----------------------

-- Scaler

INSERT INTO Policy_YieldModifiers
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_RATIONALISM', 'YIELD_SCIENCE', 5),
	('POLICY_HUMANISM', 'YIELD_SCIENCE', 2),
	('POLICY_SCIENTIFIC_REVOLUTION', 'YIELD_SCIENCE', 2),
	('POLICY_FREE_THOUGHT', 'YIELD_SCIENCE', 2),
	('POLICY_SOVEREIGNTY', 'YIELD_SCIENCE', 2),
	('POLICY_SECULARISM', 'YIELD_SCIENCE', 2);
