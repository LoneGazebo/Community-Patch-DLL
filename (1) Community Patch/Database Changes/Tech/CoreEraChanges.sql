-- Modifier on warmonger score
UPDATE Eras SET WarmongerPercent = 50 WHERE Type = 'ERA_ANCIENT';
UPDATE Eras SET WarmongerPercent = 60 WHERE Type = 'ERA_CLASSICAL';
UPDATE Eras SET WarmongerPercent = 70 WHERE Type = 'ERA_MEDIEVAL';
UPDATE Eras SET WarmongerPercent = 80 WHERE Type = 'ERA_RENAISSANCE';
UPDATE Eras SET WarmongerPercent = 90 WHERE Type = 'ERA_INDUSTRIAL';
UPDATE Eras SET WarmongerPercent = 100 WHERE Type = 'ERA_MODERN';
UPDATE Eras SET WarmongerPercent = 110 WHERE Type = 'ERA_POSTMODERN';
UPDATE Eras SET WarmongerPercent = 120 WHERE Type = 'ERA_FUTURE';

-- Update the base eras with the correct values
-- Yes, Medieval and Renaissance are correct values, blame the typo to Firaxis
UPDATE Eras SET EraSplashImage = 'ERA_Classical.dds' WHERE Type = 'ERA_CLASSICAL';
UPDATE Eras SET EraSplashImage = 'ERA_Medievel.dds' WHERE Type = 'ERA_MEDIEVAL';
UPDATE Eras SET EraSplashImage = 'ERA_Renissance.dds' WHERE Type = 'ERA_RENAISSANCE';
UPDATE Eras SET EraSplashImage = 'ERA_Industrial.dds' WHERE Type = 'ERA_INDUSTRIAL';
UPDATE Eras SET EraSplashImage = 'ERA_Modern.dds' WHERE Type = 'ERA_MODERN';
UPDATE Eras SET EraSplashImage = 'ERA_Atomic.dds' WHERE Type = 'ERA_POSTMODERN';
UPDATE Eras SET EraSplashImage = 'ERA_Future.dds' WHERE Type = 'ERA_FUTURE';
