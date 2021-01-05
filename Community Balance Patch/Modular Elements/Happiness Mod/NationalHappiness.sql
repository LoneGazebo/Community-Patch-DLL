
--EMPIRE BONUSES BELOW

-- These values modify empire-wide bonuses or penalties gained (or lost) from happiness. Change the values below, making sure to keep the integers + or - as they are below.

-- Maximum happiness penalty % mod. (Should always be a negative value)
	INSERT INTO Defines (Name, Value)
	SELECT 'BALANCE_HAPPINESS_PENALTY_MAXIMUM', '-75';

	INSERT INTO Defines (Name, Value)
	SELECT 'UNHAPPY_PRODUCTION_PENALTY', '-25';

	INSERT INTO Defines (Name, Value)
	SELECT 'VERY_UNHAPPY_PRODUCTION_PENALTY', '-50';

	-- War Weariness Cap
	-- This is the % of empire population that war weariness is capped at.
	INSERT INTO Defines (Name, Value)
	SELECT 'BALANCE_WAR_WEARINESS_POPULATION_CAP', '25';

-- Happiness Flavors
	UPDATE AIEconomicStrategy_City_Flavors
	SET Flavor = '-10'
	WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_NEED_HAPPINESS_CRITICAL' AND FlavorType = 'FLAVOR_GROWTH';