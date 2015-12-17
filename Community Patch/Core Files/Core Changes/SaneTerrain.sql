-- treat mountain as a normal tile which is simply impassable without the proper trait

UPDATE Terrains
SET InfluenceCost = '2'
WHERE Type = 'TERRAIN_HILL';

UPDATE Terrains
SET Movement = '3', Defense = '25', SeeFrom = '2', Impassable = '1', Found = '1', InfluenceCost = '3'
WHERE Type = 'TERRAIN_MOUNTAIN';

INSERT INTO ArtDefine_StrategicView (StrategicViewType, TileType, Asset)
SELECT 'ART_DEF_TERRAIN_MOUNTAIN' , 'Terrain' , 'Sv_TerrainHexGrasslands.dds';

-- treat water tiles as impassable without the proper tech

UPDATE Terrains
SET Impassable = '1', PassableTechTerrain = 'TECH_FISHING'
WHERE Type = 'TERRAIN_COAST';

UPDATE Terrains
SET Impassable = '1', PassableTechTerrain = 'TECH_ASTRONOMY'
WHERE Type = 'TERRAIN_OCEAN';

-- Ice is impassable!

UPDATE Features
SET Impassable = '1'
WHERE Type = 'FEATURE_ICE';

