-- Base Approach Weight (affects all AI players)
-- Adds or subtracts approach weight for human and/or AI players.
-- Approaches towards human players
INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_NEUTRAL_BASE_HUMAN', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_FRIENDLY_BASE_HUMAN', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_AFRAID_BASE_HUMAN', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_GUARDED_BASE_HUMAN', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_DECEPTIVE_BASE_HUMAN', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_HOSTILE_BASE_HUMAN', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_WAR_BASE_HUMAN', '0';

-- % settings (positive values increase the approach weight by this percentage; negative values decrease it; applied after the base weight above)
INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_NEUTRAL_BASE_HUMAN_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_FRIENDLY_BASE_HUMAN_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_AFRAID_BASE_HUMAN_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_GUARDED_BASE_HUMAN_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_DECEPTIVE_BASE_HUMAN_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_HOSTILE_BASE_HUMAN_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_WAR_BASE_HUMAN_PERCENT', '0';


-- Approaches towards AI players
INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_NEUTRAL_BASE_AI', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_FRIENDLY_BASE_AI', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_AFRAID_BASE_AI', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_GUARDED_BASE_AI', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_DECEPTIVE_BASE_AI', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_HOSTILE_BASE_AI', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_WAR_BASE_AI', '0';

-- % settings (positive values increase the approach weight by this percentage; negative values decrease it; applied after the base weight above)
INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_NEUTRAL_BASE_AI_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_FRIENDLY_BASE_AI_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_AFRAID_BASE_AI_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_GUARDED_BASE_AI_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_DECEPTIVE_BASE_AI_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_HOSTILE_BASE_AI_PERCENT', '0';

INSERT INTO Defines (Name, Value)
SELECT 'APPROACH_WAR_BASE_AI_PERCENT', '0';


-- Proximity Multipliers to Approach Weights
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
-- 25
UPDATE Defines
SET Value = '20'
WHERE Name = 'TURNS_SINCE_PEACE_WEIGHT_DAMPENER';