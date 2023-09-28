----------------------------------------------------------
-- Unique Ability: Hellenic League
----------------------------------------------------------
UPDATE Traits
SET AllianceCSStrength = 5
WHERE Type = 'TRAIT_CITY_STATE_FRIENDSHIP';

----------------------------------------------------------
-- Unique Unit: Hoplite (Spearman)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_SPEARMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_SPEARMAN') + 2
WHERE Type = 'UNIT_GREEK_HOPLITE';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_GREEK_HOPLITE', 'PROMOTION_ADJACENT_BONUS'),
	('UNIT_GREEK_HOPLITE', 'PROMOTION_SPAWN_GENERALS_II');

----------------------------------------------------------
-- Unique Building: Acropolis (Amphitheater)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_GREECE', 'BUILDINGCLASS_AMPHITHEATER', 'BUILDING_ODEON');

UPDATE Buildings SET Defense = 300 WHERE Type = 'BUILDING_ODEON';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_AMPHITHEATER' AND YieldType = 'YIELD_CULTURE') + 1
WHERE BuildingType = 'BUILDING_ODEON' AND YieldType = 'YIELD_CULTURE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_ODEON', 'YIELD_TOURISM', 3);

INSERT INTO Building_YieldFromVictoryGlobal
	(BuildingType, YieldType, Yield, IsEraScaling)
VALUES
	('BUILDING_ODEON', 'YIELD_CULTURE_LOCAL', 5, 1);

INSERT INTO Building_YieldFromVictory
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_ODEON', 'YIELD_CULTURE', 25);
