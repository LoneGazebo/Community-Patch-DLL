-- New City Yields
-- Base = 2 Food 1 Prod, below criteria give flat increases. If the city was settled on a resource, the resource's base yields are also added to this.
UPDATE Yields
SET MinCityFlatFreshWater = 1, MinCityMountainFreshWater = 1
WHERE Type = 'YIELD_FOOD';

UPDATE Yields
SET MinCityFlatNoFreshWater = 1, MinCityMountainNoFreshWater = 1
WHERE Type = 'YIELD_GOLD';

UPDATE Yields
SET MinCityHillFreshWater = 1, MinCityHillNoFreshWater = 1, MinCityMountainNoFreshWater = 1
WHERE Type = 'YIELD_PRODUCTION';

-- Lake: +1 Food/Prod over Coast
UPDATE Yields
SET LakeChange = 1
WHERE Type = 'YIELD_FOOD';

UPDATE Yields
SET LakeChange = 1
WHERE Type = 'YIELD_PRODUCTION';
