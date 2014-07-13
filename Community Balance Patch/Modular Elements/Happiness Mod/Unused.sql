


-- Maximum amount of a city's population considered for yield tests below (number below is % of population). So, 70 = max of 70% of a city's population can be used to calculate the threshold for a single test.


-- Starting calculation for population. Artificially raises a city's population by this amount for calculation purposes only.

	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPY_CITY_BASE_VALUE', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	-- Unhappiness point per gold/food threshold (i.e. gold + food rate needed per citizen before unhappiness kicks in).
-- Value is multiplied by city population - if gold rate for city is lower than value, unhappiness is calculated based on the remainder (threshold - gold rate).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_GOLD_THRESHOLD', '200'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per citizen over gold + food threshold. Value is multiplied by remainder from threshold calculation.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_GOLD_RATE', '0.5'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per defense threshold (i.e. rate needed per citizen before unhappiness kicks in).
-- Value is multiplied by city population - if defense rate from buildings + garrison for city is lower than value, unhappiness is calculated based on the remainder (threshold - defense).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_DEFENSE_THRESHOLD', '200'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per citizen over defense threshold. Value is multiplied by remainder from threshold calculation.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_DEFENSE_RATE', '0.5'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per culture + science threshold (i.e. culture + science rate needed per citizen before unhappiness kicks in).
-- Value is multiplied by city population - if culture rate for city is lower than value, unhappiness is calculated based on the remainder (threshold - culture rate).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_CULTURE_THRESHOLD', '200'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per citizen over culture threshold. Value is multiplied by remainder from threshold calculation.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_CULTURE_RATE', '0.5'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );