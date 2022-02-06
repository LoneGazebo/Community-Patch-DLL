-- This file changes the minimum city distance for major and minor civs depending on mapsize. Change these values as desired.

-- DUEL - 3 Minimum for both

UPDATE Worlds
SET MinDistanceCities = '3', MinDistanceCityStates = '2', ReformationPercentRequired = '250',
NumCitiesPolicyCostMod = '5', NumCitiesTourismCostMod = '5', NumCitiesTechCostMod = '5', ResearchPercent = '100',
NumCitiesUnhappinessPercent = '150'
WHERE Type = 'WORLDSIZE_DUEL';

-- Tiny - 3 Minimum for Major, 2 for Minor

UPDATE Worlds
SET MinDistanceCities = '3', MinDistanceCityStates = '2', ReformationPercentRequired = '200', MaxActiveReligions = '3',
NumCitiesPolicyCostMod = '5', NumCitiesTourismCostMod = '5', NumCitiesTechCostMod = '5', ResearchPercent = '100',
NumCitiesUnhappinessPercent = '125'
WHERE Type = 'WORLDSIZE_TINY';

-- Small - 3 Minimum for Major, 2 for Minor

UPDATE Worlds
SET MinDistanceCities = '3', MinDistanceCityStates = '2', ReformationPercentRequired = '150', MaxActiveReligions = '4', 
NumCitiesPolicyCostMod = '5', NumCitiesTourismCostMod = '5', NumCitiesTechCostMod = '5', ResearchPercent = '100',
NumCitiesUnhappinessPercent = '115'
WHERE Type = 'WORLDSIZE_SMALL';

-- Standard - 3 Minimum for Major, 2 for Minor

UPDATE Worlds
SET MinDistanceCities = '3', MinDistanceCityStates = '2', ReformationPercentRequired = '100', MaxActiveReligions = '5', 
NumCitiesPolicyCostMod = '5', NumCitiesTourismCostMod = '5', NumCitiesTechCostMod = '5', ResearchPercent = '100',
NumCitiesUnhappinessPercent = '100'
WHERE Type = 'WORLDSIZE_STANDARD';

-- Large - 3 Minimum for Major, 2 for Minor

UPDATE Worlds
SET MinDistanceCities = '3', MinDistanceCityStates = '2', ReformationPercentRequired = '80', MaxActiveReligions = '6', 
NumCitiesPolicyCostMod = '5', NumCitiesTourismCostMod = '5', NumCitiesTechCostMod = '5', ResearchPercent = '100',
NumCitiesUnhappinessPercent = '80'
WHERE Type = 'WORLDSIZE_LARGE';

-- Huge - 3 Minimum for Major, 2 for Minor

UPDATE Worlds
SET MinDistanceCities = '3', MinDistanceCityStates = '2', ReformationPercentRequired = '60', MaxActiveReligions = '7',
NumCitiesPolicyCostMod = '5', NumCitiesTourismCostMod = '5', NumCitiesTechCostMod = '5', ResearchPercent = '100',
NumCitiesUnhappinessPercent = '60'
WHERE Type = 'WORLDSIZE_HUGE';

-- Default value that the values above can never exceed (minimally)

-- Min City Range
UPDATE Defines
SET Value = '3'
WHERE Name = 'MIN_CITY_RANGE';
