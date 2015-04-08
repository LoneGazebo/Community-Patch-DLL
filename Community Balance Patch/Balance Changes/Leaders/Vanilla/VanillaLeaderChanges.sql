-- America -- Resort
DELETE FROM Units
WHERE Type = 'UNIT_AMERICAN_B17' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM Civilization_UnitClassOverrides
WHERE UnitType = 'UNIT_AMERICAN_B17' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET BuyOwnedTiles = '1'
WHERE Type = 'TRAIT_RIVER_EXPANSION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'All land military units have +1 sight. 50% discount when purchasing tiles. Can acquire tiles already owned by other Civilizations, though at a much higher cost than normal.'
WHERE Tag = 'TXT_KEY_TRAIT_RIVER_EXPANSION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Fixed to make sense with ability.
UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]You stole their territory![ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_CULTURE_BOMB' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Arabia -- Faith Instead of Luxury Doubling from Bazaar -- Units move faster in Desert -- Boosted Religion over Trade route spread.
UPDATE Buildings
SET ExtraLuxuries = '0'
WHERE Type = 'BUILDING_BAZAAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_GOLD] Gold for every 4 [ICON_CITIZEN] Citizens in the City. +1 [ICON_GOLD] Gold from [ICON_RES_SPICES] Spices, [ICON_RES_SUGAR] Sugar and [ICON_RES_COCOA] Cocoa. Each source of [ICON_RES_OIL] Oil and each Oasis provides +2 [ICON_GOLD] Gold.[NEWLINE][NEWLINE]Trade routes other players make to a city with a Bazaar will generate an extra 1 [ICON_GOLD] Gold for the city owner and the trade route owner gains an additional 1 [ICON_GOLD] Gold for the trade route.'
WHERE Tag = 'TXT_KEY_BUILDING_BAZAAR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'All land military units move twice as fast in the Desert. Caravans gain 50% extended range, and your trade routes spread religion much more effectively. [ICON_OIL] Oil resources are doubled.'
WHERE Tag = 'TXT_KEY_TRAIT_LAND_TRADE_GOLD2' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET TradeReligionModifier = '150'
WHERE Type = 'TRAIT_LAND_TRADE_GOLD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Aztec -- Set Floating Garden to +1 Food per 4 citizens instead of +15%
UPDATE Language_en_US
SET Text = '+1 [ICON_FOOD] Food for every 2 [ICON_CITIZEN] Citizens in the City. Each worked Lake tile provides +2 [ICON_FOOD] Food.[NEWLINE][NEWLINE]City must be built next to a River or Lake.'
WHERE Tag = 'TXT_KEY_BUILDING_FLOATING_GARDENS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Buildings
SET PrereqTech = 'TECH_CONSTRUCTION'
WHERE Type = 'BUILDING_FLOATING_GARDENS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Building_YieldChanges
SET Yield = '3'
WHERE BuildingType = 'BUILDING_FLOATING_GARDENS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET ObsoleteTech = 'TECH_CIVIL_SERVICE'
WHERE Type = 'UNIT_AZTEC_JAGUAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Askia -- Boost Mosque
UPDATE Building_YieldChanges
SET Yield = '3'
WHERE BuildingType = 'BUILDING_MUD_PYRAMID_MOSQUE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Requires no [ICON_GOLD] Gold maintenance. Reduces [ICON_HAPPINESS_3] Religious Unrest, and grants +1 [ICON_CULTURE] Culture from [ICON_RES_INCENSE] Incense and [ICON_RES_WINE] Wine resources nearby.'
WHERE Tag = 'TXT_KEY_BUILDING_MUD_PYRAMID_MOSQUE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET RiverTradeRoad = 'true'
WHERE Type = 'TRAIT_AMPHIB_WARLORD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET FasterAlongRiver = 'true'
WHERE Type = 'TRAIT_AMPHIB_WARLORD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Triple [ICON_GOLD] Gold from pillaging Encampments and Cities. Land units gain the War Canoe and Amphibious promotions, move faster along Rivers. Upon researching The Wheel, Rivers can be used to create [ICON_CONNECTED] City Connections.'
WHERE Tag = 'TXT_KEY_TRAIT_AMPHIB_WARLORD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Catherine -- Move Krepost, give bonus
UPDATE Buildings
SET PrereqTech = 'TECH_ARCHERY'
WHERE Type = 'BUILDING_KREPOST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Buildings
SET ExtraCityHitPoints = '25'
WHERE Type = 'BUILDING_KREPOST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Buildings
SET Defense = '200'
WHERE Type = 'BUILDING_KREPOST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+15 XP for all Units. [ICON_CULTURE] Culture and [ICON_GOLD] Gold costs of acquiring new tiles reduced by 25% in this city. Increases city Defense Strength by 2 and Hit Points by 25, and reduces [ICON_HAPPINESS_3] Disorder slightly.'
WHERE Tag = 'TXT_KEY_BUILDING_KREPOST_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Krepost is the Russian unique building, replacing the barracks. The Krepost provides 15 experience points for all military units constructed in its city. In addition, the Krepost increases cultural border growth by 25% and city defense strength by 2. It is quite worthwhile to construct these useful buildings in all Russian cities, but especially in cities on the the Russian frontier.'
WHERE Tag = 'TXT_KEY_BUILDINGS_KREPOST_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Adjusted startbias out of tundra

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_RUSSIA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Darius -- Adjust Satrap
UPDATE Language_en_US
SET Text = 'City must have a Market. +1 [ICON_GOLD] Gold for every 4 [ICON_CITIZEN] Citizens in the City. Trade routes other players make to a city with a Satraps Court will generate an extra 1 [ICON_GOLD] Gold for the city owner and the trade route owner gains an additional 1 [ICON_GOLD] Gold for the trade route.'
WHERE Tag = 'TXT_KEY_BUILDING_SATRAPS_COURT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Buildings
SET Happiness = '1'
WHERE Type = 'BUILDING_SATRAPS_COURT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Darius -- Stronger Units during Golden Ages
UPDATE Traits
SET GoldenAgeCombatModifier = '25'
WHERE Type = 'TRAIT_ENHANCED_GOLDEN_AGES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[ICON_GOLDEN_AGE] Golden Ages last 50% longer. During a Golden Age, units receive +1 [ICON_MOVES] Movement and a +25% [ICON_STRENGTH] Combat Strength bonus.'
WHERE Tag = 'TXT_KEY_TRAIT_ENHANCED_GOLDEN_AGES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Egypt -- Buff Burial Tomb, +1 Production in all cities.

UPDATE Language_en_US
SET Text = 'Requires no [ICON_GOLD] Gold maintenance. Reduces [ICON_HAPPINESS_3] Religious Unrest, and grants +1 [ICON_CULTURE] Culture from [ICON_RES_INCENSE] Incense and [ICON_RES_WINE] Wine resources nearby. Receive 25 [ICON_PEACE] Faith upon construction, and 5 [ICON_GOLDEN_AGE] Golden Age points whenever you are victorious in battle. Bonuses scale with era.[NEWLINE][NEWLINE]Should this city be captured, the amount of [ICON_GOLD] Gold plundered by the enemy is doubled.'
WHERE Tag = 'TXT_KEY_BUILDING_BURIAL_TOMB_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Burial Tomb is a Classical-era building which increases your output of [ICON_PEACE] Faith, and costs no maintenance for the city. However, if the city is captured, the capturing civilization will gain twice as much gold as would be the case in a city without a Burial Tomb.  It replaces the Temple.'
WHERE Tag = 'TXT_KEY_BUILDING_BURIAL_TOMB_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A fast Ranged Unit. Only the Egyptians may build it. This Unit does not require [ICON_RES_HORSE] Horses to be built, unlike the Chariot Archer which it replaces. It is also stronger than the Chariot Archer, and receives the [COLOR_POSITIVE_TEXT]Forced Labor[ENDCOLOR] Promotion for free.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_EGYPTIAN_WAR_CHARIOT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );


UPDATE Buildings
SET Happiness = '0'
WHERE Type = 'BUILDING_BURIAL_TOMB' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Building_ResourceYieldChanges (
BuildingType, ResourceType, YieldType, Yield)
SELECT 'BUILDING_BURIAL_TOMB', 'RESOURCE_INCENSE', 'YIELD_FAITH', '1'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Building_ResourceYieldChanges (
BuildingType, ResourceType, YieldType, Yield)
SELECT 'BUILDING_BURIAL_TOMB', 'RESOURCE_WINE', 'YIELD_FAITH', '1'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+20% [ICON_PRODUCTION] Production towards Wonder construction. +1 [ICON_PRODUCTION] Production in every City.'
WHERE Tag = 'TXT_KEY_TRAIT_WONDER_BUILDER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Elizabeth -- Replace Longbowman with a unique Factory
DELETE FROM Units
WHERE Type = 'UNIT_ENGLISH_LONGBOWMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM Civilization_UnitClassOverrides
WHERE UnitType = 'UNIT_ENGLISH_LONGBOWMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Extra Spies
UPDATE Language_en_US
SET Text = '+2 [ICON_MOVES] Movement for all naval and embarked units. Receive one or more additional Spies based on the number of City-States.'
WHERE Tag = 'TXT_KEY_TRAIT_OCEAN_MOVEMENT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Germany -- New UA

-- Bismarck -- Hanse Yield Per Pop
UPDATE Language_en_US
SET Text = 'Unique German Bank replacement. +5% [ICON_PRODUCTION] Production for each Trade Route your civilization has with a City-State, and +1 [ICON_GOLD] Gold for every 4 [ICON_CITIZEN] Citizens in the City. Trade routes other players make to a city with a Hanse will generate an extra 1 [ICON_GOLD] Gold for the city owner and the trade route owner gains an additional 1 [ICON_GOLD] Gold for the trade route.'
WHERE Tag = 'TXT_KEY_BUILDING_HANSE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receive +1 [ICON_GOLD] Gold and +1 [ICON_CULTURE] Culture in all owned cities for every City-State you are allied with. For every 3 City-State alliances, receive 1 additional Delegate in the World Congress.'
WHERE Tag = 'TXT_KEY_TRAIT_CONVERTS_LAND_BARBARIANS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Realpolitik'
WHERE Tag = 'TXT_KEY_TRAIT_CONVERTS_LAND_BARBARIANS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET VotePerXCSAlliance = '3'
WHERE Type = 'TRAIT_CONVERTS_LAND_BARBARIANS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET LandBarbarianConversionPercent = '0'
WHERE Type = 'TRAIT_CONVERTS_LAND_BARBARIANS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET LandUnitMaintenanceModifier = '0'
WHERE Type = 'TRAIT_CONVERTS_LAND_BARBARIANS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );


-- Greece -- +2 Culture per city -- Odeon
DELETE FROM Units
WHERE Type = 'UNIT_GREEK_COMPANIONCAVALRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM Civilization_UnitClassOverrides
WHERE UnitType = 'UNIT_GREEK_COMPANIONCAVALRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'City-State [ICON_INFLUENCE] Influence degrades at half and recovers at twice the normal rate. +1 [ICON_CULTURE] Culture in every City.'
WHERE Tag = 'TXT_KEY_TRAIT_CITY_STATE_FRIENDSHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Ancient Era Unit which specializes in defeating Mounted Units. Only the Greeks may build it. This Unit has a higher [ICON_STRENGTH] Combat Strength than the Spearman which it replaces, and produces Great Generals very quickly.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_HOPLITE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- India -- Indus Sanitation (replace Mughal Fort) -- Unhappiness from Poverty and Illiteracy reduced by 25% 
DELETE FROM Buildings
WHERE Type = 'BUILDING_MUGHAL_FORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM Civilization_BuildingClassOverrides
WHERE BuildingType = 'BUILDING_MUGHAL_FORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM Building_ClassesNeededInCity
WHERE BuildingType = 'BUILDING_MUGHAL_FORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM Building_Flavors
WHERE BuildingType = 'BUILDING_MUGHAL_FORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM Building_YieldChanges
WHERE BuildingType = 'BUILDING_MUGHAL_FORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM Building_TechEnhancedYieldChanges
WHERE BuildingType = 'BUILDING_MUGHAL_FORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET PopulationUnhappinessModifier = '0'
WHERE Type = 'TRAIT_POPULATION_GROWTH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET CityUnhappinessModifier = '0'
WHERE Type = 'TRAIT_POPULATION_GROWTH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET PopulationBoostReligion = 'true'
WHERE Type = 'TRAIT_POPULATION_GROWTH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET IsNoReligiousStrife = 'true'
WHERE Type = 'TRAIT_POPULATION_GROWTH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Cannot build Missionaries or receive [ICON_HAPPINESS_3] Unhappiness from Religious Strife. Pressure for your majority Religion increases in owned cities based on the number of [ICON_CITIZEN] Followers in them. Starts with enough [ICON_PEACE] Faith for a Pantheon.'
WHERE Tag = 'TXT_KEY_TRAIT_POPULATION_GROWTH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Font of Dharma'
WHERE Tag = 'TXT_KEY_TRAIT_POPULATION_GROWTH_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Rename War Elephant

UPDATE Language_en_US
SET Text = 'Naga-Malla'
WHERE Tag = 'TXT_KEY_UNIT_INDIAN_WARELEPHANT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A terrifying sight on the battlefield, often as dangerous to its own side as it was to the enemy, the Naga-Malla (the Hindu term for Fighting Elephant) could be considered the first weapon of mass destruction. Indians used elephants in war until the advent of gunpowder rendered them obsolete. In pre-gunpowder battle the war elephant served two primary purposes. First, their scent absolutely terrified horses, rendering enemy cavalry useless. Second, they could break through even the strongest line of infantry, crushing a wall of spears that no horse could ever surmount. Elephants were incredibly difficult to kill and history records them surviving sixty or more arrows. The primary problem with elephants was their tendency to go berserk with pain or rage, at which point they became impossible for their rider to control. Elephant riders often carried a spike and mallet that they could use to kill the animals if they attacked their own forces.'
WHERE Tag = 'TXT_KEY_CIV5_ANTIQUITY_INDIANWARELEPHANT_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'A [ponderous, mounted Ranged Unit highly effective on flat ground. Only the Indians may build it. This Unit has a higher [ICON_STRENGTH] Combat and Ranged Strength than the Horse Archer, which it replaces, but a lower Movement speed. Does not require [ICON_RES_HORSE] Horses.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_INDIAN_WAR_ELEPHANT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Naga-Malla (Fighting Elephant) is the Indian unique unit, replacing the Horse Archer. It is more powerful (if slower) than the Horse Archer and can move through any terrain without penalty. Does not require Horses. A mounted unit, the Naga-Malla is quite vulnerable to the Spearman.'
WHERE Tag = 'TXT_KEY_UNIT_INDIAN_WARELEPHANT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Iroquois -- All units receive Woodsman promotion
UPDATE Language_en_US
SET Text = 'Units move through Forest and Jungle in friendly territory as if it is Road. These tiles can be used to establish [ICON_CONNECTED] City Connections upon researching The Wheel. Land military units start with the Woodsman promotion.'
WHERE Tag = 'TXT_KEY_TRAIT_IGNORE_TERRAIN_IN_FOREST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Building_YieldChanges
SET Yield = '3'
WHERE BuildingType = 'BUILDING_LONGHOUSE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Buildings
SET PrereqTech = 'TECH_STEEL'
WHERE Type = 'BUILDING_LONGHOUSE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET PrereqTech = 'TECH_IRON_WORKING'
WHERE Type = 'UNIT_IROQUOIAN_MOHAWKWARRIOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET Combat = '17'
WHERE Type = 'UNIT_IROQUOIAN_MOHAWKWARRIOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Japan -- Dojo (culture from strategic resources) -- Boost UA
DELETE FROM Units
WHERE Type = 'UNIT_JAPANESE_ZERO' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM Civilization_UnitClassOverrides
WHERE UnitType = 'UNIT_JAPANESE_ZERO' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The [ICON_STRENGTH] Combat Strength of your units increases as they take damage. +1 [ICON_CULTURE] Culture from each Fishing Boat and +2 [ICON_CULTURE] Culture from each Atoll.'
WHERE Tag = 'TXT_KEY_TRAIT_FIGHT_WELL_DAMAGED' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Unit_ClassUpgrades
SET UnitClassType = 'UNITCLASS_RIFLEMAN'
WHERE UnitType = 'UNIT_JAPANESE_SAMURAI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET GoodyHutUpgradeUnitClass = 'UNITCLASS_RIFLEMAN'
WHERE Type = 'UNIT_JAPANESE_SAMURAI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Napoleon -- Plunders artwork!
UPDATE Traits
SET FreeGreatWorkOnConquest = 'true'
WHERE Type = 'TRAIT_ENHANCED_CULTURE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Theming bonuses doubled in [ICON_CAPITAL] Capital. Plunder [ICON_GREAT_WORK] Great Works of Art when you conquer a City. Receive a temporary [ICON_CULTURE] Culture boost if you cannot plunder.'
WHERE Tag = 'TXT_KEY_TRAIT_ENHANCED_CULTURE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Riches of Conquest'
WHERE Tag = 'TXT_KEY_TRAIT_ENHANCED_CULTURE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Language_en_US (Text, Tag)
SELECT 'Napoleon stole {1_Num} [ICON_GREAT_WORK] Great Work(s) of Art from you when he conquered {2_City}!' , 'TXT_KEY_ART_PLUNDERED'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Language_en_US (Text, Tag)
SELECT 'Art stolen!' , 'TXT_KEY_ART_PLUNDERED_SUMMARY'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Language_en_US (Text, Tag)
SELECT 'You plundered {1_Num} [ICON_GREAT_WORK] Great Work(s) of Art from the conquest of {2_City}!' , 'TXT_KEY_ART_STOLEN'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Language_en_US (Text, Tag)
SELECT 'Art plundered!' , 'TXT_KEY_ART_STOLEN_SUMMARY'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Language_en_US (Text, Tag)
SELECT 'The conquest of {2_City} has doubled your [ICON_CULTURE] Culture output for the next {1_Num} turn(s)!' , 'TXT_KEY_CULTURE_BOOST_ART'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Language_en_US (Text, Tag)
SELECT 'Culture boost!' , 'TXT_KEY_CULTURE_BOOST_ART_SUMMARY'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] from a temporary Cultural Boost (Turns left: {2_TurnsLeft}).'
WHERE Tag = 'TXT_KEY_TP_CULTURE_FROM_BONUS_TURNS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Chateau
UPDATE Language_en_US
SET Text = 'A Chateau must be built adjacent to a Luxury resource.  It may not be adjacent to another Chateau. It provides the same +50% defense bonus as a Fort. Must be built in French territory.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_CHATEAU_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Ottomans -- Receive a free Caravansary in all cities, and Trade Routes generate Golden Age points when finishing a Trade Route. 

-- Remove Sipahi, replace with Siege Workshop
DELETE FROM Units
WHERE Type = 'UNIT_OTTOMAN_SIPAHI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM Civilization_UnitClassOverrides
WHERE UnitType = 'UNIT_OTTOMAN_SIPAHI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET NavalUnitMaintenanceModifier = '0'
WHERE Type = 'TRAIT_CONVERTS_SEA_BARBARIANS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET FreeBuilding = 'BUILDING_CARAVANSARY'
WHERE Type = 'TRAIT_CONVERTS_SEA_BARBARIANS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM Trait_FreePromotionUnitCombats
WHERE TraitType = 'TRAIT_CONVERTS_SEA_BARBARIANS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Tanzimat'
WHERE Tag = 'TXT_KEY_TRAIT_CONVERTS_SEA_BARBARIANS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Completing an [ICON_INTERNATIONAL_TRADE] International Trade Route grants 50 [ICON_RESEARCH] Science, [ICON_PRODUCTION] Production, and [ICON_FOOD] Food to the origin City. Receive a free Caravansary in every city.'
WHERE Tag = 'TXT_KEY_TRAIT_CONVERTS_SEA_BARBARIANS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET ObsoleteTech = 'TECH_REPLACEABLE_PARTS'
WHERE Type = 'UNIT_OTTOMAN_JANISSARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Rome -- Unique Monument (Triumphal Arch) -- Receive Culture boost when you conquer a City.
DELETE FROM Units
WHERE Type = 'UNIT_ROMAN_BALLISTA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM Civilization_UnitClassOverrides
WHERE UnitType = 'UNIT_ROMAN_BALLISTA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Siam -- adjust Wat
UPDATE Building_FeatureYieldChanges
SET Yield = '2'
WHERE BuildingType = 'BUILDING_WAT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+2 [ICON_RESEARCH] Science from Jungle and Forest tiles worked by this City. Gain 66% of the [ICON_RESEARCH] Science output of the City as an instant boost to your current Research when a [ICON_CITIZEN] Citizen is born in this City. Reduces [ICON_HAPPINESS_3] Illiteracy.[NEWLINE][NEWLINE] Allows Archaeologists to be built in this city.[NEWLINE][NEWLINE]City must have a Library.'
WHERE Tag = 'TXT_KEY_BUILDING_WAT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Buildings
SET SpecialistCount = '1'
WHERE Type = 'BUILDING_WAT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Adjust Help for Elephant
UPDATE Language_en_US
SET Text = 'Powerful Medieval Mounted Unit, weak to Pikemen. Only the Siamese may build it. This Unit receives a bonus against other mounted Units, earns the Feared Elepehant Promotion for free, and has a higher [ICON_STRENGTH] Combat Strength than the Knight, which it replaces.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SIAMESE_WARELEPHANT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Boost UA

UPDATE Traits
SET CityStateBonusModifier = '75'
WHERE Type = 'TRAIT_CITY_STATE_BONUSES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[ICON_FOOD] Food, [ICON_CULTURE] Culture, and [ICON_PEACE] Faith from friendly City-States increased by 75%.'
WHERE Tag = 'TXT_KEY_TRAIT_CITY_STATE_BONUSES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- China


UPDATE Language_en_US
SET Text = '+1 [ICON_RESEARCH] for every 3 [ICON_CITIZEN] Citizens in the City. Reduces [ICON_HAPPINESS_3] Illiteracy. +2[ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILDING_PAPER_MAKER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Building_YieldChangesPerPop
SET Yield = '33'
WHERE BuildingType = 'BUILDING_PAPER_MAKER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );
 
UPDATE Buildings
SET SpecialistCount = '1'
WHERE Type = 'BUILDING_PAPER_MAKER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Buildings
SET SpecialistType = 'SPECIALIST_SCIENTIST'
WHERE Type = 'BUILDING_PAPER_MAKER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

