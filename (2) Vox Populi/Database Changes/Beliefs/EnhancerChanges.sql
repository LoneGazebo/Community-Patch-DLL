-- Just War (now Abode of Peace)
UPDATE Beliefs
SET
	CombatModifierEnemyCities = 0,
	CityStateMinimumInfluence = 35,
	CityStateInfluenceModifier = 25,
	FriendlyCityStateSpreadModifier = 100
WHERE Type = 'BELIEF_JUST_WAR';

INSERT INTO Belief_YieldChangePerXCityStateFollowers
	(BeliefType, YieldType, PerXFollowers)
VALUES
	('BELIEF_JUST_WAR', 'YIELD_GOLD', 2),
	('BELIEF_JUST_WAR', 'YIELD_FAITH', 2);

-- Religious Texts (now Inquisition)
UPDATE Beliefs
SET
	SpreadStrengthModifier = 0,
	InquisitorCostModifier = -33,
	HappinessFromForeignSpies = 2,
	SpyPressure = 8 -- number of full city pressure applied to target city
WHERE Type = 'BELIEF_RELIGIOUS_TEXTS';

INSERT INTO Belief_YieldFromRemoveHeresy
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_RELIGIOUS_TEXTS', 'YIELD_GOLD', 25);

-- Reliquary (now Mendicancy)
UPDATE Beliefs
SET
	GreatPersonExpendedFaith = 0,
	OtherReligionPressureErosion = 10
WHERE Type = 'BELIEF_RELIQUARY';

INSERT INTO Belief_CityYieldChanges
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_RELIQUARY', 'YIELD_CULTURE', 2),
	('BELIEF_RELIQUARY', 'YIELD_FAITH', 2);

-- Holy Order (now Orthodoxy)
UPDATE Beliefs
SET
	MissionaryCostModifier = 0,
	SpreadDistanceModifier = 40,
	PressureChangeTradeRoute = 200
WHERE Type = 'BELIEF_HOLY_ORDER';

-- Messiah (now Prophecy)
UPDATE Beliefs
SET
	ReducePolicyRequirements = 1,
	InquisitorPressureRetention = 50
WHERE Type = 'BELIEF_MESSIAH';

INSERT INTO Belief_ImprovementYieldChanges
	(BeliefType, ImprovementType, YieldType, Yield)
SELECT
	'BELIEF_MESSIAH', 'IMPROVEMENT_HOLY_SITE', YieldType, 3
FROM Improvement_Yields
WHERE ImprovementType = 'IMPROVEMENT_HOLY_SITE';

-- Missionary Zeal (now Sacred Calendar)
INSERT INTO Belief_YieldChangePerForeignCity
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_MISSIONARY_ZEAL', 'YIELD_GOLD', 3),
	('BELIEF_MISSIONARY_ZEAL', 'YIELD_GOLDEN_AGE_POINTS', 3);

INSERT INTO Belief_GoldenAgeGreatPersonRateModifier
	(BeliefType, GreatPersonType, Modifier)
SELECT
	'BELIEF_MISSIONARY_ZEAL', Type, 33
FROM GreatPersons;

-- Itinerant Preachers (now Symbolism)
UPDATE Beliefs SET SpreadDistanceModifier = 0 WHERE Type = 'BELIEF_ITINERANT_PREACHERS';

INSERT INTO Belief_HolyCityYieldChanges
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_ITINERANT_PREACHERS', 'YIELD_GOLDEN_AGE_POINTS', 5);

INSERT INTO Belief_GreatPersonPoints
	(BeliefType, GreatPersonType, Value)
SELECT
	'BELIEF_ITINERANT_PREACHERS', Type, 2
FROM GreatPersons
WHERE Type NOT IN (
	'GREATPERSON_GENERAL',
	'GREATPERSON_ADMIRAL',
	'GREATPERSON_PROPHET'
);

-- Religious Unity (now Universalism)
UPDATE Beliefs SET FriendlyCityStateSpreadModifier = 0 WHERE Type = 'BELIEF_RELIGIOUS_UNITY';

INSERT INTO Belief_YieldPerOtherReligionFollower
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_RELIGIOUS_UNITY', 'YIELD_PRODUCTION', 5),
	('BELIEF_RELIGIOUS_UNITY', 'YIELD_SCIENCE', 5);

INSERT INTO Belief_YieldChangePerXForeignFollowers
	(BeliefType, YieldType, ForeignFollowers)
VALUES
	('BELIEF_RELIGIOUS_UNITY', 'YIELD_GOLD', 10),
	('BELIEF_RELIGIOUS_UNITY', 'YIELD_FAITH', 10);

INSERT INTO Belief_YieldPerScience
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_RELIGIOUS_UNITY', 'YIELD_FAITH', 15);

-- Heathen Conversion (now Zealotry)
UPDATE Beliefs
SET
	Enhancer = 1,
	Reformation = 0,
	ConvertsBarbarians = 0
WHERE Type = 'BELIEF_HEATHEN_CONVERSION';

INSERT INTO Belief_ResourceQuantityModifiers
	(BeliefType, ResourceType, ResourceQuantityModifier)
SELECT
	'BELIEF_HEATHEN_CONVERSION', Type, 1
FROM Resources
WHERE ResourceUsage = 1 AND Type <> 'RESOURCE_PAPER';

INSERT INTO Belief_EraFaithUnitPurchase
	(BeliefType, EraType)
SELECT
	'BELIEF_HEATHEN_CONVERSION', Type
FROM Eras;
