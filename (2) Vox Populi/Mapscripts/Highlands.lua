------------------------------------------------------------------------------
--	FILE:	 Highlands.lua
--	AUTHOR:  Bob Thomas (Sirian)
--	PURPOSE: Regional map script - mountainous terrain
------------------------------------------------------------------------------
--	Copyright (c) 2010 Firaxis Games, Inc. All rights reserved.
------------------------------------------------------------------------------

include("MapGenerator");
include("TerrainGenerator");
include("FeatureGenerator");

------------------------------------------------------------------------------
function GetMapScriptInfo()
	local _, temperature, rainfall, _, resources = GetCoreMapOptions();
	return {
		Name = "TXT_KEY_MAP_HIGHLANDS_VP",
		Description = "TXT_KEY_MAP_HIGHLANDS_HELP",
		IsAdvancedMap = false,
		SupportsMultiplayer = true,
		IconIndex = 10,
		CustomOptions = {
			temperature,
			rainfall,
			resources,
			{
				Name = "TXT_KEY_MAP_OPTION_MOUNTAIN_PATTERN",
				Values = {
					{"TXT_KEY_MAP_OPTION_RIDGELINES","TXT_KEY_MAP_OPTION_RIDGELINES_HELP"},
					{"TXT_KEY_MAP_OPTION_SCATTERED", "TXT_KEY_MAP_OPTION_SCATTERED_HELP"},
					{"TXT_KEY_MAP_OPTION_RANGES", "TXT_KEY_MAP_OPTION_RANGES_HELP"},
					{"TXT_KEY_MAP_OPTION_CLUSTERS", "TXT_KEY_MAP_OPTION_CLUSTERS_HELP"},
					"TXT_KEY_MAP_OPTION_RANDOM",
				},
				DefaultValue = 5,
				SortPriority = 1,
			},
			{
				Name = "TXT_KEY_MAP_OPTION_MOUNTAIN_DENSITY",
				Values = {
					"TXT_KEY_MAP_OPTION_DENSE",
					"TXT_KEY_MAP_OPTION_NORMAL",
					"TXT_KEY_MAP_OPTION_THIN",
					"TXT_KEY_MAP_OPTION_RANDOM",
				},
				DefaultValue = 4,
				SortPriority = 2,
			},
			{
				Name = "TXT_KEY_MAP_OPTION_BODIES_OF_WATER",
				Values = {
					{"TXT_KEY_MAP_OPTION_SMALL_LAKES", "TXT_KEY_MAP_OPTION_SMALL_LAKES_HELP"},
					{"TXT_KEY_MAP_OPTION_LARGE_LAKES", "TXT_KEY_MAP_OPTION_LARGE_LAKES_HELP"},
					{"TXT_KEY_MAP_OPTION_SEAS", "TXT_KEY_MAP_OPTION_SEAS_HELP"},
					"TXT_KEY_MAP_OPTION_RANDOM",
				},
				DefaultValue = 4,
				SortPriority = 3,
			},
		},
	};
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	-- This function can reset map grid sizes or world wrap settings.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {32, 20},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {40, 24},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {52, 32},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {64, 40},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {84, 52},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {104, 64},
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
------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- Adding climate shift to GenerateMap() -- This is a useful override option in Lua.
------------------------------------------------------------------------------
local baseGenerateMap = GenerateMap; -- Store GenerateMap function in a variable.
function GenerateMap()
	-- Roll a die to determine if the cold region will be in north or south.
	local shiftRoll = Map.Rand(2, "North or South climate shift - Highlands LUA");
	-- Store terrain/features Latitude adjustment in a global variable.
	if shiftRoll == 0 then -- Cold in north
		ShiftMultiplier = 0;
	else -- Cold in south
		ShiftMultiplier = 1;
	end

	-- Now call the core GenerateMap stored in the variable.
	baseGenerateMap();
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- Highlands uses a custom plot generation, specific to this script.
------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Setting Plot Types (Lua Highlands) ...");

	local iFlags = {};
	local iW, iH = Map.GetGridSize();
	local plotTypes = table.fill(PlotTypes.PLOT_LAND, iW * iH);

	-- Get user inputs.
	local userInputGrain = Map.GetCustomOption(4);
	local userInputPeaks = Map.GetCustomOption(5);
	local userInputLakes = Map.GetCustomOption(6);
	if userInputGrain == 5 then -- Random
		userInputGrain = 1 + Map.Rand(4, "Highlands Random Peak Grain - Lua");
	end
	if userInputPeaks == 4 then -- Random
		userInputPeaks = 1 + Map.Rand(3, "Highlands Random Peak Density - Lua");
	end
	if userInputLakes == 4 then -- Random
		userInputLakes = 1 + Map.Rand(3, "Highlands Random Lake Size - Lua");
	end

	-- Varying grains for hills/peaks per map size and Mountain Ranges setting.
	-- {ridgelines_grain, clustered_grain, ranges_grain, scattered_grain}
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {4, 5, 4, 3},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {4, 5, 4, 3},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {5, 6, 5, 4},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {5, 6, 5, 4},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {5, 6, 5, 4},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {5, 6, 5, 4},
	};
	local grain_list = worldsizes[Map.GetWorldSize()];
	local grain = grain_list[userInputGrain];

	-- Peak density
	local peak_list = {70, 77, 83};
	local hill_list = {40, 45, 50};
	local peaks = peak_list[userInputPeaks];
	local hills = hill_list[userInputPeaks];

	-- Lake density
	local lake_list = {8, 10, 15};
	local lake_grains = {5, 4, 3};
	local lakes = lake_list[userInputLakes];
	local lake_grain = lake_grains[userInputLakes];

	-- Tectonics Plate Counts, for use in generating passes through the mountains.
	local platevalues = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 6,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 9,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 12,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 18,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 24,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 30,
	};
	local baseNumPlates = platevalues[Map.GetWorldSize()] or 10;
	local plate_count_multipliers = {2, 1, 2, 1};
	local plate_count_multiplier = plate_count_multipliers[userInputGrain];
	local numPlates = math.ceil(baseNumPlates * plate_count_multiplier);

	local terrainFrac = Fractal.Create(iW, iH, grain, iFlags, -1, -1);
	local lakesFrac = Fractal.Create(iW, iH, lake_grain, iFlags, -1, -1);
	local mountain_passFrac = Fractal.Create(iW, iH, grain, iFlags, -1, -1);

	-- Build tectonic ridgelines where applicable.
	mountain_passFrac:BuildRidges(numPlates, iFlags, 1, 2);
	if userInputGrain == 1 then -- Tectonics mountains, add ridgelines to terrainFrac.
		terrainFrac:BuildRidges(numPlates, iFlags, 6, 1);
	end

	local iLakesThreshold = lakesFrac:GetHeight(100 - lakes);
	local iHillsThreshold = terrainFrac:GetHeight(hills);
	local iPeaksThreshold = terrainFrac:GetHeight(peaks);
	local iPassThreshold = mountain_passFrac:GetHeight(85);

	-- Now the main loop, which will assign the plot types.
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local i = y * iW + x + 1; -- add one because Lua arrays start at 1
			local lakeVal = lakesFrac:GetHeight(x, y);
			local val = terrainFrac:GetHeight(x, y);
			if lakeVal >= iLakesThreshold then
				plotTypes[i] = PlotTypes.PLOT_OCEAN;
			elseif val >= iPeaksThreshold then
				-- Check against the tectonic-style mountain pass generator.
				local passVal = mountain_passFrac:GetHeight(x, y);
				if passVal >= iPassThreshold then
					plotTypes[i] = PlotTypes.PLOT_HILLS;
				else
					plotTypes[i] = PlotTypes.PLOT_PEAK;
				end
			elseif val >= iHillsThreshold and val < iPeaksThreshold then
				plotTypes[i] = PlotTypes.PLOT_HILLS;
			else
				plotTypes[i] = PlotTypes.PLOT_LAND;
			end
		end
	end

	SetPlotTypes(plotTypes);

	GenerateCoasts{bExpandCoasts = false};
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function TerrainGenerator:GetLatitudeAtPlot(iX, iY)
	local lat = iY / self.iHeight;
	lat = lat + (128 - self.variation:GetHeight(iX, iY)) / (255.0 * 5.0);
	lat = math.clamp(lat, 0, 1);

	-- Flip terrain if southward shift was rolled.
	lat = math.abs(lat - ShiftMultiplier); -- Global variable set in MapGenerator

	return 0.8 * lat;
end
------------------------------------------------------------------------------
function GenerateTerrain()
	print("Generating Terrain (Lua Highlands) ...");

	-- Get Temperature setting input by user.
	local temp = Map.GetCustomOption(1);
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
	local numPlots = PlotTypes.NUM_PLOT_TYPES;
	local sum = (numPlots - plot:GetPlotType()) * 20;
	local numDirections = DirectionTypes.NUM_DIRECTION_TYPES;
	for direction = 0, numDirections - 1 do
		local adjacentPlot = Map.PlotDirection(plot:GetX(), plot:GetY(), direction);
		if adjacentPlot then
			sum = sum + (numPlots - adjacentPlot:GetPlotType());
		else
			sum = 0;
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

	-- Storing x, y positions as locals to prevent redundant function calls.
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

					-- Custom addition for Highlands, to fix river problems in top row of the map. Any other all-land map may need similar special casing.
					elseif not adjacentPlot and riverPlotY == iH - 1 then -- Top row of map, needs special handling
						if flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTH or
							flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHWEST or
							flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHEAST then

							local value = Map.Rand(5, "River Rand");
							if flowDirection == originalFlowDirection then
								value = (value * 3) / 4;
							end
							if value < bestValue then
								bestValue = value;
								bestFlowDirection = flowDirection;
							end
						end

					-- Custom addition for Highlands, to fix river problems in left column of the map. Any other all-land map may need similar special casing.
					elseif not adjacentPlot and riverPlotX == 0 then -- Left column of map, needs special handling
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

------------------------------------------------------------------------------
function FeatureGenerator:GetLatitudeAtPlot(_, iY)
	local lat = iY / self.iGridH;

	-- Flip terrain if southward shift was rolled.
	return 0.8 * math.abs(lat - ShiftMultiplier) -- Global, set prior to map generation.
end
------------------------------------------------------------------------------
function FeatureGenerator:AddIceAtPlot()
	-- No ice on this map
end
------------------------------------------------------------------------------
function AddFeatures()
	print("Adding Features (Lua Highlands) ...");

	-- Get Rainfall setting input by user.
	local rain = Map.GetCustomOption(2);
	if rain == 4 then
		rain = 1 + Map.Rand(3, "Random Rainfall - Lua");
	end

	local featuregen = FeatureGenerator.Create{rainfall = rain};

	featuregen:AddFeatures();
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function AssignStartingPlots:PlaceOilInTheSea()
	-- Oil only on land for this map script.
end
------------------------------------------------------------------------------
function StartPlotSystem()
	-- Get Resources setting input by user.
	local res = Map.GetCustomOption(3);
	if res == 6 then
		res = 1 + Map.Rand(3, "Random Resources Option - Lua");
	end

	print("Creating start plot database (MapGenerator.Lua)");
	local start_plot_database = AssignStartingPlots.Create();

	print("Dividing the map in to Regions (Lua Highlands)");
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
