------------------------------------------------------------------------------
--	FILE:	  MapmakerUtilities.lua
--	AUTHOR:   Bob Thomas
--	PURPOSE:  Functions designed to support start and resource placement.
------------------------------------------------------------------------------
--	Copyright (c) 2010 Firaxis Games, Inc. All rights reserved.
------------------------------------------------------------------------------

--[[ -------------------------------------------------------------------------
NOTE: This file is an essential component of the Start Plot System. I have
separated out the functions in this file because A) They CAN be separated,
because they do not have to operate directly on the self-dot data entries,
and B) I thought they might be useful elsewhere, such as in mods or in custom
operations for map scripts -- and this would permit them to be utilized.

CONTENTS OF THIS FILE:

* GetPlayerAndTeamInfo()

* ObtainLandmassBoundaries(iAreaID)

* AdjacentToSaltWater(x, y)
* GenerateCoastalLandDataTable()
* GenerateNextToCoastalLandDataTables()

* CivNeedsCoastalStart(civType)
* CivNeedsRiverStart(civType)
* GetNumStartRegionPriorityForCiv(civType)
* GetNumStartRegionAvoidForCiv(civType)
* GetStartRegionPriorityListForCiv_GetIDs(civType)
* GetStartRegionAvoidListForCiv_GetIDs(civType)
* GetStartRegionPriorityListForCiv_GetTypes(civType)
* GetStartRegionAvoidListForCiv_GetTypes(civType)

* TestMembership(table, value)
* GetShuffledCopyOfTable(incoming_table)
* IdentifyTableIndex(incoming_table, value)
* PrintContentsOfTable(incoming_table)

Moved from MapGenerator.lua:
* Shuffle(t)
* Plots(sort)

Moved from AssignStartingPlots.lua:
* ApplyHexAdjustment(x, y, plot_adjustments)

New utility functions from VP, some originally from Communitas map:
* GetContinentBoundaries(iContinentID)
* GetLandmassBoundaries(iLandmassID)
* GetAreaBoundaries(iAreaID)
* CivNeedsMountainStart(civType)
* CivNeedsSnowStart(civType)
* GetLuaPlotIndexFromCoordinates(x, y)
* GetCoordinatesFromLuaPlotIndex(iPlotIndex)
* Plot_GetPlotsInCircle(plot, minR, maxR)
* Plot_GetPlotCoordinatesInCircle(pPlot, iMinR, iMaxR)
* Plot_GetFertilityInRange(plot, range, yieldID)
* Plot_GetFertility(plot, yieldID, ignoreStrategics)
* IsTropical(y)
* IsBetween(lower, mid, upper)
* Constrain(lower, mid, upper)
* GetRandomMultiplier(variance, rand, higher)
* GetRandomFromRangeInclusive(min, max)
* GetRandomFromWeightedList(tOptions, tWeights, strLogMessage)
* GetRandomFromWeightedTable(tOptionWeight, strLogMessage)
------------------------------------------------------------------------- ]]

include("VPUI_core");
include("CPK.lua");

local VP = MapModData and MapModData.VP or VP;
local TableCopy = CPK.Table.Copy;

--- Shuffle an array table randomly in place.
--- @generic T
--- @param t T[]
--- @return T[] # The shuffled table
--- @see GetShuffledCopyOfTable
function Shuffle(t)
	local random = Map.Rand;
	for i = #t, 2, -1 do
		local j = random(i, "Shuffling Values") + 1;
		t[i], t[j] = t[j], t[i];
	end
	return t;
end

-- Memoize table of plots
local _plots = {};

--- Get an iterator of all plots, optionally shuffled with the passed `Sort` function
--- @param Sort fun(table)? # Function that modifies a table in place, used on plot indices
--- @return fun(): PlotId?, Plot?
function Plots(Sort)
	local iNumPlots = Map.GetNumPlots();

	if not next(_plots) then
		for i = 0, iNumPlots - 1 do
			_plots[i] = Map.GetPlotByIndex(i);
		end
	end

	local cur = 0;

	if not Sort then
		return function()
			local idx = cur;
			cur = cur + 1;
			if cur > iNumPlots then return end
			return idx, _plots[idx];
		end;
	end

	local indices = {};
	for i = 1, iNumPlots do
		indices[i] = i - 1;
	end

	Sort(indices);

	return function()
		cur = cur + 1;
		if cur > iNumPlots then return end

		local idx = indices[cur];
		return idx, _plots[idx];
	end;
end

--- Obtain Civ count, CS count, correct player IDs, and basic Team information
function GetPlayerAndTeamInfo()
	local iNumCivs, iNumCityStates, player_ID_list = 0, 0, {};
	for i = 0, GameDefines.MAX_MAJOR_CIVS - 1 do
		local player = Players[i];
		if player:IsEverAlive() then
			iNumCivs = iNumCivs + 1;
			table.insert(player_ID_list, i);
		end
	end
	for i = GameDefines.MAX_MAJOR_CIVS, GameDefines.MAX_CIV_PLAYERS - 1 do
		local player = Players[i];
		if player:IsEverAlive() then
			iNumCityStates = iNumCityStates + 1;
		end
	end

	local bTeamGame = false;
	local iNumTeams = Game.CountCivTeamsEverAlive();
	local iNumTeamsOfCivs = iNumTeams - iNumCityStates;
	if iNumTeamsOfCivs < iNumCivs then
		bTeamGame = true;
	end

	local teams_with_major_civs = {};
	local number_civs_per_team = table.fill(0, GameDefines.MAX_CIV_PLAYERS);
	number_civs_per_team[0] = 0;
	for i = 0, GameDefines.MAX_MAJOR_CIVS - 1 do
		local player = Players[i];
		if player:IsEverAlive() then
			local teamID = player:GetTeam();
			number_civs_per_team[teamID] = number_civs_per_team[teamID] + 1;
			local bCheckTeamList = TestMembership(teams_with_major_civs, teamID);
			if bCheckTeamList == false then
				table.insert(teams_with_major_civs, teamID);
			end
		end
	end

	return iNumCivs, iNumCityStates, player_ID_list, bTeamGame, teams_with_major_civs, number_civs_per_team;
end

--- Generic version of ObtainLandmassBoundaries/GetAreaBoundaries, GetLandmassBoundaries, and GetContinentBoundaries
--- @param iID ContinentId|LandmassId|AreaId
--- @param strGetSectionIdFunc string # function name, where Plot:*strGetSectionIdFunc*() is the function to get the section ID
--- @return [integer, integer, integer, integer, integer, integer, boolean, boolean]
local function GetMapSectionBoundaries(iID, strGetSectionIdFunc)
	local iW, iH = Map.GetGridSize();
	local bWrapX = false;
	local bWrapY = false;
	local iWestX, iEastX, iSouthY, iNorthY, iWidth, iHeight;

	-- Locate any plot belonging to the map section
	local pStartPlot;
	for _, pPlot in Plots() do
		if pPlot[strGetSectionIdFunc](pPlot) == iID then
			pStartPlot = pPlot;
			break;
		end
	end

	if not pStartPlot then
		print("WARNING: Invalid map section ID", iID, "passed in. Returned data are dummy values and may cause further errors.");
		return {0, -1, 0, -1, 0, 0, false, false};
	end

	-- Tables for plot traversal using DFS
	local tStack = {};
	local tVisitedPlots = {}; --- @type table<PlotId, boolean>

	-- Whether these coordinates contain plots from the map section
	local tXCoords = table.fill(false, iW - 1); tXCoords[0] = false;
	local tYCoords = table.fill(false, iH - 1); tYCoords[0] = false;

	-- Traverse the map starting at the known plot. The `tVisitedPlots` table will contain all the plot indices of the map section as keys,
	-- and the `tXCoords` and `tYCoords` tables will have all distinct X- and Y-coordinates of the plots belonging to the map section.
	-- We don't need to care whether the map wraps; Plot_GetPlotsInCircle already handles that.
	table.insert(tStack, pStartPlot);
	tVisitedPlots[pStartPlot:GetIndex()] = true;
	while #tStack > 0 do
		local pPlot = table.remove(tStack);
		local x = pPlot:GetX();
		local y = pPlot:GetY();
		tXCoords[x] = true;
		tYCoords[y] = true;
		for pLoopPlot in Plot_GetPlotsInCircle(pPlot, 1) do
			local iLoopPlotIndex = pLoopPlot:GetIndex();
			if pLoopPlot[strGetSectionIdFunc](pLoopPlot) == iID and not tVisitedPlots[iLoopPlotIndex] then
				tVisitedPlots[iLoopPlotIndex] = true;
				table.insert(tStack, pLoopPlot);
			end
		end
	end

	-- Find the X boundaries.
	-- There should be 0, 1, or 2 streaks of `false` in `tXCoords`, which determines the boundaries and whether the section wraps around the map edge.
	local iLeftGapW, iLeftGapE, iRightGapW;
	for x = 0, iW - 1 do
		if not tXCoords[x] then -- this coordinate isn't within the section
			if not iLeftGapW then -- the start of the first gap
				iLeftGapW = x;
				iLeftGapE = x;
			elseif iLeftGapE == x - 1 then -- continuation of the first gap
				iLeftGapE = x;
			elseif not iRightGapW then -- the start of the second gap
				iRightGapW = x;
			end
		else
			-- Verify that the algorithm is correct
			assert(not iRightGapW, "tXCoords[x] cannot be true after the start of the second gap");
		end
	end

	if iRightGapW then -- 2 gaps
		iWestX = iLeftGapE + 1;
		iEastX = iRightGapW - 1;
		iWidth = iRightGapW - iWestX;
	elseif iLeftGapW == 0 then -- 1 gap that spans from the left edge
		iWestX = iLeftGapE + 1;
		iEastX = iW - 1;
		iWidth = iW - iWestX;
	elseif iLeftGapE == iW - 1 then -- 1 gap that spans from the right edge
		iWestX = 0;
		iEastX = iLeftGapW - 1;
		iWidth = iLeftGapW;
	elseif not iLeftGapW then -- no gap; the section spans the whole map
		iWestX = 0;
		iEastX = iW - 1;
		iWidth = iW;
	else -- 1 gap in the middle; the section wraps
		bWrapX = true;
		iWestX = iLeftGapE + 1;
		iEastX = iLeftGapW - 1;
		iWidth = iEastX + iW - iLeftGapE;
	end

	-- Find the Y boundaries.
	-- There should be 0, 1, or 2 streaks of `false` in `tYCoords`, which determines the boundaries and whether the section wraps around the map edge.
	local iBottomGapS, iBottomGapN, iTopGapS;
	for y = 0, iH - 1 do
		if not tYCoords[y] then -- this coordinate isn't within the section
			if not iBottomGapS then -- the start of the first gap
				iBottomGapS = y;
				iBottomGapN = y;
			elseif iBottomGapN == y - 1 then -- continuation of the first gap
				iBottomGapN = y;
			elseif not iTopGapS then -- the start of the second gap
				iTopGapS = y;
			end
		else
			-- Verify that the algorithm is correct
			assert(not iTopGapS, "tYCoords[y] cannot be true after the start of the second gap");
		end
	end

	if iTopGapS then -- 2 gaps
		iSouthY = iBottomGapN + 1;
		iNorthY = iTopGapS - 1;
		iHeight = iTopGapS - iSouthY;
	elseif iBottomGapS == 0 then -- 1 gap that spans from the left edge
		iSouthY = iBottomGapN + 1;
		iNorthY = iH - 1;
		iHeight = iH - iSouthY;
	elseif iBottomGapN == iH - 1 then -- 1 gap that spans from the right edge
		iSouthY = 0;
		iNorthY = iBottomGapS - 1;
		iHeight = iBottomGapS;
	elseif not iBottomGapS then -- no gap; the section spans the whole map
		iSouthY = 0;
		iNorthY = iH - 1;
		iHeight = iH;
	else -- 1 gap in the middle; the section wraps
		bWrapY = true;
		iSouthY = iBottomGapN + 1;
		iNorthY = iBottomGapS - 1;
		iHeight = iNorthY + iH - iBottomGapN;
	end

	---[[ Log dump for debug purposes only, disable otherwise.
	local iPlotCount = 0;
	for _ in pairs(tVisitedPlots) do
		iPlotCount = iPlotCount + 1;
	end
	print("--- Section Boundary Readout ---");
	print("iID:", iID, "strGetSectionIdFunc:", strGetSectionIdFunc);
	print("West X:", iWestX, "East X:", iEastX);
	print("South Y:", iSouthY, "North Y:", iNorthY);
	print("Width:", iWidth, "Height:", iHeight);
	print("Total Plots in section:", iPlotCount);
	print("Total Plots in 'section rectangle':", iWidth * iHeight);
	print("- - - - - - - - - - - - - - - - -");
	--]]

	-- Insert data into table, then return the table.
	local data = {iWestX, iSouthY, iEastX, iNorthY, iWidth, iHeight, bWrapX, bWrapY};
	return data;
end

--- Obtain the boundaries of an AREA, despite the name<br>
--- Return a table of {iWestX, iSouthY, iEastX, iNorthY, iWidth, iHeight, wrapsX, wrapsY}
--- @param iAreaID AreaId
--- @return [integer, integer, integer, integer, integer, integer, boolean, boolean]
function ObtainLandmassBoundaries(iAreaID)
	return GetMapSectionBoundaries(iAreaID, "GetArea");
end

-- Set up alias with the more accurate name
GetAreaBoundaries = ObtainLandmassBoundaries;

--- Obtain the boundaries of a landmass<br>
--- Return a table of {iWestX, iSouthY, iEastX, iNorthY, iWidth, iHeight, wrapsX, wrapsY}
--- @param iLandmassID LandmassId
--- @return [integer, integer, integer, integer, integer, integer, boolean, boolean]
function GetLandmassBoundaries(iLandmassID)
	return GetMapSectionBoundaries(iLandmassID, "GetLandmass");
end

--- Obtain the boundaries of a continent<br>
--- Return a table of {iWestX, iSouthY, iEastX, iNorthY, iWidth, iHeight, wrapsX, wrapsY}
--- @param iContinentID ContinentId
--- @return [integer, integer, integer, integer, integer, integer, boolean, boolean]
function GetContinentBoundaries(iContinentID)
	return GetMapSectionBoundaries(iContinentID, "GetContinent");
end

--- Check whether plot (x, y) is any type of land adjacent to at least one body of salt water
--- @param x integer # Must be valid coordinate
--- @param y integer # Must be valid coordinate
--- @return boolean
function AdjacentToSaltWater(x, y)
	local pPlot = Map.GetPlot(x, y);
	if pPlot:GetPlotType() ~= PlotTypes.PLOT_OCEAN then -- This plot is land, process it.
		for pLoopPlot in Plot_GetPlotsInCircle(pPlot, 1) do
			if pLoopPlot:GetPlotType() == PlotTypes.PLOT_OCEAN and not pLoopPlot:IsLake() then
				return true;
			end
		end
	end

	-- Current plot is itself water, or else no salt water found among adjacent plots.
	return false;
end

--- @return table<PlotId, boolean>
function GenerateCoastalLandDataTable()
	local iW, iH = Map.GetGridSize();
	local plotDataIsCoastal = {};
	table.fill(plotDataIsCoastal, false, iW * iH);

	for iPlotIndex = 1, Map.GetNumPlots() do
		local x, y = GetCoordinatesFromLuaPlotIndex(iPlotIndex);
		if AdjacentToSaltWater(x, y) then
			plotDataIsCoastal[iPlotIndex] = true;
		end
	end

	return plotDataIsCoastal;
end

--- @return table<PlotId, boolean>, table<PlotId, boolean>
function GenerateNextToCoastalLandDataTables()
	-- Set up data table for IsCoastal
	local plotDataIsCoastal = GenerateCoastalLandDataTable();

	-- Set up data table for IsNextToCoast
	local iW, iH = Map.GetGridSize();
	local plotDataIsNextToCoast = {};
	table.fill(plotDataIsNextToCoast, false, iW * iH);

	for iPlotIndex, pPlot in Plots() do
		if not plotDataIsCoastal[iPlotIndex + 1] and not pPlot:IsWater() then
			for pLoopPlot in Plot_GetPlotsInCircle(pPlot, 1) do
				if plotDataIsCoastal[pLoopPlot:GetIndex() + 1] then
					-- The current loop plot is not itself on the coast but is next to a plot that is on the coast.
					plotDataIsNextToCoast[iPlotIndex + 1] = true;
				end
			end
		end
	end

	return plotDataIsCoastal, plotDataIsNextToCoast;
end

--- Retrieve Start Along Ocean bias from the database
function CivNeedsCoastalStart(civType)
	for row in GameInfo.Civilization_Start_Along_Ocean{CivilizationType = civType} do
		if row.StartAlongOcean then
			return true;
		end
	end
	return false;
end

--- Retrieve Start Along River bias from the database
function CivNeedsRiverStart(civType)
	for row in GameInfo.Civilization_Start_Along_River{CivilizationType = civType} do
		if row.StartAlongRiver then
			return true;
		end
	end
	return false;
end

--- Retrieve Start Near Mountains bias from the database
function CivNeedsMountainStart(civType)
	for row in GameInfo.Civilization_Start_Prefer_Mountain{CivilizationType = civType} do
		if row.StartPreferMountain then
			return true;
		end
	end
	return false;
end

--- Retrieve Start Near Snow bias from the database
function CivNeedsSnowStart(civType)
	for row in GameInfo.Civilization_Start_Prefer_Snow{CivilizationType = civType} do
		if row.StartPreferSnow then
			return true;
		end
	end
	return false;
end

--- Retrieve Start Along Ocean Priority bias from the database
function CivNeedsPlaceFirstCoastalStart(civType)
	for row in GameInfo.Civilization_Start_Place_First_Along_Ocean{CivilizationType = civType} do
		if row.PlaceFirst then
			return true;
		end
	end
	return false;
end

--- Detect if a civ has Start Region Priority needs and return the count
function GetNumStartRegionPriorityForCiv(civType)
	for row in DB.Query("select count(*) as count from Civilization_Start_Region_Priority where CivilizationType = ?", civType) do
		return row.count;
	end
	return 0;
end

--- Detect if a civ has Start Region Avoid needs and return the count
function GetNumStartRegionAvoidForCiv(civType)
	for row in DB.Query("select count(*) as count from Civilization_Start_Region_Avoid where CivilizationType = ?", civType) do
		return row.count;
	end
	return 0;
end

--- Get the sorted list of all Start Region Priority for this civ.
--- List will include ID of each region type, plus be sorted and contain only unique values.
function GetStartRegionPriorityListForCiv_GetIDs(civType)
	local priorityRegionIDs = {};
	for row in GameInfo.Civilization_Start_Region_Priority{CivilizationType = civType} do
		local region_type = VP.GameInfoTypes[row.RegionType];
		if not TestMembership(priorityRegionIDs, region_type) then
			table.insert(priorityRegionIDs, region_type);
		end
	end
	table.sort(priorityRegionIDs);
	return priorityRegionIDs;
end

--- Get the sorted list of all Start Region Avoid for this civ.
--- List will include ID of each region type, plus be sorted and contain only unique values.
function GetStartRegionAvoidListForCiv_GetIDs(civType)
	local avoidRegionIDs = {};
	for row in GameInfo.Civilization_Start_Region_Avoid{CivilizationType = civType} do
		local region_type = VP.GameInfoTypes[row.RegionType];
		if not TestMembership(avoidRegionIDs, region_type) then
			table.insert(avoidRegionIDs, region_type);
		end
	end
	table.sort(avoidRegionIDs);
	return avoidRegionIDs;
end

--- Get the unsorted list of all Start Region Priority for this civ.
--- List will include type of each region, not IDs, e.g REGION_DESERT, REGION_HILLS, and contain only unique values.
function GetStartRegionPriorityListForCiv_GetTypes(civType)
	local priorityRegionTypes = {};
	for row in GameInfo.Civilization_Start_Region_Priority{CivilizationType = civType} do
		if not TestMembership(priorityRegionTypes, row.RegionType) then
			table.insert(priorityRegionTypes, row.RegionType);
		end
	end
	return priorityRegionTypes;
end

--- Get the unsorted list of all Start Region Avoid for this civ.
--- List will include type of each region, not IDs, e.g REGION_DESERT, REGION_HILLS, and contain only unique values.
function GetStartRegionAvoidListForCiv_GetTypes(civType)
	local avoidRegionTypes = {};
	for row in GameInfo.Civilization_Start_Region_Avoid{CivilizationType = civType} do
		if not TestMembership(avoidRegionTypes, row.RegionType) then
			table.insert(avoidRegionTypes, row.RegionType);
		end
	end
	return avoidRegionTypes;
end

--- Test whether `value` exists in `table`
--- @param table table
--- @param value any
--- @return boolean
function TestMembership(table, value)
	for _, data in pairs(table) do
		if data == value then
			return true;
		end
	end
	return false;
end

--- Designed to operate on array tables. Does not affect original table.
--- @generic T
--- @param incoming_table T[]
--- @return T[]
--- @see Shuffle
function GetShuffledCopyOfTable(incoming_table)
	return Shuffle(TableCopy(incoming_table));
end

--- Purpose of this function is to make it easy to remove a data entry from an array table when the index of the entry is unknown.
--- @generic T
--- @param incoming_table table<T, any>
--- @param value any
--- @return boolean # Whether `value` is found
--- @return integer # Number of times `value` is found in `incoming_table`
--- @return T[] # List of keys that has the value of `value` in `incoming_table`
function IdentifyTableIndex(incoming_table, value)
	local bFoundValue = false;
	local iNumTimesFoundValue = 0;
	local table_of_indices = {};
	for loop, test_value in pairs(incoming_table) do
		if test_value == value then
			bFoundValue = true;
			iNumTimesFoundValue = iNumTimesFoundValue + 1;
			table.insert(table_of_indices, loop);
		end
	end
	return bFoundValue, iNumTimesFoundValue, table_of_indices;
end

--- For debugging purposes. LOTS of table data being handled here.
function PrintContentsOfTable(incoming_table)
	print("--------------------------------------------------");
	print("Table printout for table ID:", incoming_table);
	for index, data in pairs(incoming_table) do
		print("Table index:", index, "Table entry:", data);
	end
	print("- - - - - - - - - - - - - - - - - - - - - - - - - -");
end

--- @param x integer
--- @param y integer
--- @param plot_adjustments [integer, integer]
--- @return integer adjusted_x
--- @return integer adjusted_y
function ApplyHexAdjustment(x, y, plot_adjustments)
	local iW, iH = Map.GetGridSize();
	local adjusted_x, adjusted_y;

	if Map.IsWrapX() then
		adjusted_x = (x + plot_adjustments[1]) % iW;
	else
		adjusted_x = x + plot_adjustments[1];
	end
	if Map.IsWrapY() then
		adjusted_y = (y + plot_adjustments[2]) % iH;
	else
		adjusted_y = y + plot_adjustments[2];
	end
	return adjusted_x, adjusted_y;
end

--- Get the Lua plot index corresponding to the given grid coordinates.
--- Note that Lua plot index is 1-based (by convention) while C++ plot index is 0-based.
--- @param x integer
--- @param y integer
--- @return PlotId
function GetLuaPlotIndexFromCoordinates(x, y)
	local iMapW = Map.GetGridSize();
	return y * iMapW + x + 1;
end

--- Get the grid coordinates corresponding to the given Lua plot index.
--- Note that Lua plot index is 1-based (by convention) while C++ plot index is 0-based.
--- @param iPlotIndex PlotId
--- @return integer, integer
function GetCoordinatesFromLuaPlotIndex(iPlotIndex)
	local iMapW = Map.GetGridSize();
	local x = (iPlotIndex - 1) % iMapW;
	local y = (iPlotIndex - x - 1) / iMapW;
	return x, y;
end

local tAdjustmentForEvenY = {{0, 1}, {1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};
local tAdjustmentForOddY = {{1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, 0}, {0, 1}};

--- @param pPlot Plot
--- @param iMinR integer
--- @param iMaxR integer?
--- @param bReturnPlots true
--- @return fun(): Plot, integer
--- @overload fun(pPlot: Plot, iMinR: integer, iMaxR: integer?, bReturnPlots: false): fun(): integer, integer, integer
local function Plot_GetPlotsOrCoordinatesInCircle(pPlot, iMinR, iMaxR, bReturnPlots)
	if not iMaxR then
		iMinR, iMaxR = 1, iMinR;
	end

	local iMapW, iMapH = Map.GetGridSize();
	local iX = pPlot:GetX();
	local iY = pPlot:GetY();

	return coroutine.wrap(function ()
		for iRadius = iMinR, iMaxR do
			-- Locate the plot within this ring that is due West of the center plot
			local iCurrentX = iX - iRadius;
			local iCurrentY = iY;

			-- Now loop through the six directions, moving iRadius number of times per direction.
			-- This effectively goes through all plots on the ring.
			for iDirection = 1, 6 do
				for _ = 1, iRadius do
					-- Must account for hex factor.
					local adjustment;
				 	if iCurrentY % 2 ~= 0 then
						adjustment = tAdjustmentForOddY[iDirection];
					else
						adjustment = tAdjustmentForEvenY[iDirection];
					end

					iCurrentX, iCurrentY = ApplyHexAdjustment(iCurrentX, iCurrentY, adjustment);

					-- Make sure the plot exists
					if iCurrentX >= 0 and iCurrentX < iMapW and iCurrentY >= 0 and iCurrentY < iMapH then
						if bReturnPlots then
							coroutine.yield(Map.GetPlot(iCurrentX, iCurrentY), iRadius);
						else
							coroutine.yield(iCurrentX, iCurrentY, iRadius);
						end
					end
				end
			end
		end
	end);
end

--- Generate an iterator that loops through all plots that are `iMinR` to `iMaxR` tiles (inclusive) from `pPlot`.<br>
--- If `iMaxR` is not passed, the range is 1 to `iMinR` inclusive instead.
--- @param pPlot Plot
--- @param iMinR integer
--- @param iMaxR integer?
--- @return fun(): Plot, integer
function Plot_GetPlotsInCircle(pPlot, iMinR, iMaxR)
	return Plot_GetPlotsOrCoordinatesInCircle(pPlot, iMinR, iMaxR, true);
end

--- Generate an iterator that loops through the coordinates of all plots that are `iMinR` to `iMaxR` tiles (inclusive) from `pPlot`.<br>
--- If `iMaxR` is not passed, the range is 1 to `iMinR` inclusive instead.
--- @param pPlot Plot
--- @param iMinR integer
--- @param iMaxR integer?
--- @return fun(): integer, integer, integer
function Plot_GetPlotCoordinatesInCircle(pPlot, iMinR, iMaxR)
	return Plot_GetPlotsOrCoordinatesInCircle(pPlot, iMinR, iMaxR, false);
end

--- Get the total fertility within range of the specific plot. Mainly used for City-State and Natural Wonder site evaluation.
--- @param plot Plot
--- @param range integer
--- @param yieldID YieldType? # If not specified, all 6 primary yields is considered
--- @return number
function Plot_GetFertilityInRange(plot, range, yieldID)
	local value = 0;
	for nearPlot, distance in Plot_GetPlotsInCircle(plot, range) do
		value = value + Plot_GetFertility(nearPlot, yieldID) / math.max(1, distance);
	end
	return value;
end

--- Get the fertility of a specific plot. Mainly used for City-State and Natural Wonder site evaluation.
--- @param plot Plot
--- @param yieldID YieldType? # If not specified, all 6 primary yields is considered
--- @param ignoreStrategics boolean? # If not true, strategic resources increase fertility score
--- @return number
function Plot_GetFertility(plot, yieldID, ignoreStrategics)
	if plot:IsImpassable() or plot:GetTerrainType() == TerrainTypes.TERRAIN_OCEAN then
		return 0;
	end

	local value = 0;
	local featureID = plot:GetFeatureType();
	local terrainID = plot:GetTerrainType();
	local resID = plot:GetResourceType();

	if yieldID then
		value = value + plot:CalculateYield(yieldID);
	else
		-- Science, Culture and Faith are worth more than the others at start
		value = value + plot:CalculateYield(YieldTypes.YIELD_FOOD);
		value = value + plot:CalculateYield(YieldTypes.YIELD_PRODUCTION);
		value = value + plot:CalculateYield(YieldTypes.YIELD_GOLD);
		value = value + 2 * plot:CalculateYield(YieldTypes.YIELD_SCIENCE);
		value = value + 2 * plot:CalculateYield(YieldTypes.YIELD_CULTURE);
		value = value + 2 * plot:CalculateYield(YieldTypes.YIELD_FAITH);
	end

	if plot:IsFreshWater() and plot:GetPlotType() ~= PlotTypes.PLOT_HILLS then
		-- Fresh water farm possibility
		value = value + 0.25;
	end

	if plot:IsLake() then
		-- Can't improve lakes
		value = value - 0.5;
	end

	if featureID == FeatureTypes.FEATURE_JUNGLE then
		-- Jungles aren't as good as the yields imply
		value = value - 0.5;
	end

	if resID == -1 then
		if featureID == -1 and terrainID == TerrainTypes.TERRAIN_COAST then
			-- Can't improve coast tiles until lighthouse
			-- Lower value generates more fish
			value = value - 1;
		end
	else
		local resInfo = GameInfo.Resources[resID];
		value = value + 2 * resInfo.Happiness;
		if resInfo.ResourceClassType == "RESOURCECLASS_RUSH" and not ignoreStrategics then
			-- Iron and Horses
			value = value + math.ceil(3 * math.sqrt(plot:GetNumResource()));
		elseif resInfo.ResourceClassType == "RESOURCECLASS_BONUS" then
			value = value + 2;
		end
	end

	return value;
end

--- Determine whether the given y-coordinate belongs to the tropical region.
--- Tropical range depends on the rainfall option.
function IsTropical(y)
	local _, iH = Map.GetGridSize();
	local lat = 0;
	if y >= iH / 2 then
		lat = math.abs(iH / 2 - y) / (iH / 2);
	else
		lat = math.abs(iH / 2 - (y + 1)) / (iH / 2);
	end
	local AvgJungleRange = 0;
	local rain = Map.GetCustomOption(2);
	if rain == 1 then
		-- Arid
		AvgJungleRange = 0.10;
	elseif rain == 3 then
		-- Wet
		AvgJungleRange = 0.25;
	else
		-- Normal or Random (Note: I'm currently not sure how to retrieve random, so we'll just use normal for now.)
		AvgJungleRange = 0.15;
	end
	return lat <= AvgJungleRange;
end

--- @param lower number
--- @param mid number
--- @param upper number
--- @return boolean
function IsBetween(lower, mid, upper)
	return lower <= mid and mid <= upper;
end

--- @param lower number
--- @param mid number
--- @param upper number
--- @return number
function Constrain(lower, mid, upper)
	return math.max(lower, math.min(mid, upper));
end

--- Get random multiplier normalized to 1
--- @param variance number
--- @param rand number? # Optional random value in range [0, 1)
--- @param higher boolean?
--- @return number # >= 1 if `higher` is `true`, <= 1 if `higher` is `false`, random otherwise
function GetRandomMultiplier(variance, rand, higher)
	higher = higher or (1 == Map.Rand(2, "GetRandomMultiplier"));

	local multiplier = 1;
	if not rand then
		multiplier = 1 + Map.Rand(10000, "GetRandomMultiplier") / 10000 * variance;
	else
		multiplier = 1 + rand * variance;
	end

	if higher then
		return multiplier;
	else
		return 1 / multiplier;
	end
end

--- Get random integer between min and max inclusive
--- @param min integer
--- @param max integer
--- @return integer
function GetRandomFromRangeInclusive(min, max)
	if min == max then
		return min;
	elseif min > max then
		return GetRandomFromRangeInclusive(max, min);
	end
	return min + Map.Rand(max - min + 1, "GetRandomFromRangeInclusive");
end

--- Get a random option from a list
--- @generic T
--- @param tOptions T[] # Cannot be empty
--- @param strLogMessage string # Log message printed to RandCalls.csv
--- @return T
function GetRandomFromList(tOptions, strLogMessage)
	if not next(tOptions) then
		error("Empty array passed into GetRandomFromList");
	end
	local iRand = 1 + Map.Rand(#tOptions, strLogMessage);
	return tOptions[iRand];
end

--- Get a random option and its index from a weighted list
--- @generic T
--- @param tOptions T[] # Cannot be empty
--- @param tWeights number[] # Weights must not be negative and total weight must be positive
--- @param strLogMessage string # Log message printed to RandCalls.csv
--- @return T, integer
function GetRandomFromWeightedList(tOptions, tWeights, strLogMessage)
	if not next(tOptions) or not next(tWeights) then
		error("Empty array passed into GetRandomFromWeightedList");
	end

	if #tOptions ~= #tWeights then
		error("GetRandomFromWeightedList: Option array and weight array are of different lengths");
	end

	local tThresholds = {};
	local iTotalWeight = 0;
	for _, iWeight in ipairs(tWeights) do
		iTotalWeight = iTotalWeight + iWeight;
		table.insert(tThresholds, iTotalWeight);
	end

	assert(iTotalWeight > 0);

	local iRand = Map.Rand(iTotalWeight, strLogMessage);
	for i, iThreshold in ipairs(tThresholds) do
		if iThreshold > iRand then
			return tOptions[i], i;
		end
	end

	error("UNREACHABLE");
end

--- Get a random option and its index from a weighted table (key, weight).
--- Weights must not be negative and total weight must be positive.
--- @generic T
--- @param tOptionWeight table<T, number> # Cannot be empty
--- @param strLogMessage string # Log message printed to RandCalls.csv
--- @return T
function GetRandomFromWeightedTable(tOptionWeight, strLogMessage)
	if not next(tOptionWeight) then
		error("Empty table passed into GetRandomFromWeightedTable");
	end

	local tOptions = {};
	local tWeights = {};

	for option, iWeight in pairs(tOptionWeight) do
		table.insert(tOptions, option);
		table.insert(tWeights, iWeight);
	end

	local randOption = GetRandomFromWeightedList(tOptions, tWeights, strLogMessage);
	return randOption;
end

--- Given a weighted adjacency matrix, find the matching with the lowest total score using Kuhn-Munkres algorithm
--- @param tCostMatrix number[][] Weights range from -99 to 1
--- @return table<integer, integer> # Table of Row, Column mapping
--- @return number # Total score
function GetMinMatching(tCostMatrix)
	local iRowCount = #tCostMatrix;
	local iColCount = 0;
	for i = 1, iRowCount do
		if #tCostMatrix[i] > iColCount then
			iColCount = #tCostMatrix[i];
		end
	end

	local iSize = math.max(iRowCount, iColCount);

	-- Create an array of sequential row indices [1, 2, 3, ... iSize]
	local tRowOrder = {};
	for i = 1, iSize do
		tRowOrder[i] = i;
	end
	Shuffle(tRowOrder);

	local fPaddingCost = 100.0;
	local tPaddedMatrix = {};
	for i = 1, iSize do
		tPaddedMatrix[i] = {};
		local iOriginalRow = tRowOrder[i]; -- Extract the shuffled map pointer
		for j = 1, iSize do
			if tCostMatrix[iOriginalRow] and tCostMatrix[iOriginalRow][j] then
				tPaddedMatrix[i][j] = tCostMatrix[iOriginalRow][j];
			else
				tPaddedMatrix[i][j] = fPaddingCost;
			end
		end
	end

	local tRowLabel = {};
	local tColLabel = {};
	local tColMatchedTo = {};
	local tMinSlackInCol = {};
	local tVisitedRow = {};
	local tVisitedCol = {};

	for i = 1, iSize do
		tRowLabel[i] = math.huge;
		tColLabel[i] = 0;
		tColMatchedTo[i] = -1;
		for j = 1, iSize do
			if tPaddedMatrix[i][j] < tRowLabel[i] then
				tRowLabel[i] = tPaddedMatrix[i][j];
			end
		end
	end

	local function FindPath(iRowIndex)
		tVisitedRow[iRowIndex] = true;
		for iColIndex = 1, iSize do
			if not tVisitedCol[iColIndex] then
				local fGap = tPaddedMatrix[iRowIndex][iColIndex] - tRowLabel[iRowIndex] - tColLabel[iColIndex];

				if fGap < 1e-9 then
					tVisitedCol[iColIndex] = true;
					if tColMatchedTo[iColIndex] == -1 or FindPath(tColMatchedTo[iColIndex]) then
						tColMatchedTo[iColIndex] = iRowIndex;
						return true;
					end
				else
					if tMinSlackInCol[iColIndex] > fGap then
						tMinSlackInCol[iColIndex] = fGap;
					end
				end
			end
		end
		return false;
	end

	for i = 1, iSize do
		-- Clear slack properly between tree searches
		for j = 1, iSize do
			tMinSlackInCol[j] = math.huge;
		end

		while true do
			for j = 1, iSize do
				tVisitedRow[j] = false;
				tVisitedCol[j] = false;
			end

			if FindPath(i) then
				break;
			end

			local fDelta = math.huge;
			for j = 1, iSize do
				if not tVisitedCol[j] and tMinSlackInCol[j] < fDelta then
					fDelta = tMinSlackInCol[j];
				end
			end

			if fDelta == math.huge or fDelta < 1e-9 then
				break;
			end

			for j = 1, iSize do
				if tVisitedRow[j] then
					tRowLabel[j] = tRowLabel[j] + fDelta;
				end
				if tVisitedCol[j] then
					tColLabel[j] = tColLabel[j] - fDelta;
				else
					tMinSlackInCol[j] = tMinSlackInCol[j] - fDelta;
				end
			end
		end
	end

	-- Invert the mapping back to the caller's layout
	local tFinalAssignments = {};
	local fMinTotalScore = 0;
	for iColIndex = 1, iSize do
		local iShuffledRowIndex = tColMatchedTo[iColIndex];
		if iShuffledRowIndex ~= -1 then
			local iOriginalRowIndex = tRowOrder[iShuffledRowIndex];

			if iOriginalRowIndex <= iRowCount and iColIndex <= iColCount then
				tFinalAssignments[iOriginalRowIndex] = iColIndex;
				fMinTotalScore = fMinTotalScore + tCostMatrix[iOriginalRowIndex][iColIndex];
			end
		end
	end

	return tFinalAssignments, fMinTotalScore;
end
