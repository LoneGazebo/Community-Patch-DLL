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

INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType, Cost) SELECT 'UNIT_BARBARIAN_HORSEMAN', 'RESOURCE_HORSE', '1';
INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType, Cost) SELECT 'UNIT_BARBARIAN_SWORDSMAN', 'RESOURCE_IRON', '1';