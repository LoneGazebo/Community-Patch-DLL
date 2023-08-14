-- This file changes the minimum city distance for major and minor civs depending on mapsize. Change these values as desired.

-- DUEL - 3 Minimum for both

UPDATE Worlds
SET MinDistanceCities = '3'
WHERE Type = 'WORLDSIZE_DUEL';

UPDATE Worlds
SET MinDistanceCityStates = '3'
WHERE Type = 'WORLDSIZE_DUEL';

-- Tiny - 3 Minimum for Major, 2 for Minor

UPDATE Worlds
SET MinDistanceCities = '3'
WHERE Type = 'WORLDSIZE_TINY';

UPDATE Worlds
SET MinDistanceCityStates = '3'
WHERE Type = 'WORLDSIZE_TINY';

-- Small - 3 Minimum for Major, 2 for Minor

UPDATE Worlds
SET MinDistanceCities = '3'
WHERE Type = 'WORLDSIZE_SMALL';

UPDATE Worlds
SET MinDistanceCityStates = '3'
WHERE Type = 'WORLDSIZE_SMALL';

-- Standard - 3 Minimum for Major, 3 for Minor

UPDATE Worlds
SET MinDistanceCities = '3'
WHERE Type = 'WORLDSIZE_STANDARD';

UPDATE Worlds
SET MinDistanceCityStates = '3'
WHERE Type = 'WORLDSIZE_STANDARD';

-- Large - 4 Minimum for Major, 3 for Minor

UPDATE Worlds
SET MinDistanceCities = '3'
WHERE Type = 'WORLDSIZE_LARGE';

UPDATE Worlds
SET MinDistanceCityStates = '3'
WHERE Type = 'WORLDSIZE_LARGE';

-- Huge - 4 Minimum for Major, 3 for Minor

UPDATE Worlds
SET MinDistanceCities = '3'
WHERE Type = 'WORLDSIZE_HUGE';

UPDATE Worlds
SET MinDistanceCityStates = '3'
WHERE Type = 'WORLDSIZE_HUGE';

-- Default value that the values above can never exceed (minimally)

-- Min City Range
UPDATE Defines
SET Value = '3'
WHERE Name = 'MIN_CITY_RANGE';
