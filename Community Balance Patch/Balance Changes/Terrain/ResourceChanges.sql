-- Move Coal to Steampower

	UPDATE Resources
	SET TechReveal = 'TECH_STEAM_POWER'
	WHERE Type = 'RESOURCE_COAL';

	UPDATE Resources
	SET TechCityTrade = 'TECH_STEAM_POWER'
	WHERE Type = 'RESOURCE_COAL';

-- Tech Progression for Bonus Resources (a la Civ 4)
	UPDATE Resources
	SET TechReveal = 'TECH_TRAPPING'
	WHERE Type = 'RESOURCE_DEER';

	UPDATE Resources
	SET TechReveal = 'TECH_TRAPPING'
	WHERE Type = 'RESOURCE_BISON';

	UPDATE Resources
	SET TechReveal = 'TECH_ANIMAL_HUSBANDRY'
	WHERE Type = 'RESOURCE_COW';
	
	UPDATE Resources
	SET TechReveal = 'TECH_ANIMAL_HUSBANDRY'
	WHERE Type = 'RESOURCE_SHEEP';

	UPDATE Resources
	SET TechReveal = 'TECH_SAILING'
	WHERE Type = 'RESOURCE_FISH';

	UPDATE Resources
	SET TechReveal = 'TECH_MINING'
	WHERE Type = 'RESOURCE_STONE';

	UPDATE Resources
	SET TechReveal = 'TECH_CALENDAR'
	WHERE Type = 'RESOURCE_BANANA';

	UPDATE Resources
	SET TechReveal = 'TECH_AGRICULTURE'
	WHERE Type = 'RESOURCE_WHEAT';

	-- Stone good for Ancient and Classical wonders
	UPDATE Resources
	SET WonderProductionMod = '10'
	WHERE Type = 'RESOURCE_STONE';

	UPDATE Resources
	SET WonderProductionModObsoleteEra = 'ERA_MEDIEVAL'
	WHERE Type = 'RESOURCE_STONE';

	-- Marble good for Classical thru Renaissance wonders
	UPDATE Resources
	SET WonderProductionMod = '15'
	WHERE Type = 'RESOURCE_MARBLE';

	UPDATE Resources
	SET WonderProductionModObsoleteEra = 'ERA_INDUSTRIAL'
	WHERE Type = 'RESOURCE_MARBLE';

	-- Resource Monopoly Changes
	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_ALUMINUM';

	UPDATE Resources
	SET MonopolyHealBonus = '5'
	WHERE Type = 'RESOURCE_ALUMINUM';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_CITRUS';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_CLOVES';

	UPDATE Resources
	SET MonopolyHappiness = '3'
	WHERE Type = 'RESOURCE_CLOVES';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_COAL';

	UPDATE Resources
	SET MonopolyMovementBonus = '1'
	WHERE Type = 'RESOURCE_COAL';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_COCOA';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_COPPER';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_COTTON';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_CRAB';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_DYE';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_FUR';

	UPDATE Resources
	SET MonopolyHappiness = '3'
	WHERE Type = 'RESOURCE_FUR';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_GEMS';

	UPDATE Resources
	SET MonopolyGALength = '10'
	WHERE Type = 'RESOURCE_GEMS';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_GOLD';

	UPDATE Resources
	SET MonopolyGALength = '10'
	WHERE Type = 'RESOURCE_GOLD';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_HORSE';

	UPDATE Resources
	SET MonopolyAttackBonus = '10'
	WHERE Type = 'RESOURCE_HORSE';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_INCENSE';

	UPDATE Resources
	SET MonopolyHappiness = '3'
	WHERE Type = 'RESOURCE_INCENSE';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_IRON';

	UPDATE Resources
	SET MonopolyDefenseBonus = '10'
	WHERE Type = 'RESOURCE_IRON';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_IVORY';

	UPDATE Resources
	SET MonopolyGALength = '10'
	WHERE Type = 'RESOURCE_IVORY';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_MARBLE';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_NUTMEG';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_OIL';

	UPDATE Resources
	SET MonopolyXPBonus = '5'
	WHERE Type = 'RESOURCE_OIL';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_PEARLS';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_PEPPER';

	UPDATE Resources
	SET MonopolyHappiness = '3'
	WHERE Type = 'RESOURCE_PEPPER';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_SALT';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_SILK';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_SILVER';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_SPICES';

	UPDATE Resources
	SET MonopolyGALength = '10'
	WHERE Type = 'RESOURCE_SPICES';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_SUGAR';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_TRUFFLES';

	UPDATE Resources
	SET MonopolyHappiness = '3'
	WHERE Type = 'RESOURCE_TRUFFLES';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_URANIUM';

	UPDATE Resources
	SET MonopolyAttackBonus = '10'
	WHERE Type = 'RESOURCE_URANIUM';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_WHALE';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_WINE';

	UPDATE Resources
	SET MonopolyHappiness = '3'
	WHERE Type = 'RESOURCE_WINE';

	-- Text files for changes.

	-- Other Text
	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_HAPPINESS'
	WHERE MonopolyHappiness = 3;

	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_GA_LENGTH'
	WHERE MonopolyGALength = 10;

	-- Yield Text
	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_YIELD_PRODUCTION_ALUMINUM'
	WHERE Type = 'RESOURCE_ALUMINUM';

	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_YIELD_PRODUCTION'
	WHERE Type = 'RESOURCE_NUTMEG';

	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_YIELD_GOLD_COAL'
	WHERE Type = 'RESOURCE_COAL';

	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_YIELD_GOLD'
	WHERE Type = 'RESOURCE_COTTON';

	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_YIELD_FOOD'
	WHERE Type = 'RESOURCE_CRAB';

	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_YIELD_FOOD'
	WHERE Type = 'RESOURCE_CITRUS';

	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_YIELD_CULTURE'
	WHERE Type = 'RESOURCE_SILVER';

	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_YIELD_CULTURE'
	WHERE Type = 'RESOURCE_PEARLS';

	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_YIELD_SCIENCE_OIL'
	WHERE Type = 'RESOURCE_OIL';

	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_YIELD_SCIENCE_HORSE'
	WHERE Type = 'RESOURCE_HORSE';


-- Mod Text
	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_MOD_PRODUCTION'
	WHERE Type = 'RESOURCE_COPPER';

	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_MOD_PRODUCTION_IRON'
	WHERE Type = 'RESOURCE_IRON';

	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_MOD_GOLD'
	WHERE Type = 'RESOURCE_DYE';

	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_MOD_GOLD'
	WHERE Type = 'RESOURCE_SILK';

	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_MOD_FOOD'
	WHERE Type = 'RESOURCE_SALT';

	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_MOD_FOOD'
	WHERE Type = 'RESOURCE_SUGAR';

	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_MOD_CULTURE'
	WHERE Type = 'RESOURCE_MARBLE';

	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_MOD_CULTURE'
	WHERE Type = 'RESOURCE_COCOA';

	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_MOD_SCIENCE_URANIUM'
	WHERE Type = 'RESOURCE_URANIUM';

	UPDATE Resources
	SET Help = 'TXT_KEY_RESOURCE_MONOPOLY_MOD_SCIENCE'
	WHERE Type = 'RESOURCE_WHALE';

	-- Updates to HappinessInfo.lua file
	UPDATE Language_en_US
	SET Text = 'Resource Monopolies'
	WHERE Tag = 'TXT_KEY_EO_LOCAL_RESOURCES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );