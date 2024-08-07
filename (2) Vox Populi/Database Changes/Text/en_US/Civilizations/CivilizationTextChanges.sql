--------------------
-- America
--------------------
UPDATE Language_en_US
SET Text = 'Military Land Units start with the [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_SENTRY}[ENDCOLOR] Promotion. Tile purchase cost is reduced by 25% and is reset upon founding a City. +20 [ICON_PRODUCTION] for purchasing tiles, scaling with Era. Can purchase tiles owned by others.'
WHERE Tag = 'TXT_KEY_TRAIT_RIVER_EXPANSION';

UPDATE Language_en_US
SET Text = 'Unique American {TXT_KEY_UNIT_MUSKETMAN} that can ignore terrain movement cost and generate [ICON_GOLDEN_AGE] Golden Age Points from defeated enemies equal to 100% of their [ICON_STRENGTH] Combat Strength. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_BARRAGE_1}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MINUTEMAN';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_AMERICAN_MINUTEMAN} is the American unique unit, replacing the {TXT_KEY_UNIT_MUSKETMAN}. It can move through difficult terrain without penalty and gain [ICON_GOLDEN_AGE] Golden Age Points from destroying units.'
WHERE Tag = 'TXT_KEY_UNIT_AMERICAN_MINUTEMAN_STRATEGY';

--------------------
-- Arabia
--------------------
UPDATE Language_en_US
SET Text = 'One Thousand and One Nights'
WHERE Tag = 'TXT_KEY_TRAIT_LAND_TRADE_GOLD2_SHORT';

UPDATE Language_en_US
SET Text = 'When you complete a [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR], your [ICON_CAPITAL] Capital gains +1 [ICON_RESEARCH] Science and [ICON_CULTURE] Culture, and 15% towards the progress of a random [ICON_GREAT_PEOPLE] Great Person.'
WHERE Tag = 'TXT_KEY_TRAIT_LAND_TRADE_GOLD2';

UPDATE Language_en_US
SET Text = 'Completing a [ICON_CARAVAN]/[ICON_CARGO_SHIP] Trade Route originating here and targeting another Civilization triggers a (or strengthens an existing) [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR].[NEWLINE][NEWLINE][ICON_CARAVAN] Land trade routes gain 50% range. Incoming [ICON_INTERNATIONAL_TRADE] Trade Routes generate +1 [ICON_GOLD] Gold for the City, and +1 [ICON_GOLD] Gold for the Trade Route owner.[NEWLINE][NEWLINE]Nearby [ICON_RES_SPICES] Cinnamon: +1 [ICON_FOOD] Food, +1 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_SUGAR] Sugar: +1 [ICON_FOOD] Food, +1 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_BAZAAR_HELP';

UPDATE Language_en_US
SET Text = 'Unique Arabian replacement for the {TXT_KEY_BUILDING_MARKET}. In addition to the {TXT_KEY_BUILDING_MARKET}''s bonuses, {TXT_KEY_BUILDING_BAZAAR_DESC} generates [ICON_RESEARCH] Science and [ICON_PEACE] Faith, boosts [ICON_CARAVAN] land trade route range, and enables or strengthens Historic Events on [ICON_INTERNATIONAL_TRADE] Trade Route completion.'
WHERE Tag = 'TXT_KEY_BUILDING_BAZAAR_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Arabian {TXT_KEY_DESC_MOUNTED_BOWMAN} that does not require [ICON_RES_HORSE] {TXT_KEY_RESOURCE_HORSES} and can withdraw from melee combat once per turn. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_SPLASH_1}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_CAMEL_ARCHER';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_ARABIAN_CAMELARCHER} is the Arabian unique unit, replacing the {TXT_KEY_DESC_MOUNTED_BOWMAN}. It can do chip damage to a group of enemies adjacent to itself, and can retreat from melee combat once per turn. Does not require [ICON_RES_HORSE] {TXT_KEY_RESOURCE_HORSES} to be trained.'
WHERE Tag = 'TXT_KEY_UNIT_ARABIAN_CAMELARCHER_STRATEGY';

--------------------
-- Assyria
--------------------
UPDATE Language_en_US
SET Text = 'When you conquer a City, gain a Technology already known by the owner, or if impossible, 20 [ICON_RESEARCH] Science, scaling with Era and the City''s [ICON_CITIZEN] Population. All [ICON_GREAT_WORK] Great Works produce +3 [ICON_RESEARCH] Science.'
WHERE Tag = 'TXT_KEY_TRAIT_SLAYER_OF_TIAMAT';

UPDATE Language_en_US
SET Text = '+20% [ICON_RESEARCH] Science during [ICON_GOLDEN_AGE] Golden Ages. +1 [ICON_RESEARCH] Science for every 2 [ICON_CITIZEN] Citizens in the City, and all Libraries produce +3 [ICON_RESEARCH] Science. Comes with both a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_VP_GREATWRITING] Great Work of Writing and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] {TXT_KEY_BUILDING_LIBRARY} in the City.[NEWLINE][NEWLINE]Every [ICON_VP_GREATWRITING] Great Work of Writing you control grants +5 XP to all created Units (maximum of 45 XP).[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_RESEARCH] Illiteracy in all Cities. 1 Specialist in this City no longer produces [ICON_HAPPINESS_3] Unhappiness from [ICON_URBANIZATION] Urbanization.[NEWLINE][NEWLINE]Contains 3 slots for [ICON_VP_GREATWRITING] Great Works of Writing. +3 [ICON_PRODUCTION] Production and [ICON_RESEARCH] Science if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].[NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost increases based on the number of Cities you own.'
WHERE Tag = 'TXT_KEY_BUILDING_ROYAL_LIBRARY_HELP';

UPDATE Language_en_US
SET Text = 'Unique Assyrian replacement for the {TXT_KEY_BUILDING_NATIONAL_COLLEGE}. In addition to the bonuses of the {TXT_KEY_BUILDING_NATIONAL_COLLEGE}, the {TXT_KEY_BUILDING_ROYAL_LIBRARY_DESC} generates more [ICON_RESEARCH] Science and [ICON_CULTURE] Culture and boosts all Libraries. It contains more slots for [ICON_VP_GREATWRITING] Great Works of Writing, and comes with a free [ICON_VP_GREATWRITING] Great Work of Writing. Units trained in all cities gain extra XP based on the number of [ICON_VP_GREATWRITING] Great Works of Writing in your empire. Unlike the {TXT_KEY_BUILDING_NATIONAL_COLLEGE}, the {TXT_KEY_BUILDING_ROYAL_LIBRARY_DESC} is made available with the discovery of [COLOR_CYAN]{TXT_KEY_TECH_WRITING_TITLE}[ENDCOLOR], and comes with a free {TXT_KEY_BUILDING_LIBRARY} in the city instead of requiring one.'
WHERE Tag = 'TXT_KEY_BUILDING_ROYAL_LIBRARY_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Assyrian support Unit. Nearby owned Units gain +40% [ICON_STRENGTH] Combat Strength versus Cities if this Unit is adjacent to an enemy City, or +20% if this Unit is two tiles from an enemy City. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_MEDIC}[ENDCOLOR] and [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_MEDIC_II}[ENDCOLOR]. [COLOR_NEGATIVE_TEXT]Cannot attack or defend itself.[ENDCOLOR][NEWLINE][NEWLINE]May only have [COLOR_POSITIVE_TEXT]2[ENDCOLOR] Siege Towers active at one time.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ASSYRIAN_SIEGE_TOWER';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_ASSYRIAN_SIEGE_TOWER} is the Assyrian unique unit.[NEWLINE][NEWLINE]If adjacent to a City, the {TXT_KEY_UNIT_ASSYRIAN_SIEGE_TOWER} confers a huge combat bonus to all owned units also attacking the City. If it is two tiles away from the City, all owned units attacking the City receive half of this bonus. It can see further, and nearby units heal at a faster rate. It cannot attack or defend itself, so protect it at all costs! Complement a {TXT_KEY_UNIT_ASSYRIAN_SIEGE_TOWER} with melee or ranged units, and make sure it reaches its target before getting killed, and cities will fall much more quickly.[NEWLINE][NEWLINE]Does not obsolete. You cannot have more than 2 active Siege Towers at one time.'
WHERE Tag = 'TXT_KEY_UNIT_ASSYRIAN_SIEGE_TOWER_STRATEGY';

--------------------
-- Austria
--------------------
UPDATE Language_en_US
SET Text = 'Habsburg Diplomacy'
WHERE Tag = 'TXT_KEY_TRAIT_ANNEX_CITY_STATE_SHORT';

UPDATE Language_en_US
SET Text = '+50% rewards from [ICON_CITY_STATE] Quests. May arrange [ICON_RES_MARRIAGE] Marriages to Allied CS with [ICON_GOLD] Gold. While at peace with [COLOR_POSITIVE_TEXT]Married[ENDCOLOR] CS, gain 75 Resting [ICON_INFLUENCE] Influence, scaling with Era, and +15% [ICON_GREAT_PEOPLE] GP generation in the [ICON_CAPITAL] Capital.'
WHERE Tag = 'TXT_KEY_TRAIT_ANNEX_CITY_STATE';

UPDATE Language_en_US
SET Text = '+1 [ICON_FOOD] Food for every 5 [ICON_CITIZEN] Citizens in the City. +33% [ICON_GREAT_PEOPLE] Great People generation in this City. 15% of [ICON_FOOD] Food is carried over after a new [ICON_CITIZEN] Citizen is born.[NEWLINE][NEWLINE]10% of this City''s [ICON_CULTURE] Culture is converted into [ICON_RESEARCH] Science every turn.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty.'
WHERE Tag = 'TXT_KEY_BUILDING_COFFEE_HOUSE_HELP';

UPDATE Language_en_US
SET Text = 'Unique Austrian replacement for the {TXT_KEY_BUILDING_GROCER}. In addition to the {TXT_KEY_BUILDING_GROCER}''s bonuses, {TXT_KEY_BUILDING_COFFEE_HOUSE} boosts the city''s [ICON_GREAT_PEOPLE] Great People generation, and generates [ICON_RESEARCH] Science based on the city''s [ICON_CULTURE] Culture yield.'
WHERE Tag = 'TXT_KEY_BUILDING_COFFEE_HOUSE_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Austrian {TXT_KEY_DESC_CUIRASSIER} that excels at scouting and flanking. Can move faster and ignore Zone of Control.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_AUSTRIAN_HUSSAR';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_AUSTRIAN_HUSSAR} is the Austrian unique unit, replacing the {TXT_KEY_DESC_CUIRASSIER}. It is faster, can see further, and can ignore Zone of Control. It also has a slight attack bonus.'
WHERE Tag = 'TXT_KEY_UNIT_AUSTRIAN_HUSSAR_STRATEGY';

--------------------
-- Aztecs
--------------------
UPDATE Language_en_US
SET Text = 'Killing enemy Units grants 150% of their [ICON_STRENGTH] Strength as [ICON_GOLD] Gold and [ICON_PEACE] Faith. When you win a war ([COLOR_POSITIVE_TEXT]War Score[ENDCOLOR] of 25+), a [ICON_GOLDEN_AGE] Golden Age begins.'
WHERE Tag = 'TXT_KEY_TRAIT_CULTURE_FROM_KILLS';

UPDATE Language_en_US
SET Text = '+1 [ICON_FOOD] Food and [ICON_PRODUCTION] Production for every 4 [ICON_CITIZEN] Citizens in the City, and +10% [ICON_FOOD] Food during [ICON_GOLDEN_AGE] Golden Ages. +2 [ICON_FOOD] Food from Lakes and +1 [ICON_FOOD] Food from River tiles worked by this City.'
WHERE Tag = 'TXT_KEY_BUILDING_FLOATING_GARDENS_HELP';

UPDATE Language_en_US
SET Text = 'Unique Aztec replacement for both the {TXT_KEY_BUILDING_WELL} and the {TXT_KEY_BUILDING_WATERMILL}. It further increases the City''s [ICON_FOOD] Food and [ICON_PRODUCTION] Production output, especially during Golden Ages, and boosts lakes and river tiles.'
WHERE Tag = 'TXT_KEY_BUILDING_FOATING_GARDENS_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Aztec {TXT_KEY_UNIT_WARRIOR} that excels at Forest and Jungle warfare and recovers HP when it kills an enemy unit. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_WOODSMAN}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_JAGUAR';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_AZTEC_JAGUAR} is the Aztec unique unit, replacing the {TXT_KEY_UNIT_WARRIOR}. It is much stronger, especially in forests and jungles, and can heal when defeating an enemy unit.'
WHERE Tag = 'TXT_KEY_UNIT_AZTEC_JAGUAR_STRATEGY';

--------------------
-- Babylon
--------------------
UPDATE Language_en_US
SET Text = 'Receive a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_SCIENTIST] Great Scientist when you discover [COLOR_CYAN]Writing[ENDCOLOR], and [ICON_GREAT_SCIENTIST] Great Scientists are earned 50% faster than normal. Investing [ICON_INVEST] Gold in Buildings reduces their [ICON_PRODUCTION] Production cost by an additional 15%.'
WHERE Tag = 'TXT_KEY_TRAIT_INGENIOUS';

UPDATE Language_en_US
SET Text = 'Unique Babylonian {TXT_KEY_UNIT_ARCHER} that is stronger and inflicts bonus damage on wounded units when attacking.'
WHERE Tag = 'TXT_KEY_CIV5_BABYLON_BOWMAN_HELP';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_BABYLON_BOWMAN} is the Babylonian unique unit, replacing the {TXT_KEY_UNIT_ARCHER}. It is defensively stronger, allowing placement on the front line. It can quickly shred through wounded melee units after being attacked by them.'
WHERE Tag = 'TXT_KEY_CIV5_BABYLON_BOWMAN_STRATEGY';

UPDATE Language_en_US
SET Text = '+10% [ICON_SILVER_FIST] Military Supply from Population. +1 [ICON_RANGE_STRENGTH] City Strike Range. Great Scientists provide 5% more [ICON_RESEARCH] Science when used to discover new Technology.[NEWLINE][NEWLINE][ICON_CITY_STATE] Empire Size Modifier is reduced by 5% in this City.'
WHERE Tag = 'TXT_KEY_CIV5_BABYLON_WALLS_INFO';

UPDATE Language_en_US
SET Text = 'Unique Babylonian replacement for the {TXT_KEY_BUILDING_WALLS}. In addition to the {TXT_KEY_BUILDING_WALLS}'' bonuses, {TXT_KEY_BUILDING_WALLS_OF_BABYLON_DESC} provides more City Strength and Hit Points, generates [ICON_RESEARCH] Science and [ICON_GREAT_SCIENTIST] Great Scientist points, and improves the yields when [ICON_GREAT_SCIENTIST] Great Scientists are expended to discover technology.'
WHERE Tag = 'TXT_KEY_CIV5_BABYLON_WALLS_STRATEGY';

--------------------
-- Brazil
--------------------
UPDATE Language_en_US
SET Text = '"We Love the King Day" becomes Carnival, adding -50% [ICON_HAPPINESS_3] Unhappiness from Needs. When a [ICON_GOLDEN_AGE] Golden Age begins, 30% of [ICON_GOLDEN_AGE] Golden Age Points convert into [ICON_GOLD] Gold and [ICON_TOURISM] Tourism, and Cities gain 10 turns of Carnival.'
WHERE Tag = 'TXT_KEY_TRAIT_CARNIVAL';

UPDATE Language_en_US
SET Text = 'A [ICON_RES_BRAZILWOOD] {TXT_KEY_RESOURCE_BRAZILWOOD} Resource appears on the tile. Can only be built on Forest or Jungle tiles without access to Fresh Water. Cannot be built adjacent to another {TXT_KEY_IMPROVEMENT_BRAZILWOOD_CAMP}.'
WHERE Tag = 'TXT_KEY_BUILD_BRAZILWOOD_CAMP_HELP';

UPDATE Language_en_US
SET Text = 'It will increase [ICON_GOLD] Gold and [ICON_CULTURE] Culture, and spawns a [ICON_RES_BRAZILWOOD] {TXT_KEY_RESOURCE_BRAZILWOOD} Resource under this tile.'
WHERE Tag = 'TXT_KEY_BUILD_BRAZILWOOD_CAMP_REC';

UPDATE Language_en_US
SET Text = 'Can only be built on Jungle or Forest tiles without access to Fresh Water, and cannot be built adjacent to another {TXT_KEY_IMPROVEMENT_BRAZILWOOD_CAMP}. It spawns and connects a [ICON_RES_BRAZILWOOD] {TXT_KEY_RESOURCE_BRAZILWOOD} Resource under the tile.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_BRAZILWOOD_CAMP_HELP';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CIV5_IMPROVEMENTS_BRAZILWOOD_CAMP_HELP}[NEWLINE][NEWLINE]Brazilwood is a tropical hardwood whose bark produces a bright crimson and deep purple extract used in dyes. Its wood is dense, and is prized for crafting stringed instruments (especially the bows) and cabinetry. The harvesting of brazilwood did not cease until 1875, by which time synthetic dyes dominated the textile industry and other hardwoods had been found to be better for musical instruments.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_BRAZILWOOD_CAMP_TEXT';

--------------------
-- Byzantium
--------------------
UPDATE Language_en_US
SET Text = 'Can always found a [ICON_RELIGION] Religion, receives 1 [COLOR_POSITIVE_TEXT]Additional[ENDCOLOR] Belief when founding, and may select Beliefs present in other [ICON_RELIGION] Religions. -15% [ICON_PEACE] Faith purchase costs, and may purchase unlocked [ICON_GREAT_PEOPLE] Great People starting in the Classical Era.'
WHERE Tag = 'TXT_KEY_TRAIT_EXTRA_BELIEF';

UPDATE Language_en_US
SET Text = 'Unique Byzantine {TXT_KEY_UNIT_KNIGHT} that is slower but can fortify and benefit from defensive terrain; has a [ICON_STRENGTH] Combat Strength bonus in open terrain; and assault Cities better. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_COVER_1}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BYZANTINE_CATAPHRACT';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_BYZANTINE_CATAPHRACT} is the Byzantine unique unit, replacing the {TXT_KEY_UNIT_KNIGHT}. It is slower but much more powerful, especially in open terrain. It is able to fortify and benefit from defensive terrain unlike the {TXT_KEY_UNIT_KNIGHT}. Its penalty for attacking cities is also not as severe.'
WHERE Tag = 'TXT_KEY_UNIT_BYZANTINE_CATAPHRACT_STRATEGY';

--------------------
-- Carthage
--------------------
UPDATE Language_en_US
SET Text = '+100 [ICON_GOLD] Gold and +25 [ICON_RESEARCH] Science when your all-time highest tradeable quantity of a Luxury increases, and +5 XP to [ICON_GOLD] Gold purchased Units, both scaling with Era. +100% Resource Diversity on [ICON_INTERNATIONAL_TRADE] Trade Routes.'
WHERE Tag = 'TXT_KEY_TRAIT_PHOENICIAN_HERITAGE';

UPDATE Language_en_US
SET Text = 'Unique Carthaginian {TXT_KEY_UNIT_TRIREME} that specializes in naval assaults and capturing coastal cities. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_BOARDING_PARTY_4}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_CARTHAGINIAN_QUINQUEREME';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_CARTHAGINIAN_QUINQUEREME} is the Carthaginian unique unit, replacing the {TXT_KEY_UNIT_TRIREME}. It can ignore Zone of Control and has a huge attack bonus against cities.'
WHERE Tag = 'TXT_KEY_UNIT_CARTHAGINIAN_QUINQUEREME_STRATEGY';

--------------------
-- Celts
--------------------
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Has a unique set of Pantheon Beliefs[ENDCOLOR] that no one else can benefit from. Owned Cities with your [ICON_RELIGION_PANTHEON] Pantheon or [ICON_RELIGION] Religion neither [COLOR_NEGATIVE_TEXT]generate[ENDCOLOR] nor receive foreign Religious Pressure and generate +3 [ICON_PEACE] Faith.'
WHERE Tag = 'TXT_KEY_TRAIT_FAITH_FROM_NATURE';

UPDATE Language_en_US
SET Text = 'Unique Celtic replacement for the {TXT_KEY_BUILDING_CIRCUS}. In addition to the {TXT_KEY_BUILDING_CIRCUS}'' bonuses, {TXT_KEY_BUILDING_CEILIDH_HALL} provides more [ICON_CULTURE] Culture and a longer "We Love the King Day", and generates [ICON_CULTURE] Culture and [ICON_PEACE] Faith. It also contains a Musician slot.[NEWLINE][NEWLINE]Each unique Celtic pantheon belief further boosts this building.'
WHERE Tag = 'TXT_KEY_BUILDING_CEILIDH_HALL_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Celtic {TXT_KEY_UNIT_SPEARMAN} that excels at hill, tundra, and snow warfare. Moves at double speed on hill, tundra, and snow, and additionally gains a [ICON_STRENGTH] Combat Strength bonus if these tiles are featureless. Consumes no [ICON_MOVES] Moves when pillaging, and gains 200% of the opponent''s [ICON_STRENGTH] Combat Strength as [ICON_PEACE] Faith when defeating an enemy Unit. Does not start with [COLOR_NEGATIVE_TEXT]{TXT_KEY_PROMOTION_FORMATION_1}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_CELT_PICTISH_WARRIOR';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_CELT_PICTISH_WARRIOR} is the Celtic unique unit, replacing the {TXT_KEY_UNIT_SPEARMAN}. It moves and fights better on hill, tundra, and snow; can pillage tiles without spending [ICON_MOVES] movement; and gains [ICON_PEACE] Faith when defeating enemy units. It is available with the discovery of [COLOR_CYAN]{TXT_KEY_TECH_MINING_TITLE}[ENDCOLOR], earlier than the {TXT_KEY_UNIT_SPEARMAN}, but does not start with the {TXT_KEY_PROMOTION_FORMATION_1} promotion, which makes it weaker against mounted units.'
WHERE Tag = 'TXT_KEY_UNIT_CELT_PICTISH_WARRIOR_STRATEGY';

--------------------
-- China
--------------------
UPDATE Language_en_US
SET Text = 'Mandate of Heaven'
WHERE Tag = 'TXT_KEY_TRAIT_ART_OF_WAR_SHORT';

UPDATE Language_en_US
SET Text = 'Creating [ICON_GREAT_WORK] Great Works or gaining Cities grants 5 Turns of "We Love the Empress Day", and a permanent +2 [ICON_FOOD] Food in all Cities. The permanent [ICON_FOOD] Food declines by 50% on Era Change.'
WHERE Tag = 'TXT_KEY_TRAIT_ART_OF_WAR';

UPDATE Language_en_US
SET Text = 'Unique Chinese {TXT_KEY_UNIT_CROSSBOWMAN} that specializes in city defenses and dealing splash damage.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_CHUKONU';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_CHINESE_CHUKONU} is the Chinese unique unit, replacing the {TXT_KEY_UNIT_CROSSBOWMAN}. It deals splash damage and gains additional [ICON_STRENGTH] Combat Strength when adjacent to cities, giving it a hefty punch on offense and defense. This unit is particularly deadly when garrisoned in a City that is under attack, and when fighting in packs.'
WHERE Tag = 'TXT_KEY_UNIT_CHINESE_CHUKONU_STRATEGY';

--------------------
-- Denmark
--------------------
UPDATE Language_en_US
SET Text = 'Mycel Hæþen Here'
WHERE Tag = 'TXT_KEY_TRAIT_VIKING_FURY_SHORT';

UPDATE Language_en_US
SET Text = 'Embarked Units gain +1 [ICON_MOVES] Movement and pay just 1 [ICON_MOVES] Movement to Embark or Disembark. When [COLOR_POSITIVE_TEXT]Pillaging[ENDCOLOR], Land and Naval Melee Units earn more HP and [ICON_GOLD] Gold, deal 10 damage to adjacent enemies, and spend no [ICON_MOVES] Movement.'
WHERE Tag = 'TXT_KEY_TRAIT_VIKING_FURY';

UPDATE Language_en_US
SET Text = 'Unique Danish {TXT_KEY_UNIT_PIKEMAN} that is adept at attacking from the coast. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_AMPHIBIOUS}[ENDCOLOR] and [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_CHARGE}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_CIV5_DENMARK_BERSERKER_HELP';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_DANISH_BERSERKER} is the Danish unique unit, replacing the {TXT_KEY_UNIT_PIKEMAN}. It can cross rivers and attack while embarked with no penalty, and has a [ICON_STRENGTH] Combat Strength bonus against wounded units. It is also faster, allowing it to catch up to wounded units to deal the final strike. It is available in the [COLOR_CYAN]Classical Era[ENDCOLOR] with the discovery of [COLOR_CYAN]{TXT_KEY_TECH_METAL_CASTING_TITLE}[ENDCOLOR], earlier than the {TXT_KEY_UNIT_PIKEMAN}.'
WHERE Tag = 'TXT_KEY_CIV5_DENMARK_BERSERKER_STRATEGY';

--------------------
-- Egypt
--------------------
UPDATE Language_en_US
SET Text = '+20% [ICON_PRODUCTION] Production towards [ICON_WONDER] Wonders (+40% during [ICON_GOLDEN_AGE] Golden Ages). [ICON_VP_ARTIFACT] Artifacts gain +5 [ICON_RESEARCH] Science and [ICON_CULTURE] Culture, and Landmarks gain +5 [ICON_GOLD] Gold and [ICON_TOURISM] Tourism.'
WHERE Tag = 'TXT_KEY_TRAIT_WONDER_BUILDER';

UPDATE Language_en_US
SET Text = 'Receive a free Egyptian [ICON_VP_ARTIFACT] Artifact when constructed.[NEWLINE][NEWLINE][ICON_CARAVAN] Land Trade Routes gain +50% Range and +3 [ICON_GOLD] Gold. Completing a [ICON_CARAVAN] Land Trade Route originating here and targeting another Civilization triggers a (or strengthens an existing) [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR].[NEWLINE][NEWLINE]+1 [ICON_GOLD] Gold and [ICON_FOOD] Food for every 1.5 Desert or 1.5 Tundra tiles worked by the City.[NEWLINE][NEWLINE]Contains 1 slot for [ICON_GREAT_WORK] Great Work of Art or Artifact.[NEWLINE][NEWLINE]Nearby [ICON_RES_TRUFFLES] Truffles: +2 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COTTON] Cotton: +1 [ICON_PRODUCTION] Production, +1 [ICON_CULTURE] Culture.[NEWLINE]Neabry [ICON_RES_FUR] Furs: +1 [ICON_GOLD] Gold, +1 [ICON_PRODUCTION] Production.'
WHERE Tag = 'TXT_KEY_BUILDING_BURIAL_TOMB_HELP';

UPDATE Language_en_US
SET Text = 'Unique Egyptian replacement for the {TXT_KEY_BUILDING_CARAVANSARY}. In addition to the {TXT_KEY_BUILDING_CARAVANSARY}''s bonuses, {TXT_KEY_BUILDING_BURIAL_TOMB_DESC} produces [ICON_PEACE] Faith, comes with a free [ICON_VP_ARTIFACT] Artifact, and generates even more [ICON_FOOD] Food and [ICON_GOLD] Gold from worked Desert and Tundra tiles.'
WHERE Tag = 'TXT_KEY_BUILDING_BURIAL_TOMB_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Egyptian {TXT_KEY_UNIT_CHARIOT_ARCHER} that does not require [ICON_RES_HORSE] {TXT_KEY_RESOURCE_HORSES} and grants [ICON_PRODUCTION] Production when defeating enemies equal to 150% of their [ICON_STRENGTH] Combat Strength.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_EGYPTIAN_WAR_CHARIOT';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_EGYPT_WARCHARIOT} is the Egyptian unique unit, replacing the {TXT_KEY_UNIT_CHARIOT_ARCHER}. It can gain [ICON_PRODUCTION] Production from destroying units. Does not require [ICON_RES_HORSE] {TXT_KEY_RESOURCE_HORSES} to be trained.'
WHERE Tag = 'TXT_KEY_UNIT_EGYPTIAN_WARCHARIOT_STRATEGY';

--------------------
-- England
--------------------
UPDATE Language_en_US
SET Text = 'Perfidious Albion'
WHERE Tag = 'TXT_KEY_TRAIT_OCEAN_MOVEMENT_SHORT';

UPDATE Language_en_US
SET Text = '+1 [ICON_MOVES] Movement for Naval and Embarked Units, and -25% Naval Unit [ICON_GOLD] Gold maintenance. +15 [ICON_SPY] City Security in all owned Cities. [ICON_SPY] Spies travel to any City in 1 turn and are 25% stronger in foreign Cities. Starts with a [ICON_SPY] Spy.'
WHERE Tag = 'TXT_KEY_TRAIT_OCEAN_MOVEMENT';

UPDATE Language_en_US
SET Text = 'Unique English {TXT_KEY_UNIT_FRIGATE} that specializes in wresting control of sea and coast with extra vision and splash damage. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_SPLASH_1}[ENDCOLOR] and [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_SPLASH_2}[ENDCOLOR].[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]Land attacks can only be performed on Coastal tiles.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SHIPOFTHELINE';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_ENGLISH_SHIPOFTHELINE} is the English unique unit, replacing the {TXT_KEY_UNIT_FRIGATE}. It can deal splash damage to clear out masses of enemy ships and land units on coastal tiles. It can also see further, allowing it to spot enemies in a significantly larger section of ocean.'
WHERE Tag = 'TXT_KEY_UNIT_ENGLISH_SHIPOFTHELINE_STRATEGY';

--------------------
-- Ethiopia
--------------------
UPDATE Language_en_US
SET Text = 'Solomonic Wisdom'
WHERE Tag = 'TXT_KEY_TRAIT_BONUS_AGAINST_TECH_SHORT';

UPDATE Language_en_US
SET Text = 'When you complete a Policy Branch, adopt new Beliefs, or choose your first Ideology, receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Technology. +1 [ICON_PEACE] Faith from Strategic Resources.'
WHERE Tag = 'TXT_KEY_TRAIT_BONUS_AGAINST_TECH';

UPDATE Language_en_US
SET Text = 'Unique Ethiopian {TXT_KEY_UNIT_RIFLEMAN} that specializes in fighting in Friendly Territory especially when near the [ICON_CAPITAL] Capital. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_COVER_1}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MEHAL_SEFARI';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_MEHAL_SEFARI} is the Ethiopian unique unit, replacing the {TXT_KEY_UNIT_RIFLEMAN}. It possesses significant combat bonuses when fighting in friendly land, especially when near your [ICON_CAPITAL] Capital. It is also slightly cheaper to train.'
WHERE Tag = 'TXT_KEY_UNIT_MEHAL_SEFARI_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Ethiopian replacement for the {TXT_KEY_BUILDING_MONUMENT_DESC}. In addition to the {TXT_KEY_BUILDING_MONUMENT_DESC}''s bonuses, {TXT_KEY_BUILDING_STELE} generates [ICON_PEACE] Faith as well as a {TXT_KEY_BUILDING_SHRINE}, and produces extra [ICON_PEACE] Faith during [ICON_GOLDEN_AGE] Golden Ages.'
WHERE Tag = 'TXT_KEY_BUILDING_STELE_STRATEGY';

--------------------
-- France
--------------------
UPDATE Language_en_US
SET Text = 'La Grande Armée'
WHERE Tag = 'TXT_KEY_TRAIT_ENHANCED_CULTURE_SHORT';

UPDATE Language_en_US
SET Text = 'Melee and Gunpowder Units may enlist defeated enemies, which do not consume [ICON_WAR] Military Supply. +1 [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism in every City for every 10 Military Units. +25% [ICON_SILVER_FIST] Military Supply from Population.'
WHERE Tag = 'TXT_KEY_TRAIT_ENHANCED_CULTURE';

UPDATE Language_en_US
SET Text = 'Unique French {TXT_KEY_UNIT_SPANISH_TERCIO} that excels at delivering a fast attack into the enemy. Can move faster and ignore Zone of Control.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MUSKETEER';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_FRENCH_MUSKETEER} is the French unique unit, replacing the {TXT_KEY_UNIT_SPANISH_TERCIO}. It is faster, significantly more powerful, and ignores Zone of Control.'
WHERE Tag = 'TXT_KEY_UNIT_FRENCH_MUSKETEER_STRATEGY';

UPDATE Language_en_US
SET Text = 'Can only be built adjacent to a Luxury resource, and cannot be adjacent to another {TXT_KEY_IMPROVEMENT_CHATEAU}. +50% [ICON_STRENGTH] Defensive Strength for any Unit stationed in this tile. Contains [COLOR_POSITIVE_TEXT]Fortifications[ENDCOLOR], allowing owned Units to stay stationed in this tile after attacking.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_CHATEAU_HELP';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CIV5_IMPROVEMENTS_CHATEAU_HELP}[NEWLINE][NEWLINE]A chateau is a manor house or country home of gentry, usually without fortifications. In the Middle Ages, a chateau was largely self-sufficient, being supported by the lord''s demesne (hereditary lands). In the 1600s, the wealthy and aristocratic French lords dotted the countryside with elegant, luxuriant, architecturally refined mansions such as the Chateau de Maisons. Today, the term chateau is loosely used; for instance, it is common for any winery or inn, no matter how humble, to prefix its name with "Chateau".'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_CHATEAU_TEXT';

UPDATE Language_en_US
SET Text = '+50% [ICON_STRENGTH] Defensive Strength for any Unit stationed in this tile. Can only be built adjacent to a Luxury resource, and cannot be adjacent to another {TXT_KEY_IMPROVEMENT_CHATEAU}. Contains [COLOR_POSITIVE_TEXT]Fortifications[ENDCOLOR], allowing owned Units to stay stationed in this tile after attacking.'
WHERE Tag = 'TXT_KEY_BUILD_CHATEAU_HELP';

UPDATE Language_en_US
SET Text = 'It will increase [ICON_FOOD] Food, [ICON_Gold] Gold, and [ICON_CULTURE] Culture. Removes any feature on the tile. Provides a defensive bonus for own units on the tile.'
WHERE Tag = 'TXT_KEY_BUILD_CHATEAU_REC';

--------------------
-- Germany
--------------------
UPDATE Language_en_US
SET Text = 'Blood and Iron'
WHERE Tag = 'TXT_KEY_TRAIT_CONVERTS_LAND_BARBARIANS_SHORT';

UPDATE Language_en_US
SET Text = '+2 [ICON_RESEARCH] Science in [ICON_CAPITAL] Capital per [ICON_CITY_STATE] City-State Friend, and additionally +2 [ICON_CULTURE] Culture per Ally, scaling with Era. Gain +1 [ICON_INFLUENCE] Influence per turn with [ICON_CITY_STATE] City-States for each gifted Unit (lost when killed or upgraded).'
WHERE Tag = 'TXT_KEY_TRAIT_CONVERTS_LAND_BARBARIANS';

UPDATE Language_en_US
SET Text = '+5% [ICON_PRODUCTION] Production for each [ICON_INTERNATIONAL_TRADE] Trade Route your Civilization has with a [ICON_CITY_STATE] City-State. 10% of the City''s [ICON_GOLD] Gold output is converted into [ICON_RESEARCH] Science every turn.[NEWLINE][NEWLINE]Incoming [ICON_ARROW_LEFT] Trade Routes generate +3 [ICON_GOLD] Gold for the City, and +3 [ICON_GOLD] Gold for the [ICON_ARROW_RIGHT] Trade Route owner.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty.[NEWLINE][NEWLINE]Nearby [ICON_RES_BANANA] Bananas: +2 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COFFEE] Coffee: +1 [ICON_GOLD] Gold, +2 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_TEA] Tea: +2 [ICON_GOLD] Gold, +1 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_TOBACCO] Tobacco: +3 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_HANSE_HELP';

UPDATE Language_en_US
SET Text = 'Unique German replacement for the {TXT_KEY_BUILDING_MINT}. In addition to the {TXT_KEY_BUILDING_MINT}''s bonuses, {TXT_KEY_BUILDING_HANSE_DESC} produces more [ICON_GOLD] Gold and [ICON_CULTURE] Culture; boosts [ICON_INTERNATIONAL_TRADE] international Trade Routes even more; generates [ICON_RESEARCH] Science based on the city''s [ICON_CULTURE] Gold yield; and provides a [ICON_PRODUCTION] Production bonus depending on the number of your [ICON_INTERNATIONAL_TRADE] Trade Routes connecting to City-States.'
WHERE Tag = 'TXT_KEY_BUILDING_HANSE_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique German {TXT_KEY_UNIT_SPANISH_TERCIO} that specializes as mercenaries, having no experience penalty or cooldown while being able to move immediately when purchased. Has a [ICON_STRENGTH] Combat Strength bonus against enemy Units that are at full health. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_FORMATION_2}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_LANDSKNECHT';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_GERMAN_LANDSKNECHT} is the German unique unit, replacing the {TXT_KEY_UNIT_SPANISH_TERCIO}. It has a combat bonus against full health units, and a increased bonus against mounted units. It is also slightly cheaper to train or purchase, and gain full XP and can move immediately when bought.[NEWLINE][NEWLINE]Use their fast deployment and their bonus against full health units to surprise your opponents in the first turns of your wars and break their lines quickly. Its cheap cost also makes it an excellent gift. Buy several units at once, walk them over to a nearby City State, and reap the benefits of both short- and long-term influence.'
WHERE Tag = 'TXT_KEY_UNIT_GERMAN_LANDSKNECHT_STRATEGY';

UPDATE Language_en_US
SET Text = 'The landsknecht were German mercenary pike and shot of the 15th and 16th century. Created in imitation of the legendary Swiss Reisläufer, the landsknecht would eventually supplant them as the best mercenary infantry available in Europe, fighting in almost every major engagement in the 16th century - often on both sides. Landsknechts carried a variety of weapons into battle in addition to their pikes, which could be as long as twenty feet, including halberds, massive two-handed swords, and ranged weapons. The landsknechts pioneered the inclusion of crossbows into their pike formations to break the momentum of heavy cavalry charges, a popular tactic at the time. By the late 15th century, Arquebuses replaced crossbows as the weapon of choice, making the Landsknechts the earliest example of Pike & Shot formations, ushering in a new era of infantry tactics.'
WHERE Tag = 'TXT_KEY_CIVILOPEDIA_UNITS_MEDIEVAL_LANDSKNECHT_TEXT';

UPDATE Language_en_US
SET Text = 'Aachen'
WHERE Tag = 'TXT_KEY_CITY_NAME_ESSEN';

UPDATE Language_en_US
SET Text = 'Regensburg'
WHERE Tag = 'TXT_KEY_CITY_NAME_DUISBURG';

UPDATE Language_en_US
SET Text = 'Augsburg'
WHERE Tag = 'TXT_KEY_CITY_NAME_BOCHUM';

UPDATE Language_en_US
SET Text = 'Kiel'
WHERE Tag = 'TXT_KEY_CITY_NAME_BONN';

UPDATE Language_en_US
SET Text = 'Freiburg'
WHERE Tag = 'TXT_KEY_CITY_NAME_MULHEIM';

UPDATE Language_en_US
SET Text = 'Heidelberg'
WHERE Tag = 'TXT_KEY_CITY_NAME_DARMSTADT';

UPDATE Language_en_US
SET Text = 'Bamberg'
WHERE Tag = 'TXT_KEY_CITY_NAME_CHEMNITZ';

UPDATE Language_en_US
SET Text = 'Magdeburg'
WHERE Tag = 'TXT_KEY_CITY_NAME_OBERHAUSEN';

UPDATE Language_en_US
SET Text = 'Erfurt'
WHERE Tag = 'TXT_KEY_CITY_NAME_BIELEFELD';

UPDATE Language_en_US
SET Text = 'Trier'
WHERE Tag = 'TXT_KEY_CITY_NAME_HERNE';

UPDATE Language_en_US
SET Text = 'Konstanz'
WHERE Tag = 'TXT_KEY_CITY_NAME_WOLFSBURG';

UPDATE Language_en_US
SET Text = 'Mainz'
WHERE Tag = 'TXT_KEY_CITY_NAME_HAMM';

UPDATE Language_en_US
SET Text = 'Paderborn'
WHERE Tag = 'TXT_KEY_CITY_NAME_NEUSS';

UPDATE Language_en_US
SET Text = 'Lübeck'
WHERE Tag = 'TXT_KEY_CITY_NAME_HAGEN';

UPDATE Language_en_US
SET Text = 'Duisburg'
WHERE Tag = 'TXT_KEY_CITY_NAME_HILDESHEIM';

UPDATE Language_en_US
SET Text = 'Braunschweig'
WHERE Tag = 'TXT_KEY_CITY_NAME_BRAUNSCHWEIG';

--------------------
-- Greece
--------------------
UPDATE Language_en_US
SET Text = '[ICON_INFLUENCE] Influence degrades at half and recovers at twice the normal rate. +5% [ICON_STRENGTH] Combat Strength of owned and Allied Units for each [ICON_CITY_STATE] City-State alliance (up to +25%). Treat neutral [ICON_CITY_STATE] City-State territory as friendly territory.'
WHERE Tag = 'TXT_KEY_TRAIT_CITY_STATE_FRIENDSHIP';

UPDATE Language_en_US
SET Text = 'Unique Greek {TXT_KEY_UNIT_SPEARMAN} that is stronger and produce double the [ICON_GREAT_GENERAL] Great General Points from combat than normal. Gains [ICON_STRENGTH] Combat Strength for each adjacent owned land unit.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_HOPLITE';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_GREEK_HOPLITE} is the Greek unique unit, replacing the {TXT_KEY_UNIT_SPEARMAN}. It is stronger when adjacent to more owned land units, and generates[ICON_GREAT_GENERAL] Great Generals faster through combat.'
WHERE Tag = 'TXT_KEY_UNIT_GREEK_HOPLITE_STRATEGY';

--------------------
-- Huns
--------------------
UPDATE Language_en_US
SET Text = 'Inflict +100% and receive -50% [ICON_HAPPINESS_3] [COLOR_POSITIVE_TEXT]War Weariness[ENDCOLOR]. +10% [ICON_STRENGTH] Combat Strength for each subsequent attack on the same target in a turn. Gain 1 [ICON_GOLD] Gold and [ICON_CULTURE] Culture for every 2 damage dealt to Cities, scaling with Era.'
WHERE Tag = 'TXT_KEY_TRAIT_RAZE_AND_HORSES';

UPDATE Language_en_US
SET Text = 'Unique Hunnic {TXT_KEY_UNIT_MONGOL_KESHIK} that specializes in hit-and-run attacks focusing on one target. Gains combat bonuses for subsequent attacks on the same target in a turn. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_BARRAGE_1}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_HUN_HORSE_ARCHER';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_HUN_HORSE_ARCHER} is the Hunnic unique unit, replacing the {TXT_KEY_UNIT_MONGOL_KESHIK}. It is stronger when attacking units that have already been hit multiple times in the same turn.'
WHERE Tag = 'TXT_KEY_UNIT_HUN_HORSE_ARCHER_STRATEGY';

--------------------
-- Inca
--------------------
UPDATE Language_en_US
SET Text = 'Units can cross Mountains and ignore terrain costs when entering any Hill or Mountain. Cities, Roads, and Railroads may be built on Mountains. Mountains produce +1 [ICON_FOOD] Food, [ICON_GOLD] Gold, and [ICON_RESEARCH] Science when worked, scaling with Era.'
WHERE Tag = 'TXT_KEY_TRAIT_GREAT_ANDEAN_ROAD';

UPDATE Language_en_US
SET Text = 'Waraq''Ak'
WHERE Tag = 'TXT_KEY_UNIT_INCAN_SLINGER';

UPDATE Language_en_US
SET Text = 'Unique Incan {TXT_KEY_UNIT_VP_SLINGER} that has increased [ICON_RANGE_STRENGTH] Attack Range, ignores enemy [COLOR_POSITIVE_TEXT]Zone of Control[ENDCOLOR], and can daze enemy units, lowering their [ICON_STRENGTH] Combat Strength.'
WHERE Tag = 'TXT_KEY_CIV5_INCA_SLINGER_HELP';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_INCAN_SLINGER} is the Incan unique unit, replacing the {TXT_KEY_UNIT_VP_SLINGER}. It has superior range and can daze enemy units which lowers their [ICON_STRENGTH] Combat Strength for a long time.[NEWLINE][NEWLINE]Use it to harass your foes and take down Barbarian camps quickly.'
WHERE Tag = 'TXT_KEY_CIV5_INCA_SLINGER_STRATEGY';

UPDATE Language_en_US
SET Text = 'Pata-Pata'
WHERE Tag = 'TXT_KEY_IMPROVEMENT_TERRACE_FARM';

UPDATE Language_en_US
SET Text = 'Can only be built on Hill. +1 [ICON_FOOD] Food for each adjacent Mountain or {TXT_KEY_IMPROVEMENT_TERRACE_FARM}. All adjacent Farms gain +1 [ICON_FOOD] Food.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_TERRACE_FARM_HELP';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CIV5_IMPROVEMENTS_TERRACE_FARM_HELP}[NEWLINE][NEWLINE]Terrace Farming developed in mountainous areas simultaneously around the world, including Bali, the Philippines, China, and Peru. These skinny, stepped fields are cut into hill and mountain sides, preventing the run off of irrigation water and providing space for arable land, usually where none previously was possible. The Incas in particular were masters of terrace farming, a technique handed down to them by the older Wari culture, and called Pata-Pata (meaning levelled place) in their native Quechua. The Inca erected large, drystone walls to hold their terraces in place and constructed systems of canals and aqueducts to provide the terraces with constant water, increasing the land''s fertility. The Incan terracing techniques were so successful that modern Peruvian farmers still employ them in their farms today.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_TERRACE_FARM_TEXT';

UPDATE Language_en_US
SET Text = 'Construct a [LINK=IMPROVEMENT_TERRACE_FARM]{TXT_KEY_IMPROVEMENT_TERRACE_FARM}[\LINK]'
WHERE Tag = 'TXT_KEY_BUILD_TERRACE_FARM';

--------------------
-- India
--------------------
UPDATE Language_en_US
SET Text = 'Starts with a [ICON_RELIGION_PANTHEON] Pantheon. [ICON_PROPHET] Great Prophets require 35% less [ICON_PEACE] Faith. You may spend a Prophet''s first spread action to Found or Enhance a [ICON_RELIGION] Religion, or build a Holy Site, without consuming the Prophet. [COLOR_NEGATIVE_TEXT]Cannot build [ICON_MISSIONARY] Missionaries.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_TRAIT_POPULATION_GROWTH';

UPDATE Language_en_US
SET Text = 'Mahatma'
WHERE Tag = 'TXT_KEY_TRAIT_POPULATION_GROWTH_SHORT';

UPDATE Language_en_US
SET Text = 'Naga-Malla'
WHERE Tag = 'TXT_KEY_UNIT_INDIAN_WARELEPHANT';

UPDATE Language_en_US
SET Text = 'A terrifying sight on the battlefield, often as dangerous to its own side as it was to the enemy, the Naga-Malla (the Hindu term for Fighting Elephant) could be considered the first weapon of mass destruction. Indians used elephants in war until the advent of gunpowder rendered them obsolete. In pre-gunpowder battle the war elephant served two primary purposes. First, their scent absolutely terrified horses, rendering enemy cavalry useless. Second, they could break through even the strongest line of infantry, crushing a wall of spears that no horse could ever surmount. Elephants were incredibly difficult to kill and history records them surviving sixty or more arrows. The primary problem with elephants was their tendency to go berserk with pain or rage, at which point they became impossible for their rider to control. Elephant riders often carried a spike and mallet that they could use to kill the animals if they attacked their own forces.'
WHERE Tag = 'TXT_KEY_CIV5_ANTIQUITY_INDIANWARELEPHANT_TEXT';

UPDATE Language_en_US
SET Text = 'Unique Indian {TXT_KEY_DESC_CUIRASSIER} that does not require [ICON_RES_HORSE] {TXT_KEY_RESOURCE_HORSES}. Trades 2 [ICON_MOVES] [COLOR_NEGATIVE_TEXT]Movement[ENDCOLOR] and [COLOR_NEGATIVE_TEXT]{TXT_KEY_PROMOTION_SKIRMISHER_DOCTRINE}[ENDCOLOR] for immense [ICON_STRENGTH] Combat Strength and [ICON_RANGE_STRENGTH] Ranged Combat Strength. Lowers the [ICON_STRENGTH] Combat Strength of nearby enemy Units. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_ACCURACY_1}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_INDIAN_WAR_ELEPHANT';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_INDIAN_WARELEPHANT} is the Indian unique unit, replacing the {TXT_KEY_DESC_CUIRASSIER}. As an elephant unit, it strikes fear into nearby enemy units and weakens them, has vastly superior [ICON_STRENGTH] Combat Strength and [ICON_RANGE_STRENGTH] Ranged Combat Strength, but moves slower. It does not require [ICON_RES_HORSE] {TXT_KEY_RESOURCE_HORSES} to be trained, and is available with the discovery of [COLOR_CYAN]{TXT_KEY_TECH_GUNPOWDER_TITLE}[ENDCOLOR], earlier than the {TXT_KEY_DESC_CUIRASSIER}. Does not start with the {TXT_KEY_PROMOTION_SKIRMISHER_DOCTRINE} promotion.'
WHERE Tag = 'TXT_KEY_UNIT_INDIAN_WARELEPHANT_STRATEGY';

--------------------
-- Indonesia
--------------------
UPDATE Language_en_US
SET Text = 'Sumpah Palapa'
WHERE Tag = 'TXT_KEY_TRAIT_SPICE_SHORT';

UPDATE Language_en_US
SET Text = 'When you gain a City, one of 3 unique Luxuries ([ICON_RES_CLOVES]/[ICON_RES_PEPPER]/[ICON_RES_NUTMEG]) will appear nearby. +5% to Yield and [ICON_GOLDEN_AGE] Golden Age duration modifiers from [ICON_VP_MONOPOLY] Global Monopolies; +2 to Yields and [ICON_HAPPINESS_1] Happiness from [ICON_VP_MONOPOLY] Global Monopolies.'
WHERE Tag = 'TXT_KEY_TRAIT_SPICE';

UPDATE Language_en_US
SET Text = 'A [ICON_RES_CLOVES] Cloves, [ICON_RES_PEPPER] Pepper, or [ICON_RES_NUTMEG] Nutmeg Resource will appear near or under this City when constructed.[NEWLINE][NEWLINE]+25% [ICON_GREAT_PEOPLE] Great People generation in this City. +15% [ICON_CULTURE] Culture and [ICON_PEACE] Faith in the City during "We Love the King Day."[NEWLINE][NEWLINE]1 Specialist in this City no longer produces [ICON_HAPPINESS_3] Unhappiness from [ICON_URBANIZATION] Urbanization.[NEWLINE][NEWLINE]Nearby Oases: +2 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_CITRUS] Citrus: +1 [ICON_FOOD] Food, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COCOA] Cocoa: +1 [ICON_FOOD] Food, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_CLOVES] Cloves: +1 [ICON_PEACE] Faith, +1 [ICON_CULTURE] Culture.[NEWLINE]Nearby [ICON_RES_PEPPER] Pepper: +1 [ICON_PEACE] Faith, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_NUTMEG] Nutmeg: +1 [ICON_CULTURE] Culture, +1 [ICON_PRODUCTION] Production.'
WHERE Tag = 'TXT_KEY_BUILDING_CANDI_HELP';

UPDATE Language_en_US
SET Text = 'Unique Indonesian replacement for the {TXT_KEY_BUILDING_GARDEN}. In addition to the {TXT_KEY_BUILDING_GARDEN}''s bonuses, the {TXT_KEY_BUILDING_CANDI_DESC} spawns a random unique luxury; provides [ICON_CULTURE] Culture and [ICON_PEACE] Faith; and gains an additional boost to these yields during "We Love the King Day."'
WHERE Tag = 'TXT_KEY_BUILDING_CANDI_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Indonesian {TXT_KEY_UNIT_SWORDSMAN} that has a [COLOR:30:200:255:255]{TXT_KEY_PROMOTION_MYSTIC_BLADE}[ENDCOLOR] whose abilities will be discovered the first time it is used in combat.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_INDONESIAN_KRIS_SWORDSMAN';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_INDONESIAN_KRIS_SWORDSMAN} is the Indonesian unique unit, replacing the {TXT_KEY_UNIT_SWORDSMAN}. It randomly unlocks one of the below unique promotions after its first combat:[NEWLINE][NEWLINE][COLOR:30:200:255:255]{TXT_KEY_PROMOTION_INVULNERABIILITY}[ENDCOLOR][NEWLINE]{TXT_KEY_PROMOTION_INVULNERABIILITY_HELP}[NEWLINE][NEWLINE][COLOR:30:200:255:255]{TXT_KEY_PROMOTION_SNEAK_ATTACK}[ENDCOLOR][NEWLINE]{TXT_KEY_PROMOTION_SNEAK_ATTACK_HELP}[NEWLINE][NEWLINE][COLOR:30:200:255:255]{TXT_KEY_PROMOTION_ENEMY_BLADE}[ENDCOLOR][NEWLINE]{TXT_KEY_PROMOTION_ENEMY_BLADE_HELP}[NEWLINE][NEWLINE][COLOR:30:200:255:255]{TXT_KEY_PROMOTION_AMBITION}[ENDCOLOR][NEWLINE]{TXT_KEY_PROMOTION_AMBITION_HELP}[NEWLINE][NEWLINE][COLOR:30:200:255:255]{TXT_KEY_PROMOTION_EVIL_SPIRITS}[ENDCOLOR][NEWLINE]{TXT_KEY_PROMOTION_EVIL_SPIRITS_HELP}[NEWLINE][NEWLINE][COLOR:30:200:255:255]{TXT_KEY_PROMOTION_RESTLESSNESS}[ENDCOLOR][NEWLINE]{TXT_KEY_PROMOTION_RESTLESSNESS_HELP}'
WHERE Tag = 'TXT_KEY_UNIT_INDONESIAN_KRIS_SWORDSMAN_STRATEGY';

--------------------
-- Iroquois
--------------------
UPDATE Language_en_US
SET Text = 'Units move through Forest and Jungle as if they were roads, and these tiles establish [ICON_CONNECTED] City Connections. Military Land Units start with the [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_WOODSMAN}[ENDCOLOR] Promotion.'
WHERE Tag = 'TXT_KEY_TRAIT_IGNORE_TERRAIN_IN_FOREST';

UPDATE Language_en_US
SET Text = '+1 [ICON_FOOD] Food and [ICON_PRODUCTION] Production from Forests, Jungles, Marshes, and Plantations worked by this City.'
WHERE Tag = 'TXT_KEY_BUILDING_LONGHOUSE_HELP';

UPDATE Language_en_US
SET Text = 'Unique Iroquois replacement for the {TXT_KEY_BUILDING_HERBALIST}. Instead of providing only [ICON_FOOD] Food, {TXT_KEY_BUILDING_LONGHOUSE_DESC} adds both [ICON_FOOD] Food and [ICON_PRODUCTION] Production to nearby Forests, Jungles, Marshes, and Plantations.'
WHERE Tag = 'TXT_KEY_BUILDING_LONGHOUSE_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Iroquois {TXT_KEY_UNIT_SWORDSMAN} that excels at Forest and Jungle warfare and does not require [ICON_RES_IRON] {TXT_KEY_RESOURCE_IRON}. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_WOODSMAN}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MOHAWK_WARRIOR';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_IROQUOIAN_MOHAWKWARRIOR} is the Iroquois unique unit, replacing the {TXT_KEY_UNIT_SWORDSMAN}. It receives a [ICON_MOVES] Movement and [ICON_STRENGTH] Combat bonus when fighting in Forests and Jungles. Does not require [ICON_RES_IRON] {TXT_KEY_RESOURCE_IRON} to be trained.'
WHERE Tag = 'TXT_KEY_UNIT_IROQUOIAN_MOHAWKWARRIOR_STRATEGY';

--------------------
-- Japan
--------------------
UPDATE Language_en_US
SET Text = 'Shogunate'
WHERE Tag = 'TXT_KEY_TRAIT_FIGHT_WELL_DAMAGED_SHORT';

UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture and [ICON_PEACE] Faith from Defense Buildings. When a [ICON_GREAT_ADMIRAL] Great Admiral or [ICON_GREAT_GENERAL] Great General is born, gain 50% progress toward a [ICON_GREAT_WORK] Great Artist, Writer, and Musician in your [ICON_CAPITAL] Capital.'
WHERE Tag = 'TXT_KEY_TRAIT_FIGHT_WELL_DAMAGED';

UPDATE Language_en_US
SET Text = 'Unique Japanese {TXT_KEY_UNIT_LONGSWORDSMAN} that gains 50% more experience and produces 100% more [ICON_GREAT_GENERAL] Great General Points from combat than normal.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SAMURAI';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_JAPANESE_SAMURAI} is the Japanese unique unit, replacing the {TXT_KEY_UNIT_LONGSWORDSMAN}. It gains more experience and generates [ICON_GREAT_GENERAL] Great Generals faster from combat.'
WHERE Tag = 'TXT_KEY_UNIT_JAPANESE_SAMURAI_STRATEGY';

--------------------
-- Korea
--------------------
UPDATE Language_en_US
SET Text = '+1 [ICON_RESEARCH] Science from Specialists, increasing by +1 in Medieval, Industrial, and Atomic Eras. +20% [ICON_RESEARCH] Science during [ICON_GOLDEN_AGE] Golden Ages, and +50 [ICON_GOLDEN_AGE] Golden Age Points when a [ICON_GREAT_PEOPLE] Great Person is born, scaling with Era.'
WHERE Tag = 'TXT_KEY_TRAIT_SCHOLARS_JADE_HALL';

UPDATE Language_en_US
SET Text = 'Unique Korean {TXT_KEY_UNIT_TREBUCHET} that is stronger against land units, but weaker against cities. Has normal sight range and starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_LOGISTICS}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_CIV5_KOREA_HWACHA_HELP';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_CIV5_KOREA_HWACHA_HEADING} is the Korean unique unit, replacing the {TXT_KEY_UNIT_TREBUCHET}. It operates similar to ranged units rather than siege - it lacks most bonuses and penalties that siege units have. It can also attack twice per turn, but cannot fortify or benefit from terrain defense, and retains the movement penalty in enemy territory that most siege units have.'
WHERE Tag = 'TXT_KEY_CIV5_KOREA_HWACHA_STRATEGY';

--------------------
-- Maya
--------------------
UPDATE Language_en_US
SET Text = 'After researching [COLOR_CYAN]Mathematics[ENDCOLOR], receive a bonus [ICON_GREAT_PEOPLE] Great Person at the end of every Maya Long Count cycle (every 394 years). Each bonus [ICON_GREAT_PEOPLE] Great Person can only be chosen once.'
WHERE Tag = 'TXT_KEY_TRAIT_LONG_COUNT';

UPDATE Language_en_US
SET Text = 'Unique Mayan {TXT_KEY_UNIT_COMPOSITE_BOWMAN} that can attack over obstacles.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MAYAN_ATLATLIST';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_MAYAN_ATLATLIST} is the Mayan unique unit, replacing the {TXT_KEY_UNIT_COMPOSITE_BOWMAN}. It is the only ranged unit that can shoot over obstacles in the early eras. It is also cheaper and is available with the discovery of [COLOR_CYAN]{TXT_KEY_TECH_MATHEMATICS_TITLE}[ENDCOLOR], earlier than the {TXT_KEY_UNIT_COMPOSITE_BOWMAN}.[NEWLINE][NEWLINE]The {TXT_KEY_UNIT_MAYAN_ATLATLIST} allows you to set up a stronger defense line in rough terrain against slower units.'
WHERE Tag = 'TXT_KEY_UNIT_MAYAN_ATLATLIST_STRATEGY';

--------------------
-- Mongols
--------------------
UPDATE Language_en_US
SET Text = 'Skirmisher Units have an extra Attack. Gain All Yields equal to 20% of the [ICON_GOLD] Gold from bullying [ICON_CITY_STATE] City-States.'
WHERE Tag = 'TXT_KEY_TRAIT_TERROR';

UPDATE Language_en_US
SET Text = 'Unique Mongolian {TXT_KEY_UNIT_GREAT_GENERAL} that specializes in mobile leadership. The same {TXT_KEY_PROMOTION_GREAT_GENERAL} bonuses apply, but the {TXT_KEY_UNIT_MONGOL_KHAN} moves much faster, starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_MEDIC}[ENDCOLOR] and [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_MEDIC_II}[ENDCOLOR], and deals 10 damage to enemy Units if they end their turn next to it.[NEWLINE][NEWLINE]Can be expended to construct an [COLOR_POSITIVE_TEXT]{TXT_KEY_IMPROVEMENT_MONGOLIA_ORDO}[ENDCOLOR], the unique Mongolian {TXT_KEY_IMPROVEMENT_CITADEL}.'
WHERE Tag = 'TXT_KEY_CIV5_MONGOLIA_KHAN_HELP';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_MONGOL_KHAN} is the Mongolian unique unit, replacing the {TXT_KEY_UNIT_GREAT_GENERAL}. It moves much faster, heals stacked and adjacent units for additional HP per turn, and damages adjacent enemies if they end their turn next to it. It can also be expended to construct an [COLOR_POSITIVE_TEXT]{TXT_KEY_IMPROVEMENT_MONGOLIA_ORDO}[ENDCOLOR] and claim all adjacent tiles.'
WHERE Tag = 'TXT_KEY_CIV5_MONGOLIA_KHAN_STRATEGY';

--------------------
-- Morocco
--------------------
UPDATE Language_en_US
SET Text = '+1 to All Yields in [ICON_CAPITAL] Capital per unique [ICON_INTERNATIONAL_TRADE] Trade Route partner, scaling with Era. Distance does not reduce [ICON_INTERNATIONAL_TRADE] Trade Route Yields to or from Moroccan Cities. Can plunder [ICON_INTERNATIONAL_TRADE] Trade Routes between foreign Cities without declaring war.'
WHERE Tag = 'TXT_KEY_TRAIT_GATEWAY_AFRICA';

UPDATE Language_en_US
SET Text = 'Can only be built adjacent to a City. +2 [ICON_GOLD] Gold to adjacent Fishing Boats. +1 [ICON_CULTURE] Culture to adjacent Coast tiles. +30% [ICON_STRENGTH] Defensive Strength for any Unit stationed in this tile. Any enemy unit which ends its turn next to this tile takes 5 damage (damage does not stack with other improvements). Can be built on Resources, and connects any Luxury or Strategic Resource on this tile to your trade network.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_KASBAH_HELP';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CIV5_IMPROVEMENTS_KASBAH_HELP}[NEWLINE][NEWLINE]A Kasbah is a type of medina (a walled quarter in a City) found in the countryside, usually a small settlement on a hilltop or hillside. Originally the home for a tribal chieftain or important Islamic imam, the Kasbah is characterized by high-walled, windowless houses and narrow, winding streets. It is usually dominated by a single fortified tower. Kasbahs were common along the North African coast and Middle East until the early 1900s. Building a Kasbah was a mark of wealth, influence, and power for Moroccan and Algerian families of Arab descent.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_KASBAH_TEXT';

UPDATE Language_en_US
SET Text = 'This will connect any Luxury or Strategic Resources on this tile to the trade network. +2 [ICON_GOLD] Gold to adjacent Fishing Boats. +1 [ICON_CULTURE] Culture to adjacent Coast tiles. +30% [ICON_STRENGTH] Defensive Strength for any Unit stationed in this tile. Any enemy unit which ends its turn next to the Kasbah takes 5 damage. Can only be built adjacent to a City.'
WHERE Tag = 'TXT_KEY_BUILD_KASBAH_HELP';

UPDATE Language_en_US
SET Text = 'It will increase [ICON_FOOD] Food, [ICON_PRODUCTION] Production, [ICON_GOLD] Gold, and [ICON_CULTURE] Culture. Provides additional [ICON_GOLD] Gold to adjacent Fishing Boats, and additional [ICON_CULTURE] Culture to adjacent Coast tiles.[NEWLINE]Provides a defensive bonus for own units on the tile, and lightly damages adjacent enemy units.'
WHERE Tag = 'TXT_KEY_BUILD_KASBAH_REC';

UPDATE Language_en_US
SET Text = 'Unique Moroccan {TXT_KEY_UNIT_CAVALRY} that specializes in patrolling and protecting friendly lands. Receives a [ICON_STRENGTH] Combat Strength bonus when fighting in friendly territory and ignores terrain movement cost.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BERBER_CAVALRY';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_BERBER_CAVALRY} is the Moroccan unique unit, replacing the {TXT_KEY_UNIT_CAVALRY}. It possesses a combat bonus when fighting in friendly land, and can move through difficult terrain without penalty.[NEWLINE][NEWLINE]Combined with the Moroccan Unique Ability, you can send the {TXT_KEY_UNIT_BERBER_CAVALRY} out to plunder foreign [ICON_INTERNATIONAL_TRADE] Trade Routes easily. Just make sure you aren''t seen!'
WHERE Tag = 'TXT_KEY_UNIT_BERBER_CAVALRY_STRATEGY';

--------------------
-- Netherlands
--------------------
UPDATE Language_en_US
SET Text = '+3 [ICON_GOLD] Gold and [ICON_CULTURE] Culture for each unique Luxury Resource you import or export from/to other Civilizations and [ICON_CITY_STATE] City-States, scaling with Era. Can import duplicate Luxuries from Civilizations which count toward [ICON_VP_MONOPOLY] Monopolies.'
WHERE Tag = 'TXT_KEY_TRAIT_LUXURY_RETENTION';

UPDATE Language_en_US
SET Text = 'Unique Dutch {TXT_KEY_UNIT_PRIVATEER} that specializes in attacking coastal cities and capturing enemy ships. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_COASTAL_TERROR}[ENDCOLOR] and [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_SUPPLY}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SEA_BEGGAR';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_SEA_BEGGAR} is the Dutch unique unit, replacing the {TXT_KEY_UNIT_PRIVATEER}. It has immense [ICON_STRENGTH] strength against cities, can heal anywhere, and may capture defeated enemy ships.[NEWLINE][NEWLINE]Make use of its strong promotions to take down rival navies and cities, and grow your navy with prize ships!'
WHERE Tag = 'TXT_KEY_UNIT_SEA_BEGGAR_STRATEGY';

UPDATE Language_en_US
SET Text = 'Can only be built on Marsh tiles or on featureless Lake/Coast tiles adjacent to at least 3 land tiles. Polders built on water can be traversed by land units without embarkation, as if they are land tiles.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_POLDER_HELP';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CIV5_IMPROVEMENTS_POLDER_HELP}[NEWLINE][NEWLINE]A polder is a low-lying tract enclosed by dikes with the water drained. In general, polder is land reclaimed from a lake or seabed, from flood plains, or from marshes. In time, the drained land subsides so that all polder is eventually below the surrounding water level. Because of this, water seeps into the drained area and must be pumped out or otherwise drained. The dikes are usually made of readily available materials, earth or sand; in modern times these barriers might be covered or completely composed of concrete. The drained land is extremely fertile and makes excellent pasturage or cropland.[NEWLINE][NEWLINE]The first polders of reclaimed land were constructed in the 11th Century AD, although building embankments as barriers to water date back to the Romans. The Dutch have a long history of reclaiming marshes and fenland, and even the seabed, and possess half of the polder acreage in Europe. Although there are polders in other countries of Europe, and examples can be found in Asia and North America, Holland has some 3000 polders accounting for about 27 percent of the country''s land. Amsterdam itself is built largely upon polder. As the Dutch homily states, "God made the world, but the Dutch made Holland."'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_POLDER_TEXT';

--------------------
-- Ottomans
--------------------
UPDATE Language_en_US
SET Text = 'Kanuni'
WHERE Tag = 'TXT_KEY_TRAIT_CONVERTS_SEA_BARBARIANS_SHORT';

UPDATE Language_en_US
SET Text = 'Completing a [ICON_INTERNATIONAL_TRADE] Trade Route grants +150 [ICON_FOOD] Food and [ICON_RESEARCH] Science to the origin City if International, or [ICON_GOLD] Gold and [ICON_CULTURE] Culture if Internal. Bonuses scale with Era.'
WHERE Tag = 'TXT_KEY_TRAIT_CONVERTS_SEA_BARBARIANS';

UPDATE Language_en_US
SET Text = 'Unique Ottoman {TXT_KEY_UNIT_MUSKETMAN} that gains a [ICON_STRENGTH] Combat Strength bonus when attacking. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_MARCH}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_JANISSARY';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_OTTOMAN_JANISSARY} is the Ottoman unique unit, replacing the {TXT_KEY_UNIT_MUSKETMAN}. It has a [ICON_STRENGTH] Combat Strength bonus when attacking, and may heal even after performing an action. It is available with the discovery of [COLOR_CYAN]{TXT_KEY_TECH_GUNPOWDER_TITLE}[ENDCOLOR], earlier than the {TXT_KEY_UNIT_MUSKETMAN}.'
WHERE Tag = 'TXT_KEY_UNIT_OTTOMAN_JANISSARY_STRATEGY';

--------------------
-- Persia
--------------------
UPDATE Language_en_US
SET Text = '[ICON_GOLDEN_AGE] Golden Ages last 50% longer, and 15% of your gross [ICON_GOLD] Gold income is converted into [ICON_GOLDEN_AGE] Golden Age Points every turn. During a [ICON_GOLDEN_AGE] Golden Age, Units receive +1 [ICON_MOVES] Movement and +15% [ICON_STRENGTH] Combat Strength.'
WHERE Tag = 'TXT_KEY_TRAIT_ENHANCED_GOLDEN_AGES';

UPDATE Language_en_US
SET Text = '+1 [ICON_GOLD] Gold for every 3 [ICON_CITIZEN] Citizens in the City. +1 [ICON_GOLD] Gold from [ICON_VP_SCIENTIST] Scientist, [ICON_VP_ENGINEER] Engineer, and [ICON_VP_MERCHANT] Merchant Specialists in the City.[NEWLINE][NEWLINE]Eliminates extra [ICON_HAPPINESS_4] Unhappiness from an [ICON_OCCUPIED] Occupied City (if it is [ICON_OCCUPIED] Occupied).'
WHERE Tag = 'TXT_KEY_BUILDING_SATRAPS_COURT_HELP';

UPDATE Language_en_US
SET Text = 'Unique Persian replacement for the {TXT_KEY_BUILDING_COURTHOUSE}. In addition to the {TXT_KEY_BUILDING_COURTHOUSE}''s bonuses, the {TXT_KEY_BUILDING_SATRAPS_COURT_DESC} generates [ICON_GOLD] Gold from population and specialists, and has a [ICON_VP_MERCHANT] Merchant slot. It can be built in any city instead of only occupied ones, and does not cost [ICON_GOLD] Gold maintenance.'
WHERE Tag = 'TXT_KEY_BUILDING_SATRAPS_COURT_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Persian {TXT_KEY_UNIT_SPEARMAN} that heals faster and has a defensive bonus.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_IMMORTAL';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_PERSIAN_IMMORTAL} is the Persian unique unit, replacing the {TXT_KEY_UNIT_SPEARMAN}. It has a [ICON_STRENGTH] Combat Strength bonus when defending, and can heal faster anywhere.'
WHERE Tag = 'TXT_KEY_UNIT_PERSIAN_IMMORTAL_STRATEGY';

--------------------
-- Poland
--------------------
UPDATE Language_en_US
SET Text = 'Gain 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Social Policy in the Classical Era, and in every other Era after. Gain 2 additional [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Tenets when you adopt an Ideology for the first time.'
WHERE Tag = 'TXT_KEY_TRAIT_SOLIDARITY';

UPDATE Language_en_US
SET Text = 'Provides 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_RES_HORSE] Horse. +50% [ICON_PRODUCTION] Production and +15 XP when building Mounted Melee Units. +10% [ICON_SILVER_FIST] Military Supply from Population. Internal [ICON_INTERNATIONAL_TRADE] Trade Routes from this City generate +2 [ICON_PRODUCTION] Production.[NEWLINE][NEWLINE]Nearby [ICON_RES_HORSE] Horses: +3 [ICON_PRODUCTION] Production, +3 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_SHEEP] Sheep: +3 [ICON_PRODUCTION] Production, +3 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COW] Cattle: +3 [ICON_PRODUCTION] Production, +3 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_DUCAL_STABLE_HELP';

UPDATE Language_en_US
SET Text = 'Unique Polish replacement for the {TXT_KEY_BUILDING_STABLE}. In addition to the {TXT_KEY_BUILDING_STABLE}''s bonuses, the {TXT_KEY_BUILDING_DUCAL_STABLE_DESC} provides a better [ICON_PRODUCTION] Production bonus and bonus XP for mounted melee units, and a greater boost on pasture resources. It also grants a free [ICON_RES_HORSE] Horse instead of requiring a nearby improved pasture resource to be built.'
WHERE Tag = 'TXT_KEY_BUILDING_DUCAL_STABLE_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Polish {TXT_KEY_UNIT_LANCER} that is much faster and specializes in disrupting formations. Gains a [ICON_STRENGTH] Combat Strength bonus when attacking Units that cannot retreat, and forces its target to retreat (if possible) if it has more [ICON_STRENGTH] Combat Strength than its target while attacking. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_FORMATION_1}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_WINGED_HUSSAR';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_POLISH_WINGED_HUSSAR} is the Polish unique unit, replacing the {TXT_KEY_UNIT_LANCER}. It is faster and stronger against mounted units. It can force weaker targets to retreat and strike harder if they cannot.[NEWLINE][NEWLINE]Use this ability to push the enemy front line away and expose their weaker units!'
WHERE Tag = 'TXT_KEY_UNIT_POLISH_WINGED_HUSSAR_STRATEGY';

--------------------
-- Polynesia
--------------------
UPDATE Language_en_US
SET Text = '+2 [ICON_VP_VISION] Sight when Embarked, can always Embark and move over Oceans. +2 [ICON_PRODUCTION] Production from Fishing Boats and Atolls. Melee and Gunpowder Units can construct Fishing Boats. No [ICON_HAPPINESS_3] Unhappiness from Isolation.'
WHERE Tag = 'TXT_KEY_TRAIT_WAYFINDING';

UPDATE Language_en_US
SET Text = 'Unique Polynesian {TXT_KEY_UNIT_SCOUT} that weakens adjacent enemy Units. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_AMPHIBIOUS}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_CIV5_POLYNESIAN_MAORI_WARRIOR_HELP';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_POLYNESIAN_MAORI_WARRIOR} is the Polynesian unique unit, replacing the {TXT_KEY_UNIT_SCOUT}. It is more powerful, and can attack across rivers or from the sea with no penalty. It also has a unique debuffing aura that decreases the [ICON_STRENGTH] Combat Strength of adjacent enemies.'
WHERE Tag = 'TXT_KEY_CIV5_POLYNESIAN_MAORI_WARRIOR_STRATEGY';

UPDATE Language_en_US
SET Text = 'Can only be built on coastal tiles. +1 [ICON_CULTURE] for each adjacent Moai and City. Can be built on Resources but does not connect them.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_MOAI_HELP';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CIV5_IMPROVEMENTS_MOAI_HELP}[NEWLINE][NEWLINE]Perhaps better known as the "Easter Island Statues", the Moai are large, humanoid figures carved mostly from volcanic rocks like tuff. 887 of these statues dot Easter Island, and are believed to have been created sometime between 1250 and 1500 AD. Almost half still remain at the original quarry site, but the rest were transported and erected around the island''s coastline. Easily recognizable by their large heads and elongated features, it is believed that they were individual depictions of deceased ancestors and powerful chiefs.[NEWLINE][NEWLINE]While most scholars agree on why and how the statues were created, the method of their transportation still remains a mystery. Weighing from 9 to 86 tons a piece, they would have required an amazing feat of engineering to move from quarry to final resting place.[NEWLINE][NEWLINE]In 1994, the Moai statues were given official protection on the UNESCO World Heritage site list.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_MOAI_TEXT';

UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] for each adjacent Moai and City. Can only be built on coastal tiles. Can be built on Resources but does not connect them.'
WHERE Tag = 'TXT_KEY_BUILD_MOAI_HELP';

UPDATE Language_en_US
SET Text = 'It will increase [ICON_CULTURE] Culture and [ICON_CULTURE_LOCAL] Border Growth. Removes any feature on the tile.[NEWLINE]Provides additional [ICON_CULTURE] Culture if adjacent to cities or other Moai.[NEWLINE]It will not [COLOR_NEGATIVE_TEXT]connect the resource[ENDCOLOR] on this tile (if any).'
WHERE Tag = 'TXT_KEY_BUILD_MOAI_REC';

--------------------
-- Portugal
--------------------
UPDATE Language_en_US
SET Text = 'For each active Trade Route, receive +4 [ICON_GOLD] Gold, [ICON_RESEARCH] Science, and [ICON_GREAT_ADMIRAL] Great Admiral ([ICON_CARGO_SHIP]) or [ICON_GREAT_GENERAL] Great General ([ICON_CARAVAN]) Points, scaling with Era.'
WHERE Tag = 'TXT_KEY_TRAIT_EXTRA_TRADE';

UPDATE Language_en_US
SET Text = 'Can be constructed by Workers in owned land to provide yields or by Portuguese-owned Naus (via [COLOR_POSITIVE_TEXT]{TXT_KEY_MISSION_SELL_EXOTIC_GOODS}[ENDCOLOR]) in City-State land.[NEWLINE][NEWLINE]In owned land:[NEWLINE]Can only be built on coastal tiles, and cannot be adjacent to another Feitoria.[NEWLINE][NEWLINE]In City-State land:[NEWLINE]Provides an untradeable copy of each Luxury Resource type owned by the City-State. [ICON_INTERNATIONAL_TRADE] Trade Routes to this City-State generate bonus [ICON_FOOD] Food and [ICON_PRODUCTION] Production based on [ICON_GOLD] Gold generated from the [ICON_INTERNATIONAL_TRADE] Trade Route and relationship with the City-State (i.e. Neutral/Friend/Ally).[NEWLINE][NEWLINE]In addition to these bonuses, it provides vision within a 2-tile radius and a +25% [ICON_STRENGTH] Defensive Strength bonus for any Unit stationed in this tile. Contains a [COLOR_POSITIVE_TEXT]Canal[ENDCOLOR], allowing friendly [COLOR_POSITIVE_TEXT]Sea[ENDCOLOR] Units to traverse through this tile.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_FEITORIA_HELP';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CIV5_IMPROVEMENTS_FEITORIA_HELP}[NEWLINE][NEWLINE]The Feitoria - Portuguese for "factory" - were trading posts established in foreign territories during the Middle Ages. A feitoria served simultaneously as marketplace, warehouse, settlement, and staging point for exploration. Often established with a grant from the Portuguese crown, these private enterprises repaid their debt through the buying and selling of goods on behalf of the crown and through collecting taxes on trade that passed through their gates. During the 15th and 16th centuries, a chain of over 50 feitoria were built along the coasts of west and east Africa, India, Malaya, China and Japan. The feitoria allowed Portugal to dominate the Atlantic and Indian Ocean trade routes for three centuries.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_FEITORIA_TEXT';

UPDATE Language_en_US
SET Text = '+25% [ICON_STRENGTH] Defensive Strength for any Unit stationed in this tile. Provides vision within a 2-tile radius. Contains a [COLOR_POSITIVE_TEXT]Canal[ENDCOLOR], allowing friendly [COLOR_POSITIVE_TEXT]Sea[ENDCOLOR] Units to traverse through this tile.[NEWLINE][NEWLINE]Can only be built on coastal tiles, and cannot be adjacent to another Feitoria.'
WHERE Tag = 'TXT_KEY_BUILD_FEITORIA_HELP';

UPDATE Language_en_US
SET Text = 'It will provide [ICON_PRODUCTION] Production and [ICON_GOLD] Gold. Removes any feature on the tile.[NEWLINE]Provides vision within a 2-tile radius, and a defensive bonus for own units on the tile. Allows friendly [COLOR_POSITIVE_TEXT]Sea[ENDCOLOR] Units to traverse through this tile.'
WHERE Tag = 'TXT_KEY_BUILD_FEITORIA_REC';

UPDATE Language_en_US
SET Text = 'Unique Portuguese {TXT_KEY_UNIT_CARAVEL} that excels at sea exploration and trade. Has 2 Exotic Cargo that it can sell when adjacent to a foreign tile, earning up to 400 [ICON_GOLD] Gold and 30 XP scaling with the distance from your [ICON_CAPITAL] Capital. It can also withdraw from melee combat once per turn.[NEWLINE][NEWLINE]If an Exotic Cargo is sold adjacent to City-State territory and this {TXT_KEY_UNIT_PORTUGUESE_NAU} belongs to Portugal, a {TXT_KEY_IMPROVEMENT_FEITORIA} is [COLOR_POSITIVE_TEXT]automatically created[ENDCOLOR] on one of the coastal tiles owned by the City-State.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_PORTUGUESE_NAU';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_PORTUGUESE_NAU} is the Portuguese unique unit, replacing the {TXT_KEY_UNIT_CARAVEL}. It can see further and can retreat from melee combat once per turn. It can sell up to 2 exotic cargo to foreign territory, gaining [ICON_GOLD] Gold and XP depending on distance from your [ICON_CAPITAL] Capital. If sold to a City-State as Portugal, a {TXT_KEY_IMPROVEMENT_FEITORIA} is constructed on one of the valid tiles in its territory (if available), gaining a copy of all luxury resources connected by it and boosting your [ICON_INTERNATIONAL_TRADE] Trade Routes targeting it.[NEWLINE][NEWLINE]Build a bunch of these as soon as possible and send them to City-States to reap the benefits for the rest of the game, while assembling a formidable navy at the same time!'
WHERE Tag = 'TXT_KEY_UNIT_PORTUGUESE_NAU_STRATEGY';

--------------------
-- Rome
--------------------
UPDATE Language_en_US
SET Text = '+30% [ICON_STRENGTH] Combat Strength against [ICON_CITY_STATE] City-States. Conquered City-States provide Yields, [ICON_HAPPINESS_1] Happiness and Unit Gifts as if they were [COLOR_POSITIVE_TEXT]Allies[ENDCOLOR]. +15% [ICON_PRODUCTION] Production towards Buildings present in [ICON_CAPITAL] Capital.'
WHERE Tag = 'TXT_KEY_TRAIT_CAPITAL_BUILDINGS_CHEAPER';

UPDATE Language_en_US
SET Text = 'Unique Roman {TXT_KEY_UNIT_SWORDSMAN} that can construct [COLOR_POSITIVE_TEXT]{TXT_KEY_CIV5_IMPROVEMENTS_FORT}[ENDCOLOR] and [COLOR_POSITIVE_TEXT]{TXT_KEY_CIV5_IMPROVEMENTS_ROAD}[ENDCOLOR]. Deals 10 damage to adjacent enemy Units every turn while fortified. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_COVER_1}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ROMAN_LEGION';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_ROMAN_LEGION} is the Roman unique unit, replacing the {TXT_KEY_UNIT_SWORDSMAN}. It can deal damage to adjacent enemies if fortified (not moving nor attacking) on turn end. It can also construct roads and forts slightly faster than Workers.'
WHERE Tag = 'TXT_KEY_UNIT_ROMAN_LEGION_STRATEGY';

--------------------
-- Russia
--------------------
UPDATE Language_en_US
SET Text = 'All Strategic Resource deposits provide double their normal quantity. Receive 20 [ICON_RESEARCH] Science when your borders expand, scaling with Era. [ICON_CULTURE] Culture cost of acquiring new tiles reduced by 33%.'
WHERE Tag = 'TXT_KEY_TRAIT_STRATEGIC_RICHES';

UPDATE Language_en_US
SET Text = 'Ostrog'
WHERE Tag = 'TXT_KEY_BUILDING_KREPOST_DESC';

UPDATE Language_en_US
SET Text = 'Damage from all sources against this City is reduced by 2. Allows City to [ICON_RANGE_STRENGTH] Strike over obstacles. +10% [ICON_SILVER_FIST] Military Supply from Population.[NEWLINE][NEWLINE]Enemy Land Units expend all [ICON_MOVES] Movement when entering land territory owned by this City.[NEWLINE][NEWLINE]+1 [ICON_PRODUCTION] Production and [ICON_GOLD] Gold to Camps, Mines, Lumber Mills, and Strategic Resources worked by this City.[NEWLINE][NEWLINE]Garrisoned Units receive an additional 5 Health when healing in this City.[NEWLINE][NEWLINE][ICON_CITY_STATE] Empire Size Modifier is reduced by 5% in this City.'
WHERE Tag = 'TXT_KEY_BUILDING_KREPOST_HELP';

UPDATE Language_en_US
SET Text = 'Unique Russian replacement for the {TXT_KEY_BUILDING_FORTRESS}. In addition to the {TXT_KEY_BUILDING_FORTRESS}''s bonuses, {TXT_KEY_BUILDING_KREPOST_DESC} provides more [ICON_STRENGTH] City Strength and makes enemies expend all [ICON_MOVES] movement when entering land tiles owned by the city, like a localized [ICON_WONDER] Great Wall. It also boosts camps, mines, lumber mills, and strategic resources.'
WHERE Tag = 'TXT_KEY_BUILDINGS_KREPOST_STRATEGY';

UPDATE Language_en_US
SET Text = 'Ostrog is a Russian term for a small fort, typically wooden and often non-permanently manned. Ostrogs were widely used during the age of Russian imperial expansion, particularly during the 18th and early 19th centuries. Ostrogs were encircled by 6 metres high palisade walls made from sharpened trunks. The name derives from the Russian word strogat, "to shave the wood". Ostrogs were smaller and exclusively military forts, compared to larger kremlins that were the cores of Russian cities. Ostrogs were often built in remote areas or within the fortification lines, such as the Great Abatis Line.'
WHERE Tag = 'TXT_KEY_BUILDINGS_KREPOST_PEDIA';

UPDATE Language_en_US
SET Text = 'Unique Russian {TXT_KEY_UNIT_CAVALRY} that excels at taking out weakened and cornered enemies by having a [ICON_STRENGTH] Combat Strength bonus when attacking enemies that have already taken damage, and dealing more damage to enemies that cannot retreat.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_COSSACK';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_RUSSIAN_COSSACK} is the Russian unique unit, replacing the {TXT_KEY_UNIT_CAVALRY}. It is significantly stronger, and has a [ICON_STRENGTH] Combat Strength bonus when attacking wounded units. It also does extra damage when attacking enemies that cannot retreat.'
WHERE Tag = 'TXT_KEY_UNIT_RUSSIAN_COSSACK_STRATEGY';

--------------------
-- Spain
--------------------
UPDATE Language_en_US
SET Text = 'Plus Ultra'
WHERE Tag = 'TXT_KEY_TRAIT_SEVEN_CITIES_SHORT';

UPDATE Language_en_US
SET Text = '+10 [ICON_GOLD] Gold and +4 [ICON_PEACE] Faith from gaining tiles, scaling with Era. Newly founded Cities adopt the Majority Religion of your [ICON_CAPITAL] Capital. May purchase Naval Units with [ICON_PEACE] Faith.'
WHERE Tag = 'TXT_KEY_TRAIT_SEVEN_CITIES';

UPDATE Language_en_US
SET Text = 'Unique Spanish {TXT_KEY_UNIT_EXPLORER} that specializes in conquest and settling advanced Cities overseas. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_SHOCK_4}[ENDCOLOR] and [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_COASTAL_TERROR}[ENDCOLOR].[NEWLINE][NEWLINE]Can be expended to found a City in any foreign landmass. Cities established by this Unit start with the same buildings as Cities established by a {TXT_KEY_UNIT_PIONEER} plus {TXT_KEY_BUILDING_LIGHTHOUSE} and {TXT_KEY_BUILDING_ARMORY}.'
WHERE Tag = 'TXT_KEY_CIV5_SPAIN_CONQUISTADOR_HELP';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_SPANISH_CONQUISTADOR} is the Spanish unique unit, replacing the {TXT_KEY_UNIT_EXPLORER}. It is much stronger and has large bonuses to flanking and attacking cities. Combined with its great mobility, the {TXT_KEY_UNIT_SPANISH_CONQUISTADOR} is a formidable unit, able to augment or replace your existing Medieval military.[NEWLINE][NEWLINE]It also has the ability to found new cities, but only on a foreign continent that does not contain your [ICON_CAPITAL] Capital. A city founded by {TXT_KEY_UNIT_SPANISH_CONQUISTADOR} starts with 3 [ICON_CITIZEN] Citizens, claims 3 additional tiles, and immediately receives the following selection of buildings:[NEWLINE][NEWLINE][ICON_BULLET] {TXT_KEY_BUILDING_COLOSSEUM}[NEWLINE][ICON_BULLET] {TXT_KEY_BUILDING_ARMORY}[NEWLINE][ICON_BULLET] {TXT_KEY_BUILDING_BARRACKS}[NEWLINE][ICON_BULLET] {TXT_KEY_BUILDING_GROVE}[NEWLINE][ICON_BULLET] {TXT_KEY_BUILDING_FORGE}[NEWLINE][ICON_BULLET] {TXT_KEY_BUILDING_GRANARY}[NEWLINE][ICON_BULLET] {TXT_KEY_BUILDING_HERBALIST}[NEWLINE][ICON_BULLET] {TXT_KEY_BUILDING_LIBRARY}[NEWLINE][ICON_BULLET] {TXT_KEY_BUILDING_LIGHTHOUSE}[NEWLINE][ICON_BULLET] {TXT_KEY_BUILDING_MARKET}[NEWLINE][ICON_BULLET] {TXT_KEY_BUILDING_MONUMENT_DESC}[NEWLINE][ICON_BULLET] {TXT_KEY_BUILDING_SHRINE}[NEWLINE][ICON_BULLET] {TXT_KEY_BUILDING_LODGE}[NEWLINE][ICON_BULLET] {TXT_KEY_BUILDING_WATERMILL} (if applicable)[NEWLINE][ICON_BULLET] {TXT_KEY_BUILDING_WELL} (if applicable)[NEWLINE][NEWLINE]Note: the bonuses for constructing buildings from the Progress and Industry policy branches do not apply to the buildings automatically constructed on founding!'
WHERE Tag = 'TXT_KEY_CIV5_SPAIN_CONQUISTADOR_STRATEGY';

--------------------
-- Shoshone
--------------------
UPDATE Language_en_US
SET Text = 'Claim adjacent unowned land tiles of the same type when your borders expand, and up to 4 tiles when you gain a City. Can choose rewards from Ancient Ruins. Units gain +20% [ICON_STRENGTH] Combat Strength within 2 tiles of an owned {TXT_KEY_IMPROVEMENT_ENCAMPMENT_SHOSHONE}.'
WHERE Tag = 'TXT_KEY_TRAIT_GREAT_EXPANSE';

UPDATE Language_en_US
SET Text = 'Unique Shoshone {TXT_KEY_UNIT_CAVALRY} that excels at hit-and-run attacks and pillaging. Generates [ICON_RESEARCH] Science and consumes no [ICON_MOVES] Moves when pillaging. Can move faster, and withdraw from melee combat once per turn.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SHOSHONE_COMANCHE_RIDERS';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_SHOSHONE_COMANCHE_RIDERS} is the Shoshone unique unit, replacing the {TXT_KEY_UNIT_CAVALRY}. It can move faster, pillage tiles without spending [ICON_MOVE] movement, and gain [ICON_RESEARCH] Science doing so. It can also retreat from melee combat once per turn.[NEWLINE][NEWLINE]Use this to destroy your enemies'' infrastructure while maintaining a tech lead!'
WHERE Tag = 'TXT_KEY_UNIT_SHOSHONE_COMANCHE_RIDERS_STRATEGY';

--------------------
-- Siam
--------------------
UPDATE Language_en_US
SET Text = 'Yields, Resources, [ICON_HAPPINESS_1] Happiness and Unit Gift frequency from Friendly and Allied [ICON_CITY_STATE] City-States are doubled.'
WHERE Tag = 'TXT_KEY_TRAIT_CITY_STATE_BONUSES';

UPDATE Language_en_US
SET Text = '+3 [ICON_RESEARCH] Science from Temples and Shrines in this City. +10 [ICON_INFLUENCE] Influence from Diplomatic Missions by Units originating from this City.[NEWLINE][NEWLINE]+25 [ICON_SPY] City Security, plus 1 for every two [ICON_CITIZEN] Citizens in the City.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD] and [ICON_PRODUCTION] Distress.'
WHERE Tag = 'TXT_KEY_BUILDING_WAT_HELP';

UPDATE Language_en_US
SET Text = 'Unique Siamese replacement for the {TXT_KEY_BUILDING_CONSTABLE}. In addition to the {TXT_KEY_BUILDING_CONSTABLE}''s bonuses, {TXT_KEY_BUILDING_WAT_DESC} increases [ICON_SPY] City Security even more; generates [ICON_RESEARCH] Science, [ICON_CULTURE] Culture, and [ICON_PEACE] Faith; and boosts Shrines, Temples, and diplomatic units. It also has a [ICON_VP_SCIENTIST] Scientist slot. It is available in the [COLOR_CYAN]Medieval Era[ENDCOLOR] with the discovery of [COLOR_CYAN]{TXT_KEY_TECH_THEOLOGY_TITLE}[ENDCOLOR], much earlier than the {TXT_KEY_BUILDING_CONSTABLE}.'
WHERE Tag = 'TXT_KEY_BUILDING_WAT_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Siamese {TXT_KEY_UNIT_KNIGHT} that excels at defeating Mounted Units and does not require [ICON_RES_HORSE] {TXT_KEY_RESOURCE_HORSES}. Receives an extra [ICON_STRENGTH] Combat Strength boost when stacked with a {TXT_KEY_UNIT_GREAT_GENERAL}. Trades 1 [ICON_MOVES] [COLOR_NEGATIVE_TEXT]Movement[ENDCOLOR] for the ability to lower the [ICON_STRENGTH] Combat Strength of nearby enemy Units.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SIAMESE_WARELEPHANT';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_SIAMESE_WARELEPHANT} is the Siamese unique unit, replacing the {TXT_KEY_UNIT_KNIGHT}. As an elephant unit, it strikes fear into nearby enemy units and weakens them, has superior [ICON_STRENGTH] Combat Strength, but moves slower. It has a [ICON_STRENGTH] Combat Strength bonus when fighting against fellow mounted units, and an extra bonus when stacked with a {TXT_KEY_UNIT_GREAT_GENERAL}. It does not require [ICON_RES_HORSE] {TXT_KEY_RESOURCE_HORSES} to be trained.'
WHERE Tag = 'TXT_KEY_UNIT_SIAMESE_WARELEPHANT_STRATEGY';

--------------------
-- Songhai
--------------------
UPDATE Language_en_US
SET Text = 'Triple [ICON_GOLD] Gold from destroying Encampments and plundering Cities. Land Units gain the [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_AMPHIBIOUS}[ENDCOLOR] and [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_WAR_CANOES}[ENDCOLOR] Promotions.'
WHERE Tag = 'TXT_KEY_TRAIT_AMPHIB_WARLORD';

UPDATE Language_en_US
SET Text = 'Tabya'
WHERE Tag = 'TXT_KEY_BUILDING_MUD_PYRAMID_MOSQUE_DESC';

UPDATE Language_en_US
SET Text = '+10% [ICON_PRODUCTION] Production when constructing Buildings.[NEWLINE][NEWLINE]Allows [ICON_PRODUCTION] Production to be moved from this City along trade routes inside your Civilization.[NEWLINE][NEWLINE]Nearby River: +1 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_MARBLE] Marble: +1 [ICON_PRODUCTION] Production, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_STONE] Stone: +2 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_SALT] Salt: +2 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_JADE] Jade: +1 [ICON_PRODUCTION] Production, +1 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_MUD_PYRAMID_MOSQUE_HELP';

UPDATE Language_en_US
SET Text = 'Unique Songhai replacement for the {TXT_KEY_BUILDING_STONE_WORKS}. In addition to the {TXT_KEY_BUILDING_STONE_WORKS}'' bonuses, {TXT_KEY_BUILDING_MUD_PYRAMID_MOSQUE_DESC} generates a bit of [ICON_CULTURE] Culture, boosts all river tiles, and provides a [ICON_PRODUCTION] Production bonus on building construction. It does not require a nearby improved resource to be built, which makes sure [ICON_PRODUCTION] Production internal [ICON_INTERNATIONAL_TRADE] Trade Routes are always available early.'
WHERE Tag = 'TXT_KEY_BUILDING_MUD_PYRAMID_MOSQUE_STRATEGY';

UPDATE Language_en_US
SET Text = 'Adobe, the Spanish word for mud brick, is a natural building material made from sand, clay, water and some kind of fibrous or organic material (sticks, straw or manure) and the bricks made with adobe material using molds and dried in the sun. The Great Mosque of Djenné, in central Mali, is largest mudbrick structure in the world. It, like much Sahelian architecture, is built with a mudbrick called Banco: a recipe of mud and grain husks, fermented, and either formed into bricks or applied on surfaces as a plaster like paste in broad strokes. This plaster must be reapplied annually. The facilities where these material were created were called Tabya (Cobworks), and played an essential role in West African architecture.'
WHERE Tag = 'TXT_KEY_CIV5_BUILDING_MUD_PYRAMID_MOSQUE_PEDIA';

UPDATE Language_en_US
SET Text = 'Unique Songhai {TXT_KEY_UNIT_KNIGHT} that excels at raiding cities and flanking enemies. Does not have a [ICON_STRENGTH] Combat Strength penalty and steals [ICON_GOLD] Gold when attacking Cities. Can use enemy roads and railroads.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MUSLIM_CAVALRY';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_SONGHAI_MUSLIMCAVALRY} is the Songhai unique unit, replacing the {TXT_KEY_UNIT_KNIGHT}. It has a significant flanking bonus and can use enemy roads/railroads against them. It does not suffer from the [ICON_STRENGTH] Combat Strength penalty a regular {TXT_KEY_UNIT_KNIGHT} has when attacking cities, and can steal [ICON_GOLD] Gold based on how much damage it deals to cities.'
WHERE Tag = 'TXT_KEY_UNIT_SONGHAI_MUSLIMCAVALRY_STRATEGY';

--------------------
-- Sweden
--------------------
UPDATE Language_en_US
SET Text = 'Lion of the North'
WHERE Tag = 'TXT_KEY_TRAIT_DIPLOMACY_GREAT_PEOPLE_SHORT';

UPDATE Language_en_US
SET Text = 'Melee Land Units gain +20% [ICON_STRENGTH] Combat Strength [COLOR_POSITIVE_TEXT]when attacking[ENDCOLOR], and Siege Units gain +1 [ICON_MOVES] Movement. When a [ICON_GREAT_GENERAL] Great General is born, all Military Units fully heal and earn +15 XP. +15% [ICON_GREAT_GENERAL]/[ICON_GREAT_ADMIRAL] [COLOR_POSITIVE_TEXT]Leadership[ENDCOLOR] combat bonus.'
WHERE Tag = 'TXT_KEY_TRAIT_DIPLOMACY_GREAT_PEOPLE';

UPDATE Language_en_US
SET Text = 'Unique Swedish {TXT_KEY_UNIT_RIFLEMAN} that specializes in reckless attacks. Deals 15 damage to all adjacent enemy Units on kill. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_MARCH}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SWEDISH_CAROLEAN';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_SWEDISH_CAROLEAN} is the Swedish unique unit, replacing the {TXT_KEY_UNIT_RIFLEMAN}. It deals damage to all adjacent enemies after advancing from killing a unit, and may heal even after performing an action.'
WHERE Tag = 'TXT_KEY_UNIT_SWEDISH_CAROLEAN_STRATEGY';

--------------------
-- Venice
--------------------
UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]Cannot create Settlers or Annex Cities.[ENDCOLOR] [ICON_INTERNATIONAL_TRADE] Trade Route cap doubled, target restrictions removed. [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] {TXT_KEY_UNIT_VENETIAN_MERCHANT} at [COLOR_CYAN]{TXT_KEY_TECH_HORSEBACK_RIDING_TITLE}[ENDCOLOR]. [ICON_PUPPET] Puppets have -30% Yield penalties, can use [ICON_GOLD] Gold and gain [ICON_HAPPINESS_1] Happiness like normal Cities.'
WHERE Tag = 'TXT_KEY_TRAIT_SUPER_CITY_STATE';

UPDATE Language_en_US
SET Text = 'Unique Venetian {TXT_KEY_UNIT_GREAT_MERCHANT} that can conduct better trade missions; purchase control of a [ICON_CITY_STATE] City-State and its units at no cost; and found a [COLOR_POSITIVE_TEXT]Colonia[ENDCOLOR], a [ICON_PUPPET] Puppeted City with additional territory, population, and buildings.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_VENETIAN_MERCHANT';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_VENETIAN_MERCHANT} is the Venetian unique unit, replacing the {TXT_KEY_UNIT_GREAT_MERCHANT}. It generates more [ICON_GOLD] when performing a trade mission. It can acquire a City-State outright, bringing it under Venetian control as a puppet. It can also found a Colonia, which is a city that starts puppeted with 3 [ICON_CITIZEN] Citizens, claims 3 additional tiles, and immediately receives a {TXT_KEY_BUILDING_MONUMENT_DESC} and a {TXT_KEY_BUILDING_MARKET}. The {TXT_KEY_UNIT_VENETIAN_MERCHANT} is expended when used in any of these ways.[NEWLINE][NEWLINE]Note: the bonuses for constructing buildings from the Progress and Industry policy branches do not apply to the buildings automatically constructed on founding!'
WHERE Tag = 'TXT_KEY_UNIT_VENETIAN_MERCHANT_STRATEGY';

--------------------
-- Zulu
--------------------
UPDATE Language_en_US
SET Text = '-50% [ICON_GOLD] Gold maintenance for Melee and Gunpowder Units. All Units require 25% less Experience for Promotions. Your military ignores Alliance and Protection penalties and is 50% more effective at intimidating [ICON_CITY_STATE] City-States.'
WHERE Tag = 'TXT_KEY_TRAIT_BUFFALO_HORNS';

UPDATE Language_en_US
SET Text = 'All Melee and Gunpowder Units trained in this City receive the [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_IKLWA}[ENDCOLOR] Promotion. +15 XP for all Units. +2 [ICON_WAR] Military Supply.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD] and [ICON_PRODUCTION] Distress.'
WHERE Tag = 'TXT_KEY_BUILDING_IKANDA_HELP';

UPDATE Language_en_US
SET Text = 'Unique Zulu replacement for the {TXT_KEY_BUILDING_BARRACKS}. In addition to the {TXT_KEY_BUILDING_BARRACKS}'' bonuses, the {TXT_KEY_BUILDING_IKANDA_DESC} provides more [ICON_WAR] Military Supply and grants the {TXT_KEY_PROMOTION_IKLWA} promotion to all melee and gunpowder units, which unlocks a unique promotion line that offers extra HP, extra [ICON_MOVES] movement, and a flank bonus. It is available with the discovery of [COLOR_CYAN]{TXT_KEY_TECH_BRONZE_WORKING_TITLE}[ENDCOLOR] instead of [COLOR_CYAN]{TXT_KEY_TECH_ARCHERY_TITLE}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_BUILDING_IKANDA_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Zulu {TXT_KEY_UNIT_SPANISH_TERCIO} that excels at defeating Gunpowder Units. Performs a first-strike Ranged attack ([COLOR_POSITIVE_TEXT]at 50% Combat Strength[ENDCOLOR]) before an attack, softening enemies.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ZULU_IMPI';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_UNIT_ZULU_IMPI} is the Zulu unique unit, replacing the {TXT_KEY_UNIT_SPANISH_TERCIO}. Before engaging in an melee attack, it performs a spear throw attack that can damage or kill the enemy unit. This will weaken the defender before melee combat ensues, ensuring that it takes less damage when attacking compared to its contemporary units. It also has a [ICON_STRENGTH] Combat Strength bonus when fighting against Gunpowder units, making it a deadly unit well into the Industrial Era.'
WHERE Tag = 'TXT_KEY_UNIT_ZULU_IMPI_STRATEGY';
