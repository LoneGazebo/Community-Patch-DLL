-- Liberty Opener
UPDATE Language_en_US
SET Text = 'PROGRESS'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_LIBERTY_CAP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Progress'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_LIBERTY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Progress[ENDCOLOR] is best for civilizations which desire robust infrastructure and scientific advancement.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting Progress grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] Receive 30 [ICON_RESEARCH] Science when a [ICON_CITIZEN] Citizen is born in your [ICON_CAPITAL] Capital, scaling with Era. [NEWLINE] [ICON_BULLET] Receive 10 [ICON_CULTURE] Culture when a Technology is researched.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Each Progress policy unlocked grants:[ENDCOLOR] [NEWLINE] [ICON_BULLET] Gain an additional 10 [ICON_CULTURE] Culture when a Technology is researched, scaling with Era.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting all policies in Progress grants:[ENDCOLOR][NEWLINE] [ICON_BULLET] Unlocks building the [COLOR_CYAN]Parthenon[ENDCOLOR]. [NEWLINE] [ICON_BULLET] Receive 20 [ICON_GOLD] Gold when a citizen is born in any city, scaling with Era.[NEWLINE] [ICON_BULLET] Grants access to Medieval Era Policy Branches, ignoring Era requirement.[NEWLINE] [ICON_BULLET] Provides [COLOR_CYAN]1[ENDCOLOR] point (of [COLOR_CYAN]3[ENDCOLOR] required in total) towards unlocking access to Ideologies.[NEWLINE] [ICON_BULLET] Allows for the purchase of [ICON_GREAT_WRITER] Great Writers with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_LIBERTY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'In intellectual history, the Idea of Progress is the idea that advances in technology, science, and social organization can produce an improvement in the human condition. That is, people can become better in terms of quality of life (social progress) through economic development (modernization), and the application of science and technology (scientific progress). The assumption is that the process will happen once people apply their reason and skills, for it is not divinely foreordained. The role of the expert is to identify hindrances that slow or neutralize progress.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_LIBERTY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Collective Rule (Now Code of Laws)
UPDATE Language_en_US
SET Text = 'Organization'
WHERE Tag = 'TXT_KEY_POLICY_COLLECTIVE_RULE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Organization[ENDCOLOR][NEWLINE] Cities earn +10 [ICON_FOOD] Food and [ICON_CULTURE] Culture when they construct Buildings. Bonuses scale with Era.'
WHERE Tag = 'TXT_KEY_POLICY_COLLECTIVE_RULE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'City organization, also called ''Urban planning,'' is a technical and political process concerned with the use of land and design of the urban environment, including air and water and infrastructure passing into and out of urban areas such as transportation and distribution networks. Urban planning guides and ensures the orderly development of settlements and satellite communities which commute into and out of urban areas or share resources with it. It concerns itself with research and analysis, strategic thinking, architecture, urban design, public consultation, policy recommendations, implementation and management.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_COLLECTIVERULE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Citizenship
UPDATE Language_en_US
SET Text = 'Liberty'
WHERE Tag = 'TXT_KEY_POLICY_CITIZENSHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Liberty[ENDCOLOR][NEWLINE]A Worker appears near the [ICON_CAPITAL] Capital.[NEWLINE] +1 [ICON_MOVES] Movement for all Civilian Units.'
WHERE Tag = 'TXT_KEY_POLICY_CITIZENSHIP_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The modern concept of political liberty has its origins in the Greek concepts of free labor and slave labor. To be free, to the Greeks, was to not have a master, to be independent from a master (to live like one likes). That was the original Greek concept of freedom. It is closely linked with the concept of democracy.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_CITIZENSHIP_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Republic (Now Civil Engineers)
UPDATE Language_en_US
SET Text = 'Expertise'
WHERE Tag = 'TXT_KEY_POLICY_REPUBLIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Expertise[ENDCOLOR][NEWLINE] +15% [ICON_PRODUCTION] Production towards buildings. Tile improvement construction rate increased by 25%.'
WHERE Tag = 'TXT_KEY_POLICY_REPUBLIC_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'An expert is someone widely recognized as a reliable source of technique or skill whose faculty for judging or deciding rightly, justly, or wisely is accorded authority and status by peers or the public in a specific well-distinguished domain. An expert, more generally, is a person with extensive knowledge or ability based on research, experience, or occupation and in a particular area of study. Experts are called in for advice on their respective subject, but they do not always agree on the particulars of a field of study. An expert can be believed, by virtue of credential, training, education, profession, publication or experience, to have special knowledge of a subject beyond that of the average person, sufficient that others may officially (and legally) rely upon the individual''s opinion. Historically, an expert was referred to as a sage (Sophos). The individual was usually a profound thinker distinguished for wisdom and sound judgment.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_REPUBLIC_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Representation 
UPDATE Language_en_US
SET Text = 'Equality'
WHERE Tag = 'TXT_KEY_POLICY_REPRESENTATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Equality[ENDCOLOR][NEWLINE]+2 [ICON_PRODUCTION] Production in every City, and +1 [ICON_HAPPINESS_1] Happiness for every 15 [ICON_CITIZEN] Citizens in [ICON_PUPPET] non-Puppet Cities.'
WHERE Tag = 'TXT_KEY_POLICY_REPRESENTATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Social equality is a state of affairs in which all people within a specific society or isolated group have the same status in certain respects, often including civil rights, freedom of speech, property rights, and equal access to social goods and services. However, it also includes concepts of health equity, economic equality and other social securities. It also includes equal opportunities and obligations, and so involves the whole of society. Social equality requires the absence of legally enforced social class or caste boundaries and the absence of discrimination motivated by an inalienable part of a person''s identity.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_REPRESENTATION_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Meritocracy
UPDATE Language_en_US
SET Text = 'Fraternity'
WHERE Tag = 'TXT_KEY_POLICY_MERITOCRACY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Fraternity[ENDCOLOR][NEWLINE]+2 [ICON_FOOD] Food in every City, and +3 [ICON_RESEARCH] Science from [ICON_CONNECTED] City Connections.'
WHERE Tag = 'TXT_KEY_POLICY_MERITOCRACY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'In philosophy, fraternity is a kind of ethical relationship between people, which is based on love and solidarity. A synonym of fraternity is brotherhood. Fraternity is mentioned in the national motto of France, Liberté, égalité, fraternité (Liberty, equality, fraternity). Today, connotions of fraternities vary according to the context, including companionships and brotherhoods dedicated to the religious, intellectual, academic, physical and/or social pursuits of its members. Additionally, in modern times, it sometimes connotes a secret society, especially regarding freemasonry, odd fellows and various academic and student societies.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_MERITOCRACY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Finisher