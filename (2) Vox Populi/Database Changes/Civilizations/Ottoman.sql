----------------------------------------------------------
-- Unique Ability: Kanuni
----------------------------------------------------------
UPDATE Traits
SET NavalUnitMaintenanceModifier = 0
WHERE Type = 'TRAIT_CONVERTS_SEA_BARBARIANS';

DELETE FROM Trait_FreePromotionUnitCombats
WHERE TraitType = 'TRAIT_CONVERTS_SEA_BARBARIANS';

INSERT INTO Trait_TradeRouteEndYield
	(TraitType, YieldType, YieldDomestic, YieldInternational)
VALUES
	('TRAIT_CONVERTS_SEA_BARBARIANS', 'YIELD_GOLD', 75, 0),
	('TRAIT_CONVERTS_SEA_BARBARIANS', 'YIELD_CULTURE', 75, 0),
	('TRAIT_CONVERTS_SEA_BARBARIANS', 'YIELD_FOOD', 0, 150),
	('TRAIT_CONVERTS_SEA_BARBARIANS', 'YIELD_SCIENCE', 0, 150);

----------------------------------------------------------
-- Unique Unit: Janissary (Musketman)
----------------------------------------------------------
UPDATE Units
SET
	PrereqTech = 'TECH_GUNPOWDER',
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_MUSKETMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_MUSKETMAN') + 2,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_MUSKETMAN') + 1
WHERE Type = 'UNIT_OTTOMAN_JANISSARY';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_OTTOMAN_JANISSARY', 'PROMOTION_MARCH'),
	('UNIT_OTTOMAN_JANISSARY', 'PROMOTION_ATTACK_BONUS');

----------------------------------------------------------
-- Unique Unit: Great Bombard
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_OTTOMAN', 'UNITCLASS_GREAT_BOMBARD', 'UNIT_GREAT_BOMBARD');

UPDATE Units
SET
	Combat = 11,
	RangedCombat = 43,
	"Range" = 2
WHERE Type = 'UNIT_GREAT_BOMBARD';

INSERT INTO Unit_ClassUpgrades
	(UnitType, UnitClassType)
VALUES
	('UNIT_GREAT_BOMBARD', 'UNITCLASS_FIELD_GUN');

DELETE FROM Unit_FreePromotions WHERE UnitType = 'UNIT_GREAT_BOMBARD' AND PromotionType IN ('PROMOTION_CITY_SIEGE', 'PROMOTION_SIEGE_INACCURACY');

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_GREAT_BOMBARD', 'PROMOTION_CITY_ASSAULT'),
	('UNIT_GREAT_BOMBARD', 'PROMOTION_UNWIELDY');

INSERT INTO Unit_AITypes
	(UnitType, UnitAIType)
VALUES
	('UNIT_GREAT_BOMBARD', 'UNITAI_CITY_BOMBARD'),
	('UNIT_GREAT_BOMBARD', 'UNITAI_RANGED');

INSERT INTO Unit_UniqueNames
	(UnitType, UniqueName)
VALUES
	('UNIT_GREAT_BOMBARD', 'TXT_KEY_UNIT_GREAT_BOMBARD_1'),
	('UNIT_GREAT_BOMBARD', 'TXT_KEY_UNIT_GREAT_BOMBARD_2'),
	('UNIT_GREAT_BOMBARD', 'TXT_KEY_UNIT_GREAT_BOMBARD_3'),
	('UNIT_GREAT_BOMBARD', 'TXT_KEY_UNIT_GREAT_BOMBARD_4'),
	('UNIT_GREAT_BOMBARD', 'TXT_KEY_UNIT_GREAT_BOMBARD_5');

----------------------------------------------------------
-- Unique Improvement: Tersane
----------------------------------------------------------
UPDATE Builds
SET
	Water = 1,
	CanBeEmbarked = 1
WHERE Type = 'BUILD_TERSANE';

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_GREAT_GENERAL', 'BUILD_TERSANE'),
	('UNIT_GREAT_ADMIRAL', 'BUILD_TERSANE');

UPDATE Improvements
SET
	Water = 1,
	Cityside = 1, -- makes valid
	BuildableOnResources = 1,
	ConnectsAllResources = 1,
	NoTwoAdjacent = 1,
	CultureBombRadius = 1,
	DefenseModifier = 50,
	NoFollowUp = 1,
	NearbyEnemyDamage = 30
WHERE Type = 'IMPROVEMENT_TERSANE';

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_TERSANE', 'YIELD_PRODUCTION', 2),
	('IMPROVEMENT_TERSANE', 'YIELD_GOLD', 3),
	('IMPROVEMENT_TERSANE', 'YIELD_SCIENCE', 2);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_TERSANE', 'TECH_CHEMISTRY', 'YIELD_SCIENCE', 2),
	('IMPROVEMENT_TERSANE', 'TECH_MILITARY_SCIENCE', 'YIELD_PRODUCTION', 2),
	('IMPROVEMENT_TERSANE', 'TECH_MILITARY_SCIENCE', 'YIELD_GOLD', 2),
	('IMPROVEMENT_TERSANE', 'TECH_STEALTH', 'YIELD_SCIENCE', 4),
	('IMPROVEMENT_TERSANE', 'TECH_ELECTRONICS', 'YIELD_PRODUCTION', 4),
	('IMPROVEMENT_TERSANE', 'TECH_ELECTRONICS', 'YIELD_GOLD', 4);

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_NEW_DEAL', 'IMPROVEMENT_TERSANE', 'YIELD_PRODUCTION', 6);

INSERT INTO Improvement_DomainProductionModifier
	(ImprovementType, DomainType, Modifier)
VALUES
	('IMPROVEMENT_TERSANE', 'DOMAIN_SEA', 25);

INSERT INTO Improvement_DomainFreeExperience
	(ImprovementType, DomainType, Experience)
VALUES
	('IMPROVEMENT_TERSANE', 'DOMAIN_SEA', 10);

----------------------------------------------------------
-- Unique Building: Siege Foundry (Forge)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_OTTOMAN', 'BUILDINGCLASS_FORGE', 'BUILDING_SIEGE_FOUNDRY');

UPDATE Buildings
SET TrainedFreePromotion = 'PROMOTION_VOLLEY'
WHERE Type = 'BUILDING_SIEGE_FOUNDRY';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
--	('BUILDING_SIEGE_FOUNDRY', 'YIELD_SCIENCE', 1),
	('BUILDING_SIEGE_FOUNDRY', 'YIELD_PRODUCTION', 3);

INSERT INTO Building_UnitCombatProductionModifiers
	(BuildingType, UnitCombatType, Modifier)
VALUES
	('BUILDING_SIEGE_FOUNDRY', 'UNITCOMBAT_SIEGE', 50);

INSERT INTO Building_YieldFromUnitProduction
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SIEGE_FOUNDRY', 'YIELD_SCIENCE', 20);
