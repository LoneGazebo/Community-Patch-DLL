----------------------------------------------------------
-- Unique Ability: Mare Clausum
----------------------------------------------------------
UPDATE Traits
SET TradeRouteResourceModifier = 0
WHERE Type = 'TRAIT_EXTRA_TRADE';

INSERT INTO Trait_YieldFromRouteMovement
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_EXTRA_TRADE', 'YIELD_GOLD', 4),
	('TRAIT_EXTRA_TRADE', 'YIELD_SCIENCE', 4),
	('TRAIT_EXTRA_TRADE', 'YIELD_GREAT_GENERAL_POINTS', 4),
	('TRAIT_EXTRA_TRADE', 'YIELD_GREAT_ADMIRAL_POINTS', 4);

----------------------------------------------------------
-- Unique Unit: Nau (Caravel)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CARAVEL'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_CARAVEL') + 5,
	BaseSightRange = (SELECT BaseSightRange FROM Units WHERE Type = 'UNIT_CARAVEL') + 1,
	NumExoticGoods = 2
WHERE Type = 'UNIT_PORTUGUESE_NAU';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_PORTUGUESE_NAU', 'PROMOTION_SELL_EXOTIC_GOODS'),
	('UNIT_PORTUGUESE_NAU', 'PROMOTION_WITHDRAW_BEFORE_MELEE');

----------------------------------------------------------
-- Unique Unit: Caçador (Gatling Gun)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_PORTUGAL', 'UNITCLASS_GATLINGGUN', 'UNIT_CACADOR');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_GATLINGGUN'
			)
		)
	)
WHERE Type = 'UNIT_CACADOR';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_CACADOR', 'PROMOTION_SCOUTING_2'),
	('UNIT_CACADOR', 'PROMOTION_FIGHTING_COCK');

----------------------------------------------------------
-- Unique Improvement: Feitoria
----------------------------------------------------------
UPDATE Builds
SET PrereqTech = 'TECH_COMPASS'
WHERE Type = 'BUILD_FEITORIA';

UPDATE Improvements
SET
	OnlyCityStateTerritory = 0,
	NoTwoAdjacent = 1,
	DefenseModifier = 25,
	GrantsVisionXTiles = 2,
	MakesPassable = 1
WHERE Type = 'IMPROVEMENT_FEITORIA';

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_FEITORIA', 'YIELD_PRODUCTION', 3),
	('IMPROVEMENT_FEITORIA', 'YIELD_GOLD', 4);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_FEITORIA', 'TECH_PRINTING_PRESS', 'YIELD_SCIENCE', 2),
	('IMPROVEMENT_FEITORIA', 'TECH_ASTRONOMY', 'YIELD_GOLD', 2),
	('IMPROVEMENT_FEITORIA', 'TECH_INDUSTRIALIZATION', 'YIELD_PRODUCTION', 2);

----------------------------------------------------------
-- Unique Building: University of Coimbra (Imperial College)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_PORTUGAL', 'BUILDINGCLASS_OXFORD_UNIVERSITY', 'BUILDING_UNIVERSITY_OF_COIMBRA');

UPDATE Buildings
SET
	FreeBuildingThisCity = 'BUILDINGCLASS_UNIVERSITY',
	FreePromotion = 'PROMOTION_AGE_OF_DISCOVERY'
WHERE Type = 'BUILDING_UNIVERSITY_OF_COIMBRA';

DELETE FROM Building_ClassesNeededInCity WHERE BuildingType = 'BUILDING_UNIVERSITY_OF_COIMBRA';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges a WHERE a.BuildingType = 'BUILDING_OXFORD_UNIVERSITY' AND a.YieldType = Building_YieldChanges.YieldType) + 2
WHERE BuildingType = 'BUILDING_UNIVERSITY_OF_COIMBRA';

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_UNIVERSITY_OF_COIMBRA', 'BUILDINGCLASS_UNIVERSITY', 'YIELD_GOLD', 3);

INSERT INTO Building_UnitCombatProductionModifiersGlobal
	(BuildingType, UnitCombatType, Modifier)
VALUES
	('BUILDING_UNIVERSITY_OF_COIMBRA', 'UNITCOMBAT_CARAVAN', 200),
	('BUILDING_UNIVERSITY_OF_COIMBRA', 'UNITCOMBAT_CARGO', 200);
