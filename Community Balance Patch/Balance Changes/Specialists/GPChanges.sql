-- Great Merchant Modification -- Town, not Customs House

UPDATE Language_en_US
SET Text = 'A town is a human settlement larger than a village but smaller than a city. The size definition for what constitutes a "town" varies considerably in different parts of the world. The word town shares an origin with the German word Zaun, the Dutch word tuin, and the Old Norse tun. The German word Zaun comes closest to the original meaning of the word: a fence of any material.[NEWLINE][NEWLINE]Towns receive +1 [ICON_GOLD] Gold if built on a Road that connects two owned Cities, and +2 [ICON_GOLD] if a Railroad. Receive additional [ICON_GOLD] Gold (+1 for Roads, +2 for Railroads) if a Trade Route, either internal or international, passes over this Town.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_CUSTOMS_HOUSE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'You can expend a Great Merchant to create a Town. When worked, a Town generates a lot of gold and food per turn for its city. [NEWLINE][NEWLINE]Towns receive +1 [ICON_GOLD] Gold if built on a Road that connects two owned Cities, and +2 [ICON_GOLD] if a Railroad. Receive additional [ICON_GOLD] Gold (+1 for Roads, +2 for Railroads) if a Trade Route, either internal or international, passes over this Town.'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_CUSTOMSHOUSE_HEADING4_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Construct a Town'
WHERE Tag = 'TXT_KEY_BUILD_CUSTOMS_HOUSE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Town'
WHERE Tag = 'TXT_KEY_IMPROVEMENT_CUSTOMS_HOUSE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Great Merchant can construct the special Town improvement which, when worked, produces [ICON_GOLD] Gold and [ICON_FOOD] Food. The Great Merchant can also journey to a city-state and perform a "trade mission" which produces gobs of [ICON_GOLD] Gold and garners many Influence Points with the city-state. The Great Merchant is expended when used in either of these ways.[NEWLINE][NEWLINE]Towns receive +1 [ICON_GOLD] Gold if built on a Road that connects two owned Cities, and +2 [ICON_GOLD] if a Railroad. Receive additional [ICON_GOLD] Gold (+1 for Roads, +2 for Railroads) if a Trade Route, either internal or international, passes over this Town.'
WHERE Tag = 'TXT_KEY_UNIT_GREAT_MERCHANT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'What does the town special improvement do?'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_CUSTOMSHOUSE_ADV_QUEST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Special Improvement: Town'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_CUSTOMSHOUSE_HEADING4_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

