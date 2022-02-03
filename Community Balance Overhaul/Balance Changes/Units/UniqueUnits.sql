-- Unique Unit Changes
-- Includes removed units and unique promotions/abilities applied to UUs. All information related to combat strength, cost, and tech unlocks with full unit class


---------------------------
-- America
---------------------------
-- Change B17 (now Freedom unlock), Minutman now ranged Renaissance unit
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_AMERICAN_B17';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_B17', 'TXT_KEY_CIV5_INDUSTRIAL_B17_HEADING', 'UNIT_AMERICAN_B17');

UPDATE Units SET Class = 'UNITCLASS_B17' WHERE Type = 'UNIT_AMERICAN_B17';

UPDATE Units SET PolicyType = 'POLICY_THEIR_FINEST_HOUR' WHERE Type = 'UNIT_AMERICAN_B17';

UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_GATLINGGUN' WHERE Type = 'UNIT_AMERICAN_MINUTEMAN';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_GATLINGGUN' WHERE UnitType = 'UNIT_AMERICAN_MINUTEMAN';

---------------------------
-- Arabia
---------------------------
-- Camel Archer now a Mounted Ranged unit
UPDATE Units SET Class = 'UNITCLASS_MOUNTED_BOWMAN' WHERE Type = 'UNIT_ARABIAN_CAMELARCHER';

UPDATE Civilization_UnitClassOverrides Set UnitClassType = 'UNITCLASS_MOUNTED_BOWMAN' WHERE UnitType = 'UNIT_ARABIAN_CAMELARCHER';

UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_CUIRASSIER' WHERE Type = 'UNIT_ARABIAN_CAMELARCHER';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_CUIRASSIER' WHERE UnitType = 'UNIT_ARABIAN_CAMELARCHER';

DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_ARABIAN_CAMELARCHER';

DELETE FROM Unit_AITypes WHERE UnitType = 'UNIT_ARABIAN_CAMELARCHER' AND UnitAIType = 'UNITAI_FAST_ATTACK';


---------------------------
-- Assyria
---------------------------
-- Siege Tower now pseudo-GG

UPDATE Units SET CityAttackOnly = '1' WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER';

UPDATE Units SET Combat = '0' WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER';

UPDATE Units SET Cost = '100' WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER';

UPDATE Units SET DefaultUnitAI = 'UNITAI_CITY_SPECIAL' WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER';

UPDATE Units SET CombatClass = NULL WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER';

UPDATE Units SET Moves = '4' WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER';

UPDATE Units SET Class = 'UNITCLASS_ASSYRIAN_SIEGE_TOWER' WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER';

DELETE From Unit_AITypes WHERE UnitType = 'UNIT_ASSYRIAN_SIEGE_TOWER';

UPDATE Units SET PrereqTech = 'TECH_ARCHERY' WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER';

UPDATE Units SET ObsoleteTech = 'TECH_GUNPOWDER' WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER';

DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_ASSYRIAN_SIEGE_TOWER';

UPDATE Units SET Pillage = '0' WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER';

UPDATE Defines Set Value = '40' WHERE Name = 'SAPPED_CITY_ATTACK_MODIFIER';

UPDATE Units SET GoodyHutUpgradeUnitClass = NULL WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER';

DELETE FROM Unit_ClassUpgrades
WHERE UnitType = 'UNIT_ASSYRIAN_SIEGE_TOWER';

INSERT INTO Unit_AITypes
	(UnitType, UnitAIType)
VALUES
	('UNIT_ASSYRIAN_SIEGE_TOWER', 'UNITAI_CITY_SPECIAL');

INSERT INTO UnitClasses
	(Type, Description, DefaultUnit, MaxPlayerInstances)
VALUES
	('UNITCLASS_ASSYRIAN_SIEGE_TOWER', 'TXT_KEY_UNIT_ASSYRIAN_SIEGE_TOWER', 'NONE', 2);

---------------------------
-- Austria
---------------------------
UPDATE Units SET Moves = '4' WHERE Type = 'UNIT_AUSTRIAN_HUSSAR';

-- Hussar now a Cuirassier unit
UPDATE Units SET Class = 'UNITCLASS_CUIRASSIER' WHERE Type = 'UNIT_AUSTRIAN_HUSSAR';

UPDATE Civilization_UnitClassOverrides Set UnitClassType = 'UNITCLASS_CUIRASSIER' WHERE UnitType = 'UNIT_AUSTRIAN_HUSSAR';

UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_CAVALRY' WHERE Type = 'UNIT_AUSTRIAN_HUSSAR';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_CAVALRY' WHERE UnitType = 'UNIT_AUSTRIAN_HUSSAR';

UPDATE Units SET PrereqTech = 'TECH_METALLURGY' WHERE Type = 'UNIT_AUSTRIAN_HUSSAR';

UPDATE Units SET ObsoleteTech = 'TECH_BALLISTICS' WHERE Type = 'UNIT_AUSTRIAN_HUSSAR';

---------------------------
-- Aztec
---------------------------
-- Update unit
UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_SPEARMAN' WHERE UnitType = 'UNIT_AZTEC_JAGUAR';

---------------------------
-- Babylon
---------------------------
-- Bowman now a UU composite bowman
UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_CROSSBOWMAN' WHERE UnitType = 'UNIT_BABYLONIAN_BOWMAN';

UPDATE Civilization_UnitClassOverrides Set UnitClassType = 'UNITCLASS_COMPOSITE_BOWMAN' WHERE UnitType = 'UNIT_BABYLONIAN_BOWMAN';

UPDATE Units SET Class = 'UNITCLASS_COMPOSITE_BOWMAN' WHERE Type = 'UNIT_BABYLONIAN_BOWMAN';
UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_CROSSBOWMAN' WHERE Type = 'UNIT_BABYLONIAN_BOWMAN';

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

INSERT INTO Unit_AITypes
	(UnitType, UnitAIType)
VALUES
	('UNIT_BANDEIRANTES', 'UNITAI_WORKER'),
	('UNIT_BANDEIRANTES', 'UNITAI_EXPLORE');

INSERT INTO Unit_Builds (UnitType, BuildType) VALUES ('UNIT_BANDEIRANTES', 'BUILD_FORT');
INSERT INTO Unit_Builds (UnitType, BuildType) VALUES ('UNIT_BANDEIRANTES', 'BUILD_MINE');
INSERT INTO Unit_Builds (UnitType, BuildType) VALUES ('UNIT_BANDEIRANTES', 'BUILD_BRAZILWOOD_CAMP');
INSERT INTO Unit_Builds (UnitType, BuildType) VALUES ('UNIT_BANDEIRANTES', 'BUILD_QUARRY');

UPDATE Units SET ObsoleteTech = 'TECH_FLIGHT' WHERE Type = 'UNIT_BANDEIRANTES';

-- Removed Pracinha, now a Minor Civ Gift
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_BRAZILIAN_PRACINHA';
INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_BRAZILIAN_PRACINHA', 'TXT_KEY_UNIT_BRAZILIAN_PRACINHA', 'UNIT_BRAZILIAN_PRACINHA');

UPDATE Units SET Class = 'UNITCLASS_BRAZILIAN_PRACINHA' WHERE Type = 'UNIT_BRAZILIAN_PRACINHA';
UPDATE Units SET MinorCivGift = '1' 					WHERE Type = 'UNIT_BRAZILIAN_PRACINHA';
UPDATE Units SET Cost = '-1'							WHERE Type = 'UNIT_BRAZILIAN_PRACINHA';
UPDATE Units SET FaithCost = '-1' 						WHERE Type = 'UNIT_BRAZILIAN_PRACINHA';

---------------------------
-- Byzantines
---------------------------
-- Removed Dromon 

DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_BYZANTINE_DROMON';

UPDATE Units SET Class = 'UNITCLASS_KNIGHT' WHERE Type = 'UNIT_BYZANTINE_CATAPHRACT';

UPDATE Civilization_UnitClassOverrides Set UnitClassType = 'UNITCLASS_KNIGHT' WHERE UnitType = 'UNIT_BYZANTINE_CATAPHRACT';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_LANCER' WHERE UnitType = 'UNIT_BYZANTINE_CATAPHRACT';

UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_LANCER' WHERE Type = 'UNIT_BYZANTINE_CATAPHRACT';

---------------------------
-- Carthage
---------------------------
-- Elephant now a generic unit
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT';

DELETE FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_ELEPHANT_RIDER', 'TXT_KEY_UNIT_ELEPHANT_RIDER', 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT');

UPDATE Units SET Class = 'UNITCLASS_ELEPHANT_RIDER' WHERE Type = 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT';

UPDATE Units SET Description = 'TXT_KEY_UNIT_ELEPHANT_RIDER' WHERE Type = 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT';

UPDATE Units SET Civilopedia = 'TXT_KEY_UNIT_ELEPHANT_RIDER_TEXT' WHERE Type = 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT';

UPDATE Units SET Strategy = 'TXT_KEY_UNIT_ELEPHANT_RIDER_STRATEGY' WHERE Type = 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT';

UPDATE Units SET Help = 'TXT_KEY_UNIT_ELEPHANT_RIDER_HELP' WHERE Type = 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT';

UPDATE Units SET ResourceType = 'RESOURCE_IVORY' WHERE Type = 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT';

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
-- Removed ski infantry, buffed berserker

UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_TERCIO' WHERE Type = 'UNIT_DANISH_BERSERKER';
	
UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_TERCIO' WHERE UnitType = 'UNIT_DANISH_BERSERKER';

DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_DANISH_SKI_INFANTRY';
INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_SKI_INFANTRY', 'TXT_KEY_UNIT_DANISH_SKI_INFANTRY', 'UNIT_DANISH_SKI_INFANTRY');

UPDATE Units SET Class = 'UNITCLASS_SKI_INFANTRY' 							WHERE Type = 'UNIT_DANISH_SKI_INFANTRY';
UPDATE Units SET MinorCivGift = '1' 										WHERE Type = 'UNIT_DANISH_SKI_INFANTRY';
UPDATE Units SET Cost = '-1' 												WHERE Type = 'UNIT_DANISH_SKI_INFANTRY';

UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_GREAT_WAR_INFANTRY' 	WHERE Type = 'UNIT_DANISH_SKI_INFANTRY';
UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_GREAT_WAR_INFANTRY' WHERE UnitType = 'UNIT_DANISH_SKI_INFANTRY';

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

UPDATE Units SET MinorCivGift = '1' WHERE Type = 'UNIT_ENGLISH_LONGBOWMAN';

UPDATE Units SET Cost = '-1' WHERE Type = 'UNIT_ENGLISH_LONGBOWMAN';

UPDATE Units SET Class = 'UNITCLASS_LONGBOWMAN' WHERE Type = 'UNIT_ENGLISH_LONGBOWMAN';

UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_MUSKETMAN' WHERE Type = 'UNIT_ENGLISH_LONGBOWMAN';

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

UPDATE Civilization_UnitClassOverrides Set UnitClassType = 'UNITCLASS_TERCIO' WHERE UnitType = 'UNIT_FRENCH_MUSKETEER';


---------------------------
-- Germany
---------------------------
-- Remove Panzer, Landsknecht Stuff
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_GERMAN_PANZER';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_PANZER', 'TXT_KEY_UNIT_GERMAN_PANZER', 'UNIT_GERMAN_PANZER');

UPDATE Units SET Class = 'UNITCLASS_PANZER' WHERE Type = 'UNIT_GERMAN_PANZER';

UPDATE Units SET PrereqTech = 'TECH_COMBINED_ARMS' WHERE Type = 'UNIT_GERMAN_PANZER';

UPDATE Units SET PolicyType = 'POLICY_PATRIOTIC_WAR' WHERE Type = 'UNIT_GERMAN_PANZER';

UPDATE Units SET UnitArtInfo = 'ART_DEF_UNIT_TANK' WHERE Type = 'UNIT_GERMAN_PANZER';
UPDATE Units SET UnitArtInfo = 'ART_DEF_UNIT_U_GERMAN_PANZER' WHERE Type = 'UNIT_TANK';

UPDATE Units SET UnitFlagIconOffset = '57' WHERE Type = 'UNIT_GERMAN_PANZER';
UPDATE Units SET UnitFlagIconOffset = '58' WHERE Type = 'UNIT_TANK';

UPDATE Units SET PortraitIndex = '13' WHERE Type = 'UNIT_GERMAN_PANZER';
UPDATE Units SET PortraitIndex = '39' WHERE Type = 'UNIT_TANK';

UPDATE Units SET Moves = '5' WHERE Type = 'UNIT_GERMAN_PANZER';

INSERT INTO	Civilization_UnitClassOverrides
			(CivilizationType,			UnitClassType, 		UnitType)
VALUES		('CIVILIZATION_GERMANY',	'UNITCLASS_TERCIO',	'UNIT_GERMAN_LANDSKNECHT');

UPDATE Units SET Class = 'UNITCLASS_TERCIO' 						WHERE Type = 'UNIT_GERMAN_LANDSKNECHT';
UPDATE Units SET PrereqTech = 'TECH_GUILDS' 						WHERE Type = 'UNIT_GERMAN_LANDSKNECHT';
UPDATE Units SET ObsoleteTech = 'TECH_REPLACEABLE_PARTS' 			WHERE Type = 'UNIT_GERMAN_LANDSKNECHT';
UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_RIFLEMAN' 	WHERE Type = 'UNIT_GERMAN_LANDSKNECHT';
UPDATE Units SET PurchaseOnly = 0									WHERE Type = 'UNIT_GERMAN_LANDSKNECHT';
UPDATE Units SET PolicyType = NULL									WHERE Type = 'UNIT_GERMAN_LANDSKNECHT';
UPDATE Units SET ExtraMaintenanceCost = 1 							WHERE Type = 'UNIT_GERMAN_LANDSKNECHT';
UPDATE Units SET Combat = 26 										WHERE Type = 'UNIT_GERMAN_LANDSKNECHT';
UPDATE Units SET RequiresFaithPurchaseEnabled = 1					WHERE Type = 'UNIT_GERMAN_LANDSKNECHT';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_RIFLEMAN' WHERE UnitType = 'UNIT_GERMAN_LANDSKNECHT';

INSERT INTO UnitPromotions 
		(Type,						AttackFullyHealedMod,	CannotBeChosen, LostWithUpgrade, PortraitIndex,	IconAtlas,			PediaType,		PediaEntry,							Description,						Help,									Sound)
VALUES	('PROMOTION_DOPPELSOLDNER',	30,						1,				0,				 56,			'extraPromo_Atlas',	'PEDIA_MELEE',	'TXT_KEY_PROMOTION_DOPPELSOLDNER',	'TXT_KEY_PROMOTION_DOPPELSOLDNER',	'TXT_KEY_PROMOTION_DOPPELSOLDNER_HELP',	'AS2D_IF_LEVELUP');

---------------------------
-- Greece
---------------------------
-- Removed Companion Cavalry, buffed Hoplite
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_GREEK_COMPANIONCAVALRY';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_COMPANIONCAVALRY', 'TXT_KEY_UNIT_GREEK_COMPANIONCAVALRY', 'UNIT_GREEK_COMPANIONCAVALRY');

UPDATE Units SET MinorCivGift = '1' WHERE Type = 'UNIT_GREEK_COMPANIONCAVALRY';

UPDATE Units SET Cost = '-1' WHERE Type = 'UNIT_GREEK_COMPANIONCAVALRY';

UPDATE Units SET Class = 'UNITCLASS_COMPANIONCAVALRY' WHERE Type = 'UNIT_GREEK_COMPANIONCAVALRY';

---------------------------
-- Huns
---------------------------
-- Removed Battering Ram, tweaked Horse Archer to fit into new mounted ranged line
UPDATE Units SET Class = 'UNITCLASS_HORSE_ARCHER' WHERE Type = 'UNIT_HUN_HORSE_ARCHER';

UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_HORSE_ARCHER' WHERE CivilizationType = 'CIVILIZATION_HUNS' AND UnitClassType = 'UNITCLASS_CHARIOT_ARCHER';

UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_MOUNTED_BOWMAN' WHERE Type = 'UNIT_HUN_HORSE_ARCHER';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_MOUNTED_BOWMAN' WHERE UnitType = 'UNIT_HUN_HORSE_ARCHER';

DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_HUN_BATTERING_RAM';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_HUNNIC_BATTERING_RAM', 'TXT_KEY_UNIT_HUN_BATTERING_RAM', 'UNIT_HUN_BATTERING_RAM');

UPDATE Units SET MinorCivGift = '1' WHERE Type = 'UNIT_HUN_BATTERING_RAM';

UPDATE Units SET Cost = '-1' WHERE Type = 'UNIT_HUN_BATTERING_RAM';

UPDATE Units SET Class = 'UNITCLASS_HUNNIC_BATTERING_RAM' WHERE Type = 'UNIT_HUN_BATTERING_RAM';

INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
VALUES
	('TRAIT_RAZE_AND_HORSES', 'UNITCOMBAT_MOUNTED', 'PROMOTION_COERCION'),
	('TRAIT_RAZE_AND_HORSES', 'UNITCOMBAT_ARMOR', 'PROMOTION_COERCION');
---------------------------
-- Inca
---------------------------
-- Slinger modified below

---------------------------
-- India
---------------------------
-- Rename War Elephant, make a Cuirassier UU

UPDATE Units SET Class = 'UNITCLASS_CUIRASSIER' WHERE Type = 'UNIT_INDIAN_WARELEPHANT';

UPDATE Units SET ObsoleteTech = 'TECH_COMBUSTION' WHERE Type = 'UNIT_INDIAN_WARELEPHANT';

UPDATE Civilization_UnitClassOverrides Set UnitClassType = 'UNITCLASS_CUIRASSIER' WHERE UnitType = 'UNIT_INDIAN_WARELEPHANT';

UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_CAVALRY' WHERE Type = 'UNIT_INDIAN_WARELEPHANT';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_CAVALRY' WHERE UnitType = 'UNIT_INDIAN_WARELEPHANT';

UPDATE Units SET Moves = '3' WHERE Type = 'UNIT_INDIAN_WARELEPHANT';

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

UPDATE Unit_AITypes SET UnitAIType = "UNITAI_EXPLORE" WHERE UnitType = 'UNIT_JAPANESE_SAMURAI' and UnitAIType = "UNITAI_WORKER_SEA";

DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_JAPANESE_ZERO';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_ZERO', 'TXT_KEY_UNIT_JAPANESE_ZERO', 'UNIT_JAPANESE_ZERO');

UPDATE Units SET Class = 'UNITCLASS_ZERO' WHERE Type = 'UNIT_JAPANESE_ZERO';

UPDATE Units SET PolicyType = 'POLICY_MILITARISM' WHERE Type = 'UNIT_JAPANESE_ZERO';

---------------------------
-- Korea
---------------------------
-- Remove Turtle Ship, wxtend life of Hwacha

DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_KOREAN_TURTLE_SHIP';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_KOREAN_TURTLE_SHIP', 'TXT_KEY_UNIT_KOREAN_TURTLE_SHIP', 'UNIT_KOREAN_TURTLE_SHIP');

UPDATE Units SET Cost = '-1' WHERE Type = 'UNIT_KOREAN_TURTLE_SHIP';

UPDATE Units SET MinorCivGift = '1' WHERE Type = 'UNIT_KOREAN_TURTLE_SHIP';

UPDATE Units SET Class = 'UNITCLASS_KOREAN_TURTLE_SHIP' WHERE Type = 'UNIT_KOREAN_TURTLE_SHIP';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_PRIVATEER' WHERE UnitType = 'UNIT_KOREAN_TURTLE_SHIP';

INSERT INTO Unit_AITypes (UnitType, UnitAIType) VALUES ('UNIT_KOREAN_TURTLE_SHIP', 'UNITAI_EXPLORE_SEA');

INSERT INTO Unit_AITypes (UnitType, UnitAIType) VALUES ('UNIT_KOREAN_HWACHA', 'UNITAI_CITY_BOMBARD');


---------------------------
-- Maya
---------------------------
-- Atlatlist now a UU comp bowman
UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_CROSSBOWMAN' WHERE Type = 'UNIT_MAYAN_ATLATLIST';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_CROSSBOWMAN' WHERE UnitType = 'UNIT_MAYAN_ATLATLIST';

UPDATE Civilization_UnitClassOverrides Set UnitClassType = 'UNITCLASS_COMPOSITE_BOWMAN' WHERE UnitType = 'UNIT_MAYAN_ATLATLIST';

