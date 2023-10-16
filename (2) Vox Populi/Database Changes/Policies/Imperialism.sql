-- Tree structure
UPDATE Policies
SET GridX = 1, GridY = 1
WHERE Type = 'POLICY_TREASURE_FLEETS';

UPDATE Policies
SET GridX = 3, GridY = 1
WHERE Type = 'POLICY_NAVIGATION_SCHOOL';

UPDATE Policies
SET GridX = 5, GridY = 1
WHERE Type = 'POLICY_NAVAL_TRADITION';

UPDATE Policies
SET GridX = 1, GridY = 2
WHERE Type = 'POLICY_MERCHANT_NAVY';

UPDATE Policies
SET GridX = 5, GridY = 2
WHERE Type = 'POLICY_MARITIME_INFRASTRUCTURE';

DELETE FROM Policy_PrereqPolicies
WHERE PolicyType IN (
	'POLICY_NAVIGATION_SCHOOL',
	'POLICY_MERCHANT_NAVY',
	'POLICY_TREASURE_FLEETS'
);

INSERT INTO Policy_PrereqPolicies
	(PolicyType, PrereqPolicy)
VALUES
	('POLICY_MARITIME_INFRASTRUCTURE', 'POLICY_NAVAL_TRADITION'),
	('POLICY_MERCHANT_NAVY', 'POLICY_TREASURE_FLEETS');

-- Opener (now Imperialism)
UPDATE Policies
SET
	EmbarkedExtraMoves = 1,
	UnitUpgradeCostMod = -10
WHERE Type = 'POLICY_EXPLORATION';

INSERT INTO Policy_UnitCombatProductionModifiers
	(PolicyType, UnitCombatType, ProductionModifier)
SELECT
	'POLICY_EXPLORATION', Type, 10
FROM UnitCombatInfos
WHERE IsMilitary = 1;

INSERT INTO Policy_FreePromotions
	(PolicyType, PromotionType)
VALUES
	('POLICY_EXPLORATION', 'PROMOTION_NAVAL_TRADITION'),
	('POLICY_EXPLORATION', 'PROMOTION_IMPERIALISM_OPENER'),
	('POLICY_EXPLORATION', 'PROMOTION_FASTER_GENERAL');

-- Treasure Fleets (now Civilizing Mission)
UPDATE Policies
SET
	SeaTradeRouteGoldChange = 0,
	ConquestPerEraBuildingProductionMod = 10,
	KeepConqueredBuildings = 1
WHERE Type = 'POLICY_TREASURE_FLEETS';

INSERT INTO Policy_ConquerorYield
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_TREASURE_FLEETS', 'YIELD_GOLD', 75);

-- Navigation School (now Regimental System)
UPDATE Policies
SET
	IncludesOneShotFreeUnits = 0,
	GreatAdmiralRateModifier = 33,
	GreatGeneralRateModifier = 33,
	UpgradeCSVassalTerritory = 1,
	AdmiralLuxuryBonus = 2
WHERE Type = 'POLICY_NAVIGATION_SCHOOL';

DELETE FROM Policy_FreeUnitClasses
WHERE PolicyType = 'POLICY_NAVIGATION_SCHOOL';

DELETE FROM Policy_FreePromotions
WHERE PolicyType = 'POLICY_NAVIGATION_SCHOOL';

INSERT INTO Policy_FreePromotions
	(PolicyType, PromotionType)
VALUES
	('POLICY_NAVIGATION_SCHOOL', 'PROMOTION_BETTER_LEADERSHIP');

-- Naval Tradition (now Colonialism)
UPDATE Policies
SET
	MonopolyModPercent = 10,
	MonopolyModFlat = 3
WHERE Type = 'POLICY_NAVAL_TRADITION';

DELETE FROM Policy_BuildingClassHappiness
WHERE PolicyType = 'POLICY_NAVAL_TRADITION';

INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
VALUES
	('POLICY_NAVAL_TRADITION', 'BUILDINGCLASS_BARRACKS', 'YIELD_SCIENCE', 2),
	('POLICY_NAVAL_TRADITION', 'BUILDINGCLASS_ARMORY', 'YIELD_SCIENCE', 2),
	('POLICY_NAVAL_TRADITION', 'BUILDINGCLASS_MILITARY_ACADEMY', 'YIELD_SCIENCE', 2);

INSERT INTO Policy_BuildingClassCultureChanges
	(PolicyType, BuildingClassType, CultureChange)
VALUES
	('POLICY_NAVAL_TRADITION', 'BUILDINGCLASS_BARRACKS', 1),
	('POLICY_NAVAL_TRADITION', 'BUILDINGCLASS_ARMORY', 1),
	('POLICY_NAVAL_TRADITION', 'BUILDINGCLASS_MILITARY_ACADEMY', 1);

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_NAVAL_TRADITION', 'IMPROVEMENT_FORT', 'YIELD_SCIENCE', 2),
	('POLICY_NAVAL_TRADITION', 'IMPROVEMENT_FORT', 'YIELD_CULTURE', 1),
	('POLICY_NAVAL_TRADITION', 'IMPROVEMENT_CITADEL', 'YIELD_SCIENCE', 2),
	('POLICY_NAVAL_TRADITION', 'IMPROVEMENT_CITADEL', 'YIELD_CULTURE', 1),
	('POLICY_NAVAL_TRADITION', 'IMPROVEMENT_MONGOLIA_ORDO', 'YIELD_SCIENCE', 2),
	('POLICY_NAVAL_TRADITION', 'IMPROVEMENT_MONGOLIA_ORDO', 'YIELD_CULTURE', 1);

-- Merchant Navy (now Exploitation)
DELETE FROM Policy_BuildingClassYieldChanges
WHERE PolicyType = 'POLICY_MERCHANT_NAVY';

DELETE FROM Policy_BuildingClassCultureChanges
WHERE PolicyType = 'POLICY_MERCHANT_NAVY';

INSERT INTO Policy_PlotYieldChanges
	(PolicyType, PlotType, YieldType, Yield)
VALUES
	('POLICY_MERCHANT_NAVY', 'PLOT_OCEAN', 'YIELD_SCIENCE', 1),
	('POLICY_MERCHANT_NAVY', 'PLOT_OCEAN', 'YIELD_PRODUCTION', 1);

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_MERCHANT_NAVY', 'IMPROVEMENT_FARM', 'YIELD_FOOD', 2),
	('POLICY_MERCHANT_NAVY', 'IMPROVEMENT_FARM', 'YIELD_PRODUCTION', 1),
	('POLICY_MERCHANT_NAVY', 'IMPROVEMENT_CAMP', 'YIELD_FOOD', 2),
	('POLICY_MERCHANT_NAVY', 'IMPROVEMENT_CAMP', 'YIELD_PRODUCTION', 1),
	('POLICY_MERCHANT_NAVY', 'IMPROVEMENT_PLANTATION', 'YIELD_FOOD', 2),
	('POLICY_MERCHANT_NAVY', 'IMPROVEMENT_PLANTATION', 'YIELD_PRODUCTION', 1);

-- Maritime Infrastructure (now Martial Law)
DELETE FROM Policy_CoastalCityYieldChanges
WHERE PolicyType = 'POLICY_MARITIME_INFRASTRUCTURE';

UPDATE Policies
SET
	GarrisonFreeMaintenance = 1,
	HappinessPerGarrisonedUnit = 1,
	CulturePerGarrisonedUnit = 4,
	PuppetYieldPenaltyMod = 20,
	PortraitIndex = 0,
	IconAtlas = 'EXPANSIONPATCH_POLICY_ATLAS',
	IconAtlasAchieved = 'EXPANSIONPATCH_POLICY_ACHIEVED_ATLAS'
WHERE Type = 'POLICY_MARITIME_INFRASTRUCTURE';

-- Finisher
INSERT INTO Policy_FreePromotions
	(PolicyType, PromotionType)
VALUES
	('POLICY_EXPLORATION_FINISHER', 'PROMOTION_NAVAL_DEFENSE_BOOST');

-- Scaler
INSERT INTO Policy_UnitCombatProductionModifiers
	(PolicyType, UnitCombatType, ProductionModifier)
SELECT
	p.Type, uc.Type, 5
FROM Policies p, UnitCombatInfos uc
WHERE p.PolicyBranchType = 'POLICY_BRANCH_EXPLORATION' AND uc.IsMilitary = 1;

UPDATE Policies
SET UnitUpgradeCostMod = -5
WHERE PolicyBranchType = 'POLICY_BRANCH_EXPLORATION';
