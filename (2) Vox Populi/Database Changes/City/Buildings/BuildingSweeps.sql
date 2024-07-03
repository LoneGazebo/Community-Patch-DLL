-----------------------------------------------------------------
-- ConquestProb and NeverCapture
-----------------------------------------------------------------

-- Base setting
UPDATE Buildings SET ConquestProb = 80, NeverCapture = 0;

-- World Wonders should always be retained
UPDATE Buildings SET ConquestProb = 100, NeverCapture = 0 WHERE WonderSplashImage IS NOT NULL;

-- National Wonders and other limited buildings should never be retained
UPDATE Buildings SET ConquestProb = 0, NeverCapture = 1
WHERE BuildingClass IN (
	SELECT Type FROM BuildingClasses
	WHERE MaxPlayerInstances <> -1
);

-- Instant benefit buildings, courthouse, offices, and franchises should never be retained
UPDATE Buildings SET ConquestProb = 0, NeverCapture = 1
WHERE BuildingClass IN (
	'BUILDINGCLASS_COURTHOUSE',
	'BUILDINGCLASS_CIRCUS',
	'BUILDINGCLASS_THEATRE',
	'BUILDINGCLASS_STADIUM',
	'BUILDINGCLASS_MEDICAL_LAB',
	'BUILDINGCLASS_TRADER_SIDS',
	'BUILDINGCLASS_TRADER_SIDS_FRANCHISE',
	'BUILDINGCLASS_LANDSEA_EXTRACTORS',
	'BUILDINGCLASS_LANDSEA_EXTRACTORS_FRANCHISE',
	'BUILDINGCLASS_HEXXON_REFINERY',
	'BUILDINGCLASS_HEXXON_REFINERY_FRANCHISE',
	'BUILDINGCLASS_GIORGIO_ARMEIER',
	'BUILDINGCLASS_GIORGIO_ARMEIER_FRANCHISE',
	'BUILDINGCLASS_FIRAXITE_MATERIALS',
	'BUILDINGCLASS_FIRAXITE_MATERIALS_FRANCHISE',
	'BUILDINGCLASS_TWOKAY_FOODS',
	'BUILDINGCLASS_TWOKAY_FOODS_FRANCHISE',
	'BUILDINGCLASS_CIVILIZED_JEWELERS',
	'BUILDINGCLASS_CIVILIZED_JEWELERS_FRANCHISE'
);

-- Defensive, military and policy-exclusive buildings cannot be captured normally
UPDATE Buildings SET ConquestProb = 0, NeverCapture = 0
WHERE BuildingClass IN (
	'BUILDINGCLASS_WALLS',
	'BUILDINGCLASS_CASTLE',
	'BUILDINGCLASS_FORTRESS',
	'BUILDINGCLASS_ARSENAL',
	'BUILDINGCLASS_MILITARY_BASE',
	'BUILDINGCLASS_BARRACKS',
	'BUILDINGCLASS_ARMORY',
	'BUILDINGCLASS_MILITARY_ACADEMY',
	'BUILDINGCLASS_MINEFIELD',
	'BUILDINGCLASS_AIRPORT',
	'BUILDINGCLASS_BOMB_SHELTER',
	'BUILDINGCLASS_OBSERVATORY',
	'BUILDINGCLASS_MONASTERY'
);

-- Religious buildings should always be retained
UPDATE Buildings SET ConquestProb = 100, NeverCapture = 0
WHERE BuildingClass IN (
	'BUILDINGCLASS_CATHEDRAL',
	'BUILDINGCLASS_MOSQUE',
	'BUILDINGCLASS_PAGODA',
	'BUILDINGCLASS_STUPA',
	'BUILDINGCLASS_CHURCH',
	'BUILDINGCLASS_MANDIR',
	'BUILDINGCLASS_SYNAGOGUE',
	'BUILDINGCLASS_ORDER',
	'BUILDINGCLASS_TEOCALLI',
	'BUILDINGCLASS_GURDWARA'
);

-----------------------------------------------------------------
-- National Wonder population requirements and cost scaling
-----------------------------------------------------------------

-- Base setting
UPDATE Buildings SET NationalPopRequired = 0, NumCityCostMod = 0;

UPDATE Buildings SET NumCityCostMod = 10 WHERE Type = 'BUILDING_COURTHOUSE';

UPDATE Buildings SET NumCityCostMod = 25
WHERE BuildingClass IN (
	SELECT Type FROM BuildingClasses
	WHERE MaxPlayerInstances = 1
);

-- Remove from unbuildable National Wonders
UPDATE Buildings SET NumCityCostMod = 0
WHERE BuildingClass IN (
	'BUILDINGCLASS_PALACE',
	'BUILDINGCLASS_PALACE_THRONE_ROOM',
	'BUILDINGCLASS_PALACE_TREASURY',
	'BUILDINGCLASS_CAPITAL_ENGINEER',
	'BUILDINGCLASS_PALACE_GARDEN',
	'BUILDINGCLASS_PALACE_COURT_CHAPEL',
	'BUILDINGCLASS_PALACE_ASTROLOGER',
	'BUILDINGCLASS_CONSULATE'
);

-- Exception for Unique National Wonders
UPDATE Buildings SET NumCityCostMod = 10
WHERE Type IN (
	'BUILDING_ROYAL_LIBRARY',
	'BUILDING_GREAT_COTHON',
	'BUILDING_PIAZZA_SAN_MARCO',
	'BUILDING_VENETIAN_ARSENALE',
	'BUILDING_MURANO_GLASSWORKS',
	'BUILDING_RIALTO_DISTRICT',
	'BUILDING_WHITE_TOWER',
	'BUILDING_AMERICA_INDEPENDENCEHALL'
);

-- Classical T1
UPDATE Buildings SET NationalPopRequired = 12
WHERE Type IN (
	'BUILDING_HEROIC_EPIC',
	'BUILDING_COURT_SCRIBE'
);

-- Classical T2
UPDATE Buildings SET NationalPopRequired = 15
WHERE Type IN (
	'BUILDING_NATIONAL_COLLEGE',
	'BUILDING_NATIONAL_EPIC',
	'BUILDING_CIRCUS_MAXIMUS'
);

-- Medieval T1
UPDATE Buildings SET NationalPopRequired = 19
WHERE Type IN (
	'BUILDING_OXFORD_UNIVERSITY',
	'BUILDING_GRAND_TEMPLE'
);

-- Medieval T2
UPDATE Buildings SET NationalPopRequired = 22
WHERE Type IN (
	'BUILDING_IRONWORKS',
	'BUILDING_NATIONAL_TREASURY'
);

-- Renaissance T1
UPDATE Buildings SET NationalPopRequired = 25
WHERE Type IN (
	'BUILDING_PRINTING_PRESS'
);

-- Renaissance T2
UPDATE Buildings SET NationalPopRequired = 30
WHERE Type IN (
	'BUILDING_HERMITAGE'
);

-- Modern T2 + Ideology NW
UPDATE Buildings SET NationalPopRequired = 40
WHERE Type IN (
	'BUILDING_FOREIGN_OFFICE',
	'BUILDING_PALACE_SCIENCE_CULTURE',
	'BUILDING_FINANCE_CENTER',
	'BUILDING_EHRENHALLE'
);

-- Atomic T2
UPDATE Buildings SET NationalPopRequired = 50
WHERE Type IN (
	'BUILDING_INTELLIGENCE_AGENCY'
);

-----------------------------------------------------------------
-- World Wonder policy requirements
-----------------------------------------------------------------
CREATE TEMP TABLE WonderPolicyNeeded (
	GridXTemp INTEGER,
	NumPolicies INTEGER
);

INSERT INTO WonderPolicyNeeded
VALUES
	(1, 0),
	(2, 2),
	(3, 4),
	(4, 5),
	(5, 7),
	(6, 8),
	(7, 10),
	(8, 11),
	(9, 14),
	(10, 16),
	(11, 18),
	(12, 20),
	(14, 22),
	(15, 25),
	(16, 27),
	(17, 27);

UPDATE Buildings
SET NumPoliciesNeeded = (
	SELECT NumPolicies FROM WonderPolicyNeeded WHERE PrereqTech IN (
		SELECT Type FROM Technologies WHERE GridX = GridXTemp
	)
)
WHERE WonderSplashImage IS NOT NULL
AND PolicyType IS NULL
AND EXISTS (
	SELECT 1 FROM WonderPolicyNeeded WHERE PrereqTech IN (
		SELECT Type FROM Technologies WHERE GridX = GridXTemp
	)
);

DROP TABLE WonderPolicyNeeded;

-- Outliers
UPDATE Buildings SET NumPoliciesNeeded = 3 WHERE Type = 'BUILDING_FORUM';
UPDATE Buildings SET NumPoliciesNeeded = 12 WHERE Type = 'BUILDING_PORCELAIN_TOWER';
UPDATE Buildings SET NumPoliciesNeeded = 15 WHERE Type = 'BUILDING_EIFFEL_TOWER';
UPDATE Buildings SET NumPoliciesNeeded = 19 WHERE Type = 'BUILDING_EMPIRE_STATE_BUILDING';
UPDATE Buildings SET NumPoliciesNeeded = 26 WHERE Type = 'BUILDING_SYDNEY_OPERA_HOUSE';
UPDATE Buildings SET NumPoliciesNeeded = 24 WHERE Type = 'BUILDING_GREAT_FIREWALL';

-----------------------------------------------------------------
-- Min Area Size
-----------------------------------------------------------------

-- All should be 0
UPDATE Buildings SET MinAreaSize = 0;

-- Except coastal buildings
UPDATE Buildings SET MinAreaSize = (
	SELECT Value FROM Defines
	WHERE Name = 'MIN_WATER_SIZE_FOR_OCEAN'
) WHERE Water = 1;

-----------------------------------------------------------------
-- Nuke Immunity (only used by "destroy random buildings" event in VP, as nukes don't destroy buildings)
-----------------------------------------------------------------

-- Only wonders, dummy buildings, and Strategic Defense System are nuke immune
UPDATE Buildings SET NukeImmune = 0;

UPDATE Buildings
SET NukeImmune = 1
WHERE BuildingClass IN (
	SELECT Type FROM BuildingClasses
	WHERE MaxPlayerInstances = 1
)
OR BuildingClass = 'BUILDINGCLASS_BOMB_SHELTER'
OR WonderSplashImage IS NOT NULL
OR IsDummy = 1;

-----------------------------------------------------------------
-- Hurry Cost Modifier: how much it costs to invest
-----------------------------------------------------------------

UPDATE Buildings
SET HurryCostModifier = -20;

UPDATE Buildings
SET HurryCostModifier = -5
WHERE WonderSplashImage IS NOT NULL;

UPDATE Buildings
SET HurryCostModifier = -1
WHERE IsDummy = 1 OR Cost = -1;

-----------------------------------------------------------------
-- Great Work Theming Bonuses
-----------------------------------------------------------------

CREATE TEMP TABLE Building_ThemingBonuses_Temp (
	BuildingTypeTemp TEXT,
	DescriptionTemp TEXT,
	BonusTemp INTEGER,
	AIPriorityTemp INTEGER
);

INSERT INTO Building_ThemingBonuses_Temp
VALUES
	-- Literature
	('BUILDING_AMPHITHEATER', 'TXT_KEY_THEMING_BONUS_AMPHITHEATER_NATIONAL', 4, 1),
	('BUILDING_AMPHITHEATER', 'TXT_KEY_THEMING_BONUS_AMPHITHEATER_GLOBAL', 4, 1),
	('BUILDING_ODEON', 'TXT_KEY_THEMING_BONUS_AMPHITHEATER_NATIONAL', 4, 1),
	('BUILDING_ODEON', 'TXT_KEY_THEMING_BONUS_AMPHITHEATER_GLOBAL', 6, 2),
	('BUILDING_ROYAL_LIBRARY', 'TXT_KEY_THEMING_BONUS_ROYAL_LIBRARY', 6, 2), -- Has 3 slots with a lax requirement, should be lower priority
	('BUILDING_RELIGIOUS_LIBRARY', 'TXT_KEY_THEMING_BONUS_RELIGIOUS_LIBRARY', 8, 3), -- Has 3 slots with no specified era requirement, can wait
	('BUILDING_GREAT_LIBRARY', 'TXT_KEY_THEMING_BONUS_GREAT_LIBRARY', 10, 4),
	('BUILDING_OXFORD_UNIVERSITY', 'TXT_KEY_THEMING_BONUS_OXFORD_UNIVERSITY', 10, 5), -- Strictly better yields than Great Library
	('BUILDING_GLOBE_THEATER', 'TXT_KEY_THEMING_BONUS_GLOBE_THEATER', 10, 6), -- Harsh requirements

	-- Art/Artifact
	-- Both art or both artifact, both from city owner or both not from city owner, from the same era
	('BUILDING_MUSEUM', 'TXT_KEY_THEMING_BONUS_MUSEUM_1', 6, 2),
	('BUILDING_MUSEUM', 'TXT_KEY_THEMING_BONUS_MUSEUM_2', 6, 2),
	-- Missing 1 of the 3 criteria above
	('BUILDING_MUSEUM', 'TXT_KEY_THEMING_BONUS_MUSEUM_3', 5, 1),
	('BUILDING_MUSEUM', 'TXT_KEY_THEMING_BONUS_MUSEUM_4', 5, 1),
	('BUILDING_MUSEUM', 'TXT_KEY_THEMING_BONUS_MUSEUM_5', 5, 1),
	('BUILDING_MUSEUM', 'TXT_KEY_THEMING_BONUS_MUSEUM_6', 5, 1),
	('BUILDING_MUSEUM', 'TXT_KEY_THEMING_BONUS_MUSEUM_7', 5, 1),
	('BUILDING_MUSEUM', 'TXT_KEY_THEMING_BONUS_MUSEUM_8', 5, 1),
	('BUILDING_PARTHENON', 'TXT_KEY_THEMING_BONUS_BUILDING_PARTHENON', 6, 2),
	('BUILDING_MURANO_GLASSWORKS', 'TXT_KEY_THEMING_BONUS_MURANO_GLASSWORKS', 8, 3), -- Lax requirement
	('BUILDING_NOTRE_DAME', 'TXT_KEY_THEMING_BONUS_BUILDING_NOTRE_DAME', 8, 4),
	('BUILDING_AMERICA_INDEPENDENCEHALL', 'TXT_KEY_THEMING_BONUS_INDEPENDENCEHALL', 8, 4),
	('BUILDING_SISTINE_CHAPEL', 'TXT_KEY_THEMING_BONUS_SISTINE_CHAPEL', 10, 5),
	('BUILDING_HERMITAGE', 'TXT_KEY_THEMING_BONUS_HERMITAGE', 15, 5),
	('BUILDING_UFFIZI', 'TXT_KEY_THEMING_BONUS_UFFIZI', 15, 6),
	('BUILDING_LOUVRE', 'TXT_KEY_THEMING_BONUS_LOUVRE', 15, 7), -- Not ideal, but Louvre often can't be themed if we theme Uffizi first

	-- Music
	('BUILDING_BROADCAST_TOWER', 'TXT_KEY_THEMING_BONUS_BROADCAST_TOWER_ALL', 14, 1),
	('BUILDING_BROADCAST_TOWER', 'TXT_KEY_THEMING_BONUS_BROADCAST_TOWER_CLASSICS', 14, 1),
	('BUILDING_BROADCAST_TOWER', 'TXT_KEY_THEMING_BONUS_BROADCAST_TOWER_NATIONAL', 14, 1),
	('BUILDING_BROADWAY', 'TXT_KEY_THEMING_BONUS_BROADWAY', 25, 2), -- Has 3 slots, lower priority
	('BUILDING_SYDNEY_OPERA_HOUSE', 'TXT_KEY_THEMING_BONUS_SYDNEY_OPERA_HOUSE', 25, 3);

UPDATE Building_ThemingBonuses
SET
	Bonus = (SELECT BonusTemp FROM Building_ThemingBonuses_Temp WHERE BuildingTypeTemp = BuildingType AND DescriptionTemp = Description),
	AIPriority = (SELECT AIPriorityTemp FROM Building_ThemingBonuses_Temp WHERE BuildingTypeTemp = BuildingType AND DescriptionTemp = Description)
WHERE EXISTS (SELECT 1 FROM Building_ThemingBonuses_Temp WHERE BuildingTypeTemp = BuildingType AND DescriptionTemp = Description);

DROP TABLE Building_ThemingBonuses_Temp;

-- Extra yields from theming in addition to culture/tourism
INSERT INTO Building_ThemingYieldBonus
	(BuildingType, YieldType, Yield)
VALUES
	-- Literature
	('BUILDING_GREAT_LIBRARY', 'YIELD_SCIENCE', 3),
	('BUILDING_ROYAL_LIBRARY', 'YIELD_PRODUCTION', 3),
	('BUILDING_ROYAL_LIBRARY', 'YIELD_SCIENCE', 3),
	('BUILDING_AMPHITHEATER', 'YIELD_CULTURE', 2),
	('BUILDING_ODEON', 'YIELD_CULTURE', 2),
	('BUILDING_OXFORD_UNIVERSITY', 'YIELD_SCIENCE', 4),
	('BUILDING_GLOBE_THEATER', 'YIELD_GOLD', 10),
	('BUILDING_RELIGIOUS_LIBRARY', 'YIELD_SCIENCE', 2),
	('BUILDING_RELIGIOUS_LIBRARY', 'YIELD_FAITH', 6),
	-- Art/Artifact
	('BUILDING_PARTHENON', 'YIELD_CULTURE', 3),
	('BUILDING_MURANO_GLASSWORKS', 'YIELD_SCIENCE', 5),
	('BUILDING_NOTRE_DAME', 'YIELD_FAITH', 3),
	('BUILDING_NOTRE_DAME', 'YIELD_GOLDEN_AGE_POINTS', 3),
	('BUILDING_SISTINE_CHAPEL', 'YIELD_CULTURE', 3),
	('BUILDING_SISTINE_CHAPEL', 'YIELD_FAITH', 3),
	('BUILDING_HERMITAGE', 'YIELD_GOLD', 4),
	('BUILDING_HERMITAGE', 'YIELD_CULTURE', 4),
	('BUILDING_AMERICA_INDEPENDENCEHALL', 'YIELD_GOLD', 3),
	('BUILDING_AMERICA_INDEPENDENCEHALL', 'YIELD_CULTURE', 3),
	('BUILDING_UFFIZI', 'YIELD_CULTURE', 10),
	('BUILDING_MUSEUM', 'YIELD_CULTURE', 4),
	('BUILDING_LOUVRE', 'YIELD_CULTURE', 15),
	-- Music
	('BUILDING_BROADCAST_TOWER', 'YIELD_GOLD', 10),
	('BUILDING_BROADWAY', 'YIELD_GOLD', 20),
	('BUILDING_SYDNEY_OPERA_HOUSE', 'YIELD_CULTURE', 15);

----------------------------------------------------------------------------
-- Mutual Exclusive Groups
----------------------------------------------------------------------------
UPDATE Buildings
SET MutuallyExclusiveGroup = 1
WHERE Type IN (
	'BUILDING_HYDRO_PLANT',
	'BUILDING_SOLAR_PLANT',
	'BUILDING_NUCLEAR_PLANT',
	'BUILDING_WIND_PLANT',
	'BUILDING_TIDAL_PLANT'
);

UPDATE Buildings
SET MutuallyExclusiveGroup = 3
WHERE Type IN (
	'BUILDING_WELL',
	'BUILDING_WATERMILL'
);

UPDATE Buildings
SET MutuallyExclusiveGroup = 7
WHERE Type IN (
	'BUILDING_MAUSOLEUM',
	'BUILDING_HEAVENLY_THRONE',
	'BUILDING_GREAT_ALTAR',
	'BUILDING_RELIGIOUS_LIBRARY',
	'BUILDING_DIVINE_COURT',
	'BUILDING_SACRED_GARDEN',
	'BUILDING_HOLY_COUNCIL',
	'BUILDING_GRAND_OSSUARY',
	'BUILDING_APOSTOLIC_PALACE'
);

UPDATE Buildings
SET MutuallyExclusiveGroup = 50
WHERE Type IN (
	'BUILDING_RIALTO_DISTRICT',
	'BUILDING_VENETIAN_ARSENALE',
	'BUILDING_MURANO_GLASSWORKS'
);
