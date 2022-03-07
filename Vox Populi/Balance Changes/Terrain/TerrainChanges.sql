-- Terrain 
UPDATE Terrain_Yields
SET Yield = '2'
WHERE TerrainType = 'TERRAIN_COAST' AND YieldType= 'YIELD_FOOD';

UPDATE Terrain_Yields
SET Yield = '1'
WHERE TerrainType = 'TERRAIN_MOUNTAIN' AND YieldType= 'YIELD_FOOD';

UPDATE Terrain_Yields
SET Yield = '1'
WHERE TerrainType = 'TERRAIN_MOUNTAIN' AND YieldType= 'YIELD_PRODUCTION';

UPDATE Terrains
SET Movement = '2'
WHERE Type = 'TERRAIN_DESERT';

UPDATE Terrains
SET Movement = '2'
WHERE Type = 'TERRAIN_SNOW';

-- New City Yields Method
UPDATE Yields
SET MinCityFlatFreshWater = 1, MinCityMountainFreshWater = 1
WHERE Type = 'YIELD_FOOD';

UPDATE Yields
SET MinCityFlatNoFreshWater = 1, MinCityMountainNoFreshWater = 1
WHERE Type = 'YIELD_GOLD';

UPDATE Yields
SET MinCityMountainNoFreshWater = 1
WHERE Type = 'YIELD_PRODUCTION';

--MinCityHillNoFreshWater = 1, MinCityHillFreshWater = 1


