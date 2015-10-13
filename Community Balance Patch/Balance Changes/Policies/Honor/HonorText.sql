-- Honor Opener/Finisher
UPDATE Language_en_US
SET Text = 'AUTHORITY'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_HONOR_CAP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Authority'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_HONOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Authority[ENDCOLOR] will greatly benefit warlike and expansionist civilizations.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting Authority grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +25% combat bonus VS Barbarians, and receive announcements when Barbarian Camps spawn in revealed territory. [NEWLINE] [ICON_BULLET] Gain [ICON_CULTURE] Culture when you kill Barbarians and clear Barbarian Camps. [NEWLINE] [ICON_BULLET] +5% [ICON_PRODUCTION] Production towards land units.[NEWLINE] [ICON_BULLET] Unlocks building the [COLOR_CYAN]Terracotta Army[ENDCOLOR].[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Each Authority policy unlocked grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +4% [ICON_PRODUCTION] Production towards land units.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting all policies in Authority grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET]Each City with a garrison generates +1 [ICON_HAPPINESS_1] Happiness and +3 [ICON_CULTURE] Culture.[NEWLINE] [ICON_BULLET] Grants access to Medieval Era Policy Branches, ignoring Era requirement.[NEWLINE] [ICON_BULLET] Provides [COLOR_CYAN]1[ENDCOLOR] point (of [COLOR_CYAN]3[ENDCOLOR] required in total) towards unlocking access to Ideologies.[NEWLINE] [ICON_BULLET] Allows for the purchase of [ICON_GREAT_GENERAL] Great Generals with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_HONOR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The word authority (Derived from the Latin word auctoritas) can be used to mean power given by the state (in the form of government, judges, police officers, etc.) or by academic knowledge of an area (someone can be an authority on a subject).'
WHERE Tag = 'TXT_KEY_POLICY_HONOR_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Warrior Code (Now Imperialism)
UPDATE Language_en_US
SET Text = 'Imperium'
WHERE Tag = 'TXT_KEY_POLICY_WARRIOR_CODE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Imperium[ENDCOLOR][NEWLINE]A free Settler appears near the [ICON_CAPITAL] Capital. Receive 50 [ICON_GOLDEN_AGE] Golden Age Points and [ICON_CULTURE] Culture when you found or conquer Cities, scaling with era. Conquest bonus also scales based on City [ICON_CITIZEN] population.'
WHERE Tag = 'TXT_KEY_POLICY_WARRIOR_CODE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Imperium is a Latin word which, in a broad sense, translates roughly as ''power to command.'' In ancient Rome, different kinds of power or authority were distinguished by different terms. Imperium referred to the sovereignty of the state over the individual.'
WHERE Tag = 'TXT_KEY_POLICY_WARRIORCODE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Discipline (Now Tribute)
UPDATE Language_en_US
SET Text = 'Tribute'
WHERE Tag = 'TXT_KEY_POLICY_DISCIPLINE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Tribute[ENDCOLOR][NEWLINE]City borders expand 25% faster. Cities gain +10 [ICON_PRODUCTION] Production and [ICON_FOOD] Food when their borders expand. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_POLICY_DISCIPLINE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A tribute is wealth, often in kind, that one party gives to another as a sign of respect or, as was often the case in historical contexts, of submission or allegiance. Various ancient states exacted tribute from the rulers of land which the state conquered or otherwise threatened to conquer.'
WHERE Tag = 'TXT_KEY_POLICY_DISCIPLINE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Military Tradition (Now Logistics)
UPDATE Language_en_US
SET Text = 'Dominance'
WHERE Tag = 'TXT_KEY_POLICY_MILITARY_TRADITION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Dominance[ENDCOLOR][NEWLINE]Killing a military unit generates [ICON_RESEARCH] Science based on the [ICON_STRENGTH] Strength of the defeated unit. All Melee units heal for 20 points after killing a military unit.'
WHERE Tag = 'TXT_KEY_POLICY_MILITARY_TRADITION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Power - or dominance - is the ability to influence behavior, and may not be fully assessable until it is challenged with equal force. Unlike power, which can be latent, dominance is a manifest condition characterized by individual, situational and relationship patterns in which attempts to control another party or parties may or may not be accepted.'
WHERE Tag = 'TXT_KEY_POLICY_MILITARYTRADITION_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Military Caste (Now Conscription)
UPDATE Language_en_US
SET Text = 'Conscription'
WHERE Tag = 'TXT_KEY_POLICY_MILITARY_CASTE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Conscription[ENDCOLOR][NEWLINE]Units gain +10% [ICON_STRENGTH] Strength. A Military Unit spawns near all Cities that reach a multiple of 6 [ICON_CITIZEN] Citizens (or have already exceeded 6 [ICON_CITIZEN] Citizens).'
WHERE Tag = 'TXT_KEY_POLICY_MILITARY_CASTE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Conscription is the compulsory enlistment of people in some sort of national service, most often military service. Conscription dates back to antiquity and continues in some countries to the present day under various names. The modern system of near-universal national conscription for young men dates to the French Revolution in the 1790s, where it became the basis of a very large and powerful military. Most European nations later copied the system in peacetime, so that men at a certain age would serve 1–8 years on active duty and then transfer to the reserve force.'
WHERE Tag = 'TXT_KEY_POLICY_MILITARYCASTE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Professional Army (Now Martial Law)
UPDATE Language_en_US
SET Text = 'Martial Law'
WHERE Tag = 'TXT_KEY_POLICY_PROFESSIONAL_ARMY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Martial Law[ENDCOLOR][NEWLINE][ICON_GOLD] Gold maintenance for Units reduced by 15%, and Roads by 50%. [ICON_PUPPET] Puppeted cities receive a +25% [ICON_PRODUCTION] Production modifier, and [ICON_OCCUPIED] Occupied cities a +33% [ICON_PRODUCTION] Production modifier.'
WHERE Tag = 'TXT_KEY_POLICY_PROFESSIONAL_ARMY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Martial law is the imposition of the highest-ranking military officer as the military governor or as the head of the government, thus removing all power from the previous executive, legislative, and judicial branches of government. It is usually imposed temporarily when the government or civilian authorities fail to function effectively (e.g., maintain order and security, or provide essential services).'
WHERE Tag = 'TXT_KEY_POLICY_PROFESSIONALARMY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );