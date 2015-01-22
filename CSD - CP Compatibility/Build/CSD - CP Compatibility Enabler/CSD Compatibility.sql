-- CSD

UPDATE CustomModOptions
SET Value = '1'
WHERE Name = 'DIPLOMACY_CITYSTATES' AND EXISTS(SELECT name FROM sqlite_master WHERE type='table' AND name='CSD');

UPDATE CustomModOptions
SET Value = '1'
WHERE Name = 'DIPLOMACY_CITYSTATES_QUESTS' AND EXISTS(SELECT name FROM sqlite_master WHERE type='table' AND name='CSD'); 

UPDATE CustomModOptions
SET Value = '1'
WHERE Name = 'DIPLOMACY_CITYSTATES_RESOLUTIONS' AND EXISTS(SELECT name FROM sqlite_master WHERE type='table' AND name='CSD');

UPDATE CustomModOptions
SET Value = '1'
WHERE Name = 'DIPLOMACY_CITYSTATES_HURRY' AND EXISTS(SELECT name FROM sqlite_master WHERE type='table' AND name='CSD');

UPDATE Defines
SET Value = '50'
WHERE Name = 'AI_HOMELAND_MOVE_PRIORITY_DIPLOMAT_EMBASSY' AND EXISTS(SELECT name FROM sqlite_master WHERE type='table' AND name='CSD');

UPDATE Defines
SET Value = '50'
WHERE Name = 'AI_HOMELAND_MOVE_PRIORITY_MESSENGER' AND EXISTS(SELECT name FROM sqlite_master WHERE type='table' AND name='CSD');

UPDATE Defines
SET Value = '3'
WHERE Name = 'LEAGUE_PROPOSERS_PER_SESSION' AND EXISTS(SELECT name FROM sqlite_master WHERE type='table' AND name='CSD');

UPDATE Defines
SET Value = '3'
WHERE Name = 'LEAGUE_NUM_LEADERS_FOR_EXTRA_VOTES' AND EXISTS(SELECT name FROM sqlite_master WHERE type='table' AND name='CSD');

UPDATE Defines
SET Value = '33'
WHERE Name = 'LEAGUE_PROJECT_PROGRESS_PERCENT_WARNING' AND EXISTS(SELECT name FROM sqlite_master WHERE type='table' AND name='CSD');