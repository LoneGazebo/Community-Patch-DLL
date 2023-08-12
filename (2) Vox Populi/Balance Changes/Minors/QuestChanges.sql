-- General Data
UPDATE Defines SET Value = '20' WHERE Name = 'MINOR_CIV_CONTACT_GOLD_OTHER';
UPDATE Defines SET Value = '-3000' WHERE Name = 'MINOR_FRIENDSHIP_DROP_BULLY_GOLD_SUCCESS';
UPDATE Defines SET Value = '-6000' WHERE Name = 'MINOR_FRIENDSHIP_DROP_BULLY_WORKER_SUCCESS';
UPDATE Defines SET Value = '-5' WHERE Name = 'FRIENDSHIP_PER_UNIT_INTRUDING';
UPDATE Defines SET Value = '15' WHERE Name = 'FRIENDSHIP_PER_BARB_KILLED';
UPDATE Defines SET Value = '15' WHERE Name = 'FRIENDSHIP_PER_UNIT_GIFTED';
UPDATE Defines SET Value = '50' WHERE Name = 'RETURN_CIVILIAN_FRIENDSHIP';

-- Quest Cooldowns
UPDATE Defines SET Value = '4' WHERE Name = 'MINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN';
UPDATE Defines SET Value = '0' WHERE Name = 'MINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN_RAND';
UPDATE Defines SET Value = '25' WHERE Name = 'MINOR_CIV_GLOBAL_QUEST_MIN_TURNS_BETWEEN';
UPDATE Defines SET Value = '20' WHERE Name = 'MINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN';
UPDATE Defines SET Value = '200' WHERE Name = 'MINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER';
UPDATE Defines SET Value = '4' WHERE Name = 'MINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN';
UPDATE Defines SET Value = '0' WHERE Name = 'MINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN_RAND';
UPDATE Defines SET Value = '10' WHERE Name = 'MINOR_CIV_PERSONAL_QUEST_MIN_TURNS_BETWEEN';
UPDATE Defines SET Value = '20' WHERE Name = 'MINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN';
UPDATE Defines SET Value = '200' WHERE Name = 'MINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER';

-- Route
UPDATE SmallAwards SET Influence = '40', Production = '70', RandomMod = '10', QuestDuration = '50'  WHERE Type = 'MINOR_CIV_QUEST_ROUTE';

-- Camp
UPDATE SmallAwards SET Influence = '25', GeneralPoints = '30', RandomMod = '10' WHERE Type = 'MINOR_CIV_QUEST_KILL_CAMP';

-- Resource
UPDATE SmallAwards SET Influence = '25', Gold = '90', RandomMod = '10', QuestDuration = '50' WHERE Type = 'MINOR_CIV_QUEST_CONNECT_RESOURCE';

-- Wonder
UPDATE SmallAwards SET Influence = '30', Faith = '65', RandomMod = '10', QuestDuration = '50' WHERE Type = 'MINOR_CIV_QUEST_CONSTRUCT_WONDER';

-- GP
UPDATE SmallAwards SET Influence = '25', Culture = '65', RandomMod = '10', QuestDuration = '50' WHERE Type = 'MINOR_CIV_QUEST_GREAT_PERSON';

-- Kill CS
UPDATE Defines SET Value = '0' WHERE Name = 'QUEST_DISABLED_KILL_CITY_STATE';
UPDATE SmallAwards SET Influence = '50', Gold = '200', GoldenAgePoints = '200', RandomMod = '10', QuestDuration = '60' WHERE Type = 'MINOR_CIV_QUEST_KILL_CITY_STATE';

-- Find Player
UPDATE SmallAwards SET Influence = '30', Science = '50', RandomMod = '10', QuestDuration = '50' WHERE Type = 'MINOR_CIV_QUEST_FIND_PLAYER';

-- Find NW
UPDATE SmallAwards SET Influence = '30', GoldenAgePoints = '150', RandomMod = '10' WHERE Type = 'MINOR_CIV_QUEST_FIND_NATURAL_WONDER';

-- Give Gold
UPDATE SmallAwards SET Influence = '25', Food = '100', RandomMod = '10' WHERE Type = 'MINOR_CIV_QUEST_GIVE_GOLD';

-- PtP
UPDATE SmallAwards SET Influence = '30', CapitalGPPoints = '20', RandomMod = '10' WHERE Type = 'MINOR_CIV_QUEST_PLEDGE_TO_PROTECT';

