CREATE TRIGGER HappinessMod_Buildings
AFTER INSERT ON Civilization_BuildingClassOverrides
WHEN EXISTS (SELECT BuildingType FROM Civilization_BuildingClassOverrides WHERE BuildingType = NEW.BuildingType) AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1)
BEGIN

UPDATE Buildings
SET Happiness = (SELECT Happiness FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),

PovertyHappinessChange = (SELECT PovertyHappinessChange FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
DefenseHappinessChange = (SELECT DefenseHappinessChange FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
IlliteracyHappinessChange = (SELECT IlliteracyHappinessChange FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
UnculturedHappinessChange = (SELECT UnculturedHappinessChange FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
MinorityHappinessChange = (SELECT MinorityHappinessChange FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),

PovertyHappinessChangeGlobal=(SELECT PovertyHappinessChangeGlobal FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type=NEW.BuildingClassType)),
DefenseHappinessChangeGlobal=(SELECT DefenseHappinessChangeGlobal FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type=NEW.BuildingClassType)),
IlliteracyHappinessChangeGlobal=(SELECT IlliteracyHappinessChangeGlobal FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses 
WHERE Type=NEW.BuildingClassType)),
UnculturedHappinessChangeGlobal=(SELECT UnculturedHappinessChangeGlobal FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type=NEW.BuildingClassType)),
MinorityHappinessChangeGlobal=(SELECT MinorityHappinessChangeGlobal FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type=NEW.BuildingClassType))

WHERE Type = New.BuildingType;

END;


CREATE TRIGGER Buildings_Tourism
AFTER INSERT ON Civilization_BuildingClassOverrides
WHEN EXISTS (SELECT BuildingType FROM Civilization_BuildingClassOverrides WHERE BuildingType = NEW.BuildingType) AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1)
BEGIN

	UPDATE Buildings
	SET EventTourism = (SELECT EventTourism FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
	    FinishLandTRTourism = (SELECT FinishLandTRTourism FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType)),
	    FinishSeaTRTourism = (SELECT FinishSeaTRTourism FROM Buildings WHERE Type IN (SELECT DefaultBuilding FROM BuildingClasses WHERE Type = NEW.BuildingClassType))
	WHERE Type = New.BuildingType;
	
END;
	