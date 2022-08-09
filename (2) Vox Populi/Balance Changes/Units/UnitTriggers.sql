CREATE TRIGGER VP_SwordsmanPromotion_Replace
AFTER INSERT ON Units
WHEN (SELECT Class FROM Units WHERE Type = NEW.UnitType) = 'UNITCLASS_SWORDSMAN'
BEGIN
    DELETE FROM Unit_FreePromotions WHERE PromotionType = 'PROMOTION_COVER_1' AND UnitType = NEW.UnitType;
    INSERT INTO Unit_FreePromotions (UnitType, PromotionType) VALUES (NEW.UnitType, 'PROMOTION_PROFESSIONAL_SOLDIER');
END;

CREATE TRIGGER VP_LongswordPromotion_Replace
AFTER INSERT ON Units
WHEN (SELECT Class FROM Units WHERE Type = NEW.UnitType) = 'UNITCLASS_LONGSWORDSMAN'
BEGIN
    DELETE FROM Unit_FreePromotions WHERE PromotionType = 'PROMOTION_COVER_1' AND UnitType = NEW.UnitType;
    INSERT INTO Unit_FreePromotions (UnitType, PromotionType) VALUES (NEW.UnitType, 'PROMOTION_PROFESSIONAL_SOLDIER');
END;

CREATE TRIGGER VP_Tercio_Professionalization
AFTER INSERT ON Units
WHEN (SELECT Class FROM Units WHERE Type = NEW.UnitType) = 'UNITCLASS_TERCIO'
BEGIN
    INSERT INTO Unit_FreePromotions (UnitType, PromotionType) VALUES (NEW.UnitType, 'PROMOTION_PROFESSIONAL_SOLDIER');
	UPDATE Units SET Combat = Combat - 1 WHERE Type = NEW.UnitType;
END;
