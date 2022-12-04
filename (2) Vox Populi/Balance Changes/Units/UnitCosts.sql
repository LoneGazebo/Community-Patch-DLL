--- Other --

	UPDATE Units SET FaithCost = '200' WHERE Type = 'UNIT_MISSIONARY';
	UPDATE Units SET HurryCostModifier = '-1' WHERE Type = 'UNIT_SETTLER';

	UPDATE Units SET Cost = '40' WHERE Type = 'UNIT_WORKBOAT';
	UPDATE Units SET Cost = '90' WHERE Type = 'UNIT_CARAVAN';
	UPDATE Units SET Cost = '140' WHERE Type = 'UNIT_CARGO_SHIP';
	UPDATE Units SET Cost = '80' WHERE Type = 'UNIT_WORKER';

--- Ancient ---

	UPDATE Units SET Cost = '40' , FaithCost = '100' WHERE Type = 'UNIT_SHOSHONE_PATHFINDER';
	UPDATE Units SET Cost = '40' , FaithCost = '100' WHERE Type = 'UNIT_WARRIOR';
	UPDATE Units SET Cost = '40' , FaithCost = '100' WHERE Type = 'UNIT_AZTEC_JAGUAR';
	UPDATE Units SET Cost = '40' , FaithCost = '100' WHERE Type = 'UNIT_VP_SLINGER';
	UPDATE Units SET Cost = '40' , FaithCost = '100' WHERE Type = 'UNIT_INCAN_SLINGER';
	UPDATE Units SET Cost = '55' , FaithCost = '100' WHERE Type = 'UNIT_CHARIOT_ARCHER';
	UPDATE Units SET Cost = '55' , FaithCost = '100' WHERE Type = 'UNIT_EGYPTIAN_WARCHARIOT';
	UPDATE Units SET Cost = '70' , FaithCost = '100' WHERE Type = 'UNIT_SPEARMAN';
	UPDATE Units SET Cost = '70' , FaithCost = '100' WHERE Type = 'UNIT_GREEK_HOPLITE';
	UPDATE Units SET Cost = '70' , FaithCost = '100' WHERE Type = 'UNIT_PERSIAN_IMMORTAL';
	UPDATE Units SET Cost = '70' , FaithCost = '100' WHERE Type = 'UNIT_CELT_PICTISH_WARRIOR';
	UPDATE Units SET Cost = '70' , FaithCost = '100' WHERE Type = 'UNIT_ARCHER';
	UPDATE Units SET Cost = '70',  FaithCost = '100' WHERE Type = 'UNIT_BABYLONIAN_BOWMAN';
	UPDATE Units SET Cost = '70' WHERE Type = 'UNIT_BARBARIAN_ARCHER';
	UPDATE Units SET Cost = '90' , FaithCost = '100' WHERE Type = 'UNIT_HORSEMAN';	
	UPDATE Units SET Cost = '125', FaithCost = '100' WHERE Type = 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT';
	UPDATE Units SET Cost = '100', FaithCost = '200' WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER';

	UPDATE Units SET Cost = '80' , FaithCost = '125' WHERE Type = 'UNIT_CARTHAGINIAN_QUINQUEREME'; -- available early, cheaper as benefit

--- Classical ---

	UPDATE Units SET Cost = '90',  FaithCost = '200' WHERE Type = 'UNIT_SCOUT';
	UPDATE Units SET Cost = '110', FaithCost = '200' WHERE Type = 'UNIT_COMPOSITE_BOWMAN';
	UPDATE Units SET Cost = '100', FaithCost = '200' WHERE Type = 'UNIT_MAYAN_ATLATLIST';
	UPDATE Units SET Cost = '100', FaithCost = '200' WHERE Type = 'UNIT_MONGOLIAN_KESHIK';
	UPDATE Units SET Cost = '100', FaithCost = '200' WHERE Type = 'UNIT_HUN_HORSE_ARCHER';
	UPDATE Units SET Cost = '100', FaithCost = '200' WHERE Type = 'UNIT_SWORDSMAN';
	UPDATE Units SET Cost = '100', FaithCost = '200' WHERE Type = 'UNIT_KRIS_SWORDSMAN';
	UPDATE Units SET Cost = '100', FaithCost = '200' WHERE Type = 'UNIT_ROMAN_LEGION';
	UPDATE Units SET Cost = '100', FaithCost = '200' WHERE Type = 'UNIT_IROQUOIAN_MOHAWKWARRIOR';
	UPDATE Units SET Cost = '100', FaithCost = '200' WHERE Type = 'UNIT_CATAPULT';
	UPDATE Units SET Cost = '135', FaithCost = '200' WHERE Type = 'UNIT_DANISH_BERSERKER'; -- UNIT_PIKEMAN replacement, available early

	UPDATE Units SET Cost = '90',  FaithCost = '250' WHERE Type = 'UNIT_TRIREME';
	UPDATE Units SET Cost = '100', FaithCost = '250' WHERE Type = 'UNIT_BYZANTINE_DROMON';

--- Medieval ---

	UPDATE Units SET Cost = '135', FaithCost = '300' WHERE Type = 'UNIT_PIKEMAN';
	UPDATE Units SET Cost = '160', FaithCost = '300' WHERE Type = 'UNIT_CROSSBOWMAN';
	UPDATE Units SET Cost = '160', FaithCost = '300' WHERE Type = 'UNIT_CHINESE_CHUKONU';
	UPDATE Units SET Cost = '160', FaithCost = '300' WHERE Type = 'UNIT_EXPLORER';
	UPDATE Units SET Cost = '160', FaithCost = '300' WHERE Type = 'UNIT_BANDEIRANTES';
	UPDATE Units SET Cost = '160', FaithCost = '300' WHERE Type = 'UNIT_LONGSWORDSMAN';
	UPDATE Units SET Cost = '140', FaithCost = '300' WHERE Type = 'UNIT_POLYNESIAN_MAORI_WARRIOR'; -- cheaper as benefit, available at adjacent tech
	UPDATE Units SET Cost = '160', FaithCost = '300' WHERE Type = 'UNIT_JAPANESE_SAMURAI';
	UPDATE Units SET Cost = '175', FaithCost = '300' WHERE Type = 'UNIT_MOUNTED_BOWMAN';
	UPDATE Units SET Cost = '175', FaithCost = '300' WHERE Type = 'UNIT_ARABIAN_CAMELARCHER';
	UPDATE Units SET Cost = '175', FaithCost = '300' WHERE Type = 'UNIT_TREBUCHET';
	UPDATE Units SET Cost = '175', FaithCost = '300' WHERE Type = 'UNIT_KOREAN_HWACHA';
	UPDATE Units SET Cost = '180', FaithCost = '300' WHERE Type = 'UNIT_SPANISH_CONQUISTADOR'; -- more expensive than explorer, does a lot more.
	UPDATE Units SET Cost = '200', FaithCost = '300' WHERE Type = 'UNIT_KNIGHT';
	UPDATE Units SET Cost = '200', FaithCost = '300' WHERE Type = 'UNIT_SONGHAI_MUSLIMCAVALRY';
	UPDATE Units SET Cost = '200', FaithCost = '300' WHERE Type = 'UNIT_BYZANTINE_CATAPHRACT';
	UPDATE Units SET Cost = '220', FaithCost = '300' WHERE Type = 'UNIT_SIAMESE_WARELEPHANT';

 -- UPDATE Units Set Cost = '125'					 WHERE Type = 'UNIT_FCOMPANY'; -- Entry in NewUnits.sql

	UPDATE Units SET Cost = '160', FaithCost = '350' WHERE Type = 'UNIT_CARAVEL';
	UPDATE Units SET Cost = '160', FaithCost = '350' WHERE Type = 'UNIT_PORTUGUESE_NAU';
	UPDATE Units SET Cost = '175', FaithCost = '350' WHERE Type = 'UNIT_GALLEASS';

--- Renaissance ---

	UPDATE Units SET Cost = '300', FaithCost = '400' WHERE Type = 'UNIT_SPANISH_TERCIO';
	UPDATE Units SET Cost = '275', FaithCost = '400' WHERE Type = 'UNIT_GERMAN_LANDSKNECHT'; -- cheaper as benefit
	UPDATE Units SET Cost = '300', FaithCost = '400' WHERE Type = 'UNIT_ZULU_IMPI';
	UPDATE Units SET Cost = '325', FaithCost = '400' WHERE Type = 'UNIT_FRENCH_MUSKETEER';
	UPDATE Units SET Cost = '325', FaithCost = '400' WHERE Type = 'UNIT_MUSKETMAN';
	UPDATE Units SET Cost = '325', FaithCost = '400' WHERE Type = 'UNIT_OTTOMAN_JANISSARY'; -- available early
	UPDATE Units SET Cost = '325', FaithCost = '400' WHERE Type = 'UNIT_AMERICAN_MINUTEMAN';
	UPDATE Units SET Cost = '350', FaithCost = '400' WHERE Type = 'UNIT_CUIRASSIER';
	UPDATE Units SET Cost = '350', FaithCost = '400' WHERE Type = 'UNIT_INDIAN_WARELEPHANT'; -- available early
	UPDATE Units SET Cost = '350', FaithCost = '400' WHERE Type = 'UNIT_AUSTRIAN_HUSSAR';
	UPDATE Units SET Cost = '350', FaithCost = '400' WHERE Type = 'UNIT_LANCER';
	UPDATE Units SET Cost = '350', FaithCost = '400' WHERE Type = 'UNIT_POLISH_WINGED_HUSSAR';
	UPDATE Units SET Cost = '350', FaithCost = '400' WHERE Type = 'UNIT_CANNON';

	UPDATE Units SET Cost = '350', FaithCost = '450' WHERE Type = 'UNIT_PRIVATEER';
	UPDATE Units SET Cost = '350', FaithCost = '450' WHERE Type = 'UNIT_DUTCH_SEA_BEGGAR';	
	UPDATE Units SET Cost = '375', FaithCost = '450' WHERE Type = 'UNIT_FRIGATE';
	UPDATE Units SET Cost = '375', FaithCost = '450' WHERE Type = 'UNIT_ENGLISH_SHIPOFTHELINE';

