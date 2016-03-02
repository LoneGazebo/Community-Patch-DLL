-- NAVAL UNITS
-- Ranged Units -- Reduced Combat Strength to make Melee units more important

-- NAVAL
	-- Missile Cruiser (80)

	UPDATE Units
	SET Moves = '6'
	WHERE Type = 'UNIT_MISSILE_CRUISER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Nuclear Submarine (50)

	-- Battleship (55)

	-- Submarine (35)

	-- Frigate (25)

	-- Ship of the Line (30)

	-- Galleass (16)

	UPDATE Units
	SET Moves = '3'
	WHERE Type = 'UNIT_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM Unit_AITypes
	WHERE UnitType = 'UNIT_GALLEASS' AND UnitAIType = 'UNITAI_EXPLORE_SEA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Dromon (new Classical Unit)
	INSERT INTO UnitClasses (Type, Description, DefaultUnit)
	SELECT 'UNITCLASS_BYZANTINE_DROMON', 'TXT_KEY_UNIT_BYZANTINE_DROMON', 'UNIT_BYZANTINE_DROMON'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

	DELETE FROM Unit_AITypes
	WHERE UnitType = 'UNIT_BYZANTINE_DROMON' AND UnitAIType = 'UNITAI_EXPLORE_SEA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Dominant Classical Era Naval Unit used to own the seas well into the Medieval Era with its ranged attack.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_BYZANTINE_DROMON' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Attacks with lethal Greek Fire, making it the first naval unit with a ranged attack. It cannot enter Deep Ocean tiles outside of the city borders.'
	WHERE Tag = 'TXT_KEY_UNIT_BYZANTINE_DROMON_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

	
	UPDATE Units
	SET Moves = '3'
	WHERE Type = 'UNIT_BYZANTINE_DROMON' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_OPTICS'
	WHERE Type = 'UNIT_BYZANTINE_DROMON' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_GUILDS'
	WHERE Type = 'UNIT_BYZANTINE_DROMON' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Class = 'UNITCLASS_BYZANTINE_DROMON'
	WHERE Type = 'UNIT_BYZANTINE_DROMON' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM Unit_FreePromotions
	WHERE UnitType = 'UNIT_BYZANTINE_DROMON' AND PromotionType = 'PROMOTION_NAVAL_BONUS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

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

	-- Flavors -- Naval
	UPDATE Unit_Flavors
	SET Flavor = '8'
	WHERE UnitType = 'UNIT_TRIREME' AND FlavorType = 'FLAVOR_NAVAL';

	UPDATE Unit_Flavors
	SET Flavor = '15'
	WHERE UnitType = 'UNIT_CARTHAGINIAN_QUINQUEREME' AND FlavorType = 'FLAVOR_NAVAL';

	UPDATE Unit_Flavors
	SET Flavor = '15'
	WHERE UnitType = 'UNIT_CARAVEL' AND FlavorType = 'FLAVOR_NAVAL';

	UPDATE Unit_Flavors
	SET Flavor = '20'
	WHERE UnitType = 'UNIT_PORTUGUESE_NAU' AND FlavorType = 'FLAVOR_NAVAL';

	UPDATE Unit_Flavors
	SET Flavor = '20'
	WHERE UnitType = 'UNIT_KOREAN_TURTLE_SHIP' AND FlavorType = 'FLAVOR_NAVAL';

	UPDATE Unit_Flavors
	SET Flavor = '20'
	WHERE UnitType = 'UNIT_PRIVATEER' AND FlavorType = 'FLAVOR_NAVAL';

	UPDATE Unit_Flavors
	SET Flavor = '25'
	WHERE UnitType = 'UNIT_DUTCH_SEA_BEGGAR' AND FlavorType = 'FLAVOR_NAVAL';

	UPDATE Unit_Flavors
	SET Flavor = '25'
	WHERE UnitType = 'UNIT_IRONCLAD' AND FlavorType = 'FLAVOR_NAVAL';

	UPDATE Unit_Flavors
	SET Flavor = '40'
	WHERE UnitType = 'UNIT_DESTROYER' AND FlavorType = 'FLAVOR_NAVAL';

	UPDATE Unit_Flavors
	SET Flavor = '8'
	WHERE UnitType = 'UNIT_BYZANTINE_DROMON' AND FlavorType = 'FLAVOR_NAVAL';

	UPDATE Unit_Flavors
	SET Flavor = '15'
	WHERE UnitType = 'UNIT_GALLEASS' AND FlavorType = 'FLAVOR_NAVAL';

	UPDATE Unit_Flavors
	SET Flavor = '30'
	WHERE UnitType = 'UNIT_FRIGATE' AND FlavorType = 'FLAVOR_NAVAL';

	UPDATE Unit_Flavors
	SET Flavor = '40'
	WHERE UnitType = 'UNIT_ENGLISH_SHIPOFTHELINE' AND FlavorType = 'FLAVOR_NAVAL';

	UPDATE Unit_Flavors
	SET Flavor = '40'
	WHERE UnitType = 'UNIT_CRUISER' AND FlavorType = 'FLAVOR_NAVAL';

	UPDATE Unit_Flavors
	SET Flavor = '50'
	WHERE UnitType = 'UNIT_BATTLESHIP' AND FlavorType = 'FLAVOR_NAVAL';

	UPDATE Unit_Flavors
	SET Flavor = '50'
	WHERE UnitType = 'UNIT_MISSILE_CRUISER' AND FlavorType = 'FLAVOR_NAVAL';

	-- Flavors -- Naval Recon

	UPDATE Unit_Flavors
	SET Flavor = '10'
	WHERE UnitType = 'UNIT_TRIREME' AND FlavorType = 'FLAVOR_NAVAL_RECON';

	UPDATE Unit_Flavors
	SET Flavor = '20'
	WHERE UnitType = 'UNIT_CARTHAGINIAN_QUINQUEREME' AND FlavorType = 'FLAVOR_NAVAL_RECON';

	UPDATE Unit_Flavors
	SET Flavor = '25'
	WHERE UnitType = 'UNIT_CARAVEL' AND FlavorType = 'FLAVOR_NAVAL_RECON';

	UPDATE Unit_Flavors
	SET Flavor = '30'
	WHERE UnitType = 'UNIT_PORTUGUESE_NAU' AND FlavorType = 'FLAVOR_NAVAL_RECON';

	INSERT INTO Unit_Flavors (Flavor, UnitType, FlavorType)
	SELECT '15' , 'UNIT_KOREAN_TURTLE_SHIP', 'FLAVOR_NAVAL_RECON';

	UPDATE Unit_Flavors
	SET Flavor = '25'
	WHERE UnitType = 'UNIT_PRIVATEER' AND FlavorType = 'FLAVOR_NAVAL_RECON';

	UPDATE Unit_Flavors
	SET Flavor = '30'
	WHERE UnitType = 'UNIT_DUTCH_SEA_BEGGAR' AND FlavorType = 'FLAVOR_NAVAL_RECON';

	INSERT INTO Unit_Flavors (Flavor, UnitType, FlavorType)
	SELECT '30' , 'UNIT_IRONCLAD', 'FLAVOR_NAVAL_RECON';

	UPDATE Unit_Flavors
	SET Flavor = '40'
	WHERE UnitType = 'UNIT_DESTROYER' AND FlavorType = 'FLAVOR_NAVAL_RECON';

	UPDATE Unit_Flavors
	SET Flavor = '8'
	WHERE UnitType = 'UNIT_BYZANTINE_DROMON' AND FlavorType = 'FLAVOR_NAVAL_RECON';

	UPDATE Unit_Flavors
	SET Flavor = '12'
	WHERE UnitType = 'UNIT_GALLEASS' AND FlavorType = 'FLAVOR_NAVAL_RECON';

	UPDATE Unit_Flavors
	SET Flavor = '15'
	WHERE UnitType = 'UNIT_FRIGATE' AND FlavorType = 'FLAVOR_NAVAL_RECON';

	UPDATE Unit_Flavors
	SET Flavor = '25'
	WHERE UnitType = 'UNIT_ENGLISH_SHIPOFTHELINE' AND FlavorType = 'FLAVOR_NAVAL_RECON';

	UPDATE Unit_Flavors
	SET Flavor = '30'
	WHERE UnitType = 'UNIT_CRUISER' AND FlavorType = 'FLAVOR_NAVAL_RECON';

	INSERT INTO Unit_Flavors (Flavor, UnitType, FlavorType)
	SELECT '35' , 'UNIT_BATTLESHIP', 'FLAVOR_NAVAL_RECON';

	UPDATE Unit_Flavors
	SET Flavor = '40'
	WHERE UnitType = 'UNIT_MISSILE_CRUISER' AND FlavorType = 'FLAVOR_NAVAL_RECON';

	-- Move Caravel to Compass
	UPDATE Units
	SET Moves = '4'
	WHERE Type = 'UNIT_CARAVEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	UPDATE Units
	SET PrereqTech = 'TECH_COMPASS'
	WHERE Type = 'UNIT_CARAVEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET DefaultUnitAI = 'UNITAI_ATTACK_SEA'
	WHERE Type = 'UNIT_CARAVEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_PRIVATEER'
	WHERE UnitType = 'UNIT_CARAVEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'A corvette is a small warship. It is traditionally the smallest class of vessel considered to be a proper (or "rated") warship. The warship class above is that of frigate. The class below is historically sloop-of-war. The modern types of ship below a corvette are coastal patrol craft and fast attack craft. In modern terms, a corvette is typically between 500 tons and 2,000 tons although recent designs may approach 3,000 tons, which might instead be considered a small frigate.'
	WHERE Tag = 'TXT_KEY_CIV5_PRIVATEER_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );
	
	UPDATE Language_en_US
	SET Text = 'The Caravel is a significant upgrade to your naval power. A Melee unit, it is stronger and faster than the aging Trireme, and it can enter Deep Ocean hexes. It is, however, slow, making it vulnerable to enemy ranged units. Use it to (gradually) explore the world, or to defend your home cities.'
	WHERE Tag = 'TXT_KEY_UNIT_CARAVEL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );
	
	UPDATE Language_en_US
	SET Text = 'Late-Medieval exploration Unit that can enter the Ocean, but moves slowly. Fights as a naval melee unit.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_CARAVEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );
	
	UPDATE Units
	SET ObsoleteTech = 'TECH_NAVIGATION'
	WHERE Type = 'UNIT_CARAVEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM Unit_FreePromotions
	WHERE UnitType = 'UNIT_CARAVEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );


	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_PRIVATEER'
	WHERE UnitType = 'UNIT_PORTUGUESE_NAU' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET DefaultUnitAI = 'UNITAI_ATTACK_SEA'
	WHERE Type = 'UNIT_PORTUGUESE_NAU' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_COMPASS'
	WHERE Type = 'UNIT_PORTUGUESE_NAU' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_INDUSTRIALIZATION'
	WHERE Type = 'UNIT_PORTUGUESE_NAU' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Moves = '4'
	WHERE Type = 'UNIT_PORTUGUESE_NAU' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Move Galleass to Optics
	UPDATE Units
	SET PrereqTech = 'TECH_GUILDS'
	WHERE Type = 'UNIT_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_NAVIGATION'
	WHERE Type = 'UNIT_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Move Privateer to Astronomy, make Caravel upgrade into it, rename Corvette
	UPDATE Units
	SET PrereqTech = 'TECH_NAVIGATION'
	WHERE Type = 'UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET DefaultUnitAI = 'UNITAI_ATTACK_SEA'
	WHERE Type = 'UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET DefaultUnitAI = 'UNITAI_ATTACK_SEA'
	WHERE Type = 'UNIT_DUTCH_SEA_BEGGAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM Unit_AITypes
	WHERE UnitType = 'UNIT_PRIVATEER' AND UnitAIType = 'UNITAI_PIRATE_SEA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM Unit_AITypes
	WHERE UnitType = 'UNIT_DUTCH_SEA_BEGGAR' AND UnitAIType = 'UNITAI_PIRATE_SEA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_ASTRONOMY'
	WHERE Type = 'UNIT_DUTCH_SEA_BEGGAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Naval Unit that specializes in attacking coastal cities to earn gold and capturing enemy ships. Available earlier than the Corvette, which it replaces. Only the Dutch can build it.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_SEA_BEGGAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Corvette'
	WHERE Tag = 'TXT_KEY_UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Naval Unit that specializes in melee combat and quick movement.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Naval Unit that attacks as a melee unit. It is significantly stronger and faster than the Caravel.'
	WHERE Tag = 'TXT_KEY_UNIT_PRIVATEER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_ROCKETRY'
	WHERE Type = 'UNIT_DUTCH_SEA_BEGGAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_INDUSTRIALIZATION'
	WHERE Type = 'UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET UnitFlagAtlas = 'CORVETTE_FLAG_ATLAS'
	WHERE Type = 'UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET UnitFlagIconOffset = '0'
	WHERE Type = 'UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET IconAtlas = 'COMMUNITY_ATLAS'
	WHERE Type = 'UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET UnitArtInfo = 'ART_DEF_UNIT_CORVETTE'
	WHERE Type = 'UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PortraitIndex = '60'
	WHERE Type = 'UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_IRONCLAD'
	WHERE UnitType = 'UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Unit_ClassUpgrades
	SET UnitClassType = 'UNITCLASS_IRONCLAD'
	WHERE UnitType = 'UNIT_DUTCH_SEA_BEGGAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM Unit_FreePromotions
	WHERE UnitType = 'UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Ironclad
	UPDATE Units
	SET Moves = '5'
	WHERE Type = 'UNIT_IRONCLAD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_INDUSTRIALIZATION'
	WHERE Type = 'UNIT_IRONCLAD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM Unit_ResourceQuantityRequirements
	WHERE UnitType = 'UNIT_IRONCLAD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET ObsoleteTech = 'TECH_ROCKETRY'
	WHERE Type = 'UNIT_IRONCLAD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM Unit_FreePromotions
	WHERE UnitType = 'UNIT_IRONCLAD' AND PromotionType = 'PROMOTION_STEAM_POWERED' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'A very powerful naval melee unit, the Ironclad dominates the oceans of the Industrial age.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_IRONCLAD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Destroyer
	UPDATE Units
	SET PrereqTech = 'TECH_ROCKETRY'
	WHERE Type = 'UNIT_DESTROYER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Submarines -- Can Now Attack Coastal Cities -- Reduced Ranged Damage, but boosted it versus boats. This will help AI a TON
	-- Moved to Atomic Era (Submarine)

	UPDATE Units
	SET PrereqTech = 'TECH_PENICILIN'
	WHERE Type = 'UNIT_SUBMARINE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM Unit_AITypes
	WHERE UnitType = 'UNIT_SUBMARINE' AND UnitAIType = 'UNITAI_EXPLORE_SEA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	DELETE FROM Unit_AITypes
	WHERE UnitType = 'UNIT_NUCLEAR_SUBMARINE' AND UnitAIType = 'UNITAI_EXPLORE_SEA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Battleship -- Upgrades to Missile Cruiser
	UPDATE Unit_ResourceQuantityRequirements
	SET ResourceType = 'RESOURCE_IRON'
	WHERE UnitType = 'UNIT_BATTLESHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

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
	SET ObsoleteTech = 'TECH_COMPASS'
	WHERE Type = 'UNIT_TRIREME' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET Moves = '4'
	WHERE Type = 'UNIT_TRIREME' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Classical Era Naval Unit used to wrest control of the seas.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_TRIREME' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Quinquereme -- Keep at Sailing to give Carthage a buff

	UPDATE Units
	SET Moves = '4'
	WHERE Type = 'UNIT_CARTHAGINIAN_QUINQUEREME' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Strong Ancient Era Naval Unit, available earlier than the Trireme, that is used to dominate the seas through melee attacks on naval units and cities. Receives the [COLOR_POSITIVE_TEXT]Reconnaissance[ENDCOLOR] Promotion for free, allowing it to gain experience from exploration. Only the Carthaginians may build it.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_CARTHAGINIAN_QUINQUEREME' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
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

	-- Carrier should cost Iron
	INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType, Cost)
	SELECT 'UNIT_CARRIER', 'RESOURCE_OIL', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Move Carrier
	INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_CARRIER' , 'PROMOTION_INTERCEPTION_IV'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET PrereqTech = 'TECH_COMPUTERS'
	WHERE Type = 'UNIT_CARRIER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Units
	SET AirInterceptRange = '4'
	WHERE Type = 'UNIT_CARRIER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Initially carries 2 Aircraft; capacity may be boosted through promotions. Will intercept enemy aircraft which try to attack nearby Units.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_CARRIER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The Carrier is a specialized vessel which carries fighter airplanes, bombers (but not stealth bombers), and atomic bombs. The Carrier itself is unable to attack, but the aircraft it carries make it the most powerful offensive weapon afloat. Defensively, however, the Carrier is weak and should be escorted by destroyers and submarines. Carriers are, however, armed with anti-air weaporny, and will automatically attack any air unit bombing a target within 4 tiles. (They can only intercept one unit per turn.)'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_CARRIER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Reduce Flavor
	UPDATE Unit_Flavors
	SET Flavor = '15'
	WHERE UnitType = 'UNIT_CARRIER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );