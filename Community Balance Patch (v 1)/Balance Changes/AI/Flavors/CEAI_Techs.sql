-- Insert SQL Rules Here 


--
-- Tech Priorities
--

DELETE FROM Technology_Flavors;
DELETE FROM Technology_Flavors_Human;


-- Units
	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT unit.PrereqTech, flavor.FlavorType, flavor.Flavor
	FROM Units unit, UnitClasses class, Unit_Flavors flavor
	WHERE ( unit.Type = flavor.UnitType
		AND unit.Type = class.DefaultUnit
		AND unit.Class = class.Type
		AND flavor.Flavor >= 2
	);

-- Buildings
	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT building.PrereqTech, flavor.FlavorType, flavor.Flavor * building.AIAvailability / 8
	FROM Buildings building, BuildingClasses class, Building_Flavors_Human flavor
	WHERE ( building.Type = flavor.BuildingType
		AND building.Type = class.DefaultBuilding
		AND building.BuildingClass = class.Type
		AND flavor.FlavorType <> 'FLAVOR_WONDER'
		AND (building.AIAvailability * flavor.Flavor / 8) >= 2
	);

-- Wonders
	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT building.PrereqTech, flavor.FlavorType, flavor.Flavor / 2
	FROM Buildings building, BuildingClasses class, Building_Flavors_Human flavor
	WHERE ( building.Type = flavor.BuildingType
		AND building.Type = class.DefaultBuilding
		AND building.BuildingClass = class.Type
		AND flavor.FlavorType = 'FLAVOR_WONDER'
	);

-- Projects
	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT project.TechPrereq, flavor.FlavorType, flavor.Flavor / 4
	FROM Projects project, Project_Flavors flavor
	WHERE (	project.Type = flavor.ProjectType AND flavor.Flavor / 4 >= 2 );

-- Processes
	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT process.TechPrereq, flavor.FlavorType, 4
	FROM Processes process, Process_ProductionYields yield, Yield_Flavors flavor
	WHERE (process.Type = yield.ProcessType AND yield.YieldType = flavor.YieldType);


-- Builds

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT build.PrereqTech, flavor.FlavorType, flavor.Flavor * build.AIAvailability / 8
	FROM Builds build, Build_Flavors flavor
	WHERE ( build.Type = flavor.BuildType
		AND build.AIAvailability > 0
	);

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.TechType, flavor.FlavorType, 2 * build.AIAvailability
	FROM Builds build, Improvements improve, Yield_Flavors flavor, Improvement_TechYieldChanges tech
	WHERE (build.ImprovementType = improve.Type AND tech.YieldType = flavor.YieldType AND tech.ImprovementType = improve.Type AND build.AIAvailability > 0);

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.TechType, flavor.FlavorType, 1 * build.AIAvailability
	FROM Builds build, Improvements improve, Yield_Flavors flavor, Improvement_TechFreshWaterYieldChanges tech
	WHERE (build.ImprovementType = improve.Type AND tech.YieldType = flavor.YieldType AND tech.ImprovementType = improve.Type AND build.AIAvailability > 0);

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.TechType, flavor.FlavorType, 1 * build.AIAvailability
	FROM Builds build, Improvements improve, Yield_Flavors flavor, Improvement_TechNoFreshWaterYieldChanges tech
	WHERE (build.ImprovementType = improve.Type AND tech.YieldType = flavor.YieldType AND tech.ImprovementType = improve.Type AND build.AIAvailability > 0);

	
	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.TechType, 'FLAVOR_TILE_IMPROVEMENT', 1 * build.AIAvailability
	FROM Builds build, Improvements improve, Improvement_TechYieldChanges tech
	WHERE (build.ImprovementType = improve.Type AND tech.ImprovementType = improve.Type AND build.AIAvailability > 0);

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.TechType, 'FLAVOR_TILE_IMPROVEMENT', 0.5 * build.AIAvailability
	FROM Builds build, Improvements improve, Improvement_TechFreshWaterYieldChanges tech
	WHERE (build.ImprovementType = improve.Type AND tech.ImprovementType = improve.Type AND build.AIAvailability > 0);

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.TechType, 'FLAVOR_TILE_IMPROVEMENT', 0.5 * build.AIAvailability
	FROM Builds build, Improvements improve, Improvement_TechNoFreshWaterYieldChanges tech
	WHERE (build.ImprovementType = improve.Type AND tech.ImprovementType = improve.Type AND build.AIAvailability > 0);


-- Other

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT 'TECH_FUTURE_TECH', Type, 16
	FROM Flavors WHERE Type IN ('FLAVOR_SCIENCE');

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT res.TechReveal, flavor.FlavorType, flavor.Flavor / 4
	FROM Resources res, Resource_Flavors flavor
	WHERE (res.Type = flavor.ResourceType AND res.TechReveal IS NOT NULL);

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.Type, flavor.Type, 4
	FROM Technologies tech, Flavors flavor
	WHERE (tech.AllowEmbassyTradingAllowed = 1 OR tech.OpenBordersTradingAllowed = 1)
	AND flavor.Type IN ('FLAVOR_OFFENSE', 'FLAVOR_GOLD', 'FLAVOR_DIPLOMACY');

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.Type, flavor.Type, 4
	FROM Technologies tech, Flavors flavor
	WHERE (tech.AllowsEmbarking = 1 OR tech.EmbarkedAllWaterPassage = 1 OR tech.EmbarkedMoveChange > 0)
	AND flavor.Type IN ('FLAVOR_OFFENSE', 'FLAVOR_NAVAL', 'FLAVOR_NAVAL_RECON', 'FLAVOR_EXPANSION');

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.Type, flavor.Type, 8
	FROM Technologies tech, Flavors flavor
	WHERE tech.EmbarkedAllWaterPassage = 1
	AND flavor.Type IN ('FLAVOR_NAVAL_RECON');

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.Type, flavor.Type, 8
	FROM Technologies tech, Flavors flavor
	WHERE tech.BridgeBuilding = 1 
	AND flavor.Type IN ('FLAVOR_EXPANSION');

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.Type, flavor.Type, 4
	FROM Technologies tech, Flavors flavor
	WHERE tech.BridgeBuilding = 1 
	AND flavor.Type IN ('FLAVOR_OFFENSE', 'FLAVOR_DEFENSE', 'FLAVOR_TILE_IMPROVEMENT');

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.TechType, flavor.Type, 8
	FROM Route_TechMovementChanges tech, Flavors flavor
	WHERE flavor.Type IN ('FLAVOR_EXPANSION');

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.TechType, flavor.Type, 4
	FROM Route_TechMovementChanges tech, Flavors flavor
	WHERE flavor.Type IN ('FLAVOR_OFFENSE', 'FLAVOR_DEFENSE', 'FLAVOR_TILE_IMPROVEMENT');

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.Type, flavor.Type, 8
	FROM Technologies tech, Flavors flavor
	WHERE tech.ResearchAgreementTradingAllowed = 1 
	AND flavor.Type IN ('FLAVOR_SCIENCE');

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.Type, flavor.Type, 4
	FROM Technologies tech, Flavors flavor
	WHERE tech.ResearchAgreementTradingAllowed = 1 
	AND flavor.Type IN ('FLAVOR_DIPLOMACY');

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.Type, flavor.Type, 32
	FROM Technologies tech, Flavors flavor
	WHERE tech.AllowsWorldCongress = 1 
	AND flavor.Type IN ('FLAVOR_DIPLOMACY');

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.Type, flavor.Type, 2
	FROM Technologies tech, Flavors flavor
	WHERE tech.DefensivePactTradingAllowed = 1 
	AND flavor.Type IN ('FLAVOR_DEFENSE', 'FLAVOR_DIPLOMACY');

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.Type, flavor.Type, 4
	FROM Technologies tech, Flavors flavor
	WHERE tech.MapVisible = 1 
	AND flavor.Type IN ('FLAVOR_OFFENSE');

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.Type, flavor.Type, 8
	FROM Technologies tech, Flavors flavor
	WHERE (tech.InternationalTradeRoutesChange = 1)
	AND flavor.Type IN ('FLAVOR_I_LAND_TRADE_ROUTE', 'FLAVOR_I_SEA_TRADE_ROUTE');

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.TechType, flavor.Type, 8
	FROM Technology_TradeRouteDomainExtraRange tech, Flavors flavor
	WHERE tech.DomainType = 'DOMAIN_LAND' AND flavor.Type IN ('FLAVOR_I_LAND_TRADE_ROUTE');

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.TechType, flavor.Type, 8
	FROM Technology_TradeRouteDomainExtraRange tech, Flavors flavor
	WHERE tech.DomainType = 'DOMAIN_SEA' AND flavor.Type IN ('FLAVOR_I_SEA_TRADE_ROUTE');

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.Type, flavor.Type, 156
	FROM Technologies tech, Flavors flavor
	WHERE (tech.InfluenceSpreadModifier <> 0)
	AND flavor.Type IN ('FLAVOR_TOURISM');

	INSERT OR IGNORE INTO Technology_Flavors(TechType, FlavorType, Flavor)
	SELECT tech.Type, flavor.Type, 64
	FROM Technologies tech, Flavors flavor
	WHERE (tech.ExtraVotesPerDiplomat <> 0)
	AND flavor.Type IN ('FLAVOR_DIPLOMACY');

-- Sum flavors for each FlavorType	
	/**/
	DROP TABLE IF EXISTS CEP_Collisions;
	CREATE TABLE CEP_Collisions(TechType text, FlavorType text, Flavor integer);
	INSERT INTO CEP_Collisions (TechType, FlavorType, Flavor) SELECT TechType, FlavorType, SUM(Flavor) FROM Technology_Flavors GROUP BY TechType, FlavorType;
	DELETE FROM Technology_Flavors;
	INSERT INTO Technology_Flavors (TechType, FlavorType, Flavor) SELECT TechType, FlavorType, Flavor FROM CEP_Collisions;
	DROP TABLE CEP_Collisions;





-- 
-- Adjust flavor visibility
-- 

	/**/
	UPDATE Technology_Flavors SET Flavor = 0.5 * Flavor;

	-- Concentrated in few objects - increase flavor
	UPDATE Technology_Flavors SET Flavor = 4 * Flavor WHERE FlavorType IN (
		'FLAVOR_WATER_CONNECTION'	
	);
	UPDATE Technology_Flavors SET Flavor = 2 * Flavor WHERE FlavorType IN (
		'FLAVOR_NAVAL_BOMBARDMENT'	,
		'FLAVOR_RANGED'				,
		'FLAVOR_SOLDIER'			,
		'FLAVOR_RECON'				,
		'FLAVOR_HEALING'			,
		'FLAVOR_PILLAGE'			,
		'FLAVOR_MELEE'				,
		'FLAVOR_SIEGE'				,
		'FLAVOR_ANTI_MOBILE'		,

		'FLAVOR_DIPLOMACY'			,
		'FLAVOR_ESPIONAGE'			,
		'FLAVOR_HAPPINESS'			,
		'FLAVOR_NAVAL_GROWTH'		,
		'FLAVOR_NUKE'				,
		'FLAVOR_SPACESHIP'			
	);
	UPDATE Technology_Flavors SET Flavor = 1.5 * Flavor WHERE FlavorType IN (
		'FLAVOR_CITY_DEFENSE'		,
		'FLAVOR_MOBILE'				,
		'FLAVOR_GROWTH'				,
		'FLAVOR_I_LAND_TRADE_ROUTE'	,
		'FLAVOR_I_SEA_TRADE_ROUTE'	
	);

	-- Average
	UPDATE Technology_Flavors SET Flavor = 1 * Flavor WHERE FlavorType IN (
		'FLAVOR_MILITARY_TRAINING'	,
		'FLAVOR_NAVAL'				,
		'FLAVOR_NAVAL_RECON'		,
		'FLAVOR_AIR'				,

		'FLAVOR_PRODUCTION'			,
		'FLAVOR_SCIENCE'			,

		'FLAVOR_NAVAL_TILE_IMPROVEMENT',
		'FLAVOR_INFRASTRUCTURE'		,
		'FLAVOR_EXPANSION'			,
		'FLAVOR_RELIGION'			,
		'FLAVOR_ARCHAEOLOGY'		,
		'FLAVOR_TILE_IMPROVEMENT'	
	);
	
	-- Spread among many objects - decrease flavor
	UPDATE Technology_Flavors SET Flavor = 0.75 * Flavor WHERE FlavorType IN (
		'FLAVOR_GOLD'				,
		'FLAVOR_CULTURE'			,
		'FLAVOR_TOURISM'			
	);
	UPDATE Technology_Flavors SET Flavor = 0.5 * Flavor WHERE FlavorType IN (
		'FLAVOR_GREAT_PEOPLE'		,
		'FLAVOR_DEFENSE'			,
		'FLAVOR_OFFENSE'
	);
	UPDATE Technology_Flavors SET Flavor = 0.33 * Flavor WHERE FlavorType IN (
		'FLAVOR_WONDER'				
	);





-- 
-- Planning
-- 


	/*
		Plan ahead for upcoming techs. For example:
		- Pottery (TechType)
		- leads to Sailing (Descendent with 16 naval flavor)
		- Pottery gets 8 naval flavor (Level 1 parent 16/2=8)
	*/

	-- AI code already includes planning
	INSERT INTO Technology_Flavors_Human (TechType, FlavorType, Flavor) SELECT TechType, FlavorType, SUM(Flavor) FROM Technology_Flavors GROUP BY TechType, FlavorType;

	/**/
	DROP TABLE IF EXISTS CEP_TechFlavorRipples;
	CREATE TABLE CEP_TechFlavorRipples(Descendent text, TechType text, FlavorType text, Flavor integer);

	-- Level 0 (descendent)
	INSERT OR IGNORE INTO CEP_TechFlavorRipples (TechType, FlavorType, Flavor) SELECT TechType, FlavorType, Flavor FROM Technology_Flavors_Human;

	-- Level 1 (parents)
	INSERT OR IGNORE INTO CEP_TechFlavorRipples(Descendent, TechType, FlavorType, Flavor)
	SELECT tech.TechType, techP1.PrereqTech, tech.FlavorType, tech.Flavor / 2.5
	FROM Technology_Flavors_Human tech, Technology_PrereqTechs techP1
	WHERE (tech.Flavor >= 2.5/2 AND tech.TechType = techP1.TechType);

	-- Level 2 (grandparents)
	INSERT OR IGNORE INTO CEP_TechFlavorRipples(Descendent, TechType, FlavorType, Flavor)
	SELECT tech.TechType, techP2.PrereqTech, tech.FlavorType, tech.Flavor / 6.25
	FROM Technology_Flavors_Human tech, Technology_PrereqTechs techP1, Technology_PrereqTechs techP2
	WHERE (tech.Flavor >= 6.25/2 AND tech.TechType = techP1.TechType AND techP1.PrereqTech = techP2.TechType);
	
	-- Level 3 (great-grandparents)
	INSERT OR IGNORE INTO CEP_TechFlavorRipples(Descendent, TechType, FlavorType, Flavor)
	SELECT tech.TechType, techP3.PrereqTech, tech.FlavorType, tech.Flavor / 15.6
	FROM Technology_Flavors_Human tech, Technology_PrereqTechs techP1, Technology_PrereqTechs techP2, Technology_PrereqTechs techP3
	WHERE (tech.Flavor >= 15.6/2 AND tech.TechType = techP1.TechType AND techP1.PrereqTech = techP2.TechType AND techP2.PrereqTech = techP3.TechType);

	-- Level 4 (great-great-grandparents)
	INSERT OR IGNORE INTO CEP_TechFlavorRipples(Descendent, TechType, FlavorType, Flavor)
	SELECT tech.TechType, techP4.PrereqTech, tech.FlavorType, tech.Flavor / 39.1
	FROM Technology_Flavors_Human tech, Technology_PrereqTechs techP1, Technology_PrereqTechs techP2, Technology_PrereqTechs techP3, Technology_PrereqTechs techP4
	WHERE (tech.Flavor >= 39.1/2 AND tech.TechType = techP1.TechType AND techP1.PrereqTech = techP2.TechType AND techP2.PrereqTech = techP3.TechType AND techP3.PrereqTech = techP4.TechType);
	

	-- Remove cases where a Tech leads to Descendent through multiple routes
	DROP TABLE IF EXISTS CEP_Collisions;
	CREATE TABLE CEP_Collisions(Descendent text, TechType text, FlavorType text, Flavor integer);
	INSERT INTO CEP_Collisions (Descendent, TechType, FlavorType, Flavor) SELECT Descendent, TechType, FlavorType, MAX(Flavor) FROM CEP_TechFlavorRipples GROUP BY Descendent, TechType, FlavorType;
	DROP TABLE CEP_TechFlavorRipples;

	-- Sum advance priorites
	DELETE FROM Technology_Flavors_Human;
	INSERT INTO Technology_Flavors_Human (TechType, FlavorType, Flavor) SELECT TechType, FlavorType, SUM(Flavor) FROM CEP_Collisions GROUP BY TechType, FlavorType;
	DROP TABLE CEP_Collisions;
	/**/





-- 
-- Round to nearest power of 2
-- 

	DELETE FROM Technology_Flavors WHERE Flavor < 2.83;
	--UPDATE Technology_Flavors SET Flavor =   1 WHERE (						Flavor <  1.42	) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	--UPDATE Technology_Flavors SET Flavor =   2 WHERE (  1.42 <= Flavor	AND	Flavor <  2.83	) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	UPDATE Technology_Flavors SET Flavor =   4 WHERE (  2.83 <= Flavor	AND	Flavor <  5.66	) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	UPDATE Technology_Flavors SET Flavor =   8 WHERE (  5.66 <= Flavor	AND	Flavor < 11.32	) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	UPDATE Technology_Flavors SET Flavor =  16 WHERE ( 11.32 <= Flavor	AND	Flavor < 22.63	) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	UPDATE Technology_Flavors SET Flavor =  32 WHERE ( 22.63 <= Flavor	AND	Flavor < 45.26	) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	UPDATE Technology_Flavors SET Flavor =  64 WHERE ( 45.26 <= Flavor	AND	Flavor < 90.51	) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	UPDATE Technology_Flavors SET Flavor = 128 WHERE ( 90.51 <= Flavor	AND	Flavor < 181.02	) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	UPDATE Technology_Flavors SET Flavor = 256 WHERE (181.02 <= Flavor	AND	Flavor < 362.04	) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	UPDATE Technology_Flavors SET Flavor = 512 WHERE (362.04 <= Flavor						) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	UPDATE Technology_Flavors SET Flavor = ROUND(Flavor) WHERE EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 1);

	DELETE FROM Technology_Flavors_Human WHERE Flavor < 2.83 AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	--UPDATE Technology_Flavors_Human SET Flavor =   1 WHERE (						Flavor <  1.42	) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	--UPDATE Technology_Flavors_Human SET Flavor =   2 WHERE (  1.42 <= Flavor	AND	Flavor <  2.83	) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	UPDATE Technology_Flavors_Human SET Flavor =   4 WHERE (  2.83 <= Flavor	AND	Flavor <  5.66	) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	UPDATE Technology_Flavors_Human SET Flavor =   8 WHERE (  5.66 <= Flavor	AND	Flavor < 11.32	) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	UPDATE Technology_Flavors_Human SET Flavor =  16 WHERE ( 11.32 <= Flavor	AND	Flavor < 22.63	) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	UPDATE Technology_Flavors_Human SET Flavor =  32 WHERE ( 22.63 <= Flavor	AND	Flavor < 45.26	) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	UPDATE Technology_Flavors_Human SET Flavor =  64 WHERE ( 45.26 <= Flavor	AND	Flavor < 90.51	) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	UPDATE Technology_Flavors_Human SET Flavor = 128 WHERE ( 90.51 <= Flavor	AND	Flavor < 181.02	) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	UPDATE Technology_Flavors_Human SET Flavor = 256 WHERE (181.02 <= Flavor	AND	Flavor < 362.04	) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	UPDATE Technology_Flavors_Human SET Flavor = 512 WHERE (362.04 <= Flavor						) AND EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 0);
	UPDATE Technology_Flavors_Human SET Flavor = ROUND(Flavor) WHERE EXISTS (SELECT * FROM Cep WHERE Type = 'SHOW_GOOD_FOR_RAW_NUMBERS' AND Value = 1);

	--UPDATE Technology_Flavors_Human SET Flavor = POWER( ROUND(LOG(Flavor)/LOG(2)), 1 ) -- Sqlite does not have power or log functions?!

	DELETE FROM Technology_Flavors WHERE TechType IS NULL;
	DELETE FROM Technology_Flavors_Human WHERE TechType IS NULL;

UPDATE LoadedFile SET Value=1 WHERE Type='CEAI_Techs.sql';