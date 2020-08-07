function AssignStartingPlots:__CustomInit()
	-- This function included to provide a quick and easy override for changing 
	-- any initial settings. Add your customized version to the map script.

	self.AdjustTiles = AssignStartingPlots.AdjustTiles
	self.PlaceBonusResources = AssignStartingPlots.PlaceBonusResources
	
	-- Support custom map resource settings
	self.resDensity = 2
	self.resSize = 2
	self.legStart = false
	self.resBalance = false
	self bonusDensiy = 2
	self.luxuryDensity = 2
end
------------------------------------------------------------------------------
function AssignStartingPlots:GenerateRegions(args)
	print("Map Generation - Dividing the map in to Regions");
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
	self.resource_setting = args.resources or 2; -- UNUSED, use the below resource settings instead
	
	-- Custom map resource settings (Communitu_79a support)
	if args.resources < 4 and not args.comm then
		self.resDensity = args.resources;						-- Strategic Resource Density setting
		self.resSize = args.resources;							-- Strategic Resource Deposit Size setting
		self.bonusDensity = args.resources;						-- Bonus Resource Density setting
		self.luxuryDensity = args.resources;					-- Luxury Resource Density setting
	else if args.comm then
		self.resDensity = args.resources;						-- Strategic Resource Density setting
		self.resSize = args.size;								-- Strategic Resource Deposit Size setting
		self.bonusDensity = args.bonus;							-- Bonus Resource Density setting
		self.luxuryDensity = args.lux;							-- Luxury Resource Density setting
	end
	self.legStart = args.legend or (args.resources == 4);		-- Legendary Start setting
	self.resBalance = args.balance or (args.resources == 5);	-- Strategic Balance setting

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

		--[[ for AreaID, fert in ipairs(interim_table) do
			print("Interim Table ID " .. AreaID .. " has fertility of " .. fert);
		end
		print("* * * * * * * * * *"); ]]--

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
				--local thisLandmassCurrentFertility = land_area_fert[AreaID] / (1 + numberOfCivsPerArea[area_loop]);
				-- assume 80% of fertility is in relevant land areas
				local thisLandmassCurrentFertility = land_area_fert[AreaID] - iGlobalFertilityOfLands * 0.8 * numberOfCivsPerArea[area_loop] / self.iNumCivs;
				if thisLandmassCurrentFertility > bestRemainingFertility and numberOfCivsPerArea[area_loop] < math.max(self.iNumCivs - 2, 2) then
					bestRemainingArea = AreaID;
					bestRemainingFertility = thisLandmassCurrentFertility;
					bestAreaTableIndex = area_loop;
					--
					-- print("- Found new candidate landmass with Area ID#:", bestRemainingArea, " with fertility of ", bestRemainingFertility);
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
		
		--print("*** Number of Civs per Landmass - Table Readout ***");
		--PrintContentsOfTable(numberOfCivsPerArea)
		--print("--- End of Civs per Landmass readout ***"); print("-"); print("-");
				
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
function AssignStartingPlots:AddStrategicBalanceResources(region_number)
	-- This function adds the required Strategic Resources to start plots, for
	-- games that have selected to enable Strategic Resource Balance.
	local iW, iH = Map.GetGridSize();
	local start_point_data = self.startingPlots[region_number];
	local x = start_point_data[1];
	local y = start_point_data[2];
	local plot = Map.GetPlot(x, y);
	local plotIndex = y * iW + x + 1;
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local odd = self.firstRingYIsOdd;
	local even = self.firstRingYIsEven;
	local nextX, nextY, plot_adjustments;
	local iron_list, horse_list, oil_list = {}, {}, {};
	local iron_fallback, horse_fallback, oil_fallback = {}, {}, {};
	
	--print("- Adding Strategic Balance Resources for start location in Region#", region_number);
	
	for ripple_radius = 4, 6 do
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
					local plotType = plot:GetPlotType()
					local terrainType = plot:GetTerrainType()
					local featureType = plot:GetFeatureType()
					local plotIndex = realY * iW + realX + 1;
					-- Check this plot for resource placement eligibility.
					if plotType == PlotTypes.PLOT_HILLS and featureType ~= FeatureTypes.FEATURE_JUNGLE then
						if ripple_radius < 5 then
							table.insert(iron_list, plotIndex)
						else
							table.insert(iron_fallback, plotIndex)
						end
					elseif plotType == PlotTypes.PLOT_LAND then
						if featureType == FeatureTypes.NO_FEATURE then
							if terrainType == TerrainTypes.TERRAIN_TUNDRA or terrainType == TerrainTypes.TERRAIN_DESERT then
								if plot:IsFreshWater() then
									table.insert(horse_fallback, plotIndex)
								else
									table.insert(iron_fallback, plotIndex)
								end
							elseif terrainType == TerrainTypes.TERRAIN_PLAINS or terrainType == TerrainTypes.TERRAIN_GRASS then
								if ripple_radius < 5 then
									table.insert(horse_list, plotIndex)
								else
									table.insert(horse_fallback, plotIndex)
								end
								table.insert(iron_fallback, plotIndex)
							elseif terrainType == TerrainTypes.TERRAIN_SNOW then
								if ripple_radius < 5 then
									table.insert(iron_list, plotIndex)
								else
									table.insert(iron_fallback, plotIndex)
								end
							end
						elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then		
							table.insert(horse_list, plotIndex)
							table.insert(iron_fallback, plotIndex)
						elseif featureType == FeatureTypes.FEATURE_FOREST then		
							if ripple_radius < 5 then
								table.insert(iron_list, plotIndex)
							else
								table.insert(iron_fallback, plotIndex)
							end
						end
					end
					currentX, currentY = nextX, nextY;
				end
			end
		end
	end

	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = self:GetSmallStrategicResourceQuantityValues()
	local shuf_list;
	local placed_iron, placed_horse = false, false;

	if table.maxn(iron_list) > 0 then
		shuf_list = GetShuffledCopyOfTable(iron_list)
		iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.iron_ID, iron_amt, 1, 1, -1, 0, 0, shuf_list);
		if iNumLeftToPlace == 0 then
			placed_iron = true;
		end
	end
	if table.maxn(horse_list) > 0 then
		shuf_list = GetShuffledCopyOfTable(horse_list)
		iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.horse_ID, horse_amt, 1, 1, -1, 0, 0, shuf_list);
		if iNumLeftToPlace == 0 then
			placed_horse = true;
		end
	end
	if placed_iron == false and table.maxn(iron_fallback) > 0 then
		shuf_list = GetShuffledCopyOfTable(iron_fallback)
		iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.iron_ID, iron_amt, 1, 1, -1, 0, 0, shuf_list);
	end
	if placed_horse == false and table.maxn(horse_fallback) > 0 then
		shuf_list = GetShuffledCopyOfTable(horse_fallback)
		iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.horse_ID, horse_amt, 1, 1, -1, 0, 0, shuf_list);
	end
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
	
	-- Add mandatory Iron and Horse to every start if Strategic Balance option is enabled.
	if self.resBalance then
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
	if self.legStart then
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
			if ((innerPlaced < 2 and innerCanHaveBonus > 0) or (self.legStart and innerPlaced < 3 and innerCanHaveBonus > 0))
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

			elseif ((innerPlaced + outerPlaced < 5 and outerCanHaveBonus > 0) or (self.legStart and innerPlaced + outerPlaced < 4 and outerCanHaveBonus > 0))
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
	if Plot_GetFertilityInRange(plot, 3) < 16 then
		return false
	end
	return true
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
		self:GenerateLuxuryPlotListsAtCitySite(x, y, 1, true) -- Removes Feature Ice from coasts adjacent to the city state's new location
		self:PlaceResourceImpact(x, y, 5, 4) -- City State layer
		self:PlaceResourceImpact(x, y, 2, 3) -- Luxury layer
		self:PlaceResourceImpact(x, y, 3, 3) -- Bonus layer
		self:PlaceResourceImpact(x, y, 4, 3) -- Fish layer
		self:PlaceResourceImpact(x, y, 7, 3) -- Marble layer
		-- ####Vanilla Communitas change - Militaristic CS gets more potential strategic resources? Up to you to keep or not. I do like the flavour.
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
						if res_plot:GetResourceType(-1) == -1 then -- Placing this strategic resource in this plot.
							local res_addition = 0;
							if res_range[use_this_res_index] ~= -1 then
								res_addition = Map.Rand(res_range[use_this_res_index], "Resource Radius - Place Resource LUA");
							end
							local quantity = res_quantity[use_this_res_index]
							-- added by azum4roll: give some variance to strategic amounts
							local rand = PWRand()
							if (rand >= 0.8) then
								quantity = math.floor(quantity * 1.2 + 0.5)
							elseif (rand < 0.2) then
								quantity = math.floor(quantity * 0.8 + 0.5)
							end
							--
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
						if res_plot:GetResourceType(-1) == -1 then -- Placing this luxury resource in this plot.
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
						if res_plot:GetResourceType(-1) == -1 then -- Placing this bonus resource in this plot.
							local res_addition = 0;
							if res_range[use_this_res_index] ~= -1 then
								res_addition = Map.Rand(res_range[use_this_res_index], "Resource Radius - Place Resource LUA");
							end
							--print("ProcessResourceList table 3, Resource: " .. res_ID[use_this_res_index] .. ", Quantity: " .. res_quantity[use_this_res_index]);
							res_plot:SetResourceType(res_ID[use_this_res_index], res_quantity[use_this_res_index]);
							self:PlaceResourceImpact(x, y, impact_table_number, res_min[use_this_res_index] + res_addition);
							placed_this_res = true;
							self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] = self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] + 1;
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
						if res_plot:GetResourceType(-1) == -1 then
							lowest_impact = self.strategicData[plotIndex];
							best_plot = plotIndex;
						end
					end
				elseif impact_table_number == 2 then
					if lowest_impact > self.luxuryData[plotIndex] then
						local x = (plotIndex - 1) % iW;
						local y = (plotIndex - x - 1) / iW;
						local res_plot = Map.GetPlot(x, y)
						if res_plot:GetResourceType(-1) == -1 then
							lowest_impact = self.luxuryData[plotIndex];
							best_plot = plotIndex;
						end
					end
				elseif impact_table_number == 3 then
					if lowest_impact > self.bonusData[plotIndex] then
						local x = (plotIndex - 1) % iW;
						local y = (plotIndex - x - 1) / iW;
						local res_plot = Map.GetPlot(x, y)
						if res_plot:GetResourceType(-1) == -1 then
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
function AssignStartingPlots:GetWorldLuxuryTargetNumbers()
	-- This data was separated out to allow easy replacement in map scripts.
	--
	-- The first number is the target for total luxuries in the world, NOT
	-- counting the one-per-civ "second type" added at start locations.
	--
	-- The second number affects minimum number of random luxuries placed.
	-- I say "affects" because it is only one part of the formula.
	local worldsizes = {};
	
	if self.luxuryDensity == 4 then
		self.luxuryDensity = 1 + Map.Rand(3, "Luxury Resource Density");
	end
	
	if self.luxuryDensity == 1 then -- Sparse
		worldsizes = {
			[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {14, 3},
			[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {24, 4},
			[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {36, 4},
			[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {48, 5},
			[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {60, 5},
			[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {76, 6}
		}
	elseif self.luxuryDensity == 3 then -- Abundant
		worldsizes = {
			[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {24, 3},
			[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {40, 4},
			[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {60, 4},
			[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {80, 5},
			[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {100, 5},
			[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {128, 6}
		}
	else -- Standard
		worldsizes = {
			[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {18, 3},
			[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {30, 4},
			[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {45, 4},
			[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {60, 5},
			[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {75, 5},
			[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {95, 6}
		}
	end
	local world_size_data = worldsizes[Map.GetWorldSize()];
	return world_size_data
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceLuxuries()
	-- This function is dependent upon AssignLuxuryRoles() and PlaceCityStates() having been executed first.
	local iW, iH = Map.GetGridSize();
	-- Place Luxuries at civ start locations.
	for loop, reg_data in ipairs(self.regions_sorted_by_type) do
		local region_number = reg_data[1];
		local this_region_luxury = reg_data[2];
		local x = self.startingPlots[region_number][1];
		local y = self.startingPlots[region_number][2];
		print("-"); print("Attempting to place Luxury#", this_region_luxury, "at start plot", x, y, "in Region#", region_number);
		-- Determine number to place at the start location
		local iNumToPlace = 2;	-- MOD.Barathor: Updated -- original = 1 -- Most times, 2 of the initial type are placed at the start anyway, because of the old fertility checks below.  This will make it consistent.
		if self.legStart then -- Legendary Start
			iNumToPlace = 3;	-- MOD.Barathor: Updated -- original = 2
		end
		
		-- Obtain plot lists appropriate to this luxury type.
		local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, shuf_list;					-- MOD.Barathor: New -- added a quinary and senary list
		primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(this_region_luxury);	-- MOD.Barathor: New -- added a quinary and senary list

		-- First pass, checking only first two rings with a 50% ratio.
		luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false)
		shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
		local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumToPlace, 0.5, -1, 0, 0, shuf_list);
		if iNumLeftToPlace > 0 and secondary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, -1, 0, 0, shuf_list);
		end
		if iNumLeftToPlace > 0 and tertiary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, -1, 0, 0, shuf_list);
		end
		if iNumLeftToPlace > 0 and quaternary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, -1, 0, 0, shuf_list);
		end
		if iNumLeftToPlace > 0 and quinary > 0 then		-- MOD.Barathor: New -- added a quinary list
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, -1, 0, 0, shuf_list);
		end
		if iNumLeftToPlace > 0 and senary > 0 then		-- MOD.Barathor: New -- added a senary list
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, -1, 0, 0, shuf_list);
		end

		if iNumLeftToPlace > 0 then
			-- Second pass, checking three rings with a 100% ratio.
			luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 3, false)
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			if iNumLeftToPlace > 0 and secondary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and tertiary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and quaternary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and quinary > 0 then		-- MOD.Barathor: New -- added a quinary list
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and senary > 0 then		-- MOD.Barathor: New -- added a senary list
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			end
		end

		if iNumLeftToPlace > 0 then
			-- If we haven't been able to place all of this lux type at the start, it CAN be placed
			-- in the region somewhere. Subtract remainder from this region's compensation, so that the
			-- regional process, later, will attempt to place this remainder somewhere in the region.
			self.luxury_low_fert_compensation[this_region_luxury] = self.luxury_low_fert_compensation[this_region_luxury] - iNumLeftToPlace;
			self.region_low_fert_compensation[region_number] = self.region_low_fert_compensation[region_number] - iNumLeftToPlace;
		end
		if iNumLeftToPlace > 0 and self.iNumTypesRandom > 0 then
			-- We'll attempt to place one source of a Luxury type assigned to random distribution.
			local randoms_to_place = 1;
			for loop, random_res in ipairs(self.resourceIDs_assigned_to_random) do
				primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(random_res);	-- MOD.Barathor: New -- added a quinary and senary list
				if randoms_to_place > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and secondary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and tertiary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and quaternary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and quinary > 0 then	-- MOD.Barathor: New -- added a quinary list
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and senary > 0 then	-- MOD.Barathor: New -- added a senary list
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, -1, 0, 0, shuf_list);
				end
			end
		end
	end
	
	-- Place Luxuries at City States.
	-- Candidates include luxuries exclusive to CS, the lux assigned to this CS's region (if in a region), and the randoms.
	for city_state = 1, self.iNumCityStates do
		-- First check to see if this city state number received a valid start plot.
		if self.city_state_validity_table[city_state] == false then
			-- This one did not! It does not exist on the map nor have valid data, so we will ignore it.
		else
			-- OK, it's a valid city state. Process it.
			local region_number = self.city_state_region_assignments[city_state];
			local x = self.cityStatePlots[city_state][1];
			local y = self.cityStatePlots[city_state][2];
			local allowed_luxuries = self:GetListOfAllowableLuxuriesAtCitySite(x, y, 2)
			local lux_possible_for_cs = {}; -- Recorded with ID as key, weighting as data entry
			-- Identify Allowable Luxuries assigned to City States.
			-- If any CS-Only types are eligible, then all combined will have a weighting of 75%
			local cs_only_types = {};
			for loop, res_ID in ipairs(self.resourceIDs_assigned_to_cs) do
				if allowed_luxuries[res_ID] == true then
					table.insert(cs_only_types, res_ID);
				end
			end
			local iNumCSAllowed = table.maxn(cs_only_types);
			if iNumCSAllowed > 0 then
				for loop, res_ID in ipairs(cs_only_types) do
					lux_possible_for_cs[res_ID] = 75 / iNumCSAllowed;
				end
			end
			-- Identify Allowable Random Luxuries and the Regional Luxury if any.
			-- If any random types are eligible (plus the regional type if in a region) these combined carry a 25% weighting.
			if self.iNumTypesRandom > 0 or region_number > 0 then
				local random_types_allowed = {};
				for loop, res_ID in ipairs(self.resourceIDs_assigned_to_random) do
					if allowed_luxuries[res_ID] == true then
						table.insert(random_types_allowed, res_ID);
					end
				end
				local iNumRandAllowed = table.maxn(random_types_allowed);
				local iNumAllowed = iNumRandAllowed;
				if region_number > 0 then
					iNumAllowed = iNumAllowed + 1; -- Adding the region type in to the mix with the random types.
					local res_ID = self.region_luxury_assignment[region_number];
					if allowed_luxuries[res_ID] == true then
						lux_possible_for_cs[res_ID] = 25 / iNumAllowed;
					end
				end
				if iNumRandAllowed > 0 then
					for loop, res_ID in ipairs(random_types_allowed) do
						lux_possible_for_cs[res_ID] = 25 / iNumAllowed;
					end
				end
			end

			-- If there are no allowable luxury types at this city site, then this city state gets none.
			local iNumAvailableTypes = table.maxn(lux_possible_for_cs);
			if iNumAvailableTypes == 0 then
				print("City State #", city_state, "has poor land, ineligible to receive a Luxury resource.");
			else
				-- Calculate probability thresholds for each allowable luxury type.
				local res_threshold = {};
				local totalWeight, accumulatedWeight = 0, 0;
				for res_ID, this_weight in pairs(lux_possible_for_cs) do
					totalWeight = totalWeight + this_weight;
				end
				for res_ID, this_weight in pairs(lux_possible_for_cs) do
					local threshold = (this_weight + accumulatedWeight) * 10000 / totalWeight;
					res_threshold[res_ID] = threshold;
					accumulatedWeight = accumulatedWeight + this_weight;
				end
				-- Choose luxury type.
				local use_this_ID;
				local diceroll = Map.Rand(10000, "Choose resource type - Assign Luxury To City State - Lua");
				for res_ID, threshold in pairs(res_threshold) do
					if diceroll < threshold then -- Choose this resource type.
						use_this_ID = res_ID;
						break
					end
				end
				print("-"); print("-"); print("-Assigned Luxury Type", use_this_ID, "to City State#", city_state);
				-- Place luxury.
				local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, shuf_list;			-- MOD.Barathor: New -- added a quinary and senary list
				primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(use_this_ID);	-- MOD.Barathor: New -- added a quinary and senary list
				luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false)
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
				local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				if iNumLeftToPlace > 0 and secondary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and tertiary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quaternary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quinary > 0 then		-- MOD.Barathor: New -- added a quinary list
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and senary > 0 then		-- MOD.Barathor: New -- added a senary list
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace == 0 then
					print("-"); print("Placed Luxury ID#", use_this_ID, "at City State#", city_state, "in Region#", region_number, "located at Plot", x, y);
				end
			end
		end
	end
		
	-- Place Regional Luxuries
	for region_number, res_ID in ipairs(self.region_luxury_assignment) do
		print("-"); print("- - -"); print("Attempting to place regional luxury #", res_ID, "in Region#", region_number);
		local iNumAlreadyPlaced = self.amounts_of_resources_placed[res_ID + 1];
		local assignment_split = self.luxury_assignment_count[res_ID];
		local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, shuf_list, iNumLeftToPlace;		-- MOD.Barathor: New -- added a quinary and senary list
		primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(res_ID);					-- MOD.Barathor: New -- added a quinary and senary list
		luxury_plot_lists = self:GenerateLuxuryPlotListsInRegion(region_number)

		-- Calibrate number of luxuries per region to world size and number of civs
		-- present. The amount of lux per region should be at its highest when the 
		-- number of civs in the game is closest to "default" for that map size.
		local target_list = self:GetRegionLuxuryTargetNumbers()
		local targetNum = target_list[self.iNumCivs] 		-- MOD.Barathor: Updated -- Keep it simple and consistent.  Plus, fertility compensation above is disabled anyway.
		targetNum = targetNum - self.region_low_fert_compensation[region_number];
		-- Adjust target number according to Resource Setting.
		if self.luxuryDensity == 1 then
			targetNum = targetNum - 2;
		elseif self.luxuryDensity == 3 then
			targetNum = targetNum + 2;
		elseif self.luxuryDensity == 4 then
			targetNum = targetNum - 2 + Map.Rand(5, "Luxury Resource Density");
		end
		local iNumThisLuxToPlace = math.max(1, targetNum); -- Always place at least one.

		print("-"); print("Target number for Luxury#", res_ID, "with assignment split of", assignment_split, "is", targetNum);
		
		-- Place luxuries.
		shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
		iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumThisLuxToPlace, 0.25, 2, 1, 3, shuf_list);	-- MOD.Barathor: Updated -- Existing ratio = 0.3, min radius = 0, max radius = 3
		if iNumLeftToPlace > 0 and secondary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 1, 3, shuf_list);	-- MOD.Barathor: Updated -- Existing ratio = 0.3, min radius = 0, max radius = 3
		end
		if iNumLeftToPlace > 0 and tertiary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 1, 3, shuf_list);	-- MOD.Barathor: Updated -- Existing ratio = 0.4, min radius = 0, max radius = 2
		end
		if iNumLeftToPlace > 0 and quaternary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 1, 3, shuf_list);	-- MOD.Barathor: Updated -- Existing ratio = 0.5, min radius = 0, max radius = 2 
		end
		if iNumLeftToPlace > 0 and quinary > 0 then		-- MOD.Barathor: New -- added a quinary list
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 1, 3, shuf_list);	-- MOD.Barathor: Updated -- Existing ratio = 0.5, min radius = 0, max radius = 2 
		end
		if iNumLeftToPlace > 0 and senary > 0 then		-- MOD.Barathor: New -- added a senary list
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 1, 3, shuf_list);	-- MOD.Barathor: Updated -- Existing ratio = 0.5, min radius = 0, max radius = 2 
		end
		print("-"); print("-"); print("Number of LuxuryID", res_ID, "left to place in Region#", region_number, "is", iNumLeftToPlace);
		
		-- MOD.Barathor: New -- New Fallback loop since some luxuries frequently miss the target total during regionl placement (like silk, dyes, spices, etc.)
					-- This is because some luxuries have only a few indices and don't utilize all four, plus they have strict requirements, like foliage only and on flat land only
					-- When a luxury only uses two or three selective indices, it gets harder to place them all since the first default ratios above are only 0.3 (3 placements for every 10 eligible plots, rounded up)
		if iNumLeftToPlace > 0 then	
			-- Second pass, checking all with a 100% ratio to make sure the target total is reached for this region!
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, 2, 1, 3, shuf_list);
			if iNumLeftToPlace > 0 and secondary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, 2, 1, 3, shuf_list);
			end
			if iNumLeftToPlace > 0 and tertiary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, 2, 1, 3, shuf_list);
			end
			if iNumLeftToPlace > 0 and quaternary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, 2, 1, 3, shuf_list);
			end
			if iNumLeftToPlace > 0 and quinary > 0 then		-- MOD.Barathor: New -- added a quinary list
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, 2, 1, 3, shuf_list);
			end
			if iNumLeftToPlace > 0 and senary > 0 then		-- MOD.Barathor: New -- added a senary list
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, 2, 1, 3, shuf_list);
			end
			print("-"); print("Number of LuxuryID", res_ID, "not placed in Region#", region_number, "is", iNumLeftToPlace);
		end	
	end

	-- Place Random Luxuries
	if self.iNumTypesRandom > 0 then
		print("* *"); print("* iNumTypesRandom = ", self.iNumTypesRandom); print("* *");
		-- This table governs targets for total number of luxuries placed in the world, not
		-- including the "extra types" of Luxuries placed at start locations. These targets
		-- are approximate. An additional random factor is added in based on number of civs.
		-- Any difference between regional and city state luxuries placed, and the target, is
		-- made up for with the number of randomly placed luxuries that get distributed.
		
		local world_size_data = self:GetWorldLuxuryTargetNumbers()
		-- This modifies self.luxuryDensity if random, to a value between 1 and 3
		-- Which is okay, since regional luxuries have been placed
		
		local targetLuxForThisWorldSize = world_size_data[1];
		local loopTarget = world_size_data[2];
		local extraLux = Map.Rand(self.iNumCivs, "Luxury Resource Variance - Place Resources LUA");
		local iNumRandomLuxTarget = targetLuxForThisWorldSize + extraLux - self.totalLuxPlacedSoFar;
		print("* *"); print("* targetLuxForThisWorldSize = ", targetLuxForThisWorldSize); print("* *");	-- MOD.Barathor: Test
		print("* *"); print("* random to add to target = ", extraLux); print("* *");					-- MOD.Barathor: Test
		print("* *"); print("* totalLuxPlacedSoFar = ", self.totalLuxPlacedSoFar); print("* *");		-- MOD.Barathor: Test
		print("* *"); print("* iNumRandomLuxTarget = ", iNumRandomLuxTarget); print("* *");				-- MOD.Barathor: Test
		local iNumRandomLuxPlaced, iNumThisLuxToPlace = 0, 0;
		-- This table weights the amount of random luxuries to place, with first-selected getting heavier weighting.
		local random_lux_ratios_table = {
		{1},
		{0.55, 0.45},
		{0.40, 0.33, 0.27},
		{0.35, 0.25, 0.25, 0.15},
		{0.25, 0.25, 0.20, 0.15, 0.15},
		{0.20, 0.20, 0.20, 0.15, 0.15, 0.10},
		{0.20, 0.20, 0.15, 0.15, 0.10, 0.10, 0.10},
		{0.20, 0.15, 0.15, 0.10, 0.10, 0.10, 0.10, 0.10} };

		for loop, res_ID in ipairs(self.resourceIDs_assigned_to_random) do
			local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, current_list, iNumLeftToPlace;	-- MOD.Barathor: New -- added a quinary and senary list
			primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(res_ID);					-- MOD.Barathor: New -- added a quinary and senary list
			if self.iNumTypesRandom > 8 then
				iNumThisLuxToPlace = math.max(3, math.ceil(iNumRandomLuxTarget / 10));
			else
				local lux_minimum = math.max(3, loopTarget - loop);
				local lux_share_of_remaining = math.ceil(iNumRandomLuxTarget * random_lux_ratios_table[self.iNumTypesRandom][loop]);
				iNumThisLuxToPlace = math.max(lux_minimum, lux_share_of_remaining);
			end
			-- Place this luxury type.
			current_list = self.global_luxury_plot_lists[primary];
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumThisLuxToPlace, 0.25, 2, 4, 6, current_list);
			if iNumLeftToPlace > 0 and secondary > 0 then
				current_list = self.global_luxury_plot_lists[secondary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 4, 6, current_list);
			end
			if iNumLeftToPlace > 0 and tertiary > 0 then
				current_list = self.global_luxury_plot_lists[tertiary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 4, 6, current_list);
			end
			if iNumLeftToPlace > 0 and quaternary > 0 then
				current_list = self.global_luxury_plot_lists[quaternary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 4, 6, current_list);
			end
			if iNumLeftToPlace > 0 and quinary > 0 then		-- MOD.Barathor: New -- added a quinary list
				current_list = self.global_luxury_plot_lists[quinary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 4, 6, current_list);
			end
			if iNumLeftToPlace > 0 and senary > 0 then		-- MOD.Barathor: New -- added a senary list
				current_list = self.global_luxury_plot_lists[senary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 4, 6, current_list);
			end
			iNumRandomLuxPlaced = iNumRandomLuxPlaced + iNumThisLuxToPlace - iNumLeftToPlace;
			print("-"); 
			print("Random Luxury ID#:", res_ID);	-- MOD.Barathor: Test
			print("-"); print("Random Luxury Target Number:", iNumThisLuxToPlace);
			print("Random Luxury Target Placed:", iNumThisLuxToPlace - iNumLeftToPlace); print("-");
		end
		print("-"); print("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+");
		print("+ Random Luxuries Target Number:", iNumRandomLuxTarget);
		print("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+");
		print("+ Random Luxuries Number Placed:", iNumRandomLuxPlaced);
		print("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+"); print("-");
	end

	-- For Resource settings other than Sparse, add a second luxury type at start locations.
	-- This second type will be selected from Random types if possible, CS types if necessary, and other regions' types as a final fallback.
	-- Marble is included in the types possible to be placed.
	if self.luxuryDensity ~= 1 then
		for region_number = 1, self.iNumCivs do
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];
			local use_this_ID;
			local candidate_types, iNumTypesAllowed = {}, 0;
			local allowed_luxuries = self:GetListOfAllowableLuxuriesAtCitySite(x, y, 2)
			print("-"); print("--- Eligible Types List for Second Luxury in Region#", region_number, "---");
			-- See if any Random types are eligible.
			for loop, res_ID in ipairs(self.resourceIDs_assigned_to_random) do
				if allowed_luxuries[res_ID] == true then
					print("- Found eligible luxury type:", res_ID);
					iNumTypesAllowed = iNumTypesAllowed + 1;
					table.insert(candidate_types, res_ID);
				end
			end
			-- Check to see if any Special Case luxuries are eligible.
			for loop, res_ID in ipairs(self.resourceIDs_assigned_to_special_case) do
				if allowed_luxuries[res_ID] == true then
					print("- Found eligible luxury type:", res_ID);
					iNumTypesAllowed = iNumTypesAllowed + 1;
					table.insert(candidate_types, res_ID);
				end
			end
		
			if iNumTypesAllowed > 0 then
				local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
				use_this_ID = candidate_types[diceroll];
			else
				-- See if any City State types are eligible.
				for loop, res_ID in ipairs(self.resourceIDs_assigned_to_cs) do
					if allowed_luxuries[res_ID] == true then
						print("- Found eligible luxury type:", res_ID);
						iNumTypesAllowed = iNumTypesAllowed + 1;
						table.insert(candidate_types, res_ID);
					end
				end
				if iNumTypesAllowed > 0 then
					local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
					use_this_ID = candidate_types[diceroll];
				else
					-- See if anybody else's regional type is eligible.
					local region_lux_ID = self.region_luxury_assignment[region_number];
					for loop, res_ID in ipairs(self.resourceIDs_assigned_to_regions) do
						if res_ID ~= region_lux_ID then
							if allowed_luxuries[res_ID] == true then
								print("- Found eligible luxury type:", res_ID);
								iNumTypesAllowed = iNumTypesAllowed + 1;
								table.insert(candidate_types, res_ID);
							end
						end
					end
					if iNumTypesAllowed > 0 then
						local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
						use_this_ID = candidate_types[diceroll];
					else
						print("-"); print("Failed to place second Luxury type at start in Region#", region_number, "-- no eligible types!"); print("-");
					end
				end
			end
			print("--- End of Eligible Types list for Second Luxury in Region#", region_number, "---");
			if use_this_ID ~= nil then -- Place this luxury type at this start.
				local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, shuf_list;			-- MOD.Barathor: New -- added a quinary and senary list
				primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(use_this_ID);	-- MOD.Barathor: New -- added a quinary and senary list
				luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false)
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
				local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				if iNumLeftToPlace > 0 and secondary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and tertiary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quaternary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quinary > 0 then		-- MOD.Barathor: New -- added a quinary list
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and senary > 0 then		-- MOD.Barathor: New -- added a senary list
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace == 0 then
					print("-"); print("Placed Second Luxury type of ID#", use_this_ID, "for start located at Plot", x, y, " in Region#", region_number);
				end
			end
		end
	end

	-- Handle Special Case Luxuries
	if self.iNumTypesSpecialCase > 0 then
		-- Add a special case function for each luxury to be handled as a special case.
		self:PlaceMarble()
	end

	self.realtotalLuxPlacedSoFar = self.totalLuxPlacedSoFar		-- MOD.Barathor: New -- save the real total of luxuries before it gets corrupted with non-luxury additions which use the luxury placement method
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceMarble()
	local marble_already_placed = self.amounts_of_resources_placed[self.marble_ID + 1];
	local marble_target = math.ceil(self.iNumCivs * 0.75);
	if self.luxuryDensity == 1 then
		marble_target = math.ceil(self.iNumCivs * 0.5);
	elseif self.luxuryDensity == 3 then
		marble_target = math.ceil(self.iNumCivs * 1.0);
	end
	local iNumMarbleToPlace = math.max(2, marble_target - marble_already_placed);
	local iW, iH = Map.GetGridSize();
	local iNumLeftToPlace = iNumMarbleToPlace;
	local iNumPlots = table.maxn(self.marble_list);
	if iNumPlots < 1 then
		print("No eligible plots available to place Marble!");
		return
	end
	-- Main loop
	for place_resource = 1, iNumMarbleToPlace do
		for loop, plotIndex in ipairs(self.marble_list) do
			if self.marbleData[plotIndex] == 0 and self.luxuryData[plotIndex] == 0 then
				local x = (plotIndex - 1) % iW;
				local y = (plotIndex - x - 1) / iW;
				local res_plot = Map.GetPlot(x, y)
				if res_plot:GetResourceType(-1) == -1 then -- Placing this resource in this plot.
					res_plot:SetResourceType(self.marble_ID, 1);
					self.amounts_of_resources_placed[self.marble_ID + 1] = self.amounts_of_resources_placed[self.marble_ID + 1] + 1;
					--print("-"); print("Placed Marble randomly at Plot", x, y);			-- MOD.Barathor: Test
					self.totalLuxPlacedSoFar = self.totalLuxPlacedSoFar + 1;
					--print("Total luxuries placed so far: ", self.totalLuxPlacedSoFar);	-- MOD.Barathor: Test
					iNumLeftToPlace = iNumLeftToPlace - 1;
					--print("Still need to place", iNumLeftToPlace, "more units of Marble.");
					self:PlaceResourceImpact(x, y, 2, 1)
					self:PlaceResourceImpact(x, y, 7, 9)	-- MOD.Barathor: Updated -- Increased ripple radius from 6 to 9 to spread out distribution a little better.
					break
				end
			end
		end
	end
	if iNumLeftToPlace > 0 then
		print("Failed to place", iNumLeftToPlace, "units of Marble.");
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceSmallQuantitiesOfStrategics(frequency, plot_list)
	-- This function distributes small quantities of strategic resources.
	if plot_list == nil then
		print("No strategics were placed! -SmallQuantities");
		return
	end
	local iW, iH = Map.GetGridSize();
	local iNumTotalPlots = table.maxn(plot_list);
	local iNumToPlace = math.ceil(iNumTotalPlots / frequency);

	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = self:GetSmallStrategicResourceQuantityValues()
	
	-- Main loop
	local current_index = 1;
	for place_resource = 1, iNumToPlace do
		local placed_this_res = false;
		if current_index <= iNumTotalPlots then
			for index_to_check = current_index, iNumTotalPlots do
				if placed_this_res == true then
					break
				else
					current_index = current_index + 1;
				end
				local plotIndex = plot_list[index_to_check];
				if self.strategicData[plotIndex] == 0 then
					local x = (plotIndex - 1) % iW;
					local y = (plotIndex - x - 1) / iW;
					local res_plot = Map.GetPlot(x, y)
					if res_plot:GetResourceType(-1) == -1 then
						-- Placing a small strategic resource here. Need to determine what type to place.
						local selected_ID = -1;
						local selected_quantity = 2;
						local plotType = res_plot:GetPlotType()
						local terrainType = res_plot:GetTerrainType()
						local featureType = res_plot:GetFeatureType()
						if featureType == FeatureTypes.FEATURE_MARSH or featureType == FeatureTypes.FEATURE_JUNGLE then
							local diceroll = Map.Rand(5, "Resource selection - Place Small Quantities LUA");
							if diceroll < 1 or plotType == PlotTypes.PLOT_HILLS then
								selected_ID = self.uranium_ID;
								selected_quantity = uran_amt;
							else
								selected_ID = self.oil_ID;
								selected_quantity = oil_amt;
							end
						elseif featureType == FeatureTypes.FEATURE_FOREST then
							local diceroll = Map.Rand(5, "Resource selection - Place Small Quantities LUA");
							if diceroll < 1 then
								selected_ID = self.uranium_ID;
								selected_quantity = uran_amt;
							else
								selected_ID = self.iron_ID;
								selected_quantity = iron_amt;
							end
						elseif featureType == FeatureTypes.NO_FEATURE then
							if plotType == PlotTypes.PLOT_HILLS then
								if terrainType == TerrainTypes.TERRAIN_GRASS then
									local diceroll = Map.Rand(10, "Resource selection - Place Small Quantities LUA");
									if diceroll < 1 then
										selected_ID = self.uranium_ID;
										selected_quantity = uran_amt;
									elseif diceroll < 7 then
										selected_ID = self.iron_ID;
										selected_quantity = iron_amt;
									else
										selected_ID = self.coal_ID;
										selected_quantity = coal_amt;
									end
								elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
									local diceroll = Map.Rand(10, "Resource selection - Place Small Quantities LUA");
									if diceroll < 1 then
										selected_ID = self.uranium_ID;
										selected_quantity = uran_amt;
									elseif diceroll < 5 then
										selected_ID = self.iron_ID;
										selected_quantity = iron_amt;
									elseif diceroll < 8 then
										selected_ID = self.coal_ID;
										selected_quantity = coal_amt;
									else
										selected_ID = self.aluminum_ID;
										selected_quantity = alum_amt;
									end
								else
									local diceroll = Map.Rand(10, "Resource selection - Place Small Quantities LUA");
									if diceroll < 1 then
										selected_ID = self.uranium_ID;
										selected_quantity = uran_amt;
									elseif diceroll < 7 then
										selected_ID = self.iron_ID;
										selected_quantity = iron_amt;
									else
										selected_ID = self.aluminum_ID;
										selected_quantity = alum_amt;
									end
								end
							elseif terrainType == TerrainTypes.TERRAIN_GRASS then
								if res_plot:IsFreshWater() then
									selected_ID = self.horse_ID;
									selected_quantity = horse_amt;
								else
									local diceroll = Map.Rand(10, "Resource selection - Place Small Quantities LUA");
									if diceroll < 1 then
										selected_ID = self.uranium_ID;
										selected_quantity = uran_amt;
									elseif diceroll < 3 then
										selected_ID = self.iron_ID;
										selected_quantity = iron_amt;
									elseif diceroll < 6 then
										selected_ID = self.coal_ID;
										selected_quantity = coal_amt;
									else
										selected_ID = self.horse_ID;
										selected_quantity = horse_amt;
									end
								end
							elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
								local diceroll = Map.Rand(10, "Resource selection - Place Small Quantities LUA");
								if diceroll < 1 then
									selected_ID = self.uranium_ID;
									selected_quantity = uran_amt;
								elseif diceroll < 3 then
									selected_ID = self.iron_ID;
									selected_quantity = iron_amt;
								elseif diceroll < 5 then
									selected_ID = self.coal_ID;
									selected_quantity = coal_amt;
								elseif diceroll < 8 then
									selected_ID = self.horse_ID;
									selected_quantity = horse_amt;
								else
									selected_ID = self.aluminum_ID;
									selected_quantity = alum_amt;
								end
							elseif terrainType == TerrainTypes.TERRAIN_DESERT then
								if res_plot:IsFreshWater() then
									selected_ID = self.horse_ID;
									selected_quantity = horse_amt;
								else
									local diceroll = Map.Rand(10, "Resource selection - Place Small Quantities LUA");
									if diceroll < 1 then
										selected_ID = self.uranium_ID;
										selected_quantity = uran_amt;
									elseif diceroll < 3 then
										selected_ID = self.iron_ID;
										selected_quantity = iron_amt;
									elseif diceroll < 6 then
										selected_ID = self.oil_ID;
										selected_quantity = oil_amt;
									else
										selected_ID = self.aluminum_ID;
										selected_quantity = alum_amt;
									end
								end
							elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
								local diceroll = Map.Rand(10, "Resource selection - Place Small Quantities LUA");
								if diceroll < 1 then
									selected_ID = self.uranium_ID;
									selected_quantity = uran_amt;
								elseif diceroll < 4 then
									selected_ID = self.iron_ID;
									selected_quantity = iron_amt;
								elseif diceroll < 7 then
									selected_ID = self.coal_ID;
									selected_quantity = coal_amt;
								elseif diceroll < 9 then
									selected_ID = self.horse_ID;
									selected_quantity = horse_amt;
								else
									selected_ID = self.oil_ID;
									selected_quantity = oil_amt;
								end
							else
								local diceroll = Map.Rand(10, "Resource selection - Place Small Quantities LUA");
								if diceroll < 3 then
									selected_ID = self.uranium_ID;
									selected_quantity = uran_amt;
								elseif diceroll < 6 then
									selected_ID = self.iron_ID;
									selected_quantity = iron_amt;
								else
									selected_ID = self.oil_ID;
									selected_quantity = oil_amt;
								end
							end
						end
						-- Now place the resource, then impact the strategic data layer.
						if selected_ID ~= -1 then	
							local strat_radius = Map.Rand(4, "Resource Radius - Place Small Quantities LUA");
							if strat_radius > 2 then
								strat_radius = 1;
							end
							res_plot:SetResourceType(selected_ID, selected_quantity);
							self:PlaceResourceImpact(x, y, 1, strat_radius);
							placed_this_res = true;
							self.amounts_of_resources_placed[selected_ID + 1] = self.amounts_of_resources_placed[selected_ID + 1] + selected_quantity;
						end
					end
				end
			end
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:AddModernMinorStrategicsToCityStates()
	-- This function added Spring 2011. Purpose is to add a small strategic to most city states.
	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = self:GetSmallStrategicResourceQuantityValues()
	for city_state = 1, self.iNumCityStates do
		-- First check to see if this city state number received a valid start plot.
		if self.city_state_validity_table[city_state] == false then
			-- This one did not! It does not exist on the map nor have valid data, so we will ignore it.
		else
			-- OK, it's a valid city state. Process it.
			local x = self.cityStatePlots[city_state][1];
			local y = self.cityStatePlots[city_state][2];
			-- Choose strategic type. Options are: 0 None, 1 Coal, 2 Oil, 3 Aluminum
			local diceroll = Map.Rand(5, "Choose resource type - CS Strategic LUA");
			if diceroll > 1 then
				-- This city state selected for minor strategic resource placement.
				local use_this_ID, res_amt, luxury_plot_lists, shuf_list;
				local primary, secondary, tertiary, quaternary, quinternary, sexternary = 0, 0, 0, 0, 0, 0;
				if diceroll == 2 then
					use_this_ID = self.coal_ID;
					res_amt = coal_amt;
					primary, secondary, tertiary, quaternary, quinternary, sexternary = 24, 18, 12, 13, 19, 13;
				elseif diceroll == 3 then
					use_this_ID = self.oil_ID;
					res_amt = oil_amt;
					primary, secondary, tertiary, quaternary, quinternary, sexternary = 10, 2, 8, 17, 1, 1;
				elseif diceroll == 4 then
					use_this_ID = self.aluminum_ID;
					res_amt = alum_amt;
					primary, secondary, tertiary, quaternary, quinternary, sexternary = 25, 25, 36, 36, 18, 19;
				end
				--print("-"); print("-"); print("-Assigned Strategic Type", use_this_ID, "to City State#", city_state);
				-- Place strategic.
				luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 3, false)
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
				local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, res_amt, 1, 1, -1, 0, 0, shuf_list);
				if iNumLeftToPlace > 0 and secondary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, res_amt, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and tertiary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, res_amt, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quaternary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, res_amt, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quinternary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinternary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, res_amt, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and sexternary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[sexternary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, res_amt, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace == 0 then
					print("-"); print("Placed Minor Strategic ID#", use_this_ID, "at City State#", city_state, "located at Plot", x, y);
				end
			else
				print("-"); print("-"); print("-City State#", city_state, "gets no strategic resource assigned to it.");
			end
		end
	end
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceOilInTheSea()
	-- Places sources of Oil in Coastal waters, equal to half what's on the 
	-- land. If the map has too little ocean, then whatever will fit.
	--
	-- WARNING: This operation will render the Strategic Resource Impact Table useless for
	-- further operations, so should always be called last, even after minor placements.
	local sea_oil_amt_small = 2;
	local sea_oil_amt_major = 3;
	local iNumLandOilUnits = self.amounts_of_resources_placed[self.oil_ID + 1];
	local iNumToPlaceSmall = math.floor((iNumLandOilUnits / 4) / sea_oil_amt_small);
	local iNumToPlaceMajor = math.floor((iNumLandOilUnits / 4) / sea_oil_amt_major);

	--print("Adding Oil resources to the Sea.");
	--experimental; possibly janky
	self:PlaceSpecificNumberOfResources(self.oil_ID, sea_oil_amt_small, iNumToPlaceSmall, 0.2, 1, 2, 4, self.coast_list)
	self:PlaceSpecificNumberOfResources(self.oil_ID, sea_oil_amt_major, iNumToPlaceMajor, 0.2, 1, 4, 7, self.coast_list)
end
------------------------------------------------------------------------------
function AssignStartingPlots:AdjustTiles()
	-- ####Not Communitu_79a's version since that depends on map settings, so here's a generic one
	local iW, iH = Map.GetGridSize()
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			
			local plot = Map.GetPlot(x, y)
			local res_ID = plot:GetResourceType(-1)
			local featureType = plot:GetFeatureType()
			
			-- Mined/Quarried Resources
			if res_ID == self.marble_ID or 
			   res_ID == self.gold_ID or 
			   res_ID == self.silver_ID or 
			   res_ID == self.copper_ID or 
			   res_ID == self.gems_ID or 
			   res_ID == self.salt_ID or 
			   res_ID == self.lapis_ID or 
			   res_ID == self.jade_ID or 
			   res_ID == self.amber_ID then 
			   
				-- If a forest, jungle or flood plains is present, keep it.  Remove anything else.
				if (featureType ~= FeatureTypes.FEATURE_FOREST) and (featureType ~= FeatureTypes.FEATURE_JUNGLE) and (featureType ~= FeatureTypes.FEATURE_FLOOD_PLAINS) then
					plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1)
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
				   res_ID == self.deer_ID then
				
				if res_ID == self.fur_ID then
					-- Always want it flat.  The foxes fall into the hills.
					plot:SetPlotType(PlotTypes.PLOT_LAND, false, true)
				end
				
				-- MOD.Barathor: Gets the latitude of the tile to determine jungle eligibility.  Note: I like to use symmetrical latitudes, with an equator the width of two rows, which my map script utilizes.
				local lat = 0
				if (y >= (iH/2)) then
					lat = math.abs((iH/2) - y)/(iH/2)
				else
					lat = math.abs((iH/2) - (y + 1))/(iH/2)
				end
				local AvgJungleRange = 0
				--[[ MOD.Barathor: 
					 Get Rainfall setting to determine appropriate jungle range.
					 Assumed Formula: (jungle percent) / ((jungle percent) * (jungle factor)) = X; Jungle will form from 0.0 to X, decreasing as it gets further.
					 Example: Normal Rainfall: 65 / (65 * 5) = 0.2; Divide by 2 to get the mid range, which we'll use to blend it in.  ]]
				local rain = Map.GetCustomOption(2)	
				if rain == 1 then		
					-- Arid
					AvgJungleRange = 0.08
				elseif rain == 3 then	
					-- Wet
					AvgJungleRange = 0.25
				else					
					-- Normal or Random (Note: I'm currently not sure how to retrieve random, so we'll just use normal for now.)
					AvgJungleRange = 0.10
				end
				
				-- Always want it covered for most tree resources.
				if (featureType == FeatureTypes.FEATURE_MARSH) then
					if res_ID == self.sugar_ID or res_ID == self.spices_ID or res_ID == self.dye_ID then
						-- Keep it marsh for these resources.
					else
						-- Add some jungle or forest.
						if lat <= AvgJungleRange then
							if res_ID ~= self.deer_ID and res_ID ~= self.fur_ID then
								plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE, -1)
							else
								plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
							end
						else
							plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
						end	
					end
				else
					-- Add some jungle or forest.
					if lat <= AvgJungleRange then
						if res_ID ~= self.deer_ID and res_ID ~= self.fur_ID then
							plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE, -1)
						else
							plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
						end
					else
						plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
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
				   res_ID == self.cotton_ID then 
				
				if res_ID == self.ivory_ID then
					-- Always want it flat.  Other types are fine on hills.
					plot:SetPlotType(PlotTypes.PLOT_LAND, false, true)
				end				
				
				-- Don't remove flood plains if present for the few that are placed on it, only remove other features, like marsh or any trees.				
				if (featureType ~= FeatureTypes.FEATURE_FLOOD_PLAINS) then	
					plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1)
				end
				
				if res_ID == self.incense_ID then
					-- Because incense is very restricted, it was expanded to look for grass tiles as a final fallback.
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
------------------------------------------------------------------------------
function AssignStartingPlots:GetMajorStrategicResourceQuantityValues()
	-- This function determines quantity per tile for each strategic resource's major deposit size.
	-- Note: scripts that cannot place Oil in the sea need to increase amounts on land to compensate.
	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 4, 6, 4, 7, 7;
	-- Check the strategic deposit size setting.
	if self.resSize == 1 then -- Small
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 3, 3, 2, 3, 3;
	elseif self.resSize == 3 then -- Large
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 4, 6, 9, 6, 10, 10;
	end
	return uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt
end
------------------------------------------------------------------------------
function AssignStartingPlots:GetSmallStrategicResourceQuantityValues()
	-- This function determines quantity per tile for each strategic resource's small deposit size.
	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 1, 2, 2, 2, 2, 2;
	-- Check the strategic deposit size setting.
	if self.resSize == 1 then -- Small
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 1, 1, 1, 1, 1, 1;
	elseif self.resSize == 3 then -- Large
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 2, 2, 2, 2, 2;
	end
	return uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt
end
------------------------------------------------------------------------------
function AssignStartingPlots:PlaceStrategicAndBonusResources()
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
	self:ProcessResourceList(24 * resMultiplier, 1, self.grass_flat_no_feature, resources_to_place);
	self:ProcessResourceList(34 * resMultiplier, 1, self.plains_flat_no_feature, resources_to_place);
	
	resources_to_place = {
		{self.horse_ID, horse_amt, 100, 0, 2}
	};
	self:ProcessResourceList(20 * resMultiplier, 1, self.desert_wheat_list, resources_to_place);
	self:ProcessResourceList(70 * resMultiplier, 1, self.tundra_flat_no_feature, resources_to_place);
	
	resources_to_place = {
		{self.iron_ID, iron_amt, 100, 0, 2}
	};
	self:ProcessResourceList(75 * resMultiplier, 1, self.hills_open_list, resources_to_place);
	self:ProcessResourceList(180 * resMultiplier, 1, self.flat_open_no_tundra_no_desert, resources_to_place);
	self:ProcessResourceList(40 * resMultiplier, 1, self.desert_flat_no_feature, resources_to_place);
	
	resources_to_place = {
		{self.iron_ID, iron_amt, 100, 0, 1}
	};
	self:ProcessResourceList(120 * resMultiplier, 1, self.tundra_flat_no_feature, resources_to_place);
	
	resources_to_place = {
		{self.coal_ID, coal_amt, 100, 1, 2}
	};
	self:ProcessResourceList(65 * resMultiplier, 1, self.hills_open_no_tundra_no_desert, resources_to_place);
	self:ProcessResourceList(170 * resMultiplier, 1, self.grass_flat_no_feature, resources_to_place);
	self:ProcessResourceList(120 * resMultiplier, 1, self.plains_flat_no_feature, resources_to_place);
	
	resources_to_place = {
		{self.oil_ID, oil_amt, 100, 1, 3}
	};
	self:ProcessResourceList(40 * resMultiplier, 1, self.desert_flat_no_feature, resources_to_place);
	self:ProcessResourceList(75 * resMultiplier, 1, self.tundra_flat_no_feature, resources_to_place);
	
	resources_to_place = {
		{self.aluminum_ID, alum_amt, 100, 1, 3}
	};
	self:ProcessResourceList(42 * resMultiplier, 1, self.hills_open_no_grass, resources_to_place);
	self:ProcessResourceList(27 * resMultiplier, 1, self.flat_open_no_grass_no_plains, resources_to_place);
	self:ProcessResourceList(100 * resMultiplier, 1, self.plains_flat_no_feature, resources_to_place);
	
	resources_to_place = {
		{self.uranium_ID, uran_amt, 100, 1, 2}
	};
	self:ProcessResourceList(50 * resMultiplier, 1, self.hills_jungle_list, resources_to_place);
	self:ProcessResourceList(200 * resMultiplier, 1, self.hills_open_list, resources_to_place);
	self:ProcessResourceList(90 * resMultiplier, 1, self.tundra_flat_no_feature, resources_to_place);
	self:ProcessResourceList(150 * resMultiplier, 1, self.desert_flat_no_feature, resources_to_place);
	self:ProcessResourceList(300 * resMultiplier, 1, self.flat_open_no_tundra_no_desert, resources_to_place);
	
	resources_to_place = {
		{self.iron_ID, iron_amt, 90, 0, 2},
		{self.uranium_ID, uran_amt, 10, 1, 2}
	};
	self:ProcessResourceList(45 * resMultiplier, 1, self.hills_forest_list, resources_to_place);
	self:ProcessResourceList(50 * resMultiplier, 1, self.forest_flat_that_are_not_tundra, resources_to_place);
	self:ProcessResourceList(60 * resMultiplier, 1, self.tundra_flat_forest, resources_to_place);
	
	resources_to_place = {
		{self.oil_ID, oil_amt, 60, 1, 1},
		{self.uranium_ID, uran_amt, 40, 1, 1}
	};
	self:ProcessResourceList(15 * resMultiplier, 1, self.marsh_list, resources_to_place);
	self:ProcessResourceList(60 * resMultiplier, 1, self.jungle_flat_list, resources_to_place);
	
	resources_to_place = {
		{self.oil_ID, oil_amt, 40, 1, 2},
		{self.uranium_ID, uran_amt, 20, 1, 2},
		{self.iron_ID, iron_amt, 40, 1, 2}
	};
	self:ProcessResourceList(7 * resMultiplier, 1, self.snow_flat_list, resources_to_place);
	
	self:AddModernMinorStrategicsToCityStates();
	
	self:PlaceSmallQuantitiesOfStrategics(160 * resMultiplier / self.iNumCivs, self.land_list);
	
	self:PlaceOilInTheSea();

	-- Check for low or missing Strategic resources
	uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = self:GetSmallStrategicResourceQuantityValues();
	while self.amounts_of_resources_placed[self.iron_ID + 1] < 4 * self.iNumCivs do
		--print("Map has very low iron, adding another.");
		local resources_to_place = { {self.iron_ID, iron_amt, 20, 0, 2} };
		self:ProcessResourceList(99999, 1, self.desert_flat_no_feature, resources_to_place); -- 99999 means one per that many tiles: a single instance.
		self:ProcessResourceList(99999, 1, self.hills_forest_list, resources_to_place);
		self:ProcessResourceList(99999, 1, self.hills_open_list, resources_to_place);
		self:ProcessResourceList(99999, 1, self.forest_flat_that_are_not_tundra, resources_to_place);
		self:ProcessResourceList(99999, 1, self.tundra_flat_forest, resources_to_place);
	end
	while self.amounts_of_resources_placed[self.horse_ID + 1] < 4 * self.iNumCivs do
		--print("Map has very low horse, adding another.");
		local resources_to_place = { {self.horse_ID, horse_amt, 25, 1, 2} };
		self:ProcessResourceList(99999, 1, self.grass_flat_no_feature, resources_to_place);
		self:ProcessResourceList(99999, 1, self.plains_flat_no_feature, resources_to_place);
		self:ProcessResourceList(99999, 1, self.desert_wheat_list, resources_to_place);
		self:ProcessResourceList(99999, 1, self.tundra_flat_no_feature, resources_to_place);
	end
	while self.amounts_of_resources_placed[self.coal_ID + 1] < 4 * self.iNumCivs do
		--print("Map has very low coal, adding another.");
		local resources_to_place = { {self.coal_ID, coal_amt, 33, 0, 0} };
		self:ProcessResourceList(99999, 1, self.hills_open_no_tundra_no_desert, resources_to_place);
		self:ProcessResourceList(99999, 1, self.grass_flat_no_feature, resources_to_place);
		self:ProcessResourceList(99999, 1, self.plains_flat_no_feature, resources_to_place);
	end
	while self.amounts_of_resources_placed[self.oil_ID + 1] < 4 * self.iNumCivs do
		--print("Map has very low oil, adding another.");
		local resources_to_place = { {self.oil_ID, oil_amt, 33, 0, 0} };
		self:ProcessResourceList(99999, 1, self.desert_flat_no_feature, resources_to_place);
		self:ProcessResourceList(99999, 1, self.tundra_flat_no_feature, resources_to_place);
		self:ProcessResourceList(99999, 1, self.jungle_flat_list, resources_to_place);
	end
	while self.amounts_of_resources_placed[self.aluminum_ID + 1] < 5 * self.iNumCivs do
		--print("Map has very low aluminum, adding another.");
		local resources_to_place = { {self.aluminum_ID, alum_amt, 33, 0, 0} };
		self:ProcessResourceList(99999, 1, self.hills_open_no_grass, resources_to_place);
		self:ProcessResourceList(99999, 1, self.flat_open_no_grass_no_plains, resources_to_place);
		self:ProcessResourceList(99999, 1, self.plains_flat_no_feature, resources_to_place);
	end
	while self.amounts_of_resources_placed[self.uranium_ID + 1] < 3 * self.iNumCivs do
		--print("Map has very low uranium, adding another.");
		local resources_to_place = { {self.uranium_ID, uran_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.land_list, resources_to_place);
	end
	
	self:PlaceBonusResources();
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
	-- ####Communitu_79a has a completely different PlaceFish mechanic, but that's too map dependent
	self:PlaceFish(10 * bonus_multiplier, self.coast_list);
	self:PlaceSexyBonusAtCivStarts()
	self:AddExtraBonusesToHillsRegions()
	local resources_to_place = {}

	if IsEvenMoreResourcesActive() == true then
		resources_to_place = {
		{self.deer_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(12 * resMultiplier, 3, self.extra_deer_list, resources_to_place)
		-- 8
		resources_to_place = {
		{self.deer_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(16 * resMultiplier, 3, self.tundra_flat_no_feature, resources_to_place)
		-- 12
		
		resources_to_place = {
		{self.wheat_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(20 * resMultiplier, 3, self.desert_wheat_list, resources_to_place)
		-- 10
		resources_to_place = {
		{self.wheat_ID, 1, 100, 2, 3} };
		self:ProcessResourceList(44 * resMultiplier, 3, self.plains_flat_no_feature, resources_to_place)
		-- 27
		
		resources_to_place = {
		{self.banana_ID, 1, 100, 0, 1} };
		self:ProcessResourceList(30 * resMultiplier, 3, self.banana_list, resources_to_place)
		-- 14
		
		resources_to_place = {
		{self.banana_ID, 1, 100, 0, 1} };
		self:ProcessResourceList(40 * resMultiplier, 3, self.marsh_list, resources_to_place)
		-- none
		
		resources_to_place = {
		{self.cow_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(30 * resMultiplier, 3, self.grass_flat_no_feature, resources_to_place)
		-- 18
		
	-- CBP
		resources_to_place = {
		{self.bison_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(24 * resMultiplier, 3, self.flat_open_no_tundra_no_desert, resources_to_place)
	-- END

		resources_to_place = {
		{self.sheep_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(44 * resMultiplier, 3, self.hills_open_list, resources_to_place)
		-- 13

		resources_to_place = {
		{self.stone_ID, 1, 100, 1, 1} };
		self:ProcessResourceList(40 * resMultiplier, 3, self.dry_grass_flat_no_feature, resources_to_place)
		-- 20
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 1, 1} };
		self:ProcessResourceList(40 * resMultiplier, 3, self.dry_plains_flat_no_feature, resources_to_place)
		-- none
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(30 * resMultiplier, 3, self.tundra_flat_no_feature, resources_to_place)
		-- 15
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(16 * resMultiplier, 3, self.desert_flat_no_feature, resources_to_place)
		-- 19
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(36 * resMultiplier, 3, self.hills_open_list, resources_to_place)
		-- none
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(10 * resMultiplier, 3, self.snow_flat_list, resources_to_place)
		-- none
		
		resources_to_place = {
		{self.deer_ID, 1, 100, 3, 4} };
		self:ProcessResourceList(50 * resMultiplier, 3, self.forest_flat_that_are_not_tundra, resources_to_place)
		self:ProcessResourceList(50 * resMultiplier, 3, self.hills_forest_list, resources_to_place)
		-- 25

	-- Even More Resources for VP start
		resources_to_place = {
		{self.rice_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(24 * resMultiplier, 3, self.fresh_water_grass_flat_no_feature, resources_to_place)

		resources_to_place = {
		{self.maize_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(32 * resMultiplier, 3, self.plains_flat_no_feature, resources_to_place)

		resources_to_place = {
		{self.coconut_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(12 * resMultiplier, 3, self.coconut_list, resources_to_place)

		resources_to_place = {
		{self.hardwood_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(37 * resMultiplier, 3, self.hills_covered_list, resources_to_place)

		resources_to_place = {
		{self.hardwood_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(37 * resMultiplier, 3, self.flat_covered, resources_to_place)

		resources_to_place = {
		{self.hardwood_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(29 * resMultiplier, 3, self.tundra_flat_forest, resources_to_place)

		resources_to_place = {
		{self.lead_ID, 1, 100, 1, 3} };
		self:ProcessResourceList(40 * resMultiplier, 3, self.dry_grass_flat_no_feature, resources_to_place)

		resources_to_place = {
		{self.lead_ID, 1, 100, 2, 3} };
		self:ProcessResourceList(35 * resMultiplier, 3, self.hills_open_list, resources_to_place)

		resources_to_place = {
		{self.lead_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(35 * resMultiplier, 3, self.desert_flat_no_feature, resources_to_place)

		resources_to_place = {
		{self.pineapple_ID, 1, 100, 0, 3} };
		self:ProcessResourceList(29 * resMultiplier, 3, self.banana_list, resources_to_place)

		resources_to_place = {
		{self.potato_ID, 1, 100, 2, 3} };
		self:ProcessResourceList(40 * resMultiplier, 3, self.flat_open_no_tundra_no_desert, resources_to_place)

		resources_to_place = {
		{self.potato_ID, 1, 100, 0, 3} };
		self:ProcessResourceList(29 * resMultiplier, 3, self.hills_open_no_tundra_no_desert, resources_to_place)

		resources_to_place = {
		{self.rubber_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(43 * resMultiplier, 3, self.banana_list, resources_to_place)

		resources_to_place = {
		{self.sulfur_ID, 1, 100, 1, 3} };
		self:ProcessResourceList(29 * resMultiplier, 3, self.hills_open_list, resources_to_place)

		resources_to_place = {
		{self.sulfur_ID, 1, 100, 1, 3} };
		self:ProcessResourceList(37 * resMultiplier, 3, self.hills_covered_list, resources_to_place)

		resources_to_place = {
		{self.sulfur_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(21 * resMultiplier, 3, self.snow_flat_list, resources_to_place)

		resources_to_place = {
		{self.sulfur_ID, 1, 100, 1, 3} };
		self:ProcessResourceList(43 * resMultiplier, 3, self.flat_open, resources_to_place)

		resources_to_place = {
		{self.titanium_ID, 1, 100,0, 2} };
		self:ProcessResourceList(56 * resMultiplier, 3, self.flat_open, resources_to_place)

		resources_to_place = {
		{self.titanium_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(51 * resMultiplier, 3, self.hills_open_list, resources_to_place)

		resources_to_place = {
		{self.titanium_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(48 * resMultiplier, 3, self.desert_flat_no_feature, resources_to_place)

		resources_to_place = {
		{self.titanium_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(40 * resMultiplier, 3, self.tundra_flat_including_forests, resources_to_place)

		resources_to_place = {
		{self.titanium_ID, 1, 100, 0, 1} };
		self:ProcessResourceList(24 * resMultiplier, 3, self.snow_flat_list, resources_to_place)
	-- Even More Resources for VP end
	else
		resources_to_place = {
		{self.deer_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(6 * resMultiplier, 3, self.extra_deer_list, resources_to_place)
		-- 8
		resources_to_place = {
		{self.deer_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(8 * resMultiplier, 3, self.tundra_flat_no_feature, resources_to_place)
		-- 12
		
		resources_to_place = {
		{self.wheat_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(10 * resMultiplier, 3, self.desert_wheat_list, resources_to_place)
		-- 10
		resources_to_place = {
		{self.wheat_ID, 1, 100, 2, 3} };
		self:ProcessResourceList(22 * resMultiplier, 3, self.plains_flat_no_feature, resources_to_place)
		-- 27
		
		resources_to_place = {
		{self.banana_ID, 1, 100, 0, 1} };
		self:ProcessResourceList(15 * resMultiplier, 3, self.banana_list, resources_to_place)
		-- 14
		
		resources_to_place = {
		{self.banana_ID, 1, 100, 0, 1} };
		self:ProcessResourceList(20 * resMultiplier, 3, self.marsh_list, resources_to_place)
		-- none
		
		resources_to_place = {
		{self.cow_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(15 * resMultiplier, 3, self.grass_flat_no_feature, resources_to_place)
		-- 18
		
	-- CBP
		resources_to_place = {
		{self.bison_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(12 * resMultiplier, 3, self.flat_open_no_tundra_no_desert, resources_to_place)
	-- END

		resources_to_place = {
		{self.sheep_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(22 * resMultiplier, 3, self.hills_open_list, resources_to_place)
		-- 13

		resources_to_place = {
		{self.stone_ID, 1, 100, 1, 1} };
		self:ProcessResourceList(20 * resMultiplier, 3, self.dry_grass_flat_no_feature, resources_to_place)
		-- 20
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 1, 1} };
		self:ProcessResourceList(20 * resMultiplier, 3, self.dry_plains_flat_no_feature, resources_to_place)
		-- none
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(15 * resMultiplier, 3, self.tundra_flat_no_feature, resources_to_place)
		-- 15
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(8 * resMultiplier, 3, self.desert_flat_no_feature, resources_to_place)
		-- 19
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 1, 2} };
		self:ProcessResourceList(18 * resMultiplier, 3, self.hills_open_list, resources_to_place)
		-- none
		
		resources_to_place = {
		{self.stone_ID, 1, 100, 0, 2} };
		self:ProcessResourceList(5 * resMultiplier, 3, self.snow_flat_list, resources_to_place)
		-- none
		
		resources_to_place = {
		{self.deer_ID, 1, 100, 3, 4} };
		self:ProcessResourceList(25 * resMultiplier, 3, self.forest_flat_that_are_not_tundra, resources_to_place)
		self:ProcessResourceList(25 * resMultiplier, 3, self.hills_forest_list, resources_to_place)
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

	-- Place Strategic and Bonus resources.
	self:PlaceStrategicAndBonusResources()

	print("Map Generation - Normalize City State Locations");
	self:NormalizeCityStateLocations()
	
	-- Fix tile graphics
	self:AdjustTiles()
	
	-- Necessary to implement placement of Natural Wonders, and possibly other plot-type changes.
	-- This operation must be saved for last, as it invalidates all regional data by resetting Area IDs.
	Map.RecalculateAreas();

	-- Activate for debug only
	self:PrintFinalResourceTotalsToLog()
	--
end
------------------------------------------------------------------------------
-- HELPER FUNCTIONS
------------------------------------------------------------------------------
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
------------------------------------------------------
function Plot_GetFertilityInRange(plot, range, yieldID)
	local value = 0
	for nearPlot, distance in Plot_GetPlotsInCircle(plot, range) do
		value = value + Plot_GetFertility(nearPlot, yieldID) / math.max(1, distance)
	end
	return value
end
------------------------------------------------------
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
------------------------------------------------------
function IsBetween(lower, mid, upper)
	return ((lower <= mid) and (mid <= upper))
end
------------------------------------------------------
-- Get random multiplier normalized to 1
-- rand: optional random value
-- higher: optional boolean, determines >1 or <1
function getRandomMultiplier(variance, rand, higher)
	if higher == nil then
		higher = (1 == Map.Rand(2, "getRandomMultiplier"))
	end
	
	local multiplier = 1
	if rand == nil then
		multiplier = 1 + Map.Rand(10000, "getRandomMultiplier") / 10000 * variance
	else
		multiplier = 1 + rand * variance
	end
	
	if higher then
		return multiplier
	else
		return 1 / multiplier
	end
end