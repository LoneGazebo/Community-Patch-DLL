INSERT INTO Language_en_US
			(Tag,											Text)
VALUES		('TXT_KEY_GAME_OPTION_BARB_GG_GA_POINTS',		'Barbarian GG/GA Points'),
			('TXT_KEY_GAME_OPTION_BARB_GG_GA_POINTS_HELP',	'Allows all players to accumulate Great General and Great Admiral points from fighting Barbarians.');

-- Siege Unit
UPDATE Language_en_US
SET Text = 'Moves at half-speed in enemy territory'
WHERE Tag = 'TXT_KEY_PROMOTION_MUST_SET_UP';

UPDATE Language_en_US
SET Text = 'Moves at half-speed in enemy territory'
WHERE Tag = 'TXT_KEY_PEDIA_PROMOTION_MUST_SET_UP';

UPDATE Language_en_US
SET Text = 'Artillery is a deadly siege weapon, more powerful than a cannon and with a longer range. Like the cannon it has limited visibility and moves at half speed in enemy territory, but its Ranged Combat strength is tremendous. Artillery also has the "indirect fire" ability, allowing it to shoot over obstructions at targets it cannot see (as long as other friendly units can see them). Like other siege weapons, Artillery is vulnerable to melee attack.'
WHERE Tag = 'TXT_KEY_UNIT_ARTILLERY_STRATEGY';

UPDATE Language_en_US
SET Text = 'The Cannon is an important mid-game siege unit, devastating against civilizations which have not yet acquired them. Like other siege units, the Cannon moves at half speed in enemy territory. Cannon remain potent into the Industrial era. Cannon cannot see as far as other units, and should be protected with melee units.'
WHERE Tag = 'TXT_KEY_UNIT_CANNON_STRATEGY';

UPDATE Language_en_US
SET Text = 'The Catapult is a siege weapon, extremely useful when assaulting early enemy cities. It is slow and extremely vulnerable to enemy melee attack; it always should be supported by other units when it''s in the field. It moves at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_CATAPULT_STRATEGY';

UPDATE Language_en_US
SET Text = 'Roman unique unit, more powerful than the Catapult it replaces. The Ballista is an excellent siege weapon. It is extremely useful against cities, but it is quite vulnerable to attack. Be sure to protect the Ballista with other military units. Moves at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_ROMAN_BALLISTA_STRATEGY';

UPDATE Language_en_US
SET Text = 'A Trebuchet is a medieval siege weapon, significantly more powerful than the earlier Catapult units. The Trebuchet moves at half-speed in enemy territory, but once it is in position it can do serious damage to a fortified enemy city. Defend your trebuchets with melee and ranged units to keep enemy units from getting too close.'
WHERE Tag = 'TXT_KEY_UNIT_TREBUCHET_STRATEGY';

UPDATE Language_en_US
SET Text = 'Rocket Artillery is an extremely powerful siege weapon, far superior to the standard artillery. Rocket Artillery does not lose speed in enemy territory, making it much quicker than earlier guns. Although the Rocket Artillery''s sight range is limited, it can use indirect fire to attack targets it cannot see (as long as other friendly units can see them). Like all artillery it is vulnerable to infantry or armor attack, and should be defended by appropriate units.'
WHERE Tag = 'TXT_KEY_UNIT_ROCKET_ARTILLERY_STRATEGY';

UPDATE Language_en_US
SET Text = 'Fast Siege Unit of the late-game. Unlike other Siege Units, it does not move at half speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ROCKET_ARTILLERY';

UPDATE Language_en_US
SET Text = 'The first Siege Unit in the game capable of firing 3 tiles away. Moves at half speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ARTILLERY';

UPDATE Language_en_US
SET Text = 'Powerful Renaissance Era Siege Unit. Moves at half speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_CANNON';

UPDATE Language_en_US
SET Text = 'Powerful Medieval Siege Unit, good at reducing the defenses of enemy Cities. Moves at half speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_TREBUCHET';

UPDATE Language_en_US
SET Text = 'First siege Unit of the game. Does major damage to Units and Cities from a distance. Moves at half speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_CATAPULT';

UPDATE Language_en_US
SET Text = 'Siege Unit that does major damage from a distance. Moves at half speed in enemy territory. Only the Romans may build it. This Unit has a higher [ICON_RANGE_STRENGTH] Ranged Combat Strength than the Catapult, which it replaces.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BALLISTA';

UPDATE Language_en_US
SET Text = 'The Hwach''a is extremely effective against enemy land units. It is slow and vulnerable to enemy melee attack; it always should be supported by other units when it''s in the field. The Hwach''a moves at half speed in enemy territory.'
WHERE Tag = 'TXT_KEY_CIV5_KOREA_HWACHA_STRATEGY';

UPDATE Language_en_US
SET Text = 'Anti-personnel rocket unit of the Medieval Era. Strong against land units, but weak against cities. Only Korea may build it. Replaces the Trebuchet. Moves at half speed in enemy territory.'
WHERE Tag = 'TXT_KEY_CIV5_KOREA_HWACHA_HELP';

-- CS Stuff
UPDATE Language_en_US
SET Text = 'Enslave {@1_Unit} - lose {2_NumInfluence} [ICON_INFLUENCE] Influence'
WHERE Tag = 'TXT_KEY_POPUP_MINOR_BULLY_UNIT_AMOUNT';

UPDATE Language_en_US
SET Text = 'If this City-State is more [COLOR_POSITIVE_TEXT]afraid[ENDCOLOR] of you than they are [COLOR_WARNING_TEXT]resilient[ENDCOLOR], you can demand one {@3_Unit} as tribute of at the cost of [ICON_INFLUENCE] Influence.  {1_FearLevel}{2_FactorDetails}'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_UNIT_TT';

-- Fixed quest text - transitioned to new 'quest rewards' panel
UPDATE Language_en_US
SET Text = 'You have successfully destroyed the Barbarian Encampment as requested by {1_MinorCivName:textkey}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_KILL_CAMP';

UPDATE Language_en_US
SET Text = '{3_TargetName:textkey} has declared war on {1_CivName:textkey}, citing diplomatic concerns. Receive the [COLOR_POSITIVE_TEXT]full[ENDCOLOR] Quest Reward from {1_TargetName:textkey} by [COLOR_NEGATIVE_TEXT]destroying[ENDCOLOR] {3_CivName:textkey}, or a [COLOR_POSITIVE_TEXT]partial[ENDCOLOR] Quest Reward through [COLOR_POSITIVE_TEXT]allying[ENDCOLOR] both City-States. International pressure will force peace in [COLOR_POSITIVE_TEXT]{2_Num}[ENDCOLOR] turns, so act fast if you intend to intervene!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_KILL_CITY_STATE';

UPDATE Language_en_US
SET Text = 'War breaks out between {2_CivName:textkey} and {1_TargetName:textkey}'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SUMMARY_QUEST_KILL_CITY_STATE';

UPDATE Language_en_US
SET Text = 'The war between {2_CivName:textkey} and {1_TargetName:textkey} has ended, with {2_CivName:textkey} emerging as the clear victor. Your advisors worry that this outcome may lead to future conflict.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_KILL_CITY_STATE';

UPDATE Language_en_US
SET Text = '{1_TargetName:textkey} defeated by {2_CivName:textkey}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_KILL_CITY_STATE';

UPDATE Language_en_US
SET Text = 'Conquer {1_CityStateName:textkey} (full reward), or Ally both {1_CityStateName:textkey} and this City-State (partial reward).'
WHERE Tag = 'TXT_KEY_CITY_STATE_QUEST_KILL_CITY_STATE_FORMAL';


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
SET Text = 'Your promise to protect {2_MinorCivName:textkey} has helped them recover from the tribute paid to {1_BullyName:textkey}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_PLEDGE_TO_PROTECT';

UPDATE Language_en_US
SET Text = 'You have impressed {1_MinorCivName:textkey} with your culture!  They turn a blind eye to the culture of other civilizations. Civilizations that succeeded (ties are allowed):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_CULTURE';

UPDATE Language_en_US
SET Text = 'Another civilization has impressed {1_MinorCivName:textkey} with its culture.  Your cultural growth was not enough. Civilizations that succeeded (ties are allowed):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_CULTURE';

UPDATE Language_en_US
SET Text = 'The priesthood of {1_MinorCivName:textkey} are moved by your piety.  They shun the inferior faith of other civilizations. Civilizations that succeeded (ties are allowed):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_FAITH';

UPDATE Language_en_US
SET Text = 'Another civilization has impressed {1_MinorCivName:textkey} with its faith.  Your faith growth was not enough.  Civilizations that succeeded (ties are allowed):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_FAITH';

UPDATE Language_en_US
SET Text = '{1_MinorCivName:textkey} is amazed by your swift technological advancements!  Their scientists and intellectuals dismiss other civilizations as stagnant. Civilizations that succeeded (ties are allowed):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_TECHS';

UPDATE Language_en_US
SET Text = 'Another civilization has impressed {1_MinorCivName:textkey} with its technological advancement.  Your scientific discoveries were not enough. Civilizations that succeeded (ties are allowed):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_TECHS';

UPDATE Language_en_US
SET Text = '{2_MinorCivName:textkey} enjoyed seeing its rival {1_TargetName:textkey} squirm and give up hard earned resources.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_BULLY_CITY_STATE';

UPDATE Language_en_US
SET Text = '{2_MinorCivName:textkey} is relieved that the injustice they suffered at the hands of {1_TargetName:textkey} is now known by major world powers.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_DENOUNCE_MAJOR';

UPDATE Language_en_US
SET Text = 'Citizens of {@2_MinorCivName} are glad to see {@1_ReligionName} come to their city and become the dominant religion.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_SPREAD_RELIGION';

UPDATE Language_en_US
SET Text = 'Citizens of {@1_MinorCivName} are glad to see a connected trade route.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_TRADE_ROUTE';

-- Land Dispute Text
UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]Territorial disputes strain your relationship.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_LAND_DISPUTE';

-- Reversed Tourism for Open Borders
UPDATE Language_en_US
SET Text = '+{1_Num}% Bonus for Open Borders from:[NEWLINE]   '
WHERE Tag = 'TXT_KEY_CO_CITY_TOURISM_OPEN_BORDERS_BONUS';

-- Citadel Text
UPDATE Language_en_US
SET Text = 'A Citadel is a mighty fortification that can only be constructed by a Great General. You can construct a Citadel anywhere within your territory.[NEWLINE][NEWLINE]Upon constructing the Citadel, your Culture borders will also expand to surround the Citadel on all sides by one hex. If the new Culture border claims hexes already owned by another civilization, you will incur a diplomatic penalty as a result. Any unit stationed within a Citadel receives a 100% defensive strength bonus. Additionally, any enemy unit which ends its turn next to a Citadel takes 30 damage (damage does not stack with other Citadels).[NEWLINE][NEWLINE] The Acropolis in Athens, Greece, is an example of an early powerful Citadel. Such structures were almost impossible to take by direct attack, and they were capable of withstanding protracted sieges before they fell.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_CITADEL_TEXT';

-- CS Stuff
UPDATE Language_en_US
SET Text = 'You received tribute very recently'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_BULLIED_VERY_RECENTLY';

UPDATE Language_en_US
SET Text = 'You received tribute recently'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_BULLIED_RECENTLY';

INSERT INTO Language_en_US
			(Tag,										Text)
SELECT		'TXT_KEY_POP_CSTATE_BULLY_FACTOR_MONGOL_TERROR',	'You annexed a City-State recently';

-- Razing
UPDATE Language_en_US	
SET Text = '[COLOR_POSITIVE_TEXT]Razing[ENDCOLOR] The City will burn [ICON_RAZING] down each turn until it reaches 0 population, and is removed from the game. This produces a lot of [ICON_HAPPINESS_4] Unhappiness, but also greatly increases your [COLOR_POSITIVE_TEXT]War Score[ENDCOLOR] versus this player.'
WHERE Tag = 'TXT_KEY_POPUP_CITY_CAPTURE_INFO_RAZE';

-- LUA
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
SET Text = '{@1_ImprovementDescription}: +{3_Yield} {4_Icon} {@2_YieldDescription} {TXT_KEY_ABLTY_NO_FRESH_WATER_STRING}'
WHERE Tag = 'TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_NOFRESHWATERYIELDCHANGES';

UPDATE Language_en_US
SET Text = '(Not Fresh Water)'
WHERE Tag = 'TXT_KEY_ABLTY_NO_FRESH_WATER_STRING';

UPDATE Language_en_US
SET Text = '(Fresh Water)'
WHERE Tag = 'TXT_KEY_ABLTY_FRESH_WATER_STRING';

UPDATE Language_en_US
SET Text = '{1_ImprovementName:textkey}: +{3_Num} {2_YieldType:textkey} (Not Fresh Water).'
WHERE Tag = 'TXT_KEY_NO_FRESH_WATER';

UPDATE Language_en_US
SET Text = '{1_ImprovementName:textkey}: +{3_Num} {2_YieldType:textkey} (Fresh Water).'
WHERE Tag = 'TXT_KEY_FRESH_WATER';

UPDATE Language_en_US
SET Text = '{@1_ImprovementDescription}: +{3_Yield} {4_Icon} {@2_YieldDescription} {TXT_KEY_ABLTY_FRESH_WATER_STRING}'
WHERE Tag = 'TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_FRESHWATERYIELDCHANGES';

UPDATE Language_en_US
SET Text = '{TXT_KEY_ABLTY_FASTER_MOVEMENT_STRING} {@1_RouteDescription}'
WHERE Tag = 'TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_MOVEMENT';

UPDATE Language_en_US
SET Text = 'Allowed units receive [COLOR_POSITIVE_TEXT]{@1_PromotionName}[ENDCOLOR] Promotion: {@2_PromotionHelp}'
WHERE Tag = 'TXT_KEY_FREE_PROMOTION_FROM_TECH';

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

UPDATE Language_en_US
SET Text = 'Provides additional spies based on a fraction of the number of City-States, and levels up all your existing spies. Also provides a 15% reduction in enemy spy effectiveness. Must have a Police Station in all cities.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_HELP';

UPDATE Language_en_US
SET Text = 'An important defensive National Wonder for a technology-driven civilization. The National Intelligence Agency provides additional spies, levels up all your existing spies, and provides a 15% reduction in enemy spy effectiveness. A civilization must have a Police Station in all cities before it can construct the National Intelligence Agency.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_STRATEGY';

UPDATE Language_en_US
SET Text = 'Conducting Espionage'
WHERE Tag = 'TXT_KEY_SPY_STATE_ESTABLISHED_SURVEILLANCE';

UPDATE Language_en_US
SET Text = 'Disrupting Plans'
WHERE Tag = 'TXT_KEY_SPY_STATE_ESTABLISHED_SURVEILLANCE_PROGRESS_BAR';

UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]You were caught stealing from them.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_CAUGHT_STEALING';

-- Changed Diplo Text for Trade

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]We are trade partners.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_TRADE_PARTNER';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]You have shared intrigue with them.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_SHARED_INTRIGUE';

-- Changed Diplo Text for Assistance

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Your recent diplomatic actions please them.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_ASSISTANCE_TO_THEM';

UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]Your recent diplomatic actions disappoint them.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_REFUSED_REQUESTS';

-- Diplo Text for DoF changed
UPDATE Language_en_US
SET Text = 'Our Declaration of Friendship must end.'
WHERE Tag = 'TXT_KEY_DIPLO_DISCUSS_MESSAGE_END_WORK_WITH_US';

UPDATE Language_en_US
SET Text = 'I did not want things to end up like this, but I cannot overlook your warmongering. Consider our Declaration of Friendship ended.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_WARMONGER_1';

UPDATE Language_en_US
SET Text = 'Your dealings with city-states in my sphere of influence is unacceptable. Consider our Declaration of Friendship ended.'
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

UPDATE Language_en_US
SET Text = 'We cannot offer you enough to make this a fair trade. We can, however, offer you this deal.'
WHERE Tag = 'TXT_KEY_GENERIC_TRADE_CANT_MATCH_OFFER_1';

UPDATE Language_en_US
SET Text = 'The deal we suggest may be an unfair exchange for you. Please inspect it closely.'
WHERE Tag = 'TXT_KEY_GENERIC_TRADE_CANT_MATCH_OFFER_2';

UPDATE Language_en_US
SET Text = 'Impossible. You go too far.'
WHERE Tag = 'TXT_KEY_DIPLO_DISCUSS_HOW_DARE_YOU';

UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]You are competing for World Wonders.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_WONDER_DISPUTE';

UPDATE Language_en_US
SET Text = 'Deployable Troops.'
WHERE Tag = 'TXT_KEY_DEMOGRAPHICS_ARMY_MEASURE';

UPDATE Language_en_US
SET Text = 'Germany always looks forward to favorable relations with mighty empires such as yours.'
WHERE Tag = 'TXT_KEY_LEADER_BISMARCK_GREETING_NEUTRAL_HELLO_1';

UPDATE Language_en_US
SET Text = '{1_Num} [ICON_CULTURE] from Great Works and Themes'
WHERE Tag = 'TXT_KEY_CULTURE_FROM_GREAT_WORKS';

UPDATE Language_en_US
SET Text = 'You have discovered {1_Num} technologies that {2_CivName} does not know.[NEWLINE]They are receiving +{3_Num} [ICON_RESEARCH] Science on this route due to their Cultural Influence over you.'
WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_SCIENCE_EXPLAINED';

UPDATE Language_en_US
SET Text = '{1_CivName} has discovered {2_Num} technologies that you do not know.[NEWLINE]You are receiving +{3_Num} [ICON_RESEARCH] Science on this route due to your Cultural Influence over them.'
WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_SCIENCE_EXPLAINED';

-- Trade Things
UPDATE Language_en_US
SET Text = 'The other leader has not met any third party players, or you do not have an Embassy with this player.'
WHERE Tag = 'TXT_KEY_DIPLO_OTHER_PLAYERS_NO_PLAYERS_THEM';

UPDATE Language_en_US
SET Text = 'Our economy is being hamstrung by the number of units that we have. We should disband any unneeded units so that our civilization can operate at full capacity.'
WHERE Tag = 'TXT_KEY_ECONOMICAISTRATEGY_TOO_MANY_UNITS';

-- Civilopedia Refresh
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

-- Barbarians

UPDATE Language_en_US
SET Text = 'Crom'
WHERE Tag = 'TXT_KEY_LEADER_BARBARIAN';

-- War

UPDATE Language_en_US
SET Text = '[COLOR_CYAN]Capture chance if defeated: {1_Num}%[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_EUPANEL_CAPTURE_CHANCE';

-- Religion

UPDATE Language_en_US
SET Text = 'Head of {1_ReligionName}'
WHERE Tag = 'TXT_KEY_RO_STATUS_FOUNDER';

-- Spies

UPDATE Language_en_US
SET Text = 'Potential reflects the vulnerability of a city to Espionage. The higher the value, the more vulnerable the city. The base value [COLOR_POSITIVE_TEXT](a scale, from 1 to 10)[ENDCOLOR] is based on the overall prosperity and happiness of the city, specifically its science and gold output. Potential may be decreased by defensive buildings in the city, such as the Constabulary and the Police Station. Potential may increase when using a higher ranking spy to steal technologies from the city.[NEWLINE][NEWLINE]Click to sort cities by their potential.'
WHERE Tag = 'TXT_KEY_EO_POTENTIAL_SORT_TT';

UPDATE Language_en_US
SET Text = 'If your cities have high Potential, you should consider protecting them. There are two ways to do this. You may move your own spies to your cities to act as counterspies that have a chance to catch and kill enemy spies before they steal a technology. You may also slow down how quickly enemy spies can steal technologies by constructing buildings like the Constabulary, Police Station, and the Great Firewall.'
WHERE Tag = 'TXT_KEY_EO_OWN_CITY_POTENTIAL_TT';

UPDATE Language_en_US
SET Text = '{1_SpyRank} {2_SpyName} is stealing from {3_CityName}.[NEWLINE]The current Potential of {3_CityName} is {4_Num}.[NEWLINE][NEWLINE]Potential reflects the vulnerability of a city to Espionage. The higher the value, the more vulnerable the city. The base value [COLOR_POSITIVE_TEXT](a scale, from 1 to 10)[ENDCOLOR] is based on the overall prosperity and happiness of the city, specifically its science and gold output. Potential may be decreased by defensive buildings in the city, such as the Constabulary and the Police Station.'
WHERE Tag = 'TXT_KEY_EO_CITY_POTENTIAL_TT';

UPDATE Language_en_US
SET Text = '{1_SpyRank} {2_SpyName} cannot steal technologies from {3_CityName}.[NEWLINE][NEWLINE]The base Potential of {4_CityName} is {5_Num}.[NEWLINE][NEWLINE]Potential reflects the vulnerability of a city to Espionage. The higher the value, the more vulnerable the city. The base value [COLOR_POSITIVE_TEXT](a scale, from 1 to 10)[ENDCOLOR] is based on the overall prosperity and happiness of the city, specifically its science and gold output. Potential may be decreased by defensive buildings in the city, such as the Constabulary and the Police Station.'
WHERE Tag = 'TXT_KEY_EO_CITY_POTENTIAL_CANNOT_STEAL_TT';

UPDATE Language_en_US
SET Text = 'The Potential of {1_CityName} is believed to be {2_Num}. Send a [ICON_SPY] Spy to this City to learn more about it.[NEWLINE][NEWLINE]Potential reflects the vulnerability of a city to Espionage. The higher the value, the more vulnerable the city. The base value [COLOR_POSITIVE_TEXT](a scale, from 1 to 10)[ENDCOLOR] is based on the overall prosperity of the city, specifically its science and gold output.'
WHERE Tag = 'TXT_KEY_EO_CITY_ONCE_KNOWN_POTENTIAL_TT';

UPDATE Language_en_US
SET Text = 'Options for {1_SpyRank} {2_SpyName}:[NEWLINE][NEWLINE][ICON_BULLET] Move to a City-State and attempt to [COLOR_POSITIVE_TEXT]Rig an Election[ENDCOLOR] or [COLOR_POSITIVE_TEXT]Stage a Coup[ENDCOLOR].[NEWLINE][ICON_BULLET] Move to a non-Capital City owned by a Major Civilization and attempt to [COLOR_POSITIVE_TEXT]Steal Technology[ENDCOLOR] and [COLOR_POSITIVE_TEXT]Uncover Intrigue[ENDCOLOR].[NEWLINE][ICON_BULLET] Move to a Capital City owned by a Major Civilization and attempt to [COLOR_POSITIVE_TEXT]Steal Technology[ENDCOLOR], [COLOR_POSITIVE_TEXT]Uncover Intrigue[ENDCOLOR], or [COLOR_POSITIVE_TEXT]Schmooze[ENDCOLOR] as a Diplomat.'
WHERE Tag = 'TXT_KEY_EO_SPY_MOVE_TT';

-- Notification

UPDATE Language_en_US
SET Text = '{1_CivName:textkey} lost {1_CivName: plural 1?its; other?their;} [ICON_CAPITAL] Capital'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SUMMARY_PLAYER_LOST_CAPITAL';

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

UPDATE Language_en_US
SET Text = 'The Trireme is the first naval unit available to civilizations in the game. It is a melee attack unit, engaging units at sea or cities (but it has no ranged attack). The Trireme is useful for clearing barbarian ships out of home waters and for limited exploration (it cannot end its turn on Deep Ocean hexes outside of the city borders unless it is from Polynesia).'
WHERE Tag = 'TXT_KEY_UNIT_TRIREME_STRATEGY';

UPDATE Language_en_US
SET Text = 'The Galleass is the first naval unit with a ranged attack generally available to the civilizations in the game. It is much stronger than earlier naval units but a bit slower. It cannot initiate melee combat. The Galleass is useful for clearing enemy ships out of shallow waters. It cannot end its turn on Deep Ocean hexes unless it is from Polynesia.'
WHERE Tag = 'TXT_KEY_UNIT_GALLEASS_STRATEGY';

-- Lua for Cities
UPDATE Language_en_US
SET Text = 'We have no tradeable cities and/or we do not have an Embassy with them.'
WHERE Tag = 'TXT_KEY_DIPLO_TO_TRADE_CITY_NO_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'They have no tradeable cities and/or they do not have an Embassy with us.'
WHERE Tag = 'TXT_KEY_DIPLO_TO_TRADE_CITY_NO_THEM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );
	

INSERT INTO Language_en_US
			(Tag,										Text)
SELECT		'TXT_KEY_SCIENCE_BOOST_CONQUEST_ASSYRIA',	'Your soldiers found [ICON_RESEARCH] Science during the conquest of {1_Name}!'
WHERE EXISTS (SELECT * FROM CustomModOptions WHERE Name='ALTERNATE_ASSYRIA_TRAIT' AND Value= 1 );

-- All Growth Modifier Tooltips with Icons
UPDATE Language_en_US
SET Text = '[NEWLINE][ICON_BULLET][ICON_CONNECTED] Empire Modifier: {1_Num}%'
WHERE Tag = 'TXT_KEY_FOODMOD_PLAYER';

UPDATE Language_en_US
SET Text = '[NEWLINE][ICON_BULLET][ICON_RELIGION_PANTHEON] Religious Beliefs Modifier: {1_Num}%'
WHERE Tag = 'TXT_KEY_FOODMOD_RELIGION';

UPDATE Language_en_US
SET Text = '[NEWLINE][ICON_BULLET][ICON_HAPPINESS_1] We Love the King Day Modifier: {1_Num}%'
WHERE Tag = 'TXT_KEY_FOODMOD_WLTKD';

-- Revolt TT fix.
UPDATE Language_en_US
SET Text = 'Because the empire has at least 20 [ICON_HAPPINESS_4] Unhappiness (or because our public opinion is not Content), a city will revolt in {1_Turns} turns. Based on current Unhappiness levels and Cultural/Ideological pressure, the city most likely to revolt is {2_CityName} and it will join {3_CivName}. Try to get your empire''s Happiness level out of this range as soon as possible!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_POSSIBLE_CITY_REVOLT';


UPDATE Language_en_US
SET Text = 'Very well. Not that it will help either of us in the long run – we will all die soon enough.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_TRADE_YES_HAPPY';
