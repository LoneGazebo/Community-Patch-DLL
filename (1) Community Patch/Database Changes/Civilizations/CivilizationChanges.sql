-- New spy names (supports up to 25 spies)
CREATE TEMP TABLE SpyNumbers (
	SpyNumber TEXT
);

INSERT INTO SpyNumbers VALUES ('10'), ('11'), ('12'), ('13'), ('14'), ('15'), ('16'), ('17'), ('18'), ('19'), ('20'), ('21'), ('22'), ('23'), ('24');

INSERT INTO Civilization_SpyNames
	(CivilizationType, SpyName)
SELECT
	a.Type, 'TXT_KEY_SPY_NAME_' || substr(a.Type, 14) || '_' || b.SpyNumber
FROM Civilizations a, SpyNumbers b
WHERE a.Type NOT IN ('CIVILIZATION_MINOR', 'CIVILIZATION_BARBARIAN')
ORDER BY a.Type;

DROP TABLE SpyNumbers;

-- Block Kabul from appearing if Persia is ingame, as Kabul is on Persia's city list
INSERT INTO MajorBlocksMinor
	(MajorCiv, MinorCiv)
VALUES
	('CIVILIZATION_PERSIA', 'MINOR_CIV_KABUL');

-- Delete duplicated entry
DELETE FROM Civilization_UnitClassOverrides
WHERE CivilizationType = 'CIVILIZATION_BARBARIAN'
AND UnitClassType = 'UNITCLASS_CHARIOT_ARCHER'
AND UnitType IS NULL;
