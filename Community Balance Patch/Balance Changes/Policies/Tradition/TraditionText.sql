-- Opener
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Tradition[ENDCOLOR] focuses on building robust cities and an impressive Capital.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting Tradition grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +2 [ICON_CULTURE] Culture in the [ICON_CAPITAL] Capital. [NEWLINE] [ICON_BULLET] +5% [ICON_FOOD] Growth in all Cities.[NEWLINE] [ICON_BULLET] Unlocks building the [COLOR_CYAN]Hanging Gardens[ENDCOLOR].[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Each Tradition policy unlocked grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +3% [ICON_FOOD] Growth in all Cities.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting all Policies in Tradition grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +1 [ICON_HAPPINESS_1] Happiness from National Wonders with [ICON_CITIZEN] Population requirements. [NEWLINE] [ICON_BULLET] Grants access to Medieval Era Policy Branches, ignoring Era requirement.Provides [COLOR_CYAN]1[ENDCOLOR] point (of [COLOR_CYAN]3[ENDCOLOR] required in total) towards unlocking access to Ideologies.[NEWLINE] [ICON_BULLET] Allows for the purchase of [ICON_GREAT_ENGINEER] Great Engineers with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_TRADITION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Aristocracy
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Aristocracy[ENDCOLOR][NEWLINE] +1 [ICON_CULTURE] Culture in every City. +1 [ICON_GOLD] Gold in the Capital for every 5 [ICON_CITIZEN] Citizens.'
WHERE Tag = 'TXT_KEY_POLICY_ARISTOCRACY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Oligarchy
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Oligarchy[ENDCOLOR][NEWLINE] +1 [ICON_GOLD] Gold in every City. +1 [ICON_CULTURE] Culture in the Capital for every 5 [ICON_CITIZEN] Citizens.'
WHERE Tag = 'TXT_KEY_POLICY_OLIGARCHY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Legalism
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Legalism[ENDCOLOR][NEWLINE][ICON_CAPITAL] Capital [ICON_FOOD] Growth increased by 15%. [ICON_GOLDEN_AGE] Golden Ages last 25% longer.'
WHERE Tag = 'TXT_KEY_POLICY_LEGALISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Landed Elite
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Landed Elite[ENDCOLOR][NEWLINE]+2 [ICON_FOOD] Food in every City. Internal [ICON_INTERNATIONAL_TRADE] Trade Routes are 33% more effective.'
WHERE Tag = 'TXT_KEY_POLICY_LANDED_ELITE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Monarchy
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Monarchy[ENDCOLOR][NEWLINE] [ICON_CAPITAL] Capital gains a free Engineer Specialist slot. [ICON_GREAT_PEOPLE] Great People rate increased by 25% in every city.'
WHERE Tag = 'TXT_KEY_POLICY_MONARCHY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Finisher