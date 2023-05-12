-- Their Finest Hour

UPDATE Policies
SET
	MaxAirUnitsChangeGlobal = 2, CityStrengthMod = 0, FlatDefenseFromAirUnits = 3
WHERE Type = 'POLICY_THEIR_FINEST_HOUR';

/*INSERT INTO Policy_UnitCombatProductionModifiers
	(PolicyType, UnitCombatType, ProductionModifier)
VALUES
	('POLICY_THEIR_FINEST_HOUR', 'UNITCOMBAT_BOMBER', 25),
	('POLICY_THEIR_FINEST_HOUR', 'UNITCOMBAT_FIGHTER', 25);*/

INSERT INTO Policy_UnitClassReplacements
	(PolicyType, ReplacedUnitClassType, ReplacementUnitClassType)
VALUES
	('POLICY_THEIR_FINEST_HOUR', 'UNITCLASS_BOMBER', 'UNITCLASS_B17');


-- Arsenal of Democracy

UPDATE Policies
SET 
	MilitaryUnitGiftExtraInfluence = 40,
	InfluenceGPExpend = 10
WHERE Type = 'POLICY_ARSENAL_DEMOCRACY';


-- Capitalism

DELETE FROM Policy_BuildingClassHappiness
WHERE PolicyType = 'POLICY_CAPITALISM';

UPDATE Policies
SET
	NoUnhappfromXSpecialists = 2,
	HappfromXSpecialists = 2,
	Level = 2
WHERE Type = 'POLICY_CAPITALISM';

INSERT INTO Policy_SpecialistExtraYields
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_CAPITALISM', 'YIELD_GOLD', 1),
	('POLICY_CAPITALISM', 'YIELD_SCIENCE', 1);


-- Covert Action

UPDATE Policies
SET
	FreeSpy = 1
WHERE Type = 'POLICY_COVERT_ACTION';


-- Open Society (Avant Garde)

UPDATE Policies
SET
	GreatPeopleRateModifier = 33
WHERE Type = 'POLICY_OPEN_SOCIETY';


-- New Deal

UPDATE Policy_ImprovementYieldChanges
SET
	Yield = 6
WHERE PolicyType = 'POLICY_NEW_DEAL';

INSERT INTO Policy_ImprovementYieldChanges
		(PolicyType, 		ImprovementType, YieldType, 		Yield)
SELECT 	'POLICY_NEW_DEAL', 	ImprovementType, 'YIELD_TOURISM', 	2
FROM Policy_ImprovementYieldChanges WHERE PolicyType = 'POLICY_NEW_DEAL';

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_NEW_DEAL', 'IMPROVEMENT_LANDMARK', 'YIELD_TOURISM', 8),
	('POLICY_NEW_DEAL', 'IMPROVEMENT_CITADEL', 'YIELD_PRODUCTION', 6),
	('POLICY_NEW_DEAL', 'IMPROVEMENT_CITADEL', 'YIELD_TOURISM', 2),
	('POLICY_NEW_DEAL', 'IMPROVEMENT_MONGOLIA_ORDO', 'YIELD_PRODUCTION', 6),
	('POLICY_NEW_DEAL', 'IMPROVEMENT_MONGOLIA_ORDO', 'YIELD_TOURISM', 2);


-- Civil Society

UPDATE Policies
SET
	HalfSpecialistFood = 0,
	SpecialistFoodChange = -2,
	Level = 1
WHERE Type = 'POLICY_CIVIL_SOCIETY';

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_CIVIL_SOCIETY', 'IMPROVEMENT_FARM', 				'YIELD_FOOD', 4),
	('POLICY_CIVIL_SOCIETY', 'IMPROVEMENT_PLANTATION', 			'YIELD_FOOD', 4),
	('POLICY_CIVIL_SOCIETY', 'IMPROVEMENT_CAMP', 				'YIELD_FOOD', 4),
	('POLICY_CIVIL_SOCIETY', 'IMPROVEMENT_TERRACE_FARM', 		'YIELD_FOOD', 4),
	('POLICY_CIVIL_SOCIETY', 'IMPROVEMENT_SPAIN_HACIENDA', 		'YIELD_FOOD', 4),
	('POLICY_CIVIL_SOCIETY', 'IMPROVEMENT_EKI', 				'YIELD_FOOD', 4),
	('POLICY_CIVIL_SOCIETY', 'IMPROVEMENT_KUNA', 				'YIELD_FOOD', 4),
	('POLICY_CIVIL_SOCIETY', 'IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'YIELD_FOOD', 4),
	('POLICY_CIVIL_SOCIETY', 'IMPROVEMENT_POLDER', 				'YIELD_FOOD', 4),
	('POLICY_CIVIL_SOCIETY', 'IMPROVEMENT_POLDER_WATER', 		'YIELD_FOOD', 4),
	('POLICY_CIVIL_SOCIETY', 'IMPROVEMENT_CHATEAU', 			'YIELD_FOOD', 4),
	('POLICY_CIVIL_SOCIETY', 'IMPROVEMENT_KASBAH', 				'YIELD_FOOD', 4),
	('POLICY_CIVIL_SOCIETY', 'IMPROVEMENT_BRAZILWOOD_CAMP', 	'YIELD_FOOD', 4),
	('POLICY_CIVIL_SOCIETY', 'IMPROVEMENT_MOAI', 				'YIELD_FOOD', 4),
	('POLICY_CIVIL_SOCIETY', 'IMPROVEMENT_FEITORIA', 			'YIELD_FOOD', 4),
	('POLICY_CIVIL_SOCIETY', 'IMPROVEMENT_MONGOLIA_ORDO', 		'YIELD_FOOD', 4),
	('POLICY_CIVIL_SOCIETY', 'IMPROVEMENT_SIHEYUAN', 			'YIELD_FOOD', 4);


-- Treaty Organization

UPDATE Policies
SET
	FreeWCVotes = 4,
	ProtectedMinorPerTurnInfluence = 200
WHERE Type = 'POLICY_TREATY_ORGANIZATION';


-- Creative Expression

UPDATE Policy_GreatWorkYieldChanges
SET
	YieldType = 'YIELD_TOURISM',
	Yield = 2
WHERE PolicyType = 'POLICY_CREATIVE_EXPRESSION';

INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
VALUES
	('POLICY_CREATIVE_EXPRESSION', 'BUILDINGCLASS_OPERA_HOUSE', 'YIELD_GOLDEN_AGE_POINTS', 3),
	('POLICY_CREATIVE_EXPRESSION', 'BUILDINGCLASS_OPERA_HOUSE', 'YIELD_CULTURE', 3),
	('POLICY_CREATIVE_EXPRESSION', 'BUILDINGCLASS_MUSEUM', 'YIELD_GOLDEN_AGE_POINTS', 3),
	('POLICY_CREATIVE_EXPRESSION', 'BUILDINGCLASS_MUSEUM', 'YIELD_CULTURE', 3),
	('POLICY_CREATIVE_EXPRESSION', 'BUILDINGCLASS_BROADCAST_TOWER', 'YIELD_GOLDEN_AGE_POINTS', 3),
	('POLICY_CREATIVE_EXPRESSION', 'BUILDINGCLASS_BROADCAST_TOWER', 'YIELD_CULTURE', 3),
	('POLICY_CREATIVE_EXPRESSION', 'BUILDINGCLASS_AMPHITHEATER', 'YIELD_GOLDEN_AGE_POINTS', 3),
	('POLICY_CREATIVE_EXPRESSION', 'BUILDINGCLASS_AMPHITHEATER', 'YIELD_CULTURE', 3);


-- Economic Union

UPDATE Policies
SET
	FreeTradeRoute = 2,
	SharedIdeologyTradeGoldChange = 6
WHERE Type = 'POLICY_ECONOMIC_UNION';


-- Media Culture

INSERT INTO Policy_BuildingClassHappiness
	(PolicyType, BuildingClassType, Happiness)
VALUES
	('POLICY_MEDIA_CULTURE', 'BUILDINGCLASS_BROADCAST_TOWER', 1);


 -- Universal Suffrage 

UPDATE Policies
SET
	HalfSpecialistUnhappiness = 0,
	ExtraHappinessPerCity = 1,
	GoldenAgeTurns = 10,
	Level = 1
WHERE Type = 'POLICY_UNIVERSAL_SUFFRAGE';


-- Urbanization (now Self-Determination)

DELETE FROM Policy_BuildingClassHappiness
WHERE PolicyType = 'POLICY_URBANIZATION';

UPDATE Policies
SET
	ExperienceAllUnitsFromLiberation = 15,
	InfluenceAllCSFromLiberation = 50,
	NumUnitsInLiberatedCities = 6,
	Level = 2
WHERE Type = 'POLICY_URBANIZATION';

INSERT INTO Policy_BuildingClassInLiberatedCities
	(PolicyType, BuildingClassType, Count)
VALUES
	('POLICY_URBANIZATION', 'BUILDINGCLASS_ARSENAL', 1);

INSERT INTO Policy_YieldForLiberation
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_URBANIZATION', 'YIELD_SCIENCE', 40);


-- Universal Healthcare

DELETE FROM Policy_BuildingClassHappiness
WHERE PolicyType = 'POLICY_UNIVERSAL_HEALTHCARE_F';

UPDATE Policies
SET
	Help = 'TXT_KEY_POLICY_UNIVERSAL_HEALTHCARE_F_HELP',
	Description = 'TXT_KEY_POLICY_UNIVERSAL_HEALTHCARE_F',
	AllCityFreeBuilding = 'BUILDINGCLASS_HOSPITAL',
	Level = 2
WHERE Type = 'POLICY_UNIVERSAL_HEALTHCARE_F';

INSERT INTO Policy_YieldFromBirth
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_UNIVERSAL_HEALTHCARE_F', 'YIELD_CULTURE', 50);


-- Volunteer Army (now Draft Registration)

DELETE FROM Policy_FreeUnitClasses
WHERE PolicyType = 'POLICY_VOLUNTEER_ARMY';

UPDATE Policies
SET
	BaseFreeUnits = 0,
	IncludesOneShotFreeUnits = 0,
	NoXPLossUnitPurchase = 1,
	Level = 1
WHERE Type = 'POLICY_VOLUNTEER_ARMY';


-- Space Procurements

INSERT INTO Policy_BuildingClassProductionModifiers
	(PolicyType, BuildingClassType, ProductionModifier)
VALUES
	('POLICY_SPACE_PROCUREMENTS', 'BUILDINGCLASS_SPACESHIP_FACTORY', 100);



----------------------
-- Combined Insertions
----------------------

-- Building Changes

INSERT INTO Policy_BuildingClassYieldModifiers
	(PolicyType, BuildingClassType, YieldType, YieldMod)
VALUES
	('POLICY_MEDIA_CULTURE', 'BUILDINGCLASS_STADIUM', 'YIELD_CULTURE', 20),
	('POLICY_SPACE_PROCUREMENTS', 'BUILDINGCLASS_LABORATORY', 'YIELD_SCIENCE', 20);
