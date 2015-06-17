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
	
	UPDATE Defines
	SET Value = '0'
	WHERE Name = 'HAPPINESS_PER_NATURAL_WONDER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

	-- Golden Age starting requirement.
	UPDATE Defines
	SET Value = '700'
	WHERE Name = 'GOLDEN_AGE_BASE_THRESHOLD_HAPPINESS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

	-- Golden Age per GA increase.
	UPDATE Defines
	SET Value = '500'
	WHERE Name = 'GOLDEN_AGE_EACH_GA_ADDITIONAL_HAPPINESS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

	-- % reduction of combat effectiveness per point of unhappiness.
	UPDATE Defines
	SET Value = '0'
	WHERE Name = 'VERY_UNHAPPY_COMBAT_PENALTY_PER_UNHAPPY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

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
	SELECT 'BALANCE_HAPPINESS_BONUS_MAXIMUM', '0'
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
	SELECT 'BALANCE_HAPPINESS_PENALTY_MAXIMUM', '-25'
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
	SELECT 'BALANCE_HAPPINESS_FAITH_MODIFIER', '2'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Culture point per happiness mod (should always be a positive value). THIS ONE DIVIDES BY YOUR HAPPINESS VALUE (SO /5 HAPPINESS PER TURN)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_CULTURE_MODIFIER', '0'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Culture point per happiness mod (should always be a positive value). THIS ONE DIVIDES BY YOUR HAPPINESS VALUE (SO /5 HAPPINESS PER TURN)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_CULTURE_MODIFIER', '2'
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
	Set Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] from [ICON_HAPPINESS_1] Happiness and/or a Golden Age.'
	WHERE Tag = 'TXT_KEY_TP_CULTURE_FROM_GOLDEN_AGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Update Text for Very Unhappy and Super Unhappy

-- Update text for top panel depending on which yields you have enabled above. Change as desired.
	UPDATE Language_en_US
	SET Text = 'Your empire is [ICON_HAPPINESS_3] very unhappy![ENDCOLOR] [NEWLINE][NEWLINE]Empire-wide [ICON_CULTURE] Culture, [ICON_PEACE] Faith, [ICON_GOLD] Gold, [ICON_FOOD] Growth, and [ICON_RESEARCH] Science are [COLOR_NEGATIVE_TEXT]reduced by 2%[ENDCOLOR] for each point of [ICON_HAPPINESS_3] Unhappiness (up to -20 [ICON_HAPPINESS_3] Unhappiness)!'
	WHERE Tag = 'TXT_KEY_TP_EMPIRE_VERY_UNHAPPY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Because your [ICON_HAPPINESS_4] Unhappiness has reached 20, the Empire is is in revolt! [ENDCOLOR]Cities may abandon your empire and flip to the civilization that is most culturally influential over your people, and uprisings may occur with rebel (barbarian) units appearing in your territory!'
	WHERE Tag = 'TXT_KEY_TP_EMPIRE_SUPER_UNHAPPY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );
	
-- Update text for top panel depending on which yields you have enabled above. Change as desired.
	UPDATE Language_en_US
	SET Text = 'Your empire is [ICON_HAPPINESS_3] unhappy! [NEWLINE][NEWLINE][ENDCOLOR]Empire-wide [ICON_CULTURE] Culture, [ICON_PEACE] Faith, [ICON_GOLD] Gold, [ICON_FOOD] Growth, and [ICON_RESEARCH] Science are [COLOR_NEGATIVE_TEXT]reduced by 2%[ENDCOLOR] for each point of [ICON_HAPPINESS_3] Unhappiness (up to -20 [ICON_HAPPINESS_3] Unhappiness)!'
	WHERE Tag = 'TXT_KEY_TP_EMPIRE_UNHAPPY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- TOOLTIPS FOR TOP BAR

-- Text for city view tooltip.
	INSERT INTO Language_en_US (
	Text, Tag)
	SELECT '[COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] due to [ICON_HAPPINESS_1] Happiness.', 'TXT_KEY_TP_GOLD_GAINED_FROM_HAPPINESS'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );


-- Text for city view tooltip.
	INSERT INTO Language_en_US (
	Text, Tag)
	SELECT '{1_NUM} due to [ICON_HAPPINESS_3] Unhappiness.', 'TXT_KEY_TP_GOLD_LOST_FROM_UNHAPPINESS'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Text for city view tooltip.
	INSERT INTO Language_en_US (
	Text, Tag)
	SELECT '[ICON_BULLET] [COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] due to [ICON_HAPPINESS_1] Happiness.', 'TXT_KEY_TP_FAITH_GAINED_FROM_HAPPINESS'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );


-- Text for city view tooltip.
	INSERT INTO Language_en_US (
	Text, Tag)
	SELECT '[ICON_BULLET] [COLOR_NEGATIVE_TEXT]{1_NUM}[ENDCOLOR] due to [ICON_HAPPINESS_3] Unhappiness.', 'TXT_KEY_TP_FAITH_LOST_FROM_UNHAPPINESS'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Text for city view tooltip.
	INSERT INTO Language_en_US (
	Text, Tag)
	SELECT '[ICON_BULLET] [COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] [ICON_RESEARCH] due to [ICON_HAPPINESS_1] Happiness.', 'TXT_KEY_TP_SCIENCE_GAINED_FROM_HAPPINESS'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );


-- Text for city view tooltip.
	INSERT INTO Language_en_US (
	Text, Tag)
	SELECT '[ICON_BULLET] [COLOR_NEGATIVE_TEXT]{1_Num}[ENDCOLOR] [ICON_RESEARCH] due to [ICON_HAPPINESS_3] Unhappiness.', 'TXT_KEY_TP_SCIENCE_LOST_FROM_UNHAPPINESS'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Text for city view tooltip.
	INSERT INTO Language_en_US (
	Text, Tag)
	SELECT '[ICON_BULLET] [COLOR_NEGATIVE_TEXT]{1_NUM}[ENDCOLOR] due to due to [ICON_HAPPINESS_3] Unhappiness.', 'TXT_KEY_TP_CULTURE_LOST_FROM_UNHAPPINESS'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Text for city view tooltip.
	INSERT INTO Language_en_US (
	Text, Tag)
	SELECT '[ICON_BULLET] [COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] due to [ICON_HAPPINESS_1] Happiness.', 'TXT_KEY_TP_CULTURE_GAINED_FROM_HAPPINESS'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Happiness Flavors
	UPDATE AIEconomicStrategy_City_Flavors
	SET Flavor = '-10'
	WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_NEED_HAPPINESS_CRITICAL' AND FlavorType = 'FLAVOR_GROWTH';