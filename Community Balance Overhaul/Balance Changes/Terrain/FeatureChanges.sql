-- Removal of Forest/Jungle/Marsh moved back a bit
UPDATE Builds
SET PrereqTech = 'TECH_CALENDAR'
WHERE Type = 'BUILD_REMOVE_JUNGLE';

UPDATE Builds
SET Time = '400'
WHERE Type = 'BUILD_REMOVE_JUNGLE';

INSERT INTO Build_TechTimeChanges
	(BuildType, TechType, TimeChange)
VALUES
	('BUILD_REMOVE_JUNGLE', 'TECH_MACHINERY', -200),
	('BUILD_REMOVE_JUNGLE', 'TECH_BIOLOGY', -200);

UPDATE Builds
SET PrereqTech = 'TECH_MINING'
WHERE Type = 'BUILD_REMOVE_FOREST';

UPDATE Builds
SET Time = '300'
WHERE Type = 'BUILD_REMOVE_FOREST';

INSERT INTO Build_TechTimeChanges
	(BuildType, TechType, TimeChange)
VALUES
	('BUILD_REMOVE_FOREST', 'TECH_MACHINERY', -200),
	('BUILD_REMOVE_FOREST', 'TECH_BIOLOGY', -200);

UPDATE Builds
SET PrereqTech = 'TECH_MASONRY'
WHERE Type = 'BUILD_REMOVE_MARSH';

UPDATE Builds
SET Time = '400'
WHERE Type = 'BUILD_REMOVE_MARSH';

INSERT INTO Build_TechTimeChanges
	(BuildType, TechType, TimeChange)
VALUES
	('BUILD_REMOVE_MARSH', 'TECH_MACHINERY', -200),
	('BUILD_REMOVE_MARSH', 'TECH_BIOLOGY', -200);

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
WHERE Type = 'FEATURE_MARSH';

UPDATE Features
SET Defense = '5'
WHERE Type = 'FEATURE_OASIS';

UPDATE Features
SET Defense = '5'
WHERE Type = 'FEATURE_FLOOD_PLAINS';

UPDATE Features
SET Defense = '5'
WHERE Type = 'FEATURE_FALLOUT';

-- Fallout 

UPDATE Features
SET TurnDamage = '15'
WHERE Type = 'FEATURE_FALLOUT';

UPDATE Features
SET ExtraTurnDamage = '-10'
WHERE Type = 'FEATURE_FALLOUT';

UPDATE BuildFeatures
SET Time = '400'
WHERE BuildType = 'BUILD_SCRUB_FALLOUT';

-- Buff jungles and forests
INSERT INTO BuildFeatures (BuildType, FeatureType, PrereqTech, Time, Production, Remove)
VALUES ('BUILD_CAMP', 'FEATURE_MARSH', 'TECH_IRON_WORKING', '600', 0, 1);

UPDATE BuildFeatures
SET Remove = '1'
WHERE BuildType = 'BUILD_TRADING_POST';

UPDATE BuildFeatures
SET Remove = '1'
WHERE BuildType = 'BUILD_FORT';

-- Polder specific code lacking in the BuildFeatures table
INSERT INTO BuildFeatures (BuildType, FeatureType, PrereqTech, Time, Production, Remove)
VALUES ('BUILD_POLDER', 'FEATURE_JUNGLE', 'TECH_CALENDAR', '500', 20 ,1);
INSERT INTO BuildFeatures (BuildType, FeatureType, PrereqTech, Time, Production, Remove)
VALUES ('BUILD_POLDER', 'FEATURE_FOREST', 'TECH_MINING', '300', 30, 1);

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

-- Changed Feature Yields

DELETE FROM Feature_YieldChanges WHERE FeatureType = 'FEATURE_ATOLL';
DELETE FROM Feature_YieldChanges WHERE FeatureType = 'FEATURE_FLOOD_PLAINS';
DELETE FROM Feature_YieldChanges WHERE FeatureType = 'FEATURE_FOREST';
DELETE FROM Feature_YieldChanges WHERE FeatureType = 'FEATURE_JUNGLE';
DELETE FROM Feature_YieldChanges WHERE FeatureType = 'FEATURE_MARSH';
DELETE FROM Feature_YieldChanges WHERE FeatureType = 'FEATURE_OASIS';

INSERT INTO Feature_YieldChanges
	(FeatureType, YieldType, Yield)
VALUES
	('FEATURE_ATOLL', 'YIELD_PRODUCTION', 2),
	('FEATURE_ATOLL', 'YIELD_FOOD', 2),
	('FEATURE_FLOOD_PLAINS', 'YIELD_FOOD', 3),
	('FEATURE_FOREST', 'YIELD_PRODUCTION', 1),
	('FEATURE_JUNGLE', 'YIELD_FOOD', 1),
	('FEATURE_MARSH', 'YIELD_FOOD', 1),
	('FEATURE_OASIS', 'YIELD_GOLD', 2),
	('FEATURE_OASIS', 'YIELD_FOOD', 3);

INSERT INTO Feature_TerrainBooleans
	(FeatureType, TerrainType)
VALUES
	('FEATURE_JUNGLE', 'TERRAIN_PLAINS');
