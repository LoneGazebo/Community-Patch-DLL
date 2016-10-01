-- treat mountain as a normal tile which is simply impassable without the proper trait

-- note: +2 will be added to see from and see through values for the plot type. influence cost also handled via plot type
UPDATE Terrains
SET Movement = '3', Defense = '25', Impassable = '1', SeeFrom = '1', SeeThrough = '1', Water = '0', Found = '1'
WHERE Type = 'TERRAIN_MOUNTAIN';

-- yields will only be realized if the plot is passable for the owner

INSERT INTO ArtDefine_StrategicView (StrategicViewType, TileType, Asset)
SELECT 'ART_DEF_TERRAIN_MOUNTAIN' , 'Terrain' , 'Sv_TerrainHexGrasslands.dds';

-- Ice is impassable!

UPDATE Features
SET Impassable = '1'
WHERE Type = 'FEATURE_ICE';

-- Fix Fishing Boats murdering combat units
UPDATE Builds
SET KillOnlyCivilian = '1'
WHERE Type = 'BUILD_FISHING_BOATS';