-- Culture Contest
UPDATE SmallAwards SET Influence = '50', Happiness = '1', RandomMod = '10' WHERE Type = 'MINOR_CIV_QUEST_CONTEST_CULTURE';

-- Faith Contest
UPDATE SmallAwards SET Influence = '30', GlobalGPPoints = '20', RandomMod = '10' WHERE Type = 'MINOR_CIV_QUEST_CONTEST_FAITH';

-- Tech Contest
UPDATE SmallAwards SET Influence = '25', Tourism = '300', RandomMod = '10' WHERE Type = 'MINOR_CIV_QUEST_CONTEST_TECHS';

-- Bully
UPDATE SmallAwards SET Influence = '20', Gold = '60', RandomMod = '5' WHERE Type = 'MINOR_CIV_QUEST_BULLY_CITY_STATE';

-- Denounce
UPDATE SmallAwards SET Influence = '30', GeneralPoints = '50', RandomMod = '10' WHERE Type = 'MINOR_CIV_QUEST_DENOUNCE_MAJOR';

-- Religion
UPDATE SmallAwards SET Influence = '30', Faith = '70', RandomMod = '10', QuestDuration = '50' WHERE Type = 'MINOR_CIV_QUEST_SPREAD_RELIGION';

-- Trade Route
UPDATE SmallAwards SET Influence = '30', Food = '50', RandomMod = '10', QuestDuration = '50' WHERE Type = 'MINOR_CIV_QUEST_TRADE_ROUTE';

-- War
UPDATE SmallAwards SET Influence = '75', Happiness = '1', RandomMod = '10', QuestDuration = '50' WHERE Type = 'MINOR_CIV_QUEST_WAR';

-- Build NW
UPDATE SmallAwards SET Influence = '30', GoldenAgePoints = '125', RandomMod = '10', QuestDuration = '50' WHERE Type = 'MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER';

-- Send Unit
UPDATE SmallAwards SET Influence = '25', Gold = '60', RandomMod = '10', QuestDuration = '50' WHERE Type = 'MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT';

-- Find a CS
UPDATE SmallAwards SET Influence = '15', Production = '60', RandomMod = '10', QuestDuration = '50' WHERE Type = 'MINOR_CIV_QUEST_FIND_CITY_STATE';

-- Contest Tourism
UPDATE SmallAwards SET Influence = '50', Culture = '80', RandomMod = '10' WHERE Type = 'MINOR_CIV_QUEST_CONTEST_TOURISM';

-- Archaeology
UPDATE SmallAwards SET Influence = '25', Tourism = '200', RandomMod = '10' WHERE Type = 'MINOR_CIV_QUEST_ARCHAEOLOGY';

-- Circumnavigation
UPDATE SmallAwards SET Influence = '30', AdmiralPoints = '25', RandomMod = '10' WHERE Type = 'MINOR_CIV_QUEST_CIRCUMNAVIGATION';

-- Liberation
UPDATE SmallAwards SET Influence = '40', Happiness = '1', RandomMod = '10', QuestDuration = '50' WHERE Type = 'MINOR_CIV_QUEST_LIBERATION';

-- Discover Plot
UPDATE SmallAwards SET Influence = '30', Gold = '100', RandomMod = '10', QuestDuration = '50' WHERE Type = 'MINOR_CIV_QUEST_DISCOVER_PLOT';

-- Production
UPDATE SmallAwards SET Influence = '40', Production = '75', RandomMod = '10', QuestDuration = '50' WHERE Type = 'MINOR_CIV_QUEST_BUILD_X_BUILDINGS';

-- Stealing
UPDATE SmallAwards SET Influence = '60', Science = '150', RandomMod = '10', QuestDuration = '50'  WHERE Type = 'MINOR_CIV_QUEST_UNIT_STEAL_FROM';

-- Coup
UPDATE SmallAwards SET Influence = '40', CapitalGPPoints = '150', RandomMod = '10', QuestDuration = '50' WHERE Type = 'MINOR_CIV_QUEST_UNIT_COUP_CITY';

-- Conquest
UPDATE SmallAwards SET Influence = '80', GlobalExperience = '15', RandomMod = '5', QuestDuration = '50' WHERE Type = 'MINOR_CIV_QUEST_UNIT_GET_CITY';

-- Quest Weights
UPDATE Defines SET Value = '150' WHERE Name = 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CITY_STATE';
UPDATE Defines SET Value = '150' WHERE Name = 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_KILL_CITY_STATE';
UPDATE Defines SET Value = '33' WHERE Name = 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_NEUTRAL_KILL_CITY_STATE';