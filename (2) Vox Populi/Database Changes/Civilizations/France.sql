----------------------------------------------------------
-- Unique Ability: La Grande Armée
----------------------------------------------------------
UPDATE Traits
SET
	CapitalThemingBonusModifier = 0,
	ExtraSupplyPerPopulation = 15
WHERE Type = 'TRAIT_ENHANCED_CULTURE';

INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
VALUES
	('TRAIT_ENHANCED_CULTURE', 'UNITCOMBAT_MELEE', 'PROMOTION_COERCION'),
	('TRAIT_ENHANCED_CULTURE', 'UNITCOMBAT_GUN', 'PROMOTION_COERCION');

INSERT INTO Trait_YieldFromXMilitaryUnits
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_ENHANCED_CULTURE', 'YIELD_CULTURE', 10),
	('TRAIT_ENHANCED_CULTURE', 'YIELD_TOURISM', 10);

----------------------------------------------------------
-- Unique Unit: Musketeer (Tercio)
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_TERCIO' WHERE UnitType = 'UNIT_FRENCH_MUSKETEER';

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_SPANISH_TERCIO'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_SPANISH_TERCIO') + 5
WHERE Type = 'UNIT_FRENCH_MUSKETEER';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_FRENCH_MUSKETEER', 'PROMOTION_LIGHTNING_WARFARE');

----------------------------------------------------------
-- Unique Improvement: Chateau
----------------------------------------------------------
UPDATE Improvements SET NoFollowUp = 1 WHERE Type = 'IMPROVEMENT_CHATEAU';

UPDATE Improvement_Yields
SET Yield = 3
WHERE ImprovementType = 'IMPROVEMENT_CHATEAU' AND YieldType = 'YIELD_GOLD';

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_CHATEAU', 'YIELD_FOOD', 3);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_CHATEAU', 'TECH_PRINTING_PRESS', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_CHATEAU', 'TECH_FLIGHT', 'YIELD_GOLD', 2),
	('IMPROVEMENT_CHATEAU', 'TECH_FLIGHT', 'YIELD_CULTURE', 2);
