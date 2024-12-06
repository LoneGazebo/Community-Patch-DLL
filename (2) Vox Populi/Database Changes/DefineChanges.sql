-- General Changes to the DLL's Global Values

-------------------------------------------------------------------------------------------------------------------
-- Game Options
-------------------------------------------------------------------------------------------------------------------

-- If > -1, a player with no cities but who is still alive (Complete Kills) will be granted a free Settler after this many turns
UPDATE Defines SET Value = 10 WHERE Name = 'COMPLETE_KILLS_TURN_TIMER';

-------------------------------------------------------------------------------------------------------------------
-- Defensive Pact
-------------------------------------------------------------------------------------------------------------------

-- The base number of Defensive Pacts that a civilization can form.
-- If set to 0 or lower, Defensive Pacts cannot be formed.
-- Each player on a team counts as 1 Defensive Pact.
UPDATE Defines SET Value = 2 WHERE Name = 'DEFENSIVE_PACT_LIMIT_BASE';

-- Additional number of Defensive Pacts permitted for each X other major civilizations ingame. 0 disables the scaler.
UPDATE Defines SET Value = 10 WHERE Name = 'DEFENSIVE_PACT_LIMIT_SCALER';

-- Limit for AI players. Only does anything if lower than the base limit.
UPDATE Defines SET Value = 2 WHERE Name = 'AI_DEFENSIVE_PACT_LIMIT_BASE';
UPDATE Defines SET Value = 10 WHERE Name = 'AI_DEFENSIVE_PACT_LIMIT_SCALER';

-------------------------------------------------------------------------------------------------------------------
-- War
-------------------------------------------------------------------------------------------------------------------

-- Base city HP
UPDATE Defines SET Value = 250 WHERE Name = 'MAX_CITY_HIT_POINTS';

-- Extra city HP per population
UPDATE Defines SET Value = 8 WHERE Name = 'CITY_STRENGTH_POPULATION_CHANGE';

-- City's base healing amount. Can go higher if no enemy is nearby.
UPDATE Defines SET Value = 8 WHERE Name = 'CITY_HIT_POINTS_HEALED_PER_TURN';

-- Base city strength x100
UPDATE Defines SET Value = 500 WHERE Name = 'CITY_STRENGTH_DEFAULT';

-- Extra city strength if settled on hills x100
UPDATE Defines SET Value = 200 WHERE Name = 'CITY_STRENGTH_HILL_CHANGE';

-- Divisor of garrison strength added to city strength x100
UPDATE Defines SET Value = 200 WHERE Name = 'CITY_STRENGTH_UNIT_DIVISOR';

-- Extra city strength from tech progress
-- Only used if BALANCE_CORE_CITY_DEFENSE_SWITCH is on
UPDATE Defines SET Value = 3.6 WHERE Name = 'CITY_STRENGTH_TECH_BASE';
UPDATE Defines SET Value = 2.0 WHERE Name = 'CITY_STRENGTH_TECH_EXPONENT';
UPDATE Defines SET Value = 0 WHERE Name = 'CITY_STRENGTH_TECH_MULTIPLIER';

-- Base city strike strength modifier (relative to city strength), before other modifiers
UPDATE Defines SET Value = 0 WHERE Name = 'CITY_RANGED_ATTACK_STRENGTH_MULTIPLIER';

-- Heal rate
UPDATE Defines SET Value = 5 WHERE Name = 'ENEMY_HEAL_RATE';
UPDATE Defines SET Value = 10 WHERE Name = 'NEUTRAL_HEAL_RATE';
UPDATE Defines SET Value = 15 WHERE Name = 'FRIENDLY_HEAL_RATE';
UPDATE Defines SET Value = 20 WHERE Name = 'CITY_HEAL_RATE';

UPDATE Defines SET Value = 10 WHERE Name = 'HILLS_EXTRA_DEFENSE';

-- Base number of air slots per city
UPDATE Defines SET Value = 2 WHERE Name = 'BASE_CITY_AIR_STACKING';

-- Multiplier of aircraft range for rebase x100
UPDATE Defines SET Value = 500 WHERE Name = 'AIR_UNIT_REBASE_RANGE_MULTIPLIER';

-- % population retained if city is captured. Modified by cultural influence.
UPDATE Defines SET Value = 75 WHERE Name = 'CITY_CAPTURE_POPULATION_PERCENT';

-- Additional % of population loss and resistance time reduced on top of cultural influence
UPDATE Defines SET Value = 5 WHERE Name = 'BALANCE_CONQUEST_REDUCTION_BOOST';

-- XP gained by attacking a city with ranged
UPDATE Defines SET Value = 2 WHERE Name = 'EXPERIENCE_ATTACKING_CITY_RANGED';

-- % HP healed for captured enemy unit
UPDATE Defines SET Value = 75 WHERE Name = 'COMBAT_CAPTURE_HEALTH';

-- Naval unit blockade range (land blockade is set in CustomModOptions ADJACENT_BLOCKADE)
UPDATE Defines SET Value = 1 WHERE Name = 'NAVAL_PLOT_BLOCKADE_RANGE';

-- Coastal city gold per turn penalty if all WATER tiles are blockaded 
UPDATE Defines SET Value = -25 WHERE Name = 'BLOCKADE_GOLD_PENALTY';

-- Bonus combat strength against a blockaded city (on BOTH land and water)
UPDATE Defines SET Value = 20 WHERE Name = 'BLOCKADED_CITY_ATTACK_MODIFIER';

-- Bonus combat strength against a sapped city
UPDATE Defines SET Value = 40 WHERE Name = 'SAPPED_CITY_ATTACK_MODIFIER';

-- Unused
UPDATE Defines SET Value = 2 WHERE Name = 'HEAVY_RESOURCE_THRESHOLD';

-- Vassalage
UPDATE Defines SET Value = 10 WHERE Name = 'VASSAL_HAPPINESS_PERCENT'; -- (NOTE: Halved in VP) comment in CoreDefines.sql of (1), updated because DLL hid that it was halving it.

-------------------------------------------------------------------------------------------------------------------
-- City stuff
-------------------------------------------------------------------------------------------------------------------

-- Excess food needed for next population
UPDATE Defines SET Value = 2.22 WHERE Name = 'CITY_GROWTH_EXPONENT';
UPDATE Defines SET Value = 12.0 WHERE Name = 'CITY_GROWTH_MULTIPLIER';

-- Border growth cost
UPDATE Defines SET Value = 20 WHERE Name = 'CULTURE_COST_FIRST_PLOT';
UPDATE Defines SET Value = 12 WHERE Name = 'CULTURE_COST_LATER_PLOT_MULTIPLIER';
UPDATE Defines SET Value = 1.35 WHERE Name = 'CULTURE_COST_LATER_PLOT_EXPONENT';

-- % Extra gold cost to buy a resource tile
UPDATE Defines SET Value = 0 WHERE Name = 'PLOT_BUY_RESOURCE_COST';

-- Population required for training settlers
UPDATE Defines SET Value = 4 WHERE Name = 'CITY_MIN_SIZE_FOR_SETTLERS';

-------------------------------------------------------------------------------------------------------------------
-- World Wonders
-------------------------------------------------------------------------------------------------------------------

-- Consolation yields for losing a World Wonder race
UPDATE Defines
SET Value = (
	SELECT Value FROM Community
	WHERE Type = 'COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK'
) WHERE Name = 'BALANCE_WONDER_BEATEN_CONSOLATION_PRIZE';

-------------------------------------------------------------------------------------------------------------------
-- Units/Supply
-------------------------------------------------------------------------------------------------------------------

-- Base movement for embarked unit. Can be modified by traits, promotions, tech and policies.
UPDATE Defines SET Value = 3 WHERE Name = 'EMBARKED_UNIT_MOVEMENT';

-- Production% penalty per unit over supply limit, capped at 70% (MAX_UNIT_SUPPLY_PRODMOD).
UPDATE Defines SET Value = 5 WHERE Name = 'PRODUCTION_PENALTY_PER_UNIT_OVER_SUPPLY';

-- Growth% penalty per unit over supply limit, capped at 70% (MAX_UNIT_SUPPLY_GROWTH_MOD).
UPDATE Defines SET Value = 5 WHERE Name = 'GROWTH_PENALTY_PER_UNIT_OVER_SUPPLY';

-- Supply from difficulty
-- Base supply -= EraID * UNIT_SUPPLY_BASE_TECH_REDUCTION_PER_ERA / 100
UPDATE Defines SET Value = 100 WHERE Name = 'UNIT_SUPPLY_BASE_TECH_REDUCTION_PER_ERA';

-- Supply from city
-- City supply /= (1 + Tech progress * UNIT_SUPPLY_CITIES_TECH_REDUCTION_MULTIPLIER / 100)
UPDATE Defines SET Value = 100 WHERE Name = 'UNIT_SUPPLY_CITIES_TECH_REDUCTION_MULTIPLIER';

-- Supply from population
-- % provided by puppet city population
UPDATE Defines SET Value = 50 WHERE Name = 'UNIT_SUPPLY_POPULATION_PUPPET_PERCENT';

-- Population supply /= (1 + Tech progress * UNIT_SUPPLY_POPULATION_TECH_REDUCTION_MULTIPLIER / 100)
UPDATE Defines SET Value = 100 WHERE Name = 'UNIT_SUPPLY_POPULATION_TECH_REDUCTION_MULTIPLIER';

-- Maximum XP from combat vs barbarian/CS
UPDATE Defines SET Value = 45 WHERE Name = 'BARBARIAN_MAX_XP_VALUE';
UPDATE Defines SET Value = 70 WHERE Name = 'MINOR_MAX_XP_VALUE';

-- More maintenance if this value is increased
UPDATE Defines SET Value = 8 WHERE Name = 'UNIT_MAINTENANCE_GAME_MULTIPLIER';

-- More maintenance if this value is lowered
UPDATE Defines SET Value = 5 WHERE Name = 'UNIT_MAINTENANCE_GAME_EXPONENT_DIVISOR';

-- Combat penalty if out of strategic resource
UPDATE Defines SET Value = 0 WHERE Name = 'STRATEGIC_RESOURCE_EXHAUSTED_PENALTY';

-- Number of unexplored plots required per recon unit, used by AI
-- Modified by personality!
UPDATE Defines SET Value = 27 WHERE Name = 'MAX_PLOTS_PER_EXPLORER';

UPDATE Defines SET Value = 1 WHERE Name = 'UNIT_UPGRADE_COST_PER_PRODUCTION';

-- Maximum discount you can get for stacking unit upgrade discounts, so the number doesn't go negative
UPDATE Defines SET Value = -99 WHERE Name = 'UNIT_UPGRADE_COST_DISCOUNT_MAX';
UPDATE Defines SET Value = 0.68 WHERE Name = 'HURRY_GOLD_PRODUCTION_EXPONENT';

-------------------------------------------------------------------------------------------------------------------
-- Minors
-- Defines for CS friend/ally yields and quest yields are in MinorYieldChanges.sql and MinorQuestChanges.sql
-------------------------------------------------------------------------------------------------------------------
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_MERCANTILE_RESOURCES_KEEP_ON_CAPTURE_DISABLED';
UPDATE Defines SET Value = 50 WHERE Name = 'BALANCE_MINOR_PROTECTION_MINIMUM_DURATION';
UPDATE Defines SET Value = 100 WHERE Name = 'MINOR_POLICY_RESOURCE_HAPPINESS_MULTIPLIER';
UPDATE Defines SET Value = 60 WHERE Name = 'MINOR_CIV_TECH_PERCENT';
UPDATE Defines SET Value = 115 WHERE Name = 'MINOR_CIV_PLOT_CULTURE_COST_MULTIPLIER';

-- Gold tribute amount increasing with game progress
UPDATE Defines SET Value = 1000 WHERE Name = 'MINOR_BULLY_GOLD_GROWTH_FACTOR';

-- Bully score threshold for demanding tribute
UPDATE Defines SET Value = 20 WHERE Name = 'MINOR_CIV_GOLD_TRIBUTE_THRESHOLD';
UPDATE Defines SET Value = 100 WHERE Name = 'MINOR_CIV_HEAVY_TRIBUTE_THRESHOLD';

-- Influence loss x100 if pledged CS is damaged
UPDATE Defines SET Value = 300 WHERE Name = 'MINOR_FRIENDSHIP_DROP_PER_TURN_DAMAGED_CAPITAL_MULTIPLIER';

-- Number of turns a pledge of protection is blocked after breaking it by bullying
UPDATE Defines SET Value = 30 WHERE Name = 'PLEDGE_BROKEN_MINIMUM_TURNS_BULLYING';

-- Resting influence boosts for liberation and building Landmarks
UPDATE Defines SET Value = 100 WHERE Name = 'MINOR_LIBERATION_RESTING_INFLUENCE';
UPDATE Defines SET Value = 10 WHERE Name = 'MINOR_LANDMARK_RESTING_INFLUENCE'; -- scales with era

-- Bonus to current Influence for building Landmarks (scales with era in VP)
UPDATE Defines SET Value = 0 WHERE Name = 'LANDMARK_MINOR_FRIENDSHIP_CHANGE';

-- % extra city strength if the CS has an ally
UPDATE Defines SET Value = 25 WHERE Name = 'BALANCE_CS_ALLIANCE_DEFENSE_BONUS';

-- Influence gained by performing trade mission
-- Used by Great Merchant only; diplo units use promotions instead
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_FRIENDSHIP_FROM_TRADE_MISSION';

-- Influence dropped by tributing x100
UPDATE Defines SET Value = -3000 WHERE Name = 'MINOR_FRIENDSHIP_DROP_BULLY_GOLD_SUCCESS';
UPDATE Defines SET Value = -6000 WHERE Name = 'MINOR_FRIENDSHIP_DROP_BULLY_WORKER_SUCCESS';

UPDATE Defines SET Value = -5 WHERE Name = 'FRIENDSHIP_PER_UNIT_INTRUDING';
UPDATE Defines SET Value = 15 WHERE Name = 'FRIENDSHIP_PER_BARB_KILLED';
UPDATE Defines SET Value = 50 WHERE Name = 'RETURN_CIVILIAN_FRIENDSHIP';

-- Base influence gained per unit gifted, -1 for each unit the CS already has
UPDATE Defines SET Value = 15 WHERE Name = 'FRIENDSHIP_PER_UNIT_GIFTED';

-- Weights for how much the CS wants to give out each quest
-- Personal Quests
UPDATE Defines SET Value = 20 WHERE Name = 'MINOR_CIV_QUEST_ROUTE_COPIES_MERCANTILE';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_ROUTE_COPIES_RELIGIOUS';
UPDATE Defines SET Value = 20 WHERE Name = 'MINOR_CIV_QUEST_ROUTE_COPIES_FRIENDLY';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_ROUTE_COPIES_HOSTILE';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_CONNECT_RESOURCE_COPIES_MARITIME';
UPDATE Defines SET Value = 20 WHERE Name = 'MINOR_CIV_QUEST_CONNECT_RESOURCE_COPIES_FRIENDLY';
UPDATE Defines SET Value = 80 WHERE Name = 'MINOR_CIV_QUEST_CONNECT_RESOURCE_COPIES_HYPERLINK'; -- Mercantile + Friendly
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_CONSTRUCT_WONDER_COPIES_CULTURED';
UPDATE Defines SET Value = 20 WHERE Name = 'MINOR_CIV_QUEST_CONSTRUCT_WONDER_COPIES_RELIGIOUS';
UPDATE Defines SET Value = 80 WHERE Name = 'MINOR_CIV_QUEST_CONSTRUCT_WONDER_COPIES_HYPERLINK'; -- Religious + Hostile
UPDATE Defines SET Value = 80 WHERE Name = 'MINOR_CIV_QUEST_GREAT_PERSON_COPIES_HYPERLINK'; -- Cultured + Friendly
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_FIND_PLAYER_COPIES_MERCANTILE';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_FIND_NATURAL_WONDER_COPIES_MILITARISTIC';
UPDATE Defines SET Value = 20 WHERE Name = 'MINOR_CIV_QUEST_FIND_NATURAL_WONDER_COPIES_MARITIME';
UPDATE Defines SET Value = 20 WHERE Name = 'MINOR_CIV_QUEST_FIND_NATURAL_WONDER_COPIES_RELIGIOUS';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_FIND_NATURAL_WONDER_COPIES_HOSTILE';
UPDATE Defines SET Value = 10 WHERE Name = 'MINOR_CIV_QUEST_GIVE_GOLD_COPIES_BASE';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_GIVE_GOLD_COPIES_MILITARISTIC';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_GIVE_GOLD_COPIES_MERCANTILE';
UPDATE Defines SET Value = 20 WHERE Name = 'MINOR_CIV_QUEST_GIVE_GOLD_COPIES_FRIENDLY';
UPDATE Defines SET Value = 10 WHERE Name = 'MINOR_CIV_QUEST_PLEDGE_TO_PROTECT_COPIES_BASE';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_PLEDGE_TO_PROTECT_COPIES_CULTURED';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_PLEDGE_TO_PROTECT_COPIES_MILITARISTIC';
UPDATE Defines SET Value = 20 WHERE Name = 'MINOR_CIV_QUEST_PLEDGE_TO_PROTECT_COPIES_NEUTRAL';
UPDATE Defines SET Value = 20 WHERE Name = 'MINOR_CIV_QUEST_BULLY_CITY_STATE_COPIES_MILITARISTIC';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_BULLY_CITY_STATE_COPIES_FRIENDLY';
UPDATE Defines SET Value = 20 WHERE Name = 'MINOR_CIV_QUEST_BULLY_CITY_STATE_COPIES_NEUTRAL';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_BULLY_CITY_STATE_COPIES_HOSTILE';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_BULLY_CITY_STATE_COPIES_IRRATIONAL';
UPDATE Defines SET Value = 10 WHERE Name = 'MINOR_CIV_QUEST_DENOUNCE_MAJOR_COPIES_BASE';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_DENOUNCE_MAJOR_COPIES_MARITIME';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_DENOUNCE_MAJOR_COPIES_RELIGIOUS';
UPDATE Defines SET Value = 80 WHERE Name = 'MINOR_CIV_QUEST_DENOUNCE_MAJOR_COPIES_HYPERLINK'; -- Mercantile + Hostile
UPDATE Defines SET Value = 80 WHERE Name = 'MINOR_CIV_QUEST_SPREAD_RELIGION_COPIES_HYPERLINK'; -- Religious + Friendly
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_TRADE_ROUTE_COPIES_MARITIME';
UPDATE Defines SET Value = 20 WHERE Name = 'MINOR_CIV_QUEST_TRADE_ROUTE_COPIES_MERCANTILE';
UPDATE Defines SET Value = 20 WHERE Name = 'MINOR_CIV_QUEST_TRADE_ROUTE_COPIES_FRIENDLY';
UPDATE Defines SET Value = 80 WHERE Name = 'MINOR_CIV_QUEST_TRADE_ROUTE_COPIES_HYPERLINK'; -- Mercantile + Neutral
-- Global Quests
UPDATE Defines SET Value = 10 WHERE Name = 'MINOR_CIV_QUEST_KILL_CAMP_COPIES_BASE';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_KILL_CAMP_COPIES_MILITARISTIC';
UPDATE Defines SET Value = 10 WHERE Name = 'MINOR_CIV_QUEST_KILL_CAMP_COPIES_HOSTILE';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_KILL_CITY_STATE_COPIES_MILITARISTIC';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_KILL_CITY_STATE_COPIES_NEUTRAL';
UPDATE Defines SET Value = 45 WHERE Name = 'MINOR_CIV_QUEST_CONTEST_CULTURE_COPIES_HYPERLINK'; -- Cultured + Neutral
UPDATE Defines SET Value = 10 WHERE Name = 'MINOR_CIV_QUEST_CONTEST_FAITH_COPIES_BASE';
UPDATE Defines SET Value = 10 WHERE Name = 'MINOR_CIV_QUEST_CONTEST_FAITH_COPIES_RELIGIOUS';
UPDATE Defines SET Value = 45 WHERE Name = 'MINOR_CIV_QUEST_CONTEST_FAITH_COPIES_HYPERLINK'; -- Religious + Neutral
UPDATE Defines SET Value = 10 WHERE Name = 'MINOR_CIV_QUEST_CONTEST_TECHS_COPIES_MILITARISTIC';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_QUEST_CONTEST_TECHS_COPIES_RELIGIOUS';
UPDATE Defines SET Value = 10 WHERE Name = 'MINOR_CIV_QUEST_INVEST_COPIES_MERCANTILE';
UPDATE Defines SET Value = 10 WHERE Name = 'MINOR_CIV_QUEST_INVEST_COPIES_FRIENDLY';

UPDATE Defines SET Value = 11 WHERE Name = 'MINOR_CIV_QUEST_ROUTE_MAXIMUM_DISTANCE';

-- Disable CS gold gifting and related quests
UPDATE Defines
SET Value = 0
WHERE Name IN (
	'MINOR_GOLD_GIFT_LARGE',
	'MINOR_GOLD_GIFT_MEDIUM',
	'MINOR_GOLD_GIFT_SMALL'
);

-- Disabled quests
UPDATE Defines
SET Value = 1
WHERE Name IN (
	'CSD_GOLD_GIFT_DISABLED',
	'QUEST_DISABLED_INVEST',
	'QUEST_DISABLED_GIVE_GOLD',
	'QUEST_DISABLED_FIND_PLAYER',
	'QUEST_DISABLED_COUP'
);

-- Enabled quests
UPDATE Defines
SET Value = 0
WHERE Name IN (
	'QUEST_DISABLED_KILL_CITY_STATE',
	'QUEST_DISABLED_KILL_CITY_STATE_FRIENDLY', -- Remove the prohibition on Friendly CS giving this quest
	'QUEST_DISABLED_FIND_CITY',
	'QUEST_DISABLED_WAR',
	'QUEST_DISABLED_CONSTRUCT_NATIONAL_WONDER',
	'QUEST_DISABLED_GIFT_SPECIFIC_UNIT',
	'QUEST_DISABLED_FIND_CITY_STATE',
	'QUEST_DISABLED_INFLUENCE',
	'QUEST_DISABLED_CONTEST_TOURISM',
	'QUEST_DISABLED_ARCHAEOLOGY',
	'QUEST_DISABLED_CIRCUMNAVIGATION',
	'QUEST_DISABLED_LIBERATION',
	'QUEST_DISABLED_HORDE',
	'QUEST_DISABLED_REBELLION',
	'QUEST_DISABLED_EXPLORE_AREA',
	'QUEST_DISABLED_BUILD_X_BUILDINGS',
	'QUEST_DISABLED_SPY_ON_MAJOR',
	'QUEST_DISABLED_ACQUIRE_CITY'
);

-- Global quest timer
UPDATE Defines SET Value = 4 WHERE Name = 'MINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN_RAND';
UPDATE Defines SET Value = 25 WHERE Name = 'MINOR_CIV_GLOBAL_QUEST_MIN_TURNS_BETWEEN';
UPDATE Defines SET Value = 20 WHERE Name = 'MINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN';
UPDATE Defines SET Value = 200 WHERE Name = 'MINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER'; -- Fewer quests from hostile CS

-- Personal quest timer
UPDATE Defines SET Value = 4 WHERE Name = 'MINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN';
UPDATE Defines SET Value = 0 WHERE Name = 'MINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN_RAND';
UPDATE Defines SET Value = 10 WHERE Name = 'MINOR_CIV_PERSONAL_QUEST_MIN_TURNS_BETWEEN';
UPDATE Defines SET Value = 20 WHERE Name = 'MINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN';
UPDATE Defines SET Value = 200 WHERE Name = 'MINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER'; -- Fewer quests from hostile CS

-- Need to be allied with the CS for this number of turns before marriage
UPDATE Defines SET Value = 10 WHERE Name = 'MINOR_CIV_BUYOUT_TURNS';

-- Sane value in case gold gift is re-enabled
UPDATE Defines SET Value = 18 WHERE Name = 'GOLD_GIFT_FRIENDSHIP_DIVISOR';

-------------------------------------------------------------------------------------------------------------------
-- Policies
-------------------------------------------------------------------------------------------------------------------
UPDATE Defines SET Value = 50 WHERE Name = 'BASE_POLICY_COST';
UPDATE Defines SET Value = 2.22 WHERE Name = 'POLICY_COST_EXPONENT';
UPDATE Defines SET Value = 4 WHERE Name = 'POLICY_COST_INCREASE_TO_BE_EXPONENTED';
UPDATE Defines SET Value = .20 WHERE Name = 'POLICY_COST_EXTRA_VALUE';

UPDATE Defines SET Value = 5 WHERE Name = 'SWITCH_POLICY_BRANCHES_TENETS_LOST';
UPDATE Defines SET Value = 3 WHERE Name = 'SWITCH_POLICY_BRANCHES_ANARCHY_TURNS';

-------------------------------------------------------------------------------------------------------------------
-- Tourism/Cultural Victory
-------------------------------------------------------------------------------------------------------------------
UPDATE Defines SET Value = 5 WHERE Name = 'CULTURE_LEVEL_EXOTIC';
UPDATE Defines SET Value = 25 WHERE Name = 'CULTURE_LEVEL_FAMILIAR';
UPDATE Defines SET Value = 50 WHERE Name = 'CULTURE_LEVEL_POPULAR';

UPDATE Defines SET Value = 3 WHERE Name = 'BASE_CULTURE_PER_GREAT_WORK';
UPDATE Defines SET Value = 2 WHERE Name = 'BASE_TOURISM_PER_GREAT_WORK';

UPDATE Defines SET Value = 15 WHERE Name = 'TOURISM_MODIFIER_OPEN_BORDERS';
UPDATE Defines SET Value = 10 WHERE Name = 'TOURISM_MODIFIER_TRADE_ROUTE';
UPDATE Defines SET Value = -10 WHERE Name = 'TOURISM_MODIFIER_DIFFERENT_IDEOLOGIES';
UPDATE Defines SET Value = 20 WHERE Name = 'TOURISM_MODIFIER_DIPLOMAT';
UPDATE Defines SET Value = 1 WHERE Name = 'TOURISM_MODIFIER_SHARED_RELIGION'; -- percentage
UPDATE Defines SET Value = 50 WHERE Name = 'TOURISM_MODIFIER_SHARED_RELIGION_MAX';
UPDATE Defines SET Value = 2 WHERE Name = 'TOURISM_MODIFIER_SHARED_RELIGION_TYPE'; -- 0 = no scaling, 1 = scaling per city, 2 = scaling per population

-------------------------------------------------------------------------------------------------------------------
-- Religion
-------------------------------------------------------------------------------------------------------------------
UPDATE Defines SET Value = 50 WHERE Name = 'RELIGION_MIN_FAITH_FIRST_PANTHEON';
UPDATE Defines SET Value = 0 WHERE Name = 'RELIGION_GAME_FAITH_DELTA_NEXT_PANTHEON';
UPDATE Defines SET Value = 100 WHERE Name = 'RELIGION_BASE_CHANCE_PROPHET_SPAWN';
UPDATE Defines SET Value = 3 WHERE Name = 'RELIGION_MAX_MISSIONARIES';
UPDATE Defines SET Value = 800 WHERE Name = 'RELIGION_MIN_FAITH_FIRST_PROPHET';
UPDATE Defines SET Value = 300 WHERE Name = 'RELIGION_FAITH_DELTA_NEXT_PROPHET';
UPDATE Defines SET Value = 1200 WHERE Name = 'RELIGION_MIN_FAITH_SECOND_PROPHET';
UPDATE Defines SET Value = 1000 WHERE Name = 'RELIGION_MIN_FAITH_FIRST_GREAT_PERSON';
UPDATE Defines SET Value = 1500 WHERE Name = 'RELIGION_FAITH_DELTA_NEXT_GREAT_PERSON';
UPDATE Defines SET Value = 50 WHERE Name = 'INQUISITION_EFFECTIVENESS';

-------------------------------------------------------------------------------------------------------------------
-- Science/Tech
-------------------------------------------------------------------------------------------------------------------
UPDATE Defines SET Value = 0 WHERE Name = 'SCIENCE_PER_POPULATION';
UPDATE Defines SET Value = 10 WHERE Name = 'TECH_COST_TOTAL_KNOWN_TEAM_MODIFIER';
UPDATE Defines SET Value = 80 WHERE Name = 'BARBARIAN_TECH_PERCENT';
UPDATE Defines SET Value = 60 WHERE Name = 'MINOR_CIV_TECH_PERCENT';
UPDATE Defines SET Value = 100 WHERE Name = 'TECH_COST_EXTRA_TEAM_MEMBER_MODIFIER';

-------------------------------------------------------------------------------------------------------------------
-- Puppet stuff
-------------------------------------------------------------------------------------------------------------------
UPDATE Defines SET Value = -80 WHERE Name = 'PUPPET_FAITH_MODIFIER';
UPDATE Defines SET Value = -80 WHERE Name = 'PUPPET_CULTURE_MODIFIER';
UPDATE Defines SET Value = -80 WHERE Name = 'PUPPET_SCIENCE_MODIFIER';
UPDATE Defines SET Value = -80 WHERE Name = 'PUPPET_TOURISM_MODIFIER';
UPDATE Defines SET Value = -80 WHERE Name = 'PUPPET_GOLD_MODIFIER';
UPDATE Defines SET Value = -80 WHERE Name = 'PUPPET_GOLDEN_AGE_MODIFIER';

-------------------------------------------------------------------------------------------------------------------
-- Barbarians
-------------------------------------------------------------------------------------------------------------------

-- Number of camps spawned on initial spawn turn
-- Unit: 0.1% of eligible tiles
UPDATE Defines SET Value = 10 WHERE Name = 'BARBARIAN_CAMP_FIRST_TURN_PERCENT_OF_TARGET_TO_ADD';

-- Extra number of camps spawned on initial spawn turn per era in advanced starts
-- Unit: 0.1% of eligible tiles
UPDATE Defines SET Value = 5 WHERE Name = 'BARBARIAN_CAMP_FIRST_TURN_PERCENT_PER_ERA';

-- # of tiles required in a landmass for a camp to spawn.
UPDATE Defines SET Value = 1 WHERE Name = 'BARBARIAN_CAMP_MINIMUM_ISLAND_SIZE';

-- Barbarian camps start spawning on this turn
UPDATE Defines SET Value = 2 WHERE Name = 'BARBARIAN_INITIAL_SPAWN_TURN';

-- Number of barbarians spawned with each new barbarian camp on initial spawn turn
UPDATE Defines SET Value = 2 WHERE Name = 'BARBARIAN_NUM_UNITS_INITIAL_ENCAMPMENT_CREATION_SPAWN';

-- Number of barbarians spawned with each new barbarian camp after initial spawn turn
UPDATE Defines SET Value = 2 WHERE Name = 'BARBARIAN_NUM_UNITS_ENCAMPMENT_CREATION_SPAWN';

-- Barbarian release turn is modified by +/- this number, then scaled with game speed
UPDATE Defines SET Value = 2 WHERE Name = 'AI_TACTICAL_BARBARIAN_RELEASE_VARIATION';

-- Barbarian ships start spawning on this turn
UPDATE Defines SET Value = 20 WHERE Name = 'BARBARIAN_NAVAL_UNIT_START_TURN_SPAWN';

-- Barbarian healing
UPDATE Defines SET Value = 10 WHERE Name = 'BALANCE_BARBARIAN_HEAL_RATE' AND EXISTS (SELECT 1 FROM Community WHERE Type = 'BARBARIAN_HEAL' AND Value = 1);
UPDATE Defines SET Value = 15 WHERE Name = 'BALANCE_BARBARIAN_HEAL_RATE' AND EXISTS (SELECT 1 FROM Community WHERE Type = 'BARBARIAN_HEAL' AND Value = 2);

-------------------------------------------------------------------------------------------------------------------
-- Trade Routes
-------------------------------------------------------------------------------------------------------------------
UPDATE Defines SET Value = 80 WHERE Name = 'INTERNATIONAL_TRADE_BASE';
UPDATE Defines SET Value = 6 WHERE Name = 'TRADE_ROUTE_CAPITAL_POP_GOLD_MULTIPLIER';
UPDATE Defines SET Value = 50 WHERE Name = 'TRADE_ROUTE_CITY_POP_GOLD_MULTIPLIER';
UPDATE Defines SET Value = 80 WHERE Name = 'INTERNATIONAL_TRADE_CITY_GPT_DIVISOR';
UPDATE Defines SET Value = 25 WHERE Name = 'TRADE_ROUTE_BASE_SEA_MODIFIER';
UPDATE Defines SET Value = 600 WHERE Name = 'TRADE_ROUTE_BASE_FOOD_VALUE';
UPDATE Defines SET Value = 600 WHERE Name = 'TRADE_ROUTE_BASE_PRODUCTION_VALUE';
UPDATE Defines SET Value = 5 WHERE Name = 'TRADE_ROUTE_DIFFERENT_RESOURCE_VALUE';
UPDATE Defines SET Value = 125 WHERE Name = 'TRADE_ROUTE_CULTURE_DIVISOR_TIMES100';
UPDATE Defines SET Value = 125 WHERE Name = 'TRADE_ROUTE_SCIENCE_DIVISOR_TIMES100';

-- These 5 values change the amount of science earned from trade routes with influence civs
-- Each is adding to an incremental value (so Familiar is already 4, Popular already 6, etc.)
UPDATE Defines SET Value = 2 WHERE Name = 'BALANCE_SCIENCE_INFLUENCE_LEVEL_EXOTIC';
UPDATE Defines SET Value = 4 WHERE Name = 'BALANCE_SCIENCE_INFLUENCE_LEVEL_FAMILIAR';
UPDATE Defines SET Value = 6 WHERE Name = 'BALANCE_SCIENCE_INFLUENCE_LEVEL_POPULAR';
UPDATE Defines SET Value = 8 WHERE Name = 'BALANCE_SCIENCE_INFLUENCE_LEVEL_INFLUENTIAL';
UPDATE Defines SET Value = 10 WHERE Name = 'BALANCE_SCIENCE_INFLUENCE_LEVEL_DOMINANT';

-------------------------------------------------------------------------------------------------------------------
-- Happiness
-------------------------------------------------------------------------------------------------------------------
UPDATE Defines SET Value = 35 WHERE Name = 'VERY_UNHAPPY_THRESHOLD';
UPDATE Defines SET Value = 20 WHERE Name = 'SUPER_UNHAPPY_THRESHOLD';
UPDATE Defines SET Value = -25 WHERE Name = 'UNHAPPY_GROWTH_PENALTY';
UPDATE Defines SET Value = -50 WHERE Name = 'VERY_UNHAPPY_GROWTH_PENALTY';
UPDATE Defines SET Value = -20 WHERE Name = 'VERY_UNHAPPY_MAX_COMBAT_PENALTY';
UPDATE Defines SET Value = 0 WHERE Name = 'VERY_UNHAPPY_CANT_TRAIN_SETTLERS';

-- Base unhappiness per each citizen in a city.
UPDATE Defines SET Value = 0 WHERE Name = 'UNHAPPINESS_PER_POPULATION';

-- Base unhappiness per each citizen in an occupied city.
UPDATE Defines SET Value = 1.00 WHERE Name = 'UNHAPPINESS_PER_OCCUPIED_POPULATION';

-- Flat unhappiness per founded city.
UPDATE Defines SET Value = 0 WHERE Name = 'UNHAPPINESS_PER_CITY';

-- Flat unhappiness per captured city.
UPDATE Defines SET Value = 0 WHERE Name = 'UNHAPPINESS_PER_CAPTURED_CITY';

-- Great Musician Happiness
UPDATE Defines SET Value = 1 WHERE Name = 'GREAT_MUSICIAN_BLAST_HAPPINESS_GLOBAL';

-------------------------------------------------------------------------------------------------------------------
-- Great People
-------------------------------------------------------------------------------------------------------------------
UPDATE Defines SET Value = 150 WHERE Name = 'GREAT_PERSON_THRESHOLD_BASE';
UPDATE Defines SET Value = 250 WHERE Name = 'GREAT_PERSON_THRESHOLD_INCREASE';

-------------------------------------------------------------------------------------------------------------------
-- Golden Age
-------------------------------------------------------------------------------------------------------------------
UPDATE Defines SET Value = 1000 WHERE Name = 'GOLDEN_AGE_BASE_THRESHOLD_INITIAL';
UPDATE Defines SET Value = 400 WHERE Name = 'GOLDEN_AGE_BASE_THRESHOLD_HAPPINESS';
UPDATE Defines SET Value = 2000 WHERE Name = 'GOLDEN_AGE_EACH_GA_ADDITIONAL_HAPPINESS';
UPDATE Defines SET Value = 5 WHERE Name = 'GOLDEN_AGE_THRESHOLD_CITY_MULTIPLIER';

-------------------------------------------------------------------------------------------------------------------
-- WLTKD
-------------------------------------------------------------------------------------------------------------------
UPDATE Defines SET Value = 30 WHERE Name = 'WLTKD_RESOURCE_RESET_TURNS';

-------------------------------------------------------------------------------------------------------------------
-- City Connection
-------------------------------------------------------------------------------------------------------------------

-- Bonus production% if city is connected to capital via railroad
UPDATE Defines SET Value = 0 WHERE Name = 'INDUSTRIAL_ROUTE_PRODUCTION_MOD';

-------------------------------------------------------------------------------------------------------------------
-- Espionage
-------------------------------------------------------------------------------------------------------------------
UPDATE Defines SET Value = 10 WHERE Name = 'BALANCE_SPY_RESPAWN_TIMER';
UPDATE Defines SET Value = 30 WHERE Name = 'ESPIONAGE_INFLUENCE_GAINED_FOR_RIGGED_ELECTION';
UPDATE Defines SET Value = 1000 WHERE Name = 'ESPIONAGE_GATHERING_INTEL_COST_PERCENT';

-------------------------------------------------------------------------------------------------------------------
-- World Congress
-------------------------------------------------------------------------------------------------------------------
UPDATE Defines SET Value = 33 WHERE Name = 'TEMPORARY_CULTURE_BOOST_MOD';
UPDATE Defines SET Value = 50 WHERE Name = 'TEMPORARY_TOURISM_BOOST_MOD';
UPDATE Defines SET Value = 3 WHERE Name = 'LEAGUE_PROPOSERS_PER_SESSION';
UPDATE Defines SET Value = 1 WHERE Name = 'LEAGUE_NUM_LEADERS_FOR_EXTRA_VOTES';
UPDATE Defines SET Value = 25 WHERE Name = 'LEAGUE_PROJECT_PROGRESS_PERCENT_WARNING';

-------------------------------------------------------------------------------------------------------------------
-- Score
-------------------------------------------------------------------------------------------------------------------
UPDATE Defines SET Value = 10 WHERE Name = 'SCORE_CITY_MULTIPLIER';
UPDATE Defines SET Value = 2 WHERE Name = 'SCORE_POPULATION_MULTIPLIER';
UPDATE Defines SET Value = 6 WHERE Name = 'SCORE_TECH_MULTIPLIER';
UPDATE Defines SET Value = 16 WHERE Name = 'SCORE_POLICY_MULTIPLIER';
UPDATE Defines SET Value = 5 WHERE Name = 'SCORE_BELIEF_MULTIPLIER';
UPDATE Defines SET Value = 3 WHERE Name = 'SCORE_RELIGION_CITIES_MULTIPLIER';

-------------------------------------------------------------------------------------------------------------------
-- AI
-------------------------------------------------------------------------------------------------------------------

-- Don't rush workers until city has reached this population
UPDATE Defines SET Value = 4 WHERE Name = 'AI_CITYSTRATEGY_WANT_TILE_IMPROVERS_MINIMUM_SIZE';

UPDATE Defines SET Value = 10 WHERE Name = 'AI_CITIZEN_VALUE_GPP';
UPDATE Defines SET Value = 8 WHERE Name = 'AI_CITIZEN_VALUE_GOLD';

-- War progress score modifier per % happiness below 50%
UPDATE Defines SET Value = -2 WHERE Name = 'WAR_PROGRESS_PER_UNHAPPY';

-------------------------------------------------------------------------------------------------------------------
-- Post Defines
-------------------------------------------------------------------------------------------------------------------
INSERT INTO PostDefines
	(Name, Key, "Table")
VALUES
	('EMBASSY_IMPROVEMENT', 'IMPROVEMENT_EMBASSY', 'Improvements'),
	('JUGGERNAUT_PROMOTION', 'PROMOTION_JUGGERNAUT', 'UnitPromotions'),
	('MARCH_SKIRMISHER_PROMOTION', 'PROMOTION_SKIRMISHER_MARCH', 'UnitPromotions');

UPDATE PostDefines SET Key = 'ERA_MODERN' WHERE Name = 'IDEOLOGY_START_ERA';
