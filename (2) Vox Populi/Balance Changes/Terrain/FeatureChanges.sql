-- Removal of Forest/Jungle/Marsh moved back a bit
UPDATE Builds
SET PrereqTech = 'TECH_CALENDAR'
WHERE Type = 'BUILD_REMOVE_JUNGLE';

UPDATE Builds
SET Time = '400'
WHERE Type = 'BUILD_REMOVE_JUNGLE';

UPDATE Builds
SET PrereqTech = 'TECH_MINING', Time = '300'
WHERE Type = 'BUILD_REMOVE_FOREST';

INSERT INTO Build_TechTimeChanges
	(BuildType, TechType, TimeChange)
VALUES
	('BUILD_REMOVE_JUNGLE', 'TECH_MACHINERY', -200),
	('BUILD_REMOVE_JUNGLE', 'TECH_BIOLOGY', -200),

	('BUILD_REMOVE_FOREST', 'TECH_MACHINERY', -200),
	('BUILD_REMOVE_FOREST', 'TECH_BIOLOGY', -200),

	('BUILD_REMOVE_MARSH', 'TECH_MACHINERY', -200),
	('BUILD_REMOVE_MARSH', 'TECH_BIOLOGY', -200);

UPDATE Builds
SET PrereqTech = 'TECH_MASONRY', Time = '400'
WHERE Type = 'BUILD_REMOVE_MARSH';


-- Mining gets chop, but...
UPDATE BuildFeatures
SET PrereqTech = 'TECH_MINING'
WHERE FeatureType = 'FEATURE_FOREST';

-- Bronze working gets a chop valuation bonus
UPDATE Technologies
SET FeatureProductionModifier = '50'
WHERE Type = 'TECH_BRONZE_WORKING';

UPDATE Technologies
SET FeatureProductionModifier = '50'
WHERE Type = 'TECH_IRON_WORKING';


UPDATE BuildFeatures
SET PrereqTech = 'TECH_CALENDAR'
WHERE FeatureType = 'FEATURE_JUNGLE';

UPDATE BuildFeatures
SET PrereqTech = 'TECH_MASONRY'
WHERE FeatureType = 'FEATURE_MARSH';

-- Defense Changes

UPDATE Features
SET Defense = '5'
WHERE Type IN 
('FEATURE_MARSH',
'FEATURE_OASIS',
'FEATURE_FLOOD_PLAINS',
'FEATURE_FALLOUT');

-- Fallout 

UPDATE Features
SET TurnDamage = '15', ExtraTurnDamage = '-10'
WHERE Type = 'FEATURE_FALLOUT';

UPDATE BuildFeatures
SET Time = '400'
WHERE BuildType = 'BUILD_SCRUB_FALLOUT';

-- Buff jungles and forests
UPDATE BuildFeatures
SET Remove = '1'
WHERE BuildType = 'BUILD_TRADING_POST';

UPDATE BuildFeatures
SET Remove = '1'
WHERE BuildType = 'BUILD_FORT';

-- Polder specific code lacking in the BuildFeatures table
INSERT INTO BuildFeatures (BuildType, FeatureType, PrereqTech, Time, Production, Remove) VALUES 
('BUILD_CAMP', 'FEATURE_MARSH', 'TECH_IRON_WORKING', '600', 0, 1),
('BUILD_POLDER', 'FEATURE_JUNGLE', 'TECH_CALENDAR', '500', 20 ,1),
('BUILD_POLDER', 'FEATURE_FOREST', 'TECH_MINING', '300', 30, 1);

UPDATE BuildFeatures SET Remove = 1 WHERE  FeatureType = 'FEATURE_MARSH'  AND BuildType NOT IN ('BUILD_POLDER');
UPDATE BuildFeatures SET Time = 300 WHERE (FeatureType = 'FEATURE_JUNGLE' AND Time <> 0 AND Remove = 0);
UPDATE BuildFeatures SET Time = 500 WHERE (FeatureType = 'FEATURE_JUNGLE' AND Time <> 0 AND Remove = 1);
UPDATE BuildFeatures SET Time = 400 WHERE  BuildType   = 'BUILD_REMOVE_JUNGLE';
UPDATE BuildFeatures SET Time = 300 WHERE  BuildType   = 'BUILD_REMOVE_FOREST';
UPDATE BuildFeatures SET Production = 40 WHERE FeatureType = 'FEATURE_FOREST' AND Remove = 1;
UPDATE BuildFeatures SET Production = 40 WHERE FeatureType = 'FEATURE_JUNGLE' AND Remove = 1;

UPDATE BuildFeatures SET Time = 300 WHERE FeatureType = 'FEATURE_FOREST' AND Time > 0 AND Remove = 1;
UPDATE BuildFeatures SET Time = 400 WHERE FeatureType = 'FEATURE_JUNGLE' AND Time > 0 AND Remove = 1;
UPDATE BuildFeatures SET Time = 600 WHERE FeatureType = 'FEATURE_MARSH'  AND Time > 0 AND Remove = 1;

-- Additive Nature of Features
UPDATE Features
SET YieldNotAdditive = '0'
WHERE Type = 'FEATURE_FOREST';

-- Lake Buff

UPDATE Yields
SET LakeChange = '1'
WHERE Type = 'YIELD_PRODUCTION';

UPDATE Yields
SET LakeChange = '1'
WHERE Type = 'YIELD_FOOD';

-- All Feature_Yields are in NaturalWonderChanges

INSERT INTO Feature_TerrainBooleans
	(FeatureType, TerrainType)
VALUES
	('FEATURE_JUNGLE', 'TERRAIN_PLAINS');
