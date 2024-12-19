-- Delete things we replace later
DELETE FROM Unit_ClassUpgrades;
DELETE FROM Unit_FreePromotions;
DELETE FROM Unit_ResourceQuantityRequirements;
DELETE FROM Unit_YieldFromKills;
DELETE FROM Unit_AITypes;

-- Make sure existing units have the correct UnitClass first
-- CS/Policy units get their would-be UnitClass as if they were unique units
-- They'll be updated (again) in CityStateUnitChanges.sql/PolicyUnitChanges.sql
CREATE TEMP TABLE Unit_UnitClass (
	UnitType TEXT,
	UnitClass TEXT
);

INSERT INTO Unit_UnitClass
VALUES
	-- Base units
	('UNIT_INCAN_SLINGER', 'UNITCLASS_SLINGER'),
	('UNIT_MONGOLIAN_KESHIK', 'UNITCLASS_SKIRMISHER'),
	('UNIT_SHOSHONE_PATHFINDER', 'UNITCLASS_PATHFINDER'),
	('UNIT_SPANISH_TERCIO', 'UNITCLASS_TERCIO'),
	-- Unique units
	('UNIT_ARABIAN_CAMELARCHER', 'UNITCLASS_HEAVY_SKIRMISHER'),
	('UNIT_ASSYRIAN_SIEGE_TOWER', 'UNITCLASS_SIEGE_TOWER'),
	('UNIT_AUSTRIAN_HUSSAR', 'UNITCLASS_CUIRASSIER'),
	('UNIT_AZTEC_JAGUAR', 'UNITCLASS_LONGSWORDSMAN'),
	('UNIT_BYZANTINE_CATAPHRACT', 'UNITCLASS_KNIGHT'),
	('UNIT_BYZANTINE_DROMON', 'UNITCLASS_LIBURNA'),
	('UNIT_DANISH_BERSERKER', 'UNITCLASS_PIKEMAN'),
	('UNIT_FRENCH_MUSKETEER', 'UNITCLASS_TERCIO'),
	('UNIT_GERMAN_LANDSKNECHT', 'UNITCLASS_TERCIO'),
	('UNIT_HUN_HORSE_ARCHER', 'UNITCLASS_SKIRMISHER'),
	('UNIT_INDIAN_WARELEPHANT', 'UNITCLASS_CUIRASSIER'),
	('UNIT_MAYAN_ATLATLIST', 'UNITCLASS_COMPOSITE_BOWMAN'),
	('UNIT_POLYNESIAN_MAORI_WARRIOR', 'UNITCLASS_PIKEMAN'),
	('UNIT_SPANISH_CONQUISTADOR', 'UNITCLASS_EXPLORER'),
	('UNIT_SWEDISH_HAKKAPELIITTA', 'UNITCLASS_CUIRASSIER'),
	('UNIT_ZULU_IMPI', 'UNITCLASS_TERCIO'),
	-- Policy units
	('UNIT_FRENCH_FOREIGNLEGION', 'UNITCLASS_RIFLEMAN'),
	-- City-State units
	('UNIT_HUN_BATTERING_RAM', 'UNITCLASS_SWORDSMAN'),
	-- Barbarian units
	('UNIT_BARBARIAN_AXMAN', 'UNITCLASS_SLINGER');

UPDATE Units
SET Class = (SELECT UnitClass FROM Unit_UnitClass WHERE UnitType = Type)
WHERE EXISTS (SELECT 1 FROM Unit_UnitClass WHERE UnitType = Type);

DROP TABLE Unit_UnitClass;

UPDATE Units
SET MinorCivGift = 1
WHERE Type IN (
	'UNIT_BRAZILIAN_PRACINHA',
	'UNIT_DANISH_SKI_INFANTRY',
	'UNIT_ENGLISH_LONGBOWMAN',
	'UNIT_ETHIOPIAN_MEHAL_SEFARI',
	'UNIT_GREEK_COMPANIONCAVALRY',
	'UNIT_HUN_BATTERING_RAM',
	'UNIT_OTTOMAN_SIPAHI'
);

-- Make sure existing unique, policy and CS units have the same stats and properties as the vanilla version of their base units
-- This may seem like an extra step, but it is one-time work and should never need to change again, while making things easier to balance
-- Most stats and properties are swept later and don't need to be included here anyway

-- America: Musketman

-- Aztec: Longswordsman
UPDATE Units
SET
	PrereqTech = 'TECH_STEEL',
	ObsoleteTech = 'TECH_GUNPOWDER'
WHERE Type = 'UNIT_AZTEC_JAGUAR';

-- Babylon: Archer

-- Byzantium: Knight
UPDATE Units
SET
	PrereqTech = 'TECH_CHIVALRY',
	ObsoleteTech = 'TECH_MILITARY_SCIENCE'
WHERE Type = 'UNIT_BYZANTINE_CATAPHRACT';

-- Carthage: Trireme

-- Carthage: Horseman

-- Celts: Spearman

-- China: Crossbowman

-- Denmark: Pikeman
UPDATE Units
SET
	PrereqTech = 'TECH_CIVIL_SERVICE',
	DefaultUnitAI = 'UNITAI_COUNTER',
	MoveRate = 'BIPED'
WHERE Type = 'UNIT_DANISH_BERSERKER';

-- Egypt: Chariot Archer

-- England: Frigate

-- France: Tercio
UPDATE Units SET CombatClass = 'UNITCOMBAT_MELEE' WHERE Type = 'UNIT_FRENCH_MUSKETEER';

-- Germany: Tercio
UPDATE Units
SET
	PurchaseOnly = 0,
	MoveAfterPurchase = 0,
	DefaultUnitAI = 'UNITAI_ATTACK',
	PrereqTech = 'TECH_GUNPOWDER',
	ObsoleteTech = 'TECH_RIFLING',
	PolicyType = NULL
WHERE Type = 'UNIT_GERMAN_LANDSKNECHT';

-- Greece: Spearman

-- Huns: Skirmisher
UPDATE Units
SET
	PrereqTech = 'TECH_CHIVALRY',
	ObsoleteTech = 'TECH_MILITARY_SCIENCE'
WHERE Type = 'UNIT_HUN_HORSE_ARCHER';

-- Indonesia: Swordsman

-- Iroquois: Swordsman
UPDATE Units SET ObsoleteTech = 'TECH_STEEL' WHERE Type = 'UNIT_IROQUOIAN_MOHAWKWARRIOR';

-- Japan: Longswordsman
UPDATE Units
SET
	ObsoleteTech = 'TECH_GUNPOWDER',
	WorkRate = 0
WHERE Type = 'UNIT_JAPANESE_SAMURAI';

DELETE FROM Unit_Builds WHERE UnitType = 'UNIT_JAPANESE_SAMURAI';

-- Korea: Trebuchet
UPDATE Units SET DefaultUnitAI = 'UNITAI_CITY_BOMBARD' WHERE Type = 'UNIT_KOREAN_HWACHA';

-- Korea: Caravel
UPDATE Units
SET
	DefaultUnitAI = 'UNITAI_EXPLORE_SEA',
	ObsoleteTech = 'TECH_STEAM_POWER'
WHERE Type = 'UNIT_KOREAN_TURTLE_SHIP';

-- Maya: Composite Bowman
UPDATE Units
SET
	PrereqTech = 'TECH_CONSTRUCTION',
	ObsoleteTech = 'TECH_MACHINERY'
WHERE Type = 'UNIT_MAYAN_ATLATLIST';

-- Mongolia: Great General

-- Morocco: Cavalry

-- Netherlands: Corvette

-- Ottoman: Musketman

-- Persia: Spearman

-- Poland: Lancer

-- Polynesia: Pikeman
UPDATE Units
SET
	PrereqTech = 'TECH_CIVIL_SERVICE',
	ObsoleteTech = 'TECH_METALLURGY',
	DefaultUnitAI = 'UNITAI_COUNTER'
WHERE Type = 'UNIT_POLYNESIAN_MAORI_WARRIOR';

-- Portugal: Caravel
UPDATE Units SET NumExoticGoods = 0 WHERE Type = 'UNIT_PORTUGUESE_NAU';

-- Rome: Swordsman
UPDATE Units
SET
	ObsoleteTech = 'TECH_STEEL',
	WorkRate = 0
WHERE Type = 'UNIT_ROMAN_LEGION';

DELETE FROM Unit_Builds WHERE UnitType = 'UNIT_ROMAN_LEGION';

-- Rome: Catapult

-- Russia: Cavalry

-- Shoshone: Cavalry

-- Siam: Knight

-- Songhai: Knight
UPDATE Units SET DefaultUnitAI = 'UNITAI_FAST_ATTACK' WHERE Type = 'UNIT_SONGHAI_MUSLIMCAVALRY';

-- Sweden: Fusilier

-- Venice: Great Merchant
UPDATE Units
SET
	CanBuyCityState = 0,
	MoveRate = 'GREAT_PERSON'
WHERE Type = 'UNIT_VENETIAN_MERCHANT';

-- Venice: Galleass

-- Zulu: Tercio
UPDATE Units
SET
	DefaultUnitAI = 'UNITAI_ATTACK',
	PrereqTech = 'TECH_GUNPOWDER'
WHERE Type = 'UNIT_ZULU_IMPI';

-- Authority: Fusilier
UPDATE Units
SET
	PrereqTech = 'TECH_RIFLING',
	ObsoleteTech = 'TECH_REPLACEABLE_PARTS'
WHERE Type = 'UNIT_FRENCH_FOREIGNLEGION';

-- Freedom: Heavy Bomber

-- Order: Tank

-- Autocracy: Fighter

-- Pracinha: Infantry

-- Norwegian Ski Infantry: Fusilier

-- Longbowman: Crossbowman

-- Mehal Sefari: Fusilier

-- Companion Cavalry: Horseman

-- Battering Ram: Swordsman (it's a unique class, but for consistency purposes it's classed as Swordsman for now)
UPDATE Units
SET
	DefaultUnitAI = 'UNITAI_ATTACK',
	PrereqTech = 'TECH_IRON_WORKING',
	ObsoleteTech = 'TECH_STEEL'
WHERE Type = 'UNIT_HUN_BATTERING_RAM';

-- Sipahi: Lancer

-- Barbarian: Warrior
UPDATE Units SET ObsoleteTech = 'TECH_METAL_CASTING' WHERE Type = 'UNIT_BARBARIAN_WARRIOR';

-- Barbarian: Archer
UPDATE Units SET ObsoleteTech = 'TECH_CONSTRUCTION' WHERE Type = 'UNIT_BARBARIAN_ARCHER';

-- Barbarian: Horseman

-- Barbarian: Spearman

-- Barbarian: Swordsman
UPDATE Units SET ObsoleteTech = 'TECH_STEEL' WHERE Type = 'UNIT_BARBARIAN_SWORDSMAN';

-- New unique/policy/CS units with existing base units:
-- Only need to insert into non-main tables here; others have been done in NewUnits.xml
-- Unit_Builds is the only relevant table for units

-- Zulu: Great General
INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_INDUNA', 'BUILD_CITADEL');

-- Existing unique/policy/CS units with new base units:
-- Set to follow NewUnits.xml, delete from all other tables
-- These need to be edited every time a new column value is added to/removed from the base unit in NewUnits.xml

-- Arabia: Heavy Skirmisher
UPDATE Units
SET
	PrereqTech = (SELECT PrereqTech FROM Units WHERE Type = 'UNIT_HEAVY_SKIRMISHER'),
	ObsoleteTech = (SELECT ObsoleteTech FROM Units WHERE Type = 'UNIT_HEAVY_SKIRMISHER'),
	CombatClass = (SELECT CombatClass FROM Units WHERE Type = 'UNIT_HEAVY_SKIRMISHER'),
	IsMounted = (SELECT IsMounted FROM Units WHERE Type = 'UNIT_HEAVY_SKIRMISHER'),
	DefaultUnitAI = (SELECT DefaultUnitAI FROM Units WHERE Type = 'UNIT_HEAVY_SKIRMISHER')
WHERE Type = 'UNIT_ARABIAN_CAMELARCHER';

-- Austria: Cuirassier
UPDATE Units
SET
	PrereqTech = (SELECT PrereqTech FROM Units WHERE Type = 'UNIT_CUIRASSIER'),
	ObsoleteTech = (SELECT ObsoleteTech FROM Units WHERE Type = 'UNIT_CUIRASSIER'),
	CombatClass = (SELECT CombatClass FROM Units WHERE Type = 'UNIT_CUIRASSIER'),
	IsMounted = (SELECT IsMounted FROM Units WHERE Type = 'UNIT_CUIRASSIER'),
	DefaultUnitAI = (SELECT DefaultUnitAI FROM Units WHERE Type = 'UNIT_CUIRASSIER')
WHERE Type = 'UNIT_AUSTRIAN_HUSSAR';

-- Byzantium: Liburna
UPDATE Units
SET
	PrereqTech = (SELECT PrereqTech FROM Units WHERE Type = 'UNIT_LIBURNA'),
	ObsoleteTech = (SELECT ObsoleteTech FROM Units WHERE Type = 'UNIT_LIBURNA'),
	CombatClass = (SELECT CombatClass FROM Units WHERE Type = 'UNIT_LIBURNA'),
	DefaultUnitAI = (SELECT DefaultUnitAI FROM Units WHERE Type = 'UNIT_LIBURNA')
