print("This is the modded InfoTooltipInclude from CBP- CSD")
-------------------------------------------------
-- Help text for Info Objects (Units, Buildings, etc.)
-------------------------------------------------


-- UNIT
function GetHelpTextForUnit(iUnitID, bIncludeRequirementsInfo, pCity)
	local pUnitInfo = GameInfo.Units[iUnitID];
	
	local pActivePlayer = Players[Game.GetActivePlayer()];
	local pActiveTeam = Teams[Game.GetActiveTeam()];

	local strHelpText = "";
	
	-- Name
	strHelpText = strHelpText .. Locale.ToUpper(Locale.ConvertTextKey( pUnitInfo.Description ));
	if (pCity ~= nil) then
		if(pCity:GetUnitInvestment(iUnitID) > 0) then
		strHelpText = strHelpText .. Locale.ConvertTextKey("TXT_KEY_INVESTED");
		end
	end
			
	-- Cost
	strHelpText = strHelpText .. "[NEWLINE]----------------[NEWLINE]";
	
	-- Skip cost if it's 0
	if(pUnitInfo.Cost > 0) then
		local iCost = pActivePlayer:GetUnitProductionNeeded(iUnitID);
		if (pCity ~= nil) then
			if(pCity:GetUnitInvestment(iUnitID) > 0) then
				iCost = pCity:GetUnitInvestment(iUnitID);
			end
		end
		strHelpText = strHelpText .. Locale.ConvertTextKey("TXT_KEY_PRODUCTION_COST", iCost);
	end			
	
	-- Moves
	if pUnitInfo.Domain ~= "DOMAIN_AIR" then
		strHelpText = strHelpText .. "[NEWLINE]";
		strHelpText = strHelpText .. Locale.ConvertTextKey("TXT_KEY_PRODUCTION_MOVEMENT", pUnitInfo.Moves);
	end
	
	-- Range
	local iRange = pUnitInfo.Range;
	if (iRange ~= 0) then
		strHelpText = strHelpText .. "[NEWLINE]";
		strHelpText = strHelpText .. Locale.ConvertTextKey("TXT_KEY_PRODUCTION_RANGE", iRange);
	end
	
	-- Ranged Strength
	local iRangedStrength = pUnitInfo.RangedCombat;
	if (iRangedStrength ~= 0) then
		strHelpText = strHelpText .. "[NEWLINE]";
		strHelpText = strHelpText .. Locale.ConvertTextKey("TXT_KEY_PRODUCTION_RANGED_STRENGTH", iRangedStrength);
	end
	
	-- Strength
	local iStrength = pUnitInfo.Combat;
	if (iStrength ~= 0) then
		strHelpText = strHelpText .. "[NEWLINE]";
		strHelpText = strHelpText .. Locale.ConvertTextKey("TXT_KEY_PRODUCTION_STRENGTH", iStrength);
	end

	-- Air Strength
	local iAirStrength = pUnitInfo.BaseLandAirDefense;
	if (iAirStrength ~= 0) then
		strHelpText = strHelpText .. "[NEWLINE]";
		strHelpText = strHelpText .. Locale.ConvertTextKey("TXT_KEY_PRODUCTION_AIR_STRENGTH", iAirStrength);
	end
	
	-- Resource Requirements
	local iNumResourcesNeededSoFar = 0;
	local iNumResourceNeeded;
	local iResourceID;
	for pResource in GameInfo.Resources() do
		iResourceID = pResource.ID;
		iNumResourceNeeded = Game.GetNumResourceRequiredForUnit(iUnitID, iResourceID);
		if (iNumResourceNeeded > 0) then
			-- First resource required
			if (iNumResourcesNeededSoFar == 0) then
				strHelpText = strHelpText .. "[NEWLINE]";
				strHelpText = strHelpText .. Locale.ConvertTextKey("TXT_KEY_PRODUCTION_RESOURCES_REQUIRED");
				strHelpText = strHelpText .. " " .. iNumResourceNeeded .. " " .. pResource.IconString .. " " .. Locale.ConvertTextKey(pResource.Description);
			else
				strHelpText = strHelpText .. ", " .. iNumResourceNeeded .. " " .. pResource.IconString .. " " .. Locale.ConvertTextKey(pResource.Description);
			end
			
			-- JON: Not using this for now, the formatting is better when everything is on the same line
			--iNumResourcesNeededSoFar = iNumResourcesNeededSoFar + 1;
		end
 	end

	if Game.IsCustomModOption("UNITS_RESOURCE_QUANTITY_TOTALS") then
		local iNumResourceTotalNeeded;
		for pResource in GameInfo.Resources() do
			iResourceID = pResource.ID;
			iNumResourceTotalNeeded = Game.GetNumResourceTotalRequiredForUnit(iUnitID, iResourceID);
			if (iNumResourceTotalNeeded > 0) then
				-- First resource required
				if (iNumResourcesNeededSoFar == 0) then
					strHelpText = strHelpText .. "[NEWLINE]";
					strHelpText = strHelpText .. Locale.ConvertTextKey("TXT_KEY_PRODUCTION_TOTAL_RESOURCES_REQUIRED");
					strHelpText = strHelpText .. " " .. iNumResourceTotalNeeded .. " " .. pResource.IconString .. " " .. Locale.ConvertTextKey(pResource.Description);
				else
					strHelpText = strHelpText .. ", " .. iNumResourceTotalNeeded .. " " .. pResource.IconString .. " " .. Locale.ConvertTextKey(pResource.Description);
				end
				
				-- JON: Not using this for now, the formatting is better when everything is on the same line
				--iNumResourcesNeededSoFar = iNumResourcesNeededSoFar + 1;
			end
		end
	end
	
	-- Pre-written Help text
	if (not pUnitInfo.Help) then
		print("Invalid unit help");
		print(strHelpText);
	else
		local strWrittenHelpText = Locale.ConvertTextKey( pUnitInfo.Help );
		if (strWrittenHelpText ~= nil and strWrittenHelpText ~= "") then
			-- Separator
			strHelpText = strHelpText .. "[NEWLINE]----------------[NEWLINE]";
			strHelpText = strHelpText .. strWrittenHelpText;
		end	
	end
	
	
	-- Requirements?
	if (bIncludeRequirementsInfo) then
		if (pUnitInfo.Requirements) then
			strHelpText = strHelpText .. Locale.ConvertTextKey( pUnitInfo.Requirements );
		end
	end
	
	return strHelpText;
	
end

-- BUILDING
function GetHelpTextForBuilding(iBuildingID, bExcludeName, bExcludeHeader, bNoMaintenance, pCity)
	local pBuildingInfo = GameInfo.Buildings[iBuildingID];

	local pActivePlayer = Players[Game.GetActivePlayer()];
	local pActiveTeam = Teams[Game.GetActiveTeam()];
	-- when viewing a (foreign) city, always show tooltips as they are for the city owner
	if (pCity ~= nil) then
		pActivePlayer = Players[pCity:GetOwner()];
		pActiveTeam = Teams[pActivePlayer:GetTeam()];
	end
	
	local buildingClass = GameInfo.Buildings[iBuildingID].BuildingClass;
	local buildingClassID = GameInfo.BuildingClasses[buildingClass].ID;
	
	local strHelpText = "";
	
	local lines = {};
	local bFirst = true;
	if (not bExcludeHeader) then
		
		if (not bExcludeName) then
			-- Name
			strHelpText = strHelpText .. Locale.ToUpper(Locale.ConvertTextKey( pBuildingInfo.Description ));
-- CBP
			if (pCity ~= nil) then
				if(pCity:GetBuildingInvestment(iBuildingID) > 0) then
				strHelpText = strHelpText .. Locale.ConvertTextKey("TXT_KEY_INVESTED");
				end
			end
-- END
			strHelpText = strHelpText .. "[NEWLINE]----------------[NEWLINE]";
		end
		
		-- Cost
		--Only show cost info if the cost is greater than 0.
		if(pBuildingInfo.Cost > 0) then
			local iCost = pActivePlayer:GetBuildingProductionNeeded(iBuildingID);
-- CBP
			if (pCity ~= nil) then
				if(pCity:GetBuildingInvestment(iBuildingID) > 0) then
					iCost = pCity:GetBuildingInvestment(iBuildingID);
				end
			end
-- END
			table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_COST", iCost));
			bFirst = false;
		end
		
		if(pBuildingInfo.UnlockedByLeague and Game.GetNumActiveLeagues() > 0 and not Game.IsOption("GAMEOPTION_NO_LEAGUES")) then
			local pLeague = Game.GetActiveLeague();
			if (pLeague ~= nil) then
				local iCostPerPlayer = pLeague:GetProjectBuildingCostPerPlayer(iBuildingID);
				local sCostPerPlayer = Locale.ConvertTextKey("TXT_KEY_PEDIA_COST_LABEL");
				sCostPerPlayer = sCostPerPlayer .. " " .. Locale.ConvertTextKey("TXT_KEY_LEAGUE_PROJECT_COST_PER_PLAYER", iCostPerPlayer);
				table.insert(lines, sCostPerPlayer);
				bFirst = false;
			end
		end
		
		-- Maintenance
		if (not bNoMaintenance) then
			local iMaintenance = pBuildingInfo.GoldMaintenance;
			if (iMaintenance ~= nil and iMaintenance ~= 0) then		
				table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_MAINTENANCE", iMaintenance));
				bFirst = false;
			end
		end
		
		-- CBP Num Social Policies
		if(pActivePlayer) then
			local iNumPolicies = pBuildingInfo.NumPoliciesNeeded;
			if(pCity ~= nil) then
				iNumPolicies = pCity:GetNumPoliciesNeeded(iBuildingID)
			end
			if(iNumPolicies > 0) then
				local iNumHave = pActivePlayer:GetNumPolicies(true);
				table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PEDIA_NUM_POLICY_NEEDED_LABEL", iNumPolicies, iNumHave));
				bFirst = false;
			end
		end

		--- National/Local Population
		if(pActivePlayer) then
			local iNumNationalPop = pActivePlayer:GetScalingNationalPopulationRequired(iBuildingID);
			if(iNumNationalPop > 0) then
				local iNumHave = pActivePlayer:GetTotalPopulation();
				table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PEDIA_NUM_POPULATION_NATIONAL_NEEDED_LABEL", iNumNationalPop, iNumHave));
				bFirst = false;
			end
		end

		local iNumLocalPop = pBuildingInfo.LocalPopRequired;
		if(iNumLocalPop > 0) then
			if (pCity) then
				local iNumHave = pCity:GetPopulation();
				table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PEDIA_NUM_POPULATION_LOCAL_NEEDED_LABEL", iNumLocalPop, iNumHave));
				bFirst = false;
			end
		end
	end
	
	if(bFirst == false) then
		table.insert(lines, "----------------");
	end
	
	-- Happiness (from all sources)
	local iHappinessTotal = 0;
	local iHappiness = pBuildingInfo.Happiness;
	if (iHappiness ~= nil) then
		iHappinessTotal = iHappinessTotal + iHappiness;
	end
	local iHappiness = pBuildingInfo.UnmoddedHappiness;
	if (iHappiness ~= nil) then
		iHappinessTotal = iHappinessTotal + iHappiness;
	end
	iHappinessTotal = iHappinessTotal + pActivePlayer:GetExtraBuildingHappinessFromPolicies(iBuildingID);
	if (pCity ~= nil) then
		iHappinessTotal = iHappinessTotal + pCity:GetReligionBuildingClassHappiness(buildingClassID) + pActivePlayer:GetPlayerBuildingClassHappiness(buildingClassID);
	end
	if (iHappinessTotal ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_HAPPINESS", iHappinessTotal));
	end

-- VP -- Global Average Modifiers
	local iBasicNeedsMedianModifierBuilding = Game.GetBasicNeedsMedianModifierBuilding(iBuildingID); 
	if (iBasicNeedsMedianModifierBuilding ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_BASIC_NEEDS_MEDIAN_MODIFIER", iBasicNeedsMedianModifierBuilding));
	end
	local iGoldMedianModifierBuilding = Game.GetGoldMedianModifierBuilding(iBuildingID); 
	if (iGoldMedianModifierBuilding ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_GOLD_MEDIAN_MODIFIER", iGoldMedianModifierBuilding));
	end
	local iScienceMedianModifierBuilding = Game.GetScienceMedianModifierBuilding(iBuildingID); 
	if (iScienceMedianModifierBuilding ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_SCIENCE_MEDIAN_MODIFIER", iScienceMedianModifierBuilding));
	end
	local iCultureMedianModifierBuilding = Game.GetCultureMedianModifierBuilding(iBuildingID); 
	if (iCultureMedianModifierBuilding ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_CULTURE_MEDIAN_MODIFIER", iCultureMedianModifierBuilding));
	end
	local iReligiousUnrestModifierBuilding = Game.GetReligiousUnrestModifierBuilding(iBuildingID); 
	if (iReligiousUnrestModifierBuilding ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_RELIGIOUS_UNREST_MODIFIER", iReligiousUnrestModifierBuilding));
	end

	local iBasicNeedsMedianModifierBuildingGlobal = Game.GetBasicNeedsMedianModifierBuildingGlobal(iBuildingID); 
	if (iBasicNeedsMedianModifierBuildingGlobal ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_BASIC_NEEDS_MEDIAN_MODIFIER_GLOBAL", iBasicNeedsMedianModifierBuildingGlobal));
	end
	local iGoldMedianModifierBuildingGlobal = Game.GetGoldMedianModifierBuildingGlobal(iBuildingID); 
	if (iGoldMedianModifierBuildingGlobal ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_GOLD_MEDIAN_MODIFIER_GLOBAL", iGoldMedianModifierBuildingGlobal));
	end
	local iScienceMedianModifierBuildingGlobal = Game.GetScienceMedianModifierBuildingGlobal(iBuildingID); 
	if (iScienceMedianModifierBuildingGlobal ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_SCIENCE_MEDIAN_MODIFIER_GLOBAL", iScienceMedianModifierBuildingGlobal));
	end
	local iCultureMedianModifierBuildingGlobal = Game.GetCultureMedianModifierBuildingGlobal(iBuildingID); 
	if (iCultureMedianModifierBuildingGlobal ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_CULTURE_MEDIAN_MODIFIER_GLOBAL", iCultureMedianModifierBuildingGlobal));
	end
	local iReligiousUnrestModifierBuildingGlobal = Game.GetReligiousUnrestModifierBuildingGlobal(iBuildingID); 
	if (iReligiousUnrestModifierBuildingGlobal ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_RELIGIOUS_UNREST_MODIFIER_GLOBAL", iReligiousUnrestModifierBuildingGlobal));
	end

	if (not OptionsManager.IsNoBasicHelp()) then	
		if (pCity ~= nil) then
			local iBasicNeedsMedianModifier = iBasicNeedsMedianModifierBuilding + iBasicNeedsMedianModifierBuildingGlobal;
			if (iBasicNeedsMedianModifier ~= 0) then
				local iNewMedian = pCity:GetTheoreticalNewBasicNeedsMedian(iBuildingID) / 100;
				local iOldMedian = pCity:GetBasicNeedsMedian() / 100;
				if (iNewMedian ~= 0 and iOldMedian ~= 0) then
					table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_BASIC_NEEDS_NEW_MEDIAN", iNewMedian, iOldMedian));
				end
			end
			local iGoldMedianModifier = iGoldMedianModifierBuilding + iGoldMedianModifierBuildingGlobal;
			if (iGoldMedianModifier ~= 0) then
				local iNewMedian = pCity:GetTheoreticalNewGoldMedian(iBuildingID) / 100;
				local iOldMedian = pCity:GetGoldMedian() / 100;
				if (iNewMedian ~= 0 and iOldMedian ~= 0) then
					table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_GOLD_NEW_MEDIAN", iNewMedian, iOldMedian));
				end
			end
			local iScienceMedianModifier = iScienceMedianModifierBuilding + iScienceMedianModifierBuildingGlobal;
			if (iScienceMedianModifier ~= 0) then
				local iNewMedian = pCity:GetTheoreticalNewScienceMedian(iBuildingID) / 100;
				local iOldMedian = pCity:GetScienceMedian() / 100;
				if (iNewMedian ~= 0 and iOldMedian ~= 0) then
					table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_SCIENCE_NEW_MEDIAN", iNewMedian, iOldMedian));
				end
			end
			local iCultureMedianModifier = iCultureMedianModifierBuilding + iCultureMedianModifierBuildingGlobal;
			if (iCultureMedianModifier ~= 0) then
				local iNewMedian = pCity:GetTheoreticalNewCultureMedian(iBuildingID) / 100;
				local iOldMedian = pCity:GetCultureMedian() / 100;
				if (iNewMedian ~= 0 and iOldMedian ~= 0) then
					table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_CULTURE_NEW_MEDIAN", iNewMedian, iOldMedian));
				end
			end
			local iReligiousUnrestModifier = iReligiousUnrestModifierBuilding + iReligiousUnrestModifierBuildingGlobal;
			if (iReligiousUnrestModifier ~= 0) then
				local iNewUnhappyPerPop = pCity:GetTheoreticalNewReligiousUnrestPerMinorityFollower(iBuildingID) / 100;
				local iOldUnhappyPerPop = pCity:GetReligiousUnrestPerMinorityFollower(iBuildingID) / 100;
				if (iNewUnhappyPerPop ~= 0 and iOldUnhappyPerPop ~= 0) then
					table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_RELIGIOUS_UNREST_NEW_THRESHOLD", iNewUnhappyPerPop, iOldUnhappyPerPop));
				end
			end
		end
	end

-- END
	
	
	-- Food
	local iFood = Game.GetBuildingYieldChange(iBuildingID, YieldTypes.YIELD_FOOD);
	iFood = iFood + pActivePlayer:GetBuildingTechEnhancedYields(iBuildingID, YieldTypes.YIELD_FOOD);
	if (pCity ~= nil) then
		iFood = iFood + pCity:GetReligionBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_FOOD) + pActivePlayer:GetPlayerBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_FOOD);
		iFood = iFood + pCity:GetLeagueBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_FOOD);
-- CBP
		iFood = iFood + pCity:GetLocalBuildingClassYield(buildingClassID, YieldTypes.YIELD_FOOD);
		iFood = iFood + pCity:GetBuildingYieldChangeFromCorporationFranchises(buildingClassID, YieldTypes.YIELD_FOOD);
		iFood = iFood + pActivePlayer:GetPolicyBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_FOOD);
		-- Yield bonuses to World Wonders
		if Game.IsWorldWonderClass(buildingClassID) then
			iFood = iFood + pActivePlayer:GetExtraYieldWorldWonder(buildingID, YieldTypes.YIELD_FOOD)
		end
-- END
		-- Events
		iFood = iFood + pCity:GetEventBuildingClassYield(buildingClassID, YieldTypes.YIELD_FOOD);
		-- End 
	end
	if (iFood ~= nil and iFood ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_FOOD", iFood));
	end
	
	-- Food Modifier
	local iFoodMod = Game.GetBuildingYieldModifier(iBuildingID, YieldTypes.YIELD_FOOD);
	iFoodMod = iFoodMod + pActivePlayer:GetPolicyBuildingClassYieldModifier(buildingClassID, YieldTypes.YIELD_FOOD);
	if (pCity ~= nil) then
		iFoodMod = iFoodMod + pCity:GetReligionBuildingYieldRateModifier(buildingClassID, YieldTypes.YIELD_FOOD);
	end
	if (iFoodMod ~= nil and iFoodMod ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_FOOD_MODIFIER", iFoodMod));
	end
	
		-- Production Change
	local iProd = Game.GetBuildingYieldChange(iBuildingID, YieldTypes.YIELD_PRODUCTION);
	iProd = iProd + pActivePlayer:GetPolicyBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_PRODUCTION);
	iProd = iProd + pActivePlayer:GetBuildingTechEnhancedYields(iBuildingID, YieldTypes.YIELD_PRODUCTION);
-- CBP
	if (pCity ~= nil) then
		iProd = iProd + pCity:GetLocalBuildingClassYield(buildingClassID, YieldTypes.YIELD_PRODUCTION);
		iProd = iProd + pCity:GetBuildingYieldChangeFromCorporationFranchises(buildingClassID, YieldTypes.YIELD_PRODUCTION);
-- Start Vox Populi
		-- Yield bonuses to World Wonders
		if Game.IsWorldWonderClass(buildingClassID) then
			iProd = iProd + pActivePlayer:GetExtraYieldWorldWonder(buildingID, YieldTypes.YIELD_PRODUCTION)
		end
-- End Vox Populi
	end
-- END	
	if (pCity ~= nil) then
		iProd = iProd + pCity:GetReligionBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_PRODUCTION) + pActivePlayer:GetPlayerBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_PRODUCTION);
		iProd = iProd + pCity:GetLeagueBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_PRODUCTION);
		-- Events
		iProd = iProd + pCity:GetEventBuildingClassYield(buildingClassID, YieldTypes.YIELD_PRODUCTION);
		-- End
	end
	if (iProd ~= nil and iProd ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_PRODUCTION_CHANGE", iProd));
	end
	
	-- Production Modifier
	local iProductionMod = Game.GetBuildingYieldModifier(iBuildingID, YieldTypes.YIELD_PRODUCTION);
	iProductionMod = iProductionMod + pActivePlayer:GetPolicyBuildingClassYieldModifier(buildingClassID, YieldTypes.YIELD_PRODUCTION);
	
	if (pCity ~= nil) then
		iProductionMod = iProductionMod + pCity:GetReligionBuildingYieldRateModifier(buildingClassID, YieldTypes.YIELD_PRODUCTION);
	end

	if (iProductionMod ~= nil and iProductionMod ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_PRODUCTION", iProductionMod));
	end
		
	-- Gold Change
	iGold = Game.GetBuildingYieldChange(iBuildingID, YieldTypes.YIELD_GOLD);
	iGold = iGold + pActivePlayer:GetBuildingTechEnhancedYields(iBuildingID, YieldTypes.YIELD_GOLD);
	if (pCity ~= nil) then
		iGold = iGold + pCity:GetReligionBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_GOLD) + pActivePlayer:GetPlayerBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_GOLD);
		iGold = iGold + pCity:GetLeagueBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_GOLD);
-- CBP	
		iGold = iGold + pCity:GetLocalBuildingClassYield(buildingClassID, YieldTypes.YIELD_GOLD);
		iGold = iGold + pCity:GetBuildingYieldChangeFromCorporationFranchises(buildingClassID, YieldTypes.YIELD_GOLD);
		iGold = iGold + pActivePlayer:GetPolicyBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_GOLD);
		if Game.IsWorldWonderClass(buildingClassID) then
			iGold = iGold + pActivePlayer:GetExtraYieldWorldWonder(buildingID, YieldTypes.YIELD_GOLD)
		end
-- END
		-- Events
		iGold = iGold + pCity:GetEventBuildingClassYield(buildingClassID, YieldTypes.YIELD_GOLD);
		-- End 	
	end
	if (iGold ~= nil and iGold ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_GOLD_CHANGE", iGold));
	end
	
	-- Gold Mod
	local iGoldMod = Game.GetBuildingYieldModifier(iBuildingID, YieldTypes.YIELD_GOLD);
	iGoldMod = iGoldMod + pActivePlayer:GetPolicyBuildingClassYieldModifier(buildingClassID, YieldTypes.YIELD_GOLD);
	if (pCity ~= nil) then
		iGoldMod = iGoldMod + pCity:GetReligionBuildingYieldRateModifier(buildingClassID, YieldTypes.YIELD_GOLD);
	end
	if (iGoldMod ~= nil and iGoldMod ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_GOLD", iGoldMod));
	end
	
	-- Culture
	local iCulture = Game.GetBuildingYieldChange(iBuildingID, YieldTypes.YIELD_CULTURE);
	iCulture = iCulture + pActivePlayer:GetBuildingTechEnhancedYields(iBuildingID, YieldTypes.YIELD_CULTURE);
	if (pCity ~= nil) then
		iCulture = iCulture + pCity:GetReligionBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_CULTURE) + pActivePlayer:GetPlayerBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_CULTURE);
		iCulture = iCulture + pCity:GetLeagueBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_CULTURE);
-- CBP
		iCulture = iCulture + pCity:GetBuildingClassCultureChange(buildingClassID);
		iCulture = iCulture + pCity:GetLocalBuildingClassYield(buildingClassID, YieldTypes.YIELD_CULTURE);
		iCulture = iCulture + pCity:GetBuildingYieldChangeFromCorporationFranchises(buildingClassID, YieldTypes.YIELD_CULTURE);
		iCulture = iCulture + pActivePlayer:GetPolicyBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_CULTURE);
		-- Yield bonuses to World Wonders
		if Game.IsWorldWonderClass(buildingClassID) then
			iCulture = iCulture + pActivePlayer:GetExtraYieldWorldWonder(buildingID, YieldTypes.YIELD_CULTURE)
		end
-- END
		-- Events
		iCulture = iCulture + pCity:GetEventBuildingClassYield(buildingClassID, YieldTypes.YIELD_CULTURE);
		-- End 
	end
	if (iCulture ~= nil and iCulture ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_CULTURE", iCulture));
	end
	
	-- Culture Modifier
	local iCultureMod = pBuildingInfo.CultureRateModifier;
	iCultureMod = iCultureMod + pActivePlayer:GetPolicyBuildingClassYieldModifier(buildingClassID, YieldTypes.YIELD_CULTURE);
	if (pCity ~= nil) then
		iCultureMod = iCultureMod + pCity:GetReligionBuildingYieldRateModifier(buildingClassID, YieldTypes.YIELD_CULTURE);
	end
	if (iCultureMod ~= nil and iCultureMod ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_CULTURE_MODIFIER", iCultureMod));
	end

	-- Faith
	local iFaith = Game.GetBuildingYieldChange(iBuildingID, YieldTypes.YIELD_FAITH);
	iFaith = iFaith + pActivePlayer:GetBuildingTechEnhancedYields(iBuildingID, YieldTypes.YIELD_FAITH);
	if (pCity ~= nil) then
		iFaith = iFaith + pCity:GetReligionBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_FAITH) + pActivePlayer:GetPlayerBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_FAITH);
		iFaith = iFaith + pCity:GetLeagueBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_FAITH);
		iFaith = iFaith + pCity:GetLocalBuildingClassYield(buildingClassID, YieldTypes.YIELD_FAITH);
-- CBP
		iFaith = iFaith + pCity:GetBuildingYieldChangeFromCorporationFranchises(buildingClassID, YieldTypes.YIELD_FAITH);
		iFaith = iFaith + pActivePlayer:GetPolicyBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_FAITH);
		-- Yield bonuses to World Wonders
		if Game.IsWorldWonderClass(buildingClassID) then
			iFaith = iFaith + pActivePlayer:GetExtraYieldWorldWonder(buildingID, YieldTypes.YIELD_FAITH)
		end
-- END
		-- Events
		iFaith = iFaith + pCity:GetEventBuildingClassYield(buildingClassID, YieldTypes.YIELD_FAITH);
		-- End 
	end
	if (iFaith ~= nil and iFaith ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_FAITH", iFaith));
	end
	
	-- Faith Modifier
	local iFaithMod = Game.GetBuildingYieldModifier(iBuildingID, YieldTypes.YIELD_FAITH);
	iFaithMod = iFaithMod + pActivePlayer:GetPolicyBuildingClassYieldModifier(buildingClassID, YieldTypes.YIELD_FAITH);
	if (pCity ~= nil) then
		iFaithMod = iFaithMod + pCity:GetReligionBuildingYieldRateModifier(buildingClassID, YieldTypes.YIELD_FAITH);
	end
	
	if (iFaithMod ~= nil and iFaithMod ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_FAITH_MODIFIER", iFaithMod));
	end
	
	-- Science Change
	local iScienceChange = Game.GetBuildingYieldChange(iBuildingID, YieldTypes.YIELD_SCIENCE);
	iScienceChange = iScienceChange + pActivePlayer:GetBuildingTechEnhancedYields(iBuildingID, YieldTypes.YIELD_SCIENCE);
	if (pCity ~= nil) then
		iScienceChange = iScienceChange + pCity:GetReligionBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_SCIENCE) + pActivePlayer:GetPlayerBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_SCIENCE);
		iScienceChange = iScienceChange + pCity:GetLeagueBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_SCIENCE);
-- Start Vox Populi

		iScienceChange = iScienceChange + pCity:GetLocalBuildingClassYield(buildingClassID, YieldTypes.YIELD_SCIENCE);
		iScienceChange = iScienceChange + pCity:GetBuildingYieldChangeFromCorporationFranchises(buildingClassID, YieldTypes.YIELD_SCIENCE);
		iScienceChange = iScienceChange + pActivePlayer:GetPolicyBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_SCIENCE);
		-- Yield bonuses to World Wonders
		if Game.IsWorldWonderClass(buildingClassID) then
			iScienceChange = iScienceChange + pActivePlayer:GetExtraYieldWorldWonder(buildingID, YieldTypes.YIELD_SCIENCE)
		end
-- End Vox Populi
		-- Events
		iScienceChange = iScienceChange + pCity:GetEventBuildingClassYield(buildingClassID, YieldTypes.YIELD_SCIENCE);
		-- End 
	end
	if (iScienceChange ~= nil and iScienceChange ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_SCIENCE_CHANGE", iScienceChange));
	end
	
	-- Science Modifier
	local iScience = Game.GetBuildingYieldModifier(iBuildingID, YieldTypes.YIELD_SCIENCE);
	iScience = iScience + pActivePlayer:GetPolicyBuildingClassYieldModifier(buildingClassID, YieldTypes.YIELD_SCIENCE);
-- CBP
	if (pCity ~= nil) then
		iScience = iScience + pCity:GetReligionBuildingYieldRateModifier(buildingClassID, YieldTypes.YIELD_SCIENCE);
	end
-- END	
	if (iScience ~= nil and iScience ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_SCIENCE", iScience));
	end
	
	-- Defense
	local iDefense = pBuildingInfo.Defense;
	if (iDefense ~= nil and iDefense ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_DEFENSE", iDefense / 100));
	end
	
	-- Defense Modifier
	local iDefenseMod = pBuildingInfo.BuildingDefenseModifier;
	if (iDefenseMod ~= nil and iDefenseMod ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_DEFENSE_MODIFIER", iDefenseMod));
	end
	
	-- Hit Points
	local iHitPoints = pBuildingInfo.ExtraCityHitPoints;
	if (iHitPoints ~= nil and iHitPoints ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_HITPOINTS", iHitPoints));
	end

	-- Damage Reduction
	local iDamageReductionFlat = pBuildingInfo.DamageReductionFlat;
	if (iDamageReductionFlat ~= nil and iDamageReductionFlat ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_DAMAGE_REDUCTION", iDamageReductionFlat));
	end

	--CP EVENTS
	if (pCity ~= nil) then
		for pYieldInfo in GameInfo.Yields() do
			local iYieldID = pYieldInfo.ID;
			local iYieldAmount = pCity:GetEventBuildingClassModifier(buildingClassID, iYieldID);
							
			if (iYieldAmount > 0) then
				table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_EVENT_MODIFIER", iYieldAmount, pYieldInfo.IconString, pYieldInfo.Description));
			end
		end
	end
	--END
	
	-- Great People
	local specialistType = pBuildingInfo.SpecialistType;
	if specialistType ~= nil then
		local iNumPoints = pBuildingInfo.GreatPeopleRateChange;
		if (iNumPoints > 0) then
			table.insert(lines, "[ICON_GREAT_PEOPLE] " .. Locale.ConvertTextKey(GameInfo.Specialists[specialistType].GreatPeopleTitle) .. " " .. iNumPoints); 
		
		end
		
		if(pBuildingInfo.SpecialistCount > 0) then
			-- Append a key such as TXT_KEY_SPECIALIST_ARTIST_SLOTS
			local specialistSlotsKey = GameInfo.Specialists[specialistType].Description .. "_SLOTS";
			table.insert(lines, "[ICON_GREAT_PEOPLE] " .. Locale.ConvertTextKey(specialistSlotsKey) .. " " .. pBuildingInfo.SpecialistCount);
		end
	end
-- CBP
	if(pCity ~= nil) then
		local iCorpGPChange = pBuildingInfo.GPRateModifierPerXFranchises;
		if iCorpGPChange ~=0 then
			iCorpGPChange = pCity:GetGPRateModifierPerXFranchises();
			if iCorpGPChange ~=0 then
				local localizedText = Locale.ConvertTextKey("TXT_KEY_PEDIA_CORP_GP_CHANGE", iCorpGPChange);
				table.insert(lines, localizedText);
			end
		end
	end
-- END
	local iNumGreatWorks = pBuildingInfo.GreatWorkCount;
	if(iNumGreatWorks > 0) then
		local greatWorksSlotType = GameInfo.GreatWorkSlots[pBuildingInfo.GreatWorkSlotType];
		local localizedText = Locale.Lookup(greatWorksSlotType.SlotsToolTipText, iNumGreatWorks);
		table.insert(lines, localizedText);
	end

	local iTourism = 0;
	iTourism = iTourism + pActivePlayer:GetBuildingTechEnhancedYields(iBuildingID, YieldTypes.YIELD_TOURISM);
	iTourism = iTourism + pActivePlayer:GetExtraYieldWorldWonder(iBuildingID, YieldTypes.YIELD_TOURISM);	
	if (pCity ~= nil) then
		iTourism = iTourism + pCity:GetBuildingClassTourism(buildingClassID)
		if(pCity:GetFaithBuildingTourism() > 0 and (pBuildingInfo.FaithCost > 0 or pBuildingInfo.FaithCost > 0) and pBuildingInfo.UnlockedByBelief and pBuildingInfo.Cost == -1) then
			iTourism = iTourism + pCity:GetFaithBuildingTourism();
		end
	end

	if iTourism ~= 0 then
		local localizedText = Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_TOURISM", iTourism);
		table.insert(lines, localizedText);
	end

	strHelpText = strHelpText .. table.concat(lines, "[NEWLINE]");
	
	-- Pre-written Help text
	if (pBuildingInfo.Help ~= nil) then
		local strWrittenHelpText = Locale.ConvertTextKey( pBuildingInfo.Help );
		if (strWrittenHelpText ~= nil and strWrittenHelpText ~= "") then
			-- Separator
			strHelpText = strHelpText .. "[NEWLINE]----------------[NEWLINE]";
			strHelpText = strHelpText .. strWrittenHelpText;
		end
	end

-- CBP
	if (pCity ~= nil) then
		if(pCity:GetNumRealBuilding(iBuildingID) <= 0) then
			if(pCity:GetNumFreeBuilding(iBuildingID) <= 0) then
				local iAmount = GameDefines.BALANCE_BUILDING_INVESTMENT_BASELINE;
				iAmount = (iAmount * -1);
				local iWonderAmount = (iAmount / 2);
				local localizedText = Locale.ConvertTextKey("TXT_KEY_PRODUCTION_INVESTMENT_BUILDING", iAmount, iWonderAmount);
				-- Separator
				strHelpText = strHelpText .. "[NEWLINE]----------------[NEWLINE]";
				strHelpText = strHelpText .. localizedText;

				if(pCity:IsWorldWonder(iBuildingID)) then
					-- Separator
					local iCost = pCity:GetWorldWonderCost(iBuildingID);
					if(iCost > 0) then
						local localizedText = Locale.ConvertTextKey("TXT_KEY_WONDER_COST_INCREASE_METRIC", iCost);
						strHelpText = strHelpText .. "[NEWLINE]----------------[NEWLINE]";
						strHelpText = strHelpText .. localizedText;
					end
				end
			end
		end
	end
-- END

	return strHelpText;
	
end


-- IMPROVEMENT
function GetHelpTextForImprovement(iImprovementID, bExcludeName, bExcludeHeader, bNoMaintenance)
	local pImprovementInfo = GameInfo.Improvements[iImprovementID];
	
	local pActivePlayer = Players[Game.GetActivePlayer()];
	local pActiveTeam = Teams[Game.GetActiveTeam()];
	
	local strHelpText = "";
	
	if (not bExcludeHeader) then
		
		if (not bExcludeName) then
			-- Name
			strHelpText = strHelpText .. Locale.ToUpper(Locale.ConvertTextKey( pImprovementInfo.Description ));
			strHelpText = strHelpText .. "[NEWLINE]----------------[NEWLINE]";
		end
				
	end
		
	-- if we end up having a lot of these we may need to add some more stuff here
	
	-- Pre-written Help text
	if (pImprovementInfo.Help ~= nil) then
		local strWrittenHelpText = Locale.ConvertTextKey( pImprovementInfo.Help );
		if (strWrittenHelpText ~= nil and strWrittenHelpText ~= "") then
			-- Separator
			-- strHelpText = strHelpText .. "[NEWLINE]----------------[NEWLINE]";
			strHelpText = strHelpText .. strWrittenHelpText;
		end
	end
	
	return strHelpText;
	
end


-- PROJECT
function GetHelpTextForProject(iProjectID, pCity, bIncludeRequirementsInfo)
	local pProjectInfo = GameInfo.Projects[iProjectID];
	
	local pActivePlayer = Players[Game.GetActivePlayer()];
	local pActiveTeam = Teams[Game.GetActiveTeam()];
	
	local strHelpText = "";
	
	-- Name
	strHelpText = strHelpText .. Locale.ToUpper(Locale.ConvertTextKey( pProjectInfo.Description ));
	
	-- Cost
	local iCost = 0;
	if(pCity ~= nil)then 
		iCost = pCity:GetProjectProductionNeeded(iProjectID);
	else
		iCost = pActivePlayer:GetProjectProductionNeeded(iProjectID);
	end
	strHelpText = strHelpText .. "[NEWLINE]----------------[NEWLINE]";
	strHelpText = strHelpText .. Locale.ConvertTextKey("TXT_KEY_PRODUCTION_COST", iCost);
	
	-- Pre-written Help text
	local strWrittenHelpText = Locale.ConvertTextKey( pProjectInfo.Help );
	if (strWrittenHelpText ~= nil and strWrittenHelpText ~= "") then
		-- Separator
		strHelpText = strHelpText .. "[NEWLINE]----------------[NEWLINE]";
		strHelpText = strHelpText .. strWrittenHelpText;
	end
	
	-- Requirements?
	if (bIncludeRequirementsInfo) then
		if (pProjectInfo.Requirements) then
			strHelpText = strHelpText .. Locale.ConvertTextKey( pProjectInfo.Requirements );
		end
	end
	
	return strHelpText;
	
end


-- PROCESS
function GetHelpTextForProcess(iProcessID, bIncludeRequirementsInfo)
	local pProcessInfo = GameInfo.Processes[iProcessID];
	local pActivePlayer = Players[Game.GetActivePlayer()];
	local pActiveTeam = Teams[Game.GetActiveTeam()];
	
	local strHelpText = "";
	
	-- Name
	strHelpText = strHelpText .. Locale.ToUpper(Locale.ConvertTextKey(pProcessInfo.Description));
	
	-- Pre-written Help text
	local strWrittenHelpText = Locale.ConvertTextKey(pProcessInfo.Help);
	if (strWrittenHelpText ~= nil and strWrittenHelpText ~= "") then
		strHelpText = strHelpText .. "[NEWLINE]----------------[NEWLINE]";
		strHelpText = strHelpText .. strWrittenHelpText;
	end
	
	-- League Project text
	if (not Game.IsOption("GAMEOPTION_NO_LEAGUES")) then
		local tProject = nil;
		
		for t in GameInfo.LeagueProjects() do
			if (iProcessID == GameInfo.Processes[t.Process].ID) then
				tProject = t;
				break;
			end
		end

		local pLeague = Game.GetActiveLeague();

		if (tProject ~= nil and pLeague ~= nil) then
			strHelpText = strHelpText .. "[NEWLINE][NEWLINE]";
			strHelpText = strHelpText .. pLeague:GetProjectDetails(GameInfo.LeagueProjects[tProject.Type].ID, Game.GetActivePlayer());
		end
	end

	return strHelpText;
end

-------------------------------------------------
-- Tooltips for Yield & Similar (e.g. Culture)
-------------------------------------------------

-- FOOD
function GetFoodTooltip(pCity)
	
	local iYieldType = YieldTypes.YIELD_FOOD;
	local strFoodToolTip = "";
	
	if (not OptionsManager.IsNoBasicHelp()) then
		strFoodToolTip = strFoodToolTip .. Locale.ConvertTextKey("TXT_KEY_FOOD_HELP_INFO");
		strFoodToolTip = strFoodToolTip .. "[NEWLINE][NEWLINE]";
	end
	
	local fFoodProgress = pCity:GetFoodTimes100() / 100;
	local iFoodNeeded = pCity:GrowthThreshold();
	
	strFoodToolTip = strFoodToolTip .. Locale.ConvertTextKey("TXT_KEY_FOOD_PROGRESS", fFoodProgress, iFoodNeeded);
	
	strFoodToolTip = strFoodToolTip .. "[NEWLINE][NEWLINE]";
	strFoodToolTip = strFoodToolTip .. pCity:GetYieldRateTooltip(iYieldType);

	
	strFoodToolTip = strFoodToolTip .. pCity:getPotentialUnhappinessWithGrowth();
		
	
	return strFoodToolTip;
end

-- GOLD
function GetGoldTooltip(pCity)
	
	local iYieldType = YieldTypes.YIELD_GOLD;

	local strGoldToolTip = "";
	if (not OptionsManager.IsNoBasicHelp()) then
		strGoldToolTip = strGoldToolTip .. Locale.ConvertTextKey("TXT_KEY_GOLD_HELP_INFO");
		strGoldToolTip = strGoldToolTip .. "[NEWLINE][NEWLINE]";
	end
	
	strGoldToolTip = strGoldToolTip .. pCity:GetYieldRateTooltip(iYieldType);
	
	return strGoldToolTip;
end

-- SCIENCE
function GetScienceTooltip(pCity)
	
	local strScienceToolTip = "";

	if (Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE)) then
		strScienceToolTip = Locale.ConvertTextKey("TXT_KEY_TOP_PANEL_SCIENCE_OFF_TOOLTIP");
	else

		local iYieldType = YieldTypes.YIELD_SCIENCE;
	
		if (not OptionsManager.IsNoBasicHelp()) then
			strScienceToolTip = strScienceToolTip .. Locale.ConvertTextKey("TXT_KEY_SCIENCE_HELP_INFO");
			strScienceToolTip = strScienceToolTip .. "[NEWLINE][NEWLINE]";
		end
	
		strScienceToolTip = strScienceToolTip .. pCity:GetYieldRateTooltip(iYieldType);
	end
	
	return strScienceToolTip;
end

-- PRODUCTION
function GetProductionTooltip(pCity)
	
	local strTooltip = pCity:GetYieldRateTooltip(YieldTypes.YIELD_PRODUCTION);
	
	-- Basic explanation of production
	local strProductionHelp = "";
	if (not OptionsManager.IsNoBasicHelp()) then
		strProductionHelp = strProductionHelp .. Locale.ConvertTextKey("TXT_KEY_PRODUCTION_HELP_INFO");
		strProductionHelp = strProductionHelp .. "[NEWLINE][NEWLINE]";
		--Controls.ProductionButton:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CITYVIEW_CHANGE_PROD_TT"));
	else
		--Controls.ProductionButton:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CITYVIEW_CHANGE_PROD"));
	end
	
	return strProductionHelp .. strTooltip;
end

-- CULTURE
function GetCultureTooltip(pCity)
	
	local strCultureToolTip = "";
	local strTooltip = pCity:GetYieldRateTooltip(YieldTypes.YIELD_CULTURE);
	
	if (Game.IsOption(GameOptionTypes.GAMEOPTION_NO_POLICIES)) then
		strCultureToolTip = Locale.ConvertTextKey("TXT_KEY_TOP_PANEL_POLICIES_OFF_TOOLTIP");
	else
		if (not OptionsManager.IsNoBasicHelp()) then
			strCultureToolTip = strCultureToolTip .. Locale.ConvertTextKey("TXT_KEY_CULTURE_HELP_INFO");
			strCultureToolTip = strCultureToolTip .. "[NEWLINE][NEWLINE]";
		end
		
		-- Total
		strCultureToolTip = strCultureToolTip .. strTooltip;
	end
	
	-- Tile growth
	
	local iCultureStored = pCity:GetJONSCultureStoredTimes100() / 100;
	local iCultureNeeded = pCity:GetJONSCultureThreshold();
	local borderGrowthRate = pCity:GetYieldRateTimes100(YieldTypes.YIELD_CULTURE_LOCAL) / 100;
	strCultureToolTip = strCultureToolTip .. "[NEWLINE][NEWLINE]BORDER GROWTH[NEWLINE][NEWLINE]";
	strCultureToolTip = strCultureToolTip .. pCity:GetYieldRateTooltip(YieldTypes.YIELD_CULTURE_LOCAL);

	strCultureToolTip = strCultureToolTip .. "[NEWLINE][NEWLINE]";
	strCultureToolTip = strCultureToolTip .. Locale.ConvertTextKey("TXT_KEY_CULTURE_INFO", iCultureStored, iCultureNeeded);

	if borderGrowthRate > 0 then
		local iCultureDiff = iCultureNeeded - iCultureStored;
		local iCultureTurns = math.ceil(iCultureDiff / borderGrowthRate);
		strCultureToolTip = strCultureToolTip .. " " .. Locale.ConvertTextKey("TXT_KEY_CULTURE_TURNS", iCultureTurns);
	end

	return strCultureToolTip;
end

-- FAITH
function GetFaithTooltip(pCity)
	
	local faithTips = "";
	
	if (Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
		faithTips = faithTips .. Locale.ConvertTextKey("TXT_KEY_TOP_PANEL_RELIGION_OFF_TOOLTIP");
	else

		if (not OptionsManager.IsNoBasicHelp()) then
			faithTips = faithTips .. Locale.ConvertTextKey("TXT_KEY_FAITH_HELP_INFO");
		end
		
		-- Citizens breakdown
		faithTips = faithTips .. "[NEWLINE][NEWLINE]" .. pCity:GetYieldRateTooltip(YieldTypes.YIELD_FAITH);

		faithTips = faithTips .. "[NEWLINE]" .. GetReligionTooltip(pCity);
	end
	
	return faithTips;
end

-- TOURISM
function GetTourismTooltip(pCity)
	return pCity:GetTourismTooltip();
end

-- CBP
function GetCityHappinessTooltip(pCity)
	
	local strHappinessBreakdown = pCity:GetCityHappinessBreakdown();
	return strHappinessBreakdown;
end
function GetCityUnhappinessTooltip(pCity)
	
	local strUnhappinessBreakdown = pCity:GetCityUnhappinessBreakdown(true);
	return strUnhappinessBreakdown;
end
-- END

----------------------------------------------------------------        
-- MOOD INFO
----------------------------------------------------------------        
function GetMoodInfo(iOtherPlayer)
	
	local strInfo = "";

	local iActivePlayer = Game.GetActivePlayer();
	local pActivePlayer = Players[iActivePlayer];
	local pActiveTeam = Teams[pActivePlayer:GetTeam()];
	local pOtherPlayer = Players[iOtherPlayer];
	local iOtherTeam = pOtherPlayer:GetTeam();
	local pOtherTeam = Teams[iOtherTeam];
	local iVisibleApproach = Players[iActivePlayer]:GetApproachTowardsUsGuess(iOtherPlayer);

	-- Always war!
	if (pActiveTeam:IsAtWar(iOtherTeam)) then
		if (Game.IsOption(GameOptionTypes.GAMEOPTION_ALWAYS_WAR) or Game.IsOption(GameOptionTypes.GAMEOPTION_NO_CHANGING_WAR_PEACE)) then
			return "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_ALWAYS_WAR_TT");
		end
	end

	--  Get the opinion modifier table from the DLL and convert it into bullet points
	local aOpinion = pOtherPlayer:GetOpinionTable(iActivePlayer);
	for i,v in ipairs(aOpinion) do
		strInfo = strInfo .. "[ICON_BULLET]" .. v .. "[NEWLINE]";
	end

	--  No specific modifiers are visible, so let's see what string we should use (based on visible approach towards us)
	if (strInfo == "") then
		-- Eliminated
		if (not pOtherPlayer:IsAlive()) then
			return "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_ELIMINATED_INDICATOR");
		-- Teammates
		elseif (Game.GetActiveTeam() == iOtherTeam) then
			return "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_HUMAN_TEAMMATE");
		-- At war with us
		elseif (pActiveTeam:IsAtWar(iOtherTeam)) then
			return "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_AT_WAR");
		-- Appears Friendly
		elseif (iVisibleApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_FRIENDLY) then
			return "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_FRIENDLY");
		-- Appears Afraid
		elseif (iVisibleApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_AFRAID) then
			return "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_AFRAID");
		-- Appears Guarded
		elseif (iVisibleApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_GUARDED) then
			return "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_GUARDED");
		-- Appears Hostile
		elseif (iVisibleApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_HOSTILE) then
			return "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_HOSTILE");
		-- Appears Neutral, opinions deliberately hidden
		elseif (Game.IsHideOpinionTable() and (pOtherTeam:GetTurnsSinceMeetingTeam(pActivePlayer:GetTeam()) ~= 0 or pOtherPlayer:IsActHostileTowardsHuman(iActivePlayer))) then
			if (pOtherPlayer:IsActHostileTowardsHuman(iActivePlayer)) then
				return "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_NEUTRAL_HOSTILE");
			else
				return "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_NEUTRAL_FRIENDLY");
			end
		-- Appears Neutral, no opinions
		else
			return "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_DEFAULT_STATUS");
		end
	end
	
	-- Remove extra newline off the end if we have one
	if (Locale.EndsWith(strInfo, "[NEWLINE]")) then
		local iNewLength = Locale.Length(strInfo)-9;
		strInfo = Locale.Substring(strInfo, 1, iNewLength);
	end
	
	return strInfo;
end

------------------------------
-- Helper function to build religion tooltip string
function GetReligionTooltip(city)

	local religionToolTip = "";
	
	if (Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
		return religionToolTip;
	end

	local bFoundAFollower = false;
	local eReligion = city:GetReligiousMajority();
	local bFirst = true;
	
	if (eReligion >= 0) then
		bFoundAFollower = true;
		local religion = GameInfo.Religions[eReligion];
		local strReligion = Locale.ConvertTextKey(Game.GetReligionName(eReligion));
	    local strIcon = religion.IconString;
		local strPressure = "";
			
		if (city:IsHolyCityForReligion(eReligion)) then
			if (not bFirst) then
				religionToolTip = religionToolTip .. "[NEWLINE]";
			else
				bFirst = false;
			end
			religionToolTip = religionToolTip .. Locale.ConvertTextKey("TXT_KEY_HOLY_CITY_TOOLTIP_LINE", strIcon, strReligion);			
		end

		local iPressure;
		local iNumTradeRoutesAddingPressure;
		local iExistingPressure;
		local pressureMultiplier = GameDefines["RELIGION_MISSIONARY_PRESSURE_MULTIPLIER"];
		iPressure, iNumTradeRoutesAddingPressure, iExistingPressure = city:GetPressurePerTurn(eReligion);
		local iFollowers = city:GetNumFollowers(eReligion)
		
		if (iFollowers > 0 or math.floor(iPressure/pressureMultiplier) > 0) then
			strPressure = Locale.ConvertTextKey("TXT_KEY_RELIGIOUS_PRESSURE_STRING_EXTENDED", math.floor(iExistingPressure/pressureMultiplier), math.floor(iPressure/pressureMultiplier));
		end
					
		if (not bFirst) then
			religionToolTip = religionToolTip .. "[NEWLINE]";
		else
			bFirst = false;
		end
		
		religionToolTip = religionToolTip .. Locale.ConvertTextKey("TXT_KEY_RELIGION_TOOLTIP_LINE", strIcon, iFollowers, strPressure);
	end	
		
	local iReligionID;
	for pReligion in GameInfo.Religions() do
		iReligionID = pReligion.ID;
		
		if (iReligionID >= 0 and iReligionID ~= eReligion and city:GetNumFollowers(iReligionID) > 0) then
			bFoundAFollower = true;
			local religion = GameInfo.Religions[iReligionID];
			local strReligion = Locale.ConvertTextKey(Game.GetReligionName(iReligionID));
			local strIcon = religion.IconString;
			local strPressure = "";

			if (city:IsHolyCityForReligion(iReligionID)) then
				if (not bFirst) then
					religionToolTip = religionToolTip .. "[NEWLINE]";
				else
					bFirst = false;
				end
				religionToolTip = religionToolTip .. Locale.ConvertTextKey("TXT_KEY_HOLY_CITY_TOOLTIP_LINE", strIcon, strReligion);			
			end
				
			local iPressure;
			local iNumTradeRoutesAddingPressure;
			local iExistingPressure;
			local pressureMultiplier = GameDefines["RELIGION_MISSIONARY_PRESSURE_MULTIPLIER"];
			iPressure, iNumTradeRoutesAddingPressure, iExistingPressure = city:GetPressurePerTurn(iReligionID);
			
			local iFollowers = city:GetNumFollowers(iReligionID)
			
			if (iFollowers > 0 or math.floor(iPressure/pressureMultiplier) > 0) then
				strPressure = Locale.ConvertTextKey("TXT_KEY_RELIGIOUS_PRESSURE_STRING_EXTENDED", math.floor(iExistingPressure/pressureMultiplier), math.floor(iPressure/pressureMultiplier));
			end
					
						
			if (not bFirst) then
				religionToolTip = religionToolTip .. "[NEWLINE]";
			else
				bFirst = false;
			end
			
			religionToolTip = religionToolTip .. Locale.ConvertTextKey("TXT_KEY_RELIGION_TOOLTIP_LINE", strIcon, iFollowers, strPressure);
		end
	end
	
	if (not bFoundAFollower) then
		religionToolTip = religionToolTip .. Locale.ConvertTextKey("TXT_KEY_RELIGION_NO_FOLLOWERS");
	end
		
	return religionToolTip;
end