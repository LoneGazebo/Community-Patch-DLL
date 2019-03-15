--[[------------------------------------------------------------------------------

-- Communitas.lua map script --

  Created by:
- Cephalo (Rich Marinaccio)		- Perlin landform, elevation and rainfall creation
- Sirian (Bob Thomas)			- Island creation, some code from Continents and Terra scripts
- WHoward69						- Mountain-pass finding algorithm
- Bobert13						- Bug fixes and optimizations
- Thalassicus (Victor Isbell)	- Ocean rifts, rivers through lakes, natural wonder placement,
								  resource placement, map options, inland seas, aesthetic polishing


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
local debugPrint = false
local debugWithLogger = false

--[[
Setting "overrideAssignStartingPlots = false" may help make the map compatible 
with core game patches in the distant future when I'm no longer modding Civ 5.

This disables some advanced features of the map, so it's better to
modify the map's changes to AssignStartingPlots if possible.

~ Thalassicus @ Nov 5 2013
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
	mglobal.landPercent			= 0.35
	
	--Top and bottom map latitudes.
	mglobal.topLatitude				= 90
	mglobal.bottomLatitude			= -90
	
	
	--Important latitude markers used for generating climate.
	mglobal.tropicLatitudes			= 22	--    			   tropicLatitudes to 0 : grass, jungle
	mglobal.horseLatitudes			= 23	-- polarFrontLatitude to horseLatitudes : plains, desert
	mglobal.iceLatitude				= 67	-- 		  bottomLatitude to iceLatitude : ice
	mglobal.polarFrontLatitude			= 35	-- bottomLatitude to polarFrontLatitude : snow, tundra

	
	--Adjusting these will generate larger or smaller landmasses and features.
	mglobal.landMinScatter			= 0.02 	--Recommended range:[0.02 to 0.1]
	mglobal.landMaxScatter			= 0.06	--Recommended range:[0.03 to 0.3]
											--Higher values makes continental divisions and stringy features more likely,
											--and very high values result in a lot of stringy continents and islands.
											
	mglobal.coastScatter			= 0.09 	--Recommended range:[0.01 to 0.3]
											--Higher values result in more islands and variance on landmasses and coastlines.
											
	mglobal.mountainScatter			= 250 * mapW --Recommended range:[130 to 1000]
											--Lower values make large, long, mountain ranges. Higher values make sporadic mountainous features.
	
	
	-- Terrain
	mglobal.mountainWeight		= 0.7  --Weight of the mountain elevation map versus the coastline elevation map.
	mglobal.belowMountainPercent	= 0.94 -- Percent of non-mountain land
										   -- flatPercent to belowMountainPercent : hills
	mglobal.flatPercent			= 0.78 -- Percent of flat land
	mglobal.hillsBlendPercent		= 0.45 -- Chance for flat land to become hills per near mountain. Requires at least 2 near mountains.
	mglobal.terrainBlendRange		= 3	   -- range to smooth terrain (desert surrounded by plains turns to plains, etc)
	mglobal.terrainBlendRandom		= 0.6  -- random modifier for terrain smoothing

	
	-- Features
	mglobal.featurePercent			= 0.60 -- Percent of potential feature tiles that actually create a feature (marsh/jungle/forest)
	mglobal.featureWetVariance		= 0.15 -- Percent chance increase if freshwater, decrease if dry (groups features near rivers)
	mglobal.islePercent				= 0.06 -- Percent of coast tiles with an isle
	mglobal.numNaturalWonders		= 2 + GameInfo.Worlds[Map.GetWorldSize()].NumNaturalWonders
	
	
	-- Rain
	mglobal.marshPercent		= 0.10 -- Percent chance increase for marsh from each nearby watery tile 
										   --    junglePercent to 1 : marsh
	mglobal.junglePercent		= 0.99 --    junglePercent to 1 : jungle
	mglobal.zeroTreesPercent		= 0.35 -- zeroTreesPercent to 1 : forest
										   -- 	 plainsPercent to 1 : grass
	mglobal.plainsPercent		= 0.50 -- 	 desertPercent to plainsPercent : plains
	mglobal.desertPercent		= 0.67 --				 0 to desertPercent : desert
	
	
	-- Temperature
	mglobal.jungleMinTemperature	= 0.74 -- jungle:  jungleMinTemperature	to 1
	mglobal.desertMinTemperature	= 0.61 -- desert:  desertMinTemperature	to 1
										   -- grass:      tundraTemperature to 1
										   -- plains:     tundraTemperature to 1
	mglobal.tundraTemperature		= 0.29 -- tundra: 		snowTemperature to tundraTemperature
	mglobal.snowTemperature		= 0.17 -- snow:   					  0 to snowTemperature
	mglobal.treesMinTemperature		= 0.33 -- trees:	treesMinTemperature to 1
	mglobal.forestRandomPercent		= 0.07 -- Percent of barren flatland which randomly gets a forest
	mglobal.forestTundraPercent		= 0.30 -- Percent of barren tundra   which randomly gets a forest
	
	

	-- Water
	mglobal.riverPercent			= 0.25	-- Percent of river junctions that are large enough to become rivers.	
	mglobal.riverRainCheatFactor	= 1.25	-- Values greater than one favor watershed size. Values less than one favor actual rain amount.
	mglobal.minWaterTemp			= 0.26	-- Sets water temperature compression that creates the land/sea seasonal temperature differences that cause monsoon winds.
	mglobal.maxWaterTemp			= 0.39	
	mglobal.geostrophicFactor		= 3.0	-- Strength of latitude climate versus monsoon climate. 
	mglobal.geostrophicLateralWindStrength = 0.6 
	mglobal.lakeSize				= 10	-- read-only; cannot change lake sizes with a map script
	mglobal.oceanMaxWander			= 1		-- number of tiles a rift can randomly wander from its intended path
	mglobal.oceanElevationWeight	= 0.31	-- higher numbers make oceans avoid continents
	mglobal.oceanRiftWidth			= math.max(1, Round(mapW/40)) -- minimum number of ocean tiles in a rift
	
											-- percent of map width:
	mglobal.atlanticSize			= 0.05	-- size near poles
	mglobal.atlanticBulge			= 0		-- size increase at equator
	mglobal.atlanticCurve			= 0.04	-- S-curve distance
	mglobal.pacificSize				= 0.05		-- size near poles
	mglobal.pacificBulge			= 0.20	-- size increase at equator
	mglobal.pacificCurve			= 0		-- S-curve distance
	
	
	mglobal.atlanticSize			= math.min(4, Round(mglobal.atlanticSize * mapW))
	mglobal.pacificBulge			= Round(mglobal.pacificBulge * mapW)
	mglobal.atlanticCurve			= math.min(5, Round(mglobal.atlanticCurve * mapW))
	mglobal.pacificCurve			= math.min(3, Round(mglobal.pacificCurve * mapW))
	
	
	-- Resources
	mglobal.fishTargetFertility		= 60	-- fish appear to create this average city fertility
	
	
	-- Quality vs Performance
	-- Lowering these reduces map quality and creation time.
	-- Try reducing these slightly if you experience crashes on huge maps
	mglobal.tempBlendMaxRange		= 10 -- range to smooth temperature map
	mglobal.elevationBlendRange		= 9 -- range to smooth elevation map
	
	
	
	
	
	

	
	--[[
	
		MAP OPTIONS
			
	1 - world_age
	2 - temperature
	3 - rainfall
	4 - sea_level
	5 - resources
	6 - Players Start
	7 - Ocean Rifts
	8 - Ocean Rift width
	
	--]]
	
	do
	
	local oWorldAge = Map.GetCustomOption(1)
	if oWorldAge == 4 then oWorldAge = 1 + Map.Rand(3, "Communitas random world age - Lua") end
	if oWorldAge == 1 then
		print("Map Age:  New")
		mglobal.belowMountainPercent	= 1 - (1 - mglobal.belowMountainPercent) * 1.5
		mglobal.flatPercent			= 1 - (1 - mglobal.flatPercent) * 1.5
		mglobal.landMinScatter		= mglobal.landMinScatter	/ 1.5
		mglobal.landMaxScatter		= mglobal.landMaxScatter	/ 1.5
		mglobal.coastScatter		= mglobal.coastScatter		/ 1.5
		mglobal.mountainScatter		= mglobal.mountainScatter
	elseif oWorldAge == 3 then
		print("Map Age:  Old")
		mglobal.belowMountainPercent	= 1 - (1 - mglobal.belowMountainPercent) / 1.5
		mglobal.flatPercent			= 1 - (1 - mglobal.flatPercent) / 1.5
		mglobal.landMinScatter		= mglobal.landMinScatter	* 1.5
		mglobal.landMaxScatter		= mglobal.landMaxScatter	* 1.5
		mglobal.coastScatter		= mglobal.coastScatter		* 1.5
		mglobal.mountainScatter		= mglobal.mountainScatter
	else
		print("Map Age:  Normal")
	end
	mglobal.mountainScatter = mglobal.mountainScatter * 0.00001
	
	
	local oTemp = Map.GetCustomOption(2)
	if oTemp == 4 then oTemp = 1 + Map.Rand(3, "Communitas random temperature - Lua") end
	if oTemp == 1 then
		print("Map Temp: Cool")
		mglobal.tropicLatitudes			= mglobal.tropicLatitudes		/ 1.5
		mglobal.horseLatitudes			= mglobal.horseLatitudes		/ 1.5
		mglobal.iceLatitude				= mglobal.iceLatitude			/ 3
		mglobal.polarFrontLatitude		= mglobal.polarFrontLatitude	/ 1.5
		mglobal.tundraTemperature		= mglobal.tundraTemperature		* 1.25
		--mglobal.snowTemperature		= mglobal.snowTemperature		* 1.25 -- snow is just horrible
	elseif oTemp == 3 then
		print("Map Temp: Hot")
		mglobal.tropicLatitudes			= mglobal.tropicLatitudes		* 1.5
		mglobal.horseLatitudes			= mglobal.horseLatitudes		* 1.5
		mglobal.iceLatitude				= 60
		mglobal.polarFrontLatitude		= 65
		mglobal.tundraTemperature		= mglobal.tundraTemperature		/ 1.25
		mglobal.snowTemperature			= mglobal.snowTemperature		/ 1.25
	else
		print("Map Temp: Normal")
	end
	
	
	local oRainfall = Map.GetCustomOption(3)
	if oRainfall == 4 then oRainfall = 1 + Map.Rand(3, "Communitas random rain - Lua") end
	if oRainfall == 1 then
		print("Map Rain: Arid")
		mglobal.riverPercent		= mglobal.riverPercent		/ 1.5
		mglobal.featurePercent		= mglobal.featurePercent	/ 1.5
		mglobal.marshPercent		= mglobal.marshPercent		/ 1.5
		mglobal.junglePercent		= 1 - (1 - mglobal.junglePercent) / 1.5
		mglobal.zeroTreesPercent	= mglobal.zeroTreesPercent 	* 1.5
		mglobal.plainsPercent		= mglobal.plainsPercent		* 1.25
		mglobal.desertPercent		= mglobal.desertPercent		* 1.25
	elseif oRainfall == 3 then
		print("Map Rain: Wet")
		mglobal.featurePercent		= 0.9 -- should not go above 90%
		mglobal.riverPercent		= mglobal.riverPercent		* 1.5
		mglobal.marshPercent		= mglobal.marshPercent		* 1.5
		mglobal.junglePercent		= 1 - (1 - mglobal.junglePercent) * 1.5
		mglobal.zeroTreesPercent	= mglobal.zeroTreesPercent 	/ 1.5
		mglobal.plainsPercent		= mglobal.plainsPercent		/ 1.5
		mglobal.desertPercent		= mglobal.desertPercent		/ 1.5
	else
		print("Map Rain: Normal")
	end
	
	
	local oSeaLevel = Map.GetCustomOption(4)
	if oSeaLevel == 4 then oSeaLevel = 1 + Map.Rand(3, "Communitas random sea level - Lua") end
	if oSeaLevel == 1 then
		print("Map Seas: Low")
		mglobal.landPercent			= mglobal.landPercent * 1.25
	elseif oSeaLevel == 3 then
		print("Map Seas: High")
		mglobal.landPercent			= mglobal.landPercent / 1.25
	else
		print("Map Seas: Normal")
	end
	
	
	local oStarts = Map.GetCustomOption(6)
	if oStarts == 1 then
		print("Map Starts: Everywhere")
		mglobal.offsetAtlanticPercent	= 0.55	-- Percent of land to divide at the Atlantic Ocean (50% is usually halfway on the map)
		--mglobal.offshoreCS			= 0.50	-- no longer needed
	else
		print("Map Starts: Largest Continent")
		mglobal.offsetAtlanticPercent	= 0.35	-- Percent of land to divide at the Atlantic Ocean
		mglobal.percentLargestContinent	= 0.37	-- Eurasia must be this percent of total land (ensures citystates can appear there)
		mglobal.terraConnectWeight		= 10	-- if Eurasia is too small, connect sub-continents with this (size/distance) from Eurasia
		--mglobal.offshoreCS			= 0.75	-- no longer needed
		mglobal.numNaturalWonders		= Round (1.25 * mglobal.numNaturalWonders) -- extra wonders for larger map sizes
	end
	
	
	local oRiftWidth = Map.GetCustomOption(8)
	--mglobal.oceanRiftWidth = mglobal.oceanRiftWidth * mapW	
	if oRiftWidth == 1 then
		print("Map Ocean Width: Narrow")
		mglobal.oceanRiftWidth = 1
		mglobal.landPercent = mglobal.landPercent - 0.02
	elseif oRiftWidth == 3 then
		print("Map Ocean Width: Wide")
		mglobal.oceanRiftWidth = math.max(4, Round(mapW/20)) 
		mglobal.landPercent = mglobal.landPercent + 0.05
	end	
	
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
	mglobal.northAttenuationFactor	= 0.0
	mglobal.northAttenuationRange	= 0.0 --percent of the map height.
	mglobal.southAttenuationFactor	= 0.0
	mglobal.southAttenuationRange	= 0.0

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
	
	return mglobal
end

function GetMapScriptInfo()
	local world_age, temperature, rainfall, sea_level, resources = GetCoreMapOptions()
	return {
		Name = "Communitas",
		Description = "Creates continents and islands, with climate based on elevation and wind. Includes custom game setup options.",
		IsAdvancedMap = 0,
		SupportsMultiplayer = true,
		IconIndex = 5,
		SortIndex = -999,
		CustomOptions = {
			world_age,
			temperature,
			rainfall, 
			sea_level,
			resources,
			{
                Name = "Players Start",
                Values = {
                    "Continents - Everywhere",
                    "Terra - Largest Continent"
                },
                DefaultValue = 1,
                SortPriority = 1,
            },
			{
                Name = "Ocean Rifts",
                Values = {
                    "Pacific and Atlantic",
                    "2 Atlantic",
                    "2 Pacific",
                    "2 Random",
                    "1 Random",
                    "None"
                },
                DefaultValue = 1,
                SortPriority = 2,
            },
			{
                Name = "Rift Width",
                Values = {
                    "Narrow",
                    "Normal",
                    "Wide"
                },
                DefaultValue = 2,
                SortPriority = 3,
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
		
	if Map.GetCustomOption(6) == 2 then
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
	local res = Map.GetCustomOption(5) or 2
	if res == 6 then
		res = 1 + Map.Rand(3, "Random Resources Option - Lua")
	end

	local oStarts = Map.GetCustomOption(6)
	local divMethod = nil
	if oStarts == 1 then
		-- Continents
		divMethod = 2
	else
		--Terra
		divMethod = 1
	end

	print("Creating start plot database.")
	local start_plot_database = AssignStartingPlots.Create()

	print("Dividing the map in to Regions.")
	-- Regional Division Method 2: Continental or 1:Terra
	local args = {
		method = divMethod,
		resources = res,
		}
	start_plot_database:GenerateRegions(args)

	print("Choosing start locations for civilizations.")
	start_plot_database:ChooseLocations()

	print("Normalizing start locations and assigning them to Players.")
	start_plot_database:BalanceAndAssign()

	--error(":P")
	print("Placing Natural Wonders.")
	start_plot_database:PlaceNaturalWonders()

	print("Placing Resources and City States.")
	start_plot_database:PlaceResourcesAndCityStates()
end

function GeneratePlotTypes()
	print("Creating initial map data - CommunitasMap")
	--[[
	local plot = Map.GetPlot(5, 5)
	for nearPlot, distance in Plot_GetPlotsInCircle(plot, 0, 1) do
		print(string.format(
			"plot %s distance=%s",
			Plot_GetID(nearPlot),
			distance
		))
	end
	--]]
	
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
	CreateArcticOceans()
	CreateVerticalOceans()
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
	
	local plotFunc = function(plot)
		return not Plot_IsWater(plot, true)
	end
	
	log:Info("ConnectSeasToOceans: oceanSize = %s", oceanSize)
	local newWater = {}
	for areaID=1, #areaMap.areaList do
		local seaArea = areaMap.areaList[areaID]
		if seaArea.trueMatch and seaArea.size < oceanSize then
			local pathPlots, distance, airDistance = GetPathBetweenAreas(areaMap, seaArea, oceanArea, true, plotFunc)
			if seaArea.size >= mg.lakeSize then--or seaArea.size >= 2 * distance then
				log:Info("ConnectSeasToOceans: Connect seaArea.size = %-3s distance = %-3s airDistance = %-3s", seaArea.size, distance, airDistance)
				--log:Info("                     Connect")
				for _, plot in pairs(pathPlots) do
					local plotID = Plot_GetID(plot)
					newWater[Plot_GetID(plot)] = elevationMap.seaLevelThreshold - 0.01
					if seaArea.size <= 2 * mg.lakeSize then
						table.insert(mg.seaPlots, plot)
					end
					--plot:SetFeatureType(FeatureTypes.FEATURE_ICE, -1)
				end
			end
		end
	end
	for plotID, elevation in pairs(newWater) do
		elevationMap.data[plotID] = elevation
	end
end

function ConnectTerraContinents()
	if Map.GetCustomOption(6) == 1 then
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
			log:Info("ConnectTerraContinents: Connect continents[%s].size = %-3s distance = %-3s airDistance = %-3s size/distance = %s",
				index,
				area.size,
				area.distance,
				area.airDistance,
				Round(area.size / math.max(1, area.distance))
			)
			for _, plot in pairs(area.pathPlots) do
				newLand[Plot_GetID(plot)] = elevationMap.seaLevelThreshold
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
			plots[plotID].elevation	= 0
		else
			if findLowest then
				-- connect oceans
				plots[plotID].elevation	= GetElevationByPlotID(plotID) ^ 2
			else
				-- connect continents
				plots[plotID].elevation	= 1000 - GetElevationByPlotID(plotID) ^ 2
			end
		end
		
		if plots[plotID].areaID == areaA.id then
			plots[plotID].sumElevation = 0
		else
			plots[plotID].sumElevation = 30000
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
	for _, plot in pairs(mg.lakePlots) do
		local isIce = (plot:GetFeatureType() == FeatureTypes.FEATURE_ICE)
		plot:SetTerrainType(TerrainTypes.TERRAIN_COAST, false, true)
		if isIce then
			--plot:SetFeatureType(FeatureTypes.FEATURE_ICE, -1)
		end
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
				if rainfallMap.data[i] < desertThreshold then
					if temperatureMap.data[i] < mg.snowTemperature then--and elevationMap:GetZone(y) ~= mg.NEQUATOR and elevationMap:GetZone(y) ~= mg.SEQUATOR then
						plot:SetTerrainType(TerrainTypes.TERRAIN_SNOW,false,false)
					elseif temperatureMap.data[i] < mg.tundraTemperature then
						plot:SetTerrainType(terrainTundra,false,false)
					elseif temperatureMap.data[i] < mg.desertMinTemperature then
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS,false,false)
					else
						--if rainfallMap.data[i] < (PWRand() * (desertThreshold - minRain) + desertThreshold - minRain)/2.0 + minRain then
						plot:SetTerrainType(TerrainTypes.TERRAIN_DESERT,false,false)
						--else
							--plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS,false,false)
						--end
					end
				elseif rainfallMap.data[i] < plainsThreshold then
					if temperatureMap.data[i] < mg.snowTemperature then--and elevationMap:GetZone(y) ~= mg.NEQUATOR and elevationMap:GetZone(y) ~= mg.SEQUATOR then
						plot:SetTerrainType(TerrainTypes.TERRAIN_SNOW,false,false)
					elseif temperatureMap.data[i] < mg.tundraTemperature then
						plot:SetTerrainType(terrainTundra,false,false)
					else
						if rainfallMap.data[i] < (PWRand() * (plainsThreshold - desertThreshold) + plainsThreshold - desertThreshold)/2.0 + desertThreshold then
							plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS,false,false)
						else
							plot:SetTerrainType(terrainGrass,false,false)
						end
					end
				else
					if temperatureMap.data[i] < mg.snowTemperature then--and elevationMap:GetZone(y) ~= mg.NEQUATOR and elevationMap:GetZone(y) ~= mg.SEQUATOR then
						plot:SetTerrainType(TerrainTypes.TERRAIN_SNOW,false,false)
					elseif temperatureMap.data[i] < mg.tundraTemperature then
						plot:SetTerrainType(terrainTundra,false,false)
					else
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
	local iCellWidth = 10
	local iCellHeight = 8
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
					if plotPercent.TERRAIN_DESERT >= 0.5 * randPercent then
						--plot:SetTerrainType(TerrainTypes.TERRAIN_DESERT, true, true)
					end
				elseif plotTerrainID == TerrainTypes.TERRAIN_DESERT then
					if plotPercent.TERRAIN_GRASS + plotPercent.TERRAIN_SNOW >= 0.25 * randPercent then
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, true, true)
					elseif plotPercent.FEATURE_JUNGLE + plotPercent.FEATURE_MARSH >= 0.25 * randPercent then
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, true, true)
					end
				elseif plotFeatureID == FeatureTypes.FEATURE_JUNGLE or plotFeatureID == FeatureTypes.FEATURE_MARSH then
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
	for plotID, plot in Plots(Shuffle) do
		if plot:GetPlotType() == PlotTypes.PLOT_LAND then
			local nearMountains = 0
			local nearWarm = 0
			for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
				if not nearPlot:IsWater() then
					local nearTerrainID = nearPlot:GetTerrainType()
					
					if IsMountain(nearPlot) then
						nearMountains = nearMountains + 1
					end
					
					if GetTemperature(nearPlot) > GetTemperature(plot) then
						nearWarm = nearWarm + 1
					end
				end
			end
			if (nearMountains > 1 and (nearMountains * mg.hillsBlendPercent * 100) >= Map.Rand(100, "Blend mountains - Lua")) then
				--print("Turning flatland near mountain into hills")
				plot:SetPlotType(PlotTypes.PLOT_HILLS, false, false)
				--plot:SetTerrainType(TerrainTypes.TERRAIN_SNOW, false, false)
			elseif nearWarm * 0.5 * mg.hillsBlendPercent * 100 >= Map.Rand(100, "Blend hills - Lua") then
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
	local oOceanRifts = Map.GetCustomOption(7)
	local mapW, mapH = Map.GetGridSize()	
	if oOceanRifts == 6 then
		-- No vertical rifts
		return
	end
	
	log:Info("CreateVerticalOceans mg.oceanRiftWidth = %s", mg.oceanRiftWidth)
	
	
	function CreatePacific(midline)
		CreateOceanRift{x = midline, totalSize = mg.pacificSize, bulge = mg.pacificBulge, curve = mg.pacificCurve, oceanSize = math.max(1, Round(mg.oceanRiftWidth - 1))}
	end
	function CreateAtlantic(midline)
		CreateOceanRift{x = midline, totalSize = mg.atlanticSize, bulge = mg.atlanticBulge, curve = mg.atlanticCurve, oceanSize = mg.oceanRiftWidth, cleanMid = true}
	end
	
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
	
	table.insert(mg.oceanRiftMidlines, startX)
	if mapW < 60 then
		log:Debug("CreateVerticalOceans: Creating Atlantic at x=%s", startX)
		CreateAtlantic(startX)
		return
	elseif oOceanRifts == 1 or oOceanRifts == 3 then
		-- PA or PP
		log:Debug("CreateVerticalOceans: Creating Pacific  at x=%s", startX)
		CreatePacific(startX)
	elseif oOceanRifts == 2 then
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
					isWater = true,
					terrainID = TerrainTypes.TERRAIN_OCEAN
				}
			elseif 50 >= Map.Rand(100, "Ocean rift ocean/coast - Lua") then
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
	
	function GetMidX(y)
		if curve == 0 then
			return midline
		end
		log:Trace("%s * GetSinCurve(%s, %s) = %s", curve, y, mapH, Round(curve * GetSinCurve(y, mapH, 3)))
		return (midline + Round(curve * GetSinCurve(y, mapH, 3))) % mapW
	end
	
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
		local extraRadius	= oceanSize + 2
		
		if direction == mg.E or direction == mg.W then
			log:Trace("x=%-3s, y=%-3s radius=%-3s extraRadius=%-3s", x, y, radius, extraRadius)
		else
			log:Trace("x=%-3s, y=%-3s radius=%-3s extraRadius=%-3s midX=%-3s", x, y, radius, extraRadius, GetMidX(y))
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
				--[[
				log:Trace("%s, %s, %s, %s, %s",
					nearPlot:GetX(),
					nearPlot:GetY(),
					nearDistance,
					riftPlots[nearPlotID].minDistance,
					riftPlots[nearPlotID].strip
				)
				--]]
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
			plot = nextPlotA
		else
			local distanceA = Map.PlotDistance(nextPlotA:GetX(), y, GetMidX(y), y)
			local distanceB = Map.PlotDistance(nextPlotB:GetX(), y, GetMidX(y), y)
			if distanceA < distanceB then
				oddsA = oddsA + (0.50 * distanceA / mg.oceanMaxWander)
			elseif distanceA > distanceB then
				oddsA = oddsA - (0.50 * distanceA / mg.oceanMaxWander)
			end
			
			--[[
			local nextElevationA = mg.elevationRect[nextPlotA:GetX()][nextPlotA:GetY()]
			local nextElevationB = mg.elevationRect[nextPlotB:GetX()][nextPlotB:GetY()]
			
			if nextElevationA < nextElevationB then
				oddsA = oddsA + mg.oceanElevationWeight
			elseif nextElevationA > nextElevationB then
				oddsA = oddsA - mg.oceanElevationWeight
			end
			--]]
			
			local randomPercent = 0.5--PWRand()
			log:Trace("distance A=%s B=%s oddsA=%.2f rand=%.2f", distanceA, distanceB, Round(oddsA, 2), Round(randomPercent, 2))
			if oddsA >= randomPercent then
				plot = nextPlotA
				log:Trace("choose A")
			else
				plot = nextPlotB
				log:Trace("choose B")
			end
		end
			
		x = plot:GetX()
		y = plot:GetY()
		attempts = attempts + 1
	end
	return riftPlots
end

function SetOceanRiftElevations(elevationMap)
	--
	for plotID, data in pairs(mg.oceanRiftPlots) do
		if data.isWater then
			local plot = Map.GetPlotByIndex(plotID)
			elevationMap.data[elevationMap:GetIndex(plot:GetX(), plot:GetY())] = 0
		end
	end
	--]]
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
	--
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
	--]]
end










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
	for plotID, plot in Plots(Shuffle) do
		if not plot:IsWater() then
			PlacePossibleOasis(plot)
			if IsGoodExtraForestTile(plot) then
				table.insert(potentialForestPlots, plot)
			end
		else
			PlacePossibleIce(plot)
		end
	end
	
	for _, plot in pairs(potentialForestPlots) do
		plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
	end
	
	AddIsles()
	--ConnectTerraContinents()
	
	if debugTime then print(string.format("%5s ms, AddFeatures %s", math.floor((os.clock() - timeStart) * 1000), "End")) end
end

function Plot_AddMainFeatures(plot, zeroTreesThreshold, jungleThreshold)
	local x, y					= plot:GetX(), plot:GetY()
	local i						= elevationMap:GetIndex(x,y)
	local mapW, mapH			= Map.GetGridSize()
	local plotTerrainID 		= plot:GetTerrainType()
	--local zeroTreesThreshold	= rainfallMap:FindThresholdFromPercent(mg.zeroTreesPercent,false,true)
	--local jungleThreshold		= rainfallMap:FindThresholdFromPercent(mg.junglePercent,false,true)
	
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
				plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS,false,false)
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
		local treeRange = jungleThreshold - zeroTreesThreshold
		if (rainfallMap.data[i] > PWRand() * treeRange + zeroTreesThreshold) and (temperatureMap.data[i] > mg.treesMinTemperature) then
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
	
	--[[
	if plot:IsHills() then
		--jungle on hill looks terrible in Civ5. Can't use it.
		plot:SetTerrainType(terrainGrass,false,false)
		if IsGoodFeaturePlot(plot) then
			plot:SetFeatureType(FeatureTypes.FEATURE_FOREST,-1)
		end
		return
	end
	--]]
	
	if IsGoodFeaturePlot(plot) then
		plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE,-1)
		if plot:IsHills() then
			plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS,false,false)
		else
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
			elseif nearTerrainID == TerrainTypes.TERRAIN_DESERT or nearTerrainID == TerrainTypes.TERRAIN_SNOW or nearTerrainID == TerrainTypes.TERRAIN_TUNDRA then
				return 0
			elseif nearPlot:GetFeatureType() == FeatureTypes.FEATURE_MARSH then
				odds = odds - 8 * mg.marshPercent -- avoid clumping
			elseif plot:IsRiverSide() and nearPlot:IsFreshWater() then
				odds = odds + mg.marshPercent
			end
		end
		return math.min(odds, mg.featurePercent) >= PWRand()
	end
	
	odds = mg.featurePercent 
	if plot:IsFreshWater() then
		odds = odds + mg.featureWetVariance
	else
		odds = odds - mg.featureWetVariance
	end
	
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
	local resID		= plot:GetResourceType()
	if not plot:CanHaveFeature(FeatureTypes.FEATURE_FOREST) then
		return false
	end
	
	if terrainID == TerrainTypes.TERRAIN_TUNDRA then
		if resID ~= -1 then
			return true
		end
		if plot:IsFreshWater() then
			odds = odds + mg.featureWetVariance
		end
	end
	
	-- Avoid filling flat holes of tropical areas, which are too dense already
	if not plot:IsHills() and Contains(mg.tropicalPlots, plot) then
		odds = odds - 0.30
	end
	
	for nearPlot in Plot_GetPlotsInCircle(plot, 1, 1) do
		local nearTerrainID = nearPlot:GetTerrainType()
		local nearFeatureID = nearPlot:GetFeatureType()
		
		if nearPlot:IsMountain() then
			-- do nothing
		elseif nearPlot:IsHills() then
			-- Region already has enough production and rough terrain
			odds = odds - 0.10
		elseif nearTerrainID == TerrainTypes.TERRAIN_SNOW then
			-- Help extreme polar regions
			odds = odds + 0.2
		elseif nearTerrainID == TerrainTypes.TERRAIN_TUNDRA then
			odds = odds + 0.1
		elseif terrainID == TerrainTypes.TERRAIN_TUNDRA and Plot_IsWater(nearPlot) then
			odds = odds + 0.1
		end	
		
		-- Avoid tropics
		if Contains(mg.tropicalPlots, nearPlot) then
			odds = odds - 0.10
		end
		
		-- Too dry
		if nearTerrainID == TerrainTypes.TERRAIN_DESERT then
			odds = odds - 0.20
		end
	end
	
	if 100 * mg.featurePercent * math.min(1, odds) >= Map.Rand(100, "Add Extra Forest - Lua") then
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
	
	if 0 < y and y < mapH - 1 then
		odds = 100 * (latitude - lowestIce) / (mg.topLatitude - lowestIce)
	end
	
	--if not Cep then
		local nearLand = false
		for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
			if not nearPlot:IsWater() then
				--odds * 0.5
				return
			end
		end
	--end
	
	if odds >= Map.Rand(100, "PlacePossibleIce - Lua") then
		plot:SetFeatureType(FeatureTypes.FEATURE_ICE,-1)
		--[[
		if y < 5 then
			log:Debug("Ice at y=%s odds=%s latitude=%s lowestIce=%s topLatitude=%s",
				y,
				odds,
				latitude,
				lowestIce,
				mg.topLatitude
			)
		end
		--]]
		--[[
		for nearPlot in Plot_GetPlotsInCircle(plot, 1) do
			if nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_GRASS then
				nearPlot:SetTerrainType(TerrainTypes.TERRAIN_TUNDRA, false, true)
				if nearPlot:GetFeatureType() == FeatureTypes.FEATURE_MARSH then
					nearPlot:SetFeatureType(FeatureTypes.NO_FEATURE, -1)
				end
			end
		end
		--]]
	end
end

function AddIsles()
	-- This function added Feb 2011 by Bob Thomas.
	-- Adds the new feature Isles in to the game, for oceanic maps.
	local iW, iH = Map.GetGridSize()
	local biggest_ocean = Map.FindBiggestArea(true)
	local iNumBiggestOceanPlots = 0
	
	if biggest_ocean ~= nil then
		iNumBiggestOceanPlots = biggest_ocean:GetNumTiles()
	end
	if iNumBiggestOceanPlots <= (iW * iH) / 4 then -- No major oceans on this world.
		return
	end
	
	-- World has oceans, proceed with adding Isles.
	local iNumIslesPlaced = 0
	local direction_types = {
		mg.NE,
		mg.E,
		mg.SE,
		mg.SW,
		mg.W,
		mg.NW
	}
	local numCoast = 0
	local coastID = GameInfo.Terrains.TERRAIN_COAST.ID
	for i, plot in Plots() do
		if plot:GetTerrainType() == coastID then
			numCoast = numCoast + 1
		end
	end
	local isle_target = numCoast * mg.islePercent
	local variance = 25
		  variance = isle_target * (Map.Rand(2 * variance, "Number of Isles to place - LUA") - variance) / 100
	local isle_number = math.floor(isle_target + variance)
	local isleInfo = GameInfo.Features.FEATURE_ATOLL
	local isleID = isleInfo.ID

	-- Generate candidate plot lists.
	local temp_one_tile_island_list, temp_alpha_list, temp_beta_list = {}, {}, {}
	local temp_gamma_list, temp_delta_list, temp_epsilon_list = {}, {}, {}
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = y * iW + x + 1 -- Lua tables/lists/arrays start at 1, not 0 like C++ or Python
			local plot = Map.GetPlot(x, y)
			local plotType = plot:GetPlotType()
			local featureType = plot:GetFeatureType()
			local terrainType = plot:GetTerrainType()
			if terrainType == TerrainTypes.TERRAIN_COAST and featureType ~= FeatureTypes.FEATURE_ICE and not plot:IsLake() then
				-- Check all near plots and identify near landmasses.
				local iNumLandAdjacent, biggest_adj_area = 0, 0
				local bPlotValid = true
				for loop, direction in ipairs(direction_types) do
					local nearPlot = Map.PlotDirection(x, y, direction)
					if nearPlot ~= nil then
						local nearPlotType = nearPlot:GetPlotType()
						if not islesNearIce then
							if nearPlot:GetFeatureType() == FeatureTypes.FEATURE_ICE then
								bPlotValid = false
							elseif nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_SNOW then
								bPlotValid = false
							elseif nearPlot:GetTerrainType() == TerrainTypes.TERRAIN_TUNDRA then
								bPlotValid = false
							end
						end
						if nearPlotType ~= PlotTypes.PLOT_OCEAN then -- Found land.
							iNumLandAdjacent = iNumLandAdjacent + 1
							if nearPlotType == PlotTypes.PLOT_LAND or nearPlotType == PlotTypes.PLOT_HILLS then
								local iArea = nearPlot:GetArea()
								local adjArea = Map.GetArea(iArea)
								local iNumAreaPlots = adjArea:GetNumTiles()
								if iNumAreaPlots > biggest_adj_area then
									biggest_adj_area = iNumAreaPlots
								end
							end
						end
					end
				end
				-- Only plots with a single land plot near can be eligible.
				if bPlotValid and iNumLandAdjacent <= 1 then
					--if biggest_adj_area >= 152 then
						-- discard this site
					if biggest_adj_area >= 82 then
						table.insert(temp_epsilon_list, i)
					elseif biggest_adj_area >= 34 then
						table.insert(temp_delta_list, i)
					elseif biggest_adj_area >= 16 or biggest_adj_area == 0 then
						table.insert(temp_gamma_list, i)
					elseif biggest_adj_area >= 6 then
						table.insert(temp_beta_list, i)
					elseif biggest_adj_area >= 1 then
						table.insert(temp_alpha_list, i)
					else -- Unexpected result
						--print("** Area Plot Count =", biggest_adj_area)
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
	local max_alpha = math.ceil(table.maxn(alpha_list) )--* .25)
	local max_beta = math.ceil(table.maxn(beta_list) )--* .2)
	local max_gamma = math.ceil(table.maxn(gamma_list) )--* .25)
	local max_delta = math.ceil(table.maxn(delta_list) )--* .3)
	local max_epsilon = math.ceil(table.maxn(epsilon_list) )--* .4)
	
	-- Place Isles.
	local plotIndex
	local i_alpha, i_beta, i_gamma, i_delta, i_epsilon = 1, 1, 1, 1, 1		
	local passNum = 0	
	
	while (iNumIslesPlaced < isle_number) and (passNum < isle_number * 5) do
		local able_to_proceed = true
		local diceroll = 1 + Map.Rand(100, "Isle Placement Type - LUA")
		if diceroll <= 40 and max_alpha > 0 then
			plotIndex = alpha_list[i_alpha]
			i_alpha = i_alpha + 1
			max_alpha = max_alpha - 1
			--print("- Alpha site chosen")
		elseif diceroll <= 65 then
			if max_beta > 0 then
				plotIndex = beta_list[i_beta]
				i_beta = i_beta + 1
				max_beta = max_beta - 1
				--print("- Beta site chosen")
			elseif max_alpha > 0 then
				plotIndex = alpha_list[i_alpha]
				i_alpha = i_alpha + 1
				max_alpha = max_alpha - 1
				--print("- Alpha site chosen")
			else -- Unable to place this Isle
				--print("-") print("* Isle #", loop, "was unable to be placed.")
				able_to_proceed = false
			end
		elseif diceroll <= 80 then
			if max_gamma > 0 then
				plotIndex = gamma_list[i_gamma]
				i_gamma = i_gamma + 1
				max_gamma = max_gamma - 1
				--print("- Gamma site chosen")
			elseif max_beta > 0 then
				plotIndex = beta_list[i_beta]
				i_beta = i_beta + 1
				max_beta = max_beta - 1
				--print("- Beta site chosen")
			elseif max_alpha > 0 then
				plotIndex = alpha_list[i_alpha]
				i_alpha = i_alpha + 1
				max_alpha = max_alpha - 1
				--print("- Alpha site chosen")
			else -- Unable to place this Isle
				--print("-") print("* Isle #", loop, "was unable to be placed.")
				able_to_proceed = false
			end
		elseif diceroll <= 90 then
			if max_delta > 0 then
				plotIndex = delta_list[i_delta]
				i_delta = i_delta + 1
				max_delta = max_delta - 1
				--print("- Delta site chosen")
			elseif max_gamma > 0 then
				plotIndex = gamma_list[i_gamma]
				i_gamma = i_gamma + 1
				max_gamma = max_gamma - 1
				--print("- Gamma site chosen")
			elseif max_beta > 0 then
				plotIndex = beta_list[i_beta]
				i_beta = i_beta + 1
				max_beta = max_beta - 1
				--print("- Beta site chosen")
			elseif max_alpha > 0 then
				plotIndex = alpha_list[i_alpha]
				i_alpha = i_alpha + 1
				max_alpha = max_alpha - 1
				--print("- Alpha site chosen")
			else -- Unable to place this Isle
				--print("-") print("* Isle #", loop, "was unable to be placed.")
				able_to_proceed = false
			end
		else
			if max_epsilon > 0 then
				plotIndex = epsilon_list[i_epsilon]
				i_epsilon = i_epsilon + 1
				max_epsilon = max_epsilon - 1
				--print("- Epsilon site chosen")
			elseif max_delta > 0 then
				plotIndex = delta_list[i_delta]
				i_delta = i_delta + 1
				max_delta = max_delta - 1
				--print("- Delta site chosen")
			elseif max_gamma > 0 then
				plotIndex = gamma_list[i_gamma]
				i_gamma = i_gamma + 1
				max_gamma = max_gamma - 1
				--print("- Gamma site chosen")
			elseif max_beta > 0 then
				plotIndex = beta_list[i_beta]
				--print("- Beta site chosen")
				i_beta = i_beta + 1
				max_beta = max_beta - 1
			elseif max_alpha > 0 then
				plotIndex = alpha_list[i_alpha]
				i_alpha = i_alpha + 1
				max_alpha = max_alpha - 1
				--print("- Alpha site chosen")
			else -- Unable to place this Isle
				--print("-") print("* Isle #", loop, "was unable to be placed.")
				able_to_proceed = false
			end
		end
		if able_to_proceed and plotIndex then
			local x = (plotIndex - 1) % iW
			local y = (plotIndex - x - 1) / iW
			local plot = Map.GetPlot(x, y)
			for _, direction in ipairs(direction_types) do
				local nearPlot = Map.PlotDirection(x, y, direction)
				if nearPlot and nearPlot:GetFeatureType() == isleID then
					able_to_proceed = false
					--print("Adjacent isle")
					break
				end
			end
			if able_to_proceed then
				plot:SetFeatureType(isleID, -1)
				iNumIslesPlaced = iNumIslesPlaced + 1
			end
		end
		passNum = passNum + 1
	end 
	
	-- Debug report
	print("-")
	print("-                 Isle Target Number: ", isle_number)
	print("-             Number of Isles placed: ", iNumIslesPlaced)
	print("-                           Attempts: ", passNum)
	print("-")	
	print("- Isles placed in Alpha locations   : ", i_alpha - 1)
	print("- Isles placed in Beta locations    : ", i_beta - 1)
	print("- Isles placed in Gamma locations   : ", i_gamma - 1)
	print("- Isles placed in Delta locations   : ", i_delta - 1)
	print("- Isles placed in Epsilon locations : ", i_epsilon - 1)
	--]]
end













--
-- Other Generators

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
--	freqMap:Save("freqMap.csv")

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
	--twistMap:Save("twistMap.csv")
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
	--stdDevMap:Save("stdDevMap.csv")
	--mountainMap:Save("mountainCloud.csv")
	noiseMap:Normalize()
	--noiseMap:Save("noiseMap.csv")
	
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
			--mountainMap.data[i] = val
			mountainMap.data[i] = moundMap.data[i]
		end
	end
	mountainMap:Normalize()
	--mountainMap:Save("premountMap.csv")
	--moundMap:Save("moundMap.csv")

	for y = 0, mountainMap.height - 1 do
		for x = 0,mountainMap.width - 1 do
			local i = mountainMap:GetIndex(x,y)
			local val = mountainMap.data[i]
			--mountainMap.data[i] = (math.sin(val * 2 * math.pi + math.pi * 0.5)^8 * val) + moundMap.data[i] * 2 + noiseMap.data[i] * 0.6
			mountainMap.data[i] = (math.sin(val * 3 * math.pi + math.pi * 0.5)^16 * val)^0.5
			if mountainMap.data[i] > 0.2 then
				mountainMap.data[i] = 1.0
			else
				mountainMap.data[i] = 0.0
			end
		end
	end
	--mountainMap:Save("premountMap.csv")

	local stdDevThreshold = stdDevMap:FindThresholdFromPercent(mg.landPercent + 0.05,true,false)
	log:Debug("stdDevThreshold = %s", stdDevThreshold)

	for y = 0, mountainMap.height - 1 do
		for x = 0,mountainMap.width - 1 do
			local i = mountainMap:GetIndex(x,y)
			local val = mountainMap.data[i]
			local dev = 2.0 * stdDevMap.data[i] - 2.0 * stdDevThreshold
			--mountainMap.data[i] = (math.sin(val * 2 * math.pi + math.pi * 0.5)^8 * val) + moundMap.data[i] * 2 + noiseMap.data[i] * 0.6
			mountainMap.data[i] = (val + moundMap.data[i]) * dev
		end
	end

	mountainMap:Normalize()
	--mountainMap:Save("mountainMap.csv")
	
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
	local landMinScatter = 128/width * mg.landMinScatter --0.02/128
	local landMaxScatter = 128/width * mg.landMaxScatter --0.12/128
	local coastScatter = 128/width * mg.coastScatter --0.042/128
	local mountainScatter = 128/width * mg.mountainScatter --0.05/128
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
			tVal = (math.sin(tVal*math.pi-math.pi*0.5)*0.5+0.5)^0.25 --this formula adds a curve flattening the extremes
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
	--aboveSeaLevelMap:Save("aboveSeaLevelMap.csv")

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
	
	--summerMap:Save("summerMap.csv")
	--winterMap:Save("winterMap.csv")
	--temperatureMap:Save("temperatureMap.csv")
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
	--geoMap:Save("geoMap.csv")

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
			--str = string.format("topY = %d, bottomY = %d",topY,bottomY)
			--print(str)
			local dir1,dir2 = elevationMap:GetGeostrophicWindDirections(zone)
			--str = string.format("zone = %d, dir1 = %d",zone,dir1)
			--print(str)
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
			--str = string.format("yStart = %d, yStop = %d, incY = %d",yStart,yStop,incY)
			--print(str)
			--str = string.format("xStart = %d, xStop = %d, incX = %d",xStart,xStop,incX)
			--print(str)

			for y = yStart,yStop ,incY do
				--str = string.format("y = %d",y)
				--print(str)
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
	--print(#sortedGeoMap)
	--print(#geoMap.data)

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
--~ 		if y == 35 or y == 40 then
--~ 			str = string.format("x = %d, y = %d",x,y)
--~ 			print(str)
--~ 		end
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
	--rainfallSummerMap:Save("rainFallSummerMap.csv")
	rainfallWinterMap:Normalize()
	--rainfallWinterMap:Save("rainFallWinterMap.csv")
	rainfallGeostrophicMap:Normalize()
	--rainfallGeostrophicMap:Save("rainfallGeostrophicMap.csv")

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
	--local str = string.format("geo=%s,x=%d, y=%d, srcPressure uplift = %f, upliftSource = %f",tostring(boolGeostrophic),x,y,math.pow(pressureMap.data[i],mg.upLiftExponent),upLiftSource)
	--print(str)
	if elevationMap:IsBelowSeaLevel(x,y) then
		moistureMap.data[i] = math.max(moistureMap.data[i], temperatureMap.data[i])
		--print("water tile = true")
	end
	--if debugTime then print(string.format("moistureMap.data[i] = %f",moistureMap.data[i])) end

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
		--if debugTime then print(string.format("---xx=%d, yy=%d, destPressure uplift = %f, upLiftDest = %f, cost = %f, moisturePerNeighbor = %f, bonus = %f",xx,yy,math.pow(pressureMap.data[ii],mg.upLiftExponent),upLiftDest,cost,moisturePerNeighbor,bonus)) end
		rainfallMap.data[i] = rainfallMap.data[i] + cost * moisturePerNeighbor + bonus
		--pass to neighbor.
		--if debugTime then print(string.format("---moistureMap.data[ii] = %f",moistureMap.data[ii])) end
		moistureMap.data[ii] = moistureMap.data[ii] + moisturePerNeighbor - (cost * moisturePerNeighbor)
		--if debugTime then print(string.format("---dropping %f rain",cost * moisturePerNeighbor + bonus)) end
		--if debugTime then print(string.format("---passing on %f moisture",moisturePerNeighbor - (cost * moisturePerNeighbor))) end
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
--~ 	local nList = elevationMap:GetRadiusAroundHex(x,y,1)
--~ 	local i = elevationMap:GetIndex(x,y)
--~ 	local biggestDiff = 0.0
--~ 	for n=1,#nList do
--~ 		local xx = nList[n][1]
--~ 		local yy = nList[n][2]
--~ 		local ii = elevationMap:GetIndex(xx,yy)
--~ 		local diff = nil
--~ 		if elevationMap:IsBelowSeaLevel(x,y) then
--~ 			diff = elevationMap.data[i] - elevationMap.seaLevelThreshold
--~ 		else
--~ 			diff = elevationMap.data[i] - elevationMap.data[ii]
--~ 		end
--~ 		if diff > biggestDiff then
--~ 			biggestDiff = diff
--~ 		end
--~ 	end
--~ 	if biggestDiff < 0.0 then
--~ 		biggestDiff = 0.0
--~ 	end
--~ 	return biggestDiff
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
	for nearPlot, distance in Plot_GetPlotsInCircle(plot, range, yieldID) do
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
	local resID = plot:GetResourceType()
	
	if yieldID then
		value = value + plot:CalculateYield(yieldID, true)
	else
		for _, yieldID in pairs(mg.basicYields) do
			value = value + plot:CalculateYield(yieldID, true)
		end
	end
	
	if plot:IsFreshWater() then
		value = value + 0.25
	end
	
	if plot:IsLake() then
		-- can't improve lakes
		value = value - 1
	end
	
	if featureID == FeatureTypes.FEATURE_FOREST and terrainID ~= TerrainTypes.TERRAIN_TUNDRA then
		value = value + 0.5
	end
	
	if resID == -1 then
		if featureID == -1 and terrainID == TerrainTypes.TERRAIN_COAST then
			-- can't do much with these tiles in BNW
			value = value - 0.75
		end
	else
		local resInfo = GameInfo.Resources[resID]
		value = value + 4 * resInfo.Happiness
		if resInfo.ResourceClassType == "RESOURCECLASS_RUSH" and not ignoreStrategics then
			value = value + math.ceil(5 * math.sqrt(plot:GetNumResource()))
		elseif resInfo.ResourceClassType == "RESOURCECLASS_BONUS" then
			value = value + 2
		end
	end
	--]]
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
		-- iters = iters+1
		-- if iters > 300 then
			-- print("infinite loop in GetCircle")
			-- break
		-- end
	end
	-- print(string.format("added "..str.." to table for circle starting at(%d,%d)",xx,y))
	return plotIDs
end

local plotTypeName		= {}-- -1="NO_PLOT"}
local terrainTypeName	= {}-- -1="NO_TERRAIN"}
local featureTypeName	= {}-- -1="NO_FEATURE"}
function Plot_GetCirclePercents(plot, minR, maxR)
	--[[ Plot_GetCirclePercents(centerPlot, minRadius, maxRadius) usage example:

	plotPercent = Plot_GetCirclePercents(plot, 2, 2)
	if (plotPercent.PLOT_LAND + plotPercent.PLOT_HILLS) <= 0.25 then
		return
	end
	]]
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

function GetSinCurve(value, normalize, pushEdges)
	--Returns a value along a sin curve from a 0 - 1 range
	if not pushEdges then pushEdges = 0 end
	if normalize then
		value = 1 - ((normalize - pushEdges*2) - (value - pushEdges)) / (normalize - pushEdges*2)
		--value = 1 - math.abs(value - normalize) / normalize
	end
	return math.sin(value * math.pi * 2)
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

	-- for i = 0,self.length - 1,1 do
		-- self.data[i] = self.data[i] * scaler
		-- if i == self.length/2 then
			-- print("normalized: "..self.data[i])
		-- end
	-- end

	for i=0,self.length-1,1 do
		self.data[i],expo = math.frexp(self.data[i])
		self.data[i] = (self.data[i]*scaler)
		self.data[i] = math.ldexp(self.data[i],expo)
		-- if i == self.length/2 then
			-- print("normalized: "..self.data[i].." expo: "..expo)
		-- end
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
	--log:Info("FloatMap:Smooth(%s)", radius)
	local dataCopy = {}
	local i = 0
	for y = 0, self.height - 1 do
		for x = 0, self.width - 1 do
--			local i = self:GetIndex(x,y)
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
--			local i = self:GetIndex(x,y)
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
		--str = string.format("leftExtreme = %d",leftExtreme)
		--print(str)
		local x = self:ValidateX(leftExtreme)
		local y = self:ValidateY(seg.y + seg.dy)
		local i = self:GetIndex(x,y)
		--str = string.format("x = %d, y = %d, area.trueMatch = %s, matchFunction(x,y) = %s",x,y,tostring(area.trueMatch),tostring(matchFunction(x,y)))
		--print(str)
		if self.data[i] == 0 and area.trueMatch == matchFunction(x,y) then
			self.data[i] = area.id
			area.size = area.size + 1
			--print("adding to area")
			lineFound = 1
		else
			--if no line was found, then leftExtreme is fine, but if
			--a line was found going left, then we need to increment
            --xLeftExtreme to represent the inclusive end of the line
			if lineFound == 1 then
				leftExtreme = leftExtreme + 1
				--print("line found, adding 1 to leftExtreme")
			end
			break
		end
	end
	--str = string.format("leftExtreme = %d",leftExtreme)
	--print(str)
	local rightExtreme = nil
	--now scan right to find extreme right, place each found segment on stack
	if self.wrapX then
		xStop = self.width * 20
	else
		xStop = self.width
	end
	for rightExt = seg.xLeft + lineFound - odd,xStop - 1 do
		rightExtreme = rightExt --need this saved
		--str = string.format("rightExtreme = %d",rightExtreme)
		--print(str)
		local x = self:ValidateX(rightExtreme)
		local y = self:ValidateY(seg.y + seg.dy)
		local i = self:GetIndex(x,y)
		--str = string.format("x = %d, y = %d, area.trueMatch = %s, matchFunction(x,y) = %s",x,y,tostring(area.trueMatch),tostring(matchFunction(x,y)))
		--print(str)
		if self.data[i] == 0 and area.trueMatch == matchFunction(x,y) then
			self.data[i] = area.id
			area.size = area.size + 1
			--print("adding to area")
			if lineFound == 0 then
				lineFound = 1 --starting new line
				leftExtreme = rightExtreme
			end
		elseif lineFound == 1 then --found the right end of a line segment
			--print("found right end of line")
			lineFound = 0
			--put same direction on stack
			local newSeg = LineSeg:New(y,leftExtreme,rightExtreme - 1,seg.dy)
			Push(self.segStack,newSeg)
			--str = string.format("  pushing y = %d, xLeft = %d, xRight = %d, dy = %d",y,leftExtreme,rightExtreme - 1,seg.dy)
			--print(str)
			--determine if we must put reverse direction on stack
			if leftExtreme < seg.xLeft - odd or rightExtreme >= seg.xRight + notOdd then
				--out of shadow so put reverse direction on stack
				newSeg = LineSeg:New(y,leftExtreme,rightExtreme - 1,-seg.dy)
				Push(self.segStack,newSeg)
				--str = string.format("  pushing y = %d, xLeft = %d, xRight = %d, dy = %d",y,leftExtreme,rightExtreme - 1,-seg.dy)
				--print(str)
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
				--if debugTime then print(string.format("(%d,%d)WOfRiver = true dir=%d",x,y,WOfRiver)) end
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
				--if debugTime then print(string.format("(%d,%d)NWOfRiver = true dir=%d",x,y,NWOfRiver)) end
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
				--if debugTime then print(string.format("(%d,%d)NEOfRiver = true dir=%d",x,y,NEOfRiver)) end
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
	--plot:SetFeatureType(FeatureTypes.FEATURE_ICE, -1)
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
	--if debugTime then print(string.format("longestLakeList = %d",longestLakeList)) end

	--if debugTime then print(string.format("sea level = %f",self.elevationMap.seaLevelThreshold)) end

	--[[
	local belowSeaLevelCount = 0
	local riverTest = FloatMap:New(self.elevationMap.width,self.elevationMap.height,self.elevationMap.xWrap,self.elevationMap.yWrap)
	local lakesFound = false
	for y = 0,self.elevationMap.height - 1 do
		for x = 0,self.elevationMap.width - 1 do
			local i = self.elevationMap:GetIndex(x,y)

			local northAltitude = self.riverData[i].northJunction.altitude
			local southAltitude = self.riverData[i].southJunction.altitude
			if northAltitude < self.elevationMap.seaLevelThreshold then
				belowSeaLevelCount = belowSeaLevelCount + 1
			end
			if southAltitude < self.elevationMap.seaLevelThreshold then
				belowSeaLevelCount = belowSeaLevelCount + 1
			end
			riverTest.data[i] = (northAltitude + southAltitude)/2.0

			if self:isLake(self.riverData[i].northJunction) then
				local junction = self.riverData[i].northJunction
				if debugTime then print(string.format("lake found at (%d, %d) isNorth = %s, altitude = %f!",junction.x,junction.y,tostring(junction.isNorth),junction.altitude)) end
				riverTest.data[i] = 1.0
				lakesFound = true
			end
			if self:isLake(self.riverData[i].southJunction) then
				local junction = self.riverData[i].southJunction
				if debugTime then print(string.format("lake found at (%d, %d) isNorth = %s, altitude = %f!",junction.x,junction.y,tostring(junction.isNorth),junction.altitude)) end
				riverTest.data[i] = 1.0
				lakesFound = true
			end
		end
	end

	if lakesFound then
		--error("Failed to siltify lakes. check logs")
	end
	--]]
	--riverTest:Normalize()
--	riverTest:Save("riverTest.csv")
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

--~ 	local riverMap = FloatMap:New(self.elevationMap.width,self.elevationMap.height,self.elevationMap.xWrap,self.elevationMap.yWrap)
--~ 	for y = 0,self.elevationMap.height - 1 do
--~ 		for x = 0,self.elevationMap.width - 1 do
--~ 			local i = self.elevationMap:GetIndex(x,y)
--~ 			riverMap.data[i] = math.max(self.riverData[i].northJunction.size,self.riverData[i].southJunction.size)
--~ 		end
--~ 	end
--~ 	riverMap:Normalize()
	--riverMap:Save("riverSizeMap.csv")
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
-- Override AssignStartingPlots functions
--

if overrideAssignStartingPlots then
------------------------------------------------------------------------------
function AssignStartingPlots:__CustomInit()
	-- This function included to provide a quick and easy override for changing 
	-- any initial settings. Add your customized version to the map script.
	--
	if not debugPrint then
		print = function() end
	end
	--]]
	self.AdjustTiles = AssignStartingPlots.AdjustTiles
	self.BuffIslands = AssignStartingPlots.BuffIslands
	self.PlaceBonusResources = AssignStartingPlots.PlaceBonusResources
	--self.CalculateStrategicPlotWeights = AssignStartingPlots.CalculateStrategicPlotWeights
	--self.PlaceStrategicAndBonusResourcesCEP = AssignStartingPlots.PlaceStrategicAndBonusResourcesCEP
	self.islandAreaBuffed = {}
	--self.plotResInfo = {}
	--self.impactData = {}
end	
------------------------------------------------------------------------------
function AssignStartingPlots:MeasureStartPlacementFertilityOfPlot(x, y, checkForCoastalLand)
	return Plot_GetFertility(Map.GetPlot(x, y))
end
------------------------------------------------------------------------------
function AssignStartingPlots:GenerateRegions(args)
	--print("Map Generation - Dividing the map in to Regions");
	-- This function stores its data in the instance (self) data table.
	--
	-- The "Three Methods" of regional division:
	-- 1. Biggest Landmass: All civs start on the biggest landmass.
	-- 2. Continental: Civs are assigned to continents. Any continents with more than one civ are divided.
	-- 3. Rectangular: Civs start within a given rectangle that spans the whole map, without regard to landmass sizes.
	--                 This method is primarily applied to Archipelago and other maps with lots of tiny islands.
	-- 4. Rectangular: Civs start within a given rectangle defined by arguments passed in on the function call.
	--                 Arguments required for this method: iWestX, iSouthY, iWidth, iHeight
	local args = args or {};
	local iW, iH = Map.GetGridSize();
	self.method = args.method or self.method; -- Continental method is default.
	self.resource_setting = args.resources or 2; -- Each map script has to pass in parameter for Resource setting chosen by user.

	-- Determine number of civilizations and city states present in this game.
	self.iNumCivs, self.iNumCityStates, self.player_ID_list, self.bTeamGame, self.teams_with_major_civs, self.number_civs_per_team = GetPlayerAndTeamInfo()
	self.iNumCityStatesUnassigned = self.iNumCityStates;
	--print("-"); print("Civs:", self.iNumCivs); print("City States:", self.iNumCityStates);

	if self.method == 1 then -- Biggest Landmass
		-- Identify the biggest landmass.
		local biggest_area = Map.FindBiggestArea(False);
		local iAreaID = biggest_area:GetID();
		-- We'll need all eight data fields returned in the results table from the boundary finder:
		local landmass_data = ObtainLandmassBoundaries(iAreaID);
		local iWestX = landmass_data[1];
		local iSouthY = landmass_data[2];
		local iEastX = landmass_data[3];
		local iNorthY = landmass_data[4];
		local iWidth = landmass_data[5];
		local iHeight = landmass_data[6];
		local wrapsX = landmass_data[7];
		local wrapsY = landmass_data[8];
		
		-- Obtain "Start Placement Fertility" of the landmass. (This measurement is customized for start placement).
		-- This call returns a table recording fertility of all plots within a rectangle that contains the landmass,
		-- with a zero value for any plots not part of the landmass -- plus a fertility sum and plot count.
		local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityOfLandmass(iAreaID, 
		                                         iWestX, iEastX, iSouthY, iNorthY, wrapsX, wrapsY);
		-- Now divide this landmass in to regions, one per civ.
		-- The regional divider requires three arguments:
		-- 1. Number of divisions. (For "Biggest Landmass" this means number of civs in the game).
		-- 2. Fertility table. (This was obtained from the last call.)
		-- 3. Rectangle table. This table includes seven data fields:
		-- westX, southY, width, height, AreaID, fertilityCount, plotCount
		-- This is why we got the fertCount and plotCount from the fertility function.
		--
		-- Assemble the Rectangle data table:
		local rect_table = {iWestX, iSouthY, iWidth, iHeight, iAreaID, fertCount, plotCount};
		-- The data from this call is processed in to self.regionData during the process.
		self:DivideIntoRegions(self.iNumCivs, fert_table, rect_table)
		-- The regions have been defined.
	
	elseif self.method == 3 or self.method == 4 then -- Rectangular
		-- Obtain the boundaries of the rectangle to be processed.
		-- If no coords were passed via the args table, default to processing the entire map.
		-- Note that it matters if method 3 or 4 is designated, because the difference affects
		-- how city states are placed, whether they look for any uninhabited lands outside the rectangle.
		self.inhabited_WestX = args.iWestX or 0;
		self.inhabited_SouthY = args.iSouthY or 0;
		self.inhabited_Width = args.iWidth or iW;
		self.inhabited_Height = args.iHeight or iH;
		
		-- Obtain "Start Placement Fertility" inside the rectangle.
		-- Data returned is: fertility table, sum of all fertility, plot count.
		local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityInRectangle(self.inhabited_WestX, 
		                                         self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height)
		-- Assemble the Rectangle data table:
		local rect_table = {self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, 
		                    self.inhabited_Height, -1, fertCount, plotCount}; -- AreaID -1 means ignore area IDs.
		-- Divide the rectangle.
		self:DivideIntoRegions(self.iNumCivs, fert_table, rect_table)
		-- The regions have been defined.
	
	else -- Continental.
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
		
		--[[ Debug printout
		print("* * * * * * * * * *");
		for area_loop, AreaID in ipairs(land_area_IDs) do
			print("Area ID " .. AreaID .. " is land.");
		end
		--
		print("* * * * * * * * * *");
		for AreaID, fert in pairs(land_area_fert) do
			print("Area ID " .. AreaID .. " has fertility of " .. fert);
		end
		print("* * * * * * * * * *");
		--]]
		
		-- Sort areas, achieving a list of AreaIDs with best areas first.
		--
		-- Fertility data in land_area_fert is stored with areaID index keys.
		-- Need to generate a version of this table with indices of 1 to n, where n is number of land areas.
		local interim_table = {};
		for loop_index, data_entry in pairs(land_area_fert) do
			table.insert(interim_table, data_entry);
		end
		
		--[[for AreaID, fert in ipairs(interim_table) do
			print("Interim Table ID " .. AreaID .. " has fertility of " .. fert);
		end
		print("* * * * * * * * * *"); ]]--
		
		-- Sort the fertility values stored in the interim table. Sort order in Lua is lowest to highest.
		table.sort(interim_table);

		--[[
		for AreaID, fert in ipairs(interim_table) do
			print("Interim Table ID " .. AreaID .. " has fertility of " .. fert);
		end
		print("* * * * * * * * * *");
		--]]

		-- If less players than landmasses, we will ignore the extra landmasses.
		local iNumRelevantLandAreas = math.min(iNumLandAreas, self.iNumCivs);
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
				
		--[[ Debug printout
		print("-"); print("--- Continental Division, Initial Readout ---"); print("-");
		print("- Global Fertility:", iGlobalFertilityOfLands);
		print("- Total Land Plots:", iNumLandPlots);
		print("- Total Areas:", iNumLandAreas);
		print("- Relevant Areas:", iNumRelevantLandAreas); print("-");
		--]]

		--[[ Debug printout
		print("* * * * * * * * * *");
		for area_loop, AreaID in ipairs(best_areas) do
			print("Area ID " .. AreaID .. " has fertility of " .. land_area_fert[AreaID]);
		end
		print("* * * * * * * * * *");
		--]]

		-- Assign continents to receive start plots. Record number of civs assigned to each landmass.
		local inhabitedAreaIDs = {};
		local numberOfCivsPerArea = table.fill(0, iNumRelevantLandAreas); -- Indexed in synch with best_areas. Use same index to match values from each table.
		for civToAssign = 1, self.iNumCivs do
			local bestRemainingArea;
			local bestRemainingFertility = 0;
			local bestAreaTableIndex;
			-- Loop through areas, find the one with the best remaining fertility (civs added 
			-- to a landmass reduces its fertility rating for subsequent civs).
			--
			--print("- - Searching landmasses in order to place Civ #", civToAssign); print("-");
			for area_loop, AreaID in ipairs(best_areas) do
				local thisLandmassCurrentFertility = land_area_fert[AreaID] / (1 + numberOfCivsPerArea[area_loop]);
				if thisLandmassCurrentFertility > bestRemainingFertility then
					bestRemainingArea = AreaID;
					bestRemainingFertility = thisLandmassCurrentFertility;
					bestAreaTableIndex = area_loop;
					--
					--print("- Found new candidate landmass with Area ID#:", bestRemainingArea, " with fertility of ", bestRemainingFertility);
				end
			end
			-- Record results for this pass. (A landmass has been assigned to receive one more start point than it previously had).
			numberOfCivsPerArea[bestAreaTableIndex] = numberOfCivsPerArea[bestAreaTableIndex] + 1;
			if TestMembership(inhabitedAreaIDs, bestRemainingArea) == false then
				table.insert(inhabitedAreaIDs, bestRemainingArea);
			end
			--print("Civ #", civToAssign, "has been assigned to Area#", bestRemainingArea); print("-");
		end
		--print("-"); print("--- End of Initial Readout ---"); print("-");
		
		--[[print("*** Number of Civs per Landmass - Table Readout ***");
		PrintContentsOfTable(numberOfCivsPerArea)
		print("--- End of Civs per Landmass readout ***"); print("-"); print("-");
		--]]
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
			if iNumCivsOnThisLandmass > 0 and iNumCivsOnThisLandmass <= 22 then -- valid number of civs.
			
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
		--
		-- The regions have been defined.
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
function AssignStartingPlots:ExaminePlotForNaturalWondersEligibility(x, y)
	-- This function checks only for eligibility requirements applicable to all 
	-- Natural Wonders. If a candidate plot passes all such checks, we will move
	-- on to checking it against specific needs for each particular wonderID.
	--
	-- Update, May 2011: Control over wonderID placement is being migrated to XML. Some checks here moved to there.
	local iW, iH = Map.GetGridSize();
	local plotIndex = iW * y + x + 1;
	
	-- Check for collision with player starts
	if self.naturalWondersData[plotIndex] > 0 then
		return false
	end
	
	-- Check the location is a decent city site, otherwise the wonderID is pointless
	local plot = Map.GetPlot(x, y);
	if Plot_GetFertilityInRange(plot, 3) < 12 then
		return false
	end
	return true
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceNaturalWonders()
	local NW_eligibility_order = self:GenerateNaturalWondersCandidatePlotLists()
	local iNumNWCandidates = table.maxn(NW_eligibility_order);
	if iNumNWCandidates == 0 then
		print("No Natural Wonders placed, no eligible sites found for any of them.");
		return
	end
	
	--[[ Debug printout
	print("-"); print("--- Readout of wonderID Assignment Priority ---");
	for loop, wonderID in ipairs(NW_eligibility_order) do
		print("wonderID Assignment Priority#", loop, "goes to wonderID ", self.wonder_list[wonderID]);
	end
	print("-"); print("-"); --]]
	
	-- Determine how many NWs to attempt to place. Target is regulated per map size.
	-- The final number cannot exceed the number the map has locations to support.
	local target_number = mg.numNaturalWonders;
	local iNumNWtoPlace = math.min(target_number, iNumNWCandidates);
	local selected_NWs, fallback_NWs = {}, {};
	for loop, wonderID in ipairs(NW_eligibility_order) do
		if loop <= iNumNWtoPlace then
			table.insert(selected_NWs, wonderID);
		else
			table.insert(fallback_NWs, wonderID);
		end
	end
	
	--[[
	print("-");
	for loop, wonderID in ipairs(selected_NWs) do
		print("Natural Wonder ", self.wonder_list[wonderID], "has been selected for placement.");
	end
	print("-");
	for loop, wonderID in ipairs(fallback_NWs) do
		print("Natural Wonder ", self.wonder_list[wonderID], "has been selected as fallback.");
	end
	print("-");
	--
	print("--- Placing Natural Wonders! ---");
	--]]
	
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
	
	--
	if iNumPlaced >= iNumNWtoPlace then
		print("-- Placed all Natural Wonders --"); print("-"); print("-");
	else
		print("-- Not all Natural Wonders targeted got placed --"); print("-"); print("-");
	end
	--
		
end
------------------------------------------------------------------------------
function AssignStartingPlots:CanPlaceCityStateAt(x, y, area_ID, force_it, ignore_collisions)
	local iW, iH = Map.GetGridSize();
	local plot = Map.GetPlot(x, y)
	local area = plot:GetArea()
	if area ~= area_ID and area_ID ~= -1 then
		return false
	end
	
	if plot:IsWater() or plot:IsMountain() then
		return false
	end
	
	-- Avoid natural wonders
	for nearPlot in Plot_GetPlotsInCircle(plot, 1, 4) do
		local featureInfo = GameInfo.Features[nearPlot:GetFeatureType()]
		if featureInfo and featureInfo.NaturalWonder then
			log:Debug("CanPlaceCityStateAt: avoided natural wonder %s", featureInfo.Type)
			return false
		end
	end
	
	-- Reserve the best city sites for major civs
	local fertility = Plot_GetFertilityInRange(plot, 2)
	if fertility > 28 then
		log:Trace("CanPlaceCityStateAt: avoided fertility %s", fertility)
		return false
	end
	
	local plotIndex = y * iW + x + 1;
	if self.cityStateData[plotIndex] > 0 and force_it == false then
		return false
	end
	local plotIndex = y * iW + x + 1;
	if self.playerCollisionData[plotIndex] == true and ignore_collisions == false then
		--print("-"); print("City State candidate plot rejected: collided with already-placed civ or City State at", x, y);
		return false
	end
	return true
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceCityStateInRegion(city_state_number, region_number)
	--print("Place City State in Region called for City State", city_state_number, "Region", region_number);
	local iW, iH = Map.GetGridSize();
	local placed_city_state = false;
	local reached_middle = false;
	local region_data_table = self.regionData[region_number];
	local iWestX = region_data_table[1];
	local iSouthY = region_data_table[2];
	local iWidth = region_data_table[3];
	local iHeight = region_data_table[4];
	local iAreaID = region_data_table[5];
	
	local eligible_coastal, eligible_inland = {}, {};
	
	-- Main loop, first pass, unforced
	local x, y;
	local curWX = iWestX;
	local curSY = iSouthY;
	local curWid = iWidth;
	local curHei = iHeight;
	while placed_city_state == false and reached_middle == false do
		-- Send the remaining unprocessed portion of the region to be processed.
		local nextWX, nextSY, nextWid, nextHei;
		eligible_coastal, eligible_inland, nextWX, nextSY, nextWid, nextHei, 
		  reached_middle = self:ObtainNextSectionInRegion(curWX, curSY, curWid, curHei, iAreaID, false, false) -- Don't force it. Yet.
		curWX, curSY, curWid, curHei = nextWX, nextSY, nextWid, nextHei;
		-- Attempt to place city state using the two plot lists received from the last call.
		x, y, placed_city_state = self:PlaceCityState(eligible_coastal, eligible_inland, false, false) -- Don't need to re-check collisions.
	end

	if placed_city_state == true then
		-- Record and enact the placement.
		self.cityStatePlots[city_state_number] = {x, y, region_number};
		self.city_state_validity_table[city_state_number] = true; -- This is the line that marks a city state as valid to be processed by the rest of the system.
		local city_state_ID = city_state_number + GameDefines.MAX_MAJOR_CIVS - 1;
		local cityState = Players[city_state_ID];
		local cs_start_plot = Map.GetPlot(x, y)
		cityState:SetStartingPlot(cs_start_plot)
		self:GenerateLuxuryPlotListsAtCitySite(x, y, 1, true) -- Removes Feature Ice from coasts near to the city state's new location
		self:PlaceResourceImpact(x, y, 5, 4) -- City State layer
		self:PlaceResourceImpact(x, y, 2, 3) -- Luxury layer
		self:PlaceResourceImpact(x, y, 3, 3) -- Bonus layer
		self:PlaceResourceImpact(x, y, 4, 3) -- Fish layer
		self:PlaceResourceImpact(x, y, 7, 3) -- Marble layer
		if cityState:GetMinorCivTrait() == MinorCivTraitTypes.MINOR_CIV_TRAIT_MILITARISTIC then
			self:PlaceResourceImpact(x, y, 1, 0) -- Strategic layer, at start point only.
		else
			self:PlaceResourceImpact(x, y, 1, 3) -- Strategic layer
		end
		local impactPlotIndex = y * iW + x + 1;
		self.playerCollisionData[impactPlotIndex] = true;
		--print("-"); print("City State", city_state_number, "has been started at Plot", x, y, "in Region#", region_number);
	else
		--print("-"); print("WARNING: Crowding issues for City State #", city_state_number, " - Could not find valid site in Region#", region_number);
		self.iNumCityStatesDiscarded = self.iNumCityStatesDiscarded + 1;
	end
end
--[[----------------------------------------------------------------------------
function AssignStartingPlots:AssignCityStatesToRegionsOrToUninhabited(args)
	-- Assign city states away from the main continent on Terra maps.
	local iW, iH = Map.GetGridSize()
	self.iNumCityStatesPerRegion = 0;
	local current_cs_index = 1;

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
	if self.method == 3 then -- Rectangular regional division spanning the entire globe, ALL plots belong to inhabited regions.
		self.iNumCityStatesUninhabited = 0;
		--print("Rectangular regional division spanning the whole world: all city states must belong to a region!");
	else -- Possibility of plots that do not belong to any civ's Region. Evaluate these plots and assign an appropriate number of City States to them.
		-- Generate list of inhabited area IDs.
		if self.method == 1 or self.method == 2 then
			for index, region_data in ipairs(self.regionData) do
				local region_areaID = region_data[5];
				if TestMembership(areas_inhabited_by_civs, region_areaID) == false then
					table.insert(areas_inhabited_by_civs, region_areaID);
				end
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
					if self.method == 4 then -- Determine if plot is inside or outside the regional rectangle
						if (x >= self.inhabited_WestX and x <= self.inhabited_WestX + self.inhabited_Width - 1) and
						   (y >= self.inhabited_SouthY and y <= self.inhabited_SouthY + self.inhabited_Height - 1) then -- Civ-inhabited rectangle
							iNumCivLandmassPlots = iNumCivLandmassPlots + 1;
						else
							iNumUninhabitedLandmassPlots = iNumUninhabitedLandmassPlots + plot_count;
							if self.plotDataIsCoastal[i] == true then
								table.insert(self.uninhabited_areas_coastal_plots, i);
							else
								table.insert(self.uninhabited_areas_inland_plots, i);
							end
						end
					else -- AreaID-based method must be applied, which cannot all be done in this loop
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
		end
		-- Complete the AreaID-based method. 
		if self.method == 1 or self.method == 2 then
			-- Obtain counts of inhabited and uninhabited plots. Identify areas too small to use for City States.
			for areaID, plot_count in pairs(land_area_plot_count) do
				if TestMembership(areas_inhabited_by_civs, areaID) == true then 
					iNumCivLandmassPlots = iNumCivLandmassPlots + plot_count;
				else
					iNumUninhabitedLandmassPlots = iNumUninhabitedLandmassPlots + plot_count;
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
		end
		-- Determine the number of City States to assign to uninhabited areas.
		
		self.iNumCityStatesUninhabited = mg.offshoreCS * self.iNumCityStates;
		self.iNumCityStatesUnassigned = self.iNumCityStatesUnassigned - self.iNumCityStatesUninhabited;
	end
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
		--print("All city states assigned.");
	end
end
--]]----------------------------------------------------------------------------
function AssignStartingPlots:BuffIslands()
	print("Buffing Tiny Islands")
	local biggestAreaSize = Map.FindBiggestArea(false):GetNumTiles()
	if biggestAreaSize < 20 then
		-- Skip on archipalego maps
		return
	end
	local resWeights = {
		[self.stone_ID]		= 4,
		[self.coal_ID]		= 4,
		[self.oil_ID]		= 1,
		[self.aluminum_ID]	= 1,
		[self.uranium_ID]	= 2
	}
	for plotID, plot in Plots(Shuffle) do
		local plotType		= plot:GetPlotType()
		local terrainType	= plot:GetTerrainType()
		local area			= plot:Area()
		local areaSize		= area:GetNumTiles()
		if ((plotType == PlotTypes.PLOT_HILLS or plotType == PlotTypes.PLOT_LAND )
				and plot:GetResourceType() == -1
				and IsBetween(1, areaSize, 0.1 * biggestAreaSize)
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
				if 75 >= Map.Rand(100, "BuffIslands Chance - Lua") then
					if resID == self.coal_ID and plotType == PlotTypes.PLOT_LAND then
						if terrainType == TerrainTypes.TERRAIN_TUNDRA then
							plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
						elseif terrainType == TerrainTypes.TERRAIN_GRASS or terrainType == TerrainTypes.TERRAIN_PLAINS then
							plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE, -1)
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
	-- Sugar could not be made to look good in both jungle and open/marsh at the same time.
	-- Jon and I decided the best workaround would be to turn any Sugar/Jungle in to Marsh.
	local iW, iH = Map.GetGridSize()
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local plot = Map.GetPlot(x, y)
			local resID = plot:GetResourceType()
			local featureType = plot:GetFeatureType();
			if resID == self.sugar_ID then
				if featureType == FeatureTypes.FEATURE_JUNGLE then
					local plotID = plot:GetPlotType()
					if plotID ~= PlotTypes.PLOT_LAND then
						plot:SetPlotType(PlotTypes.PLOT_LAND, false, true)
					end
					plot:SetFeatureType(FeatureTypes.FEATURE_MARSH, -1)
					plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true)
					--
					--print("-"); print("Fixed a Sugar/Jungle at plot", x, y);
				end
			elseif resID == self.deer_ID then
				if featureType == FeatureTypes.NO_FEATURE then
					local plotID = plot:GetPlotType()
					if plotID ~= PlotTypes.PLOT_LAND then
						plot:SetPlotType(PlotTypes.PLOT_LAND, false, true)
					end
					plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
					plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true)
					--
					--print("-"); print("Added forest to deer at plot", x, y);
				end
			end
			if plot:GetTerrainType() == TerrainTypes.TERRAIN_SNOW then
				if Plot_IsRiver(plot) then
					plot:SetTerrainType(TerrainTypes.TERRAIN_TUNDRA,false,true)
				end
			end
			
			if plot:IsHills() and featureType == FeatureTypes.FEATURE_JUNGLE then
				plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true)
				Game.SetPlotExtraYield( x, y, YieldTypes.YIELD_FOOD, -1)
			end
			
			--BuffDeserts(plot)					
		end
	end
end
------------------------------------------------------------------------------
function BuffDeserts(plot)
	if Cep then
		return
	end
	if plot:GetTerrainType() ~= TerrainTypes.TERRAIN_DESERT or plot:IsHills() or plot:GetFeatureType() ~= -1 then
		return
	end
	
	local resInfo = GameInfo.Resources[plot:GetResourceType()]
	if plot:IsFreshWater() then
		Game.SetPlotExtraYield( x, y, YieldTypes.YIELD_FOOD, 1)
	elseif resInfo then
		if resInfo.Type == "RESOURCE_STONE" then
			Game.SetPlotExtraYield( x, y, YieldTypes.YIELD_PRODUCTION, 2)
		elseif resInfo.ResourceClassType == "RESOURCECLASS_BONUS" then
			Game.SetPlotExtraYield( x, y, YieldTypes.YIELD_FOOD, 1)
		elseif resInfo.Happiness > 0 then
			Game.SetPlotExtraYield( x, y, YieldTypes.YIELD_GOLD, 1)
		elseif not resInfo.TechReveal then
			Game.SetPlotExtraYield( x, y, YieldTypes.YIELD_PRODUCTION, 1)
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:ProcessResourceList(frequency, impact_table_number, plot_list, resources_to_place)
	-- Added a random factor to strategic resources - Thalassicus

	-- This function needs to receive two numbers and two tables.
	-- Length of the plotlist is divided by frequency to get the number of 
	-- resources to place. .. The first table is a list of plot indices.
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
		--print("Plot list was nil! -ProcessResourceList");
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
				if impact_table_number == 1 then
					if self.strategicData[plotIndex] == 0 then
						local x = (plotIndex - 1) % iW;
						local y = (plotIndex - x - 1) / iW;
						local res_plot = Map.GetPlot(x, y)
						if res_plot:GetResourceType() == -1 then -- Placing this strategic resource in this plot.
							local res_addition = 0;
							if res_range[use_this_res_index] ~= -1 then
								res_addition = Map.Rand(res_range[use_this_res_index], "Resource Radius - Place Resource LUA");
							end
							local randValue = Map.Rand(self.resource_setting + 1, "Place Strategic Resource - Lua")
							local quantity = res_quantity[use_this_res_index] + randValue
							--print(string.format("ProcessResourceList table 1, Resource: %20s, Quantity: %s + %s - 1", GameInfo.Resources[res_ID[use_this_res_index]].Type, res_quantity[use_this_res_index], randValue));
							res_plot:SetResourceType(res_ID[use_this_res_index], quantity);
							if (Game.GetResourceUsageType(res_ID[use_this_res_index]) == ResourceUsageTypes.RESOURCEUSAGE_LUXURY) then
								self.totalLuxPlacedSoFar = self.totalLuxPlacedSoFar + 1;
							end
							self:PlaceResourceImpact(x, y, impact_table_number, res_min[use_this_res_index] + res_addition);
							placed_this_res = true;
							self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] = self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] + res_quantity[use_this_res_index];
						end
					end
				elseif impact_table_number == 2 then
					if self.luxuryData[plotIndex] == 0 then
						local x = (plotIndex - 1) % iW;
						local y = (plotIndex - x - 1) / iW;
						local res_plot = Map.GetPlot(x, y)
						if res_plot:GetResourceType() == -1 then -- Placing this luxury resource in this plot.
							local res_addition = 0;
							if res_range[use_this_res_index] ~= -1 then
								res_addition = Map.Rand(res_range[use_this_res_index], "Resource Radius - Place Resource LUA");
							end
							--print("ProcessResourceList table 2, Resource: " .. res_ID[use_this_res_index] .. ", Quantity: " .. res_quantity[use_this_res_index]);
							res_plot:SetResourceType(res_ID[use_this_res_index], res_quantity[use_this_res_index]);
							self:PlaceResourceImpact(x, y, impact_table_number, res_min[use_this_res_index] + res_addition);
							placed_this_res = true;
							self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] = self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] + 1;
						end
					end
				elseif impact_table_number == 3 then
					if self.bonusData[plotIndex] == 0 then
						local x = (plotIndex - 1) % iW;
						local y = (plotIndex - x - 1) / iW;
						local res_plot = Map.GetPlot(x, y)
						if res_plot:GetResourceType() == -1 then -- Placing this bonus resource in this plot.
							local res_addition = 0;
							if res_range[use_this_res_index] ~= -1 then
								res_addition = Map.Rand(res_range[use_this_res_index], "Resource Radius - Place Resource LUA");
							end
							--print("ProcessResourceList table 3, Resource: " .. res_ID[use_this_res_index] .. ", Quantity: " .. res_quantity[use_this_res_index]);
							res_plot:SetResourceType(res_ID[use_this_res_index], res_quantity[use_this_res_index]);
							self:PlaceResourceImpact(x, y, impact_table_number, res_min[use_this_res_index] + res_addition);
							placed_this_res = true;
							self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] = self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] + 1;
							if res_ID[use_this_res_index] == self.stone_ID then
								self.islandAreaBuffed[res_plot:GetArea()] = true
							end
						end
					end
				end
			end
		end
		if avoid_ripples == false then -- Completed first pass through plot_list, so use backup method.
			local lowest_impact = 98;
			local best_plot;
			for loop, plotIndex in ipairs(plot_list) do
				if impact_table_number == 1 then
					if lowest_impact > self.strategicData[plotIndex] then
						local x = (plotIndex - 1) % iW;
						local y = (plotIndex - x - 1) / iW;
						local res_plot = Map.GetPlot(x, y)
						if res_plot:GetResourceType() == -1 then
							lowest_impact = self.strategicData[plotIndex];
							best_plot = plotIndex;
						end
					end
				elseif impact_table_number == 2 then
					if lowest_impact > self.luxuryData[plotIndex] then
						local x = (plotIndex - 1) % iW;
						local y = (plotIndex - x - 1) / iW;
						local res_plot = Map.GetPlot(x, y)
						if res_plot:GetResourceType() == -1 then
							lowest_impact = self.luxuryData[plotIndex];
							best_plot = plotIndex;
						end
					end
				elseif impact_table_number == 3 then
					if lowest_impact > self.bonusData[plotIndex] then
						local x = (plotIndex - 1) % iW;
						local y = (plotIndex - x - 1) / iW;
						local res_plot = Map.GetPlot(x, y)
						if res_plot:GetResourceType() == -1 then
							lowest_impact = self.bonusData[plotIndex];
							best_plot = plotIndex;
						end
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
				--print("ProcessResourceList backup, Resource: " .. res_ID[use_this_res_index] .. ", Quantity: " .. res_quantity[use_this_res_index]);
				res_plot:SetResourceType(res_ID[use_this_res_index], res_quantity[use_this_res_index]);
				self:PlaceResourceImpact(x, y, impact_table_number, res_min[use_this_res_index] + res_addition);
				self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] = self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] + res_quantity[use_this_res_index];
			end
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceSpecificNumberOfResources(resource_ID, quantity, amount,
	                         ratio, impact_table_number, min_radius, max_radius, plot_list)
	-- This function needs to receive seven numbers and one table.
	--
	-- Resource_ID is the type of resource to place.
	-- Quantity is the in-game quantity of the resource, or 0 if unquantified resource type.
	-- Amount is the number of plots intended to receive an assignment of this resource.
	--
	-- Ratio should be > 0 and <= 1 and is what determines when secondary and tertiary lists 
	-- come in to play. The actual ratio is (AmountOfResource / PlotsInList). For instance, 
	-- if we are assigning Sugar resources to Marsh, then if we are to assign eight Sugar 
	-- resources, but there are only four Marsh plots in the list, a ratio of 1 would assign
	-- a Sugar to every single marsh plot, and then have to return an unplaced value of 4; 
	-- but a ratio of 0.5 would assign only two Sugars to the four marsh plots, and return a 
	-- value of 6. Any ratio less than or equal to 0.25 would assign one Sugar and return
	-- seven, as the ratio results will be rounded up not down, to the nearest integer.
	--
	-- Impact tables: -1 = ignore, 1 = strategic, 2 = luxury, 3 = bonus, 4 = fish
	-- Radius is amount of impact to place on this table when placing a resource.
	--
	-- nil tables are not acceptable but empty tables are fine
	--
	-- The plot lists will be processed sequentially, so randomize them in advance.
	-- 
	
	--print("-"); print("PlaceSpecificResource called. ResID:", resource_ID, "Quantity:", quantity, "Amount:", amount, "Ratio:", ratio);
	
	if plot_list == nil then
		--print("Plot list was nil! -PlaceSpecificNumberOfResources");
		return
	end
	local bCheckImpact = false;
	local impact_table = {};
	if impact_table_number == 1 then
		bCheckImpact = true;
		impact_table = self.strategicData;
	elseif impact_table_number == 2 then
		bCheckImpact = true;
		impact_table = self.luxuryData;
	elseif impact_table_number == 3 then
		bCheckImpact = true;
		impact_table = self.bonusData;
	elseif impact_table_number == 4 then
		bCheckImpact = true;
		impact_table = self.fishData;
	elseif impact_table_number ~= -1 then
		bCheckImpact = true;
		impact_table = self.impactData[impact_table_number];
	end
	local iW, iH = Map.GetGridSize();
	local iNumLeftToPlace = amount;
	local iNumPlots = table.maxn(plot_list);
	local iNumResources = math.min(amount, math.ceil(ratio * iNumPlots));
	-- Main loop
	for place_resource = 1, iNumResources do
		for loop, plotIndex in ipairs(plot_list) do
			if not bCheckImpact or impact_table[plotIndex] == 0 then
				local x = (plotIndex - 1) % iW;
				local y = (plotIndex - x - 1) / iW;
				local res_plot = Map.GetPlot(x, y)
				if res_plot:GetResourceType(-1) == -1 then -- Placing this resource in this plot.
					res_plot:SetResourceType(resource_ID, quantity);
					self.amounts_of_resources_placed[resource_ID + 1] = self.amounts_of_resources_placed[resource_ID + 1] + quantity;
					--print("-"); print("Placed Resource#", resource_ID, "at Plot", x, y);
					self.totalLuxPlacedSoFar = self.totalLuxPlacedSoFar + 1;
					iNumLeftToPlace = iNumLeftToPlace - 1;
					if bCheckImpact == true then
						local res_addition = 0;
						if max_radius > min_radius then
							res_addition = Map.Rand(1 + (max_radius - min_radius), "Resource Radius - Place Resource LUA");
						end
						local rad = min_radius + res_addition;
						self:PlaceResourceImpact(x, y, impact_table_number, rad)
					end
					break
				end
			end
		end
	end
	return iNumLeftToPlace
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetMajorStrategicResourceQuantityValues()
	-- This function determines quantity per tile for each strategic resource's major deposit size.
	-- Note: scripts that cannot place Oil in the sea need to increase amounts on land to compensate.
	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 4, 4, 7, 6, 7, 8;
	-- Check the resource setting.
	if self.resource_setting == 1 then -- Sparse
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 4, 5, 4, 5, 5;
	elseif self.resource_setting == 3 then -- Abundant
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 4, 6, 9, 9, 10, 10;
	end
	return uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetSmallStrategicResourceQuantityValues()
	-- This function determines quantity per tile for each strategic resource's small deposit size.
	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 2, 2, 2, 2, 3;
	-- Check the resource setting.
	if self.resource_setting == 1 then -- Sparse
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 1, 1, 2, 1, 2, 2;
	elseif self.resource_setting == 3 then -- Abundant
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 3, 3, 3, 3, 3;
	end
	return uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceOilInTheSea()
	-- Places sources of Oil in Coastal waters, equal to half what's on the 
	-- land. If the map has too little ocean, then whatever will fit.
	--
	-- WARNING: This operation will render the Strategic Resource Impact Table useless for
	-- further operations, so should always be called last, even after minor placements.
	local sea_oil_amt = 2 --+ Map.Rand(self.resource_setting, "PlaceOilInTheSea - Lua");
	local iNumLandOilUnits = self.amounts_of_resources_placed[self.oil_ID + 1];
	local iNumToPlace = math.floor((iNumLandOilUnits / 2) / sea_oil_amt);

	--print("Adding Oil resources to the Sea.");
	self:PlaceSpecificNumberOfResources(self.oil_ID, sea_oil_amt, iNumToPlace, 0.2, 1, 4, 7, self.coast_list)
end
--[=[----------------------------------------------------------------------------
function AssignStartingPlots:CalculateStrategicPlotWeights()
	local iW, iH = Map.GetGridSize()	
	local resIDs = Game.GetResourceIDsOfUsage(ResourceUsageTypes.RESOURCEUSAGE_STRATEGIC)
	local plotTypeStrings = {}
	plotTypeStrings[-1]	= "NO_PLOT"
	plotTypeStrings[0]	= "PLOT_MOUNTAIN"
	plotTypeStrings[1]	= "PLOT_HILLS"
	plotTypeStrings[2]	= "PLOT_LAND"
	plotTypeStrings[3]	= "PLOT_OCEAN"
	
	print("CalculateStrategicPlotWeights")
	
	-- Initialize tables
	for resInfo in GameInfo.Resources() do
		local resourceID = resInfo.ID
		self.plotResInfo[resourceID] = {}
		for x = 0, iW - 1 do
			for y = 0, iH - 1 do
				local plotID = y * iW + x + 1
				self.plotResInfo[resourceID][plotID] = {}
				self.plotResInfo[resourceID][plotID].numPlots	= 0
				self.plotResInfo[resourceID][plotID].weight		= 1
				self.plotResInfo[resourceID][plotID].weightAvg	= 0
				self.impactData [100+resourceID] = {}
				self.impactData [100+resourceID][plotID]		= 0
			end
		end
	end
	
	-- Pass 1: Plot Weights
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local plot				= Map.GetPlot(x, y)
			local plotID			= y * iW + x + 1
			local typeTable     	= {}
			typeTable.PlotType		= plotTypeStrings[plot:GetPlotType()]
			typeTable.NotLake		= not plot:IsLake()
			typeTable.Freshwater	= plot:IsFreshWater()
			typeTable.TerrainType	= plot:GetTerrainType()
			typeTable.FeatureType	= plot:GetFeatureType()
			typeTable.TerrainType	= (typeTable.TerrainType == TerrainTypes.NO_TERRAIN) and "NO_TERRAIN" or GameInfo.Terrains[typeTable.TerrainType].Type
			typeTable.FeatureType	= (typeTable.FeatureType == FeatureTypes.NO_FEATURE) and "NO_FEATURE" or GameInfo.Features[typeTable.FeatureType].Type
			
			for resInfo in GameInfo.Resource_TerrainWeights() do
				local resID = GameInfo.Resources[resInfo.ResourceType].ID
				local useWeight = true				
				for k, v in pairs(typeTable) do
					if resInfo[k] and resInfo[k] ~= v then
						useWeight = false
					end
				end
				if useWeight then
					self.plotResInfo[resID][plotID].weight = self.plotResInfo[resID][plotID].weight * resInfo.Weight
				end
			end
		end
	end
	--
	
	-- Pass 2: Plot Weight Averages
	local randVariance = 100
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local plot			= Map.GetPlot(x, y)
			local plotID		= y * iW + x + 1
			local terrainType	= plot:GetTerrainType()
			for nearPlot, distance in Plot_GetPlotsInCircle(plot, 0, Cep.RESOURCE_PLOT_BLUR_DISTANCE) do
				local nearPlotID	= Plot_GetID(nearPlot)
				local nearPlotType	= nearPlot:GetPlotType()
				for _, resID in pairs(resIDs) do
					print(string.format(("self.plotResInfo[resID]=%s nearPlotID=%s", nearPlotID, tostring(self.plotResInfo[resID]))))
					local nearWeight = self.plotResInfo[resID]
					nearWeight = nearWeight[nearPlotID]
					nearWeight = nearWeight.weight
					if (nearPlotType == PlotTypes.PLOT_LAND 
						or nearPlotType == PlotTypes.PLOT_HILLS
						or (resID ~= self.oil_ID and self.plotResInfo[resID][nearPlotID].weight ~= 1)
						)then
						self.plotResInfo[resID][plotID].weightAvg = self.plotResInfo[resID][plotID].weightAvg + nearWeight / (distance + 1)
						self.plotResInfo[resID][plotID].numPlots = self.plotResInfo[resID][plotID].numPlots + 1 / (distance + 1)
					end
					--
					if resID == self.oil_ID and plot:GetTerrainType() == TerrainTypes.TERRAIN_COAST and not plot:IsLake() then
						if nearPlotType ~= PlotTypes.PLOT_OCEAN then
							-- Sea oil favors terrain where land oil avoids
							if nearWeight == 0 then
								nearWeight = 0.1
							end
							nearWeight = 1 / nearWeight
						end
						self.plotResInfo[resID][plotID].weightAvg = self.plotResInfo[resID][plotID].weightAvg + nearWeight / (distance + 1)
						self.plotResInfo[resID][plotID].numPlots = self.plotResInfo[resID][plotID].numPlots + 1 / (distance + 1)
					end
				end
			end
			for _, resID in pairs(resIDs) do
				if self.plotResInfo[resID][plotID].numPlots == 0 then
					self.plotResInfo[resID][plotID].numPlots = 1
				end
				local rand = 0.01 * Map.Rand(randVariance, "AssignStartingPlots:PlaceStrategicAndBonusResources()") / randVariance
				self.plotResInfo[resID][plotID].weightAvg = rand + self.plotResInfo[resID][plotID].weightAvg / self.plotResInfo[resID][plotID].numPlots --* self.plotResInfo[resID][plotID].weight 
			end
			local testNum = 0--math.min(120, math.floor(self.plotResInfo[self.oil_ID][plotID].weightAvg * 10))
			if testNum >= 1 then
				self:PlaceSpecificNumberOfResources(self.iron_ID, testNum, 1, 1, -1, 0, 0, {plotID})
			end
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetRegionStrategicPlotList(regionInfo)
	local iW, iH	= Map.GetGridSize()
	local plotList	= {}
	local iWestX	= regionInfo[1]
	local iSouthY	= regionInfo[2]
	local iWidth	= regionInfo[3]
	local iHeight	= regionInfo[4]
	local resIDs	= Game.GetResourceIDsOfUsage(ResourceUsageTypes.RESOURCEUSAGE_STRATEGIC)
	
	for _, resID in pairs(resIDs) do
		plotList[resID] = {}
	end
	
	for region_loop_y = 0, iHeight - 1 do
		for region_loop_x = 0, iWidth - 1 do
			local x				= (region_loop_x + iWestX) % iW
			local y				= (region_loop_y + iSouthY) % iH
			local plotID		= y * iW + x + 1
			local plot			= Map.GetPlot(x, y)
			local plotType		= plot:GetPlotType()
			local featureType	= plot:GetFeatureType()
			for _, resID in pairs(resIDs) do
				if self.plotResInfo[resID][plotID].weightAvg > 0 then
					if plotType == PlotTypes.PLOT_HILLS or plotType == PlotTypes.PLOT_LAND then
						if plotType == PlotTypes.PLOT_LAND and (resID == self.aluminum_ID or resID == self.coal_ID) then
							self.plotResInfo[resID][plotID].weightAvg = self.plotResInfo[resID][plotID].weightAvg * 0.1
						end
						if featureType == FeatureTypes.NO_FEATURE then
							table.insert(plotList[resID], plotID)
						else
							local feature = GameInfo.Features[featureType]
							if not (feature.Impassable or feature.NoImprovement or feature.NaturalWonder) then
								table.insert(plotList[resID], plotID)
							end
						end
					elseif resID == self.oil_ID and plot:GetTerrainType() == TerrainTypes.TERRAIN_COAST and plot:IsAdjacentToLand() and plot:GetFeatureType() == FeatureTypes.NO_FEATURE and not plot:IsLake() then
						table.insert(plotList[resID], plotID)
					end
				end
			end
		end
	end
	return plotList
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceStrategicResourceInPlots(plotList, resID, resRemaining, maxDepositSize)
	local iW, iH = Map.GetGridSize()
	local depositSize = math.min(resRemaining, 1 + Map.Rand(maxDepositSize, "Strategic Resource Placement"))
	for _, plotID in ipairs(plotList) do
		local x = (plotID - 1) % iW;
		local y = (plotID - x - 1) / iW;
		local plot = Map.GetPlot(x, y)
		if plot:GetResourceType() == -1 then
			--print("  placed %s", depositSize)
			plot:SetResourceType(resID, depositSize);
			self.amounts_of_resources_placed[resID + 1] = self.amounts_of_resources_placed[resID + 1] + depositSize;
			resRemaining = resRemaining - depositSize;
			
			for nearPlot, distance in Plot_GetPlotsInCircle(plot, 0, Cep.RESOURCE_PLOT_BLUR_DISTANCE) do
				local nearPlotID = Plot_GetID(nearPlot)
				self.plotResInfo[resID][nearPlotID].weightAvg = self.plotResInfo[resID][nearPlotID].weightAvg * (1 - 1/(distance/depositSize * (1+Map.Rand(maxDepositSize, "Strategic Resource Placement"))))
			end
			
			break
		end
	end
	return resRemaining
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetResourceQuantities(resIDs)
	local resNum			= {}
	local resLower			= Cep.STRATEGIC_RESOURCE_LOWER_BOUND
	local resUpper			= Cep.STRATEGIC_RESOURCE_UPPER_BOUND
	local maxDepositSize	= 1
	local stratMultiplier	= 1
	local resGroups			= {}
	
	if self.resource_setting == 1 then
		stratMultiplier = 0.66667
	elseif self.resource_setting == 3 then
		stratMultiplier = 1.66667
	end

	for _, resID in pairs(resIDs) do
		resInfo = GameInfo.Resources[resID]
		if resInfo.MutuallyExclusiveGroup == -1 then
			resNum[resID] = resInfo.NumPerTerritory * stratMultiplier * 0.01 * (resLower + Map.Rand(resUpper - resLower, "Strategic Resource Placement"))
			if resNum[resID] < 0 then
				local worldInfo = GameInfo.Worlds[Map.GetWorldSize()]
				local resourceMod = 100
				--[[
				if not worldInfo.ResourceMod then
					print("Worlds[%s] has no ResourceMod column", worldInfo.Type)
				else
					resourceMod = worldInfo.ResourceMod
				end
				--]]
				resNum[resID] = -1 * resNum[resID] * resourceMod / 100
			end
			resNum[resID] = Game.Round(resNum[resID])			
		else
			resGroups[resInfo.MutuallyExclusiveGroup] = resGroups[resInfo.MutuallyExclusiveGroup] or {}
			resGroups[resInfo.MutuallyExclusiveGroup][resID] = resInfo.NumPerTerritory
		end
	end
	
	for resGroupID, resGroup in pairs(resGroups) do
		local groupSize			= 0
		local groupTotal		= 0
		local groupNormTotal	= 0
		local resBoostIndex		= 0
		local resWeight			= {}
		
		-- total for the group
		for resID, resNum in pairs(resGroup) do
			groupTotal	= groupTotal + resNum
			groupSize	= groupSize + 1
		end
		groupTotal = Game.Round(groupTotal * stratMultiplier * 0.01 * (resLower + Map.Rand(resUpper - resLower, "Strategic Resource Placement")))
		
		-- pick a random resource to boost
		resBoostIndex = Map.Rand(groupSize, "Strategic Resource Placement")
		local i = 0
		
		for resID, resNum in pairs(resGroup) do
			if i == resBoostIndex then
				resWeight[resID] = 2 * resNum
			else
				resWeight[resID] = resNum
			end
			groupNormTotal = groupNormTotal + resWeight[resID]
			i = i + 1
		end
		
		-- normalize weights to get real quantity
		for resID, resNum in pairs(resWeight) do
			resNum[resID] = resWeight[resID] * groupTotal/groupNormTotal
		end		
	end
	
	return resNum
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceStrategicAndBonusResourcesCEP()
	print("PlaceStrategicAndBonusResourcesCEP")
	local resIDs = Game.GetResourceIDsOfUsage(ResourceUsageTypes.RESOURCEUSAGE_STRATEGIC)
	
	-- Place Strategic Resources
	print("Placing Strategic Resources")
	local placedStrategics = false
	
	self:CalculateStrategicPlotWeights() -- Must do this before strategic resource placement!
	
	
	
	for regionID, regionInfo in ipairs(self.regionData) do
		local plotList = self:GetRegionStrategicPlotList(regionInfo)
		local resNum = self:GetResourceQuantities(resIDs)
		for _, resID in pairs(resIDs) do
			local resRemaining = resNum[resID]
			local maxDepositSize = math.ceil(resRemaining * 80 / #(plotList[resID]))
			local passes = 0
			--print("Placing %2s %9s in Region %2s", resRemaining, Locale.ConvertTextKey(GameInfo.Resources[resID].Description), regionID)
			while resRemaining > 0 and passes < 30 do
				passes = passes + 1
				table.sort(plotList[resID], function (a,b)
					return self.plotResInfo[resID][a].weightAvg > self.plotResInfo[resID][b].weightAvg
				end)				
				resRemaining = self:PlaceStrategicResourceInPlots(plotList[resID], resID, resRemaining, maxDepositSize)
			end
			
			if resRemaining <= 0 then
				placedStrategics = true
			else
				--print("Unable to place %i %s in region %i after %i passes.", resRemaining, GameInfo.Resources[resID].Type, regionID, passes)
			end
		end
	end
	
	local resMultiplier = 0.66667
	if self.resource_setting == 1 then -- Sparse, so increase the number of tiles per bonus.
		resMultiplier = resMultiplier * 1.5
	elseif self.resource_setting == 3 then -- Abundant, so reduce the number of tiles per bonus.
		resMultiplier = resMultiplier * 0.66667
	end
	
	-- If territorial placement failed, fall back on vanilla system.
	if not placedStrategics then
		print("Map Generation - Strategic resource placement failed, falling back to vanilla.")
		-- Adjust amounts, if applicable, based on Resource Setting.
		local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = self:GetMajorStrategicResourceQuantityValues()

		-- Place Strategic resources.
		print("Map Generation - Placing Strategics");
		local resources_to_place = {
		{self.oil_ID, oil_amt, 65, 1, 1},
		{self.uranium_ID, uran_amt, 35, 0, 1} };
		self:ProcessResourceList(9, 1, self.marsh_list, resources_to_place)

		local resources_to_place = {
		{self.oil_ID, oil_amt, 40, 1, 2},
		{self.aluminum_ID, alum_amt, 15, 1, 2},
		{self.iron_ID, iron_amt, 45, 1, 2} };
		self:ProcessResourceList(16, 1, self.tundra_flat_no_feature, resources_to_place)

		local resources_to_place = {
		{self.oil_ID, oil_amt, 60, 1, 1},
		{self.aluminum_ID, alum_amt, 15, 2, 3},
		{self.iron_ID, iron_amt, 25, 2, 3} };
		self:ProcessResourceList(17, 1, self.snow_flat_list, resources_to_place)

		local resources_to_place = {
		{self.oil_ID, oil_amt, 65, 0, 1},
		{self.iron_ID, iron_amt, 35, 1, 1} };
		self:ProcessResourceList(13, 1, self.desert_flat_no_feature, resources_to_place)

		local resources_to_place = {
		{self.iron_ID, iron_amt, 26, 0, 2},
		{self.coal_ID, coal_amt, 35, 1, 3},
		{self.aluminum_ID, alum_amt, 39, 2, 3} };
		self:ProcessResourceList(22, 1, self.hills_list, resources_to_place)

		local resources_to_place = {
		{self.coal_ID, coal_amt, 30, 1, 2},
		{self.uranium_ID, uran_amt, 70, 1, 2} };
		self:ProcessResourceList(33, 1, self.jungle_flat_list, resources_to_place)
		
		local resources_to_place = {
		{self.coal_ID, coal_amt, 30, 1, 2},
		{self.uranium_ID, uran_amt, 70, 1, 1} };
		self:ProcessResourceList(39, 1, self.forest_flat_list, resources_to_place)

		local resources_to_place = {
		{self.horse_ID, horse_amt, 100, 2, 5} };
		self:ProcessResourceList(33, 1, self.dry_grass_flat_no_feature, resources_to_place)
		local resources_to_place = {
		{self.horse_ID, horse_amt, 100, 1, 4} };
		self:ProcessResourceList(33, 1, self.plains_flat_no_feature, resources_to_place)

		self:AddModernMinorStrategicsToCityStates() -- Added spring 2011
		
		self:PlaceSmallQuantitiesOfStrategics(23 * resMultiplier, self.land_list);
		
		self:PlaceOilInTheSea();
	end
	
	
	-- Place Bonus Resources
	--print("Placing Bonus Resources")
	
	
	
	print("Map Generation - Placing Bonuses");
	self:PlaceFish(10 * resMultiplier, self.coast_list);
	self:PlaceSexyBonusAtCivStarts()
	self:AddExtraBonusesToHillsRegions()
	
	self:PlaceBonusResources()
end
--]=]----------------------------------------------------------------------------
function AssignStartingPlots:PlaceStrategicAndBonusResources()
	print("PlaceStrategicAndBonusResources")
	-- KEY: {Resource ID, Quantity (0 = unquantified), weighting, minimum radius, maximum radius}
	-- KEY: (frequency (1 per n plots in the list), impact list number, plot list, resource data)
	--
	-- The radius creates a zone around the plot that other resources of that
	-- type will avoid if possible. See ProcessResourceList for impact numbers.
	--
	-- Order of placement matters, so changing the order may affect a later dependency.
	
	-- Adjust amounts, if applicable, based on Resource Setting.
	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = self:GetMajorStrategicResourceQuantityValues()
	local resources_to_place = {}

	-- Adjust appearance rate per Resource Setting chosen by user.
	local resMultiplier = 1;
	if self.resource_setting == 1 then -- Sparse, so increase the number of tiles per bonus.
		resMultiplier = 1.5;
	elseif self.resource_setting == 3 then -- Abundant, so reduce the number of tiles per bonus.
		resMultiplier = 0.66666667;
	end
	
	print("self.resource_setting = " .. self.resource_setting)
	
	-- Place Strategic resources.
	do
	print("Map Generation - Placing Strategics");
	
	resources_to_place = {
	{self.iron_ID, iron_amt, 10, 0, 2}, --26
	{self.coal_ID, coal_amt, 80, 1, 3}, -- 39
	{self.aluminum_ID, alum_amt, 10, 2, 3} }; -- 35
	self:ProcessResourceList(15, 1, self.hills_list, resources_to_place)
	-- 22

	resources_to_place = {
	{self.coal_ID, coal_amt, 50, 1, 2}, -- 30
	{self.uranium_ID, uran_amt, 50, 1, 2} }; --70
	self:ProcessResourceList(20, 1, self.jungle_flat_list, resources_to_place)
	-- 33
	
	resources_to_place = {
	{self.coal_ID, coal_amt, 80, 1, 2}, --70
	{self.uranium_ID, uran_amt, 20, 1, 1},  --30
	{self.iron_ID, iron_amt, 100, 0, 2} };
	self:ProcessResourceList(30, 1, self.forest_flat_list, resources_to_place)
	-- 39	
	
	resources_to_place = {
	{self.oil_ID, oil_amt, 65, 1, 1},
	{self.uranium_ID, uran_amt, 35, 0, 1} };
	self:ProcessResourceList(30, 1, self.jungle_flat_list, resources_to_place)
	-- 9
	
	resources_to_place = {
	{self.oil_ID, oil_amt, 45, 1, 2},
	{self.aluminum_ID, alum_amt, 45, 1, 2},
	{self.iron_ID, iron_amt, 10, 1, 2} };
	self:ProcessResourceList(16, 1, self.tundra_flat_no_feature, resources_to_place)
	-- 16
	
	resources_to_place = {
	{self.oil_ID, oil_amt, 20, 1, 1},
	{self.aluminum_ID, alum_amt, 20, 2, 3},
	{self.uranium_ID, alum_amt, 20, 2, 3},
	{self.coal_ID, alum_amt, 20, 2, 3},
	{self.iron_ID, iron_amt, 20, 2, 3} };
	self:ProcessResourceList(5, 1, self.snow_flat_list, resources_to_place)
	-- 17
	
	resources_to_place = {
	{self.oil_ID, oil_amt, 80, 0, 1},
	{self.iron_ID, iron_amt, 20, 1, 1} };
	self:ProcessResourceList(10, 1, self.desert_flat_no_feature, resources_to_place)
	-- 13

	resources_to_place = {
	{self.iron_ID, iron_amt, 100, 0, 2} };
	self:ProcessResourceList(10, 1, self.hills_jungle_list, resources_to_place)
	-- 99
	
--	resources_to_place = {
--	{self.iron_ID, iron_amt, 100, 0, 2} };
--	self:ProcessResourceList(5, 1, self.marsh_list, resources_to_place)
	-- 99

	resources_to_place = {
	{self.horse_ID, horse_amt, 100, 2, 5} };
	self:ProcessResourceList(33, 1, self.grass_flat_no_feature, resources_to_place)
	-- 33
	
	resources_to_place = {
	{self.horse_ID, horse_amt, 100, 1, 4} };
	self:ProcessResourceList(33, 1, self.plains_flat_no_feature, resources_to_place)
	-- 33
	
	resources_to_place = {
	{self.horse_ID, horse_amt, 100, 1, 4} };
	self:ProcessResourceList(10, 1, self.flood_plains_list, resources_to_place)
	end
	-- 33

	self:AddModernMinorStrategicsToCityStates() -- Added spring 2011
	
	self:PlaceSmallQuantitiesOfStrategics(23 * resMultiplier, self.land_list);
	
	self:PlaceOilInTheSea();

	
	-- Check for low or missing Strategic resources
	do
	if self.amounts_of_resources_placed[self.iron_ID + 1] < 8 then
		--print("Map has very low iron, adding another.");
		local resources_to_place = { {self.iron_ID, iron_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.hills_list, resources_to_place) -- 99999 means one per that many tiles: a single instance.
	end
	if self.amounts_of_resources_placed[self.iron_ID + 1] < 4 * self.iNumCivs then
		--print("Map has very low iron, adding another.");
		local resources_to_place = { {self.iron_ID, iron_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.land_list, resources_to_place)
	end
	if self.amounts_of_resources_placed[self.horse_ID + 1] < 4 * self.iNumCivs then
		--print("Map has very low horse, adding another.");
		local resources_to_place = { {self.horse_ID, horse_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.plains_flat_no_feature, resources_to_place)
	end
	if self.amounts_of_resources_placed[self.horse_ID + 1] < 4 * self.iNumCivs then
		--print("Map has very low horse, adding another.");
		local resources_to_place = { {self.horse_ID, horse_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.dry_grass_flat_no_feature, resources_to_place)
	end
	if self.amounts_of_resources_placed[self.coal_ID + 1] < 8 then
		--print("Map has very low coal, adding another.");
		local resources_to_place = { {self.coal_ID, coal_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.hills_list, resources_to_place)
	end
	if self.amounts_of_resources_placed[self.coal_ID + 1] < 4 * self.iNumCivs then
		--print("Map has very low coal, adding another.");
		local resources_to_place = { {self.coal_ID, coal_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.land_list, resources_to_place)
	end
	if self.amounts_of_resources_placed[self.oil_ID + 1] < 4 * self.iNumCivs then
		--print("Map has very low oil, adding another.");
		local resources_to_place = { {self.oil_ID, oil_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.land_list, resources_to_place)
	end
	if self.amounts_of_resources_placed[self.aluminum_ID + 1] < 4 * self.iNumCivs then
		--print("Map has very low aluminum, adding another.");
		local resources_to_place = { {self.aluminum_ID, alum_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.hills_list, resources_to_place)
	end
	if self.amounts_of_resources_placed[self.uranium_ID + 1] < 2 * self.iNumCivs then
		--print("Map has very low uranium, adding another.");
		local resources_to_place = { {self.uranium_ID, uran_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.land_list, resources_to_place)
	end
	end
	
	self:PlaceBonusResources()
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceFish()
	print("AssignStartingPlots:PlaceFish()")
	for plotID, plot in Plots(Shuffle) do
		PlacePossibleFish(plot)
	end
end
function PlacePossibleFish(plot)
	if plot:GetTerrainType() ~= TerrainTypes.TERRAIN_COAST or plot:IsLake() or plot:GetFeatureType() ~= FeatureTypes.NO_FEATURE or plot:GetResourceType() ~= -1 then
		return
	end
	local x, y			= plot:GetX(), plot:GetY()
	local landDistance	= 999
	local sumFertility	= 0
	local nearFish		= 0
	local odds			= 0
	local fishID		= GameInfo.Resources.RESOURCE_FISH.ID
	local fishMod		= 0
	
	for nearPlot, distance in Plot_GetPlotsInCircle(plot, 1, 3) do
		distance = math.max(1, distance)
		if not nearPlot:IsWater() and distance < landDistance then
			landDistance = distance
		end
		sumFertility = sumFertility + Plot_GetFertility(nearPlot, false, true)
		if nearPlot:GetResourceType() == fishID then
			odds = odds - 100 / distance
		end
	end
	if landDistance >= 3 then
		return
	end
	
	fishMod = odds
	odds = odds + 100 * (1 - sumFertility/(mg.fishTargetFertility * 2))
	odds = odds / landDistance
	
	if odds >= Map.Rand(100, "PlacePossibleFish - Lua") then
		plot:SetResourceType(fishID, 1)
		--print(string.format( "PlacePossibleFish fertility=%-3s odds=%-3s fishMod=%-3s", Round(sumFertility), Round(odds), Round(fishMod) ))
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceBonusResources()
	local resMultiplier = 1;
	if self.resource_setting == 1 then -- Sparse, so increase the number of tiles per bonus.
		resMultiplier = 1.5;
	elseif self.resource_setting == 3 then -- Abundant, so reduce the number of tiles per bonus.
		resMultiplier = 0.66666667;
	end
	
	-- Place Bonus Resources
	print("Map Generation - Placing Bonuses");
	self:PlaceFish(10 * resMultiplier, self.coast_list);
	self:PlaceSexyBonusAtCivStarts()
	self:AddExtraBonusesToHillsRegions()
	local resources_to_place = {}
	
	resources_to_place = {
	{self.deer_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(6 * resMultiplier, 3, self.extra_deer_list, resources_to_place)
	-- 8

	resources_to_place = {
	{self.deer_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(8 * resMultiplier, 3, self.tundra_flat_no_feature, resources_to_place)
	-- 12
	
	resources_to_place = {
	{self.wheat_ID, 1, 100, 0, 2} };
	self:ProcessResourceList(10 * resMultiplier, 3, self.desert_wheat_list, resources_to_place)
	-- 10

	resources_to_place = {
	{self.wheat_ID, 1, 100, 2, 3} };
	self:ProcessResourceList(10 * resMultiplier, 3, self.plains_flat_no_feature, resources_to_place)
	-- 27
	
	resources_to_place = {
	{self.banana_ID, 1, 100, 0, 3} };
	self:ProcessResourceList(14 * resMultiplier, 3, self.banana_list, resources_to_place)
	-- 14
	
	resources_to_place = {
	{self.cow_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(18 * resMultiplier, 3, self.grass_flat_no_feature, resources_to_place)
	-- 18
	
-- CBP
	resources_to_place = {
	{self.bison_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(15 * resMultiplier, 3, self.plains_flat_no_feature, resources_to_place)
-- END

	resources_to_place = {
	{self.sheep_ID, 1, 100, 1, 1} };
	self:ProcessResourceList(8 * resMultiplier, 3, self.hills_open_list, resources_to_place)
	-- 13

	resources_to_place = {
	{self.stone_ID, 1, 100, 1, 1} };
	self:ProcessResourceList(10 * resMultiplier, 3, self.grass_flat_no_feature, resources_to_place)
	-- 20
	
	resources_to_place = {
	{self.stone_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(15 * resMultiplier, 3, self.tundra_flat_no_feature, resources_to_place)
	-- 15
	
	resources_to_place = {
	{self.stone_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(5 * resMultiplier, 3, self.desert_flat_no_feature, resources_to_place)
	-- 19
	
	resources_to_place = {
	{self.stone_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(50 * resMultiplier, 3, self.marsh_list, resources_to_place)
	-- 99
	
	resources_to_place = {
	{self.stone_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(8 * resMultiplier, 3, self.snow_flat_list, resources_to_place)
	-- 99
	
	resources_to_place = {
	{self.deer_ID, 1, 100, 3, 4} };
	self:ProcessResourceList(25 * resMultiplier, 3, self.forest_flat_that_are_not_tundra, resources_to_place)
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
	--[[
	if GameInfo.Cep then
		self:PlaceStrategicAndBonusResourcesCEP()
	else
		self:PlaceStrategicAndBonusResources()
	end
	--]]
	self:PlaceStrategicAndBonusResources()
	
	--print("Map Generation - Normalize City State Locations");
	self:NormalizeCityStateLocations()
	
	-- Fix Sugar graphics
	self:AdjustTiles()
	
	local largestLand = Map.FindBiggestArea(false)
	if Map.GetCustomOption(6) == 2 then
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
function AssignStartingPlots:NormalizeStartLocation(region_number)
	--[[ This function measures the value of land in two rings around a given start
	     location, primarily for the purpose of determining how much support the site
	     requires in the form of Bonus Resources. Numerous assumptions are built in 
	     to this operation that would need to be adjusted for any modifications to 
	     terrain or resources types and yields, or to game rules about rivers and 
	     other map elements. Nothing is hardcoded in a way that puts it out of the 
	     reach of modders, but any mods including changes to map elements may have a
	     significant workload involved with rebalancing the start finder and the 
	     resource distribution to fit them properly to a mod's custom needs. I have
	     labored to document every function and method in detail to make it as easy
	     as possible to modify this system.  -- Bob Thomas - April 15, 2010  ]]--
	-- 
	local iW, iH = Map.GetGridSize();
	local start_point_data = self.startingPlots[region_number];
	local x = start_point_data[1];
	local y = start_point_data[2];
	local plot = Map.GetPlot(x, y);
	local plotIndex = y * iW + x + 1;
	local isEvenY = true;
	if y / 2 > math.floor(y / 2) then
		isEvenY = false;
	end
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local innerFourFood, innerThreeFood, innerTwoFood, innerHills, innerForest, innerOneHammer, innerOcean = 0, 0, 0, 0, 0, 0, 0;
	local outerFourFood, outerThreeFood, outerTwoFood, outerHills, outerForest, outerOneHammer, outerOcean = 0, 0, 0, 0, 0, 0, 0;
	local innerCanHaveBonus, outerCanHaveBonus, innerBadTiles, outerBadTiles = 0, 0, 0, 0;
	local iNumFoodBonusNeeded = 0;
	local iNumNativeTwoFoodFirstRing, iNumNativeTwoFoodSecondRing = 0, 0; -- Cities must begin the game with at least three native 2F tiles, one in first ring.
	local search_table = {};
	
	-- Remove any feature Ice from the first ring.
	self:GenerateLuxuryPlotListsAtCitySite(x, y, 1, true)
	
	-- Set up Conditions checks.
	local alongOcean = false;
	local nextToLake = false;
	local isRiver = false;
	local nearRiver = false;
	local nearMountain = false;
	local forestCount, jungleCount = 0, 0;

	-- Check start plot to see if it's adjacent to saltwater.
	if self.plotDataIsCoastal[plotIndex] == true then
		alongOcean = true;
	end
	
	-- Check start plot to see if it's on a river.
	if plot:IsRiver() then
		isRiver = true;
	end

	-- Data Chart for early game tile potentials
	--
	-- 4F:	Flood Plains, Grass on fresh water (includes forest and marsh).
	-- 3F:	Dry Grass, Plains on fresh water (includes forest and jungle), Tundra on fresh water (includes forest), Oasis
	-- 2F:  Dry Plains, Lake, all remaining Jungles.
	--
	-- 1H:	Plains, Jungle on Plains

	-- Adding evaluation of grassland and plains for balance boost of bonus Cows for heavy grass starts. -1/26/2011 BT
	local iNumGrass, iNumPlains = 0, 0;

	-- Evaluate First Ring
	if isEvenY then
		search_table = self.firstRingYIsEven;
	else
		search_table = self.firstRingYIsOdd;
	end

	for loop, plot_adjustments in ipairs(search_table) do
		local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)
		--
		if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then
			-- This plot does not exist. It's off the map edge.
			innerBadTiles = innerBadTiles + 1;
		else
			local searchPlot = Map.GetPlot(searchX, searchY)
			local plotType = searchPlot:GetPlotType()
			local terrainType = searchPlot:GetTerrainType()
			local featureType = searchPlot:GetFeatureType()
			--
			if plotType == PlotTypes.PLOT_MOUNTAIN then
				local nearMountain = true;
				innerBadTiles = innerBadTiles + 1;
			elseif plotType == PlotTypes.PLOT_OCEAN then
				if searchPlot:IsLake() then
					nextToLake = true;
					if featureType == FeatureTypes.FEATURE_ICE then
						innerBadTiles = innerBadTiles + 1;
					else
						innerTwoFood = innerTwoFood + 1;
						iNumNativeTwoFoodFirstRing = iNumNativeTwoFoodFirstRing + 1;
					end
				else
					if featureType == FeatureTypes.FEATURE_ICE then
						innerBadTiles = innerBadTiles + 1;
					else
						innerOcean = innerOcean + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
					end
				end
			else -- Habitable plot.
				if featureType == FeatureTypes.FEATURE_JUNGLE then
					jungleCount = jungleCount + 1;
					iNumNativeTwoFoodFirstRing = iNumNativeTwoFoodFirstRing + 1;
				elseif featureType == FeatureTypes.FEATURE_FOREST then
					forestCount = forestCount + 1;
				end
				if searchPlot:IsRiver() then
					nearRiver = true;
				end
				if plotType == PlotTypes.PLOT_HILLS then
					innerHills = innerHills + 1;
					if featureType == FeatureTypes.FEATURE_JUNGLE then
						innerTwoFood = innerTwoFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
					elseif featureType == FeatureTypes.FEATURE_FOREST then
						innerCanHaveBonus = innerCanHaveBonus + 1;
					elseif terrainType == TerrainTypes.TERRAIN_GRASS then
						iNumGrass = iNumGrass + 1;
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						iNumPlains = iNumPlains + 1;
					end
				elseif featureType == FeatureTypes.FEATURE_OASIS then
					innerThreeFood = innerThreeFood + 1;
					iNumNativeTwoFoodFirstRing = iNumNativeTwoFoodFirstRing + 1;
				elseif searchPlot:IsFreshWater() then
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						innerFourFood = innerFourFood + 1;
						iNumGrass = iNumGrass + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then
							innerCanHaveBonus = innerCanHaveBonus + 1;
						end
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						end
						if featureType == FeatureTypes.NO_FEATURE then
							iNumNativeTwoFoodFirstRing = iNumNativeTwoFoodFirstRing + 1;
						end
					elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
						innerFourFood = innerFourFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
						iNumNativeTwoFoodFirstRing = iNumNativeTwoFoodFirstRing + 1;
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						innerThreeFood = innerThreeFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
						iNumPlains = iNumPlains + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						else
							innerOneHammer = innerOneHammer + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
						innerThreeFood = innerThreeFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						innerBadTiles = innerBadTiles + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1; -- Can have Oasis.
					else -- Snow
						innerBadTiles = innerBadTiles + 1;
					end
				else -- Dry Flatlands
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						innerThreeFood = innerThreeFood + 1;
						iNumGrass = iNumGrass + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then
							innerCanHaveBonus = innerCanHaveBonus + 1;
						end
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						end
						if featureType == FeatureTypes.NO_FEATURE then
							iNumNativeTwoFoodFirstRing = iNumNativeTwoFoodFirstRing + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						innerTwoFood = innerTwoFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
						iNumPlains = iNumPlains + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						else
							innerOneHammer = innerOneHammer + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
						innerCanHaveBonus = innerCanHaveBonus + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						else
							innerBadTiles = innerBadTiles + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						innerBadTiles = innerBadTiles + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1; -- Can have Oasis.
					else -- Snow
						innerBadTiles = innerBadTiles + 1;
					end
				end
			end
		end
	end
				
	-- Evaluate Second Ring
	if isEvenY then
		search_table = self.secondRingYIsEven;
	else
		search_table = self.secondRingYIsOdd;
	end

	for loop, plot_adjustments in ipairs(search_table) do
		local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)
		local plot = Map.GetPlot(x, y);
		--
		--
		if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then
			-- This plot does not exist. It's off the map edge.
			outerBadTiles = outerBadTiles + 1;
		else
			local searchPlot = Map.GetPlot(searchX, searchY)
			local plotType = searchPlot:GetPlotType()
			local terrainType = searchPlot:GetTerrainType()
			local featureType = searchPlot:GetFeatureType()
			--
			if plotType == PlotTypes.PLOT_MOUNTAIN then
				local nearMountain = true;
				outerBadTiles = outerBadTiles + 1;
			elseif plotType == PlotTypes.PLOT_OCEAN then
				if searchPlot:IsLake() then
					if featureType == FeatureTypes.FEATURE_ICE then
						outerBadTiles = outerBadTiles + 1;
					else
						outerTwoFood = outerTwoFood + 1;
						iNumNativeTwoFoodSecondRing = iNumNativeTwoFoodSecondRing + 1;
					end
				else
					if featureType == FeatureTypes.FEATURE_ICE then
						outerBadTiles = outerBadTiles + 1;
					elseif terrainType == TerrainTypes.TERRAIN_COAST then
						outerCanHaveBonus = outerCanHaveBonus + 1;
						outerOcean = outerOcean + 1;
					end
				end
			else -- Habitable plot.
				if featureType == FeatureTypes.FEATURE_JUNGLE then
					jungleCount = jungleCount + 1;
					iNumNativeTwoFoodSecondRing = iNumNativeTwoFoodSecondRing + 1;
				elseif featureType == FeatureTypes.FEATURE_FOREST then
					forestCount = forestCount + 1;
				end
				if searchPlot:IsRiver() then
					nearRiver = true;
				end
				if plotType == PlotTypes.PLOT_HILLS then
					outerHills = outerHills + 1;
					if featureType == FeatureTypes.FEATURE_JUNGLE then
						outerTwoFood = outerTwoFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
					elseif featureType == FeatureTypes.FEATURE_FOREST then
						outerCanHaveBonus = outerCanHaveBonus + 1;
					elseif terrainType == TerrainTypes.TERRAIN_GRASS then
						iNumGrass = iNumGrass + 1;
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						iNumPlains = iNumPlains + 1;
					end
				elseif featureType == FeatureTypes.FEATURE_OASIS then
					innerThreeFood = innerThreeFood + 1;
					iNumNativeTwoFoodSecondRing = iNumNativeTwoFoodSecondRing + 1;
				elseif searchPlot:IsFreshWater() then
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						outerFourFood = outerFourFood + 1;
						iNumGrass = iNumGrass + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then
							outerCanHaveBonus = outerCanHaveBonus + 1;
						end
						if featureType == FeatureTypes.FEATURE_FOREST then
							outerForest = outerForest + 1;
						end
						if featureType == FeatureTypes.NO_FEATURE then
							iNumNativeTwoFoodSecondRing = iNumNativeTwoFoodSecondRing + 1;
						end
					elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
						outerFourFood = outerFourFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
						iNumNativeTwoFoodSecondRing = iNumNativeTwoFoodSecondRing + 1;
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						outerThreeFood = outerThreeFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
						iNumPlains = iNumPlains + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							outerForest = outerForest + 1;
						else
							outerOneHammer = outerOneHammer + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
						outerThreeFood = outerThreeFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							outerForest = outerForest + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						outerBadTiles = outerBadTiles + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1; -- Can have Oasis.
					else -- Snow
						outerBadTiles = outerBadTiles + 1;
					end
				else -- Dry Flatlands
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						outerThreeFood = outerThreeFood + 1;
						iNumGrass = iNumGrass + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then
							outerCanHaveBonus = outerCanHaveBonus + 1;
						end
						if featureType == FeatureTypes.FEATURE_FOREST then
							outerForest = outerForest + 1;
						end
						if featureType == FeatureTypes.NO_FEATURE then
							iNumNativeTwoFoodSecondRing = iNumNativeTwoFoodSecondRing + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						outerTwoFood = outerTwoFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
						iNumPlains = iNumPlains + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							outerForest = outerForest + 1;
						else
							outerOneHammer = outerOneHammer + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
						outerCanHaveBonus = outerCanHaveBonus + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							outerForest = outerForest + 1;
						else
							outerBadTiles = outerBadTiles + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						outerBadTiles = outerBadTiles + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1; -- Can have Oasis.
					else -- Snow
						outerBadTiles = outerBadTiles + 1;
					end
				end
			end
		end
	end
	
	-- Adjust the hammer situation, if needed.
	local innerHammerScore = (4 * innerHills) + (2 * innerForest) + innerOneHammer;
	local outerHammerScore = (2 * outerHills) + outerForest + outerOneHammer;
	local earlyHammerScore = (2 * innerForest) + outerForest + innerOneHammer + outerOneHammer;
	-- If drastic shortage, attempt to add a hill to first ring.
	if (outerHammerScore < 8 and innerHammerScore < 2) or innerHammerScore == 0 then -- Change a first ring plot to Hills.
		if isEvenY then
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsEven);
		else
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsOdd);
		end
		for attempt = 1, 6 do
			local plot_adjustments = randomized_first_ring_adjustments[attempt];
			local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)
			-- Attempt to place a Hill at the currently chosen plot.
			local placedHill = self:AttemptToPlaceHillsAtPlot(searchX, searchY);
			if placedHill == true then
				innerHammerScore = innerHammerScore + 4;
				--print("Added hills next to hammer-poor start plot at ", x, y);
				break
			elseif attempt == 6 then
				--print("FAILED to add hills next to hammer-poor start plot at ", x, y);
			end
		end
	end
	
	-- Add mandatory Iron, Horse, Oil to every start if Strategic Balance option is enabled.
	if self.resource_setting == 5 then
		self:AddStrategicBalanceResources(region_number)
	end
	
	-- If early hammers will be too short, attempt to add a small Horse or Iron to second ring.
	if innerHammerScore < 3 and earlyHammerScore < 6 then -- Add a small Horse or Iron to second ring.
		if isEvenY then
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsEven);
		else
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsOdd);
		end
		for attempt = 1, 12 do
			local plot_adjustments = randomized_second_ring_adjustments[attempt];
			local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)
			-- Attempt to place a Hill at the currently chosen plot.
			local placedStrategic = self:AttemptToPlaceSmallStrategicAtPlot(searchX, searchY);
			if placedStrategic == true then
				break
			elseif attempt == 12 then
				--print("FAILED to add small strategic resource near hammer-poor start plot at ", x, y);
			end
		end
	end
	
	-- Rate the food situation.
	local innerFoodScore = (4 * innerFourFood) + (2 * innerThreeFood) + innerTwoFood;
	local outerFoodScore = (4 * outerFourFood) + (2 * outerThreeFood) + outerTwoFood;
	local totalFoodScore = innerFoodScore + outerFoodScore;
	local nativeTwoFoodTiles = iNumNativeTwoFoodFirstRing + iNumNativeTwoFoodSecondRing;

	--[[ Debug printout of food scores.
	print("-");
	print("-- - Start Point in Region #", region_number, " has Food Score of ", totalFoodScore, " with rings of ", innerFoodScore, outerFoodScore);
	]]--	
	
	-- Six levels for Bonus Resource support, from zero to five.
	if totalFoodScore < 4 and innerFoodScore == 0 then
		iNumFoodBonusNeeded = 5;
	elseif totalFoodScore < 6 then
		iNumFoodBonusNeeded = 4;
	elseif totalFoodScore < 8 then
		iNumFoodBonusNeeded = 3;
	elseif totalFoodScore < 12 and innerFoodScore < 5 then
		iNumFoodBonusNeeded = 3;
	elseif totalFoodScore < 17 and innerFoodScore < 9 then
		iNumFoodBonusNeeded = 2;
	elseif nativeTwoFoodTiles <= 1 then
		iNumFoodBonusNeeded = 2;
	elseif totalFoodScore < 24 and innerFoodScore < 11 then
		iNumFoodBonusNeeded = 1;
	elseif nativeTwoFoodTiles == 2 or iNumNativeTwoFoodFirstRing == 0 then
		iNumFoodBonusNeeded = 1;
	elseif totalFoodScore < 20 then
		iNumFoodBonusNeeded = 1;
	end
	
	-- Check for Legendary Start resource option.
	if self.resource_setting == 4 then
		iNumFoodBonusNeeded = iNumFoodBonusNeeded + 2;
	end
	
	-- Check to see if a Grass tile needs to be added at an all-plains site with zero native 2-food tiles in first two rings.
	if nativeTwoFoodTiles == 0 and iNumFoodBonusNeeded < 3 then
		local odd = self.firstRingYIsOdd;
		local even = self.firstRingYIsEven;
		local plot_list = {};
		-- For notes on how the hex-iteration works, refer to PlaceResourceImpact()
		local ripple_radius = 2;
		local currentX = x - ripple_radius;
		local currentY = y;
		for direction_index = 1, 6 do
			for plot_to_handle = 1, ripple_radius do
			 	if currentY / 2 > math.floor(currentY / 2) then
					plot_adjustments = odd[direction_index];
				else
					plot_adjustments = even[direction_index];
				end
				nextX = currentX + plot_adjustments[1];
				nextY = currentY + plot_adjustments[2];
				if wrapX == false and (nextX < 0 or nextX >= iW) then
					-- X is out of bounds.
				elseif wrapY == false and (nextY < 0 or nextY >= iH) then
					-- Y is out of bounds.
				else
					local realX = nextX;
					local realY = nextY;
					if wrapX then
						realX = realX % iW;
					end
					if wrapY then
						realY = realY % iH;
					end
					-- We've arrived at the correct x and y for the current plot.
					local plot = Map.GetPlot(realX, realY);
					if plot:GetResourceType(-1) == -1 then -- No resource here, safe to proceed.
						local plotType = plot:GetPlotType()
						local terrainType = plot:GetTerrainType()
						local featureType = plot:GetFeatureType()
						local plotIndex = realY * iW + realX + 1;
						-- Now check this plot for eligibility to be converted to flat open grassland.
						if plotType == PlotTypes.PLOT_LAND then
							if terrainType == TerrainTypes.TERRAIN_PLAINS then
								if featureType == FeatureTypes.NO_FEATURE then
									table.insert(plot_list, plotIndex);
								end
							end
						end
					end
				end
				currentX, currentY = nextX, nextY;
			end
		end
		local iNumConversionCandidates = table.maxn(plot_list);
		if iNumConversionCandidates == 0 then
			iNumFoodBonusNeeded = 3;
		else
			--print("-"); print("*** START HAD NO 2-FOOD TILES, YET ONLY QUALIFIED FOR 2 BONUS; CONVERTING A PLAINS TO GRASS! ***"); print("-");
			local diceroll = 1 + Map.Rand(iNumConversionCandidates, "Choosing plot to convert to Grass near food-poor Plains start - LUA");
			local conversionPlotIndex = plot_list[diceroll];
			local conv_x = (conversionPlotIndex - 1) % iW;
			local conv_y = (conversionPlotIndex - conv_x - 1) / iW;
			local plot = Map.GetPlot(conv_x, conv_y);
			plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, false)
			self:PlaceResourceImpact(conv_x, conv_y, 1, 0) -- Disallow strategic resources at this plot, to keep it a farm plot.
		end
	end
	-- Add Bonus Resources to food-poor start positions.
	if iNumFoodBonusNeeded > 0 then
		local maxBonusesPossible = innerCanHaveBonus + outerCanHaveBonus;

		--print("-");
		--print("Food-Poor start ", x, y, " needs ", iNumFoodBonusNeeded, " Bonus, with ", maxBonusesPossible, " eligible plots.");
		--print("-");

		local innerPlaced, outerPlaced = 0, 0;
		local randomized_first_ring_adjustments, randomized_second_ring_adjustments, randomized_third_ring_adjustments;
		if isEvenY then
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsEven);
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsEven);
			randomized_third_ring_adjustments = GetShuffledCopyOfTable(self.thirdRingYIsEven);
		else
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsOdd);
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsOdd);
			randomized_third_ring_adjustments = GetShuffledCopyOfTable(self.thirdRingYIsOdd);
		end
		local tried_all_first_ring = false;
		local tried_all_second_ring = false;
		local tried_all_third_ring = false;
		local allow_oasis = true; -- Permanent flag. (We don't want to place more than one Oasis per location).
		local placedOasis; -- Records returning result from each attempt.
		while iNumFoodBonusNeeded > 0 do
			if ((innerPlaced < 2 and innerCanHaveBonus > 0) or (self.resource_setting == 4 and innerPlaced < 3 and innerCanHaveBonus > 0))
			  and tried_all_first_ring == false then
				-- Add bonus to inner ring.
				for attempt = 1, 6 do
					local plot_adjustments = randomized_first_ring_adjustments[attempt];
					local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)
					-- Attempt to place a Bonus at the currently chosen plot.
					local placedBonus, placedOasis = self:AttemptToPlaceBonusResourceAtPlot(searchX, searchY, allow_oasis);
					if placedBonus == true then
						if allow_oasis == true and placedOasis == true then -- First oasis was placed on this pass, so change permission.
							allow_oasis = false;
						end
						--print("Placed a Bonus in first ring at ", searchX, searchY);
						innerPlaced = innerPlaced + 1;
						innerCanHaveBonus = innerCanHaveBonus - 1;
						iNumFoodBonusNeeded = iNumFoodBonusNeeded - 1;
						break
					elseif attempt == 6 then
						tried_all_first_ring = true;
					end
				end

			elseif ((innerPlaced + outerPlaced < 5 and outerCanHaveBonus > 0) or (self.resource_setting == 4 and innerPlaced + outerPlaced < 4 and outerCanHaveBonus > 0))
			  and tried_all_second_ring == false then
				-- Add bonus to second ring.
				for attempt = 1, 12 do
					local plot_adjustments = randomized_second_ring_adjustments[attempt];
					local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)
					-- Attempt to place a Bonus at the currently chosen plot.
					local placedBonus, placedOasis = self:AttemptToPlaceBonusResourceAtPlot(searchX, searchY, allow_oasis);
					if placedBonus == true then
						if allow_oasis == true and placedOasis == true then -- First oasis was placed on this pass, so change permission.
							allow_oasis = false;
						end
						--print("Placed a Bonus in second ring at ", searchX, searchY);
						outerPlaced = outerPlaced + 1;
						outerCanHaveBonus = outerCanHaveBonus - 1;
						iNumFoodBonusNeeded = iNumFoodBonusNeeded - 1;
						break
					elseif attempt == 12 then
						tried_all_second_ring = true;
					end
				end

			elseif tried_all_third_ring == false then
				-- Add bonus to third ring.
				for attempt = 1, 18 do
					local plot_adjustments = randomized_third_ring_adjustments[attempt];
					local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)
					-- Attempt to place a Bonus at the currently chosen plot.
					local placedBonus, placedOasis = self:AttemptToPlaceBonusResourceAtPlot(searchX, searchY, allow_oasis);
					if placedBonus == true then
						if allow_oasis == true and placedOasis == true then -- First oasis was placed on this pass, so change permission.
							allow_oasis = false;
						end
						--print("Placed a Bonus in third ring at ", searchX, searchY);
						iNumFoodBonusNeeded = iNumFoodBonusNeeded - 1;
						break
					elseif attempt == 18 then
						tried_all_third_ring = true;
					end
				end
				
			else -- Tried everywhere, have to give up.
				break				
			end
		end
	end

	-- Check for heavy grass and light plains. Adding Stone if grass count is high and plains count is low. - May 2011, BT
	local iNumStoneNeeded = 0;
	if iNumGrass >= 9 and iNumPlains == 0 then
		iNumStoneNeeded = 2;
	elseif iNumGrass >= 6 and iNumPlains <= 4 then
		iNumStoneNeeded = 1;
	end
	if iNumStoneNeeded > 0 then -- Add Stone to this grass start.
		local stonePlaced, innerPlaced = 0, 0;
		local randomized_first_ring_adjustments, randomized_second_ring_adjustments;
		if isEvenY then
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsEven);
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsEven);
		else
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsOdd);
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsOdd);
		end
		local tried_all_first_ring = false;
		local tried_all_second_ring = false;
		while iNumStoneNeeded > 0 do
			if innerPlaced < 1 and tried_all_first_ring == false then
				-- Add bonus to inner ring.
				for attempt = 1, 6 do
					local plot_adjustments = randomized_first_ring_adjustments[attempt];
					local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)
					-- Attempt to place Cows at the currently chosen plot.
					local placedBonus = self:AttemptToPlaceStoneAtGrassPlot(searchX, searchY);
					if placedBonus == true then
						--print("Placed Stone in first ring at ", searchX, searchY);
						innerPlaced = innerPlaced + 1;
						iNumStoneNeeded = iNumStoneNeeded - 1;
						break
					elseif attempt == 6 then
						tried_all_first_ring = true;
					end
				end

			elseif tried_all_second_ring == false then
				-- Add bonus to second ring.
				for attempt = 1, 12 do
					local plot_adjustments = randomized_second_ring_adjustments[attempt];
					local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)
					-- Attempt to place Stone at the currently chosen plot.
					local placedBonus = self:AttemptToPlaceStoneAtGrassPlot(searchX, searchY);
					if placedBonus == true then
						--print("Placed Stone in second ring at ", searchX, searchY);
						iNumStoneNeeded = iNumStoneNeeded - 1;
						break
					elseif attempt == 12 then
						tried_all_second_ring = true;
					end
				end

			else -- Tried everywhere, have to give up.
				break				
			end
		end
	end
	
	-- Record conditions at this start location.
	local results_table = {alongOcean, nextToLake, isRiver, nearRiver, nearMountain, forestCount, jungleCount};
	self.startLocationConditions[region_number] = results_table;
end
------------------------------------------------------------------------------
function AssignStartingPlots:BalanceAndAssign()
	-- This function determines what level of Bonus Resource support a location
	-- may need, identifies compatibility with civ-specific biases, and places starts.

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
		local playerListShuffled = GetShuffledCopyOfTable(playerList)
		for region_number, player_ID in ipairs(playerListShuffled) do
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];
			local start_plot = Map.GetPlot(x, y)
			local player = Players[player_ID]
			player:SetStartingPlot(start_plot)
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
	for loop = 1, self.iNumCivs do
		table.insert(all_regions, loop);
	end
	local regions_still_available = GetShuffledCopyOfTable(all_regions)

	local civs_needing_coastal_start = {};
	local civs_priority_coastal_start = {};
	local civs_needing_river_start = {};
	local civs_needing_region_priority = {};
	local civs_needing_region_avoid = {};
	local regions_with_coastal_start = {};
	local regions_with_lake_start = {};
	local regions_with_river_start = {};
	local regions_with_near_river_start = {};
	local civ_status = table.fill(false, GameDefines.MAX_MAJOR_CIVS); -- Have to account for possible gaps in player ID numbers, for MP.
	local region_status = table.fill(false, self.iNumCivs);
	local priority_lists = {};
	local avoid_lists = {};
	local iNumCoastalCivs, iNumRiverCivs, iNumPriorityCivs, iNumAvoidCivs = 0, 0, 0, 0;
	local iNumCoastalCivsRemaining, iNumRiverCivsRemaining, iNumPriorityCivsRemaining, iNumAvoidCivsRemaining = 0, 0, 0, 0;
	
	--print("-"); print("-"); print("--- DEBUG READOUT OF PLAYER START ASSIGNMENTS ---"); print("-");
	
	-- Generate lists of player needs. Each additional need type is subordinate to those
	-- that come before. In other words, each Civ can have only one need type.
	for loop = 1, self.iNumCivs do
		local playerNum = self.player_ID_list[loop]; -- MP games can have gaps between player numbers, so we cannot assume a sequential set of IDs.
		local player = Players[playerNum];
		local civType = GameInfo.Civilizations[player:GetCivilizationType()].Type;
		--print("Player", playerNum, "of Civ Type", civType);
		local bNeedsCoastalStart = CivNeedsCoastalStart(civType)
		if bNeedsCoastalStart == true then
			--print("- - - - - - - needs Coastal Start!"); print("-");
			iNumCoastalCivs = iNumCoastalCivs + 1;
			iNumCoastalCivsRemaining = iNumCoastalCivsRemaining + 1;
			table.insert(civs_needing_coastal_start, playerNum);
			if CivNeedsPlaceFirstCoastalStart then
				local bPlaceFirst = CivNeedsPlaceFirstCoastalStart(civType);
				if bPlaceFirst then
					--print("- - - - - - - needs to Place First!"); --print("-");
					table.insert(civs_priority_coastal_start, playerNum);
				end
			end
		else
			local bNeedsRiverStart = CivNeedsRiverStart(civType)
			if bNeedsRiverStart == true then
				--print("- - - - - - - needs River Start!"); print("-");
				iNumRiverCivs = iNumRiverCivs + 1;
				iNumRiverCivsRemaining = iNumRiverCivsRemaining + 1;
				table.insert(civs_needing_river_start, playerNum);
			else
				local iNumRegionPriority = GetNumStartRegionPriorityForCiv(civType)
				if iNumRegionPriority > 0 then
					--print("- - - - - - - needs Region Priority!"); print("-");
					local table_of_this_civs_priority_needs = GetStartRegionPriorityListForCiv_GetIDs(civType)
					iNumPriorityCivs = iNumPriorityCivs + 1;
					iNumPriorityCivsRemaining = iNumPriorityCivsRemaining + 1;
					table.insert(civs_needing_region_priority, playerNum);
					priority_lists[playerNum] = table_of_this_civs_priority_needs;
				else
					local iNumRegionAvoid = GetNumStartRegionAvoidForCiv(civType)
					if iNumRegionAvoid > 0 then
						--print("- - - - - - - needs Region Avoid!"); print("-");
						local table_of_this_civs_avoid_needs = GetStartRegionAvoidListForCiv_GetIDs(civType)
						iNumAvoidCivs = iNumAvoidCivs + 1;
						iNumAvoidCivsRemaining = iNumAvoidCivsRemaining + 1;
						table.insert(civs_needing_region_avoid, playerNum);
						avoid_lists[playerNum] = table_of_this_civs_avoid_needs;
					end
				end
			end
		end
	end
	
	--[[ Debug printout
	print("Civs with Coastal Bias:", iNumCoastalCivs);
	print("Civs with River Bias:", iNumRiverCivs);
	print("Civs with Region Priority:", iNumPriorityCivs);
	print("Civs with Region Avoid:", iNumAvoidCivs); print("-");
	--]]
	
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
			
			for loop, iPlayerNum in ipairs(shuffled_coastal_civs) do
				--print("shuffled_coastal_civs[" .. loop .. "]: " .. iPlayerNum);
			end
			
			local shuffled_coastal_civs = GetShuffledCopyOfTable(civs_needing_coastal_start);
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
					print("Ran out of Coastal and Lake start locations to assign to Coastal Bias.");
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
					print("Ran out of River and Near-River start locations to assign to River Bias.");
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
						print("Ran out of River and Near-River start locations to assign as fallbacks for Coastal Bias.");
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
			for region_type = 1, 8 do							-- Must expand if new region types are added.
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
				
	-- Assign remaining civs to start plots.
	local playerList, regionList = {}, {};
	for loop = 1, self.iNumCivs do
		local player_ID = self.player_ID_list[loop];
		if civ_status[player_ID + 1] == false then -- Using C++ player ID, which starts at zero. Add 1 for Lua indexing.
			table.insert(playerList, player_ID);
		end
		if region_status[loop] == false then
			table.insert(regionList, loop);
		end
	end
	local iNumRemainingPlayers = table.maxn(playerList);
	local iNumRemainingRegions = table.maxn(regionList);
	if iNumRemainingPlayers > 0 or iNumRemainingRegions > 0 then
		--print("-"); print("Table of players with no start bias:");
		--PrintContentsOfTable(playerList);
		--print("-"); print("Table of regions still available after bias handling:");
		--PrintContentsOfTable(regionList);
		if iNumRemainingPlayers ~= iNumRemainingRegions then
			print("-"); print("ERROR: Number of civs remaining after handling biases does not match number of regions remaining!"); print("-");
		end
		local playerListShuffled = GetShuffledCopyOfTable(playerList)
		for index, player_ID in ipairs(playerListShuffled) do
			local region_number = regionList[index];
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];
			--print("Now placing Player#", player_ID, "in Region#", region_number, "at start plot:", x, y);
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
	--	
end
------------------------------------------------------------------------------
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
  if (pPlot == nil) then return "" end

  return string.format("(%d, %d)", pPlot:GetX(), pPlot:GetY())
end

-- Get the hex-based (x, y, z) co-ordinates of the plot as a string
function plotToHexStr(pPlot)
  if (pPlot == nil) then return "" end

  local hex = PlotToHex(pPlot)

  return string.format("(%d, %d, %d)", hex.x, hex.y, hex.z)
end
