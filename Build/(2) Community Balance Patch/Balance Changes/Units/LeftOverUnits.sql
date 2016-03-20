
-- Borrowed from Whoward's tutorial on triggers

CREATE TRIGGER Unit_BuildingClassPurchaseRequireds
AFTER INSERT ON Units
WHEN EXISTS ( SELECT Type FROM Units WHERE Type = NEW.Type )
BEGIN
INSERT INTO Unit_BuildingClassPurchaseRequireds( UnitType, BuildingClassType )
SELECT ( NEW.Type ), BuildingClassType
FROM Unit_BuildingClassPurchaseRequireds WHERE UnitType IN ( SELECT DefaultUnit FROM UnitClasses WHERE Type = NEW.Class );
END;