--- Industrial ---

	UPDATE Units SET Cost = '600', FaithCost = '600' WHERE Type = 'UNIT_COMMANDO';
	UPDATE Units SET Cost = '625', FaithCost = '600' WHERE Type = 'UNIT_RIFLEMAN';
	UPDATE Units SET Cost = '600', FaithCost = '600' WHERE Type = 'UNIT_ETHIOPIAN_MEHAL_SEFARI'; -- cheaper as benefit
	UPDATE Units SET Cost = '625', FaithCost = '600' WHERE Type = 'UNIT_SWEDISH_CAROLEAN';
	UPDATE Units SET Cost = '700', FaithCost = '600' WHERE Type = 'UNIT_GATLINGGUN';
	UPDATE Units SET Cost = '800', FaithCost = '600' WHERE Type = 'UNIT_CAVALRY';
	UPDATE Units SET Cost = '800', FaithCost = '600' WHERE Type = 'UNIT_BERBER_CAVALRY';
	UPDATE Units SET Cost = '800', FaithCost = '600' WHERE Type = 'UNIT_RUSSIAN_COSSACK';
	UPDATE Units SET Cost = '800', FaithCost = '600' WHERE Type = 'UNIT_SHOSHONE_COMANCHE_RIDERS'; -- available early
	UPDATE Units SET Cost = '900', FaithCost = '600' WHERE Type = 'UNIT_FIELD_GUN';

	UPDATE Units SET Cost = '550'					 WHERE Type = 'UNIT_FRENCH_FOREIGNLEGION'; -- More changes in Honor.sql

	UPDATE Units SET Cost = '450', FaithCost = '225' WHERE Type = 'UNIT_ARCHAEOLOGIST';

	UPDATE Units SET Cost = '900', FaithCost = '650' WHERE Type = 'UNIT_IRONCLAD';
	UPDATE Units SET Cost = '900', FaithCost = '650' WHERE Type = 'UNIT_CRUISER';

--- Modern ---

	UPDATE Units SET Cost = '900',  FaithCost = '700' WHERE Type = 'UNIT_GREAT_WAR_INFANTRY';
	UPDATE Units SET Cost = '950',  FaithCost = '700' WHERE Type = 'UNIT_ANTI_AIRCRAFT_GUN';
	UPDATE Units SET Cost = '950',  FaithCost = '700' WHERE Type = 'UNIT_MACHINE_GUN';
	UPDATE Units SET Cost = '950',  FaithCost = '700' WHERE Type = 'UNIT_PARATROOPER';
	UPDATE Units SET Cost = '1000', FaithCost = '700' WHERE Type = 'UNIT_WWI_TANK';
	UPDATE Units SET Cost = '1200', FaithCost = '800' WHERE Type = 'UNIT_ANTI_TANK_GUN';
	UPDATE Units SET Cost = '1300', FaithCost = '700' WHERE Type = 'UNIT_ARTILLERY';

	UPDATE Units SET Cost = '800',  FaithCost = '700' WHERE Type = 'UNIT_TRIPLANE';
	UPDATE Units SET Cost = '850',  FaithCost = '700' WHERE Type = 'UNIT_WWI_BOMBER';

--- Atomic ---

	UPDATE Units SET Cost = '1300', FaithCost = '800' WHERE Type = 'UNIT_INFANTRY';
	UPDATE Units SET Cost = '1300', FaithCost = '800' WHERE Type = 'UNIT_BAZOOKA';
	UPDATE Units SET Cost = '1800', FaithCost = '800' WHERE Type = 'UNIT_TANK';
	UPDATE Units SET Cost = '1800', FaithCost = '800' WHERE Type = 'UNIT_GERMAN_PANZER';
	
--  UPDATE Units SET Cost = '1300',					  WHERE Type = 'UNIT_GUERILLA'; -- Entry in NewUnits.sql

	UPDATE Units SET Cost = '1400', FaithCost = '800' WHERE Type = 'UNIT_FIGHTER';
	UPDATE Units SET Cost = '1400', FaithCost = '800' WHERE Type = 'UNIT_JAPANESE_ZERO';
	UPDATE Units SET Cost = '1500', FaithCost = '800' WHERE Type = 'UNIT_BOMBER';
	UPDATE Units SET Cost = '1500', FaithCost = '800' WHERE Type = 'UNIT_AMERICAN_B17';

	UPDATE Units SET Cost = '1600', FaithCost = '1000' WHERE Type = 'UNIT_DESTROYER';
	UPDATE Units SET Cost = '1600', FaithCost = '1000' WHERE Type = 'UNIT_SUBMARINE';
	UPDATE Units SET Cost = '1800', FaithCost = '1000' WHERE Type = 'UNIT_BATTLESHIP';
	UPDATE Units SET Cost = '1800', FaithCost = '1000' WHERE Type = 'UNIT_CARRIER';

	UPDATE Units SET Cost = '1200' WHERE Type = 'UNIT_GUIDED_MISSILE';
	UPDATE Units SET Cost = '3500' WHERE Type = 'UNIT_ATOMIC_BOMB';

--- Information ---

	UPDATE Units SET Cost = '1750', FaithCost = '1000' WHERE Type = 'UNIT_MARINE';
	UPDATE Units SET Cost = '1800', FaithCost = '1000' WHERE Type = 'UNIT_MECHANIZED_INFANTRY';
	UPDATE Units SET Cost = '1800', FaithCost = '1000' WHERE Type = 'UNIT_MOBILE_SAM';
	UPDATE Units SET Cost = '2250', FaithCost = '1000' WHERE Type = 'UNIT_HELICOPTER_GUNSHIP';
	UPDATE Units SET Cost = '2250', FaithCost = '1000' WHERE Type = 'UNIT_ROCKET_ARTILLERY';
	UPDATE Units SET Cost = '2500', FaithCost = '1000' WHERE Type = 'UNIT_MODERN_ARMOR';
	UPDATE Units SET Cost = '2600', FaithCost = '1000' WHERE Type = 'UNIT_XCOM_SQUAD';
	UPDATE Units SET Cost = '3000', FaithCost = '1250' WHERE Type = 'UNIT_MECH';

	UPDATE Units SET Cost = '2100', FaithCost = '1000' WHERE Type = 'UNIT_JET_FIGHTER';
	UPDATE Units SET Cost = '2200', FaithCost = '1000' WHERE Type = 'UNIT_STEALTH_BOMBER';

	UPDATE Units SET Cost = '2250', FaithCost = '1250' WHERE Type = 'UNIT_NUCLEAR_SUBMARINE';
	UPDATE Units SET Cost = '2400', FaithCost = '1250' WHERE Type = 'UNIT_MISSILE_CRUISER';

	UPDATE Units SET Cost = '5600' WHERE Type = 'UNIT_NUCLEAR_MISSILE';

	UPDATE Units SET Cost = '8000' WHERE Type = 'UNIT_SS_BOOSTER';
	UPDATE Units SET Cost = '8000' WHERE Type = 'UNIT_SS_STASIS_CHAMBER';
	UPDATE Units SET Cost = '8000' WHERE Type = 'UNIT_SS_ENGINE';
	UPDATE Units SET Cost = '8000' WHERE Type = 'UNIT_SS_COCKPIT';

--- Hurry Costs ---

	UPDATE Units SET HurryCostModifier = '-1' WHERE Type IN ('UNIT_ATOMIC_BOMB', 'UNIT_NUCLEAR_MISSILE');

--- Faith Purchase Enablers ---

	UPDATE Units
	SET RequiresFaithPurchaseEnabled = '1'
	WHERE Type IN
	('UNIT_WWI_TANK',
	'UNIT_HELICOPTER_GUNSHIP',
	'UNIT_JET_FIGHTER',
	'UNIT_STEALTH_BOMBER',
	'UNIT_FIGHTER',
	'UNIT_BOMBER',
	'UNIT_TRIPLANE',
	'UNIT_WWI_BOMBER',
	'UNIT_AUSTRIAN_HUSSAR',
	'UNIT_ETHIOPIAN_MEHAL_SEFARI',
	'UNIT_SWEDISH_CAROLEAN',
	'UNIT_JAPANESE_ZERO',
	'UNIT_AMERICAN_B17');
