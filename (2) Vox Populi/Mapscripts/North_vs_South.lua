------------------------------------------------------------------------------
--	FILE:	 North_vs_South.lua
--	AUTHOR:  Bob Thomas
--	PURPOSE: Regional map script - Designed to pit two teams against each other
--	         on a square map, one team in the north, the other in the south.
------------------------------------------------------------------------------
--	Copyright (c) 2010 Firaxis Games, Inc. All rights reserved.
------------------------------------------------------------------------------

include("MapGenerator");
include("MultilayeredFractal");
include("FeatureGenerator");
include("TerrainGenerator");

local iNumTeams;
local teamNorthID, teamSouthID;
local iNumCivsInNorth, iNumCivsInSouth;

------------------------------------------------------------------------------
function GetMapScriptInfo()
	local world_age, temperature, rainfall, _, _ = GetCoreMapOptions();
	return {
		Name = "TXT_KEY_MAP_NORTH_VS_SOUTH_VP",
		Description = "TXT_KEY_MAP_NORTH_VS_SOUTH_HELP",
		SupportsMultiplayer = true,
		IconIndex = 14,
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
	};
end
------------------------------------------------------------------------------

-------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	-- This function can reset map grid sizes or world wrap settings.

	-- North vs South is an extremely compact multiplayer map type.
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {16, 16},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {24, 24},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {30, 30},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {36, 36},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {44, 44},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {56, 56}
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

	-- This implementation is specific to North vs South.
	local iW, iH = Map.GetGridSize();
	local fracFlags = {};

	-- Fill all rows with land plots.
	self.wholeworldPlotTypes = table.fill(PlotTypes.PLOT_LAND, iW * iH);

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
	self:ApplyTectonics{world_age = world_age};

	-- Plot Type generation completed. Return global plot array.
	return self.wholeworldPlotTypes;
end
------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Setting Plot Types (Lua North vs South) ...");

	local layered_world = MultilayeredFractal.Create();
	local plot_list = layered_world:GeneratePlotsByRegion();

	SetPlotTypes(plot_list);

	GenerateCoasts{bExpandCoasts = false};
end
----------------------------------------------------------------------------------

----------------------------------------------------------------------------------
function TerrainGenerator:GenerateTerrainAtPlot(iX, iY)
	local lat = 0.8 * self:GetLatitudeAtPlot(iX, iY);

	local plot = Map.GetPlot(iX, iY);
	if plot:IsWater() then
		local val = plot:GetTerrainType();
		if val == TerrainTypes.NO_TERRAIN then -- Error handling.
			val = self.terrainGrass;
			plot:SetPlotType(PlotTypes.PLOT_LAND, false, false);
		end
		return val;
	end

	local terrainVal = self.terrainGrass;

	if lat >= self.fSnowLatitude then
		terrainVal = self.terrainSnow;
	elseif lat >= self.fTundraLatitude then
		terrainVal = self.terrainTundra;
	elseif lat < self.fGrassLatitude then
		terrainVal = self.terrainDesert;
	else
		local desertVal = self.deserts:GetHeight(iX, iY);
		local plainsVal = self.plains:GetHeight(iX, iY);
		if desertVal >= self.iDesertBottom and desertVal <= self.iDesertTop and lat >= self.fDesertBottomLatitude and lat < self.fDesertTopLatitude then
			terrainVal = self.terrainDesert;
		elseif plainsVal >= self.iPlainsBottom and plainsVal <= self.iPlainsTop then
			terrainVal = self.terrainPlains;
		end
	end

	-- Error handling.
	if terrainVal == TerrainTypes.NO_TERRAIN then
		return plot:GetTerrainType();
	end

	return terrainVal;
end
----------------------------------------------------------------------------------
function GenerateTerrain()
	print("Generating Terrain (Lua North vs South) ...");

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
	local numPlots = PlotTypes.NUM_PLOT_TYPES;
	local sum = (numPlots - plot:GetPlotType()) * 20;
	local numDirections = DirectionTypes.NUM_DIRECTION_TYPES;
	for direction = 0, numDirections - 1 do
		local adjacentPlot = Map.PlotDirection(plot:GetX(), plot:GetY(), direction);
		if adjacentPlot then
			sum = sum + (numPlots - adjacentPlot:GetPlotType());
		else
			sum = 0; -- Custom, prevents rivers avoiding running off the map edge.
		end
	end
	sum = sum + Map.Rand(10, "River Rand");
	return sum;
end
------------------------------------------------------------------------------
function DoRiver(startPlot, thisFlowDirection, originalFlowDirection, riverID)
	-- Customizing to handle problems in top row of the map. Only this aspect has been altered.

	local _, iH = Map.GetGridSize();
	thisFlowDirection = thisFlowDirection or FlowDirectionTypes.NO_FLOWDIRECTION;
	originalFlowDirection = originalFlowDirection or FlowDirectionTypes.NO_FLOWDIRECTION;

	-- pStartPlot = the plot at whose SE corner the river is starting
	if not riverID then
		riverID = nextRiverID;
		nextRiverID = nextRiverID + 1;
	end

	local otherRiverID = _rivers[startPlot];
	if otherRiverID and otherRiverID ~= riverID and originalFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION then
		return; -- Another river already exists here; can't branch off of an existing river!
	end

	local riverPlot;

	local bestFlowDirection = FlowDirectionTypes.NO_FLOWDIRECTION;
	if thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTH then
		riverPlot = startPlot;
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_EAST);
		if not adjacentPlot or riverPlot:IsWOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater() then
			return;
		end
		_rivers[riverPlot] = riverID;
		riverPlot:SetWOfRiver(true, thisFlowDirection);
		riverPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_NORTHEAST);
	elseif thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHEAST then
		riverPlot = startPlot;
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_SOUTHEAST);
		if not adjacentPlot or riverPlot:IsNWOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater() then
			return;
		end
		_rivers[riverPlot] = riverID;
		riverPlot:SetNWOfRiver(true, thisFlowDirection);
		-- riverPlot does not change
	elseif thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST then
		riverPlot = Map.PlotDirection(startPlot:GetX(), startPlot:GetY(), DirectionTypes.DIRECTION_EAST);
		if not riverPlot then
			return;
		end
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_SOUTHWEST);
		if not adjacentPlot or riverPlot:IsNEOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater() then
			return;
		end
		_rivers[riverPlot] = riverID;
		riverPlot:SetNEOfRiver(true, thisFlowDirection);
		-- riverPlot does not change
	elseif thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTH then
		riverPlot = Map.PlotDirection(startPlot:GetX(), startPlot:GetY(), DirectionTypes.DIRECTION_SOUTHWEST);
		if not riverPlot then
			return;
		end
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_EAST);
		if not adjacentPlot or riverPlot:IsWOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater() then
			return;
		end
		_rivers[riverPlot] = riverID;
		riverPlot:SetWOfRiver(true, thisFlowDirection);
		-- riverPlot does not change
	elseif thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST then
		riverPlot = startPlot;
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_SOUTHEAST);
		if not adjacentPlot or riverPlot:IsNWOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater() then
			return;
		end
		_rivers[riverPlot] = riverID;
		riverPlot:SetNWOfRiver(true, thisFlowDirection);
		-- riverPlot does not change
	elseif thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHWEST then
		riverPlot = startPlot;
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_SOUTHWEST);
		if not adjacentPlot or riverPlot:IsNEOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater() then
			return;
		end
		_rivers[riverPlot] = riverID;
		riverPlot:SetNEOfRiver(true, thisFlowDirection);
		riverPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_WEST);
	else
		-- River is starting here, set the direction in the next step
		riverPlot = startPlot;
	end

	if not riverPlot or riverPlot:IsWater() then
		-- The river has flowed off the edge of the map or into the ocean. All is well.
		return;
	end

	-- Storing X,Y positions as locals to prevent redundant function calls.
	local riverPlotX = riverPlot:GetX();
	local riverPlotY = riverPlot:GetY();

	-- Table of methods used to determine the adjacent plot.
	local adjacentPlotFunctions = {
		[FlowDirectionTypes.FLOWDIRECTION_NORTH] = function ()
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_NORTHWEST);
		end,

		[FlowDirectionTypes.FLOWDIRECTION_NORTHEAST] = function ()
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_NORTHEAST);
		end,

		[FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST] = function ()
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_EAST);
		end,

		[FlowDirectionTypes.FLOWDIRECTION_SOUTH] = function ()
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_SOUTHWEST);
		end,

		[FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST] = function ()
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_WEST);
		end,

		[FlowDirectionTypes.FLOWDIRECTION_NORTHWEST] = function ()
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_NORTHWEST);
		end,
	};

	if bestFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION then
		-- Attempt to calculate the best flow direction.
		local bestValue = math.huge;
		for flowDirection, getAdjacentPlot in pairs(adjacentPlotFunctions) do
			if GetOppositeFlowDirection(flowDirection) ~= originalFlowDirection then
				if thisFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION or
					flowDirection == TurnRightFlowDirections[thisFlowDirection] or
					flowDirection == TurnLeftFlowDirections[thisFlowDirection] then

					local adjacentPlot = getAdjacentPlot();
					if adjacentPlot then
						local value = GetRiverValueAtPlot(adjacentPlot);
						if flowDirection == originalFlowDirection then
							value = (value * 3) / 4;
						end
						if value < bestValue then
							bestValue = value;
							bestFlowDirection = flowDirection;
						end

					-- Top row of map, needs special handling
					elseif not adjacentPlot and riverPlotY == iH - 1 then
						if flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTH or
							flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHWEST or
							flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHEAST then

							local value = Map.Rand(5, "River Rand");
							if flowDirection == originalFlowDirection then
								value = (value * 3) / 4;
							end
							if value < bestValue then
								bestValue = value;
								bestFlowDirection = flowDirection;
							end
						end

					-- Left column of map, needs special handling
					elseif not adjacentPlot and riverPlotX == 0 then
						if flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTH or
							flowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTH or
							flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHWEST or
							flowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST then

							local value = Map.Rand(5, "River Rand");
							if flowDirection == originalFlowDirection then
								value = (value * 3) / 4;
							end
							if value < bestValue then
								bestValue = value;
								bestFlowDirection = flowDirection;
							end
						end
					end
				end
			end
		end

		-- Try a second pass allowing the river to "flow backwards".
		if bestFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION then
			bestValue = math.huge;
			for flowDirection, getAdjacentPlot in pairs(adjacentPlotFunctions) do
				if thisFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION or
					flowDirection == TurnRightFlowDirections[thisFlowDirection] or
					flowDirection == TurnLeftFlowDirections[thisFlowDirection] then

					local adjacentPlot = getAdjacentPlot();
					if adjacentPlot then
						local value = GetRiverValueAtPlot(adjacentPlot);
						if value < bestValue then
							bestValue = value;
							bestFlowDirection = flowDirection;
						end
					end
				end
			end
		end
	end

	-- Recursively generate river.
	if bestFlowDirection ~= FlowDirectionTypes.NO_FLOWDIRECTION then
		if originalFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION then
			originalFlowDirection = bestFlowDirection;
		end
		DoRiver(riverPlot, bestFlowDirection, originalFlowDirection, riverID);
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
	-- No ice on this map
end
------------------------------------------------------------------------------
function AddFeatures()
	print("Adding Features (Lua North vs South) ...");

	-- Get Rainfall setting input by user.
	local rain = Map.GetCustomOption(3);
	if rain == 4 then
		rain = 1 + Map.Rand(3, "Random Rainfall - Lua");
	end

	local featuregen = FeatureGenerator.Create{rainfall = rain};

	featuregen:AddFeatures();
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function AssignStartingPlots:GenerateRegions(args)
	print("Map Generation - Dividing the map in to Regions");
	-- This is a customized version for North vs South.
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
	-- print("-"); print("Civs:", self.iNumCivs); print("City States:", self.iNumCityStates);

	-- Determine number of teams (of Major Civs only, not City States) present in this game.
	iNumTeams = table.maxn(self.teams_with_major_civs);
	print("-"); print("Teams:", iNumTeams);

	-- Fetch team setting.
	local team_setting = Map.GetCustomOption(5);

	-- If two teams are present, use team-oriented handling of start points, one team north, one south.
	if iNumTeams == 2 and team_setting == 1 then
		print("-"); print("Number of Teams present is two! Using custom team start placement for North vs South."); print("-");

		-- ToDo: Correctly identify team IDs and how many Civs are on each team.
		-- Also need to shuffle the teams so its random who starts on which half.
		local shuffled_team_list = GetShuffledCopyOfTable(self.teams_with_major_civs);
		teamNorthID = shuffled_team_list[1];
		teamSouthID = shuffled_team_list[2];
		iNumCivsInNorth = self.number_civs_per_team[teamNorthID];
		iNumCivsInSouth = self.number_civs_per_team[teamSouthID];

		-- Process the team in the south.
		self.inhabited_WestX = 0;
		self.inhabited_SouthY = 0;
		self.inhabited_Width = iW;
		self.inhabited_Height = math.floor(iH / 2) - 1;
		-- Obtain "Start Placement Fertility" inside the rectangle.
		-- Data returned is: fertility table, sum of all fertility, plot count.
		local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityInRectangle(
			self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height);
		-- Assemble the Rectangle data table. AreaID -1 means ignore landmass/area IDs.
		local rect_table = {self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height, -1, fertCount, plotCount};
		-- Divide the rectangle.
		self:DivideIntoRegions(iNumCivsInSouth, fert_table, rect_table);

		-- Process the team in the north.
		self.inhabited_WestX = 0;
		self.inhabited_SouthY = math.floor(iH / 2) + 1;
		self.inhabited_Width = iW;
		self.inhabited_Height = math.floor(iH / 2) - 1;
		-- Obtain "Start Placement Fertility" inside the rectangle.
		-- Data returned is: fertility table, sum of all fertility, plot count.
		fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityInRectangle(
			self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height);
		-- Assemble the Rectangle data table. AreaID -1 means ignore landmass/area IDs.
		rect_table = {self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height, -1, fertCount, plotCount};
		-- Divide the rectangle.
		self:DivideIntoRegions(iNumCivsInNorth, fert_table, rect_table);
		-- The regions have been defined.

	-- If number of teams is any number other than two, use standard One Landmass division.
	else
		print("-"); print("Dividing the map at random."); print("-");
		self.method = RegionDivision.BIGGEST_LANDMASS;
		-- Identify the biggest landmass.
		local iLandmassID = Map.FindBiggestLandmassID(false);
		-- We'll need all eight data fields returned in the results table from the boundary finder:
		local landmass_data = self:GetLandmassBoundaries(iLandmassID);
		local iWestX = landmass_data[1];
		local iSouthY = landmass_data[2];
		local iEastX = landmass_data[3];
		local iNorthY = landmass_data[4];
		local iWidth = landmass_data[5];
		local iHeight = landmass_data[6];
		local wrapsX = landmass_data[7];
		local wrapsY = landmass_data[8];

		-- Obtain "Start Placement Fertility" of the landmass.
		local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityOfLandmass(iLandmassID, iWestX, iEastX, iSouthY, iNorthY, wrapsX, wrapsY);
		-- Assemble the Rectangle data table:
		local rect_table = {iWestX, iSouthY, iWidth, iHeight, iLandmassID, fertCount, plotCount};
		-- The data from this call is processed in to self.regionData during the process.
		self:DivideIntoRegions(self.iNumCivs, fert_table, rect_table);
		-- The regions have been defined.
	end

	---[[ Printout is for debugging only. Deactivate otherwise.
	local tempRegionData = self.regionData;
	for i, data in ipairs(tempRegionData) do
		print("-");
		print("Data for Start Region #", i);
		print("WestX:", data[1]);
		print("SouthY:", data[2]);
		print("Width:", data[3]);
		print("Height:", data[4]);
		print("LandmassID:", data[5]);
		print("Fertility:", data[6]);
		print("Plots:", data[7]);
		print("Fert/Plot:", data[8]);
		print("-");
	end
	--]]
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
		-- Two teams, place one in the north half, other in south -- even if team membership totals are uneven.
		print("-"); print("This is a team game with two teams! Place one team in North, other in South."); print("-");
		local playerList, northList, southList = {}, {}, {};
		for loop = 1, self.iNumCivs do
			local player_ID = self.player_ID_list[loop];
			table.insert(playerList, player_ID);
			local player = Players[player_ID];
			local team_ID = player:GetTeam();
			if team_ID == teamNorthID then
				-- print("Player #", player_ID, "belongs to Team #", team_ID, "and will be placed in the North.");
				table.insert(northList, player_ID);
			elseif team_ID == teamSouthID then
				-- print("Player #", player_ID, "belongs to Team #", team_ID, "and will be placed in the South.");
				table.insert(southList, player_ID);
			else
				print("* ERROR * - Player #", player_ID, "belongs to Team #", team_ID, "which is neither North nor South!");
			end
		end

		---[[ Debug
		if table.maxn(northList) ~= iNumCivsInNorth then
			print("-"); print("*** ERROR! *** . . . Mismatch between number of Civs on North team and number of civs assigned to north locations.");
		end
		if table.maxn(southList) ~= iNumCivsInSouth then
			print("-"); print("*** ERROR! *** . . . Mismatch between number of Civs on South team and number of civs assigned to south locations.");
		end
		--]]

		local northListShuffled = GetShuffledCopyOfTable(northList);
		local southListShuffled = GetShuffledCopyOfTable(southList);
		for region_number, player_ID in ipairs(southListShuffled) do
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];
			local start_plot = Map.GetPlot(x, y);
			local player = Players[player_ID];
			player:SetStartingPlot(start_plot);
		end
		for loop, player_ID in ipairs(northListShuffled) do
			local x = self.startingPlots[loop + iNumCivsInSouth][1];
			local y = self.startingPlots[loop + iNumCivsInSouth][2];
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
		if self.bTeamGame  and team_setting ~= 2 then
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
	local args = {
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
