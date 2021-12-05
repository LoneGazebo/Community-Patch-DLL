-- These two entries control whether AI major civs and City-States are able to move their starting Settler(s) from their starting plot - set to 0 to disable the ability
-- NOTE: They can always move their starting Settler(s) if they're not able to found a city in their starting location
INSERT INTO Defines(Name, Value) SELECT 'MAJORS_CAN_MOVE_STARTING_SETTLER', '1';
INSERT INTO Defines(Name, Value) SELECT 'CS_CAN_MOVE_STARTING_SETTLER', '0';

-- Number of rings a new city starts with
INSERT INTO Defines (Name, Value) SELECT 'CITY_STARTING_RINGS', '1';

-- Player Proximity Values
UPDATE Defines SET Value = '8' WHERE Name = 'PROXIMITY_NEIGHBORS_CLOSEST_CITY_REQUIREMENT';
UPDATE Defines SET Value = '16' WHERE Name = 'PROXIMITY_CLOSE_CLOSEST_CITY_POSSIBILITY';
UPDATE Defines SET Value = '24' WHERE Name = 'PROXIMITY_FAR_DISTANCE_MAX';
UPDATE Defines SET Value = '5' WHERE Name = 'AI_DIPLO_PLOT_RANGE_FROM_CITY_HOME_FRONT'; -- this is in plots

-- Minimum war duration for AI players
INSERT INTO Defines(Name, Value) SELECT 'WAR_MAJOR_MINIMUM_TURNS', '10';
INSERT INTO Defines(Name, Value) SELECT 'WAR_MINOR_MINIMUM_TURNS', '1';


-- Combat Stuff
UPDATE Defines SET Value = '5' WHERE Name = 'AI_TACTICAL_MAP_TEMP_ZONE_TURNS'; -- Number of turns AI waits while recruiting before disbanding an operation
UPDATE Defines SET Value = '10' WHERE Name = 'AI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY'; -- ...in enemy territory
UPDATE Defines SET Value = '70' WHERE Name = 'AI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION';
UPDATE Defines SET Value = '40' WHERE Name = 'AI_TACTICAL_MAP_DOMINANCE_PERCENTAGE';
UPDATE Defines SET Value = '5' WHERE Name = 'AI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT';

INSERT INTO Defines (Name, Value) SELECT 'BALANCE_BASIC_ATTACK_ARMY_SIZE', '6';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_ARMY_NAVY_START_SIZE', '3';
INSERT INTO Defines(Name, Value) SELECT 'AI_CONFIG_MILITARY_MELEE_PER_AA', '2'; -- Number of melee units per AA unit before the AI considers that they have enough
INSERT INTO Defines(Name, Value) SELECT 'AI_CONFIG_MILITARY_TILES_PER_SHIP', '6'; -- Number of water tiles per ship for (small?) bodies of water

UPDATE Defines SET Value = '-40' WHERE Name = 'CITY_RANGED_ATTACK_STRENGTH_MULTIPLIER';

-- Combat mechanic changes that are considered "core"
UPDATE Defines SET Value = '-50' WHERE Name = 'AIR_SWEEP_INTERCEPTION_DAMAGE_MOD';
UPDATE Defines SET Value = '10' WHERE Name = 'BONUS_PER_ADJACENT_FRIEND';

-- AI combat tuning knobs

-- this controls how much damage an attack needs to inflict to be considered worthwhile
-- default value 100. higher -> more aggressive
INSERT INTO Defines(Name, Value) SELECT 'COMBAT_AI_OFFENSE_DAMAGEWEIGHT', '100';

-- this controls whether ending the turn on a certain plot is acceptable (defense and offense)
-- default value 100. higher -> more timid
INSERT INTO Defines(Name, Value) SELECT 'COMBAT_AI_OFFENSE_DANGERWEIGHT', '100';

-- this controls whether a 'bad' offensive move should still be considered
-- default value 20. higher -> more reckless
INSERT INTO Defines(Name, Value) SELECT 'COMBAT_AI_OFFENSE_SCORE_BIAS', '20';

-- this controls whether a 'bad' defensive move should still be considered
-- default value 200. higher -> more desperate
INSERT INTO Defines(Name, Value) SELECT 'COMBAT_AI_DEFENSE_SCORE_BIAS', '200';


-- City Revolt Timer
UPDATE Defines SET Value = '10' WHERE Name = 'REVOLT_COUNTER_MIN';

-- Religion Spread Rework
UPDATE Defines SET Value = '9' WHERE Name = 'RELIGION_ADJACENT_CITY_DISTANCE';

-- Spy Stuff
UPDATE Defines SET Value = '1000' WHERE Name = 'ESPIONAGE_GATHERING_INTEL_COST_PERCENT';
UPDATE Defines SET Value = '3' WHERE Name = 'ESPIONAGE_COUP_MULTIPLY_CONSTANT';

-- Trade Stuff
INSERT INTO Defines (Name, Value) SELECT 'OPEN_BORDERS_MODIFIER_TRADE_GOLD', '20'; -- Open Borders Trade Gold Value (halved if not mutual)
UPDATE Defines SET Value = '5' WHERE Name = 'EACH_GOLD_PER_TURN_VALUE_PERCENT'; -- Gold Per Turn Interest Rate for Trade Valuation

-- City-State Stuff
UPDATE Defines SET Value = '80' WHERE Name = 'MINOR_CIV_GROWTH_PERCENT'; -- CS buff to keep up with AI
UPDATE Defines SET Value = '120' WHERE Name = 'MINOR_CIV_PRODUCTION_PERCENT'; -- CS buff to keep up with AI
UPDATE Defines SET Value = '120' WHERE Name = 'MINOR_CIV_GOLD_PERCENT'; -- CS buff to keep up with AI
UPDATE Defines SET Value = '40' WHERE Name = 'PERMANENT_WAR_AGGRESSOR_CHANCE';
UPDATE Defines SET Value = '20' WHERE Name = 'PERMANENT_WAR_OTHER_WARMONGER_CHANCE_DISTANT';
UPDATE Defines SET Value = '30' WHERE Name = 'PERMANENT_WAR_OTHER_WARMONGER_CHANCE_FAR';
UPDATE Defines SET Value = '50' WHERE Name = 'PERMANENT_WAR_OTHER_WARMONGER_CHANCE_CLOSE';
UPDATE Defines SET Value = '60' WHERE Name = 'PERMANENT_WAR_OTHER_WARMONGER_CHANCE_NEIGHBORS';
UPDATE Defines SET Value = '10' WHERE Name = 'PERMANENT_WAR_OTHER_CHANCE_DISTANT';
UPDATE Defines SET Value = '15' WHERE Name = 'PERMANENT_WAR_OTHER_CHANCE_FAR';
UPDATE Defines SET Value = '20' WHERE Name = 'PERMANENT_WAR_OTHER_CHANCE_CLOSE';
UPDATE Defines SET Value = '25' WHERE Name = 'PERMANENT_WAR_OTHER_CHANCE_NEIGHBORS';
UPDATE Defines SET Value = '20' WHERE Name = 'PERMANENT_WAR_OTHER_AT_WAR';

-- Barbarian Stuff
UPDATE Defines SET Value = '4' WHERE Name = 'BARBARIAN_CAMP_MINIMUM_DISTANCE_ANOTHER_CAMP';


-- Settler Stuff
UPDATE Defines SET Value = '4' WHERE Name = 'SETTLER_DISTANCE_DROPOFF_MODIFIER'; -- Start of settle plot devaluation (prefer close settling)
UPDATE Defines SET Value = '8' WHERE Name = 'SETTLER_EVALUATION_DISTANCE'; -- Max range for settling (time offset will be added)
UPDATE Defines SET Value = '6' WHERE Name = 'CITY_RING_1_MULTIPLIER'; -- there are 6 ring1 plots but don't overemphasize the very short term
UPDATE Defines SET Value = '6' WHERE Name = 'CITY_RING_2_MULTIPLIER'; -- there are 12 ring2 plots, workable soon
UPDATE Defines SET Value = '3' WHERE Name = 'CITY_RING_3_MULTIPLIER'; -- there are 18 ring3 plots, workable late and often conflicted
UPDATE Defines SET Value = '12' WHERE Name = 'SETTLER_FOOD_MULTIPLIER';
UPDATE Defines SET Value = '12' WHERE Name = 'SETTLER_PRODUCTION_MULTIPLIER';
UPDATE Defines SET Value = '8' WHERE Name = 'SETTLER_GOLD_MULTIPLIER';
UPDATE Defines SET Value = '5' WHERE Name = 'SETTLER_SCIENCE_MULTIPLIER';
UPDATE Defines SET Value = '4' WHERE Name = 'SETTLER_FAITH_MULTIPLIER';
UPDATE Defines SET Value = '35' WHERE Name = 'BUILD_ON_RIVER_PERCENT';
UPDATE Defines SET Value = '40' WHERE Name = 'SETTLER_BUILD_ON_COAST_PERCENT';
UPDATE Defines SET Value = '-10' WHERE Name = 'BUILD_ON_RESOURCE_PERCENT';
UPDATE Defines SET Value = '3' WHERE Name = 'SETTLER_STRATEGIC_MULTIPLIER';
UPDATE Defines SET Value = '10' WHERE Name = 'CHOKEPOINT_STRATEGIC_VALUE';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_BAD_TILES_STRATEGIC_VALUE', '-34';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_FRESH_WATER_STRATEGIC_VALUE', '5';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_COAST_STRATEGIC_VALUE', '3';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_EMPIRE_BORDERLAND_STRATEGIC_VALUE', '10';

-- Worker Stuff
UPDATE Defines SET Value = '3' WHERE Name = 'BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_FOOD';
UPDATE Defines SET Value = '2' WHERE Name = 'BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_PRODUCTION';
UPDATE Defines SET Value = '2' WHERE Name = 'BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_GOLD';
UPDATE Defines SET Value = '2' WHERE Name = 'BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_SCIENCE';
UPDATE Defines SET Value = '3' WHERE Name = 'BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_CULTURE';
UPDATE Defines SET Value = '1000' WHERE Name = 'BUILDER_TASKING_BASELINE_REPAIR';
UPDATE Defines SET Value = '750' WHERE Name = 'BUILDER_TASKING_BASELINE_BUILD_ROUTES';
UPDATE Defines SET Value = '300' WHERE Name = 'BUILDER_TASKING_BASELINE_BUILD_RESOURCE_IMPROVEMENTS';
UPDATE Defines SET Value = '100' WHERE Name = 'BUILDER_TASKING_BASELINE_BUILD_IMPROVEMENTS';
INSERT INTO Defines (Name, Value) SELECT 'BUILDER_TASKING_BASELINE_ADDS_FOOD', '200';
INSERT INTO Defines (Name, Value) SELECT 'BUILDER_TASKING_BASELINE_ADDS_GOLD', '40';
INSERT INTO Defines (Name, Value) SELECT 'BUILDER_TASKING_BASELINE_ADDS_FAITH', '150';
INSERT INTO Defines (Name, Value) SELECT 'BUILDER_TASKING_BASELINE_ADDS_PRODUCTION', '200';
INSERT INTO Defines (Name, Value) SELECT 'BUILDER_TASKING_BASELINE_ADDS_SCIENCE', '200';
UPDATE Defines SET Value = '200' WHERE Name = 'BUILDER_TASKING_BASELINE_ADDS_CULTURE';

-- Scouts
INSERT INTO Defines (Name, Value) SELECT 'MAX_PLOTS_PER_EXPLORER', '20'; -- Recon unit calculations

-- Automations
INSERT INTO Defines (Name, Value) SELECT 'UNIT_AUTO_EXTRA_AUTOMATIONS_DISABLED', '0';


-- Tech Stuff
UPDATE Defines SET Value = '0.035' WHERE Name = 'AI_RESEARCH_WEIGHT_MOD_PER_TURN_LEFT';
UPDATE Defines SET Value = '0.2' WHERE Name = 'AI_RESEARCH_WEIGHT_BASE_MOD';
UPDATE Defines SET Value = '25' WHERE Name = 'TECH_PRIORITY_UNIQUE_ITEM'; -- emphasis on uniques - changed formula

-- Policy Stuff
UPDATE Defines SET Value = '90' WHERE Name = 'POLICY_WEIGHT_PERCENT_DROP_NEW_BRANCH';
UPDATE Defines SET Value = '45' WHERE Name = 'IDEOLOGY_SCORE_PER_FREE_TENET';
UPDATE Defines SET Value = '10' WHERE Name = 'IDEOLOGY_SCORE_HAPPINESS';
UPDATE Defines SET Value = '1000' WHERE Name = 'IDEOLOGY_SCORE_GRAND_STRATS';
UPDATE Defines SET Value = '500' WHERE Name = 'IDEOLOGY_SCORE_HOSTILE';
UPDATE Defines SET Value = '250' WHERE Name = 'IDEOLOGY_SCORE_GUARDED';
UPDATE Defines SET Value = '150' WHERE Name = 'IDEOLOGY_SCORE_AFRAID';
UPDATE Defines SET Value = '25' WHERE Name = 'IDEOLOGY_SCORE_FRIENDLY';

-- Religion Stuff
UPDATE Defines SET Value = '2' WHERE Name = 'RELIGION_MAX_MISSIONARIES';
UPDATE Defines SET Value = '20' WHERE Name = 'RELIGION_MISSIONARY_RANGE_IN_TURNS';
UPDATE Defines SET Value = '6' WHERE Name = 'RELIGION_BELIEF_SCORE_CITY_MULTIPLIER';
UPDATE Defines SET Value = '4' WHERE Name = 'RELIGION_BELIEF_SCORE_UNOWNED_PLOT_MULTIPLIER'; -- Unowned but in range more valuable than before to make sure AI knows it's in a good position
UPDATE Defines SET Value = '8' WHERE Name = 'RELIGION_BELIEF_SCORE_OWNED_PLOT_MULTIPLIER'; -- Owned slightly more valuable than before

-- AI Grand Strategy
UPDATE Defines SET Value = '10' WHERE Name = 'AI_GRAND_STRATEGY_NUM_TURNS_STRATEGY_MUST_BE_ACTIVE';
UPDATE Defines SET Value = '50' WHERE Name = 'AI_GRAND_STRATEGY_CURRENT_STRATEGY_WEIGHT';
UPDATE Defines SET Value = '-100' WHERE Name = 'AI_GRAND_STRATEGY_CONQUEST_NOBODY_MET_WEIGHT';
UPDATE Defines SET Value = '600' WHERE Name = 'AI_GS_SS_TECH_PROGRESS_MOD';
UPDATE Defines SET Value = '60' WHERE Name = 'AI_GS_CULTURE_RATIO_MULTIPLIER';
UPDATE Defines SET Value = '80' WHERE Name = 'AI_GS_TOURISM_RATIO_MULTIPLIER';
UPDATE Defines SET Value = '33' WHERE Name = 'AI_GRAND_STRATEGY_OTHER_PLAYERS_GS_MULTIPLIER';
UPDATE Defines SET Value = '150' WHERE Name = 'AI_GRAND_STRATEGY_GUESS_POSITIVE_THRESHOLD';
UPDATE Defines SET Value = '80' WHERE Name = 'AI_GRAND_STRATEGY_GUESS_LIKELY_THRESHOLD';

-- AI City Specialization
UPDATE Defines SET Value = '35' WHERE Name = 'AI_CITY_SPECIALIZATION_REEVALUATION_INTERVAL';
UPDATE Defines SET Value = '20' WHERE Name = 'AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_OPERATIONAL_UNITS_REQUESTED'; 
UPDATE Defines SET Value = '150' WHERE Name = 'AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CIVS_AT_WAR_WITH'; 
UPDATE Defines SET Value = '250' WHERE Name = 'AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_WAR_MOBILIZATION'; 
UPDATE Defines SET Value = '250' WHERE Name = 'AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE'; 
UPDATE Defines SET Value = '1250' WHERE Name = 'AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE_CRITICAL'; 
UPDATE Defines SET Value = '250' WHERE Name = 'AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_WONDER';

-- Citizen allocation
-- should all be in the same ballpark, specialization decides which yield to maximize; food is always highly valued when needed
UPDATE Defines SET Value = '12' WHERE Name = 'AI_CITIZEN_VALUE_FOOD';
UPDATE Defines SET Value = '12' WHERE Name = 'AI_CITIZEN_VALUE_PRODUCTION';
UPDATE Defines SET Value = '12' WHERE Name = 'AI_CITIZEN_VALUE_GOLD';
UPDATE Defines SET Value = '16' WHERE Name = 'AI_CITIZEN_VALUE_SCIENCE';
UPDATE Defines SET Value = '16' WHERE Name = 'AI_CITIZEN_VALUE_CULTURE';
UPDATE Defines SET Value = '12' WHERE Name = 'AI_CITIZEN_VALUE_FAITH';

-- AI City Strategy
UPDATE Defines SET Value = '7' WHERE Name = 'AI_CITYSTRATEGY_MEDIUM_CITY_POP_THRESHOLD';
UPDATE Defines SET Value = '15' WHERE Name = 'AI_CITYSTRATEGY_LARGE_CITY_POP_THRESHOLD';
UPDATE Defines SET Value = '5000' WHERE Name = 'AI_CITYSTRATEGY_OPERATION_UNIT_BASE_WEIGHT';
UPDATE Defines SET Value = '0.5' WHERE Name = 'AI_CITYSTRATEGY_YIELD_DEFICIENT_FOOD'; -- food is different because we include consumption.
UPDATE Defines SET Value = '1.0' WHERE Name = 'AI_CITYSTRATEGY_YIELD_DEFICIENT_PRODUCTION';
UPDATE Defines SET Value = '1.5' WHERE Name = 'AI_CITYSTRATEGY_YIELD_DEFICIENT_GOLD';
UPDATE Defines SET Value = '2.5' WHERE Name = 'AI_CITYSTRATEGY_YIELD_DEFICIENT_SCIENCE';
INSERT INTO Defines (Name, Value) SELECT 'AI_CITYSTRATEGY_YIELD_DEFICIENT_CULTURE', '2.5';
INSERT INTO Defines (Name, Value) SELECT 'AI_CITYSTRATEGY_YIELD_DEFICIENT_FAITH', '2.5';
UPDATE Defines SET Value = '3' WHERE Name = 'AI_CITYSTRATEGY_WANT_TILE_IMPROVERS_MINIMUM_SIZE';
UPDATE AICityStrategies SET WeightThreshold = '1' WHERE Type = 'AICITYSTRATEGY_WANT_TILE_IMPROVERS';

-- AI Player Strategy
UPDATE Defines SET Value = '20' WHERE Name = 'AI_STRATEGY_EARLY_EXPANSION_NUM_UNOWNED_TILES_REQUIRED'; -- Was 25 before, making the AI miss out on possible good city spots just because there might be cities nearby

-- AI City Conquest logic: used when AI conquers a city and is deciding what to do with it
INSERT INTO Defines(Name, Value) SELECT 'AI_CITY_VALUE_MULTIPLIER', '100';
INSERT INTO Defines(Name, Value) SELECT 'AI_CAPITAL_VALUE_MULTIPLIER', '150';
INSERT INTO Defines(Name, Value) SELECT 'AI_MOSTLY_HAPPY_THRESHOLD', '40';
INSERT INTO Defines(Name, Value) SELECT 'AI_CITY_HIGH_VALUE_THRESHOLD', '80';
INSERT INTO Defines(Name, Value) SELECT 'AI_CITY_SOME_VALUE_THRESHOLD', '40';
INSERT INTO Defines(Name, Value) SELECT 'AI_CITY_PUPPET_BONUS_THRESHOLD', '25';

-- AI World Congress logic
INSERT INTO Defines (Name, Value) SELECT 'AI_WORLD_LEADER_BASE_WEIGHT_SELF', '1000';

-- AI purchase logic
INSERT INTO Defines (Name, Value) SELECT 'AI_GOLD_TREASURY_BUFFER', '150';
UPDATE Defines SET Value = '250' WHERE Name = 'AI_GOLD_PRIORITY_DEFENSIVE_BUILDING'; -- No idea why this was so high by default, spending gold on defensive buildings seems like a waste unless it's an emergency
UPDATE Defines SET Value = '500' WHERE Name = 'AI_GOLD_PRIORITY_UPGRADE_BASE';
UPDATE Defines SET Value = '100' WHERE Name = 'AI_GOLD_PRIORITY_UPGRADE_PER_FLAVOR_POINT';
UPDATE Defines SET Value = '80' WHERE Name = 'AI_PLOT_VALUE_STRATEGIC_RESOURCE'; -- Now equal to the luxury resource plot bonus (for luxuries we don't have yet)
UPDATE Defines SET Value = '150' WHERE Name = 'AI_GOLD_PRIORITY_MINIMUM_PLOT_BUY_VALUE';

-- City Growth Multiplier is a float
UPDATE Defines SET Value = '8.0' WHERE Name = 'CITY_GROWTH_MULTIPLIER';


-- Diplomacy Values

-- Personality Flavors
UPDATE Defines SET Value = '1' WHERE Name = 'PERSONALITY_FLAVOR_MIN_VALUE';
INSERT INTO Defines(Name, Value) SELECT 'DIPLO_PERSONALITY_FLAVOR_MIN_VALUE', '1';
INSERT INTO Defines(Name, Value) SELECT 'DIPLO_PERSONALITY_FLAVOR_MAX_VALUE', '10';

-- War Damage Values
UPDATE Defines SET Value = '175' WHERE Name = 'WAR_DAMAGE_LEVEL_CITY_WEIGHT';
UPDATE Defines SET Value = '150' WHERE Name = 'WAR_DAMAGE_LEVEL_INVOLVED_CITY_POP_MULTIPLIER';
INSERT INTO Defines (Name, Value) SELECT 'WAR_DAMAGE_LEVEL_WORLD_WONDER_MULTIPLIER', '200';
INSERT INTO Defines (Name, Value) SELECT 'PILLAGED_TILE_BASE_WAR_VALUE', '40';
INSERT INTO Defines (Name, Value) SELECT 'STOLEN_TILE_BASE_WAR_VALUE', '80';

-- War Damage Thresholds (used in the UI only)
UPDATE Defines SET Value = '15' WHERE Name = 'WAR_DAMAGE_LEVEL_THRESHOLD_MINOR';
UPDATE Defines SET Value = '30' WHERE Name = 'WAR_DAMAGE_LEVEL_THRESHOLD_MAJOR';
UPDATE Defines SET Value = '50' WHERE Name = 'WAR_DAMAGE_LEVEL_THRESHOLD_SERIOUS';
UPDATE Defines SET Value = '70' WHERE Name = 'WAR_DAMAGE_LEVEL_THRESHOLD_CRIPPLED';

-- Aggressive Postures
UPDATE Defines SET Value = '30' WHERE Name = 'EXPANSION_BICKER_TIMEOUT';

UPDATE Defines SET Value = '12' WHERE Name = 'PLOT_BUYING_POSTURE_INCREDIBLE_THRESHOLD';
UPDATE Defines SET Value = '9' WHERE Name = 'PLOT_BUYING_POSTURE_HIGH_THRESHOLD';
UPDATE Defines SET Value = '5' WHERE Name = 'PLOT_BUYING_POSTURE_MEDIUM_THRESHOLD';
UPDATE Defines SET Value = '3' WHERE Name = 'PLOT_BUYING_POSTURE_LOW_THRESHOLD';

UPDATE Defines SET Value = '800' WHERE Name = 'MILITARY_AGGRESSIVE_POSTURE_THRESHOLD_INCREDIBLE';
UPDATE Defines SET Value = '500' WHERE Name = 'MILITARY_AGGRESSIVE_POSTURE_THRESHOLD_HIGH';
UPDATE Defines SET Value = '300' WHERE Name = 'MILITARY_AGGRESSIVE_POSTURE_THRESHOLD_MEDIUM';
UPDATE Defines SET Value = '100' WHERE Name = 'MILITARY_AGGRESSIVE_POSTURE_THRESHOLD_LOW';

-- Dispute Levels
UPDATE Defines SET Value = '8' WHERE Name = 'LAND_DISPUTE_FIERCE_THRESHOLD';
UPDATE Defines SET Value = '4' WHERE Name = 'LAND_DISPUTE_STRONG_THRESHOLD';
UPDATE Defines SET Value = '1' WHERE Name = 'LAND_DISPUTE_WEAK_THRESHOLD';
UPDATE Defines SET Value = '50' WHERE Name = 'LAND_DISPUTE_CRAMPED_MULTIPLIER';

UPDATE Defines SET Value = '24' WHERE Name = 'WONDER_DISPUTE_FIERCE_THRESHOLD';
UPDATE Defines SET Value = '12' WHERE Name = 'WONDER_DISPUTE_STRONG_THRESHOLD';
UPDATE Defines SET Value = '1' WHERE Name = 'WONDER_DISPUTE_WEAK_THRESHOLD';

UPDATE Defines SET Value = '1400' WHERE Name = 'MINOR_CIV_DISPUTE_FIERCE_THRESHOLD';
UPDATE Defines SET Value = '1000' WHERE Name = 'MINOR_CIV_DISPUTE_STRONG_THRESHOLD';
UPDATE Defines SET Value = '700' WHERE Name = 'MINOR_CIV_DISPUTE_WEAK_THRESHOLD';

UPDATE Defines SET Value = '25' WHERE Name = 'VICTORY_DISPUTE_GRAND_STRATEGY_MATCH_POSITIVE';
UPDATE Defines SET Value = '15' WHERE Name = 'VICTORY_DISPUTE_GRAND_STRATEGY_MATCH_LIKELY';
UPDATE Defines SET Value = '5' WHERE Name = 'VICTORY_DISPUTE_GRAND_STRATEGY_MATCH_UNSURE';

INSERT INTO Defines (Name, Value) SELECT 'VICTORY_BLOCK_FIERCE_THRESHOLD', '40';
INSERT INTO Defines (Name, Value) SELECT 'VICTORY_BLOCK_STRONG_THRESHOLD', '30';
INSERT INTO Defines (Name, Value) SELECT 'VICTORY_BLOCK_WEAK_THRESHOLD', '20';
INSERT INTO Defines (Name, Value) SELECT 'VICTORY_BLOCK_GRAND_STRATEGY_DIFFERENCE_POSITIVE', '20';
INSERT INTO Defines (Name, Value) SELECT 'VICTORY_BLOCK_GRAND_STRATEGY_DIFFERENCE_LIKELY', '15';
INSERT INTO Defines (Name, Value) SELECT 'VICTORY_BLOCK_GRAND_STRATEGY_DIFFERENCE_UNSURE', '5';

INSERT INTO Defines (Name, Value) SELECT 'TECH_BLOCK_FIERCE_THRESHOLD', '30';
INSERT INTO Defines (Name, Value) SELECT 'TECH_BLOCK_STRONG_THRESHOLD', '20';
INSERT INTO Defines (Name, Value) SELECT 'TECH_BLOCK_WEAK_THRESHOLD', '10';

INSERT INTO Defines (Name, Value) SELECT 'POLICY_BLOCK_FIERCE_THRESHOLD', '24';
INSERT INTO Defines (Name, Value) SELECT 'POLICY_BLOCK_STRONG_THRESHOLD', '16';
INSERT INTO Defines (Name, Value) SELECT 'POLICY_BLOCK_WEAK_THRESHOLD', '8';

-- Comparative Strength Thresholds
UPDATE Defines SET Value = '33' WHERE Name = 'MILITARY_STRENGTH_WEAK_THRESHOLD';
UPDATE Defines SET Value = '50' WHERE Name = 'MILITARY_STRENGTH_POOR_THRESHOLD';
UPDATE Defines SET Value = '75' WHERE Name = 'MILITARY_STRENGTH_AVERAGE_THRESHOLD';
UPDATE Defines SET Value = '126' WHERE Name = 'MILITARY_STRENGTH_STRONG_THRESHOLD';
UPDATE Defines SET Value = '200' WHERE Name = 'MILITARY_STRENGTH_POWERFUL_THRESHOLD';
UPDATE Defines SET Value = '300' WHERE Name = 'MILITARY_STRENGTH_IMMENSE_THRESHOLD';

UPDATE Defines SET Value = '33' WHERE Name = 'ECONOMIC_STRENGTH_WEAK_THRESHOLD';
UPDATE Defines SET Value = '50' WHERE Name = 'ECONOMIC_STRENGTH_POOR_THRESHOLD';
UPDATE Defines SET Value = '75' WHERE Name = 'ECONOMIC_STRENGTH_AVERAGE_THRESHOLD';
UPDATE Defines SET Value = '126' WHERE Name = 'ECONOMIC_STRENGTH_STRONG_THRESHOLD';
UPDATE Defines SET Value = '200' WHERE Name = 'ECONOMIC_STRENGTH_POWERFUL_THRESHOLD';
UPDATE Defines SET Value = '300' WHERE Name = 'ECONOMIC_STRENGTH_IMMENSE_THRESHOLD';

-- Skill Rating Calculation
INSERT INTO Defines (Name, Value) SELECT 'MILITARY_RATING_STARTING_VALUE', '1000';
INSERT INTO Defines (Name, Value) SELECT 'MILITARY_RATING_HUMAN_BUFFER_VALUE', '-20';

-- Target Value
UPDATE Defines SET Value = '0' WHERE Name = 'TARGET_MINOR_BACKUP_PATHETIC';
UPDATE Defines SET Value = '0' WHERE Name = 'TARGET_MINOR_BACKUP_WEAK';
UPDATE Defines SET Value = '0' WHERE Name = 'TARGET_MINOR_BACKUP_POOR';
UPDATE Defines SET Value = '15' WHERE Name = 'TARGET_MINOR_BACKUP_AVERAGE';
UPDATE Defines SET Value = '25' WHERE Name = 'TARGET_MINOR_BACKUP_STRONG';
UPDATE Defines SET Value = '35' WHERE Name = 'TARGET_MINOR_BACKUP_POWERFUL';
UPDATE Defines SET Value = '50' WHERE Name = 'TARGET_MINOR_BACKUP_IMMENSE';
UPDATE Defines SET Value = '100' WHERE Name = 'TARGET_MINOR_BACKUP_NEIGHBORS';
UPDATE Defines SET Value = '50' WHERE Name = 'TARGET_MINOR_BACKUP_CLOSE';
UPDATE Defines SET Value = '0' WHERE Name = 'TARGET_MINOR_BACKUP_FAR';
UPDATE Defines SET Value = '0' WHERE Name = 'TARGET_MINOR_BACKUP_DISTANT';
INSERT INTO Defines (Name, Value) SELECT 'TARGET_MAJOR_BACKUP_PATHETIC', '0';
INSERT INTO Defines (Name, Value) SELECT 'TARGET_MAJOR_BACKUP_WEAK', '5';
INSERT INTO Defines (Name, Value) SELECT 'TARGET_MAJOR_BACKUP_POOR', '10';
INSERT INTO Defines (Name, Value) SELECT 'TARGET_MAJOR_BACKUP_AVERAGE', '15';
INSERT INTO Defines (Name, Value) SELECT 'TARGET_MAJOR_BACKUP_STRONG', '25';
INSERT INTO Defines (Name, Value) SELECT 'TARGET_MAJOR_BACKUP_POWERFUL', '35';
INSERT INTO Defines (Name, Value) SELECT 'TARGET_MAJOR_BACKUP_IMMENSE', '50';
INSERT INTO Defines (Name, Value) SELECT 'TARGET_MAJOR_BACKUP_NEIGHBORS', '200';
INSERT INTO Defines (Name, Value) SELECT 'TARGET_MAJOR_BACKUP_CLOSE', '100';
INSERT INTO Defines (Name, Value) SELECT 'TARGET_MAJOR_BACKUP_FAR', '50';
INSERT INTO Defines (Name, Value) SELECT 'TARGET_MAJOR_BACKUP_DISTANT', '0';

-- Diplo Agreements
INSERT INTO Defines (Name, Value) SELECT 'COOP_WAR_DESIRE_THRESHOLD', '40'; -- min. GetCoopWarDesireScore() value for the AI to agree

-- "Too early for agreement" turn buffers
INSERT INTO Defines (Name, Value) SELECT 'JUST_MET_TURN_BUFFER', '10';
UPDATE Defines SET Value = '50' WHERE Name = 'DOF_TURN_BUFFER';
INSERT INTO Defines (Name, Value) SELECT 'DOF_TURN_BUFFER_REDUCTION_PER_ERA', '-5';


-- Warmonger Adjustments
-- Decay rate
UPDATE Defines SET Value = '-2' WHERE Name = 'WARMONGER_THREAT_PER_TURN_DECAY';

-- Multiplicative modifiers which increase or decrease warmonger penalty per turn decay rate
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_STRENGTH_DECAY_IMMENSE', '50';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_STRENGTH_DECAY_POWERFUL', '75';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_STRENGTH_DECAY_STRONG', '100';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_STRENGTH_DECAY_AVERAGE', '150';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_STRENGTH_DECAY_POOR', '200';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_STRENGTH_DECAY_WEAK', '250';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_STRENGTH_DECAY_PATHETIC', '300';

-- Weight of warmonger triggers
UPDATE Defines SET Value = '5' WHERE Name = 'WARMONGER_THREAT_MINOR_ATTACKED_WEIGHT';
UPDATE Defines SET Value = '10' WHERE Name = 'WARMONGER_THREAT_MAJOR_ATTACKED_WEIGHT';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_USED_NUKE_WEIGHT', '20';

UPDATE Defines SET Value = '100' WHERE Name = 'WARMONGER_THREAT_CITY_VALUE_MULTIPLIER'; -- > 100: each city is worth more ... < 100: each city is worth less (multiplicative)
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_CAPITAL_CITY_PERCENT', '150'; -- how much more the capital is worth (multiplicative)

-- Multiplicative modifier for when the bystander is on the same team as the defender, has a coop war with the defender against the attacker, or has a Defensive Pact with the defender
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_SHARED_FATE_PERCENT', '200';

-- Multiplicative modifier to the bonus for liberating a city owned by this player's team (added on top of SHARED_FATE_PERCENT)
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_LIBERATED_TEAM_BONUS_PERCENT', '200';

-- Multiplicative modifier for when the bystander doesn't know the defender, but does know the attacker
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_KNOWS_ATTACKER_PERCENT', '50';

-- Multiplicative modifier for when the bystander is in a coop war with the attacker against the defender (huge reduction!)
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_COOP_WAR_PERCENT', '10';

-- Additive modifiers which increase or decrease warmonger penalty based on how strong the attacker and defender are, compared to the bystander
-- Not applied if SHARED_FATE_PERCENT is applicable; values are inverted when liberating a city
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_ATTACKER_STRENGTH_IMMENSE', '100';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_ATTACKER_STRENGTH_POWERFUL', '75';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_ATTACKER_STRENGTH_STRONG', '50';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_ATTACKER_STRENGTH_AVERAGE', '33';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_ATTACKER_STRENGTH_POOR', '0';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_ATTACKER_STRENGTH_WEAK', '-25';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_ATTACKER_STRENGTH_PATHETIC', '-50';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_DEFENDER_STRENGTH_IMMENSE', '-75';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_DEFENDER_STRENGTH_POWERFUL', '-50';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_DEFENDER_STRENGTH_STRONG', '-25';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_DEFENDER_STRENGTH_AVERAGE', '0';

-- Additive modifiers for warmonger penalty based on various other conditions
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_MODIFIER_LARGE', '75';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_MODIFIER_MEDIUM', '50';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_MODIFIER_SMALL', '25';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_MODIFIER_NEGATIVE_SMALL', '-25';
INSERT INTO Defines(Name, Value) SELECT 'WARMONGER_THREAT_MODIFIER_NEGATIVE_MEDIUM', '-50';

-- Warmonger Percent by Era
UPDATE Eras Set WarmongerPercent = '50' WHERE Type = 'ERA_ANCIENT';
UPDATE Eras Set WarmongerPercent = '60' WHERE Type = 'ERA_CLASSICAL';
UPDATE Eras Set WarmongerPercent = '70' WHERE Type = 'ERA_MEDIEVAL';
UPDATE Eras Set WarmongerPercent = '80' WHERE Type = 'ERA_RENAISSANCE';
UPDATE Eras Set WarmongerPercent = '90' WHERE Type = 'ERA_INDUSTRIAL';
UPDATE Eras Set WarmongerPercent = '100' WHERE Type = 'ERA_MODERN';
UPDATE Eras Set WarmongerPercent = '110' WHERE Type = 'ERA_POSTMODERN';
UPDATE Eras Set WarmongerPercent = '120' WHERE Type = 'ERA_FUTURE';

-- Thresholds for warmonger threat levels (must have this much opinion penalty)
UPDATE Defines SET Value = '20' WHERE Name = 'WARMONGER_THREAT_MINOR_THRESHOLD';
UPDATE Defines SET Value = '80' WHERE Name = 'WARMONGER_THREAT_MAJOR_THRESHOLD';
UPDATE Defines SET Value = '140' WHERE Name = 'WARMONGER_THREAT_SEVERE_THRESHOLD';
UPDATE Defines SET Value = '200' WHERE Name = 'WARMONGER_THREAT_CRITICAL_THRESHOLD';
UPDATE Defines SET Value = '25' WHERE Name = 'WARMONGER_THREAT_SEVERE_PERCENT_THRESHOLD'; -- if you have conquered (killed, vassalized or captured capital) this % of major civs, threat is set to severe regardless of score. city-states count as half a major.
UPDATE Defines SET Value = '33' WHERE Name = 'WARMONGER_THREAT_CRITICAL_PERCENT_THRESHOLD'; -- if you have conquered (killed, vassalized or captured capital) this % of major civs, threat is set to critical regardless of score. city-states count as half a major.


-- War Progress Defines
-- Used as an AI sanity check to determine to prevent wars of excessive attrition in the opponent's favor.
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_INITIAL_VALUE', '100';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_DECAY_VS_STRONGER', '-5';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_DECAY_VS_EQUAL', '-4';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_DECAY_VS_WEAKER', '-3';

INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_CAPTURED_WORKER', '10';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_CAPTURED_SETTLER', '20';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_PLUNDERED_TRADE_ROUTE', '10';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_PILLAGED_IMPROVEMENT', '10';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_STOLE_TILE', '20';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_HIGH_VALUE_PILLAGE_MULTIPLIER', '200';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_KILLED_UNIT', '20';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_CAPTURED_CITY', '100';

INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_LOST_WORKER', '-5';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_LOST_SETTLER', '-10';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_LOST_TRADE_ROUTE', '-5';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_LOST_IMPROVEMENT', '-5';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_LOST_TILE', '-10';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_LOST_UNIT', '-10';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_LOST_CITY', '-50';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_PER_STRATEGIC_DEFICIT', '-5';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_PER_UNHAPPY', '-4';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_CAPITAL_MULTIPLIER', '200';
INSERT INTO Defines (Name, Value) SELECT 'WAR_PROGRESS_HOLY_CITY_MULTIPLIER', '150';

-- Peace Desire
UPDATE Defines SET Value = '20' WHERE Name = 'REQUEST_PEACE_TURN_THRESHOLD';

-- Peace Values (caps at 100)
UPDATE Defines SET Value = '100' WHERE Name = 'PEACE_WILLINGNESS_OFFER_THRESHOLD_UN_SURRENDER';
UPDATE Defines SET Value = '90' WHERE Name = 'PEACE_WILLINGNESS_OFFER_THRESHOLD_CAPITULATION';
UPDATE Defines SET Value = '80' WHERE Name = 'PEACE_WILLINGNESS_OFFER_THRESHOLD_CESSION';
UPDATE Defines SET Value = '70' WHERE Name = 'PEACE_WILLINGNESS_OFFER_THRESHOLD_SURRENDER';
UPDATE Defines SET Value = '60' WHERE Name = 'PEACE_WILLINGNESS_OFFER_THRESHOLD_SUBMISSION';
UPDATE Defines SET Value = '40' WHERE Name = 'PEACE_WILLINGNESS_OFFER_THRESHOLD_BACKDOWN';
UPDATE Defines SET Value = '30' WHERE Name = 'PEACE_WILLINGNESS_OFFER_THRESHOLD_SETTLEMENT';
UPDATE Defines SET Value = '15' WHERE Name = 'PEACE_WILLINGNESS_OFFER_THRESHOLD_ARMISTICE';

UPDATE Defines SET Value = '100' WHERE Name = 'PEACE_WILLINGNESS_ACCEPT_THRESHOLD_UN_SURRENDER';
UPDATE Defines SET Value = '90' WHERE Name = 'PEACE_WILLINGNESS_ACCEPT_THRESHOLD_CAPITULATION';
UPDATE Defines SET Value = '80' WHERE Name = 'PEACE_WILLINGNESS_ACCEPT_THRESHOLD_CESSION';
UPDATE Defines SET Value = '70' WHERE Name = 'PEACE_WILLINGNESS_ACCEPT_THRESHOLD_SURRENDER';
UPDATE Defines SET Value = '60' WHERE Name = 'PEACE_WILLINGNESS_ACCEPT_THRESHOLD_SUBMISSION';
UPDATE Defines SET Value = '40' WHERE Name = 'PEACE_WILLINGNESS_ACCEPT_THRESHOLD_BACKDOWN';
UPDATE Defines SET Value = '30' WHERE Name = 'PEACE_WILLINGNESS_ACCEPT_THRESHOLD_SETTLEMENT';
UPDATE Defines SET Value = '15' WHERE Name = 'PEACE_WILLINGNESS_ACCEPT_THRESHOLD_ARMISTICE';


-- CBO Defines

-- Happiness
INSERT INTO Defines (Name, Value) SELECT 'UNHAPPY_THRESHOLD', '50'; -- Unhappiness Threshold
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_WAR_WEARINESS_POPULATION_CAP', '34'; -- This is the % of empire population that war weariness is capped at.
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_HAPPINESS_LUXURY_POP_SCALER', '20'; -- avg pop to happiness conversion in 1/1000th; 100 means one tenth
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_HAPPINESS_THRESHOLD_PERCENTILE', '50'; -- Happiness
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_CORE_MUSICIAN_BLAST_HAPPINESS', '2'; -- Tourism Blast Happiness

-- These values modify empire-wide bonuses or penalties gained (or lost) from happiness. Change the values below, making sure to keep the integers + or - as they are below.
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_HAPPINESS_FOOD_MODIFIER', '2'; -- Food % point per happiness mod (should always be a positive value).
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_UNHAPPINESS_FOOD_MODIFIER', '10';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_HAPPINESS_PRODUCTION_MODIFIER', '10'; -- Production % point per happiness mod (should always be a positive value).

-- Maximum happiness penalty % mod. (Should always be a negative value)
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_HAPPINESS_PENALTY_MAXIMUM', '-75';
INSERT INTO Defines (Name, Value) SELECT 'UNHAPPY_PRODUCTION_PENALTY', '-25';
INSERT INTO Defines (Name, Value) SELECT 'VERY_UNHAPPY_PRODUCTION_PENALTY', '-50';

-- City Happiness Defines
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_HAPPINESS_PUPPET_THRESHOLD_MOD', '4'; -- Puppets produce flat unhappiness based on # of citizens in the city. Divisor is this, never set to zero.
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_HAPPINESS_EMPIRE_MULTIPLIER', '10'; -- Per non-puppet city % modifier for unhappiness thresholds (i.e. # cities * value below = % modifier).
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_UNHAPPINESS_PER_SPECIALIST', '100'; -- 100 = 1 unhappiness per specialist.
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_HAPPINESS_CAPITAL_MODIFIER', '25'; -- Base Modifier for Capital Thresholds. Offsets boost from Palace, helps make Capital a source of Unhappiness early on. 25% is default.
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_HAPPINESS_TECH_BASE_MODIFIER', '160'; -- 	Base Value of Tech - % of techs researched v. techs known, multiplied by this value. 100 is default (each tech increases % by fraction of techs remaining v. techs researched).
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_HAPPINESS_POP_MULTIPLIER', '-50'; -- Base increase of threshold values based on # of citizens in cities you own (1 citizen = 1%). Modifier increases as cities grow. 50 is default. Is a % modifier, so 25 would be a 25% increase over 1%.
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_UNHAPPY_CITY_BASE_VALUE_BOREDOM', '50'; -- Value by which yield/threshold difference is modified based on the remaining population
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_UNHAPPY_CITY_BASE_VALUE_ILLITERACY', '50'; -- Value by which yield/threshold difference is modified based on the remaining population
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_UNHAPPY_CITY_BASE_VALUE_DISORDER', '200'; -- Value by which yield/threshold difference is modified based on the remaining population
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_UNHAPPY_CITY_BASE_VALUE_POVERTY', '50'; -- Value by which yield/threshold difference is modified based on the remaining population
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_UNHAPPINESS_PER_MINORITY_POP', '0.5'; -- Unhappiness point per religious minority pop. A high faith to population ratio will reduce this penalty. Also note that this is the ONLY unhappiness calculation that goes down as the game progresses (religion makes slightly less unhappiness as you move into new eras)
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_UNHAPPINESS_FROM_STARVING_PER_POP', '1.0'; -- Unhappiness point per starving citizen.
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_UNHAPPINESS_PER_PILLAGED', '0.50'; -- Unhappiness point per pillaged plot owned by city.
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_UNHAPPINESS_FROM_UNCONNECTED_PER_POP', '0.34'; -- Unhappiness point per pop if unconnected or blockaded.


-- Tourism
-- These 5 values change the amount of gold earned from trade routes with influenced civs. Higher influence = more gold (trade-off for higher influence).
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_GOLD_INFLUENCE_LEVEL_EXOTIC', '200';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_GOLD_INFLUENCE_LEVEL_FAMILIAR', '400';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_GOLD_INFLUENCE_LEVEL_POPULAR', '600';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_GOLD_INFLUENCE_LEVEL_INFLUENTIAL', '800';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_GOLD_INFLUENCE_LEVEL_DOMINANT', '1000';

-- These 5 values change the amount of Food earned from trade routes with influenced civs. Higher influence = more Food (trade-off for higher influence). 
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_GROWTH_INFLUENCE_LEVEL_EXOTIC', '5';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_GROWTH_INFLUENCE_LEVEL_FAMILIAR', '10';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_GROWTH_INFLUENCE_LEVEL_POPULAR', '15';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_GROWTH_INFLUENCE_LEVEL_INFLUENTIAL', '20';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_GROWTH_INFLUENCE_LEVEL_DOMINANT', '25';

--- These 5 values change the amount of science earned from trade routes with influence civs. Each is adding to an incremental value (so Familiar is already 1, Popular already 2, etc.)
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_SCIENCE_INFLUENCE_LEVEL_EXOTIC', '0';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_SCIENCE_INFLUENCE_LEVEL_FAMILIAR', '1';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_SCIENCE_INFLUENCE_LEVEL_POPULAR', '2';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_SCIENCE_INFLUENCE_LEVEL_INFLUENTIAL', '3';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_SCIENCE_INFLUENCE_LEVEL_DOMINANT', '4';

--- Reduce turns for conquest of cities. Base is 25 for each level of influence, this adds to it.
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_CONQUEST_REDUCTION_BOOST', '0';

--- Set turns for spies to est. in cities based on influence.
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_SPY_BOOST_INFLUENCE_EXOTIC', '5';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_SPY_BOOST_INFLUENCE_FAMILIAR', '4';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_SPY_BOOST_INFLUENCE_POPULAR', '3';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_SPY_BOOST_INFLUENCE_INFLUENTIAL', '2';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_SPY_BOOST_INFLUENCE_DOMINANT', '1';


-- Wonder Consolation Tweaks
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_WONDER_BEATEN_CONSOLATION_PRIZE', '2'; -- Consolation prize of Culture (default)
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_CULTURE_PERCENTAGE_VALUE', '33'; -- % of Production awarded as Culture if consolation prize is Culture
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_GA_PERCENTAGE_VALUE', '25'; -- % of Production awarded as GAP if consolation prize is GAP
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_SCIENCE_PERCENTAGE_VALUE', '10'; -- % of Production awarded as Science if consolation prize is Science
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_FAITH_PERCENTAGE_VALUE', '10'; -- % of Production awarded as Faith if consolation prize is Faith


-- Barbarians
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_BARBARIAN_HEAL_RATE', '0';

-- Misc. Defines
INSERT INTO Defines (Name, Value) SELECT 'RELIGION_MIN_FAITH_SECOND_PROPHET', '600';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_MARRIAGE_GP_RATE', '15'; -- Austria new UA (CBO)
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_FOLLOWER_GROWTH_BONUS', '2'; -- India Growth (CBO)
INSERT INTO Defines (Name, Value) SELECT 'GWAM_THRESHOLD_DECREASE', '0'; -- Great People Rate Mod (Note, this is a subtraction, so positive = negative)
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_BUILDING_INVESTMENT_BASELINE', '-50'; -- Building Investments Base Rate
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_UNIT_INVESTMENT_BASELINE', '-50'; -- Unit Investments Base Rate
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_CORE_PRODUCTION_DESERT_IMPROVEMENT', '0';

-- Settlers
-- Pioneers/Colonists
INSERT INTO Defines (Name, Value) SELECT 'PIONEER_POPULATION_CHANGE', '3';
INSERT INTO Defines (Name, Value) SELECT 'PIONEER_EXTRA_PLOTS', '3';
INSERT INTO Defines (Name, Value) SELECT 'PIONEER_FOOD_PERCENT', '25';
INSERT INTO Defines (Name, Value) SELECT 'COLONIST_POPULATION_CHANGE', '5';
INSERT INTO Defines (Name, Value) SELECT 'COLONIST_EXTRA_PLOTS', '5';
INSERT INTO Defines (Name, Value) SELECT 'COLONIST_FOOD_PERCENT', '50';

-- Scouts
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_SCOUT_XP_RANDOM_VALUE', '12';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_SCOUT_XP_BASE', '1';

-- City-States
INSERT INTO Defines (Name, Value) SELECT 'QUEST_DISABLED_CP_QUESTS', '1'; -- New CBP Quests
INSERT INTO Defines (Name, Value) SELECT 'MOD_BALANCE_CORE_MINIMUM_RANKING_PTP', '60'; -- PTP Military Power Needed
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_CS_WAR_COOLDOWN_RATE', '50'; -- How long till a City-State forgets that a major aggressively attacked it?
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_INFLUENCE_BOOST_PROTECTION_MINOR', '0';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_MINOR_PROTECTION_MINIMUM_DURATION', '10'; -- How long before a major can revoke a PtP to a CS?
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_CS_PLEDGE_TO_PROTECT_DEFENSE_BONUS', '5'; -- Bonus to CS capital's CS for each PtP
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_CS_PLEDGE_TO_PROTECT_DEFENSE_BONUS_MAX', '25'; -- Max bonus to CS capital's CS from PtPs
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_CS_ALLIANCE_DEFENSE_BONUS', '25'; -- Bonus to CS capital's CS for having an ally
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_MAX_CS_ALLY_STRENGTH', '5'; -- Max number of CSs calc'd for bonus for Greek UA (CBO)
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_SPY_TO_MINOR_RATIO', '10';

-- Spies
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_SPY_SABOTAGE_RATE', '30';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_SPY_RESPAWN_TIMER', '5';
INSERT INTO Defines (Name, Value) SELECT 'ESPIONAGE_GATHERING_INTEL_COST_DIVISOR', '250';

-- Ideology Unlock via Policies
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_MOD_POLICY_BRANCHES_NEEDED_IDEOLOGY', '3';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_MOD_POLICIES_NEEDED_IDEOLOGY', '18';

-- Resource Monopolies
INSERT INTO Defines (Name, Value) SELECT 'GLOBAL_RESOURCE_MONOPOLY_THRESHOLD', '50';
INSERT INTO Defines (Name, Value) SELECT 'STRATEGIC_RESOURCE_MONOPOLY_THRESHOLD', '25';

-- Corporations
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_CORE_CORP_OFFICE_FRANCHISE_CONVERSION', '0.5';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_CORE_CORP_OFFICE_TR_CONVERSION', '1.0';

-- Increasing World Wonder Production costs
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_CORE_WORLD_WONDER_SAME_ERA_COST_MODIFIER', '25';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_CORE_WORLD_WONDER_PREVIOUS_ERA_COST_MODIFIER', '15';
INSERT INTO Defines (Name, Value) SELECT 'BALANCE_CORE_WORLD_WONDER_EARLIER_ERA_COST_MODIFIER', '10';

-- FOR JFD
INSERT INTO Defines (Name, Value) SELECT 'UNHAPPINESS_PER_POPULATION_FLOAT', '0.0';


-- CSD Defines
-- Disable Gold Gifts
INSERT INTO Defines (Name, Value) SELECT 'CSD_GOLD_GIFT_DISABLED', '0';

-- Quest stuff
INSERT INTO Defines (Name, Value) SELECT 'QUEST_DISABLED_WAR', '0';
INSERT INTO Defines (Name, Value) SELECT 'QUEST_DISABLED_INFLUENCE', '0';
INSERT INTO Defines (Name, Value) SELECT 'QUEST_DISABLED_FIND_CITY_STATE', '0';
INSERT INTO Defines (Name, Value) SELECT 'QUEST_DISABLED_TOURISM', '0';
INSERT INTO Defines (Name, Value) SELECT 'QUEST_DISABLED_ARCHAEOLOGY', '0';
INSERT INTO Defines (Name, Value) SELECT 'QUEST_DISABLED_CIRCUMNAVIGATION', '0';
INSERT INTO Defines (Name, Value) SELECT 'QUEST_DISABLED_LIBERATION', '0';
INSERT INTO Defines (Name, Value) SELECT 'QUEST_DISABLED_HORDE', '0';
INSERT INTO Defines (Name, Value) SELECT 'QUEST_DISABLED_REBELLION', '0';

INSERT INTO Defines (Name, Value) SELECT 'MINOR_QUEST_REBELLION_TIMER', '20';
INSERT INTO Defines (Name, Value) SELECT 'INFLUENCE_MINOR_QUEST_BOOST', '20';

-- Quest proclivity values
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_KILL_CITY_STATE', '100';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_IRRATIONAL_KILL_CITY_STATE', '75';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_BULLY_CITY_STATE', '200';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_IRRATIONAL_BULLY_CITY_STATE', '150';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_BULLY_CITY_STATE', '30';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_DENOUNCE_MAJOR', '250';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_DENOUNCE_MAJOR', '200';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_DENOUNCE_MAJOR', '150';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_SPREAD_RELIGION', '300';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_TRADE_ROUTE', '200';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_TRADE_ROUTE', '200';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_DISCOVER_PLOT', '275';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_DISCOVER_PLOT', '200';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_DISCOVER_PLOT', '50';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_BUILD_BUILDINGS', '200';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_BUILD_BUILDINGS', '200';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_BUILD_BUILDINGS', '150';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_SPY_ON_MAJOR', '350';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_SPY_ON_MAJOR', '50';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_SPY_ON_MAJOR', '125';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_SPY_ON_MAJOR', '50';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_SPY_ON_MAJOR', '75';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_COUP', '400';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_COUP', '75';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_COUP', '175';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_COUP', '50';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_COUP', '150';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_CAPTURE_CITY', '500';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_CAPTURE_CITY', '300';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_CAPTURE_CITY', '200';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_CAPTURE_CITY', '25';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_CAPTURE_CITY', '25';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_WAR', '800';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_WAR', '600';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_WAR', '500';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_WAR', '125';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_NATIONAL_WONDER', '300';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_NATIONAL_WONDER', '75';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_FIND_CITY_STATE', '115';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_FIND_CITY_STATE', '120';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_ARCHAEOLOGY', '250';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_ARCHAEOLOGY', '300';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_ARCHAEOLOGY', '75';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_CIRCUMNAVIGATION', '250';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_LIBERATION', '400';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_LIBERATION', '600';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_LIBERATION', '500';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_KILL_CAMP', '300';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_LIBERATION', '50';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_CONTEST_CULTURE', '200';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_CONTEST_FAITH', '200';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_CONTEST_FAITH', '50';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_CONTEST_TECHS', '50';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_INVEST', '150';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_INFLUENCE', '50';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_INFLUENCE', '70';
INSERT INTO Defines (Name, Value) SELECT 'MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_TOURISM', '130';
INSERT INTO Defines (Name, Value) SELECT 'BARBARIAN_HORDE_FREQUENCY', '300';
INSERT INTO Defines (Name, Value) SELECT 'QUEST_REBELLION_FREQUENCY', '700';

-- Warmongering
INSERT INTO Defines (Name, Value) SELECT 'WARMONGER_THREAT_PER_TURN_DECAY_INCREASED', '200';
INSERT INTO Defines (Name, Value) SELECT 'WARMONGER_THREAT_PER_TURN_DECAY_DECREASED', '50';
INSERT INTO Defines (Name, Value) SELECT 'WARMONGER_THREAT_ATTACKED_WEIGHT_WORLD_WAR', '50';
INSERT INTO Defines (Name, Value) SELECT 'WARMONGER_THREAT_ATTACKED_WEIGHT_WORLD_PEACE', '200';
INSERT INTO Defines (Name, Value) SELECT 'WARMONGER_THREAT_ATTACKED_SANCTIONED_PLAYER', '50';

-- Embassy
INSERT INTO Defines (Name, Value) SELECT 'EMBASSY_IMPROVEMENT', '1';

-- Military AI
INSERT INTO Defines (Name, Value) SELECT 'AI_MILITARY_RECAPTURING_CITY_STATE', '200';

-- Diplomat AI
INSERT INTO Defines (Name, Value) SELECT 'INFLUENCE_TARGET_DISTANCE_WEIGHT_VALUE', '3';
INSERT INTO Defines (Name, Value) SELECT 'NEED_DIPLOMAT_THRESHOLD_MODIFIER', '125'; -- Higher Threshold = Lower diplomacy desire.
INSERT INTO Defines (Name, Value) SELECT 'NEED_DIPLOMAT_DESIRE_MODIFIER', '2'; -- Higher Desire = Higher diplomacy desire.
INSERT INTO Defines (Name, Value) SELECT 'NEED_DIPLOMAT_DISTASTE_MODIFIER', '6'; -- Lower distaste = More sensitive to other player's diplomatic actions and/or city-state abuse.

-- World Congress Stuff
INSERT INTO Defines (Name, Value) SELECT 'LEAGUE_PROPOSERS_PER_SESSION', '2';
INSERT INTO Defines (Name, Value) SELECT 'LEAGUE_NUM_LEADERS_FOR_EXTRA_VOTES', '2';
INSERT INTO Defines (Name, Value) SELECT 'LEAGUE_PROJECT_PROGRESS_PERCENT_WARNING', '33';
INSERT INTO Defines (Name, Value) SELECT 'LEAGUE_AID_MAX', '30'; -- Max Arts/Sciences % Boost - 1/3 of this is minimum % boost.
INSERT INTO Defines (Name, Value) SELECT 'SCHOLAR_MINOR_ALLY_MULTIPLIER', '2';
INSERT INTO Defines (Name, Value) SELECT 'SCIENCE_LEAGUE_GREAT_WORK_MODIFIER', '1';


-- C4DF Values
-- Share Opinion
INSERT INTO Defines (Name, Value) SELECT 'SHARE_OPINION_TURN_BUFFER', '20'; -- How many turns after we meet a civilization must we wait before they'll share their opinion?

-- Help Requests
INSERT INTO Defines (Name, Value) SELECT 'HELP_REQUEST_TURN_LIMIT_MIN', '20'; -- Base turns before Help Request can be accepted again.
INSERT INTO Defines (Name, Value) SELECT 'HELP_REQUEST_TURN_LIMIT_RAND', '10'; -- Randomized number of turns before Help Request can be accepted again. Added to base turns.

-- Tech Trading
INSERT INTO Defines (Name, Value) SELECT 'TECH_COST_ERA_EXPONENT', '0.7'; -- Additional cost per era.

-- Vassalage
INSERT INTO Defines (Name, Value) SELECT 'VASSAL_HAPPINESS_PERCENT', '20'; -- What % of the vassal's Happiness does the master get? (NOTE: Halved in CBO)
INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_FREE_YIELD_FROM_VASSAL_PERCENT', '20'; -- What % of the vassal's Science/Culture/Faith does the master get?
INSERT INTO Defines (Name, Value) SELECT 'VASSAL_TOURISM_MODIFIER', '33'; -- What % bonus does the master get to Tourism against the vassal?
INSERT INTO Defines (Name, Value) SELECT 'VASSAL_SCORE_PERCENT', '50'; -- What % of the vassal's Land/Population score does the master get?
INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_VASSAL_TAX_PERCENT_MINIMUM', '0'; -- Minimum percent we can tax a vassal's income. Negative values may cause undefined behavior.
INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_VASSAL_TAX_PERCENT_MAXIMUM', '25'; -- Maximum percent we can tax a vassal's income. Values greater than 100% will do nothing. Make sure this is a multiple of 5 please!

INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_VASSAL_CITY_POP_EXPONENT', '0.8'; -- How much does each vassal citizen affect the master's maintenance costs?
INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_VASSAL_UNIT_MAINT_COST_PERCENT', '10'; -- How much does the master pay for the vassal's units?

INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_VASSAL_LOST_CITIES_THRESHOLD', '75'; -- Percentage of vassal cities that must be lost before Vassalage can be ended.
INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_VASSAL_POPULATION_THRESHOLD', '300'; -- Percentage of population over what the vassal started with before Vassalage can be ended.
INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_VASSAL_MASTER_CITY_PERCENT_THRESHOLD', '60';
INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_VASSAL_MASTER_POP_PERCENT_THRESHOLD', '60';

INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_CAPITULATE_BASE_THRESHOLD', '100'; -- How likely is a vassal to voluntarily capitulate?


-- Misc. Modmod Defines
INSERT INTO Defines (Name, Value) SELECT 'FRIENDSHIP_THRESHOLD_MOD_MEDIEVAL', '6';
INSERT INTO Defines (Name, Value) SELECT 'FRIENDSHIP_THRESHOLD_MOD_INDUSTRIAL', '10';
INSERT INTO Defines (Name, Value) SELECT 'CITY_STATE_SCALE_PER_CITY_MOD', '0';