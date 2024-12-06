UPDATE Worlds
SET
	MinDistanceCities = 3,
	MinDistanceCityStates = 2,
	NumCitiesPolicyCostMod = 5,
	NumCitiesTourismCostMod = 5,
	NumCitiesTechCostMod = 5,
	NumCitiesUnitSupplyMod = 5,
	ResearchPercent = 100;

UPDATE Worlds
SET
	ReformationPercentRequired = 250,
	NumCitiesUnhappinessPercent = 150
WHERE Type = 'WORLDSIZE_DUEL';

UPDATE Worlds
SET
	ReformationPercentRequired = 200,
	NumCitiesUnhappinessPercent = 125
WHERE Type = 'WORLDSIZE_TINY';

UPDATE Worlds
SET
	ReformationPercentRequired = 150,
	NumCitiesUnhappinessPercent = 115
WHERE Type = 'WORLDSIZE_SMALL';

UPDATE Worlds
SET
	ReformationPercentRequired = 100,
	NumCitiesUnhappinessPercent = 100
WHERE Type = 'WORLDSIZE_STANDARD';

UPDATE Worlds
SET
	ReformationPercentRequired = 80,
	NumCitiesUnhappinessPercent = 80
WHERE Type = 'WORLDSIZE_LARGE';

UPDATE Worlds
SET
	ReformationPercentRequired = 60,
	NumCitiesUnhappinessPercent = 60
WHERE Type = 'WORLDSIZE_HUGE';
