-- Unique Unit Changes
-- Includes removed units and unique promotions/abilities applied to UUs. All information related to combat strength, cost, and tech unlocks with full unit class


---------------------------
-- America
---------------------------
-- Change B17 (now Freedom unlock), Minutman now ranged Renaissance unit
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_AMERICAN_B17';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_B17', 'TXT_KEY_CIV5_INDUSTRIAL_B17_HEADING', 'UNIT_AMERICAN_B17');

UPDATE Units SET Class = 'UNITCLASS_B17', PolicyType = 'POLICY_THEIR_FINEST_HOUR'  WHERE Type = 'UNIT_AMERICAN_B17';

UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_GATLINGGUN' WHERE Type = 'UNIT_AMERICAN_MINUTEMAN';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_GATLINGGUN' WHERE UnitType = 'UNIT_AMERICAN_MINUTEMAN';

---------------------------
-- Arabia
---------------------------
-- Camel Archer now a Mounted Ranged unit
UPDATE Units SET Class = 'UNITCLASS_MOUNTED_BOWMAN', GoodyHutUpgradeUnitClass = 'UNITCLASS_CUIRASSIER' WHERE Type = 'UNIT_ARABIAN_CAMELARCHER';

UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_MOUNTED_BOWMAN' WHERE UnitType = 'UNIT_ARABIAN_CAMELARCHER';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_CUIRASSIER' WHERE UnitType = 'UNIT_ARABIAN_CAMELARCHER';

DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_ARABIAN_CAMELARCHER';

---------------------------
-- Assyria
---------------------------
-- Siege Tower now pseudo-GG

INSERT INTO UnitClasses
	(Type, Description, DefaultUnit, MaxPlayerInstances)
VALUES
	('UNITCLASS_ASSYRIAN_SIEGE_TOWER', 'TXT_KEY_UNIT_ASSYRIAN_SIEGE_TOWER', 'NONE', 2);

UPDATE Units SET CityAttackOnly = '1', Combat = '0', Cost = '100', DefaultUnitAI = 'UNITAI_CITY_SPECIAL', CombatClass = NULL, Moves = '4', Class = 'UNITCLASS_ASSYRIAN_SIEGE_TOWER', PrereqTech = 'TECH_ARCHERY', ObsoleteTech = NULL, Pillage = '0', GoodyHutUpgradeUnitClass = NULL  WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER';

DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_ASSYRIAN_SIEGE_TOWER';
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_ASSYRIA', 'UNITCLASS_ASSYRIAN_SIEGE_TOWER', 'UNIT_ASSYRIAN_SIEGE_TOWER');

UPDATE Defines Set Value = '40' WHERE Name = 'SAPPED_CITY_ATTACK_MODIFIER';

DELETE FROM Unit_ClassUpgrades
WHERE UnitType = 'UNIT_ASSYRIAN_SIEGE_TOWER';


---------------------------
-- Austria
---------------------------
-- Hussar now a Cuirassier unit
UPDATE Units SET Class = 'UNITCLASS_CUIRASSIER', GoodyHutUpgradeUnitClass = 'UNITCLASS_CAVALRY' WHERE Type = 'UNIT_AUSTRIAN_HUSSAR';

UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_CUIRASSIER' WHERE UnitType = 'UNIT_AUSTRIAN_HUSSAR';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_CAVALRY' WHERE UnitType = 'UNIT_AUSTRIAN_HUSSAR';

---------------------------
-- Aztec
---------------------------
-- Update unit
UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_SPEARMAN' WHERE UnitType = 'UNIT_AZTEC_JAGUAR';

---------------------------
-- Babylon
---------------------------
-- Bowman now a UU composite bowman
UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_COMPOSITE_BOWMAN' WHERE UnitType = 'UNIT_BABYLONIAN_BOWMAN';

UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_ARCHER' WHERE UnitType = 'UNIT_BABYLONIAN_BOWMAN';

UPDATE Units SET Class = 'UNITCLASS_ARCHER', GoodyHutUpgradeUnitClass = 'UNITCLASS_COMPOSITE_BOWMAN' WHERE Type = 'UNIT_BABYLONIAN_BOWMAN';

---------------------------
-- Brazil
---------------------------
-- Added Bandeirantes
INSERT INTO Civilization_UnitClassOverrides (CivilizationType, UnitClassType, UnitType) VALUES
('CIVILIZATION_BRAZIL', 'UNITCLASS_EXPLORER', 'UNIT_BANDEIRANTES');

INSERT INTO Unit_ClassUpgrades
	(UnitType, UnitClassType)
VALUES
	('UNIT_BANDEIRANTES', 'UNITCLASS_COMMANDO');

INSERT INTO Unit_Builds (UnitType, BuildType) VALUES
('UNIT_BANDEIRANTES', 'BUILD_FORT'),
('UNIT_BANDEIRANTES', 'BUILD_MINE'),
('UNIT_BANDEIRANTES', 'BUILD_BRAZILWOOD_CAMP'),
('UNIT_BANDEIRANTES', 'BUILD_QUARRY');

-- Removed Pracinha, now a Minor Civ Gift
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_BRAZILIAN_PRACINHA';
INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_BRAZILIAN_PRACINHA', 'TXT_KEY_UNIT_BRAZILIAN_PRACINHA', 'UNIT_BRAZILIAN_PRACINHA');

UPDATE Units SET Class = 'UNITCLASS_BRAZILIAN_PRACINHA', MinorCivGift = '1', Cost = '-1', FaithCost = '-1' WHERE Type = 'UNIT_BRAZILIAN_PRACINHA';

---------------------------
-- Byzantines
---------------------------
-- Removed Dromon

DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_BYZANTINE_DROMON';
INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_DROMON', 'TXT_KEY_UNIT_BYZANTIUM_DROMON', 'UNIT_BYZANTINE_DROMON');

UPDATE Units SET Class = 'UNITCLASS_DROMON', MinorCivGift = '1', Cost = '-1', GoodyHutUpgradeUnitClass = 'UNITCLASS_GALLEASS' WHERE Type = 'UNIT_BYZANTINE_DROMON';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_GALLEASS' WHERE UnitType = 'UNIT_BYZANTINE_DROMON';

UPDATE Units SET Class = 'UNITCLASS_KNIGHT', GoodyHutUpgradeUnitClass = 'UNITCLASS_LANCER'  WHERE Type = 'UNIT_BYZANTINE_CATAPHRACT';

UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_KNIGHT' WHERE UnitType = 'UNIT_BYZANTINE_CATAPHRACT';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_LANCER' WHERE UnitType = 'UNIT_BYZANTINE_CATAPHRACT';

---------------------------
-- Carthage
---------------------------
-- Elephant now a generic unit
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT';

DELETE FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_ELEPHANT_RIDER', 'TXT_KEY_UNIT_ELEPHANT_RIDER', 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT');

UPDATE Units SET Class = 'UNITCLASS_ELEPHANT_RIDER', Description = 'TXT_KEY_UNIT_ELEPHANT_RIDER', Civilopedia = 'TXT_KEY_UNIT_ELEPHANT_RIDER_TEXT', Strategy = 'TXT_KEY_UNIT_ELEPHANT_RIDER_STRATEGY', Help = 'TXT_KEY_UNIT_ELEPHANT_RIDER_HELP', ResourceType = 'RESOURCE_IVORY' WHERE Type = 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT';

INSERT INTO Unit_ClassUpgrades
	(UnitType, UnitClassType)
VALUES
	('UNIT_CARTHAGINIAN_FOREST_ELEPHANT', 'UNITCLASS_KNIGHT');

INSERT INTO Unit_BuildingClassRequireds
	(UnitType, BuildingClassType)
VALUES
	('UNIT_CARTHAGINIAN_FOREST_ELEPHANT', 'BUILDINGCLASS_BARRACKS');

---------------------------
-- Celts
---------------------------
-- Buffed Pictish Warrior, moved back starting tech

UPDATE Unit_YieldFromKills SET Yield = '200' WHERE UnitType = 'UNIT_CELT_PICTISH_WARRIOR';

---------------------------
-- China
---------------------------
--CKN upgrades into a Musketman
UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_MUSKETMAN' WHERE Type = 'UNIT_CHINESE_CHUKONU';
UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_MUSKETMAN' WHERE UnitType = 'UNIT_CHINESE_CHUKONU';
---------------------------
-- Denmark
---------------------------
-- Removed ski infantry; buffed berserker, now an early pikeman UU
DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_DANISH_BERSERKER';

UPDATE Civilization_UnitClassOverrides Set UnitClassType = 'UNITCLASS_PIKEMAN' WHERE UnitType = 'UNIT_DANISH_BERSERKER';

UPDATE Units SET Class = 'UNITCLASS_PIKEMAN', GoodyHutUpgradeUnitClass = 'UNITCLASS_TERCIO' WHERE Type = 'UNIT_DANISH_BERSERKER';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_TERCIO' WHERE UnitType = 'UNIT_DANISH_BERSERKER';

DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_DANISH_SKI_INFANTRY';
INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_SKI_INFANTRY', 'TXT_KEY_UNIT_DANISH_SKI_INFANTRY', 'UNIT_DANISH_SKI_INFANTRY');

UPDATE Units SET Class = 'UNITCLASS_SKI_INFANTRY', MinorCivGift = '1', Cost = '-1', GoodyHutUpgradeUnitClass = 'UNITCLASS_GREAT_WAR_INFANTRY' WHERE Type = 'UNIT_DANISH_SKI_INFANTRY';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_GREAT_WAR_INFANTRY' WHERE UnitType = 'UNIT_DANISH_SKI_INFANTRY';

-- Free Moves on Pillage Promotion integrated into Viking Promotion
DELETE FROM Trait_FreePromotionUnitCombats WHERE TraitType = 'TRAIT_VIKING_FURY';
INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
VALUES
	('TRAIT_VIKING_FURY', 'UNITCOMBAT_MELEE', 'PROMOTION_VIKING'),
	('TRAIT_VIKING_FURY', 'UNITCOMBAT_MOUNTED', 'PROMOTION_VIKING'),
	('TRAIT_VIKING_FURY', 'UNITCOMBAT_RECON', 'PROMOTION_VIKING'),
	('TRAIT_VIKING_FURY', 'UNITCOMBAT_ARMOR', 'PROMOTION_VIKING'),
	('TRAIT_VIKING_FURY', 'UNITCOMBAT_GUN', 'PROMOTION_VIKING'),
	('TRAIT_VIKING_FURY', 'UNITCOMBAT_NAVALMELEE', 'PROMOTION_LONGBOAT');

---------------------------
-- England
---------------------------
-- Removed  Longbowmanm SoTL - weaker, but double attack.
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_ENGLISH_LONGBOWMAN';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_LONGBOWMAN', 'TXT_KEY_UNIT_ENGLISH_LONGBOWMAN', 'UNIT_ENGLISH_LONGBOWMAN');

UPDATE Units SET MinorCivGift = '1', Cost = '-1', Class = 'UNITCLASS_LONGBOWMAN', GoodyHutUpgradeUnitClass = 'UNITCLASS_MUSKETMAN' WHERE Type = 'UNIT_ENGLISH_LONGBOWMAN';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_MUSKETMAN' WHERE UnitType = 'UNIT_ENGLISH_LONGBOWMAN';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_CRUISER' WHERE UnitType = 'UNIT_ENGLISH_SHIPOFTHELINE';

---------------------------
-- Egypt
---------------------------
-- Buff War Chariot, tweak to fit into new mounted ranged line
UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_HORSE_ARCHER' WHERE Type = 'UNIT_EGYPTIAN_WARCHARIOT';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_HORSE_ARCHER' WHERE UnitType = 'UNIT_EGYPTIAN_WARCHARIOT';

---------------------------
-- Ethiopia
---------------------------
-- Buffed Mehal Sefari

---------------------------
-- France
---------------------------
-- Buffed Musketeer

UPDATE Units SET Class = 'UNITCLASS_TERCIO' WHERE Type = 'UNIT_FRENCH_MUSKETEER';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_RIFLEMAN' WHERE UnitType = 'UNIT_FRENCH_MUSKETEER';

UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_TERCIO' WHERE UnitType = 'UNIT_FRENCH_MUSKETEER';


---------------------------
-- Germany
---------------------------
-- Remove Panzer, Landsknecht Stuff
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_GERMAN_PANZER';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_PANZER', 'TXT_KEY_UNIT_GERMAN_PANZER', 'UNIT_GERMAN_PANZER');

UPDATE Units SET Class = 'UNITCLASS_PANZER', PolicyType = 'POLICY_PATRIOTIC_WAR', Moves = '5', UnitArtInfo = 'ART_DEF_UNIT_TANK', PortraitIndex = '13', UnitFlagIconOffset = '57'  WHERE Type = 'UNIT_GERMAN_PANZER';

UPDATE Units SET UnitArtInfo = 'ART_DEF_UNIT_U_GERMAN_PANZER', UnitFlagIconOffset = '58', PortraitIndex = '39' WHERE Type = 'UNIT_TANK';

INSERT INTO	Civilization_UnitClassOverrides
			(CivilizationType,			UnitClassType, 		UnitType)
VALUES		('CIVILIZATION_GERMANY',	'UNITCLASS_TERCIO',	'UNIT_GERMAN_LANDSKNECHT');

UPDATE Units SET
Class = 'UNITCLASS_TERCIO',
GoodyHutUpgradeUnitClass = 'UNITCLASS_RIFLEMAN',
PurchaseOnly = 0,
PolicyType = NULL,
ExtraMaintenanceCost = 0,
RequiresFaithPurchaseEnabled = 1
WHERE Type = 'UNIT_GERMAN_LANDSKNECHT';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_RIFLEMAN' WHERE UnitType = 'UNIT_GERMAN_LANDSKNECHT';

---------------------------
-- Greece
---------------------------
-- Removed Companion Cavalry, buffed Hoplite
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_GREEK_COMPANIONCAVALRY';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_COMPANIONCAVALRY', 'TXT_KEY_UNIT_GREEK_COMPANIONCAVALRY', 'UNIT_GREEK_COMPANIONCAVALRY');

UPDATE Units SET MinorCivGift = '1', Cost = '-1', Class = 'UNITCLASS_COMPANIONCAVALRY' WHERE Type = 'UNIT_GREEK_COMPANIONCAVALRY';

---------------------------
-- Huns
---------------------------
-- Removed Battering Ram, tweaked Horse Archer to fit into new mounted ranged line
UPDATE Units SET Class = 'UNITCLASS_HORSE_ARCHER', GoodyHutUpgradeUnitClass = 'UNITCLASS_MOUNTED_BOWMAN' WHERE Type = 'UNIT_HUN_HORSE_ARCHER';

UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_HORSE_ARCHER' WHERE CivilizationType = 'CIVILIZATION_HUNS' AND UnitClassType = 'UNITCLASS_CHARIOT_ARCHER';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_MOUNTED_BOWMAN' WHERE UnitType = 'UNIT_HUN_HORSE_ARCHER';

DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_HUN_BATTERING_RAM';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_HUNNIC_BATTERING_RAM', 'TXT_KEY_UNIT_HUN_BATTERING_RAM', 'UNIT_HUN_BATTERING_RAM');

UPDATE Units SET Cost = '-1', MinorCivGift = '1', Class = 'UNITCLASS_HUNNIC_BATTERING_RAM', GoodyHutUpgradeUnitClass = 'UNITCLASS_TERCIO' WHERE Type = 'UNIT_HUN_BATTERING_RAM';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_TERCIO' WHERE UnitType = 'UNIT_HUN_BATTERING_RAM';


INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
VALUES
	('TRAIT_RAZE_AND_HORSES', 'UNITCOMBAT_MOUNTED', 'PROMOTION_COERCION'),
	('TRAIT_RAZE_AND_HORSES', 'UNITCOMBAT_ARMOR', 'PROMOTION_COERCION');
---------------------------
-- Inca
---------------------------
-- Slinger
UPDATE Units SET
	Class 			= 'UNITCLASS_VP_SLINGER',
	PrereqTech 		= 'TECH_AGRICULTURE', 
	ObsoleteTech 	= 'TECH_CURRENCY',
	Range 			= 2,
	GoodyHutUpgradeUnitClass = 'UNITCLASS_ARCHER'
WHERE Type = 'UNIT_INCAN_SLINGER';

UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_VP_SLINGER' WHERE UnitType = 'UNIT_INCAN_SLINGER';
UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_ARCHER' WHERE UnitType = 'UNIT_INCAN_SLINGER';

---------------------------
-- India
---------------------------
-- Rename War Elephant, make a Cuirassier UU

UPDATE Units SET Class = 'UNITCLASS_CUIRASSIER', ObsoleteTech = 'TECH_COMBUSTION', Moves = '3', GoodyHutUpgradeUnitClass = 'UNITCLASS_CAVALRY' WHERE Type = 'UNIT_INDIAN_WARELEPHANT';

UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_CUIRASSIER' WHERE UnitType = 'UNIT_INDIAN_WARELEPHANT';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_CAVALRY' WHERE UnitType = 'UNIT_INDIAN_WARELEPHANT';

DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_INDIAN_WARELEPHANT';

INSERT INTO Trait_NoTrain
	(TraitType, UnitClassType)
VALUES
	('TRAIT_POPULATION_GROWTH', 'UNITCLASS_MISSIONARY');

---------------------------
-- Indonesia
---------------------------
-- Buff Kris Swordsman

---------------------------
-- Iroquois
---------------------------
INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
VALUES
	('TRAIT_IGNORE_TERRAIN_IN_FOREST', 'UNITCOMBAT_MELEE', 'PROMOTION_WOODSMAN'),
	('TRAIT_IGNORE_TERRAIN_IN_FOREST', 'UNITCOMBAT_ARCHER', 'PROMOTION_WOODSMAN'),
	('TRAIT_IGNORE_TERRAIN_IN_FOREST', 'UNITCOMBAT_RECON', 'PROMOTION_WOODSMAN'),
	('TRAIT_IGNORE_TERRAIN_IN_FOREST', 'UNITCOMBAT_MOUNTED', 'PROMOTION_WOODSMAN'),
	('TRAIT_IGNORE_TERRAIN_IN_FOREST', 'UNITCOMBAT_SIEGE', 'PROMOTION_WOODSMAN'),
	('TRAIT_IGNORE_TERRAIN_IN_FOREST', 'UNITCOMBAT_GUN', 'PROMOTION_WOODSMAN'),
	('TRAIT_IGNORE_TERRAIN_IN_FOREST', 'UNITCOMBAT_ARMOR', 'PROMOTION_WOODSMAN');

---------------------------
-- Japan
---------------------------
-- Buff Samurai, remove Zero
UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_TERCIO' WHERE Type = 'UNIT_JAPANESE_SAMURAI';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_TERCIO' WHERE UnitType = 'UNIT_JAPANESE_SAMURAI';

DELETE FROM Unit_Builds WHERE UnitType = 'UNIT_JAPANESE_SAMURAI';

DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_JAPANESE_ZERO';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_ZERO', 'TXT_KEY_UNIT_JAPANESE_ZERO', 'UNIT_JAPANESE_ZERO');

UPDATE Units SET Class = 'UNITCLASS_ZERO', PolicyType = 'POLICY_MILITARISM' WHERE Type = 'UNIT_JAPANESE_ZERO';

---------------------------
-- Korea
---------------------------
-- Remove Turtle Ship, wxtend life of Hwacha

DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_KOREAN_TURTLE_SHIP';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_KOREAN_TURTLE_SHIP', 'TXT_KEY_UNIT_KOREAN_TURTLE_SHIP', 'UNIT_KOREAN_TURTLE_SHIP');

UPDATE Units SET Cost = '-1', MinorCivGift = '1', Class = 'UNITCLASS_KOREAN_TURTLE_SHIP' WHERE Type = 'UNIT_KOREAN_TURTLE_SHIP';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_PRIVATEER' WHERE UnitType = 'UNIT_KOREAN_TURTLE_SHIP';

---------------------------
-- Maya
---------------------------
-- Atlatlist now a UU comp bowman
UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_CROSSBOWMAN', Class = 'UNITCLASS_COMPOSITE_BOWMAN' WHERE Type = 'UNIT_MAYAN_ATLATLIST';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_CROSSBOWMAN' WHERE UnitType = 'UNIT_MAYAN_ATLATLIST';

UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_COMPOSITE_BOWMAN' WHERE UnitType = 'UNIT_MAYAN_ATLATLIST';

---------------------------
-- Mongolia
---------------------------
-- Remove Keshik, make base of Classical mounted ranged line
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_MONGOLIAN_KESHIK';

INSERT INTO UnitClasses
	(Type, Description, DefaultUnit)
VALUES
	('UNITCLASS_HORSE_ARCHER', 'TXT_KEY_UNIT_HUN_HORSE_ARCHER', 'UNIT_MONGOLIAN_KESHIK');

UPDATE Units SET Class = 'UNITCLASS_HORSE_ARCHER', Moves = '4', UnitFlagAtlas = 'MOUNTED_XBOW_FLAG_ATLAS', GoodyHutUpgradeUnitClass = 'UNITCLASS_MOUNTED_BOWMAN' WHERE Type = 'UNIT_MONGOLIAN_KESHIK';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_MOUNTED_BOWMAN' WHERE UnitType = 'UNIT_MONGOLIAN_KESHIK';

INSERT INTO UnitPromotions
	(Type, Description, Help, Sound, OrderPriority, PortraitIndex, IconAtlas, PediaType, RangedFlankAttack, FlankAttackModifier, CannotBeChosen, PediaEntry)
VALUES
	('PROMOTION_MONGOL_TERROR', 'TXT_KEY_PROMOTION_MONGOL_TERROR', 'TXT_KEY_PROMOTION_MONGOL_TERROR_HELP', 'AS2D_IF_LEVELUP', 99, 1, 'EXPANSION2_PROMOTION_ATLAS', 'PEDIA_ATTRIBUTES', 1, 10, 1, 'TXT_KEY_PROMOTION_MONGOL_TERROR');

INSERT INTO Trait_FreePromotionUnitClass
	(TraitType, UnitClassType, PromotionType)
VALUES
	('TRAIT_TERROR', 'UNITCLASS_CHARIOT_ARCHER', 'PROMOTION_MONGOL_TERROR'),
	('TRAIT_TERROR', 'UNITCLASS_HORSE_ARCHER', 'PROMOTION_MONGOL_TERROR'),
	('TRAIT_TERROR', 'UNITCLASS_MOUNTED_BOWMAN', 'PROMOTION_MONGOL_TERROR'),
	('TRAIT_TERROR', 'UNITCLASS_CUIRASSIER', 'PROMOTION_MONGOL_TERROR'),
	('TRAIT_TERROR', 'UNITCLASS_CAVALRY', 'PROMOTION_MONGOL_TERROR'),
	('TRAIT_TERROR', 'UNITCLASS_ANTI_TANK_GUN', 'PROMOTION_MONGOL_TERROR'),
	('TRAIT_TERROR', 'UNITCLASS_HELICOPTER_GUNSHIP', 'PROMOTION_MONGOL_TERROR');

---------------------------
-- Morocco
---------------------------

---------------------------
-- Netherlands
---------------------------
-- Adjust Beggar
UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_IRONCLAD' WHERE UnitType = 'UNIT_DUTCH_SEA_BEGGAR';

---------------------------
-- Ottomans
---------------------------
-- Remove Sipahi, extend life of Janissary

DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_OTTOMAN_SIPAHI';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_SIPAHI', 'TXT_KEY_UNIT_OTTOMAN_SIPAHI', 'UNIT_OTTOMAN_SIPAHI');

UPDATE Units SET MinorCivGift = '1', Cost = '-1', Class = 'UNITCLASS_SIPAHI', GoodyHutUpgradeUnitClass = 'UNITCLASS_WWI_TANK', IgnoreBuildingDefense = '0' WHERE Type = 'UNIT_OTTOMAN_SIPAHI';


UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_WWI_TANK' WHERE UnitType = 'UNIT_OTTOMAN_SIPAHI';

UPDATE Units SET Class = 'UNITCLASS_MUSKETMAN', GoodyHutUpgradeUnitClass = 'UNITCLASS_GATLINGGUN' WHERE Type = 'UNIT_OTTOMAN_JANISSARY';

UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_MUSKETMAN' WHERE UnitType = 'UNIT_OTTOMAN_JANISSARY';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_GATLINGGUN' WHERE UnitType = 'UNIT_OTTOMAN_JANISSARY';

