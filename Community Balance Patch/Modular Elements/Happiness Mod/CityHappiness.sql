	UPDATE Defines
	SET Value = '0'
	WHERE Name = 'UNHAPPINESS_PER_POPULATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Defines
	SET Value = '1.50'
	WHERE Name = 'UNHAPPINESS_PER_OCCUPIED_POPULATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );
		
	UPDATE Defines
	SET Value = '1'
	WHERE Name = 'UNHAPPINESS_PER_CITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Defines
	SET Value = '3'
	WHERE Name = 'UNHAPPINESS_PER_CAPTURED_CITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE TacticalMoves
	SET Priority = '100'
	WHERE Type = 'TACTICAL_GARRISON_1_TURN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );
	
	UPDATE Language_en_US
	SET Text = '[ICON_CITIZEN] Citizens in non-occupied Cities do not produce [ICON_HAPPINESS_4] Unhappiness directly. See the tooltips below for the sources of [ICON_HAPPINESS_4] Unhappiness in each City'
	WHERE Tag = 'TXT_KEY_POP_UNHAPPINESS_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Every (non-occupied) City produces 1 [ICON_HAPPINESS_4] Unhappiness, plus 5% more per Era.'
	WHERE Tag = 'TXT_KEY_NUMBER_OF_CITIES_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Every (non-occupied) City produces 1 [ICON_HAPPINESS_4] Unhappiness, plus 5% more per Era (Normally).'
	WHERE Tag = 'TXT_KEY_NUMBER_OF_CITIES_TT_NORMALLY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Every [ICON_OCCUPIED] Occupied City produces 3 [ICON_HAPPINESS_4] Unhappiness, plus 5% more per Era (Normally).'
	WHERE Tag = 'TXT_KEY_NUMBER_OF_OCCUPIED_CITIES_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Every [ICON_CITIZEN] Citizen in an [ICON_OCCUPIED] Occupied City produces 1.5 [ICON_HAPPINESS_4] Unhappiness'
	WHERE Tag = 'TXT_KEY_OCCUPIED_POP_UNHAPPINESS_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_SPECIALIST', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- REALLY IMPORTANT VALUE
-- % increase of threshold values for Poverty, Defense and Illiteracy values. Increases the unhappiness penalty of Connected, Pillaged and Starving values. Decreases penalty of Religious Strife value. As you advance into the game, each era increases the threshold value by this amount.

	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_ERA_BASE_INCREASE', '20'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- REALLY IMPORTANT VALUE
-- Base increase of threshold values for # of Cities. Increases the As you advance into the game, each era increases the threshold value by this amount.

	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_ERA_BASE_INCREASE_CITY_COUNT', '10'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Maximum amount of a city's population that can cause unhappiness from any given test below (number below is % of population). So, 40 = max of 40% of a city's population can be unhappy from a single test.

	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_MAX_UNHAPPY_PER_CITY_YIELD', '40'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );
	
-- The values below affect local happiness from cities.

-- Unhappiness point per religious minority pop. A high faith to population ratio will reduce this penalty. Also note that this is the ONLY unhappiness calculation that goes down as the game progresses (religion makes slightly less unhappiness as you move into new eras)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_MINORITY_POP', '0.50'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per starving citizen. This can compound, so be careful.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_FROM_STARVING_PER_POP', '0.33'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per pillaged plot owned by city.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_PILLAGED', '0.33'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per pop if unconnected or blockaded.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_FROM_UNCONNECTED_PER_POP', '0.33'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per gold/food threshold (i.e. gold + food rate needed per citizen before unhappiness kicks in).
-- Value is multiplied by city population - if gold rate for city is lower than value, unhappiness is calculated based on the remainder (threshold - gold rate).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_GOLD_THRESHOLD', '50'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per citizen over gold + food threshold. Value is multiplied by remainder from threshold calculation.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_GOLD_RATE', '1.00'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per defense threshold (i.e. rate needed per citizen before unhappiness kicks in).
-- Value is multiplied by city population - if defense rate from buildings + garrison for city is lower than value, unhappiness is calculated based on the remainder (threshold - defense).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_DEFENSE_THRESHOLD', '65'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per citizen over defense threshold. Value is multiplied by remainder from threshold calculation.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_DEFENSE_RATE', '1.25'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per culture + science threshold (i.e. culture + science rate needed per citizen before unhappiness kicks in).
-- Value is multiplied by city population - if culture rate for city is lower than value, unhappiness is calculated based on the remainder (threshold - culture rate).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_CULTURE_THRESHOLD', '50'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per citizen over culture threshold. Value is multiplied by remainder from threshold calculation.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_CULTURE_RATE', '1.25'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );