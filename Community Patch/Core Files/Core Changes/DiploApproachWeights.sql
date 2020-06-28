-- Diplo Approach Weights (affects the interactions of AI major civs with other major civs)
-- Increasing these values will not always result in that approach being adopted; the AI sometimes sets the value for an approach to 0 under certain circumstances.
-- The total value for an approach is always capped below at 0.

-- Approaches towards humans
-- Flat +/- weight
INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_NEUTRAL_HUMAN', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_FRIENDLY_HUMAN', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_AFRAID_HUMAN', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_GUARDED_HUMAN', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_DECEPTIVE_HUMAN', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_HOSTILE_HUMAN', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_WAR_HUMAN', '0';

-- % modifier to approach weight (positive values increase the approach weight by this percentage; negative values decrease it; applied after all other modifiers).
INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_NEUTRAL_HUMAN_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_FRIENDLY_HUMAN_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_AFRAID_HUMAN_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_GUARDED_HUMAN_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_DECEPTIVE_HUMAN_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_HOSTILE_HUMAN_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_WAR_HUMAN_PERCENT', '0';


-- Approaches towards other AI players
-- Flat +/- weight
INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_NEUTRAL_AI', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_FRIENDLY_AI', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_AFRAID_AI', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_GUARDED_AI', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_DECEPTIVE_AI', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_HOSTILE_AI', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_WAR_AI', '0';

-- % modifier to approach weight (positive values increase the approach weight by this percentage; negative values decrease it; applied after all other modifiers).
INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_NEUTRAL_AI_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_FRIENDLY_AI_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_AFRAID_AI_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_GUARDED_AI_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_DECEPTIVE_AI_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_HOSTILE_AI_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_WAR_AI_PERCENT', '0';


-- Proximity Multipliers to Approach Weights
-- Approach is multiplied by whichever of these values is appropriate, and then divided by 100.

-- Normal Proximity Multiplier
INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_MULTIPLIER_PROXIMITY_NEIGHBORS', '150';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_MULTIPLIER_PROXIMITY_CLOSE', '125';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_MULTIPLIER_PROXIMITY_FAR', '100';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_MULTIPLIER_PROXIMITY_DISTANT', '75';

-- Conquest Multiplier (proximity multiplier to WAR and HOSTILE approaches if AI is going for domination or just really wants to conquer this player)
-- 115
UPDATE Defines
SET Value = '200'
WHERE Name = 'APPROACH_WAR_PROXIMITY_NEIGHBORS';

-- 100
UPDATE Defines
SET Value = '150'
WHERE Name = 'APPROACH_WAR_PROXIMITY_CLOSE';

-- 60
UPDATE Defines
SET Value = '125'
WHERE Name = 'APPROACH_WAR_PROXIMITY_FAR';

-- 50
UPDATE Defines
SET Value = '100'
WHERE Name = 'APPROACH_WAR_PROXIMITY_DISTANT';


-- Peace Treaty Dampener Turns
-- Dissuades the AI from declaring war for a certain amount of time after a peace treaty has been signed.
-- NOTE: This amount is reduced on higher difficulties.
-- 25
UPDATE Defines
SET Value = '20'
WHERE Name = 'TURNS_SINCE_PEACE_WEIGHT_DAMPENER';