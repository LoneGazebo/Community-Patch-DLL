-----------------
--Air Units
-----------------
---- RANGED COMBAT
-----------------
--Modern
UPDATE Units SET RangedCombat = '45' WHERE Type = 'UNIT_TRIPLANE';
UPDATE Units SET RangedCombat = '50' WHERE Type = 'UNIT_WWI_BOMBER';
-- Atomic
UPDATE Units SET RangedCombat = '60' WHERE Type = 'UNIT_FIGHTER';
UPDATE Units SET RangedCombat = '65' WHERE Type = 'UNIT_BOMBER';
UPDATE Units SET RangedCombat = '70' WHERE Type = 'UNIT_AMERICAN_B17';
UPDATE Units SET RangedCombat = '60' WHERE Type = 'UNIT_JAPANESE_ZERO';	
UPDATE Units SET RangedCombat = '150' WHERE Type = 'UNIT_GUIDED_MISSILE';
--Information
UPDATE Units SET RangedCombat = '70' WHERE Type = 'UNIT_JET_FIGHTER';
UPDATE Units SET RangedCombat = '80' WHERE Type = 'UNIT_STEALTH_BOMBER';

-----------------
--Ranged Sea
-----------------
---- RANGED COMBAT
-----------------
-- Classical
UPDATE Units SET RangedCombat = '16' WHERE Type = 'UNIT_BYZANTINE_DROMON';	
-- Medieval
UPDATE Units SET RangedCombat = '24' WHERE Type = 'UNIT_GALLEASS';	
UPDATE Units SET RangedCombat = '26' WHERE Type = 'UNIT_VENETIAN_GALLEASS';	
--Renaissance
UPDATE Units SET RangedCombat = '38' WHERE Type = 'UNIT_ENGLISH_SHIPOFTHELINE';
UPDATE Units SET RangedCombat = '35' WHERE Type = 'UNIT_FRIGATE';		
--Industrial
UPDATE Units SET RangedCombat = '55' WHERE Type = 'UNIT_CRUISER';	
-- Modern
UPDATE Units SET RangedCombat = '65' WHERE Type = 'UNIT_SUBMARINE';
-- Atomic
UPDATE Units SET RangedCombat = '75' WHERE Type = 'UNIT_BATTLESHIP';	
UPDATE Units SET RangedCombat = '45' WHERE Type = 'UNIT_CARRIER';
-- Information
UPDATE Units SET RangedCombat = '80' WHERE Type = 'UNIT_NUCLEAR_SUBMARINE';
-----------------
--Melee	Sea
-----------------

-- MELEE COMBAT
----------------------	
-- Ancient
-- Melee
UPDATE Units SET Combat = '12' WHERE Type = 'UNIT_GALLEY';
UPDATE Units SET Combat = '14' WHERE Type = 'UNIT_CARTHAGINIAN_QUINQUEREME';

-- Classical
-- Melee
UPDATE Units SET Combat = '14' WHERE Type = 'UNIT_TRIREME';
-- Ranged
UPDATE Units SET Combat = '7' WHERE Type = 'UNIT_BYZANTINE_DROMON';
-- Medieval
-- Melee
UPDATE Units SET Combat = '25' WHERE Type = 'UNIT_CARAVEL';
UPDATE Units SET Combat = '30' WHERE Type = 'UNIT_PORTUGUESE_NAU';
UPDATE Units SET Combat = '30' WHERE Type = 'UNIT_KOREAN_TURTLE_SHIP';
-- Ranged
UPDATE Units SET Combat = '14' WHERE Type = 'UNIT_GALLEASS';
UPDATE Units SET Combat = '16' WHERE Type = 'UNIT_VENETIAN_GALLEASS';
-- Renaissance
-- Melee
UPDATE Units SET Combat = '40' WHERE Type = 'UNIT_PRIVATEER';
UPDATE Units SET Combat = '42' WHERE Type = 'UNIT_DUTCH_SEA_BEGGAR';
-- Ranged
UPDATE Units SET Combat = '28' WHERE Type = 'UNIT_ENGLISH_SHIPOFTHELINE';
UPDATE Units SET Combat = '25' WHERE Type = 'UNIT_FRIGATE';
-- Industrial
-- Melee
UPDATE Units SET Combat = '60' WHERE Type = 'UNIT_IRONCLAD';
-- Ranged
UPDATE Units SET Combat = '40' WHERE Type = 'UNIT_CRUISER';

-- Modern
-- Ranged
UPDATE Units SET Combat = '50' WHERE Type = 'UNIT_SUBMARINE';
-- Atomic
-- Melee
UPDATE Units SET Combat = '90' WHERE Type = 'UNIT_CARRIER';
UPDATE Units SET Combat = '75' WHERE Type = 'UNIT_DESTROYER';
-- Ranged
UPDATE Units SET Combat = '60' WHERE Type = 'UNIT_BATTLESHIP';
-- Information
-- Melee
UPDATE Units SET Combat = '90' WHERE Type = 'UNIT_MISSILE_CRUISER';
-- Ranged
UPDATE Units SET Combat = '70' WHERE Type = 'UNIT_NUCLEAR_SUBMARINE';

-----------------
--Ranged Land
-----------------
	
---- RANGED COMBAT
-----------------
-- Ancient
UPDATE Units SET RangedCombat = '7' WHERE Type = 'UNIT_INCAN_SLINGER';
UPDATE Units SET RangedCombat = '6' WHERE Type = 'UNIT_ARCHER';
UPDATE Units SET RangedCombat = '5' WHERE Type = 'UNIT_BARBARIAN_ARCHER';
UPDATE Units SET RangedCombat = '5' WHERE Type = 'UNIT_CHARIOT_ARCHER';
UPDATE Units SET RangedCombat = '7' WHERE Type = 'UNIT_BARBARIAN_AXMAN';
UPDATE Units SET RangedCombat = '5' WHERE Type = 'UNIT_EGYPTIAN_WARCHARIOT';
	
-- Classical
UPDATE Units SET RangedCombat = '11' WHERE Type = 'UNIT_COMPOSITE_BOWMAN';
UPDATE Units SET RangedCombat = '13' WHERE Type = 'UNIT_CATAPULT';
UPDATE Units SET RangedCombat = '11' WHERE Type = 'UNIT_MAYAN_ATLATLIST';
UPDATE Units SET RangedCombat = '10' WHERE Type = 'UNIT_MONGOLIAN_KESHIK';
UPDATE Units SET RangedCombat = '13' WHERE Type = 'UNIT_BABYLONIAN_BOWMAN';
UPDATE Units SET RangedCombat = '14' WHERE Type = 'UNIT_ROMAN_BALLISTA';
UPDATE Units SET RangedCombat = '11' WHERE Type = 'UNIT_HUN_HORSE_ARCHER';
	
-- Medieval
UPDATE Units SET RangedCombat = '20' WHERE Type = 'UNIT_CHINESE_CHUKONU';
UPDATE Units SET RangedCombat = '21' WHERE Type = 'UNIT_KOREAN_HWACHA';
UPDATE Units SET RangedCombat = '19' WHERE Type = 'UNIT_CROSSBOWMAN';
UPDATE Units SET RangedCombat = '20' WHERE Type = 'UNIT_TREBUCHET';
UPDATE Units SET RangedCombat = '18' WHERE Type = 'UNIT_MOUNTED_BOWMAN';
UPDATE Units SET RangedCombat = '19' WHERE Type = 'UNIT_ARABIAN_CAMELARCHER';
UPDATE Units SET RangedCombat = '20' WHERE Type = 'UNIT_ENGLISH_LONGBOWMAN';
	
--Renaissance
UPDATE Units SET RangedCombat = '27' WHERE Type = 'UNIT_AUSTRIAN_HUSSAR';
UPDATE Units SET RangedCombat = '27' WHERE Type = 'UNIT_CUIRASSIER';
UPDATE Units SET RangedCombat = '27' WHERE Type = 'UNIT_INDIAN_WARELEPHANT';
UPDATE Units SET RangedCombat = '29' WHERE Type = 'UNIT_CANNON';
UPDATE Units SET RangedCombat = '31' WHERE Type = 'UNIT_AMERICAN_MINUTEMAN';
UPDATE Units SET RangedCombat = '30' WHERE Type = 'UNIT_MUSKETMAN';
UPDATE Units SET RangedCombat = '31' WHERE Type = 'UNIT_OTTOMAN_JANISSARY';

-- Industrial

UPDATE Units SET RangedCombat = '44' WHERE Type = 'UNIT_GATLINGGUN';
UPDATE Units SET RangedCombat = '41' WHERE Type = 'UNIT_CAVALRY';
UPDATE Units SET RangedCombat = '42' WHERE Type = 'UNIT_BERBER_CAVALRY';
UPDATE Units SET RangedCombat = '42' WHERE Type = 'UNIT_SHOSHONE_COMANCHE_RIDERS';
UPDATE Units SET RangedCombat = '44' WHERE Type = 'UNIT_RUSSIAN_COSSACK';
UPDATE Units SET RangedCombat = '40' WHERE Type = 'UNIT_FIELD_GUN';

-- Modern
UPDATE Units SET RangedCombat = '58' WHERE Type = 'UNIT_MACHINE_GUN';
UPDATE Units SET RangedCombat = '50' WHERE Type = 'UNIT_ARTILLERY';
UPDATE Units SET RangedCombat = '52' WHERE Type = 'UNIT_ANTI_TANK_GUN';

-- Atomic
UPDATE Units SET RangedCombat = '70' WHERE Type = 'UNIT_BAZOOKA';

-- Information
UPDATE Units SET RangedCombat = '75' WHERE Type = 'UNIT_ROCKET_ARTILLERY';
UPDATE Units SET RangedCombat = '75' WHERE Type = 'UNIT_HELICOPTER_GUNSHIP';

-----------------
--Melee	Land
-----------------

-- MELEE COMBAT
----------------------		
-- Ancient

-- Melee
UPDATE Units SET Combat = '6' WHERE Type = 'UNIT_SHOSHONE_PATHFINDER';
UPDATE Units SET Combat = '8' WHERE Type = 'UNIT_WARRIOR';
UPDATE Units SET Combat = '8' WHERE Type = 'UNIT_BARBARIAN_WARRIOR';
UPDATE Units SET Combat = '10' WHERE Type = 'UNIT_AZTEC_JAGUAR';
UPDATE Units SET Combat = '12' WHERE Type = 'UNIT_SPEARMAN';
UPDATE Units SET Combat = '12' WHERE Type = 'UNIT_BARBARIAN_SPEARMAN';
UPDATE Units SET Combat = '13' WHERE Type = 'UNIT_PERSIAN_IMMORTAL';
UPDATE Units SET Combat = '13' WHERE Type = 'UNIT_CELT_PICTISH_WARRIOR';
UPDATE Units SET Combat = '14' WHERE Type = 'UNIT_GREEK_HOPLITE';
UPDATE Units SET Combat = '13' WHERE Type = 'UNIT_BARBARIAN_HORSEMAN';
UPDATE Units SET Combat = '13' WHERE Type = 'UNIT_HORSEMAN';
UPDATE Units SET Combat = '16' WHERE Type = 'UNIT_GREEK_COMPANIONCAVALRY';
UPDATE Units SET Combat = '16' WHERE Type = 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT';

--Ranged
UPDATE Units SET Combat = '4' WHERE Type = 'UNIT_INCAN_SLINGER';
UPDATE Units SET Combat = '4' WHERE Type = 'UNIT_ARCHER';
UPDATE Units SET Combat = '4' WHERE Type = 'UNIT_BARBARIAN_ARCHER';
UPDATE Units SET Combat = '8' WHERE Type = 'UNIT_CHARIOT_ARCHER';
UPDATE Units SET Combat = '7' WHERE Type = 'UNIT_BARBARIAN_AXMAN';
UPDATE Units SET Combat = '9' WHERE Type = 'UNIT_EGYPTIAN_WARCHARIOT';


-- Classical

-- Melee
UPDATE Units SET Combat = '9' WHERE Type = 'UNIT_SCOUT';
UPDATE Units SET Combat = '17' WHERE Type = 'UNIT_SWORDSMAN';
UPDATE Units SET Combat = '15' WHERE Type = 'UNIT_BARBARIAN_SWORDSMAN';
UPDATE Units SET Combat = '15' WHERE Type = 'UNIT_HUN_BATTERING_RAM';
UPDATE Units SET Combat = '17' WHERE Type = 'UNIT_KRIS_SWORDSMAN';
UPDATE Units SET Combat = '17' WHERE Type = 'UNIT_IROQUOIAN_MOHAWKWARRIOR';
UPDATE Units SET Combat = '18' WHERE Type = 'UNIT_ROMAN_LEGION';
UPDATE Units SET Combat = '21' WHERE Type = 'UNIT_POLYNESIAN_MAORI_WARRIOR';
UPDATE Units SET Combat = '20' WHERE Type = 'UNIT_DANISH_BERSERKER';

-- Ranged
UPDATE Units SET Combat = '7' WHERE Type = 'UNIT_CATAPULT';
UPDATE Units SET Combat = '7' WHERE Type = 'UNIT_ROMAN_BALLISTA';
UPDATE Units SET Combat = '11' WHERE Type = 'UNIT_MAYAN_ATLATLIST';
UPDATE Units SET Combat = '11' WHERE Type = 'UNIT_COMPOSITE_BOWMAN';
UPDATE Units SET Combat = '12' WHERE Type = 'UNIT_MONGOLIAN_KESHIK';
UPDATE Units SET Combat = '12' WHERE Type = 'UNIT_BABYLONIAN_BOWMAN';
UPDATE Units SET Combat = '13' WHERE Type = 'UNIT_HUN_HORSE_ARCHER';
	
-- Medieval
		
-- Melee
UPDATE Units SET Combat = '17' WHERE Type = 'UNIT_PIKEMAN';
-- UPDATE Units Set Combat = '19' WHERE Type = 'UNIT_FCOMPANY' -- Free Company, Entry in NewUnits.sql
UPDATE Units SET Combat = '22' WHERE Type = 'UNIT_LONGSWORDSMAN';
UPDATE Units SET Combat = '17' WHERE Type = 'UNIT_EXPLORER';
UPDATE Units SET Combat = '20' WHERE Type = 'UNIT_BANDEIRANTES';
UPDATE Units SET Combat = '23' WHERE Type = 'UNIT_JAPANESE_SAMURAI';
UPDATE Units SET Combat = '24' WHERE Type = 'UNIT_KNIGHT';
UPDATE Units SET Combat = '28' WHERE Type = 'UNIT_BYZANTINE_CATAPHRACT';
UPDATE Units SET Combat = '26' WHERE Type = 'UNIT_SPANISH_CONQUISTADOR';
UPDATE Units SET Combat = '26' WHERE Type = 'UNIT_SIAMESE_WARELEPHANT';
UPDATE Units SET Combat = '25' WHERE Type = 'UNIT_SONGHAI_MUSLIMCAVALRY';
UPDATE Units SET Combat = '26' WHERE Type = 'UNIT_GERMAN_LANDSKNECHT';

-- Ranged
UPDATE Units SET Combat = '12' WHERE Type = 'UNIT_TREBUCHET';
UPDATE Units SET Combat = '13' WHERE Type = 'UNIT_KOREAN_HWACHA';
UPDATE Units SET Combat = '15' WHERE Type = 'UNIT_CROSSBOWMAN';
UPDATE Units SET Combat = '15' WHERE Type = 'UNIT_CHINESE_CHUKONU';
UPDATE Units SET Combat = '16' WHERE Type = 'UNIT_ENGLISH_LONGBOWMAN';
UPDATE Units SET Combat = '15' WHERE Type = 'UNIT_MOUNTED_BOWMAN';
UPDATE Units SET Combat = '17' WHERE Type = 'UNIT_ARABIAN_CAMELARCHER';


-- Renaissance

-- Melee 

UPDATE Units SET Combat = '30' WHERE Type = 'UNIT_FRENCH_MUSKETEER';
UPDATE Units SET Combat = '25' WHERE Type = 'UNIT_SPANISH_TERCIO';
UPDATE Units SET Combat = '28' WHERE Type = 'UNIT_ZULU_IMPI';
UPDATE Units SET Combat = '37' WHERE Type = 'UNIT_LANCER';
UPDATE Units SET Combat = '40' WHERE Type = 'UNIT_POLISH_WINGED_HUSSAR';
UPDATE Units SET Combat = '37' WHERE Type = 'UNIT_OTTOMAN_SIPAHI';
UPDATE Units SET Combat = '37' WHERE Type = 'UNIT_SWEDISH_HAKKAPELIITTA';

-- Ranged

UPDATE Units SET Combat = '16' WHERE Type = 'UNIT_CANNON';
UPDATE Units SET Combat = '26' WHERE Type = 'UNIT_AUSTRIAN_HUSSAR';
UPDATE Units SET Combat = '25' WHERE Type = 'UNIT_CUIRASSIER';
UPDATE Units SET Combat = '25' WHERE Type = 'UNIT_INDIAN_WARELEPHANT';
UPDATE Units SET Combat = '23' WHERE Type = 'UNIT_AMERICAN_MINUTEMAN';
UPDATE Units SET Combat = '22' WHERE Type = 'UNIT_MUSKETMAN';
UPDATE Units SET Combat = '24' WHERE Type = 'UNIT_OTTOMAN_JANISSARY';

-- Industrial

-- Melee
UPDATE Units SET Combat = '38' WHERE Type = 'UNIT_RIFLEMAN';
UPDATE Units SET Combat = '41' WHERE Type = 'UNIT_SWEDISH_CAROLEAN';
UPDATE Units SET Combat = '42' WHERE Type = 'UNIT_ETHIOPIAN_MEHAL_SEFARI';
UPDATE Units SET Combat = '40' WHERE Type = 'UNIT_FRENCH_FOREIGNLEGION'; -- More changes in Honor.sql
--UPDATE Units SET Combat = '32' WHERE Type = 'UNIT_ZEPPELIN';
UPDATE Units SET Combat = '32' WHERE Type = 'UNIT_COMMANDO';
-- Ranged

UPDATE Units SET Combat = '32' WHERE Type = 'UNIT_GATLINGGUN';
UPDATE Units SET Combat = '33' WHERE Type = 'UNIT_CAVALRY';
UPDATE Units SET Combat = '34' WHERE Type = 'UNIT_BERBER_CAVALRY';
UPDATE Units SET Combat = '34' WHERE Type = 'UNIT_SHOSHONE_COMANCHE_RIDERS';
UPDATE Units SET Combat = '35' WHERE Type = 'UNIT_RUSSIAN_COSSACK';
UPDATE Units SET Combat = '25' WHERE Type = 'UNIT_FIELD_GUN';

-- Modern

-- Melee
UPDATE Units SET Combat = '40' WHERE Type = 'UNIT_ANTI_AIRCRAFT_GUN';
UPDATE Units SET Combat = '50' WHERE Type = 'UNIT_GREAT_WAR_INFANTRY';
UPDATE Units SET Combat = '42' WHERE Type = 'UNIT_PARATROOPER';
UPDATE Units SET Combat = '60' WHERE Type = 'UNIT_WWI_TANK';

-- Ranged
UPDATE Units SET Combat = '45' WHERE Type = 'UNIT_MACHINE_GUN';
UPDATE Units SET Combat = '40' WHERE Type = 'UNIT_ARTILLERY';
	
-- Atomic
-- Melee
UPDATE Units SET Combat = '62' WHERE Type = 'UNIT_DANISH_SKI_INFANTRY';
-- UPDATE Units SET Combat = '65' WHERE Type = 'UNIT_GUERILLA'; -- Mercenaries, Entry in NewUnits.sql
UPDATE Units SET Combat = '60' WHERE Type = 'UNIT_INFANTRY';
UPDATE Units SET Combat = '60' WHERE Type = 'UNIT_BRAZILIAN_PRACINHA';
UPDATE Units SET Combat = '75' WHERE Type = 'UNIT_TANK';
UPDATE Units SET Combat = '82' WHERE Type = 'UNIT_GERMAN_PANZER';

-- Ranged
UPDATE Units SET Combat = '50' WHERE Type = 'UNIT_BAZOOKA';
UPDATE Units SET Combat = '52' WHERE Type = 'UNIT_ANTI_TANK_GUN';

-- Information

-- Melee
UPDATE Units SET Combat = '65' WHERE Type = 'UNIT_MARINE';
UPDATE Units SET Combat = '55' WHERE Type = 'UNIT_MOBILE_SAM';
UPDATE Units SET Combat = '75' WHERE Type = 'UNIT_MECHANIZED_INFANTRY';
UPDATE Units SET Combat = '75' WHERE Type = 'UNIT_XCOM_SQUAD';
UPDATE Units SET Combat = '100' WHERE Type = 'UNIT_MODERN_ARMOR';
UPDATE Units SET Combat = '125' WHERE Type = 'UNIT_MECH';

-- Ranged
UPDATE Units SET Combat = '55' WHERE Type = 'UNIT_ROCKET_ARTILLERY';
UPDATE Units SET Combat = '60' WHERE Type = 'UNIT_HELICOPTER_GUNSHIP';


-- Let's set each unit's core air defense values here.
-- Industrial

-- Melee
UPDATE Units SET BaseLandAirDefense = '4' WHERE Type = 'UNIT_RIFLEMAN';
UPDATE Units SET BaseLandAirDefense = '4' WHERE Type = 'UNIT_ETHIOPIAN_MEHAL_SEFARI';
UPDATE Units SET BaseLandAirDefense = '4' WHERE Type = 'UNIT_FRENCH_FOREIGNLEGION';
--UPDATE Units SET BaseLandAirDefense = '2' WHERE Type = 'UNIT_ZEPPELIN';
UPDATE Units SET BaseLandAirDefense = '2' WHERE Type = 'UNIT_COMMANDO';

-- Ranged

UPDATE Units SET BaseLandAirDefense = '2' WHERE Type = 'UNIT_GATLINGGUN';
UPDATE Units SET BaseLandAirDefense = '2' WHERE Type = 'UNIT_CAVALRY';
UPDATE Units SET BaseLandAirDefense = '2' WHERE Type = 'UNIT_BERBER_CAVALRY';
UPDATE Units SET BaseLandAirDefense = '2' WHERE Type = 'UNIT_SHOSHONE_COMANCHE_RIDERS';
UPDATE Units SET BaseLandAirDefense = '2' WHERE Type = 'UNIT_RUSSIAN_COSSACK';
UPDATE Units SET BaseLandAirDefense = '1' WHERE Type = 'UNIT_FIELD_GUN';

-- Modern

-- Melee
UPDATE Units SET BaseLandAirDefense = '20' WHERE Type = 'UNIT_ANTI_AIRCRAFT_GUN';
UPDATE Units SET BaseLandAirDefense = '5' WHERE Type = 'UNIT_GREAT_WAR_INFANTRY';
UPDATE Units SET BaseLandAirDefense = '5' WHERE Type = 'UNIT_PARATROOPER';
UPDATE Units SET BaseLandAirDefense = '3' WHERE Type = 'UNIT_WWI_TANK';
UPDATE Units SET BaseLandAirDefense = '3' WHERE Type = 'UNIT_GERMAN_PANZER';

-- Ranged
UPDATE Units SET BaseLandAirDefense = '10' WHERE Type = 'UNIT_MACHINE_GUN';
UPDATE Units SET BaseLandAirDefense = '3' WHERE Type = 'UNIT_ARTILLERY';
	
-- Atomic
-- Melee
UPDATE Units SET BaseLandAirDefense = '5' WHERE Type = 'UNIT_DANISH_SKI_INFANTRY';
UPDATE Units SET BaseLandAirDefense = '8' WHERE Type = 'UNIT_GUERILLA'; -- Mercenaries
UPDATE Units SET BaseLandAirDefense = '8' WHERE Type = 'UNIT_INFANTRY';
UPDATE Units SET BaseLandAirDefense = '8' WHERE Type = 'UNIT_BRAZILIAN_PRACINHA';
UPDATE Units SET BaseLandAirDefense = '3' WHERE Type = 'UNIT_TANK';

-- Ranged
UPDATE Units SET BaseLandAirDefense = '15' WHERE Type = 'UNIT_BAZOOKA';
UPDATE Units SET BaseLandAirDefense = '5' WHERE Type = 'UNIT_ANTI_TANK_GUN';

-- Information
		
-- Melee
UPDATE Units SET BaseLandAirDefense = '10' WHERE Type = 'UNIT_MARINE';
UPDATE Units SET BaseLandAirDefense = '40' WHERE Type = 'UNIT_MOBILE_SAM';
UPDATE Units SET BaseLandAirDefense = '15' WHERE Type = 'UNIT_MECHANIZED_INFANTRY';
UPDATE Units SET BaseLandAirDefense = '25' WHERE Type = 'UNIT_XCOM_SQUAD';
UPDATE Units SET BaseLandAirDefense = '10' WHERE Type = 'UNIT_MODERN_ARMOR';
UPDATE Units SET BaseLandAirDefense = '30' WHERE Type = 'UNIT_MECH';

-- Ranged
UPDATE Units SET BaseLandAirDefense = '35' WHERE Type = 'UNIT_ROCKET_ARTILLERY';
UPDATE Units SET BaseLandAirDefense = '10' WHERE Type = 'UNIT_HELICOPTER_GUNSHIP';


-- And now boats.

-- Industrial
-- Melee
UPDATE Units SET BaseLandAirDefense = '5' WHERE Type = 'UNIT_IRONCLAD';
-- Ranged
UPDATE Units SET BaseLandAirDefense = '10' WHERE Type = 'UNIT_CRUISER';

-- Modern
-- Ranged
UPDATE Units SET BaseLandAirDefense = '5' WHERE Type = 'UNIT_SUBMARINE';
-- Atomic
-- Melee
UPDATE Units SET BaseLandAirDefense = '40' WHERE Type = 'UNIT_CARRIER';
UPDATE Units SET BaseLandAirDefense = '30' WHERE Type = 'UNIT_DESTROYER';
-- Ranged
UPDATE Units SET BaseLandAirDefense = '25' WHERE Type = 'UNIT_BATTLESHIP';
-- Information
-- Melee
UPDATE Units SET BaseLandAirDefense = '35' WHERE Type = 'UNIT_MISSILE_CRUISER';
-- Ranged
UPDATE Units SET BaseLandAirDefense = '10' WHERE Type = 'UNIT_NUCLEAR_SUBMARINE';