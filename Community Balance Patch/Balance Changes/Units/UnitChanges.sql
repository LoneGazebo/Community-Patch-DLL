-- Workers Reduced Work Rate to slow down early growth
	UPDATE Units
	SET WorkRate = '90'
	WHERE Type = 'UNIT_WORKER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Great Prophets no longer capturable
	DELETE FROM Units
	WHERE Type = 'UNIT_PROPHET' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Great Writer culture boost nerfed slightly.

	UPDATE Units
	SET BaseCultureTurnsToCount = '6'
	WHERE Type = 'UNIT_WRITER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Great Scientist science boost nerfed slightly.

	UPDATE Units
	SET BaseBeakersTurnsToCount = '5'
	WHERE Type = 'UNIT_SCIENTIST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Merchant gold boosted

	UPDATE Units
	SET NumGoldPerEra = '200'
	WHERE Type = 'UNIT_MERCHANT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Engineer production boost nerfed slightly.

	UPDATE Units
	SET BaseHurry = '250'
	WHERE Type = 'UNIT_ENGINEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET HurryMultiplier = '25'
	WHERE Type = 'UNIT_ENGINEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
-- Ranged Units -- Reduced Combat Strength to make Melee units more important

-- NAVAL
	-- Missile Cruiser (80)
	UPDATE Units
	SET Combat = '75'
	WHERE Type = 'UNIT_MISSILE_CRUISER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Nuclear Submarine (50)
	UPDATE Units
	SET Combat = '45'
	WHERE Type = 'UNIT_NUCLEAR_SUBMARINE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Battleship (55)
	UPDATE Units
	SET Combat = '50'
	WHERE Type = 'UNIT_BATTLESHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Submarine (35)
	UPDATE Units
	SET Combat = '30'
	WHERE Type = 'UNIT_SUBMARINE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Frigate (25)
	UPDATE Units
	SET Combat = '20'
	WHERE Type = 'UNIT_FRIGATE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Ship of the Line (30)
	UPDATE Units
	SET Combat = '25'
	WHERE Type = 'UNIT_ENGLISH_SHIPOFTHELINE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Galleass (16)
	UPDATE Units
	SET Combat = '11'
	WHERE Type = 'UNIT_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- LAND
	-- Impi
	UPDATE Units
	SET Combat = '18'
	WHERE Type = 'UNIT_ZULU_IMPI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Samurai
	UPDATE Units
	SET Combat = '28'
	WHERE Type = 'UNIT_JAPANESE_SAMURAI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Artillery (21)
	UPDATE Units
	SET Combat = '17'
	WHERE Type = 'UNIT_ARTILLERY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Cannon (14)
	UPDATE Units
	SET Combat = '10'
	WHERE Type = 'UNIT_CANNON' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Trebuchet (12)
	UPDATE Units
	SET Combat = '8'
	WHERE Type = 'UNIT_TREBUCHET' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Camel Archer (17)
	UPDATE Units
	SET Combat = '15'
	WHERE Type = 'UNIT_ARABIAN_CAMELARCHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Crossbowman (13)
	UPDATE Units
	SET Combat = '9'
	WHERE Type = 'UNIT_CROSSBOWMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Chu-Ko-Nu (13)
	UPDATE Units
	SET Combat = '9'
	WHERE Type = 'UNIT_CHINESE_CHUKONU' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET RangedCombat = '17'
	WHERE Type = 'UNIT_CHINESE_CHUKONU' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Catapult (7)
	UPDATE Units
	SET Combat = '5'
	WHERE Type = 'UNIT_CATAPULT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Chariot Archer (6)
	UPDATE Units
	SET Combat = '4'
	WHERE Type = 'UNIT_CHARIOT_ARCHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET RangedCombat = '8'
	WHERE Type = 'UNIT_CHARIOT_ARCHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	-- Egyptian Chariot (6)
	UPDATE Units
	SET Combat = '5'
	WHERE Type = 'UNIT_EGYPTIAN_WARCHARIOT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Indian Elephant (9)
	UPDATE Units
	SET Combat = '10'
	WHERE Type = 'UNIT_INDIAN_WARELEPHANT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET RangedCombat = '14'
	WHERE Type = 'UNIT_INDIAN_WARELEPHANT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	UPDATE Units
	SET ObsoleteTech = 'TECH_GUNPOWDER'
	WHERE Type = 'UNIT_INDIAN_WARELEPHANT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_MATHEMATICS'
	WHERE Type = 'UNIT_INDIAN_WARELEPHANT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Archer (5)
	UPDATE Units
	SET Combat = '4'
	WHERE Type = 'UNIT_ARCHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

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

	-- Composite Bowman (7)
	UPDATE Units
	SET Combat = '5'
	WHERE Type = 'UNIT_COMPOSITE_BOWMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_ENGINEERING'
	WHERE Type = 'UNIT_COMPOSITE_BOWMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Machine Gun (60)
	UPDATE Units
	SET Combat = '40'
	WHERE Type = 'UNIT_MACHINE_GUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET RangedCombat = '40'
	WHERE Type = 'UNIT_MACHINE_GUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Gatling Gun (30)
	UPDATE Units
	SET Combat = '25'
	WHERE Type = 'UNIT_GATLINGGUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET RangedCombat = '25'
	WHERE Type = 'UNIT_GATLINGGUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Bazooka (85)
	UPDATE Units
	SET Combat = '50'
	WHERE Type = 'UNIT_BAZOOKA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET RangedCombat = '50'
	WHERE Type = 'UNIT_BAZOOKA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Rocket Artillery (45)
	UPDATE Units
	SET Combat = '38'
	WHERE Type = 'UNIT_ROCKET_ARTILLERY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Delete Anti-Tank Gun, combine stats with Bazooka and Missile Pod (help AI)

	DELETE FROM Units
	Where Type = 'UNIT_ANTI_TANK_GUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM Civilization_UnitClassOverrides
	WHERE UnitType = 'UNIT_ANTI_TANK_GUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM Unit_AITypes
	WHERE UnitType = 'UNIT_ANTI_TANK_GUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM Unit_ClassUpgrades
	WHERE UnitType = 'UNIT_ANTI_TANK_GUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM Unit_FreePromotions
	WHERE UnitType = 'UNIT_ANTI_TANK_GUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM Unit_Flavors
	WHERE UnitType = 'UNIT_ANTI_TANK_GUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM Unit_ResourceQuantityRequirements
	WHERE UnitType = 'UNIT_ANTI_TANK_GUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM UnitGameplay2DScripts
	WHERE UnitType = 'UNIT_ANTI_TANK_GUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Update Lancers to become Cavalry, and all UUs

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_CAVALRY'
	WHERE UnitType = 'UNIT_POLISH_WINGED_HUSSAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_CAVALRY'
	WHERE UnitType = 'UNIT_OTTOMAN_SIPAHI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_CAVALRY'
	WHERE UnitType = 'UNIT_LANCER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_GUNPOWDER'
	WHERE Type = 'UNIT_POLISH_WINGED_HUSSAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_GUNPOWDER'
	WHERE Type = 'UNIT_OTTOMAN_SIPAHI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Change Name of Great War Infantry/Bomber to be more 'generic'
	UPDATE Language_en_US
	SET Text = 'Rifleman'
	WHERE Tag = 'TXT_KEY_UNIT_GREAT_WAR_INFANTRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Rifleman is the basic Modern era combat unit. It is significantly stronger than its predecessor, the Fusilier.'
	WHERE Tag = 'TXT_KEY_UNIT_GREAT_WAR_INFANTRY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_ATOMIC_THEORY'
	WHERE Type = 'UNIT_GREAT_WAR_INFANTRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Allows you to build the basic land unit [COLOR_POSITIVE_TEXT]Rifleman[ENDCOLOR], and the [COLOR_POSITIVE_TEXT]Military Base[ENDCOLOR], which boosts City defense.'
	WHERE Tag = 'TXT_KEY_TECH_REPLACEABLE_PARTS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Bomber'
	WHERE Tag = 'TXT_KEY_UNIT_WWI_BOMBER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Bomber is an early air unit. It is effective against ground targets, less so against naval targets, and it is quite vulnerable to enemy aircraft. The Bomber can be based on a player-owned city or aboard a carrier. It can move from base to base and perform missions within its range of "6." Use it to attack enemy units and cities. When possible, send in triplanes or fighters first to "use up" enemy anti-aircraft defenses for that turn. See the rules on Aircraft for more information.'
	WHERE Tag = 'TXT_KEY_UNIT_WWI_BOMBER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Borrowed for WWI Bomber
	UPDATE Language_en_US
	SET Text = 'Heavy Bomber'
	WHERE Tag = 'TXT_KEY_UNIT_BOMBER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Heavy Bomber is an air unit. It is effective against ground targets, less so against naval targets, and it is quite vulnerable to enemy aircraft. The Heavy Bomber can be based on a player-owned city or aboard a carrier. It can move from base to base and perform missions within its range of "10." Use Bombers to attack enemy units and cities. When possible, send in fighters first to "use up" enemy anti-aircraft defenses for that turn. See the rules on Aircraft for more information.'
	WHERE Tag = 'TXT_KEY_UNIT_BOMBER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Borrowed for WWI Infantry - Fusiliers more fiting.
	UPDATE Language_en_US
	SET Text = 'Fusiliers'
	WHERE Tag = 'TXT_KEY_UNIT_RIFLEMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Fusilier is the gunpowder unit following the musketman. It is significantly more powerful than the musketman, giving the army with the advanced units a big advantage over civs which have not yet upgraded to the new unit.'
	WHERE Tag = 'TXT_KEY_UNIT_RIFLEMAN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Fusilier[ENDCOLOR], a front-line infantry unit of the mid-game eras.'
	WHERE Tag = 'TXT_KEY_TECH_RIFLING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Moved Infantry to Atomic Theory
	UPDATE Units
	SET PrereqTech = 'TECH_ATOMIC_THEORY'
	WHERE Type = 'UNIT_INFANTRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Moved to an earlier date (make stronger)
	UPDATE Units
	SET PrereqTech = 'TECH_FLIGHT'
	WHERE Type = 'UNIT_BRAZILIAN_PRACINHA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_FUTURE_TECH'
	WHERE Type = 'UNIT_BRAZILIAN_PRACINHA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );


	UPDATE Language_en_US
	SET Text = 'Infantry is the basic Industrial era combat unit. It is significantly stronger than its predecessor, the Rifleman. Modern combat is increasingly complex, and on its own an Infantry unit is vulnerable to air, artillery and tank attack. When possible Infantry should be supported by artillery, tanks, and air (or anti-air) units.'
	WHERE Tag = 'TXT_KEY_UNIT_INFANTRY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Research Lab[ENDCOLOR], a building which improves [ICON_RESEARCH] Science in a City.'
	WHERE Tag = 'TXT_KEY_TECH_PLASTICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Lancer -- Moved to Gunpowder, Cheaper
	UPDATE Units
	SET PrereqTech = 'TECH_GUNPOWDER'
	WHERE Type = 'UNIT_LANCER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Combat = '30'
	WHERE Type = 'UNIT_LANCER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_MILITARY_SCIENCE'
	WHERE Type = 'UNIT_LANCER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM Unit_FreePromotions
	WHERE UnitType = 'UNIT_LANCER' AND PromotionType = 'PROMOTION_FORMATION_1' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Knight and Knight UUs -- Upgrade to Lancer
	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_LANCER'
	WHERE UnitType = 'UNIT_KNIGHT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_GUNPOWDER'
	WHERE Type = 'UNIT_KNIGHT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_LANCER'
	WHERE UnitType = 'UNIT_ARABIAN_CAMELARCHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_LANCER'
	WHERE UnitType = 'UNIT_SPANISH_CONQUISTADOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_LANCER' 
	WHERE UnitType = 'UNIT_MONGOLIAN_KESHIK' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_LANCER'
	WHERE UnitType = 'UNIT_SONGHAI_MUSLIMCAVALRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_LANCER'
	WHERE UnitType = 'UNIT_SIAMESE_WARELEPHANT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Pikeman and Pikeman UUs -- Upgrade to Musketman
	UPDATE Units
	SET ObsoleteTech = 'TECH_GUNPOWDER'
	WHERE Type = 'UNIT_PIKEMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_MUSKETMAN'
	WHERE UnitType = 'UNIT_PIKEMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_MUSKETMAN'
	WHERE UnitType = 'UNIT_GERMAN_LANDSKNECHT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_MUSKETMAN'
	WHERE UnitType = 'UNIT_ZULU_IMPI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Frigate Upgrade into Cruiser
	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_CRUISER'
	WHERE UnitType = 'UNIT_FRIGATE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Moves = '4'
	WHERE Type = 'UNIT_FRIGATE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	UPDATE Units
	SET ObsoleteTech = 'TECH_DYNAMITE'
	WHERE Type = 'UNIT_FRIGATE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Moves = '4'
	WHERE Type = 'UNIT_ENGLISH_SHIPOFTHELINE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_CRUISER'
	WHERE UnitType = 'UNIT_ENGLISH_SHIPOFTHELINE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Move Caravel to Compass
	UPDATE Units
	SET PrereqTech = 'TECH_COMPASS'
	WHERE Type = 'UNIT_CARAVEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_PRIVATEER'
	WHERE UnitType = 'UNIT_CARAVEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	UPDATE Language_en_US
	SET Text = 'The Caravel is a significant upgrade to your naval power. A Melee unit, it is stronger and faster than the aging Trireme, and it can enter Deep Ocean hexes. It is, however, slow, making it vulnerable to enemy ranged units. Use it to (gradually) explore the world, or to defend your home cities.'
	WHERE Tag = 'TXT_KEY_UNIT_CARAVEL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );
	
	UPDATE Language_en_US
	SET Text = 'Late-Medieval exploration Unit that can enter the Ocean, but moves slowly. Fights as a naval melee unit.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_CARAVEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );
	
	UPDATE Units
	SET ObsoleteTech = 'TECH_ASTRONOMY'
	WHERE Type = 'UNIT_CARAVEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Moves = '3'
	WHERE Type = 'UNIT_CARAVEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM Unit_FreePromotions
	WHERE UnitType = 'UNIT_CARAVEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_IRONCLAD'
	WHERE UnitType = 'UNIT_PORTUGUESE_NAU' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_COMPASS'
	WHERE Type = 'UNIT_PORTUGUESE_NAU' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Combat = '25'
	WHERE Type = 'UNIT_PORTUGUESE_NAU' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Moves = '4'
	WHERE Type = 'UNIT_PORTUGUESE_NAU' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Move Galleass to Optics
	UPDATE Units
	SET PrereqTech = 'TECH_PHYSICS'
	WHERE Type = 'UNIT_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_NAVIGATION'
	WHERE Type = 'UNIT_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Move Privateer to Astronomy, make Caravel upgrade into it, rename Corvette
	UPDATE Units
	SET PrereqTech = 'TECH_ASTRONOMY'
	WHERE Type = 'UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_ASTRONOMY'
	WHERE Type = 'UNIT_DUTCH_SEA_BEGGAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Corvette'
	WHERE Tag = 'TXT_KEY_UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Naval Unit that specializes in melee combat and quick movement.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Naval Unit that attacks as a melee unit. Though it comes quickly after the Caravel, it is significantly stronger and faster than the Caravel.'
	WHERE Tag = 'TXT_KEY_UNIT_PRIVATEER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_STEAM_POWER'
	WHERE Type = 'UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET UnitFlagAtlas = 'EXPANSION2_UNIT_FLAG_ATLAS'
	WHERE Type = 'UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET UnitFlagIconOffset = '19'
	WHERE Type = 'UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE ArtDefine_StrategicView
	SET Asset = 'SV_VenetianGalea.dds'
	WHERE StrategicViewType = 'ART_DEF_UNIT_XP_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Combat = '30'
	WHERE Type = 'UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Combat = '30'
	WHERE Type = 'UNIT_DUTCH_SEA_BEGGAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_IRONCLAD'
	WHERE UnitType = 'UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_IRONCLAD'
	WHERE UnitType = 'UNIT_DUTCH_SEA_BEGGAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM Unit_FreePromotions
	WHERE UnitType = 'UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Submarines -- Can Now Attack Coastal Cities -- Reduced Ranged Damage, but boosted it versus boats. This will help AI a TON
	-- Moved to Atomic Era (Submarine) -- Upgrade for Destroyer

	UPDATE Units
	SET RangedCombat = '60'
	WHERE Type = 'UNIT_SUBMARINE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_SUBMARINE' , 'PROMOTION_TARGETING_1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_NUCLEAR_SUBMARINE' , 'PROMOTION_TARGETING_1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_PENICILIN'
	WHERE Type = 'UNIT_SUBMARINE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Battleship -- Upgrades to Missile Cruiser

	INSERT INTO Unit_ClassUpgrades (UnitClassType, UnitType)
	SELECT 'UNITCLASS_MISSILE_CRUISER' , 'UNIT_BATTLESHIP'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_ROBOTICS'
	WHERE Type = 'UNIT_BATTLESHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Trieme -- Move to Optics, Increase Cost and Strength
	
	UPDATE Units
	SET PrereqTech = 'TECH_OPTICS'
	WHERE Type = 'UNIT_TRIREME' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Combat = '14'
	WHERE Type = 'UNIT_TRIREME' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Classical Era Naval Unit used to wrest control of the seas.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_TRIREME' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Quinquereme -- Keep at Sailing to give Carthage a buff

	UPDATE Units
	SET Combat = '16'
	WHERE Type = 'UNIT_CARTHAGINIAN_QUINQUEREME' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Strong Ancient Era Naval Unit, available earlier than the Trieme, that is used to dominate the seas through melee attacks on naval units and cities. Receives the [COLOR_POSITIVE_TEXT]Coastal Raider I[ENDCOLOR] Promotion for free. Only the Carthaginians may build it.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_CARTHAGINIAN_QUINQUEREME' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	-- Gatling Gun -- Moved to Rifling
	UPDATE Units
	SET PrereqTech = 'TECH_RIFLING'
	WHERE Type = 'UNIT_GATLINGGUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Gatling Gun, Machine Gun, Bazooka - make Range 2 (instead of 1) to help AI

	UPDATE Units
	SET Range = '2'
	WHERE Type = 'UNIT_GATLINGGUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Ranged infantry Unit of the mid-game that weakens nearby enemy units.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_GATLINGGUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Gatling Gun is a mid-game non-siege ranged unit that can lay down a terrifying hail of bullets. It is much more powerful than earlier ranged units like the Crossbowman, but it is much weaker than other military units of its era. As such, it should be used as a source of attrition. It weakens nearby enemy units, and gains bonus strength when defending. When attacking, the Gatling Gun deals less damage to Armored or fortified Units, as well as cities. Put Gatling Guns in your cities or on chokepoints for optimal defensive power.'
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

	-- Cargo Ship -- Move to Optics

	UPDATE Units
	SET PrereqTech = 'TECH_OPTICS'
	WHERE Type = 'UNIT_CARGO_SHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Make Landship cost iron, not oil:
	UPDATE Unit_ResourceQuantityRequirements
	SET ResourceType = 'RESOURCE_IRON'
	WHERE UnitType = 'UNIT_WWI_TANK' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Submarines and Missile Cruiser should cost Iron
	INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType, Cost)
	SELECT 'UNIT_MISSILE_CRUISER', 'RESOURCE_IRON', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType, Cost)
	SELECT 'UNIT_SUBMARINE', 'RESOURCE_IRON', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType, Cost)
	SELECT 'UNIT_NUCLEAR_SUBMARINE', 'RESOURCE_IRON', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

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
	SET ObsoleteTech = 'TECH_METALLURGY'
	WHERE Type = 'UNIT_TREBUCHET' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_DYNAMITE'
	WHERE Type = 'UNIT_KOREAN_HWACHA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_METALLURGY'
	WHERE Type = 'UNIT_CANNON' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Allows you to build the Leaning Tower of Pisa, and is an gateway to vital Renaissance-Era techs.'
	WHERE Tag = 'TXT_KEY_TECH_CHEMISTRY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Cannon[ENDCOLOR], a powerful Renaissance-Era siege unit, and the [COLOR_POSITIVE_TEXT]Arsenal[ENDCOLOR], which boosts City defense.'
	WHERE Tag = 'TXT_KEY_TECH_METALLURGY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- They should also be a little stronger ranged, to compensate

	UPDATE Units
	SET RangedCombat = '10'
	WHERE Type = 'UNIT_CATAPULT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET RangedCombat = '20'
	WHERE Type = 'UNIT_TREBUCHET' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET RangedCombat = '30'
	WHERE Type = 'UNIT_CANNON' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET RangedCombat = '40'
	WHERE Type = 'UNIT_ARTILLERY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Moved SS Parts to last 4 techs - makes Science Victory as difficult as other victories.

	UPDATE Units
	SET PrereqTech = 'TECH_NUCLEAR_FUSION'
	WHERE Type = 'UNIT_SS_BOOSTER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_GLOBALIZATION'
	WHERE Type = 'UNIT_SS_COCKPIT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Move Carrier
	UPDATE Units
	SET PrereqTech = 'TECH_COMPUTERS'
	WHERE Type = 'UNIT_CARRIER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Reduce Flavor
	UPDATE Unit_Flavors
	SET Flavor = '20'
	WHERE UnitType = 'UNIT_CARRIER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Move Helicopter
	UPDATE Units
	SET PrereqTech = 'TECH_MOBILE_TACTICS'
	WHERE Type = 'UNIT_HELICOPTER_GUNSHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Move Guided Missile
	UPDATE Units
	SET PrereqTech = 'TECH_ROCKETRY'
	WHERE Type = 'UNIT_GUIDED_MISSILE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Guided Missile range bump
	UPDATE Units
	SET Range = '12'
	WHERE Type = 'UNIT_GUIDED_MISSILE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Maintenance
	UPDATE Units
	SET NoMaintenance = 'false'
	WHERE Type = 'UNIT_GUIDED_MISSILE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Guided Missile is a one-shot unit which is destroyed when it attacks an enemy target. The Guided Missile may be based in a player-owned friendly city or aboard a nuclear submarine or missile cruiser. They may move from base to base or attack an enemy unit or city with their range of "12". See the rules on Missiles for more information.'
	WHERE Tag = 'TXT_KEY_UNIT_GUIDED_MISSILE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Fighters should obsolete with Jets, and Bombers with Stealth (helps AI make good late-game choices)

	UPDATE Units
	SET ObsoleteTech = 'TECH_LASERS'
	WHERE Type = 'UNIT_FIGHTER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_STEALTH'
	WHERE Type = 'UNIT_BOMBER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Tanks Obsolete at Modern Armor

	UPDATE Units
	SET ObsoleteTech = 'TECH_LASERS'
	WHERE Type = 'UNIT_TANK' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Atomic Bomb Obsolete at Advanced Ballistics

	UPDATE Units
	SET ObsoleteTech = 'TECH_ADVANCED_BALLISTICS'
	WHERE Type = 'UNIT_ATOMIC_BOMB' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Paratrooper Obsoletes at Nanotech
	UPDATE Units
	SET PrereqTech = 'TECH_COMBINED_ARMS'
	WHERE Type = 'UNIT_PARATROOPER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_NANOTECHNOLOGY'
	WHERE Type = 'UNIT_PARATROOPER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Scout Obsolete at Optics

	UPDATE Units
	SET ObsoleteTech = 'TECH_OPTICS'
	WHERE Type = 'UNIT_SCOUT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Warrior and Brute Upgrade into Spearman

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_SPEARMAN'
	WHERE UnitType = 'UNIT_WARRIOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_SPEARMAN'
	WHERE UnitType = 'UNIT_BARBARIAN_WARRIOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_SPEARMAN'
	WHERE UnitType = 'UNIT_AZTEC_JAGUAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_BRONZE_WORKING'
	WHERE Type = 'UNIT_WARRIOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Buff Barbarians

	UPDATE Units
	SET Combat = '9'
	WHERE Type = 'UNIT_BARBARIAN_WARRIOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Combat = '9'
	WHERE Type = 'UNIT_GALLEY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_SAILING'
	WHERE Type = 'UNIT_GALLEY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET RangedCombat = '8'
	WHERE Type = 'UNIT_BARBARIAN_ARCHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Combat = '5'
	WHERE Type = 'UNIT_BARBARIAN_ARCHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_AGRICULTURE'
	WHERE Type = 'UNIT_BARBARIAN_ARCHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET RangedCombat = '12'
	WHERE Type = 'UNIT_BARBARIAN_AXMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Combat = '12'
	WHERE Type = 'UNIT_BARBARIAN_SPEARMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Combat = '15'
	WHERE Type = 'UNIT_BARBARIAN_SWORDSMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Increase strength of horsemen, knights, lancers, and cavalry, and all UUs

	UPDATE Units
	SET Combat = '15'
	WHERE Type = 'UNIT_HORSEMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Combat = '15'
	WHERE Type = 'UNIT_BARBARIAN_HORSEMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Combat = '24'
	WHERE Type = 'UNIT_KNIGHT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Combat = '20'
	WHERE Type = 'UNIT_BYZANTINE_CATAPHRACT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Combat = '20'
	WHERE Type = 'UNIT_ARABIAN_CAMELARCHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );


	UPDATE Units
	SET Combat = '25'
	WHERE Type = 'UNIT_SPANISH_CONQUISTADOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	
	UPDATE Units
	SET Combat = '25'
	WHERE Type = 'UNIT_SONGHAI_MUSLIMCAVALRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	
	UPDATE Units
	SET Combat = '28'
	WHERE Type = 'UNIT_SIAMESE_WARELEPHANT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );


	UPDATE Units
	SET Combat = '32'
	WHERE Type = 'UNIT_POLISH_WINGED_HUSSAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Combat = '40'
	WHERE Type = 'UNIT_CAVALRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Combat = '42'
	WHERE Type = 'UNIT_RUSSIAN_COSSACK' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Combat = '40'
	WHERE Type = 'UNIT_BERBER_CAVALRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Combat = '42'
	WHERE Type = 'UNIT_AUSTRIAN_HUSSAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	