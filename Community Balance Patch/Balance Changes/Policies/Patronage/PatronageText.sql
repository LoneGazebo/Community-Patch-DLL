-- Opener
UPDATE Language_en_US
SET Text = 'STATECRAFT'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PATRONAGE_CAP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Statecraft'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PATRONAGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Statecraft[ENDCOLOR] improves your abiltiy to interact with City-States, Spies, and the World Congress.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting Statecraft grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] Resting point for [ICON_INFLUENCE] Influence with all City-States is increased by 20. [NEWLINE] [ICON_BULLET] Receive +50% [ICON_INFLUENCE] Influence from Quests completed for City-States.[NEWLINE] [ICON_BULLET] Unlocks building the [COLOR_CYAN]Forbidden Palace[ENDCOLOR].[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Each Statecraft policy unlocked grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET]Your City-State [ICON_INFLUENCE] Influence degrades 5% slower.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting all policies in Statecraft grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] Allied City-States will occasionally gift you [ICON_GREAT_PEOPLE] Great People. [NEWLINE] [ICON_BULLET] +5 [ICON_INFLUENCE] Influence with all known City-States every time you expend a [ICON_GREAT_PEOPLE] Great Person. [NEWLINE] [ICON_BULLET] Grants access to Renaissance Era Policy Branches, ignoring Era requirement. [NEWLINE] [ICON_BULLET] Provides [COLOR_CYAN]1[ENDCOLOR] point (of [COLOR_CYAN]3[ENDCOLOR] required in total) towards unlocking access to Ideologies.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PATRONAGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Statecraft is the domain of the wealthy and powerful, and is the capacity of a political entity to interact with other political entities. In many societies, statecraft is managed by a few individuals: the Royal Family, say, or the merchant princes. Today, statecraft is largely a democratic process, though some things remain the exclusive domain of those with enough influence.'
WHERE Tag = 'TXT_KEY_POLICY_PATRONAGE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Philanthropy
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Philanthropy[ENDCOLOR][NEWLINE]Receive one or more additional [ICON_SPY] Spies (based on number of City-States in game).'
WHERE Tag = 'TXT_KEY_POLICY_PHILANTHROPY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Consulates
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Consulates[ENDCOLOR][NEWLINE]Gain an additional Delegate in the World Congress for every 8 City-States in the game, and the chance of rigging elections in City-States is increased by 50%.'
WHERE Tag = 'TXT_KEY_POLICY_CONSULATES_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Scholasticism
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Scholasticism[ENDCOLOR][NEWLINE]All City-States which are [COLOR_POSITIVE_TEXT]Allies[ENDCOLOR] provide a [ICON_RESEARCH] Science bonus equal to 33% of what they produce for themselves.'
WHERE Tag = 'TXT_KEY_POLICY_SCHOLASTICISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Cultural Diplomacy
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Cultural Diplomacy[ENDCOLOR][NEWLINE]Quantity of Resources gifted by City-States increased by 100%. [ICON_HAPPINESS_1] Happiness from gifted Luxuries increased by 50%.'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_DIPLOMACY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Merchant Confederacy
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Merchant Confederacy[ENDCOLOR][NEWLINE]+3 [ICON_GOLD] Gold from [ICON_INTERNATIONAL_TRADE] Trade Routes with City-States. Gain +1 [ICON_INFLUENCE] Influence (per turn) with City-States that you have a [ICON_INTERNATIONAL_TRADE] Trade Route with.'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_CONFEDERACY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );
