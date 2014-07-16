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
SET TradeRouteLandDistanceModifier = TradeRouteLandDistanceModifier * 2, TradeRouteLandGoldBonus = 400
WHERE BuildingClass IN ('BUILDINGCLASS_CARAVANSARY');


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

UPDATE LoadedFile SET Value=1 WHERE Type='CEC__Start.sql';
