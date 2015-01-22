-- Civ IV Diplomacy Features

UPDATE CustomModOptions
SET Value = '1'
WHERE Name = 'DIPLOMACY_CIV4_FEATURES' AND EXISTS (SELECT * FROM Defines WHERE Name='SHARE_OPINION_RAND' AND Value= 100 );