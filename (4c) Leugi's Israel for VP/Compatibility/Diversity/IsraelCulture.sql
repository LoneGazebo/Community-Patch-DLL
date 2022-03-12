--==========================================================================================================================
-- Civilization_JFD_CultureTypes
--==========================================================================================================================
CREATE TABLE IF NOT EXISTS Civilization_JFD_CultureTypes(CivilizationType, CultureType, SplashScreenTag, SoundtrackTag);
 
INSERT INTO Civilization_JFD_CultureTypes
            (CivilizationType,              CultureType,		SplashScreenTag,    SoundtrackTag)
VALUES      ('CIVILIZATION_LEUGI_ISRAEL',   'JFD_Mesopotamic',  'JFD_Mesopotamic',	'JFD_Mesopotamic');
--==========================================================================================================================
--==========================================================================================================================

UPDATE Civilizations 
SET SoundtrackTag = 'JFD_Mesopotamic'
WHERE Type IN (SELECT CivilizationType FROM Civilization_JFD_CultureTypes WHERE SoundtrackTag = 'JFD_Mesopotamic');

--==========================================================================================================================