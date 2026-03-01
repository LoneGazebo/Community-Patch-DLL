-- All civs start with an extra Pathfinder
UPDATE Eras SET StartingExploreUnits = 1 WHERE Type = 'ERA_ANCIENT';

-- Enable Vassalage
UPDATE Eras SET VassalageEnabled = 1 WHERE Type = 'ERA_MEDIEVAL';

-- Specialist Food Costs
UPDATE Eras SET SpecialistFoodCost = 1 WHERE Type = 'ERA_ANCIENT';
UPDATE Eras SET SpecialistFoodCost = 2 WHERE Type = 'ERA_CLASSICAL';
UPDATE Eras SET SpecialistFoodCost = 3 WHERE Type = 'ERA_MEDIEVAL';
UPDATE Eras SET SpecialistFoodCost = 4 WHERE Type = 'ERA_RENAISSANCE';
UPDATE Eras SET SpecialistFoodCost = 5 WHERE Type = 'ERA_INDUSTRIAL';
UPDATE Eras SET SpecialistFoodCost = 6 WHERE Type = 'ERA_MODERN';
UPDATE Eras SET SpecialistFoodCost = 7 WHERE Type = 'ERA_POSTMODERN';
UPDATE Eras SET SpecialistFoodCost = 8 WHERE Type = 'ERA_FUTURE';
