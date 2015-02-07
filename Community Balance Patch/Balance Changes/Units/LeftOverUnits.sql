-- Borrowed from Whoward's tutorial on triggers

CREATE TRIGGER Civilizations
AFTER INSERT ON Civilizations
WHEN EXISTS (SELECT Type FROM Civilizations WHERE Type='CIVILIZATION_'||NEW.Type)
BEGIN
	INSERT INTO Civilization_UnitClassOverrides(CivilizationType, UnitClassType, UnitType)
    VALUES('CIVILIZATION_'||NEW.Type, UNITCLASS_LONGBOWMAN, );
	INSERT INTO Civilization_UnitClassOverrides(CivilizationType, UnitClassType, UnitType)
    VALUES('CIVILIZATION_'||NEW.Type, UNITCLASS_COMPANIONCAVALRY, );
	INSERT INTO Civilization_UnitClassOverrides(CivilizationType, UnitClassType, UnitType)
    VALUES('CIVILIZATION_'||NEW.Type, UNITCLASS_SIPAHI, );
	INSERT INTO Civilization_UnitClassOverrides(CivilizationType, UnitClassType, UnitType)
    VALUES('CIVILIZATION_'||NEW.Type, UNITCLASS_BALLISTA, );
	INSERT INTO Civilization_UnitClassOverrides(CivilizationType, UnitClassType, UnitType)
    VALUES('CIVILIZATION_'||NEW.Type, UNITCLASS_TERCIO, );
	INSERT INTO Civilization_UnitClassOverrides(CivilizationType, UnitClassType, UnitType)
    VALUES('CIVILIZATION_'||NEW.Type, UNITCLASS_HAKKAPELIITTA, );
	INSERT INTO Civilization_UnitClassOverrides(CivilizationType, UnitClassType, UnitType)
    VALUES('CIVILIZATION_'||NEW.Type, UNITCLASS_COMANCHE_RIDERS, );
	INSERT INTO Civilization_UnitClassOverrides(CivilizationType, UnitClassType, UnitType)
    VALUES('CIVILIZATION_'||NEW.Type, UNITCLASS_VENETIAN_GALLEASS, );
	INSERT INTO Civilization_UnitClassOverrides(CivilizationType, UnitClassType, UnitType)
    VALUES('CIVILIZATION_'||NEW.Type, UNITCLASS_BYZANTINE_DROMON, );
	INSERT INTO Civilization_UnitClassOverrides(CivilizationType, UnitClassType, UnitType)
    VALUES('CIVILIZATION_'||NEW.Type, UNITCLASS_KOREAN_TURTLE_SHIP, );
END;