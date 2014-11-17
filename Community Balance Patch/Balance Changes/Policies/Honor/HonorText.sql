-- Honor Opener/Finisher
UPDATE Language_en_US
SET Text = 'CONQUEST'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_HONOR_CAP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Conquest'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_HONOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Conquest[ENDCOLOR] will greatly benefit expansionists.[NEWLINE][NEWLINE]Adopting Honor gives a +33% combat bonus VS Barbarians, and notifications will be provided when new Barbarian Encampments spawn in revealed territory. Gain [ICON_CULTURE] Culture for the empire from each barbarian killed, each Camp cleared, and each City conquered. Unlocks building the Statue of Zeus.[NEWLINE][NEWLINE]Adopting all policies in the Conquest tree will reduce Policy cost due to number of cities by 25%, and will grant [ICON_GOLDEN_AGE] Golden Age Points from city conquest. It also allows the purchase of Great Generals with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_HONOR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Warrior Code (Now Imperialism)
UPDATE Language_en_US
SET Text = 'Imperialism'
WHERE Tag = 'TXT_KEY_POLICY_WARRIOR_CODE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Imperialism[ENDCOLOR][NEWLINE]No [ICON_HAPPINESS_3] Unhappiness from Isolation. A Free Settler appears in the [ICON_CAPITAL] Capital.'
WHERE Tag = 'TXT_KEY_POLICY_WARRIOR_CODE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Imperialism, as it is defined by the Dictionary of Human Geography, is an unequal human and territorial relationship, usually in the form of an empire, based on ideas of superiority and practices of dominance, and involving the extension of authority and control of one state or people over another.'
WHERE Tag = 'TXT_KEY_POLICY_WARRIORCODE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Discipline (Now Martial Law)
UPDATE Language_en_US
SET Text = 'Martial Law'
WHERE Tag = 'TXT_KEY_POLICY_DISCIPLINE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Martial Law[ENDCOLOR][NEWLINE][ICON_GOLD] Gold maintenance for Units reduced by 15%. Garrisons in [ICON_OCCUPIED] Occupied Cities reduce [ICON_HAPPINESS_3] Unhappiness by 50%.'
WHERE Tag = 'TXT_KEY_POLICY_DISCIPLINE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Martial law is the imposition of military power over designated regions on an emergency basis. Martial law is usually imposed on a temporary basis when the civilian government or civilian authorities fail to function effectively (e.g., maintain order and security, or provide essential services). In full-scale martial law, the highest-ranking military officer would take over, or be installed, as the military governor or as head of the government, thus removing all power from the previous executive, legislative, and judicial branches of government.'
WHERE Tag = 'TXT_KEY_POLICY_DISCIPLINE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Military Tradition (Now Logistics)
UPDATE Language_en_US
SET Text = 'Logistics'
WHERE Tag = 'TXT_KEY_POLICY_MILITARY_TRADITION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Logistics[ENDCOLOR][NEWLINE]Killing a military unit generates [ICON_RESEARCH] Science based on the [ICON_STRENGTH] Strength of the defeated unit. All units heal for 20 points after killing a military unit.'
WHERE Tag = 'TXT_KEY_POLICY_MILITARY_TRADITION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Logistics is the management of the flow of goods between the point of origin and the point of consumption in order to meet some requirements, of customers or corporations. The resources managed in logistics can include physical items, such as food, materials, animals, equipment and liquids, as well as abstract items, such as time, information, particles, and energy.'
WHERE Tag = 'TXT_KEY_POLICY_MILITARYTRADITION_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Military Caste (Now Conscription)
UPDATE Language_en_US
SET Text = 'Conscription'
WHERE Tag = 'TXT_KEY_POLICY_MILITARY_CASTE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Conscription[ENDCOLOR][NEWLINE]Units gain +10% [ICON_STRENGTH] Strength. A ranged unit spawns in your [ICON_CAPITAL] Capital, and in every city settled after taking this Policy.'
WHERE Tag = 'TXT_KEY_POLICY_MILITARY_CASTE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Conscription is the compulsory enlistment of people in some sort of national service, most often military service. Conscription dates back to antiquity and continues in some countries to the present day under various names. The modern system of near-universal national conscription for young men dates to the French Revolution in the 1790s, where it became the basis of a very large and powerful military. Most European nations later copied the system in peacetime, so that men at a certain age would serve 1–8 years on active duty and then transfer to the reserve force.'
WHERE Tag = 'TXT_KEY_POLICY_MILITARYCASTE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Professional Army (Now Tribute)
UPDATE Language_en_US
SET Text = 'Tribute'
WHERE Tag = 'TXT_KEY_POLICY_PROFESSIONAL_ARMY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Tribute[ENDCOLOR][NEWLINE]A free Great General appears outside your [ICON_CAPITAL] Capital. Gain a bonus to [ICON_FOOD] Food and [ICON_GOLD] Gold in a City from border expansion. Scales with Era.'
WHERE Tag = 'TXT_KEY_POLICY_PROFESSIONAL_ARMY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A tribute is wealth, often in kind, that one party gives to another as a sign of respect or, as was often the case in historical contexts, of submission or allegiance. Various ancient states exacted tribute from the rulers of land which the state conquered or otherwise threatened to conquer.'
WHERE Tag = 'TXT_KEY_POLICY_PROFESSIONALARMY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );