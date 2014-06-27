------------------------------------------------------------------------------
--	FILE:	  IslandMaker.lua
--	AUTHOR:   Bob Thomas
--	PURPOSE:  Functions designed to create realistic island chains.
------------------------------------------------------------------------------
--	Copyright (c) 2011 Firaxis Games, Inc. All rights reserved.
------------------------------------------------------------------------------

function print() end

--[[ -------------------------------------------------------------------------
CONTENTS OF THIS FILE:

* CreateSingleAxisIslandChain(iWidth, iHeight, fTilt)
* CreateAxisChainWithDots(iWidth, iHeight, fTilt, iInnerWidth, iInnerHeight, iNumDots)
* CreateAxisChainWithShiftedDots(iWidth, iHeight, fTilt, iInnerWidth, iInnerHeight, iInnerWest, iInnerSouth, iNumDots)
* CreateAxisChainWithDoubleDots(iWidth, iHeight, fTilt, iInnerWidth1, iInnerHeight1, iInnerWest1, iInnerSouth1,
                                iNumDots1, iInnerWidth2, iInnerHeight2, iInnerWest2, iInnerSouth2, iNumDots2)
------------------------------------------------------------------------- ]]--

------------------------------------------------------------------------------
function CreateSingleAxisIslandChain(iWidth, iHeight, fTilt)
	-- This function will create the simplest form of island chain. (fTilt should be in degrees.)
	-- Data returned will be a table of LOCAL plot indices, true for land, false for water.
	local iWH = iWidth * iHeight;
	-- If dimensions are too small to operate, or the angle is not between 0 and 180 degrees, abort.
	if iWidth < 4 or iHeight < 4 or iWH < 24 or fTilt < 0 or fTilt > 180 then
		print("*** ERROR! ***"); print("Cell dimensions or Axis angle invalid!"); print("-");
		return nil
	end
	-- This function is going to execute, so set up a data table for the plots.
	local plot_data = table.fill(false, iWH);
	-- Define the center and the radii.
	local iCenterX, iCenterY = (iWidth - 1) / 2, (iHeight - 1) / 2;
	local fRadiusX, fRadiusY = iWidth / 2, iHeight / 2;
	-- Using triangular method of trigonometry. If angle > 90 degrees, swap sides and convert the angle.
	local fTilt_in_degrees = fTilt;
	if fTilt > 90 then
		fTilt_in_degrees = 180 - fTilt;
	end
	-- Convert angle to radians. (Lua math table functions operate on radians).
	local fTilt_in_radians = math.rad(fTilt_in_degrees);
	-- Determine traits of the angle.
	local is_horizontal, is_vertical, is_steep, leans_left = false, false, false, false;
	if fTilt == 0 or fTilt == 180 then
		is_horizontal = true;
	elseif fTilt == 90 then
		is_vertical = true;
	else
		if fTilt > 45 and fTilt < 135 then
			is_steep = true;
		end
		if fTilt > 90 then
			leans_left = true;
		end
	end
	-- Obtain start and end plots for the axis.
	local start_x, start_y, end_x, end_y;
	if is_horizontal then
		start_x, start_y, end_x, end_y = 0, iCenterY, iWidth - 1, iCenterY;
	elseif is_vertical then
		start_x, start_y, end_x, end_y = iCenterX, 0, iCenterX, iHeight - 1;
	else
		-- Obtain the tangent of this angle.
		local TAN = math.tan(fTilt_in_radians);
		-- Figure out which axis to use as the measurement basis for the triangle.
		-- (The one that won't let the trangle exceed the defined width and height).
		local width_viable, height_viable = false, false;
		--
		-- Check with width as primary axis.
		local width_check = fRadiusY / TAN;
		if width_check <= fRadiusX then
			width_viable = true;
		end
		-- Check with height as primary axis.
		local height_check = fRadiusX * TAN;
		if height_check <= fRadiusY then
			height_viable = true;
		end
		-- Check for error.
		if width_viable == false and height_viable == false then
			-- Neither width nor height checked out as viable? That should not be mathematically possible!
			print("!!!"); print("! ERROR ! - Trigonometric failure !"); print("!!!");
			return nil
		end
		-- Assign the coordinates.
		if width_viable then
			if leans_left == false then
				start_x, start_y, end_x, end_y = iCenterX - width_check, 0, iCenterX + width_check, iHeight - 1;
			else
				start_x, start_y, end_x, end_y = iCenterX - width_check, iHeight - 1, iCenterX + width_check, 0;
			end
		elseif height_viable then
			if leans_left == false then
				start_x, start_y, end_x, end_y = 0, iCenterY - height_check, iWidth - 1, iCenterY + height_check;
			else
				start_x, start_y, end_x, end_y = 0, iCenterY + height_check, iWidth - 1, iCenterY - height_check;
			end
		else
			print("*** ERROR *** Width and Height Checks failed! ***");
		end
	end
	
	-- Debugging
	print("-"); print("- Traits List - Width:", iWidth, "Height:", iHeight, "Angle:", fTilt);
	print("-"); print("Degrees:", fTilt_in_degrees, " - Radians:", fTilt_in_radians); print("-");
	print("Start Coordinates:", start_x, start_y, " - End Coordinates:", end_x, end_y); print("-");

	local startX, startY, endX, endY, bReverseFlag, slope;
	-- Note: Incoming x/y are starting at 0/0, but we need to convert them to Lua array values that start at 1.
	if math.abs(end_y - start_y) < math.abs(end_x - start_x) then -- line is closer to horizontal
		if start_x > end_x then
			startX, startY, endX, endY = end_x + 1, end_y + 1, start_x + 1, start_y + 1; -- swap start and end
			bReverseFlag = true;
			print("Path reversed, working from the end plot.");
		else -- don't swap
			startX, startY, endX, endY = start_x + 1, start_y + 1, end_x + 1, end_y + 1;
			bReverseFlag = false;
			print("Path not reversed.");
		end
		local dx = endX - startX;
		local dy = endY - startY;
		if dx == 0 or dy == 0 then
			slope = 0;
		else
			slope = dy/dx;
		end
		print("Slope: ", slope);
		local y, y_true = nil, startY;
		-- Process the Axis.
		for x = startX, endX do
			local y_int, y_frac = math.modf(y_true);
			if y_frac >= 0.5 then
				y = math.ceil(y_true);
			else
				y = math.floor(y_true);
			end
			-- Determine if this plot is to become land.
			local diceroll = Map.Rand(5, "Island Making - Lua");
			if diceroll > 1 then -- Yes, turn this plot to land.
				local i = (y - 1) * iWidth + x;
				plot_data[i] = true;
			end	
			-- Adjust y for the next iteration of the loop.		
			y_true = y_true + slope;
			print("y plus slope: ", y_true);
		end

	else -- line is closer to vertical
		if start_y > end_y then
			startX, startY, endX, endY = end_x + 1, end_y + 1, start_x + 1, start_y + 1; -- swap start and end
			bReverseFlag = true;
			print("Path reversed, working from the end plot.");
		else -- don't swap
			startX, startY, endX, endY = start_x + 1, start_y + 1, end_x + 1, end_y + 1;
			bReverseFlag = false;
			print("Path not reversed.");
		end
		local dx = endX - startX;
		local dy = endY - startY;
		if dx == 0 or dy == 0 then
			slope = 0;
		else
			slope = dx/dy;
		end
		print("Slope: ", slope);
		local x, x_true = nil, startX;
		-- Process the Axis.
		for y = startY, endY do
			local x_int, x_frac = math.modf(x_true);
			if x_frac >= 0.5 then
				x = math.ceil(x_true);
			else
				x = math.floor(x_true);
			end
			-- Determine if this plot is to become land.
			local diceroll = Map.Rand(5, "Island Making - Lua");
			if diceroll > 1 then -- Yes, turn this plot to land.
				local i = (y - 1) * iWidth + x;
				plot_data[i] = true;
			end	
			-- Adjust x for the next iteration of the loop.		
			x_true = x_true + slope;
			print("x plus slope: ", x_true);
		end
	end
	
	return plot_data
end
------------------------------------------------------------------------------
function CreateAxisChainWithDots(iWidth, iHeight, fTilt, iInnerWidth, iInnerHeight, iNumDots)
	-- This function adds "dots" (randomly placed single plots of land) to an axis island chain.
	local plot_data = CreateSingleAxisIslandChain(iWidth, iHeight, fTilt)
	-- Error handling.
	if iInnerWidth > iWidth - 2 then
		iInnerWidth = iWidth - 2;
	end
	if iInnerHeight > iHeight - 2 then
		iInnerHeight = iHeight - 2;
	end
	local westX = 1 + math.floor((iWidth - iInnerWidth) / 2);
	local southY = 1 + math.floor((iHeight - iInnerHeight) / 2);
	-- Add dots. ... Note: No collision checks. Any dots placed on existing plots are lost. This is intended.
	for loop = 1, iNumDots do
		local randomX = westX + Map.Rand(iInnerWidth, "Random X coord for Dot - Lua");
		local randomY = southY + Map.Rand(iInnerHeight, "Random Y coord for Dot - Lua");
		local i = (randomY - 1) * iWidth + randomX;
		plot_data[i] = true;
	end

	return plot_data
end
------------------------------------------------------------------------------
function CreateAxisChainWithShiftedDots(iWidth, iHeight, fTilt, iInnerWidth, iInnerHeight, iInnerWest, iInnerSouth, iNumDots)
	-- The dots do not necessarily go in the middle, in this function. Use West and South to set the location of the dots.
	local plot_data = CreateSingleAxisIslandChain(iWidth, iHeight, fTilt)
	-- Error handling.
	if iInnerWidth > iWidth - 2 then
		iInnerWidth = iWidth - 2;
	end
	if iInnerHeight > iHeight - 2 then
		iInnerHeight = iHeight - 2;
	end
	-- Add dots. ... Note: No collision checks. Any dots placed on existing plots are lost. This is intended.
	for loop = 1, iNumDots do
		local randomX = iInnerWest + Map.Rand(iInnerWidth, "Random X coord for Dot - Lua");
		local randomY = iInnerSouth + Map.Rand(iInnerHeight, "Random Y coord for Dot - Lua");
		local i = (randomY - 1) * iWidth + randomX;
		plot_data[i] = true;
	end

	return plot_data
end
------------------------------------------------------------------------------
function CreateAxisChainWithDoubleDots(iWidth, iHeight, fTilt, iInnerWidth1, iInnerHeight1, iInnerWest1, iInnerSouth1,
                                       iNumDots1, iInnerWidth2, iInnerHeight2, iInnerWest2, iInnerSouth2, iNumDots2)
	-- Two sets of dots, with control over size and location.
	local plot_data = CreateSingleAxisIslandChain(iWidth, iHeight, fTilt)
	-- Error handling.
	if iInnerWidth1 > iWidth - 2 then
		iInnerWidth1 = iWidth - 2;
	end
	if iInnerHeight1 > iHeight - 2 then
		iInnerHeight1 = iHeight - 2;
	end
	if iInnerWidth2 > iWidth - 2 then
		iInnerWidth2 = iWidth - 2;
	end
	if iInnerHeight2 > iHeight - 2 then
		iInnerHeight2 = iHeight - 2;
	end
	-- Dots1
	for loop = 1, iNumDots1 do
		local randomX = iInnerWest1 + Map.Rand(iInnerWidth1, "Random X coord for Dot - Lua");
		local randomY = iInnerSouth1 + Map.Rand(iInnerHeight1, "Random Y coord for Dot - Lua");
		local i = (randomY - 1) * iWidth + randomX;
		plot_data[i] = true;
	end
	-- Dots2
	for loop = 1, iNumDots2 do
		local randomX = iInnerWest2 + Map.Rand(iInnerWidth2, "Random X coord for Dot - Lua");
		local randomY = iInnerSouth2 + Map.Rand(iInnerHeight2, "Random Y coord for Dot - Lua");
		local i = (randomY - 1) * iWidth + randomX;
		plot_data[i] = true;
	end

	return plot_data
end
------------------------------------------------------------------------------
