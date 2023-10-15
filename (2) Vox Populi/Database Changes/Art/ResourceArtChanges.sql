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
