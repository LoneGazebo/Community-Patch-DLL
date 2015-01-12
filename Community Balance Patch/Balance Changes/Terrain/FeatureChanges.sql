-- Defense Changes

UPDATE Features
SET Defense = '5'
WHERE Type = 'FEATURE_MARSH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE Features
SET Defense = '5'
WHERE Type = 'FEATURE_OASIS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE Features
SET Defense = '5'
WHERE Type = 'FEATURE_FLOOD_PLAINS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE Features
SET Defense = '5'
WHERE Type = 'FEATURE_FALLOUT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

--



--
-- General data
--

UPDATE Features SET Impassable=0, Movement=2, Defense=10 WHERE Type IN (
	'FEATURE_CRATER'			,
	'FEATURE_GEYSER'			,
	'FEATURE_FOUNTAIN_YOUTH'	,
	'FEATURE_EL_DORADO'			,
	'FEATURE_MESA'				,
	'FEATURE_REEF'				,
	'FEATURE_ULURU'				,
	'FEATURE_SOLOMONS_MINES'	
);

UPDATE Natural_Wonder_Placement SET ChangeCoreTileToMountain=0 WHERE NaturalWonderType IN (
	'FEATURE_CRATER'			,
	'FEATURE_GEYSER'			,
	'FEATURE_FOUNTAIN_YOUTH'	,
	'FEATURE_EL_DORADO'			,
	'FEATURE_MESA'				,
	'FEATURE_REEF'				,
	'FEATURE_ULURU'				
);

UPDATE Features SET Impassable=1 WHERE Type IN (
	'FEATURE_LAKE_VICTORIA'		
);

UPDATE Natural_Wonder_Placement SET ChangeCoreTileToMountain=1 WHERE NaturalWonderType IN (
	'FEATURE_LAKE_VICTORIA'		
);


UPDATE Natural_Wonder_Placement SET OccurrenceFrequency = 10 WHERE OccurrenceFrequency <> 0;

-- More unique than the numerous mountains
UPDATE Natural_Wonder_Placement SET OccurrenceFrequency = 20 WHERE NaturalWonderType IN (
	'FEATURE_GIBRALTAR'		,
	'FEATURE_REEF'			,
	'FEATURE_VOLCANO'		,
	'FEATURE_GEYSER'		,
	'FEATURE_MESA'			,
	'FEATURE_FOUNTAIN_YOUTH',
	'FEATURE_LAKE_VICTORIA'	
);


-- Great Improvements and Unique Improvements can build on resources, and recieve yields like the basic improvement they replace

/*
INSERT OR REPLACE INTO Improvement_ResourceTypes(ImprovementType, ResourceType) 
SELECT improve.Type, res.Type
FROM Improvements improve, Resources res
WHERE (
	improve.SpecificCivRequired = 1
	AND NOT res.TechCityTrade = 'TECH_SAILING'
	AND NOT improve.Type = 'IMPROVEMENT_POLDER'
);
*/

INSERT OR REPLACE INTO Improvement_ResourceTypes(ImprovementType, ResourceType) 
SELECT improve.Type, res.Type
FROM Improvements improve, Resources res
WHERE ( improve.CreatedByGreatPerson = 1
	AND res.OnlyMinorCivs = 0
	AND res.TechCityTrade <> 'TECH_SAILING'
	AND NOT res.CivilizationType
);

INSERT OR REPLACE INTO Improvement_ResourceTypes(ImprovementType, ResourceType) 
SELECT improve.Type, res.Type
FROM Improvements improve, Resources res
WHERE ( improve.Type = 'IMPROVEMENT_FORT'
	AND res.OnlyMinorCivs = 0
	AND res.TechCityTrade <> 'TECH_SAILING'
	AND NOT res.CivilizationType
);

INSERT OR REPLACE INTO Improvement_ResourceType_Yields(ImprovementType, ResourceType, YieldType, Yield) 
SELECT improve.Type, resTypes.ResourceType, resYields.YieldType, resYields.Yield 
FROM Improvements improve, Improvement_ResourceTypes resTypes, Improvement_ResourceType_Yields resYields, Improvements impBasic
WHERE improve.Type = 'IMPROVEMENT_FORT'
AND resTypes.ImprovementType = improve.Type
AND resTypes.ResourceType = resYields.ResourceType
AND resYields.ImprovementType = impBasic.Type
AND NOT impBasic.Water = 1;

/*
INSERT OR REPLACE INTO Improvement_ResourceType_Yields(ImprovementType, ResourceType, YieldType, Yield) 
SELECT improve.Type, resTypes.ResourceType, resYields.YieldType, resYields.Yield 
FROM Improvements improve, Improvement_ResourceTypes resTypes, Improvement_ResourceType_Yields resYields, Improvements impBasic
WHERE improve.BuildableOnResources = 1
AND resTypes.ImprovementType = improve.Type
AND resTypes.ResourceType = resYields.ResourceType
AND resYields.ImprovementType = impBasic.Type
AND NOT impBasic.Water = 1;
*/

-- Buff jungles and forests
INSERT INTO BuildFeatures (BuildType, FeatureType, PrereqTech, Time, Remove)
VALUES ('BUILD_CAMP', 'FEATURE_MARSH', 'TECH_BRONZE_WORKING', '600', 1);

UPDATE BuildFeatures
SET Remove = '0'
WHERE BuildType = 'BUILD_PLANTATION';

-- Polder specific code lacking in the BuildFeatures table
INSERT INTO BuildFeatures (BuildType, FeatureType, PrereqTech, Time, Production, Remove)
VALUES ('BUILD_POLDER', 'FEATURE_JUNGLE', 'TECH_THE_WHEEL', '600', '',1);
INSERT INTO BuildFeatures (BuildType, FeatureType, PrereqTech, Time, Production, Remove)
VALUES ('BUILD_POLDER', 'FEATURE_FOREST', 'TECH_ANIMAL_HUSBANDRY', '400', '40', 1);

UPDATE BuildFeatures SET Remove = 1 WHERE  FeatureType = 'FEATURE_MARSH'  AND BuildType NOT IN ('BUILD_POLDER');
UPDATE BuildFeatures SET Time = 400 WHERE (FeatureType = 'FEATURE_JUNGLE' AND Time <> 0 AND Remove = 0);
UPDATE BuildFeatures SET Time = 600 WHERE (FeatureType = 'FEATURE_JUNGLE' AND Time <> 0 AND Remove = 1);
UPDATE BuildFeatures SET Time = 200 WHERE  BuildType   = 'BUILD_REMOVE_JUNGLE';
UPDATE BuildFeatures SET Production = 30 WHERE FeatureType = 'FEATURE_FOREST' AND Remove = 1;
UPDATE BuildFeatures SET Production = 20 WHERE FeatureType = 'FEATURE_JUNGLE' AND Remove = 1;
