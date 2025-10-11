-- Use UnitPromotions_Terrains.DoubleMove and UnitPromotions_TerrainModifiers instead
UPDATE UnitPromotions SET HillsDoubleMove = 0 WHERE Type IN ('PROMOTION_SKI_INFANTRY', 'PROMOTION_ALTITUDE_TRAINING');
DELETE FROM UnitPromotions_Terrains WHERE PromotionType IN ('PROMOTION_ALTITUDE_TRAINING', 'PROMOTION_DESERT_WARRIOR');
INSERT INTO UnitPromotions_Terrains
	(PromotionType, TerrainType, DoubleMove)
VALUES
	('PROMOTION_SKI_INFANTRY', 'TERRAIN_HILL', 1),
	('PROMOTION_ALTITUDE_TRAINING', 'TERRAIN_HILL', 1);
INSERT INTO UnitPromotions_TerrainModifiers
	(PromotionType, TerrainType, Attack, Defense)
VALUES
	('PROMOTION_ALTITUDE_TRAINING', 'TERRAIN_HILL', 10, 10),
	('PROMOTION_DESERT_WARRIOR', 'TERRAIN_DESERT', 50, 50);

-- Fix England UA promotion because of column rework
-- ExtraNavalMovement gives embarked movement only
UPDATE UnitPromotions SET MovesChange = 2, ExtraNavalMovement = 0 WHERE Type = 'PROMOTION_OCEAN_MOVEMENT';

UPDATE UnitPromotions SET Help = 'TXT_KEY_PROMOTION_OCEAN_IMPASSABLE_HELP' WHERE Type = 'PROMOTION_OCEAN_IMPASSABLE';
