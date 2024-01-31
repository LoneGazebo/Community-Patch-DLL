------------------------------------------------------------------------------
--	FILE:	 West_vs_East.lua
--	AUTHOR:  Bob Thomas
--	PURPOSE: Regional map script - Designed to pit two teams against each other
--	         with a strip of water dividing the map east from west.
------------------------------------------------------------------------------
--	Copyright (c) 2010 Firaxis Games, Inc. All rights reserved.
------------------------------------------------------------------------------

include("MapGenerator");
include("MultilayeredFractal");
include("FeatureGenerator");
include("TerrainGenerator");

local iNumTeams;
local teamWestID, teamEastID;
local iNumCivsInWest, iNumCivsInEast;

------------------------------------------------------------------------------
function GetMapScriptInfo()
	local world_age, temperature, rainfall, _, _ = GetCoreMapOptions();
	return {
		Name = "TXT_KEY_MAP_WEST_VS_EAST_VP",
		Description = "TXT_KEY_MAP_WEST_VS_EAST_HELP",
		SupportsMultiplayer = true,
		IconIndex = 18,
		CustomOptions = {
			world_age,
			temperature,
			rainfall,
			{
				Name = "TXT_KEY_MAP_OPTION_RESOURCES", -- Customizing the Resource setting to Default to Strategic Balance.
				Values = {
					"TXT_KEY_MAP_OPTION_SPARSE",
					"TXT_KEY_MAP_OPTION_STANDARD",
					"TXT_KEY_MAP_OPTION_ABUNDANT",
					"TXT_KEY_MAP_OPTION_LEGENDARY_START",
					"TXT_KEY_MAP_OPTION_STRATEGIC_BALANCE",
					"TXT_KEY_MAP_OPTION_RANDOM",
				},
				DefaultValue = 5,
				SortPriority = -95,
			},
			{
				Name = "TXT_KEY_MAP_OPTION_TEAM_SETTING",
				Values = {
					"TXT_KEY_MAP_OPTION_START_TOGETHER",
					"TXT_KEY_MAP_OPTION_START_ANYWHERE",
				},
				DefaultValue = 1,
				SortPriority = 1,
			},
		},
	}
end
------------------------------------------------------------------------------

-------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	-- This function can reset map grid sizes or world wrap settings.

	-- East vs West is an extremely compact multiplayer map type.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {20, 14},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {30, 20},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {36, 24},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {44, 28},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {52, 32},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {64, 40},
	};
	local grid_size = worldsizes[worldSize];

	if GameInfo.Worlds[worldSize] then
		return {
			Width = grid_size[1],
			Height = grid_size[2],
			WrapX = false,
		};
	end
end
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
function MultilayeredFractal:GeneratePlotsByRegion()
	-- Sirian's MultilayeredFractal controlling function.
	-- You -MUST- customize this function for each script using MultilayeredFractal.

	-- This implementation is specific to West vs East.
	local iW, iH = Map.GetGridSize();
	local fracFlags = {};

	-- Fill all rows with land plots.
	self.wholeworldPlotTypes = table.fill(PlotTypes.PLOT_LAND, iW * iH);

	-- Add strip of ocean to middle of map.
	for y = 0, iH - 1 do
		for x = math.floor(iW / 2) - 2, math.floor(iW / 2) + 1 do
			local plotIndex = y * iW + x + 1;
			if y >= math.floor(iH / 2) - 2 and y <= math.floor(iH / 2) + 1 then
				if x == math.floor(iW / 2) or x == math.floor(iW / 2) - 1 then
					self.wholeworldPlotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
				end
			else
				self.wholeworldPlotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
			end
		end
	end

	-- Add lakes.
	local lakesFrac = Fractal.Create(iW, iH, 0, fracFlags, 6, 6);
	local iLakesThreshold = lakesFrac:GetHeight(92);
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = y * iW + x + 1; -- add one because Lua arrays start at 1
			local lakeVal = lakesFrac:GetHeight(x, y);
			if lakeVal >= iLakesThreshold then
				self.wholeworldPlotTypes[i] = PlotTypes.PLOT_OCEAN;
			end
		end
	end

	-- Land and water are set. Now apply hills and mountains.
	local world_age = Map.GetCustomOption(1);
	if world_age == 4 then
		world_age = 1 + Map.Rand(3, "Random World Age - Lua");
	end
	local args = {world_age = world_age};
	self:ApplyTectonics(args);

	-- Plot Type generation completed. Return global plot array.
	return self.wholeworldPlotTypes;
