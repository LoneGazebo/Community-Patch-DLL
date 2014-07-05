------------------------------------------------------------------------------
--	FILE:               FeatureGenerator.lua
--	MODIFIED FOR CIV5:  Bob Thomas
--	PYTHON TO LUA:      Shaun Seckman
--	PURPOSE:            Default method for feature generation
------------------------------------------------------------------------------
--	Copyright (c) 2009, 2010 Firaxis Games, Inc. All rights reserved.
------------------------------------------------------------------------------

include("MapmakerUtilities");

----------------------------------------------------------------
function IsBetween(lower, mid, upper)
	return ((lower <= mid) and (mid <= upper))
end

function Constrain(lower, mid, upper)
	return math.max(lower, math.min(mid, upper))
end
----------------------------------------------------------------

function Plot_GetPlotsInCircle(plot, minR, maxR)
	--[[ Plot_GetPlotsInCircle(plot, minR, [maxR]) usage examples:
	for nearPlot in Plot_GetPlotsInCircle(plot, 0, 4) do
		-- check all plots in radius 0-4
		nearPlot:GetX()
	end
	for nearPlot, distance in Plot_GetPlotsInCircle(plot, 3) do
		-- check all plots in radius 1-3
		if distance == 3 then
			print(nearPlot:GetX())
		end
	end
	]]
	if not plot then
		print("plot:GetPlotsInCircle plot=nil")
		return
	end
	if not maxR then
		maxR = minR
		minR = 1
	end
	
	local mapW, mapH	= Map.GetGridSize()
	local isWrapX		= Map:IsWrapX()
	local isWrapY		= Map:IsWrapY()
	local centerX		= plot:GetX()
	local centerY		= plot:GetY()
	
	leftX	= isWrapX and ((centerX-maxR) % mapW) or Game.Constrain(0, centerX-maxR, mapW-1)
	rightX	= isWrapX and ((centerX+maxR) % mapW) or Game.Constrain(0, centerX+maxR, mapW-1)
	bottomY	= isWrapY and ((centerY-maxR) % mapH) or Game.Constrain(0, centerY-maxR, mapH-1)
	topY	= isWrapY and ((centerY+maxR) % mapH) or Game.Constrain(0, centerY+maxR, mapH-1)
	
	local nearX	= leftX
	local nearY	= bottomY
	local stepX	= 0
	local stepY	= 0
	local rectW	= rightX-leftX 
	local rectH	= topY-bottomY
	
	if rectW < 0 then
		rectW = rectW + mapW
	end
	
	if rectH < 0 then
		rectH = rectH + mapH
	end
	
	local nextPlot = Map.GetPlot(nearX, nearY)
	
	return function ()
		while (stepY < 1 + rectH) and nextPlot do
			while (stepX < 1 + rectW) and nextPlot do
				local plot		= nextPlot
				local distance	= Map.PlotDistance(nearX, nearY, centerX, centerY)
				
				nearX		= (nearX + 1) % mapW
				stepX		= stepX + 1
				nextPlot	= Map.GetPlot(nearX, nearY)
				
				if Game.IsBetween(minR, distance, maxR) then
					return plot, distance
				end
			end
			nearX		= leftX
			nearY		= (nearY + 1) % mapH
			stepX		= 0
			stepY		= stepY + 1
			nextPlot	= Map.GetPlot(nearX, nearY)
		end
	end
end
---------------------------------------------------------------------




------------------------------------------------------------------------------
FeatureGenerator = {};
------------------------------------------------------------------------------
function FeatureGenerator.Create(args)
	--[[ Civ4's truncated "Climate" setting has been abandoned. Civ5 has returned to 
	Civ3-style map options for World Age, Temperature, and Rainfall. Control over the 
	terrain has been removed from the XML.  - Bob Thomas, March 2010  ]]--
	--
	-- Sea Level and World Age map options affect only plot generation.
	-- Temperature map options affect only terrain generation.
	-- Rainfall map options affect only feature generation.
	--
	local args = args or {};
	local rainfall = args.rainfall or 2; -- Default is Normal rainfall.
	local jungle_grain = args.jungle_grain or 5;
	local forest_grain = args.forest_grain or 6;
	local clump_grain = args.clump_grain or 3;
	local iJungleChange = args.iJungleChange or 20;
	local iForestChange = args.iForestChange or 15;
	local iClumpChange = args.iClumpChange or 5;
	local iJungleFactor = args.iJungleFactor or 5;
	local iAridFactor = args.iAridFactor or 6;
	local iWetFactor = args.iWetFactor or 2;
	local fMarshChange = args.fMarshChange or 1.5;
	local fOasisChange = args.fOasisChange or 1.5;
	local fracXExp = args.fracXExp or -1;
	local fracYExp = args.fracYExp or -1;
	
	-- Set feature traits.
	local iJunglePercent = args.iJunglePercent or 65;
	local iForestPercent = args.iForestPercent or 31;
	local iClumpHeight = args.iClumpHeight or 90;
	local fMarshPercent = args.fMarshPercent or 8;
	local iOasisPercent = args.iOasisPercent or 6;
	-- Adjust foliage amounts according to user's Rainfall selection. (Which must be passed in by the map script.)
	if rainfall == 1 then -- Rainfall is sparse, climate is Arid.
		iJunglePercent = iJunglePercent - iJungleChange;
		iJungleFactor = iAridFactor;
		iForestPercent = iForestPercent - iForestChange;
		iClumpHeight = iClumpHeight - iClumpChange;
		fMarshPercent = fMarshPercent / fMarshChange;
		iOasisPercent = iOasisPercent / fOasisChange;
	elseif rainfall == 3 then -- Rainfall is abundant, climate is Wet.
		iJunglePercent = iJunglePercent + iJungleChange;
		iJungleFactor = iWetFactor;
		iForestPercent = iForestPercent + iForestChange;
		iClumpHeight = iClumpHeight + iClumpChange;
		fMarshPercent = fMarshPercent * fMarshChange;
		iOasisPercent = iOasisPercent * fOasisChange;
	else -- Rainfall is Normal.
	end

	--[[ Activate printout for debugging only.
	print("-"); print("--- Rainfall Readout ---");
	print("- Rainfall Setting:", rainfall);
	print("- Jungle Percentage:", iJunglePercent);
	print("- Loose Forest %:", iForestPercent);
	print("- Clump Forest %:", 100 - iClumpHeight);
	print("- Marsh Percentage:", fMarshPercent);
	print("- Oasis Percentage:", iOasisPercent);
	print("- - - - - - - - - - - - - - -");
	]]--

	local gridWidth, gridHeight = Map.GetGridSize();
	local world_info = Game.GetWorldInfo();
	jungle_grain = jungle_grain + world_info.FeatureGrainChange;
	forest_grain = forest_grain + world_info.FeatureGrainChange;

	-- create instance data
	local instance = {
	
		-- methods
		__initFractals		= FeatureGenerator.__initFractals,
		__initFeatureTypes	= FeatureGenerator.__initFeatureTypes,
		AddFeatures			= FeatureGenerator.AddFeatures,
		GetLatitudeAtPlot	= FeatureGenerator.GetLatitudeAtPlot,
		AddFeaturesAtPlot	= FeatureGenerator.AddFeaturesAtPlot,
		AddOasisAtPlot		= FeatureGenerator.AddOasisAtPlot,
		AddIceAtPlot		= FeatureGenerator.AddIceAtPlot,
		AddMarshAtPlot		= FeatureGenerator.AddMarshAtPlot,
		AddJunglesAtPlot	= FeatureGenerator.AddJunglesAtPlot,
		AddForestsAtPlot	= FeatureGenerator.AddForestsAtPlot,
		AddAtolls			= FeatureGenerator.AddAtolls,
		AdjustTerrainTypes	= FeatureGenerator.AdjustTerrainTypes,
		
		-- members
		iGridW = gridWidth,
		iGridH = gridHeight,
		
		iJunglePercent = iJunglePercent,
		iJungleFactor = iJungleFactor,
		iForestPercent = iForestPercent,
		iClumpHeight = iClumpHeight,
		fMarshPercent = fMarshPercent,
		iOasisPercent = iOasisPercent,
	
		jungle_grain = jungle_grain,
		forest_grain = forest_grain,
		clump_grain = clump_grain,
		
		fractalFlags = Map.GetFractalFlags(),
		fracXExp = fracXExp,
		fracYExp = fracYExp,
	};

	-- initialize instance data
	instance:__initFractals()
	instance:__initFeatureTypes()
	
	return instance;
end
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
	self.iForestLevel	= self.forests:GetHeight(100 - self.iForestPercent)
	self.iClumpLevel	= self.forestclumps:GetHeight(self.iClumpHeight)
	self.iMarshLevel	= self.marsh:GetHeight(100 - self.fMarshPercent)
end
------------------------------------------------------------------------------
function FeatureGenerator:__initFeatureTypes()

	self.featureFloodPlains = FeatureTypes.FEATURE_FLOOD_PLAINS;
	self.featureIce = FeatureTypes.FEATURE_ICE;
	self.featureJungle = FeatureTypes.FEATURE_JUNGLE;
	self.featureForest = FeatureTypes.FEATURE_FOREST;
	self.featureOasis = FeatureTypes.FEATURE_OASIS;
	self.featureMarsh = FeatureTypes.FEATURE_MARSH;
	
	self.terrainIce = TerrainTypes.TERRAIN_SNOW;
	self.terrainTundra = TerrainTypes.TERRAIN_TUNDRA;
	self.terrainPlains = TerrainTypes.TERRAIN_PLAINS;
end
------------------------------------------------------------------------------
function FeatureGenerator:AddFeatures(allow_mountains_on_coast)
	local flag = allow_mountains_on_coast or true;
	--[[ Removing mountains from coasts cannot be done during plot or terrain 
	generation, because the function that determines what is or isn't adjacent
	to a salt water ocean requires both plot and terrain data to operate. So
	even though this operation is, strictly speaking, a plot-type operation, I
	have added it here in the default FeatureGenerator so I can easily call on
	it for any script that needs it.  - Bob Thomas, March 2010  ]]--
	--
	if allow_mountains_on_coast == false then -- remove any mountains from coastal plots
		for x = 0, self.iGridW - 1 do
			for y = 0, self.iGridH - 1 do
				local plot = Map.GetPlot(x, y)
				if plot:GetPlotType() == PlotTypes.PLOT_MOUNTAIN then
					if plot:IsCoastalLand() then
						plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true); -- These flags are for recalc of areas and rebuild of graphics. Instead of recalc over and over, do recalc at end of loop.
					end
				end
			end
		end
		-- This function needs to recalculate areas after operating. However, so does 
		-- adding feature ice, so the recalc was removed from here and put in MapGenerator()
	end
	
	self:AddAtolls(); -- Adds Atolls to oceanic maps.
	
	-- Main loop, adds features to all plots as appropriate
	for y = 0, self.iGridH - 1, 1 do
		for x = 0, self.iGridW - 1, 1 do
			self:AddFeaturesAtPlot(x, y);
		end
	end
	
	self:AdjustTerrainTypes(); -- Sets terrain under jungles and softens arctic rivers
end
------------------------------------------------------------------------------
function FeatureGenerator:GetLatitudeAtPlot(iX, iY)
	-- Latitude affects only jungles and ice by default.
	-- However, you can make use of it in replacement methods for AddAtPlot if you wish.
	-- Returns a value in the range of 0.0 (tropical) to 1.0 (polar)
	return math.abs((self.iGridH/2) - iY)/(self.iGridH/2);
end
------------------------------------------------------------------------------
function FeatureGenerator:AddFeaturesAtPlot(iX, iY)
	-- adds any appropriate features at the plot (iX, iY) where (0,0) is in the SW
	local lat = self:GetLatitudeAtPlot(iX, iY);
	local plot = Map.GetPlot(iX, iY);

	if plot:CanHaveFeature(self.featureFloodPlains) then
		-- All desert plots along river are set to flood plains.
		plot:SetFeatureType(self.featureFloodPlains, -1)
	end
	
	if (plot:GetFeatureType() == FeatureTypes.NO_FEATURE) then
		self:AddOasisAtPlot(plot, iX, iY, lat);
	end

	if (plot:GetFeatureType() == FeatureTypes.NO_FEATURE) then
		self:AddIceAtPlot(plot, iX, iY, lat);
	end

	if (plot:GetFeatureType() == FeatureTypes.NO_FEATURE) then
		self:AddMarshAtPlot(plot, iX, iY, lat);
	end
		
	if (plot:GetFeatureType() == FeatureTypes.NO_FEATURE) then
		self:AddJunglesAtPlot(plot, iX, iY, lat);
	end
	
	if (plot:GetFeatureType() == FeatureTypes.NO_FEATURE) then
		self:AddForestsAtPlot(plot, iX, iY, lat);
	end
		
end
------------------------------------------------------------------------------
function FeatureGenerator:AddOasisAtPlot(plot, iX, iY, lat)
	if not plot:CanHaveFeature(self.featureOasis) then
		return
	end
	
	local odds = 0
	for adjPlot in Plot_GetPlotsInCircle(plot, 3) do
		local distance = Map.PlotDistance(adjPlot:GetX(), adjPlot:GetY(), plot:GetX(), plot:GetY()) or 1
		local featureID = adjPlot:GetFeatureType()
		
		if featureID == FeatureTypes.FEATURE_OASIS then
			if distance <= 2 then
				-- at least 2 tile spacing between oases
				return
			end
			odds = odds - 200 / distance
		end
		
		if featureID == FeatureTypes.NO_FEATURE and not adjPlot:IsFreshWater() then
			if adjPlot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
				odds = odds + 10 / distance
			elseif adjPlot:IsMountain() or adjPlot:IsHills() then
				odds = odds + 10 / distance
			elseif adjPlot:GetTerrainType() == TerrainTypes.TERRAIN_PLAINS then
				odds = odds + 5 / distance
			end
		else
			odds = odds - 5 / distance
		end
	end
	
	if odds >= Map.Rand(100, "PlacePossibleOasis - Lua") then
		plot:SetFeatureType(FeatureTypes.FEATURE_OASIS, -1)
	end
end
------------------------------------------------------------------------------
function FeatureGenerator:AddIceAtPlot(plot, iX, iY, lat)
	if(plot:CanHaveFeature(self.featureIce)) then
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
------------------------------------------------------------------------------
function FeatureGenerator:AddMarshAtPlot(plot, iX, iY, lat)
	local marsh_height = self.marsh:GetHeight(iX, iY)
	if(marsh_height >= self.iMarshLevel) then
		if(plot:CanHaveFeature(self.featureMarsh)) then
			plot:SetFeatureType(self.featureMarsh, -1)
		end
	end
end
------------------------------------------------------------------------------
function FeatureGenerator:AddJunglesAtPlot(plot, iX, iY, lat)
	local jungle_height = self.jungles:GetHeight(iX, iY);
	local climate_info = GameInfo.Climates[Map.GetClimate()];
	if jungle_height <= self.iJungleTop and jungle_height >= self.iJungleBottom + (self.iJungleRange * lat) then
		if(plot:CanHaveFeature(self.featureJungle)) then
			plot:SetFeatureType(self.featureJungle, -1);
		end
	end
end
------------------------------------------------------------------------------
function FeatureGenerator:AddForestsAtPlot(plot, iX, iY, lat)
	if (self.forests:GetHeight(iX, iY) >= self.iForestLevel) or (self.forestclumps:GetHeight(iX, iY) >= self.iClumpLevel) then
		if plot:CanHaveFeature(self.featureForest) then
			plot:SetFeatureType(self.featureForest, -1)
		end
	end
end
------------------------------------------------------------------------------
function FeatureGenerator:AdjustTerrainTypes()
	-- This function added April 2009 for Civ5, by Bob Thomas.
	-- Purpose of this function is to turn terrain under jungles
	-- into Plains, and to soften arctic terrain types at rivers.
	local width = self.iGridW - 1;
	local height = self.iGridH - 1;
	
	for y = 0, height do
		for x = 0, width do
			local plot = Map.GetPlot(x, y);
			
			if (plot:GetFeatureType() == self.featureJungle) then
				plot:SetTerrainType(self.terrainPlains, false, true)  -- These flags are for recalc of areas and rebuild of graphics. No need to recalc from any of these changes.		
			elseif (plot:IsRiver()) then
				local terrainType = plot:GetTerrainType();
				if (terrainType == self.terrainTundra) then
					plot:SetTerrainType(self.terrainPlains, false, true)
				elseif (terrainType == self.terrainIce) then
					plot:SetTerrainType(self.terrainTundra, false, true)					
				end
			end
		end
	end
end
------------------------------------------------------------------------------
function FeatureGenerator:AddAtolls()
	-- This function added Feb 2011 by Bob Thomas.
	-- Adds the new feature Atolls in to the game, for oceanic maps.
	local iW, iH = Map.GetGridSize()
	local biggest_ocean = Map.FindBiggestArea(true)
	local iNumBiggestOceanPlots = 0;
	if biggest_ocean ~= nil then
		iNumBiggestOceanPlots = biggest_ocean:GetNumTiles()
	end
	if iNumBiggestOceanPlots <= (iW * iH) / 4 then -- No major oceans on this world.
		return
	end
	
	-- World has oceans, proceed with adding Atolls.
	local iNumAtollsPlaced = 0;
	local direction_types = {
		DirectionTypes.DIRECTION_NORTHEAST,
		DirectionTypes.DIRECTION_EAST,
		DirectionTypes.DIRECTION_SOUTHEAST,
		DirectionTypes.DIRECTION_SOUTHWEST,
		DirectionTypes.DIRECTION_WEST,
		DirectionTypes.DIRECTION_NORTHWEST
	};
	local numCoast = 0
	local coastID = GameInfo.Terrains.TERRAIN_COAST.ID
	for i, plot in Plots() do
		if plot:GetTerrainType() == coastID then
			numCoast = numCoast + 1
		end
	end
	--[[
	local worldsizes = {
		[GameInfo.CepWorlds.WORLDSIZE_DUEL.ID] = 2,
		[GameInfo.CepWorlds.WORLDSIZE_TINY.ID] = 4,
		[GameInfo.CepWorlds.WORLDSIZE_SMALL.ID] = 5,
		[GameInfo.CepWorlds.WORLDSIZE_STANDARD.ID] = 7,
		[GameInfo.CepWorlds.WORLDSIZE_LARGE.ID] = 9,
		[GameInfo.CepWorlds.WORLDSIZE_HUGE.ID] = 12,
	};
	--]]
	local atoll_target = numCoast * 0.10;
	local variance = 25
		  variance = atoll_target * (Map.Rand(2 * variance, "Number of Atolls to place - LUA") - variance) / 100
	local atoll_number = math.floor(atoll_target + variance);
	local feature_atoll;
	for thisFeature in GameInfo.Features() do
		if thisFeature.Type == "FEATURE_ATOLL" then
			feature_atoll = thisFeature.ID;
		end
	end

	-- Generate candidate plot lists.
	local temp_one_tile_island_list, temp_alpha_list, temp_beta_list = {}, {}, {};
	local temp_gamma_list, temp_delta_list, temp_epsilon_list = {}, {}, {};
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = y * iW + x + 1; -- Lua tables/lists/arrays start at 1, not 0 like C++ or Python
			local plot = Map.GetPlot(x, y)
			local plotType = plot:GetPlotType()
			if plotType == PlotTypes.PLOT_OCEAN then
				local featureType = plot:GetFeatureType()
				if featureType ~= FeatureTypes.FEATURE_ICE then
					if not plot:IsLake() then
						local terrainType = plot:GetTerrainType()
						if terrainType == TerrainTypes.TERRAIN_COAST then
							-- Check all adjacent plots and identify adjacent landmasses.
							local iNumLandAdjacent, biggest_adj_area = 0, 0;
							local bPlotValid = true;
							for loop, direction in ipairs(direction_types) do
								local adjPlot = Map.PlotDirection(x, y, direction)
								if adjPlot ~= nil then
									local adjPlotType = adjPlot:GetPlotType()
									--[[
									if adjPlot:GetFeatureType() == FeatureTypes.FEATURE_ICE then
										bPlotValid = false;
									elseif adjPlot:GetTerrainType() == TerrainTypes.TERRAIN_SNOW then
										bPlotValid = false;
									end
									--]]
									if adjPlotType ~= PlotTypes.PLOT_OCEAN then -- Found land.
										iNumLandAdjacent = iNumLandAdjacent + 1;
										if adjPlotType == PlotTypes.PLOT_LAND or adjPlotType == PlotTypes.PLOT_HILLS then
											local iArea = adjPlot:GetArea()
											local adjArea = Map.GetArea(iArea)

											local iNumAreaPlots = adjArea:GetNumTiles()
											if iNumAreaPlots > biggest_adj_area then
												biggest_adj_area = iNumAreaPlots;
											end
										end
									end
								end
							end
							-- Only plots with a single land plot adjacent can be eligible.
							if bPlotValid and iNumLandAdjacent <= 1 then
								--if biggest_adj_area >= 152 then
									-- discard this site
								if biggest_adj_area >= 82 then
									table.insert(temp_epsilon_list, i);
								elseif biggest_adj_area >= 34 then
									table.insert(temp_delta_list, i);
								elseif biggest_adj_area >= 16 or biggest_adj_area == 0 then
									table.insert(temp_gamma_list, i);
								elseif biggest_adj_area >= 6 then
									table.insert(temp_beta_list, i);
								elseif biggest_adj_area >= 1 then
									table.insert(temp_alpha_list, i);
								else -- Unexpected result
									--print("** Area Plot Count =", biggest_adj_area);
								end
							end
						end
					end
				end
			end
		end
	end
	local alpha_list = GetShuffledCopyOfTable(temp_alpha_list)
	local beta_list = GetShuffledCopyOfTable(temp_beta_list)
	local gamma_list = GetShuffledCopyOfTable(temp_gamma_list)
	local delta_list = GetShuffledCopyOfTable(temp_delta_list)
	local epsilon_list = GetShuffledCopyOfTable(temp_epsilon_list)

	-- Determine maximum number able to be placed, per candidate category.
	local max_alpha = math.ceil(table.maxn(alpha_list) )--* .25);
	local max_beta = math.ceil(table.maxn(beta_list) )--* .2);
	local max_gamma = math.ceil(table.maxn(gamma_list) )--* .25);
	local max_delta = math.ceil(table.maxn(delta_list) )--* .3);
	local max_epsilon = math.ceil(table.maxn(epsilon_list) )--* .4);
	
	-- Place Atolls.
	local plotIndex;
	local i_alpha, i_beta, i_gamma, i_delta, i_epsilon = 1, 1, 1, 1, 1;		
	local passNum = 0;	
	
	while (iNumAtollsPlaced < atoll_number) and (passNum < atoll_number * 5) do
		local able_to_proceed = true;
		local diceroll = 1 + Map.Rand(100, "Atoll Placement Type - LUA");
		if diceroll <= 40 and max_alpha > 0 then
			plotIndex = alpha_list[i_alpha];
			i_alpha = i_alpha + 1;
			max_alpha = max_alpha - 1;
			--print("- Alpha site chosen");
		elseif diceroll <= 65 then
			if max_beta > 0 then
				plotIndex = beta_list[i_beta];
				i_beta = i_beta + 1;
				max_beta = max_beta - 1;
				--print("- Beta site chosen");
			elseif max_alpha > 0 then
				plotIndex = alpha_list[i_alpha];
				i_alpha = i_alpha + 1;
				max_alpha = max_alpha - 1;
				--print("- Alpha site chosen");
			else -- Unable to place this Atoll
				--print("-"); print("* Atoll #", loop, "was unable to be placed.");
				able_to_proceed = false;
			end
		elseif diceroll <= 80 then
			if max_gamma > 0 then
				plotIndex = gamma_list[i_gamma];
				i_gamma = i_gamma + 1;
				max_gamma = max_gamma - 1;
				--print("- Gamma site chosen");
			elseif max_beta > 0 then
				plotIndex = beta_list[i_beta];
				i_beta = i_beta + 1;
				max_beta = max_beta - 1;
				--print("- Beta site chosen");
			elseif max_alpha > 0 then
				plotIndex = alpha_list[i_alpha];
				i_alpha = i_alpha + 1;
				max_alpha = max_alpha - 1;
				--print("- Alpha site chosen");
			else -- Unable to place this Atoll
				--print("-"); print("* Atoll #", loop, "was unable to be placed.");
				able_to_proceed = false;
			end
		elseif diceroll <= 90 then
			if max_delta > 0 then
				plotIndex = delta_list[i_delta];
				i_delta = i_delta + 1;
				max_delta = max_delta - 1;
				--print("- Delta site chosen");
			elseif max_gamma > 0 then
				plotIndex = gamma_list[i_gamma];
				i_gamma = i_gamma + 1;
				max_gamma = max_gamma - 1;
				--print("- Gamma site chosen");
			elseif max_beta > 0 then
				plotIndex = beta_list[i_beta];
				i_beta = i_beta + 1;
				max_beta = max_beta - 1;
				--print("- Beta site chosen");
			elseif max_alpha > 0 then
				plotIndex = alpha_list[i_alpha];
				i_alpha = i_alpha + 1;
				max_alpha = max_alpha - 1;
				--print("- Alpha site chosen");
			else -- Unable to place this Atoll
				--print("-"); print("* Atoll #", loop, "was unable to be placed.");
				able_to_proceed = false;
			end
		else
			if max_epsilon > 0 then
				plotIndex = epsilon_list[i_epsilon];
				i_epsilon = i_epsilon + 1;
				max_epsilon = max_epsilon - 1;
				--print("- Epsilon site chosen");
			elseif max_delta > 0 then
				plotIndex = delta_list[i_delta];
				i_delta = i_delta + 1;
				max_delta = max_delta - 1;
				--print("- Delta site chosen");
			elseif max_gamma > 0 then
				plotIndex = gamma_list[i_gamma];
				i_gamma = i_gamma + 1;
				max_gamma = max_gamma - 1;
				--print("- Gamma site chosen");
			elseif max_beta > 0 then
				plotIndex = beta_list[i_beta];
				--print("- Beta site chosen");
				i_beta = i_beta + 1;
				max_beta = max_beta - 1;
			elseif max_alpha > 0 then
				plotIndex = alpha_list[i_alpha];
				i_alpha = i_alpha + 1;
				max_alpha = max_alpha - 1;
				--print("- Alpha site chosen");
			else -- Unable to place this Atoll
				--print("-"); print("* Atoll #", loop, "was unable to be placed.");
				able_to_proceed = false;
			end
		end
		if able_to_proceed and plotIndex then
			local x = (plotIndex - 1) % iW;
			local y = (plotIndex - x - 1) / iW;
			local plot = Map.GetPlot(x, y)
			for _, direction in ipairs(direction_types) do
				local adjPlot = Map.PlotDirection(x, y, direction)
				if adjPlot and adjPlot:GetFeatureType() == feature_atoll then
					able_to_proceed = false
					--print("Adjacent atoll")
					break
				end
			end
			if able_to_proceed then
				plot:SetFeatureType(feature_atoll, -1);
				iNumAtollsPlaced = iNumAtollsPlaced + 1;
			end
		end
		passNum = passNum + 1
	end 
	
	--[[ Debug report
	print("-");
	print("-                 Atoll Target Number: ", atoll_number);
	print("-             Number of Atolls placed: ", iNumAtollsPlaced);
	print("-                            Attempts: ", passNum);
	print("-");	
	print("- Atolls placed in Alpha locations   : ", i_alpha - 1);
	print("- Atolls placed in Beta locations    : ", i_beta - 1);
	print("- Atolls placed in Gamma locations   : ", i_gamma - 1);
	print("- Atolls placed in Delta locations   : ", i_delta - 1);
	print("- Atolls placed in Epsilon locations : ", i_epsilon - 1);
	--]]
end
------------------------------------------------------------------------------
