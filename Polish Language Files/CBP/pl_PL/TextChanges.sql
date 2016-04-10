--translated by mrlg

-- Update for Progress Screen Texts (a little more serious)

	UPDATE Language_pl_PL
	SET Text = '{1_Name:textkey} kończy swoje największe dzieło, listę:'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Najwspanialszych budowniczych w historii'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_WONDERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Najlepiej prosperujących cywilizacji'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_FOOD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Najbardziej zapracowanych dziś ludzi'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_PRODUCTION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Najbogatszych ludzi na świecie'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_GOLD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Najbardziej wykształconych ludzi w historii'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_SCIENCE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Najbardziej wytwornych kultur'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_CULTURE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Najbardziej stabilnych narodów w historii'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_HAPPINESS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Najpotężniejszych władców w historii'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_POWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Najbardziej wpływowych nacji'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_CULTURAL_INFLUENCE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );
	
	UPDATE Language_pl_PL
	SET Text = 'Najbardziej olśniewających miast'
	WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_CITY_TOURISM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );
	
-- Spy Stuff

	UPDATE Language_pl_PL
	SET Text = 'Jak jednemu z twoich brudnych agentów udało się przeniknąć nasze osłony? To mogą być ostatnie słowa, jakie zamienimy pokojowo.'
	WHERE Tag = 'TXT_KEY_GENERIC_CAUGHT_YOUR_SPY_HOSTILE_1' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Mogę puścić ci płazem twoje niedawne działania szpiegowskie na moich ziemiach, jeśli przyrzekniesz, że już nigdy więcej tego nie zrobisz.'
	WHERE Tag = 'TXT_KEY_GENERIC_CAUGHT_YOUR_SPY_FRIENDLY_1' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Cieszę się, że zgadzasz się zaprzestać szpiegowania moich ludzi.'
	WHERE Tag = 'TXT_KEY_GENERIC_HUMAN_CAUGHT_YOUR_SPY_GOOD_1' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );




