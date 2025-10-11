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
	NumCitiesUnhappinessPercent = 150,
	TradeRouteDistanceMod = 80
WHERE Type = 'WORLDSIZE_DUEL';

UPDATE Worlds
SET
	ReformationPercentRequired = 200,
	NumCitiesUnhappinessPercent = 125,
	TradeRouteDistanceMod = 80
WHERE Type = 'WORLDSIZE_TINY';

UPDATE Worlds
SET
	ReformationPercentRequired = 150,
	NumCitiesUnhappinessPercent = 115,
	TradeRouteDistanceMod = 90
WHERE Type = 'WORLDSIZE_SMALL';

UPDATE Worlds
SET
	ReformationPercentRequired = 100,
	NumCitiesUnhappinessPercent = 100,
	TradeRouteDistanceMod = 100
WHERE Type = 'WORLDSIZE_STANDARD';

UPDATE Worlds
SET
	ReformationPercentRequired = 80,
	NumCitiesUnhappinessPercent = 80,
	TradeRouteDistanceMod = 130
WHERE Type = 'WORLDSIZE_LARGE';

UPDATE Worlds
SET
	ReformationPercentRequired = 60,
	NumCitiesUnhappinessPercent = 60,
	TradeRouteDistanceMod = 160
WHERE Type = 'WORLDSIZE_HUGE';
