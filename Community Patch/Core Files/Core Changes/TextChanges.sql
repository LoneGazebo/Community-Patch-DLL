-- Fixed diacritics for spy names.

UPDATE Language_en_US
SET Text = 'Antônio'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_0' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Estêvão'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_3' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Fernão'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_4' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Tomé'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_8' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

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
WHERE Tag = 'TXT_KEY_DIPLO_TRADE_PARTNER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );


