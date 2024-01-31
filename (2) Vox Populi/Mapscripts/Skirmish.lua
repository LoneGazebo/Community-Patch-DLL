------------------------------------------------------------------------------
--	FILE:	 Skirmish.lua
--	AUTHOR:  Bob Thomas (Sirian)
--	PURPOSE: Optimized for 1v1 or two-teams multiplayer.
------------------------------------------------------------------------------
--	Copyright (c) 2010 Firaxis Games, Inc. All rights reserved.
------------------------------------------------------------------------------

include("MapGenerator");
include("MultilayeredFractal");
include("FeatureGenerator");
include("TerrainGenerator");
include("MapmakerUtilities");

local dominant_terrain, userInputWaterSetting;
local iNumTeams;
local teamWestID, teamEastID;
local iNumCivsInWest, iNumCivsInEast;

------------------------------------------------------------------------------
function GetMapScriptInfo()
	return {
		Name = "TXT_KEY_MAP_SKIRMISH_VP",
		Description = "TXT_KEY_MAP_SKIRMISH_HELP",
		IconAtlas = "WORLDTYPE_ATLAS_2",
		IconIndex = 0,
		DefaultCityStates = 0,
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
					{"TXT_KEY_MAP_SCRIPT_SKIRMISH_DESERT", "TXT_KEY_MAP_SCRIPT_SKIRMISH_DESERT_HELP"},
					{"TXT_KEY_MAP_SCRIPT_SKIRMISH_TUNDRA", "TXT_KEY_MAP_SCRIPT_SKIRMISH_TUNDRA_HELP"},
					{"TXT_KEY_MAP_SCRIPT_SKIRMISH_HILLS", "TXT_KEY_MAP_SCRIPT_SKIRMISH_HILLS_HELP"},
					{"TXT_KEY_MAP_OPTION_GLOBAL_CLIMATE", "TXT_KEY_MAP_OPTION_GLOBAL_CLIMATE_HELP"},
					"TXT_KEY_MAP_OPTION_RANDOM",
				},
				DefaultValue = 9,
				SortPriority = 1,
			},
			{
				Name = "TXT_KEY_MAP_OPTION_WATER_SETTING",
				Values = {
					{"TXT_KEY_MAP_OPTION_RIVERS", "TXT_KEY_MAP_OPTION_RIVERS_HELP"},
					{"TXT_KEY_MAP_OPTION_SMALL_LAKES", "TXT_KEY_MAP_OPTION_SMALL_LAKES_HELP"},
					{"TXT_KEY_MAP_OPTION_SEAS", "TXT_KEY_MAP_OPTION_SEAS_HELP"},
					{"TXT_KEY_MAP_OPTION_RIVERS_AND_SEAS", "TXT_KEY_MAP_OPTION_RIVERS_AND_SEAS_HELP"},
					{"TXT_KEY_MAP_OPTION_DRY", "TXT_KEY_MAP_OPTION_DRY_HELP"},
					"TXT_KEY_MAP_OPTION_RANDOM",
				},
				DefaultValue = 1,
				SortPriority = 2,
			},
			{
				Name = "TXT_KEY_MAP_OPTION_RESOURCES",
				Values = { -- Only one option here, but this will let all users know that resources are not set at default.
					"TXT_KEY_MAP_OPTION_BALANCED_RESOURCES",
				},
				DefaultValue = 1,
				SortPriority = 3,
			},
		},
	};
end
------------------------------------------------------------------------------

-------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	-- This function can reset map grid sizes or world wrap settings.

	-- Skirmish is a world without oceans, so use grid sizes two levels below normal.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {24, 16},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {32, 20},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {40, 24},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {52, 32},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {64, 40},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {84, 52},
	};
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

-------------------------------------------------------------------------------
function MultilayeredFractal:GeneratePlotsByRegion()
	-- Sirian's MultilayeredFractal controlling function.
	-- You -MUST- customize this function for each script using MultilayeredFractal.

	-- This implementation is specific to Skirmish.
	local iW, iH = Map.GetGridSize();
	local fracFlags = {FRAC_WRAP_X = true, FRAC_POLAR = true};
	self.wholeworldPlotTypes = table.fill(PlotTypes.PLOT_LAND, iW * iH);

	-- Get user inputs.
	dominant_terrain = Map.GetCustomOption(1); -- GLOBAL variable.
	if dominant_terrain == 9 then -- Random
		dominant_terrain = 1 + Map.Rand(8, "Random Type of Dominant Terrain - Skirmish LUA");
	end
	userInputWaterSetting = Map.GetCustomOption(2); -- GLOBAL variable.
	if userInputWaterSetting == 6 then -- Random
		userInputWaterSetting = 1 + Map.Rand(5, "Random Water Setting - Skirmish LUA");
	end

	-- Lake density: applies only to Small Lakes and Seas settings.
	if userInputWaterSetting >= 2 and userInputWaterSetting <= 4 then
		local lake_list = {0, 93, 85, 85};
		local lake_grains = {0, 5, 3, 3};
		local lakes = lake_list[userInputWaterSetting];
		local lake_grain = lake_grains[userInputWaterSetting];

		local lakesFrac = Fractal.Create(iW, iH, lake_grain, fracFlags, -1, -1);
		local iLakesThreshold = lakesFrac:GetHeight(lakes);

		for y = 1, iH - 2 do
			for x = 0, iW - 1 do
				local i = y * iW + x + 1; -- add one because Lua arrays start at 1
				local lakeVal = lakesFrac:GetHeight(x, y);
				if lakeVal >= iLakesThreshold then
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_OCEAN;
				end
			end
		end
	end

	-- Apply hills and mountains.
	if dominant_terrain == 7 then -- Hills dominate.
		local worldsizes = {
			[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 4,
			[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 4,
			[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 5,
			[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 5,
			[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 5,
			[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 5,
		};
		local grain = worldsizes[Map.GetWorldSize()];

		local terrainFrac = Fractal.Create(iW, iH, grain, fracFlags, -1, -1);
		local iHillsThreshold = terrainFrac:GetHeight(70);
		local iPeaksThreshold = terrainFrac:GetHeight(95);
		local iHillsClumps = terrainFrac:GetHeight(10);

		local hillsFrac = Fractal.Create(iW, iH, grain, fracFlags, -1, -1);
		local iHillsBottom1 = hillsFrac:GetHeight(20);
		local iHillsTop1 = hillsFrac:GetHeight(30);
		local iHillsBottom2 = hillsFrac:GetHeight(70);
		local iHillsTop2 = hillsFrac:GetHeight(80);

		for x = 0, iW - 1 do
			for y = 0, iH - 1 do
				local i = y * iW + x + 1;
				local val = terrainFrac:GetHeight(x, y);
				if val >= iPeaksThreshold then
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
				elseif val >= iHillsThreshold or val <= iHillsClumps then
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_HILLS;
				else
					local hillsVal = hillsFrac:GetHeight(x, y);
					if hillsVal >= iHillsBottom1 and hillsVal <= iHillsTop1 then
						self.wholeworldPlotTypes[i] = PlotTypes.PLOT_HILLS;
					elseif hillsVal >= iHillsBottom2 and hillsVal <= iHillsTop2 then
						self.wholeworldPlotTypes[i] = PlotTypes.PLOT_HILLS;
					end
				end
			end
		end
	else -- Normal hills and mountains.
		local args = {
			adjust_plates = 1.2,
		};
		self:ApplyTectonics(args);
	end

	-- Create buffer zone in middle four columns. This will create some choke points.
	-- Turn all plots in buffer zone to land.
	for x = iW / 2 - 2, iW / 2 + 1 do
		for y = 1, iH - 2 do
			local i = y * iW + x + 1;
			self.wholeworldPlotTypes[i] = PlotTypes.PLOT_LAND;
		end
	end
	-- Add mountains in top and bottom rows of buffer zone.
	for x = iW / 2 - 2, iW / 2 + 1 do
		local i = x + 1;
		self.wholeworldPlotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
		i = (iH - 1) * iW + x + 1;
		self.wholeworldPlotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
	end
	-- Add random smattering of mountains to middle two columns of buffer zone.
	local west_half, east_half = {}, {};
	for loop = 1, iH - 2 do
		table.insert(west_half, loop);
		table.insert(east_half, loop);
	end
	local west_shuffled = GetShuffledCopyOfTable(west_half);
	local east_shuffled = GetShuffledCopyOfTable(east_half);
	local iNumMountainsPerColumn = math.max(math.floor(iH * 0.225), math.floor((iH / 4) - 1));
	local x_west, x_east = iW / 2 - 1, iW / 2;
	for loop = 1, iNumMountainsPerColumn do
		local y_west, y_east = west_shuffled[loop], east_shuffled[loop];
		local i_west_plot = y_west * iW + x_west + 1;
		local i_east_plot = y_east * iW + x_east + 1;
		self.wholeworldPlotTypes[i_west_plot] = PlotTypes.PLOT_MOUNTAIN;
		self.wholeworldPlotTypes[i_east_plot] = PlotTypes.PLOT_MOUNTAIN;
	end
	-- Hills need to be added near mountains, but this needs to wait until after plot types have been initially set.

	-- Plot Type generation completed. Return global plot array.
	return self.wholeworldPlotTypes;
end
------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Setting Plot Types (Lua Skirmish) ...");

	local layered_world = MultilayeredFractal.Create();
	local plotsSkirmish = layered_world:GeneratePlotsByRegion();

	SetPlotTypes(plotsSkirmish);

	-- Examine all plots in buffer zone.
	local iW, iH = Map.GetGridSize();
	local firstRingYIsEven = {{0, 1}, {1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};
	local firstRingYIsOdd = {{1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, 0}, {0, 1}};
	for x = iW / 2 - 2, iW / 2 + 1 do
		for y = 1, iH - 2 do
			local plot = Map.GetPlot(x, y);
			if plot:IsFlatlands() then -- Check for adjacent Mountain plot; if found, change this plot to Hills.
				local isEvenY, search_table = true, {};
				if y / 2 > math.floor(y / 2) then
					isEvenY = false;
				end
				if isEvenY then
					search_table = firstRingYIsEven;
				else
					search_table = firstRingYIsOdd;
				end

				for _, plot_adjustments in ipairs(search_table) do
					local searchX, searchY;
					searchX = x + plot_adjustments[1];
					searchY = y + plot_adjustments[2];
					local searchPlot = Map.GetPlot(searchX, searchY);
					local plotType = searchPlot:GetPlotType();
					if plotType == PlotTypes.PLOT_MOUNTAIN then
						plot:SetPlotType(PlotTypes.PLOT_HILLS, false, false);
						break;
					end
				end
			end
		end
	end

	GenerateCoasts();
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function TerrainGenerator:GenerateTerrainAtPlot(iX,iY)
	local lat = self:GetLatitudeAtPlot(iX,iY);
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

	-- Begin implementation of User Input for dominant terrain type (Skirmish.lua)
	if dominant_terrain == 2 then -- Plains
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
	elseif dominant_terrain == 5 then -- Desert
		local desertVal = self.deserts:GetHeight(iX, iY);
		if desertVal <= self.deserts:GetHeight(85) then
			terrainVal = self.terrainDesert;
		else
			terrainVal = self.terrainPlains;
		end
	elseif dominant_terrain == 6 then -- Tundra
		local desertVal = self.deserts:GetHeight(iX, iY);
		local plainsVal = self.plains:GetHeight(iX, iY);
		if plainsVal >= self.plains:GetHeight(85) then
			terrainVal = self.terrainPlains;
		elseif desertVal >= self.deserts:GetHeight(94) then
			terrainVal = self.terrainSnow;
		else
			terrainVal = self.terrainTundra;
		end
	elseif dominant_terrain == 8 then -- Global (aka normal climate bands)
		if lat >= self.fSnowLatitude then
			terrainVal = self.terrainSnow;
		elseif lat >= self.fTundraLatitude then
			terrainVal = self.terrainTundra;
		elseif lat < self.fGrassLatitude then
			terrainVal = self.terrainGrass;
		else
			local desertVal = self.deserts:GetHeight(iX, iY);
			local plainsVal = self.plains:GetHeight(iX, iY);
			if desertVal >= self.iDesertBottom and desertVal <= self.iDesertTop and lat >= self.fDesertBottomLatitude and lat < self.fDesertTopLatitude then
				terrainVal = self.terrainDesert;
			elseif plainsVal >= self.iPlainsBottom and plainsVal <= self.iPlainsTop then
				terrainVal = self.terrainPlains;
			end
		end
	else -- Grassland / Forest / Hills
		local plainsVal = self.plains:GetHeight(iX, iY);
		if plainsVal >= self.plains:GetHeight(90) then
			terrainVal = self.terrainPlains;
		end
	end

	return terrainVal;
end
------------------------------------------------------------------------------
function GenerateTerrain()
	print("Generating Terrain (Lua Skirmish) ...");

	local terraingen = TerrainGenerator.Create();

	local terrainTypes = terraingen:GenerateTerrain();

	SetTerrainTypes(terrainTypes);
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function GetRiverValueAtPlot(plot)
	-- Custom method to force rivers to flow away from the map center.
	local iW, iH = Map.GetGridSize();
	local x = plot:GetX();
	local y = plot:GetY();
	local random_factor = Map.Rand(3, "River direction random factor - Skirmish LUA");
	local direction_influence_value = (math.abs(iW - (x - iW / 2)) + math.abs(y - iH / 2) / 3) * random_factor;

	local numPlots = PlotTypes.NUM_PLOT_TYPES;
	local sum = (numPlots - plot:GetPlotType()) * 20 + direction_influence_value;

	local numDirections = DirectionTypes.NUM_DIRECTION_TYPES;
	for direction = 0, numDirections - 1 do
		local adjacentPlot = Map.PlotDirection(plot:GetX(), plot:GetY(), direction);
		if adjacentPlot then
			sum = sum + (numPlots - adjacentPlot:GetPlotType());
		else
			sum = sum + numPlots * 10;
		end
	end
	sum = sum + Map.Rand(10, "River Rand");

	return sum;
end
------------------------------------------------------------------------------
function DoRiver(startPlot, thisFlowDirection, originalFlowDirection, riverID)
	-- Customizing to handle problems in top row of the map. Only this aspect has been altered.

	local _, iH = Map.GetGridSize();
	thisFlowDirection = thisFlowDirection or FlowDirectionTypes.NO_FLOWDIRECTION;
	originalFlowDirection = originalFlowDirection or FlowDirectionTypes.NO_FLOWDIRECTION;

	-- pStartPlot = the plot at whose SE corner the river is starting
	if not riverID then
		riverID = nextRiverID;
		nextRiverID = nextRiverID + 1;
	end

	local otherRiverID = _rivers[startPlot];
	if otherRiverID and otherRiverID ~= riverID and originalFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION then
		return; -- Another river already exists here; can't branch off of an existing river!
	end

	local riverPlot;

	local bestFlowDirection = FlowDirectionTypes.NO_FLOWDIRECTION;
	if thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTH then
		riverPlot = startPlot;
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_EAST);
		if not adjacentPlot or riverPlot:IsWOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater() then
			return;
		end
		_rivers[riverPlot] = riverID;
		riverPlot:SetWOfRiver(true, thisFlowDirection);
		riverPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_NORTHEAST);
	elseif thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHEAST then
		riverPlot = startPlot;
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_SOUTHEAST);
		if not adjacentPlot or riverPlot:IsNWOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater() then
			return;
		end
		_rivers[riverPlot] = riverID;
		riverPlot:SetNWOfRiver(true, thisFlowDirection);
		-- riverPlot does not change
	elseif thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST then
		riverPlot = Map.PlotDirection(startPlot:GetX(), startPlot:GetY(), DirectionTypes.DIRECTION_EAST);
		if not riverPlot then
			return;
		end
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_SOUTHWEST);
		if not adjacentPlot or riverPlot:IsNEOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater() then
			return;
		end
		_rivers[riverPlot] = riverID;
		riverPlot:SetNEOfRiver(true, thisFlowDirection);
		-- riverPlot does not change
	elseif thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTH then
		riverPlot = Map.PlotDirection(startPlot:GetX(), startPlot:GetY(), DirectionTypes.DIRECTION_SOUTHWEST);
		if not riverPlot then
			return;
		end
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_EAST);
		if not adjacentPlot or riverPlot:IsWOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater() then
			return;
		end
		_rivers[riverPlot] = riverID;
		riverPlot:SetWOfRiver(true, thisFlowDirection);
		-- riverPlot does not change
	elseif thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST then
		riverPlot = startPlot;
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_SOUTHEAST);
		if not adjacentPlot or riverPlot:IsNWOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater() then
			return;
		end
		_rivers[riverPlot] = riverID;
		riverPlot:SetNWOfRiver(true, thisFlowDirection);
		-- riverPlot does not change
	elseif thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHWEST then
		riverPlot = startPlot;
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_SOUTHWEST);
		if not adjacentPlot or riverPlot:IsNEOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater() then
			return;
		end
		_rivers[riverPlot] = riverID;
		riverPlot:SetNEOfRiver(true, thisFlowDirection);
		riverPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_WEST);
	else
		-- River is starting here, set the direction in the next step
		riverPlot = startPlot;
	end

	if not riverPlot or riverPlot:IsWater() then
		-- The river has flowed off the edge of the map or into the ocean. All is well.
		return;
	end

	-- Storing X,Y positions as locals to prevent redundant function calls.
	local riverPlotX = riverPlot:GetX();
	local riverPlotY = riverPlot:GetY();

	-- Table of methods used to determine the adjacent plot.
	local adjacentPlotFunctions = {
		[FlowDirectionTypes.FLOWDIRECTION_NORTH] = function ()
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_NORTHWEST);
		end,

		[FlowDirectionTypes.FLOWDIRECTION_NORTHEAST] = function ()
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_NORTHEAST);
		end,

		[FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST] = function ()
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_EAST);
		end,

		[FlowDirectionTypes.FLOWDIRECTION_SOUTH] = function ()
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_SOUTHWEST);
		end,

		[FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST] = function ()
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_WEST);
		end,

		[FlowDirectionTypes.FLOWDIRECTION_NORTHWEST] = function ()
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_NORTHWEST);
		end,
	};

	if bestFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION then
		-- Attempt to calculate the best flow direction.
		local bestValue = math.huge;
		for flowDirection, getAdjacentPlot in pairs(adjacentPlotFunctions) do
			if GetOppositeFlowDirection(flowDirection) ~= originalFlowDirection then
				if thisFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION or
					flowDirection == TurnRightFlowDirections[thisFlowDirection] or
					flowDirection == TurnLeftFlowDirections[thisFlowDirection] then

					local adjacentPlot = getAdjacentPlot();
					if adjacentPlot then
						local value = GetRiverValueAtPlot(adjacentPlot);
						if flowDirection == originalFlowDirection then
							value = (value * 3) / 4;
						end
						if value < bestValue then
							bestValue = value;
							bestFlowDirection = flowDirection;
						end
					elseif not adjacentPlot and riverPlotY == iH - 1 then
						 -- Top row of map, needs special handling
						if flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTH or
							flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHWEST or
							flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHEAST then

							local value = Map.Rand(5, "River Rand");
							if (flowDirection == originalFlowDirection) then
								value = (value * 3) / 4;
							end
							if (value < bestValue) then
								bestValue = value;
								bestFlowDirection = flowDirection;
							end
						end
					elseif not adjacentPlot and riverPlotX == 0 then
						-- Left column of map, needs special handling
						if flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTH or
							flowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTH or
							flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHWEST or
							flowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST then

							local value = Map.Rand(5, "River Rand");
							if flowDirection == originalFlowDirection then
								value = (value * 3) / 4;
							end
							if value < bestValue then
								bestValue = value;
								bestFlowDirection = flowDirection;
							end
						end
					end
				end
			end
		end

		-- Try a second pass allowing the river to "flow backwards".
		if bestFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION then
			bestValue = math.huge;
			for flowDirection, getAdjacentPlot in pairs(adjacentPlotFunctions) do
				if thisFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION or
					flowDirection == TurnRightFlowDirections[thisFlowDirection] or
					flowDirection == TurnLeftFlowDirections[thisFlowDirection] then

					local adjacentPlot = getAdjacentPlot();
					if adjacentPlot then
						local value = GetRiverValueAtPlot(adjacentPlot);
						if value < bestValue then
							bestValue = value;
							bestFlowDirection = flowDirection;
						end
					end
				end
			end
		end
	end

	-- Recursively generate river.
	if bestFlowDirection ~= FlowDirectionTypes.NO_FLOWDIRECTION then
		if originalFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION then
			originalFlowDirection = bestFlowDirection;
		end
		DoRiver(riverPlot, bestFlowDirection, originalFlowDirection, riverID);
	end
end
------------------------------------------------------------------------------
function AddRivers()
	-- Only add rivers if Water Setting is value of 1 or 4. Otherwise no rivers.
	if userInputWaterSetting == 2 or userInputWaterSetting == 3 or userInputWaterSetting == 5 then -- No Rivers!
		return;
	end

	-- Customization for Skirmish, to keep river starts away from buffer zone in middle columns of map, and set river "original flow direction".
	local iW, iH = Map.GetGridSize();
	print("Skirmish - Adding Rivers");
	local passConditions = {
		function (plot)
			return plot:IsHills() or plot:IsMountain();
		end,

		function (plot)
			return (not plot:IsCoastalLand()) and (Map.Rand(8, "MapGenerator AddRivers") == 0);
		end,

		function (plot)
			local area = plot:Area();
			local plotsPerRiverEdge = GameDefines.PLOTS_PER_RIVER_EDGE;
			return (plot:IsHills() or plot:IsMountain()) and area:GetNumRiverEdges() < area:GetNumTiles() / plotsPerRiverEdge + 1;
		end,

		function (plot)
			local area = plot:Area();
			local plotsPerRiverEdge = GameDefines.PLOTS_PER_RIVER_EDGE;
			return area:GetNumRiverEdges() < area:GetNumTiles() / plotsPerRiverEdge + 1;
		end,
	};
	for iPass, passCondition in ipairs(passConditions) do
		local riverSourceRange;
		local seaWaterRange;
		if iPass <= 2 then
			riverSourceRange = GameDefines.RIVER_SOURCE_MIN_RIVER_RANGE;
			seaWaterRange = GameDefines.RIVER_SOURCE_MIN_SEAWATER_RANGE;
		else
			riverSourceRange = GameDefines.RIVER_SOURCE_MIN_RIVER_RANGE / 2;
			seaWaterRange = GameDefines.RIVER_SOURCE_MIN_SEAWATER_RANGE / 2;
		end
		for _, plot in Plots() do
			local current_x = plot:GetX();
			local current_y = plot:GetY();
			if current_x < 1 or current_x >= iW - 2 or current_y < 2 or current_y >= iH - 1 then
				-- Plot too close to edge, ignore it.
			elseif current_x >= iW / 2 - 2 and current_x <= iW / 2 + 1 then
				-- Plot in buffer zone, ignore it.
			elseif not plot:IsWater() then
				if passCondition(plot) then
					if not Map.FindWater(plot, riverSourceRange, true) then
						if not Map.FindWater(plot, seaWaterRange, false) then
							local inlandCorner = plot:GetInlandCorner();
							if inlandCorner then
								local start_x = inlandCorner:GetX();
								local start_y = inlandCorner:GetY();
								local orig_direction;
								if start_y < iH / 2 then -- South half of map
									if start_x < iW / 2 then -- West half of map
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_NORTHWEST;
									else -- East half
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_NORTHEAST;
									end
								else -- North half of map
									if start_x < iW / 2 then -- West half of map
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST;
									else -- NE corner
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST;
									end
								end
								DoRiver(inlandCorner, nil, orig_direction, nil);
							end
						end
					end
				end
			end
		end
	end
end
------------------------------------------------------------------------------
function AddLakes()
	-- No lakes added in this manner.
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
	elseif dominant_terrain == 5 then -- Desert
		self.iOasisPercent = 8;
		if userInputWaterSetting == 4 then
			self.iOasisPercent = 6;
		elseif userInputWaterSetting == 5 then
			self.iOasisPercent = 15;
		end
	elseif dominant_terrain == 6 then -- Tundra
		self.iClumpLevel = self.forestclumps:GetHeight(80); -- 20% forest clumps
		self.iForestLevel = self.forests:GetHeight(60); -- 40% forest coverage of what isn't covered by clumps.
	elseif dominant_terrain == 8 then -- Global
		self.iClumpLevel = self.forestclumps:GetHeight(90); -- 10% forest clumps
		self.iForestLevel = self.forests:GetHeight(69); -- 31% forest coverage of what isn't covered by clumps.
	end
end
------------------------------------------------------------------------------
function FeatureGenerator:AddIceAtPlot(plot, _, iY, lat)
	if dominant_terrain == 8 then -- Global
		if plot:CanHaveFeature(self.featureIce) then
			if Map.IsWrapX() and (iY == 0 or iY == self.iGridH - 1) then
				plot:SetFeatureType(self.featureIce);
			else
				local rand = Map.Rand(100, "Add Ice Lua") / 100.0;
				if rand < 8 * (lat - 0.875) then
					plot:SetFeatureType(self.featureIce);
				elseif rand < 4 * (lat - 0.75) then
					plot:SetFeatureType(self.featureIce);
				end
			end
		end
	end
end
------------------------------------------------------------------------------
function FeatureGenerator:AddJunglesAtPlot(plot, iX, iY, lat)
	if dominant_terrain == 4 then -- Jungle
		local jungle_height = self.jungles:GetHeight(iX, iY);
		if jungle_height <= self.jungles:GetHeight(90) and jungle_height >= self.jungles:GetHeight(15) then
			if plot:IsFlatlands() or plot:IsHills() then
				plot:SetFeatureType(self.featureJungle);
			end
		end
	elseif dominant_terrain == 8 then -- Global, use default jungle placement.
		local jungle_height = self.jungles:GetHeight(iX, iY);
		if jungle_height <= self.iJungleTop and jungle_height >= self.iJungleBottom + self.iJungleRange * lat then
			if plot:IsFlatlands() or plot:IsHills() then
				plot:SetFeatureType(self.featureJungle);
			end
		end
	else
		-- No jungles
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
	print("Adding Features (Lua Skirmish) ...");

	local featuregen = FeatureGenerator.Create();

	featuregen:AddFeatures();
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function AssignStartingPlots:GenerateRegions()
	print("Map Generation - Dividing the map in to Regions");
	-- This version is tailored for handling two-teams play.
	local iW, iH = Map.GetGridSize();
	self.method = RegionDivision.RECTANGULAR; -- Flag the map as using a Rectangular division method.

	-- Automatically set strategic resources to place a source of iron and horse near every start location.
	self.resBalance = true;

	-- Determine number of civilizations and city states present in this game.
	self.iNumCivs, self.iNumCityStates, self.player_ID_list, self.bTeamGame, self.teams_with_major_civs, self.number_civs_per_team = GetPlayerAndTeamInfo();
	self.iNumCityStatesUnassigned = self.iNumCityStates;
	print("-"); print("Civs:", self.iNumCivs); print("City States:", self.iNumCityStates);

	-- Determine number of teams (of Major Civs only, not City States) present in this game.
	iNumTeams = table.maxn(self.teams_with_major_civs); -- GLOBAL
	print("-"); print("Teams:", iNumTeams);

	-- If two teams are present, use team-oriented handling of start points, one team west, one east.
	if iNumTeams == 2 then
		print("-"); print("Number of Teams present is two! Using custom team start placement for Skirmish."); print("-");

		-- ToDo: Correctly identify team IDs and how many Civs are on each team.
		-- Also need to shuffle the teams so its random who starts on which half.
		local shuffled_team_list = GetShuffledCopyOfTable(self.teams_with_major_civs);
		teamWestID = shuffled_team_list[1];
		teamEastID = shuffled_team_list[2];
		iNumCivsInWest = self.number_civs_per_team[teamWestID];
		iNumCivsInEast = self.number_civs_per_team[teamEastID];

		-- Process the team in the west.
		self.inhabited_WestX = 0;
		self.inhabited_SouthY = 0;
		self.inhabited_Width = math.floor(iW / 2) - 1;
		self.inhabited_Height = iH;
		-- Obtain "Start Placement Fertility" inside the rectangle.
		-- Data returned is: fertility table, sum of all fertility, plot count.
		local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityInRectangle(
			self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height);
		-- Assemble the Rectangle data table. AreaID -1 means ignore landmass/area IDs.
		local rect_table = {self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height, -1, fertCount, plotCount};
		-- Divide the rectangle.
		self:DivideIntoRegions(iNumCivsInWest, fert_table, rect_table);

		-- Process the team in the east.
		self.inhabited_WestX = math.floor(iW / 2) + 1;
		self.inhabited_SouthY = 0;
		self.inhabited_Width = math.floor(iW / 2) - 1;
		self.inhabited_Height = iH;
		-- Obtain "Start Placement Fertility" inside the rectangle.
		-- Data returned is: fertility table, sum of all fertility, plot count.
		fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityInRectangle(
			self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height);
		-- Assemble the Rectangle data table. AreaID -1 means ignore landmass/area IDs.
		rect_table = {self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height, -1, fertCount, plotCount};
		-- Divide the rectangle.
		self:DivideIntoRegions(iNumCivsInEast, fert_table, rect_table);

	else -- Use standard division.
		-- Obtain the boundaries of the rectangle to be processed.
		self.inhabited_WestX = 0;
		self.inhabited_SouthY = 0;
		self.inhabited_Width = iW;
		self.inhabited_Height = iH;

		-- Obtain "Start Placement Fertility" inside the rectangle.
		-- Data returned is: fertility table, sum of all fertility, plot count.
		local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityInRectangle(self.inhabited_WestX,
												 self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height);
		-- Assemble the Rectangle data table:
		local rect_table = {self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width,
							self.inhabited_Height, -1, fertCount, plotCount}; -- AreaID -1 means ignore landmass/area IDs.
		-- Divide the rectangle.
		self:DivideIntoRegions(self.iNumCivs, fert_table, rect_table);
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:BalanceAndAssign()
	-- This function determines what level of Bonus Resource support a location may need, identifies compatibility with civ-specific biases, and places starts.

	-- Normalize each start plot location.
	local iNumStarts = table.maxn(self.startingPlots);
	for region_number = 1, iNumStarts do
		self:NormalizeStartLocation(region_number);
	end

	-- Assign Civs to start plots.
	if iNumTeams == 2 then
		-- Two teams, place one in the west half, other in east -- even if team membership totals are uneven.
		print("-"); print("This is a team game with two teams! Place one team in West, other in East."); print("-");
		local playerList, westList, eastList = {}, {}, {};
		for loop = 1, self.iNumCivs do
			local player_ID = self.player_ID_list[loop];
			table.insert(playerList, player_ID);
			local player = Players[player_ID];
			local team_ID = player:GetTeam()
			if team_ID == teamWestID then
				print("Player #", player_ID, "belongs to Team #", team_ID, "and will be placed in the North.");
				table.insert(westList, player_ID);
			elseif team_ID == teamEastID then
				print("Player #", player_ID, "belongs to Team #", team_ID, "and will be placed in the South.");
				table.insert(eastList, player_ID);
			else
				print("* ERROR * - Player #", player_ID, "belongs to Team #", team_ID, "which is neither West nor East!");
			end
		end

		---[[ Debug
		if table.maxn(westList) ~= iNumCivsInWest then
			print("-"); print("*** ERROR! *** . . . Mismatch between number of Civs on West team and number of civs assigned to west locations.");
		end
		if table.maxn(eastList) ~= iNumCivsInEast then
			print("-"); print("*** ERROR! *** . . . Mismatch between number of Civs on East team and number of civs assigned to east locations.");
		end
		--]]

		local westListShuffled = GetShuffledCopyOfTable(westList);
		local eastListShuffled = GetShuffledCopyOfTable(eastList);
		for region_number, player_ID in ipairs(westListShuffled) do
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];
			local start_plot = Map.GetPlot(x, y);
			local player = Players[player_ID];
			player:SetStartingPlot(start_plot);
		end
		for loop, player_ID in ipairs(eastListShuffled) do
			local x = self.startingPlots[loop + iNumCivsInWest][1];
			local y = self.startingPlots[loop + iNumCivsInWest][2];
			local start_plot = Map.GetPlot(x, y);
			local player = Players[player_ID];
			player:SetStartingPlot(start_plot);
		end
	else
		print("-"); print("This game does not have specific start zone assignments."); print("-");
		local playerList = {};
		for loop = 1, self.iNumCivs do
			local player_ID = self.player_ID_list[loop];
			table.insert(playerList, player_ID);
		end
		local playerListShuffled = GetShuffledCopyOfTable(playerList);
		for region_number, player_ID in ipairs(playerListShuffled) do
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];
			local start_plot = Map.GetPlot(x, y);
			local player = Players[player_ID];
			player:SetStartingPlot(start_plot);
		end

		-- If this is a team game (any team has more than one Civ in it) then make sure team members start near each other if possible.
		-- (This may scramble Civ biases in some cases, but there is no cure).
		if self.bTeamGame and iNumTeams ~= 2 then
			print("However, this IS a team game, so we will try to group team members together."); print("-");
			self:NormalizeTeamLocations();
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetMajorStrategicResourceQuantityValues()
	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 4, 4, 5, 6, 7, 8;
	-- Check the resource setting.
	if self.resSize == 1 then -- Sparse
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 4, 4, 4, 5, 5;
	elseif self.resSize == 3 then -- Abundant
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 4, 6, 7, 9, 10, 10;
	end
	return uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt;
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetSmallStrategicResourceQuantityValues()
	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 1, 2, 2, 2, 3, 3;
	-- Check the resource setting.
	if self.resSize == 1 then -- Sparse
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 1, 1, 1, 1, 2, 2;
	elseif self.resSize == 3 then -- Abundant
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 3, 3, 3, 3, 3;
	end
	return uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt;
end
------------------------------------------------------------------------------
function AssignStartingPlots:CustomOverride(resource_ID)
	-- CUSTOM OPERATION for Skirmish!
	-- This function will identify the "Luxury Plot Lists" that match terrain best suitable for this type of strategic resource.

	print("-"); print("Obtaining indices for Strategic#", resource_ID);

	local tList = {};

	if resource_ID == self.iron_ID then
		tList = {
			PlotListTypes.HILLS,
			PlotListTypes.FLAT_FOREST,
			PlotListTypes.FLAT_DESERT_NO_FEATURE,
			PlotListTypes.SNOW,
		};
	elseif resource_ID == self.horse_ID then
		tList = {
			PlotListTypes.FLAT_TUNDRA_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_DESERT_WET,
		};
	elseif resource_ID == self.coal_ID then
		tList = {
			PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE,
			PlotListTypes.PLAINS_GRASS_FOREST,
		};
	elseif resource_ID == self.oil_ID then
		tList = {
			PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE,
			PlotListTypes.FLAT_JUNGLE,
			PlotListTypes.COAST_NEXT_TO_LAND,
			PlotListTypes.MARSH,
			PlotListTypes.SNOW,
		};
	elseif resource_ID == self.aluminum_ID then
		tList = {
			PlotListTypes.HILLS_DESERT_TUNDRA_PLAINS_NO_FEATURE,
			PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE,
			PlotListTypes.FLAT_PLAINS_NO_FEATURE,
		};
	elseif resource_ID == self.uranium_ID then
		tList = {
			PlotListTypes.LAND,
		};
	end
	print("Found indices of", tList);
	return tList;
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceStrategicAndBonusResources()
	-- Distribute Strategic Resources to Regions -- CUSTOM for Skirmish.

	-- Adjust amounts, if applicable, based on Resource Setting.
	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = self:GetMajorStrategicResourceQuantityValues();
	local uran_sm, horse_sm, oil_sm, iron_sm, coal_sm, alum_sm = self:GetSmallStrategicResourceQuantityValues();

	local dist_list = {
		{self.iron_ID, iron_amt, iron_sm},
		{self.horse_ID, horse_amt, horse_sm},
		{self.coal_ID, coal_amt, coal_sm},
		{self.oil_ID, oil_amt, oil_sm},
		{self.aluminum_ID, alum_amt, alum_sm},
		{self.uranium_ID, uran_amt, uran_sm},
	};
	for _, reg_data in ipairs(self.regions_sorted_by_type) do
		local region_number = reg_data[1];
		for _, res_data_table in ipairs(dist_list) do
			local res_ID = res_data_table[1];
			local res_amt = res_data_table[2];
			local res_sm = res_data_table[3];

			print("-"); print("- - -"); print("Attempting to place strategic resource #", res_ID, "in Region#", region_number);

			local tList = self:CustomOverride(res_ID);
			local luxury_plot_lists = self:GenerateLuxuryPlotListsInRegion(region_number);

			-- Place one large source of each resource type in each region.
			local iNumLeftToPlace = 1;
			local iAttempt = 0;
			while iNumLeftToPlace > 0 and iAttempt < 100 do
				for _, index in ipairs(tList) do
					local shuf_list = luxury_plot_lists[index];
					if index > 0 then
						iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, res_amt, 1, 0.25, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
					end
					if iNumLeftToPlace <= 0 then
						break;
					end
				end
				iAttempt = iAttempt + 1;
			end

			-- Place one small source of each resource type in each region.
			iNumLeftToPlace = 1;
			iAttempt = 0;
			while iNumLeftToPlace > 0 and iAttempt < 100 do
				for _, index in ipairs(tList) do
					local shuf_list = luxury_plot_lists[index];
					if index > 0 then
						iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, res_sm, 1, 0.25, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
					end
					if iNumLeftToPlace <= 0 then
						break;
					end
				end
				iAttempt = iAttempt + 1;
			end
		end
	end

	self:PlaceBonusResources();
end
------------------------------------------------------------------------------
function StartPlotSystem()
	-- Custom for Skirmish.
	print("Creating start plot database.");
	local start_plot_database = AssignStartingPlots.Create();

	print("Dividing the map in to Regions.");
	start_plot_database:GenerateRegions();

	print("Choosing start locations for civilizations.");
	start_plot_database:ChooseLocations();

	print("Normalizing start locations and assigning them to Players.");
	start_plot_database:BalanceAndAssign();

	print("Placing Natural Wonders.");
	start_plot_database:PlaceNaturalWonders();

	print("Placing Resources and City States.");
	start_plot_database:PlaceResourcesAndCityStates();
end
------------------------------------------------------------------------------
