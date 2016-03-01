-- Opener/Finisher
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Aesthetics[ENDCOLOR] improves your ability to generate [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting Aesthetics grants:[ENDCOLOR][NEWLINE] [ICON_BULLET] +5% Great Writer, Great Artist, and Great Musician rates in all cities. [NEWLINE] [ICON_BULLET] Receive a large amount of [ICON_CULTURE] Culture every time you expend a [ICON_GREAT_PEOPLE] Great Person.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Each Aesthetics policy unlocked grants:[ENDCOLOR][NEWLINE] [ICON_BULLET] +5% Great Writer, Great Artist, and Great Musician rates in all cities.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting all policies in Aesthetics grants:[ENDCOLOR][NEWLINE] [ICON_BULLET] Unlocks building the [COLOR_CYAN]Uffizi[ENDCOLOR]. [NEWLINE] [ICON_BULLET] Doubles [ICON_TOURISM] Tourism theming bonuses from Buildings and Wonders. [NEWLINE] [ICON_BULLET] Allows you to see Hidden Antiquity Sites.[NEWLINE] [ICON_BULLET] Grants access to Renaissance Era Policy Branches, ignoring Era requirement. [NEWLINE] [ICON_BULLET] Provides [COLOR_CYAN]1[ENDCOLOR] point (of [COLOR_CYAN]3[ENDCOLOR] required in total) towards unlocking access to Ideologies. [NEWLINE] [ICON_BULLET] Allows for the purchase of [ICON_GREAT_MUSICIAN] Great Musicians with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_AESTHETICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Cultural Centers (now Humanities)
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Humanism[ENDCOLOR][NEWLINE]Amphitheaters, Opera Houses, Museums, and Broadcast Towers produce +3 [ICON_RESEARCH] Science. All [ICON_GREAT_WORK] Great Works produce +1 [ICON_GOLD] Gold and +1 [ICON_CULTURE] Culture.'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_CENTERS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Humanism'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_CENTERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Humanism is a philosophical and ethical stance that emphasizes the value and agency of human beings, individually and collectively, and generally prefers critical thinking and evidence (rationalism, empiricism) over established doctrine or faith (fideism). The meaning of the term humanism has fluctuated, according to the successive intellectual movements which have identified with it. Generally, however, humanism refers to a perspective that affirms some notion of a "human nature" (sometimes contrasted with antihumanism).'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_CENTERS_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Fine Arts
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Refinement[ENDCOLOR][NEWLINE]All positive [ICON_HAPPINESS_1] Happiness is added to your Empire-wide [ICON_CULTURE] Culture rate. +1 [ICON_HAPPINESS_1] Happiness and +3 [ICON_CULTURE] Culture from all Guilds.'
WHERE Tag = 'TXT_KEY_POLICY_FINE_ARTS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Refinement'
WHERE Tag = 'TXT_KEY_POLICY_FINE_ARTS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Sophistication is the quality of refinement — displaying good taste, wisdom and subtlety rather than crudeness, stupidity and vulgarity. In the perception of social class, sophistication can link with concepts such as status, privilege and superiority.'
WHERE Tag = 'TXT_KEY_POLICY_FINE_ARTS_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Flourishing of the Arts
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Heritage[ENDCOLOR][NEWLINE]All World Wonders produce +2 [ICON_TOURISM] Tourism. Entering a [ICON_GOLDEN_AGE] Golden Age or completing an Archaeological Dig generates [ICON_TOURISM] Tourism with all known Civilizations.'
WHERE Tag = 'TXT_KEY_POLICY_FLOURISHING_OF_ARTS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Heritage'
WHERE Tag = 'TXT_KEY_POLICY_FLOURISHING_OF_ARTS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Cultural heritage is the legacy of physical artifacts and intangible attributes of a group or society that are inherited from past generations, maintained in the present and bestowed for the benefit of future generations. Cultural heritage includes tangible culture (such as buildings, monuments, landscapes, books, works of art, and artifacts), intangible culture (such as folklore, traditions, language, and knowledge), and natural heritage (including culturally significant landscapes, and biodiversity).'
WHERE Tag = 'TXT_KEY_POLICY_FLOURISHING_OF_ARTS_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Artistic Genius
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]National Treasure[ENDCOLOR][NEWLINE]A [ICON_GREAT_PEOPLE] Great Person of your choice appears near your [ICON_CAPITAL] Capital. +3 [ICON_CULTURE] Culture from [ICON_GREAT_PEOPLE] Great Person Improvements.'
WHERE Tag = 'TXT_KEY_POLICY_ARTISTIC_GENIUS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'National Treasure'
WHERE Tag = 'TXT_KEY_POLICY_ARTISTIC_GENIUS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The idea of national treasure, like national epics and national anthems, is part of the language of Romantic nationalism, which arose in the late 18th century and 19th centuries. Nationalism is an ideology which supports the nation as the fundamental unit of human social life, which includes shared language, values and culture. Thus national treasure, part of the ideology of nationalism, is shared culture.'
WHERE Tag = 'TXT_KEY_POLICY_ARTISTIC_GENIUS_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Cultural Exchange
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Cultural Exchange[ENDCOLOR][NEWLINE]Increases the [ICON_TOURISM] Tourism modifier for shared religion, trade routes, and open borders by 25% each. The empire immediately enters a [ICON_GOLDEN_AGE] Golden Age.'
WHERE Tag = 'TXT_KEY_POLICY_ETHICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );
