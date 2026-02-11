----------------------------------------------------------
-- Unique Ability: Solomonic Wisdom
----------------------------------------------------------
UPDATE Traits
SET
	CombatBonusVsLargerCiv = 0,
	IsAdoptionFreeTech = 1
WHERE Type = 'TRAIT_BONUS_AGAINST_TECH';

INSERT INTO Trait_YieldChangesStrategicResources
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_BONUS_AGAINST_TECH', 'YIELD_FAITH', 1);

----------------------------------------------------------
-- Unique Unit: Chewa (Longswordsman)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_ETHIOPIA', 'UNITCLASS_LONGSWORDSMAN', 'UNIT_CHEWA');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_LONGSWORDSMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_LONGSWORDSMAN') + 2
WHERE Type = 'UNIT_CHEWA';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_CHEWA', 'PROMOTION_MAIM'),
	('UNIT_CHEWA', 'PROMOTION_HOOKED_WEAPON');

----------------------------------------------------------
-- Unique Unit: Oromo Cavalry (Cuirassier)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_ETHIOPIA', 'UNITCLASS_CUIRASSIER', 'UNIT_OROMO_CAVALRY');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CUIRASSIER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_CUIRASSIER') + 3,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_CUIRASSIER') + 4
WHERE Type = 'UNIT_OROMO_CAVALRY';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_OROMO_CAVALRY', 'PROMOTION_ZEMENE_MESAFINT');

----------------------------------------------------------
-- Unique Improvement: Monolithic Church
----------------------------------------------------------
UPDATE Builds
SET
	PrereqTech = 'TECH_THEOLOGY',
	Time = 900
WHERE Type = 'BUILD_MONOLITHIC_CHURCH';

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_WORKER', 'BUILD_MONOLITHIC_CHURCH');

INSERT INTO BuildFeatures
	(BuildType, FeatureType, Time, Remove)
VALUES
	('BUILD_MONOLITHIC_CHURCH', 'FEATURE_FOREST', 400, 1),
	('BUILD_MONOLITHIC_CHURCH', 'FEATURE_JUNGLE', 500, 1),
	('BUILD_MONOLITHIC_CHURCH', 'FEATURE_MARSH', 500, 1);

UPDATE Improvements
SET
	HillsMakesValid = 1,
	NoTwoAdjacent = 1
WHERE Type = 'IMPROVEMENT_MONOLITHIC_CHURCH';

INSERT INTO Improvement_ResourceTypes
	(ImprovementType, ResourceType)
VALUES
	('IMPROVEMENT_MONOLITHIC_CHURCH', 'RESOURCE_STONE');

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_MONOLITHIC_CHURCH', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_MONOLITHIC_CHURCH', 'YIELD_FAITH', 1);

INSERT INTO Improvement_AccomplishmentYieldChanges
	(ImprovementType, AccomplishmentType, YieldType, Yield)
VALUES
	('IMPROVEMENT_MONOLITHIC_CHURCH', 'ACCOMPLISHMENT_POLICY_BRANCH_ANCIENT', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_MONOLITHIC_CHURCH', 'ACCOMPLISHMENT_POLICY_BRANCH_MEDIEVAL', 'YIELD_FAITH', 1),
	('IMPROVEMENT_MONOLITHIC_CHURCH', 'ACCOMPLISHMENT_POLICY_BRANCH_INDUSTRIAL', 'YIELD_SCIENCE', 1),
	('IMPROVEMENT_MONOLITHIC_CHURCH', 'ACCOMPLISHMENT_CHOOSE_IDEOLOGY', 'YIELD_GOLD', 2),
	('IMPROVEMENT_MONOLITHIC_CHURCH', 'ACCOMPLISHMENT_BELIEF_PANTHEON', 'YIELD_FAITH', 1),
	('IMPROVEMENT_MONOLITHIC_CHURCH', 'ACCOMPLISHMENT_BELIEF_FOUNDER', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_MONOLITHIC_CHURCH', 'ACCOMPLISHMENT_BELIEF_FOLLOWER', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_MONOLITHIC_CHURCH', 'ACCOMPLISHMENT_BELIEF_ENHANCER', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_MONOLITHIC_CHURCH', 'ACCOMPLISHMENT_BELIEF_REFORMATION', 'YIELD_GOLD', 1);

----------------------------------------------------------
-- Unique Building: Stele (Monument)
----------------------------------------------------------
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_STELE', 'YIELD_FAITH', 2);

UPDATE Buildings
SET Help = 'TXT_KEY_BUILDING_STELE_HELP'
WHERE Type = 'BUILDING_STELE';

INSERT INTO Building_GoldenAgeYieldMod
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_STELE', 'YIELD_FAITH', 25);
