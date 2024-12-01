-- Direct +/- and % weights to approach scores have been moved to CoreDifficultyChanges.xml (Community Patch) and DifficultyChanges.xml (Vox Populi).

-- Target Value Multipliers to Approach Weights
-- WAR and HOSTILE approach scores are multiplied by whichever of these values is appropriate, and then divided by 100.
-- NOTE: Target Value is also affected by proximity!

REPLACE INTO Defines
	(Name, Value)
VALUES
-- Conquest Target Value Multiplier
-- Applied instead of the below values if the AI really wants to conquer this player AND has a good attack target.
	('CONQUEST_WAR_MULTIPLIER_TARGET_CAKEWALK', 250),
	('CONQUEST_WAR_MULTIPLIER_TARGET_SOFT', 200),
	('CONQUEST_WAR_MULTIPLIER_TARGET_FAVORABLE', 150),
	('CONQUEST_WAR_MULTIPLIER_TARGET_AVERAGE', 125),
	('CONQUEST_WAR_MULTIPLIER_TARGET_DIFFICULT', 100),
	('CONQUEST_WAR_MULTIPLIER_TARGET_BAD', 75),
	('CONQUEST_WAR_MULTIPLIER_TARGET_IMPOSSIBLE', 50),

-- Normal Target Value Multiplier (Major Civs)
	('MAJOR_WAR_MULTIPLIER_TARGET_CAKEWALK', 200),
	('MAJOR_WAR_MULTIPLIER_TARGET_SOFT', 150),
	('MAJOR_WAR_MULTIPLIER_TARGET_FAVORABLE', 125),
	('MAJOR_WAR_MULTIPLIER_TARGET_AVERAGE', 100),
	('MAJOR_WAR_MULTIPLIER_TARGET_DIFFICULT', 75),
	('MAJOR_WAR_MULTIPLIER_TARGET_BAD', 50),
	('MAJOR_WAR_MULTIPLIER_TARGET_IMPOSSIBLE', 25),

-- Normal Target Value Multiplier (City-States)
	('MINOR_APPROACH_WAR_TARGET_CAKEWALK', 150),
	('MINOR_APPROACH_WAR_TARGET_SOFT', 125),
	('MINOR_APPROACH_WAR_TARGET_FAVORABLE', 100),
	('MINOR_APPROACH_WAR_TARGET_AVERAGE', 75),
	('MINOR_APPROACH_WAR_TARGET_DIFFICULT', 50),
	('MINOR_APPROACH_WAR_TARGET_BAD', 33),
	('MINOR_APPROACH_WAR_TARGET_IMPOSSIBLE', 25),

-- Proximity Multipliers to Approach Weights
-- These are only for MAJOR CIVS!
-- Approach is multiplied by whichever of these values is appropriate, and then divided by 100.

-- Normal Proximity Multiplier
	('APPROACH_MULTIPLIER_PROXIMITY_NEIGHBORS', 150),
	('APPROACH_MULTIPLIER_PROXIMITY_CLOSE', 125),
	('APPROACH_MULTIPLIER_PROXIMITY_FAR', 75),
	('APPROACH_MULTIPLIER_PROXIMITY_DISTANT', 50),

-- Conquest Multiplier (proximity multiplier to WAR and HOSTILE approaches if AI really wants to conquer this player AND has a good attack target)
	('APPROACH_WAR_PROXIMITY_NEIGHBORS', 200),
	('APPROACH_WAR_PROXIMITY_CLOSE', 150),
	('APPROACH_WAR_PROXIMITY_FAR', 100),
	('APPROACH_WAR_PROXIMITY_DISTANT', 75),

-- Neutral Default Weight
-- Adds x times the AI's Neutral Bias to the Neutral approach
	('APPROACH_NEUTRAL_DEFAULT', 3),

-- Bias for Current Approach
-- Adds x times the bias for the AI's current approach to that approach's score
-- Used to make the AI less likely to switch approaches randomly. Not applied during a reevaluation!
	('APPROACH_BIAS_FOR_CURRENT', 2),

-- Current War Bias
-- Adds x times the AI's War Bias to the War approach if the AI's current approach is War
-- Added on top of the bias for current approach above. Not applied during a reevaluation!
	('APPROACH_WAR_CURRENTLY_WAR', 4),

-- Ignore City-State Default Weight
-- Adds x times the AI's Ignore (Neutral) bias to the Ignore (Neutral) approach
	('MINOR_APPROACH_IGNORE_DEFAULT', 2),

-- Approach Shift Rate
-- The rate at which AI approach scores shift from turn to turn. Valid values are between 0.01 and 1.00, default value is 0.30. Higher values make the AI more responsive to changing circumstances, while lower values make AI behavior more consistent.
-- It should be noted that regardless of the value here, there are many triggers (such as a war declaration) which cause the AI to instantly reevaluate its current approach scores, assigning those as the new ones immediately.
	('APPROACH_SHIFT_PERCENT', 0.30),

-- Approach Randomness
-- If set to a non-zero value, multiplies the weight for each *major civ* approach by (100 +/- n%), where n is a random number between 0 and APPROACH_RANDOM_PERCENT, then divides by 100.
-- Default value is 5. Valid values are between 0 and 100. Higher values make the AI more erratic.
-- The APPROACH_RANDOM_PERSONALITIES_PERCENT value is used instead if the Random Personalities game option is active.
	('APPROACH_RANDOM_PERCENT', 5),
	('APPROACH_RANDOM_PERSONALITIES_PERCENT', 10),

-- C4DF Approach Values
-- Approach multipliers for having 2+ vassals
	('APPROACH_WAR_TOO_MANY_VASSALS', 20), -- % increase to WAR and HOSTILE approaches per vassal
	('APPROACH_GUARDED_TOO_MANY_VASSALS', 20), -- % increase to GUARDED and AFRAID approaches per vassal

-- Modifiers for refusing to give a former vassal independence
	('APPROACH_WAR_VASSAL_FORCEFULLY_REVOKED', 4), -- Adds x times the AI's War Bias to the War Approach
	('APPROACH_FRIENDLY_VASSAL_FORCEFULLY_REVOKED', -10), -- Adds x times the AI's Friendly Bias to the Friendly Approach
	('APPROACH_DECEPTIVE_VASSAL_FORCEFULLY_REVOKED', -10), -- Adds x times the AI's Deceptive Bias to the Deceptive Approach

-- Modifiers for agreeing to give a former vassal independence
	('APPROACH_WAR_VASSAL_PEACEFULLY_REVOKED', -4), -- Adds x times the AI's War Bias to the War Approach
	('APPROACH_FRIENDLY_VASSAL_PEACEFULLY_REVOKED', 5), -- Adds x times the AI's Friendly Bias to the Friendly Approach
	('APPROACH_DECEPTIVE_VASSAL_PEACEFULLY_REVOKED', 2), -- Adds x times the AI's Deceptive Bias to the Deceptive Approach (not applied if master liberates vassal)

-- Approach Trade Value Modifiers
-- Change the AI's trade prices based on their surface approach towards another player.
-- WAR is possible as a surface approach, but the trade modifier for peace deals is always 100 for sanity, so there's no define for it.

-- The AI is buying from the other player
	('APPROACH_HOSTILE_BUYING_PRICE_MODIFIER', 50),
	('APPROACH_GUARDED_BUYING_PRICE_MODIFIER', 80),
	('APPROACH_AFRAID_BUYING_PRICE_MODIFIER', 125),
	('APPROACH_NEUTRAL_BUYING_PRICE_MODIFIER', 100),
	('APPROACH_FRIENDLY_BUYING_PRICE_MODIFIER', 125),

-- The AI is selling to the other player
	('APPROACH_HOSTILE_SELLING_PRICE_MODIFIER', 200),
	('APPROACH_GUARDED_SELLING_PRICE_MODIFIER', 125),
	('APPROACH_AFRAID_SELLING_PRICE_MODIFIER', 80),
	('APPROACH_NEUTRAL_SELLING_PRICE_MODIFIER', 100),
	('APPROACH_FRIENDLY_SELLING_PRICE_MODIFIER', 80),

-- "Close To Victory" Thresholds
-- A player who has completed x% of the progress towards a specific victory condition is considered "close to" that victory.
-- Affects AI calculations and (if Endgame Aggression isn't disabled for that player) triggers the Endgame Aggression boost to approach.
	('CLOSE_TO_DOMINATION_VICTORY_THRESHOLD', 50),
	('CLOSE_TO_DIPLOMATIC_VICTORY_THRESHOLD', 55),
	('CLOSE_TO_SCIENCE_VICTORY_THRESHOLD', 80),
	('CLOSE_TO_CULTURE_VICTORY_THRESHOLD', 60);