end
------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Setting Plot Types (Lua West vs East) ...");

	local layered_world = MultilayeredFractal.Create();
	local plot_list = layered_world:GeneratePlotsByRegion();

	SetPlotTypes(plot_list);

	GenerateCoasts{bExpandCoasts = false};
end
----------------------------------------------------------------------------------

----------------------------------------------------------------------------------
function TerrainGenerator:GetLatitudeAtPlot(iX, iY)
	local lat = math.abs(self.iHeight / 2 - iY) / (self.iHeight / 2);
	lat = lat + (128 - self.variation:GetHeight(iX, iY)) / (255.0 * 5.0);
	lat = 0.8 * math.clamp(lat, 0, 1);
	return lat;
end
----------------------------------------------------------------------------------
function GenerateTerrain()
	print("Generating Terrain (Lua West vs East) ...");

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

------------------------------------------------------------------------------
function GetRiverValueAtPlot(plot)
	-- Custom method to force rivers to flow toward the map center.
	local iW, iH = Map.GetGridSize();
	local x = plot:GetX();
	local y = plot:GetY();
	local random_factor = Map.Rand(3, "River direction random factor - West vs East LUA");
	local direction_influence_value = (math.abs(x - iW / 2) + math.abs(y - iH / 2)) * random_factor;

	local numPlots = PlotTypes.NUM_PLOT_TYPES;
	local sum = (numPlots - plot:GetPlotType()) * 20 + direction_influence_value;

	local numDirections = DirectionTypes.NUM_DIRECTION_TYPES;
	for direction = 0, numDirections - 1 do
		local adjacentPlot = Map.PlotDirection(plot:GetX(), plot:GetY(), direction);
		if adjacentPlot then
			sum = sum + (numPlots - adjacentPlot:GetPlotType());
		else
			sum = sum + (numPlots * 10);
		end
	end
	sum = sum + Map.Rand(10, "River Rand");

	return sum;
end
------------------------------------------------------------------------------
function AddRivers()
	-- Customization for West vs East, to keep river starts away from map edges and set river "original flow direction".
	local iW, iH = Map.GetGridSize();
	print("West vs East - Adding Rivers");
	local passConditions = {
		function (plot)
			return plot:IsHills() or plot:IsMountain();
		end,

		function (plot)
			return not plot:IsCoastalLand() and Map.Rand(8, "AddRivers Lua") == 0;
		end,

		function (plot)
			local area = plot:Area();
			local plotsPerRiverEdge = GameDefines.PLOTS_PER_RIVER_EDGE;
			return (plot:IsHills() or plot:IsMountain()) and area:GetNumRiverEdges() < area:GetNumTiles() / plotsPerRiverEdge + 1;
		end,

		function (plot)
			local area = plot:Area();
			local plotsPerRiverEdge = GameDefines.PLOTS_PER_RIVER_EDGE;
			return area:GetNumRiverEdges() < area:GetNumTiles() / plotsPerRiverEdge + 1;
		end,
	};
	for iPass, passCondition in ipairs(passConditions) do
		local riverSourceRange;
		local seaWaterRange;
		if iPass <= 2 then
			riverSourceRange = GameDefines.RIVER_SOURCE_MIN_RIVER_RANGE;
			seaWaterRange = GameDefines.RIVER_SOURCE_MIN_SEAWATER_RANGE;
		else
			riverSourceRange = GameDefines.RIVER_SOURCE_MIN_RIVER_RANGE / 2;
			seaWaterRange = GameDefines.RIVER_SOURCE_MIN_SEAWATER_RANGE / 2;
		end
		for _, plot in Plots() do
			local current_x = plot:GetX();
			local current_y = plot:GetY();
			if current_x < 1 or current_x >= iW - 2 or current_y < 2 or current_y >= iH - 1 then
				-- Plot too close to edge, ignore it.
			elseif not plot:IsWater() then
				if passCondition(plot) then
					if not Map.FindWater(plot, riverSourceRange, true) then
						if not Map.FindWater(plot, seaWaterRange, false) then
							local inlandCorner = plot:GetInlandCorner();
							if inlandCorner then
								local start_x = inlandCorner:GetX();
								local start_y = inlandCorner:GetY();
								local orig_direction;
								if start_y < iH / 2 then -- South half of map
									if start_x < iW / 2 then -- SW Corner
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_NORTHEAST;
									else -- SE Corner
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_NORTHWEST;
									end
								else -- North half of map
									if start_x < iW / 2 then -- NW corner
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST;
									else -- NE corner
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST;
									end
								end
								DoRiver(inlandCorner, nil, orig_direction, nil);
							end
						end
					end
				end
			end
		end
	end
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function FeatureGenerator:GetLatitudeAtPlot(_, iY)
	local lat = math.abs(self.iGridH / 2 - iY) / (self.iGridH / 2);
	local adjusted_lat = 0.8 * lat;
	return adjusted_lat;
