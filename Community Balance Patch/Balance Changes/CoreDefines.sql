-- Trade Routes
	UPDATE Defines
	SET Value = '60'
	WHERE Name = 'INTERNATIONAL_TRADE_CITY_GPT_DIVISOR';

	UPDATE Defines
	SET Value = '10'
	WHERE Name = 'TRADE_ROUTE_BASE_LAND_MODIFIER';

	UPDATE Defines
	SET Value = '20'
	WHERE Name = 'TRADE_ROUTE_BASE_SEA_MODIFIER';

	UPDATE Defines
	SET Value = '500'
	WHERE Name = 'TRADE_ROUTE_BASE_FOOD_VALUE';
	
	UPDATE Defines
	SET Value = '500'
	WHERE Name = 'TRADE_ROUTE_BASE_PRODUCTION_VALUE';

	UPDATE Defines
	SET Value = '15'
	WHERE Name = 'TRADE_ROUTE_RIVER_CITY_MODIFIER';

	UPDATE Defines
	SET Value = '50'
	WHERE Name = 'TRADE_ROUTE_DIFFERENT_RESOURCE_VALUE';

-- Deal Durations
	UPDATE GameSpeeds
	SET DealDuration = '100'
	WHERE Type = 'GAMESPEED_MARATHON';

	UPDATE GameSpeeds
	SET DealDuration = '70'
	WHERE Type = 'GAMESPEED_EPIC';

	UPDATE GameSpeeds
	SET DealDuration = '50'
	WHERE Type = 'GAMESPEED_STANDARD';

	UPDATE GameSpeeds
	SET DealDuration = '30'
	WHERE Type = 'GAMESPEED_QUICK';

-- Trade Route Stuff
	UPDATE Defines
	SET Value = '20'
	WHERE Name = 'TRADE_ROUTE_RIVER_CITY_MODIFIER';

-- SR Penalty
	UPDATE Defines
	SET Value = '0'
	WHERE Name = 'STRATEGIC_RESOURCE_EXHAUSTED_PENALTY';

-- Cannot Train Settlers
	UPDATE Defines
	SET Value = '0'
	WHERE Name = 'VERY_UNHAPPY_CANT_TRAIN_SETTLERS';

-- Change for Austria UA
	UPDATE Defines
	SET Value = '20'
	WHERE Name = 'MINOR_CIV_BUYOUT_TURNS';

-- City Growth Defines

	-- 15
	UPDATE Defines
	SET Value = '15'
	WHERE Name = 'BASE_CITY_GROWTH_THRESHOLD';

	-- 1.5
	UPDATE Defines
	SET Value = '2.08'
	WHERE Name = 'CITY_GROWTH_EXPONENT';

	-- 8
	UPDATE Defines
	SET Value = '11'
	WHERE Name = 'CITY_GROWTH_MULTIPLIER';

-- More cost for first and second Great Prophets - push founding/enhancing dates back slightly
	UPDATE Defines
	SET Value = '400'
	WHERE Name = 'RELIGION_MIN_FAITH_FIRST_PROPHET';

	UPDATE Defines
	SET Value = '300'
	WHERE Name = 'RELIGION_FAITH_DELTA_NEXT_PROPHET';

-- Great People more expensive - delta will increase faster as well.

	UPDATE Defines
	SET Value = '1000'
	WHERE Name = 'RELIGION_MIN_FAITH_FIRST_GREAT_PERSON';

	UPDATE Defines
	SET Value = '1200'
	WHERE Name = 'RELIGION_FAITH_DELTA_NEXT_GREAT_PERSON';

-- Rebase distance increased
	UPDATE Defines
	SET Value = '500'
	WHERE Name = 'AIR_UNIT_REBASE_RANGE_MULTIPLIER';

-- More Cost for Plot Acquisition
	UPDATE Defines
	SET Value = '20'
	WHERE Name = 'CULTURE_COST_FIRST_PLOT';

	UPDATE Defines
	SET Value = '15'
	WHERE Name = 'CULTURE_COST_LATER_PLOT_MULTIPLIER';

	UPDATE Defines
	SET Value = '1.30'
	WHERE Name = 'CULTURE_COST_LATER_PLOT_EXPONENT';
-- END
	-- Unit upgrades more expensive based on era and production needed
	UPDATE Defines
	SET Value = '2'
	WHERE Name = 'UNIT_UPGRADE_COST_PER_PRODUCTION';

	UPDATE Defines
	SET Value = '0.1'
	WHERE Name = 'UNIT_UPGRADE_COST_MULTIPLIER_PER_ERA';

	-- Barbarian Experience Cap raised to 45
	UPDATE Defines
	SET Value = '45'
	WHERE Name = 'BARBARIAN_MAX_XP_VALUE';

	-- GP Upgrade Deltas
	UPDATE Defines
	SET Value = '150'
	WHERE Name = 'GREAT_PERSON_THRESHOLD_BASE';

	UPDATE Defines
	SET Value = '250'
	WHERE Name = 'GREAT_PERSON_THRESHOLD_INCREASE';

	-- Is a subtraction in the DLL, so keep this positive!
	UPDATE Defines
	SET Value = '0'
	WHERE Name = 'GWAM_THRESHOLD_DECREASE';
	
	-- Espionage Rates Increased
	UPDATE Defines
	SET Value =  '400'
	WHERE Name = 'ESPIONAGE_GATHERING_INTEL_COST_PERCENT';
	
	UPDATE Defines
	SET Value = '25'
	WHERE Name = 'ESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT';

	-- Reduction of Pop from conquest reduced to 25% (was 50%)
	UPDATE Defines
	SET Value = '75'
	WHERE Name = 'CITY_CAPTURE_POPULATION_PERCENT';

	-- Unit Costs
	UPDATE Defines
	SET Value = '9'
	WHERE Name = 'UNIT_MAINTENANCE_GAME_MULTIPLIER';

	UPDATE Defines
	SET Value = '6'
	WHERE Name = 'UNIT_MAINTENANCE_GAME_EXPONENT_DIVISOR';

	-- Balance Defines for WC
	UPDATE Defines
	SET Value = '33'
	WHERE Name = 'TEMPORARY_CULTURE_BOOST_MOD';

	UPDATE Defines
	SET Value = '50'
	WHERE Name = 'TEMPORARY_TOURISM_BOOST_MOD';