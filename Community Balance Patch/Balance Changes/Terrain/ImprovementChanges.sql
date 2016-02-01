-- Improvements -- Trading Post -- Available at Currency

UPDATE Builds
SET PrereqTech = 'TECH_CURRENCY'
WHERE Type = 'BUILD_TRADING_POST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

-- Lumbermill moved to Steel
UPDATE Builds
SET PrereqTech = 'TECH_METAL_CASTING'
WHERE Type = 'BUILD_LUMBERMILL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

-- Railroads now more expensive (+1)
UPDATE Routes
SET GoldMaintenance = '3'
WHERE Type = 'ROUTE_RAILROAD';

-- Value Changes
UPDATE Improvement_ResourceType_Yields
SET YieldType = 'YIELD_CULTURE'
WHERE ResourceType = 'RESOURCE_INCENSE';

UPDATE Improvement_ResourceType_Yields
SET YieldType = 'YIELD_CULTURE'
WHERE ResourceType = 'RESOURCE_PEARLS';

-- Farm
UPDATE Builds
SET Help = 'TXT_KEY_BUILD_FARM_HELP'
WHERE Type = 'BUILD_FARM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

INSERT INTO Language_en_US (
Tag, Text)
SELECT 'TXT_KEY_BUILD_FARM_HELP', 'Gain an additional +1 [ICON_FOOD] Food for every 2 Farms adjacent to one another.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Builds
SET Recommendation = 'TXT_KEY_BUILD_FARM_REC'
WHERE Type = 'BUILD_FARM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TERRAIN' AND Value= 1 );

INSERT INTO Language_en_US (
Tag, Text)
SELECT 'TXT_KEY_BUILD_FARM_REC', 'It will boost your [ICON_FOOD] Food output on this tile. For every two Farms adjacent to this one, it will gain an additional +1 [ICON_FOOD] Food!'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Farms can be constructed on most any land to improve the output of food on the tile. For every two Farms adjacent to each other, Farms gain additional Food.[NEWLINE][NEWLINE]Farming is one of the earliest and most important of all human professions, as it allowed mankind to stop migrating and settle in one location without depleting the local resources.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_FARM_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );


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
SET Text = 'Villages are smaller settlements scattered around the countryside of a civilization, representative of the people that live off of the land and trade. They can generate a lot of wealth for a society, particularly when placed on Roads, Railroads, or on Trade Routes.[NEWLINE][NEWLINE]Receive [ICON_GOLD] Gold [ICON_PRODUCTION] Production if built on a Road or Railroad that connects two owned Cities.[NEWLINE][NEWLINE]Receive additional [ICON_GOLD] Gold and [ICON_PRODUCTION] Production (+1 pre-Industrial Era, +2 Industrial Era or later) if a Trade Route, either internal or international, passes over this Village.'
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
SET Text = 'Construct a Village improvement in a tile to increase its gold output. Earns additional gold and production if placed on a Road or Railroad that connects two owned cities and/or if a Trade Route, either internal or international, passes over it. Cannot be built adjacent to one another.'
WHERE Tag = 'TXT_KEY_GOLD_TRADINGPOST_HEADING3_BODY';

UPDATE Language_en_US
SET Text = 'The village increases output of a tile by 3 gold and 1 culture. It does not access a resource. It generates additional gold and production if on a route and/or if built on a trade route.[NEWLINE]Technology Required: Currency[NEWLINE]Construction Time: 5 Turns[NEWLINE]May Be Constructed On: Any land tile but ice. Cannot be built adjacent to one another.'
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

-- Landmark
UPDATE Language_en_US
SET Text = 'A Landmark is any magnificent artifact, structure, work of art, or wonder of nature that draws visitors to a location. Nelson''s Column in London is a landmark, as is Mount Rushmore in the United States. Not every significantly sized object, however, is a landmark: the World''s Largest Ball of Twine may never rise to that stature (though it might well be worth a visit). Landmarks provide +1 [ICON_CULTURE] Culture and +1 [ICON_GOLD] Gold for each additional Era that has passed in comparison to the original Era of the Artifact.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_LANDMARK_TEXT';

-- Shrink Trading Post
UPDATE ArtDefine_Landmarks
SET Scale= '0.65'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_TRADING_POST1';

-- Shrink Trading Post
UPDATE ArtDefine_Landmarks
SET Scale= '0.65'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_TRADING_POST2';

-- Shrink Trading Post
UPDATE ArtDefine_Landmarks
SET Scale= '0.65'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_TRADING_POST3';

-- Shrink Trading Post
UPDATE ArtDefine_Landmarks
SET Scale= '0.65'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_TRADING_POST4';

-- Shrink Academy
UPDATE ArtDefine_Landmarks
SET Scale= '0.75'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_ACADEMY';

-- Shrink Manufactory
UPDATE ArtDefine_Landmarks
SET Scale= '0.75'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_MANUFACTORY';

-- Shrink Holy Site
UPDATE ArtDefine_Landmarks
SET Scale= '0.75'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_HOLY_SITE';

-- Shrink Kasbah
UPDATE ArtDefine_Landmarks
SET Scale= '0.80'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_KASBAH';

-- Shrink Chateau
UPDATE ArtDefine_Landmarks
SET Scale= '0.70'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_CHATEAU';

-- Shrink Customs House
UPDATE ArtDefine_Landmarks
SET Scale= '0.70'
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_CUSTOMS_HOUSE';