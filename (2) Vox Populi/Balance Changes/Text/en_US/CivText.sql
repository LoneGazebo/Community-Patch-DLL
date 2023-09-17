--------------------
-- America
--------------------
UPDATE Language_en_US
SET Text = 'Military Land Units start with [COLOR_POSITIVE_TEXT]Sentry[ENDCOLOR] Promotion. The cost of purchasing a tile is reduced by 25%, and is reset upon founding a City. Purchasing a tile grants +20 [ICON_PRODUCTION] Production, scaling with Era. Can purchase tiles owned by others.'
WHERE Tag = 'TXT_KEY_TRAIT_RIVER_EXPANSION';

UPDATE Language_en_US
SET Text = 'The Minuteman is the American unique unit, replacing the Musketman. The Minuteman can move through difficult terrain without penalty and gains [ICON_GOLDEN_AGE] Golden Age Points from destroying units.'
WHERE Tag = 'TXT_KEY_UNIT_AMERICAN_MINUTEMAN_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique American {TXT_KEY_UNIT_MUSKETMAN} that can ignore terrain movement cost and generate [ICON_GOLDEN_AGE] Golden Age Points from defeated enemies. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_BARRAGE_1}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MINUTEMAN';

UPDATE Language_en_US
SET Text = 'The B17 Bomber is unique to the Freedom Ideology. Requires the Their Finest Hour Tenet to be unlocked. It is similar to the bomber, but it is more difficult for enemy anti-aircraft and fighters to target. The B17 also receives a bonus when attacking enemy cities. Unlike the bomber, its range is 10. See the rules on Aircraft for more details.'
WHERE Tag = 'TXT_KEY_UNIT_AMERICAN_B17_STRATEGY';

UPDATE Language_en_US
SET Text = 'Air Unit that rains death from above onto Enemy Units and Cities. This Unit has a chance of evading interception, and does additional damage to Cities compared to the Bomber. [COLOR_POSITIVE_TEXT]Does not use Military Supply.[ENDCOLOR][NEWLINE][NEWLINE]Only available to Civilizations following the [COLOR_MAGENTA]Freedom[ENDCOLOR] Ideology. Requires the [COLOR_MAGENTA]Their Finest Hour[ENDCOLOR] Tenet to be unlocked.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_B17';

--------------------
-- Arabia
--------------------
UPDATE Language_en_US
SET Text = 'Completing a [ICON_CARAVAN]/[ICON_CARGO_SHIP] Trade Route originating here and targeting another Civilization, triggers a (or strengthens an existing) [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR].[NEWLINE][NEWLINE][ICON_CARAVAN] Land trade routes gain 50% range. Incoming [ICON_INTERNATIONAL_TRADE] Trade Routes generate +1 [ICON_GOLD] Gold for the City, and +1 [ICON_GOLD] Gold for the Trade Route owner.[NEWLINE][NEWLINE]Nearby [ICON_RES_SPICES] Cinnamon: +1 [ICON_FOOD] Food, +1 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_SUGAR] Sugar: +1 [ICON_FOOD] Food, +1 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_BAZAAR_HELP';

UPDATE Language_en_US
SET Text = 'The Bazaar is the Arabian unique building, replacing the Market. The Bazaar provides [ICON_GOLD] Gold and boosts Trade Route range. The Bazaar also boosts the yields of [ICON_RES_SPICES] Cinnamon and [ICON_RES_SUGAR] Sugar.'
WHERE Tag = 'TXT_KEY_BUILDING_BAZAAR_STRATEGY';

UPDATE Language_en_US
SET Text = 'When you complete a [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR], your [ICON_CAPITAL] Capital gains +1 [ICON_RESEARCH] Science and [ICON_CULTURE] Culture, and 15% towards the progress of a random [ICON_GREAT_PEOPLE] Great Person.'
WHERE Tag = 'TXT_KEY_TRAIT_LAND_TRADE_GOLD2';

UPDATE Language_en_US
SET Text = 'One Thousand and One Nights'
WHERE Tag = 'TXT_KEY_TRAIT_LAND_TRADE_GOLD2_SHORT';

UPDATE Language_en_US
SET Text = 'Unique Arabian {TXT_KEY_DESC_MOUNTED_BOWMAN} that excels at causing chaos through its splash damage and does not require [ICON_RES_HORSE] {TXT_KEY_RESOURCE_HORSES}. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_SPLASH}[ENDCOLOR] and [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_WITHDRAW_BEFORE_MELEE}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_CAMEL_ARCHER';

UPDATE Language_en_US
SET Text = 'The Camel Archer is the Arabian unique unit, replacing the Heavy Skirmisher. The Camel Archer is a powerful ranged attack unit that starts with Splash Damage I, allowing it to do damage to groups of enemies 1 hex away. It retreats from melee combat and does not require [ICON_RES_HORSE] Horses to be constructed. As a mounted unit, the Camel Archer is still vulnerable to Pikemen.'
WHERE Tag = 'TXT_KEY_UNIT_ARABIAN_CAMELARCHER_STRATEGY';

--------------------
-- Assyria
--------------------
UPDATE Language_en_US
SET Text = '+20% [ICON_RESEARCH] Science in the City during [ICON_GOLDEN_AGE] Golden Ages. +1 [ICON_RESEARCH] Science for every 2 [ICON_CITIZEN] Citizens in the City, and all Libraries produce +3 [ICON_RESEARCH] Science. Contains 3 slots for [ICON_VP_GREATWRITING] Great Works of Writing, and comes with both a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_VP_GREATWRITING] Great Work of Writing and a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Library in the City. Every [ICON_VP_GREATWRITING] Great Work of Writing you control grants +5 XP to Units created in any City (maximum of 45 XP). 1 Specialist no longer produces [ICON_HAPPINESS_3] Unhappiness from Urbanization.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_RESEARCH] Illiteracy in all Cities. +2[ICON_GREAT_PEOPLE] Great Scientist points. [NEWLINE][NEWLINE]The [ICON_PRODUCTION] Production Cost and [ICON_CITIZEN] Population Requirements increase based on the number of Cities you own.[NEWLINE][NEWLINE]+3 [ICON_PRODUCTION] Production and [ICON_RESEARCH] Science if [COLOR_POSITIVE_TEXT]Themed[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_BUILDING_ROYAL_LIBRARY_HELP';

UPDATE Language_en_US
SET Text = 'In addition to the [ICON_RESEARCH] Science output of the School of Philosophy it replaces, the Royal Library has three Great Work of Writing slots, comes with a free Great Work and Library in the City, and is available earlier. Provides a global bonus to all Libraries, and all owned cities generate extra XP for Units trained in them based on the number of Great Works of Writing in your empire (up to 45 XP). Only the Assyrians may build it.'
WHERE Tag = 'TXT_KEY_BUILDING_ROYAL_LIBRARY_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Assyrian support Unit. Nearby owned Units gain +40% [ICON_STRENGTH] Combat Strength versus Cities when adjacent to an enemy City, or +20% [ICON_STRENGTH] Combat Strength if two tiles from an enemy City. Starts with [COLOR_POSITIVE_TEXT]Medic I-II[ENDCOLOR] and [COLOR_POSITIVE_TEXT]Extra Sight[ENDCOLOR]. Cannot attack or defend itself.[NEWLINE][NEWLINE]May only have [COLOR_POSITIVE_TEXT]2[ENDCOLOR] Siege Towers active at one time.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ASSYRIAN_SIEGE_TOWER';

UPDATE Language_en_US
SET Text = 'Unique Assyrian support Unit. This unit must move close to an enemy City to be effective, but once it has, it is devastating. It [COLOR_NEGATIVE_TEXT]cannot attack or defend itself[ENDCOLOR], so protect it at all costs! You cannot have more than 2 active Siege Towers at one time.[NEWLINE][NEWLINE]If adjacent to a City, the Siege Tower confers a huge combat bonus to all nearby units also attacking the City. If the Siege Tower is two tiles away from the City, all nearby units attacking the City receive half of this bonus. Complement a Siege Tower with melee or ranged units, and make sure it reaches its target before getting killed, and cities will fall much more quickly. Does not obsolete.'
WHERE Tag = 'TXT_KEY_UNIT_ASSYRIAN_SIEGE_TOWER_STRATEGY';

UPDATE Language_en_US
SET Text = 'When next to an enemy City, other units within 2 tiles get +40% [ICON_STRENGTH] Combat Strength when attacking the City.[NEWLINE]If 2 tiles from an enemy City, other units within 2 tiles get +20% [ICON_STRENGTH] Combat Strength when attacking the City.[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]Bonuses do not stack.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_PROMOTION_SAPPER_HELP';

UPDATE Language_en_US
SET Text = 'When you conquer a City, gain either a Technology already known by the owner or, if impossible, +20 [ICON_RESEARCH] Science scaling with the City''s [ICON_CITIZEN] Population and Era. All [ICON_GREAT_WORK] Great Works produce +3 [ICON_RESEARCH] Science.'
WHERE Tag = 'TXT_KEY_TRAIT_SLAYER_OF_TIAMAT';

--------------------
-- Austria
--------------------
UPDATE Language_en_US
SET Text = '+50% rewards from [ICON_CITY_STATE] City-State Quests. May arrange [ICON_RES_MARRIAGE] Marriages with Allied City-States using [ICON_GOLD] Gold. While at peace with the City-State, Marriages increase resting [ICON_INFLUENCE] Influence by 75, scaling with Era, and boost [ICON_GREAT_PEOPLE] Great People generation in the [ICON_CAPITAL] Capital by 15%.'
WHERE Tag = 'TXT_KEY_TRAIT_ANNEX_CITY_STATE';

UPDATE Language_en_US
SET Text = 'Habsburg Diplomacy'
WHERE Tag = 'TXT_KEY_TRAIT_ANNEX_CITY_STATE_SHORT';

UPDATE Language_en_US
SET Text = '+33% [ICON_GREAT_PEOPLE] Great People generation in this City. 10% of this City''s [ICON_CULTURE] Culture is converted into [ICON_RESEARCH] Science every turn.[NEWLINE][NEWLINE]Carries over 15% of [ICON_FOOD] Food after City growth (effect stacks with Aqueduct).[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty.'
WHERE Tag = 'TXT_KEY_BUILDING_COFFEE_HOUSE_HELP';

UPDATE Language_en_US
SET Text = 'The Coffee House is a Renaissance-era building unique to Austria, replacing the Grocer. It increases the City''s [ICON_GROWTH] Growth and speed at which [ICON_GREAT_PEOPLE] Great People are generated by 33%.'
WHERE Tag = 'TXT_KEY_BUILDING_COFFEE_HOUSE_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Austrian {TXT_KEY_DESC_CUIRASSIER} that excels at scouting and flank attacking. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_LIGHTNING_WARFARE}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_AUSTRIAN_HUSSAR';

UPDATE Language_en_US
SET Text = 'Hussars are great scouts and flank attackers. They are slightly faster than Cuirassier which they replace. They start with Promotions improving their sight range and allow them to ignore Zone of Control.'
WHERE Tag = 'TXT_KEY_UNIT_AUSTRIAN_HUSSAR_STRATEGY';

--------------------
-- Aztecs
--------------------
UPDATE Language_en_US
SET Text = '+1 [ICON_FOOD] Food and [ICON_PRODUCTION] Production for every 4 [ICON_CITIZEN] Citizens in the City, and +10% [ICON_FOOD] Food during [ICON_GOLDEN_AGE] Golden Ages. Each worked Lake tile provides +2 [ICON_FOOD] Food, and River tiles produce +1 [ICON_FOOD] Food.'
WHERE Tag = 'TXT_KEY_BUILDING_FLOATING_GARDENS_HELP';

UPDATE Language_en_US
SET Text = 'The Floating Gardens is the Aztec unique building, replacing both the Well and the Water Mill (as it can be constructed anywhere). It increases the City''s [ICON_FOOD] Food and [ICON_PRODUCTION] Production output, especially during Golden Ages, and should be built if the City is near a lake or river.'
WHERE Tag = 'TXT_KEY_BUILDING_FOATING_GARDENS_STRATEGY';

UPDATE Language_en_US
SET Text = 'Killing enemy Units grants 150% of their [ICON_STRENGTH] Strength as [ICON_GOLD] Gold and [ICON_PEACE] Faith. When you complete a favorable Peace Treaty ([COLOR_POSITIVE_TEXT]War Score[ENDCOLOR] of 25+), a [ICON_GOLDEN_AGE] Golden Age begins.'
WHERE Tag = 'TXT_KEY_TRAIT_CULTURE_FROM_KILLS';

UPDATE Language_en_US
SET Text = 'Unique Aztec {TXT_KEY_UNIT_WARRIOR} that excels at Forest and Jungle warfare and recovers HP when it kills an enemy unit. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_WOODSMAN}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_JAGUAR';

UPDATE Language_en_US
SET Text = 'The Jaguar is the Aztec Unique Unit, replacing the Warrior. Create several units as early as possible and fight with the nearest neighbour to make best use of the Aztecs'' [ICON_GOLD] Gold and [ICON_PEACE] Faith on kill, preferably luring enemies into Jungles and Forests where the Jaguar can make best use of its bonuses.'
WHERE Tag = 'TXT_KEY_UNIT_AZTEC_JAGUAR_STRATEGY';

--------------------
-- Babylon
--------------------
UPDATE Language_en_US
SET Text = 'Receive a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_GREAT_SCIENTIST] Great Scientist when you discover [COLOR_CYAN]Writing[ENDCOLOR], and [ICON_GREAT_SCIENTIST] Great Scientists are earned 50% faster than normal. Investing [ICON_INVEST] Gold in Buildings reduces their [ICON_PRODUCTION] Production cost by an additional 15%.'
WHERE Tag = 'TXT_KEY_TRAIT_INGENIOUS';

UPDATE Language_en_US
SET Text = 'Unique Babylonian {TXT_KEY_UNIT_ARCHER} that is stronger and inflicts bonus damage on wounded units through its [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_ATLATL_ATTACK}[ENDCOLOR] Promotion.'
WHERE Tag = 'TXT_KEY_CIV5_BABYLON_BOWMAN_HELP';

UPDATE Language_en_US
SET Text = 'The Bowman is the Babylonian Unique Unit, replacing the Archer. This Unit is defensively stronger than the Archer, allowing placement in the front line. The Bowman''s improved combat prowess helps ameliorate any concern that it may be quickly overrun.'
WHERE Tag = 'TXT_KEY_CIV5_BABYLON_BOWMAN_STRATEGY';

UPDATE Language_en_US
SET Text = 'The Walls of Babylon increase the damage output of all the City''s ranged attacks and increase the City''s defense. Increases Military Units supplied by this City''s population by 10%. Also helps with managing the Empire Size Modifier in this City.'
WHERE Tag = 'TXT_KEY_CIV5_BABYLON_WALLS_INFO';

UPDATE Buildings
SET Help = 'TXT_KEY_BUILDING_BABYLON_WALLS_HELP'
WHERE Type = 'BUILDING_WALLS_OF_BABYLON';

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BUILDING_BABYLON_WALLS_HELP', '[ICON_SILVER_FIST] Military Units Supplied by this City''s population increased by 10%. Increases the City''s [ICON_RANGE_STRENGTH] Ranged Strike Range by 1. Great Scientists provide 5% more [ICON_RESEARCH] Science when used to discover new Technology.[NEWLINE][NEWLINE][ICON_CITY_STATE] Empire Size Modifier is reduced by 5% in this City.');

UPDATE Language_en_US
SET Text = 'The Walls of Babylon are a Babylonian Unique Building, replacing the standard city Walls. The Walls of Babylon increase City Defense by 8 and City Hit Points by 150, providing more protection than standard Walls. Increases [ICON_SILVER_FIST] Military Units supplied by this City''s population by 10%, and increases the City''s Ranged Strike range by 1. The Walls of Babylon also provide Great Scientist points and improve the Yields of all Great Scientists at discovering technology by +10%. Also helps with managing the Empire Size Modifier in this City.'
WHERE Tag = 'TXT_KEY_CIV5_BABYLON_WALLS_STRATEGY';

--------------------
-- Brazil
--------------------
UPDATE Language_en_US
SET Text = '"We Love the King Day" become Carnivals, adding -50% [ICON_HAPPINESS_3] Unhappiness from Needs. When a [ICON_GOLDEN_AGE] Golden Age begins, 30% of [ICON_GOLDEN_AGE] Golden Age Points convert into [ICON_GOLD] Gold and [ICON_TOURISM] Tourism, and Cities gain 10 turns of Carnival.'
WHERE Tag = 'TXT_KEY_TRAIT_CARNIVAL';

UPDATE Language_en_US
SET Text = 'The Pracinha can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BRAZILIAN_PRACINHA';

UPDATE Language_en_US
SET Text = 'This Atomic Era melee unit can use its Survivalism I promotion to help it survive on the front line. When the opportunity presents itself, it can defeat a weakened enemy to earn points toward starting another [ICON_GOLDEN_AGE] Golden Age.'
WHERE Tag = 'TXT_KEY_UNIT_BRAZILIAN_PRACINHA_STRATEGY';

UPDATE Language_en_US
SET Text = 'Generates 1 copy of the [ICON_RES_BRAZILWOOD] Brazilwood Resource, and keeps the Jungle or Forest intact. Cannot be constructed near Fresh Water or adjacent to another Brazilwood Camp.'
WHERE Tag = 'TXT_KEY_BUILD_BRAZILWOOD_CAMP_HELP';

UPDATE Language_en_US
SET Text = 'It will increase the amount of yields provided by this tile, while keeping the Jungle or Forest intact. Produces 1 copy of the [ICON_RES_BRAZILWOOD] Brazilwood Resource.'
WHERE Tag = 'TXT_KEY_BUILD_BRAZILWOOD_CAMP_REC';

UPDATE Language_en_US
SET Text = 'A Brazilwood Camp can only be built on Jungle or Forest tiles without fresh water access, and cannot be built adjacent to another Brazilwood Camp.  It provides additional yields, and 1 copy of the [ICON_RES_BRAZILWOOD] Brazilwood Resource. Does not remove any features on the tile.'
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
SET Text = 'Unique Byzantine {TXT_KEY_UNIT_KNIGHT} that is slower but more powerful as it can gain Defensive Bonuses and assault Cities better. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_COVER_1}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BYZANTINE_CATAPHRACT';

UPDATE Language_en_US
SET Text = 'The Cataphract is a Byzantine unique unit, replacing the Knight. Cataphracts are slower but more powerful than the Knight unit, making them one of the most dangerous cavalry units until the arrival of the Lancer. They can move after attacking. They are able to gain defensive bonuses, unlike the Knight. Also their penalty for attacking cities is not as severe.'
WHERE Tag = 'TXT_KEY_UNIT_BYZANTINE_CATAPHRACT_STRATEGY';

UPDATE Language_en_US
SET Text = 'The Dromon can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BYZANTINE_DROMON';

UPDATE Language_en_US
SET Text = 'The Dromon performs similarly to the Liburna, but is much more powerful against units. Starts with the [COLOR_POSITIVE_TEXT]Splash I[ENDCOLOR] and [COLOR_POSITIVE_TEXT]Targeting I[ENDCOLOR] Promotions.'
WHERE Tag = 'TXT_KEY_UNIT_BYZANTINE_DROMON_STRATEGY';

--------------------
-- Carthage
--------------------
UPDATE Language_en_US
SET Text = '+125 [ICON_GOLD] Gold when founding Cities, scaling with Era. Owned Coastal Cities receive a [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Lighthouse. [ICON_INTERNATIONAL_TRADE] Trade Route Resource Diversity modifier doubled.'
WHERE Tag = 'TXT_KEY_TRAIT_PHOENICIAN_HERITAGE';

UPDATE Language_en_US
SET Text = 'Unique Carthaginians {TXT_KEY_UNIT_TRIREME} that specializes in naval assaults and capturing coastal cities. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_BOARDING_PARTY_4}[ENDCOLOR] and [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_HEAVY_SHIP}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_CARTHAGINIAN_QUINQUEREME';

--------------------
-- Celts
--------------------
INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BUILDING_CEILIDH_HALL_HELP', '+150 [ICON_CULTURE] Culture when completed, and starts 15 turns of "We Love the King Day" in the City.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_CULTURE] Boredom.[NEWLINE][NEWLINE]Nearby [ICON_RES_IVORY] Ivory: +3 [ICON_CULTURE] Culture.');

UPDATE Language_en_US
SET Text = 'The Ceilidh Hall is a Medieval-era building unique to the Celts, replacing the Circus. Reduces [ICON_HAPPINESS_3] Boredom slightly and increases City [ICON_CULTURE] Culture and [ICON_PEACE] Faith. Nearby sources of Ivory gain +3 [ICON_CULTURE] Culture. Provides 1 Musician Specialist slot, and contains a slot for a Great Work of Music.'
WHERE Tag = 'TXT_KEY_BUILDING_CEILIDH_HALL_STRATEGY';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Has a unique set of Pantheon Beliefs[ENDCOLOR] that no one else can benefit from. Owned Cities with your [ICON_RELIGION_PANTHEON] Pantheon or [ICON_RELIGION] Religion neither [COLOR_NEGATIVE_TEXT]generate[ENDCOLOR] nor receive foreign Religious Pressure and generate +3 [ICON_PEACE] Faith.'
WHERE Tag = 'TXT_KEY_TRAIT_FAITH_FROM_NATURE';

UPDATE Language_en_US
SET Text = 'Unique Celtic {TXT_KEY_UNIT_SPEARMAN} that excels at feature-less hill, tundra, and snow terrain through mobility and combat effectiveness. Consumes no [ICON_MOVES] Moves when pillaging, and earns 200% of opponents'' [ICON_STRENGTH] Combat Strength as [ICON_PEACE] Faith when it kills a unit. Available earlier than the {TXT_KEY_UNIT_SPEARMAN}.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_CELT_PICTISH_WARRIOR';

--------------------
-- China
--------------------
UPDATE Language_en_US
SET Text = 'Creating [ICON_GREAT_WORK] Great Works or gaining Cities grants 5 Turns of "We Love the Empress Day", and a permanent +2 [ICON_FOOD] Food in all Cities. Bonus Yields decline by 50% on Era Change.'
WHERE Tag = 'TXT_KEY_TRAIT_ART_OF_WAR';

UPDATE Language_en_US
SET Text = 'Mandate of Heaven'
WHERE Tag = 'TXT_KEY_TRAIT_ART_OF_WAR_SHORT';

UPDATE Language_en_US
SET Text = 'The Chu-Ko-Nu is the Chinese unique unit, replacing the Crossbowman. The Chu-Ko-Nu deals splash damage and gain additional damage near cities, giving it a hefty punch on offense and defense. This unit is particularly deadly when garrisoned in a City that is under attack.'
WHERE Tag = 'TXT_KEY_UNIT_CHINESE_CHUKONU_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Chinese {TXT_KEY_UNIT_CROSSBOWMAN} that specializes in city defenses and dealing splash damage.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_CHUKONU';

--------------------
-- Denmark
--------------------
UPDATE Language_en_US
SET Text = 'Mycel Hæþen Here'
WHERE Tag = 'TXT_KEY_TRAIT_VIKING_FURY_SHORT';

UPDATE Language_en_US
SET Text = 'Embarked Units gain +1 [ICON_MOVES] Movement and pay just 1 [ICON_MOVES] Movement to Embark or Disembark. Units with Melee Attacks Heal 10 HP, deal 10 damage to adjacent enemies, earn [ICON_GOLD] Gold, and spend no movement when Pillaging.'
WHERE Tag = 'TXT_KEY_TRAIT_VIKING_FURY';

UPDATE Language_en_US
SET Text = 'Unique Danish {TXT_KEY_UNIT_PIKEMAN} that is adept at attacking from the coast. Available earlier than the {TXT_KEY_UNIT_PIKEMAN}. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_AMPHIBIOUS}[ENDCOLOR] and [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_CHARGE}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_CIV5_DENMARK_BERSERKER_HELP';

UPDATE Language_en_US
SET Text = 'The Berserker is the Danish Unique Unit, replacing the Pikeman. This Unit has +1 Movement [ICON_MOVES] compared to the Pikeman and possesses the Amphibious and Charge promotions, allowing it to attack onto land from a Coast tile with no penalty and punish wounded units. Available after researching Metal Casting instead of Steel.'
WHERE Tag = 'TXT_KEY_CIV5_DENMARK_BERSERKER_STRATEGY';

UPDATE Language_en_US
SET Text = 'The Norwegian Ski Infantry can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_CIV5_DENMARK_SKI_INFANTRY_HELP';

UPDATE Language_en_US
SET Text = 'The Norwegian Ski Infantry moves through Snow, Tundra, and Hills at double speed. It also has +25% [ICON_STRENGTH] Combat Strength in Snow, Tundra or Hill terrain if neither Forest nor Jungle are present.'
WHERE Tag = 'TXT_KEY_CIV5_DENMARK_SKI_INFANTRY_STRATEGY';

--------------------
-- Egypt
--------------------
UPDATE Language_en_US
SET Text = 'Receive a free unique Egyptian [ICON_VP_ARTIFACT] Artifact when constructed.[NEWLINE][NEWLINE]Completing a [ICON_CARAVAN] Land Trade Route originating here and targeting another Civilization, triggers a (or strengthens an existing) [COLOR_POSITIVE_TEXT]Historic Event[ENDCOLOR].[NEWLINE][NEWLINE]+1 [ICON_GOLD] Gold and [ICON_FOOD] Food for every 2 Desert or Tundra tiles worked by the City. [ICON_CARAVAN] Land Trade Routes gain +50% Range and +3 [ICON_GOLD] Gold, [ICON_VP_MERCHANT] Merchants in this City gain +1 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]Nearby [ICON_RES_TRUFFLES] Truffles: +2 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COTTON] Cotton: +1 [ICON_PRODUCTION] Production, +1 [ICON_CULTURE] Culture.[NEWLINE]Neabry [ICON_RES_FUR] Furs: +1 [ICON_GOLD] Gold, +1 [ICON_PRODUCTION] Production.'
WHERE Tag = 'TXT_KEY_BUILDING_BURIAL_TOMB_HELP';

UPDATE Language_en_US
SET Text = 'The Burial Tomb is a Classical-era building which increases your output of [ICON_PEACE] Faith, [ICON_TOURISM] Tourism, and [ICON_CULTURE] Culture, and comes with a free Artifact. It also greatly boosts the value of Trade Routes that target the City. It replaces the Caravansary.'
WHERE Tag = 'TXT_KEY_BUILDING_BURIAL_TOMB_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Egyptian {TXT_KEY_UNIT_CHARIOT_ARCHER} that does not require [ICON_RES_HORSE] {TXT_KEY_RESOURCE_HORSES} and grants [ICON_PRODUCTION] Production from defeated enemies.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_EGYPTIAN_WAR_CHARIOT';

UPDATE Language_en_US
SET Text = '+20% [ICON_PRODUCTION] Production towards [ICON_WONDER] Wonders (+40% during [ICON_GOLDEN_AGE] Golden Ages). [ICON_VP_ARTIFACT] Artifacts gain +5 [ICON_RESEARCH] Science and [ICON_CULTURE] Culture, and Landmarks gain +5 [ICON_GOLD] Gold and [ICON_TOURISM] Tourism.'
WHERE Tag = 'TXT_KEY_TRAIT_WONDER_BUILDER';

--------------------
-- England
--------------------
UPDATE Language_en_US
SET Text = '+1 [ICON_MOVES] Movement for Naval and Embarked Units, and -25% Naval Unit [ICON_GOLD] Gold maintenance. Spy Resistance increased by 25% in all owned Cities. [ICON_SPY] Spies +10% stronger and operate one Rank higher than normal. Starts with a [ICON_SPY] Spy.'
WHERE Tag = 'TXT_KEY_TRAIT_OCEAN_MOVEMENT';

UPDATE Language_en_US
SET Text = 'Perfidious Albion'
WHERE Tag = 'TXT_KEY_TRAIT_OCEAN_MOVEMENT_SHORT';

UPDATE Language_en_US
SET Text = 'The Longbowman can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_LONGBOWMAN';

UPDATE Language_en_US
SET Text = ' The Longbowman has a greater range than the Crossbowman, allowing it to attack enemies three hexes away, often destroying them before they can strike back. Like other ranged units, Longbowmen are vulnerable to melee attack, but have increased [ICON_STRENGTH] against Mounted and Armor Units.'
WHERE Tag = 'TXT_KEY_UNIT_ENGLISH_LONGBOWMAN_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique English {TXT_KEY_UNIT_FRIGATE} that specializes in wresting control of sea and coast with extra vision and splash damage. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_SPLASH}[ENDCOLOR] and [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_SPLASH_II}[ENDCOLOR][NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]Land attacks can only be performed on Coastal tiles.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SHIPOFTHELINE';

UPDATE Language_en_US
SET Text = 'The Ship of the Line is the English unique unit, replacing the Frigate.  Use its [COLOR_POSITIVE_TEXT]Splash[ENDCOLOR] promotions to clear out masses of enemy ships. It can also see one hex farther than the Frigate, allowing it to spot enemies in a significantly larger section of ocean. Like the frigate, it cannot fire on non-coastal land tiles.'
WHERE Tag = 'TXT_KEY_UNIT_ENGLISH_SHIPOFTHELINE_STRATEGY';

--------------------
-- Ethiopia
--------------------
UPDATE Language_en_US
SET Text = 'When you complete a Policy Branch, adopt new Beliefs, or choose your first Ideology, receive 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Technology. +1 [ICON_PEACE] Faith from Strategic Resources.'
WHERE Tag = 'TXT_KEY_TRAIT_BONUS_AGAINST_TECH';

UPDATE Language_en_US
SET Text = 'Solomonic Wisdom'
WHERE Tag = 'TXT_KEY_TRAIT_BONUS_AGAINST_TECH_SHORT';

UPDATE Language_en_US
SET Text = 'Unique Ethiopian {TXT_KEY_UNIT_RIFLEMAN} that specialize in fighting in Friendly Territory especially when near the [ICON_CAPITAL] Capital. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_COVER_1}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MEHAL_SEFARI';

UPDATE Language_en_US
SET Text = 'Mehal Sefari are the backbone of the Ethiopian army. They start with promotions that give them bonuses fighting in owned land. They are slightly cheaper to build than the Fusiliers that they replace.'
WHERE Tag = 'TXT_KEY_UNIT_MEHAL_SEFARI_STRATEGY';

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BUILDING_STELE_HELP', '+34% Faster [ICON_CULTURE_LOCAL] Border Growth in this City. +25% [ICON_PEACE] Faith during [ICON_GOLDEN_AGE] Golden Ages.');

--------------------
-- France
--------------------
UPDATE Language_en_US
SET Text = 'Melee and Gunpowder Units may enlist defeated enemies, which do not consume [ICON_WAR] Unit Supply. +1 [ICON_CULTURE] Culture in every City for every 10 Military Units. +25% [ICON_SILVER_FIST] Unit Supply from [ICON_CITIZEN] Population.'
WHERE Tag = 'TXT_KEY_TRAIT_ENHANCED_CULTURE';

UPDATE Language_en_US
SET Text = 'La Grande Armée'
WHERE Tag = 'TXT_KEY_TRAIT_ENHANCED_CULTURE_SHORT';

UPDATE Language_en_US
SET Text = 'The Musketeer is the French unique unit, replacing the Tercio. The Musketeer is significantly more powerful than the Tercio, gains +1 [ICON_MOVES] Movement, and ignores Zone of Control.'
WHERE Tag = 'TXT_KEY_UNIT_FRENCH_MUSKETEER_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique French {TXT_KEY_UNIT_SPANISH_TERCIO} that excels at delivering a fast attack into the enemy. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_LIGHTNING_WARFARE}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MUSKETEER';

INSERT INTO Language_en_US (Text, Tag)
VALUES ('Napoleon stole {1_Num} [ICON_GREAT_WORK] Great Work(s) from cities near {2_City} when he conquered it!' , 'TXT_KEY_ART_PLUNDERED');

INSERT INTO Language_en_US (Text, Tag)
VALUES ('Art stolen!' , 'TXT_KEY_ART_PLUNDERED_SUMMARY');

INSERT INTO Language_en_US (Text, Tag)
VALUES ('You plundered {1_Num} [ICON_GREAT_WORK] Great Work(s) from cities near {2_City} after you conquered it!' , 'TXT_KEY_ART_STOLEN');

INSERT INTO Language_en_US (Text, Tag)
VALUES ('Art plundered!' , 'TXT_KEY_ART_STOLEN_SUMMARY');

INSERT INTO Language_en_US (Text, Tag)
VALUES ('The conquest of {2_City} has increased your empire''s total [ICON_CULTURE] Culture output by {3_Num}% for the next {1_Num} turn(s)!' , 'TXT_KEY_CULTURE_BOOST_ART');

INSERT INTO Language_en_US (Text, Tag)
VALUES ('Culture boost!' , 'TXT_KEY_CULTURE_BOOST_ART_SUMMARY');

INSERT INTO Language_en_US (Text, Tag)
VALUES ('The conquest of {2_City} has increased the [ICON_PRODUCTION] Production output of all cities by {3_Num}% for the next {1_Num} turn(s)!' , 'TXT_KEY_PRODUCTION_BOOST_ART');

INSERT INTO Language_en_US (Text, Tag)
VALUES ('Production boost!' , 'TXT_KEY_PRODUCTION_BOOST_ART_SUMMARY');

INSERT INTO Language_en_US (Text, Tag)
VALUES ('[NEWLINE][ICON_BULLET]Modifier from Esprit de Corps {2_Num}: {1_Num}%' , 'TXT_KEY_PRODMOD_TRAIT_BONUS_CONQUEST');

INSERT INTO Language_en_US (Text, Tag)
VALUES ('({1_Num} Turn(s) remaining)' , 'TXT_KEY_PRODMOD_TRAIT_BONUS_CONQUEST_TURNS');

UPDATE Language_en_US
SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] from a temporary Cultural Boost (Turns left: {2_TurnsLeft}).'
WHERE Tag = 'TXT_KEY_TP_CULTURE_FROM_BONUS_TURNS';

UPDATE Language_en_US
SET Text = 'A Chateau can only be built adjacent to a Luxury resource. It may not be adjacent to another Chateau. It provides the same +50% defense bonus as a Fort. Can only be built in French territory. Contains [COLOR_POSITIVE_TEXT]Fortifications[ENDCOLOR], allowing owned Units to stay stationed in this tile after attacking.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_CHATEAU_HELP';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CIV5_IMPROVEMENTS_CHATEAU_HELP}[NEWLINE][NEWLINE]A chateau is a manor house or country home of gentry, usually without fortifications. In the Middle Ages, a chateau was largely self-sufficient, being supported by the lord''s demesne (hereditary lands). In the 1600s, the wealthy and aristocratic French lords dotted the countryside with elegant, luxuriant, architecturally refined mansions such as the Chateau de Maisons. Today, the term chateau is loosely used; for instance, it is common for any winery or inn, no matter how humble, to prefix its name with "Chateau."'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_CHATEAU_TEXT';

UPDATE Language_en_US
SET Text = '+50% [ICON_STRENGTH] Defensive Strength for any Unit stationed in this Tile. Requires an adjacent Luxury resource to be built. Contains [COLOR_POSITIVE_TEXT]Fortifications[ENDCOLOR], allowing owned Units to stay stationed in this tile after attacking.'
WHERE Tag = 'TXT_KEY_BUILD_CHATEAU_HELP';

UPDATE Language_en_US
SET Text = 'Established in 1831, the French Foreign Legion is a unique infantry unit in the French army. The legion was specifically created for foreign nationals who wished to fight for France. Famously courageous and totally indifferent to personal safety, the Foreign Legion have seen service in every French war since their inception. The unit has been highly romanticized - according to popular culture, it is a place where heartbroken men go to forget women and scoundrels go to escape justice. This may or may not be accurate, but whatever the case the Legion is a tough and very effective fighting force.[NEWLINE][NEWLINE]The practice of recruiting foreign nationals into its own corps has been emulated by other countries, such as the Dutch KNIL established in 1814, the Chinese Ever Victorious Army in 1860, the Spanish Foreign Legion in 1920, and the Israeli Mahal in 1948.'
WHERE Tag = 'TXT_KEY_CIV5_INDUSTRIAL_FOREIGNLEGION_TEXT';

--------------------
-- Germany
--------------------
UPDATE Language_en_US
SET Text = '10% of the City''s [ICON_GOLD] Gold output is added to the City''s [ICON_RESEARCH] Science every turn. +5% [ICON_PRODUCTION] Production in this City for each [ICON_INTERNATIONAL_TRADE] Trade Route your Civilization has with a [ICON_CITY_STATE] City-State.[NEWLINE][NEWLINE]Incoming [ICON_INTERNATIONAL_TRADE] Trade Routes generate +2 [ICON_GOLD] Gold for the City, and +2 [ICON_GOLD] Gold for the [ICON_INTERNATIONAL_TRADE] Trade Route owner.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_GOLD] Poverty.[NEWLINE][NEWLINE]Nearby [ICON_RES_BANANA] Bananas: +2 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COFFEE] Coffee: +1 [ICON_GOLD] Gold, +2 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_TEA] Tea: +2 [ICON_GOLD] Gold, +1 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_TOBACCO] Tobacco: +3 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_HANSE_HELP';

UPDATE Language_en_US
SET Text = 'In addition to the regular abilities of the Customs House (additional [ICON_GOLD] Gold output, and additional [ICON_GOLD] Gold from Trade Routes), the Hanse provides extra [ICON_GOLD] Gold and an additional Specialist. Also, [ICON_PRODUCTION] Production for each Trade Route within your civilization that connects to a City-State. The Trade Routes can come from any combination of cities, even cities without the Hanse (Example: If you have trade routes from Berlin to Geneva, Munich to Geneva, Munich to Berlin, and Berlin to Brussels, then all cities with the Hanse would get +12% [ICON_PRODUCTION] Production). Only Germany may build it.'
WHERE Tag = 'TXT_KEY_BUILDING_HANSE_STRATEGY';

UPDATE Language_en_US
SET Text = '+2 [ICON_RESEARCH] Science in [ICON_CAPITAL] Capital per [ICON_CITY_STATE] City-State Friend, and additionally +2 [ICON_CULTURE] Culture per Ally, scaling with Era. Gain +1 [ICON_INFLUENCE] Influence per turn with City-States for each gifted Unit (lost when killed or upgraded).'
WHERE Tag = 'TXT_KEY_TRAIT_CONVERTS_LAND_BARBARIANS';

UPDATE Language_en_US
SET Text = 'Blood and Iron'
WHERE Tag = 'TXT_KEY_TRAIT_CONVERTS_LAND_BARBARIANS_SHORT';

UPDATE Language_en_US
SET Text = 'T-34'
WHERE Tag = 'TXT_KEY_UNIT_GERMAN_PANZER';

UPDATE Language_en_US
SET Text = 'Unique Land Unit that is incredibly effective on open ground.[NEWLINE][NEWLINE]Starts with the [COLOR_POSITIVE_TEXT]Armor Plating I[ENDCOLOR] and [COLOR_POSITIVE_TEXT]Mobility[ENDCOLOR] promotions.[NEWLINE][NEWLINE]Only available to Civilizations following the [COLOR_MAGENTA]Order[ENDCOLOR] Ideology. Requires the [COLOR_MAGENTA]Patriotic War[ENDCOLOR] Tenet to be unlocked.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_PANZER';

UPDATE Language_en_US
SET Text = 'The T-34 is unique to the Order Ideology. Requires the Patriotic War Tenet to be unlocked. The T-34 is stronger, faster, and has more defense than the tank. It can move after combat, allowing it to blow huge holes in enemy lines and then barrel through before the enemy can repair the gap.'
WHERE Tag = 'TXT_KEY_UNIT_GERMAN_PANZER_STRATEGY';

UPDATE Language_en_US
SET Text = 'The T-34 was the main medium tank of the Soviet Red Army during World War II. One of the most influential tank designs in the world, the T-34 combined strong, sloped armor, a powerful gun, speed, and cross-country reliability, totally outmatching any German tank that was sent to fight against them. With its proven design combined with the Soviets'' overwhelming industrial capacity, the T-34 also became one of the most cost effective and most produced tanks in history, with over 80,000 built. Even today, numerous countries use T-34s in their armored divisions. Truly a tank that could stand the test of time!'
WHERE Tag = 'TXT_KEY_CIV5_INDUSTRIAL_PANZER_TEXT';

UPDATE Language_en_US
SET Text = 'Unique German {TXT_KEY_UNIT_SPANISH_TERCIO} that specializes as mercenaries, having no experience penalty or cooldown while being able to move immediately when purchased. Deals increased damage against enemy Units at full health. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_FORMATION_2}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_LANDSKNECHT';

UPDATE Language_en_US
SET Text = 'In addition to being cheaper than normal Tercios, the Landsknecht gain full XP and can move immediately when bought, which allows them to be quickly amassed when needed.[NEWLINE][NEWLINE]Use their fast deployment and their bonus against full health units to surprise your opponents in the first turns of your wars and break their lines quickly.[NEWLINE][NEWLINE]The Landsknecht''s cheap cost also makes them excellent gifts. Buy several units at once, walk them over to a nearby City State, and reap the benefits of both short- and long-term influence.'
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
SET Text = 'Unique Greek {TXT_KEY_UNIT_SPEARMAN} that is stronger and produce more [ICON_GREAT_GENERAL] Great General Points from combat than normal. Gains [ICON_STRENGTH] Combat Strength for each adjacent friendly unit.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_HOPLITE';

UPDATE Language_en_US
SET Text = 'The Companion Cavalry can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_COMPANION_CAVALRY';

UPDATE Language_en_US
SET Text = 'Companion Cavalry are faster and more powerful than the Horseman unit, making them the most dangerous mounted units until the arrival of the Knight. They help generate Great Generals more rapidly than other units, and benefit greatly from being stacked with one. Use a stacked Great General''s increased movement speed to keep up with its Companion Cavalry retinue.'
WHERE Tag = 'TXT_KEY_UNIT_GREEK_COMPANIONCAVALRY_STRATEGY';

--------------------
-- Huns
--------------------
UPDATE Language_en_US
SET Text = 'Inflict +100% and receive -50% [ICON_HAPPINESS_3] [COLOR_POSITIVE_TEXT]War Weariness[ENDCOLOR]. +10% [ICON_STRENGTH] Combat Strength for each subsequent attack against a single target during a turn. Claim adjacent unowned land tiles when Cities claim a tile of the same type.'
WHERE Tag = 'TXT_KEY_TRAIT_RAZE_AND_HORSES';

UPDATE Language_en_US
SET Text = 'The Battering Ram can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_HUN_BATTERING_RAM';

UPDATE Language_en_US
SET Text = 'After defenders have been cleared away, use Battering Rams to quickly move to an enemy City and knock down its defenses with melee attacks. Battering Rams exist for a long time and become increasingly vulnerable to the units of newer eras, so keep that in mind when choosing new promotions for it.'
WHERE Tag = 'TXT_KEY_UNIT_HUN_BATTERING_RAM_STRATEGY';

UPDATE Language_en_US
SET Text = 'The barbarians in this Encampment have joined your army! All hail glorious Attila!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_BARB_CAMP_CONVERTS';

UPDATE Language_en_US
SET Text = 'Horse Archers are fast ranged units that require Horses. They start with the Barrage I and Focus Fire promotions and are strong flankers. As a mounted unit, the Horse Archer is vulnerable to Spearmen.'
WHERE Tag = 'TXT_KEY_UNIT_HUN_HORSE_ARCHER_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Hunnic {TXT_KEY_UNIT_MONGOL_KESHIK} that specializes in hit-and-run attacks. Gains combat bonuses for subsequent attacks on the same target in a turn. Starts with the [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_BARRAGE_1}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_HUN_HORSE_ARCHER';

--------------------
-- Inca
--------------------
UPDATE Language_en_US
SET Text = 'Units ignore terrain costs when on Hills and can cross Mountains. Cities, Roads, and Railroads may be built on Mountains, and Mountains can be worked to produce +1 [ICON_FOOD] Food, [ICON_GOLD] Gold, and [ICON_RESEARCH] Science, scaling with Era.'
WHERE Tag = 'TXT_KEY_TRAIT_GREAT_ANDEAN_ROAD';

UPDATE Language_en_US
SET Text = 'Waraq''Ak'
WHERE Tag = 'TXT_KEY_UNIT_INCAN_SLINGER';

UPDATE Language_en_US
SET Text = 'Unique Incan {TXT_KEY_UNIT_VP_SLINGER} that has increased [ICON_RANGE_STRENGTH] Attack Range, ignore enemy [COLOR_POSITIVE_TEXT]Zone of Control[ENDCOLOR], and can daze enemy units, lowering their [ICON_STRENGTH] Combat Strength.'
WHERE Tag = 'TXT_KEY_CIV5_INCA_SLINGER_HELP';

UPDATE Language_en_US
SET Text = 'The Waraq''Ak is an Ancient Era ranged infantry Unit that can strike foes from afar. This Incan Unique Unit can daze enemy units; use it to harass your foes and take down Barbarian camps quickly.'
WHERE Tag = 'TXT_KEY_CIV5_INCA_SLINGER_STRATEGY';

UPDATE Language_en_US
SET Text = 'Pata-Pata'
WHERE Tag = 'TXT_KEY_IMPROVEMENT_TERRACE_FARM';

UPDATE Language_en_US
SET Text = 'The Pata-Pata can only be built on hills and does not need a source of fresh water to do so. If built next to a mountain it will provide additional [ICON_FOOD] Food (1 per adjacent Mountain). +1 [ICON_FOOD] Food for each adjacent Pata-Pata, and all adjacent Farms gain +1 [ICON_FOOD] Food.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_TERRACE_FARM_HELP';

UPDATE Language_en_US
SET Text = 'Terrace Farming developed in mountainous areas simultaneously around the world, including Bali, the Philippines, China, and Peru.  These skinny, stepped fields are cut into hill and mountain sides, preventing the run off of irrigation water and providing space for arable land, usually where none previously was possible. The Incas in particular were masters of terrace farming, a technique handed down to them by the older Wari culture, and called Pata-Pata (meaning levelled place) in their native Quechua. The Inca erected large, drystone walls to hold their terraces in place and constructed systems of canals and aqueducts to provide the terraces with constant water, increasing the land''s fertility.  The Incan terracing techniques were so successful that modern Peruvian farmers still employ them in their farms today.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_TERRACE_FARM_TEXT';

UPDATE Language_en_US
SET Text = 'Construct a [LINK=IMPROVEMENT_TERRACE_FARM]Pata-Pata[\LINK]'
WHERE Tag = 'TXT_KEY_BUILD_TERRACE_FARM';
--------------------
-- India
--------------------
UPDATE Language_en_US
SET Text = 'Starts with a [ICON_RELIGION_PANTHEON] Pantheon. [ICON_PROPHET] Great Prophets require 35% less [ICON_PEACE] Faith. Founding a [ICON_RELIGION] Religion converts all owned Cities. +10% base Religious Pressure and +1% [ICON_FOOD] Food for each Follower of your primary [ICON_RELIGION] Religion in a City. [COLOR_NEGATIVE_TEXT]Cannot build [ICON_MISSIONARY] Missionaries.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_TRAIT_POPULATION_GROWTH';

UPDATE Language_en_US
SET Text = 'Naga-Malla' WHERE Tag = 'TXT_KEY_UNIT_INDIAN_WARELEPHANT';

UPDATE Language_en_US
SET Text = 'A terrifying sight on the battlefield, often as dangerous to its own side as it was to the enemy, the Naga-Malla (the Hindu term for Fighting Elephant) could be considered the first weapon of mass destruction. Indians used elephants in war until the advent of gunpowder rendered them obsolete. In pre-gunpowder battle the war elephant served two primary purposes. First, their scent absolutely terrified horses, rendering enemy cavalry useless. Second, they could break through even the strongest line of infantry, crushing a wall of spears that no horse could ever surmount. Elephants were incredibly difficult to kill and history records them surviving sixty or more arrows. The primary problem with elephants was their tendency to go berserk with pain or rage, at which point they became impossible for their rider to control. Elephant riders often carried a spike and mallet that they could use to kill the animals if they attacked their own forces.'
WHERE Tag = 'TXT_KEY_CIV5_ANTIQUITY_INDIANWARELEPHANT_TEXT';

UPDATE Language_en_US
SET Text = 'Unique Indian {TXT_KEY_DESC_CUIRASSIER} that does not require [ICON_RES_HORSE] {TXT_KEY_RESOURCE_HORSES} but loses [COLOR_NEGATIVE_TEXT]{TXT_KEY_PROMOTION_SKIRMISHER_DOCTRINE}[ENDCOLOR] to make up for its immense Combat Strength and Ranged Combat Strength. Available earlier than the {TXT_KEY_DESC_CUIRASSIER}. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_ACCURACY_1}[ENDCOLOR] and [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_FEARED_ELEPHANT}[ENDCOLOR], weakening nearby enemies.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_INDIAN_WAR_ELEPHANT';

UPDATE Language_en_US
SET Text = 'The Naga-Malla (Fighting Elephant) is the Indian unique unit, replacing the Cuirassier. It is more powerful than the Cuirassier as it does not require Horses, and is available earlier than the Cuirassier. A mounted unit, the Naga-Malla is quite vulnerable to the Pikeman.'
WHERE Tag = 'TXT_KEY_UNIT_INDIAN_WARELEPHANT_STRATEGY';

UPDATE Traits
SET ShortDescription = 'TXT_KEY_TRAIT_POPULATION_GROWTH_SHORT_CBP'
WHERE Type = 'TRAIT_POPULATION_GROWTH';

--------------------
-- Indonesia
--------------------
UPDATE Language_en_US
SET Text = 'Unique Indonesian Garden replacement. A [ICON_RES_CLOVES] Cloves, [ICON_RES_PEPPER] Pepper, or [ICON_RES_NUTMEG] Nutmeg Resource will appear near or under this City when built.[NEWLINE][NEWLINE]+25% [ICON_GREAT_PEOPLE] Great People generation in this City.[NEWLINE][NEWLINE]+15% to [ICON_PEACE] Faith and [ICON_CULTURE] Culture in the City during "We Love the King Day."[NEWLINE][NEWLINE]1 Specialist in this City no longer produces [ICON_HAPPINESS_3] Unhappiness from Urbanization.[NEWLINE][NEWLINE]Nearby Oases: +2 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_CITRUS] Citrus: +1 [ICON_FOOD] Food, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COCOA] Cocoa: +1 [ICON_FOOD] Food, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_CLOVES] Cloves: +1 [ICON_PEACE] Faith, +1 [ICON_CULTURE] Culture.[NEWLINE]Nearby [ICON_RES_PEPPER] Pepper: +1 [ICON_PEACE] Faith, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_NUTMEG] Nutmeg: +1 [ICON_CULTURE] Culture, +1 [ICON_PRODUCTION] Production.'
WHERE Tag = 'TXT_KEY_BUILDING_CANDI_HELP';

UPDATE Language_en_US
SET Text = 'In addition to the regular abilities of the Garden, the Candi provides additional [ICON_PEACE] Faith and [ICON_CULTURE] Culture, and gains an additional boost to these yields during "We Love the King Day." Only Indonesia may build it.'
WHERE Tag = 'TXT_KEY_BUILDING_CANDI_STRATEGY';

UPDATE Language_en_US
SET Text = 'When you gain a City, one of 3 unique Luxuries will appear nearby. +5% to Yield and [ICON_GOLDEN_AGE] Golden Age duration modifiers from [ICON_VP_MONOPOLY] Global Monopolies; +2 to Yields and [ICON_HAPPINESS_1] Happiness from [ICON_VP_MONOPOLY] Global Monopolies.'
WHERE Tag = 'TXT_KEY_TRAIT_SPICE';

UPDATE Language_en_US
SET Text = 'Unique Indonesian {TXT_KEY_UNIT_SWORDSMAN} that has a [COLOR:30:200:255:255]{TXT_KEY_PROMOTION_MYSTIC_BLADE}[ENDCOLOR] whose abilities will be discovered the first time it is used in combat.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_INDONESIAN_KRIS_SWORDSMAN';

UPDATE Language_en_US
SET Text = Text || '[NEWLINE][NEWLINE][ICON_BULLET][COLOR:30:200:255:255]{TXT_KEY_PROMOTION_INVULNERABIILITY}[ENDCOLOR]: Damage from all sources reduced by 2. +20 HP when healing.[NEWLINE][ICON_BULLET][COLOR:30:200:255:255]{TXT_KEY_PROMOTION_SNEAK_ATTACK}[ENDCOLOR]: [COLOR_POSITIVE_TEXT]Flanking bonus[ENDCOLOR] increased by 20%. Ignores [COLOR_POSITIVE_TEXT]Terrain Costs[ENDCOLOR] and [COLOR_POSITIVE_TEXT]Zone of Control[ENDCOLOR].[NEWLINE][ICON_BULLET][COLOR:30:200:255:255]{TXT_KEY_PROMOTION_AMBITION}[ENDCOLOR]: +30% [ICON_STRENGTH] Combat Strength when attacking. +5% [ICON_STRENGTH] Combat Strength when defending.[NEWLINE][ICON_BULLET][COLOR:30:200:255:255]{TXT_KEY_PROMOTION_RESTLESSNESS}[ENDCOLOR]: May [COLOR_POSITIVE_TEXT]Attack Twice[ENDCOLOR]. +1 [ICON_MOVES] Movement.[NEWLINE][ICON_BULLET][COLOR:30:200:255:255]{TXT_KEY_PROMOTION_ENEMY_BLADE}[ENDCOLOR]: Heals 10 HP if the unit begins its turn in [COLOR_POSITIVE_TEXT]Enemy Territory[ENDCOLOR]. 10 Damage to all adjacent enemy Units [COLOR_POSITIVE_TEXT]each Turn[ENDCOLOR][NEWLINE][ICON_BULLET][COLOR:30:200:255:255]{TXT_KEY_PROMOTION_EVIL_SPIRITS}[ENDCOLOR]: +5% [ICON_STRENGTH] Combat Strength when attacking. +30% [ICON_STRENGTH] Combat Strength when defending.'
WHERE Tag = 'TXT_KEY_PROMOTION_MYSTIC_BLADE_HELP';

UPDATE Language_en_US
SET Text = 'Heals 10 HP if the unit begins its turn in [COLOR_POSITIVE_TEXT]Enemy Territory[ENDCOLOR].[NEWLINE]10 Damage to all adjacent enemy Units [COLOR_POSITIVE_TEXT]each Turn[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_ENEMY_BLADE_HELP';

UPDATE Language_en_US
SET Text = 'Enemy Blade'
WHERE Tag = 'TXT_KEY_PROMOTION_ENEMY_BLADE';

UPDATE Language_en_US
SET Text = '+5% [ICON_STRENGTH] Combat Strength when attacking.[NEWLINE]+30% [ICON_STRENGTH] Combat Strength when defending.'
WHERE Tag = 'TXT_KEY_PROMOTION_EVIL_SPIRITS_HELP';

UPDATE Language_en_US
SET Text = 'Ancestral Protection'
WHERE Tag = 'TXT_KEY_PROMOTION_EVIL_SPIRITS';

UPDATE Language_en_US
SET Text = '+30% [ICON_STRENGTH] Combat Strength when attacking.[NEWLINE]+5% [ICON_STRENGTH] Combat Strength when defending.'
WHERE Tag = 'TXT_KEY_PROMOTION_AMBITION_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Flanking bonus[ENDCOLOR] increased by 20%.[NEWLINE]Ignores [COLOR_POSITIVE_TEXT]Terrain Costs[ENDCOLOR] and [COLOR_POSITIVE_TEXT]Zone of Control[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_SNEAK_ATTACK_HELP';

UPDATE Language_en_US
SET Text = 'Damage from all sources reduced by 2.[NEWLINE]+20 HP when healing.'
WHERE Tag = 'TXT_KEY_PROMOTION_INVULNERABIILITY_HELP';

UPDATE Language_en_US
SET Text = 'Sumpah Palapa'
WHERE Tag = 'TXT_KEY_TRAIT_SPICE_SHORT';

UPDATE Language_en_US
SET Text = 'Appears as a unique luxury good in or near an Indonesian City.'
WHERE Tag = 'TXT_KEY_RESOURCE_NUTMEG_HELP';

UPDATE Language_en_US
SET Text = 'Appears as a unique luxury good in or near an Indonesian City.'
WHERE Tag = 'TXT_KEY_RESOURCE_CLOVES_HELP';

UPDATE Language_en_US
SET Text = 'Appears as a unique luxury good in or near an Indonesian City.'
WHERE Tag = 'TXT_KEY_RESOURCE_PEPPER_HELP';

--------------------
-- Iroquois
--------------------
UPDATE Language_en_US
SET Text = 'Units move through Forest and Jungle as if they were roads, and these tiles establish [ICON_CONNECTED] City Connections. Military Land Units start with the [COLOR_POSITIVE_TEXT]Woodsman[ENDCOLOR] Promotion.'
WHERE Tag = 'TXT_KEY_TRAIT_IGNORE_TERRAIN_IN_FOREST';

UPDATE Language_en_US
SET Text = '+1 [ICON_PRODUCTION] Production and +1 [ICON_FOOD] Food from Forests, Jungles, Marshes, and Plantations worked by this City.'
WHERE Tag = 'TXT_KEY_BUILDING_LONGHOUSE_HELP';

UPDATE Language_en_US
SET Text = 'The Longhouse is the Iroquois unique building, replacing the Herbalist. It increases the City''s Food and Production output for each forest within the City''s radius, and improves Plantations.'
WHERE Tag = 'TXT_KEY_BUILDING_LONGHOUSE_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Iroquois {TXT_KEY_UNIT_SWORDSMAN} that excels at Forest and Jungle warfare and does not require [ICON_RES_IRON] {TXT_KEY_RESOURCE_IRON}.' /*Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_WOODSMAN}[ENDCOLOR]. Technically it should say that.. but the UA also grants it...*/
WHERE Tag = 'TXT_KEY_UNIT_HELP_MOHAWK_WARRIOR';

--------------------
-- Japan
--------------------
UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture and [ICON_PEACE] Faith from Defense Buildings. When a [ICON_GREAT_ADMIRAL] Great Admiral or [ICON_GREAT_GENERAL] Great General is born, gain 50% progress toward a [ICON_GREAT_WORK] Great Artist, Writer, and Musician in your [ICON_CAPITAL] Capital.'
WHERE Tag = 'TXT_KEY_TRAIT_FIGHT_WELL_DAMAGED';

UPDATE Language_en_US
SET Text = 'Shogunate'
WHERE Tag = 'TXT_KEY_TRAIT_FIGHT_WELL_DAMAGED_SHORT';

UPDATE Language_en_US
SET Text = 'The Samurai are the Japanese unique unit, replacing the Longswordsman. It is more powerful as the Swordsman, generates more Great Generals, and gains more experience from combat.'
WHERE Tag = 'TXT_KEY_UNIT_JAPANESE_SAMURAI_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Japanese {TXT_KEY_UNIT_LONGSWORDSMAN} that is stronger, gains more experience, and produce more [ICON_GREAT_GENERAL] Great General Points from combat than normal.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SAMURAI';

UPDATE Language_en_US
SET Text = 'Air Unit designed to wrest control of the skies and intercept incoming Enemy Aircraft. This Unit has a bonus against other Fighters and does not require Oil. [COLOR_POSITIVE_TEXT]Does not use Military Supply.[ENDCOLOR][NEWLINE][NEWLINE]Only available to Civilizations following the [COLOR_MAGENTA]Autocracy[ENDCOLOR] Ideology. Requires the [COLOR_MAGENTA]Air Supremacy[ENDCOLOR] Tenet to be unlocked.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ZERO';

UPDATE Language_en_US
SET Text = 'The Zero is unique to the Autocracy Ideology. Requires the Air Supremacy Tenet to be unlocked. The Zero is a moderately-powerful air unit. It is like the standard fighter, except that it gets a significant combat bonus when battling other fighters and [COLOR_POSITIVE_TEXT]does not require the Oil resource[ENDCOLOR]. It can be based in any City you own or aboard an aircraft carrier. It can move from one City to another (or an aircraft carrier) and can perform "missions" within its range of 8 tiles. See the rules on Aircraft for more information.'
WHERE Tag = 'TXT_KEY_UNIT_JAPANESE_ZERO_STRATEGY';

--------------------
-- Korea
--------------------
UPDATE Language_en_US
SET Text = '+1 [ICON_RESEARCH] Science from Specialists, increasing by +1 in Medieval, Industrial, and Atomic Eras. +20% [ICON_RESEARCH] Science during [ICON_GOLDEN_AGE] Golden Ages, and +50 [ICON_GOLDEN_AGE] Golden Age Points when a [ICON_GREAT_PEOPLE] Great Person is born, scaling with Era.'
WHERE Tag = 'TXT_KEY_TRAIT_SCHOLARS_JADE_HALL';

UPDATE Language_en_US
SET Text = 'Unique Korean {TXT_KEY_UNIT_TREBUCHET} that act as an anti-personnel rocket unit that is strong against land units, but weak against cities. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_LOGISTICS}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_CIV5_KOREA_HWACHA_HELP';

UPDATE Language_en_US
SET Text = 'The Turtle Ship can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_CIV5_KOREA_TURTLESHIP_HELP';

UPDATE Language_en_US
SET Text = 'The Turtle Ship has a more powerful attack than the Caravel, and is extremely difficult to destroy due to its Indomitable promotion.  However, while it can end its movement in Ocean tiles, the Turtle Ship still travels at half speed through them.'
WHERE Tag = 'TXT_KEY_CIV5_KOREA_TURTLESHIP_STRATEGY';

--------------------
-- Maya
--------------------
UPDATE Language_en_US
SET Text = 'After researching [COLOR_CYAN]Mathematics[ENDCOLOR], receive a bonus [ICON_GREAT_PEOPLE] Great Person at the end of every Maya Long Count cycle (every 394 years). Each bonus [ICON_GREAT_PEOPLE] Great Person can only be chosen once.'
WHERE Tag = 'TXT_KEY_TRAIT_LONG_COUNT';

UPDATE Language_en_US
SET Text = 'Unique Mayan {TXT_KEY_UNIT_COMPOSITE_BOWMAN} that is cheaper and available earlier than the {TXT_KEY_UNIT_COMPOSITE_BOWMAN}. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_INDIRECT_FIRE}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MAYAN_ATLATLIST';

UPDATE Language_en_US
SET Text = 'The Atlatlist is the Mayan unique unit, replacing the Composite Bowman. Atlatlists are cheaper and available earlier than Composite Bowmen, and can attack over obstacles. This advantage allows your archers to engage in hit-and-run skirmish tactics.'
WHERE Tag = 'TXT_KEY_UNIT_MAYAN_ATLATLIST_STRATEGY';

--------------------
-- Mongols
--------------------
UPDATE Language_en_US
SET Text = 'Skirmisher Units have an extra Attack. Gain All Yields equal to 20% of the [ICON_GOLD] Gold from bullying [ICON_CITY_STATE] City-States.'
WHERE Tag = 'TXT_KEY_TRAIT_TERROR';

UPDATE Language_en_US
SET Text = 'Skirmisher' WHERE Tag = 'TXT_KEY_UNIT_MONGOL_KESHIK';

UPDATE Language_en_US
SET Text = 'In the 12th century, nomadic tribes came boiling out of Central Asia, conquering most of Asia, Eastern Europe and a large portion of the Middle East within a century. Their primary weapon was their incomparable mounted bowmen. The Mongols were one such nomadic tribe, and their children were almost literally "raised in the saddle." Riding on their small but hearty steppe ponies, the lightly-armed and armored Mongol Keshiks, a type of skirmisher, could cover an astonishing amount of territory in a day, far outpacing any infantry or even the heavier European cavalry.[NEWLINE][NEWLINE]In battle the Mongol Keshiks would shoot from horseback with deadly accuracy, disrupting and demoralizing the enemy formations. Once the enemy was suitably weakened, the Mongol heavy cavalry units would charge into the wavering foe to deliver the coup de grace. When facing armored European knights the Mongols would simply shoot their horses, then ignore or destroy the unhorsed men as they wished.'
WHERE Tag = 'TXT_KEY_CIV5_MONGOLIA_KESHIK_TEXT';

UPDATE Language_en_US
SET Text = 'Skirmishers possess a ranged attack, an increased movement rate, and increased flanking potential, allowing them to perform hit and run attacks or support the main force.'
WHERE Tag = 'TXT_KEY_CIV5_MONGOLIA_KESHIK_HELP';

UPDATE Language_en_US
SET Text = 'Skirmishers are fast ranged units, deadly on open terrain. Unlike the Chariot before them, they can move through rough terrain without a movement penalty and can strengthen the attacks of flanking units. As a mounted unit, the Skirmisher is vulnerable to units equipped with spears.'
WHERE Tag = 'TXT_KEY_CIV5_MONGOLIA_KESHIK_STRATEGY';

UPDATE Language_en_US
SET Text = 'The Khan is a Mongolian Unique Great Person, replacing the standard Great General.  The same combat bonuses apply, but the Khan moves 5 points per turn, heals stacked and adjacent units for additional HP per turn, and damages adjacent enemies each turn. This beefed up General ensures that his cavalry units will always be in a battle ready state.'
WHERE Tag = 'TXT_KEY_CIV5_MONGOLIA_KHAN_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Mongolian {TXT_KEY_UNIT_GREAT_GENERAL} that specialize in mobile leadership. The same Leadership bonuses apply, but the Khan moves much faster, stacks healing with nearby friendly units, and deal damage to adjacent enemies.[NEWLINE][NEWLINE]Can be expended to construct a unique Mongolian [COLOR_POSITIVE_TEXT]{TXT_KEY_IMPROVEMENT_CITADEL}[ENDCOLOR], an {TXT_KEY_IMPROVEMENT_MONGOLIA_ORDO}.'
WHERE Tag = 'TXT_KEY_CIV5_MONGOLIA_KHAN_HELP';

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BALANCE_KNOWN_CS_BULLY_ANNEXED_KNOWN', '{1_Bully} has forced {2_CS} to surrender by bullying them!');

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BALANCE_KNOWN_CS_BULLY_ANNEXED_KNOWN_SUMMARY', '{1_CS} has surrendered!');

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BALANCE_UNKNOWN_CS_BULLY_ANNEXED_KNOWN', '{1_Bully} has forced an unmet City-State to surrender by bullying them!');

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BALANCE_UNKNOWN_CS_BULLY_ANNEXED_KNOWN_SUMMARY', '{1_Bully} has forced a City-State to surrender!');

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BALANCE_KNOWN_CS_BULLY_ANNEXED_UNKNOWN', 'An unmet player has forced {1_CS} to surrender by bullying them!');

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BALANCE_KNOWN_CS_BULLY_ANNEXED_UNKNOWN_SUMMARY', '{1_CS} has surrendered!');

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BALANCE_ANNEXED_CS', 'You forced {1_CS} to surrender through intimidation!');

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_POPUP_MINOR_BULLY_UNIT_AMOUNT_ANNEX', 'Forcefully Annex City-State');

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_POP_CSTATE_BULLY_UNIT_TT_ANNEX', 'If this City-State is more [COLOR_POSITIVE_TEXT]afraid[ENDCOLOR] of you than they are [COLOR_WARNING_TEXT]resilient[ENDCOLOR], you can annex the City-State. {1_FearLevel}{2_FactorDetails}');

INSERT INTO Language_en_US (Tag, Text)
VALUES ('TXT_KEY_BALANCE_ANNEXED_CS_SUMMARY', 'You intimidated {1_CS}!');

--------------------
-- Morocco
--------------------
UPDATE Language_en_US
SET Text = 'A Kasbah can only be built adjacent to a City. It provides additional [ICON_FOOD] Food, [ICON_PRODUCTION] Production, [ICON_GOLD] Gold, and [ICON_CULTURE] Culture. Adjacent Fishing Boats and Coastal tiles gain +2 [ICON_GOLD] or +1 [ICON_CULTURE] Culture, respectively. It also provides a +30% defense bonus, deals 5 damage to adjacent enemy Military units, and connects any Luxury or Strategic resources underneath it to your trade network. Does not remove any features on the tile.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_KASBAH_HELP';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CIV5_IMPROVEMENTS_KASBAH_HELP}[NEWLINE][NEWLINE]A Kasbah is a type of medina (a walled quarter in a City) found in the countryside, usually a small settlement on a hilltop or hillside. Originally the home for a tribal chieftain or important Islamic imam, the Kasbah is characterized by high-walled, windowless houses and narrow, winding streets. It is usually dominated by a single fortified tower. Kasbahs were common along the North African coast and Middle East until the early 1900s. Building a Kasbah was a mark of wealth, influence, and power for Moroccan and Algerian families of Arab descent.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_KASBAH_TEXT';

UPDATE Language_en_US
SET Text = '+1 to all Yields in [ICON_CAPITAL] Capital per unique [ICON_INTERNATIONAL_TRADE] Trade Route partner, scaling with Era. Distance does not reduce [ICON_INTERNATIONAL_TRADE] Trade Route Yields to or from Moroccan Cities. Can plunder [ICON_INTERNATIONAL_TRADE] Trade Routes between foreign Cities without declaring war.'
WHERE Tag = 'TXT_KEY_TRAIT_GATEWAY_AFRICA';

UPDATE Language_en_US
SET Text = 'Unique Moroccan {TXT_KEY_UNIT_CAVALRY} that specializes in protecting Moroccan lands. Receives combat bonuses when fighting in Moroccan territory and ignores terrain penalties.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BERBER_CAVALRY';

UPDATE Language_en_US
SET Text = 'The Berber Cavalry gives the Moroccans an edge when defending their territory. They they move quickly in the desert, in addition to other difficult terrains. This also allows them to perform raids deep into rough enemy territory but other forces will need to stabilize any gains.'
WHERE Tag = 'TXT_KEY_UNIT_BERBER_CAVALRY_STRATEGY';

UPDATE Language_en_US
SET Text = '+30% [ICON_STRENGTH] Defensive Strength for any Unit stationed in this Tile. Any enemy unit which ends its turn next to the Kasbah takes 5 damage. This Kasbah will connect any Luxury or Strategic Resources on this tile to the trade network. Does not remove any features on the tile.'
WHERE Tag = 'TXT_KEY_BUILD_KASBAH_HELP';

--------------------
-- Netherlands
--------------------
UPDATE Language_en_US
SET Text = '+3 [ICON_CULTURE] Culture and [ICON_GOLD] Gold for each unique Luxury Resource you import or export from/to other Civilizations and [ICON_CITY_STATE] City-States, scaling with Era. Can import duplicate Luxuries from Civilizations which count toward [ICON_VP_MONOPOLY] Monopolies.'
WHERE Tag = 'TXT_KEY_TRAIT_LUXURY_RETENTION';

UPDATE Language_en_US
SET Text = 'Naval Unit used to attack coastal cities and capture enemy ships. Only the Dutch may build it. Heals outside friendly territory. Starts with the Vanguard promotion allowing it to be more effective at attacking coastal cities. May capture any enemy naval units it defeats as prize ships.'
WHERE Tag = 'TXT_KEY_UNIT_SEA_BEGGAR_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Dutch {TXT_KEY_UNIT_PRIVATEER} that specializes in attacking coastal cities and capturing enemy ships. Available earlier than the {TXT_KEY_UNIT_PRIVATEER}. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_COASTAL_TERROR}[ENDCOLOR] and [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_SUPPLY}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SEA_BEGGAR';

UPDATE Language_en_US
SET Text = 'A Polder can be built on Marshes and on any Water tiles adjacent to at least 3 land tiles. It generates [ICON_FOOD] Food, [ICON_GOLD] Gold, and [ICON_PRODUCTION] Production. Provides additional yields once later techs are researched. Does not remove any features on the tile.[NEWLINE][NEWLINE]Polders can be walked on by land units as if they are land tiles.'
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
SET Text = 'The Janissary is an Ottoman unique unit, replacing the Musketman. The Janissary is stronger than the Musketman, available earlier, and heals every turn. They also gain a Combat Strength bonus when attacking. This can give a Janissary army a crushing advantage against an enemy force.'
WHERE Tag = 'TXT_KEY_UNIT_OTTOMAN_JANISSARY_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Ottoman {TXT_KEY_UNIT_MUSKETMAN} that gains a Combat Strength bonus when attacking. Available earlier than the {TXT_KEY_UNIT_MUSKETMAN}. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_MARCH}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_JANISSARY';

UPDATE Language_en_US
SET Text = 'The Sipahi can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SIPAHI';

UPDATE Language_en_US
SET Text = 'The Sipahi is stronger than the Lancer and has massive flanking bonuses. Pin your opponent''s army down using your Gunpowder Units'' Zone Of Control while attacking with your Sipahi from the side. Defeat weakened units to trigger the Overrun bonus damage, then retreat to safety.'
WHERE Tag = 'TXT_KEY_UNIT_OTTOMAN_SIPAHI_STRATEGY';

UPDATE Language_en_US
SET Text = 'Completing a [ICON_INTERNATIONAL_TRADE] Trade Route grants +150 [ICON_RESEARCH] Science and [ICON_FOOD] Food to the origin City if International, or [ICON_CULTURE] Culture and [ICON_GOLD] Gold if Internal. Bonuses scale with Era.'
WHERE Tag = 'TXT_KEY_TRAIT_CONVERTS_SEA_BARBARIANS';

--------------------
-- Persia
--------------------
UPDATE Language_en_US
SET Text = '+1 [ICON_GOLD] Gold per turn for every 3 [ICON_CITIZEN] Citizens in the City. +1 [ICON_GOLD] Gold for [ICON_VP_SCIENTIST] Scientists, [ICON_VP_ENGINEER] Engineers, and [ICON_VP_MERCHANT] Merchants in the City. Eliminates extra [ICON_HAPPINESS_4] Unhappiness from an [ICON_OCCUPIED] Occupied City (if it is [ICON_OCCUPIED] Occupied).'
WHERE Tag = 'TXT_KEY_BUILDING_SATRAPS_COURT_HELP';

UPDATE Language_en_US
SET Text = 'The Satrap''s Court is the Persian unique building. It replaces the Courthouse. Unlike a Courthouse, this Building can be built in any City. It also provides Happiness and a substantial bonus to the City''s Gold output.'
WHERE Tag = 'TXT_KEY_BUILDING_SATRAPS_COURT_STRATEGY';

UPDATE Language_en_US
SET Text = '[ICON_GOLDEN_AGE] Golden Ages last 50% longer, and 15% of your gross [ICON_GOLD] Gold income converts into [ICON_GOLDEN_AGE] Golden Age Points every turn. During a [ICON_GOLDEN_AGE] Golden Age, Units receive +1 [ICON_MOVES] Movement and a +15% [ICON_STRENGTH] Combat Strength bonus.'
WHERE Tag = 'TXT_KEY_TRAIT_ENHANCED_GOLDEN_AGES';

UPDATE Language_en_US
SET Text = 'Unique Persian {TXT_KEY_UNIT_SPEARMAN} that is stronger, heals quicker, and has a defensive bonus.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_IMMORTAL';

--------------------
-- Poland
--------------------
UPDATE Language_en_US
SET Text = 'Unique Polish Stable replacement. Provides 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] [ICON_RES_HORSE] Horse, and grants +50% [ICON_PRODUCTION] Production and +15 XP for Mounted Melee Units. Increases the Military Unit Supply Cap from Population in the City by 10%. Internal [ICON_INTERNATIONAL_TRADE] Trade Routes from this City generate +2 [ICON_PRODUCTION] Production.[NEWLINE][NEWLINE]Nearby [ICON_RES_HORSE] Horses: +3 [ICON_PRODUCTION] Production, +3 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_SHEEP] Sheep: +3 [ICON_PRODUCTION] Production, +3 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_COW] Cattle: +3 [ICON_PRODUCTION] Production, +3 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_DUCAL_STABLE_HELP';

UPDATE Language_en_US
SET Text = 'Gain 1 [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Social Policy in the Classical Era, and in every other Era after. Gain 2 additional [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Tenets when you adopt an Ideology for the first time.'
WHERE Tag = 'TXT_KEY_TRAIT_SOLIDARITY';

UPDATE Language_en_US
SET Text = 'Unique Polish {TXT_KEY_UNIT_LANCER} that is much faster and specialized in defeating {TXT_KEY_UNITCOMBAT_MOUNTED}. Forces enemies to retreat (or take more damage if they cannot retreat) if it has more [ICON_STRENGTH] Combat Strength than its enemies. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_FORMATION_1}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_WINGED_HUSSAR';
--------------------
-- Polynesia
--------------------
UPDATE Language_en_US
SET Text = '+1 [ICON_VP_VISION] Sight when Embarked, can always Embark and move over Oceans. +2 [ICON_FOOD] Food from Fishing Boats and Atolls. Melee and Gunpowder Units can construct Fishing Boats. No [ICON_HAPPINESS_3] Unhappiness from Isolation.'
WHERE Tag = 'TXT_KEY_TRAIT_WAYFINDING';

UPDATE Language_en_US
SET Text = 'Unique Polynesian {TXT_KEY_UNIT_LONGSWORDSMAN} that does not require [ICON_RES_IRON] {TXT_KEY_RESOURCE_IRON}, is cheaper to build, and is available alternatively at [COLOR_CYAN]{TXT_KEY_TECH_CHIVALRY}[ENDCOLOR]. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_HAKA_WAR_DANCE}[ENDCOLOR], weakening nearby enemies.'
WHERE Tag = 'TXT_KEY_CIV5_POLYNESIAN_MAORI_WARRIOR_HELP';

UPDATE Language_en_US
SET Text = 'Unique Longswordsman that only Polynesia may build. The Maori Warrior is similar to a Great General; however instead of increasing the strength of friendly units, it decreases the strength of adjacent enemy units. Build plenty of these units, for their promotion stays with them as they are upgraded and they are not limited by [ICON_RES_IRON] Iron.'
WHERE Tag = 'TXT_KEY_CIV5_POLYNESIAN_MAORI_WARRIOR_STRATEGY';

UPDATE Language_en_US
SET Text = 'Near Combat Bonus Tile'
WHERE Tag = 'TXT_KEY_EUPANEL_IMPROVEMENT_NEAR';

UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] for every Moai and City adjacent to this Moai. Can only be built adjacent to water.[NEWLINE][NEWLINE]+20% [ICON_STRENGTH] Combat Strength to any nearby Polynesian Units within a 3-tile radius.'
WHERE Tag = 'TXT_KEY_BUILD_MOAI_HELP';

UPDATE Language_en_US
SET Text = 'Moai can only be built on the coast.  If built next to another Moai or City, it provides additional [ICON_CULTURE] Culture.[NEWLINE][NEWLINE]If within 3 tiles of a Moai, all Polynesian units gain +20% [ICON_STRENGTH] Combat Strength.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_MOAI_HELP';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CIV5_IMPROVEMENTS_MOAI_HELP}[NEWLINE][NEWLINE]Perhaps better known as the "Easter Island Statues", the Moai are large, humanoid figures carved mostly from volcanic rocks like tuff. 887 of these statues dot Easter Island, and are believed to have been created sometime between 1250 and 1500 AD.  Almost half still remain at the original quarry site, but the rest were transported and erected around the island''s coastline.  Easily recognizable by their large heads and elongated features, it is believed that they were individual depictions of deceased ancestors and powerful chiefs.[NEWLINE][NEWLINE]While most scholars agree on why and how the statues were created, the method of their transportation still remains a mystery.  Weighing from 9 to 86 tons a piece, they would have required an amazing feat of engineering to move from quarry to final resting place.[NEWLINE][NEWLINE]In 1994, the Moai statues were given official protection on the UNESCO World Heritage site list.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_MOAI_TEXT';

--------------------
-- Portugal
--------------------
UPDATE Language_en_US
SET Text = 'Can be constructed by Workers in owned land to provide yields or by the Nau (via [COLOR_POSITIVE_TEXT]Exotic Cargo[ENDCOLOR]) in City-State land. Provides an untradeable copy of each Luxury Resource type owned by the City-State. [ICON_INTERNATIONAL_TRADE] Trade Routes to this City-State generate bonus [ICON_FOOD] Food and [ICON_PRODUCTION] Production based on [ICON_GOLD] Gold generated from the [ICON_INTERNATIONAL_TRADE] Trade Route and relationship with the City-State (i.e. Neutral/Friend/Ally).[NEWLINE][NEWLINE]In addition to these bonuses, the Feitoria provides vision within a 2-tile radius and a +25% defense bonus. Can only be built on the coast. Cannot be built adjacent to another Feitoria or on top of resources. Contains a [COLOR_POSITIVE_TEXT]Canal[ENDCOLOR], allowing friendly [COLOR_POSITIVE_TEXT]Sea[ENDCOLOR] Units to traverse through this tile.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_FEITORIA_HELP';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CIV5_IMPROVEMENTS_FEITORIA_HELP}[NEWLINE][NEWLINE]The Feitoria - Portuguese for "factory" - were trading posts established in foreign territories during the Middle Ages. A feitoria served simultaneously as marketplace, warehouse, settlement, and staging point for exploration. Often established with a grant from the Portuguese crown, these private enterprises repaid their debt through the buying and selling of goods on behalf of the crown and through collecting taxes on trade that passed through their gates. During the 15th and 16th centuries, a chain of over 50 feitoria were built along the coasts of west and east Africa, India, Malaya, China and Japan. The feitoria allowed Portugal to dominate the Atlantic and Indian Ocean trade routes for three centuries.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_FEITORIA_TEXT';

UPDATE Language_en_US
SET Text = 'If in a City-State''s land, it will provide you one untradeable copy of every Luxury Resource type connected by this City-State, regardless of your relationship with the City-State. [ICON_INTERNATIONAL_TRADE] Trade Routes to this City-State generate bonus [ICON_PRODUCTION] Production and [ICON_FOOD] Food, scaling with your [ICON_GOLD] Gold income from the [ICON_INTERNATIONAL_TRADE] Trade Route and your relationship with the City-State (i.e. Neutral/Friend/Ally).[NEWLINE][NEWLINE]+25% [ICON_STRENGTH] Defensive Strength for any Unit stationed in this Tile. Provides vision within a 2-tile radius. Contains a [COLOR_POSITIVE_TEXT]Canal[ENDCOLOR], allowing friendly [COLOR_POSITIVE_TEXT]Sea[ENDCOLOR] Units to traverse through this tile.'
WHERE Tag = 'TXT_KEY_BUILD_FEITORIA_HELP';

UPDATE Language_en_US
SET Text = 'Unique Portuguese {TXT_KEY_UNIT_CARAVEL} that is stronger and excels at sea exploration.[NEWLINE][NEWLINE]When a Nau sells its Exotic Cargo adjacent to a City-State, earn [ICON_GOLD] Gold and XP scaling with how far it is from the [ICON_CAPITAL] Capital and if possible a Feitoria is [COLOR_POSITIVE_TEXT]automatically created[ENDCOLOR] in its territory.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_PORTUGUESE_NAU';

UPDATE Language_en_US
SET Text = 'For each active Trade Routes, receive +4 [ICON_RESEARCH] Science, [ICON_GOLD] Gold, [ICON_GREAT_ADMIRAL] Great Admiral Points ([ICON_CARGO_SHIP]), and [ICON_GREAT_GENERAL] Great General Points ([ICON_CARAVAN]), scaling with Era.'
WHERE Tag = 'TXT_KEY_TRAIT_EXTRA_TRADE';

UPDATE Language_en_US
SET Text = 'May Sell Exotic Goods when adjacent to City-State lands to gain [ICON_GOLD] Gold and XP based on distance from capital.[NEWLINE]Each Nau can perform this action twice.[NEWLINE][NEWLINE]If possible, when a Nau sells its Exotic Cargo, a Feitoria is also [COLOR_POSITIVE_TEXT]automatically created[ENDCOLOR] in its territory.'
WHERE Tag = 'TXT_KEY_PROMOTION_SELL_EXOTIC_GOODS_HELP';

UPDATE Language_en_US
SET Text = 'This unit will sell its cargo, earning [ICON_GOLD] Gold and XP. More will be earned for selling farther from your capital. This may only be done twice with this unit.'
WHERE Tag = 'TXT_KEY_MISSION_SELL_EXOTIC_GOODS_HELP';

--------------------
-- Rome
--------------------
UPDATE Language_en_US
SET Text = '+30% [ICON_STRENGTH] Combat Strength against [ICON_CITY_STATE] City-States. Conquered City-States provide rewards as if they were [COLOR_POSITIVE_TEXT]Allies[ENDCOLOR]. +15% [ICON_PRODUCTION] Production towards Buildings present in [ICON_CAPITAL] Capital.'
WHERE Tag = 'TXT_KEY_TRAIT_CAPITAL_BUILDINGS_CHEAPER';

UPDATE Language_en_US
SET Text = 'The Ballista can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BALLISTA';

UPDATE Language_en_US
SET Text = ' The Ballista is an excellent siege weapon. While still very useful against cities, its lack of penalties against land units and free Field I promotion gives it an extra punch against other units. The Ballista is still quite vulnerable to attack, so be sure to protect it with other military units. Moves at half speed in enemy territory, but has normal sight range.'
WHERE Tag = 'TXT_KEY_UNIT_ROMAN_BALLISTA_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Roman {TXT_KEY_UNIT_SWORDSMAN} that is stronger and can construct [COLOR_POSITIVE_TEXT]{TXT_KEY_CIV5_IMPROVEMENTS_FORT}[ENDCOLOR] and [COLOR_POSITIVE_TEXT]{TXT_KEY_CIV5_IMPROVEMENTS_ROAD}[ENDCOLOR]. Deals 10 damage to adjacent enemies every turn when fortified. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_COVER_1}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ROMAN_LEGION';

UPDATE Language_en_US
SET Text = 'The Legion is the Roman unique unit, replacing the Swordsman. The Legion is more powerful than the Swordsman, making it the one of the most powerful melee units of the Classical Era. The Legion can also construct roads and forts and is the only non-Worker unit able to do so.'
WHERE Tag = 'TXT_KEY_UNIT_ROMAN_LEGION_STRATEGY';

--------------------
-- Russia
--------------------
UPDATE Language_en_US
SET Text = 'Reduces the city''s damage taken from attacks by 2. +1 [ICON_PRODUCTION] Production and [ICON_GOLD] Gold to Camps, Mines, Lumbermills, and Strategic Resources. Allows the City''s [ICON_RANGE_STRENGTH] Ranged Strike to ignore Line of Sight. [ICON_SILVER_FIST] Military Units Supplied by this City''s population increased by 10%. Enemy Land Units expend all [ICON_MOVES] Movement when entering land territory owned by this City.[NEWLINE][NEWLINE][ICON_CITY_STATE] Empire Size Modifier is reduced by 5% in this City.'
WHERE Tag = 'TXT_KEY_BUILDING_KREPOST_HELP';

UPDATE Language_en_US
SET Text = 'Ostrog'
WHERE Tag = 'TXT_KEY_BUILDING_KREPOST_DESC';

UPDATE Language_en_US
SET Text = 'The Ostrog is the Russian unique building, replacing the Bastion Fort. In addition to the normal bonuses of the Bastion Fort, the Ostrog is stronger, and grants +1 Production and Gold to Camps, Mines, Lumbermills, and Strategic Resources. Increases Military Units supplied by this City''s population by 10%. It also causes enemy land units to expend 1 extra movement per tile if they move into a tile near this City. It is quite worthwhile to construct these buildings in all Russian cities, but especially in cities on the the Russian frontier.'
WHERE Tag = 'TXT_KEY_BUILDINGS_KREPOST_STRATEGY';

UPDATE Language_en_US
SET Text = 'Ostrog is a Russian term for a small fort, typically wooden and often non-permanently manned. Ostrogs were widely used during the age of Russian imperial expansion, particularly during the 18th and early 19th centuries. Ostrogs were encircled by 6 metres high palisade walls made from sharpened trunks. The name derives from the Russian word strogat, "to shave the wood". Ostrogs were smaller and exclusively military forts, compared to larger kremlins that were the cores of Russian cities. Ostrogs were often built in remote areas or within the fortification lines, such as the Great Abatis Line.'
WHERE Tag = 'TXT_KEY_BUILDINGS_KREPOST_PEDIA';

UPDATE Language_en_US
SET Text = 'All Strategic Resource deposits provide double their normal quantity. Receive +20 [ICON_RESEARCH] Science when your borders expand, scaling with Era. [ICON_CULTURE] Culture cost of acquiring new tiles reduced by 33%.'
WHERE Tag = 'TXT_KEY_TRAIT_STRATEGIC_RICHES';

UPDATE Language_en_US
SET Text = 'Unique Russian {TXT_KEY_UNIT_CAVALRY} that excels at taking out weakened and cornered enemies by having a combat bonus when fighting enemies that have already taken damage, and even more to enemies that cannot retreat.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_COSSACK';

--------------------
-- Spain
--------------------
UPDATE Language_en_US
SET Text = '+10 [ICON_GOLD] Gold and 4 [ICON_PEACE] Faith from gaining tiles, scaling with Era. [ICON_INQUISITOR] Inquisitors unlock earlier and do not cost [ICON_GOLD] Gold Maintenance. Gain an [ICON_INQUISITOR] Inquisitor whenever a City is conquered. May purchase Naval Units with [ICON_PEACE] Faith.'
WHERE Tag = 'TXT_KEY_TRAIT_SEVEN_CITIES';

UPDATE Language_en_US
SET Text = 'Reconquista'
WHERE Tag = 'TXT_KEY_TRAIT_SEVEN_CITIES_SHORT';

UPDATE Language_en_US
SET Text = 'The Tercio is a powerful Renaissance-Era Melee Unit. Use these Units to protect your fragile Ranged Units, and to capture Cities.'
WHERE Tag = 'TXT_KEY_CIV5_SPAIN_TERCIO_HELP';

UPDATE Language_en_US
SET Text = 'This Tercio represents the advent of early gunpowder (''Pike and Shot'') tactics during the Renaissance Era. Fights at double-strength against mounted units, making them an effective counter to Lancers, as well as any Knights still lingering on the battlefield.'
WHERE Tag = 'TXT_KEY_CIV5_SPAIN_TERCIO_STRATEGY';

UPDATE Language_en_US
SET Text = 'The Conquistador is a multi-purpose unit that is strong both on land and at sea. It also has the ability to found new cities, but only on a foreign continent that does not contain the Spanish Capital. When founded, cities built by Conquistadors start with 3 Citizens, claim additional territory, and automatically receive the following selection of buildings:[NEWLINE][NEWLINE][ICON_BULLET] Arena[NEWLINE][ICON_BULLET] Armory[NEWLINE][ICON_BULLET] Barracks[NEWLINE][ICON_BULLET] Council[NEWLINE][ICON_BULLET] Forge[NEWLINE][ICON_BULLET] Granary[NEWLINE][ICON_BULLET] Herbalist[NEWLINE][ICON_BULLET] Library[NEWLINE][ICON_BULLET] Lighthouse[NEWLINE][ICON_BULLET] Market[NEWLINE][ICON_BULLET] Monument[NEWLINE][ICON_BULLET] Shrine[NEWLINE][ICON_BULLET] Smokehouse[NEWLINE][ICON_BULLET] Water Mill (if applicable)[NEWLINE][ICON_BULLET] Well (if applicable)[NEWLINE][NEWLINE]Note: the bonuses for constructing buildings from the Progress and Industry policy branches do not apply to the buildings constructed by Conquistadors![NEWLINE][NEWLINE]Cities built by Conquistador are able to purchase [COLOR_POSITIVE_TEXT]Mission[ENDCOLOR],  a special religious and defensive building that can only be purchased with [ICON_PEACE] Faith.[NEWLINE][NEWLINE]The Conquistador is much stronger than the Explorer it replaces, with much higher Combat Strength, and large bonuses to Flanking and attacking Cities. Combined with its great mobility, the Conquistador is a formidable unit, able to augment or replace your existing Medieval military.'
WHERE Tag = 'TXT_KEY_CIV5_SPAIN_CONQUISTADOR_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Spanish {TXT_KEY_UNIT_EXPLORER} that specializes in conquering and settling advanced Cities overseas. Starts with the [COLOR_POSITIVE_TEXT]Overrun[ENDCOLOR] and [COLOR_POSITIVE_TEXT]Vanguard[ENDCOLOR].[NEWLINE][NEWLINE]Cities established by this Unit may purchase a [COLOR_POSITIVE_TEXT]Mission[ENDCOLOR] with [ICON_PEACE] Faith.'
WHERE Tag = 'TXT_KEY_CIV5_SPAIN_CONQUISTADOR_HELP';

--------------------
-- Shoshone
--------------------
UPDATE Language_en_US
SET Text = 'Unique Shoshone {TXT_KEY_UNIT_CAVALRY} that excels at hit-and-run attacks and pillaging. Generates [ICON_RESEARCH] Science and consume no [ICON_MOVES] Moves when pillaging. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_WITHDRAW_BEFORE_MELEE}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SHOSHONE_COMANCHE_RIDERS';

UPDATE Language_en_US
SET Text = 'The Comanche Riders replaces the Cavalry, and only the Shoshone may build it. Used judiciously, this unit can enter enemy territory to destroy infrastructure with ease and escape with only minor damage.'
WHERE Tag = 'TXT_KEY_UNIT_SHOSHONE_COMANCHE_RIDERS_STRATEGY';

UPDATE Language_en_US
SET Text = '{TXT_KEY_UNITCOMBAT_RECON} such as this one are some of the only units allowed to uncover hidden secrets in [COLOR_POSITIVE_TEXT]{TXT_KEY_ANTIQUITY_LEGEND_GOODY_HUT}[ENDCOLOR]. Be wary using it to fend off barbarians, however as the Unit is meant for exploring!'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SHOSHONE_PATHFINDER';

UPDATE Language_en_US
SET Text = 'The Pathfinder is the game''s first reconnaissance unit. It fights poorly compared to a Warrior, but has better movement and vision.'
WHERE Tag = 'TXT_KEY_UNIT_SHOSHONE_PATHFINDER_STRATEGY';

UPDATE Language_en_US
SET Text = 'Cities claim up to 8 tiles on Founding and 4 tiles on Conquest. Land Units receive a +20% [ICON_STRENGTH] Combat Strength bonus when fighting in friendly territory. All Recon Units can choose rewards from Ancient Ruins.'
WHERE Tag = 'TXT_KEY_TRAIT_GREAT_EXPANSE';

--------------------
-- Siam
--------------------
UPDATE Language_en_US
SET Text = '+2 [ICON_RESEARCH] Science from Temples and Shrines in this City, and +1 [ICON_CULTURE] Culture from Jungle and Forest tiles worked by this City.[NEWLINE][NEWLINE]Improves City [ICON_SPY] Security Level by 50%.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD] and [ICON_PRODUCTION] Distress.'
WHERE Tag = 'TXT_KEY_BUILDING_WAT_HELP';

UPDATE Language_en_US
SET Text = 'The Wat is the Siamese unique building, replacing the Constabulary. It is available much earlier than the building it replaces. The Wat increases the [ICON_CULTURE] Culture and [ICON_RESEARCH] Science of a City, increases the [ICON_RESEARCH] Science output of Temples and Shrines, and reduces spy stealing rates much more than the Constabulary. It also receives an additional Scientist Specialist, thus allowing it to produce Great Scientists more rapidly than other Civilizations.'
WHERE Tag = 'TXT_KEY_BUILDING_WAT_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Siamese {TXT_KEY_UNIT_KNIGHT} that excels at defeating {TXT_KEY_UNITCOMBAT_MOUNTED} and does not require [ICON_RES_HORSE] {TXT_KEY_RESOURCE_HORSES}. Receives a combat boost when stacked with a {TXT_KEY_UNIT_GREAT_GENERAL}. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_FEARED_ELEPHANT}[ENDCOLOR], weakening nearby enemies.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SIAMESE_WARELEPHANT';

UPDATE Language_en_US
SET Text = '[ICON_INFLUENCE] Influence with [ICON_CITY_STATE] City-States starts at 40. +100% yields from Friendly and Allied [ICON_CITY_STATE] City-States. +25% to [ICON_STRENGTH] Combat Strength of Allied [ICON_CITY_STATE] City-State [ICON_CAPITAL] Capitals. +10 Experience to Units gifted by [ICON_CITY_STATE] City-States.'
WHERE Tag = 'TXT_KEY_TRAIT_CITY_STATE_BONUSES';

--------------------
-- Songhai
--------------------
UPDATE Language_en_US
SET Text = 'Grants +1 [ICON_PRODUCTION] Production to all River tiles near the City, and +10% [ICON_PRODUCTION] Production when constructing Buildings in this City.[NEWLINE][NEWLINE]Allows [ICON_PRODUCTION] Production to be moved from this City along trade routes inside your Civilization.[NEWLINE][NEWLINE]Nearby [ICON_RES_MARBLE] Marble: +1 [ICON_PRODUCTION] Production, +1 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_STONE] Stone: +2 [ICON_PRODUCTION] Production.[NEWLINE]Nearby [ICON_RES_SALT] Salt: +2 [ICON_GOLD] Gold.[NEWLINE]Nearby [ICON_RES_JADE] Jade: +1 [ICON_PRODUCTION] Production, +1 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_MUD_PYRAMID_MOSQUE_HELP';

UPDATE Language_en_US
SET Text = 'The Tabya is a Songhai unique building, replacing the Stone Works. The Tabya greatly increases the [ICON_PRODUCTION] Production of Cities on rivers, boosts the value of [ICON_RES_STONE] Stone, [ICON_RES_MARBLE] Marble, [ICON_RES_SALT] Salt, and [ICON_RES_JADE] Jade, and boosts the production of future Buildings in the City by 10%. Also allows [ICON_PRODUCTION] Production to be moved from a City along trade routes inside your Civilization.'
WHERE Tag = 'TXT_KEY_BUILDING_MUD_PYRAMID_MOSQUE_STRATEGY';

UPDATE Language_en_US
SET Text = 'Tabya'
WHERE Tag = 'TXT_KEY_BUILDING_MUD_PYRAMID_MOSQUE_DESC';

UPDATE Language_en_US
SET Text = 'Adobe, the Spanish word for mud brick, is a natural building material made from sand, clay, water and some kind of fibrous or organic material (sticks, straw or manure) and the bricks made with adobe material using molds and dried in the sun. The Great Mosque of Djenné, in central Mali, is largest mudbrick structure in the world. It, like much Sahelian architecture, is built with a mudbrick called Banco: a recipe of mud and grain husks, fermented, and either formed into bricks or applied on surfaces as a plaster like paste in broad strokes. This plaster must be reapplied annually. The facilities where these material were created were called Tabya (Cobworks), and played an essential role in West African architecture.'
WHERE Tag = 'TXT_KEY_CIV5_BUILDING_MUD_PYRAMID_MOSQUE_PEDIA';

UPDATE Language_en_US
SET Text = 'Triple [ICON_GOLD] Gold from pillaging Encampments and Cities. Land Units gain the [COLOR_POSITIVE_TEXT]Amphibious[ENDCOLOR] Promotion, and move along Rivers as if they were Roads. Rivers create [ICON_CONNECTED] City Connections.'
WHERE Tag = 'TXT_KEY_TRAIT_AMPHIB_WARLORD';

UPDATE Language_en_US
SET Text = 'This is a Songhai unique unit, replacing the Knight. This unit gains a significant bonus when attacking cities and flanking other units. The Mandekalu Cavalry can move after attacking. Its speed makes it difficult for an enemy to build a defensive line before the Mandekalu Cavalry reaches the target.'
WHERE Tag = 'TXT_KEY_UNIT_SONGHAI_MUSLIMCAVALRY_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Songhai {TXT_KEY_UNIT_KNIGHT} that excels at raiding cities and flanking enemies. Is not penalized for attacking cities and steals gold when attacking cities. Can use enemy roads.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MUSLIM_CAVALRY';

--------------------
-- Sweden
--------------------
UPDATE Language_en_US
SET Text = 'Melee Land Units gain +20% [ICON_STRENGTH] Combat Strength [COLOR_POSITIVE_TEXT]when attacking[ENDCOLOR], and Siege Units gain +1 [ICON_MOVES] Movement. When a [ICON_GREAT_GENERAL] Great General is born, all Military Units fully heal and earn +15 XP. +15% [ICON_GREAT_GENERAL]/[ICON_GREAT_ADMIRAL] [COLOR_POSITIVE_TEXT]Leadership[ENDCOLOR] combat bonus.'
WHERE Tag = 'TXT_KEY_TRAIT_DIPLOMACY_GREAT_PEOPLE';

UPDATE Language_en_US
SET Text = 'Unique Swedish {TXT_KEY_UNIT_RIFLEMAN} specializing in long marches far from Swedish land and deals AOE damage to enemy units on kill. Starts with [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_MARCH}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SWEDISH_CAROLEAN';

UPDATE Language_en_US
SET Text = 'Caroleans are the backbone of the Industrial era Swedish army. They start with the March promotion that allows it to Heal every turn, even if it performs an action. The Carolean also deals damage to all adjacent Units after advancing from killing a Unit.'
WHERE Tag = 'TXT_KEY_UNIT_SWEDISH_CAROLEAN_STRATEGY';

UPDATE Language_en_US
SET Text = 'The Hakkapeliitta can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SWEDISH_HAKKAPELIITTA';

UPDATE Language_en_US
SET Text = 'The Hakkapeliitta is much faster and can see one hex farther than the Lancer. It pillages enemy improvements at no additional cost, so foray into enemy lands to knock out key improvements, using its improved sight and speed to minimize retaliatory attacks. Keep the unit healthy by pillaging and defeating weakened units, as it has lower [ICON_STRENGTH] Combat Strength than its contemporary Lancers.'
WHERE Tag = 'TXT_KEY_UNIT_SWEDISH_HAKKAPELIITTA_STRATEGY';

UPDATE Language_en_US
SET Text = 'Lion of the North'
WHERE Tag = 'TXT_KEY_TRAIT_DIPLOMACY_GREAT_PEOPLE_SHORT';

--------------------
-- Venice
--------------------
UPDATE Language_en_US
SET Text = 'The Great Galleass can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_VENETIAN_GALLEASS';

UPDATE Language_en_US
SET Text = 'The Great Galleass performs similarly to the Galleass, but is more resilient in battle and has a stronger ranged attack. Starts with the [COLOR_POSITIVE_TEXT]Bombardment I[ENDCOLOR] Promotion.'
WHERE Tag = 'TXT_KEY_UNIT_VENETIAN_GALLEASS_STRATEGY';

UPDATE Language_en_US
SET Text = 'The Merchant of Venice is a unique Great Merchant replacement. Aside from the normal Great Merchant abilities, the Merchant of Venice can acquire a City-State outright, bringing it under Venetian control as a puppet. The Merchant of Venice can also found a Colonia, which is a City that starts Puppeted and with the following bonuses: additional territory, additional population, a Market, and a Monument. The Merchant of Venice is expended when used in any of these ways.'
WHERE Tag = 'TXT_KEY_UNIT_VENETIAN_MERCHANT_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Venetian {TXT_KEY_UNIT_GREAT_MERCHANT} that can conduct better trade missions, purchase control of a [ICON_CITY_STATE] City-State and its units at no cost, and can found a [COLOR_POSITIVE_TEXT]Colonia[ENDCOLOR], a [ICON_PUPPET] Puppeted city with additional territory, population, a {TXT_KEY_BUILDING_MARKET}, and a {TXT_KEY_BUILDING_MONUMENT_DESC}.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_VENETIAN_MERCHANT';

UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]Cannot create Settlers or Annex Cities.[ENDCOLOR] [ICON_INTERNATIONAL_TRADE] Trade Route cap doubled, target restrictions removed. [COLOR_POSITIVE_TEXT]Free[ENDCOLOR] Merchant of Venice at [COLOR_CYAN]Trade[ENDCOLOR]. [ICON_PUPPET] Puppets have -30% Yield penalties, can use [ICON_GOLD] Gold and gain [ICON_HAPPINESS_1] Happiness like normal Cities.'
WHERE Tag = 'TXT_KEY_TRAIT_SUPER_CITY_STATE';

--------------------
-- Zulu
--------------------
UPDATE Language_en_US
SET Text = 'Grants unique Promotions to Melee and Gunpowder Units. +15 XP for all Units. Increases [ICON_SILVER_FIST] Military Unit Supply Cap by 2.[NEWLINE][NEWLINE]-1 [ICON_HAPPINESS_3] Unhappiness from [ICON_FOOD] and [ICON_PRODUCTION] Distress.'
WHERE Tag = 'TXT_KEY_BUILDING_IKANDA_HELP';

UPDATE Language_en_US
SET Text = 'In addition to the regular abilities of the Barracks which it replaces, the Ikanda grants a unique set of promotions to melee units created within the City. These include faster movement, better flanking, and greater combat strength. Only the Zulu may build it.'
WHERE Tag = 'TXT_KEY_BUILDING_IKANDA_STRATEGY';

UPDATE Language_en_US
SET Text = 'Before engaging in melee, the Impi performs a spear throw attack that can damage or kill the enemy unit.  This will weaken the defender before melee combat ensues, ensuring the Impi takes much less damage when attacking compared to its contemporary units. Attack early and often!'
WHERE Tag = 'TXT_KEY_UNIT_ZULU_IMPI_STRATEGY';

UPDATE Language_en_US
SET Text = 'Unique Zulu {TXT_KEY_UNIT_SPANISH_TERCIO} excelling at defeating {TXT_KEY_UNITCOMBAT_GUN}. Performs a first-strike ranged attack ([COLOR_POSITIVE_TEXT]at 50% Combat Strength[ENDCOLOR]) before an attack, softening enemies.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ZULU_IMPI';

UPDATE Language_en_US
SET Text = '50% maintenance for Melee and Gunpowder Units. All Units require 25% less Experience for Promotions. Your military ignores Alliance and Protection penalties and is 50% more effective at intimidating [ICON_CITY_STATE] City-States.'
WHERE Tag = 'TXT_KEY_TRAIT_BUFFALO_HORNS';
