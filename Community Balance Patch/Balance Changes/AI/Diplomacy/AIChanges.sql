DELETE FROM AICityStrategies
WHERE Type = 'AICITYSTRATEGY_NO_NEED_INTERNATIONAL_LAND_TRADE_ROUTE';

DELETE FROM AICityStrategies
WHERE Type = 'AICITYSTRATEGY_NO_NEED_INTERNATIONAL_SEA_TRADE_ROUTE';

DELETE FROM AICityStrategy_Flavors
WHERE AICityStrategyType = 'AICITYSTRATEGY_NO_NEED_INTERNATIONAL_LAND_TRADE_ROUTE';

DELETE FROM AICityStrategy_Flavors
WHERE AICityStrategyType = 'AICITYSTRATEGY_NO_NEED_INTERNATIONAL_SEA_TRADE_ROUTE';

UPDATE AICityStrategy_Flavors
SET Flavor = '100' WHERE AICityStrategyType = 'AICITYSTRATEGY_NEED_INTERNATIONAL_LAND_TRADE_ROUTE';

UPDATE AICityStrategy_Flavors
SET Flavor = '100' WHERE AICityStrategyType = 'AICITYSTRATEGY_NEED_INTERNATIONAL_SEA_TRADE_ROUTE';

UPDATE AICityStrategies
SET TechPrereq = 'TECH_AGRICULTURE' WHERE Type = 'AICITYSTRATEGY_FIRST_FAITH_BUILDING';

-- Era Diplo Values

UPDATE Eras
SET DiploEmphasisReligion = '2' WHERE Type = 'ERA_ANCIENT';

UPDATE Eras
SET DiploEmphasisReligion = '4' WHERE Type = 'ERA_CLASSICAL';

UPDATE Eras
SET DiploEmphasisReligion = '6' WHERE Type = 'ERA_MEDIEVAL';

UPDATE Eras
SET DiploEmphasisReligion = '5' WHERE Type = 'ERA_RENAISSANCE';

UPDATE Eras
SET DiploEmphasisReligion = '4' WHERE Type = 'ERA_INDUSTRIAL';

UPDATE Eras
SET DiploEmphasisReligion = '3' WHERE Type = 'ERA_MODERN';

UPDATE Eras
SET DiploEmphasisReligion = '2' WHERE Type = 'ERA_POSTMODERN';

UPDATE Eras
SET DiploEmphasisReligion = '1' WHERE Type = 'ERA_FUTURE';

UPDATE Eras
SET DiploEmphasisLatePolicies = '7' WHERE Type = 'ERA_ANCIENT';

UPDATE Eras
SET DiploEmphasisLatePolicies = '7' WHERE Type = 'ERA_CLASSICAL';

UPDATE Eras
SET DiploEmphasisLatePolicies = '7' WHERE Type = 'ERA_MEDIEVAL';

UPDATE Eras
SET DiploEmphasisLatePolicies = '7' WHERE Type = 'ERA_RENAISSANCE';

UPDATE Eras
SET DiploEmphasisLatePolicies = '7' WHERE Type = 'ERA_INDUSTRIAL';

UPDATE Eras
SET DiploEmphasisLatePolicies = '9' WHERE Type = 'ERA_MODERN';

UPDATE Eras
SET DiploEmphasisLatePolicies = '10' WHERE Type = 'ERA_POSTMODERN';

UPDATE Eras
SET DiploEmphasisLatePolicies = '8' WHERE Type = 'ERA_FUTURE';