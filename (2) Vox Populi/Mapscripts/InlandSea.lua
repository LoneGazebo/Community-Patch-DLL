------------------------------------------------------------------------------
--	FILE:	 Inland_Sea.lua
--	AUTHOR:  Bob Thomas
--	PURPOSE: Regional map script - Loosely simulates a Mediterranean type
--	         temperate zone with civs ringing a central sea.
------------------------------------------------------------------------------
--	Copyright (c) 2010 Firaxis Games, Inc. All rights reserved.
------------------------------------------------------------------------------

include("MapGenerator");
include("MultilayeredFractal");
include("FeatureGenerator");
include("TerrainGenerator");

------------------------------------------------------------------------------
function GetMapScriptInfo()
	local world_age, temperature, rainfall, _, resources = GetCoreMapOptions();
	return {
		Name = "TXT_KEY_MAP_INLAND_SEA_VP",
		Description = "TXT_KEY_MAP_INLAND_SEA_HELP",
		IsAdvancedMap = false,
		IconIndex = 12,
		CustomOptions = {world_age, temperature, rainfall, resources},
	};
end
------------------------------------------------------------------------------

-------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	-- This function can reset map grid sizes or world wrap settings.

	-- Inland Sea is a regional script, nearly all land, all fertile, so use grid sizes two levels below normal.
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

	-- This implementation is specific to Inland Sea.
	local iW, iH = Map.GetGridSize();

	-- Fill all rows with land plots.
	self.wholeworldPlotTypes = table.fill(PlotTypes.PLOT_LAND, iW * iH);

	-- Generate the inland sea.
	local iWestX = math.floor(iW * 0.18) - 1;
	local iEastX = math.ceil(iW * 0.82) - 1;
	local iWidth = iEastX - iWestX;
	local iSouthY = math.floor(iH * 0.28) - 1;
	local iNorthY = math.ceil(iH * 0.72) - 1;
	local iHeight = iNorthY - iSouthY;
	local fracFlags = {FRAC_POLAR = true};
	local grain = 1 + Map.Rand(2, "Inland Sea ocean grain - LUA");
	local seaFrac = Fractal.Create(iWidth, iHeight, grain, fracFlags, -1, -1);
	local seaThreshold = seaFrac:GetHeight(47);

	for region_y = 0, iHeight - 1 do
		for region_x = 0, iWidth - 1 do
			local val = seaFrac:GetHeight(region_x, region_y);
			if val >= seaThreshold then
				local x = region_x + iWestX;
				local y = region_y + iSouthY;
				local i = y * iW + x + 1; -- add one because Lua arrays start at 1
				self.wholeworldPlotTypes[i] = PlotTypes.PLOT_OCEAN;
			end
		end
	end

	-- Second, oval layer to ensure one main body of water.
	local centerX = iW / 2 - 1;
	local centerY = iH / 2 - 1;
	local xAxis = centerX / 2;
	local yAxis = centerY * 0.35;
	local xAxisSquared = xAxis * xAxis;
	local yAxisSquared = yAxis * yAxis;
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local i = y * iW + x + 1;
			local deltaX = x - centerX;
			local deltaY = y - centerY;
			local deltaXSquared = deltaX * deltaX;
			local deltaYSquared = deltaY * deltaY;
			local oval_value = deltaXSquared / xAxisSquared + deltaYSquared / yAxisSquared;
			if oval_value <= 1 then
				self.wholeworldPlotTypes[i] = PlotTypes.PLOT_OCEAN;
			end
		end
	end

	-- Land and water are set. Now apply hills and mountains.
	local world_age = Map.GetCustomOption(1);
	if world_age == 4 then
		world_age = 1 + Map.Rand(3, "Random World Age - Lua");
	end
	self:ApplyTectonics{world_age = world_age};

	-- Plot Type generation completed. Return global plot array.
	return self.wholeworldPlotTypes;
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Setting Plot Types (Lua Inland Sea) ...");

	local layered_world = MultilayeredFractal.Create();
	local plotsIS = layered_world:GeneratePlotsByRegion();

	SetPlotTypes(plotsIS);

	GenerateCoasts();
end
----------------------------------------------------------------------------------
function TerrainGenerator:GetLatitudeAtPlot(iX, iY)
	local lat = math.abs(self.iHeight / 2 - iY) / (self.iHeight / 2);
	lat = lat + (128 - self.variation:GetHeight(iX, iY)) / (255.0 * 5.0);
	lat = math.clamp(lat, 0, 1);

	-- For Inland Sea only, adjust latitude to cut out Tundra and most Jungle.
	local adjusted_lat = 0.07 + 0.52 * lat;

	return adjusted_lat;
end
------------------------------------------------------------------------------
function GenerateTerrain()
	print("Generating Terrain (Lua Inland Sea) ...");

	-- Get Temperature setting input by user.
	local temp = Map.GetCustomOption(2)
	if temp == 4 then
		temp = 1 + Map.Rand(3, "Random Temperature - Lua");
	end

	local terraingen = TerrainGenerator.Create{temperature = temp};

	local terrainTypes = terraingen:GenerateTerrain();

	SetTerrainTypes(terrainTypes);
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function GetRiverValueAtPlot(plot)
	-- Custom method to force rivers to flow toward the map center.
	local iW, iH = Map.GetGridSize();
	local x = plot:GetX();
	local y = plot:GetY();
	local random_factor = Map.Rand(3, "River direction random factor - Inland Sea LUA");
	local direction_influence_value = (math.abs(x - iW / 2) + math.abs(y - iH / 2)) * random_factor;

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
function AddRivers()
	-- Customization for Inland Sea, to keep river starts away from map edges and set river "original flow direction".
	local iW, iH = Map.GetGridSize();
	print("Inland Sea - Adding Rivers");
	local passConditions = {
		function (plot)
			return plot:IsHills() or plot:IsMountain();
		end,

		function (plot)
			return not plot:IsCoastalLand() and Map.Rand(8, "MapGenerator AddRivers") == 0;
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
									if start_x < iW / 3 then -- SW Corner
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_NORTHEAST;
									elseif start_x > iW * 0.66 then -- SE Corner
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_NORTHWEST;
									else -- South, middle
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_NORTH;
									end
								else -- North half of map
									if start_x < iW / 3 then -- NW corner
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST;
									elseif start_x > iW * 0.66 then -- NE corner
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST;
									else -- North, middle
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_SOUTH;
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

------------------------------------------------------------------------------
function FeatureGenerator:GetLatitudeAtPlot(_, iY)
	local lat = math.abs(self.iGridH / 2 - iY) / (self.iGridH / 2);

	-- For Inland Sea only, adjust latitude to cut out Tundra and most Jungle.
	local adjusted_lat = 0.07 + 0.52 * lat;

	return adjusted_lat;
end
------------------------------------------------------------------------------
function FeatureGenerator:AddIceAtPlot()
	-- No ice on this map
end
------------------------------------------------------------------------------
function AddFeatures()
	print("Adding Features (Lua Inland Sea) ...");

	-- Get Rainfall setting input by user.
	local rain = Map.GetCustomOption(3);
	if rain == 4 then
		rain = 1 + Map.Rand(3, "Random Rainfall - Lua");
	end

	local featuregen = FeatureGenerator.Create{rainfall = rain};

	-- False parameter removes mountains from coastlines.
	featuregen:AddFeatures(false);
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function StartPlotSystem()
	-- Get Resources setting input by user.
	local res = Map.GetCustomOption(4);
	if res == 6 then
		res = 1 + Map.Rand(3, "Random Resources Option - Lua");
	end

	print("Creating start plot database (MapGenerator.Lua)");
	local start_plot_database = AssignStartingPlots.Create();

	print("Dividing the map in to Regions (Lua Inland Sea)");
	-- Regional Division Method 1: Biggest Landmass
	local args = {
		method = 1,
		resources = res,
	};
	start_plot_database:GenerateRegions(args);

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
