-- Tree structure
UPDATE Policies
SET GridX = 4, GridY = 1
WHERE Type = 'POLICY_MERCHANT_CONFEDERACY';

UPDATE Policies
SET GridX = 2, GridY = 3
WHERE Type = 'POLICY_CONSULATES';

UPDATE Policies
SET GridX = 4, GridY = 3
WHERE Type = 'POLICY_CULTURAL_DIPLOMACY';

DELETE FROM Policy_PrereqPolicies
WHERE PolicyType IN (
	'POLICY_CULTURAL_DIPLOMACY',
	'POLICY_MERCHANT_CONFEDERACY',
	'POLICY_CONSULATES'
);

INSERT INTO Policy_PrereqPolicies
	(PolicyType, PrereqPolicy)
VALUES
	('POLICY_CONSULATES', 'POLICY_SCHOLASTICISM'),
	('POLICY_CULTURAL_DIPLOMACY', 'POLICY_MERCHANT_CONFEDERACY'),
	('POLICY_CULTURAL_DIPLOMACY', 'POLICY_SCHOLASTICISM');

-- Opener (now Statecraft)
UPDATE Policies
SET MinorFriendshipDecayMod = 0
WHERE Type = 'POLICY_PATRONAGE';

INSERT INTO Policy_CapitalYieldPerPopChangeEmpire
	(PolicyType, YieldType, Yield)
SELECT
	'POLICY_PATRONAGE', Type, 20
FROM Yields
WHERE ID < 6; -- "All" yields

INSERT INTO Policy_CityYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_PATRONAGE', 'YIELD_GOLD', 1);

-- Philanthropy (now Foreign Service)
UPDATE Policies
SET
	MinorGoldFriendshipMod = 0,
	FreeSpy = 1,
	GreatDiplomatRateModifier = 50,
	IncreasedQuestRewards = 50,
	PortraitIndex = 39
WHERE Type = 'POLICY_PHILANTHROPY';

INSERT INTO Policy_ResourcefromCSAlly
	(PolicyType, ResourceType, Number)
SELECT
	'POLICY_PHILANTHROPY', Type, 34
FROM Resources
WHERE ResourceUsage = 1;

-- Merchant Confederacy (now Trade Confederacy)
UPDATE Policies
SET
	CityStateTradeChange = 0,
	TradeRouteYieldModifier = 25,
	ProtectedMinorPerTurnInfluence = 100
WHERE Type = 'POLICY_MERCHANT_CONFEDERACY';

-- Scholasticism (now Shadow Networks)
UPDATE Policies
SET
	-- MinorScienceAllies = 1,
	PortraitIndex = 37
WHERE Type = 'POLICY_SCHOLASTICISM';

INSERT INTO Policy_YieldModifierFromActiveSpies
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_SCHOLASTICISM', 'YIELD_CULTURE', 3);

INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
VALUES
	('POLICY_SCHOLASTICISM', 'BUILDINGCLASS_CONSTABLE', 'YIELD_SCIENCE', 3),
	('POLICY_SCHOLASTICISM', 'BUILDINGCLASS_POLICE_STATION', 'YIELD_SCIENCE', 3);

INSERT INTO Policy_SpecialistExtraYields
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_SCHOLASTICISM', 'YIELD_SCIENCE', 1);

-- Consulates
UPDATE Policies
SET
	MinorFriendshipMinimum = 0,
	FreeWCVotes = 8,
	EventTourismCS = 4
WHERE Type = 'POLICY_CONSULATES';

-- Cultural Diplomacy (now Exchange Markets)
UPDATE Policies
SET
	MinorResourceBonus = 0,
	FreeTradeRoute = 1,
	HappinessPerActiveTradeRoute = 1,
	TradeRouteTourismModifier = 15,
	CSResourcesCountForMonopolies = 1
WHERE Type = 'POLICY_CULTURAL_DIPLOMACY';

-- Finisher
UPDATE Policies
SET
	MinorGreatPeopleAllies = 0,
	XCSAlliesLowersPolicyNeedWonders = 3
WHERE Type = 'POLICY_PATRONAGE_FINISHER';

INSERT INTO Policy_YieldFromDelegateCount
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_PATRONAGE_FINISHER', 'YIELD_GOLD', 5),
	('POLICY_PATRONAGE_FINISHER', 'YIELD_SCIENCE', 5),
	('POLICY_PATRONAGE_FINISHER', 'YIELD_CULTURE', 5);

INSERT INTO Policy_FaithPurchaseUnitClasses
	(PolicyType, UnitClassType)
VALUES
	('POLICY_PATRONAGE_FINISHER', 'UNITCLASS_GREAT_DIPLOMAT');

-- Scaler
UPDATE Policies
SET MissionInfluenceModifier = 10
WHERE PolicyBranchType = 'POLICY_BRANCH_PATRONAGE';

INSERT INTO Policy_CityYieldChanges
	(PolicyType, YieldType, Yield)
SELECT
	Type, 'YIELD_GOLD', 1
FROM Policies
WHERE PolicyBranchType = 'POLICY_BRANCH_PATRONAGE';
