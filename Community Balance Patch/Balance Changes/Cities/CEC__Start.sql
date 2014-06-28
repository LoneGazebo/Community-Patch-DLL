--

DELETE FROM Building_DomainFreeExperiences;


--
-- Global Mods
--

UPDATE Units
SET HurryCostModifier = 50
WHERE (Combat > 0 OR RangedCombat > 0) AND HurryCostModifier >= 0;

UPDATE Units
SET HurryCostModifier = -1
WHERE Special = 'SPECIALUNIT_PEOPLE'
AND NOT CombatClass = 'UNITCOMBAT_DIPLOMACY';

UPDATE Buildings
SET TradeRouteLandDistanceModifier = TradeRouteLandDistanceModifier * 2, TradeRouteLandGoldBonus = TradeRouteLandGoldBonus * 2
WHERE BuildingClass IN ('BUILDINGCLASS_CARAVANSARY');


UPDATE Buildings
SET Cost = Cost * 2
WHERE Cost > 0 AND NOT BuildingClass IN (
	SELECT Type FROM BuildingClasses
	WHERE (
		MaxGlobalInstances = 1
		OR MaxTeamInstances = 1
		OR MaxPlayerInstances = 1
	)
);

UPDATE Buildings
SET Cost = Cost * 1.4
WHERE Cost > 0 AND BuildingClass IN (
	SELECT Type FROM BuildingClasses
	WHERE (
		MaxGlobalInstances = 1
		OR MaxTeamInstances = 1
		OR MaxPlayerInstances = 1
	)
);

UPDATE Projects
SET Cost = Cost * 1.4
WHERE Cost > 0;

UPDATE Buildings
SET Cost = Cost * 0.5, NumCityCostMod = 25
WHERE NumCityCostMod > 0;

UPDATE Buildings
SET GreatPeopleRateChange = 1
WHERE GreatPeopleRateChange = 2 AND BuildingClass IN (
	SELECT Type FROM BuildingClasses
	WHERE (
		MaxGlobalInstances = 1
	)
);

UPDATE Building_YieldChanges
SET Yield = 1
WHERE YieldType='YIELD_CULTURE' AND Yield >= 1 AND Yield <= 3 AND BuildingType IN (
	SELECT DefaultBuilding FROM BuildingClasses
	WHERE (
		MaxGlobalInstances = 1
		OR MaxTeamInstances = 1
		OR MaxPlayerInstances = 1
	)
);

UPDATE Buildings
SET HurryCostModifier = 0
WHERE HurryCostModifier > 0;

UPDATE Buildings
SET HurryCostModifier = 50
WHERE BuildingClass IN (
	'BUILDINGCLASS_WALLS',
	'BUILDINGCLASS_CASTLE',
	'BUILDINGCLASS_ARSENAL',
	'BUILDINGCLASS_MILITARY_BASE'
);


--
-- Happiness
--

UPDATE Buildings
SET Happiness = Happiness + 1
WHERE BuildingClass IN (
	'BUILDINGCLASS_COLOSSEUM'
);

UPDATE Buildings
SET UnmoddedHappiness = UnmoddedHappiness + 1
WHERE BuildingClass IN (
	'BUILDINGCLASS_THEATRE',
	'BUILDINGCLASS_STADIUM'
);


--
-- Wonders
--


DELETE FROM Building_UnitCombatProductionModifiers
WHERE BuildingType = 'BUILDING_TEMPLE_ARTEMIS';

INSERT INTO Building_UnitCombatFreeExperiences
	(BuildingType, UnitCombatType, Experience)
SELECT 'BUILDING_TEMPLE_ARTEMIS', 'UNITCOMBAT_ARCHER', '20'
WHERE EXISTS (SELECT * FROM Buildings WHERE Type='BUILDING_TEMPLE_ARTEMIS' );

INSERT INTO Building_UnitCombatFreeExperiences
	(BuildingType, UnitCombatType, Experience)
SELECT 'BUILDING_TEMPLE_ARTEMIS', 'UNITCOMBAT_GUN', '20'
WHERE EXISTS (SELECT * FROM Buildings WHERE Type='BUILDING_TEMPLE_ARTEMIS' );

INSERT INTO Building_FreeUnits
	(BuildingType, UnitType, NumUnits)
SELECT 'BUILDING_MAUSOLEUM_HALICARNASSUS', 'UNIT_MERCHANT', '1'
WHERE EXISTS (SELECT * FROM Buildings WHERE Type='BUILDING_MAUSOLEUM_HALICARNASSUS' );

--Compatibility with CSD for BUILDING_HAGIA_SOPHIA
INSERT INTO Building_SpecialistYieldChanges
	(BuildingType, SpecialistType, YieldType, Yield)
SELECT 'BUILDING_HAGIA_SOPHIA', 'SPECIALIST_CIVIL_SERVANT', 'YIELD_FAITH', 1
WHERE EXISTS (SELECT Value FROM Cep WHERE Type = 'USING_CSD' AND Value = 2);


UPDATE LoadedFile SET Value=1 WHERE Type='CEC__Start.sql';