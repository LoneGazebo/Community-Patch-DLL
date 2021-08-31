-- Foreign Office Culture Boost

INSERT INTO Policy_BuildingClassCultureChanges
(PolicyType, BuildingClassType, CultureChange)
SELECT 'POLICY_CONSULATES', 'BUILDINGCLASS_FOREIGN_OFFICE', '3'
WHERE EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

-- School of Scribes Happiness Boost

INSERT INTO Policy_BuildingClassHappiness
(PolicyType, BuildingClassType, Happiness)
SELECT 'POLICY_PHILANTHROPY', 'BUILDINGCLASS_SCRIBE', '1'
WHERE EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

-- Gutenberg Press Gold Boost

INSERT INTO Policy_BuildingClassYieldModifiers
(PolicyType, BuildingClassType, YieldType, YieldMod)
SELECT 'POLICY_CULTURAL_DIPLOMACY', 'BUILDINGCLASS_GUTENBERG_PRESS', 'YIELD_GOLD','10'
WHERE EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

-- Post Office Science Boost

INSERT INTO Policy_BuildingClassYieldModifiers
(PolicyType, BuildingClassType, YieldType, YieldMod)
SELECT 'POLICY_SCHOLASTICISM', 'BUILDINGCLASS_POSTOFFICE', 'YIELD_SCIENCE','10'
WHERE EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

-- Educated Elite Free Units (Pt. 1)

--INSERT INTO Policy_FreeUnitClasses
--(PolicyType, UnitClassType, Count)
--SELECT 'POLICY_MERCHANT_CONFEDERACY', 'UNITCLASS_DIPLOMAT', '1'
--WHERE EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

-- Educated Elite Free Units (Pt. 2)

--UPDATE Policies
--Set IncludesOneShotFreeUnits='true'
--WHERE TYPE='POLICY_MERCHANT_CONFEDERACY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

-- Patronage Text


UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Cultural Diplomacy[ENDCOLOR][NEWLINE] Receive 1 [ICON_INFLUENCE] Influence per turn with City-States with which you have Trade Routes. The [COLOR_POSITIVE_TEXT]Gutenberg Press[ENDCOLOR] boosts [ICON_GOLD] Gold production by 10%.'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_DIPLOMACY_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value=1) AND EXISTS (SELECT * FROM CSD WHERE Type='USING_VEM' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Philanthropy[ENDCOLOR][NEWLINE]Quantity of Resources gifted by City-States increased by 100%. [ICON_HAPPINESS] Happiness from gifted Luxuries increased by 50%. [COLOR_POSITIVE_TEXT]Schools of Scribes[ENDCOLOR] produce 1 [ICON_HAPPINESS_1] Happiness.'
WHERE Tag = 'TXT_KEY_POLICY_PHILANTHROPY_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value=1) AND EXISTS (SELECT * FROM CSD WHERE Type='USING_VEM' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Consulates[ENDCOLOR][NEWLINE]Resting point for [ICON_INFLUENCE] Influence with all City-States is increased by 15.  The [COLOR_POSITIVE_TEXT]Foreign Office[ENDCOLOR] produces +3 [ICON_CULTURE] Culture.'
WHERE Tag = 'TXT_KEY_POLICY_CONSULATES_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value=1) AND EXISTS (SELECT * FROM CSD WHERE Type='USING_VEM' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Merchant Confederacy[ENDCOLOR][NEWLINE]+4 [ICON_GOLD] Gold for trade routes with City-States.'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_CONFEDERACY_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value=1) AND EXISTS (SELECT * FROM CSD WHERE Type='USING_VEM' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Scholasticism[ENDCOLOR][NEWLINE]All City-States which are [COLOR_POSITIVE_TEXT]Allies[ENDCOLOR] provide a [ICON_RESEARCH] Science bonus equal to 25% of what they produce for themselves. [COLOR_POSITIVE_TEXT]Telephone Networks[ENDCOLOR] boost [ICON_RESEARCH] Science production by 10%.'
WHERE Tag = 'TXT_KEY_POLICY_SCHOLASTICISM_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value=1) AND EXISTS (SELECT * FROM CSD WHERE Type='USING_VEM' AND Value= 1 );
