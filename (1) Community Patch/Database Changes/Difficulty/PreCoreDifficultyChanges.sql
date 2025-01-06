-- Delete the data in the difficulty tables, replaced in CoreDifficultyChanges.xml and CoreDifficultyChanges.sql
DELETE FROM HandicapInfo_Goodies;
DELETE FROM HandicapInfos;

-- make sure the rows inserted later have the correct IDs (don't remove this, it's needed for modpack creation to work)
UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM HandicapInfos) - 1
WHERE name = 'HandicapInfos';
