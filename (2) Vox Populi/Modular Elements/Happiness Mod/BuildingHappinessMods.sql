-- Policies

--Progress
UPDATE Policies
SET DistressFlatReductionGlobal = '1', PovertyFlatReductionGlobal = '1', IlliteracyFlatReductionGlobal = '1', BoredomFlatReductionGlobal = '1'
WHERE Type = 'POLICY_REPRESENTATION';

-- Fealty
UPDATE Policies
SET BoredomFlatReductionGlobal = '2'
WHERE Type = 'POLICY_THEOCRACY';

-- Industry
UPDATE Policies
SET PovertyFlatReductionGlobal = '2'
WHERE Type = 'POLICY_CARAVANS';

/*-- Imperialism
UPDATE Policies
SET NeedsModifierFromAirUnits = '3'
WHERE Type = 'POLICY_EXPLORATION_FINISHER';*/

-- Rationalism
UPDATE Policies
SET DistressFlatReductionGlobal = '1', PovertyFlatReductionGlobal = '1', IlliteracyFlatReductionGlobal = '1', BoredomFlatReductionGlobal = '1', ReligiousUnrestFlatReductionGlobal = '1'
WHERE Type = 'POLICY_RATIONALISM';

UPDATE Policies
SET ReligiousUnrestFlatReductionGlobal = '5'
WHERE Type = 'POLICY_FREE_THOUGHT';

UPDATE Policies
SET DistressFlatReductionGlobal = '1', PovertyFlatReductionGlobal = '1', IlliteracyFlatReductionGlobal = '1', BoredomFlatReductionGlobal = '1', ReligiousUnrestFlatReductionGlobal = '1'
WHERE Type = 'POLICY_RATIONALISM_FINISHER';

-- Autocracy
UPDATE Policies
SET DistressFlatReductionGlobal = '2'
WHERE Type = 'POLICY_FORTIFIED_BORDERS';

-- Order
UPDATE Policies
SET IlliteracyFlatReductionGlobal = '2'
WHERE Type = 'POLICY_ACADEMY_SCIENCES';

UPDATE Policies
SET PovertyFlatReductionGlobal = '2'
WHERE Type = 'POLICY_UNIVERSAL_HEALTHCARE_O';

-- Freedom
UPDATE Policies
SET BoredomFlatReductionGlobal = '2'
WHERE Type = 'POLICY_OPEN_SOCIETY';


-- Empire
UPDATE Buildings
SET EmpireSizeModifierReduction = '-5'
WHERE Type = 'BUILDING_WALLS';

UPDATE Buildings
SET EmpireSizeModifierReduction = '-5'
WHERE Type = 'BUILDING_WALLS_OF_BABYLON';

UPDATE Buildings
SET EmpireSizeModifierReduction = '-5'
WHERE Type = 'BUILDING_CASTLE';

UPDATE Buildings
SET EmpireSizeModifierReduction = '-5'
WHERE Type = 'BUILDING_FORTRESS';

UPDATE Buildings
SET EmpireSizeModifierReduction = '-5'
WHERE Type = 'BUILDING_KREPOST';

UPDATE Buildings
SET EmpireSizeModifierReduction = '-5'
WHERE Type = 'BUILDING_ARSENAL';

UPDATE Buildings
SET EmpireSizeModifierReduction = '-5'
WHERE Type = 'BUILDING_MILITARY_BASE';

UPDATE Buildings
SET EmpireSizeModifierReductionGlobal = '-10'
WHERE Type = 'BUILDING_CHICHEN_ITZA';


-- Distress
UPDATE Buildings
SET DistressFlatReduction = '1'
WHERE Type = 'BUILDING_BARRACKS';

UPDATE Buildings
SET DistressFlatReduction = '1'
WHERE Type = 'BUILDING_IKANDA';

UPDATE Buildings
SET DistressFlatReduction = '1'
WHERE Type = 'BUILDING_SYNAGOGUE';

UPDATE Buildings
SET DistressFlatReduction = '1'
WHERE Type = 'BUILDING_ORDER';

UPDATE Buildings
SET DistressFlatReduction = '1'
WHERE Type = 'BUILDING_TEOCALLI';

UPDATE Buildings
SET DistressFlatReduction = '1'
WHERE Type = 'BUILDING_GURDWARA';

UPDATE Buildings
SET DistressFlatReduction = '1'
WHERE Type = 'BUILDING_ARMORY';

UPDATE Buildings
SET DistressFlatReduction = '1'
WHERE Type = 'BUILDING_DOJO';

UPDATE Buildings
SET DistressFlatReduction = '1'
WHERE Type = 'BUILDING_CONSTABLE';

UPDATE Buildings
SET DistressFlatReduction = '1'
WHERE Type = 'BUILDING_WAT';

UPDATE Buildings
SET DistressFlatReduction = '1'
WHERE Type = 'BUILDING_MILITARY_ACADEMY';

UPDATE Buildings
SET DistressFlatReduction = '1'
WHERE Type = 'BUILDING_MILITARY_BASE';

UPDATE Buildings
SET DistressFlatReduction = '1'
WHERE Type = 'BUILDING_KREMLIN';

UPDATE Buildings
SET DistressFlatReduction = '1'
WHERE Type = 'BUILDING_STATUE_ZEUS';

UPDATE Buildings
SET DistressFlatReduction = '1'
WHERE Type = 'BUILDING_POLICE_STATION';

UPDATE Buildings
SET DistressFlatReductionGlobal = '1'
WHERE Type = 'BUILDING_INTELLIGENCE_AGENCY';


-- Poverty
UPDATE Buildings
SET PovertyFlatReduction = '1'
WHERE Type = 'BUILDING_AQUEDUCT';

UPDATE Buildings
SET PovertyFlatReduction = '1'
WHERE Type = 'BUILDING_INDUS_CANAL';

UPDATE Buildings
SET PovertyFlatReduction = '1'
WHERE Type = 'BUILDING_CATHEDRAL';

UPDATE Buildings
SET PovertyFlatReduction = '1'
WHERE Type = 'BUILDING_MANDIR';

UPDATE Buildings
SET PovertyFlatReduction = '1'
WHERE Type = 'BUILDING_GROCER';

UPDATE Buildings
SET PovertyFlatReduction = '1'
WHERE Type = 'BUILDING_COFFEE_HOUSE';

UPDATE Buildings
SET PovertyFlatReduction = '1'
WHERE Type = 'BUILDING_GREAT_COTHON';

UPDATE Buildings
SET PovertyFlatReduction = '1'
WHERE Type = 'BUILDING_HOSPITAL';

UPDATE Buildings
SET PovertyFlatReduction = '1'
WHERE Type = 'BUILDING_MINT';

UPDATE Buildings
SET PovertyFlatReduction = '1'
WHERE Type = 'BUILDING_HANSE';

UPDATE Buildings
SET PovertyFlatReduction = '1'
WHERE Type = 'BUILDING_MEDICAL_LAB';

UPDATE Buildings
SET PovertyFlatReduction = '1'
WHERE Type = 'BUILDING_FORBIDDEN_PALACE';

UPDATE Buildings
SET PovertyFlatReduction = '1'
WHERE Type = 'BUILDING_BIG_BEN';

UPDATE Buildings
SET PovertyFlatReduction = '1'
WHERE Type = 'BUILDING_POLICE_STATION';

UPDATE Buildings
SET PovertyFlatReductionGlobal = '1'
WHERE Type = 'BUILDING_GREAT_COTHON';

UPDATE Buildings
SET PovertyFlatReductionGlobal = '1'
WHERE Type = 'BUILDING_NATIONAL_TREASURY';


-- Illiteracy
UPDATE Buildings
SET IlliteracyFlatReduction = '1'
WHERE Type = 'BUILDING_ORACLE';

UPDATE Buildings
SET IlliteracyFlatReduction = '1'
WHERE Type = 'BUILDING_LIBRARY';

UPDATE Buildings
SET IlliteracyFlatReduction = '1'
WHERE Type = 'BUILDING_MOSQUE';

UPDATE Buildings
SET IlliteracyFlatReduction = '1'
WHERE Type = 'BUILDING_STUPA';

UPDATE Buildings
SET IlliteracyFlatReduction = '1'
WHERE Type = 'BUILDING_UNIVERSITY';

UPDATE Buildings
SET IlliteracyFlatReduction = '1'
WHERE Type = 'BUILDING_SEOWON';

UPDATE Buildings
SET IlliteracyFlatReduction = '1'
WHERE Type = 'BUILDING_OXFORD_UNIVERSITY';

UPDATE Buildings
SET IlliteracyFlatReduction = '1'
WHERE Type = 'BUILDING_PORCELAIN_TOWER';

UPDATE Buildings
SET IlliteracyFlatReduction = '1'
WHERE Type = 'BUILDING_PUBLIC_SCHOOL';

UPDATE Buildings
SET IlliteracyFlatReduction = '1'
WHERE Type = 'BUILDING_SKOLA';

UPDATE Buildings
SET IlliteracyFlatReduction = '1'
WHERE Type = 'BUILDING_LABORATORY';

UPDATE Buildings
SET IlliteracyFlatReduction = '1'
WHERE Type = 'BUILDING_GIORGIO_ARMEIER';

UPDATE Buildings
SET IlliteracyFlatReduction = '1'
WHERE Type = 'BUILDING_POLICE_STATION';

UPDATE Buildings
SET IlliteracyFlatReductionGlobal = '1'
WHERE Type = 'BUILDING_NATIONAL_COLLEGE';

UPDATE Buildings
SET IlliteracyFlatReductionGlobal = '1'
WHERE Type = 'BUILDING_ROYAL_LIBRARY';


-- Boredom
UPDATE Buildings
SET BoredomFlatReduction = '1'
WHERE Type = 'BUILDING_COLOSSEUM';

UPDATE Buildings
SET BoredomFlatReduction = '1'
WHERE Type = 'BUILDING_FLAVIAN_COLOSSEUM';

UPDATE Buildings
SET BoredomFlatReduction = '1'
WHERE Type = 'BUILDING_CHURCH';

UPDATE Buildings
SET BoredomFlatReduction = '1'
WHERE Type = 'BUILDING_PAGODA';

UPDATE Buildings
SET BoredomFlatReduction = '1'
WHERE Type = 'BUILDING_CIRCUS_MAXIMUS';

UPDATE Buildings
SET BoredomFlatReduction = '1'
WHERE Type = 'BUILDING_CEILIDH_HALL';

UPDATE Buildings
SET BoredomFlatReduction = '1'
WHERE Type = 'BUILDING_CIRCUS';

UPDATE Buildings
SET BoredomFlatReduction = '1'
WHERE Type = 'BUILDING_THEATRE';

UPDATE Buildings
SET BoredomFlatReduction = '1'
WHERE Type = 'BUILDING_MUSEUM';

UPDATE Buildings
SET BoredomFlatReduction = '1'
WHERE Type = 'BUILDING_BROADCAST_TOWER';

UPDATE Buildings
SET BoredomFlatReduction = '1'
WHERE Type = 'BUILDING_PARTHENON';

UPDATE Buildings
SET BoredomFlatReduction = '1'
WHERE Type = 'BUILDING_GLOBE_THEATER';

UPDATE Buildings
SET BoredomFlatReduction = '1'
WHERE Type = 'BUILDING_GIORGIO_ARMEIER';

UPDATE Buildings
SET BoredomFlatReduction = '1'
WHERE Type = 'BUILDING_POLICE_STATION';

UPDATE Buildings
SET BoredomFlatReductionGlobal = '1'
WHERE Type = 'BUILDING_HERMITAGE';

UPDATE Buildings
SET BoredomFlatReductionGlobal = '1'
WHERE Type = 'BUILDING_AMERICA_INDEPENDENCEHALL';


-- Religious Unrest
UPDATE Buildings
SET ReligiousUnrestFlatReduction = '1'
WHERE Type = 'BUILDING_TEMPLE';

UPDATE Buildings
SET ReligiousUnrestFlatReduction = '1'
WHERE Type = 'BUILDING_BASILICA';

UPDATE Buildings
SET ReligiousUnrestFlatReduction = '1'
WHERE Type = 'BUILDING_GRAND_TEMPLE';

UPDATE Buildings
SET ReligiousUnrestFlatReduction = '2'
WHERE Type = 'BUILDING_PAGODA';

UPDATE Buildings
SET ReligiousUnrestFlatReduction = '1'
WHERE Type = 'BUILDING_GRAND_OSSUARY';

UPDATE Buildings
SET ReligiousUnrestFlatReduction = '1'
WHERE Type = 'BUILDING_APOSTOLIC_PALACE';

UPDATE Buildings
SET ReligiousUnrestFlatReduction = '1'
WHERE Type = 'BUILDING_HOLY_COUNCIL';

UPDATE Buildings
SET ReligiousUnrestFlatReduction = '1'
WHERE Type = 'BUILDING_SACRED_GARDEN';

UPDATE Buildings
SET ReligiousUnrestFlatReduction = '1'
WHERE Type = 'BUILDING_DIVINE_COURT';

UPDATE Buildings
SET ReligiousUnrestFlatReduction = '1'
WHERE Type = 'BUILDING_RELIGIOUS_LIBRARY';

UPDATE Buildings
SET ReligiousUnrestFlatReduction = '1'
WHERE Type = 'BUILDING_GREAT_ALTAR';

UPDATE Buildings
SET ReligiousUnrestFlatReduction = '1'
WHERE Type = 'BUILDING_HEAVENLY_THRONE';

UPDATE Buildings
SET ReligiousUnrestFlatReduction = '1'
WHERE Type = 'BUILDING_MAUSOLEUM';

UPDATE Buildings
SET ReligiousUnrestFlatReductionGlobal = '1'
WHERE Type = 'BUILDING_GRAND_TEMPLE';
