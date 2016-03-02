-- CIVILIANS AND GREAT PEOPLE

	-- Moved SS Parts to last 4 techs - makes Science Victory as difficult as other victories.

	UPDATE Units
	SET PrereqTech = 'TECH_NUCLEAR_FUSION'
	WHERE Type = 'UNIT_SS_BOOSTER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_GLOBALIZATION'
	WHERE Type = 'UNIT_SS_COCKPIT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Caravans moved to Pottery
	UPDATE Units
	SET PrereqTech = 'TECH_POTTERY'
	WHERE Type = 'UNIT_CARAVAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Cargo Ship -- Move to Optics

	UPDATE Units
	SET PrereqTech = 'TECH_OPTICS'
	WHERE Type = 'UNIT_CARGO_SHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Settler moved to Pottery
	UPDATE Units
	SET PrereqTech = 'TECH_POTTERY'
	WHERE Type = 'UNIT_SETTLER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_Flavors
	SET Flavor = '25'
	WHERE UnitType = 'UNIT_SETTLER';

-- Workers Reduced Work Rate to slow down early growth
	UPDATE Units
	SET WorkRate = '90'
	WHERE Type = 'UNIT_WORKER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Great Prophets no longer capturable
	DELETE FROM Units
	WHERE Type = 'UNIT_PROPHET' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Great Writer culture boost boosted slightly.

	UPDATE Units
	SET BaseCultureTurnsToCount = '10'
	WHERE Type = 'UNIT_WRITER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Great Scientist science boost buffed slightly.

	UPDATE Units
	SET BaseBeakersTurnsToCount = '10'
	WHERE Type = 'UNIT_SCIENTIST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Admiral movement buff
	UPDATE Units
	SET Moves = '5'
	WHERE Type = 'UNIT_GREAT_ADMIRAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET NumFreeLux = '2'
	WHERE Type = 'UNIT_GREAT_ADMIRAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Great Admiral can immediately cross oceans for free, making it a useful (if vulnerable) exploration vessel. Furthermore, while in owned territory, the Great Admiral can be sent on a ''Voyage of Discovery'' that expends the unit and gives you two copies of a Luxury Resource not available on the current map.[NEWLINE][NEWLINE]The Great Admiral has the ability to instantly repair every naval and embarked unit in the same hex, as well as in adjacent hexes. The Great Admiral is consumed when performing this action. The Great Admiral also provides a 15% combat bonus to all player-owned naval units within 2 tiles. The Great Admiral is NOT consumed when he provides this bonus.'
	WHERE Tag = 'TXT_KEY_UNIT_GREAT_ADMIRAL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Merchant gold boosted

	UPDATE Units
	SET NumGoldPerEra = '150'
	WHERE Type = 'UNIT_MERCHANT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'If the unit is inside City-State territory that you are not at war with, this order will expend the unit.[NEWLINE][NEWLINE]You will receive a large amount of [ICON_GOLD] Gold and an instant ''We Love the King Day'' in all owned cities. This action will consume the unit.'
	WHERE Tag = 'TXT_KEY_MISSION_CONDUCT_TRADE_MISSION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Great Merchant can construct the special Custom House improvement which, when worked, produces loads of [ICON_GOLD] Gold. The Great Merchant can also journey to a city-state and perform a "trade mission" which produces gobs of [ICON_GOLD] Gold, starts a ''We Love the King Day'' in all owned cities and, (if not using CSD) garners many Influence Points with the city-state. The Great Merchant is expended when used in any of these ways.'
	WHERE Tag = 'TXT_KEY_UNIT_GREAT_MERCHANT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Engineer production boost nerfed slightly.

	UPDATE Units
	SET BaseHurry = '250'
	WHERE Type = 'UNIT_ENGINEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET HurryMultiplier = '25'
	WHERE Type = 'UNIT_ENGINEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Changed how Musician Great Tour Works
	UPDATE Language_en_US
	SET Text = 'The value of this action depends on your [ICON_TOURISM] Cultural Influence over the target Civilization.[NEWLINE][NEWLINE][ICON_BULLET]If [COLOR_MAGENTA]Unknown[ENDCOLOR] or [COLOR_MAGENTA]Exotic[ENDCOLOR]: Your [ICON_TOURISM] Tourism with the target Civilization is immediately raised to [COLOR_MAGENTA]Familiar[ENDCOLOR] status, and you receive 2 [ICON_HAPPINESS_1] Happiness in your [ICON_CAPITAL] Capital.[NEWLINE][NEWLINE][ICON_BULLET]If [COLOR_MAGENTA]Familiar[ENDCOLOR] or [COLOR_MAGENTA]Popular[ENDCOLOR]: Gain large amount of [ICON_TOURISM] Tourism based on your Tourism and Culture output as Cultural Influence over this Civilization (20% of this [ICON_TOURISM] Tourism is also applied to all other known Civs). Additionally, you receive [ICON_HAPPINESS_1] Happiness in your [ICON_CAPITAL] Capital (scaling with Era), and the target Civilization experiences ''We Love the King Day'' in all owned Cities, scaling with your Cultural Influence and Gamespeed. This action consumes the unit.[NEWLINE][NEWLINE]Cannot perform this action if at war with the target Civilization, or if your [ICON_TOURISM] Cultural Influence over the Civilization is [COLOR_MAGENTA]Influential[ENDCOLOR] or greater.'
	WHERE Tag = 'TXT_KEY_MISSION_ONE_SHOT_TOURISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'A Great Musician can create a Great Work of Music (generates both [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism) that is placed in the nearest city that has an appropriate building with an empty slot (like an Opera House or Broadcast Tower).[NEWLINE][NEWLINE]A Great Musician can also travel to another civilization and perform a [COLOR_POSITIVE_TEXT]Concert Tour[ENDCOLOR]. The value of this action depends on your [ICON_TOURISM] Cultural Influence over the target Civilization.[NEWLINE][NEWLINE][ICON_BULLET]If [COLOR_MAGENTA]Unknown[ENDCOLOR] or [COLOR_MAGENTA]Exotic[ENDCOLOR]: Your [ICON_TOURISM] Tourism with the target Civilization is immediately raised to [COLOR_MAGENTA]Familiar[ENDCOLOR] status, and you receive 2 [ICON_HAPPINESS_1] Happiness in your [ICON_CAPITAL] Capital.[NEWLINE][NEWLINE][ICON_BULLET]If [COLOR_MAGENTA]Familiar[ENDCOLOR] or [COLOR_MAGENTA]Popular[ENDCOLOR]: Gain large amount of [ICON_TOURISM] Tourism based on your Tourism and Culture output as Cultural Influence over this Civilization (20% of this [ICON_TOURISM] Tourism is also applied to all other known Civs). Additionally, you receive [ICON_HAPPINESS_1] Happiness in your [ICON_CAPITAL] Capital (scaling with Era), and the target Civilization experiences ''We Love the King Day'' in all owned Cities, scaling with your Cultural Influence and Gamespeed.[NEWLINE][NEWLINE]Great Musicians are expended when used either of these ways. Cannot perform this action if at war with the target Civilization, or if your [ICON_TOURISM] Cultural Influence over the Civilization is [COLOR_MAGENTA]Influential[ENDCOLOR] or greater.'
	WHERE Tag = 'TXT_KEY_UNIT_GREAT_MUSICIAN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Archaeologist Text

	UPDATE Language_en_US
	SET Text = 'Archaeologists are a special subtype of Worker that are used to excavate Antiquity Sites to either create Landmark improvements or to extract Artifacts to fill in Great Work of Art slots in Museums, Palaces, Hermitages, and selected Wonders. Archaeologists may work in territory owned by any player. They are consumed once they complete an Archaeological Dig at an Antiquity Site. Archaeologists may not be purchased with [ICON_GOLD] Gold and may only be built in a city with a Public School.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_ARCHAEOLOGIST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Buff Barbarians

	UPDATE Units
	SET ShowInPedia = '0'
	WHERE Type = 'UNIT_BARBARIAN_AXMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_TRAPPING'
	WHERE Type = 'UNIT_BARBARIAN_ARCHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );



	

	

	

	