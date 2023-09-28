-- Tree structure
UPDATE Policies
SET GridX = 4, GridY = 1
WHERE Type = 'POLICY_MILITARY_TRADITION';

UPDATE Policies
SET GridX = 2, GridY = 1
WHERE Type = 'POLICY_DISCIPLINE';

UPDATE Policies
SET GridX = 2, GridY = 2
WHERE Type = 'POLICY_WARRIOR_CODE';

DELETE FROM Policy_PrereqPolicies
WHERE PrereqPolicy IN (
	'POLICY_MILITARY_TRADITION',
	'POLICY_WARRIOR_CODE',
	'POLICY_DISCIPLINE'
);

INSERT INTO Policy_PrereqPolicies
	(PolicyType, PrereqPolicy)
VALUES
	('POLICY_WARRIOR_CODE', 'POLICY_DISCIPLINE'),
	('POLICY_MILITARY_CASTE', 'POLICY_MILITARY_TRADITION');

-- Opener (now Authority)
UPDATE Policies
SET
	CultureFromBarbarianKills = 0,
	CultureFromKills = 100,
	BarbarianCombatBonus = 25
WHERE Type = 'POLICY_HONOR';

INSERT INTO Policy_CityYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_HONOR', 'YIELD_PRODUCTION', 1);

-- Warrior Code (Now Tribute)
DELETE FROM Policy_UnitCombatProductionModifiers
WHERE PolicyType = 'POLICY_WARRIOR_CODE';

DELETE FROM Policy_FreeUnitClasses
WHERE PolicyType = 'POLICY_WARRIOR_CODE';

UPDATE Policies
SET
	GreatGeneralRateModifier = 0,
	IncludesOneShotFreeUnits = 1
WHERE Type = 'POLICY_WARRIOR_CODE';

INSERT INTO Policy_FreeUnitClasses
	(PolicyType, UnitClassType, Count)
VALUES
	('POLICY_WARRIOR_CODE', 'UNITCLASS_SETTLER', 1);

INSERT INTO Policy_YieldFromMinorDemand
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_WARRIOR_CODE', 'YIELD_CULTURE', 25);

-- Discipline (Now Imperium)
DELETE FROM Policy_FreePromotions
WHERE PolicyType = 'POLICY_DISCIPLINE';

UPDATE Policies
SET PortraitIndex = 23
WHERE Type = 'POLICY_DISCIPLINE';

INSERT INTO Policy_YieldFromBorderGrowth
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_DISCIPLINE', 'YIELD_PRODUCTION', 20),
	('POLICY_DISCIPLINE', 'YIELD_GOLD', 20);

INSERT INTO Policy_FounderYield
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_DISCIPLINE', 'YIELD_SCIENCE', 40),
	('POLICY_DISCIPLINE', 'YIELD_CULTURE', 40);

INSERT INTO Policy_ConquerorYield
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_DISCIPLINE', 'YIELD_SCIENCE', 40),
	('POLICY_DISCIPLINE', 'YIELD_CULTURE', 40);

-- Military Tradition (Now Dominance)
UPDATE Policies
SET
	ExpModifier = 0,
	ExtraSupplyPerPopulation = 10
WHERE Type = 'POLICY_MILITARY_TRADITION';

INSERT INTO Policy_YieldFromKills
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_MILITARY_TRADITION', 'YIELD_SCIENCE', 100);

INSERT INTO Policy_FreePromotions
	(PolicyType, PromotionType)
VALUES
	('POLICY_MILITARY_TRADITION', 'PROMOTION_KILL_HEAL');

-- Military Caste (Now Militarism)
UPDATE Policies
SET
	CulturePerGarrisonedUnit = 0,
	HappinessPerGarrisonedUnit = 0,
	UnitGoldMaintenanceMod = -15,
	RouteGoldMaintenanceMod = -50,
	PortraitIndex = 22
WHERE Type = 'POLICY_MILITARY_CASTE';

INSERT INTO Policy_BuildingClassHappiness
	(PolicyType, BuildingClassType, Happiness)
VALUES
	('POLICY_MILITARY_CASTE', 'BUILDINGCLASS_BARRACKS', 1);

INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
VALUES
	('POLICY_MILITARY_CASTE', 'BUILDINGCLASS_BARRACKS', 'YIELD_CULTURE', 2);

-- Professional Army (Now Honor)
DELETE FROM Policy_BuildingClassProductionModifiers
WHERE PolicyType = 'POLICY_PROFESSIONAL_ARMY';

UPDATE Policies
SET
	UnitUpgradeCostMod = 0,
	WarWearinessModifier = 25,
	XPopulationConscription = 10,
	PortraitIndex = 20
WHERE Type = 'POLICY_PROFESSIONAL_ARMY';

INSERT INTO Policy_FreePromotions
	(PolicyType, PromotionType)
VALUES
	('POLICY_PROFESSIONAL_ARMY', 'PROMOTION_HONOR_BONUS');

-- Finisher
UPDATE Policies SET GoldFromKills = 0 WHERE Type = 'POLICY_HONOR_FINISHER';

INSERT INTO Policy_YieldFromBorderGrowth
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_HONOR_FINISHER', 'YIELD_GOLD', 20),
	('POLICY_HONOR_FINISHER', 'YIELD_PRODUCTION', 20);

INSERT INTO Policy_YieldFromMinorDemand
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_HONOR_FINISHER', 'YIELD_CULTURE', 25);

-- Scaler
INSERT INTO Policy_CityYieldChanges
	(PolicyType, YieldType, Yield)
SELECT
	Type, 'YIELD_PRODUCTION', 1
FROM Policies
WHERE PolicyBranchType = 'POLICY_BRANCH_HONOR';
