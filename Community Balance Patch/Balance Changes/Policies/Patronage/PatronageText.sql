-- Opener
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Patronage[ENDCOLOR] enhances the benefits of City-State friendship and Global Diplomacy.[NEWLINE][NEWLINE]Adopting Patronage will cause [ICON_INFLUENCE] Influence with City-States to degrade 25% slower than normal and raise the resting point for [ICON_INFLUENCE] Influence with all City-States by 5, with each subsequent Patronage policy you unlock increasing this by 3. Unlocks building the Forbidden Palace.[NEWLINE][NEWLINE]Adopting all policies in the Patronage tree will cause allied City-States to occasionally gift you [ICON_GREAT_PEOPLE] Great People.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PATRONAGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Philanthropy
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Philanthropy[ENDCOLOR][NEWLINE]Receive double [ICON_INFLUENCE] Influence from Quests completed for City-States, and +10 [ICON_INFLUENCE] Influence with all known City-States every time you expend a [ICON_GREAT_PEOPLE] Great Person.'
WHERE Tag = 'TXT_KEY_POLICY_PHILANTHROPY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Consulates
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Consulates[ENDCOLOR][NEWLINE]Gain two additional votes in the World Congress, and the change of rigging elections in City-States is increased by 33%.'
WHERE Tag = 'TXT_KEY_POLICY_CONSULATES_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Scholasticism
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Scholasticism[ENDCOLOR][NEWLINE]All City-States which are [COLOR_POSITIVE_TEXT]Allies[ENDCOLOR] provide a [ICON_RESEARCH] Science bonus equal to 33% of what they produce for themselves.'
WHERE Tag = 'TXT_KEY_POLICY_SCHOLASTICISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Cultural Diplomacy
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Cultural Diplomacy[ENDCOLOR][NEWLINE]Quantity of Resources gifted by City-States increased by 100%. [ICON_HAPPINESS_1] Happiness from gifted Luxuries increased by 100%.'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_DIPLOMACY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Merchant Confederacy
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Merchant Confederacy[ENDCOLOR][NEWLINE]+3 [ICON_GOLD] Gold and +1 [ICON_INFLUENCE] Influence (per turn) for each [ICON_INTERNATIONAL_TRADE] Trade Route with a City-State.'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_CONFEDERACY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );
