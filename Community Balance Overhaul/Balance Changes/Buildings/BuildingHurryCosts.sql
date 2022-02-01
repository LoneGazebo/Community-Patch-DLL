
-- Hurry Cost - reduced for all buildings (as you now need production as well to make them work)
UPDATE Buildings
SET HurryCostModifier = '5';

UPDATE Buildings
SET HurryCostModifier = '-5'
WHERE NOT WonderSplashImage = 'NULL';
