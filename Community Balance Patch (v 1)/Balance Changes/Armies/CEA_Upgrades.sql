-- This CEA_Upgrades.sql data automatically created by
-- Unit_Formulas tab of "Armies" spreadsheet
-- https://drive.google.com/folderview?id=0B58Ehya83q19eVlvWXdmLXZ6UUE

DELETE FROM Unit_ClassUpgrades;


INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_SWORDSMAN',            Type FROM Units WHERE Class = 'UNITCLASS_WARRIOR';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_LONGSWORDSMAN',        Type FROM Units WHERE Class = 'UNITCLASS_SWORDSMAN';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_RIFLEMAN',             Type FROM Units WHERE Class = 'UNITCLASS_LONGSWORDSMAN';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_GREAT_WAR_INFANTRY',   Type FROM Units WHERE Class = 'UNITCLASS_RIFLEMAN';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_INFANTRY',             Type FROM Units WHERE Class = 'UNITCLASS_GREAT_WAR_INFANTRY';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_MECHANIZED_INFANTRY',  Type FROM Units WHERE Class = 'UNITCLASS_INFANTRY';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_XCOM_SQUAD',           Type FROM Units WHERE Class = 'UNITCLASS_MECHANIZED_INFANTRY';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_KNIGHT',               Type FROM Units WHERE Class = 'UNITCLASS_HORSEMAN';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_CAVALRY',              Type FROM Units WHERE Class = 'UNITCLASS_KNIGHT';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_WWI_TANK',             Type FROM Units WHERE Class = 'UNITCLASS_CAVALRY';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_TANK',                 Type FROM Units WHERE Class = 'UNITCLASS_WWI_TANK';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_MODERN_ARMOR',         Type FROM Units WHERE Class = 'UNITCLASS_TANK';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_MECH',                 Type FROM Units WHERE Class = 'UNITCLASS_MODERN_ARMOR';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_SPEARMAN',             Type FROM Units WHERE Class = 'UNITCLASS_SCOUT';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_PIKEMAN',              Type FROM Units WHERE Class = 'UNITCLASS_SPEARMAN';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_MUSKETMAN',            Type FROM Units WHERE Class = 'UNITCLASS_PIKEMAN';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_MACHINE_GUN',          Type FROM Units WHERE Class = 'UNITCLASS_GATLINGGUN';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_BAZOOKA',              Type FROM Units WHERE Class = 'UNITCLASS_MACHINE_GUN';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_COMPOSITE_BOWMAN',     Type FROM Units WHERE Class = 'UNITCLASS_ARCHER';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_CROSSBOWMAN',          Type FROM Units WHERE Class = 'UNITCLASS_COMPOSITE_BOWMAN';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_TREBUCHET',            Type FROM Units WHERE Class = 'UNITCLASS_CATAPULT';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_CANNON',               Type FROM Units WHERE Class = 'UNITCLASS_TREBUCHET';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_ARTILLERY',            Type FROM Units WHERE Class = 'UNITCLASS_CANNON';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_ROCKET_ARTILLERY',     Type FROM Units WHERE Class = 'UNITCLASS_ARTILLERY';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_FIGHTER',              Type FROM Units WHERE Class = 'UNITCLASS_TRIPLANE';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_JET_FIGHTER',          Type FROM Units WHERE Class = 'UNITCLASS_FIGHTER';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_BOMBER',               Type FROM Units WHERE Class = 'UNITCLASS_WWI_BOMBER';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_STEALTH_BOMBER',       Type FROM Units WHERE Class = 'UNITCLASS_BOMBER';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_LANCER',               Type FROM Units WHERE Class = 'UNITCLASS_CHARIOT_ARCHER';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_HELICOPTER_GUNSHIP',   Type FROM Units WHERE Class = 'UNITCLASS_LANCER';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_MOBILE_SAM',           Type FROM Units WHERE Class = 'UNITCLASS_ANTI_AIRCRAFT_GUN';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_BIREME',               Type FROM Units WHERE Class = 'UNITCLASS_GALLEY';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_GALLEASS',             Type FROM Units WHERE Class = 'UNITCLASS_BIREME';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_PRIVATEER',            Type FROM Units WHERE Class = 'UNITCLASS_GALLEASS';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_IRONCLAD',             Type FROM Units WHERE Class = 'UNITCLASS_PRIVATEER';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_BATTLESHIP',           Type FROM Units WHERE Class = 'UNITCLASS_IRONCLAD';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_MISSILE_CRUISER',      Type FROM Units WHERE Class = 'UNITCLASS_BATTLESHIP';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_CARAVEL',              Type FROM Units WHERE Class = 'UNITCLASS_TRIREME';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_FRIGATE',              Type FROM Units WHERE Class = 'UNITCLASS_CARAVEL';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_DESTROYER',            Type FROM Units WHERE Class = 'UNITCLASS_FRIGATE';
-- INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_CARRIER',              Type FROM Units WHERE Class = 'UNITCLASS_DESTROYER';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_MISSILE_DESTROYER',    Type FROM Units WHERE Class = 'UNITCLASS_DESTROYER';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_NUCLEAR_SUBMARINE',    Type FROM Units WHERE Class = 'UNITCLASS_SUBMARINE';


INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_GATLINGGUN',           Type FROM Units WHERE Class = 'UNITCLASS_MUSKETMAN';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_GATLINGGUN',           Type FROM Units WHERE Class = 'UNITCLASS_CROSSBOWMAN';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) SELECT DISTINCT 'UNITCLASS_HORSEMAN',             Type FROM Units WHERE Class = 'UNITCLASS_CHARIOT_ARCHER';
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) VALUES         ('UNITCLASS_GALLEASS',            'UNIT_CARTHAGINIAN_QUINQUEREME');
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) VALUES         ('UNITCLASS_GALLEASS',            'UNIT_BYZANTINE_DROMON');
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) VALUES         ('UNITCLASS_KNIGHT',              'UNIT_INDIAN_WARELEPHANT');
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) VALUES         ('UNITCLASS_SPEARMAN',            'UNIT_AZTEC_JAGUAR');
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) VALUES         ('UNITCLASS_SPEARMAN',            'UNIT_POLYNESIAN_MAORI_WARRIOR');
INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) VALUES         ('UNITCLASS_SPEARMAN',            'UNIT_BARBARIAN_WARRIOR');


UPDATE LoadedFile SET Value=1 WHERE Type='CEA_Upgrades.sql';
