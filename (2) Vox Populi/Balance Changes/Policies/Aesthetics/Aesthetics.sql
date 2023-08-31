-- Unlock Time
UPDATE PolicyBranchTypes
SET EraPrereq = 'ERA_MEDIEVAL'
WHERE Type = 'POLICY_BRANCH_AESTHETICS';


-- Opener (now Artistry)

UPDATE Policies
SET
	GreatWriterRateModifier = 25,
	GreatArtistRateModifier = 25,
	GreatMusicianRateModifier = 25,
	GreatPeopleRateModifier = 0
WHERE Type = 'POLICY_AESTHETICS';

INSERT INTO Policy_BuildingClassProductionModifiers
	(PolicyType, BuildingClassType, ProductionModifier)
VALUES
	('POLICY_AESTHETICS', 'BUILDINGCLASS_WRITERS_GUILD', 100),
	('POLICY_AESTHETICS', 'BUILDINGCLASS_MUSICIANS_GUILD', 100),
	('POLICY_AESTHETICS', 'BUILDINGCLASS_ARTISTS_GUILD', 100);

INSERT INTO Policy_GoldenAgeYieldMod
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_AESTHETICS', 'YIELD_CULTURE', 10);
-- Cultural Centers (now Humanism)

UPDATE Policies
SET
	GoldenAgeMeterMod = -25
WHERE Type = 'POLICY_CULTURAL_CENTERS';

DELETE FROM Policy_BuildingClassProductionModifiers
WHERE PolicyType = 'POLICY_CULTURAL_CENTERS';

INSERT INTO Policy_LitYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_CULTURAL_CENTERS', 'YIELD_FAITH', 3);

INSERT INTO Policy_BuildingClassHappiness
	(PolicyType, BuildingClassType, Happiness)
VALUES
	('POLICY_CULTURAL_CENTERS', 'BUILDINGCLASS_WRITERS_GUILD', 1),
	('POLICY_CULTURAL_CENTERS', 'BUILDINGCLASS_ARTISTS_GUILD', 1),
	('POLICY_CULTURAL_CENTERS', 'BUILDINGCLASS_MUSICIANS_GUILD', 1);

-- Fine Arts (now Refinement)

UPDATE Policies
SET
	HappinessToCulture = 0,
	NoUnhappfromXSpecialists = 1
WHERE Type = 'POLICY_FINE_ARTS';

INSERT INTO Policy_ArtYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_FINE_ARTS', 'YIELD_CULTURE', 2);

INSERT INTO Policy_SpecialistExtraYields
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_FINE_ARTS', 'YIELD_CULTURE', 1);
	
-- Flourishing of the Arts (now Heritage)

UPDATE Policies
SET
	CultureWonderMultiplier = 0,
	GoldenAgeTurns = 0,
	CulturePerWonder = 0,
	ThemingBonusMultiplier = 50,
	LandmarksTourismPercent = 25
WHERE Type = 'POLICY_FLOURISHING_OF_ARTS';

INSERT INTO Policy_MusicYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_FLOURISHING_OF_ARTS', 'YIELD_GOLD', 4);

-- Artistic Genius (National Treasure)

DELETE FROM Policy_FreeUnitClasses
WHERE PolicyType = 'POLICY_ARTISTIC_GENIUS';

UPDATE Policies
SET
	NumFreeGreatPeople = 1
WHERE Type = 'POLICY_ARTISTIC_GENIUS';

INSERT INTO Policy_ArtifactYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_ARTISTIC_GENIUS', 'YIELD_SCIENCE', 2);

INSERT INTO Policy_YieldFromWorldWonderConstruction
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_ARTISTIC_GENIUS', 'YIELD_GOLD', 250);


-- Cultural Exchange

UPDATE Policies
SET
	SharedReligionTourismModifier = 0,
	TradeRouteTourismModifier = 0,
	OpenBordersTourismModifier = 10,
	HappinessPerXGreatWorks = 3
WHERE Type = 'POLICY_ETHICS';


-- Finisher

-- Great Musicians come from Aesthetics (and only Great Musicians)
DELETE FROM Policy_FaithPurchaseUnitClasses 
WHERE PolicyType = 'POLICY_AESTHETICS_FINISHER';

INSERT INTO Policy_FaithPurchaseUnitClasses
	(PolicyType, UnitClassType)
VALUES
	('POLICY_AESTHETICS_FINISHER', 'UNITCLASS_MUSICIAN');

-- Reveals Hidden Sites.
INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_AESTHETICS_FINISHER', 'IMPROVEMENT_LANDMARK', 'YIELD_SCIENCE', 3);

UPDATE Resources
SET
	PolicyReveal = 'POLICY_AESTHETICS_FINISHER'
WHERE Type = 'RESOURCE_HIDDEN_ARTIFACTS';

UPDATE Policies
SET 
	ThemingBonusMultiplier = 0,
	ArchaeologicalDigTourism = 3,
	GoldenAgeTourism = 4
WHERE Type = 'POLICY_AESTHETICS_FINISHER';


----------------------
-- Combined Insertions
----------------------

-- Scaler

INSERT INTO Policy_CityYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_ETHICS', 'YIELD_SCIENCE', 1),
	('POLICY_ARTISTIC_GENIUS', 'YIELD_SCIENCE', 1),
	('POLICY_FLOURISHING_OF_ARTS', 'YIELD_SCIENCE', 1),
	('POLICY_FINE_ARTS', 'YIELD_SCIENCE', 1),
	('POLICY_CULTURAL_CENTERS', 'YIELD_SCIENCE', 1);

UPDATE Policies SET GAPFromHappinessModifier = 20 WHERE Type = 'POLICY_ETHICS';
UPDATE Policies SET GAPFromHappinessModifier = 20 WHERE Type = 'POLICY_ARTISTIC_GENIUS';
UPDATE Policies SET GAPFromHappinessModifier = 20 WHERE Type = 'POLICY_FLOURISHING_OF_ARTS';
UPDATE Policies SET GAPFromHappinessModifier = 20 WHERE Type = 'POLICY_FINE_ARTS';
UPDATE Policies SET GAPFromHappinessModifier = 20 WHERE Type = 'POLICY_CULTURAL_CENTERS';

-- Building Changes

INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
VALUES
	('POLICY_ETHICS', 'BUILDINGCLASS_AMPHITHEATER', 'YIELD_CULTURE', 2),
	('POLICY_ETHICS', 'BUILDINGCLASS_AMPHITHEATER', 'YIELD_PRODUCTION', 2),
	('POLICY_ETHICS', 'BUILDINGCLASS_OPERA_HOUSE', 'YIELD_CULTURE', 2),
	('POLICY_ETHICS', 'BUILDINGCLASS_OPERA_HOUSE', 'YIELD_PRODUCTION', 2),
	('POLICY_ETHICS', 'BUILDINGCLASS_GALLERY', 'YIELD_CULTURE', 2),
	('POLICY_ETHICS', 'BUILDINGCLASS_GALLERY', 'YIELD_PRODUCTION', 2);