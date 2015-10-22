-- Change instructions for Ideologies

UPDATE Language_en_US
SET Text = 'The three Ideology trees, Freedom, Order, and Autocracy, have greatly expanded in Brave New World. All civilizations in the game, on completing three Social Policy branches after the Renaissance Era, or upon reaching the Modern Era, will be required to choose an Ideology for their civilization. Each Ideology tree contains 3 tiers of "tenets" that you use to customize your Ideology, with the third and final tier holding the most powerful benefits. As with regular Social Policies, players use Culture to purchase additional tenets as they move through the game.[NEWLINE][NEWLINE]Civilizations that share a common Ideology will receive benefits with their diplomatic relationships. Civilizations that have conflicting Ideologies have multiple side-effects. For example, a negative effect on their diplomatic relationship and happiness penalties take effect if an opposing Ideology has a stronger Cultural influence on your people. If you let your people become too unhappy, there is a chance that your cities may declare that they are joining your opponents empire. As a last resort, you, or other players, can resort to a "Revolution" to switch Ideologies to one that is preferable to your people.[NEWLINE][NEWLINE]For more information on Ideological Tenets, click on the "Social Policies" tab along the top of the Civilopedia, and scroll down to the Order, Freedom, and Autocracy sections.'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_IDEOLOGY_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Players interested in creating massive, sprawling civs should turn to the Order ideology, as the strength of the empire is determined by the total number of cities it contains. This ideology unlocks upon entering the Modern Era or completing three Social Policy branches after the Renaissance Era, whichever comes first.'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_ORDER_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Autocracy ideology is well suited for those wishing nothing more than to crush their foes under the weight of their iron-plated boots. This ideology unlocks upon entering the Modern Era or completing three Social Policy branches after the Renaissance Era, whichever comes first.'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_AUTOCRACY_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Freedom ideology provides bonuses for Culture, Tourism, Specialist production, and more. This ideology unlocks upon entering the Modern Era or completing three Social Policy branches after the Renaissance Era, whichever comes first.'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_IDEOLOGY_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Your people now consider themselves part of the Modern Era, and the ideas of modernization permeate your society. Your people clamor for you to pick an Ideology for your civilization.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CHOOSE_IDEOLOGY_FACTORIES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Ideological tenets may not be purchased until you have chosen an Ideology. This choice becomes available if you are in the Modern Era or have completed three Social Policy branches after the Renaissance Era, whichever comes first.'
WHERE Tag = 'TXT_KEY_POLICYSCREEN_IDEOLOGY_NOT_STARTED' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

-- Tourism stuff

UPDATE Defines
SET Value = '10'
WHERE Name = 'CULTURE_LEVEL_EXOTIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );
		
UPDATE Defines
SET Value = '30'
WHERE Name = 'CULTURE_LEVEL_FAMILIAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );
		
UPDATE Defines
SET Value = '60'
WHERE Name = 'CULTURE_LEVEL_POPULAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Defines
SET Value = '1'
WHERE Name = 'BASE_CULTURE_PER_GREAT_WORK' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Defines
SET Value = '25'
WHERE Name = 'TOURISM_MODIFIER_SHARED_RELIGION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

--- These 5 values change the amount of science earned from trade routes with influence civs. Each is adding to an incremental value (so Familiar is already 1, Popular already 2, etc.)
INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_SCIENCE_INFLUENCE_LEVEL_EXOTIC', '1'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_SCIENCE_INFLUENCE_LEVEL_FAMILIAR', '2'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );
	
INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_SCIENCE_INFLUENCE_LEVEL_POPULAR', '3'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );
	
INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_SCIENCE_INFLUENCE_LEVEL_INFLUENTIAL', '4'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );
	
INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_SCIENCE_INFLUENCE_LEVEL_DOMINANT', '5'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );
	
--- Reduce turns for conquest of cities. Base is 25 for each level of influence, this adds to it.

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_CONQUEST_REDUCTION_BOOST', '5'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

--- Set turns for spies to est. in cities based on influence.

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_SPY_BOOST_INFLUENCE_EXOTIC', '5'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_SPY_BOOST_INFLUENCE_FAMILIAR', '4'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_SPY_BOOST_INFLUENCE_POPULAR', '3'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_SPY_BOOST_INFLUENCE_INFLUENTIAL', '2'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_SPY_BOOST_INFLUENCE_DOMINANT', '1'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

--- These 5 values change the amount of gold earned from trade routes with influenced civs. Higher influence = more gold (trade-off for higher influence).
	
INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_GOLD_INFLUENCE_LEVEL_EXOTIC', '200'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_GOLD_INFLUENCE_LEVEL_FAMILIAR', '400'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );
	
INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_GOLD_INFLUENCE_LEVEL_POPULAR', '600'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );
	
INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_GOLD_INFLUENCE_LEVEL_INFLUENTIAL', '800'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );
	
INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_GOLD_INFLUENCE_LEVEL_DOMINANT', '1000'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_GROWTH_INFLUENCE_LEVEL_EXOTIC', '3'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_GROWTH_INFLUENCE_LEVEL_FAMILIAR', '6'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );
	
INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_GROWTH_INFLUENCE_LEVEL_POPULAR', '9'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );
	
INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_GROWTH_INFLUENCE_LEVEL_INFLUENTIAL', '12'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );
	
INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_GROWTH_INFLUENCE_LEVEL_DOMINANT', '15'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

-- Update text for diplomat.
UPDATE Language_en_US
SET Text = '+{1_Num}% Bonus from Diplomat[NEWLINE]'
WHERE Tag = 'TXT_KEY_CO_PLAYER_TOURISM_PROPAGANDA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

-- Update text for tooltips based on what you changed above.
INSERT INTO Language_en_US (
Text, Tag)
SELECT '[NEWLINE][NEWLINE][COLOR_CYAN]Influence Benefits:[ENDCOLOR][NEWLINE][COLOR_POSITIVE_TEXT]Trade Routes to this Civ Generate:[ENDCOLOR][NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+3%[ENDCOLOR] [ICON_FOOD] Growth in Origin City[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+2[ENDCOLOR] [ICON_GOLD] Gold[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+1[ENDCOLOR] [ICON_RESEARCH] Science (if already gaining [ICON_RESEARCH] Science)[NEWLINE][COLOR_POSITIVE_TEXT]Espionage Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [ICON_SPY] Spies Establish Surveillance in [COLOR_POSITIVE_TEXT]5[ENDCOLOR] turns[NEWLINE][COLOR_POSITIVE_TEXT]City Conquest Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-15%[ENDCOLOR] [ICON_RESISTANCE] Unrest Time[NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-15%[ENDCOLOR] [ICON_CITIZEN] Citizen loss from City conquest' , 'TXT_KEY_CO_INFLUENCE_BONUSES_EXOTIC'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

-- Update texts for tooltips based on what you changed above.
UPDATE Language_en_US
SET Text = '[NEWLINE][NEWLINE][COLOR_CYAN]Influence Benefits:[ENDCOLOR][NEWLINE][COLOR_POSITIVE_TEXT]Trade Routes to this Civ Generate:[ENDCOLOR][NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+6%[ENDCOLOR] [ICON_FOOD] Growth in Origin City[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+4[ENDCOLOR] [ICON_GOLD] Gold[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+2[ENDCOLOR] [ICON_RESEARCH] Science (if already gaining [ICON_RESEARCH] Science)[NEWLINE][COLOR_POSITIVE_TEXT]Espionage Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [ICON_SPY] Spies Establish Surveillance in [COLOR_POSITIVE_TEXT]4[ENDCOLOR] turns[NEWLINE][COLOR_POSITIVE_TEXT]City Conquest Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-30%[ENDCOLOR] [ICON_RESISTANCE] Unrest Time[NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-30%[ENDCOLOR] [ICON_CITIZEN] Citizen loss from City conquest'
WHERE Tag = 'TXT_KEY_CO_INFLUENCE_BONUSES_FAMILIAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

-- Update texts for tooltips based on what you changed above.
UPDATE Language_en_US
SET Text = '[NEWLINE][NEWLINE][COLOR_CYAN]Influence Benefits:[ENDCOLOR][NEWLINE][COLOR_POSITIVE_TEXT]Trade Routes to this Civ Generate:[ENDCOLOR][NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+9%[ENDCOLOR] [ICON_FOOD] Growth in Origin City[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+6[ENDCOLOR] [ICON_GOLD] Gold[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+3[ENDCOLOR] [ICON_RESEARCH] Science (if already gaining [ICON_RESEARCH] Science)[NEWLINE][COLOR_POSITIVE_TEXT]Espionage Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [ICON_SPY] Spies Establish Surveillance in [COLOR_POSITIVE_TEXT]3[ENDCOLOR] turns[NEWLINE][COLOR_POSITIVE_TEXT]City Conquest Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-55%[ENDCOLOR] [ICON_RESISTANCE] Unrest Time[NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-55%[ENDCOLOR] [ICON_CITIZEN] Citizen loss from City conquest'
WHERE Tag = 'TXT_KEY_CO_INFLUENCE_BONUSES_POPULAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

-- Update texts for tooltips based on what you changed above.
UPDATE Language_en_US
SET Text = '[NEWLINE][NEWLINE][COLOR_CYAN]Influence Benefits:[ENDCOLOR][NEWLINE][COLOR_POSITIVE_TEXT]Trade Routes to this Civ Generate:[ENDCOLOR][NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+12%[ENDCOLOR] [ICON_FOOD] Growth in Origin City[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+8[ENDCOLOR] [ICON_GOLD] Gold[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+4[ENDCOLOR] [ICON_RESEARCH] Science (if already gaining [ICON_RESEARCH] Science)[NEWLINE][COLOR_POSITIVE_TEXT]Espionage Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [ICON_SPY] Spies Establish Surveillance in [COLOR_POSITIVE_TEXT]2[ENDCOLOR] turns[NEWLINE][COLOR_POSITIVE_TEXT]City Conquest Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-80%[ENDCOLOR] [ICON_RESISTANCE] Unrest Time[NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-80%[ENDCOLOR] [ICON_CITIZEN] Citizen loss from City conquest'
WHERE Tag = 'TXT_KEY_CO_INFLUENCE_BONUSES_INFLUENTIAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

-- Update texts for tooltips based on what you changed above.
UPDATE Language_en_US
SET Text = '[NEWLINE][NEWLINE][COLOR_CYAN]Influence Benefits:[ENDCOLOR][NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Trade Routes to this Civ Generate:[ENDCOLOR][NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+15%[ENDCOLOR] [ICON_FOOD] Growth in Origin City[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+10[ENDCOLOR] [ICON_GOLD] Gold[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+5[ENDCOLOR] [ICON_RESEARCH] Science (if already gaining [ICON_RESEARCH] Science)[NEWLINE][COLOR_POSITIVE_TEXT]Espionage Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [ICON_SPY] Spies Establish Surveillance in [COLOR_POSITIVE_TEXT]1[ENDCOLOR] turn[NEWLINE][COLOR_POSITIVE_TEXT]City Conquest Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-100%[ENDCOLOR] [ICON_RESISTANCE] Unrest Time[NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-100%[ENDCOLOR] [ICON_CITIZEN] Citizen loss from City conquest'
WHERE Tag = 'TXT_KEY_CO_INFLUENCE_BONUSES_DOMINANT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

-- Text for trade view tooltip.
INSERT INTO Language_en_US (
Text, Tag)
SELECT '+{2_Num} [ICON_GOLD] Gold due to your Cultural Influence over {1_CivName}', 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_GOLD_EXPLAINED'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

-- Text for trade view tooltip.
INSERT INTO Language_en_US (
Text, Tag)
SELECT '+{2_Num} [ICON_GOLD] Gold due to the Cultural Influence of {1_CivName} over you', 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_GOLD_EXPLAINED'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

-- Text changes for Vote Purchase
UPDATE Language_en_US
SET Text = 'In the next World Congress session, [COLOR_POSITIVE_TEXT]{1_NumVotes}[ENDCOLOR] {1_NumVotes: plural 1?Delegate; other?Delegates;} currently controlled by this player will support {2_ChoiceText} on the proposal to [COLOR_POSITIVE_TEXT]enact[ENDCOLOR] these changes:[NEWLINE][NEWLINE]{3_ProposalText}[NEWLINE][NEWLINE]The number of delegates above is based on the number of delegates this player controls, and the Rank of your [ICON_DIPLOMAT] Diplomat. Higher ranks allow you to trade for more delegates.'
WHERE Tag = 'TXT_KEY_DIPLO_VOTE_TRADE_ENACT_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'In the next World Congress session, [COLOR_POSITIVE_TEXT]{1_NumVotes}[ENDCOLOR] {1_NumVotes: plural 1?Delegate; other?Delegates;} currently controlled by this player will support {2_ChoiceText} on the proposal to [COLOR_WARNING_TEXT]repeal[ENDCOLOR] these effects:[NEWLINE][NEWLINE]{3_ProposalText}[NEWLINE][NEWLINE]The number of delegates above is based on the number of delegates this player controls, and the rank of your [ICON_DIPLOMAT] Diplomat. Higher ranks allow you to trade for more delegates.'
WHERE Tag = 'TXT_KEY_DIPLO_VOTE_TRADE_REPEAL_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Artifact will be placed in nearest Great Work of Art slot. Artifact provides +1 [ICON_CULTURE] Culture and +2 [ICON_TOURISM] Tourism. Archaeologist will be consumed.'
WHERE Tag = 'TXT_KEY_CHOOSE_ARCH_ARTIFACT_RESULT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Ancient writing will be placed in nearest Great Work of Writing slot. Writing provides +1 [ICON_CULTURE] Culture and +2 [ICON_TOURISM] Tourism. Archaeologist will be consumed.'
WHERE Tag = 'TXT_KEY_CHOOSE_ARCH_WRITTEN_ARTIFACT_RESULT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

-- Theming Bonuses increased in all buildings
		
UPDATE Building_ThemingBonuses
SET Bonus = '6'
WHERE BuildingType = 'BUILDING_UFFIZI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );
		
UPDATE Building_ThemingBonuses
SET Bonus = '4'
WHERE BuildingType = 'BUILDING_GLOBE_THEATER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );
		
UPDATE Building_ThemingBonuses
SET Bonus = '6'
WHERE BuildingType = 'BUILDING_BROADWAY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );
		
UPDATE Building_ThemingBonuses
SET Bonus = '6'
WHERE BuildingType = 'BUILDING_HERMITAGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Building_ThemingBonuses
SET Bonus = '4'
WHERE BuildingType = 'BUILDING_OXFORD_UNIVERSITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Building_ThemingBonuses
SET Bonus = '4'
WHERE BuildingType = 'BUILDING_SISTINE_CHAPEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Building_ThemingBonuses
SET Bonus = '8'
WHERE BuildingType = 'BUILDING_LOUVRE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Building_ThemingBonuses
SET Bonus = '4'
WHERE BuildingType = 'BUILDING_GREAT_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Building_ThemingBonuses
SET Bonus = '4'
WHERE BuildingType = 'BUILDING_SYDNEY_OPERA_HOUSE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Building_ThemingBonuses
SET Bonus = '3'
WHERE BuildingType = 'BUILDING_MUSEUM' AND Description = 'TXT_KEY_THEMING_BONUS_MUSEUM_1' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Building_ThemingBonuses
SET Bonus = '3'
WHERE BuildingType = 'BUILDING_MUSEUM' AND Description = 'TXT_KEY_THEMING_BONUS_MUSEUM_2' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Building_ThemingBonuses
SET Bonus = '2'
WHERE BuildingType = 'BUILDING_MUSEUM' AND Description = 'TXT_KEY_THEMING_BONUS_MUSEUM_3' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Building_ThemingBonuses
SET Bonus = '2'
WHERE BuildingType = 'BUILDING_MUSEUM' AND Description = 'TXT_KEY_THEMING_BONUS_MUSEUM_4' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Building_ThemingBonuses
SET Bonus = '2'
WHERE BuildingType = 'BUILDING_MUSEUM' AND Description = 'TXT_KEY_THEMING_BONUS_MUSEUM_5' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Building_ThemingBonuses
SET Bonus = '2'
WHERE BuildingType = 'BUILDING_MUSEUM' AND Description = 'TXT_KEY_THEMING_BONUS_MUSEUM_6' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Building_ThemingBonuses
SET Bonus = '2'
WHERE BuildingType = 'BUILDING_MUSEUM' AND Description = 'TXT_KEY_THEMING_BONUS_MUSEUM_7' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Building_ThemingBonuses
SET Bonus = '2'
WHERE BuildingType = 'BUILDING_MUSEUM' AND Description = 'TXT_KEY_THEMING_BONUS_MUSEUM_8' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );




