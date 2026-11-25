-- Delete the strategic view rows of the heavy resource art (ArtDefineTagHeavy is used for the 3D models only).
-- The strategic view draws a resource with the row of its ArtDefineTag only and has room for 64 resource icons
-- (a row past the limit draws no icon), so these rows only take room from new resources.
DELETE FROM ArtDefine_StrategicView
WHERE TileType = 'Resource' AND StrategicViewType IN (
	'ART_DEF_RESOURCE_ALUMINUM_HEAVY',
	'ART_DEF_RESOURCE_COAL_HEAVY',
	'ART_DEF_RESOURCE_IRON_HEAVY',
	'ART_DEF_RESOURCE_OIL_HEAVY',
	'ART_DEF_RESOURCE_SUPERHORSE',
	'ART_DEF_RESOURCE_URANIUM_HEAVY'
);
