-- Ahmad -- Boost Kasbah, Ability
UPDATE Trait_YieldChangesPerTradePartner
SET Yield = '3'
WHERE YieldType = 'YIELD_GOLD' AND TraitType = 'TRAIT_GATEWAY_AFRICA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Trait_YieldChangesPerTradePartner
SET Yield = '3'
WHERE YieldType = 'YIELD_CULTURE' AND TraitType = 'TRAIT_GATEWAY_AFRICA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Improvement_Yields
SET Yield = '2'
WHERE YieldType = 'YIELD_GOLD' AND ImprovementType = 'IMPROVEMENT_KASBAH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Improvement_Yields
SET Yield = '1'
WHERE YieldType = 'YIELD_FOOD' AND ImprovementType = 'IMPROVEMENT_KASBAH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Improvement_Yields
SET Yield = '2'
WHERE YieldType = 'YIELD_PRODUCTION' AND ImprovementType = 'IMPROVEMENT_KASBAH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Improvements
SET Cityside = '1'
WHERE Type = 'IMPROVEMENT_KASBAH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Improvements
SET DefenseModifier = '30'
WHERE Type = 'IMPROVEMENT_KASBAH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Improvements
SET BuildableOnResources = 'true'
WHERE Type = 'IMPROVEMENT_KASBAH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Improvements
SET NearbyEnemyDamage = '5'
WHERE Type = 'IMPROVEMENT_KASBAH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Builds
SET PrereqTech = 'TECH_CHIVALRY'
WHERE Type = 'BUILD_KASBAH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Builds
SET Time = '1000'
WHERE Type = 'BUILD_KASBAH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A Kasbah can only be built adjacent to a City.  It provides one additional [ICON_FOOD] Food, [ICON_PRODUCTION] Production, and [ICON_GOLD] Gold. It also provides a +30% defense bonus.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_KASBAH_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A Kasbah is a type of medina (a walled quarter in a city) found in the countryside, usually a small settlement on a hilltop or hillside. Originally the home for a tribal chieftain or important Islamic imam, the Kasbah is characterized by high-walled, windowless houses and narrow, winding streets. It is usually dominated by a single fortified tower. Kasbahs were common along the North African coast and Middle East until the early 1900s. Building a Kasbah was a mark of wealth, influence, and power for Moroccan and Algerian families of Arab descent.[NEWLINE][NEWLINE]A Kasbah can only be built adjacent to an owned City.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_KASBAH_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receives +3 Gold [ICON_GOLD], [ICON_GOLDEN_AGE] Golden Age Points, and [ICON_CULTURE] Culture for each Trade Route to or from a different Civ or City-State. Bonuses scale with Era. Trade Route owners receive +2 [ICON_GOLD] Gold for each Trade Route sent to Morocco.'
WHERE Tag = 'TXT_KEY_TRAIT_GATEWAY_AFRICA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Mounted unit that specializes in protecting Moroccan lands. Receives combat bonuses when fighting both in Moroccan territory. Ignores terrain penalties. May only be built by Morocco.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BERBER_CAVALRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- Ashurbanipal -- Boost Royal Library
UPDATE Building_DomainFreeExperiencePerGreatWork
SET Experience = '15'
WHERE BuildingType = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Buildings
SET GreatWorkCount = '2'
WHERE Type = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Buildings
SET SpecialistCount = '1'
WHERE Type = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Buildings
SET SpecialistType = 'SPECIALIST_SCIENTIST'
WHERE Type = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Building_YieldChangesPerPop
SET Yield = '25'
WHERE BuildingType = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );
 
UPDATE Language_en_US
SET Text = '+1 [ICON_RESEARCH] Science for every 4 [ICON_CITIZEN] Citizens in this City. Reduces [ICON_HAPPINESS_3] Illiteracy.[NEWLINE][NEWLINE]Has two slots for Great Works of Writing which, when filled, give extra XP to trained Units.[NEWLINE][NEWLINE]+3 [ICON_PRODUCTION] Production if Themed.'
WHERE Tag = 'TXT_KEY_BUILDING_ROYAL_LIBRARY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
 
