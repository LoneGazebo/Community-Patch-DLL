-- Patronage Policy Tweaks

		-- Foreign Bureau Culture Boost

INSERT INTO Policy_BuildingClassYieldModifiers
(PolicyType, BuildingClassType, YieldType, YieldMod)
SELECT 'POLICY_CONSULATES', 'BUILDINGCLASS_PRINTING_PRESS' , 'YIELD_GOLD' , '10'
WHERE EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

		-- Chancery Happiness Boost

INSERT INTO Policy_BuildingClassHappiness
(PolicyType, BuildingClassType, Happiness)
SELECT 'POLICY_PHILANTHROPY', 'BUILDINGCLASS_CHANCERY', '1'
WHERE EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

		-- Printing Press Gold Boost

INSERT INTO Policy_BuildingClassCultureChanges
(PolicyType, BuildingClassType, CultureChange)
SELECT 'POLICY_CULTURAL_DIPLOMACY', 'BUILDINGCLASS_FOREIGN_OFFICE', '2'
WHERE EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

		-- Wire Service Science Boost

INSERT INTO Policy_BuildingClassYieldModifiers
(PolicyType, BuildingClassType, YieldType, YieldMod)
SELECT 'POLICY_SCHOLASTICISM', 'BUILDINGCLASS_WIRE_SERVICE', 'YIELD_SCIENCE','5'
WHERE EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

		-- Philanthropy Change

UPDATE Policies
SET MinorGoldFriendshipMod = 0
WHERE Type = 'POLICY_PHILANTHROPY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

UPDATE Policies
SET MinorResourceBonus = 'true'
WHERE Type = 'POLICY_PHILANTHROPY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

UPDATE Policies
SET GreatDiplomatRateModifier = 25
WHERE Type = 'POLICY_MERCHANT_CONFEDERACY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

		-- Cultural Diplomacy Change

UPDATE Policies
SET MinorResourceBonus = 'false'
WHERE Type = 'POLICY_CULTURAL_DIPLOMACY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

UPDATE Policies
SET ProtectedMinorPerTurnInfluence = 100
WHERE Type = 'POLICY_CULTURAL_DIPLOMACY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

		-- Treaty Organization Change

UPDATE Policies
SET ProtectedMinorPerTurnInfluence = 200
WHERE Type = 'POLICY_TREATY_ORGANIZATION' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

		-- Gunboat Diplomacy Change

UPDATE Policies
SET AfraidMinorPerTurnInfluence = 300
WHERE Type = 'POLICY_GUNBOAT_DIPLOMACY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

		-- Consulates Change

UPDATE Policies
SET MinorFriendshipMinimum = 15
WHERE Type = 'POLICY_CONSULATES' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

		-- Merchant Confederacy Change

UPDATE Policies
SET CityStateTradeChange = 300
WHERE Type = 'POLICY_MERCHANT_CONFEDERACY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

		-- Patronage Text Changes

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Cultural Diplomacy[ENDCOLOR][NEWLINE] Receive 1 [ICON_INFLUENCE] Influence per turn with City-States with which you have Trade Routes. The [COLOR_POSITIVE_TEXT]Foreign Bureau[ENDCOLOR] produces +2 [ICON_CULTURE] Culture.'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_DIPLOMACY_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value=1);

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Philanthropy[ENDCOLOR][NEWLINE]Quantity of Resources gifted by City-States increased by 100%. [ICON_HAPPINESS] Happiness from gifted Luxuries increased by 50%. The [COLOR_POSITIVE_TEXT]Chancery[ENDCOLOR] produces 1 [ICON_HAPPINESS_1] Happiness.'
WHERE Tag = 'TXT_KEY_POLICY_PHILANTHROPY_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value=1);

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Consulates[ENDCOLOR][NEWLINE]Resting point for [ICON_INFLUENCE] Influence with all City-States is increased by 15. The [COLOR_POSITIVE_TEXT]Printing Press[ENDCOLOR] boosts [ICON_GOLD] Gold production by 10%.'
WHERE Tag = 'TXT_KEY_POLICY_CONSULATES_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value=1);

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Merchant Confederacy[ENDCOLOR][NEWLINE]Earn Great Diplomats 25% faster and +3 [ICON_GOLD] Gold for trade routes with City-States.'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_CONFEDERACY_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value=1);

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Scholasticism[ENDCOLOR][NEWLINE]All City-States which are [COLOR_POSITIVE_TEXT]Allies[ENDCOLOR] provide a [ICON_RESEARCH] Science bonus equal to 25% of what they produce for themselves. The [COLOR_POSITIVE_TEXT]Wire Service[ENDCOLOR] boosts [ICON_RESEARCH] Science production by 5%.'
WHERE Tag = 'TXT_KEY_POLICY_SCHOLASTICISM_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value=1);

		-- Ideology Text Changes

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Treaty Organization[ENDCOLOR][NEWLINE]Gain 2 more [ICON_INFLUENCE] Influence per turn (at Standard speed) with City-States you have a trade route with.'
WHERE Tag = 'TXT_KEY_POLICY_TREATY_ORGANIZATION_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Gunboat Diplomacy[ENDCOLOR][NEWLINE]Gain 3 more [ICON_INFLUENCE] Influence (at Standard speed) per turn with City-States you could demand tribute from.  Your military forces are 50% more effective at intimidating City-States.'
WHERE Tag = 'TXT_KEY_POLICY_GUNBOAT_DIPLOMACY_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

-- Patronage Finisher

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Patronage[ENDCOLOR] enhances the benefits of City-State friendship.[NEWLINE][NEWLINE]Adopting Patronage will cause [ICON_INFLUENCE] Influence with City-States to degrade 25% slower than normal. Unlocks building the Forbidden Palace.[NEWLINE][NEWLINE]Adopting all policies in the Patronage tree will cause allied City-States to occasionally gift you [ICON_GREAT_PEOPLE] Great People. It also allows the purchase of Great Diplomats with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PATRONAGE_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );