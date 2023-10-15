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
	('TRAIT_CONVERTS_SEA_BARBARIANS', 'YIELD_GOLD', 150, 0),
	('TRAIT_CONVERTS_SEA_BARBARIANS', 'YIELD_CULTURE', 150, 0),
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
-- Unique Building: Siege Foundry (Forge)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_OTTOMAN', 'BUILDINGCLASS_FORGE', 'BUILDING_SIEGE_WORKSHOP');

UPDATE Buildings
SET TrainedFreePromotion = 'PROMOTION_VOLLEY'
WHERE Type = 'BUILDING_SIEGE_WORKSHOP';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
--	('BUILDING_SIEGE_WORKSHOP', 'YIELD_SCIENCE', 1),
	('BUILDING_SIEGE_WORKSHOP', 'YIELD_PRODUCTION', 3);

INSERT INTO Building_UnitCombatProductionModifiers
	(BuildingType, UnitCombatType, Modifier)
VALUES
	('BUILDING_SIEGE_WORKSHOP', 'UNITCOMBAT_SIEGE', 50);

INSERT INTO Building_YieldFromUnitProduction
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SIEGE_WORKSHOP', 'YIELD_SCIENCE', 20);
