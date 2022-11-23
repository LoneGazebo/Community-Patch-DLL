-- MELEE UNITS
-- LAND

	

	-- Tech Stuff
	-- Recon
	UPDATE Units SET								ObsoleteTech = 'TECH_OPTICS'	WHERE Type = 'UNIT_SHOSHONE_PATHFINDER';
	UPDATE Units SET PrereqTech = 'TECH_OPTICS',	ObsoleteTech = 'TECH_COMPASS'	WHERE Type = 'UNIT_SCOUT';
	UPDATE Units SET 								ObsoleteTech = 'TECH_FLIGHT'	WHERE Type = 'UNIT_BANDEIRANTES';
	UPDATE Units SET PrereqTech = 'TECH_FLIGHT',	ObsoleteTech = 'TECH_COMPUTERS'	WHERE Type = 'UNIT_PARATROOPER';
	UPDATE Units SET PrereqTech = 'TECH_COMPUTERS', ObsoleteTech = 'TECH_LASERS'	WHERE Type = 'UNIT_MARINE';
	UPDATE Units SET PrereqTech = 'TECH_LASERS'										WHERE Type = 'UNIT_XCOM_SQUAD';
	-------

	UPDATE Units SET ObsoleteTech = 'TECH_BRONZE_WORKING' WHERE Type = 'UNIT_WARRIOR';
	UPDATE Units SET ObsoleteTech = 'TECH_BRONZE_WORKING' WHERE Type = 'UNIT_BARBARIAN_WARRIOR';
	UPDATE Units SET ObsoleteTech = 'TECH_STEEL' WHERE Type = 'UNIT_AZTEC_JAGUAR';

	UPDATE Units SET							 ObsoleteTech = 'TECH_STEEL'	 WHERE Type = 'UNIT_BARBARIAN_SPEARMAN';
	UPDATE Units SET							 ObsoleteTech = 'TECH_STEEL'	 WHERE Type = 'UNIT_SPEARMAN';
	UPDATE Units SET							 ObsoleteTech = 'TECH_GUNPOWDER' WHERE Type = 'UNIT_GREEK_HOPLITE';
	UPDATE Units SET							 ObsoleteTech = 'TECH_GUNPOWDER' WHERE Type = 'UNIT_PERSIAN_IMMORTAL';
	UPDATE Units SET PrereqTech = 'TECH_MINING', ObsoleteTech = 'TECH_GUNPOWDER' WHERE Type = 'UNIT_CELT_PICTISH_WARRIOR';

	UPDATE Units SET PrereqTech = 'TECH_IRON_WORKING', ObsoleteTech = 'TECH_GUNPOWDER' WHERE Type = 'UNIT_HUN_BATTERING_RAM';

	UPDATE Units SET 								ObsoleteTech = 'TECH_GUNPOWDER' WHERE Type = 'UNIT_KRIS_SWORDSMAN';
	UPDATE Units SET PrereqTech = 'TECH_IRON_WORKING'								WHERE Type = 'UNIT_IROQUOIAN_MOHAWKWARRIOR';

	UPDATE Units SET PrereqTech = 'TECH_STEEL', 	ObsoleteTech = 'TECH_GUNPOWDER' WHERE Type = 'UNIT_PIKEMAN';
	UPDATE Units SET 								ObsoleteTech = 'TECH_RIFLING'	WHERE Type = 'UNIT_DANISH_BERSERKER';

	UPDATE Units SET PrereqTech = 'TECH_CHIVALRY',	ObsoleteTech = 'TECH_RIFLING'	WHERE Type = 'UNIT_POLYNESIAN_MAORI_WARRIOR';

	UPDATE Units SET PrereqTech = 'TECH_GUNPOWDER', ObsoleteTech = 'TECH_RIFLING'			WHERE Type = 'UNIT_SPANISH_TERCIO';
	UPDATE Units SET PrereqTech = 'TECH_GUNPOWDER', ObsoleteTech = 'TECH_REPLACEABLE_PARTS' WHERE Type = 'UNIT_GERMAN_LANDSKNECHT';
	UPDATE Units SET PrereqTech = 'TECH_GUNPOWDER', ObsoleteTech = 'TECH_REPLACEABLE_PARTS' WHERE Type = 'UNIT_ZULU_IMPI';
	UPDATE Units SET 								ObsoleteTech = 'TECH_REPLACEABLE_PARTS' WHERE Type = 'UNIT_FRENCH_MUSKETEER';

	UPDATE Units SET								ObsoleteTech = 'TECH_COMBINED_ARMS'	WHERE Type = 'UNIT_ETHIOPIAN_MEHAL_SEFARI';
--	UPDATE Units SET PrereqTech = 'TECH_RIFLING',	ObsoleteTech = 'TECH_PENICILIN'		WHERE Type = 'UNIT_FRENCH_FOREIGNLEGION'; -- Entry in Honor.sql

	UPDATE Units SET ObsoleteTech = 'TECH_COMBINED_ARMS' WHERE Type = 'UNIT_GREAT_WAR_INFANTRY';

	UPDATE Units SET PrereqTech = 'TECH_RIFLING', ObsoleteTech = 'TECH_COMBINED_ARMS' WHERE Type = 'UNIT_DANISH_SKI_INFANTRY';
	UPDATE Units SET PrereqTech = 'TECH_RIFLING', ObsoleteTech = 'TECH_COMBINED_ARMS' WHERE Type = 'UNIT_SWEDISH_CAROLEAN';

	UPDATE Units SET PrereqTech = 'TECH_COMBINED_ARMS' WHERE Type = 'UNIT_INFANTRY';
	UPDATE Units SET PrereqTech = 'TECH_COMBINED_ARMS', ObsoleteTech = NULL  WHERE Type = 'UNIT_BRAZILIAN_PRACINHA';

	UPDATE Units SET ObsoleteTech = 'TECH_ADVANCED_BALLISTICS' WHERE Type = 'UNIT_ANTI_AIRCRAFT_GUN';
	UPDATE Units SET PrereqTech = 'TECH_ADVANCED_BALLISTICS' WHERE Type = 'UNIT_MOBILE_SAM';


	-- Tercio now the Renaissance land unit -- upgrdes into rifleman, medieval melee upgrade into it.
	UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_RIFLEMAN', DefaultUnitAI = 'UNITAI_DEFENSE' WHERE Type = 'UNIT_SPANISH_TERCIO';
	UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_RIFLEMAN' WHERE UnitType = 'UNIT_SPANISH_TERCIO';

	UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_TERCIO' WHERE Type = 'UNIT_PIKEMAN';
	UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_TERCIO' WHERE UnitType = 'UNIT_PIKEMAN';

	UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_TERCIO' WHERE Type = 'UNIT_LONGSWORDSMAN';
	UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_TERCIO' WHERE UnitType = 'UNIT_LONGSWORDSMAN';

	-- Recon Unit Classes
	UPDATE Units SET CombatClass = 'UNITCOMBAT_RECON', GoodyHutUpgradeUnitClass = 'UNITCLASS_MARINE' WHERE Type = 'UNIT_PARATROOPER';	
	UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_MARINE' WHERE UnitType = 'UNIT_PARATROOPER';
	
	UPDATE Units SET CombatClass = 'UNITCOMBAT_RECON', DefaultUnitAI = 'UNITAI_PARADROP', GoodyHutUpgradeUnitClass = 'UNITCLASS_XCOM_SQUAD' WHERE Type = 'UNIT_MARINE';
	INSERT INTO Unit_ClassUpgrades (UnitType, UnitClassType) VALUES ('UNIT_MARINE', 'UNITCLASS_XCOM_SQUAD');

	UPDATE Units SET CombatClass = 'UNITCOMBAT_RECON', IgnoreBuildingDefense = '1' WHERE Type = 'UNIT_XCOM_SQUAD';

	-- Recon class base sight +1
	UPDATE Units SET BaseSightRange = '2' WHERE Type = 'UNIT_SHOSHONE_PATHFINDER';
	UPDATE Units SET BaseSightRange = '3' WHERE Type IN 
	('UNIT_SCOUT',
	'UNIT_EXPLORER',
	'UNIT_COMMANDO',
	'UNIT_PARATROOPER',
	'UNIT_MARINE',
	'UNIT_XCOM_SQUAD');

	-- Pathfinder -> Scout -> Explorer
	UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_SCOUT', DefaultUnitAI = 'UNITAI_EXPLORE' WHERE Type = 'UNIT_SHOSHONE_PATHFINDER';
	UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_EXPLORER' WHERE Type = 'UNIT_SCOUT';	

	UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_SCOUT' WHERE UnitType = 'UNIT_SHOSHONE_PATHFINDER';
	UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_EXPLORER' WHERE UnitType = 'UNIT_SCOUT';

	-- Change Appearance of Marine and Paratrooper 
	UPDATE Units SET UnitArtInfo = 'ART_DEF_UNIT_PARATROOPER', UnitFlagIconOffset = '59', IconAtlas = 'UNIT_ATLAS_2', PortraitIndex = '14', UnitFlagAtlas = 'UNIT_FLAG_ATLAS' WHERE Type = 'UNIT_MARINE';

	UPDATE Units SET UnitArtInfo = 'ART_DEF_UNIT_MARINE', UnitFlagIconOffset = '19', PortraitIndex = '19', IconAtlas = 'EXPANSION_UNIT_ATLAS_1', UnitFlagAtlas = 'EXPANSION_UNIT_FLAG_ATLAS' WHERE Type = 'UNIT_PARATROOPER';

	UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_SPEARMAN' WHERE UnitType = 'UNIT_WARRIOR';
	UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_SPEARMAN' WHERE UnitType = 'UNIT_BARBARIAN_WARRIOR';
	UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_TERCIO' WHERE UnitType = 'UNIT_PIKEMAN';
	
	UPDATE Units SET Range = '4', AirInterceptRange = '4' WHERE Type = 'UNIT_MOBILE_SAM';
	UPDATE Units SET Range = '3', AirInterceptRange = '3' WHERE Type = 'UNIT_ANTI_AIRCRAFT_GUN';

