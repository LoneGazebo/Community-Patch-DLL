-------------------------------------------------------------------------------
--	FILE:	 Europe.lua
--	AUTHOR:  Bob Thomas (Sirian)
--	PURPOSE: Regional map script - Generates a random map similar to Europe and
--	         the Mediterranean basin.
-------------------------------------------------------------------------------
--	Copyright (c) 2012 Firaxis Games, Inc. All rights reserved.
-------------------------------------------------------------------------------

include("MapGenerator");
include("MultilayeredFractal");
include("TerrainGenerator");
include("FeatureGenerator");
include("MapmakerUtilities");

-------------------------------------------------------------------------------
function GetMapScriptInfo()
	local world_age, temperature, rainfall, sea_level, resources = GetCoreMapOptions()
	return {
		Name = "TXT_KEY_MAP_EUROPE",
		Description = "TXT_KEY_MAP_EUROPE_HELP",
		IsAdvancedMap = false,
		IconIndex = 6,
		IconAtlas = "WORLDTYPE_ATLAS_3",
	}
end
-------------------------------------------------------------------------------

------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	-- Europe has fully custom grid sizes to match the slice of Earth being represented.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {20, 16},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {35, 28},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {45, 36},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {55, 44},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {70, 56},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {85, 68}
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
------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------
function MultilayeredFractal:GeneratePlotsByRegion()
	-- Sirian's MultilayeredFractal controlling function.
	-- You -MUST- customize this function for each script using MultilayeredFractal.
	--
	-- This implementation is specific to Mediterranean.
	local iW, iH = Map.GetGridSize();
	-- Initiate plot table, fill all data slots with type PLOT_LAND
	table.fill(self.wholeworldPlotTypes, PlotTypes.PLOT_LAND, iW * iH);
	-- Change western edge and northwest corner to water.
	local west_edge = {};
	--
	local variance = 0;
	local max_variance = math.floor(iW * 0.04);
	--
	local iX, iY = 2, 1;
	while iX <= math.ceil(iW * 0.08) do
		west_edge[iY] = iX;
		west_edge[iY + 1] = iX;
		iY = iY + 2;
		iX = iX + 1;
	end
	for y = iY, math.floor(iH * 0.47) do
		west_edge[y] = math.ceil(iW * 0.08) - 1 + Map.Rand(4, "Roughen coastline - Mediterranean LUA");
	end
	local startX = math.ceil(iW * 0.08);
	local startY = math.ceil(iH * 0.47);
	local endX = math.ceil(iW * 0.7);
	local endY = iH;
	local dx = endX - startX;
	local dy = endY - startY
	local slope = 0;
	if dy ~= 0 then
		slope = dx / dy;
	end
	local x = startX;
	for y = startY, endY do
		x = x + slope;
		west_edge[y] = math.floor(x) + Map.Rand(3, "Roughen coastline - Mediterranean LUA");
	end
	for y = 0, iH - 1 do
		local edge_index = y + 1;
		local edge = west_edge[edge_index];
		for x = 0, edge do
			local i = y * iW + x + 1;
			self.wholeworldPlotTypes[i] = PlotTypes.PLOT_OCEAN;
		end
	end

	-- Add water channel from Atlantic in to Mediterranean Sea
	for x = 0, math.floor(iW * 0.205) do
		local i = math.floor(iH * 0.225) * iW + x + 1;
		self.wholeworldPlotTypes[i] = PlotTypes.PLOT_OCEAN;
	end
	local startX = math.floor(iW * 0.205);
	local startY = math.floor(iH * 0.225);
	local endX = math.ceil(iW * 0.31);
	local endY = math.floor(iH * 0.295);
	local dx = endX - startX;
	local dy = endY - startY;
	local slope = 0;
	if dx ~= 0 then
		slope = dy / dx;
	end
	local y = startY;
	for x = startX, endX do
		y = y + slope;
		local i = math.floor(y) * iW + x + 1;
		self.wholeworldPlotTypes[i] = PlotTypes.PLOT_OCEAN;
		self.wholeworldPlotTypes[i + 1] = PlotTypes.PLOT_OCEAN;
	end

	-- Add layers of seas to similate the Mediterranean

	local args = {};
	args.iWaterPercent = 78;
	args.iRegionWidth = math.ceil(iW * 0.23);
	args.iRegionHeight = math.ceil(iH * 0.13);
	args.iRegionWestX = math.floor(iW * 0.035);
	args.iRegionSouthY = math.floor(iH * 0.155);
	args.iRegionGrain = 1;
	args.iRegionHillsGrain = 4;
	args.iRegionPlotFlags = self.iRoundFlags;
	args.iRegionFracXExp = 6;
	args.iRegionFracYExp = 5;
	args.iRiftGrain = Map.Rand(3, "Rift Grain - Mediterranean LUA");
	--args.bShift;

	self:GenerateWaterLayer(args)

	local args = {};
	args.iWaterPercent = 80;
	args.iRegionWidth = math.ceil(iW * 0.23);
	args.iRegionHeight = math.ceil(iH * 0.13);
	args.iRegionWestX = math.floor(iW * 0.035);
	args.iRegionSouthY = math.floor(iH * 0.155);
	args.iRegionGrain = 2;
	args.iRegionHillsGrain = 4;
	args.iRegionPlotFlags = self.iRoundFlags;
	args.iRegionFracXExp = 6;
	args.iRegionFracYExp = 5;
	args.iRiftGrain = Map.Rand(3, "Rift Grain - Mediterranean LUA");
	--args.bShift;

	self:GenerateWaterLayer(args)

	local args = {};
	args.iWaterPercent = 78;
	args.iRegionWidth = math.ceil(iW * 0.12);
	args.iRegionHeight = math.ceil(iH * 0.12);
	args.iRegionWestX = math.floor(iW * 0.2);
	args.iRegionSouthY = math.floor(iH * 0.23);
	args.iRegionGrain = 2;
	args.iRegionHillsGrain = 4;
	args.iRegionPlotFlags = self.iRoundFlags;
	args.iRegionFracXExp = 5;
	args.iRegionFracYExp = 5;
	args.iRiftGrain = Map.Rand(3, "Rift Grain - Mediterranean LUA");
	--args.bShift;

	self:GenerateWaterLayer(args)

	local args = {};
	args.iWaterPercent = 65;
	args.iRegionWidth = math.ceil(iW * 0.30);
	args.iRegionHeight = math.ceil(iH * 0.27);
	args.iRegionWestX = math.floor(iW * 0.275);
	args.iRegionSouthY = math.floor(iH * 0.215);
	args.iRegionGrain = 2;
	args.iRegionHillsGrain = 4;
	args.iRegionPlotFlags = self.iRoundFlags;
	args.iRegionFracXExp = 5;
	args.iRegionFracYExp = 5;
	args.iRiftGrain = Map.Rand(3, "Rift Grain - Mediterranean LUA");
	--args.bShift;
	
	self:GenerateWaterLayer(args)

	local args = {};
	args.iWaterPercent = 75;
	args.iRegionWidth = math.ceil(iW * 0.36);
	args.iRegionHeight = math.ceil(iH * 0.2);
	args.iRegionWestX = math.floor(iW * 0.215);
	args.iRegionSouthY = math.floor(iH * 0.2);
	args.iRegionGrain = 2;
	args.iRegionHillsGrain = 4;
	args.iRegionPlotFlags = self.iRoundFlags;
	args.iRegionFracXExp = 6;
	args.iRegionFracYExp = 5;
	args.iRiftGrain = Map.Rand(3, "Rift Grain - Mediterranean LUA");
	--args.bShift;
	
	self:GenerateWaterLayer(args)

	local args = {};
	args.iWaterPercent = 83;
	args.iRegionWidth = math.ceil(iW * 0.36);
	args.iRegionHeight = math.ceil(iH * 0.27);
	args.iRegionWestX = math.floor(iW * 0.215);
	args.iRegionSouthY = math.floor(iH * 0.215);
	args.iRegionGrain = 1;
	args.iRegionHillsGrain = 4;
	args.iRegionPlotFlags = self.iRoundFlags;
	args.iRegionFracXExp = 6;
	args.iRegionFracYExp = 5;
	args.iRiftGrain = Map.Rand(3, "Rift Grain - Mediterranean LUA");
	--args.bShift;
	
	self:GenerateWaterLayer(args)

	local args = {};
	args.iWaterPercent = 60;
	args.iRegionWidth = math.ceil(iW * 0.48);
	args.iRegionHeight = math.ceil(iH * 0.23);
	args.iRegionWestX = math.floor(iW * 0.415);
	args.iRegionSouthY = math.floor(iH * 0.08);
	args.iRegionGrain = 2;
	args.iRegionHillsGrain = 4;
	args.iRegionPlotFlags = self.iRoundFlags;
	args.iRegionFracXExp = 6;
	args.iRegionFracYExp = 5;
	--args.iRiftGrain = -1;
	--args.bShift;
	
	self:GenerateWaterLayer(args)

	local args = {};
	args.iWaterPercent = 72;
	args.iRegionWidth = math.ceil(iW * 0.48);
	args.iRegionHeight = math.ceil(iH * 0.23);
	args.iRegionWestX = math.floor(iW * 0.415);
	args.iRegionSouthY = math.floor(iH * 0.08);
	args.iRegionGrain = 2;
	args.iRegionHillsGrain = 4;
	args.iRegionPlotFlags = self.iRoundFlags;
	args.iRegionFracXExp = 6;
	args.iRegionFracYExp = 5;
	--args.iRiftGrain = -1;
	--args.bShift;
	
	self:GenerateWaterLayer(args)


	-- Simulate the Black Sea
	local args = {};
	args.iWaterPercent = 63;
	args.iRegionWidth = math.ceil(iW * 0.29);
	args.iRegionHeight = math.ceil(iH * 0.20);
	args.iRegionWestX = math.floor(iW * 0.68);
	args.iRegionSouthY = math.floor(iH * 0.375);
	args.iRegionGrain = 1;
	args.iRegionHillsGrain = 4;
	args.iRegionPlotFlags = self.iRoundFlags;
	args.iRegionFracXExp = 6;
	args.iRegionFracYExp = 5;
	--args.iRiftGrain = -1;
	--args.bShift;
	
	self:GenerateWaterLayer(args)

	local args = {};
	args.iWaterPercent = 70;
	args.iRegionWidth = math.ceil(iW * 0.29);
	args.iRegionHeight = math.ceil(iH * 0.20);
	args.iRegionWestX = math.floor(iW * 0.68);
	args.iRegionSouthY = math.floor(iH * 0.375);
	args.iRegionGrain = 1;
	args.iRegionHillsGrain = 4;
	args.iRegionPlotFlags = self.iRoundFlags;
	args.iRegionFracXExp = 6;
	args.iRegionFracYExp = 5;
	--args.iRiftGrain = -1;
	--args.bShift;
	
	self:GenerateWaterLayer(args)


	-- Generate British Isles		
	local args = {};
	args.iWaterPercent = 57;
	args.iRegionWidth = math.ceil(iW * 0.19);
	args.iRegionHeight = math.ceil(iH * 0.27);
	args.iRegionWestX = math.floor(iW * 0.055);
	args.iRegionSouthY = math.floor(iH * 0.61);
	args.iRegionGrain = 1 + Map.Rand(2, "Continental Grain - Mediterranean LUA");
	args.iRegionHillsGrain = 4;
	args.iRegionPlotFlags = self.iRoundFlags;
	args.iRegionFracXExp = 6;
	args.iRegionFracYExp = 5;
	--args.iRiftGrain = Map.Rand(3, "Rift Grain - Mediterranean LUA");
	--args.bShift
	
	self:GenerateFractalLayerWithoutHills(args)


	-- Generate Scandinavia		
	local args = {};
	args.iWaterPercent = 55;
	args.iRegionWidth = math.ceil(iW * 0.37);
	args.iRegionHeight = math.ceil(iH * 0.25);
	args.iRegionWestX = math.floor(iW * 0.275);
	args.iRegionSouthY = (iH - 1) - args.iRegionHeight;
	args.iRegionGrain = 1;
	args.iRegionHillsGrain = 4;
	args.iRegionPlotFlags = self.iRoundFlags;
	args.iRegionFracXExp = 6;
	args.iRegionFracYExp = 5;
	--args.iRiftGrain = Map.Rand(3, "Rift Grain - Mediterranean LUA");
	--args.bShift
	
	self:GenerateFractalLayerWithoutHills(args)
	
	local args = {};
	args.iWaterPercent = 55;
	args.iRegionWidth = math.ceil(iW * 0.37);
	args.iRegionHeight = math.ceil(iH * 0.25);
	args.iRegionWestX = math.floor(iW * 0.275);
	args.iRegionSouthY = (iH - 1) - args.iRegionHeight;
	args.iRegionGrain = 1;
	args.iRegionHillsGrain = 4;
	args.iRegionPlotFlags = self.iRoundFlags;
	args.iRegionFracXExp = 6;
	args.iRegionFracYExp = 5;
	--args.iRiftGrain = Map.Rand(3, "Rift Grain - Mediterranean LUA");
	--args.bShift
	
	self:GenerateFractalLayerWithoutHills(args)
	
	local args = {};
	args.iWaterPercent = 60;
	args.iRegionWidth = math.ceil(iW * 0.37);
	args.iRegionHeight = math.ceil(iH * 0.25);
	args.iRegionWestX = math.floor(iW * 0.275);
	args.iRegionSouthY = (iH - 1) - args.iRegionHeight;
	args.iRegionGrain = 2;
	args.iRegionHillsGrain = 4;
	args.iRegionPlotFlags = self.iRoundFlags;
	args.iRegionFracXExp = 6;
	args.iRegionFracYExp = 5;
	--args.iRiftGrain = Map.Rand(3, "Rift Grain - Mediterranean LUA");
	--args.bShift
	
	self:GenerateFractalLayerWithoutHills(args)
	
	-- The real Scandinavia continues north of the Arctic circle. We want our simulated one to do so as well.
	-- But the layers tend to leave the top row all water, so this will try to address that.
	for x = math.floor(iW * 0.29), math.floor(iW * 0.62) do
		local y = iH - 2;
		local i = y * iW + x + 1;
		if self.wholeworldPlotTypes[i] == PlotTypes.PLOT_LAND then
			self.wholeworldPlotTypes[i + iW] = PlotTypes.PLOT_LAND;
		end
	end
	for x = math.floor(iW * 0.29), math.floor(iW * 0.62) do
		local y = iH - 1;
		local i = y * iW + x + 1;
		if self.wholeworldPlotTypes[i] == PlotTypes.PLOT_OCEAN then
			if self.wholeworldPlotTypes[i + 1] == PlotTypes.PLOT_LAND and self.wholeworldPlotTypes[i - 1] == PlotTypes.PLOT_LAND then
				self.wholeworldPlotTypes[i] = PlotTypes.PLOT_LAND;
			end
		end
	end

	-- Simulate the Baltic Sea
	local args = {};
	args.iWaterPercent = 70;
	args.iRegionWidth = math.ceil(iW * 0.25);
	args.iRegionHeight = math.ceil(iH * 0.27);
	args.iRegionWestX = math.floor(iW * 0.37);
	args.iRegionSouthY = math.floor(iH * 0.65);
	args.iRegionGrain = 2;
	args.iRegionHillsGrain = 4;
	args.iRegionPlotFlags = self.iRoundFlags;
	args.iRegionFracXExp = 6;
	args.iRegionFracYExp = 5;
	--args.iRiftGrain = -1;
	--args.bShift;
	
	self:GenerateWaterLayer(args)


	-- Land and water are set. Apply hills and mountains.
	self:ApplyTectonics()

	-- Plot Type generation completed. Return global plot array.
	return self.wholeworldPlotTypes
end
------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Setting Plot Types (Lua Europe) ...");

	local layered_world = MultilayeredFractal.Create();
	local plotsEur = layered_world:GeneratePlotsByRegion();
	
	SetPlotTypes(plotsEur);

	GenerateCoasts();
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- Europe uses a custom terrain generation.
------------------------------------------------------------------------------
function GenerateTerrain()
	print("Generating Terrain (Lua Europe) ...");
	Map.RecalculateAreas();
	
	local iW, iH = Map.GetGridSize();
	local terrainTypes = {};
	local iFlags = {};
	local grainvalues = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 4
		}
	local grain_amount = grainvalues[Map.GetWorldSize()];

	local fDesertLine = 0.185;
	local fIceLine = 0.92;
	local fTundraLine = 0.75;
	local fGrassLine = 0.25;

	local iSnowPercent = 85;
	local iTundraPercent = 85;
	local iGrassPercent = 60;

	local snow_frac = Fractal.Create(iW, iH, grain_amount + 1, iFlags, -1, -1);
	local tundra_frac = Fractal.Create(iW, iH, grain_amount + 1, iFlags, -1, -1);
	local grass_frac = Fractal.Create(iW, iH, grain_amount, iFlags, -1, -1);
	local variation = Fractal.Create(iW, iH, grain_amount + 1, iFlags, -1, -1);

	local iSnowTop = snow_frac:GetHeight(iSnowPercent);
	local iTundraTop = tundra_frac:GetHeight(iTundraPercent);
	local iGrassTop = grass_frac:GetHeight(iGrassPercent);

	local terrainDesert	= GameInfoTypes["TERRAIN_DESERT"];
	local terrainPlains	= GameInfoTypes["TERRAIN_PLAINS"];
	local terrainGrass	= GameInfoTypes["TERRAIN_GRASS"];	
	local terrainTundra	= GameInfoTypes["TERRAIN_TUNDRA"];	
	local terrainSnow	= GameInfoTypes["TERRAIN_SNOW"];	

	-- Main loop, generate the terrain plot by plot.
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local i = y * iW + x; -- C++ Plot indices, starting at 0.
			local plot = Map.GetPlot(x, y);
			local terrainVal;

			-- Handle water plots
			if plot:IsWater() then
				terrainVal = plot:GetTerrainType();

			-- Handle land plots; begin with checking if plot is coastal.
			else
				-- Set latitude at plot
				local lat = y / iH; -- 0.0 = south
				lat = lat + (128 - variation:GetHeight(x, y))/(255.0 * 5.0);
				if lat < 0 then
					lat = 0;
				elseif lat > 1 then
					lat = 1;
				end

				-- Check all adjacent plots to see if any of those are salt water.
				local adjacent_water_count = 0;
				local directions = { DirectionTypes.DIRECTION_NORTHEAST,
				                     DirectionTypes.DIRECTION_EAST,
	    			                 DirectionTypes.DIRECTION_SOUTHEAST,
	                			     DirectionTypes.DIRECTION_SOUTHWEST,
				                     DirectionTypes.DIRECTION_WEST,
	    			                 DirectionTypes.DIRECTION_NORTHWEST };
				-- 
				for loop, current_direction in ipairs(directions) do
					local testPlot = Map.PlotDirection(x, y, current_direction);
					if testPlot ~= nil then
						local type = testPlot:GetPlotType()
						if type == PlotTypes.PLOT_OCEAN then -- Adjacent plot is water! Check if ocean or lake.
							-- Have to do a manual check of area size, because lakes have not yet been defined as such.
							local testAreaID = testPlot:GetArea()
							local testArea = Map.GetArea(testAreaID)
							local testArea_size = testArea:GetNumTiles()
							if testArea_size >= 10 then
								adjacent_water_count = adjacent_water_count + 1;
							end
						end
					end
				end
				
				-- Check count of adjacent saltwater tiles. If none, the plot is inland. If not none, the plot is coastal 
				-- and will be turned in to grassland, except in the far north, where only some of the tiles are turned to grass.
				if adjacent_water_count > 0 and x < iH - 1 then
					-- Coastal Plot
					if lat < 0.785 then -- Make it grass.
						terrainVal = terrainGrass;
					else -- Far north, roll dice to see if we make it grass. (More chance, the more adjacent tiles are water.)
						local diceroll = 1 + Map.Rand(6, "Subarctic coastland, grass die roll - Mediterranean LUA");
						if diceroll <= adjacent_water_count then -- Make this tile grass.
							terrainVal = terrainGrass;
						else
							terrainVal = terrainTundra;
						end
					end

				elseif lat <= fDesertLine then
					terrainVal = terrainDesert;
				elseif lat >= fIceLine then
					local val = snow_frac:GetHeight(x, y);
					if val >= iSnowTop then
						terrainVal = terrainTundra;
					else
						terrainVal = terrainSnow;
					end
				elseif lat >= fTundraLine then
					local val = tundra_frac:GetHeight(x, y);
					if val >= iTundraTop then
						terrainVal = terrainPlains;
					else
						terrainVal = terrainTundra;
					end
				elseif lat >= fGrassLine then
					local val = grass_frac:GetHeight(x, y);
					if val >= iGrassTop then
						terrainVal = terrainPlains;
					else
						terrainVal = terrainGrass;
					end
				else
					terrainVal = terrainPlains;
				end
			end
			
			-- Input result of this plot to terrain types array
			terrainTypes[i] = terrainVal;
		end
	end
	
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
		if (adjacentPlot ~= nil) then
			sum = sum + (numPlots - adjacentPlot:GetPlotType());
		else
			sum = 0
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

------------------------------------------------------------------------------
function FeatureGenerator:AddIceAtPlot(plot, iX, iY, lat)
	-- Do nothing. No ice to be placed.
end
------------------------------------------------------------------------------
function FeatureGenerator:AddJunglesAtPlot(plot, iX, iY, lat)
	-- Do nothing. No jungle to be placed.
end
------------------------------------------------------------------------------
function FeatureGenerator:AddAtolls()
	-- Do nothing. No atolls to be placed.
end
------------------------------------------------------------------------------
function AddFeatures()
	print("Adding Features (Lua Europe) ...");
	local featuregen = FeatureGenerator.Create();

	featuregen:AddFeatures(false);
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function AssignStartingPlots:__CustomInit()
	-- This function included to provide a quick and easy override for changing 
	-- any initial settings. Add your customized version to the map script.
	self.CanBeGibraltar = AssignStartingPlots.CanBeGibraltar;
	self.CanBeMtSinai = AssignStartingPlots.CanBeMtSinai;
	self.gibraltar_list, self.sinai_list = {}, {};
end	
------------------------------------------------------------------------------
function AssignStartingPlots:GenerateRegions(args)
	-- Custom method for Europe. Will start a civ in Britain if eight or more civs in the game.
	print("Map Generation - Dividing the map in to Regions");
	local args = args or {};
	local iW, iH = Map.GetGridSize();
	self.method = RegionDivision.BIGGEST_LANDMASS;

	-- Determine number of civilizations and city states present in this game.
	self.iNumCivs, self.iNumCityStates, self.player_ID_list, self.bTeamGame, self.teams_with_major_civs, self.number_civs_per_team = GetPlayerAndTeamInfo()
	self.iNumCityStatesUnassigned = self.iNumCityStates;

	local iNumCivsRemaining = self.iNumCivs;
	if self.iNumCivs >= 8 then -- Place one civ in the British Isles.
		self.inhabited_WestX = math.floor(iW * 0.055);
		self.inhabited_SouthY = math.floor(iH * 0.61);
		self.inhabited_Width = math.ceil(iW * 0.19);
		self.inhabited_Height = math.ceil(iH * 0.27);
		local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityInRectangle(self.inhabited_WestX, 
		                                         self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height)
		local rect_table = {self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, 
		                    self.inhabited_Height, -1, fertCount, plotCount}; -- AreaID -1 means ignore area IDs.
		self:DivideIntoRegions(1, fert_table, rect_table)
		iNumCivsRemaining = iNumCivsRemaining - 1;
	end
	-- Identify the biggest landmass.
	local biggest_area = Map.FindBiggestArea(False);
	local iAreaID = biggest_area:GetID();
	-- We'll need all eight data fields returned in the results table from the boundary finder:
	local landmass_data = ObtainLandmassBoundaries(iAreaID);
	local iWestX = landmass_data[1];
	local iSouthY = landmass_data[2];
	local iEastX = landmass_data[3];
	local iNorthY = landmass_data[4];
	local iWidth = landmass_data[5];
	local iHeight = landmass_data[6];
	local wrapsX = landmass_data[7];
	local wrapsY = landmass_data[8];
	-- Obtain "Start Placement Fertility" of the landmass.
	local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityOfLandmass(iAreaID, 
	                                         iWestX, iEastX, iSouthY, iNorthY, wrapsX, wrapsY);
	-- Assemble the Rectangle data table:
	local rect_table = {iWestX, iSouthY, iWidth, iHeight, iAreaID, fertCount, plotCount};
	-- The data from this call is processed in to self.regionData during the process.
	self:DivideIntoRegions(iNumCivsRemaining, fert_table, rect_table)
end
------------------------------------------------------------------------------
function AssignStartingPlots:ExaminePlotForNaturalWondersEligibility(x, y)
	-- This function checks only for eligibility requirements applicable to all 
	-- Natural Wonders. If a candidate plot passes all such checks, we will move
	-- on to checking it against specific needs for each particular wonderID.
	--
	-- Update, May 2011: Control over wonderID placement is being migrated to XML. Some checks here moved to there.
	local iW, iH = Map.GetGridSize();
	local plotIndex = iW * y + x + 1;
	
	-- Check for collision with player starts
	if self.impactData[ImpactLayers.LAYER_NATURAL_WONDER][plotIndex] > 0 then
		return false, false;
	end
	
	-- Custom for Europe.
	if not (((x >= iW * 0.1 and x <= iW * 0.3) and (y >= iH * 0.15 and y <= iH * 0.35)) or (x >= iW * 0.8 and y <= iH * 0.2)) then
		return false
	end
	
	-- Check the location is a decent city site, otherwise the wonderID is pointless
	local plot = Map.GetPlot(x, y);
	local fertility = self:Plot_GetFertilityInRange(plot, 3);
	if fertility < 20 then
		return false, false;
	elseif fertility < 28 then
		return false, true;
	end
	return true, true;
end
------------------------------------------------------------------------------
function AssignStartingPlots:CanBeGibraltar(x, y)
	-- Checks a candidate plot for eligibility to be Rock of Gibraltar.
	local plot = Map.GetPlot(x, y);
	-- Checking center plot, which must be in the water or on the coast.
	local iW, iH = Map.GetGridSize();
	local plotIndex = y * iW + x + 1;
	if self.plotDataIsCoastal[plotIndex] == false and plot:IsWater() == false then
		return
	end
	-- Now process the surrounding plots. Desert is not tolerable. We don't want too many mountains or plains.
	-- We are looking for a site that does not have unwanted traits but does have jungles or hills.
	local iNumLand, iNumCoast = 0, 0;
	for loop, direction in ipairs(self.direction_types) do
		local adjPlot = Map.PlotDirection(x, y, direction)
		if adjPlot == nil then
			return
		end
		local plotType = adjPlot:GetPlotType();
		local terrainType = adjPlot:GetTerrainType()
		local featureType = adjPlot:GetFeatureType()
		if terrainType == TerrainTypes.TERRAIN_COAST and plot:IsLake() == false then
			if featureType == FeatureTypes.NO_FEATURE then
				iNumCoast = iNumCoast + 1;
			end
		end
		if plotType ~= PlotTypes.PLOT_OCEAN then
			iNumLand = iNumLand + 1;
		end
	end
	-- If too much land, reject this site.
	if iNumLand ~= 1 then
		return
	end
	-- If not enough coast, reject this site.
	if iNumCoast < 4 then
		return
	end
	-- This site is good.
	table.insert(self.gibraltar_list, plotIndex);
end
------------------------------------------------------------------------------
function AssignStartingPlots:CanBeMtSinai(x, y)
	-- Checks a candidate plot for eligibility to be Mt Sinai.
	local plot = Map.GetPlot(x, y);
	-- Checking center plot, which must be at least one plot away from any salt water.
	if plot:IsWater() then
		return
	end
	local iW, iH = Map.GetGridSize();
	local plotIndex = y * iW + x + 1;
	if self.plotDataIsCoastal[plotIndex] == true then
		return
	end
	local terrainType = plot:GetTerrainType()
	local iNumMountains, iNumHills, iNumDesert = 0, 0, 0;
	local plotType = plot:GetPlotType();
	if plotType == PlotTypes.PLOT_MOUNTAIN then
		iNumMountains = iNumMountains + 1;
	elseif plotType == PlotTypes.PLOT_HILLS then
		iNumHills = iNumHills + 1;
	end
	-- Now process the surrounding plots.
	for loop, direction in ipairs(self.direction_types) do
		local adjPlot = Map.PlotDirection(x, y, direction)
		if adjPlot == nil or adjPlot:IsLake() then
			return
		end
		terrainType = adjPlot:GetTerrainType()
		if terrainType == TerrainTypes.TERRAIN_DESERT then
			iNumDesert = iNumDesert + 1;
		end
		plotType = adjPlot:GetPlotType();
		if plotType == PlotTypes.PLOT_MOUNTAIN then
			iNumMountains = iNumMountains + 1;
		elseif plotType == PlotTypes.PLOT_HILLS then
			iNumHills = iNumHills + 1;
		end
	end
	-- If not enough desert, reject this site.
	if iNumDesert < 2 then
		return
	end
	-- If too many mountains, reject this site.
	if iNumMountains > 3 then
		return
	end
	-- If not enough hills, reject this site.
	if iNumHills + iNumMountains < 1 then
		return
	end
	table.insert(self.sinai_list, plotIndex);
end
------------------------------------------------------------------------------
function AssignStartingPlots:GenerateNaturalWondersCandidatePlotLists()
	-- This function scans the map for eligible sites for all "Natural Wonders" Features.
	local iW, iH = Map.GetGridSize();
	-- Set up Landmass check for Mount Fuji (it's not to be on the biggest landmass, if the world has oceans).
	local biggest_landmass = Map.FindBiggestArea(false)
	self.iBiggestLandmassID = biggest_landmass:GetID()
	local biggest_ocean = Map.FindBiggestArea(true)
	local iNumBiggestOceanPlots = biggest_ocean:GetNumTiles()
	if iNumBiggestOceanPlots > (iW * iH) / 4 then
		self.bWorldHasOceans = true;
	else
		self.bWorldHasOceans = false;
	end
	-- Main loop
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local landEligibility, seaEligibility = self:ExaminePlotForNaturalWondersEligibility(x, y);
			if seaEligibility == true then
				self:CanBeGibraltar(x, y);
			end
			if landEligibility == true then
				self:CanBeMtSinai(x, y);
			end
		end
	end
	-- Eligibility will affect which NWs can be used, and number of candidates will affect placement order.
	local iCanBeGibraltar = table.maxn(self.gibraltar_list);
	local iCanBeSinai = table.maxn(self.sinai_list);

	-- Sort the wonders with fewest candidates listed first.
	-- If the Geyser is eligible, always choose it and give it top priority.
	local NW_eligibility_order, NW_eligibility_unsorted, NW_eligibility_sorted = {}, {}, {}; 
	if iCanBeGibraltar > 0 then
		table.insert(NW_eligibility_unsorted, {3, iCanBeGibraltar});
		table.insert(NW_eligibility_sorted, iCanBeGibraltar);
	end
	if iCanBeSinai > 0 then
		table.insert(NW_eligibility_unsorted, {11, iCanBeSinai});
		table.insert(NW_eligibility_sorted, iCanBeSinai);
	end
	table.sort(NW_eligibility_sorted);
	
	-- Match each sorted eligibility count to the matching unsorted NW number and record in sequence.
	for NW_order = 1, 2 do
		for loop, data_pair in ipairs(NW_eligibility_unsorted) do
			local unsorted_count = data_pair[2];
			if NW_eligibility_sorted[NW_order] == unsorted_count then
				local unsorted_NW_num = data_pair[1];
				table.insert(NW_eligibility_order, unsorted_NW_num);
				table.remove(NW_eligibility_unsorted, loop);
				break
			end
		end
	end
	
	-- Debug printout of natural wonder candidate plot lists
	print("-"); print("-"); print("--- Number of Candidate Plots on the map for Natural Wonders ---"); print("-");
	print("- Gibraltar:", iCanBeGibraltar);
	print("- Mt Sinai:", iCanBeSinai);
	print("-"); print("--- End of candidates readout for Natural Wonders ---"); print("-");	
	--

	return NW_eligibility_order;
end
------------------------------------------------------------------------------
function AssignStartingPlots:AttemptToPlaceNaturalWonder(iNaturalWonderNumber)
	-- Attempt to place a specific Natural Wonder.
	-- 1 Everest - 2 Crater - 3 Titicaca - 4 Fuji - 5 Mesa - 6 Reef - 7 Krakatoa (unforced) - 8 Krakatoa (forced)
	local iW, iH = Map.GetGridSize();
	local wonder_list = table.fill(-1, 8);
	for thisFeature in GameInfo.Features() do
		if thisFeature.Type == "FEATURE_MT_SINAI" then
			wonder_list[11] = thisFeature.ID;
		elseif thisFeature.Type == "FEATURE_GIBRALTAR" then
			wonder_list[3] = thisFeature.ID;
		end
	end

	if iNaturalWonderNumber == 3 then -- Gibraltar
		local candidate_plot_list = GetShuffledCopyOfTable(self.gibraltar_list)
		for loop, plotIndex in ipairs(candidate_plot_list) do
			if self.naturalWondersData[plotIndex] == 0 then -- No collision with civ start or other NW, so place Titicaca here!
				local x = (plotIndex - 1) % iW;
				local y = (plotIndex - x - 1) / iW;
				local plot = Map.GetPlot(x, y);
				-- Where it does not already, force the local terrain to conform to what the NW needs.
				plot:SetPlotType(PlotTypes.PLOT_LAND, false, false);
				plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, false)
				for loop, direction in ipairs(self.direction_types) do
					local adjPlot = Map.PlotDirection(x, y, direction)
					if adjPlot:GetPlotType() == PlotTypes.PLOT_OCEAN then
						if adjPlot:GetTerrainType() ~= TerrainTypes.TERRAIN_COAST then
							adjPlot:SetTerrainType(TerrainTypes.TERRAIN_COAST, false, false)
						end
					else
						if adjPlot:GetPlotType() ~= PlotTypes.PLOT_LAND then
							adjPlot:SetPlotType(PlotTypes.PLOT_LAND, false, false);
						end
					end
				end
				-- Now place Gibraltar and record the placement.
				plot:SetFeatureType(wonder_list[3])
				table.insert(self.placed_natural_wonder, 3);

				-- Force no resource on Natural Wonders.
				self:PlaceStrategicResourceImpact(x, y, 0);
				self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_LUXURY, 0);
				self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_BONUS, 0);
				self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_FISH, 0);
				self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_MARBLE, 0);

				-- Keep City States away from Natural Wonders!
				self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_CITY_STATE, 4);

				local plotIndex = y * iW + x + 1;
				self.playerCollisionData[plotIndex] = true;				-- Record exact plot of wonder in the collision list.
				--
				--print("- Placed Gibraltar in Plot", x, y);
				--
				return true
			end
		end
		-- If reached here, Gibraltar was unable to be placed because all candidates are too close to an already-placed NW.
		return false

	elseif iNaturalWonderNumber == 11 then -- MtSinai
		local candidate_plot_list = GetShuffledCopyOfTable(self.sinai_list)
		for loop, plotIndex in ipairs(candidate_plot_list) do
			if self.naturalWondersData[plotIndex] == 0 then -- No collision with civ start or other NW, so place Mesa here!
				local x = (plotIndex - 1) % iW;
				local y = (plotIndex - x - 1) / iW;
				local plot = Map.GetPlot(x, y);
				-- Where it does not already, force the local terrain to conform to what the NW needs.
				if not plot:IsMountain() then
					plot:SetPlotType(PlotTypes.PLOT_MOUNTAIN, false, false);
				end
				if plot:GetTerrainType() ~= TerrainTypes.TERRAIN_DESERT then
					plot:SetTerrainType(TerrainTypes.TERRAIN_DESERT, false, false)
				end
				-- Now place Mesa and record the placement.
				plot:SetFeatureType(wonder_list[11])
				table.insert(self.placed_natural_wonder, 11);

				-- Force no resource on Natural Wonders.
				self:PlaceStrategicResourceImpact(x, y, 0);
				self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_LUXURY, 0);
				self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_BONUS, 0);
				self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_FISH, 0);
				self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_MARBLE, 0);

				-- Keep City States away from Natural Wonders!
				self:PlaceResourceImpact(x, y, ImpactLayers.LAYER_CITY_STATE, 4);

				local plotIndex = y * iW + x + 1;
				self.playerCollisionData[plotIndex] = true;				-- Record exact plot of wonder in the collision list.
				return true
			end
		end
		return false

	end
	print("Unsupported Natural Wonder Number:", iNaturalWonderNumber);
	return false
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceNaturalWonders()
	local NW_eligibility_order = self:GenerateNaturalWondersCandidatePlotLists()
	local iNumNWCandidates = table.maxn(NW_eligibility_order);
	if iNumNWCandidates == 0 then
		--print("No Natural Wonders placed, no eligible sites found for any of them.");
		return
	end
	local iNumNWtoPlace = 2;
	local selected_NWs, fallback_NWs = {}, {};
	local iNumSelectedSoFar = 0;
	-- If Geyser is eligible, always choose it. (This is because its eligibility requirements are so much steeper.)
	if NW_eligibility_order[1] == 1 then
		table.insert(selected_NWs, NW_eligibility_order[1]);
		--[[ This was a section to give a second NW the "always choose" priority, but that wonder got changed.
		if NW_eligibility_order[2] == 3 then
			table.insert(selected_NWs, 3);
			table.remove(NW_eligibility_order, 2);
			iNumSelectedSoFar = iNumSelectedSoFar + 1;
		end
		]]--
		table.remove(NW_eligibility_order, 1);
		iNumSelectedSoFar = iNumSelectedSoFar + 1;
	end
	-- Choose a random selection from the others, to reach the quota to place. If any left over, set as fallbacks.
	local NW_shuffled_order = GetShuffledCopyOfTable(NW_eligibility_order);
	for loop, NW in ipairs(NW_eligibility_order) do
		for test_loop, shuffled_NW in ipairs(NW_shuffled_order) do
			if shuffled_NW == NW then
				if test_loop <= iNumNWtoPlace - iNumSelectedSoFar then
					table.insert(selected_NWs, NW);
				else
					table.insert(fallback_NWs, NW);
				end
			end
		end
	end
	-- Place the NWs
	local iNumPlaced = 0;
	for loop, NW in ipairs(selected_NWs) do
		local bSuccess = self:AttemptToPlaceNaturalWonder(NW)
		if bSuccess then
			iNumPlaced = iNumPlaced + 1;
		end
	end
	if iNumPlaced < iNumNWtoPlace then
		for loop, NW in ipairs(fallback_NWs) do
			if iNumPlaced >= iNumNWtoPlace then
				break
			end
			local bSuccess = self:AttemptToPlaceNaturalWonder(NW)
			if bSuccess then
				iNumPlaced = iNumPlaced + 1;
			end
		end
	end
end
------------------------------------------------------------------------------
function ExtraEuropeCheck1(ASP)
	-- Adding this check for Europe in AssignStartingPlots:AssignCityStatesToRegionsOrToUninhabited, so no CS is assigned to Britian if a civ is assigned there.
	local iNumCityStatesPerRegion = ASP:GetNumCityStatesPerRegion();
	local starting_region_number = 1;
	if ASP.iNumCivs >= 8 then -- Civ in Britain
		starting_region_number = 2;
	end
	local current_cs_index = 1;
	if iNumCityStatesPerRegion > 0 then
		for current_region = starting_region_number, ASP.iNumCivs do
			for _ = 1, iNumCityStatesPerRegion do
				ASP.city_state_region_assignments[current_cs_index] = current_region;
				--print("-"); print("City State", current_cs_index, "assigned to Region#", current_region);
				current_cs_index = current_cs_index + 1;
				ASP.iNumCityStatesUnassigned = ASP.iNumCityStatesUnassigned - 1;
			end
		end
	end
end
------------------------------------------------------------------------------
function ExtraEuropeCheck2(ASP, x, y)
	-- Adding this check for Europe in AssignStartingPlots:CanPlaceCityStateAt, to keep CS out of British Isles when a Civ is there.
	local iW, iH = Map.GetGridSize();
	if ASP.iNumCivs >= 8 then
		if x <= math.floor(iW * 0.055) + math.ceil(iW * 0.19) + 1 and y >= math.floor(iH * 0.61) - 1 then
			-- Plot is in Britain, reject it.
			-- print("Rejected British plot: ", x, y);
			return false;
		end
	end
	return true;
end
------------------------------------------------------------------------------
function StartPlotSystem()
	print("Creating start plot database.");
	local start_plot_database = AssignStartingPlots.Create();

	start_plot_database.ExtraEuropeCheck1 = ExtraEuropeCheck1;
	start_plot_database.ExtraEuropeCheck2 = ExtraEuropeCheck2;

	start_plot_database.oldAssignCityStatesToRegionsOrToUninhabited = start_plot_database.AssignCityStatesToRegionsOrToUninhabited;
	local newAssignCityStatesToRegionsOrToUninhabited = function (ASP)
		ASP:ExtraEuropeCheck1();
		ASP:oldAssignCityStatesToRegionsOrToUninhabited();
	end
	start_plot_database.AssignCityStatesToRegionsOrToUninhabited = newAssignCityStatesToRegionsOrToUninhabited;

	start_plot_database.oldCanPlaceCityStateAt = start_plot_database.CanPlaceCityStateAt;
	local newCanPlaceCityStateAt = function (ASP, x, y, area_ID, force_it, ignore_collisions)
		if not ASP:ExtraEuropeCheck2(x, y) then
			return false;
		end
		return ASP:oldCanPlaceCityStateAt(x, y, area_ID, force_it, ignore_collisions);
	end
	start_plot_database.CanPlaceCityStateAt = newCanPlaceCityStateAt;

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
