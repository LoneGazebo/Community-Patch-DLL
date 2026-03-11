-- All civs start with an extra Pathfinder
UPDATE Eras SET StartingExploreUnits = 1 WHERE Type = 'ERA_ANCIENT';

-- Enable Vassalage
UPDATE Eras SET VassalageEnabled = 1 WHERE Type = 'ERA_MEDIEVAL';

-- Specialist Food Costs
UPDATE Eras SET SpecialistExtraFoodCost = 1 WHERE Type = 'ERA_ANCIENT';
UPDATE Eras SET SpecialistExtraFoodCost = 2 WHERE Type = 'ERA_CLASSICAL';
UPDATE Eras SET SpecialistExtraFoodCost = 3 WHERE Type = 'ERA_MEDIEVAL';
UPDATE Eras SET SpecialistExtraFoodCost = 4 WHERE Type = 'ERA_RENAISSANCE';
UPDATE Eras SET SpecialistExtraFoodCost = 5 WHERE Type = 'ERA_INDUSTRIAL';
UPDATE Eras SET SpecialistExtraFoodCost = 6 WHERE Type = 'ERA_MODERN';
UPDATE Eras SET SpecialistExtraFoodCost = 7 WHERE Type = 'ERA_POSTMODERN';
UPDATE Eras SET SpecialistExtraFoodCost = 8 WHERE Type = 'ERA_FUTURE';
