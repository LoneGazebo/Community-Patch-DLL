-- Make Kabul's City-State color match the other City-States that are from major civs
UPDATE MinorCivilizations SET DefaultPlayerColor = 'PLAYERCOLOR_MINOR_PERSIA' WHERE Type = 'MINOR_CIV_KABUL';

-- Add the capital city name for all new City-States
INSERT INTO MinorCivilization_CityNames
	(MinorCivType, CityName)
SELECT a.Type, a.Description
FROM MinorCivilizations a
LEFT JOIN MinorCivilization_CityNames b ON a.Type = b.MinorCivType WHERE b.MinorCivType IS NULL;

-- Block the newly-added City-States from appearing ingame if their associated civilization is present
INSERT INTO MajorBlocksMinor
	(MinorCiv, MajorCiv)
VALUES
	-- Reactivated base game City-States
	('MINOR_CIV_COPENHAGEN', 'CIVILIZATION_DENMARK'),
	('MINOR_CIV_DUBLIN', 'CIVILIZATION_CELTS'),
	('MINOR_CIV_EDINBURGH', 'CIVILIZATION_CELTS'),
	('MINOR_CIV_HELSINKI', 'CIVILIZATION_SWEDEN'),
	('MINOR_CIV_JAKARTA', 'CIVILIZATION_INDONESIA'),
	('MINOR_CIV_LISBON', 'CIVILIZATION_PORTUGAL'),
	('MINOR_CIV_MARRAKECH', 'CIVILIZATION_MOROCCO'),
	('MINOR_CIV_OSLO', 'CIVILIZATION_DENMARK'),
	('MINOR_CIV_RIO_DE_JANEIRO', 'CIVILIZATION_BRAZIL'),
	('MINOR_CIV_SEOUL', 'CIVILIZATION_KOREA'),
	('MINOR_CIV_STOCKHOLM', 'CIVILIZATION_SWEDEN'),
	('MINOR_CIV_VENICE', 'CIVILIZATION_VENICE'),
	('MINOR_CIV_VIENNA', 'CIVILIZATION_AUSTRIA'),
	('MINOR_CIV_WARSAW', 'CIVILIZATION_POLAND'),
	-- Into the Renaissance scenario
	('MINOR_CIV_COLOGNE', 'CIVILIZATION_GERMANY'),
	('MINOR_CIV_GDANSK', 'CIVILIZATION_POLAND'),
	('MINOR_CIV_MECCA', 'CIVILIZATION_ARABIA'),
	('MINOR_CIV_TUNIS', 'CIVILIZATION_ARABIA'),
	-- Scramble for Africa scenario
	('MINOR_CIV_SEGOU', 'CIVILIZATION_SONGHAI');