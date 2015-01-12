-- Temple -- Amber

UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture from [ICON_RES_INCENSE] Incense, [ICON_RES_WINE] Wine and [ICON_RES_AMBER] Amber. Reduces [ICON_HAPPINESS_3] Religious Unrest.'
WHERE Tag = 'TXT_KEY_BUILDING_TEMPLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture from [ICON_RES_INCENSE] Incense, [ICON_RES_WINE] Wine and [ICON_RES_AMBER] Amber. Reduces [ICON_HAPPINESS_3] Religious Unrest. +1 [ICON_PEACE] Faith for every 2 [ICON_CITIZEN] Citizens.'
WHERE Tag = 'TXT_KEY_BUILDING_BASILICA_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture from [ICON_RES_INCENSE] Incense, [ICON_RES_WINE] Wine and [ICON_RES_AMBER] Amber. Reduces [ICON_HAPPINESS_3] Religious Unrest.'
WHERE Tag = 'TXT_KEY_BUILDING_MUD_PYRAMID_MOSQUE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture from [ICON_RES_INCENSE] Incense, [ICON_RES_WINE] Wine and [ICON_RES_AMBER] Amber. Reduces [ICON_HAPPINESS_3] Religious Unrest. Receive 25 [ICON_PEACE] Faith upon construction, and 5 [ICON_GOLDEN_AGE] Golden Age points whenever you are victorious in battle. Bonuses scale with era. [NEWLINE][NEWLINE]Should this city be captured, the amount of [ICON_GOLD] Gold plundered by the enemy is doubled.'
WHERE Tag = 'TXT_KEY_BUILDING_BURIAL_TOMB_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Building_ResourceYieldChanges (BuildingType, ResourceType, YieldType, Yield)
SELECT 'BUILDING_TEMPLE', 'RESOURCE_AMBER' , 'YIELD_CULTURE' , '1'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Building_ResourceYieldChanges (BuildingType, ResourceType, YieldType, Yield)
SELECT 'BUILDING_BASILICA', 'RESOURCE_AMBER' , 'YIELD_CULTURE' , '1'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Building_ResourceYieldChanges (BuildingType, ResourceType, YieldType, Yield)
SELECT 'BUILDING_MUD_PYRAMID_MOSQUE', 'RESOURCE_AMBER' , 'YIELD_CULTURE' , '1'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Building_ResourceYieldChanges (BuildingType, ResourceType, YieldType, Yield)
SELECT 'BUILDING_BURIAL_TOMB', 'RESOURCE_AMBER' , 'YIELD_CULTURE' , '1'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- Amphitheater -- Lapis Lazuli

UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture from [ICON_RES_DYE] Dye and [ICON_RES_LAPIS] Lapis Lazuli. +1 [ICON_CULTURE] Culture for every 10 [ICON_CITIZEN] Citizens in the City.'
WHERE Tag = 'TXT_KEY_BUILDING_AMPHITHEATER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Amphitheater increases the [ICON_CULTURE] Culture of a city and grants +1 [ICON_CULTURE] Culture to all nearby [ICON_RES_DYE] Dye and [ICON_RES_LAPIS] Lapis Lazuli resources, speeding the growth of the territory of the city and the acquisition of Social Policies.  Contains 1 slot for a Great Work of Writing.'
WHERE Tag = 'TXT_KEY_BUILDING_AMPHITHEATER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture, and +1 [ICON_CULTURE] Culture for every 5 [ICON_CITIZEN] Citizens in the city. +1 [ICON_CULTURE] Culture on nearby [ICON_RES_DYE] Dye and [ICON_RES_LAPIS] Lapis Lazuli resources. [NEWLINE][NEWLINE]+2 [ICON_TOURISM] Tourism once you research Philosophy.'
WHERE Tag = 'TXT_KEY_BUILDING_ODEON_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Building_ResourceYieldChanges (BuildingType, ResourceType, YieldType, Yield)
SELECT 'BUILDING_AMPHITHEATER', 'RESOURCE_LAPIS' , 'YIELD_CULTURE' , '1'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Building_ResourceYieldChanges (BuildingType, ResourceType, YieldType, Yield)
SELECT 'BUILDING_ODEON', 'RESOURCE_LAPIS' , 'YIELD_CULTURE' , '1'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- Coffee/Tea/Tobacco -- Windmill

UPDATE Language_en_US
SET Text = '+10% [ICON_PRODUCTION] Production when constructing Buildings. +1 [ICON_GOLD] Gold from [ICON_RES_TOBACCO] Tobacco, [ICON_RES_COFFEE] Coffee and [ICON_RES_TEA] Tea.'
WHERE Tag = 'TXT_KEY_BUILDING_WINDMILL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+25% [ICON_GREAT_PEOPLE] Great People generation in this City. +10% [ICON_PRODUCTION] Production when constructing Buildings. [NEWLINE][NEWLINE] +2 [ICON_GOLD] Gold from [ICON_RES_TOBACCO] Tobacco, [ICON_RES_COFFEE] Coffee and [ICON_RES_TEA] Tea.'
WHERE Tag = 'TXT_KEY_BUILDING_COFFEE_HOUSE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Building_ResourceYieldChanges (BuildingType, ResourceType, YieldType, Yield)
SELECT 'BUILDING_WINDMILL', 'RESOURCE_TOBACCO' , 'YIELD_GOLD' , '1'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Building_ResourceYieldChanges (BuildingType, ResourceType, YieldType, Yield)
SELECT 'BUILDING_WINDMILL', 'RESOURCE_COFFEE' , 'YIELD_GOLD' , '1'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Building_ResourceYieldChanges (BuildingType, ResourceType, YieldType, Yield)
SELECT 'BUILDING_WINDMILL', 'RESOURCE_TEA' , 'YIELD_GOLD' , '1'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Building_ResourceYieldChanges (BuildingType, ResourceType, YieldType, Yield)
SELECT 'BUILDING_COFFEE_HOUSE', 'RESOURCE_TOBACCO' , 'YIELD_GOLD' , '2'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Building_ResourceYieldChanges (BuildingType, ResourceType, YieldType, Yield)
SELECT 'BUILDING_COFFEE_HOUSE', 'RESOURCE_COFFEE' , 'YIELD_GOLD' , '2'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Building_ResourceYieldChanges (BuildingType, ResourceType, YieldType, Yield)
SELECT 'BUILDING_COFFEE_HOUSE', 'RESOURCE_TEA' , 'YIELD_GOLD' , '2'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- Jade -- Stone Works

UPDATE Language_en_US
SET Text = 'Each source of [ICON_RES_MARBLE] Marble, [ICON_RES_STONE] Stone, [ICON_RES_SALT] Salt, and [ICON_RES_JADE] Jade worked by this City produces +1 [ICON_PRODUCTION] Production.[NEWLINE][NEWLINE]City must have at least one of these resources improved with a Quarry.'
WHERE Tag = 'TXT_KEY_BUILDING_STONE_WORKS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_en_US
SET Text = 'Stone Works can only be constructed in a city near an improved [ICON_RES_STONE] Stone, [ICON_RES_MARBLE] Marble, [ICON_RES_SALT] Salt, or [ICON_RES_JADE] Jade resource. Stone Works increase [ICON_PRODUCTION] Production.'
WHERE Tag = 'TXT_KEY_BUILDING_STONE_WORKS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Building_ResourceYieldChanges (BuildingType, ResourceType, YieldType, Yield)
SELECT 'BUILDING_STONE_WORKS', 'RESOURCE_JADE' , 'YIELD_PRODUCTION' , '1'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Building_LocalResourceOrs (BuildingType, ResourceType)
SELECT 'BUILDING_STONE_WORKS', 'RESOURCE_JADE'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- Olives, Perfume -- Colosseum
UPDATE Language_en_US
SET Text = 'Reduces [ICON_HAPPINESS_3] Boredom slightly. Each source of [ICON_RES_PERFUME] Perfume and [ICON_RES_OLIVE] Olives worked by this City produces +1 [ICON_CULTURE] Culture.'
WHERE Tag = 'TXT_KEY_BUILDING_COLOSSEUM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Colosseum reduces Boredom in a city, grants additional Culture, and improves the Culture yield output of [ICON_RES_PERFUME] Perfume and [ICON_RES_OLIVE] Olives. Build these to combat Unhappiness from Boredom, and to increase your Culture.'
WHERE Tag = 'TXT_KEY_BUILDING_COLOSSEUM_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Building_ResourceYieldChanges (BuildingType, ResourceType, YieldType, Yield)
SELECT 'BUILDING_COLOSSEUM', 'RESOURCE_OLIVE' , 'YIELD_CULTURE' , '1'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Building_ResourceYieldChanges (BuildingType, ResourceType, YieldType, Yield)
SELECT 'BUILDING_COLOSSEUM', 'RESOURCE_PERFUME' , 'YIELD_CULTURE' , '1'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- Coral -- Covered by Lighthouse