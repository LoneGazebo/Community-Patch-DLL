-- Fallout
-- ExtraTurnDamage is for Fallout Resistance promotion
UPDATE Features
SET TurnDamage = 15, ExtraTurnDamage = -10
WHERE Type = 'FEATURE_FALLOUT';

-- Negative defense is bad for AI
UPDATE Features
SET Defense = 5
WHERE Type IN ('FEATURE_MARSH', 'FEATURE_OASIS', 'FEATURE_FLOOD_PLAINS', 'FEATURE_FALLOUT');

-- Forest is flat +1 Prod
UPDATE Features
SET YieldNotAdditive = 0
WHERE Type = 'FEATURE_FOREST';

-- Not sure why this wasn't in vanilla
INSERT INTO Feature_TerrainBooleans
	(FeatureType, TerrainType)
VALUES
	('FEATURE_JUNGLE', 'TERRAIN_PLAINS');
