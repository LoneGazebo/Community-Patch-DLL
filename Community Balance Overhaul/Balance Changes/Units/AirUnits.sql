-- AIR UNITS
	-- RANGE Changes

UPDATE Units
SET Range = '6'
WHERE Type = 'UNIT_TRIPLANE';

UPDATE Units
SET AirInterceptRange = '6'
WHERE Type = 'UNIT_TRIPLANE';

UPDATE Units
SET Range = '6'
WHERE Type = 'UNIT_WWI_BOMBER';

UPDATE Units
SET Range = '8'
WHERE Type = 'UNIT_FIGHTER';

UPDATE Units
SET AirInterceptRange = '8'
WHERE Type = 'UNIT_FIGHTER';

UPDATE Units
SET Range = '8'
WHERE Type = 'UNIT_JAPANESE_ZERO';

UPDATE Units
SET AirInterceptRange = '8'
WHERE Type = 'UNIT_JAPANESE_ZERO';

UPDATE Units
SET Range = '8'
WHERE Type = 'UNIT_BOMBER';

UPDATE Units
SET Range = '10'
WHERE Type = 'UNIT_AMERICAN_B17';

UPDATE Units
SET Range = '10'
WHERE Type = 'UNIT_JET_FIGHTER';

UPDATE Units
SET AirInterceptRange = '10'
WHERE Type = 'UNIT_JET_FIGHTER';

UPDATE Units
SET Range = '12'
WHERE Type = 'UNIT_STEALTH_BOMBER';

-- Atomic Bomb Range change

UPDATE Units
SET Range = '6'
WHERE Type = 'UNIT_ATOMIC_BOMB';

	-- Move Helicopter
UPDATE Units
SET PrereqTech = 'TECH_MOBILE_TACTICS'
WHERE Type = 'UNIT_HELICOPTER_GUNSHIP';

-- Move Guided Missile
UPDATE Units
SET PrereqTech = 'TECH_ROCKETRY'
WHERE Type = 'UNIT_GUIDED_MISSILE';

-- Guided Missile range bump

UPDATE Units
SET Range = '12'
WHERE Type = 'UNIT_GUIDED_MISSILE';

INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType, Cost) VALUES ('UNIT_GUIDED_MISSILE', 'RESOURCE_OIL', '1');

-- Maintenance
UPDATE Units
SET NoMaintenance = '0'
WHERE Type = 'UNIT_GUIDED_MISSILE';

-- Fighters should obsolete with Jets, and Bombers with Stealth (helps AI make good late-game choices)

--UPDATE Units
--SET ObsoleteTech = 'TECH_LASERS'
--WHERE Type = 'UNIT_FIGHTER';

--UPDATE Units
--SET ObsoleteTech = 'TECH_STEALTH'
--WHERE Type = 'UNIT_BOMBER';

-- Tech Stuff

UPDATE Units SET PrereqTech = 'TECH_RADAR' WHERE Type = 'UNIT_JAPANESE_ZERO';

UPDATE Units SET ObsoleteTech = NULL WHERE Type = 'UNIT_JAPANESE_ZERO';


UPDATE Units SET PrereqTech = 'TECH_RADAR' WHERE Type = 'UNIT_AMERICAN_B17';

UPDATE Units SET ObsoleteTech = NULL WHERE Type = 'UNIT_AMERICAN_B17';


UPDATE Units
SET PrereqTech = 'TECH_LASERS'
WHERE Type = 'UNIT_NUCLEAR_MISSILE';

UPDATE Units
SET Special = 'SPECIALUNIT_FIGHTER'
WHERE Type = 'UNIT_STEALTH_BOMBER';

	
UPDATE Units
SET RangedCombatLimit = '300'
WHERE Type = 'UNIT_TRIPLANE';

UPDATE Units
SET RangedCombatLimit = '300'
WHERE Type = 'UNIT_WWI_BOMBER';

UPDATE Units
SET RangedCombatLimit = '300'
WHERE Type = 'UNIT_FIGHTER';

UPDATE Units
SET RangedCombatLimit = '300'
WHERE Type = 'UNIT_BOMBER';

UPDATE Units
SET RangedCombatLimit = '300'
WHERE Type = 'UNIT_JET_FIGHTER';

UPDATE Units
SET RangedCombatLimit = '300'
WHERE Type = 'UNIT_STEALTH_BOMBER';

UPDATE Units
SET RangedCombatLimit = '300'
WHERE Type = 'UNIT_JAPANESE_ZERO';

UPDATE Units
SET RangedCombatLimit = '300'
WHERE Type = 'UNIT_AMERICAN_B17';

UPDATE Units
SET RangedCombatLimit = '300'
WHERE Type = 'UNIT_GUIDED_MISSILE';


UPDATE Units
SET NoSupply = '1'
WHERE Type = 'UNIT_TRIPLANE';

UPDATE Units
SET NoSupply = '1'
WHERE Type = 'UNIT_WWI_BOMBER';

UPDATE Units
SET NoSupply = '1'
WHERE Type = 'UNIT_FIGHTER';

UPDATE Units
SET NoSupply = '1'
WHERE Type = 'UNIT_BOMBER';

UPDATE Units
SET NoSupply = '1'
WHERE Type = 'UNIT_JET_FIGHTER';

UPDATE Units
SET NoSupply = '1'
WHERE Type = 'UNIT_STEALTH_BOMBER';

UPDATE Units
SET NoSupply = '1'
WHERE Type = 'UNIT_JAPANESE_ZERO';

UPDATE Units
SET NoSupply = '1'
WHERE Type = 'UNIT_AMERICAN_B17';

UPDATE Units
SET NoSupply = '1'
WHERE Type = 'UNIT_GUIDED_MISSILE';

