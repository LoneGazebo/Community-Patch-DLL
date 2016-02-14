-- Land Dispute Text
UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]Territorial disputes strain your relationship.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_LAND_DISPUTE';

-- Reversed Tourism for Open Borders
UPDATE Language_en_US
SET Text = '+{1_Num}% Bonus for Open Borders from:[NEWLINE]   '
WHERE Tag = 'TXT_KEY_CO_CITY_TOURISM_OPEN_BORDERS_BONUS';

-- Citadel Text
UPDATE Language_en_US
SET Text = 'A Citadel is a mighty fortification that can only be constructed by a Great General. You can construct a Citadel anywhere within your territory.[NEWLINE][NEWLINE]Upon constructing the Citadel, your Culture borders will also expand to surround the Citadel on all sides by one hex. If the new Culture border claims hexes already owned by another civilization, you will incur a diplomatic penalty as a result. Any unit stationed within a Citadel receives a 100% defensive strength bonus. Additionally, any enemy unit which ends its turn next to a Citadel takes 30 damage (damage does not stack with other Citadels).[NEWLINE][NEWLINE] The Acropolis in Athens, Greece, is an example of an early powerful Citadel. Such structures were almost impossible to take by direct attack, and they were capable of withstanding protracted sieges before they fell.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_CITADEL_TEXT';

-- CS Stuff
UPDATE Language_en_US
SET Text = 'You received tribute very recently'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_BULLIED_VERY_RECENTLY';

UPDATE Language_en_US
SET Text = 'You received tribute recently'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_BULLIED_RECENTLY';

-- Razing
UPDATE Language_en_US	
SET Text = '[COLOR_POSITIVE_TEXT]Razing[ENDCOLOR] The City will burn [ICON_RAZING] down each turn until it reaches 0 population, and is removed from the game. This produces a lot of [ICON_HAPPINESS_4] Unhappiness, but also greatly increases your [COLOR_POSITIVE_TEXT]War Score[ENDCOLOR] versus this player.'
WHERE Tag = 'TXT_KEY_POPUP_CITY_CAPTURE_INFO_RAZE';

-- LUA
UPDATE Language_en_US	
SET Text = 'Base Yield:'
WHERE Tag = 'TXT_KEY_PEDIA_YIELD_LABEL';

UPDATE Language_en_US	
SET Text = 'Connected By:'
WHERE Tag = 'TXT_KEY_PEDIA_IMPROVEMENTS_LABEL';

-- Fixed diacritics for spy names.

UPDATE Language_en_US
SET Text = 'Antônio'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_0';

UPDATE Language_en_US
SET Text = 'Estêvão'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_3';

UPDATE Language_en_US
SET Text = 'Fernão'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_4';

UPDATE Language_en_US
SET Text = 'Tomé'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_8';

UPDATE Language_en_US
SET Text = 'Provides additional spies based on a fraction of the number of City-States, and levels up all your existing spies. Also provides a 15% reduction in enemy spy effectiveness. Must have a Police Station in all cities.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_HELP';

UPDATE Language_en_US
SET Text = 'An important defensive National Wonder for a technology-driven civilization. The National Intelligence Agency provides additional spies, levels up all your existing spies, and provides a 15% reduction in enemy spy effectiveness. A civilization must have a Police Station in all cities before it can construct the National Intelligence Agency.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_STRATEGY';

UPDATE Language_en_US
SET Text = 'Conducting Espionage'
WHERE Tag = 'TXT_KEY_SPY_STATE_ESTABLISHED_SURVEILLANCE';

UPDATE Language_en_US
SET Text = 'Disrupting Plans'
WHERE Tag = 'TXT_KEY_SPY_STATE_ESTABLISHED_SURVEILLANCE_PROGRESS_BAR';

UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]Your spies were caught operating in their lands.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_CAUGHT_STEALING';

-- Changed Diplo Text for Trade

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]We are trade partners.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_TRADE_PARTNER';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]You have shared intrigue with them.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_SHARED_INTRIGUE';

-- Changed Diplo Text for Assistance

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Your recent diplomatic actions please them.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_ASSISTANCE_TO_THEM';

UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]Your recent diplomatic actions disappoint them.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_REFUSED_REQUESTS';

-- Unit Settle Help

UPDATE Language_en_US
SET Text = 'Cannot found a City here. If on a Standard or smaller size map, cities must have 3 Tiles between them. If Large or greater, cities must have 4 tiles between them.'
WHERE Tag = 'TXT_KEY_MISSION_BUILD_CITY_DISABLED_HELP';

-- Diplo Text for DoF changed
UPDATE Language_en_US
SET Text = 'Our Declaration of Friendship must end.'
WHERE Tag = 'TXT_KEY_DIPLO_DISCUSS_MESSAGE_END_WORK_WITH_US';

UPDATE Language_en_US
SET Text = 'I did not want things to end up like this, but I cannot overlook your warmongering. Consider our Declaration of Friendship ended.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_WARMONGER_1';

UPDATE Language_en_US
SET Text = 'Your dealings with city-states in my sphere of influence is unacceptable. Consider our Declaration of Friendship ended.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_MINORS_1';

UPDATE Language_en_US
SET Text = 'I fear that our close proximity was destined to poison our relationship. Consider our Declaration of Friendship ended.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_LAND_1';

UPDATE Language_en_US
SET Text = 'Your continued greed in collecting wonders of the world has become a major issue. Consider our Declaration of Friendship ended.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_WONDERS_1';

UPDATE Language_en_US
SET Text = 'It is clear to me now that the paths we are traveling must ultimately drive us apart. Consider our Declaration of Friendship ended.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_VICTORY_1';

UPDATE Language_en_US
SET Text = 'Circumstances have changed, and I believe there is no longer a need for our Declaration of Friendship to continue. I hope you can understand.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_DEFAULT_1';

UPDATE Language_en_US
SET Text = 'I am sorry, but it is time for our Declaration of Friendship to end.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_DEFAULT_2';

UPDATE Language_en_US
SET Text = 'We cannot offer you enough to make this a fair trade. We can, however, offer you this deal.'
WHERE Tag = 'TXT_KEY_GENERIC_TRADE_CANT_MATCH_OFFER_1';

UPDATE Language_en_US
SET Text = 'The deal we suggest may be an unfair exchange for you. Please inspect it closely.'
WHERE Tag = 'TXT_KEY_GENERIC_TRADE_CANT_MATCH_OFFER_2';

UPDATE Language_en_US
SET Text = 'Impossible. You go too far.'
WHERE Tag = 'TXT_KEY_DIPLO_DISCUSS_HOW_DARE_YOU';

UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]You are competing for World Wonders.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_WONDER_DISPUTE';

UPDATE Language_en_US
SET Text = 'Deployable Troops.'
WHERE Tag = 'TXT_KEY_DEMOGRAPHICS_ARMY_MEASURE';

UPDATE Language_en_US
SET Text = 'Germany always looks forward to favorable relations with mighty empires such as yours.'
WHERE Tag = 'TXT_KEY_LEADER_BISMARCK_GREETING_NEUTRAL_HELLO_1';

UPDATE Language_en_US
SET Text = '{1_Num} [ICON_CULTURE] from Great Works and Themes'
WHERE Tag = 'TXT_KEY_CULTURE_FROM_GREAT_WORKS';

UPDATE Language_en_US
SET Text = 'You have discovered {1_Num} technologies that {2_CivName} does not know.[NEWLINE]They are receiving +{3_Num} [ICON_RESEARCH] Science on this route due to their Cultural Influence over you.'
WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_SCIENCE_EXPLAINED';

UPDATE Language_en_US
SET Text = '{1_CivName} has discovered {2_Num} technologies that you do not know.[NEWLINE]You are receiving +{3_Num} [ICON_RESEARCH] Science on this route due to your Cultural Influence over them.'
WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_SCIENCE_EXPLAINED';

-- Trade Things
UPDATE Language_en_US
SET Text = 'The other leader has not met any third party players, or you do not have an Embassy with this player.'
WHERE Tag = 'TXT_KEY_DIPLO_OTHER_PLAYERS_NO_PLAYERS_THEM';

