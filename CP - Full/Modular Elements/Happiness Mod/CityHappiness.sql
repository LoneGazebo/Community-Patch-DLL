	UPDATE Defines
	SET Value = '0'
	WHERE Name = 'UNHAPPINESS_PER_POPULATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Defines
	SET Value = '0.75'
	WHERE Name = 'UNHAPPINESS_PER_OCCUPIED_POPULATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );
		
	UPDATE Defines
	SET Value = '3'
	WHERE Name = 'UNHAPPINESS_PER_CITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Defines
	SET Value = '6'
	WHERE Name = 'UNHAPPINESS_PER_CAPTURED_CITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = '[COLOR_POSITIVE_TEXT]Monarchy[ENDCOLOR][NEWLINE]+1 [ICON_GOLD] Gold and the [ICON_CAPITAL] Capital no longer counts towards [ICON_HAPPINESS_4] Unhappiness from Cities.'
	WHERE Tag = 'TXT_KEY_POLICY_MONARCHY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_SPECIALIST', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );
	
-- These values below affect local happiness from cities. Values should always be floats (i.e. 0.0, 10.1, etc.)

-- Unhappiness point per religious minority pop.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_MINORITY_POP', '0.75'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per starving citizen.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_FROM_STARVING_PER_POP', '0.50'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per pillaged plot owned by city.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_PILLAGED', '0.50'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per pop if unconnected.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_FROM_UNCONNECTED_PER_POP', '0.50'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per gold/food threshold (i.e. gold + food rate needed per citizen before unhappiness kicks in).
-- Value is multiplied by city population - if gold rate for city is lower than value, unhappiness is calculated based on the remainder (threshold - gold rate).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_GOLD_THRESHOLD', '1.66'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per citizen over gold threshold. Value is multiplied by remainder from threshold calculation.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_GOLD_RATE', '0.75'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per defense threshold (i.e. rate needed per citizen before unhappiness kicks in).
-- Value is multiplied by city population - if defense rate from buildings for city is lower than value, unhappiness is calculated based on the remainder (threshold - defense).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_DEFENSE_THRESHOLD', '1.20'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per citizen over defense threshold. Value is multiplied by remainder from threshold calculation.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_DEFENSE_RATE', '0.75'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per culture threshold (i.e. rate needed per citizen before unhappiness kicks in).
-- Value is multiplied by city population - if culture rate for city is lower than value, unhappiness is calculated based on the remainder (threshold - culture rate).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_CULTURE_THRESHOLD', '1.20'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per citizen over culture threshold. Value is multiplied by remainder from threshold calculation.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_CULTURE_RATE', '0.75'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );