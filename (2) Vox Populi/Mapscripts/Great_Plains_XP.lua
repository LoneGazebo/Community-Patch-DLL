-------------------------------------------------------------------------------
--	FILE:	 Great_Plains_GK.lua
--	AUTHOR:  Bob Thomas (Sirian)
--	PURPOSE: Regional map script - Great Plains, North America
-------------------------------------------------------------------------------
--	Copyright (c) 2010 Firaxis Games, Inc. All rights reserved.
-------------------------------------------------------------------------------

include("MapGenerator");
include("FractalWorld");
include("TerrainGenerator");
include("FeatureGenerator");

------------------------------------------------------------------------------
function GetMapScriptInfo()
	return {
		Name = "TXT_KEY_MAP_GREAT_PLAINS_XP_VP",
		Description = "TXT_KEY_MAP_GREAT_PLAINS_HELP",
		IconIndex = 21,
		IconAtlas = "WORLDTYPE_ATLAS_3",
	};
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	-- Great Plains has fully custom grid sizes to match the slice of Earth being represented.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {18, 14},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {28, 22},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {36, 26},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {44, 32},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {56, 44},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {72, 56},
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
-- Great Plains uses custom plot generation with regional specificity.
------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Setting Plot Types (Lua Great Plains) ...");
	local iW, iH = Map.GetGridSize();

	-- Initiate plot table, fill all data slots with type PLOT_LAND
	local plotTypes = {};
	table.fill(plotTypes, PlotTypes.PLOT_LAND, iW * iH);

	-- Set fractal flags, no wrap, no zero row anomalies.
	local iFlags = {};

	-- Grains for reducing "clumping" of hills/peaks on larger maps.
	local grainvalues = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 6,
	};
	local grain_amount = grainvalues[Map.GetWorldSize()];

	local hillsFrac = Fractal.Create(iW, iH, grain_amount, iFlags, 6, 6);
	local peaksFrac = Fractal.Create(iW, iH, grain_amount + 1, iFlags, 6, 6);
	local regionsFrac = Fractal.Create(iW, iH, grain_amount, iFlags, 6, 6);

	local iPlainsThreshold = hillsFrac:GetHeight(8);
	local iHillsBottom1 = hillsFrac:GetHeight(20);
	local iHillsTop1 = hillsFrac:GetHeight(30);
	local iHillsBottom2 = hillsFrac:GetHeight(70);
	local iHillsTop2 = hillsFrac:GetHeight(80);
	local iForty = hillsFrac:GetHeight(40);
	local iFifty = hillsFrac:GetHeight(50);
	local iSixty = hillsFrac:GetHeight(60);
	local iPeakThreshold = peaksFrac:GetHeight(25);
	local iPeakRockies = peaksFrac:GetHeight(37);

	-- Define six regions:
	-- 1. Rockies
	-- 2. Plains
	-- 3. Eastern Grasslands
	-- 4. The Gulf
	-- 5. Ozarks
	-- 6. SW Desert
	local rockies = {};
	local plains = {};
	local grass = {};
	local gulf = {};
	local ozarks = {};
	local swDesert = {};
	local plainsWest = 0.2; -- regional divide between Rockies and Plains
	local plainsEast = 0.67; -- divide between Plains and East
	local south = math.ceil(iH / 4); -- divide between Rockies and SW
	local middle = iW / 2; -- western edge of the Gulf

	-- first define the Gulf, which will always be in the SE corner.
	print("Simulate the Gulf of Mexico (Lua Great Plains) ...");
	local coast = middle;
	for y = 0, south do
		coast = coast + Map.Rand(4, "Gulf of Mexico - Great Plains Lua");
		if coast > iW - 1 then
			break;
		end
		for x = coast, iW - 1 do
			local i = y * iW + x + 1;
			table.insert(gulf, i);
		end
	end

	-- now define the Ozark Mountains, a randomly placed region of hilly terrain.
	-- First roll the location in which the Ozarks will be placed.
	print("Simulate the Ozarks (Lua Great Plains) ...");
	local leftX = math.floor(iW * 0.38);
	local rightX = math.floor(iW * 0.71);
	local rangeX = rightX - leftX;
	local bottomY = math.floor(iH * 0.35);
	local topY = math.floor(iH * 0.67);
	local rangeY = topY - bottomY;
	local slideX = Map.Rand(rangeX, "Ozarks Placement - Great Plains Lua");
	local slideY = Map.Rand(rangeY, "Ozarks Placement - Great Plains Lua");
	-- Now set the boundaries and scope of the Ozarks.
	local leftOzark = leftX + slideX;
	local botOzark = bottomY + slideY;
	local widthOzark = iW / 6;
	local heightOzark = iH / 6;
	local rightOzark = leftOzark + widthOzark + 1;
	local topOzark = botOzark + heightOzark + 1;
	local midOzarkY = botOzark + heightOzark / 2;
	-- Now loop the plots and append their index numbers to the Ozark list.
	-- Run two loops for Y, both starting in middle, one to north, one to south.
	local varLeft = leftOzark;
	local varRight = rightOzark;
	for y = midOzarkY + 1, topOzark - 1 do
		if varLeft > varRight then
			break
		end
		for x = varLeft, varRight do
			local i = y * iW + x + 1;
			table.insert(ozarks, i);
		end
		local leftSeed = Map.Rand(5, "Ozarks Shape - Great Plains Lua");
		if leftSeed == 4 then
			leftSeed = 0;
		end
		if leftSeed == 3 then
			leftSeed = 1;
		end
		varLeft = varLeft + leftSeed;
		local rightSeed = Map.Rand(5, "Ozarks Shape - Great Plains Lua");
		if rightSeed == 4 then
			rightSeed = 0;
		end
		if rightSeed == 3 then
			rightSeed = 1;
		end
		varRight = varRight - rightSeed;
	end
	-- Second Loop
	varLeft = leftOzark;
	varRight = rightOzark;
	for y = midOzarkY, botOzark + 1, -1 do
		if varLeft > varRight then
			break;
		end
		for x = varLeft, varRight do
			local i = y * iW + x + 1;
			table.insert(ozarks, i);
		end
		local leftSeed = Map.Rand(5, "Ozarks Shape - Great Plains Lua");
		if leftSeed == 4 then
			leftSeed = 0;
		end
		if leftSeed == 3 then
			leftSeed = 1;
		end
		varLeft = varLeft + leftSeed;
		local rightSeed = Map.Rand(5, "Ozarks Shape - Great Plains Lua");
		if rightSeed == 4 then
			rightSeed = 0;
		end
		if rightSeed == 3 then
			rightSeed = 1;
		end
		varRight = varRight - rightSeed;
	end
	-- now define the four easiest regions and append their plots to their plot lists
	print("Simulate the Rockies (Lua Great Plains) ...");
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local i = y * iW + x + 1;
			local lat = x / iW;
			lat = lat + (128 - regionsFrac:GetHeight(x, y)) / (255.0 * 5.0);
			if lat < 0 then
				lat = 0;
			end
			if lat > 1 then
				lat = 1;
			end
			if y >= south and lat <= plainsWest then
				table.insert(rockies, i);
			elseif y < south and lat <= plainsWest then
				table.insert(swDesert, i);
			else
				local inGulf = false;
				local inOzarks = false;
				for _, plotIndex in ipairs(gulf) do
					if i == plotIndex then
						inGulf = true;
					end
				end
				for _, plotIndex in ipairs(ozarks) do
					if i == plotIndex then
						inOzarks = true;
					end
				end
				if lat >= plainsEast then
					if not inGulf and not inOzarks then
						table.insert(grass, i);
					end
				else
					if not inGulf and not inOzarks then
						table.insert(plains, i);
					end
				end
			end
		end
	end

	-- Now assign plot types. Note, the plot table is already filled with flatlands.
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = y * iW + x + 1;
			-- Regional membership checked, effects chosen.
			-- Python had a simpler, less verbose method for checking table membership.
			local inGulf = false;
			local inOzarks = false;
			local inSWDesert = false;
			local inRockies = false;
			local inGrass = false;
			for _, plotIndex in ipairs(gulf) do
				if i == plotIndex then
					inGulf = true;
				end
			end
			for _, plotIndex in ipairs(ozarks) do
				if i == plotIndex then
					inOzarks = true;
				end
			end
			for _, plotIndex in ipairs(swDesert) do
				if i == plotIndex then
					inSWDesert = true;
				end
			end
			for _, plotIndex in ipairs(rockies) do
				if i == plotIndex then
					inRockies = true;
				end
			end
			for _, plotIndex in ipairs(grass) do
				if i == plotIndex then
					inGrass = true;
				end
			end
			if inGulf then
				plotTypes[i] = PlotTypes.PLOT_OCEAN;
			elseif inSWDesert then
				local hillVal = hillsFrac:GetHeight(x, y);
				if (hillVal >= iHillsBottom1 and hillVal <= iForty) or (hillVal >= iSixty and hillVal <= iHillsTop2) then
					local peakVal = peaksFrac:GetHeight(x, y);
					if peakVal <= iPeakThreshold then
						plotTypes[i] = PlotTypes.PLOT_PEAK;
					else
						plotTypes[i] = PlotTypes.PLOT_HILLS;
					end
				end
			elseif inOzarks then
				local hillVal = hillsFrac:GetHeight(x, y);
				if hillVal <= iHillsTop1 or (hillVal >= iForty and hillVal <= iFifty) or (hillVal >= iSixty and hillVal <= iHillsBottom2) or hillVal >= iHillsTop2 then
					plotTypes[i] = PlotTypes.PLOT_HILLS;
				end
			elseif inRockies then
				local hillVal = hillsFrac:GetHeight(x, y);
				if hillVal >= iHillsTop1 then
					local peakVal = peaksFrac:GetHeight(x, y);
					if peakVal <= iPeakRockies then
						plotTypes[i] = PlotTypes.PLOT_PEAK;
					else
						plotTypes[i] = PlotTypes.PLOT_HILLS;
					end
				end
			elseif inGrass then
				local hillVal = hillsFrac:GetHeight(x, y);
				if (hillVal >= iHillsBottom1 and hillVal <= iHillsTop1) or (hillVal >= iHillsBottom2 and hillVal <= iHillsTop2) then
					plotTypes[i] = PlotTypes.PLOT_HILLS;
				end
			else -- Plot is in the plains.
				local hillVal = hillsFrac:GetHeight(x, y);
				if hillVal < iPlainsThreshold then
					plotTypes[i] = PlotTypes.PLOT_HILLS;
				end
			end
		end
	end

	SetPlotTypes(plotTypes);
	GenerateCoasts();
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- Great Plains uses a custom terrain generation.
------------------------------------------------------------------------------
function GenerateTerrain()
	print("Generating Terrain (Lua Great Plains) ...");
	local iW, iH = Map.GetGridSize();
	local terrainTypes = {};
	local iFlags = {};
	local grainvalues = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 5,
	};
	local grain_amount = grainvalues[Map.GetWorldSize()];

	local iRockyDTopPercent = 100;
	local iRockyPTopPercent = 50;
	local iRockyPBottomPercent = 20;
	local iDesertTopPercent = 100;
	local iDesertBottomPercent = 92;
	local iTexDesertTopPercent = 70;
	local iTexDesertBottomPercent = 50;
	local iEastDTopPercent = 100;
	local iEastDBottomPercent = 98;
	local iEastPTopPercent = 98;
	local iEastPBottomPercent = 75;

	local fWestLongitude = 0.15;
	local fEastLongitude = 0.65;
	local fTexLat = 0.37;
	local fTexEast = 0.55;

	local rocky = Fractal.Create(iW, iH, grain_amount, iFlags, -1, -1);
	local plains = Fractal.Create(iW, iH, grain_amount + 1, iFlags, -1, -1);
	local east = Fractal.Create(iW, iH, grain_amount, iFlags, -1, -1);
	local variation = Fractal.Create(iW, iH, grain_amount, iFlags, -1, -1);

	local iRockyDTop = rocky:GetHeight(iRockyDTopPercent);
	local iRockyDBottom = rocky:GetHeight(iRockyPTopPercent);
	local iRockyPTop = rocky:GetHeight(iRockyPTopPercent);
	local iRockyPBottom = rocky:GetHeight(iRockyPBottomPercent);

	local iDesertTop = plains:GetHeight(iDesertTopPercent);
	local iDesertBottom = plains:GetHeight(iDesertBottomPercent);
	local iTexDesertTop = plains:GetHeight(iTexDesertTopPercent);
	local iTexDesertBottom = plains:GetHeight(iTexDesertBottomPercent);

	local iEastDTop = east:GetHeight(iEastDTopPercent);
	local iEastDBottom = east:GetHeight(iEastDBottomPercent);
	local iEastPTop = east:GetHeight(iEastPTopPercent);
	local iEastPBottom = east:GetHeight(iEastPBottomPercent);

	local terrainDesert = TerrainTypes.TERRAIN_DESERT;
	local terrainPlains = TerrainTypes.TERRAIN_PLAINS;
	local terrainGrass = TerrainTypes.TERRAIN_GRASS;

	-- Main loop, generate the terrain plot by plot.
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local i = y * iW + x; -- C++ Plot indices, starting at 0.
			local plot = Map.GetPlot(x, y);
			local terrainVal;

			-- Handle water plots
			if plot:IsWater() then
				terrainVal = plot:GetTerrainType();

			-- Handle land plots
			else
				-- Set latitude at plot
				local lat = x / iW; -- 0.0 = west
				lat = lat + (128 - variation:GetHeight(x, y)) / (255.0 * 5.0);
				if lat < 0 then
					lat = 0;
				elseif lat > 1 then
					lat = 1;
				end

				if lat <= fWestLongitude then
					local val = rocky:GetHeight(x, y);
					if val >= iRockyDBottom and val <= iRockyDTop then
						terrainVal = terrainDesert;
					elseif val >= iRockyPBottom and val <= iRockyPTop then
						terrainVal = terrainPlains;
					else
						local long = y / iH;
						if long > 0.23 then
							terrainVal = terrainGrass;
						else
							terrainVal = terrainDesert;
						end
					end
				elseif lat > fEastLongitude then
					local val = east:GetHeight(x, y);
					if val >= iEastDBottom and val <= iEastDTop then
						terrainVal = terrainDesert;
					elseif val >= iEastPBottom and val <= iEastPTop then
						terrainVal = terrainPlains;
					else
						terrainVal = terrainGrass;
					end
				elseif lat > fWestLongitude and lat <= fTexEast and y / iH <= fTexLat then
					-- More desert in Texas region.
					local val = east:GetHeight(x, y);
					if val >= iDesertBottom and val <= iDesertTop then
						terrainVal = terrainDesert;
					elseif val >= iTexDesertBottom and val <= iTexDesertTop then
						terrainVal = terrainDesert;
					else
						terrainVal = terrainPlains;
					end
				else
					local val = plains:GetHeight(x, y);
					if val >= iDesertBottom and val <= iDesertTop then
						terrainVal = terrainDesert;
					else
						terrainVal = terrainPlains;
					end
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
-- Great Plains uses a custom feature generation.
------------------------------------------------------------------------------
function AddFeatures()
	print("Adding Features (Lua Great Plains) ...");
	local iW, iH = Map.GetGridSize();
	-- set fractal flags, no wrap, no zero row anomalies.
	local iFlags = {};
	local grainvalues = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 6,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 6,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 7,
	};
	local forest_grain = grainvalues[Map.GetWorldSize()];

	local iMarshPercent = 50;
	local iForestPercent = 8;
	local iEastForestPercent = 70;
	local iRockyForestPercent = 55;

	local fracXExp = -1;
	local fracYExp = -1;

	local forests = Fractal.Create(iW, iH, forest_grain, iFlags, fracXExp, fracYExp);

	local iMarshLevel = forests:GetHeight(100 - iMarshPercent);
	local iForestLevel = forests:GetHeight(iForestPercent);
	local iEastForestLevel = forests:GetHeight(iEastForestPercent);
	local iRockyForestLevel = forests:GetHeight(iRockyForestPercent);

	local featureFloodPlains = FeatureTypes.FEATURE_FLOOD_PLAINS;
	local featureForest = FeatureTypes.FEATURE_FOREST;
	local featureOasis = FeatureTypes.FEATURE_OASIS;
	local featureMarsh = FeatureTypes.FEATURE_MARSH;

	-- Now the main loop.
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local long = x / iW;
			local lat = y / iH;
			local plot = Map.GetPlot(x, y);

			if plot:CanHaveFeature(featureFloodPlains) then
				plot:SetFeatureType(featureFloodPlains);
			end

			if plot:GetFeatureType() == FeatureTypes.NO_FEATURE then
				if plot:CanHaveFeature(featureOasis) then
					if Map.Rand(100, "Add Oasis Lua") <= 5 then
						plot:SetFeatureType(featureOasis);
					end
				end
			end

			if plot:GetFeatureType() == FeatureTypes.NO_FEATURE then
				-- Marsh only in Louisiana Wetlands!
				if long > 0.65 and lat < 0.45 then
					if plot:CanHaveFeature(featureMarsh) then
						if forests:GetHeight(x, y) >= iMarshLevel then
							plot:SetFeatureType(featureMarsh);
						end
					end
				end
			end

			if plot:GetFeatureType() == FeatureTypes.NO_FEATURE then
				-- No Evergreens in Civ5, so... Deciduous trees everywhere.
				if long < 0.16 and lat > 0.23 and (plot:IsFlatlands() or plot:IsHills()) then
					if forests:GetHeight(x, y) <= iRockyForestLevel then
						plot:SetFeatureType(featureForest);
					end
				elseif long > 0.72 and plot:CanHaveFeature(featureForest) then
					if forests:GetHeight(x, y) <= iEastForestLevel then
						plot:SetFeatureType(featureForest);
					end
				else
					if plot:CanHaveFeature(featureForest) then
						if forests:GetHeight(x, y) <= iForestLevel then
							plot:SetFeatureType(featureForest);
						end
					end
				end
			end
		end
	end
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function GetMississippiRiverValueAtPlot(plot)
	local iW, iH = Map.GetGridSize();
	local x = plot:GetX();
	local y = plot:GetY();
	local center = math.floor((iW - 1) * (0.84 + ((iH - y) / iH) * 0.08));
	local left = math.floor((iW - 1) * (0.74 + ((iH - y) / iH) * 0.08));
	local right = math.floor((iW - 1) * (0.9 + ((iH - y) / iH) * 0.08));
	local direction_influence_value;
	local random_factor_x = Map.Rand(6, "River direction random x factor - Great Plains LUA") - 1;
	local random_factor_y = Map.Rand(3, "River direction random y factor - Great Plains LUA");
	-- print("Checking Miss River Value for plot", x, y, "- - - L/C/R ", left, center, right);

	if x < left then
		direction_influence_value = 9999 * (iW - x) + y * 25;
	elseif x > right then
		direction_influence_value = 9999 * x + y * 25;
	else
		direction_influence_value = (math.abs(x - center) * 0.1 * random_factor_x + y) * random_factor_y;
	end
	-- print("-- Direction Influence Value = ", direction_influence_value);

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
	-- print("Plot Value Sum = ", sum);

	return sum;
end
------------------------------------------------------------------------------
function GetRiverValueAtPlot(plot)
	-- Custom method to force other rivers to flow toward the Mississippi River.
	local iW, iH = Map.GetGridSize();
	local x = plot:GetX();
	local y = plot:GetY();
	local random_factor = Map.Rand(3, "River direction random factor - Great Plains LUA");
	local direction_influence_value = (math.abs(x - iW * 0.85) + math.abs(y - iH / 2)) * random_factor;
	if y > iH * 0.87 then
		direction_influence_value = direction_influence_value + (400 / (iH - y));
	end

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

	return sum;
end
------------------------------------------------------------------------------
function DoMississippiRiver(startPlot, thisFlowDirection, originalFlowDirection, riverID)
	thisFlowDirection = thisFlowDirection or FlowDirectionTypes.NO_FLOWDIRECTION;
	originalFlowDirection = originalFlowDirection or FlowDirectionTypes.NO_FLOWDIRECTION;
	-- pStartPlot = the plot at whose SE corner the river is starting
	if not riverID then
		riverID = nextRiverID;
		nextRiverID = nextRiverID + 1;
	end
	local otherRiverID = _rivers[startPlot]
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
		end
	}
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
						-- print("Non-nil River Plot!");
						local value = GetMississippiRiverValueAtPlot(adjacentPlot); -- CUSTOM
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
		-- Try a second pass allowing the river to "flow backwards".
		if bestFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION then
			bestValue = math.huge;
			for flowDirection, getAdjacentPlot in pairs(adjacentPlotFunctions) do
				if thisFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION or
					flowDirection == TurnRightFlowDirections[thisFlowDirection] or
					flowDirection == TurnLeftFlowDirections[thisFlowDirection] then
					local adjacentPlot = getAdjacentPlot();
					if adjacentPlot then
						-- print("Non-nil River Plot!");
						local value = GetMississippiRiverValueAtPlot(adjacentPlot); -- CUSTOM
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
		DoMississippiRiver(riverPlot, bestFlowDirection, originalFlowDirection, riverID);
	end
end
------------------------------------------------------------------------------
function AddRivers()
	local iW, iH = Map.GetGridSize()

	-- Place the Mississippi!
	print("Charting the Mississippi (Lua Great Plains) ...");
	local startX = math.floor((iW - 1) * 0.78);
	local startY = iH - 2;
	local left = math.floor((iW - 1) * 0.74);
	local right = math.floor((iW - 1) * 0.9);
	local startPlot = Map.GetPlot(startX, startY);
	local startInlandCorner = startPlot:GetInlandCorner();
	local originalFlowDirection = FlowDirectionTypes.FLOWDIRECTION_SOUTH;
	DoMississippiRiver(startInlandCorner, nil, originalFlowDirection, nil);

	print("Great Plains - Adding Remaining Rivers");
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
	}
	for iPass, passCondition in ipairs(passConditions) do
		local riverSourceRange;
		local seaWaterRange;
		if iPass <= 2 then
			riverSourceRange = GameDefines.RIVER_SOURCE_MIN_RIVER_RANGE;
			seaWaterRange = GameDefines.RIVER_SOURCE_MIN_SEAWATER_RANGE;
		else
			riverSourceRange = (GameDefines.RIVER_SOURCE_MIN_RIVER_RANGE / 2);
			seaWaterRange = (GameDefines.RIVER_SOURCE_MIN_SEAWATER_RANGE / 2);
		end
		for _, plot in Plots() do
			local current_x = plot:GetX();
			local current_y = plot:GetY();
			if current_x < 1 or current_x >= iW - 2 or current_y < 2 or current_y >= iH - 1 then
				-- Plot too close to edge, ignore it.
			elseif current_x >= left and current_x <= right then
				-- Plot is inside Mississippi Corridor, ignore it.
			elseif not plot:IsWater() then
				if passCondition(plot) then
					if not Map.FindWater(plot, riverSourceRange, true) then
						if not Map.FindWater(plot, seaWaterRange, false) then
							local inlandCorner = plot:GetInlandCorner();
							if inlandCorner then
								local start_x = inlandCorner:GetX();
								local start_y = inlandCorner:GetY();
								local orig_direction;
								if start_y < iH / 3 then -- South third of map
									if start_x <= left then -- SW Corner
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_NORTHEAST;
									elseif start_x >= right then -- SE Corner
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_NORTHWEST;
									end
								else -- North half of map
									if start_x < left then -- NW corner
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST;
									elseif start_x > right then -- NE corner
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST;
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
function AssignStartingPlots:__CustomInit()
	-- This function included to provide a quick and easy override for changing
	-- any initial settings. Add your customized version to the map script.
	self.CanBeGeyser = AssignStartingPlots.CanBeGeyser;
	self.CanBeCrater = AssignStartingPlots.CanBeCrater;
	self.CanBeMesa = AssignStartingPlots.CanBeMesa;
	self.geyser_list, self.crater_list, self.mesa_list = {}, {}, {};
end
------------------------------------------------------------------------------
function AssignStartingPlots:DetermineRegionTypes()
	-- Determine region type and conditions. Use self.regionTypes to store the results

	-- REGION TYPES
	-- 0. Undefined
	-- 1. Tundra
	-- 2. Jungle
	-- 3. Forest
	-- 4. Desert
	-- 5. Hills
	-- 6. Plains
	-- 7. Grassland
	-- 8. Hybrid
	-- 9. Mountain
	-- 10. Snow

	-- Main loop
	for this_region, terrainCounts in ipairs(self.regionTerrainCounts) do
		-- Set each region to "Undefined Type" as default.
		-- If all efforts fail at determining what type of region this should be, region type will remain Undefined.
		-- local totalPlots = terrainCounts[1];
		local regionPlots = terrainCounts[2];
		-- local waterCount = terrainCounts[3];
		local flatlandsCount = terrainCounts[4];
		local hillsCount = terrainCounts[5];
		-- local peaksCount = terrainCounts[6];
		-- local lakeCount = terrainCounts[7];
		-- local coastCount = terrainCounts[8];
		-- local oceanCount = terrainCounts[9];
		-- local iceCount = terrainCounts[10];
		local grassCount = terrainCounts[11];
		local plainsCount = terrainCounts[12];
		local desertCount = terrainCounts[13];
		local tundraCount = terrainCounts[14];
		-- local snowCount = terrainCounts[15];
		local forestCount = terrainCounts[16];
		-- local jungleCount = terrainCounts[17];
		local marshCount = terrainCounts[18];
		-- local riverCount = terrainCounts[19];
		-- local floodplainCount = terrainCounts[20];
		-- local oasisCount = terrainCounts[21];
		-- local coastalLandCount = terrainCounts[22];
		-- local nextToCoastCount = terrainCounts[23];

		-- If Rectangular regional division, then water plots would be included in region plots.
		-- Let's recalculate region plots based only on flatland and hills plots.
		if self.method == RegionDivision.RECTANGULAR or self.method == RegionDivision.RECTANGULAR_SELF_DEFINED then
			regionPlots = flatlandsCount + hillsCount;
		end

		local found_region = false;
		-- MOD.Barathor: These are the minimum values
		local desert_percent = 0.20;
		local tundra_percent = 0.20;
		local jungle_percent = 0.08;
		local forest_percent = 0.25;
		local hills_percent = 0.25;
		local plains_percent = 0.30;
		local grass_percent = 0.30;

		-- MOD.Barathor: This variable will decrement until a region is assigned; starts off very high.
		local adjustment = 0.50;

		-- MOD.Barathor: Reordered condition checks and modified what some checks include.
		while not found_region do
			-- Desert check.
			if desertCount >= regionPlots * (desert_percent + adjustment) then
				table.insert(self.regionTypes, RegionTypes.REGION_DESERT);
				print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
				print("Region #", this_region, " has been defined as a DESERT Region.");
				found_region = true;

			-- Tundra check.
			elseif tundraCount >= regionPlots * (tundra_percent + adjustment) then
				table.insert(self.regionTypes, RegionTypes.REGION_TUNDRA);
				print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
				print("Region #", this_region, " has been defined as a TUNDRA Region.");
				found_region = true;

			-- Jungle (now Marsh) check.
			elseif marshCount >= regionPlots * (jungle_percent + adjustment) then
				table.insert(self.regionTypes, RegionTypes.REGION_JUNGLE);
				print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
				print("Region #", this_region, " has been defined as a JUNGLE Region.");
				found_region = true;

			-- Forest check.
			elseif forestCount >= regionPlots * (forest_percent + adjustment) and tundraCount < regionPlots * tundra_percent then
				table.insert(self.regionTypes, RegionTypes.REGION_FOREST);
				print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
				print("Region #", this_region, " has been defined as a FOREST Region.");
				found_region = true;

			-- Hills check.
			elseif hillsCount >= regionPlots * (hills_percent + adjustment) then
				table.insert(self.regionTypes, RegionTypes.REGION_HILLS);
				print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
				print("Region #", this_region, " has been defined as a HILLS Region.");
				found_region = true;

			else
				if adjustment <= 0 then
					-- Plains check.
					if plainsCount >= regionPlots * plains_percent and plainsCount * 0.8 > grassCount then
						table.insert(self.regionTypes, RegionTypes.REGION_PLAINS);
						print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
						print("Region #", this_region, " has been defined as a PLAINS Region.");
						found_region = true;
					-- Grass check.
					elseif grassCount >= regionPlots * grass_percent and grassCount * 0.8 > plainsCount then
						table.insert(self.regionTypes, RegionTypes.REGION_GRASSLAND);
						print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
						print("Region #", this_region, " has been defined as a GRASSLAND Region.");
						found_region = true;
					else
						-- Hybrid: No conditions dominate or other mods have included new terrain/feature/plot types which aren't recognized here.
						table.insert(self.regionTypes, RegionTypes.REGION_HYBRID);
						print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
						print("Region #", this_region, " has been defined as a HYBRID Region.");
						found_region = true;
					end
				end
				adjustment = adjustment - 0.01;
			end
		end

		-- MOD.Barathor: New data for very useful debug printouts.
		-- print("Threshold Values:")
		-- print(string.format("Desert: %.2f - Tundra: %.2f - Jungle: %.2f - Forest: %.2f - Hills: %.2f - Plains: %.2f - Grass: %.2f", desert_percent, tundra_percent, jungle_percent, forest_percent, hills_percent, plains_percent, grass_percent))
		-- print("Region Values:")
		-- print(string.format("Desert: %.2f - Tundra: %.2f - Jungle: %.2f - Forest: %.2f - Hills: %.2f - Plains: %.2f - Grass: %.2f", desertCount / regionPlots, tundraCount / regionPlots, marshCount / regionPlots, forestCount / regionPlots, hillsCount / regionPlots, plainsCount / regionPlots, grassCount / regionPlots))
		-- print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -")

	end

	-- Added by azum4roll: Support for Mountain and Snow "bias"
	-- This changes one region to a "Mountain region" or "Snow region", if a civ with the respective start bias exists
	-- It's possible that there's no snow/mountain on the map, so we need to handle it later in BalanceAndAssign

	if self.hasSnowBias then
		-- Get ID of region with the most snow, prioritizing tundra regions first
		local snowRegion = -1;
		local snowCount = 0;
		for this_region, terrainCounts in ipairs(self.regionTerrainCounts) do
			local regionType = self.regionTypes[this_region];
			local thisSnowCount = terrainCounts[15];
			if regionType == RegionTypes.REGION_TUNDRA then
				thisSnowCount = thisSnowCount + 1000;
			end
			if thisSnowCount > snowCount then
				snowRegion = this_region;
				snowCount = thisSnowCount;
			end
		end

		if snowRegion ~= -1 then
			self.regionTypes[snowRegion] = RegionTypes.REGION_SNOW;
			print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
			print("Region #", snowRegion, " has been converted into a SNOW Region.");
		end
	end

	if self.hasMountainBias then
		-- Get ID of region with the most mountains
		local mountainRegion = -1;
		local mountainCount = 0;
		for this_region, terrainCounts in ipairs(self.regionTerrainCounts) do
			local regionType = self.regionTypes[this_region];
			local thisMountainCount = terrainCounts[6];
			if thisMountainCount > mountainCount and regionType ~= RegionTypes.REGION_SNOW then
				mountainRegion = this_region;
				mountainCount = thisMountainCount;
			end
		end

		if mountainRegion ~= -1 then
			self.regionTypes[mountainRegion] = RegionTypes.REGION_MOUNTAIN;
			print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
			print("Region #", mountainRegion, " has been converted into a MOUNTAIN Region.");
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:GenerateCandidatePlotListsForSpecificNW()
	-- Only check Old Faithful, Barringer Crater and Grand Mesa.
	local iW, iH = Map.GetGridSize();
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local landEligibility, _ = self:ExaminePlotForNaturalWondersEligibility(x, y);
			-- Plot has passed checks applicable to all NW types. Move on to specific checks.
			if landEligibility then
				for nw_number, _ in ipairs(self.xml_row_numbers) do
					if self.wonder_list[nw_number] == "FEATURE_GEYSER" or
						self.wonder_list[nw_number] == "FEATURE_CRATER" or
						self.wonder_list[nw_number] == "FEATURE_MESA" then

						self:CanBeThisNaturalWonderType(x, y, nw_number);
					end
				end
			end
		end
	end
end
------------------------------------------------------------------------------
function PlaceBuffalo(ASP)
	-- Great Plains, Buffalo Herds
	print("Placing Buffalo Herds (Cows - Lua Great Plains) ...");

	local iW, iH = Map.GetGridSize();
	local herds = Fractal.Create(iW, iH, 7, {}, -1, -1);
	local iHerdsClumps = herds:GetHeight(3);
	local iHerdsBottom1 = herds:GetHeight(24);
	local iHerdsTop1 = herds:GetHeight(27);
	local iHerdsBottom2 = herds:GetHeight(73);
	local iHerdsTop2 = herds:GetHeight(76);
	-- More herds in the northern 5/8ths of the map.
	local herdNorth = iH - 1;
	local herdSouth = math.floor(iH * 0.37);
	local herdWest = math.floor(iW / 5);
	local herdEast = math.floor((2 * iW) / 3);
	local herdSlideRange = math.floor((herdEast - herdWest) / 6);
	for y = herdSouth, herdNorth do
		local herdLeft = herdWest + Map.Rand(herdSlideRange, "Herds, West Boundary - Great Plains Lua");
		local herdRight = herdEast - Map.Rand(herdSlideRange, "Herds, East Boundary - Great Plains Lua");
		for x = herdLeft, herdRight do
			-- Fractalized placement of herds
			local plot = Map.GetPlot(x, y);
			if plot:IsWater() or plot:IsMountain() or plot:GetFeatureType() == FeatureTypes.FEATURE_OASIS then
				-- No buffalo at the water hole, sorry!
			elseif plot:GetResourceType() == -1 then
				local herdVal = herds:GetHeight(x, y);
				if herdVal <= iHerdsClumps or (herdVal >= iHerdsBottom1 and herdVal <= iHerdsTop1) or (herdVal >= iHerdsBottom2 and herdVal <= iHerdsTop2) then
					plot:SetResourceType(ASP.cow_ID, 1);
					ASP.amounts_of_resources_placed[ASP.cow_ID + 1] = ASP.amounts_of_resources_placed[ASP.cow_ID + 1] + 1;
				end
			end
		end
	end
	-- Fewer herds in the southern 3/8ths of the map.
	herdNorth = math.floor(iH * 0.37);
	herdSouth = math.floor(iH * 0.15);
	herdWest = math.floor(iW * 0.32);
	herdEast = math.floor(iW * 0.59);
	herdSlideRange = math.floor((herdEast - herdWest) / 5);
	for y = herdSouth, herdNorth do
		local herdLeft = herdWest + Map.Rand(herdSlideRange, "Herds, West Boundary - Great Plains Lua");
		local herdRight = herdEast - Map.Rand(herdSlideRange, "Herds, East Boundary - Great Plains Lua");
		for x = herdLeft, herdRight do
			-- Fractalized placement of herds
			local plot = Map.GetPlot(x, y);
			if plot:IsWater() or plot:IsMountain() or plot:GetFeatureType() == FeatureTypes.FEATURE_OASIS then
				-- No buffalo at the water hole, sorry!
			elseif plot:GetResourceType() == -1 then
				local herdVal = herds:GetHeight(x, y);
				if (herdVal >= iHerdsBottom1 and herdVal <= iHerdsTop1) or (herdVal >= iHerdsBottom2 and herdVal <= iHerdsTop2) then
					plot:SetResourceType(ASP.cow_ID, 1);
				end
			end
		end
	end
end
------------------------------------------------------------------------------
function StartPlotSystem()
	print("Creating start plot database (MapGenerator.Lua)");
	local start_plot_database = AssignStartingPlots.Create();

	start_plot_database.PlaceBuffalo = PlaceBuffalo;
	start_plot_database.oldPlaceBonusResources = start_plot_database.PlaceBonusResources;
	local newPlaceBonusResources = function (ASP)
		ASP:PlaceBuffalo();
		ASP:oldPlaceBonusResources();
	end
	start_plot_database.PlaceBonusResources = newPlaceBonusResources;

	print("Dividing the map in to Regions (Lua Inland Sea)");
	-- Regional Division Method 1: Biggest Landmass
	local args = {
		method = 1,
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

-------------------------------------------------------------------------------
function DetermineContinents()
	-- Setting all continental art to America style.
	for i, plot in Plots() do
		if plot:IsWater() then
			plot:SetContinentArtType(0);
		else
			plot:SetContinentArtType(1);
		end
	end
end
-------------------------------------------------------------------------------
