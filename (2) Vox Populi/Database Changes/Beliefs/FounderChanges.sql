-- Ceremonial Burial
UPDATE Beliefs
SET
	HappinessPerFollowingCity = 0,
	CityScalerLimiter = 25
WHERE Type = 'BELIEF_CEREMONIAL_BURIAL';

INSERT INTO Belief_YieldFromGPUse
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_CEREMONIAL_BURIAL', 'YIELD_CULTURE', 10),
	('BELIEF_CEREMONIAL_BURIAL', 'YIELD_FAITH', 10);

-- Papal Primacy (now Council of Elders)
UPDATE Beliefs
SET
	CityStateMinimumInfluence = 0,
	CityScalerLimiter = 25
WHERE Type = 'BELIEF_PAPAL_PRIMACY';

INSERT INTO Belief_YieldFromConversion -- scaler = +(X^2)% where X is the number of cities following the religion
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_PAPAL_PRIMACY', 'YIELD_PRODUCTION', 20),
	('BELIEF_PAPAL_PRIMACY', 'YIELD_SCIENCE', 20);

-- Peace Loving (now Divine Inheritance)
UPDATE Beliefs SET HappinessPerXPeacefulForeignFollowers = 0 WHERE Type = 'BELIEF_PEACE_LOVING';

INSERT INTO Belief_YieldBonusGoldenAge
	(BeliefType, YieldType, Yield)
SELECT
	'BELIEF_PEACE_LOVING', Type, 20
FROM Yields
WHERE ID < 6; -- "All" yields

-- Pilgrimage (now Evangelism)
INSERT INTO Belief_YieldFromSpread
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_PILGRIMAGE', 'YIELD_FOOD', 15);

INSERT INTO Belief_YieldFromForeignSpread
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_PILGRIMAGE', 'YIELD_TOURISM', 15);

-- Interfaith Dialogue (now Hero Worship)
UPDATE Beliefs SET SciencePerOtherReligionFollower = 0 WHERE Type = 'BELIEF_INTERFAITH_DIALOGUE';

INSERT INTO Belief_YieldFromConquest
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_INTERFAITH_DIALOGUE', 'YIELD_FAITH', 100),
	('BELIEF_INTERFAITH_DIALOGUE', 'YIELD_GOLDEN_AGE_POINTS', 100),
	('BELIEF_INTERFAITH_DIALOGUE', 'YIELD_GREAT_GENERAL_POINTS', 25),
	('BELIEF_INTERFAITH_DIALOGUE', 'YIELD_GREAT_ADMIRAL_POINTS', 25);

-- Church Property (now Holy Law)
UPDATE Beliefs
SET
	GoldPerFollowingCity = 0,
	FollowerScalerLimiter = 250
WHERE Type = 'BELIEF_CHURCH_PROPERTY';

INSERT INTO Belief_YieldFromPolicyUnlock
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_CHURCH_PROPERTY', 'YIELD_GOLD', 5),
	('BELIEF_CHURCH_PROPERTY', 'YIELD_SCIENCE', 5),
	('BELIEF_CHURCH_PROPERTY', 'YIELD_FAITH', 5);

-- Tithe (now Revelation)
UPDATE Beliefs
SET
	GoldPerXFollowers = 0,
	FollowerScalerLimiter = 250
WHERE Type = 'BELIEF_TITHE';

INSERT INTO Belief_YieldFromTechUnlock
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_TITHE', 'YIELD_CULTURE', 2),
	('BELIEF_TITHE', 'YIELD_FAITH', 2),
	('BELIEF_TITHE', 'YIELD_GOLDEN_AGE_POINTS', 2);

-- World Church (now Theocratic Rule)
INSERT INTO Belief_YieldFromWLTKD
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_WORLD_CHURCH', 'YIELD_GOLD', 15),
	('BELIEF_WORLD_CHURCH', 'YIELD_CULTURE', 15),
	('BELIEF_WORLD_CHURCH', 'YIELD_FAITH', 15);

-- Initiation Rites (now Transcendent Thoughts)
UPDATE Beliefs
SET
	GoldPerFirstCityConversion = 0,
	CityScalerLimiter = 25
WHERE Type = 'BELIEF_INITIATION_RITES';

INSERT INTO Belief_YieldFromEraUnlock
	(BeliefType, YieldType, Yield)
SELECT
	'BELIEF_INITIATION_RITES', Type, 12
FROM Yields
WHERE ID < 6; -- "All" yields

-- Common, use this table to determine which reformation building can be built
INSERT INTO Belief_BuildingClassFaithPurchase
	(BeliefType, BuildingClassType)
VALUES
	('BELIEF_CEREMONIAL_BURIAL', 'BUILDINGCLASS_MAUSOLEUM'),
	('BELIEF_PEACE_LOVING', 'BUILDINGCLASS_HEAVENLY_THRONE'),
	('BELIEF_INTERFAITH_DIALOGUE', 'BUILDINGCLASS_GREAT_ALTAR'),
	('BELIEF_CHURCH_PROPERTY', 'BUILDINGCLASS_DIVINE_COURT'),
	('BELIEF_TITHE', 'BUILDINGCLASS_RELIGIOUS_LIBRARY'),
	('BELIEF_INITIATION_RITES', 'BUILDINGCLASS_SACRED_GARDEN'),
	('BELIEF_PAPAL_PRIMACY', 'BUILDINGCLASS_HOLY_COUNCIL'),
	('BELIEF_PILGRIMAGE', 'BUILDINGCLASS_APOSTOLIC_PALACE'),
	('BELIEF_WORLD_CHURCH', 'BUILDINGCLASS_GRAND_OSSUARY');
