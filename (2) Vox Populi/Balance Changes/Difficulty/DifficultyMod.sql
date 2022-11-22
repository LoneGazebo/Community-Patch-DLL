-- Delete the data in the difficulty tables, replaced in DifficultyMod.xml
DELETE FROM HandicapInfo_FreeTechs;
DELETE FROM HandicapInfo_AIFreeTechs;
DELETE FROM HandicapInfo_Goodies;
DELETE FROM HandicapInfos;

CREATE TABLE IDRemapper ( id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT );
INSERT INTO IDRemapper (Type) SELECT Type FROM HandicapInfos ORDER BY ID;
UPDATE HandicapInfos SET ID = ( SELECT IDRemapper.id-1 FROM IDRemapper WHERE HandicapInfos.Type = IDRemapper.Type);
DROP TABLE IDRemapper;

UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM HandicapInfos)-1
WHERE name = 'HandicapInfos';