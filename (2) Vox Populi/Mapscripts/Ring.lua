-------------------------------------------------------------------------------
--	FILE:	 Ring.lua
--	AUTHOR:  Bob Thomas (Sirian)
--	PURPOSE: Regional map script - One area per player, ring-linked.
-------------------------------------------------------------------------------
--	Copyright (c) 2010 Firaxis Games, Inc. All rights reserved.
-------------------------------------------------------------------------------

include("MapGenerator");
include("MultilayeredFractal");
include("FeatureGenerator");
include("TerrainGenerator");
include("MapmakerUtilities");
local dominant_terrain;
local centWestX, centEastX, centNorthY, centSouthY;
local region_coords_data;

------------------------------------------------------------------------------
--[[
SIRIAN'S NOTE - December 2, 2010

This script is based off the Civ4 version, but there are some significant
changes. With 1UPT, the single-width bridges had to go. Even two plots is
probably too narrow, but I've left that option in. Any bridge segments are
flat land, but can come with forest or jungle on them, for defensive terrain.

City States can only appear in the polar region in the center. The script is
also set to place a maximum of five of them, though I doubt more would fit
even on Huge maps.

As multiplayer only officially supports 8 Civs on the map (and allows a max
of 12) and this is primarily a multiplayer map, I have lowered the number of
supported civs on the map to 12. More than that was too crowded anyway, even
for single player use of the script.

Ring now comes with four options for dominant terrain, though, instead of
just Grassland like before. Start plots have a wider range to choose from
than previously, so tend to find the best spot available much more often. In
some cases, a civ can even start along the coast now. (Either coast!)
--]]
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function GetMapScriptInfo()
	return {
		Name = "TXT_KEY_MAP_RING_VP",
		Description = "TXT_KEY_MAP_RING_HELP",
		IconAtlas = "WORLDTYPE_ATLAS_2",
		IconIndex = 2,
		DefaultCityStates = 5,
		SupportsSinglePlayer = true,
		SupportsMultiplayer = true,
		CustomOptions = {
			{
				Name = "TXT_KEY_MAP_OPTION_DOMINANT_TERRAIN",
				Values = {
					{"TXT_KEY_MAP_SCRIPT_SKIRMISH_GRASSLANDS", "TXT_KEY_MAP_SCRIPT_SKIRMISH_GRASSLANDS_HELP"},
					{"TXT_KEY_MAP_SCRIPT_SKIRMISH_PLAINS", "TXT_KEY_MAP_SCRIPT_SKIRMISH_PLAINS_HELP"},
					{"TXT_KEY_MAP_SCRIPT_SKIRMISH_FOREST", "TXT_KEY_MAP_SCRIPT_SKIRMISH_FOREST_HELP"},
					{"TXT_KEY_MAP_SCRIPT_SKIRMISH_JUNGLE", "TXT_KEY_MAP_SCRIPT_SKIRMISH_JUNGLE_HELP"},
					"TXT_KEY_MAP_OPTION_RANDOM",
				},
				DefaultValue = 5,
				SortPriority = 1,
			},
			{
				Name = "TXT_KEY_MAP_OPTION_LAND_SHAPE",
				Values = {
					{"TXT_KEY_MAP_OPTION_NATURAL", "TXT_KEY_MAP_OPTION_NATURAL_HELP"},
					{"TXT_KEY_MAP_OPTION_PRESSED", "TXT_KEY_MAP_OPTION_PRESSED_HELP"},
					{"TXT_KEY_MAP_OPTION_SOLID", "TXT_KEY_MAP_OPTION_SOLID_HELP"},
					"TXT_KEY_MAP_OPTION_RANDOM",
				},
				DefaultValue = 4,
				SortPriority = 2,
			},
			{
				Name = "TXT_KEY_MAP_OPTION_ISTHMUS_WIDTH",
				Values = {
					{"TXT_KEY_MAP_OPTION_PLOTS_WIDE_TWO", "TXT_KEY_MAP_OPTION_PLOTS_WIDE_TWO_HELP"},
					{"TXT_KEY_MAP_OPTION_PLOTS_WIDE_THREE", "TXT_KEY_MAP_OPTION_PLOTS_WIDE_THREE_HELP"},
					{"TXT_KEY_MAP_OPTION_PLOTS_WIDE_FOUR", "TXT_KEY_MAP_OPTION_PLOTS_WIDE_FOUR_HELP"},
					"TXT_KEY_MAP_OPTION_RANDOM",
				},
				DefaultValue = 2,
				SortPriority = 3,
			},
			{
				Name = "TXT_KEY_MAP_OPTION_RESOURCES",
				Values = { -- Only one option here, but this will let all users know that resources are not set at default.
					"TXT_KEY_MAP_OPTION_BALANCED_RESOURCES",
				},
				DefaultValue = 1,
				SortPriority = 4,
			},
		},
	};
end
------------------------------------------------------------------------------

-------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	-- This function can reset map grid sizes or world wrap settings.

	-- Ring is a world with extra land vs water, so use grid sizes one level below normal.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {32, 20},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {44, 28},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {52, 32},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {64, 40},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {84, 52},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {104, 64},
	}
	local grid_size = worldsizes[worldSize];

	if GameInfo.Worlds[worldSize] then
		return {
			Width = grid_size[1],
			Height = grid_size[2],
			WrapX = false,
		};
	end
end
-------------------------------------------------------------------------------

------------------------------------------------------------------------------
function MultilayeredFractal:GeneratePlotsByRegion()
	-- Sirian's MultilayeredFractal controlling function.
	-- You -MUST- customize this function for each script using MultilayeredFractal.
	-- The following regions are specific to Ring.

	-- Get user inputs.
	dominant_terrain = Map.GetCustomOption(1); -- GLOBAL variable.
	if dominant_terrain == 5 then -- Random
		dominant_terrain = 1 + Map.Rand(4, "Random Type of Dominant Terrain - Ring LUA");
	end
	local landmass_type = Map.GetCustomOption(2);
	if landmass_type == 4 then
		landmass_type = 1 + Map.Rand(3, "Landmass Type - Lua");
	end
	local bridge_width = Map.GetCustomOption(3);
	if bridge_width == 4 then -- Random
		bridge_width = 2 + Map.Rand(3, "Random Bridge Width - Ring LUA");
	else
		bridge_width = bridge_width + 1;
	end

	-- Draw the ring to connect all players.
	local ring_corners = {
		{},
		{{0.167, 0.3}, {0.167, 0.7}, {0.833, 0.7}, {0.833, 0.3}, {0.167, 0.3}}, -- 2 civs
		{{0.125, 0.125}, {0.125, 0.875}, {0.875, 0.875}, {0.875, 0.125}, {0.125, 0.125}},
		{{0.167, 0.167}, {0.167, 0.833}, {0.833, 0.833}, {0.833, 0.167}, {0.167, 0.167}},
		{{0.125, 0.167}, {0.125, 0.833}, {0.875, 0.833}, {0.875, 0.167}, {0.125, 0.167}}, -- 5 civs
		{{0.1, 0.167}, {0.1, 0.833}, {0.9, 0.833}, {0.9, 0.167}, {0.1, 0.167}},
		{{0.1, 0.125}, {0.1, 0.875}, {0.9, 0.875}, {0.9, 0.125}, {0.1, 0.125}},
		{{0.167, 0.167}, {0.167, 0.833}, {0.833, 0.833}, {0.833, 0.167}, {0.167, 0.167}},
		{{0.083, 0.1}, {0.083, 0.9}, {0.917, 0.9}, {0.917, 0.1}, {0.083, 0.1}},
		{{0.083, 0.083}, {0.083, 0.917}, {0.917, 0.917}, {0.917, 0.083}, {0.083, 0.083}}, -- 10 civs
		{{0.1, 0.167}, {0.1, 0.833}, {0.9, 0.833}, {0.9, 0.167}, {0.1, 0.167}},
		{{0.1, 0.167}, {0.1, 0.833}, {0.9, 0.833}, {0.9, 0.167}, {0.1, 0.167}},
	};
	local iNumCivs = 0;
	for i = 0, MAX_MAJOR_CIVS - 1 do
		local player = Players[i];
		if player:IsEverAlive() then
			iNumCivs = iNumCivs + 1;
		end
	end
	local ring_coords = ring_corners[iNumCivs];
	local iW, iH = Map.GetGridSize();
	local offsetstart = 0 - math.floor(bridge_width / 2);
	for ring_loop = 1, 4 do
		local start_coords, end_coords = ring_coords[ring_loop], ring_coords[ring_loop + 1];
		local startx = math.floor(start_coords[1] * iW);
		local starty = math.floor(start_coords[2] * iH);
		local endx = math.floor(end_coords[1] * iW);
		local endy = math.floor(end_coords[2] * iH);
		if math.abs(endy - starty) < math.abs(endx - startx) then
			-- line is horizontal
			if startx > endx then
				startx, starty, endx, endy = endx, endy, startx, starty; -- swap start and end
			end
			local y = starty;
			for x = startx, endx do
				for offset = offsetstart, offsetstart + bridge_width - 1 do
					local plot = Map.GetPlot(x, y + offset);
					if plot then
						local i = (y + offset) * iW + x + 1;
						self.wholeworldPlotTypes[i] = PlotTypes.PLOT_LAND;
					end
				end
			end
		else
			-- line is vertical
			if starty > endy then
				startx, starty, endx, endy = endx, endy, startx, starty; -- swap start and end
			end
			local x = startx;
			for y = starty, endy do
				for offset = offsetstart, offsetstart + bridge_width - 1 do
					local plot = Map.GetPlot(x + offset, y);
					if plot then
						local i = y * iW + (x + offset) + 1;
						self.wholeworldPlotTypes[i] = PlotTypes.PLOT_LAND;
					end
				end
			end
		end
	end

	-- Add center island
	local center_isle_data = {
		{},
		{0.4, 0.6, 0.4, 0.6},
		{0.4, 0.7, 0.35, 0.65},
		{0.4, 0.6, 0.4, 0.6},
		{0.32, 0.68, 0.4, 0.6},
		{0.3, 0.7, 0.4, 0.6},
		{0.3, 0.7, 0.37, 0.63},
		{0.4, 0.6, 0.4, 0.6},
		{0.3, 0.7, 0.37, 0.63},
		{0.3, 0.7, 0.33, 0.67},
		{0.3, 0.7, 0.4, 0.6},
		{0.3, 0.7, 0.4, 0.6},
	};
	local center_isle = center_isle_data[iNumCivs];

	centWestX = math.floor(iW * center_isle[1]); -- GLOBALS in some cases here.
	centEastX = math.floor(iW * center_isle[2]) - 1;
	local centWidth = centEastX - centWestX + 1;
	centSouthY = math.floor(iH * center_isle[3]);
	centNorthY = math.floor(iH * center_isle[4]) - 1;
	local centHeight = centNorthY - centSouthY + 1;

	local args = {};
	args.iWaterPercent = 65;
	args.iRegionWidth = centWidth;
	args.iRegionHeight = centHeight;
	args.iRegionWestX = centWestX;
	args.iRegionSouthY = centSouthY;
	args.iRegionGrain = 4 - landmass_type;
	args.iRegionHillsGrain = 3;
	args.iRegionPlotFlags = self.iHorzFlags;

	self:GenerateFractalLayer(args);

	-- Templates are nested by keys: {NumRegions: {Region#: {WestLon, EastLon, SouthLat, NorthLat}}}
	local templates = {
		{},
		-- 2 Civs
		{
			{0.0, 0.333, 0.0, 1.0},
			{0.667, 1.0, 0.0, 1.0},
		},
		-- 3 Civs
		{
			{0.0, 0.25, 0.25, 0.75},
			{0.5, 1.0, 0.75, 1.0},
			{0.5, 1.0, 0.0, 0.25},
		},
		-- 4 Civs
		{
			{0.0, 0.333, 0.333, 0.667},
			{0.333, 0.667, 0.667, 1.0},
			{0.667, 1.0, 0.333, 0.667},
			{0.333, 0.667, 0.0, 0.333},
		},
		-- 5 Civs
		{
			{0.0, 0.25, 0.333, 1.0},
			{0.25, 0.75, 0.667, 1.0},
			{0.75, 1.0, 0.333, 1.0},
			{0.5, 1.0, 0.0, 0.333},
			{0.0, 0.5, 0.0, 0.333},
		},
		-- 6 Civs
		{
			{0.0, 0.4, 0.667, 1.0},
			{0.4, 0.8, 0.667, 1.0},
			{0.8, 1.0, 0.333, 1.0},
			{0.6, 1.0, 0.0, 0.333},
			{0.2, 0.6, 0.0, 0.333},
			{0.0, 0.2, 0.0, 0.667},
		},
		-- 7 Civs
		{
			{0.0, 0.4, 0.75, 1.0},
			{0.4, 0.8, 0.75, 1.0},
			{0.8, 1.0, 0.5, 1.0},
			{0.8, 1.0, 0.0, 0.5},
			{0.4, 0.8, 0.0, 0.25},
			{0.0, 0.4, 0.0, 0.25},
			{0.0, 0.2, 0.25, 0.75},
		},
		-- 8 Civs
		{
			{0.0, 0.3, 0.35, 0.65},
			{0.0, 0.3, 0.7, 1.0},
			{0.35, 0.65, 0.7, 1.0},
			{0.7, 1.0, 0.7, 1.0},
			{0.7, 1.0, 0.35, 0.65},
			{0.7, 1.0, 0.0, 0.3},
			{0.35, 0.65, 0.0, 0.3},
			{0.0, 0.3, 0.0, 0.3},
		},
		-- 9 Civs
		{
			{0.0, 0.167, 0.6, 1.0},
			{0.167, 0.5, 0.8, 1.0},
			{0.5, 0.833, 0.8, 1.0},
			{0.833, 1.0, 0.6, 1.0},
			{0.833, 1.0, 0.2, 0.6},
			{0.667, 1.0, 0.0, 0.2},
			{0.333, 0.667, 0.0, 0.2},
			{0.0, 0.333, 0.0, 0.2},
			{0.0, 0.167, 0.2, 0.6},
		},
		-- 10 Civs
		{
			{0.0, 0.167, 0.5, 0.833},
			{0.0, 0.333, 0.833, 1.0},
			{0.333, 0.667, 0.833, 1.0},
			{0.667, 1.0, 0.833, 1.0},
			{0.833, 1.0, 0.5, 0.833},
			{0.833, 1.0, 0.167, 0.5},
			{0.667, 1.0, 0.0, 0.167},
			{0.333, 0.667, 0.0, 0.167},
			{0.0, 0.333, 0.0, 0.167},
			{0.0, 0.167, 0.167, 0.5},
		},
		-- 11 Civs
		{
			{0.0, 0.2, 0.167, 0.5},
			{0.0, 0.2, 0.5, 0.833},
			{0.2, 0.4, 0.667, 1.0},
			{0.4, 0.6, 0.667, 1.0},
			{0.6, 0.8, 0.667, 1.0},
			{0.8, 1.0, 0.667, 1.0},
			{0.8, 1.0, 0.333, 0.667},
			{0.8, 1.0, 0.0, 0.333},
			{0.6, 0.8, 0.0, 0.333},
			{0.4, 0.6, 0.0, 0.333},
			{0.2, 0.4, 0.0, 0.333},
		},
		-- 12 Civs
		{
			{0.0, 0.2, 0.333, 0.667},
			{0.0, 0.2, 0.667, 1.0},
			{0.2, 0.4, 0.667, 1.0},
			{0.4, 0.6, 0.667, 1.0},
			{0.6, 0.8, 0.667, 1.0},
			{0.8, 1.0, 0.667, 1.0},
			{0.8, 1.0, 0.333, 0.667},
			{0.8, 1.0, 0.0, 0.333},
			{0.6, 0.8, 0.0, 0.333},
			{0.4, 0.6, 0.0, 0.333},
			{0.2, 0.4, 0.0, 0.333},
			{0.0, 0.2, 0.0, 0.333},
		},
	};
	local template_data = templates[iNumCivs];

	-- Create a subcontinent for each civ. Also record the data for later use with start plots and resources.
	region_coords_data = {}; -- GLOBAL
	for region_loop = 1, iNumCivs do
		local region_template = template_data[region_loop];
		local iWestX = math.max(1, math.floor((iW - 1) * region_template[1]));
		local iEastX = math.min(iW - 2, math.floor((iW - 1) * region_template[2] - 1));
		local iSouthY = math.max(1, math.floor((iH - 1) * region_template[3]));
		local iNorthY = math.min(iH - 2, math.floor((iH - 1) * region_template[4] - 1));
		table.insert(region_coords_data, {iWestX, iEastX, iSouthY, iNorthY});

		-- Main fractal
		local iWidth = iEastX - iWestX + 1;
		local iHeight = iNorthY - iSouthY + 1;

		args = {};
		args.iWaterPercent = 55;
		args.iRegionWidth = iWidth;
		args.iRegionHeight = iHeight;
		args.iRegionWestX = iWestX;
		args.iRegionSouthY = iSouthY;
		args.iRegionGrain = 4 - landmass_type;
		args.iRegionHillsGrain = 3;
		args.iRegionPlotFlags = self.iNoFlags;

		self:GenerateFractalLayer(args);

		-- Core fractal to increase cohesion
		args = {};
		args.iWaterPercent = 55;
		args.iRegionWidth = iWidth;
		args.iRegionHeight = iHeight;
		args.iRegionWestX = iWestX;
		args.iRegionSouthY = iSouthY;
		args.iRegionGrain = 1;
		args.iRegionHillsGrain = 3;
		args.iRegionPlotFlags = self.iHorzFlags;
		args.iRegionFracXExp = 5;
		args.iRegionFracYExp = 5;

		self:GenerateFractalLayer(args);
	end

	-- Plot Type generation completed. Return global plot array.
	return self.wholeworldPlotTypes;
end
------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Setting Plot Types (Lua Ring) ...");

	local layered_world = MultilayeredFractal.Create();
	local plotsRing = layered_world:GeneratePlotsByRegion();

	SetPlotTypes(plotsRing);
	GenerateCoasts();
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function TerrainGenerator:GenerateTerrainAtPlot(iX, iY)
	local terrainVal = self.terrainGrass;

	local plot = Map.GetPlot(iX, iY);
	if plot:IsWater() then
		local val = plot:GetTerrainType();
		if val == TerrainTypes.NO_TERRAIN then -- Error handling.
			val = self.terrainGrass;
			plot:SetPlotType(PlotTypes.PLOT_LAND, false, false);
		end
		return val;
	end

	-- Check to see if plot is in center region, which is Polar.
	if iX >= centWestX and iX <= centEastX and iY >= centSouthY and iY <= centNorthY then
		-- Mostly Tundra, but a smattering of snow or plains.
		local desertVal = self.deserts:GetHeight(iX, iY);
		local plainsVal = self.plains:GetHeight(iX, iY);
		if desertVal >= self.deserts:GetHeight(95) then
			terrainVal = self.terrainSnow;
		elseif plainsVal >= self.plains:GetHeight(88) then
			terrainVal = self.terrainPlains;
		else
			terrainVal = self.terrainTundra;
		end
	-- Begin implementation of User Input for dominant terrain type (Ring.lua)
	elseif dominant_terrain == 2 then -- Plains
		-- Mostly Plains, but a smattering of grass or desert.
		local desertVal = self.deserts:GetHeight(iX, iY);
		local plainsVal = self.plains:GetHeight(iX, iY);
		if desertVal >= self.deserts:GetHeight(95) then
			terrainVal = self.terrainDesert;
		elseif plainsVal <= self.plains:GetHeight(90) then
			terrainVal = self.terrainPlains;
		end
	elseif dominant_terrain == 4 then -- Jungle
		-- All grass all the time!
	else -- Grassland / Forest
		local plainsVal = self.plains:GetHeight(iX, iY);
		if plainsVal >= self.plains:GetHeight(90) then
			terrainVal = self.terrainPlains;
		end
	end

	return terrainVal;
end
------------------------------------------------------------------------------
function GenerateTerrain()
	print("Generating Terrain (Lua Ring) ...");

	local terraingen = TerrainGenerator.Create();

	local terrainTypes = terraingen:GenerateTerrain();

	SetTerrainTypes(terrainTypes);
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function FeatureGenerator:__initFractals()
	local width = self.iGridW;
	local height = self.iGridH;

	-- Create fractals
	self.jungles = Fractal.Create(width, height, self.jungle_grain, self.fractalFlags, self.fracXExp, self.fracYExp);
	self.forests = Fractal.Create(width, height, self.forest_grain, self.fractalFlags, self.fracXExp, self.fracYExp);
	self.forestclumps = Fractal.Create(width, height, self.clump_grain, self.fractalFlags, self.fracXExp, self.fracYExp);
	self.marsh = Fractal.Create(width, height, 4, self.fractalFlags, self.fracXExp, self.fracYExp);

	-- Get heights
	self.iJungleBottom = self.jungles:GetHeight((100 - self.iJunglePercent) / 2);
	self.iJungleTop = self.jungles:GetHeight((100 + self.iJunglePercent) / 2);
	self.iJungleRange = (self.iJungleTop - self.iJungleBottom) * self.iJungleFactor;
	self.iForestLevel = self.forests:GetHeight(80); -- 20% forest coverage
	self.iClumpLevel = self.forestclumps:GetHeight(94); -- 6% forest clumps
	self.iMarshLevel = self.marsh:GetHeight(100 - self.fMarshPercent);

	if dominant_terrain == 3 then -- Forest
		self.iClumpLevel = self.forestclumps:GetHeight(65); -- 35% forest clumps
		self.iForestLevel = self.forests:GetHeight(55); -- 45% forest coverage of what isn't covered by clumps.
	end
end
------------------------------------------------------------------------------
function FeatureGenerator:AddIceAtPlot()
	-- No ice.
end
------------------------------------------------------------------------------
function FeatureGenerator:AddJunglesAtPlot(plot, iX, iY)
	if dominant_terrain == 4 then -- Jungle
		if iX < centWestX or iX > centEastX or iY < centSouthY or iY > centNorthY then
			local jungle_height = self.jungles:GetHeight(iX, iY);
			if jungle_height <= self.jungles:GetHeight(90) and jungle_height >= self.jungles:GetHeight(15) then
				if plot:IsFlatlands() or plot:IsHills() then
					plot:SetFeatureType(self.featureJungle);
				end
			end
		end
	end
end
------------------------------------------------------------------------------
function FeatureGenerator:AdjustTerrainTypes()
	-- Edited in VP: only convert half of jungle tiles into plains
	local iW, iH = Map.GetGridSize();
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local plot = Map.GetPlot(x, y);
			local featureType = plot:GetFeatureType();
			if featureType == self.featureJungle then
				if Map.Rand(2, "Convert Jungle into Plains - LUA") == 1 then
					plot:SetTerrainType(self.terrainPlains, false, true);
				end
			end
		end
	end
end
------------------------------------------------------------------------------
function AddFeatures()
	print("Adding Features (Lua Ring) ...");

	local featuregen = FeatureGenerator.Create();

	featuregen:AddFeatures(false); -- Remove mountains from coasts.
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function AssignStartingPlots:GenerateRegions()
	print("Map Generation - Dividing the map in to Regions - Lua Ring");
	self.resBalance = true;
	self.method = RegionDivision.BIGGEST_LANDMASS;

	self.iNumCivs, self.iNumCityStates, self.player_ID_list, self.bTeamGame, self.teams_with_major_civs, self.number_civs_per_team = GetPlayerAndTeamInfo();
	if self.iNumCityStates > 5 then
		self.iNumCityStates = 5;
	end
	self.iNumCityStatesUnassigned = self.iNumCityStates;

	local iNumCivs = 0;
	for i = 0, MAX_MAJOR_CIVS - 1 do
		local player = Players[i];
		if player:IsEverAlive() then
			iNumCivs = iNumCivs + 1;
		end
	end

	-- Set up region data for Ring.
	-- (Regions are pre-defined, rather than the usual method of recursive division used in most scripts).
	for region_number = 1, iNumCivs do
		local this_regions_data = region_coords_data[region_number];
		local iWestX = this_regions_data[1];
		local iEastX = this_regions_data[2];
		local iSouthY = this_regions_data[3];
		local iNorthY = this_regions_data[4];
		local iWidth = iEastX - iWestX + 1;
		local iHeight = iNorthY - iSouthY + 1;

		local _, fertCount, plotCount = self:MeasureStartPlacementFertilityInRectangle(iWestX, iSouthY, iWidth, iHeight);
		local fAverageFertility = fertCount / plotCount;
		local rect_table = {iWestX, iSouthY, iWidth, iHeight, -1, fertCount, plotCount, fAverageFertility};
		table.insert(self.regionData, rect_table);
	end

	--[[ Printout is for debugging only. Deactivate otherwise.
	local tempRegionData = self.regionData;
	for i, data in ipairs(tempRegionData) do
		print("-");
		print("Data for Start Region #", i);
		print("WestX:", data[1]);
		print("SouthY:", data[2]);
		print("Width:", data[3]);
		print("Height:", data[4]);
		print("AreaID:", data[5]);
		print("Fertility:", data[6]);
		print("Plots:", data[7]);
		print("Fert/Plot:", data[8]);
		print("-");
	end
	--]]
end
------------------------------------------------------------------------------
function AssignStartingPlots:AssignCityStatesToRegionsOrToUninhabited()
	-- Assign to uninhabited landmasses
	local iW, _ = Map.GetGridSize();

	for x = centWestX, centEastX do
		for y = centSouthY, centNorthY do
			local plotIndex = y * iW + x + 1;
			local plot = Map.GetPlot(x, y);
			local plotType = plot:GetPlotType();
			if (plotType == PlotTypes.PLOT_LAND or plotType == PlotTypes.PLOT_HILLS) and self:CanPlaceCityStateAt(x, y, plot:GetLandmass(), false, false) then -- Habitable land plot, process it.
				if self.plotDataIsCoastal[plotIndex] then
					table.insert(self.uninhabited_areas_coastal_plots, plotIndex);
				else
					table.insert(self.uninhabited_areas_inland_plots, plotIndex);
				end
			end
		end
	end
end
------------------------------------------------------------------------------
function ExtraRingCheck(x, y)
	-- Adding this check for Ring in AssignStartingPlots:CanPlaceCityStateAt, to force all City States to the polar region in the center.
	return x > centWestX and x < centEastX and y > centSouthY and y < centNorthY;
end
------------------------------------------------------------------------------
function StartPlotSystem()
	print("Creating start plot database (MapGenerator.Lua)");
	local start_plot_database = AssignStartingPlots.Create();

	start_plot_database.oldCanPlaceCityStateAt = start_plot_database.CanPlaceCityStateAt;
	local newCanPlaceCityStateAt = function (ASP, x, y, area_ID, force_it, ignore_collisions)
		if not ExtraRingCheck(x, y) then
			return false;
		end
		return ASP:oldCanPlaceCityStateAt(x, y, area_ID, force_it, ignore_collisions);
	end
	start_plot_database.CanPlaceCityStateAt = newCanPlaceCityStateAt;

	print("Dividing the map in to Regions (Lua Ring)"); -- Custom for Ring
	start_plot_database:GenerateRegions();

	print("Choosing start locations for civilizations (MapGenerator.Lua)");
	start_plot_database:ChooseLocations();

	print("Normalizing start locations and assigning them to Players (MapGenerator.Lua)");
	start_plot_database:BalanceAndAssign();

	print("Placing Natural Wonders (MapGenerator.Lua)");
	start_plot_database:PlaceNaturalWonders();

	print("Placing Resources and City States (MapGenerator.Lua)");
	start_plot_database:PlaceResourcesAndCityStates();
end
------------------------------------------------------------------------------