-- Civilopedia Refresh
UPDATE Language_en_US
SET Text = 'Only one combat unit may occupy a city at a time. That military unit is said to "Garrison" the city, and it adds a significant defensive bonus to the city. If a city is attacked while a Garrison is in the city, the Garrison will deflect some of the damage onto itself, thus offering the city even more protection. Be careful, however, as a Garrison can be destroyed this way.[NEWLINE][NEWLINE]Additional combat units may move through the city, but they cannot end their turn there. (So if you build a combat unit in a city with a garrison, you have to move one of the two units out before you end your turn.)'
WHERE Tag = 'TXT_KEY_CITIES_COMBATUNITS_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'Only one combat unit may occupy a city at a time. That military unit is said to "Garrison" the city, and it adds a significant defensive bonus to the city. If a city is attacked while a Garrison is in the city, the Garrison will deflect some of the damage onto itself, thus offering the city even more protection. Be careful, however, as a Garrison can be destroyed this way.[NEWLINE][NEWLINE]Additional combat units may move through the city, but they cannot end their turn there. (So if you build a combat unit in a city with a garrison, you have to move one of the two units out before you end your turn.)'
WHERE Tag = 'TXT_KEY_CITIES_COMBATUNITS_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'A city''s owner may "garrison" a military unit inside the city to bolster its defenses. A portion of the garrisoned unit''s combat strength is added to the city''s strength. The garrisoned will divert part of the all damage to a city when the city is attacked. This can destroy the garrison, so be careful! If the city is captured, the garrisoned unit is destroyed.[NEWLINE][NEWLINE]A unit stationed in the city may attack surrounding enemy units, but if it does so the city loses its garrison bonus, and, if it is a melee attack, the unit may take damage during the combat as normal.'
WHERE Tag = 'TXT_KEY_COMBAT_GARRISONINCITIES_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'At the end of melee combat, one or both units may have sustained damage and lost "hit points." If a unit''s hit points are reduced to 0, that unit is destroyed. If after melee combat the defending unit has been destroyed and the attacker survives, the attacking unit moves into the defender''s hex [COLOR_POSITIVE_TEXT]unless[ENDCOLOR] defending a Citadel, Fort, or City, at which point the melee unit remains in place. If it moves, the winner will capture any non-military units in that hex. If the defending unit survives, it retains possession of its hex and any other units in the hex.[NEWLINE][NEWLINE]Most units use up all of their movement when attacking. Some however have the ability to move after combat - if they survive the battle and have movement points left to expend.[NEWLINE][NEWLINE]Any surviving units involved in the combat will receive "experience points" (XPs), which may be expended to give the unit promotions.'
WHERE Tag = 'TXT_KEY_COMBAT_MELEERESULTS_HEADING3_BODY';
UPDATE Language_en_US
SET Text = 'Great Generals are "Great People" skilled in the art of warfare. They provide combat bonuses - offensive and defensive bonuses both - to any friendly units within two tiles of their location. A Great General itself is a non-combat unit, so it may be stacked with a combat unit for protection. If an enemy unit ever enters the tile containing a Great General, the General is destroyed.[NEWLINE][NEWLINE]A Great General gives a combat bonus of 20% to units in the General''s tile and all friendly units within 2 tiles of the General.[NEWLINE][NEWLINE]Great Generals are created when your units have been in battle and also can be acquired from buildings, policies, beliefs, and tenets. See the section on "Great People" for more details.'
WHERE Tag = 'TXT_KEY_COMBAT_GREATGENERALS_HEADING2_BODY';

UPDATE Language_en_US
SET Text = 'Great Generals are "Great People" skilled in the art of warfare. They provide combat bonuses - offensive and defensive bonuses both - to any friendly units within two tiles of their location. A Great General itself is a non-combat unit, so it may be stacked with a combat unit for protection. If an enemy unit ever enters the tile containing a Great General, the General is destroyed.[NEWLINE][NEWLINE]A Great General gives a combat bonus of 20% to units in the General''s tile and all friendly units within 2 tiles of the General.[NEWLINE][NEWLINE]Great Generals are created when your units have been in battle and also can be acquired from buildings, policies, beliefs, and tenets. See the section on "Great People" for more details.'
WHERE Tag = 'TXT_KEY_COMBAT_GREATGENERALS_HEADING2_BODY';

UPDATE Language_en_US
SET Text = 'War Score'
WHERE Tag = 'TXT_KEY_DIPLOMACY_PEACE_HEADING3_TITLE';

UPDATE Language_en_US
SET Text = 'War Score is ever-shifting status of a war between players. War Score can fluctuate from 100 to -100, where 100 is a total victory for you, and -100 a total victory for your opponent. War Score will also gradually decay over time, to highlight the declining value of past actions in a long, drawn-out conflict.[NEWLINE][NEWLINE]When declared, War Score starts at zero for both players. As you (or your opponent) destroy units, pillage tiles/trade units, capture civilians and take cities, your warscore will go up. The value of these actions varies based on the overall size of your opponent.[NEWLINE][NEWLINE]When it comes time to make peace, the War Score value gives you a good idea of what you should expect to gain from your opponent, or what they will ask of you. In the trade screen, the War Score value will be translated into a ''Max Peace'' value, which shows you exactly what you can take from your opponent (or vice-versa). When peace is concluded, the War Score returns to zero.'
WHERE Tag = 'TXT_KEY_DIPLOMACY_PEACE_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'The ruin provides a map of the nearest unrevealed City (lifting the fog of war from a number of tiles).'
WHERE Tag = 'TXT_KEY_BARBARIAN_MAP_HEADING4_BODY';

UPDATE Language_en_US
SET Text = 'When they fight barbarian units, your less well-trained units will gain experience points. However, any unit that has already acquired 45 XPs (or has exchanged that many for promotions) no longer gains XPs from fighting barbarians.'
WHERE Tag = 'TXT_KEY_BARBARIAN_POINTLIMITS_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'Each civilization in Civilization V has one or more special "national units." These units are unique to that civilization, and they are in some way superior to the standard version of that unit. The American civilization, for example, has a Minuteman unit, which is superior to the standard Musketman available to other civs. The Greek civ has the Hoplite unit, which replaces the Spearman.[NEWLINE][NEWLINE]See each civilization''s Civilopedia entry to discover its special unit.'
WHERE Tag = 'TXT_KEY_BARBARIAN_POINTLIMITS_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'If another civ has captured a City-State and you capture it from them, you have the option to "liberate" that city-state. If you do so, you will receive a large amount of [ICON_INFLUENCE] Influence from the City-State, usually enough to make you [COLOR_POSITIVE_TEXT]Allies[ENDCOLOR] with it.'
WHERE Tag = 'TXT_KEY_CITYSTATE_LIBERATING_HEADING2_BODY';

UPDATE Concepts
SET InsertAfter = 'CONCEPT_SOCIAL_POLICY_GAINING'
WHERE Type = 'CONCEPT_SOCIAL_POLICY_BRANCH_IDEOLOGY';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_TERRAIN_COAST';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_TERRAIN_COAST';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_TERRAIN_COAST';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_TERRAIN_DESERT';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_TERRAIN_DESERT';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_TERRAIN_DESERT';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_TERRAIN_GRASSLAND';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_TERRAIN_GRASSLAND';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_TERRAIN_GRASSLAND';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_TERRAIN_HILLS';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_TERRAIN_HILLS';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_TERRAIN_HILLS';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_TERRAIN_MOUNTAINS';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_TERRAIN_MOUNTAINS';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_TERRAIN_MOUNTAINS';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_TERRAIN_OCEANS';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_TERRAIN_OCEANS';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_TERRAIN_OCEANS';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_TERRAIN_PLAINS';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_TERRAIN_PLAINS';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_TERRAIN_PLAINS';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_TERRAIN_SNOW';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_TERRAIN_SNOW';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_TERRAIN_SNOW';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_TERRAIN_TUNDRA';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_TERRAIN_TUNDRA';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_TERRAIN_TUNDRA';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_TERRAIN_FEATURE_FALLOUT';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_TERRAIN_FEATURE_FALLOUT';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_TERRAIN_FEATURE_FALLOUT';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_TERRAIN_FEATURE_FLOOD_PLAINS';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_TERRAIN_FEATURE_FLOOD_PLAINS';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_TERRAIN_FEATURE_FLOOD_PLAINS';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_TERRAIN_FEATURE_FORESTS';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_TERRAIN_FEATURE_FORESTS';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_TERRAIN_FEATURE_FORESTS';


DELETE FROM Concepts
WHERE Type = 'CONCEPT_TERRAIN_FEATURE_ICE';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_TERRAIN_FEATURE_ICE';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_TERRAIN_FEATURE_ICE';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_TERRAIN_FEATURE_JUNGLE';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_TERRAIN_FEATURE_JUNGLE';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_TERRAIN_FEATURE_JUNGLE';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_TERRAIN_FEATURE_MARSH';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_TERRAIN_FEATURE_MARSH';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_TERRAIN_FEATURE_MARSH';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_TERRAIN_FEATURE_OASIS';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_TERRAIN_FEATURE_OASIS';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_TERRAIN_FEATURE_OASIS';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_TERRAIN_FEATURE_NATURAL_WONDER';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_TERRAIN_FEATURE_NATURAL_WONDER';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_TERRAIN_FEATURE_NATURAL_WONDER';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_TERRAIN_RIVERS_YIELD';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_TERRAIN_RIVERS_YIELD';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_TERRAIN_RIVERS_YIELD';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_TERRAIN_RIVERS_COMBAT';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_TERRAIN_RIVERS_COMBAT';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_TERRAIN_RIVERS_COMBAT';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_WORKERS_IMPROVEMENTS_FARM';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_WORKERS_IMPROVEMENTS_FARM';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_WORKERS_IMPROVEMENTS_FARM';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_WORKERS_IMPROVEMENTS_MINE';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_WORKERS_IMPROVEMENTS_MINE';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_WORKERS_IMPROVEMENTS_MINE';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_WORKERS_CLEARINGLAND_TIME';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_WORKERS_CLEARINGLAND_TIME';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_WORKERS_CLEARINGLAND_TIME';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_SOCIAL_POLICY_BRANCHES';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_SOCIAL_POLICY_BRANCHES';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_SOCIAL_POLICY_BRANCHES';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_SOCIAL_POLICY_BRANCH_TRADITION';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_SOCIAL_POLICY_BRANCH_TRADITION';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_SOCIAL_POLICY_BRANCH_TRADITION';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_SOCIAL_POLICY_BRANCH_LIBERTY';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_SOCIAL_POLICY_BRANCH_LIBERTY';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_SOCIAL_POLICY_BRANCH_LIBERTY';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_SOCIAL_POLICY_BRANCH_HONOR';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_SOCIAL_POLICY_BRANCH_HONOR';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_SOCIAL_POLICY_BRANCH_HONOR';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_SOCIAL_POLICY_BRANCH_PIETY';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_SOCIAL_POLICY_BRANCH_PIETY';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_SOCIAL_POLICY_BRANCH_PIETY';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_SOCIAL_POLICY_BRANCH_PATRONAGE';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_SOCIAL_POLICY_BRANCH_PATRONAGE';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_SOCIAL_POLICY_BRANCH_PATRONAGE';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_SOCIAL_POLICY_BRANCH_COMMERCE';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_SOCIAL_POLICY_BRANCH_COMMERCE';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_SOCIAL_POLICY_BRANCH_COMMERCE';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_SOCIAL_POLICY_BRANCH_RATIONALISM';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_SOCIAL_POLICY_BRANCH_RATIONALISM';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_SOCIAL_POLICY_BRANCH_RATIONALISM';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_SOCIAL_POLICY_BRANCH_RATIONALISM';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_SOCIAL_POLICY_BRANCH_RATIONALISM';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_SOCIAL_POLICY_BRANCH_RATIONALISM';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_SOCIAL_POLICY_BRANCH_AESTHETICS';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_SOCIAL_POLICY_BRANCH_AESTHETICS';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_SOCIAL_POLICY_BRANCH_AESTHETICS';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_SOCIAL_POLICY_BRANCH_EXPLORATION';

DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_SOCIAL_POLICY_BRANCH_EXPLORATION';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_SOCIAL_POLICY_BRANCH_EXPLORATION';




