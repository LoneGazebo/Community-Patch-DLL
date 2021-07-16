-- Game Options

INSERT INTO Language_en_US
			(Tag,											Text)
VALUES		('TXT_KEY_GAME_OPTION_BARB_GG_GA_POINTS',		'Barbarian GG/GA Points'),
			('TXT_KEY_GAME_OPTION_BARB_GG_GA_POINTS_HELP',	'Allows all players to accumulate Great General and Great Admiral points from fighting Barbarians.');
			
UPDATE Language_en_US
SET Text = 'Each time the game is loaded, the random number seed is regenerated. This means that if you reload the game, some randomized results and AI decisions might be different from the first time you played.'
WHERE Tag = 'TXT_KEY_GAME_OPTION_NEW_RANDOM_SEED_HELP';

-- Leaders

-- Assyria UA
INSERT INTO Language_en_US
			(Tag,										Text)
SELECT		'TXT_KEY_SCIENCE_BOOST_CONQUEST_ASSYRIA',	'Your soldiers found [ICON_RESEARCH] Science during the conquest of {1_Name}!'
WHERE EXISTS (SELECT * FROM CustomModOptions WHERE Name='ALTERNATE_ASSYRIA_TRAIT' AND Value= 1 );


-- Cities

-- Razing
UPDATE Language_en_US	
SET Text = '[COLOR_POSITIVE_TEXT]Razing[ENDCOLOR] The City will burn [ICON_RAZING] down each turn until it reaches 0 population, and is removed from the game. This produces a lot of [ICON_HAPPINESS_4] Unhappiness, but also greatly increases your [COLOR_POSITIVE_TEXT]War Score[ENDCOLOR] versus this player.'
WHERE Tag = 'TXT_KEY_POPUP_CITY_CAPTURE_INFO_RAZE';

-- Production Queue
UPDATE Language_en_US
SET Text = 'LEFT CLICK adds an additional item to the end of the production queue.[NEWLINE]CTRL + LEFT CLICK adds an additional item in front of the production queue.[NEWLINE]SHIFT + LEFT CLICK replaces everything in the production queue with the chosen item.'
WHERE Tag = 'TXT_KEY_CITYVIEW_QUEUE_PROD_TT';

-- Avoid Growth
UPDATE Language_en_US
SET Text = 'Click here to stop this city from growing in [ICON_CITIZEN] Population.'
WHERE Tag = 'TXT_KEY_CITYVIEW_FOCUS_AVOID_GROWTH_TT';

-- All Growth Modifier Tooltips with Icons
UPDATE Language_en_US
SET Text = '[NEWLINE][ICON_BULLET][ICON_CONNECTED] Empire Modifier: {1_Num}%'
WHERE Tag = 'TXT_KEY_FOODMOD_PLAYER';

UPDATE Language_en_US
SET Text = '[NEWLINE][ICON_BULLET][ICON_RELIGION_PANTHEON] Religious Beliefs Modifier: {1_Num}%'
WHERE Tag = 'TXT_KEY_FOODMOD_RELIGION';

UPDATE Language_en_US
SET Text = '[NEWLINE][ICON_BULLET][ICON_HAPPINESS_1] "We Love the King Day" Modifier: {1_Num}%'
WHERE Tag = 'TXT_KEY_FOODMOD_WLTKD';


-- Units

-- Movement
UPDATE Language_en_US
SET Text = 'MOVE UNIT'
WHERE Tag = 'TXT_KEY_MOVE_STACKED_UNIT';

UPDATE Language_en_US
SET Text = 'Move a unit to a tile where it can end the turn.'
WHERE Tag = 'TXT_KEY_MOVE_STACKED_UNIT_TT';

UPDATE Language_en_US
SET Text = 'Allows land units to embark and cross water tiles.'
WHERE Tag = 'TXT_KEY_ALLOWS_EMBARKING';

-- Capture Chance
UPDATE Language_en_US
SET Text = '[COLOR_CYAN]Capture chance if defeated: {1_Num}%[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_EUPANEL_CAPTURE_CHANCE';

-- Interception
UPDATE Language_en_US
SET Text = '[COLOR_WARNING_TEXT]{1_Number} Interceptors![ENDCOLOR]'
WHERE Tag = 'TXT_KEY_EUPANEL_VISIBLE_AA_UNITS';

-- Naval Melee Units
UPDATE Language_en_US
SET Text = 'The Trireme is the first naval unit available to civilizations in the game. It is a melee attack unit, engaging units at sea or cities (but it has no ranged attack). The Trireme is useful for clearing barbarian ships out of home waters and for limited exploration (it cannot end its turn on Deep Ocean hexes outside of the city borders unless it is from Polynesia).'
WHERE Tag = 'TXT_KEY_UNIT_TRIREME_STRATEGY';

-- Naval Ranged Units
UPDATE Language_en_US
SET Text = 'The Galleass is the first naval unit with a ranged attack generally available to the civilizations in the game. It is much stronger than earlier naval units but a bit slower. It cannot initiate melee combat. The Galleass is useful for clearing enemy ships out of shallow waters. It cannot end its turn on Deep Ocean hexes unless it is from Polynesia.'
WHERE Tag = 'TXT_KEY_UNIT_GALLEASS_STRATEGY';

-- Ocean travel change
UPDATE Language_en_US
SET Text = 'Can never end turn on an Ocean tile. May move through Ocean tiles if ending on a Coastal tile.'
WHERE Tag = 'TXT_KEY_PEDIA_PROMOTION_OCEAN_IMPASSABLE_HELP';

UPDATE Language_en_US
SET Text = 'Cannot end turn on an Ocean tile until you have researched [COLOR_POSITIVE_TEXT]Astronomy[ENDCOLOR]. May move through Ocean tiles if ending on a Coastal tile.'
WHERE Tag = 'TXT_KEY_PROMOTION_OCEAN_IMPASSABLE_ASTRO_HELP';

UPDATE Language_en_US
SET Text = 'Cannot End Turn on Ocean Tile until Astronomy'
WHERE Tag = 'TXT_KEY_PROMOTION_OCEAN_IMPASSABLE_ASTRO_HELP';

UPDATE Language_en_US
SET Text = 'Cannot End Turn on Ocean Tile'
WHERE Tag = 'TXT_KEY_PROMOTION_OCEAN_IMPASSABLE';

-- Siege Units
UPDATE Language_en_US
SET Text = 'Moves at half-speed in enemy territory'
WHERE Tag = 'TXT_KEY_PROMOTION_MUST_SET_UP';

UPDATE Language_en_US
SET Text = 'Moves at half-speed in enemy territory'
WHERE Tag = 'TXT_KEY_PEDIA_PROMOTION_MUST_SET_UP';

UPDATE Language_en_US
SET Text = 'First siege Unit of the game. Does major damage to Units and Cities from a distance. Moves at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_CATAPULT';

UPDATE Language_en_US
SET Text = 'The Catapult is a siege weapon, extremely useful when assaulting early enemy cities. It is slow and extremely vulnerable to enemy melee attack; it always should be supported by other units when it''s in the field. It moves at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_CATAPULT_STRATEGY';

UPDATE Language_en_US
SET Text = 'Siege Unit that does major damage from a distance. Moves at half-speed in enemy territory. Only the Romans may build it. This Unit has a higher [ICON_RANGE_STRENGTH] Ranged Combat Strength than the Catapult, which it replaces.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BALLISTA';

UPDATE Language_en_US
SET Text = 'The Ballista is a Roman unique unit that is more powerful than the Catapult it replaces. It is an excellent siege weapon and extremely useful against cities, but it is quite vulnerable to attack. Be sure to protect the Ballista with other military units. Moves at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_ROMAN_BALLISTA_STRATEGY';

UPDATE Language_en_US
SET Text = 'Powerful Medieval Siege Unit, good at reducing the defenses of enemy Cities. Moves at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_TREBUCHET';

UPDATE Language_en_US
SET Text = 'A Trebuchet is a medieval siege weapon, significantly more powerful than the earlier Catapult units. The Trebuchet moves at half-speed in enemy territory, but once it is in position it can do serious damage to a fortified enemy city. Defend your trebuchets with melee and ranged units to keep enemy units from getting too close.'
WHERE Tag = 'TXT_KEY_UNIT_TREBUCHET_STRATEGY';

UPDATE Language_en_US
SET Text = 'Anti-personnel rocket unit of the Medieval Era. Strong against land units, but weak against cities. Only Korea may build it. Replaces the Trebuchet. Moves at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_CIV5_KOREA_HWACHA_HELP';

UPDATE Language_en_US
SET Text = 'The Hwach''a is extremely effective against enemy land units. It is slow and vulnerable to enemy melee attack; it always should be supported by other units when it''s in the field. The Hwach''a moves at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_CIV5_KOREA_HWACHA_STRATEGY';

UPDATE Language_en_US
SET Text = 'Powerful Renaissance Era Siege Unit. Moves at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_CANNON';

UPDATE Language_en_US
SET Text = 'The Cannon is an important mid-game siege unit, devastating against civilizations which have not yet acquired them. Like other siege units, the Cannon moves at half-speed in enemy territory. The Cannon remains potent into the Industrial Era. It cannot see as far as other units, and should be protected with melee units.'
WHERE Tag = 'TXT_KEY_UNIT_CANNON_STRATEGY';

UPDATE Language_en_US
SET Text = 'The first Siege Unit in the game capable of firing 3 tiles away. Moves at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ARTILLERY';

UPDATE Language_en_US
SET Text = 'Artillery is a deadly siege weapon, more powerful than a Cannon and with a longer range. Like the Cannon, it has limited visibility and moves at half-speed in enemy territory, but its Ranged Combat strength is tremendous. Artillery also has the "indirect fire" ability, allowing it to shoot over obstructions at targets it cannot see (as long as other friendly units can see them). Like other siege weapons, Artillery is vulnerable to melee attack.'
WHERE Tag = 'TXT_KEY_UNIT_ARTILLERY_STRATEGY';

UPDATE Language_en_US
SET Text = 'Fast Siege Unit of the late-game. Unlike other Siege Units, it does not move at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ROCKET_ARTILLERY';

UPDATE Language_en_US
SET Text = 'Rocket Artillery is an extremely powerful siege weapon, far superior to the standard artillery. Rocket Artillery does not lose speed in enemy territory, making it much quicker than earlier guns. Although the Rocket Artillery''s sight range is limited, it can use indirect fire to attack targets it cannot see (as long as other friendly units can see them). Like all artillery, it is vulnerable to infantry or armor attack, and should be defended by appropriate units.'
WHERE Tag = 'TXT_KEY_UNIT_ROCKET_ARTILLERY_STRATEGY';


-- Buildings

-- National Wonders
UPDATE Language_en_US
SET Text = 'Provides additional spies based on a fraction of the number of City-States, and levels up all your existing spies. Also provides a 15% reduction in enemy spy effectiveness. Must have a Police Station in all cities.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_HELP';

UPDATE Language_en_US
SET Text = 'An important defensive National Wonder for a technology-driven civilization. The National Intelligence Agency provides additional spies, levels up all your existing spies, and provides a 15% reduction in enemy spy effectiveness. A civilization must have a Police Station in all cities before it can construct the National Intelligence Agency.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_STRATEGY';


-- Tile Yields

UPDATE Language_en_US	
SET Text = 'Base Yield:'
WHERE Tag = 'TXT_KEY_PEDIA_YIELD_LABEL';

UPDATE Language_en_US	
SET Text = 'Connected By:'
WHERE Tag = 'TXT_KEY_PEDIA_IMPROVEMENTS_LABEL';

UPDATE Language_en_US
SET Text = '{@1_ImprovementDescription}: +{3_Yield} {4_Icon} {@2_YieldDescription}'
WHERE Tag = 'TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_YIELDCHANGES';

UPDATE Language_en_US
SET Text = '{1_ImprovementName:textkey}: +{3_Num} {2_YieldType:textkey} (Not Fresh Water).'
WHERE Tag = 'TXT_KEY_NO_FRESH_WATER';

UPDATE Language_en_US
SET Text = '(Not Fresh Water)'
WHERE Tag = 'TXT_KEY_ABLTY_NO_FRESH_WATER_STRING';

UPDATE Language_en_US
SET Text = '{@1_ImprovementDescription}: +{3_Yield} {4_Icon} {@2_YieldDescription} {TXT_KEY_ABLTY_NO_FRESH_WATER_STRING}'
WHERE Tag = 'TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_NOFRESHWATERYIELDCHANGES';

UPDATE Language_en_US
SET Text = '{1_ImprovementName:textkey}: +{3_Num} {2_YieldType:textkey} (Fresh Water).'
WHERE Tag = 'TXT_KEY_FRESH_WATER';

UPDATE Language_en_US
SET Text = '(Fresh Water)'
WHERE Tag = 'TXT_KEY_ABLTY_FRESH_WATER_STRING';

UPDATE Language_en_US
SET Text = '{@1_ImprovementDescription}: +{3_Yield} {4_Icon} {@2_YieldDescription} {TXT_KEY_ABLTY_FRESH_WATER_STRING}'
WHERE Tag = 'TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_FRESHWATERYIELDCHANGES';

UPDATE Language_en_US
SET Text = '{TXT_KEY_ABLTY_FASTER_MOVEMENT_STRING} {@1_RouteDescription}'
WHERE Tag = 'TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_MOVEMENT';


-- Improvements

UPDATE Language_en_US
SET Text = 'A Citadel is a mighty fortification that can only be constructed by a Great General. You can construct a Citadel anywhere within your territory.[NEWLINE][NEWLINE]Upon constructing the Citadel, your Culture borders will also expand to surround the Citadel on all sides by one hex. If the new Culture border claims hexes already owned by another civilization, you will incur a diplomatic penalty as a result. Any unit stationed within a Citadel receives a 100% defensive strength bonus. Additionally, any enemy unit which ends its turn next to a Citadel takes 30 damage (damage does not stack with other Citadels).[NEWLINE][NEWLINE] The Acropolis in Athens, Greece, is an example of an early powerful Citadel. Such structures were almost impossible to take by direct attack, and they were capable of withstanding protracted sieges before they fell.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_CITADEL_TEXT';


-- CS Stuff

-- Merchant of Venice
UPDATE Language_en_US
SET Text = 'The Merchant of Venice will use his considerable wealth to purchase a City-State and place it under your control as a [ICON_PUPPET] Puppet City. You will also gain control over all of the City-State''s existing units.'
WHERE Tag = 'TXT_KEY_MISSION_BUY_CITY_STATE_HELP';

UPDATE Language_en_US
SET Text = 'Your Merchant of Venice bought a City-State!'
WHERE Tag = 'TXT_KEY_VENETIAN_MERCHANT_BOUGHT_CITY_STATE';

-- Bullying/Tribute
UPDATE Language_en_US
SET Text = 'Enslave {@1_Unit} - lose {2_NumInfluence} [ICON_INFLUENCE] Influence'
WHERE Tag = 'TXT_KEY_POPUP_MINOR_BULLY_UNIT_AMOUNT';

UPDATE Language_en_US
SET Text = 'If this City-State is more [COLOR_POSITIVE_TEXT]afraid[ENDCOLOR] of you than they are [COLOR_WARNING_TEXT]resilient[ENDCOLOR], you can demand one {@3_Unit} as tribute of at the cost of [ICON_INFLUENCE] Influence.  {1_FearLevel}{2_FactorDetails}'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_UNIT_TT';

UPDATE Language_en_US
SET Text = 'Someone demanded tribute recently'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_BULLIED_RECENTLY';

UPDATE Language_en_US
SET Text = 'Someone demanded tribute very recently'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_BULLIED_VERY_RECENTLY';

INSERT INTO Language_en_US
			(Tag,										Text)
SELECT		'TXT_KEY_POP_CSTATE_BULLY_FACTOR_MONGOL_TERROR',	'You annexed a City-State recently';

-- Fixed quest text - transitioned to new 'quest rewards' panel
UPDATE Language_en_US
SET Text = 'You have successfully destroyed the Barbarian Encampment as requested by {1_MinorCivName:textkey}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_KILL_CAMP';

UPDATE Language_en_US
SET Text = 'War breaks out between {2_CivName:textkey} and {1_TargetName:textkey}'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SUMMARY_QUEST_KILL_CITY_STATE';

UPDATE Language_en_US
SET Text = '{3_TargetName:textkey} has declared war on {1_CivName:textkey}, citing diplomatic concerns. Receive the [COLOR_POSITIVE_TEXT]full[ENDCOLOR] Quest Reward from {1_TargetName:textkey} by [COLOR_NEGATIVE_TEXT]destroying[ENDCOLOR] {3_CivName:textkey}, or a [COLOR_POSITIVE_TEXT]partial[ENDCOLOR] Quest Reward through [COLOR_POSITIVE_TEXT]allying[ENDCOLOR] both City-States. International pressure will force peace in [COLOR_POSITIVE_TEXT]{2_Num}[ENDCOLOR] turns, so act fast if you intend to intervene!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_KILL_CITY_STATE';

UPDATE Language_en_US
SET Text = 'Conquer {1_CityStateName:textkey} (full reward), or Ally both {1_CityStateName:textkey} and this City-State (partial reward).'
WHERE Tag = 'TXT_KEY_CITY_STATE_QUEST_KILL_CITY_STATE_FORMAL';

UPDATE Language_en_US
SET Text = '{1_TargetName:textkey} defeated by {2_CivName:textkey}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_KILL_CITY_STATE';

UPDATE Language_en_US
SET Text = 'The war between {2_CivName:textkey} and {1_TargetName:textkey} has ended, with {2_CivName:textkey} emerging as the clear victor. Your advisors worry that this outcome may lead to future conflict.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_KILL_CITY_STATE';

UPDATE Language_en_US
SET Text = 'As {2_MinorCivName:textkey} requested, you have successfully connected {1_ResourceName} to your trade network! Their merchants are quite pleased!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONNECT_RESOURCE';

UPDATE Language_en_US
SET Text = 'As {2_MinorCivName:textkey} requested, you have successfully constructed {1_WonderName}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONSTRUCT_WONDER';

UPDATE Language_en_US
SET Text = 'As {2_MinorCivName:textkey} requested, you have successfully created a {1_UnitName}! Their scholars are in awe.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_GREAT_PERSON';

UPDATE Language_en_US
SET Text = 'Your discovery of a new Natural Wonder has inspired the citizens of {1_MinorCivName:textkey}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_FIND_NATURAL_WONDER';

UPDATE Language_en_US
SET Text = 'Your discovery of {1_TargetName:textkey} has greatly pleased the merchants of {2_MinorCivName:textkey}.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_FIND_PLAYER';

UPDATE Language_en_US
SET Text = 'Your gift of gold has helped {2_MinorCivName:textkey} recover from the tribute paid to {1_BullyName:textkey}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_GIVE_GOLD';

UPDATE Language_en_US
SET Text = '{2_MinorCivName:textkey} enjoyed seeing its rival {1_TargetName:textkey} squirm and give up hard-earned resources.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_BULLY_CITY_STATE';

UPDATE Language_en_US
SET Text = 'Your promise to protect {2_MinorCivName:textkey} has helped them recover from the tribute paid to {1_BullyName:textkey}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_PLEDGE_TO_PROTECT';

UPDATE Language_en_US
SET Text = '{2_MinorCivName:textkey} is relieved that the injustice they suffered at the hands of {1_TargetName:textkey} is now known by major world powers.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_DENOUNCE_MAJOR';

UPDATE Language_en_US
SET Text = 'Citizens of {@2_MinorCivName} are glad to see {@1_ReligionName} come to their city and become the dominant religion.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_SPREAD_RELIGION';

UPDATE Language_en_US
SET Text = 'Citizens of {@1_MinorCivName} are glad to see a connected trade route.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_TRADE_ROUTE';

UPDATE Language_en_US
SET Text = 'You have impressed {1_MinorCivName:textkey} with your culture! They turn a blind eye to the culture of other civilizations. Civilizations that succeeded (ties are allowed):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_CULTURE';

UPDATE Language_en_US
SET Text = 'Another civilization has impressed {1_MinorCivName:textkey} with its culture. Your cultural growth was not enough. Civilizations that succeeded (ties are allowed):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_CULTURE';

UPDATE Language_en_US
SET Text = 'The priesthood of {1_MinorCivName:textkey} are moved by your piety. They shun the inferior faith of other civilizations. Civilizations that succeeded (ties are allowed):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_FAITH';

UPDATE Language_en_US
SET Text = 'Another civilization has impressed {1_MinorCivName:textkey} with its faith. Your faith growth was not enough. Civilizations that succeeded (ties are allowed):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_FAITH';

UPDATE Language_en_US
SET Text = '{1_MinorCivName:textkey} is amazed by your swift technological advancements! Their scientists and intellectuals dismiss other civilizations as stagnant. Civilizations that succeeded (ties are allowed):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_TECHS';

UPDATE Language_en_US
SET Text = 'Another civilization has impressed {1_MinorCivName:textkey} with its technological advancement. Your scientific discoveries were not enough. Civilizations that succeeded (ties are allowed):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_TECHS';


-- Happiness

-- City Revolt
UPDATE Language_en_US
SET Text = 'Because the Empire has at least 34% [ICON_HAPPINESS_4] Unhappiness (or because our Public Opinion is not Content), a City will revolt in [COLOR_NEGATIVE_TEXT]{1_Turns}[ENDCOLOR] turns. Based on current Unhappiness levels and Cultural/Ideological pressure, the City most likely to revolt is {2_CityName} and it will join {3_CivName}. Try to get your Empire''s Happiness level out of this range as soon as possible!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_POSSIBLE_CITY_REVOLT';

UPDATE Language_en_US
SET Text = 'Because the Empire has at least 34% [ICON_HAPPINESS_4] Unhappiness, the City of {1_CityName} has revolted and joined {2_CivName}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CITY_REVOLT';

UPDATE Language_en_US
SET Text = 'Because the empire has at least 34% [ICON_HAPPINESS_4] Unhappiness, an uprising has occurred in our territory!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_REBELS';

-- Pantheons Founded

UPDATE Language_en_US
SET Text = 'Your people have started worshipping a pantheon of gods. They have chosen the belief: [COLOR_POSITIVE_TEXT]{1_BeliefName}[ENDCOLOR] ({@2_BeliefDesc})'
WHERE Tag = 'TXT_KEY_NOTIFICATION_PANTHEON_FOUNDED_ACTIVE_PLAYER';

UPDATE Language_en_US
SET Text = '{@1_CivName} {@1: plural 1?has; 2?have;} started worshipping a pantheon of gods. They have chosen the belief: [COLOR_POSITIVE_TEXT]{2_BeliefName}[ENDCOLOR] ({@3_BeliefDesc})'
WHERE Tag = 'TXT_KEY_NOTIFICATION_PANTHEON_FOUNDED';

UPDATE Language_en_US
SET Text = 'An unknown civilization has started worshipping a pantheon of gods. They have chosen the belief: [COLOR_POSITIVE_TEXT]{1_BeliefName}[ENDCOLOR] ({@2_BeliefDesc})'
WHERE Tag = 'TXT_KEY_NOTIFICATION_PANTHEON_FOUNDED_UNKNOWN';

-- Religion

UPDATE Language_en_US
SET Text = 'Head of {1_ReligionName}'
WHERE Tag = 'TXT_KEY_RO_STATUS_FOUNDER';

UPDATE Language_en_US
SET Text = 'Can be purchased with [ICON_PEACE] Faith in any city with a majority Religion that has been enhanced. They can remove other religions from your cities (expending the Inquisitor) or be placed inside or adjacent to a city to protect it from Missionaries and Prophets trying to spread other religions into that city.'
WHERE Tag = 'TXT_KEY_UNIT_INQUISITOR_STRATEGY';


-- Espionage

UPDATE Language_en_US
SET Text = 'Conducting Espionage'
WHERE Tag = 'TXT_KEY_SPY_STATE_ESTABLISHED_SURVEILLANCE';

UPDATE Language_en_US
SET Text = 'Disrupting Plans'
WHERE Tag = 'TXT_KEY_SPY_STATE_ESTABLISHED_SURVEILLANCE_PROGRESS_BAR';

UPDATE Language_en_US
SET Text = 'Security Level'
WHERE Tag = 'TXT_KEY_EO_POTENTIAL';

UPDATE Language_en_US
SET Text = 'Security Level reflects the difficulty of Espionage in a City. The higher the value, the more time it will take to complete Spy Actions. The base value [COLOR_POSITIVE_TEXT](a scale, from 1 to 10)[ENDCOLOR] is based on the overall economic value of the City (relative to all other cities). Security is also affected by Espionage modifiers and buildings in the city, such as the Constabulary and the Police Station. Security also increases when a City has a powerful Counterspy.[NEWLINE][NEWLINE]Click to sort cities by their Security level.'
WHERE Tag = 'TXT_KEY_EO_POTENTIAL_SORT_TT';

UPDATE Language_en_US
SET Text = 'If your cities have low Security, you should consider protecting them. There are two ways to do this. You may move your own spies to your cities to act as counterspies that have a chance to catch and kill enemy spies before they steal something. You may also slow down how quickly enemy spies can steal things by constructing buildings like the Constabulary, Police Station, and the Great Firewall.'
WHERE Tag = 'TXT_KEY_EO_OWN_CITY_POTENTIAL_TT';

UPDATE Language_en_US
SET Text = '{1_SpyRank} {2_SpyName} is stealing from {3_CityName}.[NEWLINE]The current Security Level of {3_CityName} is {4_Num}.[NEWLINE][NEWLINE]Security reflects the vulnerability of a city to Espionage. The higher the value, the more protected the city. The base value [COLOR_POSITIVE_TEXT](a scale, from 1 to 10)[ENDCOLOR] is based on the overall prosperity and happiness of the city (relative to all other cities). Security may be decreased by Policies and Espionage buildings in the city, such as the Constabulary and the Police Station.'
WHERE Tag = 'TXT_KEY_EO_CITY_POTENTIAL_TT';

UPDATE Language_en_US
SET Text = '{1_SpyRank} {2_SpyName} cannot steal technologies from {3_CityName}.[NEWLINE][NEWLINE]The Security Level of {4_CityName} is {5_Num}.[NEWLINE][NEWLINE]Security reflects the vulnerability of a city to Espionage. The higher the value, the more protected the city. The base value [COLOR_POSITIVE_TEXT](a scale, from 1 to 10)[ENDCOLOR] is based on the overall prosperity and happiness of the city. Security may be decreased by Policies and Espionage buildings in the city, such as the Constabulary and the Police Station..'
WHERE Tag = 'TXT_KEY_EO_CITY_POTENTIAL_CANNOT_STEAL_TT';

UPDATE Language_en_US
SET Text = 'The Security Level of {1_CityName} is believed to be {2_Num}. Send a [ICON_SPY] Spy to this City to learn more about it.[NEWLINE][NEWLINE]Security reflects the vulnerability of a city to Espionage. The higher the value, the more protected the city. The base value [COLOR_POSITIVE_TEXT](a scale, from 1 to 10)[ENDCOLOR] is based on the overall prosperity and happiness of the city. Security may be decreased by Policies and Espionage buildings in the city, such as the Constabulary and the Police Station.'
WHERE Tag = 'TXT_KEY_EO_CITY_ONCE_KNOWN_POTENTIAL_TT';

UPDATE Language_en_US
SET Text = 'Options for {1_SpyRank} {2_SpyName}:[NEWLINE][NEWLINE][ICON_BULLET] Move to a City-State and attempt to [COLOR_POSITIVE_TEXT]Rig an Election[ENDCOLOR] or [COLOR_POSITIVE_TEXT]Stage a Coup[ENDCOLOR].[NEWLINE][ICON_BULLET] Move to a non-Capital City owned by a Major Civilization and attempt to [COLOR_POSITIVE_TEXT]Steal Technology[ENDCOLOR] and [COLOR_POSITIVE_TEXT]Uncover Intrigue[ENDCOLOR].[NEWLINE][ICON_BULLET] Move to a Capital City owned by a Major Civilization and attempt to [COLOR_POSITIVE_TEXT]Steal Technology[ENDCOLOR], [COLOR_POSITIVE_TEXT]Uncover Intrigue[ENDCOLOR], or [COLOR_POSITIVE_TEXT]Schmooze[ENDCOLOR] as a Diplomat.'
WHERE Tag = 'TXT_KEY_EO_SPY_MOVE_TT';

-- Fixed diacritics for spy names.
UPDATE Language_en_US
SET Text = 'Antônio'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_0';

UPDATE Language_en_US
SET Text = 'Estêvão'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_3';

UPDATE Language_en_US
SET Text = 'Fernão'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_4';

UPDATE Language_en_US
SET Text = 'Tomé'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_8';


-- Trade

-- Trade Deals

UPDATE Language_en_US
SET Text = 'Ends after: [NEWLINE]Turn {1_turn}'
WHERE Tag = 'TXT_KEY_DO_ENDS_ON';

-- Trade Routes
UPDATE Language_en_US
SET Text = 'You must be at war with the trade route owner.'
WHERE Tag = 'TXT_KEY_MISSION_PLUNDER_TRADE_ROUTE_DISABLED_HELP';

UPDATE Language_en_US
SET Text = 'You have discovered {1_Num} technologies that {2_CivName} does not know.[NEWLINE]They are receiving +{3_Num} [ICON_RESEARCH] Science on this route due to their Cultural Influence over you.'
WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_SCIENCE_EXPLAINED';

UPDATE Language_en_US
SET Text = '{1_CivName} has discovered {2_Num} technologies that you do not know.[NEWLINE]You are receiving +{3_Num} [ICON_RESEARCH] Science on this route due to your Cultural Influence over them.'
WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_SCIENCE_EXPLAINED';

-- Item Not Tradeable
UPDATE Language_en_US
SET Text = 'The other leader has not met any third party players, or you do not have an Embassy with this player.'
WHERE Tag = 'TXT_KEY_DIPLO_OTHER_PLAYERS_NO_PLAYERS_THEM';

UPDATE Language_en_US
SET Text = 'A current war prevents this action.'
WHERE Tag = 'TXT_KEY_DIPLO_ALREADY_AT_WAR';

UPDATE Language_en_US
SET Text = 'A City-State Alliance prevents this.'
WHERE Tag = 'TXT_KEY_DIPLO_NO_WAR_ALLIES';

UPDATE Language_en_US
SET Text = 'A City-State Alliance prevents this.'
WHERE Tag = 'TXT_KEY_DIPLO_MINOR_ALLY_AT_WAR';

UPDATE Language_en_US
SET Text = 'City-State is permanently at war.'
WHERE Tag = 'TXT_KEY_DIPLO_MINOR_PERMANENT_WAR';

-- Tooltip fix for Open Borders
UPDATE Language_en_US
SET Text = 'Allows the other player''s military or civilian Units to pass through one''s territory (lasts {1_Num} turns).[NEWLINE][NEWLINE]Note: Military Units belonging to different players may never stack.'
WHERE Tag = 'TXT_KEY_DIPLO_OPEN_BORDERS_TT';

-- Tooltip fix for Defensive Pact
UPDATE Language_en_US
SET Text = 'If either player is attacked by another major civilization, the other player will immediately and automatically go to war with the aggressor. This agreement lasts {1_Num} turns.'
WHERE Tag = 'TXT_KEY_DIPLO_DEF_PACT_TT';

-- Lua for Cities
UPDATE Language_en_US
SET Text = 'We have no tradeable cities and/or we do not have an Embassy with them.'
WHERE Tag = 'TXT_KEY_DIPLO_TO_TRADE_CITY_NO_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'They have no tradeable cities and/or they do not have an Embassy with us.'
WHERE Tag = 'TXT_KEY_DIPLO_TO_TRADE_CITY_NO_THEM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );


-- Civilopedia Refresh

UPDATE Language_en_US
SET Text = 'Forces defender to retreat if it inflicts more damage than it receives. A defender who cannot retreat takes 50% extra damage.'
WHERE Tag = 'TXT_KEY_PROMOTION_HEAVY_CHARGE_HELP';

UPDATE Language_en_US
SET Text = 'Only one combat unit may occupy a city at a time. That military unit is said to "Garrison" the city, and it adds a significant defensive bonus to the city. If a city is attacked while a Garrison is in the city, the Garrison will deflect some of the damage onto itself, thus offering the city even more protection. Be careful, however, as a Garrison can be destroyed this way.[NEWLINE][NEWLINE]Additional combat units may move through the city, but they cannot end their turn there. (So if you build a combat unit in a city with a garrison, you have to move one of the two units out before you end your turn.)'
WHERE Tag = 'TXT_KEY_CITIES_COMBATUNITS_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'A city''s owner may "garrison" a military unit inside the city to bolster its defenses. A portion of the garrisoned unit''s combat strength is added to the city''s strength. The garrisoned will divert part of the all damage to a city when the city is attacked. This can destroy the garrison, so be careful! If the city is captured, the garrisoned unit is destroyed.[NEWLINE][NEWLINE]A unit stationed in the city may attack surrounding enemy units, but if it does so the city loses its garrison bonus, and, if it is a melee attack, the unit may take damage during the combat as normal.'
WHERE Tag = 'TXT_KEY_COMBAT_GARRISONINCITIES_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'At the end of melee combat, one or both units may have sustained damage and lost "hit points." If a unit''s hit points are reduced to 0, that unit is destroyed. If after melee combat the defending unit has been destroyed and the attacker survives, the attacking unit moves into the defender''s hex [COLOR_POSITIVE_TEXT]unless[ENDCOLOR] defending a Citadel, Fort, or City, at which point the melee unit remains in place. If it moves, the winner will capture any non-military units in that hex. If the defending unit survives, it retains possession of its hex and any other units in the hex.[NEWLINE][NEWLINE]Most units use up all of their movement when attacking. Some however have the ability to move after combat - if they survive the battle and have movement points left to expend.[NEWLINE][NEWLINE]Any surviving units involved in the combat will receive "experience points" (XPs), which may be expended to give the unit promotions.'
WHERE Tag = 'TXT_KEY_COMBAT_MELEERESULTS_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'Great Generals are "Great People" skilled in the art of warfare. They provide combat bonuses - offensive and defensive bonuses both - to any friendly units within two tiles of their location. A Great General itself is a non-combat unit, so it may be stacked with a combat unit for protection. If an enemy unit ever enters the tile containing a Great General, the General is destroyed.[NEWLINE][NEWLINE]A Great General gives a combat bonus of 20% to units in the General''s tile and all friendly units within 2 tiles of the General.[NEWLINE][NEWLINE]Great Generals are created when your units have been in battle and also can be acquired from buildings, policies, beliefs, and tenets. See the section on "Great People" for more details.'
WHERE Tag = 'TXT_KEY_COMBAT_GREATGENERALS_HEADING2_BODY';

UPDATE Language_en_US
SET Text = 'War Score'
WHERE Tag = 'TXT_KEY_DIPLOMACY_PEACE_HEADING3_TITLE';

UPDATE Language_en_US
SET Text = 'War Score is ever-shifting status of a war between players. War Score can fluctuate from 100 to -100, where 100 is a total victory for you, and -100 a total victory for your opponent. War Score will also gradually decay over time, to highlight the declining value of past actions in a long, drawn-out conflict.[NEWLINE][NEWLINE]When declared, War Score starts at zero for both players. As you (or your opponent) destroy units, pillage tiles/trade units, capture civilians and take cities, your warscore will go up. The value of these actions varies based on the overall size of your opponent.[NEWLINE][NEWLINE]When it comes time to make peace, the War Score value gives you a good idea of what you should expect to gain from your opponent, or what they will ask of you. In the trade screen, the War Score value will be translated into a ''Max Peace'' value, which shows you exactly what you can take from your opponent (or vice-versa). When peace is concluded, the War Score returns to zero.'
WHERE Tag = 'TXT_KEY_DIPLOMACY_PEACE_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'Once you have acquired the Chivalry tech, you may engage in a Defensive Pact. Defensive Pacts are always mutual. If a signatory to a Defensive Pact is attacked, the other partner is automatically at war with the attacker.[NEWLINE][NEWLINE]A Defensive Pact lasts for 50 turns (on standard speed). When that time has elapsed, the pact lapses unless it is renegotiated.'
WHERE Tag = 'TXT_KEY_DIPLOMACY_DEFENSIVEPACT_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'The ruin provides a map of the nearest unrevealed City (lifting the fog of war from a number of tiles).'
WHERE Tag = 'TXT_KEY_BARBARIAN_MAP_HEADING4_BODY';

UPDATE Language_en_US
SET Text = 'When they fight barbarian units, your less well-trained units will gain experience points. However, any unit that has already acquired 45 XPs (or has exchanged that many for promotions) no longer gains XPs from fighting barbarians.'
WHERE Tag = 'TXT_KEY_BARBARIAN_POINTLIMITS_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'Each civilization in Civilization V has one or more special "national units." These units are unique to that civilization, and they are in some way superior to the standard version of that unit. The American civilization, for example, has a Minuteman unit, which is superior to the standard Musketman available to other civs. The Greek civ has the Hoplite unit, which replaces the Spearman.[NEWLINE][NEWLINE]See each civilization''s Civilopedia entry to discover its special unit.'
WHERE Tag = 'TXT_KEY_UNITS_NATIONAL_HEADING2_BODY';

UPDATE Language_en_US
SET Text = 'If another civ has captured a City-State and you capture it from them, you have the option to "liberate" that city-state. If you do so, you will receive a large amount of [ICON_INFLUENCE] Influence from the City-State, usually enough to make you [COLOR_POSITIVE_TEXT]Allies[ENDCOLOR] with it.'
WHERE Tag = 'TXT_KEY_CITYSTATE_LIBERATING_HEADING2_BODY';


-- Miscellaneous

UPDATE Language_en_US
SET Text = 'Allowed units receive [COLOR_POSITIVE_TEXT]{@1_PromotionName}[ENDCOLOR] Promotion: {@2_PromotionHelp}'
WHERE Tag = 'TXT_KEY_FREE_PROMOTION_FROM_TECH';

UPDATE Language_en_US
SET Text = '{1_Num} [ICON_CULTURE] from Great Works and Themes'
WHERE Tag = 'TXT_KEY_CULTURE_FROM_GREAT_WORKS';

-- Economic Advisor
UPDATE Language_en_US
SET Text = 'Our economy is being hamstrung by the number of units that we have. We should disband any unneeded units so that our civilization can operate at full capacity.'
WHERE Tag = 'TXT_KEY_ECONOMICAISTRATEGY_TOO_MANY_UNITS';

-- Reversed Tourism for Open Borders
UPDATE Language_en_US
SET Text = '+{1_Num}% Bonus for Open Borders from:[NEWLINE]   '
WHERE Tag = 'TXT_KEY_CO_CITY_TOURISM_OPEN_BORDERS_BONUS';

-- Barbarians
UPDATE Language_en_US
SET Text = 'Crom'
WHERE Tag = 'TXT_KEY_LEADER_BARBARIAN';

-- Notification
UPDATE Language_en_US
SET Text = '{1_CivName:textkey} lost {1_CivName: plural 1?its; other?their;} [ICON_CAPITAL] Capital'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SUMMARY_PLAYER_LOST_CAPITAL';

-- Demographics
UPDATE Language_en_US
SET Text = 'Deployable Troops.'
WHERE Tag = 'TXT_KEY_DEMOGRAPHICS_ARMY_MEASURE';

-- Fix for production ranking
UPDATE Language_en_US
SET Text = 'Ranks players by the average [ICON_PRODUCTION] Production generated by all of their cities.'
WHERE Tag = 'TXT_KEY_PROGRESS_SCREEN_PRODUCTION_TT';

-- World Congress Proposals, Which Civs benefit
UPDATE Language_en_US
SET Text = '[NEWLINE][NEWLINE]Civilizations that would be positively affected by this:'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_OPINIONS_POSITIVE';

UPDATE Language_en_US
SET Text = '[NEWLINE][NEWLINE]Civilizations that would be negatively affected by this:'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_OPINIONS_NEGATIVE';

-- Diplo Opinion Table Modifiers

-- Neutral Indicators
UPDATE Language_en_US
SET Text = 'ELIMINATED'
WHERE Tag = 'TXT_KEY_EMOTIONLESS';

UPDATE Language_en_US
SET Text = 'You have gone to war in the past.'
WHERE Tag = 'TXT_KEY_DIPLO_PAST_WAR_BAD';

-- Dispute Modifiers
UPDATE Language_en_US
SET Text = 'Territorial disputes strain your relationship.'
WHERE Tag = 'TXT_KEY_DIPLO_LAND_DISPUTE';

UPDATE Language_en_US
SET Text = 'You are competing for World Wonders.'
WHERE Tag = 'TXT_KEY_DIPLO_WONDER_DISPUTE';

UPDATE Language_en_US
SET Text = 'You are competing for the favor of the same City-States!'
WHERE Tag = 'TXT_KEY_DIPLO_MINOR_CIV_DISPUTE';

-- War Stuff
UPDATE Language_en_US
SET Text = 'They have some early concerns about your warmongering.'
WHERE Tag = 'TXT_KEY_DIPLO_WARMONGER_THREAT_MINOR';

UPDATE Language_en_US
SET Text = 'They are wary of the potential threat posed by your warmongering.'
WHERE Tag = 'TXT_KEY_DIPLO_WARMONGER_THREAT_MAJOR';

UPDATE Language_en_US
SET Text = 'They believe your warmongering has become an existential threat.'
WHERE Tag = 'TXT_KEY_DIPLO_WARMONGER_THREAT_SEVERE';

UPDATE Language_en_US
SET Text = 'They fear your warmongering will end this world in fire!'
WHERE Tag = 'TXT_KEY_DIPLO_WARMONGER_THREAT_CRITICAL';

UPDATE Language_en_US
SET Text = 'You plundered their trade routes!'
WHERE Tag = 'TXT_KEY_DIPLO_PLUNDERING_OUR_TRADE_ROUTES';

UPDATE Language_en_US
SET Text = 'You nuked them!'
WHERE Tag = 'TXT_KEY_DIPLO_NUKED';

UPDATE Language_en_US
SET Text = 'You captured their original capital.'
WHERE Tag = 'TXT_KEY_DIPLO_CAPTURED_CAPITAL';

-- Recent diplomatic actions
UPDATE Language_en_US
SET Text = 'Your recent diplomatic actions please them.'
WHERE Tag = 'TXT_KEY_DIPLO_ASSISTANCE_TO_THEM';

UPDATE Language_en_US
SET Text = 'Your recent diplomatic actions disappoint them.'
WHERE Tag = 'TXT_KEY_DIPLO_REFUSED_REQUESTS';

-- Player has done nice stuff
UPDATE Language_en_US
SET Text = 'We are trade partners.'
WHERE Tag = 'TXT_KEY_DIPLO_TRADE_PARTNER';

UPDATE Language_en_US
SET Text = 'We fought together against a common foe.'
WHERE Tag = 'TXT_KEY_DIPLO_COMMON_FOE';

UPDATE Language_en_US
SET Text = 'You freed their captured citizens!'
WHERE Tag = 'TXT_KEY_DIPLO_CIVILIANS_RETURNED';

UPDATE Language_en_US
SET Text = 'You built a Landmark in their territory.'
WHERE Tag = 'TXT_KEY_DIPLO_LANDMARKS_BUILT';

UPDATE Language_en_US
SET Text = 'You restored their civilization after they were annihilated!'
WHERE Tag = 'TXT_KEY_DIPLO_RESURRECTED';

UPDATE Language_en_US
SET Text = 'You liberated their original capital.'
WHERE Tag = 'TXT_KEY_DIPLO_LIBERATED_CAPITAL';

UPDATE Language_en_US
SET Text = 'You have liberated some of their people!'
WHERE Tag = 'TXT_KEY_DIPLO_CITIES_LIBERATED';

UPDATE Language_en_US
SET Text = 'They have an embassy in your capital.'
WHERE Tag = 'TXT_KEY_DIPLO_HAS_EMBASSY';

UPDATE Language_en_US
SET Text = 'You forgave them for spying.'
WHERE Tag = 'TXT_KEY_DIPLO_FORGAVE_FOR_SPYING';

UPDATE Language_en_US
SET Text = 'You have shared intrigue with them.'
WHERE Tag = 'TXT_KEY_DIPLO_SHARED_INTRIGUE';

-- Player has done mean stuff
UPDATE Language_en_US
SET Text = 'You stole their territory!'
WHERE Tag = 'TXT_KEY_DIPLO_CULTURE_BOMB';

UPDATE Language_en_US
SET Text = 'Your spies were caught stealing from them.'
WHERE Tag = 'TXT_KEY_DIPLO_CAUGHT_STEALING';

-- Player has asked us to do things we don't like
UPDATE Language_en_US
SET Text = 'You demanded they not settle near your lands!'
WHERE Tag = 'TXT_KEY_DIPLO_NO_SETTLE_ASKED';

UPDATE Language_en_US
SET Text = 'You asked them not to spy on you.'
WHERE Tag = 'TXT_KEY_DIPLO_STOP_SPYING_ASKED';

UPDATE Language_en_US
SET Text = 'You made a trade demand of them!'
WHERE Tag = 'TXT_KEY_DIPLO_TRADE_DEMAND';

UPDATE Language_en_US
SET Text = 'You forced them to pay tribute.'
WHERE Tag = 'TXT_KEY_DIPLO_PAID_TRIBUTE';

-- Denouncing
UPDATE Language_en_US
SET Text = 'We have denounced them!'
WHERE Tag = 'TXT_KEY_DIPLO_DENOUNCED_BY_US';

UPDATE Language_en_US
SET Text = 'They have denounced us!'
WHERE Tag = 'TXT_KEY_DIPLO_DENOUNCED_BY_THEM';

UPDATE Language_en_US
SET Text = 'You have denounced a leader they made a Declaration of Friendship with!'
WHERE Tag = 'TXT_KEY_DIPLO_HUMAN_DENOUNCED_FRIEND';

UPDATE Language_en_US
SET Text = 'We have denounced the same leaders!'
WHERE Tag = 'TXT_KEY_DIPLO_MUTUAL_ENEMY';

UPDATE Language_en_US
SET Text = 'Their friends or allies have denounced you!'
WHERE Tag = 'TXT_KEY_DIPLO_DENOUNCED_BY_PEOPLE_WE_TRUST_MORE';

-- Promises
UPDATE Language_en_US
SET Text = 'You made a promise not to declare war on them, and then broke it!'
WHERE Tag = 'TXT_KEY_DIPLO_MILITARY_PROMISE';

UPDATE Language_en_US
SET Text = 'You made a promise not to declare war on another civilization, and then broke it!'
WHERE Tag = 'TXT_KEY_DIPLO_MILITARY_PROMISE_BROKEN_WITH_OTHERS';

UPDATE Language_en_US
SET Text = 'You refused to move your troops away from their borders when they asked!'
WHERE Tag = 'TXT_KEY_DIPLO_MILITARY_PROMISE_IGNORED';

UPDATE Language_en_US
SET Text = 'You made a promise to stop expanding near them, and then broke it!'
WHERE Tag = 'TXT_KEY_DIPLO_EXPANSION_PROMISE';

UPDATE Language_en_US
SET Text = 'They asked you to stop expanding near them, and you ignored them!'
WHERE Tag = 'TXT_KEY_DIPLO_EXPANSION_PROMISE_IGNORED';

UPDATE Language_en_US
SET Text = 'You made a promise to stop buying land near them, and then broke it!'
WHERE Tag = 'TXT_KEY_DIPLO_BORDER_PROMISE';

UPDATE Language_en_US
SET Text = 'They asked you to stop buying land near them, and you ignored them!'
WHERE Tag = 'TXT_KEY_DIPLO_BORDER_PROMISE_IGNORED';

UPDATE Language_en_US
SET Text = 'You made a promise not to conquer a City-State protected by them, and then broke it!'
WHERE Tag = 'TXT_KEY_DIPLO_CITY_STATE_PROMISE';

UPDATE Language_en_US
SET Text = 'You made a promise not to conquer another civilization''s protected City-State, and then broke it!'
WHERE Tag = 'TXT_KEY_DIPLO_CITY_STATE_PROMISE_BROKEN_WITH_OTHERS';

UPDATE Language_en_US
SET Text = 'They asked you to stop attacking a City-State protected by them, and you ignored them!'
WHERE Tag = 'TXT_KEY_DIPLO_CITY_STATE_PROMISE_IGNORED';

UPDATE Language_en_US
SET Text = 'You made a promise to stop demanding tribute from a City-State protected by them, and then broke it!'
WHERE Tag = 'TXT_KEY_DIPLO_BULLY_CITY_STATE_PROMISE_BROKEN';

UPDATE Language_en_US
SET Text = 'They asked you to stop demanding tribute from a City-State protected by them, and you ignored them!'
WHERE Tag = 'TXT_KEY_DIPLO_BULLY_CITY_STATE_PROMISE_IGNORED';

UPDATE Language_en_US
SET Text = 'You made a promise to stop converting their cities, and then broke it!'
WHERE Tag = 'TXT_KEY_DIPLO_NO_CONVERT_PROMISE_BROKEN';

UPDATE Language_en_US
SET Text = 'They asked you to stop converting their cities, and you ignored them!'
WHERE Tag = 'TXT_KEY_DIPLO_NO_CONVERT_PROMISE_IGNORED';

UPDATE Language_en_US
SET Text = 'You made a promise to stop excavating their artifacts, and then broke it!'
WHERE Tag = 'TXT_KEY_DIPLO_NO_DIG_PROMISE_BROKEN';

UPDATE Language_en_US
SET Text = 'They asked you to stop excavating their artifacts, and you ignored them!'
WHERE Tag = 'TXT_KEY_DIPLO_NO_DIG_PROMISE_IGNORED';

UPDATE Language_en_US
SET Text = 'You made a promise to stop spying on them, and then broke it!'
WHERE Tag = 'TXT_KEY_DIPLO_SPY_PROMISE_BROKEN';

UPDATE Language_en_US
SET Text = 'They asked you to stop spying on them, and you ignored them!'
WHERE Tag = 'TXT_KEY_DIPLO_SPY_PROMISE_IGNORED';

UPDATE Language_en_US
SET Text = 'You made a promise to start a cooperative war against another empire, and then broke it!'
WHERE Tag = 'TXT_KEY_DIPLO_COOP_WAR_PROMISE';

-- Religion / Ideology
UPDATE Language_en_US
SET Text = 'They have happily adopted your religion in the majority of their cities.' -- note for translators: swapped HIS and MY text keys from vanilla
WHERE Tag = 'TXT_KEY_DIPLO_ADOPTING_HIS_RELIGION';

UPDATE Language_en_US
SET Text = 'You have adopted their religion in the majority of your cities.' -- note for translators: swapped HIS and MY text keys from vanilla
WHERE Tag = 'TXT_KEY_DIPLO_ADOPTING_MY_RELIGION';

UPDATE Language_en_US
SET Text = 'They are spreading their own religion, but you converted some of their cities to your religion.'
WHERE Tag = 'TXT_KEY_DIPLO_RELIGIOUS_CONVERSIONS';

UPDATE Language_en_US
SET Text = 'You have both chosen to adopt the {1_PolicyTree} Ideology.'
WHERE Tag = 'TXT_KEY_DIPLO_SAME_LATE_POLICY_TREES';

UPDATE Language_en_US
SET Text = 'You have chosen to adopt the {1_YourPolicyTree} Ideology, but they believe in {2_TheirPolicyTree}.'
WHERE Tag = 'TXT_KEY_DIPLO_DIFFERENT_LATE_POLICY_TREES';

-- Protected Minors
UPDATE Language_en_US
SET Text = 'You have killed City-States under their protection!'
WHERE Tag = 'TXT_KEY_DIPLO_PROTECTED_MINORS_KILLED';

UPDATE Language_en_US
SET Text = 'You have attacked City-States under their protection!'
WHERE Tag = 'TXT_KEY_DIPLO_PROTECTED_MINORS_ATTACKED';

UPDATE Language_en_US
SET Text = 'You have demanded tribute from City-States under their protection!'
WHERE Tag = 'TXT_KEY_DIPLO_PROTECTED_MINORS_BULLIED';

UPDATE Language_en_US
SET Text = 'They mistreated your protected City-States, and you didn''t look the other way!'
WHERE Tag = 'TXT_KEY_DIPLO_SIDED_WITH_MINOR';

-- Declaration of Friendship
UPDATE Language_en_US
SET Text = 'We have made a public Declaration of Friendship!'
WHERE Tag = 'TXT_KEY_DIPLO_DOF';

UPDATE Language_en_US
SET Text = 'We have made Declarations of Friendship with the same leaders!'
WHERE Tag = 'TXT_KEY_DIPLO_MUTUAL_DOF';

UPDATE Language_en_US
SET Text = 'You have made a Declaration of Friendship with one of their enemies!'
WHERE Tag = 'TXT_KEY_DIPLO_HUMAN_DOF_WITH_ENEMY';

-- Traitor Opinion
UPDATE Language_en_US
SET Text = 'Your friends found reason to denounce you!'
WHERE Tag = 'TXT_KEY_DIPLO_HUMAN_DENOUNCED_BY_FRIENDS';

UPDATE Language_en_US
SET Text = 'You have denounced leaders you''ve made Declarations of Friendship with!'
WHERE Tag = 'TXT_KEY_DIPLO_HUMAN_DENOUNCED_FRIENDS';

UPDATE Language_en_US
SET Text = 'We made a Declaration of Friendship and then denounced them!'
WHERE Tag = 'TXT_KEY_DIPLO_HUMAN_FRIEND_DENOUNCED';

UPDATE Language_en_US
SET Text = 'You have declared war on leaders you''ve made Declarations of Friendship with!'
WHERE Tag = 'TXT_KEY_DIPLO_HUMAN_DECLARED_WAR_ON_FRIENDS';

UPDATE Language_en_US
SET Text = 'We made a Declaration of Friendship and then declared war on them!'
WHERE Tag = 'TXT_KEY_DIPLO_HUMAN_FRIEND_DECLARED_WAR';

-- Reckless Expander
UPDATE Language_en_US
SET Text = 'They believe we are expanding our empire too aggressively!'
WHERE Tag = 'TXT_KEY_DIPLO_RECKLESS_EXPANDER';

-- World Congress
UPDATE Language_en_US
SET Text = 'They liked our proposal to the World Congress.'
WHERE Tag = 'TXT_KEY_DIPLO_LIKED_OUR_PROPOSAL';

UPDATE Language_en_US
SET Text = 'They disliked our proposal to the World Congress.'
WHERE Tag = 'TXT_KEY_DIPLO_DISLIKED_OUR_PROPOSAL';

UPDATE Language_en_US
SET Text = 'We passed their proposal in the World Congress.'
WHERE Tag = 'TXT_KEY_DIPLO_SUPPORTED_THEIR_PROPOSAL';

UPDATE Language_en_US
SET Text = 'We defeated their proposal in the World Congress.'
WHERE Tag = 'TXT_KEY_DIPLO_FOILED_THEIR_PROPOSAL';

UPDATE Language_en_US
SET Text = 'We helped relocate the World Congress to their lands.'
WHERE Tag = 'TXT_KEY_DIPLO_SUPPORTED_THEIR_HOSTING';


-- Civilization Leader Dialog

-- First Greeting
UPDATE Language_en_US
SET Text = 'I am Nebuchadnezzar of Babylon. Those fools outside say I''m a god, but that seems unlikely. Who are you?'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_FIRSTGREETING_1';

UPDATE Language_en_US
SET Text = 'I am Nebuchadnezzar. Are you real or just a phantom of my tortured senses?'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_FIRSTGREETING_3';

UPDATE Language_en_US
SET Text = 'Greetings, Stranger. I am Pachacuti, ruler of the peerless Incans. If there is any way that we can assist your inferior civilization, please do not hesitate to ask.'
WHERE Tag = 'TXT_KEY_LEADER_PACHACUTI_FIRSTGREETING_3';

-- Greeting (Friendly)
UPDATE Language_en_US
SET Text = 'Hello, friend. What brings you to my court this day?'
WHERE Tag = 'TXT_KEY_GENERIC_GREETING_POLITE_HELLO_1';

UPDATE Language_en_US
SET Text = 'Hello again, my friend.'
WHERE Tag = 'TXT_KEY_GENERIC_GREETING_POLITE_HELLO_3';

UPDATE Language_en_US
SET Text = 'Ah, hello again, friend! What can this mighty general do for you?'
WHERE Tag = 'TXT_KEY_LEADER_ALEXANDER_GREETING_POLITE_HELLO_1';

UPDATE Language_en_US
SET Text = 'God smiles upon you, my friend. What can I do for you?'
WHERE Tag = 'TXT_KEY_LEADER_HARUN_GREETING_POLITE_HELLO_1';

-- Greeting (Neutral)
UPDATE Language_en_US
SET Text = 'I trust that God willed your visit to Songhai, great leader.'
WHERE Tag = 'TXT_KEY_LEADER_ASKIA_GREETING_NEUTRAL_HELLO_1';

UPDATE Language_en_US
SET Text = 'Germany always looks forward to favorable relations with mighty empires such as yours.'
WHERE Tag = 'TXT_KEY_LEADER_BISMARCK_GREETING_NEUTRAL_HELLO_1';

UPDATE Language_en_US
SET Text = 'What brings you this far into the wilderness, leader?'
WHERE Tag = 'TXT_KEY_LEADER_HIAWATHA_GREETING_NEUTRAL_HELLO_1';

UPDATE Language_en_US
SET Text = 'Here to admire the French Empire, or perhaps to do business?'
WHERE Tag = 'TXT_KEY_LEADER_NAPOLEON_GREETING_NEUTRAL_HELLO_1';

UPDATE Language_en_US
SET Text = 'Well? Speak up. I can barely hear you over the blood pounding in my brain.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_GREETING_5';

-- Greeting (Hostile)
UPDATE Language_en_US
SET Text = 'You come to mighty Persia as you are? What do you want?'
WHERE Tag = 'TXT_KEY_LEADER_DARIUS_GREETING_HOSTILE_HELLO_1';

UPDATE Language_en_US
SET Text = 'I make it a habit to speak only to great leaders, but I shall make an exception for you. Hurry up now.'
WHERE Tag = 'TXT_KEY_LEADER_SULEIMAN_GREETING_HOSTILE_HELLO_1';

-- Greeting (Repeat)
UPDATE Language_en_US
SET Text = 'You sure do like talking to me, don''t you?'
WHERE Tag = 'TXT_KEY_GENERIC_GREETING_REPEAT_2';

-- Greeting (Aggressive Plot Buying)
UPDATE Language_en_US
SET Text = 'Your recent trend of buying land near us is concerning. For the sake of our future relationship, I advise that you be cautious in this matter. Now that that''s out of the way, what brings you here today?'
WHERE Tag = 'TXT_KEY_GENERIC_GREETING_AGGRESSIVE_PLOT_BUYING_1';

-- Greeting (Hostile, Aggressive Expansion)
UPDATE Language_en_US
SET Text = 'The spread of your cities and people is like a plague infecting the land. I look forward to the day I don''t have to see it anymore.'
WHERE Tag = 'TXT_KEY_GENERIC_GREETING_HOSTILE_AGGRESSIVE_EXPANSION_1';

-- Greeting (Hostile, Small Army)
UPDATE Language_en_US
SET Text = 'Just between you and me, your military is kind of the laughing stock of the world. Don''t let that get you down though; I''m sure someone will put you out of your misery soon.'
WHERE Tag = 'TXT_KEY_GENERIC_GREETING_HOSTILE_HUMAN_SMALL_ARMY';


-- Open Trade Screen
UPDATE Language_en_US
SET Text = 'Speak: anything to drown out the whispers.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_LETSHEARIT_1';


-- Can't Match Trade Offer
UPDATE Language_en_US
SET Text = 'We cannot offer you enough to make this a fair trade. We can, however, offer you this deal.'
WHERE Tag = 'TXT_KEY_GENERIC_TRADE_CANT_MATCH_OFFER_1';

UPDATE Language_en_US
SET Text = 'The deal we suggest may be an unfair exchange for you. Please inspect it closely.'
WHERE Tag = 'TXT_KEY_GENERIC_TRADE_CANT_MATCH_OFFER_2';


-- Offer Trade (Friendly)
UPDATE Language_en_US
SET Text = 'Does this trade interest you?'
WHERE Tag = 'TXT_KEY_LEADER_GENGHIS_TRADEREQUEST_HAPPY';

UPDATE Language_en_US
SET Text = 'It appears that you do have a reason for existing: to make this deal with me.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_TRADEREQUEST_HAPPY';

-- Offer Trade (Neutral)
UPDATE Language_en_US
SET Text = 'This offer will not be open long; think about it.'
WHERE Tag = 'TXT_KEY_LEADER_ENRICO_DANDOLO_TRADEREQUEST_NEUTRAL';

-- Offer Trade (Hostile)
UPDATE Language_en_US
SET Text = 'Here is our offer - and it is far better than you deserve.'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRADEREQUEST_ANGRY';

UPDATE Language_en_US
SET Text = 'You would do well to agree to our very fair and reasonable terms.'
WHERE Tag = 'TXT_KEY_LEADER_GENGHIS_TRADEREQUEST_ANGRY';

UPDATE Language_en_US
SET Text = 'You would allay my all-consuming misery by agreeing to the following.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_TRADEREQUEST_ANGRY';

-- Offer Trade (Luxury)
UPDATE Language_en_US
SET Text = 'It is God''s will that we seek out cooperation and trade with our fellow men when possible.'
WHERE Tag = 'TXT_KEY_LEADER_HARUN_LUXURY_TRADE_1';

UPDATE Language_en_US
SET Text = 'Ah, hello, my friend. I noticed that we could make a mutually beneficial deal. What do you think?'
WHERE Tag = 'TXT_KEY_LEADER_GANDHI_LUXURY_TRADE_1';

UPDATE Language_en_US
SET Text = 'Trade is the fruit of a relationship between two great kingdoms. What do you think of this offer, my friend?'
WHERE Tag = 'TXT_KEY_LEADER_RAMKHAMHAENG_LUXURY_TRADE_1';

-- Offer Trade (Open Borders)
UPDATE Language_en_US
SET Text = 'Opening our borders seems like a convenient agreement at the present time. Do you agree?'
WHERE Tag = 'TXT_KEY_LEADER_AUGUSTUS_OPEN_BORDERS_EXCHANGE_1';

UPDATE Language_en_US
SET Text = 'Opening our borders will greatly expedite the movement of troops. Should it be necessary, of course. What do you think?'
WHERE Tag = 'TXT_KEY_LEADER_NAPOLEON_OPEN_BORDERS_EXCHANGE_1';

-- Offer Trade (Research Agreement)
UPDATE Language_en_US
SET Text = 'I feel that a Research Agreement is a mutually beneficial arrangement that would help serve to cement our friendship.'
WHERE Tag = 'TXT_KEY_GENERIC_RESEARCH_AGREEMENT_OFFER_1';


-- Accept Trade (Friendly)
UPDATE Language_en_US
SET Text = 'Very well. Not that it will help either of us in the long run...we will all die soon enough.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_TRADE_YES_HAPPY';

UPDATE Language_en_US
SET Text = 'Your offer pleases me. Well done.'
WHERE Tag ='TXT_KEY_LEADER_RAMESSES_TRADE_YES_HAPPY';

UPDATE Language_en_US
SET Text = 'Happily agreed.'
WHERE Tag = 'TXT_KEY_LEADER_SEJONG_TRADE_YES_HAPPY';

-- Accept Trade (Neutral)
UPDATE Language_en_US
SET Text = 'Your offer is agreeable to us. We accept.'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRADE_YES_NEUTRAL';

-- Accept Trade (Hostile)
UPDATE Language_en_US
SET Text = 'Oh, very well; I suppose I must. We are agreed.'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRADE_YES_ANGRY';

UPDATE Language_en_US
SET Text = 'Smart move.'
WHERE Tag = 'TXT_KEY_LEADER_GAJAH_MADA_TRADE_YES_ANGRY';


-- Need More from Trade (Friendly)
UPDATE Language_en_US
SET Text = 'You''re almost there...'
WHERE Tag = 'TXT_KEY_LEADER_ALEXANDER_TRADE_NEEDMORE_HAPPY_3';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_ALEXANDER_TRADE_NEEDMORE_HAPPY_4';

UPDATE Language_en_US
SET Text = 'You''re almost there...'
WHERE Tag = 'TXT_KEY_LEADER_ASKIA_TRADE_NEEDMORE_HAPPY_3';

UPDATE Language_en_US
SET Text = 'You''re almost there...'
WHERE Tag = 'TXT_KEY_LEADER_AUGUSTUS_TRADE_NEEDMORE_HAPPY_3';

UPDATE Language_en_US
SET Text = 'You''re almost there...'
WHERE Tag = 'TXT_KEY_LEADER_BISMARCK_TRADE_NEEDMORE_HAPPY_3';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_BISMARCK_TRADE_NEEDMORE_HAPPY_4';

UPDATE Language_en_US
SET Text = 'You''re almost there...'
WHERE Tag = 'TXT_KEY_LEADER_CATHERINE_TRADE_NEEDMORE_HAPPY_3';

UPDATE Language_en_US
SET Text = 'You''re almost there...'
WHERE Tag = 'TXT_KEY_LEADER_DARIUS_TRADE_NEEDMORE_HAPPY_3';

UPDATE Language_en_US
SET Text = 'You''re almost there...'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRADE_NEEDMORE_HAPPY_3';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRADE_NEEDMORE_HAPPY_4';

UPDATE Language_en_US
SET Text = 'You''re almost there...'
WHERE Tag = 'TXT_KEY_LEADER_GANDHI_TRADE_NEEDMORE_HAPPY_3';

UPDATE Language_en_US
SET Text = 'You''re almost there...'
WHERE Tag = 'TXT_KEY_LEADER_HARUN_TRADE_NEEDMORE_HAPPY_3';

UPDATE Language_en_US
SET Text = 'You''re almost there...'
WHERE Tag = 'TXT_KEY_LEADER_HIAWATHA_TRADE_NEEDMORE_HAPPY_3';

UPDATE Language_en_US
SET Text = 'You''re almost there...'
WHERE Tag = 'TXT_KEY_LEADER_MONTEZUMA_TRADE_NEEDMORE_HAPPY_3';

UPDATE Language_en_US
SET Text = 'You''re almost there...'
WHERE Tag = 'TXT_KEY_LEADER_NAPOLEON_TRADE_NEEDMORE_HAPPY_3';

UPDATE Language_en_US
SET Text = 'You''re almost there...'
WHERE Tag = 'TXT_KEY_LEADER_ODA_TRADE_NEEDMORE_HAPPY_3';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_ODA_TRADE_NEEDMORE_HAPPY_4';

UPDATE Language_en_US
SET Text = 'You''re almost there...'
WHERE Tag = 'TXT_KEY_LEADER_RAMKHAMHAENG_TRADE_NEEDMORE_HAPPY_3';

UPDATE Language_en_US
SET Text = 'You''re almost there...'
WHERE Tag = 'TXT_KEY_LEADER_SULEIMAN_TRADE_NEEDMORE_HAPPY_3';

UPDATE Language_en_US
SET Text = 'You''re almost there...'
WHERE Tag = 'TXT_KEY_LEADER_WASHINGTON_TRADE_NEEDMORE_HAPPY_3';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_WASHINGTON_TRADE_NEEDMORE_HAPPY_4';

UPDATE Language_en_US
SET Text = 'You''re almost there...'
WHERE Tag = 'TXT_KEY_LEADER_WUZETIAN_TRADE_NEEDMORE_HAPPY_3';

-- Need More from Trade (Neutral)
UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_ALEXANDER_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_ASKIA_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_AUGUSTUS_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_BISMARCK_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_CATHERINE_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_DARIUS_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_GANDHI_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_HARUN_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_HIAWATHA_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_MONTEZUMA_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_NAPOLEON_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_ODA_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_RAMKHAMHAENG_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_SULEIMAN_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'I do not think that the trade, as it is currently configured, would be in the United States'' best interests. Would you care to improve your offer?'
WHERE Tag = 'TXT_KEY_LEADER_WASHINGTON_TRADE_NEEDMORE_NEUTRAL_1';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_WASHINGTON_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_WUZETIAN_TRADE_NEEDMORE_NEUTRAL_4';

-- Need More from Trade (Hostile)
UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_ALEXANDER_TRADE_NEEDMORE_ANGRY_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_BISMARCK_TRADE_NEEDMORE_ANGRY_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRADE_NEEDMORE_ANGRY_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_ODA_TRADE_NEEDMORE_ANGRY_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_WASHINGTON_TRADE_NEEDMORE_ANGRY_4';


-- Refuse Trade (Friendly)
UPDATE Language_en_US
SET Text = 'Do you seek to take advantage of our intimacy? Behave, or I shall be very cross with you.'
WHERE Tag = 'TXT_KEY_LEADER_CATHERINE_TRADE_NO_HAPPY';

UPDATE Language_en_US
SET Text = 'My dear friend, whilst I would personally give you everything we possess, unto our very last farthing, my advisors are somewhat less generous. I therefore must regretfully decline your offer.'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRADE_NO_HAPPY';

UPDATE Language_en_US
SET Text = 'I am sorry, but the voices tell me that I must decline your offer.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_TRADE_NO_HAPPY';

-- Refuse Trade (Neutral)
UPDATE Language_en_US
SET Text = 'We implore you not to waste our time with such foolishness.'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRADE_NO_NEUTRAL';

UPDATE Language_en_US
SET Text = 'You must be insane to insult me with such an offer. We refuse.'
WHERE Tag = 'TXT_KEY_LEADER_GENGHIS_TRADE_NO_NEUTRAL';

UPDATE Language_en_US
SET Text = 'Your offer is declined. I would be insulted; that is, if I cared anything about you.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_TRADE_NO_NEUTRAL';

-- Refuse Trade (Hostile)
UPDATE Language_en_US
SET Text = 'My husband is deceased, you know: Russia is no longer ruled by an idiot. We decline.'
WHERE Tag = 'TXT_KEY_LEADER_CATHERINE_TRADE_NO_ANGRY';

UPDATE Language_en_US
SET Text = 'I have conceived of a blind, almost overpowering dislike for you. The answer is "no."'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_TRADE_NO_ANGRY';

-- Refuse Trade (Insulting)
UPDATE Language_en_US
SET Text = 'This is not at all acceptable. If you wish for us to get anywhere, you must offer a fair deal.'
WHERE Tag = 'TXT_KEY_GENERIC_TRADE_REJECT_INSULTING_1';


-- Make Request for Help
UPDATE Language_en_US
SET Text = 'Unfortunately, things aren''t going terribly well over here. Can you spare something for a friend? I''ll try to return the favor when things are going better for me.'
WHERE Tag = 'TXT_KEY_GENERIC_REQUEST_ITEM_1';

UPDATE Language_en_US
SET Text = 'Circumstances have not been kind to the Songhai lately, friend. I believe God wishes to teach us that we cannot triumph alone.'
WHERE Tag = 'TXT_KEY_LEADER_ASKIA_RESPONSE_REQUEST_1';

UPDATE Language_en_US
SET Text = 'The United States would be indebted to you if you were to provide us assistance.'
WHERE Tag = 'TXT_KEY_LEADER_WASHINGTON_RESPONSE_REQUEST_1';


-- Make Demand (Friendly)
UPDATE Language_en_US
SET Text = 'My beloved friend, can you assist us in our hour of need?'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_DEMANDTRIBUTE_HAPPY';

-- Make Demand (Neutral)
UPDATE Language_en_US
SET Text = 'You have something I want. If you don''t hand it over quietly, I''ll have no choice but to take it by force.'
WHERE Tag = 'TXT_KEY_GENERIC_DEMAND_1';

UPDATE Language_en_US
SET Text = 'Give me what I want, and I may spare you...for now.'
WHERE Tag = 'TXT_KEY_LEADER_GAJAH_MADA_DEMANDTRIBUTE_NEUTRAL';

UPDATE Language_en_US
SET Text = 'The following tribute would improve my black humor, greatly increasing the odds that you would survive another day.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_DEMANDTRIBUTE_NEUTRAL';


-- Accept Demand (Friendly)
UPDATE Language_en_US
SET Text = 'It is our pleasure to assist our friend and ally in their hour of need.'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRIBUTE_YES_HAPPY';

UPDATE Language_en_US
SET Text = 'Here. I am nothing but the nightmare of a mad god; what good are such things to me?'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_TRIBUTE_YES_HAPPY';


-- Accept Demand (Neutral)
UPDATE Language_en_US
SET Text = 'You may have this bit of material wealth, yes. But such indiscretions are not easily forgotten.'
WHERE Tag = 'TXT_KEY_LEADER_BOUDICCA_TRIBUTE_YES_NEUTRAL';

UPDATE Language_en_US
SET Text = 'Oh, very well; I suppose we have no choice.'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRIBUTE_YES_NEUTRAL';

UPDATE Language_en_US
SET Text = 'I agree; if only to confound my advisors, who urge me to refuse.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_TRIBUTE_YES_NEUTRAL';

UPDATE Language_en_US
SET Text = 'It honors my people to help those in need.'
WHERE Tag = 'TXT_KEY_LEADER_POCATELLO_TRIBUTE_YES_NEUTRAL';

UPDATE Language_en_US
SET Text = 'Very well. It appears that I have no choice but to accede to your demand. But I warn you not to press us too hard.'
WHERE Tag = 'TXT_KEY_LEADER_WASHINGTON_TRIBUTE_YES_NEUTRAL';


-- Accept Demand (Hostile)
UPDATE Language_en_US
SET Text = 'Very well. It appears as if I have no choice. But someday, you will pay for this.'
WHERE Tag = 'TXT_KEY_LEADER_ALEXANDER_TRIBUTE_YES_ANGRY';

UPDATE Language_en_US
SET Text = 'You have us at a disadvantage now, but it may not always be so. We shall remember this.'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRIBUTE_YES_ANGRY';

UPDATE Language_en_US
SET Text = 'I will agree to your reprehensible demands.'
WHERE Tag = 'TXT_KEY_LEADER_MARIA_I_TRIBUTE_YES_ANGRY';

UPDATE Language_en_US
SET Text = 'Your fetid breath chokes the life from me! Take what you would and be gone.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_TRIBUTE_YES_ANGRY';

UPDATE Language_en_US
SET Text = 'Very well. I hope you may choke on it!'
WHERE Tag = 'TXT_KEY_LEADER_PACHACUTI_TRIBUTE_YES_ANGRY';


-- Refuse Demand (Neutral)
UPDATE Language_en_US
SET Text = 'Foolish Jackal! You would do well to go to Egypt, where such as you are worshipped. Here, you get nothing.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_TRIBUTE_NO_NEUTRAL';

-- Refuse Demand (Hostile)
UPDATE Language_en_US
SET Text = 'You dare insult me in such a manner? If you want something so badly, then do your best to come and take it.'
WHERE Tag = 'TXT_KEY_GENERIC_HUMAN_DEMAND_REFUSE_HOSTILE_1';

UPDATE Language_en_US
SET Text = 'If you want that, you''ll have to come and get it.'
WHERE Tag = 'TXT_KEY_GENERIC_HUMAN_DEMAND_REFUSE_HOSTILE_3';

UPDATE Language_en_US
SET Text = 'You presume to demand tribute from us? Go away, you hateful villain!'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRIBUTE_NO_ANGRY';


-- Aggressive Military Warning (Hostile)
UPDATE Language_en_US
SET Text = 'I see your armies amassed near my empire. If you wish to attack, then come. Don''t hide in the shadows like a child.'
WHERE Tag = 'TXT_KEY_HOSTILE_AGGRESSIVE_MILITARY_WARNING_1';

UPDATE Language_en_US
SET Text = 'Should your massed armies cross my border, they shall have the wrath of all of China unleashed upon them.'
WHERE Tag = 'TXT_KEY_WUZETIAN_HOSTILE_AGGRESSIVE_MILITARY_WARNING_1';

UPDATE Language_en_US
SET Text = 'Ha! What are your forces doing near my territory? If you wish to fight, know that you cannot beat me.'
WHERE Tag = 'TXT_KEY_NAPOLEON_HOSTILE_AGGRESSIVE_MILITARY_WARNING_1';


-- Human attacked protected City-State
UPDATE Language_en_US
SET Text = 'Your wanton aggression against {@1_MinorCivName} does not go unnoticed. You will face serious consequences should you continue your assault.'
WHERE Tag = 'TXT_KEY_ATTACKED_PROTECTED_CITY_STATE_1';


-- AI attacked protected City-State
UPDATE Language_en_US
SET Text = 'It''s come to my attention that I may have attacked a City-State with which you have had past relations. While it was not my goal to be at odds with your empire, this was deemed a necessary course of action.'
WHERE Tag = 'TXT_KEY_WE_ATTACKED_YOUR_MINOR_1';

UPDATE Language_en_US
SET Text = 'I''ve been informed that my empire has commenced an attack on a City-State friendly to you. I assure you, this was not on purpose, and I hope that this does not serve to drive us apart.'
WHERE Tag = 'TXT_KEY_WE_ATTACKED_YOUR_MINOR_2';


-- Human denounces AI
UPDATE Language_en_US
SET Text = 'So, is that how it is, then? Very well. I shall not forget this.'
WHERE Tag = 'TXT_KEY_RESPONSE_TO_BEING_DENOUNCED_1';


-- Human Refuses Request to Denounce Third Party (Not Forgiven)
UPDATE Language_en_US
SET Text = 'So that is the kind of ally you are, then? The rest of the world''s leaders will hear of your betrayal.[NEWLINE][NEWLINE]([COLOR_WARNING_TEXT]They have publicly denounced us! NOTE: You are not at war.[ENDCOLOR])'
WHERE Tag = 'TXT_KEY_DOF_NOT_HONORED_2';


-- Human declares war on AI
UPDATE Language_en_US
SET Text = 'Declaring war was a serious error in judgement on your part. That having been said, I will not hesitate to make you regret it.'
WHERE Tag = 'TXT_KEY_GENERIC_ATTACKED_STRONG_HOSTILE_4';

UPDATE Language_en_US
SET Text = 'Why, you... you... worthless barren mule! I will crush you!'
WHERE Tag = 'TXT_KEY_LEADER_DARIUS_ATTACKED_1';

UPDATE Language_en_US
SET Text = 'Unfortunately, not everybody in my country is as committed to non-violence as I am, so please let me apologize in advance if your forces are destroyed to the last man.'
WHERE Tag = 'TXT_KEY_LEADER_GANDHI_ATTACKED_2';

UPDATE Language_en_US
SET Text = 'We shall destroy you, you know. Do you care for some cheese?'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_ATTACKED_2';

UPDATE Language_en_US
SET Text = 'Oh well. I presume you know what you''re doing.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_ATTACKED_3';


-- AI War Bluff
UPDATE Language_en_US
SET Text = 'Foolish, foolish...'
WHERE Tag = 'TXT_KEY_LEADER_HIAWATHA_WARBLUFF_1';

UPDATE Language_en_US
SET Text = 'The drums - the drums! They pound in my brain, drowning out thoughts of vengeance.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_WARBLUFF_1';

UPDATE Language_en_US
SET Text = 'The next time, I shall not be so polite.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_WARBLUFF_4';

UPDATE Language_en_US
SET Text = 'Very well. The next time, I shall not be so polite!'
WHERE Tag = 'TXT_KEY_LEADER_WASHINGTON_WARBLUFF_2';


-- AI declares war on human
UPDATE Language_en_US
SET Text = 'I had a feeling that the two of us would someday meet in the field of battle. We will now see which of us is meant to be victorious.'
WHERE Tag = 'TXT_KEY_GENERIC_DOW_GENERIC_2';

UPDATE Language_en_US
SET Text = 'It is time to fight. May the best of us win.'
WHERE Tag = 'TXT_KEY_GENERIC_DOW_GENERIC_3';

UPDATE Language_en_US
SET Text = 'Destruction might be the fate which awaits me, but should I back down, only eventual defeat awaits anyway.'
WHERE Tag = 'TXT_KEY_GENERIC_DOW_DESPERATE_8';

UPDATE Language_en_US
SET Text = 'I believe that in another life, the two of us could have been friends. Unfortunately, that is not how it is now. I wish your armies luck.'
WHERE Tag = 'TXT_KEY_GENERIC_DOW_REGRET_1';

UPDATE Language_en_US
SET Text = 'So be it. My armies will extract it from your lands, then.'
WHERE Tag = 'TXT_KEY_WAR_DEMAND_REFUSED_1';

UPDATE Language_en_US
SET Text = 'Your many insults to us and to our loyal subjects will not go unpunished - prepare for war!'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_DECLAREWAR_1';

UPDATE Language_en_US
SET Text = 'I am sending a contingent of Indian soldiers on a goodwill tour of your major cities. I trust that they will be received with open arms.'
WHERE Tag = 'TXT_KEY_LEADER_GANDHI_DECLAREWAR_2';

UPDATE Language_en_US
SET Text = 'It was inevitable that we would come to blows, and now is as good a time as another.'
WHERE Tag = 'TXT_KEY_LEADER_GENGHIS_DECLAREWAR_1';

UPDATE Language_en_US
SET Text = 'Your people remain backwards and primitive. Clearly a change of regime is in order. Prepare for war!'
WHERE Tag = 'TXT_KEY_LEADER_PACHACUTI_DECLAREWAR_1';


-- Responses to human saying "No"
UPDATE Language_en_US
SET Text = 'I have no more business with the likes of you, then.'
WHERE Tag = 'TXT_KEY_GENERIC_HUMAN_RESPONSE_BAD_2';


-- AI Insults
UPDATE Language_en_US
SET Text = 'I couldn''t help but notice how pathetic you are. And when I realized it, I couldn''t help but share.'
WHERE Tag = 'TXT_KEY_GENERIC_INSULT_GENERIC_1';

UPDATE Language_en_US
SET Text = 'With an army as weak as yours, it''s a surprise your empire hasn''t succumb to a barbarian invasion.'
WHERE Tag = 'TXT_KEY_GENERIC_INSULT_MILITARY_2';

UPDATE Language_en_US
SET Text = 'I can hear your people wailing in sorrow all the way over in my empire. If you don''t do something soon, they might just pack up and find a real civilization to live in.'
WHERE Tag = 'TXT_KEY_GENERIC_INSULT_UNHAPPINESS_1';

UPDATE Language_en_US
SET Text = 'With your people so unhappy, it''s a shock to me that they''d keep you around as their leader.'
WHERE Tag = 'TXT_KEY_GENERIC_INSULT_UNHAPPINESS_2';

UPDATE Language_en_US
SET Text = 'Travelers have told us that your empire''s economy is in pretty sad shape. If you beg a little, I might give you a hand. Okay, actually, I won''t.'
WHERE Tag = 'TXT_KEY_GENERIC_INSULT_POPULATION_1';


-- AI wins war
UPDATE Language_en_US
SET Text = 'I hope you have learned your lesson. I am invincible!'
WHERE Tag = 'TXT_KEY_LEADER_NAPOLEON_WINWAR_1';

UPDATE Language_en_US
SET Text = 'That was refreshing. If the demons are sated, perhaps they will give us a little respite now.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_WINWAR_1';


-- AI defeated
UPDATE Language_en_US
SET Text = 'You have done well. I shall put in a good word for you to the demons.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_DEFEATED_1';

UPDATE Language_en_US
SET Text = 'This is why I prefer diplomacy to violence!'
WHERE Tag = 'TXT_KEY_LEADER_PEDRO_DEFEATED_1';


-- Report Coop War Plans to Target AI
UPDATE Language_en_US
SET Text = 'Impossible. You go too far.'
WHERE Tag = 'TXT_KEY_DIPLO_DISCUSS_HOW_DARE_YOU';


-- End Declaration of Friendship
UPDATE Language_en_US
SET Text = 'Our Declaration of Friendship must end.'
WHERE Tag = 'TXT_KEY_DIPLO_DISCUSS_MESSAGE_END_WORK_WITH_US';

UPDATE Language_en_US
SET Text = 'Your actions are unforgivable. There can never be reconciliation between our two nations.[NEWLINE][NEWLINE](You are no longer friends with them. If you denounce this player or declare war on them within the next [COLOR_WARNING_TEXT]10[ENDCOLOR] turns, you will receive a diplomatic penalty for backstabbing them.)'
WHERE Tag = 'TXT_KEY_NOW_UNFORGIVABLE_1';

UPDATE Language_en_US
SET Text = 'I did not want things to end up like this, but I cannot overlook your warmongering. Consider our Declaration of Friendship ended.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_WARMONGER_1';

UPDATE Language_en_US
SET Text = 'Your dealings with City-States in my sphere of influence are unacceptable. Consider our Declaration of Friendship ended.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_MINORS_1';

UPDATE Language_en_US
SET Text = 'I fear that our close proximity was destined to poison our relationship. Consider our Declaration of Friendship ended.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_LAND_1';

UPDATE Language_en_US
SET Text = 'Your continued greed in collecting wonders of the world has become a major issue. Consider our Declaration of Friendship ended.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_WONDERS_1';

UPDATE Language_en_US
SET Text = 'It is clear to me now that the paths we are traveling must ultimately drive us apart. Consider our Declaration of Friendship ended.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_VICTORY_1';

UPDATE Language_en_US
SET Text = 'Circumstances have changed, and I believe there is no longer a need for our Declaration of Friendship to continue. I hope you can understand.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_DEFAULT_1';

UPDATE Language_en_US
SET Text = 'I am sorry, but it is time for our Declaration of Friendship to end.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_DEFAULT_2';

-- Unimportant capitalization fixes
UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_AHMAD_ALMANSUR_AGREE_SHORT_2';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_ASHURBANIPAL_AGREE_SHORT_1';

UPDATE Language_en_US
SET Text = 'Certainly not.'
WHERE Tag = 'TXT_KEY_LEADER_ASHURBANIPAL_DISAGREE_SHORT_2';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_ATTILA_AGREE_SHORT_2';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_BOUDICCA_AGREE_SHORT_2';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_CASIMIR_AGREE_SHORT_1';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_DIDO_AGREE_SHORT_2';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_ENRICO_DANDOLO_AGREE_SHORT_2';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_GAJAH_MADA_AGREE_SHORT_2';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_GENGHIS_AGREE_SHORT_2';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_GUSTAVUS_ADOLPHUS_AGREE_SHORT_2';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_HARALD_AGREE_SHORT_2';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_KAMEHAMEHA_AGREE_SHORT_2';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_MARIA_AGREE_SHORT_2';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_MARIA_I_AGREE_SHORT_2';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_PACHACUTI_AGREE_SHORT_2';

UPDATE Language_en_US
SET Text = 'Certainly not.'
WHERE Tag = 'TXT_KEY_LEADER_PACHACUTI_DISAGREE_SHORT_1';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_PEDRO_AGREE_SHORT_2';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_POCATELLO_AGREE_SHORT_2';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_SEJONG_AGREE_SHORT_2';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_SELASSIE_AGREE_SHORT_2';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_SHAKA_AGREE_SHORT_2';

UPDATE Language_en_US
SET Text = 'Very well.'
WHERE Tag = 'TXT_KEY_LEADER_WILLIAM_AGREE_SHORT_2';