-- RANGED UNITS
	-- Prereq Techs 
	-- Archers 
	UPDATE Units SET PrereqTech = 'TECH_AGRICULTURE', ObsoleteTech = 'TECH_CALENDAR' WHERE Type = 'UNIT_VP_SLINGER';
	UPDATE Units SET PrereqTech = 'TECH_AGRICULTURE', ObsoleteTech = 'TECH_CURRENCY' WHERE Type = 'UNIT_INCAN_SLINGER';

	UPDATE Units SET PrereqTech = 'TECH_CALENDAR', ObsoleteTech = 'TECH_CURRENCY'  WHERE Type = 'UNIT_ARCHER';
	UPDATE Units SET PrereqTech = 'TECH_MATHEMATICS', ObsoleteTech = 'TECH_MACHINERY' WHERE Type = 'UNIT_BABYLONIAN_BOWMAN';
	UPDATE Units SET PrereqTech = 'TECH_CALENDAR' WHERE Type = 'UNIT_BARBARIAN_ARCHER';

	UPDATE Units SET PrereqTech = 'TECH_CURRENCY', ObsoleteTech = 'TECH_MACHINERY'  WHERE Type = 'UNIT_COMPOSITE_BOWMAN';
	UPDATE Units SET PrereqTech = 'TECH_MATHEMATICS', ObsoleteTech = 'TECH_METALLURGY'  WHERE Type = 'UNIT_MAYAN_ATLATLIST';

	UPDATE Units SET ObsoleteTech = 'TECH_METALLURGY' WHERE Type = 'UNIT_CROSSBOWMAN';
	UPDATE Units SET PrereqTech = 'TECH_MACHINERY', ObsoleteTech = 'TECH_DYNAMITE' WHERE Type = 'UNIT_ENGLISH_LONGBOWMAN';
	UPDATE Units SET ObsoleteTech = 'TECH_DYNAMITE' WHERE Type = 'UNIT_CHINESE_CHUKONU';
	
	UPDATE Units SET PrereqTech = 'TECH_METALLURGY', ObsoleteTech = 'TECH_DYNAMITE' WHERE Type = 'UNIT_MUSKETMAN';
	UPDATE Units SET PrereqTech = 'TECH_METALLURGY', ObsoleteTech = 'TECH_BALLISTICS' WHERE Type = 'UNIT_AMERICAN_MINUTEMAN';
	UPDATE Units SET ObsoleteTech = 'TECH_BALLISTICS' WHERE Type = 'UNIT_OTTOMAN_JANISSARY';

	UPDATE Units SET ObsoleteTech = 'TECH_MOBILE_TACTICS' WHERE Type = 'UNIT_MACHINE_GUN';
	UPDATE Units SET PrereqTech = 'TECH_DYNAMITE' WHERE Type = 'UNIT_GATLINGGUN';
	UPDATE Units SET PrereqTech = 'TECH_MOBILE_TACTICS' WHERE Type = 'UNIT_BAZOOKA';

	-- Siege
	UPDATE Units SET PrereqTech = 'TECH_MATHEMATICS' WHERE Type = 'UNIT_CATAPULT';
	UPDATE Units SET PrereqTech = 'TECH_IRON_WORKING', ObsoleteTech = 'TECH_PHYSICS' WHERE Type = 'UNIT_ROMAN_BALLISTA';

	UPDATE Units SET ObsoleteTech = 'TECH_GUNPOWDER' WHERE Type = 'UNIT_TREBUCHET';
	UPDATE Units SET ObsoleteTech = 'TECH_RIFLING' WHERE Type = 'UNIT_KOREAN_HWACHA';

	UPDATE Units SET PrereqTech = 'TECH_GUNPOWDER', ObsoleteTech = 'TECH_RIFLING' WHERE Type = 'UNIT_CANNON';
	UPDATE Units SET PrereqTech = 'TECH_BALLISTICS', ObsoleteTech = 'TECH_ADVANCED_BALLISTICS' WHERE Type = 'UNIT_ARTILLERY';
	UPDATE Units SET PrereqTech = 'TECH_ADVANCED_BALLISTICS' WHERE Type = 'UNIT_ROCKET_ARTILLERY';

	-- Musketman now ranged (Tercio Melee) - UUs splitting down middle
	UPDATE Units SET Range = '2', CombatClass = 'UNITCOMBAT_ARCHER', DefaultUnitAI = 'UNITAI_RANGED' WHERE Type IN 
	('UNIT_AMERICAN_MINUTEMAN',
	'UNIT_MUSKETMAN',
	'UNIT_OTTOMAN_JANISSARY');
	
	-- Hand Axe
	UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_COMPOSITE_BOWMAN' WHERE Type = 'UNIT_BARBARIAN_AXMAN';
	UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_COMPOSITE_BOWMAN' WHERE UnitType = 'UNIT_BARBARIAN_AXMAN';

	-- Upgrades	
	UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_GATLINGGUN' WHERE Type = 'UNIT_MUSKETMAN';
	UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_GATLINGGUN' WHERE UnitType = 'UNIT_MUSKETMAN';

	UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_MUSKETMAN' WHERE Type = 'UNIT_CROSSBOWMAN';
	UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_MUSKETMAN' WHERE UnitType = 'UNIT_CROSSBOWMAN';
	UPDATE Units SET IgnoreBuildingDefense = '1' WHERE Type = 'UNIT_GATLINGGUN';
	
	-- Gatling Gun, Machine Gun, Bazooka - make Range 2 (instead of 1) to help AI
	UPDATE Units SET Range = '2', DefaultUnitAI = 'UNITAI_RANGED' WHERE Type IN 
	('UNIT_GATLINGGUN',
	'UNIT_MACHINE_GUN',
	'UNIT_BAZOOKA');

	-- Make Rocket Artillery cost oil, not aluminum:
	UPDATE Unit_ResourceQuantityRequirements SET ResourceType = 'RESOURCE_IRON' WHERE UnitType = 'UNIT_ROCKET_ARTILLERY';

	-- Cannons and Artillery should cost Iron
	INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType, Cost) VALUES 
	('UNIT_CANNON', 'RESOURCE_IRON', '1'),
	('UNIT_ARTILLERY', 'RESOURCE_IRON', '1');

	-- Cannon should also transition into new field gun class
	UPDATE Units SET GoodyHutUpgradeUnitClass = 'UNITCLASS_FIELD_GUN' WHERE Type = 'UNIT_CANNON';
	UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_FIELD_GUN' WHERE UnitType = 'UNIT_CANNON';



	
