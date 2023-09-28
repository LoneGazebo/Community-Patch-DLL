DELETE FROM Belief_TerrainYieldChanges;
DELETE FROM Belief_FeatureYieldChanges;
DELETE FROM Belief_ResourceYieldChanges;
DELETE FROM Belief_ImprovementYieldChanges;
DELETE FROM Belief_BuildingClassYieldChanges;
DELETE FROM Belief_BuildingClassHappiness;
DELETE FROM Belief_BuildingClassTourism;
DELETE FROM Belief_YieldChangeTradeRoute;
DELETE FROM Belief_YieldChangeNaturalWonder;
DELETE FROM Belief_YieldChangePerForeignCity;
DELETE FROM Belief_YieldChangePerXForeignFollowers;
DELETE FROM Belief_YieldChangeWorldWonder;
DELETE FROM Belief_YieldChangeAnySpecialist;
DELETE FROM Belief_MaxYieldModifierPerFollower;
DELETE FROM Belief_BuildingClassFaithPurchase;
DELETE FROM Belief_EraFaithUnitPurchase;

DELETE FROM Beliefs WHERE Type = 'BELIEF_FAITH_HEALERS';

DELETE FROM Belief_CityYieldChanges WHERE BeliefType = 'BELIEF_GOD_CRAFTSMEN';
DELETE FROM Beliefs WHERE Type = 'BELIEF_GOD_CRAFTSMEN';
