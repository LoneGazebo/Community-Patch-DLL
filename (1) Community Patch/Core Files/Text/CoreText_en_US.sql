--Texts for New Wonder and Building Production Modifiers, either by Trait, Improvement or UnitPromotion, among others
INSERT INTO Language_en_US
			(Tag,															Text)
VALUES		('TXT_KEY_PRODMOD_WONDER_UNITPROMOTION',						'[NEWLINE][ICON_BULLET]Unit Garrison Modifier for Wonders: {1_Num}%'),
			('TXT_KEY_PRODMOD_WONDER_TO_BUILDING_FROM_UNIT_TRAIT',			'[NEWLINE][ICON_BULLET]Trait Wonder Production Modifier for Buildings from Unit Garrison: {1_Num}%'),
			('TXT_KEY_PRODMOD_WONDER_TO_BUILDING_FROM_IMPROVEMENT_TRAIT',	'[NEWLINE][ICON_BULLET]Trait Wonder Production Modifier for Buildings from Improvements: {1_Num}%'),
			('TXT_KEY_PRODMOD_WONDER_TO_BUILDING_FROM_POLICY_TRAIT',		'[NEWLINE][ICON_BULLET]Trait Wonder Production Modifier for Buildings from Policies: {1_Num}%'),
			('TXT_KEY_PRODMOD_WONDER_TO_BUILDING_FROM_RELIGION_TRAIT',		'[NEWLINE][ICON_BULLET]Trait Wonder Production Modifier for Buildings from Beliefs: {1_Num}%'),
			('TXT_KEY_PRODMOD_WONDER_TO_BUILDING_FROM_RESOURCE_TRAIT',		'[NEWLINE][ICON_BULLET]Trait Wonder Production Modifier for Buildings from Resources: {1_Num}%'),
			('TXT_KEY_PRODMOD_WONDER_TO_BUILDING_FROM_PLAYER_TRAIT',		'[NEWLINE][ICON_BULLET]Trait Wonder Production Modifier for Buildings from Player Traits: {1_Num}%'),
			('TXT_KEY_PRODMOD_WONDER_TO_BUILDING_FROM_CITY_TRAIT',			'[NEWLINE][ICON_BULLET]Trait Wonder Production Modifier for Buildings from City: {1_Num}%'),
			('TXT_KEY_PRODMOD_WONDER_IMPROVEMENT',							'[NEWLINE][ICON_BULLET]Improvement Modifier for Wonders: {1_Num}%'),
			('TXT_KEY_RELIGIOUS_PRESSURE_STRING_EXTENDED',					'({1_Pressure} Pressure, +{2_PressurePerTurn} per Turn)'),
			('TXT_KEY_RELIGION_TOOLTIP_EXTENDED',							'{1_ReligionIcon} {2_NumFollowers} {2_NumFollowers: plural 1?Follower; other?Followers;} ({3_TotalPressure} + {4_PressurePerTurn} Pressure from {5_Num} {5_Num:plural 1?Source City; other?Source Cities;})'),
			('TXT_KEY_EUPANEL_EMBARKATION_DEFENSE',							'Embarkation Defense'),
			('TXT_KEY_EUPANEL_UNITCLASS_NEAR',								'Near Unit'),
			('TXT_KEY_EUPANEL_NEARBYPROMOTION_COMBAT_BONUS',				'Bonus from Nearby Unit'),
			('TXT_KEY_EUPANEL_NEARBYPROMOTION_CITY_COMBAT_BONUS',			'Near City'),
			('TXT_KEY_PRODMOD_MILITARY_UNITPROMOTION',						'[NEWLINE][ICON_BULLET]Unit Garrison Modifier for Military Units: {1_Num}%'),
			('TXT_KEY_PRODMOD_YIELD_UNITPROMOTION',							'[NEWLINE][ICON_BULLET]City Modifier from Unit Garrison: {1_Num}%'),
			('TXT_KEY_NOTIFICATION_CULTURE_UNIT',							'The birth of a {1_Resource:textkey} causes a Culture Boost in your Empire!'),
			('TXT_KEY_NOTIFICATION_SUMMARY_CULTURE_UNIT',					'A {1_Resource:textkey} has brought inspiration!');

-- Game Options

INSERT INTO Language_en_US
		(Tag,											Text)
VALUES	('TXT_KEY_GAME_OPTION_BARB_GG_GA_POINTS',		'Barbarian GG/GA Points'),
		('TXT_KEY_GAME_OPTION_BARB_GG_GA_POINTS_HELP',	'Allows all players to accumulate Great General and Great Admiral points from fighting Barbarians.');

UPDATE Language_en_US
SET Text = 'This option is unused and should not be enabled.'
WHERE Tag = 'TXT_KEY_GAME_OPTION_NEW_RANDOM_SEED_HELP';

-- Advisors
UPDATE Language_en_US
SET Text = 'I''m not sure if the {1_LongCivName:textkey} has an army at all. Any hostilities with them would be laughably one-sided.'
WHERE Tag = 'TXT_KEY_DIPLOSTRATEGY_MILITARY_STRENGTH_COMPARED_TO_US_PATHETIC';

-- Leaders

-- Assyria UA
INSERT INTO Language_en_US
		(Tag,										Text)
SELECT	'TXT_KEY_SCIENCE_BOOST_CONQUEST_ASSYRIA',	'Your soldiers found [ICON_RESEARCH] Science during the conquest of {1_Name}!'
WHERE EXISTS (SELECT * FROM CustomModOptions WHERE Name='ALTERNATE_ASSYRIA_TRAIT' AND Value= 1 );


-- Cities

UPDATE Language_en_US	
SET Text = 'Do you want to annex the puppet of {@1_CityName} into your empire? It will allow you to choose Production, but will increase your [ICON_HAPPINESS_4] Unhappiness and slow your acquisition of new Policies and Technologies. You will not be able to reverse this.'
WHERE Tag = 'TXT_KEY_POPUP_ANNEX_PUPPET';


UPDATE Language_en_US	
SET Text = '[NEWLINE]Requires {TXT_KEY_GRAMMAR_A_AN << {1_BuildingName:textkey}} in this City.'
WHERE Tag = 'TXT_KEY_NO_ACTION_UNIT_REQUIRES_BUILDING';

-- Razing
UPDATE Language_en_US	
SET Text = '[COLOR_POSITIVE_TEXT]Razing[ENDCOLOR] The City will burn [ICON_RAZING] down each turn until it reaches 0 population, and is removed from the game. This produces a lot of [ICON_HAPPINESS_4] Unhappiness, but also increases your [COLOR_POSITIVE_TEXT]War Score[ENDCOLOR] versus this player.'
WHERE Tag = 'TXT_KEY_POPUP_CITY_CAPTURE_INFO_RAZE';

-- Production Queue
UPDATE Language_en_US
SET Text = 'LEFT CLICK adds an additional item to the end of the production queue.[NEWLINE]CTRL + LEFT CLICK adds an additional item in front of the production queue.[NEWLINE]ALT + LEFT CLICK adds the chosen item to the end of the production queue on repeat.[NEWLINE]SHIFT + LEFT CLICK replaces everything in the production queue with the chosen item.[NEWLINE]H hides the chosen building from this city''s production options.'
WHERE Tag = 'TXT_KEY_CITYVIEW_QUEUE_PROD_TT';

-- Avoid Growth
UPDATE Language_en_US
SET Text = 'Click here to stop this city from growing in [ICON_CITIZEN] Population.'
WHERE Tag = 'TXT_KEY_CITYVIEW_FOCUS_AVOID_GROWTH_TT';

-- All Growth Modifier Tooltips with Icons
UPDATE Language_en_US
SET Text = '[NEWLINE][ICON_BULLET][ICON_CONNECTED] Empire Modifier from Policies etc: {1_Num}%'
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
SET Text = 'Allows land units to embark and cross water Tiles.'
WHERE Tag = 'TXT_KEY_ALLOWS_EMBARKING';

-- Capture Chance
UPDATE Language_en_US
SET Text = '[COLOR_CYAN]Capture chance if defeated[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_EUPANEL_CAPTURE_CHANCE';

-- Interception
UPDATE Language_en_US
SET Text = '[COLOR_WARNING_TEXT]{1_Number} Interceptors![ENDCOLOR]'
WHERE Tag = 'TXT_KEY_EUPANEL_VISIBLE_AA_UNITS';

-- Great Generals I/II Promotions
UPDATE Language_en_US
SET Text = '+50% [ICON_GREAT_GENERAL] Great General Points from combat.'
WHERE Tag = 'TXT_KEY_PROMOTION_SPAWN_GENERALS_I_HELP';

UPDATE Language_en_US
SET Text = '+100% [ICON_GREAT_GENERAL] Great General Points from combat.'
WHERE Tag = 'TXT_KEY_PROMOTION_SPAWN_GENERALS_II_HELP';

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
SET Text = 'Cannot End Turn on Ocean Tile'
WHERE Tag = 'TXT_KEY_PROMOTION_OCEAN_IMPASSABLE';

INSERT INTO Language_en_US
	(Tag, Text)
VALUES
	('TXT_KEY_PROMOTION_OCEAN_IMPASSABLE_HELP', 'Cannot end turn in [COLOR_NEGATIVE_TEXT]Ocean[ENDCOLOR].[NEWLINE]Can move through visible Ocean if destination is visible Coast.');

UPDATE UnitPromotions
SET Help = 'TXT_KEY_PROMOTION_OCEAN_IMPASSABLE_HELP'
WHERE Type = 'PROMOTION_OCEAN_IMPASSABLE';

UPDATE Language_en_US
SET Text = 'Cannot End Turn on Ocean Tile until Astronomy'
WHERE Tag = 'TXT_KEY_PROMOTION_OCEAN_IMPASSABLE_ASTRO';

UPDATE Language_en_US
SET Text = 'Cannot end turn in [COLOR_NEGATIVE_TEXT]Ocean[ENDCOLOR] until you have researched [COLOR_CYAN]{TXT_KEY_TECH_ASTRONOMY_TITLE}[ENDCOLOR].[NEWLINE]Can move through visible Ocean if destination is visible Coast.'
WHERE Tag = 'TXT_KEY_PROMOTION_OCEAN_IMPASSABLE_ASTRO_HELP';

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

-- Giant Death Robot
UPDATE Language_en_US
SET Text = 'Doesn''t benefit from defensive terrain bonuses, but is otherwise stronger than any other military unit in the game.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MECH';


-- Buildings

-- "Votes from World Wonders" -> "Votes from Wonders"
UPDATE Language_en_US
SET Text = '[NEWLINE][ICON_BULLET]{1_NumVotes} from Wonders'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_WONDER_VOTES';

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

UPDATE Language_en_US
SET Text = 'Adds a Moai (+1 [ICON_CULTURE] Culture) to the tile. Generates +1 [ICON_CULTURE] Culture for each adjacent Moai. Also provides +1 [ICON_GOLD] Gold after Flight is researched. Can only be built adjacent to water.'
WHERE Tag = 'TXT_KEY_BUILD_MOAI_HELP';

UPDATE Language_en_US
SET Text = 'Moai can only be built adjacent to water. If built next to another Moai, it provides additional [ICON_CULTURE] Culture. After the Flight technology is researched it also provides extra [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_MOAI_HELP';


-- CS Stuff

-- Merchant of Venice
UPDATE Language_en_US
SET Text = 'The Merchant of Venice will use his considerable wealth to purchase a City-State and place it under your control as a [ICON_PUPPET] Puppet City. You will also gain control over all of the City-State''s existing units.'
WHERE Tag = 'TXT_KEY_MISSION_BUY_CITY_STATE_HELP';

UPDATE Language_en_US
SET Text = 'Your Merchant of Venice bought a City-State!'
WHERE Tag = 'TXT_KEY_VENETIAN_MERCHANT_BOUGHT_CITY_STATE';

-- Militaristic UU Tooltip
UPDATE Language_en_US
SET Text = 'A befriended [COLOR_POSITIVE_TEXT]Militaristic[ENDCOLOR] City-State will provide you occasional gifts of advanced military units.[NEWLINE][NEWLINE]They know the secrets of the [COLOR_POSITIVE_TEXT]{@1_UniqueUnitName}[ENDCOLOR].  If you are their Ally and have researched [COLOR_CYAN]{@2_PrereqTech}[ENDCOLOR], they will provide that unit as their gift.'
WHERE Tag = 'TXT_KEY_CITY_STATE_MILITARISTIC_TT';

-- Bullying/Tribute
UPDATE Language_en_US
SET Text = 'Enslave {@1_Unit} - lose {2_NumInfluence} [ICON_INFLUENCE] Influence'
WHERE Tag = 'TXT_KEY_POPUP_MINOR_BULLY_UNIT_AMOUNT';

UPDATE Language_en_US
SET Text = 'If this City-State is more [COLOR_POSITIVE_TEXT]afraid[ENDCOLOR] of you than they are [COLOR_WARNING_TEXT]resilient[ENDCOLOR], you can demand one {@3_Unit} as tribute of at the cost of [ICON_INFLUENCE] Influence.  {1_FearLevel}{2_FactorDetails}'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_UNIT_TT';

INSERT INTO Language_en_US
		(Tag,										Text)
SELECT	'TXT_KEY_POP_CSTATE_BULLY_FACTOR_MONGOL_TERROR',	'You annexed a City-State recently';

-- Show influence change for rigged elections
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Chance to kill enemy spies: {1_Num}%[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_EO_SPY_COUNTER_INTEL_SUM_TT';

UPDATE Language_en_US
SET Text = '{1_RankName} {2_SpyName} is attempting to rig the election in {3_CityName} to increase our influence there.[NEWLINE][NEWLINE]Only one civilization may successfuly rig an election. If more than one spy is in a City-State, the highest ranked spy that has been in that City-State the longest has the greatest chance of successfully rigging the election in its favor. Successfully rigging elections also increases the success chance of a coup in the City-State.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]If you successfully rig the next election, your influence will increase by {4_Influence}.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_EO_SPY_RIGGING_ELECTIONS_TT';

UPDATE Language_en_US
SET Text = '{1_SpyRank} {2_SpyName} successfully rigged the local elections in {3_CityName}. Your influence was increased by {4_Num}. The influence of other civilizations was reduced.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_SUCCESS';

UPDATE Language_en_US
SET Text = 'After recent elections, your influence in {1_CityName} was surprisingly reduced by {2_Num}. This probably reflects the involvement of enemy spies rigging the local elections.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_ALERT';

UPDATE Language_en_US
SET Text = '{1_SpyRank} {2_SpyName} failed to rig the local elections in {3_CityName}. {4_CivShortDesc} succeeded in rigging them and have gained influence there. Your influence was reduced by {5_Num}.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_FAILURE';

-- Fixed quest text - transitioned to new 'quest rewards' panel
UPDATE Language_en_US
SET Text = 'You have successfully destroyed the Barbarian Encampment as requested by {1_MinorCivName:textkey}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_KILL_CAMP';

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
SET Text = '{1_Num} [ICON_PEACE] Faith is the minimum required to found the next religious Pantheon. If you wish to found a Pantheon, you must do it before there is an Enhanced Religion in the game (though you may always form one if there have not been as many Pantheons as the maximum number of Religions).'
WHERE Tag = 'TXT_KEY_TP_FAITH_NEXT_PANTHEON';

UPDATE Language_en_US
SET Text = '{1_Num} [ICON_PEACE] Faith is the minimum required for your next chance at a Great Prophet.'
WHERE Tag = 'TXT_KEY_TP_FAITH_NEXT_PROPHET';

UPDATE Language_en_US
SET Text = 'Can be purchased with [ICON_PEACE] Faith in any city with a majority Religion that has been enhanced. They can remove other religions from your cities (expending the Inquisitor) or be placed inside or adjacent to a city to protect it from Missionaries and Prophets trying to spread other religions into that city.'
WHERE Tag = 'TXT_KEY_UNIT_INQUISITOR_STRATEGY';

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

-- Trade Deals
UPDATE Language_en_US
SET Text = 'Ends after:[NEWLINE]Turn {1_turn}'
WHERE Tag = 'TXT_KEY_DO_ENDS_ON';

UPDATE Language_en_US
SET Text = 'Unlocks advanced trade options with this Civilization and reveals the location of their Capital.'
WHERE Tag = 'TXT_KEY_DIPLO_ALLOW_EMBASSY_TT';

UPDATE Language_en_US
SET Text = 'Allows the other player''s military or civilian Units to pass through one''s territory (lasts {1_Num} turns).[NEWLINE][NEWLINE]Note: Military Units belonging to different players may never stack.'
WHERE Tag = 'TXT_KEY_DIPLO_OPEN_BORDERS_TT';

UPDATE Language_en_US
SET Text = 'If either player is attacked by another major civilization, the other player will immediately and automatically go to war with the aggressor. This agreement lasts {1_Num} turns.[NEWLINE][NEWLINE]You can make Defensive Pacts with a maximum of [COLOR_CYAN]{2_Num}[ENDCOLOR] civs. This is based on the number of living, non-vassal civs in the world.'
WHERE Tag = 'TXT_KEY_DIPLO_DEF_PACT_TT';

UPDATE Language_en_US
SET Text = 'We already have an Embassy in their Capital!'
WHERE Tag = 'TXT_KEY_DIPLO_ALLOW_EMBASSY_HAVE';

UPDATE Language_en_US
SET Text = 'They already have an Embassy in our Capital!'
WHERE Tag = 'TXT_KEY_DIPLO_ALLOW_EMBASSY_THEY_HAVE';

UPDATE Language_en_US
SET Text = 'You do not have the Technology to establish an Embassy (Writing).'
WHERE Tag = 'TXT_KEY_DIPLO_ALLOW_EMBASSY_NO_TECH_PLAYER';

UPDATE Language_en_US
SET Text = 'They do not have the Technology to establish an Embassy (Writing).'
WHERE Tag = 'TXT_KEY_DIPLO_ALLOW_EMBASSY_NO_TECH_OTHER_PLAYER';

UPDATE Language_en_US
SET Text = 'We are already allowing Open Borders!'
WHERE Tag = 'TXT_KEY_DIPLO_OPEN_BORDERS_HAVE';

UPDATE Language_en_US
SET Text = 'They are already allowing Open Borders!'
WHERE Tag = 'TXT_KEY_DIPLO_OPEN_BORDERS_THEY_HAVE';

UPDATE Language_en_US
SET Text = 'Neither player yet has the Technology to trade this item (Civil Service).'
WHERE Tag = 'TXT_KEY_DIPLO_OPEN_BORDERS_NO_TECH';

UPDATE Language_en_US
SET Text = 'You need an Embassy with their Civilization to trade this item.'
WHERE Tag = 'TXT_KEY_DIPLO_YOU_NEED_EMBASSY_TT';

UPDATE Language_en_US
SET Text = 'They need an Embassy with your Civilization to trade this item.'
WHERE Tag = 'TXT_KEY_DIPLO_THEY_NEED_EMBASSY_TT';

UPDATE Language_en_US
SET Text = 'Both parties need an Embassy to trade this item.'
WHERE Tag = 'TXT_KEY_DIPLO_BOTH_NEED_EMBASSY_TT';

UPDATE Language_en_US
SET Text = 'We already have a Defensive Pact!'
WHERE Tag = 'TXT_KEY_DIPLO_DEF_PACT_EXISTS';

UPDATE Language_en_US
SET Text = 'Neither player yet has the Technology to trade this item (Chivalry).'
WHERE Tag = 'TXT_KEY_DIPLO_DEF_PACT_NO_TECH';

UPDATE Language_en_US
SET Text = 'We already have a Research Agreement!'
WHERE Tag = 'TXT_KEY_DIPLO_RESCH_AGREEMENT_EXISTS';

UPDATE Language_en_US
SET Text = 'One or both of us have already researched all Technologies.'
WHERE Tag = 'TXT_KEY_DIPLO_RESCH_AGREEMENT_ALL_TECHS_RESEARCHED';

UPDATE Language_en_US
SET Text = 'Neither player yet has the Technology to trade this item (Philosophy).'
WHERE Tag = 'TXT_KEY_DIPLO_RESCH_AGREEMENT_NO_TECH';

UPDATE Language_en_US
SET Text = 'A Declaration of Friendship is needed to trade this item.'
WHERE Tag = 'TXT_KEY_DIPLO_NEED_DOF_TT';

UPDATE Language_en_US
SET Text = 'A City-State Alliance prevents this action.'
WHERE Tag = 'TXT_KEY_DIPLO_MINOR_ALLY_AT_WAR';

UPDATE Language_en_US
SET Text = 'A City-State Alliance prevents this action.'
WHERE Tag = 'TXT_KEY_DIPLO_NO_WAR_ALLIES';

UPDATE Language_en_US
SET Text = 'A recent Peace Treaty prevents this action.'
WHERE Tag = 'TXT_KEY_DIPLO_FORCE_PEACE';

-- Trade Routes
UPDATE Language_en_US
SET Text = 'You must be at war with the trade route owner.'
WHERE Tag = 'TXT_KEY_MISSION_PLUNDER_TRADE_ROUTE_DISABLED_HELP';

UPDATE Language_en_US
SET Text = 'You have discovered {1_Num} {1_Num: plural 1?Technology; other?Technologies;} that {2_CivName} {2_CivName: plural 1?does; other?do;} not know.[NEWLINE]They are receiving +{3_Num} [ICON_RESEARCH] Science on this route due to their Cultural Influence over you.'
WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_SCIENCE_EXPLAINED';

UPDATE Language_en_US
SET Text = '{1_CivName} {1_CivName: plural 1?has; other?have;} discovered {2_Num} {2_Num: plural 1?Technology; other?Technologies;} that you do not know.[NEWLINE]You are receiving +{3_Num} [ICON_RESEARCH] Science on this route due to your Cultural Influence over them.'
WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_SCIENCE_EXPLAINED';

-- Lua for Cities
UPDATE Language_en_US
SET Text = 'We have no tradeable cities and/or we do not have an Embassy with them.'
WHERE Tag = 'TXT_KEY_DIPLO_TO_TRADE_CITY_NO_TT';

UPDATE Language_en_US
SET Text = 'They have no tradeable cities and/or they do not have an Embassy with us.'
WHERE Tag = 'TXT_KEY_DIPLO_TO_TRADE_CITY_NO_THEM';


-- Civilopedia Refresh

UPDATE Language_en_US
SET Text = 'Welcome to the Civilopedia! Here you will find detailed descriptions of all aspects of the game. Use the "Search" field to search for articles on any specific subject. Clicking on the tabs on the top edge of the screen will take you to the various major sections of the Civilopedia. The Navigation Bar on the left side of the screen will display the various entries within a section. Click on an entry to go directly there.[NEWLINE][NEWLINE]In the upper left-hand portion of the screen you will find the forward and back buttons which will help you navigate between pages. Click on the "X" on the upper right portion of the screen to return to the game.[NEWLINE][NEWLINE]Welcome to the Community Patch, a mod containing several bugfixes and AI improvements. [COLOR_YELLOW]Game Concepts that have been changed in the Community Patch are highlighted in yellow in the Civilopedia.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_PEDIA_HOME_PAGE_HELP_TEXT';

UPDATE Language_en_US
SET Text = 'Civilization V examines all of human history - from the deep past to the day after tomorrow. The "Game Concepts" portion of the Civilopedia explains some of the more important parts of the game - how to build and manage cities, how to fight wars, how to research technology, and so forth. The left Navigation Bar displays the major concepts; click on an entry to see the subsections within the concepts.[NEWLINE][NEWLINE][COLOR_YELLOW]Game Concepts that have been changed in the Community Patch are highlighted in yellow.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_PEDIA_GAME_CONCEPT_HELP_TEXT';

UPDATE Language_en_US
SET Text = 'Forces defender to retreat if it inflicts more damage than it receives. A defender who cannot retreat takes 50% extra damage.'
WHERE Tag = 'TXT_KEY_PROMOTION_HEAVY_CHARGE_HELP';

UPDATE Language_en_US
SET Text = 'Only one land and one naval unit may occupy a city at a time. A military unit in a city is said to "Garrison" the city, and it adds a significant defensive bonus to the city [COLOR_YELLOW]if it''s a land unit. If a city is attacked while a Garrison is in the city, the Garrison will deflect some of the damage onto itself, thus offering the city even more protection. Be careful, however, as a Garrison can be destroyed this way.[ENDCOLOR][NEWLINE][NEWLINE]Additional combat units may move through the city, but they cannot end their turn there. (So if you build a combat unit in a city with a garrison, you have to move one of the two units out before you end your turn.)[NEWLINE][NEWLINE][COLOR_YELLOW]Note also that naval units cannot perform any attacks while they are stationed in a city.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CITIES_COMBATUNITS_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Combat Units in Cities[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CITIES_COMBATUNITS_HEADING3_TITLE';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Garrison Units in Cities[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_COMBAT_GARRISONINCITIES_HEADING3_TITLE';

UPDATE Language_en_US
SET Text = 'A city''s owner may "garrison" a military unit inside the city to bolster its defenses. A portion of the garrisoned unit''s combat strength is added to the city''s strength. [COLOR_YELLOW]The garrisoned will divert part of the damage to a city when the city is attacked. This can destroy the garrison, so be careful![ENDCOLOR] If the city is captured, the garrisoned unit is destroyed.[NEWLINE][NEWLINE]'
WHERE Tag = 'TXT_KEY_COMBAT_GARRISONINCITIES_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Forts[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_COMBAT_FORT_HEADING3_TITLE';

UPDATE Language_en_US
SET Text = 'Once a civ has acquired the Engineering technology, workers can construct "forts" in friendly or neutral territory. Forts provide a hefty defensive bonus to units occupying them. Forts cannot be constructed in enemy territory. They can be constructed atop resources. [COLOR_YELLOW]Melee Units attacking from a Fort don''t leave the Fort even if they destroy the attacked enemy unit.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_COMBAT_FORT_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Siege Weapons[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_COMBAT_SEIGEWEAPONS_HEADING2_TITLE';

UPDATE Language_en_US
SET Text = 'Certain ranged weapons are classified as "siege weapons" - catapults, ballistae, trebuchets, and so forth. These units get combat bonuses when attacking enemy cities. They are extremely vulnerable to melee combat, and should be accompanied by melee units to fend off enemy assault.[NEWLINE][NEWLINE][COLOR_YELLOW]In Vox Populi and the Community Patch, siege units don''t have to be "set up" anymore. Instead, they move at half-speed in enemy territory.[ENDCOLOR][NEWLINE][NEWLINE]Siege weapons are important. It''s really difficult to capture a well-defended city without them!'
WHERE Tag = 'TXT_KEY_COMBAT_SEIGEWEAPONS_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Melee Combat Results[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_COMBAT_MELEERESULTS_HEADING3_TITLE';

UPDATE Language_en_US
SET Text = 'At the end of melee combat, one or both units may have sustained damage and lost "hit points." If a unit''s hit points are reduced to 0, that unit is destroyed. If after melee combat the defending unit has been destroyed and the attacker survives, the attacking unit moves into the defender''s hex [COLOR_YELLOW]unless defending a Citadel, Fort, or City, at which point the melee unit remains in place[ENDCOLOR]. If it moves, the winner will capture any non-military units in that hex. If the defending unit survives, it retains possession of its hex and any other units in the hex.[NEWLINE][NEWLINE]Most units use up all of their movement when attacking. Some however have the ability to move after combat - if they survive the battle and have movement points left to expend.[NEWLINE][NEWLINE]Any surviving units involved in the combat will receive "experience points" (XPs), which may be expended to give the unit promotions.'
WHERE Tag = 'TXT_KEY_COMBAT_MELEERESULTS_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'Great Generals are "Great People" skilled in the art of warfare. They provide combat bonuses - offensive and defensive bonuses both - to any friendly units within two tiles of their location. A Great General itself is a non-combat unit, so it may be stacked with a combat unit for protection. If an enemy unit ever enters the tile containing a Great General, the General is destroyed.[NEWLINE][NEWLINE]A Great General gives a combat bonus of 15% to units in the General''s tile and all friendly units within 2 tiles of the General.[NEWLINE][NEWLINE]Great Generals are created when your units have been in battle and also can be acquired from buildings, policies, beliefs, and tenets. See the section on "Great People" for more details.'
WHERE Tag = 'TXT_KEY_COMBAT_GREATGENERALS_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_GREEN]War Score[ENDCOLOR]'
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
SET Text = 'When they fight barbarian units, your less well-trained units will gain experience points. However, any unit that has already acquired [COLOR_YELLOW]45 XPs[ENDCOLOR] (or has exchanged that many for promotions) no longer gains XPs from fighting barbarians.'
WHERE Tag = 'TXT_KEY_BARBARIAN_POINTLIMITS_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Experience Points Limitations[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_BARBARIAN_POINTLIMITS_HEADING3_TITLE';

UPDATE Language_en_US
SET Text = 'Each civilization in Civilization V has one or more special "national units." These units are unique to that civilization, and they are in some way superior to the standard version of that unit. The American civilization, for example, has a Minuteman unit, which is superior to the standard Musketman available to other civs. The Greek civ has the Hoplite unit, which replaces the Spearman.[NEWLINE][NEWLINE]See each civilization''s Civilopedia entry to discover its special unit.'
WHERE Tag = 'TXT_KEY_UNITS_NATIONAL_HEADING2_BODY';

UPDATE Language_en_US
SET Text = 'If another civ has captured a City-State and you capture it from them, you have the option to "liberate" that city-state. If you do so, you will receive a large amount of [ICON_INFLUENCE] Influence from the City-State, usually enough to make you [COLOR_POSITIVE_TEXT]Allies[ENDCOLOR] with it.'
WHERE Tag = 'TXT_KEY_CITYSTATE_LIBERATING_HEADING2_BODY';


-- Miscellaneous


UPDATE Language_en_US
SET Text = 'They want you to defeat Barbarian units that are invading their territory. You are allowed to enter their territory until the Barbarians are defeated.'
WHERE Tag = 'TXT_KEY_CITY_STATE_QUEST_INVADING_BARBS_FORMAL';

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

UPDATE Language_en_US
SET Text = 'We''re having issues with our growing population, so we needn''t build growth buildings in our cities.'
WHERE Tag = 'TXT_KEY_ECONOMICAISTRATEGY_HALT_GROWTH_BUILDINGS';

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
SET Text = '[NEWLINE][NEWLINE]Civilizations positively affected by this:'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_OPINIONS_POSITIVE';

UPDATE Language_en_US
SET Text = '[NEWLINE][NEWLINE]Civilizations negatively affected by this:'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_OPINIONS_NEGATIVE';

-- Shorten the overview text to make room
UPDATE Language_en_US
SET Text = '[NEWLINE][NEWLINE]Our knowledge of other Civs'' desires:'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_VOTE_OPINIONS';


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
SET Text = ' (They strongly dislike warmongers.)'
WHERE Tag = 'TXT_KEY_WARMONGER_HATE_HIGH';

UPDATE Language_en_US
SET Text = ' (They dislike warmongers.)'
WHERE Tag = 'TXT_KEY_WARMONGER_HATE_MID';

UPDATE Language_en_US
SET Text = ' (They overlook modest warmongering.)'
WHERE Tag = 'TXT_KEY_WARMONGER_HATE_LOW';

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
SET Text = 'You stole their territory while at peace!'
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
SET Text = 'You have denounced one of their declared enemies!'
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
SET Text = 'You made a promise to stop settling near them, and then broke it!'
WHERE Tag = 'TXT_KEY_DIPLO_EXPANSION_PROMISE';

UPDATE Language_en_US
SET Text = 'They asked you to stop settling near them, and you ignored them!'
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
SET Text = 'You have adopted their religion in the majority of your cities.'  -- note for translators: swapped HIS and MY text keys from vanilla
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
SET Text = 'You have conquered City-States under their protection!'
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


-- AI denounces human
UPDATE Language_en_US
SET Text = 'It''s time I told the world of your sins.[NEWLINE][NEWLINE]([COLOR_WARNING_TEXT]They have publicly denounced us![ENDCOLOR])'
WHERE Tag = 'TXT_KEY_DENOUNCE_1';

UPDATE Language_en_US
SET Text = 'I''ve spoken to the other leaders about you. They need to know you are not to be trusted.[NEWLINE][NEWLINE]([COLOR_WARNING_TEXT]They have publicly denounced us![ENDCOLOR])'
WHERE Tag = 'TXT_KEY_DENOUNCE_2';

UPDATE Language_en_US
SET Text = 'Just so you''re aware, the other leaders have now heard the grim truth about you.[NEWLINE][NEWLINE]([COLOR_WARNING_TEXT]They have publicly denounced us![ENDCOLOR])'
WHERE Tag = 'TXT_KEY_DENOUNCE_3';

UPDATE Language_en_US
SET Text = 'I''ve had enough of you. I''ve told the other leaders of the world that working with you is a mistake.[NEWLINE][NEWLINE]([COLOR_WARNING_TEXT]They have publicly denounced us![ENDCOLOR])'
WHERE Tag = 'TXT_KEY_DENOUNCING_FRIEND_1';

UPDATE Language_en_US
SET Text = 'Alright, enough is enough. Informing everyone else of your ill deeds is long overdue.[NEWLINE][NEWLINE]([COLOR_WARNING_TEXT]They have publicly denounced us![ENDCOLOR])'
WHERE Tag = 'TXT_KEY_DENOUNCING_FRIEND_2';


-- Human Refuses Request to Denounce Third Party (Not Forgiven)
UPDATE Language_en_US
SET Text = 'Pathetic. So you now show your true colors. The world will know of your dishonor.[NEWLINE][NEWLINE]([COLOR_WARNING_TEXT]They have publicly denounced us![ENDCOLOR])'
WHERE Tag = 'TXT_KEY_DOF_NOT_HONORED_1';

UPDATE Language_en_US
SET Text = 'So that is the kind of ally you are, then? The rest of the world''s leaders will hear of your betrayal.[NEWLINE][NEWLINE]([COLOR_WARNING_TEXT]They have publicly denounced us![ENDCOLOR])'
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

UPDATE Language_en_US
SET Text = 'What cursed treachery is this? Have you no honor, no shame?'
WHERE Tag = 'TXT_KEY_LEADER_SEJONG_ATTACKED_1';

UPDATE Language_en_US
SET Text = 'You sad little person. Soon you shall feel the full weight of our mighty science bearing down upon you.'
WHERE Tag = 'TXT_KEY_LEADER_SEJONG_ATTACKED_2';

UPDATE Language_en_US
SET Text = 'Excellent, excellent! I have been looking for a reason to try out my newly outfitted army, and you have provided me with just the occasion.'
WHERE Tag = 'TXT_KEY_LEADER_SEJONG_ATTACKED_3';


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

UPDATE Language_en_US
SET Text = 'The Hall grows weary of your tiresome antics. We shall forcefully liberate your downtrodden, and lead them to enlightenment.'
WHERE Tag = 'TXT_KEY_LEADER_SEJONG_DECLAREWAR_1';

UPDATE Language_en_US
SET Text = 'Time waits for no man, and I feel its changeless march growing ever more urgent. You stand in our way, and I have not the resources left for more diplomatic means.'
WHERE Tag = 'TXT_KEY_LEADER_SEJONG_DECLAREWAR_2';

UPDATE Language_en_US
SET Text = 'I''ve experiments to run, research to be done, and here you stand like an aging wall to block my path. I shall remove you.'
WHERE Tag = 'TXT_KEY_LEADER_SEJONG_DECLAREWAR_3';


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

UPDATE Language_en_US
SET Text = 'My kingdom will always remember its true king...regardless of your petty victory.'
WHERE Tag = 'TXT_KEY_LEADER_CASIMIR_DEFEATED_3';


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


-- Kiev -> Kyiv
UPDATE Language_en_US
SET Text = 'Kyiv'
WHERE Tag = 'TXT_KEY_CITYSTATE_KIEV';

UPDATE Language_en_US
SET Text = 'Kyivan'
WHERE Tag = 'TXT_KEY_CITYSTATE_KIEV_ADJ';

UPDATE Language_en_US
SET Text = 'According to legend, Kyiv was founded by three Slavic brothers - Kyi, Schek, and Khoryv -around the beginning of the 6th Century AD. Since then, Kyiv, one of the oldest cities in Eastern Europe, has passed through times of great prominence and periods of relative obscurity. Throughout the centuries, however, it has remained the most important Slavic cultural center, playing a pivotal role in the development of the medieval East Slavic civilization.[NEWLINE][NEWLINE]Located on the Dnieper trade route between Constantinople and Scandinavia, Kyiv came under the control of local Varangian (Viking) nobility and became the nucleus of the state of Rus. During the 1000s, it reached its ascendancy, a cultural hub with over 400 churches, eight markets, docks and a caravansary, and magnificent homes. With the founding of the Kyiv-Mogilyanskaya Academy in 1632, the city became the center of Ukrainian learning and scholarship. But as the capital of Kyivan Rus, the city was repeatedly sacked and occupied.[NEWLINE][NEWLINE]In 1667 the city became part of the Russian Empire; Kyiv remained an ecclesiastical and cultural center, but its political and economic influence was marginalized. Kyiv experienced growing "Russification" in the 1800s, as a result of Russian migration, educational policies, and industrialization. However, enthusiasts among ethnic Ukrainian nobles, military officers and merchants kept the native culture alive in Kyiv, often through clandestine activities such as book-printing, amateur theater, private soirees, folk festivals and the like.[NEWLINE][NEWLINE]Forcefully incorporated into the Soviet Union, the city weathered all manner of troubles: the famine of 1932-1933 devastated the lower classes, Stalin''s 1937-1938 purges almost obliterated the city''s intelligensia, and the 1941-1943 Nazi invasion brought death and destruction. But Kyiv survived, and in the wake of the collapse of the Soviet Union an independent Ukraine was proclaimed in 1991, with resurgent Kyiv as its capital and cultural heart.'
WHERE Tag = 'TXT_KEY_CIV5_KIEV_TEXT';

UPDATE Language_en_US
SET Text = 'The Eastern Orthodox Church is currently the second-largest Christian denomination, with an estimated 300 million adherents. The goal of Orthodox Christians is to draw nearer to God throughout their lives through theosis, a spiritual pilgrimage to become more "Christ-like."[NEWLINE][NEWLINE]The church traces its roots to the Great Schism. During the 9th and 10th centuries AD the Christian church under the Patriarch of Constantinople made significant conversions among the peoples of eastern Europe, including Kyivan Russia and the Balkans. Doctrinal issues such as the filioque split and the authority of the Pope over the Patriarch in matters religious, exacerbated by the political and economic rivalry of Rome and Constantinople, led to a schism. The final breach between these two branches of Christianity is acknowledged as the sacking of Constantinople by the Catholic crusaders in 1204 AD. Moreover, after the fall of Constantinople to the Ottomans in 1453, the Eastern Church became even more isolated from Rome under the relatively tolerant rule of the Turks. The Orthodox Church, which saw itself as the true heir to the teachings and practices of the early Christians, flourished under the Russian Tsars. Although somewhat diminished by Communist rule and Balkan secularism, Eastern Orthodoxy has experienced a resurgence in recent decades and remains a vital aspect of Christianity.'
WHERE Tag = 'TXT_KEY_RELIGION_ORTHODOXY_PEDIA';

UPDATE Language_en_US
SET Text = 'Kyivan'
WHERE Tag = 'TXT_KEY_CIV5_RUSSIA_HEADING_4';

UPDATE Language_en_US
SET Text = 'The eighth century saw the first written record of "Kyivan Rus." The Rus are believed to have been Scandinavian Vikings who migrated south from the Baltic coast (although this is disputed by some Russian scholars, who believe that the original founders of Kyivan Rus were Slavs). By 860 the Rus were sending raiding parties as far south as Constantinople, and by 1000 AD Kyivan Rus controlled a trade route from the Baltic to the Black Sea; this would form the economic backbone of the growing regional power.[NEWLINE][NEWLINE]By the 12th century, the Kyiv Empire covered much of what would become eastern Russia, extending from Poland in the west to the Volga in the east, and from Finland in the north to the Ukraine in the south. It was a vast territory to manage from one centralized location, especially as component parts of the Empire began developing individual identities and national aspirations. Economically, the Empire also became divided, with northern provinces aligning themselves with the Baltic powers while the western areas were drawn to Poland and Hungary, and the southern regions to Asia Minor and the Mediterranean. By the closing of the 12th century Rus Kyiv was dissolved in all but name, replaced by a number of smaller quasi-feudal states.'
WHERE Tag = 'TXT_KEY_CIV5_RUSSIA_TEXT_4';

UPDATE Language_en_US
SET Text = 'The first Mongol incursion into Kyivan territory occurred in 1223, when a Mongol reconnaissance unit met the combined warriors of several Rus states under the command of the wonderfully-named "Mstislav the Bold" and "Mstislav Romanovich the Old" at the Battle of the Kalka River. The Rus forces enjoyed early success, but they became disorganized in the pursuit of the retreating foe. The Mongol horsemen rallied and defeated the pursuers in detail before they could reorganize. A large portion of the Rus forces surrendered to the Mongols on the condition that they would be spared; the Mongols accepted the conditions then slaughtered them anyway. The Mongols then left Rus for several years before returning in much greater force.[NEWLINE][NEWLINE]In 1237 a vast Mongol army of some 30,000 or more horse archers once again crossed the Volga River. In a few short years the Mongols captured, looted and destroyed dozens of Russian cities and towns, including Ryazan, Kolomna, Moscow, Rostov, Kashin, Dmitrov, Kozelsk, Halych and Kyiv. They soundly thrashed every force raised to oppose them. By 1240 most of Rus was a smoking ruin, firmly under the control of the Mongols, who then turned their sight further west, towards Hungary and Poland.[NEWLINE]'
WHERE Tag = 'TXT_KEY_CIV5_RUSSIA_TEXT_5';

-- singular/plural forms

UPDATE Language_en_US
SET Text = 'Next Policy: {1_Num: number #} {1_Num: plural 1?Turn; other?Turns;}'
WHERE Tag = 'TXT_KEY_NEXT_POLICY_TURN_LABEL';

UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]The empire is in Anarchy due to a change in Ideology for {1_Turns} more {1_Num: plural 1?turn; other?turns;}. During Anarchy, the empire will not gain any [ICON_RESEARCH] Science, [ICON_GOLD] Gold, [ICON_CULTURE] Culture, or [ICON_PEACE] Faith, and cities have no [ICON_PRODUCTION] Production to build anything[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TP_ANARCHY';

UPDATE Language_en_US
SET Text = 'Your empire has entered a state of anarchy! This will last for {1_num} {1_Num: plural 1?turn; other?turns;}.'
WHERE Tag = 'TXT_KEY_ANARCHY_BEGINS';

UPDATE Language_en_US
SET Text = 'Your Unit may move {1_Num} more {1_Num: plural 1?tile; other?tiles;} this turn.'
WHERE Tag = 'TXT_KEY_UPANEL_UNIT_MAY_MOVE';

UPDATE Language_en_US
SET Text = 'Your Unit may strike within {1_Num} {1_Num: plural 1?tile; other?tiles;} or rebase within {2_Num} tiles.'
WHERE Tag = 'TXT_KEY_UPANEL_UNIT_MAY_STRIKE_REBASE';

UPDATE Language_en_US
SET Text = '{1_Num} {1_Num: plural 1?Turn; other?Turns;}'
WHERE Tag = 'TXT_KEY_BUILD_NUM_TURNS';

UPDATE Language_en_US
SET Text = 'Influential in {1_Num} {1_Num: plural 1?Turn; other?Turns;} (assuming Tourism output remains unchanged)'
WHERE Tag = 'TXT_KEY_CO_INFLUENTIAL_TURNS_TT';

UPDATE Language_en_US
SET Text = 'You have {1_TradeRoutesUsedNum} {1_TradeRoutesUsedNum: plural 1?Trade Unit; other?Trade Units;}.[NEWLINE]You have {2_TradeRoutesAvailableNum} {2_TradeRoutesAvailableNum: plural 1?Trade Route; other?Trade Routes;} available.'
WHERE Tag = 'TXT_KEY_TOP_PANEL_INTERNATIONAL_TRADE_ROUTES_TT';

UPDATE Language_en_US
SET Text = 'You have {1_Num} unassigned {2_UnitName}{1_Num: plural 1?; other?s;}.[NEWLINE]'
WHERE Tag = 'TXT_KEY_TOP_PANEL_INTERNATIONAL_TRADE_ROUTES_TT_UNASSIGNED';

UPDATE Language_en_US
SET Text = '{1_CivName:textkey} {1_CivName: plural 1?is; other?are;} their current Ally.  You need another {2_NumInfluence} [ICON_INFLUENCE] Influence to surpass {1CivName:textkey} and become their Ally.[NEWLINE][NEWLINE]If a player becomes the Ally of a City-State, they receive extra bonuses relating to that City-State''s trait, and they will also receive any luxury and strategic Resources the City-State has connected.'
WHERE Tag = 'TXT_KEY_CITY_STATE_ALLY_TT';

UPDATE Language_en_US
SET Text = 'You cannot negotiate peace with this player for another {1_Num} {1_Num: plural 1?turn; other?turns;} because of a deal you made with another player.'
WHERE Tag = 'TXT_KEY_DIPLO_NEGOTIATE_PEACE_BLOCKED_TT';

UPDATE Language_en_US
SET Text = '{1_Progress}/{2_Threshold}, {3_Turns} {3_Turns: plural 1?Turn; other?Turns;} Remaining'
WHERE Tag = 'TXT_KEY_GPLIST_PROGRESS';

UPDATE Language_en_US
SET Text = '{1_ReligionIcon} {2_NumFollowers} {2_NumFollowers: plural 1?Follower; other?Followers;} {3_PressureString}'
WHERE Tag = 'TXT_KEY_RELIGION_TOOLTIP_LINE';

UPDATE Language_en_US
SET Text = '{1_ReligionIcon} {2_NumFollowers} {2_NumFollowers: plural 1?Follower; other?Followers;} {3_PressureString} ({4_Num} trade routes)'
WHERE Tag = 'TXT_KEY_RELIGION_TOOLTIP_LINE_WITH_TRADE';

UPDATE Language_en_US
SET Text = 'After this action is performed [COLOR_POSITIVE_TEXT]{2_NumFollowers}[ENDCOLOR] {2_NumFollowers: plural 1?Citizens; other?Citizens;} will be following [COLOR_POSITIVE_TEXT]{1_ReligionName}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_MISSION_SPREAD_RELIGION_RESULT';

UPDATE Language_en_US
SET Text = 'All city-states around the world hold elections simultaneously every {1_Num} Turns. The next election will be in {2_Num} {2_Num: plural 1?Turn; other?Turns;}.'
WHERE Tag = 'TXT_KEY_EO_CITY_STATE_ELECTION';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CITY_STATE_QUEST_CONTEST_TECHS_FORMAL} So far, you have the lead with [COLOR_POSITIVE_TEXT]{1_PlayerScore}[ENDCOLOR] {1_PlayerScore: plural 1?Technology; other?Technologies;}.'
WHERE Tag = 'TXT_KEY_CITY_STATE_QUEST_CONTEST_TECHS_WINNING_FORMAL';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CITY_STATE_QUEST_CONTEST_TECHS_FORMAL} So far, the leader has {1_LeaderScore}  {1_LeaderScore: plural 1?Technology; other?Technologies;} and you have [COLOR_POSITIVE_TEXT]{2_PlayerScore}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_CITY_STATE_QUEST_CONTEST_TECHS_LOSING_FORMAL';

UPDATE Language_en_US
SET Text = '{1_Num} [ICON_TOURISM] Tourism from {2_Num} [ICON_GREAT_WORK] {2_Num: plural 1?Great Work; other?Great Works;}'
WHERE Tag = 'TXT_KEY_CO_CITY_TOURISM_GREAT_WORKS';

UPDATE Language_en_US
SET Text = '{1_Num} [ICON_TOURISM] Tourism from buildings purchased with [ICON_PEACE] Faith'
WHERE Tag = 'TXT_KEY_CO_CITY_TOURISM_FAITH_BUILDINGS';

UPDATE Language_en_US
SET Text = '+{1_Num}% Bonus from Trade Route between Empires[NEWLINE]'
WHERE Tag = 'TXT_KEY_CO_PLAYER_TOURISM_TRADE_ROUTE';


