------------------------------------------------
-- Ranking popup
------------------------------------------------
UPDATE Language_en_US
SET Text = '{1_Name:textkey} has completed their greatest work, the list of:'
WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_TITLE';

UPDATE Language_en_US
SET Text = 'The Greatest Builders in History'
WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_WONDERS';

UPDATE Language_en_US
SET Text = 'The Most Prosperous Civilizations'
WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_FOOD';

UPDATE Language_en_US
SET Text = 'The Most Industrious People Today'
WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_PRODUCTION';

UPDATE Language_en_US
SET Text = 'The Wealthiest People in the World'
WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_GOLD';

UPDATE Language_en_US
SET Text = 'The Most Advanced People in History'
WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_SCIENCE';

UPDATE Language_en_US
SET Text = 'The Most Sophisticated Cultures'
WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_CULTURE';

UPDATE Language_en_US
SET Text = 'The Most Stable Nations in History'
WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_HAPPINESS';

UPDATE Language_en_US
SET Text = 'The Mightiest Rulers in History'
WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_POWER';

UPDATE Language_en_US
SET Text = 'The Most Influential Nations'
WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_CULTURAL_INFLUENCE';

UPDATE Language_en_US
SET Text = 'The Most Dazzling Cities'
WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_CITY_TOURISM';

------------------------------------------------
-- Combat simulator
------------------------------------------------
UPDATE Language_en_US
SET Text = 'Near Combat Bonus Tile'
WHERE Tag = 'TXT_KEY_EUPANEL_IMPROVEMENT_NEAR';

------------------------------------------------
-- Top panel
------------------------------------------------

-- Date
UPDATE Language_en_US
SET Text = 'Your scientists and theologians have introduced the wonders of the Maya calendar. When the current b''ak''tun ends, you will receive your choice of Great Person as part of the celebration.[NEWLINE][NEWLINE]The current Maya Long Count date is:[NEWLINE]{1_LongMayaCalendarString}[NEWLINE][NEWLINE]A b''ak''tun begins at Turn 33 and restarts at Turn 42/52/62/72/86/101/117/133/152/183/234/432 at Standard game speed.[NEWLINE][NEWLINE]({2_TraditionalDateString} in the rest of the world).'
WHERE Tag = 'TXT_KEY_MAYA_DATE_TOOLTIP';

-- Science
UPDATE Language_en_US
SET Text = 'Each ([ICON_PUPPET] non-Puppet) City you own will increase Technology costs by {1_Num}%.'
WHERE Tag = 'TXT_KEY_TP_TECH_CITY_COST';

UPDATE Language_en_US
SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] [ICON_RESEARCH] from Policies.'
WHERE Tag = 'TXT_KEY_TP_SCIENCE_FROM_HAPPINESS';

UPDATE Language_en_US
SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] [ICON_RESEARCH] from Scholasticism.'
WHERE Tag = 'TXT_KEY_TP_SCIENCE_FROM_MINORS';

-- Culture
UPDATE Language_en_US
SET Text = 'Each ([ICON_PUPPET] non-Puppet) City you own will increase Social Policy costs by {1_Num}%.'
WHERE Tag = 'TXT_KEY_TP_CULTURE_CITY_COST';

UPDATE Language_en_US
SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] from a temporary Cultural Boost (Turns left: {2_TurnsLeft}).'
WHERE Tag = 'TXT_KEY_TP_CULTURE_FROM_BONUS_TURNS';

UPDATE Language_en_US
SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] from Policies.'
WHERE Tag = 'TXT_KEY_TP_CULTURE_FROM_HAPPINESS';

UPDATE Language_en_US
SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] from your [ICON_GOLDEN_AGE] Golden Age.'
WHERE Tag = 'TXT_KEY_TP_CULTURE_FROM_GOLDEN_AGE';

-- Gold
UPDATE Language_en_US
SET Text = 'With [ICON_GOLD] Gold you may purchase land and units, or invest in buildings in your cities.'
WHERE Tag = 'TXT_KEY_TP_GOLD_EXPLANATION';

-- Faith
UPDATE Language_en_US
SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] from Religion.'
WHERE Tag = 'TXT_KEY_TP_FAITH_FROM_RELIGION';

-- Golden Age Point
UPDATE Language_en_US
SET Text = '[ICON_BULLET]{1_Num} is added per turn from positive [ICON_HAPPINESS_1] Happiness produced by Cities.'
WHERE Tag = 'TXT_KEY_TP_GOLDEN_AGE_ADDITION';

-- Happiness
UPDATE Language_en_US
SET Text = 'Your approval rating is at least 50%. [ICON_HAPPINESS_1] Happy Cities will [ICON_FOOD] Grow more quickly, but [ICON_HAPPINESS_3] Unhappy Cities will suffer penalties to [ICON_FOOD] Growth, Military Unit costs and Settler costs.'
WHERE Tag = 'TXT_KEY_TP_TOTAL_HAPPINESS';

UPDATE Language_en_US
SET Text = 'Your approval rating is less than 50%!'
WHERE Tag = 'TXT_KEY_TP_EMPIRE_UNHAPPY';

UPDATE Language_en_US
SET Text = 'Your approval rating is less than 35% - your Empire is in open rebellion! Uprisings may occur with rebel (barbarian) units appearing in your territory, and Cities may abandon your Empire and flip to the Civilization that is most culturally influential over your people!'
WHERE Tag = 'TXT_KEY_TP_EMPIRE_VERY_UNHAPPY';

UPDATE Language_en_US
SET Text = 'Your approval rating is less than 20% - your Empire is fracturing! Rebels (barbarians) will spawn more frequently and Cities will rapidly abandon your Empire and flip to the Civilization that is most culturally influential over your people!'
WHERE Tag = 'TXT_KEY_TP_EMPIRE_SUPER_UNHAPPY';

UPDATE Language_en_US
SET Text = 'Empire-Wide [ICON_HAPPINESS_1] Happiness Total: {1_Num}'
WHERE Tag = 'TXT_KEY_TP_HAPPINESS_SOURCES';

UPDATE Language_en_US
SET Text = '{1_Num} from Natural Wonders and Landmarks.'
WHERE Tag = 'TXT_KEY_TP_HAPPINESS_NATURAL_WONDERS';

UPDATE Language_en_US
SET Text = 'Empire-Wide [ICON_HAPPINESS_3] Unhappiness Total: {1_Num} (capped by City Population)'
WHERE Tag = 'TXT_KEY_TP_UNHAPPINESS_TOTAL';

UPDATE Language_en_US
SET Text = '{1_Num} from Public Opinion due to Ideological Pressure.'
WHERE Tag = 'TXT_KEY_TP_UNHAPPINESS_PUBLIC_OPINION';

UPDATE Language_en_US
SET Text = '{1_Num} from [ICON_CITIZEN] Population in [ICON_OCCUPIED] Occupied, [ICON_RESISTANCE] Resisting or [ICON_RAZING] Razing Cities.'
WHERE Tag = 'TXT_KEY_TP_UNHAPPINESS_OCCUPIED_POPULATION';

UPDATE Language_en_US
SET Text = '{1_Num : number "#.##"} from [ICON_PUPPET] Puppet Cities.'
WHERE Tag = 'TXT_KEY_TP_UNHAPPINESS_PUPPET_CITIES';

UPDATE Language_en_US
SET Text = '{1_Num : number "#.##"} from [ICON_URBANIZATION] Urbanization (Specialists).'
WHERE Tag = 'TXT_KEY_TP_UNHAPPINESS_SPECIALISTS';

-- Supply
UPDATE Language_en_US
SET Text = 'Your empire can support {1_Num} Units. You are over that limit by {2_Num}, which decreases [ICON_PRODUCTION] Production and [ICON_FOOD] Growth in your Cities by {3_Num}%.'
WHERE Tag = 'TXT_KEY_UNIT_SUPPLY_REACHED_TOOLTIP';

------------------------------------------------
-- City screen/banner
------------------------------------------------
UPDATE Language_en_US
SET Text = '[ICON_CULTURE] Culture is spent as [ICON_CULTURE_LOCAL] Border Growth in Cities to claim new territory, allowing them to be more productive. It is also spent on [COLOR_POSITIVE_TEXT]Social Policies[ENDCOLOR] at the empire-level.'
WHERE Tag = 'TXT_KEY_CULTURE_HELP_INFO';

UPDATE Language_en_US
SET Text = '{1_Num} {2_IconString} from Traits and Other Sources'
WHERE Tag = 'TXT_KEY_YIELD_FROM_MISC';

UPDATE Language_en_US
SET Text = '{1_Num} [ICON_TOURISM] Tourism from Theming Bonuses'
WHERE Tag = 'TXT_KEY_CO_CITY_TOURISM_THEMING_BONUSES';

UPDATE Language_en_US
SET Text = '[NEWLINE][ICON_BULLET][COLOR_WARNING_TEXT][ICON_HAPPINESS_3] Unhappiness Modifier: {1_Num}%[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_FOODMOD_UNHAPPY';

UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]-{1_Num}[ENDCOLOR] {2_IconString} eaten by [ICON_CITIZEN] Citizens'
WHERE Tag = 'TXT_KEY_YIELD_EATEN_BY_POP';

UPDATE Language_en_US
SET Text = 'City will be [ICON_RAZING] Razed to the ground in {1_Num} {1_Num: plural 1?Turn; other?Turns;}! While the City is being Razed, 100% of its [ICON_CITIZEN] Population will produce [ICON_HAPPINESS_3] Unhappiness, but Units within its borders will heal faster!'
WHERE Tag = 'TXT_KEY_CITY_BURNING';

UPDATE Language_en_US
SET Text = 'City is in [ICON_RESISTANCE] Resistance for {1_Num} {1_Num: plural 1?Turn; other?Turns;}! While in Resistance, a City cannot do anything useful, Units within its borders will heal slower, and 100% of its [ICON_CITIZEN] Population will produce [ICON_HAPPINESS_3] Unhappiness!'
WHERE Tag = 'TXT_KEY_CITY_RESISTANCE';

UPDATE Language_en_US
SET Text = 'City is [ICON_BLOCKADED] Blockaded by an enemy unit![NEWLINE][ICON_BULLET]Disables Hit Point recovery of the City.[NEWLINE][ICON_BULLET]Enemy Units gain [COLOR_POSITIVE_TEXT]+' || (SELECT Value FROM Defines WHERE Name = 'BLOCKADED_CITY_ATTACK_MODIFIER') || '%[ENDCOLOR] Combat Strength when attacking the City.'
WHERE Tag = 'TXT_KEY_CITY_BLOCKADED';

UPDATE Language_en_US
SET Text = '[ICON_GOLD] Gold spent on Buildings Invests in them, reducing their [ICON_PRODUCTION] Production cost by 50% (25% for Wonders).[NEWLINE][ICON_GOLD] Gold-purchased Units start with half of the XP of a [ICON_PRODUCTION] Produced Unit.[NEWLINE]Some Units and Buildings can be purchased with [ICON_PEACE] Faith (based on Belief and Policy selections).'
WHERE Tag = 'TXT_KEY_CITYVIEW_PURCHASE_TT';

UPDATE Language_en_US
SET Text = 'Laborers'
WHERE Tag = 'TXT_KEY_CITYVIEW_UNEMPLOYED_TEXT';

------------------------------------------------
-- Military overview
------------------------------------------------
UPDATE Language_en_US
SET Text = '[ICON_FOOD]/[ICON_PRODUCTION] Penalty'
WHERE Tag = 'TXT_KEY_SUPPLY_DEFICIT_PENALTY';

------------------------------------------------
-- Economic overview
------------------------------------------------
UPDATE Language_en_US
SET Text = 'Total cost of maintaining Roads and Railroads you have built, or are responsible for. Roads cost 1 [ICON_GOLD] Gold per turn, and Railroads cost 3 [ICON_GOLD].'
WHERE Tag = 'TXT_KEY_EO_EX_IMPROVEMENTS';

UPDATE Language_en_US
SET Text = 'Sort By Unhappiness'
WHERE Tag = 'TXT_KEY_EO_SORT_STRENGTH';

UPDATE Language_en_US
SET Text = '[ICON_CITIZEN] Citizens in non-occupied Cities do not produce [ICON_HAPPINESS_3] Unhappiness directly. See the tooltips below for the sources of [ICON_HAPPINESS_3] Unhappiness in each City'
WHERE Tag = 'TXT_KEY_POP_UNHAPPINESS_TT';

UPDATE Language_en_US
SET Text = 'Every (non-occupied) City produces 0 [ICON_HAPPINESS_3] Unhappiness.'
WHERE Tag = 'TXT_KEY_NUMBER_OF_CITIES_TT';

UPDATE Language_en_US
SET Text = 'Every (non-occupied) City produces 0 [ICON_HAPPINESS_3] Unhappiness (normally).'
WHERE Tag = 'TXT_KEY_NUMBER_OF_CITIES_TT_NORMALLY';

UPDATE Language_en_US
SET Text = 'Every [ICON_OCCUPIED] Occupied City produces 0 [ICON_HAPPINESS_3] Unhappiness.'
WHERE Tag = 'TXT_KEY_NUMBER_OF_OCCUPIED_CITIES_TT';

UPDATE Language_en_US
SET Text = 'Every [ICON_CITIZEN] Citizen in an [ICON_OCCUPIED] Occupied City produces 1 [ICON_HAPPINESS_3] Unhappiness'
WHERE Tag = 'TXT_KEY_OCCUPIED_POP_UNHAPPINESS_TT';

UPDATE Language_en_US
SET Text = '[ICON_PLUS] Total City Unhappiness'
WHERE Tag = 'TXT_KEY_EO_CITY_UNHAPPINESS';

UPDATE Language_en_US
SET Text = '[ICON_MINUS] Total City Unhappiness'
WHERE Tag = 'TXT_KEY_EO_CITY_UNHAPPINESS_COLLAPSE';

UPDATE Language_en_US
SET Text = '[ICON_PLUS] Total City Happiness'
WHERE Tag = 'TXT_KEY_EO_LOCAL_CITY';

UPDATE Language_en_US
SET Text = '[ICON_MINUS] Total City Happiness'
WHERE Tag = 'TXT_KEY_EO_LOCAL_CITY_COLLAPSE';

UPDATE Language_en_US
SET Text = '{1_Num} base [ICON_GOLD] Gold per Route[NEWLINE]{2_Num} [ICON_GOLD] Gold per [ICON_CAPITAL] Capital Citizen ({3_Num})[NEWLINE]{4_Num} [ICON_GOLD] Gold per [ICON_CITIZEN] Citizen'
WHERE Tag = 'TXT_KEY_TRADE_ROUTE_INCOME_INFO';

UPDATE Language_en_US
SET Text = '{1_Num}% [ICON_GOLD] Gold from [ICON_CONNECTED] City Connections'
WHERE Tag = 'TXT_KEY_EGI_TRADE_ROUTE_MOD_INFO';

------------------------------------------------
-- Cultural overview
------------------------------------------------

-- Tourism/happiness screen
UPDATE Language_en_US
SET Text = 'Public Opinion is calculated by comparing the [ICON_TOURISM] Influence of Civs with an Ideology to your own [ICON_TOURISM] Influence, but only if the other Civ is least [COLOR_MAGENTA]Popular[ENDCOLOR] with you, and the other Civ''s [ICON_TOURISM] Influence is higher than yours.[NEWLINE][NEWLINE]If both are true, a [COLOR_POSITIVE_TEXT]Content[ENDCOLOR] Public Opinion may cause [COLOR_NEGATIVE_TEXT]Ideological Pressure[ENDCOLOR], generating [ICON_HAPPINESS_4] Unhappiness. There are ways to combat this:[NEWLINE][ICON_BULLET] Ideologies with fewer followers have a natural resistance to Ideological Pressure.[NEWLINE][ICON_BULLET] Static or [COLOR_NEGATIVE_TEXT]Falling[ENDCOLOR] [ICON_TOURISM] Influence trends reduce a foreign Civ''s Ideological Pressure.[NEWLINE][ICON_BULLET] Adopting or switching Ideologies creates a 30-Turn (standard speed) period of reduced Ideological Pressure.[NEWLINE][NEWLINE]See the summary below to see what is causing Ideological Pressure. If the amount of icons for your Ideology is not greater than all the others combined, then [COLOR_NEGATIVE_TEXT]Ideological Pressure[ENDCOLOR] is generated![NEWLINE][NEWLINE]'
WHERE Tag = 'TXT_KEY_CO_OPINION_TT_OVERVIEW';

UPDATE Language_en_US
SET Text = '[ICON_RES_ARTIFACTS] Sites Left: {1_Num}'
WHERE Tag = 'TXT_KEY_ANTIQUITY_SITES_TO_EXCAVATE';

UPDATE Language_en_US
SET Text = '[ICON_RES_HIDDEN_ARTIFACTS] Hidden Sites Left: {1_Num}'
WHERE Tag = 'TXT_KEY_HIDDEN_SITES_TO_EXCAVATE';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]{1_Num}%[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CO_PUBLIC_OPINION_HAPPINESS';

UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]{1_Num}%[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CO_PUBLIC_OPINION_UNHAPPINESS';

UPDATE Language_en_US
SET Text = 'Current Empire-Wide [ICON_HAPPINESS_1] Happiness, from 0-100%[NEWLINE][NEWLINE]At 50% or below the civilization will start to lose cities to revolt and may change to the public''s Preferred Ideology (you can find that preference in the Public Opinion tooltip).'
WHERE Tag = 'TXT_KEY_CO_VICTORY_EXCESS_HAPPINESS_HEADER_TT';

-- CV progress screen
UPDATE Language_en_US
SET Text = '+{1_Num}% Bonus from Diplomat[NEWLINE]'
WHERE Tag = 'TXT_KEY_CO_PLAYER_TOURISM_PROPAGANDA';

UPDATE Language_en_US
SET Text = '[NEWLINE][NEWLINE][COLOR_CYAN]Influence Benefits:[ENDCOLOR][NEWLINE][COLOR_POSITIVE_TEXT]Trade Routes to this Civ Generate:[ENDCOLOR][NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]+10%[ENDCOLOR] [ICON_FOOD] Growth in Origin City[NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]+4[ENDCOLOR] [ICON_GOLD] Gold[NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]+4[ENDCOLOR] [ICON_RESEARCH] Science[NEWLINE][COLOR_POSITIVE_TEXT]Espionage Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET] [ICON_SPY] Spies Establish Surveillance in [COLOR_POSITIVE_TEXT]4[ENDCOLOR] Turns[NEWLINE][ICON_BULLET] [ICON_SPY] Spies generate [COLOR_POSITIVE_TEXT]+10[ENDCOLOR] Network Points per Turn[NEWLINE][COLOR_POSITIVE_TEXT]City Conquest Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]-30%[ENDCOLOR] [ICON_RESISTANCE] Unrest Time[NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]-30%[ENDCOLOR] [ICON_CITIZEN] Citizen loss from City conquest'
WHERE Tag = 'TXT_KEY_CO_INFLUENCE_BONUSES_FAMILIAR';

UPDATE Language_en_US
SET Text = '[NEWLINE][NEWLINE][COLOR_CYAN]Influence Benefits:[ENDCOLOR][NEWLINE][COLOR_POSITIVE_TEXT]Trade Routes to this Civ Generate:[ENDCOLOR][NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]+15%[ENDCOLOR] [ICON_FOOD] Growth in Origin City[NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]+6[ENDCOLOR] [ICON_GOLD] Gold[NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]+6[ENDCOLOR] [ICON_RESEARCH] Science[NEWLINE][COLOR_POSITIVE_TEXT]Espionage Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET] [ICON_SPY] Spies Establish Surveillance in [COLOR_POSITIVE_TEXT]3[ENDCOLOR] Turns[NEWLINE][ICON_BULLET] [ICON_SPY] Spies generate [COLOR_POSITIVE_TEXT]+20[ENDCOLOR] Network Points per Turn[NEWLINE][COLOR_POSITIVE_TEXT]City Conquest Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]-55%[ENDCOLOR] [ICON_RESISTANCE] Unrest Time[NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]-55%[ENDCOLOR] [ICON_CITIZEN] Citizen loss from City conquest'
WHERE Tag = 'TXT_KEY_CO_INFLUENCE_BONUSES_POPULAR';

UPDATE Language_en_US
SET Text = '[NEWLINE][NEWLINE][COLOR_CYAN]Influence Benefits:[ENDCOLOR][NEWLINE][COLOR_POSITIVE_TEXT]Trade Routes to this Civ Generate:[ENDCOLOR][NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]+20%[ENDCOLOR] [ICON_FOOD] Growth in Origin City[NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]+8[ENDCOLOR] [ICON_GOLD] Gold[NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]+8[ENDCOLOR] [ICON_RESEARCH] Science[NEWLINE][COLOR_POSITIVE_TEXT]Espionage Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET] [ICON_SPY] Spies Establish Surveillance in [COLOR_POSITIVE_TEXT]2[ENDCOLOR] Turns[NEWLINE][ICON_BULLET] [ICON_SPY] Spies generate [COLOR_POSITIVE_TEXT]+30[ENDCOLOR] Network Points per Turn[NEWLINE][COLOR_POSITIVE_TEXT]City Conquest Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]-80%[ENDCOLOR] [ICON_RESISTANCE] Unrest Time[NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]-80%[ENDCOLOR] [ICON_CITIZEN] Citizen loss from City conquest'
WHERE Tag = 'TXT_KEY_CO_INFLUENCE_BONUSES_INFLUENTIAL';

UPDATE Language_en_US
SET Text = '[NEWLINE][NEWLINE][COLOR_CYAN]Influence Benefits:[ENDCOLOR][NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Trade Routes to this Civ Generate:[ENDCOLOR][NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]+25%[ENDCOLOR] [ICON_FOOD] Growth in Origin City[NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]+10[ENDCOLOR] [ICON_GOLD] Gold[NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]+10[ENDCOLOR] [ICON_RESEARCH] Science[NEWLINE][COLOR_POSITIVE_TEXT]Espionage Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET] [ICON_SPY] Spies Establish Surveillance in [COLOR_POSITIVE_TEXT]1[ENDCOLOR] Turn[NEWLINE][ICON_BULLET] [ICON_SPY] Spies generate [COLOR_POSITIVE_TEXT]+30[ENDCOLOR] Network Points per Turn[NEWLINE][COLOR_POSITIVE_TEXT]City Conquest Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]-100%[ENDCOLOR] [ICON_RESISTANCE] Unrest Time[NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]-100%[ENDCOLOR] [ICON_CITIZEN] Citizen loss from City conquest'
WHERE Tag = 'TXT_KEY_CO_INFLUENCE_BONUSES_DOMINANT';

------------------------------------------------
-- Espionage overview
------------------------------------------------
UPDATE Language_en_US
SET Text = 'Name'
WHERE Tag = 'TXT_KEY_EO_NAME_RANK';

-- No ranks anymore in VP, so every Recruit is now an Agent
UPDATE Language_en_US
SET Text = 'Agent'
WHERE Tag = 'TXT_KEY_SPY_RANK_0';

UPDATE Language_en_US
SET Text = 'Security Level'
WHERE Tag = 'TXT_KEY_EO_POTENTIAL';

UPDATE Language_en_US
SET Text = 'Security Level reflects the difficulty of espionage in a city. The higher the value, the more time it will take for foreign spies to complete spy missions. To increase Security in a city, you can build espionage buildings, such as the Constabulary and the Police Station.[NEWLINE][NEWLINE]Click to sort cities by their Security level.'
WHERE Tag = 'TXT_KEY_EO_POTENTIAL_SORT_TT';

UPDATE Language_en_US
SET Text = 'The current Security Level of {1_CityName} is [COLOR_NEGATIVE_TEXT]{2_Num}[ENDCOLOR]. This reduces the number of Network Points foreign spies generate per turn by [COLOR_NEGATIVE_TEXT]{3_PercentReduction}%[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_EO_CITY_POTENTIAL_TT';

UPDATE Language_en_US
SET Text = 'The Security Level of {1_CityName} is [COLOR_POSITIVE_TEXT]{2_Security}[ENDCOLOR]. This reduces the number of Network Points foreign spies generate per turn by [COLOR_POSITIVE_TEXT]{3_PercentReduction}%[ENDCOLOR].[NEWLINE]{4_SecurityDetails}'
WHERE Tag = 'TXT_KEY_EO_OWN_CITY_POTENTIAL_TT';

UPDATE Language_en_US
SET Text = 'Spy Missions may not be conducted in City-States, so they do not have a Security Level.[NEWLINE][NEWLINE]Sending a spy into a City-State will increase your influence there by rigging elections. If there is more than one spy in the city, the highest ranked spy who has been in the City-State the longest has the greatest chance of rigging the election.'
WHERE Tag = 'TXT_KEY_EO_CITY_STATE_POTENTIAL_TT';

UPDATE Language_en_US
SET Text = 'The population of the city.[NEWLINE][NEWLINE]Click to sort cities by their population.'
WHERE Tag = 'TXT_KEY_EO_POPULATION_SORT_TT';

UPDATE Language_en_US
SET Text = '{1_RankName} {2_SpyName} is attempting to rig the election in {3_CityName} to increase our influence there.[NEWLINE][NEWLINE]Only one civilization may successfuly rig an election. If more than one spy is in a City-State, the highest ranked spy that has been in that City-State the longest has the greatest chance of successfully rigging the election in its favor. Streaks of consecutive successfully rigged elections provide more influence.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]If you successfully rig the next election, your influence will increase by {4_Influence}.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_EO_SPY_RIGGING_ELECTIONS_TT';

UPDATE Language_en_US
SET Text = '{1_RankName} {2_SpyName} is gathering intelligence in {3_CityName}. Each turn, they collect Network Points, and the accumulated Network Points unlock passive bonuses or can be spent to perform spy missions.'
WHERE Tag = 'TXT_KEY_EO_SPY_GATHERING_INTEL_TT';

UPDATE Language_en_US
SET Text = '{1_RankName} {2_SpyName} is schmoozing in {3_CityName} as a diplomat, providing a boost to the [ICON_TOURISM] Tourism output to the target civilization.[NEWLINE][NEWLINE]Once the World Congress convenes, you will be able to determine their opinion on proposals and trade for their support if needed. {1_RankName} {2_SpyName} is also collecting Network Points each turn, the accumulated Network Points allow you to gain additional information about the target civilization.'
WHERE Tag = 'TXT_KEY_SPY_STATE_SCHMOOZING_TT';

UPDATE Language_en_US
SET Text = '{1_RankName} {2_SpyName} is conducting counter-intelligence operations in {3_CityName}.'
WHERE Tag = 'TXT_KEY_EO_SPY_COUNTER_INTEL_TT';

UPDATE Language_en_US
SET Text = '{1_SpyRank} {2_SpyName} is in {3_CityName}. While {4_SpyRank} {5_SpyName} is in your city, they will perform counter-espionage operations to capture and kill any enemy spies that try to complete Spy Missions.'
WHERE Tag = 'TXT_KEY_CITY_SPY_YOUR_CITY_TT';

UPDATE Language_en_US
SET Text = '{1_SpyRank} {2_SpyName} is in {3_CityName}. While {4_SpyRank} {5_SpyName} is in the city, they establish surveillance and work towards completing Spy Missions. {6_SpyRank} {7_SpyName} will also inform you of any intrigue that they discover during their operations.'
WHERE Tag = 'TXT_KEY_CITY_SPY_OTHER_CIV_TT';

UPDATE Language_en_US
SET Text = 'Options for {1_SpyRank} {2_SpyName}:[NEWLINE][NEWLINE][ICON_BULLET] Move to a City-State and attempt to [COLOR_POSITIVE_TEXT]Rig an Election[ENDCOLOR].[NEWLINE][ICON_BULLET] Move to a City owned by a Major Civilization and Gather Intelligence to conduct a [COLOR_POSITIVE_TEXT]Spy Mission[ENDCOLOR].[NEWLINE][ICON_BULLET] Move to a Capital City owned by a Major Civilization and [COLOR_POSITIVE_TEXT]Schmooze[ENDCOLOR] as a Diplomat.'
WHERE Tag = 'TXT_KEY_EO_SPY_MOVE_TT';

UPDATE Language_en_US
SET Text = '{1_SpyName} has achieved the rank of {2_RankName}.[NEWLINE][NEWLINE]There are three spy ranks: Recruit, Agent, and Special Agent. Each subsequent level makes the spy more effective. A higher ranking spy will operate more effectively.[NEWLINE][NEWLINE]Spies level up when they successfully complete Spy Missions or if they uncover intrigue. They also get a small amount of Experience every turn while gathering intelligence, schmoozing as a Diplomat or conducting counterintelligence.'
WHERE Tag = 'TXT_KEY_EO_SPY_RANK_TT';

UPDATE Language_en_US
SET Text = '[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Spy is operating {1_SpyBonus} {1_SpyBonus: plural 1?Rank; other?Ranks;} higher than normal when rigging elections since this city state is allied to a civ over which you have strong cultural influence.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SPY_BONUS_CITY_STATE';

UPDATE Language_en_US
SET Text = 'Do you want this spy to be a diplomat? A spy can be a diplomat if they are placed in the capital of another civilization you are not at war with. (If war is declared, your spy will escape the city.) A diplomat provides access to information about the targeted civilization and may uncover intrigues. Once the World Congress convenes, you will be able to determine their opinion on proposals and trade for their support if needed. Diplomats automatically provide a boost to [ICON_TOURISM] Tourism output to the target civilization.'
WHERE Tag = 'TXT_KEY_SPY_BE_DIPLOMAT';

-- Button tooltip
UPDATE Language_en_US
SET Text = 'You have {1_Num: plural 1?a spy that is not; other?{1_Num} spies that are not;} assigned to tasks.[NEWLINE][NEWLINE]Spies can sabotage and steal from other civilizations, uncover other civilization''s plans through intrigue, and increase your influence over City-States by rigging elections. Assign spies in the Espionage Overview screen to have them start working!'
WHERE Tag = 'TXT_KEY_EO_UNASSIGNED_SPIES_TT';

------------------------------------------------
-- Victory overview
------------------------------------------------
UPDATE Language_en_US
SET Text = 'Victory Progress'
WHERE Tag = 'TXT_KEY_VP_TITLE';

UPDATE Language_en_US
SET Text = '{1_Team} needs [COLOR_POSITIVE_TEXT]{2_num}[ENDCOLOR] original [ICON_CAPITAL] Capitals/Vassals to win!'
WHERE Tag = 'TXT_KEY_VP_DIPLO_CAPITALS_TEAM_LEADING';

UPDATE Language_en_US
SET Text = 'Control [COLOR_POSITIVE_TEXT]{1_num}[ENDCOLOR] more original [ICON_CAPITAL] Capitals/Vassals to win!'
WHERE Tag = 'TXT_KEY_VP_DIPLO_CAPITALS_ACTIVE_PLAYER_LEADING';

UPDATE Language_en_US
SET Text = '{1_player} will need [COLOR_POSITIVE_TEXT]{2_num}[ENDCOLOR] original [ICON_CAPITAL] Capitals/Vassals to win!'
WHERE Tag = 'TXT_KEY_VP_DIPLO_CAPITALS_PLAYER_LEADING';

UPDATE Language_en_US
SET Text = 'An unmet player needs [COLOR_POSITIVE_TEXT]{1_num}[ENDCOLOR] original [ICON_CAPITAL] Capitals/Vassals to win!'
WHERE Tag = 'TXT_KEY_VP_DIPLO_CAPITALS_UNMET_PLAYER_LEADING';

UPDATE Language_en_US
SET Text = '{1_CivName} has attained Global Hegemony.'
WHERE Tag = 'TXT_KEY_VP_DIPLO_SOMEONE_WON';

------------------------------------------------
-- Demographics
------------------------------------------------
UPDATE Language_en_US
SET Text = 'Demographics'
WHERE Tag = 'TXT_KEY_DEMOGRAPHICS_TITLE';

------------------------------------------------
-- Diplomacy overview / player icon tooltip
------------------------------------------------
UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]At war with {1_enemy} (Warscore: {2_Num})[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_AT_WAR_WITH';

------------------------------------------------
-- City-State screen/tooltip
------------------------------------------------
UPDATE Language_en_US
SET Text = 'Diplomatic Missions'
WHERE Tag = 'TXT_KEY_POP_CSTATE_GIFT_GOLD';

UPDATE Language_en_US
SET Text = 'You may initiate a Diplomatic Mission that can improve your [ICON_INFLUENCE] Influence with this City-State.'
WHERE Tag = 'TXT_KEY_POP_CSTATE_GIFT_GOLD_TT';

UPDATE Language_en_US
SET Text = 'Pledging to protect a City-State lets the other major powers in the game know that you will protect the City-State from attacks and tribute demands.[NEWLINE][NEWLINE]During protection, your resting point for [ICON_INFLUENCE] Influence with this City-State is increased by {1_InfluenceMinimum}, and you will earn [COLOR_POSITIVE_TEXT]{3_InfluenceBoost}%[ENDCOLOR] more [ICON_INFLUENCE] Influence from quests completed for this City-State. The City-State [ICON_CAPITAL] Capital''s base [ICON_STRENGTH] Combat Strength will be increased by [COLOR_POSITIVE_TEXT]{4_DefenseBoost}%[ENDCOLOR], up to a global maximum of [COLOR_POSITIVE_TEXT]{5_DefenseTotal}%[ENDCOLOR].[NEWLINE][NEWLINE]Protection cannot be revoked until {2_TurnsMinimum} turns after the pledge is made.'
WHERE Tag = 'TXT_KEY_POP_CSTATE_PLEDGE_TT';

UPDATE Language_en_US
SET Text = '[NEWLINE][COLOR_WARNING_TEXT][ICON_BULLET] You must have {1_InfluenceNeededToPledge} or more [ICON_INFLUENCE] Influence to pledge.'
WHERE Tag = 'TXT_KEY_POP_CSTATE_PLEDGE_DISABLED_INFLUENCE_TT';

UPDATE Language_en_US
SET Text = '[NEWLINE][COLOR_WARNING_TEXT][ICON_BULLET] {1_TurnsUntilPledgeAvailable} turns must pass before you can pledge again.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_POP_CSTATE_PLEDGE_DISABLED_MISTRUST_TT';

UPDATE Language_en_US
SET Text = 'Each turn, your [ICON_INFLUENCE] Influence with them will change by {1_InfluenceRate}. It rests at {2_InfluenceAnchor}. If their Capital is damaged and you have a [COLOR_POSITIVE_TEXT]Pledge of Protection[ENDCOLOR] with them, this decay value will triple.'
WHERE Tag = 'TXT_KEY_CSTATE_INFLUENCE_RATE';

UPDATE Language_en_US
SET Text = 'You are [COLOR_POSITIVE_TEXT]Allies[ENDCOLOR] with {1_CityStateName:textkey}. Each turn, your [ICON_INFLUENCE] Influence with them will change by {2_Num}. If {1_CityStateName:textkey} is damaged and you have a [COLOR_POSITIVE_TEXT]Pledge of Protection[ENDCOLOR] with them, this decay value will triple.'
WHERE Tag = 'TXT_KEY_ALLIES_CSTATE_TT';

UPDATE Language_en_US
SET Text = '{1_CityStateName:textkey} neither likes nor dislikes you. You should send a Diplomatic Unit to increase your [ICON_INFLUENCE] Influence.'
WHERE Tag = 'TXT_KEY_NEUTRAL_CSTATE_TT';

UPDATE Language_en_US
SET Text = 'You have done something to anger {1_CityStateName:textkey}. Perhaps walking through their borders, or going to war recently.[NEWLINE][NEWLINE]Your [ICON_INFLUENCE] Influence with them recovers {2_Num} per turn. You should send a Diplomatic Unit to hasten the recovery of your [ICON_INFLUENCE] Influence.'
WHERE Tag = 'TXT_KEY_ANGRY_CSTATE_TT';

UPDATE Language_en_US
SET Text = 'They recognize your military strength, and will give you [COLOR_POSITIVE_TEXT]{1_Num}%[ENDCOLOR] of their total yields as tribute.'
WHERE Tag = 'TXT_KEY_CSTATE_CAN_BULLY';

UPDATE Language_en_US
SET Text = 'They do not fear you, and will resist tribute demands, as you are [COLOR_NEGATIVE_TEXT]{1_Num}%[ENDCOLOR] below the threshold.'
WHERE Tag = 'TXT_KEY_CSTATE_CANNOT_BULLY';

UPDATE Language_en_US
SET Text = 'City-States can be bullied into giving up large amounts of Yields, but using force in this way will decrease your [ICON_INFLUENCE] Influence over them.'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_TT';

UPDATE Language_en_US
SET Text = 'Take {1_GoldValue} [ICON_GOLD] Gold (-{2_Influence} [ICON_INFLUENCE])'
WHERE Tag = 'TXT_KEY_POPUP_MINOR_BULLY_GOLD_AMOUNT';

UPDATE Language_en_US
SET Text = 'Take {1_GoldValue} [ICON_GOLD] Gold and all Personal Quest Rewards (-{2_Influence} [ICON_INFLUENCE])'
WHERE Tag = 'TXT_KEY_POPUP_MINOR_BULLY_UNIT_AMOUNT';

UPDATE Language_en_US
SET Text = '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT](+{1_FactorScore}%) {@2_FactorDescription}[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_POSITIVE';

UPDATE Language_en_US
SET Text = '[NEWLINE][ICON_BULLET][COLOR_WARNING_TEXT]({1_FactorScore}%) {@2_FactorDescription}[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE';

UPDATE Language_en_US
SET Text = 'Has Pledges of Protection'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_PLEDGES_TO_PROTECT';

UPDATE Language_en_US
SET Text = 'Reclusive Personality'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_HOSTILE';

UPDATE Language_en_US
SET Text = 'Policy Modifier for Bullying'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_GUNBOAT_DIPLOMACY';

UPDATE Language_en_US
SET Text = 'They are currently [COLOR_POSITIVE_TEXT]afraid[ENDCOLOR], and are willing to give up [COLOR_POSITIVE_TEXT]{1_FactorScore}%[ENDCOLOR] of max tribute:'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_AFRAID';

UPDATE Language_en_US
SET Text = 'They are currently [COLOR_WARNING_TEXT]resilient[ENDCOLOR], because you are {1_FactorScore}% below the tribute threshold:'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_RESILIENT';

-- Personalities/Traits
UPDATE Language_en_US
SET Text = 'Affable', Gender = 'neuter:an'
WHERE Tag = 'TXT_KEY_CITY_STATE_PERSONALITY_FRIENDLY';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Affable[ENDCOLOR] City-States are more likely to request that you create a Road to them, start a Trade Route with them, and obtain Resources. They are also more likely to host diplomatic forums.'
	|| '[NEWLINE][NEWLINE]Quest Rewards Modifiers:'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]+' || (SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_FRIENDLY') || '%[ENDCOLOR] rewards from all [ICON_CITY_STATE] City-State Quests.'
WHERE Tag = 'TXT_KEY_CITY_STATE_PERSONALITY_FRIENDLY_TT';

UPDATE Language_en_US
SET Text = 'Reclusive'
WHERE Tag = 'TXT_KEY_CITY_STATE_PERSONALITY_HOSTILE';

UPDATE Language_en_US
SET Text = '[ICON_INFLUENCE] Influence with [COLOR_NEGATIVE_TEXT]Reclusive[ENDCOLOR] City-States drops more quickly, and they are more resilient to tribute demands. They are more likely to request that you conquer Cities, clear Encampments, and send them Units. They are also more likely to wage war against neighboring City-States.'
	|| '[NEWLINE][NEWLINE]Quest Rewards Modifiers:'
	|| '[NEWLINE][ICON_BULLET][COLOR_NEGATIVE_TEXT]' || (SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_HOSTILE') || '%[ENDCOLOR] rewards from all [ICON_CITY_STATE] City-State Quests.'
WHERE Tag = 'TXT_KEY_CITY_STATE_PERSONALITY_HOSTILE_TT';

UPDATE Language_en_US
SET Text = 'Enigmatic'
WHERE Tag = 'TXT_KEY_CITY_STATE_PERSONALITY_IRRATIONAL';

UPDATE Language_en_US
SET Text = 'Compared with other City-States, an [COLOR_POSITIVE_TEXT]Enigmatic[ENDCOLOR] City-State is more random with the requests it makes.'
	|| '[NEWLINE][NEWLINE]Quest Rewards Modifiers:'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_CULTURE_IRRATIONAL') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_CULTURE] Culture.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_TOURISM_IRRATIONAL') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_TOURISM] Tourism.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GAP_IRRATIONAL') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GOLDEN_AGE] Golden Age Points.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GPP_IRRATIONAL') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_PEOPLE] Great People Points in the [ICON_CAPITAL] Capital.'
WHERE Tag = 'TXT_KEY_CITY_STATE_PERSONALITY_IRRATIONAL_TT';

UPDATE Language_en_US
SET Text = 'Pragmatic'
WHERE Tag = 'TXT_KEY_CITY_STATE_PERSONALITY_NEUTRAL';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Pragmatic[ENDCOLOR] City-States are more likely to request that you Pledge to Protect them, bully other nearby City-States, and conduct espionage.'
	|| '[NEWLINE][NEWLINE]Quest Rewards Modifiers:'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_FOOD_NEUTRAL') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_FOOD] Food.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_CULTURE_NEUTRAL') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_CULTURE] Culture.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_FAITH_NEUTRAL') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_PEACE] Faith.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GPP_NEUTRAL') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_PEOPLE] Great People Points in the [ICON_CAPITAL] Capital.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GPP_GLOBAL_NEUTRAL') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_PEOPLE] Great People Points in all Cities.'
WHERE Tag = 'TXT_KEY_CITY_STATE_PERSONALITY_NEUTRAL_TT';

UPDATE Language_en_US
SET Text = Text
	|| '[NEWLINE][NEWLINE]Quest Rewards Modifiers:'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_CULTURE_CULTURED') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_CULTURE] Culture.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_TOURISM_CULTURED') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_TOURISM] Tourism.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GAP_CULTURED') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GOLDEN_AGE] Golden Age Points.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GENERAL_POINTS_CULTURED') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_GENERAL] Great General Points.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_ADMIRAL_POINTS_CULTURED') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_ADMIRAL] Great Admiral Points.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GPP_CULTURED') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_PEOPLE] Great People Points in the [ICON_CAPITAL] Capital.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GPP_GLOBAL_CULTURED') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_PEOPLE] Great People Points in all Cities.'
WHERE Tag = 'TXT_KEY_CITY_STATE_CULTURED_TT';

UPDATE Language_en_US
SET Text = Text
	|| '[NEWLINE][NEWLINE]Quest Rewards Modifiers:'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_FOOD_MARITIME') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_FOOD] Food.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GOLD_MARITIME') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GOLD] Gold.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_ADMIRAL_POINTS_MARITIME') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_ADMIRAL] Great Admiral Points.'
WHERE Tag = 'TXT_KEY_CITY_STATE_MARITIME_TT';

UPDATE Language_en_US
SET Text = Text
	|| '[NEWLINE][NEWLINE]Quest Rewards Modifiers:'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_FOOD_MERCANTILE') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_FOOD] Food.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GOLD_MERCANTILE') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_CITY_STATE_MERCANTILE_TT';

UPDATE Language_en_US
SET Text = Text
	|| '[NEWLINE][NEWLINE]Quest Rewards Modifiers:'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_PRODUCTION_MILITARISTIC') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_PRODUCTION] Production.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_TOURISM_MILITARISTIC') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_TOURISM] Tourism.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GENERAL_POINTS_MILITARISTIC') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_GENERAL] Great General Points.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_EXPERIENCE_MILITARISTIC') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] Experience.'
WHERE Tag = 'TXT_KEY_CITY_STATE_MILITARISTIC_TT';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CITY_STATE_MILITARISTIC_TT}'
WHERE Tag = 'TXT_KEY_CITY_STATE_MILITARISTIC_NO_UU_TT';

UPDATE Language_en_US
SET Text = Text
	|| '[NEWLINE][NEWLINE]Quest Rewards Modifiers:'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_SCIENCE_RELIGIOUS') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_RESEARCH] Science.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_FAITH_RELIGIOUS') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_PEACE] Faith.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GAP_RELIGIOUS') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GOLDEN_AGE] Golden Age Points.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GPP_GLOBAL_RELIGIOUS') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_PEOPLE] Great People Points in all Cities.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_EXPERIENCE_RELIGIOUS') / 100.0 AS NUMERIC) || 'x[ENDCOLOR] Experience.'
WHERE Tag = 'TXT_KEY_CITY_STATE_RELIGIOUS_TT';

-- Unit Spawn Counter
UPDATE Language_en_US
SET Text = 'You will receive a [COLOR_POSITIVE_TEXT]new Military Unit[ENDCOLOR] from them approximately every {1_Num} {1_Num: plural 1?Turn; other?Turns;}. A new Military Unit will arrive in [COLOR_POSITIVE_TEXT]{2_Num}[ENDCOLOR] {2_Num: plural 1?Turn; other?Turns;}.'
WHERE Tag = 'TXT_KEY_CSTATE_MILITARY_BONUS';

-- Quests
UPDATE Language_en_US
SET Text = 'They will reward the player(s) with the largest number of new Followers.'
WHERE Tag = 'TXT_KEY_CITY_STATE_QUEST_CONTEST_FAITH_FORMAL';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CITY_STATE_QUEST_CONTEST_FAITH_FORMAL} So far, you have the lead with [COLOR_POSITIVE_TEXT]{1_PlayerScore} {1_PlayerScore: plural 1?new Follower; other?new Followers;}'
WHERE Tag = 'TXT_KEY_CITY_STATE_QUEST_CONTEST_FAITH_WINNING_FORMAL';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CITY_STATE_QUEST_CONTEST_FAITH_FORMAL} So far, the leader has {1_LeaderScore} {1_LeaderScore: plural 1?new Follower; other?new Followers;} and you have [COLOR_POSITIVE_TEXT]{2_PlayerScore}[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CITY_STATE_QUEST_CONTEST_FAITH_LOSING_FORMAL';

UPDATE Language_en_US
SET Text = 'They want you to start a land or sea trade route to their city.'
WHERE Tag = 'TXT_KEY_CITY_STATE_QUEST_TRADE_ROUTE_FORMAL';

UPDATE Language_en_US
SET Text = 'Conquer {1_CityStateName:textkey} (full reward), or Ally both {1_CityStateName:textkey} and this City-State (partial reward).'
WHERE Tag = 'TXT_KEY_CITY_STATE_QUEST_KILL_CITY_STATE_FORMAL';

UPDATE Language_en_US
SET Text = 'War breaks out between {2_CivName:textkey} and {1_TargetName:textkey}'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SUMMARY_QUEST_KILL_CITY_STATE';

UPDATE Language_en_US
SET Text = '{3_TargetName:textkey} has declared war on {1_CivName:textkey}, citing diplomatic concerns. Receive the [COLOR_POSITIVE_TEXT]full[ENDCOLOR] Quest Reward from {1_TargetName:textkey} by [COLOR_NEGATIVE_TEXT]destroying[ENDCOLOR] {3_CivName:textkey}, or a [COLOR_POSITIVE_TEXT]partial[ENDCOLOR] Quest Reward through [COLOR_POSITIVE_TEXT]allying[ENDCOLOR] both City-States. International pressure will force peace in [COLOR_POSITIVE_TEXT]{2_Num}[ENDCOLOR] {2_Num: plural 1?Turn; other?Turns;}, so act fast if you intend to intervene!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_KILL_CITY_STATE';

UPDATE Language_en_US
SET Text = '{1_TargetName:textkey} defeated by {2_CivName:textkey}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_KILL_CITY_STATE';

UPDATE Language_en_US
SET Text = 'The war between {2_CivName:textkey} and {1_TargetName:textkey} has ended, with {2_CivName:textkey} emerging as the clear victor. Your advisors worry that this outcome may lead to future conflict.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_KILL_CITY_STATE';


------------------------------------------------
-- Advisor Counsel (suppresses them because we build XP buildings in every city)
------------------------------------------------
UPDATE AICityStrategies
SET Advisor = NULL
WHERE AdvisorCounsel = 'TXT_KEY_AICITYSTRATEGY_NEED_NAVAL_GROWTH' OR AdvisorCounsel = 'TXT_KEY_AICITYSTRATEGY_HAVE_TRAINING_FACILITY' OR AdvisorCounsel = 'TXT_KEY_AICITYSTRATEGY_ENOUGH_TILE_IMPROVERS' OR AdvisorCounsel = 'TXT_KEY_AICITYSTRATEGY_NEED_TILE_IMPROVERS' OR AdvisorCounsel = 'TXT_KEY_AICITYSTRATEGY_WANT_TILE_IMPROVERS';

------------------------------------------------
-- Tech tree
------------------------------------------------
UPDATE Language_en_US
SET Text = 'Allows Research Agreements (if enabled)'
WHERE Tag = 'TXT_KEY_ABLTY_R_PACT_STRING';

UPDATE Language_en_US
SET Text = 'Chopping Forests/Jungles: +{1_Num}[ICON_PRODUCTION] Production.'
WHERE Tag = 'TXT_KEY_ABLTY_TECH_BOOST_CHOP';

UPDATE Language_en_US
SET Text = 'Embarking/disembarking in owned Cities and Fortifications expends only 1 Movement.'
WHERE Tag = 'TXT_KEY_ABLTY_CITY_LESS_EMBARK_COST_STRING';

UPDATE Language_en_US
SET Text = 'Embarking/disembarking in owned Cities and Fortifications expends only 0.1 Movement.'
WHERE Tag = 'TXT_KEY_ABLTY_CITY_NO_EMBARK_COST_STRING';

UPDATE Language_en_US
SET Text = '[ICON_INTERNATIONAL_TRADE] Allows establishing an additional trade route.'
WHERE Tag = 'TXT_KEY_ADDITIONAL_INTERNATIONAL_TRADE_ROUTE';

UPDATE Language_en_US
SET Text = '+50% empire-wide [ICON_TOURISM] Tourism output.'
WHERE Tag = 'TXT_KEY_DOUBLE_TOURISM';

UPDATE Language_en_US
SET Text = '{1_Build}: {2_Turns}% {2_Turns: plural 1?Turn; other?Turns;} Required'
WHERE Tag = 'TXT_KEY_TECH_HELP_BUILD_REDUCTION';

------------------------------------------------
-- Policy tree
------------------------------------------------
UPDATE Language_en_US
SET Text = 'PROGRESS'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_LIBERTY_CAP';

UPDATE Language_en_US
SET Text = 'AUTHORITY'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_HONOR_CAP';

UPDATE Language_en_US
SET Text = 'FEALTY'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PIETY_CAP';

UPDATE Language_en_US
SET Text = 'STATECRAFT'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PATRONAGE_CAP';

UPDATE Language_en_US
SET Text = 'ARTISTRY'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_AESTHETICS_CAP';

UPDATE Language_en_US
SET Text = 'INDUSTRY'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_COMMERCE_CAP';

UPDATE Language_en_US
SET Text = 'IMPERIALISM'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_EXPLORATION_CAP';

UPDATE Language_en_US
SET Text = 'Ideological tenets may not be purchased until you have chosen an Ideology. This choice becomes available if you are in the Atomic Era, or have unlocked 18 Policies and have advanced at least to the Industrial Era, whichever comes first.'
WHERE Tag = 'TXT_KEY_POLICYSCREEN_IDEOLOGY_NOT_STARTED';

------------------------------------------------
-- Trade Route picker
------------------------------------------------
UPDATE Language_en_US
SET Text = 'Max Distance: {1_Num}'
WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_RANGE';

UPDATE Language_en_US
SET Text = 'Resource Diversity Modifier: {1_Num}%[NEWLINE][ICON_BULLET] Our Local Resources: {2_Num}[NEWLINE][ICON_BULLET] Their Local Resources: {3_Num}[NEWLINE]Routes to Cities with unique Resources earn more [ICON_GOLD] Gold, and Monopoly Resources count double.'
WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_RESOURCE_DIFFERENT';

UPDATE Language_en_US
SET Text = '{1_CivName} {1_CivName: plural 1?has; other?have;} discovered {2_Num} {2_Num: plural 1?Technology; other?Technologies;} that you do not know, granting you +{4_Num} [ICON_RESEARCH] Science.[NEWLINE]+{3_Num} [ICON_RESEARCH] Science on this route due to your Cultural Influence over them.'
WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_SCIENCE_EXPLAINED';

UPDATE Language_en_US
SET Text = 'You have discovered {1_Num} {1_Num: plural 1?Technology; other?Technologies;} that {2_CivName} {2_CivName: plural 1?does; other?do;} not know, granting them +{4_Num} [ICON_RESEARCH] Science.[NEWLINE]+{3_Num} [ICON_RESEARCH] Science on this route due to their Cultural Influence over you.'
WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_SCIENCE_EXPLAINED';

------------------------------------------------
-- Archaeological Dig picker
------------------------------------------------
UPDATE Language_en_US
SET Text = 'When constructed, a Landmark generates +3 Empire-Wide [ICON_HAPPINESS_1] Happiness for the builder and plot owner. When worked, a Landmark provides [ICON_CULTURE] Culture and [ICON_GOLD] Gold based on its age. +1 [ICON_CULTURE] Culture and [ICON_GOLD] Gold per Era older than this civ''s current Era. Archaeologist will be consumed.'
WHERE Tag = 'TXT_KEY_CHOOSE_ARCH_LANDMARK_RESULT';

UPDATE Language_en_US
SET Text = '[NEWLINE][COLOR_POSITIVE_TEXT]Creating a Landmark in another Civilization''s territory will generate +3 Empire-Wide [ICON_HAPPINESS_1] Happiness for the builder and plot owner, and give a temporary diplomatic boost with that nation.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CHOOSE_ARCH_LANDMARK_MAJOR_CIV';

UPDATE Language_en_US
SET Text = '[NEWLINE][COLOR_POSITIVE_TEXT]Creating a Landmark in a City-State''s territory will generate +3 Empire-Wide [ICON_HAPPINESS_1] Happiness for you, and will give you 10 resting [ICON_INFLUENCE] Influence with that City-State, scaling with Era.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CHOOSE_ARCH_LANDMARK_MINOR_CIV';

UPDATE Language_en_US
SET Text = 'Artifact will be placed in nearest Great Work of Art or Artifact slot. Artifact provides +' || (SELECT Value FROM Defines WHERE Name = 'BASE_CULTURE_PER_GREAT_WORK') || ' [ICON_CULTURE] Culture and +' || (SELECT Value FROM Defines WHERE Name = 'BASE_TOURISM_PER_GREAT_WORK') || ' [ICON_TOURISM] Tourism. Archaeologist will be consumed.'
WHERE Tag = 'TXT_KEY_CHOOSE_ARCH_ARTIFACT_RESULT';

UPDATE Language_en_US
SET Text = 'Ancient writing will be placed in nearest Great Work of Writing slot. Writing provides +' || (SELECT Value FROM Defines WHERE Name = 'BASE_CULTURE_PER_GREAT_WORK') || ' [ICON_CULTURE] Culture and +' || (SELECT Value FROM Defines WHERE Name = 'BASE_TOURISM_PER_GREAT_WORK') || ' [ICON_TOURISM] Tourism. Archaeologist will be consumed.'
WHERE Tag = 'TXT_KEY_CHOOSE_ARCH_WRITTEN_ARTIFACT_RESULT';

------------------------------------------------
-- Conquest picker
------------------------------------------------
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Annexing[ENDCOLOR] causes the following:[NEWLINE][ICON_BULLET] Produces [ICON_HAPPINESS_4] Unhappiness equal to the City''s [ICON_CITIZEN] Population (until you construct a Courthouse).[NEWLINE][ICON_BULLET] Invests in a Courthouse in the City, reducing its construction time by at least 50% (if annexed immediately after conquest).[NEWLINE][ICON_BULLET] Halves City [ICON_RESISTANCE] Resistance after capture (if annexed immediately after conquest).'
WHERE Tag = 'TXT_KEY_POPUP_CITY_CAPTURE_INFO_ANNEX';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Puppeting[ENDCOLOR] causes the following:[NEWLINE][ICON_BULLET] Generates 80% less Gold, Science, Culture, Tourism, and Faith than a normal city.[NEWLINE][ICON_BULLET] No longer produces Great Person Points.[NEWLINE][ICON_BULLET] You [COLOR_POSITIVE_TEXT]cannot choose what it produces[ENDCOLOR] or spend Gold in the City.[NEWLINE][ICON_BULLET] Generates 1 [ICON_HAPPINESS_3] Unhappiness for every 4 [ICON_CITIZEN] Citizens in the City.[NEWLINE][ICON_BULLET] Does not increase the cost of Social Policies, Technologies, or Tourism.[NEWLINE][NEWLINE]If you choose this, you may later Annex the City at any point.'
WHERE Tag = 'TXT_KEY_POPUP_CITY_CAPTURE_INFO_PUPPET';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Razing[ENDCOLOR] causes the following:[NEWLINE][ICON_BULLET] City loses [ICON_CITIZEN] Population each turn until it reaches 0 population and is removed from the game.[NEWLINE][ICON_BULLET] Produces [ICON_HAPPINESS_4] Unhappiness equal to the City''s [ICON_CITIZEN] Population.[NEWLINE][ICON_BULLET] Increases your [COLOR_POSITIVE_TEXT]War Score[ENDCOLOR] every time Population is reduced.[NEWLINE][ICON_BULLET] Friendly units within city borders heal as if inside the city (looting).[NEWLINE][ICON_BULLET] May generate [COLOR_NEGATIVE_TEXT]Partisans[ENDCOLOR] loyal to the former owner (if you are still at war with this player) or barbarians each turn while razing.'
WHERE Tag = 'TXT_KEY_POPUP_CITY_CAPTURE_INFO_RAZE';

UPDATE Language_en_US
SET Text = 'This City was formerly owned by [COLOR_POSITIVE_TEXT]{@1_PlayerName}[ENDCOLOR]. You have the opportunity to return it to them, for which they would be eternally grateful. Doing so will make this player your [COLOR_POSITIVE_TEXT]Voluntary Vassal[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_POPUP_CITY_CAPTURE_INFO_LIBERATE_RESURRECT';

------------------------------------------------
-- Trade screen
------------------------------------------------
UPDATE Language_en_US
SET Text = 'In the next World Congress session, [COLOR_POSITIVE_TEXT]{1_NumVotes}[ENDCOLOR] {1_NumVotes: plural 1?Delegate; other?Delegates;} currently controlled by this player will support {2_ChoiceText} on the proposal to [COLOR_POSITIVE_TEXT]enact[ENDCOLOR] these changes:[NEWLINE][NEWLINE]{3_ProposalText}[NEWLINE][NEWLINE]The number of delegates above is based on the number of delegates this player controls and the number of Network Points your [ICON_DIPLOMAT] Diplomat has gathered.'
WHERE Tag = 'TXT_KEY_DIPLO_VOTE_TRADE_ENACT_TT';

UPDATE Language_en_US
SET Text = 'In the next World Congress session, [COLOR_POSITIVE_TEXT]{1_NumVotes}[ENDCOLOR] {1_NumVotes: plural 1?Delegate; other?Delegates;} currently controlled by this player will support {2_ChoiceText} on the proposal to [COLOR_WARNING_TEXT]repeal[ENDCOLOR] these effects:[NEWLINE][NEWLINE]{3_ProposalText}[NEWLINE][NEWLINE]The number of delegates above is based on the number of delegates this player controls and the number of Network Points your [ICON_DIPLOMAT] Diplomat has gathered.'
WHERE Tag = 'TXT_KEY_DIPLO_VOTE_TRADE_REPEAL_TT';

UPDATE Language_en_US
SET Text = 'Allows the other player''s military or civilian Units to pass through one''s territory (lasts {1_Num} turns).[NEWLINE][NEWLINE]Allowing another civilization to pass through one''s own territory increases the [ICON_TOURISM] Tourism modifier towards that civilization.[NEWLINE][NEWLINE]Note: Military Units belonging to different players may never stack.'
WHERE Tag = 'TXT_KEY_DIPLO_OPEN_BORDERS_TT';

------------------------------------------------
-- World Congress screen
------------------------------------------------
UPDATE Language_en_US
SET Text = 'Turns until attempt at Global Hegemony: [COLOR_POSITIVE_TEXT]{1_DelegatesForWin}[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_WORLD_LEADER_INFO_SESSION';

UPDATE Language_en_US
SET Text = 'Delegates needed for Global Hegemony: [COLOR_POSITIVE_TEXT]{1_DelegatesForWin}[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_WORLD_LEADER_INFO_VOTES';

UPDATE Language_en_US
SET Text = 'Since the World Congress has moved to the newly-constructed United Nations, Diplomatic Victory is now possible. To enable the Diplomatic Victory special sessions, a [COLOR_POSITIVE_TEXT]World Ideology Resolution[ENDCOLOR] must be active. Once active, a Civilization must receive the support of {1_DelegatesForWin} or more Delegates on a Global Hegemony proposal. This number is based on the number of Civilizations and City-States in the game.[NEWLINE][NEWLINE]The Global Hegemony proposal is made to the Congress every other session once a [COLOR_POSITIVE_TEXT]World Ideology Resolution[ENDCOLOR] is active, alternating with regular sessions of regular proposals.'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_WORLD_LEADER_INFO_TT';

UPDATE Language_en_US
SET Text = '[NEWLINE][ICON_BULLET]{1_NumVotes} from previous attempts at Global Hegemony'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_EXTRA_VOTES';

UPDATE Language_en_US
SET Text = 'Sanctions placed on City-States'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_EMBARGO_CITY_STATES';

UPDATE Language_en_US
SET Text = 'Sanctions placed on: {1_CivsList}'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_EMBARGO_PLAYERS';

UPDATE Language_en_US
SET Text = '+{1_UnitMaintenancePercent}% [ICON_GOLD] Gold cost for Unit Maintenance. ' || CAST((SELECT Value FROM Defines WHERE Name = 'WARMONGER_THREAT_ATTACKED_WEIGHT_WORLD_PEACE') / 100.0 AS NUMERIC) || 'x [ICON_VICTORY_DOMINATION] Warmonger penalties, ' || CAST((SELECT Value FROM Defines WHERE Name = 'WARMONGER_THREAT_PER_TURN_DECAY_DECREASED') / 100.0 AS NUMERIC) || 'x [ICON_VICTORY_DOMINATION] Warmonger scores decay'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_UNIT_MAINTENANCE';

UPDATE Language_en_US
SET Text = '-{1_ResearchCostPercent}% [ICON_RESEARCH] Research cost for Technologies already discovered by another Civilization. Additional -2% for every City-State ally, up to -50%'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_DISCOVERED_TECH_MODIFIER';

UPDATE Language_en_US
SET Text = '+{1_CulturePerWorldWonder} [ICON_CULTURE] Culture from World Wonders, +1 [ICON_RESEARCH] Science from Great Works'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_CULTURE_FROM_WONDERS';

UPDATE Language_en_US
SET Text = '+{1_CulturePerWorldWonder} to yields from Natural Wonders'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_CULTURE_FROM_NATURAL_WONDERS';

UPDATE Language_en_US
SET Text = '+{1_NumCulture} [ICON_FOOD]/[ICON_PRODUCTION]/[ICON_GOLD] from Great Person improvements'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_GREAT_PERSON_TILE_IMPROVEMENT_CULTURE';

UPDATE Language_en_US
SET Text = '+{1_NumCulture} [ICON_RESEARCH]/[ICON_CULTURE]/[ICON_PEACE] from Landmarks'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_LANDMARK_CULTURE';

-- Button tooltip
UPDATE Language_en_US
SET Text = 'Diplomacy[NEWLINE][NEWLINE]Turns until the United Nations convenes: {1_TurnsUntilSession}[NEWLINE][NEWLINE]Turns until the next [COLOR_POSITIVE_TEXT]Global Hegemony[ENDCOLOR] proposal: {2_TurnsUntilVictorySession}[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Diplomatic Victory[ENDCOLOR] is now possible, if a [COLOR_POSITIVE_TEXT]World Ideology Resolution[ENDCOLOR] is active.'
WHERE Tag = 'TXT_KEY_EO_DIPLOMACY_AND_VICTORY_SESSION';

------------------------------------------------
-- Era advance popup
------------------------------------------------
UPDATE Language_en_US
SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]Global Hegemony[ENDCOLOR] proposal on alternating sessions if the [COLOR_POSITIVE_TEXT]United Nations[ENDCOLOR] have been built and a [COLOR_POSITIVE_TEXT]World Ideology Resolution[ENDCOLOR] is active'
WHERE Tag = 'TXT_KEY_LEAGUE_SPLASH_MESSAGE_ERA_DIPLO_VICTORY_POSSIBLE';

