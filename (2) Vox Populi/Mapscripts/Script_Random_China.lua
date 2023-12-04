-------------------------------------------------------------------------------
--	FILE:	 China.lua
--	AUTHOR:  Bob Thomas (Sirian)
--	PURPOSE: Regional map script - Chinese Heartland
-------------------------------------------------------------------------------
--	Copyright (c) 2013 Firaxis Games, Inc. All rights reserved.
-------------------------------------------------------------------------------

include("MapGenerator");
include("FractalWorld");
include("TerrainGenerator");
include("FeatureGenerator");

local himalayas = {};
local sichuan = {};
local desert = {};
local tibetan_plateau = {};
local central_china = {};
local south_china = {};
local arid_northwest = {};
local grasslands = {};

------------------------------------------------------------------------------
function GetMapScriptInfo()
	return {
		Name = "TXT_KEY_CW_MAP_PACK_CHINA_VP",
		Description = "TXT_KEY_CW_MAP_PACK_CHINA_DESC",
		IconIndex = 26,
		IconAtlas = "WORLDTYPE_ATLAS_3",
		Folder = "MAP_FOLDER_SP_DLC_3",
	};
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	-- China has fully custom grid sizes to match the slice of Earth being represented.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {26, 20},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {36, 26},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {44, 32},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {52, 36},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {60, 42},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {72, 50},
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
-- China uses custom plot generation with regional specificity.
------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Setting Plot Types (Lua China) ...");
	local iW, iH = Map.GetGridSize();
	-- Initiate plot table, fill all data slots with type PLOT_LAND
	local plotTypes = {};
	table.fill(plotTypes, PlotTypes.PLOT_LAND, iW * iH);

	-- Grains for reducing "clumping" of hills/peaks.
	local grainvalues = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 6,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 6,
	};
	local grain_amount = grainvalues[Map.GetWorldSize()];

	local hillsFrac = Fractal.Create(iW, iH, grain_amount, {}, 7, 6);
	local peaksFrac = Fractal.Create(iW, iH, grain_amount + 1, {}, 7, 6);

	local iHillsBottom1 = hillsFrac:GetHeight(20);
	local iHillsTop1 = hillsFrac:GetHeight(30);
	local iHillsBottom2 = hillsFrac:GetHeight(70);
	local iHillsTop2 = hillsFrac:GetHeight(80);
	local iForty = hillsFrac:GetHeight(40);
	local iSixty = hillsFrac:GetHeight(60);
	local iPeakTibet = peaksFrac:GetHeight(87);
	local iPeakNW = peaksFrac:GetHeight(95);
	local iPeakSichuan = peaksFrac:GetHeight(97);
	local iHillsRegional = hillsFrac:GetHeight(94);

	-- Define the Pacific.
	print("Simulate the Pacific Ocean. (Lua China) ...");
	local pacific_coords = {
		{math.floor(iW * 0.56), 0},
		{math.floor(iW * 0.52), math.floor(iH * 0.08)},
		{math.floor(iW - 1), math.floor(iH * 0.9)},
	};
	-- Draw the Pacific Line and fill in everything east of it with ocean.
	for loop = 1, 2 do
		local startX = pacific_coords[loop][1];
		local startY = pacific_coords[loop][2];
		local endX = pacific_coords[loop + 1][1];
		local endY = pacific_coords[loop + 1][2];
		local dx = endX - startX;
		local dy = endY - startY;
		local slope = 0;
		if dy ~= 0 then
			slope = dx / dy;
		end
		local x = startX;
		for y = startY, endY - 1 do
			x = x + slope;
			local iX = math.floor(x);
			for loop_x = iX, iW - 1 do
				local i = y * iW + loop_x + 1;
				plotTypes[i] = PlotTypes.PLOT_OCEAN;
			end
		end
	end

	-- Define the Indian, which will be in the SW corner.
	local indian_coords = {
		{0, math.floor(iH * 0.16)},
		{math.floor(iW * 0.08), math.floor(iH * 0.2)},
		{math.floor(iW * 0.16), 0},
	};
	-- Draw the Indian Line and fill in everything south of it with ocean.
	for loop = 1, 2 do
		local startX = indian_coords[loop][1];
		local startY = indian_coords[loop][2];
		local endX = indian_coords[loop + 1][1];
		local endY = indian_coords[loop + 1][2];
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
			for loop_y = 0, iY do
				local i = loop_y * iW + x + 1;
				plotTypes[i] = PlotTypes.PLOT_OCEAN;
			end
		end
	end

-- Add the mainland bulge as an oval of land plots.
	local centerX = iW * 0.64;
	local centerY = iH * 0.52;
	local majorAxis = iW * 0.32;
	local minorAxis = iH * 0.37;
	local majorAxisSquared = majorAxis * majorAxis;
	local minorAxisSquared = minorAxis * minorAxis;
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local deltaX = x - centerX;
			local deltaY = y - centerY;
			local deltaXSquared = deltaX * deltaX;
			local deltaYSquared = deltaY * deltaY;
			local d = deltaXSquared/majorAxisSquared + deltaYSquared/minorAxisSquared;
			if d <= 1 then
				if y <= iH - 1 then
					local i = y * iW + x + 1;
					plotTypes[i] = PlotTypes.PLOT_LAND;
				end
			end
		end
	end

	-- Define the hilly regions and append their plots to their plot lists. GLOBAL variables used here.
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local i = y * iW + x + 1;
			if x <= iW * 0.15 and y >= iH * 0.4 and y <= iH * 0.47 then
				table.insert(himalayas, i);
			elseif x >= iW * 0.225 and x <= iW * 0.44 and y >= iH * 0.32 and y <= iH * 0.61 then
				table.insert(sichuan, i);
			elseif x <= iW * 0.26 and y >= iH * 0.83 then
				table.insert(desert, i);
			elseif x >= iW * 0.54 and x <= iW * 0.65 and y >= iH * 0.5 and y <= iH * 0.72 then
				table.insert(central_china, i);
			elseif x <= iW * 0.72 and y >= iH * 0.81 then
				table.insert(arid_northwest, i);
			elseif x <= iW * 0.4 and y >= iH * 0.45 then
				table.insert(tibetan_plateau, i);
			elseif x >= iW * 0.35 and y <= iH * 0.45 then
				table.insert(south_china, i);
			elseif x >= iW * 0.4 and y >= iH * 0.4 then
				table.insert(grasslands, i);
			end
		end
	end

	-- Now assign plot types. Note, the plot table is already filled with flatlands.
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = y * iW + x + 1;
			-- Regional membership checked, effects chosen.
			-- Python had a simpler, less verbose method for checking table membership.
			local inHima = false;
			local inSichuan = false;
			local inDesert = false;
			local inCent = false;
			local inArid = false;
			local inTibet = false;
			local inSouth = false;
			local inGrass = false;
			for _, plotIndex in ipairs(himalayas) do
				if i == plotIndex then
					inHima = true;
					break;
				end
			end
			for _, plotIndex in ipairs(sichuan) do
				if i == plotIndex then
					inSichuan = true;
					break;
				end
			end
			for _, plotIndex in ipairs(desert) do
				if i == plotIndex then
					inDesert = true;
					break;
				end
			end
			for _, plotIndex in ipairs(central_china) do
				if i == plotIndex then
					inCent = true;
					break;
				end
			end
			for _, plotIndex in ipairs(arid_northwest) do
				if i == plotIndex then
					inArid = true;
					break;
				end
			end
			for _, plotIndex in ipairs(tibetan_plateau) do
				if i == plotIndex then
					inTibet= true;
					break;
				end
			end
			for _, plotIndex in ipairs(south_china) do
				if i == plotIndex then
					inSouth = true;
					break;
				end
			end
			for _, plotIndex in ipairs(grasslands) do
				if i == plotIndex then
					inGrass = true;
					break;
				end
			end
			local hillVal = hillsFrac:GetHeight(x,y);
			if inHima then
				plotTypes[i] = PlotTypes.PLOT_PEAK;
			elseif inSichuan then
				if hillVal >= iSixty then
					local peakVal = peaksFrac:GetHeight(x,y);
					if peakVal >= iPeakSichuan then
						plotTypes[i] = PlotTypes.PLOT_PEAK;
					else
						plotTypes[i] = PlotTypes.PLOT_HILLS;
					end
				end
			elseif inDesert then
				if hillVal >= iHillsTop2 then
					local peakVal = peaksFrac:GetHeight(x,y);
					if peakVal >= iPeakNW then
						plotTypes[i] = PlotTypes.PLOT_PEAK;
					else
						plotTypes[i] = PlotTypes.PLOT_HILLS;
					end
				end
			elseif inCent then
				if (hillVal >= iHillsBottom1 and hillVal <= iForty) or (hillVal >= iSixty and hillVal <= iHillsTop2) then
					plotTypes[i] = PlotTypes.PLOT_HILLS;
				end
			elseif inArid then
				if hillVal >= iSixty then
					local peakVal = peaksFrac:GetHeight(x,y);
					if peakVal >= iPeakNW then
						plotTypes[i] = PlotTypes.PLOT_PEAK;
					else
						plotTypes[i] = PlotTypes.PLOT_HILLS;
					end
				end
			elseif inTibet then
				if hillVal >= iForty then
					local peakVal = peaksFrac:GetHeight(x,y);
					if peakVal >= iPeakTibet then
						plotTypes[i] = PlotTypes.PLOT_PEAK;
					else
						plotTypes[i] = PlotTypes.PLOT_HILLS;
					end
				end
			elseif inSouth then
				if plotTypes[i] ~= PlotTypes.PLOT_OCEAN then
					if (hillVal >= iHillsBottom1 and hillVal <= iHillsTop1) or (hillVal >= iHillsBottom2 and hillVal <= iHillsTop2) then
						plotTypes[i] = PlotTypes.PLOT_HILLS;
					end
				end
			elseif inGrass then
				if plotTypes[i] ~= PlotTypes.PLOT_OCEAN then
					if hillVal >= iHillsBottom1 and hillVal <= iHillsTop1 then
						plotTypes[i] = PlotTypes.PLOT_HILLS;
					end
				end
			else
				if plotTypes[i] ~= PlotTypes.PLOT_OCEAN then
					if hillVal >= iHillsRegional then
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
-- China uses a custom terrain generation.
------------------------------------------------------------------------------
function GenerateTerrain()
	print("Generating Terrain (Lua China) ...");
	local iW, iH = Map.GetGridSize();
	local terrainTypes = {};
	local terrainDesert = TerrainTypes.TERRAIN_DESERT;
	local terrainPlains = TerrainTypes.TERRAIN_PLAINS;
	local terrainGrass = TerrainTypes.TERRAIN_GRASS;

	-- Initiate terrain table, fill all land slots with type TERRAIN_GRASS
	table.fill(terrainTypes, terrainGrass, iW * iH);
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local plot = Map.GetPlot(x, y);
			if plot:IsWater() then
				local i = y * iW + x; -- C++ Plot indices, starting at 0.
				terrainTypes[i] = plot:GetTerrainType();
			end
		end
	end

	-- Set up fractals and thresholds
	local plains_check = Fractal.Create(iW, iH, 5, {}, 6, 6);
	local desert_check = Fractal.Create(iW, iH, 4, {}, 6, 6);
	local iHunan = plains_check:GetHeight(65);
	local iDesert = desert_check:GetHeight(30);
	local iDesertPlains = plains_check:GetHeight(15);
	local iTibet = plains_check:GetHeight(25);
	local iNW = plains_check:GetHeight(35);

	-- Main loop
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = y * iW + x + 1;
			local plot = Map.GetPlot(x, y);
			if plot:IsWater() then
				terrainTypes[i - 1] = plot:GetTerrainType();
			else
				local plainsVal = plains_check:GetHeight(x, y);
				local inSichuan = false;
				local inDesert = false;
				local inArid = false;
				local inTibet = false;
				for _, plotIndex in ipairs(sichuan) do
					if i == plotIndex then
						inSichuan = true;
						break;
					end
				end
				for _, plotIndex in ipairs(desert) do
					if i == plotIndex then
						inDesert = true;
						break;
					end
				end
				for _, plotIndex in ipairs(arid_northwest) do
					if i == plotIndex then
						inArid = true;
						break;
					end
				end
				for _, plotIndex in ipairs(tibetan_plateau) do
					if i == plotIndex then
						inTibet = true;
						break;
					end
				end
				if inSichuan then
					if plainsVal >= iHunan then
						terrainTypes[i - 1] = terrainPlains;
					end
				elseif inDesert then
					if plainsVal >= iDesertPlains then
						local desertVal = desert_check:GetHeight(x, y);
						if desertVal >= iDesert then
							terrainTypes[i - 1] = terrainDesert;
						else
							terrainTypes[i - 1] = terrainPlains;
						end
					end
				elseif inArid then
					if plainsVal >= iNW then
						terrainTypes[i - 1] = terrainPlains;
					end
				elseif inTibet then
					if plainsVal >= iTibet then
						terrainTypes[i - 1] = terrainPlains;
					end
				elseif x >= iW * 0.57 and x <= iW * 0.75 and y >= iH * 0.31 and y <= iH * 0.47 then
					if plainsVal >= iHunan then
						terrainTypes[i - 1] = terrainPlains;
					end
				end
			end
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
					-- Top row of map, needs special handling
					elseif not adjacentPlot and riverPlotY == iH - 1 then
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
					-- Left column of map, needs special handling
					elseif not adjacentPlot and riverPlotX == 0 then
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
-- China uses a custom feature generation.
------------------------------------------------------------------------------
function FeatureGenerator:GetLatitudeAtPlot(_, iY)
	local lat = 0.45 * (iY / self.iGridH);
	return lat;
end
------------------------------------------------------------------------------
function FeatureGenerator:AddIceAtPlot()
	-- No ice on this map.
end
------------------------------------------------------------------------------
function FeatureGenerator:AddAtolls()
end
------------------------------------------------------------------------------
function AddFeatures()
	print("Adding Features (Lua China) ...");

	local featuregen = FeatureGenerator.Create{rainfall = 2};

	featuregen:AddFeatures();
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function StartPlotSystem()
	print("Creating start plot database.");
	local start_plot_database = AssignStartingPlots.Create();

	print("Dividing the map in to Regions.");
	-- Regional Division Method 1: Biggest Landmass
	start_plot_database:GenerateRegions{method = 1};

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
	-- Setting all continental art to Asia style.
	for _, plot in Plots() do
		if plot:IsWater() then
			plot:SetContinentArtType(0);
		else
			plot:SetContinentArtType(2);
		end
	end
end
-------------------------------------------------------------------------------
