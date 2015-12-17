CREATE TABLE IDRemapper ( id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT );
INSERT INTO IDRemapper (Type) SELECT Type FROM UnitPromotions ORDER BY ID;
UPDATE UnitPromotions SET ID = ( SELECT IDRemapper.id-1 FROM IDRemapper WHERE UnitPromotions.Type = IDRemapper.Type);
DROP TABLE IDRemapper;

UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM UnitPromotions)-1
WHERE name = 'UnitPromotions';

CREATE TABLE IDRemapper ( id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT );
INSERT INTO IDRemapper (Type) SELECT Type FROM UnitClasses ORDER BY ID;
UPDATE UnitClasses SET ID = ( SELECT IDRemapper.id-1 FROM IDRemapper WHERE UnitClasses.Type = IDRemapper.Type);
DROP TABLE IDRemapper;

UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM UnitClasses)-1
WHERE name = 'UnitClasses';

CREATE TABLE IDRemapper ( id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT );
INSERT INTO IDRemapper (Type) SELECT Type FROM Units ORDER BY ID;
UPDATE Units SET ID = ( SELECT IDRemapper.id-1 FROM IDRemapper WHERE Units.Type = IDRemapper.Type);
DROP TABLE IDRemapper;

UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM Units)-1
WHERE name = 'Units';