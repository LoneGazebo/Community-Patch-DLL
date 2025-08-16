CREATE TEMP TABLE HandicapsList (
	HandicapType text
);

INSERT INTO HandicapsList
	(HandicapType)
VALUES
	('HANDICAP_SETTLER'),
	('HANDICAP_CHIEFTAIN'),
	('HANDICAP_WARLORD'),
	('HANDICAP_PRINCE'),
	('HANDICAP_KING'),
	('HANDICAP_EMPEROR'),
	('HANDICAP_IMMORTAL'),
	('HANDICAP_DEITY'),
	('HANDICAP_AI_DEFAULT');

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
	('GOODY_UPGRADE_UNIT'),
	('GOODY_PRODUCTION'),
	('GOODY_GOLDEN_AGE'),
	('GOODY_TILES'),
	('GOODY_SCIENCE');

-- Settler-exclusive rewards
INSERT INTO HandicapInfo_Goodies
	('HANDICAP_SETTLER', 'GOODY_WORKER'),
	('HANDICAP_SETTLER', 'GOODY_SETTLER');

-- General rewards
INSERT INTO HandicapInfo_Goodies
	(HandicapType, GoodyType)
SELECT
	a.HandicapType, b.GoodyType
FROM HandicapsList a, GoodyHutRewards b;

DROP TABLE HandicapsList;
DROP TABLE GoodyHutRewards;