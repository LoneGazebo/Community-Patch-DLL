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
SET MaxActiveReligions = '2'
WHERE Type = 'WORLDSIZE_TINY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET MaxActiveReligions = '3'
WHERE Type = 'WORLDSIZE_SMALL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET MaxActiveReligions = '4'
WHERE Type = 'WORLDSIZE_STANDARD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET MaxActiveReligions = '5'
WHERE Type = 'WORLDSIZE_LARGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET MaxActiveReligions = '6'
WHERE Type = 'WORLDSIZE_HUGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

-- Update World Size Tech/Policy city rate

UPDATE Worlds
SET NumCitiesPolicyCostMod = '19'
WHERE Type = 'WORLDSIZE_DUEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesPolicyCostMod = '17'
WHERE Type = 'WORLDSIZE_TINY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesPolicyCostMod = '15'
WHERE Type = 'WORLDSIZE_SMALL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesPolicyCostMod = '13'
WHERE Type = 'WORLDSIZE_STANDARD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesPolicyCostMod = '11'
WHERE Type = 'WORLDSIZE_LARGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesPolicyCostMod = '9'
WHERE Type = 'WORLDSIZE_HUGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );


UPDATE Worlds
SET NumCitiesTechCostMod = '18'
WHERE Type = 'WORLDSIZE_DUEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesTechCostMod = '16'
WHERE Type = 'WORLDSIZE_TINY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesTechCostMod = '14'
WHERE Type = 'WORLDSIZE_SMALL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesTechCostMod = '12'
WHERE Type = 'WORLDSIZE_STANDARD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesTechCostMod = '10'
WHERE Type = 'WORLDSIZE_LARGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesTechCostMod = '8'
WHERE Type = 'WORLDSIZE_HUGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );
