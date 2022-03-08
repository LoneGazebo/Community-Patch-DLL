-- Median value for global average selection model. 50 = true median.
UPDATE Defines
SET Value = '55'
WHERE Name = 'BALANCE_HAPPINESS_THRESHOLD_PERCENTILE';

-- DEFINES FOR CITY HAPPINESS

-- Base unhappiness per each citizen in a city.
UPDATE Defines
SET Value = '0'
WHERE Name = 'UNHAPPINESS_PER_POPULATION';

-- Base unhappiness per each citizen in an occupied city.
UPDATE Defines
SET Value = '1.00'
WHERE Name = 'UNHAPPINESS_PER_OCCUPIED_POPULATION';

-- Flat unhappiness per founded city.
UPDATE Defines
SET Value = '0'
WHERE Name = 'UNHAPPINESS_PER_CITY';

-- Flat unhappiness per captured city.
UPDATE Defines
SET Value = '0'
WHERE Name = 'UNHAPPINESS_PER_CAPTURED_CITY';