---------------------------------------------------------------------------------------------------------
-- Global Quests
-- Given to every met player at the same time, and ends when any player completes the goal
---------------------------------------------------------------------------------------------------------

-- Ally or Conquer specific CS
UPDATE SmallAwards SET Influence = 50, Gold = 200, GoldenAgePoints = 200, RandomMod = 10, QuestDuration = 60 WHERE Type = 'MINOR_CIV_QUEST_KILL_CITY_STATE';

-- Destroy specific Barb Camp
UPDATE SmallAwards SET Influence = 25, GeneralPoints = 30, RandomMod = 10 WHERE Type = 'MINOR_CIV_QUEST_KILL_CAMP';

-- Excavate specific Dig Site
UPDATE SmallAwards SET Influence = 25, Tourism = 200, RandomMod = 10 WHERE Type = 'MINOR_CIV_QUEST_ARCHAEOLOGY';

-- Circumnavigate the World
UPDATE SmallAwards SET Influence = 30, AdmiralPoints = 25, RandomMod = 10 WHERE Type = 'MINOR_CIV_QUEST_CIRCUMNAVIGATION';

-- Faith contest
UPDATE SmallAwards SET Influence = 30, GlobalGPPoints = 20, RandomMod = 10 WHERE Type = 'MINOR_CIV_QUEST_CONTEST_FAITH';

-- Artsy Units contest
UPDATE SmallAwards SET Influence = 50, Happiness = 1, RandomMod = 10 WHERE Type = 'MINOR_CIV_QUEST_CONTEST_ARTSY_UNITS';

-- Sciencey Units contest
UPDATE SmallAwards SET Influence = 25, Gold = 300, GoldenAgePoints = 300, RandomMod = 10 WHERE Type = 'MINOR_CIV_QUEST_CONTEST_SCIENCEY_UNITS';

-- Tech contest
UPDATE SmallAwards SET Influence = 25, Tourism = 300, RandomMod = 10 WHERE Type = 'MINOR_CIV_QUEST_CONTEST_TECHS';

---------------------------------------------------------------------------------------------------------
-- Personal Quests
-- Given to one player only
---------------------------------------------------------------------------------------------------------

-- Build specific World Wonder
UPDATE SmallAwards SET Influence = 30, Faith = 65, RandomMod = 10, QuestDuration = 50 WHERE Type = 'MINOR_CIV_QUEST_CONSTRUCT_WONDER';

-- Build specific National Wonder
UPDATE SmallAwards SET Influence = 30, GoldenAgePoints = 125, RandomMod = 10, QuestDuration = 50 WHERE Type = 'MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER';

-- Build X specific Buildings
UPDATE SmallAwards SET Influence = 40, Production = 75, RandomMod = 10, QuestDuration = 50 WHERE Type = 'MINOR_CIV_QUEST_BUILD_X_BUILDINGS';

-- Find specific City
UPDATE SmallAwards SET Influence = 20, Science = 30, RandomMod = 5, QuestDuration = 50 WHERE Type = 'MINOR_CIV_QUEST_FIND_CITY';

-- Find specific CS
UPDATE SmallAwards SET Influence = 15, Production = 60, RandomMod = 10, QuestDuration = 50 WHERE Type = 'MINOR_CIV_QUEST_FIND_CITY_STATE';

-- Find Natural Wonder
UPDATE SmallAwards SET Influence = 30, GoldenAgePoints = 150, RandomMod = 10 WHERE Type = 'MINOR_CIV_QUEST_FIND_NATURAL_WONDER';

-- Bully specific CS
UPDATE SmallAwards SET Influence = 20, Gold = 60, RandomMod = 5, QuestDuration = 0 WHERE Type = 'MINOR_CIV_QUEST_BULLY_CITY_STATE';

-- Acquire specific City
UPDATE SmallAwards SET Influence = 80, Juggernauts = 1, QuestDuration = 50 WHERE Type = 'MINOR_CIV_QUEST_ACQUIRE_CITY';

-- Liberate specific CS
UPDATE SmallAwards SET Influence = 40, Happiness = 1, RandomMod = 10, QuestDuration = 50 WHERE Type = 'MINOR_CIV_QUEST_LIBERATION';

