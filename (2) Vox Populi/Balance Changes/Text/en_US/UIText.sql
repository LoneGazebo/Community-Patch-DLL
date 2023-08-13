-- Misc text update
-- Puppet TT TopPanel
UPDATE Language_en_US
SET Text = '{1_Num} {2_IconString} from Traits and Other Sources'
WHERE Tag = 'TXT_KEY_YIELD_FROM_MISC';

-- Puppet TT TopPanel
UPDATE Language_en_US
SET Text = 'Each ([ICON_PUPPET] non-Puppet) City you own will increase Technology costs by {1_Num}%.'
WHERE Tag = 'TXT_KEY_TP_TECH_CITY_COST';

-- Puppet TT TopPanel
UPDATE Language_en_US
SET Text = 'Each ([ICON_PUPPET] non-Puppet) City you own will increase Social Policy costs by {1_Num}%.'
WHERE Tag = 'TXT_KEY_TP_CULTURE_CITY_COST';

-- Supply Info
UPDATE Language_en_US
SET Text = 'Your empire can support {1_Num} Units. You are over that limit by {2_Num}, which decreases [ICON_PRODUCTION] Production and [ICON_FOOD] Growth in your Cities by {3_Num}%.'
WHERE Tag = 'TXT_KEY_UNIT_SUPPLY_REACHED_TOOLTIP';

UPDATE Language_en_US
SET Text = '[ICON_FOOD]/[ICON_PRODUCTION] Penalty'
WHERE Tag = 'TXT_KEY_SUPPLY_DEFICIT_PENALTY';

-- Fortifications
UPDATE Language_en_US
SET Text = 'Many units have the ability to "fortify." This means that the unit "digs in" and creates defensive works in its current location. This gives the unit certain defensive bonuses, making it much tougher to kill. However, fortifications are strictly defensive: if the unit moves or attacks, the fortifications are destroyed.  While fortified, a unit will not activate. It will remain inactive until you manually activate it by clicking on the unit.[NEWLINE][NEWLINE]An improvement may contain [COLOR_POSITIVE_TEXT]Fortifications[ENDCOLOR] which means units stationed on this improvement will follow the behavior as a city garrison of not following up after a victory in combat.'
WHERE Tag = 'TXT_KEY_COMBAT_FORTIFICATION_HEADING3_BODY';

-- Wonders
UPDATE Language_en_US
SET Text = 'Wonders are the remarkable, one-of-a-kind buildings that ensure that a civilization will be remembered throughout all of history. Wonders engage the mind and lift the spirits.[NEWLINE][NEWLINE]The Pyramids, Notre Dame Cathedral, and Stonehenge are examples of wonders. Wonders require much time and energy from your cities to construct, but once completed, they provide your civilization with many benefits.[NEWLINE][NEWLINE]There are three basic types of wonders: World Wonders, National Wonders and Project Wonders. Only one copy of a World Wonder may be constructed anywhere in the world in a given game. National Wonders are less exclusive: each nation may construct one (but only one) copy of a National Wonder.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]World Wonder Production Modifiers[ENDCOLOR]: In addition to certain Traits and Policies, [ICON_RES_MARBLE] Marble and [ICON_RES_STONE] Stone increase the production modifier of cities with these resources nearby. Marble increases Wonder production by 15% for all pre-Industrial Wonders, whereas Stone increases Wonder production by 10% for all pre-Medieval Wonders.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]World Wonder Production Costs[ENDCOLOR]: For every Wonder you control, the cost of future Wonders goes up. This cost varies based on the Era of the Wonder.[NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]For every owned Wonder of the same Era: 25%. [NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]For every owned Wonder from the previous Era: 15%. [NEWLINE][ICON_BULLET] [COLOR_POSITIVE_TEXT]For every owned Wonder from two Eras prior: 10% (Wonders earlier than two eras do not count).[NEWLINE][NEWLINE]If you build too many Wonders during an Era, your ability to gain future Wonders will be compromised, so don''t be too greedy!'
WHERE Tag = 'TXT_KEY_PEDIA_WONDERS_HELP_TEXT';

-- Barb CS

UPDATE Language_en_US
SET Text = 'You have killed a group of Barbarians near {1_CivName:textkey}! They are grateful, and your [ICON_INFLUENCE] Influence with them has increased by 15!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_BARB_KILLED';

-- Culture Victory changes

UPDATE Language_en_US
SET Text = 'To win a Cultural Victory, [COLOR_YELLOW]you must construct the Citizen Earth Protocol wonder. This is possible if you are [ENDCOLOR][COLOR_MAGENTA]Influential[ENDCOLOR][COLOR_YELLOW] over every other civilization in the game. In addition, you must have two Tier 3 Tenets of an Ideology and your people must be Content[ENDCOLOR]. See the section on Victory for details.'
WHERE Tag = 'TXT_KEY_CULTURE_VICTORY_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Cultural Victory[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CULTURE_VICTORY_HEADING2_TITLE';

UPDATE Language_en_US
SET Text = 'You no longer win a Cultural Victory by fully exploring five Social Policy branches. Instead, you must become the dominant cultural influence in every civilization in the game using Tourism. [COLOR_YELLOW]You must also have two Tier 3 Tenets of an Ideology, your people must be Content, and the Citizen Earth Protocol wonder must be constructed. Once these requirements are met, you can construct the Citizen Earth Protocol to win this victory.[ENDCOLOR] See the section on Victory for more information.'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_CULTUREVICTORY_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Cultural Victory[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SOCIALPOLICY_CULTUREVICTORY_HEADING2_TITLE';

UPDATE Language_en_US
SET Text = 'To win a Cultural Victory, you must become the dominant cultural influence in every civilization in the game [COLOR_YELLOW]and construct the Citizen Earth Protocol. You must also have two Tier 3 Tenets of an Ideology, and your people must be Content[ENDCOLOR]. Cultural Influence is achieved through Tourism generated by Great Works that you create, and artifacts that you discover. The Tourism you generate has a constant impact on other civilizations, and its effect is amplified through Open Borders, Trade Routes, shared Religion and Ideologies, Research Agreements, etc.[NEWLINE][NEWLINE]When your cumulative Tourism output (for the entire game) surpasses the cumulative Culture output of each remaining civilization in the game, [COLOR_YELLOW]you will be able to construct the Citizen Earth Protocol if you also have an ideology and your population is Content[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_VICTORY_CULTURAL_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Cultural Victory[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CULTURE_VICTORY_HEADING3_TITLE';

UPDATE Language_en_US
SET Text = 'If you have [COLOR_POSITIVE_TEXT]two[ENDCOLOR] Tier 3 Tenets of an Ideology, and your population is Content, you only need to become Influential with 1 more civilization to be eligible to construct the Citizen Earth Protocol and win a Culture Victory!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_ONE_ACTIVE_PLAYER_TT';

UPDATE Language_en_US
SET Text = 'If they have [COLOR_POSITIVE_TEXT]two[ENDCOLOR] Tier 3 Tenets of an Ideology, and their population is Content, {1_CivName} only needs their culture to become Influential with 1 more civilization to construct the Citizen Earth Protocol and win a Culture Victory!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_ONE_TT';

UPDATE Language_en_US
SET Text = 'If they have an Ideology, and their population is Content, an unmet civilization only needs their culture to become Influential with 1 more civilization to construct the Citizen Earth Protocol and win a Culture Victory!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_ONE_UNMET_TT';

UPDATE Language_en_US
SET Text = 'If you have [COLOR_POSITIVE_TEXT]two[ENDCOLOR] Tier 3 Tenets of an Ideology, and your population is Content, you only need to become Influential with 2 more civilizations to construct the Citizen Earth Protocol and win a Culture Victory.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO_ACTIVE_PLAYER_TT';

UPDATE Language_en_US
SET Text = 'If they have [COLOR_POSITIVE_TEXT]two[ENDCOLOR] Tier 3 Tenets of an Ideology, and their population is Content, {1_CivName} only needs their culture to become Influential with 2 more civilizations to construct the Citizen Earth Protocol and win a Culture Victory.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO_TT';

UPDATE Language_en_US
SET Text = 'If they have [COLOR_POSITIVE_TEXT]two[ENDCOLOR] Tier 3 Tenets of an Ideology, and their population is Content, an unmet civilization only needs their culture to become Influential with 2 more civilizations to construct the Citizen Earth Protocol and win a Culture Victory.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO_UNMET_TT';

-- Healing

UPDATE Language_en_US
SET Text = 'To heal damage, a unit must remain inactive for a turn. The amount of damage that a unit heals depends upon the unit''s location.[NEWLINE][NEWLINE]In a Friendly City: A unit heals [COLOR_YELLOW]20[ENDCOLOR] HPs per turn.[NEWLINE]In Friendly Territory: [COLOR_YELLOW]15[ENDCOLOR] HP per turn.[NEWLINE]In Neutral Territory: [COLOR_YELLOW]10[ENDCOLOR] HP per turn.[NEWLINE]In Enemy Territory: [COLOR_YELLOW]5[ENDCOLOR] HP per turn.[NEWLINE][NEWLINE][COLOR_YELLOW]Units within the borders of a City that is in Resistance heal only 5HP per turn. Units within the borders of a City that is being razed heal 20HP per turn (looting).[ENDCOLOR][NEWLINE][NEWLINE]Note that certain promotions will accelerate a unit''s healing rate.'
WHERE Tag = 'TXT_KEY_COMBAT_HEALTINGDAMAGE_HEADING3_BODY';		

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Healing[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_COMBAT_HEALTINGDAMAGE_HEADING3_TITLE';
		
-- Difficulty Level Tooltips
UPDATE Language_en_US
SET Text = 'Beginner. The AI is at a significant disadvantage.'
WHERE Tag = 'TXT_KEY_HANDICAP_SETTLER_HELP';

UPDATE Language_en_US
SET Text = 'Easy. The AI is at a slight disadvantage.'
WHERE Tag = 'TXT_KEY_HANDICAP_CHIEFTAIN_HELP';

UPDATE Language_en_US
SET Text = 'Normal. The AI is on equal footing with human players.'
WHERE Tag = 'TXT_KEY_HANDICAP_WARLORD_HELP';

UPDATE Language_en_US
SET Text = 'Intermediate. The AI has noticeable bonuses.'
WHERE Tag = 'TXT_KEY_HANDICAP_PRINCE_HELP';

UPDATE Language_en_US
SET Text = 'Hard. The AI has substantial bonuses.'
WHERE Tag = 'TXT_KEY_HANDICAP_KING_HELP';

UPDATE Language_en_US
SET Text = 'Expert. The AI has large bonuses.'
WHERE Tag = 'TXT_KEY_HANDICAP_EMPEROR_HELP';

UPDATE Language_en_US
SET Text = 'The AI has massive bonuses at all stages of the game. You have been warned!'
WHERE Tag = 'TXT_KEY_HANDICAP_IMMORTAL_HELP';

UPDATE Language_en_US
SET Text = 'Muuuhahaha! Good luck, sucker!'
WHERE Tag = 'TXT_KEY_HANDICAP_DEITY_HELP';

-- Top Panel Tooltip Adjustment (make generic for more than just founder beliefs)
UPDATE Language_en_US
SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] from Religion.'
WHERE Tag = 'TXT_KEY_TP_FAITH_FROM_RELIGION';

UPDATE Language_en_US
SET Text = 'Allows Research Agreements (if enabled)'
WHERE Tag = 'TXT_KEY_ABLTY_R_PACT_STRING';

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
SET Text = 'Once their civilization has learned certain technologies (see tech tree), workers can remove forests, jungles, and marshes from tiles. Once these features are removed, they are gone forever.'
WHERE Tag = 'TXT_KEY_WORKERS_CLEARINGLAND_HEADING2_BODY';
UPDATE Language_en_US
SET Text = 'If your civilization is unhappy, then the city will produce less food. The city will produce enough food to feed its citizens, but there is a decrease in growth locally.'
WHERE Tag = 'TXT_KEY_FOOD_UNHAPPINESS_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'Settlers can only be constructed in cities of size [COLOR_YELLOW]4[ENDCOLOR] or larger [COLOR_YELLOW]after researching Pottery[ENDCOLOR]. During construction, settlers consume a city''s production and all of the city''s excess food intake. As long as the settler is in production, the city will not grow or add food to its growth bucket. [COLOR_YELLOW]After the construction of the Settler is completed, the city loses one Population. Later in the game, Settlers are replaced by more advanced units which can found cities with additional infrastructure.[ENDCOLOR] See the section on Settlers for more details.'
WHERE Tag = 'TXT_KEY_FOOD_SETTLERS_HEADING2_BODY';
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Settlers And Food Production[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_FOOD_SETTLERS_HEADING2_TITLE';

UPDATE Language_en_US
SET Text = 'Periodically a city may request that you acquire [COLOR_YELLOW]or possess[ENDCOLOR] a specific luxury resource. If you do so, the city will go into "We Love the King Day" for [COLOR_YELLOW]10[ENDCOLOR] turns, during which the city''s growth rate is increased by 25%. When the [COLOR_YELLOW]10[ENDCOLOR] turns are over, the city will demand another luxury resource. Filling that request will cause the city to resume WLtKD for another [COLOR_YELLOW]10[ENDCOLOR] turns.'
WHERE Tag = 'TXT_KEY_RESOURCES_CITYREQUESTS_HEADING3_BODY';
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]We Love the King Day (WLtKD)[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_RESOURCES_CITYREQUESTS_HEADING3_TITLE';

-- Resource Limit breached
UPDATE Language_en_US
SET Text = 'You are currently using more {1_Resource:textkey} than you have! All Units which require it are [COLOR_NEGATIVE_TEXT]unable to heal[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_NOTIFICATION_OVER_RESOURCE_LIMIT';
	
-- Text Changes for Spies in Cities

-- Espionage

UPDATE Language_en_US
SET Text = 'Security Level'
WHERE Tag = 'TXT_KEY_EO_POTENTIAL';

UPDATE Language_en_US
SET Text = 'Security Level reflects the difficulty of Espionage in a City. The higher the value, the more time it will take to complete Spy Missions. The base value (from 5 to 50) is based on the overall economic value of the City (relative to all other cities). Security is also affected by Espionage modifiers and buildings in the city, such as the Constabulary and the Police Station. Security also increases when a City has a powerful Counterspy.[NEWLINE][NEWLINE]Click to sort cities by their Security level.'
WHERE Tag = 'TXT_KEY_EO_POTENTIAL_SORT_TT';

UPDATE Language_en_US
SET Text = 'If your cities have low Security, you should consider protecting them. There are two ways to do this. You may move your own spies to your cities to act as counterspies that have a chance to catch and kill enemy spies before they steal something. You may also slow down how quickly enemy spies can steal things by constructing buildings like the Constabulary, Police Station, and the Great Firewall.'
WHERE Tag = 'TXT_KEY_EO_OWN_CITY_POTENTIAL_TT';

UPDATE Language_en_US
SET Text = '{1_SpyRank} {2_SpyName} is stealing from {3_CityName}.[NEWLINE]The current Security Level of {3_CityName} is {4_Num}.[NEWLINE][NEWLINE]Security reflects the vulnerability of a city to Espionage. The higher the value, the more protected the city. The base value (from 1 to 50) is based on the overall prosperity and happiness of the city (relative to all other cities). Security may be decreased by Policies and Espionage buildings in the city, such as the Constabulary and the Police Station.'
WHERE Tag = 'TXT_KEY_EO_CITY_POTENTIAL_TT';

UPDATE Language_en_US
SET Text = '{1_SpyRank} {2_SpyName} cannot steal technologies from {3_CityName}.[NEWLINE][NEWLINE]The Security Level of {4_CityName} is {5_Num}.[NEWLINE][NEWLINE]Security reflects the vulnerability of a city to Espionage. The higher the value, the more protected the city. The base value (from 5 to 50) is based on the overall prosperity and happiness of the city. Security may be decreased by Policies and Espionage buildings in the city, such as the Constabulary and the Police Station..'
WHERE Tag = 'TXT_KEY_EO_CITY_POTENTIAL_CANNOT_STEAL_TT';

UPDATE Language_en_US
SET Text = 'The Security Level of {1_CityName} is believed to be {2_Num}. Send a [ICON_SPY] Spy to this City to learn more about it.[NEWLINE][NEWLINE]Security reflects the vulnerability of a city to Espionage. The higher the value, the more protected the city. The base value (from 5 to 50) is based on the overall prosperity and happiness of the city. Security may be decreased by Policies and Espionage buildings in the city, such as the Constabulary and the Police Station.'
WHERE Tag = 'TXT_KEY_EO_CITY_ONCE_KNOWN_POTENTIAL_TT';


UPDATE Language_en_US
SET Text = '[ICON_CAPITAL] Security Level: [COLOR_POSITIVE_TEXT]{2_Num}[ENDCOLOR][NEWLINE][ICON_SPY] Spy Resistance: [COLOR_POSITIVE_TEXT]{1_Num}%[ENDCOLOR][NEWLINE][NEWLINE]If [ICON_SPY] Spy Resistance is negative, the City''s Security Level will [COLOR_NEGATIVE_TEXT]fall[ENDCOLOR] towards 5 and enemy Spies will operate more quickly. If positive, it will [COLOR_POSITIVE_TEXT]rise[ENDCOLOR] towards 50 and enemy Spies will operate more slowly. [NEWLINE][NEWLINE]If an enemy Spy completes a Mission in the City, the Security Level is reset to 50.'
WHERE Tag = 'TXT_KEY_POTENTIAL_CALCULATION';
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Estimated Duration:[ENDCOLOR] {1_Num} Turns'
WHERE Tag = 'TXT_KEY_ESPIONAGE_MISSION_DURATION';

UPDATE Language_en_US
SET Text = 'Conducting Spy Mission'
WHERE Tag = 'TXT_KEY_SPY_STATE_GATHERING_INTEL';

UPDATE Language_en_US
SET Text = '{1_RankName} {2_SpyName} is attempting to rig the election in {3_CityName} to increase our influence there. If a coup is possible in the city (ignoring cooldown time), each successfully rigged election increases its success chance. The success chance of a coup is increased by a higher amount for higher ranking spies.'
WHERE Tag = 'TXT_KEY_EO_SPY_RIGGING_ELECTIONS_SHORT_TT';

UPDATE Language_en_US
SET Text = '{1_RankName} {2_SpyName} is conducting a Spy Mission in {3_CityName}. If the city''s owner has a spy in the city, your spy has a chance of being discovered and killed when they try to complete their mission!'
WHERE Tag = 'TXT_KEY_EO_SPY_GATHERING_INTEL_TT';

UPDATE Language_en_US
SET Text = '{1_RankName} {2_SpyName} is schmoozing in {3_CityName} as a diplomat.[NEWLINE][NEWLINE]A spy can be a diplomat if they are placed in the capital of another civilization you are not at war with. (If war is declared, your spy will escape the city.) A diplomat will not attempt to conduct Spy Missions but will still provide intrigue. Once the World Congress convenes and the diplomat has begun schmoozing, you will be able to determine their opinion on proposals and trade for their support if needed. Diplomats also conduct propaganda that provides a boost to [ICON_TOURISM] Tourism output to the target civilization.'
WHERE Tag = 'TXT_KEY_SPY_STATE_SCHMOOZING_TT';

UPDATE Language_en_US
SET Text = '{1_RankName} {2_SpyName} is trying to make all the right connections in  {3_CityName} as a diplomat. After they have made their introductions, they will be able to schmooze.[NEWLINE][NEWLINE]A spy can be a diplomat if they are placed in the capital of another civilization. A diplomat will not attempt to conduct Spy Missions but will still provide intrigue. Once the World Congress convenes and the diplomat has begun schmoozing, you will be able to determine their opinion on proposals and trade for their support if needed.'
WHERE Tag = 'TXT_KEY_SPY_STATE_MAKING_INTRODUCTIONS_TT';

UPDATE Language_en_US
SET Text = '{1_RankName} {2_SpyName} is conducting counter-intelligence operations in {3_CityName}.[NEWLINE][NEWLINE]If an enemy spy tries to finish a Spy Mission in the city you have the spy in, your spy will ensure that the theft is detected. It has a chance of determining who stole the information as well as killing the enemy spy outright. The higher rank your spy, the more likely you are to kill the enemy spy.'
WHERE Tag = 'TXT_KEY_EO_SPY_COUNTER_INTEL_TT';

UPDATE Language_en_US
SET Text = '{1_SpyRank} {2_SpyName} is in {3_CityName}. While {4_SpyRank} {5_SpyName} is in your city, they will perform counter-espionage operations to capture and kill any enemy spies that try to complete Spy Missions.'
WHERE Tag = 'TXT_KEY_CITY_SPY_YOUR_CITY_TT';

UPDATE Language_en_US
SET Text = '{1_SpyRank} {2_SpyName} is in {3_CityName}. While {4_SpyRank} {5_SpyName} is in the city, they establish surveillance and work towards completing Spy Missions. {6_SpyRank} {7_SpyName} will also inform you of any intrigue that they discover during their operations.'
WHERE Tag = 'TXT_KEY_CITY_SPY_OTHER_CIV_TT';

UPDATE Language_en_US
SET Text = 'Options for {1_SpyRank} {2_SpyName}:[NEWLINE][NEWLINE][ICON_BULLET] Move to a City-State and attempt to [COLOR_POSITIVE_TEXT]Rig an Election[ENDCOLOR] or [COLOR_POSITIVE_TEXT]Stage a Coup[ENDCOLOR].[NEWLINE][ICON_BULLET] Move to a non-Capital City owned by a Major Civilization and attempt to conduct a [COLOR_POSITIVE_TEXT]Spy Mission[ENDCOLOR].[NEWLINE][ICON_BULLET] Move to a Capital City owned by a Major Civilization and attempt to conduct a [COLOR_POSITIVE_TEXT]Spy Mission[ENDCOLOR], [COLOR_POSITIVE_TEXT]Uncover Intrigue[ENDCOLOR], or [COLOR_POSITIVE_TEXT]Schmooze[ENDCOLOR] as a Diplomat.'
WHERE Tag = 'TXT_KEY_EO_SPY_MOVE_TT';


UPDATE Language_en_US
SET Text = '{1_SpyName} has achieved the rank of {2_RankName}.[NEWLINE][NEWLINE]There are three spy ranks: Recruit, Agent, and Special Agent. Each subsequent level makes the spy more effective. A higher ranking spy will operate faster and kill enemy spies that are trying to work against you more frequently, rig elections in City-States more effectively, and have a greater chance of pulling off a coup in a City-State allied with another civilization.[NEWLINE][NEWLINE]Spies level up when they successfully complete Spy Missions, kill an enemy spy, schmooze as as a diplomat, or if they uncover intrigue.'
WHERE Tag = 'TXT_KEY_EO_SPY_RANK_TT';

UPDATE Language_en_US
SET Text = 'Click to have {1_SpyRank} {2_SpyName} attempt a coup in {3_CityName} to overthrow the {4_CivAdjective} sympathizers there.[NEWLINE][NEWLINE][COLOR_HIGHLIGHT_TEXT]They have a {5_Num}% chance of success (assuming no enemy counterspy is present).[ENDCOLOR] If {6_SpyRank} {7_SpyName} succeeds, you will become allies with {8_CityName} and {9_CivShortDesc} will be lowered to your current influence amount. Then all other players influence will drop. If {10_SpyRank} {11_SpyName} fails, they will be killed and your influence with {12_CityName} will greatly diminish.[NEWLINE][NEWLINE]To increase your chance of success, you may increase your influence over the City-State, rig elections, use a higher ranking spy, or wait for the current ally''s influence to decay.'
WHERE Tag = 'TXT_KEY_EO_SPY_COUP_ENABLED_TT';

UPDATE Language_en_US
SET Text = '{1_SpyRank} {2_SpyName} may attempt to stage a coup in {3_CityName}, bringing into power a group that will be our ally. The leading faction of {4_CivShortDesc} supporters will be ousted and their influence diminished.[NEWLINE][NEWLINE][COLOR_HIGHLIGHT_TEXT]This operation has a {5_PERCENT}% chance of success (assuming no enemy counterspy is present).[ENDCOLOR] If successful, you will become allies with {6_CityName} and the {7_CivShortDesc} will have their influence dropped. If you fail, your spy will be killed in the attempt and your influence over {8_CityName} will decrease.[NEWLINE][NEWLINE]Do you want {9_SpyRank} {10_SpyName} to attempt a coup in {11_CityName}?'
WHERE Tag = 'TXT_KEY_EO_STAGE_COUP_QUESTION';

UPDATE Language_en_US
SET Text = '{1_RankName} {2_SpyName} is attempting to rig the election in {3_CityName} to increase our influence there.[NEWLINE][NEWLINE]Only one civilization may successfuly rig an election. If more than one spy is in a City-State, the highest ranked spy that has been in that City-State the longest has the greatest chance of successfully rigging the election in its favor. Successfully rigging elections also increases the success chance of a coup in the City-State.'
WHERE Tag = 'TXT_KEY_EO_SPY_RIGGING_ELECTIONS_TT';

UPDATE Language_en_US
SET Text = 'Spies can be used to increase your influence with City-States by "rigging" their local elections or staging a coup. To interact with a City-State, first move a spy to a target City-State. Once the spy arrives, there will be a period of time when they are "Establishing Surveillance" (noted in the "Activity" column). Once surveillance is established, your espionage options become available. Rigging an election, a passive ability, occurs every 15 turns while your spy is located within the City-State. If you are the only spy in the City-State, your influence will increase while everyone else''s will drop. The presence of an enemy spy, especially one of a higher rank, can cause this mission to fail. No worries though, failing to rig an election is not dangerous to your spy.[NEWLINE][NEWLINE]Staging a coup is more powerful than rigging an election, but also much riskier. A successful coup essentially allows you to "steal" the allied status from another civiization. To stage a coup, target a City-State that is allied with another player, and place your spy the same way you would with rigging an election. Once surveillance is established, you will have a new button available to you called "Coup". Clicking it will open a pop-up message that gives you the odds of success. The closer you are in influence to the target civilization, the greater your odds. A high-level spy[COLOR_YELLOW] or having succesfully rigged elections in the City-State [ENDCOLOR] also increases your coup chances. The catch? If you fail, your spy is executed.'
WHERE Tag = 'TXT_KEY_CONCEPT_ESPIONAGE_RIG_ELECTION_SUMMARY';
UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Spies and City-States[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CONCEPT_ESPIONAGE_RIG_ELECTION_TOPIC';


UPDATE Language_en_US
SET Text = '[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Spy is operating {1_SpyBonus} {1_SpyBonus: plural 1?Rank; other?Ranks;} higher than normal when rigging elections since this city state is allied to a civ over which you have strong cultural influence. The success chance of coups is also increased.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_SPY_BONUS_CITY_STATE';

-- Public Opinion

UPDATE Language_en_US
SET Text = '{1_Num} from Public Opinion due to Ideological Pressure.'
WHERE Tag = 'TXT_KEY_TP_UNHAPPINESS_PUBLIC_OPINION';

-- Building Purchased
UPDATE Language_en_US
SET Text = '[ICON_GOLD] Gold spent on Buildings Invests in them, reducing their [ICON_PRODUCTION] Production cost by 50% (25% for Wonders).[NEWLINE][ICON_GOLD] Gold-purchased Units start with half of the XP of a [ICON_PRODUCTION] Produced Unit.[NEWLINE]Some Units and Buildings can be purchased with [ICON_PEACE] Faith (based on Belief and Policy selections).'
WHERE Tag = 'TXT_KEY_CITYVIEW_PURCHASE_TT';

UPDATE Language_en_US
SET Text = 'You can change city construction orders on the City Screen. You can also expend gold to purchase a unit (or invest in a building) on this screen as well. See the City Screen section for details.'
WHERE Tag = 'TXT_KEY_BUILDINGS_PURCHASING_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'You can purchase units with gold, [COLOR_YELLOW]or invest gold in buildings to reduce their construction cost[ENDCOLOR], from within your city screen. This can be helpful if you need something in a hurry, like additional units to defend against an invader.'
WHERE Tag = 'TXT_KEY_CITIES_PURCHASINGITEM_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Purchasing an Item[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CITIES_PURCHASINGITEM_HEADING3_TITLE';

UPDATE Language_en_US
SET Text = 'You can spend gold to purchase units [COLOR_YELLOW]or invest in a building[ENDCOLOR] in a city. Click on a unit (if you can afford it!) and it will be immediately trained in the city, and the amount deducted from your treasury. [COLOR_YELLOW]If you click on a building, you will invest in it, reducing the production cost of the building by 50%.[ENDCOLOR][NEWLINE][NEWLINE]Note that "projects" - the Manhattan Project, etc. - cannot be purchased.'
WHERE Tag = 'TXT_KEY_GOLD_PURCHASEUNITS_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Buying Units, Buildings or Wonders[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GOLD_PURCHASEUNITS_HEADING3_TITLE';

-- Happiness from Policies to Culture/Science change
UPDATE Language_en_US
SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] from Policies.'
WHERE Tag = 'TXT_KEY_TP_CULTURE_FROM_HAPPINESS';

UPDATE Language_en_US
SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] [ICON_RESEARCH] from Policies.'
WHERE Tag = 'TXT_KEY_TP_SCIENCE_FROM_HAPPINESS';

UPDATE Language_en_US
SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] [ICON_RESEARCH] from Scholasticism.'
WHERE Tag = 'TXT_KEY_TP_SCIENCE_FROM_MINORS';

UPDATE Language_en_US
SET Text = '[ICON_RES_ARTIFACTS] Sites Left: {1_Num}'
WHERE Tag = 'TXT_KEY_ANTIQUITY_SITES_TO_EXCAVATE';

UPDATE Language_en_US
SET Text = '[ICON_RES_HIDDEN_ARTIFACTS] Hidden Sites Left: {1_Num}'
WHERE Tag = 'TXT_KEY_HIDDEN_SITES_TO_EXCAVATE';

-- Update text for diplomat.
UPDATE Language_en_US
SET Text = '+{1_Num}% Bonus from Diplomat[NEWLINE]'
WHERE Tag = 'TXT_KEY_CO_PLAYER_TOURISM_PROPAGANDA';

UPDATE Language_en_US
SET Text = 'Do you want this spy to be a diplomat? A spy can be a diplomat if they are placed in the capital of another civilization you are not at war with. (If war is declared, your spy will escape the city.) A diplomat will not attempt to steal technology from the other player but will still provide intrigue. Once the World Congress convenes, you will be able to determine their opinion on proposals and trade for their support if needed. Diplomats automatically provide a boost to [ICON_TOURISM] Tourism output to the target civilization.'
WHERE Tag = 'TXT_KEY_SPY_BE_DIPLOMAT';

-- Update text for tooltips based on what you changed above.
INSERT INTO Language_en_US (Text, Tag)
VALUES ('[NEWLINE][NEWLINE][COLOR_CYAN]Influence Benefits:[ENDCOLOR][NEWLINE][COLOR_POSITIVE_TEXT]Trade Routes to this Civ Generate:[ENDCOLOR][NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+5%[ENDCOLOR] [ICON_FOOD] Growth in Origin City[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+2[ENDCOLOR] [ICON_GOLD] Gold[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+2[ENDCOLOR] [ICON_RESEARCH] Science[NEWLINE][COLOR_POSITIVE_TEXT]Espionage Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [ICON_SPY] Spies Establish Surveillance in [COLOR_POSITIVE_TEXT]5[ENDCOLOR] turns[NEWLINE][COLOR_POSITIVE_TEXT]City Conquest Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-15%[ENDCOLOR] [ICON_RESISTANCE] Unrest Time[NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-15%[ENDCOLOR] [ICON_CITIZEN] Citizen loss from City conquest' , 'TXT_KEY_CO_INFLUENCE_BONUSES_EXOTIC');

-- Update texts for tooltips based on what you changed above.
UPDATE Language_en_US
SET Text = '[NEWLINE][NEWLINE][COLOR_CYAN]Influence Benefits:[ENDCOLOR][NEWLINE][COLOR_POSITIVE_TEXT]Trade Routes to this Civ Generate:[ENDCOLOR][NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+10%[ENDCOLOR] [ICON_FOOD] Growth in Origin City[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+4[ENDCOLOR] [ICON_GOLD] Gold[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+4[ENDCOLOR] [ICON_RESEARCH] Science[NEWLINE][COLOR_POSITIVE_TEXT]Espionage Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [ICON_SPY] Spies Establish Surveillance in [COLOR_POSITIVE_TEXT]4[ENDCOLOR] turns[NEWLINE][COLOR_POSITIVE_TEXT]City Conquest Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-30%[ENDCOLOR] [ICON_RESISTANCE] Unrest Time[NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-30%[ENDCOLOR] [ICON_CITIZEN] Citizen loss from City conquest'
WHERE Tag = 'TXT_KEY_CO_INFLUENCE_BONUSES_FAMILIAR';

-- Update texts for tooltips based on what you changed above.
UPDATE Language_en_US
SET Text = '[NEWLINE][NEWLINE][COLOR_CYAN]Influence Benefits:[ENDCOLOR][NEWLINE][COLOR_POSITIVE_TEXT]Trade Routes to this Civ Generate:[ENDCOLOR][NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+15%[ENDCOLOR] [ICON_FOOD] Growth in Origin City[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+6[ENDCOLOR] [ICON_GOLD] Gold[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+6[ENDCOLOR] [ICON_RESEARCH] Science[NEWLINE][COLOR_POSITIVE_TEXT]Espionage Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [ICON_SPY] Spies Establish Surveillance in [COLOR_POSITIVE_TEXT]3[ENDCOLOR] turns[NEWLINE][COLOR_POSITIVE_TEXT]City Conquest Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-55%[ENDCOLOR] [ICON_RESISTANCE] Unrest Time[NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-55%[ENDCOLOR] [ICON_CITIZEN] Citizen loss from City conquest'
WHERE Tag = 'TXT_KEY_CO_INFLUENCE_BONUSES_POPULAR';

-- Update texts for tooltips based on what you changed above.
UPDATE Language_en_US
SET Text = '[NEWLINE][NEWLINE][COLOR_CYAN]Influence Benefits:[ENDCOLOR][NEWLINE][COLOR_POSITIVE_TEXT]Trade Routes to this Civ Generate:[ENDCOLOR][NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+20%[ENDCOLOR] [ICON_FOOD] Growth in Origin City[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+8[ENDCOLOR] [ICON_GOLD] Gold[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+8[ENDCOLOR] [ICON_RESEARCH] Science[NEWLINE][COLOR_POSITIVE_TEXT]Espionage Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [ICON_SPY] Spies Establish Surveillance in [COLOR_POSITIVE_TEXT]2[ENDCOLOR] turns[NEWLINE][COLOR_POSITIVE_TEXT]City Conquest Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-80%[ENDCOLOR] [ICON_RESISTANCE] Unrest Time[NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-80%[ENDCOLOR] [ICON_CITIZEN] Citizen loss from City conquest'
WHERE Tag = 'TXT_KEY_CO_INFLUENCE_BONUSES_INFLUENTIAL';

-- Update texts for tooltips based on what you changed above.
UPDATE Language_en_US
SET Text = '[NEWLINE][NEWLINE][COLOR_CYAN]Influence Benefits:[ENDCOLOR][NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Trade Routes to this Civ Generate:[ENDCOLOR][NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+25%[ENDCOLOR] [ICON_FOOD] Growth in Origin City[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+10[ENDCOLOR] [ICON_GOLD] Gold[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+10[ENDCOLOR] [ICON_RESEARCH] Science[NEWLINE][COLOR_POSITIVE_TEXT]Espionage Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [ICON_SPY] Spies Establish Surveillance in [COLOR_POSITIVE_TEXT]1[ENDCOLOR] turn[NEWLINE][COLOR_POSITIVE_TEXT]City Conquest Bonuses versus this Civ:[ENDCOLOR][NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-100%[ENDCOLOR] [ICON_RESISTANCE] Unrest Time[NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-100%[ENDCOLOR] [ICON_CITIZEN] Citizen loss from City conquest'
WHERE Tag = 'TXT_KEY_CO_INFLUENCE_BONUSES_DOMINANT';

-- Text for trade view tooltip.
INSERT INTO Language_en_US (Text, Tag)
VALUES ('+{2_Num} [ICON_GOLD] Gold due to your Cultural Influence over {1_CivName}', 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_GOLD_EXPLAINED');

INSERT INTO Language_en_US (Text, Tag)
VALUES ('+{1_Num}% from Policies', 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YIELD_MODIFIER');

INSERT INTO Language_en_US (Text, Tag)
VALUES ('+{1_Num}% [ICON_PRODUCTION] Production from Policies', 'TXT_KEY_CHOOSE_INTERNAL_TRADE_ROUTE_ITEM_TT_YIELD_MODIFIER_PROD');

INSERT INTO Language_en_US (Text, Tag)
VALUES ('+{1_Num}% [ICON_FOOD] Food from Policies', 'TXT_KEY_CHOOSE_INTERNAL_TRADE_ROUTE_ITEM_TT_YIELD_MODIFIER_FOOD');



-- Text for trade view tooltip.
INSERT INTO Language_en_US (Text, Tag)
VALUES ('+{2_Num} [ICON_GOLD] Gold due to the Cultural Influence of {1_CivName} over you', 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_GOLD_EXPLAINED');

-- Text changes for Vote Purchase
UPDATE Language_en_US
SET Text = 'In the next World Congress session, [COLOR_POSITIVE_TEXT]{1_NumVotes}[ENDCOLOR] {1_NumVotes: plural 1?Delegate; other?Delegates;} currently controlled by this player will support {2_ChoiceText} on the proposal to [COLOR_POSITIVE_TEXT]enact[ENDCOLOR] these changes:[NEWLINE][NEWLINE]{3_ProposalText}[NEWLINE][NEWLINE]The number of delegates above is based on the number of delegates this player controls, and the Rank of your [ICON_DIPLOMAT] Diplomat. Higher ranks allow you to trade for more delegates.'
WHERE Tag = 'TXT_KEY_DIPLO_VOTE_TRADE_ENACT_TT';

UPDATE Language_en_US
SET Text = 'In the next World Congress session, [COLOR_POSITIVE_TEXT]{1_NumVotes}[ENDCOLOR] {1_NumVotes: plural 1?Delegate; other?Delegates;} currently controlled by this player will support {2_ChoiceText} on the proposal to [COLOR_WARNING_TEXT]repeal[ENDCOLOR] these effects:[NEWLINE][NEWLINE]{3_ProposalText}[NEWLINE][NEWLINE]The number of delegates above is based on the number of delegates this player controls, and the rank of your [ICON_DIPLOMAT] Diplomat. Higher ranks allow you to trade for more delegates.'
WHERE Tag = 'TXT_KEY_DIPLO_VOTE_TRADE_REPEAL_TT';

UPDATE Language_en_US
SET Text = 'Barbarian Encampment'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_ENCAMPMENT';

UPDATE Language_en_US
SET Text = 'Barbarian Encampment'
WHERE Tag = 'TXT_KEY_IMPROVEMENT_ENCAMPMENT';

UPDATE Language_en_US
SET Text = 'A Barbarian Encampment is a temporary settlement by a group of rampaging barbarians intent upon overthrowing and destroying your civilization. These perpetual thorns in the sides of civilizations are capable of spitting out an unending stream of angry barbarians, all who seek to overrun your cities, pillage your lands and stare lewdly at your women. Encampments near to your cities should be destroyed without question. Encampments nearer to competing civilizations may be left to harass your foes, if you are that kind of sneaky leader...'
WHERE Tag = 'TXT_KEY_IMPROVEMENT_BARBARIAN_CAMP_PEDIA';

-- CS Protection Tooltip for new influence perk
UPDATE Language_en_US
SET Text = 'Pledging to protect a City-State lets the other major powers in the game know that you will protect the City-State from attacks and tribute demands.[NEWLINE][NEWLINE]During protection, your resting point for [ICON_INFLUENCE] Influence with this City-State is increased by {1_InfluenceMinimum}, and you will earn [COLOR_POSITIVE_TEXT]{3_InfluenceBoost}%[ENDCOLOR] more [ICON_INFLUENCE] Influence from quests completed for this City-State. The City-State [ICON_CAPITAL] Capital''s base [ICON_STRENGTH] Combat Strength will be increased by [COLOR_POSITIVE_TEXT]{4_DefenseBoost}%[ENDCOLOR], up to a global maximum of [COLOR_POSITIVE_TEXT]{5_DefenseTotal}%[ENDCOLOR]. [NEWLINE][NEWLINE]Protection cannot be revoked until {2_TurnsMinimum} turns after the pledge is made.'
WHERE Tag = 'TXT_KEY_POP_CSTATE_PLEDGE_TT';
	
UPDATE Language_en_US
SET Text = '[NEWLINE][COLOR_WARNING_TEXT][ICON_BULLET] You must have {1_InfluenceNeededToPledge} or more [ICON_INFLUENCE] Influence to pledge.'
WHERE Tag = 'TXT_KEY_POP_CSTATE_PLEDGE_DISABLED_INFLUENCE_TT';
	
UPDATE Language_en_US
SET Text = '[NEWLINE][COLOR_WARNING_TEXT][ICON_BULLET] {1_TurnsUntilPledgeAvailable} turns must pass before you can pledge again.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_POP_CSTATE_PLEDGE_DISABLED_MISTRUST_TT';

UPDATE Language_en_US
SET Text = '[ICON_INFLUENCE] Influence above Neutral threshold'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_LOW_INFLUENCE';

UPDATE Language_en_US
SET Text = 'They recognize your military strength, and will give you [COLOR_POSITIVE_TEXT]{1_Num}%[ENDCOLOR] of their total yields as tribute.'
WHERE Tag = 'TXT_KEY_CSTATE_CAN_BULLY';	

UPDATE Language_en_US
SET Text = 'They do not fear you, and will resist tribute demands, as you are [COLOR_NEGATIVE_TEXT]{1_Num}%[ENDCOLOR] below the threshold.'
WHERE Tag = 'TXT_KEY_CSTATE_CANNOT_BULLY';	

UPDATE Language_en_US
SET Text = 'Take {1_GoldValue} [ICON_GOLD] Gold and Partial Quest Rewards (-{2_Influence} [ICON_INFLUENCE])'
WHERE Tag = 'TXT_KEY_POPUP_MINOR_BULLY_UNIT_AMOUNT';

UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]Fallout deals 15 Damage to Units that end their turn on a tile with Fallout.[ENDCOLOR][NEWLINE][NEWLINE]Fallout is the residual radiation left over following a nuclear explosion. The fallout "falls out" of the air as a layer of radioactive particles which are highly dangerous to plants and animals, killing them immediately or damaging their DNA, giving them cancer, other diseases, or unfortunate mutations. Depending upon the type of nuclear explosion, the land may remain poisoned for decades, possibly centuries. Cleanup requires the replacement of the contaminated buildings, soil and vegetation.'
WHERE Tag = 'TXT_KEY_FEATURE_FALLOUT_PEDIA';
	
INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BUILD_FARM_REC', 'It will boost your [ICON_FOOD] Food output on this tile. Farms adjacent to two other Farms and/or to Fresh Water gain +1 [ICON_FOOD] Food.');

UPDATE Language_en_US
SET Text = 'Farms can be constructed on most any land to improve the output of food on the tile. Farms gain +1 Food for every two adjacent Farms and/or if Fresh Water is available.[NEWLINE][NEWLINE]Farming is one of the earliest and most important of all human professions, as it allowed mankind to stop migrating and settle in one location without depleting the local resources.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_FARM_TEXT';

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BUILD_FARM_HELP', '+1 [ICON_FOOD] Food for every 2 Farms adjacent to this Farm and/or from Fresh Water.');

-- Trading Post - name change

UPDATE Language_en_US
SET Text = 'Villages are smaller settlements scattered around the countryside of a civilization, representative of the people that live off of the land and trade. They can generate a lot of wealth for a society, particularly when placed on Roads, Railroads, or on Trade Routes.[NEWLINE][NEWLINE]Receive [ICON_GOLD] Gold [ICON_PRODUCTION] Production if built on a Road or Railroad that connects two owned Cities.[NEWLINE][NEWLINE]Receive additional [ICON_GOLD] Gold and [ICON_PRODUCTION] Production (+1 pre-Industrial Era, +2 Industrial Era or later) if a Trade Route, either internal or international, passes over this Village.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_TRADING_POST_TEXT';

UPDATE Language_en_US
SET Text = 'Village'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_TRADINGPOST';

UPDATE Language_en_US
SET Text = 'Village'
WHERE Tag = 'TXT_KEY_IMPROVEMENT_TRADING_POST';

UPDATE Language_en_US
SET Text = 'Construct a [LINK=IMPROVEMENT_TRADING_POST]Village[\LINK]'
WHERE Tag = 'TXT_KEY_BUILD_TRADING_POST';

UPDATE Language_en_US
SET Text = 'Does the village provide gold?'
WHERE Tag = 'TXT_KEY_GOLD_TRADINGPOST_ADV_QUEST';

UPDATE Language_en_US
SET Text = 'Construct a Village improvement in a tile to increase its gold output. Earns additional gold and production if placed on a Road or Railroad that connects two owned cities and/or if a Trade Route, either internal or international, passes over it. Cannot be built adjacent to one another.'
WHERE Tag = 'TXT_KEY_GOLD_TRADINGPOST_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'The village increases output of a tile by 3 gold and 1 culture. It does not access a resource. It generates additional gold and production if on a route and/or if built on a trade route.[NEWLINE]Technology Required: Currency[NEWLINE]Construction Time: 5 Turns[NEWLINE]May Be Constructed On: Any land tile but ice. Cannot be built adjacent to one another.'
WHERE Tag = 'TXT_KEY_WORKERS_TRADINGPOST_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'If there is a road between your capital and another of your cities, those cities have a "city connection." City connections give your civilization gold bonuses each turn, the amount depending upon the size of the cities involved. ([COLOR_YELLOW]Lighthouses[ENDCOLOR] can also create city connections between coastal cities.) See the section on City Connections for details.'
WHERE Tag = 'TXT_KEY_WORKERS_TRADEROUTES_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Roads and Trade Routes[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_WORKERS_TRADEROUTES_HEADING3_TITLE';

UPDATE Language_en_US
SET Text = '[COLOR_GREEN]The Village[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_GOLD_TRADINGPOST_HEADING3_TITLE';

UPDATE Language_en_US
SET Text = 'During a game, you will create "workers" - non-military units who will "improve" the land around your cities, increasing productivity or providing access to a nearby "resource." Improvements include farms, villages, lumber mills, quarries, mines, and more. During wartime your enemy may "pillage" (destroy) your improvements. Pillaged improvements are ineffective until a worker has "repaired" them.'
WHERE Tag = 'TXT_KEY_PEDIA_IMPROVEMENT_HELP_TEXT';

UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]Costs[ENDCOLOR] 2 [ICON_GOLD] Gold per turn to maintain.[NEWLINE][NEWLINE]An unbroken chain of Railroads allows Units to move extremely quickly through Tiles.'
WHERE Tag = 'TXT_KEY_BUILD_RAILROAD_HELP';

-- Fort
UPDATE Language_en_US
SET Text = 'A fort is a special improvement that improves the defensive bonus of the tile by 50% for units stationed in that tile. However, forts do not provide a defensive bonus to units in enemy territory. Cannot be built adjacent to one another. Contains [COLOR_POSITIVE_TEXT]Fortifications[ENDCOLOR], allowing owned Units to stay stationed in the Tile after attacking and a [COLOR_POSITIVE_TEXT]Canal[ENDCOLOR], allowing friendly [COLOR_POSITIVE_TEXT]Sea[ENDCOLOR] Units to traverse through this tile.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_FORT_TEXT';

-- Landmark
UPDATE Language_en_US
SET Text = 'Landmarks provide +3 Empire-Wide [ICON_HAPPINESS_1] Happiness to the builder and plot owner when constructed, as well as +1 [ICON_CULTURE] Culture and +1 [ICON_GOLD] Gold on the Tile for each additional Era that has passed in comparison to the original Era of the Artifact.[NEWLINE][NEWLINE]A Landmark is any magnificent artifact, structure, work of art, or wonder of nature that draws visitors to a location. Nelson''s Column in London is a landmark, as is Mount Rushmore in the United States. Not every significantly sized object, however, is a landmark: the World''s Largest Ball of Twine may never rise to that stature (though it might well be worth a visit).'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_LANDMARK_TEXT';

UPDATE Language_en_US
SET Text = 'When constructed, a Landmark generates +3 Empire-Wide [ICON_HAPPINESS_1] Happiness for the builder and plot owner. When worked, a Landmark provides [ICON_CULTURE] Culture and [ICON_GOLD] Gold based on its age. +1 [ICON_CULTURE] Culture and [ICON_GOLD] Gold per Era older than this civ''s current Era. Archaeologist will be consumed.'
WHERE Tag = 'TXT_KEY_CHOOSE_ARCH_LANDMARK_RESULT';

UPDATE Language_en_US
SET Text = '[NEWLINE][COLOR_POSITIVE_TEXT]Creating a Landmark in another civ''s territory will generate +3 Empire-Wide [ICON_HAPPINESS_1] Happiness for the builder and plot owner, and give you a permanent diplomatic boost with that nation.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CHOOSE_ARCH_LANDMARK_MAJOR_CIV';

UPDATE Language_en_US
SET Text = '[NEWLINE][COLOR_POSITIVE_TEXT]Creating a Landmark in a city state''s territory will generate +3 Empire-Wide [ICON_HAPPINESS_1] Happiness for the builder and plot owner, and will give you a large, one-time influence boost with that city state.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CHOOSE_ARCH_LANDMARK_MINOR_CIV';


UPDATE Language_en_US
SET Text = 'You have found {@1_Name}!'
WHERE Tag = 'TXT_KEY_FOUND_NATURAL_WONDER';


UPDATE Language_en_US
SET Text = '{1_Num} from Natural Wonders and Landmarks.'
WHERE Tag = 'TXT_KEY_TP_HAPPINESS_NATURAL_WONDERS';

-- Theming
UPDATE Language_en_US
SET Text = '{1_Num} [ICON_TOURISM] Tourism from Theming Bonuses'
WHERE Tag = 'TXT_KEY_CO_CITY_TOURISM_THEMING_BONUSES';

-- Happiness
	
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
SET Text = 'Sort By Unhappiness'
WHERE Tag = 'TXT_KEY_EO_SORT_STRENGTH';

UPDATE Language_en_US
SET Text = 'City will be [ICON_RAZING] Razed to the ground in {1_Num} turn(s)! While the City is being Razed, 100% of its [ICON_CITIZEN] Population will produce [ICON_HAPPINESS_3] Unhappiness, but Units within its borders will heal faster!'
WHERE Tag = 'TXT_KEY_CITY_BURNING';

UPDATE Language_en_US
SET Text = 'City is in [ICON_RESISTANCE] Resistance for {1_Num} turn(s)! While in Resistance, a City cannot do anything useful, Units within its borders will heal slower, and 100% of its [ICON_CITIZEN] Population will produce [ICON_HAPPINESS_3] Unhappiness!'
WHERE Tag = 'TXT_KEY_CITY_RESISTANCE';

UPDATE Language_en_US
SET Text = '{1_Num} from [ICON_CITIZEN] Population in [ICON_OCCUPIED] Occupied, [ICON_RESISTANCE] Resisting or [ICON_RAZING] Razing Cities.'
WHERE Tag = 'TXT_KEY_TP_UNHAPPINESS_OCCUPIED_POPULATION';

UPDATE Language_en_US
SET Text = '{1_Num : number "#.##"} from [ICON_PUPPET] Puppet Cities.'
WHERE Tag = 'TXT_KEY_TP_UNHAPPINESS_PUPPET_CITIES';

UPDATE Language_en_US
SET Text = '{1_Num : number "#.##"} from [ICON_URBANIZATION] Urbanization (Specialists).'
WHERE Tag = 'TXT_KEY_TP_UNHAPPINESS_SPECIALISTS';

-- Tutorial Text

UPDATE Language_en_US
SET Text = 'The happiness system in Vox Populi is completely revised, favoring neither tall nor wide playstyles. Happiness is now based on a cause-and-effect mechanism that makes your empire feel far more alive, and dynamic, than ever before. [NEWLINE][NEWLINE]Happiness and Unhappiness are calculated locally for each City in your Empire. When the Local Unhappiness in a City exceeds the Local Happiness, the City grows slower than normally, and Production will be reduced when producing a Settler or Military Unit. This is also indicated by an [ICON_HAPPINESS_3] Unhappiness Icon displayed under the City Banner. Special rules apply for Puppet Cities and Occupied Cities.[NEWLINE][NEWLINE]The sum of the Happiness in all Cities of your Empire is compared to the sum of the Unhappiness in all of your Cities to determine the Approval Rating, which is displayed on the status line of the Main Screen (in the upper left-hand corner of the game). Watch it carefully. If it starts to drop below 50%, your population is getting restless, resulting in a growth reduction in all cities, a penalty when producing Settlers, and reduced combat strength. If it starts to dip below 35%, you are in trouble. Barbarians will spawn in your empire, and cities may abandon your empire and join other civilizations. You can get an overview of the different sources of your Happiness and Unhappiness by hovering your cursor over the status line.'
WHERE Tag = 'TXT_KEY_HAPPINESS_HEADING1_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_GREEN]Happiness[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_HAPPINESS_HEADING1_TITLE';

DELETE FROM Concepts
WHERE Type = 'CONCEPT_HAPPINESS_STARTING';
DELETE FROM Concepts
WHERE Type = 'CONCEPT_HAPPINESS_HAPPINESS_LEVELS';
DELETE FROM Concepts
WHERE Type = 'CONCEPT_HAPPINESS_UNHAHPPY';
DELETE FROM Concepts
WHERE Type = 'CONCEPT_HAPPINESS_VERY_UNHAPPY';
DELETE FROM Concepts
WHERE Type = 'CONCEPT_HAPPINESS_REVOLT';

DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_HAPPINESS_STARTING';
DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_HAPPINESS_HAPPINESS_LEVELS';
DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_HAPPINESS_UNHAHPPY';
DELETE FROM Concepts_RelatedConcept
WHERE ConceptType = 'CONCEPT_HAPPINESS_VERY_UNHAPPY';
DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_HAPPINESS_STARTING';
DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_HAPPINESS_HAPPINESS_LEVELS';
DELETE FROM Concepts_RelatedConcept
WHERE RelatedConcept = 'CONCEPT_HAPPINESS_VERY_UNHAPPY';

UPDATE Concepts
SET InsertBefore = 'CONCEPT_HAPPINESS_LOCAL_HAPPINESS',
    InsertAfter = 'CONCEPT_HAPPINESS'
WHERE Type = 'CONCEPT_HAPPINESS_CAUSES_HAPPINESS';

UPDATE Language_en_US
SET Text = 'Happiness in your empire can come from the following sources:[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Starting Happiness[ENDCOLOR]: The amount of Happiness that your civilization begins with and the starting Local Happiness in the [ICON_CAPITAL] Capital are determined by the game''s difficulty setting. Starting Happiness will become less important over time as Happiness and Unhappiness from other sources grow. [NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Natural Wonders and Landmarks[ENDCOLOR]: Discovering Natural Wonders or using Archaeologists to build Landmarks (in your own or in other player''s lands) gives you a small Happiness bonus.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]City States[ENDCOLOR]: Mercantile City States provide you with a Happiness bonus if you are friends or allies with them.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Vassals[ENDCOLOR]: If you have vassalized other civilizations, they may provide you with some additional Happiness based on their own Approval Rating.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Luxury Resources[ENDCOLOR]: You receive Happiness for each unique Luxury Resource connected to your empire. Luxuries are therefore less vital early on, but also not useless in the late game. Moreover, Monopolies on some Luxury Resources grant additional Happiness to your empire.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Locally Generated Happiness[ENDCOLOR]: Certain Buildings increase your local Happiness in the Cities where they are built. Other sources of Local Happiness include Religious Beliefs, Policies, World Wonders and Events. Note that this does not apply to Puppet Cities and Occupied Cities, which can never generate Local Happiness.'
WHERE Tag = 'TXT_KEY_HAPPINESS_CAUSES_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_GREEN]What Causes Happiness[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_HAPPINESS_CAUSES_HEADING2_TITLE';

UPDATE Concepts
SET InsertBefore = 'CONCEPT_HAPPINESS_NEED_THRESHOLDS',
    InsertAfter = 'CONCEPT_HAPPINESS_LOCAL_HAPPINESS'
WHERE Type = 'CONCEPT_HAPPINESS_CAUSES_UNHAPPINESS';

UPDATE Language_en_US
SET Text = 'The Unhappiness a city produces is no longer equal to the number of citizens in it, although population still strongly affects Unhappiness. Unhappiness in Vox Populi is caused by several sources, each of them contributing to the total Unhappiness a city has. A detailed overview of the sources of Unhappiness in a city can be seen by hovering over the "[ICON_HAPPINESS_3] Unhappiness" tab in the City Screen. The total amount of Unhappiness in a city cannot exceed the city''s population, no matter how much Unhappiness is generated from the different sources. The only exception to this is Unhappiness from Urbanization, which always applies, even if it puts the total Unhappiness of your city above its population. The possible sources of Unhappiness are the following: [NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]Unhappiness from Needs[ENDCOLOR]: Often, most of the Unhappiness in a city is caused by various Needs. With the exception of Religious Unrest, Unhappiness arises in a city if its output of a certain yield per citizen (like gold per citizen, science per citizen, etc.) is lower than the respective Needs Threshold, a reference value which is calculated based on the yields in other cities worldwide. The lower the city''s yield output per citizen is, the higher the generated Unhappiness will be. It is therefore necessary to keep yields in your cities increasing, as your population will become more and more demanding over time due to the stronger competition (other cities grow as well, increasing the Needs Thresholds). Avoiding population growth can help in keeping Unhappiness from Needs manageable, and certain Building and Policies reduce Unhappiness from Needs by a fixed amount. There are five main Needs, four based on different yield types and one based on religious diversity:[NEWLINE]
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
SET Text = '[COLOR_GREEN]What Causes Unhappiness[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_HAPPINESS_CAUSESUNHAPPINESS_HEADING2_TITLE';

-- Text for city view tooltip.
INSERT INTO Language_en_US (
Text, Tag)
SELECT '[NEWLINE][ICON_BULLET][ICON_HAPPINESS_1] Local Happiness Modifier: {1_Num}%', 'TXT_KEY_FOODMOD_HAPPY';

INSERT INTO Language_en_US (
Text, Tag)
SELECT '[NEWLINE][ICON_BULLET]Modified due to Happiness: {1_Num}%', 'TXT_KEY_PRODMOD_BALANCE_HAPPINESS_MOD';


-- Text for city view tooltip.
UPDATE Language_en_US
SET Text = '[NEWLINE][ICON_BULLET][COLOR_WARNING_TEXT][ICON_HAPPINESS_3] Unhappiness Modifier: {1_Num}%[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_FOODMOD_UNHAPPY';


-- Text for city view tooltip.
UPDATE Language_en_US
SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] from your [ICON_GOLDEN_AGE] Golden Age.'
WHERE Tag = 'TXT_KEY_TP_CULTURE_FROM_GOLDEN_AGE';

-- Update Text for Very Unhappy and Super Unhappy

-- Update text for top panel depending on which yields you have enabled above. Change as desired.
UPDATE Language_en_US
SET Text = 'Your approval rating is less than 35% - your Empire is in open rebellion! Uprisings may occur with rebel (barbarian) units appearing in your territory, and Cities may abandon your Empire and flip to the Civilization that is most culturally influential over your people!'
WHERE Tag = 'TXT_KEY_TP_EMPIRE_VERY_UNHAPPY';

UPDATE Language_en_US
SET Text = 'Your approval rating is less than 20% - your Empire is fracturing! Rebels (barbarians) will spawn more frequently and Cities will rapidly abandon your Empire and flip to the Civilization that is most culturally influential over your people!'
WHERE Tag = 'TXT_KEY_TP_EMPIRE_SUPER_UNHAPPY';
	
-- Update text for top panel depending on which yields you have enabled above. Change as desired.
UPDATE Language_en_US
SET Text = 'Your approval rating is less than 50%!'
WHERE Tag = 'TXT_KEY_TP_EMPIRE_UNHAPPY';

UPDATE Language_en_US
SET Text = 'Your approval rating is at least 50%. [ICON_HAPPINESS_1] Happy Cities will [ICON_FOOD] Grow more quickly, but [ICON_HAPPINESS_3] Unhappy Cities will suffer penalties to [ICON_FOOD] Growth, Military Unit costs and Settler costs.'
WHERE Tag = 'TXT_KEY_TP_TOTAL_HAPPINESS';

-- TOOLTIPS FOR TOP BAR

-- Text for city view tooltip.
INSERT INTO Language_en_US (Text, Tag)
VALUES ('{1_Num} due to [ICON_HAPPINESS_1] Happiness.', 'TXT_KEY_TP_GOLD_GAINED_FROM_HAPPINESS');


-- Text for city view tooltip.
INSERT INTO Language_en_US (Text, Tag)
VALUES ('{1_NUM} due to [ICON_HAPPINESS_3] Unhappiness.', 'TXT_KEY_TP_GOLD_LOST_FROM_UNHAPPINESS');

-- Text for city view tooltip.
INSERT INTO Language_en_US (Text, Tag)
VALUES ('[ICON_BULLET] [COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] due to [ICON_HAPPINESS_1] Happiness.', 'TXT_KEY_TP_FAITH_GAINED_FROM_HAPPINESS');


-- Text for city view tooltip.
INSERT INTO Language_en_US (Text, Tag)
VALUES ('[ICON_BULLET] [COLOR_NEGATIVE_TEXT]{1_NUM}[ENDCOLOR] due to [ICON_HAPPINESS_3] Unhappiness.', 'TXT_KEY_TP_FAITH_LOST_FROM_UNHAPPINESS');

-- Text for city view tooltip.
INSERT INTO Language_en_US (Text, Tag)
VALUES ('[ICON_BULLET] [COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] [ICON_RESEARCH] due to [ICON_HAPPINESS_1] Happiness.', 'TXT_KEY_TP_SCIENCE_GAINED_FROM_HAPPINESS');


-- Text for city view tooltip.
INSERT INTO Language_en_US (Text, Tag)
VALUES ('[ICON_BULLET] [COLOR_NEGATIVE_TEXT]{1_Num}[ENDCOLOR] [ICON_RESEARCH] due to [ICON_HAPPINESS_3] Unhappiness.', 'TXT_KEY_TP_SCIENCE_LOST_FROM_UNHAPPINESS');

-- Text for city view tooltip.
INSERT INTO Language_en_US (Text, Tag)
VALUES ('[ICON_BULLET] [COLOR_NEGATIVE_TEXT]{1_NUM}[ENDCOLOR] due to [ICON_HAPPINESS_3] Unhappiness.', 'TXT_KEY_TP_CULTURE_LOST_FROM_UNHAPPINESS');

-- Text for city view tooltip.
INSERT INTO Language_en_US (Text, Tag)
VALUES ('[ICON_BULLET] [COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] due to [ICON_HAPPINESS_1] Happiness.', 'TXT_KEY_TP_CULTURE_GAINED_FROM_HAPPINESS');

UPDATE Language_en_US
SET Text = '{1_CityName} can no longer work on {2_BldgName}. The lost [ICON_PRODUCTION] is converted into {3_NumGold}[ICON_CULTURE]!'
WHERE Tag = 'TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK' AND Value= 2 );

UPDATE Language_en_US
SET Text = '{1_CityName} can no longer work on {2_BldgName}. The lost [ICON_PRODUCTION] is converted into {3_NumGold}[ICON_GOLDEN_AGE]!'
WHERE Tag = 'TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK' AND Value= 3 );

UPDATE Language_en_US
SET Text = '{1_CityName} can no longer work on {2_BldgName}. The lost [ICON_PRODUCTION] is converted into {3_NumGold}[ICON_RESEARCH]!'
WHERE Tag = 'TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK' AND Value= 4 );

UPDATE Language_en_US
SET Text = '{1_CityName} can no longer work on {2_BldgName}. The lost [ICON_PRODUCTION] is converted into {3_NumGold}[ICON_PEACE]!'
WHERE Tag = 'TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK' AND Value= 5 );


-- Resource Changes

UPDATE Language_en_US
SET Text = 'Marble is a beautiful rock highly prized as a building material and by artists for sculpting. Marble comes in a variety of colors and shades, sometimes mono-colored and sometimes with veins and pockets of many different colors within. Many of the most beautiful structures in the world - including the Taj Mahal, for one - are constructed of marble, as are the greatest sculptures of antiquity.[NEWLINE][NEWLINE]+15% [ICON_PRODUCTION] Production when building pre-Industrial Wonders in the City where it is worked.'
WHERE Tag = 'TXT_KEY_CIV5_RESOURCE_MARBLE_TEXT';

UPDATE Language_en_US
SET Text = 'Found in the Earth''s crust and upper mantle, stone is perhaps the most crucial element in human construction efforts throughout history. There are three major classifications of stone - Sedimentary, Igneous, and Metamorphic - each differing in their origin.[NEWLINE][NEWLINE]+10% [ICON_PRODUCTION] Production when building pre-Renaissance Wonders in the City where it is worked.'
WHERE Tag = 'TXT_KEY_CIV5_RESOURCE_STONE_TEXT';

-- Lumber Mill

UPDATE Language_en_US
SET Text = 'A Lumber Mill allows you to improve the Production and Gold output of forest and jungle tiles. Gains additional Gold and Production for every two adjacent Lumber Mills, so try to build them in clusters of three if possible.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_LUMBERMILL_TEXT';

UPDATE Language_en_US
SET Text = 'It will increase the amount of [ICON_PRODUCTION] Production (Forest) or [ICON_GOLD] Gold (Jungle) provided by this tile, while keeping the Forest/Jungle intact. A Lumber Mill adjacent to two other Lumber Mills gains +1 [ICON_PRODUCTION] Production and [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILD_LUMBERMILL_REC';

-- We Love the King Day

UPDATE Language_en_US
SET Text = 'Because you have {1_Resource:textkey}, the city of {2_CityName:textkey} enters "We Love the King Day", giving it a [ICON_FOOD] growth bonus!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CITY_WLTKD';

-- Founding Cities

UPDATE Language_en_US
SET Text = 'City Locations and Yields'
WHERE Tag = 'TXT_KEY_CITIES_WHERETOCONSTRUCT_HEADING2_TITLE';

	
UPDATE Language_en_US
SET Text = 'Cities should be founded in locations with plenty of food and production and with access to resources such as wheat, fish, and cattle. Cities founded on hills gain a defensive bonus, making it harder for enemies to capture them. It''s often a good idea to build a city on a river or coastal hex, as cities built in different locations have different starting yields:[NEWLINE][ICON_BULLET] Cities on hills: 2 [ICON_FOOD], 2 [ICON_PRODUCTION] Production.[NEWLINE][ICON_BULLET] Cities on flat land or mountains next to fresh water: 3 [ICON_FOOD] Food, 1 [ICON_PRODUCTION] Production.[NEWLINE][ICON_BULLET] Cities on mountains without fresh water: 2 [ICON_FOOD] Food, 2 [ICON_PRODUCTION] Production, 1 [ICON_GOLD] Gold.[NEWLINE][ICON_BULLET] Cities on flat land without fresh water: 2 [ICON_FOOD], 1 [ICON_PRODUCTION] Production, 1 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_CITIES_WHERETOCONSTRUCT_HEADING2_BODY';

-- Trade Routes
UPDATE Language_en_US
SET Text = 'Trade Routes are established between two cities of different civilizations, providing Gold to each every turn. The civilization that the Trade Route originates from gets a larger sum of Gold than the destination civilization. The amount of Gold a Trade Route produces is dependent upon the resource diversity and revenue of the origin city.[NEWLINE][NEWLINE]Trade Routes can also be created between two cities of the same civilization. Once the origin city has a Granary, it can send Food to the destination city, and once it has a Workshop it can send Production. These types of internal Trade Routes do not benefit the origin city.[NEWLINE][NEWLINE]You can only have a limited number of Trade Routes at any time. Researching certain technologies increases the number of routes you have available. For details about your existing Trade Routes, open the Trade Route Overview screen, located in the Additional Information drop-down menu.[NEWLINE][NEWLINE]The distance that Trade Routes can travel is affected by roads, terrain, hostile troops, and features. Keep this in mind when expanding, and when trying to link cities via trade routes.'
WHERE Tag = 'TXT_KEY_TRADE_ROUTES_HEADING2_BODY';

-- Warscore Diplo

UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]At war with {1_enemy} (Warscore: {2_Num})[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_AT_WAR_WITH';


-- New Influence Vector

UPDATE Language_en_US
SET Text = 'Public Opinion is calculated by comparing the [ICON_TOURISM] Influence of Civs with an Ideology to your own [ICON_TOURISM] Influence, but only if the other Civ is least [COLOR_MAGENTA]Popular[ENDCOLOR] with you, and the other Civ''s [ICON_TOURISM] Influence is higher than yours.[NEWLINE][NEWLINE]If both are true, a [COLOR_POSITIVE_TEXT]Content[ENDCOLOR] Public Opinion may cause [COLOR_NEGATIVE_TEXT]Ideological Pressure[ENDCOLOR], generating [ICON_HAPPINESS_4] Unhappiness. There are ways to combat this:[NEWLINE][ICON_BULLET]  Ideologies with fewer followers have a natural resistance to Ideological Pressure.[NEWLINE][ICON_BULLET]  Static or [COLOR_NEGATIVE_TEXT]Falling[ENDCOLOR] [ICON_TOURISM] Influence trends reduce a foreign Civ''s Ideological Pressure.[NEWLINE][ICON_BULLET]  Adopting or switching Ideologies creates a 30-Turn (standard speed) period of reduced Ideological Pressure.[NEWLINE][NEWLINE]See the summary below to see what is causing Ideological Pressure. If the amount of icons for your Ideology is not greater than all the others combined, then [COLOR_NEGATIVE_TEXT]Ideological Pressure[ENDCOLOR] is generated![NEWLINE][NEWLINE]'
WHERE Tag = 'TXT_KEY_CO_OPINION_TT_OVERVIEW';



UPDATE Language_en_US
SET Text = 'Destroy this trade route to gain [ICON_GOLD] Gold.[NEWLINE][NEWLINE]If you are not at war with the owner of the trade route, you will need to declare war before you plunder (unless an ability allows otherwise). Plundering a trade route going to another civilization will damage your relationship with the trade route owner and the destination civilization if you''re not currently at war.'
WHERE Tag = 'TXT_KEY_MISSION_PLUNDER_TRADE_ROUTE_HELP';


UPDATE Language_en_US
SET Text = 'Max Distance: {1_Num}'
WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_RANGE';

UPDATE Language_en_US
SET Text = 'Resource Diversity Modifier: {1_Num}%[NEWLINE][ICON_BULLET]   Our Local Resources: {2_Num}[NEWLINE][ICON_BULLET]   Their Local Resources: {3_Num}[NEWLINE]Routes to Cities with unique Resources earn more [ICON_GOLD] Gold, and Monopoly Resources count double.'
WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_RESOURCE_DIFFERENT';


UPDATE Language_en_US
SET Text = 'Artifact will be placed in nearest Great Work of Art slot. Artifact provides +3 [ICON_CULTURE] Culture and +4 [ICON_TOURISM] Tourism. Archaeologist will be consumed.'
WHERE Tag = 'TXT_KEY_CHOOSE_ARCH_ARTIFACT_RESULT';

UPDATE Language_en_US
SET Text = 'Ancient writing will be placed in nearest Great Work of Writing slot. Writing provides +3 [ICON_CULTURE] Culture and +4 [ICON_TOURISM] Tourism. Archaeologist will be consumed.'
WHERE Tag = 'TXT_KEY_CHOOSE_ARCH_WRITTEN_ARTIFACT_RESULT';


UPDATE Language_en_US
SET Text = 'A Luxury resource is designated as immoral, endangered, or otherwise inappropriate for use by the general population.  As long as the ban is in place, Civilizations receive no [ICON_HAPPINESS_1] Happiness or Monopoly benefits from this Luxury resource.'
WHERE Tag = 'TXT_KEY_RESOLUTION_BAN_LUXURY_HAPPINESS_HELP';

--Big UI Changes
UPDATE Language_en_US
SET Text = 'Victory Progress'
WHERE Tag = 'TXT_KEY_VP_TITLE';

UPDATE Language_en_US
SET Text = 'Demographics'
WHERE Tag = 'TXT_KEY_DEMOGRAPHICS_TITLE';

-- Happiness

UPDATE Language_en_US
SET Text = 'Empire-Wide [ICON_HAPPINESS_1] Happiness Total: {1_Num}'
WHERE Tag = 'TXT_KEY_TP_HAPPINESS_SOURCES';

UPDATE Language_en_US
SET Text = 'Empire-Wide [ICON_HAPPINESS_3] Unhappiness Total: {1_Num} (capped by City Population)'
WHERE Tag = 'TXT_KEY_TP_UNHAPPINESS_TOTAL';

UPDATE Language_en_US
SET Text = '[ICON_BULLET]{1_Num} is added per turn from positive [ICON_HAPPINESS_1] Happiness produced by Cities.'
WHERE Tag = 'TXT_KEY_TP_GOLDEN_AGE_ADDITION';

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
SET Text = '[COLOR_POSITIVE_TEXT]{1_Num}%[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CO_PUBLIC_OPINION_HAPPINESS';

UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]{1_Num}%[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CO_PUBLIC_OPINION_UNHAPPINESS';

UPDATE Language_en_US
SET Text = 'Current Empire-Wide [ICON_HAPPINESS_1] Happiness, from 0-100%[NEWLINE][NEWLINE]At 50% or below the civilization will start to lose cities to revolt and may change to the public''s Preferred Ideology (you can find that preference in the Public Opinion tooltip).'
WHERE Tag = 'TXT_KEY_CO_VICTORY_EXCESS_HAPPINESS_HEADER_TT';

UPDATE Language_en_US
SET Text = 'With [ICON_GOLD] Gold you may purchase land and units, or invest in buildings in your cities.'
WHERE Tag = 'TXT_KEY_TP_GOLD_EXPLANATION';

UPDATE Language_en_US
SET Text = '[NEWLINE][COLOR_POSITIVE_TEXT]Creating a Landmark in another civ''s territory will give you a temporary diplomatic boost with that nation.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CHOOSE_ARCH_LANDMARK_MAJOR_CIV';


UPDATE Language_en_US
SET Text = 'Alternative Energy'
WHERE Tag = 'TXT_KEY_TECH_ECOLOGY_TITLE';

UPDATE Language_en_US
SET Text = 'Renewable, or alternative energy is energy that is collected from renewable resources, which are naturally replenished on a human timescale, such as sunlight, wind, rain, tides, waves, and geothermal heat. Renewable energy often provides energy in four important areas: electricity generation, air and water heating/cooling, transportation, and rural (off-grid) energy services.'
WHERE Tag = 'TXT_KEY_TECH_ECOLOGY_DESC';

UPDATE Language_en_US
SET Text = 'Can Air Sweep to clear intercepting Units. If no interceptors found, it will deal 10% of it''s [ICON_RANGE_STRENGTH] Ranged Combat Strength to all enemy Air Units within 3 tiles of the target Tile.' 
WHERE Tag = 'TXT_KEY_PROMOTION_AIR_SWEEP_HELP';

UPDATE Language_en_US
SET Text = 'Order the unit to sweep an area for air and ground-based interceptors. If no interceptors found, it will deal 10% of it''s [ICON_RANGE_STRENGTH] Ranged Combat Strength to all enemy Air and Anti-Air Units in and adjacent to the target Tile.' 
WHERE Tag = 'TXT_KEY_INTERFACEMODE_AIR_SWEEP_HELP';

-- Bully Changes

UPDATE Language_en_US
SET Text = '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT](+{1_FactorScore}%) {@2_FactorDescription}[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_POSITIVE';

UPDATE Language_en_US
SET Text = '[NEWLINE][ICON_BULLET][COLOR_WARNING_TEXT]({1_FactorScore}%) {@2_FactorDescription}[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE';

UPDATE Language_en_US
SET Text = 'They are currently [COLOR_POSITIVE_TEXT]afraid[ENDCOLOR], and are willing to give up [COLOR_POSITIVE_TEXT]{1_FactorScore}%[ENDCOLOR] of max tribute:'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_AFRAID';

UPDATE Language_en_US
SET Text = 'They are currently [COLOR_WARNING_TEXT]resilient[ENDCOLOR], because you are {1_FactorScore}% below the tribute threshold:'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_RESILIENT';

UPDATE Language_en_US
SET Text = 'Has Pledges of Protection'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_PLEDGES_TO_PROTECT';

UPDATE Language_en_US
SET Text = 'Already bullied this turn'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_BULLIED_VERY_RECENTLY';

-- Quest Changes
UPDATE Language_en_US
SET Text = 'They want you to start a land or sea trade route to their city.'
WHERE Tag = 'TXT_KEY_CITY_STATE_QUEST_TRADE_ROUTE_FORMAL';

UPDATE Language_en_US
SET Text = 'We want you to start a trade route.'
WHERE Tag = 'TXT_KEY_CITY_STATE_QUEST_TRADE_ROUTE';

-- City-State
UPDATE Language_en_US
SET Text = Text 
	|| '[NEWLINE][NEWLINE]Quest Rewards Modifiers:'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]+' || (SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_FRIENDLY') || '%[ENDCOLOR] rewards from all [ICON_CITY_STATE] City-State Quests.'
WHERE Tag = 'TXT_KEY_CITY_STATE_PERSONALITY_FRIENDLY_TT';

UPDATE Language_en_US
SET Text = Text 
	|| '[NEWLINE][NEWLINE]Quest Rewards Modifiers:'
	|| '[NEWLINE][ICON_BULLET][COLOR_NEGATIVE_TEXT]' || (SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_HOSTILE') || '%[ENDCOLOR] rewards from all [ICON_CITY_STATE] City-State Quests.'
WHERE Tag = 'TXT_KEY_CITY_STATE_PERSONALITY_HOSTILE_TT';

UPDATE Language_en_US
SET Text = Text 
	|| '[NEWLINE][NEWLINE]Quest Rewards Modifiers:'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_CULTURE_IRRATIONAL')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_CULTURE] Culture.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_TOURISM_IRRATIONAL')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_TOURISM] Tourism.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GAP_IRRATIONAL')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GOLDEN_AGE] Golden Age Points.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GPP_IRRATIONAL')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_PEOPLE] Great People Points in the [ICON_CAPITAL] Capital.'
WHERE Tag = 'TXT_KEY_CITY_STATE_PERSONALITY_IRRATIONAL_TT';

UPDATE Language_en_US
SET Text = Text 
	|| '[NEWLINE][NEWLINE]Quest Rewards Modifiers:'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_FOOD_NEUTRAL')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_FOOD] Food.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_CULTURE_NEUTRAL')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_CULTURE] Culture.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_FAITH_NEUTRAL')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_PEACE] Faith.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GPP_NEUTRAL')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_PEOPLE] Great People Points in the [ICON_CAPITAL] Capital.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GPP_GLOBAL_NEUTRAL')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_PEOPLE] Great People Points in all Cities.'
WHERE Tag = 'TXT_KEY_CITY_STATE_PERSONALITY_NEUTRAL_TT';

UPDATE Language_en_US
SET Text = Text 
	|| '[NEWLINE][NEWLINE]Quest Rewards Modifiers:'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_CULTURE_CULTURED')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_CULTURE] Culture.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_TOURISM_CULTURED')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_TOURISM] Tourism.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GAP_CULTURED')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GOLDEN_AGE] Golden Age Points.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GENERAL_POINTS_CULTURED')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_GENERAL] Great General Points.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_ADMIRAL_POINTS_CULTURED')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_ADMIRAL] Great Admiral Points.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GPP_CULTURED')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_PEOPLE] Great People Points in the [ICON_CAPITAL] Capital.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GPP_GLOBAL_CULTURED')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_PEOPLE] Great People Points in all Cities.'
WHERE Tag = 'TXT_KEY_CITY_STATE_CULTURED_TT';

UPDATE Language_en_US
SET Text = Text 
	|| '[NEWLINE][NEWLINE]Quest Rewards Modifiers:'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_FOOD_MARITIME')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_FOOD] Food.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GOLD_MARITIME')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GOLD] Gold.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_ADMIRAL_POINTS_MARITIME')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_ADMIRAL] Great Admiral Points.'
WHERE Tag = 'TXT_KEY_CITY_STATE_MARITIME_TT';

UPDATE Language_en_US
SET Text = Text 
	|| '[NEWLINE][NEWLINE]Quest Rewards Modifiers:'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_FOOD_MERCANTILE')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_FOOD] Food.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GOLD_MERCANTILE')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_CITY_STATE_MERCANTILE_TT';

UPDATE Language_en_US
SET Text = Text 
	|| '[NEWLINE][NEWLINE]Quest Rewards Modifiers:'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_PRODUCTION_MILITARISTIC')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_PRODUCTION] Production.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_TOURISM_MILITARISTIC')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_TOURISM] Tourism.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GENERAL_POINTS_MILITARISTIC')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_GENERAL] Great General Points.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_EXPERIENCE_MILITARISTIC')/100.0 AS NUMERIC) || 'x[ENDCOLOR] Experience.'
WHERE Tag = 'TXT_KEY_CITY_STATE_MILITARISTIC_NO_UU_TT';

UPDATE Language_en_US
SET Text = Text 
	|| '[NEWLINE][NEWLINE]Quest Rewards Modifiers:'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_PRODUCTION_MILITARISTIC')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_PRODUCTION] Production.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_TOURISM_MILITARISTIC')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_TOURISM] Tourism.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GENERAL_POINTS_MILITARISTIC')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_GENERAL] Great General Points.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_EXPERIENCE_MILITARISTIC')/100.0 AS NUMERIC) || 'x[ENDCOLOR] Experience.'
WHERE Tag = 'TXT_KEY_CITY_STATE_MILITARISTIC_TT';

UPDATE Language_en_US
SET Text = Text 
	|| '[NEWLINE][NEWLINE]Quest Rewards Modifiers:'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_SCIENCE_RELIGIOUS')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_RESEARCH] Science.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_FAITH_RELIGIOUS')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_PEACE] Faith.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GAP_RELIGIOUS')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GOLDEN_AGE] Golden Age Points.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_GPP_GLOBAL_RELIGIOUS')/100.0 AS NUMERIC) || 'x[ENDCOLOR] [ICON_GREAT_PEOPLE] Great People Points in all Cities.'
	|| '[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]' || CAST((SELECT Value FROM Defines WHERE Name = 'MINOR_CIV_QUEST_REWARD_EXPERIENCE_RELIGIOUS')/100.0 AS NUMERIC) || 'x[ENDCOLOR] Experience.'
WHERE Tag = 'TXT_KEY_CITY_STATE_RELIGIOUS_TT';

INSERT INTO Language_en_US (Tag, Text)
VALUES	('TXT_KEY_CITY_STATE_CULTURED_TT_ANNEXED', 'A conquered [COLOR_POSITIVE_TEXT]Cultured[ENDCOLOR] City-State will provide you gifts of [ICON_CULTURE] Culture, allowing you to obtain Social Policies more quickly.'),
	('TXT_KEY_CITY_STATE_MILITARISTIC_NO_UU_TT_ANNEXED', 'A conquered [COLOR_POSITIVE_TEXT]Militaristic[ENDCOLOR] City-State will provide you occasional gifts of advanced military units.'),
	('TXT_KEY_CITY_STATE_MILITARISTIC_TT_ANNEXED', 'A conquered [COLOR_POSITIVE_TEXT]Militaristic[ENDCOLOR] City-State will provide you occasional gifts of advanced military units.[NEWLINE][NEWLINE]They know the secrets of the [COLOR_POSITIVE_TEXT]{@1_UniqueUnitName}[ENDCOLOR]. If you have researched [COLOR_CYAN]{@2_PrereqTech}[ENDCOLOR], they will provide that unit as their gift.'),
	('TXT_KEY_CITY_STATE_MARITIME_TT_ANNEXED', 'A conquered [COLOR_POSITIVE_TEXT]Maritime[ENDCOLOR] City-State will boost the [ICON_FOOD] Food of your [ICON_CAPITAL] Capital greatly, and all of your cities.'),
	('TXT_KEY_CITY_STATE_MERCANTILE_TT_ANNEXED', 'A conquered [COLOR_POSITIVE_TEXT]Mercantile[ENDCOLOR] City-State will provide extra [ICON_HAPPINESS_1] Happiness in your empire, and manufactures special luxury resources which cannot be acquired any other way.'),
	('TXT_KEY_CITY_STATE_RELIGIOUS_TT_ANNEXED', 'A conquered [COLOR_POSITIVE_TEXT]Religious[ENDCOLOR] City-State will increase your [ICON_PEACE] Faith, allowing you to empower your religion with new beliefs and units.');
	
-- Culture Overview
INSERT INTO Language_en_US (Tag, Text)
VALUES  ('TXT_KEY_CO_TAB_HISTORIC_EVENTS', ' Historic Events'),
	('TXT_KEY_CO_HISTORIC_EVENTS_HEADER', ' Historic Events'),
	('TXT_KEY_CO_CULTURE_OUTPUT', 'Current [ICON_CULTURE] Culture Output: '),
	('TXT_KEY_CO_HISTORIC_EVENTS_COUNTER', 'Total Historic Events Achieved: '),
	('TXT_KEY_CO_CULTURE_OUTPUT_TT', 'A fraction of [ICON_CULTURE] Culture generated per turn and instant yields of [ICON_CULTURE] Culture gained in the prior 10 turns influences the amount of [ICON_TOURISM] Tourism generated from [COLOR_POSITIVE_TEXT]Historic Events[ENDCOLOR].'),
	('TXT_KEY_CO_TOURISM_OUTPUT_TT', '[ICON_TOURISM] Tourism generated per turn and instant yields of [ICON_TOURISM] Tourism gained in the prior 10 turns influences the amount of Tourism generated from [COLOR_POSITIVE_TEXT]Historic Events[ENDCOLOR].'),
	('TXT_KEY_CO_HISTORIC_EVENTS_COUNTER_TT', 'This is how many [COLOR_POSITIVE_TEXT]Historic Events[ENDCOLOR] you have accomplished with your Civilization in total of all types!'),
	('TXT_KEY_CO_HISTORIC_EVENTS_HEADER_TT', 'This is the list of [COLOR_POSITIVE_TEXT]Historic Events[ENDCOLOR] available for you to achieve. You can unlock some more through Buildings or Social Policies.'),
	('TXT_KEY_CO_HISTORIC_EVENTS_TOURISM_HEADER_TT', 'This is the amount of [ICON_TOURISM] Tourism you will generate from this [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR] to all known Civilizations unless stated otherwise!'),
	('TXT_KEY_CO_HISTORIC_EVENT_ERA_CHANGE', '[ICON_RESEARCH] Era Transition'),
	('TXT_KEY_CO_HISTORIC_EVENT_WORLD_WONDER', '[ICON_WONDER] World Wonder Construction'),
	('TXT_KEY_CO_HISTORIC_EVENT_GREAT_PERSON', '[ICON_GREAT_PEOPLE] Great People Birth'),
	('TXT_KEY_CO_HISTORIC_EVENT_WON_WAR', '[ICON_WAR] Favorable Peace Treaty'),
	('TXT_KEY_CO_HISTORIC_EVENT_GOLDEN_AGE', '[ICON_GOLDEN_AGE] Entering a Golden Age'),
	('TXT_KEY_CO_HISTORIC_EVENT_DIG', '[ICON_RES_ARTIFACTS] Digging Archaeological Sites'),
	('TXT_KEY_CO_HISTORIC_EVENT_TRADE_LAND', '[ICON_CARAVAN] Trade with a Major City'), --Also Unused kinda it uses City Name
	('TXT_KEY_CO_HISTORIC_EVENT_TRADE_SEA', '[ICON_CARGO_SHIP] Trade with a Major City'), --Also Unused kinda it uses City Name
	('TXT_KEY_CO_HISTORIC_EVENT_TRADE_CS', '[ICON_CARAVAN]/[ICON_CARGO_SHIP] Trade with a [ICON_CITY_STATE] City-State'),
	('TXT_KEY_CO_HISTORIC_EVENT_CITY_FOUND_CAPITAL', 'Founding Capital ([ICON_CAPITAL])'), --Unused
	('TXT_KEY_CO_HISTORIC_EVENT_ERA_CHANGE_TT', 'Each [ICON_RESEARCH] Era you advance to will be a [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR]!'),
	('TXT_KEY_CO_HISTORIC_EVENT_WORLD_WONDER_TT', 'Each [ICON_WONDER] World Wonders you finish will be a [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR]!'),
	('TXT_KEY_CO_HISTORIC_EVENT_GREAT_PERSON_TT', 'Each [ICON_GREAT_PEOPLE] Great Person born in your Civilization will be a [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR]!'),
	('TXT_KEY_CO_HISTORIC_EVENT_WON_WAR_TT', 'Each [ICON_PEACE] favorable peace treaty ([COLOR_POSITIVE_TEXT]War Score 25+[ENDCOLOR]) you complete will be a [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR]!'),
	('TXT_KEY_CO_HISTORIC_EVENT_GOLDEN_AGE_TT', 'Each [ICON_GOLDEN_AGE] Golden Age you enter will be a [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR]!'),
	('TXT_KEY_CO_HISTORIC_EVENT_DIG_TT', 'Each [ICON_RES_ARTIFACTS] Archaeological Sites evacuated will be a [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR]!'),
	('TXT_KEY_CO_HISTORIC_EVENT_TRADE_LAND_TT', 'Each [ICON_CARAVAN] Land Trade Route completed from [COLOR_POSITIVE_TEXT]{1_Name}[ENDCOLOR] to another foreign Major City will be a [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR]! The amount here will be sent toward the targeted Major Civilization and one-third of the amount will be sent to every other Civilizations.'),
	('TXT_KEY_CO_HISTORIC_EVENT_TRADE_SEA_TT', 'Each [ICON_CARGO_SHIP] Sea Trade Route completed from [COLOR_POSITIVE_TEXT]{1_Name}[ENDCOLOR] to another foreign Major City will be a [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR]! The amount here will be sent toward the targeted Major Civilization and one-third of the amount will be sent to every other Civilizations.'),
	('TXT_KEY_CO_HISTORIC_EVENT_TRADE_CS_TT', 'Each [ICON_INTERNATIONAL_TRADE] Trade Route completed either by land or sea with a [ICON_CITY_STATE] City-State will be a [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR]! Unlike trading with a foreign Major City, this amount will be the same sent to all other Civilizations.'),
	('TXT_KEY_CO_HISTORIC_EVENT_CITY_FOUND', 'Founding a City'), --Unused
	('TXT_KEY_CO_HISTORIC_EVENT_CITY_CONQUEST', 'City Conquest'), --Unused
	('TXT_KEY_CO_HISTORIC_EVENT_PLAYER_TURNS_PASSED', 'Time Passage'), --Unused
	('TXT_KEY_CO_HISTORIC_EVENT_AI_TURNS_PASSED', 'Time Passage'); --Unused


-- Victory Panel
INSERT INTO Language_en_US (Tag, Text)
VALUES  ('TXT_KEY_VP_DIPLO_TT_OTHER_PLAYER_VASSALIZE_YOU', ' {1_PlayerName} controls you as a Vassal.'),
	('TXT_KEY_VP_DIPLO_TT_YOU_VASSALIZE_OTHER_PLAYER', ' You control {1_CivName} as a Vassal.'),
	('TXT_KEY_VP_DIPLO_TT_OTHER_PLAYER_VASSALIZE_OTHER_PLAYER', ' {1_PlayerName} controls {2_CivName} as a Vassal.'),
	('TXT_KEY_VP_DIPLO_TT_UNMET_PLAYER_VASSALIZE_UNMET_PLAYER', ' An unmet player controls another unmet civilization as a Vassal.'),
	('TXT_KEY_VP_DIPLO_CAPITALS_TEAM_LEADING_TT', 'Team {1_Team} controls {2_num} original [ICON_CAPITAL] Capitals/Vassals.'),
	('TXT_KEY_VP_DIPLO_CAPITALS_ACTIVE_PLAYER_LEADING_TT', 'You are leading by controlling {1_num} original [ICON_CAPITAL] Capitals/Vassals.'),
	('TXT_KEY_VP_DIPLO_CAPITALS_PLAYER_LEADING_TT', '{1_player} controls {2_num} original [ICON_CAPITAL] Capitals/Vassals.'),
	('TXT_KEY_VP_DIPLO_CAPITALS_UNMET_PLAYER_LEADING_TT', 'An unmet player controls {1_num} original [ICON_CAPITAL] Capitals/Vassals.'),
	('TXT_KEY_VP_DIPLO_TT_MINOR_PLAYER_CONTROLS_CAPITAL_ALLY', 'The City-State of {1_Name} controls {2_Num}. They contribute the City to their Ally. Break their alliance or take the [ICON_CAPITAL] Capital from them![NEWLINE][NEWLINE]'),
	('TXT_KEY_VP_DIPLO_TT_MINOR_PLAYER_CONTROLS_CAPITAL_VASSAL_ALLY', 'The City-State of {1_Name} controls {2_Num}. They contribute the City to their Ally''s Master. Break their alliance, liberate their ally, or take the [ICON_CAPITAL] Capital from them![NEWLINE][NEWLINE]'),
	('TXT_KEY_VP_DIPLO_TT_MINOR_PLAYER_CONTROLS_CAPITAL_NO_ALLY', 'The City-State of {1_Name} controls {2_Num}. They have no current ally. Ally with them or take the [ICON_CAPITAL] Capital from them![NEWLINE][NEWLINE]'),
	('TXT_KEY_VP_DIPLO_TT_MINOR_VASSAL_CONTROLS_CAPITAL', '{1_Name} controls {2_Num}. They contribute the City to their Master. Liberate them or take the [ICON_CAPITAL] Capital from them![NEWLINE][NEWLINE]');

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
SET Text = 'An unmet player needs [COLOR_POSITIVE_TEXT]{1_num}[ENDCOLOR] original [ICON_CAPITAL] Capitals/Vassals to win!.' 
WHERE Tag = 'TXT_KEY_VP_DIPLO_CAPITALS_UNMET_PLAYER_LEADING';
