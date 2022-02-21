--==========================================================================================================================
-- IMPROVEMENTS
--==========================================================================================================================
		
INSERT INTO ArtDefine_LandmarkTypes(Type, LandmarkType, FriendlyName)
SELECT 'ART_DEF_IMPROVEMENT_JFD_MACHU', 'Improvement', 'IncaCity';

INSERT INTO ArtDefine_Landmarks(Era, State, Scale, ImprovementType, LayoutHandler, ResourceType, Model, TerrainContour)
SELECT 'Any', 'Constructed', '1',  'ART_DEF_IMPROVEMENT_JFD_MACHU', 'SNAPSHOT', 'ART_DEF_RESOURCE_ALL', 'MachuPicchu.fxsxml', '0.75';

-- Improvements
--------------------------------------------------------------------------------------------------------------------------
INSERT INTO Language_en_US (Tag, Text)
	VALUES ('TXT_KEY_IMPROVEMENT_JFD_MACHU_PICCHU', 'Mountain City');
INSERT OR REPLACE INTO Improvements
		(Type,								Description,							ArtDefineTag,						GraphicalOnly)
SELECT	'IMPROVEMENT_JFD_MACHU_PICCHU',		'TXT_KEY_IMPROVEMENT_JFD_MACHU_PICCHU',	'ART_DEF_IMPROVEMENT_JFD_MACHU',	1;
--==========================================================================================================================
--==========================================================================================================================		