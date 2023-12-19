-- Cathedrals

-- Pagodas

-- Mosques

-- Sword into Plowshare (now Churches)
UPDATE Beliefs
SET
	CityGrowthModifier = 0,
	RequiresPeace = 0
WHERE Type = 'BELIEF_SWORD_PLOWSHARES';

-- Guruship (now Mandirs)

-- Holy Warriors (now Synagogues)

-- Monasteries (now Stupas)

-- Religious Center (now Orders)
UPDATE Beliefs SET MinFollowers = 0 WHERE Type = 'BELIEF_RELIGIOUS_CENTER';

-- Peace Gardens (now Indulgences)
INSERT INTO Belief_YieldPerGPT
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_PEACE_GARDENS', 'YIELD_FAITH', 10);

INSERT INTO Belief_YieldFromFaithPurchase
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_PEACE_GARDENS', 'YIELD_GOLD', 10),
	('BELIEF_PEACE_GARDENS', 'YIELD_PRODUCTION', 10);

-- Religious Art (now Mastery)
INSERT INTO Belief_SpecialistYieldChanges
	(BeliefType, SpecialistType, YieldType, Yield)
VALUES
	('BELIEF_RELIGIOUS_ART', 'SPECIALIST_WRITER', 'YIELD_CULTURE', 1),
	('BELIEF_RELIGIOUS_ART', 'SPECIALIST_ARTIST', 'YIELD_CULTURE', 1),
	('BELIEF_RELIGIOUS_ART', 'SPECIALIST_MUSICIAN', 'YIELD_CULTURE', 1),
	('BELIEF_RELIGIOUS_ART', 'SPECIALIST_MERCHANT', 'YIELD_GOLD', 1),
	('BELIEF_RELIGIOUS_ART', 'SPECIALIST_SCIENTIST', 'YIELD_SCIENCE', 1),
	('BELIEF_RELIGIOUS_ART', 'SPECIALIST_ENGINEER', 'YIELD_PRODUCTION', 1),
	('BELIEF_RELIGIOUS_ART', 'SPECIALIST_CIVIL_SERVANT', 'YIELD_GOLD', 1);

INSERT INTO Belief_SpecialistYieldChanges
	(BeliefType, SpecialistType, YieldType, Yield)
SELECT
	'BELIEF_RELIGIOUS_ART', Type, 'YIELD_GOLDEN_AGE_POINTS', 1
FROM Specialists
WHERE GreatPeopleUnitClass IS NOT NULL;

-- Asceticism
UPDATE Beliefs SET MinFollowers = 0 WHERE Type = 'BELIEF_ASCETISM';

INSERT INTO Belief_MaxYieldPerFollowerPercent
	(BeliefType, YieldType, Max)
VALUES
	('BELIEF_ASCETISM', 'YIELD_FOOD', 100);

INSERT INTO Belief_MaxYieldPerFollower
	(BeliefType, YieldType, Max)
VALUES
	('BELIEF_ASCETISM', 'YIELD_FOOD', 15);

-- Religious Community (now Diligence)
INSERT INTO Belief_MaxYieldPerFollowerPercent
	(BeliefType, YieldType, Max)
VALUES
	('BELIEF_RELIGIOUS_COMMUNITY', 'YIELD_PRODUCTION', 50);

INSERT INTO Belief_MaxYieldPerFollower
	(BeliefType, YieldType, Max)
VALUES
	('BELIEF_RELIGIOUS_COMMUNITY', 'YIELD_PRODUCTION', 15);

-- Divine Inspiration (now Creativity)
INSERT INTO Belief_MaxYieldPerFollowerPercent
	(BeliefType, YieldType, Max)
VALUES
	('BELIEF_DIVINE_INSPIRATION', 'YIELD_CULTURE', 34);

INSERT INTO Belief_MaxYieldPerFollower
	(BeliefType, YieldType, Max)
VALUES
	('BELIEF_DIVINE_INSPIRATION', 'YIELD_CULTURE', 6);

INSERT INTO Belief_YieldChangeAnySpecialist
(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_DIVINE_INSPIRATION', 'YIELD_FAITH', 2);

-- Choral Music (now Scholarship)
UPDATE Beliefs SET MinFollowers = 0 WHERE Type = 'BELIEF_CHORAL_MUSIC';

INSERT INTO Belief_MaxYieldPerFollowerPercent
	(BeliefType, YieldType, Max)
VALUES
	('BELIEF_CHORAL_MUSIC', 'YIELD_SCIENCE', 50);

INSERT INTO Belief_MaxYieldPerFollower
	(BeliefType, YieldType, Max)
VALUES
	('BELIEF_CHORAL_MUSIC', 'YIELD_SCIENCE', 15);

-- Feed the World (now Thrift)
INSERT INTO Belief_MaxYieldPerFollowerPercent
	(BeliefType, YieldType, Max)
VALUES
	('BELIEF_FEED_WORLD', 'YIELD_GOLD', 100);

INSERT INTO Belief_MaxYieldPerFollower
	(BeliefType, YieldType, Max)
VALUES
	('BELIEF_FEED_WORLD', 'YIELD_GOLD', 10);

-- Liturgical Drama (now Gurukulam)
UPDATE Beliefs SET MinFollowers = 0 WHERE Type = 'BELIEF_LITURGICAL_DRAMA';

INSERT INTO Belief_YieldPerScience
	(BeliefType, YieldType, Yield)
VALUES ('BELIEF_LITURGICAL_DRAMA', 'YIELD_FAITH', 5);

INSERT INTO Belief_YieldFromFaithPurchase
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_LITURGICAL_DRAMA', 'YIELD_FOOD', 10),
	('BELIEF_LITURGICAL_DRAMA', 'YIELD_CULTURE', 10);

-- Cooperation
INSERT INTO Belief_YieldChangeAnySpecialist
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_COMMUNALISM', 'YIELD_PRODUCTION', 2);

INSERT INTO Belief_TradeRouteYieldChange
	(BeliefType, DomainType, YieldType, Yield)
VALUES
	('BELIEF_COMMUNALISM', 'DOMAIN_LAND', 'YIELD_FOOD', 3),
	('BELIEF_COMMUNALISM', 'DOMAIN_LAND', 'YIELD_PRODUCTION', 3),
	('BELIEF_COMMUNALISM', 'DOMAIN_SEA', 'YIELD_FOOD', 3),
	('BELIEF_COMMUNALISM', 'DOMAIN_SEA', 'YIELD_PRODUCTION', 3);

-- Common, use this table to determine which follower building can be faith purchased
INSERT INTO Belief_BuildingClassFaithPurchase
	(BeliefType, BuildingClassType)
VALUES
	('BELIEF_CATHEDRALS', 'BUILDINGCLASS_CATHEDRAL'),
	('BELIEF_MOSQUES', 'BUILDINGCLASS_MOSQUE'),
	('BELIEF_PAGODAS', 'BUILDINGCLASS_PAGODA'),
	('BELIEF_MONASTERIES', 'BUILDINGCLASS_STUPA'),
	('BELIEF_SWORD_PLOWSHARES', 'BUILDINGCLASS_CHURCH'),
	('BELIEF_GURUSHIP', 'BUILDINGCLASS_MANDIR'),
	('BELIEF_HOLY_WARRIORS', 'BUILDINGCLASS_SYNAGOGUE'),
	('BELIEF_RELIGIOUS_CENTER', 'BUILDINGCLASS_ORDER'),
	('BELIEF_GURDWARA', 'BUILDINGCLASS_GURDWARA'),
	('BELIEF_TEOCALLI', 'BUILDINGCLASS_TEOCALLI');
