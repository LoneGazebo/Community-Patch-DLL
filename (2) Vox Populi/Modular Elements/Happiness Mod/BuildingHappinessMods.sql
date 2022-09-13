-- Policies

--Progress
UPDATE Policies
SET PovertyHappinessMod = '-10', UnculturedHappinessMod = '-10', DefenseHappinessMod = '-10', IlliteracyHappinessMod = '-10'
WHERE Type = 'POLICY_REPRESENTATION';

-- Fealty
UPDATE Policies
SET UnculturedHappinessMod = '-15'
WHERE Type = 'POLICY_THEOCRACY';


-- Industry
UPDATE Policies
SET PovertyHappinessMod = '-15'
WHERE Type = 'POLICY_CARAVANS';

-- Imperialism

UPDATE Policies
SET NeedsModifierFromAirUnits = '3'
WHERE Type = 'POLICY_EXPLORATION_FINISHER';

-- Rationalism

UPDATE Policies
SET PovertyHappinessMod = '-5', DefenseHappinessMod = '-5', IlliteracyHappinessMod = '-5', UnculturedHappinessMod = '-5'
WHERE Type = 'POLICY_RATIONALISM';

UPDATE Policies
SET PovertyHappinessMod = '-2', DefenseHappinessMod = '-2', IlliteracyHappinessMod = '-2', UnculturedHappinessMod = '-2'
WHERE Type = 'POLICY_HUMANISM';

UPDATE Policies
SET PovertyHappinessMod = '-2', DefenseHappinessMod = '-2', IlliteracyHappinessMod = '-2', UnculturedHappinessMod = '-2'
WHERE Type = 'POLICY_SCIENTIFIC_REVOLUTION';

UPDATE Policies
SET PovertyHappinessMod = '-2', DefenseHappinessMod = '-2', IlliteracyHappinessMod = '-2', UnculturedHappinessMod = '-2'
WHERE Type = 'POLICY_FREE_THOUGHT';

UPDATE Policies
SET PovertyHappinessMod = '-2', DefenseHappinessMod = '-2', IlliteracyHappinessMod = '-2', UnculturedHappinessMod = '-2'
WHERE Type = 'POLICY_SOVEREIGNTY';

UPDATE Policies
SET PovertyHappinessMod = '-2', DefenseHappinessMod = '-2', IlliteracyHappinessMod = '-2', UnculturedHappinessMod = '-2'
WHERE Type = 'POLICY_SECULARISM';

-- Autocracy
UPDATE Policies
SET DefenseHappinessMod = '-20'
WHERE Type = 'POLICY_FORTIFIED_BORDERS';

-- Order
UPDATE Policies
SET IlliteracyHappinessMod = '-20'
WHERE Type = 'POLICY_ACADEMY_SCIENCES';

UPDATE Policies
SET PovertyHappinessMod = '-20'
WHERE Type = 'POLICY_UNIVERSAL_HEALTHCARE_O';

-- Freedom
UPDATE Policies
SET UnculturedHappinessMod = '-20'
WHERE Type = 'POLICY_OPEN_SOCIETY';

-- Empire
UPDATE Buildings
SET EmpireNeedsModifier = '-5'
WHERE Type = 'BUILDING_WALLS';

UPDATE Buildings
SET EmpireNeedsModifier = '-5'
WHERE Type = 'BUILDING_WALLS_OF_BABYLON';

UPDATE Buildings
SET EmpireNeedsModifier = '-5'
WHERE Type = 'BUILDING_CASTLE';

--UPDATE Buildings
--SET EmpireNeedsModifier = '-5'
--WHERE Type = 'BUILDING_MISSION';

UPDATE Buildings
SET EmpireNeedsModifier = '-5'
WHERE Type = 'BUILDING_ARSENAL';

UPDATE Buildings
SET EmpireNeedsModifier = '-5'
WHERE Type = 'BUILDING_KREPOST';

UPDATE Buildings
SET EmpireNeedsModifier = '-10'
WHERE Type = 'BUILDING_MILITARY_BASE';

UPDATE Buildings
SET EmpireNeedsModifierGlobal = '-10'
WHERE Type = 'BUILDING_CHICHEN_ITZA';

-- Poverty
INSERT INTO Building_UnhappinessNeedsFlatReduction
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_AQUEDUCT', 'YIELD_GOLD', 1),
	('BUILDING_INDUS_CANAL', 'YIELD_GOLD', 1),
	('BUILDING_CATHEDRAL', 'YIELD_GOLD', 1),
	('BUILDING_MANDIR', 'YIELD_GOLD', 1),
	('BUILDING_GROCER', 'YIELD_GOLD', 1),
	('BUILDING_COFFEE_HOUSE', 'YIELD_GOLD', 1),
	('BUILDING_GREAT_COTHON', 'YIELD_GOLD', 1),
	('BUILDING_HOSPITAL', 'YIELD_GOLD', 1),
	('BUILDING_MINT', 'YIELD_GOLD', 1),
	('BUILDING_HANSE', 'YIELD_GOLD', 1),
	('BUILDING_MEDICAL_LAB', 'YIELD_GOLD', 1),
	('BUILDING_FORBIDDEN_PALACE', 'YIELD_GOLD', 1),
	('BUILDING_BIG_BEN', 'YIELD_GOLD', 1),
	('BUILDING_MEDICAL_LAB', 'YIELD_GOLD', 1),
	('BUILDING_POLICE_STATION', 'YIELD_GOLD', 1);

-- Poverty Global
UPDATE Buildings
SET PovertyHappinessChangeGlobal = '-10'
WHERE Type = 'BUILDING_NATIONAL_TREASURY';

UPDATE Buildings
SET PovertyHappinessChangeGlobal = '-10'
WHERE Type = 'BUILDING_GREAT_COTHON';

-- Illiteracy
INSERT INTO Building_UnhappinessNeedsFlatReduction
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_ORACLE', 'YIELD_SCIENCE', 1),
	('BUILDING_LIBRARY', 'YIELD_SCIENCE', 1),
	('BUILDING_PAPER_MAKER', 'YIELD_SCIENCE', 1),
	('BUILDING_MOSQUE', 'YIELD_SCIENCE', 1),
	('BUILDING_STUPA', 'YIELD_SCIENCE', 1),
	('BUILDING_UNIVERSITY', 'YIELD_SCIENCE', 1),
	('BUILDING_SEOWON', 'YIELD_SCIENCE', 1),
	('BUILDING_OXFORD_UNIVERSITY', 'YIELD_SCIENCE', 1),
	('BUILDING_PORCELAIN_TOWER', 'YIELD_SCIENCE', 1),
	('BUILDING_PUBLIC_SCHOOL', 'YIELD_SCIENCE', 1),
	('BUILDING_SKOLA', 'YIELD_SCIENCE', 1),
	('BUILDING_LABORATORY', 'YIELD_SCIENCE', 1),
	('BUILDING_GIORGIO_ARMEIER', 'YIELD_SCIENCE', 1),
	('BUILDING_POLICE_STATION', 'YIELD_SCIENCE', 1);

-- Illiteracy Global
UPDATE Buildings
SET IlliteracyHappinessChangeGlobal = '-10'
WHERE Type = 'BUILDING_NATIONAL_COLLEGE';

UPDATE Buildings
SET IlliteracyHappinessChangeGlobal = '-10'
WHERE Type = 'BUILDING_ROYAL_LIBRARY';

-- Distress
INSERT INTO Building_UnhappinessNeedsFlatReduction
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BARRACKS', 'YIELD_PRODUCTION', 1),
	('BUILDING_IKANDA', 'YIELD_PRODUCTION', 1),
	('BUILDING_SYNAGOGUE', 'YIELD_PRODUCTION', 1),
	('BUILDING_ORDER', 'YIELD_PRODUCTION', 1),
	('BUILDING_ARMORY', 'YIELD_PRODUCTION', 1),
	('BUILDING_DOJO', 'YIELD_PRODUCTION', 1),
	('BUILDING_CONSTABLE', 'YIELD_PRODUCTION', 1),
	('BUILDING_WAT', 'YIELD_PRODUCTION', 1),
	('BUILDING_MILITARY_ACADEMY', 'YIELD_PRODUCTION', 1),
	('BUILDING_MILITARY_BASE', 'YIELD_PRODUCTION', 1),
	('BUILDING_KREMLIN', 'YIELD_PRODUCTION', 1),
	('BUILDING_STATUE_ZEUS', 'YIELD_PRODUCTION', 1),
	('BUILDING_POLICE_STATION', 'YIELD_PRODUCTION', 1);

-- Defense Global

UPDATE Buildings
SET DefenseHappinessChangeGlobal = '-10'
WHERE Type = 'BUILDING_INTELLIGENCE_AGENCY';

-- Boredom
INSERT INTO Building_UnhappinessNeedsFlatReduction
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_COLOSSEUM', 'YIELD_CULTURE', 1),
	('BUILDING_FLAVIAN_COLOSSEUM', 'YIELD_CULTURE', 1),
	('BUILDING_CHURCH', 'YIELD_CULTURE', 1),
	('BUILDING_PAGODA', 'YIELD_CULTURE', 1),
	('BUILDING_CIRCUS_MAXIMUS', 'YIELD_CULTURE', 1),
	('BUILDING_CEILIDH_HALL', 'YIELD_CULTURE', 1),
	('BUILDING_CIRCUS', 'YIELD_CULTURE', 1),
	('BUILDING_THEATRE', 'YIELD_CULTURE', 1),
	('BUILDING_MUSEUM', 'YIELD_CULTURE', 1),
	('BUILDING_BROADCAST_TOWER', 'YIELD_CULTURE', 1),
	('BUILDING_PARTHENON', 'YIELD_CULTURE', 1),
	('BUILDING_GLOBE_THEATER', 'YIELD_CULTURE', 1),
	('BUILDING_GIORGIO_ARMEIER', 'YIELD_CULTURE', 1),
	('BUILDING_POLICE_STATION', 'YIELD_CULTURE', 1);

-- Culture Global

UPDATE Buildings
SET UnculturedHappinessChangeGlobal = '-10'
WHERE Type = 'BUILDING_HERMITAGE';

UPDATE Buildings
SET UnculturedHappinessChangeGlobal = '-10'
WHERE Type = 'BUILDING_MOMA';

-- Religion
INSERT INTO Building_UnhappinessNeedsFlatReduction
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TEMPLE', 'YIELD_FAITH', 1),
	('BUILDING_BASILICA', 'YIELD_FAITH', 1),
	('BUILDING_GRAND_TEMPLE', 'YIELD_FAITH', 1),
	('BUILDING_PAGODA', 'YIELD_FAITH', 2),
	('BUILDING_GRAND_OSSUARY', 'YIELD_FAITH', 1),
	('BUILDING_APOSTOLIC_PALACE', 'YIELD_FAITH', 1),
	('BUILDING_HOLY_COUNCIL', 'YIELD_FAITH', 1),
	('BUILDING_SACRED_GARDEN', 'YIELD_FAITH', 1),
	('BUILDING_DIVINE_COURT', 'YIELD_FAITH', 1),
	('BUILDING_RELIQUARY', 'YIELD_FAITH', 1),
	('BUILDING_GREAT_ALTAR', 'YIELD_FAITH', 1),
	('BUILDING_HEAVENLY_THRONE', 'YIELD_FAITH', 1),
	('BUILDING_MAUSOLEUM', 'YIELD_FAITH', 1);

-- Religion Global 

UPDATE Buildings
SET MinorityHappinessChangeGlobal = '-10'
WHERE Type = 'BUILDING_GRAND_TEMPLE';