---------------------------
-- Persia
---------------------------
-- Immortal gains extra defense

---------------------------
-- Poland
---------------------------
-- Adjust Winged Hussar
UPDATE Units SET IgnoreBuildingDefense = '0', GoodyHutUpgradeUnitClass = 'UNITCLASS_WWI_TANK' WHERE Type = 'UNIT_POLISH_WINGED_HUSSAR';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_WWI_TANK' WHERE UnitType = 'UNIT_POLISH_WINGED_HUSSAR';


---------------------------
-- Polynesia
---------------------------
-- Maori Warrior now a longswordsman UU
UPDATE Units SET Class = 'UNITCLASS_LONGSWORDSMAN', GoodyHutUpgradeUnitClass = 'UNITCLASS_TERCIO' WHERE Type = 'UNIT_POLYNESIAN_MAORI_WARRIOR';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_TERCIO' WHERE UnitType = 'UNIT_POLYNESIAN_MAORI_WARRIOR';

UPDATE Civilization_UnitClassOverrides Set UnitClassType = 'UNITCLASS_LONGSWORDSMAN' WHERE UnitType = 'UNIT_POLYNESIAN_MAORI_WARRIOR';

-- Polynesian Melee Units can build Fishing Boats (updated text that no longer says "consumes unit", can also change the time for construction here)
INSERT INTO Builds
	(Type,						Time,	Description,	HotKey,	HotKeyPriority,	OrderPriority,	AltDown,	Cost,	CostIncreasePerImprovement,	Kill,	Water,	CanBeEmbarked,	PrereqTech,	ImprovementType,	EntityEvent,	IconIndex,	IconAtlas,	ShowInPedia,	ShowInTechTree,	ObsoleteTech,	IsFreeBestDomainUnit,	CultureBoost)
SELECT
	'BUILD_POLYNESIAN_BOATS',	NULL,	Description,	HotKey,	HotKeyPriority,	OrderPriority,	1,			Cost,	CostIncreasePerImprovement, 0,		1,		1,				PrereqTech,	ImprovementType,	EntityEvent,	IconIndex,	IconAtlas,	0,				0,				ObsoleteTech,	IsFreeBestDomainUnit,	CultureBoost
FROM Builds WHERE Type = 'BUILD_FISHING_BOATS';

  -- collect all melee unit classes into table
CREATE TEMPORARY TABLE temp_uniquemeleeclasses (UnitClassType text UNIQUE ON CONFLICT IGNORE);

INSERT INTO temp_uniquemeleeclasses
SELECT
	Class
FROM Units WHERE CombatClass = 'UNITCOMBAT_MELEE' OR CombatClass = 'UNITCOMBAT_GUN';

INSERT INTO Trait_BuildsUnitClasses
	(TraitType,			UnitClassType,	BuildType)
SELECT
	'TRAIT_WAYFINDING',	UnitClassType, 	'BUILD_POLYNESIAN_BOATS'
From temp_uniquemeleeclasses;

DROP TABLE temp_uniquemeleeclasses;
  -- if a unit is added with a combat class of melee/gunpowder, add it
CREATE TRIGGER VP_PolynesiaCompatibility_Build_Insert
AFTER INSERT ON Units
FOR EACH ROW
WHEN (
		(NEW.CombatClass = 'UNITCOMBAT_MELEE' OR NEW.CombatClass = 'UNITCOMBAT_GUN')
		AND NOT EXISTS (SELECT * FROM Trait_BuildsUnitClasses
			WHERE TraitType = 'TRAIT_WAYFINDING' AND UnitClassType = NEW.Class AND BuildType = 'BUILD_POLYNESIAN_BOATS')
	)
BEGIN
	INSERT INTO Trait_BuildsUnitClasses
			(TraitType, UnitClassType, BuildType)
	SELECT	'TRAIT_WAYFINDING', NEW.Class, 'BUILD_POLYNESIAN_BOATS';
END;
  -- if a unit's class or combat class changes, and its new combat class is melee/gunpowder, add it
CREATE TRIGGER VP_PolynesiaCompatibility_Build_UpdateTo
AFTER UPDATE OF Class,CombatClass ON Units
FOR EACH ROW
WHEN (
		(NEW.CombatClass = 'UNITCOMBAT_MELEE' OR NEW.CombatClass = 'UNITCOMBAT_GUN')
		AND NOT EXISTS (SELECT * FROM Trait_BuildsUnitClasses
			WHERE TraitType = 'TRAIT_WAYFINDING' AND UnitClassType = NEW.Class AND BuildType = 'BUILD_POLYNESIAN_BOATS')
	)
BEGIN
	INSERT INTO Trait_BuildsUnitClasses
			(TraitType, UnitClassType, BuildType)
	SELECT	'TRAIT_WAYFINDING', NEW.Class, 'BUILD_POLYNESIAN_BOATS';
END;
  -- if there are no units left of a given class and melee/gunpowder combat class, remove it
CREATE TRIGGER VP_PolynesiaCompatibility_Build_UpdateAway
AFTER UPDATE OF Class,CombatClass ON Units
FOR EACH ROW 
WHEN (
		(OLD.CombatClass = 'UNITCOMBAT_MELEE' OR OLD.CombatClass = 'UNITCOMBAT_GUN') AND (NEW.CombatClass IS NULL OR (NEW.CombatClass <> 'UNITCOMBAT_MELEE' AND NEW.CombatClass <> 'UNITCOMBAT_GUN'))
		AND EXISTS (SELECT * FROM Trait_BuildsUnitClasses
			WHERE TraitType = 'TRAIT_WAYFINDING' AND UnitClassType = OLD.Class AND BuildType = 'BUILD_POLYNESIAN_BOATS')
		AND NOT EXISTS (SELECT * FROM Units
			WHERE Class = OLD.Class AND (CombatClass = 'UNITCOMBAT_MELEE' OR CombatClass = 'UNITCOMBAT_GUN'))
	)
BEGIN
	DELETE FROM Trait_BuildsUnitClasses
	WHERE UnitClassType = OLD.Class AND TraitType = 'TRAIT_WAYFINDING' AND BuildType = 'BUILD_POLYNESIAN_BOATS';
END;

---------------------------
--Portugal
---------------------------
-- Nau buffed a little, grants a feitoria on use of exotic goods

UPDATE Units SET NumExoticGoods = '2', DefaultUnitAI = 'UNITAI_ATTACK_SEA' WHERE Type = 'UNIT_PORTUGUESE_NAU';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_PRIVATEER' WHERE UnitType = 'UNIT_PORTUGUESE_NAU';

---------------------------
-- Rome
---------------------------
-- Removed Ballista

DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_ROMAN_BALLISTA';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_BALLISTA', 'TXT_KEY_UNIT_ROMAN_BALLISTA', 'UNIT_ROMAN_BALLISTA');

UPDATE Units SET MinorCivGift = '1', Cost = '-1', Class = 'UNITCLASS_BALLISTA' WHERE Type = 'UNIT_ROMAN_BALLISTA';

---------------------------
-- Russia
---------------------------

---------------------------
-- Shoshone
---------------------------
-- Removed Pathfinder

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_PATHFINDER', 'TXT_KEY_UNIT_SHOSHONE_PATHFINDER', 'UNIT_SHOSHONE_PATHFINDER');

UPDATE Units SET Class = 'UNITCLASS_PATHFINDER' WHERE Type = 'UNIT_SHOSHONE_PATHFINDER';

DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_SHOSHONE_PATHFINDER';

INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
VALUES
	('TRAIT_GREAT_EXPANSE', 'UNITCOMBAT_RECON', 'PROMOTION_GOODY_HUT_PICKER');

---------------------------
-- Siam
---------------------------
-- Adjust Elephant

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_LANCER' WHERE UnitType = 'UNIT_SIAMESE_WARELEPHANT';

UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_LANCER' WHERE Type = 'UNIT_SIAMESE_WARELEPHANT';

---------------------------
-- Songhai
---------------------------
-- Adjust Cav

UPDATE Units SET Class = 'UNITCLASS_KNIGHT', GoodyHutUpgradeUnitClass = 'UNITCLASS_LANCER', DefaultUnitAI = 'UNITAI_FAST_ATTACK' WHERE Type = 'UNIT_SONGHAI_MUSLIMCAVALRY';

UPDATE Civilization_UnitClassOverrides Set UnitClassType = 'UNITCLASS_KNIGHT' WHERE UnitType = 'UNIT_SONGHAI_MUSLIMCAVALRY';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_LANCER' WHERE UnitType = 'UNIT_SONGHAI_MUSLIMCAVALRY';

---------------------------
-- Spain
---------------------------
-- Tercio now melee renaissance unit  - musketman now the 'ranged' unit
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_SPANISH_TERCIO';

UPDATE Units SET Class = 'UNITCLASS_TERCIO', IconAtlas = 'COMMUNITY_2_ATLAS', PortraitIndex = 23 WHERE Type = 'UNIT_SPANISH_TERCIO';

UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_EXPLORER' WHERE UnitType = 'UNIT_SPANISH_CONQUISTADOR';

UPDATE Units SET 
	FoundMid = '1', 
	DefaultUnitAI = 'UNITAI_FAST_ATTACK',
	GoodyHutUpgradeUnitClass = 'UNITCLASS_COMMANDO',
	Class = 'UNITCLASS_EXPLORER',
	Combat = 23,
	Cost = 180,
	FaithCost = 300,
	Moves = 3,
	BaseSightRange = 3,
	PrereqTech = 'TECH_COMPASS',
	ObsoleteTech = 'TECH_FLIGHT',
	CombatClass = 'UNITCOMBAT_RECON'
WHERE Type = 'UNIT_SPANISH_CONQUISTADOR';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_COMMANDO' WHERE UnitType = 'UNIT_SPANISH_CONQUISTADOR';
DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_SPANISH_CONQUISTADOR';

DELETE FROM ArtDefine_UnitInfoMemberInfos WHERE UnitInfoType = 'ART_DEF_UNIT_U_SPANISH_CONQUISTADOR';
INSERT INTO ArtDefine_UnitInfoMemberInfos	 
		(UnitInfoType,		  					UnitMemberInfoType,		  							NumMembers)
VALUES  ('ART_DEF_UNIT_U_SPANISH_CONQUISTADOR', 'ART_DEF_UNIT_MEMBER_U_SPANISH_CONQUISTADOR',		2),
		('ART_DEF_UNIT_U_SPANISH_CONQUISTADOR',	'ART_DEF_UNIT_MEMBER_SCOUT',						1),
		('ART_DEF_UNIT_U_SPANISH_CONQUISTADOR',	'ART_DEF_UNIT_MEMBER_U_SPANISH_TERCIO_PIQUERO',		1),
		('ART_DEF_UNIT_U_SPANISH_CONQUISTADOR', 'ART_DEF_UNIT_MEMBER_U_SPANISH_CONQUISTADOR',		1),
		('ART_DEF_UNIT_U_SPANISH_CONQUISTADOR',	'ART_DEF_UNIT_MEMBER_U_SPANISH_TERCIO_PIQUERO',		2);
---------------------------
-- Sweden
---------------------------

-- Removed Hakka, moved Carolean to Renaissance

DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_SWEDISH_HAKKAPELIITTA';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_SWEDISH_HAKKAPELIITTA', 'TXT_KEY_UNIT_SWEDISH_HAKKAPELIITTA', 'UNIT_SWEDISH_HAKKAPELIITTA');

UPDATE Units SET MinorCivGift = '1', Cost = '-1', Class = 'UNITCLASS_SWEDISH_HAKKAPELIITTA', GoodyHutUpgradeUnitClass = 'UNITCLASS_WWI_TANK' WHERE Type = 'UNIT_SWEDISH_HAKKAPELIITTA';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_WWI_TANK' WHERE UnitType = 'UNIT_SWEDISH_HAKKAPELIITTA';

UPDATE Units SET Class = 'UNITCLASS_RIFLEMAN', DefaultUnitAI = 'UNITAI_ATTACK', GoodyHutUpgradeUnitClass = 'UNITCLASS_GREAT_WAR_INFANTRY' WHERE Type = 'UNIT_SWEDISH_CAROLEAN';

UPDATE Civilization_UnitClassOverrides Set UnitClassType = 'UNITCLASS_RIFLEMAN' WHERE UnitType = 'UNIT_SWEDISH_CAROLEAN';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_GREAT_WAR_INFANTRY' WHERE UnitType = 'UNIT_SWEDISH_CAROLEAN';

INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
VALUES
	('TRAIT_DIPLOMACY_GREAT_PEOPLE', 'UNITCOMBAT_MELEE', 'PROMOTION_ATTACK_BONUS_SWEDEN'),
	('TRAIT_DIPLOMACY_GREAT_PEOPLE', 'UNITCOMBAT_RECON', 'PROMOTION_ATTACK_BONUS_SWEDEN'),
	('TRAIT_DIPLOMACY_GREAT_PEOPLE', 'UNITCOMBAT_MOUNTED', 'PROMOTION_ATTACK_BONUS_SWEDEN'),
	('TRAIT_DIPLOMACY_GREAT_PEOPLE', 'UNITCOMBAT_SIEGE', 'PROMOTION_MOBILITY'),
	('TRAIT_DIPLOMACY_GREAT_PEOPLE', 'UNITCOMBAT_GUN', 'PROMOTION_ATTACK_BONUS_SWEDEN'),
	('TRAIT_DIPLOMACY_GREAT_PEOPLE', 'UNITCOMBAT_ARMOR', 'PROMOTION_ATTACK_BONUS_SWEDEN'),
	('TRAIT_DIPLOMACY_GREAT_PEOPLE', 'UNITCOMBAT_HELICOPTER', 'PROMOTION_ATTACK_BONUS_SWEDEN');
---------------------------
-- Venice
---------------------------

-- Removed Great Galleass, now a minor civ gift
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_VENETIAN_GALLEASS';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_VENETIAN_GALLEASS', 'UNIT_VENETIAN_GALLEASS');

UPDATE Units SET MinorCivGift = '1', Cost = '-1', Class = 'UNITCLASS_VENETIAN_GALLEASS' WHERE Type = 'UNIT_VENETIAN_GALLEASS';

---------------------------
-- Zulu
---------------------------
-- Impi now a musketman UU
UPDATE Units SET Class = 'UNITCLASS_TERCIO', GoodyHutUpgradeUnitClass = 'UNITCLASS_RIFLEMAN' WHERE Type = 'UNIT_ZULU_IMPI';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_RIFLEMAN' WHERE UnitType = 'UNIT_ZULU_IMPI';

UPDATE Civilization_UnitClassOverrides Set UnitClassType = 'UNITCLASS_TERCIO' WHERE UnitType = 'UNIT_ZULU_IMPI';
