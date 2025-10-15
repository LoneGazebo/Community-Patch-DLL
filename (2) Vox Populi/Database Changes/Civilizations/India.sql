----------------------------------------------------------
-- Unique Ability: Mahatma
----------------------------------------------------------
UPDATE Traits
SET
	CityUnhappinessModifier = 0,
	PopulationUnhappinessModifier = 0,
	StartsWithPantheon = 1,
	ProphetFervor = 1
WHERE Type = 'TRAIT_POPULATION_GROWTH';

INSERT INTO Trait_GreatPersonCostReduction
	(TraitType, GreatPersonType, Modifier)
VALUES
	('TRAIT_POPULATION_GROWTH', 'GREATPERSON_PROPHET', -35);

INSERT INTO Trait_NoTrain
	(TraitType, UnitClassType)
VALUES
	('TRAIT_POPULATION_GROWTH', 'UNITCLASS_MISSIONARY');

----------------------------------------------------------
-- Unique Unit: Naga-Malla (Cuirassier)
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_CUIRASSIER' WHERE UnitType = 'UNIT_INDIAN_WARELEPHANT';

UPDATE Units
SET
	PrereqTech = 'TECH_GUNPOWDER',
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CUIRASSIER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_CUIRASSIER') + 3,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_CUIRASSIER') + 8,
	Moves = 3
WHERE Type = 'UNIT_INDIAN_WARELEPHANT';

DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_INDIAN_WARELEPHANT';

-- Not really a skirmisher
DELETE FROM Unit_FreePromotions WHERE UnitType = 'UNIT_INDIAN_WARELEPHANT' AND PromotionType = 'PROMOTION_SKIRMISHER_DOCTRINE';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_INDIAN_WARELEPHANT', 'PROMOTION_ACCURACY_1'),
	('UNIT_INDIAN_WARELEPHANT', 'PROMOTION_FEARED_ELEPHANT');

----------------------------------------------------------
-- Unique Unit: Dhanurdhara (Archer)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_INDIA', 'UNITCLASS_ARCHER', 'UNIT_DHANURDHARA');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_ARCHER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_ARCHER') + 1,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_ARCHER') + 2
WHERE Type = 'UNIT_DHANURDHARA';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_DHANURDHARA', 'PROMOTION_EPIC'),
	('UNIT_DHANURDHARA', 'PROMOTION_DHANURVIDYA');

----------------------------------------------------------
-- Unique Building: Harappan Reservoir (Aqueduct)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_INDIA', 'BUILDINGCLASS_AQUEDUCT', 'BUILDING_HARAPPAN_RESERVOIR');

UPDATE Buildings
SET
	FoodBonusPerCityMajorityFollower = 1,
	FoodKept = (SELECT FoodKept FROM Buildings WHERE Type = 'BUILDING_AQUEDUCT') + 5
WHERE Type = 'BUILDING_HARAPPAN_RESERVOIR';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_AQUEDUCT' AND YieldType = 'YIELD_FOOD') + 2
WHERE BuildingType = 'BUILDING_HARAPPAN_RESERVOIR' AND YieldType = 'YIELD_FOOD';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_HARAPPAN_RESERVOIR', 'YIELD_PRODUCTION', 3);

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
VALUES
	('BUILDING_HARAPPAN_RESERVOIR', 'IMPROVEMENT_FARM', 'YIELD_FOOD', 1),
	('BUILDING_HARAPPAN_RESERVOIR', 'IMPROVEMENT_FARM', 'YIELD_PRODUCTION', 1);

----------------------------------------------------------
-- Unique Building: Qila (Castle)
----------------------------------------------------------
UPDATE Buildings
SET
	DefensePerXWonder = 2,
	DamageReductionFlat = (SELECT DamageReductionFlat FROM Buildings WHERE Type = 'BUILDING_CASTLE') + 2
WHERE Type = 'BUILDING_MUGHAL_FORT';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MUGHAL_FORT', 'YIELD_CULTURE', 2);

INSERT INTO Building_YieldChangesPerCityStrengthTimes100
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MUGHAL_FORT', 'YIELD_CULTURE', 20);
