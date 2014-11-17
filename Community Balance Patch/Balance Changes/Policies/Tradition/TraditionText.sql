-- Opener
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Tradition[ENDCOLOR] focuses on [ICON_FOOD] Food and [ICON_GOLDEN_AGE] Golden Ages.[NEWLINE][NEWLINE]Adopting Tradition grants 2 [ICON_CULTURE] Culture in the [ICON_CAPITAL] Capital, and 1 [ICON_FOOD] Food in the [ICON_CAPITAL] Capital for every unlocked Tradition policy. Unlocks building the Hanging Gardens.[NEWLINE][NEWLINE]Adopting all Policies in the Tradition tree will grant +25% to the [ICON_GREAT_PEOPLE] Great People Rate of all cities. It also allows the purchase of Great Engineers with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_TRADITION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Aristocracy
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Aristocracy[ENDCOLOR][NEWLINE]+15% [ICON_PRODUCTION] Production bonus to World Wonders. All World Wonders produce +1 [ICON_CULTURE] Culture.'
WHERE Tag = 'TXT_KEY_POLICY_ARISTOCRACY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Oligarchy
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Oligarchy[ENDCOLOR][NEWLINE]+2 [ICON_FOOD] Food in every City, and +1 [ICON_PRODUCTION] Production in the Capital per every 4 [ICON_CITIZEN] Citizens. The Capital gains a free Engineer Specialist slot.'
WHERE Tag = 'TXT_KEY_POLICY_OLIGARCHY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Legalism
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Legalism[ENDCOLOR][NEWLINE][ICON_CAPITAL] Capital growth increased by 25%. [ICON_GOLDEN_AGE] Golden Ages last 25% longer.'
WHERE Tag = 'TXT_KEY_POLICY_LEGALISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Landed Elite
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Landed Elite[ENDCOLOR][NEWLINE]City borders expand 25% faster. Internal [ICON_INTERNATIONAL_TRADE] Trade Routes are 25% more effective.'
WHERE Tag = 'TXT_KEY_POLICY_LANDED_ELITE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Monarchy
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Monarchy[ENDCOLOR][NEWLINE]Receive 10 [ICON_Gold] Gold when a [ICON_CITIZEN] Citizen is born in any city, and 20 [ICON_GOLDEN_AGE] Golden Age Points when a [ICON_CITIZEN] Citizen is born in your Capital. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_POLICY_MONARCHY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Finisher