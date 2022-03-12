-- Diplo Approach Weights (affects the interactions of AI major civs with other major civs)
-- Increasing these values will not always result in that approach being adopted; the AI sometimes sets the value for an approach to 0 under certain circumstances.
-- The total value for an approach is always capped below at 0.

-- Approaches towards humans
-- Flat +/- weight
INSERT INTO Defines (Name, Value) VALUES
('APPROACH_FRIENDLY_HUMAN', '0'),
('APPROACH_NEUTRAL_HUMAN', '0'),
('APPROACH_AFRAID_HUMAN', '0'),
('APPROACH_GUARDED_HUMAN', '0'),
('APPROACH_DECEPTIVE_HUMAN', '0'),
('APPROACH_HOSTILE_HUMAN', '0'),
('APPROACH_WAR_HUMAN', '0'),

-- % modifier to approach weight (positive values increase the approach weight by this percentage; negative values decrease it; applied after all other modifiers).
('APPROACH_FRIENDLY_HUMAN_PERCENT', '0'),
('APPROACH_NEUTRAL_HUMAN_PERCENT', '0'),
('APPROACH_AFRAID_HUMAN_PERCENT', '0'),
('APPROACH_GUARDED_HUMAN_PERCENT', '0'),
('APPROACH_DECEPTIVE_HUMAN_PERCENT', '0'),
('APPROACH_HOSTILE_HUMAN_PERCENT', '0'),
('APPROACH_WAR_HUMAN_PERCENT', '0'),


-- Approaches towards other AI players
-- Flat +/- weight
('APPROACH_FRIENDLY_AI', '0'),
('APPROACH_NEUTRAL_AI', '0'),
('APPROACH_AFRAID_AI', '0'),
('APPROACH_GUARDED_AI', '0'),
('APPROACH_DECEPTIVE_AI', '0'),
('APPROACH_HOSTILE_AI', '0'),
('APPROACH_WAR_AI', '0'),

-- % modifier to approach weight (positive values increase the approach weight by this percentage; negative values decrease it; applied after all other modifiers).
('APPROACH_FRIENDLY_AI_PERCENT', '0'),
('APPROACH_NEUTRAL_AI_PERCENT', '0'),
('APPROACH_AFRAID_AI_PERCENT', '0'),
('APPROACH_GUARDED_AI_PERCENT', '0'),
('APPROACH_DECEPTIVE_AI_PERCENT', '0'),
('APPROACH_HOSTILE_AI_PERCENT', '0'),
('APPROACH_WAR_AI_PERCENT', '0'),


-- Target Value Multipliers to Approach Weights
-- WAR and HOSTILE approach scores are multiplied by whichever of these values is appropriate, and then divided by 100.
-- NOTE: Target Value is also affected by proximity!

-- Normal Target Value Multiplier (Major Civs)
('MAJOR_WAR_MULTIPLIER_TARGET_SOFT', '150'),
('MAJOR_WAR_MULTIPLIER_TARGET_FAVORABLE', '125'),
('MAJOR_WAR_MULTIPLIER_TARGET_AVERAGE', '75'),
('MAJOR_WAR_MULTIPLIER_TARGET_BAD', '50'),
('MAJOR_WAR_MULTIPLIER_TARGET_IMPOSSIBLE', '25');

-- Normal Target Value Multiplier (City-States)
UPDATE Defines SET Value = '150' WHERE Name = 'MINOR_APPROACH_WAR_TARGET_SOFT';
UPDATE Defines SET Value = '125' WHERE Name = 'MINOR_APPROACH_WAR_TARGET_FAVORABLE';
UPDATE Defines SET Value = '75' WHERE Name = 'MINOR_APPROACH_WAR_TARGET_AVERAGE';
UPDATE Defines SET Value = '50' WHERE Name = 'MINOR_APPROACH_WAR_TARGET_BAD';
UPDATE Defines SET Value = '25' WHERE Name = 'MINOR_APPROACH_WAR_TARGET_IMPOSSIBLE';

-- Conquest Target Value Multiplier
-- Applied if AI really wants to conquer this player AND has a good attack target.
INSERT INTO Defines (Name, Value) VALUES
('CONQUEST_WAR_MULTIPLIER_TARGET_SOFT', '200'),
('CONQUEST_WAR_MULTIPLIER_TARGET_FAVORABLE', '150'),
('CONQUEST_WAR_MULTIPLIER_TARGET_AVERAGE', '125'),
('CONQUEST_WAR_MULTIPLIER_TARGET_BAD', '100'),
('CONQUEST_WAR_MULTIPLIER_TARGET_IMPOSSIBLE', '75');


-- Proximity Multipliers to Approach Weights
-- These are only for MAJOR CIVS!
-- Approach is multiplied by whichever of these values is appropriate, and then divided by 100.

-- Normal Proximity Multiplier
INSERT INTO Defines (Name, Value) VALUES
('APPROACH_MULTIPLIER_PROXIMITY_NEIGHBORS', '150'),
('APPROACH_MULTIPLIER_PROXIMITY_CLOSE', '125'),
('APPROACH_MULTIPLIER_PROXIMITY_FAR', '75'),
('APPROACH_MULTIPLIER_PROXIMITY_DISTANT', '50');

-- Conquest Multiplier (proximity multiplier to WAR and HOSTILE approaches if AI really wants to conquer this player AND has a good attack target)
UPDATE Defines SET Value = '200' WHERE Name = 'APPROACH_WAR_PROXIMITY_NEIGHBORS';
UPDATE Defines SET Value = '150' WHERE Name = 'APPROACH_WAR_PROXIMITY_CLOSE';
UPDATE Defines SET Value = '100' WHERE Name = 'APPROACH_WAR_PROXIMITY_FAR';
UPDATE Defines SET Value = '75' WHERE Name = 'APPROACH_WAR_PROXIMITY_DISTANT';


-- Neutral Default Weight
-- Adds x times the AI's Neutral Bias to the Neutral approach
UPDATE Defines SET Value = '3' WHERE Name = 'APPROACH_NEUTRAL_DEFAULT';

-- Bias for Current Approach
-- Adds x times the bias for the AI's current approach to that approach's score
-- Used to make the AI less likely to switch approaches randomly. Not applied during a reevaluation!
UPDATE Defines SET Value = '2' WHERE Name = 'APPROACH_BIAS_FOR_CURRENT';

-- Current War Bias
-- Adds x times the AI's War Bias to the War approach if the AI's current approach is War
-- Added on top of the bias for current approach above. Not applied during a reevaluation!
UPDATE Defines SET Value = '4' WHERE Name = 'APPROACH_WAR_CURRENTLY_WAR';

-- Ignore City-State Default Weight
-- Adds x times the AI's Ignore (Neutral) bias to the Ignore (Neutral) approach
UPDATE Defines SET Value = '2' WHERE Name = 'MINOR_APPROACH_IGNORE_DEFAULT';


-- Peace Treaty Dampener Turns
-- Dissuades the AI from declaring war for a certain amount of time after a peace treaty has been signed.
-- NOTE: For MAJOR CIVS, this amount is reduced on higher difficulties.
UPDATE Defines SET Value = '20' WHERE Name = 'TURNS_SINCE_PEACE_WEIGHT_DAMPENER';


-- Approach Randomness
-- If set to a non-zero value, multiplies the weight for each *major civ* approach by (100 +/- n%), where n is a random number between 0 and APPROACH_RANDOM_PERCENT, then divides by 100.
-- Default value is 5. Valid values are between 0 and 100. Higher values make the AI more erratic.
-- This value is doubled if the Random Personalities game option is active.
UPDATE Defines SET Value = '5' WHERE Name = 'APPROACH_RANDOM_PERCENT';


-- C4DF Approach Values
INSERT INTO Defines (Name, Value) VALUES

-- Approach multipliers for having 2+ vassals
('APPROACH_WAR_TOO_MANY_VASSALS', '20'), -- % increase to WAR and HOSTILE approaches per vassal
('APPROACH_GUARDED_TOO_MANY_VASSALS', '20'), -- % increase to GUARDED and AFRAID approaches per vassal

-- Modifiers for refusing to give a former vassal independence
('APPROACH_WAR_VASSAL_FORCEFULLY_REVOKED', '4'), -- Adds x times the AI's War Bias to the War Approach
('APPROACH_FRIENDLY_VASSAL_FORCEFULLY_REVOKED', '-10'), -- Adds x times the AI's Friendly Bias to the Friendly Approach
('APPROACH_DECEPTIVE_VASSAL_FORCEFULLY_REVOKED', '-10'), -- Adds x times the AI's Deceptive Bias to the Deceptive Approach

-- Modifiers for agreeing to give a former vassal independence
('APPROACH_WAR_VASSAL_PEACEFULLY_REVOKED', '-4'), -- Adds x times the AI's War Bias to the War Approach
('APPROACH_FRIENDLY_VASSAL_PEACEFULLY_REVOKED', '5'), -- Adds x times the AI's Friendly Bias to the Friendly Approach
('APPROACH_DECEPTIVE_VASSAL_PEACEFULLY_REVOKED', '2'); -- Adds x times the AI's Deceptive Bias to the Deceptive Approach (not applied if master liberates vassal)


-- "Close To Victory" Thresholds
-- A player who has completed x% of the progress towards a specific victory condition is considered "close to" that victory.
-- Affects AI calculations and (if Endgame Aggression isn't disabled for that player) triggers the Endgame Aggression boost to approach.
INSERT INTO Defines (Name, Value) VALUES
('CLOSE_TO_DOMINATION_VICTORY_THRESHOLD', '50'),
('CLOSE_TO_DIPLOMATIC_VICTORY_THRESHOLD', '75'),
('CLOSE_TO_SCIENCE_VICTORY_THRESHOLD', '80'),
('CLOSE_TO_CULTURE_VICTORY_THRESHOLD', '75');