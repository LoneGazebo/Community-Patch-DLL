-- Move Coal to Steampower

	UPDATE Resources
	SET TechReveal = 'TECH_STEAM_POWER'
	WHERE Type = 'RESOURCE_COAL';

	UPDATE Resources
	SET TechCityTrade = 'TECH_STEAM_POWER'
	WHERE Type = 'RESOURCE_COAL';

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
	SET MonopolyGALength = '25'
	WHERE Type = 'RESOURCE_GEMS';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_GOLD';

	UPDATE Resources
	SET MonopolyGALength = '25'
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
	SET MonopolyGALength = '25'
	WHERE Type = 'RESOURCE_IVORY';

	UPDATE Resources
	SET IsMonopoly = 'true'
	WHERE Type = 'RESOURCE_JEWELRY';

	UPDATE Resources
	SET MonopolyHappiness = '3'
	WHERE Type = 'RESOURCE_JEWELRY';

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
	WHERE Type = 'RESOURCE_PORCELAIN';

	UPDATE Resources
	SET MonopolyGALength = '25'
	WHERE Type = 'RESOURCE_PORCELAIN';

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
	SET MonopolyGALength = '25'
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
	WHERE MonopolyGALength = 25;

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


	-- Strategic Values

	UPDATE Resources
	Set StrategicHelp = 'TXT_KEY_RESOURCE_MONOPOLY_STRAT_HORSE'
	WHERE Type = 'RESOURCE_HORSE';

	UPDATE Resources
	Set StrategicHelp = 'TXT_KEY_RESOURCE_MONOPOLY_STRAT_IRON'
	WHERE Type = 'RESOURCE_IRON';
	
	UPDATE Resources
	Set StrategicHelp = 'TXT_KEY_RESOURCE_MONOPOLY_STRAT_COAL'
	WHERE Type = 'RESOURCE_COAL';

	UPDATE Resources
	Set StrategicHelp = 'TXT_KEY_RESOURCE_MONOPOLY_STRAT_ALUMINUM'
	WHERE Type = 'RESOURCE_ALUMINUM';

	UPDATE Resources
	Set StrategicHelp = 'TXT_KEY_RESOURCE_MONOPOLY_STRAT_OIL'
	WHERE Type = 'RESOURCE_OIL';

	UPDATE Resources
	Set StrategicHelp = 'TXT_KEY_RESOURCE_MONOPOLY_STRAT_URANIUM'
	WHERE Type = 'RESOURCE_URANIUM';