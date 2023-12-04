------------------------------------------------------------------------------
--	FILE:	 Boreal.lua
--	AUTHOR:  Bob Thomas (Sirian)
--	PURPOSE: Regional map script - Arctic terrain
------------------------------------------------------------------------------
--	Copyright (c) 2012 Firaxis Games, Inc. All rights reserved.
------------------------------------------------------------------------------

include("MapGenerator");
include("FeatureGenerator");

------------------------------------------------------------------------------
function GetMapScriptInfo()
	local world_age, _, rainfall, _, resources = GetCoreMapOptions();
	return {
		Name = "TXT_KEY_MAP_BOREAL_VP",
		Description = "TXT_KEY_MAP_BOREAL_HELP",
		IsAdvancedMap = false,
		IconIndex = 0,
		CustomOptions = {world_age, rainfall, resources},
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
-- Boreal uses a custom plot/terrain generation, specific to this script.
------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Setting Plot Types (Lua Boreal) ...");

	local iFlags = {};
	local iW, iH = Map.GetGridSize();
	local plotTypes = table.fill(PlotTypes.PLOT_LAND, iW * iH);
	local adjust_plates = 1.0;

	for x = 0, iW - 1 do
		local y_shift = math.floor((iH / 10) - ((math.abs(x - (iW / 2))) / (iW / 2) * (iH / 10)));
		local start_y = iH - 2 - y_shift - Map.Rand(4, "Roughen coastline - Lua");
		for y = start_y, iH - 1 do
			local i = y * iW + x + 1;
			plotTypes[i] = PlotTypes.PLOT_OCEAN;
		end
	end

	-- Set values for hills and mountains according to World Age chosen by user. - Bob
	local world_age = Map.GetCustomOption(1);
	if world_age == 4 then
		world_age = 1 + Map.Rand(3, "Random World Age - Lua");
	end
	local world_age_old = 2;
	local world_age_normal = 3;
	local world_age_new = 5;
	local adjustment = world_age_normal;
	if world_age == 3 then -- 5 Billion Years
		adjustment = world_age_old;
		adjust_plates = adjust_plates * 0.75;
	elseif world_age == 1 then -- 3 Billion Years
		adjustment = world_age_new;
		adjust_plates = adjust_plates * 1.5;
	else -- 4 Billion Years
	end

	-- Apply adjustment to hills and peaks settings.
	local hillsBottom1 = 28 - adjustment;
	local hillsTop1 = 28 + adjustment;
	local hillsBottom2 = 72 - adjustment;
	local hillsTop2 = 72 + adjustment;
	local hillsNearMountains = 88 - (adjustment * 2);
	local mountains = 95 - adjustment;

	-- Hills and Mountains handled differently according to map size
	local WorldSizeTypes = {};
	for row in GameInfo.Worlds() do
		WorldSizeTypes[row.Type] = row.ID;
	end
	local sizekey = Map.GetWorldSize();
	-- Fractal Grains
	local sizevalues = {
		[WorldSizeTypes.WORLDSIZE_DUEL] = 3,
		[WorldSizeTypes.WORLDSIZE_TINY] = 3,
		[WorldSizeTypes.WORLDSIZE_SMALL] = 4,
		[WorldSizeTypes.WORLDSIZE_STANDARD] = 4,
		[WorldSizeTypes.WORLDSIZE_LARGE] = 5,
		[WorldSizeTypes.WORLDSIZE_HUGE] = 5,
	};
	local grain = sizevalues[sizekey] or 3;

	-- Tectonics Plate Counts
	local platevalues = {
		[WorldSizeTypes.WORLDSIZE_DUEL] = 6,
		[WorldSizeTypes.WORLDSIZE_TINY] = 9,
		[WorldSizeTypes.WORLDSIZE_SMALL] = 12,
		[WorldSizeTypes.WORLDSIZE_STANDARD] = 18,
		[WorldSizeTypes.WORLDSIZE_LARGE] = 24,
		[WorldSizeTypes.WORLDSIZE_HUGE] = 30,
	};
	local numPlates = platevalues[sizekey] or 15;
	-- Add in any plate count modifications passed in from the map script.
	numPlates = numPlates * adjust_plates;

	-- Generate fractals to govern hills and mountains
	local lakesFrac = Fractal.Create(iW, iH, 2, iFlags, -1, -1);
	local hillsFrac = Fractal.Create(iW, iH, grain, iFlags, -1, -1);
	local mountainsFrac = Fractal.Create(iW, iH, grain, iFlags, -1, -1);
	hillsFrac:BuildRidges(numPlates, iFlags, 1, 2);
	mountainsFrac:BuildRidges((numPlates * 0.75), iFlags, 6, 1);
	-- Get height values
	local iLakesThreshold = lakesFrac:GetHeight(94);
	local iHillsBottom1 = hillsFrac:GetHeight(hillsBottom1);
	local iHillsTop1 = hillsFrac:GetHeight(hillsTop1);
	local iHillsBottom2 = hillsFrac:GetHeight(hillsBottom2);
	local iHillsTop2 = hillsFrac:GetHeight(hillsTop2);
	local iHillsNearMountains = mountainsFrac:GetHeight(hillsNearMountains);
	local iMountainThreshold = mountainsFrac:GetHeight(mountains);
	local iPassThreshold = hillsFrac:GetHeight(85);

	-- Set Hills and Mountains
	-- Now the main loop, which will assign the plot types.
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local i = y * iW + x + 1;
			if plotTypes[i] == PlotTypes.PLOT_LAND then -- Check to see if lakes, hills or mountains will be applied.
				local lakeVal = lakesFrac:GetHeight(x,y);
				if lakeVal >= iLakesThreshold then
					plotTypes[i] = PlotTypes.PLOT_OCEAN;
				else
					local mountainVal = mountainsFrac:GetHeight(x, y);
					local hillVal = hillsFrac:GetHeight(x, y);

					if mountainVal >= iMountainThreshold then
						if hillVal >= iPassThreshold then -- Mountain Pass though the ridgeline
							plotTypes[i] = PlotTypes.PLOT_HILLS;
						else -- Mountain
							plotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
						end
					elseif mountainVal >= iHillsNearMountains then
						plotTypes[i] = PlotTypes.PLOT_HILLS;
					elseif (hillVal >= iHillsBottom1 and hillVal <= iHillsTop1) or (hillVal >= iHillsBottom2 and hillVal <= iHillsTop2) then
						plotTypes[i] = PlotTypes.PLOT_HILLS;
					end
				end
			end
		end
	end

	SetPlotTypes(plotTypes);
	GenerateCoasts();
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function GenerateTerrain()
	print("Generating Terrain (Lua Boreal) ...");
	local iFlags = {};
	local iW, iH = Map.GetGridSize();
	local snowFrac = Fractal.Create(iW, iH, 3, iFlags, -1, -1);
	local snowFrac2 = Fractal.Create(iW, 1 + math.floor(iH * 0.4), 2, iFlags, 7, 6);
	local plainsFrac = Fractal.Create(iW, iH, 4, iFlags, -1, -1);
	local iSnowThreshold = snowFrac:GetHeight(65);
	local iSnowThreshold2 = snowFrac2:GetHeight(45);
	local iPlainsThreshold = plainsFrac:GetHeight(91);

	-- Now the main loop, which will assign the terrain types.
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local plot = Map.GetPlot(x, y);
			local plot_type = plot:GetPlotType();
			if plot_type ~= PlotTypes.PLOT_OCEAN then
				local snowVal = snowFrac:GetHeight(x, y);
				local plainsVal = plainsFrac:GetHeight(x, y);
				if snowVal >= iSnowThreshold or plot_type == PlotTypes.PLOT_MOUNTAIN then
					plot:SetTerrainType(TerrainTypes.TERRAIN_SNOW, false, false);
				elseif plainsVal < iPlainsThreshold or plot_type == PlotTypes.PLOT_HILLS then
					plot:SetTerrainType(TerrainTypes.TERRAIN_TUNDRA, false, false);
				else
					plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, false);
				end
			end
		end
	end
	-- Second pass for extra frostiness in the north.
	for x = 0, iW - 1 do
		for y = iH - math.floor(iH * 0.4), iH - 1 do
			local plot = Map.GetPlot(x, y);
			local plot_type = plot:GetPlotType();
			if plot_type ~= PlotTypes.PLOT_OCEAN then
				local snowVal = snowFrac2:GetHeight(x, y - math.floor(iH * 0.6));
				if snowVal >= iSnowThreshold2 then
					plot:SetTerrainType(TerrainTypes.TERRAIN_SNOW, false, false);
				end
			end
		end
	end
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function GetRiverValueAtPlot(plot)
	-- Custom method to force rivers to flow toward the north.
	local _, iH = Map.GetGridSize();
	local x = plot:GetX();
	local y = plot:GetY();
	local random_factor = Map.Rand(3, "River direction random factor - Boreal LUA");
	local direction_influence_value = (math.abs(x - X_target) + (iH - y - 1)) * random_factor;

	local numPlots = PlotTypes.NUM_PLOT_TYPES;
	local sum = (numPlots - plot:GetPlotType()) * 20 + direction_influence_value;

	local numDirections = DirectionTypes.NUM_DIRECTION_TYPES;
	for direction = 0, numDirections - 1 do
		local adjacentPlot = Map.PlotDirection(plot:GetX(), plot:GetY(), direction);
		if adjacentPlot then
			sum = sum + (numPlots - adjacentPlot:GetPlotType());
		else
			sum = sum + (numPlots * 10);
		end
	end
	sum = sum + Map.Rand(10, "River Rand");

	return sum;
end
------------------------------------------------------------------------------
function DoRiver(startPlot, thisFlowDirection, originalFlowDirection, riverID)
	-- Customizing to handle problems in top row of the map. Only this aspect has been altered.

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
	-- Customization for Inland Sea, to keep river starts away from map edges and set river "original flow direction".
	local iW, iH = Map.GetGridSize()
	print("Inland Sea - Adding Rivers");
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
			return (plot:IsHills() or plot:IsMountain()) and (area:GetNumRiverEdges() < ((area:GetNumTiles() / plotsPerRiverEdge) + 1));
		end,

		function (plot)
			local area = plot:Area();
			local plotsPerRiverEdge = GameDefines.PLOTS_PER_RIVER_EDGE;
			return (area:GetNumRiverEdges() < (area:GetNumTiles() / plotsPerRiverEdge) + 1);
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
								local orig_direction;
								local diceroll = Map.Rand(7, "River Original Direction - Lua");
								if diceroll < 2 then
									orig_direction = FlowDirectionTypes.FLOWDIRECTION_NORTHEAST;
								elseif diceroll < 4 then
									orig_direction = FlowDirectionTypes.FLOWDIRECTION_NORTHWEST;
								else
									orig_direction = FlowDirectionTypes.FLOWDIRECTION_NORTH;
								end

								-- Add X target, for Boreal. This is a global variable.
								X_target = math.floor(iW / 5 + Map.Rand((iW * 0.6), "River Target - Lua"));

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
function FeatureGenerator:AddFeatures()
	self:AdjustTerrainTypes(); -- Sets terrain under jungles and softens arctic rivers

	-- Main loop, adds features to all plots as appropriate
	for y = 0, self.iGridH - 1 do
		for x = 0, self.iGridW - 1 do
			self:AddFeaturesAtPlot(x, y);
		end
	end
end
------------------------------------------------------------------------------
function FeatureGenerator:AddMarshAtPlot()
end
------------------------------------------------------------------------------
function FeatureGenerator:AddJunglesAtPlot()
end
------------------------------------------------------------------------------
function FeatureGenerator:AddIceAtPlot(plot, _, iY)
	-- Custom for Boreal
	local _, iH = Map.GetGridSize();
	if iY == iH - 1 then
		plot:SetFeatureType(self.featureIce);
	elseif iY >= iH - 5 and plot:IsWater() then
		local terrain_type = plot:GetTerrainType();
		if terrain_type == TerrainTypes.TERRAIN_OCEAN then
			local dice_target = 0;
			if iY == iH - 2 then
				dice_target = 70;
			elseif iY == iH - 3 then
				dice_target = 50;
			elseif iY == iH - 4 then
				dice_target = 30;
			elseif iY == iH - 5 then
				dice_target = 20;
			end
			local diceroll = Map.Rand(100, "Ice Formation - Boreal LUA");
			if diceroll < dice_target then
				plot:SetFeatureType(self.featureIce);
			end
		end
	end
end
------------------------------------------------------------------------------
function AddFeatures()
	print("Adding Features (Lua Boreal) ...");

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

	print("Dividing the map in to Regions (Lua Boreal)");
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
