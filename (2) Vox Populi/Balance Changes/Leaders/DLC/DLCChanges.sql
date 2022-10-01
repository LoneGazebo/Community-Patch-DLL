-- Babylon

UPDATE Traits
SET InvestmentModifier = '-15'
WHERE Type = 'TRAIT_INGENIOUS';

-- Babylon 

UPDATE Buildings
SET CityRangedStrikeRange = '1'
WHERE Type = 'BUILDING_WALLS_OF_BABYLON';

UPDATE Buildings
SET SpecialistType = 'SPECIALIST_SCIENTIST'
WHERE Type = 'BUILDING_WALLS_OF_BABYLON';

UPDATE Buildings
SET SpecialistCount = '1'
WHERE Type = 'BUILDING_WALLS_OF_BABYLON';

-- Mongols
UPDATE Traits
SET BullyAnnex = '0'
WHERE Type = 'TRAIT_TERROR';

UPDATE Traits
SET IgnoreBullyPenalties = '0'
WHERE Type = 'TRAIT_TERROR';

UPDATE Traits
SET CSBullyValueModifier = '50'
WHERE Type = 'TRAIT_TERROR';

UPDATE Traits
SET BullyYieldMultiplierAnnex = '0'
WHERE Type = 'TRAIT_TERROR';

UPDATE Traits
SET CityStateCombatModifier = '0'
WHERE Type = 'TRAIT_TERROR';

DELETE FROM Trait_MovesChangeUnitCombats
WHERE TraitType = 'TRAIT_TERROR';

INSERT INTO ArtDefine_LandmarkTypes
			(Type, 									LandmarkType, 	FriendlyName)
VALUES 		('ART_DEF_IMPROVEMENT_MONGOLIA_ORDO', 	'Improvement', 	'MONGOLIAN_ORDO');

INSERT INTO ArtDefine_Landmarks
		(Era, 	State, 					Scale, ImprovementType, 					LayoutHandler, 	ResourceType, 			Model, 					TerrainContour)
VALUES 	('Any', 'UnderConstruction', 	0.57,  'ART_DEF_IMPROVEMENT_MONGOLIA_ORDO', 	'SNAPSHOT', 	'ART_DEF_RESOURCE_ALL', 'YurtCamp.fxsxml', 	1),
		('Any', 'Constructed', 			0.57,  'ART_DEF_IMPROVEMENT_MONGOLIA_ORDO', 	'SNAPSHOT', 	'ART_DEF_RESOURCE_ALL', 'YurtCamp.fxsxml', 	1),
		('Any', 'Pillaged', 			0.57,  'ART_DEF_IMPROVEMENT_MONGOLIA_ORDO', 	'SNAPSHOT', 	'ART_DEF_RESOURCE_ALL', 'YurtCamp_PL.fxsxml', 	1);

INSERT INTO ArtDefine_StrategicView
			(StrategicViewType, 					TileType, 		Asset)
VALUES 		('ART_DEF_IMPROVEMENT_MONGOLIA_ORDO', 	'Improvement', 	'Yurt_SV.dds');

INSERT INTO IconTextureAtlases 
			(Atlas, 					IconSize, 	Filename, 				IconsPerRow, 	IconsPerColumn)
VALUES		('MONGOLIA_ORDO_ATLAS', 	256, 		'YurtIcons256.dds',		2, 				1),
			('MONGOLIA_ORDO_ATLAS', 	64, 		'YurtIcons064.dds',		2, 				1),
			('MONGOLIA_ORDO_ATLAS', 	45, 		'YurtIcons045.dds',		2, 				1);

INSERT INTO Improvements
			(Type, 							Description, 							Civilopedia, 								ArtDefineTag, 							Help, 										CultureBombRadius, 	MovesChange, 	MakesPassable, 	InAdjacentFriendly, PillageGold,	CreatedByGreatPerson,	BuildableOnResources,	SpecificCivRequired,	Permanent,	DestroyedWhenPillaged, 	PortraitIndex, 	IconAtlas, 				NoTwoAdjacent,	DefenseModifier, NoFollowUp)
VALUES		('IMPROVEMENT_MONGOLIA_ORDO',	'TXT_KEY_IMPROVEMENT_MONGOLIA_ORDO',	'TXT_KEY_IMPROVEMENT_MONGOLIA_ORDO_TEXT',	'ART_DEF_IMPROVEMENT_MONGOLIA_ORDO',	'TXT_KEY_IMPROVEMENT_MONGOLIA_ORDO_HELP',	1, 					1, 				1, 				1, 					17,				1,						1,						0, 						0,			0,						0,				'MONGOLIA_ORDO_ATLAS',	1,				50, 			1);

INSERT INTO Improvement_Flavors	
			(ImprovementType, 				FlavorType,				Flavor)
VALUES		('IMPROVEMENT_MONGOLIA_ORDO',	'FLAVOR_CULTURE',		60),
			('IMPROVEMENT_MONGOLIA_ORDO',	'FLAVOR_PRODUCTION',	60),
			('IMPROVEMENT_MONGOLIA_ORDO',	'FLAVOR_CITY_DEFENSE',	20),
			('IMPROVEMENT_MONGOLIA_ORDO',	'FLAVOR_DEFENSE',		50);

INSERT INTO Improvement_Yields 	
			(ImprovementType, 				YieldType,				Yield)
VALUES		('IMPROVEMENT_MONGOLIA_ORDO',	'YIELD_FOOD',			1),
			('IMPROVEMENT_MONGOLIA_ORDO',	'YIELD_GOLD',			1),
			('IMPROVEMENT_MONGOLIA_ORDO',	'YIELD_CULTURE',		1),
			('IMPROVEMENT_MONGOLIA_ORDO',	'YIELD_PRODUCTION',		1);

INSERT INTO Improvement_ResourceTypes	
			(ImprovementType, 				ResourceType,			ResourceMakesValid)
VALUES		('IMPROVEMENT_MONGOLIA_ORDO',	'RESOURCE_HORSE',		1),
			('IMPROVEMENT_MONGOLIA_ORDO',	'RESOURCE_IRON',		1),
			('IMPROVEMENT_MONGOLIA_ORDO',	'RESOURCE_COAL',		1),
			('IMPROVEMENT_MONGOLIA_ORDO',	'RESOURCE_OIL',			1),
			('IMPROVEMENT_MONGOLIA_ORDO',	'RESOURCE_ALUMINUM',	1),
			('IMPROVEMENT_MONGOLIA_ORDO',	'RESOURCE_URANIUM',		1);

INSERT INTO Improvement_TechYieldChanges
			(ImprovementType,				TechType,					YieldType,			Yield)
VALUES		('IMPROVEMENT_MONGOLIA_ORDO', 	'TECH_CHEMISTRY', 			'YIELD_CULTURE', 	2),
			('IMPROVEMENT_MONGOLIA_ORDO', 	'TECH_MILITARY_SCIENCE', 	'YIELD_PRODUCTION', 2),
			('IMPROVEMENT_MONGOLIA_ORDO', 	'TECH_STEALTH', 			'YIELD_CULTURE', 	4),
			('IMPROVEMENT_MONGOLIA_ORDO', 	'TECH_ELECTRONICS', 		'YIELD_PRODUCTION', 4);

INSERT INTO Improvement_ValidTerrains 	
			(ImprovementType, 				TerrainType)
VALUES		('IMPROVEMENT_MONGOLIA_ORDO',	'TERRAIN_PLAINS'),
			('IMPROVEMENT_MONGOLIA_ORDO',	'TERRAIN_GRASS'),
			('IMPROVEMENT_MONGOLIA_ORDO',	'TERRAIN_TUNDRA'),
			('IMPROVEMENT_MONGOLIA_ORDO',	'TERRAIN_DESERT'),			
			('IMPROVEMENT_MONGOLIA_ORDO',	'TERRAIN_SNOW');			

INSERT INTO Builds		
			(Type,					PrereqTech,		ImprovementType, 				Description, 					Help, 								Recommendation, 					EntityEvent, 			OrderPriority, 	Kill, 	IconIndex, 	IconAtlas)
VALUES		('BUILD_MONGOLIA_ORDO',	null,			'IMPROVEMENT_MONGOLIA_ORDO',	'TXT_KEY_BUILD_MONGOLIA_ORDO',	'TXT_KEY_BUILD_MONGOLIA_ORDO_HELP',	'TXT_KEY_BUILD_MONGOLIA_ORDO_REC',	'ENTITY_EVENT_MINE',	95,				1,		1,			'MONGOLIA_ORDO_ATLAS');

DELETE FROM Unit_Builds WHERE UnitType = 'UNIT_MONGOLIAN_KHAN' AND BuildType = 'BUILD_CITADEL';
			
INSERT INTO Unit_Builds	
			(UnitType, 				BuildType)
VALUES		('UNIT_MONGOLIAN_KHAN',	'BUILD_MONGOLIA_ORDO');

INSERT INTO BuildFeatures	
			(BuildType, 			FeatureType, PrereqTech, Production, Remove)
VALUES		('BUILD_MONGOLIA_ORDO',	'FEATURE_JUNGLE', 'TECH_CALENDAR', 40, 1),
			('BUILD_MONGOLIA_ORDO',	'FEATURE_FOREST', 'TECH_MINING', 40, 1),
			('BUILD_MONGOLIA_ORDO',	'FEATURE_MARSH', 'TECH_IRON_WORKING', 40, 1);

-- Polynesia -- Boost Range of Moai
UPDATE Traits
Set NearbyImprovementBonusRange = '3'
WHERE Type = 'TRAIT_WAYFINDING';

UPDATE Traits
Set NearbyImprovementCombatBonus = '20'
WHERE Type = 'TRAIT_WAYFINDING';

UPDATE Builds
Set PrereqTech = 'TECH_MASONRY'
WHERE Type = 'BUILD_MOAI';

UPDATE Improvements
SET CultureAdjacentSameType = '0'
WHERE Type = 'IMPROVEMENT_MOAI';


UPDATE Traits
SET NoConnectionUnhappiness = '1'
WHERE Type = 'TRAIT_WAYFINDING';

-- Spain

--UPDATE Traits
--Set Reconquista = '1'
--WHERE Type = 'TRAIT_SEVEN_CITIES';

--UPDATE Traits
--Set NoSpread = '1'
--WHERE Type = 'TRAIT_SEVEN_CITIES';

UPDATE Traits
SET FreeUnitOnConquest = 'UNIT_SPAIN_INQUISITOR' 
WHERE Type = 'TRAIT_SEVEN_CITIES';

UPDATE Traits
Set CanPurchaseNavalUnitsFaith = '1'
WHERE Type = 'TRAIT_SEVEN_CITIES';

UPDATE Traits
Set NaturalWonderFirstFinderGold = '0'
WHERE Type = 'TRAIT_SEVEN_CITIES';

UPDATE Traits
Set NaturalWonderSubsequentFinderGold = '0'
WHERE Type = 'TRAIT_SEVEN_CITIES';

UPDATE Traits
Set NaturalWonderYieldModifier = '0'
WHERE Type = 'TRAIT_SEVEN_CITIES';

UPDATE Traits
Set NaturalWonderHappinessModifier = '0'
WHERE Type = 'TRAIT_SEVEN_CITIES';

-- Spain Inquisitor
INSERT INTO Civilization_UnitClassOverrides 
			(CivilizationType, 		UnitClassType, 				UnitType)
VALUES		('CIVILIZATION_SPAIN',	'UNITCLASS_INQUISITOR', 	'UNIT_SPAIN_INQUISITOR');

INSERT INTO Units 	
			(Type,						RequiresEnhancedReligion, 	ReligiousStrength, ShowInPedia, NoMaintenance, 	ProhibitsSpread, RemoveHeresy, Class, BaseSightRange, Combat, Cost, FaithCost,	RequiresFaithPurchaseEnabled, Moves, CombatClass, Domain, DefaultUnitAI, ObsoleteTech, GoodyHutUpgradeUnitClass, XPValueAttack,	Description, Civilopedia, Strategy, Help, Pillage, MilitarySupport, MilitaryProduction, IgnoreBuildingDefense, CivilianAttackPriority, Mechanized, AirUnitCap, AdvancedStartCost, RangedCombatLimit, CombatLimit, XPValueDefense, UnitArtInfo, UnitFlagIconOffset, UnitFlagAtlas, PortraitIndex, IconAtlas, MoveRate, PurchaseCooldown)
SELECT		'UNIT_SPAIN_INQUISITOR',	0, 							ReligiousStrength, 0, 			1, 				ProhibitsSpread, RemoveHeresy, Class, BaseSightRange, Combat, Cost, FaithCost,	RequiresFaithPurchaseEnabled, Moves, CombatClass, Domain, DefaultUnitAI, ObsoleteTech, GoodyHutUpgradeUnitClass, XPValueAttack,	Description, Civilopedia, Strategy,	Help, Pillage, MilitarySupport, MilitaryProduction, IgnoreBuildingDefense, CivilianAttackPriority, Mechanized, AirUnitCap, AdvancedStartCost, RangedCombatLimit, CombatLimit, XPValueDefense, UnitArtInfo, UnitFlagIconOffset, UnitFlagAtlas, PortraitIndex, IconAtlas, MoveRate, PurchaseCooldown
FROM Units WHERE Type = 'UNIT_INQUISITOR';

INSERT INTO UnitGameplay2DScripts 	
			(UnitType, 					SelectionSound, FirstSelectionSound)
SELECT		'UNIT_SPAIN_INQUISITOR',	SelectionSound, FirstSelectionSound
FROM UnitGameplay2DScripts WHERE UnitType = 'UNIT_INQUISITOR';	

INSERT INTO Unit_AITypes 	
			(UnitType, 					UnitAIType)
SELECT		'UNIT_SPAIN_INQUISITOR',	UnitAIType
FROM Unit_AITypes WHERE UnitType = 'UNIT_INQUISITOR';

-- Hacienda
INSERT INTO ArtDefine_LandmarkTypes
			(Type, 									LandmarkType, 	FriendlyName)
VALUES 		('ART_DEF_IMPROVEMENT_SPAIN_HACIENDA', 	'Improvement', 	'SPAIN_HACIENDA');

INSERT INTO ArtDefine_Landmarks
			(Era, 	State, 					Scale, 	ImprovementType, 						LayoutHandler, 	ResourceType, 			Model, 					TerrainContour)
VALUES 		('Any', 'UnderConstruction', 	0.8,	'ART_DEF_IMPROVEMENT_SPAIN_HACIENDA', 	'SNAPSHOT', 	'ART_DEF_RESOURCE_ALL', 'hacienda_hb.fxsxml',	1),
			('Any', 'Constructed', 			0.8,	'ART_DEF_IMPROVEMENT_SPAIN_HACIENDA', 	'SNAPSHOT', 	'ART_DEF_RESOURCE_ALL', 'hacienda.fxsxml',		1),
			('Any', 'Pillaged', 			0.8,	'ART_DEF_IMPROVEMENT_SPAIN_HACIENDA', 	'SNAPSHOT', 	'ART_DEF_RESOURCE_ALL', 'hacienda_pl.fxsxml',	1);

INSERT INTO ArtDefine_StrategicView
			(StrategicViewType, 					TileType, 		Asset)
VALUES 		('ART_DEF_IMPROVEMENT_SPAIN_HACIENDA', 	'Improvement', 	'HaciendaIcons_128.dds');

INSERT INTO IconTextureAtlases 
		(Atlas, 								IconSize, 	Filename, 							IconsPerRow, 	IconsPerColumn)
VALUES	('IMPROVEMENT_SPAIN_ATLAS', 			256, 		'HaciendaIcons_256.dds',			2, 				1),
		('IMPROVEMENT_SPAIN_ATLAS', 			64, 		'HaciendaIcons_064.dds',			2, 				1),
		('IMPROVEMENT_SPAIN_ATLAS', 			45, 		'HaciendaIcons_045.dds',			2, 				1);

INSERT INTO Improvements
			(Type, 							Description, 							Civilopedia, 								ArtDefineTag, 							Help, 										SpecificCivRequired, 	CivilizationType, 		PillageGold,	CreatedByGreatPerson,	BuildableOnResources,	DestroyedWhenPillaged, 	PortraitIndex, 	IconAtlas, 					NoTwoAdjacent,	DefenseModifier,	AdjacentLuxury)
VALUES		('IMPROVEMENT_SPAIN_HACIENDA',	'TXT_KEY_IMPROVEMENT_SPAIN_HACIENDA',	'TXT_KEY_IMPROVEMENT_SPAIN_HACIENDA_TEXT',	'ART_DEF_IMPROVEMENT_SPAIN_HACIENDA',	'TXT_KEY_IMPROVEMENT_SPAIN_HACIENDA_HELP',	1,						'CIVILIZATION_SPAIN',	20,				0,						0,						0,						0,				'IMPROVEMENT_SPAIN_ATLAS',	1,				0,					0);
		
INSERT INTO Improvement_Flavors	
			(ImprovementType, 				FlavorType,				Flavor)
VALUES		('IMPROVEMENT_SPAIN_HACIENDA',	'FLAVOR_GOLD',			80),
			('IMPROVEMENT_SPAIN_HACIENDA',	'FLAVOR_PRODUCTION',	70),
			('IMPROVEMENT_SPAIN_HACIENDA',	'FLAVOR_RELIGION',		5),
			('IMPROVEMENT_SPAIN_HACIENDA',	'FLAVOR_GROWTH',		15);
	
INSERT INTO Improvement_ValidTerrains 	
			(ImprovementType, 				TerrainType)
VALUES		('IMPROVEMENT_SPAIN_HACIENDA',	'TERRAIN_PLAINS'),
			('IMPROVEMENT_SPAIN_HACIENDA',	'TERRAIN_GRASS'),
			('IMPROVEMENT_SPAIN_HACIENDA',	'TERRAIN_TUNDRA'),
			('IMPROVEMENT_SPAIN_HACIENDA',	'TERRAIN_DESERT'),			
			('IMPROVEMENT_SPAIN_HACIENDA',	'TERRAIN_SNOW');
	
INSERT INTO Improvement_Yields 	
			(ImprovementType, 				YieldType,			Yield)
VALUES		('IMPROVEMENT_SPAIN_HACIENDA',	'YIELD_GOLD',		1),
			('IMPROVEMENT_SPAIN_HACIENDA',	'YIELD_FOOD',		1),
			('IMPROVEMENT_SPAIN_HACIENDA',	'YIELD_PRODUCTION',	1);
	
INSERT INTO Improvement_AdjacentCityYields 	
			(ImprovementType, 				YieldType,				Yield)
VALUES		('IMPROVEMENT_SPAIN_HACIENDA',	'YIELD_CULTURE',	2);
	
INSERT INTO Improvement_AdjacentResourceYieldChanges	
			(ImprovementType,				ResourceType,	YieldType,		Yield)
SELECT		'IMPROVEMENT_SPAIN_HACIENDA',	Type,			'YIELD_GOLD',	2
FROM Resources WHERE ResourceClassType = 'RESOURCECLASS_LUXURY';

CREATE TRIGGER VP_HaciendaCompatibility_Luxury
AFTER INSERT ON Resources 
WHEN NEW.ResourceClassType = 'RESOURCECLASS_LUXURY'
BEGIN
	INSERT INTO Improvement_AdjacentResourceYieldChanges
				(ImprovementType, ResourceType, YieldType, Yield)
	SELECT		'IMPROVEMENT_SPAIN_HACIENDA', NEW.Type, 'YIELD_GOLD', 2;
END;

INSERT INTO Improvement_AdjacentResourceYieldChanges	
			(ImprovementType,				ResourceType,	YieldType,		Yield)
SELECT		'IMPROVEMENT_SPAIN_HACIENDA',	Type,			'YIELD_FOOD',	2
FROM Resources WHERE ResourceClassType = 'RESOURCECLASS_BONUS';

CREATE TRIGGER VP_HaciendaCompatibility_Bonus
AFTER INSERT ON Resources 
WHEN NEW.ResourceClassType = 'RESOURCECLASS_BONUS'
BEGIN
	INSERT INTO Improvement_AdjacentResourceYieldChanges
				(ImprovementType, ResourceType, YieldType, Yield)
	SELECT		'IMPROVEMENT_SPAIN_HACIENDA', NEW.Type, 'YIELD_FOOD', 2;
END;

INSERT INTO Improvement_AdjacentResourceYieldChanges	
			(ImprovementType,				ResourceType,	YieldType,			Yield)
SELECT		'IMPROVEMENT_SPAIN_HACIENDA',	Type,			'YIELD_PRODUCTION',	2
FROM Resources WHERE ResourceClassType = 'RESOURCECLASS_MODERN';

INSERT INTO Improvement_AdjacentResourceYieldChanges	
			(ImprovementType, 				ResourceType,			YieldType,				Yield)
VALUES		('IMPROVEMENT_SPAIN_HACIENDA',	'RESOURCE_HORSE',		'YIELD_PRODUCTION',		2),
			('IMPROVEMENT_SPAIN_HACIENDA',	'RESOURCE_IRON',		'YIELD_PRODUCTION',		2);

CREATE TRIGGER VP_HaciendaCompatibility_Strategic
AFTER INSERT ON Resources 
WHEN NEW.ResourceClassType IN('RESOURCECLASS_MODERN', 'RESOURCECLASS_RUSH')
BEGIN
	INSERT INTO Improvement_AdjacentResourceYieldChanges
				(ImprovementType, ResourceType, YieldType, Yield)
	SELECT		'IMPROVEMENT_SPAIN_HACIENDA', NEW.Type, 'YIELD_PRODUCTION', 2;
END;

INSERT INTO Improvement_TechYieldChanges
			(ImprovementType,				TechType,					YieldType,			Yield)
VALUES		('IMPROVEMENT_SPAIN_HACIENDA',	'TECH_ARCHITECTURE',		'YIELD_CULTURE',		1),
			('IMPROVEMENT_SPAIN_HACIENDA',	'TECH_FERTILIZER',			'YIELD_FOOD',		1),
			('IMPROVEMENT_SPAIN_HACIENDA',	'TECH_FERTILIZER',			'YIELD_PRODUCTION',	1),
			('IMPROVEMENT_SPAIN_HACIENDA',	'TECH_FERTILIZER',			'YIELD_GOLD',		1);
	
INSERT INTO Builds
			(Type,						PrereqTech,		ImprovementType, 				Description, 					Help, 									Recommendation, 					EntityEvent, 			Time,	OrderPriority, 	Kill, 	IconIndex, 	IconAtlas)
VALUES		('BUILD_SPAIN_HACIENDA',	'TECH_MACHINERY',	'IMPROVEMENT_SPAIN_HACIENDA',	'TXT_KEY_BUILD_SPAIN_HACIENDA',	'TXT_KEY_BUILD_SPAIN_HACIENDA_HELP',	'TXT_KEY_BUILD_SPAIN_HACIENDA_REC',	'ENTITY_EVENT_BUILD',	800,	95,				0,		1,			'IMPROVEMENT_SPAIN_ATLAS');
	
INSERT INTO Unit_Builds	
			(UnitType, 			BuildType)
VALUES		('UNIT_WORKER',		'BUILD_SPAIN_HACIENDA');

INSERT INTO BuildFeatures	
			(BuildType, 			FeatureType, PrereqTech, Time, Production, Remove)
SELECT		'BUILD_SPAIN_HACIENDA',	FeatureType, PrereqTech, Time, Production, Remove
FROM BuildFeatures WHERE BuildType = 'BUILD_CHATEAU';

-- Inca
UPDATE Traits
Set ImprovementMaintenanceModifier = '0'
WHERE Type = 'TRAIT_GREAT_ANDEAN_ROAD';

UPDATE Traits
Set NoHillsImprovementMaintenance = '0'
WHERE Type = 'TRAIT_GREAT_ANDEAN_ROAD';

UPDATE Traits
Set MountainPass = '1'
WHERE Type = 'TRAIT_GREAT_ANDEAN_ROAD';

-- Denmark -- Unique National Epic (Jelling Stones) -- Replace Ski Infantry

-- Korea -- Replace Turtle Ship with UB, adjust UA
UPDATE Trait_SpecialistYieldChanges
Set Yield = '1'
WHERE TraitType = 'TRAIT_SCHOLARS_JADE_HALL';

UPDATE Traits
Set TechBoostFromCapitalScienceBuildings = '0'
WHERE Type = 'TRAIT_SCHOLARS_JADE_HALL';

UPDATE Traits
Set IsOddEraScaler = '1'
WHERE Type = 'TRAIT_SCHOLARS_JADE_HALL';

UPDATE Traits
SET GoldenAgeGreatArtistRateModifier = '30'
WHERE Type = 'TRAIT_SCHOLARS_JADE_HALL';

UPDATE Traits
SET GoldenAgeGreatMusicianRateModifier = '30'
WHERE Type = 'TRAIT_SCHOLARS_JADE_HALL';

UPDATE Traits
SET GoldenAgeGreatWriterRateModifier = '30'
WHERE Type = 'TRAIT_SCHOLARS_JADE_HALL';

INSERT INTO Trait_GoldenAgeGreatPersonRateModifier
	(TraitType, GreatPersonType, Modifier)
VALUES
	('TRAIT_SCHOLARS_JADE_HALL', 'GREATPERSON_ENGINEER', 30),
	('TRAIT_SCHOLARS_JADE_HALL', 'GREATPERSON_SCIENTIST', 30),
	('TRAIT_SCHOLARS_JADE_HALL', 'GREATPERSON_MERCHANT', 30);

INSERT INTO Trait_GreatPersonBornYield
	(TraitType, GreatPersonType, YieldType, Yield)
VALUES
	('TRAIT_SCHOLARS_JADE_HALL', 'GREATPERSON_SCIENTIST', 'YIELD_GOLDEN_AGE_POINTS', 50),
	('TRAIT_SCHOLARS_JADE_HALL', 'GREATPERSON_ENGINEER', 'YIELD_GOLDEN_AGE_POINTS', 50),
	('TRAIT_SCHOLARS_JADE_HALL', 'GREATPERSON_PROPHET', 'YIELD_GOLDEN_AGE_POINTS', 50),
	('TRAIT_SCHOLARS_JADE_HALL', 'GREATPERSON_GENERAL', 'YIELD_GOLDEN_AGE_POINTS', 50),
	('TRAIT_SCHOLARS_JADE_HALL', 'GREATPERSON_ADMIRAL', 'YIELD_GOLDEN_AGE_POINTS', 50),
	('TRAIT_SCHOLARS_JADE_HALL', 'GREATPERSON_MERCHANT', 'YIELD_GOLDEN_AGE_POINTS', 50),
	('TRAIT_SCHOLARS_JADE_HALL', 'GREATPERSON_ARTIST', 'YIELD_GOLDEN_AGE_POINTS', 50),
	('TRAIT_SCHOLARS_JADE_HALL', 'GREATPERSON_MUSICIAN', 'YIELD_GOLDEN_AGE_POINTS', 50),
	('TRAIT_SCHOLARS_JADE_HALL', 'GREATPERSON_WRITER', 'YIELD_GOLDEN_AGE_POINTS', 50);

DELETE FROM Trait_ImprovementYieldChanges
WHERE TraitType = 'TRAIT_SCHOLARS_JADE_HALL';
-- New Changes

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_MOAI', 'TECH_ENGINEERING', 'YIELD_PRODUCTION', 1);

INSERT INTO Improvement_YieldAdjacentSameType
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_MOAI', 'YIELD_CULTURE', 1);

INSERT INTO Improvement_AdjacentImprovementYieldChanges
	(ImprovementType, OtherImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_TERRACE_FARM', 'IMPROVEMENT_FARM', 'YIELD_FOOD', 1);

INSERT INTO Improvement_YieldAdjacentSameType
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_TERRACE_FARM', 'YIELD_FOOD', 1);

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_TERRACE_FARM', 'YIELD_PRODUCTION', 2),
	('IMPROVEMENT_MOAI', 'YIELD_PRODUCTION', 1);

--INSERT INTO Trait_YieldFromConquest
--	(TraitType, YieldType, Yield)
--VALUES
--	('TRAIT_SEVEN_CITIES', 'YIELD_FAITH', 150),
--	('TRAIT_SEVEN_CITIES', 'YIELD_FOOD', 75);

--INSERT INTO Trait_YieldFromSettle
--	(TraitType, YieldType, Yield)
--VALUES
--	('TRAIT_SEVEN_CITIES', 'YIELD_FAITH', 40),
--	('TRAIT_SEVEN_CITIES', 'YIELD_FOOD', 40);

INSERT INTO Trait_YieldFromTilePurchase
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_SEVEN_CITIES', 'YIELD_GOLD', 	10),
	('TRAIT_SEVEN_CITIES', 'YIELD_FAITH', 	4);

INSERT INTO Trait_YieldFromTileEarn
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_SEVEN_CITIES', 'YIELD_GOLD', 	10),
	('TRAIT_SEVEN_CITIES', 'YIELD_FAITH', 	4);

INSERT INTO Trait_YieldFromTileConquest		(TraitType, TerrainType, YieldType, Yield) SELECT 'TRAIT_SEVEN_CITIES', Type, 'YIELD_GOLD', 10 FROM Terrains;
INSERT INTO Trait_YieldFromTileConquest		(TraitType, TerrainType, YieldType, Yield) SELECT 'TRAIT_SEVEN_CITIES', Type, 'YIELD_FAITH', 4 FROM Terrains;

INSERT INTO Trait_YieldFromTileCultureBomb 	(TraitType, TerrainType, YieldType, Yield) SELECT 'TRAIT_SEVEN_CITIES', Type, 'YIELD_GOLD', 10 FROM Terrains;
INSERT INTO Trait_YieldFromTileCultureBomb 	(TraitType, TerrainType, YieldType, Yield) SELECT 'TRAIT_SEVEN_CITIES', Type, 'YIELD_FAITH', 4 FROM Terrains;

INSERT INTO Trait_YieldFromTileStealCultureBomb (TraitType, TerrainType, YieldType, Yield) SELECT 'TRAIT_SEVEN_CITIES', Type, 'YIELD_GOLD', 10 FROM Terrains;
INSERT INTO Trait_YieldFromTileStealCultureBomb (TraitType, TerrainType, YieldType, Yield) SELECT 'TRAIT_SEVEN_CITIES', Type, 'YIELD_FAITH', 4 FROM Terrains;

INSERT INTO Trait_YieldFromTileSettle 		(TraitType, TerrainType, YieldType, Yield) SELECT 'TRAIT_SEVEN_CITIES', Type, 'YIELD_GOLD', 10 FROM Terrains;
INSERT INTO Trait_YieldFromTileSettle 		(TraitType, TerrainType, YieldType, Yield) SELECT 'TRAIT_SEVEN_CITIES', Type, 'YIELD_FAITH', 4 FROM Terrains;

-- NEW DLC Leader Data and Yields

--UPDATE Buildings
--SET CitySupplyModifier = '10'
--WHERE Type = 'BUILDING_MISSION';

--UPDATE Buildings
--SET GreatWorkSlotType = 'GREAT_WORK_SLOT_ART_ARTIFACT'
--WHERE Type = 'BUILDING_MISSION';

--UPDATE Buildings
--SET GreatWorkCount = '1'
--WHERE Type = 'BUILDING_MISSION';


UPDATE Buildings
SET CitySupplyModifier = '10'
WHERE Type = 'BUILDING_WALLS_OF_BABYLON';

INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_DENMARK', 'BUILDINGCLASS_LIGHTHOUSE', 'BUILDING_JELLING_STONES'),
	('CIVILIZATION_KOREA', 'BUILDINGCLASS_UNIVERSITY', 'BUILDING_SEOWON'),
	('CIVILIZATION_MONGOL', 'BUILDINGCLASS_LODGE', 'BUILDING_YURT');

INSERT INTO Building_ClassesNeededInCity
	(BuildingType,				BuildingClassType)
VALUES	
	('BUILDING_SPAIN_MISSION',	'BUILDINGCLASS_D_FOR_SPAIN_MISSION');

INSERT INTO Building_SeaPlotYieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_JELLING_STONES', 'YIELD_FOOD', 2),
	('BUILDING_JELLING_STONES', 'YIELD_GOLD', 1);

INSERT INTO Building_ImprovementYieldChanges	
	(BuildingType, 				ImprovementType,				YieldType,		Yield)
VALUES	
	('BUILDING_SPAIN_MISSION',	'IMPROVEMENT_SPAIN_HACIENDA',	'YIELD_FAITH',	1);

INSERT INTO Building_UnitCombatProductionModifiers
	(BuildingType, UnitCombatType, Modifier)
VALUES
	('BUILDING_JELLING_STONES', 'UNITCOMBAT_MELEE', 25),
	('BUILDING_JELLING_STONES', 'UNITCOMBAT_NAVALMELEE', 25);

INSERT INTO Building_YieldFromBorderGrowth
	(BuildingType, 		YieldType, 		Yield)
VALUES	
	('BUILDING_YURT', 	'YIELD_FOOD', 	5);
		
INSERT INTO Building_YieldFromPillage
	(BuildingType, YieldType, Yield)
VALUES 
	('BUILDING_JELLING_STONES', 'YIELD_GOLD', 30),
	('BUILDING_JELLING_STONES', 'YIELD_CULTURE', 30);

INSERT INTO Building_GreatWorkYieldChangesLocal
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SEOWON', 'YIELD_SCIENCE', 1),
	('BUILDING_SEOWON', 'YIELD_PRODUCTION', 1);

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SEOWON', 'YIELD_SCIENCE', 3),
	('BUILDING_SEOWON', 'YIELD_FAITH', 2),
	('BUILDING_JELLING_STONES', 'YIELD_CULTURE', 2),
	('BUILDING_YURT', 'YIELD_FAITH', 1),
	('BUILDING_YURT', 'YIELD_PRODUCTION', 1),
	('BUILDING_WALLS_OF_BABYLON', 'YIELD_SCIENCE', 1),
	('BUILDING_SPAIN_MISSION',	'YIELD_FAITH',	2);

INSERT INTO Building_SpecialistYieldChangesLocal
	(BuildingType, SpecialistType, YieldType, Yield)
VALUES
	('BUILDING_WALLS_OF_BABYLON', 'SPECIALIST_SCIENTIST', 'YIELD_GOLD', 2);

INSERT INTO Building_GoldenAgeYieldMod
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SEOWON', 'YIELD_SCIENCE', 15);

INSERT INTO Building_GrowthExtraYield
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SEOWON', 'YIELD_SCIENCE', 50);

INSERT INTO Building_ClassesNeededInCity
	(BuildingType, BuildingClassType)
VALUES
	('BUILDING_SEOWON', 'BUILDINGCLASS_LIBRARY');

INSERT INTO Building_FeatureYieldChanges
	(BuildingType, FeatureType, YieldType, Yield)
VALUES
	('BUILDING_SEOWON', 'FEATURE_JUNGLE', 'YIELD_SCIENCE', 1);
	
INSERT INTO Building_TerrainYieldChanges
	(BuildingType, TerrainType, YieldType, Yield)
VALUES
	('BUILDING_SEOWON', 'TERRAIN_SNOW', 'YIELD_SCIENCE', 1);

INSERT INTO Building_ResourceYieldChanges
	(BuildingType, ResourceType, YieldType, Yield)
VALUES
	('BUILDING_YURT', 'RESOURCE_HORSE', 'YIELD_FOOD', 1),
	('BUILDING_YURT', 'RESOURCE_SHEEP', 'YIELD_FOOD', 1),
	('BUILDING_YURT', 'RESOURCE_COW', 'YIELD_FOOD', 1),
	('BUILDING_YURT', 'RESOURCE_DEER', 'YIELD_FOOD', 1),
	('BUILDING_YURT', 'RESOURCE_BISON', 'YIELD_FOOD', 1);

INSERT INTO Trait_MountainRangeYield
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_GREAT_ANDEAN_ROAD', 'YIELD_SCIENCE', '1'),
	('TRAIT_GREAT_ANDEAN_ROAD', 'YIELD_GOLD', '1'),
	('TRAIT_GREAT_ANDEAN_ROAD', 'YIELD_FOOD', '1');

INSERT INTO Trait_UnimprovedFeatureYieldChanges
	(TraitType, FeatureType, YieldType, Yield)
VALUES
	('TRAIT_WAYFINDING', 'FEATURE_ATOLL', 'YIELD_FOOD', 2);

INSERT INTO Trait_ImprovementYieldChanges
	(TraitType, ImprovementType, YieldType, Yield)
VALUES
	('TRAIT_WAYFINDING', 'IMPROVEMENT_FISHING_BOATS', 'YIELD_FOOD', 2);
