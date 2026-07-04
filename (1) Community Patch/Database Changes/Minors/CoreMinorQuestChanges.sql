CREATE TEMP TABLE Helper (
	MinorCivTraitType TEXT,
	MinorCivPersonalityType TEXT,
	QuestType TEXT,
	Weight INTEGER
);

-- This code replaces the previous ugly method of using GameDefines for every quest combination
-- Start by adding 10 weight to every quest, even the ones disabled in Community Patch Only
INSERT INTO Helper
	(MinorCivTraitType, MinorCivPersonalityType, QuestType, Weight)
SELECT
	a.Type, b.Type, c.Type, 10
FROM MinorCivTraits a, MinorCivPersonalityTypes b, SmallAwards c;

-- Disable the quests we don't want to pop up by setting their weight to 0
UPDATE Helper
SET Weight = 0
WHERE QuestType NOT IN
	('MINOR_CIV_QUEST_ROUTE',
	'MINOR_CIV_QUEST_CONNECT_RESOURCE',
	'MINOR_CIV_QUEST_CONSTRUCT_WONDER',
	'MINOR_CIV_QUEST_GREAT_PERSON',
	--'MINOR_CIV_QUEST_KILL_CITY_STATE', -- this one is from pre-BNW, but is disabled in BNW
	'MINOR_CIV_QUEST_FIND_PLAYER',
	'MINOR_CIV_QUEST_FIND_NATURAL_WONDER',
	'MINOR_CIV_QUEST_GIVE_GOLD',
	'MINOR_CIV_QUEST_PLEDGE_TO_PROTECT',
	'MINOR_CIV_QUEST_BULLY_CITY_STATE',
	'MINOR_CIV_QUEST_DENOUNCE_MAJOR',
	'MINOR_CIV_QUEST_SPREAD_RELIGION',
	'MINOR_CIV_QUEST_TRADE_ROUTE',
	'MINOR_CIV_QUEST_KILL_CAMP',
	'MINOR_CIV_QUEST_CONTEST_CULTURE',
	'MINOR_CIV_QUEST_CONTEST_FAITH',
	'MINOR_CIV_QUEST_CONTEST_TECHS',
	'MINOR_CIV_QUEST_INVEST'
);

-- Now process the individual BNW overrides for each quest

-- PERSONAL QUESTS
-- Construct Route
UPDATE Helper SET Weight = 12 WHERE QuestType = 'MINOR_CIV_QUEST_ROUTE' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MARITIME';
UPDATE Helper SET Weight = 15 WHERE QuestType = 'MINOR_CIV_QUEST_ROUTE' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MERCANTILE';
UPDATE Helper SET Weight = 20 WHERE QuestType = 'MINOR_CIV_QUEST_ROUTE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_FRIENDLY';
UPDATE Helper SET Weight = 24 WHERE QuestType = 'MINOR_CIV_QUEST_ROUTE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_FRIENDLY' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MARITIME';
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_ROUTE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_FRIENDLY' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MERCANTILE';
UPDATE Helper SET Weight = 2 WHERE QuestType = 'MINOR_CIV_QUEST_ROUTE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_HOSTILE';
UPDATE Helper SET Weight = 3 WHERE QuestType = 'MINOR_CIV_QUEST_ROUTE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_HOSTILE' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MERCANTILE';

-- Connect Resource
UPDATE Helper SET Weight = 20 WHERE QuestType = 'MINOR_CIV_QUEST_CONNECT_RESOURCE' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MARITIME';
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_CONNECT_RESOURCE' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MERCANTILE';

-- Build Wonder
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_CONSTRUCT_WONDER' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_CULTURED';

-- Generate Great Person
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_GREAT_PERSON' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_CULTURED';

-- Find Player
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_FIND_PLAYER' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MARITIME';
UPDATE Helper SET Weight = 20 WHERE QuestType = 'MINOR_CIV_QUEST_FIND_PLAYER' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MERCANTILE';

-- Find Natural Wonder
UPDATE Helper SET Weight = 5 WHERE QuestType = 'MINOR_CIV_QUEST_FIND_NATURAL_WONDER' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MILITARISTIC';
UPDATE Helper SET Weight = 3 WHERE QuestType = 'MINOR_CIV_QUEST_FIND_NATURAL_WONDER' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_HOSTILE';
UPDATE Helper SET Weight = 1 WHERE QuestType = 'MINOR_CIV_QUEST_FIND_NATURAL_WONDER' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MILITARISTIC' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_HOSTILE';

-- Give Gold
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_GIVE_GOLD';
UPDATE Helper SET Weight = 20 WHERE QuestType = 'MINOR_CIV_QUEST_GIVE_GOLD' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MILITARISTIC';
UPDATE Helper SET Weight = 35 WHERE QuestType = 'MINOR_CIV_QUEST_GIVE_GOLD' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MERCANTILE';

-- Pledge Protection
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_PLEDGE_TO_PROTECT';
UPDATE Helper SET Weight = 35 WHERE QuestType = 'MINOR_CIV_QUEST_PLEDGE_TO_PROTECT' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_CULTURED';
UPDATE Helper SET Weight = 20 WHERE QuestType = 'MINOR_CIV_QUEST_PLEDGE_TO_PROTECT' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MILITARISTIC';

-- Demand Tribute from City-State
UPDATE Helper SET Weight = 20 WHERE QuestType = 'MINOR_CIV_QUEST_BULLY_CITY_STATE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_HOSTILE';
UPDATE Helper SET Weight = 15 WHERE QuestType = 'MINOR_CIV_QUEST_BULLY_CITY_STATE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_IRRATIONAL';
UPDATE Helper SET Weight = 3 WHERE QuestType = 'MINOR_CIV_QUEST_BULLY_CITY_STATE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_FRIENDLY';

-- Denounce Player
UPDATE Helper SET Weight = 15 WHERE QuestType = 'MINOR_CIV_QUEST_DENOUNCE_MAJOR';
UPDATE Helper SET Weight = 25 WHERE QuestType = 'MINOR_CIV_QUEST_DENOUNCE_MAJOR' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_RELIGIOUS';
UPDATE Helper SET Weight = 20 WHERE QuestType = 'MINOR_CIV_QUEST_DENOUNCE_MAJOR' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MARITIME';

-- Spread Religion
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_SPREAD_RELIGION' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_RELIGIOUS';

-- Establish Trade Route
UPDATE Helper SET Weight = 20 WHERE QuestType = 'MINOR_CIV_QUEST_TRADE_ROUTE' AND MinorCivTraitType IN ('MINOR_CIV_TRAIT_MARITIME', 'MINOR_CIV_TRAIT_MERCANTILE');


-- GLOBAL QUESTS
-- Kill Camp
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_KILL_CAMP';
UPDATE Helper SET Weight = 90 WHERE QuestType = 'MINOR_CIV_QUEST_KILL_CAMP' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MILITARISTIC';

-- Culture Contest
UPDATE Helper SET Weight = 20 WHERE QuestType = 'MINOR_CIV_QUEST_CONTEST_CULTURE' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_CULTURED';

-- Faith Contest
UPDATE Helper SET Weight = 5 WHERE QuestType = 'MINOR_CIV_QUEST_CONTEST_FAITH';
UPDATE Helper SET Weight = 20 WHERE QuestType = 'MINOR_CIV_QUEST_CONTEST_FAITH' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_RELIGIOUS';

-- Tech Contest
UPDATE Helper SET Weight = 5 WHERE QuestType = 'MINOR_CIV_QUEST_CONTEST_TECHS' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_RELIGIOUS'; -- a typo sets this to 0 in vanilla BNW; 5 is the intended value

-- Invest
UPDATE Helper SET Weight = 15 WHERE QuestType = 'MINOR_CIV_QUEST_INVEST' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MERCANTILE';


-- Now move this data to the table where it is used!
-- Different quest weights by era are supported but not used in Community Patch Only
INSERT INTO MinorCivilizations_QuestWeights
	(MinorCivTraitType, MinorCivPersonalityType, EraType, QuestType, Weight)
SELECT
	a.MinorCivTraitType, a.MinorCivPersonalityType, b.Type, a.QuestType, a.Weight
FROM Helper a, Eras b;

DROP TABLE Helper;
