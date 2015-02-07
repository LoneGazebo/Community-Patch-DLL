-- Honor Opener/Finisher
UPDATE Language_en_US
SET Text = 'MIGHT'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_HONOR_CAP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Might'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_HONOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Might[ENDCOLOR] will greatly benefit expansionists.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting Might grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +33% combat bonus VS Barbarians. [NEWLINE] [ICON_BULLET] Notifications will be provided when new Barbarian Encampments spawn in revealed territory. [NEWLINE] [ICON_BULLET] Gain [ICON_CULTURE] Culture for the empire from each barbarian killed, each Camp cleared, and each City conquered. [NEWLINE] [ICON_BULLET] +5% [ICON_PRODUCTION] Production towards land units.[NEWLINE] [ICON_BULLET] Unlocks building the [COLOR_CYAN]Statue of Zeus[ENDCOLOR].[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Each Might policy unlocked grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +4% [ICON_PRODUCTION] Production towards land units.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting all policies in Might grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] Policy cost due to number of cities reduced by 25%. [NEWLINE] [ICON_BULLET] Grants [ICON_GOLDEN_AGE] Golden Age Points from city conquest. [NEWLINE] [ICON_BULLET] Grants access to Medieval Era Policy Branches, ignoring Era requirement.Provides [COLOR_CYAN]1[ENDCOLOR] point (of [COLOR_CYAN]3[ENDCOLOR] required in total) towards unlocking access to Ideologies.[NEWLINE] [ICON_BULLET] Allows for the purchase of [ICON_GREAT_GENERAL] Great Generals with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_HONOR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Warrior Code (Now Imperialism)
UPDATE Language_en_US
SET Text = 'Expansionism'
WHERE Tag = 'TXT_KEY_POLICY_WARRIOR_CODE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Expansionism[ENDCOLOR][NEWLINE]A Free Settler appears in the [ICON_CAPITAL] Capital. Each City with a garrison increases empire [ICON_HAPPINESS_1] Happiness and [ICON_CULTURE] Culture by 1.'
WHERE Tag = 'TXT_KEY_POLICY_WARRIOR_CODE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'In general, expansionism consists of expansionist policies of governments and states. While some have linked the term to promoting economic growth (in contrast to no growth / sustainable policies), more commonly expansionism refers to the doctrine of a state expanding its territorial base (or economic influence) usually, though not necessarily, by means of military aggression. Compare empire-building, colonialism, and Lebensraum. Irredentism, revanchism, reunification or pan-nationalism are sometimes used to justify and legitimize expansionism, but only when the explicit goal is to reconquer territories that have been lost, or to take over ancestral lands. A simple territorial dispute, such as a border dispute, is not usually referred to as expansionism.'
WHERE Tag = 'TXT_KEY_POLICY_WARRIORCODE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Discipline (Now Tribute)
UPDATE Language_en_US
SET Text = 'Tribute'
WHERE Tag = 'TXT_KEY_POLICY_DISCIPLINE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Tribute[ENDCOLOR][NEWLINE]City borders expand 25% faster. Gain a bonus to [ICON_FOOD] Food and [ICON_GOLD] Gold in a City from border expansion. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_POLICY_DISCIPLINE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A tribute is wealth, often in kind, that one party gives to another as a sign of respect or, as was often the case in historical contexts, of submission or allegiance. Various ancient states exacted tribute from the rulers of land which the state conquered or otherwise threatened to conquer.'
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

-- Professional Army (Now Martial Law)
UPDATE Language_en_US
SET Text = 'Martial Law'
WHERE Tag = 'TXT_KEY_POLICY_PROFESSIONAL_ARMY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Martial Law[ENDCOLOR][NEWLINE][ICON_GOLD] Gold maintenance for Units reduced by 15%. [ICON_PUPPET] Puppeted cities receive a +25% [ICON_PRODUCTION] Production modifier, and [ICON_OCCUPIED] Occupied cities a +33% [ICON_PRODUCTION] Production modifier.'
WHERE Tag = 'TXT_KEY_POLICY_PROFESSIONAL_ARMY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Martial law is the imposition of military power over designated regions on an emergency basis. Martial law is usually imposed on a temporary basis when the civilian government or civilian authorities fail to function effectively (e.g., maintain order and security, or provide essential services). In full-scale martial law, the highest-ranking military officer would take over, or be installed, as the military governor or as head of the government, thus removing all power from the previous executive, legislative, and judicial branches of government.'
WHERE Tag = 'TXT_KEY_POLICY_PROFESSIONALARMY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );