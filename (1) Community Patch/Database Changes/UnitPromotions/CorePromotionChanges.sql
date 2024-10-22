-- Fix England UA promotion because of column rework
-- ExtraNavalMovement gives embarked movement only
UPDATE UnitPromotions SET MovesChange = 2, ExtraNavalMovement = 0 WHERE Type = 'PROMOTION_OCEAN_MOVEMENT';

UPDATE UnitPromotions SET Help = 'TXT_KEY_PROMOTION_OCEAN_IMPASSABLE_HELP' WHERE Type = 'PROMOTION_OCEAN_IMPASSABLE';
