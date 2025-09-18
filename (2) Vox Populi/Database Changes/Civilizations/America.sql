----------------------------------------------------------
-- Unique Ability: Manifest Destiny
----------------------------------------------------------
UPDATE Traits
SET
	PlotBuyCostModifier = -25,
	BuyOwnedTiles = 1
WHERE Type = 'TRAIT_RIVER_EXPANSION';

INSERT INTO Trait_YieldFromTilePurchase
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_RIVER_EXPANSION', 'YIELD_PRODUCTION', 20);

----------------------------------------------------------
-- Unique Unit: Minuteman (Musketman)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_MUSKETMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_MUSKETMAN') + 1,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_MUSKETMAN') + 1
WHERE Type = 'UNIT_AMERICAN_MINUTEMAN';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_AMERICAN_MINUTEMAN', 'PROMOTION_BARRAGE_1'),
	('UNIT_AMERICAN_MINUTEMAN', 'PROMOTION_IGNORE_TERRAIN_COST'),
	('UNIT_AMERICAN_MINUTEMAN', 'PROMOTION_GOLDEN_AGE_POINTS');

INSERT INTO Unit_AITypes
	(UnitType, UnitAIType)
VALUES
	('UNIT_AMERICAN_MINUTEMAN', 'UNITAI_EXPLORE');

----------------------------------------------------------
-- Unique Unit: Monitor (Ironclad)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_AMERICA', 'UNITCLASS_IRONCLAD', 'UNIT_MONITOR');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_IRONCLAD'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_IRONCLAD') + 6
WHERE Type = 'UNIT_MONITOR';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_MONITOR', 'PROMOTION_HARDENED'),
	('UNIT_MONITOR', 'PROMOTION_HARBOR_DEFENSE');

----------------------------------------------------------
-- Unique Building: Independence Hall (Royal Collection)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_AMERICA', 'BUILDINGCLASS_HERMITAGE', 'BUILDING_INDEPENDENCE_HALL');

UPDATE Buildings
SET
	PolicyCostModifier = -5,
	GreatWorkCount = 2
WHERE Type = 'BUILDING_INDEPENDENCE_HALL';

INSERT INTO Building_ThemingBonuses
	(BuildingType, Description, RequiresOwner, MustBeArt)
VALUES
	('BUILDING_INDEPENDENCE_HALL', 'TXT_KEY_THEMING_BONUS_INDEPENDENCEHALL', 1, 1);

----------------------------------------------------------
-- Extra Buildings:
-- Smithsonian Institution
-- West Point
-- Slater Mill
----------------------------------------------------------
INSERT INTO Building_ClassNeededAnywhere
	(BuildingType, BuildingClassType)
VALUES
	('BUILDING_SMITHSONIAN_INSTITUTION', 'BUILDINGCLASS_HERMITAGE'),
	('BUILDING_WEST_POINT', 'BUILDINGCLASS_HERMITAGE'),
	('BUILDING_SLATER_MILL', 'BUILDINGCLASS_HERMITAGE');

INSERT INTO Building_ClassNeededNowhere
	(BuildingType, BuildingClassType)
VALUES
	('BUILDING_SMITHSONIAN_INSTITUTION', 'BUILDINGCLASS_WEST_POINT'),
	('BUILDING_SMITHSONIAN_INSTITUTION', 'BUILDINGCLASS_SLATER_MILL'),
	('BUILDING_WEST_POINT', 'BUILDINGCLASS_SMITHSONIAN_INSTITUTION'),
	('BUILDING_WEST_POINT', 'BUILDINGCLASS_SLATER_MILL'),
	('BUILDING_SLATER_MILL', 'BUILDINGCLASS_SMITHSONIAN_INSTITUTION'),
	('BUILDING_SLATER_MILL', 'BUILDINGCLASS_WEST_POINT');

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SMITHSONIAN_INSTITUTION', 'YIELD_SCIENCE', 2),
	('BUILDING_SMITHSONIAN_INSTITUTION', 'YIELD_CULTURE', 3),
	('BUILDING_WEST_POINT', 'YIELD_PRODUCTION', 2),
	('BUILDING_WEST_POINT', 'YIELD_SCIENCE', 2),
	('BUILDING_WEST_POINT', 'YIELD_GREAT_GENERAL_POINTS', 5),
	('BUILDING_WEST_POINT', 'YIELD_CULTURE', 1),
	('BUILDING_SLATER_MILL', 'YIELD_PRODUCTION', 2),
	('BUILDING_SLATER_MILL', 'YIELD_GOLD', 2),
	('BUILDING_SLATER_MILL', 'YIELD_CULTURE', 1);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_SMITHSONIAN_INSTITUTION', 'BUILDINGCLASS_MUSEUM', 'YIELD_SCIENCE', 5),
	('BUILDING_SMITHSONIAN_INSTITUTION', 'BUILDINGCLASS_MUSEUM', 'YIELD_CULTURE', 5),
	('BUILDING_SMITHSONIAN_INSTITUTION', 'BUILDINGCLASS_LABORATORY', 'YIELD_SCIENCE', 5),
	('BUILDING_SMITHSONIAN_INSTITUTION', 'BUILDINGCLASS_LABORATORY', 'YIELD_CULTURE', 5),
	('BUILDING_WEST_POINT', 'BUILDINGCLASS_MILITARY_ACADEMY', 'YIELD_PRODUCTION', 5),
	('BUILDING_WEST_POINT', 'BUILDINGCLASS_MILITARY_ACADEMY', 'YIELD_SCIENCE', 5),
	('BUILDING_WEST_POINT', 'BUILDINGCLASS_ARSENAL', 'YIELD_PRODUCTION', 5),
	('BUILDING_WEST_POINT', 'BUILDINGCLASS_ARSENAL', 'YIELD_SCIENCE', 5),
	('BUILDING_SLATER_MILL', 'BUILDINGCLASS_FACTORY', 'YIELD_PRODUCTION', 5),
	('BUILDING_SLATER_MILL', 'BUILDINGCLASS_FACTORY', 'YIELD_GOLD', 5),
	('BUILDING_SLATER_MILL', 'BUILDINGCLASS_STOCK_EXCHANGE', 'YIELD_PRODUCTION', 5),
	('BUILDING_SLATER_MILL', 'BUILDINGCLASS_STOCK_EXCHANGE', 'YIELD_GOLD', 5);

INSERT INTO Building_YieldChangeWorldWonderGlobal
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SMITHSONIAN_INSTITUTION', 'YIELD_SCIENCE', 2),
	('BUILDING_SMITHSONIAN_INSTITUTION', 'YIELD_CULTURE', 2),
	('BUILDING_WEST_POINT', 'YIELD_PRODUCTION', 2),
	('BUILDING_WEST_POINT', 'YIELD_SCIENCE', 2),
	('BUILDING_SLATER_MILL', 'YIELD_PRODUCTION', 2),
	('BUILDING_SLATER_MILL', 'YIELD_GOLD', 2);

INSERT INTO Building_YieldFromYieldPercentGlobal
	(BuildingType, YieldIn, YieldOut, Value)
VALUES
	('BUILDING_SMITHSONIAN_INSTITUTION', 'YIELD_GOLD', 'YIELD_CULTURE', 10),
	('BUILDING_WEST_POINT', 'YIELD_GOLD', 'YIELD_SCIENCE', 10),
	('BUILDING_SLATER_MILL', 'YIELD_GOLD', 'YIELD_PRODUCTION', 10);

INSERT INTO Building_ImprovementYieldChangesGlobal
	(BuildingType, ImprovementType, YieldType, Yield)
VALUES
	('BUILDING_SMITHSONIAN_INSTITUTION', 'IMPROVEMENT_LANDMARK', 'YIELD_SCIENCE', 5);

INSERT INTO Building_GreatWorkYieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SMITHSONIAN_INSTITUTION', 'YIELD_SCIENCE', 1);

----------------------------------------------------------
-- Unique Building: Homestead (Stable)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_AMERICA', 'BUILDINGCLASS_STABLE', 'BUILDING_HOMESTEAD');

DELETE FROM Building_LocalResourceOrs WHERE BuildingType = 'BUILDING_HOMESTEAD';

INSERT INTO Building_ResourceYieldChanges
	(BuildingType, ResourceType, YieldType, Yield)
VALUES
	('BUILDING_HOMESTEAD', 'RESOURCE_BISON', 'YIELD_FOOD', 2),
	('BUILDING_HOMESTEAD', 'RESOURCE_BISON', 'YIELD_PRODUCTION', 2),
	('BUILDING_HOMESTEAD', 'RESOURCE_BISON', 'YIELD_GOLD', 2),
	('BUILDING_HOMESTEAD', 'RESOURCE_COW', 'YIELD_FOOD', 2),
	('BUILDING_HOMESTEAD', 'RESOURCE_COW', 'YIELD_GOLD', 2),
	('BUILDING_HOMESTEAD', 'RESOURCE_HORSE', 'YIELD_FOOD', 2),
	('BUILDING_HOMESTEAD', 'RESOURCE_HORSE', 'YIELD_GOLD', 2),
	('BUILDING_HOMESTEAD', 'RESOURCE_SHEEP', 'YIELD_FOOD', 2),
	('BUILDING_HOMESTEAD', 'RESOURCE_SHEEP', 'YIELD_GOLD', 2);

INSERT INTO Building_UnitCombatProductionModifiers
	(BuildingType, UnitCombatType, Modifier)
VALUES
	('BUILDING_HOMESTEAD', 'UNITCOMBAT_WORKER', 25),
	('BUILDING_HOMESTEAD', 'UNITCOMBAT_SETTLER', 25);

INSERT INTO Building_UnitClassTrainingAllowed
	(BuildingType, UnitClassType)
VALUES
	('BUILDING_HOMESTEAD', 'UNITCLASS_PIONEER');

INSERT INTO Building_ResourceClaim
	(BuildingType, ResourceType, IncludeOwnedByOtherPlayer)
VALUES
	('BUILDING_HOMESTEAD', 'RESOURCE_BISON', 1),
	('BUILDING_HOMESTEAD', 'RESOURCE_COW', 1),
	('BUILDING_HOMESTEAD', 'RESOURCE_HORSE', 1),
	('BUILDING_HOMESTEAD', 'RESOURCE_SHEEP', 1);
