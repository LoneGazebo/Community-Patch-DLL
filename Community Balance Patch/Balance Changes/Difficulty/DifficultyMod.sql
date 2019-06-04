

--Difficulties
DELETE FROM HandicapInfos;
DELETE FROM HandicapInfo_AIFreeTechs;

CREATE TABLE IDRemapper ( id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT );
INSERT INTO IDRemapper (Type) SELECT Type FROM HandicapInfos ORDER BY ID;
UPDATE HandicapInfos SET ID = ( SELECT IDRemapper.id-1 FROM IDRemapper WHERE HandicapInfos.Type = IDRemapper.Type);
DROP TABLE IDRemapper;

UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM HandicapInfos)-1
WHERE name = 'HandicapInfos';

-- Goody Hut Payout

UPDATE GoodyHuts
SET GoldRandAmount = '20'
WHERE Type = 'GOODY_GOLD';

UPDATE GoodyHuts
SET GoldRandAmount = '20'
WHERE Type = 'GOODY_LOW_GOLD';

UPDATE GoodyHuts
SET GoldRandAmount = '20'
WHERE Type = 'GOODY_HIGH_GOLD';

UPDATE GoodyHuts
SET Experience = '20'
WHERE Type = 'GOODY_EXPERIENCE';