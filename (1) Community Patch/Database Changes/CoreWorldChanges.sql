UPDATE Worlds
SET
	MinDistanceCities = 3,
	MinDistanceCityStates = 3,
	NumCitiesPolicyCostMod = 1000,
	NumCitiesTechCostMod = 500;

UPDATE Worlds
SET
	NumCitiesPolicyCostMod = 750,
	NumCitiesTechCostMod = 375
WHERE Type = 'WORLDSIZE_LARGE';

UPDATE Worlds
SET
	NumCitiesPolicyCostMod = 500,
	NumCitiesTechCostMod = 250
WHERE Type = 'WORLDSIZE_HUGE';
