
-- Hurry Cost - reduced for all buildings (as you now need production as well to make them work)
UPDATE Buildings
SET HurryCostModifier = '5'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Buildings
SET HurryCostModifier = '-5'
WHERE NOT WonderSplashImage = 'NULL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
