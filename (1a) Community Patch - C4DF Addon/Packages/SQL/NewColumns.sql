-- Civ IV Diplomacy Features

UPDATE CustomModOptions
SET Value = '1'
WHERE Name = 'DIPLOMACY_CIV4_FEATURES';

-- Insert SQL Rules Here 
---------------------------------------------------------------------------------------------
-- GameSpeedsInfos

UPDATE	GameSpeeds	SET	'ShareOpinionDuration'			= 10	WHERE Type = 'GAMESPEED_QUICK';
UPDATE	GameSpeeds	SET	'TechCostPerTurnMultiplier'		= 20	WHERE Type = 'GAMESPEED_QUICK';
UPDATE	GameSpeeds	SET	'MinimumVoluntaryVassalTurns'	= 10	WHERE Type = 'GAMESPEED_QUICK';
UPDATE	GameSpeeds	SET	'MinimumVassalTurns'			= 33	WHERE Type = 'GAMESPEED_QUICK';
UPDATE	GameSpeeds	SET	'NumTurnsBetweenVassals'		= 67	WHERE Type = 'GAMESPEED_QUICK';
UPDATE	GameSpeeds	SET	'MinimumVassalLiberateTurns'	= 33	WHERE Type = 'GAMESPEED_QUICK';
UPDATE	GameSpeeds	SET	'MinimumVassalTaxTurns'			= 16	WHERE Type = 'GAMESPEED_QUICK';


UPDATE	GameSpeeds	SET	'ShareOpinionDuration'			= 10	WHERE Type = 'GAMESPEED_STANDARD';
UPDATE	GameSpeeds	SET	'TechCostPerTurnMultiplier'		= 30	WHERE Type = 'GAMESPEED_STANDARD';
UPDATE	GameSpeeds	SET	'MinimumVoluntaryVassalTurns'	= 10	WHERE Type = 'GAMESPEED_STANDARD';
UPDATE	GameSpeeds	SET	'MinimumVassalTurns'			= 50	WHERE Type = 'GAMESPEED_STANDARD';
UPDATE	GameSpeeds	SET	'NumTurnsBetweenVassals'		= 100	WHERE Type = 'GAMESPEED_STANDARD';
UPDATE	GameSpeeds	SET	'MinimumVassalLiberateTurns'	= 50	WHERE Type = 'GAMESPEED_STANDARD';
UPDATE	GameSpeeds	SET	'MinimumVassalTaxTurns'			= 25	WHERE Type = 'GAMESPEED_STANDARD';

UPDATE	GameSpeeds	SET	'ShareOpinionDuration'			= 15	WHERE Type = 'GAMESPEED_EPIC';
UPDATE	GameSpeeds	SET	'TechCostPerTurnMultiplier'		= 45	WHERE Type = 'GAMESPEED_EPIC';
UPDATE	GameSpeeds	SET	'MinimumVoluntaryVassalTurns'	= 15	WHERE Type = 'GAMESPEED_EPIC';
UPDATE	GameSpeeds	SET	'MinimumVassalTurns'			= 75	WHERE Type = 'GAMESPEED_EPIC';
UPDATE	GameSpeeds	SET	'NumTurnsBetweenVassals'		= 150	WHERE Type = 'GAMESPEED_EPIC';
UPDATE	GameSpeeds	SET	'MinimumVassalLiberateTurns'	= 75	WHERE Type = 'GAMESPEED_EPIC';
UPDATE	GameSpeeds	SET	'MinimumVassalTaxTurns'			= 37	WHERE Type = 'GAMESPEED_EPIC';

UPDATE	GameSpeeds	SET	'ShareOpinionDuration'			= 30	WHERE Type = 'GAMESPEED_MARATHON';
UPDATE	GameSpeeds	SET	'TechCostPerTurnMultiplier'		= 90	WHERE Type = 'GAMESPEED_MARATHON';
UPDATE	GameSpeeds	SET	'MinimumVoluntaryVassalTurns'	= 30	WHERE Type = 'GAMESPEED_MARATHON';
UPDATE	GameSpeeds	SET	'MinimumVassalTurns'			= 150	WHERE Type = 'GAMESPEED_MARATHON';
UPDATE	GameSpeeds	SET	'NumTurnsBetweenVassals'		= 300	WHERE Type = 'GAMESPEED_MARATHON';
UPDATE	GameSpeeds	SET	'MinimumVassalLiberateTurns'	= 150	WHERE Type = 'GAMESPEED_MARATHON';
UPDATE	GameSpeeds	SET	'MinimumVassalTaxTurns'			= 75	WHERE Type = 'GAMESPEED_MARATHON';

---------------------------------------------------------------------------------------------
-- Technologies

UPDATE	Technologies	SET	'MapTrading'				= 1		WHERE Type = 'TECH_MILITARY_SCIENCE';				-- Enables World Map Trading
UPDATE	Technologies	SET	'TechTrading'				= 1		WHERE Type = 'TECH_SCIENTIFIC_THEORY';				-- Enables Technology Trading

---------------------------------------------------------------------------------------------
-- Eras
UPDATE	Eras	SET	'VassalageEnabled'					= 1		WHERE	Type = 'ERA_MEDIEVAL';

-- Taken from CBP Compatibilty files to enable levies
UPDATE Buildings
SET VassalLevyEra = '1'
WHERE Type = 'BUILDING_PALACE';