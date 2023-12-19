------------------------------------------------------------------------------
--	FILE:	 Four_Corners.lua
--	AUTHOR:  Bob Thomas
--	PURPOSE: Regional map script - Designed to pit four teams against one another
--	         on a square map, one in each corner. Can work for two or three teams.
------------------------------------------------------------------------------
--	Copyright (c) 2010 Firaxis Games, Inc. All rights reserved.
------------------------------------------------------------------------------

include("MapGenerator");
include("MultilayeredFractal");
include("FeatureGenerator");
include("TerrainGenerator");

------------------------------------------------------------------------------
function GetMapScriptInfo()
	local world_age, temperature, rainfall, _, _ = GetCoreMapOptions();
	return {
		Name = "TXT_KEY_MAP_FOUR_CORNERS_VP",
		Description = "TXT_KEY_MAP_FOUR_CORNERS_HELP",
		SupportsMultiplayer = true,
		IconIndex = 8,
		CustomOptions = {
			world_age,
			temperature,
			rainfall,
			{
				Name = "TXT_KEY_MAP_OPTION_RESOURCES", -- Customizing the Resource setting to Default to Strategic Balance.
				Values = {
					"TXT_KEY_MAP_OPTION_SPARSE",
					"TXT_KEY_MAP_OPTION_STANDARD",
					"TXT_KEY_MAP_OPTION_ABUNDANT",
					"TXT_KEY_MAP_OPTION_LEGENDARY_START",
					"TXT_KEY_MAP_OPTION_STRATEGIC_BALANCE",
					"TXT_KEY_MAP_OPTION_RANDOM",
				},
				DefaultValue = 5,
				SortPriority = -95,
			},
			{
				Name = "TXT_KEY_MAP_OPTION_BUFFER_ZONES",
				Values = {
					"TXT_KEY_MAP_OPTION_OCEAN",
					"TXT_KEY_MAP_OPTION_MOUNTAINS",
					"TXT_KEY_MAP_OPTION_RANDOM",
				},
				DefaultValue = 1,
				SortPriority = 1,
			},
		},
	};
end
------------------------------------------------------------------------------

-------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	-- This function can reset map grid sizes or world wrap settings.

	-- Four Corners is an extremely compact multiplayer map type.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {16, 16},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {24, 24},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {30, 30},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {36, 36},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {44, 44},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {56, 56},
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

	-- This implementation is specific to Four Corners.
	local iW, iH = Map.GetGridSize();

	-- Fill all rows with land plots.
	self.wholeworldPlotTypes = table.fill(PlotTypes.PLOT_LAND, iW * iH);

	-- Add strips of ocean to middle of map.
	for y = 0, iH - 1 do
		for x = math.floor(iW / 2) - 2, math.floor(iW / 2) + 1 do
			local plotIndex = y * iW + x + 1;
			self.wholeworldPlotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
		end
	end
	for x = 0, iW - 1 do
		for y = math.floor(iH / 2) - 2, math.floor(iH / 2) + 1 do
			local plotIndex = y * iW + x + 1;
			self.wholeworldPlotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
		end
	end

	-- Land and water are set. Now apply hills and mountains.
	local world_age = Map.GetCustomOption(1);
	if world_age == 4 then
		world_age = 1 + Map.Rand(3, "Random World Age - Lua");
	end
	self:ApplyTectonics{world_age = world_age};

	-- If buffer zones are set to Mountains, add those now. (Must come after Tectonics layer to avoid being overwritten.)
	local buffer_setting = Map.GetCustomOption(5);
	if buffer_setting == 3 then
		buffer_setting = 1 + Map.Rand(2, "Random Buffer Zone Type - Four Corners, Lua");
	end
	if buffer_setting == 2 then -- Apply mountains.
		for y = 0, iH - 1 do
			for x = math.floor(iW / 2) - 2, math.floor(iW / 2) + 1 do
				local plotIndex = y * iW + x + 1;
				self.wholeworldPlotTypes[plotIndex] = PlotTypes.PLOT_MOUNTAIN;
			end
		end
		for x = 0, iW - 1 do
			for y = math.floor(iH / 2) - 2, math.floor(iH / 2) + 1 do
				local plotIndex = y * iW + x + 1;
				self.wholeworldPlotTypes[plotIndex] = PlotTypes.PLOT_MOUNTAIN;
			end
		end
	end

	-- Now add the bridge across the center!
	local centerX = (iW - 1) / 2;
	local centerY = (iH - 1) / 2;
	local axis;
	if buffer_setting == 2 then -- Widen the center region.
		axis = 6.5;
	else
		axis = 5.5;
	end
	local axisSquared = axis * axis;
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local deltaX = x - centerX;
			local deltaY = y - centerY;
			local deltaXSquared = deltaX * deltaX;
			local deltaYSquared = deltaY * deltaY;
			local d = deltaXSquared / axisSquared + deltaYSquared / axisSquared;
			if d <= 1 then
				local i = y * iW + x + 1;
				self.wholeworldPlotTypes[i] = PlotTypes.PLOT_LAND;
			end
		end
	end

	-- Plot Type generation completed. Return global plot array.
	return self.wholeworldPlotTypes;
end
------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Setting Plot Types (Lua Four Corners) ...");

	local layered_world = MultilayeredFractal.Create();
	local plot_list = layered_world:GeneratePlotsByRegion();

	SetPlotTypes(plot_list);

	GenerateCoasts{bExpandCoasts = false};
end
----------------------------------------------------------------------------------

----------------------------------------------------------------------------------
function TerrainGenerator:GetLatitudeAtPlot(iX, iY)
	local lat = math.abs((self.iHeight / 2) - iY) / (self.iHeight / 2);
	lat = lat + (128 - self.variation:GetHeight(iX, iY)) / (255.0 * 5.0);
	lat = 0.8 * math.clamp(lat, 0, 1);
	return lat;
end
----------------------------------------------------------------------------------
function GenerateTerrain()
	print("Generating Terrain (Lua Four Corners) ...");

	-- Get Temperature setting input by user.
	local temp = Map.GetCustomOption(2);
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
	for direction = 0, numDirections - 1, 1 do
		local adjacentPlot = Map.PlotDirection(plot:GetX(), plot:GetY(), direction);
		if adjacentPlot then
			sum = sum + (numPlots - adjacentPlot:GetPlotType());
		else
			sum = 0 -- Custom, prevents rivers avoiding running off the map edge.
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
							if flowDirection == originalFlowDirection then
								value = (value * 3) / 4;
							end
							if value < bestValue then
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

------------------------------------------------------------------------------
function FeatureGenerator:GetLatitudeAtPlot(_, iY)
	local lat = math.abs((self.iGridH / 2) - iY) / (self.iGridH / 2);
	local adjusted_lat = 0.8 * lat;
	return adjusted_lat;
end
------------------------------------------------------------------------------
function FeatureGenerator:AddIceAtPlot()
	-- No ice in this map.
end
------------------------------------------------------------------------------
function AddFeatures()
	print("Adding Features (Lua Four Corners) ...");

	-- Get Rainfall setting input by user.
	local rain = Map.GetCustomOption(3);
	if rain == 4 then
		rain = 1 + Map.Rand(3, "Random Rainfall - Lua");
	end

	local featuregen = FeatureGenerator.Create{rainfall = rain};

	featuregen:AddFeatures();
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function StartPlotSystem()
	-- Get Resources setting input by user.
	local res = Map.GetCustomOption(4);
	if res == 6 then
		res = 1 + Map.Rand(3, "Random Resources Option - Lua");
	end

	print("Creating start plot database.");
	local start_plot_database = AssignStartingPlots.Create();

	print("Dividing the map in to Regions.");
	-- Regional Division Method 2: Continental
	local args = {
		method = 2,
		resources = res,
	};
	start_plot_database:GenerateRegions(args);

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
