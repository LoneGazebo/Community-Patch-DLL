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
SET BuildableOnResources = '1'
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
SET Text = 'A Kasbah can only be built adjacent to a City.  It provides additional [ICON_FOOD] Food, [ICON_PRODUCTION] Production, [ICON_GOLD] Gold, and [ICON_CULTURE] Culture. It also provides a +30% defense bonus and connects any Luxury or Strategic resources underneath it to your trade network.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_KASBAH_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A Kasbah can only be built adjacent to an owned City. It provides additional [ICON_FOOD] Food, [ICON_PRODUCTION] Production, [ICON_GOLD] Gold, and [ICON_CULTURE] Culture. It also provides a +30% defense bonus and connects any Luxury or Strategic resources underneath it to your trade network.[NEWLINE][NEWLINE]A Kasbah is a type of medina (a walled quarter in a city) found in the countryside, usually a small settlement on a hilltop or hillside. Originally the home for a tribal chieftain or important Islamic imam, the Kasbah is characterized by high-walled, windowless houses and narrow, winding streets. It is usually dominated by a single fortified tower. Kasbahs were common along the North African coast and Middle East until the early 1900s. Building a Kasbah was a mark of wealth, influence, and power for Moroccan and Algerian families of Arab descent.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_KASBAH_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receives +3 Gold [ICON_GOLD], [ICON_GOLDEN_AGE] Golden Age Points, and [ICON_CULTURE] Culture for each Trade Route to or from a different Civ or City-State. Bonuses scale with Era. Trade Route owners receive +2 [ICON_GOLD] Gold for each Trade Route sent to Morocco.'
WHERE Tag = 'TXT_KEY_TRAIT_GATEWAY_AFRICA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Mounted unit that specializes in protecting Moroccan lands. Receives combat bonuses when fighting both in Moroccan territory. Ignores terrain penalties. May only be built by Morocco.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BERBER_CAVALRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- Ashurbanipal -- Boost Royal Library
UPDATE Buildings
SET GreatWorkCount = '2'
WHERE Type = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Buildings
SET SpecialistType = 'SPECIALIST_SCIENTIST'
WHERE Type = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET GreatPeopleRateChange = '1'
WHERE Type = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NumCityCostMod = '15'
WHERE Type = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Building_YieldChangesPerPop
SET Yield = '50'
WHERE BuildingType = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

DELETE FROM Building_DomainFreeExperiencePerGreatWork
WHERE BuildingType = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Must have built a Library in the city.[NEWLINE][NEWLINE]+1 [ICON_RESEARCH] Science for every 2 [ICON_CITIZEN] Citizens in the City, and all Libraries produce +2 [ICON_RESEARCH] Science. Every Great Work of Writing you control grants +5 XP to units created in any City.[NEWLINE][NEWLINE]Requires a national population of at least 35 before it can be constructed. The cost goes up the more cities there are in the empire. Contains 2 slots for Great Works of Writing. [NEWLINE][NEWLINE]+3 [ICON_PRODUCTION] Production if Themed.'
WHERE Tag = 'TXT_KEY_BUILDING_ROYAL_LIBRARY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
 
UPDATE Language_en_US
SET Text = 'In addition to the [ICON_RESEARCH] Science output of the School of Philosophy it replaces, the Royal Library also has two Great Work of Writing slots. Provides a global bonus to all Libraries, and all owned cities generate extra XP for Units trained in them based on the number of Great Works of Writing in your empire. Only the Assyrians may build it.'
WHERE Tag = 'TXT_KEY_BUILDING_ROYAL_LIBRARY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'When you conquer a City, gain either a Technology already known by the owner or, if impossible, a large [ICON_RESEARCH] Science boost. All Great Works produce +2 [ICON_RESEARCH] Science.'
WHERE Tag = 'TXT_KEY_TRAIT_SLAYER_OF_TIAMAT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Buildings
SET ThemingBonusHelp = 'TXT_KEY_BUILDING_ROYAL_LIBRARY_THEMING_BONUS_HELP'
WHERE Type = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Buildings
SET Cost = '125'
WHERE Type = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Buildings
SET NationalPopRequired = '35'
WHERE Type = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET BuildingClass = 'BUILDINGCLASS_NATIONAL_COLLEGE'
WHERE Type = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Civilization_BuildingClassOverrides
SET BuildingClassType = 'BUILDINGCLASS_NATIONAL_COLLEGE'
WHERE CivilizationType = 'CIVILIZATION_ASSYRIA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

-- Ashurbanipal -- Earlier Siege Tower
UPDATE Unit_Flavors
SET Flavor = '5'
WHERE FlavorType = 'FLAVOR_RANGED' AND UnitType = 'UNIT_ASSYRIAN_SIEGE_TOWER';

UPDATE Unit_Flavors
SET Flavor = '30'
WHERE FlavorType = 'FLAVOR_OFFENSE' AND UnitType = 'UNIT_ASSYRIAN_SIEGE_TOWER';	

UPDATE Units
SET CityAttackOnly = '1'
WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET Combat = '0'
WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET Cost = '100'
WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET DefaultUnitAI = 'UNITAI_CITY_SPECIAL'
WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET CombatClass = NULL
WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET Moves = '4'
WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET Class = 'UNITCLASS_ASSYRIAN_SIEGE_TOWER'
WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

DELETE From Unit_AITypes
WHERE UnitType = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

DELETE From Unit_FreePromotions
WHERE UnitType = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND PromotionType = 'PROMOTION_CITY_SIEGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

DELETE From Unit_FreePromotions
WHERE UnitType = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND PromotionType = 'PROMOTION_COVER_1' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

DELETE From Unit_FreePromotions
WHERE UnitType = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND PromotionType = 'PROMOTION_ONLY_ATTACKS_CITIES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

DELETE From Unit_FreePromotions
WHERE UnitType = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND PromotionType = 'PROMOTION_NO_DEFENSIVE_BONUSES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET PrereqTech = 'TECH_ARCHERY'
WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET ObsoleteTech = 'TECH_GUNPOWDER'
WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

DELETE FROM Civilization_UnitClassOverrides
WHERE UnitType = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET Pillage = '0'
WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Defines
Set Value = '40'
WHERE Name = 'SAPPED_CITY_ATTACK_MODIFIER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET GoodyHutUpgradeUnitClass = NULL
WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

DELETE FROM Unit_ClassUpgrades
WHERE UnitType = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Unique Assyrian support Unit. Nearby owned Units gain +40% [ICON_STRENGTH] Combat Strength versus Cities when adjacent to an enemy City, or +20% [ICON_STRENGTH] Combat Strength if two tiles from an enemy City. Starts with [COLOR_POSITIVE_TEXT]Medic I[ENDCOLOR] and [COLOR_POSITIVE_TEXT]Extra Sight[ENDCOLOR]. Cannot attack or defend itself.[NEWLINE][NEWLINE]May only have [COLOR_POSITIVE_TEXT]2[ENDCOLOR] Siege Towers active at one time.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Unique Assyrian support Unit. This unit must move close to an enemy city to be effective, but once it has, it is devastating. It [COLOR_NEGATIVE_TEXT]cannot attack or defend itself[ENDCOLOR], so protect it at all costs! You cannot have more than 2 active Siege Towers at one time.[NEWLINE][NEWLINE]If adjacent to a city, the Siege Tower confers a huge combat bonus to all nearby units also attacking the city. If the Siege Tower is two tiles away from the City, all nearby units attacking the city receive half of this bonus. Complement a Siege Tower with melee or ranged units, and make sure it reaches its target before getting killed, and cities will fall much more quickly.'
WHERE Tag = 'TXT_KEY_UNIT_ASSYRIAN_SIEGE_TOWER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'When next to an enemy city, other units within 2 tiles get a 40% Bonus when attacking the city. If 2 tiles from an enemy city, other units within 2 tiles get a 20% Bonus when attacking the city. Bonuses do not stack.'
WHERE Tag = 'TXT_KEY_PROMOTION_SAPPER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- Enrico - Unique Building (Doge's Palace)

DELETE FROM Civilization_UnitClassOverrides
WHERE UnitType = 'UNIT_VENETIAN_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO UnitClasses (Type, Description, DefaultUnit)
SELECT 'UNITCLASS_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_VENETIAN_GALLEASS', 'UNIT_VENETIAN_GALLEASS'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET MinorCivGift = '1'
WHERE Type = 'UNIT_VENETIAN_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET Cost = '-1'
WHERE Type = 'UNIT_VENETIAN_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Great Galleass can only be gifted by Minor Civilizations.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_VENETIAN_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Great Galleass is can only be gifted by Minor Civilizations. Has a stronger ranged attack, and is more resilient in battle.'
WHERE Tag = 'TXT_KEY_UNIT_VENETIAN_GALLEASS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

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
SET Text = 'Unique Indonesian Garden replacement. A [ICON_RES_CLOVES] Clove, [ICON_RES_PEPPER] Pepper, or [ICON_RES_NUTMEG] Nutmeg Resource will appear near or under this City when built.[NEWLINE][NEWLINE]10% of the City''s [ICON_CULTURE] Culture output is added to the City''s [ICON_RESEARCH] Science every turn. +25% [ICON_GREAT_PEOPLE] Great People generation in this City, and +2 [ICON_PEACE] Faith for each World Religion that has at least 1 follower in the city.[NEWLINE]Nearby [ICON_RES_CITRUS] Citrus: +1 [ICON_FOOD] Food, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COCOA] Cocoa: +1 [ICON_FOOD] Food, +1 [ICON_GOLD] Gold.'
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
SET NoConnectionUnhappiness = '1'
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
SET Text = 'Melee naval unit which excels at sea exploration. Has higher Combat Strength than the Caravel, and can perform an ability when adjacent to City-States to earn [ICON_GOLD] Gold and XP. May only be built by the Portuguese. [NEWLINE][NEWLINE]If possible, when a Nau sells its Exotic Cargo, a Feitoria is [COLOR_POSITIVE_TEXT]automatically created[ENDCOLOR] in its territory.'
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

UPDATE Language_en_US
SET Text = 'A Feitoria can be constructed by Workers or by the Nau (via Exotic Cargo), and only in City-State land without a resource. Provides a copy of each Luxury Resource type owned by the City-State, but that copy cannot be traded. [ICON_INTERNATIONAL_TRADE] Trade Routes to this City-State generate bonus [ICON_PRODUCTION] Production and [ICON_FOOD] Food based on your [ICON_GOLD] Gold income from the [ICON_INTERNATIONAL_TRADE] Trade Route, and your relationship with the City-State (i.e. Neutral/Friend/Ally). [NEWLINE][NEWLINE]In addition to these bonus, the Feitoria provides vision of its tile and all tiles within a radius of 3 and provides the same +50% defense bonus as a Fort. Can only be built by the Portuguese.[NEWLINE][NEWLINE]The Feitoria - Portuguese for "factory" - were trading posts established in foreign territories during the Middle Ages. A feitoria served simultaneously as marketplace, warehouse, settlement, and staging point for exploration. Often established with a grant from the Portuguese crown, these private enterprises repaid their debt through the buying and selling of goods on behalf of the crown and through collecting taxes on trade that passed through their gates. During the 15th and 16th centuries, a chain of over 50 feitoria were built along the coasts of west and east Africa, India, Malaya, China and Japan. The feitoria allowed Portugal to dominate the Atlantic and Indian Ocean trade routes for three centuries.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_FEITORIA_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

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
SET Text = 'Unique Polish Stable replacement. +25% [ICON_PRODUCTION] Production and  +15 XP for Mounted Units.[NEWLINE][NEWLINE][NEWLINE]Nearby [ICON_RES_HORSE] Horses: +3 [ICON_PRODUCTION] Production, +3 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_SHEEP] Sheep: +3 [ICON_PRODUCTION] Production, +3 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COW] Cattle: +3 [ICON_PRODUCTION] Production, +3 [ICON_GOLD] Gold.'
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
SET Text = 'Constructs a Brazilwood Camp, which increases the amount of [ICON_GOLD] Gold provided by this tile, while keeping the Jungle intact.'
WHERE Tag = 'TXT_KEY_BUILD_BRAZILWOOD_CAMP_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'It will increase the amount of [ICON_GOLD] Gold provided by this tile, while keeping the Jungle intact.'
WHERE Tag = 'TXT_KEY_BUILD_BRAZILWOOD_CAMP_REC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A Brazilwood Camp can only be built on a Jungle tile.  It provides additional [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_BRAZILWOOD_CAMP_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A Brazilwood Camp can only be built on a Jungle tile.  It provides additional [ICON_GOLD] Gold. [NEWLINE][NEWLINE]Brazilwood is a tropical hardwood whose bark produces a bright crimson and deep purple extract used in dyes. Its wood is dense, and is prized for crafting stringed instruments (especially the bows) and cabinetry. The harvesting of brazilwood did not cease until 1875, by which time synthetic dyes dominated the textile industry and other hardwoods had been found to be better for musical instruments.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_BRAZILWOOD_CAMP_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Zulu -- Ikanda gives promotions to all melee infantry

UPDATE Language_en_US
SET Text = 'Nearby Mines gain +1 [ICON_PRODUCTION] Production. Grants unique promotions to all melee units, and reduces [ICON_STRENGTH] Crime slightly.'
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