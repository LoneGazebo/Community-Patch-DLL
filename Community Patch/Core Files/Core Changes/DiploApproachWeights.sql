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