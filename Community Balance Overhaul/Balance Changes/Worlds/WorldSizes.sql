-- This file changes the minimum city distance for major and minor civs depending on mapsize. Change these values as desired.

-- DUEL - 3 Minimum for both

UPDATE Worlds
SET MinDistanceCities = '3'
WHERE Type = 'WORLDSIZE_DUEL';

UPDATE Worlds
SET MinDistanceCityStates = '2'
WHERE Type = 'WORLDSIZE_DUEL';

UPDATE Worlds
SET ReformationPercentRequired = '250'
WHERE Type = 'WORLDSIZE_DUEL';

-- Tiny - 3 Minimum for Major, 2 for Minor

UPDATE Worlds
SET MinDistanceCities = '3'
WHERE Type = 'WORLDSIZE_TINY';

UPDATE Worlds
SET MinDistanceCityStates = '2'
WHERE Type = 'WORLDSIZE_TINY';

UPDATE Worlds
SET ReformationPercentRequired = '200'
WHERE Type = 'WORLDSIZE_TINY';

-- Small - 3 Minimum for Major, 2 for Minor

UPDATE Worlds
SET MinDistanceCities = '3'
WHERE Type = 'WORLDSIZE_SMALL';

UPDATE Worlds
SET MinDistanceCityStates = '2'
WHERE Type = 'WORLDSIZE_SMALL';

UPDATE Worlds
SET ReformationPercentRequired = '150'
WHERE Type = 'WORLDSIZE_SMALL';


-- Standard - 3 Minimum for Major, 2 for Minor

UPDATE Worlds
SET MinDistanceCities = '3'
WHERE Type = 'WORLDSIZE_STANDARD';

UPDATE Worlds
SET MinDistanceCityStates = '2'
WHERE Type = 'WORLDSIZE_STANDARD';

UPDATE Worlds
SET ReformationPercentRequired = '100'
WHERE Type = 'WORLDSIZE_STANDARD';


-- Large - 3 Minimum for Major, 2 for Minor

UPDATE Worlds
SET MinDistanceCities = '3'
WHERE Type = 'WORLDSIZE_LARGE';

UPDATE Worlds
SET MinDistanceCityStates = '2'
WHERE Type = 'WORLDSIZE_LARGE';

UPDATE Worlds
SET ReformationPercentRequired = '80'
WHERE Type = 'WORLDSIZE_LARGE';

-- Huge - 3 Minimum for Major, 2 for Minor

UPDATE Worlds
SET MinDistanceCities = '3'
WHERE Type = 'WORLDSIZE_HUGE';

UPDATE Worlds
SET MinDistanceCityStates = '2'
WHERE Type = 'WORLDSIZE_HUGE';

UPDATE Worlds
SET ReformationPercentRequired = '60'
WHERE Type = 'WORLDSIZE_HUGE';

-- Default value that the values above can never exceed (minimally)

-- Min City Range
UPDATE Defines
SET Value = '3'
WHERE Name = 'MIN_CITY_RANGE';

-- Update # of Max Religions (mapsize)
UPDATE Worlds
SET MaxActiveReligions = '3'
WHERE Type = 'WORLDSIZE_TINY';

UPDATE Worlds
SET MaxActiveReligions = '4'
WHERE Type = 'WORLDSIZE_SMALL';

UPDATE Worlds
SET MaxActiveReligions = '5'
WHERE Type = 'WORLDSIZE_STANDARD';

UPDATE Worlds
SET MaxActiveReligions = '6'
WHERE Type = 'WORLDSIZE_LARGE';

UPDATE Worlds
SET MaxActiveReligions = '7'
WHERE Type = 'WORLDSIZE_HUGE';

-- Update World Size Tech/Policy city rate

UPDATE Worlds
SET NumCitiesPolicyCostMod = '5'
WHERE Type = 'WORLDSIZE_DUEL';

UPDATE Worlds
SET NumCitiesPolicyCostMod = '5'
WHERE Type = 'WORLDSIZE_TINY';

UPDATE Worlds
SET NumCitiesPolicyCostMod = '5'
WHERE Type = 'WORLDSIZE_SMALL';

UPDATE Worlds
SET NumCitiesPolicyCostMod = '5'
WHERE Type = 'WORLDSIZE_STANDARD';

UPDATE Worlds
SET NumCitiesPolicyCostMod = '5'
WHERE Type = 'WORLDSIZE_LARGE';

UPDATE Worlds
SET NumCitiesPolicyCostMod = '5'
WHERE Type = 'WORLDSIZE_HUGE';


UPDATE Worlds
SET NumCitiesTourismCostMod = '5'
WHERE Type = 'WORLDSIZE_DUEL';

UPDATE Worlds
SET NumCitiesTourismCostMod = '5'
WHERE Type = 'WORLDSIZE_TINY';

UPDATE Worlds
SET NumCitiesTourismCostMod = '5'
WHERE Type = 'WORLDSIZE_SMALL';

UPDATE Worlds
SET NumCitiesTourismCostMod = '5'
WHERE Type = 'WORLDSIZE_STANDARD';

UPDATE Worlds
SET NumCitiesTourismCostMod = '5'
WHERE Type = 'WORLDSIZE_LARGE';

UPDATE Worlds
SET NumCitiesTourismCostMod = '5'
WHERE Type = 'WORLDSIZE_HUGE';


UPDATE Worlds
SET NumCitiesTechCostMod = '5'
WHERE Type = 'WORLDSIZE_DUEL';

UPDATE Worlds
SET NumCitiesTechCostMod = '5'
WHERE Type = 'WORLDSIZE_TINY';

UPDATE Worlds
SET NumCitiesTechCostMod = '5'
WHERE Type = 'WORLDSIZE_SMALL';

UPDATE Worlds
SET NumCitiesTechCostMod = '5'
WHERE Type = 'WORLDSIZE_STANDARD';

UPDATE Worlds
SET NumCitiesTechCostMod = '5'
WHERE Type = 'WORLDSIZE_LARGE';

UPDATE Worlds
SET NumCitiesTechCostMod = '5'
WHERE Type = 'WORLDSIZE_HUGE';


UPDATE Worlds
SET ResearchPercent = '100'
WHERE Type = 'WORLDSIZE_DUEL';

UPDATE Worlds
SET ResearchPercent = '100'
WHERE Type = 'WORLDSIZE_TINY';

UPDATE Worlds
SET ResearchPercent = '100'
WHERE Type = 'WORLDSIZE_SMALL';

UPDATE Worlds
SET ResearchPercent = '100'
WHERE Type = 'WORLDSIZE_STANDARD';

UPDATE Worlds
SET ResearchPercent = '100'
WHERE Type = 'WORLDSIZE_LARGE';

UPDATE Worlds
SET ResearchPercent = '100'
WHERE Type = 'WORLDSIZE_HUGE';

UPDATE Worlds
SET NumCitiesUnhappinessPercent = '150'
WHERE Type = 'WORLDSIZE_DUEL';

UPDATE Worlds
SET NumCitiesUnhappinessPercent = '125'
WHERE Type = 'WORLDSIZE_TINY';

UPDATE Worlds
SET NumCitiesUnhappinessPercent = '115'
WHERE Type = 'WORLDSIZE_SMALL';

UPDATE Worlds
SET NumCitiesUnhappinessPercent = '100'
WHERE Type = 'WORLDSIZE_STANDARD';

UPDATE Worlds
SET NumCitiesUnhappinessPercent = '80'
WHERE Type = 'WORLDSIZE_LARGE';

UPDATE Worlds
SET NumCitiesUnhappinessPercent = '60'
WHERE Type = 'WORLDSIZE_HUGE';
