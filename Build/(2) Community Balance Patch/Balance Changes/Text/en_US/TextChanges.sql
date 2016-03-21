
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

	UPDATE Language_en_US
	SET Text = 'The Most Influential Nations'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_CULTURAL_INFLUENCE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );
	
	UPDATE Language_en_US
	SET Text = 'The Most Dazzling Cities'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_CITY_TOURISM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );
	
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




