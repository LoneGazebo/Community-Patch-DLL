-- treat mountain as a normal tile which is simply impassable without the proper trait

UPDATE Terrains
SET InfluenceCost = '2'
WHERE Type = 'TERRAIN_HILL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE Terrains
SET Movement = '3', Defense = '25', SeeFrom = '2', Impassable = '1', Found = '1', InfluenceCost = '3'
WHERE Type = 'TERRAIN_MOUNTAIN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

INSERT INTO Terrain_Yields (TerrainType, YieldType, Yield)
SELECT 'TERRAIN_MOUNTAIN', 'YIELD_FOOD' , '1'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

INSERT INTO Terrain_Yields (TerrainType, YieldType, Yield)
SELECT 'TERRAIN_MOUNTAIN', 'YIELD_PRODUCTION' , '1'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

-- treat water tiles as impassable without the proper tech

UPDATE Terrains
SET Impassable = '1', PassableTechTerrain = 'TECH_FISHING'
WHERE Type = 'TERRAIN_COAST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE Terrains
SET Impassable = '1', PassableTechTerrain = 'TECH_ASTRONOMY'
WHERE Type = 'TERRAIN_OCEAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );
