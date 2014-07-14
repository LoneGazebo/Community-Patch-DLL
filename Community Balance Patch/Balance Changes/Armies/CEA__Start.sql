--


UPDATE Units SET Cost = Cost * 1.212, Combat = Combat * 1.067                                       WHERE Class = 'UNITCLASS_MECH';
UPDATE Units SET Cost = Cost * 1.306, Combat = Combat * 1.250                                       WHERE Class = 'UNITCLASS_XCOM_SQUAD';
UPDATE Units SET Cost = Cost * 1.667,                          RangedCombat = RangedCombat * 1.250  WHERE Class = 'UNITCLASS_GUIDED_MISSILE';
UPDATE Units SET Cost = Cost * 0.889,                          RangedCombat = RangedCombat * 0.941  WHERE Class = 'UNITCLASS_STEALTH_BOMBER';
UPDATE Units SET Cost = Cost * 0.889,                          RangedCombat = RangedCombat * 1.067  WHERE Class = 'UNITCLASS_JET_FIGHTER';
UPDATE Units SET Cost = Cost * 1.294, Combat = Combat * 1.100                                       WHERE Class = 'UNITCLASS_MODERN_ARMOR';
UPDATE Units SET Cost = Cost * 1.274, Combat = Combat * 1.056                                       WHERE Class = 'UNITCLASS_MECHANIZED_INFANTRY';
UPDATE Units SET Cost = Cost * 0.933, Combat = Combat * 0.988, RangedCombat = RangedCombat * 0.788  WHERE Class = 'UNITCLASS_BAZOOKA';
UPDATE Units SET Cost = Cost * 0.784, Combat = Combat * 1.167,                                      WHERE Class = 'UNITCLASS_HELICOPTER_GUNSHIP';
UPDATE Units SET Cost = Cost * 0.641, Combat = Combat * 1                                           WHERE Class = 'UNITCLASS_MOBILE_SAM';
UPDATE Units SET Cost = Cost * 0.980, Combat = Combat * 1.111, RangedCombat = RangedCombat * 1.25   WHERE Class = 'UNITCLASS_ROCKET_ARTILLERY';
UPDATE Units SET Cost = Cost * 0.696,                          RangedCombat = RangedCombat * 0.846  WHERE Class = 'UNITCLASS_BOMBER';
UPDATE Units SET Cost = Cost * 0.696,                          RangedCombat = RangedCombat * 1.222  WHERE Class = 'UNITCLASS_FIGHTER';
UPDATE Units SET Cost = Cost * 1.007, Combat = Combat * 1.071                                       WHERE Class = 'UNITCLASS_TANK';
UPDATE Units SET Cost = Cost * 0.764, Combat = Combat * 1.000                                       WHERE Class = 'UNITCLASS_MARINE';
UPDATE Units SET Cost = Cost * 0.652, Combat = Combat * 0.846                                       WHERE Class = 'UNITCLASS_PARATROOPER';
UPDATE Units SET Cost = Cost * 0.889, Combat = Combat * 0.857                                       WHERE Class = 'UNITCLASS_INFANTRY';
UPDATE Units SET Cost = Cost * 0.698, Combat = Combat * 0.917, RangedCombat = RangedCombat * 0.733  WHERE Class = 'UNITCLASS_MACHINE_GUN';
UPDATE Units SET Cost = Cost * 0.637, Combat = Combat * 1.000                                       WHERE Class = 'UNITCLASS_ANTI_TANK_GUN';
UPDATE Units SET Cost = Cost * 0.504, Combat = Combat * 0.9                                         WHERE Class = 'UNITCLASS_ANTI_AIRCRAFT_GUN';
UPDATE Units SET Cost = Cost * 0.530,                          RangedCombat = RangedCombat * 0.720  WHERE Class = 'UNITCLASS_WWI_BOMBER';
UPDATE Units SET Cost = Cost * 0.530,                          RangedCombat = RangedCombat * 1.029  WHERE Class = 'UNITCLASS_TRIPLANE';
UPDATE Units SET Cost = Cost * 0.873, Combat = Combat * 0.917                                       WHERE Class = 'UNITCLASS_WWI_TANK';
UPDATE Units SET Cost = Cost * 0.781, Combat = Combat * 0.90                                        WHERE Class = 'UNITCLASS_GREAT_WAR_INFANTRY';
UPDATE Units SET Cost = Cost * 0.933, Combat = Combat * 1.381, RangedCombat = RangedCombat * 1.500  WHERE Class = 'UNITCLASS_ARTILLERY';
UPDATE Units SET Cost = Cost * 0.667, Combat = Combat * 1.133, RangedCombat = RangedCombat * 0.9    WHERE Class = 'UNITCLASS_GATLINGGUN';
UPDATE Units SET Cost = Cost * 0.938, Combat = Combat * 1.118                                       WHERE Class = 'UNITCLASS_CAVALRY';
UPDATE Units SET Cost = Cost * 0.864, Combat = Combat * 1.029                                       WHERE Class = 'UNITCLASS_RIFLEMAN';
UPDATE Units SET Cost = Cost * 0.901, Combat = Combat * 1.2                                         WHERE Class = 'UNITCLASS_LANCER';
UPDATE Units SET Cost = Cost * 0.961, Combat = Combat * 1.571, RangedCombat = RangedCombat * 1.60   WHERE Class = 'UNITCLASS_CANNON';
UPDATE Units SET Cost = Cost * 1.157, Combat = Combat * 1.25                                        WHERE Class = 'UNITCLASS_KNIGHT';
UPDATE Units SET Cost = Cost * 1.111, Combat = Combat * 1.143                                       WHERE Class = 'UNITCLASS_LONGSWORDSMAN';
UPDATE Units SET Cost = Cost * 0.889, Combat = Combat * 1                                           WHERE Class = 'UNITCLASS_MUSKETMAN';
UPDATE Units SET Cost = Cost * 0.787, Combat = Combat * 0.923, RangedCombat = RangedCombat * 0.944  WHERE Class = 'UNITCLASS_CROSSBOWMAN';
UPDATE Units SET Cost = Cost * 1.019, Combat = Combat * 1.25 , RangedCombat = RangedCombat * 1.571  WHERE Class = 'UNITCLASS_TREBUCHET';
UPDATE Units SET Cost = Cost * 0.864, Combat = Combat * 1.125                                       WHERE Class = 'UNITCLASS_PIKEMAN';
UPDATE Units SET Cost = Cost * 1.185, Combat = Combat * 1.333                                       WHERE Class = 'UNITCLASS_HORSEMAN';
UPDATE Units SET Cost = Cost * 1.111, Combat = Combat * 1.071                                       WHERE Class = 'UNITCLASS_SWORDSMAN';
UPDATE Units SET Cost = Cost * 0.815, Combat = Combat * 1.143, RangedCombat = RangedCombat * 1      WHERE Class = 'UNITCLASS_COMPOSITE_BOWMAN';
UPDATE Units SET Cost = Cost * 1.037, Combat = Combat * 1.286, RangedCombat = RangedCombat * 1.75   WHERE Class = 'UNITCLASS_CATAPULT';
UPDATE Units SET Cost = Cost * 1.091, Combat = Combat * 1.167, RangedCombat = RangedCombat * 1.1    WHERE Class = 'UNITCLASS_CHARIOT_ARCHER';
UPDATE Units SET Cost = Cost * 0.794, Combat = Combat * 1                                           WHERE Class = 'UNITCLASS_SPEARMAN';
UPDATE Units SET Cost = Cost * 0.972, Combat = Combat * 1                                           WHERE Class = 'UNITCLASS_WARRIOR';
UPDATE Units SET Cost = Cost * 1.111, Combat = Combat * 1    , RangedCombat = RangedCombat * 1      WHERE Class = 'UNITCLASS_ARCHER';
UPDATE Units SET Cost = Cost * 1.111, Combat = Combat * 1.2                                         WHERE Class = 'UNITCLASS_SCOUT';
UPDATE Units SET Cost = Cost * 1.477, Combat = 95 , RangedCombat = 125, Range = 3, CombatClass = 'UNITCOMBAT_NAVALRANGED', Moves = 8  WHERE Class = 'UNITCLASS_MISSILE_CRUISER';
UPDATE Units SET Cost = 470         , Combat = 84 , RangedCombat = 0  , Range = 0, CombatClass = 'UNITCOMBAT_NAVALMELEE' , Moves = 8  WHERE Class = 'UNITCLASS_MISSILE_DESTROYER';
UPDATE Units SET Cost = Cost * 1.230, Combat = 40 , RangedCombat = 125, Range = 1, CombatClass = 'UNITCOMBAT_SUBMARINE'  , Moves = 7  WHERE Class = 'UNITCLASS_NUCLEAR_SUBMARINE';
UPDATE Units SET Cost = Cost * 0.978, Combat = 66 , RangedCombat = 0  , Range = 3, CombatClass = 'UNITCOMBAT_CARRIER'    , Moves = 7  WHERE Class = 'UNITCLASS_CARRIER';
UPDATE Units SET Cost = Cost * 1.037, Combat = 50 , RangedCombat = 70 , Range = 3, CombatClass = 'UNITCOMBAT_NAVALRANGED', Moves = 6  WHERE Class = 'UNITCLASS_BATTLESHIP';
UPDATE Units SET Cost = Cost * 0.815, Combat = 55 , RangedCombat = 0  , Range = 0, CombatClass = 'UNITCOMBAT_NAVALMELEE' , Moves = 7  WHERE Class = 'UNITCLASS_DESTROYER';
UPDATE Units SET Cost = 260         , Combat = 33 , RangedCombat = 46 , Range = 2, CombatClass = 'UNITCOMBAT_NAVALRANGED', Moves = 5  WHERE Class = 'UNITCLASS_CRUISER';
UPDATE Units SET Cost = Cost * 0.957, Combat = 25 , RangedCombat = 75 , Range = 1, CombatClass = 'UNITCOMBAT_SUBMARINE'  , Moves = 6  WHERE Class = 'UNITCLASS_SUBMARINE';
UPDATE Units SET Cost = Cost * 0.933, Combat = 42 , RangedCombat = 0  , Range = 0, CombatClass = 'UNITCOMBAT_NAVALMELEE' , Moves = 3  WHERE Class = 'UNITCLASS_IRONCLAD';
UPDATE Units SET Cost = Cost * 1.185, Combat = 32 , RangedCombat = 0  , Range = 0, CombatClass = 'UNITCOMBAT_NAVALMELEE' , Moves = 5  WHERE Class = 'UNITCLASS_PRIVATEER';
UPDATE Units SET Cost = Cost * 0.991, Combat = 25 , RangedCombat = 33 , Range = 2, CombatClass = 'UNITCOMBAT_NAVALRANGED', Moves = 4  WHERE Class = 'UNITCLASS_FRIGATE';
UPDATE Units SET Cost = Cost * 0.972, Combat = 21 , RangedCombat = 0  , Range = 0, CombatClass = 'UNITCOMBAT_NAVALMELEE' , Moves = 4  WHERE Class = 'UNITCLASS_CARAVEL';
UPDATE Units SET Cost = Cost * 1.167, Combat = 16 , RangedCombat = 21 , Range = 2, CombatClass = 'UNITCOMBAT_NAVALRANGED', Moves = 3  WHERE Class = 'UNITCLASS_GALLEASS';
UPDATE Units SET Cost = Cost * 1.235, Combat = 13 , RangedCombat = 0  , Range = 0, CombatClass = 'UNITCOMBAT_NAVALMELEE' , Moves = 4  WHERE Class = 'UNITCLASS_TRIREME';


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
	'UNIT_VENETIAN_GALLEASS'	
);

