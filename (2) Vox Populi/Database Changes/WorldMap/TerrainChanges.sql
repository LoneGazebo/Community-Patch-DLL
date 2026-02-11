UPDATE Terrains
SET Movement = 2
WHERE Type = 'TERRAIN_DESERT';

INSERT INTO Terrain_Yields
	(TerrainType, YieldType, Yield)
VALUES
	('TERRAIN_DESERT', 'YIELD_PRODUCTION', 1);

-- otherwise Desert hills would go to +3
UPDATE Terrain_HillsYieldChanges SET Yield = 1 WHERE TerrainType = 'TERRAIN_DESERT';

UPDATE Terrains
SET Movement = 2
WHERE Type = 'TERRAIN_SNOW';

UPDATE Terrain_Yields
SET Yield = 2
WHERE TerrainType = 'TERRAIN_COAST' AND YieldType = 'YIELD_FOOD';
