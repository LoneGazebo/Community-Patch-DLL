-- City Strategy Updates
DELETE FROM AICityStrategies WHERE Type = 'AICITYSTRATEGY_NO_NEED_INTERNATIONAL_LAND_TRADE_ROUTE';
DELETE FROM AICityStrategies WHERE Type = 'AICITYSTRATEGY_NO_NEED_INTERNATIONAL_SEA_TRADE_ROUTE';
DELETE FROM AICityStrategy_Flavors WHERE AICityStrategyType = 'AICITYSTRATEGY_NO_NEED_INTERNATIONAL_LAND_TRADE_ROUTE';
DELETE FROM AICityStrategy_Flavors WHERE AICityStrategyType = 'AICITYSTRATEGY_NO_NEED_INTERNATIONAL_SEA_TRADE_ROUTE';

UPDATE AICityStrategy_Flavors SET Flavor = '100' WHERE AICityStrategyType = 'AICITYSTRATEGY_NEED_INTERNATIONAL_LAND_TRADE_ROUTE';
UPDATE AICityStrategy_Flavors SET Flavor = '100' WHERE AICityStrategyType = 'AICITYSTRATEGY_NEED_INTERNATIONAL_SEA_TRADE_ROUTE';

UPDATE Defines SET Value = '4' WHERE Name = 'AI_CITYSTRATEGY_WANT_TILE_IMPROVERS_MINIMUM_SIZE';

UPDATE AICityStrategies SET TechPrereq = 'TECH_AGRICULTURE' WHERE Type = 'AICITYSTRATEGY_FIRST_FAITH_BUILDING';


-- Mobile SAM -- reduce interest
UPDATE Unit_Flavors SET Flavor = '15' WHERE UnitType = 'UNIT_MOBILE_SAM' AND FlavorType = 'FLAVOR_ANTIAIR';


-- Misc. AI Defines
UPDATE Defines SET Value = '-2' WHERE Name = 'WAR_PROGRESS_PER_UNHAPPY';


-- Era Diplo Values
UPDATE Eras SET DiploEmphasisReligion = '2' WHERE Type = 'ERA_ANCIENT';
UPDATE Eras SET DiploEmphasisReligion = '4' WHERE Type = 'ERA_CLASSICAL';
UPDATE Eras SET DiploEmphasisReligion = '6' WHERE Type = 'ERA_MEDIEVAL';
UPDATE Eras SET DiploEmphasisReligion = '5' WHERE Type = 'ERA_RENAISSANCE';
UPDATE Eras SET DiploEmphasisReligion = '4' WHERE Type = 'ERA_INDUSTRIAL';
UPDATE Eras SET DiploEmphasisReligion = '3' WHERE Type = 'ERA_MODERN';
UPDATE Eras SET DiploEmphasisReligion = '2' WHERE Type = 'ERA_POSTMODERN';
UPDATE Eras SET DiploEmphasisReligion = '1' WHERE Type = 'ERA_FUTURE';

UPDATE Eras SET DiploEmphasisLatePolicies = '7';
UPDATE Eras SET DiploEmphasisLatePolicies = '9' WHERE Type = 'ERA_MODERN';
UPDATE Eras SET DiploEmphasisLatePolicies = '10' WHERE Type = 'ERA_POSTMODERN';
UPDATE Eras SET DiploEmphasisLatePolicies = '8' WHERE Type = 'ERA_FUTURE';