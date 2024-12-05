----------------------------------------------------------
-- Unique Ability: Monument Builders
----------------------------------------------------------
UPDATE Traits
SET WonderProductionModGA = 20
WHERE Type = 'TRAIT_WONDER_BUILDER';

INSERT INTO Trait_ArtifactYieldChanges
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_WONDER_BUILDER', 'YIELD_SCIENCE', 5),
	('TRAIT_WONDER_BUILDER', 'YIELD_CULTURE', 5);

INSERT INTO Trait_ImprovementYieldChanges
	(TraitType, ImprovementType, YieldType, Yield)
VALUES
	('TRAIT_WONDER_BUILDER', 'IMPROVEMENT_LANDMARK', 'YIELD_GOLD', 5),
	('TRAIT_WONDER_BUILDER', 'IMPROVEMENT_LANDMARK', 'YIELD_TOURISM', 5);

----------------------------------------------------------
-- Unique Unit: War Chariot (Chariot Archer)
----------------------------------------------------------
UPDATE Units
SET
	PrereqTech = 'TECH_THE_WHEEL',
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CHARIOT_ARCHER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_CHARIOT_ARCHER') + 1,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_CHARIOT_ARCHER') + 1
WHERE Type = 'UNIT_EGYPTIAN_WARCHARIOT';

DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_EGYPTIAN_WARCHARIOT';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_EGYPTIAN_WARCHARIOT', 'PROMOTION_GIFT_OF_THE_PHARAOH');
