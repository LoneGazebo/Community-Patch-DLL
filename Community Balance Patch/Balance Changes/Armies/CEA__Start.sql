--



--
-- Set unique unit classes
--

UPDATE Units SET Class = 'UNITCLASS_LANCER',		CombatClass = 'UNITCOMBAT_MOUNTED',		GoodyHutUpgradeUnitClass = 'UNITCLASS_WWI_TANK',	PrereqTech = 'TECH_MILITARY_SCIENCE'
Where Type = 'UNIT_AUSTRIAN_HUSSAR';

UPDATE Units SET Class = 'UNITCLASS_PRIVATEER',		CombatClass = 'UNITCOMBAT_NAVALMELEE',		GoodyHutUpgradeUnitClass = 'UNITCLASS_DESTROYER',	PrereqTech = 'TECH_NAVIGATION'
Where Type = 'UNIT_DUTCH_SEA_BEGGAR';

UPDATE Units SET Class = 'UNITCLASS_FRIGATE',		CombatClass = 'UNITCOMBAT_NAVALRANGED',		GoodyHutUpgradeUnitClass = 'UNITCLASS_IRONCLAD',	PrereqTech = 'TECH_NAVIGATION'
Where Type = 'UNIT_ENGLISH_SHIPOFTHELINE';

UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_FRIGATE'	    Where UnitType = 'UNIT_ENGLISH_SHIPOFTHELINE';


--
-- Unit Details
--

-- This CEA_Start.sql data automatically created by
-- "Units" tab of Armies spreadsheet
-- https://drive.google.com/folderview?id=0B58Ehya83q19eVlvWXdmLXZ6UUE

UPDATE Units SET Cost = Cost * 1.212, Combat = Combat * 1.067                                       WHERE Class = 'UNITCLASS_MECH';
UPDATE Units SET Cost = Cost * 1.306, Combat = Combat * 1.250                                       WHERE Class = 'UNITCLASS_XCOM_SQUAD';
UPDATE Units SET Cost = Cost * 1.667,                          RangedCombat = RangedCombat * 1.250  WHERE Class = 'UNITCLASS_GUIDED_MISSILE';
UPDATE Units SET Cost = Cost * 0.889,                          RangedCombat = RangedCombat * 0.941  WHERE Class = 'UNITCLASS_STEALTH_BOMBER';
UPDATE Units SET Cost = Cost * 0.889,                          RangedCombat = RangedCombat * 1.067  WHERE Class = 'UNITCLASS_JET_FIGHTER';
UPDATE Units SET Cost = Cost * 1.294, Combat = Combat * 1.100                                       WHERE Class = 'UNITCLASS_MODERN_ARMOR';
UPDATE Units SET Cost = Cost * 1.274, Combat = Combat * 1.056                                       WHERE Class = 'UNITCLASS_MECHANIZED_INFANTRY';
UPDATE Units SET Cost = Cost * 0.933, Combat = Combat * 0.988, RangedCombat = RangedCombat * 0.788  WHERE Class = 'UNITCLASS_BAZOOKA';
UPDATE Units SET Cost = Cost * 1.111, Combat = Combat * 0.833, RangedCombat =       Combat * 1.667  WHERE Class = 'UNITCLASS_HELICOPTER_GUNSHIP';
UPDATE Units SET Cost = Cost * 0.641, Combat = Combat * 1                                           WHERE Class = 'UNITCLASS_MOBILE_SAM';
UPDATE Units SET Cost = Cost * 0.980, Combat = Combat * 1.111, RangedCombat = RangedCombat * 1.25   WHERE Class = 'UNITCLASS_ROCKET_ARTILLERY';
UPDATE Units SET Cost = Cost * 0.696,                          RangedCombat = RangedCombat * 0.846  WHERE Class = 'UNITCLASS_BOMBER';
UPDATE Units SET Cost = Cost * 0.696,                          RangedCombat = RangedCombat * 1.222  WHERE Class = 'UNITCLASS_FIGHTER';
UPDATE Units SET Cost = Cost * 1.007, Combat = Combat * 1.071                                       WHERE Class = 'UNITCLASS_TANK';
UPDATE Units SET Cost = Cost * 0.139, Combat = Combat * 0.154                                       WHERE Class = 'UNITCLASS_MARINE';
UPDATE Units SET Cost = Cost * 0.652, Combat = Combat * 0.846                                       WHERE Class = 'UNITCLASS_PARATROOPER';
UPDATE Units SET Cost = Cost * 0.889, Combat = Combat * 0.857                                       WHERE Class = 'UNITCLASS_INFANTRY';
UPDATE Units SET Cost = Cost * 0.698, Combat = Combat * 0.917, RangedCombat = RangedCombat * 0.733  WHERE Class = 'UNITCLASS_MACHINE_GUN';
UPDATE Units SET Cost = Cost * 0.044, Combat = Combat * 0.1                                         WHERE Class = 'UNITCLASS_ANTI_TANK_GUN';
UPDATE Units SET Cost = Cost * 0.504, Combat = Combat * 0.9                                         WHERE Class = 'UNITCLASS_ANTI_AIRCRAFT_GUN';
UPDATE Units SET Cost = Cost * 0.530,                          RangedCombat = RangedCombat * 0.720  WHERE Class = 'UNITCLASS_WWI_BOMBER';
UPDATE Units SET Cost = Cost * 0.530,                          RangedCombat = RangedCombat * 1.029  WHERE Class = 'UNITCLASS_TRIPLANE';
UPDATE Units SET Cost = Cost * 0.873, Combat = Combat * 0.917                                       WHERE Class = 'UNITCLASS_WWI_TANK';
UPDATE Units SET Cost = Cost * 0.747, Combat = Combat * 0.86                                        WHERE Class = 'UNITCLASS_GREAT_WAR_INFANTRY';
UPDATE Units SET Cost = Cost * 1.000, Combat = Combat * 1.381, RangedCombat = RangedCombat * 1.607  WHERE Class = 'UNITCLASS_ARTILLERY';
UPDATE Units SET Cost = Cost * 0.667, Combat = Combat * 1.133, RangedCombat = RangedCombat * 0.9    WHERE Class = 'UNITCLASS_GATLINGGUN';
UPDATE Units SET Cost = Cost * 0.938, Combat = Combat * 0.706, RangedCombat =       Combat * 1.324  WHERE Class = 'UNITCLASS_CAVALRY';
UPDATE Units SET Cost = Cost * 0.840, Combat = Combat * 1.000                                       WHERE Class = 'UNITCLASS_RIFLEMAN';
UPDATE Units SET Cost = Cost * 1.141, Combat = Combat * 1.52                                        WHERE Class = 'UNITCLASS_LANCER';
UPDATE Units SET Cost = Cost * 0.991, Combat = Combat * 1.571, RangedCombat = RangedCombat * 1.65   WHERE Class = 'UNITCLASS_CANNON';
UPDATE Units SET Cost = Cost * 1.204, Combat = Combat * 1.3                                         WHERE Class = 'UNITCLASS_KNIGHT';
UPDATE Units SET Cost = Cost * 1.111, Combat = Combat * 1.143                                       WHERE Class = 'UNITCLASS_LONGSWORDSMAN';
UPDATE Units SET Cost = Cost * 0.889, Combat = Combat * 1                                           WHERE Class = 'UNITCLASS_MUSKETMAN';
UPDATE Units SET Cost = Cost * 0.787, Combat = Combat * 0.923, RangedCombat = RangedCombat * 0.944  WHERE Class = 'UNITCLASS_CROSSBOWMAN';
UPDATE Units SET Cost = Cost * 1.019, Combat = Combat * 1.25 , RangedCombat = RangedCombat * 1.571  WHERE Class = 'UNITCLASS_TREBUCHET';
UPDATE Units SET Cost = Cost * 0.864, Combat = Combat * 1.125                                       WHERE Class = 'UNITCLASS_PIKEMAN';
UPDATE Units SET Cost = Cost * 1.185, Combat = Combat * 1.333                                       WHERE Class = 'UNITCLASS_HORSEMAN';
UPDATE Units SET Cost = Cost * 1.111, Combat = Combat * 1.071                                       WHERE Class = 'UNITCLASS_SWORDSMAN';
UPDATE Units SET Cost = Cost * 0.815, Combat = Combat * 1.143, RangedCombat = RangedCombat * 1      WHERE Class = 'UNITCLASS_COMPOSITE_BOWMAN';
UPDATE Units SET Cost = Cost * 1.037, Combat = Combat * 1.286, RangedCombat = RangedCombat * 1.75   WHERE Class = 'UNITCLASS_CATAPULT';
UPDATE Units SET Cost = Cost * 1.290, Combat = Combat * 1.333, RangedCombat = RangedCombat * 1.5    WHERE Class = 'UNITCLASS_CHARIOT_ARCHER';
UPDATE Units SET Cost = Cost * 0.794, Combat = Combat * 1                                           WHERE Class = 'UNITCLASS_SPEARMAN';
UPDATE Units SET Cost = Cost * 0.972, Combat = Combat * 1                                           WHERE Class = 'UNITCLASS_WARRIOR';
UPDATE Units SET Cost = Cost * 1.111, Combat = Combat * 1    , RangedCombat = RangedCombat * 1      WHERE Class = 'UNITCLASS_ARCHER';
UPDATE Units SET Cost = Cost * 1.111, Combat = Combat * 1.2                                         WHERE Class = 'UNITCLASS_SCOUT';
UPDATE Units SET Cost = Cost * 1.480, Combat = 95 , RangedCombat = 125, Range = 3, CombatClass = 'UNITCOMBAT_NAVALRANGED', Moves = 8  WHERE Class = 'UNITCLASS_MISSILE_CRUISER';
UPDATE Units SET Cost = Cost * 1.230, Combat = 40 , RangedCombat = 125, Range = 1, CombatClass = 'UNITCOMBAT_SUBMARINE'  , Moves = 7  WHERE Class = 'UNITCLASS_NUCLEAR_SUBMARINE';
UPDATE Units SET Cost = Cost * 0.978, Combat = 66 , RangedCombat = 0  , Range = 3, CombatClass = 'UNITCOMBAT_CARRIER'    , Moves = 7  WHERE Class = 'UNITCLASS_CARRIER';
UPDATE Units SET Cost = Cost * 1.037, Combat = 50 , RangedCombat = 70 , Range = 3, CombatClass = 'UNITCOMBAT_NAVALRANGED', Moves = 6  WHERE Class = 'UNITCLASS_BATTLESHIP';
UPDATE Units SET Cost = Cost * 0.741, Combat = 50 , RangedCombat = 0  , Range = 0, CombatClass = 'UNITCOMBAT_NAVALMELEE' , Moves = 7  WHERE Class = 'UNITCLASS_DESTROYER';
UPDATE Units SET Cost = Cost * 0.957, Combat = 25 , RangedCombat = 75 , Range = 1, CombatClass = 'UNITCOMBAT_SUBMARINE'  , Moves = 6  WHERE Class = 'UNITCLASS_SUBMARINE';
UPDATE Units SET Cost = Cost * 0.956, Combat = 32 , RangedCombat = 43 , Range = 2, CombatClass = 'UNITCOMBAT_NAVALRANGED', Moves = 4  WHERE Class = 'UNITCLASS_IRONCLAD';
UPDATE Units SET Cost = Cost * 1.185, Combat = 32 , RangedCombat = 0  , Range = 0, CombatClass = 'UNITCOMBAT_NAVALMELEE' , Moves = 5  WHERE Class = 'UNITCLASS_PRIVATEER';
UPDATE Units SET Cost = Cost * 0.961, Combat = 24 , RangedCombat = 32 , Range = 2, CombatClass = 'UNITCOMBAT_NAVALRANGED', Moves = 4  WHERE Class = 'UNITCLASS_FRIGATE';
UPDATE Units SET Cost = Cost * 0.972, Combat = 21 , RangedCombat = 0  , Range = 0, CombatClass = 'UNITCOMBAT_NAVALMELEE' , Moves = 4  WHERE Class = 'UNITCLASS_CARAVEL';
UPDATE Units SET Cost = Cost * 1.167, Combat = 16 , RangedCombat = 21 , Range = 2, CombatClass = 'UNITCOMBAT_NAVALRANGED', Moves = 3  WHERE Class = 'UNITCLASS_GALLEASS';
UPDATE Units SET Cost = Cost * 1.235, Combat = 13 , RangedCombat = 0  , Range = 0, CombatClass = 'UNITCOMBAT_NAVALMELEE' , Moves = 4  WHERE Class = 'UNITCLASS_TRIREME';
UPDATE Units SET Cost = 80          , Combat = 8  , RangedCombat = 10 , Range = 2, CombatClass = 'UNITCOMBAT_NAVALRANGED', Moves = 3  WHERE Class = 'UNITCLASS_GALLEY';


--
-- Civilians
--

UPDATE Units SET Cost = Cost * 0.500 WHERE Class = 'UNITCLASS_WORKBOAT';
UPDATE Units SET Cost = Cost * 0.200 WHERE Class = 'UNITCLASS_CARAVAN';
UPDATE Units SET Cost = Cost * 0.500 WHERE Class = 'UNITCLASS_CARGO_SHIP';

--
-- Global Mods
--

UPDATE Units SET Combat = Combat * 1.15, RangedCombat = RangedCombat * 1.15, Moves = Moves + 1 WHERE Type IN (
	'UNIT_BYZANTINE_DROMON'			,
	'UNIT_CARTHAGINIAN_QUINQUEREME'	,
	'UNIT_VENETIAN_GALLEASS'		,
	'UNIT_DUTCH_SEA_BEGGAR'			,
	'UNIT_ENGLISH_SHIPOFTHELINE'	,
	'UNIT_ROMAN_LIBURNA'			,
	'UNIT_VENETIAN_GALLEASS'	
);

UPDATE Units SET Combat = Combat * 1.5, RangedCombat = RangedCombat * 1.5, Moves = Moves - 1 WHERE Type IN (
	'UNIT_KOREAN_TURTLE_SHIP'		
);

UPDATE Units SET       Combat = ROUND(           Combat, 0)       WHERE       Combat <> 0;
UPDATE Units SET RangedCombat = ROUND(     RangedCombat, 0)       WHERE RangedCombat <> 0;

--
-- Combat Classes
--


/*
Mounted Archers need a combat class,
but we cannot associate combat animations with a new combat class.
I therefore reassigned the low-importance "gun" class to serve as mounted archers.

-- Thalassicus
*/

UPDATE Units
SET CombatClass = 'UNITCOMBAT_MELEE'
WHERE CombatClass = 'UNITCOMBAT_GUN';

UPDATE Units
SET CombatClass = 'UNITCOMBAT_GUN'
WHERE CombatClass = 'UNITCOMBAT_ARCHER' AND MoveRate NOT IN (
	'BIPED'			,
	'HEAVY_BIPED'	
);

UPDATE Units
SET CombatClass = 'UNITCOMBAT_GUN'	
WHERE Class IN (
	'UNITCLASS_CAVALRY'				
);

UPDATE Units SET Range = 1 WHERE Class IN (
	'UNITCLASS_HELICOPTER_GUNSHIP'	,
	'UNITCLASS_CHARIOT_ARCHER'		,
	'UNITCLASS_CAVALRY'				
);

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
SELECT DISTINCT Type, 'PROMOTION_ONLY_DEFENSIVE'
FROM Units WHERE Class IN (
	'UNITCLASS_HELICOPTER_GUNSHIP'	,
	'UNITCLASS_CAVALRY'
);
/*
UPDATE Units
SET CombatClass = 'UNITCOMBAT_RECON'
WHERE Class IN (
	'UNITCLASS_SPEARMAN'			,
	'UNITCLASS_PIKEMAN'				
);
*/

UPDATE Units
SET CombatClass = 'UNITCOMBAT_MELEE'	
WHERE Class IN (
	'UNIT_HUN_BATTERING_RAM'				
);


--
-- Upgrades
--


--
-- Resources
--

DELETE FROM Unit_ResourceQuantityRequirements
WHERE UnitType IN (
	'UNIT_ROCKET_ARTILLERY',
	'UNIT_HELICOPTER_GUNSHIP'
);

UPDATE Unit_ResourceQuantityRequirements
SET ResourceType = 'RESOURCE_ALUMINUM'
WHERE UnitType IN (
	'UNIT_TRIPLANE',
	'UNIT_WWI_BOMBER',
	'UNIT_BOMBER',
	'UNIT_AMERICAN_B17',
	'UNIT_FIGHTER',
	'UNIT_JAPANESE_ZERO',
	'UNIT_JET_FIGHTER',
	'UNIT_STEALTH_BOMBER'
);

UPDATE Unit_ResourceQuantityRequirements
SET ResourceType = 'RESOURCE_OIL'
WHERE UnitType IN (
	'UNIT_WWI_TANK',
	'UNIT_TANK',
	'UNIT_MODERN_ARMOR',
	'UNIT_BATTLESHIP'
);

INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType)
SELECT DISTINCT Type, 'RESOURCE_OIL'
FROM Units WHERE Class IN (
	'UNITCLASS_MISSILE_CRUISER'
);

UPDATE Unit_ResourceQuantityRequirements
SET ResourceType = 'RESOURCE_IRON', Cost = 1
WHERE UnitType IN (SELECT DISTINCT Type FROM Units WHERE Class IN (
	'UNITCLASS_IRONCLAD'
);


--
-- Conquest
--

UPDATE Buildings
SET ConquestProb = 100
WHERE HurryCostModifier != -1;

UPDATE Buildings
SET ConquestProb = 0
WHERE BuildingClass IN (
	'BUILDINGCLASS_COURTHOUSE',
	'BUILDINGCLASS_CASTLE',
	'BUILDINGCLASS_ARSENAL',
	'BUILDINGCLASS_MILITARY_BASE',
	'BUILDINGCLASS_FACTORY',
	'BUILDINGCLASS_SOLAR_PLANT',
	'BUILDINGCLASS_NUCLEAR_PLANT'
);

UPDATE Buildings
SET ConquestProb = 25
WHERE BuildingClass IN (
	'BUILDINGCLASS_WALLS'
);

/*
UPDATE Buildings
SET ConquestProb = 50
WHERE BuildingClass IN (
	'BUILDINGCLASS_LIBRARY',
	'BUILDINGCLASS_COLOSSEUM',
	'BUILDINGCLASS_THEATRE',
	'BUILDINGCLASS_STADIUM',
	'BUILDINGCLASS_MARKET',
	'BUILDINGCLASS_BANK',
	'BUILDINGCLASS_STOCK_EXCHANGE',
	'BUILDINGCLASS_MINT',
	'BUILDINGCLASS_HARBOR',
	'BUILDINGCLASS_WAREHOUSE'
);
*/

--
-- Other
--

-- This CEA_Start.sql data automatically created by:
-- UnitFormulas tab of GEM_Details.xlsspreadsheet (in mod folder).
UPDATE Eras SET EmbarkedUnitDefense = 7 WHERE ID = 0;
UPDATE Eras SET EmbarkedUnitDefense = 10 WHERE ID = 1;
UPDATE Eras SET EmbarkedUnitDefense = 14 WHERE ID = 2;
UPDATE Eras SET EmbarkedUnitDefense = 18 WHERE ID = 3;
UPDATE Eras SET EmbarkedUnitDefense = 24 WHERE ID = 4;
UPDATE Eras SET EmbarkedUnitDefense = 32 WHERE ID = 5;
UPDATE Eras SET EmbarkedUnitDefense = 40 WHERE ID = 6;
UPDATE Eras SET EmbarkedUnitDefense = 48 WHERE ID = 7;



UPDATE LoadedFile SET Value=1 WHERE Type='CEA__Start.sql';
