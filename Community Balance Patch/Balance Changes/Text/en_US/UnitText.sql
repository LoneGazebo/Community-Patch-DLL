	
	-- Civilians

	-- Settler

	UPDATE Language_en_US SET Text = 'Founds new Cities to expand your Empire.[NEWLINE][NEWLINE][ICON_FOOD] Growth of the City is stopped while this Unit is being built. Reduces City''s [ICON_CITIZEN] Population by 1 when completed.[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]Cannot found Cities next to anyone''s border or if your Empire is very unhappy.[ENDCOLOR]' WHERE Tag = 'TXT_KEY_UNIT_HELP_SETTLER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	-- Great Merchant Text

	UPDATE Language_en_US SET Text = 'If the unit is inside City-State territory that you are not at war with, this order will expend the unit.[NEWLINE][NEWLINE]You will receive a large amount of [ICON_GOLD] Gold and an instant "We Love the King Day" in all owned cities (scaling with the number of owned Towns you''ve controlled). This action will consume the unit.' WHERE Tag = 'TXT_KEY_MISSION_CONDUCT_TRADE_MISSION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US SET Text = 'The Great Merchant can construct the special Town improvement which, when worked, produces loads of [ICON_GOLD] Gold and increases the number of WLTKD turns from the Merchant''s Trade Mission ability by 1. The Great Merchant can also journey to a city-state and perform a "trade mission" which produces gobs of [ICON_GOLD] Gold, starts a "We Love the King Day" in all owned cities and, (if not using CSD) garners many Influence Points with the city-state. The Great Merchant is expended when used in any of these ways.' WHERE Tag = 'TXT_KEY_UNIT_GREAT_MERCHANT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Great Engineer Text

	UPDATE Language_en_US SET Text = 'This order will hurry production on the city''s current effort. Every owned Manufactory you''ve created increases the amount of production gained by 20%. It consumes the Great Person.' WHERE Tag = 'TXT_KEY_MISSION_HURRY_PRODUCTION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US SET Text = 'Great Engineers can construct the special Manufactory improvement. When worked, it provides a lot of [ICON_PRODUCTION] Production for a city and increase the power of the Engineer''s Hurry ability by 20% (per Manufactory). The Great Engineer can also hurry production of a unit, building or Wonder in a city. The Great Engineer is expended when used in either of these ways.' WHERE Tag = 'TXT_KEY_UNIT_GREAT_ENGINEER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Great Engineer Text

	UPDATE Language_en_US SET Text = 'This will have the Great Person help research a technology. Every owned Academy you''ve created increases the amount of science gained by 10%. It consumes the Great Person.' WHERE Tag = 'TXT_KEY_MISSION_DISCOVER_TECH_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US SET Text = 'The Great Scientist can construct the special Academy improvement which, when worked, produces lots of [ICON_RESEARCH] Science and increases the value of the Scientist''s Discover ability by 10%. Further, a Great Scientist can give you a considerable boost towards your next tech. The Great Scientist is expended when used in any of these ways.' WHERE Tag = 'TXT_KEY_UNIT_GREAT_SCIENTIST_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );


	-- Changed how Musician Great Tour Works
	UPDATE Language_en_US SET Text = 'The value of this action depends on the number of [ICON_GREAT_WORK] Great Works of Music you have created. [COLOR_POSITIVE_TEXT]The more Great Works of Music you own, the stronger your Concert Tours[ENDCOLOR]. When you completed a Concert Tour, your [ICON_TOURISM] Tourism with the target Civilization is immediately increased by an amount equal to your recent Tourism per turn output ([COLOR_POSITIVE_TEXT]12[ENDCOLOR] Turns, plus 1 additional Turn for every owned Great Work of Music) and any relevant Tourism modifiers with the target Civilization. In addition, you receive +1 [ICON_HAPPINESS_1] Happiness in your [ICON_CAPITAL] Capital. This action consumes the unit.[NEWLINE][NEWLINE]Cannot perform this action if at war with the target Civilization, or if your [ICON_TOURISM] Cultural Influence over the Civilization is [COLOR_MAGENTA]Influential[ENDCOLOR] or greater.' WHERE Tag = 'TXT_KEY_MISSION_ONE_SHOT_TOURISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US SET Text = 'A Great Musician can create a Great Work of Music (generates both [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism) that is placed in the nearest city that has an appropriate building with an empty slot (like an Opera House or Broadcast Tower).[NEWLINE][NEWLINE]A Great Musician can also travel to another civilization and perform a [COLOR_POSITIVE_TEXT]Concert Tour[ENDCOLOR]. The value of this action depends on the number of [ICON_GREAT_WORK] Great Works of Music you have created, and any relevant Tourism modifiers with the target Civilization. [COLOR_POSITIVE_TEXT]The more Great Works of Music you own, the stronger your Concert Tours[ENDCOLOR]. When you completed a Concert Tour, your [ICON_TOURISM] Tourism with the target Civilization is immediately increased by an amount equal to your recent Tourism per turn output ([COLOR_POSITIVE_TEXT]12[ENDCOLOR] Turns, plus 1 additional Turn for every owned Great Work of Music). In addition, you receive +1 [ICON_HAPPINESS_1] Happiness in your [ICON_CAPITAL] Capital. This action consumes the unit.[NEWLINE][NEWLINE]Cannot perform this action if at war with the target Civilization, or if your [ICON_TOURISM] Cultural Influence over the Civilization is [COLOR_MAGENTA]Influential[ENDCOLOR] or greater.' WHERE Tag = 'TXT_KEY_UNIT_GREAT_MUSICIAN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Artist
	UPDATE Language_en_US SET Text = 'A Great Artist can create a Great Work of Art (generates both [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism) that is placed in the nearest city that has an appropriate building with an empty slot (like the Palace, a Museum, or a Cathedral). A Great Artist can also generate a large sum of Golden Age Points, scaling with [ICON_GOLDEN_AGE] Golden Age Point output, [ICON_TOURISM] Tourism output, and the number of [COLOR_POSITIVE_TEXT]themed[ENDCOLOR] sets of [ICON_GREAT_WORK] Great Works. Great Artists are expended when used either of these ways.' WHERE Tag = 'TXT_KEY_UNIT_GREAT_ARTIST_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	UPDATE Language_en_US SET Text = '+{1_Num} [ICON_GOLDEN_AGE] Golden Age Points[NEWLINE][NEWLINE]This order will consume the Great Artist and create a Golden Age Points, which may trigger a [ICON_GOLDEN_AGE] Golden Age (extra [ICON_PRODUCTION] Production, [ICON_GOLD] Gold, and [ICON_CULTURE] Culture). Scales based on the [ICON_TOURISM] Tourism and [ICON_GOLDEN_AGE] Golden Age Point output of the past 15 turns, and is increased by 20% for every owned [COLOR_POSITIVE_TEXT]themed[ENDCOLOR] Great Work set.' WHERE Tag = 'TXT_KEY_MISSION_START_GOLDENAGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Writer
	UPDATE Language_en_US SET Text = 'A Great Writer can create a Great Work of Writing (generates both [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism) that is placed in the nearest city that has an appropriate building with an empty slot (like an Amphitheater, National Epic, Heroic Epic, or Royal Library). A Great Writer can also write a Political Treatise, which grants the player an amount of Culture that scales with the number of owned [ICON_GREAT_WORK] Great Works. Great Writers are expended when used either of these ways.' WHERE Tag = 'TXT_KEY_UNIT_GREAT_WRITER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	UPDATE Language_en_US SET Text = 'This order will consume the Great Person and will grant a large amount of [ICON_CULTURE] Culture, scaling by 3% for every owned Great Work.' WHERE Tag = 'TXT_KEY_MISSION_GIVE_POLICIES_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Admiral 

	UPDATE Language_en_US SET Text = 'The Great Admiral can immediately cross oceans for free, making it a useful (if vulnerable) exploration vessel. Furthermore, while in owned territory, the Great Admiral can be sent on a [COLOR_POSITIVE_TEXT]Voyage of Discovery[ENDCOLOR] that expends the Unit and gives you two copies of a Luxury Resource not available on the current map.[NEWLINE][NEWLINE]The Great Admiral has the ability to instantly [COLOR_POSITIVE_TEXT]Repair[ENDCOLOR] every Naval and Embarked Unit in the same hex, as well as in adjacent hexes. The Great Admiral is consumed when performing this action. The Great Admiral also provides +15% [ICON_STRENGTH] Combat Strength to all player-owned Naval Units within 2 tiles. The Great Admiral is NOT consumed when he provides this bonus. When the Great Admiral is expended for a [COLOR_POSITIVE_TEXT]Voyage of Discovery[ENDCOLOR] or [COLOR_POSITIVE_TEXT]Repair[ENDCOLOR], it increases your Military Unit Supply Cap by 2.' WHERE Tag = 'TXT_KEY_UNIT_GREAT_ADMIRAL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US SET Text = 'This order will consume the Great Admiral and heal all of your Naval Units and Embarked Units in this tile and all adjacent tiles. It also increases your Military Unit Supply Cap by 2.' WHERE Tag = 'TXT_KEY_MISSION_REPAIR_FLEET_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- General
	UPDATE Language_en_US SET Text = 'The Great General can construct the special [COLOR_POSITIVE_TEXT]Citadel[ENDCOLOR] improvement which gives a big defensive bonus to any Unit occupying it, damages Enemy Units that finish their turn in tiles adjacent to it, puts all adjacent tiles into your territory, and increases your Military Unit Supply Cap by 2. The Great General is consumed when it builds the [COLOR_POSITIVE_TEXT]Citadel[ENDCOLOR]. The Great General provides +15% [ICON_STRENGTH] Combat Strength to all player-owned Land Units within 2 tiles. The Great General is NOT consumed when he provides this bonus.' WHERE Tag = 'TXT_KEY_UNIT_GREAT_GENERAL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Inquisitor
	UPDATE Language_en_US SET Text = 'Can be purchased with [ICON_PEACE] Faith in any city with a majority Religion that has been enhanced. They can remove other religions from your cities (expending the Inquisitor) or be placed inside a city to reduce Missionary and Prophet spread power in that City by 50%. Removing Heresy causes 1 turn of [ICON_RESISTANCE] Resistance in the City.' WHERE Tag = 'TXT_KEY_UNIT_INQUISITOR_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	UPDATE Language_en_US SET Text = 'Used to remove other religions from cities. Inquisitors stationed in cities reduce foriegn Missionary and Prohet spread power by 50%. Removing Heresy causes 1 turn of Resistance in the City. May only be obtained by purchasing with [ICON_PEACE] Faith.' WHERE Tag = 'TXT_KEY_UNIT_HELP_INQUISITOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US SET Text = 'This order will remove the presence of other religions from the nearby, friendly-controlled city. Causes 1 turn of Resistance in the City. An inquisitor is consumed by this action.' WHERE Tag = 'TXT_KEY_MISSION_REMOVE_HERESY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	-- Archaeologist Text

	UPDATE Language_en_US SET Text = 'Maximum [COLOR_POSITIVE_TEXT]3[ENDCOLOR] active Archaeologists per player at any one time. Archaeologists are a special subtype of Worker that are used to excavate Antiquity Sites to either create Landmark improvements or to extract [ICON_GREAT_WORK] Artifacts to fill in [ICON_GREAT_WORK] Great Work of Art slots in Museums, Palaces, Hermitages, and selected Wonders. Archaeologists may work in territory owned by any player. They are consumed once they complete an Archaeological Dig at an Antiquity Site. Archaeologists may not be purchased with [ICON_GOLD] Gold and may only be built in a City with a [COLOR_POSITIVE_TEXT]Public School[ENDCOLOR].' WHERE Tag = 'TXT_KEY_UNIT_HELP_ARCHAEOLOGIST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'A town is a human settlement larger than a village but smaller than a city. The size definition for what constitutes a "town" varies considerably in different parts of the world. The word town shares an origin with the German word Zaun, the Dutch word tuin, and the Old Norse tun. The German word Zaun comes closest to the original meaning of the word: a fence of any material.[NEWLINE][NEWLINE]Towns receive +1 [ICON_GOLD] Gold and [ICON_PRODUCTION] Production if built on a Road that connects two owned Cities, and +2 [ICON_GOLD] and [ICON_PRODUCTION] Production if a Railroad. Receive additional [ICON_GOLD] Gold and [ICON_PRODUCTION] Production (+1 for Roads, +2 for Railroads) if a Trade Route, either internal or international, passes over this Town.'
	WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_CUSTOMS_HOUSE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'You can expend a Great Merchant to create a Town. When worked, a Town generates a lot of gold and food per turn for its city, and increases the "We Love the King Day" duration from Great Merchant Trade Missions.[NEWLINE][NEWLINE]Towns gain additional [ICON_GOLD] Gold and [ICON_PRODUCTION] Production if built on a Road or Railroad that connects two owned Cities[NEWLINE][NEWLINE]Receive additional [ICON_GOLD] Gold and [ICON_PRODUCTION] Production (+1 pre-Industrial Era, +2 Industrial Era or later) if a Trade Route, either internal or international, passes over this Town.'
	WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_CUSTOMSHOUSE_HEADING4_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Construct a Town'
	WHERE Tag = 'TXT_KEY_BUILD_CUSTOMS_HOUSE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Town'
	WHERE Tag = 'TXT_KEY_IMPROVEMENT_CUSTOMS_HOUSE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Great Merchant can construct the special Town improvement which, when worked, produces [ICON_GOLD] Gold and [ICON_FOOD] Food. The Great Merchant can also journey to a city-state and perform a "trade mission" which produces [ICON_GOLD] Gold, starts a "We Love the King Day" in all owned cities (scaling with the number of owned Towns) and, (if not using CSD) garners many Influence Points with the city-state. The Great Merchant is expended when used in either of these ways.[NEWLINE][NEWLINE]Towns receive +1 [ICON_GOLD] Gold and [ICON_PRODUCTION] Production if built on a Road that connects two owned Cities, and +2 [ICON_GOLD] Gold and [ICON_PRODUCTION] Production if a Railroad. Receive additional [ICON_GOLD] Gold and [ICON_PRODUCTION] Production (+1 for Roads, +2 for Railroads) if a Trade Route, either internal or international, passes over this Town.'
	WHERE Tag = 'TXT_KEY_UNIT_GREAT_MERCHANT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'What does the town special improvement do?'
	WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_CUSTOMSHOUSE_ADV_QUEST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Special Improvement: Town'
	WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_CUSTOMSHOUSE_HEADING4_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Specialist Yields'
	WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_HEADING1_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Great People Improvement Yields'
	WHERE Tag = 'TXT_KEY_BUILDINGS_SPECIALISTS_HEADING2_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = '[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Specialists consume variable amounts of food, based on your current Era.[ENDCOLOR][NEWLINE][NEWLINE]    [COLOR_CYAN]Ancient-Medieval[ENDCOLOR]: Specialists consume [ICON_FOOD] 3 Food.[NEWLINE]    [COLOR_CYAN]Renaissance[ENDCOLOR]: Specialists consume [ICON_FOOD] 4 Food.[NEWLINE]    [COLOR_CYAN]Industrial[ENDCOLOR]: Specialists consume [ICON_FOOD] 5 Food.[NEWLINE]    [COLOR_CYAN]Modern[ENDCOLOR]: Specialists consume [ICON_FOOD] 6 Food.[NEWLINE]    [COLOR_CYAN]Atomic:[ENDCOLOR]: Specialists consume [ICON_FOOD] 7 Food.[NEWLINE]    [COLOR_CYAN]Information[ENDCOLOR]: Specialists consume [ICON_FOOD] 8 Food.[NEWLINE][NEWLINE]All Specialists begin the game with a base set of yields. These yields change as the game progresses based on Technologies, Policies, Buildings, and Beliefs.'
	WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_HEADING1_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'All Great Person Improvements begin the game with a base set of yields. These yields change as the game progresses based on Technologies, Policies, Buildings, and Beliefs.'
	WHERE Tag = 'TXT_KEY_BUILDINGS_SPECIALISTS_HEADING2_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Time and again throughout history, there have arisen men and women who have profoundly changed the world around them - artists, scientists, generals, merchants and others, whose genius sets them head and shoulders above the rest. In this game such visionaries are called "Great People."[NEWLINE][NEWLINE]There are five different kinds of great people in the game: Great Merchants, Great Artists, Great Scientists, Great Engineers, and Great Generals (a sixth, the Great Diplomat, is added in CSD). Each has a special ability.[NEWLINE][NEWLINE]Your civilization acquires Great People by constructing certain buildings and wonders, and then staffing them with "specialists", citizens from your cities who have given up working in the field or the mines. While specialists do not work city tiles, they do greatly speed the arrival of Great People. Balancing the need for food and the desire for Great People is an important challenge of city management.[NEWLINE][NEWLINE]{TXT_KEY_SPECIALISTSANDGP_HEADING1_BODY}'
	WHERE Tag = 'TXT_KEY_PEDIA_SPEC_HELP_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'During a game, you will create "workers" - non-military units who will "improve" the land around your cities, increasing the land''s productivity or providing access to a nearby "resource." Improvements include farms, trading posts, lumber mills, logging camps, quarries, mines, and more. During wartime your enemy may "pillage" (destroy) your improvements. Pillaged improvements are ineffective until a worker has "repaired" them.[NEWLINE][NEWLINE]{TXT_KEY_BUILDINGS_SPECIALISTS_HEADING2_BODY}'
	WHERE Tag = 'TXT_KEY_PEDIA_IMPROVEMENT_HELP_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Unit must be in a tile owned by another major Civilization that you are not at war with.'
	WHERE Tag = 'TXT_KEY_MISSION_ONE_SHOT_TOURISM_DISABLED_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	-- Air Units

	-- Atomic Bomb
	
	UPDATE Language_en_US
	SET Text = 'Unleash nuclear destruction upon your foes. Does great damage to Cities and damages any Unit caught in the blast radius. May be stationed on Aircraft Carriers.[NEWLINE][NEWLINE]Requires [COLOR_POSITIVE_TEXT]Manhattan Project[ENDCOLOR] and 1 [ICON_RES_URANIUM] Uranium.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_ATOMIC_BOMB' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	UPDATE Language_en_US
	SET Text = 'An Atomic Bomb is an extremely powerful unit. The Atomic Bomb can be based in a player-owned city or aboard a carrier. It can move from base to base or attack a target within its range of 6 tiles. When it detonates, an Atomic Bomb will damage or possibly destroy units, and cities will be severely damaged, within its blast radius of 2 tiles. It is automatically destroyed when it attacks. See the rules on Nuclear Weapons for more details.'
	WHERE Tag = 'TXT_KEY_UNIT_ATOMIC_BOMB_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	-- Nuclear Missile
	
	UPDATE Language_en_US
	SET Text = 'Unleash nuclear destruction upon your foes. Does great damage to Cities and damages any Unit caught in the blast radius.[NEWLINE][NEWLINE]Requires [COLOR_POSITIVE_TEXT]Manhattan Project[ENDCOLOR] and 2 [ICON_RES_URANIUM] Uranium.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_NUCLEAR_MISSILE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Bomber'
	WHERE Tag = 'TXT_KEY_UNIT_WWI_BOMBER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Early Air Unit that can bombard Enemy Units and Cities from the skies. [COLOR_POSITIVE_TEXT]Does not use Military Supply.[ENDCOLOR]'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_WWI_BOMBER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Bomber is an early air unit. It is effective against ground targets, less so against naval targets, and it is quite vulnerable to enemy aircraft. The Bomber can be based on a player-owned city or aboard a carrier. It can move from base to base and perform missions within its range. Use it to attack enemy units and cities. When possible, send in triplanes or fighters first to "use up" enemy anti-aircraft defenses for that turn. See the rules on Aircraft for more information.'
	WHERE Tag = 'TXT_KEY_UNIT_WWI_BOMBER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Fighter is a moderately-powerful air unit. It can be based in any city you own or aboard an aircraft carrier. It can move from city to city (or carrier) and can perform "missions" within its range. Use fighters to attack enemy aircraft and ground units, to scout enemy positions, and to defend against enemy air attacks. Fighters are especially effective against enemy helicopters. See the rules on Aircraft for more information.'
	WHERE Tag = 'TXT_KEY_UNIT_FIGHTER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Air Unit designed to wrest control of the skies and intercept incoming Enemy Aircraft. [COLOR_POSITIVE_TEXT]Does not use Military Supply.[ENDCOLOR]'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_FIGHTER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Jet Fighter is a powerful air unit. It can be based in any city you own or aboard an aircraft carrier. It can move from base to base and can perform "missions" within its range. Use Jet Fighters to attack enemy aircraft and ground units, to scout enemy positions, and to defend against enemy air attacks. Jet Fighters are especially effective against enemy helicopters. The Jet Fighter has the "air recon" ability, which means that everything within 6 tiles of its starting location is visible at the beginning of the turn. See the rules on Aircraft for more information.'
	WHERE Tag = 'TXT_KEY_UNIT_JET_FIGHTER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Air Unit designed to wrest control of the skies and intercept incoming Enemy Aircraft. [COLOR_POSITIVE_TEXT]Does not use Military Supply.[ENDCOLOR]'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_JET_FIGHTER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Triplane is an early air unit. It can be based in any city you own or aboard an aircraft carrier. It can move from city to city (or carrier) and can perform "missions" within its range. Use triplanes to attack enemy aircraft and ground units, to scout enemy positions, and to defend against enemy air attacks. See the rules on Aircraft for more information.'
	WHERE Tag = 'TXT_KEY_UNIT_TRIPLANE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	UPDATE Language_en_US
	SET Text = 'Early Air Unit designed to intercept incoming Enemy Aircraft. [COLOR_POSITIVE_TEXT]Does not use Military Supply.[ENDCOLOR]'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_TRIPLANE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	UPDATE Language_en_US
	SET Text = 'A long-range Air Unit that rains death from above onto Enemy Units and Cities without detection. [COLOR_POSITIVE_TEXT]Does not use Military Supply.[ENDCOLOR]'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_STEALTH_BOMBER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );


	-- Borrowed for WWI Bomber
	UPDATE Language_en_US
	SET Text = 'Heavy Bomber'
	WHERE Tag = 'TXT_KEY_UNIT_BOMBER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Air Unit that rains death from above onto Enemy Units and Cities. [COLOR_POSITIVE_TEXT]Does not use Military Supply.[ENDCOLOR]'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_BOMBER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );


	UPDATE Language_en_US
	SET Text = 'The Heavy Bomber is an air unit. It is effective against ground targets, less so against naval targets, and it is quite vulnerable to enemy aircraft. The Heavy Bomber can be based on a player-owned city or aboard a carrier. It can move from base to base and perform missions within its range. Use Bombers to attack enemy units and cities. When possible, send in fighters first to "use up" enemy anti-aircraft defenses for that turn. See the rules on Aircraft for more information.'
	WHERE Tag = 'TXT_KEY_UNIT_BOMBER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	UPDATE Language_en_US
	SET Text = ' The Stealth Bomber is an upgraded bomber, possessing increased range, a greater Ranged Combat Strength, and an increased ability to avoid enemy anti-aircraft and fighters. The Stealth Bomber has the "air recon" ability, which means that everything within 6 tiles of its starting location is visible at the beginning of the turn. See the rules on Aircraft for more information.'
	WHERE Tag = 'TXT_KEY_UNIT_STEALTH_BOMBER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Guided Missile is a one-shot unit which is destroyed when it attacks an enemy target. The Guided Missile may be based in a player-owned friendly city or aboard a nuclear submarine or missile cruiser. They may move from base to base or attack an enemy unit or city with their range of "6". See the rules on Missiles for more information.'
	WHERE Tag = 'TXT_KEY_UNIT_GUIDED_MISSILE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'A cheap Unit that may be used once to damage Enemy Units or Garrisoned Units in Cities. [COLOR_POSITIVE_TEXT]Does not use Military Supply.[ENDCOLOR][NEWLINE][NEWLINE]Requires 1 [ICON_RES_OIL] Oil.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_GUIDED_MISSILE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Archer

	UPDATE Language_en_US
	SET Text = 'This is the earliest ranged unit. Attacks without retaliation. It is critically important when attacking enemy cities early in the game. However, it is fairly weak when attacked by other units. Assign it to garrison cities or keep it behind your lines, protected by melee units.'
	WHERE Tag = 'TXT_KEY_UNIT_ARCHER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Ancient Era ranged Unit that can strike foes from two tiles away without retaliation.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_ARCHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );


	-- Melee

	UPDATE Language_en_US
	SET Text = 'The Spearman is the first melee unit available after the Warrior. It is more powerful than the Warrior, and gets a significant combat bonus against mounted units (Chariot Archer, Horsemen and so forth).'
	WHERE Tag = 'TXT_KEY_UNIT_SPEARMAN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Change Name of Great War Infantry/Bomber to be more 'generic'
	UPDATE Language_en_US
	SET Text = 'Rifleman'
	WHERE Tag = 'TXT_KEY_UNIT_GREAT_WAR_INFANTRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Rifleman is the basic Modern era combat unit. It is significantly stronger than its predecessor, the Fusilier.'
	WHERE Tag = 'TXT_KEY_UNIT_GREAT_WAR_INFANTRY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Musketman

	UPDATE Language_en_US
	SET Text = 'First ranged gunpowder Unit of the game. Fairly cheap and powerful.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_MUSKETMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'First ranged gunpowder Unit of the game. Fairly cheap and powerful.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_MUSKETMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Musketman is the first ranged gunpowder unit in the game, and it replaces all of the older ranged foot-soldier types - Crossbowmen, Archers, and the like. Because it is a ranged unit, it can attack an enemy that is up to two hexes away.'
	WHERE Tag = 'TXT_KEY_UNIT_MUSKETMAN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Borrowed for WWI Infantry - Fusiliers more fiting.
	
	UPDATE Language_en_US
	SET Text = 'Fusilier'
	WHERE Tag = 'TXT_KEY_UNIT_RIFLEMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Fusilier is the gunpowder unit following the Tercio. It is significantly more powerful than the Tercio, giving the army with the advanced units a big advantage over civs which have not yet upgraded to the new unit. Furthermore, as the first gunpowder melee unit, it comes equipped with promotions designed to help it hold and push the front lines of a fight.'
	WHERE Tag = 'TXT_KEY_UNIT_RIFLEMAN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Infantry is the basic Industrial era combat unit. It is significantly stronger than its predecessor, the Rifleman. Modern combat is increasingly complex, and on its own an Infantry unit is vulnerable to air, artillery and tank attack. When possible Infantry should be supported by artillery, tanks, and air (or anti-air) units.'
	WHERE Tag = 'TXT_KEY_UNIT_INFANTRY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Strong infantry Unit capable of paradropping up to 9 tiles from friendly territory. Paratrooper can also move and pillage after paradropping, but cannot enter combat until the following turn. Has a combat bonus versus Siege units.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_PARATROOPER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Paratrooper is a late-game infantryman. It can parachute up to 9 tiles away (when starting in friendly territory). This allows the paratrooper to literally jump over enemy positions and destroy road networks, pillage vital resources and so forth, wrecking havoc behind his lines. The Paratrooper is at great risk when on such missions, so make sure the target is worth it!'
	WHERE Tag = 'TXT_KEY_UNIT_PARATROOPER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Special Forces'
	WHERE Tag = 'TXT_KEY_UNIT_MARINE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Information Era Unit especially useful for embarking and invading across the sea as well as taking out Gun (Melee) units. Can also paradrop behind enemy lines.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_MARINE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Special Forces unit possesses promotions that enhance its Sight and attack strength when embarked at sea. It is also stronger versus Gun (Melee) units, and can paradrop up to 9 tiles away from friendly territory.'
	WHERE Tag = 'TXT_KEY_UNIT_MARINE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Special forces and special operations forces are military units trained to perform unconventional missions. Special forces, as they would now be recognised, emerged in the early 20th century, with a significant growth in the field during the Second World War. Depending on the country, special forces may perform some of the following functions: airborne operations, counter-insurgency, "counter-terrorism", covert ops, direct action, hostage rescue, high-value targets/manhunting, intelligence operations, mobility operations, and unconventional warfare.'
	WHERE Tag = 'TXT_KEY_CIV5_MARINE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	UPDATE Language_en_US
	SET Text = 'These specialized artillery units will automatically attack any air unit bombing a target within 3 tiles. (They can only intercept one unit per turn.) They are quite weak in combat against other ground units and should be defended by stronger units when under threat of ground attack.'
	WHERE Tag = 'TXT_KEY_UNIT_ANTI_AIRCRAFT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Mobile SAM (surface-to-air) units provide an advancing army with anti-air defense. Mobile SAM units can intercept and shoot at enemy aircraft bombing targets within 4 hexes (but only one unit per turn). These units are fairly vulnerable to non-air attack and should be accompanied by infantry or armor.'
	WHERE Tag = 'TXT_KEY_UNIT_MOBILE_SAM_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	UPDATE Language_en_US SET Text = 'Light Tank' WHERE Tag = 'TXT_KEY_UNIT_AT_GUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	UPDATE Language_en_US SET Text = 'A light tank is a tank variant initially designed for rapid movement, and now primarily employed in the reconnaissance role, or in support of expeditionary forces where main battle tanks cannot be made available. Early light tanks were generally armed and armored similar to an armored car, but used tracks in order to provide better cross-country mobility. The fast light tank was a major feature of the pre-WWII buildup, where it was expected they would be used to exploit breakthroughs in enemy lines created by slower, heavier tanks. Numerous small tank designs and "tankettes" were developed during this period and known under a variety of names, including the ''combat car''.' WHERE Tag = 'TXT_KEY_CIV5_INDUSTRIAL_ANTITANKGUN_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	UPDATE Language_en_US SET Text = 'The Light Tank is a specialized combat unit designed for hit-and-run tactics. Back them up with Riflemen, Tanks, and Artillery for a potent Modern Era fighting force.' WHERE Tag = 'TXT_KEY_UNIT_AT_GUN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	UPDATE Language_en_US SET Text = 'Highly-mobile ranged unit specialized in hit-and-run tactics and skirmishing.' WHERE Tag = 'TXT_KEY_UNIT_HELP_ANTI_TANK_GUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	-- Naval Units

	INSERT INTO Language_en_US (Tag, Text)
		SELECT 'TXT_KEY_DESC_CRUISER', 'Cruiser'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_en_US (Tag, Text)
		SELECT 'TXT_KEY_CIV5_CRUISER_PEDIA', 'A cruiser is a type of warship. The term has been in use for several hundred years, and has had different meanings throughout this period. During the Age of Sail, the term cruising referred to certain kinds of missions: independent scouting, raiding or commerce protection fulfilled by a frigate or sloop, which were the cruising warships of a fleet. In the middle of the 19th century, cruiser came to be a classification for the ships intended for this kind of role, though cruisers came in a wide variety of sizes, from the small protected cruiser to armored cruisers which were as large (though not as powerful) as a battleship. By the early 20th century, cruisers could be placed on a consistent scale of warship size, smaller than a battleship but larger than a destroyer. In 1922, the Washington Naval Treaty placed a formal limit on cruisers, which were defined as warships of up to 10,000 tons displacement carrying guns no larger than 8 inches in calibre. These limits shaped cruisers up until the end of World War II. The very large battlecruisers of the World War I era were now classified, along with battleships, as capital ships.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_en_US (Tag, Text)
		SELECT 'TXT_KEY_CIV5_CRUISER_STRATEGY', 'Build Cruisers to augment your Ironclads, and to take control of the seas! Make sure you have plenty of Iron, however.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_en_US (Tag, Text)
		SELECT 'TXT_KEY_CIV5_CRUISER_HELP', 'Industrial-Era Ranged Naval Unit designed to support naval invasions, and to wreak havoc on the wooden ships of previous Eras.[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]Land attacks can only be performed on Coastal Tiles.[ENDCOLOR]'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Powerful Renaissance Era Naval Unit used to wrest control of the seas.[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]Land attacks can only be performed on Coastal Tiles.[ENDCOLOR]'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_FRIGATE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	UPDATE Language_en_US
	SET Text = 'The Galley is a Barbarian unit which remains in coastal hexes, looking for embarked units to destroy. The Galley can annoy cities and units near the coast.'
	WHERE Tag = 'TXT_KEY_UNIT_GALLEY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Dominant Classical Era Naval Unit used to own the seas well into the Medieval Era with its ranged attack.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_BYZANTINE_DROMON' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Attacks with lethal Greek Fire, making it the first naval unit with a ranged attack. It cannot end its turn in Deep Ocean tiles outside of the city borders.'
	WHERE Tag = 'TXT_KEY_UNIT_BYZANTINE_DROMON_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Galleass is the second naval unit with a ranged attack available to the civilizations in the game. It is much stronger than earlier naval units but a bit slower. It cannot initiate melee combat. The Galleass is useful for clearing enemy ships out of shallow waters. It cannot end its turn in Deep Ocean hexes.'
	WHERE Tag = 'TXT_KEY_UNIT_GALLEASS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'A corvette is a small warship. It is traditionally the smallest class of vessel considered to be a proper (or "rated") warship. The warship class above is that of frigate. The class below is historically sloop-of-war. The modern types of ship below a corvette are coastal patrol craft and fast attack craft. In modern terms, a corvette is typically between 500 tons and 2,000 tons although recent designs may approach 3,000 tons, which might instead be considered a small frigate.'
	WHERE Tag = 'TXT_KEY_CIV5_PRIVATEER_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );
	
	UPDATE Language_en_US
	SET Text = 'The Caravel is a significant upgrade to your naval power. A Melee unit, it is stronger and faster than the ageing Trireme, and it can enter Deep Ocean hexes.Use it to explore the world, or to defend your home cities.'
	WHERE Tag = 'TXT_KEY_UNIT_CARAVEL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );
	
	UPDATE Language_en_US
	SET Text = 'Late-Medieval exploration Unit that can enter the Ocean. Fights as a naval melee unit.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_CARAVEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );
	
	UPDATE Language_en_US
	SET Text = 'The Frigate is an upgrade over the Galleass. Its Range, Combat and Ranged Combat strengths are much larger than the older naval unit. The Frigate can clear the seas of any Caravels, Triremes, and Barbarian units still afloat. It cannot, however, fire on non-coastal land tiles.'
	WHERE Tag = 'TXT_KEY_UNIT_FRIGATE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Naval Unit that specializes in attacking coastal cities and capturing enemy ships. Available earlier than the Corvette, which it replaces. Only the Dutch can build it.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_SEA_BEGGAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Corvette'
	WHERE Tag = 'TXT_KEY_UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Naval Unit that specializes in melee combat and quick movement.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Naval Unit that attacks as a melee unit. It is significantly stronger and faster than the Caravel.'
	WHERE Tag = 'TXT_KEY_UNIT_PRIVATEER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'A very powerful naval melee unit, the Ironclad dominates the oceans of the Industrial age.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_IRONCLAD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Unit specialized in fighting Modern Armor and Tanks. It is capable of hovering over Mountains.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_HELICOPTER_GUNSHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Classical Era Naval Unit used to wrest control of the seas.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_TRIREME' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Initially carries 2 Aircraft; capacity may be boosted through promotions. Will intercept Enemy Aircraft which try to attack nearby Units.[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]Land attacks can only be performed on Coastal Tiles.[ENDCOLOR]'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_CARRIER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Carrier is a specialized vessel which carries fighter airplanes, bombers (but not stealth bombers), and atomic bombs. The Carrier itself is unable to attack, but the aircraft it carries make it the most powerful offensive weapon afloat. Defensively, however, the Carrier is weak and should be escorted by destroyers and submarines. Carriers are, however, armed with anti-air weaporny, and will automatically attack any air unit bombing a target within 4 tiles. (They can only intercept one unit per turn.)'
	WHERE Tag = 'TXT_KEY_UNIT_CARRIER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Most powerful Ranged Naval Unit in the game. Starts with [COLOR_POSITIVE_TEXT]Indirect Fire[ENDCOLOR] Promotion.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_BATTLESHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Melee Naval Unit specialized in warding off enemy aircraft and hunting submarines. Can also hold 3 Missiles.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_MISSILE_CRUISER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	UPDATE Language_en_US
	SET Text = 'The Missile Cruiser is a modern warship. It''s fast and tough, carries a mean punch and is pretty good at intercepting enemy aircraft. Most importantly, the Missile Cruiser can carry Guided Missiles and Nuclear Missiles, allowing you to carry these deadly weapons right up to the enemy''s shore. Missile Cruisers combined with carriers, submarines, and battleships make a fiendishly powerful armada.'
	WHERE Tag = 'TXT_KEY_UNIT_MISSILE_CRUISER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );


	-- Mounted

	UPDATE Language_en_US
	SET Text = 'The Lancer is the Renaissance horse unit that comes between the Knight and the first mechanized vehicle, the Landship. It is faster and more powerful than the Knight, able to sweep those once-mighty units from the map. The Lancer is a powerful offensive weapon.'
	WHERE Tag = 'TXT_KEY_UNIT_LANCER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Mounted Unit designed to be on the attack at all times. Good for hunting down enemy ranged units, and for skirmishing on the edges of the battlefield.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_LANCER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Cavalry mounted units are devastating against all units, but somewhat vulnerable on the defensive. Use them to destroy other units and escape via their ability to move after attacking.'
	WHERE Tag = 'TXT_KEY_UNIT_CAVALRY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	INSERT INTO Language_en_US (Tag, Text)
		SELECT 'TXT_KEY_DESC_CUIRASSIER', 'Cuirassier'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_en_US (Tag, Text)
		SELECT 'TXT_KEY_CIV5_CUIRASSIER_PEDIA', 'Cuirassiers, from French cuirassier, were cavalry equipped with armour and firearms, first appearing in late 15th-century Europe. This French term means "the one with a cuirass" (cuirasse), the breastplate armour which they wore. The first cuirassiers were produced as a result of armoured cavalry, such as the man-at-arms and demi-lancer, discarding their lances and adopting the use of pistols as their primary weapon. In the later 17th century, the cuirassier lost his limb armour and subsequently employed only the cuirass (breastplate and backplate), and sometimes a helmet. By this time, the sword was the primary weapon of the cuirassier, pistols being relegated to a secondary function.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_en_US (Tag, Text)
		SELECT 'TXT_KEY_CIV5_CUIRASSIER_STRATEGY', 'Cuirassiers are powerful Renaissance-Era units capable of rapid movement and deadly ranged attacks. Use them to harass the enemy and support your war effort.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_en_US (Tag, Text)
		SELECT 'TXT_KEY_CIV5_CUIRASSIER_HELP', 'Powerful Renaissance-Era mounted ranged unit, useful for harassment and hit-and-run tactics.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_en_US (Tag, Text)
		SELECT 'TXT_KEY_DESC_MOUNTED_BOWMAN', 'Heavy Skirmisher'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_en_US (Tag, Text)
		SELECT 'TXT_KEY_CIV5_MOUNTED_BOWMAN_PEDIA', 'Mounted knights armed with lances proved ineffective against formations of pikemen combined with crossbowmen whose weapons could penetrate most knights'' armor. The invention of pushlever and ratchet drawing mechanisms enabled the use of crossbows on horseback, leading to the development of new cavalry tactics. Knights and mercenaries deployed in triangular formations, with the most heavily armored knights at the front. Some of these riders would carry small, powerful all-metal crossbows of their own. Crossbows were eventually replaced in warfare by more powerful gunpowder weapons, although early guns had slower rates of fire and much worse accuracy than contemporary crossbows. Later, similar competing tactics would feature harquebusiers or musketeers in formation with pikemen (pike and shot), pitted against cavalry firing pistols or carbines.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_en_US (Tag, Text)
		SELECT 'TXT_KEY_CIV5_MOUNTED_BOWMAN_STRATEGY', 'Heavy Skirmishers are powerful Medieval-Era units capable of rapid movement and deadly ranged attacks. Use them to harass the enemy and support your war effort, but be careful about letting them get caught alone.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_en_US (Tag, Text)
		SELECT 'TXT_KEY_CIV5_MOUNTED_BOWMAN_HELP', 'Powerful Medieval-Era mounted ranged unit, useful for harassment and hit-and-run tactics.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Ranged

	INSERT INTO Language_en_US (Tag, Text)
		SELECT 'TXT_KEY_DESC_VOLLEY_GUN', 'Volley Gun'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_en_US (Tag, Text)
		SELECT 'TXT_KEY_CIV5_VOLLEY_GUN_PEDIA', 'A volley gun is a gun with several barrels for firing a number of shots, either simultaneously or in sequence. They differ from modern machine guns in that they lack automatic loading and automatic fire and are limited by the number of barrels bundled together. '
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_en_US (Tag, Text)
		SELECT 'TXT_KEY_CIV5_VOLLEY_GUN_STRATEGY', 'Volley Guns are a powerful Renaissance ranged weapon, though they are quite vulnerable to Melee. Keep them away from your front lines.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_en_US (Tag, Text)
		SELECT 'TXT_KEY_CIV5_VOLLEY_GUN_HELP', 'Renaissance-Era ranged land unit designed to support your Musketmen.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Ranged infantry Unit of the mid-game that weakens nearby enemy units.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_GATLINGGUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Gatling Gun is a mid-game non-siege ranged unit that can lay down a terrifying hail of bullets. It is much more powerful than earlier ranged units like the Musketman, but it is much weaker than other military units of its era. As such, it should be used as a source of attrition. It weakens nearby enemy units, and gains bonus strength when defending. When attacking, the Gatling Gun deals less damage to Armored or fortified Units, as well as cities. Put Gatling Guns in your cities or on chokepoints for optimal defensive power.'
	WHERE Tag = 'TXT_KEY_UNIT_GATLINGGUN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	UPDATE Language_en_US
	SET Text = 'Late-game ranged Unit that weakens nearby enemy units.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_MACHINE_GUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Machine Gun is the penultimate non-siege ranged unit, and can lay down a terrifying hail of suppressive fire. It is more powerful than earlier ranged units, but it is much weaker than other military units of its era. As such, it should be used as a source of attrition. Like the Gatling Gun, it weakens nearby enemy units. When attacking, the Machine Gun deals less damage to Armored or fortified Units, as well as cities. It is vulnerable to melee attack. Put Machine Guns in your city for defense, or use them to control chokepoints.'
	WHERE Tag = 'TXT_KEY_UNIT_MACHINE_GUN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Atomic era ranged unit. Deals great damage to Armor Units.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_BAZOOKA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Bazooka is the last non-siege ranged unit, and is capable of truly terrfiying amounts of damage, especially to Armor units. It is the most powerful ranged unit, but it is much weaker than other military units of its era. As such, it should be used as a source of attrition. Like the Machine Gun, it weakens nearby enemy units. When attacking, the Bazooka deals less damage to fortified Units and cities, but deals additional damage to Armored units. This makes it a great defense unit.'
	WHERE Tag = 'TXT_KEY_UNIT_BAZOOKA_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Artillery is a deadly siege weapon, more powerful than a cannon and with a longer range. Like the cannon it has limited visibility and must set up (1 mp) to attack, but its Ranged Combat strength is tremendous. Artillery also has the "indirect fire" ability, allowing it to shoot over obstructions at targets it cannot see (as long as other friendly units can see them). Like other siege weapons, Artillery is vulnerable to melee attack.'
	WHERE Tag = 'TXT_KEY_UNIT_ARTILLERY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Capable of firing 3 tiles away. Must set up prior to firing.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_ARTILLERY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Quests
	
	UPDATE Language_en_US
	SET Text = 'The people of {3_MinorCivName:textkey} look to worldly affairs for religious guidance.  Whichever global religion can garner the most [ICON_PEACE] Followers in a period of {2_TurnsDuration} turns will gain [ICON_INFLUENCE] Influence with them.  [COLOR_POSITIVE_TEXT]{1_TurnsRemaining} turns remaining.[ENDCOLOR]'
	WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_CONTEST_FAITH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );


	UPDATE Language_en_US
	SET Text = '{1_MinorCivName:textkey} calls for conversions!'
	WHERE Tag = 'TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONTEST_FAITH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Another religion has impressed {1_MinorCivName:textkey} with its faith.  Your faith growth was not enough, and your [ICON_INFLUENCE] Influence remains the same as before.  Civilizations that succeeded (ties are allowed):[NEWLINE]'
	WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_FAITH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
