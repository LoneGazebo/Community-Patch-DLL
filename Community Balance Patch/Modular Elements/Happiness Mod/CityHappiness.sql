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
	SET Priority = '40'
	WHERE Type = 'TACTICAL_GARRISON_1_TURN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = '[COLOR_POSITIVE_TEXT]Monarchy[ENDCOLOR][NEWLINE]+1 [ICON_GOLD] Gold and the [ICON_CAPITAL] Capital no longer counts towards [ICON_HAPPINESS_4] Unhappiness from Cities.'
	WHERE Tag = 'TXT_KEY_POLICY_MONARCHY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_SPECIALIST', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );
	
-- These values below affect local happiness from cities. Values should always be floats (i.e. 0.0, 10.1, etc.)

-- Population calculations are increased by per city, per era - you will need more and more yields as the game goes along to keep your civilization happy!

-- Unhappiness point per religious minority pop. A high faith to population ratio will reduce this penalty. Also note that this is the ONLY unhappiness calculation that goes down as the game progresses (religion makes slightly less unhappiness as you move into new eras)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_MINORITY_POP', '0.60'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per starving citizen. This can compound, so be careful.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_FROM_STARVING_PER_POP', '0.60'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per pillaged plot owned by city.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_PILLAGED', '0.60'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per pop if unconnected or blockaded.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_FROM_UNCONNECTED_PER_POP', '0.75'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per gold/food threshold (i.e. gold + food rate needed per citizen before unhappiness kicks in).
-- Value is multiplied by city population - if gold rate for city is lower than value, unhappiness is calculated based on the remainder (threshold - gold rate).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_GOLD_THRESHOLD', '0.55'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per citizen over gold + food threshold. Value is multiplied by remainder from threshold calculation.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_GOLD_RATE', '0.75'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per defense threshold (i.e. rate needed per citizen before unhappiness kicks in).
-- Value is multiplied by city population - if defense rate from buildings + garrison for city is lower than value, unhappiness is calculated based on the remainder (threshold - defense).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_DEFENSE_THRESHOLD', '0.70'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per citizen over defense threshold. Value is multiplied by remainder from threshold calculation.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_DEFENSE_RATE', '0.75'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per culture + science threshold (i.e. culture + science rate needed per citizen before unhappiness kicks in).
-- Value is multiplied by city population - if culture rate for city is lower than value, unhappiness is calculated based on the remainder (threshold - culture rate).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_CULTURE_THRESHOLD', '0.55'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per citizen over culture threshold. Value is multiplied by remainder from threshold calculation.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_CULTURE_RATE', '0.75'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );