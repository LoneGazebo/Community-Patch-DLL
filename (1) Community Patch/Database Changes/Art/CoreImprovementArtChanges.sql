-- Shrink Citadel
UPDATE ArtDefine_Landmarks
SET Scale = 0.80
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_CITADEL';
