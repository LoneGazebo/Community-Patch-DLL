-- Update Hover Units to obey coast/ocean tiles

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
SELECT 'UNIT_HELICOPTER_GUNSHIP' , 'PROMOTION_EMBARKATION';

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
SELECT 'UNIT_HELICOPTER_GUNSHIP' , 'PROMOTION_DEFENSIVE_EMBARKATION';

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
SELECT 'UNIT_HELICOPTER_GUNSHIP' , 'PROMOTION_ALLWATER_EMBARKATION';

DELETE FROM Civilization_UnitClassOverrides WHERE CivilizationType = 'CIVILIZATION_BARBARIAN' AND UnitClassType = 'UNITCLASS_CHARIOT_ARCHER';
INSERT INTO Civilization_UnitClassOverrides (CivilizationType, UnitClassType, UnitType)
SELECT 'CIVILIZATION_BARBARIAN' , 'UNITCLASS_CHARIOT_ARCHER', 'UNIT_BARBARIAN_AXMAN';