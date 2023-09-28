-- Process yields
UPDATE Processes SET DefenseValue = 10 WHERE Type = 'PROCESS_DEFENSE';

INSERT INTO Process_ProductionYields
	(ProcessType, YieldType, Yield)
VALUES
	('PROCESS_CULTURE', 'YIELD_CULTURE', 25),
	('PROCESS_FOOD', 'YIELD_FOOD', 25),
	('PROCESS_TOURISM', 'YIELD_TOURISM', 15);

-- Prereq tech
UPDATE Processes SET TechPrereq = 'TECH_CURRENCY' WHERE Type = 'PROCESS_WEALTH';
