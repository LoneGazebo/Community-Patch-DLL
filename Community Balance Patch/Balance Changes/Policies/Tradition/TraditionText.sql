-- Opener
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Tradition[ENDCOLOR] focuses on generating Great People and the construction of an impressive Capital.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting Tradition grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +3 [ICON_CULTURE] Culture and +3 [ICON_FOOD] Food in the [ICON_CAPITAL] Capital. [NEWLINE] [ICON_BULLET] +5% [ICON_FOOD] Growth in all Cities.[NEWLINE] [ICON_BULLET] Unlocks building the [COLOR_CYAN]Hanging Gardens[ENDCOLOR].[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Each Tradition policy unlocked grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +4% [ICON_FOOD] Growth in all Cities, and +2 [ICON_FOOD] Food in your [ICON_CAPITAL] Capital.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting all Policies in Tradition grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +33% [ICON_FOOD] Food or [ICON_PRODUCTION] Production from Internal [ICON_INTERNATIONAL_TRADE] Trade Routes originating in your [ICON_CAPITAL] Capital.[NEWLINE] [ICON_BULLET] Grants access to Medieval Era Policy Branches, ignoring Era requirement.[NEWLINE] [ICON_BULLET] Provides [COLOR_CYAN]1[ENDCOLOR] point (of [COLOR_CYAN]3[ENDCOLOR] required in total) towards unlocking access to Ideologies.[NEWLINE] [ICON_BULLET] Allows for the purchase of [ICON_GREAT_ENGINEER] Great Engineers with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_TRADITION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Aristocracy
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Justice[ENDCOLOR][NEWLINE]Cities with a garrison gain +25% [ICON_RANGE_STRENGTH] Ranged Combat Strength. [COLOR_CYAN]Royal Guardhouse[ENDCOLOR] built in [ICON_CAPITAL] Capital (1 Engineer Specialist, +3 [ICON_STRENGTH] Defense, +25 Hit Points, +3 [ICON_PRODUCTION] Production).'
WHERE Tag = 'TXT_KEY_POLICY_ARISTOCRACY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Justice'
WHERE Tag = 'TXT_KEY_POLICY_ARISTOCRACY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Justice, in its broadest context, includes both the attainment of that which is just and the philosophical discussion of that which is just. The concept of justice is based on numerous fields, and many differing viewpoints and perspectives including the concepts of moral correctness based on law, equity, ethics, rationality, religion, and fairness. Often, the general discussion of justice is divided into the realm of societal justice as found in philosophy, theology and religion, and, procedural justice as found in the study and application of the law.'
WHERE Tag = 'TXT_KEY_POLICY_ARISTOCRACY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Oligarchy
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Sovereignty[ENDCOLOR][NEWLINE]+15% [ICON_GREAT_PEOPLE] Great Person Rate in all Cities. [COLOR_CYAN]Throne Room[ENDCOLOR] built in [ICON_CAPITAL] Capital (1 Writer Specialist, 1 [ICON_GREAT_WORK] Great Work of Writing slot, +3 [ICON_CULTURE] Culture).'
WHERE Tag = 'TXT_KEY_POLICY_OLIGARCHY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Sovereignty'
WHERE Tag = 'TXT_KEY_POLICY_OLIGARCHY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Sovereignty is understood in jurisprudence as the full right and power of a governing body to govern itself without any interference from outside sources or bodies. In political theory, sovereignty is a substantive term designating supreme authority over some polity. It is a basic principle underlying the dominant Westphalian model of state foundation.'
WHERE Tag = 'TXT_KEY_POLICY_OLIGARCHY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Legalism
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Majesty[ENDCOLOR][NEWLINE]Specialists in [ICON_CAPITAL] Capital consume half the normal amount of [ICON_FOOD] Food. [COLOR_CYAN]State Treasury[ENDCOLOR] built in [ICON_CAPITAL] Capital (1 Merchant Specialist, +3 [ICON_GOLD] Gold, [ICON_GOLDEN_AGE] Golden Ages last 25% longer).'
WHERE Tag = 'TXT_KEY_POLICY_MONARCHY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Majesty'
WHERE Tag = 'TXT_KEY_POLICY_MONARCHY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Majesty is an English word derived ultimately from the Latin maiestas, meaning greatness, and used as a style by many monarchs, usually kings or emperors. Where used, the style outranks [Royal] Highness. It has cognates in many other languages.'
WHERE Tag = 'TXT_KEY_POLICY_MONARCHY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Landed Elite
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Splendor[ENDCOLOR][NEWLINE]Expending a Great Person grants 50 [ICON_GOLDEN_AGE] Golden Age Points and [ICON_CULTURE] Culture, scaling with Era. [COLOR_CYAN]Palace Garden[ENDCOLOR] built in [ICON_CAPITAL] Capital (1 Scientist Specialist, +3 [ICON_RESEARCH] Science, +2 [ICON_CULTURE] Culture from Monuments and Gardens).'
WHERE Tag = 'TXT_KEY_POLICY_LANDED_ELITE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Splendor'
WHERE Tag = 'TXT_KEY_POLICY_LANDED_ELITE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Splendor a term often used when describing the public activities of monarchs and aristocrats, specifically when referring to displays of power or prestige.'
WHERE Tag = 'TXT_KEY_POLICY_LANDED_ELITE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Monarchy
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Devotion[ENDCOLOR][NEWLINE]Palace and National Wonders with Building requirements gain +2 [ICON_HAPPINESS_1] Happiness. [COLOR_CYAN]Court Chapel[ENDCOLOR] built in [ICON_CAPITAL] Capital (1 Artist Specialist, 1 [ICON_GREAT_WORK] Great Work of Art slot, +3 [ICON_PEACE] Faith).'
WHERE Tag = 'TXT_KEY_POLICY_LEGALISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Devotion'
WHERE Tag = 'TXT_KEY_POLICY_LEGALISM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Devotion is faithfulness or a loyalty to a person, country, group, or cause. Philosophers disagree on what can be an object of loyalty as some argue that loyalty is strictly interpersonal and only other human beings can be the object of loyalty. The idea has been treated by writers from Aeschylus through John Galsworthy to Joseph Conrad, by psychologists, psychiatrists, sociologists, scholars of religion, political economists, scholars of business and marketing, and—most particularly—by political theorists, who deal with it in terms of loyalty oaths and patriotism.'
WHERE Tag = 'TXT_KEY_POLICY_LEGALISM_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Finisher