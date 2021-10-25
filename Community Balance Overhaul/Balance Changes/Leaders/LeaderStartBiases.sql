-- Start biases
INSERT INTO Civilization_Start_Along_River
	(CivilizationType, StartAlongRiver)
VALUES
	('CIVILIZATION_EGYPT', '1'),
	('CIVILIZATION_INDIA', '1'),
	('CIVILIZATION_BABYLON', '1'),
	('CIVILIZATION_CHINA', '1'),
	('CIVILIZATION_ASSYRIA', '1');

INSERT INTO Civilization_Start_Along_Ocean
	(CivilizationType, StartAlongOcean)
VALUES
	('CIVILIZATION_NETHERLANDS', '1');

DELETE FROM Civilization_Start_Region_Avoid
WHERE CivilizationType = 'CIVILIZATION_SIAM';

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_INDIA';

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_CELTS';

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_AUSTRIA';

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_FRANCE';

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_GERMANY';

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_CELTS';

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_ROME';

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_PERSIA';

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_ETHIOPIA';

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_INCA';

INSERT INTO Civilization_Start_Region_Priority
	(CivilizationType, RegionType)
VALUES
	('CIVILIZATION_OTTOMAN', 'REGION_HILLS'),
	('CIVILIZATION_GREECE', 'REGION_HILLS'),
	('CIVILIZATION_AMERICA', 'REGION_FOREST'),
	('CIVILIZATION_SIAM', 'REGION_JUNGLE'),
	('CIVILIZATION_SIAM', 'REGION_FOREST'),
	('CIVILIZATION_HUNS', 'REGION_PLAINS'),
	('CIVILIZATION_ZULU', 'REGION_PLAINS'),
	('CIVILIZATION_POLAND', 'REGION_GRASS'),
	('CIVILIZATION_FRANCE', 'REGION_GRASS'),
	('CIVILIZATION_GERMANY', 'REGION_GRASS'),
	('CIVILIZATION_CELTS', 'REGION_HILLS'),
	('CIVILIZATION_ROME', 'REGION_GRASS'),
	('CIVILIZATION_SHOSHONE', 'REGION_GRASS'),
	('CIVILIZATION_GERMANY', 'REGION_FOREST'),
	('CIVILIZATION_MAYA', 'REGION_JUNGLE'),
	('CIVILIZATION_PERSIA', 'REGION_HILLS'),
	('CIVILIZATION_ETHIOPIA', 'REGION_HILLS'),
	('CIVILIZATION_EGYPT', 'REGION_DESERT');

DELETE FROM Civilization_Start_Along_Ocean
WHERE CivilizationType = 'CIVILIZATION_KOREA';

DELETE FROM Civilization_Start_Along_Ocean
WHERE CivilizationType = 'CIVILIZATION_JAPAN';

DELETE FROM Civilization_Start_Along_Ocean
WHERE CivilizationType = 'CIVILIZATION_BYZANTIUM';

DELETE FROM Civilization_Start_Along_Ocean
WHERE CivilizationType = 'CIVILIZATION_INDONESIA';

DELETE FROM Civilization_Start_Along_Ocean
WHERE CivilizationType = 'CIVILIZATION_OTTOMAN';

INSERT INTO Civilization_Start_Prefer_Mountain
	(CivilizationType, StartPreferMountain)
VALUES
	('CIVILIZATION_INCA', 1);