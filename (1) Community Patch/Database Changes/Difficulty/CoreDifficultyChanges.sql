CREATE TEMP TABLE GoodyHutRewards (
	GoodyType text
);

INSERT INTO GoodyHutRewards
	(GoodyType)
VALUES
	('GOODY_POPULATION'),
	('GOODY_CULTURE'),
	('GOODY_PANTHEON_FAITH'),
	('GOODY_PROPHET_FAITH'),
	('GOODY_GOLD'),
	('GOODY_MAP'),
	('GOODY_TECH'),
	('GOODY_REVEAL_NEARBY_BARBS'),
	('GOODY_UPGRADE_UNIT');

-- Settler-exclusive rewards
INSERT INTO HandicapInfo_Goodies
	(HandicapType, GoodyType)
VALUES
	('HANDICAP_SETTLER', 'GOODY_WORKER'),
	('HANDICAP_SETTLER', 'GOODY_SETTLER');

-- General rewards
INSERT INTO HandicapInfo_Goodies
	(HandicapType, GoodyType)
SELECT
	a.Type, b.GoodyType
FROM HandicapInfos a, GoodyHutRewards b;

DROP TABLE GoodyHutRewards;
