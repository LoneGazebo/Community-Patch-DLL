
UPDATE Defines SET Value = '200' WHERE Name = 'CITY_STRENGTH_HILL_CHANGE';

-- City Defense
UPDATE Buildings SET Defense = '600' WHERE Type = 'BUILDING_WALLS';

UPDATE Buildings SET Defense = '800' WHERE Type = 'BUILDING_WALLS_OF_BABYLON';

UPDATE Buildings SET Defense = '800' WHERE Type = 'BUILDING_CASTLE';

--UPDATE Buildings SET Defense = '800' WHERE Type = 'BUILDING_MISSION';

UPDATE Buildings SET Defense = '2000' WHERE Type = 'BUILDING_ARSENAL';

UPDATE Buildings SET Defense = '2000' WHERE Type = 'BUILDING_KREPOST';

UPDATE Buildings SET Defense = '600' WHERE Type = 'BUILDING_RED_FORT';

UPDATE Buildings SET Defense = '2000' WHERE Type = 'BUILDING_MILITARY_BASE';

UPDATE Buildings SET Defense = '300' WHERE Type = 'BUILDING_PALACE';

UPDATE Buildings SET Defense = '300' WHERE Type = 'BUILDING_WHITE_TOWER';

UPDATE Buildings SET Defense = '300' WHERE Type = 'BUILDING_ODEON';

UPDATE Buildings SET Defense = '300' WHERE Type = 'BUILDING_VENETIAN_ARSENALE';

UPDATE Buildings SET Defense = '2000' WHERE Type = 'BUILDING_MINEFIELD';

UPDATE Buildings SET Defense = '300' WHERE Type = 'BUILDING_ORDER';

UPDATE Buildings SET Defense = '300' WHERE Type = 'BUILDING_CAPITAL_ENGINEER';

UPDATE Buildings SET Defense = '500' WHERE Type = 'BUILDING_BOMB_SHELTER';

-- City HP
UPDATE Buildings
SET ExtraCityHitPoints = '125'
WHERE Type = 'BUILDING_WALLS';

UPDATE Buildings
SET ExtraCityHitPoints = '150'
WHERE Type = 'BUILDING_WALLS_OF_BABYLON';

UPDATE Buildings
SET ExtraCityHitPoints = '150'
WHERE Type = 'BUILDING_CASTLE';

--UPDATE Buildings
--SET ExtraCityHitPoints = '150'
--WHERE Type = 'BUILDING_MISSION';

UPDATE Buildings
SET ExtraCityHitPoints = '175'
WHERE Type = 'BUILDING_ARSENAL';

UPDATE Buildings
SET ExtraCityHitPoints = '175'
WHERE Type = 'BUILDING_KREPOST';

UPDATE Buildings
SET ExtraCityHitPoints = '200'
WHERE Type = 'BUILDING_MILITARY_BASE';

UPDATE Buildings
SET ExtraCityHitPoints = '100'
WHERE Type = 'BUILDING_RED_FORT';

UPDATE Buildings
SET ExtraCityHitPoints = '200'
WHERE Type = 'BUILDING_BOMB_SHELTER';

UPDATE Buildings
SET ExtraCityHitPoints = '200'
WHERE Type = 'BUILDING_MINEFIELD';

UPDATE Buildings
SET ExtraCityHitPoints = '150'
WHERE Type = 'BUILDING_HARBOR';

-- Bonus RCS

UPDATE Buildings
SET RangedStrikeModifier = '10'
WHERE Type = 'BUILDING_MILITARY_BASE';
