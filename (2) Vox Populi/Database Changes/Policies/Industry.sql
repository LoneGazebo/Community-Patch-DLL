-- Tree structure
UPDATE Policies
SET GridX = 5, GridY = 1
WHERE Type = 'POLICY_CARAVANS';

UPDATE Policies
SET GridX = 3, GridY = 1
WHERE Type = 'POLICY_TRADE_UNIONS';

UPDATE Policies
SET GridX = 2, GridY = 3
WHERE Type = 'POLICY_ENTREPRENEURSHIP';

UPDATE Policies
SET GridX = 5, GridY = 2,
PortraitIndex = 11
WHERE Type = 'POLICY_MERCANTILISM';

UPDATE Policies
SET GridX = 4, GridY = 3,
PortraitIndex = 9
WHERE Type = 'POLICY_PROTECTIONISM';

DELETE FROM Policy_PrereqPolicies WHERE PolicyType IN ('POLICY_PROTECTIONISM', 'POLICY_ENTREPRENEURSHIP', 'POLICY_MERCANTILISM');

INSERT INTO Policy_PrereqPolicies
	(PolicyType, PrereqPolicy)
VALUES	
	('POLICY_MERCANTILISM', 'POLICY_CARAVANS'),	
	('POLICY_ENTREPRENEURSHIP', 'POLICY_TRADE_UNIONS'),
	('POLICY_PROTECTIONISM', 'POLICY_MERCANTILISM'),
	('POLICY_PROTECTIONISM', 'POLICY_TRADE_UNIONS');

-- Opener (now Industry)
DELETE FROM Policy_CapitalYieldModifiers
WHERE PolicyType = 'POLICY_COMMERCE';

UPDATE Policies
SET 
	FreeTradeRoute = 0,
	GreatMerchantRateModifier = 50
WHERE Type = 'POLICY_COMMERCE';

INSERT INTO Policy_BuildingClassProductionModifiers
	(PolicyType, BuildingClassType, ProductionModifier)
VALUES
	('POLICY_COMMERCE', 'BUILDINGCLASS_COALING_STATION', 100);

INSERT INTO Policy_HurryModifiers
	(PolicyType, HurryType, HurryCostModifier)
VALUES
	('POLICY_COMMERCE', 'HURRY_GOLD', -10);

-- Caravans (was Wagon Trains, now Trade Subsidies)
UPDATE Policies
SET
	RouteGoldMaintenanceMod = 0,
	GreatMerchantRateModifier = 0,
	RouteGoldMaintenanceMod = -50
WHERE Type = 'POLICY_CARAVANS';

INSERT INTO Policy_YieldFromConstruction
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_CARAVANS', 'YIELD_SCIENCE', 15);

INSERT INTO Policy_BuildingClassHappiness
	(PolicyType, BuildingClassType, Happiness)
VALUES
	('POLICY_CARAVANS', 'BUILDINGCLASS_WINDMILL', 1);

-- Trade Unions (was Mercenary Army, now Division of Labor)
UPDATE Policies
SET
	PortraitIndex = 6,
	LandTradeRouteGoldChange = 500,
	SeaTradeRouteGoldChange = 500,
	IconAtlas = 'POLICY_ATLAS_EXP2',
	IconAtlasAchieved = 'POLICY_A_ATLAS_EXP2'
WHERE Type = 'POLICY_TRADE_UNIONS';

INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
SELECT
	'POLICY_TRADE_UNIONS', b.Type, y.Type, 2
FROM BuildingClasses b, Yields y
WHERE b.Type IN ('BUILDINCLASS_WORKSHOP', 'BUILDINCLASS_WINDMILL','BUILDINCLASS_COALING_STATION','BUILDINCLASS_FACTORY')
AND y.Type IN ('YIELD_PRODUCTION', 'YIELD_CULTURE');

-- Entrepreneurship
UPDATE Policies
SET
	GreatMerchantRateModifier = 0,
	EmbarkedExtraMoves = 0,
	TradeMissionGoldModifier = 0
WHERE Type = 'POLICY_ENTREPRENEURSHIP';

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES		
	('POLICY_ENTREPRENEURSHIP', 'IMPROVEMENT_CUSTOMS_HOUSE', 'YIELD_CULTURE', 3),
	('POLICY_ENTREPRENEURSHIP', 'IMPROVEMENT_CUSTOMS_HOUSE', 'YIELD_FOOD', 3),
	('POLICY_ENTREPRENEURSHIP', 'IMPROVEMENT_TRADING_POST', 'YIELD_CULTURE', 1),
	('POLICY_ENTREPRENEURSHIP', 'IMPROVEMENT_TRADING_POST', 'YIELD_GOLD', 2),
	('POLICY_ENTREPRENEURSHIP', 'IMPROVEMENT_MINE', 'YIELD_PRODUCTION', 2),
	('POLICY_ENTREPRENEURSHIP', 'IMPROVEMENT_MINE', 'YIELD_GOLD', 1),
	('POLICY_ENTREPRENEURSHIP', 'IMPROVEMENT_FARM', 'YIELD_PRODUCTION', 2),
	('POLICY_ENTREPRENEURSHIP', 'IMPROVEMENT_FARM', 'YIELD_GOLD', 1),
	('POLICY_ENTREPRENEURSHIP', 'IMPROVEMENT_LUMBERMILL', 'YIELD_PRODUCTION', 2),
	('POLICY_ENTREPRENEURSHIP', 'IMPROVEMENT_LUMBERMILL', 'YIELD_GOLD', 1);

-- Mercantilism
DELETE FROM Policy_BuildingClassYieldChanges
WHERE PolicyType = 'POLICY_MERCANTILISM';

DELETE FROM Policy_HurryModifiers -- will be re-inserted in scaler
WHERE PolicyType = 'POLICY_MERCANTILISM';

UPDATE Policies SET FreeTradeRoute = 2 WHERE Type = 'POLICY_MERCANTILISM';

INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
SELECT
	'POLICY_MERCANTILISM', b.Type, y.Type, 2
FROM BuildingClasses b, Yields y
WHERE b.Type IN ('BUILDINCLASS_MINT', 'BUILDINCLASS_BANK','BUILDINCLASS_HOTEL','BUILDINCLASS_STOCK_EXCHANGE')
AND y.Type IN ('YIELD_GOLD', 'YIELD_SCIENCE');

-- Protectionism (now Gold Standard)
UPDATE Policies
SET
	FreeTradeRoute = 0,
	ExtraHappinessPerLuxury = 0,
	TradeMissionGoldModifier = 25,
	InvestmentModifier = -10
WHERE Type = 'POLICY_PROTECTIONISM';

INSERT INTO Policy_WLTKDYieldMod
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_PROTECTIONISM', 'YIELD_GOLD', 10),
	('POLICY_PROTECTIONISM', 'YIELD_CULTURE', 10);

-- Finisher
DELETE FROM Policy_ImprovementYieldChanges
WHERE PolicyType = 'POLICY_COMMERCE_FINISHER';

INSERT INTO Policy_ResourceYieldChanges
	(PolicyType, ResourceType, YieldType, Yield)
SELECT
	'POLICY_COMMERCE_FINISHER', Type, 'YIELD_PRODUCTION', 1
FROM Resources  -- bonus resources
WHERE ResourceUsage = 0 AND Type NOT IN ('RESOURCE_ARTIFACTS', 'RESOURCE_HIDDEN_ARTIFACTS') UNION ALL
SELECT
	'POLICY_COMMERCE_FINISHER', Type, 'YIELD_SCIENCE', 2
FROM Resources WHERE ResourceUsage = 0 AND Type NOT IN ('RESOURCE_ARTIFACTS', 'RESOURCE_HIDDEN_ARTIFACTS');

UPDATE Policies
SET ExtraHappinessPerLuxury = 3
WHERE Type = 'POLICY_COMMERCE_FINISHER';

-- Scaler
INSERT INTO Policy_HurryModifiers
	(PolicyType, HurryType, HurryCostModifier)
SELECT
	Type, 'HURRY_GOLD', -4
FROM Policies
WHERE PolicyBranchType = 'POLICY_BRANCH_COMMERCE';
