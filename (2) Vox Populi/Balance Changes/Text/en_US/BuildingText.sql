-- added Recycling Center
UPDATE Language_en_US
SET Text = 'Provides 2 [ICON_RES_ALUMINUM] Aluminum.[NEWLINE][NEWLINE]Maximum of 5 of these Buildings in your Empire.'
WHERE Tag = 'TXT_KEY_BUILDING_RECYCLING_CENTER_HELP';

UPDATE Language_en_US
SET Text = '+1 [ICON_RESEARCH] Science for every 3 [ICON_CITIZEN] Citizens in the City. Contains 1 slot for a [ICON_GREAT_WORK] Great Work of Art. 1 Specialist in this City no longer produces [ICON_HAPPINESS_3] Unhappiness from [ICON_URBANIZATION] Urbanization.[NEWLINE][NEWLINE]When one of the following [COLOR_POSITIVE_TEXT]Historic Events[ENDCOLOR] is completed, receive a [ICON_TOURISM] Tourism boost with all known Civs based on your recent [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism output:[NEWLINE][ICON_BULLET] Earn a [ICON_GREAT_PEOPLE] Great Person[NEWLINE][ICON_BULLET] Build a [ICON_GOLDEN_AGE] World Wonder[NEWLINE][ICON_BULLET] Win a [ICON_WAR] War (Warscore 25+)[NEWLINE][ICON_BULLET] Enter a new [ICON_RESEARCH] Era[NEWLINE][NEWLINE][ICON_CONNECTED] Connecting other Cities to the [ICON_CAPITAL] Capital by Road will produce additional [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_PALACE_HELP';

UPDATE Language_en_US
SET Text = '+25 [ICON_FOOD] Food when completed. 15% of [ICON_FOOD] Food is carried over after a new [ICON_CITIZEN] Citizen is born.[NEWLINE][NEWLINE]Allows [ICON_FOOD] Food to be moved from this City along trade routes inside your Civilization.[NEWLINE][NEWLINE]Nearby [ICON_RES_WHEAT] Wheat: +1 [ICON_FOOD] Food.[NEWLINE]Nearby [ICON_RES_MAIZE] Maize: +1 [ICON_FOOD] Food.[NEWLINE]Nearby [ICON_RES_RICE] Rice: +1 [ICON_FOOD] Food.[NEWLINE]Nearby [ICON_RES_BANANA] Bananas: +1 [ICON_FOOD] Food.'
WHERE Tag = 'TXT_KEY_BUILDING_GRANARY_HELP';

UPDATE Language_en_US
SET Text = '+1 [ICON_PRODUCTION] Production for every 4 [ICON_CITIZEN] Citizens in the City.[NEWLINE][NEWLINE]Can only be built in a City next to a River.'
WHERE Tag = 'TXT_KEY_BUILDING_WATERMILL_HELP';

UPDATE Language_en_US
SET Text = 'The Water Mill can only be constructed in a City located next to a river. The Water Mill increases the City''s [ICON_PRODUCTION] Production more efficiently (per Citizen in the City) than the Well.'
WHERE Tag = 'TXT_KEY_BUILDING_WATERMILL_STRATEGY';

UPDATE Language_en_US
SET Text = '15% of [ICON_FOOD] Food is carried over after a new [ICON_CITIZEN] Citizen is born, and +25% of the [ICON_PRODUCTION] Production of the City is added to the City''s current [ICON_PRODUCTION] Production. +2 [ICON_FOOD] Food on Lakes and Oases worked by this City.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty.'
WHERE Tag = 'TXT_KEY_BUILDING_AQUEDUCT_HELP';

UPDATE Language_en_US
SET Text = 'The Aqueduct decreases the amount of [ICON_FOOD] Food a City needs to increase in size by 15%. Build Aqueducts in cities that you want to grow large over time. The City needs to have a Granary in order to construct the Aqueduct.'
WHERE Tag = 'TXT_KEY_BUILDING_AQUEDUCT_STRATEGY';

UPDATE Language_en_US
SET Text = '+2 [ICON_CITIZEN] Population when completed. 15% of [ICON_FOOD] Food is carried over after a new [ICON_CITIZEN] Citizen is born. [ICON_VP_SCIENTIST] Scientists, [ICON_VP_MERCHANT] Merchants, and [ICON_VP_ENGINEER] Engineers in the City produce +2 of their base Yield.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty.'
WHERE Tag = 'TXT_KEY_BUILDING_MEDICAL_LAB_HELP';

UPDATE Language_en_US
SET Text = 'The Medical Lab is a late-game building which decreases the amount of [ICON_FOOD] Food a City needs to increase in size by 15% and boosts the City''s Science production from specialists. The City needs to have a Hospital in order to construct the Medical Lab.'
WHERE Tag = 'TXT_KEY_BUILDING_MEDICAL_LAB_STRATEGY';

UPDATE Language_en_US
SET Text = '+25% [ICON_GREAT_PEOPLE] Great People generation in this City. [NEWLINE][NEWLINE]1 Specialist in this City no longer produces [ICON_HAPPINESS_3] Unhappiness from [ICON_URBANIZATION] Urbanization.[NEWLINE][NEWLINE]Nearby Oases: +2 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_CITRUS] Citrus: +1 [ICON_FOOD] Food, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COCOA] Cocoa: +1 [ICON_FOOD] Food, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_CLOVES] Cloves: +1 [ICON_PEACE] Faith, +1 [ICON_CULTURE] Culture.[NEWLINE]Nearby [ICON_RES_PEPPER] Pepper: +1 [ICON_PEACE] Faith, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_NUTMEG] Nutmeg: +1 [ICON_CULTURE] Culture, +1 [ICON_PRODUCTION] Production.'
WHERE Tag = 'TXT_KEY_BUILDING_GARDEN_HELP';

UPDATE Language_en_US
SET Text = 'The Garden increases the speed at which [ICON_GREAT_PEOPLE] Great People are generated in the City by 25%, and buffs the food output of Oases. Gardens require an Aqueduct in the City in order to be built.'
WHERE Tag = 'TXT_KEY_BUILDING_GARDEN_STRATEGY';

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BUILDING_HOSPITAL_HELP', '+1 [ICON_FOOD] Food for every 10 [ICON_CITIZEN] Citizens in the City. 5% of the City''s [ICON_FOOD] Food is converted into [ICON_RESEARCH] Science every turn. Land Units in this City heal 15 points per turn whether or not they take an action.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty.');
INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BUILDING_TEMPLE_HELP', 'Generates +25% Religious Pressure. Contains 1 slot for a [ICON_VP_GREATMUSIC] Great Work of Music.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_RELIGION] Religious Unrest.[NEWLINE][NEWLINE]Nearby [ICON_RES_INCENSE] Incense: +1 [ICON_CULTURE] Culture, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_WINE] Wine: +1 [ICON_CULTURE] Culture, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_AMBER] Amber: +1 [ICON_CULTURE] Culture, +1 [ICON_GOLD] Gold.');

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BUILDING_MONUMENT_HELP', '+34% Faster [ICON_CULTURE_LOCAL] Border Growth in this City.');

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BUILDING_AMPHITHEATER_HELP', '+33% [ICON_GREAT_WRITER] Great Writer Rate in the City, and all Writers'' Guilds produce +1 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]Nearby [ICON_RES_DYE] Dye: +1 [ICON_CULTURE] Culture, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_SILK] Silk: +1 [ICON_CULTURE] Culture, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_LAPIS] Lapis Lazuli: +2 [ICON_CULTURE] Culture.[NEWLINE]Nearby [ICON_RES_BRAZILWOOD] Brazilwood: +1 [ICON_CULTURE] Culture, +1 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]+2 [ICON_CULTURE] Culture if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].');

UPDATE Language_en_US
SET Text = 'The Amphitheater increases the [ICON_CULTURE] Culture of a city. Also boosts the City''s Great Writer rate and the value of Writers'' Guilds. Grants bonuses to all nearby [ICON_RES_DYE] Dye, [ICON_RES_SILK] Silk, [ICON_RES_LAPIS] Lapis Lazuli, and [ICON_RES_BRAZILWOOD] Brazilwood resources, speeding the growth of the territory of the city and the acquisition of Social Policies. Contains 2 slots for Great Works of Writing.'
WHERE Tag = 'TXT_KEY_BUILDING_AMPHITHEATER_STRATEGY';

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BUILDING_OPERA_HOUSE_HELP', '+5% [ICON_CULTURE] Culture in the City. [ICON_GREAT_WORK] Great Works in the City produce +1 [ICON_TOURISM] Tourism. +33% [ICON_GREAT_MUSICIAN] Great Musician Rate in the City, and all Musicians'' Guilds produce +1 [ICON_GOLD] Gold.');

UPDATE Language_en_US
SET Text = 'The Opera House is a Renaissance-era building which increases the [ICON_CULTURE] Culture of a City. Also boosts the City''s Great Musician rate and the value of Musicians'' Guilds. Contains 1 slot for a Great Work of Music. Requires an Amphitheater in the City before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_OPERA_HOUSE_STRATEGY';

UPDATE Language_en_US
SET Text = 'The Museum is a mid-game building which increases [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism output and reduces Boredom. Contains 2 slots for Great Works of Art.  Requires a Gallery in the City before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_MUSEUM_STRATEGY';

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BUILDING_MUSEUM_HELP', 'Allows [COLOR_YELLOW]Archaeologists[ENDCOLOR] to be built in this City. +1 [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism for every 4 [ICON_CITIZEN] Citizens in the City. [ICON_GREAT_WORK] Great Works in the City produce +1 [ICON_TOURISM] Tourism.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom.[NEWLINE][NEWLINE]Contains 2 slots for  [ICON_GREAT_WORK] Great Works of Art or Artifacts. +4 [ICON_CULTURE] Culture if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].');

UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism for every 2 [ICON_CITIZEN] Citizens in the City. [ICON_GREAT_WORK] Great Works in the City produce +2 [ICON_TOURISM] Tourism.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom.[NEWLINE][NEWLINE]+10 [ICON_GOLD] Gold if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_BUILDING_BROADCAST_TOWER_HELP';

UPDATE Language_en_US
SET Text = 'A Broadcast Tower is a late-game building which increases [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism output of the City, and reduces Boredom. Requires a Museum in the City before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_BROADCAST_TOWER_STRATEGY';

UPDATE Language_en_US
SET Text = '+15 XP for all Units. Increases [ICON_SILVER_FIST] Military Unit Supply Cap by 1.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD] and [ICON_PRODUCTION] Distress.'
WHERE Tag = 'TXT_KEY_BUILDING_BARRACKS_HELP';

UPDATE Language_en_US
SET Text = '+20 XP for all Units. Increases [ICON_SILVER_FIST] Military Unit Supply Cap by 1.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD] and [ICON_PRODUCTION] Distress.'
WHERE Tag = 'TXT_KEY_BUILDING_ARMORY_HELP';

UPDATE Language_en_US
SET Text = '+15% [ICON_PRODUCTION] Production towards Land Units, and +25 XP for all Units. Increases [ICON_SILVER_FIST] Military Unit Supply Cap by 1.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD] and [ICON_PRODUCTION] Distress.'
WHERE Tag = 'TXT_KEY_BUILDING_MILITARY_ACADEMY_HELP';

UPDATE Language_en_US
SET Text = '+33% [ICON_PRODUCTION] Production when building Mounted Melee Units. Increases [ICON_SILVER_FIST] Military Unit Supply Cap from Population in the City by 10%. Internal [ICON_INTERNATIONAL_TRADE] Trade Routes from this City generate +2 [ICON_PRODUCTION] Production.[NEWLINE][NEWLINE]Nearby [ICON_RES_HORSE] Horses: +2 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_SHEEP] Sheep: +2 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_COW] Cattle: +2 [ICON_PRODUCTION] Production.[NEWLINE][NEWLINE]City must have at least one of these resources improved with a Pasture.'
WHERE Tag = 'TXT_KEY_BUILDING_STABLE_HELP';

UPDATE Language_en_US
SET Text = 'Mines worked by this City and [ICON_VP_ENGINEER] Engineers gain +2 [ICON_PRODUCTION] Production.[NEWLINE][NEWLINE]Nearby [ICON_RES_IRON] Iron: +1 [ICON_PRODUCTION] Production, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COPPER] Copper: +2 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_FORGE_HELP';

UPDATE Language_en_US
SET Text = 'The Forge improves sources of [ICON_RES_IRON] Iron and [ICON_RES_COPPER] Copper nearby, and boosts the [ICON_PRODUCTION] Production of Mines and Engineers.'
WHERE Tag = 'TXT_KEY_BUILDING_FORGE_STRATEGY';

UPDATE Language_en_US
SET Text = '+1 [ICON_PRODUCTION] Production from all Forests worked by this City, and +1 [ICON_PRODUCTION] Production for every 4 [ICON_CITIZEN] Citizens in the City. Internal [ICON_INTERNATIONAL_TRADE] Trade Routes from this City generate +4 [ICON_PRODUCTION] Production.[NEWLINE][NEWLINE]Allows [ICON_PRODUCTION] Production to be moved from this City along trade routes inside your Civilization.'
WHERE Tag = 'TXT_KEY_BUILDING_WORKSHOP_HELP';

UPDATE Language_en_US
SET Text = 'Allows [ICON_PRODUCTION] Production to be moved from this City along trade routes inside your Civilization.[NEWLINE][NEWLINE]Nearby [ICON_RES_MARBLE] Marble: +1 [ICON_PRODUCTION] Production, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_STONE] Stone: +2 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_SALT] Salt: +2 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_JADE] Jade: +1 [ICON_PRODUCTION] Production, +1 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]City must have at least one of these resources improved.'
WHERE Tag = 'TXT_KEY_BUILDING_STONE_WORKS_HELP';

UPDATE Language_en_US
SET Text = 'Stone Works can only be constructed in a City near an improved [ICON_RES_STONE] Stone, [ICON_RES_MARBLE] Marble, [ICON_RES_SALT] Salt, [ICON_RES_JADE] Jade, [ICON_RES_AMBER] Amber, [ICON_RES_LAPIS] Lapis Lazuli, or [ICON_RES_URANIUM] Uranium resource. Stone Works increase [ICON_PRODUCTION] Production and allow [ICON_PRODUCTION] Production to be moved from this City along trade routes inside your Civilization.'
WHERE Tag = 'TXT_KEY_BUILDING_STONE_WORKS_STRATEGY';

UPDATE Language_en_US
SET Text = '+1 [ICON_FOOD] Food and +1 [ICON_GOLD] Gold from Coast and Ocean tiles. Internal [ICON_INTERNATIONAL_TRADE] Trade Routes from this City generate +4 [ICON_FOOD] Food. Increases [ICON_SILVER_FIST] Military Unit Supply Cap by 1.[NEWLINE][NEWLINE]Forms a [ICON_CONNECTED] City Connection with other Cities via water if they also have Lighthouses.[NEWLINE][NEWLINE]Can only be constructed in a Coastal City.'
WHERE Tag = 'TXT_KEY_BUILDING_LIGHTHOUSE_HELP';

UPDATE Language_en_US
SET Text = 'The Lighthouse can only be constructed in a City next to a Coast Tile. It increases the [ICON_FOOD] Food and [ICON_GOLD] Gold output of water tiles. Also allows for City Connections over Water (see Concepts for more details).'
WHERE Tag = 'TXT_KEY_BUILDING_LIGHTHOUSE_STRATEGY';

UPDATE Language_en_US
SET Text = 'If a City is connected by a road and/or [COLOR_YELLOW]Lighthouse[ENDCOLOR] to your Capital City (i.e. both cities have a Lighthouse), that City has a "trade route" with the Capital. Each trade route is worth a certain amount of gold each turn, the amount determined by the size of the two cities.'
WHERE Tag = 'TXT_KEY_GOLD_TRADE_ROUTES_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'City Connections'
WHERE Tag = 'TXT_KEY_GOLD_TRADE_ROUTES_HEADING3_TITLE';

UPDATE Language_en_US
SET Text = 'Completing a [ICON_CARGO_SHIP] Sea Trade Route originating here and targeting another Civilization, triggers a (or strengthens an existing) [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR], producing [ICON_TOURISM] Tourism with the targeted Civilization and one-third the amount with every other Civilization.[NEWLINE][NEWLINE]+1 [ICON_FOOD] Food from Coast and Ocean tiles, and +1 [ICON_PRODUCTION] Production from Sea Resources worked by this City. [ICON_CARGO_SHIP] Sea Trade Routes gain +50% Range and +2 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]+15% [ICON_PRODUCTION] Production of Naval Units, and +2 to [ICON_SILVER_FIST] Military Unit Supply Cap.[NEWLINE][NEWLINE]Can only be constructed in a Coastal City.'
WHERE Tag = 'TXT_KEY_BUILDING_HARBOR_HELP';

UPDATE Language_en_US
SET Text = 'The Harbor is a Medieval-era building, requiring a Lighthouse. It improves the range and [ICON_GOLD] Gold yield of sea trade routes, and boosts the value of sea tiles and resources. The Harbor also increases the [ICON_PRODUCTION] Production of Naval units by 15%.'
WHERE Tag = 'TXT_KEY_BUILDING_HARBOR_STRATEGY';

UPDATE Language_en_US
SET Text = '+1 [ICON_PRODUCTION] Production and +1 [ICON_GOLD] Gold from all Coast and Ocean tiles. +2 [ICON_PRODUCTION] Production and [ICON_GOLD] Gold from Sea Resources worked by this City. [ICON_SILVER_FIST] Military Units Supplied by this City''s population increased by 20%.[NEWLINE][NEWLINE]Requires 1 [ICON_RES_COAL] Coal. Can only be constructed in a Coastal City, and cannot have a [COLOR_NEGATIVE_TEXT]Train Station[ENDCOLOR] in the City.'
WHERE Tag = 'TXT_KEY_BUILDING_SEAPORT_HELP';

UPDATE Language_en_US
SET Text = 'Provides a massive boost to City Production and Sea Resource yields during the Industrial Era. Increases Military Units supplied by this City''s population by 20%. Requires a Harbor, and cannot be built in the same City as a Train Station.'
WHERE Tag = 'TXT_KEY_BUILDING_SEAPORT_STRATEGY';

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BUILDING_MARKET_HELP', 'Incoming [ICON_INTERNATIONAL_TRADE] Trade Routes generate +1 [ICON_GOLD] Gold for the City, and +1 [ICON_GOLD] Gold for [ICON_INTERNATIONAL_TRADE] Trade Route owner.[NEWLINE][NEWLINE]Nearby [ICON_RES_SPICES] Cinnamon: +1 [ICON_FOOD] Food, +1 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_SUGAR] Sugar: +1 [ICON_FOOD] Food, +1 [ICON_GOLD] Gold.');

UPDATE Language_en_US
SET Text = 'Markets increase the amount of [ICON_GOLD] Gold a City generates, and improves nearby [ICON_RES_SPICES] Cinnamon and [ICON_RES_SUGAR] Sugar. Incoming [ICON_INTERNATIONAL_TRADE] Trade Routes generate +1 [ICON_GOLD] Gold for the City, and +1 [ICON_GOLD] Gold for Trade Route owner.'
WHERE Tag = 'TXT_KEY_BUILDING_MARKET_STRATEGY';

UPDATE Language_en_US
SET Text = 'Incoming [ICON_INTERNATIONAL_TRADE] Trade Routes generate +2 [ICON_GOLD] Gold for the City, and +2 [ICON_GOLD] Gold for [ICON_INTERNATIONAL_TRADE] Trade Route owner.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty.[NEWLINE][NEWLINE]Nearby [ICON_RES_BANANA] Bananas: +2 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COFFEE] Coffee: +1 [ICON_GOLD] Gold, +2 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_TEA] Tea: +2 [ICON_GOLD] Gold, +1 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_TOBACCO] Tobacco: +3 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_MINT_HELP';

UPDATE Language_en_US
SET Text = 'The Customs House boosts the Gold value of Trade Routes, and boosts the Tourism you generate from Trade Routes to foreign Civilizations. Build these buildings in all Cities if you wish to improve your Gold output as well as the cultural value of your Trade Routes.'
WHERE Tag = 'TXT_KEY_BUILDING_MINT_STRATEGY';

UPDATE Language_en_US
SET Text = 'Customs House'
WHERE Tag = 'TXT_KEY_BUILDING_MINT';

UPDATE Language_en_US
SET Text = 'A custom house or customs house was a building housing the offices for the government officials who processed the paperwork for the import and export of goods into and out of a country. Customs officials also collected customs duty on imported goods. The custom house was typically located in a seaport or in a city on a major river with access to the ocean. These cities acted as a port of entry into a country. The government had officials at such locations to collect taxes and regulate commerce. Due to advances in electronic information systems, the increased volume of international trade and the introduction of air travel, the custom house is now often a historical anachronism. There are many examples of buildings around the world whose former use was as a custom house but that have since been converted for other use, such as museums or civic buildings.'
WHERE Tag = 'TXT_KEY_CIV5_BUILDINGS_MINT_TEXT';

UPDATE Language_en_US
SET Text = 'When you spend [ICON_GOLD] Gold to purchase Units or invest in Buildings in this City, 15% of the cost is converted into [ICON_RESEARCH] Science. Caravansaries and Customs Houses in the City gain +3 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]Nearby [ICON_RES_GOLD] Gold: +3 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_SILVER] Silver: +1 [ICON_GOLD] Gold, +1 [ICON_PRODUCTION] Production, +1 [ICON_CULTURE] Culture.[NEWLINE]Nearby [ICON_RES_GEMS] Gems: +2 [ICON_CULTURE] Culture, +1 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_BANK_HELP';

UPDATE Language_en_US
SET Text = 'The Bank is a Renaissance-era building which increases the City''s output of [ICON_GOLD] Gold, boosts the value of investments in the City, and reduces Poverty. The City must already possess a Market before a Bank can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_BANK_STRATEGY';

UPDATE Language_en_US
SET Text = '+1 [ICON_GOLD] Gold for every 2 [ICON_CITIZEN] Citizens in the City. Cost of [ICON_GOLD] Gold purchasing in this City reduced by 20%. +2 [ICON_GOLD] Gold on Towns and Villages worked by this City.'
WHERE Tag = 'TXT_KEY_BUILDING_STOCK_EXCHANGE_HELP';

UPDATE Language_en_US
SET Text = 'Completing a [ICON_CARAVAN] Land Trade Route originating here and targeting another Civilization, triggers a (or strengthens an existing) [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR], producing [ICON_TOURISM] Tourism with the targeted Civilization and one-third the amount with every other Civilization.[NEWLINE][NEWLINE]+1 [ICON_FOOD] Food and [ICON_GOLD] Gold for every 3 Desert or 3 Tundra tiles worked by the City. [ICON_CARAVAN] Land Trade Routes gain +50% Range and +3 [ICON_GOLD] Gold, [ICON_VP_MERCHANT] Merchants in this City gain +1 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]Nearby [ICON_RES_TRUFFLES] Truffles: +2 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COTTON] Cotton: +1 [ICON_PRODUCTION] Production, +1 [ICON_CULTURE] Culture.[NEWLINE]Nearby [ICON_RES_FUR] Furs: +1 [ICON_GOLD] Gold, +1 [ICON_PRODUCTION] Production.'
WHERE Tag = 'TXT_KEY_BUILDING_CARAVANSARY_HELP';

UPDATE Language_en_US
SET Text = '-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_RESEARCH] Illiteracy. 1 Specialist in this City no longer produces [ICON_HAPPINESS_3] Unhappiness from [ICON_URBANIZATION] Urbanization.'
WHERE Tag = 'TXT_KEY_BUILDING_LIBRARY_HELP';

UPDATE Language_en_US
SET Text = '+1 [ICON_RESEARCH] Science from Jungle and Snow tiles worked by the City. Gain 25% of the [ICON_RESEARCH] Science output of the City as an instant boost to your current Research when a [ICON_CITIZEN] Citizen is born in this City.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_RESEARCH] Illiteracy.'
WHERE Tag = 'TXT_KEY_BUILDING_UNIVERSITY_HELP';

UPDATE Language_en_US
SET Text = 'The University is a Medieval-era building. It increases the [ICON_RESEARCH] Science output of the City, and it''s even more effective if the City is near to Jungle or Snow tiles. The City must already possess a Library before a University can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_UNIVERSITY_STRATEGY';

UPDATE Language_en_US
SET Text = '+1 [ICON_RESEARCH] Science for every 2 [ICON_CITIZEN] Citizens in the City.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_RESEARCH] Illiteracy.'
WHERE Tag = 'TXT_KEY_BUILDING_PUBLIC_SCHOOL_HELP';

UPDATE Language_en_US
SET Text = 'The Public School is a mid-game building which increase a City''s output of [ICON_RESEARCH] based on the population of the City.  The City must already possess a Univeristy before a Public School can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_PUBLIC_SCHOOL_STRATEGY';

UPDATE Language_en_US
SET Text = '+4 [ICON_RESEARCH] Science from [ICON_RES_OIL] Oil, [ICON_RES_ALUMINUM] Aluminum, [ICON_RES_URANIUM] Uranium, and Academies worked by this City.[NEWLINE][NEWLINE]+4 [ICON_RESEARCH] Science from Hospitals, Factories, and Medical Labs, and [ICON_GREAT_SCIENTIST] Great Scientists are produced 33% more quickly in this City.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_RESEARCH] Illiteracy.'
WHERE Tag = 'TXT_KEY_BUILDING_LABORATORY_HELP';

UPDATE Language_en_US
SET Text = REPLACE(Text, 'city', 'City')
WHERE Tag = 'TXT_KEY_BUILDING_LABORATORY_STRATEGY';

UPDATE Language_en_US
SET Text = REPLACE(Text, 'a Public School to construct the Research Laboratory', 'a Public School before Research Laboratory can be constructed')
WHERE Tag = 'TXT_KEY_BUILDING_LABORATORY_STRATEGY';

UPDATE Language_en_US
SET Text = 'City gains +1 [ICON_RESEARCH] Science for every Mountain within the workable tiles.[NEWLINE][NEWLINE]Requires [COLOR_MAGENTA]Scientific Revolution[ENDCOLOR] Policy.'
WHERE Tag = 'TXT_KEY_BUILDING_OBSERVATORY_HELP';

UPDATE Language_en_US
SET Text = 'The Observatory increases [ICON_RESEARCH] Science output, especially for Cities near lots of Mountains. Requires ''Scientific Revolution'' Policy to unlock.'
WHERE Tag = 'TXT_KEY_BUILDING_OBSERVATORY_STRATEGY';

UPDATE Language_en_US
SET Text = '+15% [ICON_PRODUCTION] Production when constructing Buildings. Grocer and Granary in the City produce +1 [ICON_FOOD] Food. Nearby Marshes and Lakes produce +2 [ICON_PRODUCTION] Production and [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_WINDMILL_HELP';

UPDATE Language_en_US
SET Text = 'The Windmill is a Renaissance-era building which increases the [ICON_PRODUCTION] Production output of a City when constructing buildings.'
WHERE Tag = 'TXT_KEY_BUILDING_WINDMILL_STRATEGY';

UPDATE Language_en_US
SET Text = 'All City Processes are 10% more efficient at converting [ICON_PRODUCTION] Production into Yields.[NEWLINE][NEWLINE]+3 [ICON_PRODUCTION] Production, [ICON_RESEARCH] Science, and [ICON_FOOD] Food on every Lake Tile and tile next to a River.[NEWLINE][NEWLINE]Requires 1 [ICON_RES_ALUMINUM] Aluminum to be built. City must be built next to a River. City must not contain another energy-producing Plant.'
WHERE Tag = 'TXT_KEY_BUILDING_HYDRO_PLANT_HELP';

UPDATE Language_en_US
SET Text = 'Hydroelectric Power is a power system that greatly improves the yield output of freshwater tiles. Construct this building in cities with lots of rivers and lakes.'
WHERE Tag = 'TXT_KEY_BUILDING_HYDRO_PLANT_STRATEGY';

UPDATE Language_en_US
SET Text = 'Hydroelectric Power Plant'
WHERE Tag = 'TXT_KEY_BUILDING_HYDRO_PLANT';

UPDATE Language_en_US
SET Text = 'All City Processes are 10% more efficient at converting [ICON_PRODUCTION] Production into Yields.[NEWLINE][NEWLINE]+25% [ICON_GREAT_PEOPLE] Great Person Rate in this City, and all Specialists generate +1 [ICON_PRODUCTION] Production, [ICON_GOLD] Gold, [ICON_RESEARCH] Science, and [ICON_CULTURE] Culture.[NEWLINE][NEWLINE]Requires 1 [ICON_RES_URANIUM] Uranium. City must not contain another energy-producing Plant.'
WHERE Tag = 'TXT_KEY_BUILDING_NUCLEAR_PLANT_HELP';

UPDATE Language_en_US
SET Text = 'Nuclear Power Plant'
WHERE Tag = 'TXT_KEY_BUILDING_NUCLEAR_PLANT';

UPDATE Language_en_US
SET Text = 'All City Processes are 10% more efficient at converting [ICON_PRODUCTION] Production into Yields.[NEWLINE][NEWLINE]+3 [ICON_PRODUCTION] Production and [ICON_RESEARCH] Science on every Desert Tile, and +1 [ICON_PRODUCTION] Production and [ICON_RESEARCH] Science on other types of Land tiles.[NEWLINE][NEWLINE]Requires [ICON_RES_ALUMINUM] Aluminum to be built. City must be built on or next to Desert. City must not contain another energy-producing Plant.'
WHERE Tag = 'TXT_KEY_BUILDING_SOLAR_PLANT_HELP';

UPDATE Language_en_US
SET Text = 'Solar Power is an Atomic-era electrical system which can only be constructed in a City near a Desert. The Solar Farm increases the [ICON_PRODUCTION] Production and [ICON_RESEARCH] Science output of every land tile the City works, but provides triple yields on Desert tiles. The Solar Farm also increases the efficiency of working processes.'
WHERE Tag = 'TXT_KEY_BUILDING_SOLAR_PLANT_STRATEGY';

UPDATE Language_en_US
SET Text = 'Solar Farm'
WHERE Tag = 'TXT_KEY_BUILDING_SOLAR_PLANT';

UPDATE Language_en_US
SET Text = 'Eliminates extra [ICON_HAPPINESS_4] Unhappiness from an [ICON_OCCUPIED] Occupied City. Cost increases based on the number of cities in your empire.'
WHERE Tag = 'TXT_KEY_BUILDING_COURTHOUSE_HELP';

UPDATE Language_en_US
SET Text = 'Walls increase a City Defense Strength and Hit Points, making the City more difficult to capture. Increases Military Units supplied by this City''s population by 10%, and increases the City''s Ranged Strike range by 1. Also helps with managing the Empire Size Modifier in this City. Walls are quite useful for cities located along a frontier.'
WHERE Tag = 'TXT_KEY_BUILDING_WALLS_STRATEGY';

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BUILDING_WALLS_HELP', 'Increases the City''s [ICON_RANGE_STRENGTH] Ranged Strike Range by 1. [ICON_SILVER_FIST] Military Units Supplied by this City''s population increased by 10%.[NEWLINE][NEWLINE][ICON_CITY_STATE] Empire Size Modifier is reduced by 5% in this City.');

UPDATE Language_en_US
SET Text = 'The Castle is a Medieval-era building which increases Defensive Strength and reduces all incoming damage by 2. Increases Military Units supplied by this City''s population by 10%, and increases Production for all nearby Quarries by 1. Also helps with managing the Empire Size Modifier in this City. The City must already possess Walls before a Castle can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_CASTLE_STRATEGY';

UPDATE Language_en_US
SET Text = 'Reduces the city''s damage taken from attacks by 2. +1 [ICON_PRODUCTION] Production for Quarries worked by this City. [ICON_SILVER_FIST] Military Units Supplied by this City''s population increased by 10%. Contains 1 slot for a [ICON_GREAT_WORK] Great Work of Art or Artifact.[NEWLINE][NEWLINE][ICON_CITY_STATE] Empire Size Modifier is reduced by 5% in this City.'
WHERE Tag = 'TXT_KEY_BUILDING_CASTLE_HELP';

UPDATE Language_en_US
SET Text = 'The Arsenal is a Modern-era military building that increases Defense Strength and Hit Points, making the City more difficult to capture. Increases the City''s [ICON_RANGE_STRENGTH] Ranged Strike Range by 1. Garrisoned units receive an additional 5 Health when healing in this City. Increases Military Units supplied by this City''s population by 10%. Also helps with managing the Empire Size Modifier in this City. The City must already possess a Bastion Fort before an Arsenal can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_ARSENAL_STRATEGY';

UPDATE Language_en_US
SET Text = '[ICON_SILVER_FIST] Military Units Supplied by this City''s population increased by 10%. Increases the City''s [ICON_RANGE_STRENGTH] Ranged Strike Range by 1.[NEWLINE][NEWLINE]Garrisoned Units receive an additional 5 Health when healing in this City.[NEWLINE][NEWLINE][ICON_CITY_STATE] Empire Size Modifier is reduced by 5% in this City.'
WHERE Tag = 'TXT_KEY_BUILDING_ARSENAL_HELP';

UPDATE Language_en_US
SET Text = '+25% [ICON_PRODUCTION] Production of Air Units, +15 [ICON_STRENGTH] Damage to Air Units during Air Strikes on City. Improves City [ICON_SPY] Spy Resistance by 50%. Increases the City''s [ICON_RANGE_STRENGTH] Ranged Strike Damage by 10%. [ICON_SILVER_FIST] Military Units Supplied by this City''s population increased by 20%, and garrisoned Units receive an additional 20 Health when healing in this City.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD] and [ICON_PRODUCTION] Distress. [ICON_CITY_STATE] Empire Size Modifier is reduced by 5% in this City.'
WHERE Tag = 'TXT_KEY_BUILDING_MILITARY_BASE_HELP';

UPDATE Language_en_US
SET Text = 'The Military Base is a late-game building which increases Defensive Strength and Hit Points, and improves defense against air units. Garrisoned units receive an additional 20 Health when healing in this City. Increases Military Units supplied by this City''s population by 20%. Also helps with managing the Empire Size Modifier in this City. The City must already possess an Arsenal before a Military Base can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_MILITARY_BASE_STRATEGY';

UPDATE Language_en_US
SET Text = '50% chance to detonate nuclear weapons [COLOR_POSITIVE_TEXT]early[ENDCOLOR]. Early detonations destroy Atomic Bombs outright and make Nuclear Missiles only as effective as Atomic Bombs. Reduces population loss from nuclear attack by 75%. +10 [ICON_STRENGTH] Damage to Air Units during Air Strikes on City.'
WHERE Tag = 'TXT_KEY_BUILDING_BOMB_SHELTER_HELP';

UPDATE Language_en_US
SET Text = 'Strategic Defense Systems have a 50% chance to detonate nuclear weapons early, which destroys Atomic Bombs outright and makes Nuclear Missiles only as effective as Atomic Bombs. They also reduce population loss from a nuclear attack on this City by 75% if a missile does strike, and deal 10 Damage to Air Units which strike this City. Duck and Cover!'
WHERE Tag = 'TXT_KEY_BUILDING_BOMB_SHELTER_STRATEGY';

UPDATE Language_en_US
SET Text = 'Strategic Defense System'
WHERE Tag = 'TXT_KEY_BUILDING_BOMB_SHELTER';

UPDATE Language_en_US
SET Text = 'The Strategic Defense Initiative (SDI) was a proposed missile defense system intended to protect the United States from attack by ballistic strategic nuclear weapons (intercontinental ballistic missiles and submarine-launched ballistic missiles). The concept was first announced publicly by President Ronald Reagan on 23 March 1983. Reagan was a vocal critic of the doctrine of mutual assured destruction (MAD), which he described as a ''suicide pact,'' and he called upon the scientists and engineers of the United States to develop a system that would render nuclear weapons obsolete.'
WHERE Tag = 'TXT_KEY_CIV5_BUILDINGS_BOMB_SHELTER_TEXT';

UPDATE Language_en_US
SET Text = '+1 [ICON_PRODUCTION] Production for every 4 [ICON_CITIZEN] Citizens in the City. Manufactories worked by the City gain +2 [ICON_PRODUCTION] Production, and all owned Factories gain +2 [ICON_PRODUCTION] Production. [NEWLINE][NEWLINE]1 Specialist in this City no longer produces [ICON_HAPPINESS_3] Unhappiness from [ICON_URBANIZATION] Urbanization.[NEWLINE][NEWLINE]Nearby [ICON_RES_COAL] Coal: +2 [ICON_PRODUCTION] Production, +2 [ICON_RESEARCH] Science.[NEWLINE][NEWLINE]Requires 1 [ICON_RES_COAL] Coal. '
WHERE Tag = 'TXT_KEY_BUILDING_FACTORY_HELP';

UPDATE Language_en_US
SET Text = '+50% [ICON_PRODUCTION] Production when building Spaceship Parts.[NEWLINE][NEWLINE]Requires 1 [ICON_RES_ALUMINUM] Aluminum.'
WHERE Tag = 'TXT_KEY_BUILDING_SPACESHIP_FACTORY_HELP';

UPDATE Language_en_US
SET Text = 'The Spaceship Factory increases the speed at which a City constructs spaceship parts, and greatly boosts Science in the City. The Spaceship Factory requires one [ICON_RES_ALUMINUM] Aluminum resource, and the City must already possess a Factory before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_SPACESHIP_FACTORY_STRATEGY';

UPDATE Language_en_US
SET Text = '25% of the [ICON_CULTURE] Culture from [ICON_WONDER] World Wonders and Tiles is added to the [ICON_TOURISM] Tourism output of the City. [ICON_TOURISM] Tourism output from Great Works +25%.'
WHERE Tag = 'TXT_KEY_BUILDING_HOTEL_HELP';

UPDATE Language_en_US
SET Text = 'Allows [COLOR_POSITIVE_TEXT]Airlifts[ENDCOLOR] to or from this City. [COLOR_POSITIVE_TEXT]Increases Air Unit Capacity of the City from 2 to 6.[ENDCOLOR] +10 [ICON_STRENGTH] Damage to Air Units during Air Strikes on City.'
WHERE Tag = 'TXT_KEY_BUILDING_AIRPORT_HELP';

UPDATE Language_en_US
SET Text = '+100 [ICON_CULTURE] Culture when completed, and starts 10 turns of "We Love the King Day" in the City.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom.[NEWLINE][NEWLINE]Nearby [ICON_RES_IVORY] Ivory: +3 [ICON_CULTURE] Culture.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_HELP';

UPDATE Language_en_US
SET Text = 'The Circus reduces Boredom in a City and improves the Culture output of [ICON_RES_IVORY] Ivory. It also starts 10 turns of "We Love the King Day" in the City. Build these to combat Unhappiness from Boredom, and gain quick bursts of Culture.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_STRATEGY';

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BUILDING_COLOSSEUM_HELP', '+1 [ICON_TOURISM] Tourism for every 4 [ICON_CITIZEN] Citizens in the City. Barracks, Forge, and Armory in this City gain +2 [ICON_PRODUCTION] Production. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom.[NEWLINE][NEWLINE]Nearby [ICON_RES_PERFUME] Perfume: +1 [ICON_GOLD] Gold, +1 [ICON_CULTURE] Culture.[NEWLINE]Nearby [ICON_RES_OLIVE] Olives: +1 [ICON_GOLD] Gold, +1 [ICON_FOOD] Food.');

UPDATE Language_en_US
SET Text = 'The Arena provides additional Culture, and generates Tourism from City''s population. Reduces Boredom in a City, and improves the output of nearby [ICON_RES_PERFUME] Perfume and [ICON_RES_OLIVE] Olives. Build these to combat Unhappiness from Boredom, to increase your Culture, and to improve the production of your military buildings.'
WHERE Tag = 'TXT_KEY_BUILDING_COLOSSEUM_STRATEGY';

UPDATE Language_en_US
SET Text = 'An arena is an enclosed area, often circular or oval-shaped, designed to showcase theater, musical performances, or sporting events. The word derives from Latin harena, a particularly fine/smooth sand used to absorb blood in ancient arenas such as the Colosseum in Rome. It is composed of a large open space surrounded on most or all sides by tiered seating for spectators. The key feature of an arena is that the event space is the lowest point, allowing for maximum visibility. Arenas are usually designed to accommodate a large number of spectators.'
WHERE Tag = 'TXT_KEY_CIV5_BUILDINGS_COLISEUM_TEXT';

UPDATE Language_en_US
SET Text = 'Arena'
WHERE Tag = 'TXT_KEY_BUILDING_COLOSSEUM';

UPDATE Language_en_US
SET Text = '+500 [ICON_RESEARCH] Science when completed. Nearby Jungle and Forest tiles gain +1 [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism.[NEWLINE][NEWLINE]Upon completing a [ICON_INTERNATIONAL_TRADE] Trade Route originating here and targeting another Civilization, triggers a (or strengthens an existing) Historic Event, producing [ICON_TOURISM] Tourism with the targeted Civilization and one third the amount with every other Civilization.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom.'
WHERE Tag = 'TXT_KEY_BUILDING_THEATRE_HELP';

UPDATE Language_en_US
SET Text = 'The Zoo reduces Boredom in a City, produces additional Culture, and boosts the Culture value of nearby Jungle and Forest tiles.'
WHERE Tag = 'TXT_KEY_BUILDING_THEATRE_STRATEGY';

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Globe Theatre[ENDCOLOR], a building which increases [ICON_TOURISM] Tourism. Also enables the founding of the World Congress.'
WHERE Tag = 'TXT_KEY_TECH_PRINTING_PRESS_HELP';

-- Stadium (BuildingClasses table was pointing to the wrong TXT_KEY)
UPDATE BuildingClasses
SET Description = 'TXT_KEY_BUILDING_STADIUM_DESC'
WHERE Type = 'BUILDINGCLASS_STADIUM';

UPDATE Language_en_US
SET Text = 'Build the Stadium if you are going for a [COLOR_POSITIVE_TEXT]Culture Victory[ENDCOLOR]. Increases your influence with all City-States by 20 when completed. Coordinate the construction of Stadiums across your empire for a global boost in diplomatic relations.'
WHERE Tag = 'TXT_KEY_BUILDING_STADIUM_STRATEGY';

UPDATE Language_en_US
SET Text = '+20 [ICON_INFLUENCE] Influence with all City-States when completed. +1 [ICON_GOLDEN_AGE] Golden Age Point for every 2 [ICON_CITIZEN] Citizens in the City.[NEWLINE][NEWLINE][ICON_TOURISM] Tourism output from [ICON_GREAT_WORK] Great Works increased by +50%. Requires a Zoo in the City.'
WHERE Tag = 'TXT_KEY_BUILDING_STADIUM_HELP';

UPDATE Language_en_US
SET Text = 'Improves City [ICON_SPY] Spy Resistance by 50%. Increases mission duration by 1 turn in the City for Foreign Spies. When you successfully counter a foreign [ICON_SPY] Espionage action in this City, gain 25 [ICON_CULTURE] Culture and [ICON_GOLD] Gold, scaling with Era and the Level of the foreign [ICON_SPY] Spy.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD] and [ICON_PRODUCTION] Distress, [ICON_GOLD] Poverty, [ICON_RESEARCH] Illiteracy, and [ICON_CULTURE] Boredom.'
WHERE Tag = 'TXT_KEY_BUILDING_POLICE_STATION_HELP';

UPDATE Language_en_US
SET Text = 'Improves City [ICON_SPY] Spy Resistance by 50%. Increases mission duration by 1 turn in the City for Foreign Spies.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD] and [ICON_PRODUCTION] Distress. '
WHERE Tag = 'TXT_KEY_BUILDING_CONSTABLE_HELP';

UPDATE Language_en_US
SET Text = 'Provides 5 [ICON_GREAT_PEOPLE] Great Person Points (GPP) towards a [ICON_GREAT_MUSICIAN] Great Musician. Add up to 2 Specialists to this Building to gain [ICON_CULTURE] Culture and increase the rate of [ICON_GREAT_MUSICIAN] Great Musician acquisition.[NEWLINE][NEWLINE]1 Specialist in this City no longer produces [ICON_HAPPINESS_3] Unhappiness from [ICON_URBANIZATION] Urbanization.[NEWLINE][NEWLINE]Maximum of 3 of these Buildings in your Empire.'
WHERE Tag = 'TXT_KEY_BUILDING_MUSICIANS_GUILD_HELP';

UPDATE Language_en_US
SET Text = 'Provides 4 [ICON_GREAT_PEOPLE] Great Person Points (GPP) towards a [ICON_GREAT_ARTIST] Great Artist. Add up to 2 Specialists to this Building to gain [ICON_CULTURE] Culture and increase the rate of [ICON_GREAT_ARTIST] Great Artist acquisition.[NEWLINE][NEWLINE]1 Specialist in this City no longer produces [ICON_HAPPINESS_3] Unhappiness from [ICON_URBANIZATION] Urbanization.[NEWLINE][NEWLINE]Maximum of 3 of these Buildings in your Empire.'
WHERE Tag = 'TXT_KEY_BUILDING_ARTISTS_GUILD_HELP';

UPDATE Language_en_US
SET Text = 'Provides 3 [ICON_GREAT_PEOPLE] Great Person Points (GPP) towards a [ICON_GREAT_WRITER] Great Writer. Add up to 2 Specialists to this Building to gain [ICON_CULTURE] Culture and increase the rate of [ICON_GREAT_WRITER] Great Writer acquisition.[NEWLINE][NEWLINE]1 Specialist in this City no longer produces [ICON_HAPPINESS_3] Unhappiness from [ICON_URBANIZATION] Urbanization.[NEWLINE][NEWLINE]Maximum of 3 of these Buildings in your Empire.'
WHERE Tag = 'TXT_KEY_BUILDING_WRITERS_GUILD_HELP';

UPDATE Language_en_US
SET Text = 'National Monument'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_EPIC';

UPDATE Language_en_US
SET Text = 'A National monument is a monument constructed in order to commemorate something of national importance such as a war or the founding of the country. The term may also refer to a specific monument status, such as a national heritage site, which most national monuments are by reason of their cultural importance rather than age. The National monument aims to represent the nation, and serve as a focus for national identity.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_EPIC_PEDIA';

UPDATE Language_en_US
SET Text = 'School of Philosophy'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_COLLEGE';

UPDATE Language_en_US
SET Text = 'Philosophy is the study of general and fundamental problems, such as those connected with reality, existence, knowledge, values, reason, mind, and language. Philosophy is organized into schools of thought and distinguished from other ways of addressing such problems by its critical, generally systematic approach and its reliance on rational argument. In more casual speech, by extension, philosophy can refer to the most basic beliefs, concepts, and attitudes of an individual or group. The word philosophy comes from the Ancient Greek philosophia, which literally means "love of wisdom". The introduction of the terms philosopher and philosophy has been ascribed to the Greek thinker Pythagoras.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_COLLEGE_PEDIA';

UPDATE Language_en_US
SET Text = 'Royal Collection'
WHERE Tag = 'TXT_KEY_BUILDING_HERMITAGE';

UPDATE Language_en_US
SET Text = 'The largest private collections of fine art pieces have historically accrued to royalty, who purchase and patronize artistic works as an exercise of their prestige. Many of the largest art collections in the world have started from the private collections of monarchs which were handed down for generations. In response to public pressure and popular desire for a stronger national identity, many royal collections (and often the private residences that house them) have been nationalized by their respective governments to form State Galleries and National Art Museums in the modern day.'
WHERE Tag = 'TXT_KEY_BUILDING_HERMITAGE_PEDIA';

UPDATE Language_en_US
SET Text = 'This National Wonder gives all land units built in this City (past and future) the "Morale" promotion, increasing their combat strength by +10%. +1 [ICON_PRODUCTION] Production in the City for every 5 [ICON_CITIZEN] Citizens. Increases Military Units supplied by this City''s population by 10%. Also creates a free Great Writer upon completion. The City must have a Barracks before it can construct the Heroic Epic.'
WHERE Tag = 'TXT_KEY_BUILDING_HEROIC_EPIC_STRATEGY';

UPDATE Language_en_US
SET Text = 'This National Wonder increases the [ICON_GREAT_PEOPLE] Great People generation of a City by 25%. Receive [ICON_CULTURE] Culture when a [ICON_CITIZEN] Citizen is born in the City, and [ICON_GOLDEN_AGE] Golden Age Points whenever you unlock a policy. It also provides +1 [ICON_CULTURE] Culture. The City must have a Monument before it can construct the National Monument.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_EPIC_STRATEGY';

UPDATE Language_en_US
SET Text = 'This National Wonder provides +2 [ICON_HAPPINESS_1] Happiness, 2 [ICON_RES_HORSE] Horses, +1 [ICON_CULTURE] Culture, and reduces [ICON_HAPPINESS_3] Boredom, and generates +10% [ICON_CULTURE] Culture and [ICON_GOLD] Gold during "We Love the King Day" in the City where it is built. The City must have an Arena before it can construct the Circus Maximus.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_MAXIMUS_STRATEGY';

UPDATE Language_en_US
SET Text = 'The East India Company increases the amount of [ICON_GOLD] Gold a City generates and reduces [ICON_HAPPINESS_3] Poverty. Resource Diversity Modifiers for Trade Routes from this City increase by 25% if positive, and decrease by 25% if negative. You also receive a free copy of all Luxury Resources around the City.[NEWLINE][NEWLINE]Trade routes other players make to a City with an East India Company will generate an extra 4 [ICON_GOLD] Gold for the City owner and the trade route owner gains an additional 2 [ICON_GOLD] Gold for the trade route.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_TREASURY_STRATEGY';

UPDATE Language_en_US
SET Text = 'The School of Philosophy National Wonder produces [ICON_RESEARCH] Science and [ICON_CULTURE] Culture, especially during [ICON_GOLDEN_AGE] Golden Ages. The City must have a Library before it can construct the School of Philosophy.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_COLLEGE_STRATEGY';

UPDATE Language_en_US
SET Text = 'The Ironworks National Wonder provides 2 [ICON_RES_IRON] Iron, increases [ICON_PRODUCTION] Production in a City by 10, and generates [ICON_RESEARCH] Science every time you construct a building. The City must have a Forge before it can construct an Ironworks.'
WHERE Tag = 'TXT_KEY_BUILDING_IRONWORKS_STRATEGY';

UPDATE Language_en_US
SET Text = 'The Oxford University National Wonder provides +1 [ICON_CULTURE] Culture, and reduces [ICON_HAPPINESS_3] Illiteracy. +50 [ICON_CULTURE] Culture every time you research a Technology. The City must have a University before it can construct Oxford University.'
WHERE Tag = 'TXT_KEY_BUILDING_OXFORD_UNIVERSITY_STRATEGY';

UPDATE Language_en_US
SET Text = 'This National Wonder produces +1 [ICON_CULTURE] Culture for every 4 [ICON_CITIZEN] Citizens in the City, and +10% [ICON_CULTURE] Culture for the City. The City must have a Gallery before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_HERMITAGE_STRATEGY';

UPDATE Language_en_US
SET Text = 'An important defensive National Wonder for a technology-driven civilization. Reduces [ICON_HAPPINESS_3] Distress in all Cities. The National Intelligence Agency provides an additional spy, levels up all your existing spies, and improves City Spy Resistance by 15%. It also decreases mission duration by 1 turn for your Spies and increases mission duration by 2 turns for enemy Spies in the City. Empires with a lot of offensive spies will benefit greatly from this building. The City must have a Police Station before it can construct the National Intelligence Agency.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_STRATEGY';

UPDATE Language_en_US
SET Text = 'An important building for a civilization trying to spread their religion world-wide from an empire with few, populous cities. A City must have a Temple before it can construct the Grand Temple.'
WHERE Tag = 'TXT_KEY_BUILDING_GRAND_TEMPLE_STRATEGY';

-- Help Text

UPDATE Language_en_US
SET Text = 'Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_WRITER] Great Writer in the City in which it is built. All Land Units created by this City (past or future) receive the [COLOR_POSITIVE_TEXT]Morale[ENDCOLOR] Promotion, which gives +10% [ICON_STRENGTH] Combat Strength. [ICON_SILVER_FIST] Military Units Supplied by this City''s population increased by 10%. Contains 1 slot for a [ICON_GREAT_WORK] Great Work of Writing.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_HEROIC_EPIC_HELP';

UPDATE Language_en_US
SET Text = '+25% [ICON_GREAT_PEOPLE] Great People generation in this City. Receive 15 [ICON_CULTURE] Culture when a [ICON_CITIZEN] Citizen is born in the City, and 50 [ICON_GOLDEN_AGE] Golden Age Points whenever you unlock a Policy, scaling with Era. Contains 1 slot for a [ICON_GREAT_WORK] Great Work of Art or an Artifact.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_EPIC_HELP';

-- Removed +2 Happiness because already present
UPDATE Language_en_US
SET Text = 'Provides 2 [ICON_RES_HORSE] Horses. +10% [ICON_CULTURE] Culture and [ICON_GOLD] Gold during "We Love the King Day" in the City in which it is built.[NEWLINE][NEWLINE]All Arenas in the Empire gain +2 [ICON_GOLD] Gold. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_MAXIMUS_HELP';

UPDATE Language_en_US
SET Text = 'Receive an additional copy of all Luxury Resources around this City. Incoming [ICON_INTERNATIONAL_TRADE] Trade Routes generate +4 [ICON_GOLD] Gold for the City, and +2 [ICON_GOLD] Gold for [ICON_INTERNATIONAL_TRADE] Trade Route owner. Resource Diversity Modifier for Trade Routes from this City increases by 25%. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty in all Cities.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_TREASURY_HELP';

UPDATE Language_en_US
SET Text = '+20% [ICON_RESEARCH] Science in the City during [ICON_GOLDEN_AGE] Golden Ages. 1 Specialist in this City no longer produces [ICON_HAPPINESS_3] Unhappiness from [ICON_URBANIZATION] Urbanization. Contains 1 slot for a [ICON_GREAT_WORK] Great Work of Writing.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_RESEARCH] Illiteracy in all Cities.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_COLLEGE_HELP';

UPDATE Language_en_US
SET Text = 'Provides 2 [ICON_RES_IRON] Iron. +25 [ICON_RESEARCH] Science when you construct a Building in this City. Bonus scales with Era.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_IRONWORKS_HELP';

UPDATE Language_en_US
SET Text = 'A [ICON_GREAT_SCIENTIST] Great Scientist appears near the City. +50 [ICON_CULTURE] Culture every time you research a Technology, scaling with Era. Contains 2 slots for [ICON_GREAT_WORK] Great Works of Writing. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_RESEARCH] Illiteracy.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.[NEWLINE][NEWLINE]+4 [ICON_RESEARCH] Science if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_BUILDING_OXFORD_UNIVERSITY_HELP';

UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture for every 4 [ICON_CITIZEN] Citizens in the City. +10% [ICON_CULTURE] Culture in this City. Contains 3 slots for [ICON_GREAT_WORK] Great Works of Art.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom in all Cities.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.[NEWLINE][NEWLINE]+4 [ICON_GOLD] Gold and [ICON_CULTURE] Culture if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_BUILDING_HERMITAGE_HELP';

UPDATE Language_en_US
SET Text = 'Improves City [ICON_SPY] Spy Resistance by 15%, provides 1 or more [COLOR_POSITIVE_TEXT]Additional[ENDCOLOR] [ICON_SPY] Spies (based on number of [ICON_CITY_STATE] City-States in game), and levels up all existing [ICON_SPY] Spies. Spy mission duration decreased by 1 turn for your Spies and increased by 2 turns for enemy Spies in the City. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD] and [ICON_PRODUCTION] Distress in all Cities.[NEWLINE][NEWLINE]+100 [ICON_RESEARCH] Science and [ICON_GOLD] Gold when you successfully complete a [ICON_SPY] Spy Mission in a foreign City, scaling with Era.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_HELP';

UPDATE Language_en_US
SET Text = 'All Temples gain +2 [ICON_PEACE] Faith and +1 [ICON_GOLD] Gold. -2 [ICON_HAPPINESS_3] Unhappiness from Religious Unrest in this City, and -1 in all other Cities.[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_GRAND_TEMPLE_HELP';

UPDATE Language_en_US
SET Text = 'Can only be constructed in a Coastal City. Gains 1 [COLOR_POSITIVE_TEXT]Additional[ENDCOLOR] [ICON_INTERNATIONAL_TRADE] Trade Route slot and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Cargo Ship in the City in which it is built. Incoming [ICON_INTERNATIONAL_TRADE] Trade Routes generate +2 [ICON_GOLD] Gold for the City, and +1 [ICON_GOLD] Gold for [ICON_INTERNATIONAL_TRADE] Trade Route owner.'
WHERE Tag = 'TXT_KEY_WONDER_COLOSSUS_HELP';

UPDATE Language_en_US
SET Text = 'A Worker appears near the City. Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Barracks in the City in which it is built. All Units gain +25% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR]. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD] and [ICON_PRODUCTION] Distress in this City.'
WHERE Tag = 'TXT_KEY_WONDER_STATUE_ZEUS_HELP';

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BUILDING_STONEHENGE_HELP', 'Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Council and 60 [ICON_PEACE] Faith in the City in which it is built.');

UPDATE Language_en_US
SET Text = 'Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Stone Works in the City in which it is built. "We Love the King Day" begins in the City. During "We Love the King Day", City [ICON_PRODUCTION] Production, [ICON_RESEARCH] Science, and [ICON_GOLD] Gold increase by +10%.'
WHERE Tag = 'TXT_KEY_WONDER_MAUSOLEUM_HALICARNASSUS_HELP';

UPDATE Language_en_US
SET Text = 'Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Herbalist in the City in which it is built. +10% [ICON_FOOD] Food in all Cities. +25% [ICON_PRODUCTION] Production when building Ranged Units in this City. 2 Specialists in this City no longer produce [ICON_HAPPINESS_3] Unhappiness from [ICON_URBANIZATION] Urbanization.'
WHERE Tag = 'TXT_KEY_WONDER_TEMPLE_ARTEMIS_HELP';

UPDATE Language_en_US
SET Text = 'Can only be built in a City on or next to Desert. Gains 1 [COLOR_POSITIVE_TEXT]Additional[ENDCOLOR] [ICON_INTERNATIONAL_TRADE] Trade Route slot and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Caravan appears in the City in which it is built. +1 [ICON_GOLD] Gold on all Desert tiles worked by this City. +6 [ICON_CULTURE] Culture once [COLOR_CYAN]Archaeology[ENDCOLOR] is discovered.'
WHERE Tag = 'TXT_KEY_WONDER_PETRA_HELP';

UPDATE Language_en_US
SET Text = 'Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Technology and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Library in the City in which it is built. Contains 2 slots for [ICON_GREAT_WORK] Great Works of Writing. +3 [ICON_RESEARCH] Science if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_GREAT_LIBRARY_HELP';

UPDATE Language_en_US
SET Text = 'Can only be constructed in a Coastal City. Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Lighthouse in the City in which it is built. All Military Naval Units receive +1 [ICON_MOVES] Movement and +1 Sight.'
WHERE Tag = 'TXT_KEY_WONDER_GREAT_LIGHTHOUSE_HELP';

UPDATE Language_en_US
SET Text = 'Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Garden in the City in which it is built.'
WHERE Tag = 'TXT_KEY_WONDER_HANGING_GARDEN_HELP';

UPDATE Language_en_US
SET Text = 'Tile improvement construction speed increased by 25%. +10 [ICON_CULTURE] Culture when you destroy an enemy Unit in battle, scaling with Era. Increases [ICON_SILVER_FIST] Military Unit Supply Cap by 5.'
WHERE Tag = 'TXT_KEY_WONDER_TERRA_COTTA_ARMY_HELP';

UPDATE Language_en_US
SET Text = 'Increases [ICON_SILVER_FIST] Military Units Supplied by this City''s population by 10%. Contains a prebuilt [ICON_GREAT_WORK] Great Work of Art in one of the [ICON_GREAT_WORK] Great Work slots. All owned Amphitheaters gain +1 [ICON_CULTURE] Culture and +1 [ICON_RESEARCH] Science. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom in this City. +3 [ICON_CULTURE] Culture if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_PARTHENON_HELP';

UPDATE Language_en_US
SET Text = 'Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Temple in the City in which it is built. Instantly receive 400 [ICON_CULTURE] Culture and [ICON_RESEARCH] Science. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_RESEARCH] Illiteracy in the City.'
WHERE Tag = 'TXT_KEY_WONDER_ORACLE_HELP';

UPDATE Language_en_US
SET Text = 'Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Mandir in the City in which it is built. +50% Faster [ICON_CULTURE_LOCAL] Border Growth and the [ICON_GOLD] Gold cost of acquiring new tiles is reduced by 25% in every City.'
WHERE Tag = 'TXT_KEY_WONDER_ANGKOR_WAT_HELP';

UPDATE Language_en_US
SET Text = 'Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_GENERAL] Great General and [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Walls in the City in which it is built. Enemy Land Units expend all [ICON_MOVES] Movement when entering your territory by land. Increases [ICON_SILVER_FIST] Military Unit Supply Cap by 3. All of the bonuses obsolete upon discovering [COLOR_NEGATIVE_TEXT]Gunpowder[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_GREAT_WALL_HELP';

UPDATE Language_en_US
SET Text = 'Requires completion of [COLOR_MAGENTA]Authority[ENDCOLOR] Branch. Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Castle in the City in which it is built. All Mounted Melee units receive the [COLOR_POSITIVE_TEXT]Jinete[ENDCOLOR] Promotion, improving [ICON_STRENGTH] Combat Strength when attacking and providing a chance to withdraw from combat. Boosts City [ICON_CULTURE] Culture output by 10%.'
WHERE Tag = 'TXT_KEY_WONDER_ALHAMBRA_HELP';

-- University of Sankore
UPDATE Language_en_US
SET Text = 'Requires completion of [COLOR_MAGENTA]Tradition[ENDCOLOR] Branch. Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Mosque in the City in which it is built. Also, when you expend a [ICON_GREAT_PEOPLE] Great Person, receive 50 [ICON_RESEARCH] Science, bonus scales with Era.'
WHERE Tag = 'TXT_KEY_WONDER_MOSQUE_OF_DJENNE_HELP';

UPDATE Language_en_US
SET Text = 'University of Sankore'
WHERE Tag = 'TXT_KEY_BUILDING_MOSQUE_OF_DJENNE';

UPDATE Language_en_US
SET Text = 'As the center of an Islamic scholarly community, the University of Sankore was very different in organization from the universities of medieval Europe. It had no central administration other than the Emperor. It had no student registers but kept copies of its student publishings. It was composed of several entirely independent schools or colleges, each run by a single master or imam. Students associated themselves with a single teacher, and courses took place in the open courtyard of the mosque or at private residences.'
WHERE Tag = 'TXT_KEY_WONDER_MOSQUE_OF_DJENNE_DESC';

UPDATE Language_en_US
SET Text = 'Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_PROPHET] Great Prophet and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Church in the City in which it is built. Boosts starting Religious Strength of all [ICON_MISSIONARY] Missionaries by 25%.'
WHERE Tag = 'TXT_KEY_WONDER_HAGIA_SOPHIA_HELP';

UPDATE Language_en_US
SET Text = 'Requires [COLOR_MAGENTA]Order[ENDCOLOR]. +33% [ICON_PRODUCTION] Production when building Armor Units. 1 Free Social Policy. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD] and [ICON_PRODUCTION] Distress in this City.'
WHERE Tag = 'TXT_KEY_WONDER_KREMLIN_HELP';

UPDATE Language_en_US
SET Text = 'Can only be built in a Holy City. Receive 2 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_MISSIONARY] Missionaries and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Stupa in the City in which it is built. All new and existing [ICON_MISSIONARY] Missionaries and Prophets gain 1 [COLOR_POSITIVE_TEXT]Additional[ENDCOLOR] Religous Spread action.'
WHERE Tag = 'TXT_KEY_WONDER_BOROBUDUR_HELP';

UPDATE Language_en_US
SET Text = 'Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Bastion Fort in the City in which it is built. +15% [ICON_STRENGTH] Combat Strength for Units fighting in [COLOR_POSITIVE_TEXT]Friendly Territory[ENDCOLOR], and +10% [ICON_STRENGTH] Combat Strength for all Cities. Increases [ICON_SILVER_FIST] Military Unit Supply Cap by 5.'
WHERE Tag = 'TXT_KEY_WONDER_HIMEJI_CASTLE_HELP';

UPDATE Language_en_US
SET Text = 'Can only be built in a City within 2 tiles of a Mountain that is inside your territory. +15% [ICON_GOLD] Gold from [ICON_CONNECTED] City Connections. City gains +1 [ICON_FOOD] Food, [ICON_PRODUCTION] Production, [ICON_CULTURE] Culture, and [ICON_PEACE] Faith for every Mountain within 3 tiles of the City.'
WHERE Tag = 'TXT_KEY_WONDER_MACHU_PICHU_HELP';

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BUILDING_NOTRE_DAME_HELP', 'Receive a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Cathedral in the City in which it is built. The Empire enters a [ICON_GOLDEN_AGE] Golden Age. Contains 2 slots for [ICON_GREAT_WORK] Great Works of Art or Artifacts. +3 [ICON_PEACE] Faith and [ICON_GOLDEN_AGE] Golden Age Points if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].');

UPDATE Language_en_US
SET Text = 'Requires completion of [COLOR_MAGENTA]Progress[ENDCOLOR] Branch. Cost of [ICON_GOLD] Gold purchasing in all Cities reduced by 15%. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty in this City.'
WHERE Tag = 'TXT_KEY_WONDER_FORBIDDEN_PALACE_HELP';

UPDATE Language_en_US
SET Text = 'Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_WRITER] Great Writer. [ICON_VP_WRITER] Writers, [ICON_VP_ARTIST] Artists, and [ICON_VP_MUSICIAN] Musicians in all Cities gain +2 [ICON_GOLDEN_AGE] Golden Age Points. Contains 2 slots for [ICON_VP_GREATWRITING] Great Works of Writing. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom in this City. 1 Specialist in every City no longer generates [ICON_HAPPINESS_3] Unhappiness from [ICON_URBANIZATION] Urbanization. +10 [ICON_GOLD] Gold if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_GLOBE_THEATER_HELP';

UPDATE Language_en_US
SET Text = 'Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_ARTIST] Great Artist and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Gallery in the City in which it is built. +1 [ICON_CULTURE] Culture from [ICON_GREAT_WORK] Great Works in all Cities. Contains 3 slots for [ICON_GREAT_WORK] Great Works of Art or Artifacts. 1 Specialist in every City no longer generates [ICON_HAPPINESS_3] Unhappiness from [ICON_URBANIZATION] Urbanization. +10 [ICON_CULTURE] Culture if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_UFFIZI_HELP';

-- Here is in fact the TAJ MAHAL, in the original files is wrong defined
UPDATE Language_en_US
SET Text = 'The Empire enters a [ICON_GOLDEN_AGE] Golden Age. +1 [ICON_GOLDEN_AGE] Golden Age Points per turn for every 2 [ICON_CITIZEN] Citizens in the City. +3 [ICON_RESEARCH] Science, [ICON_CULTURE] Culture, and [ICON_PEACE] Faith in the City for every [ICON_RELIGION] Religion present.'
WHERE Tag = 'TXT_KEY_WONDER_PYRAMIDS_HELP';

-- Here is in fact the CHICHEN ITZA, in the original files is wrong defined
UPDATE Language_en_US
SET Text = 'Length of [ICON_GOLDEN_AGE] Golden Ages increased by 50%. [ICON_CITY_STATE] Empire Size Modifier is reduced by 10% in all Cities.'
WHERE Tag = 'TXT_KEY_WONDER_TAJ_MAHAL_HELP';

-- Here is in fact the PYRAMIDS, in the original files is wrong defined
UPDATE Language_en_US
SET Text = 'A Settler appears near the City. +50 [ICON_GOLDEN_AGE] Golden Age Points when you expend a [ICON_GREAT_PEOPLE] Great Person, scaling with Era.'
WHERE Tag = 'TXT_KEY_WONDER_CHICHEN_ITZA_HELP';

UPDATE Language_en_US
SET Text = '+10% [ICON_CULTURE] Culture in all Cities. Contains 2 slots for [ICON_GREAT_WORK] Great Works of Art or Artifacts. +3 [ICON_PEACE] Faith and [ICON_CULTURE] Culture if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_SISTINE_CHAPEL_HELP';

UPDATE Language_en_US
SET Text = 'Requires completion of [COLOR_MAGENTA]Fealty[ENDCOLOR] Branch. Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_ENGINEER] Great Engineer and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Arsenal. City [ICON_RANGE_STRENGTH] Ranged Strike Strength and Range increased by 10% and 1, respectively. Greatly increases [ICON_STRENGTH] Strength and HP of the City and [ICON_SILVER_FIST] Military Units Supplied by City''s population increased by 5% in all Cities.'
WHERE Tag = 'TXT_KEY_WONDER_RED_FORT_HELP';

UPDATE Language_en_US
SET Text = 'Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_PEOPLE] Great Person of your choice near the [ICON_CAPITAL] Capital. +25% generation of [ICON_GREAT_PEOPLE] Great People in the City, and +10% in all other Cities.'
WHERE Tag = 'TXT_KEY_WONDER_LEANING_TOWER_HELP';

UPDATE Language_en_US
SET Text = 'Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_SCIENTIST] Great Scientist. 50% more [ICON_RESEARCH] Science generated from Research Agreements. If Research Agreements are disabled, provides a +25% [ICON_RESEARCH] Science bonus in the City in which it is built. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_RESEARCH] Illiteracy in this City.'
WHERE Tag = 'TXT_KEY_WONDER_PORCELAIN_TOWER_HELP';

UPDATE Language_en_US
SET Text = 'Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_GENERAL] Great General. +15 XP for Units built in all Cities. Increases [ICON_SILVER_FIST] Military Unit Supply Cap by 10. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty in this City.'
WHERE Tag = 'TXT_KEY_WONDER_BRANDENBURG_GATE_HELP';

UPDATE Language_en_US
SET Text = 'Requires completion of [COLOR_MAGENTA]Artistry[ENDCOLOR] Branch. Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_ARTIST] Great Artist, 2 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Archaeologists, and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Museum in the City in which it is built. Contains 4 slots for [ICON_GREAT_WORK] Great Works of Art or Artifacts. +15 [ICON_CULTURE] Culture if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_LOUVRE_HELP';

UPDATE Language_en_US
SET Text = '[ICON_CULTURE] Culture cost of adopting new Policies reduced by 10%.'
WHERE Tag = 'TXT_KEY_WONDER_EIFFEL_TOWER_HELP';

UPDATE Language_en_US
SET Text = 'Requires completion of [COLOR_MAGENTA]Statecraft[ENDCOLOR] Branch. Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_MERCHANT] Great Merchant, and Grants 2 [COLOR_POSITIVE_TEXT]Additional[ENDCOLOR] [ICON_DIPLOMAT] Delegates in the World Congress for every 8 [ICON_CITY_STATE] City-States in the game. -1 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty in this City.'
WHERE Tag = 'TXT_KEY_WONDER_BIG_BEN_HELP';

UPDATE Language_en_US
SET Text = 'Palace of Westminster'
WHERE Tag = 'TXT_KEY_BUILDING_BIG_BEN';

UPDATE Language_en_US
SET Text = 'Can only be built in a City within 2 tiles of a Mountain that is inside your territory. +1 [ICON_GOLD] Gold, [ICON_CULTURE] Culture, and [ICON_HAPPINESS_1] Happiness from every Castle.'
WHERE Tag = 'TXT_KEY_WONDER_NEUSCHWANSTEIN_HELP';

UPDATE Language_en_US
SET Text = 'Requires completion of [COLOR_MAGENTA]Industry[ENDCOLOR] Branch. Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_MUSICIAN] Great Musician. Contains 3 slots for [ICON_VP_GREATMUSIC] Great Works of Music. +250 [ICON_CULTURE] Culture when you construct a building in this City, scaling with Era. +20 [ICON_GOLD] Gold if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_BROADWAY_HELP';

UPDATE Language_en_US
SET Text = 'Requires [COLOR_MAGENTA]Autocracy[ENDCOLOR] and can only be constructed in a Coastal City. Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Social Policy. +1 [ICON_HAPPINESS_1] Happiness in [ICON_CAPITAL] Capital for every 2 Policies you have adopted. Receive 25 [ICON_CULTURE] Culture when any owned Unit pillages a tile, scaling with Era.'
WHERE Tag = 'TXT_KEY_WONDER_PRORA_RESORT_HELP';

-- added line for Statue of Liberty
UPDATE Language_en_US
SET Text = 'Requires [COLOR_MAGENTA]Freedom[ENDCOLOR]. Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Social Policy. +1 [ICON_PRODUCTION] Production from Specialists in every City.'
WHERE Tag = 'TXT_KEY_WONDER_STATUE_OF_LIBERTY_HELP';

-- added line for International Space Station
UPDATE Language_en_US
SET Text = 'May only be built collaboratively through the World Congress. +1 [ICON_PRODUCTION] Production from [ICON_VP_SCIENTIST] Scientists, and +1 [ICON_RESEARCH] Science from [ICON_VP_ENGINEER]  Engineers. [ICON_GREAT_SCIENTIST] Great Scientists provide 33% more [ICON_RESEARCH] Science when used to discover new Technology.[NEWLINE][NEWLINE]+100 [ICON_PRODUCTION] in City where it is built when you unlock a new Technology, scaling with Era.'
WHERE Tag = 'TXT_KEY_BUILDING_INTERNATIONAL_SPACE_STATION_HELP';

UPDATE Language_en_US
SET Text = 'Requires completion of [COLOR_MAGENTA]Imperialism[ENDCOLOR] Branch. Increases [ICON_SILVER_FIST] Military Unit Supply Cap by 1, and the Air Unit Cap by 2 in every City. Receive 2 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Jet Fighters in the City when completed. +50% [ICON_PRODUCTION] Production of Air Units in the City, and Air Units created in the City gain +20 XP.'
WHERE Tag = 'TXT_KEY_WONDER_PENTAGON_HELP';

-- here added and updated
UPDATE Language_en_US
SET Text = 'Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_SCIENTIST] Great Scientist and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Spaceship Factory in the City in which it is built. +25% [ICON_PRODUCTION] Production when building Spaceship parts.'
WHERE Tag = 'TXT_KEY_WONDER_HUBBLE_HELP';

UPDATE Language_en_US
SET Text = '+1 [ICON_HAPPINESS_1] Happiness in every City. Provides a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Broadcast Tower in the City in which it is built.[NEWLINE][NEWLINE]All [ICON_GREAT_WORK] Great Works gain +2 [ICON_GOLD] Gold and [ICON_TOURISM] Tourism. 25% of the [ICON_CULTURE] Culture from [ICON_WONDER] World Wonders and Tiles is added to the [ICON_TOURISM] Tourism output of all Cities, and an additional 25% from this City. [ICON_TOURISM] Tourism output from [ICON_GREAT_WORK] Great Works in all Cities increased by 25%, and an additional 25% in this City.'
WHERE Tag = 'TXT_KEY_WONDER_CN_TOWER_HELP';

UPDATE Language_en_US
SET Text = '+25 [ICON_TOURISM] Tourism. Hotels provide +2 [ICON_GOLDEN_AGE] Golden Age Points, [ICON_CULTURE] Culture, and [ICON_TOURISM] Tourism.'
WHERE Tag = 'TXT_KEY_WONDER_CRISTO_REDENTOR_HELP';

UPDATE Language_en_US
SET Text = 'Can only be constructed in a Coastal City. Receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Social Policy and +25% [ICON_CULTURE] Culture in this City. Contains 2 slots for [ICON_VP_GREATMUSIC] Great Works of Music. +15 [ICON_CULTURE] Culture if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_SYDNEY_OPERA_HOUSE_HELP';

UPDATE Language_en_US
SET Text = 'All Research Labs gain +10 [ICON_RESEARCH] Science. 99.9% reduction in effectiveness of enemy [ICON_SPY] Spies in the City in which it is built. All other Cities in your Empire get a 25% reduction in enemy [ICON_SPY] Spy effectiveness. Negates the [ICON_TOURISM] Tourism bonus from the Technologies of other players.'
WHERE Tag = 'TXT_KEY_BUILDING_GREAT_FIREWALL_HELP';

UPDATE Language_en_US
SET Text = 'Contribute this City''s [ICON_PRODUCTION] Production towards the World''s Fair project. Cost goes up based on the number of players in the game, and the current Era.'
WHERE Tag = 'TXT_KEY_PROCESS_WORLD_FAIR_HELP';

UPDATE Language_en_US
SET Text = 'Contribute this City''s [ICON_PRODUCTION] Production towards the International Games project. Cost goes up based on the number of players in the game, and the current Era.'
WHERE Tag = 'TXT_KEY_PROCESS_WORLD_GAMES_HELP';

UPDATE Language_en_US
SET Text = 'Contribute this City''s [ICON_PRODUCTION] Production towards the International Space Station project. Cost goes up based on the number of players in the game, and the current Era.'
WHERE Tag = 'TXT_KEY_PROCESS_INTERNATIONAL_SPACE_STATION_HELP';

UPDATE Language_en_US
SET Text = 'Citizen Earth Protocol'
WHERE Tag = 'TXT_KEY_PROJECT_UTOPIA_PROJECT';

-- correctly realigned the first bullet and colored in green 'Cultural Victory'
UPDATE Language_en_US
SET Text = 'Construct the Citizen Earth Protocol to win a [COLOR_POSITIVE_TEXT]Cultural Victory[ENDCOLOR]![NEWLINE][NEWLINE]Requirements for Construction: [NEWLINE]   [ICON_BULLET] Must have an Ideology (with a [COLOR_POSITIVE_TEXT]Content[ENDCOLOR] Population).[NEWLINE]   [ICON_BULLET] Must be [ICON_TOURISM] [COLOR_POSITIVE_TEXT]Influential[ENDCOLOR] with all other Civilizations in the world.[NEWLINE]   [ICON_BULLET] Must have [COLOR_POSITIVE_TEXT]2[ENDCOLOR] Tier 3 Tenets in your current Ideology.'
WHERE Tag = 'TXT_KEY_PROJECT_UTOPIA_PROJECT_HELP';

UPDATE Language_en_US
SET Text = 'Global citizenship is idea of all persons having rights and civic responsibilities that come with being a member of the World, with whole-world philosophy and sensibilities, rather than as a citizen of a particular nation or place. The idea is that ones identity transcends geography or political borders and that responsibilities or rights are derived from membership in a broader class: ''humanity.'' This does not mean that such a person denounces or waives their nationality or other, more local identities, but such identities are given ''second place'' to their membership in a global community. Extended, the idea leads to questions about the state of global society in the age of globalization. In general usage, the term may have much the same meaning as ''world citizen'' or cosmopolitan, but it also has additional, specialized meanings in differing contexts. Various organizations, such as the World Service Authority, have advocated global citizenship.'
WHERE Tag = 'TXT_KEY_PROJECT_UTOPIA_PROJECT_PEDIA';

UPDATE Language_en_US
SET Text = 'You must have an Ideology (with a [COLOR_POSITIVE_TEXT]Content[ENDCOLOR] population), two Tier 3 Tenets in this Ideology, and be [ICON_TOURISM] Influential with all other Civilizations in the world in order to construct this. Once you have completed these tasks, construct this project to win a Cultural Victory!'
WHERE Tag = 'TXT_KEY_PROJECT_UTOPIA_PROJECT_STRATEGY';
