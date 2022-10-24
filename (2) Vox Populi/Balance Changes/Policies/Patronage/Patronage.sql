-- Unlock Time

UPDATE PolicyBranchTypes
SET EraPrereq = 'ERA_MEDIEVAL'
WHERE Type = 'POLICY_BRANCH_PATRONAGE';


-- Shift Policies Around

UPDATE Policies
SET
	GridX = 4, GridY = 1
WHERE Type = 'POLICY_MERCHANT_CONFEDERACY';

UPDATE Policies
SET
	GridX = 2, GridY = 3
WHERE Type = 'POLICY_CONSULATES';

UPDATE Policies
SET
	GridX = 4, GridY = 3
WHERE Type = 'POLICY_CULTURAL_DIPLOMACY';

DELETE FROM Policy_PrereqPolicies
WHERE PolicyType IN
('POLICY_CULTURAL_DIPLOMACY',
 'POLICY_MERCHANT_CONFEDERACY',
 'POLICY_CONSULATES');

INSERT INTO Policy_PrereqPolicies
	(PolicyType, PrereqPolicy)
VALUES
	('POLICY_CONSULATES', 'POLICY_SCHOLASTICISM'),
	('POLICY_CULTURAL_DIPLOMACY', 'POLICY_MERCHANT_CONFEDERACY'),
	('POLICY_CULTURAL_DIPLOMACY', 'POLICY_SCHOLASTICISM');


-- Opener (now Statecraft)

UPDATE Policies
SET
	MinorFriendshipMinimum = 0,
	MinorFriendshipDecayMod = 0
WHERE Type = 'POLICY_PATRONAGE';

INSERT INTO Policy_CapitalYieldPerPopChangeEmpire
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_PATRONAGE', 'YIELD_FOOD', 20),
	('POLICY_PATRONAGE', 'YIELD_GOLD', 20),
	('POLICY_PATRONAGE', 'YIELD_CULTURE', 20),
	('POLICY_PATRONAGE', 'YIELD_SCIENCE', 20),
	('POLICY_PATRONAGE', 'YIELD_FAITH', 20),
	('POLICY_PATRONAGE', 'YIELD_PRODUCTION', 20),
	('POLICY_PATRONAGE', 'YIELD_GOLDEN_AGE_POINTS', 20),
	('POLICY_PATRONAGE', 'YIELD_TOURISM', 20);


-- Philanthropy (Foreign Service)

UPDATE Policies
SET
	MinorScienceAllies = 0,
	MinorGoldFriendshipMod = 0,
	MissionInfluenceModifier = 10,
	TradeRouteTourismModifier = 15,
	IncreasedQuestRewards = 50,
	FreeSpy = 1,
	PortraitIndex = 39
WHERE Type = 'POLICY_PHILANTHROPY';


-- Consulates

UPDATE Policies
SET
	MinorFriendshipMinimum = 0,
	MissionInfluenceModifier = 10,
	FreeWCVotes = 1,
	EventTourismCS = 4
WHERE Type = 'POLICY_CONSULATES';


-- Scholasticism (now Shadow Networks)

UPDATE Policies
SET
	MissionInfluenceModifier = 10,
	MinorScienceAllies = 1,
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


-- Cultural Diplomacy (now Exchange Markets)

UPDATE Policies
SET
	MinorResourceBonus = 0,
	MissionInfluenceModifier = 10,
	FreeTradeRoute = 1,
	HappinessPerActiveTradeRoute = 1,
	CSResourcesCountForMonopolies = 1
WHERE Type = 'POLICY_CULTURAL_DIPLOMACY';

UPDATE Defines
SET
	Value = 100
WHERE Name = 'MINOR_POLICY_RESOURCE_HAPPINESS_MULTIPLIER';


-- Merchant Confederacy (now Trade Confederacy)

UPDATE Policies
SET
	CityStateTradeChange = 0,
	ExtraCultureandScienceTradeRoutes = 0,
	MissionInfluenceModifier = 10,
	ProtectedMinorPerTurnInfluence = 100,
	TradeRouteYieldModifier = 25
WHERE Type = 'POLICY_MERCHANT_CONFEDERACY';


-- Finisher

UPDATE Policies
SET
	MinorGreatPeopleAllies = 0,
	XCSAlliesLowersPolicyNeedWonders = 3
WHERE Type = 'POLICY_PATRONAGE_FINISHER';

INSERT INTO Policy_YieldFromDelegateCount
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_PATRONAGE_FINISHER', 'YIELD_CULTURE', 5),
	('POLICY_PATRONAGE_FINISHER', 'YIELD_SCIENCE', 5),
	('POLICY_PATRONAGE_FINISHER', 'YIELD_GOLD', 5);



----------------------
-- Combined Insertions
----------------------

-- Scaler

INSERT INTO Policy_CityYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_PATRONAGE', 'YIELD_GOLD', 1),
	('POLICY_PHILANTHROPY', 'YIELD_GOLD', 1),
	('POLICY_CONSULATES', 'YIELD_GOLD', 1),
	('POLICY_SCHOLASTICISM', 'YIELD_GOLD', 1),
	('POLICY_CULTURAL_DIPLOMACY', 'YIELD_GOLD', 1),
	('POLICY_MERCHANT_CONFEDERACY', 'YIELD_GOLD', 1);