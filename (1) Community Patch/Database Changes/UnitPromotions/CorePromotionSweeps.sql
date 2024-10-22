-- Flank mechanic is changed from modifier to base flank% to stacking additively with base flank%
UPDATE UnitPromotions SET FlankAttackModifier = 5 WHERE FlankAttackModifier = 50;
UPDATE UnitPromotions SET FlankAttackModifier = 3 WHERE FlankAttackModifier = 25;

-- Base embarked sight range is now 1 instead of 0
UPDATE UnitPromotions SET EmbarkExtraVisibility = EmbarkExtraVisibility - 1 WHERE EmbarkExtraVisibility > 1;
