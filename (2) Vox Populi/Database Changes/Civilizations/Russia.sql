----------------------------------------------------------
-- Unique Ability: Siberian Riches
----------------------------------------------------------
UPDATE Traits SET PlotCultureCostModifier = -33 WHERE Type = 'TRAIT_STRATEGIC_RICHES';

DELETE FROM Trait_YieldChangesStrategicResources WHERE TraitType = 'TRAIT_STRATEGIC_RICHES';

INSERT INTO Trait_ResourceQuantityModifiers
	(TraitType, ResourceType, ResourceQuantityModifier)
VALUES
--	('TRAIT_STRATEGIC_RICHES', 'RESOURCE_HORSE', 100),
--	('TRAIT_STRATEGIC_RICHES', 'RESOURCE_IRON', 100),
--	('TRAIT_STRATEGIC_RICHES', 'RESOURCE_URANIUM', 100),
	('TRAIT_STRATEGIC_RICHES', 'RESOURCE_COAL', 100),
	('TRAIT_STRATEGIC_RICHES', 'RESOURCE_OIL', 100),
	('TRAIT_STRATEGIC_RICHES', 'RESOURCE_ALUMINUM', 100);

INSERT INTO Trait_YieldFromTileEarn
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_STRATEGIC_RICHES', 'YIELD_SCIENCE', 20);

----------------------------------------------------------
-- Unique Unit: Cossack (Cavalry)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CAVALRY'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_CAVALRY') + 4,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_CAVALRY') + 6
WHERE Type = 'UNIT_RUSSIAN_COSSACK';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_RUSSIAN_COSSACK', 'PROMOTION_STRONGER_VS_DAMAGED'),
	('UNIT_RUSSIAN_COSSACK', 'PROMOTION_WITHERING_FIRE');

----------------------------------------------------------
-- Unique Unit: Licorne (Field Gun)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_RUSSIA', 'UNITCLASS_FIELD_GUN', 'UNIT_LICORNE');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_FIELD_GUN'
			)
		)
	),
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_FIELD_GUN') + 5
WHERE Type = 'UNIT_LICORNE';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_LICORNE', 'PROMOTION_CAN_MOVE_AFTER_ATTACKING'),
	('UNIT_LICORNE', 'PROMOTION_GRAPESHOT');

----------------------------------------------------------
-- Unique Building: Ostrog (Bastion Fort)
----------------------------------------------------------
UPDATE Civilization_BuildingClassOverrides
SET BuildingClassType = 'BUILDINGCLASS_BASTION_FORT'
WHERE BuildingType = 'BUILDING_KREPOST';

UPDATE Buildings
SET
	Defense = (SELECT Defense FROM Buildings WHERE Type = 'BUILDING_BASTION_FORT') + 200,
	DamageReductionFlat = (SELECT DamageReductionFlat FROM Buildings WHERE Type = 'BUILDING_BASTION_FORT') + 2,
	BorderObstacleCity = 1
WHERE Type = 'BUILDING_KREPOST';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_KREPOST', 'YIELD_PRODUCTION', 3);

CREATE TEMP TABLE Helper (
	YieldType TEXT
);

INSERT INTO Helper
VALUES
	('YIELD_PRODUCTION'),
	('YIELD_GOLD');

INSERT INTO Building_ResourceYieldChanges
	(BuildingType, ResourceType, YieldType, Yield)
SELECT
	'BUILDING_KREPOST', a.Type, b.YieldType, 2
FROM Resources a, Helper b
WHERE a.ResourceUsage = 1 AND a.type <> 'RESOURCE_PAPER';

DROP TABLE Helper;

----------------------------------------------------------
-- Unique Building: Pogost (Customs House)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_RUSSIA', 'BUILDINGCLASS_MINT', 'BUILDING_POGOST');

UPDATE Buildings
SET TradeRouteRecipientBonus = (SELECT TradeRouteRecipientBonus FROM Buildings WHERE Type = 'BUILDING_MINT') + 2
WHERE Type = 'BUILDING_POGOST';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
--	('BUILDING_POGOST', 'YIELD_CULTURE', 2),
	('BUILDING_POGOST', 'YIELD_GOLD', 2);

INSERT INTO Building_TechEnhancedYieldChanges
	(BuildingType, TechType, YieldType, Yield)
VALUES
	('BUILDING_POGOST', 'TECH_BANKING', 'YIELD_SCIENCE', 2),
	('BUILDING_POGOST', 'TECH_ECONOMICS', 'YIELD_FOOD', 2),
	('BUILDING_POGOST', 'TECH_RAILROAD', 'YIELD_PRODUCTION', 2),
	('BUILDING_POGOST', 'TECH_RAILROAD', 'YIELD_FAITH', 2),
	('BUILDING_POGOST', 'TECH_INDUSTRIALIZATION', 'YIELD_FOOD', 1),
	('BUILDING_POGOST', 'TECH_INDUSTRIALIZATION', 'YIELD_PRODUCTION', 1),
	('BUILDING_POGOST', 'TECH_INDUSTRIALIZATION', 'YIELD_GOLD', 1),
	('BUILDING_POGOST', 'TECH_INDUSTRIALIZATION', 'YIELD_SCIENCE', 1),
	('BUILDING_POGOST', 'TECH_INDUSTRIALIZATION', 'YIELD_CULTURE', 1),
	('BUILDING_POGOST', 'TECH_INDUSTRIALIZATION', 'YIELD_FAITH', 1);

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
VALUES
	('BUILDING_POGOST', 'IMPROVEMENT_TRADING_POST', 'YIELD_PRODUCTION', 1),
	('BUILDING_POGOST', 'IMPROVEMENT_TRADING_POST', 'YIELD_GOLD', 1),
	('BUILDING_POGOST', 'IMPROVEMENT_CUSTOMS_HOUSE', 'YIELD_PRODUCTION', 1),
	('BUILDING_POGOST', 'IMPROVEMENT_CUSTOMS_HOUSE', 'YIELD_GOLD', 1);
