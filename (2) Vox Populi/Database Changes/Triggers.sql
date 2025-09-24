-- NOTE: THIS FILE MUST BE LAST IN THE LOAD ORDER FOR (2) Vox Populi
-- For better data control, VP doesn't use triggers for itself.
-- These triggers only aim to save work for modmodders.

---------------------------------------------------------------------------------
-- New units have same building requirements for purchase as base unit
---------------------------------------------------------------------------------
CREATE TRIGGER Unit_BuildingClassPurchaseRequireds
AFTER INSERT ON Units
WHEN (EXISTS (SELECT Type FROM Units WHERE Type = NEW.Type))
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
	SELECT 'TRAIT_WAYFINDING', NEW.Class, 'BUILD_FISHING_BOATS_EMBARKED'
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
	SELECT 'TRAIT_WAYFINDING', NEW.Class, 'BUILD_FISHING_BOATS_EMBARKED'
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
-- This assumes resource class won't be updated
---------------------------------------------------------------------------------
CREATE TRIGGER VP_HaciendaCompatibility_Bonus
AFTER INSERT ON Resources
WHEN (NEW.ResourceClassType = 'RESOURCECLASS_BONUS')
BEGIN
	INSERT INTO Improvement_AdjacentResourceYieldChanges
		(ImprovementType, ResourceType, YieldType, Yield)
	SELECT
		'IMPROVEMENT_HACIENDA', NEW.Type, 'YIELD_FOOD', 3;
END;

CREATE TRIGGER VP_HaciendaCompatibility_Strategic
AFTER INSERT ON Resources
WHEN (NEW.ResourceClassType IN ('RESOURCECLASS_RUSH', 'RESOURCECLASS_MODERN'))
BEGIN
	INSERT INTO Improvement_AdjacentResourceYieldChanges
		(ImprovementType, ResourceType, YieldType, Yield)
	SELECT
		'IMPROVEMENT_HACIENDA', NEW.Type, 'YIELD_PRODUCTION', 3;
END;

CREATE TRIGGER VP_HaciendaCompatibility_Luxury
AFTER INSERT ON Resources
WHEN (NEW.ResourceClassType = 'RESOURCECLASS_LUXURY')
BEGIN
	INSERT INTO Improvement_AdjacentResourceYieldChanges
		(ImprovementType, ResourceType, YieldType, Yield)
	SELECT
		'IMPROVEMENT_HACIENDA', NEW.Type, 'YIELD_GOLD', 3;
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