WHERE Type = 'UNIT_BYZANTINE_DROMON';

-- Inca: Slinger
UPDATE Units
SET
	PrereqTech = (SELECT PrereqTech FROM Units WHERE Type = 'UNIT_INCAN_SLINGER'),
	ObsoleteTech = (SELECT ObsoleteTech FROM Units WHERE Type = 'UNIT_INCAN_SLINGER'),
	CombatClass = (SELECT CombatClass FROM Units WHERE Type = 'UNIT_INCAN_SLINGER'),
	DefaultUnitAI = (SELECT DefaultUnitAI FROM Units WHERE Type = 'UNIT_INCAN_SLINGER')
WHERE Type = 'UNIT_WARAKAQ';

-- India: Cuirassier
UPDATE Units
SET
	PrereqTech = (SELECT PrereqTech FROM Units WHERE Type = 'UNIT_CUIRASSIER'),
	ObsoleteTech = (SELECT ObsoleteTech FROM Units WHERE Type = 'UNIT_CUIRASSIER'),
	CombatClass = (SELECT CombatClass FROM Units WHERE Type = 'UNIT_CUIRASSIER'),
	IsMounted = (SELECT IsMounted FROM Units WHERE Type = 'UNIT_CUIRASSIER'),
	DefaultUnitAI = (SELECT DefaultUnitAI FROM Units WHERE Type = 'UNIT_CUIRASSIER')
WHERE Type = 'UNIT_INDIAN_WARELEPHANT';

-- Spain: Explorer
UPDATE Units
SET
	PrereqTech = (SELECT PrereqTech FROM Units WHERE Type = 'UNIT_EXPLORER'),
	ObsoleteTech = (SELECT ObsoleteTech FROM Units WHERE Type = 'UNIT_EXPLORER'),
	CombatClass = (SELECT CombatClass FROM Units WHERE Type = 'UNIT_EXPLORER'),
	DefaultUnitAI = (SELECT DefaultUnitAI FROM Units WHERE Type = 'UNIT_EXPLORER'),
	FoundAbroad = (SELECT FoundAbroad FROM Units WHERE Type = 'UNIT_EXPLORER')
WHERE Type = 'UNIT_SPANISH_CONQUISTADOR';

-- Sweden: Cuirassier
UPDATE Units
SET
	PrereqTech = (SELECT PrereqTech FROM Units WHERE Type = 'UNIT_CUIRASSIER'),
	ObsoleteTech = (SELECT ObsoleteTech FROM Units WHERE Type = 'UNIT_CUIRASSIER'),
	CombatClass = (SELECT CombatClass FROM Units WHERE Type = 'UNIT_CUIRASSIER'),
	IsMounted = (SELECT IsMounted FROM Units WHERE Type = 'UNIT_CUIRASSIER'),
	DefaultUnitAI = (SELECT DefaultUnitAI FROM Units WHERE Type = 'UNIT_CUIRASSIER')
WHERE Type = 'UNIT_SWEDISH_HAKKAPELIITTA';

-- Barbarian: Slinger
UPDATE Units
SET
	PrereqTech = (SELECT PrereqTech FROM Units WHERE Type = 'UNIT_INCAN_SLINGER'),
	ObsoleteTech = (SELECT ObsoleteTech FROM Units WHERE Type = 'UNIT_INCAN_SLINGER'),
	CombatClass = (SELECT CombatClass FROM Units WHERE Type = 'UNIT_INCAN_SLINGER'),
	DefaultUnitAI = (SELECT DefaultUnitAI FROM Units WHERE Type = 'UNIT_INCAN_SLINGER')
WHERE Type = 'UNIT_BARBARIAN_AXMAN';

-- Barbarian: Galley
UPDATE Units
SET
	PrereqTech = (SELECT PrereqTech FROM Units WHERE Type = 'UNIT_VP_GALLEY'),
	ObsoleteTech = (SELECT ObsoleteTech FROM Units WHERE Type = 'UNIT_VP_GALLEY'),
	CombatClass = (SELECT CombatClass FROM Units WHERE Type = 'UNIT_VP_GALLEY'),
	DefaultUnitAI = (SELECT DefaultUnitAI FROM Units WHERE Type = 'UNIT_VP_GALLEY')
WHERE Type = 'UNIT_GALLEY';
