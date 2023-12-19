-- Evangelism (now Crusader Spirit)
UPDATE Beliefs
SET
	OtherReligionPressureErosion = 0,
	CombatVersusOtherReligionTheirLands = 20
WHERE Type = 'BELIEF_EVANGELISM';

INSERT INTO Belief_YieldFromConquest
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_EVANGELISM', 'YIELD_GOLD', 50),
	('BELIEF_EVANGELISM', 'YIELD_CULTURE', 50);

-- Defender of the Faith
UPDATE Beliefs
SET
	Enhancer = 0,
	Reformation = 1,
	CombatModifierFriendlyCities = 0,
	CombatVersusOtherReligionOwnLands = 20
WHERE Type = 'BELIEF_DEFENDER_FAITH';

CREATE TEMP TABLE Helper (
	BuildingClassType TEXT
);

CREATE TEMP TABLE Helper2 (
	YieldType TEXT,
	YieldChange INTEGER
);

INSERT INTO Helper
VALUES
	('BUILDINGCLASS_WALLS'),
	('BUILDINGCLASS_CASTLE'),
	('BUILDINGCLASS_FORTRESS'),
	('BUILDINGCLASS_ARSENAL'),
	('BUILDINGCLASS_MILITARY_BASE'),
	('BUILDINGCLASS_MINEFIELD'),
	('BUILDINGCLASS_BOMB_SHELTER');

INSERT INTO Helper2
VALUES
	('YIELD_CULTURE', 2),
	('YIELD_FAITH', 1);

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
SELECT
	'BELIEF_DEFENDER_FAITH', a.BuildingClassType, b.YieldType, b.YieldChange
FROM Helper a, Helper2 b;

DROP TABLE Helper;
DROP TABLE Helper2;

-- Jesuit Education (now Divine Teachings)
UPDATE Beliefs SET GreatPersonExpendedFaith = 20 WHERE Type = 'BELIEF_JESUIT_EDUCATION';

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_JESUIT_EDUCATION', 'BUILDINGCLASS_LIBRARY', 'YIELD_SCIENCE', 2),
	('BELIEF_JESUIT_EDUCATION', 'BUILDINGCLASS_UNIVERSITY', 'YIELD_SCIENCE', 2),
	('BELIEF_JESUIT_EDUCATION', 'BUILDINGCLASS_PUBLIC_SCHOOL', 'YIELD_SCIENCE', 2),
	('BELIEF_JESUIT_EDUCATION', 'BUILDINGCLASS_LABORATORY', 'YIELD_SCIENCE', 2);

INSERT INTO Belief_BuildingClassFaithPurchase
	(BeliefType, BuildingClassType)
VALUES
	('BELIEF_JESUIT_EDUCATION', 'BUILDINGCLASS_LIBRARY'),
	('BELIEF_JESUIT_EDUCATION', 'BUILDINGCLASS_UNIVERSITY'),
	('BELIEF_JESUIT_EDUCATION', 'BUILDINGCLASS_PUBLIC_SCHOOL'),
	('BELIEF_JESUIT_EDUCATION', 'BUILDINGCLASS_LABORATORY');

-- Underground Sect (now Faith of the Masses)
UPDATE Beliefs
SET
	SpyPressure = 0,
	HappinessPerFollowingCity = 0.5
WHERE Type = 'BELIEF_UNDERGROUND_SECT';

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_UNDERGROUND_SECT', 'BUILDINGCLASS_AMPHITHEATER', 'YIELD_CULTURE', 2),
	('BELIEF_UNDERGROUND_SECT', 'BUILDINGCLASS_OPERA_HOUSE', 'YIELD_CULTURE', 2),
	('BELIEF_UNDERGROUND_SECT', 'BUILDINGCLASS_MUSEUM', 'YIELD_CULTURE', 2),
	('BELIEF_UNDERGROUND_SECT', 'BUILDINGCLASS_BROADCAST_TOWER', 'YIELD_CULTURE', 2);

INSERT INTO Belief_BuildingClassFaithPurchase
	(BeliefType, BuildingClassType)
VALUES
	('BELIEF_UNDERGROUND_SECT', 'BUILDINGCLASS_AMPHITHEATER'),
	('BELIEF_UNDERGROUND_SECT', 'BUILDINGCLASS_OPERA_HOUSE'),
	('BELIEF_UNDERGROUND_SECT', 'BUILDINGCLASS_MUSEUM'),
	('BELIEF_UNDERGROUND_SECT', 'BUILDINGCLASS_BROADCAST_TOWER');

