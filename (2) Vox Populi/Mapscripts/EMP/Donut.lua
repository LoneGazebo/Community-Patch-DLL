-------------------------------------------------------------------------------
--	FILE:	 Donut.lua
--	AUTHOR:  Bob Thomas (Sirian)
--	PURPOSE: Global map script - Circular continent with center region.
-------------------------------------------------------------------------------
--	Copyright (c) 2011 Firaxis Games, Inc. All rights reserved.
-------------------------------------------------------------------------------

include("MapGenerator");
include("MultilayeredFractal");
include("TerrainGenerator");
include("FeatureGenerator");

------------------------------------------------------------------------------
function GetMapScriptInfo()
	local world_age, temperature, rainfall, sea_level, resources = GetCoreMapOptions()
	return {
		Name = "TXT_KEY_MAP_DONUT",
		Description = "TXT_KEY_MAP_DONUT_HELP",
		IconIndex = 18,
		IconAtlas = "WORLDTYPE_ATLAS_3",
		SupportsMultiplayer = true,
		Folder = "MAP_FOLDER_SP_DLC_1",
		CustomOptions = {
			{
				Name = "TXT_KEY_MAP_OPTION_CENTER_REGION",
				Values = {
					"TXT_KEY_MAP_OPTION_HILLS",
					"TXT_KEY_MAP_OPTION_MOUNTAINS",
					"TXT_KEY_MAP_OPTION_OCEAN",
					"TXT_KEY_MAP_OPTION_DESERT",
					"TXT_KEY_MAP_OPTION_STANDARD",
					"TXT_KEY_MAP_OPTION_RANDOM",
				},
				DefaultValue = 3,
				SortPriority = 1,
			},
		}
	};
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	-- Donut uses a square map grid.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {24, 24},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {36, 36},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {44, 44},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {52, 52},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {64, 64},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {80, 80}
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

-------------------------------------------------------------------------------
function MultilayeredFractal:GeneratePlotsByRegion()
	-- Sirian's MultilayeredFractal controlling function.
	-- You -MUST- customize this function for each script using MultilayeredFractal.
	--
	-- This implementation is specific to Donut.
	local iW, iH = Map.GetGridSize();
	local fracFlags = {FRAC_WRAP_X = false, FRAC_POLAR = true};

	-- Get user input.
	hole_type = Map.GetCustomOption(1) -- Global
	if hole_type == 6 then
		hole_type = 1 + Map.Rand(5, "Random terrain type for center region - Donut Lua");
	end

	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 6,
		};
	local grain = worldsizes[Map.GetWorldSize()];

	local terrainFrac = Fractal.Create(iW, iH, grain, fracFlags, -1, -1);
	local iHillsThreshold = terrainFrac:GetHeight(91);
	local iPeaksThreshold = terrainFrac:GetHeight(96);
	local iHillsClumps = terrainFrac:GetHeight(4);

	local hillsFrac = Fractal.Create(iW, iH, grain, fracFlags, -1, -1);
	local iHillsBottom1 = hillsFrac:GetHeight(20);
	local iHillsTop1 = hillsFrac:GetHeight(30);
	local iHillsBottom2 = hillsFrac:GetHeight(70);
	local iHillsTop2 = hillsFrac:GetHeight(80);

	local iCenterX = math.floor(iW / 2);
	local iCenterY = math.floor(iH / 2);
	local iRadius = iCenterX - 4;
	local iHoleRadius = math.floor(iRadius / 2);

	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = y * iW + x + 1;
			local fDistance = 0;
			if x ~= iCenterX or y ~= iCenterY then
				fDistance = math.sqrt(((x - iCenterX) ^ 2) + ((y - iCenterY) ^ 2));
			end
			if fDistance > iRadius then
				self.wholeworldPlotTypes[i] = PlotTypes.PLOT_OCEAN;
			elseif fDistance < iHoleRadius and hole_type < 4 then -- Plot is in hole of donut.
				if hole_type == 1 then
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_HILLS;
				elseif hole_type == 2 then
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
				else
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_OCEAN;
				end
			else
				local val = terrainFrac:GetHeight(x, y);
				local hillsVal = hillsFrac:GetHeight(x, y);
				if val >= iPeaksThreshold then
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
				elseif val >= iHillsThreshold or val <= iHillsClumps then
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_HILLS;
				elseif hillsVal >= iHillsBottom1 and hillsVal <= iHillsTop1 then
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_HILLS;
				elseif hillsVal >= iHillsBottom2 and hillsVal <= iHillsTop2 then
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_HILLS;
				else
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_LAND;
				end
			end
		end
	end

	-- Plot Type generation completed. Return global plot array.
	return self.wholeworldPlotTypes
end
------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Setting Plot Types (Lua Donut) ...");

	local layered_world = MultilayeredFractal.Create();
	local plotsDonut = layered_world:GeneratePlotsByRegion();
	
	SetPlotTypes(plotsDonut);

	GenerateCoasts();
end
------------------------------------------------------------------------------

----------------------------------------------------------------------------------
function TerrainGenerator:GenerateTerrainAtPlot(iX, iY)
	local plot = Map.GetPlot(iX, iY);
	if (plot:IsWater()) then
		local val = plot:GetTerrainType();
		if val == TerrainTypes.NO_TERRAIN then -- Error handling.
			val = self.terrainGrass;
			plot:SetPlotType(PlotTypes.PLOT_LAND, false, false);
		end
		return val;	 
	end

	local iW, iH = Map.GetGridSize();
	local iCenterX = math.floor(iW / 2);
	local iCenterY = math.floor(iH / 2);
	local iRadius = iCenterX - 4;
	local iHoleRadius = math.floor(iRadius / 2);
	local terrainVal = self.terrainGrass;

	local fDistance = 0;
	if iX ~= iCenterX or iY ~= iCenterY then
		fDistance = math.sqrt(((iX - iCenterX) ^ 2) + ((iY - iCenterY) ^ 2));
	end
	if fDistance < iHoleRadius and hole_type == 4 then -- Desert plot in center.
		terrainVal = self.terrainDesert;
	else
		local desertVal = self.deserts:GetHeight(iX, iY);
		local plainsVal = self.plains:GetHeight(iX, iY);
		if ((desertVal >= self.iDesertBottom) and (desertVal <= self.iDesertTop)) then
			terrainVal = self.terrainDesert;
		elseif ((plainsVal >= self.iPlainsBottom) and (plainsVal <= self.iPlainsTop)) then
			terrainVal = self.terrainPlains;
		end
	end
	
	return terrainVal;
end
----------------------------------------------------------------------------------
function GenerateTerrain()
	print("Generating Terrain (Lua Donut) ...");
	
	local terraingen = TerrainGenerator.Create();

	terrainTypes = terraingen:GenerateTerrain();
	
	SetTerrainTypes(terrainTypes);
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function FeatureGenerator:AddIceAtPlot(plot, iX, iY, lat)
	-- No ice.
end
------------------------------------------------------------------------------
function FeatureGenerator:AddJunglesAtPlot(plot, iX, iY, lat)
	-- No jungle.
end
------------------------------------------------------------------------------
function AddFeatures()
	print("Adding Features (Lua Donut) ...");

	local featuregen = FeatureGenerator.Create();

	-- False parameter removes mountains from coastlines.
	featuregen:AddFeatures(false);
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function StartPlotSystem()
	print("Creating start plot database.");
	local start_plot_database = AssignStartingPlots.Create()
	
	print("Dividing the map in to Regions.");
	-- Regional Division Method 1: Biggest Landmass
	local args = {
		method = 1,
		};
	start_plot_database:GenerateRegions(args)

	print("Choosing start locations for civilizations.");
	local args = {
	mustBeCoast = true,
	};
	start_plot_database:ChooseLocations(args)
	
	print("Normalizing start locations and assigning them to Players.");
	start_plot_database:BalanceAndAssign()

	print("Placing Natural Wonders.");
	start_plot_database:PlaceNaturalWonders()

	print("Placing Resources and City States.");
	start_plot_database:PlaceResourcesAndCityStates()
end
------------------------------------------------------------------------------
