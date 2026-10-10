-- Opener (now Artistry)
INSERT INTO Policy_BuildingClassProductionModifiers
	(PolicyType, BuildingClassType, ProductionModifier)
VALUES
	('POLICY_AESTHETICS', 'BUILDINGCLASS_WRITERS_GUILD', 100),
	('POLICY_AESTHETICS', 'BUILDINGCLASS_ARTISTS_GUILD', 100),
	('POLICY_AESTHETICS', 'BUILDINGCLASS_MUSICIANS_GUILD', 100);

INSERT INTO Policy_GoldenAgeYieldMod
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_AESTHETICS', 'YIELD_CULTURE', 10);

-- Cultural Centers (now Humanism)
UPDATE Policies
SET GoldenAgeMeterMod = -25
WHERE Type = 'POLICY_CULTURAL_CENTERS';

DELETE FROM Policy_BuildingClassProductionModifiers
WHERE PolicyType = 'POLICY_CULTURAL_CENTERS';

INSERT INTO Policy_GreatWorkYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_CULTURAL_CENTERS', 'YIELD_CULTURE', 2);

INSERT INTO Policy_BuildingClassHappiness
	(PolicyType, BuildingClassType, Happiness)
VALUES
	('POLICY_CULTURAL_CENTERS', 'BUILDINGCLASS_WRITERS_GUILD', 2),
	('POLICY_CULTURAL_CENTERS', 'BUILDINGCLASS_ARTISTS_GUILD', 2),
	('POLICY_CULTURAL_CENTERS', 'BUILDINGCLASS_MUSICIANS_GUILD', 2);

-- Fine Arts (now Refinement)
UPDATE Policies
SET
	HappinessToCulture = 0,
	NoUnhappfromXSpecialists = 1
WHERE Type = 'POLICY_FINE_ARTS';

INSERT INTO Policy_GreatWorkYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_FINE_ARTS', 'YIELD_SCIENCE', 2);

INSERT INTO Policy_SpecialistExtraYields
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_FINE_ARTS', 'YIELD_CULTURE', 1);

-- Flourishing of the Arts (now Heritage)
UPDATE Policies
SET
	CultureWonderMultiplier = 0,
	GoldenAgeTurns = 0
WHERE Type = 'POLICY_FLOURISHING_OF_ARTS';

INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
VALUES
	('POLICY_FLOURISHING_OF_ARTS', 'BUILDINGCLASS_AMPHITHEATER', 'YIELD_PRODUCTION', 3),
	('POLICY_FLOURISHING_OF_ARTS', 'BUILDINGCLASS_AMPHITHEATER', 'YIELD_CULTURE', 3),
	('POLICY_FLOURISHING_OF_ARTS', 'BUILDINGCLASS_OPERA_HOUSE', 'YIELD_PRODUCTION', 3),
	('POLICY_FLOURISHING_OF_ARTS', 'BUILDINGCLASS_OPERA_HOUSE', 'YIELD_CULTURE', 3),
	('POLICY_FLOURISHING_OF_ARTS', 'BUILDINGCLASS_GALLERY', 'YIELD_PRODUCTION', 3),
	('POLICY_FLOURISHING_OF_ARTS', 'BUILDINGCLASS_GALLERY', 'YIELD_CULTURE', 3);

INSERT INTO Policy_GreatWorkYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_FLOURISHING_OF_ARTS', 'YIELD_GOLD', 4);

-- Artistic Genius (now National Treasure)
DELETE FROM Policy_FreeUnitClasses
WHERE PolicyType = 'POLICY_ARTISTIC_GENIUS';

UPDATE Policies
SET
	NumFreeGreatPeople = 1,
	IncludesOneShotFreeUnits = 1
WHERE Type = 'POLICY_ARTISTIC_GENIUS';

INSERT INTO Policy_YieldFromWorldWonderConstruction
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_ARTISTIC_GENIUS', 'YIELD_GOLD', 250);

INSERT INTO Policy_GreatPersonExpendedYield
	(PolicyType, GreatPersonType, YieldType, Yield)
VALUES
	('POLICY_ARTISTIC_GENIUS', 'GREATPERSON_WRITER', 'YIELD_GOLD', 150),
	('POLICY_ARTISTIC_GENIUS', 'GREATPERSON_ARTIST', 'YIELD_GOLD', 150),
	('POLICY_ARTISTIC_GENIUS', 'GREATPERSON_MUSICIAN', 'YIELD_GOLD', 150);

-- Cultural Exchange
UPDATE Policies
SET
	SharedReligionTourismModifier = 0,
	TradeRouteTourismModifier = 0,
	OpenBordersTourismModifier = 10,
	IlliteracyFlatReduction = 2,
	ThemingBonusMultiplier = 50
WHERE Type = 'POLICY_ETHICS';

-- Finisher
UPDATE Policies
SET
	ThemingBonusMultiplier = 0,
	ArchaeologicalDigTourism = 3,
	GoldenAgeTourism = 4
WHERE Type = 'POLICY_AESTHETICS_FINISHER';

DELETE FROM Policy_FaithPurchaseUnitClasses
WHERE PolicyType = 'POLICY_AESTHETICS_FINISHER';

INSERT INTO Policy_FaithPurchaseUnitClasses
	(PolicyType, UnitClassType)
VALUES
	('POLICY_AESTHETICS_FINISHER', 'UNITCLASS_MUSICIAN');

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_AESTHETICS_FINISHER', 'IMPROVEMENT_LANDMARK', 'YIELD_SCIENCE', 2),
	('POLICY_AESTHETICS_FINISHER', 'IMPROVEMENT_LANDMARK', 'YIELD_CULTURE', 2),
	('POLICY_AESTHETICS_FINISHER', 'IMPROVEMENT_LANDMARK', 'YIELD_GOLD', 4);

INSERT INTO Policy_ArtifactYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_AESTHETICS_FINISHER', 'YIELD_SCIENCE', 3);

-- Scaler
INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
SELECT
	p.Type, bc.Type, 'YIELD_SCIENCE', 1
FROM Policies p, BuildingClasses bc
WHERE p.PolicyBranchType = 'POLICY_BRANCH_AESTHETICS'
AND bc.Type IN ('BUILDINGCLASS_WRITERS_GUILD', 'BUILDINGCLASS_ARTISTS_GUILD', 'BUILDINGCLASS_MUSICIANS_GUILD');

UPDATE Policies
SET GAPFromHappinessModifier = 20
WHERE PolicyBranchType = 'POLICY_BRANCH_AESTHETICS';
