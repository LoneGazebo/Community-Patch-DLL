-- Unlock Time
UPDATE PolicyBranchTypes
SET EraPrereq = 'ERA_RENAISSANCE'
WHERE Type = 'POLICY_BRANCH_EXPLORATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Opener -- Now called Imperialism -- Grants +1 Movement and +1 Sight for Naval Units, and speeds production of land units by +5%, with an additional +4% for every policy unlocked in Imperialism. Receive 1 extra happiness for every owned luxury, and 1 Happiness for every garrisoned unit.

-- Maritime Infrastructure -- Now called Organization -- +2 Production and +1 Gold from Seaports, Workshops, and Factories
DELETE FROM Policy_CoastalCityYieldChanges
WHERE PolicyType = 'POLICY_MARITIME_INFRASTRUCTURE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Merchant Navy -- Now called Exploitation -- +1 Food and +1 Production from Snow, Desert and Mountain Tiles
DELETE FROM Policy_BuildingClassYieldChanges
WHERE PolicyType = 'POLICY_MERCHANT_NAVY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Naval Tradition -- Military Tradition -- +33% to Great Admiral and Great General Production -- Barracks, Armories and Military Academies provide +1 Science.
DELETE FROM Policy_BuildingClassHappiness
WHERE PolicyType = 'POLICY_NAVAL_TRADITION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Policies
SET GreatAdmiralRateModifier = '33'
WHERE Type = 'POLICY_NAVAL_TRADITION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Policies
SET GreatGeneralRateModifier = '33'
WHERE Type = 'POLICY_NAVAL_TRADITION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Navigation School -- Now called Exploration -- Receive a free Archaeologist -- Reveals Hidden Sites.
UPDATE Policy_FreeUnitClasses
SET Count = '2'
WHERE PolicyType = 'POLICY_NAVIGATION_SCHOOL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Policy_FreeUnitClasses
SET UnitClassType = 'UNITCLASS_ARCHAEOLOGIST'
WHERE PolicyType = 'POLICY_NAVIGATION_SCHOOL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Resources
SET PolicyReveal = 'POLICY_NAVIGATION_SCHOOL'
WHERE Type = 'RESOURCE_HIDDEN_ARTIFACTS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

DELETE FROM Policy_FreePromotions
WHERE PolicyType = 'POLICY_NAVIGATION_SCHOOL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Policies
SET GreatAdmiralRateModifier = '0'
WHERE Type = 'POLICY_NAVIGATION_SCHOOL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Treasure Fleets -- Now called Civilizing Mission -- Receive a free Factory, and a lump sum of Gold, when you conquer a city. Garrisons are free.
UPDATE Policies
SET SeaTradeRouteGoldChange = '0'
WHERE Type = 'POLICY_TREASURE_FLEETS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Policies
SET FreeBuildingOnConquest = 'BUILDING_FACTORY'
WHERE Type = 'POLICY_TREASURE_FLEETS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Policies
SET GarrisonFreeMaintenance = '1'
WHERE Type = 'POLICY_TREASURE_FLEETS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Finisher

UPDATE Policies
SET ExtraHappinessPerLuxury = '1'
WHERE Type = 'POLICY_EXPLORATION_FINISHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );
