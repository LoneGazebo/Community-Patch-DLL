--

--UPDATE UnitPromotions
--SET CargoChange = CargoChange + 2 WHERE CargoChange > 0;

--
-- Gold Costs
--

UPDATE Units
SET HurryCostModifier = 0
WHERE (Combat = 0 AND RangedCombat = 0) AND HurryCostModifier >= 0;

UPDATE Units
SET HurryCostModifier = 50
WHERE (Combat > 0 OR RangedCombat > 0) AND HurryCostModifier >= 0;

UPDATE Units
SET HurryCostModifier = 50
WHERE (Found = 1) AND HurryCostModifier >= 0;

UPDATE Units
SET HurryCostModifier = -1
WHERE Special = 'SPECIALUNIT_PEOPLE'
AND NOT CombatClass = 'UNITCOMBAT_DIPLOMACY';


--
-- Faith Costs
--

UPDATE Units SET FaithCost = 1 * Cost WHERE FaithCost > 0 AND Cost > 0;


--
-- Conquest
--

UPDATE Improvements SET PillageGold = PillageGold * 2 WHERE PillageGold <> 0;

UPDATE Buildings
SET ConquestProb = 100
WHERE HurryCostModifier != -1;

UPDATE Buildings
SET ConquestProb = 0
WHERE BuildingClass IN (
	'BUILDINGCLASS_COURTHOUSE',
	'BUILDINGCLASS_CASTLE',
	'BUILDINGCLASS_ARSENAL',
	'BUILDINGCLASS_MILITARY_BASE',
	'BUILDINGCLASS_COLOSSEUM',
	'BUILDINGCLASS_THEATRE',
	'BUILDINGCLASS_STADIUM'
);

UPDATE Buildings
SET ConquestProb = 25
WHERE BuildingClass IN (
	'BUILDINGCLASS_WALLS'
);


--Must also set ObsoleteTech = 'TECH_AGRICULTURE' in CER_End.sql
UPDATE Units SET Cost = -1, ShowInPedia = 0, PrereqTech = NULL WHERE Class IN (
	'UNITCLASS_MARINE'				
);


UPDATE Units SET GoodyHutUpgradeUnitClass = NULL WHERE Class IN (
	'UNITCLASS_WARRIOR'			,
	'UNITCLASS_ARCHER'			
);


UPDATE LoadedFile SET Value=1 WHERE Type='CEA__End.sql';
