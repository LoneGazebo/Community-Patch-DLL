UPDATE Improvements SET NoFollowUp = 1 WHERE Type IN ('IMPROVEMENT_CITADEL', 'IMPROVEMENT_FORT', 'IMPROVEMENT_BARBARIAN_CAMP');

-- Note: Does not take effect unless GLOBAL_PASSABLE_FORTS=1
UPDATE Improvements SET MakesPassable = 1 WHERE Type IN ('IMPROVEMENT_CITADEL', 'IMPROVEMENT_FORT');

-- Use Improvement_YieldPerXAdjacentImprovement instead
UPDATE Improvements SET CultureAdjacentSameType = 0 WHERE Type = 'IMPROVEMENT_MOAI';
INSERT INTO Improvement_YieldPerXAdjacentImprovement
	(ImprovementType, OtherImprovementType, YieldType, Yield, NumRequired)
VALUES
	('IMPROVEMENT_MOAI', 'IMPROVEMENT_MOAI', 'YIELD_CULTURE', 1, 1);
