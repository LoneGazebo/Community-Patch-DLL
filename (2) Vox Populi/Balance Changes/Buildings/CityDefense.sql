
UPDATE Defines SET Value = '200' WHERE Name = 'CITY_STRENGTH_HILL_CHANGE';

-- City Defense
UPDATE Buildings SET Defense = '200' WHERE Type = 'BUILDING_CAPITAL_ENGINEER';

UPDATE Buildings SET Defense = '200' WHERE Type = 'BUILDING_SPAIN_MISSION';

UPDATE Buildings SET Defense = '300' WHERE Type IN
('BUILDING_WHITE_TOWER',
'BUILDING_ODEON',
'BUILDING_ORDER');

UPDATE Buildings SET Defense = '400' WHERE Type = 'BUILDING_PALACE';

UPDATE Buildings SET Defense = '500' WHERE Type = 'BUILDING_BOMB_SHELTER';

UPDATE Buildings SET Defense = '600' WHERE Type = 'BUILDING_RED_FORT';

UPDATE Buildings SET Defense = '600' WHERE Type = 'BUILDING_WALLS';

UPDATE Buildings SET Defense = '800' WHERE Type = 'BUILDING_WALLS_OF_BABYLON';

UPDATE Buildings SET Defense = '800' WHERE Type = 'BUILDING_CASTLE';

UPDATE Buildings SET Defense = '1000' WHERE Type = 'BUILDING_FORTRESS';

UPDATE Buildings SET Defense = '1200' WHERE Type = 'BUILDING_KREPOST';

UPDATE Buildings SET Defense = '1200' WHERE Type = 'BUILDING_ARSENAL';

UPDATE Buildings SET Defense = '2400' WHERE Type = 'BUILDING_MILITARY_BASE';

-- City HP
UPDATE Buildings SET ExtraCityHitPoints = '50' WHERE Type = 'BUILDING_CAPITAL_ENGINEER';

UPDATE Buildings SET ExtraCityHitPoints = '100' WHERE Type = 'BUILDING_RED_FORT';

UPDATE Buildings SET ExtraCityHitPoints = '125' WHERE Type = 'BUILDING_WALLS';

UPDATE Buildings SET ExtraCityHitPoints = '150' WHERE Type = 'BUILDING_WALLS_OF_BABYLON';

UPDATE Buildings SET ExtraCityHitPoints = '0' WHERE Type = 'BUILDING_CASTLE';

UPDATE Buildings SET ExtraCityHitPoints = '300' WHERE Type = 'BUILDING_ARSENAL';

UPDATE Buildings SET ExtraCityHitPoints = '200' WHERE Type = 'BUILDING_MILITARY_BASE';

-- City Damage Reduction
UPDATE Buildings SET DamageReductionFlat = '2' WHERE Type = 'BUILDING_CASTLE';

UPDATE Buildings SET DamageReductionFlat = '2' WHERE Type = 'BUILDING_FORTRESS';

UPDATE Buildings SET DamageReductionFlat = '2' WHERE Type = 'BUILDING_KREPOST';

-- City Supply
UPDATE Buildings SET CitySupplyModifier = '10' WHERE Type = 'BUILDING_WALLS';

UPDATE Buildings SET CitySupplyModifier = '10' WHERE Type = 'BUILDING_WALLS_OF_BABYLON';

UPDATE Buildings SET CitySupplyModifier = '10' WHERE Type = 'BUILDING_CASTLE';

UPDATE Buildings SET CitySupplyModifier = '10' WHERE Type = 'BUILDING_FORTRESS';

UPDATE Buildings SET CitySupplyModifier = '10' WHERE Type = 'BUILDING_KREPOST';

UPDATE Buildings SET CitySupplyModifier = '10' WHERE Type = 'BUILDING_ARSENAL';

UPDATE Buildings SET CitySupplyModifier = '20' WHERE Type = 'BUILDING_MILITARY_BASE';

-- Range Strike Bonuses
UPDATE Buildings
SET CityRangedStrikeRange = '1'
WHERE Type = 'BUILDING_WALLS';

UPDATE Buildings
SET CityRangedStrikeRange = '1'
WHERE Type = 'BUILDING_WALLS_OF_BABYLON';

UPDATE Buildings
SET CityIndirectFire = '1'
WHERE Type = 'BUILDING_FORTRESS';

UPDATE Buildings
SET CityIndirectFire = '1'
WHERE Type = 'BUILDING_KREPOST';

UPDATE Buildings
SET CityRangedStrikeRange = '1'
WHERE Type = 'BUILDING_ARSENAL';

UPDATE Buildings
SET RangedStrikeModifier = '10'
WHERE Type = 'BUILDING_MILITARY_BASE';

--Other Defense bonuses
UPDATE Buildings
SET HealRateChange = '5'
WHERE Type = 'BUILDING_FORTRESS';

UPDATE Buildings
SET HealRateChange = '5'
WHERE Type = 'BUILDING_KREPOST';

UPDATE Buildings
SET HealRateChange = '5'
WHERE Type = 'BUILDING_ARSENAL';

UPDATE Buildings
SET HealRateChange = '20'
WHERE Type = 'BUILDING_MILITARY_BASE';

UPDATE Buildings
SET CityAirStrikeDefense = '5'
WHERE Type = 'BUILDING_MILITARY_BASE';