------------------------------------------------
-- Toast
------------------------------------------------
UPDATE Language_en_US
SET Text = 'You have gained {1_Num} [ICON_GOLD] Gold and {2_Num} [ICON_INFLUENCE] Influence from the Diplomatic Mission!'
WHERE Tag = 'TXT_KEY_MERCHANT_RESULT';

------------------------------------------------
-- Minimap panel
------------------------------------------------
UPDATE Language_en_US
SET Text = 'Center map on previous item'
WHERE Tag = 'TXT_KEY_PREVIOUS_ITEM_TT';

UPDATE Language_en_US
SET Text = 'Center map on next item'
WHERE Tag = 'TXT_KEY_NEXT_ITEM_TT';

------------------------------------------------
-- Civilopedia
------------------------------------------------
UPDATE Language_en_US
SET Text = 'Welcome to the Civilopedia! Here you will find detailed descriptions of all aspects of the game. Use the "Search" field to search for articles on any specific subject. Clicking on the tabs on the top edge of the screen will take you to the various major sections of the Civilopedia. The Navigation Bar on the left side of the screen will display the various entries within a section. Click on an entry to go directly there.[NEWLINE][NEWLINE]In the upper left-hand portion of the screen you will find the forward and back buttons which will help you navigate between pages. Click on the "X" on the upper right portion of the screen to return to the game.[NEWLINE][NEWLINE]Welcome to Vox Populi, a mod that completely overhauls the game by rebalancing policies, leaders, buildings, wonders, and more! [COLOR_YELLOW]Game Concepts that have been modified in Vox Populi are highlighted in yellow in the Civilopedia.[ENDCOLOR] [COLOR_GREEN]Entirely new Game Concepts or Game Concepts that have been completely overhauled are highlighted in green[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PEDIA_HOME_PAGE_HELP_TEXT';

UPDATE Language_en_US
SET Text = 'Civilization V examines all of human history - from the deep past to the day after tomorrow. The "Game Concepts" portion of the Civilopedia explains some of the more important parts of the game - how to build and manage cities, how to fight wars, how to research technology, and so forth. The left Navigation Bar displays the major concepts; click on an entry to see the subsections within the concepts.[NEWLINE][NEWLINE][COLOR_YELLOW]Game Concepts that have been modified in Vox Populi are highlighted in yellow.[ENDCOLOR] [COLOR_GREEN]Entirely new Game Concepts or Game Concepts that have been completely overhauled are highlighted in green[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PEDIA_GAME_CONCEPT_HELP_TEXT';

UPDATE Language_en_US
SET Text = 'Time and again throughout history, there have arisen men and women who have profoundly changed the world around them - artists, scientists, generals, merchants and others, whose genius sets them head and shoulders above the rest. In this game such visionaries are called "Great People."[NEWLINE][NEWLINE]There are five different kinds of great people in the game: Great Merchants, Great Artists, Great Scientists, Great Engineers, and Great Generals (a sixth, the Great Diplomat, is added in CSD). Each has a special ability.[NEWLINE][NEWLINE]Your civilization acquires Great People by constructing certain buildings and wonders, and then staffing them with "specialists", citizens from your cities who have given up working in the field or the mines. While specialists do not work city tiles, they do greatly speed the arrival of Great People. Balancing the need for food and the desire for Great People is an important challenge of city management.[NEWLINE][NEWLINE]{TXT_KEY_SPECIALISTSANDGP_HEADING1_BODY}'
WHERE Tag = 'TXT_KEY_PEDIA_SPEC_HELP_TEXT';

UPDATE Language_en_US
SET Text = 'Wonders are the remarkable, one-of-a-kind buildings that ensure that a civilization will be remembered throughout all of history. Wonders engage the mind and lift the spirits.[NEWLINE][NEWLINE]The Pyramids, Notre Dame Cathedral, and Stonehenge are examples of wonders. Wonders require much time and energy from your cities to construct, but once completed, they provide your civilization with many benefits.[NEWLINE][NEWLINE]There are three basic types of wonders: World Wonders, National Wonders and Project Wonders. Only one copy of a World Wonder may be constructed anywhere in the world in a given game. National Wonders are less exclusive: each nation may construct one (but only one) copy of a National Wonder.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]World Wonder Production Modifiers[ENDCOLOR]: In addition to certain Traits and Policies, [ICON_RES_MARBLE] Marble and [ICON_RES_STONE] Stone increase the production modifier of cities with these resources nearby. Marble increases Wonder production by 15% for all pre-Industrial Wonders, whereas Stone increases Wonder production by 10% for all pre-Medieval Wonders.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]World Wonder Production Costs[ENDCOLOR]: For every Wonder you control, the cost of future Wonders goes up. This cost varies based on the Era of the Wonder.[NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]For every owned Wonder of the same Era: 25%.[NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]For every owned Wonder from the previous Era: 15%.[NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]For every owned Wonder from two Eras prior: 10% (Wonders earlier than two eras do not count).[NEWLINE][NEWLINE]If you build too many Wonders during an Era, your ability to gain future Wonders will be compromised, so don''t be too greedy!'
WHERE Tag = 'TXT_KEY_PEDIA_WONDERS_HELP_TEXT';

UPDATE Language_en_US
SET Text = 'During a game, you will create "workers" - non-military units who will "improve" the land around your cities, increasing the land''s productivity or providing access to a nearby "resource." Improvements include farms, trading posts, lumber mills, quarries, mines, and more. During wartime your enemy may "pillage" (destroy) your improvements. Pillaged improvements are ineffective until a worker has "repaired" them.[NEWLINE][NEWLINE]{TXT_KEY_BUILDINGS_SPECIALISTS_HEADING2_BODY}'
WHERE Tag = 'TXT_KEY_PEDIA_IMPROVEMENT_HELP_TEXT';
