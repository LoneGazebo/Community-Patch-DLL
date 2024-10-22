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

-- Culture contest
UPDATE SmallAwards SET Influence = 50, Happiness = 1, RandomMod = 10 WHERE Type = 'MINOR_CIV_QUEST_CONTEST_CULTURE';

-- Faith contest
UPDATE SmallAwards SET Influence = 30, GlobalGPPoints = 20, RandomMod = 10 WHERE Type = 'MINOR_CIV_QUEST_CONTEST_FAITH';

-- Tech contest
UPDATE SmallAwards SET Influence = 25, Tourism = 300, RandomMod = 10 WHERE Type = 'MINOR_CIV_QUEST_CONTEST_TECHS';

-- Tourism contest
UPDATE SmallAwards SET Influence = 50, Culture = 80, RandomMod = 10 WHERE Type = 'MINOR_CIV_QUEST_CONTEST_TOURISM';

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

-- Spy on specific Player
UPDATE SmallAwards SET Influence = 60, Science = 150, RandomMod = 10 WHERE Type = 'MINOR_CIV_QUEST_SPY_ON_MAJOR';

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
