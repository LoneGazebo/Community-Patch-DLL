--==========================================================================================================================
-- IMPROVEMENTS
--==========================================================================================================================
		
INSERT INTO ArtDefine_LandmarkTypes(Type, LandmarkType, FriendlyName)
SELECT 'ART_DEF_IMPROVEMENT_JFD_MACHU', 'Improvement', 'IncaCity';

INSERT INTO ArtDefine_Landmarks(Era, State, Scale, ImprovementType, LayoutHandler, ResourceType, Model, TerrainContour)
SELECT 'Any', 'Constructed', '1',  'ART_DEF_IMPROVEMENT_JFD_MACHU', 'SNAPSHOT', 'ART_DEF_RESOURCE_ALL', 'MachuPicchu.fxsxml', '0.75';

-- Improvements
--------------------------------------------------------------------------------------------------------------------------
INSERT OR REPLACE INTO Improvements
		(Type,								ArtDefineTag, 						GraphicalOnly)
SELECT	'IMPROVEMENT_JFD_MACHU_PICCHU',		'ART_DEF_IMPROVEMENT_JFD_MACHU',	1
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type = 'COMMUNITY_CORE_BALANCE_LEADERS' AND Value = 1);
--==========================================================================================================================
--==========================================================================================================================		