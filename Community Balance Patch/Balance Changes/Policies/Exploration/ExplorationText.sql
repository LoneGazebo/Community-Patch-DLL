-- Opener -- Now called Imperialism -- Grants +1 Movement and +1 Sight for Naval Units, and speeds production of land units by +5%, with an additional +4% for every policy unlocked in Imperialism. Receive 1 extra happiness for every owned luxury, and 1 Happiness for every garrisoned unit.
UPDATE Language_en_US
SET Text = 'IMPERIALISM'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_EXPLORATION_CAP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Imperialism'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_EXPLORATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Imperialism[ENDCOLOR] enhances your ability to spread your empire through military power, particularly naval (and later air) supremacy.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting Imperialism grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +1 [ICON_MOVES] Movement for Naval units and +1 Sight for Naval combat units. [NEWLINE] [ICON_BULLET] +5% [ICON_PRODUCTION] Production towards Naval and Air units.[NEWLINE] [ICON_BULLET] Unlocks building [COLOR_CYAN]the Louvre[ENDCOLOR].[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Each Imperialism policy unlocked grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +4% [ICON_PRODUCTION] Production towards Naval and Air units.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting all policies in Imperialism grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] All Air units start with the [COLOR_POSITIVE_TEXT]Siege I[ENDCOLOR] or the [COLOR_POSITIVE_TEXT]Sortie[ENDCOLOR] Promotion. [NEWLINE] [ICON_BULLET] All Ocean, Coast and Lake tiles gain +1 [ICON_RESEARCH] Science and +1 [ICON_PRODUCTION] Production. [NEWLINE] [ICON_BULLET] Allows for the purchase of [ICON_GREAT_ADMIRAL] Great Admirals with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_EXPLORATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Maritime Infrastructure -- Now called Organization -- +2 Production and +1 Gold from Seaports, Workshops, and Factories
UPDATE Language_en_US
SET Text = 'Organization'
WHERE Tag = 'TXT_KEY_POLICY_MARITIME_INFRASTRUCTURE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Organization[ENDCOLOR][NEWLINE]+2 [ICON_PRODUCTION] Production and +1 [ICON_GOLD] Gold from Seaports, Workshops, and Factories.'
WHERE Tag = 'TXT_KEY_POLICY_MARITIME_INFRASTRUCTURE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'There are a variety of legal types of organisations, including corporations, governments, non-governmental organizations, international organizations, armed forces, charities, not-for-profit corporations, partnerships, cooperatives, universities, and various types of political organizations. A hybrid organization is a body that operates in both the public sector and the private sector simultaneously, fulfilling public duties and developing commercial market activities. A voluntary association is an organization consisting of volunteers. Such organizations may be able to operate without legal formalities, depending on jurisdiction, including informal clubs. Organizations may also operate in secret and/or illegally in the case of secret societies, criminal organizations and resistance movements.'
WHERE Tag = 'TXT_KEY_POLICY_MARITIME_INFRASTRUCTURE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Merchant Navy -- Now called Exploitation -- +1 Food and +1 Production from Snow, Desert and Mountain Tiles
UPDATE Language_en_US
SET Text = 'Exploitation'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_NAVY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Exploitation[ENDCOLOR][NEWLINE]+2 [ICON_FOOD] Food and +2 [ICON_PRODUCTION] Production from Snow, Mountain, and (non-Floodplain) Desert tiles.'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_NAVY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Exploitation is the use of someone or something in an unjust or cruel manner, or generally as a means to an end. Most often, the word exploitation is used to refer to economic exploitation; that is, the act of using another person as a means to profit, particularly using their labor without offering or providing them fair compensation.'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_NAVY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Naval Tradition -- Military Tradition -- +25% to Great Admiral and Great General Production -- Barracks, Armories and Military Academies provide +1 Science.
UPDATE Language_en_US
SET Text = 'Military Tradition'
WHERE Tag = 'TXT_KEY_POLICY_NAVAL_TRADITION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Military Tradition[ENDCOLOR][NEWLINE]Earn [ICON_GREAT_ADMIRAL] Great Admirals and [ICON_GREAT_GENERAL] Great Generals 33% more quickly. Barracks, Armories and Military Academies provide +1 [ICON_RESEARCH] Science and +1 [ICON_CULTURE] Culture.'
WHERE Tag = 'TXT_KEY_POLICY_NAVAL_TRADITION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Military tradition is the specific set of practices associated with the military or soldiers in general. The concept may describe the styles of military uniform, drill, or even the music of a military unit.'
WHERE Tag = 'TXT_KEY_POLICY_NAVAL_TRADITION_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Navigation School -- Now called Exploration -- Receive a free Archaeologist -- Reveals Hidden Sites.
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Exploration[ENDCOLOR][NEWLINE]Receive two free Archaeologists near your [ICON_CAPITAL] Capital. Allows you to see Hidden Antiquity Sites.'
WHERE Tag = 'TXT_KEY_POLICY_NAVIGATION_SCHOOL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Exploration'
WHERE Tag = 'TXT_KEY_POLICY_NAVIGATION_SCHOOL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Exploration is the act of searching for the purpose of discovery of information or resources. Exploration occurs in all non-sessile animal species, including humans. In human history, its most dramatic rise was during the Age of Discovery when European explorers sailed and charted much of the rest of the world, largely in a pursuit of material wealth. Since then, major explorations after the Age of Discovery have occurred for reasons mostly aimed at information discovery.'
WHERE Tag = 'TXT_KEY_POLICY_NAVIGATION_SCHOOL_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Treasure Fleets -- Now called Civilizing Mission -- Receive a free Factory, and a lump sum of Gold, when you conquer a city. Reduced maintenance.
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Civilizing Mission[ENDCOLOR][NEWLINE]Receive a free Factory, and a large sum of [ICON_GOLD] Gold, when you conquer a city. No [ICON_GOLD] Gold maintenance for Garrisons.'
WHERE Tag = 'TXT_KEY_POLICY_TREASURE_FLEETS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Civilizing Mission'
WHERE Tag = 'TXT_KEY_POLICY_TREASURE_FLEETS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The mission civilisatrice (the French for "civilizing mission") is a rationale for intervention or colonization, proposing to contribute to the spread of civilization, mostly amounting to the Westernization of indigenous peoples. It was notably the underlying principle of French and Portuguese colonial rule in the late 19th and early 20th centuries. It was influential in the French colonies of Algeria, French West Africa, and Indochina, and in the Portuguese colonies of Angola, Guinea, Mozambique and Timor. The European colonial powers felt it was their duty to bring Western civilization to what they perceived as backward peoples. Rather than merely govern colonial peoples, the Europeans would attempt to Westernize them in accordance with a colonial ideology known as "assimilation".'
WHERE Tag = 'TXT_KEY_POLICY_TREASURE_FLEETS_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );
