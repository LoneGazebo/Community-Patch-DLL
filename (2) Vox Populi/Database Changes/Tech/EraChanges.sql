-- All civs start with an extra Pathfinder
UPDATE Eras SET StartingExploreUnits = 1 WHERE Type = 'ERA_ANCIENT';

-- Enable Vassalage
UPDATE Eras SET VassalageEnabled = 1 WHERE Type = 'ERA_MEDIEVAL';
