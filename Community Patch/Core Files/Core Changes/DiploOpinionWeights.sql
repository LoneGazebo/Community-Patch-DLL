-- Diplo Opinion Weights
-- All opinion weights are listed here, even the unmodified ones, for easier modding.

-- Opinion Thresholds
-- Defines how much positive or negative Opinion is required for a specific opinion rating.
-- 80
UPDATE Defines
SET Value = '160'
WHERE Name = 'OPINION_THRESHOLD_UNFORGIVABLE';

-- 40
UPDATE Defines
SET Value = '80'
WHERE Name = 'OPINION_THRESHOLD_ENEMY';

-- 15
UPDATE Defines
SET Value = '30'
WHERE Name = 'OPINION_THRESHOLD_COMPETITOR';

-- -15
UPDATE Defines
SET Value = '-30'
WHERE Name = 'OPINION_THRESHOLD_FAVORABLE';

-- -40
UPDATE Defines
SET Value = '-80'
WHERE Name = 'OPINION_THRESHOLD_FRIEND';

-- -80
UPDATE Defines
SET Value = '-160'
WHERE Name = 'OPINION_THRESHOLD_ALLY';

-- Base Opinion Weight (affects all AI players)
-- Can be used to add an opinion bonus or penalty to human and/or AI players.
-- Negative values add good opinion, positive values add bad opinion.
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_BASE_HUMAN', '0';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_BASE_AI', '0';

-- 30
UPDATE Defines
SET Value = '40'
WHERE Name = 'OPINION_WEIGHT_LAND_FIERCE';

-- 20
UPDATE Defines
SET Value = '25'
WHERE Name = 'OPINION_WEIGHT_LAND_STRONG';

-- 10
UPDATE Defines
SET Value = '15'
WHERE Name = 'OPINION_WEIGHT_LAND_WEAK';

-- -6
UPDATE Defines
SET Value = '-10'
WHERE Name = 'OPINION_WEIGHT_LAND_NONE';

-- Mods for Land Dispute Weight
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LAND_ANCIENT_ERA', '10';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LAND_CLASSICAL_ERA', '5';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LAND_NONE_EARLY_GAME', '-5';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LAND_WARMONGER', '10';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LAND_NONE_WARMONGER', '-5';

-- 20 (unchanged)
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_WONDER_FIERCE';

-- 15 (unchanged)
UPDATE Defines
SET Value = '15'
WHERE Name = 'OPINION_WEIGHT_WONDER_STRONG';

-- 10 (unchanged)
UPDATE Defines
SET Value = '10'
WHERE Name = 'OPINION_WEIGHT_WONDER_WEAK';

-- 0 (unchanged)
UPDATE Defines
SET Value = '0'
WHERE Name = 'OPINION_WEIGHT_WONDER_NONE';

-- Mods for Wonder Dispute Weight
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_WONDER_CULTURAL', '10';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_WONDER_NONE_CULTURAL', '-10';

-- 30 (unchanged)
UPDATE Defines
SET Value = '30'
WHERE Name = 'OPINION_WEIGHT_MINOR_CIV_FIERCE';

-- 20 (unchanged)
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_MINOR_CIV_STRONG';

-- 10 (unchanged)
UPDATE Defines
SET Value = '10'
WHERE Name = 'OPINION_WEIGHT_MINOR_CIV_WEAK';

-- 0 (unchanged)
UPDATE Defines
SET Value = '0'
WHERE Name = 'OPINION_WEIGHT_MINOR_CIV_NONE';

-- Mods for Minor Civ Dispute Weight
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_MINOR_CIV_DIPLOMAT', '10';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_MINOR_CIV_NONE_DIPLOMAT', '-10';

-- Tech Dispute Weight (for scientific civs)
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_TECH_FIERCE', '30';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_TECH_STRONG', '20';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_TECH_WEAK', '10';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_TECH_NONE', '-10';

-- 30
UPDATE Defines
SET Value = '40'
WHERE Name = 'OPINION_WEIGHT_VICTORY_FIERCE';

-- 20
UPDATE Defines
SET Value = '30'
WHERE Name = 'OPINION_WEIGHT_VICTORY_STRONG';

-- 10
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_VICTORY_WEAK';

-- -6
UPDATE Defines
SET Value = '0'
WHERE Name = 'OPINION_WEIGHT_VICTORY_NONE';

-- Per-era increase to victory dispute
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_VICTORY_PER_ERA', '3';

