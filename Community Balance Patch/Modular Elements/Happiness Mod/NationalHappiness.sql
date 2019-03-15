
--EMPIRE BONUSES BELOW

-- These values modify empire-wide bonuses or penalties gained (or lost) from happiness. Change the values below, making sure to keep the integers + or - as they are below.

-- Maximum happiness bonus above threshold mod. (should always be a positive value)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_BONUS_MAXIMUM', '0';

-- Maximum happiness penalty % mod. (Should always be a negative value)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_PENALTY_MAXIMUM', '-75';

	INSERT INTO Defines (
	Name, Value)
	SELECT 'UNHAPPY_PRODUCTION_PENALTY', '-25';

	INSERT INTO Defines (
	Name, Value)
	SELECT 'VERY_UNHAPPY_PRODUCTION_PENALTY', '-50';

-- Happiness Flavors
	UPDATE AIEconomicStrategy_City_Flavors
	SET Flavor = '-10'
	WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_NEED_HAPPINESS_CRITICAL' AND FlavorType = 'FLAVOR_GROWTH';