UPDATE Units SET Combat = Combat * 1.25, RangedCombat = RangedCombat * 1.25, Moves = Moves - 1 WHERE Type IN (
	'UNIT_KOREAN_TURTLE_SHIP'		
);



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
	'UNIT_MODERN_ARMOR'
);

UPDATE Unit_ResourceQuantityRequirements
SET ResourceType = 'RESOURCE_OIL', Cost = 2
WHERE UnitType IN (SELECT DISTINCT Type FROM Units WHERE Class IN (
	'UNIT_BATTLESHIP'
));

INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType)
SELECT DISTINCT Type, 'RESOURCE_OIL'
FROM Units WHERE Class IN (
	'UNITCLASS_MISSILE_CRUISER'
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

UPDATE Units SET GoodyHutUpgradeUnitClass = NULL WHERE Type IN (
	'UNIT_EGYPTIAN_WARCHARIOT'		,
	'UNIT_PERSIAN_IMMORTAL'			,
	'UNIT_HUN_HORSE_ARCHER'			,
	'UNIT_IROQUOIAN_MOHAWKWARRIOR'	,
	'UNIT_BABYLONIAN_BOWMAN'		,
	'UNIT_MAYAN_ATLATLIST'			
);

UPDATE Units SET	   Combat = ROUND(1.08 * (SELECT Combat FROM Units WHERE Type = 'UNIT_MUSKETMAN'), 0)
					   WHERE Type IN ('UNIT_AMERICAN_MINUTEMAN');
					   
					   
UPDATE Units SET	   Combat = 13  WHERE Type IN ('UNIT_HUN_BATTERING_RAM');
UPDATE Units SET	   Cost = 75   WHERE Type IN ('UNIT_HUN_BATTERING_RAM');


					   
UPDATE Units SET	   Cost = 105   WHERE Type IN ('UNIT_CARTHAGINIAN_FOREST_ELEPHANT');
UPDATE Units SET	   Cost = 130   WHERE Type IN ('UNIT_SONGHAI_MUSLIMCAVALRY');

UPDATE Units SET	   Combat = ROUND(1.25 * (SELECT Combat FROM Units WHERE Type = 'UNIT_WARRIOR'), 0)
					   WHERE Type IN ('UNIT_AZTEC_JAGUAR');

UPDATE Units SET	   Combat = ROUND(1.15 * (SELECT Combat FROM Units WHERE Type = 'UNIT_CROSSBOWMAN'), 0)
					   WHERE Type IN ('UNIT_CHINESE_CHUKONU');
UPDATE Units SET RangedCombat = ROUND(1.15 * (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_CROSSBOWMAN'), 0)
					   WHERE Type IN ('UNIT_CHINESE_CHUKONU');

UPDATE Units SET	   Combat = ROUND(1.15 * (SELECT Combat FROM Units WHERE Type = 'UNIT_LONGSWORDSMAN'), 0)
					   WHERE Type IN ('UNIT_DANISH_BERSERKER');

UPDATE Units SET	   Combat = ROUND(1.00 * (SELECT Combat FROM Units WHERE Type = 'UNIT_CHARIOT_ARCHER'), 0)
					   WHERE Type IN ('UNIT_EGYPTIAN_WARCHARIOT');
UPDATE Units SET RangedCombat = ROUND(1.07 * (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_CHARIOT_ARCHER'), 0)
					   WHERE Type IN ('UNIT_EGYPTIAN_WARCHARIOT');

UPDATE Units SET	   Combat = ROUND(0.80 * (SELECT Combat FROM Units WHERE Type = 'UNIT_CROSSBOWMAN'), 0)
					   WHERE Type IN ('UNIT_ENGLISH_LONGBOWMAN');

UPDATE Units SET	   Combat = ROUND(1.25 * (SELECT Combat FROM Units WHERE Type = 'UNIT_MUSKETMAN'), 0)
					   WHERE Type IN ('UNIT_FRENCH_MUSKETEER');
					   
UPDATE Units SET	   Combat = 40   WHERE Type IN ('UNIT_FRENCH_FOREIGNLEGION');

UPDATE Units SET	   Cost   = 200  WHERE Type IN ('UNIT_FRENCH_FOREIGNLEGION');
UPDATE Units SET	   Cost   = 470	   WHERE Type IN ('UNIT_GERMAN_PANZER');
UPDATE Units SET	   Combat = ROUND(0.77 * (SELECT Combat FROM Units WHERE Type = 'UNIT_MODERN_ARMOR'), 0)
					   WHERE Type IN ('UNIT_GERMAN_PANZER');

UPDATE Units SET	   Combat = ROUND(1.10 * (SELECT Combat FROM Units WHERE Type = 'UNIT_SPEARMAN'), 0)
					   WHERE Type IN ('UNIT_GREEK_HOPLITE');

UPDATE Units SET	   Combat = ROUND(1.00 * (SELECT Combat FROM Units WHERE Type = 'UNIT_ARCHER'), 0)
					   WHERE Type IN ('UNIT_INCAN_SLINGER');
UPDATE Units SET RangedCombat = ROUND(1.00 * (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_ARCHER'), 0)
					   WHERE Type IN ('UNIT_INCAN_SLINGER');
					   
UPDATE Units SET	   Cost   = ROUND(1.00 * (SELECT Cost   FROM Units WHERE Type = 'UNIT_HORSEMAN'), 0)
					   WHERE Type IN ('UNIT_INDIAN_WARELEPHANT');
UPDATE Units SET	   Combat = ROUND(0.65 * (SELECT Combat FROM Units WHERE Type = 'UNIT_HORSEMAN'), 0)
					   WHERE Type IN ('UNIT_INDIAN_WARELEPHANT');
UPDATE Units SET RangedCombat = ROUND(0.85 * (SELECT Combat FROM Units WHERE Type = 'UNIT_HORSEMAN'), 0)
					   WHERE Type IN ('UNIT_INDIAN_WARELEPHANT');

UPDATE Units SET	   Combat = ROUND(1.20 * (SELECT Combat FROM Units WHERE Type = 'UNIT_SWORDSMAN'), 0)
					   WHERE Type IN ('UNIT_KRIS_SWORDSMAN');

UPDATE Units SET	   Cost   = 156   WHERE Type IN ('UNIT_JAPANESE_SAMURAI');
UPDATE Units SET	   Combat = 28    WHERE Type IN ('UNIT_JAPANESE_SAMURAI');
					   
UPDATE Units SET	   Cost   = 156   WHERE Type IN ('UNIT_SPANISH_CONQUISTADOR');
UPDATE Units SET	   Cost   = 140   WHERE Type IN ('UNIT_SPANISH_TERCIO');

UPDATE Units SET	   Cost   = 145   WHERE Type IN ('UNIT_KOREAN_HWACHA');
UPDATE Units SET	   Combat = ROUND(1.20 * (SELECT Combat FROM Units WHERE Type = 'UNIT_TREBUCHET'), 0)
					   WHERE Type IN ('UNIT_KOREAN_HWACHA');
UPDATE Units SET RangedCombat = ROUND(1.20 * (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_TREBUCHET'), 0)
					   WHERE Type IN ('UNIT_KOREAN_HWACHA');

UPDATE Units SET	   Combat = ROUND(1.15 * (SELECT Combat FROM Units WHERE Type = 'UNIT_SPEARMAN'), 0)
					   WHERE Type IN ('UNIT_PERSIAN_IMMORTAL');
					   
UPDATE Units SET	   Combat = ROUND(1.25 * (SELECT Combat FROM Units WHERE Type = 'UNIT_WARRIOR'), 0)
					   WHERE Type IN ('UNIT_POLYNESIAN_MAORI_WARRIOR');
					   
					   -- Based on next unit
UPDATE Units SET	   Cost   = ROUND(0.80 * (SELECT Cost   FROM Units WHERE Type = 'UNIT_LONGSWORDSMAN'), 0)
					   WHERE Type IN ('UNIT_ROMAN_LEGION');
UPDATE Units SET	   Combat = ROUND(0.80 * (SELECT Combat FROM Units WHERE Type = 'UNIT_LONGSWORDSMAN'), 0)
					   WHERE Type IN ('UNIT_ROMAN_LEGION');
					   
UPDATE Units SET	   Combat = ROUND(1.11 * (SELECT Combat FROM Units WHERE Type = 'UNIT_CAVALRY'), 0)
					   WHERE Type IN ('UNIT_RUSSIAN_COSSACK');

UPDATE Units SET	   Combat = ROUND(1.05 * (SELECT Combat FROM Units WHERE Type = 'UNIT_CAVALRY'), 0)
					   WHERE Type IN ('UNIT_AUSTRIAN_HUSSAR');
					   
UPDATE Units SET	   Combat = ROUND(0.8 * (SELECT Combat FROM Units WHERE Type = 'UNIT_KNIGHT'), 0)
					   WHERE Type IN ('UNIT_MONGOLIAN_KESHIK');
UPDATE Units SET RangedCombat = ROUND(0.84 * (SELECT Combat FROM Units WHERE Type = 'UNIT_KNIGHT'), 0)
					   WHERE Type IN ('UNIT_MONGOLIAN_KESHIK');
UPDATE Units SET	   Cost   = ROUND(0.84 * (SELECT Cost   FROM Units WHERE Type = 'UNIT_KNIGHT'), 0)
					   WHERE Type IN ('UNIT_MONGOLIAN_KESHIK');
					   
UPDATE Units SET	   Combat = ROUND(0.84 * (SELECT Combat FROM Units WHERE Type = 'UNIT_KNIGHT'), 0)
					   WHERE Type IN ('UNIT_ARABIAN_CAMELARCHER');
UPDATE Units SET RangedCombat = ROUND(1.00 * (SELECT Combat FROM Units WHERE Type = 'UNIT_KNIGHT'), 0)
					   WHERE Type IN ('UNIT_ARABIAN_CAMELARCHER');
					   
UPDATE Units SET       Combat = ROUND(           Combat, 0)       WHERE       Combat <> 0;
UPDATE Units SET RangedCombat = ROUND(     RangedCombat, 0)       WHERE RangedCombat <> 0;

UPDATE Units SET         Cost = ROUND((Cost * 1) / 1, 0) * 1  WHERE         Cost  > 0;



UPDATE LoadedFile SET Value=1 WHERE Type='CEA__Start.sql';
