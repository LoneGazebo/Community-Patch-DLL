------------------------------------------------------------------------------
--	FILE:	 Terra.lua
--	AUTHOR:  Bob Thomas (Sirian)
--	PURPOSE: Global map script - Simulates Terran (Earth-like) worlds
------------------------------------------------------------------------------
--	Copyright (c) 2010 Firaxis Games, Inc. All rights reserved.
------------------------------------------------------------------------------

include("MapGenerator");
include("MultilayeredFractal");
include("TerrainGenerator");
include("FeatureGenerator");

------------------------------------------------------------------------------
-- Terra is still one of my favorite scripts. The main change to this map type in Civ5 is the presence of the City States.
-- No longer is the New World a rats nest of Barbarian Cities, but instead now a home to numerous City States.

-- Civilizations start in the Old World. - Bob Thomas, April 2010
------------------------------------------------------------------------------
function GetMapScriptInfo()
	local world_age, temperature, rainfall, _, resources = GetCoreMapOptions();
	return {
		Name = "TXT_KEY_MAP_TERRA_VP",
		Description = "TXT_KEY_MAP_TERRA_HELP",
		IsAdvancedMap = false,
		SupportsMultiplayer = true,
		IconIndex = 3,
		CustomOptions = {world_age, temperature, rainfall, resources},
	};
end
------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	-- This function can reset map grid sizes or world wrap settings.

	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {52, 32},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {64, 40},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {84, 52},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {104, 64},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {128, 80},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {152, 96}
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
------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- Terra uses the MultilayeredFractal for plot generation.
------------------------------------------------------------------------------
function MultilayeredFractal:GeneratePlotsByRegion()
	-- Sirian's MultilayeredFractal controlling function.
	-- You -MUST- customize this function for each script using MultilayeredFractal.

	-- The following regions are specific to Terra.
	local newworldWestLon = 0.05;
	local newworldEastLon = 0.35;
	local eurasiaWestLon = 0.45;
	local eurasiaEastLon = 0.95;
	local eurasiaNorthLat = 0.95;
	local eurasiaSouthLat = 0.45;
	local thirdworldDimension = 0.125;
	local thirdworldNorthLat = 0.35;
	local thirdworldSouthLat = 0.05;
	local subcontinentLargeHorz = 0.2;
	local subcontinentLargeVert = 0.35;
	local subcontinentLargeNorthLat = 0.7;
	local subcontinentLargeSouthLat = 0.3;
	local subcontinentSmallDimension = 0.125;
	local subcontinentSmallNorthLat = 0.525;
	local subcontinentSmallSouthLat = 0.4;

	-- Dice rolls to randomize the quadrants
	local roll1 = Map.Rand(2, "Eurasian Hemisphere N/S - Terra Lua");
	if roll1 == 1 then
		eurasiaNorthLat = eurasiaNorthLat - 0.4;
		eurasiaSouthLat = eurasiaSouthLat - 0.4;
		thirdworldNorthLat = thirdworldNorthLat + 0.6;
		thirdworldSouthLat = thirdworldSouthLat + 0.6;
		subcontinentLargeNorthLat = subcontinentLargeNorthLat + 0.00;
		subcontinentLargeSouthLat = subcontinentLargeSouthLat + 0.00;
		subcontinentSmallNorthLat = subcontinentSmallNorthLat + 0.075;
		subcontinentSmallSouthLat = subcontinentSmallSouthLat + 0.075;
	end
	local roll2 = Map.Rand(2, "Eurasian Hemisphere E/W - Terra Lua");
	if roll2 == 1 then
		newworldWestLon = newworldWestLon + 0.6;
		newworldEastLon = newworldEastLon + 0.6;
		eurasiaWestLon = eurasiaWestLon - 0.4;
		eurasiaEastLon = eurasiaEastLon - 0.4;
	end

	-- Set up variables that depend on world size.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 5,
	};
	local archGrain = worldsizes[Map.GetWorldSize()];

	-- These values were set up to handle Sea Level implementation.
	-- Then I decided not to include Sea Level for Terra this time, because
	-- the high sea level results are sometimes wonky, and because Terra
	-- was having issues with running up against the memory cap for DX9 version.
	local continent_change = 0;
	local subcontinent_change = 0;
	-- The change levels I had set were 5% for continents, 3% for subcontinents -- in case anyone wants to mess with it after all.

	-- Simulate the Old World - a large continent akin to Earth's Eurasia.
	print("Generating the Old World (Lua Terra) ...");
	-- Set dimensions of the Old World region
	local eurasiaWestX = math.floor(self.iW * eurasiaWestLon);
	local eurasiaEastX = math.floor(self.iW * eurasiaEastLon);
	local eurasiaNorthY = math.floor(self.iH * eurasiaNorthLat);
	local eurasiaSouthY = math.floor(self.iH * eurasiaSouthLat);
	local eurasiaWidth = eurasiaEastX - eurasiaWestX + 1;
	local eurasiaHeight = eurasiaNorthY - eurasiaSouthY + 1;

	local water = 55 + continent_change;

	-- Region's parameters. Any lines commented out are running defaults.
	local args = {};
	args.iWaterPercent = water;
	args.iRegionWidth = eurasiaWidth;
	args.iRegionHeight = eurasiaHeight;
	args.iRegionWestX = eurasiaWestX;
	args.iRegionSouthY = eurasiaSouthY;
	args.iRegionHillsGrain = archGrain;
	args.iRegionPlotFlags = self.iHorzFlags;
	args.iRegionFracXExp = 7;
	args.iRegionFracYExp = 6;
	args.iRiftGrain = 2;

	self:GenerateFractalLayerWithoutHills(args);

	-- Eurasia, second layer (to increase pangaea-like cohesion).
	local twHeight = math.floor(eurasiaHeight / 2);
	local twWestX = eurasiaWestX + math.floor(eurasiaWidth / 10);
	local twEastX = eurasiaEastX - math.floor(eurasiaWidth / 10);
	local twWidth = twEastX - twWestX + 1;
	local twNorthY = eurasiaNorthY - math.floor(eurasiaHeight / 4);
	local twSouthY = eurasiaSouthY + math.floor(eurasiaHeight / 4);

	local water = 60 + continent_change;

	args = {};
	args.iWaterPercent = water;
	args.iRegionWidth = twWidth;
	args.iRegionHeight = twHeight;
	args.iRegionWestX = twWestX;
	args.iRegionSouthY = twSouthY;
	args.iRegionHillsGrain = archGrain;
	args.iRegionPlotFlags = self.iHorzFlags;
	args.iRegionFracXExp = 7;
	args.iRegionFracYExp = 6;
	args.iRiftGrain = 2;

	self:GenerateFractalLayerWithoutHills(args);

	-- Simulate the New World - land masses akin to Earth's American continents.
	-- First simulate North America
	print("Generating the New World (Lua Terra) ...");
	local nwWestX = math.floor(self.iW * newworldWestLon);
	local nwEastX = math.floor(self.iW * newworldEastLon);
	local nwNorthY = math.floor(self.iH * 0.85);
	local nwSouthY = math.floor(self.iH * 0.52);
	local nwWidth = nwEastX - nwWestX + 1;
	local nwHeight = nwNorthY - nwSouthY + 1;

	water = 61 + continent_change;

	args = {};
	args.iWaterPercent = water;
	args.iRegionWidth = nwWidth;
	args.iRegionHeight = nwHeight;
	args.iRegionWestX = nwWestX;
	args.iRegionSouthY = nwSouthY;
	args.iRegionHillsGrain = archGrain;
	args.iRegionPlotFlags = self.iVertFlags;
	args.iRegionFracYExp = 6;

	self:GenerateFractalLayerWithoutHills(args);

	-- Now simulate South America
	local nwsRoll = Map.Rand(2, "New World South E/W - Terra Lua");
	local nwsVar = 0.0;
	if nwsRoll == 1 then
		nwsVar = 0.05;
	end
	local nwsWestX = nwWestX + math.floor(self.iW * (0.08 - nwsVar)); -- Not as wide as the north
	local nwsEastX = nwEastX - math.floor(self.iW * (0.03 + nwsVar));
	local nwsNorthY = math.floor(self.iH * 0.47);
	local nwsSouthY = math.floor(self.iH * 0.25);
	local nwsWidth = nwsEastX - nwsWestX + 1;
	local nwsHeight = nwsNorthY - nwsSouthY + 1;

	water = 55 + continent_change;

	args = {};
	args.iWaterPercent = water;
	args.iRegionWidth = nwsWidth;
	args.iRegionHeight = nwsHeight;
	args.iRegionWestX = nwsWestX;
	args.iRegionSouthY = nwsSouthY;
	args.iRegionHillsGrain = archGrain;
	args.iRegionFracYExp = 6;

	self:GenerateFractalLayerWithoutHills(args);

	local nwpWestX = nwWestX + math.floor(self.iW * (0.1 - nwsVar)); -- Not as wide as the north
	local nwpEastX = nwEastX - math.floor(self.iW * (0.07 + nwsVar));
	local nwpNorthY = math.floor(self.iH * 0.3);
	local nwpSouthY = math.floor(self.iH * 0.18);
	local nwpWidth = nwpEastX - nwpWestX + 1;
	local nwpHeight = nwpNorthY - nwpSouthY + 1;

	water = 67 + subcontinent_change;

	args = {};
	args.iWaterPercent = water;
	args.iRegionWidth = nwpWidth;
	args.iRegionHeight = nwpHeight;
	args.iRegionWestX = nwpWestX;
	args.iRegionSouthY = nwpSouthY;
	args.iRegionHillsGrain = archGrain;
	args.iRegionPlotFlags = self.iVertFlags;

	self:GenerateFractalLayerWithoutHills(args);

	-- Now the Yukon
	twWidth = math.floor(self.iW * 0.15);
	twWestX = nwWestX;
	local boreal = Map.Rand(2, "New World North E/W - Terra Lua");
	if boreal == 1 then
		twWestX = twWestX + math.floor(self.iW * 0.15);
	end
	twEastX = twWestX + twWidth;
	twNorthY = math.floor(self.iH * 0.93);
	twSouthY = math.floor(self.iH * 0.75);
	twHeight = twNorthY - twSouthY + 1;

	water = 68 + subcontinent_change;

	args = {};
	args.iWaterPercent = water;
	args.iRegionWidth = twWidth;
	args.iRegionHeight = twHeight;
	args.iRegionWestX = twWestX;
	args.iRegionSouthY = twSouthY;
	args.iRegionGrain = 2;
	args.iRegionHillsGrain = archGrain;
	args.iRegionPlotFlags = self.iVertFlags;

	self:GenerateFractalLayerWithoutHills(args);

	-- Now add a random region of arctic islands
	twWidth = math.floor(thirdworldDimension * self.iW);
	twHeight = math.floor(thirdworldDimension * self.iH);
	if boreal == 0 then
		twEastX = nwEastX;
		twWestX = twEastX - twWidth;
	else
		twWestX = nwWestX;
		twEastX = twWestX + twWidth;
	end
	twNorthY = math.floor(self.iH * 0.975);
	twSouthY = math.floor(self.iH * 0.85);

	water = 76 + subcontinent_change;

	args = {};
	args.iWaterPercent = water;
	args.iRegionWidth = twWidth;
	args.iRegionHeight = twHeight;
	args.iRegionWestX = twWestX;
	args.iRegionSouthY = twSouthY;
	args.iRegionGrain = archGrain;
	args.iRegionHillsGrain = archGrain;
	args.iRegionPlotFlags = self.iHorzFlags;

	self:GenerateFractalLayerWithoutHills(args);

	-- Now simulate Central America
	local nwcVar = 0.0;
	if nwsRoll == 1 then
		nwcVar = 0.04;
	end
	local nwcWidth = math.floor(self.iW * 0.06);
	local nwcWestX = nwWestX + math.floor(self.iW * (0.1 + nwcVar));
	local nwcNorthY = math.floor(self.iH * 0.6);
	local nwcSouthY = math.floor(self.iH * 0.42);
	local nwcHeight = nwcNorthY - nwcSouthY + 1;

	water = 60;

	args = {};
	args.iWaterPercent = water;
	args.iRegionWidth = nwcWidth;
	args.iRegionHeight = nwcHeight;
	args.iRegionWestX = nwcWestX;
	args.iRegionSouthY = nwcSouthY;
	args.iRegionHillsGrain = archGrain;
	args.iRegionPlotFlags = self.iVertFlags;

	self:GenerateFractalLayerWithoutHills(args);

	-- Now the Carribean islands
	local carVar = 0.0;
	if nwsRoll == 1 then
		carVar = 0.15;
	end
	twWidth = math.floor(0.15 * self.iW);
	twEastX = nwEastX - math.floor(carVar * self.iW);
	twWestX = twEastX - twWidth;
	twNorthY = math.floor(self.iH * 0.55);
	twSouthY = math.floor(self.iH * 0.47);
	twHeight = twNorthY - twSouthY + 1;

	water = 75 + subcontinent_change;

	args = {};
	args.iWaterPercent = water;
	args.iRegionWidth = twWidth;
	args.iRegionHeight = twHeight;
	args.iRegionWestX = twWestX;
	args.iRegionSouthY = twSouthY;
	args.iRegionGrain = archGrain + 1;
	args.iRegionHillsGrain = archGrain;
	args.iRegionPlotFlags = self.iNoFlags;

	self:GenerateFractalLayerWithoutHills(args);

	-- Add subcontinents to the Old World, one large, one small.
	-- Subcontinents can be akin to pangaea, continents, or archipelago.
	-- The large adds an amount of land akin to subSaharan Africa.
	-- The small adds an amount of land akin to South Pacific islands.
	print("Generating the Third World (Lua Terra) ...");
	local scLargeWidth = math.floor(subcontinentLargeHorz * self.iW);
	local scLargeHeight = math.floor(subcontinentLargeVert * self.iH);
	local scRoll = Map.Rand((eurasiaWidth - scLargeWidth), "Large Subcontinent Placement - Terra Lua");
	local scWestX = eurasiaWestX + scRoll;
	local scSouthY = math.floor(self.iH * subcontinentLargeSouthLat);

	local scShape = Map.Rand(3, "Large Subcontinent Shape - Terra Lua");
	local scWater = 63;
	local scGrain = 2;
	local scRift = 2;
	if scShape > 0 then -- Massive subcontinent! (Africa style)
		scWater = 53;
		scGrain = 1;
		scRift = 2;
	else -- Standard subcontinent.
	end

	water = scWater + subcontinent_change;

	args = {};
	args.iWaterPercent = water;
	args.iRegionWidth = scLargeWidth;
	args.iRegionHeight = scLargeHeight;
	args.iRegionWestX = scWestX;
	args.iRegionSouthY = scSouthY;
	args.iRegionGrain = scGrain;
	args.iRegionHillsGrain = archGrain;
	args.iRegionFracYExp = 6;
	args.iRiftGrain = scRift;

	self:GenerateFractalLayerWithoutHills(args);

	local scsWestX = 0;
	local scSmallWidth = math.floor(subcontinentSmallDimension * self.iW);
	local scSmallHeight = math.floor(subcontinentSmallDimension * self.iH);
	local endless = 1;
	while endless == 1 do -- Prevent excessive overlap of the two subcontinents.
		local scsRoll = Map.Rand((eurasiaWidth - scSmallWidth), "Small Subcontinent Placement - Terra Lua");
		scsWestX = eurasiaWestX + scsRoll;
		if math.abs((scsWestX + self.iW / 12) - scWestX) > self.iW / 8 then
			break;
		end
	end
	local scsSouthY = math.floor(self.iH * subcontinentSmallSouthLat);

	local scsShape = Map.Rand(4, "Small Subcontinent Shape - Terra Lua");
	local scsWater = 75;
	local scsGrain = archGrain;
	local scsRift = -1;
	if scsShape == 2 then -- Major subcontinent!
		scsWater = 55;
		scsGrain = 1;
		scsRift = 2;
	elseif scsShape == 1 then -- Standard subcontinent. (India style).
		scsWater = 66;
		scsGrain = 2;
		scsRift = 2;
	else -- scsShape == 0 or 3, Archipelago subcontinent (East Indies style).
	end

	water = scsWater + subcontinent_change;

	args = {};
	args.iWaterPercent = water;
	args.iRegionWidth = scSmallWidth;
	args.iRegionHeight = scSmallHeight;
	args.iRegionWestX = scsWestX;
	args.iRegionSouthY = scsSouthY;
	args.iRegionGrain = scsGrain;
	args.iRegionHillsGrain = archGrain;
	args.iRegionPlotFlags = self.iHorzFlags;
	args.iRiftGrain = scsRift;

	self:GenerateFractalLayerWithoutHills(args);

	-- Now simulate random lands akin to Australia and Antarctica
	local extras = 2 + Map.Rand(3, "Number of Minor Regions - Terra Lua");
	for _ = 0, extras do
		-- Two to four of these regions.
		twWidth = math.floor(thirdworldDimension * self.iW);
		twHeight = math.floor(thirdworldDimension * self.iH);
		local twVertRange = math.floor(0.3 * self.iH) - twHeight;
		local twRoll = Map.Rand((eurasiaWidth - twWidth), "Minor Region Placement - Terra Lua");
		twWestX = eurasiaWestX + twRoll;
		local twVertRoll = Map.Rand(twVertRange, "Minor Region Placement - Terra Lua");
		twSouthY = math.floor(self.iH * thirdworldSouthLat) + twVertRoll;

		local twShape = Map.Rand(3, "Minor Region Shape - Terra Lua");
		local twWater = 70;
		local twGrain = archGrain;
		local twRift = -1;
		if twShape == 2 then -- Massive subcontinent!
			twWater = 60;
			twGrain = 1;
			twRift = 2;
		elseif twShape == 1 then -- Standard subcontinent.
			twWater = 65;
			twGrain = 2;
			twRift = 2;
		else -- twShape == 0, Archipelago subcontinent.
		end

		water = twWater + subcontinent_change;

		args = {};
		args.iWaterPercent = water;
		args.iRegionWidth = twWidth;
		args.iRegionHeight = twHeight;
		args.iRegionWestX = twWestX;
		args.iRegionSouthY = twSouthY;
		args.iRegionGrain = twGrain;
		args.iRegionHillsGrain = archGrain;
		args.iRegionPlotFlags = self.iHorzFlags;
		args.iRiftGrain = twRift;

		self:GenerateFractalLayerWithoutHills(args);
	end

	-- All regions have been processed. Now apply hills and mountains.
	local world_age = Map.GetCustomOption(1);
	if world_age == 4 then
		world_age = 1 + Map.Rand(3, "Random World Age - Lua");
	end
	args = {
		world_age = world_age,
		adjust_plates = 1.35,
		tectonic_islands = true,
	};
	self:ApplyTectonics(args);

	-- Plot Type generation completed. Return global plot array.
	return self.wholeworldPlotTypes;
end
------------------------------------------------------------------------------

--[[
Regional Variables Key:

iWaterPercent,				DEFAULT: 55
iRegionWidth,				MANDATORY (no default)
iRegionHeight,				MANDATORY (no default)
iRegionWestX,				MANDATORY (no default)
iRegionSouthY,				MANDATORY (no default)
iRegionGrain,				DEFAULT: 1
iRegionHillsGrain,			DEFAULT: 3
iRegionPlotFlags,			DEFAULT: iRoundFlags
iRegionFracXExp,			DEFAULT: 6
iRegionFracYExp,			DEFAULT: 5
iRiftGrain,					DEFAULT: -1 (no rifts)
bShift,						DEFAULT: true
--]]

------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Setting Plot Types (Lua Terra) ...");

	local layered_world = MultilayeredFractal.Create();
	local plotsTerra = layered_world:GeneratePlotsByRegion();

	SetPlotTypes(plotsTerra);
	GenerateCoasts();
end
------------------------------------------------------------------------------
function GenerateTerrain()
	print("Generating Terrain (Lua Terra) ...");

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
function AddFeatures()
	print("Adding Features (Lua Terra) ...");

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
function AssignStartingPlots:GetNumCityStatesPerRegion()
	local ratio = self.iNumCityStates / self.iNumCivs;
	if ratio >= 3.5 then
		return 2;
	elseif ratio >= 2 then
		return 1;
	else
		return 0;
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetNumCityStatesInUninhabitedRegion()
	local max_by_method = math.ceil(self.iNumCityStates * 0.67);
	return math.min(self.iNumCityStatesUnassigned, max_by_method);
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetRandomLuxuriesTargetNumber()
	-- MOD.Barathor:
	-- Terra uses much larger map sizes. Standard maps are the size of Large, Large the size of Huge, etc. but player totals remain the same.
	-- So, random luxury totals are increased.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 7,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 9,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 10,
	};
	local maxRandoms = worldsizes[Map.GetWorldSize()];
	return maxRandoms;
end
------------------------------------------------------------------------------
-- MOD.Barathor: Note: when I fix the system which determines random luxury totals,
-- I'll come back to this and add Abundant and Sparse values too, since they're missing from this overriding function.
-- (Currently, they all use this single set of values.)
function AssignStartingPlots:GetWorldLuxuryTargetNumbers()
	-- Because Terra has extra land over normal worlds, increasing the world target to provide more random resources and populate the New World with more resources.
	-- The first number is the target for total luxuries in the world, NOT counting the one-per-civ "second type" added at start locations.
	-- The second number affects minimum number of random luxuries placed.
	-- I say "affects" because it is only one part of the formula.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {21, 3},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {36, 4},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {52, 4},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {70, 5},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {90, 5},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {110, 6},
	};
	return worldsizes[Map.GetWorldSize()];
end
------------------------------------------------------------------------------
function StartPlotSystem()
	-- Get Resources setting input by user.
	local res = Map.GetCustomOption(4);
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

	-- tell the AI that we should treat this as a offshore expansion map
	Map.ChangeAIMapHint(4);
end
------------------------------------------------------------------------------
