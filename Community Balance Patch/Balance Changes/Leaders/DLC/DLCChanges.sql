-- Babylon

UPDATE Traits
SET InvestmentModifier = '-15'
WHERE Type = 'TRAIT_INGENIOUS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receive a free Great Scientist when you discover Writing. Earn Great Scientists 50% faster. [ICON_GOLD] Gold investments in Buildings reduce their [ICON_PRODUCTION] Production cost by an additional 15%.'
WHERE Tag = 'TXT_KEY_TRAIT_INGENIOUS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Units
SET PrereqTech = 'TECH_ENGINEERING'
WHERE Type = 'UNIT_BABYLONIAN_BOWMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Units
SET ObsoleteTech = 'TECH_METALLURGY'
WHERE Type = 'UNIT_BABYLONIAN_BOWMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Unit_ClassUpgrades
SET UnitClassType = 'UNITCLASS_CROSSBOWMAN'
WHERE UnitType = 'UNIT_BABYLONIAN_BOWMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Civilization_UnitClassOverrides
Set UnitClassType = 'UNITCLASS_COMPOSITE_BOWMAN'
WHERE UnitType = 'UNIT_BABYLONIAN_BOWMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET Class = 'UNITCLASS_COMPOSITE_BOWMAN'
WHERE Type = 'UNIT_BABYLONIAN_BOWMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET Combat = '8'
WHERE Type = 'UNIT_BABYLONIAN_BOWMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Units
SET RangedCombat = '15'
WHERE Type = 'UNIT_BABYLONIAN_BOWMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Unit_FreePromotions
SET PromotionType = 'PROMOTION_INDIRECT_FIRE'
WHERE UnitType = 'UNIT_BABYLONIAN_BOWMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'This Babylonian Unique Unit replaces the Composite Bowman. The Bowman is stronger and can better withstand melee attacks than the standard Composite Bowman. Starts with Indirect Fire promotion.'
WHERE Tag = 'TXT_KEY_CIV5_BABYLON_BOWMAN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Bowman is the Babylonian Unique Unit, replacing the Composite Bowman. This Unit is defensively stronger than the Composite Bowman, allowing placement in the front line. The Bowman''s improved combat prowess helps ameliorate any concern that it may be quickly overrun.'
WHERE Tag = 'TXT_KEY_CIV5_BABYLON_BOWMAN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Babylon 

UPDATE Buildings
SET ExtraCityHitPoints = '75'
WHERE Type = 'BUILDING_WALLS_OF_BABYLON' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Buildings
SET Defense = '900'
WHERE Type = 'BUILDING_WALLS_OF_BABYLON' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Buildings
SET SpecialistType = 'SPECIALIST_SCIENTIST'
WHERE Type = 'BUILDING_WALLS_OF_BABYLON' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Buildings
SET SpecialistCount = '1'
WHERE Type = 'BUILDING_WALLS_OF_BABYLON' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Walls of Babylon increase the damage output of all the city''s ranged attacks and increase the city''s defense. +1 [ICON_RESEARCH] Science per 5 [ICON_CITIZEN] Citizens in the City.'
WHERE Tag = 'TXT_KEY_CIV5_BABYLON_WALLS_INFO' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Walls of Babylon are a Babylonian Unique Building, replacing the standard city Walls. The Walls of Babylon increase Defense Strength in a city by 9 and Hit Points by 100, both values much higher than standard Walls. The Walls of Babylon also provide a Scientist specialist slot and Science per Citizen in the City.'
WHERE Tag = 'TXT_KEY_CIV5_BABYLON_WALLS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Mongols
UPDATE Traits
SET BullyAnnex = '1'
WHERE Type = 'TRAIT_TERROR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Mounted ranged units gain +2 [ICON_MOVES] Movement and ignore Zone of Control. Forcefully annex City-States instead of exacting Heavy Tribute, and receive [ICON_GOLDEN_AGE] Golden Age Points when you conquer or annex a City-State.'
WHERE Tag = 'TXT_KEY_TRAIT_TERROR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Khan is a Mongolian Unique Great Person, replacing the standard Great General.  The same combat bonuses apply, but the Khan moves 5 points per turn and heals adjacent units for additional HP per turn. This beefed up General ensures that his cavalry units will always be in a battle ready state.'
WHERE Tag = 'TXT_KEY_CIV5_MONGOLIA_KHAN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_en_US (
Tag, Text)
SELECT 'TXT_KEY_BALANCE_KNOWN_CS_BULLY_ANNEXED_KNOWN', '{1_Bully} has forced {2_CS} to surrender by bullying them!'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_en_US (
Tag, Text)
SELECT 'TXT_KEY_BALANCE_KNOWN_CS_BULLY_ANNEXED_KNOWN_SUMMARY', '{1_CS} has surrendered!'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_en_US (
Tag, Text)
SELECT 'TXT_KEY_BALANCE_UNKNOWN_CS_BULLY_ANNEXED_KNOWN', '{1_Bully} has forced an unmet City-State to surrender by bullying them!'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_en_US (
Tag, Text)
SELECT 'TXT_KEY_BALANCE_UNKNOWN_CS_BULLY_ANNEXED_KNOWN_SUMMARY', '{1_Bully} has forced a City-State to surrender!'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_en_US (
Tag, Text)
SELECT 'TXT_KEY_BALANCE_KNOWN_CS_BULLY_ANNEXED_UNKNOWN', 'An unmet player has forced {1_CS} to surrender by bullying them!'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_en_US (
Tag, Text)
SELECT 'TXT_KEY_BALANCE_KNOWN_CS_BULLY_ANNEXED_UNKNOWN_SUMMARY', '{1_CS} has surrendered!'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_en_US (
Tag, Text)
SELECT 'TXT_KEY_BALANCE_ANNEXED_CS', 'You forced {1_CS} to surrender through intimidation!'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_en_US (
Tag, Text)
SELECT 'TXT_KEY_POPUP_MINOR_BULLY_UNIT_AMOUNT_ANNEX', 'Forcefully Annex City-State'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_en_US (
Tag, Text)
SELECT 'TXT_KEY_POP_CSTATE_BULLY_UNIT_TT_ANNEX', 'If this City-State is more [COLOR_POSITIVE_TEXT]afraid[ENDCOLOR] of you than they are [COLOR_WARNING_TEXT]resilient[ENDCOLOR], you can annex the City-State. Doing so will net you a large amount of [ICON_GOLDEN_AGE] Golden Age points. {1_FearLevel}{2_FactorDetails}'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_en_US (
Tag, Text)
SELECT 'TXT_KEY_BALANCE_ANNEXED_CS_SUMMARY', 'You intimidated {1_CS}!'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

DELETE FROM Civilization_UnitClassOverrides
WHERE UnitType = 'UNIT_MONGOLIAN_KESHIK' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Keshik -- Break Off, Make basic for Horse Archer

UPDATE Units
SET Class = 'UNITCLASS_HORSE_ARCHER'
WHERE Type = 'UNIT_MONGOLIAN_KESHIK' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET PrereqTech = 'TECH_MATHEMATICS'
WHERE Type = 'UNIT_MONGOLIAN_KESHIK' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET Moves = '4'
WHERE Type = 'UNIT_MONGOLIAN_KESHIK' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET UnitFlagAtlas = 'MOUNTED_XBOW_FLAG_ATLAS'
WHERE Type = 'UNIT_MONGOLIAN_KESHIK' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET ObsoleteTech = 'TECH_PHYSICS'
WHERE Type = 'UNIT_MONGOLIAN_KESHIK' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Unit_Flavors
SET Flavor = '5'
WHERE UnitType = 'UNIT_MONGOLIAN_KESHIK' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM Unit_FreePromotions
WHERE UnitType = 'UNIT_MONGOLIAN_KESHIK'  AND PromotionType = 'PROMOTION_SPAWN_GENERALS_I' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM Unit_FreePromotions
WHERE UnitType = 'UNIT_MONGOLIAN_KESHIK'  AND PromotionType = 'PROMOTION_GAIN_EXPERIENCE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
SELECT 'UNIT_MONGOLIAN_KESHIK', 'PROMOTION_CITY_PENALTY'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Skirmisher'
WHERE Tag = 'TXT_KEY_UNIT_MONGOL_KESHIK' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'In the 12th century, nomadic tribes came boiling out of Central Asia, conquering most of Asia, Eastern Europe and a large portion of the Middle East within a century. Their primary weapon was their incomparable mounted bowmen. The Mongols were one such nomadic tribe, and their children were almost literally "raised in the saddle." Riding on their small but hearty steppe ponies, the lightly-armed and armored Mongol Keshiks, a type of skirmisher, could cover an astonishing amount of territory in a day, far outpacing any infantry or even the heavier European cavalry.[NEWLINE][NEWLINE]In battle the Mongol Keshiks would shoot from horseback with deadly accuracy, disrupting and demoralizing the enemy formations. Once the enemy was suitably weakened, the Mongol heavy cavalry units would charge into the wavering foe to deliver the coup de grace. When facing armored European knights the Mongols would simply shoot their horses, then ignore or destroy the unhorsed men as they wished.'
WHERE Tag = 'TXT_KEY_CIV5_MONGOLIA_KESHIK_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );
		
UPDATE Language_en_US
SET Text = 'Mounted Bowmen possess a strong ranged attack and an increased movement rate, allowing them to perform hit and run attacks.'
WHERE Tag = 'TXT_KEY_CIV5_MONGOLIA_KESHIK_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Skirmishers are fast ranged units, deadly on open terrain. Unlike the Chariot before them, they can move through rough terrain without a movement penalty. As a mounted unit, the Skirmisher is vulnerable to units equipped with spears.'
WHERE Tag = 'TXT_KEY_CIV5_MONGOLIA_KESHIK_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET GoodyHutUpgradeUnitClass = 'UNITCLASS_MOUNTED_BOWMAN'
WHERE Type = 'UNIT_MONGOLIAN_KESHIK' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Unit_ClassUpgrades
SET UnitClassType = 'UNITCLASS_MOUNTED_BOWMAN'
WHERE UnitType = 'UNIT_MONGOLIAN_KESHIK' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Polynesia -- Boost Range of Moai
UPDATE Traits
Set NearbyImprovementBonusRange = '4'
WHERE Type = 'TRAIT_WAYFINDING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Builds
Set PrereqTech = 'TECH_THE_WHEEL'
WHERE Type = 'BUILD_MOAI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Improvements
SET CultureAdjacentSameType = '0'
WHERE Type = 'IMPROVEMENT_MOAI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET NaturalWonderYieldModifier = '50'
WHERE Type = 'TRAIT_WAYFINDING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET CombatBoostNearNaturalWonder = '1'
WHERE Type = 'TRAIT_WAYFINDING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 Sight when embarked. Can embark and move over Oceans immediately. +50% tile yields from Natural Wonders, and +10% [ICON_STRENGTH] Combat Strength bonus if within 4 tiles of an owned Moai or Natural Wonder.'
WHERE Tag = 'TXT_KEY_TRAIT_WAYFINDING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Near Combat Bonus Tile'
WHERE Tag = 'TXT_KEY_EUPANEL_IMPROVEMENT_NEAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Move Maori Warrior 
UPDATE Units
SET GoodyHutUpgradeUnitClass = 'UNITCLASS_MUSKETMAN'
WHERE Type = 'UNIT_POLYNESIAN_MAORI_WARRIOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Unit_ClassUpgrades
SET UnitClassType = 'UNITCLASS_MUSKETMAN'
WHERE UnitType = 'UNIT_POLYNESIAN_MAORI_WARRIOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Civilization_UnitClassOverrides
Set UnitClassType = 'UNITCLASS_LONGSWORDSMAN'
WHERE UnitType = 'UNIT_POLYNESIAN_MAORI_WARRIOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET PrereqTech = 'TECH_METAL_CASTING'
WHERE Type = 'UNIT_POLYNESIAN_MAORI_WARRIOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET ObsoleteTech = 'TECH_RIFLING'
WHERE Type = 'UNIT_POLYNESIAN_MAORI_WARRIOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET Class = 'UNITCLASS_LONGSWORDSMAN'
WHERE Type = 'UNIT_POLYNESIAN_MAORI_WARRIOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET Combat = '19'
WHERE Type = 'UNIT_POLYNESIAN_MAORI_WARRIOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Unit_Flavors
SET Flavor = '15'
WHERE UnitType = 'UNIT_POLYNESIAN_MAORI_WARRIOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Medieval Era unit that strikes fear into nearby enemies, making them less effective in combat. Does not require [ICON_RES_IRON] Iron, unlike the Longswordsman it replaces, and is available earlier. Only Polynesia may build it.'
WHERE Tag = 'TXT_KEY_CIV5_POLYNESIAN_MAORI_WARRIOR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Maori Warrior is similar to a Great General; however instead of increasing the strength of friendly units, it decreases the strength of adjacent enemy units. Build plenty of these units, for their promotion stays with them as they are upgraded.'
WHERE Tag = 'TXT_KEY_CIV5_POLYNESIAN_MAORI_WARRIOR_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Spain
DELETE FROM Units
WHERE Type = 'UNIT_SPANISH_TERCIO' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );



DELETE FROM Civilization_UnitClassOverrides
WHERE UnitType = 'UNIT_SPANISH_TERCIO' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
Set Reconquista = '1'
WHERE Type = 'TRAIT_SEVEN_CITIES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
Set NoSpread = '1'
WHERE Type = 'TRAIT_SEVEN_CITIES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
Set NaturalWonderFirstFinderGold = '0'
WHERE Type = 'TRAIT_SEVEN_CITIES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
Set NaturalWonderSubsequentFinderGold = '0'
WHERE Type = 'TRAIT_SEVEN_CITIES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
Set NaturalWonderYieldModifier = '0'
WHERE Type = 'TRAIT_SEVEN_CITIES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Religions other than your founded or majority Religion cannot spread to owned Cities or allied City-States. When you gain a new City after your [ICON_CAPITAL] Capital, its [ICON_CITIZEN] Citizens are converted to your Religion and you receive [ICON_PEACE] Faith.'
WHERE Tag = 'TXT_KEY_TRAIT_SEVEN_CITIES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reconquista'
WHERE Tag = 'TXT_KEY_TRAIT_SEVEN_CITIES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Inca
UPDATE Traits
Set ImprovementMaintenanceModifier = '0'
WHERE Type = 'TRAIT_GREAT_ANDEAN_ROAD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
Set NoHillsImprovementMaintenance = '0'
WHERE Type = 'TRAIT_GREAT_ANDEAN_ROAD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
Set MountainPass = '1'
WHERE Type = 'TRAIT_GREAT_ANDEAN_ROAD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Units ignore terrain costs when on Hills and may cross Mountains. Cities, Roads and Railroads may be built on Mountains.'
WHERE Tag = 'TXT_KEY_TRAIT_GREAT_ANDEAN_ROAD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'This Incan Unique Unit replaces the Archer. The Slinger is less powerful than the Archer, and is even more fragile if subjected to a melee attack. However it possesses a promotion that gives it a good chance to withdraw to the rear before an enemy melee attack can occur, and can attack twice in one turn.'
WHERE Tag = 'TXT_KEY_CIV5_INCA_SLINGER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Slinger is an Ancient Era ranged infantry Unit that can strike foes from afar. This Incan Unique Unit can withdraw before most melee attacks, and can attack twice: use it to harass your foes. However the Slinger can be easily defeated if the enemy pins it against obstructions or chases it with fast units.'
WHERE Tag = 'TXT_KEY_CIV5_INCA_SLINGER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Terrace Farm can only be built on hills and does not need a source of fresh water to do so. If built next to a mountain it will provide additional [ICON_FOOD] Food (1 per adjacent Mountain). +1 [ICON_FOOD] Food per adjacent Terrace Farm, and all adjacent Farms gain +1 [ICON_FOOD] Food.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_TERRACE_FARM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Denmark -- Unique National Epic (Jelling Stones) -- Replace Ski Infantry
DELETE FROM Units
WHERE Type = 'UNIT_DANISH_SKI_INFANTRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );



DELETE FROM Civilization_UnitClassOverrides
WHERE UnitType = 'UNIT_DANISH_SKI_INFANTRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET ObsoleteTech = 'TECH_RIFLING'
WHERE Type = 'UNIT_DANISH_BERSERKER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
SELECT 'UNIT_DANISH_BERSERKER' , 'PROMOTION_CHARGE'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Embarked units have +1 Movement [ICON_MOVES] and pay just 1 movement point to move from sea to land. All Melee units pillage without movement cost and plunder [ICON_GOLD] Gold when they assault a City.'
WHERE Tag = 'TXT_KEY_TRAIT_VIKING_FURY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Berserker is the Danish Unique Unit, replacing the Longswordsman. This Unit has +1 Movement [ICON_MOVES] compared to the Longswordsman and possesses the Amphibious and Charge promotions, allowing it to attack onto land from a Coast tile with no penalty and punish wounded units. Available after researching Metal Casting instead of Steel.'
WHERE Tag = 'TXT_KEY_CIV5_DENMARK_BERSERKER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Korea -- Replace Turtle Ship with UB

UPDATE Units
SET ObsoleteTech = 'TECH_METALLURGY'
WHERE Type = 'UNIT_KOREAN_HWACHA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

DELETE FROM Civilization_UnitClassOverrides
WHERE UnitType = 'UNIT_KOREAN_TURTLE_SHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO UnitClasses (Type, Description, DefaultUnit)
SELECT 'UNITCLASS_KOREAN_TURTLE_SHIP', 'TXT_KEY_UNIT_KOREAN_TURTLE_SHIP', 'UNIT_KOREAN_TURTLE_SHIP'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Turtle Ship can only be gifted by Minor Civilizations.'
WHERE Tag = 'TXT_KEY_CIV5_KOREA_TURTLESHIP_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Turtle Ship has a more powerful attack than the Caravel, and is extremely difficult to destroy.  However it may not enter deep ocean hexes outside of the city borders.'
WHERE Tag = 'TXT_KEY_CIV5_KOREA_TURTLESHIP_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+2 [ICON_RESEARCH] science from all Specialists and Great Person tile improvements. Receive a tech boost each time a scientific building/Wonder is built in your [ICON_CAPITAL] Capital. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_TRAIT_SCHOLARS_JADE_HALL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET Combat = '30'
WHERE Type = 'UNIT_KOREAN_TURTLE_SHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET Cost = '-1'
WHERE Type = 'UNIT_KOREAN_TURTLE_SHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Units
SET MinorCivGift = '1'
WHERE Type = 'UNIT_KOREAN_TURTLE_SHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET Class = 'UNITCLASS_KOREAN_TURTLE_SHIP'
WHERE Type = 'UNIT_KOREAN_TURTLE_SHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET PrereqTech = 'TECH_COMPASS'
WHERE Type = 'UNIT_KOREAN_TURTLE_SHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET ObsoleteTech = 'TECH_INDUSTRIALIZATION'
WHERE Type = 'UNIT_KOREAN_TURTLE_SHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
