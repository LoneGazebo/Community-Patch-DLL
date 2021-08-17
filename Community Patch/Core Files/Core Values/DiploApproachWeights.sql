-- Diplo Approach Weights (affects the interactions of AI major civs with other major civs)
-- Increasing these values will not always result in that approach being adopted; the AI sometimes sets the value for an approach to 0 under certain circumstances.
-- The total value for an approach is always capped below at 0.

-- Approaches towards humans
-- Flat +/- weight
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_NEUTRAL_HUMAN', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_FRIENDLY_HUMAN', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_AFRAID_HUMAN', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_GUARDED_HUMAN', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_DECEPTIVE_HUMAN', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_HOSTILE_HUMAN', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_WAR_HUMAN', '0';

-- % modifier to approach weight (positive values increase the approach weight by this percentage; negative values decrease it; applied after all other modifiers).
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_NEUTRAL_HUMAN_PERCENT', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_FRIENDLY_HUMAN_PERCENT', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_AFRAID_HUMAN_PERCENT', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_GUARDED_HUMAN_PERCENT', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_DECEPTIVE_HUMAN_PERCENT', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_HOSTILE_HUMAN_PERCENT', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_WAR_HUMAN_PERCENT', '0';


-- Approaches towards other AI players
-- Flat +/- weight
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_NEUTRAL_AI', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_FRIENDLY_AI', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_AFRAID_AI', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_GUARDED_AI', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_DECEPTIVE_AI', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_HOSTILE_AI', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_WAR_AI', '0';

-- % modifier to approach weight (positive values increase the approach weight by this percentage; negative values decrease it; applied after all other modifiers).
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_NEUTRAL_AI_PERCENT', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_FRIENDLY_AI_PERCENT', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_AFRAID_AI_PERCENT', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_GUARDED_AI_PERCENT', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_DECEPTIVE_AI_PERCENT', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_HOSTILE_AI_PERCENT', '0';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_WAR_AI_PERCENT', '0';


-- Target Value Multipliers to Approach Weights
-- WAR and HOSTILE approach scores are multiplied by whichever of these values is appropriate, and then divided by 100.
-- NOTE: Target Value is also affected by proximity!

-- Normal Target Value Multiplier (Major Civs)
INSERT INTO Defines (Name, Value) SELECT 'MAJOR_WAR_MULTIPLIER_TARGET_SOFT', '150';
INSERT INTO Defines (Name, Value) SELECT 'MAJOR_WAR_MULTIPLIER_TARGET_FAVORABLE', '125';
INSERT INTO Defines (Name, Value) SELECT 'MAJOR_WAR_MULTIPLIER_TARGET_AVERAGE', '75';
INSERT INTO Defines (Name, Value) SELECT 'MAJOR_WAR_MULTIPLIER_TARGET_BAD', '50';
INSERT INTO Defines (Name, Value) SELECT 'MAJOR_WAR_MULTIPLIER_TARGET_IMPOSSIBLE', '25';

-- Normal Target Value Multiplier (City-States)
UPDATE Defines SET Value = '150' WHERE Name = 'MINOR_APPROACH_WAR_TARGET_SOFT';
UPDATE Defines SET Value = '125' WHERE Name = 'MINOR_APPROACH_WAR_TARGET_FAVORABLE';
UPDATE Defines SET Value = '75' WHERE Name = 'MINOR_APPROACH_WAR_TARGET_AVERAGE';
UPDATE Defines SET Value = '50' WHERE Name = 'MINOR_APPROACH_WAR_TARGET_BAD';
UPDATE Defines SET Value = '25' WHERE Name = 'MINOR_APPROACH_WAR_TARGET_IMPOSSIBLE';

-- Conquest Target Value Multiplier
-- Applied if AI really wants to conquer this player AND has a good attack target.
INSERT INTO Defines (Name, Value) SELECT 'CONQUEST_WAR_MULTIPLIER_TARGET_SOFT', '200';
INSERT INTO Defines (Name, Value) SELECT 'CONQUEST_WAR_MULTIPLIER_TARGET_FAVORABLE', '150';
INSERT INTO Defines (Name, Value) SELECT 'CONQUEST_WAR_MULTIPLIER_TARGET_AVERAGE', '125';
INSERT INTO Defines (Name, Value) SELECT 'CONQUEST_WAR_MULTIPLIER_TARGET_BAD', '100';
INSERT INTO Defines (Name, Value) SELECT 'CONQUEST_WAR_MULTIPLIER_TARGET_IMPOSSIBLE', '75';


-- Proximity Multipliers to Approach Weights
-- These are only for MAJOR CIVS!
-- Approach is multiplied by whichever of these values is appropriate, and then divided by 100.

-- Normal Proximity Multiplier
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_MULTIPLIER_PROXIMITY_NEIGHBORS', '150';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_MULTIPLIER_PROXIMITY_CLOSE', '125';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_MULTIPLIER_PROXIMITY_FAR', '75';
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_MULTIPLIER_PROXIMITY_DISTANT', '50';

-- Conquest Multiplier (proximity multiplier to WAR and HOSTILE approaches if AI really wants to conquer this player AND has a good attack target)
UPDATE Defines SET Value = '200' WHERE Name = 'APPROACH_WAR_PROXIMITY_NEIGHBORS';
UPDATE Defines SET Value = '150' WHERE Name = 'APPROACH_WAR_PROXIMITY_CLOSE';
UPDATE Defines SET Value = '100' WHERE Name = 'APPROACH_WAR_PROXIMITY_FAR';
UPDATE Defines SET Value = '75' WHERE Name = 'APPROACH_WAR_PROXIMITY_DISTANT';


-- Peace Treaty Dampener Turns
-- Dissuades the AI from declaring war for a certain amount of time after a peace treaty has been signed.
-- NOTE: For MAJOR CIVS, this amount is reduced on higher difficulties.
UPDATE Defines SET Value = '20' WHERE Name = 'TURNS_SINCE_PEACE_WEIGHT_DAMPENER';


-- C4DF Approach Values

-- Approach multipliers for having 2+ vassals
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_WAR_TOO_MANY_VASSALS', '20'; -- % increase to WAR and HOSTILE approaches per vassal
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_GUARDED_TOO_MANY_VASSALS', '20'; -- % increase to GUARDED and AFRAID approaches per vassal

-- Modifiers for refusing to give a former vassal independence
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_WAR_VASSAL_FORCEFULLY_REVOKED', '4'; -- Adds x times the AI's War Bias to the War Approach
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_FRIENDLY_VASSAL_FORCEFULLY_REVOKED', '-10'; -- Adds x times the AI's Friendly Bias to the Friendly Approach
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_DECEPTIVE_VASSAL_FORCEFULLY_REVOKED', '-10'; -- Adds x times the AI's Deceptive Bias to the Deceptive Approach

-- Modifiers for agreeing to give a former vassal independence
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_WAR_VASSAL_PEACEFULLY_REVOKED', '-4'; -- Adds x times the AI's War Bias to the War Approach
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_FRIENDLY_VASSAL_PEACEFULLY_REVOKED', '5'; -- Adds x times the AI's Friendly Bias to the Friendly Approach
INSERT INTO Defines (Name, Value) SELECT 'APPROACH_DECEPTIVE_VASSAL_PEACEFULLY_REVOKED', '2'; -- Adds x times the AI's Deceptive Bias to the Deceptive Approach (not applied if master liberates vassal)