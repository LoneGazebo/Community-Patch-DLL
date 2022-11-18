-- Drop the difficulty tables, replaced in DifficultyTables.xml
DELETE FROM PostDefines WHERE Name = 'STANDARD_HANDICAP';
DELETE FROM PostDefines WHERE Name = 'STANDARD_HANDICAP_QUICK';
DELETE FROM PostDefines WHERE Name = 'MULTIPLAYER_HANDICAP';
DELETE FROM PostDefines WHERE Name = 'AI_HANDICAP';
DELETE FROM PostDefines WHERE Name = 'BARBARIAN_HANDICAP';
DELETE FROM PostDefines WHERE Name = 'MINOR_CIV_HANDICAP';
DROP TABLE HandicapInfo_FreeTechs;
DROP TABLE HandicapInfo_AIFreeTechs;
DROP TABLE HandicapInfo_Goodies;
DROP TABLE HandicapInfos;

-- Add support for new rewards from Ancient Ruins
INSERT INTO GoodyHuts
	(Type, Description, ChooseDescription, Sound, Production, GoldenAge, FreeTiles, Science)
VALUES
	('GOODY_PRODUCTION', 'TXT_KEY_GOODY_PRODUCTION', 'TXT_KEY_GOODY_CHOOSE_PRODUCTION', 'AS2D_GOODY_WARRIOR', 30, 0, 0, 0),
	('GOODY_GOLDEN_AGE', 'TXT_KEY_GOODY_GOLDEN_AGE', 'TXT_KEY_GOODY_CHOOSE_GOLDEN_AGE', 'AS2D_GOODY_WARRIOR', 0, 200, 0, 0),
	('GOODY_TILES', 'TXT_KEY_GOODY_TILES', 'TXT_KEY_GOODY_CHOOSE_FREE_TILES', 'AS2D_GOODY_WARRIOR', 0, 0, 4, 0);