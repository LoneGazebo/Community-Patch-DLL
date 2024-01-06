-- NOTE: In NewUnits.xml and PreUnitChanges.sql, we already made sure all unique/policy/CS units have the same stats and properties as their counterparts.
-- We modify units based on UnitClass or UnitCombat here to keep them the same still.
-- The Civilization files/PolicyUnitChanges.sql/CityStateUnitChanges.sql/BarbarianUnitChanges.sql only have deltas between the unique and the base unit.

-- Helper temp tables because CTE isn't a thing in this SQLite version
CREATE TEMP TABLE Helper (
	UnitClassType TEXT,
	PromotionType TEXT,
	BuildingClassType TEXT
);

----------------------------------------------------------------------------
-- Melee/Gun
----------------------------------------------------------------------------

-- Warrior
UPDATE Units SET ObsoleteTech = 'TECH_BRONZE_WORKING' WHERE Class = 'UNITCLASS_WARRIOR';

-- Spearman
UPDATE Units SET ObsoleteTech = 'TECH_STEEL' WHERE Class = 'UNITCLASS_SPEARMAN';

-- Swordsman

-- Pikeman
UPDATE Units
SET
	PrereqTech = 'TECH_STEEL',
	ObsoleteTech = 'TECH_GUNPOWDER'
WHERE Class = 'UNITCLASS_PIKEMAN';

-- Longswordsman

-- Tercio
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_SPANISH_TERCIO';

UPDATE Units
SET
	UnitFlagAtlas = 'TERCIO_FLAG_ATLAS',
	UnitFlagIconOffset = 0,
	IconAtlas = 'COMMUNITY_2_ATLAS',
	PortraitIndex = 23
WHERE Type = 'UNIT_SPANISH_TERCIO';

UPDATE Units
SET
	PrereqTech = 'TECH_GUNPOWDER',
	ObsoleteTech = 'TECH_RIFLING',
	DefaultUnitAI = 'UNITAI_DEFENSE'
WHERE Class = 'UNITCLASS_TERCIO';

-- Fusilier

-- Rifleman
UPDATE Units SET ObsoleteTech = 'TECH_COMBINED_ARMS' WHERE Class = 'UNITCLASS_GREAT_WAR_INFANTRY';

-- Infantry
UPDATE Units SET PrereqTech = 'TECH_COMBINED_ARMS' WHERE Class = 'UNITCLASS_INFANTRY';

-- Mechanized Infantry

----------------------------------------------------------------------------
-- Ranged
----------------------------------------------------------------------------

-- Slinger

-- Archer
UPDATE Units
SET
	PrereqTech = 'TECH_CALENDAR',
	ObsoleteTech = 'TECH_CURRENCY'
WHERE Class = 'UNITCLASS_ARCHER';

-- Composite Bowman
UPDATE Units SET PrereqTech = 'TECH_CURRENCY' WHERE Class = 'UNITCLASS_COMPOSITE_BOWMAN';

-- Crossbowman
UPDATE Units SET ObsoleteTech = 'TECH_METALLURGY' WHERE Class = 'UNITCLASS_CROSSBOWMAN';

-- Musketman
UPDATE Units
SET
	CombatClass = 'UNITCOMBAT_ARCHER',
	PrereqTech = 'TECH_METALLURGY',
	ObsoleteTech = 'TECH_DYNAMITE',
	DefaultUnitAI = 'UNITAI_RANGED'
WHERE Class = 'UNITCLASS_MUSKETMAN';

-- Gatling Gun
UPDATE Units
SET
	PrereqTech = 'TECH_DYNAMITE',
	DefaultUnitAI = 'UNITAI_RANGED'
WHERE Class = 'UNITCLASS_GATLINGGUN';

-- Machine Gun
UPDATE Units
SET
	ObsoleteTech = 'TECH_MOBILE_TACTICS',
	DefaultUnitAI = 'UNITAI_RANGED'
WHERE Class = 'UNITCLASS_MACHINE_GUN';

-- Bazooka
UPDATE Units
SET
	PrereqTech = 'TECH_MOBILE_TACTICS',
	DefaultUnitAI = 'UNITAI_RANGED'
WHERE Class = 'UNITCLASS_BAZOOKA';

----------------------------------------------------------------------------
-- Mounted Melee/Armor
----------------------------------------------------------------------------

-- Horseman
UPDATE Units
SET PrereqTech = 'TECH_ARCHERY' -- Military Strategy
WHERE Class = 'UNITCLASS_HORSEMAN';

-- War Elephant
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT';

UPDATE Units
SET
	Description = 'TXT_KEY_UNIT_ELEPHANT_RIDER',
	Civilopedia = 'TXT_KEY_UNIT_ELEPHANT_RIDER_TEXT',
	Help = 'TXT_KEY_UNIT_ELEPHANT_RIDER_HELP',
	Strategy = 'TXT_KEY_UNIT_ELEPHANT_RIDER_STRATEGY'
WHERE Type = 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT';

UPDATE Units
SET
	PrereqTech = 'TECH_ARCHERY', -- Military Strategy
	ResourceType = 'RESOURCE_IVORY'
WHERE Class = 'UNITCLASS_ELEPHANT_RIDER';

INSERT INTO Unit_BuildingClassRequireds
	(UnitType, BuildingClassType)
SELECT
	Type, 'BUILDINGCLASS_BARRACKS'
FROM Units
WHERE Class = 'UNITCLASS_ELEPHANT_RIDER';

-- Knight
UPDATE Units SET ObsoleteTech = 'TECH_METALLURGY' WHERE Class = 'UNITCLASS_KNIGHT';

-- Lancer
UPDATE Units
SET
	PrereqTech = 'TECH_METALLURGY',
	ObsoleteTech = 'TECH_COMBUSTION'
WHERE Class = 'UNITCLASS_LANCER';

-- Landship
UPDATE Units SET DefaultUnitAI = 'UNITAI_FAST_ATTACK' WHERE Class = 'UNITCLASS_WWI_TANK';

-- Tank
UPDATE Units
SET
	UnitArtInfo = 'ART_DEF_UNIT_U_GERMAN_PANZER',
	UnitFlagIconOffset = 58,
	PortraitIndex = 39
WHERE Type = 'UNIT_TANK';

UPDATE Units SET ObsoleteTech = 'TECH_ADVANCED_BALLISTICS' WHERE Class = 'UNITCLASS_TANK';

-- Modern Armor
UPDATE Units SET PrereqTech = 'TECH_ADVANCED_BALLISTICS' WHERE Class = 'UNITCLASS_MODERN_ARMOR';

-- Giant Death Robot
UPDATE Units
SET
	PrereqTech = 'TECH_ROBOTICS',
	DefaultUnitAI = 'UNITAI_FAST_ATTACK'
WHERE Class = 'UNITCLASS_MECH';

----------------------------------------------------------------------------
-- Skirmisher
----------------------------------------------------------------------------

-- Chariot Archer
UPDATE Units
SET
	IsMounted = 1,
	PrereqTech = 'TECH_ANIMAL_HUSBANDRY',
	ObsoleteTech = 'TECH_MATHEMATICS'
WHERE Class = 'UNITCLASS_CHARIOT_ARCHER';

-- Skirmisher
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_MONGOLIAN_KESHIK';

UPDATE Units
SET
	UnitFlagAtlas = 'MOUNTED_XBOW_FLAG_ATLAS',
	MoveRate = 'QUADRUPED'
WHERE Type = 'UNIT_MONGOLIAN_KESHIK';

UPDATE Units
SET
	IsMounted = 1,
	PrereqTech = 'TECH_MATHEMATICS',
	ObsoleteTech = 'TECH_PHYSICS'
WHERE Class = 'UNITCLASS_HORSE_ARCHER';

-- Heavy Skirmisher

-- Cuirassier

-- Cavalry
UPDATE Units
SET
	CombatClass = 'UNITCOMBAT_ARCHER',
	IsMounted = 1,
	ObsoleteTech = 'TECH_BALLISTICS',
	DefaultUnitAI = 'UNITAI_RANGED'
WHERE Class = 'UNITCLASS_CAVALRY';

-- Light Tank
UPDATE Units
SET
	UnitArtInfo = 'ART_DEF_UNIT_ARMORED_CAR',
	UnitFlagAtlas = 'ARMORED_CAR_FLAG_ATLAS',
	UnitFlagIconOffset = 0,
	IconAtlas = 'COMMUNITY_2_ATLAS',
	PortraitIndex = 5
WHERE Type = 'UNIT_ANTI_TANK_GUN';

UPDATE Units
SET
	CombatClass = 'UNITCOMBAT_ARCHER',
	IsMounted = 1,
	PrereqTech = 'TECH_BALLISTICS',
	ObsoleteTech = 'TECH_MOBILE_TACTICS',
	DefaultUnitAI = 'UNITAI_RANGED'
WHERE Class = 'UNITCLASS_ANTI_TANK_GUN';

-- Helicopter Gunship
UPDATE Units
SET
	CombatClass = 'UNITCOMBAT_ARCHER',
	IsMounted = 1,
	PrereqTech = 'TECH_MOBILE_TACTICS',
	DefaultUnitAI = 'UNITAI_RANGED'
WHERE Class = 'UNITCLASS_HELICOPTER_GUNSHIP';

----------------------------------------------------------------------------
-- Siege
----------------------------------------------------------------------------

-- Catapult
UPDATE Units
SET PrereqTech = 'TECH_CONSTRUCTION' -- Masonry
WHERE Class = 'UNITCLASS_CATAPULT';

-- Trebuchet
UPDATE Units SET ObsoleteTech = 'TECH_GUNPOWDER' WHERE Class = 'UNITCLASS_TREBUCHET';

-- Cannon
UPDATE Units
SET
	PrereqTech = 'TECH_GUNPOWDER',
	ObsoleteTech = 'TECH_RIFLING'
WHERE Class = 'UNITCLASS_CANNON';

-- Field Gun

-- Artillery
UPDATE Units
SET
	PrereqTech = 'TECH_BALLISTICS',
	ObsoleteTech = 'TECH_ADVANCED_BALLISTICS'
WHERE Class = 'UNITCLASS_ARTILLERY';

-- Rocket Artillery
UPDATE Units SET PrereqTech = 'TECH_ADVANCED_BALLISTICS' WHERE Class = 'UNITCLASS_ROCKET_ARTILLERY';

----------------------------------------------------------------------------
-- Recon
----------------------------------------------------------------------------

-- Pathfinder
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_SHOSHONE_PATHFINDER';

UPDATE Units
SET ObsoleteTech = 'TECH_OPTICS' -- Sailing
WHERE Class = 'UNITCLASS_PATHFINDER';

-- Scout
UPDATE Units
SET
	PrereqTech = 'TECH_OPTICS', -- Sailing
	ObsoleteTech = 'TECH_COMPASS'
WHERE Class = 'UNITCLASS_SCOUT';

-- Explorer

-- Commando

-- Paratrooper
UPDATE Units
SET
	UnitArtInfo = 'ART_DEF_UNIT_MARINE',
	UnitFlagAtlas = 'EXPANSION_UNIT_FLAG_ATLAS',
	UnitFlagIconOffset = 19,
	IconAtlas = 'EXPANSION_UNIT_ATLAS_1',
	PortraitIndex = 19
WHERE Type = 'UNIT_PARATROOPER';

UPDATE Units
SET
	CombatClass = 'UNITCOMBAT_RECON',
	PrereqTech = 'TECH_FLIGHT',
	ObsoleteTech = 'TECH_COMPUTERS'
WHERE Class = 'UNITCLASS_PARATROOPER';

-- Special Forces
UPDATE Units
SET
	UnitArtInfo = 'ART_DEF_UNIT_PARATROOPER',
	UnitFlagAtlas = 'UNIT_FLAG_ATLAS',
	UnitFlagIconOffset = 59,
	IconAtlas = 'UNIT_ATLAS_2',
	PortraitIndex = 14
WHERE Type = 'UNIT_MARINE';

UPDATE Units
SET
	CombatClass = 'UNITCOMBAT_RECON',
	PrereqTech = 'TECH_COMPUTERS',
	ObsoleteTech = 'TECH_LASERS',
	DefaultUnitAI = 'UNITAI_PARADROP'
WHERE Class = 'UNITCLASS_MARINE';

-- XCOM Squad
UPDATE Units
SET
	CombatClass = 'UNITCOMBAT_RECON',
	PrereqTech = 'TECH_LASERS'
WHERE Class = 'UNITCLASS_XCOM_SQUAD';

----------------------------------------------------------------------------
-- Anti-Air
----------------------------------------------------------------------------

-- Anti-Aircraft Gun
UPDATE Units
SET
	ObsoleteTech = 'TECH_ADVANCED_BALLISTICS',
	AirInterceptRange = 3
WHERE Class = 'UNITCLASS_ANTI_AIRCRAFT_GUN';

-- Mobile SAM
UPDATE Units
SET
	PrereqTech = 'TECH_ADVANCED_BALLISTICS',
	AirInterceptRange = 4
WHERE Class = 'UNITCLASS_MOBILE_SAM';

----------------------------------------------------------------------------
-- Naval Melee
----------------------------------------------------------------------------

-- Galley
UPDATE UnitClasses SET DefaultUnit = 'UNIT_VP_GALLEY' WHERE Type = 'UNITCLASS_GALLEY';

UPDATE Units
SET
	PrereqTech = 'TECH_SAILING', -- Fishing
	ObsoleteTech = 'TECH_PHILOSOPHY'
WHERE Class = 'UNITCLASS_GALLEY';

-- Trireme
UPDATE Units
SET
	PrereqTech = 'TECH_PHILOSOPHY',
	ObsoleteTech = 'TECH_COMPASS'
WHERE Class = 'UNITCLASS_TRIREME';

-- Caravel
UPDATE Units
SET
	PrereqTech = 'TECH_COMPASS',
	ObsoleteTech = 'TECH_NAVIGATION',
	DefaultUnitAI = 'UNITAI_ATTACK_SEA'
WHERE Class = 'UNITCLASS_CARAVEL';

-- Corvette
UPDATE Units
SET
	UnitArtInfo = 'ART_DEF_UNIT_CORVETTE',
	UnitFlagAtlas = 'CORVETTE_FLAG_ATLAS',
	UnitFlagIconOffset = 0,
	IconAtlas = 'COMMUNITY_ATLAS',
	PortraitIndex = 60
WHERE Type = 'UNIT_PRIVATEER';

UPDATE Units
SET
	PrereqTech = 'TECH_NAVIGATION',
	ObsoleteTech = 'TECH_INDUSTRIALIZATION',
	DefaultUnitAI = 'UNITAI_ATTACK_SEA'
WHERE Class = 'UNITCLASS_PRIVATEER';

-- Ironclad
UPDATE Units
SET
	PrereqTech = 'TECH_INDUSTRIALIZATION',
	ObsoleteTech = 'TECH_RADIO'
WHERE Class = 'UNITCLASS_IRONCLAD';

-- Destroyer

-- Fleet Destroyer
UPDATE Units
SET
	PrereqTech = 'TECH_NUCLEAR_FISSION',
	ObsoleteTech = 'TECH_STEALTH'
WHERE Class = 'UNITCLASS_DESTROYER';

-- Sensor Combat Ship
UPDATE Units SET AirInterceptRange = 3 WHERE Class = 'UNITCLASS_MISSILE_DESTROYER';

----------------------------------------------------------------------------
-- Naval Ranged
----------------------------------------------------------------------------

-- Liburna

-- Galleass
UPDATE Units SET PrereqTech = 'TECH_GUILDS' WHERE Class = 'UNITCLASS_GALLEASS';

-- Frigate
UPDATE Units SET ObsoleteTech = 'TECH_DYNAMITE' WHERE Class = 'UNITCLASS_FRIGATE';

-- Cruiser

-- Dreadnought

-- Battleship
UPDATE Units
SET
	PrereqTech = 'TECH_NUCLEAR_FISSION',
	ObsoleteTech = 'TECH_LASERS'
WHERE Class = 'UNITCLASS_BATTLESHIP';

-- Missile Cruiser
UPDATE Units
SET
	CombatClass = 'UNITCOMBAT_NAVALRANGED',
	PrereqTech = 'TECH_LASERS'
WHERE Type = 'UNIT_MISSILE_CRUISER';

----------------------------------------------------------------------------
-- Submarine
----------------------------------------------------------------------------

-- Submarine
UPDATE Units
SET
	PrereqTech = 'TECH_PLASTIC',
	ObsoleteTech = 'TECH_ELECTRONICS'
WHERE Class = 'UNITCLASS_SUBMARINE';

-- Attack Submarine

-- Nuclear Submarine
UPDATE Units SET PrereqTech = 'TECH_ROBOTICS' WHERE Class = 'UNITCLASS_NUCLEAR_SUBMARINE';

----------------------------------------------------------------------------
-- Carrier
----------------------------------------------------------------------------

-- Carrier
UPDATE Units
SET
	PrereqTech = 'TECH_COMPUTERS',
	AirInterceptRange = 4
WHERE Class = 'UNITCLASS_CARRIER';

-- Supercarrier
UPDATE UnitClasses SET MaxPlayerInstances = 2 WHERE Type = 'UNITCLASS_SUPERCARRIER';

UPDATE Units SET AirInterceptRange = 5 WHERE Class = 'UNITCLASS_SUPERCARRIER';

----------------------------------------------------------------------------
-- Fighter
----------------------------------------------------------------------------

-- Triplane
UPDATE Units SET AirInterceptRange = 6 WHERE Class = 'UNITCLASS_TRIPLANE';

-- Fighter

-- Jet Fighter

----------------------------------------------------------------------------
-- Bomber
----------------------------------------------------------------------------

-- Bomber

-- Heavy Bomber

-- Stealth Bomber
UPDATE Units
SET Special = 'SPECIALUNIT_FIGHTER' -- Carrier-compatible
WHERE Class = 'UNITCLASS_STEALTH_BOMBER';

----------------------------------------------------------------------------
-- Missile
----------------------------------------------------------------------------

-- Rocket Missile

-- Guided Missile
UPDATE Units
SET
	PrereqTech = 'TECH_ADVANCED_BALLISTICS',
	NoMaintenance = 0
WHERE Class = 'UNITCLASS_GUIDED_MISSILE';

----------------------------------------------------------------------------
-- Nuke
----------------------------------------------------------------------------

-- Atomic Bomb

-- Nuclear Missile
UPDATE Units SET PrereqTech = 'TECH_LASERS' WHERE Class = 'UNITCLASS_NUCLEAR_MISSILE';

----------------------------------------------------------------------------
-- Settler
----------------------------------------------------------------------------

-- Settler
UPDATE Units
SET
	CombatClass = 'UNITCOMBAT_SETTLER',
	PrereqTech = 'TECH_POTTERY',
	ObsoleteTech = 'TECH_BANKING'
WHERE Class = 'UNITCLASS_SETTLER';

-- Pioneer
INSERT INTO Helper
	(BuildingClassType)
VALUES
	('BUILDINGCLASS_MONUMENT'),
	('BUILDINGCLASS_SHRINE'),
	('BUILDINGCLASS_GRANARY'),
	('BUILDINGCLASS_GROVE'),
	('BUILDINGCLASS_LODGE'),
	('BUILDINGCLASS_HERBALIST'),
	('BUILDINGCLASS_MARKET'),
	('BUILDINGCLASS_BARRACKS'),
	('BUILDINGCLASS_LIBRARY'),
	('BUILDINGCLASS_COLOSSEUM'),
	('BUILDINGCLASS_FORGE'),
	('BUILDINGCLASS_WELL'), -- no river only
	('BUILDINGCLASS_WATERMILL'); -- river only

INSERT INTO Unit_BuildOnFound
	(UnitType, BuildingClassType)
SELECT
	a.Type, b.BuildingClassType
FROM Units a, Helper b
WHERE a.Class = 'UNITCLASS_PIONEER';

-- Colonist
INSERT INTO Helper
	(BuildingClassType)
VALUES
	('BUILDINGCLASS_LIGHTHOUSE'),
	('BUILDINGCLASS_AMPHITHEATER'),
	('BUILDINGCLASS_TEMPLE'),
	('BUILDINGCLASS_AQUEDUCT'),
	('BUILDINGCLASS_HARBOR'),
	('BUILDINGCLASS_WORKSHOP'),
	('BUILDINGCLASS_WINDMILL');

INSERT INTO Unit_BuildOnFound
	(UnitType, BuildingClassType)
SELECT
	a.Type, b.BuildingClassType
FROM Units a, Helper b
WHERE a.Class = 'UNITCLASS_COLONIST';

DELETE FROM Helper;

----------------------------------------------------------------------------
-- Diplo units
----------------------------------------------------------------------------

-- Emissary

-- Envoy

-- Diplomat

-- Ambassador

----------------------------------------------------------------------------
-- Trade units
----------------------------------------------------------------------------

-- Caravan
UPDATE Units
SET
	CombatClass = 'UNITCOMBAT_CARAVAN',
	PrereqTech = 'TECH_HORSEBACK_RIDING' -- Trade
WHERE Class = 'UNITCLASS_CARAVAN';

-- Cargo Ship
UPDATE Units
SET
	CombatClass = 'UNITCOMBAT_CARGO',
	PrereqTech = 'TECH_OPTICS' -- Sailing
WHERE Class = 'UNITCLASS_CARGO_SHIP';

----------------------------------------------------------------------------
-- Spaceship parts
----------------------------------------------------------------------------

-- SS Stasis Chamber

-- SS Engine

-- SS Cockpit
UPDATE Units SET PrereqTech = 'TECH_GLOBALIZATION' WHERE Class = 'UNITCLASS_SS_COCKPIT';

-- SS Booster
UPDATE Units SET PrereqTech = 'TECH_NUCLEAR_FUSION' WHERE Class = 'UNITCLASS_SS_BOOSTER';

----------------------------------------------------------------------------
-- Great People
----------------------------------------------------------------------------

-- Great Writer
UPDATE Units
SET
	BaseCultureTurnsToCount = 5,
	ScaleFromNumGWs = 3
WHERE Class = 'UNITCLASS_WRITER';

-- Great Artist
UPDATE Units
SET
	GoldenAgeTurns = 0,
	BaseTurnsForGAPToCount = 10,
	ScaleFromNumThemes = 20
WHERE Class = 'UNITCLASS_ARTIST';

-- Great Musician
UPDATE Units
SET
	OneShotTourism = 0,
	OneShotTourismPercentOthers = 0,
	TourismBonusTurns = 10
WHERE Class = 'UNITCLASS_MUSICIAN';

-- Great Scientist
UPDATE Units SET BaseBeakersTurnsToCount = 3 WHERE Class = 'UNITCLASS_SCIENTIST';

INSERT INTO Unit_ScalingFromOwnedImprovements
	(UnitType, ImprovementType, Amount)
SELECT
	Type, 'IMPROVEMENT_ACADEMY', 10
FROM Units
WHERE Class = 'UNITCLASS_SCIENTIST';

-- Great Merchant
UPDATE Units
SET
	BaseGold = 0,
	NumGoldPerEra = 0,
	BaseGoldTurnsToCount = 2,
	BaseWLTKDTurns = 5
WHERE Class = 'UNITCLASS_MERCHANT';

INSERT INTO Unit_ScalingFromOwnedImprovements
	(UnitType, ImprovementType, Amount)
SELECT
	Type, 'IMPROVEMENT_CUSTOMS_HOUSE', 20
FROM Units
WHERE Class = 'UNITCLASS_MERCHANT';

-- Great Engineer
UPDATE Units
SET
	BaseHurry = 0,
	HurryMultiplier = 0,
	BaseProductionTurnsToCount = 5
WHERE Class = 'UNITCLASS_ENGINEER';

INSERT INTO Unit_ScalingFromOwnedImprovements
	(UnitType, ImprovementType, Amount)
SELECT
	Type, 'IMPROVEMENT_MANUFACTORY', 10
FROM Units
WHERE Class = 'UNITCLASS_ENGINEER';

-- Great Diplomat
INSERT INTO Unit_ResourceQuantityExpended
	(UnitType, ResourceType, Amount)
SELECT
	Type, 'RESOURCE_PAPER', 1
FROM Units
WHERE Class = 'UNITCLASS_GREAT_DIPLOMAT';

-- Great General

-- Great Admiral
UPDATE Units SET NumFreeLux = 2 WHERE Class = 'UNITCLASS_GREAT_ADMIRAL';

-- Great Prophet
UPDATE Units SET Capture = NULL WHERE Class = 'UNITCLASS_PROPHET';

----------------------------------------------------------------------------
-- Other units
----------------------------------------------------------------------------

-- Worker
UPDATE Units
SET
	CombatClass = 'UNITCOMBAT_WORKER',
	WorkRate = 90 -- slowed down a bit, was 100
WHERE Class = 'UNITCLASS_WORKER';

-- Work Boat
UPDATE Units SET CombatClass = 'UNITCOMBAT_WORKBOAT' WHERE Class = 'UNITCLASS_WORKBOAT';

-- Missionary
UPDATE Units SET CombatClass = 'UNITCOMBAT_MISSIONARY' WHERE Class = 'UNITCLASS_MISSIONARY';

-- Inquisitor
UPDATE Units SET CombatClass = 'UNITCOMBAT_INQUISITOR' WHERE Class = 'UNITCLASS_INQUISITOR';

-- Archaeologist
UPDATE UnitClasses SET MaxPlayerInstances = 3 WHERE Type = 'UNITCLASS_ARCHAEOLOGIST';

UPDATE Unit_BuildingClassRequireds
SET BuildingClassType = 'BUILDINGCLASS_MUSEUM'
WHERE UnitType IN (
	SELECT Type FROM Units WHERE Class = 'UNITCLASS_ARCHAEOLOGIST'
);

DROP TABLE Helper;
