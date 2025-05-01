--[[

-- Communitu_79a map script --
-- For Vox Populi only --

  Created by:
- Cephalo (Rich Marinaccio)		- Perlin landform, elevation and rainfall creation
- Sirian (Bob Thomas)			- Island creation, some code from Continents and Terra scripts
- WHoward69						- Mountain-pass finding algorithm
- Bobert13						- Bug fixes and optimizations
- Thalassicus (Victor Isbell)	- Ocean rifts, rivers through lakes, natural wonder placement,
								  resource placement, map options, inland seas, aesthetic polishing
- tu_79							- Tweaked values for Vox Populi mod, clean deserts, reduced resources
- azum4roll						- Tweaked resource distribution even more and added more spawns according to Civilopedia

This map script generates climate based on a simplified model of geostrophic
and monsoon wind patterns. Rivers are generated along accurate drainage paths
governed by the elevation map used to create the landforms.

- Natural wonders appear in useful locations.
- Islands reward exploration and settlement.
- Ocean rifts prevent ancient ships from circling the world.
- Inland seas, lakes, and rivers flowing out of lakes.

--]]

include("MapGenerator");
include("FeatureGenerator");
include("TerrainGenerator");
include("IslandMaker");
include("FLuaVector");

MapGlobals = {};

local debugTime = false;
local debugPrint = true;
local elevationMap = {};
local rainfallMap = {};
local temperatureMap = {};
local riverMap = {};

--[[
Setting "overrideAssignStartingPlots = false" may help make the map compatible
with core game patches in the distant future when I'm no longer modding Civ 5.

This disables some advanced features of the map, so it's better to
modify the map's changes to AssignStartingPlots if possible.

~ Thalassicus @ Nov 5 2013

This is now a map option

~ azum4roll @ Sept 28 2019
--]]
local overrideAssignStartingPlots = true;

----------------------------
-- Map Information
----------------------------
function MapGlobals:New()
	print("MapGlobals:New");
	local mglobal = {};
	setmetatable(mglobal, self);
	self.__index = self;

	local mapW, _ = Map.GetGridSize();

	-- Percent of land tiles on the map.
	mglobal.landPercent = 0.40;

	-- Top and bottom map latitudes.
	mglobal.topLatitude = 90;
	mglobal.bottomLatitude = -mglobal.topLatitude;

	-- Important latitude markers used for generating climate.
	mglobal.tropicLatitudes = 25; -- tropicLatitudes to 0: grass, jungle
	mglobal.horseLatitudes = 50; -- polarFrontLatitude to horseLatitudes: plains, desert
	mglobal.iceLatitude = 65; -- bottomLatitude to iceLatitude: ice
	mglobal.polarFrontLatitude = 72; -- bottomLatitude to polarFrontLatitude: snow, tundra

	-- Adjusting these will generate larger or smaller landmasses and features.
	-- Higher values makes continental divisions and stringy features more likely,
	-- and very high values result in a lot of stringy continents and islands.
	mglobal.landMinScatter = 0.02; -- Recommended range: [0.02 to 0.1]
	mglobal.landMaxScatter = 0.04; -- Recommended range: [0.03 to 0.3]

	-- Higher values result in more islands and variance on landmasses and coastlines.
	mglobal.coastScatter = 0.02; -- Recommended range: [0.01 to 0.3]

	-- Lower values make large, long, mountain ranges. Higher values make sporadic mountainous features.
	mglobal.mountainScatter = 200 * mapW; -- Recommended range: [130 to 1000]

	--------------
	-- Terrain
	--------------
	mglobal.mountainWeight = 1; -- Weight of the mountain elevation map versus the coastline elevation map.
	mglobal.belowMountainPercent = 0.95; -- Percent of non-mountain land
	-- flatPercent to belowMountainPercent : hills
	mglobal.flatPercent = 0.65; -- Percent of flat land
	mglobal.hillsBlendPercent = 0.15; -- Chance for flat land to become hills per near mountain. Requires at least 2 near mountains.
	mglobal.terrainBlendRange = 2; -- range to smooth terrain (desert surrounded by plains turns to plains, etc)
	mglobal.terrainBlendRandom = 0.4; -- random modifier for terrain smoothing

	--------------
	-- Features
	--------------
	mglobal.featurePercent = 0.30; -- Percent of potential feature tiles that actually create a feature (marsh/jungle/forest)
	mglobal.featureWetVariance = 0.15; -- Percent chance increase if freshwater, decrease if dry (groups features near rivers)
	mglobal.islePercent = 0.20; -- Percent of coast tiles with an isle
	mglobal.atollNorthLatitudeLimit = 40;
	mglobal.atollSouthLatitudeLimit = 15;
	mglobal.atollMinDeepWaterNeighbors = 3; -- Minimum of adjacent ocean tiles for an atoll.
	mglobal.numNaturalWonders = 2 * (Map.GetWorldSize() + 1);

	--------------
	-- Rain
	--------------
	mglobal.marshPercent = 0.025; -- Percent chance increase for marsh from each nearby watery tile

	-- junglePercent to 1: marsh, jungle
	mglobal.junglePercent = 0.75;
	-- zeroTreesPercent to 1: forest
	mglobal.zeroTreesPercent = 0.30;
	-- plainsPercent to 1: heavy rain
	mglobal.plainsPercent = 0.70;
	-- desertPercent to plainsPercent: moderate rain
	mglobal.desertPercent = 0.15;
	-- 0 to desertPercent: arid

	--------------
	-- Temperature
	--------------

	-- Terrain
	-- hot desert, grass: desertMinTemperature to 1
	mglobal.desertMinTemperature = 0.60;
	-- grass, desert: jungleMinTemperature to desertMinTemperature
	mglobal.jungleMinTemperature = 0.48;
	-- grass, plains, cold desert: tundraTemperature to jungleMinTemperature
	mglobal.tundraTemperature = 0.30;
	-- grass, plains, tundra: snowTemperature to tundraTemperature
	mglobal.snowTemperature = 0.25;
	-- snow, tundra: 0 to snowTemperature

	-- Features
	-- trees: treesMinTemperature to 1
	mglobal.treesMinTemperature = 0.15;
	mglobal.forestRandomPercent = 0.07; -- Percent of barren flatland which randomly gets a forest
	mglobal.forestTundraPercent = 0.33; -- Percent of barren tundra which randomly gets a forest

	--------------
	-- Water
	--------------
	mglobal.riverPercent = 0.18; -- Percent of river junctions that are large enough to become rivers.
	mglobal.riverRainCheatFactor = 3.00; -- Values greater than one favor watershed size. Values less than one favor actual rain amount.
	mglobal.minWaterTemp = 0.10; -- Sets water temperature compression that creates the land/sea seasonal temperature differences that cause monsoon winds.
	mglobal.maxWaterTemp = 0.50;
	mglobal.geostrophicFactor = 5.0; -- Strength of latitude climate versus monsoon climate.
	mglobal.geostrophicLateralWindStrength = 0.3;
	mglobal.lakeSize = GameDefines.MIN_WATER_SIZE_FOR_OCEAN; -- read-only; cannot change lake sizes with a map script
	mglobal.oceanRiftWidth = 2 + math.floor(mapW / 60); -- minimum number of ocean tiles in a rift

	-- percent of map width
	mglobal.atlanticSize = 0.09; -- size near poles
	mglobal.atlanticBulge = -0.07; -- size increase at equator
	mglobal.pacificSize = 0.02; -- size near poles
	mglobal.pacificBulge = 0.10; -- size increase at equator

	-- percent of straightness
	mglobal.atlanticCurve = 0.30; -- deviance from the starting point
	mglobal.pacificCurve = 0.40;

	mglobal.atlanticSize = math.max(2, math.min(mapW / 2, Round(mglobal.atlanticSize * mapW)));
	mglobal.pacificSize = math.max(3, math.min(mapW / 2, Round(mglobal.pacificSize * mapW)));
	mglobal.atlanticBulge = Round(mglobal.atlanticBulge * mapW);
	mglobal.pacificBulge = Round(mglobal.pacificBulge * mapW);

	--------------
	-- Resources
	--------------
	mglobal.fishTargetFertility = 60; -- fish appear to create this average city fertility

	------------------------------------------------------------------------------------
	-- Quality vs Performance
	-- Lowering these reduces map quality and creation time.
	-- Try reducing these slightly if you experience crashes on huge maps.
	------------------------------------------------------------------------------------
	mglobal.tempBlendMaxRange = Round(mapW / 8); -- range to smooth temperature map
	mglobal.elevationBlendRange = Round(mapW / 8); -- range to smooth elevation map

	--[[
	MAP OPTIONS
	1 - world_age
	2 - temperature
	3 - rainfall
	4 - sea_level
	5 - Players Starting Locations
	6 - Ocean Shapes
	7 - Ocean Rift Width
	8 - Circumnavigation Path
	9 - Force Coastal Starts
	10 - Land Shapes
	11 - Resources in Starting Locations
	12 - Strategic Deposit Size
	13 - Bonus Resource Density
	14 - Strategic Resource Density
	15 - Luxury Resource Density
	16 - Override AssignStartingPlots
	--]]

	overrideAssignStartingPlots = (Map.GetCustomOption(16) == 2);

	-- World Age
	local oWorldAge = Map.GetCustomOption(1);
	if oWorldAge == 1 then
		print("Map Age: New");
		mglobal.belowMountainPercent = ModifyOdds(mglobal.belowMountainPercent, 1 / 1.1);
		mglobal.flatPercent = ModifyOdds(mglobal.flatPercent, 1 / 1.2);
	elseif oWorldAge == 3 then
		print("Map Age: Old");
		mglobal.belowMountainPercent = ModifyOdds(mglobal.belowMountainPercent, 1.1);
		mglobal.flatPercent = ModifyOdds(mglobal.flatPercent, 1.2);
	elseif oWorldAge == 4 then
		print("Map Age: Random");
		local worldAgeRand = PWRand();
		local higher = (1 == Map.Rand(2, "ModifyOddsRandom for world age"));
		mglobal.belowMountainPercent = ModifyOddsRandom(mglobal.belowMountainPercent, 0.1, worldAgeRand, higher);
		mglobal.flatPercent = ModifyOddsRandom(mglobal.flatPercent, 0.2, worldAgeRand, higher);
		print("worldAgeRand:", worldAgeRand);
		print("higher:", higher);
	else
		print("Map Age: Normal");
	end
	mglobal.mountainScatter = mglobal.mountainScatter * 0.00001;

	-- Temperature
	local oTemp = Map.GetCustomOption(2);
	if oTemp == 1 then
		print("Map Temp: Cool");
		mglobal.tropicLatitudes = ModifyOdds(mglobal.tropicLatitudes / mglobal.topLatitude, 2 / 3) * mglobal.topLatitude;
		mglobal.horseLatitudes = ModifyOdds(mglobal.horseLatitudes / mglobal.topLatitude, 2 / 3) * mglobal.topLatitude;
		mglobal.iceLatitude = ModifyOdds(mglobal.iceLatitude / mglobal.topLatitude, 2 / 3) * mglobal.topLatitude;
		mglobal.polarFrontLatitude = ModifyOdds(mglobal.polarFrontLatitude / mglobal.topLatitude, 2 / 3) * mglobal.topLatitude;
		mglobal.tundraTemperature = ModifyOdds(mglobal.tundraTemperature, 1.25);
		mglobal.snowTemperature = ModifyOdds(mglobal.snowTemperature, 1.25);
	elseif oTemp == 3 then
		print("Map Temp: Hot");
		mglobal.tropicLatitudes = ModifyOdds(mglobal.tropicLatitudes / mglobal.topLatitude, 1.5) * mglobal.topLatitude;
		mglobal.horseLatitudes = ModifyOdds(mglobal.horseLatitudes / mglobal.topLatitude, 1.5) * mglobal.topLatitude;
		mglobal.iceLatitude = ModifyOdds(mglobal.iceLatitude / mglobal.topLatitude, 1.5) * mglobal.topLatitude;
		mglobal.polarFrontLatitude = ModifyOdds(mglobal.polarFrontLatitude / mglobal.topLatitude, 1.5) * mglobal.topLatitude;
		mglobal.tundraTemperature = ModifyOdds(mglobal.tundraTemperature, 0.8);
		mglobal.snowTemperature = ModifyOdds(mglobal.snowTemperature, 0.8);
	elseif oTemp == 4 then
		print("Map Temp: Random")
		local tempRand = PWRand()
		local higher = (1 == Map.Rand(2, "ModifyOddsRandom for temperature"));
		mglobal.tropicLatitudes = ModifyOddsRandom(mglobal.tropicLatitudes / mglobal.topLatitude, 0.5, tempRand, higher) * mglobal.topLatitude;
		mglobal.horseLatitudes = ModifyOddsRandom(mglobal.horseLatitudes / mglobal.topLatitude, 0.5, tempRand, higher) * mglobal.topLatitude;
		mglobal.iceLatitude = ModifyOddsRandom(mglobal.iceLatitude / mglobal.topLatitude, 0.5, tempRand, higher) * mglobal.topLatitude;
		mglobal.polarFrontLatitude = ModifyOddsRandom(mglobal.polarFrontLatitude / mglobal.topLatitude, 0.5, tempRand, higher) * mglobal.topLatitude;
		mglobal.tundraTemperature = ModifyOddsRandom(mglobal.tundraTemperature, 0.25, tempRand, higher);
		mglobal.snowTemperature = ModifyOddsRandom(mglobal.snowTemperature, 0.25, tempRand, higher);
		print("tempRand:", tempRand);
		print("higher:", higher);
	else
		print("Map Temp: Normal");
	end

	-- Rainfall
	local oRainfall = Map.GetCustomOption(3);
	if oRainfall == 1 then
		print("Map Rain: Arid");
		mglobal.riverPercent = ModifyOdds(mglobal.riverPercent, 2 / 3);
		mglobal.featurePercent = ModifyOdds(mglobal.featurePercent, 2 / 3);
		mglobal.marshPercent = ModifyOdds(mglobal.marshPercent, 2 / 3);
		mglobal.junglePercent = ModifyOdds(mglobal.junglePercent, 1.5);
		mglobal.zeroTreesPercent = ModifyOdds(mglobal.zeroTreesPercent, 1.5);
		mglobal.plainsPercent = ModifyOdds(mglobal.plainsPercent, 1.25);
		mglobal.desertPercent = ModifyOdds(mglobal.desertPercent, 1.25);
	elseif oRainfall == 3 then
		print("Map Rain: Wet");
		mglobal.riverPercent = ModifyOdds(mglobal.riverPercent, 1.5);
		mglobal.featurePercent = ModifyOdds(mglobal.featurePercent, 1.5);
		mglobal.marshPercent = ModifyOdds(mglobal.marshPercent, 1.5);
		mglobal.junglePercent = ModifyOdds(mglobal.junglePercent, 2 / 3);
		mglobal.zeroTreesPercent = ModifyOdds(mglobal.zeroTreesPercent, 2 / 3);
		mglobal.plainsPercent = ModifyOdds(mglobal.plainsPercent, 0.8);
		mglobal.desertPercent = ModifyOdds(mglobal.desertPercent, 0.8);
	elseif oRainfall == 4 then
		print("Map Rain: Random");
		local rainfallRand = PWRand();
		local higher = (1 == Map.Rand(2, "ModifyOddsRandom for rainfall"));
		mglobal.riverPercent = ModifyOddsRandom(mglobal.riverPercent, 0.5, rainfallRand, higher);
		mglobal.featurePercent = ModifyOddsRandom(mglobal.featurePercent, 0.5, rainfallRand, higher);
		mglobal.marshPercent = ModifyOddsRandom(mglobal.marshPercent, 0.5, rainfallRand, higher);
		mglobal.junglePercent = ModifyOddsRandom(mglobal.junglePercent, 0.5, rainfallRand, not higher);
		mglobal.zeroTreesPercent = ModifyOddsRandom(mglobal.zeroTreesPercent, 0.5, rainfallRand, not higher);
		mglobal.plainsPercent = ModifyOddsRandom(mglobal.plainsPercent, 0.25, rainfallRand, not higher);
		mglobal.desertPercent = ModifyOddsRandom(mglobal.desertPercent, 0.25, rainfallRand, not higher);
		print("rainfallRand:", rainfallRand);
		print("higher:", higher);
	else
		print("Map Rain: Normal");
	end

	-- Sea Level
	local oSeaLevel = Map.GetCustomOption(4);
	if oSeaLevel == 1 then
		print("Map Seas: Low");
		mglobal.landPercent = mglobal.landPercent * 1.25;
	elseif oSeaLevel == 3 then
		print("Map Seas: High");
		mglobal.landPercent = mglobal.landPercent / 1.25;
	elseif oSeaLevel == 4 then
		print("Map Seas: Random");
		local seaLevelRand = 1 + PWRand() * 0.25;
		local higher = (1 == Map.Rand(2, "sea level"));
		if higher then
			mglobal.landPercent = mglobal.landPercent * seaLevelRand;
		else
			mglobal.landPercent = mglobal.landPercent / seaLevelRand;
		end
	else
		print("Map Seas: Normal");
	end
	print("Land Percent:", mglobal.landPercent);

	-- Players Start In
	local oStarts = Map.GetCustomOption(5);
	if oStarts == 2 then
		print("Map Starts: Everywhere");
		mglobal.offsetAtlanticPercent = 0.48; -- Percent of land to divide at the Atlantic Ocean (50% is usually halfway on the map)
		mglobal.offshoreCS = 0.25; -- Percent of city states on uninhabited islands
	else
		print("Map Starts: Largest Continent");
		mglobal.offsetAtlanticPercent = 0.35; -- Percent of land to divide at the Atlantic Ocean
		mglobal.percentLargestContinent = 0.60; -- Eurasia must be this percent of total land (ensures citystates can appear there)
		mglobal.terraConnectWeight = 10; -- if Eurasia is too small, connect sub-continents with this (size/distance) from Eurasia
		mglobal.offshoreCS = 0.75; -- Percent of city states on uninhabited islands
		mglobal.numNaturalWonders = Round(1.25 * mglobal.numNaturalWonders); -- extra wonders for larger map sizes
	end

	-- Ocean Shapes
	local oRiftWidth = Map.GetCustomOption(7);
	if oRiftWidth == 4 then
		oRiftWidth = 1 + Map.Rand(3, "Random Rift Width");
	end
	if oRiftWidth == 1 then
		print("Map Ocean Width: Narrow");
		mglobal.oceanRiftWidth = 1;
		mglobal.landPercent = mglobal.landPercent - 0.02;
	elseif oRiftWidth == 3 then
		print("Map Ocean Width: Wide");
		mglobal.oceanRiftWidth = 1 + mglobal.oceanRiftWidth;
		mglobal.landPercent = mglobal.landPercent + 0.05;
	end

	-- Land Shapes
	local oLandScatter = Map.GetCustomOption(10);
	local scatterOdds = 1;
	if oLandScatter == 1 then
		print("Map Land Scatter: Blocky");
		scatterOdds = 1 / 3;
	elseif oLandScatter == 3 then
		print("Map Land Scatter: Stringy");
		scatterOdds = 5;
	elseif oLandScatter == 4 then
		print("Map Land Scatter: Extremely Stringy");
		scatterOdds = 15;
	elseif oLandScatter == 5 then
		print("Map Land Scatter: Random");
		local scatterRand = PWRand();
		local higher = (1 == Map.Rand(2, "ModifyOddsRandom for scatter"));
		mglobal.landMinScatter = ModifyOddsRandom(mglobal.landMinScatter, 10, scatterRand, higher);
		mglobal.landMaxScatter = ModifyOddsRandom(mglobal.landMaxScatter, 10, scatterRand, higher);
		mglobal.coastScatter = ModifyOddsRandom(mglobal.coastScatter, 10, scatterRand, higher);
	else
		print("Map Land Scatter: Normal");
	end

	if oLandScatter ~= 5 and oLandScatter ~= 2 then
		mglobal.landMinScatter = ModifyOdds(mglobal.landMinScatter, scatterOdds);
		mglobal.landMaxScatter = ModifyOdds(mglobal.landMaxScatter, scatterOdds);
		mglobal.coastScatter = ModifyOdds(mglobal.coastScatter, scatterOdds);
	end

	print("Land Min Scatter: ", mglobal.landMinScatter);
	print("Land Max Scatter: ", mglobal.landMaxScatter);
	print("CoastScatter: ", mglobal.coastScatter);

	----------------------------
	-- Other settings
	----------------------------
	--[[
	These attenuation factors lower the altitude of the map edges.
	This is currently used to prevent large continents in the uninhabitable polar regions.
	East/west attenuation is set to zero, but modded maps may have need for them.
	--]]
	mglobal.northAttenuationFactor = 0.0;
	mglobal.northAttenuationRange = 0.0; -- percent of the map height.
	mglobal.southAttenuationFactor = 0.0;
	mglobal.southAttenuationRange = 0.0;

	-- East west attenuation may be desired for flat maps.
	mglobal.eastAttenuationFactor = 0.0;
	mglobal.eastAttenuationRange = 0.0; -- percent of the map width.
	mglobal.westAttenuationFactor = 0.0;
	mglobal.westAttenuationRange = 0.0;

	-- Rain tweaking variables. I wouldn't touch these.
	mglobal.pressureNorm = 1.0; -- [1.0 = no normalization] Helps to prevent exaggerated Jungle/Marsh banding on the equator. - Bobert13
	mglobal.minimumRainCost = 0.0001;
	mglobal.upLiftExponent = 4;
	mglobal.polarRainBoost = 0.0;

	-- North and south isle latitude limits.
	mglobal.islesNearIce = false;
	mglobal.isleNorthLatitudeLimit = 65;
	mglobal.isleSouthLatitudeLimit = -65;
	mglobal.isleMinDeepWaterNeighbors = 0;

	-----------------------------------------------------------------------
	-- Below is map data that should not be altered.
	-----------------------------------------------------------------------
	mglobal.mountainPasses = {};
	mglobal.tropicalPlots = {};
	mglobal.oceanRiftPlots = {};
	mglobal.lakePlots = {};
	mglobal.seaPlots = {};
	mglobal.elevationRect = {};

	-- Directions
	mglobal.C = DirectionTypes.NO_DIRECTION;
	mglobal.NE = DirectionTypes.DIRECTION_NORTHEAST;
	mglobal.E = DirectionTypes.DIRECTION_EAST;
	mglobal.SE = DirectionTypes.DIRECTION_SOUTHEAST;
	mglobal.SW = DirectionTypes.DIRECTION_SOUTHWEST;
	mglobal.W = DirectionTypes.DIRECTION_WEST;
	mglobal.NW = DirectionTypes.DIRECTION_NORTHWEST;
	mglobal.N = DirectionTypes.DIRECTION_NORTHWEST + 1;
	mglobal.S = DirectionTypes.DIRECTION_NORTHWEST + 2;

	mglobal.edgeDirections = {
		mglobal.NE,
		mglobal.E,
		mglobal.SE,
		mglobal.SW,
		mglobal.W,
		mglobal.NW,
	};

	mglobal.directionNames = {
		[mglobal.C] = "C",
		[mglobal.NE] = "NE",
		[mglobal.E] = "E",
		[mglobal.SE] = "SE",
		[mglobal.SW] = "SW",
		[mglobal.W] = "W",
		[mglobal.NW] = "NW",
		[mglobal.N] = "N",
		[mglobal.S] = "S",
	};

	-- Flow Directions
	mglobal.NOFLOW = 0;
	mglobal.WESTFLOW = 1;
	mglobal.EASTFLOW = 2;
	mglobal.VERTFLOW = 3;

	mglobal.flowNONE = FlowDirectionTypes.NO_FLOWDIRECTION;
	mglobal.flowN = FlowDirectionTypes.FLOWDIRECTION_NORTH;
	mglobal.flowNE = FlowDirectionTypes.FLOWDIRECTION_NORTHEAST;
	mglobal.flowSE = FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST;
	mglobal.flowS = FlowDirectionTypes.FLOWDIRECTION_SOUTH;
	mglobal.flowSW = FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST;
	mglobal.flowNW = FlowDirectionTypes.FLOWDIRECTION_NORTHWEST;

	mglobal.flowNames = {
		[mglobal.flowNONE] = "NONE",
		[mglobal.flowN] = "N",
		[mglobal.flowNE] = "NE",
		[mglobal.flowSE] = "SE",
		[mglobal.flowS] = "S",
		[mglobal.flowSW] = "SW",
		[mglobal.flowNW] = "NW",
	};

	-- Wind zones
	mglobal.NOZONE = -1;
	mglobal.NPOLAR = 0;
	mglobal.NTEMPERATE = 1;
	mglobal.NEQUATOR = 2;
	mglobal.SEQUATOR = 3;
	mglobal.STEMPERATE = 4;
	mglobal.SPOLAR = 5;

	-- Hex maps are shorter in the y direction than they are wide per unit by this much.
	-- We need to know this to sample the perlin maps properly so they don't look squished.
	mglobal.YtoXRatio = 1.5 / (math.sqrt(0.75) * 2);

	-- Array of route types - you can change the text, but NOT the order
	mglobal.routes = {"Land", "Road", "Railroad", "Coastal", "Ocean", "Submarine"};

	-- Array of highlight colours
	mglobal.highlights = {
		Red = Vector4(1.0, 0.0, 0.0, 1.0),
		Green = Vector4(0.0, 1.0, 0.0, 1.0),
		Blue = Vector4(0.0, 0.0, 1.0, 1.0),
		Cyan = Vector4(0.0, 1.0, 1.0, 1.0),
		Yellow = Vector4(1.0, 1.0, 0.0, 1.0),
		Magenta = Vector4(1.0, 0.0, 1.0, 1.0),
		Black = Vector4(0.5, 0.5, 0.5, 1.0),
	};

	mglobal.lastRouteLength = 0;

	-----------------------------------------------------------------------
	-- Override AssignStartingPlots functions
	-----------------------------------------------------------------------
	if overrideAssignStartingPlots then
		print("Override AssignStartingPlots: True");

		local islandAreaBuffed = {};

		function DoNothing() end

		function __CustomInit(ASP)
			if not debugPrint then
				print = DoNothing;
			end

			-- Function overrides
			ASP.MeasureStartPlacementFertilityOfPlot = MeasureStartPlacementFertilityOfPlot;
			ASP.GetNumNaturalWondersToPlace = GetNumNaturalWondersToPlace;
			ASP.GetNumCityStatesInUninhabitedRegion = GetNumCityStatesInUninhabitedRegion;
			ASP.BuffIslands = BuffIslands;
			ASP.AttemptToPlaceTreesAtResourcePlot = AttemptToPlaceTreesAtResourcePlot;
			ASP.PlaceStrategicAndBonusResources = PlaceStrategicAndBonusResources;
			ASP.PlaceFish = PlaceFish;
			ASP.PlacePossibleFish = PlacePossibleFish;
			ASP.PlaceBonusResources = PlaceBonusResources;
			ASP.PlaceResourcesAndCityStates = PlaceResourcesAndCityStates;
		end
		------------------------------------------------------------------------------
		function MeasureStartPlacementFertilityOfPlot(ASP, x, y)
			return ASP:Plot_GetFertility(Map.GetPlot(x, y));
		end
		------------------------------------------------------------------------------
		function GetNumNaturalWondersToPlace(_, iWorldSize)
			local iTarget = (iWorldSize + 1) * 2;
			if Map.GetCustomOption(5) == 1 then
				iTarget = Round(1.25 * iTarget); -- more NWs for Terra's larger map size
			end
			return iTarget;
		end
		------------------------------------------------------------------------------
		function GetNumCityStatesInUninhabitedRegion(ASP, iNumCivLandmassPlots, iNumUninhabitedLandmassPlots)
			local uninhabited_ratio = iNumUninhabitedLandmassPlots / (iNumCivLandmassPlots + iNumUninhabitedLandmassPlots);
			local max_by_ratio = math.floor(5 * uninhabited_ratio * ASP.iNumCityStates);
			local max_by_method = math.ceil(MG.offshoreCS * ASP.iNumCityStates);
			return math.min(ASP.iNumCityStatesUnassigned, max_by_ratio, max_by_method);
		end
		------------------------------------------------------------------------------
		function BuffIslands(ASP)
			local biggestLandmassSize = Map.GetNumTilesOfLandmass(Map.FindBiggestLandmassID(false));
			if biggestLandmassSize < 20 then
				-- Skip on archipalego maps
				return;
			end

			-- MOD.HungryForFood: Start
			local resWeights;
			if ASP:IsEvenMoreResourcesActive() then
				resWeights = {
					[ASP.stone_ID] = 1,
					[ASP.coal_ID] = 8,
					[ASP.oil_ID] = 6,
					[ASP.aluminum_ID] = 4,
					[ASP.uranium_ID] = 4,
					[ASP.sulfur_ID] = 2,
					[ASP.titanium_ID] = 2,
					[ASP.lead_ID] = 2,
				};
			else
				resWeights = {
					[ASP.stone_ID] = 1,
					[ASP.coal_ID] = 5,
					[ASP.oil_ID] = 3,
					[ASP.aluminum_ID] = 2,
					[ASP.uranium_ID] = 2,
				};
			end
			-- MOD.HungryForFood: End

			for _, plot in Plots(Shuffle) do
				local plotType = plot:GetPlotType();
				local terrainType = plot:GetTerrainType();
				local featureType = plot:GetFeatureType();
				local landmassID = plot:GetLandmass();
				local landmassSize = Map.GetNumTilesOfLandmass(landmassID);
				if (plotType == PlotTypes.PLOT_HILLS or plotType == PlotTypes.PLOT_LAND) and
						plot:GetResourceType() == -1 and
						IsBetween(1, landmassSize, 0.05 * biggestLandmassSize) and
						not islandAreaBuffed[landmassID] then
					local resID = GetRandomWeighted(resWeights);
					local resNum = 1;
					if resID ~= ASP.stone_ID then
						resNum = resNum + Map.Rand(2, "BuffIslands Random Resource Quantity - Lua");
						if resID ~= ASP.uranium_ID then
							resNum = resNum + 1;
						end
					end
					if resNum > 0 then
						islandAreaBuffed[landmassID] = true;
						if 25 >= Map.Rand(100, "BuffIslands Chance - Lua") then
							-- Adjust terrain/feature for resource about to be placed
							if resID == ASP.stone_ID then
								if featureType == FeatureTypes.FEATURE_JUNGLE or featureType == FeatureTypes.FEATURE_FOREST then
									plot:SetFeatureType(FeatureTypes.NO_FEATURE);
								end
							elseif resID == ASP.coal_ID then
								if terrainType == TerrainTypes.TERRAIN_TUNDRA then
									plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true);
								elseif terrainType == TerrainTypes.TERRAIN_DESERT then
									plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true);
								end
							elseif resID == ASP.oil_ID then
								if plotType == PlotTypes.PLOT_HILLS then
									plot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
								end
								if (terrainType == TerrainTypes.TERRAIN_GRASS or terrainType == TerrainTypes.TERRAIN_PLAINS) and featureType == FeatureTypes.NO_FEATURE then
									for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
										if nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
											nearPlot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true);
										end
									end
									plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE);
								end
							elseif resID == ASP.aluminum_ID then
								if featureType == FeatureTypes.FEATURE_JUNGLE or featureType == FeatureTypes.FEATURE_FOREST then
									plot:SetFeatureType(FeatureTypes.NO_FEATURE);
								end
								if terrainType == TerrainTypes.TERRAIN_GRASS then
									plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true);
								end
							end
							plot:SetResourceType(resID, resNum);
							ASP.amounts_of_resources_placed[resID + 1] = ASP.amounts_of_resources_placed[resID + 1] + resNum;
						end
					end
				end
			end
		end
		------------------------------------------------------------------------------
		function AttemptToPlaceTreesAtResourcePlot(_, plot)
			-- Force add trees to a specific plot for AdjustTiles
			-- No need to check IsGoodFeaturePlot or whether the resource is furs (restricted to tundra)
			local x, y = plot:GetX(), plot:GetY();
			local i = elevationMap:GetIndex(x, y);
			local plotTerrainID = plot:GetTerrainType();
			local jungleThreshold = rainfallMap:FindThresholdFromPercent(MG.junglePercent, false, true);

			-- Micro-climates for tiny volcanic islands
			if not plot:IsMountain() and (plotTerrainID == TerrainTypes.TERRAIN_PLAINS or plotTerrainID == TerrainTypes.TERRAIN_GRASS) then
				local landmassSize = Map.GetNumTilesOfLandmass(plot:GetLandmass());
				if landmassSize <= 6 - math.max(1, Map.Rand(5, "Add Island Features - Lua")) then
					local zone = elevationMap:GetZone(y);
					if zone == MG.NEQUATOR or zone == MG.SEQUATOR then
						for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
							if nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
								return;
							end
						end
						plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE);
						return;
					elseif zone == MG.NTEMPERATE or zone == MG.STEMPERATE then
						plot:SetFeatureType(FeatureTypes.FEATURE_FOREST);
						return;
					end
				end
			end

			-- Too dry for jungle
			if rainfallMap.data[i] < jungleThreshold then
				plot:SetFeatureType(FeatureTypes.FEATURE_FOREST);
				return;
			end

			-- Too cold for jungle
			if temperatureMap.data[i] < MG.jungleMinTemperature then
				plot:SetFeatureType(FeatureTypes.FEATURE_FOREST);
				return;
			end

			-- Too near desert for jungle
			for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
				if nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
					plot:SetFeatureType(FeatureTypes.FEATURE_FOREST);
					return;
				end
			end

			plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE);
		end
		------------------------------------------------------------------------------
		function PlaceStrategicAndBonusResources(ASP)
			-- Keeping this for the map for easier adjustment, despite being ported to VP

			-- KEY: {Resource ID, Quantity (0 = unquantified), weighting, minimum radius, maximum radius}
			-- KEY: (frequency (1 per n plots in the list), impact list number, plot list, resource data)

			-- The radius creates a zone around the plot that other resources of that
			-- type will avoid if possible. See ProcessResourceList for impact numbers.

			-- Order of placement matters, so changing the order may affect a later dependency.

			-- Adjust amounts, if applicable, based on strategic deposit size setting.
			local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = ASP:GetMajorStrategicResourceQuantityValues();
			local resources_to_place = {};

			-- Adjust appearance rate per resource density setting chosen by user.
			local resMultiplier = 1;
			if ASP.resDensity == 1 then -- Sparse, so increase the number of tiles per strategic.
				resMultiplier = 1.5;
			elseif ASP.resDensity == 3 then -- Abundant, so reduce the number of tiles per strategic.
				resMultiplier = 0.66666667;
			elseif ASP.resDensity == 4 then -- Random
				resMultiplier = ASP:GetRandomMultiplier(0.5);
			end

			-- Place Strategic resources.
			print("Map Generation - Placing Strategics");

			-- Revamped by azum4roll: now place most resources one by one for easier balancing
			local tPlotList = ASP.global_resource_plot_lists;

			resources_to_place = {
				{ASP.horse_ID, horse_amt, 100, 1, 2}
			};
			ASP:ProcessResourceList(24 * resMultiplier, ImpactLayers.LAYER_HORSE, tPlotList[PlotListTypes.FLAT_GRASS_NO_FEATURE], resources_to_place);
			ASP:ProcessResourceList(36 * resMultiplier, ImpactLayers.LAYER_HORSE, tPlotList[PlotListTypes.FLAT_PLAINS_NO_FEATURE], resources_to_place);

			resources_to_place = {
				{ASP.horse_ID, horse_amt * 0.7, 100, 2, 3}
			};
			ASP:ProcessResourceList(35 * resMultiplier, ImpactLayers.LAYER_HORSE, tPlotList[PlotListTypes.FLAT_DESERT_WET], resources_to_place);
			ASP:ProcessResourceList(60 * resMultiplier, ImpactLayers.LAYER_HORSE, tPlotList[PlotListTypes.FLAT_TUNDRA_NO_FEATURE], resources_to_place);

			resources_to_place = {
				{ASP.iron_ID, iron_amt, 100, 1, 3}
			};
			ASP:ProcessResourceList(90 * resMultiplier, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.HILLS_NO_FEATURE], resources_to_place);
			ASP:ProcessResourceList(75 * resMultiplier, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.HILLS_FOREST], resources_to_place);
			ASP:ProcessResourceList(60 * resMultiplier, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.FLAT_DESERT_NO_FEATURE], resources_to_place);
			ASP:ProcessResourceList(110 * resMultiplier, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE], resources_to_place);
			ASP:ProcessResourceList(80 * resMultiplier, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_FOREST], resources_to_place);
			ASP:ProcessResourceList(90 * resMultiplier, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.FLAT_TUNDRA_NO_FEATURE], resources_to_place);
			ASP:ProcessResourceList(85 * resMultiplier, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.FLAT_TUNDRA_FOREST], resources_to_place);
			ASP:ProcessResourceList(17 * resMultiplier, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.FLAT_SNOW], resources_to_place);

			resources_to_place = {
				{ASP.coal_ID, coal_amt, 100, 1, 2}
			};
			ASP:ProcessResourceList(100 * resMultiplier, ImpactLayers.LAYER_COAL, tPlotList[PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE], resources_to_place);
			ASP:ProcessResourceList(90 * resMultiplier, ImpactLayers.LAYER_COAL, tPlotList[PlotListTypes.FLAT_GRASS_NO_FEATURE], resources_to_place);
			ASP:ProcessResourceList(90 * resMultiplier, ImpactLayers.LAYER_COAL, tPlotList[PlotListTypes.FLAT_PLAINS_NO_FEATURE], resources_to_place);
			ASP:ProcessResourceList(100 * resMultiplier, ImpactLayers.LAYER_COAL, tPlotList[PlotListTypes.PLAINS_GRASS_FOREST], resources_to_place);

			resources_to_place = {
				{ASP.oil_ID, oil_amt, 100, 1, 3}
			};
			ASP:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_OIL, tPlotList[PlotListTypes.FLAT_DESERT_NO_FEATURE], resources_to_place);
			ASP:ProcessResourceList(75 * resMultiplier, ImpactLayers.LAYER_OIL, tPlotList[PlotListTypes.FLAT_TUNDRA_NO_FEATURE], resources_to_place);
			ASP:ProcessResourceList(100 * resMultiplier, ImpactLayers.LAYER_OIL, tPlotList[PlotListTypes.FLAT_JUNGLE], resources_to_place);
			ASP:ProcessResourceList(25 * resMultiplier, ImpactLayers.LAYER_OIL, tPlotList[PlotListTypes.MARSH], resources_to_place);
			ASP:ProcessResourceList(17 * resMultiplier, ImpactLayers.LAYER_OIL, tPlotList[PlotListTypes.FLAT_SNOW], resources_to_place);

			resources_to_place = {
				{ASP.aluminum_ID, alum_amt, 100, 1, 3}
			};
			ASP:ProcessResourceList(75 * resMultiplier, ImpactLayers.LAYER_ALUMINUM, tPlotList[PlotListTypes.HILLS_DESERT_TUNDRA_PLAINS_NO_FEATURE], resources_to_place);
			ASP:ProcessResourceList(50 * resMultiplier, ImpactLayers.LAYER_ALUMINUM, tPlotList[PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE], resources_to_place);
			ASP:ProcessResourceList(60 * resMultiplier, ImpactLayers.LAYER_ALUMINUM, tPlotList[PlotListTypes.FLAT_PLAINS_NO_FEATURE], resources_to_place);

			resources_to_place = {
				{ASP.uranium_ID, uran_amt, 100, 2, 4}
			};
			ASP:ProcessResourceList(200 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.HILLS_NO_FEATURE], resources_to_place);
			ASP:ProcessResourceList(300 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.HILLS_FOREST], resources_to_place);
			ASP:ProcessResourceList(50 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.HILLS_JUNGLE], resources_to_place);
			ASP:ProcessResourceList(150 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.FLAT_JUNGLE], resources_to_place);
			ASP:ProcessResourceList(150 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.FLAT_DESERT_NO_FEATURE], resources_to_place);
			ASP:ProcessResourceList(90 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.FLAT_TUNDRA_NO_FEATURE], resources_to_place);
			ASP:ProcessResourceList(350 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.FLAT_TUNDRA_FOREST], resources_to_place);
			ASP:ProcessResourceList(300 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE], resources_to_place);
			ASP:ProcessResourceList(325 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_FOREST], resources_to_place);
			ASP:ProcessResourceList(37 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.MARSH], resources_to_place);
			ASP:ProcessResourceList(35 * resMultiplier, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.FLAT_SNOW], resources_to_place);

			ASP:AddModernMinorStrategicsToCityStates();

			ASP:PlaceSmallQuantitiesOfStrategics(42 * resMultiplier, tPlotList[PlotListTypes.LAND]);

			ASP:PlaceOilInTheSea();

			-- Check for low or missing Strategic resources
			uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = ASP:GetSmallStrategicResourceQuantityValues();
			local iLoop = 0;
			while ASP.amounts_of_resources_placed[ASP.iron_ID + 1] < 4 * ASP.iNumCivs and iLoop < 999 do
				print("Map has very low iron, adding another.");
				resources_to_place = { {ASP.iron_ID, iron_amt, 33, 1, 2} };
				ASP:ProcessResourceList(99999, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.HILLS], resources_to_place); -- 99999 means one per that many tiles: a single instance.
				ASP:ProcessResourceList(99999, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.FLAT_FOREST], resources_to_place);
				ASP:ProcessResourceList(99999, ImpactLayers.LAYER_IRON, tPlotList[PlotListTypes.FLAT_DESERT_NO_FEATURE], resources_to_place);
				iLoop = iLoop + 1;
			end
			iLoop = 0;
			while ASP.amounts_of_resources_placed[ASP.horse_ID + 1] < 4 * ASP.iNumCivs and iLoop < 999 do
				print("Map has very low horse, adding another.");
				resources_to_place = { {ASP.horse_ID, horse_amt, 50, 1, 2} };
				ASP:ProcessResourceList(99999, ImpactLayers.LAYER_HORSE, tPlotList[PlotListTypes.FLAT_TUNDRA_PLAINS_GRASS_NO_FEATURE], resources_to_place);
				ASP:ProcessResourceList(99999, ImpactLayers.LAYER_HORSE, tPlotList[PlotListTypes.FLAT_DESERT_WET], resources_to_place);
				iLoop = iLoop + 1;
			end
			iLoop = 0;
			while ASP.amounts_of_resources_placed[ASP.coal_ID + 1] < 4 * ASP.iNumCivs and iLoop < 999 do
				print("Map has very low coal, adding another.");
				resources_to_place = { {ASP.coal_ID, coal_amt, 33, 1, 2} };
				ASP:ProcessResourceList(99999, ImpactLayers.LAYER_COAL, tPlotList[PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE], resources_to_place);
				ASP:ProcessResourceList(99999, ImpactLayers.LAYER_COAL, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE], resources_to_place);
				ASP:ProcessResourceList(99999, ImpactLayers.LAYER_COAL, tPlotList[PlotListTypes.PLAINS_GRASS_FOREST], resources_to_place);
				iLoop = iLoop + 1;
			end
			iLoop = 0;
			while ASP.amounts_of_resources_placed[ASP.oil_ID + 1] < 4 * ASP.iNumCivs and iLoop < 999 do
				print("Map has very low oil, adding another.");
				resources_to_place = { {ASP.oil_ID, oil_amt, 50, 1, 2} };
				ASP:ProcessResourceList(99999, ImpactLayers.LAYER_OIL, tPlotList[PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE], resources_to_place);
				ASP:ProcessResourceList(99999, ImpactLayers.LAYER_OIL, tPlotList[PlotListTypes.FLAT_JUNGLE], resources_to_place);
				iLoop = iLoop + 1;
			end
			iLoop = 0;
			while ASP.amounts_of_resources_placed[ASP.aluminum_ID + 1] < 5 * ASP.iNumCivs and iLoop < 999 do
				print("Map has very low aluminum, adding another.");
				resources_to_place = { {ASP.aluminum_ID, alum_amt, 33, 1, 2} };
				ASP:ProcessResourceList(99999, ImpactLayers.LAYER_ALUMINUM, tPlotList[PlotListTypes.HILLS_DESERT_TUNDRA_PLAINS_NO_FEATURE], resources_to_place);
				ASP:ProcessResourceList(99999, ImpactLayers.LAYER_ALUMINUM, tPlotList[PlotListTypes.FLAT_DESERT_TUNDRA_NO_FEATURE], resources_to_place);
				ASP:ProcessResourceList(99999, ImpactLayers.LAYER_ALUMINUM, tPlotList[PlotListTypes.FLAT_PLAINS_NO_FEATURE], resources_to_place);
				iLoop = iLoop + 1;
			end
			iLoop = 0;
			while ASP.amounts_of_resources_placed[ASP.uranium_ID + 1] < 2 * ASP.iNumCivs and iLoop < 999 do
				print("Map has very low uranium, adding another.");
				resources_to_place = { {ASP.uranium_ID, uran_amt, 100, 2, 4} };
				ASP:ProcessResourceList(99999, ImpactLayers.LAYER_URANIUM, tPlotList[PlotListTypes.LAND], resources_to_place);
				iLoop = iLoop + 1;
			end

			ASP:PlaceBonusResources();
		end
		------------------------------------------------------------------------------
		function PlaceFish(ASP)
			for _, plot in Plots(Shuffle) do
				if ASP:PlacePossibleFish(plot) then
					ASP.amounts_of_resources_placed[ASP.fish_ID + 1] = ASP.amounts_of_resources_placed[ASP.fish_ID + 1] + 1;
				end
			end
		end
		------------------------------------------------------------------------------
		function PlacePossibleFish(ASP, plot)
			if plot:GetTerrainType() ~= TerrainTypes.TERRAIN_COAST or plot:IsLake() or plot:GetFeatureType() ~= FeatureTypes.NO_FEATURE or plot:GetResourceType() ~= -1 then
				return;
			end
			local landDistance = 999;
			local sumFertility = 0;
			local odds = 0;
			local oddsMultiplier = 1.0;

			for nearPlot, distance in Plot_GetPlotsInCircle(plot, 1, 3) do
				distance = math.max(1, distance);
				if not nearPlot:IsWater() and distance < landDistance then
					landDistance = distance;
				end
				sumFertility = sumFertility + ASP:Plot_GetFertility(nearPlot, false, true);
				if nearPlot:GetResourceType() == ASP.fish_ID then
					oddsMultiplier = oddsMultiplier - 0.5 / distance;
				end
			end
			if landDistance >= 3 then
				return;
			end

			odds = math.max((1.2 - sumFertility / MG.fishTargetFertility) / 2, 0.15); -- 15% to 60%
			oddsMultiplier = oddsMultiplier / landDistance;
			odds = ModifyOdds(odds, oddsMultiplier);

			if odds >= PWRand() then
				plot:SetResourceType(ASP.fish_ID, 1);
				return true;
			end
		end
		------------------------------------------------------------------------------
		function PlaceBonusResources(ASP)
			local resMultiplier = 1;
			if ASP.bonusDensity == 1 then -- Sparse, so increase the number of tiles per bonus.
				resMultiplier = 1.5;
			elseif ASP.bonusDensity == 3 then -- Abundant, so reduce the number of tiles per bonus.
				resMultiplier = 0.66666667;
			elseif ASP.bonusDensity == 4 then -- Random
				resMultiplier = GetRandomMultiplier(0.5);
			end

			-- Place Bonus Resources
			print("Map Generation - Placing Bonuses");

			-- modified by azum4roll: PlaceFish only depends on fishTargetFertility now
			local tPlotList = ASP.global_resource_plot_lists;
			ASP:PlaceFish();
			ASP:PlaceSexyBonusAtCivStarts();
			ASP:AddExtraBonusesToHillsRegions();

			local resources_to_place = {};

			if ASP:IsAloeVeraResourceActive() then
				resources_to_place = {
					{ASP.aloevera_ID, 1, 100, 1, 2}
				};
				ASP:ProcessResourceList(10 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_DESERT_NO_FEATURE], resources_to_place);
			end

			if ASP:IsEvenMoreResourcesActive() then
				resources_to_place = {
					{ASP.deer_ID, 1, 100, 0, 2}
				};
				ASP:ProcessResourceList(12 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.TUNDRA_FOREST], resources_to_place);
				ASP:ProcessResourceList(16 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_TUNDRA_NO_FEATURE], resources_to_place);

				resources_to_place = {
					{ASP.deer_ID, 1, 100, 0, 1}
				};
				ASP:ProcessResourceList(30 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.JUNGLE], resources_to_place);

				resources_to_place = {
					{ASP.wheat_ID, 1, 100, 1, 2}
				};
				ASP:ProcessResourceList(20 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_DESERT_WET], resources_to_place);

				resources_to_place = {
					{ASP.wheat_ID, 1, 100, 2, 3}
				};
				ASP:ProcessResourceList(44 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.WHEAT], resources_to_place);

				resources_to_place = {
					{ASP.rice_ID, 1, 100, 0, 2}
				};
				ASP:ProcessResourceList(24 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.RICE], resources_to_place);

				resources_to_place = {
					{ASP.maize_ID, 1, 100, 1, 2}
				};
				ASP:ProcessResourceList(32 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.MAIZE], resources_to_place);

				resources_to_place = {
					{ASP.banana_ID, 1, 100, 0, 1}
				};
				ASP:ProcessResourceList(30 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_JUNGLE], resources_to_place);
				ASP:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.MARSH], resources_to_place);

				resources_to_place = {
					{ASP.cow_ID, 1, 100, 0, 2}
				};
				ASP:ProcessResourceList(30 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_GRASS_NO_FEATURE], resources_to_place);

				resources_to_place = {
					{ASP.bison_ID, 1, 100, 0, 2}
				};
				ASP:ProcessResourceList(24 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE], resources_to_place);

				resources_to_place = {
					{ASP.sheep_ID, 1, 100, 0, 2}
				};
				ASP:ProcessResourceList(44 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_NO_FEATURE], resources_to_place);

				resources_to_place = {
					{ASP.stone_ID, 1, 100, 1, 1}
				};
				ASP:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_PLAINS_DRY_NO_FEATURE], resources_to_place);
				ASP:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_GRASS_DRY_NO_FEATURE], resources_to_place);

				resources_to_place = {
					{ASP.stone_ID, 1, 100, 1, 2}
				};
				ASP:ProcessResourceList(30 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_TUNDRA_NO_FEATURE], resources_to_place);
				ASP:ProcessResourceList(36 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_NO_FEATURE], resources_to_place);

				resources_to_place = {
					{ASP.stone_ID, 1, 100, 0, 2}
				};
				ASP:ProcessResourceList(16 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_DESERT_NO_FEATURE], resources_to_place);
				ASP:ProcessResourceList(10 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_SNOW], resources_to_place);

			-- Even More Resources for VP start
				resources_to_place = {
					{ASP.coconut_ID, 1, 100, 0, 2}
				};
				ASP:ProcessResourceList(12 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.COCONUT], resources_to_place);

				resources_to_place = {
					{ASP.hardwood_ID, 1, 100, 1, 2}
				};
				ASP:ProcessResourceList(37 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_COVERED], resources_to_place);
				ASP:ProcessResourceList(37 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_COVERED], resources_to_place);
				ASP:ProcessResourceList(29 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_TUNDRA_FOREST], resources_to_place);

				resources_to_place = {
					{ASP.pineapple_ID, 1, 100, 0, 3}
				};
				ASP:ProcessResourceList(29 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_JUNGLE], resources_to_place);

				resources_to_place = {
					{ASP.rubber_ID, 1, 100, 0, 2}
				};
				ASP:ProcessResourceList(43 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_JUNGLE], resources_to_place);

				resources_to_place = {
					{ASP.sulfur_ID, 1, 100, 0, 2}
				};
				ASP:ProcessResourceList(21 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_SNOW], resources_to_place);

				resources_to_place = {
					{ASP.sulfur_ID, 1, 100, 1, 3}
				};
				ASP:ProcessResourceList(29 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_NO_FEATURE], resources_to_place);
				ASP:ProcessResourceList(37 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_COVERED], resources_to_place);
				ASP:ProcessResourceList(43 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_NO_FEATURE], resources_to_place);

				resources_to_place = {
					{ASP.titanium_ID, 1, 100, 0, 2}
				};
				ASP:ProcessResourceList(56 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_NO_FEATURE], resources_to_place);
				ASP:ProcessResourceList(51 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_NO_FEATURE], resources_to_place);
				ASP:ProcessResourceList(48 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_DESERT_NO_FEATURE], resources_to_place);
				ASP:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_TUNDRA], resources_to_place);

				resources_to_place = {
					{ASP.titanium_ID, 1, 100, 0, 1}
				};
				ASP:ProcessResourceList(24 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_SNOW], resources_to_place);

				resources_to_place = {
					{ASP.potato_ID, 1, 100, 0, 3}
				};
				ASP:ProcessResourceList(29 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_PLAINS_GRASS_NO_FEATURE], resources_to_place);

				resources_to_place = {
					{ASP.potato_ID, 1, 100, 2, 3}
				};
				ASP:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE], resources_to_place);

				resources_to_place = {
					{ASP.lead_ID, 1, 100, 1, 2}
				};
				ASP:ProcessResourceList(35 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_DESERT_NO_FEATURE], resources_to_place);

				resources_to_place = {
					{ASP.lead_ID, 1, 100, 1, 3}
				};
				ASP:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_GRASS_DRY_NO_FEATURE], resources_to_place);

				resources_to_place = {
					{ASP.lead_ID, 1, 100, 2, 3}
				};
				ASP:ProcessResourceList(35 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_NO_FEATURE], resources_to_place);
			-- Even More Resources for VP end

				-- Placed last, since this blocks a large area
				resources_to_place = {
					{ASP.deer_ID, 1, 100, 3, 4}
				};
				ASP:ProcessResourceList(50 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_FOREST], resources_to_place);
				ASP:ProcessResourceList(50 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_FOREST], resources_to_place);
			else
				resources_to_place = {
					{ASP.deer_ID, 1, 100, 0, 2}
				};
				ASP:ProcessResourceList(6 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.TUNDRA_FOREST], resources_to_place);
				ASP:ProcessResourceList(8 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_TUNDRA_NO_FEATURE], resources_to_place);

				resources_to_place = {
					{ASP.deer_ID, 1, 100, 0, 1}
				};
				ASP:ProcessResourceList(12 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.JUNGLE], resources_to_place);

				resources_to_place = {
					{ASP.wheat_ID, 1, 100, 0, 2}
				};
				ASP:ProcessResourceList(20 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.WHEAT], resources_to_place);

				resources_to_place = {
					{ASP.rice_ID, 1, 100, 0, 1}
				};
				ASP:ProcessResourceList(12 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.RICE], resources_to_place);

				resources_to_place = {
					{ASP.maize_ID, 1, 100, 0, 2}
				};
				ASP:ProcessResourceList(24 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.MAIZE], resources_to_place);

				resources_to_place = {
					{ASP.banana_ID, 1, 100, 0, 1}
				};
				ASP:ProcessResourceList(12 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_JUNGLE], resources_to_place);
				ASP:ProcessResourceList(16 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.MARSH], resources_to_place);

				resources_to_place = {
					{ASP.cow_ID, 1, 100, 0, 2}
				};
				ASP:ProcessResourceList(14 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_GRASS_NO_FEATURE], resources_to_place);

				resources_to_place = {
					{ASP.bison_ID, 1, 100, 0, 2}
				};
				ASP:ProcessResourceList(18 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_NO_FEATURE], resources_to_place);

				resources_to_place = {
					{ASP.sheep_ID, 1, 100, 0, 2}
				};
				ASP:ProcessResourceList(18 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_NO_FEATURE], resources_to_place);

				resources_to_place = {
					{ASP.stone_ID, 1, 100, 1, 1}
				};
				ASP:ProcessResourceList(60 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_PLAINS_DRY_NO_FEATURE], resources_to_place);
				ASP:ProcessResourceList(30 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_GRASS_DRY_NO_FEATURE], resources_to_place);

				resources_to_place = {
					{ASP.stone_ID, 1, 100, 0, 2}
				};
				ASP:ProcessResourceList(14 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_DESERT_NO_FEATURE], resources_to_place);
				ASP:ProcessResourceList(8 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_SNOW], resources_to_place);

				resources_to_place = {
					{ASP.stone_ID, 1, 100, 1, 2}
				};
				ASP:ProcessResourceList(60 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_TUNDRA_NO_FEATURE], resources_to_place);
				ASP:ProcessResourceList(60 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_NO_FEATURE], resources_to_place);

				-- Placed last, since this blocks a large area
				resources_to_place = {
					{ASP.deer_ID, 1, 100, 3, 4}
				};
				ASP:ProcessResourceList(25 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.FLAT_PLAINS_GRASS_FOREST], resources_to_place);
				ASP:ProcessResourceList(25 * resMultiplier, ImpactLayers.LAYER_BONUS, tPlotList[PlotListTypes.HILLS_FOREST], resources_to_place);
			end
		end
		------------------------------------------------------------------------------
		function PlaceResourcesAndCityStates(ASP)
			--[[
			This function controls nearly all resource placement.
			Only resources placed during Normalization operations are handled elsewhere.
		
			Luxury resources are placed in relationship to Regions, adapting to the details of the given map instance, including number of civs and city states present.
			At Jon's direction, Luxuries have been implemented to be diplomatic widgets for trading, in addition to sources of Happiness.
		
			Strategic and Bonus resources are terrain-adjusted. They will customize to each map instance.
			Each terrain type has been measured and has certain resource types assigned to it.
			You can customize resource placement to any degree desired by controlling generation of plot groups to feed into the process.
			The default plot groups are terrain-based, but any criteria you desire could be used to determine plot group membership.
		
			If any default methods fail to meet a specific need, don't hesitate to replace them with custom methods.
			I have labored to make this new system as accessible and powerful as any ever before offered.
			--]]

			print("Map Generation - Assigning Luxury Resource Distribution");
			ASP:AssignLuxuryRoles();

			print("Map Generation - Placing City States");
			ASP:PlaceCityStates();

			-- Generate global plot lists for resource distribution.
			ASP:GenerateGlobalResourcePlotLists();

			print("Map Generation - Placing Luxuries");
			ASP:PlaceLuxuries();

			print("Map Generation - Buffing Tiny Islands");
			ASP:BuffIslands();

			-- Place Strategic and Bonus resources.
			ASP:PlaceStrategicAndBonusResources();

			print("Map Generation - Normalize City State Locations");
			ASP:NormalizeCityStateLocations();

			print("Map Generation - Fix Tile Graphics");
			ASP:AdjustTiles();

			local largestLandmassID = Map.FindBiggestLandmassID(false);
			if Map.GetCustomOption(5) == 1 then
				-- Biggest continent placement
				if Map.GetNumTilesOfLandmass(largestLandmassID) < 0.25 * Map.GetLandPlots() then
					print("AI Map Strategy - Offshore expansion with navy bias");
					-- Tell the AI that we should treat this as a offshore expansion map with naval bias
					Map.ChangeAIMapHint(4 + 1);
				else
					print("AI Map Strategy - Offshore expansion");
					-- Tell the AI that we should treat this as a offshore expansion map
					Map.ChangeAIMapHint(4);
				end
			elseif Map.GetNumTilesOfLandmass(largestLandmassID) < 0.25 * Map.GetLandPlots() then
				print("AI Map Strategy - Navy bias");
				-- Tell the AI that we should treat this as a map with naval bias
				Map.ChangeAIMapHint(1);
			else
				print("AI Map Strategy - Normal");
			end

			-- Necessary to implement placement of Natural Wonders, and possibly other plot-type changes.
			-- This operation must be saved for last, as it invalidates all regional data by resetting Area IDs.
			Map.RecalculateAreas();

			-- Activate for debug only
			ASP:PrintFinalResourceTotalsToLog();
		end
	else
		print("Override AssignStartingPlots: False");
	end

	return mglobal;
end

function GetMapScriptInfo()
	local world_age, temperature, rainfall, sea_level = GetCoreMapOptions();
	return {
		Name = "Communitu_79a v3.1.0",
		Description = "Communitas mapscript for Vox Populi (version 4.5+)",
		IsAdvancedMap = false,
		SupportsMultiplayer = true,
		IconIndex = 1,
		SortIndex = 1,
		CustomOptions = {
			world_age,
			temperature,
			rainfall,
			sea_level,
			{
				Name = "TXT_KEY_MAP_PLAYERS_START_IN",
				Values = {
					"TXT_KEY_MAP_TERRA_LARGEST_CONTINENT",
					"TXT_KEY_MAP_CONTINENTS_EVERYWHERE",
				},
				DefaultValue = 2,
				SortPriority = -99, -- use negative values so they don't interfere with those of the standard map options
			},
			{
				Name = "TXT_KEY_MAP_OCEAN_SHAPES",
				Description = "TXT_KEY_MAP_OCEAN_SHAPES_DESCRIPTION",
				Values = {
					"TXT_KEY_MAP_2_ATLANTIC",
					"TXT_KEY_MAP_PACIFIC_AND_ATLANTIC",
					"TXT_KEY_MAP_2_PACIFIC",
					"TXT_KEY_MAP_2_RANDOM",
					"TXT_KEY_MAP_1_RANDOM",
					"TXT_KEY_MAP_NONE",
					"TXT_KEY_MAP_RANDOM",
				},
				DefaultValue = 2,
				SortPriority = -98,
			},
			{
				Name = "TXT_KEY_MAP_RIFT_WIDTH",
				Description = "TXT_KEY_MAP_RIFT_WIDTH_DESCRIPTION",
				Values = {
					"TXT_KEY_MAP_NARROW",
					"TXT_KEY_MAP_NORMAL",
					"TXT_KEY_MAP_WIDE",
					"TXT_KEY_MAP_RANDOM",
				},
				DefaultValue = 2,
				SortPriority = -97,
			},
			{
				Name = "TXT_KEY_MAP_CIRCUMNAVIGATION",
				Description = "TXT_KEY_MAP_CIRCUMNAVIGATION_DESCRIPTION",
				Values = {
					"TXT_KEY_MAP_ON",
					"TXT_KEY_MAP_OFF",
				},
				DefaultValue = 2,
				SortPriority = -96,
			},
			{
				Name = "TXT_KEY_MAP_FORCE_COASTAL_START",
				Description = "TXT_KEY_MAP_FORCE_COASTAL_START_DESCRIPTION",
				Values = {
					"TXT_KEY_MAP_YES",
					"TXT_KEY_MAP_NO",
					"TXT_KEY_MAP_RANDOM",
				},
				DefaultValue = 2,
				SortPriority = -95,
			},
			{
				Name = "TXT_KEY_MAP_LAND_SHAPES",
				Description = "TXT_KEY_MAP_LAND_SHAPES_DESCRIPTION",
				Values = {
					"TXT_KEY_MAP_BLOCKY",
					"TXT_KEY_MAP_NORMAL",
					"TXT_KEY_MAP_STRINGY",
					"TXT_KEY_MAP_EXTREMELY_STRINGY",
					"TXT_KEY_MAP_RANDOM",
				},
				DefaultValue = 2,
				SortPriority = -94,
			},
			{
				Name = "TXT_KEY_MAP_STARTING_RESOURCES",
				Description = "TXT_KEY_MAP_STARTING_RESOURCES_DESCRIPTION",
				Values = {
					"TXT_KEY_MAP_ADD_BOTH",
					"TXT_KEY_MAP_NORMAL",
					"TXT_KEY_MAP_ADD_SOME_STRATEGICS",
					"TXT_KEY_MAP_ADD_SOME_EXTRA_BONUS",
				},
				DefaultValue = 2,
				SortPriority = -93,
			},
			{
				Name = "TXT_KEY_MAP_STRATEGIC_DEPOSIT_SIZE",
				Description = "TXT_KEY_MAP_STRATEGIC_DEPOSIT_SIZE_DESCRIPTION",
				Values = {
					"TXT_KEY_MAP_SMALL",
					"TXT_KEY_MAP_NORMAL",
					"TXT_KEY_MAP_LARGE",
					"TXT_KEY_MAP_RANDOM",
				},
				DefaultValue = 2,
				SortPriority = -92,
			},
			{
				Name = "TXT_KEY_MAP_BONUS_RESOURCE_DENSITY",
				Description = "TXT_KEY_MAP_BONUS_RESOURCE_DENSITY_DESCRIPTION",
				Values = {
					"TXT_KEY_MAP_SPARSE",
					"TXT_KEY_MAP_NORMAL",
					"TXT_KEY_MAP_ABUNDANT",
					"TXT_KEY_MAP_RANDOM",
				},
				DefaultValue = 2,
				SortPriority = -91,
			},
			{
				Name = "TXT_KEY_MAP_STRATEGIC_RESOURCE_DENSITY",
				Description = "TXT_KEY_MAP_STRATEGIC_RESOURCE_DENSITY_DESCRIPTION",
				Values = {
					"TXT_KEY_MAP_SPARSE",
					"TXT_KEY_MAP_NORMAL",
					"TXT_KEY_MAP_ABUNDANT",
					"TXT_KEY_MAP_RANDOM",
				},
				DefaultValue = 2,
				SortPriority = -90,
			},
			{
				Name = "TXT_KEY_MAP_LUXURY_RESOURCE_DENSITY",
				Description = "TXT_KEY_MAP_LUXURY_RESOURCE_DENSITY_DESCRIPTION",
				Values = {
					"TXT_KEY_MAP_SPARSE",
					"TXT_KEY_MAP_NORMAL",
					"TXT_KEY_MAP_ABUNDANT",
					"TXT_KEY_MAP_RANDOM",
				},
				DefaultValue = 2,
				SortPriority = -89,
			},
			{
				Name = "TXT_KEY_MAP_OVERRIDE_ASSIGN_STARTING_PLOTS",
				Description = "TXT_KEY_MAP_OVERRIDE_ASSIGN_STARTING_PLOTS_DESCRIPTION",
				Values = {
					"TXT_KEY_MAP_NO",
					"TXT_KEY_MAP_YES",
				},
				DefaultValue = 2,
				SortPriority = -88,
			},
		},
	};
end

function GetMapInitData(worldSize)
	print("GetMapInitData");
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {39, 28},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {56, 38},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {69, 46},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {79, 53},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {87, 59},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {97, 66},
	};

	if Map.GetCustomOption(5) == 1 then
		-- Enlarge terra-style maps 30% to create expansion room on the new world
		worldsizes = {
			[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {44, 31},
			[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {64, 43},
			[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {78, 52},
			[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {90, 60},
			[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {99, 66},
			[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {109, 74},
		};
	end

	local grid_size = worldsizes[worldSize];
	if GameInfo.Worlds[worldSize] then
		return {
			Width = grid_size[1],
			Height = grid_size[2],
			WrapX = true,
		};
	end
end

function DetermineContinents()
	print("Determining continents for art purposes (CommunitasMap)");
	-- Each plot has a continent art type.
	-- Command for setting the art type for a plot is: <plot object>:SetContinentArtType(<art_set_number>)

	-- CONTINENTAL ART SETS - in order from hot to cold
	-- 0) Ocean
	-- 3) Africa
	-- 2) Asia
	-- 1) America
	-- 4) Europe

	local contArt = {};
	contArt.OCEAN = 0;
	contArt.AFRICA = 3;
	contArt.ASIA = 2;
	contArt.AMERICA = 1;
	contArt.EUROPE = 4;

	local mapW, mapH = Map.GetGridSize();

	--[[
 	for _, plot in Plots() do
 		if plot:IsWater() then
 			plot:SetContinentArtType(contArt.OCEAN);
 		else
 			plot:SetContinentArtType(contArt.AFRICA);
 		end
 	end
	--]]

	local continentMap = PWAreaMap:New(elevationMap.width, elevationMap.height, elevationMap.wrapX, elevationMap.wrapY);
	continentMap:DefineAreas(OceanMatch);
	table.sort(continentMap.areaList, function (a, b) return a.size > b.size end);

	-- Determine area artstyle
	for n = 1, #continentMap.areaList do
		if not continentMap.areaList[n].trueMatch then
			continentMap.areaList[n].artStyle = 1 + Map.Rand(4, "Continent Art Styles - Lua");
		end
	end
	for y = 0, elevationMap.height - 1 do
		for x = 0, elevationMap.width - 1 do
			local plot = Map.GetPlot(x, y);
			local i = elevationMap:GetIndex(x, y);
			local landArtStyle = continentMap:GetAreaByID(continentMap.data[i]).artStyle;
			if plot:IsWater() then
				plot:SetContinentArtType(contArt.OCEAN);
			else
				plot:SetContinentArtType(landArtStyle);
			end
		end
	end

	-- Africa has the best looking deserts, so for the biggest desert use Africa.
	-- America has a nice dirty looking desert also, so that should be the second biggest desert.
	local desertMap = PWAreaMap:New(elevationMap.width, elevationMap.height, elevationMap.wrapX, elevationMap.wrapY);
	desertMap:DefineAreas(DesertMatch);
	table.sort(desertMap.areaList, function (a, b) return a.size > b.size end);
	local largestDesertID;
	local secondLargestDesertID;
	for n = 1, #desertMap.areaList do
		-- print(string.format("area[%d] size = %d", n, desertMap.areaList[n].size));
		if desertMap.areaList[n].trueMatch then
			if not largestDesertID then
				largestDesertID = desertMap.areaList[n].id;
			else
				secondLargestDesertID = desertMap.areaList[n].id;
				break;
			end
		end
	end
	for y = 0, elevationMap.height - 1 do
		for x = 0, elevationMap.width - 1 do
			local plot = Map.GetPlot(x, y);
			local i = elevationMap:GetIndex(x, y);
			if desertMap.data[i] == largestDesertID then
				plot:SetContinentArtType(contArt.AFRICA);
			elseif desertMap.data[i] == secondLargestDesertID then
				plot:SetContinentArtType(contArt.AMERICA);
			elseif plot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
				plot:SetContinentArtType(contArt.ASIA);
			end
		end
	end

	-- Set tundra/mountains -> snowy when near snow tiles
	for y = 0, mapH - 1 do
		for x = 0, mapW - 1 do
			local plot = Map.GetPlot(x, y);
			local plotTerrainID = plot:GetTerrainType();
			if Plot_IsMountain(plot) then
				local coldness = 0;
				local zone = elevationMap:GetZone(y);

				if (zone == MG.NPOLAR or zone == MG.SPOLAR) then
					coldness = coldness + 2;
				elseif (zone == MG.NTEMPERATE or zone == MG.STEMPERATE) then
					coldness = coldness + 1;
				else
					coldness = coldness - 1;
				end

				for nearPlot in Plot_GetPlotsInCircle(plot, 1, 1) do
					local nearTerrainID = nearPlot:GetTerrainType();
					local nearFeatureID = nearPlot:GetFeatureType();
					if Plot_IsMountain(nearPlot) then
						coldness = coldness + 0.5;
					elseif nearTerrainID == TerrainTypes.TERRAIN_SNOW then
						coldness = coldness + 2;
					elseif nearTerrainID == TerrainTypes.TERRAIN_TUNDRA then
						coldness = coldness + 1;
					elseif nearTerrainID == TerrainTypes.TERRAIN_DESERT then
						coldness = coldness - 1;
					elseif nearFeatureID == FeatureTypes.FEATURE_JUNGLE or nearFeatureID == FeatureTypes.FEATURE_MARSH then
						coldness = coldness - 8;
					end
				end

				for nearPlot in Plot_GetPlotsInCircle(plot, 2, 2) do
					if Plot_IsMountain(nearPlot) then
						coldness = coldness + 0.25;
					end
				end

				-- Avoid snow near tropical jungle
				if coldness >= 1 then
					for nearPlot in Plot_GetPlotsInCircle(plot, 2, 3) do
						local nearFeatureID = nearPlot:GetFeatureType();
						if nearFeatureID == FeatureTypes.FEATURE_JUNGLE or nearFeatureID == FeatureTypes.FEATURE_MARSH then
							coldness = coldness - 8 / math.max(1, Map.PlotDistance(x, y, nearPlot:GetX(), nearPlot:GetY()));
						end
					end
				end

				if coldness >= 6 then
					plot:SetContinentArtType(contArt.EUROPE);
				elseif coldness >= 4 then
					plot:SetContinentArtType(contArt.AMERICA);
				elseif coldness >= 2 then
					plot:SetContinentArtType(contArt.ASIA);
				else
					plot:SetContinentArtType(contArt.AFRICA);
				end

			elseif plotTerrainID == TerrainTypes.TERRAIN_TUNDRA then
				local coldness = 0;
				for nearPlot in Plot_GetPlotsInCircle(plot, 1, 1) do
					local nearTerrainID = nearPlot:GetTerrainType();
					local nearFeatureID = nearPlot:GetFeatureType();
					if nearTerrainID == TerrainTypes.TERRAIN_SNOW then
						coldness = coldness + 5;
					elseif nearTerrainID == TerrainTypes.TERRAIN_TUNDRA then
						coldness = coldness + 1;
					elseif nearTerrainID == TerrainTypes.TERRAIN_DESERT or nearFeatureID == FeatureTypes.FEATURE_JUNGLE or nearFeatureID == FeatureTypes.FEATURE_MARSH then
						coldness = coldness - 2;
					end
				end
				for nearPlot in Plot_GetPlotsInCircle(plot, 2, 2) do
					local nearTerrainID = nearPlot:GetTerrainType();
					local nearFeatureID = nearPlot:GetFeatureType();
					if nearTerrainID == TerrainTypes.TERRAIN_DESERT or nearFeatureID == FeatureTypes.FEATURE_JUNGLE or nearFeatureID == FeatureTypes.FEATURE_MARSH then
						coldness = coldness - 1;
					end
				end
				if coldness >= 6 then
					if plot:GetFeatureType() == FeatureTypes.FEATURE_FOREST then
						plot:SetContinentArtType(contArt.ASIA);
					else
						plot:SetContinentArtType(contArt.EUROPE);
					end
				else
					plot:SetContinentArtType(contArt.AFRICA);
				end
			elseif plotTerrainID == TerrainTypes.TERRAIN_SNOW then
				plot:SetContinentArtType(contArt.EUROPE);
			end
		end
	end
end

function InheritsFrom(baseClass)
	-- inheritance mechanism from http://www.gamedev.net/community/forums/topic.asp?topic_id=561909
	local new_class = {};
	local class_mt = {__index = new_class};

	function new_class:create()
		local newinst = {};
		setmetatable(newinst, class_mt);
		return newinst;
	end

	if baseClass then
		setmetatable(new_class, {__index = baseClass});
	end

	-- Implementation of additional OO properties starts here
	-- Return the class object of the instance
	function new_class:class()
		return new_class;
	end

	-- Return the super class object of the instance, optional base class of the given class (must be part of hiearchy)
	function new_class:baseClass(class)
		return new_class:_B(class);
	end

	-- Return the super class object of the instance, optional base class of the given class (must be part of hiearchy)
	function new_class:_B(class)
		if not class or new_class == class then
			return baseClass;
		elseif baseClass then
			return baseClass:_B(class);
		end
		return nil;
	end

	-- Return true if the caller is an instance of theClass
	function new_class:_ISA(theClass)
		local b_isa = false;
		local cur_class = new_class;
		while cur_class and not b_isa do
			if cur_class == theClass then
				b_isa = true;
			else
				cur_class = cur_class:baseClass();
			end
		end
		return b_isa;
	end

	return new_class;
end

--------------------------
-- Generate Plots
--------------------------
function StartPlotSystem()
	if __CustomInit then
		AssignStartingPlots.__CustomInit = __CustomInit;
	end

	-- Get Resources setting input by user.
	local resDensity = Map.GetCustomOption(14) or 2;
	local resSize = Map.GetCustomOption(12) or 2;
	local resBalance = (Map.GetCustomOption(11) == 1 or Map.GetCustomOption(11) == 3);
	local bonusDensity = Map.GetCustomOption(13) or 2;
	local legStart = (Map.GetCustomOption(11) == 1 or Map.GetCustomOption(11) == 4);
	local luxuryDensity = Map.GetCustomOption(15) or 2;

	if resSize == 4 then
		resSize = 1 + Map.Rand(3, "Random Strategic Deposit Size");
	end

	if Map.GetCustomOption(16) ~= 2 then
		-- Legendary Balance, Strategic Balance and Resource Density are one option only
		if resBalance then
			resDensity = 5;
		elseif legStart then
			resDensity = 4;
		elseif resDensity == 4 then
			resDensity = 1 + Map.Rand(3, "Random Resource Density");
		end
	end

	local oStarts = Map.GetCustomOption(5);
	local divMethod;
	if oStarts == 2 then
		-- Continents
		divMethod = RegionDivision.CONTINENTAL;
	else
		-- Terra
		divMethod = RegionDivision.BIGGEST_LANDMASS;
	end

	print("Creating start plot database.");
	local start_plot_database = AssignStartingPlots.Create();

	print("Dividing the map in to Regions.");
	local args = {
		method = divMethod,
		resources = resDensity,
		size = resSize,
		balance = resBalance,
		bonus = bonusDensity,
		legendary = legStart,
		lux = luxuryDensity,
		comm = true,
	};
	start_plot_database:GenerateRegions(args);

	-- Do we need to force starts along the ocean?
	local coastalStarts = Map.GetCustomOption(9);
	if coastalStarts == 3 then
		coastalStarts = 1 + Map.Rand(2, "Random Coastal Starts");
	end

	print("Choosing start locations for civilizations.");
	print("Coastal Starts Option", coastalStarts);
	start_plot_database:ChooseLocations{mustBeCoast = (coastalStarts == 1)};

	print("Normalizing start locations and assigning them to Players.");
	start_plot_database:BalanceAndAssign();

	print("Placing Natural Wonders.");
	start_plot_database:PlaceNaturalWonders();

	print("Placing Resources and City States.");
	start_plot_database:PlaceResourcesAndCityStates();
end

function GeneratePlotTypes()
	print("Creating initial map data - CommunitasMap");

	local mapW, mapH = Map.GetGridSize();

	-- First do all the preliminary calculations in this function
	MG = MapGlobals:New();
	PWRandSeed();

	-- Elevations
	elevationMap = GenerateElevationMap(mapW, mapH, true, false);
	-- elevationMap:Save("elevationMap.csv");

	-- Plots
	print("Generating plot types - CommunitasMap");
	ShiftMaps();
	DiffMap = GenerateDiffMap(mapW, mapH, true, false);
	CreateArcticOceans(); -- Makes a line of water in the poles
	CreateVerticalOceans();
	CreateMagallanes(); -- Circumnavegable path (by tu_79)
	ConnectSeasToOceans();
	FillInLakes();
	SetOceanRiftElevations();
	ConnectTerraContinents();

	-- Rainfall
	local summerMap, winterMap;
	summerMap, winterMap, temperatureMap = GenerateTempMaps();
	rainfallMap = GenerateRainfallMap(summerMap, winterMap);
	-- rainfallMap:Save("rainfallMap.csv");

	-- Rivers
	riverMap = RiverMap:New();
	riverMap:SetJunctionAltitudes();
	riverMap:SiltifyLakes();
	riverMap:SetFlowDestinations();
	riverMap:SetRiverSizes();

	-- Find exact elevation thresholds
	local hillsThreshold = DiffMap:FindThresholdFromPercent(MG.flatPercent, false, true);
	local mountainsThreshold = DiffMap:FindThresholdFromPercent(MG.belowMountainPercent, false, true);
	local i = 0;
	for y = 0, mapH - 1 do
		for x = 0, mapW - 1 do
			local plot = Map.GetPlot(x, y);
			if elevationMap:IsBelowSeaLevel(x, y) then
				plot:SetPlotType(PlotTypes.PLOT_OCEAN, false, true);
			elseif DiffMap.data[i] < hillsThreshold then
				plot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
			-- This code makes the game only ever plot flat land if it's within two tiles of the seam.
			-- This prevents issues with tiles that don't look like what they are.
			elseif x == 0 or x == 1 or x == mapW - 1 or x == mapW - 2 then
				plot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
			-- Bobert13
			elseif DiffMap.data[i] < mountainsThreshold then
				plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
			else
				plot:SetPlotType(PlotTypes.PLOT_MOUNTAIN, false, true);
			end
			i = i + 1;
		end
	end
	Map.RecalculateAreas();
	GenerateIslands();
	GenerateCoasts();
	SetOceanRiftPlots();
end

function ConnectSeasToOceans()
	local areaMap = PWAreaMap:New(elevationMap.width, elevationMap.height, elevationMap.wrapX, elevationMap.wrapY);
	areaMap:DefineAreas(WaterMatch);
	local oceanArea, oceanSize = GetLargestArea(areaMap);

	if not oceanArea then
		print("ConnectSeasToOceans: No ocean!");
		return;
	end

	-- sort the ocean list from largest to smallest
	table.sort(areaMap.areaList, function (a, b) return a.size > b.size end);

	local plotFunc = function(plot)
		return not Plot_IsWater(plot, true);
	end

	-- print("ConnectSeasToOceans: oceanSize = %s", oceanSize);
	local newWater = {};
	for areaID = 1, #areaMap.areaList do
		local seaArea = areaMap.areaList[areaID];
		if seaArea.trueMatch and seaArea.size < oceanSize then
			local pathPlots, distance, airDistance = GetPathBetweenAreas(areaMap, seaArea, oceanArea, true, plotFunc);
			if seaArea.size >= MG.lakeSize then
				print("ConnectSeasToOceans: Connect seaArea.size = ", seaArea.size, " distance = ", distance, " airDistance = ", airDistance);
				for _, plot in pairs(pathPlots) do
					newWater[Plot_GetID(plot)] = elevationMap.seaLevelThreshold - 0.01;
					print("ConnectSeasToOceans: x = ", plot:GetX(), " y = ", plot:GetY(), "elevation = ", GetElevationByPlotID(Plot_GetID(plot)));
					table.insert(MG.seaPlots, plot);
				end
			end
		end
	end
	for plotID, elevation in pairs(newWater) do
		elevationMap.data[plotID] = elevation;
	end
end

function ConnectPolarSeasToOceans()
	local areaMap = PWAreaMap:New(elevationMap.width, elevationMap.height, elevationMap.wrapX, elevationMap.wrapY);
	areaMap:DefineAreas(OceanButNotIceMatch);
	local oceanArea, oceanSize = GetLargestArea(areaMap);

	if not oceanArea then
		print("ConnectPolarSeasToOceans: No ocean!");
		return;
	end

	-- Sort the ocean list from largest to smallest
	table.sort(areaMap.areaList, function (a, b) return a.size > b.size end);

	local plotFunc = function(plot)
		if Map.GetCustomOption(5) == 1 then
			-- Terra-style formation, don't remove land
			return plot:GetFeatureType() == FeatureTypes.FEATURE_ICE;
		end
		return not Plot_IsWater(plot, false) or plot:GetFeatureType() == FeatureTypes.FEATURE_ICE or Plot_IsLake(plot);
	end

	-- print("ConnectPolarSeasToOceans: oceanSize =", oceanSize);
	local newWater = {};
	for areaID = 1, #areaMap.areaList do
		local seaArea = areaMap.areaList[areaID];
		if seaArea.trueMatch and seaArea.size < oceanSize then
			local pathPlots, distance, airDistance = GetPathBetweenAreas(areaMap, seaArea, oceanArea, true, plotFunc);
			print("ConnectPolarSeasToOceans: Connect seaArea.size =", seaArea.size, " distance =", distance, " airDistance =", airDistance);
			for _, plot in pairs(pathPlots) do
				print("ConnectPolarSeasToOceans: x =", plot:GetX(), "y =", plot:GetY(), "elevation =", GetElevationByPlotID(Plot_GetID(plot)));
				local plotID = Plot_GetID(plot);
				if plotID then
					newWater[plotID] = elevationMap.seaLevelThreshold - 0.01;
				end
				plot:SetPlotType(PlotTypes.PLOT_OCEAN, false, true);
				plot:SetTerrainType(TerrainTypes.TERRAIN_COAST, false, true);
				plot:SetFeatureType(FeatureTypes.NO_FEATURE);
			end
			for _, plot in pairs(pathPlots) do
				local foundLand = false;
				for nearPlot in Plot_GetPlotsInCircle(plot, 1, 2) do
					if not Plot_IsWater(nearPlot) then
						foundLand = true;
						break;
					end
				end
				if not foundLand then
					plot:SetTerrainType(TerrainTypes.TERRAIN_OCEAN, false, true);
				end
			end
		end
	end
	for plotID, elevation in pairs(newWater) do
		elevationMap.data[plotID] = elevation;
	end
end

function ConnectTerraContinents()
	if Map.GetCustomOption(5) == 2 then
		-- Continents-style formation
		return;
	end

	print("ConnectTerraContinents");

	local continents = {};
	local totalLand = 0;
	local areaMap = PWAreaMap:New(elevationMap.width, elevationMap.height, elevationMap.wrapX, elevationMap.wrapY);

	areaMap:DefineAreas(LandMatch);

	for areaID = 1, #areaMap.areaList do
		local area = areaMap.areaList[areaID];
		if area.trueMatch and area.size > 10 then
			table.insert(continents, area);
		end
	end

	if continents == {} then
		print("ConnectTerraContinents: No biggest continent!");
		return;
	end

	for _, elevation in pairs(elevationMap.data) do
		if elevation > elevationMap.seaLevelThreshold then
			totalLand = totalLand + 1;
		end
	end

	table.sort(continents, function(a, b) return a.size > b.size end);

	print(string.format("ConnectTerraContinents: largestLand = %s%% of %s totalLand", Round(100 * continents[1].size / totalLand), totalLand));

	local largestSize = continents[1].size;
	if largestSize > MG.percentLargestContinent * totalLand then
		return;
	end

	local plotFunc = function(plot)
		return Plot_IsWater(plot, true);
	end

	for i = 1, #continents do
		if i == 1 then
			continents[i].pathPlots = {};
			continents[i].airDistance = 0;
			continents[i].distance = 0;
		else
			local pathPlots, distance, airDistance = GetPathBetweenAreas(areaMap, continents[i], continents[1], false, plotFunc);
			continents[i].pathPlots = pathPlots;
			continents[i].airDistance = airDistance;
			continents[i].distance = distance;
		end
	end

	for index, area in ipairs(continents) do
		print(string.format("ConnectTerraContinents: continent #%-2s size = %-4s distance = %-3s size/distance = %s",
			index, area.size, area.distance, Round(area.size / math.max(1, area.distance))));
	end

	local newLand = {};
	for index, area in ipairs(continents) do
		if index ~= 1 and area.distance < MG.oceanRiftWidth + 2 and area.size / math.max(1, area.distance) > MG.terraConnectWeight then
			print("ConnectTerraContinents: Connect continent", index, "size =", "distance =", area.distance, "airDistance =", area.airDistance);
			for _, plot in pairs(area.pathPlots) do
				newLand[Plot_GetID(plot)] = elevationMap.seaLevelThreshold;
				print("ConnectTerraContinents: x =", plot:GetX(), " y =", plot:GetY(), "elevation =", GetElevationByPlotID(Plot_GetID(plot)));
			end
			largestSize = largestSize + area.size;
			if largestSize > MG.percentLargestContinent * totalLand then
				break;
			end
		end
	end

	for plotID, elevation in pairs(newLand) do
		elevationMap.data[plotID] = elevation;
	end
end

function GetPathBetweenAreas(areaMap, areaA, areaB, findLowest, plotMatchFunc)
	-- Using Dijkstra's algorithm
	local mapW, mapH = Map.GetGridSize();

	-- Initialize
	local plots = {};
	for plotID = 0, areaMap.length - 1 do
		plots[plotID] = {};
		plots[plotID].plot = Map.GetPlot(elevationMap:GetXYFromIndex(plotID));
		plots[plotID].areaID = areaMap.data[plotID];
		local y = plots[plotID].plot:GetY();
		if y < 2 or y > mapH - 3 then
			-- No digging here!
			plots[plotID].elevation = 10000;
		elseif plots[plotID].areaID == areaA.id or plots[plotID].areaID == areaB.id then
			-- Consider all plots equal in start and end areas
			plots[plotID].elevation = 0;
		else
			plots[plotID].elevation = 2 ^ ((GetElevationByPlotID(plotID) - elevationMap.seaLevelThreshold) * 10);
			if plots[plotID].elevation < 1 then
				plots[plotID].elevation = 0;
			end
			if findLowest then
				-- Connect oceans
				if plots[plotID].plot:GetFeatureType() == FeatureTypes.FEATURE_ICE then
					plots[plotID].elevation = 0.5;
				end
			else
				-- Connect continents
			end
		end

		if plots[plotID].areaID == areaA.id then
			plots[plotID].sumElevation = 0;
		else
			plots[plotID].sumElevation = 10000;
		end
	end

	-- Main loop
	local lowestID = -1;
	local lowest;
	local attempts = 0;
	while attempts < mapW * mapH do
		lowestID, lowest = GetBestFromTable(plots,
			function(a, b)
				if not a or not b then
					return a or b;
				end
				if a.sumElevation ~= b.sumElevation then
					return a.sumElevation < b.sumElevation;
				end
				return a.elevation < b.elevation;
			end
		);

		if not lowest or lowest.sumElevation == math.huge or lowest.areaID == areaB.id then
			break;
		end

		plots[lowestID] = nil;
		for nearPlot in Plot_GetPlotsInCircle(lowest.plot, 1) do
			local nearID = Plot_GetID(nearPlot);
			local nearPlotInfo = plots[nearID];
			if nearPlotInfo then
				local altSumElevation = lowest.sumElevation + nearPlotInfo.elevation;
				if altSumElevation < nearPlotInfo.sumElevation then
					plots[nearID].sumElevation = altSumElevation;
					plots[nearID].previous = lowest;
				end
			end
		end
	end

	local path = {};
	local start = lowest;
	while lowest.previous do
		if plotMatchFunc(lowest.plot) then
			table.insert(path, lowest.plot);
		end
		lowest = lowest.previous;
	end
	return path, #path, Map.PlotDistance(start.plot:GetX(), start.plot:GetY(), lowest.plot:GetX(), lowest.plot:GetY());
end

function GetBestFromTable(list, compareFunc)
	local least;
	local leastID = -1;
	for k, v in pairs(list) do
		if compareFunc(v, least) then
			leastID = k;
			least = v;
		end
	end
	return leastID, least;
end

function GetLargestArea(areaMap)
	local largestArea;
	local largestSize = 0;
	for areaID = 1, #areaMap.areaList do
		local area = areaMap.areaList[areaID];
		if area.trueMatch and area.size > largestSize then
			largestSize = area.size;
			largestArea = area;
		end
	end
	return largestArea, largestSize;
end

function FillInLakes()
	local areaMap = PWAreaMap:New(elevationMap.width, elevationMap.height, elevationMap.wrapX, elevationMap.wrapY);
	areaMap:DefineAreas(WaterMatch);
	for i = 1, #areaMap.areaList do
		local area = areaMap.areaList[i];
		if area.trueMatch and area.size <= MG.lakeSize then
			for n = 0, areaMap.length do
				if areaMap.data[n] == area.id then
					local _, y = areaMap:GetXYFromIndex(n);
					local _, mapH = Map.GetGridSize();
					if y <= 2 or y >= mapH - 3 then
						-- These can't be lakes as they're ice or bordering ice
						elevationMap.data[n] = elevationMap.seaLevelThreshold + 0.01;
					else
						elevationMap.data[n] = elevationMap.seaLevelThreshold;
						table.insert(MG.lakePlots, Map.GetPlot(elevationMap:GetXYFromIndex(n)));
					end
				end
			end
		end
	end
end

function RestoreLakes()
	-- This actually sets the terrain type of lakes, as well as fixing rivers.
	print("RestoreLakes");

	--[[
	-- Seperate lakes from new ocean rifts
	for index, plot in pairs(MG.lakePlots) do
		local isLake = true;
		for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
			if nearPlot:IsWater() and not nearPlot:IsLake() then
				table.remove(MG.lakePlots, index);
				break;
			end
		end
	end
	--]]

	-- Remove all rivers bordering lakes or oceans
	for _, plot in Plots() do
		if Plot_IsWater(plot) then
			for edgeDirection = 0, DirectionTypes.NUM_DIRECTION_TYPES - 1 do
				Plot_SetRiver(plot, edgeDirection, MG.flowNONE);
			end
		end
	end

	-- Add lakes
	for _, plot in pairs(MG.lakePlots) do
		plot:SetTerrainType(TerrainTypes.TERRAIN_COAST, false, true);
	end

	-- Calculate outflow from lakes
	local riversToAdd = {};
	local lakesDone = {};
	for _, plot in Plots(Shuffle) do
		for edgeDirection = 0, DirectionTypes.NUM_DIRECTION_TYPES - 1 do
			if Plot_IsRiver(plot, edgeDirection) then
				local prevPlot, edgeA, edgeB, flowA, flowB = Plot_GetPreviousRiverPlot(plot, edgeDirection);
				if prevPlot and Plot_IsLake(prevPlot) and not Contains(lakesDone, prevPlot) then
					print(string.format(
						"%2s flowing river: add edge %2s flowing %2s, edge %2s flowing %2s",
						MG.flowNames[Plot_GetRiverFlowDirection(plot, edgeDirection)],
						MG.directionNames[edgeA],
						MG.flowNames[flowA],
						MG.directionNames[edgeB],
						MG.flowNames[flowB]
					));
					table.insert(riversToAdd, {plot = prevPlot, edge = edgeA, flow = flowA});
					table.insert(riversToAdd, {plot = prevPlot, edge = edgeB, flow = flowB});
					table.insert(lakesDone, prevPlot);
				end
			end
		end
	end

	for _, v in pairs(riversToAdd) do
		Plot_SetRiver(v.plot, v.edge, v.flow);
	end
end

function AddLakes()
	-- Disable vanilla lake creation
end

--------------------------
-- Generate Terrain
--------------------------

function GenerateTerrain()
	print("Generating terrain - CommunitasMap");
	local timeStart = debugTime and os.clock() or 0;

	local mapW, mapH = Map.GetGridSize();

	-- First find minimum rain above sea level for a soft desert transition
	local minRain = 100.0;
	for y = 0, mapH - 1 do
		for x = 0, mapW - 1 do
			local i = elevationMap:GetIndex(x, y);
			if not elevationMap:IsBelowSeaLevel(x, y) then
				if rainfallMap.data[i] < minRain then
					minRain = rainfallMap.data[i];
				end
			end
		end
	end

	-- Find exact thresholds
	local desertThreshold = rainfallMap:FindThresholdFromPercent(MG.desertPercent, false, true);
	local plainsThreshold = rainfallMap:FindThresholdFromPercent(MG.plainsPercent, false, true);
	for y = 0, mapH - 1 do
		for x = 0, mapW - 1 do
			local i = elevationMap:GetIndex(x, y);
			local plot = Map.GetPlot(x, y);
			if not elevationMap:IsBelowSeaLevel(x, y) then
				if temperatureMap.data[i] < MG.snowTemperature then
					if rainfallMap.data[i] < desertThreshold then
						plot:SetTerrainType(TerrainTypes.TERRAIN_SNOW, false, true);
					else
						plot:SetTerrainType(TerrainTypes.TERRAIN_TUNDRA, false, true);
					end
				elseif temperatureMap.data[i] < MG.tundraTemperature then
					if rainfallMap.data[i] < plainsThreshold then
						plot:SetTerrainType(TerrainTypes.TERRAIN_TUNDRA, false, true);
					else
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true);
					end
				elseif temperatureMap.data[i] < MG.jungleMinTemperature then
					-- Here is where most life happens
					if rainfallMap.data[i] < desertThreshold then
						-- It's a cold desert
						plot:SetTerrainType(TerrainTypes.TERRAIN_DESERT, false, true);
					elseif rainfallMap.data[i] < plainsThreshold then
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true);
					else
						plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true);
					end
				elseif temperatureMap.data[i] < MG.desertMinTemperature then
					-- In the tropics, jungle can happen with enough rain.
					if rainfallMap.data[i] < desertThreshold then
						-- Hot desert
						plot:SetTerrainType(TerrainTypes.TERRAIN_DESERT, false, true);
					else
						-- Except where rain is high.
						plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true);
					end
				else
					-- Temperatures too extreme for any life.
					if rainfallMap.data[i] < plainsThreshold then
						plot:SetTerrainType(TerrainTypes.TERRAIN_DESERT, false, true);
					else
						-- Only most humid terrain gets some jungle
						plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true);
					end
				end
			end
		end
	end

	if debugTime then
		timeStart = os.clock();
	end

	BlendTerrain();

	if debugTime then
		print(string.format("%5s ms, GenerateTerrain %s", math.floor((os.clock() - timeStart) * 1000), "BlendTerrain"));
	end
end

function GenerateIslands()
	-- A cell system will be used to combine predefined land chunks with randomly generated island groups.
	-- Define the cell traits. (These need to fit correctly with the map grid width and height.)
	local iW, iH = Map.GetGridSize();
	local iCellWidth = iW / 3;
	local iCellHeight = iH / 2;
	local iNumCellColumns = math.floor(iW / iCellWidth);
	local iNumCellRows = math.floor(iH / iCellHeight);
	local iNumTotalCells = iNumCellColumns * iNumCellRows;
	local cell_data = table.fill(false, iNumTotalCells); -- Stores data on map cells in use. All cells begin as empty.
	local iNumCellsInUse = 0;
	local iNumCellTarget = iNumTotalCells;
	local island_chain_PlotTypes = table.fill(PlotTypes.PLOT_OCEAN, iW * iH);

	-- Add randomly generated island groups
	local iNumGroups = iNumCellTarget; -- Should virtually never use all the groups.
	for group = 1, iNumGroups do
		if iNumCellsInUse >= iNumCellTarget then -- Map has reached desired island population.
			print("-"); print("** Number of Island Groups produced:", group - 1); print("-");
			break;
		end

		--[[ Formation Chart
		1. Single Cell, Axis Only
		2. Double Cell, Horizontal, Axis Only
		3. Single Cell With Dots
		4. Double Cell, Horizontal, With Shifted Dots
		5. Double Cell, Vertical, Axis Only
		6. Double Cell, Vertical, With Shifted Dots
		7. Triple Cell, Vertical, With Double Dots
		8. Square of Cells 2x2 With Double Dots
		9. Rectangle 3x2 With Double Dots
		10. Rectangle 2x3 With Double Dots
		--]]

		-- Choose a formation
		local rate_threshold = {};
		local total_appearance_rate, iNumFormations = 0, 0;
		local appearance_rates = { -- These numbers are relative to one another. No specific target total is necessary.
			7, -- #1
			3, -- #2
			15, -- #3
			8, -- #4
			3, -- #5
			6, -- #6
			4, -- #7
			6, -- #8
			4, -- #9
			3, -- #10
		};
		for _, rate in ipairs(appearance_rates) do
			total_appearance_rate = total_appearance_rate + rate;
			iNumFormations = iNumFormations + 1;
		end
		local accumulated_rate = 0;
		for index = 1, iNumFormations do
			local threshold = (appearance_rates[index] + accumulated_rate) * 10000 / total_appearance_rate;
			table.insert(rate_threshold, threshold);
			accumulated_rate = accumulated_rate + appearance_rates[index];
		end
		local formation_type;
		local diceroll = Map.Rand(10000, "Choose formation type - Island Making - Lua");
		for index, threshold in ipairs(rate_threshold) do
			if diceroll <= threshold then -- Choose this formation type.
				formation_type = index;
				break;
			end
		end
		-- Choose cell(s) not in use
		local iNumAttemptsToFindOpenCells = 0;
		local found_unoccupied_cell = false;
		local anchor_cell, cell_x, cell_y;
		while not found_unoccupied_cell do
			if iNumAttemptsToFindOpenCells > 100 then -- Too many attempts on this pass. Might not be any valid locations for this formation.
				print("-"); print("*-* ERROR:  Formation type of", formation_type, "for island group#", group, "unable to find an open space. Switching to single-cell.");
				formation_type = 3; -- Reset formation type.
				iNumAttemptsToFindOpenCells = 0;
			end
			diceroll = 1 + Map.Rand(iNumTotalCells, "Choosing a cell for an island group - Lua");
			if not cell_data[diceroll] then -- Anchor cell is unoccupied.
				-- If formation type is multiple-cell, all secondary cells must also be unoccupied.
				if formation_type == 1 or formation_type == 3 then -- single cell, proceed.
					anchor_cell = diceroll;
					found_unoccupied_cell = true;
				elseif formation_type == 2 or formation_type == 4 then -- double cell, horizontal.
					-- Check to see if anchor cell is in the final column. If so, reject.
					cell_x = math.fmod(diceroll, iNumCellColumns);
					if cell_x ~= 0 then -- Anchor cell is valid, but still have to check near cell.
						if not cell_data[diceroll + 1] then -- Adjacent cell is unoccupied.
							anchor_cell = diceroll;
							found_unoccupied_cell = true;
						end
					end
				elseif formation_type == 5 or formation_type == 6 then -- double cell, vertical.
					-- Check to see if anchor cell is in the final row. If so, reject.
					cell_y = math.modf(diceroll / iNumCellColumns);
					cell_y = cell_y + 1;
					if cell_y < iNumCellRows then -- Anchor cell is valid, but still have to check cell above it.
						if not cell_data[diceroll + iNumCellColumns] then -- Adjacent cell is unoccupied.
							anchor_cell = diceroll;
							found_unoccupied_cell = true;
						end
					end
				elseif formation_type == 7 then -- triple cell, vertical.
					-- Check to see if anchor cell is in the northern two rows. If so, reject.
					cell_y = math.modf(diceroll / iNumCellColumns);
					cell_y = cell_y + 1;
					if cell_y < iNumCellRows - 1 then -- Anchor cell is valid, but still have to check cells above it.
						if not cell_data[diceroll + iNumCellColumns] then -- Cell directly above is unoccupied.
							if not cell_data[diceroll + (iNumCellColumns * 2)] then -- Cell two rows above is unoccupied.
								anchor_cell = diceroll;
								found_unoccupied_cell = true;
							end
						end
					end
				elseif formation_type == 8 then -- square, 2x2.
					-- Check to see if anchor cell is in the final row or column. If so, reject.
					cell_x = math.fmod(diceroll, iNumCellColumns);
					if cell_x ~= 0 then
						cell_y = math.modf(diceroll / iNumCellColumns);
						cell_y = cell_y + 1;
						if cell_y < iNumCellRows then -- Anchor cell is valid. Still have to check the other three cells.
							if not cell_data[diceroll + iNumCellColumns] then
								if not cell_data[diceroll + 1] then
									if not cell_data[diceroll + iNumCellColumns + 1] then -- All cells are open.
										anchor_cell = diceroll;
										found_unoccupied_cell = true;
									end
								end
							end
						end
					end
				elseif formation_type == 9 then -- horizontal, 3x2.
					-- Check to see if anchor cell is too near to an edge. If so, reject.
					cell_x = math.fmod(diceroll, iNumCellColumns);
					if cell_x ~= 0 and cell_x ~= iNumCellColumns - 1 then
						cell_y = math.modf(diceroll / iNumCellColumns);
						cell_y = cell_y + 1;
						if cell_y < iNumCellRows then -- Anchor cell is valid. Still have to check the other cells.
							if not cell_data[diceroll + iNumCellColumns] then
								if not cell_data[diceroll + 1] and not cell_data[diceroll + 2] then
									if not cell_data[diceroll + iNumCellColumns + 1] then
										if not cell_data[diceroll + iNumCellColumns + 2] then -- All cells are open.
											anchor_cell = diceroll;
											found_unoccupied_cell = true;
										end
									end
								end
							end
						end
					end
				elseif formation_type == 10 then -- vertical, 2x3.
					-- Check to see if anchor cell is too near to an edge. If so, reject.
					cell_x = math.fmod(diceroll, iNumCellColumns);
					if cell_x ~= 0 then
						cell_y = math.modf(diceroll / iNumCellColumns);
						cell_y = cell_y + 1;
						if cell_y < iNumCellRows - 1 then -- Anchor cell is valid. Still have to check the other cells.
							if not cell_data[diceroll + iNumCellColumns] then
								if not cell_data[diceroll + 1] then
									if not cell_data[diceroll + iNumCellColumns + 1] then
										if not cell_data[diceroll + (iNumCellColumns * 2)] then
											if not cell_data[diceroll + (iNumCellColumns * 2) + 1] then -- All cells are open.
												anchor_cell = diceroll;
												found_unoccupied_cell = true;
											end
										end
									end
								end
							end
						end
					end
				end
			end
			iNumAttemptsToFindOpenCells = iNumAttemptsToFindOpenCells + 1;
		end
		-- Find Cell X and Y
		cell_x = math.fmod(anchor_cell, iNumCellColumns);
		if cell_x == 0 then
			cell_x = iNumCellColumns;
		end
		cell_y = math.modf(anchor_cell / iNumCellColumns);
		cell_y = cell_y + 1;

		-- print("-"); print("-"); print("* Group# " .. group, "Formation Type: " .. formation_type, "Cell X, Y: " .. cell_x .. ", " .. cell_y);

		-- Create this island group.
		local iWidth, iHeight, fTilt; -- Scope the variables needed for island group creation.
		local plot_data = {};
		local x_shift, y_shift;
		if formation_type == 1 then -- single cell
			local x_shrinkage = Map.Rand(4, "Cell Width adjustment - Lua");
			if x_shrinkage > 2 then
				x_shrinkage = 0;
			end
			local y_shrinkage = Map.Rand(5, "Cell Height adjustment - Lua");
			if y_shrinkage > 2 then
				y_shrinkage = 0;
			end
			x_shift, y_shift = 0, 0;
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua");
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua");
			end
			iWidth = iCellWidth - x_shrinkage;
			iHeight = iCellHeight - y_shrinkage;

			fTilt = Map.Rand(181, "Angle for island chain axis - LUA");

			plot_data = CreateSingleAxisIslandChain(iWidth, iHeight, fTilt);

		elseif formation_type == 2 then -- two cells, horizontal
			local x_shrinkage = Map.Rand(8, "Cell Width adjustment - Lua");
			if x_shrinkage > 5 then
				x_shrinkage = 0;
			end
			local y_shrinkage = Map.Rand(5, "Cell Height adjustment - Lua");
			if y_shrinkage > 2 then
				y_shrinkage = 0;
			end
			x_shift, y_shift = 0, 0;
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua");
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua");
			end
			iWidth = iCellWidth * 2 - x_shrinkage;
			iHeight = iCellHeight - y_shrinkage;

			-- Limit angles to mostly horizontal ones.
			fTilt = 145 + Map.Rand(71, "Angle for island chain axis - LUA");
			if fTilt > 180 then
				fTilt = fTilt - 180;
			end

			plot_data = CreateSingleAxisIslandChain(iWidth, iHeight, fTilt);

		elseif formation_type == 3 then -- single cell, with dots
			local x_shrinkage = Map.Rand(4, "Cell Width adjustment - Lua");
			if x_shrinkage > 2 then
				x_shrinkage = 0;
			end
			local y_shrinkage = Map.Rand(5, "Cell Height adjustment - Lua");
			if y_shrinkage > 2 then
				y_shrinkage = 0;
			end
			x_shift, y_shift = 0, 0;
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua");
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua");
			end
			iWidth = iCellWidth - x_shrinkage;
			iHeight = iCellHeight - y_shrinkage;

			fTilt = Map.Rand(181, "Angle for island chain axis - LUA");

			-- Determine "dot box"
			local iInnerWidth, iInnerHeight = iWidth - 2, iHeight - 2;

			-- Determine number of dots
			local die_1 = Map.Rand(4, "Diceroll - Lua");
			local die_2 = Map.Rand(8, "Diceroll - Lua");
			local iNumDots = 4;
			if die_1 + die_2 > 1 then
				iNumDots = iNumDots + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua");
			else
				iNumDots = iNumDots + die_1 + die_2;
			end

			plot_data = CreateAxisChainWithDots(iWidth, iHeight, fTilt, iInnerWidth, iInnerHeight, iNumDots);

		elseif formation_type == 4 then -- two cells, horizontal, with dots
			local x_shrinkage = Map.Rand(8, "Cell Width adjustment - Lua");
			if x_shrinkage > 5 then
				x_shrinkage = 0;
			end
			local y_shrinkage = Map.Rand(5, "Cell Height adjustment - Lua");
			if y_shrinkage > 2 then
				y_shrinkage = 0;
			end
			x_shift, y_shift = 0, 0;
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua");
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua");
			end
			iWidth = iCellWidth * 2 - x_shrinkage;
			iHeight = iCellHeight - y_shrinkage;

			-- Limit angles to mostly horizontal ones.
			fTilt = 145 + Map.Rand(71, "Angle for island chain axis - LUA");
			if fTilt > 180 then
				fTilt = fTilt - 180;
			end

			-- Determine "dot box"
			local iInnerWidth = math.floor(iWidth / 2);
			local iInnerHeight = iHeight - 2;
			local iInnerWest = 2 + Map.Rand((iWidth - 1) - iInnerWidth, "Shift for sub island group - Lua");
			local iInnerSouth = 2;

			-- Determine number of dots
			local die_1 = Map.Rand(4, "Diceroll - Lua");
			local die_2 = Map.Rand(10, "Diceroll - Lua");
			local iNumDots = 5;
			if die_1 + die_2 > 1 then
				iNumDots = iNumDots + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua");
			else
				iNumDots = iNumDots + die_1 + die_2;
			end

			plot_data = CreateAxisChainWithShiftedDots(iWidth, iHeight, fTilt, iInnerWidth, iInnerHeight, iInnerWest, iInnerSouth, iNumDots);

		elseif formation_type == 5 then -- Double Cell, Vertical, Axis Only
			local x_shrinkage = Map.Rand(5, "Cell Width adjustment - Lua");
			if x_shrinkage > 2 then
				x_shrinkage = 0;
			end
			local y_shrinkage = Map.Rand(7, "Cell Height adjustment - Lua");
			if y_shrinkage > 4 then
				y_shrinkage = 0;
			end
			x_shift, y_shift = 0, 0;
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua");
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua");
			end
			iWidth = iCellWidth - x_shrinkage;
			iHeight = iCellHeight * 2 - y_shrinkage;

			-- Limit angles to mostly vertical ones.
			fTilt = 55 + Map.Rand(71, "Angle for island chain axis - LUA");

			plot_data = CreateSingleAxisIslandChain(iWidth, iHeight, fTilt);

		elseif formation_type == 6 then -- Double Cell, Vertical With Dots
			local x_shrinkage = Map.Rand(5, "Cell Width adjustment - Lua");
			if x_shrinkage > 2 then
				x_shrinkage = 0;
			end
			local y_shrinkage = Map.Rand(7, "Cell Height adjustment - Lua");
			if y_shrinkage > 4 then
				y_shrinkage = 0;
			end
			x_shift, y_shift = 0, 0;
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua");
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua");
			end
			iWidth = iCellWidth - x_shrinkage;
			iHeight = iCellHeight * 2 - y_shrinkage;

			-- Limit angles to mostly vertical ones.
			fTilt = 55 + Map.Rand(71, "Angle for island chain axis - LUA");

			-- Determine "dot box"
			local iInnerWidth = iWidth - 2;
			local iInnerHeight = math.floor(iHeight / 2);
			local iInnerWest = 2;
			local iInnerSouth = 2 + Map.Rand((iHeight - 1) - iInnerHeight, "Shift for sub island group - Lua");

			-- Determine number of dots
			local die_1 = Map.Rand(4, "Diceroll - Lua");
			local die_2 = Map.Rand(10, "Diceroll - Lua");
			local iNumDots = 5;
			if die_1 + die_2 > 1 then
				iNumDots = iNumDots + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua");
			else
				iNumDots = iNumDots + die_1 + die_2;
			end

			plot_data = CreateAxisChainWithShiftedDots(iWidth, iHeight, fTilt, iInnerWidth, iInnerHeight, iInnerWest, iInnerSouth, iNumDots);

		elseif formation_type == 7 then -- Triple Cell, Vertical With Double Dots
			local x_shrinkage = Map.Rand(4, "Cell Width adjustment - Lua");
			if x_shrinkage > 1 then
				x_shrinkage = 0;
			end
			local y_shrinkage = Map.Rand(9, "Cell Height adjustment - Lua");
			if y_shrinkage > 5 then
				y_shrinkage = 0;
			end
			x_shift, y_shift = 0, 0;
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua");
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua");
			end
			iWidth = iCellWidth - x_shrinkage;
			iHeight = iCellHeight * 3 - y_shrinkage;

			-- Limit angles to steep ones.
			fTilt = 70 + Map.Rand(41, "Angle for island chain axis - Lua");

			-- Handle Dots Group 1.
			local iInnerWidth1 = iWidth - 3;
			local iInnerHeight1 = iCellHeight - 1;
			local iInnerWest1 = 2 + Map.Rand(2, "Shift for sub island group - Lua");
			local iInnerSouth1 = 2 + Map.Rand(iCellHeight - 3, "Shift for sub island group - Lua");

			-- Determine number of dots
			local die_1 = Map.Rand(4, "Diceroll - Lua");
			local die_2 = Map.Rand(8, "Diceroll - Lua");
			local iNumDots1 = 4;
			if die_1 + die_2 > 1 then
				iNumDots1 = iNumDots1 + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua");
			else
				iNumDots1 = iNumDots1 + die_1 + die_2;
			end

			-- Handle Dots Group 2.
			local iInnerWidth2 = iWidth - 3;
			local iInnerHeight2 = iCellHeight - 1;
			local iInnerWest2 = 2 + Map.Rand(2, "Shift for sub island group - Lua");
			local iInnerSouth2 = iCellHeight + 2 + Map.Rand(iCellHeight - 3, "Shift for sub island group - Lua");

			-- Determine number of dots
			die_1 = Map.Rand(4, "Diceroll - Lua");
			die_2 = Map.Rand(8, "Diceroll - Lua");
			local iNumDots2 = 4;
			if die_1 + die_2 > 1 then
				iNumDots2 = iNumDots2 + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua");
			else
				iNumDots2 = iNumDots2 + die_1 + die_2;
			end

			plot_data = CreateAxisChainWithDoubleDots(iWidth, iHeight, fTilt, iInnerWidth1, iInnerHeight1, iInnerWest1, iInnerSouth1,
				iNumDots1, iInnerWidth2, iInnerHeight2, iInnerWest2, iInnerSouth2, iNumDots2);

		elseif formation_type == 8 then -- Square Block 2x2 With Double Dots
			local x_shrinkage = Map.Rand(6, "Cell Width adjustment - Lua");
			if x_shrinkage > 4 then
				x_shrinkage = 0;
			end
			local y_shrinkage = Map.Rand(5, "Cell Height adjustment - Lua");
			if y_shrinkage > 3 then
				y_shrinkage = 0;
			end
			x_shift, y_shift = 0, 0;
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua");
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua");
			end
			iWidth = iCellWidth * 2 - x_shrinkage;
			iHeight = iCellHeight * 2 - y_shrinkage;

			-- Full range of angles
			fTilt = Map.Rand(181, "Angle for island chain axis - Lua");

			-- Handle Dots Group 1.
			local iInnerWidth1 = iCellWidth - 2;
			local iInnerHeight1 = iCellHeight - 2;
			local iInnerWest1 = 3 + Map.Rand(iCellWidth - 2, "Shift for sub island group - Lua");
			local iInnerSouth1 = 3 + Map.Rand(iCellHeight - 2, "Shift for sub island group - Lua");

			-- Determine number of dots
			local die_1 = Map.Rand(6, "Diceroll - Lua");
			local die_2 = Map.Rand(10, "Diceroll - Lua");
			local iNumDots1 = 5;
			if die_1 + die_2 > 1 then
				iNumDots1 = iNumDots1 + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua");
			else
				iNumDots1 = iNumDots1 + die_1 + die_2;
			end

			-- Handle Dots Group 2.
			local iInnerWidth2 = iCellWidth - 2;
			local iInnerHeight2 = iCellHeight - 2;
			local iInnerWest2 = 3 + Map.Rand(iCellWidth - 2, "Shift for sub island group - Lua");
			local iInnerSouth2 = 3 + Map.Rand(iCellHeight - 2, "Shift for sub island group - Lua");

			-- Determine number of dots
			die_1 = Map.Rand(4, "Diceroll - Lua");
			die_2 = Map.Rand(8, "Diceroll - Lua");
			local iNumDots2 = 5;
			if die_1 + die_2 > 1 then
				iNumDots2 = iNumDots2 + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua");
			else
				iNumDots2 = iNumDots2 + die_1 + die_2;
			end

			plot_data = CreateAxisChainWithDoubleDots(iWidth, iHeight, fTilt, iInnerWidth1, iInnerHeight1, iInnerWest1, iInnerSouth1,
				iNumDots1, iInnerWidth2, iInnerHeight2, iInnerWest2, iInnerSouth2, iNumDots2);

		elseif formation_type == 9 then -- Horizontal Block 3x2 With Double Dots
			local x_shrinkage = Map.Rand(8, "Cell Width adjustment - Lua");
			if x_shrinkage > 5 then
				x_shrinkage = 0;
			end
			local y_shrinkage = Map.Rand(5, "Cell Height adjustment - Lua");
			if y_shrinkage > 3 then
				y_shrinkage = 0;
			end
			x_shift, y_shift = 0, 0;
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua");
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua");
			end
			iWidth = iCellWidth * 3 - x_shrinkage;
			iHeight = iCellHeight * 2 - y_shrinkage;

			-- Limit angles to mostly horizontal ones.
			fTilt = 145 + Map.Rand(71, "Angle for island chain axis - Lua");
			if fTilt > 180 then
				fTilt = fTilt - 180;
			end

			-- Handle Dots Group 1.
			local iInnerWidth1 = iCellWidth;
			local iInnerHeight1 = iCellHeight - 2;
			local iInnerWest1 = 4 + Map.Rand(iCellWidth + 2, "Shift for sub island group - Lua");
			local iInnerSouth1 = 3 + Map.Rand(iCellHeight - 2, "Shift for sub island group - Lua");

			-- Determine number of dots
			local die_1 = Map.Rand(4, "Diceroll - Lua");
			local die_2 = Map.Rand(8, "Diceroll - Lua");
			local iNumDots1 = 9;
			if die_1 + die_2 > 1 then
				iNumDots1 = iNumDots1 + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua");
			else
				iNumDots1 = iNumDots1 + die_1 + die_2;
			end

			-- Handle Dots Group 2.
			local iInnerWidth2 = iCellWidth;
			local iInnerHeight2 = iCellHeight - 2;
			local iInnerWest2 = 4 + Map.Rand(iCellWidth + 2, "Shift for sub island group - Lua");
			local iInnerSouth2 = 3 + Map.Rand(iCellHeight - 2, "Shift for sub island group - Lua");

			-- Determine number of dots
			die_1 = Map.Rand(5, "Diceroll - Lua");
			die_2 = Map.Rand(7, "Diceroll - Lua");
			local iNumDots2 = 8;
			if die_1 + die_2 > 1 then
				iNumDots2 = iNumDots2 + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua");
			else
				iNumDots2 = iNumDots2 + die_1 + die_2;
			end

			plot_data = CreateAxisChainWithDoubleDots(iWidth, iHeight, fTilt, iInnerWidth1, iInnerHeight1, iInnerWest1, iInnerSouth1,
				iNumDots1, iInnerWidth2, iInnerHeight2, iInnerWest2, iInnerSouth2, iNumDots2);

		elseif formation_type == 10 then -- Vertical Block 2x3 With Double Dots
			local x_shrinkage = Map.Rand(6, "Cell Width adjustment - Lua");
			if x_shrinkage > 4 then
				x_shrinkage = 0;
			end
			local y_shrinkage = Map.Rand(8, "Cell Height adjustment - Lua");
			if y_shrinkage > 5 then
				y_shrinkage = 0;
			end
			x_shift, y_shift = 0, 0;
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua");
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua");
			end
			iWidth = iCellWidth * 2 - x_shrinkage;
			iHeight = iCellHeight * 3 - y_shrinkage;

			-- Mostly vertical
			fTilt = 55 + Map.Rand(71, "Angle for island chain axis - Lua");

			-- Handle Dots Group 1.
			local iInnerWidth1 = iCellWidth - 2;
			local iInnerHeight1 = iCellHeight;
			local iInnerWest1 = 3 + Map.Rand(iCellWidth - 2, "Shift for sub island group - Lua");
			local iInnerSouth1 = 4 + Map.Rand(iCellHeight + 2, "Shift for sub island group - Lua");

			-- Determine number of dots
			local die_1 = Map.Rand(4, "Diceroll - Lua");
			local die_2 = Map.Rand(10, "Diceroll - Lua");
			local iNumDots1 = 8;
			if die_1 + die_2 > 1 then
				iNumDots1 = iNumDots1 + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua");
			else
				iNumDots1 = iNumDots1 + die_1 + die_2;
			end

			-- Handle Dots Group 2.
			local iInnerWidth2 = iCellWidth - 2;
			local iInnerHeight2 = iCellHeight;
			local iInnerWest2 = 3 + Map.Rand(iCellWidth - 2, "Shift for sub island group - Lua");
			local iInnerSouth2 = 4 + Map.Rand(iCellHeight + 2, "Shift for sub island group - Lua");

			-- Determine number of dots
			die_1 = Map.Rand(4, "Diceroll - Lua");
			die_2 = Map.Rand(8, "Diceroll - Lua");
			local iNumDots2 = 7;
			if die_1 + die_2 > 1 then
				iNumDots2 = iNumDots2 + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua");
			else
				iNumDots2 = iNumDots2 + die_1 + die_2;
			end

			plot_data = CreateAxisChainWithDoubleDots(iWidth, iHeight, fTilt, iInnerWidth1, iInnerHeight1, iInnerWest1, iInnerSouth1,
				iNumDots1, iInnerWidth2, iInnerHeight2, iInnerWest2, iInnerSouth2, iNumDots2);
		end

		-- Obtain land plots from the plot data
		local x_adjust = (cell_x - 1) * iCellWidth + x_shift;
		local y_adjust = (cell_y - 1) * iCellHeight + y_shift;
		for y = 1, iHeight do
			for x = 1, iWidth do
				local data_index = (y - 1) * iWidth + x;
				if plot_data[data_index] then -- This plot is land.
					local real_x, real_y = x + x_adjust - 1, y + y_adjust - 1;
					local plot_index = real_y * iW + real_x + 1;
					island_chain_PlotTypes[plot_index] = PlotTypes.PLOT_LAND;
				end
			end
		end

		-- Record cells in use
		if formation_type == 1 then -- single cell
			cell_data[anchor_cell] = true;
			iNumCellsInUse = iNumCellsInUse + 1;
		elseif formation_type == 2 then
			cell_data[anchor_cell], cell_data[anchor_cell + 1] = true, true;
			iNumCellsInUse = iNumCellsInUse + 2;
		elseif formation_type == 3 then
			cell_data[anchor_cell] = true;
			iNumCellsInUse = iNumCellsInUse + 1;
		elseif formation_type == 4 then
			cell_data[anchor_cell], cell_data[anchor_cell + 1] = true, true;
			iNumCellsInUse = iNumCellsInUse + 2;
		elseif formation_type == 5 then
			cell_data[anchor_cell], cell_data[anchor_cell + iNumCellColumns] = true, true;
			iNumCellsInUse = iNumCellsInUse + 2;
		elseif formation_type == 6 then
			cell_data[anchor_cell], cell_data[anchor_cell + iNumCellColumns] = true, true;
			iNumCellsInUse = iNumCellsInUse + 2;
		elseif formation_type == 7 then
			cell_data[anchor_cell], cell_data[anchor_cell + iNumCellColumns] = true, true;
			cell_data[anchor_cell + (iNumCellColumns * 2)] = true;
			iNumCellsInUse = iNumCellsInUse + 3;
		elseif formation_type == 8 then
			cell_data[anchor_cell], cell_data[anchor_cell + 1] = true, true;
			cell_data[anchor_cell + iNumCellColumns], cell_data[anchor_cell + iNumCellColumns + 1] = true, true;
			iNumCellsInUse = iNumCellsInUse + 4;
		elseif formation_type == 9 then
			cell_data[anchor_cell], cell_data[anchor_cell + 1] = true, true;
			cell_data[anchor_cell + iNumCellColumns], cell_data[anchor_cell + iNumCellColumns + 1] = true, true;
			cell_data[anchor_cell + 2], cell_data[anchor_cell + iNumCellColumns + 2] = true, true;
			iNumCellsInUse = iNumCellsInUse + 6;
		elseif formation_type == 10 then
			cell_data[anchor_cell], cell_data[anchor_cell + 1] = true, true;
			cell_data[anchor_cell + iNumCellColumns], cell_data[anchor_cell + iNumCellColumns + 1] = true, true;
			cell_data[anchor_cell + (iNumCellColumns * 2)], cell_data[anchor_cell + (iNumCellColumns * 2) + 1] = true, true;
			iNumCellsInUse = iNumCellsInUse + 6;
		end
	end

	--[[ Debug check of cell occupation.
	print("- - -");
	for loop = iNumCellRows, 1, -1 do
		local c = (loop - 1) * iNumCellColumns;
		local stringdata = {};
		for innerloop = 1, iNumCellColumns do
			if cell_data[c + innerloop] == false then
				stringdata[innerloop] = "false ";
			else
				stringdata[innerloop] = "true  ";
			end
		end
		print("Row:", table.concat(stringdata));
	end
	--]]

	-- Add Hills and Peaks to randomly generated islands.
	local regionHillsFrac = Fractal.Create(iW, iH, 5, {}, 7, 7);
	local regionPeaksFrac = Fractal.Create(iW, iH, 6, {}, 7, 7);
	local iHillsBottom1 = regionHillsFrac:GetHeight(20);
	local iHillsTop1 = regionHillsFrac:GetHeight(35);
	local iHillsBottom2 = regionHillsFrac:GetHeight(65);
	local iHillsTop2 = regionHillsFrac:GetHeight(80);
	local iPeakThreshold = regionPeaksFrac:GetHeight(80);
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local i = y * iW + x + 1;
			if island_chain_PlotTypes[i] ~= PlotTypes.PLOT_OCEAN then
				local hillVal = regionHillsFrac:GetHeight(x, y);
				if (hillVal >= iHillsBottom1 and hillVal <= iHillsTop1) or (hillVal >= iHillsBottom2 and hillVal <= iHillsTop2) then
					local peakVal = regionPeaksFrac:GetHeight(x, y);
					if peakVal >= iPeakThreshold then
						island_chain_PlotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
					else
						island_chain_PlotTypes[i] = PlotTypes.PLOT_HILLS;
					end
				end
			end
		end
	end

	-- Apply island data to the map.
	for y = 2, iH - 3 do -- avoid polar caps
		for x = 0, iW - 1 do
			local i = y * iW + x + 1;
			local plot = Map.GetPlot(x, y);
			if island_chain_PlotTypes[i] ~= PlotTypes.PLOT_OCEAN and Plot_IsWater(plot, true) then
				local isValid = true;
				local numAdjacentLand = 0;
				-- Don't fill river deltas with land
				for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
					if nearPlot:GetPlotType() ~= PlotTypes.PLOT_OCEAN and Map.GetNumTilesOfLandmass(nearPlot:GetLandmass()) >= 10 then
						numAdjacentLand = numAdjacentLand + 1;
						if numAdjacentLand > 1 then
							isValid = false;
							break;
						end
					end
				end
				if isValid then
					plot:SetPlotType(island_chain_PlotTypes[i], false, true);
				else
					-- plot:SetPlotType(PlotTypes.PLOT_MOUNTAIN, false, true);
				end
			end
		end
	end
end

function GetTemperature(myPlot)
	if myPlot:GetTerrainType() == TerrainTypes.TERRAIN_SNOW then
		return 0;
	elseif myPlot:GetTerrainType() == TerrainTypes.TERRAIN_TUNDRA then
		return 1;
	end
	return 2;
end

function BlendTerrain()
	local timeStart = debugTime and os.clock() or 0;
	local mountainCheckTime = 0;
	for _, plot in Plots(Shuffle) do
		if Plot_IsWater(plot) then
			-- Do nothing
		else
			local plotTerrainID = plot:GetTerrainType();
			local plotFeatureID = plot:GetFeatureType();
			local plotPercent = Plot_GetCirclePercents(plot, 1, MG.terrainBlendRange);
			local randPercent = 1 + PWRand() * 2 * MG.terrainBlendRandom - MG.terrainBlendRandom;

			if plot:IsMountain() then
				-- Minimize necessary pathfinding
				local numNearMountains = 0;
				for nearPlot in Plot_GetPlotsInCircle(plot, 1, 1) do
					if nearPlot:IsMountain() then
						numNearMountains = numNearMountains + 1;
					end
				end
				if debugTime then
					timeStart = os.clock();
				end
				if 2 <= numNearMountains and numNearMountains <= 4 then
					CreatePossibleMountainPass(plot);
				end
				if debugTime then
					mountainCheckTime = mountainCheckTime + (os.clock() - timeStart);
				end
			else
				if plotTerrainID == TerrainTypes.TERRAIN_GRASS then
					if plotPercent.TERRAIN_DESERT + plotPercent.TERRAIN_SNOW >= 0.33 * randPercent then
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true);
						if plot:GetFeatureType() == FeatureTypes.FEATURE_MARSH then
							plot:SetFeatureType(FeatureTypes.FEATURE_FOREST);
						end
					end
				--[[
				elseif plotTerrainID == TerrainTypes.TERRAIN_PLAINS then
					if plotPercent.TERRAIN_DESERT >= 0.5 * randPercent then
						plot:SetTerrainType(TerrainTypes.TERRAIN_DESERT, false, true);
					end
				--]]
				elseif plotTerrainID == TerrainTypes.TERRAIN_DESERT then
					if plotPercent.TERRAIN_GRASS + plotPercent.TERRAIN_SNOW >= 0.25 * randPercent then
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true);
					elseif plotPercent.FEATURE_JUNGLE + plotPercent.FEATURE_MARSH >= 0.25 * randPercent then
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true);
					end
				elseif plotFeatureID == FeatureTypes.FEATURE_JUNGLE or plotFeatureID == FeatureTypes.FEATURE_MARSH or plotFeatureID == FeatureTypes.FEATURE_FOREST then
					if plotPercent.TERRAIN_SNOW + plotPercent.TERRAIN_TUNDRA + plotPercent.TERRAIN_DESERT >= 0.25 * randPercent then
						plot:SetFeatureType(FeatureTypes.NO_FEATURE);
					end
				elseif plotTerrainID == TerrainTypes.TERRAIN_TUNDRA then
					if 2 * plotPercent.TERRAIN_GRASS + plotPercent.TERRAIN_PLAINS + plotPercent.TERRAIN_DESERT >= 0.5 * randPercent then
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true);
					end
				end
			end
			if plotTerrainID == TerrainTypes.TERRAIN_SNOW then
				local isMountain = plot:IsMountain();
				local warmCount = 2 * plotPercent.FEATURE_JUNGLE + 2 * plotPercent.FEATURE_MARSH + plotPercent.TERRAIN_GRASS +
					plotPercent.TERRAIN_DESERT + 0.5 * plotPercent.TERRAIN_PLAINS;
				if warmCount >= 0.25 * randPercent then
					plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true);
				elseif warmCount >= 0.10 * randPercent or plot:IsFreshWater() then
					plot:SetTerrainType(TerrainTypes.TERRAIN_TUNDRA, false, true);
				end
				if isMountain then
					plot:SetPlotType(PlotTypes.PLOT_MOUNTAIN, false, true);
				end
			end
			if plot:IsHills() then
				-- Warm hills -> flat when surrounded by cold
				local nearCold = 0;
				for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
					if not nearPlot:IsWater() and not Plot_IsMountain(nearPlot) and GetTemperature(nearPlot) < GetTemperature(plot) then
						nearCold = nearCold + 1;
					end
				end
				if nearCold > 1 and nearCold * MG.hillsBlendPercent * 100 >= Map.Rand(100, "Blend hills - Lua") then
					plot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
				end
			end
		end
	end

	if debugTime then
		print(string.format("%5s ms, BlendTerrain %s", math.floor(mountainCheckTime * 1000), "MountainCheckTime"));
	end

	-- Flat -> hills near mountain, and flat cold -> hills when surrounded by warm
	-- Add flat desert -> hills when surrounded by flat desert too
	for _, plot in Plots(Shuffle) do
		if plot:GetPlotType() == PlotTypes.PLOT_LAND then
			local nearMountains = 0;
			local nearWarm = 0;
			local nearDesert = 0;
			for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
				if not nearPlot:IsWater() then
					if Plot_IsMountain(nearPlot) then
						nearMountains = nearMountains + 1;
					end

					if GetTemperature(nearPlot) > GetTemperature(plot) then
						nearWarm = nearWarm + 1;
					end

					if plot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT and
						nearPlot:GetPlotType() == PlotTypes.PLOT_LAND and
						nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
						nearDesert = nearDesert + 1;
					end
				end
			end
			if nearMountains > 1 and nearMountains * MG.hillsBlendPercent * 10000 >= Map.Rand(10000, "Blend mountains - Lua") then
				-- print("Turning flatland near mountain into hills", nearMountains);
				plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
				-- plot:SetTerrainType(TerrainTypes.TERRAIN_SNOW, false, true);
			elseif nearWarm * 0.5 * MG.hillsBlendPercent * 10000 >= Map.Rand(10000, "Blend hills - Lua") then
				-- print("Turning flatland near warm into hills", nearWarm);
				plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
			elseif nearDesert * 0.5 * MG.hillsBlendPercent * 10000 >= Map.Rand(10000, "Blend hills - Lua") then
				-- print("Turning flat desert surrounded by flat desert into hills", nearDesert);
				plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
			end
		end
	end
end

function CreatePossibleMountainPass(plot)
	local x, y = plot:GetX(), plot:GetY();
	if not plot:IsMountain() then
		return;
	end

	local longestRoute = 0;
	for dirA = 0, 3 do
		local plotA = Map.PlotDirection(x, y, dirA);
		if plotA and (plotA:GetPlotType() == PlotTypes.PLOT_LAND or plotA:GetPlotType() == PlotTypes.PLOT_HILLS) then
			for dirB = dirA + 2, 5 do
				local plotB = Map.PlotDirection(x, y, dirB);
				if plotB and (plotB:GetPlotType() == PlotTypes.PLOT_LAND or plotB:GetPlotType() == PlotTypes.PLOT_HILLS) then
					IsPlotConnected(nil, plotA, plotB, "Land", true);
					if longestRoute < GetRouteLength() then
						longestRoute = GetRouteLength();
					end
					if GetRouteLength() == 0 or GetRouteLength() > 15 then
						print(string.format("CreatePossibleMountainPass path distance = %2s - Change to Hills", GetRouteLength()));
						plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
						table.insert(MG.mountainPasses, plot); -- still consider this a mountain tile for art purposes
						return;
					end
				end
			end
		end
	end
	-- print(string.format("CreatePossibleMountainPass longestRoute = %2s", longestRoute));
end

function CreateArcticOceans()
	local _, mapH = Map.GetGridSize();
	CreateOceanRift{y = 0, direction = MG.E, totalSize = 1, oceanSize = 1, fill = true};
	CreateOceanRift{y = mapH - 1, direction = MG.E, totalSize = 1, oceanSize = 1, fill = true};
end

function CreateVerticalOceans()
	local oOceanRifts = Map.GetCustomOption(6);
	if oOceanRifts == 7 then
		oOceanRifts = 1 + Map.Rand(6, "Random Ocean Rifts");
	end
	if oOceanRifts == 6 then
		-- No vertical rifts
		return;
	end

	print("CreateVerticalOceans MG.oceanRiftWidth =", MG.oceanRiftWidth);

	local mapW, mapH = Map.GetGridSize();
	local landInColumn = {};
	for x = 0, mapW - 1 do
		landInColumn[x] = 0;
		MG.elevationRect[x] = {};
		MG.elevationRect[x][-1] = 0;
		for y = 0, mapH - 1 do
			MG.elevationRect[x][y] = 0;
		end
	end

	-- Scan plots
	local totalLand = 0
	for x = 0, mapW - 1 do
		for y = 0, mapH - 1 do
			local plot = Map.GetPlot(x, y);
			if not Plot_IsWater(plot, true) then
				totalLand = totalLand + 1;
				landInColumn[x] = landInColumn[x] + 1;
			end

			-- Elevation in a size-wide rectangle
			local elevation = Plot_GetElevation(plot, false);
			for x2 = 0 - math.ceil(2 + MG.oceanRiftWidth), 1 + math.floor(2 + MG.oceanRiftWidth) do
				local xOffset = (x + x2) % mapW;
				MG.elevationRect[xOffset][-1] = MG.elevationRect[xOffset][-1] + elevation;
				MG.elevationRect[xOffset][y] = MG.elevationRect[xOffset][y] + elevation;
			end
		end
	end

	-- find biggest ocean (usually Pacific)
	local lowestElevation = 0;
	local startX = 0;
	for x = 0, mapW - 1 do
		local elevation = MG.elevationRect[x][-1];
		if elevation < lowestElevation then
			lowestElevation = elevation;
			startX = x;
		end
	end
	print("startX pacific:", startX);

	if mapW < 60 then
		print("CreateVerticalOceans: Creating Atlantic at x =", startX);
		CreateAtlantic(startX);
		return;
	elseif oOceanRifts == 2 or oOceanRifts == 3 then
		-- PA or PP
		print("CreateVerticalOceans: Creating Pacific at x =", startX);
		CreatePacific(startX);
	elseif oOceanRifts == 1 then
		-- AA
		print("CreateVerticalOceans: Creating Atlantic at x =", startX);
		CreateAtlantic(startX);
	elseif oOceanRifts == 4 or oOceanRifts == 5 then
		-- 1 or 2 random
		if 50 >= Map.Rand(100, "Random ocean rift - Lua") then
			print(string.format("CreateVerticalOceans: Creating Pacific at x = %s (Random)", startX));
			CreatePacific(startX);
		else
			print(string.format("CreateVerticalOceans: Creating Atlantic at x = %s (Random)", startX));
			CreateAtlantic(startX);
		end
	end

	if oOceanRifts == 5 then
		-- Only one rift, end here
		return;
	end

	-- find median land (usually place for Atlantic)
	startX = 0;
	local sumLand = 0;
	local offsetAtlanticPercent = Map.Rand(2, "Atlantic Offset - Lua") == 0 and MG.offsetAtlanticPercent or 1 - MG.offsetAtlanticPercent;
	for x = 0, mapW - 1 do
		local xOffset = (x + startX) % mapW;
		sumLand = sumLand + landInColumn[xOffset];
		if sumLand > offsetAtlanticPercent * totalLand then
			startX = xOffset;
			print("startX atlantic:", startX);
			break;
		end
	end

	print(string.format("totalLand = %4f sumElevation = %4f", totalLand, sumLand));

	if oOceanRifts == 1 or oOceanRifts == 2 then
		-- PA or AA
		print("CreateVerticalOceans: Creating Atlantic at x =", startX);
		CreateAtlantic(startX);
	elseif oOceanRifts == 3 then
		-- PP
		print("CreateVerticalOceans: Creating Pacific at x =", startX);
		CreatePacific(startX);
	elseif oOceanRifts == 4 then
		-- 2 random
		if 50 >= Map.Rand(100, "Random ocean rift - Lua") then
			print(string.format("CreateVerticalOceans: Creating Pacific at x = %s (Random)", startX));
			CreatePacific(startX);
		else
			print(string.format("CreateVerticalOceans: Creating Atlantic at x = %s (Random)", startX));
			CreateAtlantic(startX);
		end
	end
end

function CreatePacific(midline)
	CreateOceanRift{x = midline, totalSize = MG.pacificSize, bulge = MG.pacificBulge, curve = MG.pacificCurve, oceanSize = math.max(1, Round(MG.oceanRiftWidth - 1))};
	print("Create Pacific at:", midline);
end

function CreateAtlantic(midline)
	CreateOceanRift{x = midline, totalSize = MG.atlanticSize, bulge = MG.atlanticBulge, curve = MG.atlanticCurve, oceanSize = MG.oceanRiftWidth, cleanMid = true};
	print("Create Atlantic at:", midline);
end

function CreateOceanRift(args)
	print("CreateOceanRift");
	--[[
	for k, v in pairs(args) do
		print(string.format("%s = %s", k, v));
	end
	--]]
	local x = args.x or 0;
	local y = args.y or 0;
	local midline = args.midline or x;
	local direction = args.direction or MG.N;
	local totalSize = args.totalSize or 3;
	local oceanSize = args.oceanSize or MG.oceanRiftWidth;
	local bulge = args.bulge or 0;
	local curve = args.curve or 0;
	local fill = args.fill;
	local cleanMid = args.cleanMid;

	local plots = {};
	if bulge ~= 0 then
		-- See which curve direction fits the land better
		plots = GetRiftPlots(x - 2, midline - 2, y, direction, totalSize, oceanSize, bulge, curve);
		local plotsB = GetRiftPlots(x + 2, midline + 2, y, direction, totalSize, oceanSize, bulge, curve);

		if GetMatchingPlots(plotsB) > GetMatchingPlots(plots) then
			plots = DeepCopy(plotsB);
		end
	elseif curve ~= 0 then
		-- See which curve direction fits the land better
		plots = GetRiftPlots(x - 1, midline - 1, y, direction, totalSize, oceanSize, bulge, curve);
		local plotsB = GetRiftPlots(x + 1, midline + 1, y, direction, totalSize, oceanSize, bulge, -curve);

		if GetMatchingPlots(plotsB) > GetMatchingPlots(plots) then
			plots = DeepCopy(plotsB);
		end
	else
		plots = GetRiftPlots(x, midline, y, direction, totalSize, oceanSize, bulge, curve);
	end

	for plotID, v in pairs(plots) do
		-- print("oceanRiftPlots", v.plot:GetX(), v.plot:GetY(), v.strip);
		if (fill and v.strip <= 0) or IsBetween(0, v.strip, oceanSize) then
			MG.oceanRiftPlots[plotID] = {
				isWater = true,
				terrainID = TerrainTypes.TERRAIN_OCEAN,
			};
		elseif v.strip == -1 or v.strip == oceanSize + 1 then
			if cleanMid and v.strip <= 0 then
				MG.oceanRiftPlots[plotID] = {
					isWater = true,
					terrainID = TerrainTypes.TERRAIN_OCEAN,
				};
			elseif 80 >= Map.Rand(100, "Ocean rift ocean/coast - Lua") then
				MG.oceanRiftPlots[plotID] = {
					isWater = true,
					terrainID = TerrainTypes.TERRAIN_OCEAN,
				};
			else
				MG.oceanRiftPlots[plotID] = MG.oceanRiftPlots[plotID] or {
					isWater = true,
					terrainID = TerrainTypes.TERRAIN_COAST,
				};
			end
		elseif v.strip == -2 or v.strip == oceanSize + 2 then
			if 50 >= Map.Rand(100, "Ocean rift coast/land - Lua") then
				if cleanMid and v.strip <= 0 then
					MG.oceanRiftPlots[plotID] = {
						isWater = true,
						terrainID = TerrainTypes.TERRAIN_OCEAN,
					};
				else
					MG.oceanRiftPlots[plotID] = MG.oceanRiftPlots[plotID] or {
						isWater = true,
						terrainID = TerrainTypes.TERRAIN_COAST,
					};
				end
			else
				MG.oceanRiftPlots[plotID] = MG.oceanRiftPlots[plotID] or {
					isWater = false,
					terrainID = TerrainTypes.TERRAIN_COAST,
				};
			end
		end
	end

	function GetMatchingPlots(tPlots)
		local nicePlots = 0;
		for _, v in pairs(tPlots) do
			if (fill and v.strip <= 0) or IsBetween(0, v.strip, oceanSize) or v.strip == -1 or v.strip == oceanSize + 1 then
				-- Turn plots to water
				if Plot_IsWater(v.plot, true) then
					nicePlots = nicePlots + 1;
				else
					nicePlots = nicePlots - 1;
				end
			elseif v.strip == -2 or v.strip == oceanSize + 2 then
				-- 50% chance turns to water
				if Plot_IsWater(v.plot, true) then
					nicePlots = nicePlots + 1; -- okay if already water
				else
					nicePlots = nicePlots - 0.5; -- destroys land half the time
				end
			else
				-- Encourage land in center area
				if Plot_IsWater(v.plot, true) then
					nicePlots = nicePlots - 0.1;
				else
					nicePlots = nicePlots + 0.1;
				end
			end
		end
		return nicePlots;
	end
end

function GetRiftPlots(x, midline, y, direction, totalSize, oceanSize, bulge, curve)
	local mapW, mapH = Map.GetGridSize();
	local riftPlots = {};

	print(string.format("x = %-3s, y = %-3s Creating %s ocean rift with midline %s curve %s on map size (%s, %s)",
		x, y, MG.directionNames[direction], midline, curve, mapW, mapH));

	local nextDirA = 0;
	local nextDirB = 0;

	if direction == MG.N then
		nextDirA = MG.NE;
		nextDirB = MG.NW;
	elseif direction == MG.S then
		nextDirA = MG.SE;
		nextDirB = MG.SW;
	elseif direction == MG.E then
		nextDirA = MG.E;
		nextDirB = MG.E;
	elseif direction == MG.W then
		nextDirA = MG.W;
		nextDirB = MG.W;
	else
		print("Invalid direction %s for CreateOceanRift");
		return {};
	end

	local plot = Map.GetPlot(x, y);
	local attempts = 0;
	while plot and attempts < mapW do
		local radius = math.max(0, Round((totalSize - 1) / 2 + bulge / 2 * GetBellCurve(y, mapH)));
		local extraRadius = oceanSize;

		if direction == MG.E or direction == MG.W then
			print(string.format("x = %-3s, y = %-3s radius = %-3s extraRadius = %-3s", x, y, radius, extraRadius));
		else
			print(string.format("x = %-3s, y = %-3s radius = %-3s extraRadius = %-3s midX = %-3s", x, y, radius, extraRadius, midline));
		end

		for nearPlot, nearDistance in Plot_GetPlotsInCircle(plot, 0, radius + extraRadius) do
			local nearPlotID = Plot_GetID(nearPlot);
			if not MG.oceanRiftPlots[nearPlotID] then
				if nearPlotID and not riftPlots[nearPlotID] then
					riftPlots[nearPlotID] = {plot = nearPlot, minDistance = 999, strip = 0};
				end
				if nearDistance < riftPlots[nearPlotID].minDistance then
					riftPlots[nearPlotID].minDistance = nearDistance;
					riftPlots[nearPlotID].strip = nearDistance - radius;
				end
			end
		end

		local nextPlotA = Map.PlotDirection(x, y, nextDirA);
		local nextPlotB = Map.PlotDirection(x, y, nextDirB);

		if not nextPlotA or not nextPlotB then
			-- Reached edge of map
			-- print("End ocean rift");
			return riftPlots;
		end

		local oddsA = 0.50;
		if nextPlotA == nextPlotB then
			if 0.5 > PWRand() then
				plot = nextPlotA;
			else
				plot = nextPlotB;
			end
		else
			local distanceA = Map.PlotDistance(nextPlotA:GetX(), y, midline, y);
			local distanceB = Map.PlotDistance(nextPlotB:GetX(), y, midline, y);
			local nextElevationA = MG.elevationRect[nextPlotA:GetX()][nextPlotA:GetY()];
			local nextElevationB = MG.elevationRect[nextPlotB:GetX()][nextPlotB:GetY()];
			-- print("nextElevationA =", nextElevationA, " nextElevationB =", nextElevationB);

			oddsA = nextElevationB / (nextElevationA + nextElevationB); -- Inversely proportional
			oddsA = math.min(0.9, math.max(0.1, oddsA)); -- Neither too big or too low
			oddsA = oddsA * (distanceB / (distanceA + distanceB)) ^ GetBellCurve(y, mapH);

			-- print("oddsA =", oddsA);

			local randomPercent = PWRand();
			-- print(string.format("distance A = %s B = %s oddsA = %.2f rand = %.2f", distanceA, distanceB, Round(oddsA, 2), Round(randomPercent, 2)));
			if oddsA >= randomPercent then
				plot = nextPlotA;
				-- print("choose A");
			else
				plot = nextPlotB;
				-- print("choose B");
			end
			--[[
			print("nextPlotA =", nextPlotA:GetX(), "-", nextPlotA:GetY());
			print("nextPlotB =", nextPlotB:GetX(), "-", nextPlotB:GetY());
			print("plot =", plot:GetX(), "-", plot:GetY());
			--]]
		end

		x = plot:GetX();
		y = plot:GetY();
		attempts = attempts + 1;
	end
	return riftPlots;
end

-- Creates an horizontal passage (by tu_79)
function CreateMagallanes()
	-- Don't do this for Terra maps or if the option is turned off
	if Map.GetCustomOption(5) == 1 or Map.GetCustomOption(8) == 2 then
		return;
	end

	local mapW, mapH = Map.GetGridSize();
	local startY = 0;

	print("CreateMagallanes MG.oceanRiftWidth =", MG.oceanRiftWidth);

	local landInRow = {};
	for y = 0, mapH - 1 do
		landInRow[y] = 0;
	end

	-- Scan rows of plots to get the row with the least land
	local totalLand = mapW - 1;
	for y = Round(mapH / 5), Round(4 * mapH / 5) - 1 do
		for x = 0, mapW - 1 do
			local plot = Map.GetPlot(x, y);
			if not Plot_IsWater(plot, true) then
				landInRow[y] = landInRow[y] + 1;
			end
		end
		if landInRow[y] < totalLand then
			totalLand = landInRow[y];
			startY = y;
		end
	end

	local plots = GetMagallanesPlots(startY);

	for plotID, v in pairs(plots) do
		print("Magallane Plots", v.plot:GetX(), v.plot:GetY(), v.strip);
		if v.strip == 0 then
			MG.oceanRiftPlots[plotID] = {
				isWater = true,
				terrainID = TerrainTypes.TERRAIN_OCEAN,
			};
		elseif v.strip == 1 then
			if 50 >= Map.Rand(100, "Ocean rift ocean/coast - Lua") then
				MG.oceanRiftPlots[plotID] = {
					isWater = true,
					terrainID = TerrainTypes.TERRAIN_COAST,
				};
			else
				MG.oceanRiftPlots[plotID] = MG.oceanRiftPlots[plotID] or {
					isWater = false,
					terrainID = TerrainTypes.TERRAIN_COAST,
				};
			end
		end
	end
end

function GetMagallanesPlots(startY)
	local mapW, mapH = Map.GetGridSize();
	local riftPlots = {};

	print(string.format("Creating magallane rift starting (0, %s) on map size %s * %s", startY, mapW, mapH));

	local nextDirA = MG.NE;
	local nextDirB = MG.E;
	local nextDirC = MG.SE;
	local nextPlotA = {};
	local nextPlotB = {};
	local nextPlotC = {};
	local x = 0;
	local y = startY;

	local plot = Map.GetPlot(x, y);
	local attempts = 0;

	while plot and attempts < mapW + mapH do
		if plot:GetX() == mapW - 1 then
			print("Reached the map end");
			return riftPlots;
		end
		print("X:", plot:GetX(), " Y:", plot:GetY(), "StartY:", startY);

		for nearPlot, nearDistance in Plot_GetPlotsInCircle(plot, 0, 1) do
			local nearPlotID = Plot_GetID(nearPlot);
			if nearPlotID then
				if not riftPlots[nearPlotID] then
					riftPlots[nearPlotID] = {plot = nearPlot, strip = nearDistance};
				end
				if nearDistance == 0 then
					riftPlots[nearPlotID].strip = nearDistance;
				end
			end
		end

		if 0 < plot:GetY() and plot:GetY() < mapH - 1 then
			nextPlotA = Map.PlotDirection(x, y, nextDirA);
			nextPlotB = Map.PlotDirection(x, y, nextDirB);
			nextPlotC = Map.PlotDirection(x, y, nextDirC);
			-- print("nextA:", nextPlotA:GetY(), "nextB:", nextPlotB:GetY(), "nextC:", nextPlotC:GetY());
			if not nextPlotA or not nextPlotB or not nextPlotC then
				-- Reached edge of map
				print("End magallanes rift");
				return riftPlots;
			end
		else
			nextPlotB = Map.PlotDirection(x, y, nextDirB);
			nextPlotA = nextPlotB;
			nextPlotC = nextPlotB;
		end
		-- print("A:", nextPlotA:GetY(), "B:", nextPlotB:GetY(), "C:", nextPlotC:GetY());

		local oddsA = 0;
		local oddsB = 0;
		local oddsC = 0;
		if nextPlotA == nextPlotB and nextPlotA == nextPlotC then
			plot = nextPlotA;
		else
			local distanceA = Map.PlotDistance(x, nextPlotA:GetY(), x, startY);
			local distanceB = Map.PlotDistance(x, nextPlotB:GetY(), x, startY);
			local distanceC = Map.PlotDistance(x, nextPlotC:GetY(), x, startY);
			local totalDistance = distanceA + distanceB + distanceC;
			distanceA = distanceA / totalDistance;
			distanceB = distanceB / totalDistance;
			distanceC = distanceC / totalDistance;

			local nextElevationA = 0;
			local nextElevationB = 0;
			local nextElevationC = 0;

			-- Consider only the next 10% of the map.
			for i = x, math.min(mapW - 1, x + Round(mapW / 10)) do
				local horizontalBand = Round(mapH / 18);
				local topS = math.max(4, y - 3 * horizontalBand);
				local topN = math.min(mapH - 5, y + 3 * horizontalBand);
				local midN = math.min(topN - 1, y + horizontalBand);
				local midS = math.max(topS + 1, y - horizontalBand);

				-- print("topN", topN, "midN", midN, "midS", midS, "topS", topS);

				for j = topS, midS do
					if not Plot_IsWater(Map.GetPlot(i, j), true) then
						nextElevationC = nextElevationC + 1;
					end
				end
				nextElevationC = math.min(nextElevationC, nextElevationC / (midS - topS));
				for j = midS, midN do
					if not Plot_IsWater(Map.GetPlot(i, j), true) then
						nextElevationB = nextElevationB + 1;
					end
				end
				nextElevationB = math.min(nextElevationB, nextElevationB / (midN - midS));
				for j = midN, topN do
					if not Plot_IsWater(Map.GetPlot(i, j), true) then
						nextElevationA = nextElevationA + 1;
					end
				end
				nextElevationA = math.min(nextElevationA, nextElevationA / (topN - midN));
			end

			local totalElevation = nextElevationA + nextElevationB + nextElevationC;
			nextElevationA = nextElevationA / totalElevation;
			nextElevationB = nextElevationB / totalElevation;
			nextElevationC = nextElevationC / totalElevation;

			local looseness = (1 - GetBellCurve(x, mapW - 1)) ^ 2;
			oddsA = (1 - looseness) * nextElevationA + looseness * distanceA;
			oddsB = (1 - looseness) * nextElevationB + looseness * distanceB;
			oddsC = (1 - looseness) * nextElevationC + looseness * distanceC;

			oddsA = math.cos(oddsA * math.pi / 2) / 3;
			oddsB = math.cos(oddsB * math.pi / 2) / 3;
			oddsC = math.cos(oddsC * math.pi / 2) / 3;

			local rand = PWRand();
			if oddsA / (oddsA + oddsB + oddsC) > rand and nextPlotA:GetY() < mapH - 4 then
				plot = nextPlotA;
			elseif oddsC / (oddsA + oddsB + oddsC) > rand and nextPlotC:GetY() > 3 then
				plot = nextPlotC;
			else
				plot = nextPlotB;
			end
		end
		x = plot:GetX();
		y = plot:GetY();
		attempts = attempts + 1;
	end
	print("No more Magallanes attempts");
	return riftPlots;
end

function SetOceanRiftElevations()
	for plotID, data in pairs(MG.oceanRiftPlots) do
		if data.isWater then
			local plot = Map.GetPlotByIndex(plotID);
			elevationMap.data[elevationMap:GetIndex(plot:GetX(), plot:GetY())] = 0;
		end
	end
end

function SetOceanRiftPlots()
	print("SetOceanRiftPlots");
	for plotID, data in pairs(MG.oceanRiftPlots) do
		local plot = Map.GetPlotByIndex(plotID);
		-- print(string.format("oceanRiftPlots plotID = %-4s isWater = %-6s terrainID = %-3s", plotID, tostring(data.isWater), data.terrainID));
		if data.isWater then
			if not plot:IsWater() or data.terrainID == TerrainTypes.TERRAIN_OCEAN then
				plot:SetTerrainType(data.terrainID, false, true);
			end
		end
	end

	for plotID in pairs(MG.oceanRiftPlots) do
		local plot = Map.GetPlotByIndex(plotID);
		if plot:GetTerrainType() == TerrainTypes.TERRAIN_COAST then
			local foundLand = false;
			for nearPlot in Plot_GetPlotsInCircle(plot, 1, 2) do
				if not Plot_IsWater(nearPlot) then
					foundLand = true;
					break;
				end
			end
			if not foundLand then
				plot:SetTerrainType(TerrainTypes.TERRAIN_OCEAN, false, true);
			end
		elseif plot:GetTerrainType() == TerrainTypes.TERRAIN_OCEAN then
			for nearPlot in Plot_GetPlotsInCircle(plot, 1, 1) do
				if not Plot_IsWater(nearPlot) then
					plot:SetTerrainType(TerrainTypes.TERRAIN_COAST, false, true);
					break;
				end
			end
		end
	end
end

-----------------------------------------------------
-- Generate Features
-----------------------------------------------------
function AddFeatures()
	print("Adding Features CommunitasMap");
	Map.RecalculateAreas();
	RestoreLakes();

	local timeStart = debugTime and os.clock() or 0;
	local zeroTreesThreshold = rainfallMap:FindThresholdFromPercent(MG.zeroTreesPercent, false, true);
	local jungleThreshold = rainfallMap:FindThresholdFromPercent(MG.junglePercent, false, true);

	for _, plot in Plots(Shuffle) do
		Plot_AddMainFeatures(plot, zeroTreesThreshold, jungleThreshold);
	end

	local potentialForestPlots = {};
	for plotID, plot in Plots(Shuffle) do
		if not plot:IsWater() then
			PlacePossibleOasis(plot);
			if IsGoodExtraForestTile(plot) then
				table.insert(potentialForestPlots, plot);
			end
		else
			PlacePossibleIce(plot);
			PlacePossibleAtoll(plotID);
		end
	end

	for _, plot in pairs(potentialForestPlots) do
		plot:SetFeatureType(FeatureTypes.FEATURE_FOREST);
	end

	ConnectPolarSeasToOceans();

	-- Remove all rivers bordering lakes or oceans
	for _, plot in Plots() do
		if Plot_IsWater(plot) then
			for edgeDirection = 0, DirectionTypes.NUM_DIRECTION_TYPES - 1 do
				Plot_SetRiver(plot, edgeDirection, MG.flowNONE);
			end
		end
	end

	-- Add flood plains here since rivers were just finalized
	for _, plot in Plots() do
		if plot:CanHaveFeature(FeatureTypes.FEATURE_FLOOD_PLAINS) then
			plot:SetFeatureType(FeatureTypes.FEATURE_FLOOD_PLAINS);
		end
	end

	if debugTime then
		print(string.format("%5s ms, AddFeatures %s", math.floor((os.clock() - timeStart) * 1000), "End"));
	end

	Map.RecalculateAreas();
end

function Plot_AddMainFeatures(plot, zeroTreesThreshold, jungleThreshold)
	local x, y = plot:GetX(), plot:GetY();
	local i = elevationMap:GetIndex(x, y);
	local plotTerrainID = plot:GetTerrainType();

	if plot:IsWater() or plot:IsMountain() then
		return;
	end

	-- Supposed to have flood plains here, skip
	if plot:CanHaveFeature(FeatureTypes.FEATURE_FLOOD_PLAINS) then
		return;
	end

	-- Micro-climates for tiny volcanic islands
	if not plot:IsMountain() and (plotTerrainID == TerrainTypes.TERRAIN_PLAINS or plotTerrainID == TerrainTypes.TERRAIN_GRASS) then
		local landmassSize = Map.GetNumTilesOfLandmass(plot:GetLandmass());
		if landmassSize <= 6 - math.max(1, Map.Rand(5, "Add Island Features - Lua")) then
			local zone = elevationMap:GetZone(y);
			if zone == MG.NEQUATOR or zone == MG.SEQUATOR then
				for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
					if nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
						return;
					end
				end
				plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE);
				return;
			elseif zone == MG.NTEMPERATE or zone == MG.STEMPERATE then
				plot:SetFeatureType(FeatureTypes.FEATURE_FOREST);
				return;
			end
		end
	end

	-- Too dry for jungle
	if rainfallMap.data[i] < jungleThreshold then
		if rainfallMap.data[i] > zeroTreesThreshold and temperatureMap.data[i] > MG.treesMinTemperature then
			if IsGoodFeaturePlot(plot) then
				plot:SetFeatureType(FeatureTypes.FEATURE_FOREST);
			end
		end
		return;
	end

	-- Too cold for jungle
	if temperatureMap.data[i] < MG.jungleMinTemperature then
		if temperatureMap.data[i] > MG.treesMinTemperature and IsGoodFeaturePlot(plot) then
			plot:SetFeatureType(FeatureTypes.FEATURE_FOREST);
		end
		return;
	end

	-- Too near desert for jungle
	for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
		if nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
			return;
		end
	end

	-- This tile is tropical marsh or jungle, or featureless
	table.insert(MG.tropicalPlots, plot);

	-- Check marsh
	if temperatureMap.data[i] > MG.treesMinTemperature and IsGoodFeaturePlot(plot, FeatureTypes.FEATURE_MARSH) then
		plot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
		plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true);
		plot:SetFeatureType(FeatureTypes.FEATURE_MARSH);
		return;
	end

	if IsGoodFeaturePlot(plot) then
		plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE);
		-- Turn some non-hill jungle into plains so they at least have some production - azum4roll
		if not plot:IsHills() and ModifyOdds(MG.featurePercent, 2) >= PWRand() then
			plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true);
		end
	else
		plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true);
	end
end

function IsGoodFeaturePlot(plot, featureID)
	local odds, oddsMultiplier;

	if featureID == FeatureTypes.FEATURE_MARSH then
		if plot:GetPlotType() ~= PlotTypes.PLOT_LAND then
			return false;
		end
		odds = MG.marshPercent;
		oddsMultiplier = 1;
		for nearPlot in Plot_GetPlotsInCircle(plot, 0, 1) do
			local nearTerrainID = nearPlot:GetTerrainType();
			if nearPlot:GetFeatureType() == FeatureTypes.FEATURE_MARSH then
				oddsMultiplier = oddsMultiplier + 8; -- encourage clumping
			elseif nearPlot:GetPlotType() == PlotTypes.PLOT_OCEAN then
				if nearPlot:IsLake() then
					oddsMultiplier = oddsMultiplier + 4;
				elseif plot:IsRiverSide() then
					oddsMultiplier = oddsMultiplier + 2;
				else
					oddsMultiplier = oddsMultiplier + 1;
				end
			elseif nearTerrainID == TerrainTypes.TERRAIN_DESERT or nearTerrainID == TerrainTypes.TERRAIN_SNOW then
				return 0;
			elseif plot:IsRiverSide() and nearPlot:IsFreshWater() then
				oddsMultiplier = oddsMultiplier + 1;
			end
		end
		odds = ModifyOdds(odds, oddsMultiplier);
		return odds >= PWRand();
	end

	-- No features on deserts and snow
	if plot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT or plot:GetTerrainType() == TerrainTypes.TERRAIN_SNOW then
		return false;
	end

	odds = MG.featurePercent;
	oddsMultiplier = 1.0;
	if plot:IsFreshWater() then
		oddsMultiplier = oddsMultiplier + MG.featureWetVariance;
	else
		oddsMultiplier = oddsMultiplier - MG.featureWetVariance;
	end

	-- Increase chance for features when there are other features nearby already.
	for nearPlot in Plot_GetPlotsInCircle(plot, 0, 2) do
		local nearFeatureID = nearPlot:GetFeatureType();
		if nearFeatureID == FeatureTypes.FEATURE_FOREST then
			oddsMultiplier = oddsMultiplier * 1.30;
		elseif nearFeatureID == FeatureTypes.FEATURE_JUNGLE then
			oddsMultiplier = oddsMultiplier * 1.40;
		elseif nearFeatureID ~= FeatureTypes.NO_FEATURE then
			oddsMultiplier = oddsMultiplier * 1.00;
		elseif nearPlot:GetTerrainType() == TerrainTypes.PLOT_HILLS or nearPlot:IsMountain() then
			oddsMultiplier = oddsMultiplier * 0.85;
		end
	end

	-- Decrease chance for features on hilly tundra
	if plot:GetTerrainType() == TerrainTypes.TERRAIN_TUNDRA and plot:GetPlotType() == PlotTypes.PLOT_HILLS then
		oddsMultiplier = oddsMultiplier * 0.50;
	end

	odds = ModifyOdds(odds, oddsMultiplier);
	return odds >= PWRand();
end

function IsBarrenDesert(plot)
	return plot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT and not plot:IsMountain() and plot:GetFeatureType() == FeatureTypes.NO_FEATURE;
end

function PlacePossibleOasis(plot)
	if not plot:CanHaveFeature(FeatureTypes.FEATURE_OASIS) then
		return;
	end

	local odds = 0;
	for nearPlot, distance in Plot_GetPlotsInCircle(plot, 3) do
		distance = distance or 1;
		local featureID = nearPlot:GetFeatureType();

		if featureID == FeatureTypes.FEATURE_OASIS then
			if distance <= 2 then
				-- at least 2 tile spacing between oases
				return;
			end
			odds = odds - 200 / distance;
		end

		if featureID == FeatureTypes.NO_FEATURE and not nearPlot:IsFreshWater() then
			if nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
				odds = odds + 10 / distance;
			elseif Plot_IsMountain(nearPlot) or nearPlot:IsHills() then
				odds = odds + 5 / distance;
			elseif nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_PLAINS then
				odds = odds + 2 / distance;
			end
		else
			odds = odds - 5 / distance;
		end
	end

	if odds >= Map.Rand(100, "PlacePossibleOasis - Lua") then
		plot:SetFeatureType(FeatureTypes.FEATURE_OASIS);
	end
end

function IsGoodExtraForestTile(plot)
	local odds = MG.forestRandomPercent;
	local terrainID = plot:GetTerrainType();
	local resID = plot:GetResourceType();
	if not plot:CanHaveFeature(FeatureTypes.FEATURE_FOREST) then
		return false;
	end

	local oddsMultiplier = 1.0;
	if terrainID == TerrainTypes.TERRAIN_TUNDRA then
		if resID ~= -1 then
			return true;
		end
		if plot:IsFreshWater() then
			oddsMultiplier = oddsMultiplier + MG.featureWetVariance;
		end
	end

	-- Avoid filling flat holes of tropical areas, which are too dense already
	if not plot:IsHills() and Contains(MG.tropicalPlots, plot) then
		oddsMultiplier = oddsMultiplier - 0.45;
	end

	for nearPlot in Plot_GetPlotsInCircle(plot, 1, 1) do
		local nearTerrainID = nearPlot:GetTerrainType();

		if nearPlot:IsMountain() then
			-- do nothing
		elseif nearPlot:IsHills() then
			-- Region already has enough production and rough terrain
			oddsMultiplier = oddsMultiplier * 0.95;
		elseif nearTerrainID == TerrainTypes.TERRAIN_SNOW then
			-- Help extreme polar regions
			oddsMultiplier = oddsMultiplier * 1.25;
		elseif nearTerrainID == TerrainTypes.TERRAIN_TUNDRA then
			oddsMultiplier = oddsMultiplier * 1.10;
		elseif terrainID == TerrainTypes.TERRAIN_TUNDRA and Plot_IsWater(nearPlot) then
			oddsMultiplier = oddsMultiplier * 1.05;
		end

		-- Avoid tropics
		if Contains(MG.tropicalPlots, nearPlot) then
			oddsMultiplier = oddsMultiplier * 0.90;
		end

		-- Too dry
		if nearTerrainID == TerrainTypes.TERRAIN_DESERT then
			oddsMultiplier = oddsMultiplier * 0.80;
		end
	end

	odds = ModifyOdds(odds, oddsMultiplier);

	if 100 * MG.featurePercent * odds >= Map.Rand(100, "Add Extra Forest - Lua") then
		return true;
	end

	return false;
end

function PlacePossibleIce(plot)
	local _, mapH = Map.GetGridSize();
	local y = plot:GetY();
	if not plot:IsWater() then
		return;
	end

	local latitude = math.abs(temperatureMap:GetLatitudeForY(y));
	local lowestIce = MG.iceLatitude;
	local odds = 100;
	local oddsMultiplier = 1.0;

	if 1 < y and y < mapH - 2 then
		if latitude >= lowestIce then
			odds = math.pow((latitude - lowestIce) / (MG.topLatitude - lowestIce), 0.5);
		else
			return;
		end
	end

	-- Unblock islands surrounded by ice.
	-- Avoid ice between land tiles and equator, increase ice between land tiles and poles
	for nearPlot in Plot_GetPlotsInCircle(plot, 2) do
		if not nearPlot:IsWater() then
			if y < mapH / 2 then -- South hemisphere
				if nearPlot:GetY() < y then
					return;
				else
					oddsMultiplier = oddsMultiplier + 0.2;
				end
			else -- North hemisphere
				if nearPlot:GetY() > y then
					return;
				else
					oddsMultiplier = oddsMultiplier + 0.2;
				end
			end
		end
	end

	odds = ModifyOdds(odds, oddsMultiplier);

	if odds >= PWRand() then
		plot:SetFeatureType(FeatureTypes.FEATURE_ICE);

		-- No grass beside ice
		for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
			if nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_GRASS then
				if nearPlot:GetFeatureType() == FeatureTypes.FEATURE_MARSH then
					nearPlot:SetFeatureType(FeatureTypes.NO_FEATURE);
				end
				nearPlot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true);
			end
		end
	end
end

function PlacePossibleAtoll(i)
	local W, _ = Map.GetGridSize();
	local plot = Map.GetPlotByIndex(i);
	local x = i % W;
	local y = (i - x) / W;
	if plot:GetTerrainType() == TerrainTypes.TERRAIN_COAST then
		local latitude = math.abs(temperatureMap:GetLatitudeForY(y));
		if latitude < MG.atollNorthLatitudeLimit and latitude > MG.atollSouthLatitudeLimit then
			local deepCount = 0;
			local totalFeatures = 0;
			local thereIsLand = false;
			for nearPlot in Plot_GetPlotsInCircle(plot, 1, 3) do
				if nearPlot:GetPlotType() == PlotTypes.PLOT_LAND then
					thereIsLand = true;
				end
			end
			if not thereIsLand then
				return;
			end
			for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
				if nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_OCEAN then
					deepCount = deepCount + 1;
				end
				if nearPlot:GetFeatureType() ~= FeatureTypes.NO_FEATURE then
					totalFeatures = totalFeatures + 1;
				end
			end
			if deepCount >= MG.atollMinDeepWaterNeighbors then
				if totalFeatures < Map.Rand(3, "Place Possible Atoll - Lua") then
					plot:SetFeatureType(FeatureTypes.FEATURE_ATOLL);
				end
			end
		end
	end
end

function AddRivers()
	local mapW, mapH = Map.GetGridSize();

	for y = 0, mapH - 1 do
		for x = 0, mapW - 1 do
			local plot = Map.GetPlot(x, y);

			local WOfRiver, NWOfRiver, NEOfRiver = riverMap:GetFlowDirections(x, y);

			if WOfRiver == MG.flowNONE then
				plot:SetWOfRiver(false, WOfRiver);
			else
				local xx, yy = elevationMap:GetNeighbor(x, y, MG.E);
				local nPlot = Map.GetPlot(xx, yy);
				if plot:IsMountain() and nPlot:IsMountain() then
					plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
				end
				plot:SetWOfRiver(true, WOfRiver);
			end

			if NWOfRiver == MG.flowNONE then
				plot:SetNWOfRiver(false, NWOfRiver);
			else
				local xx, yy = elevationMap:GetNeighbor(x, y, MG.SE);
				local nPlot = Map.GetPlot(xx, yy);
				if plot:IsMountain() and nPlot:IsMountain() then
					plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
				end
				plot:SetNWOfRiver(true, NWOfRiver);
			end

			if NEOfRiver == MG.flowNONE then
				plot:SetNEOfRiver(false, NEOfRiver);
			else
				local xx, yy = elevationMap:GetNeighbor(x, y, MG.SW);
				local nPlot = Map.GetPlot(xx, yy);
				if plot:IsMountain() and nPlot:IsMountain() then
					plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
				end
				plot:SetNEOfRiver(true, NEOfRiver);
			end
		end
	end
end

-----------------------------------------------------
-- Other Generators
-----------------------------------------------------
DiffMap = InheritsFrom(FloatMap);

function GenerateDiffMap(width, height, xWrap, yWrap)
	DiffMap = FloatMap:New(width, height, xWrap, yWrap)
	local i = 0
	for y = 0, height - 1 do
		for x = 0, width - 1 do
			if elevationMap:IsBelowSeaLevel(x, y) then
				DiffMap.data[i] = 0.0;
			else
				DiffMap.data[i] = GetDifferenceAroundHex(x, y);
			end
			i = i + 1;
		end
	end

	DiffMap:Normalize();
	i = 0;
	for y = 0, height - 1 do
		for x = 0, width - 1 do
			if elevationMap:IsBelowSeaLevel(x, y) then
				DiffMap.data[i] = 0.0;
			else
				DiffMap.data[i] = DiffMap.data[i] + elevationMap.data[i] * 1.1;
			end
			i = i + 1;
		end
	end

	DiffMap:Normalize();
	return DiffMap;
end

function GenerateTwistedPerlinMap(width, height, xWrap, yWrap, minFreq, maxFreq, varFreq)
	local inputNoise = FloatMap:New(width, height, xWrap, yWrap);
	inputNoise:GenerateNoise();
	inputNoise:Normalize();

	local freqMap = FloatMap:New(width, height, xWrap, yWrap);
	for y = 0, freqMap.height - 1 do
		for x = 0, freqMap.width - 1 do
			local i = freqMap:GetIndex(x, y);
			local odd = y % 2;
			local xx = x + odd * 0.5;
			freqMap.data[i] = GetPerlinNoise(xx, y * MG.YtoXRatio, freqMap.width, freqMap.height * MG.YtoXRatio, varFreq, 1.0, 0.1, 8, inputNoise);
		end
	end
	freqMap:Normalize();

	local twistMap = FloatMap:New(width, height, xWrap, yWrap);
	for y = 0, twistMap.height - 1 do
		for x = 0, twistMap.width - 1 do
			local i = twistMap:GetIndex(x, y);
			local freq = freqMap.data[i] * (maxFreq - minFreq) + minFreq;
			local mid = (maxFreq - minFreq) / 2 + minFreq;
			local coordScale = freq / mid;
			local offset = (1.0 - coordScale) / mid;
			-- print("1 - coordscale = " .. (1.0 - coordScale) .. ", offset = " .. offset);
			local ampChange = 0.85 - freqMap.data[i] * 0.5;
			local odd = y % 2;
			local xx = x + odd * 0.5;
			twistMap.data[i] = GetPerlinNoise(xx + offset, (y + offset) * MG.YtoXRatio, twistMap.width, twistMap.height * MG.YtoXRatio, mid, 1.0, ampChange, 8, inputNoise);
		end
	end

	twistMap:Normalize();
	return twistMap;
end

function GenerateMountainMap(width, height, xWrap, yWrap, initFreq)
	local timeStart = debugTime and os.clock() or 0;

	local inputNoise = FloatMap:New(width, height, xWrap, yWrap);
	inputNoise:GenerateBinaryNoise();
	inputNoise:Normalize();
	local inputNoise2 = FloatMap:New(width, height, xWrap, yWrap);
	inputNoise2:GenerateNoise();
	inputNoise2:Normalize();

	local mountainMap = FloatMap:New(width, height, xWrap, yWrap);
	local stdDevMap = FloatMap:New(width, height, xWrap, yWrap);
	local noiseMap = FloatMap:New(width, height, xWrap, yWrap);

	if debugTime then
		print(string.format("%5s ms, GenerateMountainMap %s", math.floor((os.clock() - timeStart) * 1000), "Start"));
		timeStart = os.clock();
	end

	for y = 0, mountainMap.height - 1 do
		for x = 0, mountainMap.width - 1 do
			local i = mountainMap:GetIndex(x, y);
			local odd = y % 2;
			local xx = x + odd * 0.5;
			mountainMap.data[i] = GetPerlinNoise(xx, y * MG.YtoXRatio, mountainMap.width, mountainMap.height * MG.YtoXRatio, initFreq, 1.0, 0.4, 8, inputNoise);
			noiseMap.data[i] = GetPerlinNoise(xx, y * MG.YtoXRatio, mountainMap.width, mountainMap.height * MG.YtoXRatio, initFreq, 1.0, 0.4, 8, inputNoise2);
			stdDevMap.data[i] = mountainMap.data[i];
		end
	end

	if debugTime then
		print(string.format("%5s ms, GenerateMountainMap %s", math.floor((os.clock() - timeStart) * 1000), "A"));
		timeStart = os.clock();
	end

	mountainMap:Normalize();

	if debugTime then
		print(string.format("%5s ms, GenerateMountainMap %s", math.floor((os.clock() - timeStart) * 1000), "B"))
		timeStart = os.clock();
	end

	stdDevMap:Deviate(MG.elevationBlendRange);

	if debugTime then
		print(string.format("%5s ms, GenerateMountainMap %s", math.floor((os.clock() - timeStart) * 1000), "C"));
		timeStart = os.clock();
	end

	stdDevMap:Normalize();
	noiseMap:Normalize();

	if debugTime then
		print(string.format("%5s ms, GenerateMountainMap %s", math.floor((os.clock() - timeStart) * 1000), "D"));
		timeStart = os.clock();
	end

	local moundMap = FloatMap:New(width, height, xWrap, yWrap);
	for y = 0, mountainMap.height - 1 do
		for x = 0, mountainMap.width - 1 do
			local i = mountainMap:GetIndex(x, y);
			local val = mountainMap.data[i];
			moundMap.data[i] = (math.sin(val * math.pi * 2 - math.pi * 0.5) * 0.5 + 0.5) * GetAttenuationFactor(mountainMap, x, y);
			if val < 0.5 then
				val = val * 4;
			else
				val = (1 - val) * 4;
			end
			mountainMap.data[i] = moundMap.data[i];
		end
	end
	mountainMap:Normalize();

	for y = 0, mountainMap.height - 1 do
		for x = 0, mountainMap.width - 1 do
			local i = mountainMap:GetIndex(x, y);
			local val = mountainMap.data[i];
			mountainMap.data[i] = (math.sin(val * 3 * math.pi + math.pi * 0.5) ^ 16 * val) ^ 0.5;
			if mountainMap.data[i] > 0.2 then
				mountainMap.data[i] = 1.0;
			else
				mountainMap.data[i] = 0.0;
			end
		end
	end

	local stdDevThreshold = stdDevMap:FindThresholdFromPercent(MG.landPercent + 0.05, true, false);
	print("stdDevThreshold =", stdDevThreshold);

	for y = 0, mountainMap.height - 1 do
		for x = 0, mountainMap.width - 1 do
			local i = mountainMap:GetIndex(x, y);
			local val = mountainMap.data[i];
			local dev = 2.0 * stdDevMap.data[i] - 2.0 * stdDevThreshold;
			mountainMap.data[i] = (val + moundMap.data[i]) * dev;
		end
	end

	mountainMap:Normalize();

	if debugTime then
		print(string.format("%5s ms, GenerateMountainMap %s", math.floor((os.clock() - timeStart) * 1000), "End"));
	end

	return mountainMap;
end

function GetAttenuationFactor(map, x, y)
	local southY = map.height * MG.southAttenuationRange;
	local southRange = map.height * MG.southAttenuationRange;
	local yAttenuation = 1.0;
	if y < southY then
		yAttenuation = MG.southAttenuationFactor + (y / southRange) * (1.0 - MG.southAttenuationFactor);
	end

	local northY = map.height - (map.height * MG.northAttenuationRange);
	local northRange = map.height * MG.northAttenuationRange;
	if y > northY then
		yAttenuation = MG.northAttenuationFactor + ((map.height - y) / northRange) * (1.0 - MG.northAttenuationFactor);
	end

	local eastY = map.width - (map.width * MG.eastAttenuationRange);
	local eastRange = map.width * MG.eastAttenuationRange;
	local xAttenuation = 1.0;
	if x > eastY then
		xAttenuation = MG.eastAttenuationFactor + ((map.width - x) / eastRange) * (1.0 - MG.eastAttenuationFactor);
	end

	local westY = map.width * MG.westAttenuationRange;
	local westRange = map.width * MG.westAttenuationRange;
	if x < westY then
		xAttenuation = MG.westAttenuationFactor + (x / westRange) * (1.0 - MG.westAttenuationFactor);
	end

	return yAttenuation * xAttenuation;
end

function GenerateElevationMap(width, height, xWrap, yWrap)
	local timeStart = debugTime and os.clock() or 0;
	local landMinScatter = (128 / width) * MG.landMinScatter;
	local landMaxScatter = (128 / width) * MG.landMaxScatter;
	local coastScatter = (128 / width) * MG.coastScatter;
	local mountainScatter = (128 / width) * MG.mountainScatter;
	local twistMap = GenerateTwistedPerlinMap(width, height, xWrap, yWrap, landMinScatter, landMaxScatter, coastScatter);

	if debugTime then
		timeStart = os.clock();
	end

	local mountainMap = GenerateMountainMap(width, height, xWrap, yWrap, mountainScatter);

	if debugTime then
		print(string.format("%5s ms, GenerateElevationMap %s", math.floor((os.clock() - timeStart) * 1000), "GenerateMountainMap"));
		timeStart = os.clock();
	end

	local eMap = ElevationMap:New(width, height, xWrap, yWrap);
	for y = 0, height - 1 do
		for x = 0, width - 1 do
			local i = eMap:GetIndex(x, y);
			local tVal = twistMap.data[i];
			eMap.data[i] = tVal + (mountainMap.data[i] * 2 - 1) * MG.mountainWeight;
		end
	end

	eMap:Normalize();

	-- attentuation should not break normalization
	for y = 0, height - 1 do
		for x = 0, width - 1 do
			local i = eMap:GetIndex(x, y);
			local attenuationFactor = GetAttenuationFactor(eMap, x, y);
			eMap.data[i] = eMap.data[i] * attenuationFactor;
		end
	end

	eMap.seaLevelThreshold = eMap:FindThresholdFromPercent(MG.landPercent + 0.05, true, false);
	-- print("seaLevelThreshold", eMap.seaLevelThreshold);

	if debugTime then
		print(string.format("%5s ms, GenerateElevationMap %s", math.floor((os.clock() - timeStart) * 1000), "End"));
	end

	return eMap;
end

function GenerateTempMaps()
	local timeStart = debugTime and os.clock() or 0;
	local aboveSeaLevelMap = FloatMap:New(elevationMap.width, elevationMap.height, elevationMap.xWrap, elevationMap.yWrap);
	for y = 0, elevationMap.height - 1 do
		for x = 0, elevationMap.width - 1 do
			local i = aboveSeaLevelMap:GetIndex(x, y);
			if elevationMap:IsBelowSeaLevel(x, y) then
				aboveSeaLevelMap.data[i] = 0.0;
			else
				aboveSeaLevelMap.data[i] = elevationMap.data[i] - elevationMap.seaLevelThreshold;
			end
		end
	end
	aboveSeaLevelMap:Normalize();

	local summerMap = FloatMap:New(elevationMap.width, elevationMap.height, elevationMap.xWrap, elevationMap.yWrap);
	local zenith = MG.tropicLatitudes;
	local topTempLat = MG.topLatitude + zenith;
	local bottomTempLat = MG.bottomLatitude;
	local latRange = topTempLat - bottomTempLat;
	for y = 0, elevationMap.height - 1 do
		for x = 0, elevationMap.width - 1 do
			local i = summerMap:GetIndex(x, y);
			local lat = summerMap:GetLatitudeForY(y);
			-- print("y = " .. y .. ", lat = " .. lat);
			local latPercent = (lat - bottomTempLat) / latRange;
			-- print("latPercent = " .. latPercent);
			local temp = (math.sin(latPercent * math.pi * 2 - math.pi * 0.5) * 0.5 + 0.5);
			if elevationMap:IsBelowSeaLevel(x, y) then
				temp = temp * MG.maxWaterTemp + MG.minWaterTemp;
			end
			summerMap.data[i] = temp;
		end
	end

	if debugTime then
		timeStart = os.clock();
	end

	summerMap:Smooth(math.min(MG.tempBlendMaxRange, math.floor(elevationMap.width / 8)));

	if debugTime then
		print(string.format("%5s ms, GenerateTempMaps %s", math.floor((os.clock() - timeStart) * 1000), "Smooth"));
		timeStart = os.clock();
	end

	summerMap:Normalize();

	local winterMap = FloatMap:New(elevationMap.width, elevationMap.height, elevationMap.xWrap, elevationMap.yWrap);
	zenith = -MG.tropicLatitudes;
	topTempLat = MG.topLatitude;
	bottomTempLat = MG.bottomLatitude + zenith;
	latRange = topTempLat - bottomTempLat;
	for y = 0, elevationMap.height - 1 do
		for x = 0, elevationMap.width - 1 do
			local i = winterMap:GetIndex(x, y);
			local lat = winterMap:GetLatitudeForY(y);
			local latPercent = (lat - bottomTempLat) / latRange;
			local temp = math.sin(latPercent * math.pi * 2 - math.pi * 0.5) * 0.5 + 0.5;
			if elevationMap:IsBelowSeaLevel(x, y) then
				temp = temp * MG.maxWaterTemp + MG.minWaterTemp;
			end
			winterMap.data[i] = temp;
		end
	end
	winterMap:Smooth(math.min(MG.tempBlendMaxRange, math.floor(elevationMap.width / 8)));
	winterMap:Normalize();

	local tMap = FloatMap:New(elevationMap.width, elevationMap.height, elevationMap.xWrap, elevationMap.yWrap);
	for y = 0, elevationMap.height - 1 do
		for x = 0, elevationMap.width - 1 do
			local i = tMap:GetIndex(x, y);
			tMap.data[i] = (winterMap.data[i] + summerMap.data[i]) * (1.0 - aboveSeaLevelMap.data[i]);
		end
	end
	tMap:Normalize();

	return summerMap, winterMap, tMap;
end

function GenerateRainfallMap(summerMap, winterMap)
	local geoMap = FloatMap:New(elevationMap.width, elevationMap.height, elevationMap.xWrap, elevationMap.yWrap);
	for y = 0, elevationMap.height - 1 do
		for x = 0, elevationMap.width - 1 do
			local i = elevationMap:GetIndex(x, y);
			local lat = elevationMap:GetLatitudeForY(y);
			local pressure = elevationMap:GetGeostrophicPressure(lat);
			geoMap.data[i] = pressure;
		end
	end
	geoMap:Normalize();

	local sortedSummerMap = {};
	local sortedWinterMap = {};
	for y = 0, elevationMap.height - 1 do
		for x = 0, elevationMap.width - 1 do
			local i = elevationMap:GetIndex(x, y);
			sortedSummerMap[i + 1] = {x, y, summerMap.data[i]};
			sortedWinterMap[i + 1] = {x, y, winterMap.data[i]};
		end
	end
	table.sort(sortedSummerMap, function (a, b) return a[3] < b[3] end);
	table.sort(sortedWinterMap, function (a, b) return a[3] < b[3] end);

	local sortedGeoMap = {};
	local xStart = 0;
	local xStop = 0;
	local yStart = 0;
	local yStop = 0;
	local incX = 0;
	local incY = 0;
	local geoIndex = 1;
	for zone = 0, 5 do
		local topY = elevationMap:GetYFromZone(zone, true);
		local bottomY = elevationMap:GetYFromZone(zone, false);
		if not (topY == -1 and bottomY == -1) then
			if topY == -1 then
				topY = elevationMap.height - 1;
			end
			if bottomY == -1 then
				bottomY = 0;
			end
			local dir1, dir2 = elevationMap:GetGeostrophicWindDirections(zone);
			if dir1 == MG.SW or dir1 == MG.SE then
				yStart = topY;
				yStop = bottomY;
				incY = -1;
			else
				yStart = bottomY;
				yStop = topY;
				incY = 1;
			end
			if dir2 == MG.W then
				xStart = elevationMap.width - 1;
				xStop = 0;
				incX = -1;
			else
				xStart = 0;
				xStop = elevationMap.width;
				incX = 1;
			end

			for y = yStart, yStop, incY do
				-- Each line should start on water to avoid vast areas without rain
				local xxStart = xStart;
				local xxStop = xStop;
				for xx = xStart, xStop - incX, incX do
					if elevationMap:IsBelowSeaLevel(xx, y) then
						xxStart = xx;
						xxStop = xx + elevationMap.width * incX;
						break;
					end
				end
				for x = xxStart, xxStop - incX, incX do
					local i = elevationMap:GetIndex(x, y);
					sortedGeoMap[geoIndex] = {x, y, geoMap.data[i]};
					geoIndex = geoIndex + 1;
				end
			end
		end
	end

	local rainfallSummerMap = FloatMap:New(elevationMap.width, elevationMap.height, elevationMap.xWrap, elevationMap.yWrap);
	local moistureMap = FloatMap:New(elevationMap.width, elevationMap.height, elevationMap.xWrap, elevationMap.yWrap);
	for i = 1, #sortedSummerMap do
		local x = sortedSummerMap[i][1];
		local y = sortedSummerMap[i][2];
		DistributeRain(x, y, summerMap, rainfallSummerMap, moistureMap, false);
	end

	local rainfallWinterMap = FloatMap:New(elevationMap.width, elevationMap.height, elevationMap.xWrap, elevationMap.yWrap);
	moistureMap = FloatMap:New(elevationMap.width, elevationMap.height, elevationMap.xWrap, elevationMap.yWrap);
	for i = 1, #sortedWinterMap do
		local x = sortedWinterMap[i][1];
		local y = sortedWinterMap[i][2];
		DistributeRain(x, y, winterMap, rainfallWinterMap, moistureMap, false);
	end

	local rainfallGeostrophicMap = FloatMap:New(elevationMap.width, elevationMap.height, elevationMap.xWrap, elevationMap.yWrap);
	moistureMap = FloatMap:New(elevationMap.width, elevationMap.height, elevationMap.xWrap, elevationMap.yWrap);

	--[[
	print("----------------------------------------------------------------------------------------");
	print("--GEOSTROPHIC---------------------------------------------------------------------------");
	print("----------------------------------------------------------------------------------------");
	--]]

	for i = 1, #sortedGeoMap do
		local x = sortedGeoMap[i][1];
		local y = sortedGeoMap[i][2];
		DistributeRain(x, y, geoMap, rainfallGeostrophicMap, moistureMap, true);
	end

	-- Zero below sea level for proper percent threshold finding
	for y = 0, elevationMap.height - 1 do
		for x = 0, elevationMap.width - 1 do
			local i = elevationMap:GetIndex(x, y);
			if elevationMap:IsBelowSeaLevel(x, y) then
				rainfallSummerMap.data[i] = 0.0;
				rainfallWinterMap.data[i] = 0.0;
				rainfallGeostrophicMap.data[i] = 0.0;
			end
		end
	end

	rainfallSummerMap:Normalize();
	rainfallWinterMap:Normalize();
	rainfallGeostrophicMap:Normalize();

	local rMap = FloatMap:New(elevationMap.width, elevationMap.height, elevationMap.xWrap, elevationMap.yWrap);
	for y = 0, elevationMap.height - 1 do
		for x = 0, elevationMap.width - 1 do
			local i = elevationMap:GetIndex(x, y);
			rMap.data[i] = rainfallSummerMap.data[i] + rainfallWinterMap.data[i] + rainfallGeostrophicMap.data[i] * MG.geostrophicFactor;
		end
	end
	rMap:Normalize();

	return rMap;
end

function DistributeRain(x, y, pressureMap, rMap, moistureMap, boolGeostrophic)
	local i = elevationMap:GetIndex(x, y);
	local upLiftSource = math.max(math.pow(pressureMap.data[i], MG.upLiftExponent), 1.0 - temperatureMap.data[i]);
	if elevationMap:IsBelowSeaLevel(x, y) then
		moistureMap.data[i] = math.max(moistureMap.data[i], temperatureMap.data[i]);
	end

	-- Make list of neighbors
	local nList = {};
	if boolGeostrophic then
		local zone = elevationMap:GetZone(y);
		local dir1, dir2 = elevationMap:GetGeostrophicWindDirections(zone);
		local x1, y1 = elevationMap:GetNeighbor(x, y, dir1);
		local ii = elevationMap:GetIndex(x1, y1);
		-- Neighbor must be on map and in same wind zone
		if ii >= 0 and elevationMap:GetZone(y1) == elevationMap:GetZone(y) then
			table.insert(nList, {x1, y1});
		end
		local x2, y2 = elevationMap:GetNeighbor(x, y, dir2);
		ii = elevationMap:GetIndex(x2, y2);
		if ii >= 0 then
			table.insert(nList, {x2, y2});
		end
	else
		for dir = 0, 5 do
			local xx, yy = elevationMap:GetNeighbor(x, y, dir);
			local ii = elevationMap:GetIndex(xx, yy);
			if ii >= 0 and pressureMap.data[i] <= pressureMap.data[ii] then
				table.insert(nList, {xx, yy});
			end
		end
	end
	if #nList == 0 or boolGeostrophic and #nList == 1 then
		local cost = moistureMap.data[i];
		rMap.data[i] = cost;
		return;
	end
	local moisturePerNeighbor = moistureMap.data[i] / #nList;

	-- Drop rain and pass moisture to neighbors
	for n = 1, #nList do
		local xx = nList[n][1];
		local yy = nList[n][2];
		local ii = elevationMap:GetIndex(xx, yy);
		local upLiftDest = math.max(math.pow(pressureMap.data[ii], MG.upLiftExponent), 1.0 - temperatureMap.data[ii]);
		local cost = GetRainCost(upLiftSource, upLiftDest);
		local bonus = 0.0;
		if elevationMap:GetZone(y) == MG.NPOLAR or elevationMap:GetZone(y) == MG.SPOLAR then
			bonus = MG.polarRainBoost;
		end
		if boolGeostrophic and #nList == 2 then
			if n == 1 then
				moisturePerNeighbor = (1.0 - MG.geostrophicLateralWindStrength) * moistureMap.data[i];
			else
				moisturePerNeighbor = MG.geostrophicLateralWindStrength * moistureMap.data[i];
			end
		end
		rMap.data[i] = rMap.data[i] + cost * moisturePerNeighbor + bonus;
		-- Pass to neighbor.
		moistureMap.data[ii] = moistureMap.data[ii] + moisturePerNeighbor - cost * moisturePerNeighbor;
	end
end

function GetRainCost(upLiftSource, upLiftDest)
	local cost = MG.minimumRainCost;
	cost = math.max(MG.minimumRainCost, cost + upLiftDest - upLiftSource);
	if cost < 0.0 then
		cost = 0.0;
	end
	return cost;
end

function GetDifferenceAroundHex(x, y)
	local avg = elevationMap:GetAverageInHex(x, y, 1);
 	local i = elevationMap:GetIndex(x, y);
	return elevationMap.data[i] - avg;
end

---------------------
-- Plot functions
---------------------
function Plot_GetID(plot)
	if not plot then
		print("plot:GetID plot = nil");
		return nil;
	end
	local iW, _ = Map.GetGridSize();
	return plot:GetY() * iW + plot:GetX();
end

function Plot_GetPlotsInCircle(plot, minR, maxR)
	if not plot then
		print("plot:GetPlotsInCircle plot = nil");
		return;
	end
	if not maxR then
		maxR = minR;
		minR = 1;
	end

	local mapW, mapH = Map.GetGridSize();
	local isWrapX = Map:IsWrapX();
	local isWrapY = Map:IsWrapY();
	local centerX = plot:GetX();
	local centerY = plot:GetY();

	local leftX = isWrapX and (centerX - maxR) % mapW or Constrain(0, centerX - maxR, mapW - 1);
	local rightX = isWrapX and (centerX + maxR) % mapW or Constrain(0, centerX + maxR, mapW - 1);
	local bottomY = isWrapY and (centerY - maxR) % mapH or Constrain(0, centerY - maxR, mapH - 1);
	local topY = isWrapY and (centerY + maxR) % mapH or Constrain(0, centerY + maxR, mapH - 1);

	local nearX = leftX;
	local nearY = bottomY;
	local stepX = 0;
	local stepY = 0;
	local rectW = rightX - leftX;
	local rectH = topY - bottomY;

	if rectW < 0 then
		rectW = rectW + mapW;
	end

	if rectH < 0 then
		rectH = rectH + mapH;
	end

	local nextPlot = Map.GetPlot(nearX, nearY);

	return function ()
		while stepY < 1 + rectH and nextPlot do
			while stepX < 1 + rectW and nextPlot do
				local thisPlot = nextPlot;
				local distance = Map.PlotDistance(nearX, nearY, centerX, centerY);

				nearX = (nearX + 1) % mapW;
				stepX = stepX + 1;
				nextPlot = Map.GetPlot(nearX, nearY);

				if IsBetween(minR, distance, maxR) then
					return thisPlot, distance;
				end
			end
			nearX = leftX;
			nearY = (nearY + 1) % mapH;
			stepX = 0;
			stepY = stepY + 1;
			nextPlot = Map.GetPlot(nearX, nearY);
		end
	end
end

function Plot_GetPlotsInCircleFast(x, y, radius)
	-- Assumes X wrap
	local plotIDs = {};
	local W, H = Map.GetGridSize();
	local odd = y % 2;
	local topY = radius;
	local bottomY = radius;
	local currentY;
	local len = 1 + radius;
	local i = (y % H) * W + (x % W);

	-- Constrain the top of our circle to be on the map
	if y + radius > H - 1 then
		for r = 0, radius do
			if y + r == H - 1 then
				topY = r;
				break;
			end
		end
	end

	-- Constrain the bottom of our circle to be on the map
	if y - radius < 0 then
		for r = 0, radius do
			if y - r == 0 then
				bottomY = r;
				break;
			end
		end
	end

	-- Adjust starting length, apply the top and bottom limits, and correct odd for the starting point
	len = len + (radius - bottomY);
	currentY = y - bottomY;
	topY = y + topY;
	odd = (odd + bottomY) % 2;

	-- Set starting point
	i = i - W * bottomY - (radius - bottomY) - math.floor((bottomY + odd) / 2);
	x = x - (radius - bottomY) - math.floor((bottomY + odd) / 2);
	if x < 0 then
		i = i + W;
		x = x + W;
		-- print(string.format("i for (%d,%d) WOULD have been in outer space. x is (%d,%d) i is (%d)", x, y, x, y - bottomY, i));
	end

	-- Cycle through the plot indexes and add them to a table
	-- local str = "";
	while currentY <= topY do
		-- Insert the start value, scan left to right adding each index in the line to our table
		-- str = str .. i .. ", ";
		table.insert(plotIDs, i);
		local wrapped = false;
		for _ = 1, len - 1 do
			if x ~= W - 1 then
				i = i + 1;
				x = x + 1;
			else
				i = i - (W - 1);
				x = 0;
				wrapped = true;
			end
			-- str = str .. i .. ", ";
			table.insert(plotIDs, i);
		end
		if currentY < y then
			-- Move i NW and increment the length to scan
			i = i + W - len + odd;
			x = x - len + odd;
			if wrapped then
				i = i + W;
				x = x + W;
			end
			len = len + 1;
		else
			-- Move i NE and decrement the length to scan
			i = i + W - len + 1 + odd;
			x = x - len + 1 + odd;
			if wrapped then
				i = i + W;
				x = x + W;
			end
			len = len - 1;
		end
		currentY = currentY + 1;
		odd = (odd + 1) % 2;
	end
	-- print(string.format("added " .. str .. "to table for circle starting at (%d,%d)", x, y));
	return plotIDs;
end

function Plot_GetCirclePercents(plot, minR, maxR)
	local plotTypeName = {};
	local terrainTypeName = {};
	local featureTypeName = {};

	for k, v in pairs(PlotTypes) do
		plotTypeName[v] = k;
	end

	for itemInfo in GameInfo.Terrains() do
		terrainTypeName[itemInfo.ID] = itemInfo.Type;
	end

	for itemInfo in GameInfo.Features() do
		featureTypeName[itemInfo.ID] = itemInfo.Type;
	end

	local weights = {TOTAL = 0, SEA = 0, NO_PLOT = 0, NO_TERRAIN = 0, NO_FEATURE = 0};

	for k in pairs(PlotTypes) do
		weights[k] = 0;
	end
	for itemInfo in GameInfo.Terrains() do
		weights[itemInfo.Type] = 0;
	end
	for itemInfo in GameInfo.Features() do
		weights[itemInfo.Type] = 0;
	end

	for nearPlot, distance in Plot_GetPlotsInCircle(plot, minR, maxR) do
		local nearWeight = (distance == 0) and 6 or 1 / distance;
		local plotType = plotTypeName[nearPlot:GetPlotType()];
		local terrainType = terrainTypeName[nearPlot:GetTerrainType()];
		local featureType = featureTypeName[nearPlot:GetFeatureType()] or "NO_FEATURE";

		weights.TOTAL = weights.TOTAL + nearWeight;
		weights[plotType] = weights[plotType] + nearWeight;
		weights[terrainType] = weights[terrainType] + nearWeight;
		weights[featureType] = weights[featureType] + nearWeight;

		if plotType == "PLOT_OCEAN" then
			if not nearPlot:IsLake() and featureType ~= "FEATURE_ICE" then
				weights.SEA = weights.SEA + nearWeight;
			end
		end
	end

	if weights.TOTAL == 0 then
		print("plot:GetAreaWeights Total = 0! x = %s y = %s", plot:GetX(), plot:GetY());
	end

	for k in pairs(weights) do
		if k ~= "TOTAL" then
			weights[k] = weights[k] / weights.TOTAL;
		end
	end

	return weights;
end

function Plot_GetElevation(plot, ignoreSeas)
	if ignoreSeas and Contains(MG.seaPlots, plot) then
		-- try to preserve inland seas
		return elevationMap.seaLevelThreshold;
	end
	return elevationMap.data[elevationMap:GetIndex(plot:GetX(), plot:GetY())];
end

function GetElevationByPlotID(plotID)
	local elevation = elevationMap.data[plotID];
	if not elevation then
		print(string.format("GetElevationByPlotID elevationMap.data[%s] = %s", plotID, elevation));
		return 0;
	end
	return elevation;
end

function Plot_IsWater(plot, useElevation, ignoreSeas)
	if useElevation then
		return elevationMap.data[elevationMap:GetIndex(plot:GetX(), plot:GetY())] < elevationMap.seaLevelThreshold;
	end
	if ignoreSeas and Contains(MG.seaPlots, plot) then
		return false;
	end
	return plot:GetPlotType() == PlotTypes.PLOT_OCEAN or Contains(MG.lakePlots, plot);
end

function Plot_IsLake(plot)
	return plot:IsLake() or Contains(MG.lakePlots, plot);
end

function WaterMatch(x, y)
	return elevationMap:IsBelowSeaLevel(x, y);
end

function OceanButNotIceMatch(x, y)
	local plot = Map.GetPlot(x, y);
	return plot:GetFeatureType() ~= FeatureTypes.FEATURE_ICE and plot:GetPlotType() == PlotTypes.PLOT_OCEAN and not Plot_IsLake(plot);
end

function LandMatch(x, y)
	return not elevationMap:IsBelowSeaLevel(x, y);
end

function OceanMatch(x, y)
	local plot = Map.GetPlot(x, y);
	if plot:GetPlotType() == PlotTypes.PLOT_OCEAN then
		return true;
	end
	return false;
end

function DesertMatch(x, y)
	local plot = Map.GetPlot(x, y);
	if plot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
		return true;

	-- Include any mountains on the border as part of the desert.
	elseif Plot_IsMountain(plot) then
		local nList = elevationMap:GetRadiusAroundHex(x, y, 1);
		for n = 1, #nList do
			local xx = nList[n][1];
			local yy = nList[n][2];
			local ii = elevationMap:GetIndex(xx, yy);
			if ii ~= -1 then
				local nPlot = Map.GetPlot(xx, yy);
				if not Plot_IsMountain(nPlot) and nPlot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
					return true;
				end
			end
		end
	end
	return false;
end

function Plot_IsMountain(plot)
	return plot:IsMountain() or Contains(MG.mountainPasses, plot);
end

function Plot_IsRiver(plot, edgeDirection)
	if edgeDirection then
		return Plot_IsRiverInDirection(plot, edgeDirection);
	end
	for _, direction in pairs(MG.edgeDirections) do
		if Plot_IsRiverInDirection(plot, direction) then
			return true;
		end
	end
	return false;
end

function Plot_GetPreviousRiverPlot(plot, edgeDirection)
	local flowDirection = Plot_GetRiverFlowDirection(plot, edgeDirection);
	local nearDirection = MG.C;
	if edgeDirection == (flowDirection + 1) % 6 then
		nearDirection = (flowDirection + 2) % 6;
	else
		nearDirection = (flowDirection + 3) % 6;
	end
	local nearPlot = Map.PlotDirection(plot:GetX(), plot:GetY(), nearDirection);
	local edgeA = (flowDirection - 1) % 6;
	local edgeB = (flowDirection + 0) % 6;
	local flowA = (flowDirection + 1) % 6;
	local flowB = (flowDirection - 1) % 6;
	return nearPlot, edgeA, edgeB, flowA, flowB;
end

function Plot_GetNextRiverPlot(plot, edgeDirection)
	local flowDirection = Plot_GetRiverFlowDirection(plot, edgeDirection);
	local nearDirection = MG.C;
	if edgeDirection == (flowDirection + 1) % 6 then
		nearDirection = (flowDirection + 0) % 6;
	else
		nearDirection = (flowDirection - 1) % 6;
	end
	local nearPlot = Map.PlotDirection(plot:GetX(), plot:GetY(), nearDirection);
	local edgeA = (flowDirection + 2) % 6;
	local edgeB = (flowDirection + 3) % 6;
	local flowA = (flowDirection + 4) % 6;
	local flowB = (flowDirection + 2) % 6;
	return nearPlot, edgeA, flowA, edgeB, flowB;
end

function Plot_GetRiverFlowDirection(plot, edgeDirection)
	if edgeDirection == MG.E then
		return plot:GetRiverEFlowDirection();
	elseif edgeDirection == MG.SW then
		return plot:GetRiverSWFlowDirection();
	elseif edgeDirection == MG.SE then
		return plot:GetRiverSEFlowDirection();
	else
		local nextPlot = Map.PlotDirection(plot:GetX(), plot:GetY(), edgeDirection);
		if not nextPlot then
			return MG.flowNONE;
		end
		if edgeDirection == MG.W then
			return nextPlot:GetRiverEFlowDirection();
		elseif edgeDirection == MG.NE then
			return nextPlot:GetRiverSWFlowDirection();
		elseif edgeDirection == MG.NW then
			return nextPlot:GetRiverSEFlowDirection();
		end
	end
	return MG.flowNONE;
end

function Plot_GetRiverRotation(plot, edgeDirection)
	if not Plot_IsRiver(plot, edgeDirection) then
		return 0; -- no river
	end
	if Plot_GetRiverFlowDirection(plot, edgeDirection) == (edgeDirection + 2) % 6 then
		return 1; -- clockwise
	end
	return -1; -- counterclockwise
end

function Plot_IsRiverInDirection(plot, edgeDirection)
	if edgeDirection == MG.E then
		return plot:IsWOfRiver();
	elseif edgeDirection == MG.SE then
		return plot:IsNWOfRiver();
	elseif edgeDirection == MG.SW then
		return plot:IsNEOfRiver();
	else
		local nextPlot = Map.PlotDirection(plot:GetX(), plot:GetY(), edgeDirection);
		if not nextPlot then
			return false
		end
		if edgeDirection == MG.W then
			return nextPlot:IsWOfRiver();
		elseif edgeDirection == MG.NW then
			return nextPlot:IsNWOfRiver();
		elseif edgeDirection == MG.NE then
			return nextPlot:IsNEOfRiver();
		end
	end
	return false;
end

function Plot_SetRiver(plot, edgeDirection, flowDirection)
	local isRiver = (flowDirection ~= MG.flowNONE);
	if edgeDirection == MG.E then
		plot:SetWOfRiver(isRiver, flowDirection);
	elseif edgeDirection == MG.SE then
		plot:SetNWOfRiver(isRiver, flowDirection);
	elseif edgeDirection == MG.SW then
		plot:SetNEOfRiver(isRiver, flowDirection);
	else
		plot = Map.PlotDirection(plot:GetX(), plot:GetY(), edgeDirection);
		if not plot then
			return false;
		end
		if edgeDirection == MG.W then
			plot:SetWOfRiver(isRiver, flowDirection);
		elseif edgeDirection == MG.NW then
			plot:SetNWOfRiver(isRiver, flowDirection);
		elseif edgeDirection == MG.NE then
			plot:SetNEOfRiver(isRiver, flowDirection);
		end
	end
	return true;
end

----------------
-- Utilities
----------------
function Round(num, places)
	local mult = 10 ^ (places or 0);
	return math.floor(num * mult + 0.5) / mult;
end

function IsBetween(lower, mid, upper)
	return lower <= mid and mid <= upper;
end

function Contains(list, value)
	for _, v in pairs(list) do
		if v == value then
			return true;
		end
	end
	return false;
end

function DeepCopy(object)
	-- DeepCopy(object) copies all elements of a table
	local lookup_table = {};
	local function _copy(o)
		if type(o) ~= "table" then
			return o;
		elseif lookup_table[o] then
			return lookup_table[o];
		end
		local new_table = {};
		lookup_table[o] = new_table;
		for index, value in pairs(o) do
			new_table[_copy(index)] = _copy(value);
		end
		return setmetatable(new_table, getmetatable(o));
	end
	return _copy(object);
end

function Constrain(lower, mid, upper)
	return math.max(lower, math.min(mid, upper));
end

function Push(a, item)
	table.insert(a, item);
end

function Pop(a)
	return table.remove(a);
end

function GetRandomWeighted(list, size)
	-- GetRandomWeighted(list, size) returns a key from a list of (key, weight) pairs
	size = size or 100;
	local chanceIDs = GetWeightedTable(list, size);

	if chanceIDs == -1 then
		return -1;
	end
	local randomID = 1 + Map.Rand(size, "GetRandomWeighted");
	if not chanceIDs[randomID] then
		print("GetRandomWeighted: invalid random index selected = %s", randomID);
		chanceIDs[randomID] = -1;
	end
	return chanceIDs[randomID];
end

function GetWeightedTable(list, size)
	-- GetWeightedTable(list, size) returns a table with key blocks sized proportionately to a weighted list
	local totalWeight = 0;
	local chanceIDs = {};
	local position = 1;

	for _, weight in pairs(list) do
		totalWeight = totalWeight + weight;
	end

	if totalWeight == 0 then
		for key in pairs(list) do
			list[key] = 1;
			totalWeight = totalWeight + 1;
		end
		if totalWeight == 0 then
			print("GetWeightedTable: empty list");
			return -1;
		end
	end

	for key, weight in pairs(list) do
		local positionNext = position + size * weight / totalWeight;
		for i = math.floor(position), math.floor(positionNext) do
			chanceIDs[i] = key;
		end
		position = positionNext;
	end
	return chanceIDs;
end

function GetBellCurve(value, normalize)
	-- Returns a value along a bell curve from a 0 - 1 range
	if normalize then
		value = 1 - math.abs(value - normalize) / normalize;
	end
	return math.sin(value * math.pi * 2 - math.pi / 2) / 2 + 0.5;
end

--------------------------------------------
-- Interpolation and Perlin functions
--------------------------------------------
function CubicInterpolate(v0, v1, v2, v3, mu)
	local mu2 = mu * mu;
	local a0 = v3 - v2 - v0 + v1;
	local a1 = v0 - v1 - a0;
	local a2 = v2 - v0;
	local a3 = v1;

	return a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3;
end

function BicubicInterpolate(v, muX, muY)
	local a0 = CubicInterpolate(v[1], v[2], v[3], v[4], muX);
	local a1 = CubicInterpolate(v[5], v[6], v[7], v[8], muX);
	local a2 = CubicInterpolate(v[9], v[10], v[11], v[12], muX);
	local a3 = CubicInterpolate(v[13], v[14], v[15], v[16], muX);

	return CubicInterpolate(a0, a1, a2, a3, muY);
end

function CubicDerivative(v0, v1, v2, v3, mu)
	local mu2 = mu * mu;
	local a0 = v3 - v2 - v0 + v1;
	local a1 = v0 - v1 - a0;
	local a2 = v2 - v0;

	return 3 * a0 * mu2 + 2 * a1 * mu + a2;
end

function BicubicDerivative(v, muX, muY)
	local a0 = CubicInterpolate(v[1], v[2], v[3], v[4], muX);
	local a1 = CubicInterpolate(v[5], v[6], v[7], v[8], muX);
	local a2 = CubicInterpolate(v[9], v[10], v[11], v[12], muX);
	local a3 = CubicInterpolate(v[13], v[14], v[15], v[16], muX);

	return CubicDerivative(a0, a1, a2, a3, muY);
end

function GetInterpolatedValue(X, Y, srcMap)
	-- This function gets a smoothly interpolated value from srcMap.
	-- x and y are non-integer coordinates of where the value is to be calculated, and wrap in both directions.
	-- srcMap is an object of type FloatMap.
	local points = {};
	local fractionX = X - math.floor(X);
	local fractionY = Y - math.floor(Y);

	-- wrappedX and wrappedY are set to -1,-1 of the sampled area so that the sample area is in the middle quad of the 4x4 grid.
	local wrappedX = (math.floor(X) - 1) % srcMap.rectWidth + srcMap.rectX;
	local wrappedY = (math.floor(Y) - 1) % srcMap.rectHeight + srcMap.rectY;

	for pY = 0, 4 - 1 do
		local y = pY + wrappedY;
		for pX = 0, 4 - 1 do
			local x = pX + wrappedX;
			local srcIndex = srcMap:GetRectIndex(x, y);
			points[(pY * 4 + pX) + 1] = srcMap.data[srcIndex];
		end
	end

	return BicubicInterpolate(points, fractionX, fractionY);
end

function GetDerivativeValue(X, Y, srcMap)
	local points = {};
	local fractionX = X - math.floor(X);
	local fractionY = Y - math.floor(Y);

	-- wrappedX and wrappedY are set to -1,-1 of the sampled area
	-- so that the sample area is in the middle quad of the 4x4 grid
	local wrappedX = (math.floor(X) - 1) % srcMap.rectWidth + srcMap.rectX;
	local wrappedY = (math.floor(Y) - 1) % srcMap.rectHeight + srcMap.rectY;

	for pY = 0, 4 - 1 do
		local y = pY + wrappedY;
		for pX = 0, 4 - 1 do
			local x = pX + wrappedX;
			local srcIndex = srcMap:GetRectIndex(x, y);
			points[(pY * 4 + pX) + 1] = srcMap.data[srcIndex];
		end
	end

	return BicubicDerivative(points, fractionX, fractionY);
end

function GetPerlinNoise(x, y, destMapWidth, destMapHeight, initialFrequency, initialAmplitude, amplitudeChange, octaves, noiseMap)
	-- This function gets Perlin noise for the destination coordinates.
	-- Note that in order for the noise to wrap, the area sampled on the noise map must change to fit each octave.
	local finalValue = 0.0;
	local frequency = initialFrequency;
	local amplitude = initialAmplitude;
	local frequencyX; -- slight adjustment for seamless wrapping
	local frequencyY;
	for _ = 1, octaves do
		if noiseMap.wrapX then
			noiseMap.rectX = math.floor(noiseMap.width / 2 - (destMapWidth * frequency) / 2);
			noiseMap.rectWidth = math.max(math.floor(destMapWidth * frequency), 1);
			frequencyX = noiseMap.rectWidth / destMapWidth;
		else
			noiseMap.rectX = 0;
			noiseMap.rectWidth = noiseMap.width;
			frequencyX = frequency;
		end
		if noiseMap.wrapY then
			noiseMap.rectY = math.floor(noiseMap.height / 2 - (destMapHeight * frequency) / 2);
			noiseMap.rectHeight = math.max(math.floor(destMapHeight * frequency), 1);
			frequencyY = noiseMap.rectHeight / destMapHeight;
		else
			noiseMap.rectY = 0;
			noiseMap.rectHeight = noiseMap.height;
			frequencyY = frequency;
		end
		finalValue = finalValue + GetInterpolatedValue(x * frequencyX, y * frequencyY, noiseMap) * amplitude;
		frequency = frequency * 2.0;
		amplitude = amplitude * amplitudeChange;
	end
	finalValue = finalValue / octaves;
	return finalValue;
end

function GetPerlinDerivative(x, y, destMapWidth, destMapHeight, initialFrequency, initialAmplitude, amplitudeChange, octaves, noiseMap)
	local finalValue = 0.0;
	local frequency = initialFrequency;
	local amplitude = initialAmplitude;
	local frequencyX; -- slight adjustment for seamless wrapping
	local frequencyY;
	for _ = 1, octaves do
		if noiseMap.wrapX then
			noiseMap.rectX = math.floor(noiseMap.width / 2 - (destMapWidth * frequency) / 2);
			noiseMap.rectWidth = math.floor(destMapWidth * frequency);
			frequencyX = noiseMap.rectWidth / destMapWidth;
		else
			noiseMap.rectX = 0;
			noiseMap.rectWidth = noiseMap.width;
			frequencyX = frequency;
		end
		if noiseMap.wrapY then
			noiseMap.rectY = math.floor(noiseMap.height / 2 - (destMapHeight * frequency) / 2);
			noiseMap.rectHeight = math.floor(destMapHeight * frequency);
			frequencyY = noiseMap.rectHeight / destMapHeight;
		else
			noiseMap.rectY = 0;
			noiseMap.rectHeight = noiseMap.height;
			frequencyY = frequency;
		end
		finalValue = finalValue + GetDerivativeValue(x * frequencyX, y * frequencyY, noiseMap) * amplitude;
		frequency = frequency * 2.0;
		amplitude = amplitude * amplitudeChange;
	end
	finalValue = finalValue / octaves;
	return finalValue;
end

----------------------------
-- ShiftMap Class
----------------------------
function ShiftMaps()
	local shift_x = 0;
	local shift_y = 0;

	shift_x = DetermineXShift();

	ShiftMapsBy(shift_x, shift_y);
end

function ShiftMapsBy(xshift, yshift)
	local W, H = Map.GetGridSize();
	if xshift > 0 or yshift > 0 then
		local shift = {};
		local iDestI = 0;
		for iDestY = 0, H - 1 do
			for iDestX = 0, W - 1 do
				local iSourceX = (iDestX + xshift) % W;

				-- local iSourceY = (iDestY + yshift) % H; -- If using yshift, enable this and comment out the faster line below. - Bobert13
				local iSourceY = iDestY;

				local iSourceI = W * iSourceY + iSourceX;
				shift[iDestI] = elevationMap.data[iSourceI];
				iDestI = iDestI + 1;
			end
		end
		elevationMap.data = shift; -- It's faster to do one large table operation here than it is to do thousands of small operations to set up a copy of the input table at the beginning. - Bobert13
	end
	return elevationMap;
end

function DetermineXShift()
	--[[
	This function will align the most water-heavy vertical portion of the map with the vertical map edge.
	This is a form of centering the landmasses, but it emphasizes the edge not the middle.
	If there are columns completely empty of land, these will tend to be chosen as the new map edge,
	but it is possible for a narrow column between two large continents to be passed over in favor of the thinnest section of a continent,
	because the operation looks at a group of columns not just a single column,
	then picks the center of the most water heavy group of columns to be the new vertical map edge.
	--]]

	-- First loop through the map columns and record land plots in each column.
	local mapW, mapH = Map.GetGridSize();
	local land_totals = {};
	for x = 0, mapW - 1 do
		local current_column = 0;
		for y = 0, mapH - 1 do
			if not elevationMap:IsBelowSeaLevel(x, y) then
				current_column = current_column + 1;
			end
		end
		table.insert(land_totals, current_column);
	end

	-- Now evaluate column groups, each record applying to the center column of the group.
	local column_groups = {};
	-- Determine the group size in relation to map width.
	local group_radius = 3;
	-- Measure the groups.
	for column_index = 1, mapW do
		local current_group_total = 0;
		-- for current_column = column_index - group_radius, column_index + group_radius do
		-- Changed how group_radius works to get groups of four. -Bobert13
		for current_column = column_index, column_index + group_radius do
			local current_index = current_column % mapW;
			if current_index == 0 then -- Modulo of the last column will be zero this repairs the issue.
				current_index = mapW;
			end
			current_group_total = current_group_total + land_totals[current_index];
		end
		table.insert(column_groups, current_group_total);
	end

	-- Identify the group with the least amount of land in it.
	local best_value = mapH * (group_radius + 1); -- Set initial value to max possible.
	local best_group = 1; -- Set initial best group as current map edge.
	for column_index, group_land_plots in ipairs(column_groups) do
		if group_land_plots < best_value then
			best_value = group_land_plots;
			best_group = column_index;
		end
	end

	-- Determine X Shift
	local x_shift = best_group + 2;

	return x_shift;
end

-----------------------------------------------------------------------------
-- Random functions
-- Use lua rands for stand alone script running and Map.rand for in game.
-----------------------------------------------------------------------------
function PWRand()
	if Map then
		return Map.Rand(10000, "Random - Lua") / 10000;
	end
	return math.random();
end

function PWRandSeed(fixedseed)
	local seed
	if not fixedseed then
		seed = (Map.Rand(32767, "") * 65536) + Map.Rand(65535, "");
	else
		seed = fixedseed;
	end
	math.randomseed(seed);
	print("random seed for this map is " .. seed);
end

function PWRandint(low, high)
	-- range is inclusive, low and high are possible results
	return math.random(low, high);
end

-- Get random multiplier normalized to 1
-- rand: optional random value
-- higher: optional boolean, determines >1 or <1
function GetRandomMultiplier(variance, rand, higher)
	if higher == nil then
		higher = (1 == Map.Rand(2, "GetRandomMultiplier"));
	end

	local multiplier = 1;
	if not rand then
		multiplier = 1 + PWRand() * variance;
	else
		multiplier = 1 + rand * variance;
	end

	if higher then
		return multiplier;
	else
		return 1 / multiplier;
	end
end

-- Get modified odds when you roll the dice multiple times
function ModifyOdds(odds, multiplier)
	if odds >= 1 then
		return 1;
	end
	if multiplier <= 0 then
		return 0;
	end
	return 1 - math.pow(1 - odds, multiplier);
end

-- Get modified odds with random multiplier normalized to 1
-- rand: optional random value
-- higher: optional boolean, determines >1 or <1
function ModifyOddsRandom(odds, variance, rand, higher)
	return ModifyOdds(odds, GetRandomMultiplier(variance, rand, higher));
end

----------------------
-- FloatMap class
----------------------
--[[
This is for storing 2D map data.
The 'data' field is a zero based, one dimensional array.
To access map data by x and y coordinates, use the GetIndex method to obtain the 1D index,
which will handle any needs for wrapping in the x and y directions.
--]]
-----------------------------------------------------------------------------
FloatMap = InheritsFrom(nil);

function FloatMap:New(width, height, wrapX, wrapY)
	local new_inst = {};
	setmetatable(new_inst, {__index = FloatMap}); -- setup metatable

	new_inst.width = width;
	new_inst.height = height;
	new_inst.wrapX = wrapX;
	new_inst.wrapY = wrapY;
	new_inst.length = width * height;

	-- These fields are used to access only a subset of the map with the GetRectIndex function.
	-- This is useful for making Perlin noise wrap without generating separate noise fields for each octave.
	new_inst.rectX = 0;
	new_inst.rectY = 0;
	new_inst.rectWidth = width;
	new_inst.rectHeight = height;

	new_inst.data = {};
	for i = 0, width * height - 1 do
		new_inst.data[i] = 0.0;
	end

	return new_inst;
end

function FloatMap:GetNeighbor(x, y, dir)
	local odd = y % 2;
	if dir == MG.C then
		return x, y;
	elseif dir == MG.W then
		return x - 1, y;
	elseif dir == MG.NW then
		return x - 1 + odd, y + 1;
	elseif dir == MG.NE then
		return x + odd, y + 1;
	elseif dir == MG.E then
		return x + 1, y;
	elseif dir == MG.SE then
		return x + odd, y - 1;
	elseif dir == MG.SW then
		return x - 1 + odd, y - 1;
	else
		print(string.format("Bad direction %s in FloatMap:GetNeighbor", dir));
	end
	return -1, -1;
end

function FloatMap:GetIndex(x, y)
	if not self.wrapY and (y < 0 or y > self.height - 1) then
		return -1;
	elseif not self.wrapX and (x < 0 or x > self.width - 1) then
		return -1;
	end
	return (y % self.height) * self.width + x % self.width;
end

function FloatMap:GetXYFromIndex(i)
	local x = i % self.width;
	local y = (i - x) / self.width;
	return x, y;
end

-- Quadrants are labeled
-- A B
-- D C
function FloatMap:GetQuadrant(x, y)
	if x < self.width / 2 then
		if y < self.height / 2 then
			return "A";
		else
			return "D";
		end
	else
		if y < self.height / 2 then
			return "B";
		else
			return "C";
		end
	end
end

-- Gets an index for x and y based on the current rect settings. x and y are local to the defined rect.
-- Wrapping is assumed in both directions
function FloatMap:GetRectIndex(x, y)
	local xx = x % self.rectWidth;
	local yy = y % self.rectHeight;

	xx = self.rectX + xx;
	yy = self.rectY + yy;

	return self:GetIndex(xx, yy);
end

function FloatMap:Normalize()
	-- Find highest and lowest values
	local maxAlt = -1000.0;
	local minAlt = 1000.0;
	for i = 0, self.length - 1 do
		local alt = self.data[i];
		if alt > maxAlt then
			maxAlt = alt;
		end
		if alt < minAlt then
			minAlt = alt;
		end
	end

	-- Subtract minAlt from all values so that all values are zero and above
	for i = 0, self.length - 1 do
		self.data[i] = self.data[i] - minAlt;
	end

	-- Subract minAlt also from maxAlt
	maxAlt = maxAlt - minAlt;

	-- Determine and apply scaler to whole map
	local scaler;
	if maxAlt == 0.0 then
		scaler = 0.0;
	else
		scaler = 1.0 / maxAlt;
	end

	for i = 0, self.length - 1 do
		local expo;
		self.data[i], expo = math.frexp(self.data[i]);
		self.data[i] = self.data[i] * scaler;
		self.data[i] = math.ldexp(self.data[i], expo);
	end
end

function FloatMap:GenerateNoise()
	for i = 0, self.length - 1 do
		self.data[i] = PWRand();
	end
end

function FloatMap:GenerateBinaryNoise()
	for i = 0, self.length - 1 do
		if PWRand() > 0.5 then
			self.data[i] = 1;
		else
			self.data[i] = 0;
		end
	end
end

function FloatMap:FindThresholdFromPercent(percent, greaterThan, excludeZeros)
	local mapList = {};
	local percentage = percent * 100;

	if greaterThan then
		percentage = 100 - percentage;
	end

	if percentage >= 100 then
		return 1.01; -- whole map
	elseif percentage <= 0 then
		return -0.01; -- none of the map
	end

	for i = 0, self.length - 1 do
		if not (self.data[i] == 0.0 and excludeZeros) then
			table.insert(mapList, self.data[i]);
		end
	end

	table.sort(mapList, function (a, b) return a < b end);
	local threshIndex = math.floor(#mapList * percentage / 100);

	-- print(string.format("threshIndex %s = math.floor(%s * %s / 100)", threshIndex, #mapList, percentage));

	return mapList[threshIndex - 1];
end

function FloatMap:GetLatitudeForY(y)
	local range = MG.topLatitude - MG.bottomLatitude;
	local lat;
	if y < self.height / 2 then
		lat = (y + 1) / self.height * range + (MG.bottomLatitude - MG.topLatitude / self.height);
	else
		lat = y / self.height * range + (MG.bottomLatitude + MG.topLatitude / self.height);
	end
	return lat;
end

function FloatMap:GetYForLatitude(lat)
	local range = MG.topLatitude - MG.bottomLatitude;
	return math.floor((lat - MG.bottomLatitude) / range * self.height + 0.5);
end

function FloatMap:GetZone(y)
	local lat = self:GetLatitudeForY(y);
	if y < 0 or y >= self.height then
		return MG.NOZONE;
	end
	if lat > MG.polarFrontLatitude then
		return MG.NPOLAR;
	elseif lat >= MG.horseLatitudes then
		return MG.NTEMPERATE;
	elseif lat >= 0.0 then
		return MG.NEQUATOR;
	elseif lat > -MG.horseLatitudes then
		return MG.SEQUATOR;
	elseif lat >= -MG.polarFrontLatitude then
		return MG.STEMPERATE;
	else
		return MG.SPOLAR;
	end
end

function FloatMap:GetYFromZone(zone, bTop)
	if bTop then
		for y = self.height - 1, 0, -1 do
			if zone == self:GetZone(y) then
				return y;
			end
		end
	else
		for y = 0, self.height - 1 do
			if zone == self:GetZone(y) then
				return y;
			end
		end
	end
	return -1;
end

function FloatMap:GetGeostrophicWindDirections(zone)
	if zone == MG.NPOLAR then
		return MG.SW, MG.W;
	elseif zone == MG.NTEMPERATE then
		return MG.NE, MG.E;
	elseif zone == MG.NEQUATOR then
		return MG.SW, MG.W;
	elseif zone == MG.SEQUATOR then
		return MG.NW, MG.W;
	elseif zone == MG.STEMPERATE then
		return MG.SE, MG.E;
	else
		return MG.NW, MG.W;
	end
end

function FloatMap:GetGeostrophicPressure(lat)
	local latRange;
	local latPercent;
	local pressure;
	if lat > MG.polarFrontLatitude then
		latRange = 90.0 - MG.polarFrontLatitude;
		latPercent = (lat - MG.polarFrontLatitude) / latRange;
		pressure = 1.0 - latPercent;
	elseif lat >= MG.horseLatitudes then
		latRange = MG.polarFrontLatitude - MG.horseLatitudes;
		latPercent = (lat - MG.horseLatitudes) / latRange;
		pressure = latPercent;
	elseif lat >= 0.0 then
		latRange = MG.horseLatitudes - 0.0;
		latPercent = (lat - 0.0) / latRange;
		pressure = 1.0 - latPercent;
	elseif lat > -MG.horseLatitudes then
		latRange = 0.0 + MG.horseLatitudes;
		latPercent = (lat + MG.horseLatitudes) / latRange;
		pressure = latPercent;
	elseif lat >= -MG.polarFrontLatitude then
		latRange = -MG.horseLatitudes + MG.polarFrontLatitude;
		latPercent = (lat + MG.polarFrontLatitude) / latRange;
		pressure = 1.0 - latPercent;
	else
		latRange = -MG.polarFrontLatitude + 90.0;
		latPercent = (lat + 90) / latRange;
		pressure = latPercent;
	end

	-- Prevents excessively high and low pressures which helps distribute rain more evenly in the affected areas. - Bobert13
	pressure = pressure + 1;
	if pressure > 1.5 then
		pressure = pressure * MG.pressureNorm;
	else
		pressure = pressure / MG.pressureNorm;
	end
	pressure = pressure - 1;

	-- print(pressure);
	return pressure;
end

function FloatMap:ApplyFunction(func)
	for i = 0, self.length - 1 do
		self.data[i] = func(self.data[i]);
	end
end

function FloatMap:GetRadiusAroundHex(x, y, radius)
	local list = {};
	table.insert(list, {x, y});
	if radius == 0 then
		return list;
	end

	local hereX = x;
	local hereY = y;

	-- Make a circle for each radius
	for r = 1, radius do
		-- Start 1 to the west
		hereX, hereY = self:GetNeighbor(hereX, hereY, MG.W);
		if self:IsOnMap(hereX, hereY) then
			table.insert(list, {hereX, hereY});
		end

		-- Go r times to the NE
		for _ = 1, r do
			hereX, hereY = self:GetNeighbor(hereX, hereY, MG.NE);
			if self:IsOnMap(hereX, hereY) then
				table.insert(list, {hereX, hereY});
			end
		end
		-- Go r times to the E
		for _ = 1, r do
			hereX, hereY = self:GetNeighbor(hereX, hereY, MG.E);
			if self:IsOnMap(hereX, hereY) then
				table.insert(list, {hereX, hereY});
			end
		end
		-- Go r times to the SE
		for _ = 1, r do
			hereX, hereY = self:GetNeighbor(hereX, hereY, MG.SE);
			if self:IsOnMap(hereX, hereY) then
				table.insert(list, {hereX, hereY});
			end
		end
		-- Go r times to the SW
		for _ = 1, r do
			hereX, hereY = self:GetNeighbor(hereX, hereY, MG.SW);
			if self:IsOnMap(hereX, hereY) then
				table.insert(list, {hereX, hereY});
			end
		end
		-- Go r times to the W
		for _ = 1, r do
			hereX, hereY = self:GetNeighbor(hereX, hereY, MG.W);
			if self:IsOnMap(hereX, hereY) then
				table.insert(list, {hereX, hereY});
			end
		end
		-- Go r - 1 times to the NW
		for _ = 1, r - 1 do
			hereX, hereY = self:GetNeighbor(hereX, hereY, MG.NW);
			if self:IsOnMap(hereX, hereY) then
				table.insert(list, {hereX, hereY});
			end
		end

		-- One extra NW to set up for next circle
		hereX, hereY = self:GetNeighbor(hereX, hereY, MG.NW);
	end
	return list;
end

function FloatMap:GetAverageInHex(x, y, radius)
	local sum = 0;
	local numPlots = 0;
	local nearPlotIDs = Plot_GetPlotsInCircleFast(x, y, radius);
	for _, nearPlotID in pairs(nearPlotIDs) do
		sum = sum + self.data[nearPlotID];
		numPlots = numPlots + 1;
	end
	return sum / numPlots;
end

function FloatMap:GetStdDevInHex(x, y, radius)
	local average = 0;
	local numPlots = 0;
	local nearPlotIDs = Plot_GetPlotsInCircleFast(x, y, radius);
	for _, nearPlotID in ipairs(nearPlotIDs) do
		average = average + self.data[nearPlotID];
		numPlots = numPlots + 1;
	end
	average = average / numPlots;

	local deviation = 0.0;
	for _, nearPlotID in ipairs(nearPlotIDs) do
		deviation = deviation + (self.data[nearPlotID] - average) ^ 2;
	end
	return math.sqrt(deviation / numPlots);
end

function FloatMap:Smooth(radius)
	local dataCopy = {};
	local i = 0;
	for y = 0, self.height - 1 do
		for x = 0, self.width - 1 do
			dataCopy[i] = self:GetAverageInHex(x, y, radius);
			i = i + 1;
		end
	end
	self.data = dataCopy;
end

function FloatMap:Deviate(radius)
	local dataCopy = {};
	local i = 0;
	for y = 0, self.height - 1 do
		for x = 0, self.width - 1 do
			dataCopy[i] = self:GetStdDevInHex(x, y, radius);
			i = i + 1;
		end
	end
	self.data = dataCopy;
end

function FloatMap:IsOnMap(x, y)
	local i = self:GetIndex(x, y);
	if i == -1 then
		return false;
	end
	return true;
end

function FloatMap:Save(name)
	print("Saving " .. name .. "..");
	local str = self.width .. "," .. self.height;
	for i = 0, self.length - 1 do
		str = str .. "," .. self.data[i];
	end
	local file = assert(io.open(name, "w+"));
	file:write(str);
	file:close();
	print("Bitmap saved as " .. name);
end

---------------
-- AreaMap
---------------
PWAreaMap = InheritsFrom(FloatMap);

function PWAreaMap:New(width, height, wrapX, wrapY)
	local new_inst = FloatMap:New(width, height, wrapX, wrapY);
	setmetatable(new_inst, {__index = PWAreaMap}); -- setup metatable

	new_inst.areaList = {};
	new_inst.segStack = {};
	return new_inst;
end

function PWAreaMap:DefineAreas(matchFunction)
	-- Zero map data
	for i = 0, self.width * self.height - 1 do
		self.data[i] = 0.0;
	end

	self.areaList = {};
	local currentAreaID = 0;
	for y = 0, self.height - 1 do
		for x = 0, self.width - 1 do
			local i = self:GetIndex(x, y);
			if self.data[i] == 0 then
				currentAreaID = currentAreaID + 1;
				local area = PWArea:New(currentAreaID, x, y, matchFunction(x, y));
				-- print(string.format("Filling area %d, matchFunction(x = %d, y = %d) = %s", area.id, x, y, tostring(matchFunction(x, y))));
				self:FillArea(x, y, area, matchFunction);
				table.insert(self.areaList, area);
			end
		end
	end
end

function PWAreaMap:FillArea(x, y, area, matchFunction)
	self.segStack = {};
	local seg = LineSeg:New(y, x, x, 1);
	Push(self.segStack, seg);
	seg = LineSeg:New(y + 1, x, x, -1);
	Push(self.segStack, seg);
	while #self.segStack > 0 do
		seg = Pop(self.segStack);
		self:ScanAndFillLine(seg, area, matchFunction);
	end
end

function PWAreaMap:ScanAndFillLine(seg, area, matchFunction)
	-- print(string.format("Processing line y = %d, xLeft = %d, xRight = %d, dy = %d -------", seg.y, seg.xLeft, seg.xRight, seg.dy));
	if self:ValidateY(seg.y + seg.dy) == -1 then
		return;
	end

	local odd = (seg.y + seg.dy) % 2;
	local notOdd = seg.y % 2;
	-- print(string.format("odd = %d, notOdd = %d", odd, notOdd));

	local lineFound = 0;
	local xStop;
	if self.wrapX then
		xStop = -self.width * 30;
	else
		xStop = -1;
	end

	local leftExtreme;
	for leftExt = seg.xLeft - odd, xStop + 1, -1 do
		leftExtreme = leftExt; -- need this saved
		local x = self:ValidateX(leftExtreme);
		local y = self:ValidateY(seg.y + seg.dy);
		local i = self:GetIndex(x, y);
		if self.data[i] == 0 and area.trueMatch == matchFunction(x, y) then
			self.data[i] = area.id;
			area.size = area.size + 1;
			lineFound = 1;
		else
			-- If no line was found, then leftExtreme is fine, but if a line was found going left,
			-- then we need to increment xLeftExtreme to represent the inclusive end of the line
			if lineFound == 1 then
				leftExtreme = leftExtreme + 1;
			end
			break;
		end
	end

	-- Now scan right to find extreme right, place each found segment on stack
	local rightExtreme;
	if self.wrapX then
		xStop = self.width * 20;
	else
		xStop = self.width;
	end
	for rightExt = seg.xLeft + lineFound - odd, xStop - 1 do
		rightExtreme = rightExt; -- need this saved
		local x = self:ValidateX(rightExtreme);
		local y = self:ValidateY(seg.y + seg.dy);
		local i = self:GetIndex(x, y);
		if self.data[i] == 0 and area.trueMatch == matchFunction(x, y) then
			self.data[i] = area.id;
			area.size = area.size + 1;
			-- print("adding to area");
			if lineFound == 0 then
				lineFound = 1 -- starting new line
				leftExtreme = rightExtreme;
			end
		elseif lineFound == 1 then -- found the right end of a line segment
			lineFound = 0;
			-- Put same direction on stack
			local newSeg = LineSeg:New(y, leftExtreme, rightExtreme - 1, seg.dy);
			Push(self.segStack, newSeg);
			-- Determine if we must put reverse direction on stack
			if leftExtreme < seg.xLeft - odd or rightExtreme >= seg.xRight + notOdd then
				-- Out of shadow so put reverse direction on stack
				newSeg = LineSeg:New(y, leftExtreme, rightExtreme - 1, -seg.dy);
				Push(self.segStack, newSeg);
			end
			if rightExtreme >= seg.xRight + notOdd then
				break;
			end
		elseif lineFound == 0 and rightExtreme >= seg.xRight + notOdd then
			break; -- past the end of the parent line and no line found
		end
		-- Continue finding segments
	end

	if lineFound == 1 then -- still needing a line to be put on stack
		print("still need line segments");
		lineFound = 0;
		-- Put same direction on stack
		local newSeg = LineSeg:New(seg.y + seg.dy, leftExtreme, rightExtreme - 1, seg.dy);
		Push(self.segStack, newSeg);
		print(string.format("pushing y = %d, xLeft = %d, xRight = %d, dy = %d", seg.y + seg.dy, leftExtreme, rightExtreme - 1, seg.dy));
		-- Determine if we must put reverse direction on stack
		if leftExtreme < seg.xLeft - odd or rightExtreme >= seg.xRight + notOdd then
			-- Out of shadow so put reverse direction on stack
			newSeg = LineSeg:New(seg.y + seg.dy, leftExtreme, rightExtreme - 1, -seg.dy);
			Push(self.segStack, newSeg);
			print(string.format("pushing y = %d, xLeft = %d, xRight = %d, dy = %d", seg.y + seg.dy, leftExtreme, rightExtreme - 1, -seg.dy));
		end
	end
end

function PWAreaMap:GetAreaByID(id)
	for i = 1, #self.areaList do
		if self.areaList[i].id == id then
			return self.areaList[i];
		end
	end
	error("Can't find area id in AreaMap.areaList");
end

function PWAreaMap:ValidateY(y)
	if self.wrapY then
		return y % self.height;
	elseif y < 0 or y >= self.height then
		return -1;
	else
		return y;
	end
end

function PWAreaMap:ValidateX(x)
	if self.wrapX then
		return x % self.width;
	elseif x < 0 or x >= self.width then
		return -1;
	else
		return x;
	end
end

function PWAreaMap:PrintAreaList()
	for i = 1, #self.areaList do
		local id = self.areaList[i].id;
		local seedx = self.areaList[i].seedx;
		local seedy = self.areaList[i].seedy;
		local size = self.areaList[i].size;
		local trueMatch = self.areaList[i].trueMatch;
		local str = string.format("area id = %d, trueMatch = %s, size = %d, seedx = %d, seedy = %d", id, tostring(trueMatch), size, seedx, seedy);
		print(str);
	end
end

---------------
-- Area class
---------------
PWArea = InheritsFrom(nil);
function PWArea:New(id, seedx, seedy, trueMatch)
	local new_inst = {};
	setmetatable(new_inst, {__index = PWArea}); -- setup metatable

	new_inst.id = id;
	new_inst.seedx = seedx;
	new_inst.seedy = seedy;
	new_inst.trueMatch = trueMatch;
	new_inst.size = 0;

	return new_inst;
end

-----------------
-- LineSeg class
-----------------
LineSeg = InheritsFrom(nil);
function LineSeg:New(y, xLeft, xRight, dy)
	local new_inst = {};
	setmetatable(new_inst, {__index = LineSeg}); -- setup metatable

	new_inst.y = y;
	new_inst.xLeft = xLeft;
	new_inst.xRight = xRight;
	new_inst.dy = dy;

	return new_inst;
end

-----------------
-- ElevationMap
-----------------
ElevationMap = InheritsFrom(FloatMap);

function ElevationMap:New(width, height, wrapX, wrapY)
	local new_inst = FloatMap:New(width, height, wrapX, wrapY);
	setmetatable(new_inst, {__index = ElevationMap}); -- setup metatable
	return new_inst;
end

function ElevationMap:IsBelowSeaLevel(x, y)
	local i = self:GetIndex(x, y);
	if self.data[i] < self.seaLevelThreshold then
		return true;
	else
		return false;
	end
end

---------------
-- RiverMap
---------------
RiverMap = InheritsFrom(nil);

function RiverMap:New()
	local new_inst = {};
	setmetatable(new_inst, {__index = RiverMap});

	new_inst.elevationMap = elevationMap;
	new_inst.riverData = {};
	for y = 0, new_inst.elevationMap.height - 1 do
		for x = 0, new_inst.elevationMap.width - 1 do
			local i = new_inst.elevationMap:GetIndex(x, y);
			new_inst.riverData[i] = RiverHex:New(x, y);
		end
	end

	return new_inst;
end

function RiverMap:GetJunction(x, y, isNorth)
	local i = self.elevationMap:GetIndex(x, y);
	if isNorth then
		return self.riverData[i].northJunction;
	else
		return self.riverData[i].southJunction;
	end
end

function RiverMap:GetJunctionNeighbor(direction, junction)
	local odd = junction.y % 2;
	if direction == MG.NOFLOW then
		error("can't get junction neighbor in direction NOFLOW");
	elseif direction == MG.WESTFLOW then
		local xx = junction.x + odd - 1;
		local yy;
		if junction.isNorth then
			yy = junction.y + 1;
		else
			yy = junction.y - 1;
		end
		if self.elevationMap:GetIndex(xx, yy) ~= -1 then
			return self:GetJunction(xx, yy, not junction.isNorth);
		end
	elseif direction == MG.EASTFLOW then
		local xx = junction.x + odd;
		local yy;
		if junction.isNorth then
			yy = junction.y + 1;
		else
			yy = junction.y - 1;
		end
		if self.elevationMap:GetIndex(xx, yy) ~= -1 then
			return self:GetJunction(xx, yy, not junction.isNorth);
		end
	elseif direction == MG.VERTFLOW then
		local xx = junction.x;
		local yy;
		if junction.isNorth then
			yy = junction.y + 2;
		else
			yy = junction.y - 2;
		end
		if self.elevationMap:GetIndex(xx, yy) ~= -1 then
			return self:GetJunction(xx, yy, not junction.isNorth);
		end
	end

	return nil; -- neighbor off map
end

-- Get the west or east hex neighboring this junction
function RiverMap:GetRiverHexNeighbor(junction, westNeighbor)
	local xx, yy;
	local odd = junction.y % 2;
	if junction.isNorth then
		yy = junction.y + 1;
	else
		yy = junction.y - 1;
	end
	if westNeighbor then
		xx = junction.x + odd - 1;
	else
		xx = junction.x + odd;
	end

	local ii = self.elevationMap:GetIndex(xx, yy);
	if ii ~= -1 then
		return self.riverData[ii];
	end

	return nil;
end

function RiverMap:SetJunctionAltitudes()
	for y = 0, self.elevationMap.height - 1 do
		for x = 0, self.elevationMap.width - 1 do
			local i = self.elevationMap:GetIndex(x, y);
			local vertAltitude = self.elevationMap.data[i];
			local vertNeighbor = self.riverData[i];

			-- First do north
			local westNeighbor = self:GetRiverHexNeighbor(vertNeighbor.northJunction, true);
			local eastNeighbor = self:GetRiverHexNeighbor(vertNeighbor.northJunction, false);

			local westAltitude = vertAltitude;
			if westNeighbor then
				local ii = self.elevationMap:GetIndex(westNeighbor.x, westNeighbor.y);
				if ii ~= -1 then
					westAltitude = self.elevationMap.data[ii];
				end
			end

			local eastAltitude = vertAltitude;
			if eastNeighbor then
				local ii = self.elevationMap:GetIndex(eastNeighbor.x, eastNeighbor.y);
				if ii ~= -1 then
					eastAltitude = self.elevationMap.data[ii];
				end
			end

			vertNeighbor.northJunction.altitude = math.min(vertAltitude, westAltitude, eastAltitude);

			-- Then south
			westNeighbor = self:GetRiverHexNeighbor(vertNeighbor.southJunction, true);
			eastNeighbor = self:GetRiverHexNeighbor(vertNeighbor.southJunction, false);

			westAltitude = vertAltitude;
			if westNeighbor then
				local ii = self.elevationMap:GetIndex(westNeighbor.x, westNeighbor.y);
				if ii ~= -1 then
					westAltitude = self.elevationMap.data[ii];
				end
			end

			eastAltitude = vertAltitude;
			if eastNeighbor then
				local ii = self.elevationMap:GetIndex(eastNeighbor.x, eastNeighbor.y);
				if ii ~= -1 then
					eastAltitude = self.elevationMap.data[ii];
				end
			end

			vertNeighbor.southJunction.altitude = math.min(vertAltitude, westAltitude, eastAltitude);
		end
	end
end

function RiverMap:IsLake(junction)
	-- First exclude the map edges that don't have neighbors
	if junction.y == 0 and not junction.isNorth then
		return false;
	elseif junction.y == self.elevationMap.height - 1 and junction.isNorth then
		return false;
	end

	-- Exclude altitudes below sea level
	if junction.altitude < self.elevationMap.seaLevelThreshold then
		return false;
	end

	-- print(string.format("junction = (%d,%d) N = %s, alt = %f", junction.x, junction.y, tostring(junction.isNorth), junction.altitude));

	local vertNeighbor = self:GetJunctionNeighbor(MG.VERTFLOW, junction);
	local vertAltitude;
	if vertNeighbor then
		vertAltitude = vertNeighbor.altitude;
		-- print(string.format("--vertNeighbor = (%d,%d) N = %s, alt = %f", vertNeighbor.x, vertNeighbor.y, tostring(vertNeighbor.isNorth), vertNeighbor.altitude));
	else
		vertAltitude = junction.altitude;
		-- print("--vertNeighbor == nil");
	end

	local westNeighbor = self:GetJunctionNeighbor(MG.WESTFLOW, junction);
	local westAltitude;
	if westNeighbor then
		westAltitude = westNeighbor.altitude;
		-- print(string.format("--westNeighbor = (%d,%d) N = %s, alt = %f", westNeighbor.x, westNeighbor.y, tostring(westNeighbor.isNorth), westNeighbor.altitude));
	else
		westAltitude = junction.altitude;
		-- print("--westNeighbor == nil");
	end

	local eastNeighbor = self:GetJunctionNeighbor(MG.EASTFLOW, junction);
	local eastAltitude;
	if eastNeighbor then
		eastAltitude = eastNeighbor.altitude;
		-- print(string.format("--eastNeighbor = (%d,%d) N = %s, alt = %f", eastNeighbor.x, eastNeighbor.y, tostring(eastNeighbor.isNorth), eastNeighbor.altitude));
	else
		eastAltitude = junction.altitude;
		-- print("--eastNeighbor == nil");
	end

	local lowest = math.min(vertAltitude, westAltitude, eastAltitude, junction.altitude);

	if lowest == junction.altitude then
		-- print("--is lake");
		return true;
	end
	-- print("--is not lake");
	return false;
end

-- Get the average altitude of the two lowest neighbors that are higher than the junction altitude.
function RiverMap:GetLowerNeighborAverage(junction)
	local vertNeighbor = self:GetJunctionNeighbor(MG.VERTFLOW, junction);
	local vertAltitude;
	if vertNeighbor then
		vertAltitude = vertNeighbor.altitude;
	else
		vertAltitude = junction.altitude;
	end

	local westNeighbor = self:GetJunctionNeighbor(MG.WESTFLOW, junction);
	local westAltitude;
	if westNeighbor then
		westAltitude = westNeighbor.altitude;
	else
		westAltitude = junction.altitude;
	end

	local eastNeighbor = self:GetJunctionNeighbor(MG.EASTFLOW, junction);
	local eastAltitude;
	if eastNeighbor then
		eastAltitude = eastNeighbor.altitude;
	else
		eastAltitude = junction.altitude;
	end

	local nList = {vertAltitude, westAltitude, eastAltitude};
	table.sort(nList);
	local avg;
	if nList[1] > junction.altitude then
		avg = (nList[1] + nList[2]) / 2.0;
	elseif nList[2] > junction.altitude then
		avg = (nList[2] + nList[3]) / 2.0;
	elseif nList[3] > junction.altitude then
		avg = (nList[3] + junction.altitude) / 2.0;
	else
		avg = junction.altitude; -- All neighbors are the same height. Dealt with later
	end
	return avg;
end

-- this function alters the drainage pattern
function RiverMap:SiltifyLakes()
	local lakeList = {};
	local onQueueMapNorth = {};
	local onQueueMapSouth = {};
	for y = 0, self.elevationMap.height - 1 do
		for x = 0, self.elevationMap.width - 1 do
			local i = self.elevationMap:GetIndex(x, y);
			onQueueMapNorth[i] = false;
			onQueueMapSouth[i] = false;
			if self:IsLake(self.riverData[i].northJunction) then
				Push(lakeList, self.riverData[i].northJunction);
				onQueueMapNorth[i] = true;
			end
			if self:IsLake(self.riverData[i].southJunction) then
				Push(lakeList, self.riverData[i].southJunction);
				onQueueMapSouth[i] = true;
			end
		end
	end

	local longestLakeList = #lakeList;
	local shortestLakeList = #lakeList;
	local iterations = 0;
	local debugOn = false;
	-- print(string.format("initial lake count = %d", longestLakeList));
	while #lakeList > 0 do
		-- print(string.format("length of lakeList = %d",#lakeList));
		iterations = iterations + 1;
		if #lakeList > longestLakeList then
			longestLakeList = #lakeList;
		end

		if #lakeList < shortestLakeList then
			shortestLakeList = #lakeList;
			-- print(string.format("shortest lake list = %d, iterations = %d", shortestLakeList, iterations));
			iterations = 0;
		end

		if iterations > 1000000 then
			debugOn = true;
		end

		if iterations > 1001000 then
			error("endless loop in lake siltification. check logs");
		end

		local junction = Pop(lakeList);
		local i = self.elevationMap:GetIndex(junction.x, junction.y);
		if junction.isNorth then
			onQueueMapNorth[i] = false;
		else
			onQueueMapSouth[i] = false;
		end

		if debugOn then
			print(string.format("processing (%d,%d) N = %s alt = %f", junction.x, junction.y, tostring(junction.isNorth), junction.altitude));
		end

		local avgLowest = self:GetLowerNeighborAverage(junction);

		if debugOn then
			print(string.format("--avgLowest == %f", avgLowest));
		end

		if avgLowest < junction.altitude + 0.005 then -- cant use == in fp comparison
			junction.altitude = avgLowest + 0.005;
			if debugOn then
				print("--adding 0.005 to avgLowest");
			end
		else
			junction.altitude = avgLowest;
		end

		if debugOn then
			print(string.format("--changing altitude to %f", junction.altitude));
		end

		for dir = MG.WESTFLOW, MG.VERTFLOW do
			local neighbor = self:GetJunctionNeighbor(dir, junction);
			if debugOn and not neighbor then
				print(string.format("--nil neighbor at direction = %d", dir));
			end
			if neighbor and self:IsLake(neighbor) then
				local ii = self.elevationMap:GetIndex(neighbor.x, neighbor.y);
				if neighbor.isNorth and not onQueueMapNorth[ii] then
					Push(lakeList, neighbor);
					onQueueMapNorth[ii] = true;
					if debugOn then
						print(string.format("--pushing (%d,%d) N = %s alt = %f", neighbor.x, neighbor.y, tostring(neighbor.isNorth), neighbor.altitude));
					end
				elseif not neighbor.isNorth and not onQueueMapSouth[ii] then
					Push(lakeList, neighbor);
					onQueueMapSouth[ii] = true;
					if debugOn then
						print(string.format("--pushing (%d,%d) N = %s alt = %f", neighbor.x, neighbor.y, tostring(neighbor.isNorth), neighbor.altitude));
					end
				end
			end
		end
	end
end

function RiverMap:SetFlowDestinations()
	local junctionList = {};
	for y = 0, self.elevationMap.height - 1 do
		for x = 0, self.elevationMap.width - 1 do
			local i = self.elevationMap:GetIndex(x, y);
			table.insert(junctionList, self.riverData[i].northJunction);
			table.insert(junctionList, self.riverData[i].southJunction);
		end
	end

	table.sort(junctionList, function (a, b) return a.altitude > b.altitude end);

	for n = 1, #junctionList do
		local junction = junctionList[n];
		local validList = self:GetValidFlows(junction);
		if #validList > 0 then
			local choice = PWRandint(1, #validList);
			junction.flow = validList[choice];
		else
			junction.flow = MG.NOFLOW;
		end
	end
end

function RiverMap:GetValidFlows(junction)
	local validList = {};
	for dir = MG.WESTFLOW, MG.VERTFLOW do
		local neighbor = self:GetJunctionNeighbor(dir, junction);
		if neighbor and neighbor.altitude < junction.altitude then
			table.insert(validList, dir);
		end
	end
	return validList;
end

function RiverMap:IsTouchingOcean(junction)
	if elevationMap:IsBelowSeaLevel(junction.x, junction.y) then
		return true;
	end
	local westNeighbor = self:GetRiverHexNeighbor(junction, true);
	local eastNeighbor = self:GetRiverHexNeighbor(junction, false);

	if not westNeighbor or elevationMap:IsBelowSeaLevel(westNeighbor.x, westNeighbor.y) then
		return true;
	end
	if not eastNeighbor or elevationMap:IsBelowSeaLevel(eastNeighbor.x, eastNeighbor.y) then
		return true;
	end
	return false;
end

function RiverMap:SetRiverSizes()
	local junctionList = {}; -- only include junctions not touching ocean in this list
	for y = 0, self.elevationMap.height - 1 do
		for x = 0, self.elevationMap.width - 1 do
			local i = self.elevationMap:GetIndex(x, y);
			if not self:IsTouchingOcean(self.riverData[i].northJunction) then
				table.insert(junctionList, self.riverData[i].northJunction);
			end
			if not self:IsTouchingOcean(self.riverData[i].southJunction) then
				table.insert(junctionList, self.riverData[i].southJunction);
			end
		end
	end

	table.sort(junctionList, function (a, b) return a.altitude > b.altitude end);

	for n = 1, #junctionList do
		local junction = junctionList[n];
		local nextJunction = junction;
		local i = self.elevationMap:GetIndex(junction.x, junction.y);
		while true do
			nextJunction.size = (nextJunction.size + rainfallMap.data[i]) * MG.riverRainCheatFactor;
			if nextJunction.flow == MG.NOFLOW or self:IsTouchingOcean(nextJunction) then
				nextJunction.size = 0.0;
				break;
			end
			nextJunction = self:GetJunctionNeighbor(nextJunction.flow, nextJunction);
		end
	end

	-- Now sort by river size to find river threshold
	table.sort(junctionList, function (a, b) return a.size > b.size end);
	local riverIndex = math.floor(MG.riverPercent * #junctionList);
	self.riverThreshold = junctionList[riverIndex].size;
	print(string.format("river threshold = %f", self.riverThreshold));
end

-- This function returns the flow directions needed by civ
function RiverMap:GetFlowDirections(x, y)
	-- print(string.format("Get flow dirs for %d,%d", x, y));
	local i = elevationMap:GetIndex(x, y);

	local WOfRiver = MG.flowNONE;
	local xx, yy = elevationMap:GetNeighbor(x, y, MG.NE);
	local ii = elevationMap:GetIndex(xx, yy);
	if ii ~= -1 and self.riverData[ii].southJunction.flow == MG.VERTFLOW and self.riverData[ii].southJunction.size > self.riverThreshold then
		-- print(string.format("--NE(%d,%d) south flow = %d, size = %f", xx, yy, self.riverData[ii].southJunction.flow, self.riverData[ii].southJunction.size));
		WOfRiver = MG.flowS;
	end

	xx, yy = elevationMap:GetNeighbor(x, y, MG.SE);
	ii = elevationMap:GetIndex(xx, yy);
	if ii ~= -1 and self.riverData[ii].northJunction.flow == MG.VERTFLOW and self.riverData[ii].northJunction.size > self.riverThreshold then
		-- print(string.format("--SE(%d,%d) north flow = %d, size = %f", xx, yy, self.riverData[ii].northJunction.flow, self.riverData[ii].northJunction.size));
		WOfRiver = MG.flowN;
	end

	local NWOfRiver = MG.flowNONE;
	xx, yy = elevationMap:GetNeighbor(x, y, MG.SE);
	ii = elevationMap:GetIndex(xx, yy);
	if ii ~= -1 and self.riverData[ii].northJunction.flow == MG.WESTFLOW and self.riverData[ii].northJunction.size > self.riverThreshold then
		NWOfRiver = MG.flowSW;
	end
	if self.riverData[i].southJunction.flow == MG.EASTFLOW and self.riverData[i].southJunction.size > self.riverThreshold then
		NWOfRiver = MG.flowNE;
	end

	local NEOfRiver = MG.flowNONE;
	xx, yy = elevationMap:GetNeighbor(x, y, MG.SW);
	ii = elevationMap:GetIndex(xx, yy);
	if ii ~= -1 and self.riverData[ii].northJunction.flow == MG.EASTFLOW and self.riverData[ii].northJunction.size > self.riverThreshold then
		NEOfRiver = MG.flowSE;
	end
	if self.riverData[i].southJunction.flow == MG.WESTFLOW and self.riverData[i].southJunction.size > self.riverThreshold then
		NEOfRiver = MG.flowNW;
	end

	return WOfRiver, NWOfRiver, NEOfRiver;
end

--------------------
-- RiverHex class
--------------------
RiverHex = InheritsFrom(nil);
function RiverHex:New(x, y)
	local new_inst = {};
	setmetatable(new_inst, {__index = RiverHex});

	new_inst.x = x;
	new_inst.y = y;
	new_inst.northJunction = RiverJunction:New(x, y, true);
	new_inst.southJunction = RiverJunction:New(x, y, false);

	return new_inst;
end

------------------------
-- RiverJunction class
------------------------
RiverJunction = InheritsFrom(nil);
function RiverJunction:New(x, y, isNorth)
	local new_inst = {};
	setmetatable(new_inst, {__index = RiverJunction});

	new_inst.x = x;
	new_inst.y = y;
	new_inst.isNorth = isNorth;
	new_inst.altitude = 0.0;
	new_inst.flow = MG.NOFLOW;
	new_inst.size = 0.0;

	return new_inst;
end

--
-- RouteConnections.lua
--
-- Copyright 2011  (c)  William Howard
--
-- Determines if a route exists between two plots/cities
--
-- Permission granted to re-distribute this file as part of a mod
-- on the condition that this comment block is preserved in its entirity
--

----- PUBLIC METHODS -----

--
-- pPlayer                 - player object (not ID) or nil
-- pStartPlot, pTargetPlot - plot objects (not IDs)
-- sRoute                  - one of MG.routes (see above)
-- bShortestRoute          - true to find the shortest route
-- sHighlight              - one of the highlight keys (see above)
-- fBlockaded              - call-back function of the form f(pPlot, pPlayer) to determine if a plot is blocked for this player (return true if blocked)
--

function IsCityConnected(pPlayer, pStartCity, pTargetCity, sRoute, bShortestRoute, sHighlight, fBlockaded)
	return IsPlotConnected(pPlayer, pStartCity:Plot(), pTargetCity:Plot(), sRoute, bShortestRoute, sHighlight, fBlockaded);
end

function IsPlotConnected(pPlayer, pStartPlot, pTargetPlot, sRoute, bShortestRoute, sHighlight, fBlockaded)
	if bShortestRoute then
		MG.lastRouteLength = PlotToPlotShortestRoute(pPlayer, pStartPlot, pTargetPlot, sRoute, MG.highlights[sHighlight], fBlockaded);
	else
		MG.lastRouteLength = PlotToPlotConnection(pPlayer, pStartPlot, pTargetPlot, sRoute, 1, MG.highlights[sHighlight], ListAddPlot(pStartPlot, {}), fBlockaded);
	end

	return (MG.lastRouteLength ~= 0);
end

function GetRouteLength()
	return MG.lastRouteLength;
end

function GetDistance(pPlot1, pPlot2)
	return DistanceBetween(pPlot1, pPlot2);
end

----- PRIVATE DATA AND METHODS -----

--
-- Check if pStartPlot is connected to pTargetPlot
--
-- NOTE: This is a recursive method
--
-- Returns the length of the route between the start and target plots (inclusive) - so 0 if no route
--

function PlotToPlotConnection(pPlayer, pStartPlot, pTargetPlot, sRoute, iLength, highlight, listVisitedPlots, fBlockaded)
	if highlight then
		Events.SerialEventHexHighlight(PlotToHex(pStartPlot), true, highlight);
	end

	-- Have we got there yet?
	if IsSamePlot(pStartPlot, pTargetPlot) then
		return iLength;
	end

	-- Find any new plots we can visit from here
	local listRoutes = ListFilter(ReachablePlots(pPlayer, pStartPlot, sRoute, fBlockaded), listVisitedPlots);

	-- New routes to check, so there is an onward path
	-- Convert the associative array into a linear array so it can be sorted
	local array = {};
	for _, pPlot in pairs(listRoutes) do
		table.insert(array, pPlot);
	end

	-- Now sort the linear array by distance from the target plot
	table.sort(array, function(x, y) return (DistanceBetween(x, pTargetPlot) < DistanceBetween(y, pTargetPlot)) end);

	-- Now check each onward plot in turn to see if that is connected
	for _, pPlot in ipairs(array) do
		-- Check that a prior route didn't visit this plot
		if not ListContainsPlot(pPlot, listVisitedPlots) then
			-- Add this plot to the list of visited plots
			ListAddPlot(pPlot, listVisitedPlots);

			-- If there's a route, we're done
			local iLen = PlotToPlotConnection(pPlayer, pPlot, pTargetPlot, sRoute, iLength + 1, highlight, listVisitedPlots, fBlockaded);
			if iLen > 0 then
				return iLen;
			end
		end
	end

	if highlight then
		Events.SerialEventHexHighlight(PlotToHex(pStartPlot), false);
	end

	-- No connection found
	return 0;
end

--
-- Find the shortest route between two plots
--
-- We start at the TARGET plot - as the path length from here to the target plot is 1, we will call this "ring 1".
-- We then find all reachable near plots and place them in "ring 2".
-- If the START plot is in "ring 2", we have a route. if "ring 2" is empty, there is no route.
-- Otherwise find all reachable near plots that have not already been seen and place those in "ring 3".
-- We then loop, checking "ring N" otherwise generating "ring N+1".
--
-- Once we have found a route, the path length will be of length N and we know that there must be at least one route by picking a plot from each ring.
-- The plot needed from "ring N" is the START plot.
-- We then need ANY plot from "ring N-1" that is near to the start plot.
-- And in general we need any plot from "ring M-1" that is near to the plot choosen from "ring M".
-- The final plot in the path will always be the target plot as that is the only plot in "ring 1".
--
-- Returns the length of the route between the start and target plots (inclusive) - so 0 if no route
--

function PlotToPlotShortestRoute(pPlayer, pStartPlot, pTargetPlot, sRoute, highlight, fBlockaded)
	local rings = {};

	local iRing = 1;
	rings[iRing] = ListAddPlot(pTargetPlot, {});

	local bFound;
	repeat
		iRing = GenerateNextRing(pPlayer, sRoute, rings, iRing, fBlockaded);
		bFound = ListContainsPlot(pStartPlot, rings[iRing]);
	until bFound or not rings[iRing] or #rings[iRing] == 0;

	if bFound and highlight then
		Events.SerialEventHexHighlight(PlotToHex(pStartPlot), true, highlight);

		local pLastPlot = pStartPlot;
		for i = iRing - 1, 1, -1 do
			local pNextPlot = ListFirstAdjacentPlot(pLastPlot, rings[i]);

			-- Check should be completely unnecessary
			if not pNextPlot then
				return 0;
			end

			Events.SerialEventHexHighlight(PlotToHex(pNextPlot), true, highlight);
			pLastPlot = pNextPlot;
		end
	end

	return bFound and iRing or 0;
end

-- Helper method to find all plots near to the plots in the specified ring
function GenerateNextRing(pPlayer, sRoute, rings, iRing, fBlockaded)
	local nextRing = {};

	-- print("GenerateNextRing " .. iRing);
	for _, pPlot in pairs(rings[iRing]) do
		-- Consider two near tiles A and B, if A is in ring N, B must either be unvisited or in ring N-1;
		-- for if B was in ring N-2, A would have to be in ring N-1 - which it is not.
		local listRoutes = ListFilter(ReachablePlots(pPlayer, pPlot, sRoute, fBlockaded), iRing > 1 and rings[iRing - 1] or {});

		for _, pRoutePlot in pairs(listRoutes) do
			ListAddPlot(pRoutePlot, nextRing);
		end
	end

	rings[iRing + 1] = nextRing;

	return iRing + 1;
end

--
-- Methods dealing with finding all near tiles that can be reached by the specified route type
--

-- Return a list of (up to 6) reachable plots from this one by route type
function ReachablePlots(pPlayer, pPlot, sRoute, fBlockaded)
	local list = {};
	for pDestPlot in Plot_GetPlotsInCircle(pPlot, 1) do
		-- Don't let submarines fall over the edge!
		if pDestPlot then
			if not pPlayer or pDestPlot:IsRevealed(pPlayer:GetTeam()) then
				local bAdd = false;
				-- Be careful of order, must check for road before rail, and coastal before ocean
				if sRoute == MG.routes[1] and (pDestPlot:GetPlotType() == PlotTypes.PLOT_LAND or pDestPlot:GetPlotType() == PlotTypes.PLOT_HILLS) then
					bAdd = true;
				elseif sRoute == MG.routes[2] and pDestPlot:GetRouteType() >= 0 then
					bAdd = true;
				elseif sRoute == MG.routes[3] and pDestPlot:GetRouteType() >= 1 then
					bAdd = true;
				elseif sRoute == MG.routes[4] and pDestPlot:GetTerrainType() == TerrainTypes.TERRAIN_COAST then
					bAdd = true;
				elseif sRoute == MG.routes[5] and pDestPlot:IsWater() then
					bAdd = true;
				elseif sRoute == MG.routes[6] and pDestPlot:IsWater() then
					bAdd = true;
				end

				-- Special case for water, a city on the coast counts as water
				if not bAdd and (sRoute == MG.routes[4] or sRoute == MG.routes[5] or sRoute == MG.routes[6]) then
					bAdd = pDestPlot:IsCity();
				end

				-- Check for impassable and blockaded tiles
				if bAdd and IsPassable(pDestPlot, sRoute) and not IsBlockaded(pDestPlot, pPlayer, fBlockaded) then
					ListAddPlot(pDestPlot, list);
				end
			end
		end
	end
	return list;
end

-- Is the plot passable for this route type ..
function IsPassable(pPlot, sRoute)
	local bPassable = true;

	-- .. due to terrain, eg natural wonders and those covered in ice
	local iFeature = pPlot:GetFeatureType();
	-- Edited: not every Natural Wonder is impassable!
	if iFeature > 0 and GameInfo.Features[iFeature].NaturalWonder and GameInfo.Features[iFeature].Impassable then
		bPassable = false;
	elseif iFeature == FeatureTypes.FEATURE_ICE and sRoute ~= MG.routes[6] then
		bPassable = false;
	end

	return bPassable;
end

-- Is the plot blockaded for this player ..
function IsBlockaded(pPlot, pPlayer, fBlockaded)
	local bBlockaded = false;

	if fBlockaded then
		bBlockaded = fBlockaded(pPlot, pPlayer);
	end

	return bBlockaded;
end

--
-- Calculate the distance between two plots
--
-- See http://www-cs-students.stanford.edu/~amitp/Articles/HexLOS.html
-- Also http://keekerdc.com/2011/03/hexagon-grids-coordinate-systems-and-distance-calculations/
--
function DistanceBetween(pPlot1, pPlot2)
	local mapX, _ = Map.GetGridSize();

	-- Need to work on a hex based grid
	local hex1 = PlotToHex(pPlot1);
	local hex2 = PlotToHex(pPlot2);

	-- Calculate the distance between the x and z co-ordinate pairs allowing for the East-West wrap, (ie shortest route may be by going backwards)!
	local deltaX = math.min(math.abs(hex2.x - hex1.x), mapX - math.abs(hex2.x - hex1.x));
	local deltaZ = math.min(math.abs(hex2.z - hex1.z), mapX - math.abs(hex2.z - hex1.z));

	-- Calculate the distance between the y co-ordinates
	-- There is no North-South wrap, so this is easy
	local deltaY = math.abs(hex2.y - hex1.y);

	-- Calculate the distance between the plots
	local distance = math.max(deltaX, deltaY, deltaZ);

	-- Allow for both end points in the distance calculation
	return distance + 1;
end

-- Get the hex co-ordinates of a plot
function PlotToHex(pPlot)
	local hex = ToHexFromGrid(Vector2(pPlot:GetX(), pPlot:GetY()));

	-- X + y + z = 0, hence z = -(x + y)
	hex.z = -(hex.x + hex.y);

	return hex;
end

--
-- List (associative arrays) helper methods
--

-- Return a list formed by removing all entries from list1 which are in list2
function ListFilter(list1, list2)
	local list = {};

	if list1 then
		for sKey, pPlot in pairs(list1) do
			if not list2 or not list2[sKey] then
				list[sKey] = pPlot;
			end
		end
	end

	return list;
end

-- Return true if pPlot is in list
function ListContainsPlot(pPlot, list)
	return (list and list[GetPlotKey(pPlot)]);
end

-- Add the plot to the list
function ListAddPlot(pPlot, list)
	if list then
		list[GetPlotKey(pPlot)] = pPlot;
	end

	return list;
end

function ListFirstAdjacentPlot(pPlot, list)
	for _, plot in pairs(list) do
		if DistanceBetween(pPlot, plot) == 2 then
			return plot;
		end
	end

	-- We should NEVER reach here
	error("ListFirstAdjacentPlot - failed to find one!");
end

--
-- Plot helper methods
--

-- Are the plots one and the same?
function IsSamePlot(pPlot1, pPlot2)
	return (pPlot1:GetX() == pPlot2:GetX() and pPlot1:GetY() == pPlot2:GetY());
end

-- Get a unique key for the plot
function GetPlotKey(pPlot)
	return string.format("%d:%d", pPlot:GetX(), pPlot:GetY());
end

-- Get the grid-based (x, y) co-ordinates of the plot as a string
function PlotToGridStr(pPlot)
	if not pPlot then
		return "";
	end
	return string.format("(%d, %d)", pPlot:GetX(), pPlot:GetY());
end

-- Get the hex-based (x, y, z) co-ordinates of the plot as a string
function PlotToHexStr(pPlot)
	if not pPlot then
		return "";
	end
	local hex = PlotToHex(pPlot);
	return string.format("(%d, %d, %d)", hex.x, hex.y, hex.z);
end
