-- Wonder Consolation
UPDATE Defines SET Value = '1' WHERE Name = 'BALANCE_WONDER_BEATEN_CONSOLATION_PRIZE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK' AND Value= 1 );
UPDATE Defines SET Value = '3' WHERE Name = 'BALANCE_WONDER_BEATEN_CONSOLATION_PRIZE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK' AND Value= 3 );
UPDATE Defines SET Value = '4' WHERE Name = 'BALANCE_WONDER_BEATEN_CONSOLATION_PRIZE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK' AND Value= 4 );
UPDATE Defines SET Value = '5' WHERE Name = 'BALANCE_WONDER_BEATEN_CONSOLATION_PRIZE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK' AND Value= 5 );

UPDATE Defines SET Value = '0' WHERE Name = 'BALANCE_CULTURE_PERCENTAGE_VALUE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK_ENABLER' AND Value= 0 );
UPDATE Defines SET Value = '0' WHERE Name = 'BALANCE_GA_PERCENTAGE_VALUE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK_ENABLER' AND Value= 0 );
UPDATE Defines SET Value = '0' WHERE Name = 'BALANCE_SCIENCE_PERCENTAGE_VALUE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK_ENABLER' AND Value= 0 );
UPDATE Defines SET Value = '0' WHERE Name = 'BALANCE_FAITH_PERCENTAGE_VALUE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK_ENABLER' AND Value= 0 );

------------------
-- Ancient

-- Pyramids

UPDATE Buildings
SET PolicyBranchType = NULL, Cost = '150', PrereqTech = 'TECH_MINING', WorkerSpeedModifier = '0'
WHERE Type = 'BUILDING_PYRAMID';

UPDATE Building_FreeUnits
SET UnitType = 'UNIT_SETTLER', NumUnits = '1'
WHERE BuildingType = 'BUILDING_PYRAMID';

INSERT INTO Building_YieldFromGPExpend
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_PYRAMID', 'YIELD_GOLDEN_AGE_POINTS', 50);

-- Statue of Zeus
UPDATE Buildings
SET PolicyBranchType = NULL, Cost = '185', PrereqTech = 'TECH_BRONZE_WORKING', FreeBuildingThisCity = 'BUILDINGCLASS_BARRACKS'
WHERE Type = 'BUILDING_STATUE_ZEUS';

INSERT INTO Building_YieldChanges (BuildingType, YieldType, Yield)
VALUES ('BUILDING_STATUE_ZEUS' , 'YIELD_FAITH' , '1');

INSERT INTO Building_FreeUnits (BuildingType, UnitType, NumUnits)
VALUES ('BUILDING_STATUE_ZEUS' , 'UNIT_WORKER' , '1');

-- Stonehenge
UPDATE Buildings
SET Cost = '150', PrereqTech = 'TECH_THE_WHEEL', FreeBuildingThisCity = 'BUILDINGCLASS_GROVE', Help = 'TXT_KEY_BUILDING_STONEHENGE_HELP'
WHERE Type = 'BUILDING_STONEHENGE';

INSERT INTO Building_InstantYield (BuildingType, YieldType, Yield)
VALUES ('BUILDING_STONEHENGE' , 'YIELD_FAITH' , '60');

UPDATE Building_YieldChanges
SET Yield = '2'
WHERE BuildingType = 'BUILDING_STONEHENGE';

-- Halicarnassus
UPDATE Building_YieldChanges
SET Yield = '2'
WHERE BuildingType = 'BUILDING_MAUSOLEUM_HALICARNASSUS';

DELETE FROM Building_ResourceYieldChanges
WHERE BuildingType = 'BUILDING_MAUSOLEUM_HALICARNASSUS';

UPDATE Buildings
SET GreatPersonExpendGold = '0', WLTKDTurns = '20', FreeBuildingThisCity = 'BUILDINGCLASS_STONE_WORKS'
WHERE Type = 'BUILDING_MAUSOLEUM_HALICARNASSUS';

-- Artemis

UPDATE Buildings
SET FreeBuildingThisCity = 'BUILDINGCLASS_HERBALIST', NoUnhappfromXSpecialists = '2', PrereqTech = 'TECH_CALENDAR'
WHERE Type = 'BUILDING_TEMPLE_ARTEMIS';

UPDATE Building_UnitCombatProductionModifiers
SET Modifier = '25'
WHERE BuildingType = 'BUILDING_TEMPLE_ARTEMIS';

-- Petra
UPDATE Buildings
SET Cost = '185', PrereqTech = 'TECH_HORSEBACK_RIDING'
WHERE Type = 'BUILDING_PETRA';

UPDATE Building_YieldChanges
SET Yield = '2'
WHERE BuildingType = 'BUILDING_PETRA';

DELETE FROM Building_TerrainYieldChanges
WHERE BuildingType = 'BUILDING_PETRA';

DELETE FROM Building_FeatureYieldChanges
WHERE BuildingType = 'BUILDING_PETRA';

INSERT INTO Building_TerrainYieldChanges
	(BuildingType, TerrainType, YieldType, Yield)
VALUES
	('BUILDING_PETRA', 'TERRAIN_DESERT', 'YIELD_GOLD', 1);

-----------------
-- Classical

-- Great Library

UPDATE Buildings
SET Cost = '200'
WHERE Type = 'BUILDING_GREAT_LIBRARY';

-- Hanging Gardens
UPDATE Buildings
SET Cost = '200', PolicyBranchType = NULL
WHERE Type = 'BUILDING_HANGING_GARDEN';

UPDATE Building_YieldChanges
SET Yield = '10'
WHERE BuildingType = 'BUILDING_HANGING_GARDEN' AND YieldType = 'YIELD_FOOD';

-- Great Lighthouse
INSERT INTO Building_YieldChanges (BuildingType, YieldType, Yield)
VALUES ('BUILDING_GREAT_LIGHTHOUSE' , 'YIELD_PRODUCTION' , '2');

UPDATE Buildings
SET FreePromotion = 'PROMOTION_NAVAL_SENTRY_II', Cost = '200', MinAreaSize = '10'
WHERE Type = 'BUILDING_GREAT_LIGHTHOUSE';

-- Colossus
UPDATE Buildings
SET PrereqTech = 'TECH_METAL_CASTING', Cost = '250', MinAreaSize = '10'
WHERE Type = 'BUILDING_COLOSSUS';

-- Terracotta Army
UPDATE Buildings
SET PolicyBranchType = NULL, Cost = '200', CitySupplyFlat = '5', WorkerSpeedModifier = '25', InstantMilitaryIncrease = '0'
WHERE Type = 'BUILDING_TERRACOTTA_ARMY';

INSERT INTO Building_YieldFromVictoryGlobal 
	(BuildingType, YieldType, Yield)
VALUES  
	('BUILDING_TERRACOTTA_ARMY' , 'YIELD_CULTURE' , '10');

-- Parthenon
UPDATE Buildings
SET PolicyBranchType = NULL, ThemingBonusHelp = 'TXT_KEY_PARTHENON_THEMING_BONUS_HELP', GreatWorkCount = '2', CitySupplyModifier = '10'
WHERE Type = 'BUILDING_PARTHENON';

UPDATE Building_YieldChanges
SET Yield = '2'
WHERE BuildingType = 'BUILDING_PARTHENON';

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_PARTHENON', 'BUILDINGCLASS_AMPHITHEATER', 'YIELD_CULTURE', 1),
	('BUILDING_PARTHENON', 'BUILDINGCLASS_AMPHITHEATER', 'YIELD_SCIENCE', 1);

-- Oracle
DELETE FROM Building_YieldChanges
WHERE BuildingType = 'BUILDING_ORACLE';

UPDATE Buildings
SET FreePolicies = '0', FreeBuildingThisCity = 'BUILDINGCLASS_TEMPLE'
WHERE Type = 'BUILDING_ORACLE';

INSERT INTO Building_InstantYield
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_ORACLE', 'YIELD_CULTURE', 400),
	('BUILDING_ORACLE', 'YIELD_SCIENCE', 400);

-- Great Wall

INSERT INTO Building_FreeUnits (BuildingType, UnitType, NumUnits)
VALUES ('BUILDING_GREAT_WALL' , 'UNIT_GREAT_GENERAL' , '1');

UPDATE Buildings
SET ObsoleteTech = 'TECH_GUNPOWDER', CitySupplyFlat = '3'
WHERE Type = 'BUILDING_GREAT_WALL';

-- Angkor Wat

UPDATE Buildings
SET GlobalPlotCultureCostModifier = '0'
WHERE Type = 'BUILDING_ANGKOR_WAT';

UPDATE Buildings
SET BorderGrowthRateIncreaseGlobal = '50'
WHERE Type = 'BUILDING_ANGKOR_WAT';

UPDATE Buildings
SET PrereqTech = 'TECH_CURRENCY', Cost = '250', FreeBuildingThisCity = 'BUILDINGCLASS_MANDIR'
WHERE Type = 'BUILDING_ANGKOR_WAT';

INSERT INTO Building_YieldChanges (BuildingType, YieldType, Yield)
VALUES ('BUILDING_ANGKOR_WAT' , 'YIELD_FAITH' , '1');

----------------------------
-- Medieval

-- Alhambra

UPDATE Buildings
SET PolicyType = 'POLICY_HONOR_FINISHER', Cost = '400', CultureRateModifier = '10', FreePromotion = 'PROMOTION_ALHAMBRA', TrainedFreePromotion = NULL
WHERE Type = 'BUILDING_ALHAMBRA';

-- Mosque of Djenne
UPDATE Buildings
SET PolicyBranchType = NULL, Cost = '400', PolicyType = 'POLICY_TRADITION_FINISHER', PrereqTech = 'TECH_EDUCATION', ExtraMissionarySpreads = '0', SpecialistType = 'SPECIALIST_SCIENTIST', GreatPeopleRateChange = '1', FreeBuildingThisCity = 'BUILDINGCLASS_MOSQUE'
WHERE Type = 'BUILDING_MOSQUE_OF_DJENNE';

-- St. Basil's
UPDATE Buildings
SET Cost = '1600', PrereqTech = 'TECH_REPLACEABLE_PARTS'
WHERE Type = 'BUILDING_KREMLIN';

UPDATE Building_UnitCombatProductionModifiers
SET Modifier = 33
WHERE BuildingType = 'BUILDING_KREMLIN';

-- Borobudur
UPDATE Buildings
SET PrereqTech = 'TECH_THEOLOGY', Cost = '400', FreeBuildingThisCity = 'BUILDINGCLASS_STUPA', ExtraMissionarySpreadsGlobal = '1'
WHERE Type = 'BUILDING_BOROBUDUR';

UPDATE Building_FreeUnits
SET NumUnits  = '2'
WHERE BuildingType = 'BUILDING_BOROBUDUR';

-- Hagia Sophia

UPDATE Buildings
SET Cost = '400', FreeBuildingThisCity = 'BUILDINGCLASS_CHURCH', ExtraMissionaryStrengthGlobal = '25'
WHERE Type = 'BUILDING_HAGIA_SOPHIA';

DELETE FROM Building_YieldChanges
WHERE BuildingType = 'BUILDING_HAGIA_SOPHIA';

-- Machu Pichu
UPDATE Buildings
SET PrereqTech = 'TECH_PHYSICS', Cost = '400', CityConnectionTradeRouteModifier = '15'
WHERE Type = 'BUILDING_MACHU_PICHU';

-- Notre Dame
UPDATE Buildings
SET UnmoddedHappiness = '0', Happiness = '1', Cost = '500', PrereqTech = 'TECH_MACHINERY', ThemingBonusHelp = 'TXT_KEY_BUILDING_NOTRE_DAME_THEMING_BONUS_HELP', GreatWorkCount = '2', GreatWorkSlotType = 'GREAT_WORK_SLOT_ART_ARTIFACT', Help = 'TXT_KEY_BUILDING_NOTRE_DAME_HELP', FreeBuildingThisCity = 'BUILDINGCLASS_CATHEDRAL', GoldenAge = '1'
WHERE Type = 'BUILDING_NOTRE_DAME';

-- Forbidden Palace
UPDATE Buildings
SET PrereqTech = 'TECH_CIVIL_SERVICE', PolicyType = 'POLICY_LIBERTY_FINISHER', PolicyBranchType = NULL, Cost = '500', UnhappinessModifier = '0', SingleLeagueVotes = '0'
WHERE Type = 'BUILDING_FORBIDDEN_PALACE';

INSERT INTO Building_HurryModifiers (BuildingType, HurryType, HurryCostModifier)
VALUES ('BUILDING_FORBIDDEN_PALACE' , 'HURRY_GOLD' , '-15');

----------------------
-- Renaissance

-- Himeji Castle

UPDATE Buildings
SET Cost = '800', GlobalDefenseMod = '10', CitySupplyFlat = '5', FreeBuildingThisCity = 'BUILDINGCLASS_FORTRESS'
WHERE Type = 'BUILDING_HIMEJI_CASTLE';

-- Chichen Itza

UPDATE Buildings
SET PrereqTech = 'TECH_ASTRONOMY', Cost = '800', Happiness = '0', FreeBuildingThisCity = 'BUILDINGCLASS_TEOCALLI'
WHERE Type = 'BUILDING_CHICHEN_ITZA';

-- Globe Theater
UPDATE Buildings
SET Cost = '800', NoUnhappfromXSpecialistsGlobal = '1'
WHERE Type = 'BUILDING_GLOBE_THEATER';

UPDATE Building_ThemingBonuses SET Bonus = '8' WHERE BuildingType = 'BUILDING_GLOBE_THEATER';


-- Pisa
UPDATE Buildings
SET PrereqTech = 'TECH_CHEMISTRY', GlobalGreatPeopleRateModifier = '10', GreatPeopleRateModifier = '15', Cost = '800'
WHERE Type = 'BUILDING_LEANING_TOWER';

-- Uffizi
UPDATE Buildings
SET PrereqTech = 'TECH_ECONOMICS', PolicyBranchType = NULL, Cost = '900', NoUnhappfromXSpecialistsGlobal = '1', FreeBuildingThisCity = 'BUILDINGCLASS_GALLERY'
WHERE Type = 'BUILDING_UFFIZI';

UPDATE Building_ThemingBonuses SET Bonus = '14' WHERE BuildingType = 'BUILDING_UFFIZI';

-- Taj Mahal
UPDATE Buildings
SET Happiness = '0', Cost = '900', PolicyType = NULL
WHERE Type = 'BUILDING_TAJ_MAHAL';

UPDATE Building_YieldChanges
SET Yield = '2'
WHERE BuildingType = 'BUILDING_TAJ_MAHAL';

INSERT INTO Building_YieldChangesPerReligion
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TAJ_MAHAL', 'YIELD_CULTURE', 300),
	('BUILDING_TAJ_MAHAL', 'YIELD_SCIENCE', 300),
	('BUILDING_TAJ_MAHAL', 'YIELD_FAITH', 300);

-- Porcelain Tower

UPDATE Buildings
SET PrereqTech = 'TECH_ARCHITECTURE', PolicyBranchType = NULL, Cost = '900'
WHERE Type = 'BUILDING_PORCELAIN_TOWER';

-- Sistine Chapel

UPDATE Buildings
SET Cost = '900', GlobalCultureRateModifier = '10'
WHERE Type = 'BUILDING_SISTINE_CHAPEL';

-- Red Fort
UPDATE Buildings
SET GlobalDefenseMod = '0', CitySupplyModifierGlobal = '5', Cost = '900', CityRangedStrikeRange = '1', RangedStrikeModifier = '10', PolicyType = 'POLICY_PIETY_FINISHER', FreeBuildingThisCity = 'BUILDINGCLASS_ARSENAL'
WHERE Type = 'BUILDING_RED_FORT';

INSERT INTO Building_FreeUnits (BuildingType, UnitType, NumUnits)
VALUES ('BUILDING_RED_FORT' , 'UNIT_ENGINEER' , '1');

--------------------
-- Industrial

-- Brandenburg Gate

UPDATE Buildings
SET PolicyBranchType = NULL, Cost = '1250', CitySupplyFlat = '10'
WHERE Type = 'BUILDING_BRANDENBURG_GATE';

INSERT INTO Building_DomainFreeExperiencesGlobal
	(BuildingType, DomainType, Experience)
VALUES
	('BUILDING_BRANDENBURG_GATE', 'DOMAIN_LAND', 15),
	('BUILDING_BRANDENBURG_GATE', 'DOMAIN_AIR', 15),
	('BUILDING_BRANDENBURG_GATE', 'DOMAIN_SEA', 15);

DELETE FROM Building_DomainFreeExperiences WHERE BuildingType = 'BUILDING_BRANDENBURG_GATE';

-- Louvre
UPDATE Buildings
SET PolicyBranchType = NULL, FreeBuildingThisCity = 'BUILDINGCLASS_MUSEUM', PolicyType = 'POLICY_AESTHETICS_FINISHER', Cost = '1250', PrereqTech = 'TECH_ARCHAEOLOGY'
WHERE Type = 'BUILDING_LOUVRE';

INSERT INTO Building_FreeUnits (BuildingType, UnitType, NumUnits)
VALUES ('BUILDING_LOUVRE' , 'UNIT_ARCHAEOLOGIST' , '2');

-- Eiffel Tower
UPDATE Buildings
SET UnmoddedHappiness = '0', PrereqTech = 'TECH_DYNAMITE', Cost = '1250', PolicyCostModifier = '-10', EnhancedYieldTech = NULL, TechEnhancedTourism = '0'
WHERE Type = 'BUILDING_EIFFEL_TOWER';

-- Big Ben

UPDATE Buildings
SET PolicyType = 'POLICY_PATRONAGE_FINISHER', Cost = '1250', PolicyBranchType = NULL, ExtraLeagueVotes = '2'
WHERE Type = 'BUILDING_BIG_BEN';

DELETE FROM Building_HurryModifiers WHERE BuildingType = 'BUILDING_BIG_BEN';

INSERT INTO Building_FreeUnits (BuildingType, UnitType, NumUnits)
VALUES ('BUILDING_BIG_BEN' , 'UNIT_MERCHANT' , '1');

-- Neuschwanstein

UPDATE Buildings
SET Happiness = '0', Cost = '1000'
WHERE Type = 'BUILDING_NEUSCHWANSTEIN';

UPDATE Building_BuildingClassYieldChanges
SET YieldChange = '1'
WHERE BuildingType = 'BUILDING_NEUSCHWANSTEIN';

-----------------------
--Modern

-- Statue of Liberty
UPDATE Buildings
SET PrereqTech = 'TECH_BIOLOGY', Cost = '1600'
WHERE Type = 'BUILDING_STATUE_OF_LIBERTY';

-- Cristo Redentor
UPDATE Buildings
SET Cost = '1700', PolicyCostModifier = '0', TechEnhancedTourism = '0', EnhancedYieldTech = NULL
WHERE Type = 'BUILDING_CRISTO_REDENTOR';

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_CRISTO_REDENTOR', 'BUILDINGCLASS_HOTEL', 'YIELD_TOURISM', 2),
	('BUILDING_CRISTO_REDENTOR', 'BUILDINGCLASS_HOTEL', 'YIELD_GOLDEN_AGE_POINTS', 2),
	('BUILDING_CRISTO_REDENTOR', 'BUILDINGCLASS_HOTEL', 'YIELD_CULTURE', 2);


INSERT INTO Building_YieldChanges (BuildingType, YieldType, Yield)
VALUES ('BUILDING_CRISTO_REDENTOR' , 'YIELD_TOURISM' , '25');

-- Prora
UPDATE Buildings
SET Cost = '1700', UnmoddedHappiness = '0', Happiness = '1', MinAreaSize = '10'
WHERE Type = 'BUILDING_PRORA_RESORT';

INSERT INTO Building_YieldFromPillageGlobal
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_PRORA_RESORT', 'YIELD_CULTURE', 25);
	
-- Broadway
UPDATE Buildings
SET Cost = '1700', PolicyType = 'POLICY_COMMERCE_FINISHER'
WHERE Type = 'BUILDING_BROADWAY';

INSERT INTO Building_YieldFromConstruction
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BROADWAY', 'YIELD_CULTURE', 250);

-------------------------
-- Atomic
-- Pentagon
UPDATE Buildings
SET Cost = '1900', PolicyType = 'POLICY_EXPLORATION_FINISHER', CitySupplyFlatGlobal = '1', UnitUpgradeCostMod = '0', AirModifierGlobal = '2'
WHERE Type = 'BUILDING_PENTAGON';

INSERT INTO Building_FreeUnits (BuildingType, UnitType, NumUnits)
VALUES ('BUILDING_PENTAGON' , 'UNIT_JET_FIGHTER' , '2');

INSERT INTO Building_UnitCombatProductionModifiers 
(BuildingType, UnitCombatType, Modifier)
VALUES
	('BUILDING_PENTAGON', 'UNITCOMBAT_FIGHTER', 50),
	('BUILDING_PENTAGON', 'UNITCOMBAT_BOMBER', 50);

INSERT INTO Building_UnitCombatFreeExperiences
(BuildingType, UnitCombatType, Experience)
VALUES
	('BUILDING_PENTAGON', 'UNITCOMBAT_FIGHTER', 20),
	('BUILDING_PENTAGON', 'UNITCOMBAT_BOMBER', 20);

-- Bletchley

UPDATE Buildings
SET PolicyType = 'POLICY_RATIONALISM_FINISHER'
WHERE Type = 'BUILDING_BLETCHLEY_PARK';

----------------
-- Information

-- CN Tower
UPDATE Buildings SET 
	Cost = 2300, 
	FreeBuilding = NULL, 
	FreeBuildingThisCity = 'BUILDINGCLASS_BROADCAST_TOWER', 
	LandmarksTourismPercent = 25, 
	GreatWorksTourismModifier = 25,
	GlobalLandmarksTourismPercent = 25, 
	GlobalGreatWorksTourismModifier = 25,
	HappinessPerCity = '1', 
	GlobalPopulationChange = '0'
WHERE Type = 'BUILDING_CN_TOWER';

INSERT INTO Building_GreatWorkYieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CN_TOWER', 'YIELD_GOLD', 2),
	('BUILDING_CN_TOWER', 'YIELD_TOURISM', 2);

-- Sydney Opera House
UPDATE Buildings
SET PrereqTech = 'TECH_SATELLITES', Cost = '2300', CultureRateModifier = '25', MinAreaSize = '10'
WHERE Type = 'BUILDING_SYDNEY_OPERA_HOUSE';

-- Move Apollo 
UPDATE Projects
SET TechPrereq = 'TECH_SATELLITES'
WHERE Type = 'PROJECT_APOLLO_PROGRAM';

-- Move Hubble
UPDATE Buildings
SET PrereqTech = 'TECH_ROBOTICS', Cost = '3000'
WHERE Type = 'BUILDING_HUBBLE';

UPDATE Building_FreeUnits
SET NumUnits = '1'
WHERE BuildingType = 'BUILDING_HUBBLE';

-- Move Great Firewall
UPDATE Buildings
SET PrereqTech = 'TECH_INTERNET', EspionageModifier = '-75', Cost = '3000'
WHERE Type = 'BUILDING_GREAT_FIREWALL';

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_GREAT_FIREWALL', 'BUILDINGCLASS_LABORATORY', 'YIELD_SCIENCE', 10);

-- ISS

INSERT INTO Building_YieldFromTech
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_INTERNATIONAL_SPACE_STATION', 'YIELD_PRODUCTION', 100);

-- Projects

UPDATE LeagueProjects
SET CostPerPlayer = '8250'
WHERE Type = 'LEAGUE_PROJECT_INTERNATIONAL_SPACE_STATION';

UPDATE LeagueProjects
SET CostPerPlayer = '3750'
WHERE Type = 'LEAGUE_PROJECT_WORLD_FAIR';

UPDATE LeagueProjects
SET CostPerPlayer = '6750'
WHERE Type = 'LEAGUE_PROJECT_WORLD_GAMES';

-- NEW DATA


INSERT INTO Building_YieldPerXTerrainTimes100
	(BuildingType, TerrainType, YieldType, Yield)
VALUES
	('BUILDING_MACHU_PICHU', 'TERRAIN_MOUNTAIN', 'YIELD_FOOD', 100),
	('BUILDING_MACHU_PICHU', 'TERRAIN_MOUNTAIN', 'YIELD_CULTURE', 100),
	('BUILDING_MACHU_PICHU', 'TERRAIN_MOUNTAIN', 'YIELD_FAITH', 100),
	('BUILDING_MACHU_PICHU', 'TERRAIN_MOUNTAIN', 'YIELD_PRODUCTION', 100);

INSERT INTO Building_WLTKDYieldMod
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MAUSOLEUM_HALICARNASSUS', 'YIELD_PRODUCTION', 10),
	('BUILDING_MAUSOLEUM_HALICARNASSUS', 'YIELD_SCIENCE', 10),
	('BUILDING_MAUSOLEUM_HALICARNASSUS', 'YIELD_GOLD', 10);

INSERT INTO Building_YieldFromGPExpend
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MOSQUE_OF_DJENNE', 'YIELD_SCIENCE', 50);

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TAJ_MAHAL', 'YIELD_GOLDEN_AGE_POINTS', 50);

INSERT INTO Building_ThemingBonuses
	(BuildingType, Description, Bonus, RequiresOwner, RequiresUniquePlayers, RequiresAnyButOwner, AIPriority)
VALUES
	('BUILDING_PARTHENON', 'TXT_KEY_THEMING_BONUS_BUILDING_PARTHENON', 6, 1, 0, 0, 2),
	('BUILDING_NOTRE_DAME', 'TXT_KEY_THEMING_BONUS_BUILDING_NOTRE_DAME', 8, 0, 1, 1, 2);

-- New Wonder Yields and Data
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SLATER_MILL', 'YIELD_PRODUCTION', 5),
	('BUILDING_SLATER_MILL', 'YIELD_CULTURE', 1),
	('BUILDING_MOTHERLAND_STATUE', 'YIELD_CULTURE', 1),
	('BUILDING_CN_TOWER', 'YIELD_CULTURE', 1),
	('BUILDING_PRORA_RESORT', 'YIELD_CULTURE', 1),
	('BUILDING_BLETCHLEY_PARK', 'YIELD_SCIENCE', 5),
	('BUILDING_BLETCHLEY_PARK', 'YIELD_CULTURE', 1),
	('BUILDING_EMPIRE_STATE_BUILDING', 'YIELD_CULTURE', 1),
	('BUILDING_CERN', 'YIELD_SCIENCE', 10),
	('BUILDING_CERN', 'YIELD_CULTURE', 1),
	('BUILDING_GREAT_FIREWALL', 'YIELD_CULTURE', 1),
	('BUILDING_HUBBLE', 'YIELD_CULTURE', 1),
	('BUILDING_SYDNEY_OPERA_HOUSE', 'YIELD_CULTURE', 1),
	('BUILDING_MOSQUE_OF_DJENNE', 'YIELD_FAITH', 1),
	('BUILDING_MOSQUE_OF_DJENNE', 'YIELD_SCIENCE', 1),
	('BUILDING_MACHU_PICHU', 'YIELD_CULTURE', 1),
	('BUILDING_NOTRE_DAME', 'YIELD_CULTURE', 1),
	('BUILDING_KARLSTEJN', 'YIELD_FAITH', 4),
	('BUILDING_KARLSTEJN', 'YIELD_CULTURE', 3),
	('BUILDING_INTERNATIONAL_SPACE_STATION', 'YIELD_CULTURE', 1),
	('BUILDING_STATUE_ZEUS', 'YIELD_CULTURE', 1),
	('BUILDING_STONEHENGE', 'YIELD_CULTURE', 1),
	('BUILDING_ORACLE', 'YIELD_SCIENCE', 1),
	('BUILDING_ORACLE', 'YIELD_CULTURE', 1),
	('BUILDING_BOROBUDUR', 'YIELD_CULTURE', 1),
	('BUILDING_HAGIA_SOPHIA', 'YIELD_FAITH', 1),
	('BUILDING_HAGIA_SOPHIA', 'YIELD_CULTURE', 2);

INSERT INTO Building_ResourceQuantity
	(BuildingType, ResourceType, Quantity)
VALUES
	('BUILDING_SLATER_MILL', 'RESOURCE_COAL', 4);

INSERT INTO Building_SpecialistYieldChanges
	(BuildingType, SpecialistType, YieldType, Yield)
VALUES
	('BUILDING_GLOBE_THEATER', 'SPECIALIST_ARTIST', 'YIELD_GOLDEN_AGE_POINTS', 2),
	('BUILDING_GLOBE_THEATER', 'SPECIALIST_MUSICIAN', 'YIELD_GOLDEN_AGE_POINTS', 2),
	('BUILDING_GLOBE_THEATER', 'SPECIALIST_WRITER', 'YIELD_GOLDEN_AGE_POINTS', 2),
	('BUILDING_BLETCHLEY_PARK', 'SPECIALIST_SCIENTIST', 'YIELD_SCIENCE', 1);


INSERT INTO Building_SpecialistYieldChanges
	(BuildingType, SpecialistType, YieldType, Yield)
VALUES
	('BUILDING_EMPIRE_STATE_BUILDING', 'SPECIALIST_CITIZEN', 'YIELD_GOLD', 1),
	('BUILDING_EMPIRE_STATE_BUILDING', 'SPECIALIST_ARTIST', 'YIELD_GOLD', 1),
	('BUILDING_EMPIRE_STATE_BUILDING', 'SPECIALIST_SCIENTIST', 'YIELD_GOLD', 1),
	('BUILDING_EMPIRE_STATE_BUILDING', 'SPECIALIST_MERCHANT', 'YIELD_GOLD', 1),
	('BUILDING_EMPIRE_STATE_BUILDING', 'SPECIALIST_ENGINEER', 'YIELD_GOLD', 1),
	('BUILDING_EMPIRE_STATE_BUILDING', 'SPECIALIST_WRITER', 'YIELD_GOLD', 1),
	('BUILDING_EMPIRE_STATE_BUILDING', 'SPECIALIST_MUSICIAN', 'YIELD_GOLD', 1);

INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_MINOR', 'BUILDINGCLASS_CERN', NULL),
	('CIVILIZATION_MINOR', 'BUILDINGCLASS_EMPIRE_STATE_BUILDING', NULL),
	('CIVILIZATION_MINOR', 'BUILDINGCLASS_KARLSTEJN', NULL),
	('CIVILIZATION_MINOR', 'BUILDINGCLASS_SLATER_MILL', NULL),
	('CIVILIZATION_MINOR', 'BUILDINGCLASS_MOTHERLAND_STATUE', NULL),
	('CIVILIZATION_MINOR', 'BUILDINGCLASS_BLETCHLEY_PARK', NULL),
	('CIVILIZATION_BARBARIAN', 'BUILDINGCLASS_CERN', NULL),
	('CIVILIZATION_BARBARIAN', 'BUILDINGCLASS_EMPIRE_STATE_BUILDING', NULL),
	('CIVILIZATION_BARBARIAN', 'BUILDINGCLASS_KARLSTEJN', NULL),
	('CIVILIZATION_BARBARIAN', 'BUILDINGCLASS_SLATER_MILL', NULL),
	('CIVILIZATION_BARBARIAN', 'BUILDINGCLASS_MOTHERLAND_STATUE', NULL),
	('CIVILIZATION_BARBARIAN', 'BUILDINGCLASS_BLETCHLEY_PARK', NULL);

--Motherland Calls Speech
INSERT INTO Audio_Sounds 
			(SoundID, 										Filename, 							LoadType)
VALUES		('SND_WONDER_SPEECH_MOTHERLAND_STATUE', 		'MotherlandStatue', 				'DynamicResident');

INSERT INTO Audio_2DSounds 
			(ScriptID, 										SoundID, 									SoundType, 		MinVolume, 	MaxVolume,	IsMusic,	Looping)
VALUES		('AS2D_WONDER_SPEECH_MOTHERLAND_STATUE', 		'SND_WONDER_SPEECH_MOTHERLAND_STATUE', 		'GAME_SPEECH', 	80, 		170, 		0, 			0);
