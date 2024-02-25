-------------------------------------------------------------------------------------------------------------
--	FILE:	 Europe.lua
--	AUTHOR:  Bob Thomas (Sirian)
--	PURPOSE: Regional map script - Generates a random map similar to Europe and the Mediterranean basin.
-------------------------------------------------------------------------------------------------------------
--	Copyright (c) 2012 Firaxis Games, Inc. All rights reserved.
-------------------------------------------------------------------------------------------------------------

include("MapGenerator");
include("MultilayeredFractal");
include("TerrainGenerator");
include("FeatureGenerator");
include("MapmakerUtilities");

-------------------------------------------------------------------------------------------------------------
function GetMapScriptInfo()
	return {
		Name = "TXT_KEY_MAP_EUROPE_VP",
		Description = "TXT_KEY_MAP_EUROPE_HELP",
		IsAdvancedMap = false,
		IconIndex = 6,
		IconAtlas = "WORLDTYPE_ATLAS_3",
	};
end
-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	-- Europe has fully custom grid sizes to match the slice of Earth being represented.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {20, 16},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {35, 28},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {45, 36},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {55, 44},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {70, 56},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {85, 68},
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
-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------
function MultilayeredFractal:GeneratePlotsByRegion()
	-- Sirian's MultilayeredFractal controlling function.
	-- You -MUST- customize this function for each script using MultilayeredFractal.

	-- This implementation is specific to Mediterranean.
	local iW, iH = Map.GetGridSize();
	-- Initiate plot table, fill all data slots with type PLOT_LAND
	table.fill(self.wholeworldPlotTypes, PlotTypes.PLOT_LAND, iW * iH);
	-- Change western edge and northwest corner to water.
	local west_edge = {};
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
	local dy = endY - startY;
	local slope = 0;
	if dy ~= 0 then
		slope = dx / dy;
	end
	local tempX = startX;
	for y = startY, endY do
		tempX = tempX + slope;
		west_edge[y] = math.floor(tempX) + Map.Rand(3, "Roughen coastline - Mediterranean LUA");
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
	startX = math.floor(iW * 0.205);
	startY = math.floor(iH * 0.225);
	endX = math.ceil(iW * 0.31);
	endY = math.floor(iH * 0.295);
	dx = endX - startX;
	dy = endY - startY;
	slope = 0;
	if dx ~= 0 then
		slope = dy / dx;
	end
	local tempY = startY;
	for x = startX, endX do
		tempY = tempY + slope;
		local i = math.floor(tempY) * iW + x + 1;
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

	self:GenerateWaterLayer(args);

	args = {};
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

	self:GenerateWaterLayer(args);

	args = {};
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

	self:GenerateWaterLayer(args);

	args = {};
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

	self:GenerateWaterLayer(args);

	args = {};
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

	self:GenerateWaterLayer(args);

	args = {};
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

	self:GenerateWaterLayer(args);

	args = {};
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

	self:GenerateWaterLayer(args);

	args = {};
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

	self:GenerateWaterLayer(args);

	-- Simulate the Black Sea
	args = {};
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

	self:GenerateWaterLayer(args);

	args = {};
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

	self:GenerateWaterLayer(args);

	-- Generate British Isles
	args = {};
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

	self:GenerateFractalLayerWithoutHills(args);

	-- Generate Scandinavia
	args = {};
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

	self:GenerateFractalLayerWithoutHills(args);

	args = {};
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

	self:GenerateFractalLayerWithoutHills(args);

	args = {};
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

	self:GenerateFractalLayerWithoutHills(args);

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
	args = {};
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

	self:GenerateWaterLayer(args);

	-- Land and water are set. Apply hills and mountains.
	self:ApplyTectonics();

	-- Plot Type generation completed. Return global plot array.
	return self.wholeworldPlotTypes;
end
-------------------------------------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Setting Plot Types (Lua Europe) ...");

	local layered_world = MultilayeredFractal.Create();
	local plotsEur = layered_world:GeneratePlotsByRegion();

	SetPlotTypes(plotsEur);

	GenerateCoasts();
end
-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------
-- Europe uses a custom terrain generation.
-------------------------------------------------------------------------------------------------------------
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
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 4,
	};
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

	local terrainDesert = TerrainTypes.TERRAIN_DESERT;
	local terrainPlains = TerrainTypes.TERRAIN_PLAINS;
	local terrainGrass = TerrainTypes.TERRAIN_GRASS;
	local terrainTundra = TerrainTypes.TERRAIN_TUNDRA;
	local terrainSnow = TerrainTypes.TERRAIN_SNOW;

	-- Main loop, generate the terrain plot by plot.
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local i = y * iW + x; -- C++ Plot indices, starting at 0.
			local plot = Map.GetPlot(x, y);
			local terrainVal;

			if plot:IsWater() then
				terrainVal = plot:GetTerrainType();
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
				local directions = {
					DirectionTypes.DIRECTION_NORTHEAST,
					DirectionTypes.DIRECTION_EAST,
					DirectionTypes.DIRECTION_SOUTHEAST,
					DirectionTypes.DIRECTION_SOUTHWEST,
					DirectionTypes.DIRECTION_WEST,
					DirectionTypes.DIRECTION_NORTHWEST,
				};

				for _, current_direction in ipairs(directions) do
					local testPlot = Map.PlotDirection(x, y, current_direction);
					if testPlot then
						local type = testPlot:GetPlotType()
						if type == PlotTypes.PLOT_OCEAN then -- Adjacent plot is water! Check if ocean or lake.
							-- Have to do a manual check of landmass size, because lakes have not yet been defined as such.
							local testLandmassID = testPlot:GetLandmass();
							local testLandmassSize = Map.GetNumTilesOfLandmass(testLandmassID);
							if testLandmassSize >= 10 then
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
-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------
function GetRiverValueAtPlot(plot)
	local numPlots = PlotTypes.NUM_PLOT_TYPES;
	local sum = (numPlots - plot:GetPlotType()) * 20;
	local numDirections = DirectionTypes.NUM_DIRECTION_TYPES;
	for direction = 0, numDirections - 1, 1 do
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
-------------------------------------------------------------------------------------------------------------
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
-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------
function FeatureGenerator:AddIceAtPlot()
	-- Do nothing. No ice to be placed.
end
-------------------------------------------------------------------------------------------------------------
function FeatureGenerator:AddJunglesAtPlot()
	-- Do nothing. No jungle to be placed.
end
-------------------------------------------------------------------------------------------------------------
function FeatureGenerator:AddAtolls()
	-- Do nothing. No atolls to be placed.
end
-------------------------------------------------------------------------------------------------------------
function AddFeatures()
	print("Adding Features (Lua Europe) ...");
	local featuregen = FeatureGenerator.Create();
	featuregen:AddFeatures(false);
end
-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------
function AssignStartingPlots:__CustomInit()
	-- This function included to provide a quick and easy override for changing any initial settings.
	-- Add your customized version to the map script.
	self.CanBeGibraltar = AssignStartingPlots.CanBeGibraltar;
	self.CanBeMtSinai = AssignStartingPlots.CanBeMtSinai;
	self.gibraltar_list, self.sinai_list = {}, {};
end
-------------------------------------------------------------------------------------------------------------
function AssignStartingPlots:GenerateRegions()
	-- Custom method for Europe. Will start a civ in Britain if eight or more civs in the game.
	print("Map Generation - Dividing the map in to Regions");
	local iW, iH = Map.GetGridSize();
	self.method = RegionDivision.BIGGEST_LANDMASS;

	-- Determine number of civilizations and city states present in this game.
	self.iNumCivs, self.iNumCityStates, self.player_ID_list, self.bTeamGame, self.teams_with_major_civs, self.number_civs_per_team = GetPlayerAndTeamInfo();
	self.iNumCityStatesUnassigned = self.iNumCityStates;

	local iNumCivsRemaining = self.iNumCivs;
	if self.iNumCivs >= 8 then -- Place one civ in the British Isles.
		self.inhabited_WestX = math.floor(iW * 0.055);
		self.inhabited_SouthY = math.floor(iH * 0.61);
		self.inhabited_Width = math.ceil(iW * 0.19);
		self.inhabited_Height = math.ceil(iH * 0.27);
		local fert_table, fertCount, plotCount =
			self:MeasureStartPlacementFertilityInRectangle(self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height);
		-- AreaID -1 means ignore landmass/area IDs.
		local rect_table = {self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height, -1, fertCount, plotCount};
		self:DivideIntoRegions(1, fert_table, rect_table);
		iNumCivsRemaining = iNumCivsRemaining - 1;
	end

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

	-- Obtain "Start Placement Fertility" of the landmass.
	local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityOfLandmass(iLandmassID, iWestX, iEastX, iSouthY, iNorthY, wrapsX, wrapsY);

	-- Assemble the Rectangle data table:
	local rect_table = {iWestX, iSouthY, iWidth, iHeight, iLandmassID, fertCount, plotCount};

	-- The data from this call is processed in to self.regionData during the process.
	self:DivideIntoRegions(iNumCivsRemaining, fert_table, rect_table);

	---[[ Printout is for debugging only. Deactivate otherwise.
	local tempRegionData = self.regionData;
	for i, data in ipairs(tempRegionData) do
		print("-");
		print("Data for Start Region #", i);
		print("WestX:", data[1]);
		print("SouthY:", data[2]);
		print("Width:", data[3]);
		print("Height:", data[4]);
		print("LandmassID:", data[5]);
		print("Fertility:", data[6]);
		print("Plots:", data[7]);
		print("Fert/Plot:", data[8]);
		print("-");
	end
	--]]
end
-------------------------------------------------------------------------------------------------------------
function AssignStartingPlots:GenerateCandidatePlotListsForSpecificNW()
	-- Only check Gibraltar and Sinai.
	local iW, iH = Map.GetGridSize();
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local landEligibility, seaEligibility = self:ExaminePlotForNaturalWondersEligibility(x, y);
			-- Plot has passed checks applicable to all NW types. Move on to specific checks.
			for nw_number, _ in ipairs(self.xml_row_numbers) do
				if seaEligibility and self.wonder_list[nw_number] == "FEATURE_GIBRALTAR" then
					self:CanBeThisNaturalWonderType(x, y, nw_number);
				elseif landEligibility and self.wonder_list[nw_number] == "FEATURE_MT_SINAI" then
					self:CanBeThisNaturalWonderType(x, y, nw_number);
				end
			end
		end
	end
end
-------------------------------------------------------------------------------------------------------------
function AssignStartingPlots:GetNumNaturalWondersToPlace()
	return 2;
end
-------------------------------------------------------------------------------------------------------------
function AssignStartingPlots:AssignCityStatesToRegions(current_cs_index)
	local starting_region_number = 1;
	if self.iNumCivs >= 8 then -- Civ in Britain
		starting_region_number = 2;
	end

	local iNumCityStatesPerRegion = self:GetNumCityStatesPerRegion();
	if iNumCityStatesPerRegion > 0 then
		for current_region = starting_region_number, self.iNumCivs do
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
-------------------------------------------------------------------------------------------------------------
function ExtraNWEligibilityCheck(x, y)
	-- Adding this check for Europe in AssignStartingPlots:ExaminePlotForNaturalWondersEligibility, forcing NWs to be in specific areas
	local iW, iH = Map.GetGridSize();
	return ((x >= iW * 0.1 and x <= iW * 0.3) and (y >= iH * 0.15 and y <= iH * 0.35)) or (x >= iW * 0.8 and y <= iH * 0.2);
end
-------------------------------------------------------------------------------------------------------------
function ExtraCityStateCheck(ASP, x, y)
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
-------------------------------------------------------------------------------------------------------------
function StartPlotSystem()
	print("Creating start plot database.");
	local start_plot_database = AssignStartingPlots.Create();

	start_plot_database.ExtraCityStateCheck = ExtraCityStateCheck;

	start_plot_database.oldExaminePlotForNaturalWondersEligibility = start_plot_database.ExaminePlotForNaturalWondersEligibility;
	local newExaminePlotForNaturalWondersEligibility = function (ASP, x, y)
		if not ExtraNWEligibilityCheck(x, y) then
			return false, false;
		end
		return ASP:oldExaminePlotForNaturalWondersEligibility(x, y);
	end
	start_plot_database.ExaminePlotForNaturalWondersEligibility = newExaminePlotForNaturalWondersEligibility;

	start_plot_database.oldCanPlaceCityStateAt = start_plot_database.CanPlaceCityStateAt;
	local newCanPlaceCityStateAt = function (ASP, x, y, area_ID, force_it, ignore_collisions)
		if not ASP:ExtraCityStateCheck(x, y) then
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
-------------------------------------------------------------------------------------------------------------
