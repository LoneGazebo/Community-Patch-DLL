-- Uranium: Mine -> Quarry
UPDATE ArtDefine_Landmarks
SET ImprovementType = 'ART_DEF_IMPROVEMENT_QUARRY'
WHERE ResourceType IN (
	'ART_DEF_RESOURCE_URANIUM',
	'ART_DEF_RESOURCE_URANIUM_HEAVY',
	'ART_DEF_RESOURCE_URANIUM_HEAVY_MARSH',
	'ART_DEF_RESOURCE_URANIUM_MARSH'
) AND State <> 'Any';

-- Update 3D model for Hidden Artifacts to be different from normal ones
UPDATE ArtDefine_Landmarks
SET Model = 'hidden_antiquity_site.fxsxml'
WHERE ResourceType = 'ART_DEF_RESOURCE_HIDDEN_ARTIFACTS';

-- Sugar uses new art (ART_DEF_RESOURCE_SUGAR_ALT), so the strategic view row of its old art is never drawn.
-- Delete it to leave its icon room to new resources (see CoreResourceArtChanges.sql in Community Patch).
DELETE FROM ArtDefine_StrategicView
WHERE TileType = 'Resource' AND StrategicViewType = 'ART_DEF_RESOURCE_SUGAR';
