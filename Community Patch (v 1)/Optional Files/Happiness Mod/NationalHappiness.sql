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

-- These values modify empire-wide bonuses or penalties gained (or lost) from happiness. Change the values below, making sure to keep the integers + or - as they are below

-- Division line for when happiness bonuses begin. Happiness above threshold grants bonus. (should always be a positive value)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_THRESHOLD', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Division line for when happiness bonuses begin. Happiness above threshold grants bonus. (should always be a positive value)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_THRESHOLD', '10'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Maximum happiness bonus % mod. (should always be a positive value)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_BONUS_MAXIMUM', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Maximum happiness bonus % mod. (should always be a positive value)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_BONUS_MAXIMUM', '20'
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
	SELECT 'BALANCE_HAPPINESS_PENALTY_MAXIMUM', '-35'
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
	SELECT 'BALANCE_HAPPINESS_SCIENCE_MODIFIER', '2'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );
 
--Gold % point per happiness mod (should always be a positive value).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_GOLD_MODIFIER', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

--Gold % point per happiness mod (should always be a positive value).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_GOLD_MODIFIER', '2'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Production % point per happiness mod (should always be a positive value).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_PRODUCTION_MODIFIER', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Production % point per happiness mod (should always be a positive value).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_PRODUCTION_MODIFIER', '2'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Food % point per happiness mod (should always be a positive value).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_FOOD_MODIFIER', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Food % point per happiness mod (should always be a positive value).
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_FOOD_MODIFIER', '2'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Faith point per happiness mod (should always be a positive value). THIS ONE DIVIDES BY YOUR HAPPINESS VALUE (SO /5 HAPPINESS PER TURN)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_FAITH_MODIFIER', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Faith point per happiness mod (should always be a positive value). THIS ONE DIVIDES BY YOUR HAPPINESS VALUE (SO /5 HAPPINESS PER TURN)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_FAITH_MODIFIER', '5'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Culture point per happiness mod (should always be a positive value). THIS ONE DIVIDES BY YOUR HAPPINESS VALUE (SO /5 HAPPINESS PER TURN)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_CULTURE_MODIFIER', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Culture point per happiness mod (should always be a positive value). THIS ONE DIVIDES BY YOUR HAPPINESS VALUE (SO /5 HAPPINESS PER TURN)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_CULTURE_MODIFIER', '5'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Text for city view tooltip.
	INSERT INTO Language_en_US (
	Text, Tag)
	SELECT '[NEWLINE][ICON_BULLET]Modified due to Happiness: {1_Num}%', 'TXT_KEY_PRODMOD_BALANCE_HAPPINESS_MOD'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Text for city view tooltip.
	INSERT INTO Language_en_US (
	Text, Tag)
	SELECT '[NEWLINE][ICON_BULLET]Modified due to Happiness: {1_Num}%', 'TXT_KEY_PRODMOD_BALANCE_HAPPINESS_MOD'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Text for city view tooltip.
	UPDATE Language_en_US
	Set Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] from Happiness and/or a Golden Age.'
	WHERE Tag = 'TXT_KEY_TP_CULTURE_FROM_GOLDEN_AGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );


-- Update text for top panel depending on which yields you have enabled above. Change as desired.
	UPDATE Language_en_US
	SET Text = 'Cities produce less while the empire is [ICON_HAPPINESS_3] unhappy. [NEWLINE]City and empire-wide yields are reduced for each point of Unhappiness. Units in combat will also fight less effectively.'
	WHERE Tag = 'TXT_KEY_TP_EMPIRE_UNHAPPY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );