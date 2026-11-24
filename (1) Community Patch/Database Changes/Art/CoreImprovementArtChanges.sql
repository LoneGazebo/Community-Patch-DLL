-- Shrink Citadel
UPDATE ArtDefine_Landmarks
SET Scale = 0.80
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_CITADEL';

-- Delete the strategic view rows of scenario improvement art that no improvement uses.
-- The strategic view has room for 64 improvement icons (a row past the limit draws no icon),
-- so these rows only take room from new improvements.
DELETE FROM ArtDefine_StrategicView
WHERE TileType = 'Improvement' AND StrategicViewType IN (
	'ART_DEF_FEATURE_HADRIANS_WALL_1',
	'ART_DEF_FEATURE_HADRIANS_WALL_2',
	'ART_DEF_FEATURE_HADRIANS_WALL_3',
	'ART_DEF_IMPROVEMENT_MOTTE_BAILEY',
	'ART_DEF_IMPROVEMENT_WAJO'
);
