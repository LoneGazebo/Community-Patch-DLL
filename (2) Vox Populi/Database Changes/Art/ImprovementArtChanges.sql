-- Differentiate 3D models for active dig sites depending on which type of arifacts they are on
-- (Inserted in NewImprovementArts.xml)
DELETE FROM ArtDefine_Landmarks
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_ARCHAEOLOGICAL_DIG'
AND ResourceType = 'ART_DEF_RESOURCE_ALL';

-- Shrink Village
UPDATE ArtDefine_Landmarks
SET Scale = 0.65
WHERE ImprovementType IN (
	'ART_DEF_IMPROVEMENT_TRADING_POST1',
	'ART_DEF_IMPROVEMENT_TRADING_POST2',
	'ART_DEF_IMPROVEMENT_TRADING_POST3',
	'ART_DEF_IMPROVEMENT_TRADING_POST4'
);

-- Shrink Academy, Manufactory, Holy Site
UPDATE ArtDefine_Landmarks
SET Scale = 0.75
WHERE ImprovementType IN (
	'ART_DEF_IMPROVEMENT_ACADEMY',
	'ART_DEF_IMPROVEMENT_MANUFACTORY',
	'ART_DEF_IMPROVEMENT_HOLY_SITE'
);

-- Shrink Town
UPDATE ArtDefine_Landmarks
SET Scale = 0.70
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_CUSTOMS_HOUSE';

-- Shrink Chateau
UPDATE ArtDefine_Landmarks
SET Scale = 0.70
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_CHATEAU';

-- Shrink Kasbah
UPDATE ArtDefine_Landmarks
SET Scale = 0.80
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_KASBAH';
