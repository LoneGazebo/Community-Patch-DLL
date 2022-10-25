------------------------------------------------------------------------------
--	FILE:	 Frontier.lua
--	AUTHOR:  Bob Thomas
--	PURPOSE: Global map script - Adds extra, unpopulated regions to the map.
------------------------------------------------------------------------------
--	Copyright (c) 2012 Firaxis Games, Inc. All rights reserved.
------------------------------------------------------------------------------

include("MapGenerator");
include("FractalWorld");
include("FeatureGenerator");
include("TerrainGenerator");

------------------------------------------------------------------------------

------------------------------------------------------------------------------
function GetMapScriptInfo()
	local world_age, temperature, rainfall, sea_level, resources = GetCoreMapOptions()
	return {
		Name = "TXT_KEY_MAP_FRONTIER",
		Description = "TXT_KEY_MAP_FRONTIER_HELP",
		IsAdvancedMap = 0,
		IconIndex = 1,
		SortIndex = 1,
		CustomOptions = {world_age, temperature, rainfall, sea_level, resources,
			{
				Name = "TXT_KEY_MAP_OPTION_LANDMASS_TYPE",
				Values = {
					{"TXT_KEY_MAP_OPTION_PANGAEA", "TXT_KEY_MAP_PANGAEA_HELP"},
					{"TXT_KEY_MAP_OPTION_LARGE_CONTINENTS", "TXT_KEY_MAP_CONTINENTS_HELP"},
					"TXT_KEY_MAP_OPTION_RANDOM",
				},
				DefaultValue = 3,
				SortPriority = 1,
			},
		};
	};
end
------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	-- Customized for Frontier. Large and Huge truncated a bit to aid performance.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {52, 32},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {64, 40},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {84, 52},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {100, 64},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {116, 76},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {132, 88}
		}
	local grid_size = worldsizes[worldSize];
	--
	local world = GameInfo.Worlds[worldSize];
	if(world ~= nil) then
	return {
		Width = grid_size[1],
		Height = grid_size[2],
		WrapX = true,
	};      
     end
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
PangaeaFractalWorld = {};
------------------------------------------------------------------------------
function PangaeaFractalWorld.Create(fracXExp, fracYExp)
	local gridWidth, gridHeight = Map.GetGridSize();
	
	local data = {
		InitFractal = FractalWorld.InitFractal,
		ShiftPlotTypes = FractalWorld.ShiftPlotTypes,
		ShiftPlotTypesBy = FractalWorld.ShiftPlotTypesBy,
		DetermineXShift = FractalWorld.DetermineXShift,
		DetermineYShift = FractalWorld.DetermineYShift,
		GenerateCenterRift = FractalWorld.GenerateCenterRift,
		GeneratePlotTypes = PangaeaFractalWorld.GeneratePlotTypes,	-- Custom method
		
		iFlags = Map.GetFractalFlags(),
		
		fracXExp = fracXExp,
		fracYExp = fracYExp,
		
		iNumPlotsX = gridWidth,
		iNumPlotsY = gridHeight,
		plotTypes = table.fill(PlotTypes.PLOT_OCEAN, gridWidth * gridHeight)
	};
		
	return data;
end	
------------------------------------------------------------------------------
function PangaeaFractalWorld:GeneratePlotTypes(args)
	if(args == nil) then args = {}; end
	
	local sea_level_low = 63;
	local sea_level_normal = 68;
	local sea_level_high = 73;
	local world_age_old = 2;
	local world_age_normal = 3;
	local world_age_new = 5;
	--
	local extra_mountains = 6;
	local grain_amount = 3;
	local adjust_plates = 1.3;
	local shift_plot_types = true;
	local tectonic_islands = true;
	local hills_ridge_flags = self.iFlags;
	local peaks_ridge_flags = self.iFlags;
	local has_center_rift = false;
	
	local sea_level = Map.GetCustomOption(4)
	if sea_level == 4 then
		sea_level = 1 + Map.Rand(3, "Random Sea Level - Lua");
	end
	local world_age = Map.GetCustomOption(1)
	if world_age == 4 then
		world_age = 1 + Map.Rand(3, "Random World Age - Lua");
	end

	-- Set Sea Level according to user selection.
	local water_percent = sea_level_normal;
	if sea_level == 1 then -- Low Sea Level
		water_percent = sea_level_low
	elseif sea_level == 3 then -- High Sea Level
		water_percent = sea_level_high
	else -- Normal Sea Level
	end

	-- Set values for hills and mountains according to World Age chosen by user.
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
	local hillsClumps = 1 + adjustment;
	local hillsNearMountains = 91 - (adjustment * 2) - extra_mountains;
	local mountains = 97 - adjustment - extra_mountains;

	-- Hills and Mountains handled differently according to map size - Bob
	local WorldSizeTypes = {};
	for row in GameInfo.Worlds() do
		WorldSizeTypes[row.Type] = row.ID;
	end
	local sizekey = Map.GetWorldSize();
	-- Fractal Grains
	local sizevalues = {
		[WorldSizeTypes.WORLDSIZE_DUEL]     = 3,
		[WorldSizeTypes.WORLDSIZE_TINY]     = 3,
		[WorldSizeTypes.WORLDSIZE_SMALL]    = 4,
		[WorldSizeTypes.WORLDSIZE_STANDARD] = 4,
		[WorldSizeTypes.WORLDSIZE_LARGE]    = 5,
		[WorldSizeTypes.WORLDSIZE_HUGE]		= 5
	};
	local grain = sizevalues[sizekey] or 3;
	-- Tectonics Plate Counts
	local platevalues = {
		[WorldSizeTypes.WORLDSIZE_DUEL]		= 6,
		[WorldSizeTypes.WORLDSIZE_TINY]     = 9,
		[WorldSizeTypes.WORLDSIZE_SMALL]    = 12,
		[WorldSizeTypes.WORLDSIZE_STANDARD] = 18,
		[WorldSizeTypes.WORLDSIZE_LARGE]    = 24,
		[WorldSizeTypes.WORLDSIZE_HUGE]     = 30
	};
	local numPlates = platevalues[sizekey] or 5;
	-- Add in any plate count modifications passed in from the map script. - Bob
	numPlates = numPlates * adjust_plates;

	-- Generate continental fractal layer and examine the largest landmass. Reject
	-- the result until the largest landmass occupies 84% or more of the total land.
	local done = false;
	local iAttempts = 0;
	local iWaterThreshold, biggest_area, iNumTotalLandTiles, iNumBiggestAreaTiles, iBiggestID;
	while done == false do
		local grain_dice = Map.Rand(7, "Continental Grain roll - LUA Pangaea");
		if grain_dice < 4 then
			grain_dice = 1;
		else
			grain_dice = 2;
		end
		local rift_dice = Map.Rand(3, "Rift Grain roll - LUA Pangaea");
		if rift_dice < 1 then
			rift_dice = -1;
		end
		
		self.continentsFrac = nil;
		self:InitFractal{continent_grain = grain_dice, rift_grain = rift_dice};
		iWaterThreshold = self.continentsFrac:GetHeight(water_percent);
		
		iNumTotalLandTiles = 0;
		for x = 0, self.iNumPlotsX - 1 do
			for y = 0, self.iNumPlotsY - 1 do
				local i = y * self.iNumPlotsX + x;
				local val = self.continentsFrac:GetHeight(x, y);
				if(val <= iWaterThreshold) then
					self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
				else
					self.plotTypes[i] = PlotTypes.PLOT_LAND;
					iNumTotalLandTiles = iNumTotalLandTiles + 1;
				end
			end
		end

		SetPlotTypes(self.plotTypes);
		Map.RecalculateAreas();
		
		biggest_area = Map.FindBiggestArea(false);
		iNumBiggestAreaTiles = biggest_area:GetNumTiles();
		-- Now test the biggest landmass to see if it is large enough.
		if iNumBiggestAreaTiles >= iNumTotalLandTiles * 0.84 then
			done = true;
			iBiggestID = biggest_area:GetID();
		end
		iAttempts = iAttempts + 1;
		
		--[[ Printout for debug use only
		print("-"); print("--- Pangaea landmass generation, Attempt#", iAttempts, "---");
		print("- This attempt successful: ", done);
		print("- Total Land Plots in world:", iNumTotalLandTiles);
		print("- Land Plots belonging to biggest landmass:", iNumBiggestAreaTiles);
		print("- Percentage of land belonging to Pangaea: ", 100 * iNumBiggestAreaTiles / iNumTotalLandTiles);
		print("- Continent Grain for this attempt: ", grain_dice);
		print("- Rift Grain for this attempt: ", rift_dice);
		print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
		print(".");
		]]--
	end
	
	-- Generate fractals to govern hills and mountains
	self.hillsFrac = Fractal.Create(self.iNumPlotsX, self.iNumPlotsY, grain, self.iFlags, self.fracXExp, self.fracYExp);
	self.mountainsFrac = Fractal.Create(self.iNumPlotsX, self.iNumPlotsY, grain, self.iFlags, self.fracXExp, self.fracYExp);
	self.hillsFrac:BuildRidges(numPlates, hills_ridge_flags, 1, 2);
	self.mountainsFrac:BuildRidges((numPlates * 2) / 3, peaks_ridge_flags, 6, 1);
	-- Get height values
	local iHillsBottom1 = self.hillsFrac:GetHeight(hillsBottom1);
	local iHillsTop1 = self.hillsFrac:GetHeight(hillsTop1);
	local iHillsBottom2 = self.hillsFrac:GetHeight(hillsBottom2);
	local iHillsTop2 = self.hillsFrac:GetHeight(hillsTop2);
	local iHillsClumps = self.mountainsFrac:GetHeight(hillsClumps);
	local iHillsNearMountains = self.mountainsFrac:GetHeight(hillsNearMountains);
	local iMountainThreshold = self.mountainsFrac:GetHeight(mountains);
	local iPassThreshold = self.hillsFrac:GetHeight(hillsNearMountains);
	-- Get height values for tectonic islands
	local iMountain100 = self.mountainsFrac:GetHeight(100);
	local iMountain99 = self.mountainsFrac:GetHeight(99);
	local iMountain97 = self.mountainsFrac:GetHeight(97);
	local iMountain95 = self.mountainsFrac:GetHeight(95);

	-- Because we haven't yet shifted the plot types, we will not be able to take advantage 
	-- of having water and flatland plots already set. We still have to generate all data
	-- for hills and mountains, too, then shift everything, then set plots one more time.
	for x = 0, self.iNumPlotsX - 1 do
		for y = 0, self.iNumPlotsY - 1 do
		
			local i = y * self.iNumPlotsX + x;
			local val = self.continentsFrac:GetHeight(x, y);
			local mountainVal = self.mountainsFrac:GetHeight(x, y);
			local hillVal = self.hillsFrac:GetHeight(x, y);
	
			if(val <= iWaterThreshold) then
				self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
				
				if tectonic_islands then -- Build islands in oceans along tectonic ridge lines - Brian
					if (mountainVal == iMountain100) then -- Isolated peak in the ocean
						self.plotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
					elseif (mountainVal == iMountain99) then
						self.plotTypes[i] = PlotTypes.PLOT_HILLS;
					elseif (mountainVal == iMountain97) or (mountainVal == iMountain95) then
						self.plotTypes[i] = PlotTypes.PLOT_LAND;
					end
				end
					
			else
				if (mountainVal >= iMountainThreshold) then
					if (hillVal >= iPassThreshold) then -- Mountain Pass though the ridgeline - Brian
						self.plotTypes[i] = PlotTypes.PLOT_HILLS;
					else -- Mountain
						self.plotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
					end
				elseif (mountainVal >= iHillsNearMountains) then
					self.plotTypes[i] = PlotTypes.PLOT_HILLS; -- Foot hills - Bob
				else
					if ((hillVal >= iHillsBottom1 and hillVal <= iHillsTop1) or (hillVal >= iHillsBottom2 and hillVal <= iHillsTop2)) then
						self.plotTypes[i] = PlotTypes.PLOT_HILLS;
					else
						self.plotTypes[i] = PlotTypes.PLOT_LAND;
					end
				end
			end
		end
	end

	self:ShiftPlotTypes();
	
	-- Now shift everything toward one of the poles, to reduce how much jungles tend to dominate this script.
	local shift_dice = Map.Rand(2, "Shift direction - LUA Pangaea");
	local iStartRow, iNumRowsToShift;
	local bFoundPangaea, bDoShift = false, false;
	if shift_dice == 1 then
		-- Shift North
		for y = self.iNumPlotsY - 2, 1, -1 do
			for x = 0, self.iNumPlotsX - 1 do
				local i = y * self.iNumPlotsX + x;
				if self.plotTypes[i] == PlotTypes.PLOT_HILLS or self.plotTypes[i] == PlotTypes.PLOT_LAND then
					local plot = Map.GetPlot(x, y);
					local iAreaID = plot:GetArea();
					if iAreaID == iBiggestID then
						bFoundPangaea = true;
						iStartRow = y + 1;
						if iStartRow < self.iNumPlotsY - 4 then -- Enough rows of water space to do a shift.
							bDoShift = true;
						end
						break
					end
				end
			end
			-- Check to see if we've found the Pangaea.
			if bFoundPangaea == true then
				break
			end
		end
	else
		-- Shift South
		for y = 1, self.iNumPlotsY - 2 do
			for x = 0, self.iNumPlotsX - 1 do
				local i = y * self.iNumPlotsX + x;
				if self.plotTypes[i] == PlotTypes.PLOT_HILLS or self.plotTypes[i] == PlotTypes.PLOT_LAND then
					local plot = Map.GetPlot(x, y);
					local iAreaID = plot:GetArea();
					if iAreaID == iBiggestID then
						bFoundPangaea = true;
						iStartRow = y - 1;
						if iStartRow > 3 then -- Enough rows of water space to do a shift.
							bDoShift = true;
						end
						break
					end
				end
			end
			-- Check to see if we've found the Pangaea.
			if bFoundPangaea == true then
				break
			end
		end
	end
	if bDoShift == true then
		if shift_dice == 1 then -- Shift North
			local iRowsDifference = self.iNumPlotsY - iStartRow - 2;
			local iRowsInPlay = math.floor(iRowsDifference * 0.7);
			local iRowsBase = math.ceil(iRowsDifference * 0.3);
			local rows_dice = Map.Rand(iRowsInPlay, "Number of Rows to Shift - LUA Pangaea");
			local iNumRows = math.min(iRowsDifference - 1, iRowsBase + rows_dice);
			local iNumEvenRows = 2 * math.floor(iNumRows / 2); -- MUST be an even number or we risk breaking a 1-tile isthmus and splitting the Pangaea.
			local iNumRowsToShift = math.max(2, iNumEvenRows);
			--print("-"); print("Shifting lands northward by this many plots: ", iNumRowsToShift); print("-");
			-- Process from top down.
			for y = (self.iNumPlotsY - 1) - iNumRowsToShift, 0, -1 do
				for x = 0, self.iNumPlotsX - 1 do
					local sourcePlotIndex = y * self.iNumPlotsX + x + 1;
					local destPlotIndex = (y + iNumRowsToShift) * self.iNumPlotsX + x + 1;
					self.plotTypes[destPlotIndex] = self.plotTypes[sourcePlotIndex]
				end
			end
			for y = 0, iNumRowsToShift - 1 do
				for x = 0, self.iNumPlotsX - 1 do
					local i = y * self.iNumPlotsX + x + 1;
					self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
				end
			end
		else -- Shift South
			local iRowsDifference = iStartRow - 1;
			local iRowsInPlay = math.floor(iRowsDifference * 0.7);
			local iRowsBase = math.ceil(iRowsDifference * 0.3);
			local rows_dice = Map.Rand(iRowsInPlay, "Number of Rows to Shift - LUA Pangaea");
			local iNumRows = math.min(iRowsDifference - 1, iRowsBase + rows_dice);
			local iNumEvenRows = 2 * math.floor(iNumRows / 2); -- MUST be an even number or we risk breaking a 1-tile isthmus and splitting the Pangaea.
			local iNumRowsToShift = math.max(2, iNumEvenRows);
			--print("-"); print("Shifting lands southward by this many plots: ", iNumRowsToShift); print("-");
			-- Process from bottom up.
			for y = 0, (self.iNumPlotsY - 1) - iNumRowsToShift do
				for x = 0, self.iNumPlotsX - 1 do
					local sourcePlotIndex = (y + iNumRowsToShift) * self.iNumPlotsX + x + 1;
					local destPlotIndex = y * self.iNumPlotsX + x + 1;
					self.plotTypes[destPlotIndex] = self.plotTypes[sourcePlotIndex]
				end
			end
			for y = self.iNumPlotsY - iNumRowsToShift, self.iNumPlotsY - 1 do
				for x = 0, self.iNumPlotsX - 1 do
					local i = y * self.iNumPlotsX + x + 1;
					self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
				end
			end
		end
	end

	return self.plotTypes;
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
ContinentsFractalWorld = {};
------------------------------------------------------------------------------
function ContinentsFractalWorld.Create(fracXExp, fracYExp)
	local gridWidth, gridHeight = Map.GetGridSize();
	
	local data = {
		InitFractal = FractalWorld.InitFractal,
		ShiftPlotTypes = FractalWorld.ShiftPlotTypes,
		ShiftPlotTypesBy = FractalWorld.ShiftPlotTypesBy,
		DetermineXShift = FractalWorld.DetermineXShift,
		DetermineYShift = FractalWorld.DetermineYShift,
		GenerateCenterRift = FractalWorld.GenerateCenterRift,
		GeneratePlotTypes = ContinentsFractalWorld.GeneratePlotTypes,	-- Custom method
		
		iFlags = Map.GetFractalFlags(),
		
		fracXExp = fracXExp,
		fracYExp = fracYExp,
		
		iNumPlotsX = gridWidth,
		iNumPlotsY = gridHeight,
		plotTypes = table.fill(PlotTypes.PLOT_OCEAN, gridWidth * gridHeight)
	};
		
	return data;
end	
------------------------------------------------------------------------------
function ContinentsFractalWorld:GeneratePlotTypes(args)
	if(args == nil) then args = {}; end
	
	local sea_level_low = 67;
	local sea_level_normal = 72;
	local sea_level_high = 76;
	local world_age_old = 2;
	local world_age_normal = 3;
	local world_age_new = 5;
	--
	local extra_mountains = 0;
	local grain_amount = 3;
	local adjust_plates = 1.0;
	local shift_plot_types = true;
	local tectonic_islands = false;
	local hills_ridge_flags = self.iFlags;
	local peaks_ridge_flags = self.iFlags;
	local has_center_rift = true;
	
	local sea_level = Map.GetCustomOption(4)
	if sea_level == 4 then
		sea_level = 1 + Map.Rand(3, "Random Sea Level - Lua");
	end
	local world_age = Map.GetCustomOption(1)
	if world_age == 4 then
		world_age = 1 + Map.Rand(3, "Random World Age - Lua");
	end

	-- Set Sea Level according to user selection.
	local water_percent = sea_level_normal;
	if sea_level == 1 then -- Low Sea Level
		water_percent = sea_level_low
	elseif sea_level == 3 then -- High Sea Level
		water_percent = sea_level_high
	else -- Normal Sea Level
	end

	-- Set values for hills and mountains according to World Age chosen by user.
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
	local hillsClumps = 1 + adjustment;
	local hillsNearMountains = 91 - (adjustment * 2) - extra_mountains;
	local mountains = 97 - adjustment - extra_mountains;

	-- Hills and Mountains handled differently according to map size
	local WorldSizeTypes = {};
	for row in GameInfo.Worlds() do
		WorldSizeTypes[row.Type] = row.ID;
	end
	local sizekey = Map.GetWorldSize();
	-- Fractal Grains
	local sizevalues = {
		[WorldSizeTypes.WORLDSIZE_DUEL]     = 3,
		[WorldSizeTypes.WORLDSIZE_TINY]     = 3,
		[WorldSizeTypes.WORLDSIZE_SMALL]    = 4,
		[WorldSizeTypes.WORLDSIZE_STANDARD] = 4,
		[WorldSizeTypes.WORLDSIZE_LARGE]    = 5,
		[WorldSizeTypes.WORLDSIZE_HUGE]		= 5
	};
	local grain = sizevalues[sizekey] or 3;
	-- Tectonics Plate Counts
	local platevalues = {
		[WorldSizeTypes.WORLDSIZE_DUEL]		= 6,
		[WorldSizeTypes.WORLDSIZE_TINY]     = 9,
		[WorldSizeTypes.WORLDSIZE_SMALL]    = 12,
		[WorldSizeTypes.WORLDSIZE_STANDARD] = 18,
		[WorldSizeTypes.WORLDSIZE_LARGE]    = 24,
		[WorldSizeTypes.WORLDSIZE_HUGE]     = 30
	};
	local numPlates = platevalues[sizekey] or 5;
	-- Add in any plate count modifications passed in from the map script.
	numPlates = numPlates * adjust_plates;

	-- Generate continental fractal layer and examine the largest landmass. Reject
	-- the result until the largest landmass occupies 58% or less of the total land.
	local done = false;
	local iAttempts = 0;
	local iWaterThreshold, biggest_area, iNumTotalLandTiles, iNumBiggestAreaTiles, iBiggestID;
	while done == false do
		local grain_dice = Map.Rand(7, "Continental Grain roll - LUA Continents");
		if grain_dice < 4 then
			grain_dice = 2;
		else
			grain_dice = 1;
		end
		local rift_dice = Map.Rand(3, "Rift Grain roll - LUA Continents");
		if rift_dice < 1 then
			rift_dice = -1;
		end
		
		self.continentsFrac = nil;
		self:InitFractal{continent_grain = grain_dice, rift_grain = rift_dice};
		iWaterThreshold = self.continentsFrac:GetHeight(water_percent);
		
		iNumTotalLandTiles = 0;
		for x = 0, self.iNumPlotsX - 1 do
			for y = 0, self.iNumPlotsY - 1 do
				local i = y * self.iNumPlotsX + x;
				local val = self.continentsFrac:GetHeight(x, y);
				if(val <= iWaterThreshold) then
					self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
				else
					self.plotTypes[i] = PlotTypes.PLOT_LAND;
					iNumTotalLandTiles = iNumTotalLandTiles + 1;
				end
			end
		end

		self:ShiftPlotTypes();
		self:GenerateCenterRift()

		SetPlotTypes(self.plotTypes);
		Map.RecalculateAreas();
		
		biggest_area = Map.FindBiggestArea(false);
		iNumBiggestAreaTiles = biggest_area:GetNumTiles();
		-- Now test the biggest landmass to see if it is large enough.
		if iNumBiggestAreaTiles <= iNumTotalLandTiles * 0.58 then
			done = true;
			iBiggestID = biggest_area:GetID();
		end
		iAttempts = iAttempts + 1;
		
		--[[ Printout for debug use only
		print("-"); print("--- Continents landmass generation, Attempt#", iAttempts, "---");
		print("- This attempt successful: ", done);
		print("- Total Land Plots in world:", iNumTotalLandTiles);
		print("- Land Plots belonging to biggest landmass:", iNumBiggestAreaTiles);
		print("- Percentage of land belonging to biggest: ", 100 * iNumBiggestAreaTiles / iNumTotalLandTiles);
		print("- Continent Grain for this attempt: ", grain_dice);
		print("- Rift Grain for this attempt: ", rift_dice);
		print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
		print(".");
		]]--
	end
	
	-- Generate fractals to govern hills and mountains
	self.hillsFrac = Fractal.Create(self.iNumPlotsX, self.iNumPlotsY, grain, self.iFlags, self.fracXExp, self.fracYExp);
	self.mountainsFrac = Fractal.Create(self.iNumPlotsX, self.iNumPlotsY, grain, self.iFlags, self.fracXExp, self.fracYExp);
	self.hillsFrac:BuildRidges(numPlates, hills_ridge_flags, 1, 2);
	self.mountainsFrac:BuildRidges((numPlates * 2) / 3, peaks_ridge_flags, 6, 1);
	-- Get height values
	local iHillsBottom1 = self.hillsFrac:GetHeight(hillsBottom1);
	local iHillsTop1 = self.hillsFrac:GetHeight(hillsTop1);
	local iHillsBottom2 = self.hillsFrac:GetHeight(hillsBottom2);
	local iHillsTop2 = self.hillsFrac:GetHeight(hillsTop2);
	local iHillsClumps = self.mountainsFrac:GetHeight(hillsClumps);
	local iHillsNearMountains = self.mountainsFrac:GetHeight(hillsNearMountains);
	local iMountainThreshold = self.mountainsFrac:GetHeight(mountains);
	local iPassThreshold = self.hillsFrac:GetHeight(hillsNearMountains);
	
	-- Set Hills and Mountains
	for x = 0, self.iNumPlotsX - 1 do
		for y = 0, self.iNumPlotsY - 1 do
			local plot = Map.GetPlot(x, y);
			local mountainVal = self.mountainsFrac:GetHeight(x, y);
			local hillVal = self.hillsFrac:GetHeight(x, y);
	
			if plot:GetPlotType() ~= PlotTypes.PLOT_OCEAN then
				if (mountainVal >= iMountainThreshold) then
					if (hillVal >= iPassThreshold) then -- Mountain Pass though the ridgeline
						plot:SetPlotType(PlotTypes.PLOT_HILLS, false, false);
					else -- Mountain
						plot:SetPlotType(PlotTypes.PLOT_MOUNTAIN, false, false);
					end
				elseif (mountainVal >= iHillsNearMountains) then
					plot:SetPlotType(PlotTypes.PLOT_HILLS, false, false);
				elseif ((hillVal >= iHillsBottom1 and hillVal <= iHillsTop1) or (hillVal >= iHillsBottom2 and hillVal <= iHillsTop2)) then
					plot:SetPlotType(PlotTypes.PLOT_HILLS, false, false);
				end
			end
		end
	end
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Generating Plot Types (Lua Frontier) ...");
	
	-- Obtain landmass type selected by user.
	userInputLandmass = Map.GetCustomOption(6)
	if userInputLandmass == 3 then
		userInputLandmass = 1 + Map.Rand(2, "Random Landmass Type - Lua");
	end

	-- Implement landmass type.
	if userInputLandmass == 2 then -- Continents
		local fractal_world = ContinentsFractalWorld.Create();
		fractal_world:GeneratePlotTypes();
	
		GenerateCoasts();
		
	else -- Pangaea
		local fractal_world = PangaeaFractalWorld.Create();
		local plotTypes = fractal_world:GeneratePlotTypes();
	
		SetPlotTypes(plotTypes);
		GenerateCoasts();

	end
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function GenerateTerrain()
	print("Generating Terrain (Lua Frontier) ...");
	
	-- Get Temperature setting input by user.
	local temp = Map.GetCustomOption(2)
	if temp == 4 then
		temp = 1 + Map.Rand(3, "Random Temperature - Lua");
	end

	local args = {temperature = temp};
	local terraingen = TerrainGenerator.Create(args);

	terrainTypes = terraingen:GenerateTerrain();
	
	SetTerrainTypes(terrainTypes);
end
------------------------------------------------------------------------------
function AddFeatures()
	print("Adding Features (Lua Frontier) ...");

	-- Get Rainfall setting input by user.
	local rain = Map.GetCustomOption(3)
	if rain == 4 then
		rain = 1 + Map.Rand(3, "Random Rainfall - Lua");
	end
	
	local args = {rainfall = rain}
	local featuregen = FeatureGenerator.Create(args);

	-- False parameter removes mountains from coastlines.
	featuregen:AddFeatures(false);
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- Start Plot System - Needs numerous overwrites to make this concept work.
------------------------------------------------------------------------------
function AssignStartingPlots:GenerateRegions(args)
	print("Map Generation - Dividing the map in to Regions");
	local args = args or {};
	local iW, iH = Map.GetGridSize();
	self.method = RegionDivision.CONTINENTAL; -- Continental method for Frontier.
	args.resources = args.resources or 2;
	
	-- Custom map resource settings (Communitu_79a support)
	if args.resources == 6 then
		args.resources = 1 + Map.Rand(3, "Resource setting - AssignStartingPlots");
	end
	
	if args.resources < 4 and not args.comm then
		self.resDensity = args.resources;						-- Strategic Resource Density setting
		self.resSize = args.resources;							-- Strategic Resource Deposit Size setting
		self.bonusDensity = args.resources;						-- Bonus Resource Density setting
		self.luxuryDensity = args.resources;					-- Luxury Resource Density setting
	elseif args.comm then
		self.resDensity = args.resources;						-- Strategic Resource Density setting
		self.resSize = args.size;								-- Strategic Resource Deposit Size setting
		self.bonusDensity = args.bonus;							-- Bonus Resource Density setting
		self.luxuryDensity = args.lux;							-- Luxury Resource Density setting
	end
	self.legStart = args.legend or (args.resources == 4);		-- Legendary Start setting
	self.resBalance = args.balance or (args.resources == 5);	-- Strategic Balance setting

	print("-"); print("Resource settings");
	print("Strategic Density = ", self.resDensity);
	print("Strategic Size = ", self.resSize);
	print("Bonus Density = ", self.bonusDensity);
	print("Luxury Density = ", self.luxuryDensity);
	print("Legendary Start = ", self.legStart);
	print("Strategic Balance = ", self.resBalance);

	-- Determine number of civilizations and city states present in this game.
	self.iNumCivs, self.iNumCityStates, self.player_ID_list, self.bTeamGame, self.teams_with_major_civs, self.number_civs_per_team = GetPlayerAndTeamInfo()
	self.iNumCityStatesUnassigned = self.iNumCityStates;
	print("-"); print("Civs:", self.iNumCivs); print("City States:", self.iNumCityStates);

	-- Custom to Frontier
	local FrontierRegionMinCount = {1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 3, 2, 1, 0};
	local FrontierRegionMaxCount = {1, 1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0};
	local difference_amt = FrontierRegionMaxCount[self.iNumCivs] - FrontierRegionMinCount[self.iNumCivs];
	if difference_amt > 0 then
		local diceroll = Map.Rand(difference_amt + 1, "Number of Frontier Regions - Frontier LUA");
		self.iNumFrontiers = FrontierRegionMinCount[self.iNumCivs] + diceroll;
	else
		self.iNumFrontiers = FrontierRegionMinCount[self.iNumCivs];
	end
	print("Frontiers:", self.iNumFrontiers); print("-");

	-- Continental Method for use in Frontier.
	--[[ Loop through all plots on the map, measuring fertility of each land 
		 plot, identifying its AreaID, building a list of landmass AreaIDs, and
		 tallying the Start Placement Fertility for each landmass. ]]--

	-- region_data: [WestX, EastX, SouthY, NorthY, 
	-- numLandPlotsinRegion, numCoastalPlotsinRegion,
	-- numOceanPlotsinRegion, iRegionNetYield, 
	-- iNumLandAreas, iNumPlotsinRegion]
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
	for civToAssign = 1, self.iNumCivs + self.iNumFrontiers do
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
	
	-- Entry point for easier overrides.
	self:CustomOverride()
	
	--[[ Printout is for debugging only. Deactivate otherwise.
	local tempRegionData = self.regionData;
	for i, data in ipairs(tempRegionData) do
		print("-");
		print("Data for Start Region #", i);
		print("WestX:  ", data[1]);
		print("SouthY: ", data[2]);
		print("Width:  ", data[3]);
		print("Height: ", data[4]);
		print("AreaID: ", data[5]);
		print("Fertility:", data[6]);
		print("Plots:  ", data[7]);
		print("Fert/Plot:", data[8]);
		print("-");
	end
	--]]
end
------------------------------------------------------------------------------
function AssignStartingPlots:BalanceAndAssign()
	-- Customized for Frontier.

	-- Normalize each start plot location.
	local iNumStarts = table.maxn(self.startingPlots);
	for region_number = 1, iNumStarts do
		self:NormalizeStartLocation(region_number)
	end

	-- Check Game Option for disabling civ-specific biases.
	-- If they are to be disabled, then all civs are simply assigned to start plots at random.
	local bDisableStartBias = Game.GetCustomOption("GAMEOPTION_DISABLE_START_BIAS");
	if bDisableStartBias == 1 then
		--print("-"); print("ALERT: Civ Start Biases have been selected to be Disabled!"); print("-");
		local playerList = {};
		for loop = 1, self.iNumCivs do
			local player_ID = self.player_ID_list[loop];
			table.insert(playerList, player_ID);
		end
		--
		-- Frontier needs some empty regions. Using -1 for player ID to mark empties.
		if self.iNumFrontiers > 0 then
			for loop = 1, self.iNumFrontiers do
				table.insert(playerList, -1);
			end
		end
		--
		local playerListShuffled = GetShuffledCopyOfTable(playerList)
		for region_number, player_ID in ipairs(playerListShuffled) do
			if player_ID >= 0 then
				local x = self.startingPlots[region_number][1];
				local y = self.startingPlots[region_number][2];
				local start_plot = Map.GetPlot(x, y)
				local player = Players[player_ID]
				player:SetStartingPlot(start_plot)
			end
		end
		-- If this is a team game (any team has more than one Civ in it) then make 
		-- sure team members start near each other if possible. (This may scramble 
		-- Civ biases in some cases, but there is no cure).
		if self.bTeamGame == true then
			self:NormalizeTeamLocations()
		end
		-- Done with un-biased Civ placement.
		return
	end

	-- If the process reaches here, civ-specific start-location biases are enabled. Handle them now.
	-- Create a randomized list of all regions. As a region gets assigned, we'll remove it from the list.
	local all_regions = {};
	for loop = 1, self.iNumCivs + self.iNumFrontiers do
		table.insert(all_regions, loop);
	end
	local regions_still_available = GetShuffledCopyOfTable(all_regions)

	local civs_needing_coastal_start = {};
	local civs_priority_coastal_start = {};
	local civs_needing_river_start = {};
	local civs_needing_region_priority = {};
	local civs_needing_region_avoid = {};
	local civs_needing_mountain_start = {};
	local civs_needing_snow_start = {};
	local regions_with_coastal_start = {};
	local regions_with_lake_start = {};
	local regions_with_river_start = {};
	local regions_with_near_river_start = {};
	local civ_status = table.fill(false, GameDefines.MAX_MAJOR_CIVS); -- Have to account for possible gaps in player ID numbers, for MP.
	local region_status = table.fill(false, self.iNumCivs + self.iNumFrontiers);
	local priority_lists = {};
	local avoid_lists = {};
	local iNumCoastalCivs, iNumRiverCivs, iNumPriorityCivs, iNumAvoidCivs, iNumMountainCivs, iNumSnowCivs = 0, 0, 0, 0, 0, 0;
	local iNumCoastalCivsRemaining, iNumRiverCivsRemaining, iNumPriorityCivsRemaining, iNumAvoidCivsRemaining, iNumMountainCivsRemaining, iNumSnowCivsRemaining = 0, 0, 0, 0, 0, 0;
	
	--print("-"); print("-"); print("--- DEBUG READOUT OF PLAYER START ASSIGNMENTS ---"); print("-");
	
	-- Generate lists of player needs. Each additional need type is subordinate to those
	-- that come before. In other words, each Civ can have only one need type.
	for loop = 1, self.iNumCivs do
		local playerNum = self.player_ID_list[loop]; -- MP games can have gaps between player numbers, so we cannot assume a sequential set of IDs.
		local player = Players[playerNum];
		local civType = GameInfo.Civilizations[player:GetCivilizationType()].Type;
		print("Player", playerNum, "of Civ Type", civType);
		if CivNeedsMountainStart(civType) then
			print("- - - - - - - needs Mountain Start!"); print("-");
			iNumMountainCivs = iNumMountainCivs + 1;
			iNumMountainCivsRemaining = iNumMountainCivsRemaining + 1;
			table.insert(civs_needing_mountain_start, playerNum);
		elseif CivNeedsSnowStart(civType) then
			print("- - - - - - - needs Snow Start!"); print("-");
			iNumSnowCivs = iNumSnowCivs + 1;
			iNumSnowCivsRemaining = iNumSnowCivsRemaining + 1;
			table.insert(civs_needing_snow_start, playerNum);
		elseif CivNeedsCoastalStart(civType) then
			print("- - - - - - - needs Coastal Start!"); print("-");
			iNumCoastalCivs = iNumCoastalCivs + 1;
			iNumCoastalCivsRemaining = iNumCoastalCivsRemaining + 1;
			table.insert(civs_needing_coastal_start, playerNum);
			local bPlaceFirst = CivNeedsPlaceFirstCoastalStart(civType);
			if bPlaceFirst then
				print("- - - - - - - needs to Place First!"); print("-");
				table.insert(civs_priority_coastal_start, playerNum);
			end
		elseif CivNeedsRiverStart(civType) then
			print("- - - - - - - needs River Start!"); print("-");
			iNumRiverCivs = iNumRiverCivs + 1;
			iNumRiverCivsRemaining = iNumRiverCivsRemaining + 1;
			table.insert(civs_needing_river_start, playerNum);
		elseif GetNumStartRegionPriorityForCiv(civType) > 0 then
			print("- - - - - - - needs Region Priority!"); print("-");
			local table_of_this_civs_priority_needs = GetStartRegionPriorityListForCiv_GetIDs(civType)
			iNumPriorityCivs = iNumPriorityCivs + 1;
			iNumPriorityCivsRemaining = iNumPriorityCivsRemaining + 1;
			table.insert(civs_needing_region_priority, playerNum);
			priority_lists[playerNum] = table_of_this_civs_priority_needs;
		elseif GetNumStartRegionAvoidForCiv(civType) > 0 then
			print("- - - - - - - needs Region Avoid!"); print("-");
			local table_of_this_civs_avoid_needs = GetStartRegionAvoidListForCiv_GetIDs(civType)
			iNumAvoidCivs = iNumAvoidCivs + 1;
			iNumAvoidCivsRemaining = iNumAvoidCivsRemaining + 1;
			table.insert(civs_needing_region_avoid, playerNum);
			avoid_lists[playerNum] = table_of_this_civs_avoid_needs;
		end
	end
	--[[
	print("Civs with Coastal Bias:", iNumCoastalCivs);
	print("Civs with River Bias:", iNumRiverCivs);
	print("Civs with Region Priority:", iNumPriorityCivs);
	print("Civs with Region Avoid:", iNumAvoidCivs); print("-");
	]]--
	-- Handle Snow Bias (maximum one civ for now)
	if iNumSnowCivs > 0 then
		for region_number, bAlreadyAssigned in ipairs(region_status) do
			if not bAlreadyAssigned and self.regionTypes[region_number] == 10 then
				local iPlayerNum = civs_needing_snow_start[1];
				local x = self.startingPlots[region_number][1];
				local y = self.startingPlots[region_number][2];
				local plot = Map.GetPlot(x, y);
				local player = Players[iPlayerNum];
				player:SetStartingPlot(plot);
				print("Player Number", iPlayerNum, "with Snow bias assigned to Region#", region_number, "at Plot", x, y);
				iNumSnowCivsRemaining = iNumSnowCivsRemaining - 1;
				region_status[region_number] = true;
				civ_status[iPlayerNum + 1] = true;
				local a, b, c = IdentifyTableIndex(regions_still_available, region_number);
				if a then
					table.remove(regions_still_available, c[1]);
				end
			end
		end
	end

	-- Handle Mountain Bias (maximum one civ for now)
	if iNumMountainCivs > 0 then
		for region_number, bAlreadyAssigned in ipairs(region_status) do
			if not bAlreadyAssigned and self.regionTypes[region_number] == 9 then
				local iPlayerNum = civs_needing_mountain_start[1];
				local x = self.startingPlots[region_number][1];
				local y = self.startingPlots[region_number][2];
				local plot = Map.GetPlot(x, y);
				local player = Players[iPlayerNum];
				player:SetStartingPlot(plot);
				print("Player Number", iPlayerNum, "with Mountain bias assigned to Region#", region_number, "at Plot", x, y);
				iNumMountainCivsRemaining = iNumMountainCivsRemaining - 1;
				region_status[region_number] = true;
				civ_status[iPlayerNum + 1] = true;
				local a, b, c = IdentifyTableIndex(regions_still_available, region_number);
				if a then
					table.remove(regions_still_available, c[1]);
				end
			end
		end
	end

	-- Handle Coastal Start Bias
	if iNumCoastalCivs > 0 then
		-- Generate lists of regions eligible to support a coastal start.
		local iNumRegionsWithCoastalStart, iNumRegionsWithLakeStart, iNumUnassignableCoastStarts = 0, 0, 0;
		for region_number, bAlreadyAssigned in ipairs(region_status) do
			if bAlreadyAssigned == false then
				if self.startLocationConditions[region_number][1] == true then
					--print("Region#", region_number, "has a Coastal Start.");
					iNumRegionsWithCoastalStart = iNumRegionsWithCoastalStart + 1;
					table.insert(regions_with_coastal_start, region_number);
				end
			end
		end
		if iNumRegionsWithCoastalStart < iNumCoastalCivs then
			for region_number, bAlreadyAssigned in ipairs(region_status) do
				if bAlreadyAssigned == false then
					if self.startLocationConditions[region_number][2] == true and
					   self.startLocationConditions[region_number][1] == false then
						--print("Region#", region_number, "has a Lake Start.");
						iNumRegionsWithLakeStart = iNumRegionsWithLakeStart + 1;
						table.insert(regions_with_lake_start, region_number);
					end
				end
			end
		end
		if iNumRegionsWithCoastalStart + iNumRegionsWithLakeStart < iNumCoastalCivs then
			iNumUnassignableCoastStarts = iNumCoastalCivs - (iNumRegionsWithCoastalStart + iNumRegionsWithLakeStart);
		end
		-- Now assign those with coastal bias to start locations, where possible.
		--print("iNumCoastalCivs: " .. iNumCoastalCivs);
		--print("iNumUnassignableCoastStarts: " .. iNumUnassignableCoastStarts);
		if iNumCoastalCivs - iNumUnassignableCoastStarts > 0 then
			-- create non-priority coastal start list
			local non_priority_coastal_start = {};
			for loop1, iPlayerNum1 in ipairs(civs_needing_coastal_start) do
				local bAdd = true;
				for loop2, iPlayerNum2 in ipairs(civs_priority_coastal_start) do
					if (iPlayerNum1 == iPlayerNum2) then
						bAdd = false;
					end
				end
				if bAdd then
					table.insert(non_priority_coastal_start, iPlayerNum1);
				end
			end
			
			local shuffled_priority_coastal_start = GetShuffledCopyOfTable(civs_priority_coastal_start);
			local shuffled_non_priority_coastal_start = GetShuffledCopyOfTable(non_priority_coastal_start);
			local shuffled_coastal_civs = {};
			
			-- insert priority coastal starts first
			for loop, iPlayerNum in ipairs(shuffled_priority_coastal_start) do
				table.insert(shuffled_coastal_civs, iPlayerNum);
			end
			
			-- insert non-priority coastal starts second
			for loop, iPlayerNum in ipairs(shuffled_non_priority_coastal_start) do
				table.insert(shuffled_coastal_civs, iPlayerNum);
			end			
			--[[
			for loop, iPlayerNum in ipairs(shuffled_coastal_civs) do
				print("shuffled_coastal_civs[" .. loop .. "]: " .. iPlayerNum);
			end
			]]--
			local shuffled_coastal_regions, shuffled_lake_regions;
			local current_lake_index = 1;
			if iNumRegionsWithCoastalStart > 0 then
				shuffled_coastal_regions = GetShuffledCopyOfTable(regions_with_coastal_start);
			end
			if iNumRegionsWithLakeStart > 0 then
				shuffled_lake_regions = GetShuffledCopyOfTable(regions_with_lake_start);
			end
			for loop, playerNum in ipairs(shuffled_coastal_civs) do
				if loop > iNumCoastalCivs - iNumUnassignableCoastStarts then
					--print("Ran out of Coastal and Lake start locations to assign to Coastal Bias.");
					break
				end
				-- Assign next randomly chosen civ in line to next randomly chosen eligible region.
				if loop <= iNumRegionsWithCoastalStart then
					-- Assign this civ to a region with coastal start.
					local choose_this_region = shuffled_coastal_regions[loop];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[playerNum];
					player:SetStartingPlot(plot);
					--print("Player Number", playerNum, "assigned a COASTAL START BIAS location in Region#", choose_this_region, "at Plot", x, y);
					region_status[choose_this_region] = true;
					civ_status[playerNum + 1] = true;
					iNumCoastalCivsRemaining = iNumCoastalCivsRemaining - 1;
					local a, b, c = IdentifyTableIndex(civs_needing_coastal_start, playerNum)
					if a then
						table.remove(civs_needing_coastal_start, c[1]);
					end
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end
				else
					-- Out of coastal starts, assign this civ to region with lake start.
					local choose_this_region = shuffled_lake_regions[current_lake_index];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[playerNum];
					player:SetStartingPlot(plot);
					--print("Player Number", playerNum, "with Coastal Bias assigned a fallback Lake location in Region#", choose_this_region, "at Plot", x, y);
					region_status[choose_this_region] = true;
					civ_status[playerNum + 1] = true;
					iNumCoastalCivsRemaining = iNumCoastalCivsRemaining - 1;
					local a, b, c = IdentifyTableIndex(civs_needing_coastal_start, playerNum)
					if a then
						table.remove(civs_needing_coastal_start, c[1]);
					end
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end
					current_lake_index = current_lake_index + 1;
				end
			end
		--else
			--print("Either no civs required a Coastal Start, or no Coastal Starts were available.");
		end
	end
	
	-- Handle River bias
	if iNumRiverCivs > 0 or iNumCoastalCivsRemaining > 0 then
		-- Generate lists of regions eligible to support a river start.
		local iNumRegionsWithRiverStart, iNumRegionsNearRiverStart, iNumUnassignableRiverStarts = 0, 0, 0;
		for region_number, bAlreadyAssigned in ipairs(region_status) do
			if bAlreadyAssigned == false then
				if self.startLocationConditions[region_number][3] == true then
					iNumRegionsWithRiverStart = iNumRegionsWithRiverStart + 1;
					table.insert(regions_with_river_start, region_number);
				end
			end
		end
		for region_number, bAlreadyAssigned in ipairs(region_status) do
			if bAlreadyAssigned == false then
				if self.startLocationConditions[region_number][4] == true and
				   self.startLocationConditions[region_number][3] == false then
					iNumRegionsNearRiverStart = iNumRegionsNearRiverStart + 1;
					table.insert(regions_with_near_river_start, region_number);
				end
			end
		end
		if iNumRegionsWithRiverStart + iNumRegionsNearRiverStart < iNumRiverCivs then
			iNumUnassignableRiverStarts = iNumRiverCivs - (iNumRegionsWithRiverStart + iNumRegionsNearRiverStart);
		end
		-- Now assign those with river bias to start locations, where possible.
		-- Also handle fallback placement for coastal bias that failed to find a match.
		if iNumRiverCivs - iNumUnassignableRiverStarts > 0 then
			local shuffled_river_civs = GetShuffledCopyOfTable(civs_needing_river_start);
			local shuffled_river_regions, shuffled_near_river_regions;
			if iNumRegionsWithRiverStart > 0 then
				shuffled_river_regions = GetShuffledCopyOfTable(regions_with_river_start);
			end
			if iNumRegionsNearRiverStart > 0 then
				shuffled_near_river_regions = GetShuffledCopyOfTable(regions_with_near_river_start);
			end
			for loop, playerNum in ipairs(shuffled_river_civs) do
				if loop > iNumRiverCivs - iNumUnassignableRiverStarts then
					--print("Ran out of River and Near-River start locations to assign to River Bias.");
					break
				end
				-- Assign next randomly chosen civ in line to next randomly chosen eligible region.
				if loop <= iNumRegionsWithRiverStart then
					-- Assign this civ to a region with river start.
					local choose_this_region = shuffled_river_regions[loop];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[playerNum];
					player:SetStartingPlot(plot);
					--print("Player Number", playerNum, "assigned a RIVER START BIAS location in Region#", choose_this_region, "at Plot", x, y);
					region_status[choose_this_region] = true;
					civ_status[playerNum + 1] = true;
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end
				else
					-- Assign this civ to a region where a river is near the start.
					local choose_this_region = shuffled_near_river_regions[loop - iNumRegionsWithRiverStart];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[playerNum];
					player:SetStartingPlot(plot);
					--print("Player Number", playerNum, "with River Bias assigned a fallback 'near river' location in Region#", choose_this_region, "at Plot", x, y);
					region_status[choose_this_region] = true;
					civ_status[playerNum + 1] = true;
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end
				end
			end
		end
		-- Now handle any fallbacks for unassigned coastal bias.
		if iNumCoastalCivsRemaining > 0 and iNumRiverCivs < iNumRegionsWithRiverStart + iNumRegionsNearRiverStart then
			local iNumFallbacksWithRiverStart, iNumFallbacksNearRiverStart = 0, 0;
			local fallbacks_with_river_start, fallbacks_with_near_river_start = {}, {};
			for region_number, bAlreadyAssigned in ipairs(region_status) do
				if bAlreadyAssigned == false then
					if self.startLocationConditions[region_number][3] == true then
						iNumFallbacksWithRiverStart = iNumFallbacksWithRiverStart + 1;
						table.insert(fallbacks_with_river_start, region_number);
					end
				end
			end
			for region_number, bAlreadyAssigned in ipairs(region_status) do
				if bAlreadyAssigned == false then
					if self.startLocationConditions[region_number][4] == true and
					   self.startLocationConditions[region_number][3] == false then
						iNumFallbacksNearRiverStart = iNumFallbacksNearRiverStart + 1;
						table.insert(fallbacks_with_near_river_start, region_number);
					end
				end
			end
			if iNumFallbacksWithRiverStart + iNumFallbacksNearRiverStart > 0 then
			
				local shuffled_coastal_fallback_civs = GetShuffledCopyOfTable(civs_needing_coastal_start);
				local shuffled_river_fallbacks, shuffled_near_river_fallbacks;
				if iNumFallbacksWithRiverStart > 0 then
					shuffled_river_fallbacks = GetShuffledCopyOfTable(fallbacks_with_river_start);
				end
				if iNumFallbacksNearRiverStart > 0 then
					shuffled_near_river_fallbacks = GetShuffledCopyOfTable(fallbacks_with_near_river_start);
				end
				for loop, playerNum in ipairs(shuffled_coastal_fallback_civs) do
					if loop > iNumFallbacksWithRiverStart + iNumFallbacksNearRiverStart then
						--print("Ran out of River and Near-River start locations to assign as fallbacks for Coastal Bias.");
						break
					end
					-- Assign next randomly chosen civ in line to next randomly chosen eligible region.
					if loop <= iNumFallbacksWithRiverStart then
						-- Assign this civ to a region with river start.
						local choose_this_region = shuffled_river_fallbacks[loop];
						local x = self.startingPlots[choose_this_region][1];
						local y = self.startingPlots[choose_this_region][2];
						local plot = Map.GetPlot(x, y);
						local player = Players[playerNum];
						player:SetStartingPlot(plot);
						--print("Player Number", playerNum, "with Coastal Bias assigned a fallback river location in Region#", choose_this_region, "at Plot", x, y);
						region_status[choose_this_region] = true;
						civ_status[playerNum + 1] = true;
						local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
						if a then
							table.remove(regions_still_available, c[1]);
						end
					else
						-- Assign this civ to a region where a river is near the start.
						local choose_this_region = shuffled_near_river_fallbacks[loop - iNumRegionsWithRiverStart];
						local x = self.startingPlots[choose_this_region][1];
						local y = self.startingPlots[choose_this_region][2];
						local plot = Map.GetPlot(x, y);
						local player = Players[playerNum];
						player:SetStartingPlot(plot);
						--print("Player Number", playerNum, "with Coastal Bias assigned a fallback 'near river' location in Region#", choose_this_region, "at Plot", x, y);
						region_status[choose_this_region] = true;
						civ_status[playerNum + 1] = true;
						local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
						if a then
							table.remove(regions_still_available, c[1]);
						end
					end
				end
			end
		end
	end
	
	-- Handle Region Priority
	if iNumPriorityCivs > 0 then
		--print("-"); print("-"); print("--- REGION PRIORITY READOUT ---"); print("-");
		local iNumSinglePriority, iNumMultiPriority, iNumNeedFallbackPriority = 0, 0, 0;
		local single_priority, multi_priority, fallback_priority = {}, {}, {};
		local single_sorted, multi_sorted = {}, {};
		-- Separate priority civs in to two categories: single priority, multiple priority.
		for playerNum, priority_needs in pairs(priority_lists) do
			local len = table.maxn(priority_needs)
			if len == 1 then
				--print("Player#", playerNum, "has a single Region Priority of type", priority_needs[1]);
				local priority_data = {playerNum, priority_needs[1]};
				table.insert(single_priority, priority_data)
				iNumSinglePriority = iNumSinglePriority + 1;
			else
				--print("Player#", playerNum, "has multiple Region Priority, this many types:", len);
				local priority_data = {playerNum, len};
				table.insert(multi_priority, priority_data)
				iNumMultiPriority = iNumMultiPriority + 1;
			end
		end
		-- Single priority civs go first, and will engage fallback methods if no match found.
		if iNumSinglePriority > 0 then
			-- Sort the list so that proper order of execution occurs. (Going to use a blunt method for easy coding.)
			for region_type = 1, NUM_REGION_TYPES - 1 do
				for loop, data in ipairs(single_priority) do
					if data[2] == region_type then
						--print("Adding Player#", data[1], "to sorted list of single Region Priority.");
						table.insert(single_sorted, data);
					end
				end
			end
			-- Match civs who have a single Region Priority to the region type they need, if possible.
			for loop, data in ipairs(single_sorted) do
				local iPlayerNum = data[1];
				local iPriorityType = data[2];
				--print("* Attempting to assign Player#", iPlayerNum, "to a region of Type#", iPriorityType);
				local bFoundCandidate, candidate_regions = false, {};
				for test_loop, region_number in ipairs(regions_still_available) do
					if self.regionTypes[region_number] == iPriorityType then
						table.insert(candidate_regions, region_number);
						bFoundCandidate = true;
						--print("- - Found candidate: Region#", region_number);
					end
				end
				if bFoundCandidate then
					local diceroll = 1 + Map.Rand(table.maxn(candidate_regions), "Choosing from among Candidate Regions for start bias - LUA");
					local choose_this_region = candidate_regions[diceroll];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[iPlayerNum];
					player:SetStartingPlot(plot);
					--print("Player Number", iPlayerNum, "with single Region Priority assigned to Region#", choose_this_region, "at Plot", x, y);
					region_status[choose_this_region] = true;
					civ_status[iPlayerNum + 1] = true;
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end
				else
					table.insert(fallback_priority, data)
					iNumNeedFallbackPriority = iNumNeedFallbackPriority + 1;
					--print("Player Number", iPlayerNum, "with single Region Priority was UNABLE to be matched to its type. Added to fallback list.");
				end
			end
		end
		-- Multiple priority civs go next, with fewest regions of priority going first.
		if iNumMultiPriority > 0 then
			for iNumPriorities = 2, 8 do						-- Must expand if new region types are added.
				for loop, data in ipairs(multi_priority) do
					if data[2] == iNumPriorities then
						--print("Adding Player#", data[1], "to sorted list of multi Region Priority.");
						table.insert(multi_sorted, data);
					end
				end
			end
			-- Match civs who have mulitple Region Priority to one of the region types they need, if possible.
			for loop, data in ipairs(multi_sorted) do
				local iPlayerNum = data[1];
				local iNumPriorityTypes = data[2];
				--print("* Attempting to assign Player#", iPlayerNum, "to one of its Priority Region Types.");
				local bFoundCandidate, candidate_regions = false, {};
				for test_loop, region_number in ipairs(regions_still_available) do
					for inner_loop = 1, iNumPriorityTypes do
						local region_type_to_test = priority_lists[iPlayerNum][inner_loop];
						if self.regionTypes[region_number] == region_type_to_test then
							table.insert(candidate_regions, region_number);
							bFoundCandidate = true;
							--print("- - Found candidate: Region#", region_number);
						end
					end
				end
				if bFoundCandidate then
					local diceroll = 1 + Map.Rand(table.maxn(candidate_regions), "Choosing from among Candidate Regions for start bias - LUA");
					local choose_this_region = candidate_regions[diceroll];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[iPlayerNum];
					player:SetStartingPlot(plot);
					--print("Player Number", iPlayerNum, "with multiple Region Priority assigned to Region#", choose_this_region, "at Plot", x, y);
					region_status[choose_this_region] = true;
					civ_status[iPlayerNum + 1] = true;
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end
				--else
					--print("Player Number", iPlayerNum, "with multiple Region Priority was unable to be matched.");
				end
			end
		end
		-- Fallbacks are done (if needed) after multiple-region priority is handled. The list is pre-sorted.
		if iNumNeedFallbackPriority > 0 then
			for loop, data in ipairs(fallback_priority) do
				local iPlayerNum = data[1];
				local iPriorityType = data[2];
				--print("* Attempting to assign Player#", iPlayerNum, "to a fallback region as similar as possible to Region Type#", iPriorityType);
				local choose_this_region = self:FindFallbackForUnmatchedRegionPriority(iPriorityType, regions_still_available)
				if choose_this_region == -1 then
					--print("FAILED to find fallback region bias for player#", iPlayerNum);
				else
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[iPlayerNum];
					player:SetStartingPlot(plot);
					--print("Player Number", iPlayerNum, "with single Region Priority assigned to FALLBACK Region#", choose_this_region, "at Plot", x, y);
					region_status[choose_this_region] = true;
					civ_status[iPlayerNum + 1] = true;
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end
				end
			end
		end
	end
	
	-- Handle Region Avoid
	if iNumAvoidCivs > 0 then
		--print("-"); print("-"); print("--- REGION AVOID READOUT ---"); print("-");
		local avoid_sorted, avoid_unsorted, avoid_counts = {}, {}, {};
		-- Sort list of civs with Avoid needs, then process in reverse order, so most needs goes first.
		for playerNum, avoid_needs in pairs(avoid_lists) do
			local len = table.maxn(avoid_needs)
			--print("- Player#", playerNum, "has this number of Region Avoid needs:", len);
			local avoid_data = {playerNum, len};
			table.insert(avoid_unsorted, avoid_data)
			table.insert(avoid_counts, len)
		end
		table.sort(avoid_counts)
		for loop, avoid_count in ipairs(avoid_counts) do
			for test_loop, avoid_data in ipairs(avoid_unsorted) do
				if avoid_count == avoid_data[2] then
					table.insert(avoid_sorted, avoid_data[1])
					table.remove(avoid_unsorted, test_loop)
				end
			end
		end
		-- Process the Region Avoid needs.
		for loop = iNumAvoidCivs, 1, -1 do
			local iPlayerNum = avoid_sorted[loop];
			local candidate_regions = {};
			for test_loop, region_number in ipairs(regions_still_available) do
				local bFoundCandidate = true;
				for inner_loop, region_type_to_avoid in ipairs(avoid_lists[iPlayerNum]) do
					if self.regionTypes[region_number] == region_type_to_avoid then
						bFoundCandidate = false;
					end
				end
				if bFoundCandidate == true then
					table.insert(candidate_regions, region_number);
					--print("- - Found candidate: Region#", region_number)
				end
			end
			if table.maxn(candidate_regions) > 0 then
				local diceroll = 1 + Map.Rand(table.maxn(candidate_regions), "Choosing from among Candidate Regions for start bias - LUA");
				local choose_this_region = candidate_regions[diceroll];
				local x = self.startingPlots[choose_this_region][1];
				local y = self.startingPlots[choose_this_region][2];
				local plot = Map.GetPlot(x, y);
				local player = Players[iPlayerNum];
				player:SetStartingPlot(plot);
				--print("Player Number", iPlayerNum, "with Region Avoid assigned to allowed region type in Region#", choose_this_region, "at Plot", x, y);
				region_status[choose_this_region] = true;
				civ_status[iPlayerNum + 1] = true;
				local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
				if a then
					table.remove(regions_still_available, c[1]);
				end
			--else
				--print("Player Number", iPlayerNum, "with Region Avoid was unable to avoid the undesired region types.");
			end
		end
	end

	-- Assign remaining civs to start plots. Customized for Frontier.
	local playerList, regionList = {}, {};
	for loop = 1, self.iNumCivs do
		local player_ID = self.player_ID_list[loop];
		if civ_status[player_ID + 1] == false then -- Using C++ player ID, which starts at zero. Add 1 for Lua indexing.
			table.insert(playerList, player_ID);
		end
	end
	if self.iNumFrontiers > 0 then
		for loop = 1, self.iNumFrontiers do
			table.insert(playerList, -1);
		end
	end
	for loop = 1, self.iNumCivs + self.iNumFrontiers do
		if region_status[loop] == false then
			table.insert(regionList, loop);
		end
	end
	--
	-- Frontier customization.
	local iNumRemainingPlayers = table.maxn(playerList);
	local iNumRemainingRegions = table.maxn(regionList);
	
	--
	print("- - -");
	print("- Civ Count: ", self.iNumCivs);
	print("- Frontiers: ", self.iNumFrontiers);
	print("- Players Left: ", iNumRemainingPlayers);
	print("- Regions Left: ", iNumRemainingRegions);
	print("- - -");
	--
	
	if iNumRemainingPlayers > 0 or iNumRemainingRegions > 0 then
		--print("-"); print("Table of players with no start bias:");
		--PrintContentsOfTable(playerList);
		--print("-"); print("Table of regions still available after bias handling:");
		--PrintContentsOfTable(regionList);
		--[[
		if iNumRemainingPlayers ~= iNumRemainingRegions then
			print("-"); print("ERROR: Number of civs remaining after handling biases does not match number of regions remaining!"); print("-");
		end
		]]--
		local playerListShuffled = GetShuffledCopyOfTable(playerList)
		for index, player_ID in ipairs(playerListShuffled) do
			if player_ID >= 0 then
				local region_number = regionList[index];
				local x = self.startingPlots[region_number][1];
				local y = self.startingPlots[region_number][2];
				--print("Now placing Player#", player_ID, "in Region#", region_number, "at start plot:", x, y);
				local start_plot = Map.GetPlot(x, y)
				local player = Players[player_ID]
				player:SetStartingPlot(start_plot)
			end
		end
	end

	-- If this is a team game (any team has more than one Civ in it) then make 
	-- sure team members start near each other if possible. (This may scramble 
	-- Civ biases in some cases, but there is no cure).
	if self.bTeamGame == true then
		self:NormalizeTeamLocations()
	end
	--	
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetWorldLuxuryTargetNumbers()
	-- This data was separated out to allow easy replacement in map scripts.
	--
	-- The first number is the target for total luxuries in the world, NOT
	-- counting the one-per-civ "second type" added at start locations.
	--
	-- The second number affects minimum number of random luxuries placed.
	-- I say "affects" because it is only one part of the formula.
	local worldsizes = {};

	if self.luxuryDensity == 4 then
		self.luxuryDensity = 1 + Map.Rand(3, "Luxury Resource Density");
	end

	if self.luxuryDensity == 1 then -- Sparse
		worldsizes = {
			[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {17, 3},
			[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {30, 4},
			[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {42, 4},
			[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {55, 5},
			[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {70, 5},
			[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {85, 6}
		}
	elseif self.luxuryDensity == 3 then -- Abundant
		worldsizes = {
			[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {30, 3},
			[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {48, 4},
			[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {68, 4},
			[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {90, 5},
			[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {120, 5},
			[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {150, 6}
		}
	else -- Standard
		worldsizes = {
			[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {22, 3},
			[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {38, 4},
			[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {52, 4},
			[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {70, 5},
			[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {88, 5},
			[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {110, 6}
		}
	end
	local world_size_data = worldsizes[Map.GetWorldSize()];
	return world_size_data
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceLuxuries()
	-- Customized for Frontier.
	local iW, iH = Map.GetGridSize();
	table.fill(self.region_low_fert_compensation, 0, self.iNumCivs + self.iNumFrontiers); --
	-- Place Luxuries at civ start locations.
	for loop, reg_data in ipairs(self.regions_sorted_by_type) do
		local region_number = reg_data[1];
		local this_region_luxury = reg_data[2];
		local x = self.startingPlots[region_number][1];
		local y = self.startingPlots[region_number][2];
		--print("-"); print("Attempting to place Luxury#", this_region_luxury, "at start plot", x, y, "in Region#", region_number);
		-- Determine number to place at the start location
		local iNumToPlace = 2;	-- MOD.Barathor: Updated -- original = 1 -- Most times, 2 of the initial type are placed at the start anyway, because of the old fertility checks below.  This will make it consistent.
		if self.legStart then -- Legendary Start
			iNumToPlace = 3;	-- MOD.Barathor: Updated -- original = 2
		end
		if self.regionData[region_number][8] < 2.5 then -- Low fertility per region rectangle plot, add a lux.
			--print("-"); print("Region#", region_number, "has low rectangle fertility, giving it an extra Luxury at start plot.");
			iNumToPlace = iNumToPlace + 1;
			self.luxury_low_fert_compensation[this_region_luxury] = self.luxury_low_fert_compensation[this_region_luxury] + 1;
			self.region_low_fert_compensation[region_number] = self.region_low_fert_compensation[region_number] + 1;
		end
		if self.regionData[region_number][6] / self.regionTerrainCounts[region_number][2] < 4 then -- Low fertility per land plot.
			--print("-"); print("Region#", region_number, "has low per-plot fertility, giving it an extra Luxury at start plot.");
			iNumToPlace = iNumToPlace + 1;
			self.luxury_low_fert_compensation[this_region_luxury] = self.luxury_low_fert_compensation[this_region_luxury] + 1;
			self.region_low_fert_compensation[region_number] = self.region_low_fert_compensation[region_number] + 1;
		end
		-- Obtain plot lists appropriate to this luxury type.
		local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, shuf_list;					-- MOD.Barathor: New -- added a quinary and senary list
		primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(this_region_luxury);	-- MOD.Barathor: New -- added a quinary and senary list

		-- First pass, checking only first two rings with a 50% ratio.
		luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false)
		shuf_list = luxury_plot_lists[primary];
		local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumToPlace, 0.5, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
		if iNumLeftToPlace > 0 and secondary > 0 then
			shuf_list = luxury_plot_lists[secondary];
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
		end
		if iNumLeftToPlace > 0 and tertiary > 0 then
			shuf_list = luxury_plot_lists[tertiary];
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
		end
		if iNumLeftToPlace > 0 and quaternary > 0 then
			shuf_list = luxury_plot_lists[quaternary];
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
		end
		if iNumLeftToPlace > 0 and quinary > 0 then		-- MOD.Barathor: New -- added a quinary list
			shuf_list = luxury_plot_lists[quinary];
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
		end
		if iNumLeftToPlace > 0 and senary > 0 then		-- MOD.Barathor: New -- added a senary list
			shuf_list = luxury_plot_lists[senary];
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
		end

		if iNumLeftToPlace > 0 then
			-- Second pass, checking three rings with a 100% ratio.
			luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 3, false)
			shuf_list = luxury_plot_lists[primary];
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
			if iNumLeftToPlace > 0 and secondary > 0 then
				shuf_list = luxury_plot_lists[secondary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and tertiary > 0 then
				shuf_list = luxury_plot_lists[tertiary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and quaternary > 0 then
				shuf_list = luxury_plot_lists[quaternary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and quinary > 0 then		-- MOD.Barathor: New -- added a quinary list
				shuf_list = luxury_plot_lists[quinary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and senary > 0 then		-- MOD.Barathor: New -- added a senary list
				shuf_list = luxury_plot_lists[senary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
			end
		end

		if iNumLeftToPlace > 0 then
			-- If we haven't been able to place all of this lux type at the start, it CAN be placed
			-- in the region somewhere. Subtract remainder from this region's compensation, so that the
			-- regional process, later, will attempt to place this remainder somewhere in the region.
			self.luxury_low_fert_compensation[this_region_luxury] = self.luxury_low_fert_compensation[this_region_luxury] - iNumLeftToPlace;
			self.region_low_fert_compensation[region_number] = self.region_low_fert_compensation[region_number] - iNumLeftToPlace;
		end
		if iNumLeftToPlace > 0 and self.iNumTypesRandom > 0 then
			-- We'll attempt to place one source of a Luxury type assigned to random distribution.
			local randoms_to_place = 1;
			for loop, random_res in ipairs(self.resourceIDs_assigned_to_random) do
				primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(random_res);	-- MOD.Barathor: New -- added a quinary and senary list
				if randoms_to_place > 0 then
					shuf_list = luxury_plot_lists[primary];
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and secondary > 0 then
					shuf_list = luxury_plot_lists[secondary];
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and tertiary > 0 then
					shuf_list = luxury_plot_lists[tertiary];
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and quaternary > 0 then
					shuf_list = luxury_plot_lists[quaternary];
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and quinary > 0 then	-- MOD.Barathor: New -- added a quinary list
					shuf_list = luxury_plot_lists[quinary];
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and senary > 0 then	-- MOD.Barathor: New -- added a senary list
					shuf_list = luxury_plot_lists[senary];
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
				end
			end
		end
	end
	
	-- Place Luxuries at City States.
	-- Candidates include luxuries exclusive to CS, the lux assigned to this CS's region (if in a region), and the randoms.
	for city_state = 1, self.iNumCityStates do
		-- First check to see if this city state number received a valid start plot.
		if self.city_state_validity_table[city_state] == false then
			-- This one did not! It does not exist on the map nor have valid data, so we will ignore it.
		else
			-- OK, it's a valid city state. Process it.
			local region_number = self.city_state_region_assignments[city_state];
			local x = self.cityStatePlots[city_state][1];
			local y = self.cityStatePlots[city_state][2];
			local allowed_luxuries = self:GetListOfAllowableLuxuriesAtCitySite(x, y, 2)
			local lux_possible_for_cs = {}; -- Recorded with ID as key, weighting as data entry
			-- Identify Allowable Luxuries assigned to City States.
			-- If any CS-Only types are eligible, then all combined will have a weighting of 75%
			local cs_only_types = {};
			for loop, res_ID in ipairs(self.resourceIDs_assigned_to_cs) do
				if allowed_luxuries[res_ID] == true then
					table.insert(cs_only_types, res_ID);
				end
			end
			local iNumCSAllowed = table.maxn(cs_only_types);
			if iNumCSAllowed > 0 then
				for loop, res_ID in ipairs(cs_only_types) do
					lux_possible_for_cs[res_ID] = 75 / iNumCSAllowed;
				end
			end
			-- Identify Allowable Random Luxuries and the Regional Luxury if any.
			-- If any random types are eligible (plus the regional type if in a region) these combined carry a 25% weighting.
			if self.iNumTypesRandom > 0 or region_number > 0 then
				local random_types_allowed = {};
				for loop, res_ID in ipairs(self.resourceIDs_assigned_to_random) do
					if allowed_luxuries[res_ID] == true then
						table.insert(random_types_allowed, res_ID);
					end
				end
				local iNumRandAllowed = table.maxn(random_types_allowed);
				local iNumAllowed = iNumRandAllowed;
				if region_number > 0 then
					iNumAllowed = iNumAllowed + 1; -- Adding the region type in to the mix with the random types.
					local res_ID = self.region_luxury_assignment[region_number];
					if allowed_luxuries[res_ID] == true then
						lux_possible_for_cs[res_ID] = 25 / iNumAllowed;
					end
				end
				if iNumRandAllowed > 0 then
					for loop, res_ID in ipairs(random_types_allowed) do
						lux_possible_for_cs[res_ID] = 25 / iNumAllowed;
					end
				end
			end

			-- If there are no allowable luxury types at this city site, then this city state gets none.
			local iNumAvailableTypes = table.maxn(lux_possible_for_cs);
			if iNumAvailableTypes == 0 then
				--print("City State #", city_state, "has poor land, ineligible to receive a Luxury resource.");
			else
				-- Calculate probability thresholds for each allowable luxury type.
				local res_threshold = {};
				local totalWeight, accumulatedWeight = 0, 0;
				for res_ID, this_weight in pairs(lux_possible_for_cs) do
					totalWeight = totalWeight + this_weight;
				end
				for res_ID, this_weight in pairs(lux_possible_for_cs) do
					local threshold = (this_weight + accumulatedWeight) * 10000 / totalWeight;
					res_threshold[res_ID] = threshold;
					accumulatedWeight = accumulatedWeight + this_weight;
				end
				-- Choose luxury type.
				local use_this_ID;
				local diceroll = Map.Rand(10000, "Choose resource type - Assign Luxury To City State - Lua");
				for res_ID, threshold in pairs(res_threshold) do
					if diceroll < threshold then -- Choose this resource type.
						use_this_ID = res_ID;
						break
					end
				end
				--print("-"); print("-"); print("-Assigned Luxury Type", use_this_ID, "to City State#", city_state);
				-- Place luxury.
				local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, shuf_list;			-- MOD.Barathor: New -- added a quinary and senary list
				primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(use_this_ID);	-- MOD.Barathor: New -- added a quinary and senary list
				luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false)
				shuf_list = luxury_plot_lists[primary];
				local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
				if iNumLeftToPlace > 0 and secondary > 0 then
					shuf_list = luxury_plot_lists[secondary];
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and tertiary > 0 then
					shuf_list = luxury_plot_lists[tertiary];
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quaternary > 0 then
					shuf_list = luxury_plot_lists[quaternary];
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quinary > 0 then		-- MOD.Barathor: New -- added a quinary list
					shuf_list = luxury_plot_lists[quinary];
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and senary > 0 then		-- MOD.Barathor: New -- added a senary list
					shuf_list = luxury_plot_lists[senary];
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
				end
				if iNumLeftToPlace == 0 then
					--print("-"); print("Placed Luxury ID#", use_this_ID, "at City State#", city_state, "in Region#", region_number, "located at Plot", x, y);
				end
			end
		end
	end
		
	-- Place Regional Luxuries
	for region_number, res_ID in ipairs(self.region_luxury_assignment) do
		--print("-"); print("- - -"); print("Attempting to place regional luxury #", res_ID, "in Region#", region_number);
		local iNumAlreadyPlaced = self.amounts_of_resources_placed[res_ID + 1];
		local assignment_split = self.luxury_assignment_count[res_ID];
		local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, shuf_list, iNumLeftToPlace;		-- MOD.Barathor: New -- added a quinary and senary list
		primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(res_ID);					-- MOD.Barathor: New -- added a quinary and senary list
		luxury_plot_lists = self:GenerateLuxuryPlotListsInRegion(region_number)

		-- Calibrate number of luxuries per region to world size and number of civs
		-- present. The amount of lux per region should be at its highest when the 
		-- number of civs in the game is closest to "default" for that map size.
		local target_list = self:GetRegionLuxuryTargetNumbers()
		local targetNum = math.floor((target_list[self.iNumCivs] + (0.5 * self.luxury_low_fert_compensation[res_ID])) / assignment_split);
		targetNum = targetNum - self.region_low_fert_compensation[region_number];
		-- Adjust target number according to Resource Setting.
		if self.luxuryDensity == 1 then
			targetNum = targetNum - 2;
		elseif self.luxuryDensity == 3 then
			targetNum = targetNum + 2;
		elseif self.luxuryDensity == 4 then
			targetNum = targetNum - 2 + Map.Rand(5, "Luxury Resource Density");
		end
		local iNumThisLuxToPlace = math.max(1, targetNum); -- Always place at least one.

		--print("-"); print("Target number for Luxury#", res_ID, "with assignment split of", assignment_split, "is", targetNum);
		
		-- Place luxuries.
		shuf_list = luxury_plot_lists[primary];
		iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumThisLuxToPlace, 0.25, ImpactLayers.LAYER_LUXURY, 1, 3, shuf_list);	-- MOD.Barathor: Updated -- Existing ratio = 0.3, min radius = 0, max radius = 3
		if iNumLeftToPlace > 0 and secondary > 0 then
			shuf_list = luxury_plot_lists[secondary];
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, ImpactLayers.LAYER_LUXURY, 1, 3, shuf_list);	-- MOD.Barathor: Updated -- Existing ratio = 0.3, min radius = 0, max radius = 3
		end
		if iNumLeftToPlace > 0 and tertiary > 0 then
			shuf_list = luxury_plot_lists[tertiary];
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, ImpactLayers.LAYER_LUXURY, 1, 3, shuf_list);	-- MOD.Barathor: Updated -- Existing ratio = 0.4, min radius = 0, max radius = 2
		end
		if iNumLeftToPlace > 0 and quaternary > 0 then
			shuf_list = luxury_plot_lists[quaternary];
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, ImpactLayers.LAYER_LUXURY, 1, 3, shuf_list);	-- MOD.Barathor: Updated -- Existing ratio = 0.5, min radius = 0, max radius = 2 
		end
		if iNumLeftToPlace > 0 and quinary > 0 then		-- MOD.Barathor: New -- added a quinary list
			shuf_list = luxury_plot_lists[quinary];
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, ImpactLayers.LAYER_LUXURY, 1, 3, shuf_list);	-- MOD.Barathor: Updated -- Existing ratio = 0.5, min radius = 0, max radius = 2 
		end
		if iNumLeftToPlace > 0 and senary > 0 then		-- MOD.Barathor: New -- added a senary list
			shuf_list = luxury_plot_lists[senary];
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, ImpactLayers.LAYER_LUXURY, 1, 3, shuf_list);	-- MOD.Barathor: Updated -- Existing ratio = 0.5, min radius = 0, max radius = 2 
		end
		--print("-"); print("-"); print("Number of LuxuryID", res_ID, "left to place in Region#", region_number, "is", iNumLeftToPlace);
		
		-- MOD.Barathor: New -- New Fallback loop since some luxuries frequently miss the target total during regionl placement (like silk, dyes, spices, etc.)
					-- This is because some luxuries have only a few indices and don't utilize all four, plus they have strict requirements, like foliage only and on flat land only
					-- When a luxury only uses two or three selective indices, it gets harder to place them all since the first default ratios above are only 0.3 (3 placements for every 10 eligible plots, rounded up)
		if iNumLeftToPlace > 0 then	
			-- Second pass, checking all with a 100% ratio to make sure the target total is reached for this region!
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, ImpactLayers.LAYER_LUXURY, 1, 2, shuf_list);
			if iNumLeftToPlace > 0 and secondary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, ImpactLayers.LAYER_LUXURY, 1, 2, shuf_list);
			end
			if iNumLeftToPlace > 0 and tertiary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, ImpactLayers.LAYER_LUXURY, 1, 2, shuf_list);
			end
			if iNumLeftToPlace > 0 and quaternary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, ImpactLayers.LAYER_LUXURY, 1, 2, shuf_list);
			end
			if iNumLeftToPlace > 0 and quinary > 0 then		-- MOD.Barathor: New -- added a quinary list
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, ImpactLayers.LAYER_LUXURY, 1, 2, shuf_list);
			end
			if iNumLeftToPlace > 0 and senary > 0 then		-- MOD.Barathor: New -- added a senary list
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, ImpactLayers.LAYER_LUXURY, 1, 2, shuf_list);
			end
			print("-"); print("Number of LuxuryID", res_ID, "not placed in Region#", region_number, "is", iNumLeftToPlace);
		end	
	end

	-- Place Random Luxuries
	if self.iNumTypesRandom > 0 then
		--print("* *"); print("* iNumTypesRandom = ", self.iNumTypesRandom); print("* *");
		-- This table governs targets for total number of luxuries placed in the world, not
		-- including the "extra types" of Luxuries placed at start locations. These targets
		-- are approximate. An additional random factor is added in based on number of civs.
		-- Any difference between regional and city state luxuries placed, and the target, is
		-- made up for with the number of randomly placed luxuries that get distributed.
		local world_size_data = self:GetWorldLuxuryTargetNumbers()
		-- This modifies self.luxuryDensity if random, to a value between 1 and 3
		-- Which is okay, since regional luxuries have been placed
		
		local targetLuxForThisWorldSize = world_size_data[1];
		local loopTarget = world_size_data[2];
		local extraLux = Map.Rand(self.iNumCivs, "Luxury Resource Variance - Place Resources LUA");
		local iNumRandomLuxTarget = targetLuxForThisWorldSize + extraLux - self.totalLuxPlacedSoFar;
		--print("* *"); print("* targetLuxForThisWorldSize = ", targetLuxForThisWorldSize); print("* *");	-- MOD.Barathor: Test
		--print("* *"); print("* random to add to target = ", extraLux); print("* *");					-- MOD.Barathor: Test
		--print("* *"); print("* totalLuxPlacedSoFar = ", self.totalLuxPlacedSoFar); print("* *");		-- MOD.Barathor: Test
		--print("* *"); print("* iNumRandomLuxTarget = ", iNumRandomLuxTarget); print("* *");				-- MOD.Barathor: Test
		local iNumRandomLuxPlaced, iNumThisLuxToPlace = 0, 0;
		-- This table weights the amount of random luxuries to place, with first-selected getting heavier weighting.
		local random_lux_ratios_table = {
		{1},
		{0.55, 0.45},
		{0.40, 0.33, 0.27},
		{0.35, 0.25, 0.25, 0.15},
		{0.25, 0.25, 0.20, 0.15, 0.15},
		{0.20, 0.20, 0.20, 0.15, 0.15, 0.10},
		{0.20, 0.20, 0.15, 0.15, 0.10, 0.10, 0.10},
		{0.20, 0.15, 0.15, 0.10, 0.10, 0.10, 0.10, 0.10} };

		for loop, res_ID in ipairs(self.resourceIDs_assigned_to_random) do
			local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, current_list, iNumLeftToPlace;	-- MOD.Barathor: New -- added a quinary and senary list
			primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(res_ID);					-- MOD.Barathor: New -- added a quinary and senary list
			if self.iNumTypesRandom > 8 then
				iNumThisLuxToPlace = math.max(3, math.ceil(iNumRandomLuxTarget / 10));
			else
				local lux_minimum = math.max(3, loopTarget - loop);
				local lux_share_of_remaining = math.ceil(iNumRandomLuxTarget * random_lux_ratios_table[self.iNumTypesRandom][loop]);
				iNumThisLuxToPlace = math.max(lux_minimum, lux_share_of_remaining);
			end
			-- Place this luxury type.
			current_list = self.global_luxury_plot_lists[primary];
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumThisLuxToPlace, 0.25, ImpactLayers.LAYER_LUXURY, 4, 6, current_list);
			if iNumLeftToPlace > 0 and secondary > 0 then
				current_list = self.global_luxury_plot_lists[secondary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, ImpactLayers.LAYER_LUXURY, 4, 6, current_list);
			end
			if iNumLeftToPlace > 0 and tertiary > 0 then
				current_list = self.global_luxury_plot_lists[tertiary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, ImpactLayers.LAYER_LUXURY, 4, 6, current_list);
			end
			if iNumLeftToPlace > 0 and quaternary > 0 then
				current_list = self.global_luxury_plot_lists[quaternary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, ImpactLayers.LAYER_LUXURY, 4, 6, current_list);
			end
			if iNumLeftToPlace > 0 and quinary > 0 then		-- MOD.Barathor: New -- added a quinary list
				current_list = self.global_luxury_plot_lists[quinary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, ImpactLayers.LAYER_LUXURY, 4, 6, current_list);
			end
			if iNumLeftToPlace > 0 and senary > 0 then		-- MOD.Barathor: New -- added a senary list
				current_list = self.global_luxury_plot_lists[senary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, ImpactLayers.LAYER_LUXURY, 4, 6, current_list);
			end
			iNumRandomLuxPlaced = iNumRandomLuxPlaced + iNumThisLuxToPlace - iNumLeftToPlace;
			--print("-"); 
			--print("Random Luxury ID#:", res_ID);	-- MOD.Barathor: Test
			--print("-"); print("Random Luxury Target Number:", iNumThisLuxToPlace);
			--print("Random Luxury Target Placed:", iNumThisLuxToPlace - iNumLeftToPlace); print("-");
		end
		--print("-"); print("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+");
		--print("+ Random Luxuries Target Number:", iNumRandomLuxTarget);
		--print("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+");
		--print("+ Random Luxuries Number Placed:", iNumRandomLuxPlaced);
		--print("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+"); print("-");
	end

	-- For Resource settings other than Sparse, add a second luxury type at start locations.
	-- This second type will be selected from Random types if possible, CS types if necessary, and other regions' types as a final fallback.
	-- Marble is included in the types possible to be placed.
	if self.luxuryDensity ~= 1 then
		for region_number = 1, self.iNumCivs do
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];
			local use_this_ID;
			local candidate_types, iNumTypesAllowed = {}, 0;
			local allowed_luxuries = self:GetListOfAllowableLuxuriesAtCitySite(x, y, 2)
			--print("-"); print("--- Eligible Types List for Second Luxury in Region#", region_number, "---");
			-- See if any Random types are eligible.
			for loop, res_ID in ipairs(self.resourceIDs_assigned_to_random) do
				if allowed_luxuries[res_ID] == true then
					--print("- Found eligible luxury type:", res_ID);
					iNumTypesAllowed = iNumTypesAllowed + 1;
					table.insert(candidate_types, res_ID);
				end
			end
			-- Check to see if any Special Case luxuries are eligible.
			for loop, res_ID in ipairs(self.resourceIDs_assigned_to_special_case) do
				if allowed_luxuries[res_ID] == true then
					--print("- Found eligible luxury type:", res_ID);
					iNumTypesAllowed = iNumTypesAllowed + 1;
					table.insert(candidate_types, res_ID);
				end
			end
		
			if iNumTypesAllowed > 0 then
				local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
				use_this_ID = candidate_types[diceroll];
			else
				-- See if any City State types are eligible.
				for loop, res_ID in ipairs(self.resourceIDs_assigned_to_cs) do
					if allowed_luxuries[res_ID] == true then
						--print("- Found eligible luxury type:", res_ID);
						iNumTypesAllowed = iNumTypesAllowed + 1;
						table.insert(candidate_types, res_ID);
					end
				end
				if iNumTypesAllowed > 0 then
					local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
					use_this_ID = candidate_types[diceroll];
				else
					-- See if anybody else's regional type is eligible.
					local region_lux_ID = self.region_luxury_assignment[region_number];
					for loop, res_ID in ipairs(self.resourceIDs_assigned_to_regions) do
						if res_ID ~= region_lux_ID then
							if allowed_luxuries[res_ID] == true then
								--print("- Found eligible luxury type:", res_ID);
								iNumTypesAllowed = iNumTypesAllowed + 1;
								table.insert(candidate_types, res_ID);
							end
						end
					end
					if iNumTypesAllowed > 0 then
						local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
						use_this_ID = candidate_types[diceroll];
					else
						--print("-"); print("Failed to place second Luxury type at start in Region#", region_number, "-- no eligible types!"); print("-");
					end
				end
			end
			--print("--- End of Eligible Types list for Second Luxury in Region#", region_number, "---");
			if use_this_ID ~= nil then -- Place this luxury type at this start.
				local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, shuf_list;			-- MOD.Barathor: New -- added a quinary and senary list
				primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(use_this_ID);	-- MOD.Barathor: New -- added a quinary and senary list
				luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false)
				shuf_list = luxury_plot_lists[primary];
				local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
				if iNumLeftToPlace > 0 and secondary > 0 then
					shuf_list = luxury_plot_lists[secondary];
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and tertiary > 0 then
					shuf_list = luxury_plot_lists[tertiary];
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quaternary > 0 then
					shuf_list = luxury_plot_lists[quaternary];
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quinary > 0 then		-- MOD.Barathor: New -- added a quinary list
					shuf_list = luxury_plot_lists[quinary];
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and senary > 0 then		-- MOD.Barathor: New -- added a senary list
					shuf_list = luxury_plot_lists[senary];
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, ImpactLayers.LAYER_NONE, 0, 0, shuf_list);
				end
				if iNumLeftToPlace == 0 then
					--print("-"); print("Placed Second Luxury type of ID#", use_this_ID, "for start located at Plot", x, y, " in Region#", region_number);
				end
			end
		end
	end

	-- Handle Special Case Luxuries
	if self.iNumTypesSpecialCase > 0 then
		-- Add a special case function for each luxury to be handled as a special case.
		self:PlaceMarble()
	end

	self.realtotalLuxPlacedSoFar = self.totalLuxPlacedSoFar		-- MOD.Barathor: New -- save the real total of luxuries before it gets corrupted with non-luxury additions which use the luxury placement method
end
------------------------------------------------------------------------------
function StartPlotSystem()
	-- Get Resources setting input by user.
	local res = Map.GetCustomOption(5)
	if res == 6 then
		res = 1 + Map.Rand(3, "Random Resources Option - Lua");
	end

	print("Creating start plot database.");
	local start_plot_database = AssignStartingPlots.Create()
	
	print("Dividing the map in to Regions.");
	-- Regional Division Method 1: Biggest Landmass
	local args = {resources = res};
	start_plot_database:GenerateRegions(args)

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
