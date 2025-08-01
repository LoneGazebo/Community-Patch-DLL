-- Opener (now Industry)
DELETE FROM Policy_CapitalYieldModifiers
WHERE PolicyType = 'POLICY_COMMERCE';

UPDATE Policies
SET FreeTradeRoute = 2
WHERE Type = 'POLICY_COMMERCE';

INSERT INTO Policy_HurryModifiers
	(PolicyType, HurryType, HurryCostModifier)
VALUES
	('POLICY_COMMERCE', 'HURRY_GOLD', -5);

-- Wagon Trains (now Free Trade)
UPDATE Policies
SET
	RouteGoldMaintenanceMod = 0,
	GreatMerchantRateModifier = 50,
	LandTradeRouteGoldChange = 500,
	SeaTradeRouteGoldChange = 500,
	PovertyFlatReduction = 2
WHERE Type = 'POLICY_CARAVANS';

-- Mercenary Army (now Division of Labor)
UPDATE Policies
SET
	PortraitIndex = 6,
	IconAtlas = 'POLICY_ATLAS_EXP2',
	IconAtlasAchieved = 'POLICY_A_ATLAS_EXP2'
WHERE Type = 'POLICY_TRADE_UNIONS';

INSERT INTO Policy_BuildingClassYieldModifiers
	(PolicyType, BuildingClassType, YieldType, YieldMod)
VALUES
	('POLICY_TRADE_UNIONS', 'BUILDINGCLASS_FORGE', 'YIELD_PRODUCTION', 3),
	('POLICY_TRADE_UNIONS', 'BUILDINGCLASS_FORGE', 'YIELD_GOLD', 3),
	('POLICY_TRADE_UNIONS', 'BUILDINGCLASS_WINDMILL', 'YIELD_PRODUCTION', 3),
	('POLICY_TRADE_UNIONS', 'BUILDINGCLASS_WINDMILL', 'YIELD_GOLD', 3),
	('POLICY_TRADE_UNIONS', 'BUILDINGCLASS_WORKSHOP', 'YIELD_PRODUCTION', 3),
	('POLICY_TRADE_UNIONS', 'BUILDINGCLASS_WORKSHOP', 'YIELD_GOLD', 3),
	('POLICY_TRADE_UNIONS', 'BUILDINGCLASS_FACTORY', 'YIELD_PRODUCTION', 3),
	('POLICY_TRADE_UNIONS', 'BUILDINGCLASS_FACTORY', 'YIELD_GOLD', 3),
	('POLICY_TRADE_UNIONS', 'BUILDINGCLASS_COALING_STATION', 'YIELD_PRODUCTION', 3),
	('POLICY_TRADE_UNIONS', 'BUILDINGCLASS_COALING_STATION', 'YIELD_GOLD', 3);

INSERT INTO Policy_BuildingClassProductionModifiers
	(PolicyType, BuildingClassType, ProductionModifier)
VALUES
	('POLICY_TRADE_UNIONS', 'BUILDINGCLASS_COALING_STATION', 100);

-- Entrepreneurship
UPDATE Policies
SET
	GreatMerchantRateModifier = 0,
	EmbarkedExtraMoves = 0,
	TradeMissionGoldModifier = 25,
	GreatEngineerHurryModifier = 25
WHERE Type = 'POLICY_ENTREPRENEURSHIP';

INSERT INTO Policy_BuildingClassHappiness
	(PolicyType, BuildingClassType, Happiness)
VALUES
	('POLICY_ENTREPRENEURSHIP', 'BUILDINGCLASS_WORKSHOP', 1);

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_ENTREPRENEURSHIP', 'IMPROVEMENT_MINE', 'YIELD_PRODUCTION', 2),
	('POLICY_ENTREPRENEURSHIP', 'IMPROVEMENT_MINE', 'YIELD_GOLD', 1),
	('POLICY_ENTREPRENEURSHIP', 'IMPROVEMENT_QUARRY', 'YIELD_PRODUCTION', 2),
	('POLICY_ENTREPRENEURSHIP', 'IMPROVEMENT_QUARRY', 'YIELD_GOLD', 1),
	('POLICY_ENTREPRENEURSHIP', 'IMPROVEMENT_LUMBERMILL', 'YIELD_PRODUCTION', 2),
	('POLICY_ENTREPRENEURSHIP', 'IMPROVEMENT_LUMBERMILL', 'YIELD_GOLD', 1);

-- Mercantilism
DELETE FROM Policy_BuildingClassYieldChanges
WHERE PolicyType = 'POLICY_MERCANTILISM';

UPDATE Policy_HurryModifiers
SET HurryCostModifier = 0 -- will be updated in scaler
WHERE PolicyType = 'POLICY_MERCANTILISM';

INSERT INTO Policy_YieldFromConstruction
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_MERCANTILISM', 'YIELD_SCIENCE', 10);

INSERT INTO Policy_BuildingClassYieldModifiers
	(PolicyType, BuildingClassType, YieldType, YieldMod)
VALUES
	('POLICY_MERCANTILISM', 'BUILDINGCLASS_MARKET', 'YIELD_SCIENCE', 3),
	('POLICY_MERCANTILISM', 'BUILDINGCLASS_MARKET', 'YIELD_CULTURE', 3),
	('POLICY_MERCANTILISM', 'BUILDINGCLASS_CARAVANSARY', 'YIELD_SCIENCE', 3),
	('POLICY_MERCANTILISM', 'BUILDINGCLASS_CARAVANSARY', 'YIELD_CULTURE', 3),
	('POLICY_MERCANTILISM', 'BUILDINGCLASS_MINT', 'YIELD_SCIENCE', 3),
	('POLICY_MERCANTILISM', 'BUILDINGCLASS_MINT', 'YIELD_CULTURE', 3),
	('POLICY_MERCANTILISM', 'BUILDINGCLASS_BANK', 'YIELD_SCIENCE', 3),
	('POLICY_MERCANTILISM', 'BUILDINGCLASS_BANK', 'YIELD_CULTURE', 3),
	('POLICY_MERCANTILISM', 'BUILDINGCLASS_STOCK_EXCHANGE', 'YIELD_CULTURE', 3),
	('POLICY_MERCANTILISM', 'BUILDINGCLASS_STOCK_EXCHANGE', 'YIELD_SCIENCE', 3);

-- Protectionism
UPDATE Policies
SET
	FreeTradeRoute = 0,
	ExtraHappinessPerLuxury = 0,
	InternalTradeRouteYieldModifier = 33,
	InvestmentModifier = -10
WHERE Type = 'POLICY_PROTECTIONISM';

INSERT INTO Policy_WLTKDYieldMod
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_PROTECTIONISM', 'YIELD_FOOD', 10),
	('POLICY_PROTECTIONISM', 'YIELD_SCIENCE', 10);

-- Finisher
DELETE FROM Policy_ImprovementYieldChanges
WHERE PolicyType = 'POLICY_COMMERCE_FINISHER';

UPDATE Policies
SET ExtraHappinessPerLuxury = 3
WHERE Type = 'POLICY_COMMERCE_FINISHER';

INSERT INTO Policy_SpecialistExtraYields
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_COMMERCE_FINISHER', 'YIELD_PRODUCTION', 2),
	('POLICY_COMMERCE_FINISHER', 'YIELD_GOLD', 2);

-- Scaler
INSERT INTO Policy_HurryModifiers
	(PolicyType, HurryType, HurryCostModifier)
SELECT
	Type, 'HURRY_GOLD', -5
FROM Policies
WHERE PolicyBranchType = 'POLICY_BRANCH_COMMERCE';
