
	-- Median value for global average selection model. 50 = true median.
	UPDATE Defines
	SET Value = '55'
	WHERE Name = 'BALANCE_HAPPINESS_THRESHOLD_PERCENTILE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	-- Food % point per happiness mod (should always be a positive value).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_FOOD_MODIFIER', '2';

	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_FOOD_MODIFIER', '10';

	-- Production % point per happiness mod (should always be a positive value).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_PRODUCTION_MODIFIER', '10';



	-- DEFINES FOR CITY HAPPINESS
	
	-- Base unhappiness per each citizen in a city.
	UPDATE Defines
	SET Value = '0'
	WHERE Name = 'UNHAPPINESS_PER_POPULATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	-- Base unhappiness per each citizen in an occupied city.
	UPDATE Defines
	SET Value = '1.00'
	WHERE Name = 'UNHAPPINESS_PER_OCCUPIED_POPULATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	-- Puppets produce flat unhappines based on # of citizens in the city. Divisor is this, never set to zero.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_PUPPET_THRESHOLD_MOD', '4'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	-- Per non-puppet city % modifier for unhappiness thresholds (i.e. # cities * value below = % modifier).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_EMPIRE_MULTIPLIER', '9'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	-- Flat unhappiness per founded city.
	UPDATE Defines
	SET Value = '0'
	WHERE Name = 'UNHAPPINESS_PER_CITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	-- Flat unhappiness per captured city.
	UPDATE Defines
	SET Value = '0'
	WHERE Name = 'UNHAPPINESS_PER_CAPTURED_CITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- 34 = 0.34 unhappiness per specialist.
	INSERT INTO Defines (
	Name, Value)     
	SELECT 'BALANCE_UNHAPPINESS_PER_SPECIALIST', '100'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Base Modifier for Capital Thresholds. Offsets boost from Palace, helps make Capital a source of Unhappiness early on. 25% is default.
		INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_CAPITAL_MODIFIER', '25'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- 	Base Value of Tech - % of techs researched v. techs known, multiplied by this value. 100 is default (each tech increases % by fraction of techs remaining v. techs researched).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_TECH_BASE_MODIFIER', '150'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Base increase of threshold values based on # of citizens in cities you own (1 citizen = 1%). Modifier increases as cities grow. 50 is default. Is a % modifier, so 25 would be a 25% increase over 1%.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_POP_MULTIPLIER', '-50'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );
	
-- Value by which yield/threshold difference is modified based on the remaining population
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPY_CITY_BASE_VALUE_BOREDOM', '50'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Value by which yield/threshold difference is modified based on the remaining population
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPY_CITY_BASE_VALUE_ILLITERACY', '40'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Value by which yield/threshold difference is modified based on the remaining population
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPY_CITY_BASE_VALUE_DISORDER', '70'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Value by which yield/threshold difference is modified based on the remaining population
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPY_CITY_BASE_VALUE_POVERTY', '60'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per religious minority pop. A high faith to population ratio will reduce this penalty. Also note that this is the ONLY unhappiness calculation that goes down as the game progresses (religion makes slightly less unhappiness as you move into new eras)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_MINORITY_POP', '0.5'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per starving citizen.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_FROM_STARVING_PER_POP', '1.0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per pillaged plot owned by city.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_PILLAGED', '0.50'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per pop if unconnected or blockaded.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_FROM_UNCONNECTED_PER_POP', '0.34'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );


