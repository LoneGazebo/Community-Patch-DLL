--



--
-- Set unique unit classes
--

UPDATE Units SET Class = 'UNITCLASS_BIREME',		CombatClass = 'UNITCOMBAT_NAVALRANGED',		GoodyHutUpgradeUnitClass = 'UNITCLASS_GALLEASS',	PrereqTech = 'TECH_SAILING'
Where Type = 'UNIT_CARTHAGINIAN_QUINQUEREME';

UPDATE Units SET Class = 'UNITCLASS_BIREME',		CombatClass = 'UNITCOMBAT_NAVALRANGED',		GoodyHutUpgradeUnitClass = 'UNITCLASS_GALLEASS',	PrereqTech = 'TECH_SAILING'
Where Type = 'UNIT_BYZANTINE_DROMON';

UPDATE Units SET Class = 'UNITCLASS_FRIGATE',		CombatClass = 'UNITCOMBAT_NAVALMELEE',		GoodyHutUpgradeUnitClass = 'UNITCLASS_SUBMARINE',	PrereqTech = 'TECH_NAVIGATION'
Where Type = 'UNIT_DUTCH_SEA_BEGGAR';

UPDATE Units SET Class = 'UNITCLASS_PRIVATEER',		CombatClass = 'UNITCOMBAT_NAVALRANGED',		GoodyHutUpgradeUnitClass = 'UNITCLASS_IRONCLAD',	PrereqTech = 'TECH_NAVIGATION'
Where Type = 'UNIT_ENGLISH_SHIPOFTHELINE';

--UPDATE Units SET Class = 'UNITCLASS_MILITIA',		CombatClass = 'UNITCOMBAT_RECON',			GoodyHutUpgradeUnitClass = 'UNITCLASS_CONSCRIPT',	PrereqTech = 'TECH_BANKING'
--Where Type = 'UNIT_AMERICAN_MINUTEMAN';


UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_BIREME'		Where UnitType = 'UNIT_CARTHAGINIAN_QUINQUEREME';
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_BIREME'		Where UnitType = 'UNIT_BYZANTINE_DROMON';
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_PRIVATEER'	Where UnitType = 'UNIT_ENGLISH_SHIPOFTHELINE';


--
-- Unit Details
--

-- This CEA_Start.sql data automatically created by
-- "Units" tab of Armies spreadsheet
-- https://drive.google.com/folderview?id=0B58Ehya83q19eVlvWXdmLXZ6UUE

UPDATE Units SET Cost = Cost * 1.046, Combat = Combat * 1.067                                       WHERE Class = 'UNITCLASS_MECH';
UPDATE Units SET Cost = Cost * 1.083, Combat = Combat * 1.409                                       WHERE Class = 'UNITCLASS_XCOM_SQUAD';
UPDATE Units SET Cost = Cost * 1.481,                          RangedCombat = RangedCombat * 1.333  WHERE Class = 'UNITCLASS_GUIDED_MISSILE';
UPDATE Units SET Cost = Cost * 0.784,                          RangedCombat = RangedCombat * 0.941  WHERE Class = 'UNITCLASS_STEALTH_BOMBER';
UPDATE Units SET Cost = Cost * 0.784,                          RangedCombat = RangedCombat * 1.067  WHERE Class = 'UNITCLASS_JET_FIGHTER';
UPDATE Units SET Cost = Cost * 1.373, Combat = Combat * 1.05                                        WHERE Class = 'UNITCLASS_MODERN_ARMOR';
UPDATE Units SET Cost = Cost * 1.481, Combat = Combat * 1.111                                       WHERE Class = 'UNITCLASS_MECHANIZED_INFANTRY';
UPDATE Units SET Cost = Cost * 0.889, Combat = Combat * 1.059, RangedCombat = RangedCombat * 0.824  WHERE Class = 'UNITCLASS_BAZOOKA';
UPDATE Units SET Cost = Cost * 0.837, Combat = Combat * 1.083, RangedCombat =       Combat * 1.417  WHERE Class = 'UNITCLASS_HELICOPTER_GUNSHIP';
UPDATE Units SET Cost = Cost * 0.575, Combat = Combat * 1                                           WHERE Class = 'UNITCLASS_MOBILE_SAM';
UPDATE Units SET Cost = Cost * 0.98 , Combat = Combat * 1.111, RangedCombat = RangedCombat * 1.25   WHERE Class = 'UNITCLASS_ROCKET_ARTILLERY';
UPDATE Units SET Cost = Cost * 0.607,                          RangedCombat = RangedCombat * 0.846  WHERE Class = 'UNITCLASS_BOMBER';
UPDATE Units SET Cost = Cost * 0.607,                          RangedCombat = RangedCombat * 1.222  WHERE Class = 'UNITCLASS_FIGHTER';
UPDATE Units SET Cost = Cost * 1.111, Combat = Combat * 1.071                                       WHERE Class = 'UNITCLASS_TANK';
UPDATE Units SET Cost = Cost * 0.139, Combat = Combat * 0.154                                       WHERE Class = 'UNITCLASS_MARINE';
UPDATE Units SET Cost = Cost * 0.504, Combat = Combat * 0.769                                       WHERE Class = 'UNITCLASS_PARATROOPER';
UPDATE Units SET Cost = Cost * 0.889, Combat = Combat * 0.857                                       WHERE Class = 'UNITCLASS_INFANTRY';
UPDATE Units SET Cost = Cost * 0.635, Combat = Combat * 1    , RangedCombat = RangedCombat * 0.833  WHERE Class = 'UNITCLASS_MACHINE_GUN';
UPDATE Units SET Cost = Cost * 0.044, Combat = Combat * 0.1                                         WHERE Class = 'UNITCLASS_ANTI_TANK_GUN';
UPDATE Units SET Cost = Cost * 0.444, Combat = Combat * 0.9                                         WHERE Class = 'UNITCLASS_ANTI_AIRCRAFT_GUN';
UPDATE Units SET Cost = Cost * 0.513,                          RangedCombat = RangedCombat * 0.8    WHERE Class = 'UNITCLASS_WWI_BOMBER';
UPDATE Units SET Cost = Cost * 0.513,                          RangedCombat = RangedCombat * 1.143  WHERE Class = 'UNITCLASS_TRIPLANE';
UPDATE Units SET Cost = Cost * 0.873, Combat = Combat * 0.917                                       WHERE Class = 'UNITCLASS_WWI_TANK';
UPDATE Units SET Cost = Cost * 0.747, Combat = Combat * 0.86                                        WHERE Class = 'UNITCLASS_GREAT_WAR_INFANTRY';
UPDATE Units SET Cost = Cost * 1    , Combat = Combat * 1.429, RangedCombat = RangedCombat * 1.607  WHERE Class = 'UNITCLASS_ARTILLERY';
UPDATE Units SET Cost = Cost * 0.667, Combat = Combat * 1.333, RangedCombat = RangedCombat * 1      WHERE Class = 'UNITCLASS_GATLINGGUN';
UPDATE Units SET Cost = Cost * 0.988, Combat = Combat * 1.176                                       WHERE Class = 'UNITCLASS_CAVALRY';
UPDATE Units SET Cost = Cost * 0.914, Combat = Combat * 1.088                                       WHERE Class = 'UNITCLASS_RIFLEMAN';
UPDATE Units SET Cost = Cost * 0.721, Combat = Combat * 0.96 , RangedCombat =       Combat * 1.28   WHERE Class = 'UNITCLASS_LANCER';
UPDATE Units SET Cost = Cost * 0.991, Combat = Combat * 1.571, RangedCombat = RangedCombat * 1.65   WHERE Class = 'UNITCLASS_CANNON';
UPDATE Units SET Cost = Cost * 1.204, Combat = Combat * 1.3                                         WHERE Class = 'UNITCLASS_KNIGHT';
UPDATE Units SET Cost = Cost * 1.111, Combat = Combat * 1.143                                       WHERE Class = 'UNITCLASS_LONGSWORDSMAN';
UPDATE Units SET Cost = Cost * 0.593, Combat = Combat * 1                                           WHERE Class = 'UNITCLASS_MUSKETMAN';
UPDATE Units SET Cost = Cost * 0.787, Combat = Combat * 0.923, RangedCombat = RangedCombat * 0.944  WHERE Class = 'UNITCLASS_CROSSBOWMAN';
UPDATE Units SET Cost = Cost * 1.019, Combat = Combat * 1.25 , RangedCombat = RangedCombat * 1.571  WHERE Class = 'UNITCLASS_TREBUCHET';
UPDATE Units SET Cost = Cost * 0.741, Combat = Combat * 1.125                                       WHERE Class = 'UNITCLASS_PIKEMAN';
UPDATE Units SET Cost = Cost * 1.185, Combat = Combat * 1.333                                       WHERE Class = 'UNITCLASS_HORSEMAN';
UPDATE Units SET Cost = Cost * 1.111, Combat = Combat * 1.071                                       WHERE Class = 'UNITCLASS_SWORDSMAN';
UPDATE Units SET Cost = Cost * 0.815, Combat = Combat * 1.143, RangedCombat = RangedCombat * 1      WHERE Class = 'UNITCLASS_COMPOSITE_BOWMAN';
UPDATE Units SET Cost = Cost * 1.037, Combat = Combat * 1.286, RangedCombat = RangedCombat * 1.75   WHERE Class = 'UNITCLASS_CATAPULT';
UPDATE Units SET Cost = Cost * 0.992, Combat = Combat * 1.667, RangedCombat =       Combat * 2.167  WHERE Class = 'UNITCLASS_CHARIOT_ARCHER';
UPDATE Units SET Cost = Cost * 0.694, Combat = Combat * 1                                           WHERE Class = 'UNITCLASS_SPEARMAN';
UPDATE Units SET Cost = Cost * 0.972, Combat = Combat * 1                                           WHERE Class = 'UNITCLASS_WARRIOR';
UPDATE Units SET Cost = Cost * 1.111, Combat = Combat * 1    , RangedCombat = RangedCombat * 1      WHERE Class = 'UNITCLASS_ARCHER';
UPDATE Units SET Cost = Cost * 1.111, Combat = Combat * 1.4                                         WHERE Class = 'UNITCLASS_SCOUT';
UPDATE Units SET Cost = Cost * 1.634, Combat = 95 , RangedCombat = 125, Range = 3, CombatClass = 'UNITCOMBAT_NAVALRANGED', Moves = 8  WHERE Class = 'UNITCLASS_MISSILE_CRUISER';
UPDATE Units SET Cost = 850         , Combat = 85 , RangedCombat = 0  , Range = 0, CombatClass = 'UNITCOMBAT_NAVALMELEE' , Moves = 8  WHERE Class = 'UNITCLASS_MISSILE_DESTROYER';
UPDATE Units SET Cost = Cost * 1.229, Combat = 40 , RangedCombat = 125, Range = 1, CombatClass = 'UNITCOMBAT_SUBMARINE'  , Moves = 7  WHERE Class = 'UNITCLASS_NUCLEAR_SUBMARINE';
UPDATE Units SET Cost = Cost * 1.037, Combat = 70 , RangedCombat = 0  , Range = 0, CombatClass = 'UNITCOMBAT_CARRIER'    , Moves = 7  WHERE Class = 'UNITCLASS_CARRIER';
UPDATE Units SET Cost = Cost * 1.037, Combat = 50 , RangedCombat = 70 , Range = 3, CombatClass = 'UNITCOMBAT_NAVALRANGED', Moves = 6  WHERE Class = 'UNITCLASS_BATTLESHIP';
UPDATE Units SET Cost = Cost * 0.741, Combat = 50 , RangedCombat = 0  , Range = 0, CombatClass = 'UNITCOMBAT_NAVALMELEE' , Moves = 7  WHERE Class = 'UNITCLASS_DESTROYER';
UPDATE Units SET Cost = Cost * 0.957, Combat = 25 , RangedCombat = 75 , Range = 1, CombatClass = 'UNITCOMBAT_SUBMARINE'  , Moves = 6  WHERE Class = 'UNITCLASS_SUBMARINE';
UPDATE Units SET Cost = Cost * 0.956, Combat = 32 , RangedCombat = 43 , Range = 2, CombatClass = 'UNITCOMBAT_NAVALRANGED', Moves = 5  WHERE Class = 'UNITCLASS_IRONCLAD';
UPDATE Units SET Cost = Cost * 1.185, Combat = 32 , RangedCombat = 0  , Range = 0, CombatClass = 'UNITCOMBAT_NAVALMELEE' , Moves = 5  WHERE Class = 'UNITCLASS_FRIGATE';
UPDATE Units SET Cost = Cost * 0.961, Combat = 24 , RangedCombat = 32 , Range = 2, CombatClass = 'UNITCOMBAT_NAVALRANGED', Moves = 4  WHERE Class = 'UNITCLASS_PRIVATEER';
UPDATE Units SET Cost = Cost * 0.972, Combat = 21 , RangedCombat = 0  , Range = 0, CombatClass = 'UNITCOMBAT_NAVALMELEE' , Moves = 4  WHERE Class = 'UNITCLASS_CARAVEL';
UPDATE Units SET Cost = Cost * 1.167, Combat = 16 , RangedCombat = 21 , Range = 2, CombatClass = 'UNITCOMBAT_NAVALRANGED', Moves = 3  WHERE Class = 'UNITCLASS_GALLEASS';
UPDATE Units SET Cost = Cost * 1.235, Combat = 13 , RangedCombat = 0  , Range = 0, CombatClass = 'UNITCOMBAT_NAVALMELEE' , Moves = 4  WHERE Class = 'UNITCLASS_TRIREME';
UPDATE Units SET Cost = 100         , Combat = 10 , RangedCombat = 13 , Range = 2, CombatClass = 'UNITCOMBAT_NAVALRANGED', Moves = 3  WHERE Class = 'UNITCLASS_BIREME';
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

UPDATE Units SET Combat = Combat * 1.25, RangedCombat = RangedCombat * 1.25, Moves = Moves + 1 WHERE Type IN (
	'UNIT_BYZANTINE_DROMON'			,
	'UNIT_CARTHAGINIAN_QUINQUEREME'	,
	'UNIT_VENETIAN_GALLEASS'		,
	'UNIT_DUTCH_SEA_BEGGAR'			,
	'UNIT_ENGLISH_SHIPOFTHELINE'	,
	'UNIT_ROMAN_LIBURNA'			,
	'UNIT_HUN_BATTERING_RAM'		,
	'UNIT_VENETIAN_GALLEASS'	
);

UPDATE Units SET Combat = Combat * 1.5, RangedCombat = RangedCombat * 1.5, Moves = Moves - 1 WHERE Type IN (
	'UNIT_KOREAN_TURTLE_SHIP'		
);

UPDATE Units SET       Combat = ROUND(           Combat, 0)       WHERE       Combat <> 0;
UPDATE Units SET RangedCombat = ROUND(     RangedCombat, 0)       WHERE RangedCombat <> 0;
UPDATE Units SET         Cost = ROUND((Cost * 2) / 10, 0) * 10  WHERE         Cost  > 0 AND NOT CLASS IN (
	'UNITCLASS_BIREME',
	'UNITCLASS_SHIP_OF_THE_LINE',
	'UNITCLASS_MISSILE_DESTROYER'
);


--
-- Combat Classes
--

UPDATE Units
SET CombatClass = 'UNITCOMBAT_MELEE'
WHERE Class IN (
	'UNITCLASS_GATLINGGUN'	,
	'UNITCLASS_MACHINE_GUN'	,
	'UNITCLASS_BAZOOKA'		
);

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
	'UNITCLASS_LANCER'				
);

UPDATE Units SET Range = 1 WHERE Class IN (
	'UNITCLASS_HELICOPTER_GUNSHIP'	,
	'UNITCLASS_CHARIOT_ARCHER'		,
	'UNITCLASS_LANCER'				
);

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
SELECT DISTINCT Type, 'PROMOTION_ONLY_DEFENSIVE'
FROM Units WHERE Class IN (
	'UNITCLASS_HELICOPTER_GUNSHIP'	,
	'UNITCLASS_LANCER'
);

UPDATE Units
SET CombatClass = 'UNITCOMBAT_RECON'
WHERE Class IN (
	'UNITCLASS_SPEARMAN'			,
	'UNITCLASS_PIKEMAN'				,
	'UNITCLASS_MUSKETMAN'			,
	'UNITCLASS_GATLINGGUN'			,
	'UNITCLASS_MACHINE_GUN'			,
	'UNITCLASS_BAZOOKA'				
);

--
-- Upgrades
--

DELETE FROM Unit_ClassUpgrades WHERE UnitType IN (SELECT Type FROM Units WHERE Domain = 'DOMAIN_SEA');

INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_CARAVEL',              Type FROM Units WHERE Class = 'UNITCLASS_TRIREME';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_FRIGATE',              Type FROM Units WHERE Class = 'UNITCLASS_CARAVEL';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_DESTROYER',            Type FROM Units WHERE Class = 'UNITCLASS_FRIGATE';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_MISSILE_DESTROYER',    Type FROM Units WHERE Class = 'UNITCLASS_DESTROYER';

INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_GALLEASS',             Type FROM Units WHERE Class = 'UNITCLASS_BIREME';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_PRIVATEER',            Type FROM Units WHERE Class = 'UNITCLASS_GALLEASS';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_IRONCLAD',             Type FROM Units WHERE Class = 'UNITCLASS_PRIVATEER';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_BATTLESHIP',           Type FROM Units WHERE Class = 'UNITCLASS_IRONCLAD';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_MISSILE_CRUISER',      Type FROM Units WHERE Class = 'UNITCLASS_BATTLESHIP';

INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_NUCLEAR_SUBMARINE',    Type FROM Units WHERE Class = 'UNITCLASS_SUBMARINE';


DELETE FROM Unit_ClassUpgrades WHERE UnitType IN (
SELECT DISTINCT Type FROM Units WHERE Class IN (
	'UNITCLASS_SCOUT'				,
	'UNITCLASS_PIKEMAN'				
));

INSERT INTO Unit_ClassUpgrades (UnitType, UnitClassType) SELECT DISTINCT Type, 'UNITCLASS_SPEARMAN'		FROM Units WHERE Class = 'UNITCLASS_SCOUT';
INSERT INTO Unit_ClassUpgrades (UnitType, UnitClassType) SELECT DISTINCT Type, 'UNITCLASS_MUSKETMAN'	FROM Units WHERE Class = 'UNITCLASS_PIKEMAN';

--
-- Resources
--

DELETE FROM Unit_ResourceQuantityRequirements
WHERE UnitType IN (
	'UNIT_CATAPULT',
	'UNIT_ROMAN_BALLISTA',
	'UNIT_TREBUCHET',
	'UNIT_ROCKET_ARTILLERY',
	'UNIT_HELICOPTER_GUNSHIP',
	'UNIT_FRIGATE'
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

INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType, Cost)
SELECT DISTINCT Type, 'RESOURCE_IRON', 1
FROM Units WHERE Class IN (
	'UNITCLASS_PRIVATEER'			,
	'UNITCLASS_SHIP_OF_THE_LINE'
);

UPDATE Unit_ResourceQuantityRequirements
SET ResourceType = 'RESOURCE_IRON', Cost = 1
WHERE UnitType IN (SELECT DISTINCT Type FROM Units WHERE Class IN (
	'UNITCLASS_IRONCLAD'
));


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

--
-- Other
--

-- This CEA_Start.sql data automatically created by:
-- UnitFormulas tab of GEM_Details.xlsspreadsheet (in mod folder).
UPDATE Eras SET EmbarkedUnitDefense = 7 WHERE ID = 0;
UPDATE Eras SET EmbarkedUnitDefense = 10 WHERE ID = 1;
UPDATE Eras SET EmbarkedUnitDefense = 13 WHERE ID = 2;
UPDATE Eras SET EmbarkedUnitDefense = 17 WHERE ID = 3;
UPDATE Eras SET EmbarkedUnitDefense = 22 WHERE ID = 4;
UPDATE Eras SET EmbarkedUnitDefense = 28 WHERE ID = 5;
UPDATE Eras SET EmbarkedUnitDefense = 35 WHERE ID = 6;
UPDATE Eras SET EmbarkedUnitDefense = 46 WHERE ID = 7;



UPDATE LoadedFile SET Value=1 WHERE Type='CEA__Start.sql';