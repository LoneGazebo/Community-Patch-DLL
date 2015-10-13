-- Opener -- Now called Imperialism -- Grants +1 Movement and +1 Sight for Naval Units, and speeds production of land units by +5%, with an additional +4% for every policy unlocked in Imperialism. Receive 1 extra happiness for every owned luxury, and 1 Happiness for every garrisoned unit.
UPDATE Language_en_US
SET Text = 'IMPERIALISM'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_EXPLORATION_CAP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Imperialism'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_EXPLORATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Imperialism[ENDCOLOR] enhances your ability to spread your empire through military power, particularly naval (and later air) supremacy.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting Imperialism grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +1 [ICON_MOVES] Movement for Naval units and +1 Sight for Naval combat units. [NEWLINE] [ICON_BULLET] +5% [ICON_PRODUCTION] Production towards Naval and Air units.[NEWLINE] [ICON_BULLET] Unlocks building the [COLOR_CYAN]Brandeburg Gate[ENDCOLOR].[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Each Imperialism policy unlocked grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +4% [ICON_PRODUCTION] Production towards Naval and Air units.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting all policies in Imperialism grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] Air units start with the [COLOR_POSITIVE_TEXT]Range[ENDCOLOR] Promotion, and Naval units start with the [COLOR_POSITIVE_TEXT]Ironsides[ENDCOLOR] Promotion. [NEWLINE] [ICON_BULLET] All Ocean, Coast and Lake tiles gain +1 [ICON_RESEARCH] Science and +1 [ICON_PRODUCTION] Production.[NEWLINE] [ICON_BULLET] Provides [COLOR_CYAN]1[ENDCOLOR] point (of [COLOR_CYAN]3[ENDCOLOR] required in total) towards unlocking access to Ideologies. [NEWLINE] [ICON_BULLET] Allows for the purchase of [ICON_GREAT_ADMIRAL] Great Admirals with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_EXPLORATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Maritime Infrastructure -- Now called Mercenary Army
UPDATE Language_en_US
SET Text = 'Mercenary Army'
WHERE Tag = 'TXT_KEY_POLICY_MARITIME_INFRASTRUCTURE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Mercenary Army[ENDCOLOR][NEWLINE]Allows the purchasing of Landsknechts, Foreign Legions, and Mercenaries as their prerequisite technologies are researched. [ICON_GOLD] Gold required for Military Unit upgrades reduced by 25%.'
WHERE Tag = 'TXT_KEY_POLICY_MARITIME_INFRASTRUCTURE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Mercenaries - those who take part in a conflict for personal gain or compensation rather than duty or belief - have been a fact of war since ancient times; but the practice reached its height during the Renaissance, when the so-called "Free Companies" were formed to fight for hire. Often composed of veterans from national armies and led by a charismatic commander such as the Englishman Sir John Hawkwood, who formed the infamous White Company, these were usually hired during times of war by city-states or small kingdoms that lacked the funds or manpower to maintain standing armies. While some of the early mercenary forces specialized in specific forms of combat - for instance, cavalry or artillery companies - most were formed of German or Swiss landsknecht and saw extensive use during the religious wars of the Reformation. Coincidentally, to the south the condottieri served in the internecine wars between the Italian city-states, including the Papacy. As late as the 1700s Scots-Gaelic Gallowglass companies were available for hire to continental nobles. In modern times, mercenaries tend to be hired in small, elite teams for specific missions, or be part of private security companies.'
WHERE Tag = 'TXT_KEY_POLICY_MARITIME_INFRASTRUCTURE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Foreign Legion Text
UPDATE Language_en_US
SET Text = 'Special Infantry Unit of the Modern Era. May only be received as free units through the Volunteer Army tenet of the Freedom Ideology, or for [ICON_GOLD] Gold purchase through the Mercenary Army policy. This Unit has a combat bonus outside of friendly territory but is otherwise very similar to Great War Infantry.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_FOREIGN_LEGION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Foreign Legion is a special unit, only available as free units through the Volunteer Army tenet of the Freedom Ideology, or for purchase through the Mercenary Army policy. It receives a significant combat bonus when operating outside of home territory, making it an excellent unit to use to gain control of foreign lands.'
WHERE Tag = 'TXT_KEY_UNIT_FRENCH_FOREIGNLEGION_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Merchant Navy -- Now called Exploitation -- +1 Food and +1 Production from Snow, Desert and Mountain Tiles
UPDATE Language_en_US
SET Text = 'Exploitation'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_NAVY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Exploitation[ENDCOLOR][NEWLINE]Farms and Plantations gain +1 [ICON_PRODUCTION] Production and +1 [ICON_FOOD] Food. Military Units can be upgraded in territory owned by allied City-States.'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_NAVY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Exploitation is the use of someone or something in an unjust or cruel manner, or generally as a means to an end. Most often, the word exploitation is used to refer to economic exploitation; that is, the act of using another person as a means to profit, particularly using their labor without offering or providing them fair compensation.'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_NAVY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Naval Tradition -- Military Tradition -- +25% to Great Admiral and Great General Production -- Barracks, Armories and Military Academies provide +1 Science.
UPDATE Language_en_US
SET Text = 'Military Tradition'
WHERE Tag = 'TXT_KEY_POLICY_NAVAL_TRADITION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Military Tradition[ENDCOLOR][NEWLINE]Earn [ICON_GREAT_ADMIRAL] Great Admirals and [ICON_GREAT_GENERAL] Great Generals 33% more quickly. Barracks, Armories and Military Academies provide +2 [ICON_RESEARCH] Science and +1 [ICON_CULTURE] Culture.'
WHERE Tag = 'TXT_KEY_POLICY_NAVAL_TRADITION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Military tradition is the specific set of practices associated with the military or soldiers in general. The concept may describe the styles of military uniform, drill, or even the music of a military unit.'
WHERE Tag = 'TXT_KEY_POLICY_NAVAL_TRADITION_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Navigation School -- Now called Exploration -- Receive a free Great Admiral -- Reveals all Capitals and starts a Golden Age.
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Exploration[ENDCOLOR][NEWLINE] Receive a free [ICON_GREAT_ADMIRAL] Great Admiral. All undiscovered [ICON_CAPITAL] Capitals are revealed. Embarked units receive +2 [ICON_MOVES] Movement, and [ICON_GREAT_GENERAL] Great Generals recieve +1 [ICON_MOVES] Movement.'
WHERE Tag = 'TXT_KEY_POLICY_NAVIGATION_SCHOOL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Exploration'
WHERE Tag = 'TXT_KEY_POLICY_NAVIGATION_SCHOOL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Exploration is the act of searching for the purpose of discovery of information or resources. Exploration occurs in all non-sessile animal species, including humans. In human history, its most dramatic rise was during the Age of Discovery when European explorers sailed and charted much of the rest of the world, largely in a pursuit of material wealth. Since then, major explorations after the Age of Discovery have occurred for reasons mostly aimed at information discovery.'
WHERE Tag = 'TXT_KEY_POLICY_NAVIGATION_SCHOOL_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Treasure Fleets -- Now called Civilizing Mission -- Receive a free Military Base, and a lump sum of Gold, when you conquer a city. Reduced maintenance.
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Civilizing Mission[ENDCOLOR][NEWLINE]Receive a free Factory, and a large sum of [ICON_GOLD] Gold, when you conquer a city. No [ICON_GOLD] Gold maintenance for Garrisons.'
WHERE Tag = 'TXT_KEY_POLICY_TREASURE_FLEETS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Civilizing Mission'
WHERE Tag = 'TXT_KEY_POLICY_TREASURE_FLEETS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The mission civilisatrice (the French for "civilizing mission") is a rationale for intervention or colonization, proposing to contribute to the spread of civilization, mostly amounting to the Westernization of indigenous peoples. It was notably the underlying principle of French and Portuguese colonial rule in the late 19th and early 20th centuries. It was influential in the French colonies of Algeria, French West Africa, and Indochina, and in the Portuguese colonies of Angola, Guinea, Mozambique and Timor. The European colonial powers felt it was their duty to bring Western civilization to what they perceived as backward peoples. Rather than merely govern colonial peoples, the Europeans would attempt to Westernize them in accordance with a colonial ideology known as "assimilation".'
WHERE Tag = 'TXT_KEY_POLICY_TREASURE_FLEETS_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );
