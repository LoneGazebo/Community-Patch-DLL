-- Opener
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Tradition[ENDCOLOR] is best for small empires.[NEWLINE][NEWLINE]Adopting Tradition grants 3 [ICON_CULTURE] Culture in the [ICON_CAPITAL] Capital, and 1 [ICON_FOOD] Food in the [ICON_CAPITAL] Capital for every unlocked Tradition policy. Unlocks building the Hanging Gardens.[NEWLINE][NEWLINE]Adopting all Policies in the Tradition tree will grant +25% to the [ICON_GREAT_PEOPLE] Great People Rate of all cities. It also allows the purchase of Great Engineers with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_TRADITION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Aristocracy
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Aristocracy[ENDCOLOR][NEWLINE]+15% [ICON_PRODUCTION] Production bonus to wonders. +1 [ICON_HAPPINESS_1] Happiness for each National Wonder.'
WHERE Tag = 'TXT_KEY_POLICY_ARISTOCRACY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Oligarchy
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Oligarchy[ENDCOLOR][NEWLINE][ICON_CAPITAL] Capital gains +1 [ICON_PRODUCTION] Production, +1 [ICON_PRODUCTION] for every 5 [ICON_CITIZEN] Citizens, and a free Engineer Specialist slot.'
WHERE Tag = 'TXT_KEY_POLICY_OLIGARCHY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Legalism
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Legalism[ENDCOLOR][NEWLINE][ICON_CAPITAL] Capital gains +1 [ICON_RESEARCH] Science, and +1 [ICON_RESEARCH] Science for every 5 [ICON_CITIZEN] Citizens.'
WHERE Tag = 'TXT_KEY_POLICY_LEGALISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Landed Elite
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Landed Elite[ENDCOLOR][NEWLINE]City borders expand 25% faster. Internal [ICON_INTERNATIONAL_TRADE] Trade Routes are 25% more effective.'
WHERE Tag = 'TXT_KEY_POLICY_LANDED_ELITE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Monarchy
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Monarchy[ENDCOLOR][NEWLINE]+1 [ICON_GOLD] Gold for every 2 [ICON_CITIZEN] Citizens in [ICON_CAPITAL] Capital. All [ICON_HAPPINESS_3] Unhappiness Global Averages reduced by 15% in [ICON_CAPITAL] Capital.'
WHERE Tag = 'TXT_KEY_POLICY_MONARCHY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Finisher