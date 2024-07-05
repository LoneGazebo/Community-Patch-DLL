--------------------
-- Tradition
--------------------

-- Leader title change
UPDATE Language_en_US
SET Text = '{1_PlayerName:textkey} the Great of {2_CivName:textkey}'
WHERE Tag = 'TXT_KEY_TRADITION_TITLE';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Tradition[ENDCOLOR] focuses on generating [ICON_GREAT_PEOPLE] Great People and the construction of an impressive [ICON_CAPITAL] Capital.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting Tradition grants:[ENDCOLOR][NEWLINE][ICON_BULLET]+3 [ICON_FOOD] Food, +2 [ICON_CITIZEN] Population, and +2 [ICON_HAPPINESS_1] Happiness in the [ICON_CAPITAL] Capital.[NEWLINE][ICON_BULLET]+1 [ICON_CULTURE] Culture in the [ICON_CAPITAL] Capital for every 2 [ICON_CITIZEN] Citizens.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Each Tradition policy unlocked grants:[ENDCOLOR][NEWLINE][ICON_BULLET]+1 [ICON_FOOD] Food in every City, and +1 [ICON_RESEARCH] Science in your [ICON_CAPITAL] Capital.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting all Policies in Tradition grants:[ENDCOLOR][NEWLINE][ICON_BULLET]Unlocks building the [COLOR_POSITIVE_TEXT]University of Sankore[ENDCOLOR].[NEWLINE][ICON_BULLET]+1 [ICON_FOOD] Food and [ICON_PRODUCTION] Production from all Great Person Improvements and [ICON_RES_ARTIFACTS] Landmarks.[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]{TXT_KEY_BUILDING_THRONE_ROOM}[ENDCOLOR] built in [ICON_CAPITAL] Capital (+10% to all Yields, +1 City tile working range, 1 [ICON_VP_GREATMUSIC] Great Work of Music slot, 1 [ICON_VP_MUSICIAN] Musician Slot).[NEWLINE][ICON_BULLET]Allows for the purchase of [ICON_GREAT_ENGINEER] Great Engineers with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_TRADITION_HELP';

UPDATE Language_en_US
SET Text = 'Justice'
WHERE Tag = 'TXT_KEY_POLICY_ARISTOCRACY';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Justice[ENDCOLOR][NEWLINE][ICON_BULLET]+1 [ICON_PRODUCTION] Production in every City.[NEWLINE][ICON_BULLET]Cities with a garrison gain +25% [ICON_RANGE_STRENGTH] Ranged Combat Strength.[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]{TXT_KEY_BUILDING_CAPITAL_ENGINEER}[ENDCOLOR] built in [ICON_CAPITAL] Capital (+3 [ICON_PRODUCTION] Production, +2 [ICON_STRENGTH] Defense, +50 Hit Points, +20% [ICON_SILVER_FIST] Military Supply from [ICON_CITIZEN] Population in the City, 1 [ICON_VP_ENGINEER] Engineer Slot).'
WHERE Tag = 'TXT_KEY_POLICY_ARISTOCRACY_HELP';

UPDATE Language_en_US
SET Text = 'Justice, in its broadest context, includes both the attainment of that which is just and the philosophical discussion of that which is just. The concept of justice is based on numerous fields, and many differing viewpoints and perspectives including the concepts of moral correctness based on law, equity, ethics, rationality, religion, and fairness. Often, the general discussion of justice is divided into the realm of societal justice as found in philosophy, theology and religion, and, procedural justice as found in the study and application of the law.'
WHERE Tag = 'TXT_KEY_POLICY_ARISTOCRACY_TEXT';

UPDATE Language_en_US
SET Text = 'Sovereignty'
WHERE Tag = 'TXT_KEY_POLICY_OLIGARCHY';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Sovereignty[ENDCOLOR][NEWLINE][ICON_BULLET][ICON_CULTURE] Culture cost of tiles reduced by 20% (exponentially) in all Cities.[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]{TXT_KEY_BUILDING_PALACE_COURT_CHAPEL}[ENDCOLOR] built in [ICON_CAPITAL] Capital (+3 [ICON_PEACE] Faith, 1 [ICON_GREAT_WORK] Great Work of Art or Artifact slot, 1 [ICON_VP_ARTIST] Artist Slot).'
WHERE Tag = 'TXT_KEY_POLICY_OLIGARCHY_HELP';

UPDATE Language_en_US
SET Text = 'Sovereignty is understood in jurisprudence as the full right and power of a governing body to govern itself without any interference from outside sources or bodies. In political theory, sovereignty is a substantive term designating supreme authority over some polity. It is a basic principle underlying the dominant Westphalian model of state foundation.'
WHERE Tag = 'TXT_KEY_POLICY_OLIGARCHY_TEXT';

UPDATE Language_en_US
SET Text = 'Majesty'
WHERE Tag = 'TXT_KEY_POLICY_MONARCHY';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Majesty[ENDCOLOR][NEWLINE][ICON_BULLET]Specialists in [ICON_CAPITAL] Capital consume half the normal amount of [ICON_FOOD] Food.[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]{TXT_KEY_BUILDING_PALACE_GARDEN}[ENDCOLOR] built in [ICON_CAPITAL] Capital (+5 [ICON_FOOD] Food, +25% [ICON_GREAT_PEOPLE] Great Person Rate, -2 [ICON_HAPPINESS_3] Unhappiness from [ICON_URBANIZATION] Urbanization, 1 [ICON_GREAT_WORK] Great Work of Writing slot, 1 [ICON_VP_WRITER] Writer Slot).'
WHERE Tag = 'TXT_KEY_POLICY_MONARCHY_HELP';

UPDATE Language_en_US
SET Text = 'Majesty is an English word derived ultimately from the Latin maiestas, meaning greatness, and used as a style by many monarchs, usually kings or emperors. Where used, the style outranks [Royal] Highness. It has cognates in many other languages.'
WHERE Tag = 'TXT_KEY_POLICY_MONARCHY_TEXT';

UPDATE Language_en_US
SET Text = 'Splendor'
WHERE Tag = 'TXT_KEY_POLICY_LANDED_ELITE';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Splendor[ENDCOLOR][NEWLINE][ICON_BULLET]Expending a [ICON_GREAT_PEOPLE] Great Person grants 50 [ICON_CULTURE] Culture, scaling with Era.[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]{TXT_KEY_BUILDING_PALACE_TREASURY}[ENDCOLOR] built in [ICON_CAPITAL] Capital (+4 [ICON_GOLD] Gold, +2 [ICON_CULTURE] Culture to all Monuments, Gardens, and Baths; 1 [ICON_VP_MERCHANT] Merchant Slot).'
WHERE Tag = 'TXT_KEY_POLICY_LANDED_ELITE_HELP';

UPDATE Language_en_US
SET Text = 'Splendor a term often used when describing the public activities of monarchs and aristocrats, specifically when referring to displays of power or prestige.'
WHERE Tag = 'TXT_KEY_POLICY_LANDED_ELITE_TEXT';

UPDATE Language_en_US
SET Text = 'Ceremony'
WHERE Tag = 'TXT_KEY_POLICY_LEGALISM';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Ceremony[ENDCOLOR][NEWLINE][ICON_BULLET]+1 [ICON_HAPPINESS_1] Happiness from National Wonders with Building requirements.[NEWLINE][ICON_BULLET]+25% [ICON_PRODUCTION] Production toward National Wonders with Building requirements.[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]{TXT_KEY_BUILDING_PALACE_ASTROLOGER}[ENDCOLOR] built in [ICON_CAPITAL] Capital (+3 [ICON_RESEARCH] Science, +1 [ICON_RESEARCH] Science to all Councils, Smokehouses, and Herbalists; 1 [ICON_VP_SCIENTIST] Scientist Slot).'
WHERE Tag = 'TXT_KEY_POLICY_LEGALISM_HELP';

UPDATE Language_en_US
SET Text = 'A ceremony is an event of ritual significance, performed on a special occasion. The word may be of Etruscan origin, via the Latin caerimonia. One of the major markers of a court is ceremony. Most monarchal courts included ceremonies concerning the investiture or coronation of the monarch and audiences with the monarch. Some courts had ceremonies around the waking and the sleeping of the monarch called a levee. Orders of chivalry as honorific orders became an important part of court culture starting in the 15th century. They were the right of the monarch to create and grant as the fount of honour.'
WHERE Tag = 'TXT_KEY_POLICY_LEGALISM_TEXT';

--------------------
-- Progress
--------------------
UPDATE Language_en_US
SET Text = 'Progress'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_LIBERTY';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Progress[ENDCOLOR] is best for civilizations which desire robust infrastructure and scientific advancement.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting Progress grants:[ENDCOLOR][NEWLINE][ICON_BULLET]Receive 20 [ICON_RESEARCH] Science when a [ICON_CITIZEN] Citizen is born in your [ICON_CAPITAL] Capital and 15 [ICON_RESEARCH] Science for every [ICON_CITIZEN] Citizen already in your [ICON_CAPITAL] Capital, both scaling with Era.[NEWLINE][ICON_BULLET]Receive 15 [ICON_CULTURE] Culture when a Technology is researched, scaling with Era, and for every Technology already unlocked (not scaling with Era).[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Each Progress policy unlocked grants:[ENDCOLOR][NEWLINE][ICON_BULLET]Gain an additional 15 [ICON_CULTURE] Culture when a Technology is researched, scaling with Era.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting all policies in Progress grants:[ENDCOLOR][NEWLINE][ICON_BULLET]Unlocks building the [COLOR_POSITIVE_TEXT]Forbidden Palace[ENDCOLOR].[NEWLINE][ICON_BULLET]Receive 25 [ICON_GOLD] Gold when a [ICON_CITIZEN] Citizen is born in any city, scaling with Era.[NEWLINE][ICON_BULLET]Allows for the purchase of [ICON_GREAT_WRITER] Great Writers with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_LIBERTY_HELP';

UPDATE Language_en_US
SET Text = 'In intellectual history, the Idea of Progress is the idea that advances in technology, science, and social organization can produce an improvement in the human condition. That is, people can become better in terms of quality of life (social progress) through economic development (modernization), and the application of science and technology (scientific progress). The assumption is that the process will happen once people apply their reason and skills, for it is not divinely foreordained. The role of the expert is to identify hindrances that slow or neutralize progress.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_LIBERTY_TEXT';

UPDATE Language_en_US
SET Text = 'Organization'
WHERE Tag = 'TXT_KEY_POLICY_COLLECTIVE_RULE';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Organization[ENDCOLOR][NEWLINE][ICON_BULLET]+2 [ICON_PRODUCTION] Production in every City.[NEWLINE][ICON_BULLET]+25% [ICON_PRODUCTION] Production toward Workers and Trade Units.[NEWLINE][ICON_BULLET]+1 [ICON_MOVES] Movement for Workers.'
WHERE Tag = 'TXT_KEY_POLICY_COLLECTIVE_RULE_HELP';

UPDATE Language_en_US
SET Text = 'City organization, also called ''Urban planning,'' is a technical and political process concerned with the use of land and design of the urban environment, including air and water and infrastructure passing into and out of urban areas such as transportation and distribution networks. Urban planning guides and ensures the orderly development of settlements and satellite communities which commute into and out of urban areas or share resources with it. It concerns itself with research and analysis, strategic thinking, architecture, urban design, public consultation, policy recommendations, implementation and management.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_COLLECTIVERULE_TEXT';

UPDATE Language_en_US
SET Text = 'Liberty'
WHERE Tag = 'TXT_KEY_POLICY_CITIZENSHIP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Liberty[ENDCOLOR][NEWLINE][ICON_BULLET]+2 [ICON_GOLD] Gold in every City.[NEWLINE][ICON_BULLET]+25% Tile improvement rate.[NEWLINE][ICON_BULLET]A [COLOR_POSITIVE_TEXT]Worker[ENDCOLOR] appears near the [ICON_CAPITAL] Capital.'
WHERE Tag = 'TXT_KEY_POLICY_CITIZENSHIP_HELP';

UPDATE Language_en_US
SET Text = 'The modern concept of political liberty has its origins in the Greek concepts of free labor and slave labor. To be free, to the Greeks, was to not have a master, to be independent from a master (to live like one likes). That was the original Greek concept of freedom. It is closely linked with the concept of democracy.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_CITIZENSHIP_TEXT';

UPDATE Language_en_US
SET Text = 'Expertise'
WHERE Tag = 'TXT_KEY_POLICY_REPUBLIC';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Expertise[ENDCOLOR][NEWLINE][ICON_BULLET]+15% [ICON_PRODUCTION] Production toward Buildings.[NEWLINE][ICON_BULLET]Cities earn +10 [ICON_CULTURE] Culture when they construct Buildings, scaling with Era.'
WHERE Tag = 'TXT_KEY_POLICY_REPUBLIC_HELP';

UPDATE Language_en_US
SET Text = 'An expert is someone widely recognized as a reliable source of technique or skill whose faculty for judging or deciding rightly, justly, or wisely is accorded authority and status by peers or the public in a specific well-distinguished domain. An expert, more generally, is a person with extensive knowledge or ability based on research, experience, or occupation and in a particular area of study. Experts are called in for advice on their respective subject, but they do not always agree on the particulars of a field of study. An expert can be believed, by virtue of credential, training, education, profession, publication or experience, to have special knowledge of a subject beyond that of the average person, sufficient that others may officially (and legally) rely upon the individual''s opinion. Historically, an expert was referred to as a sage (Sophos). The individual was usually a profound thinker distinguished for wisdom and sound judgment.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_REPUBLIC_TEXT';

UPDATE Language_en_US
SET Text = 'Equality'
WHERE Tag = 'TXT_KEY_POLICY_REPRESENTATION';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Equality[ENDCOLOR][NEWLINE][ICON_BULLET]+1 [ICON_HAPPINESS_1] Happiness in every City and +1 [ICON_HAPPINESS_1] Happiness for every 15 [ICON_CITIZEN] Citizens in a City.[NEWLINE][ICON_BULLET]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD] and [ICON_PRODUCTION] Distress, [ICON_GOLD] Poverty, [ICON_RESEARCH] Illiteracy, and [ICON_CULTURE] Boredom in all Cities.'
WHERE Tag = 'TXT_KEY_POLICY_REPRESENTATION_HELP';

UPDATE Language_en_US
SET Text = 'Social equality is a state of affairs in which all people within a specific society or isolated group have the same status in certain respects, often including civil rights, freedom of speech, property rights, and equal access to social goods and services. However, it also includes concepts of health equity, economic equality and other social securities. It also includes equal opportunities and obligations, and so involves the whole of society. Social equality requires the absence of legally enforced social class or caste boundaries and the absence of discrimination motivated by an inalienable part of a person''s identity.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_REPRESENTATION_TEXT';

UPDATE Language_en_US
SET Text = 'Fraternity'
WHERE Tag = 'TXT_KEY_POLICY_MERITOCRACY';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Fraternity[ENDCOLOR][NEWLINE][ICON_BULLET]+3 [ICON_FOOD] Food in every City.[NEWLINE][ICON_BULLET]+3 [ICON_RESEARCH] Science from [ICON_CONNECTED] City Connections.'
WHERE Tag = 'TXT_KEY_POLICY_MERITOCRACY_HELP';

UPDATE Language_en_US
SET Text = 'In philosophy, fraternity is a kind of ethical relationship between people, which is based on love and solidarity. A synonym of fraternity is brotherhood. Fraternity is mentioned in the national motto of France, Liberté, égalité, fraternité (Liberty, equality, fraternity). Today, connotions of fraternities vary according to the context, including companionships and brotherhoods dedicated to the religious, intellectual, academic, physical and/or social pursuits of its members. Additionally, in modern times, it sometimes connotes a secret society, especially regarding freemasonry, odd fellows and various academic and student societies.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_MERITOCRACY_TEXT';

--------------------
-- Authority
--------------------

-- Leader title change
UPDATE Language_en_US
SET Text = '{1_PlayerName:textkey} the Mighty of {2_CivName:textkey}'
WHERE Tag = 'TXT_KEY_HONOR_TITLE';

UPDATE Language_en_US
SET Text = 'Authority'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_HONOR';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Authority[ENDCOLOR] will greatly benefit warlike and expansionist civilizations.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting Authority grants:[ENDCOLOR][NEWLINE][ICON_BULLET]+25% combat bonus VS Barbarians and receive announcements when Barbarian Camps spawn in revealed territory.[NEWLINE][ICON_BULLET]Receive 25 [ICON_CULTURE] Culture when you clear Barbarian Camps, scaling with Era.[NEWLINE][ICON_BULLET]Receive [ICON_RESEARCH] Science and [ICON_CULTURE] Culture when you kill a Unit equal to 50% of its [ICON_STRENGTH] Strength.[NEWLINE][ICON_BULLET]+1 [ICON_PRODUCTION] Production in every City.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Each Authority policy unlocked grants:[ENDCOLOR][NEWLINE][ICON_BULLET]Receive [ICON_RESEARCH] Science and [ICON_CULTURE] Culture when you kill a Unit equal to 10% of its [ICON_STRENGTH] Strength.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting all policies in Authority grants:[ENDCOLOR][NEWLINE][ICON_BULLET]Unlocks building [COLOR_POSITIVE_TEXT]Alhambra[ENDCOLOR].[NEWLINE][ICON_BULLET]Allows the purchasing of [COLOR_YELLOW]Free Companies[ENDCOLOR], [COLOR_YELLOW]Foreign Legions[ENDCOLOR], and [COLOR_YELLOW]Mercenaries[ENDCOLOR] as their prerequisite technologies are researched.[NEWLINE][ICON_BULLET]Allows for the purchase of [ICON_GREAT_GENERAL] Great Generals with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_HONOR_HELP';

UPDATE Language_en_US
SET Text = 'The word authority (Derived from the Latin word auctoritas) can be used to mean power given by the state (in the form of government, judges, police officers, etc.) or by academic knowledge of an area (someone can be an authority on a subject).'
WHERE Tag = 'TXT_KEY_POLICY_HONOR_TEXT';

UPDATE Language_en_US
SET Text = 'Tribute'
WHERE Tag = 'TXT_KEY_POLICY_WARRIOR_CODE';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Tribute[ENDCOLOR][NEWLINE][ICON_BULLET]A [COLOR_POSITIVE_TEXT]Settler[ENDCOLOR] appears near the [ICON_CAPITAL] Capital.[NEWLINE][ICON_BULLET]Gain [ICON_CULTURE] Culture in [ICON_CAPITAL] Capital equal to 25% of the Gold Tribute demanded from City-States ([COLOR_YELLOW]50% if all {TXT_KEY_POLICY_BRANCH_HONOR} policies are adopted[ENDCOLOR]).'
WHERE Tag = 'TXT_KEY_POLICY_WARRIOR_CODE_HELP';

UPDATE Language_en_US
SET Text = 'A tribute is wealth, often in kind, that one party gives to another as a sign of respect or, as was often the case in historical contexts, of submission or allegiance. Various ancient states exacted tribute from the rulers of land which the state conquered or otherwise threatened to conquer.'
WHERE Tag = 'TXT_KEY_POLICY_WARRIORCODE_TEXT';

UPDATE Language_en_US
SET Text = 'Imperium'
WHERE Tag = 'TXT_KEY_POLICY_DISCIPLINE';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Imperium[ENDCOLOR][NEWLINE][ICON_BULLET]Cities gain 20 [ICON_PRODUCTION] Production and [ICON_GOLD] Gold when their borders expand, scaling with Era ([COLOR_YELLOW]40 if all {TXT_KEY_POLICY_BRANCH_HONOR} policies are adopted[ENDCOLOR]).[NEWLINE][ICON_BULLET]Receive 40 [ICON_RESEARCH] Science and [ICON_CULTURE] Culture when you found or conquer Cities, scaling with Era. Conquest bonus also scales based on City [ICON_CITIZEN] population.'
WHERE Tag = 'TXT_KEY_POLICY_DISCIPLINE_HELP';

UPDATE Language_en_US
SET Text = 'Imperium is a Latin word which, in a broad sense, translates roughly as ''power to command.'' In ancient Rome, different kinds of power or authority were distinguished by different terms. Imperium referred to the sovereignty of the state over the individual.'
WHERE Tag = 'TXT_KEY_POLICY_DISCIPLINE_TEXT';

UPDATE Language_en_US
SET Text = 'Militarism'
WHERE Tag = 'TXT_KEY_POLICY_MILITARY_TRADITION';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Militarism[ENDCOLOR][NEWLINE][ICON_BULLET]+1 [ICON_HAPPINESS_1] Happiness and +2 [ICON_CULTURE] Culture in Cities with at least 10 [ICON_STRENGTH] Strength.[NEWLINE][ICON_BULLET]-15% [ICON_GOLD] Unit Maintenance.[NEWLINE][ICON_BULLET]-50% [ICON_GOLD] Route Maintenance.'
WHERE Tag = 'TXT_KEY_POLICY_MILITARY_TRADITION_HELP';

UPDATE Language_en_US
SET Text = 'Militarism is defined as "the belief or desire of a government or people that the nation maintain a strong military capability and be prepared to use it aggressively." Militarism has been a significant element of policy in most imperialistic or expansionist nations throughout history, from Ancient Assyria and Sparta to Nazi Germany. This philosophy has many components that include: glorification of the military and of the ideals of a professional military class, idealizing personal military accomplishments, and government policies that devote a significant portion of the nation''s resources to supporting and expanding its military forces.'
WHERE Tag = 'TXT_KEY_POLICY_MILITARYTRADITION_TEXT';

UPDATE Language_en_US
SET Text = 'Dominance'
WHERE Tag = 'TXT_KEY_POLICY_MILITARY_CASTE';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Dominance[ENDCOLOR][NEWLINE][ICON_BULLET]All Melee Units gain the [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_KILL_HEAL}[ENDCOLOR] Promotion (Heal 15 HP after defeating an Enemy Unit).[NEWLINE][ICON_BULLET]+1 [ICON_PRODUCTION] Production in every City for every 10 Military Units in Empire.[NEWLINE][ICON_BULLET]+10% [ICON_SILVER_FIST] Military Supply from [ICON_CITIZEN] Population.'
WHERE Tag = 'TXT_KEY_POLICY_MILITARY_CASTE_HELP';

UPDATE Language_en_US
SET Text = 'Power - or dominance - is the ability to influence behavior, and may not be fully assessable until it is challenged with equal force. Unlike power, which can be latent, dominance is a manifest condition characterized by individual, situational and relationship patterns in which attempts to control another party or parties may or may not be accepted.'
WHERE Tag = 'TXT_KEY_POLICY_MILITARYCASTE_TEXT';

UPDATE Language_en_US
SET Text = 'Honor'
WHERE Tag = 'TXT_KEY_POLICY_PROFESSIONAL_ARMY';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Honor[ENDCOLOR][NEWLINE][ICON_BULLET]All Military Units gain the [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_HONOR_BONUS}[ENDCOLOR] Promotion (+10% [ICON_STRENGTH] Combat Strength).[NEWLINE][ICON_BULLET]Supply-free military Units spawn near Cities that reach (or have already reached) a multiple of 10 [ICON_CITIZEN] Citizens for the first time.[NEWLINE][ICON_BULLET]-25% [ICON_WAR] War Weariness.'
WHERE Tag = 'TXT_KEY_POLICY_PROFESSIONAL_ARMY_HELP';

UPDATE Language_en_US
SET Text = 'In an "honorable" society, the citizens'' status is based upon the society''s judgment of their personal qualities. Qualities that are usually considered honorable include fealty, honesty, integrity, and courage. In many such societies men are expected to "defend their honor" to the death, as any loss of honor is considered far worse than loss of life.'
WHERE Tag = 'TXT_KEY_POLICY_PROFESSIONALARMY_TEXT';

--------------------
-- Fealty
--------------------

-- Leader title change
UPDATE Language_en_US
SET Text = '{@1: gender feminine?Lady; other?Lord;} {1_PlayerName:textkey} of {2_CivName:textkey}'
WHERE Tag = 'TXT_KEY_PIETY_TITLE';

UPDATE Language_en_US
SET Text = 'Fealty'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PIETY';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Fealty[ENDCOLOR] enhances your ability to defend and leverage your [ICON_RELIGION] Majority Religion.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting Fealty grants:[ENDCOLOR][NEWLINE][ICON_BULLET]Can purchase Monasteries with [ICON_PEACE] Faith (+3 [ICON_FOOD] Food, +3 [ICON_RESEARCH] Science, +2 [ICON_PEACE] Faith).[NEWLINE][ICON_BULLET]-25% [ICON_PEACE] Faith costs for purchasing Buildings, Missionaries, and Inquisitors.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Each Fealty policy unlocked grants:[ENDCOLOR][NEWLINE][ICON_BULLET]+1 [ICON_PEACE] Faith and [ICON_STRENGTH] Strength in every City.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting all Policies in Fealty grants:[ENDCOLOR][NEWLINE][ICON_BULLET]Unlocks building the [COLOR_POSITIVE_TEXT]Red Fort[ENDCOLOR].[NEWLINE][ICON_BULLET]Your [ICON_TOURISM] Tourism modifier for [COLOR_POSITIVE_TEXT]Shared Religion[ENDCOLOR] is 50% stronger.[NEWLINE][ICON_BULLET]Cities that follow your [ICON_RELIGION] Majority Religion generate +3 [ICON_PRODUCTION] Production, [ICON_GOLD] Gold, [ICON_RESEARCH] Science, and [ICON_CULTURE] Culture.[NEWLINE][ICON_BULLET]Allows for the purchase of [ICON_GREAT_ARTIST] Great Artists with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PIETY_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Organized Religion[ENDCOLOR][NEWLINE][ICON_BULLET]+25% Pressure from owned Cities following your [ICON_RELIGION] Primary Religion.[NEWLINE][ICON_BULLET]+1 [ICON_DIPLOMAT] Delegate in the World Congress for every 10 Cities following your [ICON_RELIGION] Primary Religion.[NEWLINE][ICON_BULLET]+2 [ICON_CULTURE] Culture from Shrines and Temples.'
WHERE Tag = 'TXT_KEY_POLICY_ORGANIZED_RELIGION_HELP';

UPDATE Language_en_US
SET Text = 'Nobility'
WHERE Tag = 'TXT_KEY_POLICY_MANDATE_OF_HEAVEN';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Nobility[ENDCOLOR][NEWLINE][ICON_BULLET]+1 [ICON_HAPPINESS_1] Happiness and +2 [ICON_GOLD] Gold from Castles.[NEWLINE][ICON_BULLET]+2 [ICON_GOLD] Gold from Armories.[NEWLINE][ICON_BULLET]+100% [ICON_PRODUCTION] Production towards Castles and Armories.'
WHERE Tag = 'TXT_KEY_POLICY_MANDATE_OF_HEAVEN_HELP';

UPDATE Language_en_US
SET Text = 'Nobility is a social class, normally ranked immediately under royalty, that possesses more acknowledged privileges and higher social status than most other classes in a society, membership thereof typically being hereditary. The privileges associated with nobility may constitute substantial advantages over or relative to non-nobles, or may be largely honorary (e.g., precedence), and vary from country to country and era to era. The Medieval chivalry motto noblesse oblige, which literally means ''nobility obligates'', explains that privileges carry a life-long obligation of duty to uphold social responsibilities, be it of honorable behavior, customary service or leadership, that lives on by a familial or kinship bond.'
WHERE Tag = 'TXT_KEY_POLICY_MANDATEOFHEAVEN_TEXT';

UPDATE Language_en_US
SET Text = 'Divine Right'
WHERE Tag = 'TXT_KEY_POLICY_THEOCRACY';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Divine Right[ENDCOLOR][NEWLINE][ICON_BULLET]+33% Yields from Internal [ICON_INTERNATIONAL_TRADE] Trade Routes.[NEWLINE][ICON_BULLET]Completing an Internal [ICON_INTERNATIONAL_TRADE] Trade Route triggers a Historic Event as if completing an International [ICON_INTERNATIONAL_TRADE] Trade Route.[NEWLINE][ICON_BULLET]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom in all Cities.'
WHERE Tag = 'TXT_KEY_POLICY_THEOCRACY_HELP';

UPDATE Language_en_US
SET Text = 'The divine right of kings, or divine-right theory of kingship, is a political and religious doctrine of royal and political legitimacy. It asserts that a monarch is subject to no earthly authority, deriving the right to rule directly from the will of God. The king is thus not subject to the will of his people, the aristocracy, or any other estate of the realm, including (in the view of some, especially in Protestant countries) the Church. According to this doctrine, only God can judge an unjust king. The doctrine implies that any attempt to depose the king or to restrict his powers runs contrary to the will of God and may constitute a sacrilegious act. It is often expressed in the phrase "by the Grace of God," attached to the titles of a reigning monarch.'
WHERE Tag = 'TXT_KEY_POLICY_THEOCRACY_TEXT';

UPDATE Language_en_US
SET Text = 'Fiefdoms'
WHERE Tag = 'TXT_KEY_POLICY_REFORMATION';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Fiefdoms[ENDCOLOR][NEWLINE][ICON_BULLET]+1 [ICON_HAPPINESS_1] Happiness for every 10 Military Units in Empire.[NEWLINE][ICON_BULLET]+15% [ICON_PRODUCTION] Production and +100% [ICON_CULTURE_LOCAL] Border Growth during "[COLOR_POSITIVE_TEXT]We Love the King Day[ENDCOLOR]."'
WHERE Tag = 'TXT_KEY_POLICY_REFORMATION_HELP';

UPDATE Language_en_US
SET Text = 'A fief is a heritable right to property or income source granted by a king to a lesser noble in return for vows of fealty. A fief usually took the form of revenue-producing lands which the noble could levy taxes from, in which case the estate was called a Fiefdom, but it could also take other forms, such as a trade monopoly or resource rights, like mining. These fiefs were central to the Medieval feudal system, and allowed kings to bind knights to their service without the need for the large bureaucracies and costs of a standing army; it was up to knights to sustain their own livelihoods once they had been given the means to do so.'
WHERE Tag = 'TXT_KEY_POLICY_REFORMATION_TEXT';

UPDATE Language_en_US
SET Text = 'Serfdom'
WHERE Tag = 'TXT_KEY_POLICY_FREE_RELIGION';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Serfdom[ENDCOLOR][NEWLINE][ICON_BULLET]+1 [ICON_CULTURE] Culture and [ICON_PEACE] Faith in Cities for every 4 non-Specialist [ICON_CITIZEN] Citizens.[NEWLINE][ICON_BULLET]+2 [ICON_PRODUCTION] Production and +1 [ICON_GOLD] Gold from Pastures.'
WHERE Tag = 'TXT_KEY_POLICY_FREE_RELIGION_HELP';

UPDATE Language_en_US
SET Text = 'Serfdom is the status of many peasants under feudalism, specifically relating to manorialism. It was a condition of bondage, which developed primarily during the High Middle Ages in Europe and lasted in some countries until the mid-19th century. Serfs who occupied a plot of land were required to work for the lord of the manor who owned that land. In return they were entitled to protection, justice, and the right to cultivate certain fields within the manor to maintain their own subsistence. Serfs were often required not only to work on the lord''s fields, but also in his mines and forests and to labor to maintain roads. The manor formed the basic unit of feudal society, and the lord of the manor and the villeins, and to a certain extent serfs, were bound legally: by taxation in the case of the former, and economically and socially in the latter.'
WHERE Tag = 'TXT_KEY_POLICY_FREERELIGION_TEXT';

--------------------
-- Statecraft
--------------------

-- Leader title change
UPDATE Language_en_US
SET Text = '{@1: gender feminine?Mistress; other?Master;} {1_PlayerName:textkey} of {2_CivName:textkey}'
WHERE Tag = 'TXT_KEY_PATRONAGE_TITLE';

UPDATE Language_en_US
SET Text = 'Statecraft'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PATRONAGE';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Statecraft[ENDCOLOR] improves your ability to benefit from [ICON_INTERNATIONAL_TRADE] Trade Routes, [ICON_CITY_STATE] City-States, [ICON_SPY] Spies, and [ICON_DIPLOMAT] the World Congress.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting Statecraft grants:[ENDCOLOR][NEWLINE][ICON_BULLET] +1 of every Yield in [ICON_CAPITAL] Capital for every 20 [ICON_CITIZEN] Citizens in your Empire.[NEWLINE][ICON_BULLET] +1 [ICON_GOLD] Gold in every City.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Each Statecraft policy unlocked grants:[ENDCOLOR][NEWLINE][ICON_BULLET] +1 [ICON_GOLD] Gold in every City, and +10% [ICON_INFLUENCE] Influence from {TXT_KEY_POP_CSTATE_GIFT_GOLD}.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting all policies in Statecraft grants:[ENDCOLOR][NEWLINE][ICON_BULLET] Unlocks building the [COLOR_POSITIVE_TEXT]Palace of Westminster[ENDCOLOR].[NEWLINE][ICON_BULLET] Every World Congress Session, gain 5 [ICON_CULTURE] Culture, [ICON_RESEARCH] Science, and [ICON_GOLD] Gold for each [ICON_DIPLOMAT] Delegate you control, scaling with Era.[NEWLINE][ICON_BULLET] World Wonders require 1 less unlocked Policy for every 3 City-State alliances you maintain.[NEWLINE] [ICON_BULLET] Allows for the purchase of [ICON_DIPLOMAT] Great Diplomats with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PATRONAGE_HELP';

UPDATE Language_en_US
SET Text = 'Statecraft is the domain of the wealthy and powerful, and is the capacity of a political entity to interact with other political entities. In many societies, statecraft is managed by a few individuals: the Royal Family, say, or the merchant princes. Today, statecraft is largely a democratic process, though some things remain the exclusive domain of those with enough influence.'
WHERE Tag = 'TXT_KEY_POLICY_PATRONAGE_TEXT';

UPDATE Language_en_US
SET Text = 'Foreign Service'
WHERE Tag = 'TXT_KEY_POLICY_PHILANTHROPY';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Foreign Service[ENDCOLOR][NEWLINE][ICON_BULLET]Receive 100 [ICON_VP_SPY_POINTS] Spy Points.[NEWLINE][ICON_BULLET]Earn [ICON_DIPLOMAT] Great Diplomats 50% faster.[NEWLINE][ICON_BULLET]+50% Rewards from [ICON_CITY_STATE] City-State Quests.[NEWLINE][ICON_BULLET]+1 of every Strategic Resource for every three City-State Alliances you maintain.'
WHERE Tag = 'TXT_KEY_POLICY_PHILANTHROPY_HELP';

UPDATE Language_en_US
SET Text = 'Diplomatic (or foreign) service is the body of diplomats and foreign policy officers maintained by the government of a country to communicate with the governments of other countries. Diplomatic personnel enjoy diplomatic immunity when they are accredited to other countries. Diplomatic services are often part of the larger civil service and sometimes a constituent part of the foreign ministry.'
WHERE Tag = 'TXT_KEY_POLICY_PHILANTHROPY_TEXT';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Consulates[ENDCOLOR][NEWLINE][ICON_BULLET]+1 [ICON_DIPLOMAT] Delegate in the World Congress for every 8 [ICON_CITY_STATE] City-States originally in the World.[NEWLINE][ICON_BULLET]Completing a [ICON_INTERNATIONAL_TRADE] Trade Route to a City-State triggers (or strengthens an existing) [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_POLICY_CONSULATES_HELP';

UPDATE Language_en_US
SET Text = 'Shadow Networks'
WHERE Tag = 'TXT_KEY_POLICY_SCHOLASTICISM';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Shadow Networks[ENDCOLOR][NEWLINE][ICON_BULLET]Receive 25% of [ICON_RESEARCH] Science generated by [COLOR_POSITIVE_TEXT]Allied[ENDCOLOR] City-States.[NEWLINE][ICON_BULLET]+3% [ICON_CULTURE] Culture in [ICON_CAPITAL] Capital for every 100 [ICON_VP_SPY_POINTS] Spy Points ever accumulated (up to 30%).[NEWLINE][ICON_BULLET]+3 [ICON_RESEARCH] Science from Constabularies and Police Stations.[NEWLINE][ICON_BULLET]+1 [ICON_RESEARCH] Science from Specialists.'
WHERE Tag = 'TXT_KEY_POLICY_SCHOLASTICISM_HELP';

UPDATE Language_en_US
SET Text = 'A primary purpose of intelligence organizations is to penetrate a target with a human agent, or a network of human agents. Such agents can either infiltrate the target, or be recruited ''in place''. Case officers are professionally trained employees of intelligence organizations that manage human agents and human agent networks. Intelligence that derives from such human sources is known as HUMINT. Sometimes, agent handling is done indirectly, through ''principal agents'' that serve as proxies for case officers. It is not uncommon, for example, for a case officer to manage a number of principal agents, who in turn handle agent networks, which are preferably organized in a cellular fashion. In such a case, the principal agent can serve as a ''cut-out'' for the case officer, buffering him or her from direct contact with the agent network.'
WHERE Tag = 'TXT_KEY_POLICY_SCHOLASTICISM_TEXT';

UPDATE Language_en_US
SET Text = 'Exchange Markets'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_DIPLOMACY';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Exchange Markets[ENDCOLOR][NEWLINE][ICON_BULLET]+1 [ICON_INTERNATIONAL_TRADE] Trade Route.[NEWLINE][ICON_BULLET]+1 [ICON_HAPPINESS_1] Happiness for every active [ICON_INTERNATIONAL_TRADE] Trade Route.[NEWLINE][ICON_BULLET]+15% [ICON_TOURISM] Tourism modifier for [COLOR_POSITIVE_TEXT]Trade Routes[ENDCOLOR].[NEWLINE][ICON_BULLET]Resources from City-States count towards Global Monopolies.'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_DIPLOMACY_HELP';

UPDATE Language_en_US
SET Text = 'A commodity exchange market is a market that trades in primary economic sector rather than manufactured products. Soft commodities are agricultural products such as wheat, coffee, cocoa, fruit and sugar. Hard commodities are mined, such as gold and oil. Investors access about 50 major commodity markets worldwide with purely financial transactions increasingly outnumbering physical trades in which goods are delivered. Futures contracts are the oldest way of investing in commodities. Futures are secured by physical assets. Commodity markets can include physical trading and derivatives trading using spot prices, forwards, futures, and options on futures. Farmers have used a simple form of derivative trading in the commodity market for centuries for price risk management.'
WHERE Tag = 'TXT_KEY_POLICY_CULTURALDIPLOMACY_TEXT';

UPDATE Language_en_US
SET Text = 'Trade Confederacy'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_CONFEDERACY';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Trade Confederacy[ENDCOLOR][NEWLINE][ICON_BULLET]+25% Yields for International [ICON_INTERNATIONAL_TRADE] Trade Routes.[NEWLINE][ICON_BULLET][ICON_INTERNATIONAL_TRADE] Trade Routes to City-States generate +1 [ICON_INFLUENCE] Influence per turn (with the target City-State) per each owned City-State Trade Route (up to +5).'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_CONFEDERACY_HELP';

UPDATE Language_en_US
SET Text = 'A trade confederacy, also known as an industry trade group, business association, sector association or industry body, is an organization founded and funded by businesses that operate in a specific industry. An industry trade association participates in public relations activities such as advertising, education, political donations, lobbying and publishing, but its focus is collaboration between companies. Associations may offer other services, such as producing conferences, networking or charitable events or offering classes or educational materials. Many associations are non-profit organizations governed by bylaws and directed by officers who are also members.'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_CONFEDERACY_TEXT';

--------------------
-- Artistry
--------------------

-- Leader title change
UPDATE Language_en_US
SET Text = '{@1: gender feminine?Patroness; other?Patron;} {1_PlayerName:textkey} of {2_CivName:textkey}'
WHERE Tag = 'TXT_KEY_AESTHETICS_TITLE';

UPDATE Language_en_US
SET Text = 'Artistry'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_AESTHETICS';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Artistry[ENDCOLOR] allows you to maximize the potential of [ICON_GREAT_WORK] Great Works and [ICON_GOLDEN_AGE] Golden Ages.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting Artistry grants:[ENDCOLOR][NEWLINE][ICON_BULLET]Earn Great [ICON_GREAT_WRITER] Writers, [ICON_GREAT_ARTIST] Artists, and [ICON_GREAT_MUSICIAN] Musicians 25% faster.[NEWLINE][ICON_BULLET]+10% [ICON_CULTURE] Culture during [ICON_GOLDEN_AGE] Golden Ages.[NEWLINE][ICON_BULLET]+100% [ICON_PRODUCTION] Production towards all Guilds.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Each Artistry policy unlocked grants:[ENDCOLOR][NEWLINE][ICON_BULLET]+1 [ICON_RESEARCH] Science in every City.[NEWLINE][ICON_BULLET]20% of Excess [ICON_HAPPINESS_1] Happiness produced in each City is added as progress toward a [ICON_GOLDEN_AGE] Golden Age.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting all policies in Artistry grants:[ENDCOLOR][NEWLINE][ICON_BULLET]Unlocks building the [COLOR_POSITIVE_TEXT]Louvre[ENDCOLOR].[NEWLINE][ICON_BULLET]Completing an [ICON_RES_ARTIFACTS] Archaeological Dig or starting a [ICON_GOLDEN_AGE] Golden Age triggers (or strengthens an existing) [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR].[NEWLINE][ICON_BULLET]Allows you to see [ICON_RES_HIDDEN_ARTIFACTS] Hidden Antiquity Sites.[NEWLINE][ICON_BULLET]+3 [ICON_RESEARCH] Science from [ICON_RES_ARTIFACTS] Landmarks.[NEWLINE][ICON_BULLET]Allows for the purchase of [ICON_GREAT_MUSICIAN] Great Musicians with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_AESTHETICS_HELP';

UPDATE Language_en_US
SET Text = 'Art is a diverse range of human activities in creating visual, auditory or performing artifacts (artworks), expressing the author''s imaginative or technical skill, intended to be appreciated for their beauty or emotional power. In their most general form these activities include the production of works of art, the criticism of art, the study of the history of art, and the aesthetic dissemination of art.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_AESTHETICS_TEXT';

UPDATE Language_en_US
SET Text = 'Artistry'
WHERE Tag = 'TXT_KEY_POLICY_AESTHETICS';

UPDATE Language_en_US
SET Text = 'Humanism'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_CENTERS';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Humanism[ENDCOLOR][NEWLINE][ICON_BULLET]+3 [ICON_PEACE] Faith from [ICON_VP_GREATWRITING] Great Works of Writing.[NEWLINE][ICON_BULLET]-25% [ICON_GOLDEN_AGE] Golden Age Points needed to trigger a [ICON_GOLDEN_AGE] Golden Age.[NEWLINE][ICON_BULLET]+1 [ICON_HAPPINESS_1] Happiness from all Guilds.'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_CENTERS_HELP';

UPDATE Language_en_US
SET Text = 'Humanism is a philosophical and ethical stance that emphasizes the value and agency of human beings, individually and collectively, and generally prefers critical thinking and evidence (rationalism, empiricism) over established doctrine or faith (fideism). The meaning of the term humanism has fluctuated, according to the successive intellectual movements which have identified with it. Generally, however, humanism refers to a perspective that affirms some notion of a "human nature" (sometimes contrasted with antihumanism).'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_CENTERS_TEXT';

UPDATE Language_en_US
SET Text = 'Refinement'
WHERE Tag = 'TXT_KEY_POLICY_FINE_ARTS';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Refinement[ENDCOLOR][NEWLINE][ICON_BULLET]+2 [ICON_CULTURE] Culture from [ICON_VP_GREATART] Great Works of Art.[NEWLINE][ICON_BULLET]1 Specialist in all cities does not produce [ICON_HAPPINESS_3] Unhappiness from Urbanization.[NEWLINE][ICON_BULLET]+1 [ICON_CULTURE] Culture from Specialists.'
WHERE Tag = 'TXT_KEY_POLICY_FINE_ARTS_HELP';

UPDATE Language_en_US
SET Text = 'Sophistication is the quality of refinement — displaying good taste, wisdom and subtlety rather than crudeness, stupidity and vulgarity. In the perception of social class, sophistication can link with concepts such as status, privilege and superiority.'
WHERE Tag = 'TXT_KEY_POLICY_FINE_ARTS_TEXT';

UPDATE Language_en_US
SET Text = 'Heritage'
WHERE Tag = 'TXT_KEY_POLICY_FLOURISHING_OF_ARTS';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Heritage[ENDCOLOR][NEWLINE][ICON_BULLET]+4 [ICON_GOLD] Gold from [ICON_VP_GREATMUSIC] Great Works of Music.[NEWLINE][ICON_BULLET]+50% to all [ICON_TOURISM]/[ICON_CULTURE] Theming Bonuses on Empire[NEWLINE][ICON_BULLET]25% of the [ICON_CULTURE] Culture from [ICON_WONDER] World Wonders and Tiles is added to the [ICON_TOURISM] Tourism output of the city.'
WHERE Tag = 'TXT_KEY_POLICY_FLOURISHING_OF_ARTS_HELP';

UPDATE Language_en_US
SET Text = 'Cultural heritage is the legacy of physical artifacts and intangible attributes of a group or society that are inherited from past generations, maintained in the present and bestowed for the benefit of future generations. Cultural heritage includes tangible culture (such as buildings, monuments, landscapes, books, works of art, and artifacts), intangible culture (such as folklore, traditions, language, and knowledge), and natural heritage (including culturally significant landscapes, and biodiversity).'
WHERE Tag = 'TXT_KEY_POLICY_FLOURISHING_OF_ARTS_TEXT';

UPDATE Language_en_US
SET Text = 'National Treasure'
WHERE Tag = 'TXT_KEY_POLICY_ARTISTIC_GENIUS';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]National Treasure[ENDCOLOR][NEWLINE][ICON_BULLET]+2 [ICON_RESEARCH] Science from [ICON_VP_ARTIFACT] Artifacts.[NEWLINE][ICON_BULLET]A [ICON_GREAT_PEOPLE] Great Person of your choice appears near your [ICON_CAPITAL] Capital.[NEWLINE][ICON_BULLET]Gain 250 [ICON_GOLD] Gold when you construct [ICON_WONDER] World Wonders, scaling with Era.'
WHERE Tag = 'TXT_KEY_POLICY_ARTISTIC_GENIUS_HELP';

UPDATE Language_en_US
SET Text = 'The idea of national treasure, like national epics and national anthems, is part of the language of Romantic nationalism, which arose in the late 18th century and 19th centuries. Nationalism is an ideology which supports the nation as the fundamental unit of human social life, which includes shared language, values and culture. Thus national treasure, part of the ideology of nationalism, is shared culture.'
WHERE Tag = 'TXT_KEY_POLICY_ARTISTIC_GENIUS_TEXT';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Cultural Exchange[ENDCOLOR][NEWLINE][ICON_BULLET]+1 [ICON_HAPPINESS_1] Happiness for every 3 [ICON_GREAT_WORK] Great Works in a City.[NEWLINE][ICON_BULLET]+2 [ICON_PRODUCTION] Production and [ICON_CULTURE] Culture from Amphitheaters, Galleries, and Opera Houses.[NEWLINE][ICON_BULLET]+10% [ICON_TOURISM] Tourism modifier for [COLOR_POSITIVE_TEXT]Open Borders[ENDCOLOR] with other Civilizations.'
WHERE Tag = 'TXT_KEY_POLICY_ETHICS_HELP';

--------------------
-- Industry
--------------------

-- Leader title change
UPDATE Language_en_US
SET Text = 'Prime Minister {1_PlayerName:textkey} of {2_CivName:textkey}'
WHERE Tag = 'TXT_KEY_COMMERCE_TITLE';

UPDATE Language_en_US
SET Text = 'Industry'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_COMMERCE';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Industry[ENDCOLOR] provides bonuses to empires focused on [ICON_GOLD] Gold and [ICON_PRODUCTION] Production.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting Industry grants:[ENDCOLOR][NEWLINE][ICON_BULLET]+2 [ICON_INTERNATIONAL_TRADE] Trade Routes.[NEWLINE][ICON_BULLET]-5% [ICON_INVEST] Gold needed for purchases.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Each Industry policy unlocked grants:[ENDCOLOR][NEWLINE][ICON_BULLET]-5% [ICON_INVEST] Gold needed for purchases.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting all Policies in Industry grants:[ENDCOLOR][NEWLINE][ICON_BULLET]Unlocks building [COLOR_POSITIVE_TEXT]Broadway[ENDCOLOR].[NEWLINE][ICON_BULLET]+3 [ICON_HAPPINESS_1] Happiness per unique owned Luxury Resource.[NEWLINE][ICON_BULLET]+2 [ICON_PRODUCTION] Production and [ICON_GOLD] Gold from Specialists.[NEWLINE][ICON_BULLET]Allows for the purchase of [ICON_GREAT_MERCHANT] Great Merchants with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_COMMERCE_HELP';

UPDATE Language_en_US
SET Text = 'Industry is the production of a good or service within an economy. Manufacturing industry became a key sector of production and labour in European and North American countries during the Industrial Revolution, upsetting previous mercantile and feudal economies. This occurred through many successive rapid advances in technology, such as the production of steel and coal. Following the Industrial Revolution, perhaps a third of world economic output is derived from manufacturing industries. Many developed countries and many developing/semi-developed countries (China, India etc.) depend significantly on manufacturing industry. Industries, the countries they reside in, and the economies of those countries are interlinked in a complex web of interdependence.'
WHERE Tag = 'TXT_KEY_POLICY_COMMERCE_TEXT';

UPDATE Language_en_US
SET Text = 'Division of Labor'
WHERE Tag = 'TXT_KEY_POLICY_TRADE_UNIONS';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Division of Labor[ENDCOLOR][NEWLINE][ICON_BULLET]+3% [ICON_PRODUCTION] Production and [ICON_GOLD] Gold from Forges, Windmills, Workshops, Factories, and Coaling Stations.[NEWLINE][ICON_BULLET]+100% [ICON_PRODUCTION] Production towards Coaling Stations.'
WHERE Tag = 'TXT_KEY_POLICY_TRADE_UNIONS_HELP';

UPDATE Language_en_US
SET Text = 'The division of labor is the specialization of cooperating individuals who perform specific tasks and roles. Because of the large amount of labor saved by giving workers specialized tasks in Industrial Revolution-era factories, some classical economists as well as some mechanical engineers such as Charles Babbage were proponents of division of labor. Also, having workers perform single or limited tasks eliminated the long training period required to train craftsmen, who were replaced with lesser paid but more productive unskilled workers. Historically, an increasingly complex division of labor is associated with the growth of total output and trade, the rise of capitalism, and of the complexity of industrialised processes.'
WHERE Tag = 'TXT_KEY_POLICY_TRADEUNIONS_TEXT';

UPDATE Language_en_US
SET Text = 'Free Trade'
WHERE Tag = 'TXT_KEY_POLICY_CARAVANS';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Free Trade[ENDCOLOR][NEWLINE][ICON_BULLET]Earn [ICON_GREAT_MERCHANT] Great Merchants 50% faster.[NEWLINE][ICON_BULLET]+5 [ICON_GOLD] Gold from [ICON_INTERNATIONAL_TRADE] International Trade Routes.[NEWLINE][ICON_BULLET]-2 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty in all Cities.'
WHERE Tag = 'TXT_KEY_POLICY_CARAVANS_HELP';

UPDATE Language_en_US
SET Text = 'Free trade is a policy followed by some international markets in which governments do not restrict imports from or exports to other countries. Free trade is exemplified by the European Economic Area and the North American Free Trade Agreement, which have established open markets. Most nations are today members of the World Trade Organization (WTO) multilateral trade agreements. However, most governments still impose some protectionist policies that are intended to support local employment, such as applying tariffs to imports or subsidies to exports. Governments may also restrict free trade to limit exports of natural resources. Other barriers that may hinder trade include import quotas, taxes, and non-tariff barriers, such as regulatory legislation.'
WHERE Tag = 'TXT_KEY_POLICY_CARAVANS_TEXT';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Mercantilism[ENDCOLOR][NEWLINE][ICON_BULLET]+3% [ICON_RESEARCH] Science and [ICON_CULTURE] Culture from Markets, Caravansaries, Customs Houses, Banks, and Stock Exchanges.[NEWLINE][ICON_BULLET]Cities earn +10 [ICON_RESEARCH] Science when they construct Buildings, scaling with Era. '
WHERE Tag = 'TXT_KEY_POLICY_MERCANTILISM_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Entrepreneurship[ENDCOLOR][NEWLINE][ICON_BULLET]+1 [ICON_HAPPINESS_1] Happiness from Workshops.[NEWLINE][ICON_BULLET]+2 [ICON_PRODUCTION] Production and +1 [ICON_GOLD] Gold from every Mine, Quarry, and Lumber Mill.[NEWLINE][ICON_BULLET]+25% Yields when you expend [ICON_GREAT_MERCHANT] Great Merchants or [ICON_GREAT_ENGINEER] Great Engineers for their Instant Yield abilities.'
WHERE Tag = 'TXT_KEY_POLICY_ENTREPRENEURSHIP_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Protectionism[ENDCOLOR][NEWLINE][ICON_BULLET][ICON_INVEST] Gold investments in Buildings reduce their [ICON_PRODUCTION] Production cost by an additional 10%.[NEWLINE][ICON_BULLET]+33% Yields from Internal [ICON_INTERNATIONAL_TRADE] Trade Routes.[NEWLINE][ICON_BULLET]+10% [ICON_FOOD] Food and [ICON_RESEARCH] Science during "[COLOR_POSITIVE_TEXT]We Love the King Day[ENDCOLOR]."'
WHERE Tag = 'TXT_KEY_POLICY_PROTECTIONISM_HELP';

--------------------
-- Imperialism
--------------------

-- Leader title change
UPDATE Language_en_US
SET Text = '{@1: gender feminine?Empress; other?Emperor;} {1_PlayerName:textkey} of {2_CivName:textkey}'
WHERE Tag = 'TXT_KEY_EXPLORATION_TITLE';

UPDATE Language_en_US
SET Text = 'Imperialism'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_EXPLORATION';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Imperialism[ENDCOLOR] enhances your ability to spread your empire through military power, particularly naval (and later air) supremacy.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting Imperialism grants:[ENDCOLOR][NEWLINE][ICON_BULLET]+1 [ICON_MOVES] Movement for Naval Units, Embarked Units, and [ICON_GREAT_GENERAL] Great Generals, as well as +1 Sight for Melee, Recon, Naval Melee and Gunpowder Units.[NEWLINE][ICON_BULLET]+10% [ICON_PRODUCTION] Production toward Military Units, -10% [ICON_GOLD] Gold required for upgrades.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Each Imperialism policy unlocked grants:[ENDCOLOR][NEWLINE][ICON_BULLET]+5% [ICON_PRODUCTION] Production for Military Units, -5% [ICON_GOLD] Gold required for upgrades.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting all policies in Imperialism grants:[ENDCOLOR][NEWLINE][ICON_BULLET]Unlocks building the [COLOR_POSITIVE_TEXT]Pentagon[ENDCOLOR].[NEWLINE][ICON_BULLET]All Naval and Air units gain the [COLOR_POSITIVE_TEXT]Banzai![ENDCOLOR] Promotion (become stronger as they take damage).[NEWLINE][ICON_BULLET]Allows for the purchase of [ICON_GREAT_ADMIRAL] Great Admirals with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_EXPLORATION_HELP';

UPDATE Language_en_US
SET Text = 'Martial Law'
WHERE Tag = 'TXT_KEY_POLICY_MARITIME_INFRASTRUCTURE';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Martial Law[ENDCOLOR][NEWLINE][ICON_BULLET]No [ICON_GOLD] Gold Maintenance from Garrisons.[NEWLINE][ICON_BULLET]+1 [ICON_HAPPINESS_1] Happiness and +4 [ICON_CULTURE] Culture for each Garrison.[NEWLINE][ICON_BULLET]-20% Yield penalties of [ICON_PUPPET] Puppeted Cities.'
WHERE Tag = 'TXT_KEY_POLICY_MARITIME_INFRASTRUCTURE_HELP';

UPDATE Language_en_US
SET Text = 'Martial law is the imposition of the highest-ranking military officer as the military governor or as the head of the government, thus removing all power from the previous executive, legislative, and judicial branches of government. It is usually imposed temporarily when the government or civilian authorities fail to function effectively (e.g., maintain order and security, or provide essential services).'
WHERE Tag = 'TXT_KEY_POLICY_MARITIME_INFRASTRUCTURE_TEXT';

UPDATE Language_en_US
SET Text = 'Exploitation'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_NAVY';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Exploitation[ENDCOLOR][NEWLINE][ICON_BULLET]+2 [ICON_FOOD] Food and +1 [ICON_PRODUCTION] Production from Farms, Camps, and Plantations.[NEWLINE][ICON_BULLET]+1 [ICON_PRODUCTION] Production and [ICON_RESEARCH] Science from Coasts, Lakes, and Oceans.'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_NAVY_HELP';

UPDATE Language_en_US
SET Text = 'Exploitation is the use of someone or something in an unjust or cruel manner, or generally as a means to an end. Most often, the word exploitation is used to refer to economic exploitation; that is, the act of using another person as a means to profit, particularly using their labor without offering or providing them fair compensation.'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANTNAVY_TEXT';

UPDATE Language_en_US
SET Text = 'Regimental System'
WHERE Tag = 'TXT_KEY_POLICY_NAVIGATION_SCHOOL';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Regimental System[ENDCOLOR][NEWLINE][ICON_BULLET]Earn [ICON_GREAT_GENERAL] Great Generals and [ICON_GREAT_ADMIRAL] Great Admirals 33% faster.[NEWLINE][ICON_BULLET][ICON_GREAT_GENERAL] Great Generals and [ICON_GREAT_ADMIRAL] Great Admirals gain the [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_BETTER_LEADERSHIP}[ENDCOLOR] Promotion (+10% [COLOR_POSITIVE_TEXT]Leadership[ENDCOLOR] Combat Bonus and +1 Tile Radius).[NEWLINE][ICON_BULLET]+2 additional copies of the unique Luxury Resources from expending [ICON_GREAT_ADMIRAL] Great Admirals for their Voyage of Discovery ability.[NEWLINE][ICON_BULLET]Military Units can be upgraded in territory owned by Vassals and friendly [ICON_CITY_STATE] City-States.'
WHERE Tag = 'TXT_KEY_POLICY_NAVIGATION_SCHOOL_HELP';

UPDATE Language_en_US
SET Text = 'In a regimental system, each regiment is responsible for recruiting, training, and administration; each regiment is permanently maintained and therefore the regiment will develop its unique esprit de corps because of its unitary history, traditions, recruitment, and function. Usually, the regiment is responsible for recruiting and administering all of a soldier''s military career. Depending upon the country, regiments can be either combat units or administrative units or both.'
WHERE Tag = 'TXT_KEY_POLICY_NAVIGATION_SCHOOL_TEXT';

UPDATE Language_en_US
SET Text = 'Colonialism'
WHERE Tag = 'TXT_KEY_POLICY_NAVAL_TRADITION';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Colonialism[ENDCOLOR][NEWLINE][ICON_BULLET]+2 [ICON_RESEARCH] Science and +1 [ICON_CULTURE] Culture from Barracks, Armories, Military Academies, Forts, and Citadels.[NEWLINE][ICON_BULLET]Each unique [ICON_VP_MONOPOLY] Global Monopoly modifier is increased by an additional 10% if it''s percentage-based, or +3 otherwise.'
WHERE Tag = 'TXT_KEY_POLICY_NAVAL_TRADITION_HELP';

UPDATE Language_en_US
SET Text = 'Colonialism is the establishment of a colony in one territory by a political power from another territory, and the subsequent maintenance, expansion, and exploitation of that colony. The term is also used to describe a set of unequal relationships between the colonial power and the colony and often between the colonists and the indigenous peoples.'
WHERE Tag = 'TXT_KEY_POLICY_NAVALTRADITION_TEXT';

UPDATE Language_en_US
SET Text = 'Civilizing Mission'
WHERE Tag = 'TXT_KEY_POLICY_TREASURE_FLEETS';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Civilizing Mission[ENDCOLOR][NEWLINE][ICON_BULLET]Retain all buildings from conquered Cities.[NEWLINE][ICON_BULLET]Receive 75 [ICON_GOLD] Gold when you conquer a city, scaling with Era and City [ICON_CITIZEN] Population.[NEWLINE][ICON_BULLET]+10% [ICON_PRODUCTION] Production toward Buildings, with an additional +10% [ICON_PRODUCTION] Production per Era difference between your current Era and the building''s Era in all [ICON_PUPPET] Puppeted Cities and Cities with a Courthouse.'
WHERE Tag = 'TXT_KEY_POLICY_TREASURE_FLEETS_HELP';

UPDATE Language_en_US
SET Text = 'The mission civilisatrice (the French for "civilizing mission") is a rationale for intervention or colonization, proposing to contribute to the spread of civilization, mostly amounting to the Westernization of indigenous peoples. It was notably the underlying principle of French and Portuguese colonial rule in the late 19th and early 20th centuries. It was influential in the French colonies of Algeria, French West Africa, and Indochina, and in the Portuguese colonies of Angola, Guinea, Mozambique and Timor. The European colonial powers felt it was their duty to bring Western civilization to what they perceived as backward peoples. Rather than merely govern colonial peoples, the Europeans would attempt to Westernize them in accordance with a colonial ideology known as "assimilation".'
WHERE Tag = 'TXT_KEY_POLICY_TREASURE_FLEETS_TEXT';

--------------------
-- Rationalism
--------------------

-- Leader title change
UPDATE Language_en_US
SET Text = 'Chancellor {1_PlayerName:textkey} of {2_CivName:textkey}'
WHERE Tag = 'TXT_KEY_RATIONALISM_TITLE';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Rationalism[ENDCOLOR] focuses on maximizing [ICON_FOOD] Growth and [ICON_RESEARCH] Science output.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting Rationalism grants:[ENDCOLOR][NEWLINE][ICON_BULLET]+2 [ICON_PRODUCTION] Production and +3 [ICON_RESEARCH] Science from all Strategic Resources.[NEWLINE][ICON_BULLET]+5% [ICON_RESEARCH] Science in all Cities.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Each Rationalism policy unlocked grants:[ENDCOLOR][NEWLINE][ICON_BULLET]+2% [ICON_RESEARCH] Science.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Adopting all Policies in Rationalism grants:[ENDCOLOR][NEWLINE][ICON_BULLET]Unlocks building [COLOR_POSITIVE_TEXT]Bletchley Park[ENDCOLOR].[NEWLINE][ICON_BULLET]-1 [ICON_HAPPINESS_3] Unhappiness from all Needs in all Cities.[NEWLINE][ICON_BULLET]+12 City Security in every City.[NEWLINE][ICON_BULLET]Allows for the purchase of [ICON_GREAT_SCIENTIST] Great Scientists with [ICON_PEACE] Faith starting in the Industrial Era.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_RATIONALISM_HELP';

UPDATE Language_en_US
SET Text = 'Enlightenment'
WHERE Tag = 'TXT_KEY_POLICY_HUMANISM';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Enlightenment[ENDCOLOR][NEWLINE][ICON_BULLET]Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Technology.[NEWLINE][ICON_BULLET]+1 [ICON_HAPPINESS_1] Happiness from Universities.'
WHERE Tag = 'TXT_KEY_POLICY_HUMANISM_HELP';

UPDATE Language_en_US
SET Text = 'The Age of Enlightenment (or simply the Enlightenment or Age of Reason) is the time period in which cultural and social changes occurred emphasizing reason, analysis and individualism rather than traditional ways of thinking. Initiated by philosophes beginning in late 17th-century Western Europe the process of change fueled from voluntary organizations of men who were committed to the betterment of society. These men convened at coffeehouses, salons and masonic lodges. Institutions that were deeply rooted in society, such as religion and the government began to be questioned and a greater emphasis was placed on ways to reform society with toleration, science and skepticism.'
WHERE Tag = 'TXT_KEY_POLICY_HUMANISM_TEXT';

UPDATE Language_en_US
SET Text = 'Empiricism'
WHERE Tag = 'TXT_KEY_POLICY_SCIENTIFIC_REVOLUTION';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Empiricism[ENDCOLOR][NEWLINE][ICON_BULLET]+1 [ICON_FOOD] Food for every [ICON_CITIZEN] Citizen and +25% [ICON_FOOD] Growth in all Cities.[NEWLINE][ICON_BULLET]+3% [ICON_RESEARCH] Science in a City for every [ICON_GREAT_WORK] Great Work present (up to 20%).'
WHERE Tag = 'TXT_KEY_POLICY_SCIENTIFIC_REVOLUTION_HELP';

UPDATE Language_en_US
SET Text = 'Empiricism is a theory which states that knowledge comes only or primarily from sensory experience. One of several views of epistemology, the study of human knowledge, along with rationalism and skepticism, empiricism emphasizes the role of experience and evidence, especially sensory experience, in the formation of ideas, over the notion of innate ideas or traditions; empiricists may argue however that traditions (or customs) arise due to relations of previous sense experiences.'
WHERE Tag = 'TXT_KEY_POLICY_SCIENTIFICREVOLUTION_TEXT';

UPDATE Language_en_US
SET Text = 'Scientific Revolution'
WHERE Tag = 'TXT_KEY_POLICY_SECULARISM';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Scientific Revolution[ENDCOLOR][NEWLINE][ICON_BULLET]Can construct [COLOR_POSITIVE_TEXT]Observatories[ENDCOLOR] (+6 [ICON_RESEARCH] Science, +1 [ICON_RESEARCH] Science from nearby Mountains, 2 [ICON_VP_SCIENTIST] Scientist Slot).[NEWLINE][ICON_BULLET]+2 [ICON_RESEARCH] Science from Jungle and Snow.'
WHERE Tag = 'TXT_KEY_POLICY_SECULARISM_HELP';

UPDATE Language_en_US
SET Text = 'A scientific revolution is a period when rapid advances in human knowledge or technology overturns the current worldview, as a result triggering yet more advancements in thought and knowledge. Much of Europe underwent a scientific revolution in the 16th century, following the publication of Nicolaus Copernicus'' work, "On the Revolutions of the Heavenly Spheres" and Andreas Vesalius'' "On the Fabric of the Human Body In Seven Books." Both used modern (for the period) scientific practices to examine parts of the world around them and overturned incorrect scientific theories, some dating back to the ancient Greeks. Following the success of these books, the floodgates were opened, and scientists began carefully examining everything around them, and human knowledge increased exponentially over the succeeding centuries.'
WHERE Tag = 'TXT_KEY_POLICY_SECULARISM_TEXT';

UPDATE Language_en_US
SET Text = 'Rights of Man'
WHERE Tag = 'TXT_KEY_POLICY_SOVEREIGNTY';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Rights of Man[ENDCOLOR][NEWLINE][ICON_BULLET]-1 [ICON_HAPPINESS_3] Unhappiness from all Needs in all Cities.[NEWLINE][ICON_BULLET]+2 [ICON_PRODUCTION] Production and [ICON_GOLD] Gold from Villages.[NEWLINE][ICON_BULLET]+10% [ICON_RESEARCH] Science during [ICON_GOLDEN_AGE] Golden Ages.'
WHERE Tag = 'TXT_KEY_POLICY_SOVEREIGNTY_HELP';

UPDATE Language_en_US
SET Text = 'The Declaration was drafted by General Lafayette, Thomas Jefferson, and Honoré Mirabeau. Influenced by the doctrine of ''natural right'', the rights of man are held to be universal: valid at all times and in every place, pertaining to human nature itself. It became the basis for a nation of free individuals protected equally by the law. It is included in the beginning of the constitutions of both the Fourth French Republic (1946) and Fifth Republic (1958) and is still current. Inspired by the Enlightenment philosophers, the Declaration was a core statement of the values of the French Revolution and had a major impact on the development of freedom and democracy in Europe and worldwide.'
WHERE Tag = 'TXT_KEY_POLICY_SOVEREIGNTY_TEXT';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Free Thought[ENDCOLOR][NEWLINE][ICON_BULLET]-5 [ICON_HAPPINESS_3] Unhappiness from Religious Unrest in all Cities.[NEWLINE][ICON_BULLET]+25% Instant Yields from [ICON_GREAT_SCIENTIST] Great Scientists.[NEWLINE][ICON_BULLET]Earn [ICON_GREAT_SCIENTIST] Great Scientists +33% faster.'
WHERE Tag = 'TXT_KEY_POLICY_FREE_THOUGHT_HELP';

--------------------
-- Freedom
--------------------

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Arsenal of Democracy[ENDCOLOR]: +15% [ICON_PRODUCTION] Production towards Military Units. +10 [ICON_INFLUENCE] Influence with all known City-States when you expend [ICON_GREAT_PEOPLE] Great People, and +40 [ICON_INFLUENCE] Influence when you gift Units to City-States.'
WHERE Tag = 'TXT_KEY_POLICY_ARSENAL_DEMOCRACY_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Capitalism[ENDCOLOR]: 2 [ICON_CITIZEN] Specialists in each of your cities generate +1 [ICON_HAPPINESS_1] Happiness instead of -1 [ICON_HAPPINESS_3] Unhappiness from Urbanization. Specialists generate +1 [ICON_GOLD] Gold and [ICON_RESEARCH] Science.'
WHERE Tag = 'TXT_KEY_POLICY_CAPITALISM_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Avant Garde[ENDCOLOR]: The rate at which [ICON_GREAT_PEOPLE] Great People are born is increased by 33%. -2 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom in all Cities.'
WHERE Tag = 'TXT_KEY_POLICY_OPEN_SOCIETY_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Civil Society[ENDCOLOR]: Specialists consume 2 [ICON_FOOD] Food less than normal (minimum 1 [ICON_FOOD] Food). Farms, Plantations, Camps, and all Unique Improvements produce +4 [ICON_FOOD] Food.'
WHERE Tag = 'TXT_KEY_POLICY_CIVIL_SOCIETY_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Covert Action[ENDCOLOR]: Receive 100 [ICON_VP_SPY_POINTS] Spy Points. When successfully rigging an election in a City-State, the Influence you gain and the Influence other Players lose are increased by +50%. Spies stationed in a foreign City gain +10 Network Points per turn.'
WHERE Tag = 'TXT_KEY_POLICY_COVERT_ACTION_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Creative Expression[ENDCOLOR]: +2 [ICON_TOURISM] Tourism from Great Works. Museums, Broadcast Towers, Opera Houses, and Amphitheaters gain +3 [ICON_GOLDEN_AGE] Golden Age Points and [ICON_CULTURE] Culture.'
WHERE Tag = 'TXT_KEY_POLICY_CREATIVE_EXPRESSION_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Economic Union[ENDCOLOR]: Receive two additional [ICON_INTERNATIONAL_TRADE] Trade Routes. +6 [ICON_GOLD] Gold from trade routes with civilizations following Freedom.'
WHERE Tag = 'TXT_KEY_POLICY_ECONOMIC_UNION_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Media Culture[ENDCOLOR]: +25% [ICON_TOURISM] Tourism and +1 [ICON_HAPPINESS_1] Happiness generated by cities with a Broadcast Tower. +20% [ICON_CULTURE] Culture from Stadiums.'
WHERE Tag = 'TXT_KEY_POLICY_MEDIA_CULTURE_HELP';

UPDATE Language_en_US
SET Text = 'Containment'
WHERE Tag = 'TXT_KEY_POLICY_TREATY_ORGANIZATION';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Containment[ENDCOLOR]: [ICON_INTERNATIONAL_TRADE] Trade Routes to City-States generate +2 [ICON_INFLUENCE] Influence per turn (with the target City-State) per owned City-State Trade Route (up to +10). +1 [ICON_DIPLOMAT] Delegate in the World Congress for every 2 [ICON_CITY_STATE] City-States originally in the World.'
WHERE Tag = 'TXT_KEY_POLICY_TREATY_ORGANIZATION_HELP';

UPDATE Language_en_US
SET Text = 'In the Cold War era, competing ideologies to liberalism were perceived similarly to diseases whose spread must be curtailed for the safety of global order. As a foreign policy doctrine, Containment was most adamantly adhered to by the United States in their attempts to prevent the spread of Communism, acting as the guiding principle behind the generous aid and clemency for post-war Europe and Japan, but also taking the form of violent coups and repression in Latin America. The approach taken was the one deemed most effective at limiting the capacity of Communism to project power and influence in the world.'
WHERE Tag = 'TXT_KEY_POLICY_TREATY_ORGANIZATION_TEXT';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Their Finest Hour[ENDCOLOR]: All cities get +2 Air Unit Slots. Each Air Unit stationed in a city increases the City''s [ICON_STRENGTH] Defense by 3. Can build [COLOR_YELLOW]B17 Bombers[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_POLICY_THEIR_FINEST_HOUR_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Universal Suffrage[ENDCOLOR]: A [ICON_GOLDEN_AGE] Golden Age begins, and [ICON_GOLDEN_AGE] Golden Ages last 50% longer. Cities produce +1 [ICON_HAPPINESS_1] Happiness.'
WHERE Tag = 'TXT_KEY_POLICY_UNIVERSAL_SUFFRAGE_HELP';

UPDATE Language_en_US
SET Text = 'Self Determination'
WHERE Tag = 'TXT_KEY_POLICY_URBANIZATION';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Self Determination[ENDCOLOR]: Liberating a city (including removing another civ''s Sphere of Influence on a City-State through conquest) gives 15 XP to all units, 50 [ICON_INFLUENCE] Influence with all City-States, and 40 [ICON_RESEARCH] Science, scaling with Era and City [ICON_CITIZEN] Population. The liberated city gains an Arsenal and 6 units.'
WHERE Tag = 'TXT_KEY_POLICY_URBANIZATION_HELP';

UPDATE Language_en_US
SET Text = 'Self Determination is the idea that every group of people has the right to choose, or determine, how they are ruled. While the idea had already spread, one of the first major declarations of it was the Fourteen Points by US President Woodrow Wilson, which among other things, attempted to establish this principle in post World War I Europe. After World War II the Allies made similar pledges, and the UN holds this as a policy to this day. Self Determination is a simple idea, but it has had great impacts on many things, most importantly Independence. Though, it can be more complex as what exactly the people determine is not necessarily Independence, and many paths are open. Additionally, what group counts as a single people can further complicate the matter.'
WHERE Tag = 'TXT_KEY_POLICY_URBANIZATION_TEXT';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]New Deal[ENDCOLOR]: Landmarks and Great Person improvements produce +6 of their base yield types ([ICON_PRODUCTION] Manufactory/Citadel, [ICON_GOLD] Town, [ICON_RESEARCH] Academy, [ICON_CULTURE] Embassy, [ICON_PEACE] Holy Site, [ICON_TOURISM] Landmark), and +2 [ICON_TOURISM] Tourism'
WHERE Tag = 'TXT_KEY_POLICY_NEW_DEAL_HELP';

UPDATE Language_en_US
SET Text = 'Draft Registration'
WHERE Tag = 'TXT_KEY_POLICY_VOLUNTEER_ARMY';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Draft Registration[ENDCOLOR]: Military Units purchased with [ICON_GOLD] Gold receive full XP.'
WHERE Tag = 'TXT_KEY_POLICY_VOLUNTEER_ARMY_HELP';

UPDATE Language_en_US
SET Text = 'Conscription in the United States, commonly known as the draft, has been employed by the federal government of the United States in five conflicts: the American Revolution, the American Civil War, World War I, World War II, and the Cold War (including both the Korean War and the Vietnam War). The third incarnation of the draft came into being in 1940 through the Selective Training and Service Act. It was the country''s first peacetime draft. From 1940 until 1973, during both peacetime and periods of conflict, men were drafted to fill vacancies in the United States Armed Forces that could not be filled through voluntary means. The draft came to an end when the United States Armed Forces moved to an all-volunteer military force. However, the Selective Service System remains in place as a contingency plan; all male civilians between the ages of 18 and 25 are required to register so that a draft can be readily resumed if needed. United States Federal Law also provides for the compulsory conscription of men between the ages of 17 and 45 and certain women for militia service pursuant to Article I, Section 8 of the United States Constitution and 10 U.S. Code § 246.'
WHERE Tag = 'TXT_KEY_POLICY_VOLUNTEER_ARMY_TEXT';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Space Procurements[ENDCOLOR]: May invest in Spaceship parts with [ICON_GOLD] Gold. Build Spaceship Factories in half the usual time. +20% [ICON_RESEARCH] Science from Research Labs.'
WHERE Tag = 'TXT_KEY_POLICY_SPACE_PROCUREMENTS_HELP';

--------------------
-- Order
--------------------
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Academy of Sciences[ENDCOLOR]: -2 [ICON_HAPPINESS_3] Unhappiness from [ICON_RESEARCH] Illiteracy in all Cities, +100% [ICON_PRODUCTION] Production towards Research Labs, and receive 5 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Research Labs in your Empire. Research Labs generate an additional +2 [ICON_RESEARCH] Science.'
WHERE Tag = 'TXT_KEY_POLICY_ACADEMY_SCIENCES_HELP';

UPDATE Language_en_US
SET Text = 'Socialist Realism'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_REVOLUTION';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Socialist Realism[ENDCOLOR]: Unlocks the Propaganda Process (converts 15% of [ICON_PRODUCTION] Production into [ICON_TOURISM] Tourism). +25% [ICON_TOURISM] Tourism to other Civilizations following Order. +4 [ICON_PRODUCTION] Production from all [ICON_GREAT_WORK] Great Works.'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_REVOLUTION_HELP';

UPDATE Language_en_US
SET Text = 'The Socialist Realism movement, a style of "realistic expression" that began in Soviet Russia and became dominant in communist countries around the world, promoted the triumph of socialism through art, photography, film and public monuments. Along with the Socialist Classical style of architecture, Socialist Realism was the only Party approved style of public art in the Soviet Union for sixty years. The dramatic and minimalist monuments that resulted were seen as powerful propaganda tools in promoting the dictatorship of the proletariat and the physical display of communist beliefs.'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_REVOLUTION_TEXT';

UPDATE Language_en_US
SET Text = 'Cultural Revolution'
WHERE Tag = 'TXT_KEY_POLICY_DICTATORSHIP_PROLETARIAT';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Cultural Revolution[ENDCOLOR]: +25% [ICON_TOURISM] Tourism to civilizations with less [ICON_HAPPINESS_1] Happiness. +1 [ICON_HAPPINESS_1] Happiness from Factories.'
WHERE Tag = 'TXT_KEY_POLICY_DICTATORSHIP_PROLETARIAT_HELP';

UPDATE Language_en_US
SET Text = 'The Great Proletarian Cultural Revolution was a social movement conducted from 1966 through 1976 in the People''s Republic of China. Begun by Mao Zedong and the Communist Party, the Cultural Revolution sought to re-educate the common people by removing or repressing capitalist, traditional and religious elements, through arrests and violence if necessary. The cultural struggle spread through all levels of society, students, the military, the party and urban workers, delaying China''s reemergence in world affairs for decades.'
WHERE Tag = 'TXT_KEY_POLICY_DICTATORSHIP_PROLETARIAT_TEXT';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Double Agents[ENDCOLOR]: Receive 200 [ICON_VP_SPY_POINTS] Spy Points. Gain 125 [ICON_RESEARCH] Science when you identify a foreign Spy, scaling with Era.'
WHERE Tag = 'TXT_KEY_POLICY_DOUBLE_AGENTS_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Hero of the People[ENDCOLOR]: [ICON_GREAT_PEOPLE] Great Person rate increases by 25%. A [ICON_GREAT_PEOPLE] Great Person of your choice appears near your [ICON_CAPITAL] Capital.'
WHERE Tag = 'TXT_KEY_POLICY_HERO_OF_THE_PEOPLE_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Iron Curtain[ENDCOLOR]: Free Courthouse upon immediate City annexation. +200% [ICON_FOOD] Food or [ICON_PRODUCTION] Production from Internal Trade Routes. [ICON_CONNECTED] City connections generate +5 [ICON_GOLD] Gold and [ICON_PRODUCTION] Production.'
WHERE Tag = 'TXT_KEY_POLICY_IRON_CURTAIN_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Spaceflight Pioneers[ENDCOLOR]: Gain a free Great Engineer and Great Scientist. May finish Spaceship parts with Great Engineers, and expending a [ICON_GREAT_PEOPLE] Great Person grants 100 [ICON_RESEARCH] Science, scaling with Era. Hurrying [ICON_PRODUCTION] Production with Great Engineers 50% more effective.'
WHERE Tag = 'TXT_KEY_POLICY_SPACEFLIGHT_PIONEERS_HELP';

UPDATE Language_en_US
SET Text = 'Dictatorship of the Proletariat'
WHERE Tag = 'TXT_KEY_POLICY_PARTY_LEADERSHIP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Dictatorship of the Proletariat[ENDCOLOR]: +7 [ICON_FOOD] Food, [ICON_RESEARCH] Science, [ICON_GOLD] Gold, and [ICON_CULTURE] Culture per city.'
WHERE Tag = 'TXT_KEY_POLICY_PARTY_LEADERSHIP_HELP';

UPDATE Language_en_US
SET Text = 'In Marxist socio-political theory, the Dictatorship of the Proletariat refers to a social state where the working class has direct control of political power; "dictatorship" in this case does not refer to the common definition but rather that an entire social class holds control of the nation. Whether or not capitalists or others were disenfranchised in such a political order would depend, according to Marx and Engels, on specific circumstances at the time. Bringing about such a political order might, or might not, entail violence; but whatever the means, in the end the proletariat would supplant the bourgeoisie.'
WHERE Tag = 'TXT_KEY_POLICY_PARTY_LEADERSHIP_TEXT';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Patriotic War[ENDCOLOR]: Land Units gain +20% [ICON_STRENGTH] Strength when in or adjacent to a City. When you conquer a City for the first time, all Combat Units within 3 Tiles of the City are fully healed. Can build [COLOR_YELLOW]T-34s[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_POLICY_PATRIOTIC_WAR_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Resettlement[ENDCOLOR]: New Cities have +3 [ICON_CITIZEN] Population. All Cities gain +2 [ICON_CITIZEN] Citizens immediately. No Partisans from razing enemy Cities.'
WHERE Tag = 'TXT_KEY_POLICY_RESETTLEMENT_HELP';

UPDATE Language_en_US
SET Text = 'Communism'
WHERE Tag = 'TXT_KEY_POLICY_SKYSCRAPERS';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Communism[ENDCOLOR]: [ICON_GOLD] Gold cost of investing in buildings reduced by 25%. +20% [ICON_PRODUCTION] Production when building [ICON_WONDER] Wonders.'
WHERE Tag = 'TXT_KEY_POLICY_SKYSCRAPERS_HELP';

UPDATE Language_en_US
SET Text = 'Communism is a socioeconomic system structured upon common ownership of the means of production and characterized by the absence of social classes, money,[3][4] and the state; as well as a social, political and economic ideology and movement that aims to establish this social order. The movement to develop communism, in its Marxist–Leninist interpretations, significantly influenced the history of the 20th century, which saw intense rivalry between the states which claimed to follow this ideology and their enemies.'
WHERE Tag = 'TXT_KEY_POLICY_SKYSCRAPERS_TEXT';

UPDATE Language_en_US
SET Text = 'People''s Army'
WHERE Tag = 'TXT_KEY_POLICY_SOCIALIST_REALISM';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]People''s Army[ENDCOLOR]: +100% [ICON_PRODUCTION] Production towards Military Academies, and receive 5 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Military Academies in your Empire. Public Schools produce +5 [ICON_CULTURE] Culture and +2 [ICON_HAPPINESS_1] Happiness.'
WHERE Tag = 'TXT_KEY_POLICY_SOCIALIST_REALISM_HELP';

UPDATE Language_en_US
SET Text = 'The People''s Liberation Army (PLA) traces its origins to the August 1, 1927, Nanchang Uprising in which Kuomintang (Nationalists, also spelled ''Guomindang'') troops led by Communist Party of China leaders Zhu De and Zhou Enlai (while engaged in the Northern Expedition) rebelled following the violent dissolution of the first Kuomintang-Communist Party of China united front earlier that year. The survivors of that and other abortive communist insurrections, including the Autumn Harvest Uprising led by Mao Zedong, fled to the Jinggang Mountains along the border of Hunan and Jiangxi provinces. Joining forces under the leadership of Mao and Zhu, this collection of communists, bandits, Kuomintang deserters, and impoverished peasants became the First Workers'' and Peasants'' Army, or Red Army—the military arm of the Chinese Communist Party.'
WHERE Tag = 'TXT_KEY_POLICY_SOCIALIST_REALISM_TEXT';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Worker Faculties[ENDCOLOR]: Factories increase City [ICON_RESEARCH] Science output by 10%. +100% [ICON_PRODUCTION] Production towards Factories, and receive 5 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Factories in your Empire.'
WHERE Tag = 'TXT_KEY_POLICY_WORKERS_FACULTIES_HELP';

UPDATE Language_en_US
SET Text = 'Great Leap Forward'
WHERE Tag = 'TXT_KEY_POLICY_YOUNG_PIONEERS';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Great Leap Forward[ENDCOLOR]: Receive a free Technology. Spies gain +100% [ICON_RESEARCH] Science. +1 [ICON_RESEARCH] Science in Cities for every 3 non-Specialist [ICON_CITIZEN] Citizens.'
WHERE Tag = 'TXT_KEY_POLICY_YOUNG_PIONEERS_HELP';

UPDATE Language_en_US
SET Text = 'The Great Leap Forward of China was an economic and social campaign by the Communist Party of China (CPC) from 1958 to 1961. The campaign was led by Mao Zedong and aimed to rapidly transform the country from an agrarian economy into a communist society through rapid industrialization and collectivization.'
WHERE Tag = 'TXT_KEY_POLICY_YOUNG_PIONEERS_TEXT';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Five-Year Plan[ENDCOLOR]: +20% [ICON_PRODUCTION] Production towards Buildings in all Cities. +3 [ICON_PRODUCTION] Production for every Mine, Quarry, Lumber Mill, Oil Well, and Unique Improvement.'
WHERE Tag = 'TXT_KEY_POLICY_FIVE_YEAR_PLAN_HELP';

--------------------
-- Autocracy
--------------------

-- Leader title change
UPDATE Language_en_US
SET Text = 'Leader {1_PlayerName:textkey} of {2_CivName:textkey}'
WHERE Tag = 'TXT_KEY_AUTOCRACY_TITLE';

UPDATE Language_en_US
SET Text = 'Martial Spirit'
WHERE Tag = 'TXT_KEY_POLICY_NEW_ORDER';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Martial Spirit[ENDCOLOR]: +25% [ICON_STRENGTH] Combat Strength when attacking in melee for 50 turns, scaling with game speed. [ICON_WAR] War Weariness reduced by 25%, and [ICON_RAZING] Razing Speed is doubled.'
WHERE Tag = 'TXT_KEY_POLICY_NEW_ORDER_HELP';

UPDATE Language_en_US
SET Text = 'Martial spirit represents the prevasiveness of competition and combat-oriented sports within a society. Today, athletes usually fight one-on-one, but may still use various skill sets such as strikes in boxing that only allows punching, taekwondo where punches and kicks are the focus or muay thai and burmese boxing that also allow the use of elbows and knees. There are also grappling based sports that may concentrate on obtaining a superior position as in freestyle or Collegiate wrestling using throws such as in judo and Greco-Roman wrestling the use of submissions as in Brazilian jiu-jitsu. Modern mixed martial arts competitions are similar to the historic Greek Olympic sport of pankration and allow a wide range of both striking and grappling techniques. Combat sports may also be armed and the athletes compete using weapons, such as types of sword in western fencing (the foil, épée and saber) and kendo (shinai). Modern combat sports may also wear complex armour, like SCA Heavy Combat and kendo. In Gatka and Modern Arnis sticks are used, sometimes representing knives and swords.'
WHERE Tag = 'TXT_KEY_POLICY_NEW_ORDER_TEXT';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Cult of Personality[ENDCOLOR]: +50% Tourism [ICON_TOURISM] to civilizations fighting a common enemy. 50% of your highest Warscore counts as a [ICON_TOURISM] Tourism Modifier with all Civilizations. Free [ICON_GREAT_PEOPLE] Great Person of your choice.'
WHERE Tag = 'TXT_KEY_POLICY_CULT_PERSONALITY_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Elite Forces[ENDCOLOR]: Newly created Military Units receive +15 Experience. Military Units gain 50% more Experience from combat.'
WHERE Tag = 'TXT_KEY_POLICY_ELITE_FORCES_HELP';

UPDATE Language_en_US
SET Text = 'New World Order'
WHERE Tag = 'TXT_KEY_POLICY_FORTIFIED_BORDERS';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]New World Order[ENDCOLOR]: -2 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD] and [ICON_PRODUCTION] Distress in all Cities. Police Stations and Constabularies provide +3 [ICON_CULTURE] Culture and +5 [ICON_PRODUCTION] Production, and are constructed 100% more quickly.'
WHERE Tag = 'TXT_KEY_POLICY_FORTIFIED_BORDERS_HELP';

-- MISSING an updated _TEXT to define New World Order

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Futurism[ENDCOLOR]: Strengthens [ICON_TOURISM] Tourism of [COLOR_POSITIVE_TEXT]Historic Events[ENDCOLOR] generated by the Palace. +2 [ICON_CULTURE] Culture from Great Works. Earn 50 [ICON_TOURISM] Tourism when you conquer a city for the first time, scaling with Era and city size.'
WHERE Tag = 'TXT_KEY_POLICY_FUTURISM_HELP';

UPDATE Language_en_US
SET Text = 'Lebensraum'
WHERE Tag = 'TXT_KEY_POLICY_INDUSTRIAL_ESPIONAGE';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Lebensraum[ENDCOLOR]: Receive 10 [ICON_CULTURE] Culture and [ICON_GOLDEN_AGE] Golden Age Points when your borders expand, scaling with Era. Citadel tile-acquisition radius doubled.'
WHERE Tag = 'TXT_KEY_POLICY_INDUSTRIAL_ESPIONAGE_HELP';

UPDATE Language_en_US
SET Text = 'Lebensraum (German for "habitat" or literally "living space") was an ideology proposing an aggressive expansion of Germany and the German people. Developed under the German Empire, it became part of German goals during the First World War and was later adopted as an important component of Nazi ideology in Germany.'
WHERE Tag = 'TXT_KEY_POLICY_INDUSTRIAL_ESPIONAGE_TEXT';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Lightning Warfare[ENDCOLOR]: +3 [ICON_MOVES] Movement for Great Generals. Gunpowder units gain +15% attack and ignore enemy ZOC; Armored units gain +15% attack and +1 [ICON_MOVES] Movement.'
WHERE Tag = 'TXT_KEY_POLICY_LIGHTNING_WARFARE_HELP';

UPDATE Language_en_US
SET Text = 'Air Supremacy'
WHERE Tag = 'TXT_KEY_POLICY_MILITARISM';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Air Supremacy[ENDCOLOR]: Receive a free Airport in every City. +25% [ICON_PRODUCTION] Production when building Aircraft. Can build [COLOR_YELLOW]Zeroes[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_POLICY_MILITARISM_HELP';

UPDATE Language_en_US
SET Text = 'Air supremacy is a position in war where a side holds complete control of air warfare and air power over opposing forces. It is defined by NATO and the United States Department of Defense as the "degree of air superiority wherein the opposing air force is incapable of effective interference."'
WHERE Tag = 'TXT_KEY_POLICY_MILITARISM_TEXT';

UPDATE Language_en_US
SET Text = 'Military-Industrial Complex'
WHERE Tag = 'TXT_KEY_POLICY_MOBILIZATION';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Military-Industrial Complex[ENDCOLOR]: -33% [ICON_GOLD] Gold cost of purchasing units. +3 [ICON_RESEARCH] Science from [ICON_STRENGTH] Defense Buildings, Citadels, Forts, and Unique Improvements.'
WHERE Tag = 'TXT_KEY_POLICY_MOBILIZATION_HELP';

UPDATE Language_en_US
SET Text = 'The military-industrial complex, or military-industrial-congressional complex, comprises the policy and monetary relationships which exist between legislators, national armed forces, and the arms industry that supports them. These relationships include political contributions, political approval for military spending, lobbying to support bureaucracies, and oversight of the industry. It is a type of iron triangle. The term is most often used in reference to the system behind the military of the United States, where it gained popularity after its use in the farewell address of President Dwight D. Eisenhower on January 17, 1961, though the term is applicable to any country with a similarly developed infrastructure.'
WHERE Tag = 'TXT_KEY_POLICY_MOBILIZATION_TEXT';

UPDATE Language_en_US
SET Text = 'Mare Nostrum'
WHERE Tag = 'TXT_KEY_POLICY_NATIONALISM';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Mare Nostrum[ENDCOLOR]: +1 [ICON_RES_OIL] Oil and [ICON_RES_COAL] Coal for every City-State Alliance. +2 [ICON_PRODUCTION] Production and [ICON_CULTURE] Culture to Atolls, Fishing Boats, and Offshore Oil Platforms. Naval Ranged Units gain the [COLOR_POSITIVE_TEXT]Mare Nostrum[ENDCOLOR] Promotion.'
WHERE Tag = 'TXT_KEY_POLICY_NATIONALISM_HELP';

UPDATE Language_en_US
SET Text = 'Mare Nostrum, meaning "Our Sea", refers to the Roman Empire''s control of the entire Mediterranean Basin. Following Italian Unification, the term became a rallying cry for Italian expansion and reconquest, as befitting the true successors of the Roman Empire.'
WHERE Tag = 'TXT_KEY_POLICY_NATIONALISM_TEXT';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Police State[ENDCOLOR]: +3 [ICON_HAPPINESS_1] Local Happiness from every Courthouse, and +10 City Security from Police Stations. +100% [ICON_PRODUCTION] Production towards Courthouses and Police Stations.'
WHERE Tag = 'TXT_KEY_POLICY_POLICE_STATE_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Third Alternative[ENDCOLOR]: Quantity of Strategic Resources produced is increased by 100%. Reduces Unit [ICON_GOLD] Gold Maintenance costs by 25%.'
WHERE Tag = 'TXT_KEY_POLICY_THIRD_ALTERNATIVE_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Total War[ENDCOLOR]: +25% [ICON_PRODUCTION] Production when building Land Units. Warscore increases 25% more quickly, and it is 25% easier to bully City-States.'
WHERE Tag = 'TXT_KEY_POLICY_TOTAL_WAR_HELP';

UPDATE Language_en_US
SET Text = 'Co-Prosperity Sphere'
WHERE Tag = 'TXT_KEY_POLICY_GUNBOAT_DIPLOMACY';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Co-Prosperity Sphere[ENDCOLOR]: Bullying [ICON_CITY_STATE] City-States no longer reduces [ICON_INFLUENCE] Influence, revokes Quests, or cancels Pledges of Protection. Doing so decreases the [ICON_INFLUENCE] Influence of all other Civs with the bullied City-State by 10%, and increases your [ICON_INFLUENCE] Influence by 10%.'
WHERE Tag = 'TXT_KEY_POLICY_GUNBOAT_DIPLOMACY_HELP';

UPDATE Language_en_US
SET Text = 'The Greater East Asia Co-Prosperity Sphere was an economic bloc consisting of Japan and its Asian allies, plus the occupied states which they controlled. The purported goal of this organization was East Asian cooperation, self-sufficiency, and resistance against western colonialism. In reality, the Co-Prosperity Sphere was a propaganda tool to solidify Japanese Imperial conquests. Mobilizing anti-western sentiment worked to disguise the hegemonic aims of the Japanese, and facilitated their expansion into the Dutch East Indies, where the Japanese were initially welcomed as liberators. With the conclusion of World War II, the Japanese lost control of their vassals. Even today, the idea of an East Asian economic union is met with skepticism.'
WHERE Tag = 'TXT_KEY_POLICY_GUNBOAT_DIPLOMACY_TEXT';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]United Front[ENDCOLOR]: Unit Supply generated by [ICON_CITIZEN] Population increased by 50%. While at war, your resting point for [ICON_INFLUENCE] Influence with allied City-States is increased by 100, and Militaristic City-State unit gift rates triple.'
WHERE Tag = 'TXT_KEY_POLICY_UNITED_FRONT_HELP';
