UPDATE GameSpeeds
SET
	TechCostPerTurnMultiplier = 20,
	MinimumVoluntaryVassalTurns = 10,
	MinimumVassalTurns = 33,
	NumTurnsBetweenVassals = 20,
	MinimumVassalLiberateTurns = 33,
	MinimumVassalTaxTurns = 16,
	DealDuration = 30,
	InstantYieldPercent = 75,
	DifficultyBonusPercent = 75,
	SpyRatePercent = 67,
	ResearchPercent = 67,
	StartingHappiness = 0,
	GoldenAgePercent = 67
WHERE Type = 'GAMESPEED_QUICK';

UPDATE GameSpeeds
SET
	TechCostPerTurnMultiplier = 30,
	MinimumVoluntaryVassalTurns = 10,
	MinimumVassalTurns = 50,
	NumTurnsBetweenVassals = 30,
	MinimumVassalLiberateTurns = 50,
	MinimumVassalTaxTurns = 25,
	DealDuration = 50,
	InstantYieldPercent = 100,
	DifficultyBonusPercent = 100,
	SpyRatePercent = 100,
	ResearchPercent = 100,
	StartingHappiness = 0,
	GoldenAgePercent = 100
WHERE Type = 'GAMESPEED_STANDARD';

UPDATE GameSpeeds
SET
	TechCostPerTurnMultiplier = 45,
	MinimumVoluntaryVassalTurns = 15,
	MinimumVassalTurns = 75,
	NumTurnsBetweenVassals = 45,
	MinimumVassalLiberateTurns = 75,
	MinimumVassalTaxTurns = 37,
	DealDuration = 70,
	InstantYieldPercent = 150,
	DifficultyBonusPercent = 150,
	SpyRatePercent = 150,
	ResearchPercent = 150,
	StartingHappiness = 1,
	GoldenAgePercent = 150
WHERE Type = 'GAMESPEED_EPIC';

UPDATE GameSpeeds
SET
	TechCostPerTurnMultiplier = 90,
	MinimumVoluntaryVassalTurns = 30,
	MinimumVassalTurns = 150,
	NumTurnsBetweenVassals = 60,
	MinimumVassalLiberateTurns = 150,
	MinimumVassalTaxTurns = 75,
	DealDuration = 100,
	InstantYieldPercent = 200,
	DifficultyBonusPercent = 200,
	SpyRatePercent = 300,
	ResearchPercent = 300,
	StartingHappiness = 2,
	GoldenAgePercent = 300
WHERE Type = 'GAMESPEED_MARATHON';
