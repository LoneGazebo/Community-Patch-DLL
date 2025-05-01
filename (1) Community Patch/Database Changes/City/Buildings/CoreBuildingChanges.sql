-- ExtraLeagueVotes is now used for scaling votes. SingleLeagueVotes has the original behavior.
UPDATE Buildings
SET SingleLeagueVotes = 2, ExtraLeagueVotes = 0
WHERE Type = 'BUILDING_FORBIDDEN_PALACE';

-- Fix wrong text key for Monument class
UPDATE BuildingClasses
SET Description = 'TXT_KEY_BUILDING_MONUMENT_DESC'
WHERE Type = 'BUILDINGCLASS_MONUMENT';