-- Coup specific CS
UPDATE SmallAwards SET Influence = 40, Science = 100, RandomMod = 5 WHERE Type = 'MINOR_CIV_QUEST_COUP';

-- Declare War on specific Player
UPDATE SmallAwards SET Influence = 75, Happiness = 1, RandomMod = 10, QuestDuration = 50 WHERE Type = 'MINOR_CIV_QUEST_WAR';

-- Denounce specific Player
UPDATE SmallAwards SET Influence = 30, GeneralPoints = 50, RandomMod = 10 WHERE Type = 'MINOR_CIV_QUEST_DENOUNCE_MAJOR';

-- Conduct spy mission
UPDATE SmallAwards SET Influence = 60, Science = 150, RandomMod = 10 WHERE Type = 'MINOR_CIV_QUEST_SPY_MISSION';

-- Pledge to Protect us
UPDATE SmallAwards SET Influence = 30, CapitalGPPoints = 20, RandomMod = 10 WHERE Type = 'MINOR_CIV_QUEST_PLEDGE_TO_PROTECT';

-- Spread Religion to us
UPDATE SmallAwards SET Influence = 30, Faith = 70, RandomMod = 10, QuestDuration = 50 WHERE Type = 'MINOR_CIV_QUEST_SPREAD_RELIGION';

-- Make City Connection to us
UPDATE SmallAwards SET Influence = 40, Production = 70, RandomMod = 10, QuestDuration = 50 WHERE Type = 'MINOR_CIV_QUEST_ROUTE';

-- Start Trade Route with us
UPDATE SmallAwards SET Influence = 30, Food = 50, RandomMod = 10, QuestDuration = 50 WHERE Type = 'MINOR_CIV_QUEST_TRADE_ROUTE';

-- Gift specific Unit to us
UPDATE SmallAwards SET Influence = 25, Gold = 60, RandomMod = 10, QuestDuration = 50 WHERE Type = 'MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT';

-- Give Gold to us (disabled)
UPDATE SmallAwards SET Influence = 25, Food = 100, RandomMod = 10 WHERE Type = 'MINOR_CIV_QUEST_GIVE_GOLD';

-- Connect specific Resource
UPDATE SmallAwards SET Influence = 25, Gold = 90, RandomMod = 10, QuestDuration = 50 WHERE Type = 'MINOR_CIV_QUEST_CONNECT_RESOURCE';

-- Generate specific GP
UPDATE SmallAwards SET Influence = 25, Culture = 65, RandomMod = 10, QuestDuration = 50 WHERE Type = 'MINOR_CIV_QUEST_GREAT_PERSON';

-- Explore specific area
UPDATE SmallAwards SET Influence = 30, Gold = 100, RandomMod = 10, QuestDuration = 50 WHERE Type = 'MINOR_CIV_QUEST_EXPLORE_AREA';

---------------------------------------------------------------------------------------------------------
-- Quest reward modifiers for pledges and CS personalities (additive)
---------------------------------------------------------------------------------------------------------

-- % change to quest rewards if pledged to protect
UPDATE Defines SET Value = 15 WHERE Name = 'BALANCE_INFLUENCE_BOOST_PROTECTION_MINOR';

-- % change to quest rewards from friendly CS
UPDATE Defines SET Value = 25 WHERE Name = 'MINOR_CIV_QUEST_REWARD_FRIENDLY';

-- % change to quest rewards from hostile CS
UPDATE Defines SET Value = -25 WHERE Name = 'MINOR_CIV_QUEST_REWARD_HOSTILE';

---------------------------------------------------------------------------------------------------------
-- Quest reward multipliers for CS types (multiplicative, applied after above modifiers)
-- TODO: make a table for this
---------------------------------------------------------------------------------------------------------

-- Food (in Capital)
UPDATE Defines SET Value = 150 WHERE Name = 'MINOR_CIV_QUEST_REWARD_FOOD_MARITIME';
UPDATE Defines SET Value = 50 WHERE Name = 'MINOR_CIV_QUEST_REWARD_FOOD_MERCANTILE';
UPDATE Defines SET Value = 80 WHERE Name = 'MINOR_CIV_QUEST_REWARD_FOOD_NEUTRAL';

-- Production (in Capital)
UPDATE Defines SET Value = 125 WHERE Name = 'MINOR_CIV_QUEST_REWARD_PRODUCTION_MILITARISTIC';

-- Gold
UPDATE Defines SET Value = 150 WHERE Name = 'MINOR_CIV_QUEST_REWARD_GOLD_MERCANTILE';
UPDATE Defines SET Value = 125 WHERE Name = 'MINOR_CIV_QUEST_REWARD_GOLD_MARITIME';

-- Science
UPDATE Defines SET Value = 60 WHERE Name = 'MINOR_CIV_QUEST_REWARD_SCIENCE_RELIGIOUS';

-- Culture
UPDATE Defines SET Value = 150 WHERE Name = 'MINOR_CIV_QUEST_REWARD_CULTURE_CULTURED';
UPDATE Defines SET Value = 80 WHERE Name = 'MINOR_CIV_QUEST_REWARD_CULTURE_NEUTRAL';
UPDATE Defines SET Value = 125 WHERE Name = 'MINOR_CIV_QUEST_REWARD_CULTURE_IRRATIONAL';

-- Faith
UPDATE Defines SET Value = 150 WHERE Name = 'MINOR_CIV_QUEST_REWARD_FAITH_RELIGIOUS';
UPDATE Defines SET Value = 60 WHERE Name = 'MINOR_CIV_QUEST_REWARD_FAITH_NEUTRAL';

-- Golden Age Point
UPDATE Defines SET Value = 150 WHERE Name = 'MINOR_CIV_QUEST_REWARD_GAP_CULTURED';
UPDATE Defines SET Value = 125 WHERE Name = 'MINOR_CIV_QUEST_REWARD_GAP_RELIGIOUS';
UPDATE Defines SET Value = 60 WHERE Name = 'MINOR_CIV_QUEST_REWARD_GAP_IRRATIONAL';

-- Tourism
UPDATE Defines SET Value = 50 WHERE Name = 'MINOR_CIV_QUEST_REWARD_TOURISM_MILITARISTIC';
UPDATE Defines SET Value = 125 WHERE Name = 'MINOR_CIV_QUEST_REWARD_TOURISM_CULTURED';
UPDATE Defines SET Value = 125 WHERE Name = 'MINOR_CIV_QUEST_REWARD_TOURISM_NEUTRAL';
UPDATE Defines SET Value = 75 WHERE Name = 'MINOR_CIV_QUEST_REWARD_TOURISM_IRRATIONAL';

-- Great General Point
UPDATE Defines SET Value = 150 WHERE Name = 'MINOR_CIV_QUEST_REWARD_GENERAL_POINTS_MILITARISTIC';
UPDATE Defines SET Value = 75 WHERE Name = 'MINOR_CIV_QUEST_REWARD_GENERAL_POINTS_CULTURED';

-- Great Admiral Point
UPDATE Defines SET Value = 150 WHERE Name = 'MINOR_CIV_QUEST_REWARD_ADMIRAL_POINTS_MARITIME';
UPDATE Defines SET Value = 75 WHERE Name = 'MINOR_CIV_QUEST_REWARD_ADMIRAL_POINTS_CULTURED';

-- Great People Point (in Capital)
UPDATE Defines SET Value = 75 WHERE Name = 'MINOR_CIV_QUEST_REWARD_GPP_CULTURED';
UPDATE Defines SET Value = 150 WHERE Name = 'MINOR_CIV_QUEST_REWARD_GPP_NEUTRAL';
UPDATE Defines SET Value = 75 WHERE Name = 'MINOR_CIV_QUEST_REWARD_GPP_IRRATIONAL';

-- Great People Point (global)
UPDATE Defines SET Value = 125 WHERE Name = 'MINOR_CIV_QUEST_REWARD_GPP_GLOBAL_RELIGIOUS';
UPDATE Defines SET Value = 75 WHERE Name = 'MINOR_CIV_QUEST_REWARD_GPP_GLOBAL_CULTURED';
UPDATE Defines SET Value = 125 WHERE Name = 'MINOR_CIV_QUEST_REWARD_GPP_GLOBAL_NEUTRAL';

-- Experience
UPDATE Defines SET Value = 125 WHERE Name = 'MINOR_CIV_QUEST_REWARD_EXPERIENCE_MILITARISTIC';
UPDATE Defines SET Value = 50 WHERE Name = 'MINOR_CIV_QUEST_REWARD_EXPERIENCE_RELIGIOUS';

---------------------------------------------------------------------------------------------------------
-- City-State Quest probabilities
---------------------------------------------------------------------------------------------------------

DELETE FROM MinorCivilizations_QuestWeights;

CREATE TEMP TABLE Helper (
	MinorCivTraitType TEXT,
	MinorCivPersonalityType TEXT,
	QuestType TEXT,
	Weight INTEGER
);

-- This code replaces the previous ugly method of using GameDefines for every quest combination
-- Start by adding 10 weight to every quest, even the ones disabled in Vox Populi
INSERT INTO Helper
	(MinorCivTraitType, MinorCivPersonalityType, QuestType, Weight)
SELECT
	a.Type, b.Type, c.Type, 10
FROM MinorCivTraits a, MinorCivPersonalityTypes b, SmallAwards c;

-- Disable the quests we don't want to pop up by setting their weight to 0
UPDATE Helper
SET Weight = 0
WHERE QuestType IN
	('MINOR_CIV_QUEST_FIND_PLAYER',
	'MINOR_CIV_QUEST_GIVE_GOLD',
	'MINOR_CIV_QUEST_SPY_ON_MAJOR',
	'MINOR_CIV_QUEST_COUP',
	'MINOR_CIV_QUEST_CONTEST_CULTURE',
	'MINOR_CIV_QUEST_INVEST',
	'MINOR_CIV_QUEST_CONTEST_TOURISM'
);

-- Now process the individual VP overrides for each quest

-- PERSONAL QUESTS
-- Construct Route
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_ROUTE' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MERCANTILE';
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_ROUTE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_FRIENDLY';
UPDATE Helper SET Weight = 50 WHERE QuestType = 'MINOR_CIV_QUEST_ROUTE' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MERCANTILE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_FRIENDLY';

-- Connect Resource
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_CONNECT_RESOURCE' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MERCANTILE';
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_CONNECT_RESOURCE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_FRIENDLY';
UPDATE Helper SET Weight = 80 WHERE QuestType = 'MINOR_CIV_QUEST_CONNECT_RESOURCE' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MERCANTILE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_FRIENDLY';

-- Build Wonder
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_CONSTRUCT_WONDER' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_RELIGIOUS';
UPDATE Helper SET Weight = 80 WHERE QuestType = 'MINOR_CIV_QUEST_CONSTRUCT_WONDER' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_RELIGIOUS' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_HOSTILE';

-- Generate Great Person
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_GREAT_PERSON' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_CULTURED';
UPDATE Helper SET Weight = 80 WHERE QuestType = 'MINOR_CIV_QUEST_GREAT_PERSON' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_CULTURED' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_FRIENDLY';

-- Find Natural Wonder
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_FIND_NATURAL_WONDER' AND MinorCivTraitType IN ('MINOR_CIV_TRAIT_MARITIME', 'MINOR_CIV_TRAIT_RELIGIOUS');

-- Pledge Protection
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_PLEDGE_TO_PROTECT' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_NEUTRAL';

-- Demand Tribute from City-State
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_BULLY_CITY_STATE' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MILITARISTIC';
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_BULLY_CITY_STATE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_NEUTRAL';
UPDATE Helper SET Weight = 50 WHERE QuestType = 'MINOR_CIV_QUEST_BULLY_CITY_STATE' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MILITARISTIC' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_NEUTRAL';

-- Denounce Player
UPDATE Helper SET Weight = 80 WHERE QuestType = 'MINOR_CIV_QUEST_DENOUNCE_MAJOR' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MERCANTILE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_HOSTILE';

-- Spread Religion
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_SPREAD_RELIGION' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_RELIGIOUS';
UPDATE Helper SET Weight = 80 WHERE QuestType = 'MINOR_CIV_QUEST_SPREAD_RELIGION' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_RELIGIOUS' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_FRIENDLY';

-- Establish Trade Route
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_TRADE_ROUTE' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MERCANTILE';
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_TRADE_ROUTE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_FRIENDLY';
UPDATE Helper SET Weight = 50 WHERE QuestType = 'MINOR_CIV_QUEST_TRADE_ROUTE' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MERCANTILE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_FRIENDLY';
UPDATE Helper SET Weight = 80 WHERE QuestType = 'MINOR_CIV_QUEST_TRADE_ROUTE' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MERCANTILE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_NEUTRAL';

-- Find City
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_FIND_CITY' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MARITIME';

-- Start War against Player
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_WAR' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MILITARISTIC';
UPDATE Helper SET Weight = 80 WHERE QuestType = 'MINOR_CIV_QUEST_WAR' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MILITARISTIC' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_FRIENDLY';

-- Build National Wonder
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_CULTURED';
UPDATE Helper SET Weight = 80 WHERE QuestType = 'MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_CULTURED' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_HOSTILE';

-- Gift Unit
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_HOSTILE';
UPDATE Helper SET Weight = 80 WHERE QuestType = 'MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_HOSTILE' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MILITARISTIC';

-- Find City-State
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_FIND_CITY_STATE' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MARITIME';

-- Liberate City-State
UPDATE Helper SET Weight = 80 WHERE QuestType = 'MINOR_CIV_QUEST_LIBERATION' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MILITARISTIC' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_FRIENDLY';
UPDATE Helper SET Weight = 80 WHERE QuestType = 'MINOR_CIV_QUEST_LIBERATION' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MERCANTILE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_HOSTILE';

-- Discover Land
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_EXPLORE_AREA' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MARITIME';
UPDATE Helper SET Weight = 80 WHERE QuestType = 'MINOR_CIV_QUEST_EXPLORE_AREA' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MARITIME' AND MinorCivPersonalityType IN ('MINOR_CIV_PERSONALITY_FRIENDLY', 'MINOR_CIV_PERSONALITY_NEUTRAL');

-- Conduct Spy Mission
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_SPY_MISSION' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MERCANTILE';
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_SPY_MISSION' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_NEUTRAL';
UPDATE Helper SET Weight = 50 WHERE QuestType = 'MINOR_CIV_QUEST_SPY_MISSION' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MERCANTILE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_NEUTRAL';

-- Conquer City
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_ACQUIRE_CITY';
UPDATE Helper SET Weight = 50 WHERE QuestType = 'MINOR_CIV_QUEST_ACQUIRE_CITY' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_HOSTILE';
UPDATE Helper SET Weight = 80 WHERE QuestType = 'MINOR_CIV_QUEST_ACQUIRE_CITY' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_NEUTRAL';
UPDATE Helper SET Weight = 0 WHERE QuestType = 'MINOR_CIV_QUEST_ACQUIRE_CITY' AND MinorCivTraitType <> 'MINOR_CIV_TRAIT_MILITARISTIC';


-- GLOBAL QUESTS
-- Kill Camp
UPDATE Helper SET Weight = 20 WHERE QuestType = 'MINOR_CIV_QUEST_KILL_CAMP' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_HOSTILE';

-- Kill City-State
UPDATE Helper SET Weight = 20 WHERE QuestType = 'MINOR_CIV_QUEST_KILL_CITY_STATE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_HOSTILE';

-- Faith Contest
UPDATE Helper SET Weight = 20 WHERE QuestType = 'MINOR_CIV_QUEST_CONTEST_FAITH' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_RELIGIOUS';
UPDATE Helper SET Weight = 45 WHERE QuestType = 'MINOR_CIV_QUEST_CONTEST_FAITH' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_RELIGIOUS' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_NEUTRAL';

-- Artsy Units Contest
UPDATE Helper SET Weight = 20 WHERE QuestType = 'MINOR_CIV_QUEST_CONTEST_ARTSY_UNITS' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_CULTURED';
UPDATE Helper SET Weight = 45 WHERE QuestType = 'MINOR_CIV_QUEST_CONTEST_ARTSY_UNITS' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_CULTURED' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_NEUTRAL';

