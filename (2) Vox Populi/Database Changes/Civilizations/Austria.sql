----------------------------------------------------------
-- Unique Ability: Habsburg Diplomacy
----------------------------------------------------------
UPDATE Traits
SET
	AbleToAnnexCityStates = 0,
	MinorQuestYieldModifier = 50,
	DiplomaticMarriage = 1
WHERE Type = 'TRAIT_ANNEX_CITY_STATE';

----------------------------------------------------------
-- Unique Unit: Hussar (Cuirassier)
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_CUIRASSIER' WHERE UnitType = 'UNIT_AUSTRIAN_HUSSAR';

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CUIRASSIER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_CUIRASSIER') + 1,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_CUIRASSIER') + 1,
	BaseSightRange = (SELECT BaseSightRange FROM Units WHERE Type = 'UNIT_CUIRASSIER') + 1
WHERE Type = 'UNIT_AUSTRIAN_HUSSAR';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_AUSTRIAN_HUSSAR', 'PROMOTION_LIGHTNING_WARFARE');

----------------------------------------------------------
-- Unique Unit: Grenzer (Fusilier)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_AUSTRIA', 'UNITCLASS_RIFLEMAN', 'UNIT_GRENZER');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_RIFLEMAN'
			)
		)
	)
WHERE Type = 'UNIT_GRENZER';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_GRENZER', 'PROMOTION_FRONTIERSMAN');

----------------------------------------------------------
-- Unique Building: Ballhausplatz (Printing Press)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_AUSTRIA', 'BUILDINGCLASS_PRINTING_PRESS', 'BUILDING_BALLHAUSPLATZ');

UPDATE Buildings
SET
	SpecialistCount = (SELECT SpecialistCount FROM Buildings WHERE Type = 'BUILDING_PRINTING_PRESS') + 1,
	GreatPeopleRateChange = (SELECT GreatPeopleRateChange FROM Buildings WHERE Type = 'BUILDING_PRINTING_PRESS') + 2,
	GPRateModifierPerMarriage = 15
WHERE Type = 'BUILDING_BALLHAUSPLATZ';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_PRINTING_PRESS' AND YieldType = 'YIELD_CULTURE') + 4
WHERE BuildingType = 'BUILDING_BALLHAUSPLATZ' AND YieldType = 'YIELD_CULTURE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BALLHAUSPLATZ', 'YIELD_GOLD', 5);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_BALLHAUSPLATZ', 'BUILDINGCLASS_CHANCERY', 'YIELD_GOLD', 2),
	('BUILDING_BALLHAUSPLATZ', 'BUILDINGCLASS_CHANCERY', 'YIELD_CULTURE', 2);

----------------------------------------------------------
-- Unique Building: Schützenstand (Military Academy)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_AUSTRIA', 'BUILDINGCLASS_MILITARY_ACADEMY', 'BUILDING_SCHUTZENSTAND');

UPDATE Buildings SET TrainedFreePromotion = 'PROMOTION_SCHUTZENKONIG' WHERE Type = 'BUILDING_SCHUTZENSTAND';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_MILITARY_ACADEMY' AND YieldType = 'YIELD_SCIENCE') + 1
WHERE BuildingType = 'BUILDING_SCHUTZENSTAND' AND YieldType = 'YIELD_SCIENCE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SCHUTZENSTAND', 'YIELD_PRODUCTION', 3),
	('BUILDING_SCHUTZENSTAND', 'YIELD_CULTURE', 3);

INSERT INTO Building_UnitCombatProductionModifiers
	(BuildingType, UnitCombatType, Modifier)
VALUES
	('BUILDING_SCHUTZENSTAND', 'UNITCOMBAT_GUN', 30);

INSERT INTO Building_UnitCombatFreeExperiences
	(BuildingType, UnitCombatType, Experience)
VALUES
	('BUILDING_SCHUTZENSTAND', 'UNITCOMBAT_GUN', 15);
