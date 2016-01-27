UPDATE Projects
Set FreeBuildingClassIfFirst = 'BUILDINGCLASS_LABORATORY'
WHERE Type = 'PROJECT_MANHATTAN_PROJECT';

UPDATE Language_en_US
SET Text = 'Allows your Cities to build Atomic Bombs and Nuclear Missiles.[NEWLINE][NEWLINE]If you are the [COLOR_POSITIVE_TEXT]first[ENDCOLOR] to complete this, receive a free Research Lab and Atomic Bomb in your [ICON_CAPITAL] Capital.'
WHERE Tag = 'TXT_KEY_PROJECT_MANHATTAN_PROJECT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Manhattan Project allows a civilization to construct nuclear weapons. Each civilization must construct the Manhattan Project before it can construct nukes. If you are part of the first global team to complete this, you will receive a free Research Lab and Atomic Bomb in your Capital.'
WHERE Tag = 'TXT_KEY_PROJECT_MANHATTAN_PROJECT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Projects
Set FreePolicyIfFirst = 'POLICY_FIRST_ON_MOON'
WHERE Type = 'PROJECT_APOLLO_PROGRAM';

UPDATE Language_en_US
SET Text = 'Signals the start of the space race, allowing your Cities to build spaceship parts. When the spaceship is completed, you win a [COLOR_POSITIVE_TEXT]Science Victory[ENDCOLOR]![NEWLINE][NEWLINE]If you are the [COLOR_POSITIVE_TEXT]first[ENDCOLOR] to complete this, receive a free [ICON_GREAT_SCIENTIST] Great Scientist near your [ICON_CAPITAL] Capital, and a [ICON_GOLDEN_AGE] Golden Age begins.'
WHERE Tag = 'TXT_KEY_PROJECT_APOLLO_PROGRAM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Apollo Program is the start of the space race. It allows construction of spaceship parts. Each civilization must complete this project before they can construct spaceship parts. If you are part of the first global team to complete this, you will receive a free Great Scientist near your Capital, and a Golden Age will begin.'
WHERE Tag = 'TXT_KEY_PROJECT_APOLLO_PROGRAM_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );
