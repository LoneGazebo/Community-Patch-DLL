-- NATIONAL WONDERS

-- Guilds - now buildable in three Cities, give a bit more stuff
UPDATE BuildingClasses
SET MaxPlayerInstances = '3'
WHERE Type IN ('BUILDINGCLASS_WRITERS_GUILD', 'BUILDINGCLASS_ARTISTS_GUILD', 'BUILDINGCLASS_MUSICIANS_GUILD');

-- National Epic -- Change Name, give art
UPDATE Buildings
SET GreatWorkSlotType = 'GREAT_WORK_SLOT_ART_ARTIFACT'
WHERE Type = 'BUILDING_NATIONAL_EPIC';

-- Heroic Epic

UPDATE Buildings
SET CitySupplyModifier = '10'
WHERE Type = 'BUILDING_HEROIC_EPIC';

UPDATE Buildings
SET PrereqTech = 'TECH_IRON_WORKING'
WHERE Type = 'BUILDING_HEROIC_EPIC';

INSERT INTO Building_FreeUnits (BuildingType, UnitType, NumUnits)
VALUES ('BUILDING_HEROIC_EPIC' , 'UNIT_WRITER' , 1);



-- National College -- Now called School of Philosophy

UPDATE Buildings
SET GreatWorkSlotType = 'GREAT_WORK_SLOT_LITERATURE'
WHERE Type = 'BUILDING_NATIONAL_COLLEGE';

UPDATE Buildings
SET SpecialistType = 'SPECIALIST_SCIENTIST'
WHERE Type = 'BUILDING_NATIONAL_COLLEGE';

UPDATE Buildings
SET GreatPeopleRateChange = '2'
WHERE Type = 'BUILDING_NATIONAL_COLLEGE';

UPDATE Buildings
SET GreatWorkCount = '1'
WHERE Type = 'BUILDING_NATIONAL_COLLEGE';

UPDATE Building_YieldChanges
Set Yield = '5'
WHERE BuildingType = 'BUILDING_NATIONAL_COLLEGE' AND YieldType = 'YIELD_SCIENCE';

UPDATE Buildings
SET NoUnhappfromXSpecialists = '1'
WHERE Type = 'BUILDING_NATIONAL_COLLEGE';

-- East India Co.
UPDATE Buildings
SET ExtraLuxuries = '1'
WHERE Type = 'BUILDING_NATIONAL_TREASURY';

UPDATE Buildings
SET ResourceDiversityModifier = '25'
WHERE Type = 'BUILDING_NATIONAL_TREASURY';

UPDATE Building_ClassesNeededInCity
SET BuildingClassType = 'BUILDINGCLASS_MINT'
WHERE BuildingType = 'BUILDING_NATIONAL_TREASURY';

-- Circus Maximus

UPDATE Buildings
SET PrereqTech = 'TECH_METAL_CASTING'
WHERE Type = 'BUILDING_CIRCUS_MAXIMUS';

UPDATE Buildings
SET UnmoddedHappiness = '0'
WHERE Type = 'BUILDING_CIRCUS_MAXIMUS';

UPDATE Buildings
SET Happiness = '2'
WHERE Type = 'BUILDING_CIRCUS_MAXIMUS';

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_CIRCUS_MAXIMUS', 'BUILDINGCLASS_COLOSSEUM', 'YIELD_GOLD', 2);

-- National Intelligence Agency

-- Oxford University

-- Ironworks
UPDATE Building_ClassesNeededInCity
Set BuildingClassType = 'BUILDINGCLASS_FORGE'
WHERE BuildingType = 'BUILDING_IRONWORKS';

UPDATE Building_YieldChanges
Set Yield = '10'
WHERE BuildingType = 'BUILDING_IRONWORKS';

INSERT INTO Building_ResourceQuantity
	(BuildingType, ResourceType, Quantity)
VALUES
	('BUILDING_IRONWORKS', 'RESOURCE_IRON', 2);

-- Grand Temple
UPDATE Buildings
SET GreatWorkSlotType = 'GREAT_WORK_SLOT_MUSIC'
WHERE Type = 'BUILDING_GRAND_TEMPLE';

UPDATE Buildings
SET GreatWorkCount = '1'
WHERE Type = 'BUILDING_GRAND_TEMPLE';

-- Moved NIA

UPDATE Buildings
SET PrereqTech = 'TECH_COMPUTERS'
WHERE Type = 'BUILDING_INTELLIGENCE_AGENCY';

-- Palace

UPDATE Buildings
SET NoUnhappfromXSpecialists = '1'
WHERE Type = 'BUILDING_PALACE';

UPDATE Buildings
SET EnablesTechSteal = '1'
WHERE Type = 'BUILDING_PALACE';

UPDATE Buildings
SET EnablesTechSteal = '1'
WHERE Type = 'BUILDING_PALACE';

UPDATE Buildings
SET EnablesGWSteal = '1'
WHERE Type = 'BUILDING_PALACE';

-- Hermitage Remove Culture Mod
UPDATE Buildings
SET CultureRateModifier = '10'
WHERE Type = 'BUILDING_HERMITAGE';

-- Oxford Change
UPDATE Buildings
SET FreeTechs = '0'
WHERE Type = 'BUILDING_OXFORD_UNIVERSITY';

UPDATE Building_YieldChanges
SET Yield = '3'
WHERE BuildingType = 'BUILDING_OXFORD_UNIVERSITY';

INSERT INTO Building_FreeUnits (BuildingType, UnitType, NumUnits)
VALUES ('BUILDING_OXFORD_UNIVERSITY' , 'UNIT_SCIENTIST' , 1);

-- Spy Agency
UPDATE Buildings
SET EspionageModifier = '-15'
WHERE Type = 'BUILDING_INTELLIGENCE_AGENCY';

UPDATE Buildings
SET GlobalEspionageModifier = '0'
WHERE Type = 'BUILDING_INTELLIGENCE_AGENCY';

-- Recycling -- move to 
UPDATE Buildings
SET PrereqTech = 'TECH_ELECTRONICS'
WHERE Type = 'BUILDING_RECYCLING_CENTER';



-- Building Requirement
DELETE FROM Building_PrereqBuildingClasses;

-- Pop Requirement
UPDATE Buildings
SET NationalPopRequired = '20'
WHERE Type = 'BUILDING_HEROIC_EPIC';

UPDATE Buildings
SET NationalPopRequired = '25'
WHERE Type = 'BUILDING_NATIONAL_EPIC';

UPDATE Buildings
SET NationalPopRequired = '25'
WHERE Type = 'BUILDING_CIRCUS_MAXIMUS';

UPDATE Buildings
SET NationalPopRequired = '40'
WHERE Type = 'BUILDING_NATIONAL_TREASURY';

UPDATE Buildings
SET NationalPopRequired = '25'
WHERE Type = 'BUILDING_NATIONAL_COLLEGE';

UPDATE Buildings
SET NationalPopRequired = '40'
WHERE Type = 'BUILDING_IRONWORKS';

UPDATE Buildings
SET NationalPopRequired = '35'
WHERE Type = 'BUILDING_OXFORD_UNIVERSITY';

UPDATE Buildings
SET NationalPopRequired = '45'
WHERE Type = 'BUILDING_HERMITAGE';

UPDATE Buildings
SET NationalPopRequired = '70'
WHERE Type = 'BUILDING_INTELLIGENCE_AGENCY';

UPDATE Buildings
SET NationalPopRequired = '30'
WHERE Type = 'BUILDING_GRAND_TEMPLE';

-- City-Based Cost 

-- Pop Requirement

UPDATE Buildings SET NumCityCostMod = '10' WHERE Type IN 
('BUILDING_HEROIC_EPIC',
'BUILDING_NATIONAL_EPIC',
'BUILDING_CIRCUS_MAXIMUS',
'BUILDING_NATIONAL_TREASURY',
'BUILDING_NATIONAL_COLLEGE',
'BUILDING_IRONWORKS',
'BUILDING_OXFORD_UNIVERSITY',
'BUILDING_HERMITAGE',
'BUILDING_INTELLIGENCE_AGENCY',
'BUILDING_GRAND_TEMPLE');

-- NEW

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_HERMITAGE', 'YIELD_CULTURE', 25);

INSERT INTO Building_YieldFromBirth
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_NATIONAL_EPIC', 'YIELD_CULTURE', 15);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_GRAND_TEMPLE', 'BUILDINGCLASS_TEMPLE', 'YIELD_GOLD', 1),
	('BUILDING_GRAND_TEMPLE', 'BUILDINGCLASS_TEMPLE', 'YIELD_FAITH', 2);

INSERT INTO Building_YieldFromSpyAttack
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_INTELLIGENCE_AGENCY', 'YIELD_SCIENCE', 100),
	('BUILDING_INTELLIGENCE_AGENCY', 'YIELD_GOLD', 100),
	('BUILDING_WHITE_TOWER', 'YIELD_SCIENCE', 25),
	('BUILDING_WHITE_TOWER', 'YIELD_GOLD', 25);

INSERT INTO Building_YieldFromSpyDefense
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_WHITE_TOWER', 'YIELD_SCIENCE', 25),
	('BUILDING_WHITE_TOWER', 'YIELD_GOLD', 25);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_WHITE_TOWER', 'BUILDINGCLASS_CONSTABLE', 'YIELD_CULTURE', 1),
	('BUILDING_WHITE_TOWER', 'BUILDINGCLASS_ARMORY', 'YIELD_CULTURE', 1),
	('BUILDING_WHITE_TOWER', 'BUILDINGCLASS_CASTLE', 'YIELD_CULTURE', 1);

INSERT INTO Building_YieldFromTech
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_OXFORD_UNIVERSITY', 'YIELD_CULTURE', 50);

INSERT INTO Building_GoldenAgeYieldMod
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_NATIONAL_COLLEGE', 'YIELD_SCIENCE', 20);

INSERT INTO Building_WLTKDYieldMod
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CIRCUS_MAXIMUS', 'YIELD_CULTURE', 10),
	('BUILDING_CIRCUS_MAXIMUS', 'YIELD_GOLD', 10);

INSERT INTO Building_YieldFromConstruction
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_IRONWORKS', 'YIELD_SCIENCE', 25);

INSERT INTO Building_YieldFromPolicyUnlock
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_NATIONAL_EPIC', 'YIELD_GOLDEN_AGE_POINTS', 50);