-- No Barbarians Game Option Change
UPDATE Language_en_US
SET Text = 'Barbarians and their Encampments do not appear on the map. Rebel (barbarian) uprisings from [ICON_HAPPINESS_4] Unhappiness do not occur.'
WHERE Tag = 'TXT_KEY_GAME_OPTION_NO_BARBARIANS_HELP';

-- Tooltip fix for Open Borders
UPDATE Language_en_US
SET Text = 'Allows the other player''s military or civilian Units to pass through one''s territory (lasts {1_Num} turns).[NEWLINE][NEWLINE]Allowing another civilization to pass through one''s own territory increases the [ICON_TOURISM] Tourism modifier towards that civilization.[NEWLINE][NEWLINE]Note: Military Units belonging to different players may never stack.'
WHERE Tag = 'TXT_KEY_DIPLO_OPEN_BORDERS_TT';

UPDATE Language_en_US
SET Text = '[ICON_TOURISM] Tourism increases by 50% for 20 Turns.  Free Social Policy.'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_GAMES_3_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = '[ICON_CULTURE] Culture increases by 33% for 20 Turns.'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_3_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- CS Text Change
UPDATE Language_en_US
SET Text = 'Giving Heavy Tribute'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_UNIT_RELUCTANCE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Aesthetics Change
UPDATE Language_en_US
SET Text = 'When improved with an Archaeological Dig, a Hidden Antiquity Site can then yield an Artifact that can be placed in a Great Work of Art slot, or be transformed into a Landmark improvement. Hidden Antiquity Sites are only visible to civs that have all the policies in the Artistry policy tree.'
WHERE Tag = 'TXT_KEY_RESOURCE_HIDDEN_ARTIFACTS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Help Text Changes
UPDATE Language_en_US
SET Text = '[ICON_TOURISM] Tourism is the primary yield you will use to spread your cultural influence to other civilizations. It is generated passively by [COLOR_POSITIVE_TEXT]Great Works[ENDCOLOR] or [COLOR_POSITIVE_TEXT]Artifacts[ENDCOLOR], and actively by [COLOR_POSITIVE_TEXT]Historic Events[ENDCOLOR] (for more on ''Historic Events,'' see the section with this label) or completing [COLOR_POSITIVE_TEXT]Trade Routes[ENDCOLOR] (if you have constructed buildings that grant such bonuses). As you place more and more Great Works and Artifacts into your Museums, Amphitheaters, Opera Houses, etc., your [ICON_TOURISM] Tourism will continue to increase. Additionally, you can boost your [ICON_TOURISM] Tourism output quite a bit through Open Borders, Trade Routes, shared Religion, and certain Ideological Tenets.'
WHERE Tag = 'TXT_KEY_CULTURE_TOURISM_HEADING2_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Capture Popup

UPDATE Language_en_US	
SET Text = '[COLOR_POSITIVE_TEXT]Puppeting[ENDCOLOR] causes the following:[NEWLINE][ICON_BULLET] Generates 80% less Gold, Science, Culture, Tourism, and Faith than a normal city.[NEWLINE][ICON_BULLET] No longer produces Great Person Points.[NEWLINE][ICON_BULLET] You [COLOR_POSITIVE_TEXT]cannot choose what it produces[ENDCOLOR] or spend Gold in the City.[NEWLINE][ICON_BULLET] Generates 1 [ICON_HAPPINESS_3] Unhappiness for every 4 [ICON_CITIZEN] Citizens in the City.[NEWLINE][ICON_BULLET] Does not increase the cost of Social Policies, Technologies, or Tourism.[NEWLINE][NEWLINE]If you choose this, you may later Annex the City at any point.'
WHERE Tag = 'TXT_KEY_POPUP_CITY_CAPTURE_INFO_PUPPET' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US	
SET Text = '[COLOR_POSITIVE_TEXT]Annexing[ENDCOLOR] causes the following:[NEWLINE][ICON_BULLET] Produces [ICON_HAPPINESS_4] Unhappiness equal to the City''s [ICON_CITIZEN] Population (until you construct a Courthouse).[NEWLINE][ICON_BULLET] Invests in a Courthouse in the City, reducing its construction time by at least 50% (if annexed immediately after conquest).[NEWLINE][ICON_BULLET] Halves City [ICON_RESISTANCE] Resistance after capture (if annexed immediately after conquest).'
WHERE Tag = 'TXT_KEY_POPUP_CITY_CAPTURE_INFO_ANNEX' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US	
SET Text = '[COLOR_POSITIVE_TEXT]Razing[ENDCOLOR] causes the following:[NEWLINE][ICON_BULLET] City loses [ICON_CITIZEN] Population each turn until it reaches 0 population, and is removed from the game.[NEWLINE][ICON_BULLET] Produces [ICON_HAPPINESS_4] Unhappiness equal to the City''s [ICON_CITIZEN] Population.[NEWLINE][ICON_BULLET] Greatly increases your [COLOR_POSITIVE_TEXT]War Score[ENDCOLOR] every time Population is reduced.[NEWLINE][ICON_BULLET] May generate [COLOR_NEGATIVE_TEXT]Partisans[ENDCOLOR] loyal to the former owner (if you are still at war with this player) or barbarians each turn while razing.'
WHERE Tag = 'TXT_KEY_POPUP_CITY_CAPTURE_INFO_RAZE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Citizen Food Consumption
UPDATE Language_en_US	
SET Text = '[COLOR_NEGATIVE_TEXT]-{1_Num}[ENDCOLOR] {2_IconString} eaten by [ICON_CITIZEN] Citizens'
WHERE Tag = 'TXT_KEY_YIELD_EATEN_BY_POP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Citizen Food Consumption

UPDATE Language_en_US	
SET Text = 'Laborers'
WHERE Tag = 'TXT_KEY_CITIES_UNEMPLOYEDCITIZENS_HEADING3_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US	
SET Text = 'Unemployed Citizens each provide a small amount yields compared to other Specialists, however you can assign as many of your citizens to this position as desired. They should, when possible, be assigned to work tiles or made into specialists.'
WHERE Tag = 'TXT_KEY_SPECIALIST_CITIZEN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US	
SET Text = 'Laborer|Laborers'
WHERE Tag = 'TXT_KEY_SPECIALIST_CITIZEN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US	
SET Text = 'Laborers'
WHERE Tag = 'TXT_KEY_CITYVIEW_UNEMPLOYED_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- CSs
UPDATE Language_en_US	
SET Text = 'City-States can be bullied into giving up large amounts of Yields, but using force in this way will decrease your [ICON_INFLUENCE] Influence over them.'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Trade Stuff
UPDATE Language_en_US	
SET Text = '{1_CivName} has discovered {2_Num} technologies that you do not know, granting you +{4_Num} [ICON_RESEARCH] Science.[NEWLINE]+{3_Num} [ICON_RESEARCH] Science on this route due to your Cultural Influence over them.'
WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_SCIENCE_EXPLAINED' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US	
SET Text = 'You have discovered {1_Num} technologies that {2_CivName} does not know, granting them +{4_Num} [ICON_RESEARCH] Science.[NEWLINE]+{3_Num} [ICON_RESEARCH] Science on this route due to their Cultural Influence over you.'
WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_SCIENCE_EXPLAINED' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- CS Gift Stuff

UPDATE Language_en_US	
SET Text = '({1_NumHappiness} [ICON_HAPPINESS_1] Happiness, {2_Gold} [ICON_GOLD] Gold)'
WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_LOST_MERCANTILE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US	
SET Text = 'As a [COLOR_POSITIVE_TEXT]Mercantile[ENDCOLOR] City-State, their markets offer exotic goods to your people! (+{1_NumHappiness} [ICON_HAPPINESS_1] Happiness, +{2_Gold} [ICON_GOLD] Gold)'
WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_MERCANTILE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US	
SET Text = 'As a [COLOR_POSITIVE_TEXT]Mercantile[ENDCOLOR] City-State, their markets spread trade of exotic goods across your empire! (+{1_NumHappiness} [ICON_HAPPINESS_1] Happiness, +{2_Gold} [ICON_GOLD] Gold)'
WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_MERCANTILE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US	
SET Text = 'They will contribute fewer Units to your military, and less Science! ({1_Science} [ICON_RESEARCH] Science)'
WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_LOST_ALLIES_MILITARISTIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US	
SET Text = 'They will no longer give you military Units or Science! ({1_Science} [ICON_RESEARCH] Science)'
WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_LOST_FRIENDS_MILITARISTIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US	
SET Text = 'As a [COLOR_POSITIVE_TEXT]Militaristic[ENDCOLOR] City-State, they will give you military Units and Science. (+{1_Science} [ICON_RESEARCH] Science)'
WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_MILITARISTIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );		

UPDATE Language_en_US	
SET Text = 'As a [COLOR_POSITIVE_TEXT]Militaristic[ENDCOLOR] City-State, they will regularly give you military Units and Science. (+{1_Science} [ICON_RESEARCH] Science)'
WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_MILITARISTIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );		

-- Change instructions for Ideologies

UPDATE Language_en_US
SET Text = 'The three Ideology trees, Freedom, Order, and Autocracy, have greatly expanded in Brave New World. All civilizations in the game, on acquiring 18 Policies and having advanced at least to the Industrial Era, or upon reaching the Atomic Era, will be required to choose an Ideology for their civilization. Each Ideology tree contains 3 tiers of "tenets" that you use to customize your Ideology, with the third and final tier holding the most powerful benefits. As with regular Social Policies, players use Culture to purchase additional tenets as they move through the game.[NEWLINE][NEWLINE]Civilizations that share a common Ideology will receive benefits with their diplomatic relationships. Civilizations that have conflicting Ideologies have multiple side-effects. For example, a negative effect on their diplomatic relationship and happiness penalties take effect if an opposing Ideology has a stronger Cultural influence on your people. If you let your people become too unhappy, there is a chance that your cities may declare that they are joining your opponents empire. As a last resort, you, or other players, can resort to a "Revolution" to switch Ideologies to one that is preferable to your people.[NEWLINE][NEWLINE]For more information on Ideological Tenets, click on the "Social Policies" tab along the top of the Civilopedia, and scroll down to the Order, Freedom, and Autocracy sections.'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_IDEOLOGY_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Players interested in creating massive, sprawling civs should turn to the Order ideology, as the strength of the empire is determined by the total number of cities it contains. This ideology unlocks upon entering the Atomic Era, or unlocking 18 Policies and have advanced at least to the Industrial Era, whichever comes first.'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_ORDER_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Autocracy ideology is well suited for those wishing nothing more than to crush their foes under the weight of their iron-plated boots. This ideology unlocks upon entering the Atomic Era, or unlocking 18 Policies and having advanced at least to the Industrial Era, whichever comes first.'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_AUTOCRACY_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Freedom ideology provides bonuses for Culture, Tourism, Specialist production, and more. This ideology unlocks upon entering the Atomic Era, or unlocking 18 Policies and having advanced at least to the Industrial Era, whichever comes first.'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_FREEDOM_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Your people now consider themselves part of the Atomic Era, and the ideas of modernization permeate your society. Your people clamor for you to pick an Ideology for your civilization.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CHOOSE_IDEOLOGY_FACTORIES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Ideological tenets may not be purchased until you have chosen an Ideology. This choice becomes available if you are in the Atomic Era, or have unlocked 18 Policies and have advanced at least to the Industrial Era, whichever comes first.'
WHERE Tag = 'TXT_KEY_POLICYSCREEN_IDEOLOGY_NOT_STARTED' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'You have moved beyond the Modern Era. The ideas of modernity now permeate your society. Your people clamor for you to pick an Ideology for your civilization.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CHOOSE_IDEOLOGY_ERA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = 'Allows your Cities to build [COLOR_POSITIVE_TEXT]Atomic Bombs[ENDCOLOR] and [COLOR_POSITIVE_TEXT]Nuclear Missiles[ENDCOLOR].[NEWLINE][NEWLINE]If you are the [COLOR_POSITIVE_TEXT]First[ENDCOLOR] to complete this, receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Atomic Bomb and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Research Lab in your [ICON_CAPITAL] Capital.'
WHERE Tag = 'TXT_KEY_PROJECT_MANHATTAN_PROJECT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Manhattan Project allows a civilization to construct nuclear weapons. Each civilization must construct the Manhattan Project before it can construct nukes. If you are part of the first global team to complete this, you will receive a free Research Lab and Atomic Bomb in your Capital.'
WHERE Tag = 'TXT_KEY_PROJECT_MANHATTAN_PROJECT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Signals the start of the space race, allowing your Cities to build Spaceship parts. When the Spaceship is completed, you win a [COLOR_POSITIVE_TEXT]Science Victory[ENDCOLOR]![NEWLINE][NEWLINE]If you are the [COLOR_POSITIVE_TEXT]First[ENDCOLOR] to complete this, receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_SCIENTIST] Great Scientist near your [ICON_CAPITAL] Capital, and your Empire enters a [ICON_GOLDEN_AGE] Golden Age.'
WHERE Tag = 'TXT_KEY_PROJECT_APOLLO_PROGRAM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Apollo Program is the start of the space race. It allows construction of spaceship parts. Each civilization must complete this project before they can construct spaceship parts. If you are part of the first global team to complete this, you will receive a free Great Scientist near your Capital, and a Golden Age will begin.'
WHERE Tag = 'TXT_KEY_PROJECT_APOLLO_PROGRAM_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Fishing'
WHERE Tag = 'TXT_KEY_TECH_SAILING_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Fishing is the activity of trying to catch fish. Fishing usually takes place in the wild. Techniques for catching fish include hand gathering, spearing, netting, angling and trapping. The term fishing may be applied to catching other aquatic animals such as molluscs, cephalopods, crustaceans, and echinoderms. The term is not normally applied to catching farmed fish, or to aquatic mammals, such as whales, where the term whaling is more appropriate.'
WHERE Tag = 'TXT_KEY_TECH_SAILING_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Military Theory'
WHERE Tag = 'TXT_KEY_TECH_ARCHERY_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Military theory is the analysis of normative behavior and trends in military affairs and military history, beyond simply describing events in war, Military theories, especially since the influence of Clausewitz in the nineteenth century, attempt to encapsulate the complex cultural, political and economic relationships between societies and the conflicts they create. Theories and conceptions of warfare have varied in different places throughout human history. The Chinese Sun Tzu is recognized by scholars to be one of the earliest military theorists. His now-iconic Art of War laid the foundations for operational planning, tactics, strategy and logistics. In India, Chanakya (350  275 BCE) laid the foundations of military theory through his seminal text called Arthashastra. While the views of Clausewitz, Sun Tzu and Kautilya are not directly applicable to the modern battlefield, they are still referenced and acknowledged by military theorists for the insights they provide, which are then adapted to modern times.'
WHERE Tag = 'TXT_KEY_TECH_ARCHERY_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Barracks[ENDCOLOR], an essential building for early war.'
WHERE Tag = 'TXT_KEY_TECH_ARCHERY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Trade'
WHERE Tag = 'TXT_KEY_TECH_HORSEBACK_RIDING_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Trade involves the transfer of the ownership of goods or services from one person or entity to another in exchange for other goods or services or for money. Possible synonyms of "trade" include "commerce" and "financial transaction". Types of trade include barter. A network that allows trade is called a market. The original form of trade, barter, saw the direct exchange of goods and services for other goods and services. Later one side of the barter started to involve precious metals, which gained symbolic as well as practical importance. Modern traders generally negotiate through a medium of exchange, such as money. As a result, buying can be separated from selling, or earning. The invention of money (and later credit, paper money and non-physical money) greatly simplified and promoted trade. Trade between two traders is called bilateral trade, while trade between more than two traders is called multilateral trade.'
WHERE Tag = 'TXT_KEY_TECH_HORSEBACK_RIDING_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receive an additional [ICON_INTERNATIONAL_TRADE] Trade Route. Allows you to build the [COLOR_POSITIVE_TEXT]Horseman[ENDCOLOR], a fast and powerful mounted unit. Also allows you to build the [COLOR_POSITIVE_TEXT]Market[ENDCOLOR], an excellent source of early [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_TECH_HORSEBACK_RIDING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Sailing is the art of harnessing the power of the wind to move a boat over the water. To be successful, sailing required the mastery of a number of diverse skills. First, the culture must be able to construct a seaworthy craft. Second, they must be able to construct some sort of sturdy sheets (sails) which can catch the wind and transmit its energy to the hull. Third they must be able to build the various ropes and cleats and pulleys used to control the sails of the ship, and finally they must be able to successfully navigate the vessel from point to point without getting lost or capsizing or suffering some other misfortune.[NEWLINE][NEWLINE]The earliest recorded evidence of watercraft can be found in illustrations in Egypt which date from around 4,000 BC. As a riparian (river-based) civilization, the Egyptians were excellent sailors. Many of their vessels contained both oars and sails, the former being used when the winds were not strong enough or weren not coming from a favorable direction.[NEWLINE][NEWLINE]By 3000 BC the Egyptians were venturing out into the Mediterranean Sea in their vessels, steering the lengthy journey across the open water to Crete and later Phoenicia. The Egyptians also sailed down the coast of Africa, looking for knowledge, trade and treasure.[NEWLINE][NEWLINE]The earliest warships - biremes and triremes and the like - were powered by oar and sail and possessed rams or beaks on their prows. During battle the helmsman would attempt to ram the enemy vessel at high speed, while avoiding enemy attempts to do the same thing. Some vessels were equipped with archers to fire at enemy craft from longer distance, while others had soldiers aboard; these vessels sought to come alongside the enemy craft so that their soldiers could board the other ship and take it by storm.[NEWLINE][NEWLINE]The Greeks - especially the Athenians and the island colonies - were masters at naval warfare. One of the reasons that they were able to defeat their much larger and more powerful neighbor, Persia, was that the Athenian navy dominated the Aegean Sea and thus constantly threatened the increasingly lengthy supply chain of Persia.'
WHERE Tag = 'TXT_KEY_TECH_OPTICS_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Sailing'
WHERE Tag = 'TXT_KEY_TECH_OPTICS_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receive an additional [ICON_INTERNATIONAL_TRADE] Trade Route. Allows you to build the [COLOR_POSITIVE_TEXT]Windmill[ENDCOLOR] in cities built on flat land, increasing [ICON_PRODUCTION] Production.'
WHERE Tag = 'TXT_KEY_TECH_ECONOMICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the Leaning Tower of Pisa, and is an gateway to vital Renaissance-Era techs.'
WHERE Tag = 'TXT_KEY_TECH_CHEMISTRY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Cannon[ENDCOLOR], a powerful Renaissance-Era siege unit, and the [COLOR_POSITIVE_TEXT]Arsenal[ENDCOLOR], which boosts City defense.'
WHERE Tag = 'TXT_KEY_TECH_METALLURGY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Factory[ENDCOLOR], a building which greatly improves the [ICON_PRODUCTION] Production of a city.'
WHERE Tag = 'TXT_KEY_TECH_INDUSTRIALIZATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
-- Fix Steam Power Text

UPDATE Language_en_US
SET Text = 'Reveals [ICON_RES_COAL] [COLOR_POSITIVE_TEXT]Coal[ENDCOLOR], an essential Industrial-Era resource.'
WHERE Tag = 'TXT_KEY_TECH_STEAM_POWER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Rifling Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Fusilier[ENDCOLOR], a front-line infantry unit of the mid-game eras.'
WHERE Tag = 'TXT_KEY_TECH_RIFLING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Replaceable Parts Text
UPDATE Language_en_US
SET Text = 'Allows you to build the basic land unit [COLOR_POSITIVE_TEXT]Rifleman[ENDCOLOR], and the [COLOR_POSITIVE_TEXT]Military Base[ENDCOLOR], which boosts City defense.'
WHERE Tag = 'TXT_KEY_TECH_REPLACEABLE_PARTS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Combustion Text

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Landship[ENDCOLOR], an incredibly powerful armored Unit that can move after attacking.'
WHERE Tag = 'TXT_KEY_TECH_COMBUSTION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Plastics Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Research Lab[ENDCOLOR], a building which improves [ICON_RESEARCH] Science in a City.'
WHERE Tag = 'TXT_KEY_TECH_PLASTICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Rocketry Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Rocket Artillery[ENDCOLOR] military unit, and the [COLOR_POSITIVE_TEXT]Mobile SAM[ENDCOLOR], a fast unit specialized in thwarting enemy aircraft.'
WHERE Tag = 'TXT_KEY_TECH_ROCKETRY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Satellites Text
UPDATE Language_en_US
SET Text = 'Allow you to build the [COLOR_POSITIVE_TEXT]Apollo Program[ENDCOLOR], a project necessary to win a [COLOR_POSITIVE_TEXT]Science Victory[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_SATELLITES_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Advanced Ballistics Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Nuclear Missile[ENDCOLOR], a frightening weapon that requires [ICON_RES_URANIUM] Uranium, and is capable of destroying units and cities.'
WHERE Tag = 'TXT_KEY_TECH_ADVANCED_BALLISTICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Globalization Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]SS Cockpit[ENDCOLOR], a component necessary to win a [COLOR_POSITIVE_TEXT]Science Victory[ENDCOLOR]. With Globalization, each Spy you send to another Civilization as a [ICON_DIPLOMAT] Diplomat will increase the number of Delegates you control in the World Congress, bringing you closer to [COLOR_POSITIVE_TEXT]Diplomatic Victory[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_GLOBALIZATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Computers Text
UPDATE Language_en_US
SET Text = 'Receive an additional [ICON_INTERNATIONAL_TRADE] Trade Route. Allows you to build the [COLOR_POSITIVE_TEXT]Carrier[ENDCOLOR], a naval unit capable of carrying aircraft.'
WHERE Tag = 'TXT_KEY_TECH_COMPUTERS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Nuclear Fusion Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Giant Death Robot[ENDCOLOR], the strongest unit in the game. Also allows you to build the [COLOR_POSITIVE_TEXT]SS Booster[ENDCOLOR], a component necessary to win a [COLOR_POSITIVE_TEXT]Science Victory[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_NUCLEAR_FUSION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Telecom Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Nuclear Submarine[ENDCOLOR], a Naval Unit invisible to most other Units and capable of carrying 2 Missiles.'
WHERE Tag = 'TXT_KEY_TECH_TELECOM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Electronics Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Battleship[ENDCOLOR], a powerful late-game ranged Naval Unit.'
WHERE Tag = 'TXT_KEY_TECH_ELECTRONICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Combined Arms Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Tank[ENDCOLOR], a fast and deadly armor Unit.'
WHERE Tag = 'TXT_KEY_TECH_COMBINED_ARMS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Refrigeration Text
UPDATE Language_en_US
SET Text = 'Allows you to build [COLOR_POSITIVE_TEXT]Stadiums[ENDCOLOR], which increases [ICON_HAPPINESS_1] Happiness within the City. Also allows Work Boats to construct an Offshore Platform, and increases [ICON_INTERNATIONAL_TRADE] Trade Route maximum travel distance.'
WHERE Tag = 'TXT_KEY_TECH_REFRIGERATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Flight Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Triplane[ENDCOLOR], an Air Unit designed to gain control of the skies, and the [COLOR_POSITIVE_TEXT]Bomber[ENDCOLOR], which can devastate Land Units and Cities.'
WHERE Tag = 'TXT_KEY_TECH_FLIGHT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Radar Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Heavy Bomber[ENDCOLOR], a deadly air Unit capable of delivering death from afar, and the [COLOR_POSITIVE_TEXT]Fighter[ENDCOLOR], an air Unit designed to wrest control of the skies from enemy aircraft. Also allows you to build the [COLOR_POSITIVE_TEXT]Paratrooper[ENDCOLOR], a late-game infantry unit capable of paradropping behind enemy lines.'
WHERE Tag = 'TXT_KEY_TECH_RADAR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Compass Text
UPDATE Language_en_US
SET Text = 'Receive an additional [ICON_INTERNATIONAL_TRADE] Trade Route. Allows you to build the [COLOR_POSITIVE_TEXT]Harbor[ENDCOLOR], which creates city connections from cities to the capital over the water, producing [ICON_GOLD] Gold. Additionally, the [ICON_FOOD] Food yield from Fishing Boats is increased.'
WHERE Tag = 'TXT_KEY_TECH_COMPASS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Metal Casting Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Baths[ENDCOLOR], which increase the creation of [ICON_GREAT_PEOPLE] Great People.'
WHERE Tag = 'TXT_KEY_TECH_METAL_CASTING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Construction Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Arena[ENDCOLOR], which improves [ICON_HAPPINESS_1] Happiness in the empire, which in turn helps your city growth and makes [ICON_GOLDEN_AGE] Golden Ages more likely.'
WHERE Tag = 'TXT_KEY_TECH_CONSTRUCTION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Wheel Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Chariot Archer[ENDCOLOR], a fast and powerful ranged unit which requires [ICON_RES_HORSE] Horses. Also allows Workers to construct [COLOR_POSITIVE_TEXT]Roads[ENDCOLOR], which allow units to move across the map faster and provide extra [ICON_GOLD] Gold when connecting cities to your capital.'
WHERE Tag = 'TXT_KEY_TECH_THE_WHEEL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Currency Text

UPDATE Language_en_US
SET Text = 'Receive an additional [ICON_INTERNATIONAL_TRADE] Trade Route. Allows you to construct the [COLOR_POSITIVE_TEXT]Caravansary[ENDCOLOR], which greatly boosts the range and value of your land [ICON_INTERNATIONAL_TRADE] Trade Routes. Also allows Workers to construct the [COLOR_POSITIVE_TEXT]Village[ENDCOLOR], which increases the [ICON_GOLD] Gold output of map tiles.'
WHERE Tag = 'TXT_KEY_TECH_CURRENCY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Other Text Fixes
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Catapult[ENDCOLOR], a powerful siege weapon, and the [COLOR_POSITIVE_TEXT]Horse Archer[ENDCOLOR], a strong mounted ranged unit.'
WHERE Tag = 'TXT_KEY_TECH_MATHEMATICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Temple[ENDCOLOR], which increases the [ICON_PEACE] Faith output of a city. Also allows you to build the [COLOR_POSITIVE_TEXT]Courthouse[ENDCOLOR], a building which reduces the [ICON_HAPPINESS_4] Unhappiness from [ICON_OCCUPIED] Occupied Cities.'
WHERE Tag = 'TXT_KEY_TECH_PHILOSOPHY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receive an additional [ICON_INTERNATIONAL_TRADE] Trade Route. Allows Workers to build [COLOR_POSITIVE_TEXT]Railroads[ENDCOLOR] on map tiles. Connecting cities with them will increase [ICON_PRODUCTION] Production, and units traveling along them will move more quickly.'
WHERE Tag = 'TXT_KEY_TECH_RAILROAD_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows Workers to construct [COLOR_POSITIVE_TEXT]Pastures[ENDCOLOR] on [ICON_RES_COW] [COLOR_POSITIVE_TEXT]Cows[ENDCOLOR] and [ICON_RES_SHEEP] [COLOR_POSITIVE_TEXT]Sheep[ENDCOLOR]. Also reveals [ICON_RES_HORSE] Horses, which are used to build powerful mounted units.'
WHERE Tag = 'TXT_KEY_TECH_ANIMAL_HUSBANDRY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows Workers to construct [COLOR_POSITIVE_TEXT]Mines[ENDCOLOR] to increase the [ICON_PRODUCTION] Production of map tiles.'
WHERE Tag = 'TXT_KEY_TECH_MINING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reveals [ICON_RES_IRON] [COLOR_POSITIVE_TEXT]Iron[ENDCOLOR]. Also allows you to build the [COLOR_POSITIVE_TEXT]Spearman[ENDCOLOR], a military unit strong against mounted enemies.'
WHERE Tag = 'TXT_KEY_TECH_BRONZE_WORKING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build [COLOR_POSITIVE_TEXT]Walls[ENDCOLOR], which greatly improve the defense of cities from attack. Also allows Workers to construct Quarries on [ICON_RES_MARBLE] [COLOR_POSITIVE_TEXT]Marble[ENDCOLOR] and [ICON_RES_STONE] [COLOR_POSITIVE_TEXT]Stone[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_MASONRY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receive an additional [ICON_INTERNATIONAL_TRADE] Trade Route. Allows you to build the [COLOR_POSITIVE_TEXT]Caravan[ENDCOLOR], used to establish lucrative Trade Routes, and the [COLOR_POSITITVE_TEXT]Settler, a unit essential to expanding your empire. Also allows your cities to build the [COLOR_POSITIVE_TEXT]Granary[ENDCOLOR], which provides [ICON_FOOD] Food, helping your cities grow larger.'
WHERE Tag = 'TXT_KEY_TECH_POTTERY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Customs House[ENDCOLOR] and [COLOR_POSITIVE_TEXT]East India Company[ENDCOLOR], improving [ICON_GOLD] Gold in your cities.'
WHERE Tag = 'TXT_KEY_TECH_GUILDS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows land units to [COLOR_POSITIVE_TEXT]Embark[ENDCOLOR] onto water tiles and travel along the coast. Also allows you to build economic ships, useful for exploration and harvesting resources in the water like [ICON_RES_FISH] [COLOR_POSITIVE_TEXT]Fish[ENDCOLOR] and [ICON_RES_PEARLS] [COLOR_POSITIVE_TEXT]Pearls[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_SAILING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Lighthouse[ENDCOLOR] in coastal cities, improving [ICON_FOOD] Food output from water tiles, speeding their growth. Also grants you access to the [COLOR_POSITIVE_TEXT]Cargo Ship[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_OPTICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Observatory[ENDCOLOR], a powerful [ICON_RESEARCH] Science building. Also allows embarked land units to cross ocean tiles.'
WHERE Tag = 'TXT_KEY_TECH_ASTRONOMY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Frigate[ENDCOLOR] and the [COLOR_POSITIVE_TEXT]Corvette[ENDCOLOR], powerful Renaissance-era warships. Also allows coastal cities to build the [COLOR_POSITIVE_TEXT]Seaport[ENDCOLOR], which provides [ICON_PRODUCTION] Production from sea resources.'
WHERE Tag = 'TXT_KEY_TECH_NAVIGATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receive an additional [ICON_INTERNATIONAL_TRADE] Trade Route. Reveals [ICON_RES_ALUMINUM] Aluminum, a resource used for many late-game units. Also allows Cities to build the [COLOR_POSITIVE_TEXT]Stock Exchange[ENDCOLOR], a building which boosts [ICON_GOLD] Gold. Also allows you to build the [COLOR_POSITIVE_TEXT]Hydro Plant[ENDCOLOR], which increases [ICON_PRODUCTION] Production from tiles next to a River.'
WHERE Tag = 'TXT_KEY_TECH_ELECTRICITY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Penicilin and Nanotech 

UPDATE Language_en_US
SET Text = 'All Workers receive the [COLOR_POSITIVE_TEXT]Fallout Reduction[ENDCOLOR] Promotion, which reduces the amount of damage they take while scrubbing Fallout. Allows you to build the [COLOR_POSITIVE_TEXT]Marine[ENDCOLOR], a Unit that specializes in amphibious assaults. Also allows you to build the [COLOR_POSITIVE_TEXT]Medical Lab[ENDCOLOR], a building which speeds the [ICON_FOOD] growth of your Cities.'
WHERE Tag = 'TXT_KEY_TECH_PENICILIN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'All Workers receive the [COLOR_POSITIVE_TEXT]Fallout Immunity[ENDCOLOR] Promotion, which eliminates any damage they would take while scrubbing Fallout. Allows you to build the [COLOR_POSITIVE_TEXT]XCOM Squad[ENDCOLOR], and the [COLOR_POSITIVE_TEXT]SS Stasis Chamber[ENDCOLOR], one of the Spaceship parts necessary to win a [COLOR_POSITIVE_TEXT]Science Victory[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_NANOTECHNOLOGY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Who knows what the future holds?[NEWLINE][NEWLINE]A repeating technology that will increase your score and create 1 [ICON_HAPPINESS_1] Happiness in every City each time it is researched.'
WHERE Tag = 'TXT_KEY_TECH_FUTURE_TECH_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
