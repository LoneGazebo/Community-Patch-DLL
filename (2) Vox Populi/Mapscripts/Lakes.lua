------------------------------------------------------------------------------
--	FILE:	 Lakes.lua
--	AUTHOR:  Bob Thomas (Sirian)
--	PURPOSE: Global map script - An oceanless planet.
------------------------------------------------------------------------------
--	Copyright (c) 2009 Firaxis Games, Inc. All rights reserved.
------------------------------------------------------------------------------

include("MapGenerator");
include("MultilayeredFractal");
include("FeatureGenerator");
include("TerrainGenerator");

------------------------------------------------------------------------------
function GetMapScriptInfo()
	local world_age, temperature, rainfall, _, resources = GetCoreMapOptions();
	return {
		Name = "TXT_KEY_MAP_LAKES_VP",
		Description = "TXT_KEY_MAP_LAKES_HELP",
		IsAdvancedMap = false,
		SupportsMultiplayer = true,
		IconIndex = 13,
		CustomOptions = {
			world_age,
			temperature,
			rainfall,
			resources,
			{
				Name = "TXT_KEY_MAP_OPTION_BODIES_OF_WATER",
				Values = {
					{"TXT_KEY_MAP_OPTION_SMALL_LAKES", "TXT_KEY_MAP_OPTION_SMALL_LAKES_HELP"},
					{"TXT_KEY_MAP_OPTION_LARGE_LAKES", "TXT_KEY_MAP_OPTION_LARGE_LAKES_HELP"},
					{"TXT_KEY_MAP_OPTION_SEAS", "TXT_KEY_MAP_OPTION_SEAS_HELP"},
					"TXT_KEY_MAP_OPTION_RANDOM",
				},
				DefaultValue = 4,
				SortPriority = 1,
			},
		},
	};
end
------------------------------------------------------------------------------

-------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	-- This function can reset map grid sizes or world wrap settings.

	-- Lakes is a world without oceans, so use grid sizes two levels below normal.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {24, 16},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {32, 20},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {40, 24},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {52, 32},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {64, 40},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {84, 52},
	};
	local grid_size = worldsizes[worldSize];

	if GameInfo.Worlds[worldSize] then
		return {
			Width = grid_size[1],
			Height = grid_size[2],
			WrapX = true,
		};
	end
end
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
function MultilayeredFractal:GeneratePlotsByRegion()
	-- Sirian's MultilayeredFractal controlling function.
	-- You -MUST- customize this function for each script using MultilayeredFractal.

	-- This implementation is specific to Lakes.
	local iW, iH = Map.GetGridSize();
	local fracFlags = {FRAC_WRAP_X = true, FRAC_POLAR = true};

	-- Fill all but the top two rows with land plots.
	self.wholeworldPlotTypes = table.fill(PlotTypes.PLOT_LAND, iW * (iH - 2));

	-- Ensure top two and bottom two rows are ocean (to give rivers a place to end and to create polar ice).
	for x = 0, iW - 1 do
		self.wholeworldPlotTypes[x + 1] = PlotTypes.PLOT_OCEAN;
		self.wholeworldPlotTypes[iW + x + 1] = PlotTypes.PLOT_OCEAN;
		self.wholeworldPlotTypes[iW * iH - x] = PlotTypes.PLOT_OCEAN;
		self.wholeworldPlotTypes[iW * (iH - 1) - x] = PlotTypes.PLOT_OCEAN;
	end

	-- Get user inputs.
	local world_age = Map.GetCustomOption(1);
	if world_age == 4 then
		world_age = 1 + Map.Rand(3, "Random World Age - Lua");
	end
	local userInputLakes = Map.GetCustomOption(5);
	if userInputLakes == 4 then -- Random
		userInputLakes = 1 + Map.Rand(3, "Highlands Random Lake Size - Lua");
	end

	-- Lake density
	local lake_list = {93, 90, 85};
	local lake_grains = {5, 4, 3};
	local lakes = lake_list[userInputLakes];
	local lake_grain = lake_grains[userInputLakes];

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

	-- Land and water are set. Now apply hills and mountains.
	local args = {
		adjust_plates = 1.2,
		world_age = world_age,
	};
	self:ApplyTectonics(args);

	-- Plot Type generation completed. Return global plot array.
	return self.wholeworldPlotTypes;
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Setting Plot Types (Lua Lakes) ...");

	local layered_world = MultilayeredFractal.Create();
	local plotsLakes = layered_world:GeneratePlotsByRegion();

	SetPlotTypes(plotsLakes);

	GenerateCoasts();
end
------------------------------------------------------------------------------
function GenerateTerrain()
	print("Generating Terrain (Lua Lakes) ...");

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
function FeatureGenerator:AddIceAtPlot(plot, _, iY, lat)
	-- Overriding default feature ice to ensure that the ice fully covers two rows, at top and bottom
	if plot:CanHaveFeature(self.featureIce) then
		if iY < 2 or iY >= self.iGridH - 2 then
			plot:SetFeatureType(self.featureIce);
		else
			local rand = Map.Rand(100, "Add Ice Lua") / 100.0;
			if rand < 8 * (lat - 0.875) then
				plot:SetFeatureType(self.featureIce);
			elseif rand < 4 * (lat - 0.75) then
				plot:SetFeatureType(self.featureIce);
			end
		end
	end
end
------------------------------------------------------------------------------
function AddFeatures()
	print("Adding Features (Lua Lakes) ...");

	-- Get Rainfall setting input by user.
	local rain = Map.GetCustomOption(3);
	if rain == 4 then
		rain = 1 + Map.Rand(3, "Random Rainfall - Lua");
	end

	local featuregen = FeatureGenerator.Create{rainfall = rain};

	-- False parameter removes mountains from coastlines.
	featuregen:AddFeatures(false);
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function AssignStartingPlots:PlaceOilInTheSea()
	-- Oil only on land for this map script.
end
------------------------------------------------------------------------------
function StartPlotSystem()
	-- Get Resources setting input by user.
	local res = Map.GetCustomOption(5);
	if res == 6 then
		res = 1 + Map.Rand(3, "Random Resources Option - Lua");
	end

	print("Creating start plot database.");
	local start_plot_database = AssignStartingPlots.Create();

	print("Dividing the map in to Regions.");
	-- Regional Division Method 1: Biggest Landmass
	local args = {
		method = 1,
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
