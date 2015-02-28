	
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

-- Public Opinion

	UPDATE Language_en_US
	SET Text = '{1_Num} from Public Opinion (Ideological Pressure or War Weariness).'
	WHERE Tag = 'TXT_KEY_TP_UNHAPPINESS_PUBLIC_OPINION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );