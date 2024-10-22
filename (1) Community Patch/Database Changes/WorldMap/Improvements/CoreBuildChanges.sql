UPDATE Builds SET CanBeEmbarked = 1 WHERE Type = 'BUILD_FISHING_BOATS_NO_KILL';

-- Building fishing boats shouldn't consume military units
UPDATE Builds SET KillOnlyCivilian = 1 WHERE Type = 'BUILD_FISHING_BOATS';

UPDATE BuildFeatures
SET RemoveOnly = 1
WHERE BuildType IN (
	'BUILD_REMOVE_JUNGLE',
	'BUILD_REMOVE_FOREST',
	'BUILD_REMOVE_MARSH',
	'BUILD_SCRUB_FALLOUT'
);