-- Victory Block Opinion Weights
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_VICTORY_BLOCK_FIERCE', '30';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_VICTORY_BLOCK_STRONG', '20';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_VICTORY_BLOCK_WEAK', '10';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_VICTORY_BLOCK_NONE', '0';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_VICTORY_BLOCK_PER_ERA', '3';

-- Endgame Victory Opinion Weight Increase
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_VICTORY_ENDGAME_MULTIPLIER', '2';

-- -20
UPDATE Defines
SET Value = '-10'
WHERE Name = 'OPINION_WEIGHT_RETURNED_CIVILIAN';

-- -20 (unchanged)
UPDATE Defines
SET Value = '-20'
WHERE Name = 'OPINION_WEIGHT_BUILT_LANDMARK';

-- -200 (unchanged)
UPDATE Defines
SET Value = '-200'
WHERE Name = 'OPINION_WEIGHT_RESURRECTED';

-- -80 (unchanged)
UPDATE Defines
SET Value = '-80'
WHERE Name = 'OPINION_WEIGHT_LIBERATED_CAPITAL';

-- -60 (unchanged)
UPDATE Defines
SET Value = '-60'
WHERE Name = 'OPINION_WEIGHT_LIBERATED_THREE_CITIES';

-- -50 (unchanged)
UPDATE Defines
SET Value = '-50'
WHERE Name = 'OPINION_WEIGHT_LIBERATED_TWO_CITIES';

-- -30 (unchanged)
UPDATE Defines
SET Value = '-30'
WHERE Name = 'OPINION_WEIGHT_LIBERATED_ONE_CITY';

-- Embassy Opinion Weights
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_EMBASSY_MUTUAL', '-3';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_EMBASSY_THEM', '-1';

-- -1
UPDATE Defines
SET Value = '-2'
WHERE Name = 'OPINION_WEIGHT_EMBASSY';

-- -10 (unchanged)
UPDATE Defines
SET Value = '-10'
WHERE Name = 'OPINION_WEIGHT_FORGAVE_FOR_SPYING';

-- 20 (unchanged)
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_ASKED_NO_SETTLE';

-- 10 (unchanged)
UPDATE Defines
SET Value = '10'
WHERE Name = 'OPINION_WEIGHT_ASKED_STOP_SPYING';

-- 20
UPDATE Defines
SET Value = '10'
WHERE Name = 'OPINION_WEIGHT_MADE_DEMAND_OF_US';

-- 30 (unchanged)
UPDATE Defines
SET Value = '30'
WHERE Name = 'OPINION_WEIGHT_CULTURE_BOMBED';

-- 5 (unchanged)
-- Religious conversion points are reduced to (at minimum) this number minus 1 when player makes a promise to stop converting AI's cities.
UPDATE Defines
SET Value = '5'
WHERE Name = 'RELIGION_DIPLO_HIT_THRESHOLD';


-- Religion Diplo Values
-- These values are multiplied by the current game era's Diplo Emphasis for Religion, except the World Religion Modifier.

-- 2 (unchanged)
UPDATE Defines
SET Value = '2'
WHERE Name = 'OPINION_WEIGHT_PER_NEGATIVE_CONVERSION';

-- -5
UPDATE Defines
SET Value = '-4'
WHERE Name = 'OPINION_WEIGHT_ADOPTING_HIS_RELIGION';

-- -3
UPDATE Defines
SET Value = '-8'
WHERE Name = 'OPINION_WEIGHT_ADOPTING_MY_RELIGION';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_SAME_MAJORITY_RELIGIONS', '-2';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_DIFFERENT_STATE_RELIGIONS', '5';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_DIFFERENT_MAJORITY_RELIGIONS', '2';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_WORLD_RELIGION_MODIFIER', '150'; -- increases + & - opinion weights by 50% for the World Religion


-- Ideology Diplo Values
-- These values are multiplied by the current game era's Diplo Emphasis for Late Game Policies (aka Ideology), except the World Ideology Modifier.

-- 5
UPDATE Defines
SET Value = '-10'
WHERE Name = 'OPINION_WEIGHT_SAME_LATE_POLICIES';

-- 5
UPDATE Defines
SET Value = '10'
WHERE Name = 'OPINION_WEIGHT_DIFFERENT_LATE_POLICIES';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_WORLD_IDEOLOGY_MODIFIER', '150'; -- increases + & - opinion weights by 50% for the World Ideology


-- 20 (unchanged)
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_ROBBED_BY';

-- Theft Opinion Weights
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_PLUNDERED_TRADE_ROUTE', '5';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_PLOTTED_AGAINST_US', '20';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_PERFORMED_COUP', '15';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_EXCAVATED_THREE_ARTIFACTS', '40';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_EXCAVATED_TWO_ARTIFACTS', '30';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_EXCAVATED_ONE_ARTIFACT', '20';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_MADE_NO_DIGGING_PROMISE_REDUCTION', '-10';

-- -10 (unchanged)
UPDATE Defines
SET Value = '-10'
WHERE Name = 'OPINION_WEIGHT_INTRIGUE_SHARED_BY';

-- 40 (unchanged)
UPDATE Defines
SET Value = '40'
WHERE Name = 'OPINION_WEIGHT_BROKEN_MILITARY_PROMISE';

-- 15
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_BROKEN_MILITARY_PROMISE_WORLD';

-- 15
UPDATE Defines
SET Value = '0'
WHERE Name = 'OPINION_WEIGHT_IGNORED_MILITARY_PROMISE';

-- 1 (unchanged)
UPDATE Defines
SET Value = '1'
WHERE Name = 'BROKEN_EXPANSION_PROMISE_PER_OPINION_WEIGHT';

-- 20 (unchanged)
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_EXPANSION_PROMISE_BROKE_MAX';

-- 1 (unchanged)
UPDATE Defines
SET Value = '1'
WHERE Name = 'IGNORED_EXPANSION_PROMISE_PER_OPINION_WEIGHT';

-- 15 (unchanged)
UPDATE Defines
SET Value = '15'
WHERE Name = 'OPINION_WEIGHT_EXPANSION_PROMISE_IGNORED_MAX';

-- 1 (unchanged)
UPDATE Defines
SET Value = '1'
WHERE Name = 'BROKEN_BORDER_PROMISE_PER_OPINION_WEIGHT';

-- 20 (unchanged)
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_BORDER_PROMISE_BROKE_MAX';

-- 1 (unchanged)
UPDATE Defines
SET Value = '1'
WHERE Name = 'IGNORED_BORDER_PROMISE_PER_OPINION_WEIGHT';

-- 15 (unchanged)
UPDATE Defines
SET Value = '15'
WHERE Name = 'OPINION_WEIGHT_BORDER_PROMISE_IGNORED_MAX';

-- 40 (unchanged)
UPDATE Defines
SET Value = '40'
WHERE Name = 'OPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE';

-- 0
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE_WORLD';

-- 15 (unchanged)
UPDATE Defines
SET Value = '15'
WHERE Name = 'OPINION_WEIGHT_IGNORED_CITY_STATE_PROMISE';

-- Missing Firaxis Opinion Weights for Bully City-State Promise
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_BROKEN_BULLY_CITY_STATE_PROMISE', '20';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_IGNORED_BULLY_CITY_STATE_PROMISE', '10';

-- 8 (unchanged)
-- This is multiplied by the current game era's Diplo Emphasis for Religion.
UPDATE Defines
SET Value = '8'
WHERE Name = 'OPINION_WEIGHT_BROKEN_NO_CONVERT_PROMISE';

-- 4 (unchanged)
-- This is multiplied by the current game era's Diplo Emphasis for Religion.
UPDATE Defines
SET Value = '4'
WHERE Name = 'OPINION_WEIGHT_IGNORED_NO_CONVERT_PROMISE';

-- 30 (unchanged)
UPDATE Defines
SET Value = '30'
WHERE Name = 'OPINION_WEIGHT_BROKEN_NO_DIG_PROMISE';

-- 20 (unchanged)
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_IGNORED_NO_DIG_PROMISE';

-- 20 (unchanged)
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_BROKEN_SPY_PROMISE';

-- 10 (unchanged)
UPDATE Defines
SET Value = '10'
WHERE Name = 'OPINION_WEIGHT_IGNORED_SPY_PROMISE';

-- 20 (unchanged)
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_BROKEN_COOP_WAR_PROMISE';

-- 40 (unchanged)
UPDATE Defines
SET Value = '40'
WHERE Name = 'OPINION_WEIGHT_KILLED_PROTECTED_MINOR';

-- 10 (unchanged)
UPDATE Defines
SET Value = '10'
WHERE Name = 'OPINION_WEIGHT_KILLED_MANY_PROTECTED_MINORS';

-- 50 (unchanged)
UPDATE Defines
SET Value = '50'
WHERE Name = 'OPINION_WEIGHT_KILLED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN';

-- 20 (unchanged)
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY';

-- 15 (unchanged)
UPDATE Defines
SET Value = '15'
WHERE Name = 'OPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_WHILE_AGO';

-- 15 (unchanged)
UPDATE Defines
SET Value = '15'
WHERE Name = 'OPINION_WEIGHT_ATTACKED_MANY_PROTECTED_MINORS';

-- 20 (unchanged)
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY_NUM_TURNS';

-- 30 (unchanged)
UPDATE Defines
SET Value = '30'
WHERE Name = 'OPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN';

-- 15 (unchanged)
UPDATE Defines
SET Value = '15'
WHERE Name = 'OPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY';

-- 10 (unchanged)
UPDATE Defines
SET Value = '10'
WHERE Name = 'OPINION_WEIGHT_BULLIED_PROTECTED_MINOR_WHILE_AGO';

-- 10 (unchanged)
UPDATE Defines
SET Value = '10'
WHERE Name = 'OPINION_WEIGHT_BULLIED_MANY_PROTECTED_MINORS';

-- 20 (unchanged)
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY_NUM_TURNS';

-- 30 (unchanged)
UPDATE Defines
SET Value = '30'
WHERE Name = 'OPINION_WEIGHT_BULLIED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN';

-- 5
UPDATE Defines
SET Value = '10'
WHERE Name = 'OPINION_WEIGHT_SIDED_WITH_THEIR_MINOR';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_SIDED_WITH_THEIR_MINOR_AGGRESSIVE_MOD', '10';

-- 10 (unchanged)
UPDATE Defines
SET Value = '10'
WHERE Name = 'OPINION_WEIGHT_SIDED_WITH_THEIR_MINOR_NUM_TURNS_UNTIL_FORGIVEN';

-- Research Agreement Opinion Weight
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_RA', '-5';

-- Defensive Pact Opinion Weights
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_DP', '-10';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_DP_WITH_FRIEND', '-5';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_DP_WITH_ENEMY', '10';

-- Open Borders Opinion Weights
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_OPEN_BORDERS_MUTUAL', '-12';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_OPEN_BORDERS_US', '-8';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_OPEN_BORDERS_THEM', '-4';

-- -35
UPDATE Defines
SET Value = '-30'
WHERE Name = 'OPINION_WEIGHT_DOF';

-- -15
UPDATE Defines
SET Value = '-18'
WHERE Name = 'OPINION_WEIGHT_DOF_WITH_FRIEND';

-- 15
UPDATE Defines
SET Value = '18'
WHERE Name = 'OPINION_WEIGHT_DOF_WITH_ENEMY';

-- -10
UPDATE Defines
SET Value = '10'
WHERE Name = 'OPINION_WEIGHT_DENOUNCED_BY_FRIEND_DONT_LIKE';

-- 20 (unchanged)
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_DENOUNCED_BY_FRIEND_EACH';

-- 15
UPDATE Defines
SET Value = '30'
WHERE Name = 'OPINION_WEIGHT_DENOUNCED_FRIEND_EACH';

-- 35
UPDATE Defines
SET Value = '45'
WHERE Name = 'OPINION_WEIGHT_DENOUNCED_ME_FRIENDS';

-- 50
UPDATE Defines
SET Value = '50'
WHERE Name = 'OPINION_WEIGHT_WAR_FRIEND_EACH';

-- 60
UPDATE Defines
SET Value = '100'
WHERE Name = 'OPINION_WEIGHT_WAR_ME_FRIENDS';

-- 35 (unchanged)
UPDATE Defines
SET Value = '35'
WHERE Name = 'OPINION_WEIGHT_DENOUNCED_ME';

-- 35 (unchanged)
UPDATE Defines
SET Value = '35'
WHERE Name = 'OPINION_WEIGHT_DENOUNCED_THEM';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_MUTUAL_DENOUNCEMENT', '50';

-- Weight for excessive empire expansion
-- 35
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_RECKLESS_EXPANDER';

INSERT INTO Defines (Name, Value)
SELECT 'RECKLESS_EXPANDER_CITIES_THRESHOLD', '200'; -- must have at least this % city count compared to the median

INSERT INTO Defines (Name, Value)
SELECT 'RECKLESS_EXPANDER_LAND_THRESHOLD', '250'; -- OR must have at least this % plot count compared to the median

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_RECKLESS_EXPANDER_PER_CITY', '10';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_RECKLESS_EXPANDER_PER_TILE', '1';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_RECKLESS_EXPANDER_STRATEGIC_MOD', '20';

-- Weight for spamming World Wonders
INSERT INTO Defines (Name, Value)
SELECT 'WONDER_SPAMMER_THRESHOLD', '3'; -- must have constructed this many more Wonders than the median (only counting Wonder-building civs)

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_WONDER_SPAMMER', '20';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_WONDER_SPAMMER_PER_WONDER', '5';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_WONDER_SPAMMER_CAP', '60';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_WONDER_SPAMMER_STRATEGIC_MOD', '20';

-- 15
UPDATE Defines
SET Value = '18'
WHERE Name = 'OPINION_WEIGHT_DENOUNCED_FRIEND';

-- -15
UPDATE Defines
SET Value = '-18'
WHERE Name = 'OPINION_WEIGHT_DENOUNCED_ENEMY';

-- Recent Trade Values
-- Makes trade partnerships and trade deals last a bit longer
-- 3
UPDATE Defines
SET Value = '2'
WHERE Name = 'DEAL_VALUE_PER_TURN_DECAY';

-- 10
UPDATE Defines
SET Value = '5'
WHERE Name = 'DEAL_VALUE_PER_OPINION_WEIGHT';

-- -30
UPDATE Defines
SET Value = '-40'
WHERE Name = 'OPINION_WEIGHT_TRADE_MAX';

-- Common Foe Values
-- 25 (unchanged)
UPDATE Defines
SET Value = '25'
WHERE Name = 'COMMON_FOE_VALUE_PER_TURN_DECAY';

-- 50 (unchanged)
UPDATE Defines
SET Value = '50'
WHERE Name = 'COMMON_FOE_VALUE_PER_OPINION_WEIGHT';

-- -50 (unchanged)
UPDATE Defines
SET Value = '-50'
WHERE Name = 'OPINION_WEIGHT_COMMON_FOE_MAX';

-- Recent Assist Values
INSERT INTO Defines (Name, Value)
SELECT 'ASSIST_VALUE_PER_TURN_DECAY', '3';

INSERT INTO Defines (Name, Value)
SELECT 'ASSIST_VALUE_PER_OPINION_WEIGHT', '5';

-- 30 (unchanged)
UPDATE Defines
SET Value = '30'
WHERE Name = 'OPINION_WEIGHT_ASSIST_MAX';

-- 50
UPDATE Defines
SET Value = '100'
WHERE Name = 'OPINION_WEIGHT_NUKED_MAX';

-- Killed/Captured Civilians Weight
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_PER_CIVILIAN_KILLER_VALUE', '1';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_CIVILIAN_KILLER_MAX', '50';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_CIVILIAN_KILLER_WORLD', '10';

-- Similar/Divergent Social Policies Weight
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_PER_SIMILAR_POLICY', '-1';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_PER_DIVERGENT_POLICY', '-1';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_SIMILAR_POLICIES', '-5';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_DIVERGENT_POLICIES', '5';

INSERT INTO Defines (Name, Value)
SELECT 'POLICY_SCORE_NEEDY_THRESHOLD', '8';

INSERT INTO Defines (Name, Value)
SELECT 'POLICY_SCORE_NEEDY_MULTIPLIER', '2';

-- Pledge to Protect Same CS Weight
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_PTP_SAME_MINOR_EACH', '-3';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_PTP_SAME_MINOR_MIN', '-12';

-- 80
UPDATE Defines
SET Value = '160'
WHERE Name = 'OPINION_WEIGHT_CAPTURED_CAPITAL';

-- Captured Holy City Weight
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_CAPTURED_HOLY_CITY', '80';

-- Opinion Weight Divisor for Capitulated Vassals
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_CAPTURED_KEY_CITY_CAPITULATION_DIVISOR', '2';

-- -15 (unchanged)
UPDATE Defines
Set Value = '-15'
WHERE Name = 'OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL';

-- 15 (unchanged)
UPDATE Defines
SET Value = '15'
WHERE Name = 'OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL';

-- -20
UPDATE Defines
SET Value = '-10'
WHERE Name = 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL';

-- 20
UPDATE Defines
SET Value = '15'
WHERE Name = 'OPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL';

-- -20 (unchanged)
UPDATE Defines
SET Value = '-20'
WHERE Name = 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING';

-- AI spam message reduction
-- 45
UPDATE Defines
SET Value = '30'
WHERE Name = 'OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_NUM_TURNS';

-- 45
UPDATE Defines
SET Value = '30'
WHERE Name = 'OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_NUM_TURNS';

-- 45
UPDATE Defines
SET Value = '30'
WHERE Name = 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL_NUM_TURNS';

-- 45
UPDATE Defines
SET Value = '30'
WHERE Name = 'OPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL_NUM_TURNS';

-- 45 (unchanged)
UPDATE Defines
SET Value = '45'
WHERE Name = 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING_NUM_TURNS';