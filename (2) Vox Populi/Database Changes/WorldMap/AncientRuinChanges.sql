UPDATE GoodyHuts
SET
	MapOffset = 10, -- radius to search for area reveal
	MapRange = 3, -- radius of revealed area
	MapProb = 110 -- every tile in the area always revealed
WHERE Type = 'GOODY_MAP';

-- Ruin yields
UPDATE GoodyHuts SET Production = 50 WHERE Type = 'GOODY_PRODUCTION';
UPDATE GoodyHuts SET Gold = 100, NumGoldRandRolls = 0 WHERE Type = 'GOODY_GOLD';
UPDATE GoodyHuts SET Culture = 15 WHERE Type = 'GOODY_CULTURE';
UPDATE GoodyHuts SET PantheonFaith = 0, PantheonPercent = 60 WHERE Type = 'GOODY_PANTHEON_FAITH';
UPDATE GoodyHuts SET ProphetPercent = 25 WHERE Type = 'GOODY_PROPHET_FAITH';
UPDATE GoodyHuts SET Experience = 20 WHERE Type = 'GOODY_EXPERIENCE';
