CREATE TRIGGER HappinessMod_Buildings
AFTER INSERT ON Civilization_BuildingClassOverrides
WHEN EXISTS (SELECT BuildingType FROM Civilization_BuildingClassOverrides WHERE BuildingType = NEW.BuildingType)
BEGIN

UPDATE Buildings
SET Happiness = (SELECT Happiness FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),

DistressFlatReduction = (SELECT DistressFlatReduction FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
PovertyFlatReduction = (SELECT PovertyFlatReduction FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
IlliteracyFlatReduction = (SELECT IlliteracyFlatReduction FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
BoredomFlatReduction = (SELECT BoredomFlatReduction FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
ReligiousUnrestFlatReduction = (SELECT ReligiousUnrestFlatReduction FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),

DistressFlatReductionGlobal = (SELECT DistressFlatReductionGlobal FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
PovertyFlatReductionGlobal = (SELECT PovertyFlatReductionGlobal FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
IlliteracyFlatReductionGlobal = (SELECT IlliteracyFlatReductionGlobal FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
BoredomFlatReductionGlobal = (SELECT BoredomFlatReductionGlobal FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
ReligiousUnrestFlatReductionGlobal = (SELECT ReligiousUnrestFlatReductionGlobal FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),

BasicNeedsMedianModifier = (SELECT BasicNeedsMedianModifier FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
GoldMedianModifier = (SELECT GoldMedianModifier FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
ScienceMedianModifier = (SELECT ScienceMedianModifier FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
CultureMedianModifier = (SELECT CultureMedianModifier FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
ReligiousUnrestModifier = (SELECT ReligiousUnrestModifier FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),

BasicNeedsMedianModifierGlobal = (SELECT BasicNeedsMedianModifierGlobal FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type=NEW.BuildingClassType)),
GoldMedianModifierGlobal = (SELECT GoldMedianModifierGlobal FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type=NEW.BuildingClassType)),
ScienceMedianModifierGlobal = (SELECT ScienceMedianModifierGlobal FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type=NEW.BuildingClassType)),
CultureMedianModifierGlobal = (SELECT CultureMedianModifierGlobal FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type=NEW.BuildingClassType)),
ReligiousUnrestModifierGlobal = (SELECT ReligiousUnrestModifierGlobal FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type=NEW.BuildingClassType))

WHERE Type = New.BuildingType;

END;


CREATE TRIGGER Buildings_Tourism
AFTER INSERT ON Civilization_BuildingClassOverrides
WHEN EXISTS (SELECT BuildingType FROM Civilization_BuildingClassOverrides WHERE BuildingType = NEW.BuildingType)
BEGIN

	UPDATE Buildings
	SET EventTourism = (SELECT EventTourism FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
	    FinishLandTRTourism = (SELECT FinishLandTRTourism FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
	    FinishSeaTRTourism = (SELECT FinishSeaTRTourism FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType))
	WHERE Type = New.BuildingType;
END;