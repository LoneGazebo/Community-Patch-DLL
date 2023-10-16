-- This code is necessary to avoid a UI glitch, do not remove it
CREATE TABLE IDRemapper (id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT);
INSERT INTO IDRemapper (Type) SELECT Type FROM HandicapInfos ORDER BY ID;
UPDATE HandicapInfos SET ID = (SELECT IDRemapper.id-1 FROM IDRemapper WHERE HandicapInfos.Type = IDRemapper.Type);
DROP TABLE IDRemapper;

UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM HandicapInfos)-1
WHERE name = 'HandicapInfos';
-- End UI glitch fix

CREATE TABLE IDRemapper (id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT);
INSERT INTO IDRemapper (Type) SELECT Type FROM UnitPromotions ORDER BY ID;
UPDATE UnitPromotions SET ID = (SELECT IDRemapper.id - 1 FROM IDRemapper WHERE UnitPromotions.Type = IDRemapper.Type);
DROP TABLE IDRemapper;

UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM UnitPromotions) - 1
WHERE name = 'UnitPromotions';

CREATE TABLE IDRemapper (id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT);
INSERT INTO IDRemapper (Type) SELECT Type FROM UnitClasses ORDER BY ID;
UPDATE UnitClasses SET ID = (SELECT IDRemapper.id - 1 FROM IDRemapper WHERE UnitClasses.Type = IDRemapper.Type);
DROP TABLE IDRemapper;

UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM UnitClasses) - 1
WHERE name = 'UnitClasses';

CREATE TABLE IDRemapper (id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT);
INSERT INTO IDRemapper (Type) SELECT Type FROM Units ORDER BY ID;
UPDATE Units SET ID = (SELECT IDRemapper.id - 1 FROM IDRemapper WHERE Units.Type = IDRemapper.Type);
DROP TABLE IDRemapper;

UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM Units) - 1
WHERE name = 'Units';

CREATE TABLE IDRemapper (id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT);
INSERT INTO IDRemapper (Type) SELECT Type FROM BuildingClasses ORDER BY ID;
UPDATE BuildingClasses SET ID = (SELECT IDRemapper.id - 1 FROM IDRemapper WHERE BuildingClasses.Type = IDRemapper.Type);
DROP TABLE IDRemapper;

UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM BuildingClasses) - 1
WHERE name = 'BuildingClasses';

CREATE TABLE IDRemapper (id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT);
INSERT INTO IDRemapper (Type) SELECT Type FROM Buildings ORDER BY ID;
UPDATE Buildings SET ID = (SELECT IDRemapper.id - 1 FROM IDRemapper WHERE Buildings.Type = IDRemapper.Type);
DROP TABLE IDRemapper;

UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM Buildings) - 1
WHERE name = 'Buildings';

CREATE TABLE IDRemapper (id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT);
INSERT INTO IDRemapper (Type) SELECT Type FROM Resources ORDER BY ID;
UPDATE Resources SET ID = (SELECT IDRemapper.id - 1 FROM IDRemapper WHERE Resources.Type = IDRemapper.Type);
DROP TABLE IDRemapper;

UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM Resources) - 1
WHERE name = 'Resources';

CREATE TABLE IDRemapper (id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT);
INSERT INTO IDRemapper (Type) SELECT Type FROM Improvements ORDER BY ID;
UPDATE Improvements SET ID = (SELECT IDRemapper.id - 1 FROM IDRemapper WHERE Improvements.Type = IDRemapper.Type);
DROP TABLE IDRemapper;

UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM Improvements) - 1
WHERE name = 'Improvements';

CREATE TABLE IDRemapper (id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT);
INSERT INTO IDRemapper (Type) SELECT Type FROM Beliefs ORDER BY ID;
UPDATE Beliefs SET ID = (SELECT IDRemapper.id - 1 FROM IDRemapper WHERE Beliefs.Type = IDRemapper.Type);
DROP TABLE IDRemapper;

UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM Beliefs) - 1
WHERE name = 'Beliefs';

CREATE TABLE IDRemapper (id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT);
INSERT INTO IDRemapper (Type) SELECT Type FROM Concepts ORDER BY ID;
UPDATE Concepts SET ID = (SELECT IDRemapper.id - 1 FROM IDRemapper WHERE Concepts.Type = IDRemapper.Type);
DROP TABLE IDRemapper;

UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM Concepts) - 1
WHERE name = 'Concepts';

CREATE TABLE IDRemapper (id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT);
INSERT INTO IDRemapper (Type) SELECT Type FROM AICityStrategies ORDER BY ID;
UPDATE AICityStrategies SET ID = (SELECT IDRemapper.id - 1 FROM IDRemapper WHERE AICityStrategies.Type = IDRemapper.Type);
DROP TABLE IDRemapper;

UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM AICityStrategies) - 1
WHERE name = 'AICityStrategies';

CREATE TABLE IDRemapper (id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT);
INSERT INTO IDRemapper (Type) SELECT Type FROM MinorCivilizations ORDER BY ID;
UPDATE MinorCivilizations SET ID = (SELECT IDRemapper.id - 1 FROM IDRemapper WHERE MinorCivilizations.Type = IDRemapper.Type);
DROP TABLE IDRemapper;

UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM MinorCivilizations) - 1
WHERE name = 'MinorCivilizations';

CREATE TABLE IDRemapper (id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT);
INSERT INTO IDRemapper (Type) SELECT Type FROM PlayerColors ORDER BY ID;
UPDATE PlayerColors SET ID = (SELECT IDRemapper.id - 1 FROM IDRemapper WHERE PlayerColors.Type = IDRemapper.Type);
DROP TABLE IDRemapper;

UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM PlayerColors) - 1
WHERE name = 'PlayerColors';