UPDATE Language_en_US
SET Text = 'In addition to the [ICON_RESEARCH] Science output of the Library it replaces, the Royal Library also has two Great Work of Writing slots. Once filled with a Great Work, each provides extra XP to Units trained in this city, so place Great Works here as soon as possible for the maximum benefit. Only the Assyrians may build it.'
WHERE Tag = 'TXT_KEY_BUILDING_ROYAL_LIBRARY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'When a city is conquered for the first time, gain a free Technology already discovered by its owner. If there are no Technologies to discover, receive a large sum of [ICON_RESEARCH] Science instead.'
WHERE Tag = 'TXT_KEY_TRAIT_SLAYER_OF_TIAMAT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Buildings
SET ThemingBonusHelp = 'TXT_KEY_BUILDING_ROYAL_LIBRARY_THEMING_BONUS_HELP'
WHERE Type = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- Ashurbanipal -- Earlier Siege Tower
UPDATE Unit_Flavors
SET Flavor = '5'
WHERE FlavorType = 'FLAVOR_RANGED' AND UnitType = 'UNIT_ASSYRIAN_SIEGE_TOWER';

UPDATE Unit_Flavors
SET Flavor = '15'
WHERE FlavorType = 'FLAVOR_OFFENSE' AND UnitType = 'UNIT_ASSYRIAN_SIEGE_TOWER';	

UPDATE Units
SET CityAttackOnly = 'true'
WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET PrereqTech = 'TECH_ARCHERY'
WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET PrereqTech = 'TECH_ARCHERY'
WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET ObsoleteTech = 'TECH_METALLURGY'
WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Powerful melee siege unit which grants a city attack bonus to nearby units when next to an enemy city. Available at Military Training instead of Mathematics. May only be built by the Assyrians.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- Enrico - Unique Building (Doge's Palace)

DELETE FROM Civilization_UnitClassOverrides
WHERE UnitType = 'UNIT_VENETIAN_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO UnitClasses (Type, Description, DefaultUnit)
SELECT 'UNITCLASS_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_VENETIAN_GALLEASS', 'UNIT_VENETIAN_GALLEASS'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET MinorCivGift = 'true'
WHERE Type = 'UNIT_VENETIAN_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Great Galleass can only be gifted by Minor Civilizations.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_VENETIAN_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Cannot gain settlers nor annex cities. Double the normal number of trade routes available. A Merchant of Venice appears after researching Sailing. May purchase in puppeted cities.'
WHERE Tag = 'TXT_KEY_TRAIT_SUPER_CITY_STATE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET Class = 'UNITCLASS_VENETIAN_GALLEASS'
WHERE Type = 'UNIT_VENETIAN_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET PrereqTech = 'TECH_GUILDS'
WHERE Type = 'UNIT_VENETIAN_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET ObsoleteTech = 'TECH_DYNAMITE'
WHERE Type = 'UNIT_VENETIAN_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Gajah Mada -- Boost Candi, No Isolation Unhappiness

UPDATE Units
SET ObsoleteTech = 'TECH_GUNPOWDER'
WHERE Type = 'UNIT_KRIS_SWORDSMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET Combat = '16'
WHERE Type = 'UNIT_KRIS_SWORDSMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Unique Indonesian Garden replacement. +2 [ICON_GOLD] Gold from [ICON_RES_CITRUS] Citrus and [ICON_RES_COCOA] Cocoa.[NEWLINE][NEWLINE]A [ICON_RES_CLOVES] Clove, [ICON_RES_PEPPER] Pepper, or [ICON_RES_NUTMEG] Nutmeg Resource will appear near or under this City when built.[NEWLINE][NEWLINE]+25% [ICON_GREAT_PEOPLE] Great People generation in this City, and +2 [ICON_PEACE] Faith for each World Religion that has at least 1 follower in the city. [NEWLINE][NEWLINE]Unlike the Garden the Candi does not have to be built next to a River or Lake.'
WHERE Tag = 'TXT_KEY_BUILDING_CANDI_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Buildings
SET GrantsRandomResourceTerritory = '1'
WHERE Type = 'BUILDING_CANDI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Buildings
SET MutuallyExclusiveGroup = '2'
WHERE Type = 'BUILDING_CANDI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Traits
SET UniqueLuxuryRequiresNewArea = '0'
WHERE Type = 'TRAIT_SPICE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET UniqueLuxuryQuantity = '1'
WHERE Type = 'TRAIT_SPICE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET UniqueLuxuryCities = '100'
WHERE Type = 'TRAIT_SPICE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET NoConnectionUnhappiness = 'true'
WHERE Type = 'TRAIT_SPICE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Every time you found a city, one of three unique Luxuries will appear next to or under the City. No [ICON_HAPPINESS_3] Unhappiness from Isolation.'
WHERE Tag = 'TXT_KEY_TRAIT_SPICE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Sumpah Palapa'
WHERE Tag = 'TXT_KEY_TRAIT_SPICE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Appears as a unique luxury good in or near an Indonesian city.'
WHERE Tag = 'TXT_KEY_RESOURCE_NUTMEG_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Appears as a unique luxury good in or near an Indonesian city.'
WHERE Tag = 'TXT_KEY_RESOURCE_CLOVES_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Appears as a unique luxury good in or near an Indonesian city.'
WHERE Tag = 'TXT_KEY_RESOURCE_PEPPER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '50% chance of Healing 10 HP if turn ended in Enemy Territory'
WHERE Tag = 'TXT_KEY_PROMOTION_ENEMY_BLADE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Enemy Defection'
WHERE Tag = 'TXT_KEY_PROMOTION_ENEMY_BLADE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+5% [ICON_STRENGTH] Strength when attacking, +30% [ICON_STRENGTH] Strength when defending.'
WHERE Tag = 'TXT_KEY_PROMOTION_EVIL_SPIRITS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Ancestral Protection'
WHERE Tag = 'TXT_KEY_PROMOTION_EVIL_SPIRITS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+30% [ICON_STRENGTH] Strength when attacking, +5% [ICON_STRENGTH] Strength when defending.'
WHERE Tag = 'TXT_KEY_PROMOTION_AMBITION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Maria I
UPDATE Language_en_US
SET Text = 'Melee naval unit which excels at sea exploration. It has more [ICON_MOVES] Movement than the Caravel, which it replaces, and can perform an ability when adjacent to City-States to earn [ICON_GOLD] Gold and XP. May only be built by the Portuguese. [NEWLINE][NEWLINE]If possible, when a Nau sells its Exotic Cargo, a Feitoria is [COLOR_POSITIVE_TEXT]automatically created[ENDCOLOR] in its territory.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_PORTUGUESE_NAU' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Resource diversity doubles [ICON_GOLD] Gold from Trade Routes. When your Trade Units move, receive [ICON_RESEARCH] Science, [ICON_GREAT_ADMIRAL] Great Admiral Points (for Cargo Ships), and [ICON_GREAT_GENERAL] Great General Points (for Caravans).'
WHERE Tag = 'TXT_KEY_TRAIT_EXTRA_TRADE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'May Sell Exotic Goods when adjacent to City-State lands to gain [ICON_GOLD] Gold and XP based on distance from capital. Each Nau can perform this action twice. [NEWLINE][NEWLINE]If possible, when a Nau sells its Exotic Cargo, a Feitoria is also [COLOR_POSITIVE_TEXT]automatically created[ENDCOLOR] in its territory.'
WHERE Tag = 'TXT_KEY_PROMOTION_SELL_EXOTIC_GOODS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET NumExoticGoods = '2'
WHERE Type = 'UNIT_PORTUGUESE_NAU' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Builds
SET PrereqTech = 'TECH_COMPASS'
WHERE Type = 'BUILD_FEITORIA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Improvements
SET GrantsVisionXTiles = '3'
WHERE Type = 'IMPROVEMENT_FEITORIA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A Feitoria can be constructed by Workers or by the Nau (via Exotic Cargo), and only in City-State land without a resource. Provides a copy of each Luxury Resource type owned by the City-State, but that copy cannot be traded. [ICON_INTERNATIONAL_TRADE] Trade Routes to this City-State generate bonus [ICON_PRODUCTION] Production and [ICON_FOOD] Food based on your [ICON_GOLD] Gold income from the [ICON_INTERNATIONAL_TRADE] Trade Route, and your relationship with the City-State (i.e. Neutral/Friend/Ally). [NEWLINE][NEWLINE]In addition to these bonus, the Feitoria provides vision of its tile and all tiles within a radius of 3 and provides the same +50% defense bonus as a Fort. Can only be built by the Portuguese.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_FEITORIA_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Shoshone

UPDATE Units
SET ObsoleteTech = 'TECH_ASTRONOMY'
WHERE Type = 'UNIT_SHOSHONE_PATHFINDER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

DELETE FROM Units
WHERE Type = 'UNIT_SHOSHONE_COMANCHE_RIDERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );



DELETE FROM Civilization_UnitClassOverrides
WHERE UnitType = 'UNIT_SHOSHONE_COMANCHE_RIDERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Barbarian Encampment'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_ENCAMPMENT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Barbarian Encampment'
WHERE Tag = 'TXT_KEY_IMPROVEMENT_ENCAMPMENT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A Barbarian Encampment is a temporary settlement by a group of rampaging barbarians intent upon overthrowing and destroying your civilization. These perpetual thorns in the sides of civilizations are capable of spitting out an unending stream of angry barbarians, all who seek to overrun your cities, pillage your lands and stare lewdly at your women. Encampments near to your cities should be destroyed without question. Encampments nearer to competing civilizations may be left to harass your foes, if you are that kind of sneaky leader...'
WHERE Tag = 'TXT_KEY_IMPROVEMENT_BARBARIAN_CAMP_PEDIA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Casimir III -- Ducal Stable +1 Production
UPDATE Buildings
SET PrereqTech = 'TECH_CHIVALRY'
WHERE Type = 'BUILDING_DUCAL_STABLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Building_YieldChanges
SET Yield = '3'
WHERE BuildingType = 'BUILDING_DUCAL_STABLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Building_ResourceYieldChanges
SET Yield = '3'
WHERE BuildingType = 'BUILDING_DUCAL_STABLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Building_UnitCombatProductionModifiers
SET Modifier = '25'
WHERE BuildingType = 'BUILDING_DUCAL_STABLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Unique Polish Stable replacement. +25% [ICON_PRODUCTION] Production and  +15 XP for Mounted Units.[NEWLINE][NEWLINE]Each Pasture worked by this City produces +3 [ICON_PRODUCTION] Production and +3 [ICON_GOLD] Gold, and the City itself produces +3 [ICON_PRODUCTION] Production.'
WHERE Tag = 'TXT_KEY_BUILDING_DUCAL_STABLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Pedro -- Improve Brazilwood Camp, increase all GP production during GA
UPDATE Language_en_US
SET Text = 'During a [ICON_GOLDEN_AGE] Golden Age, [ICON_TOURISM] Tourism output increases by +25%, all [ICON_GREAT_PEOPLE] Great People are earned 50% more quickly, and 50% of your [ICON_CAPITAL] Capital''s [ICON_CULTURE] Culture is added to your [ICON_TOURISM] Tourism.'
WHERE Tag = 'TXT_KEY_TRAIT_CARNIVAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Because of your unique Carnival bonus, [ICON_TOURISM] Tourism output is currently +25% with all civilizations, you are earning [ICON_GREAT_PEOPLE] Great People 25% faster, and 25% of your [ICON_CAPITAL] Capital''s [ICON_CULTURE] Culture is being converted into [ICON_TOURISM] Tourism.'
WHERE Tag = 'TXT_KEY_TP_CARNIVAL_EFFECT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Modern Era infantry unit that earns points toward a [ICON_GOLDEN_AGE] Golden Age when it defeats an enemy. Starts with Survivalism I. May only be built by Brazil.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BRAZILIAN_PRACINHA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET GoldenAgeTourismModifier = '25'
WHERE Type = 'TRAIT_CARNIVAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET TourismGABonus = '50'
WHERE Type = 'TRAIT_CARNIVAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET GoldenAgeGreatArtistRateModifier = '50'
WHERE Type = 'TRAIT_CARNIVAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET GoldenAgeGreatMusicianRateModifier = '50'
WHERE Type = 'TRAIT_CARNIVAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET GoldenAgeGreatWriterRateModifier = '50'
WHERE Type = 'TRAIT_CARNIVAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );


UPDATE Builds
SET PrereqTech = 'TECH_CALENDAR'
WHERE Type = 'BUILD_BRAZILWOOD_CAMP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Constructs a Brazilwood Camp, which increases the amount of [ICON_GOLD] Gold provided by this tile, while keeping the Jungle intact. Provides additional [ICON_CULTURE] Culture when Physics, Acoustics, or Radio are researched.'
WHERE Tag = 'TXT_KEY_BUILD_BRAZILWOOD_CAMP_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'It will increase the amount of [ICON_GOLD] Gold provided by this tile, while keeping the Jungle intact. Also provides additional [ICON_CULTURE] Culture when Physics, Acoustics, or Radio are researched.'
WHERE Tag = 'TXT_KEY_BUILD_BRAZILWOOD_CAMP_REC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A Brazilwood Camp can only be built on a Jungle tile.  It provides additional [ICON_GOLD] Gold. Provides additional [ICON_CULTURE] Culture when Physics, Acoustics, or Radio are researched.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_BRAZILWOOD_CAMP_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Zulu -- Ikanda gives promotions to all melee infantry

UPDATE Language_en_US
SET Text = 'Unique Zulu Barracks replacement. Grants unique promotions to all melee units, and reduces [ICON_STRENGTH] Crime slightly.'
WHERE Tag = 'TXT_KEY_BUILDING_IKANDA_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'In addition to the regular abilities of the Barracks which it replaces, the Ikanda grants a unique set of promotions to all melee units created within the city. These include faster movement, and greater combat strength. Only the Zulu may build it.'
WHERE Tag = 'TXT_KEY_BUILDING_IKANDA_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET Class = 'UNITCLASS_MUSKETMAN'
WHERE Type = 'UNIT_ZULU_IMPI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET PrereqTech = 'TECH_GUNPOWDER'
WHERE Type = 'UNIT_ZULU_IMPI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET Combat = '28'
WHERE Type = 'UNIT_ZULU_IMPI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );
	
UPDATE Unit_ClassUpgrades
SET UnitClassType = 'UNITCLASS_RIFLEMAN'	
WHERE UnitType = 'UNIT_ZULU_IMPI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET ObsoleteTech = 'TECH_REPLACEABLE_PARTS'
WHERE Type = 'UNIT_ZULU_IMPI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Civilization_UnitClassOverrides
Set UnitClassType = 'UNITCLASS_MUSKETMAN'
WHERE UnitType = 'UNIT_ZULU_IMPI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Specialized in defeating mounted units, and stronger overall than most units from its era. Only the Zulu may build it. Also performs a first-strike ranged attack before combat.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ZULU_IMPI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Melee units cost 50% less maintenance, units require 25% less experience to earn their next promotion, and your [ICON_GREAT_GENERAL] Great General combat bonus is increased by 20%.'
WHERE Tag = 'TXT_KEY_TRAIT_BUFFALO_HORNS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET GreatGeneralExtraBonus = '20'
WHERE Type = 'TRAIT_BUFFALO_HORNS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );