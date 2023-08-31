-- Academy of Sciences

DELETE FROM Policy_BuildingClassHappiness
WHERE PolicyType = 'POLICY_ACADEMY_SCIENCES';

UPDATE Policies
SET AllCityFreeBuilding = NULL
WHERE Type = 'POLICY_ACADEMY_SCIENCES';

-- Cultural Revolution (now Socialist Realism)

UPDATE Policies
SET
	SharedIdeologyTourismModifier = 25,
	Level = 3
WHERE Type = 'POLICY_CULTURAL_REVOLUTION';

INSERT INTO Policy_GreatWorkYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_CULTURAL_REVOLUTION', 'YIELD_PRODUCTION', 4);

INSERT INTO Processes
	(Type, Description, Help, Strategy, IconAtlas, PortraitIndex, RequiredPolicy)
VALUES
	('PROCESS_TOURISM', 'TXT_KEY_PROCESS_TOURISM', 'TXT_KEY_PROCESS_TOURISM_HELP', 'TXT_KEY_PROCESS_TOURISM_STRATEGY', 'VP_PROCESS_ATLAS', 1, 'POLICY_CULTURAL_REVOLUTION');

INSERT INTO Process_ProductionYields
	(ProcessType, YieldType, Yield)
VALUES
	('PROCESS_TOURISM', 'YIELD_TOURISM', 15);


-- Dictatorship of the Proletariat (now Cultural Revolution)

UPDATE Policies
SET
	LessHappyTourismModifier = 25,
	Level = 2
WHERE Type = 'POLICY_DICTATORSHIP_PROLETARIAT';


-- Double Agents

UPDATE Policies
SET
	FreeSpy = 3,
	EspionageTurnsModifierFriendly = -1,
	EspionageTurnsModifierEnemy = 1
WHERE Type = 'POLICY_DOUBLE_AGENTS';


-- Hero of the People

UPDATE Policies
SET
	NumFreeGreatPeople = 1,
	GreatPeopleRateModifier = 25,
	IncludesOneShotFreeUnits = 1
WHERE Type = 'POLICY_HERO_OF_THE_PEOPLE';


-- Party Leadership (now Dictatorship of the Proletariat)

DELETE FROM Policy_CityYieldChanges
WHERE PolicyType = 'POLICY_PARTY_LEADERSHIP' AND YieldType = 'YIELD_PRODUCTION';

UPDATE Policy_CityYieldChanges
SET
	Yield = 7
WHERE PolicyType = 'POLICY_PARTY_LEADERSHIP';


-- Patriotic War

DELETE FROM UnitPromotions_UnitCombats
WHERE PromotionType = 'PROMOTION_NATIONALISM';

UPDATE Policies
SET
	CityCaptureHealLocal = 100
WHERE Type = 'POLICY_PATRIOTIC_WAR';

INSERT INTO Policy_UnitClassReplacements
	(PolicyType, ReplacedUnitClassType, ReplacementUnitClassType)
VALUES
	('POLICY_PATRIOTIC_WAR', 'UNITCLASS_TANK', 'UNITCLASS_PANZER');

INSERT INTO UnitPromotions_UnitCombats
	(PromotionType, UnitCombatType)
VALUES
	('PROMOTION_NATIONALISM', 'UNITCOMBAT_RECON'),
	('PROMOTION_NATIONALISM', 'UNITCOMBAT_ARCHER'),
	('PROMOTION_NATIONALISM', 'UNITCOMBAT_MOUNTED'),
	('PROMOTION_NATIONALISM', 'UNITCOMBAT_MELEE'),
	('PROMOTION_NATIONALISM', 'UNITCOMBAT_SIEGE'),
	('PROMOTION_NATIONALISM', 'UNITCOMBAT_GUN'),
	('PROMOTION_NATIONALISM', 'UNITCOMBAT_ARMOR');


-- Resettlement

UPDATE Policies
SET
	FreePopulation = 2,
	NoPartisans = 1
WHERE Type = 'POLICY_RESETTLEMENT';


-- Skyscrapers (now Communism)

UPDATE Policies
SET
	WonderProductionModifier = 20,
	BuildingPurchaseCostModifier = -25
WHERE Type = 'POLICY_SKYSCRAPERS';


-- Socialist Realism (now People's Army)

DELETE FROM Policy_BuildingClassHappiness
WHERE PolicyType = 'POLICY_SOCIALIST_REALISM';

DELETE FROM Policy_BuildingClassProductionModifiers
WHERE PolicyType = 'POLICY_SOCIALIST_REALISM';

UPDATE Policies
SET
	AllCityFreeBuilding = NULL
WHERE Type = 'POLICY_SOCIALIST_REALISM';


-- Worker's Faculties

UPDATE Policies
SET
	Level = 1
WHERE Type = 'POLICY_WORKERS_FACULTIES';

UPDATE Policy_BuildingClassYieldModifiers
SET
	YieldMod = 10
WHERE PolicyType = 'POLICY_WORKERS_FACULTIES';


-- Peace, Land, Bread

DELETE FROM Policy_BuildingClassHappiness
WHERE PolicyType = 'POLICY_UNIVERSAL_HEALTHCARE_O';

UPDATE Policies
SET
	Description = 'TXT_KEY_POLICY_UNIVERSAL_HEALTHCARE_O',
	Help = 'TXT_KEY_POLICY_UNIVERSAL_HEALTHCARE_O_HELP',
	Civilopedia = 'TXT_KEY_POLICY_UNIVERSAL_HEALTHCARE_TEXT_O',
	CityGrowthMod = 20,
	BuildingGoldMaintenanceMod = -20
WHERE Type = 'POLICY_UNIVERSAL_HEALTHCARE_O';


-- Young Pioneers (now Great Leap Forward)

DELETE FROM Policy_BuildingClassHappiness
WHERE PolicyType = 'POLICY_YOUNG_PIONEERS';

UPDATE Policies
SET
	NumFreeTechs = 1,
	OneShot = 1,
	StealTechFasterModifier = 100,
	Level = 2
WHERE Type = 'POLICY_YOUNG_PIONEERS';

INSERT INTO Policy_YieldFromNonSpecialistCitizens
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_YOUNG_PIONEERS', 'YIELD_SCIENCE', 34);


-- Iron Curtain

UPDATE Policies
SET InternalTradeRouteYieldModifier = 200
WHERE Type = 'POLICY_IRON_CURTAIN';

INSERT INTO Policy_YieldChangeTradeRoute
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_IRON_CURTAIN', 'YIELD_GOLD', 5),
	('POLICY_IRON_CURTAIN', 'YIELD_PRODUCTION', 5);


-- Spaceflight Pioneers

UPDATE Policies
SET
	GreatEngineerHurryModifier = 50
WHERE Type = 'POLICY_SPACEFLIGHT_PIONEERS';

INSERT INTO Policy_YieldGPExpend
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_SPACEFLIGHT_PIONEERS', 'YIELD_SCIENCE', 100);


-- Five Year Plan

DELETE FROM Policy_CityYieldChanges
WHERE PolicyType = 'POLICY_FIVE_YEAR_PLAN';

UPDATE Policies
SET
	BuildingProductionModifier = 20
WHERE Type = 'POLICY_FIVE_YEAR_PLAN';

UPDATE Policy_ImprovementYieldChanges
SET
	Yield = 3
WHERE PolicyType = 'POLICY_FIVE_YEAR_PLAN' AND ImprovementType IN ('IMPROVEMENT_MINE', 'IMPROVEMENT_QUARRY');

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_FIVE_YEAR_PLAN', 'IMPROVEMENT_LUMBERMILL', 'YIELD_PRODUCTION', 3),
	('POLICY_FIVE_YEAR_PLAN', 'IMPROVEMENT_WELL', 'YIELD_PRODUCTION', 3),
	('POLICY_FIVE_YEAR_PLAN', 'IMPROVEMENT_TERRACE_FARM', 'YIELD_PRODUCTION', 3),
	('POLICY_FIVE_YEAR_PLAN', 'IMPROVEMENT_SPAIN_HACIENDA', 'YIELD_PRODUCTION', 3),
	('POLICY_FIVE_YEAR_PLAN', 'IMPROVEMENT_EKI', 'YIELD_PRODUCTION', 3),
	('POLICY_FIVE_YEAR_PLAN', 'IMPROVEMENT_KUNA', 'YIELD_PRODUCTION', 3),
	('POLICY_FIVE_YEAR_PLAN', 'IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'YIELD_PRODUCTION', 3),
	('POLICY_FIVE_YEAR_PLAN', 'IMPROVEMENT_POLDER', 'YIELD_PRODUCTION', 3),
	('POLICY_FIVE_YEAR_PLAN', 'IMPROVEMENT_POLDER_WATER', 'YIELD_PRODUCTION', 3),
	('POLICY_FIVE_YEAR_PLAN', 'IMPROVEMENT_CHATEAU', 'YIELD_PRODUCTION', 3),
	('POLICY_FIVE_YEAR_PLAN', 'IMPROVEMENT_KASBAH', 'YIELD_PRODUCTION', 3),
	('POLICY_FIVE_YEAR_PLAN', 'IMPROVEMENT_BRAZILWOOD_CAMP', 'YIELD_PRODUCTION', 3),
	('POLICY_FIVE_YEAR_PLAN', 'IMPROVEMENT_MOAI', 'YIELD_PRODUCTION', 3),
	('POLICY_FIVE_YEAR_PLAN', 'IMPROVEMENT_FEITORIA', 'YIELD_PRODUCTION', 3),
	('POLICY_FIVE_YEAR_PLAN', 'IMPROVEMENT_MONGOLIA_ORDO', 'YIELD_PRODUCTION', 3),
	('POLICY_FIVE_YEAR_PLAN', 'IMPROVEMENT_SIHEYUAN', 'YIELD_PRODUCTION', 3);



----------------------
-- Combined Insertions
----------------------

-- Building Changes

INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
VALUES
	('POLICY_ACADEMY_SCIENCES', 'BUILDINGCLASS_LABORATORY', 'YIELD_SCIENCE', 2),
	('POLICY_SOCIALIST_REALISM', 'BUILDINGCLASS_PUBLIC_SCHOOL', 'YIELD_CULTURE', 5);

INSERT INTO Policy_BuildingClassProductionModifiers
	(PolicyType, BuildingClassType, ProductionModifier)
VALUES
	('POLICY_ACADEMY_SCIENCES', 'BUILDINGCLASS_LABORATORY', 100),
	('POLICY_SOCIALIST_REALISM', 'BUILDINGCLASS_MILITARY_ACADEMY', 100);

INSERT INTO Policy_FreeBuilding
			(PolicyType, BuildingClassType, Count)
VALUES		('POLICY_ACADEMY_SCIENCES', 'BUILDINGCLASS_LABORATORY', 5),
			('POLICY_SOCIALIST_REALISM', 'BUILDINGCLASS_MILITARY_ACADEMY', 5),
			('POLICY_WORKERS_FACULTIES', 'BUILDINGCLASS_FACTORY', 5);

INSERT INTO Policy_BuildingClassHappiness
	(PolicyType, BuildingClassType, Happiness)
VALUES
	('POLICY_DICTATORSHIP_PROLETARIAT', 'BUILDINGCLASS_FACTORY', 1),
	('POLICY_SOCIALIST_REALISM', 'BUILDINGCLASS_PUBLIC_SCHOOL', 2);