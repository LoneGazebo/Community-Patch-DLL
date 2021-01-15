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

-- Other Opinion Weights
-- These values affects the many individual opinion modifiers.

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

-- Tech Dispute Weight (for scientific civs)
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_POLICY_FIERCE', '30';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_POLICY_STRONG', '20';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_POLICY_WEAK', '10';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_POLICY_NONE', '-10';

-- 30 (unchanged)
UPDATE Defines
SET Value = '30'
WHERE Name = 'OPINION_WEIGHT_VICTORY_FIERCE';

-- 20 (unchanged)
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_VICTORY_STRONG';

-- 10 (unchanged)
UPDATE Defines
SET Value = '10'
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
SELECT 'OPINION_WEIGHT_VICTORY_BLOCK_FIERCE', '40';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_VICTORY_BLOCK_STRONG', '30';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_VICTORY_BLOCK_WEAK', '20';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_VICTORY_BLOCK_NONE', '0';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_VICTORY_BLOCK_PER_ERA', '4';

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

-- -80
UPDATE Defines
SET Value = '-120'
WHERE Name = 'OPINION_WEIGHT_LIBERATED_CAPITAL';

-- -40
UPDATE Defines
SET Value = '-30'
WHERE Name = 'OPINION_WEIGHT_LIBERATED_CITY';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LIBERATED_HOLY_CITY', '-80';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LIBERATED_CAPITAL_VASSAL_MULTIPLIER', '150';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LIBERATED_HOLY_CITY_VASSAL_MULTIPLIER', '150';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LIBERATED_CITY_VASSAL_MULTIPLIER', '200';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_RETURNED_CAPITAL', '-60';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_RETURNED_HOLY_CITY', '-40';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_RETURNED_CAPITAL_VASSAL_MULTIPLIER', '150';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_RETURNED_HOLY_CITY_VASSAL_MULTIPLIER', '150';

-- Embassy Opinion Weights
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_EMBASSY_MUTUAL', '-3';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_EMBASSY_THEM', '-1';

-- -1
UPDATE Defines
SET Value = '-2'
WHERE Name = 'OPINION_WEIGHT_EMBASSY';

-- Diplomat in their Capital
-- Additional bonus if AI has Diplomat personality
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_DIPLOMAT', '-15';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_DIPLOMAT_MOD', '-10';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_DIPLOMAT_CAPITULATED_VASSAL', '-10';

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

-- 20 (unchanged)
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_MADE_DEMAND_OF_US';

-- 30
UPDATE Defines
SET Value = '10'
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
SELECT 'OPINION_WEIGHT_PERFORMED_COUP', '30';

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

-- 20 (unchanged)
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_EXPANSION_PROMISE_BROKE_MAX';

-- 15 (unchanged)
UPDATE Defines
SET Value = '15'
WHERE Name = 'OPINION_WEIGHT_EXPANSION_PROMISE_IGNORED_MAX';

-- 20 (unchanged)
UPDATE Defines
SET Value = '20'
WHERE Name = 'OPINION_WEIGHT_BORDER_PROMISE_BROKE_MAX';

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
WHERE Name = 'OPINION_WEIGHT_ATTACKED_MANY_PROTECTED_MINORS';

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
WHERE Name = 'OPINION_WEIGHT_BULLIED_MANY_PROTECTED_MINORS';

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

-- Increases current and maximum trade bonus if player is considered a strategic trade partner
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_STRATEGIC_TRADE_PARTNER_MULTIPLIER', '150';

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
SELECT 'OPINION_WEIGHT_CIVILIAN_KILLER_WORLD', '20';

-- Similar/Divergent Social Policies Weight
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_PER_SIMILAR_POLICY', '-5';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_PER_DIVERGENT_POLICY', '5';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_SIMILAR_POLICIES', '-10';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_DIVERGENT_POLICIES', '10';

INSERT INTO Defines (Name, Value)
SELECT 'POLICY_SCORE_NEEDY_THRESHOLD', '8';

INSERT INTO Defines (Name, Value)
SELECT 'POLICY_SCORE_NEEDY_BONUS', '5';

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
SELECT 'OPINION_WEIGHT_CAPTURED_KEY_CITY_CAPITULATION_DIVISOR', '200';

-- World Congress Modifiers
-- League Alignment
INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_PRIME_LEAGUE_COMPETITOR', '25';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LEAGUE_ALIGNMENT_ENEMY', '20';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LEAGUE_ALIGNMENT_HATRED', '15';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LEAGUE_ALIGNMENT_RIVAL', '10';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LEAGUE_ALIGNMENT_FRIEND', '-10';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LEAGUE_ALIGNMENT_CONFIDANT', '-15';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LEAGUE_ALIGNMENT_ALLY', '-20';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LEAGUE_ALIGNMENT_LIBERATOR', '-25';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_PRIME_LEAGUE_COMPETITOR_DIPLOMAT', '50';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LEAGUE_ALIGNMENT_ENEMY_DIPLOMAT', '40';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LEAGUE_ALIGNMENT_HATRED_DIPLOMAT', '30';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LEAGUE_ALIGNMENT_RIVAL_DIPLOMAT', '20';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LEAGUE_ALIGNMENT_FRIEND_DIPLOMAT', '-20';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LEAGUE_ALIGNMENT_CONFIDANT_DIPLOMAT', '-30';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LEAGUE_ALIGNMENT_ALLY_DIPLOMAT', '-40';

INSERT INTO Defines (Name, Value)
SELECT 'OPINION_WEIGHT_LEAGUE_ALIGNMENT_LIBERATOR_DIPLOMAT', '-50';

-- Proposals
UPDATE Defines
SET Value = '-15'
WHERE Name = 'OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL';

INSERT INTO Defines(Name, Value)
SELECT 'OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_STRONG', '-30';

INSERT INTO Defines(Name, Value)
SELECT 'OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_OVERWHELMING', '-45';

INSERT INTO Defines(Name, Value)
SELECT 'OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_DIPLOMAT_MULTIPLIER', '134';

UPDATE Defines
SET Value = '15'
WHERE Name = 'OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL';

INSERT INTO Defines(Name, Value)
SELECT 'OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_STRONG', '30';

INSERT INTO Defines(Name, Value)
SELECT 'OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_OVERWHELMING', '45';

INSERT INTO Defines(Name, Value)
SELECT 'OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_DIPLOMAT_MULTIPLIER', '134';

UPDATE Defines
SET Value = '-10'
WHERE Name = 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL'; -- min

INSERT INTO Defines(Name, Value)
SELECT 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL_MAX', '-60';

INSERT INTO Defines(Name, Value)
SELECT 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL_DIPLOMAT_MULTIPLIER', '167';

UPDATE Defines
SET Value = '10'
WHERE Name = 'OPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL'; -- min

INSERT INTO Defines(Name, Value)
SELECT 'OPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL_MAX', '60';

INSERT INTO Defines(Name, Value)
SELECT 'OPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL_DIPLOMAT_MULTIPLIER', '167';

UPDATE Defines
SET Value = '-20'
WHERE Name = 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING'; -- min

INSERT INTO Defines(Name, Value)
SELECT 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING_MAX', '-70';

INSERT INTO Defines(Name, Value)
SELECT 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING_DIPLOMAT_MULTIPLIER', '150';

-- This % of the difference between the max and min values is added for each % that you contributed to the triggering vote
INSERT INTO Defines(Name, Value)
SELECT 'OPINION_WEIGHT_PER_VOTE_PERCENT', '2';

-- 45
UPDATE Defines
SET Value = '50'
WHERE Name = 'OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_NUM_TURNS';

-- 45
UPDATE Defines
SET Value = '50'
WHERE Name = 'OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_NUM_TURNS';

-- 45
UPDATE Defines
SET Value = '50'
WHERE Name = 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL_NUM_TURNS';

-- 45
UPDATE Defines
SET Value = '50'
WHERE Name = 'OPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL_NUM_TURNS';

-- 45
UPDATE Defines
SET Value = '50'
WHERE Name = 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING_NUM_TURNS';


-- Promise Duration (affects all players)
-- Defines how many turns a specific promise lasts for (for making the promise).
-- Defines how many turns until the AI forgets that you ignored/broke a specific promise (for other values). Once the AI forgets that you ignored/broke a promise, they become able to ask you to make it again.
-- Most promise lengths scale with game speed: Quick 0.67x, Standard 1x, Epic 1.5x, Marathon 3x

-- Military/Move Troops Request
-- This promise does NOT scale with game speed!

INSERT INTO Defines (Name, Value)
SELECT 'MOVE_TROOPS_MEMORY_TURN_EXPIRATION', '20';

INSERT INTO Defines (Name, Value)
SELECT 'MILITARY_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', '40';

INSERT INTO Defines (Name, Value)
SELECT 'MILITARY_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', '80';

-- Stop Settling Near Us

-- 50
UPDATE Defines
SET Value = '50'
WHERE Name = 'EXPANSION_PROMISE_TURNS_EFFECTIVE';

INSERT INTO Defines (Name, Value)
SELECT 'EXPANSION_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', '30';

INSERT INTO Defines (Name, Value)
SELECT 'EXPANSION_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', '50';

-- Stop Buying Land Near Us

-- 50
UPDATE Defines
SET Value = '50'
WHERE Name = 'BORDER_PROMISE_TURNS_EFFECTIVE'; 

INSERT INTO Defines (Name, Value)
SELECT 'BORDER_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', '30';

INSERT INTO Defines (Name, Value)
SELECT 'BORDER_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', '50';

-- Stop Bullying Our Protected City-States

INSERT INTO Defines (Name, Value)
SELECT 'BULLY_CS_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', '30';

INSERT INTO Defines (Name, Value)
SELECT 'BULLY_CS_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', '50';

-- Don't Conquer Our Protected City-States

INSERT INTO Defines (Name, Value)
SELECT 'ATTACK_CS_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', '40';

INSERT INTO Defines (Name, Value)
SELECT 'ATTACK_CS_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', '60';

-- Stop Spying On Us

INSERT INTO Defines (Name, Value)
SELECT 'SPY_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', '30';

INSERT INTO Defines (Name, Value)
SELECT 'SPY_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', '50';

-- Stop Converting Our Cities

INSERT INTO Defines (Name, Value)
SELECT 'CONVERT_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', '40';

INSERT INTO Defines (Name, Value)
SELECT 'CONVERT_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', '60';

-- Stop Digging Up Our Artifacts

INSERT INTO Defines (Name, Value)
SELECT 'DIGGING_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', '40';

INSERT INTO Defines (Name, Value)
SELECT 'DIGGING_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', '60';

-- Broken Coop War Promise
-- This promise does NOT scale with game speed!

INSERT INTO Defines (Name, Value)
SELECT 'COOP_WAR_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', '60';


-- Backstabbing Penalties Duration (affects all players)
-- Defines how many turns until the AI forgets that you backstabbed them.
-- The amount of turns scales with game speed: Quick 0.67x, Standard 1x, Epic 1.5x, Marathon 3x
-- NOTE: Broken military / CS conquest promises count as backstabbing penalties as well, but they are handled in the Promises section just above.

INSERT INTO Defines (Name, Value)
SELECT 'DOF_BROKEN_TURNS_UNTIL_FORGIVEN', '50';

INSERT INTO Defines (Name, Value)
SELECT 'DOF_BROKEN_TURNS_UNTIL_FORGIVEN_FRIENDS', '10'; -- if Opinion has risen to FRIEND or higher since ending the DoF

INSERT INTO Defines (Name, Value)
SELECT 'FRIEND_DENOUNCED_US_TURNS_UNTIL_FORGIVEN', '75';

INSERT INTO Defines (Name, Value)
SELECT 'FRIEND_DECLARED_WAR_ON_US_TURNS_UNTIL_FORGIVEN', '100';

INSERT INTO Defines (Name, Value)
SELECT 'MASTER_DECLARED_WAR_ON_US_TURNS_UNTIL_FORGIVEN', '100';

-- Number of turns after prematurely ending a DoF during which you will obtain backstabbing penalties for denouncing or declaring war.
-- It will always take AT LEAST this many turns to clear the penalty for prematurely ending a DoF.
-- This value does not scale with game speed!
INSERT INTO Defines (Name, Value)
SELECT 'DOF_BROKEN_BACKSTAB_TIMER', '10';