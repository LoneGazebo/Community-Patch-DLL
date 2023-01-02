--[[------------------------------------------------------------------------------

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

--]]------------------------------------------------------------------------------

include("MapGenerator")
include("FeatureGenerator")
include("TerrainGenerator")
include("IslandMaker")
include("FLuaVector")

MapGlobals = {}


local debugTime = false
local debugPrint = true
local debugWithLogger = false

--[[
Setting "overrideAssignStartingPlots = false" may help make the map compatible
with core game patches in the distant future when I'm no longer modding Civ 5.

This disables some advanced features of the map, so it's better to
modify the map's changes to AssignStartingPlots if possible.

~ Thalassicus @ Nov 5 2013

This is now a map option

~ azum4roll @ Sept 28 2019
--]]
local overrideAssignStartingPlots = true





--
-- Map Information
--

function MapGlobals:New()
	print("MapGlobals:New")
	local mglobal = {}
	setmetatable(mglobal, self)
	self.__index = self

	local mapW, mapH = Map.GetGridSize()


	--Percent of land tiles on the map.
	mglobal.landPercent				= 0.40

	--Top and bottom map latitudes.
	mglobal.topLatitude				= 90
	mglobal.bottomLatitude			= -mglobal.topLatitude


	--Important latitude markers used for generating climate.
	mglobal.tropicLatitudes			= 25	--    			   tropicLatitudes to 0 : grass, jungle
	mglobal.horseLatitudes			= 50	-- polarFrontLatitude to horseLatitudes : plains, desert
	mglobal.iceLatitude				= 65	-- 		  bottomLatitude to iceLatitude : ice
	mglobal.polarFrontLatitude		= 72	-- bottomLatitude to polarFrontLatitude : snow, tundra


	--Adjusting these will generate larger or smaller landmasses and features.
	mglobal.landMinScatter			= 0.02 	--Recommended range:[0.02 to 0.1]
	mglobal.landMaxScatter			= 0.04	--Recommended range:[0.03 to 0.3]
											--Higher values makes continental divisions and stringy features more likely,
											--and very high values result in a lot of stringy continents and islands.

	mglobal.coastScatter			= 0.02 	--Recommended range:[0.01 to 0.3]
											--Higher values result in more islands and variance on landmasses and coastlines.

	mglobal.mountainScatter			= 200 * mapW --Recommended range:[130 to 1000]
											--Lower values make large, long, mountain ranges. Higher values make sporadic mountainous features.


	-- Terrain
	mglobal.mountainWeight			= 1  --Weight of the mountain elevation map versus the coastline elevation map.
	mglobal.belowMountainPercent	= 0.95 -- Percent of non-mountain land
										   -- flatPercent to belowMountainPercent : hills
	mglobal.flatPercent			    = 0.70 -- Percent of flat land
	mglobal.hillsBlendPercent		= 0.15 -- Chance for flat land to become hills per near mountain. Requires at least 2 near mountains.
	mglobal.terrainBlendRange		= 2	   -- range to smooth terrain (desert surrounded by plains turns to plains, etc)
	mglobal.terrainBlendRandom		= 0.4  -- random modifier for terrain smoothing


	-- Features
	mglobal.featurePercent			= 0.30 -- Percent of potential feature tiles that actually create a feature (marsh/jungle/forest)
	mglobal.featureWetVariance		= 0.15 -- Percent chance increase if freshwater, decrease if dry (groups features near rivers)
	mglobal.islePercent				= 0.20 -- Percent of coast tiles with an isle
	mglobal.atollNorthLatitudeLimit = 40
	mglobal.atollSouthLatitudeLimit	= 15
	mglobal.atollMinDeepWaterNeighbors	= 3	-- Minimum of adjacent ocean tiles for an atoll.
	mglobal.numNaturalWonders		= 2 * (Map.GetWorldSize() + 1)


	-- Rain
	mglobal.marshPercent			= 0.02 -- Percent chance increase for marsh from each nearby watery tile
										   --    junglePercent to 1 : marsh
	mglobal.junglePercent			= 0.75 --    junglePercent to 1 : jungle
	mglobal.zeroTreesPercent		= 0.15 -- zeroTreesPercent to 1 : forest

										   -- 	             plainsPercent to 1 : heavy rain
	mglobal.plainsPercent			= 0.70 -- 	 desertPercent to plainsPercent : moderate rain
	mglobal.desertPercent			= 0.15 --				 0 to desertPercent : arid



	-- Temperature terrain
	mglobal.desertMinTemperature	= 0.60 -- hot desert, grass:			desertMinTemperature 	to 1
	mglobal.jungleMinTemperature	= 0.45 -- grass, desert:  				jungleMinTemperature	to desertMinTemperature
	mglobal.tundraTemperature		= 0.30 -- grass, plains, cold desert:   tundraTemperature 		to jungleMinTemperature
	mglobal.snowTemperature			= 0.25 -- grass, plains, tundra:   		snowTemperature			to tundraTemperature
										   -- snow, tundra:										0	to snowTemperature
	-- Temperature features
	mglobal.treesMinTemperature		= 0.15	-- trees:	treesMinTemperature to 1
	mglobal.forestRandomPercent		= 0.07 -- Percent of barren flatland which randomly gets a forest
	mglobal.forestTundraPercent		= 0.33 -- Percent of barren tundra   which randomly gets a forest



	-- Water
	mglobal.riverPercent			= 0.18	-- Percent of river junctions that are large enough to become rivers.
	mglobal.riverRainCheatFactor	= 3.00	-- Values greater than one favor watershed size. Values less than one favor actual rain amount.
	mglobal.minWaterTemp			= 0.10	-- Sets water temperature compression that creates the land/sea seasonal temperature differences that cause monsoon winds.
	mglobal.maxWaterTemp			= 0.50
	mglobal.geostrophicFactor		= 5.0	-- Strength of latitude climate versus monsoon climate.
	mglobal.geostrophicLateralWindStrength = 0.3
	mglobal.lakeSize				= 10	-- read-only; cannot change lake sizes with a map script
	mglobal.oceanRiftWidth			= 2 + math.floor(mapW/60) -- minimum number of ocean tiles in a rift
											-- percent of map width:
	mglobal.atlanticSize			= 0.09	-- size near poles
	mglobal.atlanticBulge			= -0.07	-- size increase at equator
	mglobal.pacificSize				= 0.02	-- size near poles
	mglobal.pacificBulge			= 0.10	-- size increase at equator
											-- percent of straightness
	mglobal.atlanticCurve			= 0.30	-- deviance from the starting point
	mglobal.pacificCurve			= 0.40	--

	mglobal.atlanticSize			= math.max(2, math.min(mapW / 2, Round(mglobal.atlanticSize * mapW)))
	mglobal.pacificSize				= math.max(3, math.min(mapW / 2, Round(mglobal.pacificSize * mapW)))
	mglobal.atlanticBulge			= Round(mglobal.atlanticBulge * mapW)
	mglobal.pacificBulge			= Round(mglobal.pacificBulge * mapW)



	-- Resources
	mglobal.fishTargetFertility		= 60	-- fish appear to create this average city fertility


	-- Quality vs Performance
	-- Lowering these reduces map quality and creation time.
	-- Try reducing these slightly if you experience crashes on huge maps
	mglobal.tempBlendMaxRange		= Round(mapW/8) -- range to smooth temperature map
	mglobal.elevationBlendRange		= Round(mapW/8) -- range to smooth elevation map


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

	do

	overrideAssignStartingPlots = (Map.GetCustomOption(16) == 2)

	local oWorldAge = Map.GetCustomOption(1)
	if oWorldAge == 1 then
		print("Map Age: New")
		mglobal.belowMountainPercent	= modifyOdds(mglobal.belowMountainPercent,	1 / 1.1)
		mglobal.flatPercent				= modifyOdds(mglobal.flatPercent,			1 / 1.2)
	elseif oWorldAge == 3 then
		print("Map Age: Old")
		mglobal.belowMountainPercent	= modifyOdds(mglobal.belowMountainPercent,	1.1)
		mglobal.flatPercent				= modifyOdds(mglobal.flatPercent,			1.2)
	elseif oWorldAge == 4 then
		print("Map Age: Random")
		local worldAgeRand = PWRand()
		local higher = (1 == Map.Rand(2, "modifyOddsRandom for world age"))
		mglobal.belowMountainPercent	= modifyOddsRandom(mglobal.belowMountainPercent,	0.1, worldAgeRand, higher)
		mglobal.flatPercent				= modifyOddsRandom(mglobal.flatPercent,				0.2, worldAgeRand, higher)
		print("worldAgeRand: ", worldAgeRand)
		print("higher: ", higher)
	else
		print("Map Age: Normal")
	end
	mglobal.mountainScatter = mglobal.mountainScatter * 0.00001


	local oTemp = Map.GetCustomOption(2)
	if oTemp == 1 then
		print("Map Temp: Cool")
		mglobal.tropicLatitudes			= modifyOdds(mglobal.tropicLatitudes	/ mglobal.topLatitude, 2/3) * mglobal.topLatitude
		mglobal.horseLatitudes			= modifyOdds(mglobal.horseLatitudes		/ mglobal.topLatitude, 2/3) * mglobal.topLatitude
		mglobal.iceLatitude				= modifyOdds(mglobal.iceLatitude		/ mglobal.topLatitude, 2/3) * mglobal.topLatitude
		mglobal.polarFrontLatitude		= modifyOdds(mglobal.polarFrontLatitude	/ mglobal.topLatitude, 2/3) * mglobal.topLatitude
		mglobal.tundraTemperature		= modifyOdds(mglobal.tundraTemperature,	1.25)
		mglobal.snowTemperature			= modifyOdds(mglobal.snowTemperature,	1.25)
	elseif oTemp == 3 then
		print("Map Temp: Hot")
		mglobal.tropicLatitudes			= modifyOdds(mglobal.tropicLatitudes	/ mglobal.topLatitude, 1.5) * mglobal.topLatitude
		mglobal.horseLatitudes			= modifyOdds(mglobal.horseLatitudes		/ mglobal.topLatitude, 1.5) * mglobal.topLatitude
		mglobal.iceLatitude				= modifyOdds(mglobal.iceLatitude		/ mglobal.topLatitude, 1.5) * mglobal.topLatitude
		mglobal.polarFrontLatitude		= modifyOdds(mglobal.polarFrontLatitude	/ mglobal.topLatitude, 1.5) * mglobal.topLatitude
		mglobal.tundraTemperature		= modifyOdds(mglobal.tundraTemperature,	0.8)
		mglobal.snowTemperature			= modifyOdds(mglobal.snowTemperature,	0.8)
	elseif oTemp == 4 then
		print("Map Temp: Random")
		local tempRand = PWRand()
		local higher = (1 == Map.Rand(2, "modifyOddsRandom for temperature"))
		mglobal.tropicLatitudes			= modifyOddsRandom(mglobal.tropicLatitudes		/ mglobal.topLatitude, 0.5, tempRand, higher) * mglobal.topLatitude
		mglobal.horseLatitudes			= modifyOddsRandom(mglobal.horseLatitudes		/ mglobal.topLatitude, 0.5, tempRand, higher) * mglobal.topLatitude
		mglobal.iceLatitude				= modifyOddsRandom(mglobal.iceLatitude			/ mglobal.topLatitude, 0.5, tempRand, higher) * mglobal.topLatitude
		mglobal.polarFrontLatitude		= modifyOddsRandom(mglobal.polarFrontLatitude	/ mglobal.topLatitude, 0.5, tempRand, higher) * mglobal.topLatitude
		mglobal.tundraTemperature		= modifyOddsRandom(mglobal.tundraTemperature,	0.25, tempRand, higher)
		mglobal.snowTemperature			= modifyOddsRandom(mglobal.snowTemperature,		0.25, tempRand, higher)
		print("tempRand: ", tempRand)
		print("higher: ", higher)
	else
		print("Map Temp: Normal")
	end


	local oRainfall = Map.GetCustomOption(3)
	if oRainfall == 1 then
		print("Map Rain: Arid")
		mglobal.riverPercent		= modifyOdds(mglobal.riverPercent,		2/3)
		mglobal.featurePercent		= modifyOdds(mglobal.featurePercent,	2/3)
		mglobal.marshPercent		= modifyOdds(mglobal.marshPercent,		2/3)
		mglobal.junglePercent		= modifyOdds(mglobal.junglePercent,		1.5)
		mglobal.zeroTreesPercent	= modifyOdds(mglobal.zeroTreesPercent,	1.5)
		mglobal.plainsPercent		= modifyOdds(mglobal.plainsPercent,		1.25)
		mglobal.desertPercent		= modifyOdds(mglobal.desertPercent,		1.25)
	elseif oRainfall == 3 then
		print("Map Rain: Wet")
		mglobal.riverPercent		= modifyOdds(mglobal.riverPercent,		1.5)
		mglobal.featurePercent		= modifyOdds(mglobal.featurePercent,	1.5)
		mglobal.marshPercent		= modifyOdds(mglobal.marshPercent,		1.5)
		mglobal.junglePercent		= modifyOdds(mglobal.junglePercent,		2/3)
		mglobal.zeroTreesPercent	= modifyOdds(mglobal.zeroTreesPercent,	2/3)
		mglobal.plainsPercent		= modifyOdds(mglobal.plainsPercent,		0.8)
		mglobal.desertPercent		= modifyOdds(mglobal.desertPercent,		0.8)
	elseif oRainfall == 4 then
		print("Map Rain: Random")
		local rainfallRand = PWRand()
		local higher = (1 == Map.Rand(2, "modifyOddsRandom for rainfall"))
		mglobal.riverPercent		= modifyOddsRandom(mglobal.riverPercent,		0.5, rainfallRand, higher)
		mglobal.featurePercent		= modifyOddsRandom(mglobal.featurePercent,		0.5, rainfallRand, higher)
		mglobal.marshPercent		= modifyOddsRandom(mglobal.marshPercent,		0.5, rainfallRand, higher)
		mglobal.junglePercent		= modifyOddsRandom(mglobal.junglePercent,		0.5, rainfallRand, not higher)
		mglobal.zeroTreesPercent	= modifyOddsRandom(mglobal.zeroTreesPercent,	0.5, rainfallRand, not higher)
		mglobal.plainsPercent		= modifyOddsRandom(mglobal.plainsPercent,		0.25, rainfallRand, not higher)
		mglobal.desertPercent		= modifyOddsRandom(mglobal.desertPercent,		0.25, rainfallRand, not higher)
		print("rainfallRand: ", rainfallRand)
		print("higher: ", higher)
	else
		print("Map Rain: Normal")
	end


	local oSeaLevel = Map.GetCustomOption(4)
	if oSeaLevel == 1 then
		print("Map Seas: Low")
		mglobal.landPercent = mglobal.landPercent * 1.25
	elseif oSeaLevel == 3 then
		print("Map Seas: High")
		mglobal.landPercent = mglobal.landPercent / 1.25
	elseif oSeaLevel == 4 then
		print("Map Seas: Random")
		local seaLevelRand = 1 + PWRand() * 0.25
		local higher = (1 == Map.Rand(2, "sea level"))
		if higher then
			mglobal.landPercent = mglobal.landPercent * seaLevelRand
		else
			mglobal.landPercent = mglobal.landPercent / seaLevelRand
		end
	else
		print("Map Seas: Normal")
	end

	print("Land Percent: ", mglobal.landPercent)


	local oStarts = Map.GetCustomOption(5)
	if oStarts == 2 then
		print("Map Starts: Everywhere")
		mglobal.offsetAtlanticPercent	= 0.48	-- Percent of land to divide at the Atlantic Ocean (50% is usually halfway on the map)
		mglobal.offshoreCS				= 0.25	-- Percent of city states on uninhabited islands
	else
		print("Map Starts: Largest Continent")
		mglobal.offsetAtlanticPercent	= 0.35	-- Percent of land to divide at the Atlantic Ocean
		mglobal.percentLargestContinent	= 0.46	-- Eurasia must be this percent of total land (ensures citystates can appear there)
		mglobal.terraConnectWeight		= 10	-- if Eurasia is too small, connect sub-continents with this (size/distance) from Eurasia
		mglobal.offshoreCS				= 0.75	-- Percent of city states on uninhabited islands
		mglobal.numNaturalWonders		= Round (1.25 * mglobal.numNaturalWonders) -- extra wonders for larger map sizes
	end


	local oRiftWidth = Map.GetCustomOption(7)
	if oRiftWidth == 4 then oRiftWidth = 1 + Map.Rand(3, "Random Rift Width") end
	if oRiftWidth == 1 then
		print("Map Ocean Width: Narrow")
		mglobal.oceanRiftWidth = 1
		mglobal.landPercent = mglobal.landPercent - 0.02
	elseif oRiftWidth == 3 then
		print("Map Ocean Width: Wide")
		mglobal.oceanRiftWidth = 1 + mglobal.oceanRiftWidth
		mglobal.landPercent = mglobal.landPercent + 0.05
	end

	local oLandScatter = Map.GetCustomOption(10)
	local scatterOdds = 1
	if oLandScatter == 1 then
		print("Map Land Scatter: Blocky")
		scatterOdds = 1/3
	elseif oLandScatter == 3 then
		print("Map Land Scatter: Stringy")
		scatterOdds = 5
	elseif oLandScatter == 4 then
		print("Map Land Scatter: Extremely Stringy")
		scatterOdds = 15
	elseif oLandScatter == 5 then
		print("Map Land Scatter: Random")
		local scatterRand = PWRand()
		local higher = (1 == Map.Rand(2, "modifyOddsRandom for scatter"))
		mglobal.landMinScatter = modifyOddsRandom(mglobal.landMinScatter, 10, scatterRand, higher)
		mglobal.landMaxScatter = modifyOddsRandom(mglobal.landMaxScatter, 10, scatterRand, higher)
		mglobal.coastScatter = modifyOddsRandom(mglobal.coastScatter, 10, scatterRand, higher)
	else
		print("Map Land Scatter: Normal")
	end

	if oLandScatter ~= 5 and oLandScatter ~= 2 then
		mglobal.landMinScatter = modifyOdds(mglobal.landMinScatter, scatterOdds)
		mglobal.landMaxScatter = modifyOdds(mglobal.landMaxScatter, scatterOdds)
		mglobal.coastScatter = modifyOdds(mglobal.coastScatter, scatterOdds)
	end

	print("Land Min Scatter: ", mglobal.landMinScatter)
	print("Land Max Scatter: ", mglobal.landMaxScatter)
	print("CoastScatter: ", mglobal.coastScatter)

	-- Ocean rift sizes

	mglobal.oceanRiftWidth	= Round(mglobal.oceanRiftWidth)

	end










	--
	-- Other settings
	--

	do
	--These attenuation factors lower the altitude of the map edges. This is
	--currently used to prevent large continents in the uninhabitable polar
	--regions. East/west attenuation is set to zero, but modded maps may
	--have need for them.
	mglobal.northAttenuationFactor	= 0.00
	mglobal.northAttenuationRange	= 0.00 --percent of the map height.
	mglobal.southAttenuationFactor	= 0.00
	mglobal.southAttenuationRange	= 0.00

	--east west attenuation may be desired for flat maps.
	mglobal.eastAttenuationFactor	= 0.0
	mglobal.eastAttenuationRange	= 0.0 --percent of the map width.
	mglobal.westAttenuationFactor	= 0.0
	mglobal.westAttenuationRange	= 0.0

	-- Rain tweaking variables. I wouldn't touch these.
	mglobal.pressureNorm		= 1.0 --[1.0 = no normalization] Helps to prevent exaggerated Jungle/Marsh banding on the equator. -Bobert13
	mglobal.minimumRainCost		= 0.0001
	mglobal.upLiftExponent		= 4
	mglobal.polarRainBoost		= 0.00

	--North and south isle latitude limits.
	mglobal.islesNearIce = false
	mglobal.isleNorthLatitudeLimit = 65
	mglobal.isleSouthLatitudeLimit = -65
	mglobal.isleMinDeepWaterNeighbors = 0

	end













	-----------------------------------------------------------------------
	--Below is map data that should not be altered.

	do
	mglobal.MountainPasses		= {}
	mglobal.tropicalPlots		= {}
	mglobal.oceanRiftPlots		= {}
	mglobal.islandAreaBuffed	= {}
	mglobal.lakePlots			= {}
	mglobal.seaPlots			= {}
	mglobal.elevationRect		= {}
	mglobal.oceanRiftMidlines	= {}

	-- Directions
	mglobal.C  = DirectionTypes.NO_DIRECTION
	mglobal.NE = DirectionTypes.DIRECTION_NORTHEAST
	mglobal.E  = DirectionTypes.DIRECTION_EAST
	mglobal.SE = DirectionTypes.DIRECTION_SOUTHEAST
	mglobal.SW = DirectionTypes.DIRECTION_SOUTHWEST
	mglobal.W  = DirectionTypes.DIRECTION_WEST
	mglobal.NW = DirectionTypes.DIRECTION_NORTHWEST
	mglobal.N  = DirectionTypes.DIRECTION_NORTHWEST + 1
	mglobal.S  = DirectionTypes.DIRECTION_NORTHWEST + 2

	mglobal.edgeDirections = {
		mglobal.NE,
		mglobal.E,
		mglobal.SE,
		mglobal.SW,
		mglobal.W,
		mglobal.NW
	}

	mglobal.directionNames = {
		[mglobal.C]  = "C"	,
		[mglobal.NE] = "NE"	,
		[mglobal.E]  = "E"	,
		[mglobal.SE] = "SE"	,
		[mglobal.SW] = "SW"	,
		[mglobal.W]  = "W"	,
		[mglobal.NW] = "NW"	,
		[mglobal.N]  = "N"	,
		[mglobal.S]  = "S"
	}

	-- Flow Directions
	mglobal.NOFLOW = 0
	mglobal.WESTFLOW = 1
	mglobal.EASTFLOW = 2
	mglobal.VERTFLOW = 3

	mglobal.flowNONE = FlowDirectionTypes.NO_FLOWDIRECTION
	mglobal.flowN  = FlowDirectionTypes.FLOWDIRECTION_NORTH
	mglobal.flowNE = FlowDirectionTypes.FLOWDIRECTION_NORTHEAST
	mglobal.flowSE = FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST
	mglobal.flowS  = FlowDirectionTypes.FLOWDIRECTION_SOUTH
	mglobal.flowSW = FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST
	mglobal.flowNW = FlowDirectionTypes.FLOWDIRECTION_NORTHWEST

	mglobal.flowNames = {
		[mglobal.flowNONE] = "NONE",
		[mglobal.flowN]  = "N"	,
		[mglobal.flowNE] = "NE"	,
		[mglobal.flowSE] = "SE"	,
		[mglobal.flowS]  = "S"	,
		[mglobal.flowSW] = "SW"	,
		[mglobal.flowNW] = "NW"
	}

	-- basic tile yields
	mglobal.basicYields = {
		YieldTypes.YIELD_FOOD,
		YieldTypes.YIELD_PRODUCTION,
		YieldTypes.YIELD_GOLD,
		YieldTypes.YIELD_SCIENCE,
		YieldTypes.YIELD_CULTURE,
		YieldTypes.YIELD_FAITH
	}

	--wind zones
	mglobal.NOZONE = -1
	mglobal.NPOLAR = 0
	mglobal.NTEMPERATE = 1
	mglobal.NEQUATOR = 2
	mglobal.SEQUATOR = 3
	mglobal.STEMPERATE = 4
	mglobal.SPOLAR = 5

	--Hex maps are shorter in the y direction than they are
	--wide per unit by this much. We need to know this to sample the perlin
	--maps properly so they don't look squished.
	mglobal.YtoXRatio = 1.5/(math.sqrt(0.75) * 2)


	-- Array of route types - you can change the text, but NOT the order
	mglobal.routes = {"Land", "Road", "Railroad", "Coastal", "Ocean", "Submarine"}

	-- Array of highlight colours
	mglobal.highlights = {  Red     = Vector4(1.0, 0.0, 0.0, 1.0),
						   Green   = Vector4(0.0, 1.0, 0.0, 1.0),
						   Blue    = Vector4(0.0, 0.0, 1.0, 1.0),
						   Cyan    = Vector4(0.0, 1.0, 1.0, 1.0),
						   Yellow  = Vector4(1.0, 1.0, 0.0 ,1.0),
						   Magenta = Vector4(1.0, 0.0, 1.0, 1.0),
						   Black   = Vector4(0.5, 0.5, 0.5, 1.0)}

	mglobal.lastRouteLength = 0
	mglobal.pathDirections  = {DirectionTypes.DIRECTION_NORTHEAST, DirectionTypes.DIRECTION_EAST, DirectionTypes.DIRECTION_SOUTHEAST,
							  DirectionTypes.DIRECTION_SOUTHWEST, DirectionTypes.DIRECTION_WEST, DirectionTypes.DIRECTION_NORTHWEST}

	end

	--
	-- Override AssignStartingPlots functions
	--

	if overrideAssignStartingPlots then
		print("Override AssignStartingPlots: True");
		------------------------------------------------------------------------------
		function AssignStartingPlots:__CustomInit()
			-- This function included to provide a quick and easy override for changing
			-- any initial settings. Add your customized version to the map script.

			if not debugPrint then
				print = function() end
			end

			self.BuffIslands = AssignStartingPlots.BuffIslands
			self.islandAreaBuffed = {}
		end
		------------------------------------------------------------------------------
		function AssignStartingPlots:MeasureStartPlacementFertilityOfPlot(x, y, checkForCoastalLand)
			return Plot_GetFertility(Map.GetPlot(x, y))
		end
		------------------------------------------------------------------------------
		function AssignStartingPlots:PlaceNaturalWonders()
			-- Determine how many NWs to attempt to place. Target is regulated per map size.
			-- The final number cannot exceed the number the map has locations to support.
			local target_number = mg.numNaturalWonders;
			local NW_eligibility_order = self:GenerateNaturalWondersCandidatePlotLists(target_number)
			local iNumNWCandidates = table.maxn(NW_eligibility_order);
			if iNumNWCandidates == 0 then
				print("No Natural Wonders placed, no eligible sites found for any of them.");
				return
			end

			-- Debug printout
			print("-"); print("--- Readout of wonderID Assignment Priority ---");
			for loop, wonderID in ipairs(NW_eligibility_order) do
				print("wonderID Assignment Priority#", loop, "goes to wonderID ", self.wonder_list[wonderID]);
			end
			print("-"); print("-");

			local iNumNWtoPlace = math.min(target_number, iNumNWCandidates);
			local selected_NWs, fallback_NWs = {}, {};
			for loop, wonderID in ipairs(NW_eligibility_order) do
				if loop <= iNumNWtoPlace then
					table.insert(selected_NWs, wonderID);
				else
					table.insert(fallback_NWs, wonderID);
				end
			end

			-- Place the NWs
			local iNumPlaced = 0;
			for loop, nw_number in ipairs(selected_NWs) do
				local nw_type = self.wonder_list[nw_number];
				-- Obtain the correct Row number from the xml Placement table.
				local row_number;
				for row in GameInfo.Natural_Wonder_Placement() do
					if row.NaturalWonderType == nw_type then
						row_number = row.ID;
					end
				end
				-- Place the wonder, using the correct row data from XML.
				local bSuccess = self:AttemptToPlaceNaturalWonder(nw_number, row_number)
				if bSuccess then
					iNumPlaced = iNumPlaced + 1;
				end
			end
			if iNumPlaced < iNumNWtoPlace then
				for loop, nw_number in ipairs(fallback_NWs) do
					if iNumPlaced >= iNumNWtoPlace then
						break
					end
					local nw_type = self.wonder_list[nw_number];
					-- Obtain the correct Row number from the xml Placement table.
					local row_number;
					for row in GameInfo.Natural_Wonder_Placement() do
						if row.NaturalWonderType == nw_type then
							row_number = row.ID;
						end
					end
					-- Place the wonder, using the correct row data from XML.
					local bSuccess = self:AttemptToPlaceNaturalWonder(nw_number, row_number)
					if bSuccess then
						iNumPlaced = iNumPlaced + 1;
					end
				end
			end

			if iNumPlaced >= iNumNWtoPlace then
				print("-- Placed all Natural Wonders --"); print("-"); print("-");
			else
				print("-- Not all Natural Wonders targeted got placed --"); print("-"); print("-");
			end

		end
		------------------------------------------------------------------------------
		function AssignStartingPlots:AssignCityStatesToRegionsOrToUninhabited(args)
			-- Placement methods include:
			-- 1. Assign n Per Region (Continents only)
			-- 2. Assign to uninhabited landmasses
			-- 3. Assign to regions with shared luxury IDs
			-- 4. Assign to low fertility regions

			-- Determine number to assign Per Region
			local iW, iH = Map.GetGridSize()
			local ratio = self.iNumCityStates / self.iNumCivs;
			if ratio > 14 then -- This is a ridiculous number of city states for a game with two civs, but we'll account for it anyway.
				self.iNumCityStatesPerRegion = 10;
			elseif ratio > 11 then -- This is a ridiculous number of cs for two or three civs.
				self.iNumCityStatesPerRegion = 8;
			elseif ratio > 8 then
				self.iNumCityStatesPerRegion = 6;
			elseif ratio > 5.7 then
				self.iNumCityStatesPerRegion = 4;
			elseif ratio > 4.35 then
				self.iNumCityStatesPerRegion = 3;
			elseif ratio > 2.7 then
				self.iNumCityStatesPerRegion = 2;
			elseif ratio > 1.35 then
				self.iNumCityStatesPerRegion = 1;
			else
				self.iNumCityStatesPerRegion = 0;
			end

			local current_cs_index = 1;
			if self.iNumCityStatesPerRegion > 0 and self.method == 1 then
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

			-- Possibility of plots that do not belong to any civ's Region. Evaluate these plots and assign an appropriate number of City States to them.
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
						-- AreaID-based method must be applied, which cannot all be done in this loop
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
			-- Complete the AreaID-based method.
			-- Obtain counts of inhabited and uninhabited plots. Identify areas too small to use for City States.
			for areaID, plot_count in pairs(land_area_plot_count) do
				if TestMembership(areas_inhabited_by_civs, areaID) == true then
					iNumCivLandmassPlots = iNumCivLandmassPlots + plot_count;
				else
					iNumUninhabitedLandmassPlots = iNumUninhabitedLandmassPlots + plot_count;
					-- allow CS on smaller islands than base VP (4 -> 2)
					if plot_count < 2 then
						table.insert(areas_too_small, areaID);
					else
						table.insert(areas_uninhabited, areaID);
					end
				end
			end
			-- Now loop through all Uninhabited Areas and append their plots to the candidates tables.
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
			-- Better numbers for Communitu_79a
			local uninhabited_ratio = iNumUninhabitedLandmassPlots / (iNumCivLandmassPlots + iNumUninhabitedLandmassPlots);
			local max_by_ratio = math.floor(5 * uninhabited_ratio * self.iNumCityStates);
			local max_by_method = math.ceil(mg.offshoreCS * self.iNumCityStates);
			self.iNumCityStatesUninhabited = math.min(self.iNumCityStatesUnassigned, max_by_ratio, max_by_method);
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

			-- Debug check
			if self.iNumCityStatesUnassigned ~= 0 then
				print("Wrong number of City States assigned at end of assignment process. This number unassigned: ", self.iNumCityStatesUnassigned);
			else
				print("All city states assigned.");
			end
		end
		------------------------------------------------------------------------------
		function AssignStartingPlots:BuffIslands()
			print("Buffing Tiny Islands")
			local biggestAreaSize = Map.FindBiggestArea(false):GetNumTiles()
			if biggestAreaSize < 20 then
				-- Skip on archipalego maps
				return
			end

			-- MOD.HungryForFood: Start
			if IsEvenMoreResourcesActive() == true then
				resWeights = {
					[self.stone_ID]		= 1,
					[self.coal_ID]		= 8,
					[self.oil_ID]		= 6,
					[self.aluminum_ID]	= 4,
					[self.uranium_ID]	= 4,
					[self.sulfur_ID]	= 2,
					[self.titanium_ID]	= 2,
					[self.lead_ID]		= 2
				}
			else
				resWeights = {
					[self.stone_ID]		= 1,
					[self.coal_ID]		= 5,
					[self.oil_ID]		= 3,
					[self.aluminum_ID]	= 2,
					[self.uranium_ID]	= 2
				}
			end
			-- MOD.HungryForFood: End

			for plotID, plot in Plots(Shuffle) do
				local plotType		= plot:GetPlotType()
				local terrainType	= plot:GetTerrainType()
				local featureType	= plot:GetFeatureType()
				local area			= plot:Area()
				local areaSize		= area:GetNumTiles()
				if ((plotType == PlotTypes.PLOT_HILLS or plotType == PlotTypes.PLOT_LAND )
						and plot:GetResourceType(-1) == -1
						and IsBetween(1, areaSize, 0.05 * biggestAreaSize)
						and not self.islandAreaBuffed[area:GetID()]
						)then
					local resID  = GetRandomWeighted(resWeights)
					local resNum = 1
					if resID ~= self.stone_ID then
						resNum = resNum + Map.Rand(2, "BuffIslands Random Resource Quantity - Lua")
						if resID ~= self.uranium_ID then
							resNum = resNum + 1
						end
					end
					if resNum > 0 then
						self.islandAreaBuffed[area:GetID()] = true
						if 25 >= Map.Rand(100, "BuffIslands Chance - Lua") then
							-- adjust terrain/feature for resource about to be placed
							if resID == self.stone_ID then
								if featureType == FeatureTypes.FEATURE_JUNGLE or featureType == FeatureTypes.FEATURE_FOREST then
									plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1)
								end
							elseif resID == self.coal_ID then
								if terrainType == TerrainTypes.TERRAIN_TUNDRA then
									plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true)
								elseif terrainType == TerrainTypes.TERRAIN_DESERT then
									plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true)
								end
							elseif resID == self.oil_ID then
								if plotType == PlotTypes.PLOT_HILLS then
									plot:SetPlotType(PlotTypes.PLOT_LAND, false, true)
								end
								if (terrainType == TerrainTypes.TERRAIN_GRASS or terrainType == TerrainTypes.TERRAIN_PLAINS) and featureType == FeatureTypes.NO_FEATURE then
									for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
										if nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
											nearPlot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true)
										end
									end
									plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE, -1)
								end
							elseif resID == self.aluminum_ID then
								if featureType == FeatureTypes.FEATURE_JUNGLE or featureType == FeatureTypes.FEATURE_FOREST then
									plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1)
								end
								if terrainType == TerrainTypes.TERRAIN_GRASS then
									plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true)
								end
							end
							plot:SetResourceType(resID, resNum)
							self.amounts_of_resources_placed[resID + 1] = self.amounts_of_resources_placed[resID + 1] + resNum
						end
					end
				end
			end
		end
		------------------------------------------------------------------------------
		function AssignStartingPlots:AdjustTiles()
			local iW, iH = Map.GetGridSize();
			for y = 0, iH - 1 do
				for x = 0, iW - 1 do
					
					local plot = Map.GetPlot(x, y);
					local res_ID = plot:GetResourceType(-1);
					local plotType = plot:GetPlotType();
					local terrainType = plot:GetTerrainType();
					local featureType = plot:GetFeatureType();
					
					-- Mined/Quarried Resources
					if res_ID == self.marble_ID or 
					res_ID == self.gold_ID or 
					res_ID == self.silver_ID or 
					res_ID == self.copper_ID or 
					res_ID == self.gems_ID or 
					res_ID == self.salt_ID or 
					res_ID == self.lapis_ID or 
					res_ID == self.jade_ID or 
					res_ID == self.amber_ID or
					-- MOD.HungryForFood: Start
					self:IsEvenMoreResourcesActive() == true and
					(
					res_ID == self.obsidian_ID or
					res_ID == self.platinum_ID or
					res_ID == self.tin_ID or
					res_ID == self.lead_ID or
					res_ID == self.sulfur_ID or
					res_ID == self.titanium_ID
					)
					-- MOD.HungryForFood: End
					then 
					
						-- Changed by azum4roll: now follows Civilopedia more closely
						local removeFeature = false;
						if featureType ~= FeatureTypes.FEATURE_FLOOD_PLAINS then
							removeFeature = true;
						end
						-- Gems can be in jungles
						if featureType == FeatureTypes.FEATURE_JUNGLE and res_ID == self.gems_ID then
							removeFeature = false;
						end
						if removeFeature then
							plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1);
						end

						if res_ID == self.silver_ID and terrainType == TerrainTypes.TERRAIN_SNOW then
							-- Turn snow into tundra
							plot:SetTerrainType(TerrainTypes.TERRAIN_TUNDRA, false, true);
						end
						
					-- Tree Resources
					elseif res_ID == self.cocoa_ID or 
						res_ID == self.citrus_ID or 
						res_ID == self.spices_ID or 
						res_ID == self.sugar_ID or 
						res_ID == self.truffles_ID or 
						res_ID == self.silk_ID or 
						res_ID == self.dye_ID or 
						res_ID == self.fur_ID or 
						res_ID == self.deer_ID or
						-- MOD.HungryForFood: Start
						self:IsEvenMoreResourcesActive() == true and
						(
						res_ID == self.hardwood_ID or
						res_ID == self.rubber_ID
						)
						-- MOD.HungryForFood: End
						then
						
						if res_ID == self.fur_ID then
							-- Always want it flat.  The foxes fall into the hills.
							-- They need to be on tundra too.
							plot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
							plot:SetTerrainType(TerrainTypes.TERRAIN_TUNDRA, false, true);
						end

						-- Always want it covered for most tree resources.
						if (featureType == FeatureTypes.FEATURE_MARSH) then
							if res_ID == self.sugar_ID or res_ID == self.truffles_ID then
								-- Keep it marsh for these resources.
							else
								Plot_AddTrees(plot);
							end
						else
							-- Sugar can only be on marsh or flood plains
							if res_ID == self.sugar_ID then
								if featureType ~= FeatureTypes.FEATURE_FLOOD_PLAINS then
									plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true);
									plot:SetFeatureType(FeatureTypes.FEATURE_MARSH, -1);
								end
							else
								Plot_AddTrees(plot);
							end
						end
						
					-- Open Land Resources
					elseif res_ID == self.incense_ID or 
						res_ID == self.ivory_ID or 
						res_ID == self.wine_ID or 
						res_ID == self.olives_ID or 
						res_ID == self.coffee_ID or
						res_ID == self.tobacco_ID or 
						res_ID == self.tea_ID or 
						res_ID == self.perfume_ID or 
						res_ID == self.cotton_ID or 
						-- MOD.HungryForFood: Start
						self:IsEvenMoreResourcesActive() == true and
						(
						res_ID == self.poppy_ID
						)
						-- MOD.HungryForFood: End
						then
						
						if res_ID == self.ivory_ID then
							-- Always want it flat.  Other types are fine on hills.
							plot:SetPlotType(PlotTypes.PLOT_LAND, false, true)
						end
						
						-- Don't remove flood plains if present for the few that are placed on it, only remove other features, like marsh or any trees.				
						if (featureType ~= FeatureTypes.FEATURE_FLOOD_PLAINS) then	
							plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1)
						end
						
						if res_ID == self.incense_ID or res_ID == self.ivory_ID then
							-- Because incense/ivory are very restricted, it was expanded to look for grass tiles as a final fallback.
							-- This will help with certain distributions that incense previously didn't work well in, such as assignments to city-states which could be hit or miss.
							-- Besides jungle placements, this is the only luxury which will change the terrain it's found on.  Plus, plains are mixed in with grass anyway.
							if terrainType == TerrainTypes.TERRAIN_GRASS then
								plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true)
							end
						end
					end
				end
			end
		end
		function Plot_AddTrees(plot)
			-- Force add trees to a specific plot for AdjustTiles
			-- No need to check IsGoodFeaturePlot
			local x, y					= plot:GetX(), plot:GetY()
			local i						= elevationMap:GetIndex(x,y)
			local mapW, mapH			= Map.GetGridSize()
			local plotTerrainID 		= plot:GetTerrainType()
			local zeroTreesThreshold	= rainfallMap:FindThresholdFromPercent(mg.zeroTreesPercent,false,true)
			local jungleThreshold		= rainfallMap:FindThresholdFromPercent(mg.junglePercent,false,true)

			-- Micro-climates for tiny volcanic islands
			if not plot:IsMountain() and (plotTerrainID == TerrainTypes.TERRAIN_PLAINS or plotTerrainID == TerrainTypes.TERRAIN_GRASS) then
				local areaSize = plot:Area():GetNumTiles()
				if areaSize <= 5 and (6 - areaSize) >= Map.Rand(5, "Add Island Features - Lua") then
					local zone = elevationMap:GetZone(y)
					if zone == mg.NEQUATOR or zone == mg.SEQUATOR then
						for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
							if nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
								return
							end
						end
						plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE,-1)
						return
					elseif zone == mg.NTEMPERATE or zone == mg.STEMPERATE then
						plot:SetFeatureType(FeatureTypes.FEATURE_FOREST,-1)
						return
					end
				end
			end

			-- Too dry for jungle
			if rainfallMap.data[i] < jungleThreshold then
				plot:SetFeatureType(FeatureTypes.FEATURE_FOREST,-1)
				return
			end

			-- Too cold for jungle
			if temperatureMap.data[i] < mg.jungleMinTemperature then
				plot:SetFeatureType(FeatureTypes.FEATURE_FOREST,-1)
				return
			end

			-- Too near desert for jungle
			for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
				if nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
					plot:SetFeatureType(FeatureTypes.FEATURE_FOREST,-1)
					return
				end
			end

			plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE,-1)
		end
		------------------------------------------------------------------------------
		function AssignStartingPlots:ProcessResourceList(frequency, impact_table_number, plot_list, resources_to_place)
			-- Added a random factor to strategic resources - Thalassicus
		
			-- This function needs to receive two numbers and two tables.
			-- Length of the plotlist is divided by frequency to get the number of 
			-- resources to place. ... The first table is a list of plot indices.
			-- The second table contains subtables, one per resource type, detailing the
			-- resource ID number, quantity, weighting, and impact radius of each applicable
			-- resource. If radius min and max are different, the radius length is variable
			-- and a die roll will determine a value >= min and <= max.
			--
			-- The system may be easiest to manage if the weightings add up to 100, so they
			-- can be handled as percentages, but this is not required.
			--
			-- Impact #s - 1 strategic - 2 luxury - 3 bonus
			-- Res data  - 1 ID - 2 quantity - 3 weight - 4 radius min - 5 radius max
			--
			-- The plot list will be processed sequentially, so randomize it in advance.
			-- The default lists are terrain-oriented and are randomized during __Init
			if plot_list == nil then
				print("Plot list was nil! -ProcessResourceList");
				return
			end
			local iW, iH = Map.GetGridSize();
			local iNumTotalPlots = table.maxn(plot_list);
			local iNumResourcesToPlace = math.ceil(iNumTotalPlots / frequency);
			local iNumResourcesTypes = table.maxn(resources_to_place);
			local res_ID, res_quantity, res_weight, res_min, res_max, res_range, res_threshold = {}, {}, {}, {}, {}, {}, {};
			local totalWeight, accumulatedWeight = 0, 0;
			for index, resource_data in ipairs(resources_to_place) do
				res_ID[index] = resource_data[1];
				res_quantity[index] = resource_data[2];
				res_weight[index] = resource_data[3];
				totalWeight = totalWeight + resource_data[3];
				res_min[index] = resource_data[4];
				res_max[index] = resource_data[5];
				if res_max[index] > res_min[index] then
					res_range[index] = res_max[index] - res_min[index] + 1;
				else
					res_range[index] = -1;
				end
			end
			for index = 1, iNumResourcesTypes do
				-- We'll roll a die and check each resource in turn to see if it is 
				-- the one to get placed in that particular case. The weightings are 
				-- used to decide how much percentage of the total each represents.
				-- This chunk sets the threshold for each resource in turn.
				local threshold = (res_weight[index] + accumulatedWeight) * 10000 / totalWeight;
				table.insert(res_threshold, threshold);
				accumulatedWeight = accumulatedWeight + res_weight[index];
			end
			-- Main loop
			local current_index = 1;
			local avoid_ripples = true;
			for place_resource = 1, iNumResourcesToPlace do
				local placed_this_res = false;
				local use_this_res_index = 1;
				local diceroll = Map.Rand(10000, "Choose resource type - Distribute Resources - Lua");
				for index, threshold in ipairs(res_threshold) do
					if diceroll < threshold then -- Choose this resource type.
						use_this_res_index = index;
						break
					end
				end
				if avoid_ripples == true then -- Still on first pass through plot_list, seek first eligible 0 value on impact matrix.
					for index_to_check = current_index, iNumTotalPlots do
						if index_to_check == iNumTotalPlots then -- Completed first pass of plot_list, now change to seeking lowest value instead of zero value.
							avoid_ripples = false;
						end
						if placed_this_res == true then
							break
						else
							current_index = current_index + 1;
						end
						local plotIndex = plot_list[index_to_check];
						if self.impactData[impact_table_number][plotIndex] == 0 then
							local x = (plotIndex - 1) % iW;
							local y = (plotIndex - x - 1) / iW;
							local res_plot = Map.GetPlot(x, y);
							if res_plot:GetResourceType(-1) == -1 then -- Placing this strategic resource in this plot.
								local res_addition = 0;
								if res_range[use_this_res_index] ~= -1 then
									res_addition = Map.Rand(res_range[use_this_res_index], "Resource Radius - Place Resource LUA");
								end
								local quantity = res_quantity[use_this_res_index];
								-- added by azum4roll: give some variance to strategic amounts
								if self:IsImpactLayerStrategic(impact_table_number) then
									local rand = Map.Rand(10000, "ProcessResourceList - Lua") / 10000
									if (rand >= 0.75) then
										quantity = quantity * 1.2
									elseif (rand < 0.25) then
										quantity = quantity * 0.8
									end
									quantity = math.floor(quantity + 0.5)
								end
								res_plot:SetResourceType(res_ID[use_this_res_index], quantity);
								if (Game.GetResourceUsageType(res_ID[use_this_res_index]) == ResourceUsageTypes.RESOURCEUSAGE_LUXURY) then
									self.totalLuxPlacedSoFar = self.totalLuxPlacedSoFar + 1;
								end
								self:PlaceResourceImpact(x, y, impact_table_number, res_min[use_this_res_index] + res_addition);
								placed_this_res = true;
								self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] = self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] + quantity;

								-- if stone is added, don't buff the island
								if res_ID[use_this_res_index] == self.stone_ID then
									self.islandAreaBuffed[res_plot:GetArea()] = true
								end
							end
						end
					end
				end
				if avoid_ripples == false then -- Completed first pass through plot_list, so use backup method.
					local lowest_impact = 98;
					local best_plot;
					for loop, plotIndex in ipairs(plot_list) do
						if lowest_impact > self.impactData[impact_table_number][plotIndex] then
							local x = (plotIndex - 1) % iW;
							local y = (plotIndex - x - 1) / iW;
							local res_plot = Map.GetPlot(x, y)
							if res_plot:GetResourceType(-1) == -1 then
								lowest_impact = self.impactData[impact_table_number][plotIndex];
								best_plot = plotIndex;
							end
						end
					end
					if best_plot ~= nil then
						local x = (best_plot - 1) % iW;
						local y = (best_plot - x - 1) / iW;
						local res_plot = Map.GetPlot(x, y)
						local res_addition = 0;
						if res_range[use_this_res_index] ~= -1 then
							res_addition = Map.Rand(res_range[use_this_res_index], "Resource Radius - Place Resource LUA");
						end
						local quantity = res_quantity[use_this_res_index];
						-- added by azum4roll: give some variance to strategic amounts
						if self:IsImpactLayerStrategic(impact_table_number) then
							local rand = Map.Rand(10000, "ProcessResourceList - Lua") / 10000
							if (rand >= 0.75) then
								quantity = quantity * 1.2
							elseif (rand < 0.25) then
								quantity = quantity * 0.8
							end
							quantity = math.floor(quantity + 0.5)
						end
						--print("ProcessResourceList backup, Resource: " .. res_ID[use_this_res_index] .. ", Quantity: " .. quantity);
						res_plot:SetResourceType(res_ID[use_this_res_index], quantity);
						self:PlaceResourceImpact(x, y, impact_table_number, res_min[use_this_res_index] + res_addition);
						self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] = self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] + quantity;

						-- if stone is added, don't buff the island
						if res_ID[use_this_res_index] == self.stone_ID then
							self.islandAreaBuffed[res_plot:GetArea()] = true
						end
					end
				end
			end
		end
		------------------------------------------------------------------------------
		function AssignStartingPlots:PlaceStrategicAndBonusResources()
			-- Keeping this for the map for easier adjustment, despite being ported to VP

			-- KEY: {Resource ID, Quantity (0 = unquantified), weighting, minimum radius, maximum radius}
			-- KEY: (frequency (1 per n plots in the list), impact list number, plot list, resource data)
			--
			-- The radius creates a zone around the plot that other resources of that
			-- type will avoid if possible. See ProcessResourceList for impact numbers.
			--
			-- Order of placement matters, so changing the order may affect a later dependency.
			
			-- Adjust amounts, if applicable, based on strategic deposit size setting.
			local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = self:GetMajorStrategicResourceQuantityValues()
			local resources_to_place = {}

			-- Adjust appearance rate per resource density setting chosen by user.
			local resMultiplier = 1;
			if self.resDensity == 1 then -- Sparse, so increase the number of tiles per strategic.
				resMultiplier = 1.5;
			elseif self.resDensity == 3 then -- Abundant, so reduce the number of tiles per strategic.
				resMultiplier = 0.66666667;
			elseif self.resDensity == 4 then -- Random
				resMultiplier = getRandomMultiplier(0.5);
			end

			-- Place Strategic resources.
			print("Map Generation - Placing Strategics");
			
			-- Revamped by azum4roll: now place most resources one by one for easier balancing
			resources_to_place = {
				{self.horse_ID, horse_amt, 100, 1, 2}
			};
			self:ProcessResourceList(20 * resMultiplier, ImpactLayers.LAYER_HORSE, self.grass_flat_no_feature, resources_to_place);
			self:ProcessResourceList(32 * resMultiplier, ImpactLayers.LAYER_HORSE, self.plains_flat_no_feature, resources_to_place);
			
			resources_to_place = {
				{self.horse_ID, horse_amt * 0.7, 100, 2, 3}
			};
			self:ProcessResourceList(35 * resMultiplier, ImpactLayers.LAYER_HORSE, self.desert_wheat_list, resources_to_place);
			self:ProcessResourceList(60 * resMultiplier, ImpactLayers.LAYER_HORSE, self.tundra_flat_no_feature, resources_to_place);
			
			resources_to_place = {
				{self.iron_ID, iron_amt, 100, 1, 3}
			};
			self:ProcessResourceList(90 * resMultiplier, ImpactLayers.LAYER_IRON, self.hills_open_list, resources_to_place);
			self:ProcessResourceList(110 * resMultiplier, ImpactLayers.LAYER_IRON, self.flat_open_no_tundra_no_desert, resources_to_place);
			self:ProcessResourceList(60 * resMultiplier, ImpactLayers.LAYER_IRON, self.desert_flat_no_feature, resources_to_place);
			self:ProcessResourceList(75 * resMultiplier, ImpactLayers.LAYER_IRON, self.hills_forest_list, resources_to_place);
			self:ProcessResourceList(80 * resMultiplier, ImpactLayers.LAYER_IRON, self.forest_flat_that_are_not_tundra, resources_to_place);
			self:ProcessResourceList(85 * resMultiplier, ImpactLayers.LAYER_IRON, self.tundra_flat_forest, resources_to_place);
			self:ProcessResourceList(90 * resMultiplier, ImpactLayers.LAYER_IRON, self.tundra_flat_no_feature, resources_to_place);
			self:ProcessResourceList(17 * resMultiplier, ImpactLayers.LAYER_IRON, self.snow_flat_list, resources_to_place);
			
			resources_to_place = {
				{self.coal_ID, coal_amt, 100, 1, 2}
			};
			self:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_COAL, self.hills_open_no_tundra_no_desert, resources_to_place);
			self:ProcessResourceList(70 * resMultiplier, ImpactLayers.LAYER_COAL, self.grass_flat_no_feature, resources_to_place);
			self:ProcessResourceList(55 * resMultiplier, ImpactLayers.LAYER_COAL, self.plains_flat_no_feature, resources_to_place);
			
			resources_to_place = {
				{self.oil_ID, oil_amt, 100, 1, 3}
			};
			self:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_OIL, self.desert_flat_no_feature, resources_to_place);
			self:ProcessResourceList(75 * resMultiplier, ImpactLayers.LAYER_OIL, self.tundra_flat_no_feature, resources_to_place);
			self:ProcessResourceList(25 * resMultiplier, ImpactLayers.LAYER_OIL, self.marsh_list, resources_to_place);
			self:ProcessResourceList(100 * resMultiplier, ImpactLayers.LAYER_OIL, self.jungle_flat_list, resources_to_place);
			self:ProcessResourceList(17 * resMultiplier, ImpactLayers.LAYER_OIL, self.snow_flat_list, resources_to_place);
	
			resources_to_place = {
				{self.aluminum_ID, alum_amt, 100, 1, 3}
			};
			self:ProcessResourceList(42 * resMultiplier, ImpactLayers.LAYER_ALUMINUM, self.hills_open_no_grass, resources_to_place);
			self:ProcessResourceList(35 * resMultiplier, ImpactLayers.LAYER_ALUMINUM, self.flat_open_no_grass_no_plains, resources_to_place);
			self:ProcessResourceList(100 * resMultiplier, ImpactLayers.LAYER_ALUMINUM, self.plains_flat_no_feature, resources_to_place);
			
			resources_to_place = {
				{self.uranium_ID, uran_amt, 100, 2, 4}
			};
			self:ProcessResourceList(50 * resMultiplier, ImpactLayers.LAYER_URANIUM, self.hills_jungle_list, resources_to_place);
			self:ProcessResourceList(200 * resMultiplier, ImpactLayers.LAYER_URANIUM, self.hills_open_list, resources_to_place);
			self:ProcessResourceList(90 * resMultiplier, ImpactLayers.LAYER_URANIUM, self.tundra_flat_no_feature, resources_to_place);
			self:ProcessResourceList(150 * resMultiplier, ImpactLayers.LAYER_URANIUM, self.desert_flat_no_feature, resources_to_place);
			self:ProcessResourceList(300 * resMultiplier, ImpactLayers.LAYER_URANIUM, self.flat_open_no_tundra_no_desert, resources_to_place);
			self:ProcessResourceList(300 * resMultiplier, ImpactLayers.LAYER_URANIUM, self.hills_forest_list, resources_to_place);
			self:ProcessResourceList(325 * resMultiplier, ImpactLayers.LAYER_URANIUM, self.forest_flat_that_are_not_tundra, resources_to_place);
			self:ProcessResourceList(350 * resMultiplier, ImpactLayers.LAYER_URANIUM, self.tundra_flat_forest, resources_to_place);
			self:ProcessResourceList(37 * resMultiplier, ImpactLayers.LAYER_URANIUM, self.marsh_list, resources_to_place);
			self:ProcessResourceList(150 * resMultiplier, ImpactLayers.LAYER_URANIUM, self.jungle_flat_list, resources_to_place);
			self:ProcessResourceList(35 * resMultiplier, ImpactLayers.LAYER_URANIUM, self.snow_flat_list, resources_to_place);

			self:AddModernMinorStrategicsToCityStates();
			
			self:PlaceSmallQuantitiesOfStrategics(36 * resMultiplier, self.land_hills_list);
			
			self:PlaceOilInTheSea();

			-- Check for low or missing Strategic resources
			uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = self:GetSmallStrategicResourceQuantityValues();
			while self.amounts_of_resources_placed[self.iron_ID + 1] < 4 * self.iNumCivs do
				print("Map has very low iron, adding another.");
				local resources_to_place = { {self.iron_ID, iron_amt, 20, 1, 2} };
				self:ProcessResourceList(99999, 1, self.desert_flat_no_feature, resources_to_place); -- 99999 means one per that many tiles: a single instance.
				self:ProcessResourceList(99999, 1, self.hills_forest_list, resources_to_place);
				self:ProcessResourceList(99999, 1, self.hills_open_list, resources_to_place);
				self:ProcessResourceList(99999, 1, self.forest_flat_that_are_not_tundra, resources_to_place);
				self:ProcessResourceList(99999, 1, self.tundra_flat_forest, resources_to_place);
			end
			while self.amounts_of_resources_placed[self.horse_ID + 1] < 4 * self.iNumCivs do
				print("Map has very low horse, adding another.");
				local resources_to_place = { {self.horse_ID, horse_amt, 25, 1, 2} };
				self:ProcessResourceList(99999, 1, self.grass_flat_no_feature, resources_to_place);
				self:ProcessResourceList(99999, 1, self.plains_flat_no_feature, resources_to_place);
				self:ProcessResourceList(99999, 1, self.desert_wheat_list, resources_to_place);
				self:ProcessResourceList(99999, 1, self.tundra_flat_no_feature, resources_to_place);
			end
			while self.amounts_of_resources_placed[self.coal_ID + 1] < 4 * self.iNumCivs do
				print("Map has very low coal, adding another.");
				local resources_to_place = { {self.coal_ID, coal_amt, 33, 1, 2} };
				self:ProcessResourceList(99999, 1, self.hills_open_no_tundra_no_desert, resources_to_place);
				self:ProcessResourceList(99999, 1, self.grass_flat_no_feature, resources_to_place);
				self:ProcessResourceList(99999, 1, self.plains_flat_no_feature, resources_to_place);
			end
			while self.amounts_of_resources_placed[self.oil_ID + 1] < 4 * self.iNumCivs do
				print("Map has very low oil, adding another.");
				local resources_to_place = { {self.oil_ID, oil_amt, 33, 1, 2} };
				self:ProcessResourceList(99999, 1, self.desert_flat_no_feature, resources_to_place);
				self:ProcessResourceList(99999, 1, self.tundra_flat_no_feature, resources_to_place);
				self:ProcessResourceList(99999, 1, self.jungle_flat_list, resources_to_place);
			end
			while self.amounts_of_resources_placed[self.aluminum_ID + 1] < 5 * self.iNumCivs do
				print("Map has very low aluminum, adding another.");
				local resources_to_place = { {self.aluminum_ID, alum_amt, 33, 1, 2} };
				self:ProcessResourceList(99999, 1, self.hills_open_no_grass, resources_to_place);
				self:ProcessResourceList(99999, 1, self.flat_open_no_grass_no_plains, resources_to_place);
				self:ProcessResourceList(99999, 1, self.plains_flat_no_feature, resources_to_place);
			end
			while self.amounts_of_resources_placed[self.uranium_ID + 1] < 2 * self.iNumCivs do
				print("Map has very low uranium, adding another.");
				local resources_to_place = { {self.uranium_ID, uran_amt, 100, 2, 4} };
				self:ProcessResourceList(99999, 1, self.land_hills_list, resources_to_place);
			end
			
			self:PlaceBonusResources();
		end
		------------------------------------------------------------------------------
		function AssignStartingPlots:PlaceFish()
			print("AssignStartingPlots:PlaceFish()")
			for plotID, plot in Plots(Shuffle) do
				if PlacePossibleFish(plot) then
					self.amounts_of_resources_placed[self.fish_ID + 1] = self.amounts_of_resources_placed[self.fish_ID + 1] + 1;
				end
			end
		end
		function PlacePossibleFish(plot)
			if plot:GetTerrainType() ~= TerrainTypes.TERRAIN_COAST or plot:IsLake() or plot:GetFeatureType() ~= FeatureTypes.NO_FEATURE or plot:GetResourceType(-1) ~= -1 then
				return
			end
			local x, y			= plot:GetX(), plot:GetY()
			local landDistance	= 999
			local sumFertility	= 0
			local nearFish		= 0
			local odds			= 0
			local fishID		= GameInfo.Resources.RESOURCE_FISH.ID
			local fishMod		= 0
			local oddsMultiplier= 1.0

			for nearPlot, distance in Plot_GetPlotsInCircle(plot, 1, 3) do
				distance = math.max(1, distance)
				if not nearPlot:IsWater() and distance < landDistance then
					landDistance = distance
				end
				sumFertility = sumFertility + Plot_GetFertility(nearPlot, false, true)
				if nearPlot:GetResourceType(-1) == fishID then
					-- odds = odds - 100 / distance
					oddsMultiplier = oddsMultiplier - 0.5 / distance
				end
			end
			if landDistance >= 3 then
				return
			end

			-- fishMod = odds
			odds = math.max((1.2 - sumFertility / mg.fishTargetFertility) / 2, 0.15) -- 15% to 60%
			-- odds = odds / landDistance
			oddsMultiplier = oddsMultiplier / landDistance
			odds = modifyOdds(odds, oddsMultiplier)
			--print("oddsMultiplier:", oddsMultiplier, "odds:", odds)

			if odds >= PWRand() then
				plot:SetResourceType(fishID, 1)
				return true
				--print(string.format( "PlacePossibleFish fertility=%-3s odds=%-3s fishMod=%-3s", Round(sumFertility), Round(odds), Round(fishMod) ))
			end
		end
		------------------------------------------------------------------------------
		function AssignStartingPlots:PlaceBonusResources()
			local resMultiplier = 1;
			if self.bonusDensity == 1 then -- Sparse, so increase the number of tiles per bonus.
				resMultiplier = 1.5;
			elseif self.bonusDensity == 3 then -- Abundant, so reduce the number of tiles per bonus.
				resMultiplier = 0.66666667;
			elseif self.bonusDensity == 4 then -- Random
				resMultiplier = getRandomMultiplier(0.5);
			end
			
			-- Place Bonus Resources
			print("Map Generation - Placing Bonuses");
			
			-- modified by azum4roll: PlaceFish only depends on fishTargetFertility now
			-- self:PlaceFish(8 * resMultiplier, self.coast_list);
			self:PlaceFish()
			self:PlaceSexyBonusAtCivStarts()
			self:AddExtraBonusesToHillsRegions()
			
			local resources_to_place = {}

			if IsEvenMoreResourcesActive() == true then
				resources_to_place = {
				{self.deer_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(12 * resMultiplier, ImpactLayers.LAYER_BONUS, self.extra_deer_list, resources_to_place)
				-- 8
				resources_to_place = {
				{self.deer_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(16 * resMultiplier, ImpactLayers.LAYER_BONUS, self.tundra_flat_no_feature, resources_to_place)
				-- 12
				resources_to_place = {
				{self.deer_ID, 1, 100, 0, 1} };
				self:ProcessResourceList(30 * resMultiplier, ImpactLayers.LAYER_BONUS, self.banana_list, resources_to_place)
				-- none
				resources_to_place = {
				{self.deer_ID, 1, 100, 0, 1} };
				self:ProcessResourceList(30 * resMultiplier, ImpactLayers.LAYER_BONUS, self.hills_jungle_list, resources_to_place)
				-- none
				
				resources_to_place = {
				{self.wheat_ID, 1, 100, 1, 2} };
				self:ProcessResourceList(20 * resMultiplier, ImpactLayers.LAYER_BONUS, self.desert_wheat_list, resources_to_place)
				-- 10
				resources_to_place = {
				{self.wheat_ID, 1, 100, 2, 3} };
				self:ProcessResourceList(44 * resMultiplier, ImpactLayers.LAYER_BONUS, self.plains_flat_no_feature, resources_to_place)
				-- 27
				
				resources_to_place = {
				{self.banana_ID, 1, 100, 0, 1} };
				self:ProcessResourceList(30 * resMultiplier, ImpactLayers.LAYER_BONUS, self.banana_list, resources_to_place)
				-- 14
				
				resources_to_place = {
				{self.banana_ID, 1, 100, 0, 1} };
				self:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_BONUS, self.marsh_list, resources_to_place)
				-- none
				
				resources_to_place = {
				{self.cow_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(30 * resMultiplier, ImpactLayers.LAYER_BONUS, self.grass_flat_no_feature, resources_to_place)
				-- 18
				
			-- CBP
				resources_to_place = {
				{self.bison_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(24 * resMultiplier, ImpactLayers.LAYER_BONUS, self.flat_open_no_tundra_no_desert, resources_to_place)
			-- END

				resources_to_place = {
				{self.sheep_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(44 * resMultiplier, ImpactLayers.LAYER_BONUS, self.hills_open_list, resources_to_place)
				-- 13

				resources_to_place = {
				{self.stone_ID, 1, 100, 1, 1} };
				self:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_BONUS, self.dry_grass_flat_no_feature, resources_to_place)
				-- 20
				
				resources_to_place = {
				{self.stone_ID, 1, 100, 1, 1} };
				self:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_BONUS, self.dry_plains_flat_no_feature, resources_to_place)
				-- none
				
				resources_to_place = {
				{self.stone_ID, 1, 100, 1, 2} };
				self:ProcessResourceList(30 * resMultiplier, ImpactLayers.LAYER_BONUS, self.tundra_flat_no_feature, resources_to_place)
				-- 15
				
				resources_to_place = {
				{self.stone_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(16 * resMultiplier, ImpactLayers.LAYER_BONUS, self.desert_flat_no_feature, resources_to_place)
				-- 19
				
				resources_to_place = {
				{self.stone_ID, 1, 100, 1, 2} };
				self:ProcessResourceList(36 * resMultiplier, ImpactLayers.LAYER_BONUS, self.hills_open_list, resources_to_place)
				-- none
				
				resources_to_place = {
				{self.stone_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(10 * resMultiplier, ImpactLayers.LAYER_BONUS, self.snow_flat_list, resources_to_place)
				-- none
				
				resources_to_place = {
				{self.deer_ID, 1, 100, 3, 4} };
				self:ProcessResourceList(50 * resMultiplier, ImpactLayers.LAYER_BONUS, self.forest_flat_that_are_not_tundra, resources_to_place)
				self:ProcessResourceList(50 * resMultiplier, ImpactLayers.LAYER_BONUS, self.hills_forest_list, resources_to_place)
				-- 25

			-- Even More Resources for VP start
				resources_to_place = {
				{self.rice_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(24 * resMultiplier, ImpactLayers.LAYER_BONUS, self.fresh_water_grass_flat_no_feature, resources_to_place)

				resources_to_place = {
				{self.maize_ID, 1, 100, 1, 2} };
				self:ProcessResourceList(32 * resMultiplier, ImpactLayers.LAYER_BONUS, self.plains_flat_no_feature, resources_to_place)

				resources_to_place = {
				{self.coconut_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(12 * resMultiplier, ImpactLayers.LAYER_BONUS, self.coconut_list, resources_to_place)

				resources_to_place = {
				{self.hardwood_ID, 1, 100, 1, 2} };
				self:ProcessResourceList(37 * resMultiplier, ImpactLayers.LAYER_BONUS, self.hills_covered_list, resources_to_place)

				resources_to_place = {
				{self.hardwood_ID, 1, 100, 1, 2} };
				self:ProcessResourceList(37 * resMultiplier, ImpactLayers.LAYER_BONUS, self.flat_covered, resources_to_place)

				resources_to_place = {
				{self.hardwood_ID, 1, 100, 1, 2} };
				self:ProcessResourceList(29 * resMultiplier, ImpactLayers.LAYER_BONUS, self.tundra_flat_forest, resources_to_place)

				resources_to_place = {
				{self.lead_ID, 1, 100, 1, 3} };
				self:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_BONUS, self.dry_grass_flat_no_feature, resources_to_place)

				resources_to_place = {
				{self.lead_ID, 1, 100, 2, 3} };
				self:ProcessResourceList(35 * resMultiplier, ImpactLayers.LAYER_BONUS, self.hills_open_list, resources_to_place)

				resources_to_place = {
				{self.lead_ID, 1, 100, 1, 2} };
				self:ProcessResourceList(35 * resMultiplier, ImpactLayers.LAYER_BONUS, self.desert_flat_no_feature, resources_to_place)

				resources_to_place = {
				{self.pineapple_ID, 1, 100, 0, 3} };
				self:ProcessResourceList(29 * resMultiplier, ImpactLayers.LAYER_BONUS, self.banana_list, resources_to_place)

				resources_to_place = {
				{self.potato_ID, 1, 100, 2, 3} };
				self:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_BONUS, self.flat_open_no_tundra_no_desert, resources_to_place)

				resources_to_place = {
				{self.potato_ID, 1, 100, 0, 3} };
				self:ProcessResourceList(29 * resMultiplier, ImpactLayers.LAYER_BONUS, self.hills_open_no_tundra_no_desert, resources_to_place)

				resources_to_place = {
				{self.rubber_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(43 * resMultiplier, ImpactLayers.LAYER_BONUS, self.banana_list, resources_to_place)

				resources_to_place = {
				{self.sulfur_ID, 1, 100, 1, 3} };
				self:ProcessResourceList(29 * resMultiplier, ImpactLayers.LAYER_BONUS, self.hills_open_list, resources_to_place)

				resources_to_place = {
				{self.sulfur_ID, 1, 100, 1, 3} };
				self:ProcessResourceList(37 * resMultiplier, ImpactLayers.LAYER_BONUS, self.hills_covered_list, resources_to_place)

				resources_to_place = {
				{self.sulfur_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(21 * resMultiplier, ImpactLayers.LAYER_BONUS, self.snow_flat_list, resources_to_place)

				resources_to_place = {
				{self.sulfur_ID, 1, 100, 1, 3} };
				self:ProcessResourceList(43 * resMultiplier, ImpactLayers.LAYER_BONUS, self.flat_open, resources_to_place)

				resources_to_place = {
				{self.titanium_ID, 1, 100,0, 2} };
				self:ProcessResourceList(56 * resMultiplier, ImpactLayers.LAYER_BONUS, self.flat_open, resources_to_place)

				resources_to_place = {
				{self.titanium_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(51 * resMultiplier, ImpactLayers.LAYER_BONUS, self.hills_open_list, resources_to_place)

				resources_to_place = {
				{self.titanium_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(48 * resMultiplier, ImpactLayers.LAYER_BONUS, self.desert_flat_no_feature, resources_to_place)

				resources_to_place = {
				{self.titanium_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(40 * resMultiplier, ImpactLayers.LAYER_BONUS, self.tundra_flat_including_forests, resources_to_place)

				resources_to_place = {
				{self.titanium_ID, 1, 100, 0, 1} };
				self:ProcessResourceList(24 * resMultiplier, ImpactLayers.LAYER_BONUS, self.snow_flat_list, resources_to_place)
			-- Even More Resources for VP end
			else
				resources_to_place = {
				{self.deer_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(6 * resMultiplier, ImpactLayers.LAYER_BONUS, self.extra_deer_list, resources_to_place)
				-- 8
				resources_to_place = {
				{self.deer_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(8 * resMultiplier, ImpactLayers.LAYER_BONUS, self.tundra_flat_no_feature, resources_to_place)
				-- 12
				resources_to_place = {
				{self.deer_ID, 1, 100, 0, 1} };
				self:ProcessResourceList(12 * resMultiplier, ImpactLayers.LAYER_BONUS, self.banana_list, resources_to_place)
				-- none
				resources_to_place = {
				{self.deer_ID, 1, 100, 0, 1} };
				self:ProcessResourceList(12 * resMultiplier, ImpactLayers.LAYER_BONUS, self.hills_jungle_list, resources_to_place)
				-- none
				
				resources_to_place = {
				{self.wheat_ID, 1, 100, 1, 2} };
				self:ProcessResourceList(20 * resMultiplier, ImpactLayers.LAYER_BONUS, self.wheat_list, resources_to_place)
				
				resources_to_place = {
				{self.rice_ID, 1, 100, 1, 2} };
				self:ProcessResourceList(12 * resMultiplier, ImpactLayers.LAYER_BONUS, self.rice_list, resources_to_place)
				
				resources_to_place = {
				{self.maize_ID, 1, 100, 1, 2} };
				self:ProcessResourceList(24 * resMultiplier, ImpactLayers.LAYER_BONUS, self.maize_list, resources_to_place)
				
				resources_to_place = {
				{self.banana_ID, 1, 100, 0, 1} };
				self:ProcessResourceList(12 * resMultiplier, ImpactLayers.LAYER_BONUS, self.banana_list, resources_to_place)
				-- 14
				
				resources_to_place = {
				{self.banana_ID, 1, 100, 0, 1} };
				self:ProcessResourceList(16 * resMultiplier, ImpactLayers.LAYER_BONUS, self.marsh_list, resources_to_place)
				-- none
				
				resources_to_place = {
				{self.cow_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(14 * resMultiplier, ImpactLayers.LAYER_BONUS, self.grass_flat_no_feature, resources_to_place)
				-- 18
				
			-- CBP
				resources_to_place = {
				{self.bison_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(18 * resMultiplier, ImpactLayers.LAYER_BONUS, self.flat_open_no_tundra_no_desert, resources_to_place)
			-- END

				resources_to_place = {
				{self.sheep_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(18 * resMultiplier, ImpactLayers.LAYER_BONUS, self.hills_open_list, resources_to_place)
				-- 13

				resources_to_place = {
				{self.stone_ID, 1, 100, 1, 1} };
				self:ProcessResourceList(30 * resMultiplier, ImpactLayers.LAYER_BONUS, self.dry_grass_flat_no_feature, resources_to_place)
				-- 20
				
				resources_to_place = {
				{self.stone_ID, 1, 100, 1, 1} };
				self:ProcessResourceList(60 * resMultiplier, ImpactLayers.LAYER_BONUS, self.dry_plains_flat_no_feature, resources_to_place)
				-- none
				
				resources_to_place = {
				{self.stone_ID, 1, 100, 1, 2} };
				self:ProcessResourceList(60 * resMultiplier, ImpactLayers.LAYER_BONUS, self.tundra_flat_no_feature, resources_to_place)
				-- 15
				
				resources_to_place = {
				{self.stone_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(14 * resMultiplier, ImpactLayers.LAYER_BONUS, self.desert_flat_no_feature, resources_to_place)
				-- 19
				
				resources_to_place = {
				{self.stone_ID, 1, 100, 1, 2} };
				self:ProcessResourceList(60 * resMultiplier, ImpactLayers.LAYER_BONUS, self.hills_open_list, resources_to_place)
				-- none
				
				resources_to_place = {
				{self.stone_ID, 1, 100, 0, 2} };
				self:ProcessResourceList(8 * resMultiplier, ImpactLayers.LAYER_BONUS, self.snow_flat_list, resources_to_place)
				-- none
				
				resources_to_place = {
				{self.deer_ID, 1, 100, 3, 4} };
				self:ProcessResourceList(25 * resMultiplier, ImpactLayers.LAYER_BONUS, self.forest_flat_that_are_not_tundra, resources_to_place)
				self:ProcessResourceList(25 * resMultiplier, ImpactLayers.LAYER_BONUS, self.hills_forest_list, resources_to_place)
				-- 25
			end
		end
		------------------------------------------------------------------------------
		function AssignStartingPlots:PlaceResourcesAndCityStates()
			-- This function controls nearly all resource placement. Only resources
			-- placed during Normalization operations are handled elsewhere.
			--
			-- Luxury resources are placed in relationship to Regions, adapting to the
			-- details of the given map instance, including number of civs and city
			-- states present. At Jon's direction, Luxuries have been implemented to
			-- be diplomatic widgets for trading, in addition to sources of Happiness.
			--
			-- Strategic and Bonus resources are terrain-adjusted. They will customize
			-- to each map instance. Each terrain type has been measured and has certain
			-- resource types assigned to it. You can customize resource placement to
			-- any degree desired by controlling generation of plot groups to feed in
			-- to the process. The default plot groups are terrain-based, but any
			-- criteria you desire could be used to determine plot group membership.
			--
			-- If any default methods fail to meet a specific need, don't hesitate to
			-- replace them with custom methods. I have labored to make this new
			-- system as accessible and powerful as any ever before offered.

			print("Map Generation - Assigning Luxury Resource Distribution");
			self:AssignLuxuryRoles()

			print("Map Generation - Placing City States");
			self:PlaceCityStates()

			-- Generate global plot lists for resource distribution.
			self:GenerateGlobalResourcePlotLists()

			print("Map Generation - Placing Luxuries");
			self:PlaceLuxuries()

			--print("Map Generation - Placing Stone on Islands");
			self:BuffIslands()

			-- Place Strategic and Bonus resources.
			self:PlaceStrategicAndBonusResources()

			--print("Map Generation - Normalize City State Locations");
			self:NormalizeCityStateLocations()

			-- Fix tile graphics
			self:AdjustTiles()

			local largestLand = Map.FindBiggestArea(false)
			if Map.GetCustomOption(5) == 1 then
				-- Biggest continent placement
				if largestLand:GetNumTiles() < 0.25 * Map.GetLandPlots() then
					print("AI Map Strategy - Offshore expansion with navy bias")
					-- Tell the AI that we should treat this as a offshore expansion map with naval bias
					Map.ChangeAIMapHint(4+1)
				else
					print("AI Map Strategy - Offshore expansion")
					-- Tell the AI that we should treat this as a offshore expansion map
					Map.ChangeAIMapHint(4)
				end
			elseif largestLand:GetNumTiles() < 0.25 * Map.GetLandPlots() then
				print("AI Map Strategy - Navy bias")
				-- Tell the AI that we should treat this as a map with naval bias
				Map.ChangeAIMapHint(1)
			else
				print("AI Map Strategy - Normal")
			end

			-- Necessary to implement placement of Natural Wonders, and possibly other plot-type changes.
			-- This operation must be saved for last, as it invalidates all regional data by resetting Area IDs.
			Map.RecalculateAreas();

			-- Activate for debug only
			self:PrintFinalResourceTotalsToLog()
			--
		end
		------------------------------------------------------------------------------
	else
		print("Override AssignStartingPlots: False");
	end

	return mglobal
end

function GetMapScriptInfo()
	local world_age, temperature, rainfall, sea_level = GetCoreMapOptions()
	return {
		Name = "Communitu_79a v2.6.0",
		Description = "Communitas mapscript for Vox Populi (version 3.1+)",
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
                Name = "Players Start In",
                Values = {
                    "Terra - Largest Continent",
                    "Continents - Everywhere",
                },
                DefaultValue = 2,
                SortPriority = 1,
            },
			{
                Name = "Ocean Shapes",
				Description = "Vertical ocean rifts separating continents",
                Values = {
                    "2 Atlantic",
                    "Pacific and Atlantic",
                    "2 Pacific",
                    "2 Random",
                    "1 Random",
                    "None",
					"Random",
                },
                DefaultValue = 2,
                SortPriority = 2,
            },
			{
                Name = "Rift Width",
				Description = "Impassable ocean width",
                Values = {
                    "Narrow",
                    "Normal",
                    "Wide",
					"Random",
                },
                DefaultValue = 2,
                SortPriority = 3,
            },
			{
                Name = "Circumnavigation",
				Description = "Guarantees horizontal water path around the world",
                Values = {
                    "On",
                    "Off",
                },
                DefaultValue = 2,
                SortPriority = 4,
            },
			{
				Name = "Force Coastal Start",
				Description = "Every major civ starts on the coast",
				Values = {
					"Yes",
					"No",
					"Random",
				},
				DefaultValue = 2,
                SortPriority = 5,
            },
			{
				Name = "Land Shapes",
				Description = "Controls continent and island shapes",
				Values = {
					"Blocky",
					"Normal",
					"Stringy",
					"Extremely Stringy",
					"Random",
				},
				DefaultValue = 2,
                SortPriority = 6,
            },
			{
                Name = "Starting Resources",
				Description = "Adds strategics and/or extra resources to starting position",
                Values = {
					"Add both",
					"Normal",
					"Add some strategics",
					"Add some extra bonus",
                },
				DefaultValue = 2,
                SortPriority = 7,
            },
			{
                Name = "Strategic Deposit Size",
				Description = "Controls the size of each strategic resource deposit",
                Values = {
                    "Small",
                    "Normal",
					"Large",
					"Random",
                },
                DefaultValue = 2,
                SortPriority = 8,
            },
			{
                Name = "Bonus Resource Density",
				Description = "Controls the amount of bonus resources on the map",
                Values = {
                    "Sparse",
                    "Normal",
					"Abundant",
					"Random",
                },
                DefaultValue = 2,
                SortPriority = 9,
            },
			{
                Name = "Strategic Resource Density",
				Description = "Controls the amount of strategic resource deposits appearing on the map",
                Values = {
                    "Sparse",
                    "Normal",
					"Abundant",
					"Random",
                },
                DefaultValue = 2,
                SortPriority = 10,
            },
			{
                Name = "Luxury Resource Density",
				Description = "Controls the amount of luxury resources on the map",
                Values = {
                    "Sparse",
                    "Normal",
					"Abundant",
					"Random",
                },
                DefaultValue = 2,
				SortPriority = 11,
			},
			{
				Name = "Override AssignStartingPlots",
				Description = "Extra map-specific optimizations when turned on. Turn off for standard VP performance.",
				Values = {
					"No",
					"Yes",
				},
				DefaultValue = 2,
				SortPriority = 12,
			},
		},
	}
end

function GetMapInitData(worldSize)
	print("GetMapInitData")
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {39, 28},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {56, 38},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {69, 46},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {79, 53},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {87, 59},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {97, 66}
		}

	if Map.GetCustomOption(5) == 1 then
		-- Enlarge terra-style maps 30% to create expansion room on the new world
		worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {44, 31},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {64, 43},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {78, 52},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {90, 60},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {99, 66},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {109, 74}
		}
	end
	--
	local grid_size = worldsizes[worldSize]


	--
	local world = GameInfo.Worlds[worldSize]
	if(world ~= nil) then
	return {
		Width = grid_size[1],
		Height = grid_size[2],
		WrapX = true,
	}
     end
end

function DetermineContinents()
	print("Determining continents for art purposes (CommunitasMap)")
	-- Each plot has a continent art type.
	-- Command for setting the art type for a plot is: <plot object>:SetContinentArtType(<art_set_number>)

	-- CONTINENTAL ART SETS - in order from hot to cold
	-- 0) Ocean
	-- 3) Africa
	-- 2) Asia
	-- 1) America
	-- 4) Europe

	contArt = {}
	contArt.OCEAN	= 0
	contArt.AFRICA	= 3
	contArt.ASIA	= 2
	contArt.AMERICA	= 1
	contArt.EUROPE	= 4

	local mapW, mapH = Map.GetGridSize()

	--[[
 	for i, plot in Plots() do
 		if plot:IsWater() then
 			plot:SetContinentArtType(contArt.OCEAN)
 		else
 			plot:SetContinentArtType(contArt.AFRICA)
 		end
 	end
	--]]

	local continentMap = PWAreaMap:New(elevationMap.width,elevationMap.height,elevationMap.wrapX,elevationMap.wrapY)
	continentMap:DefineAreas(oceanMatch)
	table.sort(continentMap.areaList,function (a,b) return a.size > b.size end)

	--check for jungle
	for y=0, elevationMap.height - 1 do
		for x=0,elevationMap.width - 1 do
			local i = elevationMap:GetIndex(x,y)
			local area = continentMap:GetAreaByID(continentMap.data[i])
			area.hasJungle = false
		end
	end
	for y=0, elevationMap.height - 1 do
		for x=0, elevationMap.width - 1 do
			local plot = Map.GetPlot(x,y)
			if plot:GetFeatureType() == FeatureTypes.FEATURE_JUNGLE then
				local i = elevationMap:GetIndex(x,y)
				local area = continentMap:GetAreaByID(continentMap.data[i])
				area.hasJungle = true
			end
		end
	end
	for n=1, #continentMap.areaList do
--		if not continentMap.areaList[n].trueMatch and not continentMap.areaList[n].hasJungle then
		if not continentMap.areaList[n].trueMatch then
			continentMap.areaList[n].artStyle = 1 + Map.Rand(2, "Continent Art Styles - Lua") -- left out America's orange trees
		end
	end
	for y=0, elevationMap.height - 1 do
		for x=0, elevationMap.width - 1 do
			local plot = Map.GetPlot(x,y)
			local i = elevationMap:GetIndex(x,y)
			local artStyle = continentMap:GetAreaByID(continentMap.data[i]).artStyle
			if plot:IsWater() then
				plot:SetContinentArtType(contArt.OCEAN)
			elseif jungleMatch(x,y) then
				plot:SetContinentArtType(contArt.ASIA)
			else
				plot:SetContinentArtType(contArt.AFRICA)
			end
		end
	end

	--Africa has the best looking deserts, so for the biggest
	--desert use Africa. America has a nice dirty looking desert also, so
	--that should be the second biggest desert.
	local desertMap = PWAreaMap:New(elevationMap.width,elevationMap.height,elevationMap.wrapX,elevationMap.wrapY)
	desertMap:DefineAreas(desertMatch)
	table.sort(desertMap.areaList,function (a,b) return a.size > b.size end)
	local largestDesertID = nil
	local secondLargestDesertID = nil
	for n=1,#desertMap.areaList do
		--if debugTime then print(string.format("area[%d] size = %d",n,desertMap.areaList[n].size)) end
		if desertMap.areaList[n].trueMatch then
			if largestDesertID == nil then
				largestDesertID = desertMap.areaList[n].id
			else
				secondLargestDesertID = desertMap.areaList[n].id
				break
			end
		end
	end
	for y=0,elevationMap.height - 1 do
		for x=0,elevationMap.width - 1 do
			local plot = Map.GetPlot(x,y)
			local i = elevationMap:GetIndex(x,y)
			if desertMap.data[i] == largestDesertID then
				plot:SetContinentArtType(contArt.AFRICA)
			elseif desertMap.data[i] == secondLargestDesertID then
				plot:SetContinentArtType(contArt.AMERICA)
			elseif plot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
				plot:SetContinentArtType(contArt.ASIA)
			end
		end
	end

	-- Set tundra/mountains -> snowy when near to snow tiles
	for y = 0, mapH-1 do
		for x = 0, mapW-1 do
			local plot = Map.GetPlot(x,y)
			local plotTerrainID = plot:GetTerrainType()
			if IsMountain(plot) then
				local coldness = 0
				local zone = elevationMap:GetZone(y)

				if (zone == mg.NPOLAR or zone == mg.SPOLAR) then
					coldness = coldness + 2
				elseif (zone == mg.NTEMPERATE or zone == mg.STEMPERATE) then
					coldness = coldness + 1
				else
					coldness = coldness - 1
				end

				for nearPlot in Plot_GetPlotsInCircle(plot, 1, 1) do
					local nearTerrainID = nearPlot:GetTerrainType()
					local nearFeatureID = nearPlot:GetFeatureType()
					if IsMountain(nearPlot) then
						coldness = coldness + 0.5
					elseif nearTerrainID == TerrainTypes.TERRAIN_SNOW then
						coldness = coldness + 2
					elseif nearTerrainID == TerrainTypes.TERRAIN_TUNDRA then
						coldness = coldness + 1
					elseif nearTerrainID == TerrainTypes.TERRAIN_DESERT then
						coldness = coldness - 1
					elseif nearFeatureID == FeatureTypes.FEATURE_JUNGLE or nearFeatureID == FeatureTypes.FEATURE_MARSH then
						coldness = coldness - 8
					end
				end

				for nearPlot in Plot_GetPlotsInCircle(plot, 2, 2) do
					if IsMountain(nearPlot) then
						coldness = coldness + 0.25
					end
				end

				-- Avoid snow near tropical jungle
				if coldness >= 1 then
					for nearPlot in Plot_GetPlotsInCircle(plot, 2, 3) do
						local nearFeatureID = nearPlot:GetFeatureType()
						if nearFeatureID == FeatureTypes.FEATURE_JUNGLE or nearFeatureID == FeatureTypes.FEATURE_MARSH then
							coldness = coldness - 8 / math.max(1, Map.PlotDistance(x, y, nearPlot:GetX(), nearPlot:GetY()))
						end
					end
				end

				if coldness >= 6 then
					--plot:SetTerrainType(TerrainTypes.TERRAIN_SNOW, false, true)
					plot:SetContinentArtType(contArt.EUROPE)
				elseif coldness >= 4 then
					--plot:SetTerrainType(TerrainTypes.TERRAIN_TUNDRA, false, true)
					plot:SetContinentArtType(contArt.AMERICA)
				elseif coldness >= 2 then
					--plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true)
					plot:SetContinentArtType(contArt.ASIA)
				else
					--plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true)
					plot:SetContinentArtType(contArt.AFRICA)
				end


			elseif plotTerrainID == TerrainTypes.TERRAIN_TUNDRA then
				local coldness = 0
				for nearPlot in Plot_GetPlotsInCircle(plot, 1, 1) do
					local nearTerrainID = nearPlot:GetTerrainType()
					local nearFeatureID = nearPlot:GetFeatureType()
					if nearTerrainID == TerrainTypes.TERRAIN_SNOW then
						coldness = coldness + 5
					elseif nearTerrainID == TerrainTypes.TERRAIN_TUNDRA then
						coldness = coldness + 1
					elseif nearTerrainID == TerrainTypes.TERRAIN_DESERT or nearFeatureID == FeatureTypes.FEATURE_JUNGLE or nearFeatureID == FeatureTypes.FEATURE_MARSH then
						coldness = coldness - 2
					end
				end
				for nearPlot in Plot_GetPlotsInCircle(plot, 2, 2) do
					if nearTerrainID == TerrainTypes.TERRAIN_DESERT or nearFeatureID == FeatureTypes.FEATURE_JUNGLE or nearFeatureID == FeatureTypes.FEATURE_MARSH then
						coldness = coldness - 1
					end
				end
				if coldness >= 6 then
					if plot:GetFeatureType() == FeatureTypes.FEATURE_FOREST then
						plot:SetContinentArtType(contArt.ASIA)
					else
						plot:SetContinentArtType(contArt.EUROPE)
					end
				else
					plot:SetContinentArtType(contArt.AFRICA)
				end
			elseif plotTerrainID == TerrainTypes.TERRAIN_SNOW then
				plot:SetContinentArtType(contArt.EUROPE)
			end
		end
	end
end

function inheritsFrom( baseClass )
	--inheritance mechanism from http://www.gamedev.net/community/forums/topic.asp?topic_id=561909

    local new_class = {}
    local class_mt = { __index = new_class }

    function new_class:create()
        local newinst = {}
        setmetatable( newinst, class_mt )
        return newinst
    end

    if nil ~= baseClass then
        setmetatable( new_class, { __index = baseClass } )
    end

    -- Implementation of additional OO properties starts here --
    -- Return the class object of the instance
    function new_class:class()
        return new_class
    end

	-- Return the super class object of the instance, optional base class of the given class (must be part of hiearchy)
    function new_class:baseClass(class)
		return new_class:_B(class)
    end

    -- Return the super class object of the instance, optional base class of the given class (must be part of hiearchy)
    function new_class:_B(class)
		if (class==nil) or (new_class==class) then
			return baseClass
		elseif(baseClass~=nil) then
			return baseClass:_B(class)
		end
		return nil
    end

	-- Return true if the caller is an instance of theClass
    function new_class:_ISA( theClass )
        local b_isa = false

        local cur_class = new_class

        while ( nil ~= cur_class ) and ( false == b_isa ) do
            if cur_class == theClass then
                b_isa = true
            else
                cur_class = cur_class:baseClass()
            end
        end

        return b_isa
    end

    return new_class
end

function Logger(self)
	local logger = {}
	setmetatable(logger, self)
	self.__index = self

	logger.level = LEVEL.INFO

	logger.SetLevel = function (self, level)
		self.level = level
	end

	logger.Message = function (self, level, ...)
		local arg = {...}
		if LEVEL[level] < LEVEL[self.level] then
			return false
		end
		if type(arg[1]) == "string" then
			local _, numCommands = string.gsub(arg[1], "[%%]", "")
			for i = 2, numCommands+1 do
				if type(arg[i]) ~= "number" and type(arg[i]) ~= "string" then
					arg[i] = tostring(arg[i])
				end
			end
		else
			arg[1] = tostring(arg[1])
		end
		local message = string.format(unpack(arg))
		if level == LOG_FATAL then
			message = string.format("Turn %-3s %s", Game.GetGameTurn(), message)
			print(level .. string.rep(" ", 7-level:len()) .. message)
			if debug then print(debug.traceback()) end
		else
			if level >= LOG_INFO then
				message = string.format("Turn %-3s %s", Game.GetGameTurn(), message)
			end
			print(level .. string.rep(" ", 7-level:len()) .. message)
		end
		return true
	end

	if debugWithLogger then
		logger.Trace = function (logger, ...) return logger:Message(LOG_TRACE, unpack{...}) end
		logger.Debug = function (logger, ...) return logger:Message(LOG_DEBUG, unpack{...}) end
		logger.Info  = function (logger, ...) return logger:Message(LOG_INFO,  unpack{...}) end
		logger.Warn  = function (logger, ...) return logger:Message(LOG_WARN,  unpack{...}) end
		logger.Error = function (logger, ...) return logger:Message(LOG_ERROR, unpack{...}) end
		logger.Fatal = function (logger, ...) return logger:Message(LOG_FATAL, unpack{...}) end
	else
		logger.Trace = function () end
		logger.Debug = function () end
		logger.Info  = function () end
		logger.Warn  = function () end
		logger.Error = function () end
		logger.Fatal = function () end
	end
	return logger
end

LOG_TRACE	= "TRACE"
LOG_DEBUG	= "DEBUG"
LOG_INFO	= "INFO"
LOG_WARN	= "WARN"
LOG_ERROR	= "ERROR"
LOG_FATAL	= "FATAL"

LEVEL = {
	[LOG_TRACE] = 1,
	[LOG_DEBUG] = 2,
	[LOG_INFO]  = 3,
	[LOG_WARN]  = 4,
	[LOG_ERROR] = 5,
	[LOG_FATAL] = 6,
}

LuaLogger = {}
LuaLogger.New = Logger

log = LuaLogger:New()
log:SetLevel("INFO")


--
-- Generate Plots
--

function StartPlotSystem()
	-- Get Resources setting input by user.
	local resDensity = Map.GetCustomOption(14) or 2;
	local resSize = Map.GetCustomOption(12) or 2;
	local resBalance = (Map.GetCustomOption(11) == 1 or Map.GetCustomOption(11) == 3);
	local bonusDensity = Map.GetCustomOption(13) or 2;
	local legStart = (Map.GetCustomOption(11) == 1 or Map.GetCustomOption(11) == 4);
	local luxuryDensity = Map.GetCustomOption(15) or 2;

	if resSize == 4 then resSize = 1 + Map.Rand(3, "Random Strategic Deposit Size") end

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
	local divMethod = nil;
	if oStarts == 2 then
		-- Continents
		divMethod = 2;
	else
		--Terra
		divMethod = 1;
	end

	print("Creating start plot database.")
	local start_plot_database = AssignStartingPlots.Create();

	print("Dividing the map in to Regions.")
	-- Regional Division Method 2: Continental or 1:Terra
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
	print("Coastal Starts Option", coastalStarts);
	args = {mustBeCoast = (coastalStarts == 1) };

	print("Choosing start locations for civilizations.");
	start_plot_database:ChooseLocations(args);

	print("Normalizing start locations and assigning them to Players.");
	start_plot_database:BalanceAndAssign();

	--error(":P")
	print("Placing Natural Wonders.");
	start_plot_database:PlaceNaturalWonders();

	print("Placing Resources and City States.");
	start_plot_database:PlaceResourcesAndCityStates();
end

function GeneratePlotTypes()
	print("Creating initial map data - CommunitasMap")

	local timeStart = debugTime and os.clock() or 0
	local mapW, mapH = Map.GetGridSize()

	--first do all the preliminary calculations in this function
	if debugTime then print(string.format("map size: width=%d, height=%d",mapW,mapH)) end
	mg = MapGlobals:New()
	PWRandSeed()

	-- Elevations

	elevationMap = GenerateElevationMap(mapW,mapH,true,false)

	--elevationMap:Save("elevationMap.csv")

	-- Plots
	print("Generating plot types - CommunitasMap")
	ShiftMaps()
	DiffMap = GenerateDiffMap(mapW,mapH,true,false)
	CreateArcticOceans() -- Makes a line of water in the poles
	CreateVerticalOceans()
	CreateMagallanes() -- Circumnavegable path (by tu_79)
	ConnectSeasToOceans()
	FillInLakes()
	elevationMap = SetOceanRiftElevations(elevationMap)
	ConnectTerraContinents()

	-- Rainfall
	rainfallMap, temperatureMap = GenerateRainfallMap(elevationMap)
	--rainfallMap:Save("rainfallMap.csv")

	-- Rivers
	riverMap = RiverMap:New(elevationMap)
	riverMap:SetJunctionAltitudes()
	riverMap:SiltifyLakes()
	riverMap:SetFlowDestinations()
	riverMap:SetRiverSizes(rainfallMap)

	--find exact thresholds
	local hillsThreshold = DiffMap:FindThresholdFromPercent(mg.flatPercent,false,true)
	local mountainsThreshold = DiffMap:FindThresholdFromPercent(mg.belowMountainPercent,false,true)
	local i = 0
	for y = 0, mapH - 1,1 do
		for x = 0,mapW - 1,1 do
			local plot = Map.GetPlot(x,y)
			if elevationMap:IsBelowSeaLevel(x,y) then
				plot:SetPlotType(PlotTypes.PLOT_OCEAN, false, false)
			elseif DiffMap.data[i] < hillsThreshold then
				plot:SetPlotType(PlotTypes.PLOT_LAND,false,false)
			--This code makes the game only ever plot flat land if it's within two tiles of
			--the seam. This prevents issues with tiles that don't look like what they are.
			elseif x == 0 or x == 1 or x == mapW - 1 or x == mapW -2 then
				plot:SetPlotType(PlotTypes.PLOT_LAND,false,false)
			-- Bobert13
			elseif DiffMap.data[i] < mountainsThreshold then
				plot:SetPlotType(PlotTypes.PLOT_HILLS,false,false)
			else
				plot:SetPlotType(PlotTypes.PLOT_MOUNTAIN,false,false)
			end
			i=i+1
		end
	end
	Map.RecalculateAreas()
	GenerateIslands()
	GenerateCoasts()
	SetOceanRiftPlots()
end

function ConnectSeasToOceans()
	local areaMap = PWAreaMap:New(elevationMap.width,elevationMap.height,elevationMap.wrapX,elevationMap.wrapY)
	areaMap:DefineAreas(waterMatch)
	local oceanArea, oceanSize = GetLargestArea(areaMap)

	if not oceanArea then
		print("ConnectSeasToOceans: No ocean!")
		return
	end

	-- sort the ocean list from largest to smallest
	table.sort(areaMap.areaList, function (a,b) return a.size > b.size end)

	local plotFunc = function(plot)
		return not Plot_IsWater(plot, true)
	end

	-- print("ConnectSeasToOceans: oceanSize = %s", oceanSize)
	local newWater = {}
	for areaID=1, #areaMap.areaList do
		local seaArea = areaMap.areaList[areaID]
		if seaArea.trueMatch and seaArea.size < oceanSize then
			local pathPlots, distance, airDistance = GetPathBetweenAreas(areaMap, seaArea, oceanArea, true, plotFunc)
			if seaArea.size >= mg.lakeSize then--or seaArea.size >= 2 * distance then
				print("ConnectSeasToOceans: Connect seaArea.size = ", seaArea.size, " distance = ", distance, " airDistance = ", airDistance)
				--log:Info("                     Connect")
				for _, plot in pairs(pathPlots) do
					local plotID = Plot_GetID(plot)
					newWater[Plot_GetID(plot)] = elevationMap.seaLevelThreshold - 0.01
					print("ConnectSeasToOceans: x = ", plot:GetX(), " y = ", plot:GetY(), "elevation = ", GetElevationByPlotID(Plot_GetID(plot)));
					table.insert(mg.seaPlots, plot)
					--plot:SetFeatureType(FeatureTypes.FEATURE_ICE, -1)
				end
			end
		end
	end
	for plotID, elevation in pairs(newWater) do
		elevationMap.data[plotID] = elevation
	end
end

function ConnectPolarSeasToOceans()
	
	local areaMap = PWAreaMap:New(elevationMap.width,elevationMap.height,elevationMap.wrapX,elevationMap.wrapY)
	areaMap:DefineAreas(oceanButNotIceMatch)
	local oceanArea, oceanSize = GetLargestArea(areaMap)

	if not oceanArea then
		print("ConnectPolarSeasToOceans: No ocean!")
		return
	end

	-- sort the ocean list from largest to smallest
	table.sort(areaMap.areaList, function (a,b) return a.size > b.size end)

	local plotFunc = function(plot)
		local mapW, mapH = Map.GetGridSize()
		local x, y = plot:GetX(), plot:GetY()
		-- if y < 2 or y > mapH - 3 then
			-- return false
		-- end
		
		if Map.GetCustomOption(5) == 1 then
			-- Terra-style formation, don't remove land
			return plot:GetFeatureType() == FeatureTypes.FEATURE_ICE
		end
		return not Plot_IsWater(plot, false) or plot:GetFeatureType() == FeatureTypes.FEATURE_ICE or Plot_IsLake(plot)
	end

	-- print("ConnectPolarSeasToOceans: oceanSize = ", oceanSize)
	local newWater = {}
	for areaID=1, #areaMap.areaList do
		local seaArea = areaMap.areaList[areaID]
		if seaArea.trueMatch and seaArea.size < oceanSize then
			local pathPlots, distance, airDistance = GetPathBetweenAreas(areaMap, seaArea, oceanArea, true, plotFunc)
			print("ConnectPolarSeasToOceans: Connect seaArea.size = ", seaArea.size, " distance = ", distance, " airDistance = ", airDistance)
			for _, plot in pairs(pathPlots) do
				print("ConnectPolarSeasToOceans: x = ", plot:GetX(), " y = ", plot:GetY(), "elevation = ", GetElevationByPlotID(Plot_GetID(plot)));
				local plotID = Plot_GetID(plot)
				newWater[plotID] = elevationMap.seaLevelThreshold - 0.01
				plot:SetPlotType(PlotTypes.PLOT_OCEAN, false, true)
				plot:SetTerrainType(TerrainTypes.TERRAIN_COAST, false, true)
				plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1)
			end
			for _, plot in pairs(pathPlots) do
				local foundLand = false
				for nearPlot in Plot_GetPlotsInCircle(plot, 1, 2) do
					if not Plot_IsWater(nearPlot) then
						foundLand = true
						break
					end
				end
				if not foundLand then
					plot:SetTerrainType(TerrainTypes.TERRAIN_OCEAN, true, true)
				end
			end
		end
	end
	for plotID, elevation in pairs(newWater) do
		elevationMap.data[plotID] = elevation
	end
end

function ConnectTerraContinents()
	if Map.GetCustomOption(5) == 2 then
		-- Continents-style formation
		return
	end

	log:Info("ConnectTerraContinents")

	local oceanX1		= mg.oceanRiftMidlines[1]
	local oceanX2		= mg.oceanRiftMidlines[2]
	local continents	= {}
	local totalLand		= 0
	local areaMap		= PWAreaMap:New(elevationMap.width,elevationMap.height,elevationMap.wrapX,elevationMap.wrapY)

	areaMap:DefineAreas(landMatch)

	for areaID=1, #areaMap.areaList do
		local area = areaMap.areaList[areaID]
		if area.trueMatch and area.size > 10 then
			if not IsAreaBetweenOceans(area, oceanX1, oceanX2) then
				table.insert(continents, area)
			end
		end
	end

	if continents == {} then
		print("ConnectTerraContinents: No biggest continent!")
		return
	end

	for plotID, elevation in pairs(elevationMap.data) do
		if elevation > elevationMap.seaLevelThreshold then
			totalLand = totalLand + 1
		end
	end

	table.sort(continents, function(a, b)
		return a.size > b.size
	end)

	log:Info("ConnectTerraContinents: largestLand = %s%% of %s totalLand", Round(100 * continents[1].size / totalLand), totalLand)
	local largestSize = continents[1].size
	if largestSize > mg.percentLargestContinent * totalLand then
		return
	end

	--

	local plotFunc = function(plot)
		return Plot_IsWater(plot, true)
	end

	for i = 1, #continents do
		if i == 1 then
			continents[i].pathPlots		= {}
			continents[i].airDistance	= 0
			continents[i].distance		= 0
		else
			local pathPlots, distance, airDistance = GetPathBetweenAreas(areaMap, continents[i], continents[1], false, plotFunc)
			continents[i].pathPlots		= pathPlots
			continents[i].airDistance	= airDistance
			continents[i].distance		= distance
		end
	end

	for index, area in ipairs(continents) do
		log:Info("ConnectTerraContinents: continent #%-2s size = %-4s distance = %-3s size/distance = %s", index, area.size, area.distance, Round(area.size / math.max(1, area.distance)))
	end

	local newLand = {}

	for index, area in ipairs(continents) do
		if index ~= 1 and area.distance < mg.oceanRiftWidth + 2 and area.size / math.max(1, area.distance) > mg.terraConnectWeight then
			print("ConnectTerraContinents: Connect continent ", index, "size = ", "distance = ", area.distance, "airDistance = ", area.airDistance);
			for _, plot in pairs(area.pathPlots) do
				newLand[Plot_GetID(plot)] = elevationMap.seaLevelThreshold
				print("ConnectTerraContinents: x = ", plot:GetX(), " y = ", plot:GetY(), "elevation = ", GetElevationByPlotID(Plot_GetID(plot)));
			end
			largestSize = largestSize + area.size
			if largestSize > mg.percentLargestContinent * totalLand then
				break
			end
		end
	end
	for plotID, elevation in pairs(newLand) do
		--Map.GetPlotByIndex(plotID):SetFeatureType(FeatureTypes.FEATURE_ICE, -1)
		elevationMap.data[plotID] = elevation
	end
end

function IsAreaBetweenOceans(area, oceanX1, oceanX2)
	return false
end

function GetPathBetweenAreas(areaMap, areaA, areaB, findLowest, plotMatchFunc)
	-- using Dijkstra's algorithm
	local mapW, mapH = Map.GetGridSize()
	
	-- initialize
	local plots = {}
	for plotID = 0, areaMap.length - 1 do
		plots[plotID]			= {}
		plots[plotID].plot		= Map.GetPlot(elevationMap:GetXYFromIndex(plotID))
		plots[plotID].areaID	= areaMap.data[plotID]

		if plots[plotID].areaID == areaA.id or plots[plotID].areaID == areaB.id then
			-- consider all plots equal in start and end areas
			plots[plotID].elevation	= 0;
		else
			if findLowest then
				-- connect oceans
				plots[plotID].elevation	= 2 ^ ((GetElevationByPlotID(plotID) - elevationMap.seaLevelThreshold) * 10);
				if plots[plotID].elevation < 1 then
					plots[plotID].elevation	= 0;
				end
				if plots[plotID].plot:GetFeatureType() == FeatureTypes.FEATURE_ICE then
					plots[plotID].elevation	= 0.5;
				end
			else
				-- connect continents
				plots[plotID].elevation	= 2 ^ ((GetElevationByPlotID(plotID) - elevationMap.seaLevelThreshold) * 10);
				if plots[plotID].elevation < 1 then
					plots[plotID].elevation	= 0;
				end
			end
		end

		if plots[plotID].areaID == areaA.id then
			plots[plotID].sumElevation = 0
		else
			plots[plotID].sumElevation = 10000
		end
	end

	-- main loop
	local lowestID = -1
	local lowest = nil
	local attempts = 0
	while attempts < mapW * mapH do
		lowestID, lowest = GetBestFromTable(plots,
			function(a, b)
				if not a or not b then
					return a or b
				end
				if a.sumElevation ~= b.sumElevation then
					return a.sumElevation < b.sumElevation
				end
				return a.elevation < b.elevation
			end
		)

		if not lowest or lowest.sumElevation == math.huge or lowest.areaID == areaB.id then
			break
		end
		plots[lowestID] = nil
		for nearPlot in Plot_GetPlotsInCircle(lowest.plot, 1) do
			local nearID = Plot_GetID(nearPlot)
			local nearPlotInfo = plots[nearID]
			if nearPlotInfo then
				local altSumElevation = lowest.sumElevation + nearPlotInfo.elevation
				if altSumElevation < nearPlotInfo.sumElevation then
					plots[nearID].sumElevation = altSumElevation
					plots[nearID].previous = lowest
				end
			end
		end
	end

	local path = {}
	local start = lowest
	while lowest.previous do
		if plotMatchFunc(lowest.plot) then
			table.insert(path, lowest.plot)
		end
		lowest = lowest.previous
	end
	return path, #path, Map.PlotDistance(start.plot:GetX(), start.plot:GetY(), lowest.plot:GetX(), lowest.plot:GetY())
end

function GetBestFromTable(list, compareFunc)
	local least = nil
	local leastID = -1
	for k, v in pairs(list) do
		if compareFunc(v, least) then
			leastID = k
			least = v
		end
	end
	return leastID, least
end



function GetLargestArea(areaMap)
	local largestArea = nil
	local largestSize = 0
	for areaID=1, #areaMap.areaList do
		local area = areaMap.areaList[areaID]
		if area.trueMatch and area.size > largestSize then
			largestSize = area.size
			largestArea = area
		end
	end
	return largestArea, largestSize
end

function FillInLakes()
	local areaMap = PWAreaMap:New(elevationMap.width,elevationMap.height,elevationMap.wrapX,elevationMap.wrapY)
	areaMap:DefineAreas(waterMatch)
	for i=1, #areaMap.areaList do
		local area = areaMap.areaList[i]
		if area.trueMatch and area.size <= mg.lakeSize then
			for n = 0, areaMap.length do
				if areaMap.data[n] == area.id then
					elevationMap.data[n] = elevationMap.seaLevelThreshold
					--print("Saving lake of size ".. area.size)
					table.insert(mg.lakePlots, Map.GetPlot(elevationMap:GetXYFromIndex(n)))
				end
			end
		end
	end
end

function RestoreLakes()
	log:Info("RestoreLakes")

	--[[
	-- Seperate lakes from new ocean rifts
	for index, plot in pairs(mg.lakePlots) do
		local isLake = true
		for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
			if nearPlot:IsWater() and not nearPlot:IsLake() then
				table.remove(mg.lakePlots, index)
				break
			end
		end
	end
	--]]

	-- Remove all rivers bordering lakes or oceans
	for _, plot in Plots() do
		if Plot_IsWater(plot) then
			for edgeDirection = 0, DirectionTypes.NUM_DIRECTION_TYPES - 1 do
				Plot_SetRiver(plot, edgeDirection, mg.flowNONE)
			end
		end
	end

	-- Add lakes
	local newLand = {};
	for _, plot in pairs(mg.lakePlots) do
		local mapW, mapH = Map.GetGridSize();
		local x, y = plot:GetX(), plot:GetY();
		if y < 2 or y > mapH - 3 then
			-- Set to ice
			plot:SetPlotType(PlotTypes.PLOT_OCEAN, false, true);
			plot:SetTerrainType(TerrainTypes.TERRAIN_COAST, false, true);
			plot:SetFeatureType(FeatureTypes.FEATURE_ICE, -1);
		elseif y == 2 or y == mapH - 3 then
			-- Set to flat snow
			local plotID = Plot_GetID(plot);
			newLand[plotID] = elevationMap.seaLevelThreshold + 0.01;
			plot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
			plot:SetTerrainType(TerrainTypes.TERRAIN_SNOW, false, true);
			plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1);
		else
			plot:SetTerrainType(TerrainTypes.TERRAIN_COAST, false, true);
		end
	end
	for plotID, elevation in pairs(newLand) do
		elevationMap.data[plotID] = elevation
	end

	-- Calculate outflow from lakes
	local riversToAdd = {}
	local lakesDone = {}
	for plotID, plot in Plots(Shuffle) do
		for edgeDirection = 0, DirectionTypes.NUM_DIRECTION_TYPES - 1 do
			if Plot_IsRiver(plot, edgeDirection) then
				prevPlot, edgeA, edgeB, flowA, flowB = Plot_GetPreviousRiverPlot(plot, edgeDirection)
				if prevPlot and Plot_IsLake(prevPlot) and not Contains(lakesDone, prevPlot) then
					print(string.format(
						"%2s flowing river: add edge %2s flowing %2s, edge %2s flowing %2s",
						mg.flowNames[Plot_GetRiverFlowDirection(plot, edgeDirection)],
						mg.directionNames[edgeA],
						mg.flowNames[flowA],
						mg.directionNames[edgeB],
						mg.flowNames[flowB]
					))
					table.insert(riversToAdd, {plot=prevPlot, edge=edgeA, flow=flowA})
					table.insert(riversToAdd, {plot=prevPlot, edge=edgeB, flow=flowB})
					table.insert(lakesDone, prevPlot)
				end
			end
		end
	end

	for _, v in pairs(riversToAdd) do
		Plot_SetRiver(v.plot, v.edge, v.flow)
	end
end

function AddLakes()
	-- disable vanilla lake creation
end



--
-- Generate Terrain
--

function GenerateTerrain()
	print("Generating terrain - CommunitasMap")
	local timeStart = debugTime and os.clock() or 0
	local terrainTundra	= GameInfoTypes["TERRAIN_TUNDRA"]
	local terrainGrass	= GameInfoTypes["TERRAIN_GRASS"]

	local mapW, mapH = Map.GetGridSize()

	--first find minimum rain above sea level for a soft desert transition
	local minRain = 100.0
	for y = 0, mapH-1 do
		for x = 0,mapW-1 do
			local i = elevationMap:GetIndex(x,y)
			if not elevationMap:IsBelowSeaLevel(x,y) then
				if rainfallMap.data[i] < minRain then
					minRain = rainfallMap.data[i]
				end
			end
		end
	end

	--find exact thresholds
	local desertThreshold = rainfallMap:FindThresholdFromPercent(mg.desertPercent,false,true)
	local plainsThreshold = rainfallMap:FindThresholdFromPercent(mg.plainsPercent,false,true)
	for y = 0, mapH-1 do
		for x = 0,mapW-1 do
			local i = elevationMap:GetIndex(x,y)
			local plot = Map.GetPlot(x, y)
			if not elevationMap:IsBelowSeaLevel(x,y) then
				if temperatureMap.data[i] < mg.snowTemperature then
					if rainfallMap.data[i] < desertThreshold then
						plot:SetTerrainType(TerrainTypes.TERRAIN_SNOW,false,false)
					else
						plot:SetTerrainType(terrainTundra,false,false)
					end
				elseif temperatureMap.data[i] < mg.tundraTemperature then
					if rainfallMap.data[i] < plainsThreshold then
						plot:SetTerrainType(terrainTundra,false,false)
					else
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS,false,false)
					end
				elseif temperatureMap.data[i] < mg.jungleMinTemperature then
					-- Here is where most life happens
					if rainfallMap.data[i] < desertThreshold then
						-- It's a cold desert
						plot:SetTerrainType(TerrainTypes.TERRAIN_DESERT,false,false)
					elseif rainfallMap.data[i] < plainsThreshold then
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS,false,false)
					else
						plot:SetTerrainType(terrainGrass,false,false)
					end
				elseif temperatureMap.data[i] < mg.desertMinTemperature then
					-- In the tropics, jungle can happen with enough rain.
					if rainfallMap.data[i] < desertThreshold then
						-- Hot desert
						plot:SetTerrainType(TerrainTypes.TERRAIN_DESERT,false,false)
					else
						-- Except where rain is high.
						plot:SetTerrainType(terrainGrass,false,false)
					end
				else
					--Temperatures too extreme for any life.
					if rainfallMap.data[i] < plainsThreshold then
						plot:SetTerrainType(TerrainTypes.TERRAIN_DESERT,false,false)
					else
						-- Only most humid terrain gets some jungle
						plot:SetTerrainType(terrainGrass,false,false)
					end
				end
			end
		end
	end


	if debugTime then print(string.format("%5s ms, GenerateTerrain %s", math.floor((os.clock() - timeStart) * 1000), "Main")) end
	if debugTime then timeStart = os.clock() end

	if debugTime then print(string.format("%5s ms, GenerateTerrain %s", math.floor((os.clock() - timeStart) * 1000), "SetOceanRiftPlots")) end
	if debugTime then timeStart = os.clock() end
	BlendTerrain()
	if debugTime then print(string.format("%5s ms, GenerateTerrain %s", math.floor((os.clock() - timeStart) * 1000), "BlendTerrain")) end
end


function GenerateIslands()
	-- A cell system will be used to combine predefined land chunks with randomly generated island groups.
	-- Define the cell traits. (These need to fit correctly with the map grid width and height.)
	local iW, iH = Map.GetGridSize()
	local iCellWidth = iW / 3
	local iCellHeight = iH / 2
	local iNumCellColumns = math.floor(iW / iCellWidth)
	local iNumCellRows = math.floor(iH / iCellHeight)
	local iNumTotalCells = iNumCellColumns * iNumCellRows
	local cell_data = table.fill(false, iNumTotalCells) -- Stores data on map cells in use. All cells begin as empty.
	local iNumCellsInUse = 0
	local iNumCellTarget = math.floor(iNumTotalCells * 1)
	local island_chain_PlotTypes = table.fill(PlotTypes.PLOT_OCEAN, iW * iH)

	-- Add randomly generated island groups
	local iNumGroups = iNumCellTarget -- Should virtually never use all the groups.
	for group = 1, iNumGroups do
		if iNumCellsInUse >= iNumCellTarget then -- Map has reeached desired island population.
			print("-") print("** Number of Island Groups produced:", group - 1) print("-")
			break
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
		10. Rectangle 2x3 With Double Dots ]]--
		--
		-- Choose a formation
		local rate_threshold = {}
		local total_appearance_rate, iNumFormations = 0, 0
		local appearance_rates = { -- These numbers are relative to one another. No specific target total is necessary.
			7, -- #1
			3, -- #2
			15, --#3
			8, -- #4
			3, -- #5
			6, -- #6
			4, -- #7
			6, -- #8
			4, -- #9
			3, -- #10
		}
		for i, rate in ipairs(appearance_rates) do
			total_appearance_rate = total_appearance_rate + rate
			iNumFormations = iNumFormations + 1
		end
		local accumulated_rate = 0
		for index = 1, iNumFormations do
			local threshold = (appearance_rates[index] + accumulated_rate) * 10000 / total_appearance_rate
			table.insert(rate_threshold, threshold)
			accumulated_rate = accumulated_rate + appearance_rates[index]
		end
		local formation_type
		local diceroll = Map.Rand(10000, "Choose formation type - Island Making - Lua")
		for index, threshold in ipairs(rate_threshold) do
			if diceroll <= threshold then -- Choose this formation type.
				formation_type = index
				break
			end
		end
		-- Choose cell(s) not in use
		local iNumAttemptsToFindOpenCells = 0
		local found_unoccupied_cell = false
		local anchor_cell, cell_x, cell_y, foo
		while found_unoccupied_cell == false do
			if iNumAttemptsToFindOpenCells > 100 then -- Too many attempts on this pass. Might not be any valid locations for this formation.
				print("-") print("*-* ERROR:  Formation type of", formation_type, "for island group#", group, "unable to find an open space. Switching to single-cell.")
				formation_type = 3 -- Reset formation type.
				iNumAttemptsToFindOpenCells = 0
			end
			local diceroll = 1 + Map.Rand(iNumTotalCells, "Choosing a cell for an island group - Polynesia LUA")
			if cell_data[diceroll] == false then -- Anchor cell is unoccupied.
				-- If formation type is multiple-cell, all secondary cells must also be unoccupied.
				if formation_type == 1 or formation_type == 3 then -- single cell, proceed.
					anchor_cell = diceroll
					found_unoccupied_cell = true
				elseif formation_type == 2 or formation_type == 4 then -- double cell, horizontal.
					-- Check to see if anchor cell is in the final column. If so, reject.
					cell_x = math.fmod(diceroll, iNumCellColumns)
					if cell_x ~= 0 then -- Anchor cell is valid, but still have to check near cell.
						if cell_data[diceroll + 1] == false then -- Adjacent cell is unoccupied.
							anchor_cell = diceroll
							found_unoccupied_cell = true
						end
					end
				elseif formation_type == 5 or formation_type == 6 then -- double cell, vertical.
					-- Check to see if anchor cell is in the final row. If so, reject.
					cell_y, foo = math.modf(diceroll / iNumCellColumns)
					cell_y = cell_y + 1
					if cell_y < iNumCellRows then -- Anchor cell is valid, but still have to check cell above it.
						if cell_data[diceroll + iNumCellColumns] == false then -- Adjacent cell is unoccupied.
							anchor_cell = diceroll
							found_unoccupied_cell = true
						end
					end
				elseif formation_type == 7 then -- triple cell, vertical.
					-- Check to see if anchor cell is in the northern two rows. If so, reject.
					cell_y, foo = math.modf(diceroll / iNumCellColumns)
					cell_y = cell_y + 1
					if cell_y < iNumCellRows - 1 then -- Anchor cell is valid, but still have to check cells above it.
						if cell_data[diceroll + iNumCellColumns] == false then -- Cell directly above is unoccupied.
							if cell_data[diceroll + (iNumCellColumns * 2)] == false then -- Cell two rows above is unoccupied.
								anchor_cell = diceroll
								found_unoccupied_cell = true
							end
						end
					end
				elseif formation_type == 8 then -- square, 2x2.
					-- Check to see if anchor cell is in the final row or column. If so, reject.
					cell_x = math.fmod(diceroll, iNumCellColumns)
					if cell_x ~= 0 then
						cell_y, foo = math.modf(diceroll / iNumCellColumns)
						cell_y = cell_y + 1
						if cell_y < iNumCellRows then -- Anchor cell is valid. Still have to check the other three cells.
							if cell_data[diceroll + iNumCellColumns] == false then
								if cell_data[diceroll + 1] == false then
									if cell_data[diceroll + iNumCellColumns + 1] == false then -- All cells are open.
										anchor_cell = diceroll
										found_unoccupied_cell = true
									end
								end
							end
						end
					end
				elseif formation_type == 9 then -- horizontal, 3x2.
					-- Check to see if anchor cell is too near to an edge. If so, reject.
					cell_x = math.fmod(diceroll, iNumCellColumns)
					if cell_x ~= 0 and cell_x ~= iNumCellColumns - 1 then
						cell_y, foo = math.modf(diceroll / iNumCellColumns)
						cell_y = cell_y + 1
						if cell_y < iNumCellRows then -- Anchor cell is valid. Still have to check the other cells.
							if cell_data[diceroll + iNumCellColumns] == false then
								if cell_data[diceroll + 1] == false and cell_data[diceroll + 2] == false then
									if cell_data[diceroll + iNumCellColumns + 1] == false then
										if cell_data[diceroll + iNumCellColumns + 2] == false then -- All cells are open.
											anchor_cell = diceroll
											found_unoccupied_cell = true
										end
									end
								end
							end
						end
					end
				elseif formation_type == 10 then -- vertical, 2x3.
					-- Check to see if anchor cell is too near to an edge. If so, reject.
					cell_x = math.fmod(diceroll, iNumCellColumns)
					if cell_x ~= 0 then
						cell_y, foo = math.modf(diceroll / iNumCellColumns)
						cell_y = cell_y + 1
						if cell_y < iNumCellRows - 1 then -- Anchor cell is valid. Still have to check the other cells.
							if cell_data[diceroll + iNumCellColumns] == false then
								if cell_data[diceroll + 1] == false then
									if cell_data[diceroll + iNumCellColumns + 1] == false then
										if cell_data[diceroll + (iNumCellColumns * 2)] == false then
											if cell_data[diceroll + (iNumCellColumns * 2) + 1] == false then -- All cells are open.
												anchor_cell = diceroll
												found_unoccupied_cell = true
											end
										end
									end
								end
							end
						end
					end
				end
			end
			iNumAttemptsToFindOpenCells = iNumAttemptsToFindOpenCells + 1
		end
		-- Find Cell X and Y
		cell_x = math.fmod(anchor_cell, iNumCellColumns)
		if cell_x == 0 then
			cell_x = iNumCellColumns
		end
		cell_y, foo = math.modf(anchor_cell / iNumCellColumns)
		cell_y = cell_y + 1

		-- Debug
		--print("-") print("-") print("* Group# " .. group, "Formation Type: " .. formation_type, "Cell X, Y: " .. cell_x .. ", " .. cell_y)

		-- Create this island group.
		local iWidth, iHeight, fTilt -- Scope the variables needed for island group creation.
		local plot_data = {}
		local x_shift, y_shift
		if formation_type == 1 then -- single cell
			local x_shrinkage = Map.Rand(4, "Cell Width adjustment - Lua")
			if x_shrinkage > 2 then
				x_shrinkage = 0
			end
			local y_shrinkage = Map.Rand(5, "Cell Height adjustment - Lua")
			if y_shrinkage > 2 then
				y_shrinkage = 0
			end
			x_shift, y_shift = 0, 0
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua")
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua")
			end
			iWidth = iCellWidth - x_shrinkage
			iHeight = iCellHeight - y_shrinkage
			fTilt = Map.Rand(181, "Angle for island chain axis - LUA")
			plot_data = CreateSingleAxisIslandChain(iWidth, iHeight, fTilt)

		elseif formation_type == 2 then -- two cells, horizontal
			local x_shrinkage = Map.Rand(8, "Cell Width adjustment - Lua")
			if x_shrinkage > 5 then
				x_shrinkage = 0
			end
			local y_shrinkage = Map.Rand(5, "Cell Height adjustment - Lua")
			if y_shrinkage > 2 then
				y_shrinkage = 0
			end
			x_shift, y_shift = 0, 0
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua")
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua")
			end
			iWidth = iCellWidth * 2 - x_shrinkage
			iHeight = iCellHeight - y_shrinkage
			-- Limit angles to mostly horizontal ones.
			fTilt = 145 + Map.Rand(90, "Angle for island chain axis - LUA")
			if fTilt > 180 then
				fTilt = fTilt - 180
			end
			plot_data = CreateSingleAxisIslandChain(iWidth, iHeight, fTilt)

		elseif formation_type == 3 then -- single cell, with dots
			local x_shrinkage = Map.Rand(4, "Cell Width adjustment - Lua")
			if x_shrinkage > 2 then
				x_shrinkage = 0
			end
			local y_shrinkage = Map.Rand(5, "Cell Height adjustment - Lua")
			if y_shrinkage > 2 then
				y_shrinkage = 0
			end
			x_shift, y_shift = 0, 0
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua")
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua")
			end
			iWidth = iCellWidth - x_shrinkage
			iHeight = iCellHeight - y_shrinkage
			fTilt = Map.Rand(181, "Angle for island chain axis - LUA")
			-- Determine "dot box"
			local iInnerWidth, iInnerHeight = iWidth - 2, iHeight - 2
			-- Determine number of dots
			local die_1 = Map.Rand(4, "Diceroll - Lua")
			local die_2 = Map.Rand(8, "Diceroll - Lua")
			local iNumDots = 4
			if die_1 + die_2 > 1 then
				iNumDots = iNumDots + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua")
			else
				iNumDots = iNumDots + die_1 + die_2
			end
			plot_data = CreateAxisChainWithDots(iWidth, iHeight, fTilt, iInnerWidth, iInnerHeight, iNumDots)

		elseif formation_type == 4 then -- two cells, horizontal, with dots
			local x_shrinkage = Map.Rand(8, "Cell Width adjustment - Lua")
			if x_shrinkage > 5 then
				x_shrinkage = 0
			end
			local y_shrinkage = Map.Rand(5, "Cell Height adjustment - Lua")
			if y_shrinkage > 2 then
				y_shrinkage = 0
			end
			x_shift, y_shift = 0, 0
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua")
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua")
			end
			iWidth = iCellWidth * 2 - x_shrinkage
			iHeight = iCellHeight - y_shrinkage
			-- Limit angles to mostly horizontal ones.
			fTilt = 145 + Map.Rand(90, "Angle for island chain axis - LUA")
			if fTilt > 180 then
				fTilt = fTilt - 180
			end
			-- Determine "dot box"
			local iInnerWidth = math.floor(iWidth / 2)
			local iInnerHeight = iHeight - 2
			local iInnerWest = 2 + Map.Rand((iWidth - 1) - iInnerWidth, "Shift for sub island group - Lua")
			local iInnerSouth = 2
			-- Determine number of dots
			local die_1 = Map.Rand(4, "Diceroll - Lua")
			local die_2 = Map.Rand(10, "Diceroll - Lua")
			local iNumDots = 5
			if die_1 + die_2 > 1 then
				iNumDots = iNumDots + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua")
			else
				iNumDots = iNumDots + die_1 + die_2
			end
			plot_data = CreateAxisChainWithShiftedDots(iWidth, iHeight, fTilt, iInnerWidth, iInnerHeight, iInnerWest, iInnerSouth, iNumDots)

		elseif formation_type == 5 then -- Double Cell, Vertical, Axis Only
			local x_shrinkage = Map.Rand(5, "Cell Width adjustment - Lua")
			if x_shrinkage > 2 then
				x_shrinkage = 0
			end
			local y_shrinkage = Map.Rand(7, "Cell Height adjustment - Lua")
			if y_shrinkage > 4 then
				y_shrinkage = 0
			end
			x_shift, y_shift = 0, 0
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua")
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua")
			end
			iWidth = iCellWidth - x_shrinkage
			iHeight = iCellHeight * 2 - y_shrinkage
			-- Limit angles to mostly vertical ones.
			fTilt = 55 + Map.Rand(71, "Angle for island chain axis - LUA")
			plot_data = CreateSingleAxisIslandChain(iWidth, iHeight, fTilt)

		elseif formation_type == 6 then -- Double Cell, Vertical With Dots
			local x_shrinkage = Map.Rand(5, "Cell Width adjustment - Lua")
			if x_shrinkage > 2 then
				x_shrinkage = 0
			end
			local y_shrinkage = Map.Rand(7, "Cell Height adjustment - Lua")
			if y_shrinkage > 4 then
				y_shrinkage = 0
			end
			x_shift, y_shift = 0, 0
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua")
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua")
			end
			iWidth = iCellWidth - x_shrinkage
			iHeight = iCellHeight * 2 - y_shrinkage
			-- Limit angles to mostly vertical ones.
			fTilt = 55 + Map.Rand(71, "Angle for island chain axis - LUA")
			-- Determine "dot box"
			local iInnerWidth = iWidth - 2
			local iInnerHeight = math.floor(iHeight / 2)
			local iInnerWest = 2
			local iInnerSouth = 2 + Map.Rand((iHeight - 1) - iInnerHeight, "Shift for sub island group - Lua")
			-- Determine number of dots
			local die_1 = Map.Rand(4, "Diceroll - Lua")
			local die_2 = Map.Rand(10, "Diceroll - Lua")
			local iNumDots = 5
			if die_1 + die_2 > 1 then
				iNumDots = iNumDots + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua")
			else
				iNumDots = iNumDots + die_1 + die_2
			end
			plot_data = CreateAxisChainWithShiftedDots(iWidth, iHeight, fTilt, iInnerWidth, iInnerHeight, iInnerWest, iInnerSouth, iNumDots)

		elseif formation_type == 7 then -- Triple Cell, Vertical With Double Dots
			local x_shrinkage = Map.Rand(4, "Cell Width adjustment - Lua")
			if x_shrinkage > 1 then
				x_shrinkage = 0
			end
			local y_shrinkage = Map.Rand(9, "Cell Height adjustment - Lua")
			if y_shrinkage > 5 then
				y_shrinkage = 0
			end
			x_shift, y_shift = 0, 0
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua")
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua")
			end
			iWidth = iCellWidth - x_shrinkage
			iHeight = iCellHeight * 3 - y_shrinkage
			-- Limit angles to steep ones.
			fTilt = 70 + Map.Rand(41, "Angle for island chain axis - LUA")
			-- Handle Dots Group 1.
			local iInnerWidth1 = iWidth - 3
			local iInnerHeight1 = iCellHeight - 1
			local iInnerWest1 = 2 + Map.Rand(2, "Shift for sub island group - Lua")
			local iInnerSouth1 = 2 + Map.Rand(iCellHeight - 3, "Shift for sub island group - Lua")
			-- Determine number of dots
			local die_1 = Map.Rand(4, "Diceroll - Lua")
			local die_2 = Map.Rand(8, "Diceroll - Lua")
			local iNumDots1 = 4
			if die_1 + die_2 > 1 then
				iNumDots1 = iNumDots1 + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua")
			else
				iNumDots1 = iNumDots1 + die_1 + die_2
			end
			-- Handle Dots Group 2.
			local iInnerWidth2 = iWidth - 3
			local iInnerHeight2 = iCellHeight - 1
			local iInnerWest2 = 2 + Map.Rand(2, "Shift for sub island group - Lua")
			local iInnerSouth2 = iCellHeight + 2 + Map.Rand(iCellHeight - 3, "Shift for sub island group - Lua")
			-- Determine number of dots
			local die_1 = Map.Rand(4, "Diceroll - Lua")
			local die_2 = Map.Rand(8, "Diceroll - Lua")
			local iNumDots2 = 4
			if die_1 + die_2 > 1 then
				iNumDots2 = iNumDots2 + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua")
			else
				iNumDots2 = iNumDots2 + die_1 + die_2
			end
			plot_data = CreateAxisChainWithDoubleDots(iWidth, iHeight, fTilt, iInnerWidth1, iInnerHeight1, iInnerWest1, iInnerSouth1,
                                                      iNumDots1, iInnerWidth2, iInnerHeight2, iInnerWest2, iInnerSouth2, iNumDots2)
		elseif formation_type == 8 then -- Square Block 2x2 With Double Dots
			local x_shrinkage = Map.Rand(6, "Cell Width adjustment - Lua")
			if x_shrinkage > 4 then
				x_shrinkage = 0
			end
			local y_shrinkage = Map.Rand(5, "Cell Height adjustment - Lua")
			if y_shrinkage > 3 then
				y_shrinkage = 0
			end
			x_shift, y_shift = 0, 0
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua")
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua")
			end
			iWidth = iCellWidth * 2 - x_shrinkage
			iHeight = iCellHeight * 2 - y_shrinkage
			-- Full range of angles
			fTilt = Map.Rand(181, "Angle for island chain axis - LUA")
			-- Handle Dots Group 1.
			local iInnerWidth1 = iCellWidth - 2
			local iInnerHeight1 = iCellHeight - 2
			local iInnerWest1 = 3 + Map.Rand(iCellWidth - 2, "Shift for sub island group - Lua")
			local iInnerSouth1 = 3 + Map.Rand(iCellHeight - 2, "Shift for sub island group - Lua")
			-- Determine number of dots
			local die_1 = Map.Rand(6, "Diceroll - Lua")
			local die_2 = Map.Rand(10, "Diceroll - Lua")
			local iNumDots1 = 5
			if die_1 + die_2 > 1 then
				iNumDots1 = iNumDots1 + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua")
			else
				iNumDots1 = iNumDots1 + die_1 + die_2
			end
			-- Handle Dots Group 2.
			local iInnerWidth2 = iCellWidth - 2
			local iInnerHeight2 = iCellHeight - 2
			local iInnerWest2 = 3 + Map.Rand(iCellWidth - 2, "Shift for sub island group - Lua")
			local iInnerSouth2 = 3 + Map.Rand(iCellHeight - 2, "Shift for sub island group - Lua")
			-- Determine number of dots
			local die_1 = Map.Rand(4, "Diceroll - Lua")
			local die_2 = Map.Rand(8, "Diceroll - Lua")
			local iNumDots2 = 5
			if die_1 + die_2 > 1 then
				iNumDots2 = iNumDots2 + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua")
			else
				iNumDots2 = iNumDots2 + die_1 + die_2
			end
			plot_data = CreateAxisChainWithDoubleDots(iWidth, iHeight, fTilt, iInnerWidth1, iInnerHeight1, iInnerWest1, iInnerSouth1,
                                                      iNumDots1, iInnerWidth2, iInnerHeight2, iInnerWest2, iInnerSouth2, iNumDots2)

		elseif formation_type == 9 then -- Horizontal Block 3x2 With Double Dots
			local x_shrinkage = Map.Rand(8, "Cell Width adjustment - Lua")
			if x_shrinkage > 5 then
				x_shrinkage = 0
			end
			local y_shrinkage = Map.Rand(5, "Cell Height adjustment - Lua")
			if y_shrinkage > 3 then
				y_shrinkage = 0
			end
			x_shift, y_shift = 0, 0
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua")
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua")
			end
			iWidth = iCellWidth * 3 - x_shrinkage
			iHeight = iCellHeight * 2 - y_shrinkage
			-- Limit angles to mostly horizontal ones.
			fTilt = 145 + Map.Rand(90, "Angle for island chain axis - LUA")
			if fTilt > 180 then
				fTilt = fTilt - 180
			end
			-- Handle Dots Group 1.
			local iInnerWidth1 = iCellWidth
			local iInnerHeight1 = iCellHeight - 2
			local iInnerWest1 = 4 + Map.Rand(iCellWidth + 2, "Shift for sub island group - Lua")
			local iInnerSouth1 = 3 + Map.Rand(iCellHeight - 2, "Shift for sub island group - Lua")
			-- Determine number of dots
			local die_1 = Map.Rand(4, "Diceroll - Lua")
			local die_2 = Map.Rand(8, "Diceroll - Lua")
			local iNumDots1 = 9
			if die_1 + die_2 > 1 then
				iNumDots1 = iNumDots1 + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua")
			else
				iNumDots1 = iNumDots1 + die_1 + die_2
			end
			-- Handle Dots Group 2.
			local iInnerWidth2 = iCellWidth
			local iInnerHeight2 = iCellHeight - 2
			local iInnerWest2 = 4 + Map.Rand(iCellWidth + 2, "Shift for sub island group - Lua")
			local iInnerSouth2 = 3 + Map.Rand(iCellHeight - 2, "Shift for sub island group - Lua")
			-- Determine number of dots
			local die_1 = Map.Rand(5, "Diceroll - Lua")
			local die_2 = Map.Rand(7, "Diceroll - Lua")
			local iNumDots2 = 8
			if die_1 + die_2 > 1 then
				iNumDots2 = iNumDots2 + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua")
			else
				iNumDots2 = iNumDots2 + die_1 + die_2
			end
			plot_data = CreateAxisChainWithDoubleDots(iWidth, iHeight, fTilt, iInnerWidth1, iInnerHeight1, iInnerWest1, iInnerSouth1,
                                                      iNumDots1, iInnerWidth2, iInnerHeight2, iInnerWest2, iInnerSouth2, iNumDots2)

		elseif formation_type == 10 then -- Vertical Block 2x3 With Double Dots
			local x_shrinkage = Map.Rand(6, "Cell Width adjustment - Lua")
			if x_shrinkage > 4 then
				x_shrinkage = 0
			end
			local y_shrinkage = Map.Rand(8, "Cell Height adjustment - Lua")
			if y_shrinkage > 5 then
				y_shrinkage = 0
			end
			x_shift, y_shift = 0, 0
			if x_shrinkage > 0 then
				x_shift = Map.Rand(x_shrinkage, "Cell Width offset - Lua")
			end
			if y_shrinkage > 0 then
				y_shift = Map.Rand(y_shrinkage, "Cell Height offset - Lua")
			end
			iWidth = iCellWidth * 2 - x_shrinkage
			iHeight = iCellHeight * 3 - y_shrinkage
			-- Mostly vertical
			fTilt = 55 + Map.Rand(71, "Angle for island chain axis - LUA")
			-- Handle Dots Group 1.
			local iInnerWidth1 = iCellWidth - 2
			local iInnerHeight1 = iCellHeight
			local iInnerWest1 = 3 + Map.Rand(iCellWidth - 2, "Shift for sub island group - Lua")
			local iInnerSouth1 = 4 + Map.Rand(iCellHeight + 2, "Shift for sub island group - Lua")
			-- Determine number of dots
			local die_1 = Map.Rand(4, "Diceroll - Lua")
			local die_2 = Map.Rand(10, "Diceroll - Lua")
			local iNumDots1 = 8
			if die_1 + die_2 > 1 then
				iNumDots1 = iNumDots1 + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua")
			else
				iNumDots1 = iNumDots1 + die_1 + die_2
			end
			-- Handle Dots Group 2.
			local iInnerWidth2 = iCellWidth - 2
			local iInnerHeight2 = iCellHeight
			local iInnerWest2 = 3 + Map.Rand(iCellWidth - 2, "Shift for sub island group - Lua")
			local iInnerSouth2 = 4 + Map.Rand(iCellHeight + 2, "Shift for sub island group - Lua")
			-- Determine number of dots
			local die_1 = Map.Rand(4, "Diceroll - Lua")
			local die_2 = Map.Rand(8, "Diceroll - Lua")
			local iNumDots2 = 7
			if die_1 + die_2 > 1 then
				iNumDots2 = iNumDots2 + Map.Rand(die_1 + die_2, "Number of dots to add to island chain - Lua")
			else
				iNumDots2 = iNumDots2 + die_1 + die_2
			end
			plot_data = CreateAxisChainWithDoubleDots(iWidth, iHeight, fTilt, iInnerWidth1, iInnerHeight1, iInnerWest1, iInnerSouth1,
                                                      iNumDots1, iInnerWidth2, iInnerHeight2, iInnerWest2, iInnerSouth2, iNumDots2)
		end

		-- Obtain land plots from the plot data
		local x_adjust = (cell_x - 1) * iCellWidth + x_shift
		local y_adjust = (cell_y - 1) * iCellHeight + y_shift
		for y = 1, iHeight do
			for x = 1, iWidth do
				local data_index = (y - 1) * iWidth + x
				if plot_data[data_index] == true then -- This plot is land.
					local real_x, real_y = x + x_adjust - 1, y + y_adjust - 1
					local plot_index = real_y * iW + real_x + 1
					island_chain_PlotTypes[plot_index] = PlotTypes.PLOT_LAND
				end
			end
		end

		-- Record cells in use
		if formation_type == 1 then -- single cell
			cell_data[anchor_cell] = true
			iNumCellsInUse = iNumCellsInUse + 1
		elseif formation_type == 2 then
			cell_data[anchor_cell], cell_data[anchor_cell + 1] = true, true
			iNumCellsInUse = iNumCellsInUse + 2
		elseif formation_type == 3 then
			cell_data[anchor_cell] = true
			iNumCellsInUse = iNumCellsInUse + 1
		elseif formation_type == 4 then
			cell_data[anchor_cell], cell_data[anchor_cell + 1] = true, true
			iNumCellsInUse = iNumCellsInUse + 2
		elseif formation_type == 5 then
			cell_data[anchor_cell], cell_data[anchor_cell + iNumCellColumns] = true, true
			iNumCellsInUse = iNumCellsInUse + 2
		elseif formation_type == 6 then
			cell_data[anchor_cell], cell_data[anchor_cell + iNumCellColumns] = true, true
			iNumCellsInUse = iNumCellsInUse + 2
		elseif formation_type == 7 then
			cell_data[anchor_cell], cell_data[anchor_cell + iNumCellColumns] = true, true
			cell_data[anchor_cell + (iNumCellColumns * 2)] = true
			iNumCellsInUse = iNumCellsInUse + 3
		elseif formation_type == 8 then
			cell_data[anchor_cell], cell_data[anchor_cell + 1] = true, true
			cell_data[anchor_cell + iNumCellColumns], cell_data[anchor_cell + iNumCellColumns + 1] = true, true
			iNumCellsInUse = iNumCellsInUse + 4
		elseif formation_type == 9 then
			cell_data[anchor_cell], cell_data[anchor_cell + 1] = true, true
			cell_data[anchor_cell + iNumCellColumns], cell_data[anchor_cell + iNumCellColumns + 1] = true, true
			cell_data[anchor_cell + 2], cell_data[anchor_cell + iNumCellColumns + 2] = true, true
			iNumCellsInUse = iNumCellsInUse + 6
		elseif formation_type == 10 then
			cell_data[anchor_cell], cell_data[anchor_cell + 1] = true, true
			cell_data[anchor_cell + iNumCellColumns], cell_data[anchor_cell + iNumCellColumns + 1] = true, true
			cell_data[anchor_cell + (iNumCellColumns * 2)], cell_data[anchor_cell + (iNumCellColumns * 2) + 1] = true, true
			iNumCellsInUse = iNumCellsInUse + 6
		end
	end

	-- Debug check of cell occupation.
	--print("- - -")
	for loop = iNumCellRows, 1, -1 do
		local c = (loop - 1) * iNumCellColumns
		local stringdata = {}
		for innerloop = 1, iNumCellColumns do
			if cell_data[c + innerloop] == false then
				stringdata[innerloop] = "false"
			else
				stringdata[innerloop] = "true "
			end
		end
		--print("Row: ", table.concat(stringdata))
	end
	--

	-- Add Hills and Peaks to randomly generated islands.
	local regionHillsFrac = Fractal.Create(iW, iH, 5, {}, 7, 7)
	local regionPeaksFrac = Fractal.Create(iW, iH, 6, {}, 7, 7)
	local iHillsBottom1 = regionHillsFrac:GetHeight(20)
	local iHillsTop1 = regionHillsFrac:GetHeight(35)
	local iHillsBottom2 = regionHillsFrac:GetHeight(65)
	local iHillsTop2 = regionHillsFrac:GetHeight(80)
	local iPeakThreshold = regionPeaksFrac:GetHeight(80)
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local i = y * iW + x + 1
			if island_chain_PlotTypes[i] ~= PlotTypes.PLOT_OCEAN then
				local hillVal = regionHillsFrac:GetHeight(x,y)
				if ((hillVal >= iHillsBottom1 and hillVal <= iHillsTop1) or (hillVal >= iHillsBottom2 and hillVal <= iHillsTop2)) then
					local peakVal = regionPeaksFrac:GetHeight(x,y)
					if (peakVal >= iPeakThreshold) then
						island_chain_PlotTypes[i] = PlotTypes.PLOT_MOUNTAIN
					else
						island_chain_PlotTypes[i] = PlotTypes.PLOT_HILLS
					end
				end
			end
		end
	end

	-- Apply island data to the map.
	for y = 2, iH - 3 do -- avoid polar caps
		for x = 0, iW - 1 do
			local i = y * iW + x + 1
			local plot = Map.GetPlot(x, y)
			if island_chain_PlotTypes[i] ~= PlotTypes.PLOT_OCEAN and Plot_IsWater(plot, true) then
				local isValid = true
				local numAdjacentLand = 0
				-- Don't fill river deltas with land
				for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
					if nearPlot:GetPlotType() ~= PlotTypes.PLOT_OCEAN and nearPlot:Area():GetNumTiles() >= 10 then
						numAdjacentLand = numAdjacentLand + 1
						if numAdjacentLand > 1 then
							isValid = false
							break
						end
					end
				end
				if isValid then
					plot:SetPlotType(island_chain_PlotTypes[i], false, false)
				else
					--plot:SetPlotType(PlotTypes.PLOT_MOUNTAIN, false, false)
				end
			end
		end
	end
end

function GetTemperature(myPlot)
	if myPlot:GetTerrainType() == TerrainTypes.TERRAIN_SNOW then
		return 0
	elseif myPlot:GetTerrainType() == TerrainTypes.TERRAIN_TUNDRA then
		return 1
	end
	return 2
end

function BlendTerrain()
	local mapW, mapH = Map.GetGridSize()
	local landPlots = {}

	local mountainCheckTime = 0
	for plotID, plot in Plots(Shuffle) do
		if Plot_IsWater(plot) then
			--
		else
			local plotTerrainID = plot:GetTerrainType()
			local plotFeatureID = plot:GetFeatureType()
			local plotPercent = Plot_GetCirclePercents(plot, 1, mg.terrainBlendRange)
			local randPercent = 1 + PWRand() * 2 * mg.terrainBlendRandom - mg.terrainBlendRandom

			if plot:IsMountain() then
				-- minimize necessary pathfinding
				local numNearMountains = 0
				for nearPlot in Plot_GetPlotsInCircle(plot, 1, 1) do
					if nearPlot:IsMountain() then
						numNearMountains = numNearMountains + 1
					end
				end
				if debugTime then timeStart = os.clock() end
				if 2 <= numNearMountains and numNearMountains <= 4 then
					CreatePossibleMountainPass(plot)
				end
				if debugTime then mountainCheckTime = mountainCheckTime + (os.clock() - timeStart) end
			else
				if plotTerrainID == TerrainTypes.TERRAIN_GRASS then
					if plotPercent.TERRAIN_DESERT + plotPercent.TERRAIN_SNOW >= 0.33 * randPercent then
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, true, true)
						if plot:GetFeatureType() == FeatureTypes.FEATURE_MARSH then
							plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
						end
					end
				elseif plotTerrainID == TerrainTypes.TERRAIN_PLAINS then
--					if plotPercent.TERRAIN_DESERT >= 0.5 * randPercent then
--						--plot:SetTerrainType(TerrainTypes.TERRAIN_DESERT, true, true)
--					end
				elseif plotTerrainID == TerrainTypes.TERRAIN_DESERT then
					if plotPercent.TERRAIN_GRASS + plotPercent.TERRAIN_SNOW >= 0.25 * randPercent then
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, true, true)
					elseif plotPercent.FEATURE_JUNGLE + plotPercent.FEATURE_MARSH >= 0.25 * randPercent then
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, true, true)
					end
				elseif plotFeatureID == FeatureTypes.FEATURE_JUNGLE or plotFeatureID == FeatureTypes.FEATURE_MARSH or plotFeatureID == FeatureTypes.FEATURE_FOREST then
					if plotPercent.TERRAIN_SNOW + plotPercent.TERRAIN_TUNDRA + plotPercent.TERRAIN_DESERT >= 0.25 * randPercent then
						plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1)
					end
				elseif plotTerrainID == TerrainTypes.TERRAIN_TUNDRA then
					if 2 * plotPercent.TERRAIN_GRASS + plotPercent.TERRAIN_PLAINS + plotPercent.TERRAIN_DESERT >= 0.5 * randPercent then
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, true, true)
					end
				end
			end
			if plotTerrainID == TerrainTypes.TERRAIN_SNOW then
				local isMountain = plot:IsMountain()
				local warmCount = 2 * plotPercent.FEATURE_JUNGLE + 2 * plotPercent.FEATURE_MARSH + plotPercent.TERRAIN_GRASS + plotPercent.TERRAIN_DESERT + 0.5 * plotPercent.TERRAIN_PLAINS
				if warmCount >= 0.25 * randPercent then
					plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, true, true)
				elseif warmCount >= 0.10 * randPercent or plot:IsFreshWater() then
					plot:SetTerrainType(TerrainTypes.TERRAIN_TUNDRA, true, true)
				end
				if isMountain then
					plot:SetPlotType(PlotTypes.PLOT_MOUNTAIN, false, false)
				end
			end
			if plot:IsHills() then
				-- warm hills -> flat when surrounded by cold
				local nearCold = 0
				for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
					if not nearPlot:IsWater() and not IsMountain(nearPlot) and GetTemperature(nearPlot) < GetTemperature(plot) then
						nearCold = nearCold + 1
					end
				end
				if nearCold > 1 and (nearCold * mg.hillsBlendPercent * 100) >= Map.Rand(100, "Blend hills - Lua") then
					plot:SetPlotType(PlotTypes.PLOT_LAND, false, false)
				end
			end
		end
	end
	if debugTime then print(string.format("%5s ms, BlendTerrain %s", math.floor(mountainCheckTime * 1000), "MountainCheckTime")) end

	-- flat -> hills near mountain, and flat cold -> hills when surrounded by warm
	-- add flat desert -> hills when surrounded by flat desert too
	for plotID, plot in Plots(Shuffle) do
		if plot:GetPlotType() == PlotTypes.PLOT_LAND then
			local nearMountains = 0
			local nearWarm = 0
			local nearDesert = 0
			for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
				if not nearPlot:IsWater() then
					local nearTerrainID = nearPlot:GetTerrainType()

					if IsMountain(nearPlot) then
						nearMountains = nearMountains + 1
					end

					if GetTemperature(nearPlot) > GetTemperature(plot) then
						nearWarm = nearWarm + 1
					end
					
					if plot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT and nearPlot:GetPlotType() == PlotTypes.PLOT_LAND and nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
						nearDesert = nearDesert + 1
					end
				end
			end
			if (nearMountains > 1 and (nearMountains * mg.hillsBlendPercent * 10000) >= Map.Rand(10000, "Blend mountains - Lua")) then
				--print("Turning flatland near mountain into hills", nearMountains)
				plot:SetPlotType(PlotTypes.PLOT_HILLS, false, false)
				--plot:SetTerrainType(TerrainTypes.TERRAIN_SNOW, false, false)
			elseif nearWarm * 0.5 * mg.hillsBlendPercent * 10000 >= Map.Rand(10000, "Blend hills - Lua") then
				--print("Turning flatland near warm into hills", nearWarm)
				plot:SetPlotType(PlotTypes.PLOT_HILLS, false, false)
			elseif nearDesert * 0.5 * mg.hillsBlendPercent * 10000 >= Map.Rand(10000, "Blend hills - Lua") then
				--print("Turning flat desert surrounded by flat desert into hills", nearDesert)
				plot:SetPlotType(PlotTypes.PLOT_HILLS, false, false)
			end
		end
	end
end

function CreatePossibleMountainPass(plot)
	local x, y = plot:GetX(), plot:GetY()
	if not plot:IsMountain() then
		return
	end

	local longestRoute = 0
	for dirA = 0, 3 do
		plotA = Map.PlotDirection(x, y, dirA)
		if plotA and (plotA:GetPlotType() == PlotTypes.PLOT_LAND or plotA:GetPlotType() == PlotTypes.PLOT_HILLS) then
			for dirB = dirA+2, 5 do
				local plotB = Map.PlotDirection(x, y, dirB)
				if plotB and (plotB:GetPlotType() == PlotTypes.PLOT_LAND or plotB:GetPlotType() == PlotTypes.PLOT_HILLS) then
					local isConnected = isPlotConnected(nil, plotA, plotB, "Land", true)
					if longestRoute < getRouteLength() then
						longestRoute = getRouteLength()
					end
					if (getRouteLength() == 0 or getRouteLength() > 15) then
						print(string.format("CreatePossibleMountainPass path distance = %2s - Change to Hills", getRouteLength()))
						plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true)
						--plot:SetFeatureType(FeatureTypes.FEATURE_FALLOUT, -1) -- debug

						table.insert(mg.MountainPasses, plot) -- still consider this a mountain tile for art purposes
						return
					end
				end
			end
		end
	end
	--print(string.format("CreatePossibleMountainPass longestRoute = %2s", longestRoute))
end


function CreateArcticOceans()
	local mapW, mapH = Map.GetGridSize()
	CreateOceanRift{y = 0,  	direction = mg.E, totalSize = 1, oceanSize = 1, fill = true}
	CreateOceanRift{y = mapH-1, direction = mg.E, totalSize = 1, oceanSize = 1, fill = true}
end

function CreateVerticalOceans()
	local oOceanRifts = Map.GetCustomOption(6)
	if oOceanRifts == 7 then oOceanRifts = 1 + Map.Rand(6, "Random Ocean Rifts") end
	local mapW, mapH = Map.GetGridSize()
	if oOceanRifts == 6 then
		-- No vertical rifts
		return
	end

	log:Info("CreateVerticalOceans mg.oceanRiftWidth = %s", mg.oceanRiftWidth)

	local landInColumn = {}
	for x = 0, mapW - 1 do
		landInColumn[x]			= 0
		mg.elevationRect[x]		= {}
		mg.elevationRect[x][-1]	= 0
		for y = 0, mapH - 1 do
			mg.elevationRect[x][y] = 0
		end
	end

	-- scan plots
	local totalLand = 0
	for x = 0, mapW - 1 do
		for y = 0, mapH - 1 do
			plot = Map.GetPlot(x, y)

			if not Plot_IsWater(plot, true) then
				totalLand = totalLand + 1
				landInColumn[x] = landInColumn[x] + 1
			end

			-- elevation in a size-wide rectangle
			local elevation = Plot_GetElevation(plot, false)
			for x2 = 0 - math.ceil(2 + mg.oceanRiftWidth), 1 + math.floor(2 + mg.oceanRiftWidth) do
				xOffset = (x + x2) % mapW
				mg.elevationRect[xOffset][-1] = mg.elevationRect[xOffset][-1] + elevation
				mg.elevationRect[xOffset][y] = mg.elevationRect[xOffset][y] + elevation
			end
		end
	end

	-- find biggest ocean (usually Pacific)
	local lowestElevation	= 0
	local startX			= 0
	local startY			= math.floor(mapH/2)
	for x = 0, mapW - 1 do
		local elevation = mg.elevationRect[xOffset][-1]
		if elevation < lowestElevation then
			lowestElevation = elevation
			startX = x
		end
	end
	print("startX pacific:", startX)

	table.insert(mg.oceanRiftMidlines, startX)
	if mapW < 60 then
		log:Debug("CreateVerticalOceans: Creating Atlantic at x=%s", startX)
		CreateAtlantic(startX)
		return
	elseif oOceanRifts == 2 or oOceanRifts == 3 then
		-- PA or PP
		log:Debug("CreateVerticalOceans: Creating Pacific  at x=%s", startX)
		CreatePacific(startX)
	elseif oOceanRifts == 1 then
		-- AA
		log:Debug("CreateVerticalOceans: Creating Atlantic at x=%s", startX)
		CreateAtlantic(startX)
	elseif oOceanRifts == 4 or oOceanRifts == 5 then
		-- 1 or 2 random
		if 50 >= Map.Rand(100, "Random ocean rift - Lua") then
			log:Debug("CreateVerticalOceans: Creating Pacific  at x=%s (Random)", startX)
			CreatePacific(startX)
		else
			log:Debug("CreateVerticalOceans: Creating Atlantic at x=%s (Random)", startX)
			CreateAtlantic(startX)
		end
	end


	if oOceanRifts == 5 then
		-- Only one rift
		return
	end

	-- find median land (usually place for Atlantic)
	local startX	= 0
	local sumLand	= 0
	local offsetAtlanticPercent	= (0 == Map.Rand(2, "Atlantic Offset - Lua")) and mg.offsetAtlanticPercent or 1 - mg.offsetAtlanticPercent
	for x = 0, mapW - 1 do
		local xOffset = (x + startX) % mapW
		sumLand = sumLand + landInColumn[xOffset]
		if sumLand > offsetAtlanticPercent * totalLand then
			startX = xOffset
			print("startX atlantic:", startX)
			break
		end
	end

	log:Debug("totalLand=%4f sumElevation=%4f", totalLand, sumLand)

	table.insert(mg.oceanRiftMidlines, startX)
	if oOceanRifts == 1 or oOceanRifts == 2 then
		-- PA or AA
		log:Debug("CreateVerticalOceans: Creating Atlantic at x=%s", startX)
		CreateAtlantic(startX)
	elseif oOceanRifts == 3 then
		-- PP
		log:Debug("CreateVerticalOceans: Creating Pacific  at x=%s", startX)
		CreatePacific(startX)
	elseif oOceanRifts == 4 then
		-- 2 random
		if 50 >= Map.Rand(100, "Random ocean rift - Lua") then
			log:Debug("CreateVerticalOceans: Creating Pacific  at x=%s (Random)", startX)
			CreatePacific(startX)
		else
			log:Debug("CreateVerticalOceans: Creating Atlantic at x=%s (Random)", startX)
			CreateAtlantic(startX)
		end
	end

end

function CreatePacific(midline)
	CreateOceanRift{x = midline, totalSize = mg.pacificSize, bulge = mg.pacificBulge, curve = mg.pacificCurve, oceanSize = math.max(1, Round(mg.oceanRiftWidth - 1))}
	-- CreateOceanRift{x = midline, totalSize = 3, bulge = 3, curve = 0, oceanSize = 2}
	print("Create Pacific at: ", midline)
end
function CreateAtlantic(midline)
	CreateOceanRift{x = midline, totalSize = mg.atlanticSize, bulge = mg.atlanticBulge, curve = mg.atlanticCurve, oceanSize = mg.oceanRiftWidth, cleanMid = true}
	print("Create Atlantic at: ", midline)
end


function CreateOceanRift(args)
	log:Debug("CreateOceanRift")
	for k, v in pairs(args) do
		log:Debug("%s = %s", k, v)
	end
	local x			= args.x or 0
	local y			= args.y or 0
	local midline	= args.midline or x
	local direction	= args.direction or mg.N
	local totalSize	= args.totalSize or 3
	local oceanSize	= args.oceanSize or mg.oceanRiftWidth
	local bulge		= args.bulge or 0
	local curve		= args.curve or 0
	local fill		= args.fill
	local cleanMid	= args.cleanMid

	local plots = {}
	if bulge ~= 0 then
		-- see which curve direction fits the land better
			  plots  = GetRiftPlots(x-2, midline-2, y, direction, totalSize, oceanSize, bulge, curve)
		local plotsB = GetRiftPlots(x+2, midline+2, y, direction, totalSize, oceanSize, bulge, curve)

		if GetMatchingPlots(plotsB) > GetMatchingPlots(plots) then
			plots = DeepCopy(plotsB)
		end
	elseif curve ~= 0 then
		-- see which curve direction fits the land better
			  plots  = GetRiftPlots(x-1, midline-1, y, direction, totalSize, oceanSize, bulge, curve)
		local plotsB = GetRiftPlots(x+1, midline+1, y, direction, totalSize, oceanSize, bulge, -1 * curve)

		if GetMatchingPlots(plotsB) > GetMatchingPlots(plots) then
			plots = DeepCopy(plotsB)
		end
	else
		plots = GetRiftPlots(x, midline, y, direction, totalSize, oceanSize, bulge, curve)
	end

	for plotID, v in pairs(plots) do
		log:Trace("oceanRiftPlots %s, %s, %s", v.plot:GetX(), v.plot:GetY(), v.strip)
		if (fill and v.strip <= 0) or IsBetween(0, v.strip, oceanSize) then
			mg.oceanRiftPlots[plotID] = {
				isWater = true,
				terrainID = TerrainTypes.TERRAIN_OCEAN
			}
		elseif v.strip == -1 or v.strip == oceanSize+1 then
			if (cleanMid and v.strip <= 0) then
				mg.oceanRiftPlots[plotID] = {
					isWater = true, --true
					terrainID = TerrainTypes.TERRAIN_OCEAN
				}
			elseif 80 >= Map.Rand(100, "Ocean rift ocean/coast - Lua") then
				mg.oceanRiftPlots[plotID] = {
					isWater = true,
					terrainID = TerrainTypes.TERRAIN_OCEAN
				}
			else
				mg.oceanRiftPlots[plotID] = mg.oceanRiftPlots[plotID] or {
					isWater = true,
					terrainID = TerrainTypes.TERRAIN_COAST
				}
			end
		elseif v.strip == -2 or v.strip == oceanSize+2 then
			if 50 >= Map.Rand(100, "Ocean rift coast/land - Lua") then
				if (cleanMid and v.strip <= 0) then
					mg.oceanRiftPlots[plotID] = {
						isWater = true,
						terrainID = TerrainTypes.TERRAIN_OCEAN
					}
				else
					mg.oceanRiftPlots[plotID] = mg.oceanRiftPlots[plotID] or {
						isWater = true,
						terrainID = TerrainTypes.TERRAIN_COAST
					}
				end
			else
				mg.oceanRiftPlots[plotID] = mg.oceanRiftPlots[plotID] or {
					isWater = false,
					terrainID = TerrainTypes.TERRAIN_COAST
				}
			end
		else
		end
	end


	function GetMatchingPlots(plots)
		local nicePlots = 0
		for plotID, v in pairs(plots) do
			if (fill and v.strip <= 0) or IsBetween(0, v.strip, oceanSize) or v.strip == -1 or v.strip == oceanSize+1 then
				-- turns plots to water
				if Plot_IsWater(v.plot, true) then
					nicePlots = nicePlots + 1
				else
					nicePlots = nicePlots - 1
				end
			elseif v.strip == -2 or v.strip == oceanSize+2 then
				-- 50% chance turns to water
				if Plot_IsWater(v.plot, true) then
					nicePlots = nicePlots + 1 -- okay if already water
				else
					nicePlots = nicePlots - 0.5 -- destroys land half the time
				end
			else
				-- encourage land in center area
				if Plot_IsWater(v.plot, true) then
					nicePlots = nicePlots - 0.1
				else
					nicePlots = nicePlots + 0.1
				end
			end
		end
		return nicePlots
	end
end

function GetRiftPlots(x, midline, y, direction, totalSize, oceanSize, bulge, curve)
	local mapW, mapH = Map.GetGridSize()
	local riftPlots = {}

	log:Debug("x=%-3s, y=%-3s Creating %s ocean rift with midline %s curve %s on map size (%s, %s)", x, y, mg.directionNames[direction], midline, curve, mapW, mapH)

	local nextDirA		= 0
	local nextDirB		= 0

	local curveNormal = (50 >= Map.Rand(100, "Ocean Rift Curve - Lua"))

	if direction == mg.N then
		nextDirA = mg.NE
		nextDirB = mg.NW
	elseif direction == mg.S then
		nextDirA = mg.SE
		nextDirB = mg.SW
	elseif direction == mg.E then
		nextDirA = mg.E
		nextDirB = mg.E
	elseif direction == mg.W then
		nextDirA = mg.W
		nextDirB = mg.W
	else
		print("Invalid direction %s for CreateOceanRift")
		return
	end


	plot = Map.GetPlot(x, y)
	local attempts = 0
	while plot and attempts < mapW do
		local foundNewPlots	= false
		local plotID		= Plot_GetID(plot)
		local radius		= math.max(0, Round((totalSize-1)/2 + bulge/2 * GetBellCurve(y, mapH)))
		local extraRadius	= oceanSize

		if direction == mg.E or direction == mg.W then
			log:Trace("x=%-3s, y=%-3s radius=%-3s extraRadius=%-3s", x, y, radius, extraRadius)
		else
			log:Trace("x=%-3s, y=%-3s radius=%-3s extraRadius=%-3s midX=%-3s", x, y, radius, extraRadius, midline)
		end

		for nearPlot, nearDistance in Plot_GetPlotsInCircle(plot, 0, radius + extraRadius) do
			if not mg.oceanRiftPlots[nearPlotID] then
				local nearPlotID = Plot_GetID(nearPlot)
				if not riftPlots[nearPlotID] then
					riftPlots[nearPlotID] = {plot=nearPlot, minDistance=999, strip=0}
				end
				if nearDistance < riftPlots[nearPlotID].minDistance then
					foundNewPlots = true
					riftPlots[nearPlotID].minDistance = nearDistance
					riftPlots[nearPlotID].strip = nearDistance - radius
				end
			end
		end

		local nextPlotA  	= Map.PlotDirection(x, y, nextDirA)
		local nextPlotB 	= Map.PlotDirection(x, y, nextDirB)

		if not nextPlotA or not nextPlotB then
			-- reached edge of map
			--print("End ocean rift")
			return riftPlots
		end


		local oddsA = 0.50
		if nextPlotA == nextPlotB then
			if 0.5 > PWRand() then
				plot = nextPlotA
			else
				plot = nextPlotB
			end
		else
			local distanceA = Map.PlotDistance(nextPlotA:GetX(), y, midline, y)
			local distanceB = Map.PlotDistance(nextPlotB:GetX(), y, midline, y)
			local nextElevationA = mg.elevationRect[nextPlotA:GetX()][nextPlotA:GetY()]
			local nextElevationB = mg.elevationRect[nextPlotB:GetX()][nextPlotB:GetY()]
			--print("nextElevationA = ", nextElevationA, " nextElevationB = ", nextElevationB)

			oddsA = nextElevationB / (nextElevationA + nextElevationB) -- Inversely proportional
			oddsA = math.min(0.9, math.max(0.1, oddsA)) -- Neither too big or too low
			oddsA = oddsA * (distanceB / (distanceA + distanceB)) ^ GetBellCurve(y, mapH)

			--print("oddsA = ", oddsA)

			local randomPercent = PWRand() -- 0.5
			log:Trace("distance A=%s B=%s oddsA=%.2f rand=%.2f", distanceA, distanceB, Round(oddsA, 2), Round(randomPercent, 2))
			if oddsA >= randomPercent then
				plot = nextPlotA
				log:Trace("choose A")
			else
				plot = nextPlotB
				log:Trace("choose B")
			end
			--print("nextPlotA = ", nextPlotA:GetX(), " - ", nextPlotA:GetY())
			--print("nextPlotB = ", nextPlotB:GetX(), " - ", nextPlotB:GetY())
			--print("plot = ", plot:GetX(), " - ", plot:GetY())
		end

		x = plot:GetX()
		y = plot:GetY()
		attempts = attempts + 1
	end
	return riftPlots
end

-- Creates an horizontal passage (by tu_79)
function CreateMagallanes()
	-- Don't do this for Terra maps or if the option is turned off
	if Map.GetCustomOption(5) == 1 or Map.GetCustomOption(8) == 2 then
		return
	end

	local mapW, mapH = Map.GetGridSize()
	local x = 0
	local y = 0
	local startY = 0

	log:Info("CreateMagallanes mg.oceanRiftWidth = %s", mg.oceanRiftWidth)

	local landInRow = {}
	for y = 0, mapH - 1 do
		landInRow[y]			= 0
	end

	-- scan plots
	local totalLand = mapW - 1
	for y = Round(mapH / 5), Round(4 * mapH / 5) - 1 do
		for x = 0, mapW - 1 do
			plot = Map.GetPlot(x, y)

			if not Plot_IsWater(plot, true) then
				landInRow[y] = landInRow[y] + 1
			end
		end
		if landInRow[y] < totalLand then
			totalLand = landInRow[y]
			startY = y
		end
	end

	table.insert(mg.oceanRiftMidlines, startY)

	local direction	= mg.E
	local totalSize	= 1
	local oceanSize	= 0
	local bulge		= 0
	local curve		= 0.1

	local plots = {}

	plots = GetMagallanesPlots(x, y, startY, direction, totalSize, oceanSize, bulge, curve)

	for plotID, v in pairs(plots) do
		log:Trace("oceanRiftPlots %s, %s, %s", v.plot:GetX(), v.plot:GetY(), v.strip)
		if IsBetween(0, v.strip, oceanSize) then
			mg.oceanRiftPlots[plotID] = {
				isWater = true,
				terrainID = TerrainTypes.TERRAIN_OCEAN
			}
		elseif v.strip == -1 or v.strip == oceanSize+1 then
			if 80 >= Map.Rand(100, "Ocean rift ocean/coast - Lua") then
				mg.oceanRiftPlots[plotID] = {
					isWater = true,
					terrainID = TerrainTypes.TERRAIN_COAST
				}
			else
				mg.oceanRiftPlots[plotID] = mg.oceanRiftPlots[plotID] or {
					isWater = false,
					terrainID = TerrainTypes.TERRAIN_COAST
				}
			end
		end
	end
end

function GetMagallanesPlots(x, y, startY, direction, totalSize, oceanSize, bulge, curve)
	local mapW, mapH = Map.GetGridSize()
	local riftPlots = {}

	log:Debug("x=%-3s, y=%-3s Creating %s ocean rift with midline %s curve %s on map size (%s, %s)", x, y, mg.directionNames[direction], midline, curve, mapW, mapH)

	local nextDirA		= 0
	local nextDirB		= 0
	local nextDirC		= 0
	local nextPlotA		= {}
	local nextPlotB		= {}
	local nextPlotC		= {}
	local direction 	= direction or mg.E

	local curveNormal = (50 >= Map.Rand(100, "Ocean Rift Curve - Lua"))

	if direction == mg.SE then
		nextDirA = mg.E
		nextDirB = mg.E
		nextDirC = mg.E
	elseif direction == mg.NE then
		nextDirA = mg.E
		nextDirB = mg.E
		nextDirC = mg.E
	elseif direction == mg.E then
		nextDirA = mg.NE
		nextDirB = mg.E
		nextDirC = mg.SE
	else
		print("Invalid direction %s for CreateOceanRift")
		return
	end
	--print("direction: ", direction)
	--print("nextDirA: ", nextDirA, "nextDirB: ", nextDirB, "nextDirC: ", nextDirC)
	--print("startY ", startY)
	y = startY

	plot = Map.GetPlot(x, y)
	local attempts = 0
	local foundNewPlots = true
	local magallanesPlots = {}

	while plot and attempts < (mapW + mapH) do
		if plot:GetX() == mapW - 1 then
			print("Reached the map end")
			return riftPlots
		end
		--print("X:", plot:GetX(), " Y:", plot:GetY(), "StartY:", startY)
		foundNewPlots		= false
		local plotID		= Plot_GetID(plot)
		local radius		= math.max(0, Round((totalSize-1)/2 + bulge/2 * GetBellCurve(x, mapW)))
		local extraRadius	= oceanSize + 2

		for nearPlot, nearDistance in Plot_GetPlotsInCircle(plot, 0, radius + extraRadius) do
			if not magallanesPlots[nearPlotID] then
				local nearPlotID = Plot_GetID(nearPlot)
				if not riftPlots[nearPlotID] then
					riftPlots[nearPlotID] = {plot=nearPlot, minDistance=999, strip=0}
				end
				if nearDistance < riftPlots[nearPlotID].minDistance then
					foundNewPlots = true
					--print("found new magallanes plots")
					magallanesPlots[nearPlotID] = true
					riftPlots[nearPlotID].minDistance = nearDistance
					riftPlots[nearPlotID].strip = nearDistance - radius
				end
			end
		end

		if 0 < plot:GetY() and plot:GetY() < mapH -1 then
			nextPlotA  		= Map.PlotDirection(x, y, nextDirA)
			nextPlotB 		= Map.PlotDirection(x, y, nextDirB)
			nextPlotC		= Map.PlotDirection(x, y, nextDirC)
			--print("nextA:", nextPlotA:GetY(), "nextB:", nextPlotB:GetY(), "nextC:", nextPlotC:GetY())
			if not nextPlotA or not nextPlotB or not nextPlotC then
				-- reached edge of map
					print("End magallanes rift")
					return riftPlots
			end
		else
			nextPlotB 		= Map.PlotDirection(x, y, nextDirB)
			nextPlotA		= nextPlotB
			nextPlotC		= nextPlotB
		end
		--print("A:", nextPlotA:GetY(), "B:", nextPlotB:GetY(), "C:", nextPlotC:GetY())

		local oddsA = 0
		local oddsB = 0
		local oddsC = 0
		if nextPlotA == nextPlotB and nextPlotA == nextPlotC then
			plot = nextPlotA
		else
			local distanceA = Map.PlotDistance(x, nextPlotA:GetY(), x, startY)
			local distanceB = Map.PlotDistance(x, nextPlotB:GetY(), x, startY)
			local distanceC = Map.PlotDistance(x, nextPlotC:GetY(), x, startY)
			local totalDistance = distanceA + distanceB + distanceC
			distanceA = distanceA / totalDistance
			distanceB = distanceB / totalDistance
			distanceC = distanceC / totalDistance

			local nextElevationA = 0
			local nextElevationB = 0
			local nextElevationC = 0
							--Consider only the next 10% of the map.
			for i = x, math.min(mapW - 1, x + Round(mapW/10)) do
				local horizontalBand = Round(mapH / 18)
				local topS = math.max(4, y - 3 * horizontalBand)
				local topN = math.min(mapH - 5, y + 3 * horizontalBand)
				local midN = math.min(topN - 1, y + horizontalBand)
				local midS = math.max(topS + 1, y - horizontalBand)

				--print ("topN", topN, "midN", midN, "midS", midS, "topS", topS)

				for j = topS, midS do
					if not Plot_IsWater(Map.GetPlot(i, j), true) then
						nextElevationC = nextElevationC + 1
					end
				end
				nextElevationC = math.min(nextElevationC, nextElevationC / (midS - topS))
				for j = midS, midN do
					if not Plot_IsWater(Map.GetPlot(i, j), true) then
						nextElevationB = nextElevationB + 1
					end
				end
				nextElevationB = math.min(nextElevationB, nextElevationB / (midN - midS))
				for j = midN, topN do
					if not Plot_IsWater(Map.GetPlot(i, j), true) then
						nextElevationA = nextElevationA + 1
					end
				end
				nextElevationA = math.min(nextElevationA, nextElevationA / (topN - midN))
			end

			local totalElevation = nextElevationA + nextElevationB + nextElevationC
			nextElevationA = nextElevationA / totalElevation
			nextElevationB = nextElevationB / totalElevation
			nextElevationC = nextElevationC / totalElevation

			local looseness = (1 - GetBellCurve(x, mapW - 1))^2
			oddsA = (1 - looseness) * nextElevationA + looseness * distanceA
			oddsB = (1 - looseness) * nextElevationB + looseness * distanceB
			oddsC = (1 - looseness) * nextElevationC + looseness * distanceC

			oddsA = math.cos(oddsA * math.pi / 2) / 3
			oddsB = math.cos(oddsB * math.pi / 2) / 3
			oddsC = math.cos(oddsC * math.pi / 2) / 3

			if oddsA > PWRand(oddsA + oddsB + oddsC) and nextPlotA:GetY() < mapH - 4 then
				plot = nextPlotA
			elseif oddsC > PWRand(oddsA + oddsB + oddsC) and nextPlotC:GetY() > 3 then
				plot = nextPlotC
			else
				plot = nextPlotB
			end
		end
		x = plot:GetX()
		y = plot:GetY()
		attempts = attempts + 1
	end
	print("No more Magallanes attempts")
	return riftPlots
end

function SetOceanRiftElevations(elevationMap)

	for plotID, data in pairs(mg.oceanRiftPlots) do
		if data.isWater then
			local plot = Map.GetPlotByIndex(plotID)
			elevationMap.data[elevationMap:GetIndex(plot:GetX(), plot:GetY())] = 0
		end
	end

	return elevationMap
end

function SetOceanRiftPlots()
	print("SetOceanRiftPlots")
	for plotID, data in pairs(mg.oceanRiftPlots) do
		local plot = Map.GetPlotByIndex(plotID)
		--print(string.format("oceanRiftPlots plotID=%-4s isWater=%-6s terrainID=%-3s", plotID, tostring(data.isWater), data.terrainID))
		if data.isWater then
			if not plot:IsWater() or data.terrainID == TerrainTypes.TERRAIN_OCEAN then
				plot:SetTerrainType(data.terrainID, true, true)
			end
		end
	end

	for plotID, data in pairs(mg.oceanRiftPlots) do
		local plot = Map.GetPlotByIndex(plotID)
		if plot:GetTerrainType() == TerrainTypes.TERRAIN_COAST then
			local foundLand = false
			for nearPlot in Plot_GetPlotsInCircle(plot, 1, 2) do
				if not Plot_IsWater(nearPlot) then
					foundLand = true
					break
				end
			end
			if not foundLand then
				plot:SetTerrainType(TerrainTypes.TERRAIN_OCEAN, true, true)
			end
		elseif plot:GetTerrainType() == TerrainTypes.TERRAIN_OCEAN then
			for nearPlot in Plot_GetPlotsInCircle(plot, 1, 1) do
				if not Plot_IsWater(nearPlot) then
					plot:SetTerrainType(TerrainTypes.TERRAIN_COAST, true, true)
					break
				end
			end
		end
	end
end

------------------------------------------------------------------------------------------------------------


--
-- Generate Features
--

function AddFeatures()
	print("Adding Features CommunitasMap")
	local mapW, mapH = Map.GetGridSize()
	Map.RecalculateAreas()
	RestoreLakes()

	local timeStart = debugTime and os.clock() or 0
	local zeroTreesThreshold	= rainfallMap:FindThresholdFromPercent(mg.zeroTreesPercent,false,true)
	local jungleThreshold		= rainfallMap:FindThresholdFromPercent(mg.junglePercent,false,true)

	for plotID, plot in Plots(Shuffle) do
		Plot_AddMainFeatures(plot, zeroTreesThreshold, jungleThreshold)
	end

	local potentialForestPlots = {}
	print("place possible ice")
	print("place possible atoll")
	for plotID, plot in Plots(Shuffle) do
		if not plot:IsWater() then
			PlacePossibleOasis(plot)
			if IsGoodExtraForestTile(plot) then
				table.insert(potentialForestPlots, plot)
			end
		else
			PlacePossibleIce(plot)
			PlacePossibleAtoll(plotID)
		end
	end

	for _, plot in pairs(potentialForestPlots) do
		plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
	end

	ConnectPolarSeasToOceans()

	-- Remove all rivers bordering lakes or oceans
	for _, plot in Plots() do
		if Plot_IsWater(plot) then
			for edgeDirection = 0, DirectionTypes.NUM_DIRECTION_TYPES - 1 do
				Plot_SetRiver(plot, edgeDirection, mg.flowNONE);
			end
		end
	end

	-- Remove flood plains from non-river deserts
	if plot:GetFeatureType() == FeatureTypes.FEATURE_FLOOD_PLAINS and not plot:CanHaveFeature(FeatureTypes.FEATURE_FLOOD_PLAINS) then
		plot:SetFeatureType(FeatureTypes.FEATURE_NONE, -1);
		return
	end

	if debugTime then print(string.format("%5s ms, AddFeatures %s", math.floor((os.clock() - timeStart) * 1000), "End")) end
end

function Plot_AddMainFeatures(plot, zeroTreesThreshold, jungleThreshold)
	local x, y					= plot:GetX(), plot:GetY()
	local i						= elevationMap:GetIndex(x,y)
	local mapW, mapH			= Map.GetGridSize()
	local plotTerrainID 		= plot:GetTerrainType()

	if plot:IsWater() or plot:IsMountain() then
		return
	end

	-- Set desert rivers to floodplains
	if plot:CanHaveFeature(FeatureTypes.FEATURE_FLOOD_PLAINS) then
		plot:SetFeatureType(FeatureTypes.FEATURE_FLOOD_PLAINS,-1)
		return
	end

	-- Micro-climates for tiny volcanic islands
	if not plot:IsMountain() and (plotTerrainID == TerrainTypes.TERRAIN_PLAINS or plotTerrainID == TerrainTypes.TERRAIN_GRASS) then
		local areaSize = plot:Area():GetNumTiles()
		if areaSize <= 5 and (6 - areaSize) >= Map.Rand(5, "Add Island Features - Lua") then
			local zone = elevationMap:GetZone(y)
			if zone == mg.NEQUATOR or zone == mg.SEQUATOR then
				for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
					if nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
						return
					end
				end
				plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE,-1)
				return
			elseif zone == mg.NTEMPERATE or zone == mg.STEMPERATE then
				plot:SetFeatureType(FeatureTypes.FEATURE_FOREST,-1)
				return
			end
		end
	end

	-- Too dry for jungle

	if rainfallMap.data[i] < jungleThreshold then
		if (rainfallMap.data[i] > zeroTreesThreshold) and (temperatureMap.data[i] > mg.treesMinTemperature) then
			if IsGoodFeaturePlot(plot) then
				plot:SetFeatureType(FeatureTypes.FEATURE_FOREST,-1)
			end
		end
		return
	end

	-- Too cold for jungle
	if temperatureMap.data[i] < mg.jungleMinTemperature then
		if temperatureMap.data[i] > mg.treesMinTemperature and IsGoodFeaturePlot(plot) then
			plot:SetFeatureType(FeatureTypes.FEATURE_FOREST,-1)
		end
		return
	end

	-- Too near desert for jungle
	for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
		if nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
			return
		end
	end

	-- This tile is tropical forest or jungle
	table.insert(mg.tropicalPlots, plot)

	-- Check marsh
	if temperatureMap.data[i] > mg.treesMinTemperature and IsGoodFeaturePlot(plot, FeatureTypes.FEATURE_MARSH) then
		plot:SetPlotType(PlotTypes.PLOT_LAND,false,false)
		plot:SetFeatureType(FeatureTypes.FEATURE_MARSH,-1)
		return
	end

	if IsGoodFeaturePlot(plot) then
		plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE,-1)
		-- Turn some non-hill jungle into plains so they at least have some production - azum4roll
		if not plot:IsHills() and modifyOdds(mg.featurePercent, 2) >= PWRand() then
			plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS,false,false)
		end
	else
		plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS,false,false)
	end
end

function IsGoodFeaturePlot(plot, featureID)
	local odds = 0

	if featureID == FeatureTypes.FEATURE_MARSH then
		if plot:GetPlotType() ~= PlotTypes.PLOT_LAND then
			return false
		end
		for nearPlot in Plot_GetPlotsInCircle(plot, 0, 1) do
			local nearTerrainID = nearPlot:GetTerrainType()
			if nearPlot:GetPlotType() == PlotTypes.PLOT_OCEAN then
				if plot:IsRiverSide() then
					odds = odds + 2 * mg.marshPercent
				end
				if nearPlot:IsLake() then
					odds = odds + 4 * mg.marshPercent
				else
					odds = odds + mg.marshPercent
				end
			-- elseif nearTerrainID == TerrainTypes.TERRAIN_DESERT or nearTerrainID == TerrainTypes.TERRAIN_SNOW or nearTerrainID == TerrainTypes.TERRAIN_TUNDRA then
			elseif nearTerrainID == TerrainTypes.TERRAIN_DESERT or nearTerrainID == TerrainTypes.TERRAIN_SNOW then
				return 0
			elseif nearPlot:GetFeatureType() == FeatureTypes.FEATURE_MARSH then
				odds = odds - 8 * mg.marshPercent -- avoid clumping
			elseif plot:IsRiverSide() and nearPlot:IsFreshWater() then
				odds = odds + mg.marshPercent
			end
		end
		return math.min(odds, mg.featurePercent) >= PWRand()
	end

--	No features on deserts and snow
	if plot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT or plot:GetTerrainType() == TerrainTypes.TERRAIN_SNOW then
		return false
	end

	odds = mg.featurePercent
	if plot:IsFreshWater() then
		oddsMultiplier = 1.0 + mg.featureWetVariance
	else
		oddsMultiplier = 1.0 - mg.featureWetVariance
	end

--	Increase chance for features when there are other features nearby already.
	for nearPlot in Plot_GetPlotsInCircle(plot, 0, 2) do
		local nearFeatureID = nearPlot:GetFeatureType()
		if nearFeatureID == FeatureTypes.FEATURE_FOREST then
			oddsMultiplier = oddsMultiplier * 1.30
		elseif nearFeatureID == FeatureTypes.FEATURE_JUNGLE then
			oddsMultiplier = oddsMultiplier * 1.40
		elseif nearFeatureID ~= FeatureTypes.NO_FEATURE then
			oddsMultiplier = oddsMultiplier * 1.0
		elseif nearPlot:GetTerrainType() == TerrainTypes.PLOT_HILLS or nearPlot:IsMountain() then
			oddsMultiplier = oddsMultiplier * 0.85
		end
	end

--	Decrease chance for features on hilly tundra
	if plot:GetTerrainType() == TerrainTypes.TERRAIN_TUNDRA and plot:GetPlotType() == PlotTypes.PLOT_HILLS then
		oddsMultiplier = oddsMultiplier * 0.50
	end

	odds = modifyOdds(odds, oddsMultiplier)
	return odds >= PWRand()
end

function IsBarrenDesert(plot)
	return plot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT and not plot:IsMountain() and plot:GetFeatureType() == FeatureTypes.NO_FEATURE
end

function PlacePossibleOasis(plot)
	local x, y = plot:GetX(), plot:GetY()
	if not plot:CanHaveFeature(FeatureTypes.FEATURE_OASIS) then
		return
	end

	local odds = 0
	for nearPlot, distance in Plot_GetPlotsInCircle(plot, 3) do
		local distance = distance or 1
		local featureID = nearPlot:GetFeatureType()

		if featureID == FeatureTypes.FEATURE_OASIS then
			if distance <= 2 then
				-- at least 2 tile spacing between oases
				return
			end
			odds = odds - 200 / distance
		end

		if featureID == FeatureTypes.NO_FEATURE and not nearPlot:IsFreshWater() then
			if nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
				odds = odds + 10 / distance
			elseif IsMountain(nearPlot) or nearPlot:IsHills() then
				odds = odds + 5 / distance
			elseif nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_PLAINS then
				odds = odds + 2 / distance
			end
		else
			odds = odds - 5 / distance
		end
	end

	if odds >= Map.Rand(100, "PlacePossibleOasis - Lua") then
		plot:SetFeatureType(FeatureTypes.FEATURE_OASIS, -1)
	end
end

function IsGoodExtraForestTile(plot)
	local x, y		= plot:GetX(), plot:GetY()
	local odds		= mg.forestRandomPercent
	local terrainID = plot:GetTerrainType()
	local resID		= plot:GetResourceType(-1)
	if not plot:CanHaveFeature(FeatureTypes.FEATURE_FOREST) then
		return false
	end

	local oddsMultiplier = 1.0
	if terrainID == TerrainTypes.TERRAIN_TUNDRA then
		if resID ~= -1 then
			return true
		end
		if plot:IsFreshWater() then
			oddsMultiplier = oddsMultiplier + mg.featureWetVariance
		end
	end

	-- Avoid filling flat holes of tropical areas, which are too dense already
	if not plot:IsHills() and Contains(mg.tropicalPlots, plot) then
		oddsMultiplier = oddsMultiplier - 0.45
	end

	for nearPlot in Plot_GetPlotsInCircle(plot, 1, 1) do
		local nearTerrainID = nearPlot:GetTerrainType()
		local nearFeatureID = nearPlot:GetFeatureType()

		if nearPlot:IsMountain() then
			-- do nothing
		elseif nearPlot:IsHills() then
			-- Region already has enough production and rough terrain
			oddsMultiplier = oddsMultiplier * 0.95
		elseif nearTerrainID == TerrainTypes.TERRAIN_SNOW then
			-- Help extreme polar regions
			oddsMultiplier = oddsMultiplier * 1.25
		elseif nearTerrainID == TerrainTypes.TERRAIN_TUNDRA then
			oddsMultiplier = oddsMultiplier * 1.10
		elseif terrainID == TerrainTypes.TERRAIN_TUNDRA and Plot_IsWater(nearPlot) then
			oddsMultiplier = oddsMultiplier * 1.05
		end

		-- Avoid tropics
		if Contains(mg.tropicalPlots, nearPlot) then
			oddsMultiplier = oddsMultiplier * 0.90
		end

		-- Too dry
		if nearTerrainID == TerrainTypes.TERRAIN_DESERT then
			oddsMultiplier = oddsMultiplier * 0.80
		end
	end

	odds = modifyOdds(odds, oddsMultiplier)

	if 100 * mg.featurePercent * odds >= Map.Rand(100, "Add Extra Forest - Lua") then
		--plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
		return true
	end

	return false
end

function PlacePossibleIce(plot)
	local mapW, mapH = Map.GetGridSize()
	local x, y = plot:GetX(), plot:GetY()
	if not plot:IsWater() then
		return
	end

	local latitude = math.abs(temperatureMap:GetLatitudeForY(y))
	local lowestIce = mg.iceLatitude
	local odds = 100
	local oddsMultiplier = 1.0

	if 1 < y and y < mapH - 2 then
		if (latitude >= lowestIce) then
			odds = math.pow((latitude - lowestIce) / (mg.topLatitude - lowestIce), 0.5)
		else
			return
		end
	end

	-- Unblock islands surrounded by ice.
	-- Avoid ice between land tiles and equator, increase ice between land tiles and poles

	for nearPlot in Plot_GetPlotsInCircle(plot, 2) do
		if not nearPlot:IsWater() then
			if y < mapH/2 then -- South hemisphere
				if nearPlot:GetY() < y then
					return
				else
					oddsMultiplier = oddsMultiplier + 0.2
				end
			else -- North hemisphere
				if nearPlot:GetY() > y then
					return
				else
					oddsMultiplier = oddsMultiplier + 0.2
				end
			end
		end
	end

	odds = modifyOdds(odds, oddsMultiplier)

	if odds >= PWRand() then
		plot:SetFeatureType(FeatureTypes.FEATURE_ICE, -1)

		for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
			if nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_GRASS then
				nearPlot:SetTerrainType(TerrainTypes.TERRAIN_TUNDRA, false, true)
				if nearPlot:GetFeatureType() == FeatureTypes.FEATURE_MARSH then
					nearPlot:SetFeatureType(FeatureTypes.NO_FEATURE, -1)
				end
			end
		end
	end
end

function PlacePossibleAtoll(i)
	local W,H = Map.GetGridSize();
	local plot = Map.GetPlotByIndex(i)
	local x = i%W
	local y = (i-x)/W
	if plot:GetTerrainType() == TerrainTypes.TERRAIN_COAST then
		local latitude = math.abs(temperatureMap:GetLatitudeForY(y))
		if latitude < mg.atollNorthLatitudeLimit and latitude > mg.atollSouthLatitudeLimit then
			local deepCount = 0
			local totalFeatures = 0
			local thereIsLand = false
			for nearPlot in Plot_GetPlotsInCircle(plot, 1, 3) do
				if nearPlot:GetPlotType() == PlotTypes.PLOT_LAND then
					thereIsLand = true
				end
			end
			if thereIsLand == false then
				return
			end
			for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
				if nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_OCEAN then
					deepCount = deepCount + 1
				end
				if nearPlot:GetFeatureType() ~= FeatureTypes.NO_FEATURE then
					totalFeatures = totalFeatures + 1
				end
			end
			if deepCount >= mg.atollMinDeepWaterNeighbors then
				if totalFeatures < Map.Rand(3, "Place Possible Atoll - Lua") then
					plot:SetFeatureType(GameInfo.Features.FEATURE_ATOLL.ID, -1)
				end
			end
		end
	end
end


--
-- Other Generators
--

DiffMap = inheritsFrom(FloatMap)
function GenerateDiffMap(width,height,xWrap,yWrap)
	DiffMap = FloatMap:New(width,height,xWrap,yWrap)
	local i = 0
	for y = 0, height - 1,1 do
		for x = 0,width - 1,1 do
			if elevationMap:IsBelowSeaLevel(x,y) then
				DiffMap.data[i] = 0.0
			else
				DiffMap.data[i] = GetDifferenceAroundHex(x,y)
			end
			i=i+1
		end
	end

	DiffMap:Normalize()
	i = 0
	for y = 0, height - 1,1 do
		for x = 0,width - 1,1 do
			if elevationMap:IsBelowSeaLevel(x,y) then
				DiffMap.data[i] = 0.0
			else
				DiffMap.data[i] = DiffMap.data[i] + elevationMap.data[i] * 1.1
			end
			i=i+1
		end
	end

	DiffMap:Normalize()
	return DiffMap
end

function GenerateTwistedPerlinMap(width, height, xWrap, yWrap,minFreq,maxFreq,varFreq)
	local inputNoise = FloatMap:New(width,height,xWrap,yWrap)
	inputNoise:GenerateNoise()
	inputNoise:Normalize()

	local freqMap = FloatMap:New(width,height,xWrap,yWrap)
	for y = 0, freqMap.height - 1 do
		for x = 0,freqMap.width - 1 do
			local i = freqMap:GetIndex(x,y)
			local odd = y % 2
			local xx = x + odd * 0.5
			freqMap.data[i] = GetPerlinNoise(xx,y * mg.YtoXRatio,freqMap.width,freqMap.height * mg.YtoXRatio,varFreq,1.0,0.1,8,inputNoise)
		end
	end
	freqMap:Normalize()

	local twistMap = FloatMap:New(width,height,xWrap,yWrap)
	for y = 0, twistMap.height - 1 do
		for x = 0,twistMap.width - 1 do
			local i = twistMap:GetIndex(x,y)
			local freq = freqMap.data[i] * (maxFreq - minFreq) + minFreq
			local mid = (maxFreq - minFreq)/2 + minFreq
			local coordScale = freq/mid
			local offset = (1.0 - coordScale)/mid
			--print("1-coordscale = " .. (1.0 - coordScale) .. ", offset = " .. offset)
			local ampChange = 0.85 - freqMap.data[i] * 0.5
			local odd = y % 2
			local xx = x + odd * 0.5
			twistMap.data[i] = GetPerlinNoise(xx + offset,(y + offset) * mg.YtoXRatio,twistMap.width,twistMap.height * mg.YtoXRatio,mid,1.0,ampChange,8,inputNoise)
		end
	end

	twistMap:Normalize()
	return twistMap
end

function GenerateMountainMap(width,height,xWrap,yWrap,initFreq)
	local timeStart = debugTime and os.clock() or 0

	local inputNoise = FloatMap:New(width,height,xWrap,yWrap)
	inputNoise:GenerateBinaryNoise()
	inputNoise:Normalize()
	local inputNoise2 = FloatMap:New(width,height,xWrap,yWrap)
	inputNoise2:GenerateNoise()
	inputNoise2:Normalize()

	local mountainMap = FloatMap:New(width,height,xWrap,yWrap)
	local stdDevMap = FloatMap:New(width,height,xWrap,yWrap)
	local noiseMap = FloatMap:New(width,height,xWrap,yWrap)

	if debugTime then print(string.format("%5s ms, GenerateMountainMap %s", math.floor((os.clock() - timeStart) * 1000), "Start")) end
	if debugTime then timeStart = os.clock() end
	for y = 0, mountainMap.height - 1 do
		for x = 0,mountainMap.width - 1 do
			local i = mountainMap:GetIndex(x,y)
			local odd = y % 2
			local xx = x + odd * 0.5
			mountainMap.data[i] = GetPerlinNoise(xx,y * mg.YtoXRatio,mountainMap.width,mountainMap.height * mg.YtoXRatio,initFreq,1.0,0.4,8,inputNoise)
			noiseMap.data[i] = GetPerlinNoise(xx,y * mg.YtoXRatio,mountainMap.width,mountainMap.height * mg.YtoXRatio,initFreq,1.0,0.4,8,inputNoise2)
			stdDevMap.data[i] = mountainMap.data[i]
		end
	end
	if debugTime then print(string.format("%5s ms, GenerateMountainMap %s", math.floor((os.clock() - timeStart) * 1000), "A")) end
	if debugTime then timeStart = os.clock() end
	mountainMap:Normalize()
	if debugTime then print(string.format("%5s ms, GenerateMountainMap %s", math.floor((os.clock() - timeStart) * 1000), "B")) end
	if debugTime then timeStart = os.clock() end
	stdDevMap:Deviate(mg.elevationBlendRange)
	if debugTime then print(string.format("%5s ms, GenerateMountainMap %s", math.floor((os.clock() - timeStart) * 1000), "C")) end
	if debugTime then timeStart = os.clock() end
	stdDevMap:Normalize()
	noiseMap:Normalize()


	if debugTime then print(string.format("%5s ms, GenerateMountainMap %s", math.floor((os.clock() - timeStart) * 1000), "D")) end
	if debugTime then timeStart = os.clock() end

	local moundMap = FloatMap:New(width,height,xWrap,yWrap)
	for y = 0, mountainMap.height - 1 do
		for x = 0,mountainMap.width - 1 do
			local i = mountainMap:GetIndex(x,y)
			local val = mountainMap.data[i]
			moundMap.data[i] = (math.sin(val*math.pi*2-math.pi*0.5)*0.5+0.5) * GetAttenuationFactor(mountainMap,x,y)
			if val < 0.5 then
				val = val^1 * 4
			else
				val = (1 - val)^1 * 4
			end
			mountainMap.data[i] = moundMap.data[i]
		end
	end
	mountainMap:Normalize()

	for y = 0, mountainMap.height - 1 do
		for x = 0,mountainMap.width - 1 do
			local i = mountainMap:GetIndex(x,y)
			local val = mountainMap.data[i]
			mountainMap.data[i] = (math.sin(val * 3 * math.pi + math.pi * 0.5)^16 * val)^0.5
			if mountainMap.data[i] > 0.2 then
				mountainMap.data[i] = 1.0
			else
				mountainMap.data[i] = 0.0
			end
		end
	end

	local stdDevThreshold = stdDevMap:FindThresholdFromPercent(mg.landPercent + 0.05,true,false)
	log:Debug("stdDevThreshold = %s", stdDevThreshold)

	for y = 0, mountainMap.height - 1 do
		for x = 0,mountainMap.width - 1 do
			local i = mountainMap:GetIndex(x,y)
			local val = mountainMap.data[i]
			local dev = 2.0 * stdDevMap.data[i] - 2.0 * stdDevThreshold
			mountainMap.data[i] = (val + moundMap.data[i]) * dev
		end
	end

	mountainMap:Normalize()

	return mountainMap
end

function GetAttenuationFactor(map,x,y)
	local southY = map.height * mg.southAttenuationRange
	local southRange = map.height * mg.southAttenuationRange
	local yAttenuation = 1.0
	if y < southY then
		yAttenuation = mg.southAttenuationFactor + (y/southRange) * (1.0 - mg.southAttenuationFactor)
	end

	local northY = map.height - (map.height * mg.northAttenuationRange)
	local northRange = map.height * mg.northAttenuationRange
	if y > northY then
		yAttenuation = mg.northAttenuationFactor + ((map.height - y)/northRange) * (1.0 - mg.northAttenuationFactor)
	end

	local eastY = map.width - (map.width * mg.eastAttenuationRange)
	local eastRange = map.width * mg.eastAttenuationRange
	local xAttenuation = 1.0
	if x > eastY then
		xAttenuation = mg.eastAttenuationFactor + ((map.width - x)/eastRange) * (1.0 - mg.eastAttenuationFactor)
	end

	local westY = map.width * mg.westAttenuationRange
	local westRange = map.width * mg.westAttenuationRange
	if x < westY then
		xAttenuation = mg.westAttenuationFactor + (x/westRange) * (1.0 - mg.westAttenuationFactor)
	end

	return yAttenuation * xAttenuation
end

function GenerateElevationMap(width,height,xWrap,yWrap)
	local timeStart = debugTime and os.clock() or 0
	local landMinScatter = (128/width) * mg.landMinScatter --0.02/128
	local landMaxScatter = (128/width) * mg.landMaxScatter --0.12/128
	local coastScatter = (128/width) * mg.coastScatter --0.042/128
	local mountainScatter = (128/width) * mg.mountainScatter --0.05/128
	local twistMap = GenerateTwistedPerlinMap(width,height,xWrap,yWrap,landMinScatter,landMaxScatter,coastScatter)

	if debugTime then timeStart = os.clock() end
	local mountainMap = GenerateMountainMap(width,height,xWrap,yWrap,mountainScatter)
	if debugTime then print(string.format("%5s ms, GenerateElevationMap %s", math.floor((os.clock() - timeStart) * 1000), "GenerateMountainMap")) end

	if debugTime then timeStart = os.clock() end
	local elevationMap = ElevationMap:New(width,height,xWrap,yWrap)
	for y = 0,height - 1 do
		for x = 0,width - 1 do
			local i = elevationMap:GetIndex(x,y)
			local tVal = twistMap.data[i]
			elevationMap.data[i] = (tVal + ((mountainMap.data[i] * 2) - 1) * mg.mountainWeight)
		end
	end

	if debugTime then timeStart = os.clock() end

	elevationMap:Normalize()

	--attentuation should not break normalization
	for y = 0,height - 1 do
		for x = 0,width - 1 do
			local i = elevationMap:GetIndex(x,y)
			local attenuationFactor = GetAttenuationFactor(elevationMap,x,y)
			elevationMap.data[i] = elevationMap.data[i] * attenuationFactor
		end
	end

	if debugTime then timeStart = os.clock() end

	elevationMap.seaLevelThreshold = elevationMap:FindThresholdFromPercent(mg.landPercent + 0.05,true,false)
	log:Debug("seaLevelThreshold = %s", elevationMap.seaLevelThreshold)

	if debugTime then print(string.format("%5s ms, GenerateElevationMap %s", math.floor((os.clock() - timeStart) * 1000), "End")) end
	return elevationMap
end

function GenerateTempMaps(elevationMap)
	local timeStart = debugTime and os.clock() or 0
	local aboveSeaLevelMap = FloatMap:New(elevationMap.width,elevationMap.height,elevationMap.xWrap,elevationMap.yWrap)
	for y = 0,elevationMap.height - 1 do
		for x = 0,elevationMap.width - 1 do
			local i = aboveSeaLevelMap:GetIndex(x,y)
			if elevationMap:IsBelowSeaLevel(x,y) then
				aboveSeaLevelMap.data[i] = 0.0
			else
				aboveSeaLevelMap.data[i] = elevationMap.data[i] - elevationMap.seaLevelThreshold
			end
		end
	end
	aboveSeaLevelMap:Normalize()

	local summerMap = FloatMap:New(elevationMap.width,elevationMap.height,elevationMap.xWrap,elevationMap.yWrap)
	local zenith = mg.tropicLatitudes
	local topTempLat = mg.topLatitude + zenith
	local bottomTempLat = mg.bottomLatitude
	local latRange = topTempLat - bottomTempLat
	for y = 0,elevationMap.height - 1 do
		for x = 0,elevationMap.width - 1 do
			local i = summerMap:GetIndex(x,y)
			local lat = summerMap:GetLatitudeForY(y)
			--print("y=" .. y ..",lat=" .. lat)
			local latPercent = (lat - bottomTempLat)/latRange
			--print("latPercent=" .. latPercent)
			local temp = (math.sin(latPercent * math.pi * 2 - math.pi * 0.5) * 0.5 + 0.5)
			if elevationMap:IsBelowSeaLevel(x,y) then
				temp = temp * mg.maxWaterTemp + mg.minWaterTemp
			end
			summerMap.data[i] = temp
		end
	end
	if debugTime then timeStart = os.clock() end
	summerMap:Smooth(math.min(mg.tempBlendMaxRange, math.floor(elevationMap.width/8)))
	if debugTime then print(string.format("%5s ms, GenerateTempMaps %s", math.floor((os.clock() - timeStart) * 1000), "Smooth")) end
	if debugTime then timeStart = os.clock() end
	summerMap:Normalize()
	local winterMap = FloatMap:New(elevationMap.width,elevationMap.height,elevationMap.xWrap,elevationMap.yWrap)
	zenith = -mg.tropicLatitudes
	topTempLat = mg.topLatitude
	bottomTempLat = mg.bottomLatitude + zenith
	latRange = topTempLat - bottomTempLat
	for y = 0,elevationMap.height - 1 do
		for x = 0,elevationMap.width - 1 do
			local i = winterMap:GetIndex(x,y)
			local lat = winterMap:GetLatitudeForY(y)
			local latPercent = (lat - bottomTempLat)/latRange
			local temp = math.sin(latPercent * math.pi * 2 - math.pi * 0.5) * 0.5 + 0.5
			if elevationMap:IsBelowSeaLevel(x,y) then
				temp = temp * mg.maxWaterTemp + mg.minWaterTemp
			end
			winterMap.data[i] = temp
		end
	end
	winterMap:Smooth(math.min(mg.tempBlendMaxRange, math.floor(elevationMap.width/8)))
	winterMap:Normalize()

	local temperatureMap = FloatMap:New(elevationMap.width,elevationMap.height,elevationMap.xWrap,elevationMap.yWrap)
	for y = 0,elevationMap.height - 1 do
		for x = 0,elevationMap.width - 1 do
			local i = temperatureMap:GetIndex(x,y)
			temperatureMap.data[i] = (winterMap.data[i] + summerMap.data[i]) * (1.0 - aboveSeaLevelMap.data[i])
		end
	end
	temperatureMap:Normalize()

	return summerMap,winterMap,temperatureMap
end

function GenerateRainfallMap(elevationMap)
	local summerMap,winterMap,temperatureMap = GenerateTempMaps(elevationMap)

	local geoMap = FloatMap:New(elevationMap.width,elevationMap.height,elevationMap.xWrap,elevationMap.yWrap)
	for y = 0,elevationMap.height - 1 do
		for x = 0,elevationMap.width - 1 do
			local i = elevationMap:GetIndex(x,y)
			local lat = elevationMap:GetLatitudeForY(y)
			local pressure = elevationMap:GetGeostrophicPressure(lat)
			geoMap.data[i] = pressure
		end
	end
	geoMap:Normalize()

	local sortedSummerMap = {}
	local sortedWinterMap = {}
	for y = 0,elevationMap.height - 1 do
		for x = 0,elevationMap.width - 1 do
			local i = elevationMap:GetIndex(x,y)
			sortedSummerMap[i + 1] = {x,y,summerMap.data[i]}
			sortedWinterMap[i + 1] = {x,y,winterMap.data[i]}
		end
	end
	table.sort(sortedSummerMap, function (a,b) return a[3] < b[3] end)
	table.sort(sortedWinterMap, function (a,b) return a[3] < b[3] end)

	local sortedGeoMap = {}
	local xStart = 0
	local xStop = 0
	local yStart = 0
	local yStop = 0
	local incX = 0
	local incY = 0
	local geoIndex = 1
	local str = ""
	for zone=0,5 do
		local topY = elevationMap:GetYFromZone(zone,true)
		local bottomY = elevationMap:GetYFromZone(zone,false)
		if not (topY == -1 and bottomY == -1) then
			if topY == -1 then
				topY = elevationMap.height - 1
			end
			if bottomY == -1 then
				bottomY = 0
			end
			local dir1,dir2 = elevationMap:GetGeostrophicWindDirections(zone)
			if (dir1 == mg.SW) or (dir1 == mg.SE) then
				yStart = topY
				yStop = bottomY --- 1
				incY = -1
			else
				yStart = bottomY
				yStop = topY --+ 1
				incY = 1
			end
			if dir2 == mg.W then
				xStart = elevationMap.width - 1
				xStop = 0---1
				incX = -1
			else
				xStart = 0
				xStop = elevationMap.width
				incX = 1
			end

			for y = yStart,yStop ,incY do
				--each line should start on water to avoid vast areas without rain
				local xxStart = xStart
				local xxStop = xStop
				for xx = xStart,xStop - incX, incX do
					local i = elevationMap:GetIndex(xx,y)
					if elevationMap:IsBelowSeaLevel(xx,y) then
						xxStart = xx
						xxStop = xx + elevationMap.width * incX
						break
					end
				end
				for x = xxStart,xxStop - incX,incX do
					local i = elevationMap:GetIndex(x,y)
					sortedGeoMap[geoIndex] = {x,y,geoMap.data[i]}
					geoIndex = geoIndex + 1
				end
			end
		end
	end
--	table.sort(sortedGeoMap, function (a,b) return a[3] < b[3] end)

	local rainfallSummerMap = FloatMap:New(elevationMap.width,elevationMap.height,elevationMap.xWrap,elevationMap.yWrap)
	local moistureMap = FloatMap:New(elevationMap.width,elevationMap.height,elevationMap.xWrap,elevationMap.yWrap)
	for i = 1,#sortedSummerMap do
		local x = sortedSummerMap[i][1]
		local y = sortedSummerMap[i][2]
		local pressure = sortedSummerMap[i][3]
		DistributeRain(x,y,elevationMap,temperatureMap,summerMap,rainfallSummerMap,moistureMap,false)
	end

	local rainfallWinterMap = FloatMap:New(elevationMap.width,elevationMap.height,elevationMap.xWrap,elevationMap.yWrap)
	local moistureMap = FloatMap:New(elevationMap.width,elevationMap.height,elevationMap.xWrap,elevationMap.yWrap)
	for i = 1,#sortedWinterMap do
		local x = sortedWinterMap[i][1]
		local y = sortedWinterMap[i][2]
		local pressure = sortedWinterMap[i][3]
		DistributeRain(x,y,elevationMap,temperatureMap,winterMap,rainfallWinterMap,moistureMap,false)
	end

	local rainfallGeostrophicMap = FloatMap:New(elevationMap.width,elevationMap.height,elevationMap.xWrap,elevationMap.yWrap)
	moistureMap = FloatMap:New(elevationMap.width,elevationMap.height,elevationMap.xWrap,elevationMap.yWrap)

	--print("----------------------------------------------------------------------------------------")
	--print("--GEOSTROPHIC---------------------------------------------------------------------------")
	--print("----------------------------------------------------------------------------------------")
	for i = 1,#sortedGeoMap do
		local x = sortedGeoMap[i][1]
		local y = sortedGeoMap[i][2]
		DistributeRain(x,y,elevationMap,temperatureMap,geoMap,rainfallGeostrophicMap,moistureMap,true)
	end
	--zero below sea level for proper percent threshold finding
	for y = 0,elevationMap.height - 1 do
		for x = 0,elevationMap.width - 1 do
			local i = elevationMap:GetIndex(x,y)
			if elevationMap:IsBelowSeaLevel(x,y) then
				rainfallSummerMap.data[i] = 0.0
				rainfallWinterMap.data[i] = 0.0
				rainfallGeostrophicMap.data[i] = 0.0
			end
		end
	end

	rainfallSummerMap:Normalize()
	rainfallWinterMap:Normalize()
	rainfallGeostrophicMap:Normalize()

	local rainfallMap = FloatMap:New(elevationMap.width,elevationMap.height,elevationMap.xWrap,elevationMap.yWrap)
	for y = 0,elevationMap.height - 1 do
		for x = 0,elevationMap.width - 1 do
			local i = elevationMap:GetIndex(x,y)
			rainfallMap.data[i] = rainfallSummerMap.data[i] + rainfallWinterMap.data[i] + (rainfallGeostrophicMap.data[i] * mg.geostrophicFactor)
		end
	end
	rainfallMap:Normalize()

	return rainfallMap, temperatureMap
end

function DistributeRain(x,y,elevationMap,temperatureMap,pressureMap,rainfallMap,moistureMap,boolGeostrophic)

	local i = elevationMap:GetIndex(x,y)
	local upLiftSource = math.max(math.pow(pressureMap.data[i],mg.upLiftExponent),1.0 - temperatureMap.data[i])
	if elevationMap:IsBelowSeaLevel(x,y) then
		moistureMap.data[i] = math.max(moistureMap.data[i], temperatureMap.data[i])
	end

	--make list of neighbors
	local nList = {}
	if boolGeostrophic then
		local zone = elevationMap:GetZone(y)
		local dir1,dir2 = elevationMap:GetGeostrophicWindDirections(zone)
		local x1,y1 = elevationMap:GetNeighbor(x,y,dir1)
		local ii = elevationMap:GetIndex(x1,y1)
		--neighbor must be on map and in same wind zone
		if ii >= 0 and (elevationMap:GetZone(y1) == elevationMap:GetZone(y)) then
			table.insert(nList,{x1,y1})
		end
		local x2,y2 = elevationMap:GetNeighbor(x,y,dir2)
		ii = elevationMap:GetIndex(x2,y2)
		if ii >= 0 then
			table.insert(nList,{x2,y2})
		end
	else
		for dir = 0, 5 do
			local xx,yy = elevationMap:GetNeighbor(x,y,dir)
			local ii = elevationMap:GetIndex(xx,yy)
			if ii >= 0 and pressureMap.data[i] <= pressureMap.data[ii] then
				table.insert(nList,{xx,yy})
			end
		end
	end
	if #nList == 0 or boolGeostrophic and #nList == 1 then
		local cost = moistureMap.data[i]
		rainfallMap.data[i] = cost
		return
	end
	local moisturePerNeighbor = moistureMap.data[i]/#nList
	--drop rain and pass moisture to neighbors
	for n = 1,#nList do
		local xx = nList[n][1]
		local yy = nList[n][2]
		local ii = elevationMap:GetIndex(xx,yy)
		local upLiftDest = math.max(math.pow(pressureMap.data[ii],mg.upLiftExponent),1.0 - temperatureMap.data[ii])
		local cost = GetRainCost(upLiftSource,upLiftDest)
		local bonus = 0.0
		if (elevationMap:GetZone(y) == mg.NPOLAR or elevationMap:GetZone(y) == mg.SPOLAR) then
			bonus = mg.polarRainBoost
		end
		if boolGeostrophic and #nList == 2 then
			if n == 1 then
				moisturePerNeighbor = (1.0 - mg.geostrophicLateralWindStrength) * moistureMap.data[i]
			else
				moisturePerNeighbor = mg.geostrophicLateralWindStrength * moistureMap.data[i]
			end
		end
		rainfallMap.data[i] = rainfallMap.data[i] + cost * moisturePerNeighbor + bonus
		--pass to neighbor.
		moistureMap.data[ii] = moistureMap.data[ii] + moisturePerNeighbor - (cost * moisturePerNeighbor)
	end

end

function GetRainCost(upLiftSource,upLiftDest)
	local cost = mg.minimumRainCost
	cost = math.max(mg.minimumRainCost, cost + upLiftDest - upLiftSource)
	if cost < 0.0 then
		cost = 0.0
	end
	return cost
end

function GetDifferenceAroundHex(x,y)
	local avg = elevationMap:GetAverageInHex(x,y,1)
 	local i = elevationMap:GetIndex(x,y)
	return elevationMap.data[i] - avg
end




--
-- Plot functions
--

function Plot_GetID(plot)
	if not plot then
		error("plot:GetID plot=nil")
		return nil
	end
	local iW, iH = Map.GetGridSize()
	return plot:GetY() * iW + plot:GetX()
end

function Plot_GetFertilityInRange(plot, range, yieldID)
	local value = 0
	for nearPlot, distance in Plot_GetPlotsInCircle(plot, range) do
		value = value + Plot_GetFertility(nearPlot, yieldID) / math.max(1, distance)
	end
	return value
end

function Plot_GetFertility(plot, yieldID, ignoreStrategics)
	if plot:IsImpassable() or plot:GetTerrainType() == TerrainTypes.TERRAIN_OCEAN then
		return 0
	end

	local value = 0
	local featureID = plot:GetFeatureType()
	local terrainID = plot:GetTerrainType()
	local resID = plot:GetResourceType(-1)

	if yieldID then
		value = value + plot:CalculateYield(yieldID, true)
	else
		-- Science, Culture and Faith are worth more than the others at start
		value = value + plot:CalculateYield(YieldTypes.YIELD_FOOD, true)
		value = value + plot:CalculateYield(YieldTypes.YIELD_PRODUCTION, true)
		value = value + plot:CalculateYield(YieldTypes.YIELD_GOLD, true)
		value = value + 2 * plot:CalculateYield(YieldTypes.YIELD_SCIENCE, true)
		value = value + 2 * plot:CalculateYield(YieldTypes.YIELD_CULTURE, true)
		value = value + 2 * plot:CalculateYield(YieldTypes.YIELD_FAITH, true)
	end

	if plot:IsFreshWater() and plot:GetPlotType() ~= PlotTypes.PLOT_HILLS then
		-- Fresh water farm possibility
		value = value + 0.25
	end

	if plot:IsLake() then
		-- can't improve lakes
		value = value - 0.5
	end

	if featureID == FeatureTypes.FEATURE_JUNGLE then
		-- jungles aren't as good as the yields imply
		value = value - 0.5
	end

	if resID == -1 then
		if featureID == -1 and terrainID == TerrainTypes.TERRAIN_COAST then
			-- can't improve coast tiles until lighthouse
			-- lower value generates more fish
			value = value - 1
		end
	else
		local resInfo = GameInfo.Resources[resID]
		value = value + 2 * resInfo.Happiness
		if resInfo.ResourceClassType == "RESOURCECLASS_RUSH" and not ignoreStrategics then
			-- Iron and Horses
			value = value + math.ceil(3 * math.sqrt(plot:GetNumResource()))
		elseif resInfo.ResourceClassType == "RESOURCECLASS_BONUS" then
			value = value + 2
		end
	end
	return value
end
----------------------------------------------------------------
function Plot_GetPlotsInCircle(plot, minR, maxR)
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

	leftX	= isWrapX and ((centerX-maxR) % mapW) or Constrain(0, centerX-maxR, mapW-1)
	rightX	= isWrapX and ((centerX+maxR) % mapW) or Constrain(0, centerX+maxR, mapW-1)
	bottomY	= isWrapY and ((centerY-maxR) % mapH) or Constrain(0, centerY-maxR, mapH-1)
	topY	= isWrapY and ((centerY+maxR) % mapH) or Constrain(0, centerY+maxR, mapH-1)

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

				if IsBetween(minR, distance, maxR) then
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

function Plot_GetPlotsInCircleFast(x, y, radius)
	-- assumes X wrap

	local plotIDs	= {}
	local W, H		= Map.GetGridSize()
	local odd		= y % 2
	local topY		= radius
	local bottomY	= radius
	local currentY	= nil
	local len		= 1+radius
	local i			= (y % H) * W + (x % W)

	--constrain the top of our circle to be on the map
	if y+radius > H-1 then
		for r=0,radius,1 do
			if y+r == H-1 then
				topY = r
				break
			end
		end
	end

	--constrain the bottom of our circle to be on the map
	if y-radius < 0 then
		for r=0,radius,1 do
			if y-r == 0 then
				bottomY = r
				break
			end
		end
	end

	--adjust starting length, apply the top and bottom limits, and correct odd for the starting point
	len			= len+(radius-bottomY)
	currentY	= y - bottomY
	topY		= y + topY
	odd			= (odd+bottomY)%2

	--set starting point
	if x-(radius-bottomY) - math.floor((bottomY+odd)/2) < 0 then
		i = i - (W*bottomY) + (W-(radius-bottomY)) - math.floor((bottomY+odd)/2)
		x = x + (W-(radius-bottomY)) - math.floor((bottomY+odd)/2)
		-- print(string.format("i for (%d,%d) WOULD have been in outer space. x is (%d,%d) i is (%d)",xx,y,x,y-bottomY,i))
	else
		i = i - (W*bottomY) - (radius-bottomY) - math.floor((bottomY+odd)/2)
		x = x - (radius-bottomY) - math.floor((bottomY+odd)/2)
	end

	--cycle through the plot indexes and add them to a table
	--local str = ""
	--local iters = 0
	while currentY <= topY do
		--insert the start value, scan left to right adding each index in the line to our table
		--str = str..i..","
		table.insert(plotIDs,i)
		local wrapped = false
		for n=1,len-1,1 do
			if x ~= (W-1) then
				i = i + 1
				x = x + 1
			else
				i = i-(W-1)
				x = 0
				wrapped = true
			end
			--str = str..i..","
			table.insert(plotIDs,i)
		end
		if currentY < y then
			--move i NW and increment the length to scan
			if not wrapped then
				i = i+W-len+odd
				x = x-len+odd
			else
				i = i+W+(W-len+odd)
				x = x+(W-len+odd)
			end
			len = len+1
		else
			--move i NE and decrement the length to scan
			if not wrapped then
				i = i+W-len+1+odd
				x = x-len+1+odd
			else
				i = i+W+(W-len+1+odd)
				x = x+(W-len+1+odd)
			end
			len = len-1
		end
		currentY = currentY+1
		odd = (odd+1)%2
	end
	-- print(string.format("added "..str.." to table for circle starting at(%d,%d)",xx,y))
	return plotIDs
end

local plotTypeName		= {}-- -1="NO_PLOT"}
local terrainTypeName	= {}-- -1="NO_TERRAIN"}
local featureTypeName	= {}-- -1="NO_FEATURE"}
function Plot_GetCirclePercents(plot, minR, maxR)
	for k, v in pairs(PlotTypes) do
		plotTypeName[v] = k
	end
	for itemInfo in GameInfo.Terrains() do
		terrainTypeName[itemInfo.ID] = itemInfo.Type
	end
	for itemInfo in GameInfo.Features() do
		featureTypeName[itemInfo.ID] = itemInfo.Type
	end

	local weights = {TOTAL=0, SEA=0, NO_PLOT=0, NO_TERRAIN=0, NO_FEATURE=0}

	for k, v in pairs(PlotTypes) do
		weights[k] = 0
	end
	for itemInfo in GameInfo.Terrains() do
		weights[itemInfo.Type] = 0
	end
	for itemInfo in GameInfo.Features() do
		weights[itemInfo.Type] = 0
	end

	for nearPlot, distance in Plot_GetPlotsInCircle(plot, minR, maxR) do
		local nearWeight	 = (distance == 0) and 6 or (1/distance)
		local plotType		 = plotTypeName[nearPlot:GetPlotType()]
		local terrainType	 = terrainTypeName[nearPlot:GetTerrainType()]
		local featureType	 = featureTypeName[nearPlot:GetFeatureType()] or "NO_FEATURE"

		weights.TOTAL		 = weights.TOTAL		+ nearWeight
		weights[plotType]	 = weights[plotType]	+ nearWeight
		weights[terrainType] = weights[terrainType]	+ nearWeight
		weights[featureType] = weights[featureType]	+ nearWeight

		if plotType == "PLOT_OCEAN" then
			if not nearPlot:IsLake() and featureType ~= "FEATURE_ICE" then
				weights.SEA = weights.SEA + nearWeight
			end
		end
	end

	if weights.TOTAL == 0 then
		print("plot:GetAreaWeights Total=0! x=%s y=%s", x, y)
	end
	for k, v in pairs(weights) do
		if k ~= "TOTAL" then
			weights[k] = weights[k] / weights.TOTAL
		end
	end

	return weights
end

function Plot_GetElevation(plot, ignoreSeas)
	if ignoreSeas and Contains(mg.seaPlots, plot) then
		-- try to preserve inland seas
		return elevationMap.seaLevelThreshold
	end
	return elevationMap.data[elevationMap:GetIndex(plot:GetX(), plot:GetY())]
end

function GetElevationByPlotID(plotID)
	local elevation = elevationMap.data[plotID]
	if not elevation then
		log:Warn("GetElevationByPlotID elevationMap.data[%s] = %s", plotID, elevation)
		return 0
	end
	return elevation
end

function Plot_IsWater(plot, useElevation, ignoreSeas)
	if useElevation then
		return elevationMap.data[elevationMap:GetIndex(plot:GetX(), plot:GetY())] < elevationMap.seaLevelThreshold
	end
	if ignoreSeas and Contains(mg.seaPlots, plot) then
		return false
	end
	return (plot:GetPlotType() == PlotTypes.PLOT_OCEAN) or Contains(mg.lakePlots, plot)
end

function Plot_IsLake(plot)
	return plot:IsLake() or Contains(mg.lakePlots, plot)
end

function waterMatch(x,y)
	return elevationMap:IsBelowSeaLevel(x,y)
end

function oceanButNotIceMatch(x, y)
	local plot = Map.GetPlot(x, y)
	return plot:GetFeatureType() ~= FeatureTypes.FEATURE_ICE and plot:GetPlotType() == PlotTypes.PLOT_OCEAN and not Plot_IsLake(plot);
end

function landMatch(x,y)
	return not elevationMap:IsBelowSeaLevel(x,y)
end

function oceanMatch(x,y)
	local plot = Map.GetPlot(x,y)
	if plot:GetPlotType() == PlotTypes.PLOT_OCEAN then
		return true
	end
	return false
end

function jungleMatch(x,y)
	local plot = Map.GetPlot(x,y)
	if plot:GetFeatureType() == FeatureTypes.FEATURE_JUNGLE or Contains(mg.tropicalPlots, plot) then
		return true
	--include any mountains on the border as part of the desert.
	elseif (plot:GetFeatureType() == FeatureTypes.FEATURE_MARSH or plot:GetFeatureType() == FeatureTypes.FEATURE_FOREST) and plot:GetTerrainType() == TerrainTypes.TERRAIN_GRASS then
		local nList = elevationMap:GetRadiusAroundHex(x,y,1)
		for n=1,#nList do
			local xx = nList[n][1]
			local yy = nList[n][2]
			local ii = elevationMap:GetIndex(xx,yy)
			if 11 ~= -1 then
				local nPlot = Map.GetPlot(xx,yy)
				if nPlot:GetFeatureType() == FeatureTypes.FEATURE_JUNGLE then
					return true
				end
			end
		end
	end
	return false
end

function desertMatch(x,y)
	local plot = Map.GetPlot(x,y)
	if plot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
		return true
	--include any mountains on the border as part of the desert.
	elseif IsMountain(plot) then
		local nList = elevationMap:GetRadiusAroundHex(x,y,1)
		for n=1,#nList do
			local xx = nList[n][1]
			local yy = nList[n][2]
			local ii = elevationMap:GetIndex(xx,yy)
			if 11 ~= -1 then
				local nPlot = Map.GetPlot(xx,yy)
				if not IsMountain(nPlot) and nPlot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT then
					return true
				end
			end
		end
	end
	return false
end

function IsMountain(plot)
	return plot:IsMountain() or Contains(mg.MountainPasses, plot)
end



--
-- Utilities
--
---------------------------------------------------------------------
function Round(num, places)
	local mult = 10^(places or 0)
	return math.floor(num * mult + 0.5) / mult
end

function ShuffleList(list)
	local len = #list
	for i=0,len - 1 do
		local k = PWRandint(0,len-1)
		list[i], list[k] = list[k], list[i]
	end
end

function IsBetween(lower, mid, upper)
	return ((lower <= mid) and (mid <= upper))
end

function Contains(list, value)
	for k, v in pairs(list) do
		if v == value then
			return true
		end
	end
	return false
end

function DeepCopy(object)
	-- DeepCopy(object) copies all elements of a table
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end
        local new_table = {}
        lookup_table[object] = new_table
        for index, value in pairs(object) do
            new_table[_copy(index)] = _copy(value)
        end
        return setmetatable(new_table, getmetatable(object))
    end
    return _copy(object)
end

function Constrain(lower, mid, upper)
	return math.max(lower, math.min(mid, upper))
end

function Push(a,item)
	table.insert(a,item)
end

function Pop(a)
	return table.remove(a)
end

function GetRandomWeighted(list, size)
	-- GetRandomWeighted(list, size) returns a key from a list of (key, weight) pairs
	size = size or 100
	local chanceIDs = GetWeightedTable(list, size)

	if chanceIDs == -1 then
		return -1
	end
	local randomID = 1 + Map.Rand(size, "GetRandomWeighted")
	if not chanceIDs[randomID] then
		print("GetRandomWeighted: invalid random index selected = %s", randomID)
		chanceIDs[randomID] = -1
	end
	return chanceIDs[randomID]
end

function GetWeightedTable(list, size)
	-- GetWeightedTable(list, size) returns a table with key blocks sized proportionately to a weighted list
	local totalWeight	= 0
	local chanceIDs		= {}
	local position		= 1

	for key, weight in pairs(list) do
		totalWeight = totalWeight + weight
	end

	if totalWeight == 0 then
		for key, weight in pairs(list) do
			list[key] = 1
			totalWeight = totalWeight + 1
		end
		if totalWeight == 0 then
			print("GetWeightedTable: empty list")
			--print(debug.traceback())
			return -1
		end
	end

	for key, weight in pairs(list) do
		local positionNext = position + size * weight / totalWeight
		for i = math.floor(position), math.floor(positionNext) do
			chanceIDs[i] = key
		end
		position = positionNext
	end
	return chanceIDs
end

function GetOppositeDir(dir)
	return ((dir + 2) % 6) + 1
end

function GetBellCurve(value, normalize)
	--Returns a value along a bell curve from a 0 - 1 range
	if normalize then
		value = 1 - math.abs(value - normalize) / normalize
	end
	return math.sin(value * math.pi * 2 - math.pi/2)/2 + 0.5
end

-----------------------------------------------------------------------------
--Interpolation and Perlin functions
function CubicInterpolate(v0,v1,v2,v3,mu)
	local mu2 = mu * mu
	local a0 = v3 - v2 - v0 + v1
	local a1 = v0 - v1 - a0
	local a2 = v2 - v0
	local a3 = v1

	return (a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3)
end

function BicubicInterpolate(v,muX,muY)
	local a0 = CubicInterpolate(v[1],v[2],v[3],v[4],muX)
	local a1 = CubicInterpolate(v[5],v[6],v[7],v[8],muX)
	local a2 = CubicInterpolate(v[9],v[10],v[11],v[12],muX)
	local a3 = CubicInterpolate(v[13],v[14],v[15],v[16],muX)

	return CubicInterpolate(a0,a1,a2,a3,muY)
end

function CubicDerivative(v0,v1,v2,v3,mu)
	local mu2 = mu * mu
	local a0 = v3 - v2 - v0 + v1
	local a1 = v0 - v1 - a0
	local a2 = v2 - v0
	--local a3 = v1

	return (3 * a0 * mu2 + 2 * a1 * mu + a2)
end

function BicubicDerivative(v,muX,muY)
	local a0 = CubicInterpolate(v[1],v[2],v[3],v[4],muX)
	local a1 = CubicInterpolate(v[5],v[6],v[7],v[8],muX)
	local a2 = CubicInterpolate(v[9],v[10],v[11],v[12],muX)
	local a3 = CubicInterpolate(v[13],v[14],v[15],v[16],muX)

	return CubicDerivative(a0,a1,a2,a3,muY)
end

function GetInterpolatedValue(X,Y,srcMap)
	--This function gets a smoothly interpolated value from srcMap.
	--x and y are non-integer coordinates of where the value is to
	--be calculated, and wrap in both directions. srcMap is an bject
	--of type FloatMap.
	local points = {}
	local fractionX = X - math.floor(X)
	local fractionY = Y - math.floor(Y)

	--wrappedX and wrappedY are set to -1,-1 of the sampled area
	--so that the sample area is in the middle quad of the 4x4 grid
	local wrappedX = ((math.floor(X) - 1) % srcMap.rectWidth) + srcMap.rectX
	local wrappedY = ((math.floor(Y) - 1) % srcMap.rectHeight) + srcMap.rectY

	local x
	local y

	for pY = 0, 4-1 do
		y = pY + wrappedY
		for pX = 0,4-1 do
			x = pX + wrappedX
			local srcIndex = srcMap:GetRectIndex(x, y)
			points[(pY * 4 + pX) + 1] = srcMap.data[srcIndex]
		end
	end

	local finalValue = BicubicInterpolate(points,fractionX,fractionY)

	return finalValue

end

function GetDerivativeValue(X,Y,srcMap)
	local points = {}
	local fractionX = X - math.floor(X)
	local fractionY = Y - math.floor(Y)

	--wrappedX and wrappedY are set to -1,-1 of the sampled area
	--so that the sample area is in the middle quad of the 4x4 grid
	local wrappedX = ((math.floor(X) - 1) % srcMap.rectWidth) + srcMap.rectX
	local wrappedY = ((math.floor(Y) - 1) % srcMap.rectHeight) + srcMap.rectY

	local x
	local y

	for pY = 0, 4-1 do
		y = pY + wrappedY
		for pX = 0,4-1 do
			x = pX + wrappedX
			local srcIndex = srcMap:GetRectIndex(x, y)
			points[(pY * 4 + pX) + 1] = srcMap.data[srcIndex]
		end
	end

	local finalValue = BicubicDerivative(points,fractionX,fractionY)

	return finalValue

end

function GetPerlinNoise(x,y,destMapWidth,destMapHeight,initialFrequency,initialAmplitude,amplitudeChange,octaves,noiseMap)
	--This function gets Perlin noise for the destination coordinates. Note
	--that in order for the noise to wrap, the area sampled on the noise map
	--must change to fit each octave.
	local finalValue = 0.0
	local frequency = initialFrequency
	local amplitude = initialAmplitude
	local frequencyX --slight adjustment for seamless wrapping
	local frequencyY --''
	for i = 1,octaves do
		if noiseMap.wrapX then
			noiseMap.rectX = math.floor(noiseMap.width/2 - (destMapWidth * frequency)/2)
			noiseMap.rectWidth = math.max(math.floor(destMapWidth * frequency),1)
			frequencyX = noiseMap.rectWidth/destMapWidth
		else
			noiseMap.rectX = 0
			noiseMap.rectWidth = noiseMap.width
			frequencyX = frequency
		end
		if noiseMap.wrapY then
			noiseMap.rectY = math.floor(noiseMap.height/2 - (destMapHeight * frequency)/2)
			noiseMap.rectHeight = math.max(math.floor(destMapHeight * frequency),1)
			frequencyY = noiseMap.rectHeight/destMapHeight
		else
			noiseMap.rectY = 0
			noiseMap.rectHeight = noiseMap.height
			frequencyY = frequency
		end

		finalValue = finalValue + GetInterpolatedValue(x * frequencyX, y * frequencyY, noiseMap) * amplitude
		frequency = frequency * 2.0
		amplitude = amplitude * amplitudeChange
	end
	finalValue = finalValue/octaves
	return finalValue
end

function GetPerlinDerivative(x,y,destMapWidth,destMapHeight,initialFrequency,initialAmplitude,amplitudeChange,octaves,noiseMap)
	local finalValue = 0.0
	local frequency = initialFrequency
	local amplitude = initialAmplitude
	local frequencyX --slight adjustment for seamless wrapping
	local frequencyY --''
	for i = 1,octaves do
		if noiseMap.wrapX then
			noiseMap.rectX = math.floor(noiseMap.width/2 - (destMapWidth * frequency)/2)
			noiseMap.rectWidth = math.floor(destMapWidth * frequency)
			frequencyX = noiseMap.rectWidth/destMapWidth
		else
			noiseMap.rectX = 0
			noiseMap.rectWidth = noiseMap.width
			frequencyX = frequency
		end
		if noiseMap.wrapY then
			noiseMap.rectY = math.floor(noiseMap.height/2 - (destMapHeight * frequency)/2)
			noiseMap.rectHeight = math.floor(destMapHeight * frequency)
			frequencyY = noiseMap.rectHeight/destMapHeight
		else
			noiseMap.rectY = 0
			noiseMap.rectHeight = noiseMap.height
			frequencyY = frequency
		end

		finalValue = finalValue + GetDerivativeValue(x * frequencyX, y * frequencyY, noiseMap) * amplitude
		frequency = frequency * 2.0
		amplitude = amplitude * amplitudeChange
	end
	finalValue = finalValue/octaves
	return finalValue
end


---------------------------------------------------------------------

--ShiftMap Class
function ShiftMaps()
	--local stripRadius = self.stripRadius
	local shift_x = 0
	local shift_y = 0

	shift_x = DetermineXShift()

	ShiftMapsBy(shift_x, shift_y)
end

function ShiftMapsBy(xshift, yshift)
	local W, H = Map.GetGridSize()
	if(xshift > 0 or yshift > 0) then
		local Shift = {}
		local iDestI = 0
		for iDestY = 0, H-1 do
			for iDestX = 0, W-1 do
				local iSourceX = (iDestX + xshift) % W

				--local iSourceY = (iDestY + yshift) % H -- If using yshift, enable this and comment out the faster line below. - Bobert13
				local iSourceY = iDestY

				local iSourceI = W * iSourceY + iSourceX
				Shift[iDestI] = elevationMap.data[iSourceI]
				--if debugTime then print(string.format("Shift:%d,	%f	|	eMap:%d,	%f",iDestI,Shift[iDestI],iSourceI,elevationMap.data[iSourceI])) end
				iDestI = iDestI + 1
			end
		end
		elevationMap.data = Shift --It's faster to do one large table operation here than it is to do thousands of small operations to set up a copy of the input table at the beginning. -Bobert13
	end
	return elevationMap
end

function DetermineXShift()
	--[[ This function will align the most water-heavy vertical portion of the map with the
	vertical map edge. This is a form of centering the landmasses, but it emphasizes the
	edge not the middle. If there are columns completely empty of land, these will tend to
	be chosen as the new map edge, but it is possible for a narrow column between two large
	continents to be passed over in favor of the thinnest section of a continent, because
	the operation looks at a group of columns not just a single column, then picks the
	center of the most water heavy group of columns to be the new vertical map edge. ]]--

	-- First loop through the map columns and record land plots in each column.
	local mapW, mapH = Map.GetGridSize()
	local land_totals = {}
	for x = 0, mapW - 1 do
		local current_column = 0
		for y = 0, mapH - 1 do
			local i = y * mapW + x + 1
			if not elevationMap:IsBelowSeaLevel(x,y) then
				current_column = current_column + 1
			end
		end
		table.insert(land_totals, current_column)
	end

	-- Now evaluate column groups, each record applying to the center column of the group.
	local column_groups = {}
	-- Determine the group size in relation to map width.
	local group_radius = 3
	-- Measure the groups.
	for column_index = 1, mapW do
		local current_group_total = 0
		--for current_column = column_index - group_radius, column_index + group_radius do
		--Changed how group_radius works to get groups of four. -Bobert13
		for current_column = column_index, column_index + group_radius do
			local current_index = current_column % mapW
			if current_index == 0 then -- Modulo of the last column will be zero this repairs the issue.
				current_index = mapW
			end
			current_group_total = current_group_total + land_totals[current_index]
		end
		table.insert(column_groups, current_group_total)
	end

	-- Identify the group with the least amount of land in it.
	local best_value = mapH * (group_radius + 1) -- Set initial value to max possible.
	local best_group = 1 -- Set initial best group as current map edge.
	for column_index, group_land_plots in ipairs(column_groups) do
		if group_land_plots < best_value then
			best_value = group_land_plots
			best_group = column_index
		end
	end

	-- Determine X Shift
	local x_shift = best_group + 2

	return x_shift
end

-----------------------------------------------------------------------------
-- Random functions will use lua rands for stand alone script running and Map.rand for in game.
function PWRand()
	if Map then
		return Map.Rand(10000, "Random - Lua") / 10000
	end
	return math.random()
end

function PWRandSeed(fixedseed)
    local seed
    if fixedseed == nil then
        seed = (Map.Rand(32767,"") * 65536) + Map.Rand(65535,"")
    else
        seed = fixedseed
    end
    math.randomseed(seed)
    print("random seed for this map is " .. seed)
end

function PWRandint(low, high)
	--range is inclusive, low and high are possible results
	return math.random(low, high)
end


--
-- FloatMap
--

-----------------------------------------------------------------------------
-- FloatMap class
-- This is for storing 2D map data. The 'data' field is a zero based, one
-- dimensional array. To access map data by x and y coordinates, use the
-- GetIndex method to obtain the 1D index, which will handle any needs for
-- wrapping in the x and y directions.
-----------------------------------------------------------------------------
FloatMap = inheritsFrom(nil)

function FloatMap:New(width, height, wrapX, wrapY)
	local new_inst = {}
	setmetatable(new_inst, {__index = FloatMap})	--setup metatable

	new_inst.width = width
	new_inst.height = height
	new_inst.wrapX = wrapX
	new_inst.wrapY = wrapY
	new_inst.length = width*height

	--These fields are used to access only a subset of the map
	--with the GetRectIndex function. This is useful for
	--making Perlin noise wrap without generating separate
	--noise fields for each octave
	new_inst.rectX = 0
	new_inst.rectY = 0
	new_inst.rectWidth = width
	new_inst.rectHeight = height

	new_inst.data = {}
	for i = 0,width*height - 1 do
		new_inst.data[i] = 0.0
	end

	return new_inst
end

function FloatMap:GetNeighbor(x,y,dir)
	local xx
	local yy
	local odd = y % 2
	if dir == mg.C then
		return x,y
	elseif dir == mg.W then
		xx = x - 1
		yy = y
		return xx,yy
	elseif dir == mg.NW then
		xx = x - 1 + odd
		yy = y + 1
		return xx,yy
	elseif dir == mg.NE then
		xx = x + odd
		yy = y + 1
		return xx,yy
	elseif dir == mg.E then
		xx = x + 1
		yy = y
		return xx,yy
	elseif dir == mg.SE then
		xx = x + odd
		yy = y - 1
		return xx,yy
	elseif dir == mg.SW then
		xx = x - 1 + odd
		yy = y - 1
		return xx,yy
	else
		print(string.format("Bad direction %s in FloatMap:GetNeighbor", dir))
	end
	return -1,-1
end

function FloatMap:GetIndex(x,y)
	if not self.wrapY and (y < 0 or y > self.height-1) then
		return -1
	elseif not self.wrapX and (x < 0 or x > self.width-1) then
		return -1
	end

	return (y % self.height) * self.width + (x % self.width)
end

function FloatMap:GetXYFromIndex(i)
	local x = i % self.width
	local y = (i - x)/self.width
	return x,y
end

--quadrants are labeled
--A B
--D C
function FloatMap:GetQuadrant(x,y)
	if x < self.width/2 then
		if y < self.height/2 then
			return "A"
		else
			return "D"
		end
	else
		if y < self.height/2 then
			return "B"
		else
			return "C"
		end
	end
end

--Gets an index for x and y based on the current rect settings. x and y are local to the defined rect.
--Wrapping is assumed in both directions
function FloatMap:GetRectIndex(x,y)
	local xx = x % self.rectWidth
	local yy = y % self.rectHeight

	xx = self.rectX + xx
	yy = self.rectY + yy

	return self:GetIndex(xx,yy)
end

function FloatMap:Normalize()
	--find highest and lowest values
	local maxAlt = -1000.0
	local minAlt = 1000.0
	for i = 0,self.length - 1 do
		local alt = self.data[i]
		if alt > maxAlt then
			maxAlt = alt
		end
		if alt < minAlt then
			minAlt = alt
		end

	end
	--subtract minAlt from all values so that
	--all values are zero and above
	for i = 0, self.length - 1, 1 do
		self.data[i] = self.data[i] - minAlt
	end

	--subract minAlt also from maxAlt
	maxAlt = maxAlt - minAlt

	--determine and apply scaler to whole map
	local scaler
	if maxAlt == 0.0 then
		scaler = 0.0
	else
		scaler = 1.0/maxAlt
	end

	for i=0,self.length-1,1 do
		self.data[i],expo = math.frexp(self.data[i])
		self.data[i] = (self.data[i]*scaler)
		self.data[i] = math.ldexp(self.data[i],expo)
	end
end

function FloatMap:GenerateNoise()
	for i = 0,self.length - 1 do
		self.data[i] = PWRand()
	end

end

function FloatMap:GenerateBinaryNoise()
	for i = 0,self.length - 1 do
		if PWRand() > 0.5 then
			self.data[i] = 1
		else
			self.data[i] = 0
		end
	end

end

function FloatMap:FindThresholdFromPercent(percent, greaterThan, excludeZeros)
	local mapList = {}
	local percentage = percent * 100

	if greaterThan then
		percentage = 100 - percentage
	end

	if percentage >= 100 then
		return 1.01 --whole map
	elseif percentage <= 0 then
		return -0.01 --none of the map
	end

	for i = 0,self.length - 1 do
		if not (self.data[i] == 0.0 and excludeZeros) then
			table.insert(mapList,self.data[i])
		end
	end

	table.sort(mapList, function (a,b) return a < b end)
	local threshIndex = math.floor((#mapList * percentage)/100)

	log:Debug("threshIndex %s = math.floor((%s * %s)/100)", threshIndex, #mapList, percentage)

	return mapList[threshIndex - 1]

end

function FloatMap:GetLatitudeForY(y)
	local range = mg.topLatitude - mg.bottomLatitude
	local lat = nil
	if y < self.height/2 then
		lat = (y+1) / self.height * range + (mg.bottomLatitude - mg.topLatitude / self.height)
	else
		lat = y / self.height * range + (mg.bottomLatitude + mg.topLatitude / self.height)
	end
	return lat
end

function FloatMap:GetYForLatitude(lat)
	local range = mg.topLatitude - mg.bottomLatitude
	return math.floor(((lat - mg.bottomLatitude) /range * self.height) + 0.5)
end

function FloatMap:GetZone(y)
	local lat = self:GetLatitudeForY(y)
	if y < 0 or y >= self.height then
		return mg.NOZONE
	end
	if lat > mg.polarFrontLatitude then
		return mg.NPOLAR
	elseif lat >= mg.horseLatitudes then
		return mg.NTEMPERATE
	elseif lat >= 0.0 then
		return mg.NEQUATOR
	elseif lat > -mg.horseLatitudes then
		return mg.SEQUATOR
	elseif lat >= -mg.polarFrontLatitude then
		return mg.STEMPERATE
	else
		return mg.SPOLAR
	end
end

function FloatMap:GetYFromZone(zone, bTop)
	if bTop then
		for y=self.height - 1,0,-1 do
			if zone == self:GetZone(y) then
				return y
			end
		end
	else
		for y=0,self.height - 1 do
			if zone == self:GetZone(y) then
				return y
			end
		end
	end
	return -1
end

function FloatMap:GetGeostrophicWindDirections(zone)

	if zone == mg.NPOLAR then
		return mg.SW,mg.W
	elseif zone == mg.NTEMPERATE then
		return mg.NE,mg.E
	elseif zone == mg.NEQUATOR then
		return mg.SW,mg.W
	elseif zone == mg.SEQUATOR then
		return mg.NW,mg.W
	elseif zone == mg.STEMPERATE then
		return mg.SE, mg.E
	else
		return mg.NW,mg.W
	end
	return -1,-1
end

function FloatMap:GetGeostrophicPressure(lat)
	local latRange = nil
	local latPercent = nil
	local pressure = nil
	if lat > mg.polarFrontLatitude then
		latRange = 90.0 - mg.polarFrontLatitude
		latPercent = (lat - mg.polarFrontLatitude)/latRange
		pressure = 1.0 - latPercent
	elseif lat >= mg.horseLatitudes then
		latRange = mg.polarFrontLatitude - mg.horseLatitudes
		latPercent = (lat - mg.horseLatitudes)/latRange
		pressure = latPercent
	elseif lat >= 0.0 then
		latRange = mg.horseLatitudes - 0.0
		latPercent = (lat - 0.0)/latRange
		pressure = 1.0 - latPercent
	elseif lat > -mg.horseLatitudes then
		latRange = 0.0 + mg.horseLatitudes
		latPercent = (lat + mg.horseLatitudes)/latRange
		pressure = latPercent
	elseif lat >= -mg.polarFrontLatitude then
		latRange = -mg.horseLatitudes + mg.polarFrontLatitude
		latPercent = (lat + mg.polarFrontLatitude)/latRange
		pressure = 1.0 - latPercent
	else
		latRange = -mg.polarFrontLatitude + 90.0
		latPercent = (lat + 90)/latRange
		pressure = latPercent
	end
-- Prevents excessively high and low pressures which helps distribute rain more evenly in the affected areas. -Bobert13
	pressure = pressure + 1
	if pressure > 1.5 then
		pressure = pressure * mg.pressureNorm
	else
		pressure = pressure / mg.pressureNorm
	end
	pressure = pressure - 1
-- FIN -Bobert13
	--print(pressure)
	return pressure
end

function FloatMap:ApplyFunction(func)
	for i = 0,self.length - 1 do
		self.data[i] = func(self.data[i])
	end
end

function FloatMap:GetRadiusAroundHex(x,y,radius)
	local list = {}
	table.insert(list,{x,y})
	if radius == 0 then
		return list
	end

	local hereX = x
	local hereY = y

	--make a circle for each radius
	for r = 1,radius do
		--start 1 to the west
		hereX,hereY = self:GetNeighbor(hereX,hereY,mg.W)
		if self:IsOnMap(hereX,hereY) then
			table.insert(list,{hereX,hereY})
		end
		--Go r times to the NE
		for z = 1,r do
			hereX, hereY = self:GetNeighbor(hereX,hereY,mg.NE)
			if self:IsOnMap(hereX,hereY) then
				table.insert(list,{hereX,hereY})
			end
		end
		--Go r times to the E
		for z = 1,r do
			hereX, hereY = self:GetNeighbor(hereX,hereY,mg.E)
			if self:IsOnMap(hereX,hereY) then
				table.insert(list,{hereX,hereY})
			end
		end
		--Go r times to the SE
		for z = 1,r do
			hereX, hereY = self:GetNeighbor(hereX,hereY,mg.SE)
			if self:IsOnMap(hereX,hereY) then
				table.insert(list,{hereX,hereY})
			end
		end
		--Go r times to the SW
		for z = 1,r do
			hereX, hereY = self:GetNeighbor(hereX,hereY,mg.SW)
			if self:IsOnMap(hereX,hereY) then
				table.insert(list,{hereX,hereY})
			end
		end
		--Go r times to the W
		for z = 1,r do
			hereX, hereY = self:GetNeighbor(hereX,hereY,mg.W)
			if self:IsOnMap(hereX,hereY) then
				table.insert(list,{hereX,hereY})
			end
		end
		--Go r - 1 times to the NW!!!!!
		for z = 1,r - 1 do
			hereX, hereY = self:GetNeighbor(hereX,hereY,mg.NW)
			if self:IsOnMap(hereX,hereY) then
				table.insert(list,{hereX,hereY})
			end
		end
		--one extra NW to set up for next circle
		hereX, hereY = self:GetNeighbor(hereX,hereY,mg.NW)
	end
	return list
end

function FloatMap:GetAverageInHex(x,y,radius)
	local sum = 0
	local numPlots = 0
	--print("GetAverageInHex A")
	local nearPlotIDs = Plot_GetPlotsInCircleFast(x, y, radius)
	--print("GetAverageInHex B")
	for _, nearPlotID in pairs(nearPlotIDs) do
		sum = sum + self.data[nearPlotID]
		numPlots = numPlots + 1
	end
	return sum / numPlots
end

function FloatMap:GetStdDevInHex(x,y,radius)
	local average = 0
	local numPlots = 0
	--print("GetStdDevInHex A")
	local nearPlotIDs = Plot_GetPlotsInCircleFast(x, y, radius)
	--print("GetStdDevInHex B")

	for _, nearPlotID in ipairs(nearPlotIDs) do
		average = average + self.data[nearPlotID]
		numPlots = numPlots + 1
	end
	average = average / numPlots

	local deviation = 0.0
	for _, nearPlotID in ipairs(nearPlotIDs) do
		deviation = deviation + (self.data[nearPlotID] - average) ^ 2
	end
	return math.sqrt(deviation/numPlots)
end

function FloatMap:Smooth(radius)
	local dataCopy = {}
	local i = 0
	for y = 0, self.height - 1 do
		for x = 0, self.width - 1 do
			dataCopy[i] = self:GetAverageInHex(x,y,radius)
			i = i + 1
		end
	end
	self.data = dataCopy
end

function FloatMap:Deviate(radius)
	local dataCopy = {}
	local i = 0
	for y = 0, self.height - 1 do
		for x = 0, self.width - 1 do
			dataCopy[i] = self:GetStdDevInHex(x,y,radius)
			i = i + 1
		end
	end
	self.data = dataCopy
end

function FloatMap:IsOnMap(x,y)
	local i = self:GetIndex(x,y)
	if i == -1 then
		return false
	end
	return true
end

function FloatMap:Save(name)
	print("saving " .. name .. "..")
	local str = self.width .. "," .. self.height
	for i = 0,self.length - 1 do
		str = str .. "," .. self.data[i]
	end
	local file = io.open(name,"w+")
	file:write(str)
	file:close()
	print("bitmap saved as " .. name .. ".")
end



--
-- AreaMap
--

PWAreaMap = inheritsFrom(FloatMap)

function PWAreaMap:New(width,height,wrapX,wrapY)
	local new_inst = FloatMap:New(width,height,wrapX,wrapY)
	setmetatable(new_inst, {__index = PWAreaMap})	--setup metatable

	new_inst.areaList = {}
	new_inst.segStack = {}
	return new_inst
end

function PWAreaMap:DefineAreas(matchFunction)
	--zero map data
	for i = 0,self.width*self.height - 1 do
		self.data[i] = 0.0
	end

	self.areaList = {}
	local currentAreaID = 0
	for y = 0, self.height - 1 do
		for x = 0, self.width - 1 do
			local i = self:GetIndex(x,y)
			if self.data[i] == 0 then
				currentAreaID = currentAreaID + 1
				local area = PWArea:New(currentAreaID,x,y,matchFunction(x,y))
				--str = string.format("Filling area %d, matchFunction(x = %d,y = %d) = %s",area.id,x,y,tostring(matchFunction(x,y)))
				--print(str)
				self:FillArea(x,y,area,matchFunction)
				table.insert(self.areaList, area)

			end
		end
	end
end

function PWAreaMap:FillArea(x,y,area,matchFunction)
	self.segStack = {}
	local seg = LineSeg:New(y,x,x,1)
	Push(self.segStack,seg)
	seg = LineSeg:New(y + 1,x,x,-1)
	Push(self.segStack,seg)
	while #self.segStack > 0 do
		seg = Pop(self.segStack)
		self:ScanAndFillLine(seg,area,matchFunction)
	end
end

function PWAreaMap:ScanAndFillLine(seg,area,matchFunction)

	--str = string.format("Processing line y = %d, xLeft = %d, xRight = %d, dy = %d -------",seg.y,seg.xLeft,seg.xRight,seg.dy)
	--print(str)
	if self:ValidateY(seg.y + seg.dy) == -1 then
		return
	end

	local odd = (seg.y + seg.dy) % 2
	local notOdd = seg.y % 2
	--str = string.format("odd = %d, notOdd = %d",odd,notOdd)
	--print(str)

	local lineFound = 0
	local xStop = nil
	if self.wrapX then
		xStop = 0 - (self.width * 30)
	else
		xStop = -1
	end
	local leftExtreme = nil
	for leftExt = seg.xLeft - odd,xStop + 1,-1 do
		leftExtreme = leftExt --need this saved
		local x = self:ValidateX(leftExtreme)
		local y = self:ValidateY(seg.y + seg.dy)
		local i = self:GetIndex(x,y)
		if self.data[i] == 0 and area.trueMatch == matchFunction(x,y) then
			self.data[i] = area.id
			area.size = area.size + 1
			lineFound = 1
		else
			--if no line was found, then leftExtreme is fine, but if
			--a line was found going left, then we need to increment
            --xLeftExtreme to represent the inclusive end of the line
			if lineFound == 1 then
				leftExtreme = leftExtreme + 1
			end
			break
		end
	end
	local rightExtreme = nil

	--now scan right to find extreme right, place each found segment on stack
	if self.wrapX then
		xStop = self.width * 20
	else
		xStop = self.width
	end
	for rightExt = seg.xLeft + lineFound - odd,xStop - 1 do
		rightExtreme = rightExt --need this saved
		local x = self:ValidateX(rightExtreme)
		local y = self:ValidateY(seg.y + seg.dy)
		local i = self:GetIndex(x,y)
		if self.data[i] == 0 and area.trueMatch == matchFunction(x,y) then
			self.data[i] = area.id
			area.size = area.size + 1
			--print("adding to area")
			if lineFound == 0 then
				lineFound = 1 --starting new line
				leftExtreme = rightExtreme
			end
		elseif lineFound == 1 then --found the right end of a line segment
			lineFound = 0
			--put same direction on stack
			local newSeg = LineSeg:New(y,leftExtreme,rightExtreme - 1,seg.dy)
			Push(self.segStack,newSeg)
			--determine if we must put reverse direction on stack
			if leftExtreme < seg.xLeft - odd or rightExtreme >= seg.xRight + notOdd then
				--out of shadow so put reverse direction on stack
				newSeg = LineSeg:New(y,leftExtreme,rightExtreme - 1,-seg.dy)
				Push(self.segStack,newSeg)
			end
			if(rightExtreme >= seg.xRight + notOdd) then
				break
			end
		elseif lineFound == 0 and rightExtreme >= seg.xRight + notOdd then
			break --past the end of the parent line and no line found
		end
		--continue finding segments
	end
	if lineFound == 1 then --still needing a line to be put on stack
		print("still need line segments")
		lineFound = 0
		--put same direction on stack
		local newSeg = LineSeg:New(seg.y + seg.dy,leftExtreme,rightExtreme - 1,seg.dy)
		Push(self.segStack,newSeg)
		str = string.format("  pushing y = %d, xLeft = %d, xRight = %d, dy = %d",seg.y + seg.dy,leftExtreme,rightExtreme - 1,seg.dy)
		print(str)
		--determine if we must put reverse direction on stack
		if leftExtreme < seg.xLeft - odd or rightExtreme >= seg.xRight + notOdd then
			--out of shadow so put reverse direction on stack
			newSeg = LineSeg:New(seg.y + seg.dy,leftExtreme,rightExtreme - 1,-seg.dy)
			Push(self.segStack,newSeg)
			str = string.format("  pushing y = %d, xLeft = %d, xRight = %d, dy = %d",seg.y + seg.dy,leftExtreme,rightExtreme - 1,-seg.dy)
			print(str)
		end
	end
end

function PWAreaMap:GetAreaByID(id)
	for i = 1,#self.areaList do
		if self.areaList[i].id == id then
			return self.areaList[i]
		end
	end
	error("Can't find area id in AreaMap.areaList")
end

function PWAreaMap:ValidateY(y)
	local yy = nil
	if self.wrapY then
		yy = y % self.height
	elseif y < 0 or y >= self.height then
		return -1
	else
		yy = y
	end
	return yy
end

function PWAreaMap:ValidateX(x)
	local xx = nil
	if self.wrapX then
		xx = x % self.width
	elseif x < 0 or x >= self.width then
		return -1
	else
		xx = x
	end
	return xx
end

function PWAreaMap:PrintAreaList()
	for i=1,#self.areaList do
		local id = self.areaList[i].id
		local seedx = self.areaList[i].seedx
		local seedy = self.areaList[i].seedy
		local size = self.areaList[i].size
		local trueMatch = self.areaList[i].trueMatch
		local str = string.format("area id = %d, trueMatch = %s, size = %d, seedx = %d, seedy = %d",id,tostring(trueMatch),size,seedx,seedy)
		print(str)
	end
end

--Area class
PWArea = inheritsFrom(nil)
function PWArea:New(id,seedx,seedy,trueMatch)
	local new_inst = {}
	setmetatable(new_inst, {__index = PWArea})	--setup metatable

	new_inst.id = id
	new_inst.seedx = seedx
	new_inst.seedy = seedy
	new_inst.trueMatch = trueMatch
	new_inst.size = 0

	return new_inst
end

--LineSeg class
LineSeg = inheritsFrom(nil)
function LineSeg:New(y,xLeft,xRight,dy)
	local new_inst = {}
	setmetatable(new_inst, {__index = LineSeg})	--setup metatable

	new_inst.y = y
	new_inst.xLeft = xLeft
	new_inst.xRight = xRight
	new_inst.dy = dy

	return new_inst
end



--
-- ElevationMap
--

ElevationMap = inheritsFrom(FloatMap)

function ElevationMap:New(width, height, wrapX, wrapY)
	local new_inst = FloatMap:New(width,height,wrapX,wrapY)
	setmetatable(new_inst, {__index = ElevationMap})	--setup metatable
	return new_inst
end
function ElevationMap:IsBelowSeaLevel(x,y)
	local i = self:GetIndex(x,y)
	if self.data[i] < self.seaLevelThreshold then
		return true
	else
		return false
	end
end




--
-- Rivers
--


function AddRivers()
	local mapW, mapH = Map.GetGridSize()


	for y = 0, mapH-1 do
		for x = 0,mapW-1 do
			local plot = Map.GetPlot(x, y)

			local WOfRiver, NWOfRiver, NEOfRiver = riverMap:GetFlowDirections(x,y)

			if WOfRiver == mg.flowNONE then
				plot:SetWOfRiver(false,WOfRiver)
			else
				local xx,yy = elevationMap:GetNeighbor(x,y,mg.E)
				local nPlot = Map.GetPlot(xx,yy)
				if plot:IsMountain() and nPlot:IsMountain() then
					plot:SetPlotType(PlotTypes.PLOT_HILLS,false,false)
				end
				plot:SetWOfRiver(true,WOfRiver)
			end

			if NWOfRiver == mg.flowNONE then
				plot:SetNWOfRiver(false,NWOfRiver)
			else
				local xx,yy = elevationMap:GetNeighbor(x,y,mg.SE)
				local nPlot = Map.GetPlot(xx,yy)
				if plot:IsMountain() and nPlot:IsMountain() then
					plot:SetPlotType(PlotTypes.PLOT_HILLS,false,false)
				end
				plot:SetNWOfRiver(true,NWOfRiver)
			end

			if NEOfRiver == mg.flowNONE then
				plot:SetNEOfRiver(false,NEOfRiver)
			else
				local xx,yy = elevationMap:GetNeighbor(x,y,mg.SW)
				local nPlot = Map.GetPlot(xx,yy)
				if plot:IsMountain() and nPlot:IsMountain() then
					plot:SetPlotType(PlotTypes.PLOT_HILLS,false,false)
				end
				plot:SetNEOfRiver(true,NEOfRiver)
			end
		end
	end
end

function Plot_GetPreviousRiverPlot(plot, edgeDirection)
	local flowDirection = Plot_GetRiverFlowDirection(plot, edgeDirection)
	local nearDirection = mg.C
	if edgeDirection == (flowDirection + 1) % 6 then
		nearDirection = (flowDirection + 2) % 6
	else
		nearDirection = (flowDirection + 3) % 6
	end
	local nearPlot = Map.PlotDirection(plot:GetX(), plot:GetY(), nearDirection)
	local edgeA = (flowDirection - 1) % 6
	local edgeB = (flowDirection + 0) % 6
	local flowA = (flowDirection + 1) % 6
	local flowB = (flowDirection - 1) % 6
	return nearPlot, edgeA, edgeB, flowA, flowB
end

function Plot_GetNextRiverPlot(plot, edgeDirection)
	local flowDirection = Plot_GetRiverFlowDirection(plot, edgeDirection)
	local nearDirection = mg.C
	if edgeDirection == (flowDirection + 1) % 6 then
		nearDirection = (flowDirection + 0) % 6
	else
		nearDirection = (flowDirection - 1) % 6
	end
	local nearPlot = Map.PlotDirection(plot:GetX(), plot:GetY(), nearDirection)
	local edgeA = (flowDirection + 2) % 6
	local edgeB = (flowDirection + 3) % 6
	local flowA = (flowDirection + 4) % 6
	local flowB = (flowDirection + 2) % 6
	return nearPlot, edgeA, flowA, edgeB, flowB
end

function Plot_GetRiverFlowDirection(plot, edgeDirection)
	if edgeDirection == mg.E then
		return plot:GetRiverEFlowDirection()
	elseif edgeDirection == mg.SW then
		return plot:GetRiverSWFlowDirection()
	elseif edgeDirection == mg.SE then
		return plot:GetRiverSEFlowDirection()
	else
		nextPlot = Map.PlotDirection(plot:GetX(), plot:GetY(), edgeDirection)
		if not nextPlot then
			return mg.flowNONE
		end
		if edgeDirection == mg.W then
			return nextPlot:GetRiverEFlowDirection()
		elseif edgeDirection == mg.NE then
			return nextPlot:GetRiverSWFlowDirection()
		elseif edgeDirection == mg.NW then
			return nextPlot:GetRiverSEFlowDirection()
		end
	end
	return mg.flowNONE
end

function Plot_GetRiverRotation(plot, edgeDirection)
	if not Plot_IsRiver(plot, edgeDirection) then
		return 0 -- no river
	end
	if Plot_GetRiverFlowDirection(plot, edgeDirection) == (edgeDirection + 2) % 6 then
		return 1 -- clockwise
	end
	return -1 -- counterclockwise
end

function Plot_IsRiver(plot, edgeDirection)
	if edgeDirection then
		return Plot_IsRiverInDirection(plot, edgeDirection)
	end
	for _, edgeDirection in pairs(mg.edgeDirections) do
		if Plot_IsRiverInDirection(plot, edgeDirection) then
			return true
		end
	end
	return false
end

function Plot_IsRiverInDirection(plot, edgeDirection)
	if edgeDirection == mg.E then
		return plot:IsWOfRiver()
	elseif edgeDirection == mg.SE then
		return plot:IsNWOfRiver()
	elseif edgeDirection == mg.SW then
		return plot:IsNEOfRiver()
	else
		nextPlot = Map.PlotDirection(plot:GetX(), plot:GetY(), edgeDirection)
		if not nextPlot then
			return false
		end
		if edgeDirection == mg.W then
			return nextPlot:IsWOfRiver()
		elseif edgeDirection == mg.NW then
			return nextPlot:IsNWOfRiver()
		elseif edgeDirection == mg.NE then
			return nextPlot:IsNEOfRiver()
		end
	end
	return false
end

function Plot_SetRiver(plot, edgeDirection, flowDirection)
	local isRiver = (flowDirection ~= mg.flowNONE)
	if edgeDirection == mg.E then
		plot:SetWOfRiver(isRiver, flowDirection)
	elseif edgeDirection == mg.SE then
		plot:SetNWOfRiver(isRiver, flowDirection)
	elseif edgeDirection == mg.SW then
		plot:SetNEOfRiver(isRiver, flowDirection)
	else
		plot = Map.PlotDirection(plot:GetX(), plot:GetY(), edgeDirection)
		if not plot then
			return false
		end
		if edgeDirection == mg.W then
			plot:SetWOfRiver(isRiver, flowDirection)
		elseif edgeDirection == mg.NW then
			plot:SetNWOfRiver(isRiver, flowDirection)
		elseif edgeDirection == mg.NE then
			plot:SetNEOfRiver(isRiver, flowDirection)
		end
	end
	return true
end

RiverMap = inheritsFrom(nil)

function RiverMap:New(elevationMap)
	local new_inst = {}
	setmetatable(new_inst, {__index = RiverMap})

	new_inst.elevationMap = elevationMap
	new_inst.riverData = {}
	for y = 0,new_inst.elevationMap.height - 1 do
		for x = 0,new_inst.elevationMap.width - 1 do
			local i = new_inst.elevationMap:GetIndex(x,y)
			new_inst.riverData[i] = RiverHex:New(x,y)
		end
	end

	return new_inst
end

function RiverMap:GetJunction(x,y,isNorth)
	local i = self.elevationMap:GetIndex(x,y)
	if isNorth then
		return self.riverData[i].northJunction
	else
		return self.riverData[i].southJunction
	end
end

function RiverMap:GetJunctionNeighbor(direction,junction)
	local xx = nil
	local yy = nil
	local ii = nil
	local neighbor = nil
	local odd = junction.y % 2
	if direction == mg.NOFLOW then
		error("can't get junction neighbor in direction NOFLOW")
	elseif direction == mg.WESTFLOW then
		xx = junction.x + odd - 1
		if junction.isNorth then
			yy = junction.y + 1
		else
			yy = junction.y - 1
		end
		ii = self.elevationMap:GetIndex(xx,yy)
		if ii ~= -1 then
			neighbor = self:GetJunction(xx,yy,not junction.isNorth)
			return neighbor
		end
	elseif direction == mg.EASTFLOW then
		xx = junction.x + odd
		if junction.isNorth then
			yy = junction.y + 1
		else
			yy = junction.y - 1
		end
		ii = self.elevationMap:GetIndex(xx,yy)
		if ii ~= -1 then
			neighbor = self:GetJunction(xx,yy,not junction.isNorth)
			return neighbor
		end
	elseif direction == mg.VERTFLOW then
		xx = junction.x
		if junction.isNorth then
			yy = junction.y + 2
		else
			yy = junction.y - 2
		end
		ii = self.elevationMap:GetIndex(xx,yy)
		if ii ~= -1 then
			neighbor = self:GetJunction(xx,yy,not junction.isNorth)
			return neighbor
		end
	end

	return nil --neighbor off map
end

--Get the west or east hex neighboring this junction
function RiverMap:GetRiverHexNeighbor(junction,westNeighbor)
	local xx = nil
	local yy = nil
	local ii = nil
	local odd = junction.y % 2
	if junction.isNorth then
		yy = junction.y + 1
	else
		yy = junction.y - 1
	end
	if westNeighbor then
		xx = junction.x + odd - 1
	else
		xx = junction.x + odd
	end

	ii = self.elevationMap:GetIndex(xx,yy)
	if ii ~= -1 then
		return self.riverData[ii]
	end

	return nil
end

function RiverMap:SetJunctionAltitudes()
	for y = 0,self.elevationMap.height - 1 do
		for x = 0,self.elevationMap.width - 1 do
			local i = self.elevationMap:GetIndex(x,y)
			local vertAltitude = self.elevationMap.data[i]
			local westAltitude = nil
			local eastAltitude = nil
			local vertNeighbor = self.riverData[i]
			local westNeighbor = nil
			local eastNeighbor = nil
			local xx = nil
			local yy = nil
			local ii = nil

			--first do north
			westNeighbor = self:GetRiverHexNeighbor(vertNeighbor.northJunction,true)
			eastNeighbor = self:GetRiverHexNeighbor(vertNeighbor.northJunction,false)

			if westNeighbor ~= nil then
				ii = self.elevationMap:GetIndex(westNeighbor.x,westNeighbor.y)
			else
				ii = -1
			end

			if ii ~= -1 then
				westAltitude = self.elevationMap.data[ii]
			else
				westAltitude = vertAltitude
			end

			if eastNeighbor ~= nil then
				ii = self.elevationMap:GetIndex(eastNeighbor.x, eastNeighbor.y)
			else
				ii = -1
			end

			if ii ~= -1 then
				eastAltitude = self.elevationMap.data[ii]
			else
				eastAltitude = vertAltitude
			end

			vertNeighbor.northJunction.altitude = math.min(math.min(vertAltitude,westAltitude),eastAltitude)

			--then south
			westNeighbor = self:GetRiverHexNeighbor(vertNeighbor.southJunction,true)
			eastNeighbor = self:GetRiverHexNeighbor(vertNeighbor.southJunction,false)

			if westNeighbor ~= nil then
				ii = self.elevationMap:GetIndex(westNeighbor.x,westNeighbor.y)
			else
				ii = -1
			end

			if ii ~= -1 then
				westAltitude = self.elevationMap.data[ii]
			else
				westAltitude = vertAltitude
			end

			if eastNeighbor ~= nil then
				ii = self.elevationMap:GetIndex(eastNeighbor.x, eastNeighbor.y)
			else
				ii = -1
			end

			if ii ~= -1 then
				eastAltitude = self.elevationMap.data[ii]
			else
				eastAltitude = vertAltitude
			end

			vertNeighbor.southJunction.altitude = math.min(math.min(vertAltitude,westAltitude),eastAltitude)
		end
	end
end

function RiverMap:isLake(junction)

	--first exclude the map edges that don't have neighbors
	if junction.y == 0 and junction.isNorth == false then
		return false
	elseif junction.y == self.elevationMap.height - 1 and junction.isNorth == true then
		return false
	end

	--exclude altitudes below sea level
	if junction.altitude < self.elevationMap.seaLevelThreshold then
		return false
	end

	--if debugTime then print(string.format("junction = (%d,%d) N = %s, alt = %f",junction.x,junction.y,tostring(junction.isNorth),junction.altitude)) end

	local vertNeighbor = self:GetJunctionNeighbor(mg.VERTFLOW,junction)
	local vertAltitude = nil
	if vertNeighbor == nil then
		vertAltitude = junction.altitude
		--print("--vertNeighbor == nil")
	else
		vertAltitude = vertNeighbor.altitude
		--if debugTime then print(string.format("--vertNeighbor = (%d,%d) N = %s, alt = %f",vertNeighbor.x,vertNeighbor.y,tostring(vertNeighbor.isNorth),vertNeighbor.altitude)) end
	end

	local westNeighbor = self:GetJunctionNeighbor(mg.WESTFLOW,junction)
	local westAltitude = nil
	if westNeighbor == nil then
		westAltitude = junction.altitude
		--print("--westNeighbor == nil")
	else
		westAltitude = westNeighbor.altitude
		--if debugTime then print(string.format("--westNeighbor = (%d,%d) N = %s, alt = %f",westNeighbor.x,westNeighbor.y,tostring(westNeighbor.isNorth),westNeighbor.altitude)) end
	end

	local eastNeighbor = self:GetJunctionNeighbor(mg.EASTFLOW,junction)
	local eastAltitude = nil
	if eastNeighbor == nil then
		eastAltitude = junction.altitude
		--print("--eastNeighbor == nil")
	else
		eastAltitude = eastNeighbor.altitude
		--if debugTime then print(string.format("--eastNeighbor = (%d,%d) N = %s, alt = %f",eastNeighbor.x,eastNeighbor.y,tostring(eastNeighbor.isNorth),eastNeighbor.altitude)) end
	end

	local lowest = math.min(vertAltitude,math.min(westAltitude,math.min(eastAltitude,junction.altitude)))

	if lowest == junction.altitude then
		--print("--is lake")
		return true
	end
	--print("--is not lake")
	return false
end

--get the average altitude of the two lowest neighbors that are higher than the junction altitude.
function RiverMap:GetLowerNeighborAverage(junction)
	local vertNeighbor = self:GetJunctionNeighbor(mg.VERTFLOW,junction)
	local vertAltitude = nil
	if vertNeighbor == nil then
		vertAltitude = junction.altitude
	else
		vertAltitude = vertNeighbor.altitude
	end

	local westNeighbor = self:GetJunctionNeighbor(mg.WESTFLOW,junction)
	local westAltitude = nil
	if westNeighbor == nil then
		westAltitude = junction.altitude
	else
		westAltitude = westNeighbor.altitude
	end

	local eastNeighbor = self:GetJunctionNeighbor(mg.EASTFLOW,junction)
	local eastAltitude = nil
	if eastNeighbor == nil then
		eastAltitude = junction.altitude
	else
		eastAltitude = eastNeighbor.altitude
	end

	local nList = {vertAltitude,westAltitude,eastAltitude}
	table.sort(nList)
	local avg = nil
	if nList[1] > junction.altitude then
		avg = (nList[1] + nList[2])/2.0
	elseif nList[2] > junction.altitude then
		avg = (nList[2] + nList[3])/2.0
	elseif nList[3] > junction.altitude then
		avg = (nList[3] + junction.altitude)/2.0
	else
		avg = junction.altitude --all neighbors are the same height. Dealt with later
	end
	return avg
end

--this function alters the drainage pattern
function RiverMap:SiltifyLakes()
	local lakeList = {}
	local onQueueMapNorth = {}
	local onQueueMapSouth = {}
	for y = 0,self.elevationMap.height - 1 do
		for x = 0,self.elevationMap.width - 1 do
			local i = self.elevationMap:GetIndex(x,y)
			onQueueMapNorth[i] = false
			onQueueMapSouth[i] = false
			if self:isLake(self.riverData[i].northJunction) then
				Push(lakeList,self.riverData[i].northJunction)
				onQueueMapNorth[i] = true
			end
			if self:isLake(self.riverData[i].southJunction) then
				Push(lakeList,self.riverData[i].southJunction)
				onQueueMapSouth[i] = true
			end
		end
	end

	local longestLakeList = #lakeList
	local shortestLakeList = #lakeList
	local iterations = 0
	local debugOn = false
	--if debugTime then print(string.format("initial lake count = %d",longestLakeList)) end
	while #lakeList > 0 do
		--if debugTime then print(string.format("length of lakeList = %d",#lakeList)) end
		iterations = iterations + 1
		if #lakeList > longestLakeList then
			longestLakeList = #lakeList
		end

		if #lakeList < shortestLakeList then
			shortestLakeList = #lakeList
			--if debugTime then print(string.format("shortest lake list = %d, iterations = %d",shortestLakeList,iterations)) end
			iterations = 0
		end

		if iterations > 1000000 then
			debugOn = true
		end

		if iterations > 1001000 then
			error("endless loop in lake siltification. check logs")
		end

		local junction = Pop(lakeList)
		local i = self.elevationMap:GetIndex(junction.x,junction.y)
		if junction.isNorth then
			onQueueMapNorth[i] = false
		else
			onQueueMapSouth[i] = false
		end

		if debugOn then
			if debugTime then print(string.format("processing (%d,%d) N=%s alt=%f",junction.x,junction.y,tostring(junction.isNorth),junction.altitude)) end
		end

		local avgLowest = self:GetLowerNeighborAverage(junction)

		if debugOn then
			if debugTime then print(string.format("--avgLowest == %f",avgLowest)) end
		end

		if avgLowest < junction.altitude + 0.005 then --cant use == in fp comparison
			junction.altitude = avgLowest + 0.005
			if debugOn then
				print("--adding 0.005 to avgLowest")
			end
		else
			junction.altitude = avgLowest
		end

		if debugOn then
			if debugTime then print(string.format("--changing altitude to %f",junction.altitude)) end
		end

		for dir = mg.WESTFLOW,mg.VERTFLOW do
			local neighbor = self:GetJunctionNeighbor(dir,junction)
			if debugOn and neighbor == nil then
				if debugTime then print(string.format("--nil neighbor at direction = %d",dir)) end
			end
			if neighbor ~= nil and self:isLake(neighbor) then
				local i = self.elevationMap:GetIndex(neighbor.x,neighbor.y)
				if neighbor.isNorth == true and onQueueMapNorth[i] == false then
					Push(lakeList,neighbor)
					onQueueMapNorth[i] = true
					if debugOn then
						if debugTime then print(string.format("--pushing (%d,%d) N=%s alt=%f",neighbor.x,neighbor.y,tostring(neighbor.isNorth),neighbor.altitude)) end
					end
				elseif neighbor.isNorth == false and onQueueMapSouth[i] == false then
					Push(lakeList,neighbor)
					onQueueMapSouth[i] = true
					if debugOn then
						if debugTime then print(string.format("--pushing (%d,%d) N=%s alt=%f",neighbor.x,neighbor.y,tostring(neighbor.isNorth),neighbor.altitude)) end
					end
				end
			end
		end
	end
end

function RiverMap:SetFlowDestinations()
	junctionList = {}
	for y = 0,self.elevationMap.height - 1 do
		for x = 0,self.elevationMap.width - 1 do
			local i = self.elevationMap:GetIndex(x,y)
			table.insert(junctionList,self.riverData[i].northJunction)
			table.insert(junctionList,self.riverData[i].southJunction)
		end
	end

	table.sort(junctionList,function (a,b) return a.altitude > b.altitude end)

	for n=1,#junctionList do
		local junction = junctionList[n]
		local validList = self:GetValidFlows(junction)
		if #validList > 0 then
			local choice = PWRandint(1,#validList)
			junction.flow = validList[choice]
		else
			junction.flow = mg.NOFLOW
		end
	end
end

function RiverMap:GetValidFlows(junction)
	local validList = {}
	for dir = mg.WESTFLOW,mg.VERTFLOW do
		neighbor = self:GetJunctionNeighbor(dir,junction)
		if neighbor ~= nil and neighbor.altitude < junction.altitude then
			table.insert(validList,dir)
		end
	end
	return validList
end

function RiverMap:IsTouchingOcean(junction)

	if elevationMap:IsBelowSeaLevel(junction.x,junction.y) then
		return true
	end
	local westNeighbor = self:GetRiverHexNeighbor(junction,true)
	local eastNeighbor = self:GetRiverHexNeighbor(junction,false)

	if westNeighbor == nil or elevationMap:IsBelowSeaLevel(westNeighbor.x,westNeighbor.y) then
		return true
	end
	if eastNeighbor == nil or elevationMap:IsBelowSeaLevel(eastNeighbor.x,eastNeighbor.y) then
		return true
	end
	return false
end

function RiverMap:SetRiverSizes(rainfallMap)
	local junctionList = {} --only include junctions not touching ocean in this list
	for y = 0,self.elevationMap.height - 1 do
		for x = 0,self.elevationMap.width - 1 do
			local i = self.elevationMap:GetIndex(x,y)
			if not self:IsTouchingOcean(self.riverData[i].northJunction) then
				table.insert(junctionList,self.riverData[i].northJunction)
			end
			if not self:IsTouchingOcean(self.riverData[i].southJunction) then
				table.insert(junctionList,self.riverData[i].southJunction)
			end
		end
	end

	table.sort(junctionList,function (a,b) return a.altitude > b.altitude end)

	for n=1,#junctionList do
		local junction = junctionList[n]
		local nextJunction = junction
		local i = self.elevationMap:GetIndex(junction.x,junction.y)
		while true do
			nextJunction.size = (nextJunction.size + rainfallMap.data[i]) * mg.riverRainCheatFactor
			if nextJunction.flow == mg.NOFLOW or self:IsTouchingOcean(nextJunction) then
				nextJunction.size = 0.0
				break
			end
			nextJunction = self:GetJunctionNeighbor(nextJunction.flow,nextJunction)
		end
	end

	--now sort by river size to find river threshold
	table.sort(junctionList,function (a,b) return a.size > b.size end)
	local riverIndex = math.floor(mg.riverPercent * #junctionList)
	self.riverThreshold = junctionList[riverIndex].size
	if debugTime then print(string.format("river threshold = %f",self.riverThreshold)) end

end

--This function returns the flow directions needed by civ
function RiverMap:GetFlowDirections(x,y)
	--if debugTime then print(string.format("Get flow dirs for %d,%d",x,y)) end
	local i = elevationMap:GetIndex(x,y)

	local WOfRiver = mg.flowNONE
	local xx,yy = elevationMap:GetNeighbor(x,y,mg.NE)
	local ii = elevationMap:GetIndex(xx,yy)
	if ii ~= -1 and self.riverData[ii].southJunction.flow == mg.VERTFLOW and self.riverData[ii].southJunction.size > self.riverThreshold then
		--if debugTime then print(string.format("--NE(%d,%d) south flow=%d, size=%f",xx,yy,self.riverData[ii].southJunction.flow,self.riverData[ii].southJunction.size)) end
		WOfRiver = mg.flowS
	end
	xx,yy = elevationMap:GetNeighbor(x,y,mg.SE)
	ii = elevationMap:GetIndex(xx,yy)
	if ii ~= -1 and self.riverData[ii].northJunction.flow == mg.VERTFLOW and self.riverData[ii].northJunction.size > self.riverThreshold then
		--if debugTime then print(string.format("--SE(%d,%d) north flow=%d, size=%f",xx,yy,self.riverData[ii].northJunction.flow,self.riverData[ii].northJunction.size)) end
		WOfRiver = mg.flowN
	end

	local NWOfRiver = mg.flowNONE
	xx,yy = elevationMap:GetNeighbor(x,y,mg.SE)
	ii = elevationMap:GetIndex(xx,yy)
	if ii ~= -1 and self.riverData[ii].northJunction.flow == mg.WESTFLOW and self.riverData[ii].northJunction.size > self.riverThreshold then
		NWOfRiver = mg.flowSW
	end
	if self.riverData[i].southJunction.flow == mg.EASTFLOW and self.riverData[i].southJunction.size > self.riverThreshold then
		NWOfRiver = mg.flowNE
	end

	local NEOfRiver = mg.flowNONE
	xx,yy = elevationMap:GetNeighbor(x,y,mg.SW)
	ii = elevationMap:GetIndex(xx,yy)
	if ii ~= -1 and self.riverData[ii].northJunction.flow == mg.EASTFLOW and self.riverData[ii].northJunction.size > self.riverThreshold then
		NEOfRiver = mg.flowSE
	end
	if self.riverData[i].southJunction.flow == mg.WESTFLOW and self.riverData[i].southJunction.size > self.riverThreshold then
		NEOfRiver = mg.flowNW
	end

	return WOfRiver,NWOfRiver,NEOfRiver
end

--RiverHex class
RiverHex = inheritsFrom(nil)
function RiverHex:New(x, y)
	local new_inst = {}
	setmetatable(new_inst, {__index = RiverHex})

	new_inst.x = x
	new_inst.y = y
	new_inst.northJunction = RiverJunction:New(x,y,true)
	new_inst.southJunction = RiverJunction:New(x,y,false)

	return new_inst
end

--RiverJunction class
RiverJunction = inheritsFrom(nil)
function RiverJunction:New(x,y,isNorth)
	local new_inst = {}
	setmetatable(new_inst, {__index = RiverJunction})

	new_inst.x = x
	new_inst.y = y
	new_inst.isNorth = isNorth
	new_inst.altitude = 0.0
	new_inst.flow = mg.NOFLOW
	new_inst.size = 0.0

	return new_inst
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
-- sRoute                  - one of mg.routes (see above)
-- bShortestRoute          - true to find the shortest route
-- sHighlight              - one of the highlight keys (see above)
-- fBlockaded              - call-back function of the form f(pPlot, pPlayer) to determine if a plot is blocked for this player (return true if blocked)
--

function isCityConnected(pPlayer, pStartCity, pTargetCity, sRoute, bShortestRoute, sHighlight, fBlockaded)
  return isPlotConnected(pPlayer, pStartCity:Plot(), pTargetCity:Plot(), sRoute, bShortestRoute, sHighlight, fBlockaded)
end

function isPlotConnected(pPlayer, pStartPlot, pTargetPlot, sRoute, bShortestRoute, sHighlight, fBlockaded)
  if (bShortestRoute) then
    mg.lastRouteLength = plotToPlotShortestRoute(pPlayer, pStartPlot, pTargetPlot, sRoute, mg.highlights[sHighlight], fBlockaded)
  else
    mg.lastRouteLength = plotToPlotConnection(pPlayer, pStartPlot, pTargetPlot, sRoute, 1, mg.highlights[sHighlight], listAddPlot(pStartPlot, {}), fBlockaded)
  end

  return (mg.lastRouteLength ~= 0)
end

function getRouteLength()
  return mg.lastRouteLength
end

function getDistance(pPlot1, pPlot2)
  return distanceBetween(pPlot1, pPlot2)
end


----- PRIVATE DATA AND METHODS -----

--
-- Check if pStartPlot is connected to pTargetPlot
--
-- NOTE: This is a recursive method
--
-- Returns the length of the route between the start and target plots (inclusive) - so 0 if no route
--

function plotToPlotConnection(pPlayer, pStartPlot, pTargetPlot, sRoute, iLength, highlight, listVisitedPlots, fBlockaded)
  if (highlight ~= nil) then
    Events.SerialEventHexHighlight(PlotToHex(pStartPlot), true, highlight)
  end

  -- Have we got there yet?
  if (isSamePlot(pStartPlot, pTargetPlot)) then
    return iLength
  end

  -- Find any new plots we can visit from here
  local listRoutes = listFilter(reachablePlots(pPlayer, pStartPlot, sRoute, fBlockaded), listVisitedPlots)

  -- New routes to check, so there is an onward path
  if (listRoutes ~= nil) then
    -- Covert the associative array into a linear array so it can be sorted
    local array = {}
    for sId, pPlot in pairs(listRoutes) do
      table.insert(array, pPlot)
    end

    -- Now sort the linear array by distance from the target plot
    table.sort(array, function(x, y) return (distanceBetween(x, pTargetPlot) < distanceBetween(y, pTargetPlot)) end)

    -- Now check each onward plot in turn to see if that is connected
    for i, pPlot in ipairs(array) do
      -- Check that a prior route didn't visit this plot
      if (not listContainsPlot(pPlot, listVisitedPlots)) then
        -- Add this plot to the list of visited plots
        listAddPlot(pPlot, listVisitedPlots)

        -- If there's a route, we're done
        local iLen = plotToPlotConnection(pPlayer, pPlot, pTargetPlot, sRoute, iLength+1, highlight, listVisitedPlots, fBlockaded)
        if (iLen > 0) then
          return iLen
        end
      end
    end
  end

  if (highlight ~= nil) then
    Events.SerialEventHexHighlight(PlotToHex(pStartPlot), false)
  end

  -- No connection found
  return 0
end


--
-- Find the shortest route between two plots
--
-- We start at the TARGET plot - as the path length from here to the target plot is 1,
-- we will call this "ring 1".  We then find all reachable near plots and place them in "ring 2".
-- If the START plot is in "ring 2", we have a route, if "ring 2" is empty, there is no route,
-- otherwise find all reachable near plots that have not already been seen and place those in "ring 3"
-- We then loop, checking "ring N" otherwise generating "ring N+1"
--
-- Once we have found a route, the path length will be of length N and we know that there must be at
-- least one route by picking a plot from each ring.  The plot needed from "ring N" is the START plot,
-- we then need ANY plot from "ring N-1" that is near to the start plot. And in general we need
-- any plot from "ring M-1" that is near to the plot choosen from "ring M".  The final plot in
-- the path will always be the target plot as that is the only plot in "ring 1"
--
-- Returns the length of the route between the start and target plots (inclusive) - so 0 if no route
--

function plotToPlotShortestRoute(pPlayer, pStartPlot, pTargetPlot, sRoute, highlight, fBlockaded)
  local rings = {}

  local iRing = 1
  rings[iRing] = listAddPlot(pTargetPlot, {})

  repeat
    iRing = generateNextRing(pPlayer, sRoute, rings, iRing, fBlockaded)

    bFound = listContainsPlot(pStartPlot, rings[iRing])
    bNoRoute = (rings[iRing] == nil)
  until (bFound or bNoRoute)

  if (bFound and highlight ~= nil) then
    Events.SerialEventHexHighlight(PlotToHex(pStartPlot), true, highlight)

    local pLastPlot = pStartPlot

    for i = iRing - 1, 1, -1 do
      pNextPlot = listFirstAdjacentPlot(pLastPlot, rings[i])

      -- Check should be completely unnecessary
      if (pNextPlot == nil) then
        return 0
      end

      Events.SerialEventHexHighlight(PlotToHex(pNextPlot), true, highlight)

      pLastPlot = pNextPlot
    end
  end

  return (bFound) and iRing or 0
end

-- Helper method to find all plots near to the plots in the specified ring
function generateNextRing(pPlayer, sRoute, rings, iRing, fBlockaded)
  local nextRing = nil

  --print("generateNextRing " .. iRing)
  for k, pPlot in pairs(rings[iRing]) do
    -- Consider two near tiles A and B,  if A is in ring N, B must either be unvisited or in ring N-1
    -- for if B was in ring N-2, A would have to be in ring N-1 - which it is not
    local listRoutes = listFilter(reachablePlots(pPlayer, pPlot, sRoute, fBlockaded), ((iRing > 1) and rings[iRing-1] or {}))

    if (listRoutes ~= nil) then
      for sId, pPlot in pairs(listRoutes) do
        nextRing = nextRing or {}
        listAddPlot(pPlot, nextRing)
      end
    end
  end

  rings[iRing+1] = nextRing

  return iRing+1
end


--
-- Methods dealing with finding all near tiles that can be reached by the specified route type
--

-- Return a list of (up to 6) reachable plots from this one by route type
function reachablePlots(pPlayer, pPlot, sRoute, fBlockaded)
  local list = nil
  for pDestPlot in Plot_GetPlotsInCircle(pPlot, 1) do
    -- Don't let submarines fall over the edge!
    if (pDestPlot ~= nil) then
      if (pPlayer == nil or pDestPlot:IsRevealed(pPlayer:GetTeam())) then
        local bAdd = false
        -- Be careful of order, must check for road before rail, and coastal before ocean
        if (sRoute == mg.routes[1] and (pDestPlot:GetPlotType() == PlotTypes.PLOT_LAND or pDestPlot:GetPlotType() == PlotTypes.PLOT_HILLS)) then
          bAdd = true
        elseif (sRoute == mg.routes[2] and pDestPlot:GetRouteType() >= 0) then
          bAdd = true
        elseif (sRoute == mg.routes[3] and pDestPlot:GetRouteType() >= 1) then
          bAdd = true
        elseif (sRoute == mg.routes[4] and pDestPlot:GetTerrainType() == TerrainTypes.TERRAIN_COAST) then
          bAdd = true
        elseif (sRoute == mg.routes[5] and pDestPlot:IsWater()) then
          bAdd = true
        elseif (sRoute == mg.routes[6] and pDestPlot:IsWater()) then
          bAdd = true
        end
        -- Special case for water, a city on the coast counts as water
        if (not bAdd and (sRoute == mg.routes[4] or sRoute == mg.routes[5] or sRoute == mg.routes[6])) then
          bAdd = pDestPlot:IsCity()
        end
        -- Check for impassable and blockaded tiles
        bAdd = bAdd and isPassable(pDestPlot, sRoute) and not isBlockaded(pDestPlot, pPlayer, fBlockaded)
        if (bAdd) then
          list = list or {}
          listAddPlot(pDestPlot, list)
        end
      end
    end
  end
  return list
end

-- Is the plot passable for this route type ..
function isPassable(pPlot, sRoute)
  bPassable = true

  -- .. due to terrain, eg natural wonders and those covered in ice
  iFeature = pPlot:GetFeatureType()
  if (iFeature > 0 and GameInfo.Features[iFeature].NaturalWonder == true) then
    bPassable = false
  elseif (iFeature == FeatureTypes.FEATURE_ICE and sRoute ~= mg.routes[6]) then
    bPassable = false
  end

  return bPassable
end

-- Is the plot blockaded for this player ..
function isBlockaded(pPlot, pPlayer, fBlockaded)
  bBlockaded = false

  if (fBlockaded ~= nil) then
    bBlockaded = fBlockaded(pPlot, pPlayer)
  end

  return bBlockaded
end



--
-- Calculate the distance between two plots
--
-- See http://www-cs-students.stanford.edu/~amitp/Articles/HexLOS.html
-- Also http://keekerdc.com/2011/03/hexagon-grids-coordinate-systems-and-distance-calculations/
--
function distanceBetween(pPlot1, pPlot2)
  local mapX, mapY = Map.GetGridSize()

  -- Need to work on a hex based grid
  local hex1 = PlotToHex(pPlot1)
  local hex2 = PlotToHex(pPlot2)

  -- Calculate the distance between the x and z co-ordinate pairs
  -- allowing for the East-West wrap, (ie shortest route may be by going backwards!)
  local deltaX = math.min(math.abs(hex2.x - hex1.x), mapX - math.abs(hex2.x - hex1.x))
  local deltaZ = math.min(math.abs(hex2.z - hex1.z), mapX - math.abs(hex2.z - hex1.z))

  -- Calculate the distance between the y co-ordinates
  -- there is no North-South wrap, so this is easy
  local deltaY = math.abs(hex2.y - hex1.y)

  -- Calculate the distance between the plots
  local distance = math.max(deltaX, deltaY, deltaZ)

  -- Allow for both end points in the distance calculation
  return distance + 1
end

-- Get the hex co-ordinates of a plot
function PlotToHex(pPlot)
  local hex = ToHexFromGrid(Vector2(pPlot:GetX(), pPlot:GetY()))

  -- X + y + z = 0, hence z = -(x+y)
  hex.z = -(hex.x + hex.y)

  return hex
end


--
-- List (associative arrays) helper methods
--

-- Return a list formed by removing all entries from list1 which are in list2
function listFilter(list1, list2)
  local list = nil

  if (list1 ~= nil) then
    for sKey, pPlot in pairs(list1) do
      if (list2 == nil or list2[sKey] == nil) then
        list = list or {}
        list[sKey] = pPlot
      end
    end
  end

  return list
end

-- Return true if pPlot is in list
function listContainsPlot(pPlot, list)
  return (list ~= nil and list[getPlotKey(pPlot)] ~= nil)
end

-- Add the plot to the list
function listAddPlot(pPlot, list)
  if (list ~= nil) then
    list[getPlotKey(pPlot)] = pPlot
  end

  return list
end

function listFirstAdjacentPlot(pPlot, list)
  for key, plot in pairs(list) do
    if (distanceBetween(pPlot, plot) == 2) then
      return plot
    end
  end

  -- We should NEVER reach here
  return nil
end


--
-- Plot helper methods
--

-- Are the plots one and the same?
function isSamePlot(pPlot1, pPlot2)
	return (pPlot1:GetX() == pPlot2:GetX() and pPlot1:GetY() == pPlot2:GetY())
end

-- Get a unique key for the plot
function getPlotKey(pPlot)
	return string.format("%d:%d", pPlot:GetX(), pPlot:GetY())
end

-- Get the grid-based (x, y) co-ordinates of the plot as a string
function plotToGridStr(pPlot)
	if (pPlot == nil) then
		return ""
	end

	return string.format("(%d, %d)", pPlot:GetX(), pPlot:GetY())
end

-- Get the hex-based (x, y, z) co-ordinates of the plot as a string
function plotToHexStr(pPlot)
	if (pPlot == nil) then
		return ""
	end

	local hex = PlotToHex(pPlot)

	return string.format("(%d, %d, %d)", hex.x, hex.y, hex.z)
end

-- Get random multiplier normalized to 1
-- rand: optional random value
-- higher: optional boolean, determines >1 or <1
function getRandomMultiplier(variance, rand, higher)
	if higher == nil then
		higher = (1 == Map.Rand(2, "getRandomMultiplier"))
	end

	local multiplier = 1
	if rand == nil then
		multiplier = 1 + PWRand() * variance
	else
		multiplier = 1 + rand * variance
	end

	if higher then
		return multiplier
	else
		return 1 / multiplier
	end
end

-- Get modified odds when you roll the dice multiple times
function modifyOdds(odds, multiplier)
	if (odds >= 1) then
		return 1
	end
	if (multiplier <= 0) then
		return 0
	end
	return 1 - math.pow(1 - odds, multiplier)
end

-- Get modified odds with random multiplier normalized to 1
-- rand: optional random value
-- higher: optional boolean, determines >1 or <1
function modifyOddsRandom(odds, variance, rand, higher)
	return modifyOdds(odds, getRandomMultiplier(variance, rand, higher))
end

-- Check if Even More Resources for Vox Populi is activated
function IsEvenMoreResourcesActive()
	local evenMoreResourcesModID = "8e54eb87-31e8-4fcd-aafe-ede055b463d0"
	local isUsingEvenMoreResources = false

	for _, mod in pairs(Modding.GetActivatedMods()) do
		if (mod.ID == evenMoreResourcesModID) then
			isUsingEvenMoreResources = true
			break
		end
	end

	return isUsingEvenMoreResources
end
