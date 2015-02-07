-- Barbarian Healing

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_BARBARIAN_HEAL_RATE', '0'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='BARBARIAN_HEAL' AND Value= 0 );

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_BARBARIAN_HEAL_RATE', '5'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='BARBARIAN_HEAL' AND Value= 1 );

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_BARBARIAN_HEAL_RATE', '10'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='BARBARIAN_HEAL' AND Value= 2 );

-- Barbarian Promotions

UPDATE Features
SET SpawnLocationUnitFreePromotion = 'PROMOTION_MARSH_WALKER'
WHERE Type = 'FEATURE_MARSH';

UPDATE Features
SET SpawnLocationUnitFreePromotion = 'PROMOTION_MARSH_WALKER'
WHERE Type = 'FEATURE_FLOOD_PLAINS';

UPDATE Features
SET IsBarbarianOnly = 'true'
WHERE Type = 'FEATURE_MARSH';

UPDATE Features
SET IsBarbarianOnly = 'true'
WHERE Type = 'FEATURE_FLOOD_PLAINS';

UPDATE Features
SET SpawnLocationUnitFreePromotion = 'PROMOTION_WOODSMAN'
WHERE Type = 'FEATURE_FOREST';

UPDATE Features
SET IsBarbarianOnly = 'true'
WHERE Type = 'FEATURE_FOREST';

UPDATE Features
SET SpawnLocationUnitFreePromotion = 'PROMOTION_WOODSMAN'
WHERE Type = 'FEATURE_JUNGLE';

UPDATE Features
SET IsBarbarianOnly = 'true'
WHERE Type = 'FEATURE_JUNGLE';

UPDATE Terrains
SET SpawnLocationUnitFreePromotion = 'PROMOTION_DESERT_SHIPS'
WHERE Type = 'TERRAIN_DESERT';

UPDATE Terrains
SET IsBarbarianOnly = 'true'
WHERE Type = 'TERRAIN_DESERT';

UPDATE Terrains
SET IsBarbarianOnly = 'true'
WHERE Type = 'TERRAIN_COAST';

UPDATE Terrains
SET AdjacentUnitFreePromotion = 'PROMOTION_EMBARKATION'
WHERE Type = 'TERRAIN_COAST';

UPDATE Terrains
SET IsBarbarianOnly = 'true'
WHERE Type = 'TERRAIN_HILL';

UPDATE Terrains
SET SpawnLocationUnitFreePromotion = 'PROMOTION_ALTITUDE_TRAINING'
WHERE Type = 'TERRAIN_HILL';

UPDATE Terrains
SET SpawnLocationUnitFreePromotion = 'PROMOTION_WHITE_WALKER'
WHERE Type = 'TERRAIN_SNOW';

UPDATE Terrains
SET SpawnLocationUnitFreePromotion = 'PROMOTION_WHITE_WALKER'
WHERE Type = 'TERRAIN_TUNDRA';

UPDATE Terrains
SET IsBarbarianOnly = 'true'
WHERE Type = 'TERRAIN_SNOW';

UPDATE Terrains
SET IsBarbarianOnly = 'true'
WHERE Type = 'TERRAIN_TUNDRA';
