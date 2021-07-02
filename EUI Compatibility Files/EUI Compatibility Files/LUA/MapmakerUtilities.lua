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
------------------------------------------------------------------------- ]]--

------------------------------------------------------------------------------
function GetPlayerAndTeamInfo()
	-- This function obtains Civ count, CS count, correct player IDs, and basic Team information.
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
	local iNumTeams = Game.CountCivTeamsEverAlive()
	local iNumTeamsOfCivs = iNumTeams - iNumCityStates;
	if iNumTeamsOfCivs < iNumCivs then
		bTeamGame = true;
	end

	local teams_with_major_civs = {};
	local number_civs_per_team = table.fill(0, GameDefines.MAX_CIV_PLAYERS)
	number_civs_per_team[0] = 0;
	for i = 0, GameDefines.MAX_MAJOR_CIVS - 1 do
		local player = Players[i];
		if player:IsEverAlive() then
			local teamID = player:GetTeam()
			number_civs_per_team[teamID] = number_civs_per_team[teamID] + 1;
			local bCheckTeamList = TestMembership(teams_with_major_civs, teamID)
			if bCheckTeamList == false then
				table.insert(teams_with_major_civs, teamID)
			end
		end
	end
	
	return iNumCivs, iNumCityStates, player_ID_list, bTeamGame, teams_with_major_civs, number_civs_per_team
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function ObtainLandmassBoundaries(iAreaID)
	local iW, iH = Map.GetGridSize();
	-- Set up variables that will be returned by this function.
	local wrapsX = false;
	local wrapsY = false;
	local iWestX, iEastX, iSouthY, iNorthY, iWidth, iHeight;
	
	if Map:IsWrapX() then -- Check to see if landmass Wraps X.
		local foundFirstColumn = false;
		local foundLastColumn = false;
		for y = 0, iH - 1 do
			local plotFirst = Map.GetPlot(0, y);
			local plotLast = Map.GetPlot(iW - 1, y);
			local area = plotFirst:GetArea();
			if area == iAreaID then -- Found a plot belonging to iAreaID in first column.
				foundFirstColumn = true;
			end
			area = plotLast:GetArea();
			if area == iAreaID then -- Found a plot belonging to iAreaID in last column.
				foundLastColumn = true;
			end
		end
		if foundFirstColumn and foundLastColumn then -- Plot on both sides of map edge.
			wrapsX = true;
		end
	end
	
	if Map:IsWrapY() then -- Check to see if landmass Wraps Y.
		local foundFirstRow = false;
		local foundLastRow = false;
		for y = 0, iH - 1 do
			local plotFirst = Map.GetPlot(x, 0);
			local plotLast = Map.GetPlot(x, iH - 1);
			local area = plotFirst:GetArea();
			if area == iAreaID then -- Found a plot belonging to iAreaID in first row.
				foundFirstRow = true;
			end
			area = plotLast:GetArea();
			if area == iAreaID then -- Found a plot belonging to iAreaID in last row.
				foundLastRow = true;
			end
		end
		if foundFirstRow and foundLastRow then -- Plot on both sides of map edge.
			wrapsY = true;
		end
	end

	-- Find West and East edges of this landmass.
	if not wrapsX then -- no X wrap
		for x = 0, iW - 1 do -- Check for any area membership one column at a time, left to right.
			local foundAreaInColumn = false;
			for y = 0, iH - 1 do -- Checking column.
				local plot = Map.GetPlot(x, y);
				local area = plot:GetArea();
				if area == iAreaID then -- Found a plot belonging to iAreaID, set WestX to this column.
					foundAreaInColumn = true;
					iWestX = x;
					break
				end
			end
			if foundAreaInColumn then -- Found WestX, done looking.
				break
			end
		end
		for x = iW - 1, 0, -1 do -- Check for any area membership one column at a time, right to left.
			local foundAreaInColumn = false;
			for y = 0, iH - 1 do -- Checking column.
				local plot = Map.GetPlot(x, y);
				local area = plot:GetArea();
				if area == iAreaID then -- Found a plot belonging to iAreaID, set EastX to this column.
					foundAreaInColumn = true;
					iEastX = x;
					break
				end
			end
			if foundAreaInColumn then -- Found EastX, done looking.
				break
			end
		end
	else -- Landmass Xwraps.
		local landmassSpansEntireWorldX = true;
		for x = iW - 2, 1, -1 do -- Check for end of area membership one column at a time, right to left.
			local foundAreaInColumn = false;
			for y = 0, iH - 1 do -- Checking column.
				local plot = Map.GetPlot(x, y);
				local area = plot:GetArea();
				if area == iAreaID then -- Found a plot belonging to iAreaID, will have to check the next column too.
					foundAreaInColumn = true;
				end
			end
			if not foundAreaInColumn then -- Found empty column, which is just west of WestX.
				iWestX = x + 1;
				landmassSpansEntireWorldX = false;
				break
			end
		end
		for x = 1, iW - 2 do -- Check for end of area membership one column at a time, left to right.
			local foundAreaInColumn = false;
			for y = 0, iH - 1 do -- Checking column.
				local plot = Map.GetPlot(x, y);
				local area = plot:GetArea();
				if area == iAreaID then -- Found a plot belonging to iAreaID, will have to check the next column too.
					foundAreaInColumn = true;
				end
			end
			if not foundAreaInColumn then -- Found empty column, which is just east of EastX.
				iEastX = x - 1;
				landmassSpansEntireWorldX = false;
				break
			end
		end
		-- If landmass spans entire world, we'll treat it as if it does not wrap.
		if landmassSpansEntireWorldX then
			wrapsX = false;
			iWestX = 0;
			iEastX = iW - 1;
		end
	end
				
	-- Find South and North edges of this landmass.
	if not wrapsY then -- no Y wrap
		for y = 0, iH - 1 do -- Check for any area membership one row at a time, bottom to top.
			local foundAreaInRow = false;
			for x = 0, iW - 1 do -- Checking row.
				local plot = Map.GetPlot(x, y);
				local area = plot:GetArea();
				if area == iAreaID then -- Found a plot belonging to iAreaID, set SouthY to this row.
					foundAreaInRow = true;
					iSouthY = y;
					break
				end
			end
			if foundAreaInRow then -- Found SouthY, done looking.
				break
			end
		end
		for y = iH - 1, 0, -1 do -- Check for any area membership one row at a time, top to bottom.
			local foundAreaInRow = false;
			for x = 0, iW - 1 do -- Checking row.
				local plot = Map.GetPlot(x, y);
				local area = plot:GetArea();
				if area == iAreaID then -- Found a plot belonging to iAreaID, set NorthY to this row.
					foundAreaInRow = true;
					iNorthY = y;
					break
				end
			end
			if foundAreaInRow then -- Found NorthY, done looking.
				break
			end
		end
	else -- Landmass Ywraps.
		local landmassSpansEntireWorldY = true;
		for y = iH - 2, 1, -1 do -- Check for end of area membership one row at a time, top to bottom.
			local foundAreaInRow = false;
			for x = 0, iW - 1 do -- Checking row.
				local plot = Map.GetPlot(x, y);
				local area = plot:GetArea();
				if area == iAreaID then -- Found a plot belonging to iAreaID, will have to check the next row too.
					foundAreaInRow = true;
				end
			end
			if not foundAreaInRow then -- Found empty row, which is just south of southY.
				iSouthY = y + 1;
				landmassSpansEntireWorldY = false;
				break
			end
		end
		for y = 1, iH - 2 do -- Check for end of area membership one row at a time, bottom to top.
			local foundAreaInRow = false;
			for x = 0, iW - 1 do -- Checking row.
				local plot = Map.GetPlot(x, y);
				local area = plot:GetArea();
				if area == iAreaID then -- Found a plot belonging to iAreaID, will have to check the next row too.
					foundAreaInRow = true;
				end
			end
			if not foundAreaInRow then -- Found empty column, which is just north of NorthY.
				iNorthY = y - 1;
				landmassSpansEntireWorldY = false;
				break
			end
		end
		-- If landmass spans entire world, we'll treat it as if it does not wrap.
		if landmassSpansEntireWorldY then
			wrapsY = false;
			iSouthY = 0;
			iNorthY = iH - 1;
		end
	end
	
	-- Convert EastX and NorthY into width and height.
	if wrapsX then
		iWidth = (iEastX + iW) - iWestX + 1;
	else
		iWidth = iEastX - iWestX + 1;
	end
	if wrapsY then
		iHeight = (iNorthY + iH) - iSouthY + 1;
	else
		iHeight = iNorthY - iSouthY + 1;
	end

	--[[ Log dump for debug purposes only, disable otherwise.
	print("--- Landmass Boundary Readout ---");
	print("West X:", iWestX, "East X:", iEastX);
	print("South Y:", iSouthY, "North Y:", iNorthY);
	print("Width:", iWidth, "Height:", iHeight);
	local plotTotal = iWidth * iHeight;
	print("Total Plots in 'landmass rectangle':", plotTotal);
	print("- - - - - - - - - - - - - - - - -");
	]]--

	-- Insert data into table, then return the table.
	local data = {iWestX, iSouthY, iEastX, iNorthY, iWidth, iHeight, wrapsX, wrapsY};
	return data
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function AdjacentToSaltWater(x, y)
	-- Checks a plot (x, y) to see if it is any type of land adjacent to at least one body of salt water.
	local plot = Map.GetPlot(x, y);
	local plotType = plot:GetPlotType()
	if plotType ~= PlotTypes.PLOT_OCEAN then -- This plot is land, process it.
		-- Check all adjacent plots to see if any of those are salt water.
		local directions = { DirectionTypes.DIRECTION_NORTHEAST,
		                     DirectionTypes.DIRECTION_EAST,
		                     DirectionTypes.DIRECTION_SOUTHEAST,
		                     DirectionTypes.DIRECTION_SOUTHWEST,
		                     DirectionTypes.DIRECTION_WEST,
		                     DirectionTypes.DIRECTION_NORTHWEST };
		-- 
		for loop, current_direction in ipairs(directions) do
			local testPlot = Map.PlotDirection(x, y, current_direction);
			if testPlot ~= nil then
				local type = testPlot:GetPlotType()
				if type == PlotTypes.PLOT_OCEAN then -- Adjacent plot is water! Check if ocean or lake.
					if testPlot:IsLake() == false then -- Adjacent plot is salt water!
						return true
					end
				end
			end
		end
	end
	-- Current plot is itself water, or else no salt water found among adjacent plots.
	return false
end
------------------------------------------------------------------------------
function GenerateCoastalLandDataTable()
	local iW, iH = Map.GetGridSize();
	local plotDataIsCoastal = {};
	table.fill(plotDataIsCoastal, false, iW * iH);
	-- When generating a plot data table incrementally, process Y first so that plots go row by row.
	-- Keeping plot data table indices consistent with the main plot database could save you enormous grief.
	-- In this case, accessing by plot index, it doesn't matter.
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local plotIsAdjacent = AdjacentToSaltWater(x, y)
			if plotIsAdjacent then
				local i = iW * y + x + 1;
				plotDataIsCoastal[i] = true;
			end
		end
	end
	
	-- returns table
	return plotDataIsCoastal
end
------------------------------------------------------------------------------
function GenerateNextToCoastalLandDataTables()
	-- Set up data table for IsCoastal
	local plotDataIsCoastal = GenerateCoastalLandDataTable()

	-- Set up data table for IsNextToCoast
	local iW, iH = Map.GetGridSize();
	local plotDataIsNextToCoast = {};
	table.fill(plotDataIsNextToCoast, false, iW * iH);
	-- When generating a plot data table incrementally, process Y first so that plots go row by row.
	-- Keeping plot data table indices consistent with the main plot database could save you enormous grief.
	-- In this case, accessing an existing table by plot index, it doesn't matter.
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local i = iW * y + x + 1;
			local plot = Map.GetPlot(x, y);
			if plotDataIsCoastal[i] == false and not plot:IsWater() then -- plot is not itself on the coast or in the water.
				-- So we will check all adjacent plots to see if any of those are on the coast.
				local NEPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_NORTHEAST);
				local EPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_EAST);
				local SEPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_SOUTHEAST);
				local SWPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_SOUTHWEST);
				local WPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_WEST);
				local NWPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_NORTHWEST);
				-- 
				-- Check plot to northeast of current plot. This operation accounts for map edge and world wrap.
				if NEPlot ~= nil then
					local adjX = NEPlot:GetX();
					local adjY = NEPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataIsCoastal[adjI] == true then
						-- The current loop plot is not itself on the coast but is next to a plot that is on the coast.
						plotDataIsNextToCoast[i] = true;
					end
				end
				-- Check plot to east of current plot.
				if EPlot ~= nil then
					local adjX = EPlot:GetX();
					local adjY = EPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataIsCoastal[adjI] == true then
						plotDataIsNextToCoast[i] = true;
					end
				end
				-- Check plot to southeast of current plot.
				if SEPlot ~= nil then
					local adjX = SEPlot:GetX();
					local adjY = SEPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataIsCoastal[adjI] == true then
						plotDataIsNextToCoast[i] = true;
					end
				end
				-- Check plot to southwest of current plot.
				if SWPlot ~= nil then
					local adjX = SWPlot:GetX();
					local adjY = SWPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataIsCoastal[adjI] == true then
						plotDataIsNextToCoast[i] = true;
					end
				end
				-- Check plot to west of current plot.
				if WPlot ~= nil then
					local adjX = WPlot:GetX();
					local adjY = WPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataIsCoastal[adjI] == true then
						plotDataIsNextToCoast[i] = true;
					end
				end
				-- Check plot to northwest of current plot.
				if NWPlot ~= nil then
					local adjX = NWPlot:GetX();
					local adjY = NWPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataIsCoastal[adjI] == true then
						plotDataIsNextToCoast[i] = true;
					end
				end
			end
		end
	end
	
	-- returns table, table
	return plotDataIsCoastal, plotDataIsNextToCoast
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function CivNeedsCoastalStart(civType)
	-- This function retrieves Start Along Ocean bias from the XML.
	for row in GameInfo.Civilization_Start_Along_Ocean{CivilizationType = civType} do
		if(row.StartAlongOcean == true) then
			return true;
		end
	end
	return false;
end
------------------------------------------------------------------------------
function CivNeedsRiverStart(civType)
	-- This function retrieves Start Along River bias from the XML.
	for row in GameInfo.Civilization_Start_Along_River{CivilizationType = civType} do
		if(row.StartAlongRiver == true) then
			return true;
		end
	end
	return false;
end
------------------------------------------------------------------------------
function CivNeedsPlaceFirstCoastalStart(civType)
	for row in GameInfo.Civilization_Start_Place_First_Along_Ocean{CivilizationType = civType} do
		if(row.PlaceFirst == true) then
			return true;
		end
	end
	return false;
end
------------------------------------------------------------------------------
function GetNumStartRegionPriorityForCiv(civType)
	-- This function detects if a civ has Start Region Priority needs and counts how many.
	for row in DB.Query("select count(*) as count from Civilization_Start_Region_Priority where CivilizationType = ?", civType) do
		return row.count;
	end
	return 0;
end
------------------------------------------------------------------------------
function GetNumStartRegionAvoidForCiv(civType)
	-- This function detects if a civ has Start Region Avoid needs and counts how many.
	for row in DB.Query("select count(*) as count from Civilization_Start_Region_Avoid where CivilizationType = ?", civType) do
		return row.count;
	end
	return 0;
end
------------------------------------------------------------------------------
function GetStartRegionPriorityListForCiv_GetIDs(civType)
	-- This function returns a sorted list of all Start Region Priority for this civ.
	-- List will include ID of each region type, plus be sorted and contain only unique values.
	local priorityRegionTypes = {};
	for row in GameInfo.Civilization_Start_Region_Priority {CivilizationType = civType} do
		table.insert(priorityRegionTypes, row.RegionType);
	end
    local priorityRegionIDs = {};
    for i, v in ipairs(priorityRegionTypes) do
    	local region_type = GameInfo.Regions[v].ID;
    	local bTest = TestMembership(priorityRegionIDs, region_type);
    	if bTest == false then
    		table.insert(priorityRegionIDs, region_type);
    	end
    end
    table.sort(priorityRegionIDs);
    return priorityRegionIDs;
end
------------------------------------------------------------------------------
function GetStartRegionAvoidListForCiv_GetIDs(civType)
	-- This function returns a sorted list of all Start Region Avoid for this civ.
	-- List will include ID of each region type, plus be sorted and contain only unique values.
	local avoidRegionTypes = {};
	for row in GameInfo.Civilization_Start_Region_Avoid {CivilizationType = civType} do
		table.insert(avoidRegionTypes, row.RegionType);
	end
    local avoidRegionIDs = {};
    for i, v in ipairs(avoidRegionTypes) do
    	local region_type = GameInfo.Regions[v].ID;
    	local bTest = TestMembership(avoidRegionIDs, region_type);
    	if bTest == false then
    		table.insert(avoidRegionIDs, region_type);
    	end
    end
    table.sort(avoidRegionIDs);
    return avoidRegionIDs;
end
------------------------------------------------------------------------------
function GetStartRegionPriorityListForCiv_GetTypes(civType)
	-- This function returns an unsorted list of all Start Region Priority for this civ.
	-- List will include type of each region, not IDs. eg: REGION_DESERT, REGION_HILLS
	local priorityRegionTypes = {};
	for row in GameInfo.Civilization_Start_Region_Priority{CivilizationType = civType} do
		table.insert(priorityRegionTypes, row.RegionType);
	end
	return priorityRegionTypes;
end
------------------------------------------------------------------------------
function GetStartRegionAvoidListForCiv_GetTypes(civType)
	-- This function returns an unsorted list of all Start Region Avoid for this civ.
	-- List will include type of each region, not IDs. eg: REGION_DESERT, REGION_HILLS
	local avoidRegionTypes = {};
	for row in GameInfo.Civilization_Start_Region_Avoid{CivilizationType = civType} do
		table.insert(avoidRegionTypes, row.RegionType);
	end
	return avoidRegionTypes;
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function TestMembership(table, value)
	local testResult = false;
	for index, data in pairs(table) do
		if data == value then
			testResult = true;
			break
		end
	end
	return testResult
end
------------------------------------------------------------------------------
function GetShuffledCopyOfTable(incoming_table)
	-- Designed to operate on tables with no gaps. Does not affect original table.
	local len = table.maxn(incoming_table);
	local copy = {};
	local shuffledVersion = {};
	-- Make copy of table.
	for loop = 1, len do
		copy[loop] = incoming_table[loop];
	end
	-- One at a time, choose a random index from Copy to insert in to final table, then remove it from the copy.
	local left_to_do = table.maxn(copy);
	for loop = 1, len do
		local random_index = 1 + Map.Rand(left_to_do, "Shuffling table entry - Lua");
		table.insert(shuffledVersion, copy[random_index]);
		table.remove(copy, random_index);
		left_to_do = left_to_do - 1;
	end
	return shuffledVersion
end
------------------------------------------------------------------------------
function IdentifyTableIndex(incoming_table, value)
	-- Purpose of this function is to make it easy to remove a data entry from 
	-- a list (table) when the index of the entry is unknown.
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
------------------------------------------------------------------------------
function PrintContentsOfTable(incoming_table) -- For debugging purposes. LOT of table data being handled here.
	print("--------------------------------------------------");
	print("Table printout for table ID:", table);
	for index, data in pairs(incoming_table) do
		print("Table index:", index, "Table entry:", data);
	end
	print("- - - - - - - - - - - - - - - - - - - - - - - - - -");
end
------------------------------------------------------------------------------
