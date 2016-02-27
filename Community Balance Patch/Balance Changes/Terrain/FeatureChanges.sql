-- Removal of Forest/Jungle/Marsh moved back a bit
UPDATE Builds
SET PrereqTech = 'TECH_IRON_WORKING'
WHERE Type = 'BUILD_REMOVE_JUNGLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE Builds
SET PrereqTech = 'TECH_BRONZE_WORKING'
WHERE Type = 'BUILD_REMOVE_FOREST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE Builds
SET PrereqTech = 'TECH_MACHINERY'
WHERE Type = 'BUILD_REMOVE_MARSH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE BuildFeatures
SET PrereqTech = 'TECH_BRONZE_WORKING'
WHERE FeatureType = 'FEATURE_FOREST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE BuildFeatures
SET PrereqTech = 'TECH_IRON_WORKING'
WHERE FeatureType = 'FEATURE_JUNGLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE BuildFeatures
SET PrereqTech = 'TECH_MACHINERY'
WHERE FeatureType = 'FEATURE_MARSH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );



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

-- Fallout 

UPDATE Features
SET TurnDamage = '15'
WHERE Type = 'FEATURE_FALLOUT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE Features
SET ExtraTurnDamage = '-10'
WHERE Type = 'FEATURE_FALLOUT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE BuildFeatures
SET Time = '400'
WHERE BuildType = 'BUILD_SCRUB_FALLOUT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_NEGATIVE_TEXT]Fallout deals 15 Damage to Units that end their turn on a tile with Fallout.[ENDCOLOR][NEWLINE][NEWLINE]Fallout is the residual radiation left over following a nuclear explosion. The fallout "falls out" of the air as a layer of radioactive particles which are highly dangerous to plants and animals, killing them immediately or damaging their DNA, giving them cancer, other diseases, or unfortunate mutations. Depending upon the type of nuclear explosion, the land may remain poisoned for decades, possibly centuries. Cleanup requires the replacement of the contaminated buildings, soil and vegetation.'
WHERE Tag = 'TXT_KEY_FEATURE_FALLOUT_PEDIA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );


-- Buff jungles and forests
INSERT INTO BuildFeatures (BuildType, FeatureType, PrereqTech, Time, Remove)
VALUES ('BUILD_CAMP', 'FEATURE_MARSH', 'TECH_MACHINERY', '600', 1);

UPDATE BuildFeatures
SET Remove = '1'
WHERE BuildType = 'BUILD_TRADING_POST';

UPDATE BuildFeatures
SET Remove = '1'
WHERE BuildType = 'BUILD_FORT';

-- Polder specific code lacking in the BuildFeatures table
INSERT INTO BuildFeatures (BuildType, FeatureType, PrereqTech, Time, Production, Remove)
VALUES ('BUILD_POLDER', 'FEATURE_JUNGLE', 'TECH_IRON_WORKING', '500', '',1);
INSERT INTO BuildFeatures (BuildType, FeatureType, PrereqTech, Time, Production, Remove)
VALUES ('BUILD_POLDER', 'FEATURE_FOREST', 'TECH_BRONZE_WORKING', '300', '40', 1);

UPDATE BuildFeatures SET Remove = 1 WHERE  FeatureType = 'FEATURE_MARSH'  AND BuildType NOT IN ('BUILD_POLDER');
UPDATE BuildFeatures SET Time = 300 WHERE (FeatureType = 'FEATURE_JUNGLE' AND Time <> 0 AND Remove = 0);
UPDATE BuildFeatures SET Time = 500 WHERE (FeatureType = 'FEATURE_JUNGLE' AND Time <> 0 AND Remove = 1);
UPDATE BuildFeatures SET Time = 400 WHERE  BuildType   = 'BUILD_REMOVE_JUNGLE';
UPDATE BuildFeatures SET Time = 300 WHERE  BuildType   = 'BUILD_REMOVE_FOREST';
UPDATE BuildFeatures SET Production = 30 WHERE FeatureType = 'FEATURE_FOREST' AND Remove = 1;
UPDATE BuildFeatures SET Production = 20 WHERE FeatureType = 'FEATURE_JUNGLE' AND Remove = 1;

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
