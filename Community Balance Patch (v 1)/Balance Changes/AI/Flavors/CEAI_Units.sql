-- 

/*

Load order must be:

1. All other flavor sql files
2. CEAI_Units.sql
3. CEAI_Buildings.sql
4. CEAI_Techs.sql

*/


--
-- Unit Flavors: update flavor types
--

/*
DELETE FROM Unit_Flavors WHERE FlavorType IN (
	'FLAVOR_RELIGION'		,
	'FLAVOR_ANTI_MOBILE'	,
	'FLAVOR_SOLDIER'	,
	'FLAVOR_SIEGE'	,
	'FLAVOR_NAVAL_BOMBARDMENT'	,
	''	,
	''	,
	''	,
	''	,
	''	,
);
*/

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_TOURISM', 1
FROM Units WHERE Class IN (
	'UNITCLASS_WRITER'		,
	'UNITCLASS_ARTIST'		,
	'UNITCLASS_MUSICIAN'	
);

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_RELIGION', 1
FROM Units WHERE Class IN (
	'UNITCLASS_MISSIONARY' ,
	'UNITCLASS_INQUISITOR'			
);

DELETE FROM Unit_Flavors WHERE FlavorType = 'FLAVOR_RECON'	AND UnitType = 'UNIT_WARRIOR';
DELETE FROM Unit_Flavors WHERE FlavorType = 'FLAVOR_GOLD'	AND UnitType = 'UNIT_PRIVATEER';
DELETE FROM Unit_Flavors WHERE FlavorType = 'FLAVOR_AIR'	AND UnitType = 'UNIT_PARATROOPER';
DELETE FROM Unit_Flavors WHERE FlavorType IN ('FLAVOR_NAVAL', 'FLAVOR_NAVAL_RECON');

DELETE FROM Unit_Flavors WHERE UnitType IN (SELECT Type FROM Units WHERE Class IN (
	'UNITCLASS_BIREME'			,
	'UNITCLASS_SHIP_OF_THE_LINE'	
) OR CombatClass IN (
	'UNITCOMBAT_RECON'			
));

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
SELECT unit.Type, 'FLAVOR_ANTI_MOBILE', 1
FROM Units unit, Unit_FreePromotions promo, UnitPromotions_UnitCombatMods modifier
WHERE (unit.Type = promo.UnitType AND promo.PromotionType = modifier.PromotionType AND unit.CombatClass <> 'UNITCOMBAT_RECON')
	AND modifier.UnitCombatType IN ('UNITCOMBAT_MOUNTED', 'UNITCOMBAT_MOUNTED_ARCHER', 'UNITCOMBAT_ARMOR')
;

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_MELEE', 1
FROM Units WHERE Class IN (
	'UNITCLASS_WARRIOR'				,
	'UNITCLASS_SWORDSMAN'			,
	'UNITCLASS_LONGSWORDSMAN'		
);

/*
INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_SOLDIER', 1
FROM Units WHERE Class IN (
	'UNITCLASS_WARRIOR'				,
	'UNITCLASS_SWORDSMAN'			,
	'UNITCLASS_LONGSWORDSMAN'		,
	'UNITCLASS_MUSKETMAN'			,
	'UNITCLASS_RIFLEMAN'			,
	'UNITCLASS_GREAT_WAR_INFANTRY'	,
	'UNITCLASS_INFANTRY'			,
	'UNITCLASS_MECHANIZED_INFANTRY'	
);
*/

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_SIEGE', 1
FROM Units WHERE CombatClass IN (
	'UNITCOMBAT_SIEGE'				
);

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_DEFENSE', 1
FROM Units WHERE Class IN (
	'UNITCLASS_BATTLESHIP'			,
	'UNITCLASS_CHARIOT_ARCHER'		,
	'UNITCLASS_IRONCLAD'			,
	'UNITCLASS_MISSILE_CRUISER'		,
	'UNITCLASS_SHIP_OF_THE_LINE'	
) OR Type IN (
	'UNIT_ARABIAN_CAMELARCHER'
);

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_NAVAL', 1
FROM Units WHERE CombatClass IN (
	'UNITCOMBAT_NAVALMELEE'				,
	'UNITCOMBAT_SUBMARINE'				
) AND NOT Class IN (
	'UNITCLASS_CARRIER'				
);

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_NAVAL_BOMBARDMENT', 1
FROM Units WHERE CombatClass IN (
	'UNITCOMBAT_NAVALRANGED'				
);

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_NAVAL_RECON', 1
FROM Units WHERE Class IN (
	'UNITCLASS_TRIREME'				,
	'UNITCLASS_BIREME'			,
	'UNITCLASS_CARAVEL'				,
	'UNITCLASS_PRIVATEER'			,
	'UNITCLASS_DESTROYER'			,
	'UNITCLASS_MISSILE_DESTROYER'	
);

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_OFFENSE', 1
FROM Units WHERE Class IN (
	'UNITCLASS_ANTI_AIRCRAFT_GUN'	,
	'UNITCLASS_BATTLESHIP'			,
	'UNITCLASS_IRONCLAD'			,
	'UNITCLASS_MISSILE_CRUISER'		,
	'UNITCLASS_SHIP_OF_THE_LINE'	
);

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_RANGED', 1
FROM Units WHERE CombatClass IN (
	'UNITCOMBAT_BOMBER'				
) OR Class IN (
	'UNITCLASS_GUIDED_MISSILE'
);

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_CITY_DEFENSE', 1
FROM Units WHERE CombatClass IN (
	'UNITCOMBAT_ARCHER',
	'UNITCOMBAT_MOUNTED_ARCHER',
	'UNITCOMBAT_BOMBER'
) OR Class IN (
	'UNITCLASS_GATLINGGUN',
	'UNITCLASS_MACHINE_GUN'
);

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
SELECT unit.Type, flavor.Type, 1
FROM Units unit, Flavors flavor
WHERE unit.CombatClass IN (
	'UNITCOMBAT_RECON'			
) AND flavor.Type IN (
	'FLAVOR_MELEE'				,
	'FLAVOR_HEALING'				,
	'FLAVOR_PILLAGE'				,
	'FLAVOR_RECON'					,
	'FLAVOR_OFFENSE'				,
	'FLAVOR_DEFENSE'				,
	'FLAVOR_CONQUEST'				,
	--'FLAVOR_CITY_DEFENSE'			,
	'FLAVOR_ANTI_MOBILE'			
);

/*
INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
SELECT unit.Type, flavor.Type, 1
FROM Units unit, Flavors flavor
WHERE unit.Class IN (
	'UNITCLASS_SCOUT'				,
	'UNITCLASS_SPEARMAN'			,
	'UNITCLASS_PIKEMAN'				,
	'UNITCLASS_MUSKETMAN'			,
	'UNITCLASS_RIFLEMAN'			,
	'UNITCLASS_GREAT_WAR_INFANTRY'	,
	'UNITCLASS_INFANTRY'			,
	'UNITCLASS_MECHANIZED_INFANTRY'	,
	--'UNITCLASS_SENTINEL'			,
	--'UNITCLASS_LEVY'				,
	--'UNITCLASS_MILITIA'			
) AND flavor.Type IN (
	'FLAVOR_RECON'					,
	'FLAVOR_DEFENSE'				,
	'FLAVOR_CITY_DEFENSE'			,
	'FLAVOR_ANTI_MOBILE'			
);
*/


--
-- Unit Flavors: update flavor values
--

UPDATE Unit_Flavors SET Flavor = 4;

UPDATE Unit_Flavors SET Flavor = Flavor * 2
WHERE FlavorType IN ('FLAVOR_NAVAL', 'FLAVOR_NAVAL_RECON', 'FLAVOR_RELIGION', 'FLAVOR_I_LAND_TRADE_ROUTE', 'FLAVOR_I_SEA_TRADE_ROUTE', 'FLAVOR_ARCHAEOLOGY' );


-- Great People and Specialists
UPDATE Unit_Flavors SET Flavor = 8
WHERE UnitType IN (SELECT Type FROM Units WHERE Special = 'SPECIALUNIT_PEOPLE');

DELETE FROM SpecialistFlavors;

INSERT INTO SpecialistFlavors (SpecialistType, FlavorType, Flavor)
	SELECT specYield.SpecialistType, yieldFlavor.FlavorType, 8
	FROM Yield_Flavors yieldFlavor, SpecialistYields specYield
	WHERE yieldFlavor.YieldType = specYield.YieldType
	;

INSERT INTO SpecialistFlavors (SpecialistType, FlavorType, Flavor)
	SELECT spec.Type, unitFlavor.FlavorType, 8
	FROM Unit_Flavors unitFlavor, UnitClasses class, Specialists spec
	WHERE class.Type = spec.GreatPeopleUnitClass
	AND unitFlavor.UnitType = class.DefaultUnit
	;

-- The "mobile" role involves flanking, which ranged units do not get a bonus for
UPDATE Unit_Flavors SET Flavor = ROUND(Flavor / 2, 0)
WHERE (FlavorType = 'FLAVOR_MOBILE')
AND UnitType IN (SELECT Type FROM Units WHERE Range > 0);


-- Unique Units
UPDATE Unit_Flavors SET Flavor = ROUND(Flavor * 2, 0)
WHERE UnitType IN (SELECT unit.Type FROM Units unit, UnitClasses class WHERE (
		unit.Class = class.Type
	AND unit.Type <> class.DefaultUnit
));

-- Vanguard
UPDATE Unit_Flavors SET Flavor = ROUND(Flavor * 2, 0)
WHERE FlavorType IN ('FLAVOR_HEALING', 'FLAVOR_RECON')
AND UnitType IN (SELECT Type FROM Units WHERE CombatClass IN (
	'UNITCOMBAT_RECON'
));

-- Naval Exploration
UPDATE Unit_Flavors SET Flavor = ROUND(Flavor / 2, 0)
WHERE FlavorType = 'FLAVOR_NAVAL_RECON'
AND UnitType IN (SELECT Type FROM Units WHERE CombatClass IN (
	'UNITCOMBAT_NAVALRANGED'
));

-- Naval Bombardment
UPDATE Unit_Flavors SET Flavor = ROUND(Flavor * 2, 0)
WHERE FlavorType = 'FLAVOR_NAVAL_BOMBARDMENT'
AND UnitType IN (SELECT Type FROM Units WHERE Class IN (
	'UNITCLASS_GALLEY'				,
	'UNITCLASS_BIREME'			,
	'UNITCLASS_GALLEASS'			
));

-- Ranged
UPDATE Unit_Flavors SET Flavor = ROUND(Flavor * 2, 0)
WHERE FlavorType IN ('FLAVOR_DEFENSE', 'FLAVOR_CITY_DEFENSE')
AND UnitType IN (SELECT Type FROM Units WHERE CombatClass IN (
	'UNITCOMBAT_ARCHER'				,
	'UNITCOMBAT_MOUNTED_ARCHER'		,
	'UNITCOMBAT_BOMBER'				
));

-- Machine Guns
UPDATE Unit_Flavors SET Flavor = ROUND(Flavor * 2, 0)
WHERE FlavorType IN ('FLAVOR_DEFENSE')
AND UnitType IN (SELECT Type FROM Units WHERE Class IN (
	'UNITCLASS_GATLINGGUN'			,
	'UNITCLASS_MACHINE_GUN'			
));

-- Anti-Mobile
--UPDATE Unit_Flavors SET Flavor = ROUND(Flavor * 2, 0)
--WHERE FlavorType = 'FLAVOR_ANTI_MOBILE';

-- Carrier
UPDATE Unit_Flavors SET Flavor = ROUND(Flavor * 2, 0)
WHERE FlavorType = 'FLAVOR_AIR_CARRIER';

-- Siege
UPDATE Unit_Flavors SET Flavor = ROUND(Flavor * 2, 0)
WHERE (FlavorType = 'FLAVOR_OFFENSE' OR FlavorType = 'FLAVOR_SIEGE')
AND UnitType IN (SELECT Type FROM Units WHERE CombatClass IN (
	'UNITCOMBAT_SIEGE'
));

UPDATE Unit_Flavors SET Flavor = ROUND(Flavor / 2, 0)
WHERE (FlavorType = 'FLAVOR_DEFENSE' OR FlavorType = 'FLAVOR_RANGED')
AND UnitType IN (SELECT Type FROM Units WHERE CombatClass IN (
	'UNITCOMBAT_SIEGE'
));

-- Strategic Default
UPDATE Unit_Flavors SET Flavor = ROUND(Flavor * 2, 0)
WHERE UnitType IN (SELECT unit.Type FROM Units unit, UnitClasses class
WHERE (	unit.Class = class.Type AND class.DefaultUnit IN (SELECT UnitType FROM Unit_ResourceQuantityRequirements)
));

-- Strategic
UPDATE Unit_Flavors SET Flavor = ROUND(Flavor / 2, 0)
WHERE FlavorType IN ('FLAVOR_DEFENSE', 'FLAVOR_CITY_DEFENSE')
AND UnitType IN (SELECT UnitType FROM Unit_ResourceQuantityRequirements);

-- Spaceship parts
UPDATE Unit_Flavors SET Flavor = ROUND(Flavor * 2, 0)
WHERE FlavorType = 'FLAVOR_SPACESHIP';

-- Fighters
UPDATE Unit_Flavors SET Flavor = ROUND(Flavor / 2, 0)
WHERE FlavorType = 'FLAVOR_AIR'
AND UnitType IN (SELECT Type FROM Units WHERE CombatClass IN (
	'UNITCOMBAT_FIGHTER'
));


-- Specific units

UPDATE Unit_Flavors SET Flavor = ROUND(Flavor * 4, 0)
WHERE UnitType IN (SELECT Type FROM Units WHERE Class IN (
	'UNITCLASS_SETTLER'				
));

UPDATE Unit_Flavors SET Flavor = ROUND(Flavor * 2, 0) --originally x2
WHERE UnitType IN (SELECT Type FROM Units WHERE Class IN (
	'UNITCLASS_WORKER'				,
	'UNITCLASS_WORKBOAT'			,
	--'UNITCLASS_MUSKETMAN'			,
	'UNITCLASS_ATOMIC_BOMB'			,
	'UNITCLASS_NUCLEAR_MISSILE'		,
	'UNITCLASS_MECH'
));

UPDATE Unit_Flavors SET Flavor = ROUND(Flavor / 2, 0)
WHERE (FlavorType = 'FLAVOR_OFFENSE' OR FlavorType = 'FLAVOR_DEFENSE')
AND UnitType IN (SELECT Type FROM Units WHERE Class IN (
	'UNITCLASS_TRIPLANE'			,
	'UNITCLASS_FIGHTER'				,
	'UNITCLASS_JET_FIGHTER'
));

UPDATE Unit_Flavors SET Flavor = ROUND(Flavor / 2, 0)
WHERE (FlavorType = 'FLAVOR_OFFENSE')
AND UnitType IN (SELECT Type FROM Units WHERE Class IN (
	'UNITCLASS_ANTI_TANK_GUN'		,
	'UNITCLASS_ANTI_AIRCRAFT_GUN'	,
	'UNITCLASS_MOBILE_SAM'
));

UPDATE Unit_Flavors SET Flavor = ROUND(Flavor / 2, 0)
WHERE (FlavorType = 'FLAVOR_DEFENSE')
AND UnitType IN (SELECT Type FROM Units WHERE Class IN (
	'UNITCLASS_WARRIOR'				,
	'UNITCLASS_MARINE'
));

UPDATE Unit_Flavors SET Flavor = ROUND(Flavor / 4, 0)
WHERE FlavorType NOT IN ('FLAVOR_HEALING', 'FLAVOR_PILLAGE')
AND UnitType IN (SELECT Type FROM Units WHERE Class IN (
	'UNITCLASS_CONSCRIPT',
	'UNITCLASS_PARATROOPER'
));

/*
UPDATE Unit_Flavors SET Flavor = ROUND(Flavor * 0.5, 0)
WHERE FlavorType = 'FLAVOR_DEFENSE' 
AND UnitType IN (SELECT UnitType FROM Unit_ResourceQuantityRequirements);
*/




-- Join duplicated flavors
DROP TABLE IF EXISTS CEP_Collisions;
CREATE TABLE CEP_Collisions(UnitType text, FlavorType text, Flavor integer);
INSERT INTO CEP_Collisions (UnitType, FlavorType, Flavor) SELECT UnitType, FlavorType, MAX(Flavor) FROM Unit_Flavors GROUP BY UnitType, FlavorType;
DELETE FROM Unit_Flavors;
INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor) SELECT UnitType, FlavorType, Flavor FROM CEP_Collisions;
DROP TABLE CEP_Collisions;

-- Join duplicated flavors
DROP TABLE IF EXISTS CEP_Collisions;
CREATE TABLE CEP_Collisions(SpecialistType text, FlavorType text, Flavor integer);
INSERT INTO CEP_Collisions (SpecialistType, FlavorType, Flavor) SELECT SpecialistType, FlavorType, MAX(Flavor) FROM SpecialistFlavors GROUP BY SpecialistType, FlavorType;
DELETE FROM SpecialistFlavors;
INSERT INTO SpecialistFlavors (SpecialistType, FlavorType, Flavor) SELECT SpecialistType, FlavorType, Flavor FROM CEP_Collisions;
DROP TABLE CEP_Collisions;

-- Revert BNW Flavors
DELETE FROM Unit_Flavors WHERE UnitType IN (SELECT UnitType FROM Unit_Flavors_BNW);
INSERT INTO Unit_Flavors SELECT * FROM Unit_Flavors_BNW WHERE UnitType IN (SELECT Type FROM Units);

UPDATE LoadedFile SET Value=1 WHERE Type='CEAI_Units.sql';