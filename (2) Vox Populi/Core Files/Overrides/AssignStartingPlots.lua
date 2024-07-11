------------------------------------------------------------------------------
--	FILE:	  AssignStartingPlots.lua
--	AUTHOR:   Bob Thomas
--	PURPOSE:  Civ5's new and improved start plot assignment method.
------------------------------------------------------------------------------
--	MODDER:   Barathor
------------------------------------------------------------------------------
--	REGIONAL DIVISION CONCEPT:   Bob Thomas
--	DIVISION ALGORITHM CONCEPT:  Ed Beach
--	CENTER BIAS CONCEPT:         Bob Thomas
--	RESOURCE BALANCING:          Bob Thomas
--	LUA IMPLEMENTATION:          Bob Thomas
------------------------------------------------------------------------------
--	Copyright (c) 2010 Firaxis Games, Inc. All rights reserved.
------------------------------------------------------------------------------

include("MapmakerUtilities");
include("NaturalWondersCustomMethods");

------------------------------------------------------------------------------
-- NOTE FOR MODDERS: There is a detailed Reference at the end of the file.
------------------------------------------------------------------------------

--                             FOREWORD

-- Jon wanted a lot of changes to terrain composition for Civ5. These have
-- had the effect of making different parts of each randomly generated map
-- more distinct, more unique, but it has also necessitated a complete
-- overhaul of where civs are placed on the map and how resources are
-- distributed. The new placements are much more precise, both for civs
-- and resources. As such, any modifications to terrain or resource types
-- will no longer be "plug and play" in the XML. Terrain modders will have
-- to work with this file as well as the XML, to integrate their mods in to
-- the new system.

-- Some civs will be purposely placed in difficult terrain, depending on what
-- a given map instance has to offer. Civs placed in tough environments will
-- receive specific amounts of assistance, primarily in the form of Bonus food
-- from Wheat, Cows, Deer, Bananas, or Fish. This part of the new system is
-- very precisely calibrated and balanced, so be aware that any changes or
-- additions to how resources are placed near start points will have a
-- dramatic effect on the game, and could pose challenges of a sort that were
-- not present in the sphere of Civ4 modding.

-- The Luxury resources are also carefully calibrated. In a given game, some
-- will be clustered near a small number of civs (perhaps even a monopoly
-- given to one). Some will be placed only near City States, requiring civs
-- to go through a City State, one way or another, to obtain any instances of
-- that luxury type. Some will be left up to fate, appearing randomly in
-- whatever is their normal habitat. Yet others may be oversupplied or perhaps
-- even absent from a given game. Which luxuries fall in to which category
-- will be scrambled, to keep players guessing, and to help further the sense
-- of curiosity when exploring a new map in a new game.

-- Bob Thomas  -  April 16, 2010

-- Note from VP dev:
-- "Area" is used instead of "Landmass" throughout all map-related files thanks to Civ5 devs.
-- A Landmass is a contiguous set of land (or water) plots,
-- which may contain multiple Areas which are separated by isthmuses or mountain ranges.
-- This causes problems in Terra-styled start placement where all civs are placed in the biggest area instead of the biggest landmass,
-- leaving half of the landmass empty and everyone clumped closely.

-- A project is in progress to change most references of "Area" to "Landmass",
-- starting from this file (ASP) and will be extended to VP-supported maps.
-- A side effect is that a region in Terra-styled maps can now span across mountain ranges or isthmuses, making its monopoly hard to get.
-- Forcing a region to abide to area boundaries is currently too much work, unfortunately. Maybe later.
-- Regions in Continents-styled maps will still be bounded by areas, but now isolated starts should hopefully be rarer.

-- azum4roll - 2024-01-14

-- There is a Reference section at the end of the file.

------------------------------------------------------------------------------
------------------------------------------------------------------------------
AssignStartingPlots = {};

-- Enum values for region division methods
RegionDivision = {
	BIGGEST_LANDMASS = 1,
	CONTINENTAL = 2,
	RECTANGULAR = 3,
	RECTANGULAR_SELF_DEFINED = 4,
};

-- Enum values for region types
RegionTypes = {
	REGION_UNDEFINED = 0,
	REGION_TUNDRA = 1,
	REGION_JUNGLE = 2,
	REGION_FOREST = 3,
	REGION_DESERT = 4,
	REGION_HILLS = 5,
	REGION_PLAINS = 6,
	REGION_GRASSLAND = 7,
	REGION_HYBRID = 8,
	REGION_MOUNTAIN = 9,
	REGION_SNOW = 10,
};

-- Enum values for impact layers
ImpactLayers = {
	LAYER_NONE = -1,
	LAYER_IRON = 1,
	LAYER_LUXURY = 2,
	LAYER_BONUS = 3,
	LAYER_FISH = 4,
	LAYER_CITY_STATE = 5,
	LAYER_NATURAL_WONDER = 6,
	LAYER_MARBLE = 7,
	LAYER_IVORY = 8,
	LAYER_HORSE = 9,
	LAYER_COAL = 10,
	LAYER_OIL = 11,
	LAYER_ALUMINUM = 12,
	LAYER_URANIUM = 13,
};

-- Enum values for plot lists
PlotListTypes = {
	UNKNOWN = -1,
	LAND = 0,
	FLAT = 1,
	HILLS = 2,
	MOUNTAIN = 3,
	OCEAN = 4,
	COAST = 5,
	LAKE = 6,
	SNOW = 7,
	DESERT = 8,
	TUNDRA = 9,
	PLAINS = 10,
	GRASS = 11,
	FOREST = 12,
	JUNGLE = 13,
	FLOOD_PLAINS = 14,
	MARSH = 15,
	ICE = 16,
	-- Resource-specific lists
	WHEAT = 100,
	RICE = 101,
	MAIZE = 102,
	COCONUT = 103,
	MARBLE = 104,
	-- Complex lists
	COAST_NEXT_TO_LAND = 1000,
	FLAT_NO_FEATURE = 1100,
	FLAT_COVERED = 1101,
	FLAT_FOREST = 1102,
	FLAT_JUNGLE = 1103,
	TROPICAL_MARSH = 1104,
	FLAT_SNOW = 1110,
	FLAT_DESERT_NO_OASIS = 1120,
	FLAT_DESERT_NO_FEATURE = 1121,
	FLAT_DESERT_WET = 1122,
	FLAT_TUNDRA = 1130,
	FLAT_TUNDRA_NO_FEATURE = 1131,
	FLAT_TUNDRA_FOREST = 1132,
	FLAT_PLAINS_NO_FEATURE = 1140,
	FLAT_PLAINS_DRY_NO_FEATURE = 1141,
	FLAT_PLAINS_WET_NO_FEATURE = 1142,
	FLAT_PLAINS_COVERED = 1143,
	FLAT_GRASS_NO_FEATURE = 1150,
	FLAT_GRASS_DRY_NO_FEATURE = 1151,
	FLAT_GRASS_WET_NO_FEATURE = 1152,
	FLAT_DESERT_TUNDRA_NO_FEATURE = 1160,
	FLAT_TUNDRA_PLAINS_COVERED = 1170,
	FLAT_PLAINS_GRASS_NO_FEATURE = 1180,
	FLAT_PLAINS_GRASS_COVERED = 1181,
	FLAT_PLAINS_GRASS_FOREST = 1182,
	FLAT_TUNDRA_PLAINS_GRASS_NO_FEATURE = 1190,
	HILLS_NO_FEATURE = 1200,
	HILLS_COVERED = 1201,
	HILLS_FOREST = 1202,
	HILLS_JUNGLE = 1203,
	HILLS_SNOW = 1210,
	HILLS_DESERT = 1220,
	HILLS_TUNDRA_NO_FEATURE = 1230,
	HILLS_TUNDRA_FOREST = 1231,
	HILLS_PLAINS_NO_FEATURE = 1240,
	HILLS_PLAINS_COVERED = 1241,
	HILLS_GRASS_NO_FEATURE = 1250,
	HILLS_DESERT_TUNDRA_NO_FEATURE = 1260,
	HILLS_DESERT_PLAINS_NO_FEATURE = 1270,
	HILLS_TUNDRA_PLAINS_COVERED = 1280,
	HILLS_PLAINS_GRASS_NO_FEATURE = 1290,
	HILLS_PLAINS_GRASS_COVERED = 1291,
	HILLS_DESERT_TUNDRA_PLAINS_NO_FEATURE = 1300,
	HILLS_DESERT_PLAINS_GRASS_NO_FEATURE = 1310,
	HILLS_TUNDRA_PLAINS_GRASS_NO_FEATURE = 1320,
	TUNDRA_FOREST = 1400,
	PLAINS_GRASS_FOREST = 1410,
};

NUM_REGION_TYPES = 11;
NUM_IMPACT_LAYERS = 13;
MAX_MAJOR_CIVS = GameDefines.MAX_MAJOR_CIVS;
MAX_MINOR_CIVS = GameDefines.MAX_MINOR_CIVS;
------------------------------------------------------------------------------
--[[
WARNING: There must not be any recalculation of AreaIDs at any time during the execution of any operations in or attached to this table.
Recalculating will invalidate all data based on AreaID relationships with plots, which will destroy Regional definitions for all but the Rectangular method.
A fix for scrambled AreaID data is theoretically possible, but I have not spent development resources and time on this, directing attention to other tasks.
--]]
------------------------------------------------------------------------------
function AssignStartingPlots.Create()
	--[[
	There are three methods of dividing the map in to regions.
	OneLandmass, Continents, Oceanic. Default method is Continents.

	Standard start plot finding uses a regional division method, then assigns one civ per region.
	Regions with lowest average fertility get their assignment first, to avoid the poor getting poorer.

	Default methods for civ and city state placement both rely on having regional division data.
	If the desired process for a given map script would not define regions of this type, replace the start finder with your custom method.
	--]]

	-- Main data table ("self dot" table).
	-- Scripters have the opportunity to replace member methods without having to replace the entire process.
	local findStarts = {

		-- Core Process member methods
		__Init = AssignStartingPlots.__Init,
		__InitLuxuryWeights = AssignStartingPlots.__InitLuxuryWeights,
		__CustomInit = AssignStartingPlots.__CustomInit,
		ApplyHexAdjustment = AssignStartingPlots.ApplyHexAdjustment,
		GenerateRegions = AssignStartingPlots.GenerateRegions,
		ChooseLocations = AssignStartingPlots.ChooseLocations,
		BalanceAndAssign = AssignStartingPlots.BalanceAndAssign,
		PlaceNaturalWonders = AssignStartingPlots.PlaceNaturalWonders,
		PlaceResourcesAndCityStates = AssignStartingPlots.PlaceResourcesAndCityStates,

		-- Generate Regions member methods
		MeasureStartPlacementFertilityOfPlot = AssignStartingPlots.MeasureStartPlacementFertilityOfPlot,
		MeasureStartPlacementFertilityInRectangle = AssignStartingPlots.MeasureStartPlacementFertilityInRectangle,
		MeasureStartPlacementFertilityOfLandmass = AssignStartingPlots.MeasureStartPlacementFertilityOfLandmass,
		RemoveDeadRows = AssignStartingPlots.RemoveDeadRows,
		DivideIntoRegions = AssignStartingPlots.DivideIntoRegions,
		ChopIntoThreeRegions = AssignStartingPlots.ChopIntoThreeRegions,
		ChopIntoTwoRegions = AssignStartingPlots.ChopIntoTwoRegions,
		CustomOverride = AssignStartingPlots.CustomOverride,

		-- Choose Locations member methods
		MeasureTerrainInRegions = AssignStartingPlots.MeasureTerrainInRegions,
		DetermineRegionTypes = AssignStartingPlots.DetermineRegionTypes,
		PlaceImpactAndRipples = AssignStartingPlots.PlaceImpactAndRipples,
		MeasureSinglePlot = AssignStartingPlots.MeasureSinglePlot,
		EvaluateCandidatePlot = AssignStartingPlots.EvaluateCandidatePlot,
		IterateThroughCandidatePlotList = AssignStartingPlots.IterateThroughCandidatePlotList,
		FindStart = AssignStartingPlots.FindStart,
		FindCoastalStart = AssignStartingPlots.FindCoastalStart,
		FindStartWithoutRegardToAreaID = AssignStartingPlots.FindStartWithoutRegardToAreaID,

		-- Balance and Assign member methods
		AttemptToPlaceBonusResourceAtPlot = AssignStartingPlots.AttemptToPlaceBonusResourceAtPlot,
		AttemptToPlaceHillsAtPlot = AssignStartingPlots.AttemptToPlaceHillsAtPlot,
		AttemptToPlaceSmallStrategicAtPlot = AssignStartingPlots.AttemptToPlaceSmallStrategicAtPlot,
		FindFallbackForUnmatchedRegionPriority = AssignStartingPlots.FindFallbackForUnmatchedRegionPriority,
		AddStrategicBalanceResources = AssignStartingPlots.AddStrategicBalanceResources,
		AttemptToPlaceCattleAtGrassPlot = AssignStartingPlots.AttemptToPlaceCattleAtGrassPlot,
		AttemptToPlaceStoneAtGrassPlot = AssignStartingPlots.AttemptToPlaceStoneAtGrassPlot,
		NormalizeStartLocation = AssignStartingPlots.NormalizeStartLocation,
		NormalizeTeamLocations = AssignStartingPlots.NormalizeTeamLocations,

		-- Natural Wonders member methods
		ExaminePlotForNaturalWondersEligibility = AssignStartingPlots.ExaminePlotForNaturalWondersEligibility,
		ExamineCandidatePlotForNaturalWondersEligibility = AssignStartingPlots.ExamineCandidatePlotForNaturalWondersEligibility,
		CanBeThisNaturalWonderType = AssignStartingPlots.CanBeThisNaturalWonderType,
		GenerateLocalVersionsOfDataFromXML = AssignStartingPlots.GenerateLocalVersionsOfDataFromXML,
		GenerateNaturalWondersCandidatePlotLists = AssignStartingPlots.GenerateNaturalWondersCandidatePlotLists,
		AttemptToPlaceNaturalWonder = AssignStartingPlots.AttemptToPlaceNaturalWonder,

		-- City States member methods
		AssignCityStatesToRegionsOrToUninhabited = AssignStartingPlots.AssignCityStatesToRegionsOrToUninhabited,
		CanPlaceCityStateAt = AssignStartingPlots.CanPlaceCityStateAt,
		ObtainNextSectionInRegion = AssignStartingPlots.ObtainNextSectionInRegion,
		PlaceCityState = AssignStartingPlots.PlaceCityState,
		PlaceCityStateInRegion = AssignStartingPlots.PlaceCityStateInRegion,
		PlaceCityStates = AssignStartingPlots.PlaceCityStates, -- Dependent on AssignLuxuryRoles being executed first, so beware.
		NormalizeCityState = AssignStartingPlots.NormalizeCityState,
		NormalizeCityStateLocations = AssignStartingPlots.NormalizeCityStateLocations, -- Dependent on PlaceLuxuries being executed first.

		-- Resources member methods
		GenerateGlobalResourcePlotLists = AssignStartingPlots.GenerateGlobalResourcePlotLists,
		PlaceResourceImpact = AssignStartingPlots.PlaceResourceImpact, -- Note: called from PlaceImpactAndRipples
		ProcessResourceList = AssignStartingPlots.ProcessResourceList,
		PlaceSpecificNumberOfResources = AssignStartingPlots.PlaceSpecificNumberOfResources,
		IdentifyRegionsOfThisType = AssignStartingPlots.IdentifyRegionsOfThisType,
		SortRegionsByType = AssignStartingPlots.SortRegionsByType,
		AssignLuxuryToRegion = AssignStartingPlots.AssignLuxuryToRegion,
		GetLuxuriesSplitCap = AssignStartingPlots.GetLuxuriesSplitCap, -- New for Expansion, because we have more luxuries now.
		GetCityStateLuxuriesTargetNumber = AssignStartingPlots.GetCityStateLuxuriesTargetNumber, -- New for Expansion
		GetDisabledLuxuriesTargetNumber = AssignStartingPlots.GetDisabledLuxuriesTargetNumber,
		GetRandomLuxuriesTargetNumber = AssignStartingPlots.GetRandomLuxuriesTargetNumber, -- MOD.Barathor: New
		AssignLuxuryRoles = AssignStartingPlots.AssignLuxuryRoles,
		GetListOfAllowableLuxuriesAtCitySite = AssignStartingPlots.GetListOfAllowableLuxuriesAtCitySite,
		GenerateLuxuryPlotListsAtCitySite = AssignStartingPlots.GenerateLuxuryPlotListsAtCitySite, -- Also doubles as Ice Removal.
		GenerateLuxuryPlotListsInRegion = AssignStartingPlots.GenerateLuxuryPlotListsInRegion,
		GetIndicesForLuxuryType = AssignStartingPlots.GetIndicesForLuxuryType,
		GetRegionLuxuryTargetNumbers = AssignStartingPlots.GetRegionLuxuryTargetNumbers,
		GetWorldLuxuryTargetNumbers = AssignStartingPlots.GetWorldLuxuryTargetNumbers,
		PlaceMarble = AssignStartingPlots.PlaceMarble,
		PlaceIvory = AssignStartingPlots.PlaceIvory,
		PlaceLuxuries = AssignStartingPlots.PlaceLuxuries,
		PlaceSmallQuantitiesOfStrategics = AssignStartingPlots.PlaceSmallQuantitiesOfStrategics,
		PlaceFish = AssignStartingPlots.PlaceFish,
		PlaceSexyBonusAtCivStarts = AssignStartingPlots.PlaceSexyBonusAtCivStarts,
		AddExtraBonusesToHillsRegions = AssignStartingPlots.AddExtraBonusesToHillsRegions,
		AddModernMinorStrategicsToCityStates = AssignStartingPlots.AddModernMinorStrategicsToCityStates,
		PlaceOilInTheSea = AssignStartingPlots.PlaceOilInTheSea,
		PrintFinalResourceTotalsToLog = AssignStartingPlots.PrintFinalResourceTotalsToLog,
		GetMajorStrategicResourceQuantityValues = AssignStartingPlots.GetMajorStrategicResourceQuantityValues,
		GetSmallStrategicResourceQuantityValues = AssignStartingPlots.GetSmallStrategicResourceQuantityValues,
		PlaceStrategicAndBonusResources = AssignStartingPlots.PlaceStrategicAndBonusResources,

		-- Extra functions for VP
		AdjustTiles = AssignStartingPlots.AdjustTiles,
		PlaceBonusResources = AssignStartingPlots.PlaceBonusResources,
		IsEvenMoreResourcesActive = AssignStartingPlots.IsEvenMoreResourcesActive,
		IsAloeVeraResourceActive = AssignStartingPlots.IsAloeVeraResourceActive,
		IsReducedSupplyActive = AssignStartingPlots.IsReducedSupplyActive,
		Plot_GetPlotsInCircle = AssignStartingPlots.Plot_GetPlotsInCircle,
		Plot_GetFertilityInRange = AssignStartingPlots.Plot_GetFertilityInRange,
		Plot_GetFertility = AssignStartingPlots.Plot_GetFertility,
		IsBetween = AssignStartingPlots.IsBetween,
		GetRandomMultiplier = AssignStartingPlots.GetRandomMultiplier,
		GetRandomFromRangeInclusive = AssignStartingPlots.GetRandomFromRangeInclusive,
		Constrain = AssignStartingPlots.Constrain,
		PlaceStrategicResourceImpact = AssignStartingPlots.PlaceStrategicResourceImpact,
		CountTotalStrategicResourceImpact = AssignStartingPlots.CountTotalStrategicResourceImpact,
		IsImpactLayerStrategic = AssignStartingPlots.IsImpactLayerStrategic,
		GenerateResourcePlotListsFromSpecificPlots = AssignStartingPlots.GenerateResourcePlotListsFromSpecificPlots,
		IsTropical = AssignStartingPlots.IsTropical,
		GetNumNaturalWondersToPlace = AssignStartingPlots.GetNumNaturalWondersToPlace,
		GenerateCandidatePlotListsForSpecificNW = AssignStartingPlots.GenerateCandidatePlotListsForSpecificNW,
		GetNumCityStatesPerRegion = AssignStartingPlots.GetNumCityStatesPerRegion,
		GetNumCityStatesInUninhabitedRegion = AssignStartingPlots.GetNumCityStatesInUninhabitedRegion,
		AssignCityStatesToRegions = AssignStartingPlots.AssignCityStatesToRegions,
		AttemptToPlaceTreesAtResourcePlot = AssignStartingPlots.AttemptToPlaceTreesAtResourcePlot,
		GetLowFertilityCompensations = AssignStartingPlots.GetLowFertilityCompensations,
		MeasureStartPlacementFertilityOfArea = AssignStartingPlots.MeasureStartPlacementFertilityOfArea,
		GetLandmassBoundaries = AssignStartingPlots.GetLandmassBoundaries,
	};

	findStarts:__Init();

	findStarts:__InitLuxuryWeights();

	-- Entry point for easy overrides, for instance if only a couple things need to change.
	findStarts:__CustomInit();

	return findStarts;
end
------------------------------------------------------------------------------
function AssignStartingPlots:__Init()
	-- Note that this operation relies on inclusion of the Mapmaker Utilities.
	local iW, iH = Map.GetGridSize();

	-- Set up data tables that record whether a plot is coastal land and whether a plot is adjacent to coastal land.
	self.plotDataIsCoastal, self.plotDataIsNextToCoast = GenerateNextToCoastalLandDataTables();

	-- Set up data for resource ID shortcuts.
	self.MAX_RESOURCE_INDEX = 0;
	for resource_data in GameInfo.Resources() do
		local resourceID = resource_data.ID;
		local resourceType = resource_data.Type;
		if resourceID > self.MAX_RESOURCE_INDEX then
			self.MAX_RESOURCE_INDEX = resourceID;
		end

		-- Set up Bonus IDs
		if resourceType == "RESOURCE_WHEAT" then
			self.wheat_ID = resourceID;
		elseif resourceType == "RESOURCE_COW" then
			self.cow_ID = resourceID;
		elseif resourceType == "RESOURCE_DEER" then
			self.deer_ID = resourceID;
		elseif resourceType == "RESOURCE_BANANA" then
			self.banana_ID = resourceID;
		elseif resourceType == "RESOURCE_FISH" then
			self.fish_ID = resourceID;
		elseif resourceType == "RESOURCE_SHEEP" then
			self.sheep_ID = resourceID;
		elseif resourceType == "RESOURCE_STONE" then
			self.stone_ID = resourceID;
		elseif resourceType == "RESOURCE_BISON" then
			self.bison_ID = resourceID;
		elseif resourceType == "RESOURCE_MAIZE" then -- new farm resources for VP
			self.maize_ID = resourceID;
		elseif resourceType == "RESOURCE_RICE" then -- new farm resources for VP
			self.rice_ID = resourceID;
		-- Set up Strategic IDs
		elseif resourceType == "RESOURCE_IRON" then
			self.iron_ID = resourceID;
		elseif resourceType == "RESOURCE_HORSE" then
			self.horse_ID = resourceID;
		elseif resourceType == "RESOURCE_COAL" then
			self.coal_ID = resourceID;
		elseif resourceType == "RESOURCE_OIL" then
			self.oil_ID = resourceID;
		elseif resourceType == "RESOURCE_ALUMINUM" then
			self.aluminum_ID = resourceID;
		elseif resourceType == "RESOURCE_URANIUM" then
			self.uranium_ID = resourceID;
		-- Set up Luxury IDs
		elseif resourceType == "RESOURCE_WHALE" then
			self.whale_ID = resourceID;
		elseif resourceType == "RESOURCE_PEARLS" then
			self.pearls_ID = resourceID;
		elseif resourceType == "RESOURCE_IVORY" then
			self.ivory_ID = resourceID;
		elseif resourceType == "RESOURCE_FUR" then
			self.fur_ID = resourceID;
		elseif resourceType == "RESOURCE_SILK" then
			self.silk_ID = resourceID;
		elseif resourceType == "RESOURCE_DYE" then
			self.dye_ID = resourceID;
		elseif resourceType == "RESOURCE_SPICES" then
			self.spices_ID = resourceID;
		elseif resourceType == "RESOURCE_SUGAR" then
			self.sugar_ID = resourceID;
		elseif resourceType == "RESOURCE_COTTON" then
			self.cotton_ID = resourceID;
		elseif resourceType == "RESOURCE_WINE" then
			self.wine_ID = resourceID;
		elseif resourceType == "RESOURCE_INCENSE" then
			self.incense_ID = resourceID;
		elseif resourceType == "RESOURCE_GOLD" then
			self.gold_ID = resourceID;
		elseif resourceType == "RESOURCE_SILVER" then
			self.silver_ID = resourceID;
		elseif resourceType == "RESOURCE_GEMS" then
			self.gems_ID = resourceID;
		elseif resourceType == "RESOURCE_MARBLE" then
			self.marble_ID = resourceID;
		-- Set up Expansion Pack Luxury IDs
		elseif resourceType == "RESOURCE_COPPER" then
			self.copper_ID = resourceID;
		elseif resourceType == "RESOURCE_SALT" then
			self.salt_ID = resourceID;
		elseif resourceType == "RESOURCE_CITRUS" then
			self.citrus_ID = resourceID;
		elseif resourceType == "RESOURCE_TRUFFLES" then
			self.truffles_ID = resourceID;
		elseif resourceType == "RESOURCE_CRAB" then
			self.crab_ID = resourceID;
		elseif resourceType == "RESOURCE_COCOA" then
			self.cocoa_ID = resourceID;
		-- Mod Luxury IDs
		elseif resourceType == "RESOURCE_COFFEE" then
			self.coffee_ID = resourceID;
		elseif resourceType == "RESOURCE_TEA" then
			self.tea_ID = resourceID;
		elseif resourceType == "RESOURCE_TOBACCO" then
			self.tobacco_ID = resourceID;
		elseif resourceType == "RESOURCE_AMBER" then
			self.amber_ID = resourceID;
		elseif resourceType == "RESOURCE_JADE" then
			self.jade_ID = resourceID;
		elseif resourceType == "RESOURCE_OLIVE" then
			self.olives_ID = resourceID;
		elseif resourceType == "RESOURCE_PERFUME" then
			self.perfume_ID = resourceID;
		elseif resourceType == "RESOURCE_CORAL" then
			self.coral_ID = resourceID;
		elseif resourceType == "RESOURCE_LAPIS" then
			self.lapis_ID = resourceID;
		-- Even More Resources for Vox Populi (luxuries)
		elseif resourceType == "RESOURCE_LAVENDER" then
			self.lavender_ID = resourceID;
		elseif resourceType == "RESOURCE_OBSIDIAN" then
			self.obsidian_ID = resourceID;
		elseif resourceType == "RESOURCE_PLATINUM" then
			self.platinum_ID = resourceID;
		elseif resourceType == "RESOURCE_POPPY" then
			self.poppy_ID = resourceID;
		elseif resourceType == "RESOURCE_TIN" then
			self.tin_ID = resourceID;
		-- Even More Resources for Vox Populi (bonus)
		elseif resourceType == "RESOURCE_COCONUT" then
			self.coconut_ID = resourceID;
		elseif resourceType == "RESOURCE_HARDWOOD" then
			self.hardwood_ID = resourceID;
		elseif resourceType == "RESOURCE_LEAD" then
			self.lead_ID = resourceID;
		elseif resourceType == "RESOURCE_PINEAPPLE" then
			self.pineapple_ID = resourceID;
		elseif resourceType == "RESOURCE_POTATO" then
			self.potato_ID = resourceID;
		elseif resourceType == "RESOURCE_RUBBER" then
			self.rubber_ID = resourceID;
		elseif resourceType == "RESOURCE_SULFUR" then
			self.sulfur_ID = resourceID;
		elseif resourceType == "RESOURCE_TITANIUM" then
			self.titanium_ID = resourceID;
		-- Aloe Vera for Vox Populi
		elseif resourceType == "RESOURCE_JAR_ALOE_VERA" then
			self.aloevera_ID = resourceID;
		end
	end

	-- Support custom map resource settings
	self.resDensity = 2;
	self.resSize = 2;
	self.legStart = false;
	self.resBalance = false;
	self.bonusDensity = 2;
	self.luxuryDensity = 2;

	-- Civ start position variables
	self.startingPlots = {}; -- Stores x and y coordinates (and "score") of starting plots for civs, indexed by region number
	self.method = RegionDivision.CONTINENTAL; -- Method of regional division, default is continental
	self.iNumCivs = 0; -- Number of civs at game start
	self.iNumFrontiers = 0; -- Number of empty regions
	self.bArea = false; -- Whether region division is based on landmass or area (true = area-based)
	self.landmass_areas = {}; -- Stores lists of areas of each landmass
	self.player_ID_list = {}; -- Correct list of player IDs (includes handling of any 'gaps' that occur in MP games)
	self.regionData = {}; -- Stores data returned from regional division algorithm
	self.regionTerrainCounts = {}; -- Stores counts of terrain elements for all regions
	self.regionTypes = {}; -- Stores region types
	self.distanceData = table.fill(0, iW * iH); -- Stores "impact and ripple" data of start points as each is placed
	self.playerCollisionData = table.fill(false, iW * iH); -- Stores "impact" data only, of start points, to avoid player collisions
	self.startLocationConditions = {}; -- Stores info regarding conditions at each start location
	self.hasMountainBias = false; -- Stores whether there's a civ that prefers to start near mountains
	self.hasSnowBias = false; -- Stores whether there's a civ that prefers to start near snow

	-- Natural Wonders variables
	self.wonder_list = {};
	self.eligibility_lists = {};
	self.xml_row_numbers = {};

	-- City States variables
	self.cityStatePlots = {}; -- Stores x and y coordinates, and region number, of city state sites
	self.iNumCityStates = 0; -- Number of city states at game start
	self.iNumCityStatesUnassigned = 0; -- Number of City States still in need of placement method assignment
	self.city_state_region_assignments = table.fill(-1, MAX_MINOR_CIVS); -- Stores region number of each city state (-1 if not in a region)
	self.uninhabited_areas_coastal_plots = {}; -- For use in placing city states outside of Regions
	self.uninhabited_areas_inland_plots = {};
	self.iNumCityStatesDiscarded = 0; -- If a city state cannot be placed without being too close to another start, it will be discarded
	self.city_state_validity_table = table.fill(false, MAX_MINOR_CIVS); -- Value set to true when a given city state is successfully assigned a start plot

	-- Resources variables
	self.amounts_of_resources_placed = table.fill(0, self.MAX_RESOURCE_INDEX + 1); -- Stores amounts of each resource ID placed. WARNING: This table uses adjusted resource ID (+1) to account for Lua indexing. Add 1 to all IDs to index this table.
	self.luxury_assignment_count = table.fill(0, self.MAX_RESOURCE_INDEX); -- Stores amount of each luxury type assigned to regions. WARNING: current implementation will crash if a Luxury is attached to resource ID 0 (default = iron), because this table uses unadjusted resource ID as table index.
	self.luxury_region_weights = {}; -- Stores weighted assignments for the types of regions
	self.luxury_fallback_weights = {}; -- In case all options for a given region type got assigned or disabled, also used for Undefined regions
	self.luxury_city_state_weights = {}; -- Stores weighted assignments for city state exclusive luxuries
	self.impactData = {}; -- Stores "impact and ripple" data in ALL layers
	self.regions_sorted_by_type = {}; -- Stores table that includes region number and Luxury ID (this is where the two are first matched)
	self.region_luxury_assignment = {}; -- Stores luxury assignments, keyed by region number.
	self.global_resource_plot_lists = {}; -- Table of plot lists to be used for distribution of resources across the map
	self.resourceIDs_assigned_to_regions = {};
	self.resourceIDs_assigned_to_cs = {};
	self.resourceIDs_assigned_to_special_case = {};
	self.resourceIDs_assigned_to_random = {};
	self.iNumTypesAssignedToRegions = 0;
	self.iNumTypesRandom = 0;
	self.totalLuxPlacedSoFar = 0;
	self.realtotalLuxPlacedSoFar = 0;
	-- MOD.Barathor: This displays the "real" total for luxuries and not the bugged one which also adds some strategics and bonuses which use the PlaceSpecificNumberOfResources placement method that luxuries use.
	-- Note: This bug doesn't affect gameplay and things are placed as intended since all luxuries are placed first, before other resource types. This just affects the debug printout data.

	-- Positioner defaults. These are the controls for the "Center Bias" placement method for civ starts in regions.
	self.centerBias = 34; -- % of radius from region center to examine first
	self.middleBias = 67; -- % of radius from region center to check second
	self.minFoodInner = 1;
	self.minProdInner = 0;
	self.minGoodInner = 3;
	self.minFoodMiddle = 4;
	self.minProdMiddle = 0;
	self.minGoodMiddle = 6;
	self.minFoodOuter = 4;
	self.minProdOuter = 2;
	self.minGoodOuter = 8;
	self.maxJunk = 9;

	-- Hex Adjustment tables.
	-- These tables direct plot by plot scans in a radius around a center hex, starting from Northeast, moving clockwise.
	self.firstRingYIsEven = {{0, 1}, {1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};
	self.secondRingYIsEven = {
		{1, 2}, {1, 1}, {2, 0}, {1, -1}, {1, -2}, {0, -2},
		{-1, -2}, {-2, -1}, {-2, 0}, {-2, 1}, {-1, 2}, {0, 2},
	};
	self.thirdRingYIsEven = {
		{1, 3}, {2, 2}, {2, 1}, {3, 0}, {2, -1}, {2, -2},
		{1, -3}, {0, -3}, {-1, -3}, {-2, -3}, {-2, -2}, {-3, -1},
		{-3, 0}, {-3, 1}, {-2, 2}, {-2, 3}, {-1, 3}, {0, 3},
	};
	self.firstRingYIsOdd = {{1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, 0}, {0, 1}};
	self.secondRingYIsOdd = {
		{1, 2}, {2, 1}, {2, 0}, {2, -1}, {1, -2}, {0, -2},
		{-1, -2}, {-1, -1}, {-2, 0}, {-1, 1}, {-1, 2}, {0, 2},
	};
	self.thirdRingYIsOdd = {
		{2, 3}, {2, 2}, {3, 1}, {3, 0}, {3, -1}, {2, -2},
		{2, -3}, {1, -3}, {0, -3}, {-1, -3}, {-2, -2}, {-2, -1},
		{-3, 0}, {-2, 1}, {-2, 2}, {-1, 3}, {0, 3}, {1, 3},
	};

	-- Direction types table, another method of handling hex adjustments, in combination with Map.PlotDirection()
	self.direction_types = {
		DirectionTypes.DIRECTION_NORTHEAST,
		DirectionTypes.DIRECTION_EAST,
		DirectionTypes.DIRECTION_SOUTHEAST,
		DirectionTypes.DIRECTION_SOUTHWEST,
		DirectionTypes.DIRECTION_WEST,
		DirectionTypes.DIRECTION_NORTHWEST,
	};

	-- Local arrays for storing Natural Wonder Placement XML data
	self.EligibilityMethodNumber = {};
	self.OccurrenceFrequency = {};
	self.RequireBiggestLandmass = {};
	self.AvoidBiggestLandmass = {};
	self.RequireFreshWater = {};
	self.AvoidFreshWater = {};
	self.LandBased = {};
	self.RequireLandAdjacentToOcean = {};
	self.AvoidLandAdjacentToOcean = {};
	self.RequireLandOnePlotInland = {};
	self.AvoidLandOnePlotInland = {};
	self.RequireLandTwoOrMorePlotsInland = {};
	self.AvoidLandTwoOrMorePlotsInland = {};
	self.CoreTileCanBeAnyPlotType = {};
	self.CoreTileCanBeFlatland = {};
	self.CoreTileCanBeHills = {};
	self.CoreTileCanBeMountain = {};
	self.CoreTileCanBeOcean = {};
	self.CoreTileCanBeAnyTerrainType = {};
	self.CoreTileCanBeGrass = {};
	self.CoreTileCanBePlains = {};
	self.CoreTileCanBeDesert = {};
	self.CoreTileCanBeTundra = {};
	self.CoreTileCanBeSnow = {};
	self.CoreTileCanBeShallowWater = {};
	self.CoreTileCanBeDeepWater = {};
	self.CoreTileCanBeAnyFeatureType = {};
	self.CoreTileCanBeNoFeature = {};
	self.CoreTileCanBeForest = {};
	self.CoreTileCanBeJungle = {};
	self.CoreTileCanBeOasis = {};
	self.CoreTileCanBeFloodPlains = {};
	self.CoreTileCanBeMarsh = {};
	self.CoreTileCanBeIce = {};
	self.CoreTileCanBeAtoll = {};
	self.AdjacentTilesCareAboutPlotTypes = {};
	self.AdjacentTilesAvoidAnyland = {};
	self.AdjacentTilesRequireFlatland = {};
	self.RequiredNumberOfAdjacentFlatland = {};
	self.AdjacentTilesRequireHills = {};
	self.RequiredNumberOfAdjacentHills = {};
	self.AdjacentTilesRequireMountain = {};
	self.RequiredNumberOfAdjacentMountain = {};
	self.AdjacentTilesRequireHillsPlusMountains = {};
	self.RequiredNumberOfAdjacentHillsPlusMountains = {};
	self.AdjacentTilesRequireOcean = {};
	self.RequiredNumberOfAdjacentOcean = {};
	self.AdjacentTilesAvoidFlatland = {};
	self.MaximumAllowedAdjacentFlatland = {};
	self.AdjacentTilesAvoidHills = {};
	self.MaximumAllowedAdjacentHills = {};
	self.AdjacentTilesAvoidMountain = {};
	self.MaximumAllowedAdjacentMountain = {};
	self.AdjacentTilesAvoidHillsPlusMountains = {};
	self.MaximumAllowedAdjacentHillsPlusMountains = {};
	self.AdjacentTilesAvoidOcean = {};
	self.MaximumAllowedAdjacentOcean = {};
	self.AdjacentTilesCareAboutTerrainTypes = {};
	self.AdjacentTilesRequireGrass = {};
	self.RequiredNumberOfAdjacentGrass = {};
	self.AdjacentTilesRequirePlains = {};
	self.RequiredNumberOfAdjacentPlains = {};
	self.AdjacentTilesRequireDesert = {};
	self.RequiredNumberOfAdjacentDesert = {};
	self.AdjacentTilesRequireTundra = {};
	self.RequiredNumberOfAdjacentTundra = {};
	self.AdjacentTilesRequireSnow = {};
	self.RequiredNumberOfAdjacentSnow = {};
	self.AdjacentTilesRequireShallowWater = {};
	self.RequiredNumberOfAdjacentShallowWater = {};
	self.AdjacentTilesRequireDeepWater = {};
	self.RequiredNumberOfAdjacentDeepWater = {};
	self.AdjacentTilesAvoidGrass = {};
	self.MaximumAllowedAdjacentGrass = {};
	self.AdjacentTilesAvoidPlains = {};
	self.MaximumAllowedAdjacentPlains = {};
	self.AdjacentTilesAvoidDesert = {};
	self.MaximumAllowedAdjacentDesert = {};
	self.AdjacentTilesAvoidTundra = {};
	self.MaximumAllowedAdjacentTundra = {};
	self.AdjacentTilesAvoidSnow = {};
	self.MaximumAllowedAdjacentSnow = {};
	self.AdjacentTilesAvoidShallowWater = {};
	self.MaximumAllowedAdjacentShallowWater = {};
	self.AdjacentTilesAvoidDeepWater = {};
	self.MaximumAllowedAdjacentDeepWater = {};
	self.AdjacentTilesCareAboutFeatureTypes = {};
	self.AdjacentTilesRequireNoFeature = {};
	self.RequiredNumberOfAdjacentNoFeature = {};
	self.AdjacentTilesRequireForest = {};
	self.RequiredNumberOfAdjacentForest = {};
	self.AdjacentTilesRequireJungle = {};
	self.RequiredNumberOfAdjacentJungle = {};
	self.AdjacentTilesRequireOasis = {};
	self.RequiredNumberOfAdjacentOasis = {};
	self.AdjacentTilesRequireFloodPlains = {};
	self.RequiredNumberOfAdjacentFloodPlains = {};
	self.AdjacentTilesRequireMarsh = {};
	self.RequiredNumberOfAdjacentMarsh = {};
	self.AdjacentTilesRequireIce = {};
	self.RequiredNumberOfAdjacentIce = {};
	self.AdjacentTilesRequireAtoll = {};
	self.RequiredNumberOfAdjacentAtoll = {};
	self.AdjacentTilesAvoidNoFeature = {};
	self.MaximumAllowedAdjacentNoFeature = {};
	self.AdjacentTilesAvoidForest = {};
	self.MaximumAllowedAdjacentForest = {};
	self.AdjacentTilesAvoidJungle = {};
	self.MaximumAllowedAdjacentJungle = {};
	self.AdjacentTilesAvoidOasis = {};
	self.MaximumAllowedAdjacentOasis = {};
	self.AdjacentTilesAvoidFloodPlains = {};
	self.MaximumAllowedAdjacentFloodPlains = {};
	self.AdjacentTilesAvoidMarsh = {};
	self.MaximumAllowedAdjacentMarsh = {};
	self.AdjacentTilesAvoidIce = {};
	self.MaximumAllowedAdjacentIce = {};
	self.AdjacentTilesAvoidAtoll = {};
	self.MaximumAllowedAdjacentAtoll = {};
	self.TileChangesMethodNumber = {};
	self.ChangeCoreTileToMountain = {};
	self.ChangeCoreTileToFlatland = {};
	self.ChangeCoreTileTerrainToGrass = {};
	self.ChangeCoreTileTerrainToPlains = {};
	self.SetAdjacentTilesToShallowWater = {};

	-- Init impact tables
	for i = 1, NUM_IMPACT_LAYERS do
		self.impactData[i] = table.fill(0, iW * iH);
	end

	-- Populate landmass/area relationship table here, since many maps override GenerateRegions() and we cannot do it there.
	local landmass_IDs = {};
	local area_IDs = {};
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local plot = Map.GetPlot(x, y);
			if not plot:IsWater() then -- Land plot, process it.
				local iLandmass = plot:GetLandmass();
				local iArea = plot:GetArea();
				if not TestMembership(landmass_IDs, iLandmass) then -- This plot is the first detected in its landmass.
					table.insert(landmass_IDs, iLandmass);
					self.landmass_areas[iLandmass] = {};
				end
				if not TestMembership(area_IDs, iArea) then -- This plot is the first detected in its area.
					table.insert(area_IDs, iArea);
					table.insert(self.landmass_areas[iLandmass], iArea);
					-- print("Area#", iArea, "is a new area in landmass#", iLandmass);
				end
			end
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:__InitLuxuryWeights()
	-- Initialize luxury data table. Index == Region Type
	-- Customize this function if the terrain will fall significantly
	-- outside Earth normal, or if region definitions have been modified.

	-- Note: The water-based luxuries are set to appear in a region only if that region has its start on the coast.
	-- So the weights shown for those are reduced in practice to the degree that a map has inland starts.

	-- MOD.Barathor: All region weights below have been updated.
	-- MOD.Barathor: Start
	self.luxury_region_weights[1] = { -- Tundra
		{self.fur_ID,		60},
		{self.amber_ID,		35},
		{self.silver_ID,	30},
		{self.copper_ID,	20},
		{self.salt_ID,		15},
		{self.gems_ID,		10},
		{self.jade_ID,		10},
		{self.lapis_ID,		10},
		{self.whale_ID,		10},
		{self.crab_ID,		10},
		{self.pearls_ID,	10},
		{self.coral_ID,		10},
	};

	-- MOD.HungryForFood: Start
	if self:IsEvenMoreResourcesActive() then
		table.insert(self.luxury_region_weights[1], {self.obsidian_ID,	10});
		table.insert(self.luxury_region_weights[1], {self.platinum_ID,	10});
		table.insert(self.luxury_region_weights[1], {self.tin_ID,		10});
	end
	-- MOD.HungryForFood: End

	-- Added for snow bias
	self.luxury_region_weights[10] = self.luxury_region_weights[1];

	self.luxury_region_weights[2] = { -- Jungle
		{self.citrus_ID,	35},
		{self.cocoa_ID,		35},
		{self.spices_ID,	35},
		{self.sugar_ID,		35},
		{self.truffles_ID,	15},
		{self.silk_ID,		10},
		{self.dye_ID,		10},
		{self.gems_ID,		10},
		{self.whale_ID,		10},
		{self.crab_ID,		10},
		{self.pearls_ID,	10},
		{self.coral_ID,		10},
	};

	-- MOD.HungryForFood: Start
	if self:IsEvenMoreResourcesActive() then
		table.insert(self.luxury_region_weights[2], {self.obsidian_ID,	35});
		table.insert(self.luxury_region_weights[2], {self.poppy_ID,		35});
		table.insert(self.luxury_region_weights[2], {self.platinum_ID,	10});
		table.insert(self.luxury_region_weights[2], {self.tin_ID,		10});
	end
	-- MOD.HungryForFood: End

	self.luxury_region_weights[3] = { -- Forest
		{self.truffles_ID,	40},
		{self.silk_ID,		35},
		{self.dye_ID,		35},
		{self.citrus_ID,	15},
		{self.cocoa_ID,		15},
		{self.spices_ID,	15},
		{self.whale_ID,		8},
		{self.crab_ID,		8},
		{self.pearls_ID,	8},
		{self.coral_ID,		8},
	};

	-- MOD.HungryForFood: Start
	if self:IsEvenMoreResourcesActive() then
		table.insert(self.luxury_region_weights[3], {self.lavender_ID,	15});
	end
	-- MOD.HungryForFood: End

	self.luxury_region_weights[4] = { -- Desert
		{self.incense_ID,	35},
		{self.lapis_ID,		35},
		{self.gold_ID,		20},
		{self.silver_ID,	20},
		{self.salt_ID,		15},
		{self.jade_ID,		15},
		{self.copper_ID,	10},
		{self.amber_ID,		10},
		{self.gems_ID,		10},
		{self.sugar_ID,		10},
		{self.cotton_ID,	10},
		{self.whale_ID,		10},
		{self.crab_ID,		10},
		{self.pearls_ID,	10},
		{self.coral_ID,		10},
	};

	-- MOD.HungryForFood: Start
	if self:IsEvenMoreResourcesActive() then
		table.insert(self.luxury_region_weights[4], {self.obsidian_ID,	10});
		table.insert(self.luxury_region_weights[4], {self.platinum_ID,	10});
		table.insert(self.luxury_region_weights[4], {self.tin_ID,		10});
	end
	-- MOD.HungryForFood: End

	self.luxury_region_weights[5] = { -- Hills
		{self.gold_ID,		20},
		{self.copper_ID,	20},
		{self.gems_ID,		20},
		{self.jade_ID,		20},
		{self.salt_ID,		10},
		{self.amber_ID,		10},
		{self.lapis_ID,		10},
		{self.whale_ID,		6},
		{self.crab_ID,		6},
		{self.pearls_ID,	6},
		{self.coral_ID,		6},
	};

	-- MOD.HungryForFood: Start
	if self:IsEvenMoreResourcesActive() then
		table.insert(self.luxury_region_weights[5], {self.obsidian_ID,	20});
		table.insert(self.luxury_region_weights[5], {self.poppy_ID,		20});
		table.insert(self.luxury_region_weights[5], {self.platinum_ID,	20});
		table.insert(self.luxury_region_weights[5], {self.tin_ID,		20});
	end
	-- MOD.HungryForFood: End

	-- Added for mountain bias
	self.luxury_region_weights[9] = self.luxury_region_weights[5];

	self.luxury_region_weights[6] = { -- Plains
		{self.wine_ID,		40},
		{self.olives_ID,	40},
		{self.coffee_ID,	40},
		{self.incense_ID,	20},
		{self.cotton_ID,	20},
		{self.salt_ID,		15},
		{self.tobacco_ID,	15},
		{self.tea_ID,		15},
		{self.perfume_ID,	15},
		{self.whale_ID,		11},
		{self.crab_ID,		11},
		{self.pearls_ID,	11},
		{self.coral_ID,		11},
	};

	-- MOD.HungryForFood: Start
	if self:IsEvenMoreResourcesActive() then
		table.insert(self.luxury_region_weights[6], {self.lavender_ID,	40});
		table.insert(self.luxury_region_weights[6], {self.poppy_ID,		40});
	end
	-- MOD.HungryForFood: End

	self.luxury_region_weights[7] = { -- Grass
		{self.tobacco_ID,	50},
		{self.tea_ID,		50},
		{self.perfume_ID,	50},
		{self.cotton_ID,	35},
		{self.wine_ID,		20},
		{self.olives_ID,	20},
		{self.coffee_ID,	20},
		{self.sugar_ID,		10},
		{self.gold_ID,		10},
		{self.whale_ID,		14},
		{self.crab_ID,		14},
		{self.pearls_ID,	14},
		{self.coral_ID,		14},
	};

	-- MOD.HungryForFood: Start
	if self:IsEvenMoreResourcesActive() then
		table.insert(self.luxury_region_weights[7], {self.lavender_ID,	50});
		table.insert(self.luxury_region_weights[7], {self.poppy_ID,		50});
	end
	-- MOD.HungryForFood: End

	self.luxury_region_weights[8] = { -- Hybrid
		{self.gold_ID,		30},      -- MOD.Barathor: Favor very flexible resources, like resources that are mined or in the water.
		{self.copper_ID,	30},
		{self.gems_ID,		30},
		{self.salt_ID,		30},
		{self.jade_ID,		30},
		{self.amber_ID,		30},
		{self.lapis_ID,		30},
		{self.coffee_ID,	05},
		{self.tobacco_ID,	05},
		{self.tea_ID,		05},
		{self.perfume_ID,	05},
		{self.cotton_ID,	05},
		{self.wine_ID,		05},
		{self.olives_ID,	05},
		{self.truffles_ID,	05},
		{self.silk_ID,		05},
		{self.dye_ID,		05},
		{self.citrus_ID,	05},
		{self.cocoa_ID,		05},
		{self.spices_ID,	05},
		{self.whale_ID,		20},
		{self.crab_ID,		20},
		{self.pearls_ID,	20},
		{self.coral_ID,		20},
	};

	-- MOD.HungryForFood: Start
	if self:IsEvenMoreResourcesActive() then
		table.insert(self.luxury_region_weights[8], {self.obsidian_ID,	30});
		table.insert(self.luxury_region_weights[8], {self.platinum_ID,	30});
		table.insert(self.luxury_region_weights[8], {self.tin_ID,		30});
		table.insert(self.luxury_region_weights[8], {self.lavender_ID,	05});
		table.insert(self.luxury_region_weights[8], {self.poppy_ID,		05});
	end
	-- MOD.HungryForFood: End

	self.luxury_fallback_weights = { -- Random / Fallback
		{self.gold_ID,		10},     -- MOD.Barathor: Favor water resources since they work great as randoms and make the coasts more interesting.
		{self.copper_ID,	10},     -- Also, slightly favor mined resources for their flexibility.
		{self.gems_ID,		10},
		{self.salt_ID,		10},
		{self.jade_ID,		10},
		{self.amber_ID,		10},
		{self.lapis_ID,		10},
		{self.coffee_ID,	05},
		{self.tobacco_ID,	05},
		{self.tea_ID,		05},
		{self.perfume_ID,	05},
		{self.cotton_ID,	05},
		{self.wine_ID,		05},
		{self.olives_ID,	05},
		{self.incense_ID,	05},
		{self.truffles_ID,	05},
		{self.silk_ID,		05},
		{self.dye_ID,		05},
		{self.citrus_ID,	05},
		{self.cocoa_ID,		05},
		{self.spices_ID,	05},
		{self.whale_ID,		30},
		{self.crab_ID,		30},
		{self.pearls_ID,	30},
		{self.coral_ID,		30},
	};

	-- MOD.HungryForFood: Start
	if self:IsEvenMoreResourcesActive() then
		table.insert(self.luxury_fallback_weights, {self.obsidian_ID,	30});
		table.insert(self.luxury_fallback_weights, {self.platinum_ID,	30});
		table.insert(self.luxury_fallback_weights, {self.tin_ID,		30});
		table.insert(self.luxury_fallback_weights, {self.lavender_ID,	05});
		table.insert(self.luxury_fallback_weights, {self.poppy_ID,		05});
	end
	-- MOD.HungryForFood: End

	self.luxury_city_state_weights = { -- City States
		{self.gold_ID,		10},       -- MOD.Barathor: Slightly favor water resources since they're flexible and most city-states are coastal.
		{self.silver_ID,	05},       -- Also, slightly favor mined resources for their flexibility.
		{self.copper_ID,	10},
		{self.gems_ID,		10},
		{self.salt_ID,		10},
		{self.jade_ID,		10},
		{self.amber_ID,		10},
		{self.lapis_ID,		10},
		{self.coffee_ID,	05},
		{self.tobacco_ID,	05},
		{self.tea_ID,		05},
		{self.perfume_ID,	05},
		{self.cotton_ID,	05},
		{self.wine_ID,		05},
		{self.olives_ID,	05},
		{self.incense_ID,	05},
		{self.truffles_ID,	05},
		{self.silk_ID,		05},
		{self.dye_ID,		05},
		{self.fur_ID,		05},
		{self.citrus_ID,	05},
		{self.cocoa_ID,		05},
		{self.spices_ID,	05},
		{self.sugar_ID,		05},
		{self.whale_ID,		10},
		{self.crab_ID,		10},
		{self.pearls_ID,	10},
		{self.coral_ID,		10},
	};

	-- MOD.HungryForFood: Start
	if self:IsEvenMoreResourcesActive() then
		table.insert(self.luxury_city_state_weights, {self.obsidian_ID,	10});
		table.insert(self.luxury_city_state_weights, {self.platinum_ID,	10});
		table.insert(self.luxury_city_state_weights, {self.tin_ID,		10});
		table.insert(self.luxury_city_state_weights, {self.lavender_ID,	05});
		table.insert(self.luxury_city_state_weights, {self.poppy_ID,	05});
	end
	-- MOD.HungryForFood: End
	-- MOD.Barathor: End
end
------------------------------------------------------------------------------
function AssignStartingPlots:__CustomInit()
	-- This function included to provide a quick and easy override for changing any initial settings.
	-- Add your customized version to the map script.
end
------------------------------------------------------------------------------
function AssignStartingPlots:ApplyHexAdjustment(x, y, plot_adjustments)
	-- Used this bit of code so many times, I had to make it a function.
	local iW, iH = Map.GetGridSize();
	local adjusted_x, adjusted_y;
	if Map:IsWrapX() then
		adjusted_x = (x + plot_adjustments[1]) % iW;
	else
		adjusted_x = x + plot_adjustments[1];
	end
	if Map:IsWrapY() then
		adjusted_y = (y + plot_adjustments[2]) % iH;
	else
		adjusted_y = y + plot_adjustments[2];
	end
	return adjusted_x, adjusted_y;
end
------------------------------------------------------------------------------
-- Start of functions tied to GenerateRegions()
------------------------------------------------------------------------------
function AssignStartingPlots:MeasureStartPlacementFertilityOfPlot(x, y, checkForCoastalLand)
	-- Fertility of plots is used to divide landmasses into Regions.
	-- Regions are used to assign starting plots and place some resources.
	-- Usage: x, y are plot coords, with 0, 0 in SW. The check is a boolean.

	-- Mountain, Oasis, FloodPlain tiles = -2, 4, 5 and do not count anything else.
	-- Rest of the tiles add up values of tile traits.
	-- Terrain: Grass 3, Plains 4, Tundra 2, Coast 2, Desert 1, Snow -1
	-- Features: Hill 1, Forest 0, FreshWater 1, River 1, Jungle -1, Marsh -2, Ice -1
	-- We want players who start in Grass to have the least room to expand.

	-- If you modify the terrain values or add or remove any terrain elements, you will need to add or modify processes here to accomodate your changes.
	-- Please be aware that the default process includes numerous assumptions that your terrain changes may invalidate, so you may need to rebalance the system.

	local plot = Map.GetPlot(x, y);
	local plotFertility = 0;
	local plotType = plot:GetPlotType();
	local terrainType = plot:GetTerrainType();
	local featureType = plot:GetFeatureType();
	-- Measure Fertility -- Any cases absent from the process have a 0 value.
	if plotType == PlotTypes.PLOT_MOUNTAIN then
		plotFertility = -1;
	elseif terrainType == TerrainTypes.TERRAIN_SNOW then
		plotFertility = -1;
	elseif featureType == FeatureTypes.FEATURE_OASIS then
		plotFertility = 4; -- Reducing Oasis value slightly. -1/26/2011 BT
	elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
		plotFertility = 5; -- Reducing Flood Plains value slightly. -1/26/2011 BT
	else
		if terrainType == TerrainTypes.TERRAIN_GRASS then -- Reversing values for Grass and Plains. -1/26/2011 BT
			plotFertility = 3;
		elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
			plotFertility = 4;
		elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
			plotFertility = 2;
		elseif terrainType == TerrainTypes.TERRAIN_COAST then
			plotFertility = 2;
		elseif terrainType == TerrainTypes.TERRAIN_DESERT then
			plotFertility = 1;
		end
		if plotType == PlotTypes.PLOT_HILLS then
			plotFertility = plotFertility + 1;
		end
		if featureType == FeatureTypes.FEATURE_FOREST then
			plotFertility = plotFertility + 0; -- Removing forest bonus as a balance tweak. -1/26/2011 BT
		elseif featureType == FeatureTypes.FEATURE_JUNGLE then
			plotFertility = plotFertility - 1;
		elseif featureType == FeatureTypes.FEATURE_MARSH then
			plotFertility = plotFertility - 2; -- Increasing penalty for Marsh plots. -1/26/2011 BT
		elseif featureType == FeatureTypes.FEATURE_ICE then
			plotFertility = plotFertility - 2;
		end
		if plot:IsRiverSide() then
			plotFertility = plotFertility + 1;
		end
		if plot:IsFreshWater() then
			plotFertility = plotFertility + 1;
		end
		if checkForCoastalLand then -- When measuring only one landmass, this shortcut helps account for coastal plots not measured.
			if plot:IsCoastalLand() then
				plotFertility = plotFertility + 2;
			end
		end
	end

	return plotFertility;
end
------------------------------------------------------------------------------
function AssignStartingPlots:MeasureStartPlacementFertilityInRectangle(iWestX, iSouthY, iWidth, iHeight)
	-- This function is designed to provide initial data for regional division recursion.
	-- Loop through plots in this rectangle and measure Fertility Rating.
	-- Results will include a data table of all measured plots.
	local landmassFertilityTable = {};
	local landmassFertilityCount = 0;
	local plotCount = iWidth * iHeight;
	for y = iSouthY, iSouthY + iHeight - 1 do -- When generating a plot data table incrementally, process Y first so that plots go row by row.
		for x = iWestX, iWestX + iWidth - 1 do
			local plotFertility = self:MeasureStartPlacementFertilityOfPlot(x, y, false); -- Check for coastal land is disabled.
			table.insert(landmassFertilityTable, plotFertility);
			landmassFertilityCount = landmassFertilityCount + plotFertility;
		end
	end

	-- Returns table, integer, integer.
	return landmassFertilityTable, landmassFertilityCount, plotCount;
end
------------------------------------------------------------------------------
function AssignStartingPlots:MeasureStartPlacementFertilityOfLandmass(iLandmassID, iWestX, iEastX, iSouthY, iNorthY, wrapsX, wrapsY)
	-- This function is designed to provide initial data for regional division recursion.
	-- Loop through plots in this landmass and measure Fertility Rating.
	-- Results will include a data table of all plots within the rectangle that includes the entirety of this landmass.

	-- This function will account for any wrapping around the world this landmass may do.

	local iW, iH = Map.GetGridSize();

	-- These coordinates will be used in case of wrapping landmass, extending the landmass "off the map", in to imaginary space to process it.
	-- Modulo math will correct the coordinates for accessing the plot data array.
	local xEnd, yEnd;

	if wrapsX then
		xEnd = iEastX + iW;
	else
		xEnd = iEastX;
	end
	if wrapsY then
		yEnd = iNorthY + iH;
	else
		yEnd = iNorthY;
	end

	local landmassFertilityTable = {};
	local landmassFertilityCount = 0;
	local plotCount = 0;
	for yLoop = iSouthY, yEnd do -- When generating a plot data table incrementally, process Y first so that plots go row by row.
		for xLoop = iWestX, xEnd do
			plotCount = plotCount + 1;
			local x = xLoop % iW;
			local y = yLoop % iH;
			local plot = Map.GetPlot(x, y);
			local thisPlotsLandmass = plot:GetLandmass();
			if thisPlotsLandmass ~= iLandmassID then -- This plot is not a member of the landmass, set value to 0
				table.insert(landmassFertilityTable, 0);
			else -- This plot is a member, process it.
				local plotFertility = self:MeasureStartPlacementFertilityOfPlot(x, y, true); -- Check for coastal land is enabled.
				table.insert(landmassFertilityTable, plotFertility);
				landmassFertilityCount = landmassFertilityCount + plotFertility;
			end
		end
	end

	-- Note: The table accounts for world wrap, so make sure to translate its index correctly.
	-- Plots in the table run from the southwest corner along the bottom row, then upward row by row, per normal plot data indexing.
	return landmassFertilityTable, landmassFertilityCount, plotCount;
end
------------------------------------------------------------------------------
function AssignStartingPlots:MeasureStartPlacementFertilityOfArea(iAreaID, iWestX, iEastX, iSouthY, iNorthY, wrapsX, wrapsY)
	-- This function is designed to provide initial data for continental regional division only.
	-- Biggest landmass method only needs to measure landmass fertility, and doesn't need to take areas into account.

	-- Loop through plots in this area and measure Fertility Rating.
	-- Results will include a data table of all plots within the rectangle that includes the entirety of this area.

	-- This function will account for any wrapping around the world this area may do.

	local iW, iH = Map.GetGridSize();

	-- These coordinates will be used in case of wrapping area, extending the area "off the map", in to imaginary space to process it.
	-- Modulo math will correct the coordinates for accessing the plot data array.
	local xEnd, yEnd;

	if wrapsX then
		xEnd = iEastX + iW;
	else
		xEnd = iEastX;
	end
	if wrapsY then
		yEnd = iNorthY + iH;
	else
		yEnd = iNorthY;
	end

	local areaFertilityTable = {};
	local areaFertilityCount = 0;
	local plotCount = 0;
	for yLoop = iSouthY, yEnd do -- When generating a plot data table incrementally, process Y first so that plots go row by row.
		for xLoop = iWestX, xEnd do
			plotCount = plotCount + 1;
			local x = xLoop % iW;
			local y = yLoop % iH;
			local plot = Map.GetPlot(x, y);
			local thisPlotsArea = plot:GetArea();
			if thisPlotsArea ~= iAreaID then -- This plot is not a member of the area, set value to 0
				table.insert(areaFertilityTable, 0);
			else -- This plot is a member, process it.
				local plotFertility = self:MeasureStartPlacementFertilityOfPlot(x, y, true); -- Check for coastal land is enabled.
				table.insert(areaFertilityTable, plotFertility);
				areaFertilityCount = areaFertilityCount + plotFertility;
			end
		end
	end

	-- Note: The table accounts for world wrap, so make sure to translate its index correctly.
	-- Plots in the table run from the southwest corner along the bottom row, then upward row by row, per normal plot data indexing.
	return areaFertilityTable, areaFertilityCount, plotCount;
end
------------------------------------------------------------------------------
function AssignStartingPlots:RemoveDeadRows(fertility_table, iWestX, iSouthY, iWidth, iHeight)
	-- Any outside rows in the fertility table of a just-divided region that contains all zeroes can be safely removed.
	-- This will improve the accuracy of operations involving any applicable region.
	local iW, iH = Map.GetGridSize();
	local adjusted_table = {};
	local adjusted_WestX;
	local adjusted_SouthY;
	local adjusted_Width;
	local adjusted_Height;

	-- Check for rows to remove on the bottom.
	local adjustSouth = 0;
	for y = 0, iHeight - 1 do
		local bKeepThisRow = false;
		for x = 0, iWidth - 1 do
			local i = y * iWidth + x + 1;
			if fertility_table[i] ~= 0 then
				bKeepThisRow = true;
				break;
			end
		end
		if bKeepThisRow then
			break;
		else
			adjustSouth = adjustSouth + 1;
		end
	end

	-- Check for rows to remove on the top.
	local adjustNorth = 0;
	for y = iHeight - 1, 0, -1 do
		local bKeepThisRow = false;
		for x = 0, iWidth - 1 do
			local i = y * iWidth + x + 1;
			if fertility_table[i] ~= 0 then
				bKeepThisRow = true;
				break;
			end
		end
		if bKeepThisRow then
			break;
		else
			adjustNorth = adjustNorth + 1;
		end
	end

	-- Check for columns to remove on the left.
	local adjustWest = 0;
	for x = 0, iWidth - 1 do
		local bKeepThisColumn = false;
		for y = 0, iHeight - 1 do
			local i = y * iWidth + x + 1;
			if fertility_table[i] ~= 0 then
				bKeepThisColumn = true;
				break;
			end
		end
		if bKeepThisColumn then
			break;
		else
			adjustWest = adjustWest + 1;
		end
	end

	-- Check for columns to remove on the right.
	local adjustEast = 0;
	for x = iWidth - 1, 0, -1 do
		local bKeepThisColumn = false;
		for y = 0, iHeight - 1 do
			local i = y * iWidth + x + 1;
			if fertility_table[i] ~= 0 then
				bKeepThisColumn = true;
				break;
			end
		end
		if bKeepThisColumn then
			break;
		else
			adjustEast = adjustEast + 1;
		end
	end

	if adjustSouth > 0 or adjustNorth > 0 or adjustWest > 0 or adjustEast > 0 then
		-- Truncate this region to remove dead rows.
		adjusted_WestX = (iWestX + adjustWest) % iW;
		adjusted_SouthY = (iSouthY + adjustSouth) % iH;
		adjusted_Width = (iWidth - adjustWest) - adjustEast;
		adjusted_Height = (iHeight - adjustSouth) - adjustNorth;
		-- Reconstruct fertility table. This must be done row by row, so process Y coord first.
		for y = 0, adjusted_Height - 1 do
			for x = 0, adjusted_Width - 1 do
				local i = (y + adjustSouth) * iWidth + (x + adjustWest) + 1;
				local plotFert = fertility_table[i];
				table.insert(adjusted_table, plotFert);
			end
		end
		--[[
		print("-");
		print("Removed Dead Rows, West: ", adjustWest, " East: ", adjustEast);
		print("Removed Dead Rows, South: ", adjustSouth, " North: ", adjustNorth);
		print("-");
		print("Incoming values: ", iWestX, iSouthY, iWidth, iHeight);
		print("Outgoing values: ", adjusted_WestX, adjusted_SouthY, adjusted_Width, adjusted_Height);
		print("-");
		print("Size of incoming fertility table: ", table.maxn(fertility_table));
		print("Size of outgoing fertility table: ", table.maxn(adjusted_table));
		--]]
		return adjusted_table, adjusted_WestX, adjusted_SouthY, adjusted_Width, adjusted_Height;

	else -- Region not adjusted, return original values unaltered.
		return fertility_table, iWestX, iSouthY, iWidth, iHeight;
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:DivideIntoRegions(iNumDivisions, fertility_table, rectangle_data_table)
	if iNumDivisions == 1 then
		local fAverageFertility = rectangle_data_table[6] / rectangle_data_table[7];
		table.insert(rectangle_data_table, fAverageFertility);
		table.insert(self.regionData, rectangle_data_table);
	elseif iNumDivisions > 1 then
		local iWidth = rectangle_data_table[3];
		local iHeight = rectangle_data_table[4];
		local bTaller = (iHeight > iWidth);

		if iNumDivisions % 3 == 0 then
			local iSubdivisions = iNumDivisions / 3;
			print(string.format("DivideIntoRegions: Three: 2, %i", iSubdivisions));

			local results = self:ChopIntoThreeRegions(fertility_table, rectangle_data_table, bTaller);
			self:DivideIntoRegions(iSubdivisions, results[1], results[2]);
			self:DivideIntoRegions(iSubdivisions, results[3], results[4]);
			self:DivideIntoRegions(iSubdivisions, results[5], results[6]);
		elseif iNumDivisions % 2 == 0 then
			local iSubdivisions = iNumDivisions / 2;
			print(string.format("DivideIntoRegions: Even: 2, %i", iSubdivisions));

			local results = self:ChopIntoTwoRegions(fertility_table, rectangle_data_table, bTaller, 49.5);
			self:DivideIntoRegions(iSubdivisions, results[1], results[2]);
			self:DivideIntoRegions(iSubdivisions, results[3], results[4]);
		else
			-- This works all the way to 79, which is fine as REALLY_MAX_PLAYERS is 80 anyway!
			local laterSubdivisions
			if iNumDivisions == 5 then
				laterSubdivisions = 2;
			elseif iNumDivisions <= 17 then
				laterSubdivisions = math.floor(iNumDivisions / 4 + 1) * 2;
			else
				laterSubdivisions = math.floor(iNumDivisions / 8 + 1) * 4;
			end
			local firstSubdivisions = iNumDivisions - laterSubdivisions;

			-- chopPercent is calculated to undershoot to compensate for the overshoot in the region chopping code
			local chopPercent = math.floor((firstSubdivisions / (iNumDivisions + 0.1)) * 1000) / 10;
			print(string.format("DivideIntoRegions: Prime: %i, %i, %f", firstSubdivisions, laterSubdivisions, chopPercent));

			local results = self:ChopIntoTwoRegions(fertility_table, rectangle_data_table, bTaller, chopPercent);
			self:DivideIntoRegions(firstSubdivisions, results[1], results[2]);
			self:DivideIntoRegions(laterSubdivisions, results[3], results[4]);
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:ChopIntoThreeRegions(fertility_table, rectangle_data_table, bTaller)
	-- print("-"); print("ChopIntoThree called.");
	-- Performs the mechanics of dividing a region into three roughly equal fertility subregions.
	local results = {};

	-- Chop off the first third.
	local initial_results = self:ChopIntoTwoRegions(fertility_table, rectangle_data_table, bTaller, 33); -- Undershoot by a bit, tends to make the actual result closer to accurate.
	-- add first subdivision to results
	local temptable = initial_results[1];
	table.insert(results, temptable);

	--[[ Activate table printouts for debug purposes only, then deactivate when done.
	print("Data returned to ChopIntoThree from ChopIntoTwo.");
	PrintContentsOfTable(temptable);
	--]]

	temptable = initial_results[2];
	table.insert(results, temptable);

	-- PrintContentsOfTable(temptable);

	-- Prepare the remainder for further processing.
	local second_section_fertility_table = initial_results[3];

	-- PrintContentsOfTable(second_section_fertility_table);

	local second_section_data_table = initial_results[4];

	-- PrintContentsOfTable(second_section_data_table);
	-- print("End of this instance, ChopIntoThree tables.");

	-- See if this piece is taller or wider. (Ed's original implementation skipped this step).
	local bTallerForRemainder = false;
	local width = second_section_data_table[3];
	local height = second_section_data_table[4];
	if height > width then
		bTallerForRemainder = true;
	end

	-- Chop the bigger piece in half.
	local interim_results = self:ChopIntoTwoRegions(second_section_fertility_table, second_section_data_table, bTallerForRemainder, 48.5); -- Undershoot just a little.
	table.insert(results, interim_results[1]);
	table.insert(results, interim_results[2]);
	table.insert(results, interim_results[3]);
	table.insert(results, interim_results[4]);

	-- Returns a table of six entries, each of which is a nested table.
	-- 1: fertility_table of first subdivision
	-- 2: rectangle_data_table of first subdivision.
	-- 3: fertility_table of second subdivision
	-- 4: rectangle_data_table of second subdivision.
	-- 5: fertility_table of third subdivision
	-- 6: rectangle_data_table of third subdivision.

	return results;
end
------------------------------------------------------------------------------
function AssignStartingPlots:ChopIntoTwoRegions(fertility_table, rectangle_data_table, bTaller, chopPercent)
	-- Performs the mechanics of dividing a region into two subregions.

	-- Fertility table is a plot data array including data for all plots to be processed here.
	-- This data already factors any need for processing LandmassID/AreaID.

	-- Rectangle table includes seven data fields:
	-- westX, southY, width, height, LandmassID/AreaID, fertilityCount, plotCount
	-- print("-"); print("ChopIntoTwo called.");

	--[[ Log dump of incoming table data. Activate for debug only.
	print("Data tables passed to ChopIntoTwoRegions.");
	PrintContentsOfTable(fertility_table);
	PrintContentsOfTable(rectangle_data_table);
	print("End of this instance, ChopIntoTwoRegions tables.");
	--]]

	-- Read the incoming data table.
	local iW, iH = Map.GetGridSize();
	local iWestX = rectangle_data_table[1];
	local iSouthY = rectangle_data_table[2];
	local iRectWidth = rectangle_data_table[3];
	local iRectHeight = rectangle_data_table[4];
	local iAreaID = rectangle_data_table[5];
	local iTargetFertility = rectangle_data_table[6] * chopPercent / 100;

	-- Now divide the region.
	-- West and South edges remain the same for first region.
	local firstRegionWestX = iWestX;
	local firstRegionSouthY = iSouthY;
	-- scope variables that get decided conditionally.
	local firstRegionWidth, firstRegionHeight;
	local secondRegionWestX, secondRegionSouthY, secondRegionWidth, secondRegionHeight;
	local iFirstRegionFertility = 0;
	local iSecondRegionFertility = 0;
	local region_one_fertility = {};
	local region_two_fertility = {};

	if bTaller then -- We will divide horizontally, resulting in first region on bottom, second on top.

		-- Width for both will remain the same as the parent rectangle.
		firstRegionWidth = iRectWidth;
		secondRegionWestX = iWestX;
		secondRegionWidth = iRectWidth;

		-- Measure one row at a time, moving up from bottom, until we have exceeded the target fertility.
		local reachedTargetRow = false;
		local rectY = 0;
		while not reachedTargetRow do
			-- Process the next row in line.
			for loopX = 0, iRectWidth - 1 do
				local fertIndex = rectY * iRectWidth + loopX + 1;
				local plotFertility = fertility_table[fertIndex];
				-- Add this plot's fertility to the region total so far.
				iFirstRegionFertility = iFirstRegionFertility + plotFertility;
				-- Record this plot in a new fertility table. (Needed for further subdivisions).
				-- Note, building this plot data table incrementally, so it must go row by row.
				table.insert(region_one_fertility, plotFertility);
			end
			if iFirstRegionFertility >= iTargetFertility then
				-- This row has completed the region.
				firstRegionHeight = rectY + 1;
				secondRegionSouthY = (iSouthY + rectY + 1) % iH;
				secondRegionHeight = iRectHeight - firstRegionHeight;
				reachedTargetRow = true;
				break;
			else
				rectY = rectY + 1;
			end
		end

		-- Debug printout of division location.
		-- print("Dividing along horizontal line between rows: ", secondRegionSouthY - 1, "-", secondRegionSouthY);

		-- Create the fertility table for the second region, the one on top.
		-- Data must be added row by row, to keep the table index behavior consistent.
		for loopY = firstRegionHeight, iRectHeight - 1 do
			for loopX = 0, iRectWidth - 1 do
				local fertIndex = loopY * iRectWidth + loopX + 1;
				local plotFertility = fertility_table[fertIndex];
				-- Add this plot's fertility to the region total so far.
				iSecondRegionFertility = iSecondRegionFertility + plotFertility;
				-- Record this plot in a new fertility table. (Needed for further subdivisions).
				-- Note, building this plot data table incrementally, so it must go row by row.
				table.insert(region_two_fertility, plotFertility);
			end
		end

	else -- We will divide vertically, resulting in first region on left, second on right.
		-- Height for both will remain the same as the parent rectangle.
		firstRegionHeight = iRectHeight;
		secondRegionSouthY = iSouthY;
		secondRegionHeight = iRectHeight;

		-- First region's new fertility table will be a little tricky.
		-- We don't know how many table entries it will need beforehand,
		-- and we cannot add the entries sequentially when the data is being generated column by column,
		-- yet the table index needs to proceed row by row.
		-- So we will have to make a second pass.

		-- Measure one column at a time, moving left to right, until we have exceeded the target fertility.
		local reachedTargetColumn = false;
		local rectX = 0;
		while not reachedTargetColumn do
			-- Process the next column in line.
			for loopY = 0, iRectHeight - 1 do
				local fertIndex = loopY * iRectWidth + rectX + 1;
				local plotFertility = fertility_table[fertIndex];
				-- Add this plot's fertility to the region total so far.
				iFirstRegionFertility = iFirstRegionFertility + plotFertility;
				-- No table record here, handle later row by row.
			end
			if iFirstRegionFertility >= iTargetFertility then
				-- This column has completed the region.
				firstRegionWidth = rectX + 1;
				secondRegionWestX = (iWestX + rectX + 1) % iW;
				secondRegionWidth = iRectWidth - firstRegionWidth;
				reachedTargetColumn = true;
				break;
			else
				rectX = rectX + 1;
			end
		end

		-- Debug printout of division location.
		-- print("Dividing along vertical line between columns: ", secondRegionWestX - 1, "-", secondRegionWestX);

		-- Create the fertility table for the second region, the one on the right.
		-- Data must be added row by row, to keep the table index behavior consistent.
		for loopY = 0, iRectHeight - 1 do
			for loopX = firstRegionWidth, iRectWidth - 1 do
				local fertIndex = loopY * iRectWidth + loopX + 1;
				local plotFertility = fertility_table[fertIndex];
				-- Add this plot's fertility to the region total so far.
				iSecondRegionFertility = iSecondRegionFertility + plotFertility;
				-- Record this plot in a new fertility table. (Needed for further subdivisions).
				-- Note, building this plot data table incrementally, so it must go row by row.
				table.insert(region_two_fertility, plotFertility);
			end
		end
		-- Now create the fertility table for the first region.
		for loopY = 0, iRectHeight - 1 do
			for loopX = 0, firstRegionWidth - 1 do
				local fertIndex = loopY * iRectWidth + loopX + 1;
				local plotFertility = fertility_table[fertIndex];
				table.insert(region_one_fertility, plotFertility);
			end
		end
	end

	-- Now check the newly divided regions for dead rows (all zero values) along
	-- the edges and remove any found.

	-- First region
	local FRFertT, FRWX, FRSY, FRWid, FRHei;
	FRFertT, FRWX, FRSY, FRWid, FRHei = self:RemoveDeadRows(region_one_fertility,
		firstRegionWestX, firstRegionSouthY, firstRegionWidth, firstRegionHeight);

	-- Second region
	local SRFertT, SRWX, SRSY, SRWid, SRHei;
	SRFertT, SRWX, SRSY, SRWid, SRHei = self:RemoveDeadRows(region_two_fertility,
		secondRegionWestX, secondRegionSouthY, secondRegionWidth, secondRegionHeight);

	-- Generate the data tables that record the location of the new subdivisions.
	local firstPlots = FRWid * FRHei;
	local secondPlots = SRWid * SRHei;
	local region_one_data = {FRWX, FRSY, FRWid, FRHei, iAreaID, iFirstRegionFertility, firstPlots};
	local region_two_data = {SRWX, SRSY, SRWid, SRHei, iAreaID, iSecondRegionFertility, secondPlots};

	-- Generate the final data.
	local outcome = {FRFertT, region_one_data, SRFertT, region_two_data};
	return outcome;
end
------------------------------------------------------------------------------
function AssignStartingPlots:CustomOverride()
	-- This function allows an easy entry point for overrides that need to
	-- take place after regional division, but before anything else.
end
------------------------------------------------------------------------------
function AssignStartingPlots:GenerateRegions(args)
	print("Map Generation - Dividing the map in to Regions");
	-- This function stores its data in the instance (self) data table.

	-- The "Three Methods" of regional division:
	-- 1. Biggest Landmass: All civs start on the biggest landmass.
	-- 2. Continental: Civs are assigned to continents. Any continents with more than one civ are divided.
	-- 3. Rectangular: Civs start within a given rectangle that spans the whole map, without regard to landmass sizes.
	--                 This method is primarily applied to Archipelago and other maps with lots of tiny islands.
	-- 4. Rectangular: Civs start within a given rectangle defined by arguments passed in on the function call.
	--                 Arguments required for this method: iWestX, iSouthY, iWidth, iHeight
	args = args or {};
	local iW, iH = Map.GetGridSize();
	self.method = args.method or self.method; -- Continental method is default.
	self.resource_setting = args.resources or 2; -- UNUSED, use the below resource settings instead
	args.resources = args.resources or 2;

	-- Custom map resource settings (Communitu_79a support)
	if args.resources == 6 then
		args.resources = 1 + Map.Rand(3, "Resource setting - AssignStartingPlots");
	end

	if args.resources < 4 and not args.comm then
		self.resDensity = args.resources;						-- Strategic Resource Density setting
		self.resSize = args.resources;							-- Strategic Resource Deposit Size setting
		self.bonusDensity = args.resources;						-- Bonus Resource Density setting
		self.luxuryDensity = args.resources;					-- Luxury Resource Density setting
	elseif args.comm then
		self.resDensity = args.resources;						-- Strategic Resource Density setting
		self.resSize = args.size;								-- Strategic Resource Deposit Size setting
		self.bonusDensity = args.bonus;							-- Bonus Resource Density setting
		self.luxuryDensity = args.lux;							-- Luxury Resource Density setting
	end
	self.legStart = args.legend or (args.resources == 4);		-- Legendary Start setting
	self.resBalance = args.balance or (args.resources == 5);	-- Strategic Balance setting

	print("-"); print("Resource settings");
	print("Strategic Density = ", self.resDensity);
	print("Strategic Size = ", self.resSize);
	print("Bonus Density = ", self.bonusDensity);
	print("Luxury Density = ", self.luxuryDensity);
	print("Legendary Start = ", self.legStart);
	print("Strategic Balance = ", self.resBalance);

	-- Determine number of civilizations and city states present in this game.
	self.iNumCivs, self.iNumCityStates, self.player_ID_list, self.bTeamGame, self.teams_with_major_civs, self.number_civs_per_team = GetPlayerAndTeamInfo();
	self.iNumCityStatesUnassigned = self.iNumCityStates;
	-- print("-"); print("Civs:", self.iNumCivs); print("City States:", self.iNumCityStates);

	-- Custom code for Frontier
	if args.addEmptyRegions and self.iNumCivs <= 22 then
		local emptyRegionMinCount = {1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 3, 2, 1, 0};
		local emptyRegionMaxCount = {1, 1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0};
		self.iNumFrontiers = self:GetRandomFromRangeInclusive(emptyRegionMinCount[self.iNumCivs], emptyRegionMaxCount[self.iNumCivs]);
		print("Number of empty regions:", self.iNumFrontiers); print("-");
	end

	local skip = false;

	if self.method == RegionDivision.BIGGEST_LANDMASS then
		-- Identify the biggest landmass.
		local iLandmassID = Map.FindBiggestLandmassID(false);
		-- We'll need all eight data fields returned in the results table from the boundary finder:
		local landmass_data = self:GetLandmassBoundaries(iLandmassID);
		local iWestX = landmass_data[1];
		local iSouthY = landmass_data[2];
		local iEastX = landmass_data[3];
		local iNorthY = landmass_data[4];
		local iWidth = landmass_data[5];
		local iHeight = landmass_data[6];
		local wrapsX = landmass_data[7];
		local wrapsY = landmass_data[8];

		-- Obtain "Start Placement Fertility" of the landmass. (This measurement is customized for start placement).
		-- This call returns a table recording fertility of all plots within a rectangle that contains the landmass,
		-- with a zero value for any plots not part of the landmass -- plus a fertility sum and plot count.
		local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityOfLandmass(iLandmassID,
												 iWestX, iEastX, iSouthY, iNorthY, wrapsX, wrapsY);
		-- Now divide this landmass in to regions, one per civ.
		-- The regional divider requires three arguments:
		-- 1. Number of divisions. (For "Biggest Landmass" this means number of civs in the game).
		-- 2. Fertility table. (This was obtained from the last call.)
		-- 3. Rectangle table. This table includes seven data fields:
		-- westX, southY, width, height, LandmassID, fertilityCount, plotCount
		-- This is why we got the fertCount and plotCount from the fertility function.

		-- Assemble the Rectangle data table:
		local rect_table = {iWestX, iSouthY, iWidth, iHeight, iLandmassID, fertCount, plotCount};
		-- The data from this call is processed in to self.regionData during the process.
		self:DivideIntoRegions(self.iNumCivs + self.iNumFrontiers, fert_table, rect_table);
		-- The regions have been defined.

	elseif self.method == RegionDivision.RECTANGULAR or self.method == RegionDivision.RECTANGULAR_SELF_DEFINED then
		-- Obtain the boundaries of the rectangle to be processed.
		-- If no coords were passed via the args table, default to processing the entire map.
		-- Note that it matters if method 3 or 4 is designated, because the difference affects
		-- how city states are placed, whether they look for any uninhabited lands outside the rectangle.
		self.inhabited_WestX = args.iWestX or 0;
		self.inhabited_SouthY = args.iSouthY or 0;
		self.inhabited_Width = args.iWidth or iW;
		self.inhabited_Height = args.iHeight or iH;

		-- Obtain "Start Placement Fertility" inside the rectangle.
		-- Data returned is: fertility table, sum of all fertility, plot count.
		local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityInRectangle(self.inhabited_WestX,
												 self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height);
		-- Assemble the Rectangle data table:
		local rect_table = {self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width,
							self.inhabited_Height, -1, fertCount, plotCount}; -- AreaID -1 means ignore landmass/area IDs.
		-- Divide the rectangle.
		self:DivideIntoRegions(self.iNumCivs + self.iNumFrontiers, fert_table, rect_table);
		-- The regions have been defined.

	else -- Continental.
		-- Loop through all plots on the map, measuring fertility of each land plot, identifying its LandmassID and AreaID,
		-- building a list of LandmassIDs and AreaIDs, and tallying the Start Placement Fertility for each landmass/area.

		self.bArea = true;

		-- Obtain info on all landmasses and areas for comparison purposes.
		local iGlobalFertilityOfLands = 0;
		local iNumAreas = 0;
		local landmass_IDs = {};
		local landmass_fert = {};
		local area_IDs = {};
		local area_fert = {};
		-- Cycle through all plots in the world, checking their Start Placement Fertility, LandmassID, and AreaID.
		for x = 0, iW - 1 do
			for y = 0, iH - 1 do
				local plot = Map.GetPlot(x, y);
				if not plot:IsWater() then -- Land plot, process it.
					local iLandmass = plot:GetLandmass();
					local iArea = plot:GetArea();
					local plotFertility = self:MeasureStartPlacementFertilityOfPlot(x, y, true); -- Check for coastal land is enabled.
					iGlobalFertilityOfLands = iGlobalFertilityOfLands + plotFertility;

					if not TestMembership(landmass_IDs, iLandmass) then -- This plot is the first detected in its landmass.
						table.insert(landmass_IDs, iLandmass);
						landmass_fert[iLandmass] = plotFertility;
					else -- This landmass already known.
						landmass_fert[iLandmass] = landmass_fert[iLandmass] + plotFertility;
					end

					if not TestMembership(area_IDs, iArea) then -- This plot is the first detected in its area.
						iNumAreas = iNumAreas + 1;
						table.insert(area_IDs, iArea);
						area_fert[iArea] = plotFertility;
						-- print("Area#", iArea, "is a new area in landmass#", iLandmass);
					else -- This area already known.
						area_fert[iArea] = area_fert[iArea] + plotFertility;
					end
				end
			end
		end

		-- Sort areas, achieving a list of areas with best areas first.
		-- Add fertility check at 25% of fertility of the largest landmass to prevent tiny islands from being considered, which will result in isolated starts.
		-- We do these by making a new table storing area fertilities that reach the threshold.
		local interim_table = {};
		local biggestLandmass = Map.FindBiggestLandmassID(false);
		local min_landmass_fertility = landmass_fert[biggestLandmass] * 0.25;
		local iNumLandmass = 0;
		local area_landmass = {};
		print("-"); print("Minimum landmass fertility required =", min_landmass_fertility); print("-");
		for iLandmass, data_entry in pairs(landmass_fert) do
			if data_entry >= min_landmass_fertility then
				iNumLandmass = iNumLandmass + 1;
				for _, iArea in ipairs(self.landmass_areas[iLandmass]) do
					if area_fert[iArea] > 0 then
						print("Area#", iArea, "in Landmass#", iLandmass, "is a candidate for start placement, Fertility =", area_fert[iArea]);
						table.insert(interim_table, area_fert[iArea]);
						area_landmass[iArea] = iLandmass;
					else
						iNumAreas = iNumAreas - 1;
					end
				end
			else
				-- These landmasses won't be considered
				iNumAreas = iNumAreas - table.maxn(self.landmass_areas[iLandmass]);
			end
		end

		-- If only one relevant landmass, we switch tracks to use biggest landmass division
		if iNumLandmass <= 1 then
			print("Only one landmass fertile enough for start placement. Switching to use Biggest Landmass division.");
			args.method = RegionDivision.BIGGEST_LANDMASS;
			self.bArea = false;
			skip = true;
			self:GenerateRegions(args);
		else
			---[[
			for i, fert in ipairs(interim_table) do
				print("Interim Table ID " .. i .. " has fertility of " .. fert);
			end
			print("* * * * * * * * * *");
			--]]

			-- Sort the fertility values stored in the interim table. Sort order in Lua is lowest to highest.
			table.sort(interim_table);

			---[[
			for i, fert in ipairs(interim_table) do
				print("Interim Table ID " .. i .. " has fertility of " .. fert);
			end
			print("* * * * * * * * * *");
			--]]

			-- If less players than areas, we will ignore the extra areas.
			local iNumRelevantAreas = math.min(iNumAreas, self.iNumCivs + self.iNumFrontiers);
			print("Number of relevant areas =", iNumRelevantAreas);

			-- Now re-match the AreaIDs with their corresponding fertility values by comparing the original fertility table with the sorted interim table.
			-- During this comparison, best_areas will be constructed from sorted AreaIDs, richest stored first.
			local best_areas = {};
			-- Currently, the best yields are at the end of the interim table. We need to step backward from there.
			local end_of_interim_table = table.maxn(interim_table);
			-- We may not need all entries in the table. Process only iNumRelevantAreas worth of table entries.
			local fertility_value_list = {};
			local fertility_value_tie = false;
			local relevantFertility = 0;
			for tableConstructionLoop = end_of_interim_table, end_of_interim_table - iNumRelevantAreas + 1, -1 do
				relevantFertility = relevantFertility + interim_table[tableConstructionLoop];
				if TestMembership(fertility_value_list, interim_table[tableConstructionLoop]) then
					fertility_value_tie = true;
					print("*** WARNING: Fertility Value Tie exists! ***");
				else
					table.insert(fertility_value_list, interim_table[tableConstructionLoop]);
				end
			end
			print("Relevant land fertility =", relevantFertility);

			if not fertility_value_tie then -- No ties, so no need of special handling for ties.
				for loop = end_of_interim_table, end_of_interim_table - iNumRelevantAreas + 1, -1 do
					for loop2, areaID in ipairs(area_IDs) do
						if interim_table[loop] == area_fert[area_IDs[loop2]] then
							table.insert(best_areas, areaID);
							break;
						end
					end
				end
			else -- Ties exist! Special handling required to protect against a shortfall in the number of defined regions.
				local iNumUniqueFertValues = table.maxn(fertility_value_list);
				for fertLoop = 1, iNumUniqueFertValues do
					for areaID, fert in pairs(area_fert) do
						if fert == fertility_value_list[fertLoop] then
							-- Add ties only if there is room!
							if table.maxn(best_areas) < iNumRelevantAreas then
								table.insert(best_areas, areaID);
							else
								break;
							end
						end
					end
				end
			end
			PrintContentsOfTable(fertility_value_list);
			PrintContentsOfTable(best_areas);

			local inhabitedAreaIDs = {};
			local numberOfCivsPerArea = {}; -- Indexed in synch with best_areas. Use same index to match values from each table.
			local fertilityPerCiv = relevantFertility * 0.7 / (self.iNumCivs + self.iNumFrontiers); -- Allow some unused fertility here
			local numberOfCivsPerLandmass = {};
			print("Expected fertility per civ =", fertilityPerCiv);
			for loop, areaID in ipairs(best_areas) do
				numberOfCivsPerArea[loop] = math.floor(area_fert[areaID] / fertilityPerCiv);
				print("Area", areaID, "can have", numberOfCivsPerArea[loop], "regions");
				if numberOfCivsPerLandmass[area_landmass[areaID]] then
					numberOfCivsPerLandmass[area_landmass[areaID]] = numberOfCivsPerLandmass[area_landmass[areaID]] + numberOfCivsPerArea[loop];
				else
					numberOfCivsPerLandmass[area_landmass[areaID]] = numberOfCivsPerArea[loop];
				end
				if numberOfCivsPerArea[loop] == 0 then
					break;
				end
			end

			local iTotalFertileAreas = 0;
			local iTotalFertileLandmassAreas = 0; -- Don't count landmasses too infertile for more than one civ to spawn (prevent isolation)
			for _, numberOfCivs in pairs(numberOfCivsPerLandmass) do
				iTotalFertileAreas = iTotalFertileAreas + numberOfCivs;
				if numberOfCivs >= 2 then
					iTotalFertileLandmassAreas = iTotalFertileLandmassAreas + numberOfCivs;
				end
			end

			-- If not enough fertile areas, the continents are too small for this division method.
			-- Switch tracks to use rectangular method instead.
			if iTotalFertileAreas < self.iNumCivs + self.iNumFrontiers then
				print("Landmasses aren't fertile enough for start placement. Switching to use Rectangular division.");
				args.method = RegionDivision.RECTANGULAR;
				self.bArea = false;
				skip = true;
				self:GenerateRegions(args);
			else
				-- If enough fertile areas in fertile landmasses, disallow landmasses with only one civ
				if iTotalFertileLandmassAreas >= self.iNumCivs + self.iNumFrontiers then
					for landmassID, numberOfCivs in pairs(numberOfCivsPerLandmass) do
						if numberOfCivs <= 1 then
							numberOfCivsPerLandmass[landmassID] = 0;
						end
					end
				end

				-- Assign areas to receive start plots. Record number of civs assigned to each area.
				-- Note that area_fert table is being altered here, and should not be relied on after this loop.
				local landmassID = -1;
				local inhabitedLandmassIDs = {};
				local actualNumberOfCivsPerArea = {};
				for civToAssign = 1, self.iNumCivs + self.iNumFrontiers do
					local bestRemainingArea = -1;
					local bestRemainingFertility = 0;

					-- Loop through areas, find the one with the best remaining fertility (civs added to an area reduces its fertility rating for subsequent civs).
					-- print("- - Searching areas in order to place Civ #", civToAssign); print("-");
					for _, areaID in ipairs(best_areas) do
						if area_fert[areaID] > bestRemainingFertility then
							-- If last assigned area is on a new landmass, always assign another civ to it
							-- It has been made sure that each landmass can support at least two civs
							if landmassID == -1 or area_landmass[areaID] == landmassID then
								-- Last civ cannot be placed on a new landmass
								if civToAssign ~= self.iNumCivs + self.iNumFrontiers or TestMembership(inhabitedLandmassIDs, area_landmass[areaID]) then
									bestRemainingArea = areaID;
									bestRemainingFertility = area_fert[areaID];
									-- print("- Found new candidate area with Area ID#:", bestRemainingArea, " with fertility of ", bestRemainingFertility);
								end
							end
						end
					end
					if bestRemainingArea == -1 then
						print("Failed to find an area somehow, assign to first area as a failsafe");
						bestRemainingArea = best_areas[1];
					end

					-- Record results for this pass
					if not TestMembership(inhabitedAreaIDs, bestRemainingArea) then
						table.insert(inhabitedAreaIDs, bestRemainingArea);
						actualNumberOfCivsPerArea[bestRemainingArea] = 0;
					end
					actualNumberOfCivsPerArea[bestRemainingArea] = actualNumberOfCivsPerArea[bestRemainingArea] + 1;

					if not TestMembership(inhabitedLandmassIDs, area_landmass[bestRemainingArea]) then
						table.insert(inhabitedLandmassIDs, area_landmass[bestRemainingArea]);
						landmassID = area_landmass[bestRemainingArea];
					else
						landmassID = -1;
					end
					print("Civ #", civToAssign, "has been assigned to Area#", bestRemainingArea, "on Landmass#", area_landmass[bestRemainingArea]); print("-");

					-- Deduct fertility taken by civ from area fertility
					area_fert[bestRemainingArea] = area_fert[bestRemainingArea] - fertilityPerCiv;
				end
				-- print("-"); print("--- End of Initial Readout ---"); print("-");

				print("*** Number of Civs per Area - Table Readout ***");
				PrintContentsOfTable(actualNumberOfCivsPerArea);
				print("--- End of Civs per Area readout ***"); print("-"); print("-");

				-- Loop through the list of inhabited areas, dividing each area into regions.
				-- Note that it is OK to divide an area with one civ on it:
				-- this will assign the whole of the area to a single region, and is the easiest method of recording such a region.
				for _, currentAreaID in ipairs(inhabitedAreaIDs) do
					-- Obtain the boundaries of and data for this area.
					local area_data = ObtainLandmassBoundaries(currentAreaID);
					local iWestX = area_data[1];
					local iSouthY = area_data[2];
					local iEastX = area_data[3];
					local iNorthY = area_data[4];
					local iWidth = area_data[5];
					local iHeight = area_data[6];
					local wrapsX = area_data[7];
					local wrapsY = area_data[8];

					-- Obtain "Start Placement Fertility" of the current area.
					-- Necessary to do this again because the fert_table can't be built prior to finding boundaries,
					-- and we had to ID the proper areas via fertility to be able to figure out their boundaries.
					local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityOfArea(currentAreaID, iWestX, iEastX, iSouthY, iNorthY, wrapsX, wrapsY);

					-- Now divide this area into regions, one per civ.
					-- The regional divider requires three arguments:
					-- 1. Number of civs assigned to this area.
					-- 2. Fertility table. (This was obtained from the last call.)
					-- 3. Rectangle table. This table includes seven data fields: westX, southY, width, height, areaID, fertilityCount, plotCount
					-- Note that the global bArea flag is true here, which means areaID is used.
					local rect_table = {iWestX, iSouthY, iWidth, iHeight, currentAreaID, fertCount, plotCount};

					-- Divide this area into number of regions equal to civs assigned here.
					local iNumCivsOnThisArea = actualNumberOfCivsPerArea[currentAreaID];
					if iNumCivsOnThisArea > 0 and iNumCivsOnThisArea <= MAX_MAJOR_CIVS then -- valid number of civs.
						--[[ Debug printout for regional division inputs.
						print("-");
						print("- Civs on this area: ", iNumCivsOnThisArea);
						print("- Area ID#: ", currentAreaID);
						print("- Fertility: ", fertCount);
						print("- Plot Count: ", plotCount);
						print("-");
						--]]
						self:DivideIntoRegions(iNumCivsOnThisArea, fert_table, rect_table);
					else
						print("Invalid number of civs assigned to an area: ", iNumCivsOnThisArea);
					end
				end
				-- The regions have been defined.
			end
		end
	end

	if not skip then
		-- Entry point for easier overrides.
		self:CustomOverride();

		---[[ Printout is for debugging only. Deactivate otherwise.
		local tempRegionData = self.regionData;
		for i, data in ipairs(tempRegionData) do
			print("-");
			print("Data for Start Region #", i);
			print("WestX:", data[1]);
			print("SouthY:", data[2]);
			print("Width:", data[3]);
			print("Height:", data[4]);
			if self.bArea then
				print("AreaID:", data[5]);
			else
				print("LandmassID:", data[5]);
			end
			print("Fertility:", data[6]);
			print("Plots:", data[7]);
			print("Fert/Plot:", data[8]);
			print("-");
		end
		--]]
	end
end
------------------------------------------------------------------------------
-- Start of functions tied to ChooseLocations()
------------------------------------------------------------------------------
function AssignStartingPlots:MeasureTerrainInRegions()
	local iW, iH = Map.GetGridSize();
	-- This function stores its data in the instance (self) data table.
	for _, region_data_table in ipairs(self.regionData) do
		local iWestX = region_data_table[1];
		local iSouthY = region_data_table[2];
		local iWidth = region_data_table[3];
		local iHeight = region_data_table[4];
		local iAreaID = region_data_table[5];

		local totalPlots, areaPlots = 0, 0;
		local waterCount, flatlandsCount, hillsCount, peaksCount = 0, 0, 0, 0;
		local lakeCount, coastCount, oceanCount, iceCount = 0, 0, 0, 0;
		local grassCount, plainsCount, desertCount, tundraCount, snowCount = 0, 0, 0, 0, 0; -- counts flatlands only!
		local forestCount, jungleCount, marshCount, riverCount, floodplainCount, oasisCount = 0, 0, 0, 0, 0, 0;
		local coastalLandCount, nextToCoastCount = 0, 0;

		-- Iterate through the region's plots, getting plotType, terrainType, featureType and river status.
		for region_loop_y = 0, iHeight - 1 do
			for region_loop_x = 0, iWidth - 1 do
				totalPlots = totalPlots + 1;
				local x = (region_loop_x + iWestX) % iW;
				local y = (region_loop_y + iSouthY) % iH;
				local plot = Map.GetPlot(x, y);
				local area_of_plot = plot:GetArea();
				local landmass_of_plot = plot:GetLandmass();
				-- get plot info
				local plotType = plot:GetPlotType();
				local terrainType = plot:GetTerrainType();
				local featureType = plot:GetFeatureType();

				-- Mountain and Ocean plot types get their own AreaIDs, but we are going to measure them anyway.
				if plotType == PlotTypes.PLOT_MOUNTAIN then
					peaksCount = peaksCount + 1; -- and that's it for Mountain plots. No other stats.
				elseif plotType == PlotTypes.PLOT_OCEAN then
					waterCount = waterCount + 1;
					if terrainType == TerrainTypes.TERRAIN_COAST then
						if plot:IsLake() then
							lakeCount = lakeCount + 1;
						else
							coastCount = coastCount + 1;
						end
					else
						oceanCount = oceanCount + 1;
					end
					if featureType == FeatureTypes.FEATURE_ICE then
						iceCount = iceCount + 1;
					end

				else
					-- Hills and Flatlands, check plot for region membership. Only process this plot if it is a member.
					if iAreaID == -1 or (self.bArea and area_of_plot == iAreaID) or (not self.bArea and landmass_of_plot == iAreaID) then
						areaPlots = areaPlots + 1;

						-- set up coastalLand and nextToCoast index
						local i = iW * y + x + 1;

						-- Record plot data
						if plotType == PlotTypes.PLOT_HILLS and terrainType ~= TerrainTypes.TERRAIN_SNOW then
							hillsCount = hillsCount + 1;

							if self.plotDataIsCoastal[i] then
								coastalLandCount = coastalLandCount + 1;
							elseif self.plotDataIsNextToCoast[i] then
								nextToCoastCount = nextToCoastCount + 1;
							end

							if plot:IsRiverSide() then
								riverCount = riverCount + 1;
							end

							-- Feature check checking for all types, in case features are not obeying standard allowances.
							if featureType == FeatureTypes.FEATURE_FOREST then
								forestCount = forestCount + 1;
							elseif featureType == FeatureTypes.FEATURE_JUNGLE then
								jungleCount = jungleCount + 1;
							elseif featureType == FeatureTypes.FEATURE_MARSH then
								marshCount = marshCount + 1;
							elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
								floodplainCount = floodplainCount + 1;
							elseif featureType == FeatureTypes.FEATURE_OASIS then
								oasisCount = oasisCount + 1;
							end

						else -- Flatlands plot OR snow
							flatlandsCount = flatlandsCount + 1;

							if self.plotDataIsCoastal[i] then
								coastalLandCount = coastalLandCount + 1;
							elseif self.plotDataIsNextToCoast[i] then
								nextToCoastCount = nextToCoastCount + 1;
							end

							if plot:IsRiverSide() then
								riverCount = riverCount + 1;
							end

							if terrainType == TerrainTypes.TERRAIN_GRASS then
								grassCount = grassCount + 1;
							elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
								plainsCount = plainsCount + 1;
							elseif terrainType == TerrainTypes.TERRAIN_DESERT then
								desertCount = desertCount + 1;
							elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
								tundraCount = tundraCount + 1;
							elseif terrainType == TerrainTypes.TERRAIN_SNOW then
								snowCount = snowCount + 1;
							end

							-- Feature check checking for all types, in case features are not obeying standard allowances.
							if featureType == FeatureTypes.FEATURE_FOREST then
								forestCount = forestCount + 1;
							elseif featureType == FeatureTypes.FEATURE_JUNGLE then
								jungleCount = jungleCount + 1;
							elseif featureType == FeatureTypes.FEATURE_MARSH then
								marshCount = marshCount + 1;
							elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
								floodplainCount = floodplainCount + 1;
							elseif featureType == FeatureTypes.FEATURE_OASIS then
								oasisCount = oasisCount + 1;
							end
						end
					end
				end
			end
		end

		-- Assemble in to an array the recorded data for this region: 23 variables.
		local regionCounts = {
			totalPlots, areaPlots,
			waterCount, flatlandsCount, hillsCount, peaksCount,
			lakeCount, coastCount, oceanCount, iceCount,
			grassCount, plainsCount, desertCount, tundraCount, snowCount,
			forestCount, jungleCount, marshCount, riverCount, floodplainCount, oasisCount,
			coastalLandCount, nextToCoastCount,
		};
		--[[ Table Key:
		1) totalPlots
		2) areaPlots                 13) desertCount
		3) waterCount                14) tundraCount
		4) flatlandsCount            15) snowCount
		5) hillsCount                16) forestCount
		6) peaksCount                17) jungleCount
		7) lakeCount                 18) marshCount
		8) coastCount                19) riverCount
		9) oceanCount                20) floodplainCount
		10) iceCount                 21) oasisCount
		11) grassCount               22) coastalLandCount
		12) plainsCount              23) nextToCoastCount
		--]]

		-- Add array to the data table.
		table.insert(self.regionTerrainCounts, regionCounts);

		--[[ Activate printout only for debugging.
		print("-");
		print("--- Region Terrain Measurements for Region #", region_loop, "---");
		print("Total Plots: ", totalPlots);
		print("Area Plots: ", areaPlots);
		print("-");
		print("Mountains: ", peaksCount, " - Cannot belong to a landmass AreaID.");
		print("Total Water Plots: ", waterCount, " - Cannot belong to a landmass AreaID.");
		print("-");
		print("Lake Plots: ", lakeCount);
		print("Coast Plots: ", coastCount, " - Does not include Lakes.");
		print("Ocean Plots: ", oceanCount);
		print("Icebergs: ", iceCount);
		print("-");
		print("Flatlands: ", flatlandsCount);
		print("Hills: ", hillsCount);
		print("-");
		print("Grass Plots: ", grassCount);
		print("Plains Plots: ", plainsCount);
		print("Desert Plots: ", desertCount);
		print("Tundra Plots: ", tundraCount);
		print("Snow Plots: ", snowCount);
		print("-");
		print("Forest Plots: ", forestCount);
		print("Jungle Plots: ", jungleCount);
		print("Marsh Plots: ", marshCount);
		print("Flood Plains: ", floodplainCount);
		print("Oases: ", oasisCount);
		print("-");
		print("Plots Along Rivers: ", riverCount);
		print("Plots Along Oceans: ", coastalLandCount);
		print("Plots Next To Plots Along Oceans: ", nextToCoastCount);
		print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
		--]]
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:DetermineRegionTypes()
	-- Determine region type and conditions. Use self.regionTypes to store the results

	-- REGION TYPES
	-- 0. Undefined
	-- 1. Tundra
	-- 2. Jungle
	-- 3. Forest
	-- 4. Desert
	-- 5. Hills
	-- 6. Plains
	-- 7. Grassland
	-- 8. Hybrid
	-- 9. Mountain
	-- 10. Snow

	-- Main loop
	for this_region, terrainCounts in ipairs(self.regionTerrainCounts) do
		-- Set each region to "Undefined Type" as default.
		-- If all efforts fail at determining what type of region this should be, region type will remain Undefined.
		-- local totalPlots = terrainCounts[1];
		local regionPlots = terrainCounts[2];
		-- local waterCount = terrainCounts[3];
		local flatlandsCount = terrainCounts[4];
		local hillsCount = terrainCounts[5];
		-- local peaksCount = terrainCounts[6];
		-- local lakeCount = terrainCounts[7];
		-- local coastCount = terrainCounts[8];
		-- local oceanCount = terrainCounts[9];
		-- local iceCount = terrainCounts[10];
		local grassCount = terrainCounts[11];
		local plainsCount = terrainCounts[12];
		local desertCount = terrainCounts[13];
		local tundraCount = terrainCounts[14];
		-- local snowCount = terrainCounts[15];
		local forestCount = terrainCounts[16];
		local jungleCount = terrainCounts[17];
		-- local marshCount = terrainCounts[18];
		-- local riverCount = terrainCounts[19];
		-- local floodplainCount = terrainCounts[20];
		-- local oasisCount = terrainCounts[21];
		-- local coastalLandCount = terrainCounts[22];
		-- local nextToCoastCount = terrainCounts[23];

		-- If Rectangular regional division, then water plots would be included in region plots.
		-- Let's recalculate region plots based only on flatland and hills plots.
		if self.method == RegionDivision.RECTANGULAR or self.method == RegionDivision.RECTANGULAR_SELF_DEFINED then
			regionPlots = flatlandsCount + hillsCount;
		end


		-- MOD.Barathor:
		-- Totally revamped this entire function.
		-- With the old method, very dominant conditions could be missed, resulting in mislabeled regions.
		-- Now, a large decrementing value is added on to the base percentage requirements to filter out very dominant conditions first.

		local found_region = false;
		-- MOD.Barathor: These are the minimum values
		local desert_percent = 0.20;
		local tundra_percent = 0.20;
		local jungle_percent = 0.28;
		local forest_percent = 0.28;
		local hills_percent = 0.37;
		local plains_percent = 0.30;
		local grass_percent = 0.30;

		-- MOD.Barathor: This variable will decrement until a region is assigned; starts off very high.
		local adjustment = 0.50;

		-- MOD.Barathor: Reordered condition checks and modified what some checks include.
		while not found_region do
			-- Desert check.
			if desertCount >= regionPlots * (desert_percent + adjustment) then
				table.insert(self.regionTypes, RegionTypes.REGION_DESERT);
				print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
				print("Region #", this_region, " has been defined as a DESERT Region.");
				found_region = true;

			-- Tundra check.
			elseif tundraCount >= regionPlots * (tundra_percent + adjustment) then
				table.insert(self.regionTypes, RegionTypes.REGION_TUNDRA);
				print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
				print("Region #", this_region, " has been defined as a TUNDRA Region.");
				found_region = true;

			-- Jungle check.
			elseif jungleCount >= regionPlots * (jungle_percent + adjustment) then
				table.insert(self.regionTypes, RegionTypes.REGION_JUNGLE);
				print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
				print("Region #", this_region, " has been defined as a JUNGLE Region.");
				found_region = true;

			-- Forest check. Can't have too much tundra.
			elseif forestCount >= regionPlots * (forest_percent + adjustment) and tundraCount < regionPlots * (tundra_percent + adjustment) * 0.5 then
				table.insert(self.regionTypes, RegionTypes.REGION_FOREST);
				print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
				print("Region #", this_region, " has been defined as a FOREST Region.");
				found_region = true;

			-- Hills check.
			elseif hillsCount >= regionPlots * (hills_percent + adjustment) then
				table.insert(self.regionTypes, RegionTypes.REGION_HILLS);
				print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
				print("Region #", this_region, " has been defined as a HILLS Region.");
				found_region = true;

			else
				if adjustment <= 0 then
					-- Plains check.
					if plainsCount >= regionPlots * plains_percent and plainsCount * 0.8 > grassCount then
						table.insert(self.regionTypes, RegionTypes.REGION_PLAINS);
						print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
						print("Region #", this_region, " has been defined as a PLAINS Region.");
						found_region = true;
					-- Grass check.
					elseif grassCount >= regionPlots * grass_percent and grassCount * 0.8 > plainsCount then
						table.insert(self.regionTypes, RegionTypes.REGION_GRASSLAND);
						print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
						print("Region #", this_region, " has been defined as a GRASSLAND Region.");
						found_region = true;
					else
						-- Hybrid: No conditions dominate or other mods have included new terrain/feature/plot types which aren't recognized here.
						table.insert(self.regionTypes, RegionTypes.REGION_HYBRID);
						print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
						print("Region #", this_region, " has been defined as a HYBRID Region.");
						found_region = true;
					end
				end
				adjustment = adjustment - 0.01;
			end
		end

		---[[ MOD.Barathor: New data for very useful debug printouts.
		print("Threshold Values:");
		print(string.format("Desert: %.2f - Tundra: %.2f - Jungle: %.2f - Forest: %.2f - Hills: %.2f - Plains: %.2f - Grass: %.2f", desert_percent + adjustment, tundra_percent + adjustment, jungle_percent + adjustment, forest_percent + adjustment, hills_percent + adjustment, plains_percent + adjustment, grass_percent + adjustment));
		print("Region Values:");
		print(string.format("Desert: %.2f - Tundra: %.2f - Jungle: %.2f - Forest: %.2f - Hills: %.2f - Plains: %.2f - Grass: %.2f", desertCount / regionPlots, tundraCount / regionPlots, jungleCount / regionPlots, forestCount / regionPlots, hillsCount / regionPlots, plainsCount / regionPlots, grassCount / regionPlots));
		print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
		--]]

	end

	-- Added by azum4roll: Support for Mountain and Snow "bias"
	-- This changes one region to a "Mountain region" or "Snow region", if a civ with the respective start bias exists
	-- It's possible that there's no snow/mountain on the map, so we need to handle it later in BalanceAndAssign

	if self.hasSnowBias then
		-- Get ID of region with the most snow, prioritizing tundra regions first
		local snowRegion = -1;
		local snowCount = 0;
		for this_region, terrainCounts in ipairs(self.regionTerrainCounts) do
			local regionType = self.regionTypes[this_region];
			local thisSnowCount = terrainCounts[15];
			if regionType == RegionTypes.REGION_TUNDRA then
				thisSnowCount = thisSnowCount + 1000;
			end
			if thisSnowCount > snowCount then
				snowRegion = this_region;
				snowCount = thisSnowCount;
			end
		end

		if snowRegion ~= -1 then
			self.regionTypes[snowRegion] = RegionTypes.REGION_SNOW;
			print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
			print("Region #", snowRegion, " has been converted into a SNOW Region.");
		end
	end

	if self.hasMountainBias then
		-- Get ID of region with the most mountains
		local mountainRegion = -1;
		local mountainCount = 0;
		for this_region, terrainCounts in ipairs(self.regionTerrainCounts) do
			local regionType = self.regionTypes[this_region];
			local thisMountainCount = terrainCounts[6];
			if thisMountainCount > mountainCount and regionType ~= RegionTypes.REGION_SNOW then
				mountainRegion = this_region;
				mountainCount = thisMountainCount;
			end
		end

		if mountainRegion ~= -1 then
			self.regionTypes[mountainRegion] = RegionTypes.REGION_MOUNTAIN;
			print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
			print("Region #", mountainRegion, " has been converted into a MOUNTAIN Region.");
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceImpactAndRipples(x, y)
	-- This function operates upon the "impact and ripple" data overlays. This is the core version, which operates on start points.
	-- Resources and city states have their own data layers, using this same design principle.
	-- Execution of this function handles a single start point (x, y).

	-- The purpose of the overlay is to strongly discourage placement of new start points near already-placed start points.
	-- Each start placed makes an "impact" on the map, and this impact "ripples" outward in rings, each ring weaker in bias than the previous ring.

	-- Civ4 attempted to adjust the minimum distance between civs according to a formula that factored map size and number of civs in the game,
	-- but the formula was chock full of faulty assumptions, resulting in an accurate calibration rate of less than ten percent.
	-- The failure of this approach is the primary reason that an all-new positioner was written for Civ5.

	-- Rather than repeating the mistakes of the old system, in part or in whole,
	-- I have opted to go with a flat 9-tile impact crater for all map sizes and number of civs.
	-- The new system will place civs at least 9 tiles away from other civs whenever and wherever a reasonable candidate plot exists at this range.
	-- If a start must be found within that range, it will attempt to balance quality of the location against proximity to another civ,
	-- with the bias becoming very heavy inside 7 plots, and all but prohibitive inside 5.
	-- The only starts that should see any Civs crowding together are those with impossible conditions,
	-- such as cramming more than a dozen civs on to Tiny or Duel sized maps.

	-- The Impact and Ripple is aimed mostly at assisting with Rectangular Method regional division on islands maps,
	-- as the primary method of spacing civs is the Center Bias factor.
	-- The Impact and Ripple is a second layer of protection, for those rare cases when regional shapes are severely distorted,
	-- with little to no land in the region center, and the start having to be placed near the edge, and for cases of extremely thin regional dimension.

	-- To establish a bias of 9, we Impact the overlay and Ripple outward 8 times.
	-- Value of 0 in a plot means no influence from existing Impacts in that plot.
	-- Value of 99 means an Impact occurred in that plot and it IS a start point.
	-- Values > 0 and < 99 are "ripples", meaning that plot is near a start point.
	local iW, iH = Map.GetGridSize();
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local impact_value = 99;
	local ripple_values = {97, 95, 92, 89, 69, 57, 24, 15};
	local odd = self.firstRingYIsOdd;
	local even = self.firstRingYIsEven;
	local nextX, nextY, plot_adjustments;

	-- Start points need to impact the resource layers, so let's handle that first.

	-- Strategic layers, at impact site only.
	self:PlaceStrategicResourceImpact(x, y, 0);

	-- Luxury and Bonus layers, set all plots within this civ start as off limits. Will be individually handled later.
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_LUXURY, 3);
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_BONUS, 3);
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_FISH, 3);

	-- Natural Wonders cannot appear within 4 plots of a civ's starting point
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_NATURAL_WONDER, 4);

	-- Now the main data layer, for start points themselves, and the City State data layer.
	-- Place Impact!
	local impactPlotIndex = y * iW + x + 1;
	self.distanceData[impactPlotIndex] = impact_value;
	self.playerCollisionData[impactPlotIndex] = true;
	self.impactData[ImpactLayers.LAYER_CITY_STATE][impactPlotIndex] = 1;
	-- Place Ripples
	for ripple_radius, ripple_value in ipairs(ripple_values) do
		-- Moving clockwise around the ring, the first direction to travel will be Northeast.
		-- This matches the direction-based data in the odd and even tables. Each
		-- subsequent change in direction will correctly match with these tables, too.

		-- Locate the plot within this ripple ring that is due West of the Impact Plot.
		local currentX = x - ripple_radius;
		local currentY = y;
		-- Now loop through the six directions, moving ripple_radius number of times
		-- per direction. At each plot in the ring, add the ripple_value for that ring
		-- to the plot's entry in the distance data table.
		for direction_index = 1, 6 do
			for _ = 1, ripple_radius do
				-- Must account for hex factor.
			 	if currentY / 2 > math.floor(currentY / 2) then -- Current Y is odd. Use odd table.
					plot_adjustments = odd[direction_index];
				else -- Current Y is even. Use plot adjustments from even table.
					plot_adjustments = even[direction_index];
				end
				-- Identify the next plot in the ring.
				nextX = currentX + plot_adjustments[1];
				nextY = currentY + plot_adjustments[2];
				-- Make sure the plot exists
				if not wrapX and (nextX < 0 or nextX >= iW) then -- X is out of bounds.
					-- Do not add ripple data to this plot.
				elseif not wrapY and (nextY < 0 or nextY >= iH) then -- Y is out of bounds.
					-- Do not add ripple data to this plot.
				else -- Plot is in bounds, process it.
					-- Handle any world wrap.
					local realX = nextX;
					local realY = nextY;
					if wrapX then
						realX = realX % iW;
					end
					if wrapY then
						realY = realY % iH;
					end
					-- Record ripple data for this plot.
					local ringPlotIndex = realY * iW + realX + 1;
					if self.distanceData[ringPlotIndex] > 0 then -- This plot is already in range of at least one other civ!
						-- First choose the greater of the two, existing value or current ripple.
						local stronger_value = math.max(self.distanceData[ringPlotIndex], ripple_value);
						-- Now increase it by 1.2x to reflect that multiple civs are in range of this plot.
						local overlap_value = math.min(97, math.floor(stronger_value * 1.2));
						self.distanceData[ringPlotIndex] = overlap_value;
					else
						self.distanceData[ringPlotIndex] = ripple_value;
					end
					-- Now impact the City State layer if appropriate.
					if ripple_radius <= 6 then
						self.impactData[ImpactLayers.LAYER_CITY_STATE][ringPlotIndex] = 1;
					end
				end
				currentX, currentY = nextX, nextY;
			end
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:MeasureSinglePlot(x, y, region_type)
	local data = table.fill(false, 4);
	-- Note that "Food" is not strictly about tile yield.
	-- Different regions get their food in different ways.
	-- Tundra, Jungle, Forest, Desert, Plains regions will get Bonus resource support to cover food shortages.

	-- Data table entries hold results; all begin as false:
	-- [1] "Food"
	-- [2] "Prod"
	-- [3] "Good"
	-- [4] "Junk"
	local plot = Map.GetPlot(x, y);
	local plotType = plot:GetPlotType();
	local terrainType = plot:GetTerrainType();
	local featureType = plot:GetFeatureType();

	if plotType == PlotTypes.PLOT_MOUNTAIN then -- Mountains are Junk, except in Mountain regions (made for mountain bias)
		if region_type ~= RegionTypes.REGION_MOUNTAIN then
			data[4] = true;
		else
			data[1] = true;
			data[3] = true;
		end
		return data;
	elseif plotType == PlotTypes.PLOT_OCEAN then
		if featureType == FeatureTypes.FEATURE_ICE then -- Icebergs are Junk, except in snow bias where it's ignored.
			if region_type ~= RegionTypes.REGION_SNOW then
				data[4] = true;
			end
			return data;
		elseif plot:IsLake() then -- Lakes are Food, Good.
			data[1] = true;
			data[3] = true;
			return data;
		elseif self.method == RegionDivision.RECTANGULAR or self.method == RegionDivision.RECTANGULAR_SELF_DEFINED then
			if terrainType == TerrainTypes.TERRAIN_COAST then -- Shallow water is Good for Archipelago-type maps.
				data[3] = true;
				return data;
			end
		elseif terrainType == TerrainTypes.TERRAIN_COAST and region_type == RegionTypes.REGION_SNOW then -- Give more coastal score for snow bias
			data[3] = true;
			return data;
		end
		-- Other water plots are ignored.
		return data;
	end

	if featureType == FeatureTypes.FEATURE_JUNGLE then -- Jungles are Food, Good, except in Grass regions.
		if region_type ~= RegionTypes.REGION_GRASSLAND then
			data[1] = true;
			data[3] = true;
		elseif plotType == PlotTypes.PLOT_HILLS then -- Jungle hill, in grass region, count as Prod but not Good.
			data[2] = true;
		end
		return data;
	elseif featureType == FeatureTypes.FEATURE_FOREST then -- Forests are Prod, Good.
		data[2] = true;
		data[3] = true;
		if region_type == RegionTypes.REGION_FOREST or
			region_type == RegionTypes.REGION_TUNDRA or
			region_type == RegionTypes.REGION_SNOW then
			data[1] = true;
		end
		return data;
	elseif featureType == FeatureTypes.FEATURE_OASIS then -- Oases are Food, Good.
		data[1] = true;
		data[3] = true;
		return data;
	elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then -- Flood Plains are Food, Good.
		data[1] = true;
		data[3] = true;
		return data;
	elseif featureType == FeatureTypes.FEATURE_MARSH then -- Marsh are ignored.
		return data;
	end

	if plotType == PlotTypes.PLOT_HILLS and terrainType ~= TerrainTypes.TERRAIN_SNOW then -- Non-Snow Hills with no features are Prod, Good.
		data[2] = true;
		data[3] = true;
		return data;
	end

	-- If we have reached this point in the process, the plot is flatlands and snow.
	if terrainType == TerrainTypes.TERRAIN_SNOW then -- Snow are Junk, except in Snow regions (made for snow bias)
		if region_type ~= RegionTypes.REGION_SNOW then
			data[4] = true;
		else
			data[1] = true;
			data[2] = true;
			data[3] = true;
		end
		return data;

	elseif terrainType == TerrainTypes.TERRAIN_DESERT then -- Non-Oasis, non-FloodPlain flat deserts are Junk, except in Desert regions.
		if region_type ~= RegionTypes.REGION_DESERT then
			data[4] = true;
		end
		return data;

	elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then -- Tundra are ignored, except in Tundra/Snow Regions where they are Food, Good.
		if region_type == RegionTypes.REGION_TUNDRA then
			data[1] = true;
			data[3] = true;
		end
		if region_type == RegionTypes.REGION_SNOW then
			data[1] = true;
			data[2] = true;
			data[3] = true;
		end
		return data;

	elseif terrainType == TerrainTypes.TERRAIN_PLAINS then -- Plains are Good for all region types, but Food in only about half of them.
		if region_type ~= RegionTypes.REGION_SNOW then
			data[3] = true;
		end
		if region_type == RegionTypes.REGION_TUNDRA or
			region_type == RegionTypes.REGION_DESERT or
			region_type == RegionTypes.REGION_HILLS or
			region_type == RegionTypes.REGION_PLAINS or
			region_type == RegionTypes.REGION_HYBRID or
			region_type == RegionTypes.REGION_MOUNTAIN then
			data[1] = true;
		end
		return data;

	elseif terrainType == TerrainTypes.TERRAIN_GRASS then -- Grass is Good for all region types, but Food in only about half of them.
		if region_type ~= RegionTypes.REGION_SNOW then
			data[3] = true;
		end
		if region_type == RegionTypes.REGION_JUNGLE or
			region_type == RegionTypes.REGION_FOREST or
			region_type == RegionTypes.REGION_HILLS or
			region_type == RegionTypes.REGION_GRASSLAND or
			region_type == RegionTypes.REGION_HYBRID or
			region_type == RegionTypes.REGION_MOUNTAIN or
			region_type == RegionTypes.REGION_SNOW then
			data[1] = true;
		end
		return data;
	end

	-- If we have arrived here, the plot has non-standard terrain.
	-- print("Encountered non-standard terrain.");
	return data;
end
------------------------------------------------------------------------------
function AssignStartingPlots:EvaluateCandidatePlot(plotIndex, region_type)
	local goodSoFar = true;
	local iW, iH = Map.GetGridSize();
	local x = (plotIndex - 1) % iW;
	local y = (plotIndex - x - 1) / iW;
	local plot = Map.GetPlot(x, y);
	local isEvenY = true;
	if y / 2 > math.floor(y / 2) then
		isEvenY = false;
	end
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local distance_bias = self.distanceData[plotIndex];
	local foodTotal, prodTotal, goodTotal, junkTotal, riverTotal, mountainTotal, coastScore = 0, 0, 0, 0, 0, 0, 0;
	local search_table = {};

	-- Check candidate plot to see if it's adjacent to saltwater. Add higher score for snow bias
	if self.plotDataIsCoastal[plotIndex] then
		if region_type == RegionTypes.REGION_SNOW then
			coastScore = 120;
		else
			coastScore = 60;
		end
	end

	-- Evaluate First Ring
	if isEvenY then
		search_table = self.firstRingYIsEven;
	else
		search_table = self.firstRingYIsOdd;
	end

	for _, plot_adjustments in ipairs(search_table) do
		local searchX, searchY;
		if wrapX then
			searchX = (x + plot_adjustments[1]) % iW;
		else
			searchX = x + plot_adjustments[1];
		end
		if wrapY then
			searchY = (y + plot_adjustments[2]) % iH;
		else
			searchY = y + plot_adjustments[2];
		end

		if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then
			-- This plot does not exist. It's off the map edge.
			junkTotal = junkTotal + 1;
		else
			if Map.GetPlot(searchX, searchY):GetPlotType() == PlotTypes.PLOT_MOUNTAIN then
				mountainTotal = mountainTotal + 1;
			end
			local result = self:MeasureSinglePlot(searchX, searchY, region_type);
			if result[4] then
				junkTotal = junkTotal + 1;
			else
				if result[1] then
					foodTotal = foodTotal + 1;
				end
				if result[2] then
					prodTotal = prodTotal + 1;
				end
				if result[3] then
					goodTotal = goodTotal + 1;
				end
				local searchPlot = Map.GetPlot(searchX, searchY);
				if searchPlot:IsRiverSide() then
					riverTotal = riverTotal + 1;
				end
			end
		end
	end

	-- Now check the results from the first ring against the established targets.
	if foodTotal < self.minFoodInner then
		goodSoFar = false;
	elseif prodTotal < self.minProdInner then
		goodSoFar = false;
	elseif goodTotal < self.minGoodInner then
		goodSoFar = false;
	end

	-- Set up the "score" for this candidate. Inner ring results weigh the heaviest.
	local weightedFoodInner = {0, 8, 14, 18, 19, 18, 14}; -- too much food is bad
	local foodResultInner = weightedFoodInner[foodTotal + 1];
	local weightedProdInner = {0, 10, 16, 20, 22, 24, 25};
	local prodResultInner = weightedProdInner[prodTotal + 1];
	local goodResultInner = goodTotal * 2;
	local innerRingScore = foodResultInner + prodResultInner + goodResultInner + riverTotal - (junkTotal * 3);

	if region_type == RegionTypes.REGION_MOUNTAIN then
		innerRingScore = innerRingScore + mountainTotal * 3;
	end

	-- Evaluate Second Ring
	if isEvenY then
		search_table = self.secondRingYIsEven;
	else
		search_table = self.secondRingYIsOdd;
	end

	for _, plot_adjustments in ipairs(search_table) do
		local searchX, searchY;
		if wrapX then
			searchX = (x + plot_adjustments[1]) % iW;
		else
			searchX = x + plot_adjustments[1];
		end
		if wrapY then
			searchY = (y + plot_adjustments[2]) % iH;
		else
			searchY = y + plot_adjustments[2];
		end
		if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then
			-- This plot does not exist. It's off the map edge.
			junkTotal = junkTotal + 1;
		else
			if Map.GetPlot(searchX, searchY):GetPlotType() == PlotTypes.PLOT_MOUNTAIN then
				mountainTotal = mountainTotal + 1;
			end
			local result = self:MeasureSinglePlot(searchX, searchY, region_type);
			if result[4] then
				junkTotal = junkTotal + 1;
			else
				if result[1] then
					foodTotal = foodTotal + 1;
				end
				if result[2] then
					prodTotal = prodTotal + 1;
				end
				if result[3] then
					goodTotal = goodTotal + 1;
				end
				if plot:IsRiverSide() then
					riverTotal = riverTotal + 1;
				end
			end
		end
	end

	-- Check the results from the second ring against the established targets.
	if foodTotal < self.minFoodMiddle then
		goodSoFar = false;
	elseif prodTotal < self.minProdMiddle then
		goodSoFar = false;
	elseif goodTotal < self.minGoodMiddle then
		goodSoFar = false;
	end

	-- Update up the "score" for this candidate. Middle ring results weigh significantly.
	local weightedFoodMiddle = {0, 2, 5, 10, 20, 25, 28, 30, 32, 34, 35}; -- 35 for any further values.
	local foodResultMiddle = 35;
	if foodTotal < 10 then
		foodResultMiddle = weightedFoodMiddle[foodTotal + 1];
	end
	local weightedProdMiddle = {0, 10, 20, 25, 30, 35, 39, 42, 44, 45}; -- 45 for any further values.
	local effectiveProdTotal = prodTotal;
	if foodTotal * 2 < prodTotal then
		effectiveProdTotal = math.ceil(foodTotal / 2);
	end
	local prodResultMiddle = 35;
	if effectiveProdTotal < 5 then
		prodResultMiddle = weightedProdMiddle[effectiveProdTotal + 1];
	end
	local goodResultMiddle = goodTotal * 2;
	local middleRingScore = foodResultMiddle + prodResultMiddle + goodResultMiddle + riverTotal - (junkTotal * 3);

	if region_type == RegionTypes.REGION_MOUNTAIN then
		middleRingScore = middleRingScore + mountainTotal * 2;
	end

	-- Evaluate Third Ring
	if isEvenY then
		search_table = self.thirdRingYIsEven;
	else
		search_table = self.thirdRingYIsOdd;
	end

	for _, plot_adjustments in ipairs(search_table) do
		local searchX, searchY;
		if wrapX then
			searchX = (x + plot_adjustments[1]) % iW;
		else
			searchX = x + plot_adjustments[1];
		end
		if wrapY then
			searchY = (y + plot_adjustments[2]) % iH;
		else
			searchY = y + plot_adjustments[2];
		end
		if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then
			-- This plot does not exist. It's off the map edge.
			junkTotal = junkTotal + 1;
		else
			if Map.GetPlot(searchX, searchY):GetPlotType() == PlotTypes.PLOT_MOUNTAIN then
				mountainTotal = mountainTotal + 1;
			end
			local result = self:MeasureSinglePlot(searchX, searchY, region_type);
			if result[4] then
				junkTotal = junkTotal + 1;
			else
				if result[1] then
					foodTotal = foodTotal + 1;
				end
				if result[2] then
					prodTotal = prodTotal + 1;
				end
				if result[3] then
					goodTotal = goodTotal + 1;
				end
				if plot:IsRiverSide() then
					riverTotal = riverTotal + 1;
				end
			end
		end
	end

	-- Check the results from the third ring against the established targets.
	if foodTotal < self.minFoodOuter then
		goodSoFar = false;
	elseif prodTotal < self.minProdOuter then
		goodSoFar = false;
	elseif goodTotal < self.minGoodOuter then
		goodSoFar = false;
	end
	if junkTotal > self.maxJunk then
		goodSoFar = false;
	end

	-- Tally the final "score" for this candidate.
	local outerRingScore = foodTotal + prodTotal + goodTotal + riverTotal - junkTotal * 2;
	if region_type == RegionTypes.REGION_MOUNTAIN then
		outerRingScore = outerRingScore + mountainTotal;
	end
	local finalScore = innerRingScore + middleRingScore + outerRingScore + coastScore;

	-- Check Impact and Ripple data to see if candidate is near an already-placed start point.
	if distance_bias > 0 then
		-- This candidate is near an already placed start. This invalidates its
		-- eligibility for first-pass placement; but it may still qualify as a
		-- fallback site, so we will reduce its Score according to the bias factor.
		goodSoFar = false;
		finalScore = finalScore - math.floor(finalScore * distance_bias / 100);
	end

	--[[ Debug
	print(".");
	print("Plot:", x, y, " Food:", foodTotal, "Prod: ", prodTotal, "Good:", goodTotal, "Junk:",
	       junkTotal, "River:", riverTotal, "Score:", finalScore);
	print("Plot:", x, y, " Coastal:", self.plotDataIsCoastal[plotIndex], "Distance Bias:", distance_bias);
	--]]

	return finalScore, goodSoFar;
end
------------------------------------------------------------------------------
function AssignStartingPlots:IterateThroughCandidatePlotList(plot_list, region_type)
	-- Iterates through a list of candidate plots.
	-- Each plot is identified by its global plot index.
	-- This function assumes all candidate plots can have a city built on them.
	-- Any plots not allowed to have a city should be weeded out when building the candidate list.
	local found_eligible = false;
	local bestPlotScore = -5000;
	local bestPlotIndex;
	local found_fallback = false;
	local bestFallbackScore = -5000;
	local bestFallbackIndex;
	-- Process list of candidate plots.
	for loop, plotIndex in ipairs(plot_list) do
		local score, meets_minimums = self:EvaluateCandidatePlot(plotIndex, region_type);
		-- Test current plot against best known plot.
		if meets_minimums then
			found_eligible = true;
			if score > bestPlotScore then
				bestPlotScore = score;
				bestPlotIndex = plotIndex;
			end
		else
			found_fallback = true;
			if score > bestFallbackScore then
				bestFallbackScore = score;
				bestFallbackIndex = plotIndex;
			end
		end
	end
	-- returns table containing six variables: boolean, integer, integer, boolean, integer, integer
	local election_results = {found_eligible, bestPlotScore, bestPlotIndex, found_fallback, bestFallbackScore, bestFallbackIndex};
	return election_results;
end
------------------------------------------------------------------------------
function AssignStartingPlots:FindStart(region_number)
	-- This function attempts to choose a start position for a single region.
	-- This function returns two boolean flags, indicating the success level of the operation.
	local bSuccessFlag = false; -- Returns true when a start is placed, false when process fails.
	local bForcedPlacementFlag = false; -- Returns true if this region had no eligible starts and one was forced to occur.

	-- Obtain data needed to process this region.
	local iW, iH = Map.GetGridSize();
	local region_data_table = self.regionData[region_number];
	local iWestX = region_data_table[1];
	local iSouthY = region_data_table[2];
	local iWidth = region_data_table[3];
	local iHeight = region_data_table[4];
	local iAreaID = region_data_table[5];

	local region_type = self.regionTypes[region_number];
	-- Done setting up region data.
	-- Set up contingency.
	local fallback_plots = {};

	-- Establish scope of center bias.
	local fCenterWidth = (self.centerBias / 100) * iWidth;
	local iNonCenterWidth = math.floor((iWidth - fCenterWidth) / 2);
	local iCenterWidth = iWidth - (iNonCenterWidth * 2);
	local iCenterWestX = (iWestX + iNonCenterWidth) % iW; -- Modulo math to synch coordinate to actual map in case of world wrap.
	local iCenterTestWestX = (iWestX + iNonCenterWidth); -- "Test" values ignore world wrap for easier membership testing.
	local iCenterTestEastX = (iCenterWestX + iCenterWidth - 1);

	local fCenterHeight = (self.centerBias / 100) * iHeight;
	local iNonCenterHeight = math.floor((iHeight - fCenterHeight) / 2);
	local iCenterHeight = iHeight - (iNonCenterHeight * 2);
	-- local iCenterSouthY = (iSouthY + iNonCenterHeight) % iH;
	local iCenterTestSouthY = (iSouthY + iNonCenterHeight);
	local iCenterTestNorthY = (iCenterTestSouthY + iCenterHeight - 1);

	-- Establish scope of "middle donut", outside the center but inside the outer.
	local fMiddleWidth = (self.middleBias / 100) * iWidth;
	local iOuterWidth = math.floor((iWidth - fMiddleWidth) / 2);
	local iMiddleWidth = iWidth - (iOuterWidth * 2);
	-- local iMiddleWestX = (iWestX + iOuterWidth) % iW;
	local iMiddleTestWestX = (iWestX + iOuterWidth);
	local iMiddleTestEastX = (iMiddleTestWestX + iMiddleWidth - 1);

	local fMiddleHeight = (self.middleBias / 100) * iHeight;
	local iOuterHeight = math.floor((iHeight - fMiddleHeight) / 2);
	local iMiddleHeight = iHeight - (iOuterHeight * 2);
	-- local iMiddleSouthY = (iSouthY + iOuterHeight) % iH;
	local iMiddleTestSouthY = (iSouthY + iOuterHeight);
	local iMiddleTestNorthY = (iMiddleTestSouthY + iMiddleHeight - 1);

	-- Assemble candidates lists.
	local two_plots_from_ocean = {};
	local center_candidates = {};
	local center_river = {};
	local center_coastal = {};
	local center_inland_dry = {};
	local middle_candidates = {};
	local middle_river = {};
	local middle_coastal = {};
	local middle_inland_dry = {};
	local outer_plots = {};

	-- Identify candidate plots.
	for region_y = 0, iHeight - 1 do -- When handling global plot indices, process Y first.
		for region_x = 0, iWidth - 1 do
			local x = (region_x + iWestX) % iW; -- Actual coords, adjusted for world wrap, if any.
			local y = (region_y + iSouthY) % iH;
			local plotIndex = y * iW + x + 1;
			local plot = Map.GetPlot(x, y);
			local plotType = plot:GetPlotType();
			if plotType == PlotTypes.PLOT_HILLS or plotType == PlotTypes.PLOT_LAND then -- Could host a city.
				-- Check if plot is two away from salt water.
				if self.plotDataIsNextToCoast[plotIndex] then
					table.insert(two_plots_from_ocean, plotIndex);
				else
					local area_of_plot = plot:GetArea();
					local landmass_of_plot = plot:GetLandmass();
					if iAreaID == -1 or (self.bArea and area_of_plot == iAreaID) or (not self.bArea and landmass_of_plot == iAreaID) then
						-- This plot is a member, so it goes on at least one candidate list.
						-- Test whether plot is in center bias, middle donut, or outer donut.
						local test_x = region_x + iWestX; -- "Test" coords, ignoring any world wrap and
						local test_y = region_y + iSouthY; -- reaching in to virtual space if necessary.
						if (test_x >= iCenterTestWestX and test_x <= iCenterTestEastX) and
							(test_y >= iCenterTestSouthY and test_y <= iCenterTestNorthY) then -- Center Bias.
							table.insert(center_candidates, plotIndex);
							if plot:IsRiverSide() then
								table.insert(center_river, plotIndex);
							elseif plot:IsFreshWater() or self.plotDataIsCoastal[plotIndex] then
								table.insert(center_coastal, plotIndex);
							else
								table.insert(center_inland_dry, plotIndex);
							end
						elseif (test_x >= iMiddleTestWestX and test_x <= iMiddleTestEastX) and
							(test_y >= iMiddleTestSouthY and test_y <= iMiddleTestNorthY) then
							table.insert(middle_candidates, plotIndex);
							if plot:IsRiverSide() then
								table.insert(middle_river, plotIndex);
							elseif plot:IsFreshWater() or self.plotDataIsCoastal[plotIndex] then
								table.insert(middle_coastal, plotIndex);
							else
								table.insert(middle_inland_dry, plotIndex);
							end
						else
							table.insert(outer_plots, plotIndex);
						end
					end
				end
			end
		end
	end

	-- Check how many plots landed on each list.
	-- local iNumDisqualified = table.maxn(two_plots_from_ocean);
	local iNumCenter = table.maxn(center_candidates);
	local iNumCenterRiver = table.maxn(center_river);
	local iNumCenterCoastLake = table.maxn(center_coastal);
	local iNumCenterInlandDry = table.maxn(center_inland_dry);
	local iNumMiddle = table.maxn(middle_candidates);
	local iNumMiddleRiver = table.maxn(middle_river);
	local iNumMiddleCoastLake = table.maxn(middle_coastal);
	local iNumMiddleInlandDry = table.maxn(middle_inland_dry);
	local iNumOuter = table.maxn(outer_plots);

	--[[ Debug printout.
	print("-");
	print("--- Number of Candidate Plots in Region #", region_number, " - Region Type:", region_type, " ---");
	print("-");
	print("Candidates in Center Bias area: ", iNumCenter);
	print("Which are next to river: ", iNumCenterRiver);
	print("Which are next to lake or sea: ", iNumCenterCoastLake);
	print("Which are inland and dry: ", iNumCenterInlandDry);
	print("-");
	print("Candidates in Middle Donut area: ", iNumMiddle);
	print("Which are next to river: ", iNumMiddleRiver);
	print("Which are next to lake or sea: ", iNumMiddleCoastLake);
	print("Which are inland and dry: ", iNumMiddleInlandDry);
	print("-");
	print("Candidate Plots in Outer area: ", iNumOuter);
	print("-");
	print("Disqualified, two plots away from salt water: ", iNumDisqualified);
	print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
	--]]

	-- Process lists of candidate plots.
	if iNumCenter + iNumMiddle > 0 then
		local candidate_lists = {};
		if iNumCenterRiver > 0 then -- Process center bias river plots.
			table.insert(candidate_lists, center_river);
		end
		if iNumCenterCoastLake > 0 then -- Process center bias lake or coastal plots.
			table.insert(candidate_lists, center_coastal);
		end
		if iNumCenterInlandDry > 0 then -- Process center bias inland dry plots.
			table.insert(candidate_lists, center_inland_dry);
		end
		if iNumMiddleRiver > 0 then -- Process middle donut river plots.
			table.insert(candidate_lists, middle_river);
		end
		if iNumMiddleCoastLake > 0 then -- Process middle donut lake or coastal plots.
			table.insert(candidate_lists, middle_coastal);
		end
		if iNumMiddleInlandDry > 0 then -- Process middle donut inland dry plots.
			table.insert(candidate_lists, middle_inland_dry);
		end

		for _, plot_list in ipairs(candidate_lists) do -- Up to six plot lists, processed by priority.
			local election_returns = self:IterateThroughCandidatePlotList(plot_list, region_type);
			-- If any candidates are eligible, choose one.
			local found_eligible = election_returns[1];
			if found_eligible then
				local bestPlotScore = election_returns[2];
				print("Region #", region_number, "has", bestPlotScore, "score.");
				local bestPlotIndex = election_returns[3];
				local x = (bestPlotIndex - 1) % iW;
				local y = (bestPlotIndex - x - 1) / iW;
				self.startingPlots[region_number] = {x, y, bestPlotScore};
				self:PlaceImpactAndRipples(x, y);
				return true, false;
			end
			-- If none eligible, check for fallback plot.
			local found_fallback = election_returns[4];
			if found_fallback then
				local bestFallbackScore = election_returns[5];
				print("Region #", region_number, "has", bestFallbackScore, "fallback score.");
				local bestFallbackIndex = election_returns[6];
				local x = (bestFallbackIndex - 1) % iW;
				local y = (bestFallbackIndex - x - 1) / iW;
				table.insert(fallback_plots, {x, y, bestFallbackScore});
			end
		end
	end
	-- Reaching this point means no eligible sites in center bias or middle donut subregions!

	-- Process candidates from Outer subregion, if any.
	if iNumOuter > 0 then
		local outer_eligible_list = {};
		local found_eligible = false;
		local found_fallback = false;
		local bestFallbackScore = -50;
		local bestFallbackIndex;
		-- Process list of candidate plots.
		for _, plotIndex in ipairs(outer_plots) do
			local score, meets_minimums = self:EvaluateCandidatePlot(plotIndex, region_type);
			-- Test current plot against best known plot.
			if meets_minimums then
				found_eligible = true;
				table.insert(outer_eligible_list, plotIndex);
			else
				found_fallback = true;
				if score > bestFallbackScore then
					bestFallbackScore = score;
					bestFallbackIndex = plotIndex;
				end
			end
		end
		if found_eligible then -- Iterate through eligible plots and choose the one closest to the center of the region.
			local closestPlot;
			local closestDistance = math.max(iW, iH);
			local bullseyeX = iWestX + (iWidth / 2);
			if bullseyeX < iWestX then -- wrapped around: un-wrap it for test purposes.
				bullseyeX = bullseyeX + iW;
			end
			local bullseyeY = iSouthY + (iHeight / 2);
			if bullseyeY < iSouthY then -- wrapped around: un-wrap it for test purposes.
				bullseyeY = bullseyeY + iH;
			end
			if bullseyeY / 2 ~= math.floor(bullseyeY / 2) then -- Y coord is odd, add .5 to X coord for hex-shift.
				bullseyeX = bullseyeX + 0.5;
			end

			for _, plotIndex in ipairs(outer_eligible_list) do
				local x = (plotIndex - 1) % iW;
				local y = (plotIndex - x - 1) / iW;
				local adjusted_x = x;
				local adjusted_y = y;
				if y / 2 ~= math.floor(y / 2) then -- Y coord is odd, add .5 to X coord for hex-shift.
					adjusted_x = x + 0.5;
				end

				if x < iWestX then -- wrapped around: un-wrap it for test purposes.
					adjusted_x = adjusted_x + iW;
				end
				if y < iSouthY then -- wrapped around: un-wrap it for test purposes.
					adjusted_y = y + iH;
				end
				local fDistance = math.sqrt((adjusted_x - bullseyeX) ^ 2 + (adjusted_y - bullseyeY) ^ 2);
				if fDistance < closestDistance then -- Found new "closer" plot.
					closestPlot = plotIndex;
					closestDistance = fDistance;
				end
			end
			-- Assign the closest eligible plot as the start point.
			local x = (closestPlot - 1) % iW;
			local y = (closestPlot - x - 1) / iW;
			-- Re-get plot score for inclusion in start plot data.
			local score, _ = self:EvaluateCandidatePlot(closestPlot, region_type);
			-- Assign this plot as the start for this region.
			self.startingPlots[region_number] = {x, y, score};
			self:PlaceImpactAndRipples(x, y);
			return true, false;
		end
		-- Add the fallback plot (best scored plot) from the Outer region to the fallback list.
		if found_fallback then
			local x = (bestFallbackIndex - 1) % iW;
			local y = (bestFallbackIndex - x - 1) / iW;
			table.insert(fallback_plots, {x, y, bestFallbackScore});
		end
	end
	-- Reaching here means no plot in the entire region met the minimum standards for selection.

	-- The fallback plot contains the best-scored plots from each test area in this region.
	-- We will compare all the fallback plots and choose the best to be the start plot.
	local iNumFallbacks = table.maxn(fallback_plots);
	if iNumFallbacks > 0 then
		local best_fallback_score = 0
		local best_fallback_x;
		local best_fallback_y;
		for _, plotData in ipairs(fallback_plots) do
			local score = plotData[3];
			if score > best_fallback_score then
				best_fallback_score = score;
				best_fallback_x = plotData[1];
				best_fallback_y = plotData[2];
			end
		end
		-- Assign the start for this region.
		self.startingPlots[region_number] = {best_fallback_x, best_fallback_y, best_fallback_score};
		self:PlaceImpactAndRipples(best_fallback_x, best_fallback_y);
		bSuccessFlag = true;
	else
		-- This region cannot have a start and something has gone way wrong.
		-- We'll force a one tile grass island in the SW corner of the region and put the start there.
		local forcePlot = Map.GetPlot(iWestX, iSouthY);
		bSuccessFlag = true;
		bForcedPlacementFlag = true;
		forcePlot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
		forcePlot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true);
		forcePlot:SetFeatureType(FeatureTypes.NO_FEATURE);
		self.startingPlots[region_number] = {iWestX, iSouthY, 0};
		self:PlaceImpactAndRipples(iWestX, iSouthY);
	end

	return bSuccessFlag, bForcedPlacementFlag;
end
------------------------------------------------------------------------------
function AssignStartingPlots:FindCoastalStart(region_number)
	-- This function attempts to choose a start position (which is along an ocean) for a single region.
	-- This function returns two boolean flags, indicating the success level of the operation.
	local bSuccessFlag = false; -- Returns true when a start is placed, false when process fails.
	local bForcedPlacementFlag = false; -- Returns true if this region had no eligible starts and one was forced to occur.

	-- Obtain data needed to process this region.
	local iW, iH = Map.GetGridSize();
	local region_data_table = self.regionData[region_number];
	local iWestX = region_data_table[1];
	local iSouthY = region_data_table[2];
	local iWidth = region_data_table[3];
	local iHeight = region_data_table[4];
	local iAreaID = region_data_table[5];

	local coastalLandCount = self.regionTerrainCounts[region_number][22];

	local region_type = self.regionTypes[region_number];
	-- Done setting up region data.
	-- Set up contingency.
	local fallback_plots = {};

	-- Check region for AlongOcean eligibility.
	if coastalLandCount < 3 then
		-- This region cannot support an Along Ocean start. Try instead to find an inland start for it.
		bSuccessFlag, bForcedPlacementFlag = self:FindStart(region_number);
		if not bSuccessFlag then
			-- This region cannot have a start and something has gone way wrong.
			-- We'll force a one tile grass island in the SW corner of the region and put the start there.
			local forcePlot = Map.GetPlot(iWestX, iSouthY);
			bForcedPlacementFlag = true;
			forcePlot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
			forcePlot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true);
			forcePlot:SetFeatureType(FeatureTypes.NO_FEATURE);
			self.startingPlots[region_number] = {iWestX, iSouthY, 0};
			self:PlaceImpactAndRipples(iWestX, iSouthY);
		end
		return bSuccessFlag, bForcedPlacementFlag;
	end

	-- Establish scope of center bias.
	local fCenterWidth = (self.centerBias / 100) * iWidth;
	local iNonCenterWidth = math.floor((iWidth - fCenterWidth) / 2);
	local iCenterWidth = iWidth - (iNonCenterWidth * 2);
	local iCenterWestX = (iWestX + iNonCenterWidth) % iW; -- Modulo math to synch coordinate to actual map in case of world wrap.
	local iCenterTestWestX = (iWestX + iNonCenterWidth); -- "Test" values ignore world wrap for easier membership testing.
	local iCenterTestEastX = (iCenterWestX + iCenterWidth - 1);

	local fCenterHeight = (self.centerBias / 100) * iHeight;
	local iNonCenterHeight = math.floor((iHeight - fCenterHeight) / 2);
	local iCenterHeight = iHeight - (iNonCenterHeight * 2);
	-- local iCenterSouthY = (iSouthY + iNonCenterHeight) % iH;
	local iCenterTestSouthY = (iSouthY + iNonCenterHeight);
	local iCenterTestNorthY = (iCenterTestSouthY + iCenterHeight - 1);

	-- Establish scope of "middle donut", outside the center but inside the outer.
	local fMiddleWidth = (self.middleBias / 100) * iWidth;
	local iOuterWidth = math.floor((iWidth - fMiddleWidth) / 2);
	local iMiddleWidth = iWidth - (iOuterWidth * 2);
	-- local iMiddleDiameterX = (iMiddleWidth - iCenterWidth) / 2;
	-- local iMiddleWestX = (iWestX + iOuterWidth) % iW;
	local iMiddleTestWestX = (iWestX + iOuterWidth);
	local iMiddleTestEastX = (iMiddleTestWestX + iMiddleWidth - 1);

	local fMiddleHeight = (self.middleBias / 100) * iHeight;
	local iOuterHeight = math.floor((iHeight - fMiddleHeight) / 2);
	local iMiddleHeight = iHeight - (iOuterHeight * 2);
	-- local iMiddleDiameterY = (iMiddleHeight - iCenterHeight) / 2;
	-- local iMiddleSouthY = (iSouthY + iOuterHeight) % iH;
	local iMiddleTestSouthY = (iSouthY + iOuterHeight);
	local iMiddleTestNorthY = (iMiddleTestSouthY + iMiddleHeight - 1);

	-- Assemble candidates lists.
	local center_coastal_plots = {};
	local center_plots_on_river = {};
	local center_fresh_plots = {};
	local center_dry_plots = {};
	local middle_coastal_plots = {};
	local middle_plots_on_river = {};
	local middle_fresh_plots = {};
	local middle_dry_plots = {};
	local outer_coastal_plots = {};

	-- Identify candidate plots.
	for region_y = 0, iHeight - 1 do -- When handling global plot indices, process Y first.
		for region_x = 0, iWidth - 1 do
			local x = (region_x + iWestX) % iW; -- Actual coords, adjusted for world wrap, if any.
			local y = (region_y + iSouthY) % iH;
			local plotIndex = y * iW + x + 1;
			if self.plotDataIsCoastal[plotIndex] then -- This plot is a land plot next to an ocean.
				local plot = Map.GetPlot(x, y);
				local plotType = plot:GetPlotType();
				if plotType ~= PlotTypes.PLOT_MOUNTAIN then -- Not a mountain plot.
					local area_of_plot = plot:GetArea();
					local landmass_of_plot = plot:GetLandmass();
					if iAreaID == -1 or (self.bArea and area_of_plot == iAreaID) or (not self.bArea and landmass_of_plot == iAreaID) then
						-- This plot is a member, so it goes on at least one candidate list.
						-- Test whether plot is in center bias, middle donut, or outer donut.
						local test_x = region_x + iWestX; -- "Test" coords, ignoring any world wrap and
						local test_y = region_y + iSouthY; -- reaching in to virtual space if necessary.
						if (test_x >= iCenterTestWestX and test_x <= iCenterTestEastX) and
						   (test_y >= iCenterTestSouthY and test_y <= iCenterTestNorthY) then
							table.insert(center_coastal_plots, plotIndex);
							if plot:IsRiverSide() then
								table.insert(center_plots_on_river, plotIndex);
							elseif plot:IsFreshWater() then
								table.insert(center_fresh_plots, plotIndex);
							else
								table.insert(center_dry_plots, plotIndex);
							end
						elseif (test_x >= iMiddleTestWestX and test_x <= iMiddleTestEastX) and
						       (test_y >= iMiddleTestSouthY and test_y <= iMiddleTestNorthY) then
							table.insert(middle_coastal_plots, plotIndex);
							if plot:IsRiverSide() then
								table.insert(middle_plots_on_river, plotIndex);
							elseif plot:IsFreshWater() then
								table.insert(middle_fresh_plots, plotIndex);
							else
								table.insert(middle_dry_plots, plotIndex);
							end
						else
							table.insert(outer_coastal_plots, plotIndex);
						end
					end
				end
			end
		end
	end
	-- Check how many plots landed on each list.
	local iNumCenterCoastal = table.maxn(center_coastal_plots);
	local iNumCenterRiver = table.maxn(center_plots_on_river);
	local iNumCenterFresh = table.maxn(center_fresh_plots);
	local iNumCenterDry = table.maxn(center_dry_plots);
	local iNumMiddleCoastal = table.maxn(middle_coastal_plots);
	local iNumMiddleRiver = table.maxn(middle_plots_on_river);
	local iNumMiddleFresh = table.maxn(middle_fresh_plots);
	local iNumMiddleDry = table.maxn(middle_dry_plots);
	local iNumOuterCoastal = table.maxn(outer_coastal_plots);

	--[[ Debug printout.
	print("-");
	print("--- Number of Candidate Plots next to an ocean in Region #", region_number, " - Region Type:", region_type, " ---");
	print("-");
	print("Coastal Plots in Center Bias area: ", iNumCenterCoastal);
	print("Which are along rivers: ", iNumCenterRiver);
	print("Which are fresh water: ", iNumCenterFresh);
	print("Which are dry: ", iNumCenterDry);
	print("-");
	print("Coastal Plots in Middle Donut area: ", iNumMiddleCoastal);
	print("Which are along rivers: ", iNumMiddleRiver);
	print("Which are fresh water: ", iNumMiddleFresh);
	print("Which are dry: ", iNumMiddleDry);
	print("-");
	print("Coastal Plots in Outer area: ", iNumOuterCoastal);
	print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
	--]]

	-- Process lists of candidate plots.
	if iNumCenterCoastal + iNumMiddleCoastal > 0 then
		local candidate_lists = {};
		if iNumCenterRiver > 0 then -- Process center bias river plots.
			table.insert(candidate_lists, center_plots_on_river);
		end
		if iNumCenterFresh > 0 then -- Process center bias fresh water plots that are not rivers.
			table.insert(candidate_lists, center_fresh_plots);
		end
		if iNumCenterDry > 0 then -- Process center bias dry plots.
			table.insert(candidate_lists, center_dry_plots);
		end
		if iNumMiddleRiver > 0 then -- Process middle bias river plots.
			table.insert(candidate_lists, middle_plots_on_river);
		end
		if iNumMiddleFresh > 0 then -- Process middle bias fresh water plots that are not rivers.
			table.insert(candidate_lists, middle_fresh_plots);
		end
		if iNumMiddleDry > 0 then -- Process middle bias dry plots.
			table.insert(candidate_lists, middle_dry_plots);
		end

		for _, plot_list in ipairs(candidate_lists) do -- Up to six plot lists, processed by priority.
			local election_returns = self:IterateThroughCandidatePlotList(plot_list, region_type);
			-- If any riverside candidates are eligible, choose one.
			local found_eligible = election_returns[1];
			if found_eligible then
				local bestPlotScore = election_returns[2];
				local bestPlotIndex = election_returns[3];
				local x = (bestPlotIndex - 1) % iW;
				local y = (bestPlotIndex - x - 1) / iW;
				self.startingPlots[region_number] = {x, y, bestPlotScore};
				self:PlaceImpactAndRipples(x, y);
				return true, false;
			end
			-- If none eligible, check for fallback plot.
			local found_fallback = election_returns[4];
			if found_fallback then
				local bestFallbackScore = election_returns[5];
				local bestFallbackIndex = election_returns[6];
				local x = (bestFallbackIndex - 1) % iW;
				local y = (bestFallbackIndex - x - 1) / iW;
				table.insert(fallback_plots, {x, y, bestFallbackScore});
			end
		end
	end
	-- Reaching this point means no strong coastal sites in center bias or middle donut subregions!

	-- Process candidates from Outer subregion, if any.
	if iNumOuterCoastal > 0 then
		local outer_eligible_list = {};
		local found_eligible = false;
		local found_fallback = false;
		local bestFallbackScore = -50;
		local bestFallbackIndex;
		-- Process list of candidate plots.
		for _, plotIndex in ipairs(outer_coastal_plots) do
			local score, meets_minimums = self:EvaluateCandidatePlot(plotIndex, region_type);
			-- Test current plot against best known plot.
			if meets_minimums then
				found_eligible = true;
				table.insert(outer_eligible_list, plotIndex);
			else
				found_fallback = true;
				if score > bestFallbackScore then
					bestFallbackScore = score;
					bestFallbackIndex = plotIndex;
				end
			end
		end
		if found_eligible then -- Iterate through eligible plots and choose the one closest to the center of the region.
			local closestPlot;
			local closestDistance = math.max(iW, iH);
			local bullseyeX = iWestX + (iWidth / 2);
			if bullseyeX < iWestX then -- wrapped around: un-wrap it for test purposes.
				bullseyeX = bullseyeX + iW;
			end
			local bullseyeY = iSouthY + (iHeight / 2);
			if bullseyeY < iSouthY then -- wrapped around: un-wrap it for test purposes.
				bullseyeY = bullseyeY + iH;
			end
			if bullseyeY / 2 ~= math.floor(bullseyeY / 2) then -- Y coord is odd, add .5 to X coord for hex-shift.
				bullseyeX = bullseyeX + 0.5;
			end

			for _, plotIndex in ipairs(outer_eligible_list) do
				local x = (plotIndex - 1) % iW;
				local y = (plotIndex - x - 1) / iW;
				local adjusted_x = x;
				local adjusted_y = y;
				if y / 2 ~= math.floor(y / 2) then -- Y coord is odd, add .5 to X coord for hex-shift.
					adjusted_x = x + 0.5;
				end

				if x < iWestX then -- wrapped around: un-wrap it for test purposes.
					adjusted_x = adjusted_x + iW;
				end
				if y < iSouthY then -- wrapped around: un-wrap it for test purposes.
					adjusted_y = y + iH;
				end
				local fDistance = math.sqrt((adjusted_x - bullseyeX) ^ 2 + (adjusted_y - bullseyeY) ^ 2);
				if fDistance < closestDistance then -- Found new "closer" plot.
					closestPlot = plotIndex;
					closestDistance = fDistance;
				end
			end
			-- Assign the closest eligible plot as the start point.
			local x = (closestPlot - 1) % iW;
			local y = (closestPlot - x - 1) / iW;
			-- Re-get plot score for inclusion in start plot data.
			local score, _ = self:EvaluateCandidatePlot(closestPlot, region_type);
			-- Assign this plot as the start for this region.
			self.startingPlots[region_number] = {x, y, score};
			self:PlaceImpactAndRipples(x, y);
			return true, false;
		end
		-- Add the fallback plot (best scored plot) from the Outer region to the fallback list.
		if found_fallback then
			local x = (bestFallbackIndex - 1) % iW;
			local y = (bestFallbackIndex - x - 1) / iW;
			table.insert(fallback_plots, {x, y, bestFallbackScore});
		end
	end
	-- Reaching here means no plot in the entire region met the minimum standards for selection.

	-- The fallback plot contains the best-scored plots from each test area in this region.
	-- This region must be something awful on food, or had too few coastal plots with none being decent.
	-- We will compare all the fallback plots and choose the best to be the start plot.
	local iNumFallbacks = table.maxn(fallback_plots);
	if iNumFallbacks > 0 then
		local best_fallback_score = 0
		local best_fallback_x;
		local best_fallback_y;
		for _, plotData in ipairs(fallback_plots) do
			local score = plotData[3];
			if score > best_fallback_score then
				best_fallback_score = score;
				best_fallback_x = plotData[1];
				best_fallback_y = plotData[2];
			end
		end
		-- Assign the start for this region.
		self.startingPlots[region_number] = {best_fallback_x, best_fallback_y, best_fallback_score};
		self:PlaceImpactAndRipples(best_fallback_x, best_fallback_y);
		bSuccessFlag = true;
	else
		-- This region cannot support an Along Ocean start. Try instead to find an Inland start for it.
		bSuccessFlag, bForcedPlacementFlag = self:FindStart(region_number);
		if not bSuccessFlag then
			-- This region cannot have a start and something has gone way wrong.
			-- We'll force a one tile grass island in the SW corner of the region and put the start there.
			local forcePlot = Map.GetPlot(iWestX, iSouthY);
			bSuccessFlag = false;
			bForcedPlacementFlag = true;
			forcePlot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
			forcePlot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true);
			forcePlot:SetFeatureType(FeatureTypes.NO_FEATURE);
			self.startingPlots[region_number] = {iWestX, iSouthY, 0};
			self:PlaceImpactAndRipples(iWestX, iSouthY);
		end
	end

	return bSuccessFlag, bForcedPlacementFlag;
end
------------------------------------------------------------------------------
function AssignStartingPlots:FindStartWithoutRegardToAreaID(region_number, bMustBeCoast)
	-- This function attempts to choose a start position on the best AreaID section within the Region's rectangle.
	-- This function returns two boolean flags, indicating the success level of the operation.
	local bSuccessFlag = false; -- Returns true when a start is placed, false when process fails.
	local bForcedPlacementFlag = false; -- Returns true if this region had no eligible starts and one was forced to occur.

	-- Obtain data needed to process this region.
	local iW, _ = Map.GetGridSize();
	local region_data_table = self.regionData[region_number];
	local iWestX = region_data_table[1];
	local iSouthY = region_data_table[2];
	local iWidth = region_data_table[3];
	local iHeight = region_data_table[4];

	local region_type = self.regionTypes[region_number];
	local fallback_plots = {};
	-- Done setting up region data.

	-- Obtain info on all landmasses wholly or partially within this region, for comparision purposes.
	local iRegionalFertilityOfLands = 0;
	local iNumLandAreas = 0;
	local land_area_IDs = {};
	local land_area_plots = {};
	local land_area_fert = {};
	local land_area_plot_lists = {};
	-- Cycle through all plots in the region, checking their Start Placement Fertility and AreaID.
	for region_y = 0, iHeight - 1 do
		for region_x = 0, iWidth - 1 do
			local x = region_x + iWestX;
			local y = region_y + iSouthY;
			local plot = Map.GetPlot(x, y);
			local plotType = plot:GetPlotType();
			if plotType == PlotTypes.PLOT_HILLS or plotType == PlotTypes.PLOT_LAND then -- Land plot, process it.
				local iArea = plot:GetArea();
				local plotFertility = self:MeasureStartPlacementFertilityOfPlot(x, y, false); -- Check for coastal land is disabled.
				iRegionalFertilityOfLands = iRegionalFertilityOfLands + plotFertility;
				if not TestMembership(land_area_IDs, iArea) then -- This plot is the first detected in its AreaID.
					iNumLandAreas = iNumLandAreas + 1;
					table.insert(land_area_IDs, iArea);
					land_area_plots[iArea] = 1;
					land_area_fert[iArea] = plotFertility;
				else -- This AreaID already known.
					land_area_plots[iArea] = land_area_plots[iArea] + 1;
					land_area_fert[iArea] = land_area_fert[iArea] + plotFertility;
				end
			end
		end
	end

	-- Generate empty (non-nil) tables for each Area ID in the plot lists matrix.
	for _, areaID in ipairs(land_area_IDs) do
		land_area_plot_lists[areaID] = {};
	end
	-- Cycle through all plots in the region again, adding candidates to the applicable AreaID plot list.
	for region_y = 0, iHeight - 1 do
		for region_x = 0, iWidth - 1 do
			local x = region_x + iWestX;
			local y = region_y + iSouthY;
			local i = y * iW + x + 1;
			local plot = Map.GetPlot(x, y);
			local plotType = plot:GetPlotType();
			if plotType == PlotTypes.PLOT_HILLS or plotType == PlotTypes.PLOT_LAND then -- Land plot, process it.
				local iArea = plot:GetArea();
				if self.plotDataIsCoastal[i] then
					table.insert(land_area_plot_lists[iArea], i);
				elseif not bMustBeCoast and not self.plotDataIsNextToCoast[i] then
					table.insert(land_area_plot_lists[iArea], i);
				end
			end
		end
	end

	local best_areas = {};
	local regionAreaListUnsorted = {};
	local regionAreaListSorted = {}; -- Have to make this a separate table, not merely a pointer to the first table.
	for areaNum, fert in pairs(land_area_fert) do
		table.insert(regionAreaListUnsorted, {areaNum, fert});
		table.insert(regionAreaListSorted, fert);
	end
	table.sort(regionAreaListSorted);

	-- Match each sorted fertilty value to the matching unsorted AreaID number and record in sequence.
	local iNumAreas = table.maxn(regionAreaListSorted);
	for area_order = iNumAreas, 1, -1 do -- Best areas are at the end of the list, so run the list backward.
		for loop, data_pair in ipairs(regionAreaListUnsorted) do
			local unsorted_fert = data_pair[2];
			if regionAreaListSorted[area_order] == unsorted_fert then
				local unsorted_area_num = data_pair[1];
				table.insert(best_areas, unsorted_area_num);
				-- HAVE TO remove the entry from the table in case of ties on fert value.
				table.remove(regionAreaListUnsorted, loop);
				break;
			end
		end
	end

	--[[ Debug printout.
	print("-");
	print("--- Number of Candidate Plots in each landmass in Region #", region_number, " - Region Type:", region_type, " ---");
	print("-");
	for loop, iAreaID in ipairs(best_areas) do
		local fert_rating = land_area_fert[iAreaID];
		local plotCount = table.maxn(land_area_plot_lists[iAreaID]);
		print("* Area ID#", iAreaID, "has fertility rating of", fert_rating, "and candidate plot count of", plotCount); print("-");
	end
	print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
	--]]

	-- Now iterate through areas, from best fertility downward, looking for a site good enough to choose.
	for _, iAreaID in ipairs(best_areas) do
		local plot_list = land_area_plot_lists[iAreaID];
		local election_returns = self:IterateThroughCandidatePlotList(plot_list, region_type);
		-- If any plots in this area are eligible, choose one.
		local found_eligible = election_returns[1];
		if found_eligible then
			local bestPlotScore = election_returns[2];
			local bestPlotIndex = election_returns[3];
			local x = (bestPlotIndex - 1) % iW;
			local y = (bestPlotIndex - x - 1) / iW;
			self.startingPlots[region_number] = {x, y, bestPlotScore};
			self:PlaceImpactAndRipples(x, y);
			return true, false;
		end
		-- If none eligible, check for fallback plot.
		local found_fallback = election_returns[4];
		if found_fallback then
			local bestFallbackScore = election_returns[5];
			local bestFallbackIndex = election_returns[6];
			local x = (bestFallbackIndex - 1) % iW;
			local y = (bestFallbackIndex - x - 1) / iW;
			table.insert(fallback_plots, {x, y, bestFallbackScore});
		end
	end
	-- Reaching this point means no strong sites far enough away from any already-placed start points.

	-- We will compare all the fallback plots and choose the best to be the start plot.
	local iNumFallbacks = table.maxn(fallback_plots);
	if iNumFallbacks > 0 then
		local best_fallback_score = 0
		local best_fallback_x;
		local best_fallback_y;
		for _, plotData in ipairs(fallback_plots) do
			local score = plotData[3];
			if score > best_fallback_score then
				best_fallback_score = score;
				best_fallback_x = plotData[1];
				best_fallback_y = plotData[2];
			end
		end
		-- Assign the start for this region.
		self.startingPlots[region_number] = {best_fallback_x, best_fallback_y, best_fallback_score};
		self:PlaceImpactAndRipples(best_fallback_x, best_fallback_y);
		bSuccessFlag = true;
	else
		-- Somehow, this region has had no eligible plots of any kind.
		-- We'll force a one tile grass island in the SW corner of the region and put the start there.
		local forcePlot = Map.GetPlot(iWestX, iSouthY);
		bSuccessFlag = false;
		bForcedPlacementFlag = true;
		forcePlot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
		forcePlot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true);
		forcePlot:SetFeatureType(FeatureTypes.NO_FEATURE);
		self.startingPlots[region_number] = {iWestX, iSouthY, 0};
		self:PlaceImpactAndRipples(iWestX, iSouthY);
	end

	return bSuccessFlag, bForcedPlacementFlag;
end
------------------------------------------------------------------------------
function CivNeedsMountainStart(civType)
	for row in GameInfo.Civilization_Start_Prefer_Mountain{CivilizationType = civType} do
		if row.StartPreferMountain then
			return true;
		end
	end
	return false;
end
------------------------------------------------------------------------------
function CivNeedsSnowStart(civType)
	for row in GameInfo.Civilization_Start_Prefer_Snow{CivilizationType = civType} do
		if row.StartPreferSnow then
			return true;
		end
	end
	return false;
end
------------------------------------------------------------------------------
function AssignStartingPlots:ChooseLocations(args)
	print("Map Generation - Choosing Start Locations for Civilizations");
	args = args or {};
	local mustBeCoast = args.mustBeCoast or false; -- if true, will force all starts on salt water coast if possible

	-- Defaults for evaluating potential start plots are assigned in .Create but args passed in here can override.
	-- If args value for a field is nil (no arg) then these assignments will keep the default values in place.
	self.centerBias = args.centerBias or self.centerBias; -- % of radius from region center to examine first
	self.middleBias = args.middleBias or self.middleBias; -- % of radius from region center to check second
	self.minFoodInner = args.minFoodInner or self.minFoodInner;
	self.minProdInner = args.minProdInner or self.minProdInner;
	self.minGoodInner = args.minGoodInner or self.minGoodInner;
	self.minFoodMiddle = args.minFoodMiddle or self.minFoodMiddle;
	self.minProdMiddle = args.minProdMiddle or self.minProdMiddle;
	self.minGoodMiddle = args.minGoodMiddle or self.minGoodMiddle;
	self.minFoodOuter = args.minFoodOuter or self.minFoodOuter;
	self.minProdOuter = args.minProdOuter or self.minProdOuter;
	self.minGoodOuter = args.minGoodOuter or self.minGoodOuter;
	self.maxJunk = args.maxJunk or self.maxJunk;

	-- Determine whether any civ should start near mountain or snow
	for loop = 1, self.iNumCivs do
		local playerNum = self.player_ID_list[loop]; -- MP games can have gaps between player numbers, so we cannot assume a sequential set of IDs.
		local player = Players[playerNum];
		local civType = GameInfo.Civilizations[player:GetCivilizationType()].Type;
		if CivNeedsMountainStart(civType) then
			self.hasMountainBias = true;
		end
		if CivNeedsSnowStart(civType) then
			self.hasSnowBias = true;
		end
	end

	-- Measure terrain/plot/feature in regions.
	self:MeasureTerrainInRegions();

	-- Determine region type.
	self:DetermineRegionTypes();

	-- Set up list of regions (to be processed in this order).
	-- First, make a list of all average fertility values...
	local regionAssignList = {};
	local averageFertilityListUnsorted = {};
	local averageFertilityListSorted = {}; -- Have to make this a separate table, not merely a pointer to the first table.
	for i, region_data in ipairs(self.regionData) do
		local thisRegionAvgFert = region_data[8];
		table.insert(averageFertilityListUnsorted, {i, thisRegionAvgFert});
		table.insert(averageFertilityListSorted, thisRegionAvgFert);
	end
	-- Now sort the copy low to high.
	table.sort(averageFertilityListSorted);
	-- Finally, match each sorted fertilty value to the matching unsorted region number and record in sequence.
	local iNumRegions = table.maxn(averageFertilityListSorted);
	for region_order = 1, iNumRegions do
		for loop, data_pair in ipairs(averageFertilityListUnsorted) do
			local unsorted_fert = data_pair[2];
			if averageFertilityListSorted[region_order] == unsorted_fert then
				local unsorted_reg_num = data_pair[1];
				table.insert(regionAssignList, unsorted_reg_num);
				-- HAVE TO remove the entry from the table in rare case of ties on fert value.
				-- Or it will just match this value for a second time, then crash when the region it was tied with ends up with nil data.
				table.remove(averageFertilityListUnsorted, loop);
				break;
			end
		end
	end

	-- main loop
	for assignIndex = 1, iNumRegions do
		local currentRegionNumber = regionAssignList[assignIndex];
		local bSuccessFlag = false;
		local bForcedPlacementFlag = false;

		if self.method == RegionDivision.RECTANGULAR or self.method == RegionDivision.RECTANGULAR_SELF_DEFINED then
			bSuccessFlag, bForcedPlacementFlag = self:FindStartWithoutRegardToAreaID(currentRegionNumber, mustBeCoast);
		elseif mustBeCoast then
			bSuccessFlag, bForcedPlacementFlag = self:FindCoastalStart(currentRegionNumber);
		else
			bSuccessFlag, bForcedPlacementFlag = self:FindStart(currentRegionNumber);
		end

		--[[ Printout for debug only.
		print("- - -");
		print("Start Plot for Region #", currentRegionNumber, " was successful: ", bSuccessFlag);
		print("Start Plot for Region #", currentRegionNumber, " was forced: ", bForcedPlacementFlag);
		--]]
	end

	--[[ Printout of start plots. Debug use only.
	print("-");
	print("--- Table of results, New Start Finder ---");
	for loop, startData in ipairs(self.startingPlots) do
		print("-");
		print("Region#", loop, " has start plot at: ", startData[1], startData[2], "with Fertility Rating of ", startData[3]);
	end
	print("-");
	print("--- Table of results, New Start Finder ---");
	print("-");
	--]]

	--[[ Printout of Impact and Ripple data.
	print("--- Impact and Ripple ---");
	PrintContentsOfTable(self.distanceData);
	print("-");
	--]]
end
------------------------------------------------------------------------------
-- Start of functions tied to BalanceAndAssign()
------------------------------------------------------------------------------
function AssignStartingPlots:AttemptToPlaceBonusResourceAtPlot(x, y, bAllowOasis)
	-- Returns two booleans. First is true if something was placed. Second true if Oasis placed.
	-- print("-"); print("Attempting to place a Bonus at: ", x, y);
	local plot = Map.GetPlot(x, y);
	if not plot then
		-- print("Placement failed, plot was nil.");
		return false;
	end
	if plot:GetResourceType() ~= -1 then
		-- print("Plot already had a resource.");
		return false;
	end
	local terrainType = plot:GetTerrainType();
	if terrainType == TerrainTypes.TERRAIN_SNOW then
		-- print("Plot was arctic land buried beneath endless snow.");
		return false;
	end
	local featureType = plot:GetFeatureType();
	if featureType == FeatureTypes.FEATURE_OASIS then
		-- print("Plot already had an Oasis.");
		return false;
	end
	local plotType = plot:GetPlotType();
	local resourceType;
	-- Randomize resource selected instead -- September 2020, azum4roll
	if featureType == FeatureTypes.FEATURE_JUNGLE then -- Place Banana
		if plotType == PlotTypes.PLOT_LAND then
			plot:SetResourceType(self.banana_ID, 1);
			-- print("Placed Banana.");
			self.amounts_of_resources_placed[self.banana_ID + 1] = self.amounts_of_resources_placed[self.banana_ID + 1] + 1;
			return true, false;
		else -- nothing fits here
			return false;
		end
	elseif featureType == FeatureTypes.FEATURE_FOREST then -- Place Deer
		plot:SetResourceType(self.deer_ID, 1);
		-- print("Placed Deer.");
		self.amounts_of_resources_placed[self.deer_ID + 1] = self.amounts_of_resources_placed[self.deer_ID + 1] + 1;
		return true, false;
	elseif plotType == PlotTypes.PLOT_HILLS and featureType == FeatureTypes.NO_FEATURE then
		plot:SetResourceType(self.sheep_ID, 1);
		-- print("Placed Sheep.");
		self.amounts_of_resources_placed[self.sheep_ID + 1] = self.amounts_of_resources_placed[self.sheep_ID + 1] + 1;
		return true, false;
	elseif plotType == PlotTypes.PLOT_LAND then
		if featureType == FeatureTypes.NO_FEATURE then
			if terrainType == TerrainTypes.TERRAIN_GRASS then -- Place Cows, Rice or Bison
				local diceroll = Map.Rand(3, "Selection of Bonus Resource type - Start Normalization LUA");
				if diceroll < 2 then
					resourceType = self.cow_ID;
					-- print("Placed Cows.");
				else
					if self:IsTropical(y) then
						resourceType = self.rice_ID;
						-- print("Placed Rice.");
					else
						resourceType = self.bison_ID;
						-- print("Placed Bison.");
					end
				end
				plot:SetResourceType(resourceType, 1);
				self.amounts_of_resources_placed[resourceType + 1] = self.amounts_of_resources_placed[resourceType + 1] + 1;
				return true, false;
			elseif terrainType == TerrainTypes.TERRAIN_PLAINS then -- Place Wheat, Maize or Bison
				local diceroll = Map.Rand(3, "Selection of Bonus Resource type - Start Normalization LUA");
				if diceroll < 2 then
					if self:IsTropical(y) then
						resourceType = self.maize_ID;
						-- print("Placed Maize.");
					else
						resourceType = self.wheat_ID;
						-- print("Placed Wheat.");
					end
				else
					resourceType = self.bison_ID;
					-- print("Placed Bison.");
				end
				plot:SetResourceType(resourceType, 1);
				self.amounts_of_resources_placed[resourceType + 1] = self.amounts_of_resources_placed[resourceType + 1] + 1;
				return true, false;
			elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then -- Place Deer
				plot:SetResourceType(self.deer_ID, 1);
				-- print("Placed Deer.");
				self.amounts_of_resources_placed[self.deer_ID + 1] = self.amounts_of_resources_placed[self.deer_ID + 1] + 1;
				return true, false;
			elseif terrainType == TerrainTypes.TERRAIN_DESERT then
				if plot:IsFreshWater() then -- Place Wheat or Maize
					if self:IsTropical(y) then
						resourceType = self.maize_ID;
						-- print("Placed Maize.");
						self.amounts_of_resources_placed[self.maize_ID + 1] = self.amounts_of_resources_placed[self.maize_ID + 1] + 1;
					else
						resourceType = self.wheat_ID;
						-- print("Placed Wheat.");
						self.amounts_of_resources_placed[self.wheat_ID + 1] = self.amounts_of_resources_placed[self.wheat_ID + 1] + 1;
					end
					return true, false;
				elseif bAllowOasis then -- Place Oasis
					plot:SetFeatureType(FeatureTypes.FEATURE_OASIS);
					-- print("Placed Oasis.");
					return true, true;
				-- else
					-- print("Not allowed to place any more Oasis help at this site.");
				end
			end
		end
	elseif plotType == PlotTypes.PLOT_OCEAN then
		if terrainType == TerrainTypes.TERRAIN_COAST and featureType == FeatureTypes.NO_FEATURE then
			if not plot:IsLake() then -- Place Fish
				plot:SetResourceType(self.fish_ID, 1);
				-- print("Placed Fish.");
				self.amounts_of_resources_placed[self.fish_ID + 1] = self.amounts_of_resources_placed[self.fish_ID + 1] + 1;
				return true, false;
			end
		end
	end
	-- Nothing placed.
	return false, false;
end
------------------------------------------------------------------------------
function AssignStartingPlots:AttemptToPlaceHillsAtPlot(x, y)
	-- This function will add hills at a specified plot, if able.
	-- print("-"); print("Attempting to add Hills at: ", x, y);
	local plot = Map.GetPlot(x, y);
	if not plot then
		-- print("Placement failed, plot was nil.");
		return false;
	end
	if plot:GetResourceType() ~= -1 then
		-- print("Placement failed, plot had a resource.");
		return false;
	end
	local plotType = plot:GetPlotType();
	local featureType = plot:GetFeatureType();
	if plotType == PlotTypes.PLOT_OCEAN then
		-- print("Placement failed, plot was water.");
		return false;
	elseif plot:IsRiverSide() then
		-- print("Placement failed, plot was next to river.");
		return false;
	elseif featureType == FeatureTypes.FEATURE_FOREST then
		-- print("Placement failed, plot had a forest already.");
		return false;
	end
	-- Change the plot type from flatlands to hills and clear any features.
	plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
	plot:SetFeatureType(FeatureTypes.NO_FEATURE);
	return true;
end
------------------------------------------------------------------------------
function AssignStartingPlots:AttemptToPlaceSmallStrategicAtPlot(x, y)
	-- This function will add a small horse or iron source to a specified plot, if able.
	-- print("-"); print("Attempting to add Small Strategic resource at: ", x, y);
	local plot = Map.GetPlot(x, y);
	if not plot then
		-- print("Placement failed, plot was nil.");
		return false;
	end
	if plot:GetResourceType() ~= -1 then
		-- print("Plot already had a resource.");
		return false;
	end
	local plotType = plot:GetPlotType();
	local terrainType = plot:GetTerrainType();
	local featureType = plot:GetFeatureType();
	if plotType ~= PlotTypes.PLOT_LAND then
		-- print("Placement failed, plot was not flat land.");
		return false;
	elseif featureType == FeatureTypes.NO_FEATURE then
		if terrainType == TerrainTypes.TERRAIN_GRASS or terrainType == TerrainTypes.TERRAIN_PLAINS then -- Could be horses.
			local choice = self.horse_ID;
			local diceroll = Map.Rand(4, "Selection of Strategic Resource type - Start Normalization LUA");
			if diceroll == 2 then
				choice = self.iron_ID;
				-- print("Placed Iron.");
			else
				-- print("Placed Horse.");
			end
			plot:SetResourceType(choice, 2);
			self.amounts_of_resources_placed[choice + 1] = self.amounts_of_resources_placed[choice + 1] + 2;
		else -- Can't be horses.
			plot:SetResourceType(self.iron_ID, 2);
			self.amounts_of_resources_placed[self.iron_ID + 1] = self.amounts_of_resources_placed[self.iron_ID + 1] + 2;
			-- print("Placed Iron.");
		end
		return true;
	end
	-- print("Placement failed, feature in the way.");
	return false;
end
------------------------------------------------------------------------------
function AssignStartingPlots:AddStrategicBalanceResources(region_number)
	-- This function adds the required Strategic Resources to start plots, for
	-- games that have selected to enable Strategic Resource Balance.
	local iW, iH = Map.GetGridSize();
	local start_point_data = self.startingPlots[region_number];
	local x = start_point_data[1];
	local y = start_point_data[2];
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local odd = self.firstRingYIsOdd;
	local even = self.firstRingYIsEven;
	local nextX, nextY, plot_adjustments;
	local iron_list, horse_list = {}, {};
	local iron_fallback, horse_fallback = {}, {};

	-- print("- Adding Strategic Balance Resources for start location in Region#", region_number);

	for ripple_radius = 4, 6 do
		local currentX = x - ripple_radius;
		local currentY = y;
		for direction_index = 1, 6 do
			for _ = 1, ripple_radius do
				if currentY / 2 > math.floor(currentY / 2) then
					plot_adjustments = odd[direction_index];
				else
					plot_adjustments = even[direction_index];
				end
				nextX = currentX + plot_adjustments[1];
				nextY = currentY + plot_adjustments[2];
				if not wrapX and (nextX < 0 or nextX >= iW) then
					-- X is out of bounds.
				elseif not wrapY and (nextY < 0 or nextY >= iH) then
					-- Y is out of bounds.
				else
					local realX = nextX;
					local realY = nextY;
					if wrapX then
						realX = realX % iW;
					end
					if wrapY then
						realY = realY % iH;
					end
					-- We've arrived at the correct x and y for the current plot.
					local plot = Map.GetPlot(realX, realY);
					local plotType = plot:GetPlotType();
					local terrainType = plot:GetTerrainType();
					local featureType = plot:GetFeatureType();
					local plotIndex = realY * iW + realX + 1;
					-- Check this plot for resource placement eligibility.
					if plotType == PlotTypes.PLOT_HILLS and featureType ~= FeatureTypes.FEATURE_JUNGLE then
						if ripple_radius < 5 then
							table.insert(iron_list, plotIndex);
						else
							table.insert(iron_fallback, plotIndex);
						end
					elseif plotType == PlotTypes.PLOT_LAND then
						if featureType == FeatureTypes.NO_FEATURE then
							if terrainType == TerrainTypes.TERRAIN_TUNDRA or terrainType == TerrainTypes.TERRAIN_DESERT then
								if plot:IsFreshWater() then
									table.insert(horse_fallback, plotIndex);
								else
									table.insert(iron_fallback, plotIndex);
								end
							elseif terrainType == TerrainTypes.TERRAIN_PLAINS or terrainType == TerrainTypes.TERRAIN_GRASS then
								if ripple_radius < 5 then
									table.insert(horse_list, plotIndex);
								else
									table.insert(horse_fallback, plotIndex);
								end
								table.insert(iron_fallback, plotIndex);
							elseif terrainType == TerrainTypes.TERRAIN_SNOW then
								if ripple_radius < 5 then
									table.insert(iron_list, plotIndex);
								else
									table.insert(iron_fallback, plotIndex);
								end
							end
						elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
							table.insert(horse_list, plotIndex);
							table.insert(iron_fallback, plotIndex);
						elseif featureType == FeatureTypes.FEATURE_FOREST then
							if ripple_radius < 5 then
								table.insert(iron_list, plotIndex);
							else
								table.insert(iron_fallback, plotIndex);
							end
						end
					end
					currentX, currentY = nextX, nextY;
				end
			end
		end
	end

	local _, horse_amt, _, iron_amt, _, _ = self:GetSmallStrategicResourceQuantityValues();
	local shuf_list;
	local placed_iron, placed_horse = false, false;

	if table.maxn(iron_list) > 0 then
		shuf_list = GetShuffledCopyOfTable(iron_list);
		local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.iron_ID, iron_amt, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
		if iNumLeftToPlace == 0 then
			placed_iron = true;
		end
	end
	if table.maxn(horse_list) > 0 then
		shuf_list = GetShuffledCopyOfTable(horse_list);
		local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.horse_ID, horse_amt, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
		if iNumLeftToPlace == 0 then
			placed_horse = true;
		end
	end
	if not placed_iron and table.maxn(iron_fallback) > 0 then
		shuf_list = GetShuffledCopyOfTable(iron_fallback);
		self:PlaceSpecificNumberOfResources(self.iron_ID, iron_amt, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
	end
	if not placed_horse and table.maxn(horse_fallback) > 0 then
		shuf_list = GetShuffledCopyOfTable(horse_fallback);
		self:PlaceSpecificNumberOfResources(self.horse_ID, horse_amt, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:AttemptToPlaceCattleAtGrassPlot(x, y)
	-- Function modified May 2011 to boost production at heavy grass starts. - BT
	-- Now placing Stone instead of Cows. Returns true if Stone is placed.
	-- Now placing Forest instead of Stone. -- September 2020, azum4roll
	-- Now placing Cattle instead of Forest. We've gone full circle! -- June 2023, azum4roll
	-- print("-"); print("Attempting to place Forest at: ", x, y);
	local plot = Map.GetPlot(x, y);
	if not plot then
		-- print("Placement failed, plot was nil.");
		return false;
	end
	if plot:GetResourceType() ~= -1 then
		-- print("Plot already had a resource.");
		return false;
	end
	local plotType = plot:GetPlotType();
	if plotType == PlotTypes.PLOT_LAND then
		local featureType = plot:GetFeatureType();
		if featureType == FeatureTypes.NO_FEATURE then
			local terrainType = plot:GetTerrainType();
			if terrainType == TerrainTypes.TERRAIN_GRASS then -- Place Cattle
				plot:SetResourceType(self.cow_ID, 1);
				-- print("Placed Cattle.");
				return true;
			end
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:AttemptToPlaceStoneAtGrassPlot(x, y)
	-- Function modified May 2011 to boost production at heavy grass starts. - BT
	-- Now placing Stone instead of Cows. Returns true if Stone is placed.
	-- print("-"); print("Attempting to place Stone at: ", x, y);
	local plot = Map.GetPlot(x, y);
	if not plot then
		-- print("Placement failed, plot was nil.");
		return false;
	end
	if plot:GetResourceType() ~= -1 then
		-- print("Plot already had a resource.");
		return false;
	end
	local plotType = plot:GetPlotType();
	if plotType == PlotTypes.PLOT_LAND then
		local featureType = plot:GetFeatureType();
		if featureType == FeatureTypes.NO_FEATURE then
			local terrainType = plot:GetTerrainType();
			if terrainType == TerrainTypes.TERRAIN_GRASS then -- Place Stone
				plot:SetResourceType(self.stone_ID, 1);
				-- print("Placed Stone.");
				self.amounts_of_resources_placed[self.stone_ID + 1] = self.amounts_of_resources_placed[self.stone_ID + 1] + 1;
				return true;
			end
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:NormalizeStartLocation(region_number)
	--[[ This function measures the value of land in two rings around a given start
	     location, primarily for the purpose of determining how much support the site
	     requires in the form of Bonus Resources. Numerous assumptions are built in
	     to this operation that would need to be adjusted for any modifications to
	     terrain or resources types and yields, or to game rules about rivers and
	     other map elements. Nothing is hardcoded in a way that puts it out of the
	     reach of modders, but any mods including changes to map elements may have a
	     significant workload involved with rebalancing the start finder and the
	     resource distribution to fit them properly to a mod's custom needs. I have
	     labored to document every function and method in detail to make it as easy
	     as possible to modify this system.  -- Bob Thomas - April 15, 2010  --]]

	local iW, iH = Map.GetGridSize();
	local start_point_data = self.startingPlots[region_number];
	local x = start_point_data[1];
	local y = start_point_data[2];
	local plot = Map.GetPlot(x, y);
	local plotIndex = y * iW + x + 1;
	local isEvenY = true;
	if y / 2 > math.floor(y / 2) then
		isEvenY = false;
	end
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local innerFourFood, innerThreeFood, innerTwoFood, innerHills, innerForest, innerOneHammer, innerOcean = 0, 0, 0, 0, 0, 0, 0;
	local outerFourFood, outerThreeFood, outerTwoFood, outerHills, outerForest, outerOneHammer, outerOcean = 0, 0, 0, 0, 0, 0, 0;
	local innerCanHaveBonus, outerCanHaveBonus, innerBadTiles, outerBadTiles = 0, 0, 0, 0;
	local iNumFoodBonusNeeded = 0;
	local iNumNativeTwoFoodFirstRing, iNumNativeTwoFoodSecondRing = 0, 0; -- Cities must begin the game with at least three native 2F tiles, one in first ring.
	local search_table = {};

	-- Remove any feature Ice from the first ring.
	self:GenerateLuxuryPlotListsAtCitySite(x, y, 1, true);

	-- Set up Conditions checks.
	local alongOcean = false;
	local nextToLake = false;
	local isRiver = false;
	local nearRiver = false;
	local nearMountain = false;
	local forestCount, jungleCount = 0, 0;
	local desertCount, tundraCount = 0, 0;

	-- Check start plot to see if it's adjacent to saltwater.
	if self.plotDataIsCoastal[plotIndex] then
		alongOcean = true;
	end

	-- Check start plot to see if it's on a river.
	if plot:IsRiver() then
		isRiver = true;
	end

	-- Data Chart for early game tile potentials
	-- 4F: Flood Plains, Grass on fresh water (includes forest and marsh).
	-- 3F: Dry Grass, Plains on fresh water (includes forest and jungle), Tundra on fresh water (includes forest), Oasis
	-- 2F: Dry Plains, Lake, all remaining Jungles.
	-- 1H: Plains, Jungle on Plains

	-- Adding evaluation of grassland and plains for balance boost of bonus Cows for heavy grass starts. -1/26/2011 BT
	local iNumGrass, iNumPlains = 0, 0;

	-- Evaluate First Ring
	if isEvenY then
		search_table = self.firstRingYIsEven;
	else
		search_table = self.firstRingYIsOdd;
	end

	for _, plot_adjustments in ipairs(search_table) do
		local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments);

		if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then
			-- This plot does not exist. It's off the map edge.
			innerBadTiles = innerBadTiles + 1;
		else
			local searchPlot = Map.GetPlot(searchX, searchY);
			local plotType = searchPlot:GetPlotType();
			local terrainType = searchPlot:GetTerrainType();
			local featureType = searchPlot:GetFeatureType();

			if plotType == PlotTypes.PLOT_MOUNTAIN then
				innerBadTiles = innerBadTiles + 1;
			elseif plotType == PlotTypes.PLOT_OCEAN then
				if searchPlot:IsLake() then
					nextToLake = true;
					if featureType == FeatureTypes.FEATURE_ICE then
						innerBadTiles = innerBadTiles + 1;
					else
						innerTwoFood = innerTwoFood + 1;
						iNumNativeTwoFoodFirstRing = iNumNativeTwoFoodFirstRing + 1;
					end
				else
					if featureType == FeatureTypes.FEATURE_ICE then
						innerBadTiles = innerBadTiles + 1;
					else
						innerOcean = innerOcean + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
					end
				end
			else -- Habitable plot.
				if featureType == FeatureTypes.FEATURE_JUNGLE then
					jungleCount = jungleCount + 1;
					iNumNativeTwoFoodFirstRing = iNumNativeTwoFoodFirstRing + 1;
				elseif featureType == FeatureTypes.FEATURE_FOREST then
					forestCount = forestCount + 1;
				end
				if searchPlot:IsRiver() then
					nearRiver = true;
				end
				if plotType == PlotTypes.PLOT_HILLS then
					innerHills = innerHills + 1;
					if featureType == FeatureTypes.FEATURE_JUNGLE then
						innerTwoFood = innerTwoFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
					elseif featureType == FeatureTypes.FEATURE_FOREST then
						innerCanHaveBonus = innerCanHaveBonus + 1;
					end
				elseif featureType == FeatureTypes.FEATURE_OASIS then
					innerThreeFood = innerThreeFood + 1;
					iNumNativeTwoFoodFirstRing = iNumNativeTwoFoodFirstRing + 1;
				elseif searchPlot:IsFreshWater() then
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						innerFourFood = innerFourFood + 1;
						iNumGrass = iNumGrass + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then
							innerCanHaveBonus = innerCanHaveBonus + 1;
						end
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						end
						if featureType == FeatureTypes.NO_FEATURE then
							iNumNativeTwoFoodFirstRing = iNumNativeTwoFoodFirstRing + 1;
						end
					elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
						innerFourFood = innerFourFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
						iNumNativeTwoFoodFirstRing = iNumNativeTwoFoodFirstRing + 1;
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						innerThreeFood = innerThreeFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
						iNumPlains = iNumPlains + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						else
							innerOneHammer = innerOneHammer + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
						innerThreeFood = innerThreeFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
						tundraCount = tundraCount + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						innerBadTiles = innerBadTiles + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1; -- Can have Oasis.
						desertCount = desertCount + 1;
					else -- Snow
						innerBadTiles = innerBadTiles + 1;
					end
				else -- Dry Flatlands
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						innerThreeFood = innerThreeFood + 1;
						iNumGrass = iNumGrass + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then
							innerCanHaveBonus = innerCanHaveBonus + 1;
						end
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						end
						if featureType == FeatureTypes.NO_FEATURE then
							iNumNativeTwoFoodFirstRing = iNumNativeTwoFoodFirstRing + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						innerTwoFood = innerTwoFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
						iNumPlains = iNumPlains + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						else
							innerOneHammer = innerOneHammer + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
						innerCanHaveBonus = innerCanHaveBonus + 1;
						tundraCount = tundraCount + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						else
							innerBadTiles = innerBadTiles + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						innerBadTiles = innerBadTiles + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1; -- Can have Oasis.
						desertCount = desertCount + 1;
					else -- Snow
						innerBadTiles = innerBadTiles + 1;
					end
				end
			end
		end
	end

	-- Evaluate Second Ring
	if isEvenY then
		search_table = self.secondRingYIsEven;
	else
		search_table = self.secondRingYIsOdd;
	end

	for _, plot_adjustments in ipairs(search_table) do
		local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments);

		if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then
			-- This plot does not exist. It's off the map edge.
			outerBadTiles = outerBadTiles + 1;
		else
			local searchPlot = Map.GetPlot(searchX, searchY);
			local plotType = searchPlot:GetPlotType();
			local terrainType = searchPlot:GetTerrainType();
			local featureType = searchPlot:GetFeatureType();

			if plotType == PlotTypes.PLOT_MOUNTAIN then
				outerBadTiles = outerBadTiles + 1;
			elseif plotType == PlotTypes.PLOT_OCEAN then
				if searchPlot:IsLake() then
					if featureType == FeatureTypes.FEATURE_ICE then
						outerBadTiles = outerBadTiles + 1;
					else
						outerTwoFood = outerTwoFood + 1;
						iNumNativeTwoFoodSecondRing = iNumNativeTwoFoodSecondRing + 1;
					end
				else
					if featureType == FeatureTypes.FEATURE_ICE then
						outerBadTiles = outerBadTiles + 1;
					elseif terrainType == TerrainTypes.TERRAIN_COAST then
						outerCanHaveBonus = outerCanHaveBonus + 1;
						outerOcean = outerOcean + 1;
					end
				end
			else -- Habitable plot.
				if featureType == FeatureTypes.FEATURE_JUNGLE then
					jungleCount = jungleCount + 1;
					iNumNativeTwoFoodSecondRing = iNumNativeTwoFoodSecondRing + 1;
				elseif featureType == FeatureTypes.FEATURE_FOREST then
					forestCount = forestCount + 1;
				end
				if searchPlot:IsRiver() then
					nearRiver = true;
				end
				if plotType == PlotTypes.PLOT_HILLS then
					outerHills = outerHills + 1;
					if featureType == FeatureTypes.FEATURE_JUNGLE then
						outerTwoFood = outerTwoFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
					elseif featureType == FeatureTypes.FEATURE_FOREST then
						outerCanHaveBonus = outerCanHaveBonus + 1;
					end
				elseif featureType == FeatureTypes.FEATURE_OASIS then
					innerThreeFood = innerThreeFood + 1;
					iNumNativeTwoFoodSecondRing = iNumNativeTwoFoodSecondRing + 1;
				elseif searchPlot:IsFreshWater() then
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						outerFourFood = outerFourFood + 1;
						iNumGrass = iNumGrass + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then
							outerCanHaveBonus = outerCanHaveBonus + 1;
						end
						if featureType == FeatureTypes.FEATURE_FOREST then
							outerForest = outerForest + 1;
						end
						if featureType == FeatureTypes.NO_FEATURE then
							iNumNativeTwoFoodSecondRing = iNumNativeTwoFoodSecondRing + 1;
						end
					elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
						outerFourFood = outerFourFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
						iNumNativeTwoFoodSecondRing = iNumNativeTwoFoodSecondRing + 1;
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						outerThreeFood = outerThreeFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
						iNumPlains = iNumPlains + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							outerForest = outerForest + 1;
						else
							outerOneHammer = outerOneHammer + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
						outerThreeFood = outerThreeFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
						tundraCount = tundraCount + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							outerForest = outerForest + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						outerBadTiles = outerBadTiles + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1; -- Can have Oasis.
						desertCount = desertCount + 1;
					else -- Snow
						outerBadTiles = outerBadTiles + 1;
					end
				else -- Dry Flatlands
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						outerThreeFood = outerThreeFood + 1;
						iNumGrass = iNumGrass + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then
							outerCanHaveBonus = outerCanHaveBonus + 1;
						end
						if featureType == FeatureTypes.FEATURE_FOREST then
							outerForest = outerForest + 1;
						end
						if featureType == FeatureTypes.NO_FEATURE then
							iNumNativeTwoFoodSecondRing = iNumNativeTwoFoodSecondRing + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						outerTwoFood = outerTwoFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
						iNumPlains = iNumPlains + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							outerForest = outerForest + 1;
						else
							outerOneHammer = outerOneHammer + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
						outerCanHaveBonus = outerCanHaveBonus + 1;
						tundraCount = tundraCount + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							outerForest = outerForest + 1;
						else
							outerBadTiles = outerBadTiles + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						outerBadTiles = outerBadTiles + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1; -- Can have Oasis.
						desertCount = desertCount + 1;
					else -- Snow
						outerBadTiles = outerBadTiles + 1;
					end
				end
			end
		end
	end

	-- Adjust the hammer situation, if needed.
	local innerHammerScore = 4 * innerHills + 2 * innerForest + innerOneHammer;
	local outerHammerScore = 2 * outerHills + outerForest + outerOneHammer;
	local earlyHammerScore = 2 * innerForest + outerForest + innerOneHammer + outerOneHammer;
	local randomized_first_ring_adjustments, randomized_second_ring_adjustments, randomized_third_ring_adjustments;
	-- If drastic shortage, attempt to add a hill to first ring.
	if (outerHammerScore < 8 and innerHammerScore < 2) or innerHammerScore == 0 then -- Change a first ring plot to Hills.
		if isEvenY then
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsEven);
		else
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsOdd);
		end
		for attempt = 1, 6 do
			local plot_adjustments = randomized_first_ring_adjustments[attempt];
			local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments);
			-- Attempt to place a Hill at the currently chosen plot.
			local placedHill = self:AttemptToPlaceHillsAtPlot(searchX, searchY);
			if placedHill then
				innerHammerScore = innerHammerScore + 4;
				print("Added hills next to hammer-poor start plot at ", x, y);
				break;
			elseif attempt == 6 then
				print("FAILED to add hills next to hammer-poor start plot at ", x, y);
			end
		end
	end

	-- Evaluate Third Ring (for counting desert and tundra)
	if isEvenY then
		search_table = self.thirdRingYIsEven;
	else
		search_table = self.thirdRingYIsOdd;
	end

	for _, plot_adjustments in ipairs(search_table) do
		local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments);
		if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then
			-- This plot does not exist. It's off the map edge.
		else
			local searchPlot = Map.GetPlot(searchX, searchY);
			local terrainType = searchPlot:GetTerrainType();
			local featureType = searchPlot:GetFeatureType();
			if terrainType == TerrainTypes.TERRAIN_TUNDRA then
				tundraCount = tundraCount + 1;
			elseif terrainType == TerrainTypes.TERRAIN_DESERT and
				featureType ~= FeatureTypes.FEATURE_FLOOD_PLAINS and
				featureType ~= FeatureTypes.FEATURE_OASIS then
				desertCount = desertCount + 1;
			end
		end
	end

	-- Add mandatory Iron and Horse to every start if Strategic Balance option is enabled.
	if self.resBalance then
		self:AddStrategicBalanceResources(region_number);
	end

	-- If early hammers will be too short, attempt to add a small Horse or Iron to second ring.
	-- Add a forest instead -- September 2020, azum4roll
	if innerHammerScore < 3 and earlyHammerScore < 6 then -- Add a forest to second ring.
		if isEvenY then
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsEven);
		else
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsOdd);
		end
		for attempt = 1, 12 do
			local plot_adjustments = randomized_second_ring_adjustments[attempt];
			-- Attempt to place a Forest at the currently chosen plot.
			if plot:GetResourceType() == -1 then -- No resource here, safe to proceed.
				local plotType = plot:GetPlotType();
				local terrainType = plot:GetTerrainType();
				local featureType = plot:GetFeatureType();
				if plotType == PlotTypes.PLOT_LAND then
					if terrainType ~= TerrainTypes.TERRAIN_DESERT and terrainType ~= TerrainTypes.TERRAIN_SNOW then
						if featureType == FeatureTypes.NO_FEATURE then
							plot:SetFeatureType(FeatureTypes.FEATURE_FOREST);
							break;
						end
					end
				end
			end
			if attempt == 12 then
				print("FAILED to add forest near hammer-poor start plot at ", x, y);
			end
		end
	end

	-- Rate the food situation.
	local innerFoodScore = (4 * innerFourFood) + (2 * innerThreeFood) + innerTwoFood;
	local outerFoodScore = (4 * outerFourFood) + (2 * outerThreeFood) + outerTwoFood;
	local totalFoodScore = innerFoodScore + outerFoodScore;
	local nativeTwoFoodTiles = iNumNativeTwoFoodFirstRing + iNumNativeTwoFoodSecondRing;

	-- Six levels for Bonus Resource support, from zero to five.
	if totalFoodScore < 4 and innerFoodScore == 0 then
		iNumFoodBonusNeeded = 5;
	elseif totalFoodScore < 6 then
		iNumFoodBonusNeeded = 4;
	elseif totalFoodScore < 8 then
		iNumFoodBonusNeeded = 3;
	elseif totalFoodScore < 12 and innerFoodScore < 5 then
		iNumFoodBonusNeeded = 3;
	elseif totalFoodScore < 17 and innerFoodScore < 9 then
		iNumFoodBonusNeeded = 2;
	elseif nativeTwoFoodTiles <= 1 then
		iNumFoodBonusNeeded = 2;
	elseif totalFoodScore < 24 and innerFoodScore < 11 then
		iNumFoodBonusNeeded = 1;
	elseif nativeTwoFoodTiles == 2 or iNumNativeTwoFoodFirstRing == 0 then
		iNumFoodBonusNeeded = 1;
	elseif totalFoodScore < 20 then
		iNumFoodBonusNeeded = 1;
	end

	-- Check for Legendary Start resource option.
	if self.legStart then
		iNumFoodBonusNeeded = iNumFoodBonusNeeded + 2;
	end

	-- Check to see if a Grass tile needs to be added at an all-plains site with zero native 2-food tiles in first two rings.
	if nativeTwoFoodTiles == 0 and iNumFoodBonusNeeded < 3 then
		local odd = self.firstRingYIsOdd;
		local even = self.firstRingYIsEven;
		local plot_list = {};
		-- For notes on how the hex-iteration works, refer to PlaceResourceImpact()
		local ripple_radius = 2;
		local currentX = x - ripple_radius;
		local currentY = y;
		for direction_index = 1, 6 do
			for _ = 1, ripple_radius do
				local plot_adjustments, nextX, nextY;
				if currentY / 2 > math.floor(currentY / 2) then
					plot_adjustments = odd[direction_index];
				else
					plot_adjustments = even[direction_index];
				end
				nextX = currentX + plot_adjustments[1];
				nextY = currentY + plot_adjustments[2];
				if not wrapX and (nextX < 0 or nextX >= iW) then
					-- X is out of bounds.
				elseif not wrapY and (nextY < 0 or nextY >= iH) then
					-- Y is out of bounds.
				else
					local realX = nextX;
					local realY = nextY;
					if wrapX then
						realX = realX % iW;
					end
					if wrapY then
						realY = realY % iH;
					end
					-- We've arrived at the correct x and y for the current plot.
					local plot = Map.GetPlot(realX, realY);
					if plot:GetResourceType() == -1 then -- No resource here, safe to proceed.
						local plotType = plot:GetPlotType();
						local terrainType = plot:GetTerrainType();
						local featureType = plot:GetFeatureType();
						local plotIndex = realY * iW + realX + 1;
						-- Now check this plot for eligibility to be converted to flat open grassland.
						if plotType == PlotTypes.PLOT_LAND then
							if terrainType == TerrainTypes.TERRAIN_PLAINS then
								if featureType == FeatureTypes.NO_FEATURE then
									table.insert(plot_list, plotIndex);
								end
							end
						end
					end
				end
				currentX, currentY = nextX, nextY;
			end
		end
		local iNumConversionCandidates = table.maxn(plot_list);
		if iNumConversionCandidates == 0 then
			iNumFoodBonusNeeded = 3;
		else
			print("-"); print("*** START HAD NO 2-FOOD TILES, YET ONLY QUALIFIED FOR 2 BONUS; CONVERTING A PLAINS TO GRASS! ***"); print("-");
			local diceroll = 1 + Map.Rand(iNumConversionCandidates, "Choosing plot to convert to Grass near food-poor Plains start - LUA");
			local conversionPlotIndex = plot_list[diceroll];
			local conv_x = (conversionPlotIndex - 1) % iW;
			local conv_y = (conversionPlotIndex - conv_x - 1) / iW;
			local plot = Map.GetPlot(conv_x, conv_y);
			plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, false);
			self:PlaceStrategicResourceImpact(conv_x, conv_y, 0) -- Disallow strategic resources at this plot, to keep it a farm plot.
		end
	end
	-- Add Bonus Resources to food-poor start positions.
	if iNumFoodBonusNeeded > 0 then
		local maxBonusesPossible = innerCanHaveBonus + outerCanHaveBonus;

		print("-");
		print("Food-Poor start ", x, y, " needs ", iNumFoodBonusNeeded, " Bonus, with ", maxBonusesPossible, " eligible plots.");
		print("-");

		local innerPlaced, outerPlaced = 0, 0;
		if isEvenY then
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsEven);
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsEven);
			randomized_third_ring_adjustments = GetShuffledCopyOfTable(self.thirdRingYIsEven);
		else
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsOdd);
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsOdd);
			randomized_third_ring_adjustments = GetShuffledCopyOfTable(self.thirdRingYIsOdd);
		end
		local tried_all_first_ring = false;
		local tried_all_second_ring = false;
		local tried_all_third_ring = false;
		local allow_oasis = true; -- Permanent flag. (We don't want to place more than one Oasis per location).
		while iNumFoodBonusNeeded > 0 do
			if ((innerPlaced < 2 and innerCanHaveBonus > 0) or (self.legStart and innerPlaced < 3 and innerCanHaveBonus > 0)) and not tried_all_first_ring then
				-- Add bonus to inner ring.
				for attempt = 1, 6 do
					local plot_adjustments = randomized_first_ring_adjustments[attempt];
					local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments);
					-- Attempt to place a Bonus at the currently chosen plot.
					local placedBonus, placedOasis = self:AttemptToPlaceBonusResourceAtPlot(searchX, searchY, allow_oasis);
					if placedBonus then
						if allow_oasis and placedOasis then -- First oasis was placed on this pass, so change permission.
							allow_oasis = false;
						end
						print("Placed a Bonus in first ring at ", searchX, searchY);
						innerPlaced = innerPlaced + 1;
						innerCanHaveBonus = innerCanHaveBonus - 1;
						iNumFoodBonusNeeded = iNumFoodBonusNeeded - 1;
						break;
					elseif attempt == 6 then
						tried_all_first_ring = true;
					end
				end

			elseif ((innerPlaced + outerPlaced < 5 and outerCanHaveBonus > 0) or (self.legStart and innerPlaced + outerPlaced < 4 and outerCanHaveBonus > 0)) and
				not tried_all_second_ring then
				-- Add bonus to second ring.
				for attempt = 1, 12 do
					local plot_adjustments = randomized_second_ring_adjustments[attempt];
					local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments);
					-- Attempt to place a Bonus at the currently chosen plot.
					local placedBonus, placedOasis = self:AttemptToPlaceBonusResourceAtPlot(searchX, searchY, allow_oasis);
					if placedBonus then
						if allow_oasis and placedOasis then -- First oasis was placed on this pass, so change permission.
							allow_oasis = false;
						end
						print("Placed a Bonus in second ring at ", searchX, searchY);
						outerPlaced = outerPlaced + 1;
						outerCanHaveBonus = outerCanHaveBonus - 1;
						iNumFoodBonusNeeded = iNumFoodBonusNeeded - 1;
						break;
					elseif attempt == 12 then
						tried_all_second_ring = true;
					end
				end

			elseif not tried_all_third_ring then
				-- Add bonus to third ring.
				for attempt = 1, 18 do
					local plot_adjustments = randomized_third_ring_adjustments[attempt];
					local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments);
					-- Attempt to place a Bonus at the currently chosen plot.
					local placedBonus, placedOasis = self:AttemptToPlaceBonusResourceAtPlot(searchX, searchY, allow_oasis);
					if placedBonus then
						if allow_oasis and placedOasis then -- First oasis was placed on this pass, so change permission.
							allow_oasis = false;
						end
						print("Placed a Bonus in third ring at ", searchX, searchY);
						iNumFoodBonusNeeded = iNumFoodBonusNeeded - 1;
						break;
					elseif attempt == 18 then
						tried_all_third_ring = true;
					end
				end
			else -- Tried everywhere, have to give up.
				break;
			end
		end
	end

	-- Check for heavy grass and light plains. Adding Stone if grass count is high and plains count is low. - May 2011, BT
	-- Changed to check for only FLAT grass/plains and add forests instead. - September 2020, azum4roll
	-- Add Cattle instead. - June 2023, azum4roll
	local iNumStoneNeeded = 0;
	if iNumGrass >= 9 and iNumPlains == 0 then
		iNumStoneNeeded = 2;
	elseif iNumGrass >= 6 and iNumPlains <= 4 then
		iNumStoneNeeded = 1;
	end
	if iNumStoneNeeded > 0 then -- Add Forest to this grass start.
		local innerPlaced = 0;
		if isEvenY then
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsEven);
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsEven);
		else
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsOdd);
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsOdd);
		end
		local tried_all_first_ring = false;
		local tried_all_second_ring = false;
		while iNumStoneNeeded > 0 do
			if innerPlaced < 1 and not tried_all_first_ring then
				-- Add bonus to inner ring.
				for attempt = 1, 6 do
					local plot_adjustments = randomized_first_ring_adjustments[attempt];
					local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments);
					-- Attempt to place Cattle at the currently chosen plot.
					local placedBonus = self:AttemptToPlaceCattleAtGrassPlot(searchX, searchY);
					if placedBonus then
						print("Placed Cattle in first ring at ", searchX, searchY);
						innerPlaced = innerPlaced + 1;
						iNumStoneNeeded = iNumStoneNeeded - 1;
						break;
					elseif attempt == 6 then
						tried_all_first_ring = true;
					end
				end

			elseif not tried_all_second_ring then
				-- Add bonus to second ring.
				for attempt = 1, 12 do
					local plot_adjustments = randomized_second_ring_adjustments[attempt];
					local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments);
					-- Attempt to place Cattle at the currently chosen plot.
					local placedBonus = self:AttemptToPlaceCattleAtGrassPlot(searchX, searchY);
					if placedBonus then
						print("Placed Cattle in second ring at ", searchX, searchY);
						iNumStoneNeeded = iNumStoneNeeded - 1;
						break;
					elseif attempt == 12 then
						tried_all_second_ring = true;
					end
				end

			else -- Tried everywhere, have to give up.
				break;
			end
		end
	end

	-- Record conditions at this start location.
	local results_table = {alongOcean, nextToLake, isRiver, nearRiver, nearMountain, forestCount, jungleCount, tundraCount, desertCount};
	self.startLocationConditions[region_number] = results_table;
end
------------------------------------------------------------------------------
function AssignStartingPlots:FindFallbackForUnmatchedRegionPriority(iRegionType, regions_still_available)
	-- This function acts upon Civs with a single Region Priority who were unable to be matched to a region of their priority type.
	-- We will scan remaining regions for the one with the most plots of the matching terrain type.
	local tMaxMatch = table.fill(0, NUM_REGION_TYPES);
	local tBestFallback = table.fill(-1, NUM_REGION_TYPES);
	local tTerrainCounts = table.fill(-1, NUM_REGION_TYPES);

	for _, region_number in ipairs(regions_still_available) do
		local tRawCounts = self.regionTerrainCounts[region_number];
		tTerrainCounts[RegionTypes.REGION_HILLS] = tRawCounts[5];
		tTerrainCounts[RegionTypes.REGION_GRASSLAND] = tRawCounts[11];
		tTerrainCounts[RegionTypes.REGION_PLAINS] = tRawCounts[12];
		tTerrainCounts[RegionTypes.REGION_DESERT] = tRawCounts[13] + math.floor((tRawCounts[20] + tRawCounts[21]) / 2);
		tTerrainCounts[RegionTypes.REGION_TUNDRA] = tRawCounts[14];
		tTerrainCounts[RegionTypes.REGION_FOREST] = tRawCounts[16];
		tTerrainCounts[RegionTypes.REGION_JUNGLE] = tRawCounts[17];
		tTerrainCounts[RegionTypes.REGION_HYBRID] = tRawCounts[11] + tRawCounts[12];

		if tTerrainCounts[iRegionType] > tMaxMatch[iRegionType] then
			tBestFallback[iRegionType] = region_number;
			tMaxMatch[iRegionType] = tTerrainCounts[iRegionType];
		end
	end

	if tBestFallback[iRegionType] ~= -1 then
		return tBestFallback[iRegionType];
	end

	return -1;
end
------------------------------------------------------------------------------
function AssignStartingPlots:NormalizeTeamLocations()
	-- This function will reorganize which Civs are assigned to which start
	-- locations, to ensure that Civs on the same team start near one another.
	-- Game:NormalizeStartingPlotLocations();
end
------------------------------------------------------------------------------
function AssignStartingPlots:BalanceAndAssign(args)
	-- This function determines what level of Bonus Resource support a location may need, identifies compatibility with civ-specific biases, and places starts.

	-- Now supports extra empty regions for maps like Frontier.
	args = args or {};
	local iNumExtraRegions = args.iNumExtraRegions or 0;
	local iNumRegions = self.iNumCivs + iNumExtraRegions;

	-- Normalize each start plot location.
	local iNumStarts = table.maxn(self.startingPlots);
	for region_number = 1, iNumStarts do
		self:NormalizeStartLocation(region_number);
	end

	-- Check Game Option for disabling civ-specific biases.
	-- If they are to be disabled, then all civs are simply assigned to start plots at random.
	local bDisableStartBias = Game.GetCustomOption("GAMEOPTION_DISABLE_START_BIAS");
	if bDisableStartBias == 1 then
		-- print("-"); print("ALERT: Civ Start Biases have been selected to be Disabled!"); print("-");
		local playerList, regionList = {}, {};
		for loop = 1, self.iNumCivs do
			local player_ID = self.player_ID_list[loop];
			table.insert(playerList, player_ID);
		end

		for loop = 1, iNumRegions do
			table.insert(regionList, loop);
		end

		local regionListShuffled = GetShuffledCopyOfTable(regionList);
		for index, player_ID in ipairs(playerList) do
			if player_ID >= 0 then
				local region_number = regionListShuffled[index];
				local x = self.startingPlots[region_number][1];
				local y = self.startingPlots[region_number][2];
				print("Now placing Player#", player_ID, "in Region#", region_number, "at start plot:", x, y);
				local start_plot = Map.GetPlot(x, y);
				local player = Players[player_ID];
				player:SetStartingPlot(start_plot);
			end
		end

		-- If this is a team game (any team has more than one Civ in it) then make sure team members start near each other if possible.
		-- (This may scramble Civ biases in some cases, but there is no cure).
		if self.bTeamGame then
			self:NormalizeTeamLocations();
		end

		-- Done with un-biased Civ placement.
		return;
	end

	-- If the process reaches here, civ-specific start-location biases are enabled. Handle them now.
	-- Create a randomized list of all regions. As a region gets assigned, we'll remove it from the list.
	local all_regions = {};
	for loop = 1, iNumRegions do
		table.insert(all_regions, loop);
	end
	local regions_still_available = GetShuffledCopyOfTable(all_regions);

	local civs_needing_coastal_start = {};
	local civs_priority_coastal_start = {};
	local civs_needing_river_start = {};
	local civs_needing_region_priority = {};
	local civs_needing_region_avoid = {};
	local civs_needing_mountain_start = {};
	local civs_needing_snow_start = {};
	local regions_with_coastal_start = {};
	local regions_with_lake_start = {};
	local regions_with_river_start = {};
	local regions_with_near_river_start = {};
	local civ_status = table.fill(false, MAX_MAJOR_CIVS); -- Have to account for possible gaps in player ID numbers, for MP.
	local region_status = table.fill(false, iNumRegions);
	local priority_lists = {};
	local avoid_lists = {};
	local iNumCoastalCivs, iNumRiverCivs, iNumPriorityCivs, iNumAvoidCivs, iNumMountainCivs, iNumSnowCivs = 0, 0, 0, 0, 0, 0;
	local iNumCoastalCivsRemaining, iNumRiverCivsRemaining, iNumPriorityCivsRemaining, iNumAvoidCivsRemaining, iNumMountainCivsRemaining, iNumSnowCivsRemaining = 0, 0, 0, 0, 0, 0;

	-- print("-"); print("-"); print("--- DEBUG READOUT OF PLAYER START ASSIGNMENTS ---"); print("-");

	-- Generate lists of player needs. Each additional need type is subordinate to those
	-- that come before. In other words, each Civ can have only one need type.
	for loop = 1, self.iNumCivs do
		local playerNum = self.player_ID_list[loop]; -- MP games can have gaps between player numbers, so we cannot assume a sequential set of IDs.
		local player = Players[playerNum];
		local civType = GameInfo.Civilizations[player:GetCivilizationType()].Type;
		print("Player", playerNum, "of Civ Type", civType);
		if CivNeedsMountainStart(civType) then
			print("- - - - - - - needs Mountain Start!"); print("-");
			iNumMountainCivs = iNumMountainCivs + 1;
			iNumMountainCivsRemaining = iNumMountainCivsRemaining + 1;
			table.insert(civs_needing_mountain_start, playerNum);
		elseif CivNeedsSnowStart(civType) then
			print("- - - - - - - needs Snow Start!"); print("-");
			iNumSnowCivs = iNumSnowCivs + 1;
			iNumSnowCivsRemaining = iNumSnowCivsRemaining + 1;
			table.insert(civs_needing_snow_start, playerNum);
		elseif CivNeedsCoastalStart(civType) then
			print("- - - - - - - needs Coastal Start!"); print("-");
			iNumCoastalCivs = iNumCoastalCivs + 1;
			iNumCoastalCivsRemaining = iNumCoastalCivsRemaining + 1;
			table.insert(civs_needing_coastal_start, playerNum);
			local bPlaceFirst = CivNeedsPlaceFirstCoastalStart(civType);
			if bPlaceFirst then
				print("- - - - - - - needs to Place First!"); print("-");
				table.insert(civs_priority_coastal_start, playerNum);
			end
		elseif CivNeedsRiverStart(civType) then
			print("- - - - - - - needs River Start!"); print("-");
			iNumRiverCivs = iNumRiverCivs + 1;
			iNumRiverCivsRemaining = iNumRiverCivsRemaining + 1;
			table.insert(civs_needing_river_start, playerNum);
		elseif GetNumStartRegionPriorityForCiv(civType) > 0 then
			print("- - - - - - - needs Region Priority!"); print("-");
			local table_of_this_civs_priority_needs = GetStartRegionPriorityListForCiv_GetIDs(civType);
			iNumPriorityCivs = iNumPriorityCivs + 1;
			iNumPriorityCivsRemaining = iNumPriorityCivsRemaining + 1;
			table.insert(civs_needing_region_priority, playerNum);
			priority_lists[playerNum] = table_of_this_civs_priority_needs;
		elseif GetNumStartRegionAvoidForCiv(civType) > 0 then
			print("- - - - - - - needs Region Avoid!"); print("-");
			local table_of_this_civs_avoid_needs = GetStartRegionAvoidListForCiv_GetIDs(civType);
			iNumAvoidCivs = iNumAvoidCivs + 1;
			iNumAvoidCivsRemaining = iNumAvoidCivsRemaining + 1;
			table.insert(civs_needing_region_avoid, playerNum);
			avoid_lists[playerNum] = table_of_this_civs_avoid_needs;
		end
	end
	--[[
	print("Civs with Coastal Bias:", iNumCoastalCivs);
	print("Civs with River Bias:", iNumRiverCivs);
	print("Civs with Region Priority:", iNumPriorityCivs);
	print("Civs with Region Avoid:", iNumAvoidCivs); print("-");
	--]]
	-- Handle Snow Bias (maximum one civ for now)
	if iNumSnowCivs > 0 then
		for region_number, bAlreadyAssigned in ipairs(region_status) do
			if not bAlreadyAssigned and self.regionTypes[region_number] == 10 then
				local iPlayerNum = civs_needing_snow_start[1];
				local x = self.startingPlots[region_number][1];
				local y = self.startingPlots[region_number][2];
				local plot = Map.GetPlot(x, y);
				local player = Players[iPlayerNum];
				player:SetStartingPlot(plot);
				print("Player Number", iPlayerNum, "with Snow bias assigned to Region#", region_number, "at Plot", x, y);
				iNumSnowCivsRemaining = iNumSnowCivsRemaining - 1;
				region_status[region_number] = true;
				civ_status[iPlayerNum + 1] = true;
				local a, _, c = IdentifyTableIndex(regions_still_available, region_number);
				if a then
					table.remove(regions_still_available, c[1]);
				end
			end
		end
	end

	-- Handle Mountain Bias (maximum one civ for now)
	if iNumMountainCivs > 0 then
		for region_number, bAlreadyAssigned in ipairs(region_status) do
			if not bAlreadyAssigned and self.regionTypes[region_number] == 9 then
				local iPlayerNum = civs_needing_mountain_start[1];
				local x = self.startingPlots[region_number][1];
				local y = self.startingPlots[region_number][2];
				local plot = Map.GetPlot(x, y);
				local player = Players[iPlayerNum];
				player:SetStartingPlot(plot);
				print("Player Number", iPlayerNum, "with Mountain bias assigned to Region#", region_number, "at Plot", x, y);
				iNumMountainCivsRemaining = iNumMountainCivsRemaining - 1;
				region_status[region_number] = true;
				civ_status[iPlayerNum + 1] = true;
				local a, _, c = IdentifyTableIndex(regions_still_available, region_number);
				if a then
					table.remove(regions_still_available, c[1]);
				end
			end
		end
	end

	-- Handle Coastal Start Bias
	if iNumCoastalCivs > 0 then
		-- Generate lists of regions eligible to support a coastal start.
		local iNumRegionsWithCoastalStart, iNumRegionsWithLakeStart, iNumUnassignableCoastStarts = 0, 0, 0;
		for region_number, bAlreadyAssigned in ipairs(region_status) do
			if not bAlreadyAssigned then
				if self.startLocationConditions[region_number][1] then
					-- print("Region#", region_number, "has a Coastal Start.");
					iNumRegionsWithCoastalStart = iNumRegionsWithCoastalStart + 1;
					table.insert(regions_with_coastal_start, region_number);
				end
			end
		end
		if iNumRegionsWithCoastalStart < iNumCoastalCivs then
			for region_number, bAlreadyAssigned in ipairs(region_status) do
				if not bAlreadyAssigned then
					if self.startLocationConditions[region_number][2] and not self.startLocationConditions[region_number][1] then
						-- print("Region#", region_number, "has a Lake Start.");
						iNumRegionsWithLakeStart = iNumRegionsWithLakeStart + 1;
						table.insert(regions_with_lake_start, region_number);
					end
				end
			end
		end
		if iNumRegionsWithCoastalStart + iNumRegionsWithLakeStart < iNumCoastalCivs then
			iNumUnassignableCoastStarts = iNumCoastalCivs - (iNumRegionsWithCoastalStart + iNumRegionsWithLakeStart);
		end
		-- Now assign those with coastal bias to start locations, where possible.
		-- print("iNumCoastalCivs: " .. iNumCoastalCivs);
		-- print("iNumUnassignableCoastStarts: " .. iNumUnassignableCoastStarts);
		if iNumCoastalCivs - iNumUnassignableCoastStarts > 0 then
			-- create non-priority coastal start list
			local non_priority_coastal_start = {};
			for _, iPlayerNum1 in ipairs(civs_needing_coastal_start) do
				local bAdd = true;
				for _, iPlayerNum2 in ipairs(civs_priority_coastal_start) do
					if iPlayerNum1 == iPlayerNum2 then
						bAdd = false;
					end
				end
				if bAdd then
					table.insert(non_priority_coastal_start, iPlayerNum1);
				end
			end

			local shuffled_priority_coastal_start = GetShuffledCopyOfTable(civs_priority_coastal_start);
			local shuffled_non_priority_coastal_start = GetShuffledCopyOfTable(non_priority_coastal_start);
			local shuffled_coastal_civs = {};

			-- insert priority coastal starts first
			for _, iPlayerNum in ipairs(shuffled_priority_coastal_start) do
				table.insert(shuffled_coastal_civs, iPlayerNum);
			end

			-- insert non-priority coastal starts second
			for _, iPlayerNum in ipairs(shuffled_non_priority_coastal_start) do
				table.insert(shuffled_coastal_civs, iPlayerNum);
			end
			--[[
			for loop, iPlayerNum in ipairs(shuffled_coastal_civs) do
				print("shuffled_coastal_civs[" .. loop .. "]: " .. iPlayerNum);
			end
			--]]
			local shuffled_coastal_regions, shuffled_lake_regions;
			local current_lake_index = 1;
			if iNumRegionsWithCoastalStart > 0 then
				shuffled_coastal_regions = GetShuffledCopyOfTable(regions_with_coastal_start);
			end
			if iNumRegionsWithLakeStart > 0 then
				shuffled_lake_regions = GetShuffledCopyOfTable(regions_with_lake_start);
			end
			for loop, playerNum in ipairs(shuffled_coastal_civs) do
				if loop > iNumCoastalCivs - iNumUnassignableCoastStarts then
					-- print("Ran out of Coastal and Lake start locations to assign to Coastal Bias.");
					break;
				end
				-- Assign next randomly chosen civ in line to next randomly chosen eligible region.
				if loop <= iNumRegionsWithCoastalStart then
					-- Assign this civ to a region with coastal start.
					local choose_this_region = shuffled_coastal_regions[loop];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[playerNum];
					player:SetStartingPlot(plot);
					print("Player Number", playerNum, "assigned a COASTAL START BIAS location in Region#", choose_this_region, "at Plot", x, y);
					region_status[choose_this_region] = true;
					civ_status[playerNum + 1] = true;
					iNumCoastalCivsRemaining = iNumCoastalCivsRemaining - 1;
					local a, _, c = IdentifyTableIndex(civs_needing_coastal_start, playerNum);
					if a then
						table.remove(civs_needing_coastal_start, c[1]);
					end
					a, _, c = IdentifyTableIndex(regions_still_available, choose_this_region);
					if a then
						table.remove(regions_still_available, c[1]);
					end
				else
					-- Out of coastal starts, assign this civ to region with lake start.
					local choose_this_region = shuffled_lake_regions[current_lake_index];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[playerNum];
					player:SetStartingPlot(plot);
					print("Player Number", playerNum, "with Coastal Bias assigned a fallback Lake location in Region#", choose_this_region, "at Plot", x, y);
					region_status[choose_this_region] = true;
					civ_status[playerNum + 1] = true;
					iNumCoastalCivsRemaining = iNumCoastalCivsRemaining - 1;
					local a, _, c = IdentifyTableIndex(civs_needing_coastal_start, playerNum);
					if a then
						table.remove(civs_needing_coastal_start, c[1]);
					end
					a, _, c = IdentifyTableIndex(regions_still_available, choose_this_region);
					if a then
						table.remove(regions_still_available, c[1]);
					end
					current_lake_index = current_lake_index + 1;
				end
			end
		else
			-- print("Either no civs required a Coastal Start, or no Coastal Starts were available.");
		end
	end

	-- Handle River bias
	if iNumRiverCivs > 0 or iNumCoastalCivsRemaining > 0 then
		-- Generate lists of regions eligible to support a river start.
		local iNumRegionsWithRiverStart, iNumRegionsNearRiverStart, iNumUnassignableRiverStarts = 0, 0, 0;
		for region_number, bAlreadyAssigned in ipairs(region_status) do
			if not bAlreadyAssigned then
				if self.startLocationConditions[region_number][3] then
					iNumRegionsWithRiverStart = iNumRegionsWithRiverStart + 1;
					table.insert(regions_with_river_start, region_number);
				end
			end
		end
		for region_number, bAlreadyAssigned in ipairs(region_status) do
			if not bAlreadyAssigned then
				if self.startLocationConditions[region_number][4] and not self.startLocationConditions[region_number][3] then
					iNumRegionsNearRiverStart = iNumRegionsNearRiverStart + 1;
					table.insert(regions_with_near_river_start, region_number);
				end
			end
		end
		if iNumRegionsWithRiverStart + iNumRegionsNearRiverStart < iNumRiverCivs then
			iNumUnassignableRiverStarts = iNumRiverCivs - (iNumRegionsWithRiverStart + iNumRegionsNearRiverStart);
		end
		-- Now assign those with river bias to start locations, where possible.
		-- Also handle fallback placement for coastal bias that failed to find a match.
		if iNumRiverCivs - iNumUnassignableRiverStarts > 0 then
			local shuffled_river_civs = GetShuffledCopyOfTable(civs_needing_river_start);
			local shuffled_river_regions, shuffled_near_river_regions;
			if iNumRegionsWithRiverStart > 0 then
				shuffled_river_regions = GetShuffledCopyOfTable(regions_with_river_start);
			end
			if iNumRegionsNearRiverStart > 0 then
				shuffled_near_river_regions = GetShuffledCopyOfTable(regions_with_near_river_start);
			end
			for loop, playerNum in ipairs(shuffled_river_civs) do
				if loop > iNumRiverCivs - iNumUnassignableRiverStarts then
					-- print("Ran out of River and Near-River start locations to assign to River Bias.");
					break;
				end
				-- Assign next randomly chosen civ in line to next randomly chosen eligible region.
				if loop <= iNumRegionsWithRiverStart then
					-- Assign this civ to a region with river start.
					local choose_this_region = shuffled_river_regions[loop];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[playerNum];
					player:SetStartingPlot(plot);
					print("Player Number", playerNum, "assigned a RIVER START BIAS location in Region#", choose_this_region, "at Plot", x, y);
					region_status[choose_this_region] = true;
					civ_status[playerNum + 1] = true;
					local a, _, c = IdentifyTableIndex(regions_still_available, choose_this_region);
					if a then
						table.remove(regions_still_available, c[1]);
					end
				else
					-- Assign this civ to a region where a river is near the start.
					local choose_this_region = shuffled_near_river_regions[loop - iNumRegionsWithRiverStart];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[playerNum];
					player:SetStartingPlot(plot);
					print("Player Number", playerNum, "with River Bias assigned a fallback 'near river' location in Region#", choose_this_region, "at Plot", x, y);
					region_status[choose_this_region] = true;
					civ_status[playerNum + 1] = true;
					local a, _, c = IdentifyTableIndex(regions_still_available, choose_this_region);
					if a then
						table.remove(regions_still_available, c[1]);
					end
				end
			end
		end
		-- Now handle any fallbacks for unassigned coastal bias.
		if iNumCoastalCivsRemaining > 0 and iNumRiverCivs < iNumRegionsWithRiverStart + iNumRegionsNearRiverStart then
			local iNumFallbacksWithRiverStart, iNumFallbacksNearRiverStart = 0, 0;
			local fallbacks_with_river_start, fallbacks_with_near_river_start = {}, {};
			for region_number, bAlreadyAssigned in ipairs(region_status) do
				if not bAlreadyAssigned then
					if self.startLocationConditions[region_number][3] then
						iNumFallbacksWithRiverStart = iNumFallbacksWithRiverStart + 1;
						table.insert(fallbacks_with_river_start, region_number);
					end
				end
			end
			for region_number, bAlreadyAssigned in ipairs(region_status) do
				if not bAlreadyAssigned then
					if self.startLocationConditions[region_number][4] and not self.startLocationConditions[region_number][3] then
						iNumFallbacksNearRiverStart = iNumFallbacksNearRiverStart + 1;
						table.insert(fallbacks_with_near_river_start, region_number);
					end
				end
			end
			if iNumFallbacksWithRiverStart + iNumFallbacksNearRiverStart > 0 then

				local shuffled_coastal_fallback_civs = GetShuffledCopyOfTable(civs_needing_coastal_start);
				local shuffled_river_fallbacks, shuffled_near_river_fallbacks;
				if iNumFallbacksWithRiverStart > 0 then
					shuffled_river_fallbacks = GetShuffledCopyOfTable(fallbacks_with_river_start);
				end
				if iNumFallbacksNearRiverStart > 0 then
					shuffled_near_river_fallbacks = GetShuffledCopyOfTable(fallbacks_with_near_river_start);
				end
				for loop, playerNum in ipairs(shuffled_coastal_fallback_civs) do
					if loop > iNumFallbacksWithRiverStart + iNumFallbacksNearRiverStart then
						print("Ran out of River and Near-River start locations to assign as fallbacks for Coastal Bias.");
						break;
					end
					-- Assign next randomly chosen civ in line to next randomly chosen eligible region.
					if loop <= iNumFallbacksWithRiverStart then
						-- Assign this civ to a region with river start.
						local choose_this_region = shuffled_river_fallbacks[loop];
						local x = self.startingPlots[choose_this_region][1];
						local y = self.startingPlots[choose_this_region][2];
						local plot = Map.GetPlot(x, y);
						local player = Players[playerNum];
						player:SetStartingPlot(plot);
						print("Player Number", playerNum, "with Coastal Bias assigned a fallback river location in Region#", choose_this_region, "at Plot", x, y);
						region_status[choose_this_region] = true;
						civ_status[playerNum + 1] = true;
						local a, _, c = IdentifyTableIndex(regions_still_available, choose_this_region);
						if a then
							table.remove(regions_still_available, c[1]);
						end
					else
						-- Assign this civ to a region where a river is near the start.
						local choose_this_region = shuffled_near_river_fallbacks[loop - iNumRegionsWithRiverStart];
						local x = self.startingPlots[choose_this_region][1];
						local y = self.startingPlots[choose_this_region][2];
						local plot = Map.GetPlot(x, y);
						local player = Players[playerNum];
						player:SetStartingPlot(plot);
						print("Player Number", playerNum, "with Coastal Bias assigned a fallback 'near river' location in Region#", choose_this_region, "at Plot", x, y);
						region_status[choose_this_region] = true;
						civ_status[playerNum + 1] = true;
						local a, _, c = IdentifyTableIndex(regions_still_available, choose_this_region);
						if a then
							table.remove(regions_still_available, c[1]);
						end
					end
				end
			end
		end
	end

	-- Handle Region Priority
	if iNumPriorityCivs > 0 then
		-- print("-"); print("-"); print("--- REGION PRIORITY READOUT ---"); print("-");
		local iNumSinglePriority, iNumMultiPriority, iNumNeedFallbackPriority = 0, 0, 0;
		local single_priority, multi_priority, fallback_priority = {}, {}, {};
		local single_sorted, multi_sorted = {}, {};
		-- Separate priority civs in to two categories: single priority, multiple priority.
		for playerNum, priority_needs in pairs(priority_lists) do
			local len = table.maxn(priority_needs);
			if len == 1 then
				-- print("Player#", playerNum, "has a single Region Priority of type", priority_needs[1]);
				local priority_data = {playerNum, priority_needs[1]};
				table.insert(single_priority, priority_data);
				iNumSinglePriority = iNumSinglePriority + 1;
			else
				-- print("Player#", playerNum, "has multiple Region Priority, this many types:", len);
				local priority_data = {playerNum, len};
				table.insert(multi_priority, priority_data);
				iNumMultiPriority = iNumMultiPriority + 1;
			end
		end
		-- Single priority civs go first, and will engage fallback methods if no match found.
		if iNumSinglePriority > 0 then
			-- Sort the list so that proper order of execution occurs. (Going to use a blunt method for easy coding.)
			for region_type = 1, NUM_REGION_TYPES - 1 do
				for _, data in ipairs(single_priority) do
					if data[2] == region_type then
						-- print("Adding Player#", data[1], "to sorted list of single Region Priority.");
						table.insert(single_sorted, data);
					end
				end
			end
			-- Match civs who have a single Region Priority to the region type they need, if possible.
			for _, data in ipairs(single_sorted) do
				local iPlayerNum = data[1];
				local iPriorityType = data[2];
				-- print("* Attempting to assign Player#", iPlayerNum, "to a region of Type#", iPriorityType);
				local bFoundCandidate, candidate_regions = false, {};
				for _, region_number in ipairs(regions_still_available) do
					if self.regionTypes[region_number] == iPriorityType then
						table.insert(candidate_regions, region_number);
						bFoundCandidate = true;
						-- print("- - Found candidate: Region#", region_number);
					end
				end
				if bFoundCandidate then
					local diceroll = 1 + Map.Rand(table.maxn(candidate_regions), "Choosing from among Candidate Regions for start bias - LUA");
					local choose_this_region = candidate_regions[diceroll];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[iPlayerNum];
					player:SetStartingPlot(plot);
					print("Player Number", iPlayerNum, "with single Region Priority assigned to Region#", choose_this_region, "at Plot", x, y);
					region_status[choose_this_region] = true;
					civ_status[iPlayerNum + 1] = true;
					local a, _, c = IdentifyTableIndex(regions_still_available, choose_this_region);
					if a then
						table.remove(regions_still_available, c[1]);
					end
				else
					table.insert(fallback_priority, data);
					iNumNeedFallbackPriority = iNumNeedFallbackPriority + 1;
					print("Player Number", iPlayerNum, "with single Region Priority was UNABLE to be matched to its type. Added to fallback list.");
				end
			end
		end
		-- Multiple priority civs go next, with fewest regions of priority going first.
		if iNumMultiPriority > 0 then
			for iNumPriorities = 2, 8 do -- Must expand if new region types are added.
				for _, data in ipairs(multi_priority) do
					if data[2] == iNumPriorities then
						-- print("Adding Player#", data[1], "to sorted list of multi Region Priority.");
						table.insert(multi_sorted, data);
					end
				end
			end
			-- Match civs who have mulitple Region Priority to one of the region types they need, if possible.
			for _, data in ipairs(multi_sorted) do
				local iPlayerNum = data[1];
				local iNumPriorityTypes = data[2];
				-- print("* Attempting to assign Player#", iPlayerNum, "to one of its Priority Region Types.");
				local bFoundCandidate, candidate_regions = false, {};
				for _, region_number in ipairs(regions_still_available) do
					for inner_loop = 1, iNumPriorityTypes do
						local region_type_to_test = priority_lists[iPlayerNum][inner_loop];
						if self.regionTypes[region_number] == region_type_to_test then
							table.insert(candidate_regions, region_number);
							bFoundCandidate = true;
							-- print("- - Found candidate: Region#", region_number);
						end
					end
				end
				if bFoundCandidate then
					local diceroll = 1 + Map.Rand(table.maxn(candidate_regions), "Choosing from among Candidate Regions for start bias - LUA");
					local choose_this_region = candidate_regions[diceroll];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[iPlayerNum];
					player:SetStartingPlot(plot);
					print("Player Number", iPlayerNum, "with multiple Region Priority assigned to Region#", choose_this_region, "at Plot", x, y);
					region_status[choose_this_region] = true;
					civ_status[iPlayerNum + 1] = true;
					local a, _, c = IdentifyTableIndex(regions_still_available, choose_this_region);
					if a then
						table.remove(regions_still_available, c[1]);
					end
				else
					print("Player Number", iPlayerNum, "with multiple Region Priority was unable to be matched.");
				end
			end
		end
		-- Fallbacks are done (if needed) after multiple-region priority is handled. The list is pre-sorted.
		if iNumNeedFallbackPriority > 0 then
			for _, data in ipairs(fallback_priority) do
				local iPlayerNum = data[1];
				local iPriorityType = data[2];
				-- print("* Attempting to assign Player#", iPlayerNum, "to a fallback region as similar as possible to Region Type#", iPriorityType);
				local choose_this_region = self:FindFallbackForUnmatchedRegionPriority(iPriorityType, regions_still_available);
				if choose_this_region == -1 then
					print("FAILED to find fallback region bias for player#", iPlayerNum);
				else
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[iPlayerNum];
					player:SetStartingPlot(plot);
					print("Player Number", iPlayerNum, "with single Region Priority assigned to FALLBACK Region#", choose_this_region, "at Plot", x, y);
					region_status[choose_this_region] = true;
					civ_status[iPlayerNum + 1] = true;
					local a, _, c = IdentifyTableIndex(regions_still_available, choose_this_region);
					if a then
						table.remove(regions_still_available, c[1]);
					end
				end
			end
		end
	end

	-- Handle Region Avoid
	if iNumAvoidCivs > 0 then
		-- print("-"); print("-"); print("--- REGION AVOID READOUT ---"); print("-");
		local avoid_sorted, avoid_unsorted, avoid_counts = {}, {}, {};
		-- Sort list of civs with Avoid needs, then process in reverse order, so most needs goes first.
		for playerNum, avoid_needs in pairs(avoid_lists) do
			local len = table.maxn(avoid_needs);
			-- print("- Player#", playerNum, "has this number of Region Avoid needs:", len);
			local avoid_data = {playerNum, len};
			table.insert(avoid_unsorted, avoid_data);
			table.insert(avoid_counts, len);
		end
		table.sort(avoid_counts);
		for _, avoid_count in ipairs(avoid_counts) do
			for test_loop, avoid_data in ipairs(avoid_unsorted) do
				if avoid_count == avoid_data[2] then
					table.insert(avoid_sorted, avoid_data[1]);
					table.remove(avoid_unsorted, test_loop);
				end
			end
		end
		-- Process the Region Avoid needs.
		for loop = iNumAvoidCivs, 1, -1 do
			local iPlayerNum = avoid_sorted[loop];
			local candidate_regions = {};
			for _, region_number in ipairs(regions_still_available) do
				local bFoundCandidate = true;
				for _, region_type_to_avoid in ipairs(avoid_lists[iPlayerNum]) do
					if self.regionTypes[region_number] == region_type_to_avoid then
						bFoundCandidate = false;
					end
				end
				if bFoundCandidate then
					table.insert(candidate_regions, region_number);
					-- print("- - Found candidate: Region#", region_number);
				end
			end
			if table.maxn(candidate_regions) > 0 then
				local diceroll = 1 + Map.Rand(table.maxn(candidate_regions), "Choosing from among Candidate Regions for start bias - LUA");
				local choose_this_region = candidate_regions[diceroll];
				local x = self.startingPlots[choose_this_region][1];
				local y = self.startingPlots[choose_this_region][2];
				local plot = Map.GetPlot(x, y);
				local player = Players[iPlayerNum];
				player:SetStartingPlot(plot);
				print("Player Number", iPlayerNum, "with Region Avoid assigned to allowed region type in Region#", choose_this_region, "at Plot", x, y);
				region_status[choose_this_region] = true;
				civ_status[iPlayerNum + 1] = true;
				local a, _, c = IdentifyTableIndex(regions_still_available, choose_this_region);
				if a then
					table.remove(regions_still_available, c[1]);
				end
			else
				print("Player Number", iPlayerNum, "with Region Avoid was unable to avoid the undesired region types.");
			end
		end
	end

	-- Assign remaining civs to start plots.
	local playerList, regionList = {}, {};
	for loop = 1, self.iNumCivs do
		local player_ID = self.player_ID_list[loop];
		if not civ_status[player_ID + 1] then -- Using C++ player ID, which starts at zero. Add 1 for Lua indexing.
			table.insert(playerList, player_ID);
		end
	end

	for loop = 1, iNumRegions do
		if not region_status[loop] then
			table.insert(regionList, loop);
		end
	end

	local iNumRemainingPlayers = table.maxn(playerList);
	local iNumRemainingRegions = table.maxn(regionList);

	--[[
	print("- - -");
	print("- Civ Count: ", self.iNumCivs);
	print("- Empty Region Count: ", iNumExtraRegions);
	print("- Players Left: ", iNumRemainingPlayers);
	print("- Regions Left: ", iNumRemainingRegions);
	print("- - -");
	--]]

	if iNumRemainingPlayers > 0 or iNumRemainingRegions > 0 then
		---[[
		print("-"); print("Table of players with no start bias:");
		PrintContentsOfTable(playerList);
		print("-"); print("Table of regions still available after bias handling:");
		PrintContentsOfTable(regionList);
		if iNumRemainingPlayers + iNumExtraRegions ~= iNumRemainingRegions then
			print("-"); print("ERROR: Number of civs remaining after handling biases does not match number of regions remaining!"); print("-");
		end
		--]]

		-- There should be more regions left than players (if we have empty regions), so we shuffle regions
		local regionListShuffled = GetShuffledCopyOfTable(regionList);
		for index, player_ID in ipairs(playerList) do
			if player_ID >= 0 then
				local region_number = regionListShuffled[index];
				local x = self.startingPlots[region_number][1];
				local y = self.startingPlots[region_number][2];
				print("Now placing Player#", player_ID, "in Region#", region_number, "at start plot:", x, y);
				local start_plot = Map.GetPlot(x, y);
				local player = Players[player_ID];
				player:SetStartingPlot(start_plot);
			end
		end
	end

	-- If this is a team game (any team has more than one Civ in it) then make sure team members start near each other if possible.
	-- (This may scramble Civ biases in some cases, but there is no cure).
	if self.bTeamGame then
		self:NormalizeTeamLocations();
	end
end
------------------------------------------------------------------------------
-- Start of functions tied to PlaceNaturalWonders()
------------------------------------------------------------------------------
function AssignStartingPlots:ExaminePlotForNaturalWondersEligibility(x, y)
	-- This function checks only for eligibility requirements applicable to all Natural Wonders.
	-- If a candidate plot passes all such checks, we will move on to checking it against specific needs for each particular wonderID.

	-- Update, May 2011: Control over wonderID placement is being migrated to XML. Some checks here moved to there.
	local iW, _ = Map.GetGridSize();
	local plotIndex = iW * y + x + 1;

	-- Check for collision with player starts
	if self.impactData[ImpactLayers.LAYER_NATURAL_WONDER][plotIndex] > 0 then
		return false, false;
	end

	-- Must have land in workable range
	local plot = Map.GetPlot(x, y);
	local hasLand = false;
	for nearPlot in self:Plot_GetPlotsInCircle(plot, 1, 3) do
		if nearPlot:GetPlotType() == PlotTypes.PLOT_LAND then
			hasLand = true;
		end
	end
	if not hasLand then
		return false, false;
	end

	-- Check the location is a decent city site, otherwise the wonderID is pointless
	local fertility = self:Plot_GetFertilityInRange(plot, 3);
	if fertility < 20 then
		return false, false;
	elseif fertility < 28 then
		return false, true;
	end
	return true, true;
end
------------------------------------------------------------------------------
function AssignStartingPlots:ExamineCandidatePlotForNaturalWondersEligibility(x, y)
	-- Now unused. Use ExaminePlotForNaturalWondersEligibility instead.
	return self:ExaminePlotForNaturalWondersEligibility(x, y);
end
------------------------------------------------------------------------------
function AssignStartingPlots:CanBeThisNaturalWonderType(x, y, wn)
	-- Checks a candidate plot for eligibility to host the supplied wonder type.
	local plot = Map.GetPlot(x, y);

	-- Check whether adjacent plots are valid
	for _, direction in ipairs(self.direction_types) do
		local adjPlot = Map.PlotDirection(x, y, direction);
		if not adjPlot then
			return;
		end
	end

	-- Use Custom Eligibility method if indicated.
	if self.EligibilityMethodNumber[wn] ~= -1 then
		local method_number = self.EligibilityMethodNumber[wn];
		if NWCustomEligibility(x, y, method_number) then
			local iW, _ = Map.GetGridSize();
			local plotIndex = y * iW + x + 1;
			table.insert(self.eligibility_lists[wn], plotIndex);
		end
		return;
	end

	-- Run root checks.
	if self.bWorldHasOceans then -- Check to see if this wonder requires or avoids the biggest landmass.
		if self.RequireBiggestLandmass[wn] then
			local iLandmassID = plot:GetLandmass();
			if iLandmassID ~= self.iBiggestLandmassID then
				return;
			end
		elseif self.AvoidBiggestLandmass[wn] then
			local iLandmassID = plot:GetLandmass();
			if iLandmassID == self.iBiggestLandmassID then
				return;
			end
		end
	end
	if self.RequireFreshWater[wn] then
		if not plot:IsFreshWater() then
			return;
		end
	elseif self.AvoidFreshWater[wn] then
		if plot:IsRiver() or plot:IsLake() or plot:IsFreshWater() then
			return;
		end
	end

	-- Land or Sea
	if self.LandBased[wn] then
		if plot:IsWater() then
			return;
		end
		local iW, _ = Map.GetGridSize();
		local plotIndex = y * iW + x + 1;
		if self.RequireLandAdjacentToOcean[wn] then
			if not self.plotDataIsCoastal[plotIndex] then
				return;
			end
		elseif self.AvoidLandAdjacentToOcean[wn] then
			if self.plotDataIsCoastal[plotIndex] then
				return;
			end
		end
		if self.RequireLandOnePlotInland[wn] then
			if not self.plotDataIsNextToCoast[plotIndex] then
				return;
			end
		elseif self.AvoidLandOnePlotInland[wn] then
			if self.plotDataIsNextToCoast[plotIndex] then
				return;
			end
		end
		if self.RequireLandTwoOrMorePlotsInland[wn] then
			if self.plotDataIsCoastal[plotIndex] then
				return;
			elseif self.plotDataIsNextToCoast[plotIndex] then
				return;
			end
		elseif self.AvoidLandTwoOrMorePlotsInland[wn] then
			if not self.plotDataIsCoastal[plotIndex] and not self.plotDataIsNextToCoast[plotIndex] then
				return;
			end
		end
	end

	-- Core Tile
	if not self.CoreTileCanBeAnyPlotType[wn] then
		local plotType = plot:GetPlotType();
		if plotType == PlotTypes.PLOT_LAND and self.CoreTileCanBeFlatland[wn] then
			-- Continue
		elseif plotType == PlotTypes.PLOT_HILLS and self.CoreTileCanBeHills[wn] then
			-- Continue
		elseif plotType == PlotTypes.PLOT_MOUNTAIN and self.CoreTileCanBeMountain[wn] then
			-- Continue
		elseif plotType == PlotTypes.PLOT_OCEAN and self.CoreTileCanBeOcean[wn] then
			-- Continue
		else -- Plot type does not match an eligible type, reject this plot.
			return;
		end
	end
	if not self.CoreTileCanBeAnyTerrainType[wn] then
		local terrainType = plot:GetTerrainType();
		if terrainType == TerrainTypes.TERRAIN_GRASS and self.CoreTileCanBeGrass[wn] then
			-- Continue
		elseif terrainType == TerrainTypes.TERRAIN_PLAINS and self.CoreTileCanBePlains[wn] then
			-- Continue
		elseif terrainType == TerrainTypes.TERRAIN_DESERT and self.CoreTileCanBeDesert[wn] then
			-- Continue
		elseif terrainType == TerrainTypes.TERRAIN_TUNDRA and self.CoreTileCanBeTundra[wn] then
			-- Continue
		elseif terrainType == TerrainTypes.TERRAIN_SNOW and self.CoreTileCanBeSnow[wn] then
			-- Continue
		elseif terrainType == TerrainTypes.TERRAIN_COAST and self.CoreTileCanBeShallowWater[wn] then
			-- Continue
		elseif terrainType == TerrainTypes.TERRAIN_OCEAN and self.CoreTileCanBeDeepWater[wn] then
			-- Continue
		else -- Terrain type does not match an eligible type, reject this plot.
			return;
		end
	end
	if not self.CoreTileCanBeAnyFeatureType[wn] then
		local featureType = plot:GetFeatureType();
		if featureType == FeatureTypes.NO_FEATURE and self.CoreTileCanBeNoFeature[wn] then
			-- Continue
		elseif featureType == FeatureTypes.FEATURE_FOREST and self.CoreTileCanBeForest[wn] then
			-- Continue
		elseif featureType == FeatureTypes.FEATURE_JUNGLE and self.CoreTileCanBeJungle[wn] then
			-- Continue
		elseif featureType == FeatureTypes.FEATURE_OASIS and self.CoreTileCanBeOasis[wn] then
			-- Continue
		elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS and self.CoreTileCanBeFloodPlains[wn] then
			-- Continue
		elseif featureType == FeatureTypes.FEATURE_MARSH and self.CoreTileCanBeMarsh[wn] then
			-- Continue
		elseif featureType == FeatureTypes.FEATURE_ICE and self.CoreTileCanBeIce[wn] then
			-- Continue
		elseif featureType == FeatureTypes.FEATURE_ATOLL and self.CoreTileCanBeAtoll[wn] then
			-- Continue
		else -- Feature type does not match an eligible type, reject this plot.
			return;
		end
	end
	-- Adjacent Tiles: Plot Types
	if self.AdjacentTilesCareAboutPlotTypes[wn] then
		local iNumAnyLand, iNumFlatland, iNumHills, iNumMountain, iNumHillsPlusMountains, iNumOcean = 0, 0, 0, 0, 0, 0;
		for _, direction in ipairs(self.direction_types) do
			local adjPlot = Map.PlotDirection(x, y, direction);
			local plotType = adjPlot:GetPlotType();
			if plotType == PlotTypes.PLOT_OCEAN then
				iNumOcean = iNumOcean + 1;
			else
				iNumAnyLand = iNumAnyLand + 1;
				if plotType == PlotTypes.PLOT_LAND then
					iNumFlatland = iNumFlatland + 1;
				else
					iNumHillsPlusMountains = iNumHillsPlusMountains + 1;
					if plotType == PlotTypes.PLOT_HILLS then
						iNumHills = iNumHills + 1;
					else
						iNumMountain = iNumMountain + 1;
					end
				end
			end
		end
		if iNumAnyLand > 0 and self.AdjacentTilesAvoidAnyland[wn] then
			return;
		end
		-- Require
		if self.AdjacentTilesRequireFlatland[wn] then
			if iNumFlatland < self.RequiredNumberOfAdjacentFlatland[wn] then
				return;
			end
		end
		if self.AdjacentTilesRequireHills[wn] then
			if iNumHills < self.RequiredNumberOfAdjacentHills[wn] then
				return;
			end
		end
		if self.AdjacentTilesRequireMountain[wn] then
			if iNumMountain < self.RequiredNumberOfAdjacentMountain[wn] then
				return;
			end
		end
		if self.AdjacentTilesRequireHillsPlusMountains[wn] then
			if iNumHillsPlusMountains < self.RequiredNumberOfAdjacentHillsPlusMountains[wn] then
				return;
			end
		end
		if self.AdjacentTilesRequireOcean[wn] then
			if iNumOcean < self.RequiredNumberOfAdjacentOcean[wn] then
				return;
			end
		end
		-- Avoid
		if self.AdjacentTilesAvoidFlatland[wn] then
			if iNumFlatland > self.MaximumAllowedAdjacentFlatland[wn] then
				return;
			end
		end
		if self.AdjacentTilesAvoidHills[wn] then
			if iNumHills > self.MaximumAllowedAdjacentHills[wn] then
				return;
			end
		end
		if self.AdjacentTilesAvoidMountain[wn] then
			if iNumMountain > self.MaximumAllowedAdjacentMountain[wn] then
				return;
			end
		end
		if self.AdjacentTilesAvoidHillsPlusMountains[wn] then
			if iNumHillsPlusMountains > self.MaximumAllowedAdjacentHillsPlusMountains[wn] then
				return;
			end
		end
		if self.AdjacentTilesAvoidOcean[wn] then
			if iNumOcean > self.MaximumAllowedAdjacentOcean[wn] then
				return;
			end
		end
	end

	-- Adjacent Tiles: Terrain Types
	if self.AdjacentTilesCareAboutTerrainTypes[wn] then
		local iNumGrass, iNumPlains, iNumDesert, iNumTundra, iNumSnow, iNumShallowWater, iNumDeepWater = 0, 0, 0, 0, 0, 0, 0;
		for _, direction in ipairs(self.direction_types) do
			local adjPlot = Map.PlotDirection(x, y, direction);
			local terrainType = adjPlot:GetTerrainType();
			if terrainType == TerrainTypes.TERRAIN_GRASS then
				iNumGrass = iNumGrass + 1;
			elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
				iNumPlains = iNumPlains + 1;
			elseif terrainType == TerrainTypes.TERRAIN_DESERT then
				iNumDesert = iNumDesert + 1;
			elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
				iNumTundra = iNumTundra + 1;
			elseif terrainType == TerrainTypes.TERRAIN_SNOW then
				iNumSnow = iNumSnow + 1;
			elseif terrainType == TerrainTypes.TERRAIN_COAST then
				iNumShallowWater = iNumShallowWater + 1;
			elseif terrainType == TerrainTypes.TERRAIN_OCEAN then
				iNumDeepWater = iNumDeepWater + 1;
			end
		end
		-- Require
		if self.AdjacentTilesRequireGrass[wn] then
			if iNumGrass < self.RequiredNumberOfAdjacentGrass[wn] then
				return;
			end
		end
		if self.AdjacentTilesRequirePlains[wn] then
			if iNumPlains < self.RequiredNumberOfAdjacentPlains[wn] then
				return;
			end
		end
		if self.AdjacentTilesRequireDesert[wn] then
			if iNumDesert < self.RequiredNumberOfAdjacentDesert[wn] then
				return;
			end
		end
		if self.AdjacentTilesRequireTundra[wn] then
			if iNumTundra < self.RequiredNumberOfAdjacentTundra[wn] then
				return;
			end
		end
		if self.AdjacentTilesRequireSnow[wn] then
			if iNumSnow < self.RequiredNumberOfAdjacentSnow[wn] then
				return;
			end
		end
		if self.AdjacentTilesRequireShallowWater[wn] then
			if iNumShallowWater < self.RequiredNumberOfAdjacentShallowWater[wn] then
				return;
			end
		end
		if self.AdjacentTilesRequireGrass[wn] then
			if iNumDeepWater < self.RequiredNumberOfAdjacentDeepWater[wn] then
				return;
			end
		end
		-- Avoid
		if self.AdjacentTilesAvoidGrass[wn] then
			if iNumGrass > self.MaximumAllowedAdjacentGrass[wn] then
				return;
			end
		end
		if self.AdjacentTilesAvoidPlains[wn] then
			if iNumPlains > self.MaximumAllowedAdjacentPlains[wn] then
				return;
			end
		end
		if self.AdjacentTilesAvoidDesert[wn] then
			if iNumDesert > self.MaximumAllowedAdjacentDesert[wn] then
				return;
			end
		end
		if self.AdjacentTilesAvoidTundra[wn] then
			if iNumTundra > self.MaximumAllowedAdjacentTundra[wn] then
				return;
			end
		end
		if self.AdjacentTilesAvoidSnow[wn] then
			if iNumSnow > self.MaximumAllowedAdjacentSnow[wn] then
				return;
			end
		end
		if self.AdjacentTilesAvoidShallowWater[wn] then
			if iNumShallowWater > self.MaximumAllowedAdjacentShallowWater[wn] then
				return;
			end
		end
		if self.AdjacentTilesAvoidDeepWater[wn] then
			if iNumDeepWater > self.MaximumAllowedAdjacentDeepWater[wn] then
				return;
			end
		end
	end

	-- Adjacent Tiles: Feature Types
	if self.AdjacentTilesCareAboutFeatureTypes[wn] then
		local iNumNoFeature, iNumForest, iNumJungle, iNumOasis, iNumFloodPlains, iNumMarsh, iNumIce, iNumAtoll = 0, 0, 0, 0, 0, 0, 0, 0;
		for _, direction in ipairs(self.direction_types) do
			local adjPlot = Map.PlotDirection(x, y, direction);
			local featureType = adjPlot:GetFeatureType();
			if featureType == FeatureTypes.NO_FEATURE then
				iNumNoFeature = iNumNoFeature + 1;
			elseif featureType == FeatureTypes.FEATURE_FOREST then
				iNumForest = iNumForest + 1;
			elseif featureType == FeatureTypes.FEATURE_JUNGLE then
				iNumJungle = iNumJungle + 1;
			elseif featureType == FeatureTypes.FEATURE_OASIS then
				iNumOasis = iNumOasis + 1;
			elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
				iNumFloodPlains = iNumFloodPlains + 1;
			elseif featureType == FeatureTypes.FEATURE_MARSH then
				iNumMarsh = iNumMarsh + 1;
			elseif featureType == FeatureTypes.FEATURE_ICE then
				iNumIce = iNumIce + 1;
			elseif featureType == FeatureTypes.FEATURE_ATOLL then
				iNumAtoll = iNumAtoll + 1;
			end
		end
		-- Require
		if self.AdjacentTilesRequireNoFeature[wn] then
			if iNumNoFeature < self.RequiredNumberOfAdjacentNoFeature[wn] then
				return;
			end
		end
		if self.AdjacentTilesRequireForest[wn] then
			if iNumForest < self.RequiredNumberOfAdjacentForest[wn] then
				return;
			end
		end
		if self.AdjacentTilesRequireJungle[wn] then
			if iNumJungle < self.RequiredNumberOfAdjacentJungle[wn] then
				return;
			end
		end
		if self.AdjacentTilesRequireOasis[wn] then
			if iNumOasis < self.RequiredNumberOfAdjacentOasis[wn] then
				return;
			end
		end
		if self.AdjacentTilesRequireFloodPlains[wn] then
			if iNumFloodPlains < self.RequiredNumberOfAdjacentFloodPlains[wn] then
				return;
			end
		end
		if self.AdjacentTilesRequireMarsh[wn] then
			if iNumMarsh < self.RequiredNumberOfAdjacentMarsh[wn] then
				return;
			end
		end
		if self.AdjacentTilesRequireIce[wn] then
			if iNumIce < self.RequiredNumberOfAdjacentIce[wn] then
				return;
			end
		end
		if self.AdjacentTilesRequireAtoll[wn] then
			if iNumAtoll < self.RequiredNumberOfAdjacentAtoll[wn] then
				return;
			end
		end
		-- Avoid
		if self.AdjacentTilesAvoidNoFeature[wn] then
			if iNumNoFeature > self.MaximumAllowedAdjacentNoFeature[wn] then
				return;
			end
		end
		if self.AdjacentTilesAvoidForest[wn] then
			if iNumForest > self.MaximumAllowedAdjacentForest[wn] then
				return;
			end
		end
		if self.AdjacentTilesAvoidJungle[wn] then
			if iNumJungle > self.MaximumAllowedAdjacentJungle[wn] then
				return;
			end
		end
		if self.AdjacentTilesAvoidOasis[wn] then
			if iNumOasis > self.MaximumAllowedAdjacentOasis[wn] then
				return;
			end
		end
		if self.AdjacentTilesAvoidFloodPlains[wn] then
			if iNumFloodPlains > self.MaximumAllowedAdjacentFloodPlains[wn] then
				return;
			end
		end
		if self.AdjacentTilesAvoidMarsh[wn] then
			if iNumMarsh > self.MaximumAllowedAdjacentMarsh[wn] then
				return;
			end
		end
		if self.AdjacentTilesAvoidIce[wn] then
			if iNumIce > self.MaximumAllowedAdjacentIce[wn] then
				return;
			end
		end
		if self.AdjacentTilesAvoidAtoll[wn] then
			if iNumAtoll > self.MaximumAllowedAdjacentAtoll[wn] then
				return;
			end
		end
	end

	-- This plot has survived all tests and is eligible to host this wonder type.
	local iW, _ = Map.GetGridSize();
	local plotIndex = y * iW + x + 1;
	table.insert(self.eligibility_lists[wn], plotIndex);
end
------------------------------------------------------------------------------
function AssignStartingPlots:GenerateLocalVersionsOfDataFromXML()
	for _, rn in ipairs(self.xml_row_numbers) do
		table.insert(self.EligibilityMethodNumber, GameInfo.Natural_Wonder_Placement[rn].EligibilityMethodNumber);
		table.insert(self.OccurrenceFrequency, GameInfo.Natural_Wonder_Placement[rn].OccurrenceFrequency);
		table.insert(self.RequireBiggestLandmass, GameInfo.Natural_Wonder_Placement[rn].RequireBiggestLandmass);
		table.insert(self.AvoidBiggestLandmass, GameInfo.Natural_Wonder_Placement[rn].AvoidBiggestLandmass);
		table.insert(self.RequireFreshWater, GameInfo.Natural_Wonder_Placement[rn].RequireFreshWater);
		table.insert(self.AvoidFreshWater, GameInfo.Natural_Wonder_Placement[rn].AvoidFreshWater);
		table.insert(self.LandBased, GameInfo.Natural_Wonder_Placement[rn].LandBased);
		table.insert(self.RequireLandAdjacentToOcean, GameInfo.Natural_Wonder_Placement[rn].RequireLandAdjacentToOcean);
		table.insert(self.AvoidLandAdjacentToOcean, GameInfo.Natural_Wonder_Placement[rn].AvoidLandAdjacentToOcean);
		table.insert(self.RequireLandOnePlotInland, GameInfo.Natural_Wonder_Placement[rn].RequireLandOnePlotInland);
		table.insert(self.AvoidLandOnePlotInland, GameInfo.Natural_Wonder_Placement[rn].AvoidLandOnePlotInland);
		table.insert(self.RequireLandTwoOrMorePlotsInland, GameInfo.Natural_Wonder_Placement[rn].RequireLandTwoOrMorePlotsInland);
		table.insert(self.AvoidLandTwoOrMorePlotsInland, GameInfo.Natural_Wonder_Placement[rn].AvoidLandTwoOrMorePlotsInland);

		table.insert(self.CoreTileCanBeAnyPlotType, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeAnyPlotType);
		table.insert(self.CoreTileCanBeFlatland, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeFlatland);
		table.insert(self.CoreTileCanBeHills, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeHills);
		table.insert(self.CoreTileCanBeMountain, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeMountain);
		table.insert(self.CoreTileCanBeOcean, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeOcean);
		table.insert(self.CoreTileCanBeAnyTerrainType, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeAnyTerrainType);
		table.insert(self.CoreTileCanBeGrass, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeGrass);
		table.insert(self.CoreTileCanBePlains, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBePlains);
		table.insert(self.CoreTileCanBeDesert, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeDesert);
		table.insert(self.CoreTileCanBeTundra, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeTundra);
		table.insert(self.CoreTileCanBeSnow, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeSnow);
		table.insert(self.CoreTileCanBeShallowWater, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeShallowWater);
		table.insert(self.CoreTileCanBeDeepWater, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeDeepWater);
		table.insert(self.CoreTileCanBeAnyFeatureType, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeAnyFeatureType);
		table.insert(self.CoreTileCanBeNoFeature, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeNoFeature);
		table.insert(self.CoreTileCanBeForest, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeForest);
		table.insert(self.CoreTileCanBeJungle, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeJungle);
		table.insert(self.CoreTileCanBeOasis, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeOasis);
		table.insert(self.CoreTileCanBeFloodPlains, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeFloodPlains);
		table.insert(self.CoreTileCanBeMarsh, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeMarsh);
		table.insert(self.CoreTileCanBeIce, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeIce);
		table.insert(self.CoreTileCanBeAtoll, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeAtoll);

		table.insert(self.AdjacentTilesCareAboutPlotTypes, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesCareAboutPlotTypes);
		table.insert(self.AdjacentTilesAvoidAnyland, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidAnyland);
		table.insert(self.AdjacentTilesRequireFlatland, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireFlatland);
		table.insert(self.RequiredNumberOfAdjacentFlatland, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentFlatland);
		table.insert(self.AdjacentTilesRequireHills, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireHills);
		table.insert(self.RequiredNumberOfAdjacentHills, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentHills);
		table.insert(self.AdjacentTilesRequireMountain, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireMountain);
		table.insert(self.RequiredNumberOfAdjacentMountain, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentMountain);
		table.insert(self.AdjacentTilesRequireHillsPlusMountains, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireHillsPlusMountains);
		table.insert(self.RequiredNumberOfAdjacentHillsPlusMountains, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentHillsPlusMountains);
		table.insert(self.AdjacentTilesRequireOcean, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireOcean);
		table.insert(self.RequiredNumberOfAdjacentOcean, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentOcean);
		table.insert(self.AdjacentTilesAvoidFlatland, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidFlatland);
		table.insert(self.MaximumAllowedAdjacentFlatland, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentFlatland);
		table.insert(self.AdjacentTilesAvoidHills, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidHills);
		table.insert(self.MaximumAllowedAdjacentHills, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentHills);
		table.insert(self.AdjacentTilesAvoidMountain, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidMountain);
		table.insert(self.MaximumAllowedAdjacentMountain, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentMountain);
		table.insert(self.AdjacentTilesAvoidHillsPlusMountains, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidHillsPlusMountains);
		table.insert(self.MaximumAllowedAdjacentHillsPlusMountains, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentHillsPlusMountains);
		table.insert(self.AdjacentTilesAvoidOcean, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidOcean);
		table.insert(self.MaximumAllowedAdjacentOcean, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentOcean);

		table.insert(self.AdjacentTilesCareAboutTerrainTypes, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesCareAboutTerrainTypes);
		table.insert(self.AdjacentTilesRequireGrass, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireGrass);
		table.insert(self.RequiredNumberOfAdjacentGrass, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentGrass);
		table.insert(self.AdjacentTilesRequirePlains, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequirePlains);
		table.insert(self.RequiredNumberOfAdjacentPlains, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentPlains);
		table.insert(self.AdjacentTilesRequireDesert, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireDesert);
		table.insert(self.RequiredNumberOfAdjacentDesert, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentDesert);
		table.insert(self.AdjacentTilesRequireTundra, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireTundra);
		table.insert(self.RequiredNumberOfAdjacentTundra, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentTundra);
		table.insert(self.AdjacentTilesRequireSnow, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireSnow);
		table.insert(self.RequiredNumberOfAdjacentSnow, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentSnow);
		table.insert(self.AdjacentTilesRequireShallowWater, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireShallowWater);
		table.insert(self.RequiredNumberOfAdjacentShallowWater, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentShallowWater);
		table.insert(self.AdjacentTilesRequireDeepWater, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireDeepWater);
		table.insert(self.RequiredNumberOfAdjacentDeepWater, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentDeepWater);
		table.insert(self.AdjacentTilesAvoidGrass, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidGrass);
		table.insert(self.MaximumAllowedAdjacentGrass, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentGrass);
		table.insert(self.AdjacentTilesAvoidPlains, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidPlains);
		table.insert(self.MaximumAllowedAdjacentPlains, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentPlains);
		table.insert(self.AdjacentTilesAvoidDesert, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidDesert);
		table.insert(self.MaximumAllowedAdjacentDesert, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentDesert);
		table.insert(self.AdjacentTilesAvoidTundra, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidTundra);
		table.insert(self.MaximumAllowedAdjacentTundra, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentTundra);
		table.insert(self.AdjacentTilesAvoidSnow, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidSnow);
		table.insert(self.MaximumAllowedAdjacentSnow, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentSnow);
		table.insert(self.AdjacentTilesAvoidShallowWater, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidShallowWater);
		table.insert(self.MaximumAllowedAdjacentShallowWater, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentShallowWater);
		table.insert(self.AdjacentTilesAvoidDeepWater, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidDeepWater);
		table.insert(self.MaximumAllowedAdjacentDeepWater, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentDeepWater);

		table.insert(self.AdjacentTilesCareAboutFeatureTypes, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesCareAboutFeatureTypes);
		table.insert(self.AdjacentTilesRequireNoFeature, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireNoFeature);
		table.insert(self.RequiredNumberOfAdjacentNoFeature, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentNoFeature);
		table.insert(self.AdjacentTilesRequireForest, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireForest);
		table.insert(self.RequiredNumberOfAdjacentForest, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentForest);
		table.insert(self.AdjacentTilesRequireJungle, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireJungle);
		table.insert(self.RequiredNumberOfAdjacentJungle, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentJungle);
		table.insert(self.AdjacentTilesRequireOasis, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireOasis);
		table.insert(self.RequiredNumberOfAdjacentOasis, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentOasis);
		table.insert(self.AdjacentTilesRequireFloodPlains, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireFloodPlains);
		table.insert(self.RequiredNumberOfAdjacentFloodPlains, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentFloodPlains);
		table.insert(self.AdjacentTilesRequireMarsh, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireMarsh);
		table.insert(self.RequiredNumberOfAdjacentMarsh, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentMarsh);
		table.insert(self.AdjacentTilesRequireIce, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireIce);
		table.insert(self.RequiredNumberOfAdjacentIce, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentIce);
		table.insert(self.AdjacentTilesRequireAtoll, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireAtoll);
		table.insert(self.RequiredNumberOfAdjacentAtoll, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentAtoll);
		table.insert(self.AdjacentTilesAvoidNoFeature, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidNoFeature);
		table.insert(self.MaximumAllowedAdjacentNoFeature, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentNoFeature);
		table.insert(self.AdjacentTilesAvoidForest, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidForest);
		table.insert(self.MaximumAllowedAdjacentForest, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentForest);
		table.insert(self.AdjacentTilesAvoidJungle, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidJungle);
		table.insert(self.MaximumAllowedAdjacentJungle, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentJungle);
		table.insert(self.AdjacentTilesAvoidOasis, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidOasis);
		table.insert(self.MaximumAllowedAdjacentOasis, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentOasis);
		table.insert(self.AdjacentTilesAvoidFloodPlains, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidFloodPlains);
		table.insert(self.MaximumAllowedAdjacentFloodPlains, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentFloodPlains);
		table.insert(self.AdjacentTilesAvoidMarsh, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidMarsh);
		table.insert(self.MaximumAllowedAdjacentMarsh, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentMarsh);
		table.insert(self.AdjacentTilesAvoidIce, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidIce);
		table.insert(self.MaximumAllowedAdjacentIce, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentIce);
		table.insert(self.AdjacentTilesAvoidAtoll, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidAtoll);
		table.insert(self.MaximumAllowedAdjacentAtoll, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentAtoll);

		table.insert(self.TileChangesMethodNumber, GameInfo.Natural_Wonder_Placement[rn].TileChangesMethodNumber);
		table.insert(self.ChangeCoreTileToMountain, GameInfo.Natural_Wonder_Placement[rn].ChangeCoreTileToMountain);
		table.insert(self.ChangeCoreTileToFlatland, GameInfo.Natural_Wonder_Placement[rn].ChangeCoreTileToFlatland);
		table.insert(self.ChangeCoreTileTerrainToGrass, GameInfo.Natural_Wonder_Placement[rn].ChangeCoreTileTerrainToGrass);
		table.insert(self.ChangeCoreTileTerrainToPlains, GameInfo.Natural_Wonder_Placement[rn].ChangeCoreTileTerrainToPlains);
		table.insert(self.SetAdjacentTilesToShallowWater, GameInfo.Natural_Wonder_Placement[rn].SetAdjacentTilesToShallowWater);
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:GenerateCandidatePlotListsForSpecificNW()
	-- Custom mapscripts can override this function to skip Natural Wonders that are not meant to be placed.
	-- By default, all Natural Wonders are checked.
	local iW, iH = Map.GetGridSize();
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local landEligibility, seaEligibility = self:ExaminePlotForNaturalWondersEligibility(x, y);
			if seaEligibility then
				-- Plot has passed checks applicable to all NW types. Move on to specific checks.
				for nw_number, _ in ipairs(self.xml_row_numbers) do
					if self.wonder_list[nw_number] == "FEATURE_REEF" or
						self.wonder_list[nw_number] == "FEATURE_GIBRALTAR" or
						self.wonder_list[nw_number] == "FEATURE_VOLCANO" or
						landEligibility then
						self:CanBeThisNaturalWonderType(x, y, nw_number);
					end
				end
			end
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:GenerateNaturalWondersCandidatePlotLists(target_number)
	-- This function scans the map for eligible sites for all "Natural Wonders" Features.

	local iW, iH = Map.GetGridSize();

	-- Set up Landmass check for wonders that avoid the biggest landmass when the world has oceans.
	self.iBiggestLandmassID = Map.FindBiggestLandmassID(false);
	local biggest_ocean = Map.FindBiggestLandmassID(true);
	local iNumBiggestOceanPlots = 0;
	if biggest_ocean then
		iNumBiggestOceanPlots = Map.GetNumTilesOfLandmass(biggest_ocean);
	end
	if iNumBiggestOceanPlots > (iW * iH) / 4 then
		self.bWorldHasOceans = true;
	else
		self.bWorldHasOceans = false;
	end

	-- Read the XML data. Count the number of wonders.
	local iNumNW = 0;
	for _ in GameInfo.Natural_Wonder_Placement() do
		iNumNW = iNumNW + 1;
	end

	if iNumNW == 0 then
		print("-"); print("*** No Natural Wonders found in Civ5Features.xml! ***"); print("-");
		return {};
	end

	-- Set up NW IDs.
	self.wonder_list = table.fill(-1, iNumNW);
	local next_wonder_number = 1;
	for row in GameInfo.Features() do
		if row.NaturalWonder or row.PseudoNaturalWonder == 1 then
			self.wonder_list[next_wonder_number] = row.Type;
			next_wonder_number = next_wonder_number + 1;
		end
	end

	-- Set up Eligibility Lists.
	for _ = 1, iNumNW do
		table.insert(self.eligibility_lists, {});
	end

	-- Set up Row Numbers.
	for _, nw_type in ipairs(self.wonder_list) do
		-- Obtain the correct Row number from the xml Placement table.
		local row_number;
		for row in GameInfo.Natural_Wonder_Placement() do
			if row.NaturalWonderType == nw_type then
				row_number = row.ID;
			end
		end
		table.insert(self.xml_row_numbers, row_number);
	end

	-- Load Data from XML.
	self:GenerateLocalVersionsOfDataFromXML();

	-- Extracted from this function for easy override since some maps may only place selected NW.
	self:GenerateCandidatePlotListsForSpecificNW();

	-- Eligibility will affect which NWs can be used, and number of candidates will affect placement order.
	local iCanBeWonder = {};
	for loop = 1, iNumNW do
		table.insert(iCanBeWonder, table.maxn(self.eligibility_lists[loop]));
		print("Wonder #", loop, "has", iCanBeWonder[loop], "candidate plots.");
	end

	-- Randomly pick NWs from the list based on occurrence
	print("-");
	print("List of OccurrenceFrequency values for each Natural Wonder in the database:");
	local NW_frequency = {};
	local isNWPicked = {};
	local chosen_NW, fallback_NW = {}, {};
	for loop = 1, iNumNW do
		table.insert(NW_frequency, GameInfo.Natural_Wonder_Placement[self.xml_row_numbers[loop]].OccurrenceFrequency);
		table.insert(isNWPicked, false);
		print("Occurrence for: #", loop, " ", self.wonder_list[loop], "is:", GameInfo.Natural_Wonder_Placement[self.xml_row_numbers[loop]].OccurrenceFrequency);
	end

	-- print("-");
	-- print("List of picked Natural Wonders and their chances:");
	for _ = 1, target_number do
		local occurrence_threshold = {};
		local iOccurrenceSum = 0;

		-- Populate threshold table
		for loop = 1, iNumNW do
			local iFrequency = 0;
			if not isNWPicked[loop] and iCanBeWonder[loop] > 0 then
				iFrequency = GameInfo.Natural_Wonder_Placement[self.xml_row_numbers[loop]].OccurrenceFrequency;
			end
			iOccurrenceSum = iOccurrenceSum + iFrequency;
			table.insert(occurrence_threshold, iOccurrenceSum);
			-- print(self.wonder_list[loop], iOccurrenceSum);
		end

		-- End early if no NW left to pick
		if iOccurrenceSum == 0 then
			break;
		end

		-- Pick the NW
		local diceroll = Map.Rand(iOccurrenceSum, "Picking NW");
		for loop = 1, iNumNW do
			if diceroll < occurrence_threshold[loop] then
				-- print("Picked:", self.wonder_list[loop], "diceroll = ", diceroll);
				isNWPicked[loop] = true;
				break;
			end
		end
	end

	print("-");
	print("List of chosen and fallback NWs:");
	for loop = 1, iNumNW do
		if isNWPicked[loop] then
			table.insert(chosen_NW, loop);
			-- print("Chosen:", self.wonder_list[loop]);
		elseif iCanBeWonder[loop] > 0 and NW_frequency[loop] > 0 then
			table.insert(fallback_NW, loop);
			-- print("Fallback:", self.wonder_list[loop]);
		end
	end

	-- Sort the chosen NWs with fewest candidates first
	table.sort(chosen_NW, function (a, b) return iCanBeWonder[a] < iCanBeWonder[b] end);

	-- Shuffle the fallback NWs
	local shuffled_fallback_NW = GetShuffledCopyOfTable(fallback_NW);

	local NW_final_selections = {};
	for loop = 1, #chosen_NW do
		table.insert(NW_final_selections, chosen_NW[loop]);
	end
	for loop = 1, #shuffled_fallback_NW do
		table.insert(NW_final_selections, shuffled_fallback_NW[loop]);
	end

	return NW_final_selections;
end
------------------------------------------------------------------------------
function AssignStartingPlots:AttemptToPlaceNaturalWonder(wonder_number, row_number)
	-- Attempts to place a specific natural wonder. The "wonder_number" is a Lua index while "row_number" is an XML index.
	local iW, _ = Map.GetGridSize();
	local feature_type_to_place;
	for thisFeature in GameInfo.Features() do
		if thisFeature.Type == self.wonder_list[wonder_number] then
			feature_type_to_place = thisFeature.ID;
			break;
		end
	end
	local temp_table = self.eligibility_lists[wonder_number];
	local candidate_plot_list = GetShuffledCopyOfTable(temp_table);
	for _, plotIndex in ipairs(candidate_plot_list) do
		if self.impactData[ImpactLayers.LAYER_NATURAL_WONDER][plotIndex] == 0 then -- No collision with civ start or other NW, so place wonder here!
			local x = (plotIndex - 1) % iW;
			local y = (plotIndex - x - 1) / iW;
			local plot = Map.GetPlot(x, y);
			-- If called for, force the local terrain to conform to what the wonder needs.
			local method_number = GameInfo.Natural_Wonder_Placement[row_number].TileChangesMethodNumber;
			if method_number ~= -1 then
				-- Custom method for tile changes needed by this wonder.
				NWCustomPlacement(x, y, row_number, method_number);
			else
				-- Check the XML data for any standard type tile changes, execute any that are indicated.
				if GameInfo.Natural_Wonder_Placement[row_number].ChangeCoreTileToMountain then
					if not plot:IsMountain() then
						plot:SetPlotType(PlotTypes.PLOT_MOUNTAIN, false, false);
					end
				elseif GameInfo.Natural_Wonder_Placement[row_number].ChangeCoreTileToFlatland then
					if plot:GetPlotType() ~= PlotTypes.PLOT_LAND then
						plot:SetPlotType(PlotTypes.PLOT_LAND, false, false);
					end
				end
				if GameInfo.Natural_Wonder_Placement[row_number].ChangeCoreTileTerrainToGrass then
					if plot:GetTerrainType() ~= TerrainTypes.TERRAIN_GRASS then
						plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, false);
					end
				elseif GameInfo.Natural_Wonder_Placement[row_number].ChangeCoreTileTerrainToPlains then
					if plot:GetTerrainType() ~= TerrainTypes.TERRAIN_PLAINS then
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, false);
					end
				end
				if GameInfo.Natural_Wonder_Placement[row_number].SetAdjacentTilesToShallowWater then
					for _, direction in ipairs(self.direction_types) do
						local adjPlot = Map.PlotDirection(x, y, direction);
						if adjPlot:GetTerrainType() ~= TerrainTypes.TERRAIN_COAST then
							adjPlot:SetTerrainType(TerrainTypes.TERRAIN_COAST, false, false);
						end
					end
				end
			end

			-- Now place this wonder and record the placement.
			plot:SetFeatureType(feature_type_to_place);

			-- Natural Wonders shouldn't be too close to each other.
			self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_NATURAL_WONDER, 11);

			-- Force no resource on Natural Wonders.
			self:PlaceStrategicResourceImpact(x, y, 0);
			self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_LUXURY, 0);
			self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_BONUS, 0);
			self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_FISH, 0);
			self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_MARBLE, 0);
			self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_IVORY, 0);

			-- Keep City States away from Natural Wonders!
			self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_CITY_STATE, 4);

			local plotIndex = y * iW + x + 1;
			self.playerCollisionData[plotIndex] = true; -- Record exact plot of wonder in the collision list.

			-- MOD.Barathor: Fixed: Added a check for the Great Barrier Reef being placed. If so, it appropriately applies impact values to its second tile to avoid buggy collisions with water resources.
			-- MOD.Barathor: Start
			if self.wonder_list[wonder_number] == "FEATURE_REEF" then
				-- print("Great Barrier Reef placed... applying impact values to its southeast tile as well.");
				local SEPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_SOUTHEAST);
				local southeastX = SEPlot:GetX();
				local southeastY = SEPlot:GetY();
				self:PlaceStrategicResourceImpact(southeastX, southeastY, 0);
				self:PlaceResourceImpact(southeastX, southeastY, ImpactLayers.LAYER_LUXURY, 0);
				self:PlaceResourceImpact(southeastX, southeastY, ImpactLayers.LAYER_BONUS, 0);
				self:PlaceResourceImpact(southeastX, southeastY, ImpactLayers.LAYER_FISH, 0);
				self:PlaceResourceImpact(southeastX, southeastY, ImpactLayers.LAYER_MARBLE, 0);
				self:PlaceResourceImpact(southeastX, southeastY, ImpactLayers.LAYER_IVORY, 0);
				local SEplotIndex = southeastY * iW + southeastX + 1;
				self.playerCollisionData[SEplotIndex] = true; -- Record exact plot of wonder in the collision list.
			end
			-- MOD.Barathor: End

			print("- Placed ".. self.wonder_list[wonder_number].. " in Plot", x, y);

			return true;
		end
	end
	-- If reached here, this wonder was unable to be placed because all candidates are too close to an already-placed NW.
	return false;
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetNumNaturalWondersToPlace(iWorldSize)
	local tTarget = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 2,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 6,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 7,
	};
	return tTarget[iWorldSize];
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceNaturalWonders()
	-- Determine how many NWs to attempt to place. Target is regulated per map size.
	-- The final number cannot exceed the number the map has locations to support.
	local target_number = self:GetNumNaturalWondersToPlace(Map.GetWorldSize());

	local NW_eligibility_order = self:GenerateNaturalWondersCandidatePlotLists(target_number);
	local iNumNWCandidates = table.maxn(NW_eligibility_order);
	if iNumNWCandidates == 0 then
		print("No Natural Wonders placed, no eligible sites found for any of them.");
		return;
	end

	-- Debug printout
	--[[
	print("-"); print("--- Readout of NW Assignment Priority ---");
	for print_loop, order in ipairs(NW_eligibility_order) do
		print("NW Assignment Priority#", print_loop, "goes to NW#", order);
	end
	print("-"); print("-");
	--]]

	local iNumNWtoPlace = math.min(target_number, iNumNWCandidates);
	local selected_NWs, fallback_NWs = {}, {};
	for loop, NW in ipairs(NW_eligibility_order) do
		if loop <= iNumNWtoPlace then
			table.insert(selected_NWs, NW);
		else
			table.insert(fallback_NWs, NW);
		end
	end
	-- print("-");
	for _, NW in ipairs(selected_NWs) do
		print("Natural Wonder #", NW, "has been selected for placement.");
	end
	print("-");
	for _, NW in ipairs(fallback_NWs) do
		print("Natural Wonder #", NW, "has been selected as fallback.");
	end
	print("-");

	print("--- Placing Natural Wonders! ---");
	-- Place the NWs
	local iNumPlaced = 0;
	for _, nw_number in ipairs(selected_NWs) do
		local nw_type = self.wonder_list[nw_number];
		-- Obtain the correct Row number from the xml Placement table.
		local row_number;
		for row in GameInfo.Natural_Wonder_Placement() do
			if row.NaturalWonderType == nw_type then
				row_number = row.ID;
			end
		end
		-- Place the wonder, using the correct row data from XML.
		local bSuccess = self:AttemptToPlaceNaturalWonder(nw_number, row_number);
		if bSuccess then
			iNumPlaced = iNumPlaced + 1;
		end
	end
	if iNumPlaced < iNumNWtoPlace then
		for _, nw_number in ipairs(fallback_NWs) do
			if iNumPlaced >= iNumNWtoPlace then
				break;
			end
			local nw_type = self.wonder_list[nw_number];
			-- Obtain the correct Row number from the xml Placement table.
			local row_number;
			for row in GameInfo.Natural_Wonder_Placement() do
				if row.NaturalWonderType == nw_type then
					row_number = row.ID;
				end
			end
			-- Place the wonder, using the correct row data from XML.
			local bSuccess = self:AttemptToPlaceNaturalWonder(nw_number, row_number);
			if bSuccess then
				iNumPlaced = iNumPlaced + 1;
			end
		end
	end

	if iNumPlaced >= iNumNWtoPlace then
		print("-- Placed all Natural Wonders --"); print("-"); print("-");
	else
		print("-- Not all Natural Wonders targeted got placed --"); print("-"); print("-");
	end
end
------------------------------------------------------------------------------
-- Start of functions tied to PlaceCityStates()
------------------------------------------------------------------------------
function AssignStartingPlots:GetNumCityStatesPerRegion()
	-- Extracted from AssignCityStatesToRegionsOrToUninhabited() for easy override
	local ratio = self.iNumCityStates / self.iNumCivs;
	if ratio > 14 then -- This is a ridiculous number of city states for a game with two civs, but we'll account for it anyway.
		return 10;
	elseif ratio > 11 then -- This is a ridiculous number of cs for two or three civs.
		return 8;
	elseif ratio > 8 then
		return 6;
	elseif ratio > 5.7 then
		return 4;
	elseif ratio > 4.35 then
		return 3;
	elseif ratio > 2.7 then
		return 2;
	elseif ratio > 1.35 then
		return 1;
	else
		return 0;
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetNumCityStatesInUninhabitedRegion(iNumCivLandmassPlots, iNumUninhabitedLandmassPlots)
	-- Extracted from AssignCityStatesToRegionsOrToUninhabited() for easy override
	local uninhabited_ratio = iNumUninhabitedLandmassPlots / (iNumCivLandmassPlots + iNumUninhabitedLandmassPlots);
	local max_by_ratio = math.floor(3 * uninhabited_ratio * self.iNumCityStates);
	local max_by_method;
	if self.method == RegionDivision.BIGGEST_LANDMASS then
		max_by_method = math.ceil(self.iNumCityStates / 4);
	else
		max_by_method = math.ceil(self.iNumCityStates / 2);
	end
	return math.min(self.iNumCityStatesUnassigned, max_by_ratio, max_by_method);
end
------------------------------------------------------------------------------
function AssignStartingPlots:AssignCityStatesToRegions(current_cs_index)
	-- Extracted from AssignCityStatesToRegionsOrToUninhabited() for easy override
	-- Assign the "Per Region" City States to their regions.
	-- print("- - - - - - - - - - - - - - - - -"); print("Assigning City States to Regions");
	local iNumCityStatesPerRegion = self:GetNumCityStatesPerRegion();
	if iNumCityStatesPerRegion > 0 then
		for current_region = 1, self.iNumCivs do
			for _ = 1, iNumCityStatesPerRegion do
				self.city_state_region_assignments[current_cs_index] = current_region;
				-- print("-"); print("City State", current_cs_index, "assigned to Region#", current_region);
				current_cs_index = current_cs_index + 1;
				self.iNumCityStatesUnassigned = self.iNumCityStatesUnassigned - 1;
			end
		end
	end
	return current_cs_index;
end
------------------------------------------------------------------------------
function AssignStartingPlots:AssignCityStatesToRegionsOrToUninhabited()
	-- Placement methods include:
	-- 1. Assign n Per Region
	-- 2. Assign to uninhabited landmasses
	-- 3. Assign to regions with shared luxury IDs
	-- 4. Assign to low fertility regions

	-- Determine number to assign Per Region
	local iW, iH = Map.GetGridSize();
	local current_cs_index = self:AssignCityStatesToRegions(1);

	-- Determine how many City States to place on uninhabited landmasses.
	-- Also generate lists of candidate plots from uninhabited areas.
	local iNumLandAreas = 0;
	local iNumCivLandmassPlots = 0;
	local iNumUninhabitedLandmassPlots = 0;
	local iNumCityStatesUninhabited = 0;
	local land_area_IDs = {};
	local land_area_plot_count = {};
	local land_area_plot_tables = {};
	local areas_inhabited_by_civs = {};
	local areas_too_small = {};
	local areas_uninhabited = {};

	if self.method == RegionDivision.RECTANGULAR then -- Rectangular regional division spanning the entire globe, ALL plots belong to inhabited regions.
		-- print("Rectangular regional division spanning the whole world: all city states must belong to a region!");
	else -- Possibility of plots that do not belong to any civ's Region. Evaluate these plots and assign an appropriate number of City States to them.
		-- Generate list of inhabited area IDs.
		if self.method == RegionDivision.BIGGEST_LANDMASS or self.method == RegionDivision.CONTINENTAL then
			for _, region_data in ipairs(self.regionData) do
				local region_areaID = region_data[5];
				if region_areaID ~= -1 then -- -1 means specially-defined region; no area ID is -1 so it can be safely omitted
					if self.bArea then -- continental division is area-based
						if not TestMembership(areas_inhabited_by_civs, region_areaID) then
							table.insert(areas_inhabited_by_civs, region_areaID);
						end
					else -- biggest landmass division is landmass-based; each may have multiple areas
						for _, areaID in ipairs(self.landmass_areas[region_areaID]) do
							if not TestMembership(areas_inhabited_by_civs, areaID) then
								table.insert(areas_inhabited_by_civs, areaID);
							end
						end
					end
				end
			end
		end

		-- Iterate through plots and, for each land area, generate a list of all its member plots
		for x = 0, iW - 1 do
			for y = 0, iH - 1 do
				local plotIndex = y * iW + x + 1;
				local plot = Map.GetPlot(x, y);
				local plotType = plot:GetPlotType();
				local iArea = plot:GetArea();
				if not self.bArea then
					iArea = plot:GetLandmass();
				end
				if (plotType == PlotTypes.PLOT_LAND or plotType == PlotTypes.PLOT_HILLS) and self:CanPlaceCityStateAt(x, y, iArea, false, false) then -- Habitable land plot, process it.
					if self.method == RegionDivision.RECTANGULAR_SELF_DEFINED then -- Determine if plot is inside or outside the regional rectangle
						if (x >= self.inhabited_WestX and x <= self.inhabited_WestX + self.inhabited_Width - 1) and
						   (y >= self.inhabited_SouthY and y <= self.inhabited_SouthY + self.inhabited_Height - 1) then -- Civ-inhabited rectangle
							iNumCivLandmassPlots = iNumCivLandmassPlots + 1;
						else
							iNumUninhabitedLandmassPlots = iNumUninhabitedLandmassPlots + 1;
							if self.plotDataIsCoastal[plotIndex] then
								table.insert(self.uninhabited_areas_coastal_plots, plotIndex);
							else
								table.insert(self.uninhabited_areas_inland_plots, plotIndex);
							end
						end
					else -- AreaID/LandmassID-based method must be applied, which cannot all be done in this loop
						if not TestMembership(land_area_IDs, iArea) then -- This plot is the first detected in its area/landmass.
							iNumLandAreas = iNumLandAreas + 1;
							table.insert(land_area_IDs, iArea);
							land_area_plot_count[iArea] = 1;
							land_area_plot_tables[iArea] = {plotIndex};
						else -- This area/landmass already known.
							land_area_plot_count[iArea] = land_area_plot_count[iArea] + 1;
							table.insert(land_area_plot_tables[iArea], plotIndex);
						end
					end
				end
			end
		end

		-- Complete the AreaID/LandmassID-based method.
		if self.method == RegionDivision.BIGGEST_LANDMASS or self.method == RegionDivision.CONTINENTAL then
			-- Obtain counts of inhabited and uninhabited plots. Identify areas too small to use for City States.
			for areaID, plot_count in pairs(land_area_plot_count) do
				if TestMembership(areas_inhabited_by_civs, areaID) then
					iNumCivLandmassPlots = iNumCivLandmassPlots + plot_count;
				else
					iNumUninhabitedLandmassPlots = iNumUninhabitedLandmassPlots + plot_count;
					if plot_count < 2 then
						table.insert(areas_too_small, areaID);
					else
						table.insert(areas_uninhabited, areaID);
					end
				end
			end
			-- Now loop through all Uninhabited Areas that are large enough to use and append their plots to the candidates tables.
			for areaID, area_plot_list in pairs(land_area_plot_tables) do
				if TestMembership(areas_uninhabited, areaID) then
					for _, plotIndex in ipairs(area_plot_list) do
						local x = (plotIndex - 1) % iW;
						local y = (plotIndex - x - 1) / iW;
						if self:CanPlaceCityStateAt(x, y, areaID, false, false) then
							if self.plotDataIsCoastal[plotIndex] then
								table.insert(self.uninhabited_areas_coastal_plots, plotIndex);
							else
								table.insert(self.uninhabited_areas_inland_plots, plotIndex);
							end
						end
					end
				end
			end
		end

		-- Determine the number of City States to assign to uninhabited areas.
		iNumCityStatesUninhabited = self:GetNumCityStatesInUninhabitedRegion(iNumCivLandmassPlots, iNumUninhabitedLandmassPlots);
		self.iNumCityStatesUnassigned = self.iNumCityStatesUnassigned - iNumCityStatesUninhabited;
	end
	-- print("-"); print("City States assigned to Uninhabited Areas: ", iNumCityStatesUninhabited);
	-- Update the city state number.
	current_cs_index = current_cs_index + iNumCityStatesUninhabited;

	if self.iNumCityStatesUnassigned > 0 then
		-- Determine how many to place in support of regions that share their luxury type with two other regions.
		local iNumRegionsSharedLux = 0;
		local iNumCityStatesSharedLux = 0;
		local shared_lux_IDs = {};
		for resource_ID, amount_assigned_to_regions in ipairs(self.luxury_assignment_count) do
			if amount_assigned_to_regions == 3 then
				iNumRegionsSharedLux = iNumRegionsSharedLux + 3;
				table.insert(shared_lux_IDs, resource_ID);
			end
		end
		local iNumCityStatesLowFertility = self.iNumCityStatesUnassigned;
		if iNumRegionsSharedLux > 0 and iNumRegionsSharedLux <= self.iNumCityStatesUnassigned then
			iNumCityStatesSharedLux = iNumRegionsSharedLux;
			iNumCityStatesLowFertility = iNumCityStatesLowFertility - iNumCityStatesSharedLux;
		end
		-- print("CS Shared Lux: ", iNumCityStatesSharedLux, " CS Low Fert: ", iNumCityStatesLowFertility);
		-- Assign remaining types to their respective regions.
		if iNumCityStatesSharedLux > 0 then
			for _, res_ID in ipairs(shared_lux_IDs) do
				for _, region_lux_data in ipairs(self.regions_sorted_by_type) do
					local this_region_res = region_lux_data[2];
					if this_region_res == res_ID then
						self.city_state_region_assignments[current_cs_index] = region_lux_data[1];
						-- print("-"); print("City State", current_cs_index, "assigned to Region#", region_lux_data[1], " to compensate for Shared Luxury ID#", res_ID);
						current_cs_index = current_cs_index + 1;
						self.iNumCityStatesUnassigned = self.iNumCityStatesUnassigned - 1;
					end
				end
			end
		end
		if iNumCityStatesLowFertility > 0 then
			-- If more to assign than number of regions, assign per region.
			while self.iNumCityStatesUnassigned >= self.iNumCivs do
				for current_region = 1, self.iNumCivs do
					self.city_state_region_assignments[current_cs_index] = current_region;
					-- print("-"); print("City State", current_cs_index, "assigned to Region#", current_region, " to compensate for Low Fertility");
					current_cs_index = current_cs_index + 1;
					self.iNumCityStatesUnassigned = self.iNumCityStatesUnassigned - 1;
				end
			end
			if self.iNumCityStatesUnassigned > 0 then
				local fert_unsorted, fert_sorted, region_list = {}, {}, {};
				for region_num = 1, self.iNumCivs do
					local area_plots = self.regionTerrainCounts[region_num][2];
					local region_fertility = self.regionData[region_num][6];
					local fertility_per_land_plot = region_fertility / area_plots;
					-- print("-"); print("Region#", region_num, "AreaPlots:", area_plots, "Region Fertility:", region_fertility, "Per Plot:", fertility_per_land_plot);

					table.insert(fert_unsorted, {region_num, fertility_per_land_plot});
					table.insert(fert_sorted, fertility_per_land_plot);
				end
				table.sort(fert_sorted);
				for _, fert_value in ipairs(fert_sorted) do
					for loop, data_pair in ipairs(fert_unsorted) do
						local this_region_fert = data_pair[2];
						if this_region_fert == fert_value then
							local regionNum = data_pair[1];
							table.insert(region_list, regionNum);
							table.remove(fert_unsorted, loop);
							break;
						end
					end
				end
				for loop = 1, self.iNumCityStatesUnassigned do
					self.city_state_region_assignments[current_cs_index] = region_list[loop];
					-- print("-"); print("City State", current_cs_index, "assigned to Region#", region_list[loop], " to compensate for Low Fertility");
					current_cs_index = current_cs_index + 1;
					self.iNumCityStatesUnassigned = self.iNumCityStatesUnassigned - 1;
				end
			end
		end
	end

	-- Debug check
	if self.iNumCityStatesUnassigned ~= 0 then
		print("Wrong number of City States assigned at end of assignment process. This number unassigned: ", self.iNumCityStatesUnassigned);
	else
		print("All city states assigned.");
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:CanPlaceCityStateAt(x, y, area_ID, force_it, ignore_collisions)
	local iW, iH = Map.GetGridSize();
	local plot = Map.GetPlot(x, y);
	local area = plot:GetArea();
	local landmass = plot:GetLandmass();
	if self.bArea and area ~= area_ID and area_ID ~= -1 then
		return false;
	end

	if not self.bArea and landmass ~= area_ID and area_ID ~= -1 then
		return false;
	end

	if plot:IsWater() or plot:IsMountain() then
		return false;
	end

	-- Must not be on map borders
	if (not Map:IsWrapX() and (x < 1 or x >= iW - 1)) or (not Map:IsWrapY() and (y < 1 or y >= iH - 1)) then
		return false;
	end

	-- Avoid ice
	for nearPlot in self:Plot_GetPlotsInCircle(plot, 1) do
		if nearPlot:GetFeatureType() == FeatureTypes.FEATURE_ICE then
			return false;
		end
	end

	-- Avoid natural wonders
	for nearPlot in self:Plot_GetPlotsInCircle(plot, 1, 4) do
		local featureInfo = GameInfo.Features[nearPlot:GetFeatureType()];
		if featureInfo and featureInfo.NaturalWonder then
			-- print("CanPlaceCityStateAt: avoided natural wonder: ", featureInfo.Type);
			return false;
		end
	end

	-- Reserve the best city sites for major civs
	local fertility = self:Plot_GetFertilityInRange(plot, 2);
	if fertility > 30 then
		-- print("CanPlaceCityStateAt: avoided fertility: ", fertility);
		return false;
	end

	local plotIndex = y * iW + x + 1;
	if self.impactData[ImpactLayers.LAYER_CITY_STATE][plotIndex] > 0 and not force_it then
		return false;
	end

	if self.playerCollisionData[plotIndex] and not ignore_collisions then
		print("-"); print("City State candidate plot rejected: collided with already-placed civ or City State at", x, y);
		return false;
	end
	return true;
end
------------------------------------------------------------------------------
function AssignStartingPlots:ObtainNextSectionInRegion(incoming_west_x, incoming_south_y, incoming_width, incoming_height, iAreaID, force_it, ignore_collisions)
	-- print("ObtainNextSectionInRegion called, for Area/Landmass#", iAreaID, "with SW plot at ", incoming_west_x, incoming_south_y, " Width/Height at", incoming_width, incoming_height);

	-- This function carves off the outermost plots in a region, checks them for City State Placement eligibility,
	-- and returns 7 variables: two plot lists, the coordinates of the inner portion of the landmass/area that was not processed on this round,
	-- and a boolean indicating whether the middle of the region was reached.

	-- If this round does not produce a suitable placement site, another round can be executed on the remaining unprocessed plots, recursively,
	-- until the middle of the region has been reached.
	-- If the entire region has no eligible plots, then it is likely that something extreme is going on with the map.
	-- Then choose a plot from the outermost portion of the region at random and hope for the best.

	local iW, iH = Map.GetGridSize();
	local reached_middle = false;
	if incoming_width <= 0 or incoming_height <= 0 then -- Nothing to process
		return {}, {}, -1, -1, -1, -1, true;
	end
	if incoming_width < 4 or incoming_height < 4 then
		reached_middle = true;
	end
	local bTaller = false;
	local rows_to_check = math.ceil(0.167 * incoming_width);
	if incoming_height > incoming_width then
		bTaller = true;
		rows_to_check = math.ceil(0.167 * incoming_height);
	end
	-- Main loop
	local coastal_plots, inland_plots = {}, {};
	for section_y = incoming_south_y, incoming_south_y + incoming_height - 1 do
		for section_x = incoming_west_x, incoming_west_x + incoming_width - 1 do
			if reached_middle then -- Process all plots.
				local x = section_x % iW;
				local y = section_y % iH;
				if self:CanPlaceCityStateAt(x, y, iAreaID, force_it, ignore_collisions) then
					local i = y * iW + x + 1;
					if self.plotDataIsCoastal[i] then
						table.insert(coastal_plots, i);
					else
						table.insert(inland_plots, i);
					end
				end
			else -- Process only plots near enough to the region edge.
				if not bTaller then -- Processing leftmost and rightmost columns.
					if section_x < incoming_west_x + rows_to_check or section_x >= incoming_west_x + incoming_width - rows_to_check then
						local x = section_x % iW;
						local y = section_y % iH;
						if self:CanPlaceCityStateAt(x, y, iAreaID, force_it, ignore_collisions) then
							local i = y * iW + x + 1;
							if self.plotDataIsCoastal[i] then
								table.insert(coastal_plots, i);
							else
								table.insert(inland_plots, i);
							end
						end
					end
				else -- Processing top and bottom rows.
					if section_y < incoming_south_y + rows_to_check or section_y >= incoming_south_y + incoming_height - rows_to_check then
						local x = section_x % iW;
						local y = section_y % iH;
						if self:CanPlaceCityStateAt(x, y, iAreaID, force_it, ignore_collisions) then
							local i = y * iW + x + 1;
							if self.plotDataIsCoastal[i] then
								table.insert(coastal_plots, i);
							else
								table.insert(inland_plots, i);
							end
						end
					end
				end
			end
		end
	end
	local new_west_x, new_south_y, new_width, new_height;
	if bTaller then
		new_west_x = incoming_west_x + rows_to_check;
		new_south_y = incoming_south_y;
		new_width = incoming_width - (2 * rows_to_check);
		new_height = incoming_height;
	else
		new_west_x = incoming_west_x;
		new_south_y = incoming_south_y + rows_to_check;
		new_width = incoming_width;
		new_height = incoming_height - (2 * rows_to_check);
	end

	return coastal_plots, inland_plots, new_west_x, new_south_y, new_width, new_height, reached_middle;
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceCityState(coastal_plot_list, inland_plot_list, check_proximity, check_collision)
	-- returns coords, plus boolean indicating whether assignment succeeded or failed.
	-- Argument "check_collision" should be false if plots in lists were already checked, true if not.
	if not coastal_plot_list or not inland_plot_list then
		print("Nil plot list incoming for PlaceCityState()");
	end
	local iW, _ = Map.GetGridSize();
	local iNumCoastal = table.maxn(coastal_plot_list);
	if iNumCoastal > 0 then
		if not check_collision then
			local diceroll = 1 + Map.Rand(iNumCoastal, "Standard City State placement - LUA");
			local selected_plot_index = coastal_plot_list[diceroll];
			local x = (selected_plot_index - 1) % iW;
			local y = (selected_plot_index - x - 1) / iW;
			return x, y, true;
		else
			local randomized_coastal = GetShuffledCopyOfTable(coastal_plot_list);
			for _, candidate_plot in ipairs(randomized_coastal) do
				if not self.playerCollisionData[candidate_plot] then
					if not check_proximity or self.impactData[ImpactLayers.LAYER_CITY_STATE][candidate_plot] == 0 then
						local x = (candidate_plot - 1) % iW;
						local y = (candidate_plot - x - 1) / iW;
						return x, y, true;
					end
				end
			end
		end
	end
	local iNumInland = table.maxn(inland_plot_list);
	if iNumInland > 0 then
		if not check_collision then
			local diceroll = 1 + Map.Rand(iNumInland, "Standard City State placement - LUA");
			local selected_plot_index = inland_plot_list[diceroll];
			local x = (selected_plot_index - 1) % iW;
			local y = (selected_plot_index - x - 1) / iW;
			return x, y, true;
		else
			local randomized_inland = GetShuffledCopyOfTable(inland_plot_list);
			for _, candidate_plot in ipairs(randomized_inland) do
				if not self.playerCollisionData[candidate_plot] then
					if not check_proximity or self.impactData[ImpactLayers.LAYER_CITY_STATE][candidate_plot] == 0 then
						local x = (candidate_plot - 1) % iW;
						local y = (candidate_plot - x - 1) / iW;
						return x, y, true;
					end
				end
			end
		end
	end
	return 0, 0, false;
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceCityStateInRegion(city_state_number, region_number)
	print("Place City State in Region called for City State", city_state_number, "Region", region_number);
	local iW, _ = Map.GetGridSize();
	local placed_city_state = false;
	local reached_middle = false;
	local region_data_table = self.regionData[region_number];
	local iWestX = region_data_table[1];
	local iSouthY = region_data_table[2];
	local iWidth = region_data_table[3];
	local iHeight = region_data_table[4];
	local iAreaID = region_data_table[5];

	local eligible_coastal, eligible_inland = {}, {};

	-- Main loop, first pass, unforced
	local x, y;
	local curWX = iWestX;
	local curSY = iSouthY;
	local curWid = iWidth;
	local curHei = iHeight;
	while not placed_city_state and not reached_middle do
		-- Send the remaining unprocessed portion of the region to be processed.
		local nextWX, nextSY, nextWid, nextHei;
		eligible_coastal, eligible_inland, nextWX, nextSY, nextWid, nextHei, reached_middle =
			self:ObtainNextSectionInRegion(curWX, curSY, curWid, curHei, iAreaID, false, false); -- Don't force it. Yet.
		curWX, curSY, curWid, curHei = nextWX, nextSY, nextWid, nextHei;
		-- Attempt to place city state using the two plot lists received from the last call.
		x, y, placed_city_state = self:PlaceCityState(eligible_coastal, eligible_inland, false, false); -- Don't need to re-check collisions.
	end

	if placed_city_state then
		-- Record and enact the placement.
		self.cityStatePlots[city_state_number] = {x, y, region_number};
		self.city_state_validity_table[city_state_number] = true; -- This is the line that marks a city state as valid to be processed by the rest of the system.
		local city_state_ID = city_state_number + MAX_MAJOR_CIVS - 1;
		local cityState = Players[city_state_ID];
		local cs_start_plot = Map.GetPlot(x, y);
		cityState:SetStartingPlot(cs_start_plot);
		self:GenerateLuxuryPlotListsAtCitySite(x, y, 1, true) -- Removes Feature Ice from coasts adjacent to the city state's new location
		self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_CITY_STATE, 4);
		self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_LUXURY, 3);
		self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_BONUS, 3);
		self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_FISH, 3);
		self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_MARBLE, 3);
		self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_IVORY, 3);
		-- Vanilla Communitas change - Militaristic CS gets more potential strategic resources
		if cityState:GetMinorCivTrait() == MinorCivTraitTypes.MINOR_CIV_TRAIT_MILITARISTIC then
			self:PlaceStrategicResourceImpact(x, y, 0);
		else
			self:PlaceStrategicResourceImpact(x, y, 3);
		end
		local impactPlotIndex = y * iW + x + 1;
		self.playerCollisionData[impactPlotIndex] = true;
		print("-"); print("City State", city_state_number, "has been started at Plot", x, y, "in Region#", region_number);
	else
		print("-"); print("WARNING: Crowding issues for City State #", city_state_number, " - Could not find valid site in Region#", region_number);
		self.iNumCityStatesDiscarded = self.iNumCityStatesDiscarded + 1;
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceCityStates()
	print("Map Generation - Choosing sites for City States");
	-- This function is dependent on AssignLuxuryRoles() having been executed first.
	-- This is because some city state placements are made in compensation for drawing
	-- the short straw in regard to multiple regions being assigned the same luxury type.

	self:AssignCityStatesToRegionsOrToUninhabited();

	print("-"); print("--- City State Placement Results ---");

	local iW, iH = Map.GetGridSize();
	local iUninhabitedCandidatePlots = table.maxn(self.uninhabited_areas_coastal_plots) + table.maxn(self.uninhabited_areas_inland_plots);
	print("-"); print("."); print(". NUMBER OF UNINHABITED CS CANDIDATE PLOTS: ", iUninhabitedCandidatePlots); print(".");
	for cs_number, region_number in ipairs(self.city_state_region_assignments) do
		if cs_number <= self.iNumCityStates then -- Make sure it's an active city state before processing.
			if region_number == -1 and iUninhabitedCandidatePlots > 0 then -- Assigned to areas outside of Regions.
				-- print("Place City States, place in uninhabited called for City State", cs_number);
				iUninhabitedCandidatePlots = iUninhabitedCandidatePlots - 1;
				local cs_x, cs_y, success;
				cs_x, cs_y, success = self:PlaceCityState(self.uninhabited_areas_coastal_plots, self.uninhabited_areas_inland_plots, true, true);

				-- Disabling fallback methods that remove proximity and collision checks.
				-- Jon has decided that city states that do not fit on the map will simply not be placed, but instead discarded.
				--[[
				if not success then -- Try again, this time with proximity checks disabled.
					cs_x, cs_y, success = self:PlaceCityState(self.uninhabited_areas_coastal_plots, self.uninhabited_areas_inland_plots, false, true);
					if not success then -- Try a third time, this time with all collision checks disabled.
						cs_x, cs_y, success = self:PlaceCityState(self.uninhabited_areas_coastal_plots, self.uninhabited_areas_inland_plots, false, false);
					end
				end
				--]]

				if success then
					self.cityStatePlots[cs_number] = {cs_x, cs_y, -1};
					self.city_state_validity_table[cs_number] = true; -- This is the line that marks a city state as valid to be processed by the rest of the system.
					local city_state_ID = cs_number + MAX_MAJOR_CIVS - 1;
					local cityState = Players[city_state_ID];
					local cs_start_plot = Map.GetPlot(cs_x, cs_y);
					cityState:SetStartingPlot(cs_start_plot);
					self:GenerateLuxuryPlotListsAtCitySite(cs_x, cs_y, 1, true) -- Removes Feature Ice from coasts adjacent to the city state's new location
					self:PlaceResourceImpact(cs_x, cs_y, ImpactLayers.LAYER_CITY_STATE, 4);
					self:PlaceResourceImpact(cs_x, cs_y, ImpactLayers.LAYER_LUXURY, 3);
					self:PlaceResourceImpact(cs_x, cs_y, ImpactLayers.LAYER_BONUS, 3);
					self:PlaceResourceImpact(cs_x, cs_y, ImpactLayers.LAYER_FISH, 3);
					self:PlaceResourceImpact(cs_x, cs_y, ImpactLayers.LAYER_MARBLE, 3);
					self:PlaceResourceImpact(cs_x, cs_y, ImpactLayers.LAYER_IVORY, 3);
					-- Vanilla Communitas change - Militaristic CS gets more potential strategic resources
					if cityState:GetMinorCivTrait() == MinorCivTraitTypes.MINOR_CIV_TRAIT_MILITARISTIC then
						self:PlaceStrategicResourceImpact(cs_x, cs_y, 0);
					else
						self:PlaceStrategicResourceImpact(cs_x, cs_y, 3);
					end
					local impactPlotIndex = cs_y * iW + cs_x + 1;
					self.playerCollisionData[impactPlotIndex] = true;
					print("-"); print("City State", cs_number, "has been started at Plot", cs_x, cs_y, "in Uninhabited Lands");
				else
					print("-"); print("WARNING: Crowding issues for City State #", cs_number, " - Could not find valid site in Uninhabited Lands.", region_number);
					self.iNumCityStatesDiscarded = self.iNumCityStatesDiscarded + 1;
				end
			elseif region_number == -1 and iUninhabitedCandidatePlots <= 0 then -- Assigned to areas outside of Regions, but nowhere there to put them!
				local iRandRegion = 1 + Map.Rand(self.iNumCivs, "Emergency Redirect of CS placement, choosing Region - LUA");
				print("Place City States, place in uninhabited called for City State", cs_number, "but it has no legal site, so is being put in Region#", iRandRegion);
				self:PlaceCityStateInRegion(cs_number, iRandRegion);
			else -- Assigned to a Region.
				print("Place City States, place in Region#", region_number, "for City State", cs_number);
				self:PlaceCityStateInRegion(cs_number, region_number);
			end
		end
	end

	-- Last chance method to place city states that didn't fit where they were supposed to go.
	if self.iNumCityStatesDiscarded > 0 then
		-- Assemble a global plot list of eligible City State sites that remain.
		local cs_last_chance_plot_list = {};
		for y = 0, iH - 1 do
			for x = 0, iW - 1 do
				if self:CanPlaceCityStateAt(x, y, -1, false, false) then
					local i = y * iW + x + 1;
					table.insert(cs_last_chance_plot_list, i);
				end
			end
		end
		local iNumLastChanceCandidates = table.maxn(cs_last_chance_plot_list);
		-- If any eligible sites were found anywhere on the map, place as many of the remaining CS as possible.
		if iNumLastChanceCandidates > 0 then
			print("-"); print("-"); print("ALERT: Some City States failed to be placed due to overcrowding. Attempting 'last chance' placement method.");
			print("Total number of remaining eligible candidate plots:", iNumLastChanceCandidates);
			local last_chance_shuffled = GetShuffledCopyOfTable(cs_last_chance_plot_list);
			local cs_list = {};
			for cs_num = 1, self.iNumCityStates do
				if not self.city_state_validity_table[cs_num] then
					table.insert(cs_list, cs_num);
					print("City State #", cs_num, "not yet placed, adding it to 'last chance' list.");
				end
			end
			for _, cs_number in ipairs(cs_list) do
				local cs_x, cs_y, success;
				cs_x, cs_y, success = self:PlaceCityState(last_chance_shuffled, {}, true, true);
				if success then
					self.cityStatePlots[cs_number] = {cs_x, cs_y, -1};
					self.city_state_validity_table[cs_number] = true; -- This is the line that marks a city state as valid to be processed by the rest of the system.
					local city_state_ID = cs_number + MAX_MAJOR_CIVS - 1;
					local cityState = Players[city_state_ID];
					local cs_start_plot = Map.GetPlot(cs_x, cs_y);
					cityState:SetStartingPlot(cs_start_plot);
					self:GenerateLuxuryPlotListsAtCitySite(cs_x, cs_y, 1, true) -- Removes Feature Ice from coasts adjacent to the city state's new location
					self:PlaceResourceImpact(cs_x, cs_y, ImpactLayers.LAYER_CITY_STATE, 4);
					self:PlaceResourceImpact(cs_x, cs_y, ImpactLayers.LAYER_LUXURY, 3);
					self:PlaceResourceImpact(cs_x, cs_y, ImpactLayers.LAYER_BONUS, 3);
					self:PlaceResourceImpact(cs_x, cs_y, ImpactLayers.LAYER_FISH, 3);
					self:PlaceResourceImpact(cs_x, cs_y, ImpactLayers.LAYER_MARBLE, 3);
					self:PlaceResourceImpact(cs_x, cs_y, ImpactLayers.LAYER_IVORY, 3);
					-- Vanilla Communitas change - Militaristic CS gets more potential strategic resources
					if cityState:GetMinorCivTrait() == MinorCivTraitTypes.MINOR_CIV_TRAIT_MILITARISTIC then
						self:PlaceStrategicResourceImpact(cs_x, cs_y, 0);
					else
						self:PlaceStrategicResourceImpact(cs_x, cs_y, 3);
					end
					local impactPlotIndex = cs_y * iW + cs_x + 1;
					self.playerCollisionData[impactPlotIndex] = true;
					self.iNumCityStatesDiscarded = self.iNumCityStatesDiscarded - 1;
					print("-"); print("City State", cs_number, "has been RESCUED from the trash bin of history and started at Fallback Plot", cs_x, cs_y);
				else
					print("-"); print("We have run out of possible 'last chance' sites for unplaced city states!");
					break;
				end
			end
			if self.iNumCityStatesDiscarded > 0 then
				print("-"); print("ALERT: No eligible city state sites remain. DISCARDING", self.iNumCityStatesDiscarded, "city states. BYE BYE!"); print("-");
			end
		else
			print("-"); print("-"); print("ALERT: No eligible city state sites remain. DISCARDING", self.iNumCityStatesDiscarded, "city states. BYE BYE!"); print("-");
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:NormalizeCityState(x, y)
	-- Similar to the version for normalizing civ starts, but less placed, no third-ring considerations and different weightings.
	local iW, iH = Map.GetGridSize();
	local isEvenY = true;
	if y / 2 > math.floor(y / 2) then
		isEvenY = false;
	end
	local innerFourFood, innerThreeFood, innerTwoFood, innerHills, innerForest, innerOneHammer, innerOcean = 0, 0, 0, 0, 0, 0, 0;
	local outerFourFood, outerThreeFood, outerTwoFood, outerOcean = 0, 0, 0, 0;
	local innerCanHaveBonus, outerCanHaveBonus, innerBadTiles, outerBadTiles = 0, 0, 0, 0;
	local iNumFoodBonusNeeded = 0;
	local search_table = {};

	-- Data Chart for early game tile potentials
	-- 4F: Flood Plains, Grass on fresh water (includes forest and marsh).
	-- 3F: Dry Grass, Plains on fresh water (includes forest and jungle), Tundra on fresh water (includes forest), Oasis
	-- 2F: Dry Plains, Lake, all remaining Jungles.
	-- 1H: Plains, Jungle on Plains

	-- Evaluate First Ring
	if isEvenY then
		search_table = self.firstRingYIsEven;
	else
		search_table = self.firstRingYIsOdd;
	end

	for _, plot_adjustments in ipairs(search_table) do
		local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments);

		if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then -- This plot's off the map edge.
			innerBadTiles = innerBadTiles + 1;
		else
			local searchPlot = Map.GetPlot(searchX, searchY);
			local plotType = searchPlot:GetPlotType();
			local terrainType = searchPlot:GetTerrainType();
			local featureType = searchPlot:GetFeatureType();

			if plotType == PlotTypes.PLOT_MOUNTAIN then
				innerBadTiles = innerBadTiles + 1;
			elseif plotType == PlotTypes.PLOT_OCEAN then
				if searchPlot:IsLake() then
					if featureType == FeatureTypes.FEATURE_ICE then
						innerBadTiles = innerBadTiles + 1;
					else
						innerTwoFood = innerTwoFood + 1;
					end
				else
					if featureType == FeatureTypes.FEATURE_ICE then
						innerBadTiles = innerBadTiles + 1;
					else
						innerOcean = innerOcean + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
					end
				end
			else -- Habitable plot.
				if plotType == PlotTypes.PLOT_HILLS then
					innerHills = innerHills + 1;
					if featureType == FeatureTypes.FEATURE_JUNGLE then
						innerTwoFood = innerTwoFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
					elseif featureType == FeatureTypes.FEATURE_FOREST then
						innerCanHaveBonus = innerCanHaveBonus + 1;
					end
				elseif searchPlot:IsFreshWater() then
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						innerFourFood = innerFourFood + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then
							innerCanHaveBonus = innerCanHaveBonus + 1;
						end
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						end
					elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
						innerFourFood = innerFourFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						innerThreeFood = innerThreeFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						else
							innerOneHammer = innerOneHammer + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
						innerThreeFood = innerThreeFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						innerBadTiles = innerBadTiles + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1; -- Can have Oasis.
					else -- Snow
						innerBadTiles = innerBadTiles + 1;
					end
				else -- Dry Flatlands
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						innerThreeFood = innerThreeFood + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then
							innerCanHaveBonus = innerCanHaveBonus + 1;
						end
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						innerTwoFood = innerTwoFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						else
							innerOneHammer = innerOneHammer + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
						innerCanHaveBonus = innerCanHaveBonus + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						else
							innerBadTiles = innerBadTiles + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						innerBadTiles = innerBadTiles + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1; -- Can have Oasis.
					else -- Snow
						innerBadTiles = innerBadTiles + 1;
					end
				end
			end
		end
	end

	-- Evaluate Second Ring
	if isEvenY then
		search_table = self.secondRingYIsEven;
	else
		search_table = self.secondRingYIsOdd;
	end
	for _, plot_adjustments in ipairs(search_table) do
		local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments);
		if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then -- This plot's off the map edge.
			outerBadTiles = outerBadTiles + 1;
		else
			local searchPlot = Map.GetPlot(searchX, searchY);
			local plotType = searchPlot:GetPlotType();
			local terrainType = searchPlot:GetTerrainType();
			local featureType = searchPlot:GetFeatureType();

			if plotType == PlotTypes.PLOT_MOUNTAIN then
				outerBadTiles = outerBadTiles + 1;
			elseif plotType == PlotTypes.PLOT_OCEAN then
				if searchPlot:IsLake() then
					if featureType == FeatureTypes.FEATURE_ICE then
						outerBadTiles = outerBadTiles + 1;
					else
						outerTwoFood = outerTwoFood + 1;
					end
				else
					if featureType == FeatureTypes.FEATURE_ICE then
						outerBadTiles = outerBadTiles + 1;
					elseif terrainType == TerrainTypes.TERRAIN_COAST then
						outerCanHaveBonus = outerCanHaveBonus + 1;
						outerOcean = outerOcean + 1;
					end
				end
			else -- Habitable plot.
				if plotType == PlotTypes.PLOT_HILLS then
					if featureType == FeatureTypes.FEATURE_JUNGLE then
						outerTwoFood = outerTwoFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
					elseif featureType == FeatureTypes.FEATURE_FOREST then
						outerCanHaveBonus = outerCanHaveBonus + 1;
					end
				elseif searchPlot:IsFreshWater() then
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						outerFourFood = outerFourFood + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then
							outerCanHaveBonus = outerCanHaveBonus + 1;
						end
					elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
						outerFourFood = outerFourFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						outerThreeFood = outerThreeFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
						outerThreeFood = outerThreeFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						outerBadTiles = outerBadTiles + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1; -- Can have Oasis.
					else -- Snow
						outerBadTiles = outerBadTiles + 1;
					end
				else -- Dry Flatlands
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						outerThreeFood = outerThreeFood + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then
							outerCanHaveBonus = outerCanHaveBonus + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						outerTwoFood = outerTwoFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
						outerCanHaveBonus = outerCanHaveBonus + 1;
						if featureType ~= FeatureTypes.FEATURE_FOREST then
							outerBadTiles = outerBadTiles + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						outerBadTiles = outerBadTiles + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1; -- Can have Oasis.
					else -- Snow
						outerBadTiles = outerBadTiles + 1;
					end
				end
			end
		end
	end

	-- Adjust the hammer situation, if needed.
	local hammerScore = (4 * innerHills) + (2 * innerForest) + innerOneHammer;
	local randomized_first_ring_adjustments;
	if hammerScore < 4 then -- Change a first ring plot to Hills.
		if isEvenY then
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsEven);
		else
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsOdd);
		end
		for attempt = 1, 6 do
			local plot_adjustments = randomized_first_ring_adjustments[attempt];
			local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments);
			-- Attempt to place a Hill at the currently chosen plot.
			local placedHill = self:AttemptToPlaceHillsAtPlot(searchX, searchY);
			if placedHill then
				hammerScore = hammerScore + 4;
				-- print("Added hills next to hammer-poor city state at ", x, y);
				break;
			elseif attempt == 6 then
				-- print("FAILED to add hills next to hammer-poor city state at ", x, y);
			end
		end
	end

	-- Rate the food situation.
	local innerFoodScore = 4 * innerFourFood + 2 * innerThreeFood + innerTwoFood;
	local outerFoodScore = 4 * outerFourFood + 2 * outerThreeFood + outerTwoFood;
	local totalFoodScore = innerFoodScore + outerFoodScore;

	-- Debug printout of food scores.
	-- print("-");
	-- print("-- - City State #", city_state_number, " has Food Score of ", totalFoodScore, " with rings of ", innerFoodScore, outerFoodScore);


	-- Three levels for Bonus Resource support, from zero to two.
	if totalFoodScore < 12 or innerFoodScore < 4 then
		iNumFoodBonusNeeded = 2;
	elseif totalFoodScore < 16 and innerFoodScore < 9 then
		iNumFoodBonusNeeded = 1;
	end
	-- Add Bonus Resources to food-poor city states.
	if iNumFoodBonusNeeded > 0 then
		-- print("-");
		-- print("Food-Poor city state ", x, y, " needs ", iNumFoodBonusNeeded, " Bonus, with ", innerCanHaveBonus + outerCanHaveBonus, " eligible plots.");
		-- print("-");

		local innerPlaced, outerPlaced = 0, 0;
		local randomized_first_ring_adjustments, randomized_second_ring_adjustments;
		if isEvenY then
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsEven);
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsEven);
		else
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsOdd);
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsOdd);
		end
		local tried_all_first_ring = false;
		local tried_all_second_ring = false;
		local allow_oasis = true; -- Permanent flag. (We don't want to place more than one Oasis per location).
		while iNumFoodBonusNeeded > 0 do
			if innerPlaced < 2 and innerCanHaveBonus > 0 and not tried_all_first_ring then -- Add bonus to inner ring.
				for attempt = 1, 6 do
					local plot_adjustments = randomized_first_ring_adjustments[attempt];
					local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments);
					-- Attempt to place a Bonus at the currently chosen plot.
					local placedBonus, placedOasis = self:AttemptToPlaceBonusResourceAtPlot(searchX, searchY, allow_oasis);
					if placedBonus then
						if allow_oasis and placedOasis then -- First oasis was placed on this pass, so change permission.
							allow_oasis = false;
						end
						-- print("Placed a Bonus in first ring at ", searchX, searchY);
						innerPlaced = innerPlaced + 1;
						innerCanHaveBonus = innerCanHaveBonus - 1;
						iNumFoodBonusNeeded = iNumFoodBonusNeeded - 1;
						break;
					elseif attempt == 6 then
						tried_all_first_ring = true;
					end
				end

			elseif innerPlaced + outerPlaced < 4 and outerCanHaveBonus > 0 and not tried_all_second_ring then
				-- Add bonus to second ring.
				for attempt = 1, 12 do
					local plot_adjustments = randomized_second_ring_adjustments[attempt];
					local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments);
					-- Attempt to place a Bonus at the currently chosen plot.
					local placedBonus, placedOasis = self:AttemptToPlaceBonusResourceAtPlot(searchX, searchY, allow_oasis);
					if placedBonus then
						if allow_oasis and placedOasis then -- First oasis was placed on this pass, so change permission.
							allow_oasis = false;
						end
						-- print("Placed a Bonus in second ring at ", searchX, searchY);
						outerPlaced = outerPlaced + 1;
						outerCanHaveBonus = outerCanHaveBonus - 1;
						iNumFoodBonusNeeded = iNumFoodBonusNeeded - 1;
						break;
					elseif attempt == 12 then
						tried_all_second_ring = true;
					end
				end

			else -- Tried everywhere, have to give up.
				break;
			end
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:NormalizeCityStateLocations()
	for city_state, data_table in ipairs(self.cityStatePlots) do
		if self.city_state_validity_table[city_state] then
			local x = data_table[1];
			local y = data_table[2];
			self:NormalizeCityState(x, y);
		else
			print("WARNING: City State #", city_state, "is not valid in this game. It must have been discarded from overcrowding.");
		end
	end
end
------------------------------------------------------------------------------
-- Start of functions tied to PlaceResourcesAndCityStates()
------------------------------------------------------------------------------
function AssignStartingPlots:GenerateResourcePlotListsFromSpecificPlots(plotList)
	-- This function generates resource plot lists out of a given list of plots.
	-- This is called by GenerateGlobalResourcePlotLists, GenerateLuxuryPlotListsAtCitySite and GenerateLuxuryPlotListsInRegion.

	if not plotList then
		return {};
	end

	local iW, _ = Map.GetGridSize();

	local tResourceList = {};
	for _, v in pairs(PlotListTypes) do
		tResourceList[v] = {};
	end

	for _, plotIndex in ipairs(plotList) do
		local x = (plotIndex - 1) % iW;
		local y = (plotIndex - x - 1) / iW;
		local plot = Map.GetPlot(x, y);
		local plotType = plot:GetPlotType();
		local terrainType = plot:GetTerrainType();
		local featureType = plot:GetFeatureType();
		if plotType == PlotTypes.PLOT_MOUNTAIN then
			table.insert(tResourceList[PlotListTypes.MOUNTAIN], plotIndex);
		elseif plotType == PlotTypes.PLOT_OCEAN then
			if featureType ~= FeatureTypes.FEATURE_ATOLL then
				if featureType == FeatureTypes.FEATURE_ICE then
					table.insert(tResourceList[PlotListTypes.ICE], plotIndex);
				elseif plot:IsLake() then
					table.insert(tResourceList[PlotListTypes.LAKE], plotIndex);
				elseif terrainType == TerrainTypes.TERRAIN_COAST then
					table.insert(tResourceList[PlotListTypes.COAST], plotIndex);
					if plot:IsAdjacentToLand() then
						table.insert(tResourceList[PlotListTypes.COAST_NEXT_TO_LAND], plotIndex);
					end
				else
					table.insert(tResourceList[PlotListTypes.OCEAN], plotIndex);
				end
			end
		elseif plotType == PlotTypes.PLOT_HILLS then
			table.insert(tResourceList[PlotListTypes.HILLS], plotIndex);
			table.insert(tResourceList[PlotListTypes.LAND], plotIndex);
			if featureType == FeatureTypes.NO_FEATURE then
				if terrainType ~= TerrainTypes.TERRAIN_SNOW then
					table.insert(tResourceList[PlotListTypes.MARBLE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_NO_FEATURE], plotIndex);
				end
				if terrainType == TerrainTypes.TERRAIN_TUNDRA then
					table.insert(tResourceList[PlotListTypes.HILLS_TUNDRA_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_TUNDRA_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_TUNDRA_PLAINS_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_TUNDRA_PLAINS_NO_FEATURE], plotIndex);
				elseif terrainType == TerrainTypes.TERRAIN_DESERT then
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_PLAINS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_TUNDRA_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_PLAINS_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_TUNDRA_PLAINS_NO_FEATURE], plotIndex);
				elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
					table.insert(tResourceList[PlotListTypes.HILLS_PLAINS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_PLAINS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_PLAINS_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_TUNDRA_PLAINS_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_TUNDRA_PLAINS_NO_FEATURE], plotIndex);
				elseif terrainType == TerrainTypes.TERRAIN_GRASS then
					table.insert(tResourceList[PlotListTypes.HILLS_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_PLAINS_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_TUNDRA_PLAINS_GRASS_NO_FEATURE], plotIndex);
				elseif terrainType == TerrainTypes.TERRAIN_SNOW then
					table.insert(tResourceList[PlotListTypes.HILLS_SNOW], plotIndex);
				else
					-- Unknown terrain!
					table.insert(tResourceList[PlotListTypes.UNKNOWN], plotIndex);
					table.remove(tResourceList[PlotListTypes.HILLS]);
					table.remove(tResourceList[PlotListTypes.LAND]);
				end
			elseif featureType == FeatureTypes.FEATURE_JUNGLE then
				table.insert(tResourceList[PlotListTypes.MARBLE], plotIndex);
				table.insert(tResourceList[PlotListTypes.HILLS_JUNGLE], plotIndex);
				table.insert(tResourceList[PlotListTypes.HILLS_COVERED], plotIndex);
				if terrainType == TerrainTypes.TERRAIN_PLAINS then
					table.insert(tResourceList[PlotListTypes.HILLS_PLAINS_COVERED], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_PLAINS_GRASS_COVERED], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_TUNDRA_PLAINS_COVERED], plotIndex);
				elseif terrainType == TerrainTypes.TERRAIN_GRASS then
					table.insert(tResourceList[PlotListTypes.HILLS_PLAINS_GRASS_COVERED], plotIndex);
				end
			elseif featureType == FeatureTypes.FEATURE_FOREST then
				table.insert(tResourceList[PlotListTypes.MARBLE], plotIndex);
				table.insert(tResourceList[PlotListTypes.HILLS_FOREST], plotIndex);
				table.insert(tResourceList[PlotListTypes.HILLS_COVERED], plotIndex);
				if terrainType == TerrainTypes.TERRAIN_TUNDRA then
					table.insert(tResourceList[PlotListTypes.TUNDRA_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_TUNDRA_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_TUNDRA_PLAINS_COVERED], plotIndex);
				elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
					table.insert(tResourceList[PlotListTypes.PLAINS_GRASS_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_PLAINS_COVERED], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_PLAINS_GRASS_COVERED], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_TUNDRA_PLAINS_COVERED], plotIndex);
				elseif terrainType == TerrainTypes.TERRAIN_GRASS then
					table.insert(tResourceList[PlotListTypes.PLAINS_GRASS_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_PLAINS_GRASS_COVERED], plotIndex);
				end
			else
				-- Unknown feature!
				table.insert(tResourceList[PlotListTypes.UNKNOWN], plotIndex);
				table.remove(tResourceList[PlotListTypes.HILLS]);
				table.remove(tResourceList[PlotListTypes.LAND]);
			end
		elseif featureType == FeatureTypes.FEATURE_MARSH then
			table.insert(tResourceList[PlotListTypes.FLAT], plotIndex);
			table.insert(tResourceList[PlotListTypes.LAND], plotIndex);
			table.insert(tResourceList[PlotListTypes.MARSH], plotIndex);
			if self:IsTropical(y) then
				table.insert(tResourceList[PlotListTypes.TROPICAL_MARSH], plotIndex);
				table.insert(tResourceList[PlotListTypes.RICE], plotIndex);
			end
		elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
			table.insert(tResourceList[PlotListTypes.FLAT], plotIndex);
			table.insert(tResourceList[PlotListTypes.LAND], plotIndex);
			table.insert(tResourceList[PlotListTypes.FLOOD_PLAINS], plotIndex);
			table.insert(tResourceList[PlotListTypes.FLAT_DESERT_WET], plotIndex);
			table.insert(tResourceList[PlotListTypes.FLAT_DESERT_NO_OASIS], plotIndex);
			if self:IsTropical(y) then
				table.insert(tResourceList[PlotListTypes.MAIZE], plotIndex);
			else
				table.insert(tResourceList[PlotListTypes.WHEAT], plotIndex);
			end
		elseif plotType == PlotTypes.PLOT_LAND then
			table.insert(tResourceList[PlotListTypes.FLAT], plotIndex);
			table.insert(tResourceList[PlotListTypes.LAND], plotIndex);
			if featureType == FeatureTypes.FEATURE_JUNGLE then
				table.insert(tResourceList[PlotListTypes.FLAT_JUNGLE], plotIndex);
				table.insert(tResourceList[PlotListTypes.FLAT_COVERED], plotIndex);
				if terrainType == TerrainTypes.TERRAIN_PLAINS then
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_COVERED], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_GRASS_COVERED], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_TUNDRA_PLAINS_COVERED], plotIndex);
					if plot:IsCoastalLand() then
						table.insert(tResourceList[PlotListTypes.COCONUT], plotIndex);
					end
				elseif terrainType == TerrainTypes.TERRAIN_GRASS then
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_GRASS_COVERED], plotIndex);
					if plot:IsCoastalLand() then
						table.insert(tResourceList[PlotListTypes.COCONUT], plotIndex);
					end
				end
			elseif featureType == FeatureTypes.FEATURE_FOREST then
				table.insert(tResourceList[PlotListTypes.FLAT_FOREST], plotIndex);
				table.insert(tResourceList[PlotListTypes.FLAT_COVERED], plotIndex);
				if terrainType == TerrainTypes.TERRAIN_TUNDRA then
					table.insert(tResourceList[PlotListTypes.FLAT_TUNDRA], plotIndex);
					table.insert(tResourceList[PlotListTypes.TUNDRA_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_TUNDRA_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_TUNDRA_PLAINS_COVERED], plotIndex);
				elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
					table.insert(tResourceList[PlotListTypes.PLAINS_GRASS_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_COVERED], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_GRASS_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_GRASS_COVERED], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_TUNDRA_PLAINS_COVERED], plotIndex);
				elseif terrainType == TerrainTypes.TERRAIN_GRASS then
					table.insert(tResourceList[PlotListTypes.PLAINS_GRASS_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_GRASS_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_GRASS_COVERED], plotIndex);
				end
			elseif featureType == FeatureTypes.NO_FEATURE then
				if terrainType == TerrainTypes.TERRAIN_SNOW then
					table.insert(tResourceList[PlotListTypes.FLAT_SNOW], plotIndex);
				elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
					table.insert(tResourceList[PlotListTypes.MARBLE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_TUNDRA], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_TUNDRA_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_TUNDRA_PLAINS_GRASS_NO_FEATURE], plotIndex);
				elseif terrainType == TerrainTypes.TERRAIN_DESERT then
					table.insert(tResourceList[PlotListTypes.MARBLE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_DESERT_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_DESERT_NO_OASIS], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE], plotIndex);
					if plot:IsFreshWater() then
						table.insert(tResourceList[PlotListTypes.FLAT_DESERT_WET], plotIndex);
						if self:IsTropical(y) then
							table.insert(tResourceList[PlotListTypes.MAIZE], plotIndex);
						else
							table.insert(tResourceList[PlotListTypes.WHEAT], plotIndex);
						end
					end
					if plot:IsCoastalLand() then
						if self:IsTropical(y) then
							table.insert(tResourceList[PlotListTypes.COCONUT], plotIndex);
						end
					end
				elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
					table.insert(tResourceList[PlotListTypes.MARBLE], plotIndex);
					table.insert(tResourceList[PlotListTypes.MAIZE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_TUNDRA_PLAINS_GRASS_NO_FEATURE], plotIndex);
					if plot:IsFreshWater() then
						table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_WET_NO_FEATURE], plotIndex);
					else
						table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_DRY_NO_FEATURE], plotIndex);
					end
					if self:IsTropical(y) then
						if plot:IsCoastalLand() then
							table.insert(tResourceList[PlotListTypes.COCONUT], plotIndex);
						end
					else
						table.insert(tResourceList[PlotListTypes.WHEAT], plotIndex);
					end
				elseif terrainType == TerrainTypes.TERRAIN_GRASS then
					table.insert(tResourceList[PlotListTypes.MARBLE], plotIndex);
					table.insert(tResourceList[PlotListTypes.MAIZE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_TUNDRA_PLAINS_GRASS_NO_FEATURE], plotIndex);
					if self:IsTropical(y) then
						table.insert(tResourceList[PlotListTypes.RICE], plotIndex);
					end
					if plot:IsFreshWater() then
						table.insert(tResourceList[PlotListTypes.FLAT_GRASS_WET_NO_FEATURE], plotIndex);
					else
						table.insert(tResourceList[PlotListTypes.FLAT_GRASS_DRY_NO_FEATURE], plotIndex);
					end
					if plot:IsCoastalLand() then
						if self:IsTropical(y) then
							table.insert(tResourceList[PlotListTypes.COCONUT], plotIndex);
						end
					end
				else
					-- Unknown terrain!
					table.insert(tResourceList[PlotListTypes.UNKNOWN], plotIndex);
					table.remove(tResourceList[PlotListTypes.FLAT]);
					table.remove(tResourceList[PlotListTypes.LAND]);
				end
			else
				-- Unknown feature!
				table.insert(tResourceList[PlotListTypes.UNKNOWN], plotIndex);
				table.remove(tResourceList[PlotListTypes.FLAT]);
				table.remove(tResourceList[PlotListTypes.LAND]);
			end
		else
			-- Unknown plot!
			table.insert(tResourceList[PlotListTypes.UNKNOWN], plotIndex);
		end
	end

	for _, v in pairs(PlotListTypes) do
		tResourceList[v] = GetShuffledCopyOfTable(tResourceList[v]);
	end

	return tResourceList;
end
------------------------------------------------------------------------------
function AssignStartingPlots:GenerateGlobalResourcePlotLists()
	-- This function generates all global plot lists needed for resource distribution.
	local iW, iH = Map.GetGridSize();
	local plotList = {};

	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = y * iW + x + 1; -- Lua tables/lists/arrays start at 1, not 0 like C++ or Python
			local plot = Map.GetPlot(x, y);
			-- Check if plot has a civ start, CS start, or Natural Wonder
			if self.playerCollisionData[i] then
				-- Do not process this plot!
			elseif plot:GetResourceType() ~= -1 then
				-- Plot has a resource already, do not include it.
			else
				table.insert(plotList, i);
			end
		end
	end

	-- Set up the Global Resource Plot Lists
	self.global_resource_plot_lists = self:GenerateResourcePlotListsFromSpecificPlots(plotList);

	-- print("Wheat list size:", table.maxn(self.global_resource_plot_lists[PlotListTypes.WHEAT]));
	-- print("Rice list size:", table.maxn(self.global_resource_plot_lists[PlotListTypes.RICE]));
	-- print("Maize list size:", table.maxn(self.global_resource_plot_lists[PlotListTypes.MAIZE]));
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceStrategicResourceImpact(x, y, radius)
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_IRON, radius);
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_HORSE, radius);
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_COAL, radius);
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_OIL, radius);
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_ALUMINUM, radius);
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_URANIUM, radius);
end
------------------------------------------------------------------------------
function AssignStartingPlots:CountTotalStrategicResourceImpact(plotIndex)
	return self.impactData[ImpactLayers.LAYER_IRON][plotIndex] +
		self.impactData[ImpactLayers.LAYER_HORSE][plotIndex] +
		self.impactData[ImpactLayers.LAYER_COAL][plotIndex] +
		self.impactData[ImpactLayers.LAYER_OIL][plotIndex] +
		self.impactData[ImpactLayers.LAYER_ALUMINUM][plotIndex] +
		self.impactData[ImpactLayers.LAYER_URANIUM][plotIndex];
end
------------------------------------------------------------------------------
function AssignStartingPlots:IsImpactLayerStrategic(layerIndex)
	return layerIndex == ImpactLayers.LAYER_IRON or
		layerIndex == ImpactLayers.LAYER_HORSE or
		layerIndex == ImpactLayers.LAYER_COAL or
		layerIndex == ImpactLayers.LAYER_OIL or
		layerIndex == ImpactLayers.LAYER_ALUMINUM or
		layerIndex == ImpactLayers.LAYER_URANIUM;
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceResourceImpact(x, y, impact_table_number, radius)
	-- This function operates upon one of the "impact and ripple" data overlays for resources.
	-- These data layers are a primary way of preventing assignments from clustering too much.
	local iW, iH = Map.GetGridSize();
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local impact_value = 99;
	local odd = self.firstRingYIsOdd;
	local even = self.firstRingYIsEven;
	local nextX, nextY, plot_adjustments;
	-- Place Impact!
	local impactPlotIndex = y * iW + x + 1;
	for i = 1, NUM_IMPACT_LAYERS do
		self.impactData[i][impactPlotIndex] = impact_value;
	end
	if radius == 0 then
		return;
	end
	-- Place Ripples
	if radius > 0 and radius < iH / 2 then
		for ripple_radius = 1, radius do
			local ripple_value = radius - ripple_radius + 1;
			-- Moving clockwise around the ring, the first direction to travel will be Northeast.
			-- This matches the direction-based data in the odd and even tables.
			-- Each subsequent change in direction will correctly match with these tables, too.

			-- Locate the plot within this ripple ring that is due West of the Impact Plot.
			local currentX = x - ripple_radius;
			local currentY = y;

			-- Now loop through the six directions, moving ripple_radius number of times per direction.
			-- At each plot in the ring, add the ripple_value for that ring to the plot's entry in the distance data table.
			for direction_index = 1, 6 do
				for _ = 1, ripple_radius do
					-- Must account for hex factor.
				 	if currentY / 2 > math.floor(currentY / 2) then -- Current Y is odd. Use odd table.
						plot_adjustments = odd[direction_index];
					else -- Current Y is even. Use plot adjustments from even table.
						plot_adjustments = even[direction_index];
					end
					-- Identify the next plot in the ring.
					nextX = currentX + plot_adjustments[1];
					nextY = currentY + plot_adjustments[2];
					-- Make sure the plot exists
					if not wrapX and (nextX < 0 or nextX >= iW) then -- X is out of bounds.
						-- Do not add ripple data to this plot.
					elseif not wrapY and (nextY < 0 or nextY >= iH) then -- Y is out of bounds.
						-- Do not add ripple data to this plot.
					else -- Plot is in bounds, process it.
						-- Handle any world wrap.
						local realX = nextX;
						local realY = nextY;
						if wrapX then
							realX = realX % iW;
						end
						if wrapY then
							realY = realY % iH;
						end
						-- Record ripple data for this plot.
						local ringPlotIndex = realY * iW + realX + 1;
						if impact_table_number == ImpactLayers.LAYER_CITY_STATE or
							impact_table_number == ImpactLayers.LAYER_MARBLE or
							impact_table_number == ImpactLayers.LAYER_IVORY then
							self.impactData[impact_table_number][ringPlotIndex] = 1;
						else
							if self.impactData[impact_table_number][ringPlotIndex] > 0 then
								-- First choose the greater of the two, existing value or current ripple.
								local stronger_value = math.max(self.impactData[impact_table_number][ringPlotIndex], ripple_value);
								-- Now increase it by 2 to reflect that multiple impact centers are in range of this plot.
								local overlap_value = math.min(50, stronger_value + 2);
								self.impactData[impact_table_number][ringPlotIndex] = overlap_value;
							else
								self.impactData[impact_table_number][ringPlotIndex] = ripple_value;
							end
						end
					end
					currentX, currentY = nextX, nextY;
				end
			end
		end
	else
		print("Unsupported Radius length of ", radius, " passed to PlaceResourceImpact()");
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:ProcessResourceList(frequency, impact_table_number, plot_list, resources_to_place)
	-- Added a random factor to strategic resources - Thalassicus

	-- This function needs to receive two numbers and two tables.
	-- Length of the plotlist is divided by frequency to get the number of resources to place.
	-- The first table is a list of plot indices.
	-- The second table contains subtables, one per resource type, detailing the resource ID number, quantity, weighting, and impact radius of each applicable resource.
	-- If radius min and max are different, the radius length is variable and a die roll will determine a value >= min and <= max.

	-- The system may be easiest to manage if the weightings add up to 100, so they can be handled as percentages, but this is not required.

	-- Impact #s - refer to the ImpactLayers enum
	-- Res data - 1 ID - 2 quantity - 3 weight - 4 radius min - 5 radius max

	-- The plot list will be processed sequentially, so shuffle it in advance.
	-- The default lists are terrain-oriented and are already shuffled.
	if not plot_list then
		print("Plot list was nil! -ProcessResourceList");
		return;
	end
	local iW, _ = Map.GetGridSize();
	local iNumTotalPlots = table.maxn(plot_list);
	local iNumResourcesToPlace = math.ceil(iNumTotalPlots / frequency);
	local iNumResourcesTypes = table.maxn(resources_to_place);
	local res_ID, res_quantity, res_weight, res_min, res_max, res_range, res_threshold = {}, {}, {}, {}, {}, {}, {};
	local totalWeight, accumulatedWeight = 0, 0;
	for index, resource_data in ipairs(resources_to_place) do
		res_ID[index] = resource_data[1];
		res_quantity[index] = resource_data[2];
		res_weight[index] = resource_data[3];
		totalWeight = totalWeight + resource_data[3];
		res_min[index] = resource_data[4];
		res_max[index] = resource_data[5];
		if res_max[index] > res_min[index] then
			res_range[index] = res_max[index] - res_min[index] + 1;
		else
			res_range[index] = -1;
		end
	end
	-- print("res_ID", res_ID[1], "iNumResourcesToPlace", iNumResourcesToPlace);

	for index = 1, iNumResourcesTypes do
		-- We'll roll a die and check each resource in turn to see if it is the one to get placed in that particular case.
		-- The weightings are used to decide how much percentage of the total each represents.
		-- This chunk sets the threshold for each resource in turn.
		local threshold = (res_weight[index] + accumulatedWeight) * 10000 / totalWeight;
		table.insert(res_threshold, threshold);
		accumulatedWeight = accumulatedWeight + res_weight[index];
	end

	-- Main loop
	local current_index = 1;
	local avoid_ripples = true;
	for _ = 1, iNumResourcesToPlace do
		local placed_this_res = false;
		local use_this_res_index = 1;
		local diceroll = Map.Rand(10000, "Choose resource type - Distribute Resources - Lua");
		for index, threshold in ipairs(res_threshold) do
			if diceroll < threshold then -- Choose this resource type.
				use_this_res_index = index;
				break;
			end
		end
		if avoid_ripples then -- Still on first pass through plot_list, seek first eligible 0 value on impact matrix.
			for index_to_check = current_index, iNumTotalPlots do
				if index_to_check == iNumTotalPlots then -- Completed first pass of plot_list, now change to seeking lowest value instead of zero value.
					avoid_ripples = false;
				end
				if placed_this_res then
					break;
				else
					current_index = current_index + 1;
				end
				local plotIndex = plot_list[index_to_check];
				if self.impactData[impact_table_number][plotIndex] == 0 then
					local x = (plotIndex - 1) % iW;
					local y = (plotIndex - x - 1) / iW;
					local res_plot = Map.GetPlot(x, y);
					if res_plot:GetResourceType() == -1 then -- Placing this strategic resource in this plot.
						local res_addition = 0;
						if res_range[use_this_res_index] ~= -1 then
							res_addition = Map.Rand(res_range[use_this_res_index], "Resource Radius - Place Resource LUA");
						end
						local quantity = res_quantity[use_this_res_index];
						-- added by azum4roll: give some variance to strategic amounts
						if self:IsImpactLayerStrategic(impact_table_number) then
							local rand = Map.Rand(10000, "ProcessResourceList - Lua") / 10000;
							if rand >= 0.75 then
								quantity = quantity * 1.2;
							elseif rand < 0.25 then
								quantity = quantity * 0.8;
							end
							quantity = math.floor(quantity + 0.5);
						end
						res_plot:SetResourceType(res_ID[use_this_res_index], quantity);
						if Game.GetResourceUsageType(res_ID[use_this_res_index]) == ResourceUsageTypes.RESOURCEUSAGE_LUXURY then
							self.totalLuxPlacedSoFar = self.totalLuxPlacedSoFar + 1;
						end
						self:PlaceResourceImpact(x, y, impact_table_number, res_min[use_this_res_index] + res_addition);
						placed_this_res = true;
						self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] = self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] + quantity;
					end
				end
			end
		end
		if not avoid_ripples then -- Completed first pass through plot_list, so use backup method.
			local lowest_impact = 98;
			local best_plot;
			for _, plotIndex in ipairs(plot_list) do
				if lowest_impact > self.impactData[impact_table_number][plotIndex] then
					local x = (plotIndex - 1) % iW;
					local y = (plotIndex - x - 1) / iW;
					local res_plot = Map.GetPlot(x, y);
					if res_plot:GetResourceType() == -1 then
						lowest_impact = self.impactData[impact_table_number][plotIndex];
						best_plot = plotIndex;
					end
				end
			end
			if best_plot then
				local x = (best_plot - 1) % iW;
				local y = (best_plot - x - 1) / iW;
				local res_plot = Map.GetPlot(x, y);
				local res_addition = 0;
				if res_range[use_this_res_index] ~= -1 then
					res_addition = Map.Rand(res_range[use_this_res_index], "Resource Radius - Place Resource LUA");
				end
				local quantity = res_quantity[use_this_res_index];
				-- added by azum4roll: give some variance to strategic amounts
				if self:IsImpactLayerStrategic(impact_table_number) then
					local rand = Map.Rand(10000, "ProcessResourceList - Lua") / 10000;
					if rand >= 0.75 then
						quantity = quantity * 1.2;
					elseif rand < 0.25 then
						quantity = quantity * 0.8;
					end
					quantity = math.floor(quantity + 0.5);
				end
				-- print("ProcessResourceList backup, Resource: " .. res_ID[use_this_res_index] .. ", Quantity: " .. quantity);
				res_plot:SetResourceType(res_ID[use_this_res_index], quantity);
				self:PlaceResourceImpact(x, y, impact_table_number, res_min[use_this_res_index] + res_addition);
				self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] = self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] + quantity;
			end
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceSpecificNumberOfResources(resource_ID, quantity, amount,
	                         ratio, impact_table_number, min_radius, max_radius, plot_list)
	-- This function needs to receive seven numbers and one table.

	-- Resource_ID is the type of resource to place.
	-- Quantity is the in-game quantity of the resource, or 0 if unquantified resource type.
	-- Amount is the number of plots intended to receive an assignment of this resource.

	-- Ratio should be > 0 and <= 1 and is what determines when secondary and tertiary lists come in to play.
	-- The actual ratio is (AmountOfResource / PlotsInList).
	-- For instance, if we are assigning Sugar resources to Marsh, then if we are to assign eight Sugar resources, but there are only four Marsh plots in the list,
	-- a ratio of 1 would assign a Sugar to every single marsh plot, and then have to return an unplaced value of 4;
	-- but a ratio of 0.5 would assign only two Sugars to the four marsh plots, and return a value of 6.
	-- Any ratio less than or equal to 0.25 would assign one Sugar and return seven, as the ratio results will be rounded up not down, to the nearest integer.

	-- Impact tables: see the ImpactLayers enum
	-- Radius is amount of impact to place on this table when placing a resource.

	-- nil tables are not acceptable but empty tables are fine

	-- The plot lists will be processed sequentially, so randomize them in advance.

	-- print("-"); print("PlaceSpecificResource called. ResID:", resource_ID, "Quantity:", quantity, "Amount:", amount, "Ratio:", ratio);

	if not plot_list then
		-- print("Plot list was nil! -PlaceSpecificNumberOfResources");
		return amount;
	end
	local bCheckImpact = false;
	local impact_table = {};
	if impact_table_number ~= ImpactLayers.LAYER_NONE then
		bCheckImpact = true;
		impact_table = self.impactData[impact_table_number];
	end
	local iW, _ = Map.GetGridSize();
	local iNumLeftToPlace = amount;
	local iNumPlots = table.maxn(plot_list);
	local iNumResources = math.min(amount, math.ceil(ratio * iNumPlots));

	-- Main loop
	for _ = 1, iNumResources do
		for _, plotIndex in ipairs(plot_list) do
			if not bCheckImpact or impact_table[plotIndex] == 0 then
				local x = (plotIndex - 1) % iW;
				local y = (plotIndex - x - 1) / iW;
				local res_plot = Map.GetPlot(x, y);
				local continue = true;
				if res_plot:GetResourceType() == -1 then -- Placing this resource in this plot.
					-- Marble and Ivory are in the special list and need extra impact checks
					if resource_ID == self.marble_ID then
						if self.impactData[ImpactLayers.LAYER_MARBLE][plotIndex] == 0 then
							self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_MARBLE, 9);
						else
							continue = false;
						end
					elseif resource_ID == self.ivory_ID then
						if self.impactData[ImpactLayers.LAYER_IVORY][plotIndex] == 0 then
							self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_IVORY, 9);
						else
							continue = false;
						end
					end
					if continue then
						res_plot:SetResourceType(resource_ID, quantity);
						self.amounts_of_resources_placed[resource_ID + 1] = self.amounts_of_resources_placed[resource_ID + 1] + quantity;
						-- print("-"); print("Placed Resource#", resource_ID, "at Plot", x, y);

						-- MOD.Barathor: Note: This is what screws up the original total, as non-luxury resources which use this placement function are added to the total.
						-- Luckily, this happens after all luxuries are already placed. (Unless you use the Strategic Balance setting.)
						self.totalLuxPlacedSoFar = self.totalLuxPlacedSoFar + 1;
						-- print("Total luxuries placed so far: ", self.totalLuxPlacedSoFar);

						iNumLeftToPlace = iNumLeftToPlace - 1;
						if bCheckImpact then
							local radius = self:GetRandomFromRangeInclusive(min_radius, max_radius);
							self:PlaceResourceImpact(x, y, impact_table_number, radius);
						end
						break;
					end
				end
			end
		end
	end
	return iNumLeftToPlace;
end
------------------------------------------------------------------------------
function AssignStartingPlots:IdentifyRegionsOfThisType(region_type)
	-- Necessary for assigning luxury types to regions.
	local regions_of_this_type = {};
	for index, current_type in ipairs(self.regionTypes) do
		if current_type == region_type then
			table.insert(regions_of_this_type, index);
		end
	end
	local length = table.maxn(regions_of_this_type);
	if length > 0 then
		local scrambled = GetShuffledCopyOfTable(regions_of_this_type);
		for _, region_to_add in ipairs(scrambled) do
			table.insert(self.regions_sorted_by_type, {region_to_add}); -- Note: adding region number as a table, so this data can be expanded later.
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:SortRegionsByType()
	-- Necessary for assigning luxury types to regions.
	for check_this_type = 1, NUM_REGION_TYPES - 1 do
		self:IdentifyRegionsOfThisType(check_this_type);
	end
	self:IdentifyRegionsOfThisType(0); -- If any Undefined Regions, put them at the bottom of the list.
end
------------------------------------------------------------------------------
function AssignStartingPlots:AssignLuxuryToRegion(region_number)
	local iNumMaxAllowedForRegions = 16; -- Constant: Maximum luxury types allowed to be assigned to regional distribution. CANNOT be reduced below 8!

	-- Assigns a luxury type to an individual region.
	local region_type = self.regionTypes[region_number];
	local luxury_candidates;
	if region_type ~= RegionTypes.REGION_UNDEFINED and region_type < NUM_REGION_TYPES then
		luxury_candidates = self.luxury_region_weights[region_type];
	else
		luxury_candidates = self.luxury_fallback_weights; -- Undefined Region, enable all possible luxury types.
	end

	-- Build options list.
	local iNumAvailableTypes = 0;
	local resource_IDs, resource_weights, res_threshold = {}, {}, {};
	local split_cap = self:GetLuxuriesSplitCap(); -- New for expansion. Cap no longer set to hardcoded value of 3.

	for _, resource_options in ipairs(luxury_candidates) do
		local res_ID = resource_options[1];
		if self.luxury_assignment_count[res_ID] < split_cap then -- This type still eligible.
			local test = TestMembership(self.resourceIDs_assigned_to_regions, res_ID);
			if self.iNumTypesAssignedToRegions < iNumMaxAllowedForRegions or test then -- Not a new type that would exceed number of allowed types, so continue.
				-- Water-based resources need to run a series of permission checks: coastal start in region, not a disallowed regions type, enough water, etc.
				if res_ID == self.whale_ID or res_ID == self.pearls_ID or res_ID == self.crab_ID or res_ID == self.coral_ID then
					-- MOD.Barathor: Removed unnecessary "regional" restrictions between whales/jungle, pearls/tundra, and crab/desert to make assignments more flexible.
					if self.startLocationConditions[region_number][1] then -- This region's start is along an ocean, so water-based luxuries are allowed.
						-- MOD.Barathor: Start
						-- MOD.Barathor: Base required coastal water total off of the target number of regional luxuries to place.
						local target_list = self:GetRegionLuxuryTargetNumbers();
						local target = target_list[self.iNumCivs];
						local water_needed = target * 8;
						-- MOD.Barathor: End
						if self.regionTerrainCounts[region_number][8] >= water_needed then -- Enough water available.
							table.insert(resource_IDs, res_ID);
							local adjusted_weight = resource_options[2] / (1 + self.luxury_assignment_count[res_ID]) -- If selected before, for a different region, reduce weight.
							table.insert(resource_weights, adjusted_weight);
							iNumAvailableTypes = iNumAvailableTypes + 1;
						end
					end
				-- Land-based resources are automatically approved if they were in the region's option table. Except silver and furs that need an extra check.
				elseif res_ID == self.silver_ID or res_ID == self.fur_ID then
					if (region_type == RegionTypes.REGION_TUNDRA and self.startLocationConditions[region_number][8] >= 2) or
						(region_type == RegionTypes.REGION_DESERT and self.startLocationConditions[region_number][9] >= 2) then
						-- enough tundra/desert to place the silver/furs at start location
						table.insert(resource_IDs, res_ID);
						local adjusted_weight = resource_options[2] / (1 + self.luxury_assignment_count[res_ID]);
						table.insert(resource_weights, adjusted_weight);
						iNumAvailableTypes = iNumAvailableTypes + 1;
					else
						-- debug
						print("Luxury", res_ID, "cannot be placed in region", region_number, "due to a lack of corresponding terrain at start location.");
					end
				else
					table.insert(resource_IDs, res_ID);
					local adjusted_weight = resource_options[2] / (1 + self.luxury_assignment_count[res_ID]);
					table.insert(resource_weights, adjusted_weight);
					iNumAvailableTypes = iNumAvailableTypes + 1;
				end
			end
		end
	end

	-- If options list is empty, pick from fallback options. First try to respect water-resources not being assigned to regions without coastal starts.
	if iNumAvailableTypes == 0 then
		print("Pick from fallback options for region#", region_number);
		for _, resource_options in ipairs(self.luxury_fallback_weights) do
			local res_ID = resource_options[1];
			if self.luxury_assignment_count[res_ID] < 3 then -- This type still eligible.
				local test = TestMembership(self.resourceIDs_assigned_to_regions, res_ID);
				if self.iNumTypesAssignedToRegions < iNumMaxAllowedForRegions or test then -- Won't exceed allowed types.
					if res_ID == self.whale_ID or res_ID == self.pearls_ID or res_ID == self.crab_ID or res_ID == self.coral_ID then
						-- MOD.Barathor: Removed unnecessary "regional" restrictions between whales/jungle, pearls/tundra, and crab/desert to make assignments more flexible.
						if self.startLocationConditions[region_number][1] then -- This region's start is along an ocean, so water-based luxuries are allowed.
							-- MOD.Barathor: Start
							-- MOD.Barathor: Base required coastal water total off of the target number of regional luxuries to place.
							local target_list = self:GetRegionLuxuryTargetNumbers();
							local target = target_list[self.iNumCivs];
							local water_needed = target * 8;
							-- MOD.Barathor: End
							if self.regionTerrainCounts[region_number][8] >= water_needed then -- Enough water available.
								table.insert(resource_IDs, res_ID);
								local adjusted_weight = resource_options[2] / (1 + self.luxury_assignment_count[res_ID]); -- If selected before, for a different region, reduce weight.
								table.insert(resource_weights, adjusted_weight);
								iNumAvailableTypes = iNumAvailableTypes + 1;
							end
						end
					-- Land-based resources are automatically approved if they were in the region's option table. Except silver and furs that need an extra check.
					elseif res_ID == self.silver_ID or res_ID == self.fur_ID then
						if (region_type == RegionTypes.REGION_TUNDRA and self.startLocationConditions[region_number][8] >= 2) or
							(region_type == RegionTypes.REGION_DESERT and self.startLocationConditions[region_number][9] >= 2) then
							-- enough tundra/desert to place the silver/furs at start location
							table.insert(resource_IDs, res_ID);
							local adjusted_weight = resource_options[2] / (1 + self.luxury_assignment_count[res_ID]);
							table.insert(resource_weights, adjusted_weight);
							iNumAvailableTypes = iNumAvailableTypes + 1;
						else
							-- debug
							print("Luxury", res_ID, "cannot be placed in region", region_number, "due to a lack of corresponding terrain at start location.");
						end
					else
						table.insert(resource_IDs, res_ID);
						local adjusted_weight = resource_options[2] / (1 + self.luxury_assignment_count[res_ID]);
						table.insert(resource_weights, adjusted_weight);
						iNumAvailableTypes = iNumAvailableTypes + 1;
					end
				end
			end
		end
	end

	-- If we get to here and still need to assign a luxury type, it means we have to force a water-based luxury in to this region, period.
	-- This should be the rarest of the rare emergency assignment cases, unless modifications to the system have tightened things too far.
	if iNumAvailableTypes == 0 then
		print("-"); print("Having to use emergency Luxury assignment process for Region#", region_number);
		print("This likely means a near-maximum number of civs in this game, and problems with not having enough legal Luxury types to spread around.");
		print("If you are modifying luxury types or number of regions allowed to get the same type, check to make sure your changes haven't violated the math so each region can have a legal assignment.");
		for _, resource_options in ipairs(self.luxury_fallback_weights) do
			local res_ID = resource_options[1];
			if self.luxury_assignment_count[res_ID] < 3 then -- This type still eligible.
				local test = TestMembership(self.resourceIDs_assigned_to_regions, res_ID);
				if self.iNumTypesAssignedToRegions < iNumMaxAllowedForRegions or test then -- Won't exceed allowed types.
					table.insert(resource_IDs, res_ID);
					local adjusted_weight = resource_options[2] / (1 + self.luxury_assignment_count[res_ID]);
					table.insert(resource_weights, adjusted_weight);
					iNumAvailableTypes = iNumAvailableTypes + 1;
				end
			end
		end
	end
	if iNumAvailableTypes == 0 then -- Bad mojo!
		print("-"); print("FAILED to assign a Luxury type to Region#", region_number); print("-");
	end

	-- Choose luxury.
	local totalWeight = 0;
	for _, this_weight in ipairs(resource_weights) do
		totalWeight = totalWeight + this_weight;
	end
	local accumulatedWeight = 0;
	for index = 1, iNumAvailableTypes do
		local threshold = (resource_weights[index] + accumulatedWeight) * 10000 / totalWeight;
		table.insert(res_threshold, threshold);
		accumulatedWeight = accumulatedWeight + resource_weights[index];
	end
	local use_this_ID;
	local diceroll = Map.Rand(10000, "Choose resource type - Assign Luxury To Region - Lua");
	for index, threshold in ipairs(res_threshold) do
		if diceroll <= threshold then -- Choose this resource type.
			use_this_ID = resource_IDs[index];
			break;
		end
	end

	return use_this_ID;
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetLuxuriesSplitCap()
	-- This data was separated out to allow easy replacement in map scripts.
	local split_cap = 1;
	if self.iNumCivs > 16 then
		split_cap = 2;
	end
	return split_cap;
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetCityStateLuxuriesTargetNumber()
	-- This data was separated out to allow easy replacement in map scripts.
	-- This function is currently UNUSED.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 4,
	};
	local CSluxCount = worldsizes[Map.GetWorldSize()];
	return CSluxCount;
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetDisabledLuxuriesTargetNumber()
	-- This data was separated out to allow easy replacement in map scripts.
	-- This function is currently UNUSED.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 10,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 7,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 1,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 0,
	};
	local maxToDisable = worldsizes[Map.GetWorldSize()];
	return maxToDisable;
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetRandomLuxuriesTargetNumber()
	-- This data was separated out to allow easy replacement in map scripts.
	-- With more luxuries available, this ensures that the total luxuries used each game still match the default game,
	-- except for Huge, which really needed a few more anyway!
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 7,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 9,
	};
	local maxRandoms = worldsizes[Map.GetWorldSize()];
	return maxRandoms;
end
------------------------------------------------------------------------------
function AssignStartingPlots:AssignLuxuryRoles()
	-- Each region gets an individual Luxury type assigned to it.
	-- Each Luxury type can be assigned to no more than three regions.
	-- No more than nine total Luxury types will be assigned to regions.
	-- Between two and four Luxury types will be assigned to City States.
	-- Remaining Luxury types will be distributed at random or left out.

	-- Luxury roles must be assigned before City States can be placed.
	-- This is because civs who are forced to share their luxury type with other
	-- civs may get extra city states placed in their region to compensate.

	self:SortRegionsByType(); -- creates self.regions_sorted_by_type, which will be expanded to store all data regarding regional luxuries.

	-- Assign a luxury to each region.
	for index, region_info in ipairs(self.regions_sorted_by_type) do
		local region_number = region_info[1];
		local resource_ID = self:AssignLuxuryToRegion(region_number);
		self.regions_sorted_by_type[index][2] = resource_ID; -- This line applies the assignment.
		self.region_luxury_assignment[region_number] = resource_ID;
		self.luxury_assignment_count[resource_ID] = self.luxury_assignment_count[resource_ID] + 1; -- Track assignments

		-- print("-"); print("Region#", region_number, " of type ", self.regionTypes[region_number], " has been assigned Luxury ID#", resource_ID);

		local already_assigned = TestMembership(self.resourceIDs_assigned_to_regions, resource_ID);
		if not already_assigned then
			table.insert(self.resourceIDs_assigned_to_regions, resource_ID);
			self.iNumTypesAssignedToRegions = self.iNumTypesAssignedToRegions + 1;
		end
	end

	-- Assign three of the remaining types to be exclusive to City States.
	-- Build options list.
	local iNumAvailableTypes = 0;
	local resource_IDs, resource_weights = {}, {};
	for _, resource_options in ipairs(self.luxury_city_state_weights) do
		local res_ID = resource_options[1];
		local test = TestMembership(self.resourceIDs_assigned_to_regions, res_ID);
		if not test then
			table.insert(resource_IDs, res_ID);
			table.insert(resource_weights, resource_options[2]);
			iNumAvailableTypes = iNumAvailableTypes + 1;
		else
			-- print("Luxury ID#", res_ID, "rejected by City States as already belonging to Regions.");
		end
	end
	if iNumAvailableTypes < 3 then
		print("---------------------------------------------------------------------------------------");
		print("- Luxuries have been modified in ways disruptive to the City State Assignment Process -");
		print("---------------------------------------------------------------------------------------");
	end
	-- Choose luxuries.
	for _ = 1, 3 do
		local totalWeight = 0;
		local res_threshold = {};
		for _, this_weight in ipairs(resource_weights) do
			totalWeight = totalWeight + this_weight;
		end
		local accumulatedWeight = 0;
		for index, weight in ipairs(resource_weights) do
			local threshold = (weight + accumulatedWeight) * 10000 / totalWeight;
			table.insert(res_threshold, threshold);
			accumulatedWeight = accumulatedWeight + resource_weights[index];
		end
		local use_this_ID;
		local diceroll = Map.Rand(10000, "Choose resource type - City State Luxuries - Lua");
		for index, threshold in ipairs(res_threshold) do
			if diceroll < threshold then -- Choose this resource type.
				use_this_ID = resource_IDs[index];
				table.insert(self.resourceIDs_assigned_to_cs, use_this_ID);
				table.remove(resource_IDs, index);
				table.remove(resource_weights, index);
				-- print("-"); print("City States have been assigned Luxury ID#", use_this_ID);
				break;
			end
		end
	end

	-- Assign Marble and Ivory to special casing.
	table.insert(self.resourceIDs_assigned_to_special_case, self.marble_ID);
	table.insert(self.resourceIDs_assigned_to_special_case, self.ivory_ID);

	--[[ MOD.Barathor:

	Modified the next block of code so that increasing the civ count on maps below the maximum Regional luxury total (which is now all of them)
	won't subtract from the Random total first.
	Instead, the Disabled total will adjust to whatever is leftover after assignments.

	This also optimizes the functionality of the Fallback weights table,
	so that flexible luxuries can be given a heavier weight when needed as a regional fallback
	and also will be more likely to be chosen for random distribution throughout the map.
	The old default method didn't use weightings for choosing random luxuries and instead randomly chose a number of luxuries to disable, then chose all the rest.

	--]]

	-- MOD.Barathor: Start
	-- MOD.Barathor: Assign some luxuries to random distribution, disable the rest.
	local rand_resource_IDs, rand_resource_weights = {}, {};
	for _, resource_options in ipairs(self.luxury_fallback_weights) do
		local res_ID = resource_options[1]
		local test1 = TestMembership(self.resourceIDs_assigned_to_regions, res_ID);
		local test2 = TestMembership(self.resourceIDs_assigned_to_cs, res_ID);
		if not test1 and not test2 then
			table.insert(rand_resource_IDs, res_ID);
			table.insert(rand_resource_weights, resource_options[2]);
		else
			-- print("Luxury ID#", res_ID, "rejected by Randoms as already belonging to Regions or City States.");
		end
	end

	self.iNumTypesRandom = self:GetRandomLuxuriesTargetNumber();
	for _ = 1, self.iNumTypesRandom do
		local totalWeight = 0;
		local res_threshold = {};
		for _, this_weight in ipairs(rand_resource_weights) do
			totalWeight = totalWeight + this_weight;
		end
		local accumulatedWeight = 0;
		for index, weight in ipairs(rand_resource_weights) do
			local threshold = (weight + accumulatedWeight) * 10000 / totalWeight;
			table.insert(res_threshold, threshold);
			accumulatedWeight = accumulatedWeight + rand_resource_weights[index];
		end
		local use_this_ID;
		local diceroll = Map.Rand(10000, "Choose resource type - Random Luxuries - Lua");
		for index, threshold in ipairs(res_threshold) do
			if diceroll < threshold then -- Choose this resource type.
				use_this_ID = rand_resource_IDs[index];
				table.insert(self.resourceIDs_assigned_to_random, use_this_ID);
				table.remove(rand_resource_IDs, index);
				table.remove(rand_resource_weights, index);
				-- print("-") print("Luxury ID#", use_this_ID, "assigned to Random.");
				break;
			end
		end
	end

	-- MOD.Barathor: Assign remaining luxuries to Disabled.
	local resourceIDs_not_being_used = {};
	for _, resource_options in ipairs(self.luxury_fallback_weights) do
		local res_ID = resource_options[1];
		-- print("-") print("Luxury ID#", res_ID, "checking to disable.");
		local test1 = TestMembership(self.resourceIDs_assigned_to_regions, res_ID);
		local test2 = TestMembership(self.resourceIDs_assigned_to_cs, res_ID);
		local test3 = TestMembership(self.resourceIDs_assigned_to_random, res_ID);
		if not test1 and not test2 and not test3 then
			table.insert(resourceIDs_not_being_used, res_ID);
			-- print("-") print("Luxury ID#", res_ID, "disabled.");
		else
			-- print("Luxury ID#", res_ID, "cannot be disabled and already assigned.");
		end
	end
	-- MOD.Barathor: End

	---[[ Debug printout of luxury assignments.
	print("--- Luxury Assignment Table ---");
	print("-"); print("- - Assigned to Regions - -");
	for _, data in ipairs(self.regions_sorted_by_type) do
		print("Region#", data[1], "has Luxury type", data[2]);
	end
	print("-"); print("Total unique regional luxuries: ", self.iNumTypesAssignedToRegions);
	print("-"); print("- - Assigned to City States - -");
	for _, type in ipairs(self.resourceIDs_assigned_to_cs) do
		print("Luxury type", type);
	end
	print("-"); print("- - Assigned to Random - -");
	for _, type in ipairs(self.resourceIDs_assigned_to_random) do
		print("Luxury type", type);
	end
	print("-"); print("- - Luxuries handled via Special Case - -");
	for _, type in ipairs(self.resourceIDs_assigned_to_special_case) do
		print("Luxury type", type);
	end
	print("-"); print("- - Disabled - -");
	for _, type in ipairs(resourceIDs_not_being_used) do
		print("Luxury type", type);
	end
	print("- - - - - - - - - - - - - - - -");
	--]]
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetListOfAllowableLuxuriesAtCitySite(x, y, radius)
	-- print("-"); print("- -"); print("Getting list of luxuries allowable at city state site:", x, y, "Radius:", radius);
	local iW, iH = Map.GetGridSize();
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local odd = self.firstRingYIsOdd;
	local even = self.firstRingYIsEven;
	local nextX, nextY, plot_adjustments;
	local allowed_luxuries = table.fill(false, self.MAX_RESOURCE_INDEX);

	for ripple_radius = 1, radius do
		local currentX = x - ripple_radius;
		local currentY = y;
		for direction_index = 1, 6 do
			for _ = 1, ripple_radius do
			 	if currentY / 2 > math.floor(currentY / 2) then
					plot_adjustments = odd[direction_index];
				else
					plot_adjustments = even[direction_index];
				end
				nextX = currentX + plot_adjustments[1];
				nextY = currentY + plot_adjustments[2];
				if not wrapX and (nextX < 0 or nextX >= iW) then
					-- X is out of bounds.
				elseif not wrapY and (nextY < 0 or nextY >= iH) then
					-- Y is out of bounds.
				else
					local realX = nextX;
					local realY = nextY;
					if wrapX then
						realX = realX % iW;
					end
					if wrapY then
						realY = realY % iH;
					end
					-- We've arrived at the correct x and y for the current plot.
					local plot = Map.GetPlot(realX, realY);
					local plotType = plot:GetPlotType();
					local terrainType = plot:GetTerrainType();
					local featureType = plot:GetFeatureType();
					-- MOD.Barathor: Start
					--[[ MOD.Barathor: Fixed: Check to make sure this plot doesn't already contain a resource!
						 This corrects a rare bug that occurs and denies some civs their 2nd bonus luxury type, since
						 it would mark a tile's resource options to "true" when there's already a regional luxury present. ]]
					if plot:GetResourceType() == -1 then
						-- Check this plot for luxury placement eligibility. Set allowed luxuries to true.
						--[[ MOD.Barathor: Updated all conditions below. Also added expansion and mod luxuries.
							 This function is used for placing luxuries at city-states and second types at civ starts. ]]
						if plotType == PlotTypes.PLOT_OCEAN then -- Testing for Water Luxury eligibility. This is more involved than land-based.
							if terrainType == TerrainTypes.TERRAIN_COAST then
								if not plot:IsLake() then
									if featureType ~= FeatureTypes.FEATURE_ATOLL and featureType ~= FeatureTypes.FEATURE_ICE then
										allowed_luxuries[self.whale_ID] = true;
										allowed_luxuries[self.pearls_ID] = true;
										allowed_luxuries[self.crab_ID] = true;
										allowed_luxuries[self.coral_ID] = true;
									end
								end
							end
						--[[ MOD.Barathor: With my new FixResourceGraphics(), plot and feature type doesn't matter.
							 The only important thing is the terrain type. This greatly increases the flexibility of
							 placing luxuries at city-states and can easily support more diversity in assignments. ]]
						elseif plotType == PlotTypes.PLOT_HILLS or plotType == PlotTypes.PLOT_LAND then
							if terrainType == TerrainTypes.TERRAIN_TUNDRA then
								allowed_luxuries[self.silver_ID] = true;
								allowed_luxuries[self.copper_ID] = true;
								allowed_luxuries[self.gems_ID] = true;
								allowed_luxuries[self.salt_ID] = true;
								allowed_luxuries[self.jade_ID] = true;
								allowed_luxuries[self.amber_ID] = true;
								allowed_luxuries[self.lapis_ID] = true;
								allowed_luxuries[self.fur_ID] = true;
								-- MOD.HungryForFood
								if self:IsEvenMoreResourcesActive() then
									allowed_luxuries[self.obsidian_ID] = true;
									allowed_luxuries[self.platinum_ID] = true;
									allowed_luxuries[self.tin_ID] = true;
								end
							elseif terrainType == TerrainTypes.TERRAIN_DESERT then
								allowed_luxuries[self.gold_ID] = true;
								allowed_luxuries[self.silver_ID] = true;
								allowed_luxuries[self.copper_ID] = true;
								allowed_luxuries[self.gems_ID] = true;
								allowed_luxuries[self.salt_ID] = true;
								allowed_luxuries[self.cotton_ID] = true;
								allowed_luxuries[self.jade_ID] = true;
								allowed_luxuries[self.amber_ID] = true;
								allowed_luxuries[self.lapis_ID] = true;
								allowed_luxuries[self.incense_ID] = true;
								if plot:GetFeatureType() == FeatureTypes.FEATURE_FLOOD_PLAINS then
									allowed_luxuries[self.sugar_ID] = true;
								end
								-- MOD.HungryForFood
								if self:IsEvenMoreResourcesActive() then
									allowed_luxuries[self.obsidian_ID] = true;
									allowed_luxuries[self.platinum_ID] = true;
									allowed_luxuries[self.poppy_ID] = true;
									allowed_luxuries[self.tin_ID] = true;
								end
							elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
								allowed_luxuries[self.gold_ID] = true;
								allowed_luxuries[self.copper_ID] = true;
								allowed_luxuries[self.gems_ID] = true;
								allowed_luxuries[self.salt_ID] = true;
								allowed_luxuries[self.jade_ID] = true;
								allowed_luxuries[self.amber_ID] = true;
								allowed_luxuries[self.lapis_ID] = true;
								allowed_luxuries[self.spices_ID] = true;
								allowed_luxuries[self.silk_ID] = true;
								allowed_luxuries[self.citrus_ID] = true;
								allowed_luxuries[self.truffles_ID] = true;
								allowed_luxuries[self.cocoa_ID] = true;
								allowed_luxuries[self.dye_ID] = true;
								allowed_luxuries[self.cotton_ID] = true;
								allowed_luxuries[self.wine_ID] = true;
								allowed_luxuries[self.coffee_ID] = true;
								allowed_luxuries[self.tea_ID] = true;
								allowed_luxuries[self.tobacco_ID] = true;
								allowed_luxuries[self.perfume_ID] = true;
								allowed_luxuries[self.olives_ID] = true;
								allowed_luxuries[self.incense_ID] = true;
								-- MOD.HungryForFood
								if self:IsEvenMoreResourcesActive() then
									allowed_luxuries[self.lavender_ID] = true;
									allowed_luxuries[self.obsidian_ID] = true;
									allowed_luxuries[self.platinum_ID] = true;
									allowed_luxuries[self.poppy_ID] = true;
									allowed_luxuries[self.tin_ID] = true;
								end
							elseif terrainType == TerrainTypes.TERRAIN_GRASS then
								allowed_luxuries[self.gold_ID] = true;
								allowed_luxuries[self.copper_ID] = true;
								allowed_luxuries[self.gems_ID] = true;
								allowed_luxuries[self.jade_ID] = true;
								allowed_luxuries[self.amber_ID] = true;
								allowed_luxuries[self.lapis_ID] = true;
								allowed_luxuries[self.spices_ID] = true;
								allowed_luxuries[self.silk_ID] = true;
								allowed_luxuries[self.sugar_ID] = true;
								allowed_luxuries[self.citrus_ID] = true;
								allowed_luxuries[self.truffles_ID] = true;
								allowed_luxuries[self.cocoa_ID] = true;
								allowed_luxuries[self.dye_ID] = true;
								allowed_luxuries[self.cotton_ID] = true;
								allowed_luxuries[self.wine_ID] = true;
								allowed_luxuries[self.coffee_ID] = true;
								allowed_luxuries[self.tea_ID] = true;
								allowed_luxuries[self.tobacco_ID] = true;
								allowed_luxuries[self.perfume_ID] = true;
								allowed_luxuries[self.olives_ID] = true;
								allowed_luxuries[self.incense_ID] = true;
								-- MOD.HungryForFood
								if self:IsEvenMoreResourcesActive() then
									allowed_luxuries[self.lavender_ID] = true;
									allowed_luxuries[self.obsidian_ID] = true;
									allowed_luxuries[self.platinum_ID] = true;
									allowed_luxuries[self.poppy_ID] = true;
									allowed_luxuries[self.tin_ID] = true;
								end
							end
						end
					end
					-- MOD.Barathor: End
					currentX, currentY = nextX, nextY;
				end
			end
		end
	end
	return allowed_luxuries;
end
------------------------------------------------------------------------------
function AssignStartingPlots:GenerateLuxuryPlotListsAtCitySite(x, y, radius, bRemoveFeatureIce)
	-- bRemoveFeatureIce is piggybacked on to this function to reduce redundant code.
	-- If ice is being removed from around a plot, ONLY that will occur. If both ice
	-- removal and plot list generation are desired, call this function twice.
	-- print("GenerateLuxuryPlotListsAtCitySite called. RemoveIce:", bRemoveFeatureIce, "Plot:", x, y, "Radius:", radius);
	local iW, iH = Map.GetGridSize();
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local odd = self.firstRingYIsOdd;
	local even = self.firstRingYIsEven;
	local nextX, nextY, plot_adjustments;
	local plotList = {};

	-- Iterate through the city's plots, adding them to the plot list.
	-- For notes on how the hex-iteration works, refer to PlaceResourceImpact()
	if radius > 0 and radius < 6 then
		for ripple_radius = 1, radius do
			local currentX = x - ripple_radius;
			local currentY = y;
			for direction_index = 1, 6 do
				for _ = 1, ripple_radius do
				 	if currentY / 2 > math.floor(currentY / 2) then
						plot_adjustments = odd[direction_index];
					else
						plot_adjustments = even[direction_index];
					end
					nextX = currentX + plot_adjustments[1];
					nextY = currentY + plot_adjustments[2];
					if not wrapX and (nextX < 0 or nextX >= iW) then
						-- X is out of bounds.
					elseif not wrapY and (nextY < 0 or nextY >= iH) then
						-- Y is out of bounds.
					else
						local realX = nextX;
						local realY = nextY;
						if wrapX then
							realX = realX % iW;
						end
						if wrapY then
							realY = realY % iH;
						end
						-- We've arrived at the correct x and y for the current plot.
						local plot = Map.GetPlot(realX, realY);
						local featureType = plot:GetFeatureType();
						local plotIndex = realY * iW + realX + 1;

						-- If Ice removal is enabled, process only that.
						if bRemoveFeatureIce then
							if featureType == FeatureTypes.FEATURE_ICE then
								plot:SetFeatureType(FeatureTypes.NO_FEATURE);
							end

						-- Otherwise generate the plot lists.
						else
							table.insert(plotList, plotIndex);
						end
					end
					currentX, currentY = nextX, nextY;
				end
			end
		end
	end

	return self:GenerateResourcePlotListsFromSpecificPlots(plotList);
end
------------------------------------------------------------------------------
function AssignStartingPlots:GenerateLuxuryPlotListsInRegion(region_number)
	local iW, iH = Map.GetGridSize();
	-- This function groups a region's plots in to lists, for Luxury resource assignment.
	local region_data_table = self.regionData[region_number];
	local iWestX = region_data_table[1];
	local iSouthY = region_data_table[2];
	local iWidth = region_data_table[3];
	local iHeight = region_data_table[4];
	local iAreaID = region_data_table[5];
	local region_area_object;
	if iAreaID ~= -1 then
		region_area_object = Map.GetArea(iAreaID);
	end
	local plotList = {};

	-- Iterate through the region's plots, adding them to the plot list.
	for region_loop_y = 0, iHeight - 1 do
		for region_loop_x = 0, iWidth - 1 do
			local x = (region_loop_x + iWestX) % iW;
			local y = (region_loop_y + iSouthY) % iH;
			local plotIndex = y * iW + x + 1;
			local plot = Map.GetPlot(x, y);
			-- get plot info
			local plotType = plot:GetPlotType();
			local terrainType = plot:GetTerrainType();
			local featureType = plot:GetFeatureType();

			if plotType == PlotTypes.PLOT_OCEAN then
				if terrainType == TerrainTypes.TERRAIN_COAST then
					if not plot:IsLake() and not plot:IsNaturalWonder(true) then
						if featureType ~= FeatureTypes.FEATURE_ATOLL and featureType ~= FeatureTypes.FEATURE_ICE then
							if iAreaID == -1 then
								if plot:IsAdjacentToLand() then
									table.insert(plotList, plotIndex);
								end
							elseif self.bArea then
								if plot:IsAdjacentToArea(region_area_object) then
									table.insert(plotList, plotIndex);
								end
							elseif plot:IsAdjacentToLandmass(iAreaID) then
								table.insert(plotList, plotIndex);
							end
						end
					end
				end
			elseif plotType ~= PlotTypes.PLOT_MOUNTAIN and not plot:IsNaturalWonder(true) and featureType ~= FeatureTypes.FEATURE_OASIS then
				if iAreaID == -1 then
					-- Always ok!
					table.insert(plotList, plotIndex);
				else
					-- Need to check whether the plot is in/adjacent to the correct area/landmass
					if self.bArea and (plot:GetArea() == iAreaID or plot:IsAdjacentToArea(region_area_object)) then
						table.insert(plotList, plotIndex);
					elseif not self.bArea and (plot:GetLandmass() == iAreaID or plot:IsAdjacentToLandmass(iAreaID)) then
						table.insert(plotList, plotIndex);
					end
				end
			end
		end
	end

	return self:GenerateResourcePlotListsFromSpecificPlots(plotList);
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetIndicesForLuxuryType(resource_ID)
	-- This function will identify up to 6 of the plot lists defined in GenerateResourcePlotListsFromSpecificPlots that match terrain best suitable for this type of luxury.
	-- print("-"); print("Obtaining indices for Luxury#", resource_ID);
	local tList = {};

	-- Water Luxuries
	if resource_ID == self.whale_ID then
		tList = {PlotListTypes.COAST_NEXT_TO_LAND};
	elseif resource_ID == self.pearls_ID then
		tList = {PlotListTypes.COAST_NEXT_TO_LAND};
	elseif resource_ID == self.crab_ID then
		tList = {PlotListTypes.COAST_NEXT_TO_LAND};
	elseif resource_ID == self.coral_ID then
		tList = {PlotListTypes.COAST_NEXT_TO_LAND};
	-- Rock Luxuries
	elseif resource_ID == self.marble_ID then
		tList = {
			PlotListTypes.HILLS_DESERT_TUNDRA_NO_FEATURE,
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.HILLS_COVERED,
			PlotListTypes.FLAT_COVERED,
		};
	elseif resource_ID == self.gold_ID then
		tList = {
			PlotListTypes.HILLS_DESERT_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_DESERT_NO_FEATURE,
			PlotListTypes.FLAT_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_NO_FEATURE,
			PlotListTypes.HILLS_PLAINS_GRASS_COVERED,
			PlotListTypes.FLAT_PLAINS_GRASS_COVERED,
		};
	elseif resource_ID == self.silver_ID then
		-- Tundra and desert only. Snow is backup (to be handled in AdjustTiles).
		tList = {
			PlotListTypes.HILLS_TUNDRA_NO_FEATURE,
			PlotListTypes.HILLS_DESERT,
			PlotListTypes.FLAT_TUNDRA_NO_FEATURE,
			PlotListTypes.FLAT_DESERT_NO_FEATURE,
			PlotListTypes.TUNDRA_FOREST,
			PlotListTypes.HILLS_SNOW,
		};
	elseif resource_ID == self.copper_ID then
		tList = {
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.HILLS_DESERT_TUNDRA_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE,
			PlotListTypes.HILLS_SNOW,
			PlotListTypes.FLAT_SNOW,
		};
	elseif resource_ID == self.gems_ID then
		tList = {
			PlotListTypes.HILLS_DESERT_TUNDRA_NO_FEATURE,
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.HILLS_JUNGLE,
			PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_JUNGLE,
		};
	elseif resource_ID == self.salt_ID then
		tList = {
			PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_NO_FEATURE,
			PlotListTypes.HILLS_DESERT_TUNDRA_NO_FEATURE,
			PlotListTypes.HILLS_PLAINS_NO_FEATURE,
			PlotListTypes.FLAT_TUNDRA_PLAINS_COVERED,
			PlotListTypes.HILLS_TUNDRA_PLAINS_COVERED,
		};
	elseif resource_ID == self.jade_ID then
		tList = {
			PlotListTypes.HILLS_DESERT_TUNDRA_NO_FEATURE,
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.HILLS_COVERED,
			PlotListTypes.FLAT_COVERED,
		};
	elseif resource_ID == self.amber_ID then
		tList = {
			PlotListTypes.HILLS_DESERT_TUNDRA_NO_FEATURE,
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.HILLS_COVERED,
			PlotListTypes.FLAT_COVERED,
		};
	elseif resource_ID == self.lapis_ID then
		tList = {
			PlotListTypes.HILLS_DESERT_TUNDRA_NO_FEATURE,
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.HILLS_COVERED,
			PlotListTypes.FLAT_COVERED,
		};
	-- Tree Luxuries
	elseif resource_ID == self.cocoa_ID then
		tList = {
			PlotListTypes.FLAT_JUNGLE,
			PlotListTypes.FLAT_PLAINS_GRASS_FOREST,
			PlotListTypes.HILLS_PLAINS_GRASS_COVERED,
			PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.MARSH,
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
		};
	elseif resource_ID == self.citrus_ID then
		tList = {
			PlotListTypes.FLAT_JUNGLE,
			PlotListTypes.FLAT_PLAINS_GRASS_FOREST,
			PlotListTypes.HILLS_PLAINS_GRASS_COVERED,
			PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.MARSH,
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
		};
	elseif resource_ID == self.spices_ID then
		tList = {
			PlotListTypes.FLAT_JUNGLE,
			PlotListTypes.FLAT_PLAINS_GRASS_FOREST,
			PlotListTypes.HILLS_PLAINS_GRASS_COVERED,
			PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.MARSH,
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
		};
	elseif resource_ID == self.sugar_ID then
		-- Marsh and flood plains only. Flat jungle and grassland/plains are backups (to be handled in AdjustTiles).
		tList = {
			PlotListTypes.MARSH,
			PlotListTypes.FLOOD_PLAINS,
			PlotListTypes.FLAT_JUNGLE,
			PlotListTypes.FLAT_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_NO_FEATURE,
		};
	elseif resource_ID == self.truffles_ID then
		tList = {
			PlotListTypes.FLAT_PLAINS_GRASS_FOREST,
			PlotListTypes.FLAT_JUNGLE,
			PlotListTypes.MARSH,
			PlotListTypes.HILLS_PLAINS_GRASS_COVERED,
			PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
		};
	elseif resource_ID == self.silk_ID then
		tList = {
			PlotListTypes.FLAT_PLAINS_GRASS_FOREST,
			PlotListTypes.FLAT_JUNGLE,
			PlotListTypes.HILLS_PLAINS_GRASS_COVERED,
			PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.MARSH,
		};
	elseif resource_ID == self.dye_ID then
		tList = {
			PlotListTypes.FLAT_FOREST,
			PlotListTypes.FLAT_JUNGLE,
			PlotListTypes.HILLS_COVERED,
			PlotListTypes.FLAT_TUNDRA_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.HILLS_TUNDRA_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.MARSH,
		};
	elseif resource_ID == self.fur_ID then
		-- Flat forest tundra only. Hills/open tundra and snow are backups (to be handled in AdjustTiles).
		tList = {
			PlotListTypes.FLAT_TUNDRA_FOREST,
			PlotListTypes.FLAT_TUNDRA_NO_FEATURE,
			PlotListTypes.HILLS_TUNDRA_FOREST,
			PlotListTypes.HILLS_TUNDRA_NO_FEATURE,
			PlotListTypes.FLAT_SNOW,
			PlotListTypes.HILLS_SNOW,
		};
	-- Land Luxuries
	elseif resource_ID == self.incense_ID then
		-- Plains and desert only. Grassland is used as backup (to be handled in AdjustTiles).
		tList = {
			PlotListTypes.FLAT_DESERT_NO_OASIS,
			PlotListTypes.FLAT_PLAINS_NO_FEATURE,
			PlotListTypes.HILLS_DESERT_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_GRASS_COVERED,
			PlotListTypes.HILLS_PLAINS_GRASS_COVERED,
			PlotListTypes.FLAT_GRASS_NO_FEATURE,
		};
	elseif resource_ID == self.ivory_ID then
		-- Open flat plains only. Grassland is used as backup (to be handled in AdjustTiles).
		tList = {
			PlotListTypes.FLAT_PLAINS_DRY_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_WET_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_COVERED,
			PlotListTypes.FLAT_GRASS_DRY_NO_FEATURE,
			PlotListTypes.FLAT_GRASS_WET_NO_FEATURE,
			PlotListTypes.HILLS_PLAINS_NO_FEATURE,
		};
	elseif resource_ID == self.wine_ID then
		tList = {
			PlotListTypes.FLAT_PLAINS_NO_FEATURE,
			PlotListTypes.FLAT_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_GRASS_COVERED,
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.HILLS_PLAINS_GRASS_COVERED,
			PlotListTypes.MARSH,
		};
	elseif resource_ID == self.olives_ID then
		tList = {
			PlotListTypes.FLAT_PLAINS_NO_FEATURE,
			PlotListTypes.FLAT_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_GRASS_COVERED,
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.HILLS_PLAINS_GRASS_COVERED,
			PlotListTypes.MARSH,
		};
	elseif resource_ID == self.coffee_ID then
		tList = {
			PlotListTypes.FLAT_PLAINS_NO_FEATURE,
			PlotListTypes.FLAT_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_GRASS_COVERED,
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.HILLS_PLAINS_GRASS_COVERED,
			PlotListTypes.MARSH,
		};
	elseif resource_ID == self.tobacco_ID then
		tList = {
			PlotListTypes.FLAT_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_GRASS_COVERED,
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.HILLS_PLAINS_GRASS_COVERED,
			PlotListTypes.MARSH,
		};
	elseif resource_ID == self.tea_ID then
		tList = {
			PlotListTypes.FLAT_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_GRASS_COVERED,
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.HILLS_PLAINS_GRASS_COVERED,
			PlotListTypes.MARSH,
		};
	elseif resource_ID == self.perfume_ID then
		tList = {
			PlotListTypes.FLAT_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_GRASS_COVERED,
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.HILLS_PLAINS_GRASS_COVERED,
			PlotListTypes.MARSH,
		};
	elseif resource_ID == self.cotton_ID then
		tList = {
			PlotListTypes.FLAT_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_NO_FEATURE,
			PlotListTypes.FLAT_DESERT_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_GRASS_COVERED,
			PlotListTypes.HILLS_DESERT_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.HILLS_PLAINS_GRASS_COVERED,
		};
	-- MOD.HungryForFood: Start
	-- Even More Resources for Vox Populi
	elseif self:IsEvenMoreResourcesActive() then
		if resource_ID == self.lavender_ID then
			tList = {
				PlotListTypes.FLAT_GRASS_NO_FEATURE,
				PlotListTypes.FLAT_PLAINS_NO_FEATURE,
				PlotListTypes.FLAT_PLAINS_GRASS_COVERED,
				PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
				PlotListTypes.HILLS_PLAINS_GRASS_COVERED,
				PlotListTypes.MARSH,
			};
		elseif resource_ID == self.obsidian_ID then
			tList = {
				PlotListTypes.HILLS_DESERT_TUNDRA_NO_FEATURE,
				PlotListTypes.HILLS_JUNGLE,
				PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
				PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE,
				PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE,
				PlotListTypes.HILLS_COVERED,
			};
		elseif resource_ID == self.platinum_ID then
			tList = {
				PlotListTypes.HILLS_DESERT_TUNDRA_NO_FEATURE,
				PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
				PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE,
				PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE,
				PlotListTypes.HILLS_COVERED,
				PlotListTypes.FLAT_COVERED,
			};
		elseif resource_ID == self.poppy_ID then
			tList = {
				PlotListTypes.HILLS_NO_FEATURE,
				PlotListTypes.FLAT_DESERT_NO_OASIS,
				PlotListTypes.FLAT_PLAINS_NO_FEATURE,
				PlotListTypes.HILLS_DESERT_PLAINS_GRASS_NO_FEATURE,
				PlotListTypes.FLAT_PLAINS_GRASS_COVERED,
				PlotListTypes.HILLS_PLAINS_GRASS_COVERED,
			};
		elseif resource_ID == self.tin_ID then
			tList = {
				PlotListTypes.HILLS_DESERT_TUNDRA_NO_FEATURE,
				PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
				PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE,
				PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE,
				PlotListTypes.HILLS_COVERED,
				PlotListTypes.FLAT_COVERED,
			};
		end
	-- MOD.HungryForFood: End
	end
	-- print("Found indices of", tList);
	return tList;
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetRegionLuxuryTargetNumbers()
	-- This data was separated out to allow easy replacement in map scripts.

	-- This table, indexed by civ-count, provides the target amount of luxuries to place in each region.
	-- MOD.Barathor: Updated -- increased inital value when increasing total civ count by 2. Instead of decreasing by 2, it'll decrease copies of regional luxuries placed by 1.
	-- MOD.Barathor: Rough Example -- Standard 8 civs x 6 copies of each regional luxury = 48 ... 10 x 4 = 40 ... 10 x 5 = 50 ... 50 is closer to 48 than 40
	-- MOD.Barathor: This will not hurt the random luxury total to be placed since it always places a minimum number at least.
	local duel_values = table.fill(1, MAX_MAJOR_CIVS); -- Max is one per region for all player counts at this size.

	local tiny_values = {0, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
	local small_values = {0, 3, 3, 3, 4, 4, 4, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
	local standard_values = {0, 3, 3, 4, 4, 5, 5, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
	local large_values = {0, 3, 4, 4, 5, 5, 5, 6, 6, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
	local huge_values = {0, 4, 5, 5, 6, 6, 6, 6, 7, 7, 7, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = duel_values,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = tiny_values,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = small_values,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = standard_values,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = large_values,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = huge_values,
	};
	local target_list = worldsizes[Map.GetWorldSize()];
	return target_list;
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetWorldLuxuryTargetNumbers()
	-- This data was separated out to allow easy replacement in map scripts.

	-- The first number is the target for total luxuries in the world, NOT
	-- counting the one-per-civ "second type" added at start locations.

	-- The second number affects minimum number of random luxuries placed.
	-- I say "affects" because it is only one part of the formula.
	local worldsizes = {};

	local luxDensity = self.luxuryDensity;

	if self.luxuryDensity == 4 then
		luxDensity = 1 + Map.Rand(3, "Luxury Resource Density");
	end

	if luxDensity == 1 then -- Sparse
		worldsizes = {
			[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {14, 3},
			[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {24, 4},
			[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {36, 4},
			[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {48, 5},
			[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {60, 5},
			[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {76, 6}
		}
	elseif luxDensity == 3 then -- Abundant
		worldsizes = {
			[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {24, 3},
			[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {40, 4},
			[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {60, 4},
			[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {80, 5},
			[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {100, 5},
			[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {128, 6}
		}
	else -- Standard
		worldsizes = {
			[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {18, 3},
			[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {30, 4},
			[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {45, 4},
			[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {60, 5},
			[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {75, 5},
			[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {95, 6}
		}
	end
	local world_size_data = worldsizes[Map.GetWorldSize()];
	return world_size_data;
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetLowFertilityCompensations(iNumRegions)
	-- Extracted from PlaceLuxuries() for easy override
	-- No fertility compensations by default
	local region_low_fert_compensation = table.fill(0, iNumRegions); -- Stores number of luxury compensation each region received
	local luxury_low_fert_compensation = table.fill(0, self.MAX_RESOURCE_INDEX); -- Stores number of times each resource ID had extras handed out at civ starts. WARNING: Don't use ID 0.
	return region_low_fert_compensation, luxury_low_fert_compensation;
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceLuxuries(args)
	-- This function is dependent upon AssignLuxuryRoles() and PlaceCityStates() having been executed first.

	args = args or {};
	local iNumExtraRegions = args.iNumExtraRegions or 0;
	local region_low_fert_compensation, luxury_low_fert_compensation = self:GetLowFertilityCompensations(self.iNumCivs + iNumExtraRegions);

	-- Place Luxuries at civ start locations.
	for _, reg_data in ipairs(self.regions_sorted_by_type) do
		local region_number = reg_data[1];
		local this_region_luxury = reg_data[2];
		local x = self.startingPlots[region_number][1];
		local y = self.startingPlots[region_number][2];
		-- print("-"); print("Attempting to place Luxury#", this_region_luxury, "at start plot", x, y, "in Region#", region_number);
		-- Determine number to place at the start location
		local iNumLeftToPlace = 2;
		if self.legStart then -- Legendary Start
			iNumLeftToPlace = 3;
		end

		-- Low fertility regions get more starting luxuries (if enabled)
		iNumLeftToPlace = iNumLeftToPlace + region_low_fert_compensation[region_number];

		-- Obtain plot lists appropriate to this luxury type.
		local tList = self:GetIndicesForLuxuryType(this_region_luxury);

		-- First pass, checking only first two rings with a 50% ratio.
		local luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false);
		for _, index in ipairs(tList) do
			local shuf_list = luxury_plot_lists[index];
			if iNumLeftToPlace > 0 and index > 0 then
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
			end
			if iNumLeftToPlace <= 0 then
				break;
			end
		end

		if iNumLeftToPlace > 0 then
			-- Second pass, checking three rings with a 100% ratio.
			luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 3, false);
			for _, index in ipairs(tList) do
				local shuf_list = luxury_plot_lists[index];
				if iNumLeftToPlace > 0 and index > 0 then
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
				end
				if iNumLeftToPlace <= 0 then
					break;
				end
			end
		end

		if iNumLeftToPlace > 0 then
			-- If we haven't been able to place all of this lux type at the start, it CAN be placed in the region somewhere.
			-- Subtract remainder from this region's compensation, so that the regional process, later, will attempt to place this remainder somewhere in the region.
			luxury_low_fert_compensation[this_region_luxury] = luxury_low_fert_compensation[this_region_luxury] - iNumLeftToPlace;
			region_low_fert_compensation[region_number] = region_low_fert_compensation[region_number] - iNumLeftToPlace;
		end
		if iNumLeftToPlace > 0 and self.iNumTypesRandom > 0 then
			-- We'll attempt to place one source of a Luxury type assigned to random distribution.
			local randoms_to_place = 1;
			local tShuffledRandom = GetShuffledCopyOfTable(self.resourceIDs_assigned_to_random);
			for _, random_res in ipairs(tShuffledRandom) do
				tList = self:GetIndicesForLuxuryType(random_res);
				for _, index in ipairs(tList) do
					local shuf_list = luxury_plot_lists[index];
					if randoms_to_place > 0 and index > 0 then
						randoms_to_place = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
					end
					if randoms_to_place <= 0 then
						break;
					end
				end
				if randoms_to_place <= 0 then
					break;
				end
			end
		end
	end

	-- Place Luxuries at City States.
	-- Candidates include luxuries exclusive to CS, the lux assigned to this CS's region (if in a region), and the randoms.
	for city_state = 1, self.iNumCityStates do
		-- First check to see if this city state number received a valid start plot.
		if not self.city_state_validity_table[city_state] then
			-- This one did not! It does not exist on the map nor have valid data, so we will ignore it.
		else
			-- OK, it's a valid city state. Process it.
			local region_number = self.city_state_region_assignments[city_state];
			local x = self.cityStatePlots[city_state][1];
			local y = self.cityStatePlots[city_state][2];
			local allowed_luxuries = self:GetListOfAllowableLuxuriesAtCitySite(x, y, 2);
			local lux_possible_for_cs = {}; -- Recorded with ID as key, weighting as data entry
			-- Identify Allowable Luxuries assigned to City States.
			-- If any CS-Only types are eligible, then all combined will have a weighting of 75%
			local cs_only_types = {};
			for _, res_ID in ipairs(self.resourceIDs_assigned_to_cs) do
				if allowed_luxuries[res_ID] then
					table.insert(cs_only_types, res_ID);
				end
			end
			local iNumCSAllowed = table.maxn(cs_only_types);
			if iNumCSAllowed > 0 then
				for _, res_ID in ipairs(cs_only_types) do
					lux_possible_for_cs[res_ID] = 75 / iNumCSAllowed;
				end
			end
			-- Identify Allowable Random Luxuries and the Regional Luxury if any.
			-- If any random types are eligible (plus the regional type if in a region) these combined carry a 25% weighting.
			if self.iNumTypesRandom > 0 or region_number > 0 then
				local random_types_allowed = {};
				local tShuffledRandom = GetShuffledCopyOfTable(self.resourceIDs_assigned_to_random);
				for _, res_ID in ipairs(tShuffledRandom) do
					if allowed_luxuries[res_ID] then
						table.insert(random_types_allowed, res_ID);
					end
				end
				local iNumRandAllowed = table.maxn(random_types_allowed);
				local iNumAllowed = iNumRandAllowed;
				if region_number > 0 then
					iNumAllowed = iNumAllowed + 1; -- Adding the region type in to the mix with the random types.
					local res_ID = self.region_luxury_assignment[region_number];
					if allowed_luxuries[res_ID] then
						lux_possible_for_cs[res_ID] = 25 / iNumAllowed;
					end
				end
				if iNumRandAllowed > 0 then
					for _, res_ID in ipairs(random_types_allowed) do
						lux_possible_for_cs[res_ID] = 25 / iNumAllowed;
					end
				end
			end

			-- If there are no allowable luxury types at this city site, then this city state gets none.
			local iNumAvailableTypes = table.maxn(lux_possible_for_cs);
			if iNumAvailableTypes == 0 then
				-- print("City State #", city_state, "has poor land, ineligible to receive a Luxury resource.");
			else
				-- Calculate probability thresholds for each allowable luxury type.
				local res_threshold = {};
				local totalWeight, accumulatedWeight = 0, 0;
				for _, this_weight in pairs(lux_possible_for_cs) do
					totalWeight = totalWeight + this_weight;
				end
				for res_ID, this_weight in pairs(lux_possible_for_cs) do
					local threshold = (this_weight + accumulatedWeight) * 10000 / totalWeight;
					res_threshold[res_ID] = threshold;
					accumulatedWeight = accumulatedWeight + this_weight;
				end
				-- Choose luxury type.
				local use_this_ID;
				local diceroll = Map.Rand(10000, "Choose resource type - Assign Luxury To City State - Lua");
				for res_ID, threshold in pairs(res_threshold) do
					if diceroll < threshold then -- Choose this resource type.
						use_this_ID = res_ID;
						break;
					end
				end
				-- print("-"); print("-"); print("-Assigned Luxury Type", use_this_ID, "to City State#", city_state);
				-- Place luxury.
				local tList = self:GetIndicesForLuxuryType(use_this_ID);
				local luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false);
				local iNumLeftToPlace = 1;
				for _, index in ipairs(tList) do
					local shuf_list = luxury_plot_lists[index];
					if iNumLeftToPlace > 0 and index > 0 then
						iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
					end
					if iNumLeftToPlace <= 0 then
						-- print("-"); print("Placed Luxury ID#", use_this_ID, "at City State#", city_state, "in Region#", region_number, "located at Plot", x, y);
						break;
					end
				end
			end
		end
	end

	-- Calibrate number of luxuries per region to world size and number of civs present.
	-- The amount of lux per region should be at its highest when the number of civs in the game is closest to "default" for that map size.
	local target_list = self:GetRegionLuxuryTargetNumbers();
	local targetNum = target_list[self.iNumCivs];
	-- Adjust target number according to Resource Setting.
	if self.luxuryDensity == 1 then
		targetNum = targetNum - 2;
	elseif self.luxuryDensity == 3 then
		targetNum = targetNum + 2;
	elseif self.luxuryDensity == 4 then
		targetNum = targetNum - 2 + Map.Rand(5, "Luxury Resource Density");
	end

	-- Handle Special Case Luxuries here before regional and random ones
	self:PlaceIvory();
	self:PlaceMarble();

	-- Place Regional Luxuries
	for region_number, res_ID in ipairs(self.region_luxury_assignment) do
		-- print("-"); print("- - -"); print("Attempting to place regional luxury #", res_ID, "in Region#", region_number);
		local tList = self:GetIndicesForLuxuryType(res_ID);
		local luxury_plot_lists = self:GenerateLuxuryPlotListsInRegion(region_number);
		local assignment_split = self.luxury_assignment_count[res_ID];

		local iTarget = targetNum + math.floor((0.5 * luxury_low_fert_compensation[res_ID]) / assignment_split);
		iTarget = iTarget - region_low_fert_compensation[region_number];

		local iNumLeftToPlace = math.max(1, iTarget); -- Always place at least one.

		-- print("-"); print("Target number for Luxury#", res_ID, "with assignment split of", self.luxury_assignment_count[res_ID], "is", targetNum);

		for _, index in ipairs(tList) do
			local shuf_list = luxury_plot_lists[index];
			if iNumLeftToPlace > 0 and index > 0 then
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, ImpactLayers.LAYER_LUXURY, 1, 2, shuf_list);
			end
			if iNumLeftToPlace <= 0 then
				break;
			end
		end

		-- print("-"); print("-"); print("Number of LuxuryID", res_ID, "left to place in Region#", region_number, "is", iNumLeftToPlace);

		if iNumLeftToPlace > 0 then
			for _, index in ipairs(tList) do
				local shuf_list = luxury_plot_lists[index];
				if iNumLeftToPlace > 0 and index > 0 then
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, ImpactLayers.LAYER_LUXURY, 1, 2, shuf_list);
				end
				if iNumLeftToPlace <= 0 then
					break;
				end
			end
			print("-"); print("Number of LuxuryID", res_ID, "not placed in Region#", region_number, "is", iNumLeftToPlace);
		end
	end

	-- Place Random Luxuries
	if self.iNumTypesRandom > 0 then
		-- print("* *"); print("* iNumTypesRandom = ", self.iNumTypesRandom); print("* *");
		-- This table governs targets for total number of luxuries placed in the world, not including the "extra types" of Luxuries placed at start locations.
		-- These targets are approximate. An additional random factor is added in based on number of civs.
		-- Any difference between regional and city state luxuries placed, and the target, is made up for with the number of randomly placed luxuries that get distributed.
		local world_size_data = self:GetWorldLuxuryTargetNumbers();

		local targetLuxForThisWorldSize = world_size_data[1];
		local loopTarget = world_size_data[2];
		local extraLux = Map.Rand(self.iNumCivs, "Luxury Resource Variance - Place Resources LUA");
		local iNumRandomLuxTarget = targetLuxForThisWorldSize + extraLux - self.totalLuxPlacedSoFar;
		--[[ MOD.Barathor: Test
		print("* *"); print("* targetLuxForThisWorldSize = ", targetLuxForThisWorldSize); print("* *");
		print("* *"); print("* random to add to target = ", extraLux); print("* *");
		print("* *"); print("* totalLuxPlacedSoFar = ", self.totalLuxPlacedSoFar); print("* *");
		print("* *"); print("* iNumRandomLuxTarget = ", iNumRandomLuxTarget); print("* *");
		--]]
		local iNumRandomLuxPlaced, iNumThisLuxToPlace = 0, 0;
		-- This table weights the amount of random luxuries to place, with first-selected getting heavier weighting.
		local random_lux_ratios_table = {
			{1},
			{0.55, 0.45},
			{0.40, 0.33, 0.27},
			{0.35, 0.25, 0.25, 0.15},
			{0.25, 0.25, 0.20, 0.15, 0.15},
			{0.20, 0.20, 0.20, 0.15, 0.15, 0.10},
			{0.20, 0.20, 0.15, 0.15, 0.10, 0.10, 0.10},
			{0.20, 0.15, 0.15, 0.10, 0.10, 0.10, 0.10, 0.10},
		};

		local tShuffledRandom = GetShuffledCopyOfTable(self.resourceIDs_assigned_to_random);
		for loop, res_ID in ipairs(tShuffledRandom) do
			local tList = self:GetIndicesForLuxuryType(res_ID);
			if self.iNumTypesRandom > 8 then
				iNumThisLuxToPlace = math.max(3, math.ceil(iNumRandomLuxTarget / 10));
			else
				local lux_minimum = math.max(3, loopTarget - loop);
				local lux_share_of_remaining = math.ceil(iNumRandomLuxTarget * random_lux_ratios_table[self.iNumTypesRandom][loop]);
				iNumThisLuxToPlace = math.max(lux_minimum, lux_share_of_remaining);
			end
			local iNumLeftToPlace = iNumThisLuxToPlace;

			-- Place this luxury type.
			for _, index in ipairs(tList) do
				local current_list = self.global_resource_plot_lists[index];
				if iNumLeftToPlace > 0 and index > 0 then
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, ImpactLayers.LAYER_LUXURY, 4, 6, current_list);
				end
				if iNumLeftToPlace <= 0 then
					break;
				end
			end
			iNumRandomLuxPlaced = iNumRandomLuxPlaced + iNumThisLuxToPlace - iNumLeftToPlace;
			--[[ MOD.Barathor: Test
			print("-");
			print("Random Luxury ID#:", res_ID);
			print("-"); print("Random Luxury Target Number:", iNumThisLuxToPlace);
			print("Random Luxury Target Placed:", iNumThisLuxToPlace - iNumLeftToPlace); print("-");
			--]]
		end
		--[[
		print("-"); print("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+");
		print("+ Random Luxuries Target Number:", iNumRandomLuxTarget);
		print("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+");
		print("+ Random Luxuries Number Placed:", iNumRandomLuxPlaced);
		print("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+"); print("-");
		--]]
	end

	-- For Resource settings other than Sparse, add a second luxury type at start locations.
	-- This second type will be selected from Random types if possible, CS types if necessary, and other regions' types as a final fallback.
	if self.luxuryDensity ~= 1 then
		for region_number = 1, self.iNumCivs do
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];
			local use_this_ID;
			local candidate_types, iNumTypesAllowed = {}, 0;
			local allowed_luxuries = self:GetListOfAllowableLuxuriesAtCitySite(x, y, 2);
			-- print("-"); print("--- Eligible Types List for Second Luxury in Region#", region_number, "---");
			-- See if any Random types are eligible.
			for _, res_ID in ipairs(self.resourceIDs_assigned_to_random) do
				if allowed_luxuries[res_ID] then
					-- print("- Found eligible luxury type:", res_ID);
					iNumTypesAllowed = iNumTypesAllowed + 1;
					table.insert(candidate_types, res_ID);
				end
			end

			if iNumTypesAllowed == 0 then
				-- See if any City State types are eligible.
				for _, res_ID in ipairs(self.resourceIDs_assigned_to_cs) do
					if allowed_luxuries[res_ID] then
						-- print("- Found eligible luxury type:", res_ID);
						iNumTypesAllowed = iNumTypesAllowed + 1;
						table.insert(candidate_types, res_ID);
					end
				end
			end

			if iNumTypesAllowed == 0 then
				-- See if anybody else's regional type is eligible.
				local region_lux_ID = self.region_luxury_assignment[region_number];
				for _, res_ID in ipairs(self.resourceIDs_assigned_to_regions) do
					if res_ID ~= region_lux_ID then
						if allowed_luxuries[res_ID] then
							-- print("- Found eligible luxury type:", res_ID);
							iNumTypesAllowed = iNumTypesAllowed + 1;
							table.insert(candidate_types, res_ID);
						end
					end
				end
			end

			if iNumTypesAllowed > 0 then
				local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
				use_this_ID = candidate_types[diceroll];
			else
				print("-"); print("Failed to place second Luxury type at start in Region#", region_number, "-- no eligible types!"); print("-");
			end
			-- print("--- End of Eligible Types list for Second Luxury in Region#", region_number, "---");

			if use_this_ID then -- Place this luxury type at this start.
				local tList = self:GetIndicesForLuxuryType(use_this_ID);
				local luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false);
				local iNumLeftToPlace = 1;
				for _, index in ipairs(tList) do
					local shuf_list = luxury_plot_lists[index];
					if iNumLeftToPlace > 0 and index > 0 then
						iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
					end
					if iNumLeftToPlace <= 0 then
						-- print("-"); print("Placed Second Luxury type of ID#", use_this_ID, "for start located at Plot", x, y, " in Region#", region_number);
						break;
					end
				end
			end
		end
	end

	self.realtotalLuxPlacedSoFar = self.totalLuxPlacedSoFar; -- MOD.Barathor: New -- save the real total of luxuries before it gets corrupted with non-luxury additions which use the luxury placement method
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceMarble()
	local marble_already_placed = self.amounts_of_resources_placed[self.marble_ID + 1];
	local marble_target = math.ceil(self.iNumCivs * 0.75);
	if self.luxuryDensity == 1 then
		marble_target = math.ceil(self.iNumCivs * 0.5);
	elseif self.luxuryDensity == 3 then
		marble_target = math.ceil(self.iNumCivs * 1.0);
	elseif self.luxuryDensity == 4 then
		marble_target = self:GetRandomFromRangeInclusive(math.ceil(self.iNumCivs * 0.5), math.ceil(self.iNumCivs * 1.0));
	end
	local iNumMarbleToPlace = math.max(2, marble_target - marble_already_placed);
	local tPlotList = self.global_resource_plot_lists[PlotListTypes.MARBLE];
	local iNumPlots = table.maxn(tPlotList);
	if iNumPlots < 1 then
		print("No eligible plots available to place Marble!");
		return;
	end
	local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.marble_ID, 1, iNumMarbleToPlace, 1, ImpactLayers.LAYER_LUXURY, 1, 1, tPlotList);
	if iNumLeftToPlace > 0 then
		print("Failed to place", iNumLeftToPlace, "units of Marble.");
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceIvory()
	local ivory_already_placed = self.amounts_of_resources_placed[self.ivory_ID + 1];
	local ivory_target = math.ceil(self.iNumCivs * 0.75);
	if self.luxuryDensity == 1 then
		ivory_target = math.ceil(self.iNumCivs * 0.5);
	elseif self.luxuryDensity == 3 then
		ivory_target = math.ceil(self.iNumCivs * 1.0);
	elseif self.luxuryDensity == 4 then
		ivory_target = self:GetRandomFromRangeInclusive(math.ceil(self.iNumCivs * 0.5), math.ceil(self.iNumCivs * 1.0));
	end
	local iNumLeftToPlace = math.max(2, ivory_target - ivory_already_placed);

	-- Unlike Marble, Ivory has more restrictions, so we're using priority lists here
	local tList = self:GetIndicesForLuxuryType(self.ivory_ID);
	for _, index in ipairs(tList) do
		local current_list = self.global_resource_plot_lists[index];
		if iNumLeftToPlace > 0 and index > 0 then
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.ivory_ID, 1, iNumLeftToPlace, 1, ImpactLayers.LAYER_LUXURY, 1, 1, current_list);
		end
		if iNumLeftToPlace <= 0 then
			break;
		end
	end

	if iNumLeftToPlace > 0 then
		print("Failed to place", iNumLeftToPlace, "units of Ivory.");
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceSmallQuantitiesOfStrategics(frequency, plot_list)
	-- This function distributes small quantities of strategic resources.
	if not plot_list then
		print("No strategics were placed! -SmallQuantities");
		return;
	end
	local iW, _ = Map.GetGridSize();
	local iNumTotalPlots = table.maxn(plot_list);
	local iNumToPlace = math.ceil(iNumTotalPlots / frequency);

	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = self:GetSmallStrategicResourceQuantityValues();

	-- Main loop
	local current_index = 1;
	for _ = 1, iNumToPlace do
		local placed_this_res = false;
		if current_index <= iNumTotalPlots then
			for index_to_check = current_index, iNumTotalPlots do
				if placed_this_res then
					break;
				else
					current_index = current_index + 1;
				end
				local plotIndex = plot_list[index_to_check];
				if self:CountTotalStrategicResourceImpact(plotIndex) == 0 then
					local x = (plotIndex - 1) % iW;
					local y = (plotIndex - x - 1) / iW;
					local res_plot = Map.GetPlot(x, y);
					if res_plot:GetResourceType() == -1 then
						-- Placing a small strategic resource here. Need to determine what type to place.
						local selected_ID = -1;
						local selected_quantity = 2;
						local plotType = res_plot:GetPlotType();
						local terrainType = res_plot:GetTerrainType();
						local featureType = res_plot:GetFeatureType();
						if featureType == FeatureTypes.FEATURE_MARSH or featureType == FeatureTypes.FEATURE_JUNGLE then
							local diceroll = Map.Rand(5, "Resource selection - Place Small Quantities LUA");
							if diceroll < 1 or plotType == PlotTypes.PLOT_HILLS then
								selected_ID = self.uranium_ID;
								selected_quantity = uran_amt;
							else
								selected_ID = self.oil_ID;
								selected_quantity = oil_amt;
							end
						elseif featureType == FeatureTypes.FEATURE_FOREST then
							if terrainType == TerrainTypes.TERRAIN_GRASS or terrainType == TerrainTypes.TERRAIN_PLAINS then
								local diceroll = Map.Rand(10, "Resource selection - Place Small Quantities LUA");
								if diceroll < 2 then
									selected_ID = self.uranium_ID;
									selected_quantity = uran_amt;
								elseif diceroll < 5 then
									selected_ID = self.coal_ID;
									selected_quantity = coal_amt;
								else
									selected_ID = self.iron_ID;
									selected_quantity = iron_amt;
								end
							else
								local diceroll = Map.Rand(10, "Resource selection - Place Small Quantities LUA");
								if diceroll < 2 then
									selected_ID = self.uranium_ID;
									selected_quantity = uran_amt;
								else
									selected_ID = self.iron_ID;
									selected_quantity = iron_amt;
								end
							end
						elseif featureType == FeatureTypes.NO_FEATURE then
							if plotType == PlotTypes.PLOT_HILLS then
								if terrainType == TerrainTypes.TERRAIN_GRASS then
									local diceroll = Map.Rand(10, "Resource selection - Place Small Quantities LUA");
									if diceroll < 1 then
										selected_ID = self.uranium_ID;
										selected_quantity = uran_amt;
									elseif diceroll < 6 then
										selected_ID = self.iron_ID;
										selected_quantity = iron_amt;
									else
										selected_ID = self.coal_ID;
										selected_quantity = coal_amt;
									end
								elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
									local diceroll = Map.Rand(10, "Resource selection - Place Small Quantities LUA");
									if diceroll < 1 then
										selected_ID = self.uranium_ID;
										selected_quantity = uran_amt;
									elseif diceroll < 4 then
										selected_ID = self.iron_ID;
										selected_quantity = iron_amt;
									elseif diceroll < 7 then
										selected_ID = self.coal_ID;
										selected_quantity = coal_amt;
									else
										selected_ID = self.aluminum_ID;
										selected_quantity = alum_amt;
									end
								else
									local diceroll = Map.Rand(10, "Resource selection - Place Small Quantities LUA");
									if diceroll < 1 then
										selected_ID = self.uranium_ID;
										selected_quantity = uran_amt;
									elseif diceroll < 5 then
										selected_ID = self.iron_ID;
										selected_quantity = iron_amt;
									else
										selected_ID = self.aluminum_ID;
										selected_quantity = alum_amt;
									end
								end
							elseif terrainType == TerrainTypes.TERRAIN_GRASS then
								if res_plot:IsFreshWater() then
									local diceroll = Map.Rand(10, "Resource selection - Place Small Quantities LUA");
									if diceroll < 1 then
										selected_ID = self.uranium_ID;
										selected_quantity = uran_amt;
									elseif diceroll < 3 then
										selected_ID = self.iron_ID;
										selected_quantity = iron_amt;
									else
										selected_ID = self.horse_ID;
										selected_quantity = horse_amt;
									end
								else
									local diceroll = Map.Rand(10, "Resource selection - Place Small Quantities LUA");
									if diceroll < 1 then
										selected_ID = self.uranium_ID;
										selected_quantity = uran_amt;
									elseif diceroll < 4 then
										selected_ID = self.iron_ID;
										selected_quantity = iron_amt;
									elseif diceroll < 7 then
										selected_ID = self.coal_ID;
										selected_quantity = coal_amt;
									else
										selected_ID = self.horse_ID;
										selected_quantity = horse_amt;
									end
								end
							elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
								local diceroll = Map.Rand(10, "Resource selection - Place Small Quantities LUA");
								if diceroll < 1 then
									selected_ID = self.uranium_ID;
									selected_quantity = uran_amt;
								elseif diceroll < 3 then
									selected_ID = self.iron_ID;
									selected_quantity = iron_amt;
								elseif diceroll < 5 then
									selected_ID = self.coal_ID;
									selected_quantity = coal_amt;
								elseif diceroll < 7 then
									selected_ID = self.horse_ID;
									selected_quantity = horse_amt;
								else
									selected_ID = self.aluminum_ID;
									selected_quantity = alum_amt;
								end
							elseif terrainType == TerrainTypes.TERRAIN_DESERT then
								if res_plot:IsFreshWater() then
									selected_ID = self.horse_ID;
									selected_quantity = horse_amt;
								else
									local diceroll = Map.Rand(10, "Resource selection - Place Small Quantities LUA");
									if diceroll < 1 then
										selected_ID = self.uranium_ID;
										selected_quantity = uran_amt;
									elseif diceroll < 3 then
										selected_ID = self.iron_ID;
										selected_quantity = iron_amt;
									elseif diceroll < 6 then
										selected_ID = self.oil_ID;
										selected_quantity = oil_amt;
									else
										selected_ID = self.aluminum_ID;
										selected_quantity = alum_amt;
									end
								end
							elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
								local diceroll = Map.Rand(10, "Resource selection - Place Small Quantities LUA");
								if diceroll < 1 then
									selected_ID = self.uranium_ID;
									selected_quantity = uran_amt;
								elseif diceroll < 6 then
									selected_ID = self.iron_ID;
									selected_quantity = iron_amt;
								elseif diceroll < 9 then
									selected_ID = self.horse_ID;
									selected_quantity = horse_amt;
								else
									selected_ID = self.oil_ID;
									selected_quantity = oil_amt;
								end
							else
								local diceroll = Map.Rand(10, "Resource selection - Place Small Quantities LUA");
								if diceroll < 3 then
									selected_ID = self.uranium_ID;
									selected_quantity = uran_amt;
								elseif diceroll < 6 then
									selected_ID = self.iron_ID;
									selected_quantity = iron_amt;
								else
									selected_ID = self.oil_ID;
									selected_quantity = oil_amt;
								end
							end
						end
						-- Now place the resource, then impact the strategic data layer.
						if selected_ID ~= -1 then
							local strat_radius = Map.Rand(4, "Resource Radius - Place Small Quantities LUA");
							if strat_radius > 2 then
								strat_radius = 1;
							end
							res_plot:SetResourceType(selected_ID, selected_quantity);
							self:PlaceStrategicResourceImpact(x, y, strat_radius);
							placed_this_res = true;
							-- print("Placed resource", selected_ID, "with amount", selected_quantity);
							self.amounts_of_resources_placed[selected_ID + 1] = self.amounts_of_resources_placed[selected_ID + 1] + selected_quantity;
						end
					end
				end
			end
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceFish(frequency, plot_list)
	-- This function places fish at members of plot_list. (Sounds fishy to me!)
	if not plot_list then
		print("No fish were placed! -PlaceFish");
		return;
	end
	local iW, _ = Map.GetGridSize();
	local iNumTotalPlots = table.maxn(plot_list);
	local iNumFishToPlace = math.ceil(iNumTotalPlots / frequency);
	-- Main loop
	local current_index = 1;
	for _ = 1, iNumFishToPlace do
		local placed_this_res = false;
		if current_index <= iNumTotalPlots then
			for index_to_check = current_index, iNumTotalPlots do
				if placed_this_res then
					break;
				else
					current_index = current_index + 1;
				end
				local plotIndex = plot_list[index_to_check];
				if self.impactData[ImpactLayers.LAYER_FISH][plotIndex] == 0 then
					local x = (plotIndex - 1) % iW;
					local y = (plotIndex - x - 1) / iW;
					local res_plot = Map.GetPlot(x, y);
					if res_plot:GetResourceType() == -1 then
						-- Placing fish here. First decide impact radius of this fish.
						local fish_radius = Map.Rand(7, "Fish Radius - Place Fish LUA");
						if fish_radius > 5 then
							fish_radius = 3;
						end
						res_plot:SetResourceType(self.fish_ID, 1);
						self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_FISH, fish_radius);
						placed_this_res = true;
						self.amounts_of_resources_placed[self.fish_ID + 1] = self.amounts_of_resources_placed[self.fish_ID + 1] + 1;
					end
				end
			end
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceSexyBonusAtCivStarts()
	-- This function will place a Bonus resource in the third ring around a Civ's start.
	-- The added Bonus is meant to make the start look more sexy, so to speak.
	-- Third-ring resources will take a long time to bring online, but will assist the site in the late game.
	-- Alternatively, it may assist a different city if another city is settled close enough to the capital and takes control of this tile.
	local iW, iH = Map.GetGridSize();
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local odd = self.firstRingYIsOdd;
	local even = self.firstRingYIsEven;
	local nextX, nextY, plot_adjustments;

	local bonus_type_associated_with_region_type = {self.deer_ID, self.banana_ID,
	self.deer_ID, self.wheat_ID, self.sheep_ID, self.wheat_ID, self.cow_ID, self.cow_ID,
	self.sheep_ID, self.deer_ID};
	local bonus_type_associated_with_tropical_region_type = {self.deer_ID, self.banana_ID,
	self.deer_ID, self.maize_ID, self.sheep_ID, self.maize_ID, self.rice_ID, self.cow_ID,
	self.sheep_ID, self.deer_ID};

	for region_number = 1, self.iNumCivs do
		local x = self.startingPlots[region_number][1];
		local y = self.startingPlots[region_number][2];
		local region_type = self.regionTypes[region_number];
		local use_this_ID = bonus_type_associated_with_region_type[region_type];
		if self:IsTropical(y) then
			use_this_ID = bonus_type_associated_with_tropical_region_type[region_type];
		end
		local plot_list, fish_list = {}, {};
		-- For notes on how the hex-iteration works, refer to PlaceResourceImpact()
		local ripple_radius = 3;
		local currentX = x - ripple_radius;
		local currentY = y;
		for direction_index = 1, 6 do
			for _ = 1, ripple_radius do
			 	if currentY / 2 > math.floor(currentY / 2) then
					plot_adjustments = odd[direction_index];
				else
					plot_adjustments = even[direction_index];
				end
				nextX = currentX + plot_adjustments[1];
				nextY = currentY + plot_adjustments[2];
				if not wrapX and (nextX < 0 or nextX >= iW) then
					-- X is out of bounds.
				elseif not wrapY and (nextY < 0 or nextY >= iH) then
					-- Y is out of bounds.
				else
					local realX = nextX;
					local realY = nextY;
					if wrapX then
						realX = realX % iW;
					end
					if wrapY then
						realY = realY % iH;
					end
					-- We've arrived at the correct x and y for the current plot.
					local plot = Map.GetPlot(realX, realY);
					local featureType = plot:GetFeatureType();
					if plot:GetResourceType() == -1 and featureType ~= FeatureTypes.FEATURE_OASIS then -- No resource or Oasis here, safe to proceed.
						local plotType = plot:GetPlotType();
						local terrainType = plot:GetTerrainType();
						local plotIndex = realY * iW + realX + 1;
						-- Now check this plot for eligibility for the applicable Bonus type for this region.
						if use_this_ID == self.deer_ID then
							if featureType == FeatureTypes.FEATURE_FOREST then
								table.insert(plot_list, plotIndex);
							elseif terrainType == TerrainTypes.TERRAIN_TUNDRA and plotType == PlotTypes.PLOT_LAND then
								table.insert(plot_list, plotIndex);
							end
						elseif use_this_ID == self.banana_ID then
							if featureType == FeatureTypes.FEATURE_JUNGLE then
								table.insert(plot_list, plotIndex);
							end
						elseif use_this_ID == self.wheat_ID then
							if plotType == PlotTypes.PLOT_LAND then
								if terrainType == TerrainTypes.TERRAIN_PLAINS and featureType == FeatureTypes.NO_FEATURE then
									table.insert(plot_list, plotIndex);
								elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
									table.insert(plot_list, plotIndex);
								elseif terrainType == TerrainTypes.TERRAIN_DESERT and plot:IsFreshWater() then
									table.insert(plot_list, plotIndex);
								end
							end
						elseif use_this_ID == self.sheep_ID then
							if plotType == PlotTypes.PLOT_HILLS and featureType == FeatureTypes.NO_FEATURE then
								if terrainType == TerrainTypes.TERRAIN_PLAINS or terrainType == TerrainTypes.TERRAIN_GRASS or terrainType == TerrainTypes.TERRAIN_TUNDRA then
									table.insert(plot_list, plotIndex);
								end
							end
						elseif use_this_ID == self.cow_ID then
							if terrainType == TerrainTypes.TERRAIN_GRASS and plotType == PlotTypes.PLOT_LAND then
								if featureType == FeatureTypes.NO_FEATURE then
									table.insert(plot_list, plotIndex);
								end
							end
						elseif use_this_ID == self.maize_ID then
							if plotType == PlotTypes.PLOT_LAND then
								if terrainType == TerrainTypes.TERRAIN_GRASS or terrainType == TerrainTypes.TERRAIN_PLAINS then
									if featureType == FeatureTypes.NO_FEATURE then
										table.insert(plot_list, plotIndex);
									end
								elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
									table.insert(plot_list, plotIndex);
								elseif terrainType == TerrainTypes.TERRAIN_DESERT and plot:IsFreshWater() then
									table.insert(plot_list, plotIndex);
								end
							end
						elseif use_this_ID == self.rice_ID then
							if terrainType == TerrainTypes.TERRAIN_GRASS and featureType == FeatureTypes.NO_FEATURE and plotType == PlotTypes.PLOT_LAND then
								table.insert(plot_list, plotIndex);
							elseif featureType == FeatureTypes.FEATURE_MARSH then
								table.insert(plot_list, plotIndex);
							end
						end
						if plotType == PlotTypes.PLOT_OCEAN then
							if not plot:IsLake() then
								if featureType ~= FeatureTypes.FEATURE_ATOLL and featureType ~= FeatureTypes.FEATURE_ICE then
									if terrainType == TerrainTypes.TERRAIN_COAST then
										table.insert(fish_list, plotIndex);
									end
								end
							end
						end
					end
				end
				currentX, currentY = nextX, nextY;
			end
		end
		local iNumCandidates = table.maxn(plot_list);
		if iNumCandidates > 0 then
			-- print("Placing 'sexy Bonus' in third ring of start location in Region#", region_number);
			local shuf_list = GetShuffledCopyOfTable(plot_list);
			self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
			if iNumCandidates > 1 and use_this_ID == self.sheep_ID then
				-- Hills region, attempt to give them a second Sexy Sheep.
				-- print("Placing a second 'sexy Sheep' in third ring of start location in Hills Region#", region_number);
				self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
			end
		else
			local iFishCandidates = table.maxn(fish_list);
			if iFishCandidates > 0 then
				-- print("Placing 'sexy Fish' in third ring of start location in Region#", region_number);
				local shuf_list = GetShuffledCopyOfTable(fish_list);
				self:PlaceSpecificNumberOfResources(self.fish_ID, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
			end
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:AddExtraBonusesToHillsRegions()
	-- Hills/Mountain regions are very low on food, yet not deemed by the fertility measurements to be so.
	-- Spreading some food bonus around in these regions will help bring them up closer to par.
	local iW, iH = Map.GetGridSize();
	-- Identify Hills Regions, if any.
	local hills_regions, iNumHillsRegions = {}, 0;
	for region_number = 1, self.iNumCivs do
		if self.regionTypes[region_number] == RegionTypes.REGION_HILLS or
			self.regionTypes[region_number] == RegionTypes.REGION_MOUNTAIN then
			iNumHillsRegions = iNumHillsRegions + 1;
			table.insert(hills_regions, region_number);
		end
	end
	if iNumHillsRegions == 0 then -- We're done.
		return;
	end
	-- Process Hills Regions
	local shuffled_hills_regions = GetShuffledCopyOfTable(hills_regions);
	for _, region_number in ipairs(shuffled_hills_regions) do
		local iWestX = self.regionData[region_number][1];
		local iSouthY = self.regionData[region_number][2];
		local iWidth = self.regionData[region_number][3];
		local iHeight = self.regionData[region_number][4];
		local iAreaID = self.regionData[region_number][5];

		local terrainCounts = self.regionTerrainCounts[region_number];
		-- local totalPlots = terrainCounts[1];
		local areaPlots = terrainCounts[2];
		-- local waterCount = terrainCounts[3];
		local flatlandsCount = terrainCounts[4];
		local hillsCount = terrainCounts[5];
		local peaksCount = terrainCounts[6];
		-- local lakeCount = terrainCounts[7];
		-- local coastCount = terrainCounts[8];
		-- local oceanCount = terrainCounts[9];
		-- local iceCount = terrainCounts[10];
		local grassCount = terrainCounts[11];
		local plainsCount = terrainCounts[12];
		-- local desertCount = terrainCounts[13];
		-- local tundraCount = terrainCounts[14];
		-- local snowCount = terrainCounts[15];
		-- local forestCount = terrainCounts[16];
		-- local jungleCount = terrainCounts[17];
		-- local marshCount = terrainCounts[18];
		-- local riverCount = terrainCounts[19];
		-- local floodplainCount = terrainCounts[20];
		-- local oasisCount = terrainCounts[21];
		-- local coastalLandCount = terrainCounts[22];
		-- local nextToCoastCount = terrainCounts[23];

		-- Check how badly infertile the region is by comparing hills and mountains to flat farmlands.
		local hills_ratio = (hillsCount + peaksCount) / areaPlots;
		local farm_ratio = (grassCount + plainsCount) / areaPlots;
		if self.method == RegionDivision.RECTANGULAR then -- Need to ignore water tiles, which are included in areaPlots with this regional division method.
			hills_ratio = (hillsCount + peaksCount) / (hillsCount + peaksCount + flatlandsCount);
			farm_ratio = (grassCount + plainsCount) / (hillsCount + peaksCount + flatlandsCount);
		end
		-- If the infertility quotient is greater than 1, this will increase how
		-- many Bonus get placed, up to a max of double the normal ratio.
		local infertility_quotient = 1 + math.max(0, hills_ratio - farm_ratio);

		print("Infertility Quotient for Hills Region#", region_number, " is:", infertility_quotient);

		-- Generate plot lists for the extra Bonus placements.
		local dry_hills, flat_plains, flat_tropical_plains, flat_grass, flat_tropical_grass, flat_tundra, jungles, forests = {}, {}, {}, {}, {}, {}, {}, {};
		for region_loop_y = 0, iHeight - 1 do
			for region_loop_x = 0, iWidth - 1 do
				local x = (region_loop_x + iWestX) % iW;
				local y = (region_loop_y + iSouthY) % iH;
				local plot = Map.GetPlot(x, y);
				local plotIndex = y * iW + x + 1;
				local area_of_plot = plot:GetArea();
				local landmass_of_plot = plot:GetLandmass();
				local plotType = plot:GetPlotType();
				local terrainType = plot:GetTerrainType();
				local featureType = plot:GetFeatureType();
				if plotType == PlotTypes.PLOT_LAND or plotType == PlotTypes.PLOT_HILLS then
					-- Check plot for region membership. Only process this plot if it is a member.
					if iAreaID == -1 or (self.bArea and area_of_plot == iAreaID) or (not self.bArea and landmass_of_plot == iAreaID) then
						if plot:GetResourceType() == -1 then
							if featureType == FeatureTypes.FEATURE_JUNGLE then
								table.insert(jungles, plotIndex);
							elseif featureType == FeatureTypes.FEATURE_FOREST then
								table.insert(forests, plotIndex);
							elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
								table.insert(flat_plains, plotIndex);
							elseif featureType == FeatureTypes.NO_FEATURE then
								if plotType == PlotTypes.PLOT_HILLS then
									if terrainType == TerrainTypes.TERRAIN_GRASS or terrainType == TerrainTypes.TERRAIN_PLAINS or terrainType == TerrainTypes.TERRAIN_TUNDRA then
										if not plot:IsFreshWater() then
											table.insert(dry_hills, plotIndex);
										end
									end
								elseif plotType == PlotTypes.PLOT_LAND then
									if terrainType == TerrainTypes.TERRAIN_PLAINS or terrainType == TerrainTypes.TERRAIN_DESERT and plot:IsFreshWater() then
										if self:IsTropical(y) then
											table.insert(flat_tropical_plains, plotIndex);
										else
											table.insert(flat_plains, plotIndex);
										end
									elseif terrainType == TerrainTypes.TERRAIN_GRASS then
										if self:IsTropical(y) then
											table.insert(flat_tropical_grass, plotIndex);
										else
											table.insert(flat_grass, plotIndex);
										end
									elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
										table.insert(flat_tundra, plotIndex);
									end
								end
							end
						end
					end
				end
			end
		end

		--[[
		print("-"); print("--- Extra-Bonus Plot Counts for Hills Region#", region_number, "---");
		print("- Jungles:", table.maxn(jungles));
		print("- Forests:", table.maxn(forests));
		print("- Tundra:", table.maxn(flat_tundra));
		print("- Plains:", table.maxn(flat_plains));
		print("- Grass:", table.maxn(flat_grass));
		print("- Tropical Plains:", table.maxn(flat_tropical_plains));
		print("- Tropical Grass:", table.maxn(flat_tropical_grass));
		print("- Dry Hills:", table.maxn(dry_hills));
		--]]

		-- Now that the plot lists are ready, place the Bonuses.
		if table.maxn(dry_hills) > 0 then
			local resources_to_place = {
			{self.sheep_ID, 1, 100, 0, 1} };
			self:ProcessResourceList(16 / infertility_quotient, ImpactLayers.LAYER_BONUS, dry_hills, resources_to_place);
		end
		if table.maxn(jungles) > 0 then
			local resources_to_place = {
			{self.banana_ID, 1, 100, 1, 2} };
			self:ProcessResourceList(14 / infertility_quotient, ImpactLayers.LAYER_BONUS, jungles, resources_to_place);
		end
		if table.maxn(flat_tundra) > 0 then
			local resources_to_place = {
			{self.deer_ID, 1, 100, 0, 1} };
			self:ProcessResourceList(14 / infertility_quotient, ImpactLayers.LAYER_BONUS, flat_tundra, resources_to_place);
		end
		if table.maxn(flat_plains) > 0 then
			local resources_to_place = {
			{self.wheat_ID, 1, 100, 0, 2} };
			self:ProcessResourceList(16 / infertility_quotient, ImpactLayers.LAYER_BONUS, flat_plains, resources_to_place);
		end
		if table.maxn(flat_grass) > 0 then
			local resources_to_place = {
			{self.cow_ID, 1, 100, 0, 2} };
			self:ProcessResourceList(14 / infertility_quotient, ImpactLayers.LAYER_BONUS, flat_grass, resources_to_place);
		end
		if table.maxn(flat_tropical_plains) > 0 then
			local resources_to_place = {
			{self.maize_ID, 1, 100, 0, 2} };
			self:ProcessResourceList(16 / infertility_quotient, ImpactLayers.LAYER_BONUS, flat_tropical_plains, resources_to_place);
		end
		if table.maxn(flat_tropical_grass) > 0 then
			local resources_to_place = {
			{self.rice_ID, 1, 100, 0, 2} };
			self:ProcessResourceList(14 / infertility_quotient, ImpactLayers.LAYER_BONUS, flat_tropical_grass, resources_to_place);
		end
		if table.maxn(forests) > 0 then
			local resources_to_place = {
			{self.deer_ID, 1, 100, 1, 2} };
			self:ProcessResourceList(24 / infertility_quotient, ImpactLayers.LAYER_BONUS, forests, resources_to_place);
		end

		-- print("-"); print("Added extra Bonus resources to Hills Region#", region_number);

	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:AddModernMinorStrategicsToCityStates()
	-- This function added Spring 2011. Purpose is to add a small strategic to most city states.
	local _, _, oil_amt, _, coal_amt, alum_amt = self:GetSmallStrategicResourceQuantityValues();
	for city_state = 1, self.iNumCityStates do
		-- First check to see if this city state number received a valid start plot.
		if not self.city_state_validity_table[city_state] then
			-- This one did not! It does not exist on the map nor have valid data, so we will ignore it.
		else
			-- OK, it's a valid city state. Process it.
			local x = self.cityStatePlots[city_state][1];
			local y = self.cityStatePlots[city_state][2];
			-- Choose strategic type. Options are: 0-1 None, 2 Coal, 3 Oil, 4 Aluminum
			local diceroll = Map.Rand(5, "Choose resource type - CS Strategic LUA");
			if diceroll > 1 then
				-- This city state selected for minor strategic resource placement.
				local use_this_ID, res_amt;
				local tList = {};
				if diceroll == 2 then
					use_this_ID = self.coal_ID;
					res_amt = coal_amt;
					tList = {
						PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
						PlotListTypes.FLAT_PLAINS_DRY_NO_FEATURE,
						PlotListTypes.FLAT_GRASS_DRY_NO_FEATURE,
						PlotListTypes.FLAT_GRASS_WET_NO_FEATURE,
						PlotListTypes.FLAT_PLAINS_WET_NO_FEATURE,
					};
				elseif diceroll == 3 then
					use_this_ID = self.oil_ID;
					res_amt = oil_amt;
					tList = {
						PlotListTypes.FLAT_DESERT_NO_FEATURE,
						PlotListTypes.MARSH,
						PlotListTypes.FLAT_JUNGLE,
						PlotListTypes.FLAT_TUNDRA_NO_FEATURE,
						PlotListTypes.COAST_NEXT_TO_LAND,
					};
				elseif diceroll == 4 then
					use_this_ID = self.aluminum_ID;
					res_amt = alum_amt;
					tList = {
						PlotListTypes.HILLS_DESERT_TUNDRA_PLAINS_NO_FEATURE,
						PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE,
						PlotListTypes.FLAT_PLAINS_DRY_NO_FEATURE,
						PlotListTypes.FLAT_PLAINS_WET_NO_FEATURE,
					};
				end
				print("Assigned Strategic Type", use_this_ID, "to City State#", city_state);
				-- Place strategic.
				local luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 3, false);
				local iNumLeftToPlace = 1;
				for _, index in ipairs(tList) do
					local shuf_list = luxury_plot_lists[index];
					if index > 0 then
						iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, res_amt, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
					end
					if iNumLeftToPlace <= 0 then
						print("Placed Minor Strategic ID#", use_this_ID, "at City State#", city_state, "located at Plot", x, y);
						break;
					end
				end
			else
				print("City State#", city_state, "gets no strategic resource assigned to it.");
			end
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceOilInTheSea()
	-- Places sources of Oil in Coastal waters, equal to half what's on the
	-- land. If the map has too little ocean, then whatever will fit.

	-- WARNING: This operation will render the Oil Impact Table useless for
	-- further operations, so should always be called last, even after minor placements.
	local sea_oil_amt_small = 2;
	local sea_oil_amt_major = 3;
	local iNumLandOilUnits = self.amounts_of_resources_placed[self.oil_ID + 1];
	local iNumToPlaceSmall = math.floor((iNumLandOilUnits / 5) / sea_oil_amt_small);
	local iNumToPlaceMajor = math.floor((iNumLandOilUnits / 5) / sea_oil_amt_major);

	-- print("Adding Oil resources to the Sea.");
	local tPlotList = self.global_resource_plot_lists[PlotListTypes.COAST_NEXT_TO_LAND];
	self:PlaceSpecificNumberOfResources(self.oil_ID, sea_oil_amt_small, iNumToPlaceSmall, 0.2, ImpactLayers.LAYER_OIL, 2, 4, tPlotList);
	self:PlaceSpecificNumberOfResources(self.oil_ID, sea_oil_amt_major, iNumToPlaceMajor, 0.2, ImpactLayers.LAYER_OIL, 4, 7, tPlotList);
end
------------------------------------------------------------------------------
function AssignStartingPlots:AttemptToPlaceTreesAtResourcePlot(plot)
	-- Sub-function of AdjustTiles()
	-- Place forest or jungle on a resource plot depending on latitude and resource type
	local iResourceType = plot:GetResourceType();
	if self:IsTropical(plot:GetY()) then
		if iResourceType ~= self.fur_ID then
			plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE);
		else
			plot:SetFeatureType(FeatureTypes.FEATURE_FOREST);
		end
	else
		plot:SetFeatureType(FeatureTypes.FEATURE_FOREST);
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:AdjustTiles()
	--[[ MOD.Barathor:

		 Hijacked this function and it now fixes many resource types. Formerly, this function was FixSugarJungles.
		 This function does not adjust terrain types (except when under jungle), so that terrain bands stay intact.

		 This allows much greater flexibility when assigning resources to the map. ]]

	-- Not Communitu_79a's version since that depends on map settings, so here's a generic one

	-- This function was renamed to AdjustTiles from FixResourceGraphics

	local iW, iH = Map.GetGridSize();
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do

			local plot = Map.GetPlot(x, y);
			local res_ID = plot:GetResourceType();
			local terrainType = plot:GetTerrainType();
			local featureType = plot:GetFeatureType();

			-- Mined/Quarried Resources
			if res_ID == self.marble_ID or
			   res_ID == self.gold_ID or
			   res_ID == self.silver_ID or
			   res_ID == self.copper_ID or
			   res_ID == self.gems_ID or
			   res_ID == self.salt_ID or
			   res_ID == self.lapis_ID or
			   res_ID == self.jade_ID or
			   res_ID == self.amber_ID or
			   -- MOD.HungryForFood: Start
			   self:IsEvenMoreResourcesActive() and
			   (
			   res_ID == self.obsidian_ID or
			   res_ID == self.platinum_ID or
			   res_ID == self.tin_ID or
			   res_ID == self.lead_ID or
			   res_ID == self.sulfur_ID or
			   res_ID == self.titanium_ID
			   )
			   -- MOD.HungryForFood: End
			   then

				-- Changed by azum4roll: now follows Civilopedia more closely
				local removeFeature = false;
				if featureType ~= FeatureTypes.FEATURE_FLOOD_PLAINS then
					removeFeature = true;
				end
				-- Gems can be in jungles
				if featureType == FeatureTypes.FEATURE_JUNGLE and res_ID == self.gems_ID then
					removeFeature = false;
				end
				if removeFeature then
					plot:SetFeatureType(FeatureTypes.NO_FEATURE);
				end

				if res_ID == self.silver_ID and terrainType == TerrainTypes.TERRAIN_SNOW then
					-- Turn snow into tundra
					plot:SetTerrainType(TerrainTypes.TERRAIN_TUNDRA, false, true);
				end

			-- Tree Resources
			elseif res_ID == self.cocoa_ID or
				   res_ID == self.citrus_ID or
				   res_ID == self.spices_ID or
				   res_ID == self.sugar_ID or
				   res_ID == self.truffles_ID or
				   res_ID == self.silk_ID or
				   res_ID == self.dye_ID or
				   res_ID == self.fur_ID or
				   res_ID == self.deer_ID or
				   -- MOD.HungryForFood: Start
				   self:IsEvenMoreResourcesActive() and
				   (
				   res_ID == self.hardwood_ID or
				   res_ID == self.rubber_ID
				   )
				   -- MOD.HungryForFood: End
				   then

				if res_ID == self.fur_ID then
					-- Always want it flat. The foxes fall into the hills.
					-- They need to be on tundra too.
					plot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
					plot:SetTerrainType(TerrainTypes.TERRAIN_TUNDRA, false, true);
				end

				-- Always want it covered for most tree resources.
				if featureType == FeatureTypes.FEATURE_MARSH then
					if res_ID == self.sugar_ID or res_ID == self.truffles_ID then
						-- Keep it marsh for these resources.
					else
						-- Add some jungle or forest.
						self:AttemptToPlaceTreesAtResourcePlot(plot);
					end
				else
					-- Sugar can only be on marsh or flood plains
					if res_ID == self.sugar_ID then
						if featureType ~= FeatureTypes.FEATURE_FLOOD_PLAINS then
							plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true);
							plot:SetFeatureType(FeatureTypes.FEATURE_MARSH);
						end
					else
						-- Add some jungle or forest.
						self:AttemptToPlaceTreesAtResourcePlot(plot);
					end
				end

			-- Open Land Resources
			elseif res_ID == self.incense_ID or
				   res_ID == self.ivory_ID or
				   res_ID == self.wine_ID or
				   res_ID == self.olives_ID or
				   res_ID == self.coffee_ID or
				   res_ID == self.tobacco_ID or
				   res_ID == self.tea_ID or
				   res_ID == self.perfume_ID or
				   res_ID == self.cotton_ID or
				   -- MOD.HungryForFood: Start
				   self:IsEvenMoreResourcesActive() and
				   (
				   res_ID == self.poppy_ID
				   )
				   -- MOD.HungryForFood: End
				   then

				if res_ID == self.ivory_ID then
					-- Always want it flat. Other types are fine on hills.
					plot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
				end

				-- Don't remove flood plains if present for the few that are placed on it, only remove other features, like marsh or any trees.
				if featureType ~= FeatureTypes.FEATURE_FLOOD_PLAINS then
					plot:SetFeatureType(FeatureTypes.NO_FEATURE);
				end

				if res_ID == self.incense_ID or res_ID == self.ivory_ID then
					-- Because incense/ivory are very restricted, it was expanded to look for grass tiles as a final fallback.
					-- This will help with certain distributions that incense previously didn't work well in, such as assignments to city-states which could be hit or miss.
					-- Besides jungle placements, this is the only luxury which will change the terrain it's found on. Plus, plains are mixed in with grass anyway.
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true);
					end
				end
			end
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:PrintFinalResourceTotalsToLog()
	print("-");
	print("--- Table of Results, New Start Finder ---");
	for loop, startData in ipairs(self.startingPlots) do
		print("-");
		print("Region#", loop, " has start plot at: ", startData[1], startData[2], "with Fertility Rating of ", startData[3]);
	end
	print("-");
	print("--- End of Start Finder Results Table ---");
	print("-");
	print("-");
	print("--- Table of Final Results, City State Placements ---");
	print("-");
	for cs_number = 1, self.iNumCityStates do
		if self.city_state_validity_table[cs_number] then
			local data_table = self.cityStatePlots[cs_number];
			local x = data_table[1];
			local y = data_table[2];
			local regNum = data_table[3];
			print("- City State", cs_number, "in Region", regNum, "is located at Plot", x, y);
		else
			print("- City State", cs_number, "was discarded due to overcrowding.");
		end
	end
	print("-");
	print("- - - - -");
	print("-");
	print("--- Table of Final Results, Resource Distribution ---");
	print("-");
	print("- LUXURY Resources -");
	-- MOD.Barathor: Updated: Added ID numbers to each resource name and reordered them for much easier testing!
	print(self.whale_ID,    "Whale...: ", self.amounts_of_resources_placed[self.whale_ID + 1]);
	print(self.pearls_ID,   "Pearls..: ", self.amounts_of_resources_placed[self.pearls_ID + 1]);
	print(self.gold_ID,     "Gold....: ", self.amounts_of_resources_placed[self.gold_ID + 1]);
	print(self.silver_ID,   "Silver..: ", self.amounts_of_resources_placed[self.silver_ID + 1]);
	print(self.gems_ID,     "Gems....: ", self.amounts_of_resources_placed[self.gems_ID + 1]);
	print(self.marble_ID,   "Marble..: ", self.amounts_of_resources_placed[self.marble_ID + 1]);
	print(self.ivory_ID,    "Ivory...: ", self.amounts_of_resources_placed[self.ivory_ID + 1]);
	print(self.fur_ID,      "Fur.....: ", self.amounts_of_resources_placed[self.fur_ID + 1]);
	print(self.dye_ID,      "Dye.....: ", self.amounts_of_resources_placed[self.dye_ID + 1]);
	print(self.spices_ID,   "Spices..: ", self.amounts_of_resources_placed[self.spices_ID + 1]);
	print(self.silk_ID,     "Silk....: ", self.amounts_of_resources_placed[self.silk_ID + 1]);
	print(self.sugar_ID,    "Sugar...: ", self.amounts_of_resources_placed[self.sugar_ID + 1]);
	print(self.cotton_ID,   "Cotton..: ", self.amounts_of_resources_placed[self.cotton_ID + 1]);
	print(self.wine_ID,     "Wine....: ", self.amounts_of_resources_placed[self.wine_ID + 1]);
	print(self.incense_ID,  "Incense.: ", self.amounts_of_resources_placed[self.incense_ID + 1]);
	print("- Expansion LUXURY Resources -");
	print(self.copper_ID,   "Copper..: ", self.amounts_of_resources_placed[self.copper_ID + 1]);
	print(self.salt_ID,     "Salt....: ", self.amounts_of_resources_placed[self.salt_ID + 1]);
	print(self.crab_ID,     "Crab....: ", self.amounts_of_resources_placed[self.crab_ID + 1]);
	print(self.truffles_ID, "Truffles: ", self.amounts_of_resources_placed[self.truffles_ID + 1]);
	print(self.citrus_ID,   "Citrus..: ", self.amounts_of_resources_placed[self.citrus_ID + 1]);
	print(self.cocoa_ID,    "Cocoa...: ", self.amounts_of_resources_placed[self.cocoa_ID + 1]);
	-- MOD.Barathor: Start
	print("- Mod LUXURY Resources -");
	print(self.coffee_ID,   "Coffee..: ", self.amounts_of_resources_placed[self.coffee_ID + 1]);
	print(self.tea_ID,      "Tea.....: ", self.amounts_of_resources_placed[self.tea_ID + 1]);
	print(self.tobacco_ID,  "Tobacco.: ", self.amounts_of_resources_placed[self.tobacco_ID + 1]);
	print(self.amber_ID,    "Amber...: ", self.amounts_of_resources_placed[self.amber_ID + 1]);
	print(self.jade_ID,     "Jade....: ", self.amounts_of_resources_placed[self.jade_ID + 1]);
	print(self.olives_ID,   "Olives..: ", self.amounts_of_resources_placed[self.olives_ID + 1]);
	print(self.perfume_ID,  "Perfume.: ", self.amounts_of_resources_placed[self.perfume_ID + 1]);
	print(self.coral_ID,  	"Coral...: ", self.amounts_of_resources_placed[self.coral_ID + 1]);
	print(self.lapis_ID,  	"Lapis...: ", self.amounts_of_resources_placed[self.lapis_ID + 1]);
	print("-");
	-- MOD.HungryForFood: Start
	if self:IsEvenMoreResourcesActive() then
		print("- Even More Resources for Vox Populi (Luxuries) -");
		print(self.lavender_ID,  "Lavender: ", self.amounts_of_resources_placed[self.lavender_ID + 1]);
		print(self.obsidian_ID,  "Obsidian: ", self.amounts_of_resources_placed[self.obsidian_ID + 1]);
		print(self.platinum_ID,  "Platinum: ", self.amounts_of_resources_placed[self.platinum_ID + 1]);
		print(self.poppy_ID,     "Poppy...: ", self.amounts_of_resources_placed[self.poppy_ID + 1]);
		print(self.tin_ID,       "Tin.....: ", self.amounts_of_resources_placed[self.tin_ID + 1]);
		print("-");
	end
	-- MOD.HungryForFood: End
	print("+ TOTAL.Lux: ", self.realtotalLuxPlacedSoFar); -- MOD.Barathor: Fixed: The old variable gets corrupted with non-luxury additions after all luxuries have been placed. This will display the correct total.
	-- MOD.Barathor: End
	print("-");
	print("- STRATEGIC Resources -");
	print(self.iron_ID,     "Iron....: ", self.amounts_of_resources_placed[self.iron_ID + 1]);
	print(self.horse_ID,    "Horse...: ", self.amounts_of_resources_placed[self.horse_ID + 1]);
	print(self.coal_ID,     "Coal....: ", self.amounts_of_resources_placed[self.coal_ID + 1]);
	print(self.oil_ID,      "Oil.....: ", self.amounts_of_resources_placed[self.oil_ID + 1]);
	print(self.aluminum_ID, "Aluminum: ", self.amounts_of_resources_placed[self.aluminum_ID + 1]);
	print(self.uranium_ID,  "Uranium.: ", self.amounts_of_resources_placed[self.uranium_ID + 1]);
	print("-");
	print("- BONUS Resources -");
	print(self.wheat_ID,    "Wheat...: ", self.amounts_of_resources_placed[self.wheat_ID + 1]);
	print(self.maize_ID,    "Maize...: ", self.amounts_of_resources_placed[self.maize_ID + 1]);
	print(self.rice_ID,     "Rice....: ", self.amounts_of_resources_placed[self.rice_ID + 1]);
	print(self.cow_ID,      "Cow.....: ", self.amounts_of_resources_placed[self.cow_ID + 1]);
	print(self.sheep_ID,    "Sheep...: ", self.amounts_of_resources_placed[self.sheep_ID + 1]);
	print(self.deer_ID,     "Deer....: ", self.amounts_of_resources_placed[self.deer_ID + 1]);
	print(self.banana_ID,   "Banana..: ", self.amounts_of_resources_placed[self.banana_ID + 1]);
	print(self.fish_ID,     "Fish....: ", self.amounts_of_resources_placed[self.fish_ID + 1]);
	print(self.stone_ID,    "Stone...: ", self.amounts_of_resources_placed[self.stone_ID + 1]);
	print(self.bison_ID,    "Bison...: ", self.amounts_of_resources_placed[self.bison_ID + 1]);
	print("-");
	-- MOD.HungryForFood: Start
	if self:IsEvenMoreResourcesActive() then
		print("- Even More Resources for Vox Populi (Bonus) -");
		print(self.coconut_ID,  "Coconut.: ", self.amounts_of_resources_placed[self.coconut_ID + 1]);
		print(self.hardwood_ID, "Hardwood: ", self.amounts_of_resources_placed[self.hardwood_ID + 1]);
		print(self.lead_ID,     "Lead....: ", self.amounts_of_resources_placed[self.lead_ID + 1]);
		print(self.pineapple_ID,"Pineapple: ", self.amounts_of_resources_placed[self.pineapple_ID + 1]);
		print(self.potato_ID,   "Potato..: ", self.amounts_of_resources_placed[self.potato_ID + 1]);
		print(self.rubber_ID,   "Rubber..: ", self.amounts_of_resources_placed[self.rubber_ID + 1]);
		print(self.sulfur_ID,   "Sulfur..: ", self.amounts_of_resources_placed[self.sulfur_ID + 1]);
		print(self.titanium_ID, "Titanium: ", self.amounts_of_resources_placed[self.titanium_ID + 1]);
	end
	-- MOD.HungryForFood: End
	if self:IsAloeVeraResourceActive() then
		print("- Aloe Vera for Vox Populi (Bonus) -");
		print(self.aloevera_ID, "AloeVera: ", self.amounts_of_resources_placed[self.aloevera_ID + 1]);
	end
	print("-----------------------------------------------------");
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetMajorStrategicResourceQuantityValues()
	-- This function determines quantity per tile for each strategic resource's major deposit size.
	-- Note: scripts that cannot place Oil in the sea need to increase amounts on land to compensate.
	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 3, 5, 3, 4, 7;
	-- Check the strategic deposit size setting.
	if self.resSize == 1 then -- Small
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 2, 3, 2, 2, 3;
	elseif self.resSize == 3 then -- Large
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 4, 6, 8, 6, 7, 10;
	end

	if self:IsReducedSupplyActive() then
		print("Reduced Supply Mod is active, halving major strategic supply quantity values");
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = math.floor(uran_amt / 2), math.floor(horse_amt / 2), math.floor(oil_amt / 2), math.floor(iron_amt / 2), math.floor(coal_amt / 2), math.floor(alum_amt / 2);
	end

	return uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt;
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetSmallStrategicResourceQuantityValues()
	-- This function determines quantity per tile for each strategic resource's small deposit size.
	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 1, 2, 2, 2, 2, 3;
	-- Check the strategic deposit size setting.
	if self.resSize == 1 then -- Small
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 1, 1, 1, 1, 1, 2;
	elseif self.resSize == 3 then -- Large
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 2, 2, 2, 2, 3;
	end

	if self:IsReducedSupplyActive() then
		print("Reduced Supply Mod is active, halving small strategic supply quantity values");
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 1, 1, 1, 1, 1, 2;
	end

	return uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt;
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceStrategicAndBonusResources()
	-- KEY: {Resource ID, Quantity (0 = unquantified), weighting, minimum radius, maximum radius}
	-- KEY: (frequency (1 per n plots in the list), impact list number, plot list, resource data)

	-- The radius creates a zone around the plot that other resources of that
	-- type will avoid if possible. See ProcessResourceList for impact numbers.

	-- Order of placement matters, so changing the order may affect a later dependency.

	-- Adjust amounts, if applicable, based on strategic deposit size setting.
	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = self:GetMajorStrategicResourceQuantityValues();
	local resources_to_place = {};

	-- Adjust appearance rate per resource density setting chosen by user.
	local resMultiplier = 1;
	if self.resDensity == 1 then -- Sparse, so increase the number of tiles per strategic.
		resMultiplier = 1.5;
	elseif self.resDensity == 3 then -- Abundant, so reduce the number of tiles per strategic.
		resMultiplier = 0.66666667;
	elseif self.resDensity == 4 then -- Random
		resMultiplier = self:GetRandomMultiplier(0.5);
	end

	-- Place Strategic resources.
	print("Map Generation - Placing Strategics");

	-- Revamped by azum4roll: now place most resources one by one for easier balancing
	local tPlotList = self.global_resource_plot_lists;

	resources_to_place = {
		{self.horse_ID, horse_amt, 100, 1, 2}
	};
	self:ProcessResourceList(24 * resMultiplier, ImpactLayers.LAYER_HORSE, tPlotList[PlotListTypes.FLAT_GRASS_NO_FEATURE], resources_to_place);
	self:ProcessResourceList(36 * resMultiplier, ImpactLayers.LAYER_HORSE, tPlotList[PlotListTypes.FLAT_PLAINS_NO_FEATURE], resources_to_place);

	resources_to_place = {
		{self.horse_ID, horse_amt * 0.7, 100, 2, 3}
	};
	self:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_HORSE, tPlotList[PlotListTypes.FLAT_DESERT_WET], resources_to_place);
	self:ProcessResourceList(60 * resMultiplier, ImpactLayers.LAYER_HORSE, tPlotList[PlotListTypes.FLAT_TUNDRA_NO_FEATURE], resources_to_place);

	resources_to_place = {
		{self.iron_ID, iron_amt, 100, 1, 3}
	};
	self:ProcessResourceList(75 * resMultiplier, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.HILLS_NO_FEATURE], resources_to_place);
	self:ProcessResourceList(45 * resMultiplier, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.HILLS_FOREST], resources_to_place);
	self:ProcessResourceList(50 * resMultiplier, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.FLAT_DESERT_NO_FEATURE], resources_to_place);
	self:ProcessResourceList(80 * resMultiplier, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE], resources_to_place);
	self:ProcessResourceList(50 * resMultiplier, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_FOREST], resources_to_place);
	self:ProcessResourceList(80 * resMultiplier, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.FLAT_TUNDRA_NO_FEATURE], resources_to_place);
	self:ProcessResourceList(55 * resMultiplier, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.FLAT_TUNDRA_FOREST], resources_to_place);
	self:ProcessResourceList(17 * resMultiplier, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.FLAT_SNOW], resources_to_place);

	resources_to_place = {
		{self.coal_ID, coal_amt, 100, 1, 2}
	};
	self:ProcessResourceList(60 * resMultiplier, ImpactLayers.LAYER_COAL, tPlotList[PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE], resources_to_place);
	self:ProcessResourceList(100 * resMultiplier, ImpactLayers.LAYER_COAL, tPlotList[PlotListTypes.FLAT_GRASS_NO_FEATURE], resources_to_place);
	self:ProcessResourceList(80 * resMultiplier, ImpactLayers.LAYER_COAL, tPlotList[PlotListTypes.FLAT_PLAINS_NO_FEATURE], resources_to_place);
	self:ProcessResourceList(70 * resMultiplier, ImpactLayers.LAYER_COAL, tPlotList[PlotListTypes.PLAINS_GRASS_FOREST], resources_to_place);

	resources_to_place = {
		{self.oil_ID, oil_amt, 100, 1, 3}
	};
	self:ProcessResourceList(35 * resMultiplier, ImpactLayers.LAYER_OIL, tPlotList[PlotListTypes.FLAT_DESERT_NO_FEATURE], resources_to_place);
	self:ProcessResourceList(75 * resMultiplier, ImpactLayers.LAYER_OIL, tPlotList[PlotListTypes.FLAT_TUNDRA_NO_FEATURE], resources_to_place);
	self:ProcessResourceList(100 * resMultiplier, ImpactLayers.LAYER_OIL, tPlotList[PlotListTypes.FLAT_JUNGLE], resources_to_place);
	self:ProcessResourceList(25 * resMultiplier, ImpactLayers.LAYER_OIL, tPlotList[PlotListTypes.MARSH], resources_to_place);
	self:ProcessResourceList(17 * resMultiplier, ImpactLayers.LAYER_OIL, tPlotList[PlotListTypes.FLAT_SNOW], resources_to_place);

	resources_to_place = {
		{self.aluminum_ID, alum_amt, 100, 1, 3}
	};
	self:ProcessResourceList(38 * resMultiplier, ImpactLayers.LAYER_ALUMINUM, tPlotList[PlotListTypes.HILLS_DESERT_TUNDRA_PLAINS_NO_FEATURE], resources_to_place);
	self:ProcessResourceList(25 * resMultiplier, ImpactLayers.LAYER_ALUMINUM, tPlotList[PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE], resources_to_place);
	self:ProcessResourceList(50 * resMultiplier, ImpactLayers.LAYER_ALUMINUM, tPlotList[PlotListTypes.FLAT_PLAINS_NO_FEATURE], resources_to_place);

	resources_to_place = {
		{self.uranium_ID, uran_amt, 100, 2, 4}
	};
	self:ProcessResourceList(200 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.HILLS_NO_FEATURE], resources_to_place);
	self:ProcessResourceList(300 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.HILLS_FOREST], resources_to_place);
	self:ProcessResourceList(50 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.HILLS_JUNGLE], resources_to_place);
	self:ProcessResourceList(150 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.FLAT_JUNGLE], resources_to_place);
	self:ProcessResourceList(150 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.FLAT_DESERT_NO_FEATURE], resources_to_place);
	self:ProcessResourceList(90 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.FLAT_TUNDRA_NO_FEATURE], resources_to_place);
	self:ProcessResourceList(350 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.FLAT_TUNDRA_FOREST], resources_to_place);
	self:ProcessResourceList(300 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE], resources_to_place);
	self:ProcessResourceList(325 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_FOREST], resources_to_place);
	self:ProcessResourceList(37 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.MARSH], resources_to_place);
	self:ProcessResourceList(35 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.FLAT_SNOW], resources_to_place);

	self:AddModernMinorStrategicsToCityStates();

	self:PlaceSmallQuantitiesOfStrategics(30 * resMultiplier, tPlotList[PlotListTypes.LAND]);

	self:PlaceOilInTheSea();

	-- Check for low or missing Strategic resources
	uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = self:GetSmallStrategicResourceQuantityValues();
	local iLoop = 0;
	while self.amounts_of_resources_placed[self.iron_ID + 1] < 4 * self.iNumCivs and iLoop < 999 do
		print("Map has very low iron, adding another.");
		resources_to_place = { {self.iron_ID, iron_amt, 33, 1, 2} };
		self:ProcessResourceList(99999, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.HILLS], resources_to_place); -- 99999 means one per that many tiles: a single instance.
		self:ProcessResourceList(99999, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.FLAT_FOREST], resources_to_place);
		self:ProcessResourceList(99999, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.FLAT_DESERT_NO_FEATURE], resources_to_place);
		iLoop = iLoop + 1;
	end
	iLoop = 0;
	while self.amounts_of_resources_placed[self.horse_ID + 1] < 4 * self.iNumCivs and iLoop < 999 do
		print("Map has very low horse, adding another.");
		resources_to_place = { {self.horse_ID, horse_amt, 50, 1, 2} };
		self:ProcessResourceList(99999, ImpactLayers.LAYER_HORSE, tPlotList[PlotListTypes.FLAT_TUNDRA_PLAINS_GRASS_NO_FEATURE], resources_to_place);
		self:ProcessResourceList(99999, ImpactLayers.LAYER_HORSE, tPlotList[PlotListTypes.FLAT_DESERT_WET], resources_to_place);
		iLoop = iLoop + 1;
	end
	iLoop = 0;
	while self.amounts_of_resources_placed[self.coal_ID + 1] < 4 * self.iNumCivs and iLoop < 999 do
		print("Map has very low coal, adding another.");
		resources_to_place = { {self.coal_ID, coal_amt, 33, 1, 2} };
		self:ProcessResourceList(99999, ImpactLayers.LAYER_COAL, tPlotList[PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE], resources_to_place);
		self:ProcessResourceList(99999, ImpactLayers.LAYER_COAL, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE], resources_to_place);
		self:ProcessResourceList(99999, ImpactLayers.LAYER_COAL, tPlotList[PlotListTypes.PLAINS_GRASS_FOREST], resources_to_place);
		iLoop = iLoop + 1;
	end
	iLoop = 0;
	while self.amounts_of_resources_placed[self.oil_ID + 1] < 4 * self.iNumCivs and iLoop < 999 do
		print("Map has very low oil, adding another.");
		resources_to_place = { {self.oil_ID, oil_amt, 50, 1, 2} };
		self:ProcessResourceList(99999, ImpactLayers.LAYER_OIL, tPlotList[PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE], resources_to_place);
		self:ProcessResourceList(99999, ImpactLayers.LAYER_OIL, tPlotList[PlotListTypes.FLAT_JUNGLE], resources_to_place);
		iLoop = iLoop + 1;
	end
	iLoop = 0;
	while self.amounts_of_resources_placed[self.aluminum_ID + 1] < 5 * self.iNumCivs and iLoop < 999 do
		print("Map has very low aluminum, adding another.");
		resources_to_place = { {self.aluminum_ID, alum_amt, 33, 1, 2} };
		self:ProcessResourceList(99999, ImpactLayers.LAYER_ALUMINUM, tPlotList[PlotListTypes.HILLS_DESERT_TUNDRA_PLAINS_NO_FEATURE], resources_to_place);
		self:ProcessResourceList(99999, ImpactLayers.LAYER_ALUMINUM, tPlotList[PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE], resources_to_place);
		self:ProcessResourceList(99999, ImpactLayers.LAYER_ALUMINUM, tPlotList[PlotListTypes.FLAT_PLAINS_NO_FEATURE], resources_to_place);
		iLoop = iLoop + 1;
	end
	iLoop = 0;
	while self.amounts_of_resources_placed[self.uranium_ID + 1] < 2 * self.iNumCivs and iLoop < 999 do
		print("Map has very low uranium, adding another.");
		resources_to_place = { {self.uranium_ID, uran_amt, 100, 2, 4} };
		self:ProcessResourceList(99999, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.LAND], resources_to_place);
		iLoop = iLoop + 1;
	end

	self:PlaceBonusResources();
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceBonusResources()
	local resMultiplier = 1;
	if self.bonusDensity == 1 then -- Sparse, so increase the number of tiles per bonus.
		resMultiplier = 1.5;
	elseif self.bonusDensity == 3 then -- Abundant, so reduce the number of tiles per bonus.
		resMultiplier = 0.66666667;
	elseif self.bonusDensity == 4 then -- Random
		resMultiplier = self:GetRandomMultiplier(0.5);
	end

	-- Place Bonus Resources
	print("Map Generation - Placing Bonuses");
	-- ####Communitu_79a has a completely different PlaceFish mechanic, but that's too map dependent
	local tPlotList = self.global_resource_plot_lists;
	self:PlaceFish(10 * resMultiplier, tPlotList[PlotListTypes.COAST]);
	self:PlaceSexyBonusAtCivStarts();
	self:AddExtraBonusesToHillsRegions();

	local resources_to_place = {};

	if self:IsAloeVeraResourceActive() then
		resources_to_place = {
			{self.aloevera_ID, 1, 100, 1, 2}
		};
		self:ProcessResourceList(10 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_DESERT_NO_FEATURE], resources_to_place);
	end

	if self:IsEvenMoreResourcesActive() then
		resources_to_place = {
			{self.deer_ID, 1, 100, 0, 2}
		};
		self:ProcessResourceList(12 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.TUNDRA_FOREST], resources_to_place);
		self:ProcessResourceList(16 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_TUNDRA_NO_FEATURE], resources_to_place);

		resources_to_place = {
			{self.deer_ID, 1, 100, 0, 1}
		};
		self:ProcessResourceList(30 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.JUNGLE], resources_to_place);

		resources_to_place = {
			{self.wheat_ID, 1, 100, 1, 2}
		};
		self:ProcessResourceList(20 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_DESERT_WET], resources_to_place);

		resources_to_place = {
			{self.wheat_ID, 1, 100, 2, 3}
		};
		self:ProcessResourceList(44 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.WHEAT], resources_to_place);

		resources_to_place = {
			{self.rice_ID, 1, 100, 0, 2}
		};
		self:ProcessResourceList(24 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.RICE], resources_to_place);

		resources_to_place = {
			{self.maize_ID, 1, 100, 1, 2}
		};
		self:ProcessResourceList(32 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.MAIZE], resources_to_place);

		resources_to_place = {
			{self.banana_ID, 1, 100, 0, 1}
		};
		self:ProcessResourceList(30 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_JUNGLE], resources_to_place);
		self:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.MARSH], resources_to_place);

		resources_to_place = {
			{self.cow_ID, 1, 100, 0, 2}
		};
		self:ProcessResourceList(30 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_GRASS_NO_FEATURE], resources_to_place);

		resources_to_place = {
			{self.bison_ID, 1, 100, 0, 2}
		};
		self:ProcessResourceList(24 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE], resources_to_place);

		resources_to_place = {
			{self.sheep_ID, 1, 100, 0, 2}
		};
		self:ProcessResourceList(44 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_NO_FEATURE], resources_to_place);

		resources_to_place = {
			{self.stone_ID, 1, 100, 1, 1}
		};
		self:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_PLAINS_DRY_NO_FEATURE], resources_to_place);
		self:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_GRASS_DRY_NO_FEATURE], resources_to_place);

		resources_to_place = {
			{self.stone_ID, 1, 100, 0, 2}
		};
		self:ProcessResourceList(16 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_DESERT_NO_FEATURE], resources_to_place);
		self:ProcessResourceList(10 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_SNOW], resources_to_place);

		resources_to_place = {
			{self.stone_ID, 1, 100, 1, 2}
		};
		self:ProcessResourceList(30 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_TUNDRA_NO_FEATURE], resources_to_place);
		self:ProcessResourceList(36 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_NO_FEATURE], resources_to_place);

	-- Even More Resources for VP start
		resources_to_place = {
			{self.coconut_ID, 1, 100, 0, 2}
		};
		self:ProcessResourceList(12 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.COCONUT], resources_to_place);

		resources_to_place = {
			{self.hardwood_ID, 1, 100, 1, 2}
		};
		self:ProcessResourceList(37 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_COVERED], resources_to_place);
		self:ProcessResourceList(37 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_COVERED], resources_to_place);
		self:ProcessResourceList(29 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_TUNDRA_FOREST], resources_to_place);

		resources_to_place = {
			{self.pineapple_ID, 1, 100, 0, 3}
		};
		self:ProcessResourceList(29 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_JUNGLE], resources_to_place);

		resources_to_place = {
			{self.rubber_ID, 1, 100, 0, 2}
		};
		self:ProcessResourceList(43 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_JUNGLE], resources_to_place);

		resources_to_place = {
			{self.sulfur_ID, 1, 100, 0, 2}
		};
		self:ProcessResourceList(21 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_SNOW], resources_to_place);

		resources_to_place = {
			{self.sulfur_ID, 1, 100, 1, 3}
		};
		self:ProcessResourceList(29 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_NO_FEATURE], resources_to_place);
		self:ProcessResourceList(37 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_COVERED], resources_to_place);
		self:ProcessResourceList(43 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_NO_FEATURE], resources_to_place);

		resources_to_place = {
			{self.titanium_ID, 1, 100, 0, 2}
		};
		self:ProcessResourceList(56 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_NO_FEATURE], resources_to_place);
		self:ProcessResourceList(51 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_NO_FEATURE], resources_to_place);
		self:ProcessResourceList(48 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_DESERT_NO_FEATURE], resources_to_place);
		self:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_TUNDRA], resources_to_place);

		resources_to_place = {
			{self.titanium_ID, 1, 100, 0, 1}
		};
		self:ProcessResourceList(24 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_SNOW], resources_to_place);

		resources_to_place = {
			{self.potato_ID, 1, 100, 0, 3}
		};
		self:ProcessResourceList(29 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE], resources_to_place);

		resources_to_place = {
			{self.potato_ID, 1, 100, 2, 3}
		};
		self:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE], resources_to_place);

		resources_to_place = {
			{self.lead_ID, 1, 100, 1, 2}
		};
		self:ProcessResourceList(35 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_DESERT_NO_FEATURE], resources_to_place);

		resources_to_place = {
			{self.lead_ID, 1, 100, 1, 3}
		};
		self:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_GRASS_DRY_NO_FEATURE], resources_to_place);

		resources_to_place = {
			{self.lead_ID, 1, 100, 2, 3}
		};
		self:ProcessResourceList(35 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_NO_FEATURE], resources_to_place);
	-- Even More Resources for VP end
		-- Placed last, since this blocks a large area
		resources_to_place = {
			{self.deer_ID, 1, 100, 3, 4}
		};
		self:ProcessResourceList(50 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_FOREST], resources_to_place);
		self:ProcessResourceList(50 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_FOREST], resources_to_place);
	else
		
		resources_to_place = {
			{self.deer_ID, 1, 100, 0, 2}
		};
		self:ProcessResourceList(6 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.TUNDRA_FOREST], resources_to_place);
		self:ProcessResourceList(8 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_TUNDRA_NO_FEATURE], resources_to_place);

		resources_to_place = {
			{self.deer_ID, 1, 100, 0, 1}
		};
		self:ProcessResourceList(15 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.JUNGLE], resources_to_place);

		resources_to_place = {
			{self.wheat_ID, 1, 100, 0, 2}
		};
		self:ProcessResourceList(20 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.WHEAT], resources_to_place);

		resources_to_place = {
			{self.rice_ID, 1, 100, 0, 1}
		};
		self:ProcessResourceList(12 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.RICE], resources_to_place);

		resources_to_place = {
			{self.maize_ID, 1, 100, 0, 2}
		};
		self:ProcessResourceList(24 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.MAIZE], resources_to_place);

		resources_to_place = {
			{self.banana_ID, 1, 100, 0, 1}
		};
		self:ProcessResourceList(15 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_JUNGLE], resources_to_place);
		self:ProcessResourceList(20 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.MARSH], resources_to_place);

		resources_to_place = {
			{self.cow_ID, 1, 100, 0, 2}
		};
		self:ProcessResourceList(15 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_GRASS_NO_FEATURE], resources_to_place);

		resources_to_place = {
			{self.bison_ID, 1, 100, 0, 2}
		};
		self:ProcessResourceList(26 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE], resources_to_place);

		resources_to_place = {
			{self.sheep_ID, 1, 100, 0, 2}
		};
		self:ProcessResourceList(18 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_NO_FEATURE], resources_to_place);

		resources_to_place = {
			{self.stone_ID, 1, 100, 1, 1}
		};
		self:ProcessResourceList(60 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_PLAINS_DRY_NO_FEATURE], resources_to_place);
		self:ProcessResourceList(30 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_GRASS_DRY_NO_FEATURE], resources_to_place);

		resources_to_place = {
			{self.stone_ID, 1, 100, 0, 2}
		};
		self:ProcessResourceList(13 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_DESERT_NO_FEATURE], resources_to_place);
		self:ProcessResourceList(8 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_SNOW], resources_to_place);

		resources_to_place = {
			{self.stone_ID, 1, 100, 1, 2}
		};
		self:ProcessResourceList(60 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_TUNDRA_NO_FEATURE], resources_to_place);
		self:ProcessResourceList(60 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_NO_FEATURE], resources_to_place);

		-- Placed last, since this blocks a large area
		resources_to_place = {
			{self.deer_ID, 1, 100, 3, 4}
		};
		self:ProcessResourceList(25 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_FOREST], resources_to_place);
		self:ProcessResourceList(25 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_FOREST], resources_to_place);
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceResourcesAndCityStates(args)
	--[[
	This function controls nearly all resource placement.
	Only resources placed during Normalization operations are handled elsewhere.

	Luxury resources are placed in relationship to Regions, adapting to the details of the given map instance, including number of civs and city states present.
	At Jon's direction, Luxuries have been implemented to be diplomatic widgets for trading, in addition to sources of Happiness.

	Strategic and Bonus resources are terrain-adjusted. They will customize to each map instance.
	Each terrain type has been measured and has certain resource types assigned to it.
	You can customize resource placement to any degree desired by controlling generation of plot groups to feed into the process.
	The default plot groups are terrain-based, but any criteria you desire could be used to determine plot group membership.

	If any default methods fail to meet a specific need, don't hesitate to replace them with custom methods.
	I have labored to make this new system as accessible and powerful as any ever before offered.
	--]]

	print("Map Generation - Assigning Luxury Resource Distribution");
	self:AssignLuxuryRoles();

	print("Map Generation - Placing City States");
	self:PlaceCityStates();

	-- Generate global plot lists for resource distribution.
	self:GenerateGlobalResourcePlotLists();

	print("Map Generation - Placing Luxuries");
	self:PlaceLuxuries(args);

	-- Place Strategic and Bonus resources.
	self:PlaceStrategicAndBonusResources();

	print("Map Generation - Normalize City State Locations");
	self:NormalizeCityStateLocations();

	print("Map Generation - Fix Tile Graphics");
	self:AdjustTiles();

	-- Necessary to implement placement of Natural Wonders, and possibly other plot-type changes.
	-- This operation must be saved for last, as it invalidates all regional data by resetting landmass/area IDs.
	Map.RecalculateAreas();

	-- Activate for debug only
	self:PrintFinalResourceTotalsToLog();

	print("End PlaceResourcesAndCityStates");
end
------------------------------------------------------------------------------
-- HELPER FUNCTIONS
------------------------------------------------------------------------------
-- Check if Even More Resources for Vox Populi is activated
function AssignStartingPlots:IsEvenMoreResourcesActive()
	local communityPatchModID = "d1b6328c-ff44-4b0d-aad7-c657f83610cd";
	local evenMoreResourcesModID = "8e54eb87-31e8-4fcd-aafe-ede055b463d0";
	local isUsingCommunityPatch = false;

	for _, mod in pairs(Modding.GetActivatedMods()) do
		if mod.ID == communityPatchModID then -- if Community Patch is not activated, then we are running in modpack mode
			isUsingCommunityPatch = true;
		elseif mod.ID == evenMoreResourcesModID then
			return true;
		end
	end

	if not isUsingCommunityPatch then -- fallback method for modpack mode
		for _ in DB.Query("SELECT * FROM Resources WHERE Type = 'RESOURCE_POPPY'") do
			return true;
		end
	end
	return false;
end
--------------------------------------------------------------------------------
function AssignStartingPlots:IsAloeVeraResourceActive()
	local communityPatchModID = "d1b6328c-ff44-4b0d-aad7-c657f83610cd";
	local aloeVeraModID = "c2c3797b-2728-417c-94c8-f30410528bf2";
	local isUsingCommunityPatch = false;

	for _, mod in pairs(Modding.GetActivatedMods()) do
		if mod.ID == communityPatchModID then -- if Community Patch is not activated, then we are running in modpack mode
			isUsingCommunityPatch = true;
		elseif mod.ID == aloeVeraModID then
			return true;
		end
	end

	if not isUsingCommunityPatch then -- fallback method for modpack mode
		for _ in DB.Query("SELECT * FROM Resources WHERE Type = 'RESOURCE_JAR_ALOE_VERA'") do
			return true;
		end
	end
	return false;
end
--------------------------------------------------------------------------------
-- Check if Reduced Supply for Vox Populi is activated
function AssignStartingPlots:IsReducedSupplyActive()
	local communityPatchModID = "d1b6328c-ff44-4b0d-aad7-c657f83610cd";
	local ReducedSupplyModID = "abbade81-2894-41e9-SUPP-SWVPx13a770d";
	local isUsingCommunityPatch = false;

	for _, mod in pairs(Modding.GetActivatedMods()) do
		if mod.ID == communityPatchModID then -- if Community Patch is not activated, then we are running in modpack mode
			isUsingCommunityPatch = true;
		elseif mod.ID == ReducedSupplyModID then
			return true;
		end
	end

	if not isUsingCommunityPatch then -- fallback method for modpack mode
		for _ in DB.Query("SELECT * FROM Defines WHERE Name = 'FRIENDS_BASE_TURNS_UNIT_SPAWN' AND Value = 28") do
			return true;
		end
	end

	return false;
end
----------------------------------------------------------------
function AssignStartingPlots:Plot_GetPlotsInCircle(plot, minR, maxR)
	if not plot then
		print("plot:GetPlotsInCircle plot=nil");
		return;
	end
	if not maxR then
		maxR = minR;
		minR = 1;
	end

	local mapW, mapH = Map.GetGridSize();
	local isWrapX = Map:IsWrapX();
	local isWrapY = Map:IsWrapY();
	local centerX = plot:GetX();
	local centerY = plot:GetY();

	local leftX = isWrapX and ((centerX - maxR) % mapW) or self:Constrain(0, centerX - maxR, mapW - 1);
	local rightX = isWrapX and ((centerX + maxR) % mapW) or self:Constrain(0, centerX + maxR, mapW - 1);
	local bottomY = isWrapY and ((centerY - maxR) % mapH) or self:Constrain(0, centerY - maxR, mapH - 1);
	local topY = isWrapY and ((centerY + maxR) % mapH) or self:Constrain(0, centerY + maxR, mapH - 1);

	local nearX = leftX;
	local nearY = bottomY;
	local stepX = 0;
	local stepY = 0;
	local rectW = rightX-leftX;
	local rectH = topY-bottomY;

	if rectW < 0 then
		rectW = rectW + mapW;
	end

	if rectH < 0 then
		rectH = rectH + mapH;
	end

	local nextPlot = Map.GetPlot(nearX, nearY);

	return function ()
		while stepY < 1 + rectH and nextPlot do
			while stepX < 1 + rectW and nextPlot do
				local thisPlot = nextPlot;
				local distance = Map.PlotDistance(nearX, nearY, centerX, centerY);

				nearX = (nearX + 1) % mapW;
				stepX = stepX + 1;
				nextPlot = Map.GetPlot(nearX, nearY);

				if self:IsBetween(minR, distance, maxR) then
					return thisPlot, distance;
				end
			end
			nearX = leftX;
			nearY = (nearY + 1) % mapH;
			stepX = 0;
			stepY = stepY + 1;
			nextPlot = Map.GetPlot(nearX, nearY);
		end
	end
end
------------------------------------------------------
function AssignStartingPlots:Plot_GetFertilityInRange(plot, range, yieldID)
	local value = 0;
	for nearPlot, distance in self:Plot_GetPlotsInCircle(plot, range) do
		value = value + self:Plot_GetFertility(nearPlot, yieldID) / math.max(1, distance);
	end
	return value;
end
------------------------------------------------------
function AssignStartingPlots:Plot_GetFertility(plot, yieldID, ignoreStrategics)
	if plot:IsImpassable() or plot:GetTerrainType() == TerrainTypes.TERRAIN_OCEAN then
		return 0;
	end

	local value = 0;
	local featureID = plot:GetFeatureType();
	local terrainID = plot:GetTerrainType();
	local resID = plot:GetResourceType();

	if yieldID then
		value = value + plot:CalculateYield(yieldID, false);
	else
		-- Science, Culture and Faith are worth more than the others at start
		value = value + plot:CalculateYield(YieldTypes.YIELD_FOOD, false);
		value = value + plot:CalculateYield(YieldTypes.YIELD_PRODUCTION, false);
		value = value + plot:CalculateYield(YieldTypes.YIELD_GOLD, false);
		value = value + 2 * plot:CalculateYield(YieldTypes.YIELD_SCIENCE, false);
		value = value + 2 * plot:CalculateYield(YieldTypes.YIELD_CULTURE, false);
		value = value + 2 * plot:CalculateYield(YieldTypes.YIELD_FAITH, false);
	end

	if plot:IsFreshWater() and plot:GetPlotType() ~= PlotTypes.PLOT_HILLS then
		-- Fresh water farm possibility
		value = value + 0.25;
	end

	if plot:IsLake() then
		-- can't improve lakes
		value = value - 0.5;
	end

	if featureID == FeatureTypes.FEATURE_JUNGLE then
		-- jungles aren't as good as the yields imply
		value = value - 0.5;
	end

	if resID == -1 then
		if featureID == -1 and terrainID == TerrainTypes.TERRAIN_COAST then
			-- can't improve coast tiles until lighthouse
			-- lower value generates more fish
			value = value - 1;
		end
	else
		local resInfo = GameInfo.Resources[resID];
		value = value + 2 * resInfo.Happiness;
		if resInfo.ResourceClassType == "RESOURCECLASS_RUSH" and not ignoreStrategics then
			-- Iron and Horses
			value = value + math.ceil(3 * math.sqrt(plot:GetNumResource()));
		elseif resInfo.ResourceClassType == "RESOURCECLASS_BONUS" then
			value = value + 2;
		end
	end

	return value;
end
------------------------------------------------------
function AssignStartingPlots:IsTropical(y)
	local _, iH = Map.GetGridSize();
	local lat = 0;
	if y >= iH / 2 then
		lat = math.abs(iH / 2 - y) / (iH / 2);
	else
		lat = math.abs(iH / 2 - (y + 1)) / (iH / 2);
	end
	local AvgJungleRange = 0;
	local rain = Map.GetCustomOption(2);
	if rain == 1 then
		-- Arid
		AvgJungleRange = 0.10;
	elseif rain == 3 then
		-- Wet
		AvgJungleRange = 0.25;
	else
		-- Normal or Random (Note: I'm currently not sure how to retrieve random, so we'll just use normal for now.)
		AvgJungleRange = 0.15;
	end
	return lat <= AvgJungleRange;
end
------------------------------------------------------
function AssignStartingPlots:IsBetween(lower, mid, upper)
	return lower <= mid and mid <= upper;
end
------------------------------------------------------
function AssignStartingPlots:Constrain(lower, mid, upper)
	return math.max(lower, math.min(mid, upper));
end
------------------------------------------------------
-- Get random multiplier normalized to 1
-- rand: optional random value
-- higher: optional boolean, determines >1 or <1
function AssignStartingPlots:GetRandomMultiplier(variance, rand, higher)
	higher = higher or (1 == Map.Rand(2, "GetRandomMultiplier"));

	local multiplier = 1;
	if not rand then
		multiplier = 1 + Map.Rand(10000, "GetRandomMultiplier") / 10000 * variance;
	else
		multiplier = 1 + rand * variance;
	end

	if higher then
		return multiplier;
	else
		return 1 / multiplier;
	end
end
------------------------------------------------------
-- Get random integer between min and max inclusive
-- min and max must be integers
function AssignStartingPlots:GetRandomFromRangeInclusive(min, max)
	if min > max then
		return self:GetRandomFromRangeInclusive(max, min);
	end
	return min + Map.Rand(max - min + 1, "GetRandomFromRangeInclusive");
end
------------------------------------------------------
-- A copy of ObtainLandmassBoundaries from MapmakerUtilities, but actually works on landmass instead of area
function AssignStartingPlots:GetLandmassBoundaries(iLandmassID)
	local iW, iH = Map.GetGridSize();
	-- Set up variables that will be returned by this function.
	local wrapsX = false;
	local wrapsY = false;
	local iWestX, iEastX, iSouthY, iNorthY, iWidth, iHeight;

	if Map:IsWrapX() then -- Check to see if landmass Wraps X.
		local foundFirstColumn = false;
		local foundLastColumn = false;
		for y = 0, iH - 1 do
			local plotFirst = Map.GetPlot(0, y);
			local plotLast = Map.GetPlot(iW - 1, y);
			local landmass = plotFirst:GetLandmass();
			if landmass == iLandmassID then -- Found a plot belonging to iLandmassID in first column.
				foundFirstColumn = true;
			end
			landmass = plotLast:GetLandmass();
			if landmass == iLandmassID then -- Found a plot belonging to iLandmassID in last column.
				foundLastColumn = true;
			end
		end
		if foundFirstColumn and foundLastColumn then -- Plot on both sides of map edge.
			wrapsX = true;
		end
	end

	if Map:IsWrapY() then -- Check to see if landmass Wraps Y.
		local foundFirstRow = false;
		local foundLastRow = false;
		for x = 0, iW - 1 do
			local plotFirst = Map.GetPlot(x, 0);
			local plotLast = Map.GetPlot(x, iH - 1);
			local landmass = plotFirst:GetLandmass();
			if landmass == iLandmassID then -- Found a plot belonging to iLandmassID in first row.
				foundFirstRow = true;
			end
			landmass = plotLast:GetLandmass();
			if landmass == iLandmassID then -- Found a plot belonging to iLandmassID in last row.
				foundLastRow = true;
			end
		end
		if foundFirstRow and foundLastRow then -- Plot on both sides of map edge.
			wrapsY = true;
		end
	end

	-- Find West and East edges of this landmass.
	if not wrapsX then -- no X wrap
		for x = 0, iW - 1 do -- Check for any landmass membership one column at a time, left to right.
			local foundLandmassInColumn = false;
			for y = 0, iH - 1 do -- Checking column.
				local plot = Map.GetPlot(x, y);
				local landmass = plot:GetLandmass();
				if landmass == iLandmassID then -- Found a plot belonging to iLandmassID, set WestX to this column.
					foundLandmassInColumn = true;
					iWestX = x;
					break;
				end
			end
			if foundLandmassInColumn then -- Found WestX, done looking.
				break;
			end
		end
		for x = iW - 1, 0, -1 do -- Check for any landmass membership one column at a time, right to left.
			local foundLandmassInColumn = false;
			for y = 0, iH - 1 do -- Checking column.
				local plot = Map.GetPlot(x, y);
				local landmass = plot:GetLandmass();
				if landmass == iLandmassID then -- Found a plot belonging to iLandmassID, set EastX to this column.
					foundLandmassInColumn = true;
					iEastX = x;
					break;
				end
			end
			if foundLandmassInColumn then -- Found EastX, done looking.
				break;
			end
		end
	else -- Landmass Xwraps.
		local landmassSpansEntireWorldX = true;
		for x = iW - 2, 1, -1 do -- Check for end of landmass membership one column at a time, right to left.
			local foundLandmassInColumn = false;
			for y = 0, iH - 1 do -- Checking column.
				local plot = Map.GetPlot(x, y);
				local landmass = plot:GetLandmass();
				if landmass == iLandmassID then -- Found a plot belonging to iAreaID, will have to check the next column too.
					foundLandmassInColumn = true;
				end
			end
			if not foundLandmassInColumn then -- Found empty column, which is just west of WestX.
				iWestX = x + 1;
				landmassSpansEntireWorldX = false;
				break;
			end
		end
		for x = 1, iW - 2 do -- Check for end of landmass membership one column at a time, left to right.
			local foundLandmassInColumn = false;
			for y = 0, iH - 1 do -- Checking column.
				local plot = Map.GetPlot(x, y);
				local landmass = plot:GetLandmass();
				if landmass == iLandmassID then -- Found a plot belonging to iLandmassID, will have to check the next column too.
					foundLandmassInColumn = true;
				end
			end
			if not foundLandmassInColumn then -- Found empty column, which is just east of EastX.
				iEastX = x - 1;
				landmassSpansEntireWorldX = false;
				break;
			end
		end
		-- If landmass spans entire world, we'll treat it as if it does not wrap.
		if landmassSpansEntireWorldX then
			wrapsX = false;
			iWestX = 0;
			iEastX = iW - 1;
		end
	end

	-- Find South and North edges of this landmass.
	if not wrapsY then -- no Y wrap
		for y = 0, iH - 1 do -- Check for any landmass membership one row at a time, bottom to top.
			local foundLandmassInRow = false;
			for x = 0, iW - 1 do -- Checking row.
				local plot = Map.GetPlot(x, y);
				local landmass = plot:GetLandmass();
				if landmass == iLandmassID then -- Found a plot belonging to iLandmassID, set SouthY to this row.
					foundLandmassInRow = true;
					iSouthY = y;
					break;
				end
			end
			if foundLandmassInRow then -- Found SouthY, done looking.
				break;
			end
		end
		for y = iH - 1, 0, -1 do -- Check for any landmass membership one row at a time, top to bottom.
			local foundLandmassInRow = false;
			for x = 0, iW - 1 do -- Checking row.
				local plot = Map.GetPlot(x, y);
				local landmass = plot:GetLandmass();
				if landmass == iLandmassID then -- Found a plot belonging to iLandmassID, set NorthY to this row.
					foundLandmassInRow = true;
					iNorthY = y;
					break;
				end
			end
			if foundLandmassInRow then -- Found NorthY, done looking.
				break;
			end
		end
	else -- Landmass Ywraps.
		local landmassSpansEntireWorldY = true;
		for y = iH - 2, 1, -1 do -- Check for end of landmass membership one row at a time, top to bottom.
			local foundLandmassInRow = false;
			for x = 0, iW - 1 do -- Checking row.
				local plot = Map.GetPlot(x, y);
				local landmass = plot:GetLandmass();
				if landmass == iLandmassID then -- Found a plot belonging to iLandmassID, will have to check the next row too.
					foundLandmassInRow = true;
				end
			end
			if not foundLandmassInRow then -- Found empty row, which is just south of southY.
				iSouthY = y + 1;
				landmassSpansEntireWorldY = false;
				break
			end
		end
		for y = 1, iH - 2 do -- Check for end of landmass membership one row at a time, bottom to top.
			local foundLandmassInRow = false;
			for x = 0, iW - 1 do -- Checking row.
				local plot = Map.GetPlot(x, y);
				local landmass = plot:GetLandmass();
				if landmass == iLandmassID then -- Found a plot belonging to iLandmassID, will have to check the next row too.
					foundLandmassInRow = true;
				end
			end
			if not foundLandmassInRow then -- Found empty column, which is just north of NorthY.
				iNorthY = y - 1;
				landmassSpansEntireWorldY = false;
				break
			end
		end
		-- If landmass spans entire world, we'll treat it as if it does not wrap.
		if landmassSpansEntireWorldY then
			wrapsY = false;
			iSouthY = 0;
			iNorthY = iH - 1;
		end
	end

	-- Convert EastX and NorthY into width and height.
	if wrapsX then
		iWidth = (iEastX + iW) - iWestX + 1;
	else
		iWidth = iEastX - iWestX + 1;
	end
	if wrapsY then
		iHeight = (iNorthY + iH) - iSouthY + 1;
	else
		iHeight = iNorthY - iSouthY + 1;
	end

	---[[ Log dump for debug purposes only, disable otherwise.
	print("--- Landmass Boundary Readout ---");
	print("West X:", iWestX, "East X:", iEastX);
	print("South Y:", iSouthY, "North Y:", iNorthY);
	print("Width:", iWidth, "Height:", iHeight);
	local plotTotal = iWidth * iHeight;
	print("Total Plots in 'landmass rectangle':", plotTotal);
	print("- - - - - - - - - - - - - - - - -");
	--]]

	-- Insert data into table, then return the table.
	local data = {iWestX, iSouthY, iEastX, iNorthY, iWidth, iHeight, wrapsX, wrapsY};
	return data;
end
------------------------------------------------------------------------------
--                             REFERENCE
------------------------------------------------------------------------------
--[[
APPENDIX A - ORDER OF OPERATIONS

1. StartPlotSystem() is called from MapGenerator() in MapGenerator.lua

2. If left to default, StartPlotSystem() executes. However, since the core map
options (World Age, etc) are now custom-handled in each map script, nearly every
script needs to overwrite this function to process the Resources option. Many
scripts also need to pass in arguments, such as division method or bias adjustments.

3. AssignStartingPlots.Create() organizes the table that includes all member
functions and all common data.

4. AssignStartingPlots.Create() executes __Init() and __InitLuxuryWeights() to
populate data structures with values applicable to the current game and map. An
empty override is also provided via __CustomInit() to allow for easy modifications
on a small scale, instead of needing to replace all of the Create() function: for
instance, if a script wants to change only a couple of values in the self dot table.

5. AssignStartingPlots:DivideIntoRegions() is called. This function and its
children carry out the creation of Region Data, to be acted on by later methods.
-a. Division method is chosen by parameter. Default is Method 2, Continental.
-b. Four core methods are included. Refer to the function for details.
-c. If applicable, start locations are assigned to specific Areas (landmasses).
-d. Each populated landmass is processed. Any with more than one civ assigned
to them are divided in to Regions. Any with one civ are designated as a Region.
If a Rectuangular method is chosen, the map is divided without regard to Areas.
-e. Regional division occurs based on "Start Placement Fertility" measurements,
which are hard-coded in the function that measures the worth of a given plot. To
change these values for a script or mod, override the applicable function.
-f. All methods generate a database of Regions. The data includes coordinates
of the southwest corner of the region, plus width and height. If width or
height, counting from the SW corner, would exceed a map edge, world-wrap is
implied. As such, all processes that act on Regions need to account for wrap.
Other data included are the AreaID of the region (-1 if a division method in
use that ignores Areas), the total Start Placement Fertility measured in that
region, the total plot count of the region's rectangle, and fertility/plot. (I
was not told until later that no Y-Wrap support would be available. As such,
the entire default system is wired for Y-Wrap support, which seems destined to
lie dormant unless this code is re-used with Civ6 or some other game with Y-Wrap.)

6. AssignStartingPlots:ChooseLocations() is called.
-a. Each Region defined in self.regionData has its terrain types measured.
-b. Using the terrain types, each Region is classified. The classifications
should match the definitions in Regions.XML -- or Regions.xml needs to be
altered to match the internal classifications of any modified process. The
Regional classifications affect favored types of terrain for the start plot
selection in that Region, plus affect matching of start locations with those
civilizations who come with Terrain Bias (preferring certain conditions to
support their specific abilities), as well as the pool from which the Region's
Luxury type will be selected.
-c. An order of processing for Regions is determined. Regions of lowest average
fertility get their start plots chosen first. When a start plot is selected, it
creates a zone around itself where any additional starts will be reluctant to
appear, so the order matters. We give those with the worst land the best pick
of the land they have, while those with the best land will be the ones (if any)
to suffer being "pushed around" by proximity to already-chosen start plots.
-d. Start plots are chosen. There is a method that forces starts to occur along
the oceans, another method that allows for inland placement, and a third method
that ignores AreaID and instead looks for the most fertile Area available.

7. AssignStartingPlots:BalanceAndAssign() is called.
-a. Each start plot is evaluated for land quality. Those not meeting playable
standards are modified via adding Bonus Resources, Oases, or Hills. Ice, if
any, is removed from the waters immediately surrounding the start plot.
-b. The civilizations active in the current game are checked for Terrain Bias.
Any civs with biases are given first pick of start locations, seeking their
particular type of bias. Then civs who have a bias against certain terrain
conditions are given pick of what is left. Finally, civs without bias are
randomly assigned to the remaining regions.
-c. If the game is a Team game, start locations may be exchanged in an effort
to ensure that teammates start near one another. (This may upset Biases).

8. AssignStartingPlots:PlaceNaturalWonders() is called.
-a. All plots on the map are evaluated for eligibility for each Natural
wonder. Map scripts can overwrite eligibility calculations for any given NW
type, where desired. Lists of candidate plots are assembled for each NW.
-b. Some NW's with stricter eligibility may be prioritized to always appear
when eligible. The number of NWs that are eligible is checked against the
map. If the map can support more than the number allowed for that game (based
on map size), then the ones that will be placed are selected at random.
-c. The order of placement gives priority to any "always appear if eligible"
wonders, then priority after that goes to the wonder with the fewest candidates.
-d. There are minimum distance considerations for both civ starts and other
Natural Wonders, reflected in the Impact Data Layers. If collisions eliminate
all of a wonder's candidate plots, then a replacement wonder will be "pulled
off the bench and put in the game", if such a replacement is available.

9. AssignStartingPlots:PlaceResourcesAndCityStates() is called.
-a. Luxury resources are assigned roles. Each Region draws from a weighted
pool of Luxury types applicable to its dominant terrain type. This process
occurs according to Region Type, with Type 1 (Tundra) going first. Where
multiple regions of the same type occur, those within each category are
randomized in the selection order. When all regions have been matched to a
Luxury type (and each Luxury type can be spread across up to three regions)
then the City States pick three of the remaining types, at random. The
number of types to be disabled for that map size are removed, then the
remainder are assigned to be distributed globally, at random. Note that all
of these values are subject to modification. See Great Plains, for example.
-b. City States are assigned roles. If enough of them (1.35x civ count, at
least), then one will be assigned to each region. If the CS count way
exceeds the civ count, multiple CS may be assigned per region. Of those
not assigned to a region off the bat, the land of the map must be evaluated
to determine how much land exists outside of any region (if any). City
States get assigned to these "Uninhabited" lands next. Then we check for
any Luxuries that got split three-ways (a misfortune for those regions)
and, if there are enough unassigned CS remaining to give each such region
a bonus CS, this is done. Any remaining CS are awarded to Regions with the
lowest average fertility per land plot (and bound to have more total land
around as a result).
-c. The city state locations are chosen. Two methods exist: regional
placement, which strongly favors the edges of regions (civ starts strongly
favor the center of regions), and Uninhabited, which are completely random.
-d. Any city states that were unable to be placed where they were slated to
go (due to proximity collisions, aka "overcrowded area", then are moved to
a "last chance" fallback list and will be squeezed in anywhere on the map
that they can fit. If even that fails, the city state is discarded.
-e. Luxury resources are placed. Each civ gets at least one at its start
location. Regions of low fertility can get up to two more at their starts.
Then each city state gets one luxury, the type depending on what is
possible to place at their territory, crossed with the pool of types
available to city states in that game. Then, affected by what has already
been placed on the map, the amount of luxuries for each given region are
determined and placed. Finally, based on what has been placed so far, the
amount of the remaining types is determined, and they are placed globally.
-f. Each civ is given a second Luxury type at its start plot (except in
games using the Resources core map option available on most scripts, and
choosing the Sparse setting.) This second Luxury type CAN be Marble, which
boosts wonder production. (Marble is not in the normal rotation, though.)
-g. City State locations low on food (typical) or hammers get some help
in the normalization process: mostly food Bonus resources.
-h. Strategic resources are placed globally. The terrain balance greatly
affects location and quantity of various types and their balance. So the
game is going to play differently on different map scripts.
-i. Bonus resources are distributed randomly, with weightings. (Poor
terrain types get more assistance, particularly the Tundra. Hills regions
get extra Bonus support as well.)
-j. Various cleanup operations occur, including placing Oil in the sea,
fixing Sugar terrain -- and as the very last item, recalculating Area IDs,
which invalidates the entire Region Data pool, so it MUST come last.

10. Process ends. Map generation continues according to MapGenerator()
------------------------------------------------------------------------------

APPENDIX B - ADVICE FOR MODDERS

Depending upon the game areas being modified, it may be necessary to modify
the start placement and resource distribution code to support your effort.

-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --
If you are modifying Civilizations:

* You can plug your new or modified civilizations in to the Terrain Bias
system via table entries in Civ5Civilizations.xml

1. Start Along Ocean
2. Start Along River		-- Ended up inactive for the initial civ pool, but is operational in the code.
3. Start Region Priority
4. Start Region Avoid

Along Ocean is boolean, defaulting to false, and is processed first,
overriding all other concerns. Along River is boolean and comes next.

Priority and Avoid refer to "region types", of which there are eight. Each
of these region types is dominated by the associated terrain.
1. Tundra
2. Jungle
3. Forest
4. Desert
5. Hills
6. Plains
7. Grass
8. Hybrid (of plains and grass).

The defintions are sequential, so that a region that might qualify for
more than one designation gets the lowest-number it qualifies for.

The Priority and Avoid can be multiple-case. There are multiple-case Avoid
needs in the initial Civ pool, but only single-case Priority needs. This is
because the single-case needs have a fallback method that will place the civ
in the region with the most of its favored terrain type, if no region is
available that is dominated by that terrain. Whereas any Civ that has multiple
Priority needs must find an exact region match to one of its needs or it gets
no bias. Thus I found that all of the biases desired for the initial Civ pool
were able to be met via single Priority.

Any clash between Priority and Avoid, Priority wins.

I hope you enjoy this new ability to flavor and influence start locations.
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --

-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --
If you are modifying Resources:

XML no longer plays any role whatsoever on the distribution of Resources, but
it does still define available resource types and how they will interact with
the game.

Do not place a Luxury resource type at the top of the list, in ID slot #0.
Other than that, you can do what you will with the list and how they are ordered.

Be warned, there is NO automatic handling of resource balance and appearance for
new or modified resource types. Gone is the Civ4 method of XML-based terrain
permissions. Gone is plug-and-play with new resource types. If you remove any
types from the list, you will need to disable the hard-coded handling of those
types present in the resource distribution. If you modify or add types, you will
not see them in the game until you add them to the hard-coded distribution.

The distribution is handled wholly within this file, here in Lua. Whether you
approve of this change is your prerogative, but it does come with benefits in
the form of greatly increased power over how the resources are placed.

Bonus resources are used as the primary method for start point balancing, both
for civs and city states. Bonus Resources do nothing at all other than affect
yields of an individual tile and the type of improvement needed in that tile.

If you modify or add bonus resources, you may want to modify Normalization
methods as well, so your mod interacts with this subsystem. These are the
methods involved with Bonus resource normalization:

AttemptToPlaceBonusResourceAtPlot()
AttemptToPlaceHillsAtPlot()
AttemptToPlaceSmallStrategicAtPlot()
NormalizeStartLocation()
NormalizeCityState()
NormalizeCityStateLocations() -- Dependent on PlaceLuxuries being executed first.
AddExtraBonusesToHillsRegions()

Strategic Resources are now quantified, so their placement is no longer of the
nature of on/off, and having extra of something may help. Strategics are no longer
a significant part of the trading system. As such, their balance is looser in
Civ5 than it had to be in Civ4 or Civ3. Strategics are now placed globally, but
you can modify this to any method you choose. The default method is here:

PlaceStrategicAndBonusResources()

And it primarily relies upon the per-tile approach of this method:

ProcessResourceList()

Bonus resources are the same, for their global distribution. Additional functions
that provide custom handling are here:

AddStrategicBalanceResources() -- Only called when this game launch option is in effect.
PlaceMarble()
PlaceSmallQuantitiesOfStrategics()
PlaceFish()
PlaceSexyBonusAtCivStarts()
PlaceOilInTheSea()
FixResourceGraphics()

All three types of Resources, which each play a different role in the game, are
dependent upon the new "Impact and Ripple" data layers to detect previously
placed instances and remain at appropriate distances. By replacing a singular,
hardcoded "minimum distance" with the impact-and-ripple, I have been able to
introduce variable distances. This creates a less contrived-looking result and
allows for some clustering of resources without letting the clusters grow too
large, and without predetermining the nature and distribution of clusters. You
can most easily understand the net effect of this change by examing the fish
distribution. You will see that is less regular than the Civ4 method, yet still
avoids packing too many fish in to any given area. Larger clusters are possible
but quite rare, and the added variance should make city placement more interesting.

Another benefit of the Impact and Ripple is that it is Hex Accurate. The ripples
form a true hexagon (radiating outward from the impact plot, with weightings and
biases weakening the farther away from the impact it sits) instead of a rectangle
defined by an x-y coordinate area scan.

What this means for you, as a Resources modder, is that you will need to grasp
the operation of Impact and Ripple in order to properly calibrate any placements
of resources that you decide to make. This is true for all three types. Each has
its own layer of Impact and Ripple, but you could choose to remove a resource
from participation in a given layer, assign it to a different layer or to its own
layer, or even discard this method and come up with your own. Realize that each
resource placed will impact its layer, rippling outward from its plot to whatever
radius range you have selected, and then bar any later placements from being close
to that resource.

Everywhere in this code that a civ start is placed, or a city state, or a resource,
there are associated Impacts and Ripples on one or more data layers. The
interaction of all this activity is why a common database was needed. Yet because
none of this data affects the game after game launch and the map has been set, it
is all handled locally here in Lua, then is discarded and its memory recycled.

Meanwhile, Luxury resources are more tightly controlled than ever. The Regional
Division methods are as close to fair as I could make them, considering the highly
varied and unpredicatable landforms generated by the various map scripts. They
are fair enough to form a basis for distributing Luxuries in a way to create
supply and demand, to foster trade and diplomacy.


All Luxury resource placements are handled via this method:

PlaceSpecificNumberOfResources()

This method also handles placement of sea-based sources of Oil.


Like ProcessResourceList(), this method acts upon a plot list fed to it, but
instead of handling large numbers of plots and placing one for every n plots, it
tends to handle much smaller number of plots, and will return the number it was
unable to place for whatever reason (collisions with the luxury data layer being
the main cause, and not enough members in the plot list to receive all the
resources being placed is another) so that fallback methods can try again.

As I mentioned earlier, XML no longer governs resource placement. Gone are the
XML terrain permissions, a hardwired "all or nothing" approach that could allow
a resource to appear in forest (any forest), or not. The new method allows for
more subtlety, such as creating a plot list that includes only forests on hills,
or which can allow a resource to appear along rivers in the plains but only
away from rivers in grasslands. The sky is the limit, now, when it comes to
customizing resource appearance. Any method you can measure and implement, and
translate in to a list of candidate plots, you can apply here.

The default permissions are now contained in an interaction between two married
functions: terrain-based plot lists and a function matching each given resource
to a selection of lists appropriate to that resource.

The three list-generating functions are these:

GenerateGlobalResourcePlotLists()
GenerateLuxuryPlotListsAtCitySite()
GenerateLuxuryPlotListsInRegion()


The indexing function is:

GetIndicesForLuxuryType()


The process uses one of these three list generations (depending on whether it
is currently trying to assign Luxuries globally, regionally, or in support of
a specific civ or city state start location).

Other methods determine WHICH Luxury fits which role and how much of it to place;
then these processes come up with candidate plot lists, and then the indexing
matches the appropriate lists to the specific luxury type. Finally, all of this
data is passed to the function that actually places the Luxury, which is:

PlaceSpecificNumberOfResources()


If you want to modify the terrain permissions of existing Luxury types, you
need only handle the list generators and the indexing function.


If you want to modify which Luxury types get assigned to which Region types:

__InitLuxuryWeights()
IdentifyRegionsOfThisType()
SortRegionsByType()
AssignLuxuryToRegion()

All weightings for regional matching are contained here. But beware, the
system has to handle up to 22 civilizations and 41 city states, so the
combination of iNumMaxAllowedForRegions and the number of regions to
which any given Luxury can be assigned must multiply out to more than 22.

The default system allows up to 8 types for regions, up to 3 regions per type,
factoring out to 24 maximum allowable, barely enough to cover 22 civs.

Perhaps in an Expansion pack, more Luxury types could be added, to ease the
stress on the system. As it is, I had to spend a lot of political capital
with Jon to get us to Fifteen Luxury Types, and have enough to make this
new concept and this new system work. The amount is plenty for the default
numbers of civs for each map size, though. If too many types come available
in a given game, it could upset the game balance regarding Happiness and Trade.


If you wish to add new Luxury types, there is quite a bit involved. You will
either have to plug your modifications in to the existing system, or replace
the entire system. I have worked to make it as easy as possible to interact
with the new system, documenting every function, every design element. And
since this entire system exists here in Lua, nothing is beyond your reach.

The "handy resource ID shortcuts" will free you from needing to order the
luxuries in the XML in any particular fashion. These IDs will adapt to the
XML list. But you will have to create new shortcuts for any added or renamed
Luxury types, and activate the shortcuts here:

__Init()

You will also need to deactivate or remove any code handling luxury types
that your mod removes. I recommend using an editor with a Find feature and
scan the file for all instances of keys that you want to remove. At each
instance found, if the key is in a group, you can safely remove it from
the group so long as the group retains at least one key in it. If the key
is the only one being acted upon, you may need to replace it with a different
key or else deactivate that chunk of code. (If the method attempts to act
upon a nil-value key, that will cause an Assert and the start finder will
exit without finishing all of its operations.)

If you are going to plug in to the new system, you need to determine if the
default terrain-based plot lists meet your needs. If not, create new list
types for all three list-generation methods and index them as applicable.

GenerateGlobalResourcePlotLists()
GenerateLuxuryPlotListsAtCitySite()
GenerateLuxuryPlotListsInRegion()
GetIndicesForLuxuryType()

You will also need to modify functions that determine which Luxury types
can be placed at City States (this affects which luxury each receives).

GetListOfAllowableLuxuriesAtCitySite()


Finally, the command center of Luxury Distribution is here:

PlaceLuxuries()
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --

-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --
If you are modifying Terrain:

You will need to update every aspect of the new system to make it operate
correctly on your terrain changes. Or you will need to replace the entire system.

I'm sorry if that is bad news, but if you modified Terrain for Civ4, you likely
experienced it being applied inconsistently (not working on lots of map scripts)
and ran in to barriers where something you wanted to do was impossible, because
only certain limited permissions were enabled through the XML. You may even have
had to rise to the level of modifying the game core DLLs in C++ to open up more
functionality.

Whatever interactions with the game core your terrain needs to make remain in the
XML and the C++. The only relevant aspects here involve how your terrain interacts
with map generation, start placement, and resource distribution.

I have modified the base map generation methods to include more parameters and
options, so that more of the map scripts can rely on them. This means less
hardcoding in individual scripts, to where an update to the core methods that
includes new terrain types or feature types will have a wider reach.

As for start placement and resources, a big part of Jon's vision for Civ5 was to
bring back grandiose terrain, realistically large regions of Desert, Tundra,
Plains, and so on. But this type of map, combined with the old start generation
method, tended to force starts on grassland, and do other things counter to the
vision. So I designed the new system to more accurately divide the map, not by
strict tile count, but by relative worth, trying to give each civ as fair a patch
of land as possible. Where the terrain would be too harsh, we would support it
with Bonus resources, which could now be placed in any quantity needed, thanks to
being untied from the trade system. The regonal division divides the map, then
the classification system identifies each region's dominant terrain type and aims
to give the civ who starts there a flavored environment, complete with a start in
or near that type of terrain, enough Bonus to remove the worst cases of "bad luck",
and a cluster of luxury resources at hand that is appropriate to that region type.

In doing all of this, I have hard-coded the system with countless assumptions
based on the default terrain. If you wish to make use of this system in tandem
with new types of terrain or with modified yields of existing terrain, or both,
you will need to rewire the system, mold it to the specific needs of the new
terrain balance that you are crafting.

This begins with the Start Placement Fertility, which is measured per plot here:

MeasureStartPlacementFertilityOfPlot()


Measurements are processed in two ways, but you likely don't need to mod these:

MeasureStartPlacementFertilityInRectangle()
MeasureStartPlacementFertilityOfLandmass()


Once you have regions dividing in ways appropriate to your new terrain, you will
need to update terrain measurements and regional classifications here:

MeasureTerrainInRegions()
DetermineRegionTypes()

You may also have XML work to do in regard to regions. The region list is here:
CIV5Regions.xml

And each Civilization's specific regional or terrain bias is found here:
CIV5Civilizations.xml


Start plot location is a rather sizable operation, spanning half a dozen functions.

MeasureSinglePlot()
EvaluateCandidatePlot()
IterateThroughCandidatePlotList()
FindStart()
FindCoastalStart()
FindStartWithoutRegardToAreaID()
ChooseLocations()

Depending on the nature of your modifications, you may need to recalibrate this
entire system. However, like with Start Fertility, the core of the system is
handled at the plot level, evaluating the meaning of each type of plot for each
type of region. I have enacted a simple mechanism at the core, with only four
categories of plot measurement: Food, Prod, Good, Junk. The food label may be
misleading, as this is the primary mechanism for biasing starting terrain. For
instance, in tundra regions I have tundra tiles set as Food, but grass are not.
A desert region sets Plains as Food but Grass is not, while a Jungle region sets
Grass as Food but Plains aren't. The Good tiles act as a hedge, and are the main
way of differentiating one candidate site from another, so that among a group of
plots of similar terrain, the best tends to get picked. I also have the overall
standards set reasonably low to keep the Center Bias element of the system at
the forefront of start placement. This is chiefly because the exact quality of
the initial starting location is less urgent than maintaining as good of a
positioning as possible among civs. Balancing the quality of the start plot
against positioning near the center of each region was a fun challenge to
tackle, and I feel that I have succeeded in my design goals. Just be aware that
any change loosening the bias toward the center could ripple through the system.

Regional terrain biases that purposely put starts in non-ideal terrain are
intended to be supported via Normalization and other compensations built in to
the system in general. Yet the normalization used in Civ5 is much more lightly
applied than Civ4's methods. The new system modifies the actual terrain as
little as possible, giving support mostly through the addition of Bonus type
resources, which add food. Jon wanted starts to occur in a variety of terrain
yet for each to be competitive. He directed me to use resources to balance it.


If your terrain modifications affect tile yields, or introduce new elements in
to the ecosystem, it is likely your mod would benefit from adjusting the start
site normalization process.

AttemptToPlaceBonusResourceAtPlot()
AttemptToPlaceHillsAtPlot()
AttemptToPlaceSmallStrategicAtPlot()
NormalizeStartLocation()
PlaceSexyBonusAtCivStarts()
AddExtraBonusesToHillsRegions()

City State placement is subordinate to civ placement, in the new system. The
city states get no consideration whatsoever to the quality of their starts, only
to the location relative to civilizations. So this is the one area of the system
that is likely to be unaffected by terrain mods, except at Normalization:

NormalizeCityState()
NormalizeCityStateLocations()


Finally, a terrain mod is sure to scramble the hard-coded and carefully balanced
resource distribution system. That entire system is predicated upon the nature
of default terrain, what it yields, how the pieces interact, how they are placed
by map scripts, and in general governed by a measured sense of realism, informed
by gameplay needs and a drive for simplicity. From the order of operations upon
regions (sorted by dominant terrain type) to the interwoven nature of resource
terrain preferences, it is unlikely the default implementation will properly
support any significant terrain mod. The work needed to integrate a terrain mod
in to resource distribution would be similar to that needed for a resource mod,
so I will refer you back to that section of the appendix for additional info.
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --

-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --
If you are modifying game rules:

The new system is rife with dependencies upon elements in the default rules. This
need not be a barrier to your mod, but it will surely assist in your cause to
be alert to possible side effects of game rules changes. One of the biggest
dependencies lies with rules that govern tile improvements: how much they benefit,
where they are possible to build, when upgrades to their yield output come online
and so forth. Evaluations for Fertility that governs regional division, for
Normalization that props up weaker locations, and the logic of resource distribution
(such as placing numerous Deer in the tundra to make small cities viable there)
all depend in large part on the current game rules. So if, for instance, your mod
were to remove or push back the activation of yield boost at Fresh Water farms,
this would impact the accuracy of the weighting that the start finder places on
fresh water plots and on fresh water grasslands in particular. This is the type of
assumption built in to the system now. In a way it is unfriendly to mods, but it
also provides a stronger support for the default rule set, and sets an example of
how the system could support mods as well, if re-calibrated successfully.

The start placement and resource distribution systems include no mechanism for
automatically detecting game rules modifications, or terrain or resource mods,
either. So to the degree that your mod may impact the logic of start placement,
you may want to consider making adjustments to the system, to ensure that it
behaves in ways productive to your mod.
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --

------------------------------------------------------------------------------

APPENDIX C - ACKNOWLEDGEMENTS

Thanks go to Jon for turning me loose on this system. I had the vision for
this system as far back as the middle of vanilla Civ4 development, but I did
not get the opportunity to act on it until now. Designing, coding, testing
and implementing my baby here have been a true pleasure. That the effort has
enabled a key element of Jon's overall vision for the game is a pride point.

Thanks to Ed Beach for his brilliant algorithm, which has enhanced the value
and performance of this system to a great degree.

Thanks to Shaun Seckman and Brian Wade for numerous instances of assistance
with Lua programming issues, and for providing the initial ports of Python to
Lua, which gave me an easy launching point for all of my tasks.

Thanks to everyone on the Civ5 development team whom I met on my visit to
Firaxis HQ in Baltimore, who were so warm and welcoming and supportive. It
has been a joy to be part of such a positive working environment and to
contribute to a team like this. If every gamer got to see the inside of
the studio and how things really work, I believe they would be inspired.

Thanks to all on the web team, who provided direct and indirect support. I
can't reveal any details here, but each of you knows who you are.

Finally, special thanks to my wife, Jaime, who offered advice, input,
feedback and general support throughout my design and programming effort.

- Robert B. Thomas (Sirian)		April 26, 2010
--]]
------------------------------------------------------------------------------
