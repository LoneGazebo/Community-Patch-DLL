-- Civ IV Diplomacy Features

UPDATE CustomModOptions
SET Value = '1'
WHERE Name = 'DIPLOMACY_CIV4_FEATURES';

-- Insert SQL Rules Here 

ALTER TABLE GameSpeeds		ADD		ShareOpinionDuration			integer;									-- How long do we have to wait after Share Opinion rejection?
ALTER TABLE GameSpeeds		ADD		TechCostPerTurnMultiplier		float;										-- How much does each turn of research add to tech cost?
ALTER TABLE GameSpeeds		ADD		MinimumVassalTurns				integer;									-- Minimum turns of vassalage (before we can break it off)
ALTER TABLE GameSpeeds		ADD		MinimumVoluntaryVassalTurns		integer;
ALTER TABLE GameSpeeds		ADD		NumTurnsBetweenVassals			integer;									-- How many turns we must wait (after we've revoked vassalage) before they'll sign a new agreement)
ALTER TABLE Technologies	ADD		VassalageTradingAllowed			boolean;									-- Enables Vassalage (tech)
ALTER TABLE Eras			ADD		VassalageEnabled				boolean;									-- Enables Vassalage (era)
ALTER TABLE Resolutions		ADD		VassalMaintenanceGoldPercent	integer	DEFAULT	0;
ALTER TABLE Resolutions		ADD		EndAllCurrentVassals			boolean	DEFAULT	0;

---------------------------------------------------------------------------------------------
-- GameSpeedsInfos
UPDATE	GameSpeeds	SET	'ShareOpinionDuration' = 10			WHERE	Type = 'GAMESPEED_QUICK';
UPDATE	GameSpeeds	SET	'TechCostPerTurnMultiplier' = 20	WHERE	Type = 'GAMESPEED_QUICK';
UPDATE	GameSpeeds	SET	'MinimumVoluntaryVassalTurns' = 10	WHERE	Type = 'GAMESPEED_QUICK';
UPDATE	GameSpeeds	SET	'MinimumVassalTurns' = 33			WHERE	Type = 'GAMESPEED_QUICK';
UPDATE	GameSpeeds	SET	'NumTurnsBetweenVassals' = 67		WHERE	Type = 'GAMESPEED_QUICK';

UPDATE	GameSpeeds	SET	'ShareOpinionDuration' = 10			WHERE	Type = 'GAMESPEED_STANDARD';
UPDATE	GameSpeeds	SET	'TechCostPerTurnMultiplier' = 30	WHERE	Type = 'GAMESPEED_STANDARD';
UPDATE	GameSpeeds	SET	'MinimumVoluntaryVassalTurns' = 10	WHERE	Type = 'GAMESPEED_STANDARD';
UPDATE	GameSpeeds	SET	'MinimumVassalTurns' = 50			WHERE	Type = 'GAMESPEED_STANDARD';
UPDATE	GameSpeeds	SET	'NumTurnsBetweenVassals' = 100		WHERE	Type = 'GAMESPEED_STANDARD';

UPDATE	GameSpeeds	SET	'ShareOpinionDuration' = 15			WHERE	Type = 'GAMESPEED_EPIC';
UPDATE	GameSpeeds	SET	'TechCostPerTurnMultiplier' = 45	WHERE	Type = 'GAMESPEED_EPIC';
UPDATE	GameSpeeds	SET	'MinimumVoluntaryVassalTurns' = 15	WHERE	Type = 'GAMESPEED_EPIC';
UPDATE	GameSpeeds	SET	'MinimumVassalTurns' = 75			WHERE	Type = 'GAMESPEED_EPIC';
UPDATE	GameSpeeds	SET	'NumTurnsBetweenVassals' = 150		WHERE	Type = 'GAMESPEED_EPIC';

UPDATE	GameSpeeds	SET	'ShareOpinionDuration' = 30			WHERE	Type = 'GAMESPEED_MARATHON';
UPDATE	GameSpeeds	SET	'TechCostPerTurnMultiplier' = 90	WHERE	Type = 'GAMESPEED_MARATHON';
UPDATE	GameSpeeds	SET	'MinimumVoluntaryVassalTurns' = 30	WHERE	Type = 'GAMESPEED_MARATHON';
UPDATE	GameSpeeds	SET	'MinimumVassalTurns' = 150			WHERE	Type = 'GAMESPEED_MARATHON';
UPDATE	GameSpeeds	SET	'NumTurnsBetweenVassals' = 300		WHERE	Type = 'GAMESPEED_MARATHON';

---------------------------------------------------------------------------------------------
-- Technologies

--UPDATE	Technologies	SET	'VassalageTradingAllowed' = 1	WHERE	Type = 'TECH_CHIVALRY';
UPDATE	Technologies	SET	'MapTrading' = 1				WHERE	Type = 'TECH_MILITARY_SCIENCE';				-- Enables World Map Trading
UPDATE	Technologies	SET	'TechTrading' = 1				WHERE	Type = 'TECH_REPLACEABLE_PARTS';				-- Enables Technology Trading

---------------------------------------------------------------------------------------------
-- Eras
UPDATE	Eras	SET	'VassalageEnabled' = 1	WHERE	Type = 'ERA_MEDIEVAL';