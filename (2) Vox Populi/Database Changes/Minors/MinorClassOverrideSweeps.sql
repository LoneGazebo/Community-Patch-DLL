DELETE FROM Civilization_UnitClassOverrides WHERE CivilizationType = 'CIVILIZATION_MINOR';
DELETE FROM Civilization_BuildingClassOverrides WHERE CivilizationType = 'CIVILIZATION_MINOR';

-- Block CS from producing these units
-- Can't use UnitCombat here unfortunately
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType)
SELECT
	'CIVILIZATION_MINOR', Type
FROM UnitClasses
WHERE Type IN (
	-- Early recon
	'UNITCLASS_PATHFINDER',
	'UNITCLASS_SCOUT',
	'UNITCLASS_EXPLORER',
	'UNITCLASS_COMMANDO',
	-- Late settlers
	'UNITCLASS_PIONEER',
	'UNITCLASS_COLONIST',
	-- Trade units
	'UNITCLASS_CARAVAN',
	'UNITCLASS_CARGO_SHIP',
	-- Diplo units
	'UNITCLASS_EMISSARY',
	'UNITCLASS_ENVOY',
	'UNITCLASS_DIPLOMAT',
	'UNITCLASS_AMBASSADOR',
	'UNITCLASS_GREAT_DIPLOMAT',
	-- Archaeologist
	'UNITCLASS_ARCHAEOLOGIST',
	-- Carriers
	'UNITCLASS_CARRIER',
	-- Nukes
	'UNITCLASS_ATOMIC_BOMB',
	'UNITCLASS_NUCLEAR_MISSILE',
	-- Spaceship parts
	'UNITCLASS_SS_COCKPIT',
	'UNITCLASS_SS_STASIS_CHAMBER',
	'UNITCLASS_SS_ENGINE',
	'UNITCLASS_SS_BOOSTER'
);

-- Block CS from building these
-- National Wonders/World Wonders
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType)
SELECT
	'CIVILIZATION_MINOR', Type
FROM BuildingClasses
WHERE (MaxPlayerInstances <> -1 OR MaxGlobalInstances <> -1)
AND Type <> 'BUILDINGCLASS_PALACE';

-- Diplo buildings
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType)
SELECT
	'CIVILIZATION_MINOR', Type
FROM BuildingClasses
WHERE Type IN (
	'BUILDINGCLASS_CHANCERY',
	'BUILDINGCLASS_WIRE_SERVICE'
);
