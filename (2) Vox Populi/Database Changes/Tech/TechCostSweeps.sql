CREATE TEMP TABLE TechCosts (
	X INTEGER,
	TechCost INTEGER
);

INSERT INTO TechCosts
VALUES
	(0, 20),
	(1, 60),
	(2, 100),
	(3, 130),
	(4, 275),
	(5, 500),
	(6, 700),
	(7, 1750),
	(8, 2400),
	(9, 3600),
	(10, 5150),
	(11, 8100),
	(12, 11000),
	(13, 13000),
	(14, 16000),
	(15, 19100),
	(16, 23400),
	(17, 24450),
	(18, 28550);

UPDATE Technologies
SET Cost = (SELECT TechCost FROM TechCosts WHERE X = GridX)
WHERE EXISTS (SELECT 1 FROM TechCosts WHERE X = GridX);

DROP TABLE TechCosts;
