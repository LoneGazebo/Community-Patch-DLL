-- Peace Loving (Now Mandate of Heaven)
UPDATE Language_en_US
SET Text = 'Unlocks the Celestial Throne National Wonder (+2 [ICON_PEACE] Faith, [ICON_CULTURE] Culture, [ICON_FOOD] Food, [ICON_RESEARCH] Science, [ICON_GOLD] Gold, and [ICON_PRODUCTION] Production; unlocks Reformation Belief). Holy City produces +15% of its yields during a [ICON_GOLDEN_AGE] Golden Age.'
WHERE Tag = 'TXT_KEY_BELIEF_PEACE_LOVING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Mandate of Heaven'
WHERE Tag = 'TXT_KEY_BELIEF_PEACE_LOVING_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Interfaith Dialog (Now Hero Worship)
UPDATE Language_en_US
SET Text = 'Unlocks Great Altar National Wonder (+5 [ICON_PEACE] Faith, and +15% Military Unit [ICON_PRODUCTION] Production; unlocks Reformation Belief). Receive [ICON_PEACE] Faith and [ICON_GOLDEN_AGE] Golden Age points when you conquer a City. Bonus scales with City population and Era.'
WHERE Tag = 'TXT_KEY_BELIEF_INTERFAITH_DIALOGUE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Hero Worship'
WHERE Tag = 'TXT_KEY_BELIEF_INTERFAITH_DIALOGUE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Ceremonial Burial
UPDATE Language_en_US
SET Text = 'Unlocks Mausoleum National Wonder (+5 [ICON_PEACE] Faith, and gain [ICON_PEACE] Faith when an owned unit is killed in battle; unlocks Reformation Belief). Receive [ICON_PEACE] Faith and [ICON_CULTURE] Culture whenever a Great Person is expended. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_CEREMONIAL_BURIAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Church Property (Now Holy Law)
UPDATE Language_en_US
SET Text = 'Unlocks Divine Court National Wonder (+4 [ICON_PEACE] Faith, +6 [ICON_GOLD] Gold; unlocks Reformation Belief). Receive [ICON_PEACE] Faith, [ICON_RESEARCH] Science, and [ICON_GOLD] Gold when you purchase a Policy. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_CHURCH_PROPERTY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Holy Law'
WHERE Tag = 'TXT_KEY_BELIEF_CHURCH_PROPERTY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Tithe (Now Way of the Pilgrim)
UPDATE Language_en_US
SET Text = 'Unlocks Reliquary National Wonder (+4 [ICON_PEACE] Faith, +2 [ICON_CULTURE] Culture, and 4 [ICON_GREAT_WORK] Art/Artfiact slots; unlocks Reformation Belief). Receive [ICON_TOURISM] Tourism when you spread your Religion to foreign cities. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_TITHE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Way of the Pilgrim'
WHERE Tag = 'TXT_KEY_BELIEF_TITHE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Initiation Rites (Now Way of Transcendence)
UPDATE Language_en_US
SET Text = 'Unlocks Sacred Garden National Wonder (+3 [ICON_PEACE] Faith, +5 [ICON_CULTURE] Culture; unlocks Reformation Belief). +300 to all National Yields when you enter a new Era. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_INITIATION_RITES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Way of Transcendence'
WHERE Tag = 'TXT_KEY_BELIEF_INITIATION_RITES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Papal Primacy (Now Council of Elders)
UPDATE Language_en_US
SET Text = 'Unlocks Holy Council National Wonder (+4 [ICON_PEACE] Faith, +5 [ICON_FOOD] Food; unlocks Reformation Belief). Receive a boost to current [ICON_RESEARCH] Science research when a city adopts your Religion. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_PAPAL_PRIMACY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Council of Elders'
WHERE Tag = 'TXT_KEY_BELIEF_PAPAL_PRIMACY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Pilgrimage (Now Apostolic Tradition
UPDATE Language_en_US
SET Text = 'Unlocks Apostolic Palace National Wonder (+4 [ICON_PEACE] Faith, and +4 [ICON_GOLDEN_AGE] Golden Age points; unlocks Reformation Belief). Receive [ICON_GOLDEN_AGE] Golden Age points when you spread your Religion to cities other than your Holy City. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_PILGRIMAGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Apostolic Tradition'
WHERE Tag = 'TXT_KEY_BELIEF_PILGRIMAGE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- World Church (Now Theocratic Rule)
UPDATE Language_en_US
SET Text = 'Unlocks Grand Ossuary National Wonder (+10 [ICON_PEACE] Faith; unlocks Reformation Belief). We Love the King Day boosts the [ICON_PEACE] Faith, [ICON_CULTURE] Culture, [ICON_GOLD] Gold and [ICON_RESEARCH] Science output of a city by 15%.'
WHERE Tag = 'TXT_KEY_BELIEF_WORLD_CHURCH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Theocratic Rule'
WHERE Tag = 'TXT_KEY_BELIEF_WORLD_CHURCH_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Top Panel Tooltip Adjustment (make generic for more than just founder beliefs)
UPDATE Language_en_US
SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] from Religion.'
WHERE Tag = 'TXT_KEY_TP_FAITH_FROM_RELIGION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );