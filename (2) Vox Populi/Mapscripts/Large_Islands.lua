------------------------------------------------------------------------------
--	FILE:	 Large_Islands.lua
--	AUTHOR:  Bob Thomas
--	PURPOSE: Global map script - Produces a world of islands, large and tiny.
------------------------------------------------------------------------------
--	Copyright (c) 2012 Firaxis Games, Inc. All rights reserved.
------------------------------------------------------------------------------

include("MapGenerator");
include("MultilayeredFractal");
include("FeatureGenerator");
include("TerrainGenerator");
include("IslandMaker");

------------------------------------------------------------------------------
function GetMapScriptInfo()
	local world_age, temperature, rainfall, _, resources = GetCoreMapOptions();
	return {
		Name = "TXT_KEY_MAP_LARGE_ISLANDS_VP",
		Description = "TXT_KEY_MAP_LARGE_ISLANDS_HELP",
		IsAdvancedMap = false,
		IconIndex = 2,
		SortIndex = 3,
		CustomOptions = {world_age, temperature, rainfall, resources},
	};
end
------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {30, 24},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {50, 40},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {60, 48},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {80, 56},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {100, 64},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {120, 80},
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

-------------------------------------------------------------------------------
function MultilayeredFractal:GeneratePlotsByRegion()
	-- Sirian's MultilayeredFractal controlling function.
	-- You -MUST- customize this function for each script using MultilayeredFractal.

	-- This implementation is specific to Large Islands.
	local iNumCivs = 0;
	for i = 0, GameDefines.MAX_MAJOR_CIVS - 1 do
		local player = Players[i];
		if player:IsEverAlive() then
			iNumCivs = iNumCivs + 1;
		end
	end

	-- Get user inputs.
	local world_age = Map.GetCustomOption(1);
	if world_age == 4 then
		world_age = 1 + Map.Rand(3, "Random World Age - Lua");
	end

	-- A cell system will be used to generate the large islands.
	-- Define the cell traits. (These need to fit correctly with the map grid width and height.)
	local iW, iH = Map.GetGridSize();
	local iCellWidth = 5;
	local iCellHeight = 4;
	local iNumCellColumns = math.floor(iW / iCellWidth);
	local iNumCellRows = math.floor(iH / iCellHeight);
	local iNumTotalCells = iNumCellColumns * iNumCellRows;
	local cell_data = table.fill(false, iNumTotalCells) -- Stores data on map cells in use. All cells begin as empty.
	local iNumCellsInUse = 0;
	local iNumCellTarget = math.floor(iNumTotalCells * 0.7);

	-- Add randomly generated island groups
	local iNumGroups = math.floor(iNumCivs * 1.65);
	print("-"); print("- Target number of Large Islands:", iNumGroups); print("-");
	local cannot_find_valid_cell = false;
	for group = 1, iNumGroups do
		if iNumCellsInUse >= iNumCellTarget or cannot_find_valid_cell then -- Map has reeached desired island population.
			print("-"); print("** Cannot place all Large Islands. Actual number placed:", group - 1); print("-");
			break;
		end

		--[[ Formation Chart
		1. Rectangle 4x2
		2. Rectangle 2x4
		3. Rectangle 3x3
		--]]

		-- Choose a formation
		local rate_threshold = {};
		local total_appearance_rate, iNumFormations = 0, 0;
		local appearance_rates = { -- These numbers are relative to one another. No specific target total is necessary.
			5, -- #1
			5, -- #2
			6, -- #3
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

		-- Choose cell(s) not in use;
		local iNumAttemptsToFindOpenCells = 0;
		local found_unoccupied_cell = false;
		local anchor_cell, cell_x, cell_y;
		while not found_unoccupied_cell and not cannot_find_valid_cell do
			if iNumAttemptsToFindOpenCells > 100 then -- Too many attempts on this pass. Might not be any valid locations.
				cannot_find_valid_cell = true;
			else
				diceroll = 1 + Map.Rand(iNumTotalCells, "Choosing a cell for an island group - Polynesia LUA");
				if not cell_data[diceroll] then -- Anchor cell is unoccupied.
					-- If formation type is multiple-cell, all secondary cells must also be unoccupied.
					if formation_type == 1 then -- horizontal, 4x2.
						-- Check to see if anchor cell is too near to an edge. If so, reject.
						cell_x = math.fmod(diceroll, iNumCellColumns);
						if cell_x ~= 0 and cell_x < iNumCellColumns - 2 then
							cell_y = math.modf(diceroll / iNumCellColumns);
							cell_y = cell_y + 1;
							if cell_y < iNumCellRows then
								-- Anchor cell is valid. Still have to check the other cells.
								if not (
									cell_data[diceroll + 1] or
									cell_data[diceroll + 2] or
									cell_data[diceroll + 3] or
									cell_data[diceroll + iNumCellColumns] or
									cell_data[diceroll + iNumCellColumns + 1] or
									cell_data[diceroll + iNumCellColumns + 2] or
									cell_data[diceroll + iNumCellColumns + 3]
								) then
									-- All cells are open.
									anchor_cell = diceroll;
									found_unoccupied_cell = true;
								end
							end
						end
					elseif formation_type == 2 then -- vertical, 2x4.
						-- Check to see if anchor cell is too near to an edge. If so, reject.
						cell_x = math.fmod(diceroll, iNumCellColumns);
						if cell_x ~= 0 then
							cell_y = math.modf(diceroll / iNumCellColumns);
							cell_y = cell_y + 1;
							if cell_y < iNumCellRows - 2 then
								-- Anchor cell is valid. Still have to check the other cells.
								if not (
									cell_data[diceroll + 1] or
									cell_data[diceroll + iNumCellColumns] or
									cell_data[diceroll + iNumCellColumns + 1] or
									cell_data[diceroll + iNumCellColumns * 2] or
									cell_data[diceroll + iNumCellColumns * 2 + 1] or
									cell_data[diceroll + iNumCellColumns * 3] or
									cell_data[diceroll + iNumCellColumns * 3 + 1]
								) then
									-- All cells are open.
									anchor_cell = diceroll;
									found_unoccupied_cell = true;
								end
							end
						end
					elseif formation_type == 3 then -- square, 3x3.
						-- Check to see if anchor cell is too near to an edge. If so, reject.
						cell_x = math.fmod(diceroll, iNumCellColumns);
						if cell_x ~= 0 and cell_x < iNumCellColumns - 1 then
							cell_y = math.modf(diceroll / iNumCellColumns);
							cell_y = cell_y + 1;
							if cell_y < iNumCellRows - 1 then
								-- Anchor cell is valid. Still have to check the other cells.
								if not (
									cell_data[diceroll + 1] or
									cell_data[diceroll + 2] or
									cell_data[diceroll + iNumCellColumns] or
									cell_data[diceroll + iNumCellColumns + 1] or
									cell_data[diceroll + iNumCellColumns + 2] or
									cell_data[diceroll + iNumCellColumns * 2] or
									cell_data[diceroll + iNumCellColumns * 2 + 1] or
									cell_data[diceroll + iNumCellColumns * 2 + 2]
								) then
									-- All cells are open.
									anchor_cell = diceroll;
									found_unoccupied_cell = true;
								end
							end
						end
					end
				end
				iNumAttemptsToFindOpenCells = iNumAttemptsToFindOpenCells + 1;
			end
		end

		-- Execute remainder of process on this loop only if a valid cell was found.
		if not cannot_find_valid_cell then

			-- Find Cell X and Y
			cell_x = math.fmod(anchor_cell, iNumCellColumns);
			if cell_x == 0 then
				cell_x = iNumCellColumns;
			end
			cell_y = math.modf(anchor_cell / iNumCellColumns);
			cell_y = cell_y + 1;

			-- Debug
			-- print("-"); print("-"); print("* Group# " .. group, "Formation Type: " .. formation_type, "Cell X, Y: " .. cell_x .. ", " .. cell_y);

			-- Create this large island.
			local iWidth, iHeight; -- Scope the variables needed for island group creation.
			local iWestX = iCellWidth * (cell_x - 1) + 1;
			local iSouthY = iCellHeight * (cell_y - 1) + 1;
			local iCohesionW, iCohesionH;
			local iCoWestX, iCoSouthY;
			local iWater, iCohesionWater = 55, 60;
			local FracXExp, FracYExp = 5, 5;
			if formation_type == 1 then -- four cells wide, two cells tall.
				iWidth = iCellWidth * 4 - 2;
				iHeight = iCellHeight * 2 - 2;
				FracXExp = 6;
				iCohesionW = math.floor(iWidth / 2);
				iCohesionH = math.floor(iHeight / 2);
				iCoWestX = iWestX + math.floor(iWidth / 4);
				iCoSouthY = iSouthY + math.floor(iHeight / 4);
			elseif formation_type == 2 then -- two cells wide, four cells tall.
				iWidth = iCellWidth * 2 - 2;
				iHeight = iCellHeight * 4 - 2;
				FracYExp = 6;
				iCohesionW = math.floor(iWidth / 2);
				iCohesionH = math.floor(iHeight / 2);
				iCoWestX = iWestX + math.floor(iWidth / 4);
				iCoSouthY = iSouthY + math.floor(iHeight / 4);
			elseif formation_type == 3 then -- three cells wide, three cells tall.
				iWidth = iCellWidth * 3 - 2;
				iHeight = iCellHeight * 3 - 2;
				iWater = 60;
				iCohesionW = math.floor(iWidth / 2);
				iCohesionH = math.floor(iHeight / 2);
				iCoWestX = iWestX + math.floor(iWidth / 4);
				iCoSouthY = iSouthY + math.floor(iHeight / 4);
			end

			-- Execute fractal layers for this Large Island.
			local args = {};
			args.iWaterPercent = iWater;
			args.iRegionWidth = iWidth;
			args.iRegionHeight = iHeight;
			args.iRegionWestX = iWestX;
			args.iRegionSouthY = iSouthY;
			args.iRegionPlotFlags = self.iRoundFlags;
			args.iRegionFracXExp = FracXExp;
			args.iRegionFracYExp = FracYExp;
			args.iRiftGrain = -1;

			self:GenerateFractalLayerWithoutHills(args);

			args = {};
			args.iWaterPercent = iCohesionWater;
			args.iRegionWidth = iCohesionW;
			args.iRegionHeight = iCohesionH;
			args.iRegionWestX = iCoWestX;
			args.iRegionSouthY = iCoSouthY;
			args.iRegionPlotFlags = self.iRoundFlags;
			args.iRegionFracXExp = FracXExp;
			args.iRegionFracYExp = FracYExp;
			args.iRiftGrain = -1;

			self:GenerateFractalLayerWithoutHills(args);

			-- Record cells in use
			if formation_type == 1 then
				cell_data[anchor_cell] = true;
				cell_data[anchor_cell + 1] = true;
				cell_data[anchor_cell + 2] = true;
				cell_data[anchor_cell + 3] = true;
				cell_data[anchor_cell + iNumCellColumns] = true;
				cell_data[anchor_cell + iNumCellColumns + 1] = true;
				cell_data[anchor_cell + iNumCellColumns + 2] = true;
				cell_data[anchor_cell + iNumCellColumns + 3] = true;
				iNumCellsInUse = iNumCellsInUse + 8;
			elseif formation_type == 2 then
				cell_data[anchor_cell] = true;
				cell_data[anchor_cell + 1] = true;
				cell_data[anchor_cell + iNumCellColumns] = true;
				cell_data[anchor_cell + iNumCellColumns + 1] = true;
				cell_data[anchor_cell + iNumCellColumns * 2] = true;
				cell_data[anchor_cell + iNumCellColumns * 2 + 1] = true;
				cell_data[anchor_cell + iNumCellColumns * 3] = true;
				cell_data[anchor_cell + iNumCellColumns * 3 + 1] = true;
				iNumCellsInUse = iNumCellsInUse + 8;
			elseif formation_type == 3 then
				cell_data[anchor_cell] = true;
				cell_data[anchor_cell + 1] = true;
				cell_data[anchor_cell + 2] = true;
				cell_data[anchor_cell + iNumCellColumns] = true;
				cell_data[anchor_cell + iNumCellColumns + 1] = true;
				cell_data[anchor_cell + iNumCellColumns + 2] = true;
				cell_data[anchor_cell + iNumCellColumns * 2] = true;
				cell_data[anchor_cell + iNumCellColumns * 2 + 1] = true;
				cell_data[anchor_cell + iNumCellColumns * 2 + 2] = true;
				iNumCellsInUse = iNumCellsInUse + 9;
			end
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
		print("Row: ", table.concat(stringdata));
	end
	--]]

	-- Large islands are set. Now apply hills and mountains.
	local args = {
		extra_mountains = 10,
		adjust_plates = 2,
		tectonic_islands = true,
		world_age = world_age,
	};
	self:ApplyTectonics(args);

	-- Now we begin to add the island chains.
	print("-"); print("- Large Islands placed. Begin placement of tiny island chains."); print("-");

	-- A cell system will be used to combine predefined land chunks with randomly generated island groups.
	-- Define the cell traits. (These need to fit correctly with the map grid width and height.)
	iW, iH = Map.GetGridSize();
	iCellWidth = 10;
	iCellHeight = 8;
	iNumCellColumns = math.floor(iW / iCellWidth);
	iNumCellRows = math.floor(iH / iCellHeight);
	iNumTotalCells = iNumCellColumns * iNumCellRows;
	cell_data = table.fill(false, iNumTotalCells) -- Stores data on map cells in use. All cells begin as empty.
	iNumCellsInUse = 0;
	iNumCellTarget = math.floor(iNumTotalCells * 0.7);
	local island_chain_PlotTypes = table.fill(PlotTypes.PLOT_OCEAN, iW * iH);

	-- Add randomly generated island groups
	iNumGroups = iNumCellTarget; -- Should virtually never use all the groups.
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
				--print("-"); print("*-* ALERT! Formation type of", formation_type, "for island group#", group, "unable to find an open space. Switching to single-cell.");
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
					if cell_x ~= 0 then -- Anchor cell is valid, but still have to check adjacent cell.
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
				if plot_data[data_index] == true then -- This plot is land.
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
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = y * iW + x + 1;
			if island_chain_PlotTypes[i] ~= PlotTypes.PLOT_OCEAN then
				if self.wholeworldPlotTypes[i] == PlotTypes.PLOT_OCEAN then
					self.wholeworldPlotTypes[i] = island_chain_PlotTypes[i];
				end
			end
		end
	end

	-- Plot Type generation completed. Return global plot array.
	return self.wholeworldPlotTypes;
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Generating Plot Types (Lua Large Islands) ...");

	local layered_world = MultilayeredFractal.Create();
	local plotsLI = layered_world:GeneratePlotsByRegion();

	SetPlotTypes(plotsLI);

	GenerateCoasts();
end
------------------------------------------------------------------------------
function GenerateTerrain()
	print("Generating Terrain (Lua Large Islands) ...");

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
	print("Adding Features (Lua Large Islands) ...");

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
function StartPlotSystem()
	-- Get Resources setting input by user.
	local res = Map.GetCustomOption(4);
	if res == 6 then
		res = 1 + Map.Rand(3, "Random Resources Option - Lua");
	end

	print("Creating start plot database (MapGenerator.Lua)");
	local start_plot_database = AssignStartingPlots.Create();

	print("Dividing the map in to Regions (Lua Large Islands)");
	local args = {
		method = 3,
		resources = res,
	};
	start_plot_database:GenerateRegions(args);

	print("Choosing start locations for civilizations (Lua Large Islands)");
	-- Forcing starts along the ocean.
	-- Lowering start position minimum eligibility thresholds.
	args = {
		mustBeCoast = true,
		minFoodMiddle = 2,
		minFoodOuter = 3,
		minProdOuter = 1,
	};
	start_plot_database:ChooseLocations(args);

	print("Normalizing start locations and assigning them to Players (MapGenerator.Lua)");
	start_plot_database:BalanceAndAssign();

	print("Placing Natural Wonders (MapGenerator.Lua)");
	start_plot_database:PlaceNaturalWonders();

	print("Placing Resources and City States (MapGenerator.Lua)");
	start_plot_database:PlaceResourcesAndCityStates();

	-- Tell the AI that we should treat this as a offshore expansion (+ naval) map
	Map.ChangeAIMapHint(4 + 1);

end
------------------------------------------------------------------------------