end
------------------------------------------------------------------------------
function FeatureGenerator:AddIceAtPlot()
	-- No ice on this map.
end
------------------------------------------------------------------------------
function AddFeatures()
	print("Adding Features (Lua West vs East) ...");

	-- Get Rainfall setting input by user.
	local rain = Map.GetCustomOption(3);
	if rain == 4 then
		rain = 1 + Map.Rand(3, "Random Rainfall - Lua");
	end

	local featuregen = FeatureGenerator.Create{rainfall = rain};

	featuregen:AddFeatures(false);
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function AssignStartingPlots:GenerateRegions(args)
	print("Map Generation - Dividing the map in to Regions");
	-- This is a customized version for West vs East.
	-- This version is tailored for handling two-teams play.
	args = args or {};
	local iW, iH = Map.GetGridSize();
	self.method = RegionDivision.RECTANGULAR; -- Flag the map as using a Rectangular division method.
	self.resDensity = args.resources; -- Strategic Resource Density setting
	self.resSize = args.resources; -- Strategic Resource Deposit Size setting
	self.bonusDensity = args.resources; -- Bonus Resource Density setting
	self.luxuryDensity = args.resources; -- Luxury Resource Density setting
	self.legStart = args.resources == 4; -- Legendary Start setting
	self.resBalance = args.resources == 5; -- Strategic Balance setting

	-- Determine number of civilizations and city states present in this game.
	self.iNumCivs, self.iNumCityStates, self.player_ID_list, self.bTeamGame, self.teams_with_major_civs, self.number_civs_per_team = GetPlayerAndTeamInfo();
	self.iNumCityStatesUnassigned = self.iNumCityStates;
	print("-"); print("Civs:", self.iNumCivs); print("City States:", self.iNumCityStates);

	-- Determine number of teams (of Major Civs only, not City States) present in this game.
	iNumTeams = table.maxn(self.teams_with_major_civs); -- GLOBAL
	print("-"); print("Teams:", iNumTeams);

	-- Fetch team setting.
	local team_setting = Map.GetCustomOption(5);

	-- If two teams are present, use team-oriented handling of start points, one team west, one east.
	if iNumTeams == 2 and team_setting == 1 then
		print("-"); print("Number of Teams present is two! Using custom team start placement for West vs East."); print("-");

		-- ToDo: Correctly identify team IDs and how many Civs are on each team.
		-- Also need to shuffle the teams so its random who starts on which half.
		local shuffled_team_list = GetShuffledCopyOfTable(self.teams_with_major_civs);
		teamWestID = shuffled_team_list[1];
		teamEastID = shuffled_team_list[2];
		iNumCivsInWest = self.number_civs_per_team[teamWestID];
		iNumCivsInEast = self.number_civs_per_team[teamEastID];

		-- Process the team in the west.
		self.inhabited_WestX = 0;
		self.inhabited_SouthY = 0;
		self.inhabited_Width = math.floor(iW / 2) - 1;
		self.inhabited_Height = iH;
		-- Obtain "Start Placement Fertility" inside the rectangle.
		-- Data returned is: fertility table, sum of all fertility, plot count.
		local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityInRectangle(
			self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height);
		-- Assemble the Rectangle data table. AreaID -1 means ignore landmass/area IDs.
		local rect_table = {self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height, -1, fertCount, plotCount};
		-- Divide the rectangle.
		self:DivideIntoRegions(iNumCivsInWest, fert_table, rect_table);

		-- Process the team in the east.
		self.inhabited_WestX = math.floor(iW / 2) + 1;
		self.inhabited_SouthY = 0;
		self.inhabited_Width = math.floor(iW / 2) - 1;
		self.inhabited_Height = iH;
		-- Obtain "Start Placement Fertility" inside the rectangle.
		-- Data returned is: fertility table, sum of all fertility, plot count.
		fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityInRectangle(
			self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height);
		-- Assemble the Rectangle data table. AreaID -1 means ignore landmass/area IDs.
		rect_table = {self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height, -1, fertCount, plotCount};
		-- Divide the rectangle.
		self:DivideIntoRegions(iNumCivsInEast, fert_table, rect_table);

	else -- Use standard division.
		-- Obtain the boundaries of the rectangle to be processed.
		self.inhabited_WestX = 0;
		self.inhabited_SouthY = 0;
		self.inhabited_Width = iW;
		self.inhabited_Height = iH;

		-- Obtain "Start Placement Fertility" inside the rectangle.
		-- Data returned is: fertility table, sum of all fertility, plot count.
		local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityInRectangle(self.inhabited_WestX,
												 self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height);
		-- Assemble the Rectangle data table:
		local rect_table = {self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width,
							self.inhabited_Height, -1, fertCount, plotCount}; -- AreaID -1 means ignore landmass/area IDs.
		-- Divide the rectangle.
		self:DivideIntoRegions(self.iNumCivs, fert_table, rect_table);
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:BalanceAndAssign()
	-- This function determines what level of Bonus Resource support a location may need, identifies compatibility with civ-specific biases, and places starts.

	-- Normalize each start plot location.
	local iNumStarts = table.maxn(self.startingPlots);
	for region_number = 1, iNumStarts do
		self:NormalizeStartLocation(region_number);
	end

	-- Assign Civs to start plots.
	local team_setting = Map.GetCustomOption(5);
	if iNumTeams == 2 and team_setting == 1 then
		-- Two teams, place one in the west half, other in east -- even if team membership totals are uneven.
		print("-"); print("This is a team game with two teams! Place one team in West, other in East."); print("-");
		local playerList, westList, eastList = {}, {}, {};
		for loop = 1, self.iNumCivs do
			local player_ID = self.player_ID_list[loop];
			table.insert(playerList, player_ID);
			local player = Players[player_ID];
			local team_ID = player:GetTeam();
			if team_ID == teamWestID then
				print("Player #", player_ID, "belongs to Team #", team_ID, "and will be placed in the North.");
				table.insert(westList, player_ID);
			elseif team_ID == teamEastID then
				print("Player #", player_ID, "belongs to Team #", team_ID, "and will be placed in the South.");
				table.insert(eastList, player_ID);
			else
				print("* ERROR * - Player #", player_ID, "belongs to Team #", team_ID, "which is neither West nor East!");
			end
		end

		---[[ Debug
		if table.maxn(westList) ~= iNumCivsInWest then
			print("-"); print("*** ERROR! *** . . . Mismatch between number of Civs on West team and number of civs assigned to west locations.");
		end
		if table.maxn(eastList) ~= iNumCivsInEast then
			print("-"); print("*** ERROR! *** . . . Mismatch between number of Civs on East team and number of civs assigned to east locations.");
		end
		--]]

		local westListShuffled = GetShuffledCopyOfTable(westList);
		local eastListShuffled = GetShuffledCopyOfTable(eastList);
		for region_number, player_ID in ipairs(westListShuffled) do
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];
			local start_plot = Map.GetPlot(x, y);
			local player = Players[player_ID];
			player:SetStartingPlot(start_plot);
		end
		for loop, player_ID in ipairs(eastListShuffled) do
			local x = self.startingPlots[loop + iNumCivsInWest][1];
			local y = self.startingPlots[loop + iNumCivsInWest][2];
			local start_plot = Map.GetPlot(x, y);
			local player = Players[player_ID];
			player:SetStartingPlot(start_plot);
		end
	else
		print("-"); print("This game does not have specific start zone assignments."); print("-");
		local playerList = {};
		for loop = 1, self.iNumCivs do
			local player_ID = self.player_ID_list[loop];
			table.insert(playerList, player_ID);
		end
		local playerListShuffled = GetShuffledCopyOfTable(playerList);
		for region_number, player_ID in ipairs(playerListShuffled) do
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];
			local start_plot = Map.GetPlot(x, y);
			local player = Players[player_ID];
			player:SetStartingPlot(start_plot);
		end

		-- If this is a team game (any team has more than one Civ in it) then make sure team members start near each other if possible.
		-- (This may scramble Civ biases in some cases, but there is no cure).
		if self.bTeamGame and team_setting ~= 2 then
			print("However, this IS a team game, so we will try to group team members together."); print("-");
			self:NormalizeTeamLocations();
		end
	end
end
------------------------------------------------------------------------------

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
	start_plot_database:GenerateRegions{resources = res};

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
