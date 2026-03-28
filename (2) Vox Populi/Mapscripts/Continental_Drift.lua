--[[--------------------------------------------------------------------------

-- Continental Drift map script --

Based on 'Tectonic' by Haftetavenscrap, James Barrows and N.Core
  Credits:
- Haftetavenscrap		- Original creator of the map script
  (Jibbles on Steam)
- James Barrows			- A (very) small part of the code is based on Small Continents Deluxe (v 100)
- N.Core				- Added map ratio selector, size modifier, rift width option, arctic ocean width option,
						  start placement method, more options for existing custom options for gradual adjustment,
						  balancing the map to be kinder to smaller sizes by generate more land and fewer islands,
						  and the opposite for larger sizes.
- axatin				- Rewrote plate type assignment algorithm to make the amount of land less random. Added the
						  option to have two meridional ocean rifts. Reduced map sizes, sizes are now closer to the
						  standard map sizes. Fixed size modifier being applied to both axes ('120%' size mod would
						  increase the map size by 144%). Added option 'Land Cohesion', influencing how plates are
						  clustered. Added custom code for atoll and lake generation (lake code based on 'Random
						  Scandinavia'). Reduced amount of land in polar regions. Additional minor tweaks. 

Generates a varied map based on global tectonics.
Maps are similar to a combination of Fractal, Small Continents, Archipelago, with added tectonic elements like mountain belts and rift zones.


--]]
------------------------------------------------------------------------------
include("MapGenerator");
include("FractalWorld");
include("FeatureGenerator");
include("TerrainGenerator");

local versionName = "3.1"
local L = Locale.ConvertTextKey;
------------------------------------------------------------------------------

function GetMapScriptInfo()
	local	world_age,
			temperature,
			rainfall,
			sea_level,
			resources,
			islands,
			plate_motion,
			continents,
			land,
			plates_set,
			map_ratio,
			size_mod,
			ocean_rifts,
			arctic_width,
			coastalStarts,
			startPlacement = GetCoreMapOptions()
	return {
		Name = L("TXT_KEY_CONTINENTAL_DRIFT") .. " - v" .. versionName,
		Description = "TXT_KEY_CONTINENTAL_DRIFT_HELP",
		IsAdvancedMap = 0,
		IconIndex = 5,
		SortIndex = 1,
		CustomOptions = {
			world_age,
			temperature,
			rainfall,
			sea_level,
			resources,
			islands,
			plate_motion,
			continents,
			land,
			plates_set,
			map_ratio,
			size_mod,
			ocean_rifts,
			arctic_width,
			coastalStarts,
			startPlacement
		},
	};
end

function GetCoreMapOptions()
	--[[ All options have a default SortPriority of 0. Lower values will be shown above
	higher values. Negative integers are valid. So the Core Map Options, which should 
	always be at the top of the list, are getting negative values from -99 to -95. Note
	that any set of options with identical SortPriority will be sorted alphabetically. ]]--
	local map_ratio = {
		Name = "TXT_KEY_MAP_OPTION_MAP_RATIO",
		Values = {
			"TXT_KEY_MAP_OPTION_SQUARED",
			"TXT_KEY_MAP_OPTION_SQUARISH",
			"TXT_KEY_MAP_OPTION_RECTANGULAR",
			"TXT_KEY_MAP_OPTION_WIDE_RECTANGULAR",
			"TXT_KEY_MAP_OPTION_RECTILINEAR",
			"TXT_KEY_MAP_OPTION_RANDOM",
		},
		DefaultValue = 3, --3,
		SortPriority = -99,
	};
	local size_mod = {
		Name = "TXT_KEY_MAP_OPTION_SIZE_MODIFIER",
		Description = "TXT_KEY_MAP_OPTION_SIZE_MODIFIER_HELP",
		Values = {
			"TXT_KEY_MAP_OPTION_SIZE_80",
			"TXT_KEY_MAP_OPTION_SIZE_85",
			"TXT_KEY_MAP_OPTION_SIZE_90",
			"TXT_KEY_MAP_OPTION_SIZE_95",
			"TXT_KEY_MAP_OPTION_SIZE_100",
			"TXT_KEY_MAP_OPTION_SIZE_105",
			"TXT_KEY_MAP_OPTION_SIZE_110",
			"TXT_KEY_MAP_OPTION_SIZE_115",
			"TXT_KEY_MAP_OPTION_SIZE_120",
			"TXT_KEY_MAP_OPTION_RANDOM",
		},
		DefaultValue = 	5, --5,
		SortPriority = -98,
	};
	local temperature = {
		Name = "TXT_KEY_MAP_OPTION_TEMPERATURE",
		Values = {
			"TXT_KEY_MAP_OPTION_COOL",
			"TXT_KEY_MAP_OPTION_TEMPERATE",
			"TXT_KEY_MAP_OPTION_HOT",
			"TXT_KEY_MAP_OPTION_RANDOM",
		},
		DefaultValue = 2, --2,
		SortPriority = -97,
	};
	local rainfall = {
		Name = "TXT_KEY_MAP_OPTION_RAINFALL",
		Values = {
			"TXT_KEY_MAP_OPTION_ARID",
			"TXT_KEY_MAP_OPTION_NORMAL",
			"TXT_KEY_MAP_OPTION_WET",
			"TXT_KEY_MAP_OPTION_RANDOM",
		},
		DefaultValue = 2, --2,
		SortPriority = -96,
	};
	local sea_level = {
		Name = "TXT_KEY_MAP_OPTION_SEA_LEVEL",
		Values = {
			"TXT_KEY_MAP_OPTION_SHALLOW",
			"TXT_KEY_MAP_OPTION_LOW",
			"TXT_KEY_MAP_OPTION_MEDIUM",
			"TXT_KEY_MAP_OPTION_HIGH",
			"TXT_KEY_MAP_OPTION_EXTREME",
			"TXT_KEY_MAP_OPTION_RANDOM",
		},
		DefaultValue = 3, --3,
		SortPriority = -95,
	};
	local ocean_rifts = {
		Name = "TXT_KEY_MAP_OPTION_OCEAN_RIFTS",
		Description = "TXT_KEY_MAP_OPTION_OCEAN_RIFTS_HELP",
		Values = {
			"TXT_KEY_MAP_NO",
			"TXT_KEY_MAP_YES",
			"TXT_KEY_MAP_OPTION_RANDOM",
		},
		DefaultValue = 2, --2,
		SortPriority = -94,
	};
	local arctic_width = {
		Name = "TXT_KEY_MAP_OPTION_ARCTIC_OCEAN_WIDTH",
		Description = "TXT_KEY_MAP_OPTION_ARCTIC_OCEAN_WIDTH_HELP",
		Values = {
			"TXT_KEY_MAP_OPTION_NARROW",
			"TXT_KEY_MAP_OPTION_NORMAL",
			"TXT_KEY_MAP_OPTION_WIDE",
			"TXT_KEY_MAP_OPTION_RANDOM",
		},
		DefaultValue = 2, --2,
		SortPriority = -93,
	};
	local resources = {
		Name = "TXT_KEY_MAP_OPTION_RESOURCES",
		Values = {
			"TXT_KEY_MAP_OPTION_SPARSE",
			"TXT_KEY_MAP_OPTION_STANDARD",
			"TXT_KEY_MAP_OPTION_ABUNDANT",
			"TXT_KEY_MAP_OPTION_LEGENDARY_START",
			"TXT_KEY_MAP_OPTION_STRATEGIC_BALANCE",
			"TXT_KEY_MAP_OPTION_RANDOM",
		},
		DefaultValue = 2, --2,
		SortPriority = -92,
	};
	local islands = {
		Name = "TXT_KEY_MAP_OPTION_ISLANDS",
		Description = "TXT_KEY_MAP_OPTION_ISLANDS_HELP",
		Values = {
			"TXT_KEY_MAP_OPTION_NONE",
			"TXT_KEY_MAP_OPTION_FEW",
			"TXT_KEY_MAP_OPTION_SCATTERED",
			"TXT_KEY_MAP_OPTION_STANDARD",
			"TXT_KEY_MAP_OPTION_MORE",
			"TXT_KEY_MAP_OPTION_ABUNDANT",
			"TXT_KEY_MAP_OPTION_RANDOM",
		},
		DefaultValue = 4, --4,
		SortPriority = -91,
	};
	local plate_motion = {
		Name = "TXT_KEY_MAP_OPTION_PLATE_MOTION",
		Description = "TXT_KEY_MAP_OPTION_PLATE_MOTION_HELP",
		Values = {
			"TXT_KEY_MAP_OPTION_SLOW",
			"TXT_KEY_MAP_OPTION_AVERAGE",
			"TXT_KEY_MAP_OPTION_FAST",
			"TXT_KEY_MAP_OPTION_RAMMING_SPEED",
			"TXT_KEY_MAP_OPTION_RANDOM",
		},
		DefaultValue = 2, --2,
		SortPriority = -90,
	};
	local continents = {
		Name = "TXT_KEY_MAP_OPTION_CONTINENTS",
		Description = "TXT_KEY_MAP_OPTION_CONTINENTS_HELP",
		Values = {
			"TXT_KEY_MAP_OPTION_FRAGMENTED",
			"TXT_KEY_MAP_OPTION_BALANCED",
			"TXT_KEY_MAP_OPTION_CLUSTERED",
			"TXT_KEY_MAP_OPTION_CONTIGUOUS",
			"TXT_KEY_MAP_OPTION_RANDOM",
		},
		DefaultValue = 2, --2,
		SortPriority = -89,
	};
	local land = {
		Name = "TXT_KEY_MAP_OPTION_LAND",
		Description = "TXT_KEY_MAP_OPTION_LAND_HELP",
		Values = {
			"TXT_KEY_MAP_OPTION_FEW",
			"TXT_KEY_MAP_OPTION_LESS",
			"TXT_KEY_MAP_OPTION_STANDARD",
			"TXT_KEY_MAP_OPTION_MORE",
			"TXT_KEY_MAP_OPTION_MUCH_MORE",
			"TXT_KEY_MAP_OPTION_RANDOM"
		},
		DefaultValue = 3, --3,
		SortPriority = -88,
	};
	local plates_set = {
		Name = "TXT_KEY_MAP_OPTION_PLATES",
		Description = "TXT_KEY_MAP_OPTION_PLATES_HELP",
		Values = {
			"TXT_KEY_MAP_OPTION_FEW",
			"TXT_KEY_MAP_OPTION_LESS",
			"TXT_KEY_MAP_OPTION_STANDARD",
			"TXT_KEY_MAP_OPTION_MODERATE",
			"TXT_KEY_MAP_OPTION_MORE",
			"TXT_KEY_MAP_OPTION_RANDOM",
		},
		DefaultValue = 3, --3,
		SortPriority = -87,
	};
	local coastalStarts = {
		Name = "TXT_KEY_MAP_OPTION_COASTAL_START",
		Description = "TXT_KEY_MAP_OPTION_COASTAL_START_HELP",
		Values = {
			"TXT_KEY_MAP_NO",
			"TXT_KEY_MAP_YES",
			"TXT_KEY_MAP_OPTION_RANDOM",
		},
		DefaultValue = 1,
		SortPriority = -86,
	};
	local startPlacement = {
		Name = "TXT_KEY_MAP_OPTION_START_PLACEMENT",
		Description = "TXT_KEY_MAP_OPTION_START_PLACEMENT_HELP",
		Values = {
			"TXT_KEY_MAP_OPTION_LARGEST_LANDMASS",
			"TXT_KEY_MAP_OPTION_ANY_CONTINENTS",
			"TXT_KEY_MAP_OPTION_START_ANYWHERE",
			"TXT_KEY_MAP_OPTION_RANDOM",
		},
		DefaultValue = 2,
		SortPriority = -85,
	};
	return world_age, temperature, rainfall, sea_level, resources, islands, plate_motion, continents, land, plates_set, map_ratio, size_mod, ocean_rifts,arctic_width, coastalStarts, startPlacement
end

function FractalWorld:ShiftPlotTypes()
	local stripRadius = self.stripRadius;
	local shift_x = 0;

	shift_x = self:DetermineXShift();
	print("Shift",shift_x);

	self:ShiftPlotTypesBy(shift_x, 0);
end

local mapWidth;
local mapHeight;
local numPlates;
local maps;
local numMaps = 4;
local skewFactor;
local skewFactor2;
local plateMap = {};
local plates = {};
local xOffset = 0;
local balancedRandom = 0;
local rain;

local size_ModMultiplier = 1;
local fLatitudeChange = 0;

function GetMapInitData(worldSize)
	-- This function can reset map grid sizes or world wrap settings.
	print("")
	print("------------------ Generating Map --------------------------")
	print("")

	local map_ratio		=	Map.GetCustomOption(11);
	local size_mod 		=	Map.GetCustomOption(12);
	
	if (size_mod == 10) then			-- Random
		size_mod = 1 + Map.Rand(5, "Random Size Modifier Setting") + Map.Rand(3, "Random Size Modifier Setting 2") + Map.Rand(3, "Random Size Modifier Setting 3");
	end
	if (size_mod == 1) then
		size_modMultiplier = 0.80;	-- Compressed size - 80%
	elseif (size_mod == 2) then
		size_modMultiplier = 0.85;	-- Squeezed size - 85%
	elseif (size_mod == 3) then
		size_modMultiplier = 0.90;	-- Reduced size - 90%
	elseif (size_mod == 4) then
		size_modMultiplier = 0.95;	-- Smaller size - 95%
	elseif (size_mod == 5) then
		size_modMultiplier = 1.00;	-- Normal size - 100%
	elseif (size_mod == 6) then
		size_modMultiplier = 1.05;	-- Plus size - 105%
	elseif (size_mod == 7) then
		size_modMultiplier = 1.10;	-- Extra size - 110%
	elseif (size_mod == 8) then
		size_modMultiplier = 1.15;	-- Super size - 115%
	elseif (size_mod == 9) then
		size_modMultiplier = 1.20;	-- Gigantic size - 120%
	end
	print("Map Size Modifier:   (" .. size_mod .. ") " .. size_modMultiplier * 100 .. "%" );
	
		
	local map_ratioMultiplierW = 1;
	local map_ratioMultiplierH = 1;
	if (map_ratio == 6) then			-- Random
		map_ratio = 1 + Map.Rand(3, "Random Map Size Ratio Setting") + Map.Rand(3, "Random Map Size Ratio Setting 2");
	end
	 local deltaW
    if (map_ratio == 1) then            -- Squared
        deltaW = -0.10
        fLatitudeChange = 0.05;
    elseif (map_ratio == 2) then        -- Squarish
        deltaW = -0.05
        fLatitudeChange = 0.03;
    elseif (map_ratio == 3) then        -- Rectangular 
        deltaW = 0
        fLatitudeChange = 0;
    elseif (map_ratio == 4) then        -- Wide Rectangular
        deltaW = 0.05
        fLatitudeChange = -0.03;
    elseif (map_ratio == 5) then        -- Rectilinear
        deltaW = 0.10
        fLatitudeChange = -0.05;
    end
    map_ratioMultiplierW = 1 + deltaW;
    map_ratioMultiplierH = 1 - deltaW/(1+ deltaW)
	
	print("Map Size Ratio:      (" .. map_ratio .. ") Width: " .. map_ratioMultiplierW * 100 .. "%, Height: " .. map_ratioMultiplierH * 100 .. "%");
		
	local worldsizes = {
			[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {44, 26},
			[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {60, 40},
			[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {72, 46},
			[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {88, 58},
			[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {114, 70},
			[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {140, 88}
	}
	local grid_size = worldsizes[worldSize];
	--
	local world = GameInfo.Worlds[worldSize];
	if(world ~= nil) then
		print("--");
		print("Default map size:    Width: " .. grid_size[1] .. ", Height: " .. grid_size[2]);
		print("Modified map size:   Width: " .. math.floor((grid_size[1] * map_ratioMultiplierW) * math.sqrt(size_modMultiplier)) .. ", Height: " .. math.floor((grid_size[2] * map_ratioMultiplierH) * math.sqrt(size_modMultiplier)));
		print("--");
	return {
		Width = math.floor((grid_size[1] * map_ratioMultiplierW) * math.sqrt(size_modMultiplier)),
		Height = math.floor((grid_size[2] * map_ratioMultiplierH) * math.sqrt(size_modMultiplier)),
		WrapX = true,
	};
    end
end

function InitHeightmaps()
	maps = {}

	for i=0, numMaps, 1 do
		maps[i] = {}

		factor = math.pow(2,i);

		for x = 0, math.ceil(mapWidth/factor), 1 do
			for y = 0, math.ceil(mapWidth/factor), 1 do
				q = (y * (math.ceil(mapWidth/factor)+1)) + x;
				maps[i][q] = RandomFloat() * 2 - 1;
			end
		end
	end
end

function Interpolate(a, b, x)
	ft = x * math.pi;
	f = (1 - math.cos(ft)) * 0.5;

	return  a*(1.0-f) + b*f;
end

function RandomFloat()
	local max = Map.Rand(16384,"Random float");
	return max/16384.0;
end

function GetHeightFromMap(i,x,y)
	factor = math.pow(2,i);

	indexX = x/factor;
	indexY = y/factor;
	intX = math.floor(indexX);
	intY = math.floor(indexY);
	fracX = indexX - intX;
	fracY = indexY - intY;

	q = (intY * (math.ceil(mapWidth/factor)+1)) + intX;
	q2 = ((intY+1) * (math.ceil(mapWidth/factor)+1)) + intX;
	r1 = Interpolate(maps[i][q], maps[i][q+1], fracX);
	r2 = Interpolate(maps[i][q2], maps[i][q2+1], fracX);

	return Interpolate(r1, r2, fracY);
end

function GetHeight(x,y,n,f)
	x = (x + xOffset) % mapWidth
	height = 0;
	for mapIndex, thisMap in ipairs(maps) do
		if mapIndex < numMaps-f then
			height = height + GetHeightFromMap(mapIndex,x,y)/(math.pow(n, numMaps-(mapIndex-1)));
		end
	end

	return height;
end

function FractalWorld:GeneratePlotTypes(args)

	-- Set variables for custom options, 1-5 are reserved for default civ 5 options
	local islands 		=	Map.GetCustomOption(6);
	local plate_motion	=	Map.GetCustomOption(7);
	local continents	=	Map.GetCustomOption(8);
	local land 			=	Map.GetCustomOption(9);
	local plates_set 	=	Map.GetCustomOption(10);
	local ocean_rifts	=	Map.GetCustomOption(13);
	local arctic_width	=	Map.GetCustomOption(14);

	local getWorldSize	=	Map.GetWorldSize();

	skewFactor = { RandomFloat()*2+1, RandomFloat()+1, RandomFloat()+1,  RandomFloat()*6+5, RandomFloat()*3+3, RandomFloat()*2+2,  RandomFloat()*7, RandomFloat()*7, RandomFloat()*7};
	skewFactor2 = { RandomFloat()*10+5, RandomFloat()*5+5, RandomFloat()*5+3, RandomFloat()*5+3, RandomFloat()*7, RandomFloat()*7, RandomFloat()*7, RandomFloat()*7};

	local WorldSizeTypes = {};
	for row in GameInfo.Worlds() do
		WorldSizeTypes[row.Type] = row.ID;
	end

	local platesSizes = {
		[WorldSizeTypes.WORLDSIZE_DUEL]     = 16, -- 4
		[WorldSizeTypes.WORLDSIZE_TINY]     = 24, -- 8
		[WorldSizeTypes.WORLDSIZE_SMALL]    = 32, -- 16
		[WorldSizeTypes.WORLDSIZE_STANDARD] = 40, -- 20
		[WorldSizeTypes.WORLDSIZE_LARGE]    = 60, -- 24
		[WorldSizeTypes.WORLDSIZE_HUGE]		= 80  -- 32
	}

	numPlates = platesSizes[getWorldSize];

	if (plates_set == 6) then				-- Random
		plates_set = 1 + Map.Rand(3, "Random Plates Setting") + Map.Rand(3, "Random Plates Setting 2");
	end
	if (plates_set == 1) then				-- Few
		numPlates = numPlates * 0.6;
		print("Plates option:       (1) Few      [" .. numPlates .. " Plates]");
	elseif (plates_set == 2) then			-- Less
		numPlates = numPlates * 0.8;
		print("Plates option:       (2) Less     [" .. numPlates .. " Plates]");
	elseif (plates_set == 3) then			-- Standard
		numPlates = numPlates;
		print("Plates option:       (3) Standard [" .. numPlates .. " Plates]");
	elseif (plates_set == 4) then			-- Moderate
		numPlates = numPlates * 1.2;
		print("Plates option:       (4) Moderate [" .. numPlates .. " Plates]");
	elseif (plates_set == 5) then			-- More
		numPlates = numPlates * 1.4;
		print("Plates option:       (5) More     [" .. numPlates .. " Plates]");
	end

	mapWidth = self.iNumPlotsX;
	mapHeight = self.iNumPlotsY;
	
	if (arctic_width == 4) then			-- Random
		arctic_width = 1 + Map.Rand(2, "Random Arctic Ocean Width Setting") + Map.Rand(2, "Random Arctic Ocean Width Setting 2");
	end
	print ("Arctic Ocean width:  (" .. arctic_width .. ")");
	
	-- we generate the first two plates at the poles. we'll fill them with water later, this reduces the amount of arctic land
	-- the remaining plates are generated at random positions
	numPlates = numPlates + 2
	for i = 1, numPlates, 1 do
		plates[i] = {};
		plates[i].x = Map.Rand(mapWidth, "centerX of Plate");
		if (i == 1) then
			plates[i].y = 1
			plates[i].mass = 14 + arctic_width;
		elseif (i == 2) then
			plates[i].y = mapHeight-1
			plates[i].mass = 14 + arctic_width;
		else
			plates[i].y = Map.Rand(mapHeight - 2*math.floor(arctic_width*mapHeight*5/100), "centerY of Plate") + math.floor(arctic_width*mapHeight*5/100)
			plates[i].mass = Map.Rand(3, "mass of Plate") + 10;
		end
		plates[i].size = 0;
		plates[i].terrainType = "?";
		plates[i].neighbours_borders = {};
		plates[i].neighbours_relativeMotions = {};
		plates[i].velocity = {};
		plates[i].velocity.x = Map.Rand(21, "velocity in x direction")-10;
		plates[i].velocity.y = Map.Rand(21, "velocity in y direction")-10;
	end

	neighbours = {};
	for i = 1, numPlates, 1 do
		neighbours[i] = {};
		for j = 1, numPlates, 1 do
			neighbours[i][j] = 0;
		end
	end

	for x = 0, mapWidth-1, 1 do
		for y = 0, mapHeight-1, 1 do
			CachePartOfPlate(x,y);
		end
	end
	
	for x = 0, mapWidth-1, 1 do
		for y = 0, mapHeight-1, 1 do
			currentPlate = PartOfPlate(x,y);
			info = GetPlateBoundaryInfo(x, y);
			for infoIndex, thisPlateInfo in ipairs(info) do
				if (infoIndex ~= currentPlate) then
					neighbours[currentPlate][infoIndex] = neighbours[currentPlate][infoIndex]+thisPlateInfo;
				end
			end
		end
	end
	
	for plateIndex, thisPlate in ipairs(plates) do
		thisPlate.neighbours_borders = neighbours[plateIndex];
	end

	InitHeightmaps()

	for plateIndex, thisPlate in ipairs(plates) do
		motions = {};
		for plateIndex2, thatPlate in ipairs(plates) do
			motions[plateIndex2] = {};
			if (thisPlate.neighbours_borders[plateIndex2] > 0) then
				x = thatPlate.velocity.x - thisPlate.velocity.x;
				y = thatPlate.velocity.y - thisPlate.velocity.y;

				motions[plateIndex2].x = x;
				motions[plateIndex2].y = y;

				mag = math.sqrt(x*x+y*y);
				if (mag < 5) then
					motions[plateIndex2].boundaryType = "Passive";
				else
					xNorm = x/mag;
					yNorm = y/mag;

					dirX = thatPlate.x - thisPlate.x;
					dirY = thatPlate.y - thisPlate.y;

					dirMag = math.sqrt(dirX*dirX+dirY*dirY);
					dirXNorm = dirX/dirMag;
					dirYNorm = dirY/dirMag;

					dotProduct = xNorm*dirXNorm + yNorm*dirYNorm;

					if (dotProduct < -0.5) then
						motions[plateIndex2].boundaryType = "Collision";
					elseif (dotProduct > 0.5) then
						motions[plateIndex2].boundaryType = "Rift";
					else
						motions[plateIndex2].boundaryType = "Transform";
					end
				end
			end
		end
		thisPlate.neighbours_relativeMotions = motions;
	end

	for x = 0, mapWidth-1, 1 do
		for y = 0, mapHeight-1, 1 do
			local currentPlate = PartOfPlate(x, y);
			plates[currentPlate].size = plates[currentPlate].size + 1;
		end
	end
	
	-- Plate positions have been determined and each tile on the map is assigned a plate.
	-- In the following, each plate will be assigned one of five possible types: Continental, Ocean, Deep Ocean, Fractal and Islands.
	-- The generated terrain will depend on the plate type.
	-- We will now calculate the percentage of plots on the map that should be assigned to each of the plate types.

	if (islands == 7) then				-- Random
		islands = 1 + Map.Rand(3, "Random Islands Setting") + Map.Rand(4, "Random Islands Setting 2");
	end
	local islandPercentDesired = 0; -- the percentage of plots on the map that should be on island plates
	if (islands == 1) then				-- None
		islandPercentDesired = 0; 
		print("Islands option:      (1) None");
	elseif (islands == 2) then			-- Few
		islandPercentDesired = 0.03;
		print("Islands option:      (2) Few");
	elseif (islands == 3) then			-- Scattered
		islandPercentDesired = 0.07;
		print("Islands option:      (3) Scattered");
	elseif (islands == 4) then			-- Standard
		islandPercentDesired = 0.10;
		print("Islands option:      (4) Standard");
	elseif (islands == 5) then			-- More
		islandPercentDesired = 0.15;
		print("Islands option:      (5) More");
	elseif (islands == 6) then			-- Abundant
		islandPercentDesired = 0.30;
		print("Islands option:      (6) Abundant");
	end
	
	local sea_level = Map.GetCustomOption(4);
	if (sea_level == 6) then			-- Random
		sea_level = 1 + Map.Rand(3, "Random Sea Level") + Map.Rand(3, "Random Sea Level 2");
	end
	local sea_levelModifier = 0;
	if (sea_level == 1) then			-- Shallow
		sea_levelModifier = -0.05;
		print("Sea Level option:    (1) Shallow");
	elseif (sea_level == 2) then		-- Low
		 sea_levelModifier = -0.025;
		print("Sea Level option:    (2) Low");
	elseif (sea_level == 3) then		-- Medium
		sea_levelModifier = 0;
		print("Sea Level option:    (3) Medium");
	elseif (sea_level == 4) then		-- High
		 sea_levelModifier = 0.025;
		print("Sea Level option:    (4) High");
	elseif (sea_level == 5) then		-- Extreme
		 sea_levelModifier = 0.05;
		print("Sea Level option:    (5) Extreme");
	end

	local landSize = 0.25

	if (land == 6) then					-- Random
		land = 1 + Map.Rand(3, "Random Land Setting") + Map.Rand(3, "Random Land Setting 2");
	end
	local landModifier = 0;
	if (land == 1) then
		landModifier = -0.25;
		print("Land option:         (1) Few");
	elseif (land == 2) then
		landModifier = -0.125;
		print("Land option:         (2) Less");
	elseif (land == 3) then
		landModifier = -0.00;
		print("Land option:         (3) Standard");
	elseif (land == 4) then
		landModifier = 0.125;
		print("Land option:         (4) More");
	elseif (land == 5) then
		landModifier = 0.25;
		print("Land option:         (5) Much More");
	end
	
	-- the desired percentage of plots that should be on landmasses (continental or fractal plates)
	local landPercentDesired = landSize * (1 + landModifier)

	-- sea level modifier: convert some land to islands (or vice versa). it is intended that this also affects the total amount of land
	landPercentDesired = landPercentDesired - sea_levelModifier
	islandPercentDesired = islandPercentDesired + sea_levelModifier
	if (islandPercentDesired < 0) then
		islandPercentDesired = 0
	end
	
	local island_land_factor = 0.25 -- assume this percentage of the plots on island plates are land tiles
	local landAmountReduced_island = 0.5
	-- for each land tile on an island, reduce the desired number of landmass tiles by this factor. Otherwise settings with many islands would have too much land
	landPercentDesired = landPercentDesired - islandPercentDesired*island_land_factor*landAmountReduced_island
	

	
	print("--")
	print("Desired Percentage of Plots on Landmasses: " .. string.format("%05.2f%%", 100*landPercentDesired),
				string.format("(%.2f%% Base Value, %.2f%% from Land Mod, %.2f%% from Sea Level, %.2f%% from Islands)",
				100*landSize, 100*landSize*landModifier,
				100*(-sea_levelModifier),
				100*(- islandPercentDesired*island_land_factor*landAmountReduced_island)));
	print("--")

	if (continents == 5) then			-- Random
		continents = 1 + Map.Rand(2, "Random Land Cohesion Setting") + Map.Rand(3, "Random Land Cohesion Setting");
	end
	local continents_modifier = 0; -- how much should land plates be clustered
	local fractal_modifier = 0; -- ratio of continent plates to be converted to fractal
	if (continents == 1) then
		continents_modifier = -1;
		fractal_modifier = 0.6 
		print("Cohesion option:     (1) Fragmented");
	elseif (continents == 2) then
		continents_modifier = -0.2;
		fractal_modifier = 0.4
		print("Cohesion option:     (2) Balanced");
	elseif (continents == 3) then
		continents_modifier = 0.2;
		fractal_modifier = 0.2
		print("Cohesion option:     (3) Clustered");
	elseif (continents == 4) then
		continents_modifier = 1;
		fractal_modifier = 0
		print("Cohesion option:     (4) Contiguous");
	end
	fractal_modifier = fractal_modifier + sea_levelModifier
	if (fractal_modifier < 0) then
		fractal_modifier = 0
	end
	
	if (plate_motion == 5) then			-- Random
		plate_motion = 1 + Map.Rand(2, "Random Plate Motion Setting") + Map.Rand(3, "Random Plate Motion Setting 2");
	end
	local plate_motionModifier = 0;
	local plate_motionMultiplier = 0;
	if (plate_motion == 1) then			-- Slow
		plate_motionModifier = -0.05;
		plate_motionMultiplier = 0.66;
		print("Plate Motion option: (1) Slow");
	elseif (plate_motion == 2) then		-- Average
		plate_motionModifier = 0.1;
		plate_motionMultiplier = 1;
		print("Plate Motion option: (2) Average");
	elseif (plate_motion == 3) then		-- Fast
		plate_motionModifier = 0.2;
		plate_motionMultiplier = 2;
		print("Plate Motion option: (3) Fast");
	elseif (plate_motion == 4) then		-- Ramming Speed!
		plate_motionModifier = 0.3;
		plate_motionMultiplier = 2.5;
		print("Plate Motion option: (4) Ramming Speed");
	else								-- Fallback for Random
		plate_motionModifier = 0.15;
		plate_motionMultiplier = 1.5;
	end
	
	
	-- Get Rainfall setting input by user.
	rain = Map.GetCustomOption(3)
	if rain == 4 then
		rain = 1 + Map.Rand(2, "Random Rainfall - Lua") + Map.Rand(2, "Random Rainfall - Lua 2");
	end
	if (rain == 1) then
		print("Rainfall option:     (1) Arid");
	elseif (rain == 2) then
		print("Rainfall option:     (2) Normal");
	else
		print("Rainfall option:     (3) Wet");
	end
	
	if (ocean_rifts == 3) then			-- Random: 25% chance for only one rift
		ocean_rifts = 1 + Map.Rand(4, "Random Ocean Rift Width Setting");
		if ocean_rifts > 2 then
			ocean_rifts = 2
		end
	end
	
	-- Plates at the northern or southern edge of the map are "deep ocean" to reduce the amount of tundra/snow tiles
	for plateIndex, thisPlate in ipairs(plates) do
		thisPlate.terrainDetail = ""
		if (thisPlate.y > (1 - 0.05*arctic_width)*mapHeight or thisPlate.y < 0.05*arctic_width*mapHeight) then
			thisPlate.terrainType = "Deep Ocean";
			thisPlate.terrainDetail = "North/South"
		end
	end
	
	local xFirstRift = 0;
	local xSecondRift = 0;
	-- if the option "Ocean Rifts" is set, we want to generate two rifts that will be "Deep Ocean", so it's not possible to meet all civs before Renaissance
	if (ocean_rifts == 2) then
		
		-- find the best location for the first rift
		-- find the coordinate that would cause the least amount of additional water if all plates with a tile near that coordinate were filled with water
		local leastwater = 99999;
		riftplates = {};
		for x = 0, mapWidth-1, 1 do
			local newplates = {}
			for y = 0, mapHeight-1, 1 do
				for xnear = x-2, x+2, 1 do
					local thisPlate = PartOfPlate(xnear % mapWidth,y);
					if (plates[thisPlate].terrainType ~= "Deep Ocean") then
						newplates[thisPlate]=true;
					end
				end
			end
			local water = 0
			for plate, _ in pairs(newplates) do
				water = water + plates[plate].size;
			end
			if water < leastwater then
				xFirstRift = x;
				leastwater = water;
				riftplates = newplates
			end
		end
		
		-- fill the plates at position xFirstRift with water
		for plate, _ in pairs(riftplates) do
			plates[plate].terrainType = "Deep Ocean"
			plates[plate].terrainDetail = "Rift 1"
		end
	
		local deepOceanPercent = 0;
		
		for plateIndex, thisPlate in ipairs(plates) do
			if (thisPlate.terrainType == "Deep Ocean") then
				deepOceanPercent = deepOceanPercent + thisPlate.size/(mapHeight*mapWidth);
			end
		end
		print("--")
		print("First Rift at x =", xFirstRift)
		
		-- if possible, find a second x-coordinate far enough from the first one for the second rift
		local freeAmountForSecondRift = 1 - deepOceanPercent - landPercentDesired - islandPercentDesired
		if (freeAmountForSecondRift > 0) then
			leastwater = 99999;
			riftplates = {};
			for x = 0, mapWidth-1, 1 do
				local distx = ((xFirstRift-x) + mapWidth/2 ) % mapWidth - mapWidth/2 -- distance between xFirstRift and x, taking into account the map wrap
				if (math.abs(distx) > mapWidth/3) then
					local newplates = {}
					for y = 0, mapHeight-1, 1 do
						for xnear = x-2, x+2, 1 do
							local thisPlate = PartOfPlate(xnear % mapWidth,y);
							if (plates[thisPlate].terrainType ~= "Deep Ocean") then
								newplates[thisPlate]=true;
							end
						end
					end
					local water = 0
					local str = "";
					for plate, _ in pairs(newplates) do
						water = water + plates[plate].size;
					end
					if water < leastwater then
						xSecondRift = x;
						leastwater = water;
						riftplates = newplates
					end
				end
			end
			
			local newOceanSize = 0;
			for plate, _ in pairs(riftplates) do
				newOceanSize = newOceanSize + plates[plate].size/(mapHeight*mapWidth)
			end
			if (newOceanSize > freeAmountForSecondRift and newOceanSize <= freeAmountForSecondRift + islandPercentDesired) then
				-- if necessary, reduce islandPercentDesired to make room for the rift
				islandPercentDesired = islandPercentDesired - (newOceanSize - freeAmountForSecondRift)
				print(string.format(" Reducing islands by %02.2f%% to make room for second rift. New desired percentage of islands:%02.2f%%",
					100*(newOceanSize - freeAmountForSecondRift), 100*islandPercentDesired))
				freeAmountForSecondRift = 1 - deepOceanPercent - landPercentDesired - islandPercentDesired
			end
			
			if (newOceanSize <= freeAmountForSecondRift) then
			
				print("Second Rift at x =", xSecondRift)
				for plate, _ in pairs(riftplates) do
					plates[plate].terrainType = "Deep Ocean"
					plates[plate].terrainDetail = "Rift 2"
				end
			else
				ocean_rifts = 1
				print("Not enough space for second Rift. Current Ocean:", string.format("%02.2f", 100*deepOceanPercent),
					"Free space left:",string.format("%02.2f",100*freeAmountForSecondRift),
					"Second Rift would consume", string.format("%02.2f", 100*newOceanSize))
			end
		else
			ocean_rifts = 1
			print("Not enough space for second Rift. Ocean:", deepOceanPercent,"Free space left:",string.format("%02.2f",freeAmountForSecondRift))
		end
		
		-- move the map wrap to the coordinate of the first rift, having water at the map wrap makes the minimap look nicer. note that this affects the values returned by `PartOfPlate` and other function
		xOffset = xFirstRift
	end
	
	print("--")
		
	deepOceanPercent = 0;
	for plateIndex, thisPlate in ipairs(plates) do
		if (thisPlate.terrainType == "Deep Ocean") then
			deepOceanPercent = deepOceanPercent + thisPlate.size/(mapHeight*mapWidth);
		end
	end
	
	-- if possible and desired, convert some of the land amount to fractal
		
	-- ratio of land we expect on Fractal plates
	local fractal_land_factor = 0.50; 

	if (fractal_modifier > 0) then
		-- we need to reserve more space for fractal plates than for continental plates to get the same amount land tiles
		local freeSpace = 1 - deepOceanPercent - islandPercentDesired 		-- tiles that can be used for continental/fractal

		-- we want to have a total of landPercentDesired land tiles (continental or fractal).
		-- On continental plates we expect a land ratio of 1, on fractal plates a ratio of fractal_land_factor

		-- In the previous calculations we made sure that freeSpace > landPercentDesired, so there's always enough space if we use only continental plates
		-- We would like to convert a share of fractal_modifier continent plates to fractal, let's see if this works
		if (landPercentDesired * (1 - fractal_modifier) + (landPercentDesired * fractal_modifier / fractal_land_factor) > freeSpace) then
			-- we don't have enough space! reduce fractal_modifier accordingly
			fractal_modifier = (freeSpace - landPercentDesired) / (landPercentDesired/fractal_land_factor - landPercentDesired)
			print("fractal_modifier reduced because we don't have enough empty space. New value: ", fractal_modifier)
		end
	end
	
	-- these are the percentages of tiles that we want to fill with Fractal Plates and Continental Plates
	local fractalPercentDesired = landPercentDesired * fractal_modifier / fractal_land_factor
	local continentalPercentDesired = landPercentDesired * (1 - fractal_modifier)
	-- islandPercentDesired has already been calculated
	-- the remaining space is filled with Ocean
	local randomOceanPercentDesired = (1 - deepOceanPercent - continentalPercentDesired - fractalPercentDesired - islandPercentDesired)

	-- we now go through the available plates and assign to each plate a type
	-- when doing so, we make sure that the values continentalPercentDesired etc. are not exceeded
	-- also, we do the calculation separately for the plates west of rift1 and for the plates east of rift1, so the distribution of plate types is similar on both sides
	
	-- rift calculations, if applicable
	local numUnassignedPlotsBetweenRifts = 0
	local numUnassignedPlotsTotal = 0
	local riftWest = -1
	local riftEast = -1
	if (ocean_rifts == 2) then
		riftWest = math.min(xFirstRift, xSecondRift)
		riftEast = math.max(xFirstRift, xSecondRift)
		for plateIndex, thisPlate in ipairs(plates) do
			if (thisPlate.terrainType == "?") then
				if (riftWest <= thisPlate.x and thisPlate.x < riftEast) then
					numUnassignedPlotsBetweenRifts = numUnassignedPlotsBetweenRifts + thisPlate.size
				end	
				numUnassignedPlotsTotal = numUnassignedPlotsTotal + thisPlate.size
			end
		end
	end
	
	-- keep track of the current values
	local continentalPercent = 0
	local fractalPercent = 0
	local islandPercent = 0
	local randomOceanPercent = 0
	
	for rift = 1, ocean_rifts, 1 do
		print("Plate Region", rift)
		
		local desired_percent = 1  -- (num tiles in this region) / (total num tiles to be filled)
		if (ocean_rifts == 2 and rift == 1) then
			desired_percent = numUnassignedPlotsBetweenRifts / numUnassignedPlotsTotal
		end
		
		-- first assign all continental plates, then the fractal plates, then island, then ocean
		plateTypeToFill = "Continental"
		
		repeat
			-- what is the highest possible size the next plate can have?
			local deficit
			if (plateTypeToFill == "Continental") then
				deficit = continentalPercentDesired * desired_percent - continentalPercent;
			elseif (plateTypeToFill == "Fractal") then
				deficit = fractalPercentDesired * desired_percent - fractalPercent;
			elseif (plateTypeToFill == "Islands") then
				deficit = islandPercentDesired * desired_percent - islandPercent;
			else
				deficit = randomOceanPercentDesired * desired_percent - randomOceanPercent;
			end
			
			-- find best next plate to fill
			-- we calculate a score based on the size of the plate and the number of adjacent land tiles
			-- the idea is to fill large plates first, and to prefer having land plates that are adjacent to each other
			
			local bestPlate = 0
			local bestScore = -99999
			local plateToFillFound = false
			for plateIndex, thisPlate in ipairs(plates) do
				-- if rift==1 and ocean_rift==2, only consider the plates between the rifts. in all other cases, consider all (remaining) plates
				if (thisPlate.terrainType == "?" and ((ocean_rifts == rift) or (riftWest <= thisPlate.x and thisPlate.x < riftEast))) then
					plateToFillFound = true
					-- if we're filling ocean plates, we know that every remaining plate has to be "ocean", there are no other types left
					-- so we can simplify
					if (plateTypeToFill == "Ocean") then
						bestPlate = plateIndex
						break
					end
					-- for all other plate types, make sure we don't exceed the limit
					if (thisPlate.size/(mapHeight*mapWidth) < deficit) then
						score = thisPlate.size
						for plateIndex2, neighborPlate in ipairs(plates) do
							if (neighborPlate.terrainType == "Continental") then
								score = score + continents_modifier * thisPlate.neighbours_borders[plateIndex2]
							elseif (neighborPlate.terrainType == "Fractal") then
								score = score + continents_modifier * thisPlate.neighbours_borders[plateIndex2] * fractal_land_factor
							end
						end
						if (score > bestScore) then
							bestScore = score
							bestPlate = plateIndex
						end
					end
				end
			end
			
			
			if (plateToFillFound) then
				
				if (bestPlate == 0) then
					-- we didn't find a plate that would not take us over the DesiredAmount limit of this plate, but there are still plates left to fill
					-- so we have to continue with the next plate type
					local newPlateType
					if (plateTypeToFill == "Continental") then
						newPlateType = "Fractal"
					elseif (plateTypeToFill == "Fractal") then
						newPlateType = "Islands"
					elseif (plateTypeToFill == "Islands") then
						newPlateType = "Ocean"
					end
					plateTypeToFill = newPlateType
					-- in the next iteration of the loop, we will repeat the scoring for the new plate type
				else
					local thisPlate = plates[bestPlate];

					-- assign the desired plate type
					thisPlate.terrainType = plateTypeToFill
					
					-- keep track of the values
					if (plateTypeToFill == "Continental") then
						continentalPercent = continentalPercent + thisPlate.size/(mapHeight*mapWidth);
					elseif (plateTypeToFill == "Fractal") then
						fractalPercent = fractalPercent + thisPlate.size/(mapHeight*mapWidth);
					elseif (plateTypeToFill == "Islands") then
						islandPercent = islandPercent + thisPlate.size/(mapHeight*mapWidth);
					else
						randomOceanPercent = randomOceanPercent + thisPlate.size/(mapHeight*mapWidth);
					end
					
					print("Plate No.", bestPlate, "x", thisPlate.x, "y", thisPlate.y, "Size", string.format("%02.2f", thisPlate.size/(mapHeight*mapWidth)*100), "Type", thisPlate.terrainType)
				end
			end
		until (not plateToFillFound)
	end
		
	print("--");
	print("Amount of deep ocean:         ", string.format("%05.2f",   deepOceanPercent*100).."%", "(Desired: " .. string.format("%05.2f",          deepOceanPercent*100).."%)");
	print("Amount of continental:        ", string.format("%05.2f", continentalPercent*100).."%", "(Desired: " .. string.format("%05.2f", continentalPercentDesired*100).."%)");
	print("Amount of fractal:            ", string.format("%05.2f",     fractalPercent*100).."%", "(Desired: " .. string.format("%05.2f",     fractalPercentDesired*100).."%)");
	print("Amount of islands:            ", string.format("%05.2f",      islandPercent*100).."%", "(Desired: " .. string.format("%05.2f",      islandPercentDesired*100).."%)");
	print("Amount of random ocean:       ", string.format("%05.2f", randomOceanPercent*100).."%", "(Desired: " .. string.format("%05.2f", randomOceanPercentDesired*100).."%)");
	print("--");
	print("--");
	
	-- if no rifts were calculated: find the x-coordinate at which there is the most amount of water, it will be our new map wrap
	if (ocean_rifts == 1) then
		local mostwater = 0;
		for x = 0, mapWidth-1, 1 do
			local water = 0
			for y = 0, mapHeight-1, 1 do
				for xnear = x-2, x+2, 1 do
					local thisPlate = PartOfPlate(x,y);
					if (plates[thisPlate].terrainType == "Ocean" or plates[thisPlate].terrainType == "Deep Ocean") then
						water = water + 1
					elseif (plates[thisPlate].terrainType == "Fractal") then
						water = water + 1 - fractal_land_factor
					elseif (plates[thisPlate].terrainType == "Islands") then
						water = water + 1 - island_land_factor
					end
				end
			end
			if water > mostwater then
				xOffset = x;
				mostwater = water;
			end
		end
		print("--")
		print("New Map Wrap at x =", xOffset)
	end

	print("Generating Terrain");
	
	for x = 0, mapWidth-1, 1 do
		for y = 0, mapHeight-1, 1 do
			local i = y * self.iNumPlotsX + x;
			local currentPlate = PartOfPlate(x, y);

			neighbour = AdjacentToPlate(x,y);
			--neighbour2 = -1;
			--if (neighbour <= 0) then neighbour2 = OneOff(x,y,plates) end
			--[[if (neighbour > 0) then
				boundaryType = plates[currentPlate].neighbours_relativeMotions[neighbour].boundaryType;
				if (boundaryType == "Collision") then
					self.plotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
				elseif (boundaryType == "Transform") then
					self.plotTypes[i] = PlotTypes.PLOT_HILLS;
				elseif (boundaryType == "Rift") then
					self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
				else
					self.plotTypes[i] = PlotTypes.PLOT_LAND;
				end
			else
				self.plotTypes[i] = PlotTypes.PLOT_LAND;
			end]]--

			local value;
			if (plates[currentPlate].terrainType == "Deep Ocean" or plates[currentPlate].terrainType == "Ocean") then value = -1;
			elseif (plates[currentPlate].terrainType == "Continental") then
				value = GetHeight(x,y,2.2,0)+0.2;
				if (value < 0 and NumAdjacentLandTiles(x,y,2.2,0,0,0.2) >= 4) then value = 0.11 end
			elseif (plates[currentPlate].terrainType == "Islands") then
				value = GetHeight(x,y,2,2)-0.05;
				if (value < 0 and NumAdjacentLandTiles(x,y,2.0,2,0,-0.05) >= 2) then value = 0.1 end
			elseif (plates[currentPlate].terrainType == "Fractal") then
				value = GetHeight(x,y,1.5,1)-0.05;
				if (value < 0 and NumAdjacentLandTiles(x,y,1.5,1,0,-0.05) >= 3) then value = 0.1 end
			end

			if (neighbour > 0 and value > -1) then
				boundaryType = plates[currentPlate].neighbours_relativeMotions[neighbour].boundaryType;
				if (boundaryType == "Collision") then
					terrainType = plates[currentPlate].terrainType;
					terrainTypeN = plates[neighbour].terrainType;

					if (terrainType == "Continental") then
						value = value + 0.05 + RandomFloat()/5.0 + plate_motionModifier;
					elseif (terrainType == "Fractal") then
						if (terrainTypeN == "Ocean" or terrainTypeN == "Islands") then
							value = value + RandomFloat()/5.0 + plate_motionModifier;
						else
							value = value + 0.05 + RandomFloat()/5.0 + plate_motionModifier;
						end
					elseif (terrainType == "Islands") then
						value = value + 0.15 + RandomFloat()/2.5 + plate_motionModifier;
					else
						if (terrainTypeN == "Continental") then
							value =  RandomFloat()/2.5 - 0.05;
						else
							value = value + 0.1 + RandomFloat()/2.5;
						end
					end

					if (value < 0.1 + RandomFloat()/5.0 and RandomFloat() < 0.5) then
						value = value - 0.5;
					end
				elseif (boundaryType == "Transform") then
					value = value - (0.05 - RandomFloat()/10.0) * plate_motionMultiplier;
				elseif (boundaryType == "Rift") then
					value = value - 0.05 - RandomFloat()/20.0 - plate_motionModifier/2;
					if (RandomFloat() > 0.97) then value = 0.5 end
				end
			end

			--[[
			if (neighbour2 > 0) then
				boundaryType = plates[currentPlate].neighbours_relativeMotions[neighbour2].boundaryType;
				if (boundaryType == "Collision") then
					terrainType = plates[currentPlate].terrainType;
					terrainTypeN = plates[neighbour2].terrainType;
					if (terrainType == "Continental") then
						value = value + math.random()/5.0 + plate_motionModifier/2;
					end
					if (value < 0.1 + math.random()/5.0 and math.random() < 0.5) then
						value = value - 0.5;
					end
				elseif (boundaryType == "Rift") then
					--value = value - math.random()/20.0 - plate_motionModifier/3;
				end
			end--]]--

			local hillRandom = RandomFloat()/2.0
			if (value < 0) then
				self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
			elseif (value < 0.1 + hillRandom) then
				self.plotTypes[i] = PlotTypes.PLOT_LAND;
			elseif (value < 0.4 + hillRandom) then
				self.plotTypes[i] = PlotTypes.PLOT_HILLS;
			else
				self.plotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
			end

			--[[ TEST
			terrainType = plates[currentPlate].terrainType;
			if (terrainType == "Deep Ocean" or terrainType == "Ocean") then
				self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
			elseif (terrainType == "Islands") then
				self.plotTypes[i] = PlotTypes.PLOT_LAND;
			elseif (terrainType == "Fractal") then
				self.plotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
			else
				self.plotTypes[i] = PlotTypes.PLOT_HILLS;
			end
			--]]--
		end
	end--]]--
	--[[
	for x = 0, mapWidth-1, 1 do
		for y = 0, mapHeight-1, 1 do
			local i = y * self.iNumPlotsX + x;
			if AdjacentToPlate(x,y) > 0 then
				self.plotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
			--elseif OneOff(x,y,plates) > 0 then
			--	self.plotTypes[i] = PlotTypes.PLOT_HILLS;
			else
				self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
			end
		end
	end--]]--
	--self:ShiftPlotTypes();		
			
	local cntoc = {};
	local cntln = {};
	local cntfl = {};
	local cnthi = {};
	local cntmn = {};
	for i=1,5 do
		cntoc[i] = 0;
		cntln[i] = 0;
		cntfl[i] = 0;
		cnthi[i] = 0;
		cntmn[i] = 0;
	end
	for x = 0, mapWidth-1, 1 do
		for y = 0, mapHeight-1, 1 do
			local pl = plates[PartOfPlate(x,y)]
			local terrain;
			if (pl.terrainType == "Deep Ocean") then
				terrain = 1
			elseif (pl.terrainType == "Ocean") then
				terrain = 2
			elseif (pl.terrainType == "Islands") then
				terrain = 3
			elseif (pl.terrainType == "Fractal") then
				terrain = 4
			else
				terrain = 5
			end
		
			local i = y * self.iNumPlotsX + x;
			if(self.plotTypes[i] == PlotTypes.PLOT_MOUNTAIN) then
				cntmn[terrain] = cntmn[terrain] + 1;
				cntln[terrain] = cntln[terrain] + 1;
			elseif(self.plotTypes[i] == PlotTypes.PLOT_LAND) then
				cntfl[terrain] = cntfl[terrain] + 1;
				cntln[terrain] = cntln[terrain] + 1;
			elseif(self.plotTypes[i] == PlotTypes.PLOT_HILLS) then
				cnthi[terrain] = cnthi[terrain] + 1;
				cntln[terrain] = cntln[terrain] + 1;
			elseif(self.plotTypes[i] == PlotTypes.PLOT_OCEAN) then
				cntoc[terrain] = cntoc[terrain] + 1;
			end
		end
	end
	
	print("--")
	print("--")
	
	local pct = 100/(mapWidth*mapHeight);
	print("Ocean:        ", (cntoc[1]+cntoc[2]+cntoc[3]+cntoc[4]+cntoc[5]) .. "  ",
		string.format("%05.2f%% (DO: %05.2f%%, RO: %05.2f%%, ISL: %05.2f%%, FRT: %05.2f%%, CNT: %05.2f%%)",
		(cntoc[1]+cntoc[2]+cntoc[3]+cntoc[4]+cntoc[5])*pct, cntoc[1]*pct, cntoc[2]*pct, cntoc[3]*pct, cntoc[4]*pct, cntoc[5]*pct));
		
	print("Total Land:   ", (cntln[1]+cntln[2]+cntln[3]+cntln[4]+cntln[5]) .. "  ",
		string.format("%05.2f%% (DO: %05.2f%%, RO: %05.2f%%, ISL: %05.2f%%, FRT: %05.2f%%, CNT: %05.2f%%)",
		(cntln[1]+cntln[2]+cntln[3]+cntln[4]+cntln[5])*pct, cntln[1]*pct, cntln[2]*pct, cntln[3]*pct, cntln[4]*pct, cntln[5]*pct));
		
	print("Flat Land:    ", (cntfl[1]+cntfl[2]+cntfl[3]+cntfl[4]+cntfl[5]) .. "  ",
		string.format("%05.2f%% (DO: %05.2f%%, RO: %05.2f%%, ISL: %05.2f%%, FRT: %05.2f%%, CNT: %05.2f%%)",
		(cntfl[1]+cntfl[2]+cntfl[3]+cntfl[4]+cntfl[5])*pct, cntfl[1]*pct, cntfl[2]*pct, cntfl[3]*pct, cntfl[4]*pct, cntfl[5]*pct));
		
	print("Hills:        ", (cnthi[1]+cnthi[2]+cnthi[3]+cnthi[4]+cnthi[5]) .. "  ",
		string.format("%05.2f%% (DO: %05.2f%%, RO: %05.2f%%, ISL: %05.2f%%, FRT: %05.2f%%, CNT: %05.2f%%)",
		(cnthi[1]+cnthi[2]+cnthi[3]+cnthi[4]+cnthi[5])*pct, cnthi[1]*pct, cnthi[2]*pct, cnthi[3]*pct, cnthi[4]*pct, cnthi[5]*pct));
		
	print("Mountain:     ", (cntmn[1]+cntmn[2]+cntmn[3]+cntmn[4]+cntmn[5]) .. "  ",
		string.format("%05.2f%% (DO: %05.2f%%, RO: %05.2f%%, ISL: %05.2f%%, FRT: %05.2f%%, CNT: %05.2f%%)",
		(cntmn[1]+cntmn[2]+cntmn[3]+cntmn[4]+cntmn[5])*pct, cntmn[1]*pct, cntmn[2]*pct, cntmn[3]*pct, cntmn[4]*pct, cntmn[5]*pct));

	print("--")
	print(string.format("Deep Ocean plates:  %05.2f%% Land. Contributing to total Land Plots: ", 100*cntln[1]/(cntoc[1]+cntln[1])), cntln[1]) 
	print(string.format("Ocean plates:       %05.2f%% Land. Contributing to total Land Plots: ", 100*cntln[2]/(cntoc[2]+cntln[2])), cntln[2]) 
	print(string.format("Island plates:      %05.2f%% Land. Contributing to total Land Plots: ", 100*cntln[3]/(cntoc[3]+cntln[3])), cntln[3]) 
	print(string.format("Fractal plates:     %05.2f%% Land. Contributing to total Land Plots: ", 100*cntln[4]/(cntoc[4]+cntln[4])), cntln[4]) 
	print(string.format("Continental plates: %05.2f%% Land. Contributing to total Land Plots: ", 100*cntln[5]/(cntoc[5]+cntln[5])), cntln[5]) 
	print("--")
	
	-- For debugging: plate types
	local str = ""

	str =  str .. "type1 = cbind("
	for x = 0, mapWidth-1, 1 do
	
		str = str .. "c("
		for y = mapHeight-1, 0, -1 do
			local pl = plates[PartOfPlate(x,y)]
			if (pl.terrainType == "Deep Ocean") then
				str = str .. 1 .. ","
			elseif (pl.terrainType == "Ocean") then
				str = str .. 2 .. ","
			elseif (pl.terrainType == "Islands") then
				str = str .. 3 .. ","
			elseif (pl.terrainType == "Fractal") then
				str = str .. 4 .. ","
			else
				str = str .. 5 .. ","
			end
		end
		str = string.sub(str, 1, -2) .. "),\n"
	end
	str = string.sub(str, 1, -3) .. "); plot_map(type1, title='Plate Types')"

	print(str)
	
	return self.plotTypes;
end

function PartOfPlate(x, y)
	x = (x + xOffset) % mapWidth
	if x < 0 or y < 0 or x >= mapWidth or y >= mapHeight then
		return -1
	end
	
	return plateMap[(x-1)*mapHeight + y - 1]
end


function AddLakes()
	print("Map Generation - Adding Custom Lakes. Based on 'Random Scandinavia'");
	local iW, iH = Map.GetGridSize();
	local iNorth = math.floor(iH * 0.8);
	local iSouth = math.floor(iH * 0.2 + 0.99);
	
	-- Identify candidate plots.
	local lake_plot_candidate_list = {};
	for x = 0, iW - 1 do
		for y = iSouth, iNorth do
			local plot = Map.GetPlot(x, y);
			if not plot:IsWater() then
				if not plot:IsMountain() then
					if not plot:IsRiver() then
						if not plot:IsCoastalLand() then
							table.insert(lake_plot_candidate_list, {x, y});
						end
					end
				end
			end
		end
	end
	local max_possible_lakes = table.maxn(lake_plot_candidate_list);
	print(tostring(max_possible_lakes).." plots eligible for lakes.");
	if max_possible_lakes < 3 then
		return
	end
	
	-- Determine Number of Lakes to be added.
	local worldsizes = { -- Data is {Addition, Maprand} where the addition is added to a random number to reach a total.
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {3, 2},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {5, 3},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {7, 4},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {10, 5},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {16, 8},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {22, 11}
		}
	local temp_table = worldsizes[Map.GetWorldSize()];
	local addition = temp_table[1];
	local lake_rand = math.floor(Map.Rand(temp_table[2], "Lua WB Script - Lakes Random"));
	local iNumLakes = addition + lake_rand
	local numLakesAdded = 0;
	local firstRingYIsEven = {{0, 1}, {1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};
	local firstRingYIsOdd = {{1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, 0}, {0, 1}};
	
	iNumLakes = math.min(math.floor(iNumLakes * (1 + (rain-2)/3) ), max_possible_lakes) -- Make sure not to target more Lakes than are possible.
	print("Number of Lakes to add =", iNumLakes);
	
	--
	function ApplyHexAdjustment(x, y, plot_adjustments)
		local iW, iH = Map.GetGridSize();
		local adjusted_x, adjusted_y;
		if Map:IsWrapX() == true then
			adjusted_x = (x + plot_adjustments[1]) % iW;
		else
			adjusted_x = x + plot_adjustments[1];
		end
		if Map:IsWrapY() == true then
			adjusted_y = (y + plot_adjustments[2]) % iH;
		else
			adjusted_y = y + plot_adjustments[2];
		end
		return adjusted_x, adjusted_y;
	end
	--
	
	-- Create lake size table for the current map.
	local worldsizes = { -- Data is number of table entries for each lake size; size values (# of plots to the lake) range from 1 - 5.
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {11, 7, 2, 0, 0},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {11, 8, 3, 1, 0},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {11, 8, 3, 1, 0},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {11, 9, 4, 2, 1},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {11, 9, 4, 2, 1},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {11, 9, 4, 2, 1},
		}
	local plot_targets = worldsizes[Map.GetWorldSize()];
	local lake_size_table = {};
	for current_size = 1, 5 do
		local target = plot_targets[current_size];
		if target > 0 then
			for loop = 1, target do
				table.insert(lake_size_table, current_size);
			end
		end
	end
	local rand_target = table.maxn(lake_size_table);
	
	-- Build Lakes.
	local numDirections = DirectionTypes.NUM_DIRECTION_TYPES;
	for loop = 1, iNumLakes do
		local plotValid = true
		local lake_center_plot
		local center_plot
		
		local numRetries = 0
		repeat
			if (numRetries > 0) then
				print("Retry No. " .. numRetries)
			end
			local rand = 1 + Map.Rand(max_possible_lakes, "Lua WB Script - Choose Lake Center Plot");
			lake_center_plot = lake_plot_candidate_list[rand];
			center_plot = Map.GetPlot(lake_center_plot[1], lake_center_plot[2]);
			plotValid = true
			if center_plot:IsWater() then
				plotValid = false
			else
				for direction = 0, numDirections - 1, 1 do
					local adjacentPlot = Map.PlotDirection(center_plot:GetX(), center_plot:GetY(), direction);
					if (adjacentPlot ~= nil) then
						if (adjacentPlot:IsWater()) then
							plotValid = false
						end
					end
				end
			end
			numRetries = numRetries + 1
		until plotValid == true or numRetries >= 5
		if plotValid == true then
			-- Choose lake target size.
			local lake_size_rand = 1 + Map.Rand(rand_target, "Lua WB Script - Choose Lake Size");
			local lake_size = lake_size_table[lake_size_rand];
			-- If Lake target size is greater than 1 plot, proceed to identify wing plots.
			if lake_size > 1 then
				-- Identify candidate wing plots.
				local x, y = lake_center_plot[1], lake_center_plot[2];
				local wing_plot_candidate_list = {};
				local randomized_first_ring_adjustments = nil;
				local isEvenY = true;
				if y / 2 > math.floor(y / 2) then
					isEvenY = false;
				end
				if isEvenY then
					randomized_first_ring_adjustments = GetShuffledCopyOfTable(firstRingYIsEven);
				else
					randomized_first_ring_adjustments = GetShuffledCopyOfTable(firstRingYIsOdd);
				end
				for attempt = 1, 6 do
					local plot_adjustments = randomized_first_ring_adjustments[attempt];
					local searchX, searchY = ApplyHexAdjustment(x, y, plot_adjustments)
					local plot = Map.GetPlot(searchX, searchY);
					if not plot:IsWater() then
						if not plot:IsMountain() then
							if not plot:IsRiver() then
								local wingplotValid = true;
								for direction = 0, numDirections - 1, 1 do
									local adjacentPlot = Map.PlotDirection(plot:GetX(), plot:GetY(), direction);
									if (adjacentPlot ~= nil) then
										if (adjacentPlot:IsWater()) then
											wingplotValid = false
										end
									end
								end
								if wingplotValid == true then
									table.insert(wing_plot_candidate_list, {searchX, searchY});
								end
							end
						end
					end
				end
				local iNumWingCandidates = table.maxn(wing_plot_candidate_list);
				if iNumWingCandidates >= 1 then
					local iNumWings = math.min(lake_size - 1, iNumWingCandidates)
					-- Create Lake "wing" plots.
					for wing_loop = 1, iNumWings do
						local plot = Map.GetPlot(wing_plot_candidate_list[wing_loop][1], wing_plot_candidate_list[wing_loop][2])
						plot:SetPlotType(PlotTypes.PLOT_OCEAN, false, false);
						plot:SetTerrainType(TerrainTypes.TERRAIN_COAST, false, false);
					end
				end
			end
			-- Create Lake "center" plot.
			center_plot:SetPlotType(PlotTypes.PLOT_OCEAN, false, false);
			center_plot:SetTerrainType(TerrainTypes.TERRAIN_COAST, false, false);
			numLakesAdded = numLakesAdded + 1;
			print("Added lake #" .. loop .. " of size ", tostring(lake_size).." at plot ", lake_center_plot[1], lake_center_plot[2]);
		end
	end

	print(tostring(numLakesAdded).." lakes added.")

	Map.RecalculateAreas();
end

function FeatureGenerator:AddAtolls()
		local iNumAtollsPlaced = 0;
	local direction_types = {
		DirectionTypes.DIRECTION_NORTHEAST,
		DirectionTypes.DIRECTION_EAST,
		DirectionTypes.DIRECTION_SOUTHEAST,
		DirectionTypes.DIRECTION_SOUTHWEST,
		DirectionTypes.DIRECTION_WEST,
		DirectionTypes.DIRECTION_NORTHWEST
	};
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 8,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 10,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 14,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 20,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 30,
	};
	local atoll_target = worldsizes[Map.GetWorldSize()];
	local atoll_number = atoll_target + Map.Rand(atoll_target, "Number of Atolls to place - LUA");
	
	local feature_atoll;
	for thisFeature in GameInfo.Features() do
		if thisFeature.Type == "FEATURE_ATOLL" then
			feature_atoll = thisFeature.ID;
		end
	end

	local iW, iH = Map.GetGridSize()
	
	local temp_one_tile_island_list, temp_alpha_list, temp_beta_list = {}, {}, {};
	local temp_gamma_list, temp_delta_list, temp_epsilon_list = {}, {}, {};
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = y * iW + x + 1; -- Lua tables/lists/arrays start at 1, not 0 like C++ or Python
			local plot = Map.GetPlot(x, y)
			local plotType = plot:GetPlotType()
			if plotType == PlotTypes.PLOT_OCEAN then
				local latitude = math.abs((y - iH/2)/iH * 180)
				if latitude < 25 then
					local featureType = plot:GetFeatureType()
					if featureType ~= FeatureTypes.FEATURE_ICE then
						if not plot:IsLake() then
							local terrainType = plot:GetTerrainType()
							if terrainType == TerrainTypes.TERRAIN_COAST then
								if plot:IsAdjacentToLand() then
									-- Check all adjacent plots and identify adjacent landmasses.
									local iNumLandAdjacent, biggest_adj_area = 0, 0;
									for loop, direction in ipairs(direction_types) do
										local adjPlot = Map.PlotDirection(x, y, direction)
										if adjPlot ~= nil then
											local adjPlotType = adjPlot:GetPlotType()
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
									if iNumLandAdjacent == 1 then
										if biggest_adj_area >= 76 then
											-- discard this site
										elseif biggest_adj_area >= 41 then
											table.insert(temp_epsilon_list, i);
										elseif biggest_adj_area >= 17 then
											table.insert(temp_delta_list, i);
										elseif biggest_adj_area >= 8 then
											table.insert(temp_gamma_list, i);
										elseif biggest_adj_area >= 3 then
											table.insert(temp_beta_list, i);
										elseif biggest_adj_area >= 1 then
											table.insert(temp_alpha_list, i);
										--else -- Unexpected result
											--print("** Area Plot Count =", biggest_adj_area);
										end
									end
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
	local max_alpha = math.ceil(table.maxn(alpha_list) / 4);
	local max_beta = math.ceil(table.maxn(beta_list) / 5);
	local max_gamma = math.ceil(table.maxn(gamma_list) / 4);
	local max_delta = math.ceil(table.maxn(delta_list) / 3);
	local max_epsilon = math.ceil(table.maxn(epsilon_list) / 4);
	
	-- Place Atolls.
	local plotIndex;
	local i_alpha, i_beta, i_gamma, i_delta, i_epsilon = 1, 1, 1, 1, 1;
	for loop = 1, atoll_number do
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
		if able_to_proceed and plotIndex ~= nil then
			local x = (plotIndex - 1) % iW;
			local y = (plotIndex - x - 1) / iW;
			local plot = Map.GetPlot(x, y)
			plot:SetFeatureType(feature_atoll, -1);
			iNumAtollsPlaced = iNumAtollsPlaced + 1;
		--else
			--print("** ERROR ** Atoll unable to be placed and/or chosen Plot Index was nil.");
		end
	end
	
end

function CachePartOfPlate(x, y)
	yShift = skewFactor[1]*math.sin(x/skewFactor[4]+skewFactor[7]) + skewFactor[2]*math.sin(x/skewFactor[5]+skewFactor[8]) + skewFactor[3]*math.sin(x/skewFactor[6]+skewFactor[9]);
	xShift = skewFactor[1]*math.sin(y/skewFactor[5]+skewFactor[9]) + skewFactor[2]*math.sin(y/skewFactor[6]+skewFactor[7]) + skewFactor[3]*math.sin(y/skewFactor[4]+skewFactor[8]);

	yFuddle = math.pow((0.5*math.sin(x/skewFactor2[1]+skewFactor2[5])+0.5*math.sin(x/skewFactor2[2]+skewFactor2[6])),2)*(math.sin(x*skewFactor2[3]+skewFactor2[7])+math.sin(x*skewFactor2[4]+skewFactor2[8]));
	xFuddle = math.pow((0.5*math.sin(y/skewFactor2[1]+skewFactor2[6])+0.5*math.sin(y/skewFactor2[2]+skewFactor2[7])),2)*(math.sin(y*skewFactor2[3]+skewFactor2[8])+math.sin(y*skewFactor2[4]+skewFactor2[5]));

	xAdj = x + xShift+xFuddle;
	yAdj = y + yShift+yFuddle;

	local distanceToPlate = {};
	for plateIndex, thisPlate in ipairs(plates) do
		-- convert distances to latitute/longitude, then use formula for distance calculations on spheres
		local lat1 = math.pi * (yAdj - mapHeight/2) / (mapHeight + 1)
		local lat2 = math.pi * (thisPlate.y - mapHeight/2) / (mapHeight + 1)
		local lon1 = 2 * math.pi * xAdj / (mapWidth)
		local lon2 = 2 * math.pi * thisPlate.x / (mapWidth)
		distanceToPlate[plateIndex] = math.acos(math.min(1, math.max(-1, math.sin(lat1)*math.sin(lat2)+math.cos(lat1)*math.cos(lat2)*math.cos(lon2-lon1))))/thisPlate.mass
	end

	local nearestPlate = 10;
	local nearestPlateDist = 10000;
	for distIndex, thisDiscance in ipairs(distanceToPlate) do
		if (thisDiscance < nearestPlateDist) then
			nearestPlate = distIndex;
			nearestPlateDist = thisDiscance;
		end
	end
	
	plateMap[(x-1)*mapHeight + y - 1] = nearestPlate;
end

function AdjacentToPlate(x,y)
	currentPlate = PartOfPlate(x,y);
	if (IsAtPlateBoundary(x, y,currentPlate)) then
		info = GetPlateBoundaryInfo(x, y);
		biggest = 0;
		biggestIndex = -1;
		for infoIndex, thisInfo in ipairs(info) do
			if (infoIndex ~= currentPlate and thisInfo > biggest) then
				biggest = thisInfo;
				biggestIndex = infoIndex
			end
		end
		return biggestIndex;
	else
		return -1;
	end
end

function GetPlateBoundaryInfo(x, y)
	local info = {};
	for plateIndex, thisPlate in ipairs(plates) do
		info[plateIndex] = 0;
	end

	local index1 = PartOfPlate((x-1+(y%2))%mapWidth, y+1);
	if (info[index1] ~= nil) then
		info[index1] = info[index1]+1;
	end
	local index2 = PartOfPlate((x+(y%2))%mapWidth, y+1);
	if (info[index2] ~= nil) then
		info[index2] = info[index2]+1;
	end
	local index3 = PartOfPlate((x-1)%mapWidth, y);
	if (info[index3] ~= nil) then
		info[index3] = info[index3]+1;
	end
	local index4 = PartOfPlate((x+1)%mapWidth, y);
	if (info[index4] ~= nil) then
		info[index4] = info[index4]+1;
	end
	local index5 = PartOfPlate((x-1+(y%2))%mapWidth, y-1);
	if (info[index5] ~= nil) then
		info[index5] = info[index5]+1;
	end
	local index6 = PartOfPlate((x+(y%2))%mapWidth, y-1);
	if (info[index6] ~= nil) then
		info[index6] = info[index6]+1;
	end
	return info;
end

function NumAdjacentLandTiles(x, y, n, f, s, d)
	if (x < 0 or y < 0 or x >= mapWidth or y >= mapHeight) then return 0 end

	adjacentLandTiles = 0;
	if (y < mapHeight - 1) then
		height = GetHeight((x-1+(y%2))%mapWidth, y+1, n, f) + d;
		if (height > 0 + s) then
			adjacentLandTiles = adjacentLandTiles+1;
		end
		height = GetHeight((x+(y%2))%mapWidth, y+1, n, f) + d;
		if (height > 0 + s) then
			adjacentLandTiles = adjacentLandTiles+1;
		end
	end
	height = GetHeight((x-1)%mapWidth, y, n, f) + d;
	if (height > 0 + s) then
		adjacentLandTiles = adjacentLandTiles+1;
	end
	height = GetHeight((x+1)%mapWidth, y, n, f) + d;
	if (height > 0 + s) then
		adjacentLandTiles = adjacentLandTiles+1;
	end
	if (y > 1) then
		height = GetHeight((x-1+(y%2))%mapWidth, y-1, n, f) + d;
		if (height > 0 + s) then
			adjacentLandTiles = adjacentLandTiles+1;
		end
		height = GetHeight((x+(y%2))%mapWidth, y-1, n, f) + d;
		if (height > 0 + s) then
			adjacentLandTiles = adjacentLandTiles+1;
		end
	end

	return adjacentLandTiles;
end

function IsAtPlateBoundary(x, y,currentPlate)

	if (PartOfPlate((x-1+(y%2))%mapWidth, y+1) ~= currentPlate) then
		return true;
	end
	if (PartOfPlate((x+(y%2))%mapWidth, y+1) ~= currentPlate) then
		return true;
	end
	if (PartOfPlate((x-1)%mapWidth, y) ~= currentPlate) then
		return true;
	end
	if (PartOfPlate((x+1)%mapWidth, y) ~= currentPlate) then
		return true;
	end
	if (PartOfPlate(x-1+(y%2)%mapWidth, y-1) ~= currentPlate) then
		return true;
	end
	if (PartOfPlate((x+(y%2))%mapWidth, y-1) ~= currentPlate) then
		return true;
	end

	return false;
end

function GenerateCoasts(args)
	print("Setting coasts and oceans (MapGenerator.Lua)");
	local args = args or {};
	local bExpandCoasts = args.bExpandCoasts or true;
	local expansion_diceroll_table = args.expansion_diceroll_table or {4, 4};

	local shallowWater = GameDefines.SHALLOW_WATER_TERRAIN;
	local deepWater = GameDefines.DEEP_WATER_TERRAIN;

	for i, plot in Plots() do
		if(plot:IsWater()) then
			if(plot:IsAdjacentToLand()) then
				plot:SetTerrainType(shallowWater, false, false);
			else
				plot:SetTerrainType(deepWater, false, false);
			end
		end
	end

	if bExpandCoasts == false then
		return
	end
	
	print("Expanding coasts (MapGenerator.Lua)");
	for loop, iExpansionDiceroll in ipairs(expansion_diceroll_table) do
		local shallowWaterPlots = {};
		for i, plot in Plots() do
			if(plot:GetTerrainType() == deepWater and plates[PartOfPlate(plot:GetX(), plot:GetY())].terrainType ~= "Deep Ocean") then
				-- Chance for each eligible plot to become an expansion is 1 / iExpansionDiceroll.
				-- Default is two passes at 1/4 chance per eligible plot on each pass.
				if(plot:IsAdjacentToShallowWater() and Map.Rand(iExpansionDiceroll, "add shallows") == 0) then
					table.insert(shallowWaterPlots, plot);
				end
			end
		end
		for i, plot in ipairs(shallowWaterPlots) do
			plot:SetTerrainType(shallowWater, false, false);
		end
	end
end

function GeneratePlotTypes()

	local fractal_world = FractalWorld.Create();
	fractal_world:InitFractal{
		continent_grain = 4};

	local plotTypes = fractal_world:GeneratePlotTypes(args);

	SetPlotTypes(plotTypes);
	
	print("Removing Lakes")
	Map.RecalculateAreas();
	for x = 0, mapWidth-1, 1 do
		for y = 0, mapHeight-1, 1 do
			local plot = Map.GetPlot(x, y);
			if plot:IsLake() then
				plot:SetPlotType(PlotTypes.PLOT_LAND)
				print("Removing lake at " .. plot:GetX() .. " " .. plot:GetY())
			end
		end
	end

	GenerateCoasts(args);
end

function GenerateTerrain()
	print("Generating Terrain ...");

	-- Get Temperature setting input by user.
	local temp = Map.GetCustomOption(2)
	if temp == 4 then
		temp = 1 + Map.Rand(2, "Random Temperature - Lua") + Map.Rand(2, "Random Temperature - Lua 2");
	end
	if (temp == 1) then
		print("Temperature option:  (1) Cool");
	elseif (temp == 2) then
		print("Temperature option:  (2) Temperate");
	else
		print("Temperature option:  (3) Hot");
	end
	
	local args = {
		temperature = temp
		};
	local terraingen = TerrainGenerator.Create(args);

	terrainTypes = terraingen:GenerateTerrain();

	SetTerrainTypes(terrainTypes);
	Map.RecalculateAreas();
	
	-- For debugging: terrain types
	str = "type2 = cbind("
	for x = 0, mapWidth-1, 1 do
	
		str = str .. "c("
		for y = mapHeight-1, 0, -1 do
			local plot = Map.GetPlot(x,y)
			if(plot:IsMountain()) then
				str = str .. 5 .. ","
			elseif(plot:IsHills()) then
				str = str .. 4 .. ","
			elseif(plot:GetTerrainType() == GameDefines.DEEP_WATER_TERRAIN) then
				str = str .. 1 .. ","
			elseif(plot:GetTerrainType() == GameDefines.SHALLOW_WATER_TERRAIN) then
				str = str .. 2 .. ","
			else
				str = str .. 3 .. ","
			end
			
		end
		str = string.sub(str, 1, -2) .. "),\n"
	end
	str = string.sub(str, 1, -3) .. "); plot_map(type2, title='Terrain')"

	print(str)
	
	--[[str = "type3 = cbind("
	for x = 0, mapWidth-1, 1 do
	
		str = str .. "c("
		for y = mapHeight-1, 0, -1 do
			local plot = Map.GetPlot(x,y)
			str = str .. plot:GetContinent() .. ","
		end
		str = string.sub(str, 1, -2) .. "),\n"
	end
	str = string.sub(str, 1, -3) .. "); plot_map(type3, title='Continents')"
	print(str)--]]--
	
	str = "type4 = cbind("
	for x = 0, mapWidth-1, 1 do
	
		str = str .. "c("
		for y = mapHeight-1, 0, -1 do
			local plot = Map.GetPlot(x,y)
			str = str .. plot:GetLandmass() .. ","
		end
		str = string.sub(str, 1, -2) .. "),\n"
	end
	str = string.sub(str, 1, -3) .. "); plot_map(type4, title='Landmasses')"
	print(str)
	
	str = "type5 = cbind("
	for x = 0, mapWidth-1, 1 do
	
		str = str .. "c("
		for y = mapHeight-1, 0, -1 do
			local plot = Map.GetPlot(x,y)
			str = str .. plot:GetArea() .. ","
		end
		str = string.sub(str, 1, -2) .. "),\n"
	end
	str = string.sub(str, 1, -3) .. "); plot_map(type5, title='Areas')"

	print(str)
end
------------------------------------------------------------------------------
-- Modified to reduce the amount of ice generated.
--
function FeatureGenerator:AddIceAtPlot(plot, iX, iY, lat)
	if(plot:CanHaveFeature(self.featureIce)) then
		if Map.IsWrapX() and (iY == 0 or iY == self.iGridH - 1) then
			plot:SetFeatureType(self.featureIce, -1)
		elseif Map.IsWrapX() and (iY == 1 or iY == self.iGridH - 2) then
			local rand = Map.Rand(100, "Add Ice Lua");
			if (rand <= 70) then
				plot:SetFeatureType(self.featureIce, -1);
			end
		elseif Map.IsWrapX() and (iY == 2 or iY == self.iGridH - 3) then
			local rand = Map.Rand(100, "Add Ice Lua");
			if (rand <= 40) then
				plot:SetFeatureType(self.featureIce, -1);
			end
		elseif Map.IsWrapX() and (iY == 3 or iY == self.iGridH - 4) then
			if plot:IsAdjacentToLand() == false then
				local rand = Map.Rand(100, "Add Ice Lua");
				if (rand <= 20) then
					plot:SetFeatureType(self.featureIce, -1);
				end
			end
		end
	end
end
------------------------------------------------------------------------------
-- Modified to make jungles and forests slightly less clumpy.
--
function AddFeatures()
	print("Adding Features ...");

	local args = {
		rainfall = rain,
		jungle_grain = 8,
		forest_grain = 8,
		clump_grain = 4,
		iJungleFactor = 6,
		iJunglePercent = 67,
		iForestPercent = 45,
		fMarshPercent = 18,
		iOasisPercent = 15
		};
	local featuregen = FeatureGenerator.Create(args);

	-- False parameter removes mountains from coastlines.
	featuregen:AddFeatures(true);
	
	local numatolls = 0
	for x = 0, mapWidth-1, 1 do
		for y = mapHeight-1, 0, -1 do
			local plot = Map.GetPlot(x,y)
			if(plot:GetFeatureType() == FeatureTypes.FEATURE_ATOLL) then
				numatolls = numatolls + 1
			end
		end
	end
	print("Number of Atolls", numatolls)
end

------------------------------------------------------------------------------
function StartPlotSystem()
	-- Get Resources setting input by user.
	local res = Map.GetCustomOption(5)
	if res == 6 then
		res = 1 + Map.Rand(5, "Random Resources Option - Lua");
	end

	print("Creating start plot database.");
	local start_plot_database = AssignStartingPlots.Create()

	local startPlacement = Map.GetCustomOption(16)
	local divMethod = nil
	if startPlacement == 4 then
		divMethod = 1 + Map.Rand(3, "Random Start Placement");
	end
	if startPlacement == 1 then			-- Biggest Landmass
		divMethod = 1
	elseif startPlacement == 2 then		-- Any Continents
		divMethod = 2
	elseif startPlacement == 3 then		-- Start Anywhere
		divMethod = 3
	else								-- Fallback
		divMethod = 2
	end
	print("Start placement method:						", startPlacement);

	local largestLand = Map.FindBiggestArea(false)
	if startPlacement == 1 then
		-- Biggest Landmass placement
		if largestLand:GetNumTiles() < 0.25 * Map.GetLandPlots() then
			print("AI Map Strategy - Offshore expansion with navy bias")
			-- Tell the AI that we should treat this as a offshore expansion map with naval bias
			Map.ChangeAIMapHint(4+1)
		else
			print("AI Map Strategy - Offshore expansion")
			-- Tell the AI that we should treat this as a offshore expansion map
			Map.ChangeAIMapHint(4)
		end
	elseif startPlacement == 2 then
		-- Any Continents placement
		if largestLand:GetNumTiles() < 0.25 * Map.GetLandPlots() then
			print("AI Map Strategy - Offshore expansion with navy bias")
			-- Tell the AI that we should treat this as a offshore expansion map with naval bias
			Map.ChangeAIMapHint(4+1)
		else
			print("AI Map Strategy - Normal")
		end
	elseif startPlacement == 3 then
		-- Start Anywhere placement
		if largestLand:GetNumTiles() < 0.25 * Map.GetLandPlots() then
			print("AI Map Strategy - Navy bias")
			-- Tell the AI that we should treat this as a map with naval bias
			Map.ChangeAIMapHint(1)
		else
			print("AI Map Strategy - Normal")
		end
	else
		print("AI Map Strategy - Normal")
	end

	print("Dividing the map into Regions.");
	-- Regional Division Method 2: Continental
	local args = {
		method = divMethod,
		resources = res,
		};
	start_plot_database:GenerateRegions(args)

	print("Choosing start locations for civilizations.");
	-- Forcing starts along the ocean.
	local coastalStarts = Map.GetCustomOption(15);
	if coastalStarts == 3 then
		coastalStarts = 1 + Map.Rand(2, "Random Coastal Starts");
	end
	print("Coastal Starts Option", coastalStarts);
	local args = {mustBeCoast = (coastalStarts == 2) };
	start_plot_database:ChooseLocations(args)

	print("Normalizing start locations and assigning them to Players.");
	start_plot_database:BalanceAndAssign()

	print("Placing Natural Wonders.");
	start_plot_database:PlaceNaturalWonders()

	print("Placing Resources and City States.");
	start_plot_database:PlaceResourcesAndCityStates()	
end
--]]--