-- Terrain 

UPDATE Terrains
SET Movement = '2'
WHERE Type = 'TERRAIN_DESERT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE Terrains
SET Movement = '2'
WHERE Type = 'TERRAIN_SNOW' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE Terrains
SET Movement = '3'
WHERE Type = 'TERRAIN_MOUNTAIN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE Terrains
SET SeeFrom = '2'
WHERE Type = 'TERRAIN_MOUNTAIN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

-- Improvements -- Trading Post -- Available at Horseback Riding

UPDATE Builds
SET PrereqTech = 'TECH_HORSEBACK_RIDING'
WHERE Type = 'BUILD_TRADING_POST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

-- Removal of Forest/Jungle/Marsh moved back a bit
UPDATE Builds
SET PrereqTech = 'TECH_METAL_CASTING'
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
SET PrereqTech = 'TECH_METAL_CASTING'
WHERE FeatureType = 'FEATURE_JUNGLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE BuildFeatures
SET PrereqTech = 'TECH_MACHINERY'
WHERE FeatureType = 'FEATURE_MARSH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );



-- Forts and Citadels 'no two adjacent' and no outside borders for Fort - cannot be built on resources (helps AI quite a bit)
UPDATE Improvements
SET NoTwoAdjacent = '1'
WHERE Type = 'IMPROVEMENT_FORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE Improvements
SET OutsideBorders = '0'
WHERE Type = 'IMPROVEMENT_FORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE Improvements
SET BuildableOnResources = '0'
WHERE Type = 'IMPROVEMENT_FORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE Improvements
SET NoTwoAdjacent = '1'
WHERE Type = 'IMPROVEMENT_CITADEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

-- Forts and Citadels now passable for ships and trade units.

UPDATE Improvements
SET MakesPassable = '1'
WHERE Type = 'IMPROVEMENT_FORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

UPDATE Improvements
SET MakesPassable = '1'
WHERE Type = 'IMPROVEMENT_CITADEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

-- Trading Post also going to be 'no two adjacent'

UPDATE Improvements
SET NoTwoAdjacent = '1'
WHERE Type = 'IMPROVEMENT_TRADING_POST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

-- Trading Post - name change

UPDATE Language_en_US
SET Text = 'Villages are smaller settlements scattered around the countryside of a civilization, representative of the people that live off of the land and trade. They can generate a lot of wealth for a society, particularly when placed on Roads, Railroads, or on Trade Routes.[NEWLINE][NEWLINE]+1 [ICON_GOLD] Gold if built on a Road that connects two owned Cities, and +2 [ICON_GOLD] if a Railroad. Receive additional [ICON_GOLD] Gold (+1 for Roads, +2 for Railroads) if a Trade Route, either internal or international, passes over this Village.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_TRADING_POST_TEXT';

UPDATE Language_en_US
SET Text = 'Village'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_TRADINGPOST';

UPDATE Language_en_US
SET Text = 'Village'
WHERE Tag = 'TXT_KEY_IMPROVEMENT_TRADING_POST';

UPDATE Language_en_US
SET Text = 'Construct a [LINK=IMPROVEMENT_TRADING_POST]Village[\LINK]'
WHERE Tag = 'TXT_KEY_BUILD_TRADING_POST';

UPDATE Language_en_US
SET Text = 'Does the village provide gold?'
WHERE Tag = 'TXT_KEY_GOLD_TRADINGPOST_ADV_QUEST';

UPDATE Language_en_US
SET Text = 'Construct a Village improvement in a tile to increase its gold output. Earns additional gold if placed on a Road or Railroad that connects two owned cities. Also earns additional gold if a Trade Route, either internal or international, passes over it.'
WHERE Tag = 'TXT_KEY_GOLD_TRADINGPOST_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'The village increases output of a tile by 1 gold. It does not access a resource. It generates additional gold if on a route, or if build on a trade route.[NEWLINE]Technology Required: Currency[NEWLINE]Construction Time: 5 Turns[NEWLINE]May Be Constructed On: Any land tile but ice.'
WHERE Tag = 'TXT_KEY_WORKERS_TRADINGPOST_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'The Village'
WHERE Tag = 'TXT_KEY_GOLD_TRADINGPOST_HEADING3_TITLE';

UPDATE Language_en_US
SET Text = 'During a game, you will create "workers" - non-military units who will "improve" the land around your cities, increasing productivity or providing access to a nearby "resource." Improvements include farms, villages, lumber mills, quarries, mines, and more. During wartime your enemy may "pillage" (destroy) your improvements. Pillaged improvements are ineffective until a worker has "repaired" them.'
WHERE Tag = 'TXT_KEY_PEDIA_IMPROVEMENT_HELP_TEXT';

-- Fort
UPDATE Language_en_US
SET Text = 'A fort is a special improvement that improves the defensive bonus of the tile by 50% for units stationed in that tile. However, forts do not provide a defensive bonus to units in enemy territory. Cannot be built adjacent to one another.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_FORT_TEXT';

-- Shrink Trading Post
UPDATE ArtDefine_Landmarks
SET Scale= '0.70'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_TRADING_POST1';

-- Shrink Trading Post
UPDATE ArtDefine_Landmarks
SET Scale= '0.70'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_TRADING_POST2';

-- Shrink Trading Post
UPDATE ArtDefine_Landmarks
SET Scale= '0.70'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_TRADING_POST3';

-- Shrink Trading Post
UPDATE ArtDefine_Landmarks
SET Scale= '0.70'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_TRADING_POST4';

-- Shrink Academy
UPDATE ArtDefine_Landmarks
SET Scale= '0.80'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_ACADEMY';

-- Shrink Manufactory
UPDATE ArtDefine_Landmarks
SET Scale= '0.80'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_MANUFACTORY';

-- Shrink Holy Site
UPDATE ArtDefine_Landmarks
SET Scale= '0.80'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_HOLY_SITE';

-- Shrink Kasbah
UPDATE ArtDefine_Landmarks
SET Scale= '0.80'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_KASBAH';

-- Shrink Chateau
UPDATE ArtDefine_Landmarks
SET Scale= '0.80'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_CHATEAU';

-- Shrink Customs House
UPDATE ArtDefine_Landmarks
SET Scale= '0.80'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_CUSTOMS_HOUSE';

