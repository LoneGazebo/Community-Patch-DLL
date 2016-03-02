-- RANGED UNITS
-- Artillery (21)

	INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_ARTILLERY' , 'PROMOTION_COVER_1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Cannon (14)

	INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_CANNON' , 'PROMOTION_COVER_1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Trebuchet (12)

	INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_TREBUCHET' , 'PROMOTION_COVER_1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Camel Archer (17)

	-- Crossbowman (13)

	UPDATE Units
	SET ObsoleteTech = 'TECH_METALLURGY'
	WHERE Type = 'UNIT_CROSSBOWMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET GoodyHutUpgradeUnitClass = 'UNITCLASS_VOLLEY_GUN'
	WHERE Type = 'UNIT_CROSSBOWMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_VOLLEY_GUN'
	WHERE UnitType = 'UNIT_CROSSBOWMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Chu-Ko-Nu (13)


	UPDATE Units
	SET ObsoleteTech = 'TECH_DYNAMITE'
	WHERE Type = 'UNIT_CHINESE_CHUKONU' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET GoodyHutUpgradeUnitClass = 'UNITCLASS_VOLLEY_GUN'
	WHERE Type = 'UNIT_CHINESE_CHUKONU' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_VOLLEY_GUN'
	WHERE UnitType = 'UNIT_CHINESE_CHUKONU' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Catapult (7)

	INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_CATAPULT' , 'PROMOTION_COVER_1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Chariot Archer (6)
	
	-- Egyptian Chariot (6)

	-- Indian Elephant (9)

	INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_INDIAN_WARELEPHANT' , 'PROMOTION_FEARED_ELEPHANT'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Archer (5)

	UPDATE Units
	SET ObsoleteTech = 'TECH_ENGINEERING'
	WHERE Type = 'UNIT_ARCHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_TRAPPING'
	WHERE Type = 'UNIT_ARCHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Set tech for slinger and bowman

	UPDATE Units
	SET PrereqTech = 'TECH_TRAPPING'
	WHERE Type = 'UNIT_INCAN_SLINGER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_MACHINERY'
	WHERE Type = 'UNIT_INCAN_SLINGER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Horse Archer
	
	-- Mounted Bowman

	-- Update ranged Knights and make them Mounted Crossbowmen replacements.

	UPDATE Units
	SET Class = 'UNITCLASS_MOUNTED_BOWMAN'
	WHERE Type = 'UNIT_ARABIAN_CAMELARCHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

	UPDATE Civilization_UnitClassOverrides
	Set UnitClassType = 'UNITCLASS_MOUNTED_BOWMAN'
	WHERE UnitType = 'UNIT_ARABIAN_CAMELARCHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_PHYSICS'
	WHERE Type = 'UNIT_ARABIAN_CAMELARCHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

	UPDATE Units
	SET GoodyHutUpgradeUnitClass = 'UNITCLASS_CUIRASSIER'
	WHERE Type = 'UNIT_ARABIAN_CAMELARCHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Fast Medieval ranged Unit, weak to Pikemen. Only the Arabians may build it. It is much stronger (defensively) than the Heavy Skirmisher which it replaces.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_CAMEL_ARCHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Camel Archer is the Arabian unique unit, replacing the Heavy Skirmisher. The Camel Archer is a powerful ranged attack unit, allowing it to do damage to enemies 2 hexes away. It can move after combat. As a mounted unit, the Camel Archer is vulnerable to Pikemen, though nowhere near as vulnerable as the Heavy Skirmisher.'
	WHERE Tag = 'TXT_KEY_UNIT_ARABIAN_CAMELARCHER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

	-- Composite Bowman (7)

	UPDATE Units
	SET PrereqTech = 'TECH_ENGINEERING'
	WHERE Type = 'UNIT_COMPOSITE_BOWMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Machine Gun (60)

	-- Gatling Gun (30)

	UPDATE Units
	SET IgnoreBuildingDefense = '1'
	WHERE Type = 'UNIT_GATLINGGUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Bazooka (85)

	-- Rocket Artillery (45)

	INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_ROCKET_ARTILLERY' , 'PROMOTION_COVER_1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_ADVANCED_BALLISTICS'
	WHERE Type = 'UNIT_ROCKET_ARTILLERY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_ADVANCED_BALLISTICS'
	WHERE Type = 'UNIT_ARTILLERY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Gatling Gun -- Moved to Dynamite
	UPDATE Units
	SET PrereqTech = 'TECH_DYNAMITE'
	WHERE Type = 'UNIT_GATLINGGUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Gatling Gun, Machine Gun, Bazooka - make Range 2 (instead of 1) to help AI

	UPDATE Units
	SET Range = '2'
	WHERE Type = 'UNIT_GATLINGGUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Ranged infantry Unit of the mid-game that weakens nearby enemy units.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_GATLINGGUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Gatling Gun is a mid-game non-siege ranged unit that can lay down a terrifying hail of bullets. It is much more powerful than earlier ranged units like the Volley Gun, but it is much weaker than other military units of its era. As such, it should be used as a source of attrition. It weakens nearby enemy units, and gains bonus strength when defending. When attacking, the Gatling Gun deals less damage to Armored or fortified Units, as well as cities. Put Gatling Guns in your cities or on chokepoints for optimal defensive power.'
	WHERE Tag = 'TXT_KEY_UNIT_GATLINGGUN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Range = '2'
	WHERE Type = 'UNIT_MACHINE_GUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Late-game ranged Unit that weakens nearby enemy units.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_MACHINE_GUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Machine Gun is the penultimate non-siege ranged unit, and can lay down a terrifying hail of suppressive fire. It is more powerful than earlier ranged units, but it is much weaker than other military units of its era. As such, it should be used as a source of attrition. Like the Gatling Gun, it weakens nearby enemy units. When attacking, the Machine Gun deals less damage to Armored or fortified Units, as well as cities. It is vulnerable to melee attack. Put Machine Guns in your city for defense, or use them to control chokepoints.'
	WHERE Tag = 'TXT_KEY_UNIT_MACHINE_GUN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Range = '2'
	WHERE Type = 'UNIT_BAZOOKA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Atomic era ranged Unit that becomes stronger as it gets closer to the enemy, and deals great damage to Armor Units.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_BAZOOKA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Bazooka is the last non-siege ranged unit, and is capable of truly terrfiying amounts of damage, especially to Armor units. It is the most powerful ranged unit, but it is much weaker than other military units of its era. As such, it should be used as a source of attrition. Like the Machine Gun, it weakens nearby enemy units. When attacking, the Bazooka deals less damage to fortified Units and cities, but deals additional damage to Armored units. This makes it a great defense unit.'
	WHERE Tag = 'TXT_KEY_UNIT_BAZOOKA_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Promotions for Gatilng/Machine/Bazooka to offset range increase

	INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_GATLINGGUN' , 'PROMOTION_MODERN_RANGED_PENALTY_I'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_MACHINE_GUN' , 'PROMOTION_MODERN_RANGED_PENALTY_I'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_BAZOOKA' , 'PROMOTION_MODERN_RANGED_PENALTY_II'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Make Rocket Artillery cost oil, not aluminum:
	UPDATE Unit_ResourceQuantityRequirements
	SET ResourceType = 'RESOURCE_IRON'
	WHERE UnitType = 'UNIT_ROCKET_ARTILLERY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Catapults, Trebuchets, and Artillery should cost Iron
	INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType, Cost)
	SELECT 'UNIT_CATAPULT', 'RESOURCE_IRON', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType, Cost)
	SELECT 'UNIT_TREBUCHET', 'RESOURCE_IRON', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType, Cost)
	SELECT 'UNIT_CANNON', 'RESOURCE_IRON', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType, Cost)
	SELECT 'UNIT_ARTILLERY', 'RESOURCE_IRON', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Move Cannon Up
	UPDATE Units
	SET ObsoleteTech = 'TECH_GUNPOWDER'
	WHERE Type = 'UNIT_TREBUCHET' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_DYNAMITE'
	WHERE Type = 'UNIT_KOREAN_HWACHA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_KOREAN_HWACHA' , 'PROMOTION_COVER_1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_KOREAN_HWACHA' , 'PROMOTION_BLITZ'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_GUNPOWDER'
	WHERE Type = 'UNIT_CANNON' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- They should also be a little stronger ranged, to compensate

	-- Ranged mounted units should have mounted trait in spite of not being in mounted combatclass

	UPDATE Units
	SET IsMounted = '1'
	WHERE CombatClass = 'UNITCOMBAT_ARCHER' AND Moves >= 3 AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );