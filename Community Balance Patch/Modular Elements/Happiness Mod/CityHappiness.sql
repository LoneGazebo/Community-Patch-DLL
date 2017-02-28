
	-- FREE HAPPINESS FROM GAME SPEED

	UPDATE GameSpeeds
	SET StartingHappiness = '10'
	WHERE Type = 'GAMESPEED_MARATHON' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE GameSpeeds
	SET StartingHappiness = '5'
	WHERE Type = 'GAMESPEED_EPIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE GameSpeeds
	SET StartingHappiness = '0'
	WHERE Type = 'GAMESPEED_STANDARD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	-- DEFINES FOR CITY HAPPINESS
	
	UPDATE Defines
	SET Value = '0'
	WHERE Name = 'UNHAPPINESS_PER_POPULATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Defines
	SET Value = '1.00'
	WHERE Name = 'UNHAPPINESS_PER_OCCUPIED_POPULATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );
		
	UPDATE Defines
	SET Value = '0'
	WHERE Name = 'UNHAPPINESS_PER_CITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Defines
	SET Value = '0'
	WHERE Name = 'UNHAPPINESS_PER_CAPTURED_CITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- 25 = 0.25 unhappiness per specialist.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_SPECIALIST', '25'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Base reduction of unhappiness threshold for Puppet cities. -50% is default.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_PUPPET_THRESHOLD_MOD', '-50'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Base Modifier for Capital Thresholds. Offsets boost from Palace, helps make Capital a source of Unhappiness early on. 25% is default.
		INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_CAPITAL_MODIFIER', '25'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- 	Base Value of Test - Modifier to tech % cost. 1.42 is default.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_TECH_BASE_MODIFIER', '1.42'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Base increase of threshold values based on # of citizens in cities you own. Modifier increases as cities grow. 100 is default. (Function is 'Need + ((CityPop x Value (134))/100)'. This value is added to City as a % modifier to needs.)

	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_BASE_CITY_COUNT_MULTIPLIER', '65'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );
	
-- Value by which yield/threshold difference is modified. Base value is 1%.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPY_CITY_BASE_VALUE', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Value by which yield/threshold difference for boredom is divded.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPY_CITY_BASE_VALUE_BOREDOM', '60'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Value by which yield/threshold difference is divded.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPY_CITY_BASE_VALUE_ILLITERACY', '45'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Value by which yield/threshold difference is divded.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPY_CITY_BASE_VALUE_DISORDER', '-30'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Value by which yield/threshold difference is divded.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPY_CITY_BASE_VALUE_POVERTY', '-10'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per religious minority pop. A high faith to population ratio will reduce this penalty. Also note that this is the ONLY unhappiness calculation that goes down as the game progresses (religion makes slightly less unhappiness as you move into new eras)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_MINORITY_POP', '0.50'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per starving citizen.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_FROM_STARVING_PER_POP', '0.34'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per pillaged plot owned by city.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_PILLAGED', '0.34'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per pop if unconnected or blockaded.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_FROM_UNCONNECTED_PER_POP', '0.25'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );


