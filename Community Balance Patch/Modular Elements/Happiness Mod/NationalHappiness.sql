-- Disables Bonus
-- DO NOT EDIT THIS VALUE
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_EMPIRE_MOD', '-1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Enables Bonus
-- DO NOT EDIT THIS VALUE
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_EMPIRE_MOD', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Division line for main happiness, including when penalties begin. This changes the global value. Acts oddly in LUA at anything above zero (TODO: FIX), so leave at zero for now.
-- DO NOT MODIFY
	
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_THRESHOLD_MAIN', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );
	
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_THRESHOLD_MAIN', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

--EMPIRE BONUSES BELOW

-- These values modify empire-wide bonuses or penalties gained (or lost) from happiness. Change the values below, making sure to keep the integers + or - as they are below.

-- These values can be modified.
	
	-- % reduction of combat effectiveness per point of unhappiness.
	UPDATE Defines
	SET Value = '-1'
	WHERE Name = 'VERY_UNHAPPY_COMBAT_PENALTY_PER_UNHAPPY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

	-- % max reduction of combat effectiveness per point of unhappiness.
	UPDATE Defines
	SET Value = '-20'
	WHERE Name = 'VERY_UNHAPPY_MAX_COMBAT_PENALTY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Division line for when happiness bonuses begin. Happiness above threshold grants bonus. (should always be a positive value)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_THRESHOLD', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Division line for when happiness bonuses begin. Happiness above threshold grants bonus. (should always be a positive value)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_THRESHOLD', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Maximum happiness bonus above threshold mod. (should always be a positive value)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_BONUS_MAXIMUM', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Maximum happiness bonus above threshold mod. (should always be a positive value)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_BONUS_MAXIMUM', '10'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Minimum happiness bonus % mod.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_BONUS_MINIMUM', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Minimum happiness bonus % mod.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_BONUS_MINIMUM', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Maximum happiness penalty % mod. (Should always be a negative value)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_PENALTY_MAXIMUM', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Maximum happiness penalty % mod. (Should always be a negative value)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_PENALTY_MAXIMUM', '-30'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Minimum happiness penalty % mod.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_PENALTY_MINIMUM', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Minimum happiness penalty % mod.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_PENALTY_MINIMUM', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Science % point per happiness mod (should always be a positive value).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_SCIENCE_MODIFIER', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Science % point per happiness mod (should always be a positive value).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_SCIENCE_MODIFIER', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );
 
--Gold % point per happiness mod (should always be a positive value).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_GOLD_MODIFIER', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

--Gold % point per happiness mod (should always be a positive value).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_GOLD_MODIFIER', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Production % point per happiness mod (should always be a positive value).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_PRODUCTION_MODIFIER', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Production % point per happiness mod (should always be a positive value).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_PRODUCTION_MODIFIER', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Food % point per happiness mod (should always be a positive value).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_FOOD_MODIFIER', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Food % point per happiness mod (should always be a positive value).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_FOOD_MODIFIER', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Faith point per happiness mod (should always be a positive value). THIS ONE DIVIDES BY YOUR HAPPINESS VALUE (SO /5 HAPPINESS PER TURN)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_FAITH_MODIFIER', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Faith point per happiness mod (should always be a positive value). THIS ONE DIVIDES BY YOUR HAPPINESS VALUE (SO /5 HAPPINESS PER TURN)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_FAITH_MODIFIER', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Culture point per happiness mod (should always be a positive value). THIS ONE DIVIDES BY YOUR HAPPINESS VALUE (SO /5 HAPPINESS PER TURN)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_CULTURE_MODIFIER', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Culture point per happiness mod (should always be a positive value). THIS ONE DIVIDES BY YOUR HAPPINESS VALUE (SO /5 HAPPINESS PER TURN)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_CULTURE_MODIFIER', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Happiness Flavors
	UPDATE AIEconomicStrategy_City_Flavors
	SET Flavor = '-10'
	WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_NEED_HAPPINESS_CRITICAL' AND FlavorType = 'FLAVOR_GROWTH';