-- Sciencey Units Contest
UPDATE Helper SET Weight = 20 WHERE QuestType = 'MINOR_CIV_QUEST_CONTEST_SCIENCEY_UNITS' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MERCANTILE';
UPDATE Helper SET Weight = 45 WHERE QuestType = 'MINOR_CIV_QUEST_CONTEST_SCIENCEY_UNITS' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MARITIME' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_NEUTRAL';

-- Tech Contest
UPDATE Helper SET Weight = 20 WHERE QuestType = 'MINOR_CIV_QUEST_CONTEST_TECHS' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MILITARISTIC';

-- Influence
UPDATE Helper SET Weight = 20 WHERE QuestType = 'MINOR_CIV_QUEST_INFLUENCE' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_FRIENDLY';

-- Archaeology
UPDATE Helper SET Weight = 20 WHERE QuestType = 'MINOR_CIV_QUEST_ARCHAEOLOGY' AND MinorCivTraitType IN ('MINOR_CIV_TRAIT_CULTURED', 'MINOR_CIV_TRAIT_MILITARISTIC', 'MINOR_CIV_TRAIT_RELIGIOUS');

-- Circumnavigation
UPDATE Helper SET Weight = 20 WHERE QuestType = 'MINOR_CIV_QUEST_CIRCUMNAVIGATION' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MARITIME';
UPDATE Helper SET Weight = 45 WHERE QuestType = 'MINOR_CIV_QUEST_CIRCUMNAVIGATION' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MARITIME' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_FRIENDLY';

-- Quell Horde
UPDATE Helper SET Weight = 30 WHERE QuestType = 'MINOR_CIV_QUEST_HORDE';
UPDATE Helper SET Weight = 60 WHERE QuestType = 'MINOR_CIV_QUEST_HORDE' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MARITIME' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_HOSTILE';

-- Quell Rebellion
UPDATE Helper SET Weight = 50 WHERE QuestType = 'MINOR_CIV_QUEST_REBELLION';
UPDATE Helper SET Weight = 80 WHERE QuestType = 'MINOR_CIV_QUEST_REBELLION' AND MinorCivTraitType = 'MINOR_CIV_TRAIT_MARITIME' AND MinorCivPersonalityType = 'MINOR_CIV_PERSONALITY_HOSTILE';


-- Now move this data to the table where it is used!
-- Different quest weights by era are supported, but are currently only used to gatekeep certain quests
INSERT INTO MinorCivilizations_QuestWeights
	(MinorCivTraitType, MinorCivPersonalityType, EraType, QuestType, Weight)
SELECT
	a.MinorCivTraitType, a.MinorCivPersonalityType, b.Type, a.QuestType, a.Weight
FROM Helper a, Eras b;

DROP TABLE Helper;


-- Block contest and Influence boost quests prior to the Medieval Era
UPDATE MinorCivilizations_QuestWeights
SET Weight = 0
WHERE QuestType IN ('MINOR_CIV_QUEST_CONTEST_FAITH', 'MINOR_CIV_QUEST_CONTEST_ARTSY_UNITS', 'MINOR_CIV_QUEST_CONTEST_SCIENCEY_UNITS', 'MINOR_CIV_QUEST_CONTEST_TECHS', 'MINOR_CIV_QUEST_INFLUENCE')
AND EraType IN (SELECT Type FROM Eras WHERE ID < (SELECT ID FROM Eras WHERE Type = 'ERA_MEDIEVAL'));

-- Block the Find-City-State quest prior to the Renaissance Era
UPDATE MinorCivilizations_QuestWeights
SET Weight = 0
WHERE QuestType = 'MINOR_CIV_QUEST_FIND_CITY_STATE'
AND EraType IN (SELECT Type FROM Eras WHERE ID < (SELECT ID FROM Eras WHERE Type = 'ERA_RENAISSANCE'));

-- Block the Spread Religion quest beginning in the Industrial Era
UPDATE MinorCivilizations_QuestWeights
SET Weight = 0
WHERE QuestType = 'MINOR_CIV_QUEST_SPREAD_RELIGION'
AND EraType IN (SELECT Type FROM Eras WHERE ID >= (SELECT ID FROM Eras WHERE Type = 'ERA_INDUSTRIAL'));
