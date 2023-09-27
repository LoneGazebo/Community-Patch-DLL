-- New City Yields
-- Base = max(2 Food 1 Prod, the tile it's on), below criteria give flat increases 
UPDATE Yields
SET MinCityFlatFreshWater = 1, MinCityMountainFreshWater = 1
WHERE Type = 'YIELD_FOOD';

UPDATE Yields
SET MinCityFlatNoFreshWater = 1, MinCityMountainNoFreshWater = 1
WHERE Type = 'YIELD_GOLD';

UPDATE Yields
SET MinCityMountainNoFreshWater = 1
WHERE Type = 'YIELD_PRODUCTION';

-- Lake: +1 Food/Prod over Coast
UPDATE Yields
SET LakeChange = 1
WHERE Type = 'YIELD_FOOD';

UPDATE Yields
SET LakeChange = 1
WHERE Type = 'YIELD_PRODUCTION';
