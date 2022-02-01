-- Unlock Time
UPDATE PolicyBranchTypes
SET EraPrereq = 'ERA_MEDIEVAL'
WHERE Type = 'POLICY_BRANCH_PATRONAGE';

-- Opener
UPDATE Policies
SET MinorFriendshipMinimum = '0'
WHERE Type = 'POLICY_PATRONAGE';

UPDATE Policies
SET MinorFriendshipDecayMod = '0'
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
	('POLICY_PATRONAGE', 'YIELD_TOURISM', 20);

INSERT INTO Policy_CityYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_PATRONAGE', 'YIELD_GOLD', 1),
	('POLICY_PHILANTHROPY', 'YIELD_GOLD', 1),
	('POLICY_CONSULATES', 'YIELD_GOLD', 1),
	('POLICY_SCHOLASTICISM', 'YIELD_GOLD', 1),
	('POLICY_CULTURAL_DIPLOMACY', 'YIELD_GOLD', 1),
	('POLICY_MERCHANT_CONFEDERACY', 'YIELD_GOLD', 1);

-- Philanthropy

UPDATE Policies
SET MinorScienceAllies = '0'
WHERE Type = 'POLICY_PHILANTHROPY';

UPDATE Policies
SET MissionInfluenceModifier = '10'
WHERE Type = 'POLICY_PHILANTHROPY';

UPDATE Policies
SET MinorGoldFriendshipMod = '0'
WHERE Type = 'POLICY_PHILANTHROPY';

UPDATE Policies
SET TradeRouteTourismModifier = '15'
WHERE Type = 'POLICY_PHILANTHROPY';

UPDATE Policies
SET PortraitIndex = '39'
WHERE Type = 'POLICY_PHILANTHROPY';

UPDATE Policies
SET IncreasedQuestRewards = '50'
WHERE Type = 'POLICY_PHILANTHROPY';

UPDATE Policies
SET FreeSpy = '1'
WHERE Type = 'POLICY_PHILANTHROPY';

-- Consulates
UPDATE Policies
SET MissionInfluenceModifier = '10'
WHERE Type = 'POLICY_CONSULATES';

UPDATE Policies
SET MinorFriendshipMinimum = '0'
WHERE Type = 'POLICY_CONSULATES';

UPDATE Policies
SET FreeWCVotes = '1'
WHERE Type = 'POLICY_CONSULATES';

UPDATE Policies
SET EventTourismCS = '4'
WHERE Type = 'POLICY_CONSULATES';

UPDATE Policies
SET GridX = '2'
WHERE Type = 'POLICY_CONSULATES';

UPDATE Policies
SET GridY = '3'
WHERE Type = 'POLICY_CONSULATES';

DELETE FROM Policy_PrereqPolicies
WHERE PolicyType = 'POLICY_CONSULATES';

-- Scholasticism
UPDATE Policies
SET MissionInfluenceModifier = '10'
WHERE Type = 'POLICY_SCHOLASTICISM';

UPDATE Policies
SET MinorScienceAllies = '1'
WHERE Type = 'POLICY_SCHOLASTICISM';

UPDATE Policies
SET PortraitIndex = '37'
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

-- Cultural Diplomacy
UPDATE Policies
SET MissionInfluenceModifier = '10'
WHERE Type = 'POLICY_CULTURAL_DIPLOMACY';

UPDATE Policies
SET MinorResourceBonus = '0'
WHERE Type = 'POLICY_CULTURAL_DIPLOMACY';

UPDATE Defines
SET Value = '100'
WHERE Name = 'MINOR_POLICY_RESOURCE_HAPPINESS_MULTIPLIER';

UPDATE Policies
SET FreeTradeRoute = '1'
WHERE Type = 'POLICY_CULTURAL_DIPLOMACY';

UPDATE Policies
SET HappinessPerActiveTradeRoute = '1'
WHERE Type = 'POLICY_CULTURAL_DIPLOMACY';

UPDATE Policies
SET CSResourcesCountForMonopolies = '1'
WHERE Type = 'POLICY_CULTURAL_DIPLOMACY';

UPDATE Policies
SET GridX = '4'
WHERE Type = 'POLICY_CULTURAL_DIPLOMACY';

UPDATE Policies
SET GridY = '3'
WHERE Type = 'POLICY_CULTURAL_DIPLOMACY';

DELETE FROM Policy_PrereqPolicies
WHERE PolicyType = 'POLICY_CULTURAL_DIPLOMACY';

-- Merchant Confederacy
UPDATE Policies
SET MissionInfluenceModifier = '10'
WHERE Type = 'POLICY_MERCHANT_CONFEDERACY';

UPDATE Policies
SET CityStateTradeChange = '0'
WHERE Type = 'POLICY_MERCHANT_CONFEDERACY';

UPDATE Policies
SET ProtectedMinorPerTurnInfluence = '100'
WHERE Type = 'POLICY_MERCHANT_CONFEDERACY';

UPDATE Policies
SET GridX = '4'
WHERE Type = 'POLICY_MERCHANT_CONFEDERACY';

UPDATE Policies
SET GridY = '1'
WHERE Type = 'POLICY_MERCHANT_CONFEDERACY';

DELETE FROM Policy_PrereqPolicies
WHERE PolicyType = 'POLICY_MERCHANT_CONFEDERACY';

UPDATE Policies
SET TradeRouteYieldModifier = '25'
WHERE Type = 'POLICY_MERCHANT_CONFEDERACY';

UPDATE Policies
SET ExtraCultureandScienceTradeRoutes = '0'
WHERE Type = 'POLICY_MERCHANT_CONFEDERACY';

-- Finisher

UPDATE Policies
SET XCSAlliesLowersPolicyNeedWonders = '3'
WHERE Type = 'POLICY_PATRONAGE_FINISHER';

UPDATE Policies
SET MinorGreatPeopleAllies = '0'
WHERE Type = 'POLICY_PATRONAGE_FINISHER';

INSERT INTO Policy_YieldFromDelegateCount
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_PATRONAGE_FINISHER', 'YIELD_CULTURE', 5),
	('POLICY_PATRONAGE_FINISHER', 'YIELD_SCIENCE', 5),
	('POLICY_PATRONAGE_FINISHER', 'YIELD_GOLD', 5);

--UPDATE Policies
--SET UnlocksPolicyBranchEra = 'ERA_RENAISSANCE'
--WHERE Type = 'POLICY_PATRONAGE_FINISHER';

-- Finisher
--UPDATE Policies
--SET IdeologyPoint = '1'
--WHERE Type = 'POLICY_PATRONAGE_FINISHER';

-- NEW

INSERT INTO Policy_PrereqPolicies
	(PolicyType, PrereqPolicy)
VALUES
	('POLICY_CONSULATES', 'POLICY_SCHOLASTICISM'),
	('POLICY_CULTURAL_DIPLOMACY', 'POLICY_MERCHANT_CONFEDERACY'),
	('POLICY_CULTURAL_DIPLOMACY', 'POLICY_SCHOLASTICISM');