-- Liberty Opener
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Liberty[ENDCOLOR] is best for civilizations which desire robust infrastructure.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting Liberty grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] 5 [ICON_CULTURE] Culture every time you research a Technology or construct a Building. [NEWLINE] [ICON_BULLET] +5% [ICON_PRODUCTION] Production towards buildings.[NEWLINE] [ICON_BULLET] Unlocks building the [COLOR_CYAN]Parthenon[ENDCOLOR].[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Each Liberty policy unlocked grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +2% [ICON_PRODUCTION] Production towards buildings.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting all policies in Liberty grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] A free [ICON_GREAT_PEOPLE] Great Person of your choice appears near the [ICON_CAPITAL] Capital. [NEWLINE] [ICON_BULLET] Grants access to Medieval Era Policy Branches, ignoring Era requirement.[NEWLINE] [ICON_BULLET] Provides [COLOR_CYAN]1[ENDCOLOR] point (of [COLOR_CYAN]4[ENDCOLOR] required in total) towards unlocking access to Ideologies.[NEWLINE] [ICON_BULLET] Allows for the purchase of [ICON_GREAT_WRITER] Great Writers with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_LIBERTY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Collective Rule (Now City Planning)
UPDATE Language_en_US
SET Text = 'City Planning'
WHERE Tag = 'TXT_KEY_POLICY_COLLECTIVE_RULE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]City Planning[ENDCOLOR][NEWLINE] Receive 20 [ICON_GOLD] Gold when a [ICON_CITIZEN] Citizen is born in any city, and 20 [ICON_CULTURE] Culture when a [ICON_CITIZEN] Citizen is born in your Capital. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_POLICY_COLLECTIVE_RULE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Urban planning (urban, merged urban regions, regional, city, and town planning) is a technical and political process concerned with the use of land and design of the urban environment, including air and water and infrastructure passing into and out of urban areas such as transportation and distribution networks. Urban planning guides and ensures the orderly development of settlements and satellite communities which commute into and out of urban areas or share resources with it. It concerns itself with research and analysis, strategic thinking, architecture, urban design, public consultation, policy recommendations, implementation and management.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_COLLECTIVERULE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Citizenship

-- Republic (Now Civil Engineers)
UPDATE Language_en_US
SET Text = 'Civil Engineers'
WHERE Tag = 'TXT_KEY_POLICY_REPUBLIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Civil Engineers[ENDCOLOR][NEWLINE] +1 [ICON_PRODUCTION] Production in all cities. Road and Railroad [ICON_GOLD] Gold maintenance reduced by 50%.'
WHERE Tag = 'TXT_KEY_POLICY_REPUBLIC_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Civil engineering is a professional engineering discipline that deals with the design, construction, and maintenance of the physical and naturally built environment, including works like roads, bridges, canals, dams, and buildings. Civil engineering is the second-oldest engineering discipline after military engineering,[4] and it is defined to distinguish non-military engineering from military engineering.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_REPUBLIC_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Representation 
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Representation[ENDCOLOR][NEWLINE] 1 [ICON_HAPPINESS_1] Happiness for every 10 [ICON_CITIZEN] Citizens in your empire.'
WHERE Tag = 'TXT_KEY_POLICY_REPRESENTATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Meritocracy
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Meritocracy[ENDCOLOR][NEWLINE] +1 [ICON_MOVES] Movement for Workers and Settlers. When you expend a Great Person, receive 50 [ICON_GOLDEN_AGE] Golden Age Points and [ICON_RESEARCH] Science. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_POLICY_MERITOCRACY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Finisher