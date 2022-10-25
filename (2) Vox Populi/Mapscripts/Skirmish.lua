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

------------------------------------------------------------------------------
function GetMapScriptInfo()
	return {
		Name = "TXT_KEY_MAP_SKIRMISH",
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
	}
end
------------------------------------------------------------------------------

-------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	-- This function can reset map grid sizes or world wrap settings.
	--
	-- Skirmish is a world without oceans, so use grid sizes two levels below normal.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {24, 16},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {32, 20},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {40, 24},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {52, 32},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {64, 40},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {84, 52}
		}
	local grid_size = worldsizes[worldSize];
	--
	local world = GameInfo.Worlds[worldSize];
	if(world ~= nil) then
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
	--
	-- This implementation is specific to Skirmish.
	local iW, iH = Map.GetGridSize();
	local fracFlags = {FRAC_WRAP_X = true, FRAC_POLAR = true};
	self.wholeworldPlotTypes = table.fill(PlotTypes.PLOT_LAND, iW * iH);
	
	-- Get user inputs.
	dominant_terrain = Map.GetCustomOption(1) -- GLOBAL variable.
	if dominant_terrain == 9 then -- Random
		dominant_terrain = 1 + Map.Rand(8, "Random Type of Dominant Terrain - Skirmish LUA");
	end
	userInputWaterSetting = Map.GetCustomOption(2) -- GLOBAL variable.
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
			}
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
		self:ApplyTectonics(args)
	end
	
	-- Create buffer zone in middle four columns. This will create some choke points.
	--
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
		local i = (iH - 1) * iW + x + 1;
		self.wholeworldPlotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
	end
	-- Add random smattering of mountains to middle two columns of buffer zone.
	local west_half, east_half = {}, {};
	for loop = 1, iH - 2 do
		table.insert(west_half, loop);
		table.insert(east_half, loop);
	end
	local west_shuffled = GetShuffledCopyOfTable(west_half)
	local east_shuffled = GetShuffledCopyOfTable(east_half)
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
	return self.wholeworldPlotTypes
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
			local plot = Map.GetPlot(x, y)
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

				for loop, plot_adjustments in ipairs(search_table) do
					local searchX, searchY;
					searchX = x + plot_adjustments[1];
					searchY = y + plot_adjustments[2];
					local searchPlot = Map.GetPlot(searchX, searchY)
					local plotType = searchPlot:GetPlotType()
					if plotType == PlotTypes.PLOT_MOUNTAIN then
						plot:SetPlotType(PlotTypes.PLOT_HILLS, false, false)
						break
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
	if (plot:IsWater()) then
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
		local plainsVal = self.plains:GetHeight(iX, iY);
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
		if(lat >= self.fSnowLatitude) then
			terrainVal = self.terrainSnow;
		elseif(lat >= self.fTundraLatitude) then
			terrainVal = self.terrainTundra;
		elseif (lat < self.fGrassLatitude) then
			terrainVal = self.terrainGrass;
		else
			local desertVal = self.deserts:GetHeight(iX, iY);
			local plainsVal = self.plains:GetHeight(iX, iY);
			if ((desertVal >= self.iDesertBottom) and (desertVal <= self.iDesertTop) and (lat >= self.fDesertBottomLatitude) and (lat < self.fDesertTopLatitude)) then
				terrainVal = self.terrainDesert;
			elseif ((plainsVal >= self.iPlainsBottom) and (plainsVal <= self.iPlainsTop)) then
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
	local iW, iH = Map.GetGridSize()
	local x = plot:GetX()
	local y = plot:GetY()
	local random_factor = Map.Rand(3, "River direction random factor - Skirmish LUA");
	local direction_influence_value = (math.abs(iW - (x - (iW / 2))) + ((math.abs(y - (iH / 2))) / 3)) * random_factor;

	local numPlots = PlotTypes.NUM_PLOT_TYPES;
	local sum = ((numPlots - plot:GetPlotType()) * 20) + direction_influence_value;

	local numDirections = DirectionTypes.NUM_DIRECTION_TYPES;
	for direction = 0, numDirections - 1 do
		local adjacentPlot = Map.PlotDirection(plot:GetX(), plot:GetY(), direction);
		if (adjacentPlot ~= nil) then
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

	local iW, iH = Map.GetGridSize()
	thisFlowDirection = thisFlowDirection or FlowDirectionTypes.NO_FLOWDIRECTION;
	originalFlowDirection = originalFlowDirection or FlowDirectionTypes.NO_FLOWDIRECTION;

	-- pStartPlot = the plot at whose SE corner the river is starting
	if (riverID == nil) then
		riverID = nextRiverID;
		nextRiverID = nextRiverID + 1;
	end

	local otherRiverID = _rivers[startPlot]
	if (otherRiverID ~= nil and otherRiverID ~= riverID and originalFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION) then
		return; -- Another river already exists here; can't branch off of an existing river!
	end

	local riverPlot;
	
	local bestFlowDirection = FlowDirectionTypes.NO_FLOWDIRECTION;
	if (thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTH) then
	
		riverPlot = startPlot;
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_EAST);
		if ( adjacentPlot == nil or riverPlot:IsWOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater() ) then
			return;
		end

		_rivers[riverPlot] = riverID;
		riverPlot:SetWOfRiver(true, thisFlowDirection);
		riverPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_NORTHEAST);
		
	elseif (thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHEAST) then
	
		riverPlot = startPlot;
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_SOUTHEAST);
		if ( adjacentPlot == nil or riverPlot:IsNWOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater() ) then
			return;
		end

		_rivers[riverPlot] = riverID;
		riverPlot:SetNWOfRiver(true, thisFlowDirection);
		-- riverPlot does not change
	
	elseif (thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST) then
	
		riverPlot = Map.PlotDirection(startPlot:GetX(), startPlot:GetY(), DirectionTypes.DIRECTION_EAST);
		if (riverPlot == nil) then
			return;
		end
		
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_SOUTHWEST);
		if (adjacentPlot == nil or riverPlot:IsNEOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater()) then
			return;
		end

		_rivers[riverPlot] = riverID;
		riverPlot:SetNEOfRiver(true, thisFlowDirection);
		-- riverPlot does not change
	
	elseif (thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTH) then
	
		riverPlot = Map.PlotDirection(startPlot:GetX(), startPlot:GetY(), DirectionTypes.DIRECTION_SOUTHWEST);
		if (riverPlot == nil) then
			return;
		end
		
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_EAST);
		if (adjacentPlot == nil or riverPlot:IsWOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater()) then
			return;
		end
		
		_rivers[riverPlot] = riverID;
		riverPlot:SetWOfRiver(true, thisFlowDirection);
		-- riverPlot does not change
	
	elseif (thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST) then

		riverPlot = startPlot;
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_SOUTHEAST);
		if (adjacentPlot == nil or riverPlot:IsNWOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater()) then
			return;
		end
		
		_rivers[riverPlot] = riverID;
		riverPlot:SetNWOfRiver(true, thisFlowDirection);
		-- riverPlot does not change

	elseif (thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHWEST) then
		
		riverPlot = startPlot;
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_SOUTHWEST);
		
		if ( adjacentPlot == nil or riverPlot:IsNEOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater()) then
			return;
		end

		_rivers[riverPlot] = riverID;
		riverPlot:SetNEOfRiver(true, thisFlowDirection);
		riverPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_WEST);

	else
		-- River is starting here, set the direction in the next step
		riverPlot = startPlot;		
	end

	if (riverPlot == nil or riverPlot:IsWater()) then
		-- The river has flowed off the edge of the map or into the ocean. All is well.
		return; 
	end

	-- Storing X,Y positions as locals to prevent redundant function calls.
	local riverPlotX = riverPlot:GetX();
	local riverPlotY = riverPlot:GetY();
	
	-- Table of methods used to determine the adjacent plot.
	local adjacentPlotFunctions = {
		[FlowDirectionTypes.FLOWDIRECTION_NORTH] = function() 
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_NORTHWEST); 
		end,
		
		[FlowDirectionTypes.FLOWDIRECTION_NORTHEAST] = function() 
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_NORTHEAST);
		end,
		
		[FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST] = function() 
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_EAST);
		end,
		
		[FlowDirectionTypes.FLOWDIRECTION_SOUTH] = function() 
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_SOUTHWEST);
		end,
		
		[FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST] = function() 
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_WEST);
		end,
		
		[FlowDirectionTypes.FLOWDIRECTION_NORTHWEST] = function() 
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_NORTHWEST);
		end	
	}
	
	if(bestFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION) then

		-- Attempt to calculate the best flow direction.
		local bestValue = math.huge;
		for flowDirection, getAdjacentPlot in pairs(adjacentPlotFunctions) do
			
			if (GetOppositeFlowDirection(flowDirection) ~= originalFlowDirection) then
				
				if (thisFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION or
					flowDirection == TurnRightFlowDirections[thisFlowDirection] or 
					flowDirection == TurnLeftFlowDirections[thisFlowDirection]) then
				
					local adjacentPlot = getAdjacentPlot();
					
					if (adjacentPlot ~= nil) then
					
						local value = GetRiverValueAtPlot(adjacentPlot);
						if (flowDirection == originalFlowDirection) then
							value = (value * 3) / 4;
						end
						
						if (value < bestValue) then
							bestValue = value;
							bestFlowDirection = flowDirection;
						end

					-- Custom addition for Highlands, to fix river problems in top row of the map. Any other all-land map may need similar special casing.
					elseif adjacentPlot == nil and riverPlotY == iH - 1 then -- Top row of map, needs special handling
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

					-- Custom addition for Highlands, to fix river problems in left column of the map. Any other all-land map may need similar special casing.
					elseif adjacentPlot == nil and riverPlotX == 0 then -- Left column of map, needs special handling
						if flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTH or
						   flowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTH or
						   flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHWEST or
						   flowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST then
							
							local value = Map.Rand(5, "River Rand");
							if (flowDirection == originalFlowDirection) then
								value = (value * 3) / 4;
							end
							if (value < bestValue) then
								bestValue = value;
								bestFlowDirection = flowDirection;
							end
						end
					end
				end
			end
		end
		
		-- Try a second pass allowing the river to "flow backwards".
		if(bestFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION) then
		
			local bestValue = math.huge;
			for flowDirection, getAdjacentPlot in pairs(adjacentPlotFunctions) do
			
				if (thisFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION or
					flowDirection == TurnRightFlowDirections[thisFlowDirection] or 
					flowDirection == TurnLeftFlowDirections[thisFlowDirection]) then
				
					local adjacentPlot = getAdjacentPlot();
					
					if (adjacentPlot ~= nil) then
						
						local value = GetRiverValueAtPlot(adjacentPlot);
						if (value < bestValue) then
							bestValue = value;
							bestFlowDirection = flowDirection;
						end
					end	
				end
			end
		end
	end
	
	--Recursively generate river.
	if (bestFlowDirection ~= FlowDirectionTypes.NO_FLOWDIRECTION) then
		if  (originalFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION) then
			originalFlowDirection = bestFlowDirection;
		end
		
		DoRiver(riverPlot, bestFlowDirection, originalFlowDirection, riverID);
	end
end
------------------------------------------------------------------------------
function AddRivers()
	-- Only add rivers if Water Setting is value of 1 or 4. Otherwise no rivers.
	if userInputWaterSetting == 2 or userInputWaterSetting == 3 or userInputWaterSetting == 5 then -- No Rivers!
		return
	end

	-- Customization for Skirmish, to keep river starts away from buffer zone in middle columns of map, and set river "original flow direction".
	local iW, iH = Map.GetGridSize()
	print("Skirmish - Adding Rivers");
	local passConditions = {
		function(plot)
			return plot:IsHills() or plot:IsMountain();
		end,
		
		function(plot)
			return (not plot:IsCoastalLand()) and (Map.Rand(8, "MapGenerator AddRivers") == 0);
		end,
		
		function(plot)
			local area = plot:Area();
			local plotsPerRiverEdge = GameDefines["PLOTS_PER_RIVER_EDGE"];
			return (plot:IsHills() or plot:IsMountain()) and (area:GetNumRiverEdges() <	((area:GetNumTiles() / plotsPerRiverEdge) + 1));
		end,
		
		function(plot)
			local area = plot:Area();
			local plotsPerRiverEdge = GameDefines["PLOTS_PER_RIVER_EDGE"];
			return (area:GetNumRiverEdges() < (area:GetNumTiles() / plotsPerRiverEdge) + 1);
		end
	}
	for iPass, passCondition in ipairs(passConditions) do
		local riverSourceRange;
		local seaWaterRange;
		if (iPass <= 2) then
			riverSourceRange = GameDefines["RIVER_SOURCE_MIN_RIVER_RANGE"];
			seaWaterRange = GameDefines["RIVER_SOURCE_MIN_SEAWATER_RANGE"];
		else
			riverSourceRange = (GameDefines["RIVER_SOURCE_MIN_RIVER_RANGE"] / 2);
			seaWaterRange = (GameDefines["RIVER_SOURCE_MIN_SEAWATER_RANGE"] / 2);
		end
		for i, plot in Plots() do
			local current_x = plot:GetX()
			local current_y = plot:GetY()
			if current_x < 1 or current_x >= iW - 2 or current_y < 2 or current_y >= iH - 1 then
				-- Plot too close to edge, ignore it.
			elseif current_x >= (iW / 2) - 2 and current_x <= (iW / 2) + 1 then
				-- Plot in buffer zone, ignore it.
			elseif (not plot:IsWater()) then
				if(passCondition(plot)) then
					if (not Map.FindWater(plot, riverSourceRange, true)) then
						if (not Map.FindWater(plot, seaWaterRange, false)) then
							local inlandCorner = plot:GetInlandCorner();
							if(inlandCorner) then
								local start_x = inlandCorner:GetX()
								local start_y = inlandCorner:GetY()
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
	self.jungles		= Fractal.Create(width, height, self.jungle_grain, self.fractalFlags, self.fracXExp, self.fracYExp);
	self.forests		= Fractal.Create(width, height, self.forest_grain, self.fractalFlags, self.fracXExp, self.fracYExp);
	self.forestclumps	= Fractal.Create(width, height, self.clump_grain, self.fractalFlags, self.fracXExp, self.fracYExp);
	self.marsh			= Fractal.Create(width, height, 4, self.fractalFlags, self.fracXExp, self.fracYExp);
	
	-- Get heights
	self.iJungleBottom	= self.jungles:GetHeight((100 - self.iJunglePercent)/2)
	self.iJungleTop		= self.jungles:GetHeight((100 + self.iJunglePercent)/2)
	self.iJungleRange	= (self.iJungleTop - self.iJungleBottom) * self.iJungleFactor;
	self.iForestLevel	= self.forests:GetHeight(80) -- 20% forest coverage
	self.iClumpLevel	= self.forestclumps:GetHeight(94) -- 6% forest clumps
	self.iMarshLevel	= self.marsh:GetHeight(100 - self.fMarshPercent)
	
	if dominant_terrain == 3 then -- Forest
		self.iClumpLevel	= self.forestclumps:GetHeight(65) -- 35% forest clumps
		self.iForestLevel	= self.forests:GetHeight(55) -- 45% forest coverage of what isn't covered by clumps.
	elseif dominant_terrain == 5 then -- Desert
		self.iOasisPercent = 8;
		if userInputWaterSetting == 4 then
			self.iOasisPercent = 6;
		elseif userInputWaterSetting == 5 then
			self.iOasisPercent = 15;
		end
	elseif dominant_terrain == 6 then -- Tundra
		self.iClumpLevel	= self.forestclumps:GetHeight(80) -- 20% forest clumps
		self.iForestLevel	= self.forests:GetHeight(60) -- 40% forest coverage of what isn't covered by clumps.
	elseif dominant_terrain == 8 then -- Global
		self.iClumpLevel	= self.forestclumps:GetHeight(90) -- 10% forest clumps
		self.iForestLevel	= self.forests:GetHeight(69) -- 31% forest coverage of what isn't covered by clumps.
	end
end
------------------------------------------------------------------------------
function FeatureGenerator:AddIceAtPlot(plot, iX, iY, lat)
	if dominant_terrain == 8 then -- Global
		if (plot:CanHaveFeature(self.featureIce)) then
			if Map.IsWrapX() and (iY == 0 or iY == self.iGridH - 1) then
				plot:SetFeatureType(self.featureIce, -1)
			else
				local rand = Map.Rand(100, "Add Ice Lua")/100.0;
				if(rand < 8 * (lat - 0.875)) then
					plot:SetFeatureType(self.featureIce, -1);
				elseif(rand < 4 * (lat - 0.75)) then
					plot:SetFeatureType(self.featureIce, -1);
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
				plot:SetFeatureType(self.featureJungle, -1);
			end
		end
	elseif dominant_terrain == 8 then -- Global, use default jungle placement.
		local jungle_height = self.jungles:GetHeight(iX, iY);
		if jungle_height <= self.iJungleTop and jungle_height >= self.iJungleBottom + (self.iJungleRange * lat) then
			if plot:IsFlatlands() or plot:IsHills() then
				plot:SetFeatureType(self.featureJungle, -1);
			end
		end
	-- else -- No jungles
	end
end
------------------------------------------------------------------------------
function FeatureGenerator:AdjustTerrainTypes()
	local width = self.iGridW - 1;
	local height = self.iGridH - 1;
	for y = 0, height do
		for x = 0, width do
			local plot = Map.GetPlot(x, y);
			if (plot:GetFeatureType() == self.featureJungle) then
				plot:SetTerrainType(self.terrainPlains, false, true)  -- These flags are for recalc of areas and rebuild of graphics. No need to recalc from any of these changes.		
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
function AssignStartingPlots:GenerateRegions(args)
	print("Map Generation - Dividing the map in to Regions");
	-- This version is tailored for handling two-teams play.
	local args = args or {};
	local iW, iH = Map.GetGridSize();
	self.method = RegionDivision.RECTANGULAR; -- Flag the map as using a Rectangular division method.

	-- Automatically set strategic resources to place a source of iron and horse near every start location.
	self.resBalance = true;
	
	-- Determine number of civilizations and city states present in this game.
	self.iNumCivs, self.iNumCityStates, self.player_ID_list, self.bTeamGame, self.teams_with_major_civs, self.number_civs_per_team = GetPlayerAndTeamInfo()
	self.iNumCityStatesUnassigned = self.iNumCityStates;
	print("-"); print("Civs:", self.iNumCivs); print("City States:", self.iNumCityStates);

	-- Determine number of teams (of Major Civs only, not City States) present in this game.
	iNumTeams = table.maxn(self.teams_with_major_civs);				-- GLOBAL
	print("-"); print("Teams:", iNumTeams);

	-- If two teams are present, use team-oriented handling of start points, one team west, one east.
	if iNumTeams == 2 then
		print("-"); print("Number of Teams present is two! Using custom team start placement for Skirmish."); print("-");
		
		-- ToDo: Correctly identify team IDs and how many Civs are on each team.
		-- Also need to shuffle the teams so its random who starts on which half.
		local shuffled_team_list = GetShuffledCopyOfTable(self.teams_with_major_civs)
		teamWestID = shuffled_team_list[1];							-- GLOBAL
		teamEastID = shuffled_team_list[2]; 						-- GLOBAL
		iNumCivsInWest = self.number_civs_per_team[teamWestID];		-- GLOBAL
		iNumCivsInEast = self.number_civs_per_team[teamEastID];		-- GLOBAL

		-- Process the team in the west.
		self.inhabited_WestX = 0;
		self.inhabited_SouthY = 0;
		self.inhabited_Width = math.floor(iW / 2) - 1;
		self.inhabited_Height = iH;
		-- Obtain "Start Placement Fertility" inside the rectangle.
		-- Data returned is: fertility table, sum of all fertility, plot count.
		local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityInRectangle(self.inhabited_WestX, 
		                                         self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height)
		-- Assemble the Rectangle data table:
		local rect_table = {self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, 
		                    self.inhabited_Height, -1, fertCount, plotCount}; -- AreaID -1 means ignore area IDs.
		-- Divide the rectangle.
		self:DivideIntoRegions(iNumCivsInWest, fert_table, rect_table)

		-- Process the team in the east.
		self.inhabited_WestX = math.floor(iW / 2) + 1;
		self.inhabited_SouthY = 0;
		self.inhabited_Width = math.floor(iW / 2) - 1;
		self.inhabited_Height = iH;
		-- Obtain "Start Placement Fertility" inside the rectangle.
		-- Data returned is: fertility table, sum of all fertility, plot count.
		local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityInRectangle(self.inhabited_WestX, 
		                                         self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height)
		-- Assemble the Rectangle data table:
		local rect_table = {self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, 
		                    self.inhabited_Height, -1, fertCount, plotCount}; -- AreaID -1 means ignore area IDs.
		-- Divide the rectangle.
		self:DivideIntoRegions(iNumCivsInEast, fert_table, rect_table)
		-- The regions have been defined.

	-- If number of teams is any number other than two, use standard division.
	else	
		local best_areas = {};
		local globalFertilityOfLands = {};

		-- Obtain info on all landmasses for comparision purposes.
		local iGlobalFertilityOfLands = 0;
		local iNumLandPlots = 0;
		local iNumLandAreas = 0;
		local land_area_IDs = {};
		local land_area_plots = {};
		local land_area_fert = {};
		-- Cycle through all plots in the world, checking their Start Placement Fertility and AreaID.
		for x = 0, iW - 1 do
			for y = 0, iH - 1 do
				local i = y * iW + x + 1;
				local plot = Map.GetPlot(x, y);
				if not plot:IsWater() then -- Land plot, process it.
					iNumLandPlots = iNumLandPlots + 1;
					local iArea = plot:GetArea();
					local plotFertility = self:MeasureStartPlacementFertilityOfPlot(x, y, true); -- Check for coastal land is enabled.
					iGlobalFertilityOfLands = iGlobalFertilityOfLands + plotFertility;
					--
					if TestMembership(land_area_IDs, iArea) == false then -- This plot is the first detected in its AreaID.
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

		-- Sort areas, achieving a list of AreaIDs with best areas first.
		--
		-- Fertility data in land_area_fert is stored with areaID index keys.
		-- Need to generate a version of this table with indices of 1 to n, where n is number of land areas.
		local interim_table = {};
		local min_area_fertility = iGlobalFertilityOfLands / self.iNumCivs * 0.5;
		print("Minimum area fertility required =", min_area_fertility);
		for loop_index, data_entry in pairs(land_area_fert) do
			-- add fertility check to prevent tiny islands from being considered
			if data_entry >= min_area_fertility then
				table.insert(interim_table, data_entry);
			else
				iNumLandAreas = iNumLandAreas - 1;
			end
		end
		
		--[[for AreaID, fert in ipairs(interim_table) do
			print("Interim Table ID " .. AreaID .. " has fertility of " .. fert);
		end
		print("* * * * * * * * * *"); ]]--
		
		-- Sort the fertility values stored in the interim table. Sort order in Lua is lowest to highest.
		table.sort(interim_table);

		--[[ for AreaID, fert in ipairs(interim_table) do
			print("Interim Table ID " .. AreaID .. " has fertility of " .. fert);
		end
		print("* * * * * * * * * *"); ]]--

		-- If less players than landmasses, we will ignore the extra landmasses.
		local iNumRelevantLandAreas = math.min(iNumLandAreas, self.iNumCivs);
		print("Number of relevant areas =", iNumRelevantLandAreas);
		-- Now re-match the AreaID numbers with their corresponding fertility values
		-- by comparing the original fertility table with the sorted interim table.
		-- During this comparison, best_areas will be constructed from sorted AreaIDs, richest stored first.
		local best_areas = {};
		-- Currently, the best yields are at the end of the interim table. We need to step backward from there.
		local end_of_interim_table = table.maxn(interim_table);
		-- We may not need all entries in the table. Process only iNumRelevantLandAreas worth of table entries.
		local fertility_value_list = {};
		local fertility_value_tie = false;
		for tableConstructionLoop = end_of_interim_table, (end_of_interim_table - iNumRelevantLandAreas + 1), -1 do
			if TestMembership(fertility_value_list, interim_table[tableConstructionLoop]) == true then
				fertility_value_tie = true;
				print("*** WARNING: Fertility Value Tie exists! ***");
			else
				table.insert(fertility_value_list, interim_table[tableConstructionLoop]);
			end
		end

		if fertility_value_tie == false then -- No ties, so no need of special handling for ties.
			for areaTestLoop = end_of_interim_table, (end_of_interim_table - iNumRelevantLandAreas + 1), -1 do
				for loop_index, AreaID in ipairs(land_area_IDs) do
					if interim_table[areaTestLoop] == land_area_fert[land_area_IDs[loop_index]] then
						table.insert(best_areas, AreaID);
						break
					end
				end
			end
		else -- Ties exist! Special handling required to protect against a shortfall in the number of defined regions.
			local iNumUniqueFertValues = table.maxn(fertility_value_list);
			for fertLoop = 1, iNumUniqueFertValues do
				for AreaID, fert in pairs(land_area_fert) do
					if fert == fertility_value_list[fertLoop] then
						-- Add ties only if there is room!
						local best_areas_length = table.maxn(best_areas);
						if best_areas_length < iNumRelevantLandAreas then
							table.insert(best_areas, AreaID);
						else
							break
						end
					end
				end
			end
		end

		-- Assign continents to receive start plots. Record number of civs assigned to each landmass.
		local inhabitedAreaIDs = {};
		local numberOfCivsPerArea = table.fill(0, iNumRelevantLandAreas); -- Indexed in synch with best_areas. Use same index to match values from each table.
		for civToAssign = 1, self.iNumCivs do
			local bestRemainingArea = -1;
			local bestRemainingFertility = 0;
			local bestAreaTableIndex;
			-- Loop through areas, find the one with the best remaining fertility (civs added 
			-- to a landmass reduces its fertility rating for subsequent civs).
			--
			--print("- - Searching landmasses in order to place Civ #", civToAssign); print("-");
			for area_loop, AreaID in ipairs(best_areas) do
				--local thisLandmassCurrentFertility = land_area_fert[AreaID] / (1 + numberOfCivsPerArea[area_loop]);
				-- assume 80% of fertility is in relevant land areas
				local thisLandmassCurrentFertility = land_area_fert[AreaID] - iGlobalFertilityOfLands * 0.8 * numberOfCivsPerArea[area_loop] / self.iNumCivs;
				if thisLandmassCurrentFertility > bestRemainingFertility and numberOfCivsPerArea[area_loop] < math.max(self.iNumCivs - 2, 2) then
					bestRemainingArea = AreaID;
					bestRemainingFertility = thisLandmassCurrentFertility;
					bestAreaTableIndex = area_loop;
					--
					-- print("- Found new candidate landmass with Area ID#:", bestRemainingArea, " with fertility of ", bestRemainingFertility);
				end
			end
			if bestRemainingArea == -1 then
				print("Failed to find an area somehow, assign to first area as a failsafe");
				bestRemainingArea = best_areas[1];
				bestAreaTableIndex = 1;
			end

			-- Record results for this pass. (A landmass has been assigned to receive one more start point than it previously had).
			numberOfCivsPerArea[bestAreaTableIndex] = numberOfCivsPerArea[bestAreaTableIndex] + 1;
			if TestMembership(inhabitedAreaIDs, bestRemainingArea) == false then
				table.insert(inhabitedAreaIDs, bestRemainingArea);
			end
			--print("Civ #", civToAssign, "has been assigned to Area#", bestRemainingArea); print("-");
		end
		--print("-"); print("--- End of Initial Readout ---"); print("-");
		
		--print("*** Number of Civs per Landmass - Table Readout ***");
		--PrintContentsOfTable(numberOfCivsPerArea)
		--print("--- End of Civs per Landmass readout ***"); print("-"); print("-");
				
		-- Loop through the list of inhabited landmasses, dividing each landmass in to regions.
		-- Note that it is OK to divide a continent with one civ on it: this will assign the whole
		-- of the landmass to a single region, and is the easiest method of recording such a region.
		local iNumInhabitedLandmasses = table.maxn(inhabitedAreaIDs);
		for loop, currentLandmassID in ipairs(inhabitedAreaIDs) do
			-- Obtain the boundaries of and data for this landmass.
			local landmass_data = ObtainLandmassBoundaries(currentLandmassID);
			local iWestX = landmass_data[1];
			local iSouthY = landmass_data[2];
			local iEastX = landmass_data[3];
			local iNorthY = landmass_data[4];
			local iWidth = landmass_data[5];
			local iHeight = landmass_data[6];
			local wrapsX = landmass_data[7];
			local wrapsY = landmass_data[8];
			-- Obtain "Start Placement Fertility" of the current landmass. (Necessary to do this
			-- again because the fert_table can't be built prior to finding boundaries, and we had
			-- to ID the proper landmasses via fertility to be able to figure out their boundaries.
			local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityOfLandmass(currentLandmassID, 
													 iWestX, iEastX, iSouthY, iNorthY, wrapsX, wrapsY);
			-- Assemble the rectangle data for this landmass.
			local rect_table = {iWestX, iSouthY, iWidth, iHeight, currentLandmassID, fertCount, plotCount};
			-- Divide this landmass in to number of regions equal to civs assigned here.
			iNumCivsOnThisLandmass = numberOfCivsPerArea[loop];
			if iNumCivsOnThisLandmass > 0 and iNumCivsOnThisLandmass <= GameDefines.MAX_MAJOR_CIVS then -- valid number of civs.
			
				--[[ Debug printout for regional division inputs.
				print("-"); print("- Region #: ", loop);
				print("- Civs on this landmass: ", iNumCivsOnThisLandmass);
				print("- Area ID#: ", currentLandmassID);
				print("- Fertility: ", fertCount);
				print("- Plot Count: ", plotCount); print("-");
				--]]
				self:DivideIntoRegions(iNumCivsOnThisLandmass, fert_table, rect_table)
			else
				print("Invalid number of civs assigned to a landmass: ", iNumCivsOnThisLandmass);
			end
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:BalanceAndAssign()
	-- This function determines what level of Bonus Resource support a location
	-- may need, identifies compatibility with civ-specific biases, and places starts.

	-- Normalize each start plot location.
	local iNumStarts = table.maxn(self.startingPlots);
	for region_number = 1, iNumStarts do
		self:NormalizeStartLocation(region_number)
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
		
		-- Debug
		if table.maxn(westList) ~= iNumCivsInWest then
			print("-"); print("*** ERROR! *** . . . Mismatch between number of Civs on West team and number of civs assigned to west locations.");
		end
		if table.maxn(eastList) ~= iNumCivsInEast then
			print("-"); print("*** ERROR! *** . . . Mismatch between number of Civs on East team and number of civs assigned to east locations.");
		end
		
		local westListShuffled = GetShuffledCopyOfTable(westList)
		local eastListShuffled = GetShuffledCopyOfTable(eastList)
		for region_number, player_ID in ipairs(westListShuffled) do
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];
			local start_plot = Map.GetPlot(x, y)
			local player = Players[player_ID]
			player:SetStartingPlot(start_plot)
		end
		for loop, player_ID in ipairs(eastListShuffled) do
			local x = self.startingPlots[loop + iNumCivsInWest][1];
			local y = self.startingPlots[loop + iNumCivsInWest][2];
			local start_plot = Map.GetPlot(x, y)
			local player = Players[player_ID]
			player:SetStartingPlot(start_plot)
		end
	else
		print("-"); print("This game does not have specific start zone assignments."); print("-");
		local playerList = {};
		for loop = 1, self.iNumCivs do
			local player_ID = self.player_ID_list[loop];
			table.insert(playerList, player_ID);
		end
		local playerListShuffled = GetShuffledCopyOfTable(playerList)
		for region_number, player_ID in ipairs(playerListShuffled) do
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];
			local start_plot = Map.GetPlot(x, y)
			local player = Players[player_ID]
			player:SetStartingPlot(start_plot)
		end
		-- If this is a team game (any team has more than one Civ in it) then make 
		-- sure team members start near each other if possible. (This may scramble 
		-- Civ biases in some cases, but there is no cure).
		if self.bTeamGame == true and team_setting ~= 2 then
			print("However, this IS a team game, so we will try to group team members together."); print("-");
			self:NormalizeTeamLocations()
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetMajorStrategicResourceQuantityValues()
	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 4, 4, 5, 6, 7, 8;
	-- Check the resource setting.
	if self.resource_setting == 1 then -- Sparse
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 4, 4, 4, 5, 5;
	elseif self.resource_setting == 3 then -- Abundant
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 4, 6, 7, 9, 10, 10;
	end
	return uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetSmallStrategicResourceQuantityValues()
	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 1, 2, 2, 2, 3, 3;
	-- Check the resource setting.
	if self.resource_setting == 1 then -- Sparse
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 1, 1, 1, 1, 2, 2;
	elseif self.resource_setting == 3 then -- Abundant
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 3, 3, 3, 3, 3;
	end
	return uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt
end
------------------------------------------------------------------------------
function AssignStartingPlots:CustomOverride(resource_ID)
	-- CUSTOM OPERATION for Skirmish!
	--
	-- This function will identify up to four of the fifteen "Luxury Plot Lists"
	-- that match terrain best suitable for this type of strategic resource.
	print("-"); print("Obtaining indices for Strategic#", resource_ID);
	local primary, secondary, tertiary, quaternary = -1, -1, -1, -1;
	if resource_ID == self.iron_ID then
		primary, secondary, tertiary, quaternary = 4, 5, 10, 14;
	elseif resource_ID == self.horse_ID then
		primary, secondary, tertiary, quaternary = 12, 11, 47, 13;
	elseif resource_ID == self.coal_ID then
		primary, secondary, tertiary, quaternary = 24, 18, 12, 13;
	elseif resource_ID == self.oil_ID then
		primary, secondary, tertiary, quaternary = 10, 2, 8, 17;
	elseif resource_ID == self.aluminum_ID then
		primary, secondary, tertiary, quaternary = 25, 36, 18, 19;
	elseif resource_ID == self.uranium_ID then
		primary, secondary, tertiary, quaternary = 8, 2, 14, 10;
	end
	print("Found indices of", primary, secondary, tertiary, quaternary);
	return primary, secondary, tertiary, quaternary;
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceStrategicAndBonusResources()
	-- KEY: {Resource ID, Quantity (0 = unquantified), weighting, minimum radius, maximum radius}
	-- KEY: (frequency (1 per n plots in the list), impact list number, plot list, resource data)
	--
	-- The radius creates a zone around the plot that other resources of that
	-- type will avoid if possible. See ProcessResourceList for impact numbers.
	--
	-- Order of placement matters, so changing the order may affect a later dependency.
	
	-- Adjust amounts, if applicable, based on Resource Setting.
	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = self:GetMajorStrategicResourceQuantityValues()
	local uran_sm, horse_sm, oil_sm, iron_sm, coal_sm, alum_sm = self:GetSmallStrategicResourceQuantityValues()
	local bonus_multiplier = 1;

	-- Distribute Strategic Resources to Regions -- CUSTOM for Skirmish.
	local dist_list = {
		{self.iron_ID, iron_amt, iron_sm},
		{self.horse_ID, horse_amt, horse_sm},
		{self.coal_ID, coal_amt, coal_sm},
		{self.oil_ID, oil_amt, oil_sm},
		{self.aluminum_ID, alum_amt, alum_sm},
		{self.uranium_ID, uran_amt, uran_sm},
	};
	for loop, reg_data in ipairs(self.regions_sorted_by_type) do
		local region_number = reg_data[1];
		for strategic_loop, res_data_table in ipairs(dist_list) do
			local res_ID = res_data_table[1];
			local res_amt = res_data_table[2];
			local res_sm = res_data_table[3];
			print("-"); print("- - -"); print("Attempting to place strategic resource #", res_ID, "in Region#", region_number);
			local iNumAlreadyPlaced = 0;
			local primary, secondary, tertiary, quaternary, luxury_plot_lists, shuf_list, iNumLeftToPlace;
			primary, secondary, tertiary, quaternary = self:CustomOverride(res_ID);
			luxury_plot_lists = self:GenerateLuxuryPlotListsInRegion(region_number)
			-- Place one large source of each resource type in each region.
			local iNumThisLuxToPlace = 1;
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, res_amt, iNumThisLuxToPlace, 0.3, ImpactLayers.LAYER_NONE, 1, 1, shuf_list);
			if iNumLeftToPlace > 0 and secondary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, res_amt, iNumLeftToPlace, 0.3, ImpactLayers.LAYER_NONE, 1, 1, shuf_list);
			end
			if iNumLeftToPlace > 0 and tertiary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, res_amt, iNumLeftToPlace, 0.4, ImpactLayers.LAYER_NONE, 1, 1, shuf_list);
			end
			if iNumLeftToPlace > 0 and quaternary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, res_amt, iNumLeftToPlace, 0.5, ImpactLayers.LAYER_NONE, 1, 1, shuf_list);
			end
			-- Place one small source of each resource type in each region.
			local iNumThisLuxToPlace = 1;
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, res_sm, iNumThisLuxToPlace, 0.3, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
			if iNumLeftToPlace > 0 and secondary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, res_sm, iNumLeftToPlace, 0.3, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and tertiary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, res_sm, iNumLeftToPlace, 0.4, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and quaternary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, res_sm, iNumLeftToPlace, 0.5, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
			end
		end
	end


	-- Place Bonus Resources
	print("Map Generation - Placing Bonuses");
	self:PlaceFish(10 * bonus_multiplier, self.coast_list);
	self:PlaceSexyBonusAtCivStarts()
	self:AddExtraBonusesToHillsRegions()
	
	local resources_to_place = {}

	if self:IsEvenMoreResourcesActive() == true then
		resources_to_place = {
		{self.deer_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(12 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.extra_deer_list, resources_to_place)
		-- 8
		resources_to_place = {
		{self.deer_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(16 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.tundra_flat_no_feature, resources_to_place)
		-- 12
		
		resources_to_place = {
		{self.wheat_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(20 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.desert_wheat_list, resources_to_place)
		-- 10
		resources_to_place = {
		{self.wheat_ID, 1, 100, 2, 3} };
		self:ProcessResourceList(44 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.plains_flat_no_feature, resources_to_place)
		-- 27
		
		resources_to_place = {
		{self.banana_ID, 1, 100, 0, 1} };
		self:ProcessResourceList(30 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.banana_list, resources_to_place)
		-- 14
		
		resources_to_place = {
		{self.banana_ID, 1, 100, 0, 1} };
		self:ProcessResourceList(40 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.tropical_marsh_list, resources_to_place)
		-- none
		
		resources_to_place = {
		{self.cow_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(30 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.grass_flat_no_feature, resources_to_place)
		-- 18
		
	-- CBP
		resources_to_place = {
		{self.bison_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(24 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.flat_open_no_tundra_no_desert, resources_to_place)
	-- END

		resources_to_place = {
		{self.sheep_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(44 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.hills_open_list, resources_to_place)
		-- 13

		resources_to_place = {
		{self.stone_ID, 1, 100, 1, 1} };
		self:ProcessResourceList(40 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.dry_grass_flat_no_feature, resources_to_place)
		-- 20
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 1, 1} };
		self:ProcessResourceList(40 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.dry_plains_flat_no_feature, resources_to_place)
		-- none
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(30 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.tundra_flat_no_feature, resources_to_place)
		-- 15
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(16 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.desert_flat_no_feature, resources_to_place)
		-- 19
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(36 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.hills_open_list, resources_to_place)
		-- none
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(10 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.snow_flat_list, resources_to_place)
		-- none
		
		resources_to_place = {
		{self.deer_ID, 1, 100, 3, 4} };
		self:ProcessResourceList(50 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.forest_flat_that_are_not_tundra, resources_to_place)
		self:ProcessResourceList(50 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.hills_forest_list, resources_to_place)
		-- 25

	-- Even More Resources for VP start
		resources_to_place = {
		{self.rice_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(24 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.fresh_water_grass_flat_no_feature, resources_to_place)

		resources_to_place = {
		{self.maize_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(32 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.plains_flat_no_feature, resources_to_place)

		resources_to_place = {
		{self.coconut_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(12 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.coconut_list, resources_to_place)

		resources_to_place = {
		{self.hardwood_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(37 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.hills_covered_list, resources_to_place)

		resources_to_place = {
		{self.hardwood_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(37 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.flat_covered, resources_to_place)

		resources_to_place = {
		{self.hardwood_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(29 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.tundra_flat_forest, resources_to_place)

		resources_to_place = {
		{self.lead_ID, 1, 100, 1, 3} };
		self:ProcessResourceList(40 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.dry_grass_flat_no_feature, resources_to_place)

		resources_to_place = {
		{self.lead_ID, 1, 100, 2, 3} };
		self:ProcessResourceList(35 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.hills_open_list, resources_to_place)

		resources_to_place = {
		{self.lead_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(35 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.desert_flat_no_feature, resources_to_place)

		resources_to_place = {
		{self.pineapple_ID, 1, 100, 0, 3} };
		self:ProcessResourceList(29 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.banana_list, resources_to_place)

		resources_to_place = {
		{self.potato_ID, 1, 100, 2, 3} };
		self:ProcessResourceList(40 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.flat_open_no_tundra_no_desert, resources_to_place)

		resources_to_place = {
		{self.potato_ID, 1, 100, 0, 3} };
		self:ProcessResourceList(29 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.hills_open_no_tundra_no_desert, resources_to_place)

		resources_to_place = {
		{self.rubber_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(43 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.banana_list, resources_to_place)

		resources_to_place = {
		{self.sulfur_ID, 1, 100, 1, 3} };
		self:ProcessResourceList(29 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.hills_open_list, resources_to_place)

		resources_to_place = {
		{self.sulfur_ID, 1, 100, 1, 3} };
		self:ProcessResourceList(37 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.hills_covered_list, resources_to_place)

		resources_to_place = {
		{self.sulfur_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(21 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.snow_flat_list, resources_to_place)

		resources_to_place = {
		{self.sulfur_ID, 1, 100, 1, 3} };
		self:ProcessResourceList(43 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.flat_open, resources_to_place)

		resources_to_place = {
		{self.titanium_ID, 1, 100,0, 2} };
		self:ProcessResourceList(56 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.flat_open, resources_to_place)

		resources_to_place = {
		{self.titanium_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(51 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.hills_open_list, resources_to_place)

		resources_to_place = {
		{self.titanium_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(48 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.desert_flat_no_feature, resources_to_place)

		resources_to_place = {
		{self.titanium_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(40 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.tundra_flat_including_forests, resources_to_place)

		resources_to_place = {
		{self.titanium_ID, 1, 100, 0, 1} };
		self:ProcessResourceList(24 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.snow_flat_list, resources_to_place)
	-- Even More Resources for VP end
	else
		resources_to_place = {
		{self.deer_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(6 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.extra_deer_list, resources_to_place)
		-- 8
		resources_to_place = {
		{self.deer_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(8 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.tundra_flat_no_feature, resources_to_place)
		-- 12
		
		resources_to_place = {
		{self.wheat_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(20 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.wheat_list, resources_to_place)
		
		resources_to_place = {
		{self.rice_ID, 1, 100, 0, 1} };
		self:ProcessResourceList(12 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.rice_list, resources_to_place)
		
		resources_to_place = {
		{self.maize_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(24 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.maize_list, resources_to_place)
		
		resources_to_place = {
		{self.banana_ID, 1, 100, 0, 1} };
		self:ProcessResourceList(15 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.banana_list, resources_to_place)
		-- 14
		
		resources_to_place = {
		{self.banana_ID, 1, 100, 0, 1} };
		self:ProcessResourceList(20 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.tropical_marsh_list, resources_to_place)
		-- none
		
		resources_to_place = {
		{self.cow_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(15 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.grass_flat_no_feature, resources_to_place)
		-- 18
		
	-- CBP
		resources_to_place = {
		{self.bison_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(26 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.flat_open_no_tundra_no_desert, resources_to_place)
	-- END

		resources_to_place = {
		{self.sheep_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(18 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.hills_open_list, resources_to_place)
		-- 13

		resources_to_place = {
		{self.stone_ID, 1, 100, 1, 1} };
		self:ProcessResourceList(30 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.dry_grass_flat_no_feature, resources_to_place)
		-- 20
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 1, 1} };
		self:ProcessResourceList(60 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.dry_plains_flat_no_feature, resources_to_place)
		-- none
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(60 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.tundra_flat_no_feature, resources_to_place)
		-- 15
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(13 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.desert_flat_no_feature, resources_to_place)
		-- 19
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(60 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.hills_open_list, resources_to_place)
		-- none
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(8 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.snow_flat_list, resources_to_place)
		-- none
		
		resources_to_place = {
		{self.deer_ID, 1, 100, 3, 4} };
		self:ProcessResourceList(25 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.forest_flat_that_are_not_tundra, resources_to_place)
		self:ProcessResourceList(25 * bonus_multiplier, ImpactLayers.LAYER_BONUS, self.hills_forest_list, resources_to_place)
		-- 25
	end
end
------------------------------------------------------------------------------
function StartPlotSystem()
	-- Custom for Skirmish.
	print("Creating start plot database.");
	local start_plot_database = AssignStartingPlots.Create()
	
	print("Dividing the map in to Regions.");
	local args = {};
	start_plot_database:GenerateRegions()

	print("Choosing start locations for civilizations.");
	start_plot_database:ChooseLocations()
	
	print("Normalizing start locations and assigning them to Players.");
	start_plot_database:BalanceAndAssign()

	print("Placing Natural Wonders.");
	start_plot_database:PlaceNaturalWonders()

	print("Placing Resources and City States.");
	start_plot_database:PlaceResourcesAndCityStates()
end
------------------------------------------------------------------------------
