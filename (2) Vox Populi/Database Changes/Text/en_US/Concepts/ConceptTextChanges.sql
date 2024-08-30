UPDATE Language_en_US
SET Text = '[COLOR_GREEN]Diplomatic Missions and You[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CITYSTATE_GIVEGOLD_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'How do Diplomatic Missions effect our relationship?'
WHERE Tag = 'TXT_KEY_CITYSTATE_GIVEGOLD_ADV_QUEST';
UPDATE Language_en_US
SET Text = 'The most effective method of raising your Influence with a city-state is to create a Diplomatic Unit and send it on a Diplomatic Mission. A Diplomatic Mission allows diplomatic units to directly raise your Influence with city-states. To conduct a Diplomatic Mission, move your Diplomatic Unit into city-state territory and press the Diplomatic Mission button. The base amount of Influence gained from a Diplomatic Mission increases as a player unlocks new promotions for Diplomatic Units through new unit types, wonders and buildings. [ENDLINE][ENDLINE]Players may only have a limited number of Diplomatic Units active at any given time. The maximum number of active Diplomatic Units increases based on the amount of the [ICON_RES_PAPER] Paper a player controls. Once used to generate influence, Diplomatic Units are expended and must be rebuilt to use again.'
WHERE Tag = 'TXT_KEY_CITYSTATE_GIVEGOLD_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Specialists[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CITIES_SPECIALISTS_HEADING2_TITLE';
UPDATE Language_en_US
SET Text = 'Specialists are citizens who have been assigned to work in a building constructed in their city. There are [COLOR_YELLOW]seven[ENDCOLOR] kinds of specialists in Civilization V: Writers, Artists, Musicians, Scientists, Merchants, Engineers, [COLOR_YELLOW]and Civil Servants[ENDCOLOR]. An Artists'' Guild, for example, allows one or two citizens to be assigned to work in the building as Artist specialists. Not all buildings allow specialists to be assigned to them. See the individual building entries for details.'
WHERE Tag = 'TXT_KEY_CITIES_SPECIALISTS_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Specialists and City Growth[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_BUILDINGS_SPECIALISTS_HEADING2_TITLE';
UPDATE Language_en_US
SET Text = 'Certain buildings allow you to create "specialists" out of your citizens to work those buildings. Specialists improve the output of the building, and they also increase the city''s output of Great People. [COLOR_YELLOW] However, they reduce the city''s growth because Specialists don''t produce food and also consume more food than other citizens. The amount of food a specialist consumes is based on your current Era.[ENDCOLOR][NEWLINE][NEWLINE]    [COLOR_CYAN]Ancient-Medieval[ENDCOLOR]: Specialists consume [ICON_FOOD] 3 Food.[NEWLINE]    [COLOR_CYAN]Renaissance[ENDCOLOR]: Specialists consume [ICON_FOOD] 4 Food.[NEWLINE]    [COLOR_CYAN]Industrial[ENDCOLOR]: Specialists consume [ICON_FOOD] 5 Food.[NEWLINE]    [COLOR_CYAN]Modern[ENDCOLOR]: Specialists consume [ICON_FOOD] 6 Food.[NEWLINE]    [COLOR_CYAN]Atomic[ENDCOLOR]: Specialists consume [ICON_FOOD] 7 Food.[NEWLINE]    [COLOR_CYAN]Information[ENDCOLOR]: Specialists consume [ICON_FOOD] 8 Food.'
WHERE Tag = 'TXT_KEY_BUILDINGS_SPECIALISTS_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Benefits of Specialists[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CITIES_BENEFITSSPECIALISTS_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'Specialists provide the following benefits:[NEWLINE][NEWLINE]Writers increase a city''s cultural output and speed the creation of Great Writers.[NEWLINE][NEWLINE]Artists increase a city''s cultural output and speed the creation of Great Artists.[NEWLINE][NEWLINE]Musicians increase a city''s cultural output and speed the creation of Great Musicians.[NEWLINE][NEWLINE]Merchants increase a city''s gold output and speed the creation of Great Merchants.[NEWLINE][NEWLINE]Scientists increase a city''s science output and speed the creation of Great Scientists.[NEWLINE][NEWLINE]Engineers increase a city''s production output and speed the creation of Great Engineers.[NEWLINE][NEWLINE][COLOR_YELLOW]Civil Servants provide a small amount of many yields and speed the creation of Great Diplomats.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CITIES_BENEFITSSPECIALISTS_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Great People[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATPEOPLE_HEADING2_TITLE';
UPDATE Language_en_US
SET Text = 'There are [COLOR_YELLOW]ten[ENDCOLOR] types of Great People: Great Artists, Great Musicians, Great Writers, Great Engineers, Great Merchants, Great Scientists, [COLOR_YELLOW]Great Diplomats[ENDCOLOR], Great Generals, Great Admirals, and Great Prophets. The first seven types are quite similar in functionality, each having abilities related to their areas of expertise, while Great Generals, Great Admirals, and Great Prophets are rather different: they are generated differently and they have different effects upon play. Great Generals and Great Admirals provide bonuses related to land and naval combat, and Great Prophets allow you to found a religion.'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATPEOPLE_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Generating Great People[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GENERATING_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = '[COLOR_CYAN]Great Writers, Artists, Musicians, Engineers, Scientists, Merchants and Diplomats[ENDCOLOR] are created in Cities by Specialists and Wonders which generate "Great People" (GP) points. Each City''s GP points are kept track of separately, points for different types of Great People are not pooled.[NEWLINE][NEWLINE]When a City has enough of a specific type of GP points, the points are expended to generate a Great Person of that type. Great People points in other Cities are not expended. Once a Great Person is generated, the amount required for the next Great Person of that type increases in all of the player''s Cities. Some buildings and policies increase the rate at which you generate Great People.[NEWLINE][NEWLINE][COLOR_CYAN]Great Generals and Admirals[ENDCOLOR] are generated somewhat differently from other Great People. Instead of being generated in cities by specialists, Great Generals and Admirals are generated by combat. Whenever one of your land units gains XPs, your civilization generates Great General points (except when fighting against barbarians). Whenever one of your navals units gains XPs, your civilization generates Great Admiral points. When you''ve got enough points, you earn a Great Person, and the amount of points needed for the next Great Person is increased.[NEWLINE][NEWLINE][COLOR_CYAN]Great Prophets[ENDCOLOR] are earned as the result of collecting [ICON_PEACE] Faith, and are the only unit that can found or enhance a religion. Once you''ve accumulated enough [ICON_PEACE] Faith (at least [COLOR_YELLOW]800[ENDCOLOR] in a standard speed game), a Great Prophet will spawn. The amount of Faith needed increases for each subsequent Great Prophet. [COLOR_YELLOW]The first Great Prophet you receive will always spawn in the Capital, and subsequent Great Prophets will spawn in your Holy City.[ENDCOLOR] After reaching the Industrial Era, Great Prophets are no longer spawned automatically, but can instead be purchased with Faith in cities.[NEWLINE][NEWLINE]Additionally, once you advance into the Industrial Age, you will be able to expend unused Faith to purchase Great People, depending on what Social Policy branches you have [COLOR_YELLOW]completed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GENERATING_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Great Writer[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATWRITER_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = '[COLOR_CYAN]Special Ability: {TXT_KEY_MISSION_CREATE_GREAT_WORK}[ENDCOLOR][NEWLINE]A Great Writer can create a [ICON_VP_GREATWRITING] Great Work of Writing (generates both [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism) that is placed in the nearest city that has an appropriate building with an empty slot (like an Amphitheater, National Monument, or Heroic Epic). The Great Writer is expended when used this way.[NEWLINE][NEWLINE][COLOR_CYAN]Special Ability: {TXT_KEY_MISSION_GIVE_POLICIES}[ENDCOLOR][NEWLINE]A Great Writer can write a Political Treatise, which grants the player a large amount of [ICON_CULTURE] Culture, [COLOR_YELLOW]scaling by 3% for every owned Great Work[ENDCOLOR]. The Great Writer is expended when used this way.'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATWRITER_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Great Artist[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATARTIST_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = '[COLOR_CYAN]Special Ability: {TXT_KEY_MISSION_CREATE_GREAT_WORK}[ENDCOLOR][NEWLINE]A Great Artist can create a [ICON_VP_GREATART] Great Work of Art (generates both [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism) that is placed in the nearest city that has an appropriate building with an empty slot (like the Palace, a Castle, or a Museum). The Great Artist is expended when used this way.[NEWLINE][NEWLINE][COLOR_CYAN]Special Ability: {TXT_KEY_MISSION_START_GOLDENAGE}[ENDCOLOR][NEWLINE]A Great Artist can be expended to [COLOR_YELLOW]generate Golden Age Points, which may trigger a [ICON_GOLDEN_AGE] Golden Age (extra [ICON_PRODUCTION] Production, [ICON_GOLD] Gold, and [ICON_CULTURE] Culture). The amount of Golden Age Points generated is based on the [ICON_TOURISM] Tourism and [ICON_GOLDEN_AGE] Golden Age Point output of the past 10 turns, and is increased by 10% for every owned themed Great Work set.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATARTIST_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Great Musician[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATMUSICIAN_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = '[COLOR_CYAN]Special Ability: {TXT_KEY_MISSION_CREATE_GREAT_WORK}[ENDCOLOR][NEWLINE]A Great Musician can create a [ICON_VP_GREATMUSIC] Great Work of Music that is placed in the nearest city that has an appropriate building with an empty slot (like an Opera House or Broadcast Tower). The Great Musician is expended when used this way.[NEWLINE][NEWLINE][COLOR_CYAN]Special Ability: {TXT_KEY_MISSION_ONE_SHOT_TOURISM}[ENDCOLOR][NEWLINE]A Great Musician can travel to another civilization and perform a Concert Tour, [COLOR_YELLOW]which will increase Tourism with the target Civilization by 100%, and all other Civilizations by 50%, for 10 Turns (plus 1 additional Turn for every owned [ICON_VP_GREATMUSIC] Great Work of Music). You also receive 1 [ICON_HAPPINESS_1] Happiness in every City. You cannot perform this action if at war with the target Civilization, or if your [ICON_TOURISM] Cultural Influence over the Civilization is [ENDCOLOR][COLOR_MAGENTA]Influential[ENDCOLOR][COLOR_YELLOW] or greater.[ENDCOLOR] This action consumes the Great Musician.'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATMUSICIAN_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Great Scientist[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATSCIENTIST_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = '[COLOR_CYAN]Special Improvement: Academy[ENDCOLOR][NEWLINE]You can expend a Great Scientist to create an Academy. While worked, the Academy will give your city a big science boost.[NEWLINE][NEWLINE][COLOR_CYAN]Special Ability: {TXT_KEY_MISSION_DISCOVER_TECH}[ENDCOLOR][NEWLINE]You can expend your Great Scientist to immediately gain a lump sum of science. [COLOR_YELLOW]Every owned Academy you''ve created and own increases the amount of science gained by 10%.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATSCIENTIST_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Great Merchant[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATMERCHANT_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = '[COLOR_CYAN]Special Improvement: Town[ENDCOLOR][NEWLINE]You can expend a Great Merchant to create the special Town improvement which, when worked, produces [ICON_GOLD] Gold [COLOR_YELLOW]and [ICON_FOOD] Food.[ENCDOLOR][NEWLINE][NEWLINE][COLOR_CYAN]Special Ability: {TXT_KEY_MISSION_CONDUCT_TRADE_MISSION}[ENDCOLOR][NEWLINE]If the Great Merchant is inside City-State territory that you are not at war with, you can expend it to conduct a Trade Mission.[NEWLINE][NEWLINE]You will receive a large amount of [ICON_GOLD] Gold [COLOR_YELLOW]and an instant "We Love the King Day" in all owned cities. Every Town you''ve created and own increases the gold and length of the "We Love the King Day" by 25%.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATMERCHANT_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Great Engineer[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATENGINEER_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = '[COLOR_CYAN]Special Improvement: Manufactory[ENDCOLOR][NEWLINE]You can expend a Great Engineer to create a Manufactory. A Manufactory produces huge amounts of production (hammers) for the city, if it is worked.[NEWLINE][NEWLINE][COLOR_CYAN]Special Ability: {TXT_KEY_MISSION_HURRY_PRODUCTION}[ENDCOLOR][NEWLINE]You can expend a Great Engineer to hurry production on the city''s current effort. [COLOR_YELLOW]The amount of Production is equal to 5 turns of the best city''s average recent output. Every owned Manufactory you''ve created and own increases it by 10%.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATENGINEER_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Great General[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATGENERAL_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = '[COLOR_CYAN]Special Ability: Combat Bonus[ENDCOLOR][NEWLINE]A Great General provides a 15% combat bonus to all friendly land units within 2 tiles. This combat bonus applies to all forms of land combat: melee, ranged, defense, and so forth.[NEWLINE][NEWLINE][COLOR_CYAN]Special Ability: {TXT_KEY_MISSION_CULTURE_BOMB}[ENDCOLOR][NEWLINE]You can expend the Great General to place a Citadel, which gives you ownership of all adjacent tiles, as your Culture borders will expand to surround the Citadel. Additionally, the Citadel provides a big defensive bonus to any unit occupying it. Further, it damages any enemy unit that ends its turn next to the Citadel (damage does not stack with other improvements). Note that a Citadel functions only when it''s in your territory. If it were to change hands when a nearby city is conquered, it will only be effective for the new owner. If the new Culture border claims hexes already owned by another civilization, you will incur a diplomatic penalty as a result. When a Citadel is placed, the Great General is consumed [COLOR_YELLOW] and your Military Unit Supply Cap is increased by 1[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATGENERAL_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Great Admiral[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATADMIRAL_TITLE';
UPDATE Language_en_US
SET Text = '[COLOR_CYAN]Special Ability: Combat Bonus[ENDCOLOR][NEWLINE]A Great Admiral provides a 15% combat bonus to all friendly naval units within 2 tiles. This combat bonus applies to all forms of naval combat: melee, ranged, defense, and so forth.[NEWLINE][NEWLINE][COLOR_CYAN]Special Ability: {TXT_KEY_MISSION_REPAIR_FLEET}[ENDCOLOR][NEWLINE]You can expend the Great Admiral to heal all of your Naval Units and Embarked Units in this tile and all adjacent tiles. [COLOR_YELLOW]This order also increases your Military Unit Supply Cap by 1.[ENDCOLOR] The Great Admiral is expended when used this way.[NEWLINE][NEWLINE][COLOR_CYAN]Special Ability: {TXT_KEY_MISSION_FREE_LUXURY}[ENDCOLOR][NEWLINE][COLOR_GREEN]You can send the Great Admiral on a Voyage of Discovery, which will provide you with two copies of a Luxury Resource not available on the current map. This order also increases your Military Unit Supply Cap by 1. The Great Admiral is expended when used this way.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATADMIRAL_BODY';

UPDATE Language_en_US
SET Text = 'Great Prophet'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATPROPHET_TITLE';
UPDATE Language_en_US
SET Text = '[COLOR_CYAN]Special Ability: {TXT_KEY_MISSION_FOUND_RELIGION}[ENDCOLOR][NEWLINE]If you haven''t founded a religion yet and there are still religions to be founded, you can use the Great Prophet to found a religion and choose religious beliefs for it. The city in which the religion is founded will be converted to your new religion. The Great Prophet is expended when used this way.[NEWLINE][NEWLINE][COLOR_CYAN]Special Ability: {TXT_KEY_MISSION_ENHANCE_RELIGION}[ENDCOLOR][NEWLINE]You can use the Great Prophet to enhance your religion with two additional religious beliefs. The Great Prophet is expended when used this way.[NEWLINE][NEWLINE][COLOR_CYAN]Special Improvement: Holy Site[ENDCOLOR][NEWLINE]A Great Prophet can construct the special Holy Site improvement which, when worked, produces additional [ICON_PEACE] Faith. The Great Prophet is expended when used this way.[NEWLINE][NEWLINE][COLOR_CYAN]Special Ability: {TXT_KEY_MISSION_SPREAD_RELIGION}[ENDCOLOR][NEWLINE]The Great Prophet can spread your religion 4 times, while removing all foreign pressure in the target cities. The Great Prophet is expended when used this way.'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_GREATPROPHET_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Special Improvements[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_SPECIALIMPROVEMENT_HEADING4_TITLE';
UPDATE Language_en_US
SET Text = 'Each Great Person type can be expended to create a Special Improvement on a tile within your civilization''s borders. The Special Improvement''s effects depend upon which Great Person is creating it - a Great Engineer''s Special Improvement generates production, for example, while a Great Merchant''s generates cash. [COLOR_YELLOW]The yields of Special Improvements increase as the game progresses based on Technologies, Policies, Buildings and Beliefs.[ENDCOLOR][NEWLINE][NEWLINE]A Special Improvement must be worked in order to have any effect. Further, a Special Improvement can be pillaged and repaired like any other Improvement. If constructed atop a resource, the Special Improvement will only provide access to it if that resource is a strategic resource.[NEWLINE][NEWLINE]Note that you have to move the Great Person out of the city and into your territory to construct a Special Improvement.'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_SPECIALIMPROVEMENT_HEADING4_BODY';

UPDATE Language_en_US
SET Text = 'Special Improvement: Town'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_CUSTOMSHOUSE_HEADING4_TITLE';
UPDATE Language_en_US
SET Text = 'You can expend a Great Merchant to create a Town. When worked, a Town generates a lot of gold and food per turn for its city, and increases the "We Love the King Day" duration from Great Merchant Trade Missions.[NEWLINE][NEWLINE]Towns gain additional [ICON_GOLD] Gold and [ICON_PRODUCTION] Production if built on a Road or Railroad that connects two owned Cities[NEWLINE][NEWLINE]Receive additional [ICON_GOLD] Gold and [ICON_PRODUCTION] Production (+1 pre-Industrial Era, +2 Industrial Era or later) if a Trade Route, either internal or international, passes over this Town.'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_CUSTOMSHOUSE_HEADING4_BODY';
UPDATE Language_en_US
SET Text = 'What does the town special improvement do?'
WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_CUSTOMSHOUSE_ADV_QUEST';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Resolutions[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CONGRESS_RESOLUTIONS_HEADING2_TITLE';
UPDATE Language_en_US
SET Text = 'A resolution is a change to the game world that is proposed to and decided by the civilizations of the World Congress. For example, a resolution can embargo trade routes with a certain civilization, increase the culture from Wonders, prevent the construction of nuclear weapons, or award someone Diplomatic Victory. A resolution may also repeal a previously passed resolution, reversing its effects.[NEWLINE][NEWLINE][COLOR_YELLOW]Only three civilizations get to propose resolutions: the current host of the Congress, and the other two players who have the most delegates.[ENDCOLOR][NEWLINE][NEWLINE]All civilizations get to help decide the outcome of proposed resolutions once the Congress is in session. They do so by allocating their delegates towards the outcome they desire: "Yea" to help it pass or "Nay" to help it fail. Some resolutions require choosing a civilization rather than simply "Yea" or "Nay".[NEWLINE][NEWLINE]Once all civilizations have used their delegates, the resolution''s outcome is decided by whichever option received the most delegate support. If there is a tie, the resolution fails.'
WHERE Tag = 'TXT_KEY_CONGRESS_RESOLUTIONS_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Delegates[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CONGRESS_DELEGATES_HEADING2_TITLE';
UPDATE Language_en_US
SET Text = 'Proposed resolutions to the Congress are decided through the use of delegates. Once a session of the Congress begins, civilizations divide their delegates as they like between proposed resolutions to support the outcomes they desire. Delegates support "Yea" in order to help pass the resolution, or "Nay" in order to help prevent it from passing. Some resolutions require choosing a civilization rather than simply "Yea" or "Nay".[NEWLINE][NEWLINE]Delegates can be obtained in the following ways:[NEWLINE][NEWLINE]  [COLOR_GREEN]Base Value[ENDCOLOR]: Each Civilization has a basic amount of Delegates depending on the current Era of the World Congress (starting with 1 Delegate when the World Congress is founded, then 1 additional Delegate per Era).[NEWLINE][NEWLINE] [COLOR_GREEN]Host Status[ENDCOLOR]: The Host of the World Congress gets 1-2 additional Delegates, depending on Era.[NEWLINE][NEWLINE]  [COLOR_GREEN]City State Alliances[ENDCOLOR]: Each City State Alliace grants 1 additional Delegate. Once the United Nations are founded, this number increases to 2 Delegates per Alliance.[NEWLINE][NEWLINE] [COLOR_GREEN]City State Embassies[ENDCOLOR]: [COLOR_YELLOW]Each Embassy built in a City State grants 1 additional Delegate.[ENDCOLOR][NEWLINE][NEWLINE]  [COLOR_GREEN]World Religions[ENDCOLOR]: If a Religion has been designated as the official World Religion by the World Congress, all Civilizations following it will get an additional Delegate. [COLOR_YELLOW]The founder of the World Religions also gets an additional Delegates for each foreign Civilization following the World Religion.[ENDCOLOR][NEWLINE][NEWLINE]  [COLOR_GREEN]Religious Authority[ENDCOLOR]: [COLOR_YELLOW]A Civilization that has founded a Religion and built the corresponding National Wonder gets 1 additional Delegate for every 10 Cities following the Religion.[ENDCOLOR][NEWLINE][NEWLINE]  [COLOR_GREEN]World Ideology[ENDCOLOR]: If an Ideology has been designated as the official World Ideology by the World Congress, all Civilizations following the ideology will get 1 additional Delegate [COLOR_YELLOW]and 1 additional Delegate for each foreign Civilization following the World Ideology.[ENDCOLOR][NEWLINE][NEWLINE]  [COLOR_GREEN]Policies and Wonders[ENDCOLOR]: Some Social Policies, National Wonders and World Wonders grant additional Delegates in different ways.[NEWLINE][NEWLINE]  [COLOR_GREEN]Globalization[ENDCOLOR]: When the Technology "Globalization" has been researched, each Diplomat in a foreign Capital grants 1 additional Delegate.'
WHERE Tag = 'TXT_KEY_CONGRESS_DELEGATES_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]The United Nations[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CONGRESS_UN_HEADING2_TITLE';
UPDATE Language_en_US
SET Text = 'Diplomatic Victory is only possible after the World Congress has become the United Nations. The United Nations are founded once half of the civilizations in the game reach the Atomic Era (or one civilization reaches the Information era). [COLOR_YELLOW]As a further requirement, the World Congress Resolution "United Nations" must have been passed and the United Nations World Wonder must have been built by a civilization. It is possible to achieve a Diplomatic Victory even if you have not built the United Nations yourself.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CONGRESS_UN_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Diplomatic Victory[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CONGRESS_VICTORY_HEADING2_TITLE';
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Once the World Congress has become the United Nations, Diplomatic Victory becomes possible if a World Ideology is active.[ENDCOLOR] To win Diplomatic Victory, a civilization must receive enough delegate support on a Global Hegemony resolution.[NEWLINE][NEWLINE]Unlike other resolutions, the Global Hegemony resolution cannot be proposed by a civilization. Instead, every other session of the United Nations is dedicated to deciding it if a World Ideology is active. During these sessions, which alternate with sessions of regular proposals, the World Leader resolution is automatically proposed to the Congress and no other proposals are made.[NEWLINE][NEWLINE]The amount of delegate support required to win depends on the number of civilizations and city-states in the game, and can be seen on the World Congress and Victory Progress screens. If no civilization receives enough support to win a World Leader resolution, the two civilizations that received the most support will permanently gain additional delegates.'
WHERE Tag = 'TXT_KEY_CONGRESS_VICTORY_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Cultural Victory[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CULTURE_VICTORY_HEADING2_TITLE';
UPDATE Language_en_US
SET Text = 'To win a Cultural Victory, [COLOR_YELLOW]you must construct the Citizen Earth Protocol wonder. This is possible if you are [ENDCOLOR][COLOR_MAGENTA]Influential[ENDCOLOR][COLOR_YELLOW] over every other civilization in the game. In addition, you must have two Tier 3 Tenets of an Ideology and your people must be Content[ENDCOLOR]. See the section on Victory for details.'
WHERE Tag = 'TXT_KEY_CULTURE_VICTORY_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Cultural Victory[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_CULTUREVICTORY_HEADING2_TITLE';
UPDATE Language_en_US
SET Text = 'You no longer win a Cultural Victory by fully exploring five Social Policy branches. Instead, you must become the dominant cultural influence in every civilization in the game using Tourism. [COLOR_YELLOW]You must also have two Tier 3 Tenets of an Ideology, and your people must be Content. Once these requirements are met, you can then construct the Citizen Earth Protocol wonder to win.[ENDCOLOR] See the section on Victory for more information.'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_CULTUREVICTORY_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Cultural Victory[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_VICTORY_CULTURAL_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'To win a Cultural Victory, you must become the dominant cultural influence in every civilization in the game [COLOR_YELLOW]and construct the Citizen Earth Protocol. You must also have two Tier 3 Tenets of an Ideology, and your people must be Content[ENDCOLOR]. Cultural Influence is achieved through Tourism generated by [ICON_GREAT_WORK] Great Works that you create, and [ICON_VP_ARTIFACT] Artifacts that you discover. The Tourism you generate has a constant impact on other civilizations, and its effect is amplified through Open Borders, Trade Routes, shared Religion and Ideologies, Research Agreements, etc.[NEWLINE][NEWLINE]When your cumulative Tourism output (for the entire game) surpasses the cumulative Culture output of each remaining civilization in the game, [COLOR_YELLOW]you will be able to construct the Citizen Earth Protocol if you also have an ideology and your population is Content[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_VICTORY_CULTURAL_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Tourism[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CULTURE_TOURISM_HEADING2_TITLE';
UPDATE Language_en_US
SET Text = '[ICON_TOURISM] Tourism is the primary yield you will use to spread your cultural influence to other civilizations. It is generated passively by [COLOR_POSITIVE_TEXT]Buildings[ENDCOLOR], [COLOR_POSITIVE_TEXT]Great Works[ENDCOLOR] or [COLOR_POSITIVE_TEXT]Artifacts[COLOR_YELLOW], and actively by [COLOR_POSITIVE_TEXT]Historic Events[COLOR_YELLOW] (for more on "Historic Events", see the section with this label) or completing [COLOR_POSITIVE_TEXT]Trade Routes[ENDCOLOR][COLOR_YELLOW] (if you have constructed buildings that grant such bonuses)[ENDCOLOR]. As you place more and more Great Works and Artifacts into your Museums, Amphitheaters, Opera Houses, etc., your [ICON_TOURISM] Tourism will continue to increase.

[NEWLINE][NEWLINE]Your [ICON_TOURISM] Tourism is multiplied with each player in the following ways [COLOR_YELLOW](minimum [COLOR_NEGATIVE_TEXT]-100%[COLOR_YELLOW])[ENDCOLOR]:
[NEWLINE][ICON_BULLET]Any Trade Route connecting your City with their City [COLOR_YELLOW]([COLOR_POSITIVE_TEXT]+10%[COLOR_YELLOW])[ENDCOLOR].
[NEWLINE][ICON_BULLET][COLOR_YELLOW]Trade Open Borders to the player ([COLOR_POSITIVE_TEXT]+15%[COLOR_YELLOW]).[ENDCOLOR]
[NEWLINE][ICON_BULLET]Share your Religion with the player [COLOR_YELLOW]([COLOR_POSITIVE_TEXT]+X%[COLOR_YELLOW], the percent of its followers in their empire, maximum [COLOR_POSITIVE_TEXT]+50%[COLOR_YELLOW])[ENDCOLOR].
[NEWLINE][ICON_BULLET][COLOR_YELLOW]Have a Diplomat in their Capital City ([COLOR_POSITIVE_TEXT]+20%[COLOR_YELLOW]).[ENDCOLOR]
[NEWLINE][ICON_BULLET][COLOR_YELLOW]Have less Boredom in your Empire ([COLOR_POSITIVE_TEXT]+X%[COLOR_YELLOW], the difference in Boredom between your two Empires).[ENDCOLOR]
[NEWLINE][ICON_BULLET][COLOR_YELLOW]Expend a Great Musician inside their borders ([COLOR_POSITIVE_TEXT]+100%[COLOR_YELLOW], and [COLOR_POSITIVE_TEXT]+50%[COLOR_YELLOW] with everyone else).[ENDCOLOR]
[NEWLINE][ICON_BULLET][COLOR_YELLOW]Gain the player as a Vassal ([COLOR_POSITIVE_TEXT]+33%[COLOR_YELLOW]).[ENDCOLOR]
[NEWLINE][ICON_BULLET]Have a different Ideology than the player [COLOR_YELLOW]([COLOR_NEGATIVE_TEXT]-10%[COLOR_YELLOW])[ENDCOLOR].
[NEWLINE][ICON_BULLET][COLOR_YELLOW]Own more non-Puppet Cities than the player ([COLOR_NEGATIVE_TEXT]-5% per additional City[COLOR_YELLOW]).[ENDCOLOR]

[NEWLINE][NEWLINE][COLOR_YELLOW]Additionally, certain player Abilities, Policies, Tenets, Corporations, and World Congress proposals can change this modifier.[ENDCOLOR] Other Abilities instead directly multiply the tourism generated in a city.'
WHERE Tag = 'TXT_KEY_CULTURE_TOURISM_HEADING2_BODY';

UPDATE Language_en_US
SET Text = 'Laborers'
WHERE Tag = 'TXT_KEY_CITIES_UNEMPLOYEDCITIZENS_HEADING3_TITLE';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Healing[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_COMBAT_HEALTINGDAMAGE_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'To heal damage, a unit must remain inactive for a turn. The amount of damage that a unit heals depends upon the unit''s location.[NEWLINE][NEWLINE]In a Friendly City: A unit heals [COLOR_YELLOW]20[ENDCOLOR] HPs per turn.[NEWLINE]In Friendly Territory: [COLOR_YELLOW]15[ENDCOLOR] HP per turn.[NEWLINE]In Neutral Territory: [COLOR_YELLOW]10[ENDCOLOR] HP per turn.[NEWLINE]In Enemy Territory: [COLOR_YELLOW]5[ENDCOLOR] HP per turn.[NEWLINE][NEWLINE][COLOR_YELLOW]Units within the borders of a City that is in Resistance heal only 5HP per turn. Units within the borders of a City that is being razed heal 20HP per turn (looting).[ENDCOLOR][NEWLINE][NEWLINE]Note that certain promotions will accelerate a unit''s healing rate.'
WHERE Tag = 'TXT_KEY_COMBAT_HEALTINGDAMAGE_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Cities Firing at Attackers[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_COMBAT_CITYFIRINGATTACKERS_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'A city has a Ranged Combat Strength equal to its full Strength at the start of combat, and [COLOR_YELLOW]it has a range of 1. This range increases as the game progresses based on researched technologies (look for the "ranged strike" icon in the tech tree for these technologies).[ENDCOLOR] It may attack any one enemy unit within that range. Note that the city''s Ranged Combat Strength doesn''t decline as the city takes damage; it remains equal to the city''s initial Strength until the city is captured.'
WHERE Tag = 'TXT_KEY_COMBAT_CITYFIRINGATTACKERS_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Acquiring XPs Through Combat[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_COMBAT_ACQUIRINGXP_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'A unit gains XPs for surviving a round of combat (or, if a scout, from exploration). The unit doesn''t have to win the combat or destroy the enemy to get the experience; it accrues each round that the unit lives through.[NEWLINE][NEWLINE]The amount of XPs the unit gets depends upon the circumstances of the combat. Generally, units get more XPs for attacking than defending, and more for engaging in melee combat than for other types. Here are some numbers (see the Charts and Tables section for a complete list):[NEWLINE]An Attacking Melee Unit: 5 XPs[NEWLINE]Defending Against a Melee Attack: 4 XPs[NEWLINE]An Attacking Ranged Unit: 2 XPs[NEWLINE]Being Attacked by a Ranged Unit: 2 XPs[NEWLINE]Limitations: Once a unit has gotten 45 XPs, it no longer gets any additional XPs for fighting Barbarians.[COLOR_YELLOW] Once a unit has gotten 70 XPs, it no longer gets any additional XPs for fighting City-State Units.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_COMBAT_ACQUIRINGXP_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Barbarian Units[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_BARBARIAN_UNITS_HEADING2_TITLE';
UPDATE Language_en_US
SET Text = 'Barbarian encampments can create almost any kind of unit in the game - from warriors and spearmen to cannons and tanks. (They can build units equal to those that the most advanced civilization can create.)[NEWLINE][NEWLINE]Once created the barbarian units will either hang around their encampment or head off toward the nearest civilization or city-state and try to cause trouble. They''ll attack units, destroy improvements and menace cities. [COLOR_YELLOW]Barbarians next to a city without a garrison can steal food, culture, production or science from the owner. They can also conquer a poorly-defended city and turn it into a Barbarian City.[ENDCOLOR][NEWLINE][NEWLINE]This is why it''s important to periodically sweep the countryside around your civilization, destroying encampments before they become a threat.'
WHERE Tag = 'TXT_KEY_BARBARIAN_UNITS_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Settlers And Food Production[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_FOOD_SETTLERS_HEADING2_TITLE';
UPDATE Language_en_US
SET Text = 'Settlers can only be constructed in cities of size [COLOR_YELLOW]4[ENDCOLOR] or larger [COLOR_YELLOW]after researching Pottery[ENDCOLOR]. During construction, settlers consume a city''s production and all of the city''s excess food intake. As long as the settler is in production, the city will not grow or add food to its growth bucket. [COLOR_YELLOW]After the construction of the Settler is completed, the city loses one Population. Later in the game, Settlers are replaced by more advanced units which can found cities with additional infrastructure.[ENDCOLOR] See the section on Settlers for more details.'
WHERE Tag = 'TXT_KEY_FOOD_SETTLERS_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]We Love the King Day (WLtKD)[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_RESOURCES_CITYREQUESTS_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'Periodically a city may request that you acquire [COLOR_YELLOW]or possess[ENDCOLOR] a specific luxury resource. If you do so, the city will go into "We Love the King Day" for [COLOR_YELLOW]10[ENDCOLOR] turns, during which the city''s growth rate is increased by 25%. When the [COLOR_YELLOW]10[ENDCOLOR] turns are over, the city will demand another luxury resource. Filling that request will cause the city to resume WLtKD for another [COLOR_YELLOW]10[ENDCOLOR] turns.'
WHERE Tag = 'TXT_KEY_RESOURCES_CITYREQUESTS_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'City Locations and Yields'
WHERE Tag = 'TXT_KEY_CITIES_WHERETOCONSTRUCT_HEADING2_TITLE';
UPDATE Language_en_US
SET Text = 'Cities should be founded in locations with plenty of food and production and with access to resources such as wheat, fish, and cattle. Cities founded on hills gain a defensive bonus, making it harder for enemies to capture them. It''s often a good idea to build a city on a river or coastal hex, as cities built in different locations have different starting yields:[NEWLINE][ICON_BULLET] Cities on hills: 2 [ICON_FOOD], 2 [ICON_PRODUCTION] Production.[NEWLINE][ICON_BULLET] Cities on flat land or mountains next to fresh water: 3 [ICON_FOOD] Food, 1 [ICON_PRODUCTION] Production.[NEWLINE][ICON_BULLET] Cities on mountains without fresh water: 2 [ICON_FOOD] Food, 2 [ICON_PRODUCTION] Production, 1 [ICON_GOLD] Gold.[NEWLINE][ICON_BULLET] Cities on flat land without fresh water: 2 [ICON_FOOD], 1 [ICON_PRODUCTION] Production, 1 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_CITIES_WHERETOCONSTRUCT_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]City Connections[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GOLD_TRADE_ROUTES_HEADING3_TITLE';

UPDATE Language_en_US
SET Text = 'If a City is connected by a road and/or [COLOR_YELLOW]Lighthouse[ENDCOLOR] to your Capital City (i.e. both cities have a Lighthouse), that City has a City Connection with the Capital. Each City Connection is worth a certain amount of gold each turn, the amount determined by the size of the two cities.[NEWLINE][NEWLINE][COLOR_GREEN]If a City is connected by a railroad and/or Seaport to your Capital City (i.e. both cities have a Seaport), that City has an Industrial City Connection with the Capital. An Industrial City Connection gives a production bonus to the targeted city, the amount determined by the size of the two cities. An Industrial City Connection also allows the construction of the Coaling Station, an industrial era building which gives a massive production boost to the city.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GOLD_TRADE_ROUTES_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_GREEN]The Village[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GOLD_TRADINGPOST_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'Does the village provide gold?'
WHERE Tag = 'TXT_KEY_GOLD_TRADINGPOST_ADV_QUEST';
UPDATE Language_en_US
SET Text = 'Construct a Village improvement in a tile to increase its gold output. Earns additional gold and production if placed on a Road or Railroad that connects two owned cities (that are both connected to the Capital) and/or if a Trade Route, either internal or international, passes over it. Cannot be built adjacent to one another.'
WHERE Tag = 'TXT_KEY_GOLD_TRADINGPOST_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Roads and City Connections[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_WORKERS_TRADEROUTES_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'If there is a road between your Capital and another of your cities, those cities have a City Connection. City Connections give your civilization gold bonuses each turn, the amount depending upon the size of the cities involved. ([COLOR_YELLOW]Lighthouses[ENDCOLOR] can also create city connections between coastal cities.) See the section on City Connections for details.'
WHERE Tag = 'TXT_KEY_WORKERS_TRADEROUTES_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Spies and City-States[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CONCEPT_ESPIONAGE_RIG_ELECTION_TOPIC';
UPDATE Language_en_US
SET Text = 'Spies can be used to increase your influence with City-States by "rigging" their local elections or staging a coup. To interact with a City-State, first move a spy to a target City-State. Once the spy arrives, there will be a period of time when they are "Establishing Surveillance" (noted in the "Activity" column). Once surveillance is established, your espionage options become available. Rigging an election, a passive ability, occurs every 15 turns while your spy is located within the City-State. If you are the only spy in the City-State, your influence will increase while everyone else''s will drop. The presence of an enemy spy, especially one of a higher rank, can cause this mission to fail. No worries though, failing to rig an election is not dangerous to your spy.[NEWLINE][NEWLINE]Staging a coup is more powerful than rigging an election, but also much riskier. A successful coup essentially allows you to "steal" the allied status from another civiization. To stage a coup, target a City-State that is allied with another player, and place your spy the same way you would with rigging an election. Once surveillance is established, you will have a new button available to you called "Coup". Clicking it will open a pop-up message that gives you the odds of success. The closer you are in influence to the target civilization, the greater your odds. A high-level spy[COLOR_YELLOW] or having succesfully rigged elections in the City-State [ENDCOLOR] also increases your coup chances. The catch? If you fail, your spy is executed.'
WHERE Tag = 'TXT_KEY_CONCEPT_ESPIONAGE_RIG_ELECTION_SUMMARY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Purchasing an Item[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CITIES_PURCHASINGITEM_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'You can purchase units with gold, [COLOR_YELLOW]or invest gold in buildings to reduce their construction cost[ENDCOLOR], from within your city screen. This can be helpful if you need something in a hurry, like additional units to defend against an invader.'
WHERE Tag = 'TXT_KEY_CITIES_PURCHASINGITEM_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Buying Units, Buildings or Wonders[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GOLD_PURCHASEUNITS_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'You can spend gold to purchase units [COLOR_YELLOW]or invest in a building[ENDCOLOR] in a city. Click on a unit (if you can afford it!) and it will be immediately trained in the city, and the amount deducted from your treasury. [COLOR_YELLOW]If you click on a building, you will invest in it, reducing the production cost of the building by 50%, or 25% if it is a wonder.[ENDCOLOR][NEWLINE][NEWLINE]Note that "projects" - the Manhattan Project, etc. - cannot be purchased.[NEWLINE][NEWLINE][COLOR_YELLOW]The purchase costs of units and buildings increase slightly with the number of technologies you have researched.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GOLD_PURCHASEUNITS_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_GREEN]Happiness[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_HAPPINESS_HEADING1_TITLE';
UPDATE Language_en_US
SET Text = 'The happiness system in Vox Populi is completely revised, favoring neither tall nor wide playstyles. Happiness is now based on a cause-and-effect mechanism that makes your empire feel far more alive, and dynamic, than ever before.[NEWLINE][NEWLINE]Happiness and Unhappiness are calculated locally for each City in your Empire. When the Local Unhappiness in a City exceeds the Local Happiness, the City grows slower than normally, and Production will be reduced when producing a Settler or Military Unit. This is also indicated by an [ICON_HAPPINESS_3] Unhappiness Icon displayed under the City Banner. Special rules apply for Puppet Cities and Occupied Cities.[NEWLINE][NEWLINE]The sum of the Happiness in all Cities of your Empire is compared to the sum of the Unhappiness in all of your Cities to determine the Approval Rating, which is displayed on the status line of the Main Screen (in the upper left-hand corner of the game). Watch it carefully. If it starts to drop below 50%, your population is getting restless, resulting in a growth reduction in all cities, a penalty when producing Settlers, and reduced combat strength. If it starts to dip below 35%, you are in trouble. Barbarians will spawn in your empire, and cities may abandon your empire and join other civilizations. You can get an overview of the different sources of your Happiness and Unhappiness by hovering your cursor over the status line.'
WHERE Tag = 'TXT_KEY_HAPPINESS_HEADING1_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_GREEN]What Causes Happiness[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_HAPPINESS_CAUSES_HEADING2_TITLE';
UPDATE Language_en_US
SET Text = 'Happiness in your empire can come from the following sources:[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Starting Happiness[ENDCOLOR]: The amount of Happiness that your civilization begins with and the starting Local Happiness in the [ICON_CAPITAL] Capital are determined by the game''s difficulty setting. Starting Happiness will become less important over time as Happiness and Unhappiness from other sources grow.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Natural Wonders and Landmarks[ENDCOLOR]: Discovering Natural Wonders or using Archaeologists to build Landmarks (in your own or in other player''s lands) gives you a small Happiness bonus.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]City States[ENDCOLOR]: Mercantile City States provide you with a Happiness bonus if you are friends or allies with them.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Vassals[ENDCOLOR]: If you have vassalized other civilizations, they may provide you with some additional Happiness based on their own Approval Rating.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Luxury Resources[ENDCOLOR]: You receive Happiness for each unique Luxury Resource connected to your empire. Luxuries are therefore less vital early on, but also not useless in the late game. Moreover, Monopolies on some Luxury Resources grant additional Happiness to your empire.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Locally Generated Happiness[ENDCOLOR]: Certain Buildings increase your local Happiness in the Cities where they are built. Other sources of Local Happiness include Religious Beliefs, Policies, World Wonders and Events. Note that this does not apply to Puppet Cities and Occupied Cities, which can never generate Local Happiness.'
WHERE Tag = 'TXT_KEY_HAPPINESS_CAUSES_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_GREEN]What Causes Unhappiness[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_HAPPINESS_CAUSESUNHAPPINESS_HEADING2_TITLE';
UPDATE Language_en_US
SET Text = 'The Unhappiness a city produces is no longer equal to the number of citizens in it, although population still strongly affects Unhappiness. Unhappiness in Vox Populi is caused by several sources, each of them contributing to the total Unhappiness a city has. A detailed overview of the sources of Unhappiness in a city can be seen by hovering over the "[ICON_HAPPINESS_3] Unhappiness" tab in the City Screen. The total amount of Unhappiness in a city cannot exceed the city''s population, no matter how much Unhappiness is generated from the different sources. The only exception to this is Unhappiness from Urbanization, which always applies, even if it puts the total Unhappiness of your city above its population. The possible sources of Unhappiness are the following:[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]Unhappiness from Needs[ENDCOLOR]: Often, most of the Unhappiness in a city is caused by various Needs. With the exception of Religious Unrest, Unhappiness arises in a city if its output of a certain yield per citizen (like gold per citizen, science per citizen, etc.) is lower than the respective Needs Threshold, a reference value which is calculated based on the yields in other cities worldwide. The lower the city''s yield output per citizen is, the higher the generated Unhappiness will be. It is therefore necessary to keep yields in your cities increasing, as your population will become more and more demanding over time due to the stronger competition (other cities grow as well, increasing the Needs Thresholds). Avoiding population growth can help in keeping Unhappiness from Needs manageable, and certain Building and Policies reduce Unhappiness from Needs by a fixed amount. There are five main Needs, four based on different yield types and one based on religious diversity:[NEWLINE]
[COLOR_NEGATIVE_TEXT]Distress[ENDCOLOR]: Distress is based on the sum of your [ICON_FOOD] food and [ICON_PRODUCTION] production yields. Internal Trade Routes can be used to increase a city''s food or production output, and thus counter Distress. The Farming Process also counters Distress.[NEWLINE]
[COLOR_NEGATIVE_TEXT]Poverty[ENDCOLOR]: Poverty is based on the [ICON_GOLD] gold yield of a city. To counteract Poverty, increase the city''s gold output, for example by working tiles that provide gold or by constructing buildings like the Market. International Trade Routes give their gold to their city of origin, so sending Trade Routes from impoverished cities can also help counter Poverty. The Wealth process also counters Poverty.[NEWLINE]
[COLOR_NEGATIVE_TEXT]Illiteracy[ENDCOLOR]: Illiteracy is based on the [ICON_RESEARCH] science yield of a city. Constructing science buildings or establishing Trade Routes to more advanced civilizations can help reduce Illiteracy. The Research process also counters Illiteracy.[NEWLINE]
[COLOR_NEGATIVE_TEXT]Boredom[ENDCOLOR]: Boredom is based on the [ICON_CULTURE] culture yield of a city. Boredom is especially concerning, as other civilizations get a tourism bonus toward you if you have high levels of Boredom. Buildings providing culture per turn and Trade Routes to more cultured civilizations can counteract Boredom. Great Works can also be moved to bored cities to increase their culture output. Finally, the Arts process also counters Boredom.[NEWLINE]
[COLOR_NEGATIVE_TEXT]Religious Unrest[ENDCOLOR]: Religious Unrest is produced by religious diversity rather than city yields. If a city has a majority religion, 1 Unhappiness is generated for every 2 citizens not following the majority religion (this base value is affected by Need Modifiers). This can be reduced by using Missionaries, Inquisitors, or Great Prophets to remove religious minorities.[NEWLINE]

[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]Other sources of Unhappiness[ENDCOLOR]: In addition to Unhappiness from Needs, there are also some other sources of Unhappiness that can affect a city.[NEWLINE]
[COLOR_NEGATIVE_TEXT]Famine[ENDCOLOR]: If a city''s food output is below zero, it gains 1 Unhappiness for each point of food lost per turn. Thus, a starving city can increase Unhappiness tremendously. This often happens when enemy units start pillaging and blocking tiles from being worked, causing mass starvation.[NEWLINE]
[COLOR_NEGATIVE_TEXT]Pillaged Tiles[ENDCOLOR]: A city gains 1 Unhappiness for every two pillaged tiles in its vicinity. People don''t like seeing their houses burning and will start generating Unhappiness until you do something about it.[NEWLINE]
[COLOR_NEGATIVE_TEXT]Isolation[ENDCOLOR]: An isolated city produces 1 Unhappiness for every 3 Citizens. A city is isolated if it lacks a City Connection to the capital via a road, Lighthouse, or Trade Route. Note that Isolation can occur if a road or lighthouse connection is broken because of a military blockade. Trade Routes are not directly affected by blockades, but may be pillaged by the enemy units if not protected.[NEWLINE]
[COLOR_NEGATIVE_TEXT]Urbanization[ENDCOLOR]: Each Specialist in the city produces 1 Unhappiness, representing the effects of people living in large cities such as pollution and a high population density. Unlike the other sources of Unhappiness, Unhappiness from Urbanization is always added to the total Unhappiness a city has, even if this puts the total Unhappiness of a city above its population. Some buildings and policies allow a city to have "free" Specialists which generate no Urbanization.[NEWLINE]
[COLOR_NEGATIVE_TEXT]War Weariness[ENDCOLOR]: If you are in a prolonged conflict or suffer high losses in war, your citizens will be unhappy due to War Weariness. War Weariness develops in every city and can significantly increase the empire''s total unhappiness. For more details on War Weariness, see the corresponding entry in the Combat Rules Section.[NEWLINE]
[COLOR_NEGATIVE_TEXT]Public Opinion[ENDCOLOR]: Later in the game, other civilizations might exert Ideological Pressure on you, influencing your citizens. If citizens are less than Content with your choice of ideology, they will generate Unhappiness from Public Opinion. If this Unhappiness is too high and your empire is unhappy, you may be forced to switch Ideologies.'
WHERE Tag = 'TXT_KEY_HAPPINESS_CAUSESUNHAPPINESS_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Ideologies[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_IDEOLOGY_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'The three Ideology trees, Freedom, Order, and Autocracy, have greatly expanded in Brave New World. All civilizations in the game, [COLOR_YELLOW]on acquiring 18 Policies and having advanced at least to the Industrial Era, or upon reaching the Atomic Era,[ENDCOLOR] will be required to choose an Ideology for their civilization. Each Ideology tree contains 3 tiers of "tenets" that you use to customize your Ideology, with the third and final tier holding the most powerful benefits. As with regular Social Policies, players use Culture to purchase additional tenets as they move through the game.[NEWLINE][NEWLINE]Civilizations that share a common Ideology will receive benefits with their diplomatic relationships. Civilizations that have conflicting Ideologies have multiple side-effects. For example, a negative effect on their diplomatic relationship and happiness penalties take effect if an opposing Ideology has a stronger Cultural influence on your people. If you let your people become too unhappy, there is a chance that your cities may declare that they are joining your opponents empire. As a last resort, you, or other players, can resort to a "Revolution" to switch Ideologies to one that is preferable to your people.[NEWLINE][NEWLINE]For more information on Ideological Tenets, click on the "Social Policies" tab along the top of the Civilopedia, and scroll down to the Order, Freedom, and Autocracy sections.'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_IDEOLOGY_HEADING3_BODY';

-- Fortification
UPDATE Language_en_US
SET Text = 'Many units have the ability to "fortify." This means that the unit "digs in" and creates defensive works in its current location. This gives the unit certain defensive bonuses, making it much tougher to kill. However, fortifications are strictly defensive: if the unit moves or attacks, the fortifications are destroyed. While fortified, a unit will not activate. It will remain inactive until you manually activate it by clicking on the unit.[NEWLINE][NEWLINE]An improvement may also contain [COLOR_POSITIVE_TEXT]Fortifications[ENDCOLOR], which means units stationed on this improvement, like those garrisoning a city, will not move out of the tile after a victory in melee combat.'
WHERE Tag = 'TXT_KEY_COMBAT_FORTIFICATION_HEADING3_BODY';

-- Clearing Land
UPDATE Language_en_US
SET Text = 'Once their civilization has learned certain technologies (see tech tree), workers can remove forests, jungles, and marshes from tiles. Once these features are removed, they are gone forever.'
WHERE Tag = 'TXT_KEY_WORKERS_CLEARINGLAND_HEADING2_BODY';

-- City Unhappiness
UPDATE Language_en_US
SET Text = 'If your civilization is unhappy, then the city will produce less food. The city will produce enough food to feed its citizens, but there is a decrease in growth locally.'
WHERE Tag = 'TXT_KEY_FOOD_UNHAPPINESS_HEADING3_BODY';

-- Trade Routes
UPDATE Language_en_US
SET Text = 'Trade Routes are established between two cities of different civilizations, providing Gold to each every turn. The civilization that the Trade Route originates from gets a larger sum of Gold than the destination civilization. The amount of Gold a Trade Route produces is dependent upon the resource diversity and revenue of the origin city.[NEWLINE][NEWLINE]Trade Routes can also be created between two cities of the same civilization. Once the origin city has a Granary, it can send Food to the destination city, and once it has a Workshop it can send Production. These types of internal Trade Routes do not benefit the origin city.[NEWLINE][NEWLINE]You can only have a limited number of Trade Routes at any time. Researching certain technologies increases the number of routes you have available. For details about your existing Trade Routes, open the Trade Route Overview screen, located in the Additional Information drop-down menu.[NEWLINE][NEWLINE]The distance that Trade Routes can travel is affected by roads, terrain, hostile troops, and features. Keep this in mind when expanding, and when trying to link cities via trade routes.'
WHERE Tag = 'TXT_KEY_TRADE_ROUTES_HEADING2_BODY';

-- Change Construction or Purchase
UPDATE Language_en_US
SET Text = 'You can change city construction orders on the City Screen. You can also expend gold to purchase a unit (or invest in a building) on this screen as well. See the City Screen section for details.'
WHERE Tag = 'TXT_KEY_BUILDINGS_PURCHASING_HEADING3_BODY';

-- Freedom
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Freedom[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_FREEDOM_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'The Freedom ideology provides bonuses for Culture, Tourism, Specialist production, and more. [COLOR_YELLOW]This ideology unlocks upon entering the Atomic Era, or unlocking 18 Policies and having advanced at least to the Industrial Era, whichever comes first.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_FREEDOM_HEADING3_BODY';

-- Order
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Order[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_ORDER_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'Players interested in creating massive, sprawling civs should turn to the Order ideology, as the strength of the empire is determined by the total number of cities it contains. [COLOR_YELLOW]This ideology unlocks upon entering the Atomic Era, or unlocking 18 Policies and have advanced at least to the Industrial Era, whichever comes first.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_ORDER_HEADING3_BODY';

-- Autocracy
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Autocracy[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_AUTOCRACY_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'The Autocracy ideology is well suited for those wishing nothing more than to crush their foes under the weight of their iron-plated boots. [COLOR_YELLOW]This ideology unlocks upon entering the Atomic Era, or unlocking 18 Policies and having advanced at least to the Industrial Era, whichever comes first.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_AUTOCRACY_HEADING3_BODY';

-- Headings whose topics have mechanics changes
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Cities[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GAME_CONCEPT_SECTION_1';
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Combat Rules[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GAME_CONCEPT_SECTION_2';
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Resources[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GAME_CONCEPT_SECTION_4';
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Constructing Improvements[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GAME_CONCEPT_SECTION_5';
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Food and City Growth[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GAME_CONCEPT_SECTION_6';
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Culture[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GAME_CONCEPT_SECTION_8';
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Diplomacy[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GAME_CONCEPT_SECTION_9';
UPDATE Language_en_US
SET Text = '[COLOR_GREEN]Happiness[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GAME_CONCEPT_SECTION_10';
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Social Policies[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GAME_CONCEPT_SECTION_12';
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Gold and the Economy[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GAME_CONCEPT_SECTION_13';
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Great People and Specialists[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GAME_CONCEPT_SECTION_15';
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]City-States[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GAME_CONCEPT_SECTION_16';
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Ruins and Barbarians[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GAME_CONCEPT_SECTION_19';
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Units[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GAME_CONCEPT_SECTION_20';
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Victory[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GAME_CONCEPT_SECTION_21';