-- Charitable Missions (now Global Commandments)
UPDATE Beliefs
SET
	CityStateInfluenceModifier = 0,
	SpreadStrengthModifier = 15
WHERE Type = 'BELIEF_CHARITABLE_MISSIONS';

INSERT INTO Belief_YieldFromProposal
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_CHARITABLE_MISSIONS', 'YIELD_GOLD', 350),
	('BELIEF_CHARITABLE_MISSIONS', 'YIELD_SCIENCE', 350),
	('BELIEF_CHARITABLE_MISSIONS', 'YIELD_CULTURE', 350),
	('BELIEF_CHARITABLE_MISSIONS', 'YIELD_FAITH', 350),
	('BELIEF_CHARITABLE_MISSIONS', 'YIELD_GOLDEN_AGE_POINTS', 350);

-- Religious Fervor (now Holy Land)
UPDATE Beliefs SET CSYieldBonusFromSharedReligion = 50 WHERE Type = 'BELIEF_RELIGIOUS_FERVOR';

INSERT INTO Belief_VotePerXImprovementOwned
	(BeliefType, ImprovementType, Amount)
VALUES
	('BELIEF_RELIGIOUS_FERVOR', 'IMPROVEMENT_HOLY_SITE', 2),
	('BELIEF_RELIGIOUS_FERVOR', 'IMPROVEMENT_LANDMARK', 2);

-- Unity of the Prophets (now Inspired Works)
UPDATE Beliefs
SET InquisitorPressureRetention = 0
WHERE Type = 'BELIEF_UNITY_OF_PROPHETS';

CREATE TEMP TABLE Helper3 (
	YieldType TEXT
);

INSERT INTO Helper3
VALUES
	('YIELD_SCIENCE'),
	('YIELD_FAITH');

INSERT INTO Belief_ImprovementYieldChanges
	(BeliefType, ImprovementType, YieldType, Yield)
SELECT
	'BELIEF_UNITY_OF_PROPHETS', a.Type, b.YieldType, 2
FROM Improvements a, Helper3 b
WHERE a.CreatedByGreatPerson = 1 OR a.Type = 'IMPROVEMENT_LANDMARK';

DROP TABLE Helper3;

INSERT INTO Belief_SpecificFaithUnitPurchase
	(BeliefType, UnitType)
SELECT
	'BELIEF_UNITY_OF_PROPHETS', Type
FROM Units
WHERE Class = 'UNITCLASS_ARCHAEOLOGIST';

INSERT INTO Belief_GreatWorkYieldChanges
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_UNITY_OF_PROPHETS', 'YIELD_CULTURE', 2);

-- Sacred Sites
UPDATE Beliefs SET FaithBuildingTourism = 3 WHERE Type = 'BELIEF_SACRED_SITES';

INSERT INTO Belief_YieldChangeWorldWonder
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_SACRED_SITES', 'YIELD_TOURISM', 4);

INSERT INTO Belief_YieldChangeNaturalWonder
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_SACRED_SITES', 'YIELD_TOURISM', 4);

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_SACRED_SITES', 'BUILDINGCLASS_HOTEL', 'YIELD_TOURISM', 3),
	('BELIEF_SACRED_SITES', 'BUILDINGCLASS_HERMITAGE', 'YIELD_CULTURE', 10),
	('BELIEF_SACRED_SITES', 'BUILDINGCLASS_HERMITAGE', 'YIELD_TOURISM', 10);

-- To the Glory of God
UPDATE Beliefs SET CityScalerLimiter = 20 WHERE Type = 'BELIEF_TO_GLORY_OF_GOD';

CREATE TEMP TABLE Helper4 (
	YieldType TEXT
);

INSERT INTO Helper4
VALUES
	('YIELD_GOLD'),
	('YIELD_SCIENCE'),
	('YIELD_CULTURE');

INSERT INTO Belief_GreatPersonExpendedYield -- does NOT scale with era unlike Ceremonial Burial
	(BeliefType, GreatPersonType, YieldType, Yield)
SELECT
	'BELIEF_TO_GLORY_OF_GOD', a.Type, b.YieldType, 3
FROM GreatPersons a, Helper4 b;

DROP TABLE Helper4;
