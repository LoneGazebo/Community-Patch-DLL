UPDATE Unit_ClassUpgrades
SET UnitClassType = 'UNITCLASS_INFANTRY'
WHERE UnitType = 'UNIT_DANISH_SKI_INFANTRY';

DELETE FROM Unit_AITypes
WHERE UnitType = 'UNIT_VENETIAN_GALLEASS' AND UnitAIType = 'UNITAI_EXPLORE_SEA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Borrowed from Whoward's tutorial on triggers

--CREATE TRIGGER Civilizations
--AFTER INSERT ON Civilizations
--WHEN EXISTS (SELECT Type FROM Civilizations WHERE Type='CIVILIZATION_'||NEW.Type)
--BEGIN
--	INSERT INTO Civilization_UnitClassOverrides(CivilizationType, UnitClassType, UnitType)
  --  VALUES('CIVILIZATION_'||NEW.Type, UNITCLASS_LONGBOWMAN, );
--	INSERT INTO Civilization_UnitClassOverrides(CivilizationType, UnitClassType, UnitType)
 --   VALUES('CIVILIZATION_'||NEW.Type, UNITCLASS_COMPANIONCAVALRY, );
--	INSERT INTO Civilization_UnitClassOverrides(CivilizationType, UnitClassType, UnitType)
--    VALUES('CIVILIZATION_'||NEW.Type, UNITCLASS_SIPAHI, );
--	INSERT INTO Civilization_UnitClassOverrides(CivilizationType, UnitClassType, UnitType)
 --   VALUES('CIVILIZATION_'||NEW.Type, UNITCLASS_BALLISTA, );
--	INSERT INTO Civilization_UnitClassOverrides(CivilizationType, UnitClassType, UnitType)
 --   VALUES('CIVILIZATION_'||NEW.Type, UNITCLASS_TERCIO, );
--	INSERT INTO Civilization_UnitClassOverrides(CivilizationType, UnitClassType, UnitType)
 --   VALUES('CIVILIZATION_'||NEW.Type, UNITCLASS_HAKKAPELIITTA, );
--	INSERT INTO Civilization_UnitClassOverrides(CivilizationType, UnitClassType, UnitType)
 --   VALUES('CIVILIZATION_'||NEW.Type, UNITCLASS_COMANCHE_RIDERS, );
--	INSERT INTO Civilization_UnitClassOverrides(CivilizationType, UnitClassType, UnitType)
 --   VALUES('CIVILIZATION_'||NEW.Type, UNITCLASS_VENETIAN_GALLEASS, );
--	INSERT INTO Civilization_UnitClassOverrides(CivilizationType, UnitClassType, UnitType)
 --   VALUES('CIVILIZATION_'||NEW.Type, UNITCLASS_KOREAN_TURTLE_SHIP, );
--END;