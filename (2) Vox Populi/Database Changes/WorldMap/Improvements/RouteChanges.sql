-- More expensive railroads (+1)
UPDATE Routes
SET GoldMaintenance = 3
WHERE Type = 'ROUTE_RAILROAD';

UPDATE Routes
SET Movement = 50, FlatMovement = 50
WHERE Type = 'ROUTE_ROAD';

UPDATE Routes
SET Movement = 25, FlatMovement = 25
WHERE Type = 'ROUTE_RAILROAD';

-- Faster roads from tech
INSERT INTO Route_TechMovementChanges
	(RouteType, TechType, MovementChange)
VALUES
	-- Road at unlock									-- 50
	('ROUTE_ROAD', 'TECH_CONSTRUCTION', -10),			-- 40
	-- ('ROUTE_ROAD', 'TECH_MACHINERY', -10),			-- 30
	('ROUTE_ROAD', 'TECH_GUNPOWDER', -5),				-- 25
	-- Railroad at unlock								-- 25
	('ROUTE_RAILROAD', 'TECH_COMBUSTION', -10),			-- 15
	('ROUTE_RAILROAD', 'TECH_COMBINED_ARMS', -5),		-- 10
	('ROUTE_RAILROAD', 'TECH_MOBILE_TACTICS', -5);		-- 5
