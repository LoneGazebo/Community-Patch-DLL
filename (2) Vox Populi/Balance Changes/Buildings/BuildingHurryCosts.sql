
-- Hurry Cost - reduced for all buildings (as you now need production as well to make them work)
UPDATE Buildings
SET HurryCostModifier = '-20';

UPDATE Buildings
SET HurryCostModifier = '-5'
WHERE NOT WonderSplashImage = 'NULL';

UPDATE Buildings
SET HurryCostModifier = '-1'
WHERE IsDummy = '1' OR Cost = '-1';