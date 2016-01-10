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
--[[ Terra is still one of my favorite scripts. The main change to this map
type in Civ5 is the presence of the City States. No longer is the New World a
rats nest of Barbarian Cities, but instead now a home to numerous City States.

Civilizations start in the Old World. - Bob Thomas, April 2010  ]]--
------------------------------------------------------------------------------
function GetMapScriptInfo()
	local world_age, temperature, rainfall, sea_level, resources = GetCoreMapOptions()
	return {
		Name = "TXT_KEY_MAP_TERRA",
		Description = "TXT_KEY_MAP_TERRA_HELP",
		IsAdvancedMap = false,
		SupportsMultiplayer = true,
		IconIndex = 3,
		CustomOptions = {world_age, temperature, rainfall, resources},
	}
end
------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	-- This function can reset map grid sizes or world wrap settings.
	--
	-- Lakes is a world without oceans, so use grid sizes two levels below normal.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {52, 32},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {64, 40},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {84, 52},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {104, 64},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {128, 80},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {152, 96}
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
-- Terra uses the MultilayeredFractal for plot generation.
------------------------------------------------------------------------------
function MultilayeredFractal:GeneratePlotsByRegion()
	-- Sirian's MultilayeredFractal controlling function.
	-- You -MUST- customize this function for each script using MultilayeredFractal.
	--
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
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 5
		}
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
	--args.iRegionGrain
	args.iRegionHillsGrain = archGrain;
	args.iRegionPlotFlags = self.iHorzFlags;
	args.iRegionFracXExp = 7;
	args.iRegionFracYExp = 6;
	args.iRiftGrain = 2;
	--args.bShift
	
	self:GenerateFractalLayerWithoutHills(args)
	
	-- Eurasia, second layer (to increase pangaea-like cohesion).
	local twHeight = math.floor(eurasiaHeight/2);
	local twWestX = eurasiaWestX + math.floor(eurasiaWidth/10);
	local twEastX = eurasiaEastX - math.floor(eurasiaWidth/10);
	local twWidth = twEastX - twWestX + 1;
	local twNorthY = eurasiaNorthY - math.floor(eurasiaHeight/4);
	local twSouthY = eurasiaSouthY + math.floor(eurasiaHeight/4);

	local water = 60 + continent_change;

	args = {};
	args.iWaterPercent = water;
	args.iRegionWidth = twWidth;
	args.iRegionHeight = twHeight;
	args.iRegionWestX = twWestX;
	args.iRegionSouthY = twSouthY;
	--args.iRegionGrain
	args.iRegionHillsGrain = archGrain;
	args.iRegionPlotFlags = self.iHorzFlags;
	args.iRegionFracXExp = 7;
	args.iRegionFracYExp = 6;
	args.iRiftGrain = 2;
	--args.bShift
	
	self:GenerateFractalLayerWithoutHills(args)
	

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
	--args.iRegionGrain
	args.iRegionHillsGrain = archGrain;
	args.iRegionPlotFlags = self.iVertFlags;
	--args.iRegionFracXExp
	args.iRegionFracYExp = 6;
	--args.iRiftGrain
	--args.bShift
	
	self:GenerateFractalLayerWithoutHills(args)
	
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
	--args.iRegionGrain
	args.iRegionHillsGrain = archGrain;
	--args.iRegionPlotFlags
	--args.iRegionFracXExp
	args.iRegionFracYExp = 6;
	--args.iRiftGrain
	--args.bShift
	
	self:GenerateFractalLayerWithoutHills(args)
	
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
	--args.iRegionGrain
	args.iRegionHillsGrain = archGrain;
	args.iRegionPlotFlags = self.iVertFlags;
	--args.iRegionFracXExp
	--args.iRegionFracYExp 
	--args.iRiftGrain
	--args.bShift
	
	self:GenerateFractalLayerWithoutHills(args)
	
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
	--args.iRegionFracXExp
	--args.iRegionFracYExp 
	--args.iRiftGrain
	--args.bShift

	self:GenerateFractalLayerWithoutHills(args)
	
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
	--args.iRegionFracXExp
	--args.iRegionFracYExp 
	--args.iRiftGrain
	--args.bShift
	
	self:GenerateFractalLayerWithoutHills(args)
	
	-- Now simulate Central America
	local nwcVar = 0.0;
	if nwsRoll == 1 then
		nwcVar = 0.04;
	end
	local nwcWidth = math.floor(self.iW * 0.06);
	local nwcRoll = Map.Rand(2, "Central America and Carribean Placement - Terra Lua");
	local nwcWestX = nwWestX + math.floor(self.iW * (0.1 + nwcVar));
	local nwcEastX = nwcWestX + nwcWidth;
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
	--args.iRegionGrain 
	args.iRegionHillsGrain = archGrain;
	args.iRegionPlotFlags = self.iVertFlags;
	--args.iRegionFracXExp
	--args.iRegionFracYExp 
	--args.iRiftGrain
	--args.bShift

	self:GenerateFractalLayerWithoutHills(args)
	
	-- Now the Carribean islands
	local carVar = 0.0;
	if nwsRoll == 1 then
		carVar = 0.15;
	end
	local twWidth = math.floor(0.15 * self.iW);
	local twEastX = nwEastX - math.floor(carVar * self.iW);
	local twWestX = twEastX - twWidth;
	local twNorthY = math.floor(self.iH * 0.55);
	local twSouthY = math.floor(self.iH * 0.47);
	local twHeight = twNorthY - twSouthY + 1;

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
	--args.iRegionFracXExp
	--args.iRegionFracYExp 
	--args.iRiftGrain
	--args.bShift
	
	self:GenerateFractalLayerWithoutHills(args)
	

	-- Add subcontinents to the Old World, one large, one small.
	-- Subcontinents can be akin to pangaea, continents, or archipelago.
	-- The large adds an amount of land akin to subSaharan Africa.
	-- The small adds an amount of land akin to South Pacific islands.
	print("Generating the Third World (Lua Terra) ...");
	local scLargeWidth = math.floor(subcontinentLargeHorz * self.iW);
	local scLargeHeight = math.floor(subcontinentLargeVert * self.iH);
	local scRoll = Map.Rand((eurasiaWidth - scLargeWidth), "Large Subcontinent Placement - Terra Lua");
	local scWestX = eurasiaWestX + scRoll;
	local scEastX = scWestX + scLargeWidth;
	local scNorthY = math.floor(self.iH * subcontinentLargeNorthLat);
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
	--args.iRegionPlotFlags
	--args.iRegionFracXExp
	args.iRegionFracYExp = 6;
	args.iRiftGrain = scRift;
	--args.bShift
	
	self:GenerateFractalLayerWithoutHills(args)
	
	local scsWestX = 0;
	local scSmallWidth = math.floor(subcontinentSmallDimension * self.iW);
	local scSmallHeight = math.floor(subcontinentSmallDimension * self.iH);
	local endless = 1;
	while endless == 1 do -- Prevent excessive overlap of the two subcontinents.
		local scsRoll = Map.Rand((eurasiaWidth - scSmallWidth), "Small Subcontinent Placement - Terra Lua");
		scsWestX = eurasiaWestX + scsRoll;
		if math.abs((scsWestX + self.iW/12) - scWestX) > self.iW/8 then
			break
		end
	end
	local scsEastX = scsWestX + scSmallWidth;
	local scsNorthY = math.floor(self.iH * subcontinentSmallNorthLat);
	local scsSouthY = math.floor(self.iH * subcontinentSmallSouthLat);
	--
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
	--args.iRegionFracXExp
	--args.iRegionFracYExp
	args.iRiftGrain = scsRift;
	--args.bShift
	
	self:GenerateFractalLayerWithoutHills(args)
	

	-- Now simulate random lands akin to Australia and Antarctica
	local extras = 2 + Map.Rand(3, "Number of Minor Regions - Terra Lua");
	for loop = 0, extras do
		-- Two to four of these regions.
		local twWidth = math.floor(thirdworldDimension * self.iW);
		local twHeight = math.floor(thirdworldDimension * self.iH);
		local twVertRange = math.floor(0.3 * self.iH) - twHeight;
		local twRoll = Map.Rand((eurasiaWidth - twWidth), "Minor Region Placement - Terra Lua");
		local twWestX = eurasiaWestX + twRoll;
		local twEastX = scWestX + scLargeWidth;
		local twVertRoll = Map.Rand(twVertRange, "Minor Region Placement - Terra Lua");
		local twNorthY = math.floor(self.iH * thirdworldNorthLat) + twVertRoll;
		local twSouthY = math.floor(self.iH * thirdworldSouthLat) + twVertRoll;

		local twShape = Map.Rand(3, "Minor Region Shape - Terra Lua");
		twWater = 70;
		twGrain = archGrain;
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
		--args.iRegionFracXExp
		--args.iRegionFracYExp
		args.iRiftGrain = twRift;
		--args.bShift
	
		self:GenerateFractalLayerWithoutHills(args)
	end

	-- All regions have been processed. Now apply hills and mountains.
	local world_age = Map.GetCustomOption(1)
	if world_age == 4 then
		world_age = 1 + Map.Rand(3, "Random World Age - Lua");
	end
	local args = {
		world_age = world_age,
		adjust_plates = 1.35,
		tectonic_islands = true,
	};
	self:ApplyTectonics(args)

	-- Plot Type generation completed. Return global plot array.
	return self.wholeworldPlotTypes
end
------------------------------------------------------------------------------

--[[ ----------------------------------------------
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
---------------------------------------------- ]]--

------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Setting Plot Types (Lua Terra) ...");

	local layered_world = MultilayeredFractal.Create();
	local plotsTerra = layered_world:GeneratePlotsByRegion();
	
	SetPlotTypes(plotsTerra);
	GenerateCoasts()
end
------------------------------------------------------------------------------
function GenerateTerrain()
	print("Generating Terrain (Lua Terra) ...");
	
	-- Get Temperature setting input by user.
	local temp = Map.GetCustomOption(2)
	if temp == 4 then
		temp = 1 + Map.Rand(3, "Random Temperature - Lua");
	end

	local args = {temperature = temp};
	local terraingen = TerrainGenerator.Create(args);

	local terrainTypes = terraingen:GenerateTerrain()
		
	SetTerrainTypes(terrainTypes);
end
------------------------------------------------------------------------------
function AddFeatures()
	print("Adding Features (Lua Terra) ...");
	
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

-- Customizing natural wonder eligibility for Terra
------------------------------------------------------------------------------
function AssignStartingPlots:CanBeGeyser(x, y)
	-- Checks a candidate plot for eligibility to be the Geyser.
	local plot = Map.GetPlot(x, y);
	-- Checking center plot, which must be at least two plots away from any salt water, and not be in the desert or tundra.
	if plot:IsWater() then
		return
	end
	local iW, iH = Map.GetGridSize();
	local plotIndex = y * iW + x + 1;
	if self.plotDataIsCoastal[plotIndex] == true or self.plotDataIsNextToCoast[plotIndex] == true then
		return
	end
	local terrainType = plot:GetTerrainType()
	if terrainType == TerrainTypes.TERRAIN_DESERT or terrainType == TerrainTypes.TERRAIN_TUNDRA then
		return
	end
	local iNumMountains, iNumHills, iNumDeserts, iNumTundra, bBiggestLandmass = 0, 0, 0, 0, false;
	local plotType = plot:GetPlotType();
	if plotType == PlotTypes.PLOT_MOUNTAIN then
		iNumMountains = iNumMountains + 1;
	elseif plotType == PlotTypes.PLOT_HILLS then
		iNumHills = iNumHills + 1;
	end
	local iAreaID = plot:GetArea();
	if iAreaID == self.iBiggestLandmassID then
		bBiggestLandmass = true;
	end
	-- Now process the surrounding plots.
	for loop, direction in ipairs(self.direction_types) do
		local adjPlot = Map.PlotDirection(x, y, direction)
		if adjPlot:IsLake() then
			return
		end
		terrainType = adjPlot:GetTerrainType()
		if terrainType == TerrainTypes.TERRAIN_TUNDRA then
			iNumTundra = iNumTundra + 1;
		elseif terrainType == TerrainTypes.TERRAIN_DESERT then
			iNumDeserts = iNumDeserts + 1;
		end
		plotType = adjPlot:GetPlotType();
		if plotType == PlotTypes.PLOT_MOUNTAIN then
			iNumMountains = iNumMountains + 1;
		elseif plotType == PlotTypes.PLOT_HILLS then
			iNumHills = iNumHills + 1;
		end
		local adjAreaID = adjPlot:GetArea();
		if adjAreaID == self.iBiggestLandmassID then
			bBiggestLandmass = true;
		end
	end
	if bBiggestLandmass == false then
		return
	end
	-- If too many deserts, tundra or mountains, reject this site.
	if iNumDeserts > 2 or iNumTundra > 2 or iNumMountains > 5 then
		return
	end
	-- If not enough hills or mountains, reject this site.
	if iNumMountains < 1 and iNumHills < 4 then
		return
	end
	-- This site is inland, has hills or mountains, not too many deserts, and not too many mountains, so it's good.
	table.insert(self.geyser_list, plotIndex);
end
------------------------------------------------------------------------------
function AssignStartingPlots:CanBeCrater(x, y)
	-- Checks a candidate plot for eligibility to be the Crater.
	local plot = Map.GetPlot(x, y);
	-- Checking center plot, which must be at least one plot away from any salt water, and it wants to be in the desert or the tundra.
	if plot:IsWater() then
		return
	end
	local iAreaID = plot:GetArea();
	if iAreaID == self.iBiggestLandmassID then
		return
	end
	local iW, iH = Map.GetGridSize();
	local plotIndex = y * iW + x + 1;
	if self.plotDataIsCoastal[plotIndex] == true then
		return
	end
	local terrainType = plot:GetTerrainType()
	if not (terrainType == TerrainTypes.TERRAIN_DESERT or terrainType == TerrainTypes.TERRAIN_TUNDRA) then
		return
	end
	local iNumMountains, iNumHills = 0, 0;
	local plotType = plot:GetPlotType();
	if plotType == PlotTypes.PLOT_MOUNTAINS then
		iNumMountains = iNumMountains + 1;
	elseif plotType == PlotTypes.PLOT_HILLS then
		iNumHills = iNumHills + 1;
	end
	-- Now process the surrounding plots.
	for loop, direction in ipairs(self.direction_types) do
		local adjPlot = Map.PlotDirection(x, y, direction)
		if adjPlot:IsLake() then
			return
		end
		local adjAreaID = adjPlot:GetArea();
		if adjAreaID == self.iBiggestLandmassID then
			return
		end
		terrainType = adjPlot:GetTerrainType()
		if terrainType == TerrainTypes.TERRAIN_GRASS then -- Grass is unacceptable.
			return
		end
		plotType = adjPlot:GetPlotType();
		if plotType == PlotTypes.PLOT_MOUNTAINS then
			iNumMountains = iNumMountains + 1;
		elseif plotType == PlotTypes.PLOT_HILLS then
			iNumHills = iNumHills + 1;
		end
	end
	-- If too many hills or mountains, reject this site.
	if iNumMountains > 1 or iNumHills + iNumMountains > 3 then
		return
	end
	-- This site is inland, in desert or tundra with no grass around, and does not have too many hills and mountains, so it's good.
	table.insert(self.crater_list, plotIndex);
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
		local plotType = adjPlot:GetPlotType();
		local terrainType = adjPlot:GetTerrainType()
		local featureType = adjPlot:GetFeatureType()
		if terrainType == TerrainTypes.TERRAIN_COAST and plot:IsLake() == false then
			if featureType == FeatureTypes.NO_FEATURE then
				iNumCoast = iNumCoast + 1;
			end
		end
		if plotType ~= PlotTypes.PLOT_OCEAN then
			local iAreaID = adjPlot:GetArea();
			if iAreaID == self.iBiggestLandmassID then
				iNumLand = iNumLand + 1;
			end
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
function AssignStartingPlots:CanBeFuji(x, y)
	-- Checks a candidate plot for eligibility to be Mount Fuji.
	local iW, iH = Map.GetGridSize();
	local plot = Map.GetPlot(x, y);
	local plotType = plot:GetPlotType();
	-- Checking center plot, which must not be on the biggest landmess (unless there are no oceans) or on too small of an island.
	-- Nor do we want it near other mountains and hills, and it must not be in desert or tundra.
	if plotType ~= PlotTypes.PLOT_LAND then
		return
	end
	local iAreaID = plot:GetArea();
	if iAreaID ~= self.iBiggestLandmassID then
		return
	end
	local plotIndex = y * iW + x + 1;
	if self.plotDataIsCoastal[plotIndex] == true then
		return
	end
	local terrainType = plot:GetTerrainType()
	if terrainType == TerrainTypes.TERRAIN_DESERT or terrainType == TerrainTypes.TERRAIN_TUNDRA then
		return
	end
	local iNumHills = 0;
	-- Now process the surrounding plots.
	for loop, direction in ipairs(self.direction_types) do
		local adjPlot = Map.PlotDirection(x, y, direction)
		if adjPlot:IsLake() then
			return
		end
		terrainType = adjPlot:GetTerrainType()
		if terrainType == TerrainTypes.TERRAIN_DESERT or terrainType == TerrainTypes.TERRAIN_TUNDRA then
			return
		end
		local featureType = adjPlot:GetFeatureType()
		if featureType == FeatureTypes.FEATURE_MARSH then
			return
		end
		plotType = adjPlot:GetPlotType();
		if plotType == PlotTypes.PLOT_MOUNTAINS then
			return
		elseif plotType == PlotTypes.PLOT_HILLS then
			iNumHills = iNumHills + 1;
		end
	end
	-- If too many hills, reject this site.
	if iNumHills > 1 then
		return
	end
	-- This site is on an eligible landmass, in grassland or plains, with no more than one Hills nearby, so it's good.
	table.insert(self.fuji_list, plotIndex);
end
------------------------------------------------------------------------------
function AssignStartingPlots:CanBeMesa(x, y)
	-- Checks a candidate plot for eligibility to be the Mesa.
	local plot = Map.GetPlot(x, y);
	-- Checking center plot, which must be at least one plot away from any salt water.
	if plot:IsWater() then
		return
	end
	local iAreaID = plot:GetArea();
	if iAreaID == self.iBiggestLandmassID then
		return
	end
	local iW, iH = Map.GetGridSize();
	local plotIndex = y * iW + x + 1;
	if self.plotDataIsCoastal[plotIndex] == true then
		return
	end
	local terrainType = plot:GetTerrainType()
	if terrainType == TerrainTypes.TERRAIN_GRASS or terrainType == TerrainTypes.TERRAIN_TUNDRA then -- Rejecting grass or tundra.
		return
	end
	local iNumMountains, iNumHills = 0, 0;
	local plotType = plot:GetPlotType();
	if plotType == PlotTypes.PLOT_MOUNTAINS then
		iNumMountains = iNumMountains + 1;
	elseif plotType == PlotTypes.PLOT_HILLS then
		iNumHills = iNumHills + 1;
	end
	-- Now process the surrounding plots.
	for loop, direction in ipairs(self.direction_types) do
		local adjPlot = Map.PlotDirection(x, y, direction)
		if adjPlot:IsLake() then
			return
		end
		local adjAreaID = adjPlot:GetArea();
		if adjAreaID == self.iBiggestLandmassID then
			return
		end
		terrainType = adjPlot:GetTerrainType()
		if terrainType == TerrainTypes.TERRAIN_GRASS or terrainType == TerrainTypes.TERRAIN_TUNDRA then
			return
		end
		plotType = adjPlot:GetPlotType();
		if plotType == PlotTypes.PLOT_MOUNTAINS then
			iNumMountains = iNumMountains + 1;
		elseif plotType == PlotTypes.PLOT_HILLS then
			iNumHills = iNumHills + 1;
		end
	end
	-- If too many mountains, reject this site.
	if iNumMountains > 3 then
		return
	end
	-- If not enough hills, reject this site.
	if iNumHills < 2 then
		return
	end
	-- This site is inland, in desert or plains with no grass or tundra around, and has a moderate amount of hills and mountains.
	table.insert(self.mesa_list, plotIndex);
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function AssignStartingPlots:AssignCityStatesToRegionsOrToUninhabited(args)
	-- Customizing City State assignment for Terra, to get more CS in the new world.
	--
	-- Placement methods include:
	-- 1. Assign n Per Region
	-- 2. Assign to uninhabited landmasses
	-- 3. Assign to regions with shared luxury IDs
	-- 4. Assign to low fertility regions

	-- Determine number to assign Per Region
	local iW, iH = Map.GetGridSize()
	local ratio = self.iNumCityStates / self.iNumCivs;
	if ratio >= 3.5 then
		self.iNumCityStatesPerRegion = 2;
	elseif ratio >= 2 then
		self.iNumCityStatesPerRegion = 1;
	else
		self.iNumCityStatesPerRegion = 0;
	end
	-- Assign the "Per Region" City States to their regions.
	--print("- - - - - - - - - - - - - - - - -"); print("Assigning City States to Regions");
	local current_cs_index = 1;
	if self.iNumCityStatesPerRegion > 0 then
		for current_region = 1, self.iNumCivs do
			for cs_to_assign_to_this_region = 1, self.iNumCityStatesPerRegion do
				self.city_state_region_assignments[current_cs_index] = current_region;
				--print("-"); print("City State", current_cs_index, "assigned to Region#", current_region);
				current_cs_index = current_cs_index + 1;
				self.iNumCityStatesUnassigned = self.iNumCityStatesUnassigned - 1;
			end
		end
	end

	-- Determine how many City States to place on uninhabited landmasses.
	-- Also generate lists of candidate plots from uninhabited areas.
	local iNumLandAreas = 0;
	local iNumCivLandmassPlots = 0;
	local iNumUninhabitedLandmassPlots = 0;
	local land_area_IDs = {};
	local land_area_plot_count = {};
	local land_area_plot_tables = {};
	local areas_inhabited_by_civs = {};
	local areas_too_small = {};
	local areas_uninhabited = {};
	--
	-- Generate list of inhabited area IDs.
	for index, region_data in ipairs(self.regionData) do
		local region_areaID = region_data[5];
		if TestMembership(areas_inhabited_by_civs, region_areaID) == false then
			table.insert(areas_inhabited_by_civs, region_areaID);
		end
	end
	-- Iterate through plots and, for each land area, generate a list of all its member plots
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local plotIndex = y * iW + x + 1;
			local plot = Map.GetPlot(x, y);
			local plotType = plot:GetPlotType()
			local terrainType = plot:GetTerrainType()
			if (plotType == PlotTypes.PLOT_LAND or plotType == PlotTypes.PLOT_HILLS) and terrainType ~= TerrainTypes.TERRAIN_SNOW then -- Habitable land plot, process it.
				local iArea = plot:GetArea();
				if TestMembership(land_area_IDs, iArea) == false then -- This plot is the first detected in its AreaID.
					iNumLandAreas = iNumLandAreas + 1;
					table.insert(land_area_IDs, iArea);
					land_area_plot_count[iArea] = 1;
					land_area_plot_tables[iArea] = {plotIndex};
				else -- This AreaID already known.
					land_area_plot_count[iArea] = land_area_plot_count[iArea] + 1;
					table.insert(land_area_plot_tables[iArea], plotIndex);
				end
			end
		end
	end
	-- Obtain counts of inhabited and uninhabited plots. Identify areas too small to use for City States.
	for areaID, plot_count in pairs(land_area_plot_count) do
		if TestMembership(areas_inhabited_by_civs, areaID) == true then 
			iNumCivLandmassPlots = iNumCivLandmassPlots + plot_count;
		else
			iNumUninhabitedLandmassPlots = iNumUninhabitedLandmassPlots + plot_count;
			if plot_count < 4 then
				table.insert(areas_too_small, areaID);
			else
				table.insert(areas_uninhabited, areaID);
			end
		end
	end
	-- Now loop through all Uninhabited Areas that are large enough to use and append their plots to the candidates tables.
	for areaID, area_plot_list in pairs(land_area_plot_tables) do
		if TestMembership(areas_uninhabited, areaID) == true then 
			for loop, plotIndex in ipairs(area_plot_list) do
				local x = (plotIndex - 1) % iW;
				local y = (plotIndex - x - 1) / iW;
				local plot = Map.GetPlot(x, y);
				local terrainType = plot:GetTerrainType();
				if terrainType ~= TerrainTypes.TERRAIN_SNOW then
					if self.plotDataIsCoastal[plotIndex] == true then
						table.insert(self.uninhabited_areas_coastal_plots, plotIndex);
					else
						table.insert(self.uninhabited_areas_inland_plots, plotIndex);
					end
				end
			end
		end
	end
	-- Determine the number of City States to assign to uninhabited areas.
	local max_by_method = math.ceil(self.iNumCityStates * 0.67);
	self.iNumCityStatesUninhabited = math.min(self.iNumCityStatesUnassigned, max_by_method);
	self.iNumCityStatesUnassigned = self.iNumCityStatesUnassigned - self.iNumCityStatesUninhabited;

	--print("-"); print("City States assigned to Uninhabited Areas: ", self.iNumCityStatesUninhabited);
	-- Update the city state number.
	current_cs_index = current_cs_index + self.iNumCityStatesUninhabited;
	
	if self.iNumCityStatesUnassigned > 0 then
		-- Determine how many to place in support of regions that share their luxury type with two other regions.
		local iNumRegionsSharedLux = 0;
		local shared_lux_IDs = {};
		for resource_ID, amount_assigned_to_regions in ipairs(self.luxury_assignment_count) do
			if amount_assigned_to_regions == 3 then
				iNumRegionsSharedLux = iNumRegionsSharedLux + 3;
				table.insert(shared_lux_IDs, resource_ID);
			end
		end
		if iNumRegionsSharedLux > 0 and iNumRegionsSharedLux <= self.iNumCityStatesUnassigned then
			self.iNumCityStatesSharedLux = iNumRegionsSharedLux;
			self.iNumCityStatesLowFertility = self.iNumCityStatesUnassigned - self.iNumCityStatesSharedLux;
		else
			self.iNumCityStatesLowFertility = self.iNumCityStatesUnassigned;
		end
		--print("CS Shared Lux: ", self.iNumCityStatesSharedLux, " CS Low Fert: ", self.iNumCityStatesLowFertility);
		-- Assign remaining types to their respective regions.
		if self.iNumCityStatesSharedLux > 0 then
			for loop, res_ID in ipairs(shared_lux_IDs) do
				for loop, region_lux_data in ipairs(self.regions_sorted_by_type) do
					local this_region_res = region_lux_data[2];
					if this_region_res == res_ID then
						self.city_state_region_assignments[current_cs_index] = region_lux_data[1];
						--print("-"); print("City State", current_cs_index, "assigned to Region#", region_lux_data[1], " to compensate for Shared Luxury ID#", res_ID);
						current_cs_index = current_cs_index + 1;
						self.iNumCityStatesUnassigned = self.iNumCityStatesUnassigned - 1;
					end
				end
			end
		end
		if self.iNumCityStatesLowFertility > 0 then
			-- If more to assign than number of regions, assign per region.
			while self.iNumCityStatesUnassigned >= self.iNumCivs do
				for current_region = 1, self.iNumCivs do
					self.city_state_region_assignments[current_cs_index] = current_region;
					--print("-"); print("City State", current_cs_index, "assigned to Region#", current_region, " to compensate for Low Fertility");
					current_cs_index = current_cs_index + 1;
					self.iNumCityStatesUnassigned = self.iNumCityStatesUnassigned - 1;
				end
			end
			if self.iNumCityStatesUnassigned > 0 then
				local fert_unsorted, fert_sorted, region_list = {}, {}, {};
				for region_num = 1, self.iNumCivs do
					local area_plots = self.regionTerrainCounts[region_num][2];
					local region_fertility = self.regionData[region_num][6];
					local fertility_per_land_plot = region_fertility / area_plots;
					--print("-"); print("Region#", region_num, "AreaPlots:", area_plots, "Region Fertility:", region_fertility, "Per Plot:", fertility_per_land_plot);
					
					table.insert(fert_unsorted, {region_num, fertility_per_land_plot});
					table.insert(fert_sorted, fertility_per_land_plot);
				end
				table.sort(fert_sorted);
				for current_lowest_fertility, fert_value in ipairs(fert_sorted) do
					for loop, data_pair in ipairs(fert_unsorted) do
						local this_region_fert = data_pair[2];
						if this_region_fert == fert_value then
							local regionNum = data_pair[1];
							table.insert(region_list, regionNum);
							table.remove(fert_unsorted, loop);
							break
						end
					end
				end
				for loop = 1, self.iNumCityStatesUnassigned do
					self.city_state_region_assignments[current_cs_index] = region_list[loop];
					--print("-"); print("City State", current_cs_index, "assigned to Region#", region_list[loop], " to compensate for Low Fertility");
					current_cs_index = current_cs_index + 1;
					self.iNumCityStatesUnassigned = self.iNumCityStatesUnassigned - 1;
				end
			end
		end
	end
	
	--[[ Debug check
	if self.iNumCityStatesUnassigned ~= 0 then
		print("-"); print("Wrong number of City States assigned at end of assignment process. This number unassigned: ", self.iNumCityStatesUnassigned);
	else
		print("-"); print("All city states assigned.");
	end
	]]--
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetRandomLuxuriesTargetNumber()
	--[[ MOD.Barathor:
		 Terra uses much larger map sizes.  Standard maps are the size of Large, Large the size of Huge, etc. but player
		 totals remain the same.  So, random luxury totals are increased.  ]]
	local worldsizes = {							
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 7,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 9,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 10,
		}
	local maxRandoms = worldsizes[Map.GetWorldSize()]
	return maxRandoms
end
------------------------------------------------------------------------------
-- MOD.Barathor: This function is no longer needed since random luxury eligibility is determined differently.
--[[
function AssignStartingPlots:GetDisabledLuxuriesTargetNumber()
	-- Because Terra has extra land over normal worlds, reducing the Disabled 
	-- target by one per world size, where possible.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 1,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 0,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 0,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 0
		}
	local maxToDisable = worldsizes[Map.GetWorldSize()];
	return maxToDisable
end
--]]
------------------------------------------------------------------------------
--[[ MOD.Barathor: Note: when I fix the system which determines random luxury totals,
	 I'll come back to this and add Abundant and Sparse values too, since they're
	 missing from this overriding function. (Currently, they all use this single set of values.) ]]
function AssignStartingPlots:GetWorldLuxuryTargetNumbers()
	-- Because Terra has extra land over normal worlds, increasing the world target
	-- to provide more random resources and populate the New World with more resources.
	--
	-- The first number is the target for total luxuries in the world, NOT
	-- counting the one-per-civ "second type" added at start locations.
	--
	-- The second number affects minimum number of random luxuries placed.
	-- I say "affects" because it is only one part of the formula.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {21, 3},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {36, 4},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {52, 4},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {70, 5},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {90, 5},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {110, 6}
		}
	local world_size_data = worldsizes[Map.GetWorldSize()];
	return world_size_data
end
------------------------------------------------------------------------------
function StartPlotSystem()
	-- Get Resources setting input by user.
	local res = Map.GetCustomOption(4)
	if res == 6 then
		res = 1 + Map.Rand(3, "Random Resources Option - Lua");
	end

	print("Creating start plot database.");
	local start_plot_database = AssignStartingPlots.Create()
	
	print("Dividing the map in to Regions.");
	-- Regional Division Method 1: Biggest Landmass
	local args = {
		method = 1,
		resources = res,
		};
	start_plot_database:GenerateRegions(args)

	print("Choosing start locations for civilizations.");
	start_plot_database:ChooseLocations()
	
	print("Normalizing start locations and assigning them to Players.");
	start_plot_database:BalanceAndAssign()

	print("Placing Natural Wonders.");
	start_plot_database:PlaceNaturalWonders()

	print("Placing Resources and City States.");
	start_plot_database:PlaceResourcesAndCityStates()
	
	-- tell the AI that we should treat this as a offshore expansion map
	Map.ChangeAIMapHint(4);

end
------------------------------------------------------------------------------
