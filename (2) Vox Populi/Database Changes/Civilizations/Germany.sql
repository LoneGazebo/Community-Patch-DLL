----------------------------------------------------------
-- Unique Ability: Blood and Iron
----------------------------------------------------------
UPDATE Traits
SET
	LandBarbarianConversionPercent = 0,
	LandUnitMaintenanceModifier = 0,
	MinorInfluencePerGiftedUnit = 1
WHERE Type = 'TRAIT_CONVERTS_LAND_BARBARIANS';

INSERT INTO Trait_UnitCombatProductionCostModifiers
	(TraitType, UnitCombatType, CostModifier)
SELECT
	'TRAIT_CONVERTS_LAND_BARBARIANS', Type, -15
FROM UnitCombatInfos
WHERE IsMilitary = 1;

----------------------------------------------------------
-- Unique Unit: Landsknecht (Tercio)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_GERMANY', 'UNITCLASS_TERCIO', 'UNIT_GERMAN_LANDSKNECHT');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_SPANISH_TERCIO'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_SPANISH_TERCIO') + 2,
	MoveAfterPurchase = 1
WHERE Type = 'UNIT_GERMAN_LANDSKNECHT';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_GERMAN_LANDSKNECHT', 'PROMOTION_FORMATION_2'),
	('UNIT_GERMAN_LANDSKNECHT', 'PROMOTION_DOPPELSOLDNER');

----------------------------------------------------------
-- Unique Unit: Krupp Gun (Artillery)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_GERMANY', 'UNITCLASS_ARTILLERY', 'UNIT_KRUPP_GUN');

UPDATE Units
SET
	PrereqTech = 'TECH_REPLACEABLE_PARTS',
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_ARTILLERY'
			)
		)
	),
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_ARTILLERY') + 5
WHERE Type = 'UNIT_KRUPP_GUN';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_KRUPP_GUN', 'PROMOTION_MINENWERFER'),
	('UNIT_KRUPP_GUN', 'PROMOTION_TROMMELFEUER');

----------------------------------------------------------
-- Unique Building: Hanse (Customs House)
----------------------------------------------------------
UPDATE Civilization_BuildingClassOverrides
SET BuildingClassType = 'BUILDINGCLASS_MINT'
WHERE BuildingType = 'BUILDING_HANSE';

UPDATE Buildings
SET
	TradeRouteRecipientBonus = (SELECT TradeRouteRecipientBonus FROM Buildings WHERE Type = 'BUILDING_MINT') + 1,
	TradeRouteTargetBonus = (SELECT TradeRouteTargetBonus FROM Buildings WHERE Type = 'BUILDING_MINT') + 1,
	CityStateTradeRouteProductionModifier = 4
WHERE Type = 'BUILDING_HANSE';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_MINT' AND YieldType = 'YIELD_CULTURE') + 1
WHERE BuildingType = 'BUILDING_HANSE' AND YieldType = 'YIELD_CULTURE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_HANSE', 'YIELD_GOLD', 5);

INSERT INTO Building_YieldFromYieldPercent
	(BuildingType, YieldIn, YieldOut, Value)
VALUES
	('BUILDING_HANSE', 'YIELD_GOLD', 'YIELD_SCIENCE', 10);

----------------------------------------------------------
-- Unique Building: Brewhouse (Windmill)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_GERMANY', 'BUILDINGCLASS_WINDMILL', 'BUILDING_BREWHOUSE');

UPDATE Buildings
SET HappinessPerXPolicies = 9
WHERE Type = 'BUILDING_BREWHOUSE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
--	('BUILDING_BREWHOUSE', 'YIELD_PRODUCTION', 3),
	('BUILDING_BREWHOUSE', 'YIELD_CULTURE', 3);

INSERT INTO Building_ResourceYieldChanges
	(BuildingType, ResourceType, YieldType, Yield)
VALUES
	('BUILDING_BREWHOUSE', 'RESOURCE_WHEAT', 'YIELD_FOOD', 1),
	('BUILDING_BREWHOUSE', 'RESOURCE_WHEAT', 'YIELD_CULTURE', 1),
	('BUILDING_BREWHOUSE', 'RESOURCE_RICE', 'YIELD_FOOD', 1),
	('BUILDING_BREWHOUSE', 'RESOURCE_RICE', 'YIELD_CULTURE', 1),
	('BUILDING_BREWHOUSE', 'RESOURCE_MAIZE', 'YIELD_FOOD', 1),
	('BUILDING_BREWHOUSE', 'RESOURCE_MAIZE', 'YIELD_CULTURE', 1);

INSERT INTO Building_BuildingClassLocalYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
--	('BUILDING_BREWHOUSE', 'BUILDINGCLASS_GRANARY', 'YIELD_FOOD', 1),
	('BUILDING_BREWHOUSE', 'BUILDINGCLASS_GRANARY', 'YIELD_CULTURE', 1),
--	('BUILDING_BREWHOUSE', 'BUILDINGCLASS_GROCER', 'YIELD_FOOD', 1),
	('BUILDING_BREWHOUSE', 'BUILDINGCLASS_GROCER', 'YIELD_CULTURE', 1);

INSERT INTO Building_YieldFromUnitGiftGlobal
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BREWHOUSE', 'YIELD_CULTURE', 10);

INSERT INTO Building_YieldFromPolicyUnlock
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BREWHOUSE', 'YIELD_GOLD', 10);
