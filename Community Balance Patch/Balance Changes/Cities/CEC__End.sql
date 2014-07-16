--

UPDATE Buildings SET Cost = Cost * 0.500 WHERE BuildingClass = 'BUILDINGCLASS_WINDMILL';
UPDATE Buildings SET Cost = Cost * 0.500 WHERE BuildingClass = 'BUILDINGCLASS_CARAVANSARY';
UPDATE Buildings SET Cost = Cost * 0.800 WHERE BuildingClass = 'BUILDINGCLASS_BARRACKS';
UPDATE Buildings SET Cost = Cost * 0.840 WHERE BuildingClass = 'BUILDINGCLASS_ARMORY';
UPDATE Buildings SET Cost = Cost * 0.800 WHERE BuildingClass = 'BUILDINGCLASS_MILITARY_ACADEMY';
UPDATE Buildings SET Cost = Cost * 0.750 WHERE BuildingClass = 'BUILDINGCLASS_ARSENAL';
UPDATE Buildings SET Cost = Cost * 0.600 WHERE BuildingClass = 'BUILDINGCLASS_MILITARY_BASE';
UPDATE Buildings SET Cost = Cost * 0.750 WHERE BuildingClass = 'BUILDINGCLASS_SHRINE';
UPDATE Buildings SET Cost = Cost * 0.900 WHERE BuildingClass = 'BUILDINGCLASS_AMPHITHEATER';
UPDATE Buildings SET Cost = Cost * 0.825 WHERE BuildingClass = 'BUILDINGCLASS_OPERA_HOUSE';
UPDATE Buildings SET Cost = Cost * 0.833 WHERE BuildingClass = 'BUILDINGCLASS_MUSEUM';
UPDATE Buildings SET Cost = Cost * 0.750 WHERE BuildingClass = 'BUILDINGCLASS_BROADCAST_TOWER';
UPDATE Buildings SET Cost = Cost * 0.600 WHERE BuildingClass = 'BUILDINGCLASS_STABLE';
UPDATE Buildings SET Cost = Cost * 0.667 WHERE BuildingClass = 'BUILDINGCLASS_WORKSHOP';
UPDATE Buildings SET Cost = Cost * 0.630 WHERE BuildingClass = 'BUILDINGCLASS_HYDRO_PLANT';
UPDATE Buildings SET Cost = Cost * 0.780 WHERE BuildingClass = 'BUILDINGCLASS_FACTORY';
UPDATE Buildings SET Cost = Cost * 0.800 WHERE BuildingClass = 'BUILDINGCLASS_SOLAR_PLANT';
UPDATE Buildings SET Cost = Cost * 0.800 WHERE BuildingClass = 'BUILDINGCLASS_NUCLEAR_PLANT';
UPDATE Buildings SET Cost = Cost * 0.750 WHERE BuildingClass = 'BUILDINGCLASS_MINT';
UPDATE Buildings SET Cost = Cost * 1.125 WHERE BuildingClass = 'BUILDINGCLASS_BANK';
UPDATE Buildings SET Cost = Cost * 0.694 WHERE BuildingClass = 'BUILDINGCLASS_HOSPITAL';
UPDATE Buildings SET Cost = Cost * 0.700 WHERE BuildingClass = 'BUILDINGCLASS_MEDICAL_LAB';
UPDATE Buildings SET Cost = Cost * 0.750 WHERE BuildingClass = 'BUILDINGCLASS_HARBOR';
UPDATE Buildings SET Cost = Cost * 0.800 WHERE BuildingClass = 'BUILDINGCLASS_COLOSSEUM';
UPDATE Buildings SET Cost = Cost * 0.640 WHERE BuildingClass = 'BUILDINGCLASS_STADIUM';
UPDATE Buildings SET Cost = Cost * 0.800 WHERE BuildingClass = 'BUILDINGCLASS_THEATRE';
UPDATE Buildings SET Cost = Cost * 1.333 WHERE BuildingClass = 'BUILDINGCLASS_CIRCUS';
UPDATE Buildings SET Cost = Cost * 0.833 WHERE BuildingClass = 'BUILDINGCLASS_GARDEN';
UPDATE Buildings SET Cost = Cost * 0.800 WHERE BuildingClass = 'BUILDINGCLASS_OBSERVATORY';

--
-- Unique Buildings
--

-- Set unique building cost to the same as the regular building it replaces
UPDATE Buildings SET Cost = (SELECT b.Cost FROM Buildings b, BuildingClasses class WHERE (
	Buildings.BuildingClass = b.BuildingClass
	AND class.Type = b.BuildingClass
	AND class.DefaultBuilding = b.Type
));

UPDATE Buildings SET	Cost   = ROUND(0.75 * (SELECT Cost   FROM Buildings WHERE Type = 'BUILDING_WORKSHOP'), 0)
						WHERE Type IN ('BUILDING_LONGHOUSE');



UPDATE Buildings
SET Cost = ROUND(Cost / 10, 0) * 10
WHERE Cost > 0;



--
-- Specific Buildings
--


INSERT INTO				Building_LakePlotYieldChanges (BuildingType, YieldType, Yield) 
SELECT					Type, 'YIELD_FOOD', 1
FROM					Buildings
WHERE					BuildingClass = 'BUILDINGCLASS_AQUEDUCT';


--
-- Resources
--

DELETE FROM Building_ResourceYieldChanges
WHERE BuildingType IN (	
	'BUILDING_MINT'
);


INSERT OR REPLACE INTO	Building_ResourceYieldChanges
						(BuildingType, ResourceType, YieldType, Yield) 
SELECT					building.Type, res.Type, 'YIELD_CULTURE', 1
FROM					Buildings building, Resources res
WHERE					building.BuildingClass = 'BUILDINGCLASS_GARDEN'
						AND res.Type IN (
							'RESOURCE_CITRUS'	,
							'RESOURCE_WINE'		
						);

INSERT OR REPLACE INTO	Building_ResourceYieldChanges
						(BuildingType, ResourceType, YieldType, Yield) 
SELECT					building.Type, res.Type, 'YIELD_CULTURE', 1
FROM					Buildings building, Resources res
WHERE					building.BuildingClass = 'BUILDINGCLASS_AMPHITHEATER'
						AND res.Type IN (
							'RESOURCE_SILK'		,
							'RESOURCE_DYE'
						);


INSERT OR REPLACE INTO	Building_ResourceYieldChanges
						(BuildingType, ResourceType, YieldType, Yield) 
SELECT					building.Type, res.Type, 'YIELD_GOLD', 2
FROM					Buildings building, Resources res
WHERE					building.BuildingClass = 'BUILDINGCLASS_MINT'
						AND res.Type IN (
							'RESOURCE_COPPER'	,
							'RESOURCE_SILVER'	,
							'RESOURCE_GOLD'		
						);


INSERT OR REPLACE INTO	Building_ResourceYieldChanges
						(BuildingType, ResourceType, YieldType, Yield) 
SELECT					building.Type, res.Type, 'YIELD_PRODUCTION', 1
FROM					Buildings building, Resources res
WHERE					building.BuildingClass = 'BUILDINGCLASS_FACTORY'
						AND res.Type IN (
							'RESOURCE_COAL'		,
							'RESOURCE_ALUMINUM'	
						);
						
INSERT OR REPLACE INTO	Building_ResourceYieldChanges
						(BuildingType, ResourceType, YieldType, Yield) 
SELECT					building.Type, res.Type, 'YIELD_GOLD', 1
FROM					Buildings building, Resources res
WHERE					building.BuildingClass = 'BUILDINGCLASS_STOCK_EXCHANGE'
						AND res.Type IN (
							'RESOURCE_OIL'		
						);

INSERT OR REPLACE INTO	Building_ResourceYieldChanges
						(BuildingType, ResourceType, YieldType, Yield) 
SELECT					building.Type, res.Type, 'YIELD_SCIENCE', 1
FROM					Buildings building, Resources res
WHERE					building.BuildingClass = 'BUILDINGCLASS_LABORATORY'
						AND res.Type IN (
							'RESOURCE_URANIUM'		
						);

INSERT OR REPLACE INTO	Building_ResourceYieldChanges
						(BuildingType, ResourceType, YieldType, Yield) 
SELECT					building.Type, res.Type, 'YIELD_GOLD', 1
FROM					Buildings building, Resources res
WHERE					building.BuildingClass = 'BUILDINGCLASS_CARAVANSARY'
						AND res.Type IN (
							'RESOURCE_FURS'	,
							'RESOURCE_COTTON',	
							'RESOURCE_TRUFFLES'	,
						);

INSERT OR REPLACE INTO	Building_ResourceYieldChanges
						(BuildingType, ResourceType, YieldType, Yield) 
SELECT					building.Type, res.Type, 'YIELD_GOLD', 1
FROM					Buildings building, Resources res
WHERE					building.BuildingClass = 'BUILDINGCLASS_MARKET'
						AND res.Type IN (
							'RESOURCE_SPICES'	,
							'RESOURCE_SUGAR'
						);
--
-- Yields
--

INSERT OR REPLACE INTO	Building_YieldChanges(BuildingType, YieldType, Yield) 
SELECT					building.Type, 'YIELD_PRODUCTION', 1
FROM					Buildings building
WHERE					building.BuildingClass IN (
						'BUILDINGCLASS_STABLE'		
						);

						
UPDATE					Building_YieldChanges
SET						Yield = 3
WHERE					(YieldType = 'YIELD_CULTURE'
AND						BuildingType IN (SELECT Type FROM Buildings WHERE BuildingClass IN (
						'BUILDINGCLASS_OPERA_HOUSE'	,
						'BUILDINGCLASS_BROADCAST_TOWER'	
						)));


DELETE FROM Building_UnitCombatProductionModifiers
WHERE BuildingType IN (
	'BUILDING_STABLE'		,
	'BUILDING_DUCAL_STABLE'	
);

INSERT OR REPLACE INTO	Building_UnitCombatProductionModifiers(BuildingType, UnitCombatType, Modifier) 
SELECT					building.Type, combat.Type, 15
FROM					Buildings building, UnitCombatInfos combat
WHERE					building.BuildingClass IN (
						'BUILDINGCLASS_STABLE'		
						)
AND						combat.Type IN (
						'UNITCOMBAT_MOUNTED'		,
						'UNITCOMBAT_GUN'			
						);


--
-- Features
--

DELETE FROM				Building_FeatureYieldChanges WHERE FeatureType = 'FEATURE_JUNGLE';

INSERT OR REPLACE INTO	Building_FeatureYieldChanges(BuildingType, FeatureType, YieldType, Yield) 
SELECT					building.Type, 'FEATURE_JUNGLE', 'YIELD_SCIENCE', 1
FROM					Buildings building
WHERE					building.BuildingClass IN (
						'BUILDINGCLASS_UNIVERSITY'		,
						'BUILDINGCLASS_LABORATORY'	
						);


--
-- Specialists
--

UPDATE Specialists
SET GreatPeopleRateChange = 3
WHERE GreatPeopleRateChange > 0;

UPDATE Buildings
SET SpecialistType = 'SPECIALIST_ENGINEER', SpecialistCount = 1
WHERE BuildingClass IN (
	'BUILDINGCLASS_IRONWORKS'			
);

UPDATE Buildings
SET SpecialistType = 'SPECIALIST_MERCHANT', SpecialistCount = 1
WHERE BuildingClass IN (
	'BUILDINGCLASS_CARAVANSARY'			,	
	'BUILDINGCLASS_STOCK_EXCHANGE'		,
	'BUILDINGCLASS_NATIONAL_TREASURY'	
);

UPDATE Buildings
SET SpecialistType = 'SPECIALIST_SCIENTIST', SpecialistCount = 1
WHERE BuildingClass IN (
	'BUILDINGCLASS_OBSERVATORY'			,
	'BUILDINGCLASS_UNIVERSITY'			,
	'BUILDINGCLASS_LABORATORY'			,
	'BUILDINGCLASS_OXFORD_UNIVERSITY'	
);


UPDATE Buildings
SET SpecialistType = 'SPECIALIST_SCIENTIST', SpecialistCount = 1
WHERE Type = 'BUILDING_WALLS_OF_BABYLON';
/*
UPDATE Buildings
SET SpecialistType = 'SPECIALIST_MERCHANT', SpecialistCount = 1
WHERE Type = 'BUILDING_TRADE_OFFICE';
*/


UPDATE LoadedFile SET Value=1 WHERE Type='CEC__End.sql';
