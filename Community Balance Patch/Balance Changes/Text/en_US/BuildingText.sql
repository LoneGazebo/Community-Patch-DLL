-- added Recycling Center
UPDATE Language_en_US
SET Text = 'Provides 2 [ICON_RES_ALUMINUM] Aluminum.[NEWLINE][NEWLINE]Maximum of 5 of these Buildings in your Empire.'
WHERE Tag = 'TXT_KEY_BUILDING_RECYCLING_CENTER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_RESEARCH] Science for every 3 [ICON_CITIZEN] Citizens in the City. Contains 1 slot for a [ICON_GREAT_WORK] Great Work of Art. 1 Specialist in this City no longer produces [ICON_HAPPINESS_3] Unhappiness from Urbanization.[NEWLINE][NEWLINE]When one of the following [COLOR_POSITIVE_TEXT]Historic Events[ENDCOLOR] is completed, receive a [ICON_TOURISM] Tourism boost with all known Civs based on your recent [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism output:[NEWLINE][ICON_BULLET] Earn a [ICON_GREAT_PEOPLE] Great Person[NEWLINE][ICON_BULLET] Build a [ICON_GOLDEN_AGE] World Wonder[NEWLINE][ICON_BULLET] Win a [ICON_WAR] War (Warscore 25+)[NEWLINE][ICON_BULLET] Enter a new [ICON_RESEARCH] Era[NEWLINE][NEWLINE][ICON_CONNECTED] Connecting other Cities to the [ICON_CAPITAL] Capital by Road will produce additional [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_PALACE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+25 [ICON_FOOD] Food when completed. 15% of [ICON_FOOD] Food is carried over after a new [ICON_CITIZEN] Citizen is born.[NEWLINE][NEWLINE]Allows [ICON_FOOD] Food to be moved from this city along trade routes inside your civilization.[NEWLINE][NEWLINE]Nearby [ICON_RES_WHEAT] Wheat: +1 [ICON_FOOD] Food.[NEWLINE]Nearby [ICON_RES_BANANA] Bananas: +1 [ICON_FOOD] Food.[NEWLINE]Nearby [ICON_RES_DEER] Deer: +1 [ICON_FOOD] Food.[NEWLINE]Nearby [ICON_RES_BISON] Bison: +1 [ICON_FOOD] Food.'
WHERE Tag = 'TXT_KEY_BUILDING_GRANARY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_PRODUCTION] Production for every 4 [ICON_CITIZEN] Citizens in the City.[NEWLINE][NEWLINE]Can only be built in a City next to a River. Cannot be built if the City already has a [COLOR_NEGATIVE_TEXT]Well[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_BUILDING_WATERMILL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Water Mill can only be constructed in a city located next to a river. The Water Mill increases the city''s [ICON_PRODUCTION] Production more efficiently (per Citizen in the City) than the Well.'
WHERE Tag = 'TXT_KEY_BUILDING_WATERMILL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '15% of [ICON_FOOD] Food is carried over after a new [ICON_CITIZEN] Citizen is born, and +25% of the [ICON_PRODUCTION] Production of the City is added to the City''s current [ICON_PRODUCTION] Production. +2 [ICON_FOOD] Food on Lakes and Oases worked by this City.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty.'
WHERE Tag = 'TXT_KEY_BUILDING_AQUEDUCT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Aqueduct decreases the amount of [ICON_FOOD] Food a city needs to increase in size by 15%. Build Aqueducts in cities that you want to grow large over time.'
WHERE Tag = 'TXT_KEY_BUILDING_AQUEDUCT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+2 [ICON_CITIZEN] Population when completed. 15% of [ICON_FOOD] Food is carried over after a new [ICON_CITIZEN] Citizen is born. Scientist, Merchant, and Engineer Specialists in the City produce +2 of their base Yield.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty.'
WHERE Tag = 'TXT_KEY_BUILDING_MEDICAL_LAB_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Medical Lab is a late-game building which decreases the amount of [ICON_FOOD] Food a city needs to increase in size by 15% and boosts the City''s Science production from specialists. The city needs to have a Hospital in order to construct the Medical Lab.'
WHERE Tag = 'TXT_KEY_BUILDING_MEDICAL_LAB_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+25% [ICON_GREAT_PEOPLE] Great People generation in this City.[NEWLINE][NEWLINE]1 Specialist in this City no longer produces [ICON_HAPPINESS_3] Unhappiness from Urbanization.[NEWLINE][NEWLINE]Nearby Oases: +2 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_CITRUS] Citrus: +1 [ICON_FOOD] Food, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COCOA] Cocoa: +1 [ICON_FOOD] Food, +1 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_GARDEN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Garden increases the speed at which [ICON_GREAT_PEOPLE] Great People are generated in the city by 25%, and buffs the food output of Oases. Gardens require an Aqueduct in the City in order to be built.'
WHERE Tag = 'TXT_KEY_BUILDING_GARDEN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
SELECT 'TXT_KEY_BUILDING_HOSPITAL_HELP', '+1 [ICON_FOOD] Food for every 10 [ICON_CITIZEN] Citizens in the City. 5% of the City''s [ICON_FOOD] Food is converted into [ICON_RESEARCH] Science every turn. Land Units in this City heal 15 points per turn whether or not they take an action. Increases Military Units Supplied by this City''s population by 10%.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
SELECT 'TXT_KEY_BUILDING_TEMPLE_HELP', '-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_PEACE] Religious Unrest. +25% Religious Pressure. Contains 1 slot for a Great Work of Music.[NEWLINE][NEWLINE]Nearby [ICON_RES_INCENSE] Incense: +1 [ICON_CULTURE] Culture, +1 [ICON_GOLD] Gold.[NEWLINE] Nearby [ICON_RES_WINE] Wine: +1 [ICON_CULTURE] Culture, +1 [ICON_GOLD] Gold.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
SELECT 'TXT_KEY_BUILDING_MONUMENT_HELP', '[ICON_CULTURE] Culture costs of acquiring new tiles reduced by 25% in this city.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
SELECT 'TXT_KEY_BUILDING_AMPHITHEATER_HELP', '+33% [ICON_GREAT_WRITER] Great Writer Rate in the City, and all Writers'' Guilds produce +1 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]Nearby [ICON_RES_DYE] Dye: +1 [ICON_CULTURE] Culture, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_SILK] Silk: +1 [ICON_CULTURE] Culture, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_LAPIS] Lapis Lazuli: +2 [ICON_CULTURE] Culture.[NEWLINE][NEWLINE]+2 [ICON_CULTURE] Culture if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = 'The Amphitheater increases the [ICON_CULTURE] Culture of a city and grants bonuses to nearby [ICON_RES_DYE] Dye and [ICON_RES_SILK] Silk. Also boosts the City''s Great Writer rate and the value of Writers'' Guilds. Contains 2 slots for a Great Work of Writing.'
WHERE Tag = 'TXT_KEY_BUILDING_AMPHITHEATER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
SELECT 'TXT_KEY_BUILDING_OPERA_HOUSE_HELP', '+5% [ICON_CULTURE] Culture in the City. +33% [ICON_GREAT_MUSICIAN] Great Musician Rate in the City, and all Musicians'' Guilds produce +1 [ICON_GOLD] Gold.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = 'The Opera House is a Renaissance-era building which increases the [ICON_CULTURE] Culture of a city. Also boosts the City''s Great Musician rate and the value of Musicians'' Guilds. Contains 1 slot for a Great Work of Music. Requires an Amphitheater in the city before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_OPERA_HOUSE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Museum is a mid-game building which increases [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism output and reduces Boredom. Also boosts the City''s Great Artist rate and the value of Artists'' Guilds. Contains 2 slots for Great Works of Art.  Requires an Opera House in the city before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_MUSEUM_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
SELECT 'TXT_KEY_BUILDING_MUSEUM_HELP', '+1 [ICON_CULTURE] Culture for every 4 [ICON_CITIZEN] Citizens in the City. [ICON_GREAT_WORK] Great Works in the City produce +1 [ICON_TOURISM] Tourism. +33% [ICON_GREAT_ARTIST] Great Artist Rate in the City, and all Artists'' Guilds produce +1 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom.[NEWLINE][NEWLINE]+4 [ICON_CULTURE] Culture if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism for every 2 [ICON_CITIZEN] Citizens in the City. [ICON_GREAT_WORK] Great Works in the City produce +2 [ICON_TOURISM] Tourism.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom.[NEWLINE][NEWLINE]+10 [ICON_GOLD] Gold if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_BUILDING_BROADCAST_TOWER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A Broadcast Tower is a late-game building which increases [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism output of the city, and reduces Boredom. Requires a Museum in the city before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_BROADCAST_TOWER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+15 XP for all Units. Increases the Military Unit Supply Cap by 1.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD]/[ICON_PRODUCTION] Distress.'
WHERE Tag = 'TXT_KEY_BUILDING_BARRACKS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = '+20 XP for all Units. Increases the Military Unit Supply Cap by 1.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD]/[ICON_PRODUCTION] Distress.'
WHERE Tag = 'TXT_KEY_BUILDING_ARMORY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+15% [ICON_PRODUCTION] Production towards Land Units, and +25 XP for all Units. Increases the Military Unit Supply Cap by 1.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD]/[ICON_PRODUCTION] Distress.'
WHERE Tag = 'TXT_KEY_BUILDING_MILITARY_ACADEMY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
		
UPDATE Language_en_US
SET Text = '+33% [ICON_PRODUCTION] Production when building Mounted Melee Units. Increases the Military Unit Supply Cap from Population in the City by 10%. Internal [ICON_INTERNATIONAL_TRADE] Trade Routes from this City generate +2 [ICON_PRODUCTION] Production.[NEWLINE][NEWLINE]Nearby [ICON_RES_HORSE] Horses: +2 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_SHEEP] Sheep: +2 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_COW] Cattle: +2 [ICON_PRODUCTION] Production.[NEWLINE][NEWLINE]City must have at least one of these resources improved with a Pasture.'
WHERE Tag = 'TXT_KEY_BUILDING_STABLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = 'Mines worked by this City gain +2 [ICON_PRODUCTION] Production.[NEWLINE][NEWLINE]Nearby [ICON_RES_IRON] Iron: +1 [ICON_PRODUCTION] Production, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COPPER] Copper: +2 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_FORGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Forge improves sources of [ICON_RES_IRON] Iron and [ICON_RES_COPPER] Copper nearby, and boosts the [ICON_PRODUCTION] Production of Mines.'
WHERE Tag = 'TXT_KEY_BUILDING_FORGE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_PRODUCTION] Production and [ICON_GOLD] Gold from all Forests worked by this City, and +1 [ICON_PRODUCTION] Production for every 10 [ICON_CITIZEN] Citizens in the City. Internal [ICON_INTERNATIONAL_TRADE] Trade Routes from this City generate +4 [ICON_PRODUCTION] Production.[NEWLINE][NEWLINE]Allows [ICON_PRODUCTION] Production to be moved from this city along trade routes inside your civilization.' WHERE Tag = 'TXT_KEY_BUILDING_WORKSHOP_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = 'Allows [ICON_PRODUCTION] Production to be moved from this city along trade routes inside your civilization.[NEWLINE][NEWLINE]Nearby [ICON_RES_MARBLE] Marble: +1 [ICON_PRODUCTION] Production, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_STONE] Stone: +2 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_SALT] Salt: +2 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]City must have at least one of these resources improved.'
WHERE Tag = 'TXT_KEY_BUILDING_STONE_WORKS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = 'Stone Works can only be constructed in a city near an improved [ICON_RES_STONE] Stone, [ICON_RES_SALT] Salt, [ICON_RES_MARBLE] Marble, or [ICON_RES_URANIUM] Uranium resource. Stone Works increase [ICON_PRODUCTION] Production and allow [ICON_PRODUCTION] Production to be moved from this city along trade routes inside your civilization.'
WHERE Tag = 'TXT_KEY_BUILDING_STONE_WORKS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
		
UPDATE Language_en_US
SET Text = '+1 [ICON_FOOD] Food and +1 [ICON_GOLD] Gold from Coast and Ocean Tiles. Internal [ICON_INTERNATIONAL_TRADE] Trade Routes from this City generate +4 [ICON_FOOD] Food. Increases Military Units Supply Cap by 1.[NEWLINE][NEWLINE]Forms a [ICON_CONNECTED] City Connection with other Cities via water if they also have Lighthouses.[NEWLINE][NEWLINE]Can only be constructed in a coastal City.'
WHERE Tag = 'TXT_KEY_BUILDING_LIGHTHOUSE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
		
UPDATE Language_en_US
SET Text = 'The Lighthouse can only be constructed in a city next to a coastal tile. It increases the [ICON_FOOD] Food and [ICON_GOLD] Gold output of water tiles. Also allows for City Connections over Water (see Concepts for more details).'
WHERE Tag = 'TXT_KEY_BUILDING_LIGHTHOUSE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
		
UPDATE Language_en_US
SET Text = 'If a city is connected by a road and/or Lighthouse to your capital city (i.e. both cities have a Lighthouse), that city has a "trade route" with the capital. Each trade route is worth a certain amount of gold each turn, the amount determined by the size of the two cities.'
WHERE Tag = 'TXT_KEY_GOLD_TRADE_ROUTES_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'When a [ICON_INTERNATIONAL_TRADE] Sea Trade Route originating here and targeting another Civ is completed, receive a [ICON_TOURISM] Tourism boost with the Civ based on your recent [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism output.[NEWLINE][NEWLINE]+1 [ICON_FOOD] Food from Coast and Ocean Tiles, and +1 [ICON_PRODUCTION] Production from Sea Resources worked by this City. Sea Trade Routes gain +50% Range and +2 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]+15% [ICON_PRODUCTION] Production of Naval Units, increases Military Units Supply Cap by 2, and City Hit Points by 150.[NEWLINE][NEWLINE]Can only be constructed in a coastal City.'
WHERE Tag = 'TXT_KEY_BUILDING_HARBOR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Harbor is a Medieval-era building, requiring a Lighthouse. It improves the range and [ICON_GOLD] Gold yield of sea trade routes, and boosts the value of sea tiles and resources. The Harbor also increases the [ICON_PRODUCTION] Production of Naval units by 15%, and increases Military Units Supplied by this City''s population by 10%.'
WHERE Tag = 'TXT_KEY_BUILDING_HARBOR_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_PRODUCTION] Production and +1 [ICON_GOLD] Gold from all Coast and Ocean Tiles. +2 [ICON_PRODUCTION] Production and [ICON_GOLD] Gold from Sea Resources worked by this City. Increases the Military Unit Supply Cap from Population in the City by 20%.[NEWLINE][NEWLINE]Requires 1 [ICON_RES_COAL] Coal. Can only be constructed in a coastal City, and cannot have a [COLOR_NEGATIVE_TEXT]Train Station[ENDCOLOR] in the City.'
WHERE Tag = 'TXT_KEY_BUILDING_SEAPORT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Provides a massive boost to City Production and Sea Resource yields during the Industrial Era. Increases the Military Unit Supply Cap from Population in the City by 10%. Requires a Harbor, and cannot be built in the same City as a Train Station.'
WHERE Tag = 'TXT_KEY_BUILDING_SEAPORT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
INSERT INTO Language_en_US (Tag, Text)
SELECT 'TXT_KEY_BUILDING_MARKET_HELP', '[ICON_INTERNATIONAL_TRADE] Trade Routes to this City generate +1 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]Nearby [ICON_RES_SPICES] Cinnamon: +1 [ICON_FOOD] Food, +1 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_SUGAR] Sugar: +1 [ICON_FOOD] Food, +1 [ICON_GOLD] Gold.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Markets increase the amount of [ICON_GOLD] Gold a city generates, and improves nearby [ICON_RES_SPICES] Cinnamon and [ICON_RES_SUGAR] Sugar. Incoming [ICON_INTERNATIONAL_TRADE] Trade Routes generate +1 [ICON_GOLD] Gold for the City, and +1 [ICON_GOLD] Gold for Trade Route owner.'
WHERE Tag = 'TXT_KEY_BUILDING_MARKET_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Incoming [ICON_INTERNATIONAL_TRADE] Trade Routes generate +2 [ICON_GOLD] Gold for the City, and +2 [ICON_GOLD] Gold for [ICON_INTERNATIONAL_TRADE] Trade Route owner.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty.[NEWLINE] Nearby [ICON_RES_BANANA] Bananas: +2 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_MINT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Customs House boosts the Gold value of Trade Routes, and boosts the Tourism you generate from Trade Routes to foreign Civilizations. Build these buildings in all Cities if you wish to improve your Gold ouput as well as the cultural value of your Trade Routes.'
WHERE Tag = 'TXT_KEY_BUILDING_MINT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Customs House'
WHERE Tag = 'TXT_KEY_BUILDING_MINT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A custom house or customs house was a building housing the offices for the government officials who processed the paperwork for the import and export of goods into and out of a country. Customs officials also collected customs duty on imported goods. The custom house was typically located in a seaport or in a city on a major river with access to the ocean. These cities acted as a port of entry into a country. The government had officials at such locations to collect taxes and regulate commerce. Due to advances in electronic information systems, the increased volume of international trade and the introduction of air travel, the custom house is now often a historical anachronism. There are many examples of buildings around the world whose former use was as a custom house but that have since been converted for other use, such as museums or civic buildings.'
WHERE Tag = 'TXT_KEY_CIV5_BUILDINGS_MINT_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'When you spend [ICON_GOLD] Gold to purchase Units or invest in Buildings in this City, 15% of the cost is converted into [ICON_RESEARCH] Science. Caravansaries and Customs Houses (or Hanse) in the City gain +3 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]Nearby [ICON_RES_GOLD] Gold: +3 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_SILVER] Silver: +1 [ICON_GOLD] Gold, +1 [ICON_PRODUCTION] Production, +1 [ICON_CULTURE] Culture.[NEWLINE]Nearby [ICON_RES_GEMS] Gems: +2 [ICON_CULTURE] Culture, +1 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_BANK_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Bank is a Renaissance-era building which increases the city''s output of [ICON_GOLD] Gold, boosts the value of investments in the City, and reduces Poverty. A city must possess a Market or Bazaar before a Bank may be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_BANK_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_GOLD] Gold for every 2 [ICON_CITIZEN] Citizens in the City. Cost of [ICON_GOLD] Gold purchasing in this City reduced by 10%. +2 [ICON_GOLD] Gold on Towns worked by this City.'
WHERE Tag = 'TXT_KEY_BUILDING_STOCK_EXCHANGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'When a [ICON_INTERNATIONAL_TRADE] Land Trade Route originating here and targeting another Civ is completed, receive a [ICON_TOURISM] Tourism boost with the Civ based on recent [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism output.[NEWLINE][NEWLINE]+1 [ICON_FOOD] Food and [ICON_GOLD] Gold for every 3 Desert or 3 Tundra tiles worked by the City. Land Trade Routes gain +50% Range and +3 [ICON_GOLD] Gold, Merchant Specialists in this City gain +1 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]Nearby [ICON_RES_TRUFFLES] Truffles: +2 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COTTON] Cotton: +1 [ICON_PRODUCTION] Production, +1 [ICON_CULTURE] Culture.[NEWLINE]Nearby [ICON_RES_FUR] Furs: +1 [ICON_GOLD] Gold, +1 [ICON_PRODUCTION] Production.'
WHERE Tag = 'TXT_KEY_BUILDING_CARAVANSARY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_RESEARCH] Illiteracy. 1 Specialist in this City no longer produces [ICON_HAPPINESS_3] Unhappiness from Urbanization.'
WHERE Tag = 'TXT_KEY_BUILDING_LIBRARY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_RESEARCH] Science from Jungle tiles worked by the City. Gain 25% of the [ICON_RESEARCH] Science output of the City as an instant boost to your current Research when a [ICON_CITIZEN] Citizen is born in this City.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_RESEARCH] Illiteracy.'
WHERE Tag = 'TXT_KEY_BUILDING_UNIVERSITY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
    
UPDATE Language_en_US
SET Text = 'Allows [COLOR_YELLOW]Archaeologists[ENDCOLOR] to be built in this City. +1 [ICON_RESEARCH] Science for every 4 [ICON_CITIZEN] Citizens in the City.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_RESEARCH] Illiteracy.'
WHERE Tag = 'TXT_KEY_BUILDING_PUBLIC_SCHOOL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+4 [ICON_RESEARCH] Science from [ICON_RES_ALUMINUM] Aluminum, [ICON_RES_URANIUM] Uranium, and Academies worked by this City.[NEWLINE][NEWLINE]+4 [ICON_RESEARCH] Science from Hospitals, Factories, and Medical Labs, and [ICON_GREAT_SCIENTIST] Great Scientists are produced 33% more quickly in this City.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_RESEARCH] Illiteracy.'
WHERE Tag = 'TXT_KEY_BUILDING_LABORATORY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'City gains +1 [ICON_RESEARCH] Science for every Mountain within 3 tiles of the City.[NEWLINE][NEWLINE]Requires [COLOR_MAGENTA]Scientific Revolution[ENDCOLOR] Policy.'
WHERE Tag = 'TXT_KEY_BUILDING_OBSERVATORY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = 'The Observatory increases [ICON_RESEARCH] Science output, especially for Cities near lots of Mountains. Requires ''Scientific Revolution'' Policy to unlock.'
WHERE Tag = 'TXT_KEY_BUILDING_OBSERVATORY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+15% [ICON_PRODUCTION] Production when constructing Buildings. Grocers and Granaries in the City produce +1 [ICON_FOOD] Food. Nearby Marshes and Lakes produce +2 [ICON_PRODUCTION] Production and [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_WINDMILL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = 'The Windmill is a Renaissance-era building which increases the [ICON_PRODUCTION] Production output of a city when constructing buildings.'
WHERE Tag = 'TXT_KEY_BUILDING_WINDMILL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+3 [ICON_PRODUCTION] Production, [ICON_RESEARCH] Science, and [ICON_GOLD] Gold on River and Lake tiles, and +2 of these Yields on Coast and Ocean Tiles.[NEWLINE][NEWLINE]Requires 1 [ICON_RES_IRON] Iron to be built. City must not contain another energy-producing Plant.'
WHERE Tag = 'TXT_KEY_BUILDING_HYDRO_PLANT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Hydroelectric Power is a Modern-era power system that greatly improves the yield output of water tiles. Construct this building in cities near the ocean or lots of lakes.'
WHERE Tag = 'TXT_KEY_BUILDING_HYDRO_PLANT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Hydroelectric Power'
WHERE Tag = 'TXT_KEY_BUILDING_HYDRO_PLANT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Specialists in the City produce +2 of their base yield and generate [ICON_GREAT_PEOPLE] Great People 25% more quickly.[NEWLINE][NEWLINE]Requires 1 [ICON_RES_URANIUM] Uranium. City must not contain another energy-producing Plant.'
WHERE Tag = 'TXT_KEY_BUILDING_NUCLEAR_PLANT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Nuclear Power'
WHERE Tag = 'TXT_KEY_BUILDING_NUCLEAR_PLANT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'All City Processes are 15% more efficient at converting [ICON_PRODUCTION] Production into Yields. [NEWLINE][NEWLINE]Requires 1 [ICON_RES_ALUMINUM] Aluminum to be built. City must not contain another energy-producing Plant.'
WHERE Tag = 'TXT_KEY_BUILDING_SOLAR_PLANT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Solar Power'
WHERE Tag = 'TXT_KEY_BUILDING_SOLAR_PLANT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Eliminates extra [ICON_HAPPINESS_4] Unhappiness from an [ICON_OCCUPIED] Occupied City. Cost increases based on the number of cities in your empire.'
WHERE Tag = 'TXT_KEY_BUILDING_COURTHOUSE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Walls increase a city Defense Strength and Hit Points, making the city more difficult to capture. Increases Military Units supplied by this City''s population by 10%, and increases the City''s Ranged Strike range by 1. Also helps with managing the Empire Needs Modifier in this City. Walls are quite useful for cities located along a frontier.'
WHERE Tag = 'TXT_KEY_BUILDING_WALLS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (
Tag, Text)
SELECT 'TXT_KEY_BUILDING_WALLS_HELP', 'Increases the City''s [ICON_RANGE_STRENGTH] Ranged Strike Range by 1. Military Units Supplied by this City''s population increased by 10%.[NEWLINE][NEWLINE]Empire [ICON_HAPPINESS_3] Needs Modifier is reduced by 5% in this City.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Castle is a Medieval-era building which increases Defensive Strength and Hit Points. Increases Military Units supplied by this City''s population by 10%, and increases Production for all nearby Quarries by 1. Also helps with managing the Empire Needs Modifier in this City. The city must possess Walls before the Castle can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_CASTLE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_PRODUCTION] Production for Quarries worked by this City. Military Units Supplied by this City''s population increased by 10%. Contains 1 slot for a [ICON_GREAT_WORK] Great Work of Art or Artifact.[NEWLINE][NEWLINE]Empire [ICON_HAPPINESS_3] Needs Modifier is reduced by 5% in this City.'
WHERE Tag = 'TXT_KEY_BUILDING_CASTLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Arsenal is an Industrial-era military building that increases Defense Strength and Hit Points, making the city more difficult to capture. Increases the City''s [ICON_RANGE_STRENGTH] Ranged Strike Range by 1, and also allows the City to Ranged Strike indirectly, ignoring Line of Sight. Increases Military Units supplied by this City''s population by 15%. Also helps with managing the Empire Needs Modifier in this City. The city must possess a Castle before it can construct an Arsenal.'
WHERE Tag = 'TXT_KEY_BUILDING_ARSENAL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Military Units Supplied by this City''s population increased by 15%. Increases the City''s [ICON_RANGE_STRENGTH] Ranged Strike Range by 1, and allows [COLOR_POSITIVE_TEXT]Indirect Fire[ENDCOLOR].[NEWLINE][NEWLINE]Garrisoned Units receive an additional 10 Health when healing in this City. Foreign [ICON_SPY] Spies cannot Steal [ICON_RESEARCH] Science from this City.[NEWLINE][NEWLINE]Empire [ICON_HAPPINESS_3] Needs Modifier is reduced by 5% in this City.'
WHERE Tag = 'TXT_KEY_BUILDING_ARSENAL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+25% [ICON_PRODUCTION] Production of Air Units, +15 [ICON_STRENGTH] Damage to Air Units during Air Strikes on City. Increases the City''s [ICON_RANGE_STRENGTH] Ranged Strike Damage by 10%. Military Units Supplied by this City''s population increased by 25%, and garrisoned Units receive an additional 10 Health when healing in this City.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD]/[ICON_PRODUCTION] Distress. Empire [ICON_HAPPINESS_3] Needs Modifier is reduced by 10% in this City.'
WHERE Tag = 'TXT_KEY_BUILDING_MILITARY_BASE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Military Base is a late-game building which increases Defensive Strength and Hit Points, and improves defense against air units. The city must possess an Arsenal before a Military Base may be constructed. Garrisoned units receive an additional 10 Health when healing in this city. Increases Military Units supplied by this City''s population by 25%. Also helps with managing the Empire Needs Modifier in this City. City must have an Arsenal.'
WHERE Tag = 'TXT_KEY_BUILDING_MILITARY_BASE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '50% chance to detonate nuclear weapons [COLOR_POSITIVE_TEXT]without damaging the city or surrounding tiles[ENDCOLOR]. Reduces population loss from nuclear attack by 75%, increases Defensive Strength by 5, and [ICON_HAPPINESS_1] Happiness by 1. +10 [ICON_STRENGTH] Damage to Air Units during Air Strikes on City.'
WHERE Tag = 'TXT_KEY_BUILDING_BOMB_SHELTER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Strategic Defense Systems have a 50% chance to detonate nuclear weapons without damaging the city or surrounding tiles, reduce population loss from a nuclear attack on this city by 75% if a missile does strike. Duck and Cover!'
WHERE Tag = 'TXT_KEY_BUILDING_BOMB_SHELTER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Strategic Defense System'
WHERE Tag = 'TXT_KEY_BUILDING_BOMB_SHELTER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Strategic Defense Initiative (SDI) was a proposed missile defense system intended to protect the United States from attack by ballistic strategic nuclear weapons (intercontinental ballistic missiles and submarine-launched ballistic missiles). The concept was first announced publicly by President Ronald Reagan on 23 March 1983. Reagan was a vocal critic of the doctrine of mutual assured destruction (MAD), which he described as a ''suicide pact,'' and he called upon the scientists and engineers of the United States to develop a system that would render nuclear weapons obsolete.'
WHERE Tag = 'TXT_KEY_CIV5_BUILDINGS_BOMB_SHELTER_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_PRODUCTION] Production for every 4 [ICON_CITIZEN] Citizens in the City. Manufactories worked by the City gain +2 [ICON_PRODUCTION] Production, and all owned Factories gain +2 [ICON_PRODUCTION] Production. [NEWLINE][NEWLINE]1 Specialist in this City no longer produces [ICON_HAPPINESS_3] Unhappiness from Urbanization.[NEWLINE][NEWLINE]Requires 1 [ICON_RES_COAL] Coal. '
WHERE Tag = 'TXT_KEY_BUILDING_FACTORY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+50% [ICON_PRODUCTION] Production when building Spaceship Parts.[NEWLINE][NEWLINE]Requires 1 [ICON_RES_ALUMINUM] Aluminum.'
WHERE Tag = 'TXT_KEY_BUILDING_SPACESHIP_FACTORY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Spaceship Factory increases the speed at which a city constructs spaceship parts, and greatly boosts Science in the City. The Spaceship Factory requires one [ICON_RES_ALUMINUM] Aluminum resource, and the city must possess a Factory before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_SPACESHIP_FACTORY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '25% of the [ICON_CULTURE] Culture from World Wonders, Natural Wonders, and Tiles is added to the [ICON_TOURISM] Tourism output of the city. [ICON_TOURISM] Tourism output from Great Works +25%.'
WHERE Tag = 'TXT_KEY_BUILDING_HOTEL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows [COLOR_POSITIVE_TEXT]Airlifts[ENDCOLOR] to or from this City. [COLOR_POSITIVE_TEXT]Increases Air Unit Capacity of the City from 2 to 6.[ENDCOLOR] +10 [ICON_STRENGTH] Damage to Air Units during Air Strikes on City. 25% of the [ICON_CULTURE] Culture from World Wonders, Natural Wonders, and Tiles is added to the [ICON_TOURISM] Tourism output of the city. [ICON_TOURISM] Tourism output from Great Works +25%.'
WHERE Tag = 'TXT_KEY_BUILDING_AIRPORT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+150 [ICON_CULTURE] Culture when completed, and starts 10 turns of "We Love the King Day" in the City.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom.[NEWLINE][NEWLINE]Nearby [ICON_RES_IVORY] Ivory: +3 [ICON_CULTURE] Culture.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Circus reduces Boredom in a city and improves the Culture output of [ICON_RES_IVORY] Ivory. It also starts 10 turns of "We Love the King Day" in the City. Build these to combat Unhappiness from Boredom, and gain quick bursts of Culture.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (
Tag, Text)
SELECT 'TXT_KEY_BUILDING_COLOSSEUM_HELP', '+2 [ICON_TOURISM] Tourism. Barracks, Forge, and Armory in this City gain +2 [ICON_PRODUCTION] Production. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Arena generates Tourism, reduces Boredom in a city, and grants additional Culture. Build these to combat Unhappiness from Boredom, to increase your Culture, and to improve the production of your military buildings.'
WHERE Tag = 'TXT_KEY_BUILDING_COLOSSEUM_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'An arena is an enclosed area, often circular or oval-shaped, designed to showcase theater, musical performances, or sporting events. The word derives from Latin harena, a particularly fine/smooth sand used to absorb blood in ancient arenas such as the Colosseum in Rome. It is composed of a large open space surrounded on most or all sides by tiered seating for spectators. The key feature of an arena is that the event space is the lowest point, allowing for maximum visibility. Arenas are usually designed to accommodate a large number of spectators.'
WHERE Tag = 'TXT_KEY_CIV5_BUILDINGS_COLISEUM_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Arena'
WHERE Tag = 'TXT_KEY_BUILDING_COLOSSEUM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1000 [ICON_TOURISM] Tourism with all known Civilizations when completed. Nearby Jungle and Forest Tiles gain +2 [ICON_TOURISM] Tourism and +1 [ICON_CULTURE] Culture. When any [ICON_INTERNATIONAL_TRADE] Trade Route originating here and targeting another Civ is completed, receive a [ICON_TOURISM] Tourism boost with the Civ based on your recent [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism output.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom.'
WHERE Tag = 'TXT_KEY_BUILDING_THEATRE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Zoo reduces Boredom in a city, produces additional Culture, and boosts the Gold value of nearby Jungle and Forest tiles. Generates a large sum of Tourism with all known Civilizations when completed.'
WHERE Tag = 'TXT_KEY_BUILDING_THEATRE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Globe Theatre[ENDCOLOR], a building which increases [ICON_TOURISM] Tourism. Also enables the founding of the World Congress.'
WHERE Tag = 'TXT_KEY_TECH_PRINTING_PRESS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Build the Stadium if you are going for a [COLOR_POSITIVE_TEXT]Culture Victory[ENDCOLOR], or you are having problems with happiness from Boredom in your empire. Provides a huge sum of Golden Age Points when completed.'
WHERE Tag = 'TXT_KEY_BUILDING_STADIUM_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1000 [ICON_GOLDEN_AGE] Golden Age Points when completed, and +1 [ICON_GOLDEN_AGE] Golden Age Point for every 4 [ICON_CITIZEN] Citizens in the City.[NEWLINE][NEWLINE]25% of the [ICON_CULTURE] Culture from World Wonders, Natural Wonders, and Tiles is added to the [ICON_TOURISM] Tourism output of the City. [ICON_TOURISM] Tourism output from [ICON_GREAT_WORK] Great Works +25%.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom.'
WHERE Tag = 'TXT_KEY_BUILDING_STADIUM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reduces Enemy [ICON_SPY] Spy Stealing rate by 25%, and prevents the theft of [ICON_GOLD] Gold through Advanced Actions. When an Enemy [ICON_SPY] Spy is killed in this City, gain [ICON_RESEARCH] Science and [ICON_CULTURE] Culture, scaling with Era and the Level of the [ICON_SPY] Spy killed.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD]/[ICON_PRODUCTION] Distress.'
WHERE Tag = 'TXT_KEY_BUILDING_POLICE_STATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reduces Enemy [ICON_SPY] Spy Stealing rate by 25%, and prevents the Disruption of Building construction by Spy Advanced Actions.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD]/[ICON_PRODUCTION] Distress. '
WHERE Tag = 'TXT_KEY_BUILDING_CONSTABLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Provides 5 [ICON_GREAT_PEOPLE] Great Person Points (GPP) towards a [ICON_GREAT_MUSICIAN] Great Musician. Add up to 2 Specialists to this Building to gain [ICON_CULTURE] Culture and increase the rate of [ICON_GREAT_MUSICIAN] Great Musician acquisition.[NEWLINE][NEWLINE]1 Specialist in this City no longer produces [ICON_HAPPINESS_3] Unhappiness from Urbanization.[NEWLINE][NEWLINE]Maximum of 3 of these Buildings in your Empire.'
WHERE Tag = 'TXT_KEY_BUILDING_MUSICIANS_GUILD_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Provides 4 [ICON_GREAT_PEOPLE] Great Person Points (GPP) towards a [ICON_GREAT_ARTIST] Great Artist. Add up to 2 Specialists to this Building to gain [ICON_CULTURE] Culture and increase the rate of [ICON_GREAT_ARTIST] Great Artist acquisition.[NEWLINE][NEWLINE]1 Specialist in this City no longer produces [ICON_HAPPINESS_3] Unhappiness from Urbanization.[NEWLINE][NEWLINE]Maximum of 3 of these Buildings in your Empire.'
WHERE Tag = 'TXT_KEY_BUILDING_ARTISTS_GUILD_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Provides 3 [ICON_GREAT_PEOPLE] Great Person Points (GPP) towards a [ICON_GREAT_WRITER] Great Writer. Add up to 2 Specialists to this Building to gain [ICON_CULTURE] Culture and increase the rate of [ICON_GREAT_WRITER] Great Writer acquisition.[NEWLINE][NEWLINE]1 Specialist in this City no longer produces [ICON_HAPPINESS_3] Unhappiness from Urbanization.[NEWLINE][NEWLINE]Maximum of 3 of these Buildings in your Empire.'
WHERE Tag = 'TXT_KEY_BUILDING_WRITERS_GUILD_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'National Monument'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_EPIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A National monument is a monument constructed in order to commemorate something of national importance such as a war or the founding of the country. The term may also refer to a specific monument status, such as a national heritage site, which most national monuments are by reason of their cultural importance rather than age. The National monument aims to represent the nation, and serve as a focus for national identity.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_EPIC_PEDIA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'School of Philosophy'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_COLLEGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Philosophy is the study of general and fundamental problems, such as those connected with reality, existence, knowledge, values, reason, mind, and language. Philosophy is organized into schools of thought and distinguished from other ways of addressing such problems by its critical, generally systematic approach and its reliance on rational argument. In more casual speech, by extension, philosophy can refer to the most basic beliefs, concepts, and attitudes of an individual or group. The word philosophy comes from the Ancient Greek philosophia, which literally means "love of wisdom". The introduction of the terms philosopher and philosophy has been ascribed to the Greek thinker Pythagoras.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_COLLEGE_PEDIA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'This National Wonder gives all land units built in this city (past and future) the "Morale" promotion, increasing their combat strength by +10%. +1 [ICON_PRODUCTION] Production in the City for every 5 [ICON_CITIZEN] Citizens. Increases the Military Unit Supply Cap from Population in the City by 10%. Also creates a free Great Writer upon completion. The Heroic Epic can be constructed when a city has a barracks.'
WHERE Tag = 'TXT_KEY_BUILDING_HEROIC_EPIC_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'This National Wonder increases the [ICON_GREAT_PEOPLE] Great People generation of a city by 25%. Receive [ICON_CULTURE] Culture when a [ICON_CITIZEN] Citizen is born in the City, and [ICON_GOLDEN_AGE] Golden Age Points whenever you unlock a policy. It also provides +1 [ICON_CULTURE] Culture. A city must have a monument before it can construct a National Monument.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_EPIC_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'This National Wonder provides +2 [ICON_HAPPINESS_1] Happiness, +1 [ICON_CULTURE] Culture, and reduces [ICON_HAPPINESS_3] Boredom, and generates +10% [ICON_CULTURE] Culture and [ICON_GOLD] Gold during "We Love the King Day" in the City where it is built. A city must have an Arena before it can construct a Circus Maximus.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_MAXIMUS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The East India Company increases the amount of [ICON_GOLD] Gold a city generates and reduces [ICON_HAPPINESS_3] Poverty. Resource Diversity Modifiers for Trade Routes from this City increase by 25% if positive, and decrease by 25% if negative. You also receive a free copy of all Luxury Resources around the City. Owned Spies are much more likely to steal [ICON_GOLD] Gold via Advanced Actions.[NEWLINE][NEWLINE]Trade routes other players make to a city with an East India Company will generate an extra 4 [ICON_GOLD] Gold for the city owner and the trade route owner gains an additional 2 [ICON_GOLD] Gold for the trade route.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_TREASURY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The School of Philosophy National Wonder produces [ICON_RESEARCH] Science and [ICON_CULTURE] Culture, especially during [ICON_GOLDEN_AGE] Golden Ages. A city must have a library before it can construct a School of Philosophy.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_COLLEGE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Ironworks National Wonder provides 2 [ICON_RES_IRON] Iron, increases [ICON_PRODUCTION] Production in a city by 10, and generates [ICON_RESEARCH] Science every time you construct a building. A city must have a Forge before it can construct an Ironworks.'
WHERE Tag = 'TXT_KEY_BUILDING_IRONWORKS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Oxford University National Wonder provides +1 [ICON_CULTURE] Culture, and reduces [ICON_HAPPINESS_3] Illiteracy. +50 [ICON_CULTURE] Culture every time you research a Technology. The city must have a university before it can construct Oxford University.'
WHERE Tag = 'TXT_KEY_BUILDING_OXFORD_UNIVERSITY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'This National Wonder produces +1 [ICON_CULTURE] Culture for every 4 [ICON_CITIZEN] Citizens in the City, and +10% [ICON_CULTURE] Culture for the City. It cannot be constructed unless the city has an Opera House.'
WHERE Tag = 'TXT_KEY_BUILDING_HERMITAGE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'An important defensive National Wonder for a technology-driven civilization. Reduces [ICON_HAPPINESS_3] Distress in all Cities. The National Intelligence Agency provides an additional spy, improves chance of Great Person assassination via Advanced Actions for spies, levels up all your existing spies, and provides a 15% reduction in enemy spy effectiveness. Empires with a lot of offensive spies will benefit greatly from this building. A city must have a Police Station before it can construct the National Intelligence Agency.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'An important building for a civilization trying to spread their religion world-wide from an empire with few, populous cities. A city must have a Temple before it can construct the Grand Temple.'
WHERE Tag = 'TXT_KEY_BUILDING_GRAND_TEMPLE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'With its massive boost to [ICON_TOURISM] Tourism, the National Visitor Center is an excellent choice for civilizations that are trying to achieve a Culture victory, or attempting to increase your ideological impact on other civilizations. Build it in your top [ICON_TOURISM] Tourism city that has a Hotel.'
WHERE Tag = 'TXT_KEY_BUILDING_TOURIST_CENTER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

-- Help Text

UPDATE Language_en_US
SET Text = 'Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_WRITER] Great Writer in the City in which it is built. All Land Units created by this City (past or future) receive the [COLOR_POSITIVE_TEXT]Morale[ENDCOLOR] Promotion, which gives +10% [ICON_STRENGTH] Combat Strength. Increases the Military Unit Supply Cap from Population in the City by 10%. Contains 1 slot for a [ICON_GREAT_WORK] Great Work of Writing.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_HEROIC_EPIC_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+25% [ICON_GREAT_PEOPLE] Great People generation in this City. Receive 15 [ICON_CULTURE] Culture when a [ICON_CITIZEN] Citizen is born in the City, and 50 [ICON_GOLDEN_AGE] Golden Age Points whenever you unlock a Policy, scaling with Era. Contains 1 slot for a [ICON_GREAT_WORK] Great Work of Art or an Artifact.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_EPIC_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

-- Removed +2 Happiness because already present
UPDATE Language_en_US
SET Text = '+10% [ICON_CULTURE] Culture and [ICON_GOLD] Gold during "We Love the King Day" in the City in which it is built. All Arenas in the Empire gain +2 [ICON_GOLD] Gold. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_MAXIMUS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receive an additional copy of all Luxury Resources around this City. Incoming [ICON_INTERNATIONAL_TRADE] Trade Routes generate +4 [ICON_GOLD] Gold for the City, and +2 [ICON_GOLD] Gold for [ICON_INTERNATIONAL_TRADE] Trade Route owner. -10% [ICON_HAPPINESS_3] Unhappiness Needs Modifier for [ICON_GOLD] Poverty in all Cities.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_TREASURY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+20% [ICON_RESEARCH] Science in the City during [ICON_GOLDEN_AGE] Golden Ages. 1 Specialist in this City no longer produces [ICON_HAPPINESS_3] Unhappiness from Urbanization. Contains 1 slot for a [ICON_GREAT_WORK] Great Work of Writing.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_COLLEGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Provides 2 [ICON_RES_IRON] Iron. +25 [ICON_RESEARCH] Science when you construct a Building in this City. Bonus scales with Era.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_IRONWORKS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A [ICON_GREAT_SCIENTIST] Great Scientist appears near the City. +50 [ICON_CULTURE] Culture every time you research a Technology, scaling with Era. Contains 2 slots for [ICON_GREAT_WORK] Great Works of Writing. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_RESEARCH] Illiteracy.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.[NEWLINE][NEWLINE]+4 [ICON_RESEARCH] Science if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_BUILDING_OXFORD_UNIVERSITY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture for every 4 [ICON_CITIZEN] Citizens in the City. +10% [ICON_CULTURE] Culture in this City. Contains 3 slots for [ICON_GREAT_WORK] Great Works of Art. Reduces [ICON_HAPPINESS_3] Unhappiness Needs Modifier for [ICON_CULTURE] Boredom by 10% in all Cities.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.[NEWLINE][NEWLINE]+4 [ICON_GOLD] Gold and [ICON_CULTURE] Culture if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_BUILDING_HERMITAGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Slows Enemy [ICON_SPY] Spy effectiveness by 15%, provides 1 or more [COLOR_POSITIVE_TEXT]Additional[ENDCOLOR] [ICON_SPY] Spies (based on number of [ICON_CITY_STATE] City-States in game), and levels up all existing [ICON_SPY] Spies. Owned [ICON_SPY] Spies are much more likely to assassinate [ICON_GREAT_PEOPLE] Great People via Advanced Actions. Reduces [ICON_HAPPINESS_3] Unhappiness Needs Modifier for [ICON_FOOD]/[ICON_PRODUCTION] Distress by 10% in all Cities.[NEWLINE][NEWLINE]+100 [ICON_RESEARCH] Science and [ICON_GOLD] Gold when you Steal a [ICON_GREAT_WORK] Great Work or [ICON_RESEARCH] Technology, complete a [ICON_CITY_STATE] City-State Coup or Election Rigging, or kill a foreign [ICON_SPY] Spy in this City (+25 [ICON_GOLD]/[ICON_RESEARCH] for Advanced Actions), all scaling with Era.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'All Temples gain +2 [ICON_PEACE] Faith and [ICON_CULTURE] Culture. -1 [ICON_HAPPINESS_3] Unhappiness from Religious Unrest, and -10% [ICON_HAPPINESS_3] Unhappiness Needs Modifier for Religious Unrest in all Cities.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_GRAND_TEMPLE_HELP';

UPDATE Language_en_US
SET Text = '25% of the [ICON_CULTURE] Culture from World Wonders, Natural Wonders, and Tiles is added to the [ICON_TOURISM] Tourism output of all Cities, and an additional 25% to this City. [ICON_TOURISM] Tourism output from [ICON_GREAT_WORK] Great Works in all Cities increased by +25%, and an additional 25% in this City.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_TOURIST_CENTER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Can only be constructed in a coastal City. Gains 1 [COLOR_POSITIVE_TEXT]Additional[ENDCOLOR] [ICON_INTERNATIONAL_TRADE] Trade Route slot and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Cargo Ship in the City in which it is built. Incoming [ICON_INTERNATIONAL_TRADE] Trade Routes generate +2 [ICON_GOLD] Gold for the City, and +1 [ICON_GOLD] Gold for [ICON_INTERNATIONAL_TRADE] Trade Route owner.'
WHERE Tag = 'TXT_KEY_WONDER_COLOSSUS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Barracks in the City in which it is built. All Units gain +15% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR]. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD]/[ICON_PRODUCTION] Distress in this City.'
WHERE Tag = 'TXT_KEY_WONDER_STATUE_ZEUS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
SELECT 'TXT_KEY_BUILDING_STONEHENGE_HELP', 'Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Council and 50 [ICON_PEACE] Faith in the City in which it is built.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Stone Works in the City in which it is built. "We Love the King Day" begins in the City. During "We Love the King Day", City [ICON_PRODUCTION] Production, [ICON_RESEARCH] Science, and [ICON_GOLD] Gold increase by +10%.'
WHERE Tag = 'TXT_KEY_WONDER_MAUSOLEUM_HALICARNASSUS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Herbalist in the City in which it is built. +10% [ICON_FOOD] Food in all Cities. +25% [ICON_PRODUCTION] Production when building Ranged Units in this City. 2 Specialists in this City no longer produce [ICON_HAPPINESS_3] Unhappiness from Urbanization.'
WHERE Tag = 'TXT_KEY_WONDER_TEMPLE_ARTEMIS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Can only be built in a City on or next to Desert. Gains 1 [COLOR_POSITIVE_TEXT]Additional[ENDCOLOR] [ICON_INTERNATIONAL_TRADE] Trade Route slot and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Caravan appears in the City in which it is built. +1 [ICON_GOLD] Gold on all Desert tiles worked by this City. +6 [ICON_CULTURE] Culture once [COLOR_CYAN]Archaeology[ENDCOLOR] is discovered.'
WHERE Tag = 'TXT_KEY_WONDER_PETRA_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Technology and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Library in the City in which it is built. Contains 2 slots for [ICON_GREAT_WORK] Great Works of Writing. +3 [ICON_RESEARCH] Science if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_GREAT_LIBRARY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Can only be constructed in a coastal City. Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Lighthouse in the City in which it is built. All Military Naval Units receive +1 [ICON_MOVES] Movement and +1 Sight.'
WHERE Tag = 'TXT_KEY_WONDER_GREAT_LIGHTHOUSE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Garden in the City in which it is built.'
WHERE Tag = 'TXT_KEY_WONDER_HANGING_GARDEN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Tile improvement construction speed increased by 25%. +20 [ICON_CULTURE] Culture when you destroy an enemy Unit in battle, scaling with Era. Increases the Military Unit Supply Cap by 5.'
WHERE Tag = 'TXT_KEY_WONDER_TERRA_COTTA_ARMY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Increases Military Units Supplied by this City''s population by 10%. Contains a prebuilt [ICON_GREAT_WORK] Great Work of Art in one of the [ICON_GREAT_WORK] Great Work slots. All owned Amphitheaters gain +1 [ICON_CULTURE] Culture and +1 [ICON_RESEARCH] Science. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom in this City. +3 [ICON_CULTURE] Culture if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_PARTHENON_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Temple in the City in which it is built. Instantly receive 400 [ICON_CULTURE] Culture and [ICON_RESEARCH] Science. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_RESEARCH] Illiteracy in the City.'
WHERE Tag = 'TXT_KEY_WONDER_ORACLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Mandir in the City in which it is built. [ICON_CULTURE] Culture and [ICON_GOLD] Gold costs of acquiring new tiles reduced by 25% in every City.'
WHERE Tag = 'TXT_KEY_WONDER_ANGKOR_WAT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_GENERAL] Great General and [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Walls in the City in which it is built. Enemy Land Units expend all [ICON_MOVES] Movement when entering your territory. Increases the Military Unit Supply Cap by 3. These bonuses become obsolete upon discovering [COLOR_NEGATIVE_TEXT]Gunpowder[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_GREAT_WALL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires completion of [COLOR_MAGENTA]Authority[ENDCOLOR] Branch. Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Castle in the City in which it is built. All Mounted Melee units (new and previously created by this City) receive the [COLOR_POSITIVE_TEXT]Jinete[ENDCOLOR] Promotion, improving [ICON_STRENGTH] Combat Strength when attacking and providing a chance to withdraw from combat. Boosts City [ICON_CULTURE] Culture output by 10%.'
WHERE Tag = 'TXT_KEY_WONDER_ALHAMBRA_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- University of Sankore
UPDATE Language_en_US
SET Text = 'Requires completion of [COLOR_MAGENTA]Tradition[ENDCOLOR] Branch. Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Mosque in the City in which it is built. Also, when you expend a [ICON_GREAT_PEOPLE] Great Person, receive 50 [ICON_RESEARCH] Science, bonus scales with Era.'
WHERE Tag = 'TXT_KEY_WONDER_MOSQUE_OF_DJENNE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'University of Sankore'
WHERE Tag = 'TXT_KEY_BUILDING_MOSQUE_OF_DJENNE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'As the center of an Islamic scholarly community, the University of Sankore was very different in organization from the universities of medieval Europe. It had no central administration other than the Emperor. It had no student registers but kept copies of its student publishings. It was composed of several entirely independent schools or colleges, each run by a single master or imam. Students associated themselves with a single teacher, and courses took place in the open courtyard of the mosque or at private residences.'
WHERE Tag = 'TXT_KEY_WONDER_MOSQUE_OF_DJENNE_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_PROPHET] Great Prophet and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Church in the City in which it is built. Boosts starting Religious Strength of all [ICON_MISSIONARY] Missionaries by 25%.'
WHERE Tag = 'TXT_KEY_WONDER_HAGIA_SOPHIA_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Can only be built in a Holy City. Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Orders building in the City in which it is built. Enables you to select a [COLOR_POSITIVE_TEXT]Reformation Belief[ENDCOLOR] at 5% below its normal Global [ICON_RELIGION] Follower requirement (20% on Standard).'
WHERE Tag = 'TXT_KEY_WONDER_KREMLIN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Cathedral of St. Basil'
WHERE Tag = 'TXT_KEY_BUILDING_KREMLIN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Cathedral of Vasily the Blessed, commonly known as the Cathedral of Saint Basil, is a former church in Red Square in Moscow, Russia. The building, now a museum, is officially known as the Cathedral of the Intercession of the Most Holy Theotokos on the Moat or Pokrovsky Cathedral. it is built from 155561 on orders from Ivan the Terrible and commemorates the capture of Kazan and Astrakhan. A world famous landmark, it has been the hub of growth since the 14th century and was the tallest building in the City until the completion of the Ivan the Great Bell Tower in 1600.'
WHERE Tag = 'TXT_KEY_WONDER_KREMLIN_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Can only be built in a Holy City. Receive 2 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_MISSIONARY] Missionaries and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Stupa in the City in which it is built. All new and existing [ICON_MISSIONARY] Missionaries and Prophets gain 1 [COLOR_POSITIVE_TEXT]Additional[ENDCOLOR] Religous Spread action.'
WHERE Tag = 'TXT_KEY_WONDER_BOROBUDUR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Castle in the City in which it is built. +15% [ICON_STRENGTH] Combat Strength for Units fighting in [COLOR_POSITIVE_TEXT]Friendly Territory[ENDCOLOR], and +10% [ICON_STRENGTH] [ICON_STRENGTH] Combat Strength for all Cities. Increases the Military Unit Supply Cap by 5.'
WHERE Tag = 'TXT_KEY_WONDER_HIMEJI_CASTLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Can only be built in a City within 2 tiles of a Mountain that is inside your territory. +15% [ICON_GOLD] Gold from [ICON_CONNECTED] City Connections. City gains +1 [ICON_FOOD] Food, [ICON_PRODUCTION] Production, [ICON_CULTURE] Culture, and [ICON_PEACE] Faith for every Mountain within 3 tiles of the City.'
WHERE Tag = 'TXT_KEY_WONDER_MACHU_PICHU_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
SELECT 'TXT_KEY_BUILDING_NOTRE_DAME_HELP', 'Receive a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Cathedral in the City in which it is built. The Empire enters a [ICON_GOLDEN_AGE] Golden Age. Contains 2 slots for [ICON_GREAT_WORK] Great Works of Art or Artifacts. +3 [ICON_PEACE] Faith and [ICON_GOLDEN_AGE] Golden Age Points if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires completion of [COLOR_MAGENTA]Progress[ENDCOLOR] Branch. Cost of [ICON_GOLD] Gold purchasing in all Cities reduced by 15%. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty in this City.'
WHERE Tag = 'TXT_KEY_WONDER_FORBIDDEN_PALACE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_WRITER] Great Writer. Writer, Artist, and Musician Specialists in all Cities gain +2 [ICON_GOLDEN_AGE] Golden Age Points. Contains 2 slots for [ICON_GREAT_WORK] Great Works of Writing. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom in this City. 1 Specialist in every City no longer generates [ICON_HAPPINESS_3] Unhappiness from Urbanization. +10 [ICON_GOLD] Gold if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_GLOBE_THEATER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_ARTIST] Great Artist. +1 [ICON_CULTURE] Culture from [ICON_GREAT_WORK] Great Works in all Cities. Contains 3 slots for [ICON_GREAT_WORK] Great Works of Art. 1 Specialist in every City no longer generates [ICON_HAPPINESS_3] Unhappiness from Urbanization. +10 [ICON_CULTURE] Culture if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_UFFIZI_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- Here is in fact the TAJ MAHAL, in the original files is wrong defined 
UPDATE Language_en_US
SET Text = 'The Empire enters a [ICON_GOLDEN_AGE] Golden Age. +1 [ICON_GOLDEN_AGE] Golden Age Points per turn for every 2 [ICON_CITIZEN] Citizens in the City. +3 [ICON_RESEARCH] Science, [ICON_CULTURE] Culture, and [ICON_PEACE] Faith in the City for every [ICON_RELIGION] Religion present.'
WHERE Tag = 'TXT_KEY_WONDER_PYRAMIDS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- Here is in fact the CHICHEN ITZA, in the original files is wrong defined 
UPDATE Language_en_US
SET Text = 'Length of [ICON_GOLDEN_AGE] Golden Ages increased by 50%. Empire [ICON_HAPPINESS_3] Needs Modifier is reduced by 10% in all Cities.'
WHERE Tag = 'TXT_KEY_WONDER_TAJ_MAHAL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- Here is in fact the PYRAMIDS, in the original files is wrong defined 
UPDATE Language_en_US
SET Text = 'A Settler appears near the City. +50 [ICON_GOLDEN_AGE] Golden Age Points when you expend a [ICON_GREAT_PEOPLE] Great Person, scaling with Era.'
WHERE Tag = 'TXT_KEY_WONDER_CHICHEN_ITZA_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+10% [ICON_CULTURE] Culture in all Cities. Contains 2 slots for [ICON_GREAT_WORK] Great Works of Art. +3 [ICON_PEACE] Faith and [ICON_CULTURE] Culture if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_SISTINE_CHAPEL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires completion of [COLOR_MAGENTA]Fealty[ENDCOLOR] Branch. Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_ENGINEER] Great Engineer and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Arsenal. City [ICON_RANGE_STRENGTH] Ranged Strike Strength and Range increased by 10% and 1, respectively. Greatly increases [ICON_STRENGTH] Strength and HP of the City and increases the Military Unit Supply Cap from population by 5% in all Cities.'
WHERE Tag = 'TXT_KEY_WONDER_RED_FORT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_PEOPLE] Great Person of your choice near the [ICON_CAPITAL] Capital. +25% generation of [ICON_GREAT_PEOPLE] Great People in the City, and +10% in all other Cities.'
WHERE Tag = 'TXT_KEY_WONDER_LEANING_TOWER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_SCIENTIST] Great Scientist. 50% more [ICON_RESEARCH] Science generated from Research Agreements. If Research Agreements are disabled, provides a +25% [ICON_RESEARCH] Science bonus in the City in which it is built. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_RESEARCH] Illiteracy in this City.'
WHERE Tag = 'TXT_KEY_WONDER_PORCELAIN_TOWER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_GENERAL] Great General. +15 XP for Units built in all Cities. Increases the Military Unit Supply Cap by 10. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty in this City.'
WHERE Tag = 'TXT_KEY_WONDER_BRANDENBURG_GATE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires completion of [COLOR_MAGENTA]Artistry[ENDCOLOR] Branch. Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_ARTIST] Great Artist, 2 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Archaeologists, and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Museum in the City in which it is built. Contains 4 slots for [ICON_GREAT_WORK] Great Works of Art. +15 [ICON_CULTURE] Culture if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_LOUVRE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[ICON_CULTURE] Culture cost of adopting new Policies reduced by 10%.'
WHERE Tag = 'TXT_KEY_WONDER_EIFFEL_TOWER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires completion of [COLOR_MAGENTA]Statecraft[ENDCOLOR] Branch. Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_MERCHANT] Great Merchant, and Grants 2 [COLOR_POSITIVE_TEXT]Additional[ENDCOLOR] [ICON_DIPLOMAT] Delegates in the World Congress for every 8 [ICON_CITY_STATE] City-States in the game. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty in this City.'
WHERE Tag = 'TXT_KEY_WONDER_BIG_BEN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Palace of Westminster'
WHERE Tag = 'TXT_KEY_BUILDING_BIG_BEN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Can only be built in a City within 2 tiles of a Mountain that is inside your territory. +1 [ICON_GOLD] Gold, [ICON_CULTURE] Culture, and [ICON_HAPPINESS_1] Happiness from every Castle.'
WHERE Tag = 'TXT_KEY_WONDER_NEUSCHWANSTEIN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires completion of [COLOR_MAGENTA]Industry[ENDCOLOR] Branch. Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_MUSICIAN] Great Musician. Contains 3 slots for [ICON_GREAT_WORK] Great Works of Music. +250 [ICON_CULTURE] Culture when you construct a building in this City, scaling with Era. +20 [ICON_GOLD] Gold if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_BROADWAY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires [COLOR_MAGENTA]Autocracy[ENDCOLOR] and can only be constructed in a coastal City. Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Social Policy. +1 [ICON_HAPPINESS_1] Happiness in [ICON_CAPITAL] Capital for every 2 Policies you have adopted. Enemy [ICON_SPY] Spies cannot disrupt World Wonder [ICON_PRODUCTION] Production in this City. Receive 25 [ICON_CULTURE] Culture when any owned Unit pillages a tile, scaling with Era.'
WHERE Tag = 'TXT_KEY_WONDER_PRORA_RESORT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- added line for Statue of Liberty
UPDATE Language_en_US
SET Text = 'Requires [COLOR_MAGENTA]Freedom[ENDCOLOR]. Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Social Policy. +1 [ICON_PRODUCTION] Production from Specialists in every City.'
WHERE Tag = 'TXT_KEY_WONDER_STATUE_OF_LIBERTY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- added line for International Space Station
UPDATE Language_en_US
SET Text = 'May only be built collaboratively through the World Congress. +1 [ICON_PRODUCTION] Production from Scientists, and +1 [ICON_RESEARCH] Science from Engineers. [ICON_GREAT_SCIENTIST] Great Scientists provide 33% more [ICON_RESEARCH] Science when used to discover new Technology.[NEWLINE][NEWLINE]+200 [ICON_PRODUCTION] in City where it is built when you unlock a new Technology, scaling with Era.'
WHERE Tag = 'TXT_KEY_BUILDING_INTERNATIONAL_SPACE_STATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires completion of [COLOR_MAGENTA]Imperialism[ENDCOLOR] Branch. Increases the Military Unit Supply Cap by 1, and the Air Unit Cap by 2 in every City. Receive 2 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Jet Fighters in the City when completed. +50% [ICON_PRODUCTION] Production of Air Units in the City, and Air Units created in the City gain +20 XP. Owned [ICON_SPY] Spies are much more likely to disrupt Unit [ICON_PRODUCTION] Production via Advanced Actions.'
WHERE Tag = 'TXT_KEY_WONDER_PENTAGON_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- here added and updated
UPDATE Language_en_US
SET Text = 'Receive 2 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_SCIENTIST] Great Scientists and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Spaceship Factory in the City in which it is built. +25% [ICON_PRODUCTION] Production when building Spaceship parts.'
WHERE Tag = 'TXT_KEY_WONDER_HUBBLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Broadcast Tower in the City in which it is built. +1 [ICON_CITIZEN] Population in each City and +1 [ICON_HAPPINESS_1] Happiness per City. All [ICON_GREAT_WORK] Great Works gain +2 [ICON_GOLD] Gold and +3 [ICON_TOURISM] Tourism. 50% of the [ICON_CULTURE] Culture from World Wonders, Natural Wonders, and tiles is added to the [ICON_TOURISM] Tourism output of the City. +50% [ICON_TOURISM] Tourism output from [ICON_GREAT_WORK] Great Works.'
WHERE Tag = 'TXT_KEY_WONDER_CN_TOWER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+25 [ICON_TOURISM] Tourism. Hotels provide +2 [ICON_GOLDEN_AGE] Golden Age Points, [ICON_CULTURE] Culture, and [ICON_TOURISM] Tourism.'
WHERE Tag = 'TXT_KEY_WONDER_CRISTO_REDENTOR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Can only be constructed in a coastal City. Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Social Policy and +25% [ICON_CULTURE] Culture in this City. Contains 2 slots for [ICON_GREAT_WORK] Great Works of Music. +15 [ICON_CULTURE] Culture if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_SYDNEY_OPERA_HOUSE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'All Research Labs gain +10 [ICON_RESEARCH] Science. 99.9% reduction in effectiveness of enemy [ICON_SPY] Spies in the City in which it is built. All other Cities in your Empire get a 25% reduction in enemy [ICON_SPY] Spy effectiveness. Negates the [ICON_TOURISM] Tourism bonus from the Technologies of other players.'
WHERE Tag = 'TXT_KEY_BUILDING_GREAT_FIREWALL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Contribute this city''s [ICON_PRODUCTION] Production towards the World''s Fair project. Cost goes up based on the number of players in the game, and the current Era.'
WHERE Tag = 'TXT_KEY_PROCESS_WORLD_FAIR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Contribute this city''s [ICON_PRODUCTION] Production towards the International Games project. Cost goes up based on the number of players in the game, and the current Era.'
WHERE Tag = 'TXT_KEY_PROCESS_WORLD_GAMES_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Contribute this city''s [ICON_PRODUCTION] Production towards the International Space Station project. Cost goes up based on the number of players in the game, and the current Era.'
WHERE Tag = 'TXT_KEY_PROCESS_INTERNATIONAL_SPACE_STATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Citizen Earth Protocol'
WHERE Tag = 'TXT_KEY_PROJECT_UTOPIA_PROJECT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- correctly realigned the first bullet and colored in green 'Cultural Victory'
UPDATE Language_en_US
SET Text = 'Construct the Citizen Earth Protocol to win a [COLOR_POSITIVE_TEXT]Cultural Victory[ENDCOLOR]![NEWLINE][NEWLINE]Requirements for Construction: [NEWLINE]   [ICON_BULLET] Must have an Ideology (with a [COLOR_POSITIVE_TEXT]Content[ENDCOLOR] Population).[NEWLINE]   [ICON_BULLET] Must be [ICON_TOURISM] [COLOR_POSITIVE_TEXT]Influential[ENDCOLOR] with all other Civilizations in the world.[NEWLINE]   [ICON_BULLET] Must have [COLOR_POSITIVE_TEXT]2[ENDCOLOR] Tier 3 Tenets in your current Ideology.'
WHERE Tag = 'TXT_KEY_PROJECT_UTOPIA_PROJECT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Global citizenship is idea of all persons having rights and civic responsibilities that come with being a member of the World, with whole-world philosophy and sensibilities, rather than as a citizen of a particular nation or place. The idea is that ones identity transcends geography or political borders and that responsibilities or rights are derived from membership in a broader class: ''humanity.'' This does not mean that such a person denounces or waives their nationality or other, more local identities, but such identities are given ''second place'' to their membership in a global community. Extended, the idea leads to questions about the state of global society in the age of globalization. In general usage, the term may have much the same meaning as ''world citizen'' or cosmopolitan, but it also has additional, specialized meanings in differing contexts. Various organizations, such as the World Service Authority, have advocated global citizenship.'
WHERE Tag = 'TXT_KEY_PROJECT_UTOPIA_PROJECT_PEDIA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'You must have an Ideology (with a [COLOR_POSITIVE_TEXT]Content[ENDCOLOR] population), two Tier 3 Tenets in this Ideology, and be [ICON_TOURISM] Influential with all other Civilizations in the world in order to construct this. Once you have completed these tasks, construct this project to win a Cultural Victory!'
WHERE Tag = 'TXT_KEY_PROJECT_UTOPIA_PROJECT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
