-- Attila

UPDATE Traits
SET LandBarbarianConversionPercent = '0'
WHERE Type = 'TRAIT_RAZE_AND_HORSES';

UPDATE Traits
SET RazeSpeedModifier = '0'
WHERE Type = 'TRAIT_RAZE_AND_HORSES';

UPDATE Traits
SET WarWearinessModifier = '50'
WHERE Type = 'TRAIT_RAZE_AND_HORSES';

UPDATE Traits
SET EnemyWarWearinessModifier = '50'
WHERE Type = 'TRAIT_RAZE_AND_HORSES';

DELETE FROM Civilization_FreeTechs
WHERE TechType = 'TECH_ANIMAL_HUSBANDRY' AND CivilizationType = 'CIVILIZATION_HUNS';

DELETE FROM Trait_ImprovementYieldChanges
WHERE TraitType = 'TRAIT_RAZE_AND_HORSES';

-- Eki

INSERT INTO ArtDefine_LandmarkTypes(Type, LandmarkType, FriendlyName)
SELECT 'ART_DEF_IMPROVEMENT_EKI', 'Improvement', 'Eki';

INSERT INTO ArtDefine_Landmarks(Era, State, Scale, ImprovementType, LayoutHandler, ResourceType, Model, TerrainContour)
SELECT 'Any', 'UnderConstruction', 1,  'ART_DEF_IMPROVEMENT_EKI', 'SNAPSHOT', 'ART_DEF_RESOURCE_ALL', 'eki_UC.fxsxml', 1 UNION ALL
SELECT 'Any', 'Constructed', 1,  'ART_DEF_IMPROVEMENT_EKI', 'SNAPSHOT', 'ART_DEF_RESOURCE_ALL', 'eki_built.fxsxml', 1 UNION ALL
SELECT 'Any', 'Pillaged', 1,  'ART_DEF_IMPROVEMENT_EKI', 'SNAPSHOT', 'ART_DEF_RESOURCE_ALL', 'eki_PL.fxsxml', 1;

-- Boudicca -- Boost Ceilidh Hall -- Move to Medieval, add faith
UPDATE Buildings
SET PrereqTech = 'TECH_PHYSICS'
WHERE Type = 'BUILDING_CEILIDH_HALL';

UPDATE Buildings
SET BuildingClass = 'BUILDINGCLASS_CIRCUS'
WHERE Type = 'BUILDING_CEILIDH_HALL';

UPDATE Buildings
SET WLTKDTurns = '15'
WHERE Type = 'BUILDING_CEILIDH_HALL';

UPDATE Buildings 
SET Help = 'TXT_KEY_BUILDING_CEILIDH_HALL_HELP'
WHERE Type = 'BUILDING_CEILIDH_HALL';

INSERT INTO Building_ResourceYieldChanges (BuildingType, ResourceType, YieldType, Yield)
VALUES ('BUILDING_CEILIDH_HALL', 'RESOURCE_IVORY' , 'YIELD_CULTURE' , '3');

UPDATE Building_YieldChanges
SET Yield = '4'
WHERE BuildingType = 'BUILDING_CEILIDH_HALL' AND YieldType = 'YIELD_CULTURE';

UPDATE Buildings
SET Happiness = '1'
WHERE Type = 'BUILDING_CEILIDH_HALL';

UPDATE Buildings
SET SpecialistType = 'SPECIALIST_MUSICIAN'
WHERE Type = 'BUILDING_CEILIDH_HALL';

UPDATE Buildings
SET SpecialistCount = '1'
WHERE Type = 'BUILDING_CEILIDH_HALL';

UPDATE Civilization_BuildingClassOverrides
SET BuildingClassType = 'BUILDINGCLASS_CIRCUS'
WHERE BuildingType = 'BUILDING_CEILIDH_HALL';

UPDATE Building_ClassesNeededInCity
Set BuildingClassType = 'BUILDINGCLASS_COLOSSEUM'
WHERE BuildingType = 'BUILDING_CEILIDH_HALL';

UPDATE Traits
SET UniqueBeliefsOnly = '1'
WHERE Type = 'TRAIT_FAITH_FROM_NATURE';

UPDATE Traits
SET NoNaturalReligionSpread = '1'
WHERE Type = 'TRAIT_FAITH_FROM_NATURE';

UPDATE Traits
SET FaithFromUnimprovedForest = '0'
WHERE Type = 'TRAIT_FAITH_FROM_NATURE';

-- Dido -- Delete African Forest Elephant, remove mountain bonus (given to incans)

UPDATE Traits
Set CrossesMountainsAfterGreatGeneral = '0'
WHERE Type = 'TRAIT_PHOENICIAN_HERITAGE';

UPDATE Traits
SET TradeRouteResourceModifier = '100'
WHERE Type = 'TRAIT_PHOENICIAN_HERITAGE';

UPDATE Traits
SET FreeBuilding = 'BUILDING_LIGHTHOUSE'
WHERE Type = 'TRAIT_PHOENICIAN_HERITAGE';

-- Gustavus Adolphus -- Remove Hakkepillita, add unique Public School

UPDATE Traits
SET GreatPersonGiftInfluence = '0'
WHERE Type = 'TRAIT_DIPLOMACY_GREAT_PEOPLE';

UPDATE Traits
SET DOFGreatPersonModifier = '0'
WHERE Type = 'TRAIT_DIPLOMACY_GREAT_PEOPLE';

UPDATE Traits
SET XPBonusFromGGBirth = '15'
WHERE Type = 'TRAIT_DIPLOMACY_GREAT_PEOPLE';

UPDATE Traits
SET GreatGeneralExtraBonus = '15'
WHERE Type = 'TRAIT_DIPLOMACY_GREAT_PEOPLE';

-- Selassie -- Peace Treaty Bonuss

UPDATE Traits
SET IsAdoptionFreeTech = '1'
WHERE Type = 'TRAIT_BONUS_AGAINST_TECH';

UPDATE Traits
SET CombatBonusVsLargerCiv = '0'
WHERE Type = 'TRAIT_BONUS_AGAINST_TECH';

INSERT INTO Trait_YieldChangesStrategicResources
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_BONUS_AGAINST_TECH', 'YIELD_FAITH', 1);


UPDATE Buildings
SET PlotCultureCostModifier = '-25'
WHERE Type = 'BUILDING_STELE';

UPDATE Building_YieldChanges
SET Yield = '2'
WHERE BuildingType = 'BUILDING_STELE' AND YieldType = 'YIELD_FAITH';

UPDATE Building_YieldChanges
SET Yield = '2'
WHERE BuildingType = 'BUILDING_STELE' AND YieldType = 'YIELD_CULTURE';

UPDATE Buildings
SET Help = 'TXT_KEY_BUILDING_STELE_HELP'
WHERE Type = 'BUILDING_STELE';

-- Theodora -- Basilica UB (Replace Dromon)


UPDATE Buildings
SET GreatWorkSlotType = 'GREAT_WORK_SLOT_MUSIC'
WHERE Type = 'BUILDING_BASILICA';

UPDATE Buildings
SET GreatWorkCount = '1'
WHERE Type = 'BUILDING_BASILICA';

UPDATE Traits
SET AnyBelief = '1'
WHERE Type = 'TRAIT_EXTRA_BELIEF';

UPDATE Traits
SET AlwaysReligion = '1'
WHERE Type = 'TRAIT_EXTRA_BELIEF';

UPDATE Traits
SET FaithCostModifier = '-15'
WHERE Type = 'TRAIT_EXTRA_BELIEF';

UPDATE Traits
SET GPFaithPurchaseEra = 'ERA_CLASSICAL'
WHERE Type = 'TRAIT_EXTRA_BELIEF';


-- William -- Change Polder (buildable on water) -- New Trait
UPDATE Traits
SET LuxuryHappinessRetention = '0'
WHERE Type = 'TRAIT_LUXURY_RETENTION';

UPDATE Improvements
SET RequiresFeature = '1'
WHERE Type = 'IMPROVEMENT_POLDER';

DELETE FROM Improvement_Yields
WHERE ImprovementType = 'IMPROVEMENT_POLDER';

DELETE FROM Improvement_ValidFeatures
WHERE ImprovementType = 'IMPROVEMENT_POLDER' AND FeatureType = 'FEATURE_FLOOD_PLAINS';

-- Maria Theresa -- Coffee House +2 Production, +2 Food.

UPDATE Defines SET Value = '10' WHERE Name = 'MINOR_CIV_BUYOUT_TURNS';

UPDATE Traits
SET DiplomaticMarriage = '1'
WHERE Type = 'TRAIT_ANNEX_CITY_STATE';

UPDATE Traits
SET AbleToAnnexCityStates = '0'
WHERE Type = 'TRAIT_ANNEX_CITY_STATE';

UPDATE Traits
SET MinorQuestYieldModifier = '50'
WHERE Type = 'TRAIT_ANNEX_CITY_STATE';

UPDATE Buildings
SET Happiness = '1'
WHERE Type = 'BUILDING_COFFEE_HOUSE';

UPDATE Buildings
SET BuildingProductionModifier = '0'
WHERE Type = 'BUILDING_COFFEE_HOUSE';

UPDATE Buildings
SET GreatPeopleRateModifier = '33'
WHERE Type = 'BUILDING_COFFEE_HOUSE';

UPDATE Buildings
SET FoodKept = '15'
WHERE Type = 'BUILDING_COFFEE_HOUSE';

UPDATE Building_YieldChanges
SET Yield = '3'
WHERE BuildingType = 'BUILDING_COFFEE_HOUSE';

UPDATE Building_YieldChanges
SET YieldType = 'YIELD_FOOD'
WHERE BuildingType = 'BUILDING_COFFEE_HOUSE';

UPDATE Civilization_BuildingClassOverrides
SET BuildingClassType = 'BUILDINGCLASS_GROCER'
WHERE BuildingType = 'BUILDING_COFFEE_HOUSE';

UPDATE Buildings
SET BuildingClass = 'BUILDINGCLASS_GROCER'
WHERE Type = 'BUILDING_COFFEE_HOUSE';

UPDATE Buildings
SET SpecialistType = 'SPECIALIST_MERCHANT'
WHERE Type = 'BUILDING_COFFEE_HOUSE';

UPDATE Buildings
SET PrereqTech = 'TECH_CHEMISTRY'
WHERE Type = 'BUILDING_COFFEE_HOUSE';

-- Maya -- Move Pyramid to Agriculture, Bring UA back to Mathematics

UPDATE Traits
SET PrereqTech = 'TECH_MATHEMATICS'
WHERE Type = 'TRAIT_LONG_COUNT';

-- Buff Atlatl, move to Classical Age
-- Kuna

INSERT INTO ArtDefine_LandmarkTypes(Type, LandmarkType, FriendlyName)
SELECT 'ART_DEF_IMPROVEMENT_KUNA', 'Improvement', 'Kuna';

INSERT INTO ArtDefine_Landmarks (Era, State, Scale, ImprovementType, LayoutHandler, ResourceType, Model, TerrainContour) VALUES
('Any', 'UnderConstruction', 1.0, 'ART_DEF_IMPROVEMENT_KUNA', 'SNAPSHOT', 'ART_DEF_RESOURCE_ALL',       'hb_kuna.fxsxml', 1 ),
('Any', 'Constructed', 1.0, 'ART_DEF_IMPROVEMENT_KUNA', 'SNAPSHOT', 'ART_DEF_RESOURCE_ALL',     'kuna.fxsxml', 1),
('Any', 'Pillaged', 1.0, 'ART_DEF_IMPROVEMENT_KUNA', 'SNAPSHOT', 'ART_DEF_RESOURCE_ALL',        'pl_kuna.fxsxml', 1);
DELETE FROM Buildings
WHERE Type = 'BUILDING_MAYA_PYRAMID';

DELETE FROM Building_YieldChanges
WHERE BuildingType = 'BUILDING_MAYA_PYRAMID';

DELETE FROM Civilization_BuildingClassOverrides
WHERE BuildingType = 'BUILDING_MAYA_PYRAMID';

-- NEW

INSERT INTO Beliefs
	(Type, Description, ShortDescription, Pantheon, CivilizationType)
VALUES
	('BELIEF_MORRIGAN', 'TXT_KEY_BELIEF_MORRIGAN', 'TXT_KEY_BELIEF_MORRIGAN_SHORT', '1', 'CIVILIZATION_CELTS'),
	('BELIEF_EPONA', 'TXT_KEY_BELIEF_EPONA', 'TXT_KEY_BELIEF_EPONA_SHORT', '1', 'CIVILIZATION_CELTS'),
	('BELIEF_NUADA', 'TXT_KEY_BELIEF_NUADA', 'TXT_KEY_BELIEF_NUADA_SHORT', '1', 'CIVILIZATION_CELTS'),
	('BELIEF_CERNUNNOS', 'TXT_KEY_BELIEF_CERNUNNOS', 'TXT_KEY_BELIEF_CERNUNNOS_SHORT', '1', 'CIVILIZATION_CELTS'),
	('BELIEF_LUGH', 'TXT_KEY_BELIEF_LUGH', 'TXT_KEY_BELIEF_LUGH_SHORT', '1', 'CIVILIZATION_CELTS'),
	('BELIEF_RHIANNON', 'TXT_KEY_BELIEF_RHIANNON', 'TXT_KEY_BELIEF_RHIANNON_SHORT', '1', 'CIVILIZATION_CELTS'),
	('BELIEF_MANANNAN', 'TXT_KEY_BELIEF_MANANNAN', 'TXT_KEY_BELIEF_MANANNAN_SHORT', '1', 'CIVILIZATION_CELTS'),
	('BELIEF_OGMA', 'TXT_KEY_BELIEF_OGMA', 'TXT_KEY_BELIEF_OGMA_SHORT', '1', 'CIVILIZATION_CELTS'),
	('BELIEF_BRAN', 'TXT_KEY_BELIEF_BRAN', 'TXT_KEY_BELIEF_BRAN_SHORT', '1', 'CIVILIZATION_CELTS'),
	('BELIEF_DAGDA', 'TXT_KEY_BELIEF_DAGDA', 'TXT_KEY_BELIEF_DAGDA_SHORT', '1', 'CIVILIZATION_CELTS');

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_MORRIGAN', 'BUILDINGCLASS_CIRCUS', 'YIELD_GREAT_GENERAL_POINTS', 2),
	('BELIEF_EPONA', 'BUILDINGCLASS_CIRCUS', 'YIELD_CULTURE_LOCAL', 5),
	('BELIEF_NUADA', 'BUILDINGCLASS_CIRCUS', 'YIELD_GOLD', 5),
	('BELIEF_CERNUNNOS', 'BUILDINGCLASS_CIRCUS', 'YIELD_CULTURE', 2),
	('BELIEF_LUGH', 'BUILDINGCLASS_CIRCUS', 'YIELD_PRODUCTION', 3),
	('BELIEF_RHIANNON', 'BUILDINGCLASS_CIRCUS', 'YIELD_GOLDEN_AGE_POINTS', 5),
	('BELIEF_MANANNAN', 'BUILDINGCLASS_CIRCUS', 'YIELD_GREAT_ADMIRAL_POINTS', 2),
	('BELIEF_OGMA', 'BUILDINGCLASS_CIRCUS', 'YIELD_SCIENCE', 4),
	('BELIEF_BRAN', 'BUILDINGCLASS_CIRCUS', 'YIELD_FOOD', 5);

INSERT INTO Belief_BuildingClassHappiness
	(BeliefType, BuildingClassType, Happiness)
VALUES
	('BELIEF_DAGDA', 'BUILDINGCLASS_CIRCUS', 2);

UPDATE Beliefs
SET CityRangeStrikeModifier = '25'
WHERE Type = 'BELIEF_BRAN';

UPDATE Beliefs
SET CityGrowthModifier = '25'
WHERE Type = 'BELIEF_BRAN';

UPDATE Beliefs
SET HappinessPerCity = '1'
WHERE Type = 'BELIEF_BRAN';

INSERT INTO Belief_YieldPerBirth
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_BRAN', 'YIELD_CULTURE', 	15),
	('BELIEF_BRAN', 'YIELD_GOLD', 		15);

UPDATE Beliefs
SET FriendlyHealChange = '5'
WHERE Type = 'BELIEF_DAGDA';
	
INSERT INTO Belief_YieldFromKills
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_MORRIGAN', 'YIELD_GOLD', 200),
	('BELIEF_MORRIGAN', 'YIELD_CULTURE', 200),
	('BELIEF_MORRIGAN', 'YIELD_GOLDEN_AGE_POINTS', 200);
	
INSERT INTO Belief_YieldFromPillageGlobal
	(BeliefType, YieldType, Yield, 	IsEraScaling)
VALUES
	('BELIEF_MORRIGAN', 'YIELD_GOLD', 		20, 	1),
	('BELIEF_MORRIGAN', 'YIELD_CULTURE', 		20, 	1),
	('BELIEF_MORRIGAN', 'YIELD_GOLDEN_AGE_POINTS', 	20, 	1);

INSERT INTO Belief_CityYieldChanges
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_EPONA', 'YIELD_CULTURE_LOCAL', 3);

INSERT INTO Belief_YieldPerBorderGrowth
	(BeliefType, YieldType, Yield, IsEraScaling)
VALUES
	('BELIEF_EPONA', 'YIELD_SCIENCE', 10, 1),
	('BELIEF_EPONA', 'YIELD_CULTURE', 10, 1),
	('BELIEF_EPONA', 'YIELD_FOOD', 10, 1);

INSERT INTO Belief_ImprovementYieldChanges
	(BeliefType, ImprovementType, YieldType, Yield)
VALUES
	('BELIEF_CERNUNNOS', 'IMPROVEMENT_PLANTATION', 'YIELD_CULTURE', 1),
	('BELIEF_CERNUNNOS', 'IMPROVEMENT_CAMP', 'YIELD_CULTURE', 1),
	('BELIEF_CERNUNNOS', 'IMPROVEMENT_PLANTATION', 'YIELD_SCIENCE', 1),
	('BELIEF_CERNUNNOS', 'IMPROVEMENT_CAMP', 'YIELD_SCIENCE', 1);

INSERT INTO Belief_FeatureYieldChanges
	(BeliefType, FeatureType, YieldType, Yield)
VALUES
	('BELIEF_CERNUNNOS', 'FEATURE_FOREST', 'YIELD_FOOD', 1),
	('BELIEF_CERNUNNOS', 'FEATURE_FOREST', 'YIELD_GOLD', 1),
	('BELIEF_CERNUNNOS', 'FEATURE_JUNGLE', 'YIELD_PRODUCTION', 1),
	('BELIEF_CERNUNNOS', 'FEATURE_JUNGLE', 'YIELD_FOOD', 1);

UPDATE Beliefs
SET WonderProductionModifier = '10'
WHERE Type = 'BELIEF_LUGH';
	
INSERT INTO Belief_YieldChangeAnySpecialist
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_LUGH', 'YIELD_CULTURE', 2),
	('BELIEF_LUGH', 'YIELD_GOLD', 2),
	('BELIEF_LUGH', 'YIELD_FOOD', 2),
	('BELIEF_LUGH', 'YIELD_SCIENCE', 2);

INSERT INTO Belief_YieldFromWLTKD
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_NUADA', 'YIELD_GOLD', 15),
	('BELIEF_NUADA', 'YIELD_SCIENCE', 15);

INSERT INTO Belief_YieldPerLux
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_NUADA', 'YIELD_GOLD', 3),
	('BELIEF_NUADA', 'YIELD_SCIENCE', 3),
	('BELIEF_NUADA', 'YIELD_GOLDEN_AGE_POINTS', 3);
	
INSERT INTO Belief_YieldPerActiveTR
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_NUADA', 'YIELD_GOLD', 3),
	('BELIEF_NUADA', 'YIELD_SCIENCE', 3),
	('BELIEF_NUADA', 'YIELD_GOLDEN_AGE_POINTS', 3);

INSERT INTO Belief_YieldPerFollowingCity
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_RHIANNON', 'YIELD_CULTURE', 2);

UPDATE Beliefs
SET RequiresResource = '1'
WHERE Type = 'BELIEF_RHIANNON';

UPDATE Beliefs
SET RequiresImprovement = '1'
WHERE Type = 'BELIEF_RHIANNON';

INSERT INTO Belief_ImprovementYieldChanges
	(BeliefType, ImprovementType, YieldType, Yield)
VALUES
	('BELIEF_RHIANNON', 'IMPROVEMENT_CAMP', 'YIELD_PRODUCTION', 1),
	('BELIEF_RHIANNON', 'IMPROVEMENT_CAMP', 'YIELD_GOLD', 1),
	('BELIEF_RHIANNON', 'IMPROVEMENT_MINE', 'YIELD_PRODUCTION', 1),
	('BELIEF_RHIANNON', 'IMPROVEMENT_MINE', 'YIELD_GOLD', 1),
	('BELIEF_RHIANNON', 'IMPROVEMENT_FARM', 'YIELD_PRODUCTION', 1),
	('BELIEF_RHIANNON', 'IMPROVEMENT_FARM', 'YIELD_GOLD', 1),
	('BELIEF_RHIANNON', 'IMPROVEMENT_PASTURE', 'YIELD_PRODUCTION', 1),
	('BELIEF_RHIANNON', 'IMPROVEMENT_PASTURE', 'YIELD_GOLD', 1),
	('BELIEF_RHIANNON', 'IMPROVEMENT_PLANTATION', 'YIELD_PRODUCTION', 1),
	('BELIEF_RHIANNON', 'IMPROVEMENT_PLANTATION', 'YIELD_GOLD', 1),
	('BELIEF_RHIANNON', 'IMPROVEMENT_FISHING_BOATS', 'YIELD_PRODUCTION', 1),
	('BELIEF_RHIANNON', 'IMPROVEMENT_FISHING_BOATS', 'YIELD_GOLD', 1),
	('BELIEF_RHIANNON', 'IMPROVEMENT_QUARRY', 'YIELD_PRODUCTION', 1),
	('BELIEF_RHIANNON', 'IMPROVEMENT_QUARRY', 'YIELD_GOLD', 1),
	('BELIEF_RHIANNON', 'IMPROVEMENT_WELL', 'YIELD_PRODUCTION', 1),
	('BELIEF_RHIANNON', 'IMPROVEMENT_WELL', 'YIELD_GOLD', 1),
	('BELIEF_RHIANNON', 'IMPROVEMENT_OFFSHORE_PLATFORM', 'YIELD_PRODUCTION', 1),
	('BELIEF_RHIANNON', 'IMPROVEMENT_OFFSHORE_PLATFORM', 'YIELD_GOLD', 1);

INSERT INTO Belief_CoastalCityYieldChanges
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_MANANNAN', 'YIELD_FOOD', 3),
	('BELIEF_MANANNAN', 'YIELD_PRODUCTION', 3),
	('BELIEF_MANANNAN', 'YIELD_GOLD', 3);

INSERT INTO Belief_CityYieldPerXTerrainTimes100
	(BeliefType, TerrainType, YieldType, Yield)
VALUES
	('BELIEF_MANANNAN', 'TERRAIN_OCEAN', 'YIELD_PRODUCTION', 50),
	('BELIEF_MANANNAN', 'TERRAIN_COAST', 'YIELD_PRODUCTION', 50),
	('BELIEF_MANANNAN', 'TERRAIN_OCEAN', 'YIELD_GOLD', 50),
	('BELIEF_MANANNAN', 'TERRAIN_COAST', 'YIELD_GOLD', 50);
	
INSERT INTO Belief_YieldPerPop
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_OGMA', 'YIELD_SCIENCE', 4);

INSERT INTO Belief_GreatWorkYieldChanges
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_OGMA', 'YIELD_PRODUCTION', 1);
	
INSERT INTO Belief_GreatPersonPoints
	(BeliefType, GreatPersonType, Value)
VALUES
	('BELIEF_OGMA', 'GREATPERSON_SCIENTIST', 	3),
	('BELIEF_OGMA', 'GREATPERSON_ARTIST', 		3);

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_DAGDA', 'BUILDINGCLASS_PALACE', 'YIELD_GOLD', 1),
	('BELIEF_DAGDA', 'BUILDINGCLASS_PALACE', 'YIELD_CULTURE', 1),
	('BELIEF_DAGDA', 'BUILDINGCLASS_PALACE', 'YIELD_PRODUCTION', 1),
	('BELIEF_DAGDA', 'BUILDINGCLASS_PALACE', 'YIELD_SCIENCE', 1),
	('BELIEF_OGMA', 'BUILDINGCLASS_PALACE', 'YIELD_SCIENCE', 3),
	('BELIEF_OGMA', 'BUILDINGCLASS_PALACE', 'YIELD_CULTURE', 3);

INSERT INTO Belief_YieldPerXFollowers
	(BeliefType, YieldType, PerXFollowers)
VALUES
	('BELIEF_DAGDA', 'YIELD_GOLD', 4),
	('BELIEF_DAGDA', 'YIELD_CULTURE', 4),
	('BELIEF_DAGDA', 'YIELD_PRODUCTION', 4),
	('BELIEF_DAGDA', 'YIELD_SCIENCE', 4);


UPDATE Beliefs
SET Tooltip = 'TXT_KEY_BELIEF_MORRIGAN'
WHERE Type = 'BELIEF_MORRIGAN';

UPDATE Beliefs
SET Tooltip = 'TXT_KEY_BELIEF_EPONA'
WHERE Type = 'BELIEF_EPONA';

UPDATE Beliefs
SET Tooltip = 'TXT_KEY_BELIEF_NUADA'
WHERE Type = 'BELIEF_NUADA';

UPDATE Beliefs
SET Tooltip = 'TXT_KEY_BELIEF_CERNUNNOS'
WHERE Type = 'BELIEF_CERNUNNOS';

UPDATE Beliefs
SET Tooltip = 'TXT_KEY_BELIEF_LUGH'
WHERE Type = 'BELIEF_LUGH';

UPDATE Beliefs
SET Tooltip = 'TXT_KEY_BELIEF_RHIANNON'
WHERE Type = 'BELIEF_RHIANNON';

UPDATE Beliefs
SET Tooltip = 'TXT_KEY_BELIEF_MANANNAN'
WHERE Type = 'BELIEF_MANANNAN';

UPDATE Beliefs
SET Tooltip = 'TXT_KEY_BELIEF_OGMA'
WHERE Type = 'BELIEF_OGMA';

UPDATE Beliefs
SET Tooltip = 'TXT_KEY_BELIEF_BRAN'
WHERE Type = 'BELIEF_BRAN';

UPDATE Beliefs
SET Tooltip = 'TXT_KEY_BELIEF_DAGDA'
WHERE Type = 'BELIEF_DAGDA';

INSERT INTO Trait_YieldFromOwnPantheon
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_FAITH_FROM_NATURE', 'YIELD_FAITH', 3);

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CEILIDH_HALL', 'YIELD_FAITH', 2),
	('BUILDING_BASILICA', 'YIELD_CULTURE', 2),
	('BUILDING_BASILICA', 'YIELD_FAITH', 3);

INSERT INTO Building_InstantYield
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CEILIDH_HALL', 'YIELD_CULTURE', 150);

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_POLDER_WATER', 'YIELD_FOOD', 1),
	('IMPROVEMENT_POLDER_WATER', 'YIELD_GOLD', 2),
	('IMPROVEMENT_POLDER_WATER', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_POLDER', 'YIELD_FOOD', 1),
	('IMPROVEMENT_POLDER', 'YIELD_GOLD', 2),
	('IMPROVEMENT_POLDER', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_EKI', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_EKI', 'YIELD_FOOD', 1),
	('IMPROVEMENT_EKI', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_KUNA', 'YIELD_SCIENCE', 1),
	('IMPROVEMENT_KUNA', 'YIELD_FAITH', 2);


INSERT INTO Improvement_AdjacentImprovementYieldChanges
	(ImprovementType, OtherImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_POLDER_WATER', 'IMPROVEMENT_TRADING_POST', 'YIELD_GOLD', 1),
	('IMPROVEMENT_POLDER_WATER', 'IMPROVEMENT_CUSTOMS_HOUSE', 'YIELD_GOLD', 1),
	('IMPROVEMENT_POLDER', 'IMPROVEMENT_TRADING_POST', 'YIELD_GOLD', 1),
	('IMPROVEMENT_POLDER', 'IMPROVEMENT_CUSTOMS_HOUSE', 'YIELD_GOLD', 1);

INSERT INTO Trait_YieldFromImport
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_LUXURY_RETENTION', 'YIELD_GOLD', 3),
	('TRAIT_LUXURY_RETENTION', 'YIELD_CULTURE', 3);

INSERT INTO Trait_YieldFromExport
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_LUXURY_RETENTION', 'YIELD_CULTURE', 3),
	('TRAIT_LUXURY_RETENTION', 'YIELD_GOLD', 3);

UPDATE Traits
SET ImportsCountTowardsMonopolies = 'true'
WHERE Type = 'TRAIT_LUXURY_RETENTION';
	

INSERT INTO Trait_YieldFromSettle
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_PHOENICIAN_HERITAGE', 'YIELD_GOLD', 125);

INSERT INTO Trait_TerrainClaimBoost
	(TraitType, TerrainType)
VALUES
	('TRAIT_RAZE_AND_HORSES', 'TERRAIN_GRASS'),
	('TRAIT_RAZE_AND_HORSES', 'TERRAIN_DESERT'),
	('TRAIT_RAZE_AND_HORSES', 'TERRAIN_TUNDRA'),
	('TRAIT_RAZE_AND_HORSES', 'TERRAIN_SNOW'),
	('TRAIT_RAZE_AND_HORSES', 'TERRAIN_PLAINS');

INSERT INTO Building_GoldenAgeYieldMod
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_STELE', 'YIELD_FAITH', 25);

INSERT INTO Building_YieldFromYieldPercent
	(BuildingType, YieldIn, YieldOut, Value)
VALUES
	('BUILDING_COFFEE_HOUSE', 'YIELD_CULTURE', 'YIELD_SCIENCE', 10);

INSERT INTO Building_ClassesNeededInCity
	(BuildingType, BuildingClassType)
VALUES
	('BUILDING_COFFEE_HOUSE', 'BUILDINGCLASS_AQUEDUCT');

INSERT INTO Building_ClassesNeededInCity
	(BuildingType, BuildingClassType)
VALUES
	('BUILDING_SKOLA', 'BUILDINGCLASS_UNIVERSITY'),
	('BUILDING_BASILICA', 'BUILDINGCLASS_SHRINE'),
	('BUILDING_GREAT_COTHON', 'BUILDINGCLASS_MARKET');

INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_SWEDEN', 'BUILDINGCLASS_PUBLIC_SCHOOL', 'BUILDING_SKOLA'),
	('CIVILIZATION_BYZANTIUM', 'BUILDINGCLASS_TEMPLE', 'BUILDING_BASILICA'),
	('CIVILIZATION_CARTHAGE', 'BUILDINGCLASS_NATIONAL_TREASURY', 'BUILDING_GREAT_COTHON');

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SKOLA', 'YIELD_SCIENCE', 100),
	('BUILDING_SKOLA', 'YIELD_CULTURE', 34),
	('BUILDING_BASILICA', 'YIELD_FAITH', 34);

INSERT INTO Building_ResourceYieldChanges
	(BuildingType, ResourceType, YieldType, Yield)
VALUES
	('BUILDING_BASILICA', 'RESOURCE_INCENSE', 'YIELD_CULTURE', 1),
	('BUILDING_BASILICA', 'RESOURCE_INCENSE', 'YIELD_GOLD', 1),
	('BUILDING_BASILICA', 'RESOURCE_WINE', 'YIELD_CULTURE', 1),
	('BUILDING_BASILICA', 'RESOURCE_WINE', 'YIELD_GOLD', 1);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_GREAT_COTHON', 'BUILDINGCLASS_HARBOR', 'YIELD_PRODUCTION', 3),
	('BUILDING_GREAT_COTHON', 'BUILDINGCLASS_LIGHTHOUSE', 'YIELD_CULTURE', 2);

-- New Improvements
INSERT INTO Builds
	(Type, Time, ImprovementType, PrereqTech, Description, Help, Recommendation, EntityEvent, HotKey, OrderPriority, IconIndex, IconAtlas)
VALUES
	('BUILD_EKI', 800, 'IMPROVEMENT_EKI', 'TECH_ARCHERY', 'TXT_KEY_BUILD_EKI', 'TXT_KEY_BUILD_EKI_HELP', 'TXT_KEY_BUILD_EKI_REC', 'ENTITY_EVENT_BUILD', 'KB_E', 1, 0, 'UNIT_ACTION_EKI'),
	('BUILD_KUNA', 700, 'IMPROVEMENT_KUNA', 'TECH_MASONRY', 'TXT_KEY_BUILD_KUNA', 'TXT_KEY_BUILD_KUNA_HELP', 'TXT_KEY_BUILD_KUNA_REC', 'ENTITY_EVENT_BUILD', 'KB_E', 1, 0, 'UNIT_ACTION_KUNA');

INSERT INTO Builds
		(Type,					Water,	CanBeEmbarked,	ShowInPedia, 	ShowInTechTree, ImprovementType,			HotKey, CtrlDown, PrereqTech, Time, Description, Help, Recommendation, EntityEvent, OrderPriority, IconIndex, IconAtlas)
SELECT	'BUILD_POLDER_WATER',	1,		1,				0, 				0, 				'IMPROVEMENT_POLDER_WATER',	HotKey, CtrlDown, PrereqTech, Time, Description, Help, Recommendation, EntityEvent, OrderPriority, IconIndex, IconAtlas
FROM Builds WHERE Type = 'BUILD_POLDER';

INSERT INTO Builds
		(Type,						Water, 	CanBeEmbarked,	ShowInPedia, 	ShowInTechTree, HotKey, CtrlDown, Repair, Time, ImprovementType, Description, Help, Recommendation, EntityEvent, OrderPriority, IconIndex, IconAtlas)
SELECT 	'BUILD_REPAIR_EMBARKED', 	1, 		1, 				0, 				0, 				HotKey, CtrlDown, Repair, Time, ImprovementType, Description, Help, Recommendation, EntityEvent, OrderPriority, IconIndex, IconAtlas
FROM Builds WHERE Type = 'BUILD_REPAIR';

INSERT INTO BuildFeatures
	(BuildType, FeatureType, PrereqTech, Time, Production, Remove)
VALUES
	('BUILD_EKI', 'FEATURE_JUNGLE', 'TECH_CALENDAR', 400, 20, 1),
	('BUILD_EKI', 'FEATURE_FOREST', 'TECH_MINING', 300, 30, 1),
	('BUILD_EKI', 'FEATURE_MARSH', 'TECH_IRON_WORKING', 600, 0, 1);

INSERT INTO Improvements
	(Type, Description, Civilopedia, Help, ArtDefineTag, SpecificCivRequired, CivilizationType, InAdjacentFriendly, NoFreshWater, RequiresFlatlands, PortraitIndex, PillageGold, IconAtlas, NoTwoAdjacent, RequiresFeature)
VALUES
	('IMPROVEMENT_EKI', 'TXT_KEY_IMPROVEMENT_EKI', 'TXT_KEY_CIV5_IMPROVEMENTS_EKI_TEXT', 'TXT_KEY_CIV5_IMPROVEMENTS_EKI_HELP', 'ART_DEF_IMPROVEMENT_EKI', 1, 'CIVILIZATION_HUNS', 1, 1, 1, 0, 5, 'TERRAIN_IMPROVEMENT_EKI', 0, 0),
	('IMPROVEMENT_KUNA', 'TXT_KEY_IMPROVEMENT_KUNA', 'TXT_KEY_CIV5_IMPROVEMENTS_KUNA_TEXT', 'TXT_KEY_CIV5_IMPROVEMENTS_KUNA_HELP', 'ART_DEF_IMPROVEMENT_KUNA', 1, 'CIVILIZATION_MAYA', 0, 0, 0, 0, 14, 'TERRAIN_IMPROVEMENT_KUNA', 1, 1);

INSERT INTO Improvements
		(Type,						GoldMaintenance, Description, Civilopedia, Help, ArtDefineTag, DestroyedWhenPillaged, Permanent, PillageGold, BuildableOnResources, PortraitIndex, IconAtlas)
SELECT	'IMPROVEMENT_POLDER_WATER',	GoldMaintenance, Description, Civilopedia, Help, ArtDefineTag, DestroyedWhenPillaged, Permanent, PillageGold, BuildableOnResources, PortraitIndex, IconAtlas
FROM Improvements WHERE Type = 'IMPROVEMENT_POLDER';

UPDATE Improvements
SET	Water = 0,
	CoastMakesValid = 1,
	RequiresXAdjacentLand = 3,
	AllowsWalkWater = 1,
	MakesPassable = 1
WHERE Type = 'IMPROVEMENT_POLDER_WATER';

INSERT INTO Improvement_ValidTerrains
	(ImprovementType, TerrainType)
VALUES
	('IMPROVEMENT_EKI', 'TERRAIN_GRASS'),
	('IMPROVEMENT_EKI', 'TERRAIN_PLAINS'),
	('IMPROVEMENT_EKI', 'TERRAIN_DESERT'),
	('IMPROVEMENT_EKI', 'TERRAIN_TUNDRA'),
	('IMPROVEMENT_EKI', 'TERRAIN_SNOW');

INSERT INTO Improvement_ValidFeatures
	(ImprovementType, FeatureType)
VALUES
	('IMPROVEMENT_KUNA', 'FEATURE_FOREST'),
	('IMPROVEMENT_KUNA', 'FEATURE_JUNGLE');
	
INSERT INTO Improvement_YieldAdjacentTwoSameType
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_EKI', 'YIELD_PRODUCTION', 1);

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_WORKER', 'BUILD_EKI'),
	('UNIT_WORKER', 'BUILD_KUNA'),
	('UNIT_WORKER', 'BUILD_REPAIR_EMBARKED');
-- Give build via traits table so that it doesn't appear in the civ select screen
INSERT INTO Trait_BuildsUnitClasses	
	(TraitType, UnitClassType, BuildType)
VALUES	
	('TRAIT_LUXURY_RETENTION', 'UNITCLASS_WORKER', 'BUILD_POLDER_WATER');
