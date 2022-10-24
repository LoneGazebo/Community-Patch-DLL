-- New Resources
INSERT INTO Resources (Type, ResourceClassType,  Hills, Flatlands, MaxLatitude, TechReveal, IconAtlas, PortraitIndex, ArtDefineTag, IconString, Description, Civilopedia,  Help, AITradeModifier, ResourceUsage, AIObjective, PlacementOrder, ConstAppearance, RandApp1, RandApp2, MinAreaSize, IsMonopoly, Normalize) VALUES
('RESOURCE_RICE',  'RESOURCECLASS_BONUS', 0, 1, 50, 'TECH_AGRICULTURE', 'VP_RESOURCE_ATLAS', 13, 'ART_DEF_RESOURCE_RICE', '[ICON_RES_RICE]', 'TXT_KEY_RESOURCE_RICE', 'TXT_KEY_CIV5_RESOURCE_RICE_TEXT', NULL, 0, 0, 0, 4, 50, 25, 25, 3, 0, 1),
('RESOURCE_MAIZE', 'RESOURCECLASS_BONUS', 0, 1, 60, 'TECH_AGRICULTURE', 'VP_RESOURCE_ATLAS', 12, 'ART_DEF_RESOURCE_MAIZE', '[ICON_RES_MAIZE]', 'TXT_KEY_RESOURCE_MAIZE', 'TXT_KEY_CIV5_RESOURCE_MAIZE_TEXT', NULL, 0, 0, 0, 4, 50, 25, 25, 3, 0, 1);

INSERT INTO Resource_FeatureBooleans (ResourceType, FeatureType) VALUES
( 'RESOURCE_RICE',  'FEATURE_MARSH'),
( 'RESOURCE_MAIZE',  'FEATURE_FLOOD_PLAINS');

INSERT INTO Resource_Flavors (ResourceType, FlavorType, Flavor) VALUES
('RESOURCE_MAIZE', 'FLAVOR_GROWTH', 10),
('RESOURCE_RICE', 'FLAVOR_GROWTH', 10);

INSERT INTO Resource_FeatureTerrainBooleans (ResourceType, TerrainType) VALUES
( 'RESOURCE_RICE',  'TERRAIN_GRASS'),
( 'RESOURCE_MAIZE',  'TERRAIN_DESERT');

INSERT INTO Resource_TerrainBooleans (ResourceType, TerrainType) VALUES
( 'RESOURCE_MAIZE',  'TERRAIN_GRASS'),
( 'RESOURCE_MAIZE',  'TERRAIN_PLAINS'),
( 'RESOURCE_RICE',  'TERRAIN_GRASS');

INSERT INTO Resource_YieldChanges (ResourceType, YieldType, Yield) VALUES 
('RESOURCE_MAIZE', 'YIELD_FOOD', 1),
('RESOURCE_RICE', 'YIELD_FOOD', 1);

INSERT INTO Improvement_AdjacentResourceYieldChanges (ImprovementType, ResourceType, YieldType, Yield) VALUES
( 'IMPROVEMENT_SPAIN_HACIENDA', 'RESOURCE_MAIZE', 'YIELD_FOOD', 2),
( 'IMPROVEMENT_SPAIN_HACIENDA', 'RESOURCE_RICE', 'YIELD_FOOD', 2);

INSERT INTO Improvement_ResourceTypes (ImprovementType, ResourceType) VALUES
( 'IMPROVEMENT_FARM', 'RESOURCE_MAIZE'),
( 'IMPROVEMENT_FARM', 'RESOURCE_RICE');

DELETE FROM Building_ResourceYieldChanges WHERE BuildingType = 'BUILDING_GRANARY';

INSERT INTO Building_ResourceYieldChanges (BuildingType, ResourceType, YieldType, Yield) VALUES
( 'BUILDING_GRANARY', 'RESOURCE_WHEAT', 'YIELD_FOOD', 1),
( 'BUILDING_GRANARY', 'RESOURCE_BANANA', 'YIELD_FOOD', 1),
( 'BUILDING_GRANARY', 'RESOURCE_MAIZE', 'YIELD_FOOD', 1),
( 'BUILDING_GRANARY', 'RESOURCE_RICE', 'YIELD_FOOD', 1);

INSERT INTO Improvement_ResourceType_Yields	(ResourceType, ImprovementType, YieldType, Yield) VALUES
('RESOURCE_RICE',	'IMPROVEMENT_FARM',	'YIELD_FOOD',		-1),
('RESOURCE_RICE',	'IMPROVEMENT_FARM',	'YIELD_GOLD',		1),
('RESOURCE_WHEAT',	'IMPROVEMENT_FARM',	'YIELD_FOOD',		-1),
('RESOURCE_WHEAT',	'IMPROVEMENT_FARM',	'YIELD_PRODUCTION',	1);
