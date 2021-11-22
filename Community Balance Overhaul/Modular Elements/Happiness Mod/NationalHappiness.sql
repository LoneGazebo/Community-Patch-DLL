-- Happiness Flavors
UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = '-10'
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_NEED_HAPPINESS_CRITICAL' AND FlavorType = 'FLAVOR_GROWTH';