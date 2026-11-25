-- Delete the strategic view row that no feature uses: each natural wonder has its own art.
-- The strategic view has room for 32 feature icons (a row past the limit draws no icon),
-- so this row only takes room from new features.
DELETE FROM ArtDefine_StrategicView
WHERE TileType = 'Feature' AND StrategicViewType = 'ART_DEF_FEATURE_NATURAL_WONDER';
