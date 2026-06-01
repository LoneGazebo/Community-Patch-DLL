-- NOTE: THIS FILE MUST BE LAST IN THE LOAD ORDER FOR (2) Vox Populi
-- For better data control, VP doesn't use triggers for itself.
-- These triggers only aim to save work for modmodders.

---------------------------------------------------------------------------------
-- New units have same building requirements for purchase as base unit
---------------------------------------------------------------------------------
CREATE TRIGGER VP_Unit_BuildingClassPurchaseRequireds
AFTER INSERT ON Units
BEGIN
	INSERT INTO Unit_BuildingClassPurchaseRequireds
		(UnitType, BuildingClassType)
	SELECT
		NEW.Type, BuildingClassType
	FROM Unit_BuildingClassPurchaseRequireds
	WHERE UnitType IN (SELECT DefaultUnit FROM UnitClasses WHERE Type = NEW.Class);
END;

---------------------------------------------------------------------------------
-- Polynesia: New land melee units build Fishing Boats
---------------------------------------------------------------------------------

-- If a unit is added with a combat class of melee/gunpowder, add it
CREATE TRIGGER VP_PolynesiaCompatibility_Build_Insert
AFTER INSERT ON Units
WHEN (NEW.CombatClass IN ('UNITCOMBAT_MELEE', 'UNITCOMBAT_GUN'))
BEGIN
	INSERT INTO Trait_BuildsUnitClasses
		(TraitType, UnitClassType, BuildType)
	SELECT
		'TRAIT_WAYFINDING', NEW.Class, 'BUILD_FISHING_BOATS_EMBARKED'
	WHERE NOT EXISTS (
		SELECT 1 FROM Trait_BuildsUnitClasses
		WHERE TraitType = 'TRAIT_WAYFINDING' AND UnitClassType = NEW.Class AND BuildType = 'BUILD_FISHING_BOATS_EMBARKED'
	);
END;

-- If a unit's class or combat class changes, and its new combat class is melee/gunpowder, add it
CREATE TRIGGER VP_PolynesiaCompatibility_Build_UpdateTo
AFTER UPDATE OF Class, CombatClass ON Units
WHEN (NEW.CombatClass IN ('UNITCOMBAT_MELEE', 'UNITCOMBAT_GUN') AND OLD.CombatClass NOT IN ('UNITCOMBAT_MELEE', 'UNITCOMBAT_GUN'))
BEGIN
	INSERT INTO Trait_BuildsUnitClasses
		(TraitType, UnitClassType, BuildType)
	SELECT
		'TRAIT_WAYFINDING', NEW.Class, 'BUILD_FISHING_BOATS_EMBARKED'
	WHERE NOT EXISTS (
		SELECT 1 FROM Trait_BuildsUnitClasses
		WHERE TraitType = 'TRAIT_WAYFINDING' AND UnitClassType = NEW.Class AND BuildType = 'BUILD_FISHING_BOATS_EMBARKED'
	);
END;

-- If there are no units of melee/gunpowder combat class left in a given class, remove it
CREATE TRIGGER VP_PolynesiaCompatibility_Build_UpdateAway
AFTER UPDATE OF Class, CombatClass ON Units
WHEN (OLD.CombatClass IN ('UNITCOMBAT_MELEE', 'UNITCOMBAT_GUN') AND NEW.CombatClass NOT IN ('UNITCOMBAT_MELEE', 'UNITCOMBAT_GUN'))
BEGIN
	DELETE FROM Trait_BuildsUnitClasses
	WHERE TraitType = 'TRAIT_WAYFINDING' AND UnitClassType = OLD.Class AND BuildType = 'BUILD_FISHING_BOATS_EMBARKED'
	AND NOT EXISTS (
		SELECT 1 FROM Units WHERE Class = OLD.Class AND CombatClass IN ('UNITCOMBAT_MELEE', 'UNITCOMBAT_GUN')
	);
END;

CREATE TRIGGER VP_PolynesiaCompatibility_Build_Delete
AFTER DELETE ON Units
WHEN (OLD.CombatClass IN ('UNITCOMBAT_MELEE', 'UNITCOMBAT_GUN'))
BEGIN
	DELETE FROM Trait_BuildsUnitClasses
	WHERE TraitType = 'TRAIT_WAYFINDING' AND UnitClassType = OLD.Class AND BuildType = 'BUILD_FISHING_BOATS_EMBARKED'
	AND NOT EXISTS (
		SELECT 1 FROM Units WHERE Class = OLD.Class AND CombatClass IN ('UNITCOMBAT_MELEE', 'UNITCOMBAT_GUN')
	);
END;

---------------------------------------------------------------------------------
-- Spain: New resources boost adjacent Hacienda
---------------------------------------------------------------------------------
CREATE TRIGGER VP_HaciendaCompatibility_Bonus_Insert
AFTER INSERT ON Resources
WHEN (NEW.ResourceClassType = 'RESOURCECLASS_BONUS')
BEGIN
	INSERT INTO Improvement_AdjacentResourceYieldChanges
		(ImprovementType, ResourceType, YieldType, Yield)
	SELECT
		'IMPROVEMENT_HACIENDA', NEW.Type, 'YIELD_FOOD', 3;
END;

CREATE TRIGGER VP_HaciendaCompatibility_Strategic_Insert
AFTER INSERT ON Resources
WHEN (NEW.ResourceClassType IN ('RESOURCECLASS_RUSH', 'RESOURCECLASS_MODERN'))
BEGIN
	INSERT INTO Improvement_AdjacentResourceYieldChanges
		(ImprovementType, ResourceType, YieldType, Yield)
	SELECT
		'IMPROVEMENT_HACIENDA', NEW.Type, 'YIELD_PRODUCTION', 3;
END;

CREATE TRIGGER VP_HaciendaCompatibility_Luxury_Insert
AFTER INSERT ON Resources
WHEN (NEW.ResourceClassType = 'RESOURCECLASS_LUXURY')
BEGIN
	INSERT INTO Improvement_AdjacentResourceYieldChanges
		(ImprovementType, ResourceType, YieldType, Yield)
	SELECT
		'IMPROVEMENT_HACIENDA', NEW.Type, 'YIELD_GOLD', 3;
END;

CREATE TRIGGER VP_HaciendaCompatibility_Bonus_Update
AFTER UPDATE OF ResourceClassType ON Resources
WHEN (NEW.ResourceClassType = 'RESOURCECLASS_BONUS')
BEGIN
	DELETE FROM Improvement_AdjacentResourceYieldChanges
	WHERE ImprovementType = 'IMPROVEMENT_HACIENDA' AND ResourceType = OLD.Type;

	INSERT INTO Improvement_AdjacentResourceYieldChanges
		(ImprovementType, ResourceType, YieldType, Yield)
	SELECT
		'IMPROVEMENT_HACIENDA', NEW.Type, 'YIELD_FOOD', 3;
END;

CREATE TRIGGER VP_HaciendaCompatibility_Strategic_Update
AFTER UPDATE OF ResourceClassType ON Resources
WHEN (NEW.ResourceClassType IN ('RESOURCECLASS_RUSH', 'RESOURCECLASS_MODERN'))
BEGIN
	DELETE FROM Improvement_AdjacentResourceYieldChanges
	WHERE ImprovementType = 'IMPROVEMENT_HACIENDA' AND ResourceType = OLD.Type;

	INSERT INTO Improvement_AdjacentResourceYieldChanges
		(ImprovementType, ResourceType, YieldType, Yield)
	SELECT
		'IMPROVEMENT_HACIENDA', NEW.Type, 'YIELD_PRODUCTION', 3;
END;

CREATE TRIGGER VP_HaciendaCompatibility_Luxury_Update
AFTER UPDATE OF ResourceClassType ON Resources
WHEN (NEW.ResourceClassType = 'RESOURCECLASS_LUXURY')
BEGIN
	DELETE FROM Improvement_AdjacentResourceYieldChanges
	WHERE ImprovementType = 'IMPROVEMENT_HACIENDA' AND ResourceType = OLD.Type;

	INSERT INTO Improvement_AdjacentResourceYieldChanges
		(ImprovementType, ResourceType, YieldType, Yield)
	SELECT
		'IMPROVEMENT_HACIENDA', NEW.Type, 'YIELD_GOLD', 3;
END;

CREATE TRIGGER VP_HaciendaCompatibility_Delete
AFTER DELETE ON Resources
BEGIN
	DELETE FROM Improvement_AdjacentResourceYieldChanges
	WHERE ImprovementType = 'IMPROVEMENT_HACIENDA' AND ResourceType = OLD.Type;
END;

--------------------------------------------------------------------------------
-- VP Great Admiral changes
-- Updating an existing unit to admiral class is not handled
--------------------------------------------------------------------------------
CREATE TRIGGER VP_Units_GlobalSeparateGreatAdmiral
AFTER INSERT ON Units
WHEN (NEW.Class = 'UNITCLASS_GREAT_ADMIRAL')
BEGIN
	UPDATE Units
	SET
		CanRepairFleet = 0,
		CanChangePort = 1
	WHERE Type = NEW.Type;
END;

--------------------------------------------------------------------------------
-- New Resources provide extra copies when a Manufactory is placed on them
--------------------------------------------------------------------------------
CREATE TRIGGER VP_ManufactoryNewResourceCompatibility_Insert
AFTER INSERT ON Resources
BEGIN
	INSERT INTO Improvement_ResourceExtractionIncrease
		(ImprovementType, ResourceType, Num)
	SELECT
		'IMPROVEMENT_MANUFACTORY', NEW.Type, 1;
END;

CREATE TRIGGER VP_ManufactoryNewResourceCompatibility_Delete
AFTER DELETE ON Resources
BEGIN
	DELETE FROM Improvement_ResourceExtractionIncrease
	WHERE ImprovementType = 'IMPROVEMENT_MANUFACTORY' AND ResourceType = OLD.Type;
END;

--------------------------------------------------------------------------------
-- New Bonus Resources gets Thrift gold bonus
--------------------------------------------------------------------------------
CREATE TRIGGER VP_BeliefFWBonusResources_Insert
AFTER INSERT ON Resources
WHEN (NEW.ResourceClassType = 'RESOURCECLASS_BONUS')
BEGIN
	INSERT INTO Belief_ResourceYieldChanges
		(BeliefType, ResourceType, YieldType, Yield)
	SELECT
		'BELIEF_FEED_WORLD', NEW.Type, 'YIELD_GOLD', 1;
END;

CREATE TRIGGER VP_BeliefFWBonusResources_UpdateTo
AFTER UPDATE OF ResourceClassType ON Resources
WHEN (NEW.ResourceClassType = 'RESOURCECLASS_BONUS' AND OLD.ResourceClassType <> 'RESOURCECLASS_BONUS')
BEGIN
	INSERT INTO Belief_ResourceYieldChanges
		(BeliefType, ResourceType, YieldType, Yield)
	SELECT
		'BELIEF_FEED_WORLD', NEW.Type, 'YIELD_GOLD', 1;
END;

CREATE TRIGGER VP_BeliefFWBonusResources_UpdateAway
AFTER UPDATE OF ResourceClassType ON Resources
WHEN (NEW.ResourceClassType <> 'RESOURCECLASS_BONUS' AND OLD.ResourceClassType = 'RESOURCECLASS_BONUS')
BEGIN
	DELETE FROM Belief_ResourceYieldChanges
	WHERE BeliefType = 'BELIEF_FEED_WORLD' AND ResourceType = OLD.Type;
END;

CREATE TRIGGER VP_BeliefFWBonusResources_Delete
AFTER DELETE ON Resources
WHEN (OLD.ResourceClassType = 'RESOURCECLASS_BONUS')
BEGIN
	DELETE FROM Belief_ResourceYieldChanges
	WHERE BeliefType = 'BELIEF_FEED_WORLD' AND ResourceType = OLD.Type;
END;

---------------------------------------------------------------------------------
-- Promotions that provide blanket immunity to all Plagues
---------------------------------------------------------------------------------
/*
CREATE TRIGGER VP_BlanketPlagueImmunityCompatibility
AFTER INSERT ON UnitPromotions_Plagues
BEGIN
	INSERT INTO UnitPromotions_BlockedPromotions
		(PromotionType, BlockedPromotionType)
	SELECT
		a.PromotionType,
		NEW.PlaguePromotionType
	FROM (
		SELECT 'PROMOTION_ONE' AS PromotionType, 'DOMAIN_SEA' AS DomainType
		UNION ALL SELECT 'PROMOTION_TWO', 'DOMAIN_LAND'
		UNION ALL SELECT 'PROMOTION_THREE', NULL
	) AS a
	WHERE (a.DomainType = NEW.DomainType OR a.DomainType IS NULL OR NEW.DomainType IS NULL)
	AND NOT EXISTS (SELECT 1 FROM UnitPromotions_BlockedPromotions WHERE PromotionType = a.PromotionType AND BlockedPromotionType = NEW.PlaguePromotionType);
END;
*/
