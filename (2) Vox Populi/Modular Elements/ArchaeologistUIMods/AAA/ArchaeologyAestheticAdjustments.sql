--Update 3D model for Hidden Artifacts to be different than normal ones.
UPDATE ArtDefine_Landmarks
SET Model='hidden_antiquity_site.fxsxml'
WHERE ResourceType='ART_DEF_RESOURCE_HIDDEN_ARTIFACTS' AND ImprovementType='ART_DEF_IMPROVEMENT_NONE';

--Differentiate 3D Art for active dig sites depending on which type of arifacts they are on.
DELETE FROM ArtDefine_Landmarks
WHERE ResourceType = 'ART_DEF_RESOURCE_ALL' AND ImprovementType = 'ART_DEF_IMPROVEMENT_ARCHAEOLOGICAL_DIG';

INSERT INTO ArtDefine_Landmarks(Era, State, Scale, ImprovementType, LayoutHandler, ResourceType, Model, TerrainContour)
SELECT 'Any', 'Any', 0.8, 'ART_DEF_IMPROVEMENT_ARCHAEOLOGICAL_DIG', 'SNAPSHOT', 'ART_DEF_RESOURCE_ARTIFACTS', 'Archaeological_Dig.fxsxml', 1;

INSERT INTO ArtDefine_Landmarks(Era, State, Scale, ImprovementType, LayoutHandler, ResourceType, Model, TerrainContour)
SELECT 'Any', 'Any', 0.8, 'ART_DEF_IMPROVEMENT_ARCHAEOLOGICAL_DIG', 'SNAPSHOT', 'ART_DEF_RESOURCE_HIDDEN_ARTIFACTS', 'Hidden_Archaeological_Dig.fxsxml', 1;