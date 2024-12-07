-- Extra Food/Production in Internal Trade Routes, extra Unit Supply
CREATE TEMP TABLE EraSweep (
	Era TEXT,
	Food INTEGER,
	Production INTEGER,
	Supply INTEGER
);

INSERT INTO EraSweep
VALUES
	('ERA_CLASSICAL', 100, 100, 1),
	('ERA_MEDIEVAL', 200, 200, 2),
	('ERA_RENAISSANCE', 400, 400, 3),
	('ERA_INDUSTRIAL', 600, 600, 4),
	('ERA_MODERN', 800, 800, 5),
	('ERA_POSTMODERN', 1000, 1000, 6),
	('ERA_FUTURE', 1200, 1200, 7);

UPDATE Eras
SET
	TradeRouteFoodBonusTimes100 = (SELECT Food FROM EraSweep WHERE Era = Type),
	TradeRouteProductionBonusTimes100 = (SELECT Production FROM EraSweep WHERE Era = Type),
	UnitSupplyBase = (SELECT Supply FROM EraSweep WHERE Era = Type)
WHERE EXISTS (SELECT 1 FROM EraSweep WHERE Era = Type);

DROP TABLE EraSweep;
