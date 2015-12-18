-- treat mountain as a normal tile which is simply impassable without the proper trait

-- note: see from and see through values are handled via plot type, as well as influence cost
UPDATE Terrains
SET Movement = '3', Defense = '25', Impassable = '1', Water = 'false', Found = '1'
WHERE Type = 'TERRAIN_MOUNTAIN';

-- yields will only be realized if the plot is passable for the owner

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

