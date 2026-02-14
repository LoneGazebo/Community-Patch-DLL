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
-- Unique Unit: SPAD S.VII (Triplane)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_FRANCE', 'UNITCLASS_TRIPLANE', 'UNIT_SPAD');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_TRIPLANE'
			)
		)
	),
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_TRIPLANE') + 5 
WHERE Type = 'UNIT_SPAD';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_SPAD', 'PROMOTION_SORTIE');

----------------------------------------------------------
-- Unique Improvement: Chateau
----------------------------------------------------------
UPDATE Builds
SET
	IconAtlas = 'BUILD_ATLAS_VP',
	IconIndex = 2
WHERE Type = 'BUILD_CHATEAU';

UPDATE Improvements
SET
	NoFollowUp = 1,
	ArtDefineTag = 'ART_DEF_IMPROVEMENT_CHATEAU_NEW',
	IconAtlas = 'IMPROVEMENT_ATLAS_VP',
	PortraitIndex = 2
WHERE Type = 'IMPROVEMENT_CHATEAU';

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

----------------------------------------------------------
-- Unique Building: Salon (Gallery)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_FRANCE', 'BUILDINGCLASS_GALLERY', 'BUILDING_SALON');

UPDATE Buildings
SET
	ThemingBonusHelp = 'TXT_KEY_SALON_THEMING_BONUS_HELP',
	GPRateModifierPerLocalTheme = 10,
	GreatWorkSlotType = 'GREAT_WORK_SLOT_ART_ARTIFACT',
	GreatWorkCount = 2
WHERE Type = 'BUILDING_SALON';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_GALLERY' AND YieldType = 'YIELD_CULTURE') + 3
WHERE BuildingType = 'BUILDING_SALON' AND YieldType = 'YIELD_CULTURE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SALON', 'YIELD_SCIENCE', 3),
	('BUILDING_SALON', 'YIELD_TOURISM', 3);

INSERT INTO Building_YieldChangesPerLocalTheme
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SALON', 'YIELD_SCIENCE', 3);

INSERT INTO Building_ThemingBonuses
	(BuildingType, Description, MustBeArt, RequiresOwner, RequiresAnyButOwner, RequiresSamePlayer, SameEra)
VALUES
	('BUILDING_SALON', 'TXT_KEY_THEMING_BONUS_SALON_1', 1, 0, 1, 1, 1),
	('BUILDING_SALON', 'TXT_KEY_THEMING_BONUS_SALON_2', 1, 0, 1, 1, 0),
	('BUILDING_SALON', 'TXT_KEY_THEMING_BONUS_SALON_3', 1, 1, 0, 0, 1),
	('BUILDING_SALON', 'TXT_KEY_THEMING_BONUS_SALON_4', 1, 1, 0, 0, 0);
