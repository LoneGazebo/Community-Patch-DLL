-- Happiness from Policies to Culture/Science change
	UPDATE Language_en_US
	SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] from Policies.'
	WHERE Tag = 'TXT_KEY_TP_CULTURE_FROM_HAPPINESS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] [ICON_RESEARCH] from Policies.'
	WHERE Tag = 'TXT_KEY_TP_SCIENCE_FROM_HAPPINESS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] [ICON_RESEARCH] from Scholasticism.'
	WHERE Tag = 'TXT_KEY_TP_SCIENCE_FROM_MINORS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = '[ICON_RES_ARTIFACTS] Sites Left: {1_Num}'
	WHERE Tag = 'TXT_KEY_ANTIQUITY_SITES_TO_EXCAVATE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = '[ICON_RES_HIDDEN_ARTIFACTS] Hidden Sites Left: {1_Num}'
	WHERE Tag = 'TXT_KEY_HIDDEN_SITES_TO_EXCAVATE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );
-- Update for Progress Screen Texts (a little more serious)

	UPDATE Language_en_US
	SET Text = '{1_Name:textkey} has completed their greatest work, the list of:'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Greatest Builders in History'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_WONDERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Most Prosperous Civilizations'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_FOOD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Most Industrious People Today'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_PRODUCTION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Wealthiest People in the World'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_GOLD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Most Advanced People in History'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_SCIENCE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Most Sophisticated Cultures'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_CULTURE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Most Stable Nations in History'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_HAPPINESS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Mightiest Rulers in History'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_POWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Text Changes for Spies in Cities
	UPDATE Language_en_US
	SET Text = 'Potential reflects the vulnerability of a city to Espionage. The higher the value, the more vulnerable the city. The base value [COLOR_POSITIVE_TEXT](a scale, from 1 to 10)[ENDCOLOR] is based on the overall prosperity and happiness of the city, specifically its science and gold output. Potential may be decreased by defensive buildings in the city, such as the Constabulary and the Police Station. Potential may increase when using a higher ranking spy to steal technologies from the city.[NEWLINE][NEWLINE]Click to sort cities by their potential.'
	WHERE Tag = 'TXT_KEY_EO_POTENTIAL_SORT_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Potential of {1_CityName} is {2_Num}.[NEWLINE][NEWLINE]If your cities have high Potential, you should consider protecting them. There are two ways to do this. You may move your own spies to your cities to act as counterspies that have a chance to catch and kill enemy spies before they steal a technology. You may also slow down how quickly enemy spies can steal technologies by constructing buildings like the Constabulary, Police Station, and the Great Firewall.'
	WHERE Tag = 'TXT_KEY_EO_OWN_CITY_POTENTIAL_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = '{1_SpyRank} {2_SpyName} is stealing from {4_CityName}.[NEWLINE]The current Potential of {5_CityName} is {6_Num}.[NEWLINE][NEWLINE]Potential reflects the vulnerability of a city to Espionage. The higher the value, the more vulnerable the city. The base value [COLOR_POSITIVE_TEXT](a scale, from 1 to 10)[ENDCOLOR] is based on the overall prosperity and happiness of the city, specifically its science and gold output. Potential may be decreased by defensive buildings in the city, such as the Constabulary and the Police Station.'
	WHERE Tag = 'TXT_KEY_EO_CITY_POTENTIAL_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = '{1_SpyRank} {2_SpyName} cannot steal technologies from {3_CityName}, however they can continue to disrupt the player through additional actions like sabotage.[NEWLINE][NEWLINE]The base Potential of {4_CityName} is {5_Num}.[NEWLINE][NEWLINE]Potential reflects the vulnerability of a city to Espionage. The higher the value, the more vulnerable the city. The base value [COLOR_POSITIVE_TEXT](a scale, from 1 to 10)[ENDCOLOR] is based on the overall prosperity and happiness of the city, specifically its science and gold output. Potential may be decreased by defensive buildings in the city, such as the Constabulary and the Police Station.'
	WHERE Tag = 'TXT_KEY_EO_CITY_POTENTIAL_CANNOT_STEAL_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Potential of {1_CityName} is believed to be {2_Num}.[NEWLINE][NEWLINE]Potential reflects the vulnerability of a city to Espionage. The higher the value, the more vulnerable the city. The base value [COLOR_POSITIVE_TEXT](a scale, from 1 to 10)[ENDCOLOR] is based on the overall prosperity and happiness of the city, specifically its science and gold output. Potential may be decreased by defensive buildings in the city, such as the Constabulary and the Police Station.'
	WHERE Tag = 'TXT_KEY_EO_CITY_ONCE_KNOWN_POTENTIAL_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = '{1_SpyRank} {2_SpyName} is in {3_CityName}. While {4_SpyRank} {5_SpyName} is in your city, they will perform counter-espionage operations to capture and kill any enemy spies that try to steal your technology or perform Advanced Actions.'
	WHERE Tag = 'TXT_KEY_CITY_SPY_YOUR_CITY_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = '{1_SpyRank} {2_SpyName} is in {3_CityName}. While {4_SpyRank} {5_SpyName} is in the city, they establish surveillance, attempt to steal any technology from that civilization, and perform Advanced Actions such as stealing Gold. {6_SpyRank} {7_SpyName} will also inform you of any intrigue that they discover during their operations.'
	WHERE Tag = 'TXT_KEY_CITY_SPY_OTHER_CIV_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Lua for Cities
	UPDATE Language_en_US
	SET Text = 'We have no tradeable cities and/or we do not have a Declaration of Friendship.'
	WHERE Tag = 'TXT_KEY_DIPLO_TO_TRADE_CITY_NO_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'They have no tradeable cities and/or we do not have a Declaration of Friendship.'
	WHERE Tag = 'TXT_KEY_DIPLO_TO_TRADE_CITY_NO_THEM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );
	
-- Public Opinion

	UPDATE Language_en_US
	SET Text = '{1_Num} from Public Opinion (Ideological Pressure or War Weariness).'
	WHERE Tag = 'TXT_KEY_TP_UNHAPPINESS_PUBLIC_OPINION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Building Purchased
	UPDATE Language_en_US
	SET Text = 'When you have enough [ICON_GOLD] Gold or [ICON_PEACE] Faith, you can spend it on units and buildings.'
	WHERE Tag = 'TXT_KEY_CITYVIEW_PURCHASE_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'You can change city construction orders on the City Screen. You can also expend gold to purchase a unit (or invest in a building) on this screen as well. See the City Screen section for details.'
	WHERE Tag = 'TXT_KEY_BUILDINGS_PURCHASING_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'You can purchase units with gold, or invest gold in buildings to reduce their construction cost, from within your city screen. This can be helpful if you need something in a hurry, like additional units to defend against an invader.'
	WHERE Tag = 'TXT_KEY_CITIES_PURCHASINGITEM_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = ' You can spend gold to purchase units or invest in a building in a city. Click on a unit (if you can afford it!) and it will be immediately trained in the city, and the amount deducted from your treasury. If you click on a building, you will invest in it, reducing the production cost of the building by 50%.[NEWLINE][NEWLINE]Note that "projects" - the Manhattan Project, etc. - cannot be purchased.'
	WHERE Tag = 'TXT_KEY_GOLD_PURCHASEUNITS_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );
	
-- Resource Limit breached
	UPDATE Language_en_US
	SET Text = ' You are currently using more {1_Resource:textkey} than you have! All units which require it are [COLOR_NEGATIVE_TEXT]unable to heal[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_NOTIFICATION_OVER_RESOURCE_LIMIT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );
	
-- Spy Stuff

	UPDATE Language_en_US
	SET Text = 'How did your wretched spies get through my defenses? These may be the last words we speak in peace.'
	WHERE Tag = 'TXT_KEY_GENERIC_CAUGHT_YOUR_SPY_HOSTILE_1' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'I am willing to overlook your recent espionage activites in my lands if you make a solemn promise that it will not happen again.'
	WHERE Tag = 'TXT_KEY_GENERIC_CAUGHT_YOUR_SPY_FRIENDLY_1' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'I am glad you agreed to stop spying on my people.'
	WHERE Tag = 'TXT_KEY_GENERIC_HUMAN_CAUGHT_YOUR_SPY_GOOD_1' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- CS Text Change
	UPDATE Language_en_US
	SET Text = 'Giving Heavy Tribute'
	WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_UNIT_RELUCTANCE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Aesthetics Change
	UPDATE Language_en_US
	SET Text = 'When improved with an Archaeological Dig, a Hidden Antiquity Site can then yield an Artifact that can be placed in a Great Work of Art slot, or be transformed into a Landmark improvement. Hidden Antiquity Sites are only visible to civs that have all the policies in the Aesthetics policy tree.'
	WHERE Tag = 'TXT_KEY_RESOURCE_HIDDEN_ARTIFACTS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Help Text Changes
	UPDATE Language_en_US
	SET Text = '[ICON_TOURISM] Tourism is the primary yield you will use to spread your cultural influence to other civilizations. It is generated passively by [COLOR_POSITIVE_TEXT]Great Works[ENDCOLOR] or [COLOR_POSITIVE_TEXT]Artifacts[ENDCOLOR, and actively by [COLOR_POSITIVE_TEXT]Historic Events[ENDCOLOR] (for more on ''Historic Events,'' see the section with this label). As you place more and more Great Works and Artifacts into your Museums, Amphitheaters, Opera Houses, etc., your [ICON_TOURISM] Tourism will continue to increase. Additionally, you can boost your [ICON_TOURISM] Tourism output quite a bit through Open Borders, Trade Routes, shared Religion, and certain Ideological Tenets.'
	WHERE Tag = 'TXT_KEY_CULTURE_TOURISM_HEADING2_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Capture Popup

	UPDATE Language_en_US	
	SET Text = '[COLOR_POSITIVE_TEXT]Puppeting[ENDCOLOR] the City causes it to generate Gold, Science, Culture, etc. for you like a normal City, but you [COLOR_POSITIVE_TEXT]may not choose what it produces[ENDCOLOR] or customize the City.[NEWLINE][NEWLINE]It will contribute much less [ICON_HAPPINESS_4] than an Annexed City, and will not increase the cost of your Social Policies and Great People. If you choose this, you may later Annex the City at any point.'
	WHERE Tag = 'TXT_KEY_POPUP_CITY_CAPTURE_INFO_PUPPET' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US	
	SET Text = '[COLOR_POSITIVE_TEXT]Annexing[ENDCOLOR] the City into your empire causes it to become a normal City that produces extra [ICON_HAPPINESS_4] Unhappiness (until you construct a Courthouse).[NEWLINE][NEWLINE]Immediately Annexing the City (instead of Puppeting or Razing the City) [COLOR_POSITIVE_TEXT]invests[ENDCOLOR] in a Courthouse in the City, reducing its construction time by at least 50%, and [COLOR_POSITIVE_TEXT]halves[ENDCOLOR] City [ICON_RESISTANCE] Resistance.'
	WHERE Tag = 'TXT_KEY_POPUP_CITY_CAPTURE_INFO_ANNEX' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_en_US	
	SET Text = '[COLOR_POSITIVE_TEXT]Razing[ENDCOLOR] the City will burn [ICON_RAZING] down each turn until it reaches 0 population, and is removed from the game. This produces [ICON_HAPPINESS_4] Unhappiness equal to the City''s population, but also greatly increases your [COLOR_POSITIVE_TEXT]War Score[ENDCOLOR] versus this player.[NEWLINE][NEWLINE]While razing, there is a large risk of generating [COLOR_NEGATIVE_TEXT]Partisans[ENDCOLOR] loyal to the former owner (if you are still at war with this player).'
	WHERE Tag = 'TXT_KEY_POPUP_CITY_CAPTURE_INFO_RAZE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Citizen Food Consumption
	UPDATE Language_en_US	
	SET Text = '-{1_Num} {2_IconString} eaten by [ICON_CITIZEN] Citizens'
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
	SET Text = 'As a [COLOR_POSITIVE_TEXT]Militaristic[ENDCOLOR] City-State, they will regularly give you military Units. (+{1_Science} [ICON_RESEARCH] Science)'
	WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_MILITARISTIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );		

	-- WC Stiff	
	UPDATE Language_en_US
	SET Text = '[ICON_TOURISM] Tourism increases by 50% for 20 Turns.  Free Social Policy.'
	WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_GAMES_3_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );
	
	UPDATE Language_en_US
	SET Text = '[ICON_CULTURE] Culture increases by 33% for 20 Turns.'
	WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_3_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );
