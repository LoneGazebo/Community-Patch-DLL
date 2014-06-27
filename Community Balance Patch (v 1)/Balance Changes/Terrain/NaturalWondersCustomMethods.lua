------------------------------------------------------------------------------
--	FILE:	  NaturalWondersCustomMethods.lua
--	AUTHOR:   Bob Thomas
--	PURPOSE:  Functions designed to support custom natural wonder placement.
------------------------------------------------------------------------------
--	Copyright (c) 2011 Firaxis Games, Inc. All rights reserved.
------------------------------------------------------------------------------

--[[ -------------------------------------------------------------------------
NOTE: This file is an essential component of the Start Plot System. I have
separated out the functions in this file to permit more convenient operation
for modders wishing to add new natural wonders or modify existing ones. If
you are supplying new custom methods, you will not have to supply an updated
version of AssignStartingPlots with your mod; instead, you only have to supply
an update of this file along with your updated Civ5Features.xml file.

CONTENTS OF THIS FILE:

* NWCustomEligibility(x, y, method_number)
* NWCustomPlacement(x, y, row_number, method_number)
------------------------------------------------------------------------- ]]--

include("MapmakerUtilities");
--include("ModTools.lua")

------------------------------------------------------------------------------
function NWCustomEligibility(x, y, method_number)
	if method_number == 1 then
		-- This method checks a candidate plot for eligibility to be the Great Barrier Reef.
		local iW, iH = Map.GetGridSize();
		local plot = Map.GetPlot(x, y);
		local plotIndex = y * iW + x + 1;
		-- We don't care about the center plot for this wonder. It can be forced. It's the surrounding plots that matter.
		-- This is also the only natural wonder type with a footprint larger than seven tiles.
		-- So first we'll check the extra tiles, make sure they are there, are ocean water, and have no Ice.
		local extra_direction_types = {
			DirectionTypes.DIRECTION_EAST,
			DirectionTypes.DIRECTION_SOUTHEAST,
			DirectionTypes.DIRECTION_SOUTHWEST};
		local SEPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_SOUTHEAST)
		local southeastX = SEPlot:GetX();
		local southeastY = SEPlot:GetY();
		for loop, direction in ipairs(extra_direction_types) do -- The three plots extending another plot past the SE plot.
			local adjPlot = Map.PlotDirection(southeastX, southeastY, direction)
			if adjPlot == nil then
				return false
			end
			if adjPlot:IsWater() == false or adjPlot:IsLake() == true then
				return false
			end
		end
		-- Now check the rest of the adjacent plots.
		local direction_types = { -- Not checking to southeast.
			DirectionTypes.DIRECTION_NORTHEAST,
			DirectionTypes.DIRECTION_EAST,
			DirectionTypes.DIRECTION_SOUTHWEST,
			DirectionTypes.DIRECTION_WEST,
			DirectionTypes.DIRECTION_NORTHWEST};
		for loop, direction in ipairs(direction_types) do
			local adjPlot = Map.PlotDirection(x, y, direction)
			if adjPlot:IsWater() == false then
				return false
			end
		end
		
		--
		local areaWeights = Plot_GetAreaWeights(plot, 2, 5)
		
		-- Avoid snow
		if areaWeights.TERRAIN_SNOW > 0 then
			return
		end
		
		-- Need land for potential city sites
		if (areaWeights.PLOT_LAND + areaWeights.PLOT_HILLS) <= 0.2 then
			return
		end
		--]]
		
		-- This site is in the water, with at least some of the water plots being coast, so it's good.
		return true
	
	elseif method_number == 2 then
		-- This method checks a candidate plot for eligibility to be Rock of Gibraltar.
		local plot = Map.GetPlot(x, y);
		local iW, iH = Map.GetGridSize();
		
		-- 1-2 adjacent plots must be land.
		local numLand = 0
		for adjPlot in Plot_GetPlotsInCircle(plot, 1, 1) do
			if adjPlot:GetPlotType() ~= PlotTypes.PLOT_OCEAN then
				numLand = numLand + 1 
			end
		end
		if numLand < 1 or numLand > 2 then
			return false
		end
		--]]
		
		-- Looking for a moderate amount of distant land.		
		local areaWeights = Plot_GetAreaWeights(plot, 2, 4)
		if not IsBetween(0.1, areaWeights.PLOT_LAND + areaWeights.PLOT_HILLS, 0.6) then
			return false
		end
		--]]
		
		-- This site is good.
		return true
		
	elseif method_number == 3 then
		-- This method checks a candidate plot for eligibility to be Krakatoa.
		local plot = Map.GetPlot(x, y);
		local iW, iH = Map.GetGridSize();
		
		-- Adjacent plots must be open sea
		local numLand = 0
		for adjPlot in Plot_GetPlotsInCircle(plot, 1, 1) do
			if adjPlot:GetPlotType() ~= PlotTypes.PLOT_OCEAN then
				numLand = numLand + 1 
			end
		end
		if numLand ~= 0 then
			return false
		end
		--]]
		
		-- Need land for potential city sites
		areaWeights = Plot_GetAreaWeights(plot, 2, 2)
		if (areaWeights.PLOT_LAND + areaWeights.PLOT_HILLS) <= 0.25 then
			return false
		end
		--]]
		
		return true
		
	-- These method numbers are not needed for the core game's natural wonders;
	-- however, this is where a modder could insert more custom methods, as needed.
	-- Any new methods added must be called from Natural_Wonder_Placement in Civ5Features.xml - Sirian, June 2011
	--
	--elseif method_number == 3 then
	--elseif method_number == 4 then
	--elseif method_number == 5 then

	else -- Unidentified Method Number
		return false
	end
end
------------------------------------------------------------------------------
function NWCustomPlacement(x, y, row_number, method_number)
	local iW, iH = Map.GetGridSize();
	if method_number == 1 then
		-- This method handles tile changes for the Great Barrier Reef.
		local plot = Map.GetPlot(x, y);
		if not plot:IsWater() then
			plot:SetPlotType(PlotTypes.PLOT_OCEAN, false, false);
		end
		if plot:GetTerrainType() ~= TerrainTypes.TERRAIN_COAST then
			plot:SetTerrainType(TerrainTypes.TERRAIN_COAST, false, false)
		end
		-- The Reef has a longer shape and demands unique handling. Process the extra plots.
		local extra_direction_types = {
			DirectionTypes.DIRECTION_EAST,
			DirectionTypes.DIRECTION_SOUTHEAST,
			DirectionTypes.DIRECTION_SOUTHWEST};
		local SEPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_SOUTHEAST)
		if not SEPlot:IsWater() then
			SEPlot:SetPlotType(PlotTypes.PLOT_OCEAN, false, false);
		end
		if SEPlot:GetTerrainType() ~= TerrainTypes.TERRAIN_COAST then
			SEPlot:SetTerrainType(TerrainTypes.TERRAIN_COAST, false, false)
		end
		if SEPlot:GetFeatureType() ~= FeatureTypes.NO_FEATURE then
			SEPlot:SetFeatureType(FeatureTypes.NO_FEATURE, -1)
		end
		local southeastX = SEPlot:GetX();
		local southeastY = SEPlot:GetY();
		for loop, direction in ipairs(extra_direction_types) do -- The three plots extending another plot past the SE plot.
			local adjPlot = Map.PlotDirection(southeastX, southeastY, direction)
			if adjPlot:GetTerrainType() ~= TerrainTypes.TERRAIN_COAST then
				adjPlot:SetTerrainType(TerrainTypes.TERRAIN_COAST, false, false)
			end
			local adjX = adjPlot:GetX();
			local adjY = adjPlot:GetY();
			local adjPlotIndex = adjY * iW + adjX + 1;
		end
		-- Now check the rest of the adjacent plots.
		local direction_types = { -- Not checking to southeast.
			DirectionTypes.DIRECTION_NORTHEAST,
			DirectionTypes.DIRECTION_EAST,
			DirectionTypes.DIRECTION_SOUTHWEST,
			DirectionTypes.DIRECTION_WEST,
			DirectionTypes.DIRECTION_NORTHWEST
			};
		for loop, direction in ipairs(direction_types) do
			local adjPlot = Map.PlotDirection(x, y, direction)
			if adjPlot:GetTerrainType() ~= TerrainTypes.TERRAIN_COAST then
				adjPlot:SetTerrainType(TerrainTypes.TERRAIN_COAST, false, false)
			end
		end
		-- Now place the Reef's second wonder plot. (The core method will place the main plot).
		local feature_type_to_place;
		for thisFeature in GameInfo.Features() do
			if thisFeature.Type == "FEATURE_REEF" then
				feature_type_to_place = thisFeature.ID;
				break
			end
		end
		SEPlot:SetFeatureType(feature_type_to_place);
	
	elseif method_number == 2 then
		-- This method handles tile changes for the Rock of Gibraltar.
		local plot = Map.GetPlot(x, y);
		plot:SetPlotType(PlotTypes.PLOT_LAND, false, false);
		plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, false)
		local direction_types = {
			DirectionTypes.DIRECTION_NORTHEAST,
			DirectionTypes.DIRECTION_EAST,
			DirectionTypes.DIRECTION_SOUTHEAST,
			DirectionTypes.DIRECTION_SOUTHWEST,
			DirectionTypes.DIRECTION_WEST,
			DirectionTypes.DIRECTION_NORTHWEST};
		for loop, direction in ipairs(direction_types) do
			local adjPlot = Map.PlotDirection(x, y, direction)
			if adjPlot:GetPlotType() == PlotTypes.PLOT_OCEAN then
				if adjPlot:GetTerrainType() ~= TerrainTypes.TERRAIN_COAST then
					adjPlot:SetTerrainType(TerrainTypes.TERRAIN_COAST, false, false)
				end
			else
				--[[
				if adjPlot:GetPlotType() ~= PlotTypes.PLOT_MOUNTAIN then
					adjPlot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, false)
					adjPlot:SetPlotType(PlotTypes.PLOT_MOUNTAIN, true, true);
				end
				--]]
			end
		end

	-- These method numbers are not needed for the core game's natural wonders;
	-- however, this is where a modder could insert more custom methods, as needed.
	-- Any new methods added must be called from Natural_Wonder_Placement in Civ5Features.xml - Sirian, June 2011
	--
	--elseif method_number == 3 then
	--elseif method_number == 4 then
	--elseif method_number == 5 then

	end
end
------------------------------------------------------------------------------

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

function Constrain(lower, mid, upper)
	return math.max(lower, math.min(mid, upper))
end
----------------------------------------------------------------
function Plot_GetPlotsInCircle(plot, minR, maxR)
	--[[ Plot_GetPlotsInCircle(plot, minR, [maxR]) usage examples:
	for nearPlot in Plot_GetPlotsInCircle(plot, 0, 4) do
		-- check all plots in radius 0-4
		nearPlot:GetX()
	end
	for nearPlot, distance in Plot_GetPlotsInCircle(plot, 3) do
		-- check all plots in radius 1-3
		if distance == 3 then
			print(nearPlot:GetX())
		end
	end
	]]
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

----------------------------------------------------------------

function Plot_GetAreaWeights(plot, minR, maxR)
	local weights = {TOTAL=0, SEA=0, NO_PLOT=0, NO_TERRAIN=0, NO_FEATURE=0}

	local plotTypeName		= {}-- -1="NO_PLOT"}
	local terrainTypeName	= {}-- -1="NO_TERRAIN"}
	local featureTypeName	= {}-- -1="NO_FEATURE"}
	
	for k, v in pairs(PlotTypes) do
		plotTypeName[v] = k
	end
	for itemInfo in GameInfo.Terrains() do
		terrainTypeName[itemInfo.ID] = itemInfo.Type
	end
	for itemInfo in GameInfo.Features() do
		featureTypeName[itemInfo.ID] = itemInfo.Type
	end
	
	for k, v in pairs(PlotTypes) do
		weights[k] = 0
	end
	for itemInfo in GameInfo.Terrains() do
		weights[itemInfo.Type] = 0
	end
	for itemInfo in GameInfo.Features() do
		weights[itemInfo.Type] = 0
	end
	
	for adjPlot in Plot_GetPlotsInCircle(plot, minR, maxR) do
		local distance		 = Map.PlotDistance(adjPlot:GetX(), adjPlot:GetY(), plot:GetX(), plot:GetY())
		local adjWeight		 = (distance == 0) and 6 or (1/distance)
		local plotType		 = plotTypeName[adjPlot:GetPlotType()]
		local terrainType	 = terrainTypeName[adjPlot:GetTerrainType()]
		local featureType	 = featureTypeName[adjPlot:GetFeatureType()] or "NO_FEATURE"
		
		weights.TOTAL		 = weights.TOTAL		+ adjWeight 
		weights[plotType]	 = weights[plotType]	+ adjWeight
		weights[terrainType] = weights[terrainType]	+ adjWeight
		weights[featureType] = weights[featureType]	+ adjWeight
				
		if plotType == "PLOT_OCEAN" then
			if not adjPlot:IsLake() and featureType ~= "FEATURE_ICE" then
				weights.SEA = weights.SEA + adjWeight
			end
		end
	end
	
	if weights.TOTAL == 0 then
		log:Fatal("plot:GetAreaWeights Total=0! x=%s y=%s", x, y)
	end
	for k, v in pairs(weights) do
		if k ~= "TOTAL" then
			weights[k] = weights[k] / weights.TOTAL
		end
	end
	
	return weights
end