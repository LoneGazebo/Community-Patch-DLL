--

UPDATE GameSpeeds SET InflationPercent = 0;

--
-- Remove buildings
--

DELETE FROM Buildings							WHERE Type IN ('BUILDING_RECYCLING_CENTER', 'BUILDING_BOMB_SHELTER', 'BUILDING_NOTRE_DAME', 'BUILDING_BOROBUDUR', 'BUILDING_PRORA_RESORT');
DELETE FROM Building_BuildingClassHappiness		WHERE BuildingType IN ('BUILDING_RECYCLING_CENTER', 'BUILDING_BOMB_SHELTER', 'BUILDING_NOTRE_DAME', 'BUILDING_BOROBUDUR', 'BUILDING_PRORA_RESORT');
DELETE FROM Building_BuildingClassYieldChanges	WHERE BuildingType IN ('BUILDING_RECYCLING_CENTER', 'BUILDING_BOMB_SHELTER', 'BUILDING_NOTRE_DAME', 'BUILDING_BOROBUDUR', 'BUILDING_PRORA_RESORT');
DELETE FROM Building_FreeUnits					WHERE BuildingType IN ('BUILDING_RECYCLING_CENTER', 'BUILDING_BOMB_SHELTER', 'BUILDING_NOTRE_DAME', 'BUILDING_BOROBUDUR', 'BUILDING_PRORA_RESORT');
DELETE FROM Building_ResourceQuantity			WHERE BuildingType IN ('BUILDING_RECYCLING_CENTER', 'BUILDING_BOMB_SHELTER', 'BUILDING_NOTRE_DAME', 'BUILDING_BOROBUDUR', 'BUILDING_PRORA_RESORT');
DELETE FROM Building_YieldChanges 				WHERE BuildingType IN ('BUILDING_RECYCLING_CENTER', 'BUILDING_BOMB_SHELTER', 'BUILDING_NOTRE_DAME', 'BUILDING_BOROBUDUR', 'BUILDING_PRORA_RESORT');

-- remap IDs
DROP TABLE IF EXISTS IDRemapper;
CREATE TABLE IDRemapper ( id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT );
INSERT INTO IDRemapper (Type) SELECT Type FROM Buildings;
UPDATE Buildings SET ID = ID + 1000;
UPDATE Buildings SET ID = ( SELECT IDRemapper.id-1 FROM IDRemapper WHERE Buildings.Type = IDRemapper.Type);
UPDATE sqlite_sequence SET seq = (SELECT COUNT(ID) FROM Buildings)-1 WHERE name = 'Buildings';



--
-- Cost: Buildings
--

/*
-- Set maintenance for buildings not included in the table
UPDATE Buildings SET GoldMaintenance = Cost / 100 WHERE GoldMaintenance <> 0;


-- This GEC_End.sql data from:
-- Buildings tab of GEM_Details.xls spreadsheet (in mod folder).
UPDATE Buildings SET Cost = Cost * 1.056, GoldMaintenance = 7  WHERE BuildingClass = 'BUILDINGCLASS_STADIUM';
UPDATE Buildings SET Cost = Cost * 1.806, GoldMaintenance = 5  WHERE BuildingClass = 'BUILDINGCLASS_THEATRE';
UPDATE Buildings SET Cost = Cost * 2.5  , GoldMaintenance = 4  WHERE BuildingClass = 'BUILDINGCLASS_COLOSSEUM';
UPDATE Buildings SET Cost = 50          , GoldMaintenance = 0  WHERE BuildingClass = 'BUILDINGCLASS_GEM_PALACE';
UPDATE Buildings SET Cost = Cost * 1.667, GoldMaintenance = 0  WHERE BuildingClass = 'BUILDINGCLASS_COURTHOUSE';
UPDATE Buildings SET Cost = Cost * 2.431, GoldMaintenance = 3  WHERE BuildingClass = 'BUILDINGCLASS_CIRCUS';
UPDATE Buildings SET Cost = Cost * 1.333, GoldMaintenance = 5  WHERE BuildingClass = 'BUILDINGCLASS_SEAPORT';
UPDATE Buildings SET Cost = 400         , GoldMaintenance = 3  WHERE BuildingClass = 'BUILDINGCLASS_WAREHOUSE';
UPDATE Buildings SET Cost = Cost * 1.481, GoldMaintenance = 1  WHERE BuildingClass = 'BUILDINGCLASS_LIGHTHOUSE';
UPDATE Buildings SET Cost = Cost * 0.926, GoldMaintenance = 1  WHERE BuildingClass = 'BUILDINGCLASS_HARBOR';
UPDATE Buildings SET Cost = Cost * 0.889, GoldMaintenance = 6  WHERE BuildingClass = 'BUILDINGCLASS_MEDICAL_LAB';
UPDATE Buildings SET Cost = Cost * 0.926, GoldMaintenance = 5  WHERE BuildingClass = 'BUILDINGCLASS_HOSPITAL';
UPDATE Buildings SET Cost = Cost * 1.944, GoldMaintenance = 3  WHERE BuildingClass = 'BUILDINGCLASS_AQUEDUCT';
UPDATE Buildings SET Cost = Cost * 1.389, GoldMaintenance = 1  WHERE BuildingClass = 'BUILDINGCLASS_GRANARY';
UPDATE Buildings SET Cost = 150         , GoldMaintenance = 1  WHERE BuildingClass = 'BUILDINGCLASS_CITY_HALL';
UPDATE Buildings SET Cost = Cost * 1.466, GoldMaintenance = 7  WHERE BuildingClass = 'BUILDINGCLASS_STOCK_EXCHANGE';
UPDATE Buildings SET Cost = Cost * 1.389, GoldMaintenance = 4  WHERE BuildingClass = 'BUILDINGCLASS_BANK';
UPDATE Buildings SET Cost = Cost * 1.389, GoldMaintenance = 2  WHERE BuildingClass = 'BUILDINGCLASS_MINT';
UPDATE Buildings SET Cost = Cost * 0.833, GoldMaintenance = 1  WHERE BuildingClass = 'BUILDINGCLASS_MARKET';
UPDATE Buildings SET Cost = Cost * 1.056, GoldMaintenance = 7  WHERE BuildingClass = 'BUILDINGCLASS_SPACESHIP_FACTORY';
UPDATE Buildings SET Cost = Cost * 0.944, GoldMaintenance = 6  WHERE BuildingClass = 'BUILDINGCLASS_NUCLEAR_PLANT';
UPDATE Buildings SET Cost = Cost * 0.722, GoldMaintenance = 4  WHERE BuildingClass = 'BUILDINGCLASS_SOLAR_PLANT';
UPDATE Buildings SET Cost = Cost * 1.08 , GoldMaintenance = 5  WHERE BuildingClass = 'BUILDINGCLASS_FACTORY';
UPDATE Buildings SET Cost = Cost * 0.926, GoldMaintenance = 2  WHERE BuildingClass = 'BUILDINGCLASS_WORKSHOP';
UPDATE Buildings SET Cost = Cost * 0.694, GoldMaintenance = 2  WHERE BuildingClass = 'BUILDINGCLASS_HYDRO_PLANT';
UPDATE Buildings SET Cost = Cost * 0.667, GoldMaintenance = 1  WHERE BuildingClass = 'BUILDINGCLASS_WINDMILL';
UPDATE Buildings SET Cost = Cost * 0.926, GoldMaintenance = 1  WHERE BuildingClass = 'BUILDINGCLASS_FORGE';
UPDATE Buildings SET Cost = Cost * 1.111, GoldMaintenance = 1  WHERE BuildingClass = 'BUILDINGCLASS_STONEWORKS';
UPDATE Buildings SET Cost = Cost * 1.481, GoldMaintenance = 1  WHERE BuildingClass = 'BUILDINGCLASS_WATERMILL';
UPDATE Buildings SET Cost = Cost * 0.833, GoldMaintenance = 1  WHERE BuildingClass = 'BUILDINGCLASS_STABLE';
UPDATE Buildings SET Cost = Cost * 0.833, GoldMaintenance = 5  WHERE BuildingClass = 'BUILDINGCLASS_BROADCAST_TOWER';
UPDATE Buildings SET Cost = Cost * 1.296, GoldMaintenance = 5  WHERE BuildingClass = 'BUILDINGCLASS_MUSEUM';
UPDATE Buildings SET Cost = Cost * 0.972, GoldMaintenance = 2  WHERE BuildingClass = 'BUILDINGCLASS_OPERA_HOUSE';
UPDATE Buildings SET Cost = Cost * 1.111, GoldMaintenance = 1  WHERE BuildingClass = 'BUILDINGCLASS_AMPHITHEATER';
UPDATE Buildings SET Cost = Cost * 1.389, GoldMaintenance = 1  WHERE BuildingClass = 'BUILDINGCLASS_MONUMENT';
UPDATE Buildings SET Cost = Cost * 0.944, GoldMaintenance = 6  WHERE BuildingClass = 'BUILDINGCLASS_LABORATORY';
UPDATE Buildings SET Cost = Cost * 1.204, GoldMaintenance = 3  WHERE BuildingClass = 'BUILDINGCLASS_PUBLIC_SCHOOL';
UPDATE Buildings SET Cost = Cost * 1.215, GoldMaintenance = 2  WHERE BuildingClass = 'BUILDINGCLASS_UNIVERSITY';
UPDATE Buildings SET Cost = Cost * 1.111, GoldMaintenance = 1  WHERE BuildingClass = 'BUILDINGCLASS_LIBRARY';
UPDATE Buildings SET Cost = 100         , GoldMaintenance = 1  WHERE BuildingClass = 'BUILDINGCLASS_MENTORS_HALL';
UPDATE Buildings SET Cost = 700         , GoldMaintenance = 5  WHERE BuildingClass = 'BUILDINGCLASS_INTELLIGENCE_AGENCY';
UPDATE Buildings SET Cost = 700         , GoldMaintenance = 5  WHERE BuildingClass = 'BUILDINGCLASS_POLICE_STATION';
UPDATE Buildings SET Cost = Cost * 1.042, GoldMaintenance = 2  WHERE BuildingClass = 'BUILDINGCLASS_CONSTABLE';
UPDATE Buildings SET Cost = Cost * 0.926, GoldMaintenance = 1  WHERE BuildingClass = 'BUILDINGCLASS_GARDEN';
UPDATE Buildings SET Cost = Cost * 1.111, GoldMaintenance = 1  WHERE BuildingClass = 'BUILDINGCLASS_TEMPLE';
UPDATE Buildings SET Cost = Cost * 0.694, GoldMaintenance = 0  WHERE BuildingClass = 'BUILDINGCLASS_SHRINE';
UPDATE Buildings SET Cost = Cost * 0.611, GoldMaintenance = 0  WHERE BuildingClass = 'BUILDINGCLASS_MILITARY_BASE';
UPDATE Buildings SET Cost = Cost * 0.741, GoldMaintenance = 0  WHERE BuildingClass = 'BUILDINGCLASS_ARSENAL';
UPDATE Buildings SET Cost = Cost * 1.042, GoldMaintenance = 0  WHERE BuildingClass = 'BUILDINGCLASS_CASTLE';
UPDATE Buildings SET Cost = Cost * 1.111, GoldMaintenance = 0  WHERE BuildingClass = 'BUILDINGCLASS_WALLS';
UPDATE Buildings SET Cost = Cost * 0.833, GoldMaintenance = 3  WHERE BuildingClass = 'BUILDINGCLASS_MILITARY_ACADEMY';
UPDATE Buildings SET Cost = Cost * 1.042, GoldMaintenance = 2  WHERE BuildingClass = 'BUILDINGCLASS_ARMORY';
UPDATE Buildings SET Cost = Cost * 0.741, GoldMaintenance = 1  WHERE BuildingClass = 'BUILDINGCLASS_BARRACKS';


*/

UPDATE Buildings SET Cost = Cost * 0.500 WHERE BuildingClass = 'BUILDINGCLASS_WINDMILL';
UPDATE Buildings SET Cost = Cost * 0.500 WHERE BuildingClass = 'BUILDINGCLASS_CARAVANSARY';

--
-- Unique Buildings
--

UPDATE Buildings SET Cost = (SELECT b.Cost FROM Buildings b, BuildingClasses class WHERE (
	Buildings.BuildingClass = b.BuildingClass
	AND class.Type = b.BuildingClass
	AND class.DefaultBuilding = b.Type
));


UPDATE Buildings SET GoldMaintenance = MAX(0, ROUND(GoldMaintenance, 0)) WHERE GoldMaintenance <> 0;

UPDATE Buildings
SET Cost = ROUND(Cost / 50, 0) * 50
WHERE Cost > 0;

UPDATE Projects
SET Cost = ROUND(Cost / 50, 0) * 50
WHERE Cost > 0;

UPDATE Buildings SET UnlockedByBelief = 1, FaithCost = 1 * Cost
WHERE (FaithCost > 0 AND Cost > 0)
OR BuildingClass IN (
	'BUILDINGCLASS_LIBRARY'			,
	'BUILDINGCLASS_WORKSHOP'		,
	'BUILDINGCLASS_STABLE'			,
	'BUILDINGCLASS_WATERMILL'		,
	'BUILDINGCLASS_WINDMILL'		,
	'BUILDINGCLASS_FACTORY'			,
	'BUILDINGCLASS_CARAVANSARY'		,
	'BUILDINGCLASS_MARKET'			,
	'BUILDINGCLASS_BANK'			,
	'BUILDINGCLASS_STOCK_EXCHANGE'	,
	'BUILDINGCLASS_LIGHTHOUSE'		,
	'BUILDINGCLASS_HARBOR'			,
	'BUILDINGCLASS_SEAPORT'			
);

--
-- Cost: Wonders
--

DROP TABLE IF EXISTS GEM_WonderTechs;
CREATE TABLE GEM_WonderTechs(BuildingClass text, GridX integer);
INSERT INTO GEM_WonderTechs (BuildingClass, GridX)
SELECT building.BuildingClass, tech.GridX
FROM Buildings building, BuildingClasses class, Technologies tech
WHERE ( building.PrereqTech = tech.Type
	AND building.BuildingClass = class.Type
	AND class.MaxGlobalInstances = 1
);

UPDATE GEM_WonderTechs
SET GridX = GridX - 1
WHERE BuildingClass IN (
	'BUILDINGCLASS_STONEHENGE'			,
	'BUILDINGCLASS_GREAT_LIGHTHOUSE'	,
	'BUILDINGCLASS_STATUE_ZEUS'			,
	'BUILDINGCLASS_MACHU_PICHU'			,
	'BUILDINGCLASS_TERRACOTTA_ARMY'		
);

UPDATE GEM_WonderTechs
SET GridX = GridX + 1
WHERE BuildingClass IN (
	'BUILDINGCLASS_GREAT_LIBRARY'		,
	'BUILDINGCLASS_BANAUE_RICE_TERRACES'
);

UPDATE Buildings SET Cost = 150   WHERE BuildingClass IN (SELECT BuildingClass FROM GEM_WonderTechs WHERE GridX < 1);
UPDATE Buildings SET Cost = 180   WHERE BuildingClass IN (SELECT BuildingClass FROM GEM_WonderTechs WHERE GridX = 1);
UPDATE Buildings SET Cost = 210   WHERE BuildingClass IN (SELECT BuildingClass FROM GEM_WonderTechs WHERE GridX = 2);
UPDATE Buildings SET Cost = 240   WHERE BuildingClass IN (SELECT BuildingClass FROM GEM_WonderTechs WHERE GridX = 3);
UPDATE Buildings SET Cost = 290   WHERE BuildingClass IN (SELECT BuildingClass FROM GEM_WonderTechs WHERE GridX = 4);
UPDATE Buildings SET Cost = 350   WHERE BuildingClass IN (SELECT BuildingClass FROM GEM_WonderTechs WHERE GridX = 5);
UPDATE Buildings SET Cost = 430   WHERE BuildingClass IN (SELECT BuildingClass FROM GEM_WonderTechs WHERE GridX = 6);
UPDATE Buildings SET Cost = 520   WHERE BuildingClass IN (SELECT BuildingClass FROM GEM_WonderTechs WHERE GridX = 7);
UPDATE Buildings SET Cost = 620   WHERE BuildingClass IN (SELECT BuildingClass FROM GEM_WonderTechs WHERE GridX = 8);
UPDATE Buildings SET Cost = 750   WHERE BuildingClass IN (SELECT BuildingClass FROM GEM_WonderTechs WHERE GridX = 9);
UPDATE Buildings SET Cost = 900   WHERE BuildingClass IN (SELECT BuildingClass FROM GEM_WonderTechs WHERE GridX = 10);
UPDATE Buildings SET Cost = 1070  WHERE BuildingClass IN (SELECT BuildingClass FROM GEM_WonderTechs WHERE GridX = 11);
UPDATE Buildings SET Cost = 1270  WHERE BuildingClass IN (SELECT BuildingClass FROM GEM_WonderTechs WHERE GridX = 12);
UPDATE Buildings SET Cost = 1270  WHERE BuildingClass IN (SELECT BuildingClass FROM GEM_WonderTechs WHERE GridX = 13);
UPDATE Buildings SET Cost = 1270  WHERE BuildingClass IN (SELECT BuildingClass FROM GEM_WonderTechs WHERE GridX = 14);
UPDATE Buildings SET Cost = 1270  WHERE BuildingClass IN (SELECT BuildingClass FROM GEM_WonderTechs WHERE GridX > 14);

DROP TABLE GEM_WonderTechs;


--
-- Specific Buildings
--

/*
UPDATE Buildings SET FoodKept = 0 WHERE BuildingClass = 'BUILDINGCLASS_AQUEDUCT';
INSERT INTO				Building_SurplusYieldModifier (BuildingType, YieldType, Yield) 
SELECT					Type, 'YIELD_FOOD', 67
FROM					Buildings
WHERE					BuildingClass = 'BUILDINGCLASS_AQUEDUCT';
*/

INSERT INTO				Building_LakePlotYieldChanges (BuildingType, YieldType, Yield) 
SELECT					Type, 'YIELD_FOOD', 1
FROM					Buildings
WHERE					BuildingClass = 'BUILDINGCLASS_AQUEDUCT';


--
-- Resources
--

DELETE FROM Building_ResourceYieldChanges
WHERE BuildingType IN (
	'BUILDING_GRANARY'		,
	'BUILDING_STABLE'		,
	'BUILDING_DUCAL_STABLE'	,
	'BUILDING_LIGHTHOUSE'	,
	'BUILDING_COTHON'		,
	'BUILDING_MINT'
);

INSERT OR REPLACE INTO	Building_ResourceYieldChanges
						(BuildingType, ResourceType, YieldType, Yield) 
SELECT					building.Type, res.Type, 'YIELD_PRODUCTION', 1
FROM					Buildings building, Resources res
WHERE					building.BuildingClass = 'BUILDINGCLASS_LIGHTHOUSE'
						AND res.TechCityTrade = 'TECH_SAILING';

INSERT OR REPLACE INTO	Building_ResourceYieldChanges
						(BuildingType, ResourceType, YieldType, Yield) 
SELECT					building.Type, res.Type, 'YIELD_FAITH', 1
FROM					Buildings building, Resources res
WHERE					building.BuildingClass = 'BUILDINGCLASS_SHRINE'
						AND res.Type IN (
							'RESOURCE_WINE'		,
							'RESOURCE_INCENSE'
						);

INSERT OR REPLACE INTO	Building_ResourceYieldChanges
						(BuildingType, ResourceType, YieldType, Yield) 
SELECT					building.Type, res.Type, 'YIELD_FOOD', 1
FROM					Buildings building, Resources res
WHERE					building.BuildingClass = 'BUILDINGCLASS_GRANARY'
						AND res.Type IN (
							'RESOURCE_WHEAT'	,
							'RESOURCE_SPICES'	,
							'RESOURCE_SUGAR'	
						);

INSERT OR REPLACE INTO	Building_ResourceYieldChanges
						(BuildingType, ResourceType, YieldType, Yield) 
SELECT					building.Type, res.Type, 'YIELD_FOOD', 1
FROM					Buildings building, Resources res
WHERE					building.BuildingClass = 'BUILDINGCLASS_AQUEDUCT'
						AND res.Type IN (
							'RESOURCE_CITRUS'	,
							'RESOURCE_TRUFFLES'	,
							'RESOURCE_BANANA'	,
							'RESOURCE_SALT'		
						);

INSERT OR REPLACE INTO	Building_ResourceYieldChanges
						(BuildingType, ResourceType, YieldType, Yield) 
SELECT					building.Type, res.Type, 'YIELD_CULTURE', 2
FROM					Buildings building, Resources res
WHERE					building.BuildingClass = 'BUILDINGCLASS_AMPHITHEATER'
						AND res.Type IN (
							'RESOURCE_SILK'		,
							'RESOURCE_COTTON'	,
							'RESOURCE_FUR'		,
							'RESOURCE_DYE'
						);

INSERT OR REPLACE INTO	Building_ResourceYieldChanges
						(BuildingType, ResourceType, YieldType, Yield) 
SELECT					building.Type, res.Type, 'YIELD_PRODUCTION', 1
FROM					Buildings building, Resources res
WHERE					building.BuildingClass = 'BUILDINGCLASS_STABLE'
						AND res.Type IN (
							'RESOURCE_HORSE'	,
							'RESOURCE_SHEEP'	,
							'RESOURCE_COW'		,
							'RESOURCE_DEER'		,
							'RESOURCE_IVORY'
						);

INSERT OR REPLACE INTO	Building_ResourceYieldChanges
						(BuildingType, ResourceType, YieldType, Yield) 
SELECT					'BUILDING_DUCAL_STABLE', res.Type, 'YIELD_GOLD', 1
FROM					Resources res
WHERE					res.Type IN (
							'RESOURCE_HORSE'	,
							'RESOURCE_SHEEP'	,
							'RESOURCE_COW'		,
							'RESOURCE_DEER'		,
							'RESOURCE_IVORY'
						);

INSERT OR REPLACE INTO	Building_ResourceYieldChanges
						(BuildingType, ResourceType, YieldType, Yield) 
SELECT					building.Type, res.Type, 'YIELD_GOLD', 1
FROM					Buildings building, Resources res
WHERE					building.BuildingClass = 'BUILDINGCLASS_MINT'
						AND res.Type IN (
							'RESOURCE_COPPER'	,
							'RESOURCE_SILVER'	,
							'RESOURCE_GOLD'		,
							'RESOURCE_GEMS'		
						);

INSERT OR REPLACE INTO	Building_ResourceYieldChanges
						(BuildingType, ResourceType, YieldType, Yield) 
SELECT					building.Type, res.Type, 'YIELD_PRODUCTION', 1
FROM					Buildings building, Resources res
WHERE					building.BuildingClass = 'BUILDINGCLASS_BARRACKS'
						AND res.TechReveal <> 'TECH_ARCHAEOLOGY'
						AND res.ResourceClassType IN (
							'RESOURCECLASS_RUSH',
							'RESOURCECLASS_MODERN'
						);

/*
INSERT OR REPLACE INTO	Building_ResourceYieldChanges
						(BuildingType, ResourceType, YieldType, Yield) 
SELECT					building.Type, res.Type, 'YIELD_PRODUCTION', 2
FROM					Buildings building, Resources res
WHERE					building.BuildingClass = 'BUILDINGCLASS_FACTORY'
						AND res.Type IN (
							'RESOURCE_COAL'		,
							'RESOURCE_ALUMINUM'	,
							'RESOURCE_OIL'		,
							'RESOURCE_URANIUM'
						);
*/

INSERT OR REPLACE INTO	Building_ResourceYieldChanges(BuildingType, ResourceType, YieldType, Yield) 
SELECT					building.Type, res.Type, 'YIELD_GOLD', 1
FROM					Buildings building, Resources res
WHERE					building.BuildingClass = 'BUILDINGCLASS_CARAVANSARY'
AND						res.Happiness > 0;



--
-- Yields
--

INSERT OR REPLACE INTO	Building_YieldChanges(BuildingType, YieldType, Yield) 
SELECT					building.Type, 'YIELD_FOOD', 1
FROM					Buildings building
WHERE					building.BuildingClass IN (
						'BUILDINGCLASS_LIGHTHOUSE'
						);

INSERT OR REPLACE INTO	Building_YieldChanges(BuildingType, YieldType, Yield) 
SELECT					building.Type, 'YIELD_PRODUCTION', 1
FROM					Buildings building
WHERE					building.BuildingClass IN (
						'BUILDINGCLASS_STABLE'		,
						--'BUILDINGCLASS_BARRACKS'	,
						'BUILDINGCLASS_FORGE'
						);

						
UPDATE					Building_YieldChanges
SET						Yield = 3
WHERE					(YieldType = 'YIELD_CULTURE'
AND						BuildingType IN (SELECT Type FROM Buildings WHERE BuildingClass IN (
						'BUILDINGCLASS_AMPHITHEATER'	,
						'BUILDINGCLASS_BROADCAST_TOWER'	
						)));


DELETE FROM Building_UnitCombatProductionModifiers
WHERE BuildingType IN (
	'BUILDING_STABLE'		,
	'BUILDING_DUCAL_STABLE'	
);

INSERT OR REPLACE INTO	Building_UnitCombatProductionModifiers(BuildingType, UnitCombatType, Modifier) 
SELECT					building.Type, combat.Type, 15
FROM					Buildings building, UnitCombatInfos combat
WHERE					building.BuildingClass IN (
						'BUILDINGCLASS_STABLE'		,
						--'BUILDINGCLASS_BARRACKS'	,
						'BUILDINGCLASS_FORGE'
						)
AND						combat.Type IN (
						'UNITCOMBAT_MOUNTED'		,
						'UNITCOMBAT_GUN'			
						);


--
-- Features
--

DELETE FROM				Building_FeatureYieldChanges WHERE FeatureType = 'FEATURE_JUNGLE';

INSERT OR REPLACE INTO	Building_FeatureYieldChanges(BuildingType, FeatureType, YieldType, Yield) 
SELECT					building.Type, 'FEATURE_JUNGLE', 'YIELD_SCIENCE', 1
FROM					Buildings building
WHERE					building.BuildingClass IN (
						'BUILDINGCLASS_UNIVERSITY'		,
						'BUILDINGCLASS_LABORATORY'	
						);




--
-- Specialists
--

UPDATE Specialists
SET GreatPeopleRateChange = 3
WHERE GreatPeopleRateChange > 0;

UPDATE Buildings
SET SpecialistType = 'SPECIALIST_ENGINEER', SpecialistCount = 1
WHERE BuildingClass IN (
	'BUILDINGCLASS_WORKSHOP'			,
	'BUILDINGCLASS_WALLS'				,
	'BUILDINGCLASS_CASTLE'				,
	'BUILDINGCLASS_FACTORY'				,
	'BUILDINGCLASS_IRONWORKS'			
);

UPDATE Buildings
SET SpecialistType = 'SPECIALIST_MERCHANT', SpecialistCount = 1
WHERE BuildingClass IN (
	'BUILDINGCLASS_CARAVANSARY'			,
	'BUILDINGCLASS_MARKET'				,
	'BUILDINGCLASS_BANK'				,
	'BUILDINGCLASS_STOCK_EXCHANGE'		,
	'BUILDINGCLASS_NATIONAL_TREASURY'	
);

UPDATE Buildings
SET SpecialistType = 'SPECIALIST_SCIENTIST', SpecialistCount = 1
WHERE BuildingClass IN (
	'BUILDINGCLASS_LIBRARY'				,
	'BUILDINGCLASS_UNIVERSITY'			,
	'BUILDINGCLASS_PUBLIC_SCHOOL'		,
	'BUILDINGCLASS_LABORATORY'			,
	'BUILDINGCLASS_OXFORD_UNIVERSITY'	
);

--/*
UPDATE Buildings
SET SpecialistType = NULL, SpecialistCount = 0
WHERE BuildingClass IN (
	'BUILDINGCLASS_WINDMILL'
);
--*/

/*
UPDATE Buildings
SET Flat = 0
WHERE Type = 'BUILDING_WINDMILL';
*/

UPDATE Buildings
SET SpecialistType = 'SPECIALIST_SCIENTIST', SpecialistCount = 2
WHERE Type = 'BUILDING_WALLS_OF_BABYLON';


--
-- Remove Buildings
--

UPDATE Buildings SET Cost = -1, PrereqTech = NULL, FaithCost = -1, GreatWorkCount = -1 WHERE BuildingClass IN (
	'BUILDINGCLASS_RECYCLING_CENTER'	,
	'BUILDINGCLASS_BOMB_SHELTER'		,
	'BUILDINGCLASS_FORGE'				
	
);

--
-- Remove Unique or Dummy Buildings
--
/* -- This is redundant; these buildings should be made invisible where the buildings are originally defined --Thal
UPDATE Buildings SET Cost = -1, PrereqTech = 'NULL', FaithCost = -1, GreatWorkCount = -1 WHERE Type IN (
	'BUILDING_TRIBUTE'					,
	'BUILDING_AMSTERDAM_BOURSE'			,
	'BUILDING_MUGHAL_FORT'				,
	'BUILDING_HANSEATIC_LEAGUE'			,
	'BUILDING_CEILIDH_HALL'				,
	'BUILDING_AI_CULTURE'				,
	'BUILDING_AI_GOLD'					,
	'BUILDING_AI_PRODUCTION'			,
	'BUILDING_AI_SCIENCE'				,
	'BUILDING_HAPPINESS_CITY'			,
	'BUILDING_HAPPINESS_NATIONAL'		
);
*/

--
-- Rename Buildings
--

UPDATE Buildings SET IconAtlas='TECH_ATLAS_1',	PortraitIndex=51	WHERE Type = 'BUILDING_STADIUM';

--UPDATE Buildings SET							PortraitIndex=20	WHERE Type = 'BUILDING_AMPHITHEATER';

UPDATE Buildings SET IconAtlas='BW_ATLAS_1',	PortraitIndex=20	WHERE Type = 'BUILDING_AMPHITHEATER';
UPDATE Buildings SET							PortraitIndex=28	WHERE Type = 'BUILDING_WINDMILL';
--UPDATE Buildings SET							PortraitIndex=4		WHERE Type = 'BUILDING_FACTORY';
--UPDATE Buildings SET IconAtlas='TECH_ATLAS_1',	PortraitIndex=14	WHERE Type = 'BUILDING_FORGE';
UPDATE Buildings SET							PortraitIndex=2		WHERE Type = 'BUILDING_WORKSHOP';
UPDATE Buildings SET IconAtlas='EXPANSION2_BUILDING_ATLAS',	PortraitIndex=10		WHERE Type = 'BUILDING_PALACE';

-- This CEC__End.sql data created by:
-- Renames tab of CEP_ArmiesCities.xls spreadsheet (in mod folder).
--UPDATE Buildings SET Description='TXT_KEY_BUILDING_THEATRE',             Civilopedia='TXT_KEY_BUILDING_THEATRE_PEDIA'              WHERE Type = 'BUILDING_AMPHITHEATER';
UPDATE Buildings SET Description='TXT_KEY_BUILDING_ARENA',               Civilopedia='TXT_KEY_BUILDING_ARENA_PEDIA'                WHERE Type = 'BUILDING_COLOSSEUM';
--UPDATE Buildings SET Description='TXT_KEY_BUILDING_ZOO',                 Civilopedia='TXT_KEY_BUILDING_ZOO_PEDIA'                  WHERE Type = 'BUILDING_THEATRE';
UPDATE Buildings SET Description='TXT_KEY_BUILDING_CINEMA',              Civilopedia='TXT_KEY_BUILDING_CINEMA_PEDIA'               WHERE Type = 'BUILDING_STADIUM';
UPDATE Buildings SET Description='TXT_KEY_BUILDING_VACCINATIONS',        Civilopedia='TXT_KEY_BUILDING_VACCINATIONS_PEDIA'         WHERE Type = 'BUILDING_MEDICAL_LAB';
UPDATE Buildings SET Description='TXT_KEY_BUILDING_SUPERMAX_PRISON',     Civilopedia='TXT_KEY_BUILDING_SUPERMAX_PRISON_PEDIA'      WHERE Type = 'BUILDING_POLICE_STATION';
UPDATE Buildings SET Description='TXT_KEY_BUILDING_CAPITAL_BUILDING',    Civilopedia='TXT_KEY_BUILDING_CAPITAL_BUILDING_PEDIA'     WHERE Type = 'BUILDING_PALACE';
UPDATE Buildings SET Description='TXT_KEY_BUILDING_WORKSHOP',            Civilopedia='TXT_KEY_CIV5_BUILDINGS_WORKSHOP_TEXT'             WHERE Type = 'BUILDING_WINDMILL';
UPDATE Buildings SET Description='TXT_KEY_BUILDING_SMITH',               Civilopedia='TXT_KEY_BUILDING_SMITH_PEDIA'                WHERE Type = 'BUILDING_WORKSHOP';
UPDATE Buildings SET Help='TXT_KEY_BUILDING_TEMPLE_HELP'		WHERE Type='BUILDING_TEMPLE';
UPDATE Buildings SET Help='TXT_KEY_BUILDING_AMPHITHEATER_HELP'	WHERE Type='BUILDING_AMPHITHEATER';
UPDATE Buildings SET Help='TXT_KEY_BUILDING_MARKET_HELP'		WHERE Type='BUILDING_MARKET';

UPDATE BuildingClasses SET Description = (SELECT Description FROM Buildings b WHERE BuildingClasses.DefaultBuilding = b.Type);

UPDATE Buildings SET Cost = -1 WHERE Cost <= 0;


UPDATE LoadedFile SET Value=1 WHERE Type='CEC__End.sql';