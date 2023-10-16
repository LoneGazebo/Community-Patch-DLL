UPDATE Terrains
SET Movement = 2
WHERE Type = 'TERRAIN_DESERT';

UPDATE Terrains
SET Movement = 2
WHERE Type = 'TERRAIN_SNOW';

UPDATE Terrain_Yields
SET Yield = 2
WHERE TerrainType = 'TERRAIN_COAST' AND YieldType = 'YIELD_FOOD';
