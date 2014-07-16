	UPDATE Defines
	SET Value = '0'
	WHERE Name = 'UNHAPPINESS_PER_POPULATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Defines
	SET Value = '1.25'
	WHERE Name = 'UNHAPPINESS_PER_OCCUPIED_POPULATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );
		
	UPDATE Defines
	SET Value = '1'
	WHERE Name = 'UNHAPPINESS_PER_CITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Defines
	SET Value = '2'
	WHERE Name = 'UNHAPPINESS_PER_CAPTURED_CITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE TacticalMoves
	SET Priority = '70'
	WHERE Type = 'TACTICAL_GARRISON_1_TURN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );
	
	UPDATE Language_en_US
	SET Text = '[ICON_CITIZEN] Citizens in non-occupied Cities do not produce [ICON_HAPPINESS_3] Unhappiness directly. See the tooltips below for the sources of [ICON_HAPPINESS_3] Unhappiness in each City'
	WHERE Tag = 'TXT_KEY_POP_UNHAPPINESS_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Every (non-occupied) City produces 1 [ICON_HAPPINESS_3] Unhappiness, plus an additional amount based on the current Era and the number of Cities you own.'
	WHERE Tag = 'TXT_KEY_NUMBER_OF_CITIES_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Every (non-occupied) City produces 1 [ICON_HAPPINESS_3] Unhappiness, plus an additional amount based on the current Era and the number of Cities you own (Normally).'
	WHERE Tag = 'TXT_KEY_NUMBER_OF_CITIES_TT_NORMALLY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Every [ICON_OCCUPIED] Occupied City produces 3 [ICON_HAPPINESS_3] Unhappiness, plus an additional amount based on the current Era and the number of Cities you own.'
	WHERE Tag = 'TXT_KEY_NUMBER_OF_OCCUPIED_CITIES_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Every [ICON_CITIZEN] Citizen in an [ICON_OCCUPIED] Occupied City produces 1.5 [ICON_HAPPINESS_3] Unhappiness'
	WHERE Tag = 'TXT_KEY_OCCUPIED_POP_UNHAPPINESS_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_SPECIALIST', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Base reduction of unhappiness threshold for Puppet cities.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_PUPPET_THRESHOLD_MOD', '-20'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );


-- REALLY IMPORTANT VALUE
-- Base increase of threshold values for # of Cities. Increases the As you advance into the game, each era increases the threshold value by this amount.

	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_ERA_BASE_INCREASE_CITY_COUNT', '10'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );
	
-- The values below affect local happiness from cities.

-- Rates at which technology affects global happiness averages for science/culture, defense and food. 100 = 1:1 rate (1% of techs researched = 1% boost to average).

-- Differentiation a byproduct of the scaling that occurrs in a typical game - food/gold/defense values from cities do not increase as fast as science and culture do.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_TECH_RATE_CULTURE_SCIENCE', '50'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );
	
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_TECH_RATE_DEFENSE', '75'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );
	
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_TECH_RATE_GOLD_FOOD', '125'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );
	
-- Unhappiness point per religious minority pop. A high faith to population ratio will reduce this penalty. Also note that this is the ONLY unhappiness calculation that goes down as the game progresses (religion makes slightly less unhappiness as you move into new eras)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_MINORITY_POP', '0.33'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per starving citizen. This can compound, so be careful.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_FROM_STARVING_PER_POP', '0.25'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per pillaged plot owned by city.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_PILLAGED', '0.33'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per pop if unconnected or blockaded.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_FROM_UNCONNECTED_PER_POP', '0.40'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );