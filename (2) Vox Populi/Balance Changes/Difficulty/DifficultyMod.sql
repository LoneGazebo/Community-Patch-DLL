-- Delete the data in the difficulty tables, replaced in DifficultyMod.xml
DELETE FROM PostDefines WHERE Name = 'STANDARD_HANDICAP';
DELETE FROM PostDefines WHERE Name = 'STANDARD_HANDICAP_QUICK';
DELETE FROM PostDefines WHERE Name = 'MULTIPLAYER_HANDICAP';
DELETE FROM PostDefines WHERE Name = 'AI_HANDICAP';
DELETE FROM PostDefines WHERE Name = 'BARBARIAN_HANDICAP';
DELETE FROM PostDefines WHERE Name = 'MINOR_CIV_HANDICAP';
DELETE FROM HandicapInfo_FreeTechs;
DELETE FROM HandicapInfo_AIFreeTechs;
DELETE FROM HandicapInfo_Goodies;
DELETE FROM HandicapInfos;