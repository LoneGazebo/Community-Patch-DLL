-- Vanilla - National Wonder Building Requirements
INSERT INTO Building_PrereqBuildingClasses (
  BuildingType, BuildingClassType, NumBuildingNeeded)
SELECT 'BUILDING_PRINTING_PRESS', 'BUILDINGCLASS_CHANCERY', '-1'
WHERE NOT EXISTS (SELECT * FROM sqlite_master WHERE name='Building_PrereqBuildingClassesPercentage');

INSERT INTO Building_PrereqBuildingClasses (
  BuildingType, BuildingClassType, NumBuildingNeeded)
SELECT 'BUILDING_FOREIGN_OFFICE', 'BUILDINGCLASS_WIRE_SERVICE', '-1'
WHERE NOT EXISTS (SELECT * FROM sqlite_master WHERE name='Building_PrereqBuildingClassesPercentage');

--Halicarnassus Fix
UPDATE Language_en_US
SET Text = 'Empire enters a [ICON_GOLDEN_AGE] Golden Age. Gain 20 [ICON_GOLD] Gold each time a Great Person is expended. Each source of [ICON_RES_MARBLE] Marble or [ICON_RES_STONE] Stone worked by this City produces +2 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_WONDER_MAUSOLEUM_HALICARNASSUS_HELP'
AND EXISTS (SELECT * FROM CSD WHERE Type='ANCIENT_WONDERS' AND Value= 1 );

UPDATE Buildings
SET GreatPersonExpendGold = 20
WHERE BuildingClass = 'BUILDINGCLASS_MAUSOLEUM_HALICARNASSUS'
AND EXISTS (SELECT * FROM CSD WHERE Type='ANCIENT_WONDERS' AND Value= 1 );

UPDATE Buildings
SET GoldenAge = 'true'
WHERE BuildingClass= 'BUILDINGCLASS_MAUSOLEUM_HALICARNASSUS' 
AND EXISTS (SELECT * FROM CSD WHERE Type='ANCIENT_WONDERS' AND Value= 1 );

------------------------------	
-- Building_ImprovementYieldChangesGlobal
------------------------------		
INSERT INTO Building_ImprovementYieldChangesGlobal	
			(BuildingType, 			ImprovementType,		YieldType,					Yield)
VALUES		('BUILDING_MENIN_GATE',	'IMPROVEMENT_FORT',		'YIELD_CULTURE',			2),
			('BUILDING_MENIN_GATE',	'IMPROVEMENT_FORT',		'YIELD_GOLDEN_AGE_POINTS',	2),
			('BUILDING_MENIN_GATE',	'IMPROVEMENT_LANDMARK',	'YIELD_CULTURE',			2),
			('BUILDING_MENIN_GATE',	'IMPROVEMENT_LANDMARK',	'YIELD_GOLDEN_AGE_POINTS',	2),
			('BUILDING_MENIN_GATE',	'IMPROVEMENT_CITADEL',	'YIELD_CULTURE',			2),
			('BUILDING_MENIN_GATE',	'IMPROVEMENT_CITADEL',	'YIELD_GOLDEN_AGE_POINTS',	2);
------------------------------	
-- Building_YieldFromDeath
------------------------------		
INSERT INTO Building_YieldFromDeath 	
			(BuildingType, 			YieldType,			Yield)
VALUES		('BUILDING_MENIN_GATE',	'YIELD_CULTURE',	2);
--==========================================================================================================================
-- ARTDEFINES
--==========================================================================================================================	
--------------------------------
-- IconTextureAtlases
--------------------------------
INSERT INTO IconTextureAtlases 
		(Atlas, 				IconSize, 	Filename, 				IconsPerRow, 	IconsPerColumn)
VALUES	('ATLAS_MENIN_GATE', 	256, 		'MeninGate_256.dds',	1, 				1),
		('ATLAS_MENIN_GATE', 	128, 		'MeninGate_128.dds',	1, 				1),
		('ATLAS_MENIN_GATE', 	80, 		'MeninGate_080.dds',	1, 				1),
		('ATLAS_MENIN_GATE', 	64, 		'MeninGate_064.dds',	1, 				1),
		('ATLAS_MENIN_GATE', 	45, 		'MeninGate_045.dds',	1, 				1);
