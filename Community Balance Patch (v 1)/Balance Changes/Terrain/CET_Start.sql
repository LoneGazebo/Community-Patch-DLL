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


-- Add Great Person Improvements to Strategic Resources Yields
INSERT OR REPLACE INTO Improvement_ResourceType_Yields(ImprovementType, ResourceType, YieldType, Yield) 
SELECT improve.Type, resTypes.ResourceType, resYields.YieldType, resYields.Yield 
FROM Improvements improve, Improvement_ResourceTypes resTypes, Improvement_ResourceType_Yields resYields, Improvements impBasic
WHERE improve.BuildableOnResources = 1
AND resTypes.ImprovementType = improve.Type
AND resTypes.ResourceType = resYields.ResourceType
AND resYields.ImprovementType = impBasic.Type
AND NOT impBasic.Water = 1;


-- This CET_Start.sql data created by:
-- TechUnlocks tab of CEM_Details.xls spreadsheet (in mod folder).
UPDATE BuildFeatures SET PrereqTech = 'TECH_ANIMAL_HUSBANDRY' WHERE FeatureType = 'FEATURE_FOREST';
UPDATE BuildFeatures SET PrereqTech = 'TECH_THE_WHEEL' WHERE FeatureType = 'FEATURE_JUNGLE';
UPDATE BuildFeatures SET PrereqTech = 'TECH_BRONZE_WORKING' WHERE FeatureType = 'FEATURE_MARSH';

UPDATE Resources SET TechCityTrade = 'TECH_ARCHERY' WHERE TechCityTrade = 'TECH_TRAPPING';

-- Buff jungles and forests
INSERT INTO BuildFeatures (BuildType, FeatureType, PrereqTech, Time, Remove)
VALUES ('BUILD_CAMP', 'FEATURE_MARSH', 'TECH_BRONZE_WORKING', '600', 1);

-- Polder specific code lacking in the BuildFeatures table
INSERT INTO BuildFeatures (BuildType, FeatureType, PrereqTech, Time, Production, Remove)
VALUES ('BUILD_POLDER', 'FEATURE_JUNGLE', 'TECH_THE_WHEEL', '600', '',1);
INSERT INTO BuildFeatures (BuildType, FeatureType, PrereqTech, Time, Production, Remove)
VALUES ('BUILD_POLDER', 'FEATURE_FOREST', 'TECH_ANIMAL_HUSBANDRY', '400', '80', 1);

UPDATE BuildFeatures SET Remove = 1 WHERE FeatureType = 'FEATURE_MARSH' AND BuildType NOT IN ('BUILD_POLDER');
UPDATE BuildFeatures SET Remove = 0 WHERE FeatureType = 'FEATURE_JUNGLE' AND BuildType IN ('BUILD_PLANTATION');
UPDATE BuildFeatures SET Time = 400 WHERE (FeatureType = 'FEATURE_JUNGLE' AND Time <> 0 AND Remove = 0);
UPDATE BuildFeatures SET Time = 600 WHERE (FeatureType = 'FEATURE_JUNGLE' AND Time <> 0 AND Remove = 1);
UPDATE BuildFeatures SET Time = 200 WHERE BuildType = 'BUILD_REMOVE_JUNGLE';
UPDATE BuildFeatures SET Production = 80 WHERE FeatureType = 'FEATURE_FOREST' AND Remove = 1;


UPDATE LoadedFile SET Value=1 WHERE Type='CET_Start.sql';
