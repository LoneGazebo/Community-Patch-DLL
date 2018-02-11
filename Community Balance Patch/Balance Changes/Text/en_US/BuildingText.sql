
UPDATE Language_en_US
SET Text = '+1 [ICON_RESEARCH] Science for every 3 [ICON_CITIZEN] Citizens in the City. Contains 1 slot for a Great Work of Art.[NEWLINE][NEWLINE]When one of the following [COLOR_POSITIVE_TEXT]Historic Events[ENDCOLOR] is completed, receive a [ICON_TOURISM] Tourism boost with all known Civs based on your recent [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism output:[NEWLINE][ICON_BULLET] Earn a [ICON_GREAT_PEOPLE] Great Person[NEWLINE][ICON_BULLET] Build a [ICON_GOLDEN_AGE] World Wonder[NEWLINE][ICON_BULLET] Win a [ICON_WAR] War[NEWLINE][ICON_BULLET] Enter a new [ICON_RESEARCH] Era[NEWLINE][NEWLINE][ICON_CONNECTED] Connecting other Cities to the [ICON_CAPITAL] Capital by Road will produce additional [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_PALACE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '15% of [ICON_FOOD] Food is carried over after a new [ICON_CITIZEN] Citizen is born.[NEWLINE][NEWLINE]Allows [ICON_FOOD] Food to be moved from this city along trade routes inside your civilization.[NEWLINE]Nearby [ICON_RES_WHEAT] Wheat: +1 [ICON_FOOD] Food.[NEWLINE]Nearby [ICON_RES_BANANA] Bananas: +1 [ICON_FOOD] Food.[NEWLINE]Nearby [ICON_RES_DEER] Deer: +1 [ICON_FOOD] Food.[NEWLINE]Nearby [ICON_RES_BISON] Bison: +1 [ICON_FOOD] Food.'
WHERE Tag = 'TXT_KEY_BUILDING_GRANARY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_PRODUCTION] Production for every 4 [ICON_CITIZEN] Citizens in the City. [NEWLINE][NEWLINE]City must be built next to a River. Cannot be built if the City already has a Well.'
WHERE Tag = 'TXT_KEY_BUILDING_WATERMILL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Water Mill can only be constructed in a city located next to a river. The Water Mill increases the city''s [ICON_PRODUCTION] Production more efficiently (per Citizen in the City) than the Well.'
WHERE Tag = 'TXT_KEY_BUILDING_WATERMILL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '15% of [ICON_FOOD] Food is carried over after a new [ICON_CITIZEN] Citizen is born, and +25% of the [ICON_PRODUCTION] Production of the City is added to the City''s current [ICON_PRODUCTION] Production. +2 [ICON_FOOD] Food on Lakes and Oases worked by this city. Reduces [ICON_HAPPINESS_3] Poverty slightly.'
WHERE Tag = 'TXT_KEY_BUILDING_AQUEDUCT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '15% of [ICON_FOOD] Food is carried over after a new [ICON_CITIZEN] Citizen is born. Scientist, Merchant, and Engineer Specialists in the City produce +1 of their base Yield. Reduces [ICON_HAPPINESS_3] Poverty greatly.'
WHERE Tag = 'TXT_KEY_BUILDING_MEDICAL_LAB_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Medical Lab is a late-game building which decreases the amount of [ICON_FOOD] Food a city needs to increase in size by 15% and boosts the City''s Science production from specialists. The city needs to have a Hospital in order to construct the Medical Lab.'
WHERE Tag = 'TXT_KEY_BUILDING_MEDICAL_LAB_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+25% [ICON_GREAT_PEOPLE] Great People generation in this City.[NEWLINE]Nearby Oases: +2 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_CITRUS] Citrus: +1 [ICON_FOOD] Food, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COCOA] Cocoa: +1 [ICON_FOOD] Food, +1 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]Requires an Aqueduct in the City.'
WHERE Tag = 'TXT_KEY_BUILDING_GARDEN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Garden increases the speed at which [ICON_GREAT_PEOPLE] Great People are generated in the city by 25%, and buffs the food output of Oases. Gardens require an Aqueduct in the City in order to be built.'
WHERE Tag = 'TXT_KEY_BUILDING_GARDEN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
SELECT 'TXT_KEY_BUILDING_HOSPITAL_HELP', 'Reduces [ICON_HAPPINESS_3] Poverty. 5% of the City''s [ICON_FOOD] Food is converted into [ICON_RESEARCH] Science every turn. Units in this City heal 15 points per turn whether or not they take an action. Increases Military Units supplied by this City''s population by 10%.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
SELECT 'TXT_KEY_BUILDING_TEMPLE_HELP', 'Reduces [ICON_HAPPINESS_3] Religious Unrest and generates +25% Religious Pressure. Contains 1 slot for a Great Work of Music.[NEWLINE]Nearby [ICON_RES_INCENSE] Incense: +1 [ICON_CULTURE] Culture, +1 [ICON_GOLD] Gold.[NEWLINE] Nearby [ICON_RES_WINE] Wine: +1 [ICON_CULTURE] Culture, +1 [ICON_GOLD] Gold.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
SELECT 'TXT_KEY_BUILDING_MONUMENT_HELP', '[ICON_CULTURE] Culture costs of acquiring new tiles reduced by 25% in this city.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
SELECT 'TXT_KEY_BUILDING_AMPHITHEATER_HELP', '+33% [ICON_GREAT_WRITER] Great Writer Rate in the City, and all Writers'' Guilds produce +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_DYE] Dye: +1 [ICON_CULTURE] Culture, +1 [ICON_GOLD] Gold.[NEWLINE] Nearby [ICON_RES_SILK] Silk: +1 [ICON_CULTURE] Culture, +1 [ICON_GOLD] Gold.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = 'The Amphitheater increases the [ICON_CULTURE] Culture of a city and grants bonuses to nearby [ICON_RES_DYE] Dye and [ICON_RES_SILK] Silk. Also boosts the City''s Great Writer rate and the value of Writers'' Guilds. Contains 1 slot for a Great Work of Writing.'
WHERE Tag = 'TXT_KEY_BUILDING_AMPHITHEATER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
SELECT 'TXT_KEY_BUILDING_OPERA_HOUSE_HELP', '+5% [ICON_CULTURE] Culture in the City. +33% [ICON_GREAT_MUSICIAN] Great Musician Rate in the City, and all Musicians'' Guilds produce +1 [ICON_GOLD] Gold.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = 'The Opera House is a Renaissance-era building which increases the [ICON_CULTURE] Culture of a city. Also boosts the City''s Great Musician rate and the value of Musicians'' Guilds. Contains 1 slot for a Great Work of Music. Requires an Amphitheater in the city before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_OPERA_HOUSE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Museum is a mid-game building which increases [ICON_CULTURE] Culture output and reduces Illiteracy. Also boosts the City''s Great Artist rate and the value of Artists'' Guilds. Contains 2 slots for Great Works of Art.  Requires an Opera House in the city before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_MUSEUM_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
SELECT 'TXT_KEY_BUILDING_MUSEUM_HELP', 'Reduces [ICON_HAPPINESS_3] Illiteracy greatly. +1 [ICON_CULTURE] Culture for every 4 [ICON_CITIZEN] Citizens in the City. +33% [ICON_GREAT_ARTIST] Great Artist Rate in the City, and all Artists'' Guilds produce +1 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]+5 [ICON_CULTURE] Culture if Themed.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reduces [ICON_HAPPINESS_3] Boredom greatly. +1 [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism for every 5 [ICON_CITIZEN] Citizens in the City.[NEWLINE][NEWLINE]+15 [ICON_GOLD] Gold if Themed.'
WHERE Tag = 'TXT_KEY_BUILDING_BROADCAST_TOWER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A Broadcast Tower is a late-game building which increases [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism output of the city, and reduces Boredom. Requires a Museum in the city before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_BROADCAST_TOWER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+15 XP for all Units. Reduces [ICON_HAPPINESS_3] Crime slightly. Increases the Military Unit Supply Cap by 1.'
WHERE Tag = 'TXT_KEY_BUILDING_BARRACKS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = '+20 XP for all Units. Reduces [ICON_HAPPINESS_3] Crime.[NEWLINE][NEWLINE]City must have a Barracks. Increases the Military Unit Supply Cap by 1.'
WHERE Tag = 'TXT_KEY_BUILDING_ARMORY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reduces [ICON_HAPPINESS_3] Crime greatly. +25 XP for all Units.[NEWLINE][NEWLINE]City must have an Armory. Increases the Military Unit Supply Cap by 1.'
WHERE Tag = 'TXT_KEY_BUILDING_MILITARY_ACADEMY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
		
UPDATE Language_en_US
SET Text = '+33% [ICON_PRODUCTION] Production when building Mounted Units. Increases the Military Unit Supply Cap from Population in the City by 10%.[NEWLINE]Nearby [ICON_RES_HORSE] Horses: +2 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_SHEEP] Sheep: +2 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_COW] Cattle: +2 [ICON_PRODUCTION] Production.[NEWLINE][NEWLINE]City must have at least one of these resources improved with a Pasture.'
WHERE Tag = 'TXT_KEY_BUILDING_STABLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = 'Mines worked by this City gain +2 [ICON_PRODUCTION] Production.[NEWLINE][NEWLINE]Nearby [ICON_RES_IRON] Iron: +1 [ICON_PRODUCTION] Production, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COPPER] Copper: +2 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_FORGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Forge improves sources of [ICON_RES_IRON] Iron and [ICON_RES_COPPER] Copper nearby, and boosts the [ICON_PRODUCTION] Production of Mines.'
WHERE Tag = 'TXT_KEY_BUILDING_FORGE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reduces [ICON_HAPPINESS_3] Poverty. +1 [ICON_PRODUCTION] Production and [ICON_GOLD] Gold from all Forests worked by this City. Allows [ICON_PRODUCTION] Production to be moved from this city along trade routes inside your civilization.'
WHERE Tag = 'TXT_KEY_BUILDING_WORKSHOP_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = 'Allows [ICON_PRODUCTION] Production to be moved from this city along trade routes inside your civilization, and reduces Poverty.[NEWLINE]Nearby [ICON_RES_MARBLE] Marble: +1 [ICON_PRODUCTION] Production, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_STONE] Stone: +2 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_SALT] Salt: +2 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]City must have at least one of these resources improved.'
WHERE Tag = 'TXT_KEY_BUILDING_STONE_WORKS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = 'Stone Works can only be constructed in a city near an improved [ICON_RES_STONE] Stone, [ICON_RES_SALT] Salt, [ICON_RES_MARBLE] Marble, or [ICON_RES_URANIUM] Uranium resource. Stone Works increase [ICON_PRODUCTION] Production and allow [ICON_PRODUCTION] Production to be moved from this city along trade routes inside your civilization.'
WHERE Tag = 'TXT_KEY_BUILDING_STONE_WORKS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
		
UPDATE Language_en_US
SET Text = '+1 [ICON_FOOD] Food and +1 [ICON_GOLD] Gold from Coast and Ocean Tiles.[NEWLINE][NEWLINE]Forms a [ICON_CONNECTED] City Connection with other cities via water if they also have Lighthouses. City must be built on the coast.'
WHERE Tag = 'TXT_KEY_BUILDING_LIGHTHOUSE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
		
UPDATE Language_en_US
SET Text = 'The Lighthouse can only be constructed in a city next to a coastal tile. It increases the [ICON_FOOD] Food and [ICON_GOLD] Gold output of water tiles. Also allows for City Connections over Water (see Concepts for more details).'
WHERE Tag = 'TXT_KEY_BUILDING_LIGHTHOUSE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
		
UPDATE Language_en_US
SET Text = 'If a city is connected by a road and/or Lighthouse to your capital city (i.e. both cities have a Lighthouse), that city has a "trade route" with the capital. Each trade route is worth a certain amount of gold each turn, the amount determined by the size of the two cities.'
WHERE Tag = 'TXT_KEY_GOLD_TRADE_ROUTES_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires a Lighthouse in the City. +100 Hit Points in the City. When a [ICON_INTERNATIONAL_TRADE] Sea Trade Route originating here and targeting another Civ is completed, receive a [ICON_TOURISM] Tourism boost with the Civ based on your recent [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism output.[NEWLINE][NEWLINE]+1 [ICON_FOOD] Food from Coast and Ocean Tiles, and +1 [ICON_PRODUCTION] Production from Sea Resources worked by this City. Sea Trade Routes gain +50% Range and +2 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]+15% [ICON_PRODUCTION] Production of Naval units, and increases Military Units supplied by this City''s population by 10%. City must be built on the coast.'
WHERE Tag = 'TXT_KEY_BUILDING_HARBOR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Harbor is a Medieval-era building, requiring a Lighthouse. It improves the range and [ICON_GOLD] Gold yield of sea trade routes, and boosts the value of sea tiles and resources. The Harbor also increases the [ICON_PRODUCTION] Production of Naval units by 15%, and increases Military Units supplied by this City''s population by 10%.'
WHERE Tag = 'TXT_KEY_BUILDING_HARBOR_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires 1 [ICON_RES_COAL] Coal. +250 Hit Points in the City. +25% [ICON_PRODUCTION] Production in City. +1 [ICON_PRODUCTION] Production and +1 [ICON_GOLD] Gold from all Ocean and Coastal tiles. +2 [ICON_PRODUCTION] Production and [ICON_GOLD] Gold from Sea Resources worked by this City. Increases the Military Unit Supply Cap from Population in the City by 10%.[NEWLINE][NEWLINE]City must be built on the coast, and cannot have a Train Station in the City.'
WHERE Tag = 'TXT_KEY_BUILDING_SEAPORT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Provides a massive boost to City Production and Sea Resource yields during the Industrial Era. Increases the Military Unit Supply Cap from Population in the City by 10%. Requires a Harbor, and cannot be built in the same City as a Train Station.'
WHERE Tag = 'TXT_KEY_BUILDING_SEAPORT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
INSERT INTO Language_en_US (Tag, Text)
SELECT 'TXT_KEY_BUILDING_MARKET_HELP', '[ICON_INTERNATIONAL_TRADE] Trade Routes to this City generate +1 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]Nearby [ICON_RES_SPICES] Spices: +1 [ICON_FOOD] Food, +1 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_SUGAR] Sugar: +1 [ICON_FOOD] Food, +1 [ICON_GOLD] Gold.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Markets increase the amount of [ICON_GOLD] Gold a city generates, and improves nearby [ICON_RES_SPICES] Spices and [ICON_RES_SUGAR] Sugar. Trade routes other players make to a city with a Market will generate an extra 1 [ICON_GOLD] Gold for the city owner and the trade route owner gains an additional 1 [ICON_GOLD] Gold for the trade route.'
WHERE Tag = 'TXT_KEY_BUILDING_MARKET_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reduces [ICON_HAPPINESS_3] Poverty. When any [ICON_INTERNATIONAL_TRADE] Trade Route originating here and targeting another Civ is completed, receive a [ICON_TOURISM] Tourism boost with the Civ based on your recent [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism output.[NEWLINE][NEWLINE][ICON_INTERNATIONAL_TRADE] Trade Routes to this City generate +2 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]City must have a Market.'
WHERE Tag = 'TXT_KEY_BUILDING_MINT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Customs House boosts the Gold value of Trade Routes, and boosts the Tourism you generate from Trade Routes to foreign Civilizations. It also reduces Poverty. Build these buildings in all Cities if you wish to improve your Gold ouput as well as the cultural value of your Trade Routes.'
WHERE Tag = 'TXT_KEY_BUILDING_MINT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Customs House'
WHERE Tag = 'TXT_KEY_BUILDING_MINT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A custom house or customs house was a building housing the offices for the government officials who processed the paperwork for the import and export of goods into and out of a country. Customs officials also collected customs duty on imported goods. The custom house was typically located in a seaport or in a city on a major river with access to the ocean. These cities acted as a port of entry into a country. The government had officials at such locations to collect taxes and regulate commerce. Due to advances in electronic information systems, the increased volume of international trade and the introduction of air travel, the custom house is now often a historical anachronism. There are many examples of buildings around the world whose former use was as a custom house but that have since been converted for other use, such as museums or civic buildings.'
WHERE Tag = 'TXT_KEY_CIV5_BUILDINGS_MINT_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'When you spend [ICON_GOLD] Gold to purchase Units or invest in Buildings in this City, 15% of the cost is converted into [ICON_RESEARCH] Science. Caravansaries and Customs Houses (or Hanse) in the City gain +3 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]City must have a Market.[NEWLINE]Nearby [ICON_RES_GOLD] Gold: +3 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_SILVER] Silver: +1 [ICON_GOLD] Gold, +1 [ICON_PRODUCTION] Production, +1 [ICON_CULTURE] Culture.[NEWLINE]Nearby [ICON_RES_GEMS] Gems: +2 [ICON_CULTURE] Culture, +1 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_BANK_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Bank is a Renaissance-era building which increases the city''s output of [ICON_GOLD] Gold and boosts the value of investments in the City. A city must possess a Market or Bazaar before a Bank may be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_BANK_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'City must have a Bank. +1 [ICON_GOLD] Gold for every 5 [ICON_CITIZEN] Citizens in the City. Cost of [ICON_GOLD] Gold purchasing in this City reduced by 10%. +2 [ICON_GOLD] Gold on Towns worked by this City.'
WHERE Tag = 'TXT_KEY_BUILDING_STOCK_EXCHANGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'When a [ICON_INTERNATIONAL_TRADE] Land Trade Route originating here and targeting another Civ is completed, receive a [ICON_TOURISM] Tourism boost with the Civ based on your recent [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism output.[NEWLINE][NEWLINE]Land Trade Routes gain +50% Range and +3 [ICON_GOLD] Gold, Merchant Specialists in this City gain +1 [ICON_GOLD] Gold, and this City''s [ICON_CONNECTED] Connection to the [ICON_CAPITAL] Capital generates +10% [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_TRUFFLES] Truffles: +2 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COTTON] Cotton: +1 [ICON_PRODUCTION] Production, +1 [ICON_CULTURE] Culture.[NEWLINE]Nearby [ICON_RES_FUR] Furs: +1 [ICON_GOLD] Gold, +1 [ICON_PRODUCTION] Production.'
WHERE Tag = 'TXT_KEY_BUILDING_CARAVANSARY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reduces [ICON_HAPPINESS_3] Illiteracy slightly. Requires a Council in the City.'
WHERE Tag = 'TXT_KEY_BUILDING_LIBRARY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_RESEARCH] Science from Jungle tiles worked by the City. Gain 25% of the [ICON_RESEARCH] Science output of the City as an instant boost to your current Research when a [ICON_CITIZEN] Citizen is born in this City. Reduces [ICON_HAPPINESS_3] Illiteracy.[NEWLINE][NEWLINE]City must have a Library.'
WHERE Tag = 'TXT_KEY_BUILDING_UNIVERSITY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
    
UPDATE Language_en_US
SET Text = '+1 [ICON_RESEARCH] for every 4 [ICON_CITIZEN] Citizens in the City. Reduces [ICON_HAPPINESS_3] Illiteracy.[NEWLINE][NEWLINE]Allows Archaeologists to be built in this city. [NEWLINE][NEWLINE]City must have a University.'
WHERE Tag = 'TXT_KEY_BUILDING_PUBLIC_SCHOOL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+4 [ICON_RESEARCH] Science from [ICON_RES_ALUMINUM] Aluminum, [ICON_RES_URANIUM] Uranium, and Academies worked by this City. Reduces [ICON_HAPPINESS_3] Illiteracy greatly.[NEWLINE][NEWLINE]+4 [ICON_RESEARCH] Science from Hospitals, Factories, and Medical Labs, and [ICON_GREAT_SCIENTIST] Great Scientists are produced 33% more quickly in this City.[NEWLINE][NEWLINE]City must have a Public School.'
WHERE Tag = 'TXT_KEY_BUILDING_LABORATORY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'City gains +2 [ICON_RESEARCH] Science for every Mountain within 3 tiles of the City. Requires ''Scientific Revolution'' Policy.'
WHERE Tag = 'TXT_KEY_BUILDING_OBSERVATORY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = 'The Observatory increases [ICON_RESEARCH] Science output, especially for Cities near lots of Mountains. Requires ''Scientific Revolution'' Policy to unlock.'
WHERE Tag = 'TXT_KEY_BUILDING_OBSERVATORY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+15% [ICON_PRODUCTION] Production when constructing Buildings. Grocers and Granaries in the City produce +1 [ICON_FOOD] Food.'
WHERE Tag = 'TXT_KEY_BUILDING_WINDMILL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = 'The Windmill is a Renaissance-era building which increases the [ICON_PRODUCTION] Production output of a city when constructing buildings.'
WHERE Tag = 'TXT_KEY_BUILDING_WINDMILL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+2 [ICON_PRODUCTION] Production, [ICON_RESEARCH] Science, and [ICON_FOOD] Food on every tile next to a River. [NEWLINE][NEWLINE]City must be built next to a River. Requires [ICON_RES_ALUMINUM] Aluminum to be built.'
WHERE Tag = 'TXT_KEY_BUILDING_HYDRO_PLANT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = 'The Hydro Plant is a Modern-era building which can only be constructed in a city which is adjacent to a river. The Hydro Plant increases the [ICON_PRODUCTION] Production, [ICON_RESEARCH] Science, and [ICON_FOOD] Food output of every tile the city works which is also adjacent to the river.'
WHERE Tag = 'TXT_KEY_BUILDING_HYDRO_PLANT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Eliminates extra [ICON_HAPPINESS_4] Unhappiness from an [ICON_OCCUPIED] Occupied City. Cost increases based on the number of cities in your empire.'
WHERE Tag = 'TXT_KEY_BUILDING_COURTHOUSE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Walls increase a city Defense Strength by 10 and Hit Points by 50, making the city more difficult to capture. Increases Military Units supplied by this City''s population by 10%. Walls are quite useful for cities located along a frontier.'
WHERE Tag = 'TXT_KEY_BUILDING_WALLS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (
Tag, Text)
SELECT 'TXT_KEY_BUILDING_WALLS_HELP', 'Military Units supplied by this City''s population increased by 10%.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Castle is a Medieval-era building which increases Defensive Strength by 15 and Hit Points by 100. Increases Military Units supplied by this City''s population by 10%, and increases Production for all nearby Quarries by 1. The city must possess Walls before the Castle can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_CASTLE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'City must have Walls. +1 [ICON_PRODUCTION] Production for Quarries worked by this City. Military Units supplied by this City''s population increased by 5%. Contains 1 slot for a Great Work of Art or Artifact.'
WHERE Tag = 'TXT_KEY_BUILDING_CASTLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Arsenal is an Industrial-era military building that increases Defense Strength by 20 and Hit Points by 150, making the city more difficult to capture. Increases Military Units supplied by this City''s population by 10%. The city must possess a Castle before it can construct an Arsenal.'
WHERE Tag = 'TXT_KEY_BUILDING_ARSENAL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'City must have a Castle. Military Units supplied by this City''s population increased by 5%.'
WHERE Tag = 'TXT_KEY_BUILDING_ARSENAL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reduces [ICON_HAPPINESS_3] Crime greatly. +20% [ICON_PRODUCTION] Production of Air units, +10 [ICON_STRENGTH] Damage to Air Units during Air Strikes on City.[NEWLINE][NEWLINE]Garrisoned units receive an additional 10 Health when healing in this city. Foreign Spies cannot steal [ICON_RESEARCH] Science from this City.[NEWLINE][NEWLINE]Military Units supplied by this City''s population increased by 5%. City must have an Arsenal.'
WHERE Tag = 'TXT_KEY_BUILDING_MILITARY_BASE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Military Base is a late-game building which increases Defensive Strength by 25 and Hit Points by 200, and improves defense against air units. The city must possess an Arsenal before a Military Base may be constructed. Garrisoned units receive an additional 10 Health when healing in this city. Increases Military Units supplied by this City''s population by 10%.'
WHERE Tag = 'TXT_KEY_BUILDING_MILITARY_BASE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reduces population loss from nuclear attack by 75%, increases Defensive Strength by 5, and [ICON_HAPPINESS_1] Happiness by 1. +10 [ICON_STRENGTH] Damage to Air Units during Air Strikes on City.'
WHERE Tag = 'TXT_KEY_BUILDING_BOMB_SHELTER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires 1 [ICON_RES_COAL] Coal. +1 [ICON_PRODUCTION] Production for every 4 [ICON_CITIZEN] Citizens in the City.[NEWLINE][NEWLINE]Manufactories worked by the City gain +2 [ICON_PRODUCTION] Production.'
WHERE Tag = 'TXT_KEY_BUILDING_FACTORY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+25% [ICON_RESEARCH] Science in the City, +50% [ICON_PRODUCTION] Production when building Spaceship Parts. Requires 1 [ICON_RES_ALUMINUM] Aluminum.[NEWLINE][NEWLINE]City must have a Factory.'
WHERE Tag = 'TXT_KEY_BUILDING_SPACESHIP_FACTORY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Spaceship Factory increases the speed at which a city constructs spaceship parts, and greatly boosts Science in the City. The Spaceship Factory requires one [ICON_RES_ALUMINUM] Aluminum resource, and the city must possess a Factory before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_SPACESHIP_FACTORY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '25% of the [ICON_CULTURE] Culture from World Wonders, Natural Wonders, and Improvements is added to the [ICON_TOURISM] Tourism output of the city. [ICON_TOURISM] Tourism output from Great Works +25%.'
WHERE Tag = 'TXT_KEY_BUILDING_HOTEL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows airlifts to or from this city. [COLOR_POSITIVE_TEXT]Increases air unit capacity of the city from 6 to 10.[ENDCOLOR] +10 [ICON_STRENGTH] Damage to Air Units during Air Strikes on City. 25% of the [ICON_CULTURE] Culture from World Wonders, Natural Wonders, and Improvements is added to the [ICON_TOURISM] Tourism output of the city. [ICON_TOURISM] Tourism output from Great Works +25%.'
WHERE Tag = 'TXT_KEY_BUILDING_AIRPORT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reduces [ICON_HAPPINESS_3] Boredom. When completed, it provides a modest sum of [ICON_CULTURE] Culture and starts 10 turns of ''We Love the King Day'' in the City.[NEWLINE]Nearby [ICON_RES_IVORY] Ivory: +3 [ICON_CULTURE] Culture.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Circus reduces Boredom in a city and improves the Culture output of [ICON_RES_IVORY] Ivory. It also starts 10 turns of ''We Love the King Day'' in the City. Build these to combat Unhappiness from Boredom, and gain quick bursts of Culture.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (
Tag, Text)
SELECT 'TXT_KEY_BUILDING_COLOSSEUM_HELP', '+2 [ICON_TOURISM] Tourism. Barracks, Forge, and Armory in this City gain +2 [ICON_PRODUCTION] Production. Reduces [ICON_HAPPINESS_3] Boredom slightly.'
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
SET Text = '+1 [ICON_HAPPINESS_1] Happiness. Reduces [ICON_HAPPINESS_3] Boredom. Nearby Jungle and Forest tiles gain +1 [ICON_TOURISM] Tourism. Gain 100 [ICON_TOURISM] Tourism (scaling with Era) with all known Civilizations when completed.'
WHERE Tag = 'TXT_KEY_BUILDING_THEATRE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Zoo reduces Boredom in a city, produces additional Culture, and boosts the Gold value of nearby Jungle and Forest tiles. Generates a large sum of Tourism with all known Civilizations when completed.'
WHERE Tag = 'TXT_KEY_BUILDING_THEATRE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Globe Theatre[ENDCOLOR], a building which increases [ICON_TOURISM] Tourism. Also enables the founding of the World Congress.'
WHERE Tag = 'TXT_KEY_TECH_PRINTING_PRESS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Build the Stadium if you are going for a [COLOR_POSITIVE_TEXt]Culture Victory[ENDCOLOR], or you are having problems with happiness from Boredom in your empire. Provides a huge sum of Golden Age Points when completed.'
WHERE Tag = 'TXT_KEY_BUILDING_STADIUM_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reduces [ICON_HAPPINESS_3] Boredom greatly. Provides a large sum of [ICON_GOLDEN_AGE] Golden Age Points when completed.[NEWLINE][NEWLINE]25% of the [ICON_CULTURE] Culture from World Wonders, Natural Wonders, and Improvements is added to the [ICON_TOURISM] Tourism output of the city. [ICON_TOURISM] Tourism output from Great Works +25%.'
WHERE Tag = 'TXT_KEY_BUILDING_STADIUM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reduces [ICON_HAPPINESS_3] Crime greatly. Reduces enemy spy stealing rate by 25%, and prevents the theft of [ICON_GOLD] Gold through Spy Advanced Actions. When an enemy Spy is killed in this City, gain 500 [ICON_RESEARCH] Science and [ICON_CULTURE] Culture, scaling with Era and the level of the Spy killed.[NEWLINE][NEWLINE]City must have a Constabulary.'
WHERE Tag = 'TXT_KEY_BUILDING_POLICE_STATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reduces [ICON_HAPPINESS_3] Crime. Reduces enemy spy stealing rate by 25%, and prevents the disruption of Building construction by Spy Advanced Actions.'
WHERE Tag = 'TXT_KEY_BUILDING_CONSTABLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Provides 5 [ICON_GREAT_PEOPLE] Great Person Points (GPP) towards a Great Musician. Add up to two specialists to this building to gain [ICON_CULTURE] Culture and increase the rate of Great Musician acquisition. [NEWLINE][NEWLINE]Maximum of 3 of these buildings in your Empire.'
WHERE Tag = 'TXT_KEY_BUILDING_MUSICIANS_GUILD_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Provides 4 [ICON_GREAT_PEOPLE] Great Person Points (GPP) towards a Great Artist. Add up to two specialists to this building to gain [ICON_CULTURE] Culture and increase the rate of Great Artist acquisition. [NEWLINE][NEWLINE]Maximum of 3 of these buildings in your Empire.'
WHERE Tag = 'TXT_KEY_BUILDING_ARTISTS_GUILD_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Provides 3 [ICON_GREAT_PEOPLE] Great Person Points (GPP) towards a Great Writer. Add up to two specialists to this building to gain [ICON_CULTURE] Culture and increase the rate of Great Writer acquisition. [NEWLINE][NEWLINE]Maximum of 3 of these buildings in your Empire.'
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
SET Text = 'This National Wonder gives all land units built in this city (past and future) the "Morale" promotion, increasing their combat strength by +10%. +1 [ICON_PRODUCTION] Production in the City for every 5 [ICON_CITIZEN] Citizens. Increases the Military Unit Supply Cap from Population in the City by 10%. The Heroic Epic can be constructed when a city has a barracks.'
WHERE Tag = 'TXT_KEY_BUILDING_HEROIC_EPIC_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'This National Wonder increases the [ICON_GREAT_PEOPLE] Great People generation of a city by 25%. Receive [ICON_CULTURE] Culture when a [ICON_CITIZEN] Citizen is born in the City, and [ICON_GOLDEN_AGE] Golden Age Points whenever you unlock a policy. It also provides +1 [ICON_CULTURE] Culture. A city must have a monument before it can construct a National Monument.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_EPIC_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'This National Wonder provides +2 [ICON_HAPPINESS_1] Happiness, +1 [ICON_CULTURE] Culture, and reduces [ICON_HAPPINESS_3] Boredom, and generates +10% [ICON_CULTURE] Culture and [ICON_GOLD] Gold during ''We Love the King Day'' in the City where it is built. A city must have an Arena before it can construct a Circus Maximus.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_MAXIMUS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The East India Company increases the amount of [ICON_GOLD] Gold a city generates and reduces [ICON_HAPPINESS_3] Poverty. Resource Diversity Modifiers for Trade Routes from this City increase by 25% if positive, and decrease by 25% if negative. You also receive a free copy of all Luxury Resources around the City. Owned Spies are much more likely to steal [ICON_GOLD] Gold via Advanced Actions.[NEWLINE][NEWLINE]Trade routes other players make to a city with an East India Company will generate an extra 4 [ICON_GOLD] Gold for the city owner and the trade route owner gains an additional 2 [ICON_GOLD] Gold for the trade route.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_TREASURY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The School of Philosophy National Wonder produces [ICON_RESEARCH] Science and [ICON_CULTURE] Culture, especially during [ICON_GOLDEN_AGE] Golden Ages. A city must have a library before it can construct a School of Philosophy.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_COLLEGE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Ironworks National Wonder increases [ICON_PRODUCTION] Production in a city by 10, and generates [ICON_RESEARCH] Science every time you construct a building. A city must have a Forge before it can construct an Ironworks.'
WHERE Tag = 'TXT_KEY_BUILDING_IRONWORKS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Oxford University National Wonder provides +1 [ICON_CULTURE] Culture, and reduces [ICON_HAPPINESS_3] Illiteracy by 2. +75 [ICON_CULTURE] Culture every time you research a Technology. The city must have a university before it can construct Oxford University.'
WHERE Tag = 'TXT_KEY_BUILDING_OXFORD_UNIVERSITY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'This National Wonder produces +1 [ICON_CULTURE] Culture for every 4 [ICON_CITIZEN] Citizens in the City, and +10% [ICON_CULTURE] Culture for the City. It cannot be constructed unless the city has an Opera House.'
WHERE Tag = 'TXT_KEY_BUILDING_HERMITAGE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'An important defensive National Wonder for a technology-driven civilization. Reduces [ICON_HAPPINESS_3] Crime. The National Intelligence Agency provides an additional spy, improves chance of Great Person assassination via Advanced Actions for spies, levels up all your existing spies, and provides a 15% reduction in enemy spy effectiveness. Empires with a lot of offensive spies will benefit greatly from this building. A city must have a Police Station before it can construct the National Intelligence Agency.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'An important building for a civilization trying to spread their religion world-wide from an empire with few, populous cities. A city must have a Temple before it can construct the Grand Temple.'
WHERE Tag = 'TXT_KEY_BUILDING_GRAND_TEMPLE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'With its massive boost to [ICON_TOURISM] Tourism, the National Visitor Center is an excellent choice for civilizations that are trying to achieve a Culture victory, or attempting to increase your ideological impact on other civilizations. Build it in your top [ICON_TOURISM] Tourism city that has a Hotel.'
WHERE Tag = 'TXT_KEY_BUILDING_TOURIST_CENTER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

-- Help Text

UPDATE Language_en_US
SET Text = 'All land Units created by this City (past or future) receive the [COLOR_POSITIVE_TEXT]Morale[ENDCOLOR] Promotion, improving [ICON_STRENGTH] combat strength by 10%. +1 [ICON_PRODUCTION] Production in the City for every 5 [ICON_CITIZEN] Citizens. Increases the Military Unit Supply Cap from Population in the City by 10%. Contains 1 slot for a Great Work of Writing.[NEWLINE][NEWLINE]Must have built a Barracks in the city. The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_HEROIC_EPIC_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+25% [ICON_GREAT_PEOPLE] Great People generation in this City. Receive 15 [ICON_CULTURE] Culture when a [ICON_CITIZEN] Citizen is born in the City, and 50 [ICON_GOLDEN_AGE] Golden Age Points whenever you unlock a policy, scaling with Era.[NEWLINE][NEWLINE] Contains 1 slot for a Great Work of Art or an Artifact.[NEWLINE][NEWLINE]Must have built a Monument in the city. The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_EPIC_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+10% [ICON_CULTURE] Culture and [ICON_GOLD] Gold during ''We Love the King Day'' in the City where it is built. +2 [ICON_HAPPINESS_1] Happiness,and all Arenas in Empire gain +2 [ICON_GOLD] Gold. Reduces [ICON_HAPPINESS_3] Boredom. Must have built an Arena in the city. The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_MAXIMUS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reduces [ICON_HAPPINESS_3] Poverty. Receive an additional copy of all Luxury Resources around this City. Incoming [ICON_INTERNATIONAL_TRADE] Trade Routes generate +4 [ICON_GOLD] Gold for the City, and +2 [ICON_GOLD] Gold for Trade Route owner.[NEWLINE][NEWLINE]Must have built a Customs House in the city. The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_TREASURY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+20% [ICON_RESEARCH] Science in the City during [ICON_GOLDEN_AGE] Golden Ages.[NEWLINE][NEWLINE]Contains 1 slot for a Great Work of Writing. Must have built a Library in the city. The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_COLLEGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+25 [ICON_RESEARCH] Science when you construct a building in this City. Bonus scales with Era. Must have built a Forge in the city. The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_IRONWORKS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A Great Scientist appears near the City. +75 [ICON_CULTURE] Culture every time you research a Technology, scaling with Era. Contains 2 slots for Great Works of Writing. Reduces [ICON_HAPPINESS_3] Illiteracy.[NEWLINE][NEWLINE]Must have built a University in the city. The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of cities you own.[NEWLINE][NEWLINE]+3 [ICON_RESEARCH] Science if Themed.'
WHERE Tag = 'TXT_KEY_BUILDING_OXFORD_UNIVERSITY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture for every 4 [ICON_CITIZEN] Citizens in the City. +10% [ICON_CULTURE] Culture in this City. Contains 3 slots for Great Works of Art.[NEWLINE][NEWLINE]Must have built an Opera House in the city. The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of cities you own.[NEWLINE][NEWLINE]+5 [ICON_GOLD] Gold if Themed.'
WHERE Tag = 'TXT_KEY_BUILDING_HERMITAGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reduces [ICON_HAPPINESS_3] Crime in all Cities. Slows enemy spy effectiveness by 15%, provides an additional spy, and levels up all existing spies. Owned Spies are much more likely to assassinate [ICON_GREAT_PEOPLE] Great People via Advanced Actions.[NEWLINE][NEWLINE]+400 [ICON_RESEARCH] Science and [ICON_GOLD] Gold when you steal a [ICON_GREAT_WORK] Great Work or [ICON_RESEARCH] Technology, or complete a City-State Coup (scaling with Era). +100 [ICON_RESEARCH] Science and [ICON_GOLD] Gold when you complete an Advanced Action (scaling with Era).[NEWLINE][NEWLINE]Must have a Police Station in the city. The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'All Temples gain +2 [ICON_PEACE] Faith and [ICON_CULTURE] Culture. Reduces [ICON_HAPPINESS_3] Religious Unrest. Must have built a Temple in the city. The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_GRAND_TEMPLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Must have built a Hotel in the city.[NEWLINE][NEWLINE]25% of the [ICON_CULTURE] Culture from World Wonders, Natural Wonders, and Improvements is added to the [ICON_TOURISM] Tourism output of all Cities, and an additional 25% to this City. [ICON_TOURISM] Tourism output from Great Works in all Cities increased by +25%, and an additional 25% in this City.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_TOURIST_CENTER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Tile improvement construction speed increased by 25% and a Settler appears near the City.'
WHERE Tag = 'TXT_KEY_WONDER_CHICHEN_ITZA_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'All units gain +15% [ICON_STRENGTH] combat strength when attacking cities. Reduces [ICON_HAPPINESS_3] Crime in all cities. Receive a free Barracks in the City.'
WHERE Tag = 'TXT_KEY_WONDER_STATUE_ZEUS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
SELECT 'TXT_KEY_BUILDING_STONEHENGE_HELP', 'Grants 50 [ICON_PEACE] Faith when completed. Receive a free Council in the city.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '''We Love the King Day'' begins in the City. During ''We Love the King Day'', City [ICON_PRODUCTION] Production, [ICON_RESEARCH] Science, and [ICON_GOLD] Gold increase by +10%. [NEWLINE][NEWLINE]Receive a free Stone Works in the City.'
WHERE Tag = 'TXT_KEY_WONDER_MAUSOLEUM_HALICARNASSUS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+10% [ICON_FOOD] Food in all cities. +15% [ICON_PRODUCTION] Production when building ranged units. Free Herbalist in the City.'
WHERE Tag = 'TXT_KEY_WONDER_TEMPLE_ARTEMIS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_GOLD] Gold on all Desert tiles worked by this city. Gains an additional trade route slot and a Caravan appears in the city. +6 [ICON_CULTURE] Culture once Archaeology is discovered. City must be built on or next to Desert.'
WHERE Tag = 'TXT_KEY_WONDER_PETRA_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '1 Free Technology. Provides a free Library in the city in which it is built.  Contains 2 slots for Great Works of Writing.[NEWLINE][NEWLINE]+3 [ICON_RESEARCH] Science if Themed.'
WHERE Tag = 'TXT_KEY_WONDER_GREAT_LIBRARY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Provides a free Garden in the city in which it is built.'
WHERE Tag = 'TXT_KEY_WONDER_HANGING_GARDEN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reduces [ICON_HAPPINESS_3] Crime in all cities, and increases the Military Unit Supply Cap by 3 in the city. Creates a copy of each type of military land unit you control and places the unit near the city where the Terracotta Army is constructed. Receive a large sum of [ICON_CULTURE] Culture when completed.'
WHERE Tag = 'TXT_KEY_WONDER_TERRA_COTTA_ARMY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reduces [ICON_HAPPINESS_3] Boredom in all cities. Increases Military Units supplied by this City''s population by 10%. Contains a prebuilt Great Work of Art in one of the Great Work Slots. All owned Amphitheaters gain +1 [ICON_CULTURE] Culture and +1 [ICON_RESEARCH] Science.[NEWLINE][NEWLINE]+3 [ICON_CULTURE] Culture if themed.'
WHERE Tag = 'TXT_KEY_WONDER_PARTHENON_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Instantly receive 500 [ICON_CULTURE] Culture and [ICON_RESEARCH] Science. Reduces [ICON_HAPPINESS_3] Illiteracy in all cities. Receive a free Temple in the City where it is built.'
WHERE Tag = 'TXT_KEY_WONDER_ORACLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[ICON_CULTURE] Culture and [ICON_GOLD] Gold costs of acquiring new tiles reduced by 25% in every city. Receive a free Mandir in the City where it is built.'
WHERE Tag = 'TXT_KEY_WONDER_ANGKOR_WAT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );



UPDATE Language_en_US
SET Text = 'Receive a free Great General. [NEWLINE][NEWLINE]Enemy land Units expend all [ICON_MOVES] movement when entering your territory. Increases the Military Unit Supply Cap by 3. Provides Walls for free in the city in which it is built.[NEWLINE][NEWLINE]Upon discovering Dynamite, The Great Wall becomes obsolete.'
WHERE Tag = 'TXT_KEY_WONDER_GREAT_WALL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires completion of Authority Branch. All non-air Units (new and previously-created by this City) receive the [COLOR_POSITIVE_TEXT]Drill I[ENDCOLOR] Promotion, improving [ICON_STRENGTH] combat strength and city attack strength. Provides a free Castle in the city in which it is built. Boosts city [ICON_CULTURE] Culture output by 10%.'
WHERE Tag = 'TXT_KEY_WONDER_ALHAMBRA_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires completion of Tradition Branch. Receive 50 [ICON_RESEARCH] Science when you expend a [ICON_GREAT_PEOPLE] Great Person. Bonus scales with Era. Provides a free Mosque in the city in which it is built.'
WHERE Tag = 'TXT_KEY_WONDER_MOSQUE_OF_DJENNE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'University of Sankore'
WHERE Tag = 'TXT_KEY_BUILDING_MOSQUE_OF_DJENNE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'As the center of an Islamic scholarly community, the University of Sankore was very different in organization from the universities of medieval Europe. It had no central administration other than the Emperor. It had no student registers but kept copies of its student publishings. It was composed of several entirely independent schools or colleges, each run by a single master or imam. Students associated themselves with a single teacher, and courses took place in the open courtyard of the mosque or at private residences.'
WHERE Tag = 'TXT_KEY_WONDER_MOSQUE_OF_DJENNE_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A free Great Prophet appears. Provides a free Church in the city in which it is built.'
WHERE Tag = 'TXT_KEY_WONDER_HAGIA_SOPHIA_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Enables you to select a Reformation Belief without the normal prerequisites. Must be built in a Holy City.'
WHERE Tag = 'TXT_KEY_WONDER_KREMLIN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Cathedral of St. Basil'
WHERE Tag = 'TXT_KEY_BUILDING_KREMLIN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Cathedral of Vasily the Blessed, commonly known as the Cathedral of Saint Basil, is a former church in Red Square in Moscow, Russia. The building, now a museum, is officially known as the Cathedral of the Intercession of the Most Holy Theotokos on the Moat or Pokrovsky Cathedral. It was built from 1555–61 on orders from Ivan the Terrible and commemorates the capture of Kazan and Astrakhan. A world famous landmark, it has been the hub of growth since the 14th century and was the tallest building in the city until the completion of the Ivan the Great Bell Tower in 1600.'
WHERE Tag = 'TXT_KEY_WONDER_KREMLIN_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Must be built in a Holy City. 3 free Missionaries appear. A free Stupa is constructed in the City.'
WHERE Tag = 'TXT_KEY_WONDER_BOROBUDUR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+15% [ICON_STRENGTH] combat strength for units fighting in friendly territory, and +10% [ICON_STRENGTH] Defense for all Cities. Increases the Military Unit Supply Cap by 1 in every city. Provides a free Castle in the city in which it is built.'
WHERE Tag = 'TXT_KEY_WONDER_HIMEJI_CASTLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+15% [ICON_GOLD] Gold from [ICON_CONNECTED] City Connections. City gains +1 [ICON_FOOD] Food, [ICON_PRODUCTION] Production, [ICON_CULTURE] Culture, and [ICON_PEACE] Faith for every Mountain within 3 tiles of the City. [NEWLINE][NEWLINE]City must be built within 2 tiles of a Mountain that is inside your territory.'
WHERE Tag = 'TXT_KEY_WONDER_MACHU_PICHU_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
SELECT 'TXT_KEY_BUILDING_NOTRE_DAME_HELP', 'Receive a free Cathedral in the City, and starts a [ICON_GOLDEN_AGE] Golden Age. Contains two slots for Great Works of Art or Artifacts.[NEWLINE][NEWLINE]+3 [ICON_PEACE] Faith if Themed.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires completion of Progress Branch. Cost of [ICON_GOLD] Gold purchasing in all cities reduced by 15%. Reduces [ICON_HAPPINESS_3] Poverty in all cities.'
WHERE Tag = 'TXT_KEY_WONDER_FORBIDDEN_PALACE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Length of [ICON_GOLDEN_AGE] Golden Ages increased by 50%. Reduces [ICON_HAPPINESS_3] Religious Unrest in all cities.'
WHERE Tag = 'TXT_KEY_WONDER_TAJ_MAHAL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '1 free Great Writer appears near the City where the Wonder was built. Contains 2 slots for Great Works of Writing.[NEWLINE][NEWLINE]+5 [ICON_GOLD] Gold if Themed.'
WHERE Tag = 'TXT_KEY_WONDER_GLOBE_THEATER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '1 free Great Artist appears near the City where the Wonder was built. Contains 3 slots for Great Works of Art.[NEWLINE][NEWLINE]+5 [ICON_CULTURE] Culture if Themed.'
WHERE Tag = 'TXT_KEY_WONDER_UFFIZI_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reqiures completion of Fealty Branch. The empire enters a [ICON_GOLDEN_AGE] Golden Age. +1 [ICON_GOLDEN_AGE] Golden Age Points per turn for every 2 [ICON_CITIZEN] Citizens in the City. Reduces [ICON_HAPPINESS_3] Religious Unrest in all cities.'
WHERE Tag = 'TXT_KEY_WONDER_PYRAMIDS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+10% [ICON_CULTURE] Culture in all Cities. Contains 2 slots for Great Works of Art.[NEWLINE][NEWLINE]+5 [ICON_PEACE] Faith if Themed.'
WHERE Tag = 'TXT_KEY_WONDER_SISTINE_CHAPEL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receive a free Great Engineer and a free Arsenal in the city. Greatly increases the [ICON_STRENGTH] Defense and Hit Points of the city, and increases the Military Unit Supply Cap from population by 5% in all Cities.'
WHERE Tag = 'TXT_KEY_WONDER_RED_FORT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+25% generation of [ICON_GREAT_PEOPLE] Great People in the City, and +10% in all other Cities. A free Great Person of your choice appears near the [ICON_CAPITAL] Capital.'
WHERE Tag = 'TXT_KEY_WONDER_LEANING_TOWER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A Great Scientist appears near the City where the Wonder was built. Reduces [ICON_HAPPINESS_3] Illiteracy in all cities. 50% more [ICON_RESEARCH] Science generated from Research Agreements. If Research Agreements are disabled, provides a +25% [ICON_RESEARCH] Science bonus in the City in which it is built.'
WHERE Tag = 'TXT_KEY_WONDER_PORCELAIN_TOWER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A Great General appears near the City where the Wonder was built and +15 XP for all Units built in this City. Reduces [ICON_HAPPINESS_3] Crime and increases the Military Unit Supply Cap by 10 in the City where it is built.'
WHERE Tag = 'TXT_KEY_WONDER_BRANDENBURG_GATE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires completion of Artistry branch. 1 free Great Artist appears near the City where the Wonder was built. Contains 4 slots for Great Works of Art.[NEWLINE][NEWLINE]+8 [ICON_CULTURE] Culture if Themed.'
WHERE Tag = 'TXT_KEY_WONDER_LOUVRE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reduces [ICON_HAPPINESS_3] Boredom in all cities. [ICON_CULTURE] Culture cost of adopting new Policies reduced by 10%.'
WHERE Tag = 'TXT_KEY_WONDER_EIFFEL_TOWER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires completion of Statecraft Branch. Grants 3 additional Delegates in the World Congress for every 8 City-States in the game. Reduces [ICON_HAPPINESS_3] Poverty in all cities.'
WHERE Tag = 'TXT_KEY_WONDER_BIG_BEN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires completion of Statecraft Branch. Grants 3 additional Delegates in the World Congress for every 8 City-States in the game. Reduces [ICON_HAPPINESS_3] Poverty in all cities.'
WHERE Tag = 'TXT_KEY_WONDER_BIG_BEN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Palace of Westminster'
WHERE Tag = 'TXT_KEY_BUILDING_BIG_BEN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_GOLD] Gold, [ICON_CULTURE] Culture, and [ICON_HAPPINESS_1] Happiness from every Castle.[NEWLINE][NEWLINE]City must be built within 2 tiles of a Mountain that is inside your territory.'
WHERE Tag = 'TXT_KEY_WONDER_NEUSCHWANSTEIN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires completion of Industry Branch. 1 free Great Musician appears near the City where the Wonder was built. Contains 3 slots for Great Works of Music. +250 [ICON_CULTURE] Culture when you construct a building in this City, scaling with Era.[NEWLINE][NEWLINE]+15 [ICON_GOLD] Gold if Themed.'
WHERE Tag = 'TXT_KEY_WONDER_BROADWAY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires Autocracy. +1 [ICON_HAPPINESS_1] Happiness for every 2 Policies you have adopted. 1 Free Social Policy. Enemy Spies cannot disrupt World Wonder [ICON_PRODUCTION] Production in this City.[NEWLINE][NEWLINE]Receive 10 [ICON_CULTURE] Culture when you pillage a tile, scaling with Era. Must be constructed in a coastal city.'
WHERE Tag = 'TXT_KEY_WONDER_PRORA_RESORT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires completion of Imperialism Branch. [ICON_GOLD] Gold cost of upgrading military units reduced by 33%. Owned Spies are much more likely to disrupt Unit [ICON_PRODUCTION] Production via Advanced Actions.'
WHERE Tag = 'TXT_KEY_WONDER_PENTAGON_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+25 [ICON_TOURISM] Tourism. Hotels provide +2 [ICON_GOLDEN_AGE] Golden Age Points, [ICON_CULTURE] Culture, and [ICON_TOURISM] Tourism.'
WHERE Tag = 'TXT_KEY_WONDER_CRISTO_REDENTOR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '1 Free Social Policy and +25% [ICON_CULTURE] Culture in this city.  Contains 2 slots for Great Works of Music. Must be constructed in a coastal city.[NEWLINE][NEWLINE]+15 [ICON_CULTURE] Culture if Themed.'
WHERE Tag = 'TXT_KEY_WONDER_SYDNEY_OPERA_HOUSE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'All Research Labs gain +10 [ICON_RESEARCH] Science. 99.9% reduction in effectiveness of enemy spies in the city in which it is built. All other cities in the civilization get a 25% reduction in enemy spy effectiveness. Negates the [ICON_TOURISM] Tourism bonus from the technologies of other players.'
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
