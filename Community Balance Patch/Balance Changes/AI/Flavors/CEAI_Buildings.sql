--

/*

Load order must be:

1. All other flavor sql files
2. CEAI_Units.sql
3. CEAI_Buildings.sql
4. CEAI_Techs.sql

*/

--
-- Availability: Buildings
--

UPDATE Buildings SET AIAvailability = 8;

UPDATE Buildings SET AIAvailability = 4
WHERE (Water = 1
	OR River = 1
	OR FreshWater = 1
	OR Hill = 1
	OR Flat = 1
	OR Mountain = 1
	OR NearbyMountainRequired = 1
	OR MutuallyExclusiveGroup = 1
	OR NoOccupiedUnhappiness = 1
	OR NearbyTerrainRequired IS NOT NULL
);

UPDATE Buildings SET AIAvailability = 4
WHERE (Type IN (SELECT BuildingType FROM Building_ResourceQuantityRequirements)
	OR Type IN (SELECT BuildingType FROM Building_LocalResourceOrs)
	OR Type IN (SELECT BuildingType FROM Building_LocalResourceAnds)
	--OR Type IN (SELECT BuildingType FROM Building_ResourceYieldModifiers)
);

UPDATE Buildings SET AIAvailability = 2
WHERE Type IN (SELECT building.Type
FROM Buildings building, BuildingClasses class
WHERE (building.BuildingClass = class.Type AND (
	   class.MaxGlobalInstances = 1
	OR class.MaxPlayerInstances = 1
	OR class.MaxTeamInstances = 1
)));

UPDATE Buildings SET AIAvailability = 0
WHERE Cost = -1;


--
-- Project Priorities
--

INSERT INTO Project_Flavors (ProjectType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_SPACESHIP', 128
FROM Projects WHERE VictoryPrereq = 'VICTORY_SPACE_RACE' AND Spaceship = 0;

INSERT INTO Project_Flavors (ProjectType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_SPACESHIP', 4
FROM Projects WHERE Spaceship = 1;

INSERT INTO Project_Flavors (ProjectType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_NUKE', 128
FROM Projects WHERE AllowsNukes = 1;


--
-- Building Priorities
--

DELETE FROM Building_Flavors;
DELETE FROM Building_Flavors_Human;

/*
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_INFRASTRUCTURE', 8
FROM Buildings;
*/

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_TILE_IMPROVEMENT', 8
FROM Buildings WHERE (
	   WorkerSpeedModifier <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_GOLDEN_AGE', 8
FROM Buildings WHERE (
	   GoldenAge = 1
	OR GoldenAgePoints > 0
	OR GoldenAgeModifier <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_GROWTH', 8
FROM Buildings WHERE (
	   InstantHappiness <> 0
	OR FoodKept <> 0
	OR GlobalPopulationChange <> 0
	OR AllowsFoodTradeRoutes <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_PRODUCTION', 8
FROM Buildings WHERE (
	   InstantHappiness <> 0
	OR BuildingProductionModifier <> 0
	OR AllowsProductionTradeRoutes <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_GOLD', 8
FROM Buildings WHERE (
	   InstantHappiness <> 0
	OR GreatPersonExpendGold <> 0
	OR UnitUpgradeCostMod <> 0
	OR CityConnectionTradeRouteModifier <> 0
	OR PlotBuyCostModifier <> 0
	OR GlobalPlotBuyCostModifier <> 0
	OR Gold <> 0
	OR MinorFriendshipChange <> 0
	OR TradeDealModifier <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_SCIENCE', 8
FROM Buildings WHERE (
	   InstantHappiness <> 0
	OR TechShare <> 0
	OR FreeTechs <> 0
	OR MedianTechPercentChange <> 0
	OR GreatScientistBeakerModifier <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_CULTURE', 8
FROM Buildings WHERE (
	   InstantHappiness <> 0
	OR CultureRateModifier <> 0
	OR GlobalCultureRateModifier <> 0
	OR PolicyCostModifier <> 0
	OR FreePolicies <> 0
	OR XBuiltTriggersIdeologyChoice <> 0
	OR GreatWorkSlotType <> 0
	OR FreeGreatWork <> 0
	OR SpecialistExtraCulture <> 0
	OR CityCaptureCulture <> 0
	OR CityCaptureCulturePerPop <> 0
	OR CityCaptureCulturePerEra <> 0
	OR CityCaptureCulturePerEraExponent <> 1
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_TOURISM', 8
FROM Buildings WHERE (
	   LandmarksTourismPercent <> 0
	OR GreatWorksTourismModifier <> 0
	OR TechEnhancedTourism <> 0
	OR GreatWorkSlotType <> 0
	OR FreeGreatWork <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_EXPANSION', 8
FROM Buildings WHERE (
	   WorkerSpeedModifier <> 0
	OR MapCentering = 1
	OR UnhappinessModifier <> 0
	OR HappinessPerCity <> 0
	OR HappinessPerXPolicies <> 0
	OR CityCountUnhappinessMod <> 0
	OR PlotCultureCostModifier <> 0
	OR GlobalPlotCultureCostModifier <> 0
	OR PlotBuyCostModifier <> 0
	OR GlobalPlotBuyCostModifier <> 0
	OR InstantBorderRadius <> 0
	OR GlobalInstantBorderRadius <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_EXPANSION', 4
FROM Buildings WHERE (
	   MapCentering = 1
	OR Happiness <> 0
	OR UnmoddedHappiness <> 0
	OR NoOccupiedUnhappiness <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_HAPPINESS', 8
FROM Buildings WHERE (
	   ExtraLuxuries = 1
	OR Happiness <> 0
	OR UnmoddedHappiness <> 0
	OR UnhappinessModifier <> 0
	OR HappinessPerCity <> 0
	OR HappinessPerXPolicies <> 0
	OR CityCountUnhappinessMod <> 0
	OR NoOccupiedUnhappiness <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_RELIGION', 8
FROM Buildings WHERE (
	   FoundsReligion = 1
	OR IsReligious = 1
	OR HolyCity = 1
	OR ExtraMissionarySpreads <> 0
	OR ReligiousPressureModifier <> 0
);

/*
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_RELIGION', 4
FROM Buildings WHERE (
	   UnlockedByBelief = 1
);
*/










INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_GREAT_PEOPLE', 8
FROM Buildings WHERE (
	   GreatPeopleRateModifier <> 0
	OR GlobalGreatPeopleRateModifier <> 0
	OR GreatPersonExpendGold <> 0
	OR FreeGreatPeople <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_GREAT_PEOPLE', 4
FROM Buildings WHERE (
	   SpecialistType <> 0
	OR SpecialistExtraCulture <> 0
	OR GreatPeopleRateChange <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_DIPLOMACY', 8
FROM Buildings WHERE (
	   DiplomaticVoting = 1
	OR MinorFriendshipChange <> 0
	OR MinorFriendshipFlatChange <> 0
	OR ExtraLeagueVotes <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_ESPIONAGE', 8
FROM Buildings WHERE (
	   AffectSpiesNow = 1
	OR NullifyInfluenceModifier = 1
	OR Espionage <> 0
	OR EspionageModifier <> 0
	OR GlobalEspionageModifier <> 0
	OR ExtraSpies <> 0
	OR SpyRankChange <> 0
	OR InstantSpyRankChange <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_WATER_CONNECTION', 8
FROM Buildings WHERE (
	   AllowsWaterRoutes = 1
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_MILITARY_TRAINING', 8
FROM Buildings WHERE (
	   Experience <> 0
	OR GlobalExperience <> 0
	OR FreePromotion <> 0
	OR TrainedFreePromotion <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_OFFENSE', 8
FROM Buildings WHERE (
	   GreatGeneralRateModifier <> 0
	OR UnitUpgradeCostMod <> 0
	OR Airlift <> 0
	OR MilitaryProductionModifier <> 0
	OR InstantMilitaryIncrease <> 0
	OR NoOccupiedUnhappiness <> 0
	OR GreatGeneralRateChange <> 0
	OR ExperiencePerTurn <> 0
	OR GlobalExperienceFixed <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_DEFENSE', 8
FROM Buildings WHERE (
	   BorderObstacle = 1
	OR PlayerBorderObstacle = 1
	OR UnitUpgradeCostMod <> 0
	OR Airlift <> 0
	OR InstantMilitaryIncrease <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_CITY_DEFENSE', 8
FROM Buildings WHERE (
	   AirModifier <> 0
	OR NukeModifier <> 0
	OR HealRateChange <> 0
	OR Defense <> 0
	OR ExtraCityHitPoints <> 0
	OR GlobalDefenseMod <> 0
	OR CityWall <> 0
	OR AllowsRangeStrike <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_SPACESHIP', 8
FROM Buildings WHERE (
	   SpaceProductionModifier <> 0
	OR GlobalSpaceProductionModifier <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_WONDER', 8
FROM Buildings WHERE (
	   WonderProductionModifier <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_I_TRADE_DESTINATION', 8
FROM Buildings WHERE (
	   TradeRouteTargetBonus <> 0
	OR NumTradeRouteBonus <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_I_TRADE_ORIGIN', 8
FROM Buildings WHERE (
	   AllowsFoodTradeRoutes <> 0
	OR AllowsProductionTradeRoutes <> 0
	OR TradeRouteRecipientBonus <> 0
	OR NumTradeRouteBonus <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_I_LAND_TRADE_ROUTE', 8
FROM Buildings WHERE (
	   TradeRouteLandDistanceModifier <> 0
	OR TradeRouteLandGoldBonus <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT Type, 'FLAVOR_I_SEA_TRADE_ROUTE', 8
FROM Buildings WHERE (
	   TradeRouteSeaDistanceModifier <> 0
	OR TradeRouteSeaGoldBonus <> 0
);

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
SELECT building.Type, 'FLAVOR_WONDER', 8
FROM Buildings building, BuildingClasses class
WHERE (building.BuildingClass = class.Type AND (
	   class.MaxGlobalInstances = 1
	OR class.MaxTeamInstances = 1
	--OR class.MaxPlayerInstances = 1
));



-- This CEAI_Flavor_Buildings.sql data created by:
-- BuildingPriorities2 tab of CEP_LeadersAI spreadsheet (in mod folder).

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT building.BuildingType, yield.FlavorType, 8 FROM Building_AreaYieldModifiers              building, Yields yield WHERE building.YieldType = yield.Type;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT building.BuildingType, yield.FlavorType, 8 FROM Building_BuildingClassYieldChanges       building, Yields yield WHERE building.YieldType = yield.Type;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT building.BuildingType, yield.FlavorType, 8 FROM Building_GlobalYieldModifiers            building, Yields yield WHERE building.YieldType = yield.Type;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT building.BuildingType, yield.FlavorType, 8 FROM Building_ResourceYieldModifiers          building, Yields yield WHERE building.YieldType = yield.Type;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT building.BuildingType, yield.FlavorType, 8 FROM Building_RiverPlotYieldChanges           building, Yields yield WHERE building.YieldType = yield.Type;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT building.BuildingType, yield.FlavorType, 8 FROM Building_SeaPlotYieldChanges             building, Yields yield WHERE building.YieldType = yield.Type;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT building.BuildingType, yield.FlavorType, 8 FROM Building_LakePlotYieldChanges            building, Yields yield WHERE building.YieldType = yield.Type;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT building.BuildingType, yield.FlavorType, 8 FROM Building_SeaResourceYieldChanges         building, Yields yield WHERE building.YieldType = yield.Type;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT building.BuildingType, yield.FlavorType, 8 FROM Building_ResourceYieldChanges            building, Yields yield WHERE building.YieldType = yield.Type;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT building.BuildingType, yield.FlavorType, 8 FROM Building_FeatureYieldChanges             building, Yields yield WHERE building.YieldType = yield.Type;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT building.BuildingType, yield.FlavorType, 8 FROM Building_TerrainYieldChanges             building, Yields yield WHERE building.YieldType = yield.Type;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT building.BuildingType, yield.FlavorType, 8 FROM Building_SpecialistYieldChanges          building, Yields yield WHERE building.YieldType = yield.Type;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT building.BuildingType, yield.FlavorType, 8 FROM Building_YieldChangesPerPop              building, Yields yield WHERE building.YieldType = yield.Type;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT building.BuildingType, yield.FlavorType, 8 FROM Building_YieldChangesPerReligion         building, Yields yield WHERE building.YieldType = yield.Type;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT building.BuildingType, yield.FlavorType, 8 FROM Building_TechEnhancedYieldChanges        building, Yields yield WHERE building.YieldType = yield.Type;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT building.BuildingType, yield.FlavorType, 8 FROM Building_YieldModifiers                  building, Yields yield WHERE building.YieldType = yield.Type;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT building.BuildingType, yield.FlavorType, 8 FROM Building_YieldChanges                    building, Yields yield WHERE building.YieldType = yield.Type;

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT building.BuildingType, combat.FlavorType, 8 FROM Building_UnitCombatFreeExperiences       building, UnitCombatInfos combat WHERE building.UnitCombatType = combat.Type;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT building.BuildingType, combat.FlavorType, 8 FROM Building_UnitCombatProductionModifiers   building, UnitCombatInfos combat WHERE building.UnitCombatType = combat.Type;

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT BuildingType, 'FLAVOR_HAPPINESS'           , 8 FROM Building_BuildingClassHappiness;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT BuildingType, 'FLAVOR_EXPANSION'           , 8 FROM Building_BuildingClassHappiness;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT BuildingType, 'FLAVOR_MILITARY_TRAINING'   , 8 FROM Building_DomainFreeExperiences;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT BuildingType, 'FLAVOR_MILITARY_TRAINING'   , 8 FROM Building_DomainFreeExperiencePerGreatWork;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT BuildingType, 'FLAVOR_MILITARY_TRAINING'   , 4  FROM Building_DomainProductionModifiers;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT BuildingType, 'FLAVOR_GREAT_PEOPLE'        , 8 FROM Building_FreeSpecialistCounts;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT BuildingType, 'FLAVOR_GOLD'                , 8 FROM Building_HurryModifiers;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT BuildingType, 'FLAVOR_CULTURE'             , 8 FROM Building_ResourceCultureChanges;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT BuildingType, 'FLAVOR_RELIGION'            , 8 FROM Building_ResourceFaithChanges;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT BuildingType, 'FLAVOR_MILITARY_TRAINING'   , 8 FROM Building_UnitCombatFreeExperiences;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT BuildingType, 'FLAVOR_MILITARY_TRAINING'   , 4  FROM Building_UnitCombatProductionModifiers;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT BuildingType, 'FLAVOR_TOURISM'             , 8 FROM Building_ThemingBonuses;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT BuildingType, 'FLAVOR_NAVAL_GROWTH'        , 8 FROM Building_SeaPlotYieldChanges;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT BuildingType, 'FLAVOR_NAVAL_GROWTH'        , 8 FROM Building_SeaResourceYieldChanges;




--
-- Building Flavors: update flavor values
--

-- Port buildings	
UPDATE Building_Flavors
	SET FlavorType = 'FLAVOR_NAVAL_GROWTH'
	WHERE FlavorType = 'FLAVOR_GROWTH'
	AND BuildingType IN (SELECT Type FROM Buildings WHERE Water = 1);

/*
UPDATE Building_Flavors SET Flavor = 16;

UPDATE Building_Flavors SET Flavor = Flavor / 2
WHERE FlavorType IN (
	'FLAVOR_GREAT_PEOPLE'
) AND BuildingType NOT IN (SELECT building.Type
FROM Buildings building, BuildingClasses class
WHERE (building.BuildingClass = class.Type AND (
	   class.MaxGlobalInstances = 1
	OR class.MaxPlayerInstances = 1
	OR class.MaxTeamInstances = 1
)));
*/

-- Unique Buildings
UPDATE Building_Flavors SET Flavor = Flavor * 2
WHERE BuildingType IN (SELECT building.Type FROM Buildings building, BuildingClasses class WHERE (
	building.BuildingClass = class.Type
	AND building.Type <> class.DefaultBuilding
));


-- National Wonders
UPDATE Building_Flavors SET Flavor = Flavor * 2
	WHERE FlavorType NOT IN (
		'FLAVOR_CULTURE'		,
		'FLAVOR_GREAT_PEOPLE'	,
		'FLAVOR_WONDER'
	) AND BuildingType IN (SELECT building.Type
	FROM Buildings building, BuildingClasses class
	WHERE (building.BuildingClass = class.Type AND (
		   class.MaxPlayerInstances = 1
	)));

UPDATE Building_Flavors SET Flavor = 32
	WHERE FlavorType = 'FLAVOR_CULTURE'
	AND BuildingType IN (SELECT building.Type
	FROM Buildings building, BuildingClasses class
	WHERE (building.BuildingClass = class.Type
	AND   (class.MaxPlayerInstances = 1
	)
	AND   (building.InstantHappiness <> 0
		OR building.CultureRateModifier <> 0
		OR building.GlobalCultureRateModifier <> 0
		OR building.PolicyCostModifier <> 0
		OR building.FreePolicies <> 0
		OR building.XBuiltTriggersIdeologyChoice <> 0
		OR building.GreatWorkSlotType <> 0
		OR building.FreeGreatWork <> 0
		OR building.SpecialistExtraCulture <> 0
	)));

UPDATE Building_Flavors SET Flavor = 32
	WHERE FlavorType = 'FLAVOR_GREAT_PEOPLE'
	AND BuildingType IN (SELECT building.Type
	FROM Buildings building, BuildingClasses class
	WHERE (building.BuildingClass = class.Type
	AND   (class.MaxPlayerInstances = 1
	)
	AND   (building.GlobalGreatPeopleRateModifier <> 0
		OR building.FreeGreatPeople <> 0
		OR building.SpecialistExtraCulture <> 0
	)));


-- World Wonders
UPDATE Building_Flavors SET Flavor = Flavor * 4
	WHERE FlavorType NOT IN (
		'FLAVOR_CULTURE'		,
		'FLAVOR_GREAT_PEOPLE'	,
		'FLAVOR_WONDER'
	) AND BuildingType IN (SELECT building.Type
	FROM Buildings building, BuildingClasses class
	WHERE (building.BuildingClass = class.Type AND (
		   class.MaxGlobalInstances = 1
		OR class.MaxTeamInstances = 1
	)));

UPDATE Building_Flavors SET Flavor = 32
	WHERE FlavorType = 'FLAVOR_CULTURE'
	AND BuildingType IN (SELECT building.Type
	FROM Buildings building, BuildingClasses class
	WHERE (building.BuildingClass = class.Type
	AND   (class.MaxGlobalInstances = 1
		OR class.MaxTeamInstances = 1
	)
	AND   (building.InstantHappiness <> 0
		OR building.CultureRateModifier <> 0
		OR building.GlobalCultureRateModifier <> 0
		OR building.PolicyCostModifier <> 0
		OR building.FreePolicies <> 0
		OR building.XBuiltTriggersIdeologyChoice <> 0
		OR building.GreatWorkSlotType <> 0
		OR building.FreeGreatWork <> 0
		OR building.SpecialistExtraCulture <> 0
	)));

UPDATE Building_Flavors SET Flavor = 32
	WHERE FlavorType = 'FLAVOR_GREAT_PEOPLE'
	AND BuildingType IN (SELECT building.Type
	FROM Buildings building, BuildingClasses class
	WHERE (building.BuildingClass = class.Type
	AND   (class.MaxGlobalInstances = 1
		OR class.MaxTeamInstances = 1
	)
	AND   (building.GlobalGreatPeopleRateModifier <> 0
		OR building.FreeGreatPeople <> 0
		OR building.SpecialistExtraCulture <> 0
	)));

-- Specialist buildings
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
	SELECT building.Type, specFlavor.FlavorType, specFlavor.Flavor * building.SpecialistCount
	FROM Buildings building, SpecialistFlavors specFlavor
	WHERE specFlavor.SpecialistType = building.SpecialistType
	  AND building.SpecialistCount > 0
	;

-- GP Point buildings
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
	SELECT building.Type, specFlavor.FlavorType, specFlavor.Flavor * building.GreatPeopleRateChange / 2
	FROM Buildings building, SpecialistFlavors specFlavor
	WHERE specFlavor.SpecialistType = building.SpecialistType
	  AND building.GreatPeopleRateChange > 0
	;

-- Specific buildings
UPDATE Building_Flavors SET Flavor = Flavor * 2
	WHERE BuildingType = 'BUILDING_GREAT_LIBRARY'
	AND FlavorType = 'FLAVOR_SCIENCE';

-- This CEAI_Buildings.sql data automatically created by:
-- Buildings_Specific tab of "Good For Priorities" spreadsheet:
-- https://docs.google.com/spreadsheet/ccc?key=0Ap8Ehya83q19dFpPbnNDMFhmM2I2YjVXR04tQWN0V2c
INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_GRANARY',                  'FLAVOR_RES_GRAINS',               8);
INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_LIGHTHOUSE',               'FLAVOR_RES_SEA',                  8);
INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_CARAVANSARY',              'FLAVOR_RES_LUXURY',               8);
INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_BARRACKS',                 'FLAVOR_RES_STRATEGIC',            8);
INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_TEMPLE',                   'FLAVOR_RES_RELIGIOUS',            8);
INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_AQUEDUCT',                 'FLAVOR_RES_EXOTIC',               8);
INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_AMPHITHEATER',             'FLAVOR_RES_COSTUME',              8);
INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_STONE_WORKS',              'FLAVOR_RES_ORE',                  8);
INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_MINT',                     'FLAVOR_RES_CURRENCY',             8);
INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_MACHU_PICHU',              'FLAVOR_GROWTH',                   32);INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_MACHU_PICHU',              'FLAVOR_GOLD',                     32);INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_MACHU_PICHU',              'FLAVOR_CULTURE',                  32);INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_MACHU_PICHU',              'FLAVOR_RELIGION',                 32);
INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_HIMEJI_CASTLE',            'FLAVOR_DEFENSE',                  32);
INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_WAT_PHRA_KAEW',            'FLAVOR_GOLD',                     32);INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_WAT_PHRA_KAEW',            'FLAVOR_CULTURE',                  32);
INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_PANAMA_CANAL',             'FLAVOR_NAVAL_RECON',              32);
INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_HOLLYWOOD',                'FLAVOR_HAPPINESS',                32);
INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_NEUSCHWANSTEIN',           'FLAVOR_CITY_DEFENSE',             32);
INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_CRISTO_REDENTOR',          'FLAVOR_CULTURE',                  32);INSERT INTO Building_Flavors(BuildingType, FlavorType, Flavor) VALUES ('BUILDING_CRISTO_REDENTOR',          'FLAVOR_TOURISM',                  32);



/*
INSERT OR IGNORE INTO Building_Flavors(BuildingType, FlavorType, Flavor)
SELECT building.Type, flavor.FlavorType, 2 * flavor.Flavor
FROM Buildings building, Buildings buildingDefault, BuildingClasses class, Building_Flavors flavor
WHERE ( buildingDefault.BuildingClass	= building.BuildingClass
	AND buildingDefault.Type			<> building.Type
	AND buildingDefault.BuildingClass	= class.Type
	AND buildingDefault.Type			= class.DefaultBuilding
	AND buildingDefault.Type			= flavor.BuildingType
);
*/

-- Freebies

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
	SELECT building.Type, freeBuildingFlavor.FlavorType, freeBuildingFlavor.Flavor
	FROM Buildings building, Buildings freeBuilding, BuildingClasses freeBuildingClass, Building_Flavors freeBuildingFlavor
	WHERE freeBuildingClass.Type			= building.FreeBuildingThisCity
	  AND freeBuildingClass.DefaultBuilding	= freeBuilding.Type
	  AND freeBuildingFlavor.BuildingType	= freeBuilding.Type
	;

INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor)
	SELECT building.BuildingType, freeUnitFlavor.FlavorType, freeUnitFlavor.Flavor
	FROM Building_FreeUnits building, Unit_Flavors freeUnitFlavor
	WHERE freeUnitFlavor.UnitType	= building.UnitType
	;


-- Join duplicated flavors
DROP TABLE IF EXISTS CEP_Collisions;
CREATE TABLE CEP_Collisions(BuildingType text, FlavorType text, Flavor integer);
INSERT INTO CEP_Collisions (BuildingType, FlavorType, Flavor) SELECT BuildingType, FlavorType, MAX(Flavor) FROM Building_Flavors GROUP BY BuildingType, FlavorType;
DELETE FROM Building_Flavors;
INSERT INTO Building_Flavors (BuildingType, FlavorType, Flavor) SELECT BuildingType, FlavorType, Flavor FROM CEP_Collisions;
DROP TABLE CEP_Collisions;

-- Join duplicated flavors
DROP TABLE IF EXISTS CEP_Collisions;
CREATE TABLE CEP_Collisions(ProjectType text, FlavorType text, Flavor integer);
INSERT INTO CEP_Collisions (ProjectType, FlavorType, Flavor) SELECT ProjectType, FlavorType, MAX(Flavor) FROM Project_Flavors GROUP BY ProjectType, FlavorType;
DELETE FROM Project_Flavors;
INSERT INTO Project_Flavors (ProjectType, FlavorType, Flavor) SELECT ProjectType, FlavorType, Flavor FROM CEP_Collisions;
DROP TABLE CEP_Collisions;



INSERT INTO Building_Flavors_Human (BuildingType, FlavorType, Flavor) SELECT BuildingType, FlavorType, MAX(Flavor) FROM Building_Flavors GROUP BY BuildingType, FlavorType;



--
-- AI specific changes
--

DELETE FROM Building_Flavors
WHERE FlavorType = 'FLAVOR_WONDER'
AND BuildingType IN (
	SELECT BuildingType FROM Building_Flavors
	WHERE FlavorType IN (
		'FLAVOR_OFFENSE'			,
		'FLAVOR_MILITARY_TRAINING'	
	)
);

UPDATE Building_Flavors
SET Flavor = Flavor * 2
WHERE BuildingType IN (
	'BUILDING_STONEHENGE'		
);

UPDATE Building_Flavors
SET Flavor = Flavor / 8
WHERE BuildingType IN (
	'BUILDING_TERRACOTTA_ARMY'	,
	'BUILDING_PYRAMID'			,
	'BUILDING_TEMPLE_ARTEMIS'	
);

--x2 flavor values of non-wonder buildings
/*UPDATE Building_Flavors
SET Flavor = Flavor * 2
WHERE BuildingType IN (
	SELECT building.Type 
	FROM Buildings building, BuildingClasses class WHERE ( 
		building.BuildingClass = class.Type AND NOT(
			class.MaxGlobalInstances = 1 OR class.MaxTeamInstances = 1
		)
	)
);*/

-- Items no longer in the Buildings table
DELETE FROM Building_Flavors WHERE BuildingType NOT IN (SELECT Type FROM Buildings);

-- Dummy buildings automatically assigned to cities
DELETE FROM Building_Flavors WHERE BuildingType IN (SELECT Type FROM Buildings WHERE Cost = 0 OR Cost = -1);

-- Revert BNW Flavors
DELETE FROM Building_Flavors WHERE BuildingType IN (SELECT BuildingType FROM Building_Flavors_BNW);
INSERT INTO Building_Flavors SELECT * FROM Building_Flavors_BNW WHERE BuildingType IN (SELECT Type FROM Buildings);

UPDATE LoadedFile SET Value=1 WHERE Type='CEAI_Buildings.sql';