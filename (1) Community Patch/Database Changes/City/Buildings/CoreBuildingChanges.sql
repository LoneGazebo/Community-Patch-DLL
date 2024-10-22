-- ExtraLeagueVotes is now used for scaling votes. SingleLeagueVotes has the original behavior.
UPDATE Buildings
SET SingleLeagueVotes = 2, ExtraLeagueVotes = 0
WHERE Type = 'BUILDING_FORBIDDEN_PALACE';
