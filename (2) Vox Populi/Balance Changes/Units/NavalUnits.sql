-- NAVAL UNITS
		-- need to change things about the barbarian Galley to match the new base unit
	UPDATE Units SET
		Cost = 70,
		Moves = 3,
		PrereqTech = 'TECH_SAILING', 
		ObsoleteTech = 'TECH_PHILOSOPHY'
	WHERE Type = 'UNIT_GALLEY';
	
	-- Frigate upgrades into new Cruiser
	UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_CRUISER' WHERE UnitType = 'UNIT_FRIGATE';
	
	-- Tech Ranged

	UPDATE Units SET PrereqTech = 'TECH_OPTICS', ObsoleteTech = 'TECH_GUILDS' WHERE Type = 'UNIT_LIBURNA';
	UPDATE Units SET PrereqTech = 'TECH_OPTICS', ObsoleteTech = 'TECH_GUILDS' WHERE Type = 'UNIT_BYZANTINE_DROMON';

	UPDATE Units SET PrereqTech = 'TECH_GUILDS', ObsoleteTech = 'TECH_NAVIGATION' WHERE Type = 'UNIT_GALLEASS';
	UPDATE Units SET PrereqTech = 'TECH_GUILDS', ObsoleteTech = 'TECH_DYNAMITE'   WHERE Type = 'UNIT_VENETIAN_GALLEASS';
	
	UPDATE Units SET ObsoleteTech = 'TECH_DYNAMITE' 		WHERE Type = 'UNIT_FRIGATE';
	UPDATE Units SET ObsoleteTech = 'TECH_NUCLEAR_FISSION'	WHERE Type = 'UNIT_ENGLISH_SHIPOFTHELINE';

	UPDATE Units SET 										ObsoleteTech = 'TECH_NUCLEAR_FISSION'	WHERE Type = 'UNIT_CRUISER';
	UPDATE Units SET PrereqTech = 'TECH_NUCLEAR_FISSION',	ObsoleteTech = NULL						WHERE Type = 'UNIT_BATTLESHIP';

	UPDATE Units SET Moves = '4' WHERE Type IN (
	'UNIT_LIBURNA',
	'UNIT_BYZANTINE_DROMON',  -- half moves in ocean
	'UNIT_GALLEASS', -- half moves in ocean
	'UNIT_VENETIAN_GALLEASS', -- half moves in ocean
	'UNIT_FRIGATE',
	'UNIT_ENGLISH_SHIPOFTHELINE');
	
	-- Tech Melee
	
	UPDATE Units SET PrereqTech = 'TECH_SAILING', ObsoleteTech = 'TECH_PHILOSOPHY' 	WHERE Type = 'UNIT_GALLEY';
	UPDATE Units SET PrereqTech = 'TECH_SAILING', ObsoleteTech = 'TECH_PHILOSOPHY' 	WHERE Type = 'UNIT_VP_GALLEY';
	UPDATE Units SET PrereqTech = 'TECH_PHILOSOPHY', ObsoleteTech = 'TECH_COMPASS'		WHERE Type = 'UNIT_TRIREME';
	UPDATE Units SET PrereqTech = 'TECH_PHILOSOPHY', ObsoleteTech = 'TECH_NAVIGATION'	WHERE Type = 'UNIT_CARTHAGINIAN_QUINQUEREME';

	UPDATE Units SET PrereqTech = 'TECH_COMPASS', ObsoleteTech = 'TECH_NAVIGATION'			WHERE Type = 'UNIT_CARAVEL';
	UPDATE Units SET PrereqTech = 'TECH_COMPASS', ObsoleteTech = 'TECH_INDUSTRIALIZATION'	WHERE Type = 'UNIT_PORTUGUESE_NAU';
	UPDATE Units SET PrereqTech = 'TECH_COMPASS', ObsoleteTech = 'TECH_INDUSTRIALIZATION'	WHERE Type = 'UNIT_KOREAN_TURTLE_SHIP';

	UPDATE Units SET PrereqTech = 'TECH_NAVIGATION',	ObsoleteTech = 'TECH_INDUSTRIALIZATION' WHERE Type = 'UNIT_PRIVATEER';
	UPDATE Units SET PrereqTech = 'TECH_ASTRONOMY',		ObsoleteTech = 'TECH_ROCKETRY'			WHERE Type = 'UNIT_DUTCH_SEA_BEGGAR';
	
	UPDATE Units SET PrereqTech = 'TECH_INDUSTRIALIZATION', ObsoleteTech = 'TECH_ROCKETRY'  WHERE Type = 'UNIT_IRONCLAD';
	UPDATE Units SET PrereqTech = 'TECH_ROCKETRY', ObsoleteTech = 'TECH_ADVANCED_BALLISTICS' WHERE Type = 'UNIT_DESTROYER';
	UPDATE Units SET PrereqTech = 'TECH_ADVANCED_BALLISTICS' WHERE Type = 'UNIT_MISSILE_CRUISER';

	UPDATE Units SET PrereqTech = 'TECH_NUCLEAR_FISSION', ObsoleteTech = 'TECH_STEALTH' WHERE Type = 'UNIT_SUBMARINE';
	UPDATE Units SET PrereqTech = 'TECH_STEALTH' WHERE Type = 'UNIT_NUCLEAR_SUBMARINE';
	UPDATE Units SET PrereqTech = 'TECH_COMPUTERS' WHERE Type = 'UNIT_CARRIER';

UPDATE Units SET Moves = '3' WHERE Type IN (
	'UNIT_GALLEY',
	'UNIT_VP_GALLEY'); -- half moves in ocean

	UPDATE Units SET Moves = '4' WHERE Type IN (
	'UNIT_KOREAN_TURTLE_SHIP', -- half moves in ocean
	'UNIT_CARAVEL',	
	'UNIT_PORTUGUESE_NAU',
	'UNIT_PRIVATEER',
	'UNIT_DUTCH_SEA_BEGGAR');

	UPDATE Units SET Moves = '5' WHERE Type IN (
	'UNIT_TRIREME', -- half moves in ocean
	'UNIT_CARTHAGINIAN_QUINQUEREME', -- half moves in ocean
	'UNIT_IRONCLAD',
	'UNIT_DESTROYER');
	UPDATE Units SET Moves = '6' WHERE Type = 'UNIT_MISSILE_CRUISER';
	
	UPDATE Units SET AirInterceptRange = '4' WHERE Type = 'UNIT_CARRIER';


	UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_PRIVATEER' WHERE UnitType = 'UNIT_CARAVEL';
	UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_IRONCLAD' WHERE UnitType = 'UNIT_PRIVATEER';
	
	UPDATE Units SET DefaultUnitAI = 'UNITAI_ATTACK_SEA' WHERE Type IN (
	'UNIT_CARAVEL',
	'UNIT_PRIVATEER',
	'UNIT_DUTCH_SEA_BEGGAR');
	
	UPDATE Units SET UnitFlagAtlas = 'CORVETTE_FLAG_ATLAS', UnitFlagIconOffset = '0', IconAtlas = 'COMMUNITY_ATLAS', UnitArtInfo = 'ART_DEF_UNIT_CORVETTE', PortraitIndex = '60' WHERE Type = 'UNIT_PRIVATEER';

	-- Missile Cruiser now Melee, upgrade from Destroyer
	-- Upgrade from Destroyer
	INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType) VALUES ('UNITCLASS_MISSILE_CRUISER' , 'UNIT_DESTROYER');
	UPDATE Units SET RangedCombat = '0', Range = '0', CombatClass = 'UNITCOMBAT_NAVALMELEE', DefaultUnitAI = 'UNITAI_ATTACK_SEA' WHERE Type = 'UNIT_MISSILE_CRUISER';

	-- Resource Changes
	DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType IN 
	('UNIT_BATTLESHIP',
	'UNIT_IRONCLAD',
	'UNIT_FRIGATE',
	'UNIT_ENGLISH_SHIPOFTHELINE');
	
	INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType, Cost) VALUES 
	('UNIT_BATTLESHIP', 'RESOURCE_IRON', '1'),
	('UNIT_SUBMARINE', 'RESOURCE_ALUMINUM', '1'),
	('UNIT_NUCLEAR_SUBMARINE', 'RESOURCE_ALUMINUM', '1'),
	('UNIT_CARRIER', 'RESOURCE_OIL', '1');

	-- Ranged ships - now start at range 1, jump to range 2 in industrial
	UPDATE Units SET Range = '1' WHERE Type IN 
	('UNIT_LIBURNA',
	'UNIT_BYZANTINE_DROMON',
	'UNIT_GALLEASS',
	'UNIT_VENETIAN_GALLEASS');

	UPDATE Units SET Range = '2' WHERE Type IN 
	('UNIT_FRIGATE',
	'UNIT_ENGLISH_SHIPOFTHELINE',
	'UNIT_CARRIER',
	'UNIT_CRUISER',
	'UNIT_BATTLESHIP');

	UPDATE Units SET Range = '1' WHERE Type IN ('UNIT_SUBMARINE', 'UNIT_NUCLEAR_SUBMARINE');
	
