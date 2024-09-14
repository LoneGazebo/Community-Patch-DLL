UPDATE Improvements SET NoFollowUp = 1 WHERE Type IN ('IMPROVEMENT_CITADEL', 'IMPROVEMENT_FORT', 'IMPROVEMENT_BARBARIAN_CAMP');

-- Building fishing boats shouldn't consume military units
UPDATE Builds SET KillOnlyCivilian = 1 WHERE Type = 'BUILD_FISHING_BOATS';
