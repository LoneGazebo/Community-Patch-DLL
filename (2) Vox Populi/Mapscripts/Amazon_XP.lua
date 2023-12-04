-------------------------------------------------------------------------------
--	FILE:	 Amazon_XP.lua
--	AUTHOR:  Bob Thomas (Sirian)
--	PURPOSE: Regional map script - Amazon Rainforest, South America
-------------------------------------------------------------------------------
--	Copyright (c) 2012 Firaxis Games, Inc. All rights reserved.
-------------------------------------------------------------------------------

include("MapGenerator");
include("FractalWorld");
include("TerrainGenerator");
include("FeatureGenerator");

------------------------------------------------------------------------------
function GetMapScriptInfo()
	return {
		Name = "TXT_KEY_MAP_AMAZON_XP_VP",
		Description = "TXT_KEY_MAP_AMAZON_HELP",
		IconIndex = 7,
		IconAtlas = "WORLDTYPE_ATLAS_3",
	};
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	-- Amazon has fully custom grid sizes to match the slice of Earth being represented.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {22, 20},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {36, 32},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {44, 40},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {52, 46},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {60, 54},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {72, 64},
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
-- Amazon uses custom plot generation with regional specificity.
------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Setting Plot Types (Lua Amazon) ...");
	local iW, iH = Map.GetGridSize();
	-- Initiate plot table, fill all data slots with type PLOT_LAND
	local plotTypes = {};
	table.fill(plotTypes, PlotTypes.PLOT_LAND, iW * iH);

	-- Grains for reducing "clumping" of hills/peaks.
	local grainvalues = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 6,
	};
	local grain_amount = grainvalues[Map.GetWorldSize()];

	local hillsFrac = Fractal.Create(iW, iH, grain_amount, {}, 6, 6);
	local peaksFrac = Fractal.Create(iW, iH, grain_amount + 1, {}, 6, 6);

	local iHillsBottom1 = hillsFrac:GetHeight(20);
	local iHillsTop1 = hillsFrac:GetHeight(30);
	local iHillsBottom2 = hillsFrac:GetHeight(70);
	local iHillsTop2 = hillsFrac:GetHeight(80);
	local iForty = hillsFrac:GetHeight(40);
	local iFifty = hillsFrac:GetHeight(50);
	local iSixty = hillsFrac:GetHeight(60);
	local iPeakCol = peaksFrac:GetHeight(75);
	local iPeakAndes = peaksFrac:GetHeight(63);
	local iHillsAmazonBasin = hillsFrac:GetHeight(95);

	-- Define the Atlantic, which will be in the NE corner.
	print("Simulate the Atlantic Ocean, where the Amazon River terminates. (Lua Amazon) ...");
	local atlantic_coords = {
		{math.floor(iW * 0.54), iH - 1},
		{math.floor(iW * 0.6), math.floor(iH * 0.935)},
		{math.floor(iW * 0.7), math.floor(iH * 0.935)},
		{math.floor(iW * 0.8), math.floor(iH * 0.855)},
		{math.floor(iW * 0.86), math.floor(iH * 0.733)},
		{iW - 1, math.floor(iH * 0.665)},
	};
	-- Draw the Atlantic Line and fill in everything north of it with ocean.
	for loop = 1, 5 do
		local startX = atlantic_coords[loop][1];
		local startY = atlantic_coords[loop][2];
		local endX = atlantic_coords[loop + 1][1];
		local endY = atlantic_coords[loop + 1][2];
		local dx = endX - startX;
		local dy = endY - startY;
		local slope = 0;
		if dx ~= 0 then
			slope = dy / dx;
		end
		local y = startY;
		for x = startX + 1, endX do
			y = y + slope;
			local iY = math.floor(y);
			for loop_y = iY, iH - 1 do
				local i = loop_y * iW + x + 1;
				plotTypes[i] = PlotTypes.PLOT_OCEAN;
			end
		end
	end

	-- Define the Pacific, which will be in the SW corner.
	local pacific_coords = {
		{0, math.floor(iH * 0.23)},
		{math.floor(iW * 0.06), math.floor(iH * 0.13)},
		{math.floor(iW * 0.23), 0},
	};
	-- Draw the Pacific Line and fill in everything south of it with ocean.
	for loop = 1, 2 do
		local startX = pacific_coords[loop][1];
		local startY = pacific_coords[loop][2];
		local endX = pacific_coords[loop + 1][1];
		local endY = pacific_coords[loop + 1][2];
		local dx = endX - startX;
		local dy = endY - startY;
		local slope = 0;
		if dx ~= 0 then
			slope = dy / dx;
		end
		local y = startY;
		for x = startX, endX - 1 do
			y = y + slope;
			local iY = math.floor(y);
			for loop_y = iY, 0, -1 do
				local i = loop_y * iW + x + 1;
				plotTypes[i] = PlotTypes.PLOT_OCEAN;
			end
		end
	end

	-- Define the hilly regions and append their plots to their plot lists. GLOBAL variables used here.
	BrazilianHills = {};
	VenezuelanHills = {};
	ColumbianHills = {};
	Andes = {};
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local i = y * iW + x + 1;
			if x + y <= iH / 2 then
				if plotTypes[i] ~= PlotTypes.PLOT_OCEAN then
					table.insert(Andes, i);
				end
			elseif x <= iW * 0.15 and y >= iH * 0.73 then
				table.insert(ColumbianHills, i);
			elseif x >= iW * 0.275 and x <= iW * 0.525 and y >= iH * 0.79 then
				table.insert(VenezuelanHills, i);
			elseif x >= iW * 0.65 and y <= iH * 0.39 then
				table.insert(BrazilianHills, i);
			end
		end
	end

	-- Now assign plot types. Note, the plot table is already filled with flatlands.
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = y * iW + x + 1;
			-- Regional membership checked, effects chosen.
			-- Python had a simpler, less verbose method for checking table membership.
			local inAndes = false;
			local inBraz = false;
			local inCol = false;
			local inVen = false;
			for _, plotIndex in ipairs(Andes) do
				if i == plotIndex then
					inAndes = true;
					break;
				end
			end
			for _, plotIndex in ipairs(BrazilianHills) do
				if i == plotIndex then
					inBraz = true;
					break;
				end
			end
			for _, plotIndex in ipairs(ColumbianHills) do
				if i == plotIndex then
					inCol = true;
					break;
				end
			end
			for _, plotIndex in ipairs(VenezuelanHills) do
				if i == plotIndex then
					inVen = true;
					break;
				end
			end
			local hillVal = hillsFrac:GetHeight(x,y);
			if inAndes then
				if plotTypes[i] ~= PlotTypes.PLOT_OCEAN then
					if hillVal >= iHillsBottom1 then
						local peakVal = peaksFrac:GetHeight(x,y);
						if peakVal >= iPeakAndes then
							plotTypes[i] = PlotTypes.PLOT_PEAK;
						else
							plotTypes[i] = PlotTypes.PLOT_HILLS;
						end
					end
				end
			elseif inBraz then
				if (hillVal >= iHillsBottom1 and hillVal <= iHillsTop1) or (hillVal <= iHillsTop2 and hillVal >= iHillsBottom2) then
					plotTypes[i] = PlotTypes.PLOT_HILLS;
				end
			elseif inCol then
				if hillVal >= iFifty then
					local peakVal = peaksFrac:GetHeight(x,y);
					if peakVal >= iPeakCol then
						plotTypes[i] = PlotTypes.PLOT_PEAK;
					else
						plotTypes[i] = PlotTypes.PLOT_HILLS;
					end
				end
			elseif inVen then
				if (hillVal >= iHillsBottom1 and hillVal <= iForty) or (hillVal >= iSixty and hillVal <= iHillsTop2) then
					plotTypes[i] = PlotTypes.PLOT_HILLS;
				end
			else
				if plotTypes[i] ~= PlotTypes.PLOT_OCEAN then
					if hillVal >= iHillsAmazonBasin then
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
-- Amazon uses a custom terrain generation.
------------------------------------------------------------------------------
function GenerateTerrain()
	print("Generating Terrain (Lua Amazon) ...");
	local iW, iH = Map.GetGridSize();
	local terrainTypes = {};
	local terrainDesert = TerrainTypes.TERRAIN_DESERT;
	local terrainPlains = TerrainTypes.TERRAIN_PLAINS;
	local terrainGrass = TerrainTypes.TERRAIN_GRASS;

	-- Initiate terrain table, fill all land slots with type TERRAIN_PLAINS
	table.fill(terrainTypes, terrainPlains, iW * iH);
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local plot = Map.GetPlot(x, y)
			if plot:IsWater() then
				local i = y * iW + x; -- C++ Plot indices, starting at 0.
				terrainTypes[i] = plot:GetTerrainType();
			end
		end
	end

	-- Set up fractals and thresholds
	local grass_check = Fractal.Create(iW, iH, 5, {}, 6, 6);
	local desert_check = Fractal.Create(iW, iH, 4, {}, 6, 6);
	local iAndesGrass = grass_check:GetHeight(65);
	local iAndesDesert = desert_check:GetHeight(75);
	local iGrass = grass_check:GetHeight(60);

	-- Main loop
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = y * iW + x + 1;
			local plot = Map.GetPlot(x, y)
			if plot:IsWater() then
				terrainTypes[i - 1] = plot:GetTerrainType();
			else
				local inAndes = false;
				local inBraz = false;
				local inCol = false;
				local inVen = false;
				for _, plotIndex in ipairs(Andes) do
					if i == plotIndex then
						inAndes = true;
						break;
					end
				end
				for _, plotIndex in ipairs(BrazilianHills) do
					if i == plotIndex then
						inBraz = true;
						break;
					end
				end
				for _, plotIndex in ipairs(ColumbianHills) do
					if i == plotIndex then
						inCol = true;
						break;
					end
				end
				for _, plotIndex in ipairs(VenezuelanHills) do
					if i == plotIndex then
						inVen = true;
						break;
					end
				end
				if inAndes then
					local desertVal = desert_check:GetHeight(x, y);
					local grassVal = grass_check:GetHeight(x, y);
					if desertVal >= iAndesDesert then
						terrainTypes[i - 1] = terrainDesert;
					elseif grassVal >= iAndesGrass then
						terrainTypes[i - 1] = terrainGrass;
					end
				elseif inBraz or inCol or inVen then
					local grassVal = grass_check:GetHeight(x, y);
					if grassVal >= iGrass then
						terrainTypes[i - 1] = terrainGrass;
					end
				end
			end
		end
	end

	SetTerrainTypes(terrainTypes);
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- Amazon uses a custom feature generation.
------------------------------------------------------------------------------
function AddFeatures()
	print("Adding Features (Lua Amazon) ...");
	local iW, iH = Map.GetGridSize();
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local plot = Map.GetPlot(x, y);
			if plot:GetPlotType() == PlotTypes.PLOT_MOUNTAIN then
				if plot:IsCoastalLand() then
					plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true); -- These flags are for recalc of areas and rebuild of graphics. Instead of recalc over and over, do recalc at end of loop.
				end
			end
		end
	end

	local grainvalues = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 6,
	};
	local forest_grain = grainvalues[Map.GetWorldSize()];

	local iMarshPercent = 10;
	local iRiverMarshPercent = 30;
	local iForestPercent = 12;
	local iSEForestPercent = 30;
	local iOpenPercent = 10;

	local marshes = Fractal.Create(iW, iH, forest_grain, {}, 6, 6);
	local forests = Fractal.Create(iW, iH, forest_grain, {}, 6, 6);
	local open_lands = Fractal.Create(iW, iH, forest_grain, {}, 6, 6);

	local iMarshLevel = marshes:GetHeight(100 - iMarshPercent);
	local iRiverMarshLevel = marshes:GetHeight(100 - iRiverMarshPercent);
	local iForestLevel = forests:GetHeight(100 - iForestPercent);
	local iSEForestLevel = forests:GetHeight(100 - iSEForestPercent);
	local iOpenLevel = open_lands:GetHeight(100 - iOpenPercent);

	local featureForest = FeatureTypes.FEATURE_FOREST;
	local featureJungle = FeatureTypes.FEATURE_JUNGLE;
	local featureMarsh = FeatureTypes.FEATURE_MARSH;

	-- Now the main loop.
	local centerX = iW * 0.825;
	local centerY = iH * 0.69;
	local xAxis = iW * 0.09;
	local yAxis = iH * 0.1;
	local xAxisSquared = xAxis * xAxis;
	local yAxisSquared = yAxis * yAxis;
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local plot = Map.GetPlot(x, y);
			if not plot:IsWater() and plot:GetFeatureType() == FeatureTypes.NO_FEATURE then
				local deltaX = x - centerX;
				local deltaY = y - centerY;
				local deltaXSquared = deltaX * deltaX;
				local deltaYSquared = deltaY * deltaY;
				local d = deltaXSquared / xAxisSquared + deltaYSquared / yAxisSquared;
				if x + y <= iH / 2 then
					if plot:CanHaveFeature(featureForest) and forests:GetHeight(x, y) >= iForestLevel then
						plot:SetFeatureType(featureForest);
						plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, false);
					end
				elseif (iW - x) + y <= iH * 0.52 then
					if plot:CanHaveFeature(featureForest) and forests:GetHeight(x, y) >= iSEForestLevel then
						plot:SetFeatureType(featureForest);
						plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, false);
					elseif plot:IsFlatlands() then
						if marshes:GetHeight(x, y) >= iMarshLevel then
							plot:SetFeatureType(featureMarsh);
							plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, false);
						end
					end
				elseif d <= 1 then
					plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, false);
					if plot:IsFlatlands() then
						plot:SetFeatureType(featureMarsh);
					end
				else
					if plot:IsFlatlands() then
						if forests:GetHeight(x, y) >= iForestLevel then
							plot:SetFeatureType(featureForest);
							plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, false);
						else
							if plot:IsRiver() then
								if marshes:GetHeight(x, y) >= iRiverMarshLevel then
									plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, false);
									plot:SetFeatureType(featureMarsh);
								end
							else
								if marshes:GetHeight(x, y) >= iMarshLevel then
									plot:SetFeatureType(featureMarsh);
									plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, false);
								end
							end
						end
					end
					if plot:GetFeatureType() == FeatureTypes.NO_FEATURE and not plot:IsMountain() then
						if open_lands:GetHeight(x, y) < iOpenLevel then
							plot:SetFeatureType(featureJungle);
						else
							plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, false);
						end
					end
				end
			end
		end
	end
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function DoAmazonRiver(startPlot, thisFlowDirection, riverID)
	local iW, iH = Map.GetGridSize();
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
	else
		riverPlot = startPlot;
	end
	if not riverPlot or riverPlot:IsWater() then
		-- The river has flowed off the edge of the map or into the ocean. All is well.
		return;
	end
	-- Storing X,Y positions as locals to prevent redundant function calls.
	local riverPlotX = riverPlot:GetX();
	local riverPlotY = riverPlot:GetY();

	-- Determine flow direction.
	if thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHEAST then
		local dice = 1 + Map.Rand(100, "Amazon River Direction - Lua");
		if riverPlotX > iW * 0.765 and riverPlotY < iH * 0.685 then
			bestFlowDirection = FlowDirectionTypes.FLOWDIRECTION_NORTH;
		elseif riverPlotX > iW * 0.83 and riverPlotY <= iH * 0.73 then
			bestFlowDirection = FlowDirectionTypes.FLOWDIRECTION_NORTH;
		elseif riverPlotY <= iH * 0.73 and dice <= 28 then
			bestFlowDirection = FlowDirectionTypes.FLOWDIRECTION_NORTH;
		else
			bestFlowDirection = FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST;
		end
	elseif thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST then
		local dice = 1 + Map.Rand(100, "Amazon River Direction - Lua");
		if riverPlotX < iW * 0.73 and riverPlotY >= iH * 0.57 and dice <= 16 then
			bestFlowDirection = FlowDirectionTypes.FLOWDIRECTION_SOUTH;
		else
			bestFlowDirection = FlowDirectionTypes.FLOWDIRECTION_NORTHEAST;
		end
	elseif thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTH then
		bestFlowDirection = FlowDirectionTypes.FLOWDIRECTION_NORTHEAST;
	elseif thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTH then
		bestFlowDirection = FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST;
	end

	-- Recursively generate river.
	if bestFlowDirection ~= FlowDirectionTypes.NO_FLOWDIRECTION then
		DoAmazonRiver(riverPlot, bestFlowDirection, riverID);
	end
end
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
						-- Fix river problems in top row of the map.
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
						-- Fix river problems in left column of the map.
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
	local iW, iH = Map.GetGridSize();

	-- Place the Amazon!
	print("Charting the Amazon River (Lua Amazon) ...");
	local startX = math.floor((iW - 1) * 0.03);
	local startY = math.floor(iH / 2);
	local top = math.floor((iH - 1) * 0.8);
	local bottom = math.floor((iH - 1) * 0.43);
	local startPlot = Map.GetPlot(startX, startY);
	local startInlandCorner = startPlot:GetInlandCorner();
	DoAmazonRiver(startInlandCorner, FlowDirectionTypes.FLOWDIRECTION_NORTHEAST, 0);
	nextRiverID = 1;

	print("Amazon - Adding Remaining Rivers");
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
			elseif current_y >= bottom and current_y <= top then
				-- Plot is inside Amazon Corridor, ignore it.
			elseif not plot:IsWater() then
				if passCondition(plot) then
					if not Map.FindWater(plot, riverSourceRange, true) then
						if not Map.FindWater(plot, seaWaterRange, false) then
							local inlandCorner = plot:GetInlandCorner();
							if inlandCorner then
								local start_x = inlandCorner:GetX();
								local start_y = inlandCorner:GetY();
								if start_x + start_y <= iH * 0.36 then -- Ocean side of Andes
									DoRiver(inlandCorner, nil, FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST, nil);
								elseif start_y >= top and start_y < iH - 2 then -- Top of map
									if start_x <= iW * 0.23 and start_x > 1 then
										DoRiver(inlandCorner, nil, FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST, nil);
									elseif start_x > iW * 0.23 and start_x < iW - 2 then
										DoRiver(inlandCorner, nil, FlowDirectionTypes.FLOWDIRECTION_SOUTH, nil);
									end
								elseif start_y <= bottom and start_y > 1 then -- Bottom half of map, Amazon side of Andes
									if start_x <= iW * 0.14 and start_x > 1 then
										DoRiver(inlandCorner, nil, FlowDirectionTypes.FLOWDIRECTION_NORTHEAST, nil);
									elseif start_x > iW * 0.14 and start_x < iW - 2 then
										DoRiver(inlandCorner, nil, FlowDirectionTypes.FLOWDIRECTION_NORTH, nil);
									end
								elseif start_y >= 0.65 and start_y < iH - 2 then
									if start_x <= iW * 0.23 and start_x > 1 then
										DoRiver(inlandCorner, nil, FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST, nil);
									end
								elseif start_y >= 0.57 and start_y < iH - 2 then
									if start_x <= iW * 0.15 and start_x > 1 then
										DoRiver(inlandCorner, nil, FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST, nil);
									end
								end
							end
						end
					end
				end
			end
		end
	end
end
------------------------------------------------------------------------------
function StartPlotSystem()
	print("Creating start plot database.");
	local start_plot_database = AssignStartingPlots.Create();

	print("Dividing the map in to Regions.");
	-- Regional Division Method 1: Biggest Landmass
	local args = {
		method = 1,
	};
	start_plot_database:GenerateRegions(args);

	print("Choosing start locations for civilizations.");
	start_plot_database:ChooseLocations();

	print("Normalizing start locations and assigning them to Players.");
	start_plot_database:BalanceAndAssign();

	print("No Natural Wonders available on this script.");
	-- start_plot_database:PlaceNaturalWonders();

	print("Placing Resources and City States.");
	start_plot_database:PlaceResourcesAndCityStates();
end
------------------------------------------------------------------------------

-------------------------------------------------------------------------------
function DetermineContinents()
	-- Setting all continental art to Africa style.
	for _, plot in Plots() do
		if plot:IsWater() then
			plot:SetContinentArtType(0);
		else
			plot:SetContinentArtType(3);
		end
	end
end
-------------------------------------------------------------------------------