UPDATE Units SET Class = 'UNITCLASS_COMPOSITE_BOWMAN' WHERE Type = 'UNIT_MAYAN_ATLATLIST';

---------------------------
-- Mongolia
---------------------------
-- Remove Keshik, make base of Classical mounted ranged line
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_MONGOLIAN_KESHIK';

UPDATE Units SET Class = 'UNITCLASS_HORSE_ARCHER' WHERE Type = 'UNIT_MONGOLIAN_KESHIK';

UPDATE Units SET Moves = '4' WHERE Type = 'UNIT_MONGOLIAN_KESHIK';

UPDATE Units SET UnitFlagAtlas = 'MOUNTED_XBOW_FLAG_ATLAS' WHERE Type = 'UNIT_MONGOLIAN_KESHIK';

UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_MOUNTED_BOWMAN' WHERE Type = 'UNIT_MONGOLIAN_KESHIK';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_MOUNTED_BOWMAN' WHERE UnitType = 'UNIT_MONGOLIAN_KESHIK';

INSERT INTO UnitPromotions
	(Type, Description, Help, Sound, OrderPriority, PortraitIndex, IconAtlas, PediaType, IgnoreZOC, MovesChange, CannotBeChosen, PediaEntry)
VALUES
	('PROMOTION_MONGOL_TERROR', 'TXT_KEY_PROMOTION_MONGOL_TERROR', 'TXT_KEY_PROMOTION_MONGOL_TERROR_HELP', 'AS2D_IF_LEVELUP', 99, 1, 'EXPANSION2_PROMOTION_ATLAS', 'PEDIA_SHARED', 1, 2, 1, 'TXT_KEY_PROMOTION_MONGOL_TERROR');

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

DELETE FROM Unit_AITypes WHERE UnitType = 'UNIT_MONGOLIAN_KESHIK' AND UnitAIType = 'UNITAI_FAST_ATTACK';
DELETE FROM Unit_AITypes WHERE UnitType = 'UNIT_MONGOLIAN_KESHIK' AND UnitAIType = 'UNITAI_DEFENSE';


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

UPDATE Units SET MinorCivGift = '1' WHERE Type = 'UNIT_OTTOMAN_SIPAHI';

UPDATE Units SET Cost = '-1' WHERE Type = 'UNIT_OTTOMAN_SIPAHI';

UPDATE Units SET Class = 'UNITCLASS_SIPAHI' WHERE Type = 'UNIT_OTTOMAN_SIPAHI';

UPDATE Units SET ObsoleteTech = 'TECH_COMBUSTION' WHERE Type = 'UNIT_OTTOMAN_SIPAHI';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_WWI_TANK' WHERE UnitType = 'UNIT_OTTOMAN_SIPAHI';
	
UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_WWI_TANK' WHERE Type = 'UNIT_OTTOMAN_SIPAHI';

UPDATE Units SET IgnoreBuildingDefense = '0' WHERE Type = 'UNIT_OTTOMAN_SIPAHI';

UPDATE Units SET Class = 'UNITCLASS_MUSKETMAN' WHERE Type = 'UNIT_OTTOMAN_JANISSARY';

UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_MUSKETMAN' WHERE UnitType = 'UNIT_OTTOMAN_JANISSARY';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_GATLINGGUN' WHERE UnitType = 'UNIT_OTTOMAN_JANISSARY';
	
UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_GATLINGGUN' WHERE Type = 'UNIT_OTTOMAN_JANISSARY';

---------------------------
-- Persia
---------------------------
-- Immortal gains extra defense

---------------------------
-- Poland
---------------------------
-- Adjust Hussar
UPDATE Units SET IgnoreBuildingDefense = '0' WHERE Type = 'UNIT_POLISH_WINGED_HUSSAR';

UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_WWI_TANK' WHERE Type = 'UNIT_POLISH_WINGED_HUSSAR';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_WWI_TANK' WHERE UnitType = 'UNIT_POLISH_WINGED_HUSSAR';

DELETE FROM Unit_AITypes WHERE UnitType = 'UNIT_POLISH_WINGED_HUSSAR' AND UnitAIType = 'UNITAI_ATTACK';


---------------------------
-- Polynesia
---------------------------
-- Maori Warrior now a pikeman UU
UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_TERCIO' WHERE Type = 'UNIT_POLYNESIAN_MAORI_WARRIOR';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_TERCIO' WHERE UnitType = 'UNIT_POLYNESIAN_MAORI_WARRIOR';

UPDATE Civilization_UnitClassOverrides Set UnitClassType = 'UNITCLASS_PIKEMAN' WHERE UnitType = 'UNIT_POLYNESIAN_MAORI_WARRIOR';

UPDATE Units SET Class = 'UNITCLASS_PIKEMAN' WHERE Type = 'UNIT_POLYNESIAN_MAORI_WARRIOR';

INSERT INTO Trait_BuildsUnitClasses
	(TraitType, UnitClassType, BuildType)
VALUES
	('TRAIT_WAYFINDING', 'UNITCLASS_TRIREME', 'BUILD_FISHING_BOATS'),
	('TRAIT_WAYFINDING', 'UNITCLASS_CARAVEL', 'BUILD_FISHING_BOATS'),
	('TRAIT_WAYFINDING', 'UNITCLASS_IRONCLAD', 'BUILD_FISHING_BOATS'),
	('TRAIT_WAYFINDING', 'UNITCLASS_DESTROYER', 'BUILD_FISHING_BOATS'),
	('TRAIT_WAYFINDING', 'UNITCLASS_PRIVATEER', 'BUILD_FISHING_BOATS'),
	('TRAIT_WAYFINDING', 'UNITCLASS_MISSILE_CRUISER', 'BUILD_FISHING_BOATS');
---------------------------
--Portugal
---------------------------
-- Nau buffed a little, grants a feitoria on use of exotic goods

UPDATE Units SET NumExoticGoods = '2' WHERE Type = 'UNIT_PORTUGUESE_NAU';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_PRIVATEER' WHERE UnitType = 'UNIT_PORTUGUESE_NAU';

UPDATE Units SET DefaultUnitAI = 'UNITAI_ATTACK_SEA' WHERE Type = 'UNIT_PORTUGUESE_NAU';

---------------------------
-- Rome
---------------------------
-- Removed Ballista 

DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_ROMAN_BALLISTA';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_BALLISTA', 'TXT_KEY_UNIT_ROMAN_BALLISTA', 'UNIT_ROMAN_BALLISTA');

UPDATE Units SET MinorCivGift = '1' WHERE Type = 'UNIT_ROMAN_BALLISTA';

UPDATE Units SET Cost = '-1' WHERE Type = 'UNIT_ROMAN_BALLISTA';

UPDATE Units SET Class = 'UNITCLASS_BALLISTA' WHERE Type = 'UNIT_ROMAN_BALLISTA';

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

UPDATE Units SET Class = 'UNITCLASS_KNIGHT' WHERE Type = 'UNIT_SONGHAI_MUSLIMCAVALRY';

UPDATE Civilization_UnitClassOverrides Set UnitClassType = 'UNITCLASS_KNIGHT' WHERE UnitType = 'UNIT_SONGHAI_MUSLIMCAVALRY';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_LANCER' WHERE UnitType = 'UNIT_SONGHAI_MUSLIMCAVALRY';

UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_LANCER' WHERE Type = 'UNIT_SONGHAI_MUSLIMCAVALRY';

UPDATE Units SET DefaultUnitAI = 'UNITAI_FAST_ATTACK' WHERE Type = 'UNIT_SONGHAI_MUSLIMCAVALRY';

DELETE FROM Unit_AITypes WHERE UnitType = 'UNIT_SONGHAI_MUSLIMCAVALRY' AND UnitAIType = 'UNITAI_ATTACK';


---------------------------
-- Spain
---------------------------
-- Tercio now melee renaissance unit  - musketman now the 'ranged' unit
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_SPANISH_TERCIO';

UPDATE Units SET Class = 'UNITCLASS_TERCIO' WHERE Type = 'UNIT_SPANISH_TERCIO';

UPDATE Units SET FoundMid = '1' WHERE Type = 'UNIT_SPANISH_CONQUISTADOR';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_LANCER' WHERE UnitType = 'UNIT_SPANISH_CONQUISTADOR';

UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_LANCER' WHERE Type = 'UNIT_SPANISH_CONQUISTADOR';

UPDATE Units SET DefaultUnitAI = 'UNITAI_FAST_ATTACK' WHERE Type = 'UNIT_SPANISH_CONQUISTADOR';

DELETE FROM Unit_AITypes WHERE UnitType = 'UNIT_SPANISH_CONQUISTADOR' AND UnitAIType = 'UNITAI_ATTACK';


---------------------------
-- Sweden
---------------------------

-- Removed Hakka, moved Carolean to Renaissance

DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_SWEDISH_HAKKAPELIITTA';

INSERT INTO UnitClasses (Type, Description, DefaultUnit) VALUES ('UNITCLASS_SWEDISH_HAKKAPELIITTA', 'TXT_KEY_UNIT_SWEDISH_HAKKAPELIITTA', 'UNIT_SWEDISH_HAKKAPELIITTA');

UPDATE Units SET MinorCivGift = '1' WHERE Type = 'UNIT_SWEDISH_HAKKAPELIITTA';

UPDATE Units SET Cost = '-1' WHERE Type = 'UNIT_SWEDISH_HAKKAPELIITTA';

UPDATE Units SET Class = 'UNITCLASS_SWEDISH_HAKKAPELIITTA' WHERE Type = 'UNIT_SWEDISH_HAKKAPELIITTA';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_WWI_TANK' WHERE UnitType = 'UNIT_SWEDISH_HAKKAPELIITTA';

UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_WWI_TANK' WHERE Type = 'UNIT_SWEDISH_HAKKAPELIITTA';

UPDATE Units SET Class = 'UNITCLASS_RIFLEMAN' WHERE Type = 'UNIT_SWEDISH_CAROLEAN';

UPDATE Units SET DefaultUnitAI = 'UNITAI_ATTACK' WHERE Type = 'UNIT_SWEDISH_CAROLEAN';

UPDATE Civilization_UnitClassOverrides Set UnitClassType = 'UNITCLASS_RIFLEMAN' WHERE UnitType = 'UNIT_SWEDISH_CAROLEAN';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_GREAT_WAR_INFANTRY' WHERE UnitType = 'UNIT_SWEDISH_CAROLEAN';

UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_GREAT_WAR_INFANTRY' WHERE Type = 'UNIT_SWEDISH_CAROLEAN';

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

UPDATE Units SET MinorCivGift = '1' WHERE Type = 'UNIT_VENETIAN_GALLEASS';

UPDATE Units SET Cost = '-1' WHERE Type = 'UNIT_VENETIAN_GALLEASS';

UPDATE Units SET Class = 'UNITCLASS_VENETIAN_GALLEASS' WHERE Type = 'UNIT_VENETIAN_GALLEASS';

DELETE FROM Unit_AITypes WHERE UnitType = 'UNIT_VENETIAN_GALLEASS' AND UnitAIType = 'UNITAI_EXPLORE_SEA';

---------------------------
-- Zulu
---------------------------
-- Impi now a musketman UU
UPDATE Units SET Class = 'UNITCLASS_TERCIO' WHERE Type = 'UNIT_ZULU_IMPI';
UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_RIFLEMAN' WHERE Type = 'UNIT_ZULU_IMPI';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_RIFLEMAN' WHERE UnitType = 'UNIT_ZULU_IMPI';

UPDATE Civilization_UnitClassOverrides Set UnitClassType = 'UNITCLASS_TERCIO' WHERE UnitType = 'UNIT_ZULU_IMPI';


-- NEW DATA

INSERT INTO UnitClasses
	(Type, Description, DefaultUnit)
VALUES
	('UNITCLASS_HORSE_ARCHER', 'TXT_KEY_UNIT_HUN_HORSE_ARCHER', 'UNIT_MONGOLIAN_KESHIK');

INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_ASSYRIA', 'UNITCLASS_ASSYRIAN_SIEGE_TOWER', 'UNIT_ASSYRIAN_SIEGE_TOWER'),
	('CIVILIZATION_BARBARIAN', 'UNITCLASS_ELEPHANT_RIDER', NULL );

