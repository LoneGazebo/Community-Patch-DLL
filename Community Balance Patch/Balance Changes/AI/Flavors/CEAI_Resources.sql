-- 

UPDATE Resources SET AIAvailability = 0;

-- Common
UPDATE Resources SET AIAvailability = 8
WHERE ResourceClassType IN (
	'RESOURCECLASS_BONUS'		
);

-- Rare
UPDATE Resources SET AIAvailability = 4
WHERE ResourceClassType IN (
	'RESOURCECLASS_RUSH'		,
	'RESOURCECLASS_MODERN'		,
	'RESOURCECLASS_LUXURY'		
);


--
-- Resource Priorities
--

DELETE FROM Resource_Flavors;



-- Improvement yields

INSERT INTO Resource_Flavors (ResourceType, FlavorType, Flavor)
	SELECT resYield.ResourceType, yield.FlavorType, 4 * resYield.Yield
	FROM Improvement_ResourceType_Yields resYield, Yields yield, Builds build
	WHERE yield.Type			= resYield.YieldType
	  AND build.ImprovementType = resYield.ImprovementType
	  AND build.AIAvailability >= 4;

INSERT INTO Resource_Flavors (ResourceType, FlavorType, Flavor)
	SELECT improveRes.ResourceType, yield.FlavorType, 4 * improveYield.Yield
	FROM Improvement_ResourceTypes improveRes, Improvement_Yields improveYield, Yields yield, Builds build
	WHERE improveYield.YieldType		= yield.Type
	  AND improveYield.ImprovementType	= improveRes.ImprovementType
	  AND build.ImprovementType			= improveRes.ImprovementType
	  AND build.AIAvailability			>= 4;

--/* One improvement per resource
DROP TABLE IF EXISTS CEP_Collisions;
CREATE TABLE CEP_Collisions(ResourceType text, FlavorType text, Flavor integer);
INSERT INTO CEP_Collisions (ResourceType, FlavorType, Flavor) SELECT ResourceType, FlavorType, MAX(Flavor) FROM Resource_Flavors GROUP BY ResourceType, FlavorType;
DELETE FROM Resource_Flavors;
INSERT INTO Resource_Flavors (ResourceType, FlavorType, Flavor) SELECT ResourceType, FlavorType, Flavor FROM CEP_Collisions;
DROP TABLE CEP_Collisions;
--*/

-- Other yields

INSERT INTO Resource_Flavors (ResourceType, FlavorType, Flavor)
	SELECT resYield.ResourceType, yield.FlavorType, 4 * resYield.Yield
	FROM Resource_YieldChanges resYield, Yields yield
	WHERE resYield.YieldType = yield.Type;
	
INSERT INTO Resource_Flavors (ResourceType, FlavorType, Flavor)
	SELECT resYield.ResourceType, yield.FlavorType, 4 * resYield.Yield
	FROM Building_ResourceYieldChanges resYield, Yields yield, Buildings building, BuildingClasses class 
	WHERE resYield.YieldType = yield.Type
	  AND resYield.BuildingType = building.Type
	  AND class.DefaultBuilding = building.Type
	  AND building.AIAvailability >= 4
	  AND building.BuildingClass <> 'BUILDINGCLASS_CARAVANSARY';

INSERT OR IGNORE INTO Resource_Flavors(ResourceType, FlavorType, Flavor)
	SELECT res.Type, flavor.Type, 2 * building.Happiness
	FROM Resources res, Flavors flavor, Buildings building, Building_LocalResourceOrs buildingRes
	WHERE res.Type      = buildingRes.ResourceType
	  AND building.Type = buildingRes.BuildingType
	  AND building.Happiness > 0
	  AND flavor.Type IN ('FLAVOR_HAPPINESS');

INSERT OR IGNORE INTO Resource_Flavors(ResourceType, FlavorType, Flavor)
	SELECT res.Type, flavor.Type, 2 * res.Happiness
	FROM Resources res, Flavors flavor
	WHERE res.Happiness > 0
	  AND flavor.Type IN ('FLAVOR_HAPPINESS');

INSERT OR IGNORE INTO Resource_Flavors(ResourceType, FlavorType, Flavor)
	SELECT res.Type, flavor.Type, 1 * res.Happiness
	FROM Resources res, Flavors flavor
	WHERE res.Happiness > 0
	  AND flavor.Type IN ('FLAVOR_EXPANSION');

--/* Sum yield flavors
DROP TABLE IF EXISTS CEP_Collisions;
CREATE TABLE CEP_Collisions(ResourceType text, FlavorType text, Flavor integer);
INSERT INTO CEP_Collisions (ResourceType, FlavorType, Flavor) SELECT ResourceType, FlavorType, SUM(Flavor) FROM Resource_Flavors GROUP BY ResourceType, FlavorType;
DELETE FROM Resource_Flavors;
INSERT INTO Resource_Flavors (ResourceType, FlavorType, Flavor) SELECT ResourceType, FlavorType, Flavor FROM CEP_Collisions;
DROP TABLE CEP_Collisions;
--*/



-- Land or water
-- Note some can be both, like oil

INSERT OR IGNORE INTO Resource_Flavors(ResourceType, FlavorType, Flavor)
	SELECT Type, 'FLAVOR_TILE_IMPROVEMENT', 4
	FROM Resources WHERE Land = 1;

INSERT OR IGNORE INTO Resource_Flavors(ResourceType, FlavorType, Flavor)
	SELECT Type, 'FLAVOR_NAVAL_TILE_IMPROVEMENT', 4
	FROM Resources WHERE Water = 1;
	
UPDATE Resource_Flavors
	SET FlavorType = 'FLAVOR_NAVAL_GROWTH'
	WHERE FlavorType = 'FLAVOR_GROWTH'
	AND ResourceType IN (SELECT Type FROM Resources WHERE Water = 1);



-- Strategics

INSERT OR IGNORE INTO Resource_Flavors(ResourceType, FlavorType, Flavor)
	SELECT res.Type, flavor.Type, 8
	FROM Resources res, Flavors flavor
	WHERE res.ResourceClassType IN ('RESOURCECLASS_RUSH', 'RESOURCECLASS_MODERN')
	  AND flavor.Type IN ('FLAVOR_OFFENSE');

INSERT OR IGNORE INTO Resource_Flavors(ResourceType, FlavorType, Flavor)
	SELECT res.Type, flavor.Type, 2
	FROM Resources res, Flavors flavor
	WHERE res.ResourceClassType IN ('RESOURCECLASS_RUSH', 'RESOURCECLASS_MODERN')
	  AND flavor.Type IN ('FLAVOR_DEFENSE');

INSERT OR IGNORE INTO Resource_Flavors(ResourceType, FlavorType, Flavor)
	SELECT res.Type, flavor.Type, 16
	FROM Resources res, Flavors flavor
	WHERE res.Type IN ('RESOURCE_IRON')
	  AND flavor.Type IN ('FLAVOR_SOLDIER');

INSERT OR IGNORE INTO Resource_Flavors(ResourceType, FlavorType, Flavor)
	SELECT res.Type, flavor.Type, 2
	FROM Resources res, Flavors flavor
	WHERE res.Type IN ('RESOURCE_IRON')
	  AND flavor.Type IN ('FLAVOR_NAVAL', 'FLAVOR_NAVAL_BOMBARDMENT');

INSERT OR IGNORE INTO Resource_Flavors(ResourceType, FlavorType, Flavor)
	SELECT res.Type, flavor.Type, 16
	FROM Resources res, Flavors flavor
	WHERE res.Type IN ('RESOURCE_HORSE')
	  AND flavor.Type IN ('FLAVOR_MOBILE');

INSERT OR IGNORE INTO Resource_Flavors(ResourceType, FlavorType, Flavor)
	SELECT res.Type, flavor.Type, 16
	FROM Resources res, Flavors flavor
	WHERE res.Type IN ('RESOURCE_COAL')
	  AND flavor.Type IN ('FLAVOR_CULTURE');

INSERT OR IGNORE INTO Resource_Flavors(ResourceType, FlavorType, Flavor)
	SELECT res.Type, flavor.Type, 16
	FROM Resources res, Flavors flavor
	WHERE res.Type IN ('RESOURCE_OIL')
	  AND flavor.Type IN ('FLAVOR_MOBILE', 'FLAVOR_NAVAL', 'FLAVOR_NAVAL_BOMBARDMENT');

INSERT OR IGNORE INTO Resource_Flavors(ResourceType, FlavorType, Flavor)
	SELECT res.Type, flavor.Type, 16
	FROM Resources res, Flavors flavor
	WHERE res.Type IN ('RESOURCE_ALUMINUM')
	  AND flavor.Type IN ('FLAVOR_AIR');

INSERT OR IGNORE INTO Resource_Flavors(ResourceType, FlavorType, Flavor)
	SELECT res.Type, flavor.Type, 16
	FROM Resources res, Flavors flavor
	WHERE res.Type IN ('RESOURCE_URANIUM')
	  AND flavor.Type IN ('FLAVOR_NUKE');



-- Archaeology

DELETE FROM Resource_Flavors WHERE ResourceType IN ('RESOURCE_ARTIFACTS', 'RESOURCE_HIDDEN_ARTIFACTS');
INSERT OR IGNORE INTO Resource_Flavors(ResourceType, FlavorType, Flavor)
	SELECT res.Type, flavor.Type, 8
	FROM Resources res, Flavors flavor
	WHERE res.Type IN ('RESOURCE_ARTIFACTS', 'RESOURCE_HIDDEN_ARTIFACTS')
	  AND flavor.Type IN ('FLAVOR_ARCHAEOLOGY', 'FLAVOR_TOURISM', 'FLAVOR_CULTURE');



	  
--/* Join duplicated flavors
DROP TABLE IF EXISTS CEP_Collisions;
CREATE TABLE CEP_Collisions(ResourceType text, FlavorType text, Flavor integer);
INSERT INTO CEP_Collisions (ResourceType, FlavorType, Flavor) SELECT ResourceType, FlavorType, MAX(Flavor) FROM Resource_Flavors GROUP BY ResourceType, FlavorType;
DELETE FROM Resource_Flavors;
INSERT INTO Resource_Flavors (ResourceType, FlavorType, Flavor) SELECT ResourceType, FlavorType, Flavor FROM CEP_Collisions;
DROP TABLE CEP_Collisions;
--*/


UPDATE LoadedFile SET Value=1 WHERE Type='CEAI_Resources.sql';