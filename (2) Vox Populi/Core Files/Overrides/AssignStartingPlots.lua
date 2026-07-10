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
include("VPUI_core");

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

-- azum4roll - 2024-01-14 (completed in version 4.5)

------------------------------------------------------------------------------

-- Due to how global variables work in Lua, every time this file is called, the AssignStartingPlots (ASP) table would be reset to its original state,
-- removing all overrides that might be done in other modmods. There were two ways to successfully override ASP functions:
-- 1. Override the entire AssignStartingPlots.lua file, using VFS (which is what this file is doing).
--    This method cannot be used for overriding specific functions; you have to recreate the entire ASP table.
-- 2. Override the functions in StartPlotSystem() in mapscripts, which is called after ASP is loaded.
--    This only works in mapscripts, as they're the only files loaded after ASP is last loaded before generating a map.

-- I've created a new method to override individual ASP functions.
-- A new EntryPoint/Content type "PreMapGenScript" has been added to load arbitrary Lua files (with ImportIntoVFS=true) just before the map is generated,
-- which you can use to modify the ASP table. See MapGenerator.lua for details.

--[[Example Lua for overriding an ASP function:

	-- Increase the number of natural wonders
	function AssignStartingPlots:GetNumNaturalWondersToPlace(iWorldSize)
		local tTarget = {
			[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 3,
			[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 6,
			[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 9,
			[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 12,
			[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 15,
			[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 18,
		};
		return tTarget[iWorldSize];
	end
]]

-- This also works in Community Patch Only.

-- azum4roll - 2025-03-07

------------------------------------------------------------------------------
--- @class AssignStartingPlots
local ASP = {};

--- @enum RegionDivision
--- Enum values for region division methods
RegionDivision = {
	BIGGEST_LANDMASS = 1,
	CONTINENTAL = 2,
	RECTANGULAR = 3,
	RECTANGULAR_SELF_DEFINED = 4,
};

--- @enum ImpactLayer
--- Enum values for impact layers
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
	LAYER_MAJOR = 14,
};

--- @enum PlotListType
--- Enum values for plot lists
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

local VP = MapModData and MapModData.VP or VP;
local GameInfoCache = VP.GameInfoCache;
local GameInfoTypes; --- @type table<string, integer>
local L = Locale.Lookup;

local NUM_REGION_TYPES = 11;
local NUM_IMPACT_LAYERS = 14;
local NUM_NATURAL_WONDERS = #GameInfo.Natural_Wonder_Placement;
local MAX_MAJOR_CIVS = GameDefines.MAX_MAJOR_CIVS;
local MAX_MINOR_CIVS = GameDefines.MAX_MINOR_CIVS;
local MAX_RESOURCE_INDEX = #GameInfo.Resources - 1;

------------------------------------------------------------------------------
--[[
WARNING: There must not be any recalculation of AreaIDs at any time during the execution of any operations in or attached to this table.
Recalculating will invalidate all data based on AreaID relationships with plots, which will destroy Regional definitions for all but the Rectangular method.
A fix for scrambled AreaID data is theoretically possible, but I have not spent development resources and time on this, directing attention to other tasks.
--]]
------------------------------------------------------------------------------

ASP.__index = ASP;

--- Create a new instance of AssignStartingPlots
--- @return AssignStartingPlots
function ASP.Create()
	local instance = setmetatable({}, ASP);

	instance:__Init();

	instance:__InitLuxuryWeights();

	-- Entry point for easy overrides, for instance if only a couple things need to change.
	instance:__CustomInit();

	return instance;
end

------------------------------------------------------------------------------
--- Core Process member methods
------------------------------------------------------------------------------

function ASP:__Init()
	local iMapW, iMapH = Map.GetGridSize();

	-- Populate GameInfoTypes here (if needed)
	GameInfoTypes = VP.GameInfoTypes;

	-- Set up data tables that record whether a plot is coastal land and whether a plot is adjacent to coastal land
	self.plotDataIsCoastal, self.plotDataIsNextToCoast = GenerateNextToCoastalLandDataTables();

	-- Set up resource IDs for quick access
	-- This isn't any quicker now that we have GameInfoTypes available, but we don't want to break everything that overrides ASP yet

	-- Bonus resources
	self.wheat_ID = GameInfoTypes.RESOURCE_WHEAT;
	self.cow_ID = GameInfoTypes.RESOURCE_COW;
	self.deer_ID = GameInfoTypes.RESOURCE_DEER;
	self.banana_ID = GameInfoTypes.RESOURCE_BANANA;
	self.fish_ID = GameInfoTypes.RESOURCE_FISH;
	self.sheep_ID = GameInfoTypes.RESOURCE_SHEEP;
	self.stone_ID = GameInfoTypes.RESOURCE_STONE;
	self.bison_ID = GameInfoTypes.RESOURCE_BISON;
	-- Strategic resources
	self.iron_ID = GameInfoTypes.RESOURCE_IRON;
	self.horse_ID = GameInfoTypes.RESOURCE_HORSE;
	self.coal_ID = GameInfoTypes.RESOURCE_COAL;
	self.oil_ID = GameInfoTypes.RESOURCE_OIL;
	self.aluminum_ID = GameInfoTypes.RESOURCE_ALUMINUM;
	self.uranium_ID = GameInfoTypes.RESOURCE_URANIUM;
	-- Luxury resources
	self.whale_ID = GameInfoTypes.RESOURCE_WHALE;
	self.pearls_ID = GameInfoTypes.RESOURCE_PEARLS;
	self.ivory_ID = GameInfoTypes.RESOURCE_IVORY;
	self.fur_ID = GameInfoTypes.RESOURCE_FUR;
	self.silk_ID = GameInfoTypes.RESOURCE_SILK;
	self.dye_ID = GameInfoTypes.RESOURCE_DYE;
	self.spices_ID = GameInfoTypes.RESOURCE_SPICES;
	self.sugar_ID = GameInfoTypes.RESOURCE_SUGAR;
	self.cotton_ID = GameInfoTypes.RESOURCE_COTTON;
	self.wine_ID = GameInfoTypes.RESOURCE_WINE;
	self.incense_ID = GameInfoTypes.RESOURCE_INCENSE;
	self.gold_ID = GameInfoTypes.RESOURCE_GOLD;
	self.silver_ID = GameInfoTypes.RESOURCE_SILVER;
	self.gems_ID = GameInfoTypes.RESOURCE_GEMS;
	self.marble_ID = GameInfoTypes.RESOURCE_MARBLE;
	-- DLC resources
	self.copper_ID = GameInfoTypes.RESOURCE_COPPER;
	self.salt_ID = GameInfoTypes.RESOURCE_SALT;
	self.citrus_ID = GameInfoTypes.RESOURCE_CITRUS;
	self.truffles_ID = GameInfoTypes.RESOURCE_TRUFFLES;
	self.crab_ID = GameInfoTypes.RESOURCE_CRAB;
	self.cocoa_ID = GameInfoTypes.RESOURCE_COCOA;
	-- VP additional resources
	self.maize_ID = GameInfoTypes.RESOURCE_MAIZE;
	self.rice_ID = GameInfoTypes.RESOURCE_RICE;
	self.coffee_ID = GameInfoTypes.RESOURCE_COFFEE;
	self.tea_ID = GameInfoTypes.RESOURCE_TEA;
	self.tobacco_ID = GameInfoTypes.RESOURCE_TOBACCO;
	self.amber_ID = GameInfoTypes.RESOURCE_AMBER;
	self.jade_ID = GameInfoTypes.RESOURCE_JADE;
	self.olives_ID = GameInfoTypes.RESOURCE_OLIVE;
	self.perfume_ID = GameInfoTypes.RESOURCE_PERFUME;
	self.coral_ID = GameInfoTypes.RESOURCE_CORAL;
	self.lapis_ID = GameInfoTypes.RESOURCE_LAPIS;
	-- Even More Resources for Vox Populi
	self.lavender_ID = GameInfoTypes.RESOURCE_LAVENDER;
	self.obsidian_ID = GameInfoTypes.RESOURCE_OBSIDIAN;
	self.platinum_ID = GameInfoTypes.RESOURCE_PLATINUM;
	self.poppy_ID = GameInfoTypes.RESOURCE_POPPY;
	self.tin_ID = GameInfoTypes.RESOURCE_TIN;
	self.coconut_ID = GameInfoTypes.RESOURCE_COCONUT;
	self.hardwood_ID = GameInfoTypes.RESOURCE_HARDWOOD;
	self.lead_ID = GameInfoTypes.RESOURCE_LEAD;
	self.pineapple_ID = GameInfoTypes.RESOURCE_PINEAPPLE;
	self.potato_ID = GameInfoTypes.RESOURCE_POTATO;
	self.rubber_ID = GameInfoTypes.RESOURCE_RUBBER;
	self.sulfur_ID = GameInfoTypes.RESOURCE_SULFUR;
	self.titanium_ID = GameInfoTypes.RESOURCE_TITANIUM;
	-- Aloe Vera bonus resource
	self.aloevera_ID = GameInfoTypes.RESOURCE_JAR_ALOE_VERA;
	-- Additional Luxury Resources for VP
	self.alpaca_ID = GameInfoTypes.RESOURCE_ALPACA;
	self.camel_ID = GameInfoTypes.RESOURCE_CAMEL;
	self.quartz_ID = GameInfoTypes.RESOURCE_QUARTZ;
	self.feathers_ID = GameInfoTypes.RESOURCE_FEATHERS;

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
	self.bArea = false; -- Whether region division is based on landmass or area (true = area-based)
	self.landmass_areas = {}; -- Stores lists of areas of each landmass
	self.player_ID_list = {}; -- Correct list of player IDs (includes handling of any 'gaps' that occur in MP games)
	self.regionData = {}; -- Stores data returned from regional division algorithm
	self.regionTerrainCounts = {}; -- Stores counts of terrain elements for all regions
	self.regionTypes = {}; -- Stores region types
	self.playerCollisionData = table.fill(false, iMapW * iMapH); -- Stores "impact" data only, of start points, to avoid player collisions
	--- @type {IsCoastal: boolean, IsLake: boolean, IsRiver: boolean, IsNearRiver: boolean, DesertCount: integer, TundraCount: integer}[]
	self.startLocationConditions = {}; -- Stores info regarding conditions at each start location
	self.iNumMountainBias = 0; -- Stores number of civs that prefer to start near mountains
	self.iNumSnowBias = 0; -- Stores number of civs that prefer to start near snow

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
	self.amounts_of_resources_placed = table.fill(0, MAX_RESOURCE_INDEX + 1); -- Stores amounts of each resource ID placed. WARNING: This table uses adjusted resource ID (+1) to account for Lua indexing. Add 1 to all IDs to index this table.
	self.luxury_assignment_count = table.fill(0, MAX_RESOURCE_INDEX); -- Stores amount of each luxury type assigned to regions. WARNING: current implementation will crash if a Luxury is attached to resource ID 0 (default = iron), because this table uses unadjusted resource ID as table index.
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
	self.minProdInner = 1;
	self.minGoodInner = 3;
	self.minFoodMiddle = 4;
	self.minProdMiddle = 1;
	self.minGoodMiddle = 6;
	self.minFoodOuter = 4;
	self.minProdOuter = 3;
	self.minGoodOuter = 8;

	-- Init impact tables
	for i = 1, NUM_IMPACT_LAYERS do
		self.impactData[i] = table.fill(0, iMapW * iMapH);
	end

	-- Populate landmass/area relationship table here, since many maps override GenerateRegions() and we cannot do it there.
	local tAreas = {};
	for _, plot in Plots() do
		if not plot:IsWater() then -- Land plot, process it.
			local iLandmass = plot:GetLandmass();
			local iArea = plot:GetArea();
			if not self.landmass_areas[iLandmass] then -- This plot is the first detected in its landmass
				self.landmass_areas[iLandmass] = {};
			end
			if not tAreas[iArea] then -- This plot is the first detected in its area.
				tAreas[iArea] = true;
				table.insert(self.landmass_areas[iLandmass], iArea);
				-- print("Area#", iArea, "is a new area in landmass#", iLandmass);
			end
		end
	end

	-----------------------------------------------------------
	-- Configurations
	-- It should be easier to override these instead of functions
	-----------------------------------------------------------

	self.MIN_LANDMASS_FERTILITY = 0.05; -- CONSTANT: Minimum fertility (out of map total) required for a landmass to be counted as part of a region

	self.MAX_LUXURIES_ASSIGNED_TO_REGION = 16; -- CONSTANT: Maximum luxury types allowed to be assigned to regional distribution. CANNOT be reduced below 8!
	self.MAX_REGIONS_PER_LUXURY = 3; -- CONSTANT: Maximum number of regions allowed for each luxury to be a regional of
	-- The above constants must multiply to more than the maximum number of civs allowed (43 for VP)

	self.NUM_WATER_NEEDED_PER_REGIONAL_LUXURY = 8; -- CONSTANT: Number of coast tiles needed for each regional luxury deposit

	-- Start evaluation criteria

	-- Food/Prod score lookup tables for each number of tiles evaluated as Food/Prod.
	-- Value is +1 for each number out of index.
	-- e.g. 15 food in the first 2 rings has 34 + (15 - 9) = 40 score.
	self.WEIGHTED_FOOD_SCORE = {
		{0, 8, 14, 18, 20},
		{0, 2, 5, 10, 20, 25, 28, 30, 32, 34},
		{0},
	};

	self.WEIGHTED_PROD_SCORE = {
		{0, 10, 16, 20, 22, 24},
		{0, 10, 20, 25, 30, 35, 39, 42, 44},
		{0},
	}

	self.GOOD_TILE_MULTIPLIER = {2, 2, 1};
	self.JUNK_TILE_MULTIPLIER = {-3, -3, -2};

	--- Minimum ratio of specific terrain/feature in the region to be classified as each region type
	self.REGION_TYPE_MIN_REQ = {
		[GameInfoTypes.REGION_TUNDRA] = 0.20,
		[GameInfoTypes.REGION_JUNGLE] = 0.24,
		[GameInfoTypes.REGION_FOREST] = 0.24,
		[GameInfoTypes.REGION_DESERT] = 0.20,
		[GameInfoTypes.REGION_HILLS] = 0.37,
		[GameInfoTypes.REGION_PLAINS] = 0.30,
		[GameInfoTypes.REGION_GRASS] = 0.30,
	};
end
------------------------------------------------------------------------------
--- Initialize luxury data table. Index = Region Type<br>
--- Customize this function if the terrain will fall significantly outside Earth normal, or if region definitions have been modified.
---
--- Note: The water-based luxuries are set to appear in a region only if that region has its start on the coast.<br>
--- So the weights shown for those are reduced in practice to the degree that a map has inland starts.
function ASP:__InitLuxuryWeights()
	-- Tundra
	self.luxury_region_weights[1] = {
		{self.fur_ID,    60},
		{self.amber_ID,  35},
		{self.silver_ID, 30},
		{self.copper_ID, 20},
		{self.salt_ID,   15},
		{self.gems_ID,   10},
		{self.jade_ID,   10},
		{self.lapis_ID,  10},
		{self.whale_ID,  10},
		{self.crab_ID,   10},
		{self.pearls_ID, 10},
		{self.coral_ID,  10},
	};

	if self:IsModActive("EvenMoreResources") then
		table.insert(self.luxury_region_weights[1], {self.obsidian_ID, 10});
		table.insert(self.luxury_region_weights[1], {self.platinum_ID, 10});
		table.insert(self.luxury_region_weights[1], {self.tin_ID,      10});
	end
	if self:IsModActive("AdditionalLuxuries") then
		table.insert(self.luxury_region_weights[1], {self.quartz_ID,   20});
	end

	-- Snow = tundra
	self.luxury_region_weights[10] = self.luxury_region_weights[1];

	 -- Jungle
	self.luxury_region_weights[2] = {
		{self.citrus_ID,   35},
		{self.cocoa_ID,    35},
		{self.spices_ID,   35},
		{self.sugar_ID,    35},
		{self.truffles_ID, 15},
		{self.silk_ID,     10},
		{self.dye_ID,      10},
		{self.gems_ID,     10},
		{self.whale_ID,    10},
		{self.crab_ID,     10},
		{self.pearls_ID,   10},
		{self.coral_ID,    10},
	};

	if self:IsModActive("EvenMoreResources") then
		table.insert(self.luxury_region_weights[2], {self.obsidian_ID, 35});
		table.insert(self.luxury_region_weights[2], {self.poppy_ID,    35});
		table.insert(self.luxury_region_weights[2], {self.platinum_ID, 10});
		table.insert(self.luxury_region_weights[2], {self.tin_ID,      10});
	end
	if self:IsModActive("AdditionalLuxuries") then
		table.insert(self.luxury_region_weights[2], {self.feathers_ID, 35});
	end

	-- Forest
	self.luxury_region_weights[3] = {
		{self.truffles_ID, 40},
		{self.silk_ID,     35},
		{self.dye_ID,      35},
		{self.citrus_ID,   15},
		{self.cocoa_ID,    15},
		{self.spices_ID,   15},
		{self.whale_ID,    08},
		{self.crab_ID,     08},
		{self.pearls_ID,   08},
		{self.coral_ID,    08},
	};

	if self:IsModActive("EvenMoreResources") then
		table.insert(self.luxury_region_weights[3], {self.lavender_ID, 15});
	end
	if self:IsModActive("AdditionalLuxuries") then
		table.insert(self.luxury_region_weights[3], {self.feathers_ID, 35});
	end

	-- Desert
	self.luxury_region_weights[4] = {
		{self.incense_ID, 35},
		{self.lapis_ID,   35},
		{self.gold_ID,    20},
		{self.silver_ID,  20},
		{self.salt_ID,    15},
		{self.jade_ID,    15},
		{self.copper_ID,  10},
		{self.amber_ID,   10},
		{self.gems_ID,    10},
		{self.sugar_ID,   10},
		{self.cotton_ID,  10},
		{self.whale_ID,   10},
		{self.crab_ID,    10},
		{self.pearls_ID,  10},
		{self.coral_ID,   10},
	};

	if self:IsModActive("EvenMoreResources") then
		table.insert(self.luxury_region_weights[4], {self.obsidian_ID, 10});
		table.insert(self.luxury_region_weights[4], {self.platinum_ID, 10});
		table.insert(self.luxury_region_weights[4], {self.tin_ID,      10});
	end
	if self:IsModActive("AdditionalLuxuries") then
		table.insert(self.luxury_region_weights[4], {self.quartz_ID,   10});
		table.insert(self.luxury_region_weights[4], {self.camel_ID,    50});
	end

	-- Hills
	self.luxury_region_weights[5] = {
		{self.gold_ID,   20},
		{self.copper_ID, 20},
		{self.gems_ID,   20},
		{self.jade_ID,   20},
		{self.salt_ID,   10},
		{self.amber_ID,  10},
		{self.lapis_ID,  10},
		{self.whale_ID,  06},
		{self.crab_ID,   06},
		{self.pearls_ID, 06},
		{self.coral_ID,  06},
	};

	if self:IsModActive("EvenMoreResources") then
		table.insert(self.luxury_region_weights[5], {self.obsidian_ID, 20});
		table.insert(self.luxury_region_weights[5], {self.poppy_ID,    20});
		table.insert(self.luxury_region_weights[5], {self.platinum_ID, 20});
		table.insert(self.luxury_region_weights[5], {self.tin_ID,      20});
	end
	if self:IsModActive("AdditionalLuxuries") then
		table.insert(self.luxury_region_weights[5], {self.alpaca_ID,   30});
		table.insert(self.luxury_region_weights[5], {self.quartz_ID,   20});
	end

	-- Mountain = hills
	self.luxury_region_weights[9] = self.luxury_region_weights[5];

	-- Plains
	self.luxury_region_weights[6] = {
		{self.wine_ID,    40},
		{self.olives_ID,  40},
		{self.coffee_ID,  40},
		{self.incense_ID, 20},
		{self.cotton_ID,  20},
		{self.salt_ID,    15},
		{self.tobacco_ID, 15},
		{self.tea_ID,     15},
		{self.perfume_ID, 15},
		{self.whale_ID,   11},
		{self.crab_ID,    11},
		{self.pearls_ID,  11},
		{self.coral_ID,   11},
	};

	if self:IsModActive("EvenMoreResources") then
		table.insert(self.luxury_region_weights[6], {self.lavender_ID, 40});
		table.insert(self.luxury_region_weights[6], {self.poppy_ID,    40});
	end
	if self:IsModActive("AdditionalLuxuries") then
		table.insert(self.luxury_region_weights[6], {self.alpaca_ID,   15});
	end

	-- Grass
	self.luxury_region_weights[7] = {
		{self.tobacco_ID, 50},
		{self.tea_ID,     50},
		{self.perfume_ID, 50},
		{self.cotton_ID,  35},
		{self.wine_ID,    20},
		{self.olives_ID,  20},
		{self.coffee_ID,  20},
		{self.sugar_ID,   10},
		{self.gold_ID,    10},
		{self.whale_ID,   14},
		{self.crab_ID,    14},
		{self.pearls_ID,  14},
		{self.coral_ID,   14},
	};

	if self:IsModActive("EvenMoreResources") then
		table.insert(self.luxury_region_weights[7], {self.lavender_ID, 50});
		table.insert(self.luxury_region_weights[7], {self.poppy_ID,    50});
	end
	if self:IsModActive("AdditionalLuxuries") then
		table.insert(self.luxury_region_weights[7], {self.alpaca_ID,   15});
	end

	-- Hybrid
	-- Favor very flexible resources, like resources that are mined or in the water.
	self.luxury_region_weights[8] = {
		{self.gold_ID,     30},
		{self.copper_ID,   30},
		{self.gems_ID,     30},
		{self.salt_ID,     30},
		{self.jade_ID,     30},
		{self.amber_ID,    30},
		{self.lapis_ID,    30},
		{self.coffee_ID,   05},
		{self.tobacco_ID,  05},
		{self.tea_ID,      05},
		{self.perfume_ID,  05},
		{self.cotton_ID,   05},
		{self.wine_ID,     05},
		{self.olives_ID,   05},
		{self.truffles_ID, 05},
		{self.silk_ID,     05},
		{self.dye_ID,      05},
		{self.citrus_ID,   05},
		{self.cocoa_ID,    05},
		{self.spices_ID,   05},
		{self.whale_ID,    20},
		{self.crab_ID,     20},
		{self.pearls_ID,   20},
		{self.coral_ID,    20},
	};

	if self:IsModActive("EvenMoreResources") then
		table.insert(self.luxury_region_weights[8], {self.obsidian_ID, 30});
		table.insert(self.luxury_region_weights[8], {self.platinum_ID, 30});
		table.insert(self.luxury_region_weights[8], {self.tin_ID,      30});
		table.insert(self.luxury_region_weights[8], {self.lavender_ID, 05});
		table.insert(self.luxury_region_weights[8], {self.poppy_ID,    05});
	end
	if self:IsModActive("AdditionalLuxuries") then
		table.insert(self.luxury_region_weights[8], {self.quartz_ID,   30});
	end

	-- Random / Fallback<br>
	-- Favor water resources since they work great as randoms and make the coasts more interesting.
	-- Also, slightly favor mined resources for their flexibility.
	--
	-- This table doubles as a list of available luxuries,
	-- so resources unviable as fallback (but aren't special case) are still listed here with a 0 weight.
	self.luxury_fallback_weights = {
		{self.gold_ID,     10},
		{self.copper_ID,   10},
		{self.gems_ID,     10},
		{self.salt_ID,     10},
		{self.jade_ID,     10},
		{self.amber_ID,    10},
		{self.lapis_ID,    10},
		{self.coffee_ID,   05},
		{self.tobacco_ID,  05},
		{self.tea_ID,      05},
		{self.perfume_ID,  05},
		{self.cotton_ID,   05},
		{self.wine_ID,     05},
		{self.olives_ID,   05},
		{self.incense_ID,  05},
		{self.truffles_ID, 05},
		{self.silk_ID,     05},
		{self.dye_ID,      05},
		{self.citrus_ID,   05},
		{self.cocoa_ID,    05},
		{self.spices_ID,   05},
		{self.whale_ID,    30},
		{self.crab_ID,     30},
		{self.pearls_ID,   30},
		{self.coral_ID,    30},
		{self.silver_ID,   00},
		{self.fur_ID,      00},
		{self.sugar_ID,    00},
	};

	if self:IsModActive("EvenMoreResources") then
		table.insert(self.luxury_fallback_weights, {self.obsidian_ID, 30});
		table.insert(self.luxury_fallback_weights, {self.platinum_ID, 30});
		table.insert(self.luxury_fallback_weights, {self.tin_ID,      30});
		table.insert(self.luxury_fallback_weights, {self.lavender_ID, 05});
		table.insert(self.luxury_fallback_weights, {self.poppy_ID,    05});
	end
	if self:IsModActive("AdditionalLuxuries") then
		table.insert(self.luxury_fallback_weights, {self.camel_ID,    05});
		table.insert(self.luxury_fallback_weights, {self.alpaca_ID,   05});
		table.insert(self.luxury_fallback_weights, {self.feathers_ID, 05});
		table.insert(self.luxury_fallback_weights, {self.quartz_ID,   30});
	end

	-- City States
	-- Slightly favor water resources since they're flexible and most city-states are coastal.
	-- Also, slightly favor mined resources for their flexibility.
	self.luxury_city_state_weights = {
		{self.gold_ID,     10},
		{self.silver_ID,   05},
		{self.copper_ID,   10},
		{self.gems_ID,     10},
		{self.salt_ID,     10},
		{self.jade_ID,     10},
		{self.amber_ID,    10},
		{self.lapis_ID,    10},
		{self.coffee_ID,   05},
		{self.tobacco_ID,  05},
		{self.tea_ID,      05},
		{self.perfume_ID,  05},
		{self.cotton_ID,   05},
		{self.wine_ID,     05},
		{self.olives_ID,   05},
		{self.incense_ID,  05},
		{self.truffles_ID, 05},
		{self.silk_ID,     05},
		{self.dye_ID,      05},
		{self.fur_ID,      05},
		{self.citrus_ID,   05},
		{self.cocoa_ID,    05},
		{self.spices_ID,   05},
		{self.sugar_ID,    05},
		{self.whale_ID,    10},
		{self.crab_ID,     10},
		{self.pearls_ID,   10},
		{self.coral_ID,    10},
	};

	if self:IsModActive("EvenMoreResources") then
		table.insert(self.luxury_city_state_weights, {self.obsidian_ID, 10});
		table.insert(self.luxury_city_state_weights, {self.platinum_ID, 10});
		table.insert(self.luxury_city_state_weights, {self.tin_ID,      10});
		table.insert(self.luxury_city_state_weights, {self.lavender_ID, 05});
		table.insert(self.luxury_city_state_weights, {self.poppy_ID,    05});
	end
	if self:IsModActive("AdditionalLuxuries") then
		table.insert(self.luxury_city_state_weights, {self.quartz_ID,   10});
	end
end
------------------------------------------------------------------------------
--- This function included to provide a quick and easy override for changing any initial settings.
--- Add your customized version to the map script.
function ASP:__CustomInit()
end

------------------------------------------------------------------------------
-- Start of functions tied to GenerateRegions()
------------------------------------------------------------------------------

--- Fertility of plots is used to divide landmasses into Regions.<br>
--- Regions are used to assign starting plots and place some resources.
---
--- Mountain, Snow, Oasis, Flood plains tiles = -1, -1, 4, 4 and do not count anything else.<br>
--- Rest of the tiles add up values of tile traits.<br>
--- Terrain: Hill 3, Grass 3, Plains 3, Tundra 1, Coast 1, Desert 1, Snow -1<br>
--- Features: Forest 1, FreshWater 1, Jungle 0, Marsh -1, Ice -2<br>
--- We want players who start in Grass/Plains to have the least room to expand.
---
--- If you modify the terrain values or add or remove any terrain elements, you will need to add or modify processes here to accomodate your changes.<br>
--- Please be aware that the default process includes numerous assumptions that your terrain changes may invalidate, so you may need to rebalance the system.
--- @param x integer The X-coordinate of the plot (0 = south)
--- @param y integer The Y-coordinate of the plot (0 = west)
--- @param checkForCoastalLand boolean When measuring only one landmass, this shortcut helps account for coastal plots not measured
--- @return integer iFertility
function ASP:MeasureStartPlacementFertilityOfPlot(x, y, checkForCoastalLand)
	local plot = Map.GetPlot(x, y);
	local iFertility = 0;
	local ePlot = plot:GetPlotType();
	local eTerrain = plot:GetTerrainType();
	local eFeature = plot:GetFeatureType();

	-- Measure Fertility -- Any cases absent from the process have a 0 value.
	if ePlot == PlotTypes.PLOT_MOUNTAIN then
		iFertility = -1;
	elseif eTerrain == TerrainTypes.TERRAIN_SNOW then
		iFertility = -1;
	elseif eFeature == FeatureTypes.FEATURE_OASIS then
		iFertility = 4;
	elseif eFeature == FeatureTypes.FEATURE_FLOOD_PLAINS then
		iFertility = 4;
	else
		if ePlot == PlotTypes.PLOT_HILLS then
			iFertility = 3;
		elseif eTerrain == TerrainTypes.TERRAIN_GRASS then
			iFertility = 3;
		elseif eTerrain == TerrainTypes.TERRAIN_PLAINS then
			iFertility = 3;
		elseif eTerrain == TerrainTypes.TERRAIN_TUNDRA then
			iFertility = 1;
		elseif eTerrain == TerrainTypes.TERRAIN_COAST then
			iFertility = 1;
		elseif eTerrain == TerrainTypes.TERRAIN_DESERT then
			iFertility = 1;
		end
		if eFeature == FeatureTypes.FEATURE_FOREST then
			iFertility = iFertility + 1;
		elseif eFeature == FeatureTypes.FEATURE_JUNGLE then
			iFertility = iFertility + 0;
		elseif eFeature == FeatureTypes.FEATURE_MARSH then
			iFertility = iFertility - 1;
		elseif eFeature == FeatureTypes.FEATURE_ICE then
			iFertility = iFertility - 2;
		end
		if plot:IsFreshWater() then
			iFertility = iFertility + 1;
		end
		if checkForCoastalLand and plot:IsCoastalLand() then
			iFertility = iFertility + 2;
		end
	end

	return iFertility;
end
------------------------------------------------------------------------------
--- This function is designed to provide initial data for regional division recursion.<br>
--- Loop through plots in this rectangle and measure Fertility Rating.<br>
--- Results will include a data table of all measured plots.
--- @param iWestX integer
--- @param iSouthY integer
--- @param iWidth integer
--- @param iHeight integer
--- @return integer[] landmassFertilityTable
--- @return integer landmassFertilityCount
--- @return integer plotCount
function ASP:MeasureStartPlacementFertilityInRectangle(iWestX, iSouthY, iWidth, iHeight)
	local landmassFertilityTable = {};
	local landmassFertilityCount = 0;
	local plotCount = iWidth * iHeight;
	for y = iSouthY, iSouthY + iHeight - 1 do -- When generating a plot data table incrementally, process Y first so that plots go row by row.
		for x = iWestX, iWestX + iWidth - 1 do
			local iFertility = self:MeasureStartPlacementFertilityOfPlot(x, y, false); -- Check for coastal land is disabled.
			table.insert(landmassFertilityTable, iFertility);
			landmassFertilityCount = landmassFertilityCount + iFertility;
		end
	end

	return landmassFertilityTable, landmassFertilityCount, plotCount;
end
------------------------------------------------------------------------------
--- This function is designed to provide initial data for regional division recursion.<br>
--- Loop through plots in this landmass and measure Fertility Rating.<br>
--- Results will include a data table of all plots within the rectangle that includes the entirety of this landmass.
---
--- This function will account for any wrapping around the world this landmass may do.
--- @param iLandmassID integer
--- @param iWestX integer
--- @param iEastX integer
--- @param iSouthY integer
--- @param iNorthY integer
--- @param wrapsX boolean
--- @param wrapsY boolean
--- @return integer[] landmassFertilityTable
--- @return integer landmassFertilityCount
--- @return integer plotCount
function ASP:MeasureStartPlacementFertilityOfLandmass(iLandmassID, iWestX, iEastX, iSouthY, iNorthY, wrapsX, wrapsY)
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
				local iFertility = self:MeasureStartPlacementFertilityOfPlot(x, y, true); -- Check for coastal land is enabled.
				table.insert(landmassFertilityTable, iFertility);
				landmassFertilityCount = landmassFertilityCount + iFertility;
			end
		end
	end

	-- Note: The table accounts for world wrap, so make sure to translate its index correctly.
	-- Plots in the table run from the southwest corner along the bottom row, then upward row by row, per normal plot data indexing.
	return landmassFertilityTable, landmassFertilityCount, plotCount;
end
------------------------------------------------------------------------------
--- This function is designed to provide initial data for continental regional division only.<br>
--- Biggest landmass method only needs to measure landmass fertility, and doesn't need to take areas into account.
---
--- Loop through plots in this area and measure Fertility Rating.<br>
--- Results will include a data table of all plots within the rectangle that includes the entirety of this area.
---
--- This function will account for any wrapping around the world this area may do.
--- @param iAreaID integer
--- @param iWestX integer
--- @param iEastX integer
--- @param iSouthY integer
--- @param iNorthY integer
--- @param wrapsX boolean
--- @param wrapsY boolean
--- @return integer[] areaFertilityTable
--- @return integer areaFertilityCount
--- @return integer plotCount
function ASP:MeasureStartPlacementFertilityOfArea(iAreaID, iWestX, iEastX, iSouthY, iNorthY, wrapsX, wrapsY)
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
				local iFertility = self:MeasureStartPlacementFertilityOfPlot(x, y, true); -- Check for coastal land is enabled.
				table.insert(areaFertilityTable, iFertility);
				areaFertilityCount = areaFertilityCount + iFertility;
			end
		end
	end

	-- Note: The table accounts for world wrap, so make sure to translate its index correctly.
	-- Plots in the table run from the southwest corner along the bottom row, then upward row by row, per normal plot data indexing.
	return areaFertilityTable, areaFertilityCount, plotCount;
end
------------------------------------------------------------------------------
--- Any outside rows in the fertility table of a just-divided region that contains all zeroes can be safely removed.<br>
--- This will improve the accuracy of operations involving any applicable region.
--- @param fertility_table integer[]
--- @param iWestX integer
--- @param iSouthY integer
--- @param iWidth integer
--- @param iHeight integer
--- @return integer[] adjusted_fertility_table
--- @return integer adjusted_westX
--- @return integer adjusted_southY
--- @return integer adjusted_width
--- @return integer adjusted_height
function ASP:RemoveDeadRows(fertility_table, iWestX, iSouthY, iWidth, iHeight)
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
function ASP:DivideIntoRegions(iNumDivisions, fertility_table, rectangle_data_table)
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
--- Performs the mechanics of dividing a region into three roughly equal fertility subregions.
function ASP:ChopIntoThreeRegions(fertility_table, rectangle_data_table, bTaller)
	-- print("-"); print("ChopIntoThree called.");
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
--- Performs the mechanics of dividing a region into two subregions.
---
--- Fertility table is a plot data array including data for all plots to be processed here.<br>
--- This data already factors any need for processing LandmassID/AreaID.
---
--- Rectangle table includes seven data fields:<br>
--- westX, southY, width, height, LandmassID/AreaID, fertilityCount, plotCount<br>
function ASP:ChopIntoTwoRegions(fertility_table, rectangle_data_table, bTaller, chopPercent)
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
				local iFertility = fertility_table[fertIndex];
				-- Add this plot's fertility to the region total so far.
				iFirstRegionFertility = iFirstRegionFertility + iFertility;
				-- Record this plot in a new fertility table. (Needed for further subdivisions).
				-- Note, building this plot data table incrementally, so it must go row by row.
				table.insert(region_one_fertility, iFertility);
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
				local iFertility = fertility_table[fertIndex];
				-- Add this plot's fertility to the region total so far.
				iSecondRegionFertility = iSecondRegionFertility + iFertility;
				-- Record this plot in a new fertility table. (Needed for further subdivisions).
				-- Note, building this plot data table incrementally, so it must go row by row.
				table.insert(region_two_fertility, iFertility);
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
				local iFertility = fertility_table[fertIndex];
				-- Add this plot's fertility to the region total so far.
				iFirstRegionFertility = iFirstRegionFertility + iFertility;
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
				local iFertility = fertility_table[fertIndex];
				-- Add this plot's fertility to the region total so far.
				iSecondRegionFertility = iSecondRegionFertility + iFertility;
				-- Record this plot in a new fertility table. (Needed for further subdivisions).
				-- Note, building this plot data table incrementally, so it must go row by row.
				table.insert(region_two_fertility, iFertility);
			end
		end
		-- Now create the fertility table for the first region.
		for loopY = 0, iRectHeight - 1 do
			for loopX = 0, firstRegionWidth - 1 do
				local fertIndex = loopY * iRectWidth + loopX + 1;
				local iFertility = fertility_table[fertIndex];
				table.insert(region_one_fertility, iFertility);
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
-- This function allows an easy entry point for overrides that need to
-- take place after regional division, but before anything else.
function ASP:CustomOverride()
end
------------------------------------------------------------------------------
--- This function stores its data in the instance (self) data table.
---
--- The "Four Methods" of regional division:
--- 1. Biggest Landmass: All civs start on the biggest landmass.
--- 2. Continental (default): Civs are assigned to continents. Any continents with more than one civ are divided.
--- 3. Rectangular: Civs start within a given rectangle that spans the whole map, without regard to landmass sizes.<br>
---                 This method is primarily applied to Archipelago and other maps with lots of tiny islands.
--- 4. Custom Rectangular: Civs start within a given rectangle defined by arguments passed in on the function call.<br>
---                        Arguments required for this method: iWestX, iSouthY, iWidth, iHeight
---
--- Default methods for civ and city state placement both rely on having regional division data.<br>
--- If the desired process for a given map script would not define regions of this type, replace this with your custom method.
function ASP:GenerateRegions(args)
	print("Map Generation - Dividing the map in to Regions");

	args = args or {};
	local iW, iH = Map.GetGridSize();
	self.method = args.method or self.method; -- Continental method is default.
	self.resource_setting = args.resources or 2; -- UNUSED, use the below resource settings instead
	args.resources = args.resources or 2;
	args.retryCount1 = args.retryCount1 or 0;
	args.retryCount2 = args.retryCount2 or 0;

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
	local iNumFrontiers = 0;
	if args.addEmptyRegions and self.iNumCivs <= 22 then
		local emptyRegionMinCount = {1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 3, 2, 1, 0};
		local emptyRegionMaxCount = {1, 1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0};
		iNumFrontiers = GetRandomFromRangeInclusive(emptyRegionMinCount[self.iNumCivs], emptyRegionMaxCount[self.iNumCivs]);
		print("Number of empty regions:", iNumFrontiers); print("-");
	end

	if self.method == RegionDivision.BIGGEST_LANDMASS then
		-- Identify the biggest landmass.
		local iLandmassID = Map.FindBiggestLandmassID(false);
		-- We'll need all eight data fields returned in the results table from the boundary finder:
		local landmass_data = GetLandmassBoundaries(iLandmassID);
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
		self:DivideIntoRegions(self.iNumCivs + iNumFrontiers, fert_table, rect_table);
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
		self:DivideIntoRegions(self.iNumCivs + iNumFrontiers, fert_table, rect_table);
		-- The regions have been defined.

	else -- Continental.
		-- Loop through all plots on the map, measuring fertility of each land plot, identifying its LandmassID and AreaID,
		-- building a list of LandmassIDs and AreaIDs, and tallying the Start Placement Fertility for each landmass/area.

		self.bArea = true;

		-- Obtain info on all landmasses and areas for comparison purposes.
		local iGlobalFertilityOfLands = 0;
		local iNumAreas = 0;
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
					local iFertility = self:MeasureStartPlacementFertilityOfPlot(x, y, true); -- Check for coastal land is enabled.
					iGlobalFertilityOfLands = iGlobalFertilityOfLands + iFertility;

					if not landmass_fert[iLandmass] then -- This plot is the first detected in its landmass.
						landmass_fert[iLandmass] = iFertility;
					else -- This landmass already known.
						landmass_fert[iLandmass] = landmass_fert[iLandmass] + iFertility;
					end

					if not area_fert[iArea] then -- This plot is the first detected in its area.
						iNumAreas = iNumAreas + 1;
						table.insert(area_IDs, iArea);
						area_fert[iArea] = iFertility;
						-- print("Area#", iArea, "is a new area in landmass#", iLandmass);
					else -- This area already known.
						area_fert[iArea] = area_fert[iArea] + iFertility;
					end
				end
			end
		end

		-- Sort areas, achieving a list of areas with best areas first.
		-- Add fertility check at 25% of fertility of the largest landmass to prevent tiny islands from being considered, which will result in isolated starts.
		-- We do these by making a new table storing area fertilities that reach the threshold.
		local interim_table = {};
		local biggestLandmass = Map.FindBiggestLandmassID(false);
		local min_landmass_fertility = landmass_fert[biggestLandmass] * (0.25 - 0.025 * args.retryCount1);
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

		-- If only one relevant landmass, we lower the fertility requirement for landmasses down to 12.5%,
		-- then switch tracks to use biggest landmass division as the last resort.
		if iNumLandmass <= 1 then
			if args.retryCount1 < 5 then
				print("Only one landmass fertile enough for start placement. Retry with lower fertility requirement.");
				args.retryCount1 = args.retryCount1 + 1;
			else
				print("Only one landmass fertile enough for start placement. Switching to use Biggest Landmass division.");
				args.method = RegionDivision.BIGGEST_LANDMASS;
				self.bArea = false;
			end
			self:GenerateRegions(args);
			return;
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
			local iNumRelevantAreas = math.min(iNumAreas, self.iNumCivs + iNumFrontiers);
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
					for _, areaID in pairs(area_IDs) do
						if interim_table[loop] == area_fert[areaID] and area_landmass[areaID] then
							table.insert(best_areas, areaID);
							break;
						end
					end
				end
			else -- Ties exist! Special handling required to protect against a shortfall in the number of defined regions.
				local iNumUniqueFertValues = table.maxn(fertility_value_list);
				for fertLoop = 1, iNumUniqueFertValues do
					for areaID, fert in pairs(area_fert) do
						if fert == fertility_value_list[fertLoop] and area_landmass[areaID] then
							table.insert(best_areas, areaID);
							-- Add ties only if there is room!
							if table.maxn(best_areas) >= iNumRelevantAreas then
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
			local fertilityPerCiv = relevantFertility * (0.7 - 0.05 * args.retryCount2) / (self.iNumCivs + iNumFrontiers); -- Allow some unused fertility here
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
			-- We retry with a lower expected fertility per civ, then switch tracks to use rectangular method as the last resort.
			if iTotalFertileAreas < self.iNumCivs + iNumFrontiers then
				if args.retryCount2 < 4 then
					print("Landmasses aren't fertile enough for start placement. Retry with lower expected fertility per civ.");
					args.retryCount2 = args.retryCount2 + 1;
				else
					print("Landmasses aren't fertile enough for start placement. Switching to use Rectangular division.");
					args.method = RegionDivision.RECTANGULAR;
					self.bArea = false;
				end
				self:GenerateRegions(args);
				return;
			else
				-- If enough fertile areas in fertile landmasses, disallow landmasses with only one civ
				if iTotalFertileLandmassAreas >= self.iNumCivs + iNumFrontiers then
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
				for civToAssign = 1, self.iNumCivs + iNumFrontiers do
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
								if civToAssign ~= self.iNumCivs + iNumFrontiers or TestMembership(inhabitedLandmassIDs, area_landmass[areaID]) then
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
					if not actualNumberOfCivsPerArea[bestRemainingArea] then
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
					local area_data = GetAreaBoundaries(currentAreaID);
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

	-- Entry point for easier overrides.
	self:CustomOverride();

	---[[ Printout is for debugging only. Deactivate otherwise.
	print("Region generation completed. retryCount1 = ", args.retryCount1, ", retryCount2 = ", args.retryCount2);
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

------------------------------------------------------------------------------
-- Start of functions tied to ChooseLocations()
------------------------------------------------------------------------------

function ASP:MeasureTerrainInRegions()
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
				local ePlot = plot:GetPlotType();
				local eTerrain = plot:GetTerrainType();
				local eFeature = plot:GetFeatureType();

				-- Mountain and Ocean plot types get their own AreaIDs, but we are going to measure them anyway.
				if ePlot == PlotTypes.PLOT_MOUNTAIN then
					peaksCount = peaksCount + 1; -- and that's it for Mountain plots. No other stats.
				elseif ePlot == PlotTypes.PLOT_OCEAN then
					waterCount = waterCount + 1;
					if eTerrain == TerrainTypes.TERRAIN_COAST then
						if plot:IsLake() then
							lakeCount = lakeCount + 1;
						else
							coastCount = coastCount + 1;
						end
					else
						oceanCount = oceanCount + 1;
					end
					if eFeature == FeatureTypes.FEATURE_ICE then
						iceCount = iceCount + 1;
					end

				else
					-- Hills and Flatlands, check plot for region membership. Only process this plot if it is a member.
					if iAreaID == -1 or (self.bArea and area_of_plot == iAreaID) or (not self.bArea and landmass_of_plot == iAreaID) then
						areaPlots = areaPlots + 1;

						-- set up coastalLand and nextToCoast index
						local i = iW * y + x + 1;

						-- Record plot data
						if ePlot == PlotTypes.PLOT_HILLS and eTerrain ~= TerrainTypes.TERRAIN_SNOW then
							hillsCount = hillsCount + 1;

							if self.plotDataIsCoastal[i] then
								coastalLandCount = coastalLandCount + 1;
							elseif self.plotDataIsNextToCoast[i] then
								nextToCoastCount = nextToCoastCount + 1;
							end

							if plot:IsRiver() then
								riverCount = riverCount + 1;
							end

							-- Feature check checking for all types, in case features are not obeying standard allowances.
							if eFeature == FeatureTypes.FEATURE_FOREST then
								forestCount = forestCount + 1;
							elseif eFeature == FeatureTypes.FEATURE_JUNGLE then
								jungleCount = jungleCount + 1;
							elseif eFeature == FeatureTypes.FEATURE_MARSH then
								marshCount = marshCount + 1;
							elseif eFeature == FeatureTypes.FEATURE_FLOOD_PLAINS then
								floodplainCount = floodplainCount + 1;
							elseif eFeature == FeatureTypes.FEATURE_OASIS then
								oasisCount = oasisCount + 1;
							end

						else -- Flatlands plot OR snow
							flatlandsCount = flatlandsCount + 1;

							if self.plotDataIsCoastal[i] then
								coastalLandCount = coastalLandCount + 1;
							elseif self.plotDataIsNextToCoast[i] then
								nextToCoastCount = nextToCoastCount + 1;
							end

							if plot:IsRiver() then
								riverCount = riverCount + 1;
							end

							if eTerrain == TerrainTypes.TERRAIN_GRASS then
								grassCount = grassCount + 1;
							elseif eTerrain == TerrainTypes.TERRAIN_PLAINS then
								plainsCount = plainsCount + 1;
							elseif eTerrain == TerrainTypes.TERRAIN_DESERT then
								desertCount = desertCount + 1;
							elseif eTerrain == TerrainTypes.TERRAIN_TUNDRA then
								tundraCount = tundraCount + 1;
							elseif eTerrain == TerrainTypes.TERRAIN_SNOW then
								snowCount = snowCount + 1;
							end

							-- Feature check checking for all types, in case features are not obeying standard allowances.
							if eFeature == FeatureTypes.FEATURE_FOREST then
								forestCount = forestCount + 1;
							elseif eFeature == FeatureTypes.FEATURE_JUNGLE then
								jungleCount = jungleCount + 1;
							elseif eFeature == FeatureTypes.FEATURE_MARSH then
								marshCount = marshCount + 1;
							elseif eFeature == FeatureTypes.FEATURE_FLOOD_PLAINS then
								floodplainCount = floodplainCount + 1;
							elseif eFeature == FeatureTypes.FEATURE_OASIS then
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
--- Determine region type and conditions. Use self.regionTypes to store the results
function ASP:DetermineRegionTypes()
	for iRegionId, terrainCounts in ipairs(self.regionTerrainCounts) do
		local tRegionTerrainCount = {};
		local iNumRegionPlots = terrainCounts[2];
		tRegionTerrainCount[GameInfoTypes.REGION_HILLS] = terrainCounts[5];
		tRegionTerrainCount[GameInfoTypes.REGION_GRASS] = terrainCounts[11];
		tRegionTerrainCount[GameInfoTypes.REGION_PLAINS] = terrainCounts[12];
		tRegionTerrainCount[GameInfoTypes.REGION_DESERT] = terrainCounts[13];
		tRegionTerrainCount[GameInfoTypes.REGION_TUNDRA] = terrainCounts[14];
		tRegionTerrainCount[GameInfoTypes.REGION_FOREST] = terrainCounts[16];
		tRegionTerrainCount[GameInfoTypes.REGION_JUNGLE] = terrainCounts[17];

		-- If Rectangular regional division, then water plots would be included in region plots.
		-- Let's recalculate region plots based only on flatland and hills plots.
		if self.method == RegionDivision.RECTANGULAR or self.method == RegionDivision.RECTANGULAR_SELF_DEFINED then
			iNumRegionPlots = terrainCounts[4] + terrainCounts[5];
		end

		-- Forest region must not have too much tundra
		local bCanBeForest = (tRegionTerrainCount[GameInfoTypes.REGION_FOREST] > 1.7 * tRegionTerrainCount[GameInfoTypes.REGION_TUNDRA]);

		-- Find the max ratio of tiles of specific terrain/feature in this region above the required ratio.
		-- If none exists, this region is plains, grassland, or hybrid.
		local eSelectedRegion;
		local fMaxTerrainRatioAboveReq = 0;
		for eRegion = GameInfoTypes.REGION_TUNDRA, GameInfoTypes.REGION_HILLS do
			local fTerrainRatioAboveReq = tRegionTerrainCount[eRegion] / iNumRegionPlots - self.REGION_TYPE_MIN_REQ[eRegion];
			print("fTerrainRatioAboveReq of Region#", iRegionId, "for region type", GameInfo.Regions[eRegion].Type, "is", fTerrainRatioAboveReq);
			if fTerrainRatioAboveReq > fMaxTerrainRatioAboveReq and (eRegion ~= GameInfoTypes.REGION_FOREST or bCanBeForest) then
				eSelectedRegion = eRegion;
				fMaxTerrainRatioAboveReq = fTerrainRatioAboveReq;
			end
		end

		-- Check for plains or grassland regions. Grassland regions cannot have too much plains, and vice versa.
		if not eSelectedRegion then
			local fGrassPlainsRatio = tRegionTerrainCount[GameInfoTypes.REGION_GRASS] / tRegionTerrainCount[GameInfoTypes.REGION_PLAINS];
			if tRegionTerrainCount[GameInfoTypes.REGION_PLAINS] >= iNumRegionPlots * self.REGION_TYPE_MIN_REQ[GameInfoTypes.REGION_PLAINS] and
			fGrassPlainsRatio <= 0.8 then
				eSelectedRegion = GameInfoTypes.REGION_PLAINS;
			elseif tRegionTerrainCount[GameInfoTypes.REGION_GRASS] >= iNumRegionPlots * self.REGION_TYPE_MIN_REQ[GameInfoTypes.REGION_GRASS] and
			fGrassPlainsRatio >= 1.25 then
				eSelectedRegion = GameInfoTypes.REGION_GRASS;
			else
				eSelectedRegion = GameInfoTypes.REGION_HYBRID;
			end
		end

		self.regionTypes[iRegionId] = eSelectedRegion;

		---[[ Debug printout
		print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
		for eRegion, kRegionInfo in GameInfoCache("Regions") do
			if eRegion == eSelectedRegion then
				print(string.format("Region #%d has been defined as %s.", iRegionId, kRegionInfo.Type));
				break;
			end
		end
		--]]
	end

	-- Re-classify region(s) as "Mountain region" or "Snow region" depending on number of civs with the respective start biases.
	-- It's possible that there's no snow/mountain on the map, so we need to handle it later in BalanceAndAssign()
	for _ = 1, self.iNumSnowBias do
		-- Get ID of region with the most snow, prioritizing tundra regions first
		local iSnowRegionId = -1;
		local iMaxNumSnow = 0;
		for iRegionId, terrainCounts in ipairs(self.regionTerrainCounts) do
			local eRegion = self.regionTypes[iRegionId];
			if eRegion ~= GameInfoTypes.REGION_SNOW then
				local iNumSnow = terrainCounts[15];
				if eRegion == GameInfoTypes.REGION_TUNDRA then
					iNumSnow = iNumSnow + 10000;
				end
				if iNumSnow > iMaxNumSnow then
					iSnowRegionId = iRegionId;
					iMaxNumSnow = iNumSnow;
				end
			end
		end

		if iSnowRegionId == -1 then
			print("No suitable Region to reclassify as SNOW Region");
			break;
		end
		self.regionTypes[iSnowRegionId] = GameInfoTypes.REGION_SNOW;
		print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
		print("Region #", iSnowRegionId, " has been reclassified as SNOW Region.");
	end

	for _ = 1, self.iNumMountainBias do
		-- Get ID of region with the most mountains
		local iMountainRegionId = -1;
		local iMaxNumMountain = 0;
		for iRegionId, terrainCounts in ipairs(self.regionTerrainCounts) do
			local eRegion = self.regionTypes[iRegionId];
			local iNumMountain = terrainCounts[6];
			if iNumMountain > iMaxNumMountain and eRegion ~= GameInfoTypes.REGION_MOUNTAIN and eRegion ~= GameInfoTypes.REGION_SNOW then
				iMountainRegionId = iRegionId;
				iMaxNumMountain = iNumMountain;
			end
		end

		if iMountainRegionId == -1 then
			print("No suitable Region to reclassify as MOUNTAIN Region");
			break;
		end
		self.regionTypes[iMountainRegionId] = GameInfoTypes.REGION_MOUNTAIN;
		print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
		print("Region #", iMountainRegionId, " has been reclassified as MOUNTAIN Region.");
	end
end
------------------------------------------------------------------------------
--- This function operates upon the "impact and ripple" data overlays. This is the core version, which operates on start points.
--- Resources and city states have their own data layers, using this same design principle.
--- Execution of this function handles a single start point (x, y).
---
--- The purpose of the overlay is to strongly discourage placement of new start points near already-placed start points.
--- Each start placed makes an "impact" on the map, and this impact "ripples" outward in rings, each ring weaker in bias than the previous ring.
---
--- Civ4 attempted to adjust the minimum distance between civs according to a formula that factored map size and number of civs in the game,
--- but the formula was chock full of faulty assumptions, resulting in an accurate calibration rate of less than ten percent.
--- The failure of this approach is the primary reason that an all-new positioner was written for Civ5.
---
--- Rather than repeating the mistakes of the old system, in part or in whole,
--- I have opted to go with a flat 9-tile impact crater for all map sizes and number of civs.
--- The new system will place civs at least 9 tiles away from other civs whenever and wherever a reasonable candidate plot exists at this range.
--- If a start must be found within that range, it will attempt to balance quality of the location against proximity to another civ,
--- with the bias becoming very heavy inside 7 plots, and all but prohibitive inside 5.
--- The only starts that should see any Civs crowding together are those with impossible conditions,
--- such as cramming more than a dozen civs on to Tiny or Duel sized maps.
---
--- The Impact and Ripple is aimed mostly at assisting with Rectangular Method regional division on islands maps,
--- as the primary method of spacing civs is the Center Bias factor.
--- The Impact and Ripple is a second layer of protection, for those rare cases when regional shapes are severely distorted,
--- with little to no land in the region center, and the start having to be placed near the edge, and for cases of extremely thin regional dimension.
---
--- To establish a bias of 9, we Impact the overlay and Ripple outward 8 times.<br>
--- Value of 0 in a plot means no influence from existing Impacts in that plot.<br>
--- Value of 99 means an Impact occurred in that plot and it IS a start point.<br>
--- Values > 0 and < 99 are "ripples", meaning that plot is near a start point.
--- @param x integer
--- @param y integer
function ASP:PlaceImpactAndRipples(x, y)
	local impact_value = 99;
	local ripple_values = {97, 95, 92, 89, 69, 57, 24, 15};

	-- Start points need to impact the resource layers, so let's handle that first.

	-- Strategic layers, at impact site only.
	self:PlaceStrategicResourceImpact(x, y, 0);

	-- Luxury and Bonus layers, set all plots within this civ start as off limits. Will be individually handled later.
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_LUXURY, 3);
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_BONUS, 3);
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_FISH, 3);

	-- Natural Wonders cannot appear within 4 plots of a civ's starting point
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_NATURAL_WONDER, 4);

	-- City-States cannot appear within 6 plots of a civ's starting point
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_CITY_STATE, 6);

	-- Now the main data layer, for start points themselves, and the City State data layer.
	-- Place Impact
	local impactPlotIndex = GetLuaPlotIndexFromCoordinates(x, y);
	self.impactData[ImpactLayers.LAYER_MAJOR][impactPlotIndex] = impact_value;
	self.playerCollisionData[impactPlotIndex] = true;

	-- Place Ripples
	for iLoopX, iLoopY, iDistance in Plot_GetPlotCoordinatesInCircle(Map.GetPlot(x, y), #ripple_values) do
		-- Record ripple data for this plot.
		local iPlotIndex = GetLuaPlotIndexFromCoordinates(iLoopX, iLoopY);
		if self.impactData[ImpactLayers.LAYER_MAJOR][iPlotIndex] > 0 then -- This plot is already in range of at least one other civ!
			-- First choose the greater of the two, existing value or current ripple.
			local iStrongerValue = math.max(self.impactData[ImpactLayers.LAYER_MAJOR][iPlotIndex], ripple_values[iDistance]);
			-- Now increase it by 1.2x to reflect that multiple civs are in range of this plot.
			self.impactData[ImpactLayers.LAYER_MAJOR][iPlotIndex] = math.min(97, math.floor(iStrongerValue * 1.2));
		else
			self.impactData[ImpactLayers.LAYER_MAJOR][iPlotIndex] = ripple_values[iDistance];
		end
	end
end
------------------------------------------------------------------------------
--- Evaluate a plot around a candidate start location.<br>
--- Note that "Food" is not strictly about tile yield.
--- Different regions get their food in different ways.
--- Non-grassland/jungle regions may get bonus resource support to cover food shortages.
--- Grassland regions may get bonus resource support to cover production shortages.
--- @param x integer
--- @param y integer
--- @param region_type RegionType
--- @return integer Food # 1 if this plot can provide food to the start location
--- @return integer Prod # 1 if this plot can provide production to the start location
--- @return integer Good # 1 if this plot is good for the start location. Can be higher for certain specific regions.
--- @return integer Junk # 1 if this plot is highly discouraged to be near the start location
function ASP:MeasureSinglePlot(x, y, region_type)
	local plot = Map.GetPlot(x, y);
	local ePlot = plot:GetPlotType();
	local eTerrain = plot:GetTerrainType();
	local eFeature = plot:GetFeatureType();

	if ePlot == PlotTypes.PLOT_MOUNTAIN then -- Mountains are Junk, except in Mountain regions (made for mountain bias)
		if region_type ~= GameInfoTypes.REGION_MOUNTAIN then
			return 0, 0, 0, 1;
		end
		return 0, 1, 3, 0;
	end

	if ePlot == PlotTypes.PLOT_OCEAN then
		if eFeature == FeatureTypes.FEATURE_ICE then -- Icebergs are Junk, except in snow bias where they're ignored.
			if region_type ~= GameInfoTypes.REGION_SNOW then
				return 0, 0, 0, 1;
			end
			return 0, 0, 0, 0;
		elseif plot:IsLake() then -- Lakes are Food, Good.
			return 1, 0, 1, 0;
		elseif self.method == RegionDivision.RECTANGULAR or self.method == RegionDivision.RECTANGULAR_SELF_DEFINED then
			if eTerrain == TerrainTypes.TERRAIN_COAST then -- Shallow water is Good for Archipelago-type maps.
				return 0, 0, 1, 0;
			end
		elseif eTerrain == TerrainTypes.TERRAIN_COAST and region_type == GameInfoTypes.REGION_SNOW then -- Give more coastal score for snow bias
			return 1, 0, 1, 0;
		end
		-- Other water plots are ignored.
		return 0, 0, 0, 0;
	end

	if eFeature == FeatureTypes.FEATURE_JUNGLE then -- Jungles are Food, Good, except in Grass regions.
		if region_type ~= GameInfoTypes.REGION_GRASS then
			return 1, 0, 1, 0;
		elseif ePlot == PlotTypes.PLOT_HILLS then -- Jungle hill, in grass region, count as Prod but not Good.
			return 0, 1, 0, 0;
		end
	elseif eFeature == FeatureTypes.FEATURE_FOREST then -- Forests are Prod, Good. In Forest, Tundra, Snow regions they're also Food.
		if region_type == GameInfoTypes.REGION_FOREST or
		region_type == GameInfoTypes.REGION_TUNDRA or
		region_type == GameInfoTypes.REGION_SNOW then
			return 1, 1, 1, 0;
		end
		return 0, 1, 1, 0;
	elseif eFeature == FeatureTypes.FEATURE_OASIS or eFeature == FeatureTypes.FEATURE_FLOOD_PLAINS then -- Oases and Flood Plains are Food, Good.
		return 1, 0, 1, 0;
	elseif eFeature == FeatureTypes.FEATURE_MARSH then -- Marsh are ignored.
		return 0, 0, 0, 0;
	end

	if ePlot == PlotTypes.PLOT_HILLS and eTerrain ~= TerrainTypes.TERRAIN_SNOW then -- Non-Snow Hills with no features are Prod, Good.
		return 0, 1, 1, 0;
	end

	-- If we have reached this point in the process, the plot is flatlands or snow.
	if eTerrain == TerrainTypes.TERRAIN_SNOW then -- Snow are Junk, except in Snow regions (made for snow bias)
		if region_type ~= GameInfoTypes.REGION_SNOW then
			return 0, 0, 0, 1;
		end
		return 1, 1, 3, 0;
	elseif eTerrain == TerrainTypes.TERRAIN_DESERT then -- Featureless flat deserts are Junk, except in Desert regions where they're ignored.
		if region_type ~= GameInfoTypes.REGION_DESERT then
			return 0, 0, 0, 1;
		end
		return 0, 0, 0, 0;
	elseif eTerrain == TerrainTypes.TERRAIN_TUNDRA then -- Tundra are ignored, except in Tundra/Snow regions where they are Food, Good.
		if region_type == GameInfoTypes.REGION_TUNDRA then
			return 1, 0, 1, 0;
		elseif region_type == GameInfoTypes.REGION_SNOW then
			return 1, 1, 2, 0;
		end
		return 0, 0, 0, 0;
	elseif eTerrain == TerrainTypes.TERRAIN_PLAINS then -- Plains are Good for all region types, but Food in only about half of them.
		if region_type == GameInfoTypes.REGION_TUNDRA or
		region_type == GameInfoTypes.REGION_DESERT or
		region_type == GameInfoTypes.REGION_HILLS or
		region_type == GameInfoTypes.REGION_PLAINS or
		region_type == GameInfoTypes.REGION_HYBRID or
		region_type == GameInfoTypes.REGION_MOUNTAIN then
			return 1, 0, 1, 0;
		elseif region_type == GameInfoTypes.REGION_SNOW then
			return 0, 0, 0, 0;
		end
		return 0, 0, 1, 0;
	elseif eTerrain == TerrainTypes.TERRAIN_GRASS then -- Grass is Good for all region types, but Food in only about half of them.
		if region_type == GameInfoTypes.REGION_DESERT or
		region_type == GameInfoTypes.REGION_TUNDRA then
			return 0, 0, 1, 0;
		elseif region_type == GameInfoTypes.REGION_SNOW then
			return 1, 0, 0, 0;
		end
		return 1, 0, 1, 0;
	end

	-- If we have arrived here, the plot has non-standard terrain.
	print("Encountered non-standard terrain.", x, y);
	return 0, 0, 0, 0;
end
------------------------------------------------------------------------------
--- @param iPlotID PlotId
--- @param region_type RegionType
--- @return integer # Score of this plot
--- @return boolean # Whether this plot passed the evaluation. Failed plots can still be in the fallback list.
function ASP:EvaluateCandidatePlot(iPlotID, region_type)
	local bFail = false;
	local foodTotal, prodTotal, goodTotal, junkTotal, riverTotal = 0, 0, 0, 0, 0;
	local pPlot = Map.GetPlotByIndex(iPlotID - 1);

	local iTotalScore = 0;

	-- Add score for coastal plots.
	if self.plotDataIsCoastal[iPlotID] then
		iTotalScore = iTotalScore + 80;
	-- Add less score for river plots.
	elseif pPlot:IsRiver() or pPlot:IsFreshWater() then
		iTotalScore = iTotalScore + 40;
	end

	-- Set up food/prod/good thresholds from arguments passed to ChooseLocations()
	local tMinFood = {self.minFoodInner, self.minFoodMiddle, self.minFoodOuter};
	local tMinProd = {self.minProdInner, self.minProdMiddle, self.minProdOuter};
	local tMinGood = {self.minGoodInner, self.minGoodMiddle, self.minGoodOuter};

	-- Evaluate each tile in the 3 rings around the candidate plot
	for i = 1, 3 do
		local iValidCount = 0;
		for iLoopX, iLoopY in Plot_GetPlotCoordinatesInCircle(pPlot, i, i) do
			iValidCount = iValidCount + 1;
			local food, prod, good, junk = self:MeasureSinglePlot(iLoopX, iLoopY, region_type);
			foodTotal = foodTotal + food;
			prodTotal = prodTotal + prod;
			goodTotal = goodTotal + good;
			junkTotal = junkTotal + junk;
			if Map.GetPlot(iLoopX, iLoopY):IsRiver() then
				riverTotal = riverTotal + 1;
			end
		end

		-- All out of bounds plots count as junk.
		junkTotal = junkTotal + i * 6 - iValidCount;

		-- Now check the results against the established targets.
		if foodTotal < tMinFood[i] or prodTotal < tMinProd[i] or goodTotal < tMinGood[i] then
			bFail = true;
		end

		-- If there's a lack of food tiles, reduce the effective prod.
		prodTotal = math.min(prodTotal, foodTotal * 2);

		-- Update the score for the candidate plot.
		local tFoodScore = self.WEIGHTED_FOOD_SCORE[i];
		local iFoodScore;
		if foodTotal + 1 > #tFoodScore then
			iFoodScore = tFoodScore[#tFoodScore] + foodTotal + 1 - #tFoodScore;
		else
			iFoodScore = tFoodScore[foodTotal + 1];
		end
		local tProdScore = self.WEIGHTED_PROD_SCORE[i];
		local iProdScore;
		if prodTotal + 1 > #tProdScore then
			iProdScore = tProdScore[#tProdScore] + prodTotal + 1 - #tProdScore;
		else
			iProdScore = tProdScore[prodTotal + 1];
		end
		iTotalScore = iTotalScore + iFoodScore + iProdScore + goodTotal * self.GOOD_TILE_MULTIPLIER[i] + junkTotal * self.JUNK_TILE_MULTIPLIER[i] + riverTotal;
	end

	-- Check Impact and Ripple data to see if candidate is near an already-placed start point.
	local iDistanceBias = self.impactData[ImpactLayers.LAYER_MAJOR][iPlotID];
	if iDistanceBias > 0 then
		-- This candidate is near an already placed start. This invalidates its eligibility for first-pass placement;
		-- but it may still qualify as a fallback site, so we will reduce its score according to the bias factor.
		bFail = true;
		iTotalScore = iTotalScore - math.floor(iTotalScore * iDistanceBias / 100);
	end

	--[[ Debug
	local x, y = GetCoordinatesFromLuaPlotIndex(iPlotID);
	print("-");
	print("Plot:", x, y, "Food:", foodTotal, "Prod:", prodTotal, "Good:", goodTotal, "Junk:", junkTotal, "River:", riverTotal, "Score:", iTotalScore);
	print("Coastal:", self.plotDataIsCoastal[iPlotID], "IsRiver:", pPlot:IsRiver(), "Distance Bias:", iDistanceBias);
	--]]

	return iTotalScore, not bFail;
end
------------------------------------------------------------------------------
--- Iterates through a list of candidate plots.
--- Each plot is identified by its global plot index.
--- This function assumes all candidate plots can have a city built on them.
--- Any plots not allowed to have a city should be weeded out when building the candidate list.
--- @param plot_list PlotId[]
--- @param region_type RegionType
function ASP:IterateThroughCandidatePlotList(plot_list, region_type)
	local found_eligible = false;
	local bestPlotScore = -5000;
	local bestPlotIndex;
	local found_fallback = false;
	local bestFallbackScore = -5000;
	local bestFallbackIndex;
	-- Process list of candidate plots.
	for _, iPlotID in ipairs(plot_list) do
		local score, meets_minimums = self:EvaluateCandidatePlot(iPlotID, region_type);
		-- Test current plot against best known plot.
		if meets_minimums then
			found_eligible = true;
			if score > bestPlotScore then
				bestPlotScore = score;
				bestPlotIndex = iPlotID;
			end
		else
			found_fallback = true;
			if score > bestFallbackScore then
				bestFallbackScore = score;
				bestFallbackIndex = iPlotID;
			end
		end
	end
	-- returns table containing six variables: boolean, integer, integer, boolean, integer, integer
	local election_results = {found_eligible, bestPlotScore, bestPlotIndex, found_fallback, bestFallbackScore, bestFallbackIndex};
	return election_results;
end
------------------------------------------------------------------------------
--- Attempt to choose a start position for a single region.
--- The plots closer to the center of the region are prioritized.
--- @param iRegionID RegionId
--- @param bForcedCoastal boolean
--- @return boolean bForcedPlacementFlag # Whether the placement is forced on the map with terraform
function ASP:FindStart(iRegionID, bForcedCoastal)
	-- Check region for forced coastal religibility.
	if bForcedCoastal and self.regionTerrainCounts[iRegionID][22] < 3 then
		print("Region", iRegionID, "cannot support a coastal start. Try instead to find an inland start for it.");
		return self:FindStart(iRegionID, false);
	end

	-- Obtain data needed to process this region.
	local iW, iH = Map.GetGridSize();
	local region_data_table = self.regionData[iRegionID];
	local iWestX = region_data_table[1];
	local iSouthY = region_data_table[2];
	local iWidth = region_data_table[3];
	local iHeight = region_data_table[4];
	local iAreaID = region_data_table[5];
	local iEastX = (iWestX + iWidth) % iW;
	local iNorthY = (iSouthY + iHeight) % iH;

	local eRegion = self.regionTypes[iRegionID];

	-- Establish scope of center bias.
	local fCenterWidth = (self.centerBias / 100) * iWidth;
	local iNonCenterWidth = math.floor((iWidth - fCenterWidth) / 2);
	local iCenterWidth = iWidth - iNonCenterWidth * 2;
	local iCenterTestWestX = iWestX + iNonCenterWidth; -- "Test" values ignore world wrap for easier membership testing.
	local iCenterTestEastX = iCenterTestWestX + iCenterWidth - 1;

	local fCenterHeight = (self.centerBias / 100) * iHeight;
	local iNonCenterHeight = math.floor((iHeight - fCenterHeight) / 2);
	local iCenterHeight = iHeight - iNonCenterHeight * 2;
	local iCenterTestSouthY = iSouthY + iNonCenterHeight;
	local iCenterTestNorthY = iCenterTestSouthY + iCenterHeight - 1;

	-- Establish scope of "middle donut", outside the center but inside the outer.
	local fMiddleWidth = (self.middleBias / 100) * iWidth;
	local iOuterWidth = math.floor((iWidth - fMiddleWidth) / 2);
	local iMiddleWidth = iWidth - iOuterWidth * 2;
	local iMiddleTestWestX = iWestX + iOuterWidth;
	local iMiddleTestEastX = iMiddleTestWestX + iMiddleWidth - 1;

	local fMiddleHeight = (self.middleBias / 100) * iHeight;
	local iOuterHeight = math.floor((iHeight - fMiddleHeight) / 2);
	local iMiddleHeight = iHeight - iOuterHeight * 2;
	local iMiddleTestSouthY = iSouthY + iOuterHeight;
	local iMiddleTestNorthY = iMiddleTestSouthY + iMiddleHeight - 1;

	-- Assemble candidates lists.
	local two_plots_from_ocean = {};
	local center_candidates = {};
	local middle_candidates = {};
	local outer_plots = {};
	local fallback_plots = {};

	-- Identify candidate plots.
	for region_y = 0, iHeight - 1 do -- When handling global plot indices, process Y first.
		for region_x = 0, iWidth - 1 do
			local x = (region_x + iWestX) % iW; -- Actual coords, adjusted for world wrap, if any.
			local y = (region_y + iSouthY) % iH;
			local iPlotID = GetLuaPlotIndexFromCoordinates(x, y);
			local plot = Map.GetPlot(x, y);
			local ePlot = plot:GetPlotType();
			if ePlot == PlotTypes.PLOT_HILLS or ePlot == PlotTypes.PLOT_LAND then -- Could host a city.
				-- Check if plot is two away from salt water.
				if self.plotDataIsNextToCoast[iPlotID] then
					table.insert(two_plots_from_ocean, iPlotID);
				elseif not bForcedCoastal or self.plotDataIsCoastal[iPlotID] then
					local area_of_plot = plot:GetArea();
					local landmass_of_plot = plot:GetLandmass();
					if iAreaID == -1 or (self.bArea and area_of_plot == iAreaID) or (not self.bArea and landmass_of_plot == iAreaID) then
						-- This plot is a member, so it goes on at least one candidate list.
						-- Test whether plot is in center bias, middle donut, or outer donut.
						local test_x = region_x + iWestX; -- "Test" coords, ignoring any world wrap and
						local test_y = region_y + iSouthY; -- reaching in to virtual space if necessary.
						if (test_x >= iCenterTestWestX and test_x <= iCenterTestEastX) and
							(test_y >= iCenterTestSouthY and test_y <= iCenterTestNorthY) then -- Center Bias.
							table.insert(center_candidates, iPlotID);
						elseif (test_x >= iMiddleTestWestX and test_x <= iMiddleTestEastX) and
							(test_y >= iMiddleTestSouthY and test_y <= iMiddleTestNorthY) then
							table.insert(middle_candidates, iPlotID);
						else
							table.insert(outer_plots, iPlotID);
						end
					end
				end
			end
		end
	end

	--[[ Debug printout.
	print("-");
	print("--- Number of Candidate Plots in Region #", iRegionID, " - Region Type:", eRegion, " ---");
	print("-");
	print("Candidates in Center Bias area: ", #center_candidates);
	print("-");
	print("Candidates in Middle Donut area: ", #middle_candidates);
	print("-");
	print("Candidate Plots in Outer area: ", #outer_plots);
	print("-");
	print("Disqualified, two plots away from salt water: ", #two_plots_from_ocean);
	print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
	--]]

	-- Process lists of candidate plots.
	local candidate_lists = {center_candidates, middle_candidates};
	for _, plot_list in ipairs(candidate_lists) do -- Up to six plot lists, processed by priority.
		local election_returns = self:IterateThroughCandidatePlotList(plot_list, eRegion);
		-- If any candidates are eligible, choose one.
		if election_returns[1] then
			local bestPlotScore = election_returns[2];
			print("Region #", iRegionID, "has", bestPlotScore, "score.");
			local bestPlotIndex = election_returns[3];
			local x, y = GetCoordinatesFromLuaPlotIndex(bestPlotIndex);
			self.startingPlots[iRegionID] = {x, y, bestPlotScore};
			self:PlaceImpactAndRipples(x, y);
			return false;
		end
		-- If none eligible, check for fallback plot.
		if election_returns[4] then
			local bestFallbackScore = election_returns[5];
			print("Region #", iRegionID, "has", bestFallbackScore, "fallback score.");
			local bestFallbackIndex = election_returns[6];
			local x, y = GetCoordinatesFromLuaPlotIndex(bestFallbackIndex);
			table.insert(fallback_plots, {x, y, bestFallbackScore});
		end
	end

	-- Reaching this point means no eligible sites in center bias or middle donut subregions!
	-- Process candidates from Outer subregion, if any.
	if #outer_plots > 0 then
		local outer_eligible_list = {};
		local found_eligible = false;
		local found_fallback = false;
		local bestFallbackScore = -50;
		local bestFallbackIndex;
		-- Process list of candidate plots.
		for _, iPlotID in pairs(outer_plots) do
			local score, meets_minimums = self:EvaluateCandidatePlot(iPlotID, eRegion);
			-- Test current plot against best known plot.
			if meets_minimums then
				found_eligible = true;
				table.insert(outer_eligible_list, iPlotID);
			else
				found_fallback = true;
				if score > bestFallbackScore then
					bestFallbackScore = score;
					bestFallbackIndex = iPlotID;
				end
			end
		end

		-- Iterate through eligible plots and choose the one closest to the center of the region.
		-- Note that sometimes the center of the region rectangle is ambiguous in a hex space,
		-- so we compare the minimum distance from the plot to each of the region corners instead.
		if found_eligible then
			local iClosestPlotIndex;
			local iMinDistance = Map.MaxPlotDistance();

			-- There can be multiple plots with the minimum distance. We shuffle the list to get a "random" plot.
			Shuffle(outer_eligible_list);
			for _, iPlotID in pairs(outer_eligible_list) do
				local x, y = GetCoordinatesFromLuaPlotIndex(iPlotID);
				local iDistanceFromSW = Map.PlotDistance(x, y, iWestX, iSouthY);
				local iDistanceFromNW = Map.PlotDistance(x, y, iWestX, iNorthY);
				local iDistanceFromSE = Map.PlotDistance(x, y, iEastX, iSouthY);
				local iDistanceFromNE = Map.PlotDistance(x, y, iEastX, iNorthY);
				local iShortest = math.min(iDistanceFromSW, iDistanceFromNW, iDistanceFromSE, iDistanceFromNE);
				if iShortest < iMinDistance then -- Found new "closer" plot.
					iClosestPlotIndex = iPlotID;
					iMinDistance = iShortest;
				end
			end
			-- Assign the closest eligible plot as the start point.
			local x, y = GetCoordinatesFromLuaPlotIndex(iClosestPlotIndex);
			-- Re-get plot score for inclusion in start plot data.
			local score = self:EvaluateCandidatePlot(iClosestPlotIndex, eRegion);
			-- Assign this plot as the start for this region.
			self.startingPlots[iRegionID] = {x, y, score};
			self:PlaceImpactAndRipples(x, y);
			return false;
		end
		-- Add the fallback plot (best scored plot) from the Outer region to the fallback list.
		if found_fallback then
			local x, y = GetCoordinatesFromLuaPlotIndex(bestFallbackIndex);
			table.insert(fallback_plots, {x, y, bestFallbackScore});
		end
	end
	-- Reaching here means no plot in the entire region met the minimum standards for selection.

	-- The fallback plot contains the best-scored plots from each test area in this region.
	-- We will compare all the fallback plots and choose the best to be the start plot.
	if #fallback_plots > 0 then
		local iFallbackScore = 0;
		local iFallbackX;
		local iFallbackY;
		for _, plotData in pairs(fallback_plots) do
			local score = plotData[3];
			if score > iFallbackScore then
				iFallbackScore = score;
				iFallbackX = plotData[1];
				iFallbackY = plotData[2];
			end
		end
		-- Assign the start for this region.
		self.startingPlots[iRegionID] = {iFallbackX, iFallbackY, iFallbackScore};
		self:PlaceImpactAndRipples(iFallbackX, iFallbackY);
		return false;
	elseif bForcedCoastal then
		print("Region", iRegionID, "cannot support a coastal start. Try instead to find an inland start for it.");
		return self:FindStart(iRegionID, false);
	else
		print("Region", iRegionID, "cannot have a start and something may have gone way wrong.");
		print("Forcing a one-tile grass in the SW corner of the region to be the start location.");
		local pPlot = Map.GetPlot(iWestX, iSouthY);
		pPlot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
		pPlot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true);
		pPlot:SetFeatureType(FeatureTypes.NO_FEATURE);
		self.startingPlots[iRegionID] = {iWestX, iSouthY, 0};
		self:PlaceImpactAndRipples(iWestX, iSouthY);
		return true;
	end
end
------------------------------------------------------------------------------
--- Attempt to choose a start position (which is along an ocean) for a single region.
--- @param iRegionID RegionId
--- @return boolean bForcedPlacementFlag # Whether the placement is forced on the map with terraform
function ASP:FindCoastalStart(iRegionID)
	return ASP:FindStart(iRegionID, true);
end
------------------------------------------------------------------------------
--- Attempt to choose a start position within a region that can span across landmasses.
--- The most fertile area in the region is prioritized.
--- @param iRegionID RegionId
--- @param bForcedCoastal boolean
--- @return boolean bForcedPlacementFlag # Whether the placement is forced on the map with terraform
function ASP:FindStartWithoutRegardToAreaID(iRegionID, bForcedCoastal)
	-- Obtain data needed to process this region.
	local iW, iH = Map.GetGridSize();
	local region_data_table = self.regionData[iRegionID];
	local iWestX = region_data_table[1];
	local iSouthY = region_data_table[2];
	local iWidth = region_data_table[3];
	local iHeight = region_data_table[4];

	local eRegion = self.regionTypes[iRegionID];

	-- Obtain info on all landmasses wholly or partially within this region, for comparision purposes.
	local land_area_fert = {}; --- @type table<AreaId, integer>
	local land_area_plot_lists = {}; --- @type table<AreaId, PlotId[]>
	-- Cycle through all plots in the region, checking their Start Placement Fertility and AreaID.
	for region_y = 0, iHeight - 1 do
		for region_x = 0, iWidth - 1 do
			local x = (region_x + iWestX) % iW; -- Actual coords, adjusted for world wrap, if any.
			local y = (region_y + iSouthY) % iH;
			local pLoopPlot = Map.GetPlot(x, y);
			local iPlotID = GetLuaPlotIndexFromCoordinates(x, y);
			local ePlot = pLoopPlot:GetPlotType();
			if ePlot == PlotTypes.PLOT_HILLS or ePlot == PlotTypes.PLOT_LAND then -- Land plot, process it.
				local iAreaID = pLoopPlot:GetArea();
				local iFertility = self:MeasureStartPlacementFertilityOfPlot(x, y, false); -- Check for coastal land is disabled.
				land_area_fert[iAreaID] = land_area_fert[iAreaID] or 0;
				land_area_fert[iAreaID] = land_area_fert[iAreaID] + iFertility;

				-- Add candidates to plot list.
				if not self.plotDataIsNextToCoast[iPlotID] and (not bForcedCoastal or self.plotDataIsCoastal[iPlotID]) then
					land_area_plot_lists[iAreaID] = land_area_plot_lists[iAreaID] or {};
					table.insert(land_area_plot_lists[iAreaID], iPlotID);
				end
			end
		end
	end

	-- Prepare array for sorting.
	local tAreaFertilityList = {}; --- @type [AreaId, integer][]
	for iAreaID, iFert in pairs(land_area_fert) do
		table.insert(tAreaFertilityList, {iAreaID, iFert});
	end

	-- Sort by best fertility first.
	table.sort(tAreaFertilityList, function (a, b) return a[2] > b[2] end);

	--[[ Debug printout.
	print("-");
	print("--- Number of Candidate Plots in each landmass in Region #", iRegionID, " - Region Type:", eRegion, " ---");
	print("-");
	for _, tAreaFertility in ipairs(tAreaFertilityList) do
		local iAreaID = tAreaFertility[1];
		print("* Area ID#", iAreaID, "has fertility rating of", tAreaFertility[2], "and candidate plot count of", #land_area_plot_lists[iAreaID]); print("-");
	end
	print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
	--]]

	-- Now iterate through areas, from best fertility downward, looking for a site good enough to choose.
	local fallback_plots = {};
	for _, tAreaFertility in ipairs(tAreaFertilityList) do
		local iAreaID = tAreaFertility[1];
		local tPlotList = land_area_plot_lists[iAreaID];
		local tSelected = self:IterateThroughCandidatePlotList(tPlotList, eRegion);
		-- If any plots in this area are eligible, choose one.
		if tSelected[1] then
			local x, y = GetCoordinatesFromLuaPlotIndex(tSelected[3]);
			self.startingPlots[iRegionID] = {x, y, tSelected[2]};
			self:PlaceImpactAndRipples(x, y);
			return false;
		end
		-- If none eligible, check for fallback plot.
		if tSelected[4] then
			local x, y = GetCoordinatesFromLuaPlotIndex(tSelected[6]);
			table.insert(fallback_plots, {x, y, tSelected[5]});
		end
	end
	-- Reaching this point means no strong sites far enough away from any already-placed start points.

	-- The fallback plot contains the best-scored plots from each test area in this region.
	-- We will compare all the fallback plots and choose the best to be the start plot.
	if #fallback_plots > 0 then
		local iFallbackScore = 0;
		local iFallbackX;
		local iFallbackY;
		for _, plotData in pairs(fallback_plots) do
			local score = plotData[3];
			if score > iFallbackScore then
				iFallbackScore = score;
				iFallbackX = plotData[1];
				iFallbackY = plotData[2];
			end
		end
		-- Assign the start for this region.
		self.startingPlots[iRegionID] = {iFallbackX, iFallbackY, iFallbackScore};
		self:PlaceImpactAndRipples(iFallbackX, iFallbackY);
		return false;
	else
		print("Region", iRegionID, "cannot have a start and something may have gone way wrong.");
		print("Forcing a one-tile grass in the SW corner of the region to be the start location.");
		local pPlot = Map.GetPlot(iWestX, iSouthY);
		pPlot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
		pPlot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true);
		pPlot:SetFeatureType(FeatureTypes.NO_FEATURE);
		self.startingPlots[iRegionID] = {iWestX, iSouthY, 0};
		self:PlaceImpactAndRipples(iWestX, iSouthY);
		return true;
	end
end
------------------------------------------------------------------------------
--- Assign the starting location for each region.<br>
--- Regions with lowest average fertility get their assignment first, to avoid the poor getting poorer.
function ASP:ChooseLocations(args)
	print("Map Generation - Choosing Start Locations for Civilizations");
	args = args or {};
	local mustBeCoast = args.mustBeCoast or false; -- if true, will force all starts on salt water coast if possible

	-- Defaults for evaluating potential start plots are assigned in __Init() but args passed in here can override.
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

	-- Determine whether any civ should start near mountain or snow
	for loop = 1, self.iNumCivs do
		local playerNum = self.player_ID_list[loop]; -- MP games can have gaps between player numbers, so we cannot assume a sequential set of IDs.
		local player = Players[playerNum];
		local civType = GameInfo.Civilizations[player:GetCivilizationType()].Type;
		if CivNeedsMountainStart(civType) then
			self.iNumMountainBias = self.iNumMountainBias + 1;
		end
		if CivNeedsSnowStart(civType) then
			self.iNumSnowBias = self.iNumSnowBias + 1;
		end
	end

	-- Measure terrain/plot/feature in regions.
	self:MeasureTerrainInRegions();

	-- Determine region type.
	self:DetermineRegionTypes();

	-- Sort the regions by average fertility, low to high.
	local tRegionAvgFertList = {}; --- @type [RegionId, integer][]
	for iRegionID, tRegionData in pairs(self.regionData) do
		table.insert(tRegionAvgFertList, {iRegionID, tRegionData[8]});
	end

	table.sort(tRegionAvgFertList, function (a, b) return a[2] < b[2] end);

	-- Find start location for each region in the order of average fertility.
	for _, tRegionAverageFertility in ipairs(tRegionAvgFertList) do
		local iRegionID = tRegionAverageFertility[1];
		local bForcedPlacementFlag = false;

		if self.method == RegionDivision.RECTANGULAR or self.method == RegionDivision.RECTANGULAR_SELF_DEFINED then
			bForcedPlacementFlag = self:FindStartWithoutRegardToAreaID(iRegionID, mustBeCoast);
		else
			bForcedPlacementFlag = self:FindStart(iRegionID, mustBeCoast);
		end

		---[[ Printout for debug only.
		print("- - -");
		print("Start Plot for Region #", iRegionID, " was forced: ", bForcedPlacementFlag);
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
	PrintContentsOfTable(self.impactData[ImpactLayers.LAYER_MAJOR]);
	print("-");
	--]]
end

------------------------------------------------------------------------------
-- Start of functions tied to BalanceAndAssign()
------------------------------------------------------------------------------

--- @param x integer
--- @param y integer
--- @param bAllowOasis boolean
--- @return boolean # Whether anything is placed
--- @return boolean # Whether an oasis is placed
function ASP:AttemptToPlaceBonusResourceAtPlot(x, y, bAllowOasis)
	-- print("-"); print("Attempting to place a Bonus at: ", x, y);
	local plot = Map.GetPlot(x, y);
	if not plot then
		-- print("Placement failed, plot was nil.");
		return false, false;
	end
	if plot:GetResourceType() ~= -1 then
		-- print("Plot already had a resource.");
		return false, false;
	end
	local eTerrain = plot:GetTerrainType();
	if eTerrain == TerrainTypes.TERRAIN_SNOW then
		-- print("Plot was arctic land buried beneath endless snow.");
		return false, false;
	end
	local eFeature = plot:GetFeatureType();
	if eFeature == FeatureTypes.FEATURE_OASIS then
		-- print("Plot already had an Oasis.");
		return false, false;
	end
	local ePlot = plot:GetPlotType();
	local eResource;
	-- Randomize resource selected instead -- September 2020, azum4roll
	if eFeature == FeatureTypes.FEATURE_JUNGLE then -- Place Banana
		if ePlot == PlotTypes.PLOT_LAND then
			eResource = self.banana_ID;
		else -- nothing fits here
			return false, false;
		end
	elseif eFeature == FeatureTypes.FEATURE_FOREST then -- Place Deer
		eResource = self.deer_ID;
	elseif ePlot == PlotTypes.PLOT_HILLS and eFeature == FeatureTypes.NO_FEATURE then
		eResource = self.sheep_ID;
	elseif ePlot == PlotTypes.PLOT_LAND then
		if eFeature == FeatureTypes.NO_FEATURE then
			if eTerrain == TerrainTypes.TERRAIN_GRASS then -- Place Cows, Rice or Bison
				local diceroll = Map.Rand(3, "Selection of Bonus Resource type - Start Normalization LUA");
				if diceroll < 2 then
					eResource = self.cow_ID;
				else
					if IsTropical(y) then
						eResource = self.rice_ID;
					else
						eResource = self.bison_ID;
					end
				end
			elseif eTerrain == TerrainTypes.TERRAIN_PLAINS then -- Place Wheat, Maize or Bison
				local diceroll = Map.Rand(3, "Selection of Bonus Resource type - Start Normalization LUA");
				if diceroll < 2 then
					if IsTropical(y) then
						eResource = self.maize_ID;
					else
						eResource = self.wheat_ID;
					end
				else
					eResource = self.bison_ID;
				end
			elseif eTerrain == TerrainTypes.TERRAIN_TUNDRA then -- Place Deer
				eResource = self.deer_ID;
			elseif eTerrain == TerrainTypes.TERRAIN_DESERT then
				if plot:IsFreshWater() then -- Place Wheat or Maize
					if IsTropical(y) then
						eResource = self.maize_ID;
					else
						eResource = self.wheat_ID;
					end
				elseif bAllowOasis then -- Place Oasis
					plot:SetFeatureType(FeatureTypes.FEATURE_OASIS);
					-- print("Placed Oasis.");
					return true, true;
				else
					-- print("Not allowed to place any more Oasis help at this site.");
				end
			end
		end
	elseif ePlot == PlotTypes.PLOT_OCEAN then
		if eTerrain == TerrainTypes.TERRAIN_COAST and eFeature == FeatureTypes.NO_FEATURE then
			if not plot:IsLake() then -- Place Fish
				eResource = self.fish_ID;
			end
		end
	end

	-- Place resource here
	if eResource then
		plot:SetResourceType(eResource, 1);
		-- print("Placed " .. L(GameInfo.Resources[eResource].Description));
		self.amounts_of_resources_placed[eResource + 1] = self.amounts_of_resources_placed[eResource + 1] + 1;
		return true, false;
	end

	-- Nothing placed.
	return false, false;
end
------------------------------------------------------------------------------
--- Convert the specified flatland plot into featureless hills, if possible.
--- The plot cannot have a resource or forest or be on a river.
--- @param x integer
--- @param y integer
--- @return boolean # Whether the plot is converted into hills
function ASP:AttemptToPlaceHillsAtPlot(x, y)
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
	if plot:GetPlotType() ~= PlotTypes.PLOT_LAND then
		-- print("Placement failed, plot was not flatland.");
		return false;
	end
	if plot:IsRiver() then
		-- print("Placement failed, plot was next to river.");
		return false;
	end
	if plot:GetFeatureType() == FeatureTypes.FEATURE_FOREST then
		-- print("Placement failed, plot had a forest already.");
		return false;
	end
	-- Change the plot type from flatlands to hills and clear any features.
	plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
	plot:SetFeatureType(FeatureTypes.NO_FEATURE);
	return true;
end
------------------------------------------------------------------------------
function ASP:AttemptToPlaceSmallStrategicAtPlot(x, y)
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
	local ePlot = plot:GetPlotType();
	local eTerrain = plot:GetTerrainType();
	local eFeature = plot:GetFeatureType();
	if ePlot ~= PlotTypes.PLOT_LAND then
		-- print("Placement failed, plot was not flat land.");
		return false;
	elseif eFeature == FeatureTypes.NO_FEATURE then
		if eTerrain == TerrainTypes.TERRAIN_GRASS or eTerrain == TerrainTypes.TERRAIN_PLAINS then -- Could be horses.
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
function ASP:AddStrategicBalanceResources(region_number)
	-- This function adds the required Strategic Resources to start plots,
	-- for games that have selected to enable Strategic Resource Balance.
	local start_point_data = self.startingPlots[region_number];
	local x = start_point_data[1];
	local y = start_point_data[2];
	local iron_list, horse_list = {}, {};
	local iron_fallback, horse_fallback = {}, {};

	-- print("- Adding Strategic Balance Resources for start location in Region#", region_number);

	for pPlot, iDistance in Plot_GetPlotsInCircle(Map.GetPlot(x, y), 4, 6) do
		local ePlot = pPlot:GetPlotType();
		local eTerrain = pPlot:GetTerrainType();
		local eFeature = pPlot:GetFeatureType();
		local iPlotIndex = pPlot:GetIndex() + 1;

		-- Check this plot for resource placement eligibility.
		if ePlot == PlotTypes.PLOT_HILLS and eFeature ~= FeatureTypes.FEATURE_JUNGLE then
			if iDistance < 5 then
				table.insert(iron_list, iPlotIndex);
			else
				table.insert(iron_fallback, iPlotIndex);
			end
		elseif ePlot == PlotTypes.PLOT_LAND then
			if eFeature == FeatureTypes.NO_FEATURE then
				if eTerrain == TerrainTypes.TERRAIN_TUNDRA or eTerrain == TerrainTypes.TERRAIN_DESERT then
					if pPlot:IsFreshWater() then
						table.insert(horse_fallback, iPlotIndex);
					else
						table.insert(iron_fallback, iPlotIndex);
					end
				elseif eTerrain == TerrainTypes.TERRAIN_PLAINS or eTerrain == TerrainTypes.TERRAIN_GRASS then
					if iDistance < 5 then
						table.insert(horse_list, iPlotIndex);
					else
						table.insert(horse_fallback, iPlotIndex);
					end
					table.insert(iron_fallback, iPlotIndex);
				elseif eTerrain == TerrainTypes.TERRAIN_SNOW then
					if iDistance < 5 then
						table.insert(iron_list, iPlotIndex);
					else
						table.insert(iron_fallback, iPlotIndex);
					end
				end
			elseif eFeature == FeatureTypes.FEATURE_FLOOD_PLAINS then
				table.insert(horse_list, iPlotIndex);
				table.insert(iron_fallback, iPlotIndex);
			elseif eFeature == FeatureTypes.FEATURE_FOREST then
				if iDistance < 5 then
					table.insert(iron_list, iPlotIndex);
				else
					table.insert(iron_fallback, iPlotIndex);
				end
			end
		end
	end

	local _, horse_amt, _, iron_amt = self:GetSmallStrategicResourceQuantityValues();
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
--- Attempt to place a cattle deposit on the specified plot if it is featureless flat grassland.
function ASP:AttemptToPlaceCattleAtGrassPlot(x, y)
	-- print("-"); print("Attempting to place Cattle at: ", x, y);
	local plot = Map.GetPlot(x, y);
	if not plot then
		return false;
	end

	if plot:GetResourceType() ~= -1 then
		return false;
	end

	if plot:GetPlotType() ~= PlotTypes.PLOT_LAND then
		return false;
	end

	if plot:GetFeatureType() ~= FeatureTypes.NO_FEATURE then
		return false;
	end

	if plot:GetTerrainType() ~= TerrainTypes.TERRAIN_GRASS then
		return false;
	end

	plot:SetResourceType(self.cow_ID, 1);
	-- print("Placed Cattle.");
	self.amounts_of_resources_placed[self.cow_ID + 1] = self.amounts_of_resources_placed[self.cow_ID + 1] + 1;
	return true;
end
------------------------------------------------------------------------------
--- Attempt to place a stone deposit on the specified plot if it is featureless flat grassland.
--- This function is currently UNUSED.
--- @param x integer
--- @param y integer
--- @return boolean
function ASP:AttemptToPlaceStoneAtGrassPlot(x, y)
	-- print("-"); print("Attempting to place Stone at: ", x, y);
	local plot = Map.GetPlot(x, y);
	if not plot then
		return false;
	end

	if plot:GetResourceType() ~= -1 then
		return false;
	end

	if plot:GetPlotType() ~= PlotTypes.PLOT_LAND then
		return false;
	end

	if plot:GetFeatureType() ~= FeatureTypes.NO_FEATURE then
		return false;
	end

	if plot:GetTerrainType() ~= TerrainTypes.TERRAIN_GRASS then
		return false;
	end

	plot:SetResourceType(self.stone_ID, 1);
	-- print("Placed Stone.");
	self.amounts_of_resources_placed[self.stone_ID + 1] = self.amounts_of_resources_placed[self.stone_ID + 1] + 1;
	return true;
end
------------------------------------------------------------------------------
--- Extracted from NormalizeStartLocation() and NormalizeCityState().<br>
--- Measure the value of land in two rings around a given start location.<br>
--- Note that the food and production counts depend on the balance of terrain and early improvement yields,
--- so any change to those will require modification to this function.
--- @param pPlot Plot
--- @return {
--- 	[1]: PlotYieldPotential,
--- 	[2]: PlotYieldPotential,
--- 	Trees: integer,
--- 	Grass: integer,
--- 	Plains: integer,
--- }
--- @return boolean bNextToLake
--- @return boolean bNearRiver
function ASP:ScanPlotsInTwoRings(pPlot)
	-- Count the surrounding plots based on their early game yield potentials.
	-- Only plot type, terrain type, and feature type are considered, as resources haven't been placed yet.
	-- Potential includes early game generic improvements i.e. farms and mines.
	-- Forests and jungles count as a later production source due to chopping potential.
	-- Number of native 2+ food plots (pre-chop) is also a factor on number of bonus resources. Featureless coast is specifically excluded.
	local tCount = {
		[1] = {
			Food = {Two = 0, Three = 0, Four = 0, Five = 0},
			Prod = {One = 0, Two = 0, Three = 0},
			NativeTwoFood = 0,
		},
		[2] = {
			Food = {Two = 0, Three = 0, Four = 0, Five = 0},
			Prod = {One = 0, Two = 0, Three = 0},
			NativeTwoFood = 0,
		},
		Trees = 0,
		Grass = 0,
		Plains = 0,
	};

	local bNextToLake = false;
	local bNearRiver = false;

	-- Data Chart for tile potentials (flat unless otherwise stated)
	-- 5F: Flood Plains
	-- 4F: Atoll, Grass on fresh water (includes features)
	-- 3F: Lake, dry Grass (includes features), Plains on fresh water (includes features), Tundra on fresh water (includes features), Oasis, Jungle on Grass on Hills
	-- 2F: Dry Plains (includes features), Coast, Forest on Grass on Hills, Jungle on Plains on Hills
	-- 3P: Non-Snow Hills (includes features)
	-- 2P: Forest on Plains, Atoll
	-- 1P: Plains, Jungle on Plains, Forest on Grass/Tundra
	for pLoopPlot, iDistance in Plot_GetPlotsInCircle(pPlot, 2) do
		local ePlot = pLoopPlot:GetPlotType();
		local eTerrain = pLoopPlot:GetTerrainType();
		local eFeature = pLoopPlot:GetFeatureType();
		local tPotential = tCount[iDistance];

		if ePlot == PlotTypes.PLOT_OCEAN then
			if eFeature ~= FeatureTypes.FEATURE_ICE then
				if pLoopPlot:IsLake() then
					if iDistance == 1 then
						bNextToLake = true;
					end
					tPotential.Food.Three = tPotential.Food.Three + 1;
					tPotential.Prod.One = tPotential.Prod.One + 1;
					tPotential.NativeTwoFood = tPotential.NativeTwoFood + 1;
				elseif eTerrain == TerrainTypes.TERRAIN_COAST then
					if eFeature == FeatureTypes.FEATURE_ATOLL then
						tPotential.Food.Four = tPotential.Food.Four + 1;
						tPotential.Prod.Two = tPotential.Prod.Two + 1;
						tPotential.NativeTwoFood = tPotential.NativeTwoFood + 1;
					else
						tPotential.Food.Two = tPotential.Food.Two + 1;
					end
				end
			end
		elseif ePlot ~= PlotTypes.PLOT_MOUNTAIN then -- hills or flatland
			if pLoopPlot:IsRiver() then
				bNearRiver = true;
			end
			if eFeature == FeatureTypes.FEATURE_JUNGLE or eFeature == FeatureTypes.FEATURE_FOREST then
				tCount.Trees = tCount.Trees + 1;
			end
			if ePlot == PlotTypes.PLOT_HILLS then
				if eTerrain ~= TerrainTypes.TERRAIN_SNOW then
					tPotential.Prod.Three = tPotential.Prod.Three + 1;
				end
				if eTerrain == TerrainTypes.TERRAIN_GRASS then
					if eFeature == FeatureTypes.FEATURE_JUNGLE then
						tPotential.Food.Three = tPotential.Food.Three + 1;
						tPotential.NativeTwoFood = tPotential.NativeTwoFood + 1;
					elseif eFeature == FeatureTypes.FEATURE_FOREST then
						tPotential.Food.Two = tPotential.Food.Two + 1;
						tPotential.NativeTwoFood = tPotential.NativeTwoFood + 1;
					end
				elseif eTerrain == TerrainTypes.TERRAIN_PLAINS and eFeature == FeatureTypes.FEATURE_JUNGLE then
					tPotential.Food.Two = tPotential.Food.Two + 1;
					tPotential.NativeTwoFood = tPotential.NativeTwoFood + 1;
				end
			elseif eFeature == FeatureTypes.FEATURE_OASIS then
				tPotential.Food.Three = tPotential.Food.Three + 1;
				tPotential.NativeTwoFood = tPotential.NativeTwoFood + 1;
			elseif eFeature == FeatureTypes.FEATURE_FLOOD_PLAINS then
				tPotential.Food.Five = tPotential.Food.Five + 1;
				tPotential.NativeTwoFood = tPotential.NativeTwoFood + 1;
			else
				if eFeature == FeatureTypes.FEATURE_FOREST then
					if eTerrain == TerrainTypes.TERRAIN_GRASS or eTerrain == TerrainTypes.TERRAIN_TUNDRA then
						tPotential.Prod.One = tPotential.Prod.One + 1;
					elseif eTerrain == TerrainTypes.TERRAIN_PLAINS then
						tPotential.Prod.Two = tPotential.Prod.Two + 1;
					end
				else -- other features don't affect prod
					if eTerrain == TerrainTypes.TERRAIN_PLAINS then
						tPotential.Prod.One = tPotential.Prod.One + 1;
					end
				end
				if eTerrain == TerrainTypes.TERRAIN_GRASS then
					tCount.Grass = tCount.Grass + 1;
					tPotential.NativeTwoFood = tPotential.NativeTwoFood + 1;
				elseif eTerrain == TerrainTypes.TERRAIN_PLAINS then
					tCount.Plains = tCount.Plains + 1;
					if eFeature == FeatureTypes.FEATURE_JUNGLE then
						tPotential.NativeTwoFood = tPotential.NativeTwoFood + 1;
					end
				end
				if pLoopPlot:IsFreshWater() then
					if eTerrain == TerrainTypes.TERRAIN_GRASS then
						tPotential.Food.Four = tPotential.Food.Four + 1;
					elseif eTerrain == TerrainTypes.TERRAIN_PLAINS or eTerrain == TerrainTypes.TERRAIN_TUNDRA then
						tPotential.Food.Three = tPotential.Food.Three + 1;
					end
				else -- dry flatland
					if eTerrain == TerrainTypes.TERRAIN_GRASS then
						tPotential.Food.Three = tPotential.Food.Three + 1;
					elseif eTerrain == TerrainTypes.TERRAIN_PLAINS or eTerrain == TerrainTypes.TERRAIN_TUNDRA then
						tPotential.Food.Two = tPotential.Food.Two + 1;
					end
				end
			end
		end
	end

	return tCount, bNextToLake, bNearRiver;
end
------------------------------------------------------------------------------
--- Measure the value of land in two rings around a given start location,
--- primarily for the purpose of determining how much support the site requires in the form of Bonus Resources.
--- Also save the start data to `self.startLocationConditions`.
--- @param iRegionId RegionId
function ASP:NormalizeStartLocation(iRegionId)
	local start_point_data = self.startingPlots[iRegionId];
	local x = start_point_data[1];
	local y = start_point_data[2];
	local plot = Map.GetPlot(x, y);
	local plotIndex = GetLuaPlotIndexFromCoordinates(x, y);

	-- Remove any feature Ice from the first ring.
	self:GenerateLuxuryPlotListsAtCitySite(x, y, 1, true);

	-- Scan the two rings around start plot.
	local tCount, bNextToLake, bNearRiver = self:ScanPlotsInTwoRings(plot);

	-- Generate plot lists of each of the three rings around start.
	-- Also count the number of desert and tundra plots here for future use.
	local tRingPlots = {{}, {}, {}};
	local iNumDesert, iNumTundra = 0, 0;
	for i = 1, 3 do
		for pLoopPlot in Plot_GetPlotsInCircle(plot, i, i) do
			table.insert(tRingPlots[i], pLoopPlot);
			local eTerrain = pLoopPlot:GetTerrainType();
			if eTerrain == TerrainTypes.TERRAIN_DESERT then
				iNumDesert = iNumDesert + 1;
			elseif eTerrain == TerrainTypes.TERRAIN_TUNDRA then
				iNumTundra = iNumTundra + 1;
			end
		end
	end

	-- Adjust the hammer situation, if needed.
	local iProdScore1 = 4 * tCount[1].Prod.Three + 2 * tCount[1].Prod.Two + tCount[1].Prod.One;
	local iProdScore2 = 2 * tCount[2].Prod.Three + tCount[2].Prod.Two + tCount[2].Prod.One;
	local iLaterProdScore = 2 * tCount.Trees;

	-- If drastic shortage, attempt to add a hill to first ring.
	if (iProdScore2 < 8 and iProdScore1 < 2) or iProdScore1 == 0 then
		local bPlaced = false;
		for _, pLoopPlot in pairs(GetShuffledCopyOfTable(tRingPlots[1])) do
			-- Attempt to place a Hill at the currently chosen plot.
			local iLoopX = pLoopPlot:GetX();
			local iLoopY = pLoopPlot:GetY();
			bPlaced = self:AttemptToPlaceHillsAtPlot(iLoopX, iLoopY);
			if bPlaced then
				iProdScore1 = iProdScore1 + 4;
				print("Added hills next to hammer-poor start plot", x, y, "at", iLoopX, iLoopY);
				break;
			end
		end
		if not bPlaced then
			print("FAILED to add hills next to hammer-poor start plot", x, y);
		end
	end

	-- If later (early game) production will be too short, attempt to add a forest to second ring.
	if iProdScore1 + iProdScore2 + iLaterProdScore < 14 then
		local bSuccess = false;
		for _, pLoopPlot in pairs(GetShuffledCopyOfTable(tRingPlots[2])) do
			-- Attempt to place a Forest at the currently chosen plot.
			if pLoopPlot:GetResourceType() == -1 then -- No resource here, safe to proceed.
				if pLoopPlot:GetPlotType() == PlotTypes.PLOT_LAND then
					local eTerrain = pLoopPlot:GetTerrainType();
					if eTerrain ~= TerrainTypes.TERRAIN_DESERT and eTerrain ~= TerrainTypes.TERRAIN_SNOW then
						if pLoopPlot:GetFeatureType() == FeatureTypes.NO_FEATURE then
							pLoopPlot:SetFeatureType(FeatureTypes.FEATURE_FOREST);
							bSuccess = true;
							print("Added forest near hammer-poor start plot", x, y, "at", pLoopPlot:GetX(), pLoopPlot:GetY());
							break;
						end
					end
				end
			end
		end
		if not bSuccess then
			print("FAILED to add forest near hammer-poor start plot", x, y);
		end
	end

	-- Rate the food situation.
	local iFoodScore1 = 6 * tCount[1].Food.Five + 4 * tCount[1].Food.Four + 2 * tCount[1].Food.Three + tCount[1].Food.Two;
	local iFoodScore2 = 6 * tCount[2].Food.Five + 4 * tCount[2].Food.Four + 2 * tCount[2].Food.Three + tCount[2].Food.Two;
	local iTotalFoodScore = iFoodScore1 + iFoodScore2;
	local iTotalNativeTwoFood = tCount[1].NativeTwoFood + tCount[2].NativeTwoFood;

	-- Six levels for Bonus Resource support, from zero to five.
	local iNumFoodBonusNeeded = 0;
	if iTotalFoodScore < 4 and iFoodScore1 == 0 then
		iNumFoodBonusNeeded = 5;
	elseif iTotalFoodScore < 6 then
		iNumFoodBonusNeeded = 4;
	elseif iTotalFoodScore < 8 then
		iNumFoodBonusNeeded = 3;
	elseif iTotalFoodScore < 12 and iFoodScore1 < 5 then
		iNumFoodBonusNeeded = 3;
	elseif iTotalFoodScore < 17 and iFoodScore1 < 9 then
		iNumFoodBonusNeeded = 2;
	elseif iTotalNativeTwoFood < 2 then
		iNumFoodBonusNeeded = 2;
	elseif iTotalFoodScore < 24 and iFoodScore1 < 11 then
		iNumFoodBonusNeeded = 1;
	elseif iTotalNativeTwoFood < 3 or tCount[1].NativeTwoFood == 0 then
		iNumFoodBonusNeeded = 1;
	elseif iTotalFoodScore < 20 then
		iNumFoodBonusNeeded = 1;
	end

	-- Add mandatory Iron and Horse near the start location if Strategic Balance option is enabled.
	if self.resBalance then
		self:AddStrategicBalanceResources(iRegionId);
	end

	-- Check for Legendary Start resource option.
	if self.legStart then
		iNumFoodBonusNeeded = iNumFoodBonusNeeded + 2;
	end

	-- Check to see if a Grass tile needs to be added at a site with zero native 2-food tiles in first two rings.
	if iTotalNativeTwoFood == 0 and iNumFoodBonusNeeded < 3 then
		local plot_list = {};
		for pLoopPlot in Plot_GetPlotsInCircle(plot, 2) do
			if pLoopPlot:GetResourceType() == -1 then -- No resource here, safe to proceed.
				local ePlot = pLoopPlot:GetPlotType();
				local eTerrain = pLoopPlot:GetTerrainType();
				local eFeature = pLoopPlot:GetFeatureType();

				-- Now check this plot for eligibility to be converted to flat open grassland.
				if ePlot == PlotTypes.PLOT_LAND then
					if eTerrain == TerrainTypes.TERRAIN_PLAINS then
						if eFeature == FeatureTypes.NO_FEATURE then
							table.insert(plot_list, pLoopPlot);
						end
					end
				end
			end
		end

		-- No tile is eligible for conversion. Add extra bonus resources instead.
		if #plot_list == 0 then
			iNumFoodBonusNeeded = 3;
		else
			print("-"); print("*** START HAD NO 2-FOOD TILES, YET ONLY QUALIFIED FOR 2 BONUS; CONVERTING A PLAINS TO GRASS! ***"); print("-");
			local pSelectedPlot = GetRandomFromList(plot_list, "Choosing plot to convert to Grass near food-poor Plains start - LUA");
			pSelectedPlot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, false);
		end
	end

	-- Add Bonus Resources to food-poor start positions.
	if iNumFoodBonusNeeded > 0 then
		print("-");
		print("Food-Poor start", x, y, "needs", iNumFoodBonusNeeded, "bonus resources.");
		print("-");

		local tMaxRingBonus = self.legStart and {2, 4, iNumFoodBonusNeeded} or {1, 3, iNumFoodBonusNeeded};
		local bPlacedOasis = false;

		-- Try each ring in order.
		local iPlacedBonus = 0;
		for i = 1, 3 do
			tMaxRingBonus[i] = math.min(iNumFoodBonusNeeded, tMaxRingBonus[i]);
			if iPlacedBonus < tMaxRingBonus[i] then
				for _, pLoopPlot in pairs(GetShuffledCopyOfTable(tRingPlots[i])) do
					local iLoopX, iLoopY = pLoopPlot:GetX(), pLoopPlot:GetY();
					local bPlacedBonus = false;
					-- Only place one oasis at most.
					bPlacedBonus, bPlacedOasis = self:AttemptToPlaceBonusResourceAtPlot(iLoopX, iLoopY, not bPlacedOasis);
					if bPlacedBonus then
						print("Placed a Bonus/Oasis in ring #", i, "at", iLoopX, iLoopY);
						iPlacedBonus = iPlacedBonus + 1;
						if iPlacedBonus >= tMaxRingBonus[i] then
							break;
						end
					end
				end
			end
		end
	end

	-- Check for heavy grass and light plains. Add Cattle to a grass plot if grass count is high and plains count is low.
	local iNumCattleNeeded = 0;
	if tCount.Grass >= 9 and tCount.Plains == 0 then
		iNumCattleNeeded = 2;
	elseif tCount.Grass >= 6 and tCount.Plains <= 4 then
		iNumCattleNeeded = 1;
	end
	if iNumCattleNeeded > 0 then -- Add Cattle to this grass start.
		local tMaxRingCattle = {1, iNumCattleNeeded};
		local iPlacedCattle = 0;
		for i = 1, 2 do
			if iPlacedCattle < tMaxRingCattle[i] then
				for _, pLoopPlot in pairs(GetShuffledCopyOfTable(tRingPlots[i])) do
					local iLoopX, iLoopY = pLoopPlot:GetX(), pLoopPlot:GetY();
					-- Attempt to place Cattle at the currently chosen plot.
					if self:AttemptToPlaceCattleAtGrassPlot(iLoopX, iLoopY) then
						print("Placed Cattle in first ring at ", iLoopX, iLoopY);
						iPlacedCattle = iPlacedCattle + 1;
						if iPlacedCattle >= tMaxRingCattle[i] then
							break;
						end
					end
				end
			end
		end
	end

	-- Record conditions at this start location.
	self.startLocationConditions[iRegionId] = {
		IsCoastal = self.plotDataIsCoastal[plotIndex],
		IsLake = bNextToLake,
		IsRiver = plot:IsRiver(),
		IsNearRiver = bNearRiver,
		DesertCount = iNumDesert,
		TundraCount = iNumTundra
	};
end
------------------------------------------------------------------------------
--- Does nothing even in vanilla, but some mapscripts call this expecting it to do something...
function ASP:NormalizeTeamLocations()
	print("NormalizeTeamLocations: This function is a Civ4 leftover and does nothing even in vanilla. You can remove the function call.");
end
------------------------------------------------------------------------------
--- Find the best unassigned region with most plots with the corresponding terrain/feature of eRegion
--- @param eRegion RegionType
--- @param tAvailableRegions RegionId[]
--- @return RegionId
function ASP:FindFallbackForUnmatchedRegionPriority(eRegion, tAvailableRegions)
	local iBestRegionId = -1;
	local fMaxTerrainCount = 0;

	for _, iRegionId in ipairs(tAvailableRegions) do
		local tRawCounts = self.regionTerrainCounts[iRegionId];
		local fTerrainCount = 0;
		if eRegion == GameInfoTypes.REGION_HILLS then
			fTerrainCount = tRawCounts[5];
		elseif eRegion == GameInfoTypes.REGION_GRASS then
			fTerrainCount = tRawCounts[11];
		elseif eRegion == GameInfoTypes.REGION_PLAINS then
			fTerrainCount = tRawCounts[12];
		elseif eRegion == GameInfoTypes.REGION_DESERT then
			fTerrainCount = tRawCounts[13] + (tRawCounts[20] + tRawCounts[21]) / 2;
		elseif eRegion == GameInfoTypes.REGION_TUNDRA then
			fTerrainCount = tRawCounts[14];
		elseif eRegion == GameInfoTypes.REGION_FOREST then
			fTerrainCount = tRawCounts[16];
		elseif eRegion == GameInfoTypes.REGION_JUNGLE then
			fTerrainCount = tRawCounts[17];
		elseif eRegion == GameInfoTypes.REGION_HYBRID then
			fTerrainCount = tRawCounts[11] + tRawCounts[12];
		end

		if fTerrainCount > fMaxTerrainCount then
			iBestRegionId = iRegionId;
			fMaxTerrainCount = fTerrainCount;
		end
	end

	return iBestRegionId;
end
------------------------------------------------------------------------------
--- Determine what level of Bonus Resource support a location may need, identify compatibility with civ-specific biases, and place starts
function ASP:BalanceAndAssign()
	local iNumRegions = #self.startingPlots;

	-- Normalize each start plot location.
	for iRegionId = 1, iNumRegions do
		self:NormalizeStartLocation(iRegionId);
	end

	-- Check Game Option for disabling civ-specific biases.
	-- If they are to be disabled, then all civs are simply assigned to start plots at random.
	if Game.GetCustomOption("GAMEOPTION_DISABLE_START_BIAS") == 1 then
		print("-"); print("ALERT: Civ Start Biases have been selected to be Disabled!"); print("-");

		-- Create a randomized list of all regions.
		local tRegions = {};
		for i = 1, iNumRegions do
			table.insert(tRegions, i);
		end
		Shuffle(tRegions);

		for i, ePlayer in ipairs(self.player_ID_list) do
			local iRegionId = tRegions[i];
			local x = self.startingPlots[iRegionId][1];
			local y = self.startingPlots[iRegionId][2];
			print("Now placing Player#", ePlayer, "in Region#", iRegionId, "at start plot:", x, y);
			Players[ePlayer]:SetStartingPlot(Map.GetPlot(x, y));
		end

		-- Done with un-biased Civ placement.
		return;
	end

	-- If the process reaches here, civ-specific start-location biases are enabled. Handle them now.
	-- Initialize the cost matrix between each player and each region according to start biases.
	-- 0 means perfect match, and 1 means complete non-match. Partial matches get a score in-between.
	-- Priority coastal, snow, and mountain biases get -99 for being absolute priorities. All other biases are ignored for these civs.
	-- Coastal non-matches get an extra +0.5 score as coastal bias is more important to fulfill.
	local tCostMatrix = {};
	for i, ePlayer in ipairs(self.player_ID_list) do
		tCostMatrix[i] = {};
		local kCivInfo = GameInfo.Civilizations[Players[ePlayer]:GetCivilizationType()];
		print("Player", ePlayer, L(kCivInfo.ShortDescription));

		-- Go through all bias tables to get the requirements.
		local bNeedMountain = false;
		local bNeedSnow = false;
		local bNeedPriorityCoast = false;
		local bNeedCoast = false;
		local bNeedRiver = false;
		local tPreferRegions = {};
		local tAvoidRegions = {};
		local iNumReq = 0;
		if CivNeedsMountainStart(kCivInfo.Type) then
			print("- - - - - - - needs Mountain Start!"); print("-");
			bNeedMountain = true;
			iNumReq = iNumReq + 1;
		elseif CivNeedsSnowStart(kCivInfo.Type) then
			print("- - - - - - - needs Snow Start!"); print("-");
			bNeedSnow = true;
			iNumReq = iNumReq + 1;
		elseif CivNeedsPlaceFirstCoastalStart(kCivInfo.Type) then
			print("- - - - - - - needs Priority Coastal Start!"); print("-");
			bNeedPriorityCoast = true;
			iNumReq = iNumReq + 1;
		else
			if CivNeedsCoastalStart(kCivInfo.Type) then
				print("- - - - - - - needs Coastal Start!"); print("-");
				bNeedCoast = true;
				iNumReq = iNumReq + 1;
			end
			if CivNeedsRiverStart(kCivInfo.Type) then
				print("- - - - - - - needs River Start!"); print("-");
				bNeedRiver = true;
				iNumReq = iNumReq + 1;
			end
			local bHasPreferAvoid = false;
			if GetNumStartRegionPriorityForCiv(kCivInfo.Type) > 0 then
				print("- - - - - - - needs Region Priority!"); print("-");
				tPreferRegions = GetStartRegionPriorityListForCiv_GetIDs(kCivInfo.Type);
				PrintContentsOfTable(tPreferRegions);
				bHasPreferAvoid = true;
			end
			if GetNumStartRegionAvoidForCiv(kCivInfo.Type) > 0 then
				print("- - - - - - - needs Region Avoid!"); print("-");
				tAvoidRegions = GetStartRegionAvoidListForCiv_GetIDs(kCivInfo.Type);
				PrintContentsOfTable(tAvoidRegions);
				bHasPreferAvoid = true;
			end
			if bHasPreferAvoid then
				iNumReq = iNumReq + 1;
			end
		end

		-- Go through all regions and assign a score for each.
		for iRegionId = 1, iNumRegions do
			local eRegion = self.regionTypes[iRegionId];
			local tStartConditions = self.startLocationConditions[iRegionId];
			tCostMatrix[i][iRegionId] = 0.0;
			if bNeedMountain and eRegion == GameInfoTypes.REGION_MOUNTAIN then
				tCostMatrix[i][iRegionId] = -99;
			elseif bNeedSnow and eRegion == GameInfoTypes.REGION_SNOW then
				tCostMatrix[i][iRegionId] = -99;
			elseif bNeedPriorityCoast and tStartConditions.IsCoastal then
				tCostMatrix[i][iRegionId] = -99;
			elseif iNumReq > 0 then
				local fScore = 0.0;
				if bNeedCoast then
					if tStartConditions.IsCoastal then
						fScore = fScore + 1;
					elseif tStartConditions.IsLake then
						fScore = fScore + 0.1; -- fallback score
					else
						tCostMatrix[i][iRegionId] = tCostMatrix[i][iRegionId] + 0.5;
					end
				end
				if bNeedRiver then
					if tStartConditions.IsRiver then
						fScore = fScore + 1;
					elseif tStartConditions.IsNearRiver then
						fScore = fScore + 0.2; -- fallback score
					end
				end
				-- Both prefer and avoid are considered together
				if next(tPreferRegions) or next(tAvoidRegions) then
					if (not next(tPreferRegions) or IdentifyTableIndex(tPreferRegions, eRegion)) and not IdentifyTableIndex(tAvoidRegions, eRegion) then
						fScore = fScore + 1;
					end
				end
				-- More positive = less matched
				tCostMatrix[i][iRegionId] = tCostMatrix[i][iRegionId] + 1 - fScore / iNumReq;
			end
		end
	end

	---[[ Debug printout
	print("Cost Matrix: ");
	for i = 1, iNumRegions do
		print(table.concat(tCostMatrix[i], ", "));
	end
	--]]

	-- Call the min matching function
	local tBestMatches, fMinScore = GetMinMatching(tCostMatrix);
	print("Final match score =", fMinScore);

	-- Assign starts to each player
	for i, ePlayer in ipairs(self.player_ID_list) do
		local iRegionId = tBestMatches[i];
		local x = self.startingPlots[iRegionId][1];
		local y = self.startingPlots[iRegionId][2];
		Players[ePlayer]:SetStartingPlot(Map.GetPlot(x, y));
		print("Player#", ePlayer, "is assigned to Region#", iRegionId, "at Plot", x, y);
	end
end

------------------------------------------------------------------------------
-- Start of functions tied to PlaceNaturalWonders()
------------------------------------------------------------------------------

--- Check for eligibility requirements applicable to all Natural Wonders.
--- If the candidate plot passes all such checks, we will move onto checking it against specific needs for each particular Wonder.
--- @param x integer
--- @param y integer
--- @return boolean # Whether this plot is eligible for a land wonder
--- @return boolean # Whether this plot is eligible for a sea wonder
function ASP:ExaminePlotForNaturalWondersEligibility(x, y)
	local iPlotIndex = GetLuaPlotIndexFromCoordinates(x, y);

	-- Check for collision with player starts
	if self.impactData[ImpactLayers.LAYER_NATURAL_WONDER][iPlotIndex] > 0 then
		return false, false;
	end

	-- Must have land in workable range
	local pPlot = Map.GetPlot(x, y);
	local bHasLand = false;
	for pNearPlot in Plot_GetPlotsInCircle(pPlot, 1, 3) do
		if pNearPlot:GetPlotType() == PlotTypes.PLOT_LAND then
			bHasLand = true;
		end
	end
	if not bHasLand then
		return false, false;
	end

	-- Must be a decent city site
	local fFertility = Plot_GetFertilityInRange(pPlot, 3);
	if fFertility < 20 then
		return false, false;
	elseif fFertility < 28 then
		return false, true;
	end
	return true, true;
end
------------------------------------------------------------------------------
--- Now unused. Use ExaminePlotForNaturalWondersEligibility instead.
function ASP:ExamineCandidatePlotForNaturalWondersEligibility(x, y)
	return self:ExaminePlotForNaturalWondersEligibility(x, y);
end
------------------------------------------------------------------------------
--- Check a candidate plot for eligibility to host the supplied wonder type.
--- @param x integer
--- @param y integer
--- @param kPlacementInfo Row The corresponding row in Natural_Wonder_Placement table
--- @return boolean
function ASP:CanBeThisNaturalWonderType(x, y, kPlacementInfo)
	local plot = Map.GetPlot(x, y);
	local plotIndex = GetLuaPlotIndexFromCoordinates(x, y);

	-- Check whether adjacent plots are valid
	for direction = 1, 6 do
		local adjPlot = Map.PlotDirection(x, y, direction);
		if not adjPlot then
			return false;
		end
	end

	-- Use Custom Eligibility method if indicated.
	if kPlacementInfo.EligibilityMethodNumber ~= -1 then
		if NWCustomEligibility(x, y, kPlacementInfo.EligibilityMethodNumber) then
			return true;
		end
		return false;
	end

	-- Run root checks.
	if self.bWorldHasOceans then -- Check to see if this wonder requires or avoids the biggest landmass.
		if kPlacementInfo.RequireBiggestLandmass then
			if plot:GetLandmass() ~= self.iBiggestLandmassID then
				return false;
			end
		elseif kPlacementInfo.AvoidBiggestLandmass then
			if plot:GetLandmass() == self.iBiggestLandmassID then
				return false;
			end
		end
	end
	if kPlacementInfo.RequireFreshWater then
		if not plot:IsFreshWater() then
			return false;
		end
	elseif kPlacementInfo.AvoidFreshWater then
		if plot:IsFreshWater() then
			return false;
		end
	end

	-- Land or Sea
	if kPlacementInfo.LandBased then
		if plot:IsWater() then
			return false;
		end
		if kPlacementInfo.RequireLandAdjacentToOcean then
			if not self.plotDataIsCoastal[plotIndex] then
				return false;
			end
		elseif kPlacementInfo.AvoidLandAdjacentToOcean then
			if self.plotDataIsCoastal[plotIndex] then
				return false;
			end
		end
		if kPlacementInfo.RequireLandOnePlotInland then
			if not self.plotDataIsNextToCoast[plotIndex] then
				return false;
			end
		elseif kPlacementInfo.AvoidLandOnePlotInland then
			if self.plotDataIsNextToCoast[plotIndex] then
				return false;
			end
		end
		if kPlacementInfo.RequireLandTwoOrMorePlotsInland then
			if self.plotDataIsCoastal[plotIndex] then
				return false;
			elseif self.plotDataIsNextToCoast[plotIndex] then
				return false;
			end
		elseif kPlacementInfo.AvoidLandTwoOrMorePlotsInland then
			if not self.plotDataIsCoastal[plotIndex] and not self.plotDataIsNextToCoast[plotIndex] then
				return false;
			end
		end
	end

	-- Core Tile
	if not kPlacementInfo.CoreTileCanBeAnyPlotType then
		local ePlot = plot:GetPlotType();
		if ePlot == PlotTypes.PLOT_LAND and kPlacementInfo.CoreTileCanBeFlatland then
			-- Continue
		elseif ePlot == PlotTypes.PLOT_HILLS and kPlacementInfo.CoreTileCanBeHills then
			-- Continue
		elseif ePlot == PlotTypes.PLOT_MOUNTAIN and kPlacementInfo.CoreTileCanBeMountain then
			-- Continue
		elseif ePlot == PlotTypes.PLOT_OCEAN and kPlacementInfo.CoreTileCanBeOcean then
			-- Continue
		else -- Plot type does not match an eligible type, reject this plot.
			return false;
		end
	end
	if not kPlacementInfo.CoreTileCanBeAnyTerrainType then
		local eTerrain = plot:GetTerrainType();
		if eTerrain == TerrainTypes.TERRAIN_GRASS and kPlacementInfo.CoreTileCanBeGrass then
			-- Continue
		elseif eTerrain == TerrainTypes.TERRAIN_PLAINS and kPlacementInfo.CoreTileCanBePlains then
			-- Continue
		elseif eTerrain == TerrainTypes.TERRAIN_DESERT and kPlacementInfo.CoreTileCanBeDesert then
			-- Continue
		elseif eTerrain == TerrainTypes.TERRAIN_TUNDRA and kPlacementInfo.CoreTileCanBeTundra then
			-- Continue
		elseif eTerrain == TerrainTypes.TERRAIN_SNOW and kPlacementInfo.CoreTileCanBeSnow then
			-- Continue
		elseif eTerrain == TerrainTypes.TERRAIN_COAST and kPlacementInfo.CoreTileCanBeShallowWater then
			-- Continue
		elseif eTerrain == TerrainTypes.TERRAIN_OCEAN and kPlacementInfo.CoreTileCanBeDeepWater then
			-- Continue
		else -- Terrain type does not match an eligible type, reject this plot.
			return false;
		end
	end
	if not kPlacementInfo.CoreTileCanBeAnyFeatureType then
		local eFeature = plot:GetFeatureType();
		if eFeature == FeatureTypes.NO_FEATURE and kPlacementInfo.CoreTileCanBeNoFeature then
			-- Continue
		elseif eFeature == FeatureTypes.FEATURE_FOREST and kPlacementInfo.CoreTileCanBeForest then
			-- Continue
		elseif eFeature == FeatureTypes.FEATURE_JUNGLE and kPlacementInfo.CoreTileCanBeJungle then
			-- Continue
		elseif eFeature == FeatureTypes.FEATURE_OASIS and kPlacementInfo.CoreTileCanBeOasis then
			-- Continue
		elseif eFeature == FeatureTypes.FEATURE_FLOOD_PLAINS and kPlacementInfo.CoreTileCanBeFloodPlains then
			-- Continue
		elseif eFeature == FeatureTypes.FEATURE_MARSH and kPlacementInfo.CoreTileCanBeMarsh then
			-- Continue
		elseif eFeature == FeatureTypes.FEATURE_ICE and kPlacementInfo.CoreTileCanBeIce then
			-- Continue
		elseif eFeature == FeatureTypes.FEATURE_ATOLL and kPlacementInfo.CoreTileCanBeAtoll then
			-- Continue
		else -- Feature type does not match an eligible type, reject this plot.
			return false;
		end
	end
	-- Adjacent Tiles: Plot Types
	if kPlacementInfo.AdjacentTilesCareAboutPlotTypes then
		local iNumAnyLand, iNumFlatland, iNumHills, iNumMountain, iNumHillsPlusMountains, iNumOcean = 0, 0, 0, 0, 0, 0;
		for adjPlot in Plot_GetPlotsInCircle(plot, 1) do
			local ePlot = adjPlot:GetPlotType();
			if ePlot == PlotTypes.PLOT_OCEAN then
				iNumOcean = iNumOcean + 1;
			else
				iNumAnyLand = iNumAnyLand + 1;
				if ePlot == PlotTypes.PLOT_LAND then
					iNumFlatland = iNumFlatland + 1;
				else
					iNumHillsPlusMountains = iNumHillsPlusMountains + 1;
					if ePlot == PlotTypes.PLOT_HILLS then
						iNumHills = iNumHills + 1;
					else
						iNumMountain = iNumMountain + 1;
					end
				end
			end
		end
		if iNumAnyLand > 0 and kPlacementInfo.AdjacentTilesAvoidAnyland then
			return false;
		end
		-- Require
		if kPlacementInfo.AdjacentTilesRequireFlatland then
			if iNumFlatland < kPlacementInfo.RequiredNumberOfAdjacentFlatland then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesRequireHills then
			if iNumHills < kPlacementInfo.RequiredNumberOfAdjacentHills then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesRequireMountain then
			if iNumMountain < kPlacementInfo.RequiredNumberOfAdjacentMountain then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesRequireHillsPlusMountains then
			if iNumHillsPlusMountains < kPlacementInfo.RequiredNumberOfAdjacentHillsPlusMountains then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesRequireOcean then
			if iNumOcean < kPlacementInfo.RequiredNumberOfAdjacentOcean then
				return false;
			end
		end
		-- Avoid
		if kPlacementInfo.AdjacentTilesAvoidFlatland then
			if iNumFlatland > kPlacementInfo.MaximumAllowedAdjacentFlatland then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesAvoidHills then
			if iNumHills > kPlacementInfo.MaximumAllowedAdjacentHills then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesAvoidMountain then
			if iNumMountain > kPlacementInfo.MaximumAllowedAdjacentMountain then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesAvoidHillsPlusMountains then
			if iNumHillsPlusMountains > kPlacementInfo.MaximumAllowedAdjacentHillsPlusMountains then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesAvoidOcean then
			if iNumOcean > kPlacementInfo.MaximumAllowedAdjacentOcean then
				return false;
			end
		end
	end

	-- Adjacent Tiles: Terrain Types
	if kPlacementInfo.AdjacentTilesCareAboutTerrainTypes then
		local iNumGrass, iNumPlains, iNumDesert, iNumTundra, iNumSnow, iNumShallowWater, iNumDeepWater = 0, 0, 0, 0, 0, 0, 0;
		for adjPlot in Plot_GetPlotsInCircle(plot, 1) do
			local eTerrain = adjPlot:GetTerrainType();
			if eTerrain == TerrainTypes.TERRAIN_GRASS then
				iNumGrass = iNumGrass + 1;
			elseif eTerrain == TerrainTypes.TERRAIN_PLAINS then
				iNumPlains = iNumPlains + 1;
			elseif eTerrain == TerrainTypes.TERRAIN_DESERT then
				iNumDesert = iNumDesert + 1;
			elseif eTerrain == TerrainTypes.TERRAIN_TUNDRA then
				iNumTundra = iNumTundra + 1;
			elseif eTerrain == TerrainTypes.TERRAIN_SNOW then
				iNumSnow = iNumSnow + 1;
			elseif eTerrain == TerrainTypes.TERRAIN_COAST then
				iNumShallowWater = iNumShallowWater + 1;
			elseif eTerrain == TerrainTypes.TERRAIN_OCEAN then
				iNumDeepWater = iNumDeepWater + 1;
			end
		end
		-- Require
		if kPlacementInfo.AdjacentTilesRequireGrass then
			if iNumGrass < kPlacementInfo.RequiredNumberOfAdjacentGrass then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesRequirePlains then
			if iNumPlains < kPlacementInfo.RequiredNumberOfAdjacentPlains then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesRequireDesert then
			if iNumDesert < kPlacementInfo.RequiredNumberOfAdjacentDesert then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesRequireTundra then
			if iNumTundra < kPlacementInfo.RequiredNumberOfAdjacentTundra then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesRequireSnow then
			if iNumSnow < kPlacementInfo.RequiredNumberOfAdjacentSnow then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesRequireShallowWater then
			if iNumShallowWater < kPlacementInfo.RequiredNumberOfAdjacentShallowWater then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesRequireGrass then
			if iNumDeepWater < kPlacementInfo.RequiredNumberOfAdjacentDeepWater then
				return false;
			end
		end
		-- Avoid
		if kPlacementInfo.AdjacentTilesAvoidGrass then
			if iNumGrass > kPlacementInfo.MaximumAllowedAdjacentGrass then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesAvoidPlains then
			if iNumPlains > kPlacementInfo.MaximumAllowedAdjacentPlains then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesAvoidDesert then
			if iNumDesert > kPlacementInfo.MaximumAllowedAdjacentDesert then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesAvoidTundra then
			if iNumTundra > kPlacementInfo.MaximumAllowedAdjacentTundra then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesAvoidSnow then
			if iNumSnow > kPlacementInfo.MaximumAllowedAdjacentSnow then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesAvoidShallowWater then
			if iNumShallowWater > kPlacementInfo.MaximumAllowedAdjacentShallowWater then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesAvoidDeepWater then
			if iNumDeepWater > kPlacementInfo.MaximumAllowedAdjacentDeepWater then
				return false;
			end
		end
	end

	-- Adjacent Tiles: Feature Types
	if kPlacementInfo.AdjacentTilesCareAboutFeatureTypes then
		local iNumNoFeature, iNumForest, iNumJungle, iNumOasis, iNumFloodPlains, iNumMarsh, iNumIce, iNumAtoll = 0, 0, 0, 0, 0, 0, 0, 0;
		for adjPlot in Plot_GetPlotsInCircle(plot, 1) do
			local eFeature = adjPlot:GetFeatureType();
			if eFeature == FeatureTypes.NO_FEATURE then
				iNumNoFeature = iNumNoFeature + 1;
			elseif eFeature == FeatureTypes.FEATURE_FOREST then
				iNumForest = iNumForest + 1;
			elseif eFeature == FeatureTypes.FEATURE_JUNGLE then
				iNumJungle = iNumJungle + 1;
			elseif eFeature == FeatureTypes.FEATURE_OASIS then
				iNumOasis = iNumOasis + 1;
			elseif eFeature == FeatureTypes.FEATURE_FLOOD_PLAINS then
				iNumFloodPlains = iNumFloodPlains + 1;
			elseif eFeature == FeatureTypes.FEATURE_MARSH then
				iNumMarsh = iNumMarsh + 1;
			elseif eFeature == FeatureTypes.FEATURE_ICE then
				iNumIce = iNumIce + 1;
			elseif eFeature == FeatureTypes.FEATURE_ATOLL then
				iNumAtoll = iNumAtoll + 1;
			end
		end
		-- Require
		if kPlacementInfo.AdjacentTilesRequireNoFeature then
			if iNumNoFeature < kPlacementInfo.RequiredNumberOfAdjacentNoFeature then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesRequireForest then
			if iNumForest < kPlacementInfo.RequiredNumberOfAdjacentForest then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesRequireJungle then
			if iNumJungle < kPlacementInfo.RequiredNumberOfAdjacentJungle then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesRequireOasis then
			if iNumOasis < kPlacementInfo.RequiredNumberOfAdjacentOasis then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesRequireFloodPlains then
			if iNumFloodPlains < kPlacementInfo.RequiredNumberOfAdjacentFloodPlains then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesRequireMarsh then
			if iNumMarsh < kPlacementInfo.RequiredNumberOfAdjacentMarsh then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesRequireIce then
			if iNumIce < kPlacementInfo.RequiredNumberOfAdjacentIce then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesRequireAtoll then
			if iNumAtoll < kPlacementInfo.RequiredNumberOfAdjacentAtoll then
				return false;
			end
		end
		-- Avoid
		if kPlacementInfo.AdjacentTilesAvoidNoFeature then
			if iNumNoFeature > kPlacementInfo.MaximumAllowedAdjacentNoFeature then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesAvoidForest then
			if iNumForest > kPlacementInfo.MaximumAllowedAdjacentForest then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesAvoidJungle then
			if iNumJungle > kPlacementInfo.MaximumAllowedAdjacentJungle then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesAvoidOasis then
			if iNumOasis > kPlacementInfo.MaximumAllowedAdjacentOasis then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesAvoidFloodPlains then
			if iNumFloodPlains > kPlacementInfo.MaximumAllowedAdjacentFloodPlains then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesAvoidMarsh then
			if iNumMarsh > kPlacementInfo.MaximumAllowedAdjacentMarsh then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesAvoidIce then
			if iNumIce > kPlacementInfo.MaximumAllowedAdjacentIce then
				return false;
			end
		end
		if kPlacementInfo.AdjacentTilesAvoidAtoll then
			if iNumAtoll > kPlacementInfo.MaximumAllowedAdjacentAtoll then
				return false;
			end
		end
	end

	-- This plot has survived all tests and is eligible to host this wonder type.
	return true;
end
------------------------------------------------------------------------------
--- Custom mapscripts can override this function to skip Natural Wonders that are not meant to be placed.
--- By default, all Natural Wonders are checked.
--- @return PlotId[][] # The eligibility plot IDs of each Natural Wonder ID
function ASP:GenerateCandidatePlotListsForSpecificNW()
	local iW, iH = Map.GetGridSize();
	local tEligibilityLists = {};
	for i = 0, NUM_NATURAL_WONDERS - 1 do
		tEligibilityLists[i] = {};
	end
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local landEligibility, seaEligibility = self:ExaminePlotForNaturalWondersEligibility(x, y);
			if seaEligibility then
				local iPlotId = GetLuaPlotIndexFromCoordinates(x, y);
				-- Plot has passed checks applicable to all NW types. Move on to specific checks.
				for iRowId, row in GameInfoCache("Natural_Wonder_Placement") do
					-- Skip (and print a warning) if the Natural Wonder isn't found in the Features table
					if not GameInfoTypes[row.NaturalWonderType] then
						print("WARNING:", row.NaturalWonderType, "doesn't exist in Features table!");
					elseif row.NaturalWonderType == "FEATURE_REEF" or
						row.NaturalWonderType == "FEATURE_GIBRALTAR" or
						row.NaturalWonderType == "FEATURE_VOLCANO" or
						landEligibility then
						if self:CanBeThisNaturalWonderType(x, y, row) then
							table.insert(tEligibilityLists[iRowId], iPlotId);
						end
					end
				end
			end
		end
	end
	return tEligibilityLists;
end
------------------------------------------------------------------------------
--- This function scans the map for eligible sites for all "Natural Wonders" Features.
--- @param target_number integer
--- @return NaturalWonderType[] # Ordered list of Natural Wonders to place
--- @return NaturalWonderType[] # Shuffled list of fallback Natural Wonders
--- @return PlotId[][] # Eligibility list of each Natural Wonder
function ASP:GenerateNaturalWondersCandidatePlotLists(target_number)
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

	if NUM_NATURAL_WONDERS == 0 then
		print("-"); print("*** No Natural Wonders found in Natural_Wonder_Placement! ***"); print("-");
		return {}, {}, {};
	end

	-- Cross check whether Features and Natural_Wonder_Placement tables have the same number of NW.
	local iNWCount = 0;
	for _, kFeatureInfo in GameInfoCache("Features") do
		if kFeatureInfo.NaturalWonder or kFeatureInfo.PseudoNaturalWonder then
			iNWCount = iNWCount + 1;
		end
	end
	if iNWCount ~= NUM_NATURAL_WONDERS then
		print("-"); print("*** ERROR: Different numbers of Natural Wonders found in Features and Natural_Wonder_Placement tables! Natural Wonders won't be placed. ***"); print("-");
		return {}, {}, {};
	end

	-- Extracted from this function for easy override since some maps may only place selected NW.
	local tEligibilityLists = self:GenerateCandidatePlotListsForSpecificNW();

	-- Eligibility will affect which NWs can be used, and number of candidates will affect placement order.
	local iCanBeWonder = {};
	for iRowId, row in GameInfoCache("Natural_Wonder_Placement") do
		iCanBeWonder[iRowId] = #tEligibilityLists[iRowId];
		print(string.format("Wonder #%s %s has %d candidate plots.", iRowId, row.NaturalWonderType, iCanBeWonder[iRowId]));
	end

	-- Randomly pick NWs from the list based on occurrence
	print("-");
	print("List of OccurrenceFrequency values for each Natural Wonder in the database:");
	local NW_frequency = {};
	local isNWPicked = table.fill(false, NUM_NATURAL_WONDERS);
	local chosen_NW, fallback_NW = {}, {};
	for iRowId, row in GameInfoCache("Natural_Wonder_Placement") do
		NW_frequency[iRowId] = row.OccurrenceFrequency;
		print(string.format("Occurrence for wonder #%s %s is: %d", iRowId, row.NaturalWonderType, NW_frequency[iRowId]));
	end

	-- print("-");
	-- print("List of picked Natural Wonders and their chances:");
	for _ = 1, target_number do
		local occurrence_threshold = {};
		local iOccurrenceSum = 0;

		-- Populate threshold table
		for iRowId, row in GameInfoCache("Natural_Wonder_Placement") do
			local iFrequency = 0;
			if not isNWPicked[iRowId] and iCanBeWonder[iRowId] > 0 then
				iFrequency = row.OccurrenceFrequency;
			end
			iOccurrenceSum = iOccurrenceSum + iFrequency;
			occurrence_threshold[iRowId] = iOccurrenceSum;
			-- print(iRowId, iOccurrenceSum);
		end

		-- End early if no NW left to pick
		if iOccurrenceSum == 0 then
			break;
		end

		-- Pick the NW
		local diceroll = Map.Rand(iOccurrenceSum, "Picking NW");
		for loop = 0, NUM_NATURAL_WONDERS - 1 do
			if diceroll < occurrence_threshold[loop] then
				-- print("Picked:", loop, "diceroll = ", diceroll);
				isNWPicked[loop] = true;
				break;
			end
		end
	end

	print("-");
	print("List of chosen and fallback NWs:");
	for loop = 0, NUM_NATURAL_WONDERS - 1 do
		if isNWPicked[loop] then
			table.insert(chosen_NW, loop);
			-- print("Chosen:", loop);
		elseif iCanBeWonder[loop] > 0 and NW_frequency[loop] > 0 then
			table.insert(fallback_NW, loop);
			-- print("Fallback:", loop);
		end
	end

	-- Sort the chosen NWs with fewest candidates first
	table.sort(chosen_NW, function (a, b) return iCanBeWonder[a] < iCanBeWonder[b] end);

	-- Shuffle the fallback NWs
	local shuffled_fallback_NW = GetShuffledCopyOfTable(fallback_NW);

	return chosen_NW, shuffled_fallback_NW, tEligibilityLists;
end
------------------------------------------------------------------------------
--- Attempt to place a specific natural wonder
--- @param kPlacementInfo Row The corresponding row in Natural_Wonder_Placement table
--- @param tEligibilityList PlotId[]
function ASP:AttemptToPlaceNaturalWonder(kPlacementInfo, tEligibilityList)
	local candidate_plot_list = GetShuffledCopyOfTable(tEligibilityList);
	for _, plotIndex in ipairs(candidate_plot_list) do
		if self.impactData[ImpactLayers.LAYER_NATURAL_WONDER][plotIndex] == 0 then
			-- No collision with civ start or other NW, so place wonder here!
			local x, y = GetCoordinatesFromLuaPlotIndex(plotIndex);
			local plot = Map.GetPlot(x, y);

			print("- Placing ".. kPlacementInfo.NaturalWonderType .. " in Plot", x, y);

			-- If called for, force the local terrain to conform to what the wonder needs.
			local method_number = kPlacementInfo.TileChangesMethodNumber;
			if method_number ~= -1 then
				-- Custom method for tile changes needed by this wonder.
				NWCustomPlacement(x, y, kPlacementInfo.ID, method_number);
			else
				-- Check the placement data for any standard type tile changes, execute any that are indicated.
				if kPlacementInfo.ChangeCoreTileToMountain then
					plot:SetPlotType(PlotTypes.PLOT_MOUNTAIN, false, false);
				elseif kPlacementInfo.ChangeCoreTileToFlatland then
					plot:SetPlotType(PlotTypes.PLOT_LAND, false, false);
				end
				if kPlacementInfo.ChangeCoreTileTerrainToGrass then
					plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, false);
				elseif kPlacementInfo.ChangeCoreTileTerrainToPlains then
					plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, false);
				end
				if kPlacementInfo.SetAdjacentTilesToShallowWater then
					for pAdjPlot in Plot_GetPlotsInCircle(plot, 1) do
						pAdjPlot:SetTerrainType(TerrainTypes.TERRAIN_COAST, false, false);
					end
				end
			end

			-- Now place this wonder and record the placement.
			plot:SetFeatureType(GameInfoTypes[kPlacementInfo.NaturalWonderType]);

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

			-- Record exact plot of wonder in the collision list.
			self.playerCollisionData[plotIndex] = true;

			-- Apply impact values to Great Barrier Reef's second tile to avoid buggy collisions with water resources.
			if kPlacementInfo.NaturalWonderType == "FEATURE_REEF" then
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
				local SEplotIndex = GetLuaPlotIndexFromCoordinates(southeastX, southeastY);
				self.playerCollisionData[SEplotIndex] = true;
			end

			print("- Placed ".. kPlacementInfo.NaturalWonderType .. " in Plot", x, y);

			return true;
		end
	end

	-- If reached here, this wonder was unable to be placed because all candidates are too close to an already-placed NW.
	return false;
end
------------------------------------------------------------------------------
--- Determine how many NWs to attempt to place. Target is regulated per map size.
--- The final number cannot exceed the number the map has locations to support.
--- @param iWorldSize integer
--- @return integer
function ASP:GetNumNaturalWondersToPlace(iWorldSize)
	local tTarget = {
		[GameInfoTypes.WORLDSIZE_DUEL] = 2,
		[GameInfoTypes.WORLDSIZE_TINY] = 3,
		[GameInfoTypes.WORLDSIZE_SMALL] = 4,
		[GameInfoTypes.WORLDSIZE_STANDARD] = 5,
		[GameInfoTypes.WORLDSIZE_LARGE] = 6,
		[GameInfoTypes.WORLDSIZE_HUGE] = 7,
	};
	return tTarget[iWorldSize];
end
------------------------------------------------------------------------------
function ASP:PlaceNaturalWonders()
	local target_number = self:GetNumNaturalWondersToPlace(Map.GetWorldSize());

	local selected_NWs, fallback_NWs, tEligibilityLists = self:GenerateNaturalWondersCandidatePlotLists(target_number);
	local iNumNWCandidates = #selected_NWs + #fallback_NWs;
	if iNumNWCandidates == 0 then
		print("No Natural Wonders placed, no eligible sites found for any of them.");
		return;
	end

	-- Debug printout
	--[[
	print("-"); print("--- Readout of NW Assignment Priority ---");
	for print_loop, order in ipairs(selected_NWs) do
		print("NW Assignment Priority#", print_loop, "goes to NW#", order);
	end
	print("-"); print("-");
	--]]

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
	local iNumNWtoPlace = math.min(target_number, iNumNWCandidates);
	local iNumPlaced = 0;
	for _, iRowId in ipairs(selected_NWs) do
		if self:AttemptToPlaceNaturalWonder(GameInfo.Natural_Wonder_Placement[iRowId], tEligibilityLists[iRowId]) then
			iNumPlaced = iNumPlaced + 1;
		end
	end

	-- Place fallback wonders
	if iNumPlaced < iNumNWtoPlace then
		for _, iRowId in ipairs(fallback_NWs) do
			if self:AttemptToPlaceNaturalWonder(GameInfo.Natural_Wonder_Placement[iRowId], tEligibilityLists[iRowId]) then
				iNumPlaced = iNumPlaced + 1;
				if iNumPlaced >= iNumNWtoPlace then
					break;
				end
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

--- Extracted from AssignCityStatesToRegionsOrToUninhabited() for easy override
--- @return integer
function ASP:GetNumCityStatesPerRegion()
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
function ASP:GetNumCityStatesInUninhabitedRegion(iNumCivLandmassPlots, iNumUninhabitedLandmassPlots)
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
function ASP:AssignCityStatesToRegions(current_cs_index)
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
function ASP:AssignCityStatesToRegionsOrToUninhabited()
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
				local ePlot = plot:GetPlotType();
				local iArea = plot:GetArea();
				if not self.bArea then
					iArea = plot:GetLandmass();
				end
				if (ePlot == PlotTypes.PLOT_LAND or ePlot == PlotTypes.PLOT_HILLS) and self:CanPlaceCityStateAt(x, y, iArea, false, false) then -- Habitable land plot, process it.
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
						if not land_area_plot_count[iArea] then -- This plot is the first detected in its area/landmass.
							iNumLandAreas = iNumLandAreas + 1;
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
						local x, y = GetCoordinatesFromLuaPlotIndex(plotIndex);
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
function ASP:CanPlaceCityStateAt(x, y, area_ID, force_it, ignore_collisions)
	local iW, iH = Map.GetGridSize();
	local plot = Map.GetPlot(x, y);
	if self.bArea and area_ID ~= -1 and area_ID ~= plot:GetArea() then
		return false;
	end

	if not self.bArea and area_ID ~= -1 and area_ID ~= plot:GetLandmass() then
		return false;
	end

	if plot:IsWater() or plot:IsMountain() then
		return false;
	end

	-- Must not be on map borders
	if (not Map.IsWrapX() and (x < 1 or x >= iW - 1)) or (not Map.IsWrapY() and (y < 1 or y >= iH - 1)) then
		return false;
	end

	-- Avoid ice
	for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
		if nearPlot:GetFeatureType() == FeatureTypes.FEATURE_ICE then
			return false;
		end
	end

	-- Reserve the best city sites for major civs
	local fertility = Plot_GetFertilityInRange(plot, 2);
	if fertility > 30 then
		-- print("CanPlaceCityStateAt: avoided fertility: ", fertility);
		return false;
	end

	local iPlotIndex = GetLuaPlotIndexFromCoordinates(x, y);
	if self.impactData[ImpactLayers.LAYER_CITY_STATE][iPlotIndex] > 0 and not force_it then
		return false;
	end

	if self.playerCollisionData[iPlotIndex] and not ignore_collisions then
		print("-"); print("City State candidate plot rejected: collided with already-placed civ or City State at", x, y);
		return false;
	end
	return true;
end
------------------------------------------------------------------------------
--- This function carves off the outermost plots in a region, checks them for City State Placement eligibility,
--- and returns 7 variables: two plot lists, the coordinates of the inner portion of the landmass/area that was not processed on this round,
--- and a boolean indicating whether the middle of the region was reached.
---
--- If this round does not produce a suitable placement site, another round can be executed on the remaining unprocessed plots, recursively,
--- until the middle of the region has been reached.<br>
--- If the entire region has no eligible plots, then it is likely that something extreme is going on with the map.
--- Choose a plot from the outermost portion of the region at random and hope for the best.
--- @param incoming_west_x integer
--- @param incoming_south_y integer
--- @param incoming_width integer
--- @param incoming_height integer
--- @param iAreaID integer
--- @param force_it boolean
--- @param ignore_collisions boolean
--- @return PlotId[], PlotId[], integer, integer, integer, integer, boolean
function ASP:ObtainNextSectionInRegion(incoming_west_x, incoming_south_y, incoming_width, incoming_height, iAreaID, force_it, ignore_collisions)
	-- print("ObtainNextSectionInRegion called, for Area/Landmass#", iAreaID, "with SW plot at ", incoming_west_x, incoming_south_y, " Width/Height at", incoming_width, incoming_height);

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
					local i = GetLuaPlotIndexFromCoordinates(x, y);
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
							local i = GetLuaPlotIndexFromCoordinates(x, y);
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
							local i = GetLuaPlotIndexFromCoordinates(x, y);
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
-- Return coords, plus boolean indicating whether assignment succeeded or failed.
-- Argument "check_collision" should be false if plots in lists were already checked, true if not.
function ASP:PlaceCityState(coastal_plot_list, inland_plot_list, check_proximity, check_collision)
	if not coastal_plot_list or not inland_plot_list then
		print("Nil plot list incoming for PlaceCityState()");
	end

	for _, tPlotList in pairs{coastal_plot_list, inland_plot_list} do
		if next(tPlotList) then
			if not check_collision then
				local selected_plot_index = GetRandomFromList(tPlotList, "Standard City State placement - LUA");
				local x, y = GetCoordinatesFromLuaPlotIndex(selected_plot_index);
				return x, y, true;
			else
				local tRandomizedList = GetShuffledCopyOfTable(tPlotList);
				for _, candidate_plot in pairs(tRandomizedList) do
					if not self.playerCollisionData[candidate_plot] then
						if not check_proximity or self.impactData[ImpactLayers.LAYER_CITY_STATE][candidate_plot] == 0 then
							local x, y = GetCoordinatesFromLuaPlotIndex(candidate_plot);
							return x, y, true;
						end
					end
				end
			end
		end
	end

	return 0, 0, false;
end
------------------------------------------------------------------------------
function ASP:PlaceCityStateInRegion(city_state_number, region_number)
	print("Place City State in Region called for City State", city_state_number, "Region", region_number);
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
		self:GenerateLuxuryPlotListsAtCitySite(x, y, 1, true); -- Removes Feature Ice from coasts adjacent to the city state's new location
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
		local impactPlotIndex = GetLuaPlotIndexFromCoordinates(x, y);
		self.playerCollisionData[impactPlotIndex] = true;
		print("-"); print("City State", city_state_number, "has been started at Plot", x, y, "in Region#", region_number);
	else
		print("-"); print("WARNING: Crowding issues for City State #", city_state_number, " - Could not find valid site in Region#", region_number);
		self.iNumCityStatesDiscarded = self.iNumCityStatesDiscarded + 1;
	end
end
------------------------------------------------------------------------------
function ASP:PlaceCityStates()
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

				if success then
					self.cityStatePlots[cs_number] = {cs_x, cs_y, -1};
					self.city_state_validity_table[cs_number] = true; -- This is the line that marks a city state as valid to be processed by the rest of the system.
					local city_state_ID = cs_number + MAX_MAJOR_CIVS - 1;
					local cityState = Players[city_state_ID];
					local cs_start_plot = Map.GetPlot(cs_x, cs_y);
					cityState:SetStartingPlot(cs_start_plot);
					self:GenerateLuxuryPlotListsAtCitySite(cs_x, cs_y, 1, true); -- Removes Feature Ice from coasts adjacent to the city state's new location
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
					local i = GetLuaPlotIndexFromCoordinates(x, y);
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
					self:GenerateLuxuryPlotListsAtCitySite(cs_x, cs_y, 1, true); -- Removes Feature Ice from coasts adjacent to the city state's new location
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
					local impactPlotIndex = GetLuaPlotIndexFromCoordinates(cs_x, cs_y);
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
--- Similar to the version for normalizing civ starts, but less placed, no third-ring considerations and different weightings.
--- @param x integer
--- @param y integer
function ASP:NormalizeCityState(x, y)
	local pPlot = Map.GetPlot(x, y);
	local tCount = self:ScanPlotsInTwoRings(pPlot);

	-- Generate plot lists of each of the two rings around start.
	local tRingPlots = {{}, {}};
	for i = 1, 2 do
		for pLoopPlot in Plot_GetPlotsInCircle(pPlot, i, i) do
			table.insert(tRingPlots[i], pLoopPlot);
		end
	end

	-- Adjust the hammer situation, if needed.
	local iProdScore = 4 * tCount[1].Prod.Three + 2 * tCount[1].Prod.Two + tCount[1].Prod.One;

	-- If drastic shortage, attempt to add a hill to first ring.
	if iProdScore < 4 then
		local bPlaced = false;
		for _, pLoopPlot in pairs(GetShuffledCopyOfTable(tRingPlots[1])) do
			-- Attempt to place a Hill at the currently chosen plot.
			local iLoopX = pLoopPlot:GetX();
			local iLoopY = pLoopPlot:GetY();
			bPlaced = self:AttemptToPlaceHillsAtPlot(iLoopX, iLoopY);
			if bPlaced then
				iProdScore = iProdScore + 4;
				print("Added hills next to hammer-poor city state", x, y, "at", iLoopX, iLoopY);
				break;
			end
		end
		if not bPlaced then
			print("FAILED to add hills next to hammer-poor city state", x, y);
		end
	end

	-- Rate the food situation.
	local iFoodScore1 = 6 * tCount[1].Food.Five + 4 * tCount[1].Food.Four + 2 * tCount[1].Food.Three + tCount[1].Food.Two;
	local iFoodScore2 = 6 * tCount[2].Food.Five + 4 * tCount[2].Food.Four + 2 * tCount[2].Food.Three + tCount[2].Food.Two;
	local iTotalFoodScore = iFoodScore1 + iFoodScore2;

	-- Three levels for Bonus Resource support, from zero to two.
	local iNumFoodBonusNeeded = 0;
	if iTotalFoodScore < 12 or iFoodScore1 < 4 then
		iNumFoodBonusNeeded = 2;
	elseif iTotalFoodScore < 16 and iFoodScore1 < 9 then
		iNumFoodBonusNeeded = 1;
	end

	-- Add Bonus Resources to food-poor city states.
	if iNumFoodBonusNeeded > 0 then
		print("-");
		print("Food-Poor city state", x, y, "needs", iNumFoodBonusNeeded, "bonus resources.");
		print("-");

		local bPlacedOasis = false;

		-- Try each ring in order.
		local iPlacedBonus = 0;
		for i = 1, 2 do
			if iPlacedBonus >= iNumFoodBonusNeeded then
				break;
			end
			for _, pLoopPlot in pairs(GetShuffledCopyOfTable(tRingPlots[i])) do
				local iLoopX, iLoopY = pLoopPlot:GetX(), pLoopPlot:GetY();
				local bPlacedBonus = false;
				-- Only place one oasis at most.
				bPlacedBonus, bPlacedOasis = self:AttemptToPlaceBonusResourceAtPlot(iLoopX, iLoopY, not bPlacedOasis);
				if bPlacedBonus then
					print("Placed a Bonus/Oasis in ring #", i, "at", iLoopX, iLoopY);
					iPlacedBonus = iPlacedBonus + 1;
					if iPlacedBonus >= iNumFoodBonusNeeded then
						break;
					end
				end
			end
		end
	end
end
------------------------------------------------------------------------------
--- Dependent on PlaceLuxuries being executed first.
function ASP:NormalizeCityStateLocations()
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

function ASP:GenerateResourcePlotListsFromSpecificPlots(plotList)
	-- This function generates resource plot lists out of a given list of plots.
	-- This is called by GenerateGlobalResourcePlotLists, GenerateLuxuryPlotListsAtCitySite and GenerateLuxuryPlotListsInRegion.

	if not plotList then
		return {};
	end

	local tResourceList = {};
	for _, v in pairs(PlotListTypes) do
		tResourceList[v] = {};
	end

	for _, plotIndex in ipairs(plotList) do
		local x, y = GetCoordinatesFromLuaPlotIndex(plotIndex);
		local plot = Map.GetPlot(x, y);
		local ePlot = plot:GetPlotType();
		local eTerrain = plot:GetTerrainType();
		local eFeature = plot:GetFeatureType();
		if ePlot == PlotTypes.PLOT_MOUNTAIN then
			table.insert(tResourceList[PlotListTypes.MOUNTAIN], plotIndex);
		elseif ePlot == PlotTypes.PLOT_OCEAN then
			if eFeature ~= FeatureTypes.FEATURE_ATOLL then
				if eFeature == FeatureTypes.FEATURE_ICE then
					table.insert(tResourceList[PlotListTypes.ICE], plotIndex);
				elseif plot:IsLake() then
					table.insert(tResourceList[PlotListTypes.LAKE], plotIndex);
				elseif eTerrain == TerrainTypes.TERRAIN_COAST then
					table.insert(tResourceList[PlotListTypes.COAST], plotIndex);
					if plot:IsAdjacentToLand() then
						table.insert(tResourceList[PlotListTypes.COAST_NEXT_TO_LAND], plotIndex);
					end
				else
					table.insert(tResourceList[PlotListTypes.OCEAN], plotIndex);
				end
			end
		elseif ePlot == PlotTypes.PLOT_HILLS then
			table.insert(tResourceList[PlotListTypes.HILLS], plotIndex);
			table.insert(tResourceList[PlotListTypes.LAND], plotIndex);
			if eFeature == FeatureTypes.NO_FEATURE then
				if eTerrain ~= TerrainTypes.TERRAIN_SNOW then
					table.insert(tResourceList[PlotListTypes.MARBLE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_NO_FEATURE], plotIndex);
				end
				if eTerrain == TerrainTypes.TERRAIN_TUNDRA then
					table.insert(tResourceList[PlotListTypes.HILLS_TUNDRA_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_TUNDRA_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_TUNDRA_PLAINS_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_TUNDRA_PLAINS_NO_FEATURE], plotIndex);
				elseif eTerrain == TerrainTypes.TERRAIN_DESERT then
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_PLAINS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_TUNDRA_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_PLAINS_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_TUNDRA_PLAINS_NO_FEATURE], plotIndex);
				elseif eTerrain == TerrainTypes.TERRAIN_PLAINS then
					table.insert(tResourceList[PlotListTypes.HILLS_PLAINS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_PLAINS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_PLAINS_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_TUNDRA_PLAINS_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_TUNDRA_PLAINS_NO_FEATURE], plotIndex);
				elseif eTerrain == TerrainTypes.TERRAIN_GRASS then
					table.insert(tResourceList[PlotListTypes.HILLS_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_DESERT_PLAINS_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_TUNDRA_PLAINS_GRASS_NO_FEATURE], plotIndex);
				elseif eTerrain == TerrainTypes.TERRAIN_SNOW then
					table.insert(tResourceList[PlotListTypes.HILLS_SNOW], plotIndex);
				else
					-- Unknown terrain!
					table.insert(tResourceList[PlotListTypes.UNKNOWN], plotIndex);
					table.remove(tResourceList[PlotListTypes.HILLS]);
					table.remove(tResourceList[PlotListTypes.LAND]);
				end
			elseif eFeature == FeatureTypes.FEATURE_JUNGLE then
				table.insert(tResourceList[PlotListTypes.MARBLE], plotIndex);
				table.insert(tResourceList[PlotListTypes.HILLS_JUNGLE], plotIndex);
				table.insert(tResourceList[PlotListTypes.HILLS_COVERED], plotIndex);
				if eTerrain == TerrainTypes.TERRAIN_PLAINS then
					table.insert(tResourceList[PlotListTypes.HILLS_PLAINS_COVERED], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_PLAINS_GRASS_COVERED], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_TUNDRA_PLAINS_COVERED], plotIndex);
				elseif eTerrain == TerrainTypes.TERRAIN_GRASS then
					table.insert(tResourceList[PlotListTypes.HILLS_PLAINS_GRASS_COVERED], plotIndex);
				end
			elseif eFeature == FeatureTypes.FEATURE_FOREST then
				table.insert(tResourceList[PlotListTypes.MARBLE], plotIndex);
				table.insert(tResourceList[PlotListTypes.HILLS_FOREST], plotIndex);
				table.insert(tResourceList[PlotListTypes.HILLS_COVERED], plotIndex);
				if eTerrain == TerrainTypes.TERRAIN_TUNDRA then
					table.insert(tResourceList[PlotListTypes.TUNDRA_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_TUNDRA_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_TUNDRA_PLAINS_COVERED], plotIndex);
				elseif eTerrain == TerrainTypes.TERRAIN_PLAINS then
					table.insert(tResourceList[PlotListTypes.PLAINS_GRASS_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_PLAINS_COVERED], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_PLAINS_GRASS_COVERED], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_TUNDRA_PLAINS_COVERED], plotIndex);
				elseif eTerrain == TerrainTypes.TERRAIN_GRASS then
					table.insert(tResourceList[PlotListTypes.PLAINS_GRASS_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.HILLS_PLAINS_GRASS_COVERED], plotIndex);
				end
			else
				-- Unknown feature!
				table.insert(tResourceList[PlotListTypes.UNKNOWN], plotIndex);
				table.remove(tResourceList[PlotListTypes.HILLS]);
				table.remove(tResourceList[PlotListTypes.LAND]);
			end
		elseif eFeature == FeatureTypes.FEATURE_MARSH then
			table.insert(tResourceList[PlotListTypes.FLAT], plotIndex);
			table.insert(tResourceList[PlotListTypes.LAND], plotIndex);
			table.insert(tResourceList[PlotListTypes.MARSH], plotIndex);
			if IsTropical(y) then
				table.insert(tResourceList[PlotListTypes.TROPICAL_MARSH], plotIndex);
				table.insert(tResourceList[PlotListTypes.RICE], plotIndex);
			end
		elseif eFeature == FeatureTypes.FEATURE_FLOOD_PLAINS then
			table.insert(tResourceList[PlotListTypes.FLAT], plotIndex);
			table.insert(tResourceList[PlotListTypes.LAND], plotIndex);
			table.insert(tResourceList[PlotListTypes.FLOOD_PLAINS], plotIndex);
			table.insert(tResourceList[PlotListTypes.FLAT_DESERT_WET], plotIndex);
			table.insert(tResourceList[PlotListTypes.FLAT_DESERT_NO_OASIS], plotIndex);
			if IsTropical(y) then
				table.insert(tResourceList[PlotListTypes.MAIZE], plotIndex);
			else
				table.insert(tResourceList[PlotListTypes.WHEAT], plotIndex);
			end
		elseif ePlot == PlotTypes.PLOT_LAND then
			table.insert(tResourceList[PlotListTypes.FLAT], plotIndex);
			table.insert(tResourceList[PlotListTypes.LAND], plotIndex);
			if eFeature == FeatureTypes.FEATURE_JUNGLE then
				table.insert(tResourceList[PlotListTypes.FLAT_JUNGLE], plotIndex);
				table.insert(tResourceList[PlotListTypes.FLAT_COVERED], plotIndex);
				if eTerrain == TerrainTypes.TERRAIN_PLAINS then
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_COVERED], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_GRASS_COVERED], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_TUNDRA_PLAINS_COVERED], plotIndex);
					if plot:IsCoastalLand() then
						table.insert(tResourceList[PlotListTypes.COCONUT], plotIndex);
					end
				elseif eTerrain == TerrainTypes.TERRAIN_GRASS then
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_GRASS_COVERED], plotIndex);
					if plot:IsCoastalLand() then
						table.insert(tResourceList[PlotListTypes.COCONUT], plotIndex);
					end
				end
			elseif eFeature == FeatureTypes.FEATURE_FOREST then
				table.insert(tResourceList[PlotListTypes.FLAT_FOREST], plotIndex);
				table.insert(tResourceList[PlotListTypes.FLAT_COVERED], plotIndex);
				if eTerrain == TerrainTypes.TERRAIN_TUNDRA then
					table.insert(tResourceList[PlotListTypes.FLAT_TUNDRA], plotIndex);
					table.insert(tResourceList[PlotListTypes.TUNDRA_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_TUNDRA_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_TUNDRA_PLAINS_COVERED], plotIndex);
				elseif eTerrain == TerrainTypes.TERRAIN_PLAINS then
					table.insert(tResourceList[PlotListTypes.PLAINS_GRASS_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_COVERED], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_GRASS_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_GRASS_COVERED], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_TUNDRA_PLAINS_COVERED], plotIndex);
				elseif eTerrain == TerrainTypes.TERRAIN_GRASS then
					table.insert(tResourceList[PlotListTypes.PLAINS_GRASS_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_GRASS_FOREST], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_GRASS_COVERED], plotIndex);
				end
			elseif eFeature == FeatureTypes.NO_FEATURE then
				if eTerrain == TerrainTypes.TERRAIN_SNOW then
					table.insert(tResourceList[PlotListTypes.FLAT_SNOW], plotIndex);
				elseif eTerrain == TerrainTypes.TERRAIN_TUNDRA then
					table.insert(tResourceList[PlotListTypes.MARBLE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_TUNDRA], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_TUNDRA_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_TUNDRA_PLAINS_GRASS_NO_FEATURE], plotIndex);
				elseif eTerrain == TerrainTypes.TERRAIN_DESERT then
					table.insert(tResourceList[PlotListTypes.MARBLE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_DESERT_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_DESERT_NO_OASIS], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE], plotIndex);
					if plot:IsFreshWater() then
						table.insert(tResourceList[PlotListTypes.FLAT_DESERT_WET], plotIndex);
						if IsTropical(y) then
							table.insert(tResourceList[PlotListTypes.MAIZE], plotIndex);
						else
							table.insert(tResourceList[PlotListTypes.WHEAT], plotIndex);
						end
					end
					if plot:IsCoastalLand() then
						if IsTropical(y) then
							table.insert(tResourceList[PlotListTypes.COCONUT], plotIndex);
						end
					end
				elseif eTerrain == TerrainTypes.TERRAIN_PLAINS then
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
					if IsTropical(y) then
						if plot:IsCoastalLand() then
							table.insert(tResourceList[PlotListTypes.COCONUT], plotIndex);
						end
					else
						table.insert(tResourceList[PlotListTypes.WHEAT], plotIndex);
					end
				elseif eTerrain == TerrainTypes.TERRAIN_GRASS then
					table.insert(tResourceList[PlotListTypes.MARBLE], plotIndex);
					table.insert(tResourceList[PlotListTypes.MAIZE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE], plotIndex);
					table.insert(tResourceList[PlotListTypes.FLAT_TUNDRA_PLAINS_GRASS_NO_FEATURE], plotIndex);
					if IsTropical(y) then
						table.insert(tResourceList[PlotListTypes.RICE], plotIndex);
					end
					if plot:IsFreshWater() then
						table.insert(tResourceList[PlotListTypes.FLAT_GRASS_WET_NO_FEATURE], plotIndex);
					else
						table.insert(tResourceList[PlotListTypes.FLAT_GRASS_DRY_NO_FEATURE], plotIndex);
					end
					if plot:IsCoastalLand() then
						if IsTropical(y) then
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
function ASP:GenerateGlobalResourcePlotLists()
	-- This function generates all global plot lists needed for resource distribution.
	local plotList = {};

	for i, plot in Plots() do
		local iPlotIndex = i + 1;
		-- Check if plot has a civ start, CS start, or Natural Wonder
		if self.playerCollisionData[iPlotIndex] then
			-- Do not process this plot!
		elseif plot:GetResourceType() ~= -1 then
			-- Plot has a resource already, do not include it.
		else
			table.insert(plotList, iPlotIndex);
		end
	end

	-- Set up the Global Resource Plot Lists
	self.global_resource_plot_lists = self:GenerateResourcePlotListsFromSpecificPlots(plotList);

	-- print("Wheat list size:", table.maxn(self.global_resource_plot_lists[PlotListTypes.WHEAT]));
	-- print("Rice list size:", table.maxn(self.global_resource_plot_lists[PlotListTypes.RICE]));
	-- print("Maize list size:", table.maxn(self.global_resource_plot_lists[PlotListTypes.MAIZE]));
end
------------------------------------------------------------------------------
function ASP:PlaceStrategicResourceImpact(x, y, radius)
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_IRON, radius);
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_HORSE, radius);
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_COAL, radius);
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_OIL, radius);
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_ALUMINUM, radius);
	self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_URANIUM, radius);
end
------------------------------------------------------------------------------
function ASP:CountTotalStrategicResourceImpact(plotIndex)
	return self.impactData[ImpactLayers.LAYER_IRON][plotIndex] +
		self.impactData[ImpactLayers.LAYER_HORSE][plotIndex] +
		self.impactData[ImpactLayers.LAYER_COAL][plotIndex] +
		self.impactData[ImpactLayers.LAYER_OIL][plotIndex] +
		self.impactData[ImpactLayers.LAYER_ALUMINUM][plotIndex] +
		self.impactData[ImpactLayers.LAYER_URANIUM][plotIndex];
end
------------------------------------------------------------------------------
function ASP:IsImpactLayerStrategic(layerIndex)
	return layerIndex == ImpactLayers.LAYER_IRON or
		layerIndex == ImpactLayers.LAYER_HORSE or
		layerIndex == ImpactLayers.LAYER_COAL or
		layerIndex == ImpactLayers.LAYER_OIL or
		layerIndex == ImpactLayers.LAYER_ALUMINUM or
		layerIndex == ImpactLayers.LAYER_URANIUM;
end
------------------------------------------------------------------------------
--- Called from PlaceImpactAndRipples<br>
--- This function operates upon one of the "impact and ripple" data overlays for resources.<br>
--- These data layers are a primary way of preventing assignments from clustering too much.
--- @param x integer
--- @param y integer
--- @param impact_table_number ImpactLayer
--- @param radius integer
function ASP:PlaceResourceImpact(x, y, impact_table_number, radius)
	local impact_value = 99;
	local _, iMapH = Map.GetGridSize();

	-- Place Impact
	local impactPlotIndex = GetLuaPlotIndexFromCoordinates(x, y);
	for i = 1, NUM_IMPACT_LAYERS do
		self.impactData[i][impactPlotIndex] = impact_value;
	end
	if radius == 0 then
		return;
	end

	-- Place Ripples
	if radius > 0 and radius < iMapH / 2 then
		for iLoopX, iLoopY, iDistance in Plot_GetPlotCoordinatesInCircle(Map.GetPlot(x, y), radius) do
			local iRippleValue = radius - iDistance + 1;
			local iPlotIndex = GetLuaPlotIndexFromCoordinates(iLoopX, iLoopY);
			if impact_table_number == ImpactLayers.LAYER_CITY_STATE or
				impact_table_number == ImpactLayers.LAYER_MARBLE or
				impact_table_number == ImpactLayers.LAYER_IVORY then
				self.impactData[impact_table_number][iPlotIndex] = 1;
			else
				if self.impactData[impact_table_number][iPlotIndex] > 0 then
					-- First choose the greater of the two, existing value or current ripple.
					local iStrongerValue = math.max(self.impactData[impact_table_number][iPlotIndex], iRippleValue);
					-- Now increase it by 2 to reflect that multiple impact centers are in range of this plot.
					self.impactData[impact_table_number][iPlotIndex] = math.min(99, iStrongerValue + 2);
				else
					self.impactData[impact_table_number][iPlotIndex] = iRippleValue;
				end
			end
		end
	else
		print("Unsupported Radius length of ", radius, " passed to PlaceResourceImpact()");
	end
end
------------------------------------------------------------------------------
--- Extracted from ProcessResourceList()
--- @param resourceToPlace ResourceToPlace
--- @param iPlotId PlotId
--- @param eImpact ImpactLayer
function ASP:TryPlacingOneResource(resourceToPlace, iPlotId, eImpact)
	local x, y = GetCoordinatesFromLuaPlotIndex(iPlotId);
	local pPlot = Map.GetPlot(x, y);

	-- Plot cannot already have a resource
	if pPlot:GetResourceType() == -1 then
		local iRadius = GetRandomFromRangeInclusive(resourceToPlace.MinRadius, resourceToPlace.MaxRadius);
		local iQuantity = resourceToPlace.Quantity;
		local eResourceUsage = Game.GetResourceUsageType(resourceToPlace.ID);
		-- Give some variance to strategic amounts
		if eResourceUsage == ResourceUsageTypes.RESOURCEUSAGE_STRATEGIC then
			local iRand = Map.Rand(4, "ProcessResourceList - Lua");
			if iRand == 3 then
				iQuantity = iQuantity * 1.2;
			elseif iRand == 0 then
				iQuantity = iQuantity * 0.8;
			end
			iQuantity = math.floor(iQuantity + 0.5);
		elseif eResourceUsage == ResourceUsageTypes.RESOURCEUSAGE_LUXURY then
			self.totalLuxPlacedSoFar = self.totalLuxPlacedSoFar + 1;
		end
		pPlot:SetResourceType(resourceToPlace.ID, iQuantity);
		self:PlaceResourceImpact(x, y, eImpact, iRadius);
		self.amounts_of_resources_placed[resourceToPlace.ID + 1] = self.amounts_of_resources_placed[resourceToPlace.ID + 1] + iQuantity;
		return true;
	end

	return false;
end
------------------------------------------------------------------------------
--- Add a random factor to strategic resources
---
--- This function needs to receive two numbers and two tables.<br>
--- Length of the plotlist is divided by frequency to get the number of resources to place.<br>
--- The first table is a list of plot indices.<br>
--- The second table contains subtables, one per resource type, detailing the resource ID number, quantity, weighting, and impact radius of each applicable resource.<br>
--- If radius min and max are different, the radius length is variable and a die roll will determine a value >= min and <= max.
---
--- resources_to_place tables - 1 ID - 2 quantity - 3 weight - 4 radius min - 5 radius max
---
--- The plot list will be processed sequentially, so shuffle it in advance.<br>
--- The default lists are terrain-oriented and are already shuffled.
--- @param frequency number
--- @param impact_table_number ImpactLayer
--- @param plot_list PlotId[]
--- @param resources_to_place ResourceToPlaceArray[]
function ASP:ProcessResourceList(frequency, impact_table_number, plot_list, resources_to_place)
	if not plot_list then
		print("Plot list was nil! - ProcessResourceList");
		return;
	end

	local iNumTotalPlots = table.maxn(plot_list);
	local iNumResourcesToPlace = math.ceil(iNumTotalPlots / frequency);
	local tResources = {}; --- @type ResourceToPlace[]
	local tWeights = {};
	for _, resource_data in ipairs(resources_to_place) do
		table.insert(tResources, {
			ID = resource_data[1],
			Quantity = resource_data[2],
			MinRadius = resource_data[4],
			MaxRadius = resource_data[5],
		});
		table.insert(tWeights, resource_data[3]);
	end

	local current_index = 1; -- each plot only to be checked once for the first pass
	local avoid_ripples = true;
	local iNumResourcePlaced = 0;
	while iNumResourcePlaced < iNumResourcesToPlace do
		local resourceToPlace = GetRandomFromWeightedList(tResources, tWeights, "Choose resource type - ProcessResourceList");

		-- Still on first pass through plot_list, seek first eligible 0 value on impact matrix
		if avoid_ripples then
			for index_to_check = current_index, iNumTotalPlots do
				if index_to_check == iNumTotalPlots then
					avoid_ripples = false;
				end

				current_index = current_index + 1;

				local plotIndex = plot_list[index_to_check];
				if self.impactData[impact_table_number][plotIndex] == 0 then
					if self:TryPlacingOneResource(resourceToPlace, plotIndex, impact_table_number) then
						iNumResourcePlaced = iNumResourcePlaced + 1;
						break;
					end
				end
			end
		end

		-- Completed first pass through plot_list, so use backup method
		-- This can be very slow if plot_list is large, so modders should use a reasonable frequency/impact radius to prevent this from being used
		if not avoid_ripples then
			local lowest_impact = 98;
			local best_plot;
			for _, plotIndex in ipairs(plot_list) do
				if self.impactData[impact_table_number][plotIndex] < lowest_impact then
					if Map.GetPlotByIndex(plotIndex - 1):GetResourceType() == -1 then
						lowest_impact = self.impactData[impact_table_number][plotIndex];
						best_plot = plotIndex;
					end
				end
			end
			if best_plot then
				if self:TryPlacingOneResource(resourceToPlace, best_plot, impact_table_number) then
					iNumResourcePlaced = iNumResourcePlaced + 1;
				end
			else
				-- print("ProcessResourceList - There isn't any place for this resource");
				break;
			end
		end
	end
end
------------------------------------------------------------------------------
--- Ratio should be > 0 and <= 1 and is what determines when secondary and tertiary lists come in to play.
--- The actual ratio is (AmountOfResource / PlotsInList).
--- For instance, if we are assigning Sugar resources to Marsh, then if we are to assign eight Sugar resources, but there are only four Marsh plots in the list,
--- a ratio of 1 would assign a Sugar to every single marsh plot, and then have to return an unplaced value of 4;
--- but a ratio of 0.5 would assign only two Sugars to the four marsh plots, and return a value of 6.
--- Any ratio less than or equal to 0.25 would assign one Sugar and return seven, as the ratio results will be rounded up not down, to the nearest integer.
---
--- Radius is amount of impact to place on this table when placing a resource.
---
--- The plot list will be processed sequentially, so randomize them in advance.
--- @param resource_ID ResourceType # The type to place
--- @param quantity integer # The in-game quantity of the resource, or 1 if unquantified
--- @param amount integer # The number of deposits to be placed
--- @param ratio number
--- @param impact_table_number ImpactLayer
--- @param min_radius integer
--- @param max_radius integer
--- @param plot_list PlotId[]
--- @return integer # The amount not placed
function ASP:PlaceSpecificNumberOfResources(resource_ID, quantity, amount, ratio, impact_table_number, min_radius, max_radius, plot_list)
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
	local iNumLeftToPlace = amount;
	local iNumPlots = #plot_list;
	local iNumResources = math.min(amount, math.ceil(ratio * iNumPlots));
	local bIsLuxury = (Game.GetResourceUsageType(resource_ID) == ResourceUsageTypes.RESOURCEUSAGE_LUXURY);

	-- Main loop
	for _ = 1, iNumResources do
		for _, plotIndex in ipairs(plot_list) do
			if not bCheckImpact or impact_table[plotIndex] == 0 then
				local x, y = GetCoordinatesFromLuaPlotIndex(plotIndex);
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

						if bIsLuxury then
							self.totalLuxPlacedSoFar = self.totalLuxPlacedSoFar + 1;
							-- print("Total luxuries placed so far: ", self.totalLuxPlacedSoFar);
						end

						iNumLeftToPlace = iNumLeftToPlace - 1;
						if bCheckImpact then
							local radius = GetRandomFromRangeInclusive(min_radius, max_radius);
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
--- Initialize self.regions_sorted_by_type table
--- @param region_type RegionType
function ASP:IdentifyRegionsOfThisType(region_type)
	local regions_of_this_type = {};
	for index, current_type in ipairs(self.regionTypes) do
		if current_type == region_type then
			table.insert(regions_of_this_type, index);
		end
	end

	if next(regions_of_this_type) then
		local scrambled = GetShuffledCopyOfTable(regions_of_this_type);
		for _, region_to_add in pairs(scrambled) do
			table.insert(self.regions_sorted_by_type, {region_to_add}); -- Note: adding region number as a table, so this data can be expanded later.
		end
	end
end
------------------------------------------------------------------------------
--- Initialize self.regions_sorted_by_type table
function ASP:SortRegionsByType()
	for check_this_type = 1, NUM_REGION_TYPES - 1 do
		self:IdentifyRegionsOfThisType(check_this_type);
	end
	-- If any Undefined Regions, put them at the end of the list.
	self:IdentifyRegionsOfThisType(GameInfoTypes.REGION_UNDEFINED);
end
------------------------------------------------------------------------------
--- Extracted from AssignLuxuryToRegion()<br>
--- `eResource` is assumed to be in `iRegionID`'s option table (self.luxury_region_weights)
--- @param eResource ResourceType
--- @param iRegionID RegionId
--- @return boolean
function ASP:IsSpecificLuxuryGoodForRegion(eResource, iRegionID)
	-- Will it exceed regional luxury cap if assigned?
	if self.iNumTypesAssignedToRegions >= self.MAX_LUXURIES_ASSIGNED_TO_REGION and
	not TestMembership(self.resourceIDs_assigned_to_regions, eResource) then
		return false;
	end

	local eRegion = self.regionTypes[iRegionID];

	-- Water-based resources need to have coastal start and enough coast tiles in region
	if eResource == self.whale_ID or eResource == self.pearls_ID or eResource == self.crab_ID or eResource == self.coral_ID then
		if not self.startLocationConditions[iRegionID].IsCoastal then
			return false;
		end

		local tLuxuryTargetNumbers = self:GetRegionLuxuryTargetNumbers();
		local iTarget = tLuxuryTargetNumbers[self.iNumCivs];
		if self.regionTerrainCounts[iRegionID][8] < iTarget * self.NUM_WATER_NEEDED_PER_REGIONAL_LUXURY then
			return false;
		end
	-- Silver and furs need 2 tundra tiles around start location for tundra regions and 2 desert tiles around start location for desert regions
	elseif eResource == self.silver_ID or eResource == self.fur_ID then
		if (eRegion == GameInfoTypes.REGION_TUNDRA and self.startLocationConditions[iRegionID].TundraCount < 2) or
		(eRegion == GameInfoTypes.REGION_DESERT and self.startLocationConditions[iRegionID].DesertCount < 2) then
			print(string.format("%s cannot be placed in region #%d due to a lack of corresponding terrain at start location.",
				L(GameInfo.Resources[eResource].Description), iRegionID));
			return false;
		end
	end

	-- All other land luxuries are automatically approved
	return true;
end
------------------------------------------------------------------------------
--- Extracted from AssignLuxuryToRegion()
--- @param eResource ResourceType
--- @param iBaseWeight integer
--- @param tResources ResourceType[]
--- @param tResourceWeights number[]
function ASP:AssignSpecificLuxury(eResource, iBaseWeight, tResources, tResourceWeights)
	table.insert(tResources, eResource);
	-- If selected before, for a different region, reduce weight
	table.insert(tResourceWeights, iBaseWeight / (1 + self.luxury_assignment_count[eResource]));
end
------------------------------------------------------------------------------
--- @param region_number integer
--- @return ResourceType
function ASP:AssignLuxuryToRegion(region_number)
	-- Assigns a luxury type to an individual region.
	local region_type = self.regionTypes[region_number];
	local luxury_candidates;
	if region_type ~= GameInfoTypes.REGION_UNDEFINED and region_type < NUM_REGION_TYPES then
		luxury_candidates = self.luxury_region_weights[region_type];
	else
		luxury_candidates = self.luxury_fallback_weights; -- Undefined Region, enable all possible luxury types.
	end

	-- Build options list.
	local iNumAvailableTypes = 0;
	local resource_IDs, resource_weights, res_threshold = {}, {}, {};
	local split_cap = self:GetLuxuriesSplitCap();

	for _, resource_options in ipairs(luxury_candidates) do
		local res_ID = resource_options[1];
		if self.luxury_assignment_count[res_ID] < split_cap and resource_options[2] > 0 then -- This type still eligible.
			if self:IsSpecificLuxuryGoodForRegion(res_ID, region_number) then
				self:AssignSpecificLuxury(res_ID, resource_options[2], resource_IDs, resource_weights);
			end
		end
	end

	-- If options list is empty, pick from fallback options. First try to respect water-resources not being assigned to regions without coastal starts.
	if not next(resource_IDs) then
		print("Pick from fallback options for region#", region_number);
		for _, resource_options in ipairs(self.luxury_fallback_weights) do
			local res_ID = resource_options[1];
			if self.luxury_assignment_count[res_ID] < self.MAX_REGIONS_PER_LUXURY and resource_options[2] > 0 then -- This type still eligible.
				if self:IsSpecificLuxuryGoodForRegion(res_ID, region_number) then
					self:AssignSpecificLuxury(res_ID, resource_options[2], resource_IDs, resource_weights);
				end
			end
		end
	end

	-- If we get to here and still need to assign a luxury type, it means we have to force a fallback luxury into this region without specific checks, period.
	-- This should be the rarest of the rare emergency assignment cases, unless modifications to the system have tightened things too far.
	if not next(resource_IDs) then
		print("-"); print("Having to use emergency Luxury assignment process for Region#", region_number);
		print("This likely means a near-maximum number of civs in this game, and problems with not having enough legal Luxury types to spread around.");
		print("If you are modifying luxury types or number of regions allowed to get the same type, check to make sure your changes haven't violated the math so each region can have a legal assignment.");
		for _, resource_options in ipairs(self.luxury_fallback_weights) do
			local res_ID = resource_options[1];
			if self.luxury_assignment_count[res_ID] < self.MAX_REGIONS_PER_LUXURY and resource_options[2] > 0 then
				if self.iNumTypesAssignedToRegions < self.MAX_LUXURIES_ASSIGNED_TO_REGION or TestMembership(self.resourceIDs_assigned_to_regions, res_ID) then
					self:AssignSpecificLuxury(res_ID, resource_options[2], resource_IDs, resource_weights);
				end
			end
		end
	end

	if not next(resource_IDs) then
		print("FAILED to assign a Luxury type to Region#", region_number);
		error("Please report to GitHub with this log file");
	end

	-- Choose luxury.
	local eResource = GetRandomFromWeightedList(resource_IDs, resource_weights, "Choose resource type - Assign Luxury To Region - Lua");
	return eResource;
end
------------------------------------------------------------------------------
--- New for expansion. Cap no longer set to hardcoded value of 3.
--- This was separated out to allow easy replacement in map scripts.
--- @return integer
function ASP:GetLuxuriesSplitCap()
	local split_cap = 1;
	if self.iNumCivs > 16 then
		split_cap = 2;
	end
	return split_cap;
end
------------------------------------------------------------------------------
--- This was separated out to allow easy replacement in map scripts.
--- This function is currently UNUSED.
--- @return integer
function ASP:GetCityStateLuxuriesTargetNumber()
	local worldsizes = {
		[GameInfoTypes.WORLDSIZE_DUEL] = 3,
		[GameInfoTypes.WORLDSIZE_TINY] = 3,
		[GameInfoTypes.WORLDSIZE_SMALL] = 3,
		[GameInfoTypes.WORLDSIZE_STANDARD] = 3,
		[GameInfoTypes.WORLDSIZE_LARGE] = 4,
		[GameInfoTypes.WORLDSIZE_HUGE] = 4,
	};
	local CSluxCount = worldsizes[Map.GetWorldSize()];
	return CSluxCount;
end
------------------------------------------------------------------------------
--- This was separated out to allow easy replacement in map scripts.
--- This function is currently UNUSED.
function ASP:GetDisabledLuxuriesTargetNumber()
	local worldsizes = {
		[GameInfoTypes.WORLDSIZE_DUEL] = 10,
		[GameInfoTypes.WORLDSIZE_TINY] = 7,
		[GameInfoTypes.WORLDSIZE_SMALL] = 5,
		[GameInfoTypes.WORLDSIZE_STANDARD] = 3,
		[GameInfoTypes.WORLDSIZE_LARGE] = 1,
		[GameInfoTypes.WORLDSIZE_HUGE] = 0,
	};
	local maxToDisable = worldsizes[Map.GetWorldSize()];
	return maxToDisable;
end
------------------------------------------------------------------------------
--- This was separated out to allow easy replacement in map scripts.<br>
--- With more luxuries available, this ensures that the total luxuries used each game still match the default game,
--- except for Huge, which really needed a few more anyway!
function ASP:GetRandomLuxuriesTargetNumber()
	local worldsizes = {
		[GameInfoTypes.WORLDSIZE_DUEL] = 4,
		[GameInfoTypes.WORLDSIZE_TINY] = 5,
		[GameInfoTypes.WORLDSIZE_SMALL] = 5,
		[GameInfoTypes.WORLDSIZE_STANDARD] = 5,
		[GameInfoTypes.WORLDSIZE_LARGE] = 7,
		[GameInfoTypes.WORLDSIZE_HUGE] = 9,
	};
	local maxRandoms = worldsizes[Map.GetWorldSize()];
	return maxRandoms;
end
------------------------------------------------------------------------------
--- Each region gets an individual Luxury type assigned to it.<br>
--- Each Luxury type can be assigned to no more than three regions.<br>
--- No more than nine total Luxury types will be assigned to regions.<br>
--- Between two and four Luxury types will be assigned to City States.<br>
--- Remaining Luxury types will be distributed at random or left out.
---
--- Luxury roles must be assigned before City States can be placed.
--- This is because civs who are forced to share their luxury type with other
--- civs may get extra city states placed in their region to compensate.
function ASP:AssignLuxuryRoles()
	-- Create self.regions_sorted_by_type, which will be expanded to store all data regarding regional luxuries.
	self:SortRegionsByType();

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
		local eResource, iIndex = GetRandomFromWeightedList(resource_IDs, resource_weights, "Choose resource type - City State Luxuries - Lua");
		table.insert(self.resourceIDs_assigned_to_cs, eResource);
		table.remove(resource_IDs, iIndex);
		table.remove(resource_weights, iIndex);
		-- print("-"); print("City States have been assigned Luxury ID#", eResource);
	end

	-- Assign Marble and Ivory to special casing.
	table.insert(self.resourceIDs_assigned_to_special_case, self.marble_ID);
	table.insert(self.resourceIDs_assigned_to_special_case, self.ivory_ID);

	-- Assign some luxuries to random distribution, disable the rest.
	local rand_resource_IDs, rand_resource_weights = {}, {};
	for _, resource_options in ipairs(self.luxury_fallback_weights) do
		if resource_options[2] > 0 then
			local res_ID = resource_options[1];
			local test1 = TestMembership(self.resourceIDs_assigned_to_regions, res_ID);
			local test2 = TestMembership(self.resourceIDs_assigned_to_cs, res_ID);
			if not test1 and not test2 then
				table.insert(rand_resource_IDs, res_ID);
				table.insert(rand_resource_weights, resource_options[2]);
			else
				-- print("Luxury ID#", res_ID, "rejected by Randoms as already belonging to Regions or City States.");
			end
		end
	end

	self.iNumTypesRandom = self:GetRandomLuxuriesTargetNumber();
	for i = 1, self.iNumTypesRandom do
		if #rand_resource_IDs == 0 then
			self.iNumTypesRandom = i - 1;
			break;
		end
		local eResource, iIndex = GetRandomFromWeightedList(rand_resource_IDs, rand_resource_weights, "Choose resource type - Random Luxuries - Lua");
		table.insert(self.resourceIDs_assigned_to_random, eResource);
		table.remove(rand_resource_IDs, iIndex);
		table.remove(rand_resource_weights, iIndex);
		-- print("-") print("Luxury ID#", eResource, "assigned to Random.");
	end

	-- Assign remaining luxuries to Disabled.
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

	---[[ Debug printout of luxury assignments.
	print("--- Luxury Assignment Table ---");
	print("-"); print("- - Assigned to Regions - -");
	for _, data in ipairs(self.regions_sorted_by_type) do
		print("Region#", data[1], "has Luxury type", data[2], L(GameInfo.Resources[data[2]].Description));
	end
	print("-"); print("Total unique regional luxuries: ", self.iNumTypesAssignedToRegions);
	print("-"); print("- - Assigned to City States - -");
	for _, type in ipairs(self.resourceIDs_assigned_to_cs) do
		print("Luxury type", type, L(GameInfo.Resources[type].Description));
	end
	print("-"); print("- - Assigned to Random - -");
	for _, type in ipairs(self.resourceIDs_assigned_to_random) do
		print("Luxury type", type, L(GameInfo.Resources[type].Description));
	end
	print("-"); print("- - Luxuries handled via Special Case - -");
	for _, type in ipairs(self.resourceIDs_assigned_to_special_case) do
		print("Luxury type", type, L(GameInfo.Resources[type].Description));
	end
	print("-"); print("- - Disabled - -");
	for _, type in ipairs(resourceIDs_not_being_used) do
		print("Luxury type", type, L(GameInfo.Resources[type].Description));
	end
	print("- - - - - - - - - - - - - - - -");
	--]]
end
------------------------------------------------------------------------------
--- This function is used for placing luxuries at city-states and second types at civ starts.
function ASP:GetListOfAllowableLuxuriesAtCitySite(x, y, radius)
	-- print("-"); print("- -"); print("Getting list of luxuries allowable at city state site:", x, y, "Radius:", radius);
	local allowed_luxuries = table.fill(false, MAX_RESOURCE_INDEX);

	for pLoopPlot in Plot_GetPlotsInCircle(Map.GetPlot(x, y), radius) do
		local ePlot = pLoopPlot:GetPlotType();
		local eTerrain = pLoopPlot:GetTerrainType();
		local eFeature = pLoopPlot:GetFeatureType();

		if pLoopPlot:GetResourceType() == -1 then
			-- Check this plot for luxury placement eligibility. Set allowed luxuries to true.
			if ePlot == PlotTypes.PLOT_OCEAN then -- Testing for Water Luxury eligibility. This is more involved than land-based.
				if eTerrain == TerrainTypes.TERRAIN_COAST then
					if not pLoopPlot:IsLake() then
						if eFeature ~= FeatureTypes.FEATURE_ATOLL and eFeature ~= FeatureTypes.FEATURE_ICE then
							allowed_luxuries[self.whale_ID] = true;
							allowed_luxuries[self.pearls_ID] = true;
							allowed_luxuries[self.crab_ID] = true;
							allowed_luxuries[self.coral_ID] = true;
						end
					end
				end
			-- With AdjustTiles(), plot and feature types don't matter. The only important thing is the terrain type.
			-- This greatly increases the flexibility of placing luxuries at city-states and can easily support more diversity in assignments.
			elseif ePlot == PlotTypes.PLOT_HILLS or ePlot == PlotTypes.PLOT_LAND then
				if eTerrain == TerrainTypes.TERRAIN_TUNDRA then
					allowed_luxuries[self.silver_ID] = true;
					allowed_luxuries[self.copper_ID] = true;
					allowed_luxuries[self.gems_ID] = true;
					allowed_luxuries[self.salt_ID] = true;
					allowed_luxuries[self.jade_ID] = true;
					allowed_luxuries[self.amber_ID] = true;
					allowed_luxuries[self.lapis_ID] = true;
					allowed_luxuries[self.fur_ID] = true;

					if self:IsModActive("EvenMoreResources") then
						allowed_luxuries[self.obsidian_ID] = true;
						allowed_luxuries[self.platinum_ID] = true;
						allowed_luxuries[self.tin_ID] = true;
					end
					if self:IsModActive("AdditionalLuxuries") then
						allowed_luxuries[self.quartz_ID] = true;
						allowed_luxuries[self.feathers_ID] = true;
					end
				elseif eTerrain == TerrainTypes.TERRAIN_DESERT then
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
					if eFeature == FeatureTypes.FEATURE_FLOOD_PLAINS then
						allowed_luxuries[self.sugar_ID] = true;
					end

					if self:IsModActive("EvenMoreResources") then
						allowed_luxuries[self.obsidian_ID] = true;
						allowed_luxuries[self.platinum_ID] = true;
						allowed_luxuries[self.poppy_ID] = true;
						allowed_luxuries[self.tin_ID] = true;
					end
					if self:IsModActive("AdditionalLuxuries") then
						allowed_luxuries[self.quartz_ID] = true;
						allowed_luxuries[self.camel_ID] = true;
					end
				elseif eTerrain == TerrainTypes.TERRAIN_PLAINS then
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

					if self:IsModActive("EvenMoreResources") then
						allowed_luxuries[self.lavender_ID] = true;
						allowed_luxuries[self.obsidian_ID] = true;
						allowed_luxuries[self.platinum_ID] = true;
						allowed_luxuries[self.poppy_ID] = true;
						allowed_luxuries[self.tin_ID] = true;
					end
					if self:IsModActive("AdditionalLuxuries") then
						allowed_luxuries[self.quartz_ID] = true;
						allowed_luxuries[self.alpaca_ID] = true;
						allowed_luxuries[self.feathers_ID] = true;
					end
				elseif eTerrain == TerrainTypes.TERRAIN_GRASS then
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

					if self:IsModActive("EvenMoreResources") then
						allowed_luxuries[self.lavender_ID] = true;
						allowed_luxuries[self.obsidian_ID] = true;
						allowed_luxuries[self.platinum_ID] = true;
						allowed_luxuries[self.poppy_ID] = true;
						allowed_luxuries[self.tin_ID] = true;
					end
					if self:IsModActive("AdditionalLuxuries") then
						allowed_luxuries[self.quartz_ID] = true;
						allowed_luxuries[self.alpaca_ID] = true;
						allowed_luxuries[self.feathers_ID] = true;
					end
				end
			end
		end
	end

	return allowed_luxuries;
end
------------------------------------------------------------------------------
-- Also doubles as Ice Removal.
function ASP:GenerateLuxuryPlotListsAtCitySite(x, y, radius, bRemoveFeatureIce)
	-- bRemoveFeatureIce is piggybacked on to this function to reduce redundant code.
	-- If ice is being removed from around a plot, ONLY that will occur. If both ice
	-- removal and plot list generation are desired, call this function twice.
	-- print("GenerateLuxuryPlotListsAtCitySite called. RemoveIce:", bRemoveFeatureIce, "Plot:", x, y, "Radius:", radius);
	local plotList = {};

	-- Iterate through the city's plots, adding them to the plot list.
	for pLoopPlot in Plot_GetPlotsInCircle(Map.GetPlot(x, y), radius) do
		-- If Ice removal is enabled, process only that.
		if bRemoveFeatureIce then
			if pLoopPlot:GetFeatureType() == FeatureTypes.FEATURE_ICE then
				pLoopPlot:SetFeatureType(FeatureTypes.NO_FEATURE);
			end
		-- Otherwise generate the plot lists.
		else
			local iPlotIndex = pLoopPlot:GetIndex() + 1;
			table.insert(plotList, iPlotIndex);
		end
	end

	return self:GenerateResourcePlotListsFromSpecificPlots(plotList);
end
------------------------------------------------------------------------------
function ASP:GenerateLuxuryPlotListsInRegion(region_number)
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
			local plotIndex = GetLuaPlotIndexFromCoordinates(x, y);
			local plot = Map.GetPlot(x, y);
			-- get plot info
			local ePlot = plot:GetPlotType();
			local eTerrain = plot:GetTerrainType();
			local eFeature = plot:GetFeatureType();

			if ePlot == PlotTypes.PLOT_OCEAN then
				if eTerrain == TerrainTypes.TERRAIN_COAST then
					if not plot:IsLake() and not plot:IsNaturalWonder(true) then
						if eFeature ~= FeatureTypes.FEATURE_ATOLL and eFeature ~= FeatureTypes.FEATURE_ICE then
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
			elseif ePlot ~= PlotTypes.PLOT_MOUNTAIN and not plot:IsNaturalWonder(true) and eFeature ~= FeatureTypes.FEATURE_OASIS then
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
function ASP:GetIndicesForLuxuryType(resource_ID)
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
	-- Additional Resources for Vox Populi
	elseif resource_ID == self.alpaca_ID then
		tList = {
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.HILLS_PLAINS_GRASS_COVERED,
			PlotListTypes.FLAT_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_NO_FEATURE,
		};
	elseif resource_ID == self.camel_ID then
		tList = {
			PlotListTypes.FLAT_DESERT_NO_FEATURE,
			PlotListTypes.HILLS_DESERT,
		};
	elseif resource_ID == self.quartz_ID then
		tList = {
			PlotListTypes.HILLS_DESERT_TUNDRA_NO_FEATURE,
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.HILLS_COVERED,
			PlotListTypes.FLAT_COVERED,
			PlotListTypes.HILLS_SNOW,
			PlotListTypes.FLAT_SNOW,
		};
	elseif resource_ID == self.feathers_ID then
		tList = {
			PlotListTypes.FLAT_FOREST,
			PlotListTypes.FLAT_JUNGLE,
			PlotListTypes.MARSH,
			PlotListTypes.HILLS_COVERED,
			PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE,
		};
	-- Even More Resources for Vox Populi
	elseif self:IsModActive("EvenMoreResources") then
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
	end
	-- print("Found indices of", tList);
	return tList;
end
------------------------------------------------------------------------------
--- This was separated out to allow easy replacement in map scripts.
--- @return integer[] # This table, indexed by civ-count, provides the target amount of luxuries to place in each region.
function ASP:GetRegionLuxuryTargetNumbers()
	local duel_values = table.fill(1, MAX_MAJOR_CIVS); -- Max is one per region for all player counts at this size.
	local tiny_values = {0, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
	local small_values = {0, 3, 3, 3, 4, 4, 4, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
	local standard_values = {0, 3, 3, 4, 4, 5, 5, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
	local large_values = {0, 3, 4, 4, 5, 5, 5, 6, 6, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
	local huge_values = {0, 4, 5, 5, 6, 6, 6, 6, 7, 7, 7, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

	local worldsizes = {
		[GameInfoTypes.WORLDSIZE_DUEL] = duel_values,
		[GameInfoTypes.WORLDSIZE_TINY] = tiny_values,
		[GameInfoTypes.WORLDSIZE_SMALL] = small_values,
		[GameInfoTypes.WORLDSIZE_STANDARD] = standard_values,
		[GameInfoTypes.WORLDSIZE_LARGE] = large_values,
		[GameInfoTypes.WORLDSIZE_HUGE] = huge_values,
	};
	local target_list = worldsizes[Map.GetWorldSize()];
	return target_list;
end
------------------------------------------------------------------------------
--- This was separated out to allow easy replacement in map scripts.
---
--- The first number is the target for total luxuries in the world, NOT
--- counting the one-per-civ "second type" added at start locations.
---
--- The second number affects minimum number of random luxuries placed.<br>
--- I say "affects" because it is only one part of the formula.
--- @return [integer, integer]
function ASP:GetWorldLuxuryTargetNumbers()
	local worldsizes = {};

	local luxDensity = self.luxuryDensity;
	if luxDensity == 4 then
		luxDensity = 1 + Map.Rand(3, "Luxury Resource Density");
	end

	if luxDensity == 1 then -- Sparse
		worldsizes = {
			[GameInfoTypes.WORLDSIZE_DUEL] = {14, 3},
			[GameInfoTypes.WORLDSIZE_TINY] = {24, 4},
			[GameInfoTypes.WORLDSIZE_SMALL] = {36, 4},
			[GameInfoTypes.WORLDSIZE_STANDARD] = {48, 5},
			[GameInfoTypes.WORLDSIZE_LARGE] = {60, 5},
			[GameInfoTypes.WORLDSIZE_HUGE] = {76, 6},
		}
	elseif luxDensity == 3 then -- Abundant
		worldsizes = {
			[GameInfoTypes.WORLDSIZE_DUEL] = {24, 3},
			[GameInfoTypes.WORLDSIZE_TINY] = {40, 4},
			[GameInfoTypes.WORLDSIZE_SMALL] = {60, 4},
			[GameInfoTypes.WORLDSIZE_STANDARD] = {80, 5},
			[GameInfoTypes.WORLDSIZE_LARGE] = {100, 5},
			[GameInfoTypes.WORLDSIZE_HUGE] = {128, 6},
		}
	else -- Standard
		worldsizes = {
			[GameInfoTypes.WORLDSIZE_DUEL] = {18, 3},
			[GameInfoTypes.WORLDSIZE_TINY] = {30, 4},
			[GameInfoTypes.WORLDSIZE_SMALL] = {45, 4},
			[GameInfoTypes.WORLDSIZE_STANDARD] = {60, 5},
			[GameInfoTypes.WORLDSIZE_LARGE] = {75, 5},
			[GameInfoTypes.WORLDSIZE_HUGE] = {95, 6},
		}
	end
	return worldsizes[Map.GetWorldSize()];
end
------------------------------------------------------------------------------
function ASP:GetLowFertilityCompensations(iNumRegions)
	-- Extracted from PlaceLuxuries() for easy override
	-- No fertility compensations by default
	local region_low_fert_compensation = table.fill(0, iNumRegions); -- Stores number of luxury compensation each region received
	local luxury_low_fert_compensation = table.fill(0, MAX_RESOURCE_INDEX); -- Stores number of times each resource ID had extras handed out at civ starts. WARNING: Don't use ID 0.
	return region_low_fert_compensation, luxury_low_fert_compensation;
end
------------------------------------------------------------------------------
function ASP:PlaceLuxuries()
	-- This function is dependent upon AssignLuxuryRoles() and PlaceCityStates() having been executed first.
	local region_low_fert_compensation, luxury_low_fert_compensation = self:GetLowFertilityCompensations(#self.startingPlots);

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
			-- Identify Allowable Luxuries assigned to City States.
			-- If any CS-Only types are eligible, then all combined will have a weighting of 75%.
			local tCSLuxury = {};
			for _, eCSLuxury in ipairs(self.resourceIDs_assigned_to_cs) do
				if allowed_luxuries[eCSLuxury] then
					table.insert(tCSLuxury, eCSLuxury);
				end
			end
			local tCSLuxuryWeight = table.fill(75 / #tCSLuxury, #tCSLuxury);
			-- Identify Allowable Random Luxuries and the Regional Luxury if any.
			-- If any random types are eligible (plus the regional type if in a region), these combined carry a 25% weighting.
			if self.iNumTypesRandom > 0 or region_number > 0 then
				local iNumAllowed = 0;
				for _, eRandomLuxury in pairs(self.resourceIDs_assigned_to_random) do
					if allowed_luxuries[eRandomLuxury] then
						iNumAllowed = iNumAllowed + 1;
						table.insert(tCSLuxury, eRandomLuxury);
					end
				end
				if region_number > 0 then
					iNumAllowed = iNumAllowed + 1; -- Adding the region type into the mix with the random types.
					local eRegionLuxury = self.region_luxury_assignment[region_number];
					table.insert(tCSLuxury, eRegionLuxury);
				end
				for _ = 1, iNumAllowed do
					table.insert(tCSLuxuryWeight, 25 / iNumAllowed);
				end
			end

			-- If there are no allowable luxury types at this city site, then this city state gets none.
			if not next(tCSLuxury) then
				print("City State #", city_state, "has poor land, ineligible to receive a Luxury resource.");
			else
				-- Calculate probability thresholds for each allowable luxury type.
				local eResource = GetRandomFromWeightedList(tCSLuxury, tCSLuxuryWeight, "Choose resource type - Assign Luxury To City State - Lua");
				-- print("-"); print("-"); print("-Assigned Luxury Type", eResource, "to City State#", city_state);
				-- Place luxury.
				local tList = self:GetIndicesForLuxuryType(eResource);
				local luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false);
				local iNumLeftToPlace = 1;
				for _, index in ipairs(tList) do
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(eResource, 1, iNumLeftToPlace, 1, ImpactLayers.LAYER_NONE, 0, 0, luxury_plot_lists[index]);
					if iNumLeftToPlace <= 0 then
						-- print("-"); print("Placed Luxury ID#", eResource, "at City State#", city_state, "in Region#", region_number, "located at Plot", x, y);
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
			local eResource;
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
				eResource = candidate_types[diceroll];
			else
				print("-"); print("Failed to place second Luxury type at start in Region#", region_number, "-- no eligible types!"); print("-");
			end
			-- print("--- End of Eligible Types list for Second Luxury in Region#", region_number, "---");

			if eResource then -- Place this luxury type at this start.
				local tList = self:GetIndicesForLuxuryType(eResource);
				local luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false);
				local iNumLeftToPlace = 1;
				for _, index in ipairs(tList) do
					local shuf_list = luxury_plot_lists[index];
					if iNumLeftToPlace > 0 and index > 0 then
						iNumLeftToPlace = self:PlaceSpecificNumberOfResources(eResource, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
					end
					if iNumLeftToPlace <= 0 then
						-- print("-"); print("Placed Second Luxury type of ID#", eResource, "for start located at Plot", x, y, " in Region#", region_number);
						break;
					end
				end
			end
		end
	end

	self.realtotalLuxPlacedSoFar = self.totalLuxPlacedSoFar; -- MOD.Barathor: New -- save the real total of luxuries before it gets corrupted with non-luxury additions which use the luxury placement method
end
------------------------------------------------------------------------------
function ASP:PlaceMarble()
	local marble_already_placed = self.amounts_of_resources_placed[self.marble_ID + 1];
	local marble_target = math.ceil(self.iNumCivs * 0.75);
	if self.luxuryDensity == 1 then
		marble_target = math.ceil(self.iNumCivs * 0.5);
	elseif self.luxuryDensity == 3 then
		marble_target = math.ceil(self.iNumCivs * 1.0);
	elseif self.luxuryDensity == 4 then
		marble_target = GetRandomFromRangeInclusive(math.ceil(self.iNumCivs * 0.5), math.ceil(self.iNumCivs * 1.0));
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
function ASP:PlaceIvory()
	local ivory_already_placed = self.amounts_of_resources_placed[self.ivory_ID + 1];
	local ivory_target = math.ceil(self.iNumCivs * 0.75);
	if self.luxuryDensity == 1 then
		ivory_target = math.ceil(self.iNumCivs * 0.5);
	elseif self.luxuryDensity == 3 then
		ivory_target = math.ceil(self.iNumCivs * 1.0);
	elseif self.luxuryDensity == 4 then
		ivory_target = GetRandomFromRangeInclusive(math.ceil(self.iNumCivs * 0.5), math.ceil(self.iNumCivs * 1.0));
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
function ASP:PlaceSmallQuantitiesOfStrategics(frequency, plot_list)
	-- This function distributes small quantities of strategic resources.
	if not plot_list then
		print("No strategics were placed! -SmallQuantities");
		return;
	end

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
					local x, y = GetCoordinatesFromLuaPlotIndex(plotIndex);
					local res_plot = Map.GetPlot(x, y);
					if res_plot:GetResourceType() == -1 then
						-- Placing a small strategic resource here. Need to determine what type to place.
						local selected_ID = -1;
						local selected_quantity = 2;
						local ePlot = res_plot:GetPlotType();
						local eTerrain = res_plot:GetTerrainType();
						local eFeature = res_plot:GetFeatureType();
						if eFeature == FeatureTypes.FEATURE_MARSH or eFeature == FeatureTypes.FEATURE_JUNGLE then
							local diceroll = Map.Rand(5, "Resource selection - Place Small Quantities LUA");
							if diceroll < 1 or ePlot == PlotTypes.PLOT_HILLS then
								selected_ID = self.uranium_ID;
								selected_quantity = uran_amt;
							else
								selected_ID = self.oil_ID;
								selected_quantity = oil_amt;
							end
						elseif eFeature == FeatureTypes.FEATURE_FOREST then
							if eTerrain == TerrainTypes.TERRAIN_GRASS or eTerrain == TerrainTypes.TERRAIN_PLAINS then
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
						elseif eFeature == FeatureTypes.NO_FEATURE then
							if ePlot == PlotTypes.PLOT_HILLS then
								if eTerrain == TerrainTypes.TERRAIN_GRASS then
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
								elseif eTerrain == TerrainTypes.TERRAIN_PLAINS then
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
							elseif eTerrain == TerrainTypes.TERRAIN_GRASS then
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
							elseif eTerrain == TerrainTypes.TERRAIN_PLAINS then
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
							elseif eTerrain == TerrainTypes.TERRAIN_DESERT then
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
							elseif eTerrain == TerrainTypes.TERRAIN_TUNDRA then
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
function ASP:PlaceFish(frequency, plot_list)
	-- This function places fish at members of plot_list. (Sounds fishy to me!)
	if not plot_list then
		print("No fish were placed! -PlaceFish");
		return;
	end

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
					local x, y = GetCoordinatesFromLuaPlotIndex(plotIndex);
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
--- This function will place a Bonus resource in the third ring around a Civ's start.<br>
--- The added Bonus is meant to make the start look more sexy, so to speak.<br>
--- Third-ring resources will take a long time to bring online, but will assist the site in the late game.<br>
--- Alternatively, it may assist a different city if another city is settled close enough to the capital and takes control of this tile.
function ASP:PlaceSexyBonusAtCivStarts()
	local bonus_type_associated_with_region_type = {
		self.deer_ID, self.banana_ID,
		self.deer_ID, self.wheat_ID, self.sheep_ID, self.wheat_ID, self.cow_ID, self.cow_ID,
		self.sheep_ID, self.deer_ID,
	};
	local bonus_type_associated_with_tropical_region_type = {
		self.deer_ID, self.banana_ID,
		self.deer_ID, self.maize_ID, self.sheep_ID, self.maize_ID, self.rice_ID, self.cow_ID,
		self.sheep_ID, self.deer_ID,
	};

	for region_number = 1, self.iNumCivs do
		local x = self.startingPlots[region_number][1];
		local y = self.startingPlots[region_number][2];
		local eRegion = self.regionTypes[region_number];
		local eResource = bonus_type_associated_with_region_type[eRegion];
		if IsTropical(y) then
			eResource = bonus_type_associated_with_tropical_region_type[eRegion];
		end
		local plot_list, fish_list = {}, {};
		for iLoopX, iLoopY in Plot_GetPlotCoordinatesInCircle(Map.GetPlot(x, y), 3, 3) do
			local plot = Map.GetPlot(iLoopX, iLoopY);
			local eFeature = plot:GetFeatureType();
			if plot:GetResourceType() == -1 and eFeature ~= FeatureTypes.FEATURE_OASIS then -- No resource or Oasis here, safe to proceed.
				local ePlot = plot:GetPlotType();
				local eTerrain = plot:GetTerrainType();
				local iPlotId = GetLuaPlotIndexFromCoordinates(iLoopX, iLoopY);
				-- Now check this plot for eligibility for the applicable Bonus type for this region.
				if eResource == self.deer_ID then
					if eFeature == FeatureTypes.FEATURE_FOREST then
						table.insert(plot_list, iPlotId);
					elseif eTerrain == TerrainTypes.TERRAIN_TUNDRA and ePlot == PlotTypes.PLOT_LAND then
						table.insert(plot_list, iPlotId);
					end
				elseif eResource == self.banana_ID then
					if eFeature == FeatureTypes.FEATURE_JUNGLE then
						table.insert(plot_list, iPlotId);
					end
				elseif eResource == self.wheat_ID then
					if ePlot == PlotTypes.PLOT_LAND then
						if eTerrain == TerrainTypes.TERRAIN_PLAINS and eFeature == FeatureTypes.NO_FEATURE then
							table.insert(plot_list, iPlotId);
						elseif eFeature == FeatureTypes.FEATURE_FLOOD_PLAINS then
							table.insert(plot_list, iPlotId);
						elseif eTerrain == TerrainTypes.TERRAIN_DESERT and plot:IsFreshWater() then
							table.insert(plot_list, iPlotId);
						end
					end
				elseif eResource == self.sheep_ID then
					if ePlot == PlotTypes.PLOT_HILLS and eFeature == FeatureTypes.NO_FEATURE then
						if eTerrain == TerrainTypes.TERRAIN_PLAINS or eTerrain == TerrainTypes.TERRAIN_GRASS or eTerrain == TerrainTypes.TERRAIN_TUNDRA then
							table.insert(plot_list, iPlotId);
						end
					end
				elseif eResource == self.cow_ID then
					if eTerrain == TerrainTypes.TERRAIN_GRASS and ePlot == PlotTypes.PLOT_LAND then
						if eFeature == FeatureTypes.NO_FEATURE then
							table.insert(plot_list, iPlotId);
						end
					end
				elseif eResource == self.maize_ID then
					if ePlot == PlotTypes.PLOT_LAND then
						if eTerrain == TerrainTypes.TERRAIN_GRASS or eTerrain == TerrainTypes.TERRAIN_PLAINS then
							if eFeature == FeatureTypes.NO_FEATURE then
								table.insert(plot_list, iPlotId);
							end
						elseif eFeature == FeatureTypes.FEATURE_FLOOD_PLAINS then
							table.insert(plot_list, iPlotId);
						elseif eTerrain == TerrainTypes.TERRAIN_DESERT and plot:IsFreshWater() then
							table.insert(plot_list, iPlotId);
						end
					end
				elseif eResource == self.rice_ID then
					if eTerrain == TerrainTypes.TERRAIN_GRASS and eFeature == FeatureTypes.NO_FEATURE and ePlot == PlotTypes.PLOT_LAND then
						table.insert(plot_list, iPlotId);
					elseif eFeature == FeatureTypes.FEATURE_MARSH then
						table.insert(plot_list, iPlotId);
					end
				end
				if ePlot == PlotTypes.PLOT_OCEAN then
					if not plot:IsLake() then
						if eFeature ~= FeatureTypes.FEATURE_ATOLL and eFeature ~= FeatureTypes.FEATURE_ICE then
							if eTerrain == TerrainTypes.TERRAIN_COAST then
								table.insert(fish_list, iPlotId);
							end
						end
					end
				end
			end
		end
		local iNumCandidates = table.maxn(plot_list);
		if iNumCandidates > 0 then
			-- print("Placing 'sexy Bonus' in third ring of start location in Region#", region_number);
			local shuf_list = GetShuffledCopyOfTable(plot_list);
			self:PlaceSpecificNumberOfResources(eResource, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
			if iNumCandidates > 1 and eResource == self.sheep_ID then
				-- Hills region, attempt to give them a second Sexy Sheep.
				-- print("Placing a second 'sexy Sheep' in third ring of start location in Hills Region#", region_number);
				self:PlaceSpecificNumberOfResources(eResource, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
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
function ASP:AddExtraBonusesToHillsRegions()
	-- Hills/Mountain regions are very low on food, yet not deemed by the fertility measurements to be so.
	-- Spreading some food bonus around in these regions will help bring them up closer to par.
	local iW, iH = Map.GetGridSize();
	-- Identify Hills Regions, if any.
	local hills_regions, iNumHillsRegions = {}, 0;
	for region_number = 1, self.iNumCivs do
		if self.regionTypes[region_number] == GameInfoTypes.REGION_HILLS or
			self.regionTypes[region_number] == GameInfoTypes.REGION_MOUNTAIN then
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
				local ePlot = plot:GetPlotType();
				local eTerrain = plot:GetTerrainType();
				local eFeature = plot:GetFeatureType();
				if ePlot == PlotTypes.PLOT_LAND or ePlot == PlotTypes.PLOT_HILLS then
					-- Check plot for region membership. Only process this plot if it is a member.
					if iAreaID == -1 or (self.bArea and area_of_plot == iAreaID) or (not self.bArea and landmass_of_plot == iAreaID) then
						if plot:GetResourceType() == -1 then
							if eFeature == FeatureTypes.FEATURE_JUNGLE then
								table.insert(jungles, plotIndex);
							elseif eFeature == FeatureTypes.FEATURE_FOREST then
								table.insert(forests, plotIndex);
							elseif eFeature == FeatureTypes.FEATURE_FLOOD_PLAINS then
								table.insert(flat_plains, plotIndex);
							elseif eFeature == FeatureTypes.NO_FEATURE then
								if ePlot == PlotTypes.PLOT_HILLS then
									if eTerrain == TerrainTypes.TERRAIN_GRASS or eTerrain == TerrainTypes.TERRAIN_PLAINS or eTerrain == TerrainTypes.TERRAIN_TUNDRA then
										if not plot:IsFreshWater() then
											table.insert(dry_hills, plotIndex);
										end
									end
								elseif ePlot == PlotTypes.PLOT_LAND then
									if eTerrain == TerrainTypes.TERRAIN_PLAINS or eTerrain == TerrainTypes.TERRAIN_DESERT and plot:IsFreshWater() then
										if IsTropical(y) then
											table.insert(flat_tropical_plains, plotIndex);
										else
											table.insert(flat_plains, plotIndex);
										end
									elseif eTerrain == TerrainTypes.TERRAIN_GRASS then
										if IsTropical(y) then
											table.insert(flat_tropical_grass, plotIndex);
										else
											table.insert(flat_grass, plotIndex);
										end
									elseif eTerrain == TerrainTypes.TERRAIN_TUNDRA then
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
function ASP:AddModernMinorStrategicsToCityStates()
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
				local eResource, res_amt;
				local tList = {};
				if diceroll == 2 then
					eResource = self.coal_ID;
					res_amt = coal_amt;
					tList = {
						PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
						PlotListTypes.FLAT_PLAINS_DRY_NO_FEATURE,
						PlotListTypes.FLAT_GRASS_DRY_NO_FEATURE,
						PlotListTypes.FLAT_GRASS_WET_NO_FEATURE,
						PlotListTypes.FLAT_PLAINS_WET_NO_FEATURE,
					};
				elseif diceroll == 3 then
					eResource = self.oil_ID;
					res_amt = oil_amt;
					tList = {
						PlotListTypes.FLAT_DESERT_NO_FEATURE,
						PlotListTypes.MARSH,
						PlotListTypes.FLAT_JUNGLE,
						PlotListTypes.FLAT_TUNDRA_NO_FEATURE,
						PlotListTypes.COAST_NEXT_TO_LAND,
					};
				elseif diceroll == 4 then
					eResource = self.aluminum_ID;
					res_amt = alum_amt;
					tList = {
						PlotListTypes.HILLS_DESERT_TUNDRA_PLAINS_NO_FEATURE,
						PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE,
						PlotListTypes.FLAT_PLAINS_DRY_NO_FEATURE,
						PlotListTypes.FLAT_PLAINS_WET_NO_FEATURE,
					};
				end
				print("Assigned Strategic Type", L(GameInfo.Resources[eResource].Description), "to City State#", city_state);
				-- Place strategic.
				local luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 3, false);
				local iNumLeftToPlace = 1;
				for _, index in ipairs(tList) do
					if index > 0 then
						local shuf_list = luxury_plot_lists[index];
						iNumLeftToPlace = self:PlaceSpecificNumberOfResources(eResource, res_amt, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
					end
					if iNumLeftToPlace <= 0 then
						print("Placed Minor", L(GameInfo.Resources[eResource].Description), "at City State#", city_state, "located at Plot", x, y);
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
function ASP:PlaceOilInTheSea()
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
function ASP:AttemptToPlaceTreesAtResourcePlot(plot)
	-- Sub-function of AdjustTiles()
	-- Place forest or jungle on a resource plot depending on latitude and resource type
	local iResourceType = plot:GetResourceType();
	if IsTropical(plot:GetY()) then
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
function ASP:AdjustTiles()
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
			local eTerrain = plot:GetTerrainType();
			local eFeature = plot:GetFeatureType();

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
			   (self:IsModActive("AdditionalLuxuries") and res_ID == self.quartz_ID) or
			   -- MOD.HungryForFood: Start
			   self:IsModActive("EvenMoreResources") and
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
				if eFeature ~= FeatureTypes.FEATURE_FLOOD_PLAINS then
					removeFeature = true;
				end
				-- Gems can be in jungles
				if eFeature == FeatureTypes.FEATURE_JUNGLE and res_ID == self.gems_ID then
					removeFeature = false;
				end
				if removeFeature then
					plot:SetFeatureType(FeatureTypes.NO_FEATURE);
				end

				if res_ID == self.silver_ID and eTerrain == TerrainTypes.TERRAIN_SNOW then
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
				   res_ID == self.feathers_ID or
				   -- MOD.HungryForFood: Start
				   self:IsModActive("EvenMoreResources") and
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
				if eFeature == FeatureTypes.FEATURE_MARSH then
					if res_ID == self.sugar_ID or res_ID == self.truffles_ID or res_ID == self.feathers_ID then
						-- Keep it marsh for these resources.
					else
						-- Add some jungle or forest.
						self:AttemptToPlaceTreesAtResourcePlot(plot);
					end
				else
					-- Sugar can only be on marsh or flood plains
					if res_ID == self.sugar_ID then
						if eFeature ~= FeatureTypes.FEATURE_FLOOD_PLAINS then
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
				   (self:IsModActive("AdditionalLuxuries") and res_ID == self.alpaca_ID) or
				   -- MOD.HungryForFood: Start
				   self:IsModActive("EvenMoreResources") and
				   (
				   res_ID == self.poppy_ID
				   )
				   -- MOD.HungryForFood: End
				   then

				if self:IsModActive("AdditionalLuxuries") and res_ID == self.alpaca_ID then
					-- Always want it on bare hills.
					plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
					plot:SetFeatureType(FeatureTypes.NO_FEATURE);
				end

				if res_ID == self.ivory_ID then
					-- Always want it flat. Other types are fine on hills.
					plot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
				end

				-- Don't remove flood plains if present for the few that are placed on it, only remove other features, like marsh or any trees.
				if eFeature ~= FeatureTypes.FEATURE_FLOOD_PLAINS then
					plot:SetFeatureType(FeatureTypes.NO_FEATURE);
				end

				if res_ID == self.incense_ID or res_ID == self.ivory_ID then
					-- Because incense/ivory are very restricted, it was expanded to look for grass tiles as a final fallback.
					-- This will help with certain distributions that incense previously didn't work well in, such as assignments to city-states which could be hit or miss.
					-- Besides jungle placements, this is the only luxury which will change the terrain it's found on. Plus, plains are mixed in with grass anyway.
					if eTerrain == TerrainTypes.TERRAIN_GRASS then
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true);
					end
				end
			end
		end
	end
end
------------------------------------------------------------------------------
function ASP:PrintFinalResourceTotalsToLog()
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
	if self:IsModActive("AdditionalLuxuries") then
		print(self.alpaca_ID, 	"Alpaca..: ", self.amounts_of_resources_placed[self.alpaca_ID + 1]);
		print(self.camel_ID, 	"Camel...: ", self.amounts_of_resources_placed[self.camel_ID + 1]);
		print(self.quartz_ID,   "Quartz..: ", self.amounts_of_resources_placed[self.quartz_ID + 1]);
		print(self.feathers_ID, "Feathers: ", self.amounts_of_resources_placed[self.feathers_ID + 1]);
	end
	print("-");
	-- MOD.HungryForFood: Start
	if self:IsModActive("EvenMoreResources") then
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
	if self:IsModActive("EvenMoreResources") then
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
	if self:IsModActive("AloeVera") then
		print("- Aloe Vera for Vox Populi (Bonus) -");
		print(self.aloevera_ID, "AloeVera: ", self.amounts_of_resources_placed[self.aloevera_ID + 1]);
	end
	print("-----------------------------------------------------");
end
------------------------------------------------------------------------------
function ASP:GetMajorStrategicResourceQuantityValues()
	-- This function determines quantity per tile for each strategic resource's major deposit size.
	-- Note: scripts that cannot place Oil in the sea need to increase amounts on land to compensate.
	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 3, 5, 3, 4, 7;
	-- Check the strategic deposit size setting.
	if self.resSize == 1 then -- Small
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 2, 3, 2, 2, 3;
	elseif self.resSize == 3 then -- Large
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 4, 6, 8, 6, 7, 10;
	end

	if self:IsModActive("ReducedSupply") then
		print("Reduced Supply Mod is active, halving major strategic supply quantity values");
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = math.floor(uran_amt / 2), math.floor(horse_amt / 2), math.floor(oil_amt / 2), math.floor(iron_amt / 2), math.floor(coal_amt / 2), math.floor(alum_amt / 2);
	end

	return uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt;
end
------------------------------------------------------------------------------
function ASP:GetSmallStrategicResourceQuantityValues()
	-- This function determines quantity per tile for each strategic resource's small deposit size.
	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 1, 2, 2, 2, 2, 3;
	-- Check the strategic deposit size setting.
	if self.resSize == 1 then -- Small
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 1, 1, 1, 1, 1, 2;
	elseif self.resSize == 3 then -- Large
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 2, 2, 2, 2, 3;
	end

	if self:IsModActive("ReducedSupply") then
		print("Reduced Supply Mod is active, halving small strategic supply quantity values");
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 1, 1, 1, 1, 1, 2;
	end

	return uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt;
end
------------------------------------------------------------------------------
function ASP:PlaceStrategicAndBonusResources()
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
		resMultiplier = GetRandomMultiplier(0.5);
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
function ASP:PlaceBonusResources()
	local resMultiplier = 1;
	if self.bonusDensity == 1 then -- Sparse, so increase the number of tiles per bonus.
		resMultiplier = 1.5;
	elseif self.bonusDensity == 3 then -- Abundant, so reduce the number of tiles per bonus.
		resMultiplier = 0.66666667;
	elseif self.bonusDensity == 4 then -- Random
		resMultiplier = GetRandomMultiplier(0.5);
	end

	-- Place Bonus Resources
	print("Map Generation - Placing Bonuses");
	-- ####Communitu_79a has a completely different PlaceFish mechanic, but that's too map dependent
	local tPlotList = self.global_resource_plot_lists;
	self:PlaceFish(10 * resMultiplier, tPlotList[PlotListTypes.COAST]);
	self:PlaceSexyBonusAtCivStarts();
	self:AddExtraBonusesToHillsRegions();

	local resources_to_place = {};

	if self:IsModActive("AloeVera") then
		resources_to_place = {
			{self.aloevera_ID, 1, 100, 1, 2}
		};
		self:ProcessResourceList(10 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_DESERT_NO_FEATURE], resources_to_place);
	end

	if self:IsModActive("EvenMoreResources") then
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
function ASP:PlaceResourcesAndCityStates()
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
	self:PlaceLuxuries();

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
-- Mod checker
------------------------------------------------------------------------------
local tModIdentifier = {
	CommunityPatch = {
		ID = "d1b6328c-ff44-4b0d-aad7-c657f83610cd",
	},
	EvenMoreResources = {
		ID = "8e54eb87-31e8-4fcd-aafe-ede055b463d0",
		Query = DB.CreateQuery("SELECT 1 FROM Resources WHERE Type = 'RESOURCE_POPPY'"),
	},
	AloeVera = {
		ID = "c2c3797b-2728-417c-94c8-f30410528bf2",
		Query = DB.CreateQuery("SELECT 1 FROM Resources WHERE Type = 'RESOURCE_JAR_ALOE_VERA'"),
	},
	AdditionalLuxuries = {
		ID = "8634880a-749f-4d64-aa68-560378e0f007",
		Query = DB.CreateQuery("SELECT 1 FROM Resources WHERE Type = 'RESOURCE_QUARTZ'"),
	},
	ReducedSupply = {
		ID = "abbade81-2894-41e9-SUPP-SWVPx13a770d",
		Query = DB.CreateQuery("SELECT 1 FROM Defines WHERE Name = 'FRIENDS_BASE_TURNS_UNIT_SPAWN' AND Value = 28"),
	},
}

--- @param strModName string
--- @return boolean
function ASP:IsModActive(strModName)
	if not tModIdentifier[strModName] then
		print("WARNING: Unsupported mod passed into IsModActive:", strModName);
		return false;
	end

	local bCPActive = false;
	for _, mod in pairs(Modding.GetActivatedMods()) do
		if mod.ID == tModIdentifier[strModName].ID then
			return true;
		end
		if mod.ID == tModIdentifier.CommunityPatch.ID then -- if Community Patch is activated, then we are not running in modpack mode
			bCPActive = true;
		end
	end

	if not bCPActive then -- fallback method for modpack mode
		for _ in tModIdentifier[strModName].Query() do
			return true;
		end
	end
	return false;
end

-- Finally, "export" the ASP class as a global
AssignStartingPlots = ASP;

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
AdjustTiles()

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

## VP Note:
The constants are now called MAX_REGIONS_PER_LUXURY and MAX_LUXURIES_ASSIGNED_TO_REGION respectively.
Their product must be more than 43 if your mod supports 43 civs.
##

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
