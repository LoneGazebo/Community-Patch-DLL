print("This is the modded InfoTooltipInclude from CBP")
-------------------------------------------------
-- Help text for Info Objects (Units, Buildings, etc.)
-------------------------------------------------


-- UNIT
function GetHelpTextForUnit(iUnitID, bIncludeRequirementsInfo)
	local pUnitInfo = GameInfo.Units[iUnitID];
	
	local pActivePlayer = Players[Game.GetActivePlayer()];
	local pActiveTeam = Teams[Game.GetActiveTeam()];

	local strHelpText = "";
	
	-- Name
	strHelpText = strHelpText .. Locale.ToUpper(Locale.ConvertTextKey( pUnitInfo.Description ));
	
	-- Cost
	strHelpText = strHelpText .. "[NEWLINE]----------------[NEWLINE]";
	
	-- Skip cost if it's 0
	if(pUnitInfo.Cost > 0) then
		strHelpText = strHelpText .. Locale.ConvertTextKey("TXT_KEY_PRODUCTION_COST", pActivePlayer:GetUnitProductionNeeded(iUnitID));
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
	
	local buildingClass = GameInfo.Buildings[iBuildingID].BuildingClass;
	local buildingClassID = GameInfo.BuildingClasses[buildingClass].ID;
	
	local strHelpText = "";
	
	local lines = {};
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
		end
		
		if(pBuildingInfo.UnlockedByLeague and Game.GetNumActiveLeagues() > 0 and not Game.IsOption("GAMEOPTION_NO_LEAGUES")) then
			local pLeague = Game.GetActiveLeague();
			if (pLeague ~= nil) then
				local iCostPerPlayer = pLeague:GetProjectBuildingCostPerPlayer(iBuildingID);
				local sCostPerPlayer = Locale.ConvertTextKey("TXT_KEY_PEDIA_COST_LABEL");
				sCostPerPlayer = sCostPerPlayer .. " " .. Locale.ConvertTextKey("TXT_KEY_LEAGUE_PROJECT_COST_PER_PLAYER", iCostPerPlayer);
				table.insert(lines, sCostPerPlayer);
			end
		end
		
		-- Maintenance
		if (not bNoMaintenance) then
			local iMaintenance = pBuildingInfo.GoldMaintenance;
			if (iMaintenance ~= nil and iMaintenance ~= 0) then		
				table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_MAINTENANCE", iMaintenance));
			end
		end
		
		-- CBP Num Social Policies
		local iNumPolicies = pBuildingInfo.NumPoliciesNeeded;
		if(pActivePlayer and iNumPolicies > 0) then
			local iNumHave = pActivePlayer:GetNumPolicies(true);
			table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PEDIA_NUM_POLICY_NEEDED_LABEL", iNumPolicies, iNumHave));
		end
		--- National/Local Population
		if(pActivePlayer) then
			local iNumNationalPop = pActivePlayer:GetScalingNationalPopulationRequrired(iBuildingID);
			if(iNumNationalPop > 0) then
				local iNumHave = pActivePlayer:GetCurrentTotalPop();
				table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PEDIA_NUM_POPULATION_NATIONAL_NEEDED_LABEL", iNumNationalPop, iNumHave));
			end
		end

		local iNumLocalPop = pBuildingInfo.LocalPopRequired;
		if(iNumLocalPop > 0) then
			if (pCity) then
				local iNumHave = pCity:GetPopulation();
				table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PEDIA_NUM_POPULATION_LOCAL_NEEDED_LABEL", iNumLocalPop, iNumHave));
			end
		end
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

-- CBP -- Global Average Modifiers
	local iGetPovertyHappinessChangeBuilding = Game.GetPovertyHappinessChangeBuilding( iBuildingID); 
	if (iGetPovertyHappinessChangeBuilding ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_POVERTY_AVERAGE_MODIFIER", iGetPovertyHappinessChangeBuilding));
	end
	local iGetDefenseHappinessChangeBuilding = Game.GetDefenseHappinessChangeBuilding( iBuildingID); 
	if (iGetDefenseHappinessChangeBuilding ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_DEFENSE_AVERAGE_MODIFIER", iGetDefenseHappinessChangeBuilding));
	end
	local iGetUnculturedHappinessChangeBuilding = Game.GetUnculturedHappinessChangeBuilding( iBuildingID); 
	if (iGetUnculturedHappinessChangeBuilding ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_CULTURE_AVERAGE_MODIFIER", iGetUnculturedHappinessChangeBuilding));
	end
	local iGetIlliteracyHappinessChangeBuilding = Game.GetIlliteracyHappinessChangeBuilding( iBuildingID); 
	if (iGetIlliteracyHappinessChangeBuilding ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_SCIENCE_AVERAGE_MODIFIER", iGetIlliteracyHappinessChangeBuilding));
	end
	local iGetMinorityHappinessChangeBuilding = Game.GetMinorityHappinessChangeBuilding( iBuildingID); 
	if (iGetMinorityHappinessChangeBuilding ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_MINORITY_AVERAGE_MODIFIER", iGetMinorityHappinessChangeBuilding));
	end

	local iGetPovertyHappinessChangeBuildingGlobal = Game.GetPovertyHappinessChangeBuildingGlobal( iBuildingID); 
	if (iGetPovertyHappinessChangeBuildingGlobal ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_POVERTY_GLOBAL_AVERAGE_MODIFIER", iGetPovertyHappinessChangeBuildingGlobal));
	end
	local iGetDefenseHappinessChangeBuildingGlobal = Game.GetDefenseHappinessChangeBuildingGlobal( iBuildingID); 
	if (iGetDefenseHappinessChangeBuildingGlobal ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_DEFENSE_GLOBAL_AVERAGE_MODIFIER", iGetDefenseHappinessChangeBuildingGlobal));
	end
	local iGetUnculturedHappinessChangeBuildingGlobal = Game.GetUnculturedHappinessChangeBuildingGlobal( iBuildingID); 
	if (iGetUnculturedHappinessChangeBuildingGlobal ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_CULTURE_GLOBAL_AVERAGE_MODIFIER", iGetUnculturedHappinessChangeBuildingGlobal));
	end
	local iGetIlliteracyHappinessChangeBuildingGlobal = Game.GetIlliteracyHappinessChangeBuildingGlobal( iBuildingID); 
	if (iGetIlliteracyHappinessChangeBuildingGlobal ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_SCIENCE_GLOBAL_AVERAGE_MODIFIER", iGetIlliteracyHappinessChangeBuildingGlobal));
	end
	local iGetMinorityHappinessChangeBuildingGlobal = Game.GetMinorityHappinessChangeBuildingGlobal( iBuildingID); 
	if (iGetMinorityHappinessChangeBuildingGlobal ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_MINORITY_GLOBAL_AVERAGE_MODIFIER", iGetMinorityHappinessChangeBuildingGlobal));
	end

	if (not OptionsManager.IsNoBasicHelp()) then	
		if (pCity ~= nil) then
			local iPovertyTotal = iGetPovertyHappinessChangeBuilding + iGetPovertyHappinessChangeBuildingGlobal;
			if(iPovertyTotal ~= 0) then
				iNewThreshold = pCity:GetTheoreticalUnhappinessDecrease(buildingID) / 100;
				local iOldThreshold = pCity:GetUnhappinessFromGoldNeeded() / 100;
				if(iNewThreshold ~= 0 and iOldThreshold ~= 0)then
					table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_POVERTY_NEW_THRESHOLD", iNewThreshold, iOldThreshold));
				end
			end		
			local iDefenseTotal = iGetDefenseHappinessChangeBuilding + iGetDefenseHappinessChangeBuildingGlobal;
			if(iDefenseTotal ~= 0) then
				iNewThreshold = pCity:GetTheoreticalUnhappinessDecrease(buildingID) / 100;
				local iOldThreshold = pCity:GetUnhappinessFromDefenseNeeded() / 100;
				if(iNewThreshold ~= 0 and iOldThreshold ~= 0)then
					table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_DEFENSE_NEW_THRESHOLD", iNewThreshold, iOldThreshold));
				end
			end
			local iIlliteracyTotal = iGetIlliteracyHappinessChangeBuilding + iGetIlliteracyHappinessChangeBuildingGlobal;
			if(iIlliteracyTotal ~= 0) then
				iNewThreshold = pCity:GetTheoreticalUnhappinessDecrease(buildingID) / 100;
				local iOldThreshold = pCity:GetUnhappinessFromScienceNeeded() / 100;
				if(iNewThreshold ~= 0 and iOldThreshold ~= 0)then
					table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_ILLITERACY_NEW_THRESHOLD", iNewThreshold, iOldThreshold));
				end
			end
			local iCultureTotal = iGetUnculturedHappinessChangeBuilding + iGetUnculturedHappinessChangeBuildingGlobal;
			if(iCultureTotal ~= 0) then
				iNewThreshold = pCity:GetTheoreticalUnhappinessDecrease(buildingID) / 100;
				local iOldThreshold = pCity:GetUnhappinessFromCultureNeeded() / 100;
				if(iNewThreshold ~= 0 and iOldThreshold ~= 0)then
					table.insert(lines, Locale.ConvertTextKey("TXT_KEY_BUILDING_CULTURE_NEW_THRESHOLD", iNewThreshold, iOldThreshold));
				end
			end		
		end
	end

-- END
	
	-- Culture
	local iCulture = Game.GetBuildingYieldChange(iBuildingID, YieldTypes.YIELD_CULTURE);
	if (pCity ~= nil) then
		iCulture = iCulture + pCity:GetReligionBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_CULTURE) + pActivePlayer:GetPlayerBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_CULTURE);
		iCulture = iCulture + pCity:GetLeagueBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_CULTURE);
-- CBP
		iCulture = iCulture + pCity:GetBuildingClassCultureChange(buildingClassID);
		iCulture = iCulture + pCity:GetLocalBuildingClassYield(buildingClassID, YieldTypes.YIELD_CULTURE);
		iCulture = iCulture + pCity:GetReligionBuildingYieldRateModifier(buildingClassID, YieldTypes.YIELD_CULTURE);

		local corporatechange = Game.GetBuildingCorporateYieldChange( iBuildingID, YieldTypes.YIELD_CULTURE )
		if (corporatechange > 0) then
			corporatechange = pCity:GetCorporationYieldChange(YieldTypes.YIELD_CULTURE)
			if(corporatechange > 0) then
				iCulture = iCulture + corporatechange;
			end
		end
-- END
		-- Events
		iCulture = iCulture + pCity:GetEventBuildingClassYield(buildingClassID, YieldTypes.YIELD_CULTURE);
		-- End 
	end
	if (iCulture ~= nil and iCulture ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_CULTURE", iCulture));
	end

	-- Faith
	local iFaith = Game.GetBuildingYieldChange(iBuildingID, YieldTypes.YIELD_FAITH);
	if (pCity ~= nil) then
		iFaith = iFaith + pCity:GetReligionBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_FAITH) + pActivePlayer:GetPlayerBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_FAITH);
		iFaith = iFaith + pCity:GetLeagueBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_FAITH);
		iFaith = iFaith + pCity:GetLocalBuildingClassYield(buildingClassID, YieldTypes.YIELD_FAITH);
-- CBP
		iFaith = iFaith + pCity:GetReligionBuildingYieldRateModifier(buildingClassID, YieldTypes.YIELD_FAITH);
		local corporatechange = Game.GetBuildingCorporateYieldChange( iBuildingID, YieldTypes.YIELD_FAITH )
		if (corporatechange > 0) then
			corporatechange = pCity:GetCorporationYieldChange(YieldTypes.YIELD_FAITH)
			if(corporatechange > 0) then
				iFaith = iFaith + corporatechange;
			end
		end
-- END
		-- Events
		iFaith = iFaith + pCity:GetEventBuildingClassYield(buildingClassID, YieldTypes.YIELD_FAITH);
		-- End 
	end
	if (iFaith ~= nil and iFaith ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_FAITH", iFaith));
	end
	
	-- Defense
	local iDefense = pBuildingInfo.Defense;
	if (iDefense ~= nil and iDefense ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_DEFENSE", iDefense / 100));
	end
	
	-- Hit Points
	local iHitPoints = pBuildingInfo.ExtraCityHitPoints;
	if (iHitPoints ~= nil and iHitPoints ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_HITPOINTS", iHitPoints));
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
	
	-- Food
	local iFood = Game.GetBuildingYieldChange(iBuildingID, YieldTypes.YIELD_FOOD);
	if (pCity ~= nil) then
		iFood = iFood + pCity:GetReligionBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_FOOD) + pActivePlayer:GetPlayerBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_FOOD);
		iFood = iFood + pCity:GetLeagueBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_FOOD);
-- CBP
		iFood = iFood + pCity:GetReligionBuildingYieldRateModifier(buildingClassID, YieldTypes.YIELD_FOOD);
		iFood = iFood + pCity:GetLocalBuildingClassYield(buildingClassID, YieldTypes.YIELD_FOOD);
		local corporatechange = Game.GetBuildingCorporateYieldChange( iBuildingID, YieldTypes.YIELD_FOOD )
		if (corporatechange > 0) then
			corporatechange = pCity:GetCorporationYieldChange(YieldTypes.YIELD_FOOD)
			if(corporatechange > 0) then
				iFood = iFood + corporatechange;
			end
		end
-- END
		-- Events
		iFood = iFood + pCity:GetEventBuildingClassYield(buildingClassID, YieldTypes.YIELD_FOOD);
		-- End 
	end
	if (iFood ~= nil and iFood ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_FOOD", iFood));
	end
	
	-- Gold Mod
	local iGold = Game.GetBuildingYieldModifier(iBuildingID, YieldTypes.YIELD_GOLD);
	iGold = iGold + pActivePlayer:GetPolicyBuildingClassYieldModifier(buildingClassID, YieldTypes.YIELD_GOLD);
	if (iGold ~= nil and iGold ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_GOLD", iGold));
	end
	
	-- Gold Change
	iGold = Game.GetBuildingYieldChange(iBuildingID, YieldTypes.YIELD_GOLD);
	if (pCity ~= nil) then
		iGold = iGold + pCity:GetReligionBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_GOLD) + pActivePlayer:GetPlayerBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_GOLD);
		iGold = iGold + pCity:GetLeagueBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_GOLD);
-- CBP	
		iGold = iGold + pCity:GetReligionBuildingYieldRateModifier(buildingClassID, YieldTypes.YIELD_GOLD);
		iGold = iGold + pCity:GetLocalBuildingClassYield(buildingClassID, YieldTypes.YIELD_GOLD);
		local corporatechange = Game.GetBuildingCorporateYieldChange( iBuildingID, YieldTypes.YIELD_GOLD )
		if (corporatechange > 0) then
			corporatechange = pCity:GetCorporationYieldChange(YieldTypes.YIELD_GOLD)
			if(corporatechange > 0) then
				iGold = iGold + corporatechange;
			end
		end
-- END
		-- Events
		iGold = iGold + pCity:GetEventBuildingClassYield(buildingClassID, YieldTypes.YIELD_GOLD);
		-- End 	
	end
	if (iGold ~= nil and iGold ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_GOLD_CHANGE", iGold));
	end
	
	-- Science
	local iScience = Game.GetBuildingYieldModifier(iBuildingID, YieldTypes.YIELD_SCIENCE);
	iScience = iScience + pActivePlayer:GetPolicyBuildingClassYieldModifier(buildingClassID, YieldTypes.YIELD_SCIENCE);
-- CBP
	if (pCity ~= nil) then
		iScience = iScience + pCity:GetReligionBuildingYieldRateModifier(buildingClassID, YieldTypes.YIELD_SCIENCE);
		iScience = iScience + pCity:GetLocalBuildingClassYield(buildingClassID, YieldTypes.YIELD_SCIENCE);
		local corporatechange = Game.GetBuildingCorporateYieldChange( iBuildingID, YieldTypes.YIELD_SCIENCE )
		if (corporatechange > 0) then
			corporatechange = pCity:GetCorporationYieldChange(YieldTypes.YIELD_SCIENCE)
			if(corporatechange > 0) then
				iScience = iScience + corporatechange;
			end
		end
	end
-- END	
	if (iScience ~= nil and iScience ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_SCIENCE", iScience));
	end
	
	-- Science
	local iScienceChange = Game.GetBuildingYieldChange(iBuildingID, YieldTypes.YIELD_SCIENCE) + pActivePlayer:GetPolicyBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_SCIENCE);
	if (pCity ~= nil) then
		iScienceChange = iScienceChange + pCity:GetReligionBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_SCIENCE) + pActivePlayer:GetPlayerBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_SCIENCE);
		iScienceChange = iScienceChange + pCity:GetLeagueBuildingClassYieldChange(buildingClassID, YieldTypes.YIELD_SCIENCE);
		-- Events
		iScienceChange = iScienceChange + pCity:GetEventBuildingClassYield(buildingClassID, YieldTypes.YIELD_SCIENCE);
		-- End 
	end
	if (iScienceChange ~= nil and iScienceChange ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_SCIENCE_CHANGE", iScienceChange));
	end
	
	-- Production
	local iProduction = Game.GetBuildingYieldModifier(iBuildingID, YieldTypes.YIELD_PRODUCTION);
	iProduction = iProduction + pActivePlayer:GetPolicyBuildingClassYieldModifier(buildingClassID, YieldTypes.YIELD_PRODUCTION);

	if (iProduction ~= nil and iProduction ~= 0) then
		table.insert(lines, Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_PRODUCTION", iProduction));
	end

	-- Production Change
	local iProd = Game.GetBuildingYieldChange(iBuildingID, YieldTypes.YIELD_PRODUCTION);
-- CBP
	if (pCity ~= nil) then
		iProd = iProd + pCity:GetReligionBuildingYieldRateModifier(buildingClassID, YieldTypes.YIELD_PRODUCTION);
		iProd = iProd + pCity:GetLocalBuildingClassYield(buildingClassID, YieldTypes.YIELD_PRODUCTION);
		local corporatechange = Game.GetBuildingCorporateYieldChange( iBuildingID, YieldTypes.YIELD_PRODUCTION )
		if (corporatechange > 0) then
			corporatechange = pCity:GetCorporationYieldChange(YieldTypes.YIELD_PRODUCTION)
			if(corporatechange > 0) then
				iProd = iProd + corporatechange;
			end
		end
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
	local iCorpGPChange = pBuildingInfo.CorporationGPChange;
	if iCorpGPChange ~=0 then
		local localizedText = Locale.ConvertTextKey("TXT_KEY_PEDIA_CORP_GP_CHANGE", iCorpGPChange);
		table.insert(lines, localizedText);
	end
-- END
	local iNumGreatWorks = pBuildingInfo.GreatWorkCount;
	if(iNumGreatWorks > 0) then
		local greatWorksSlotType = GameInfo.GreatWorkSlots[pBuildingInfo.GreatWorkSlotType];
		local localizedText = Locale.Lookup(greatWorksSlotType.SlotsToolTipText, iNumGreatWorks);
		table.insert(lines, localizedText);
	end

	if (pCity ~= nil) then
		local iTourism = pCity:GetFaithBuildingTourism();
		if(iTourism > 0 and pBuildingInfo.FaithCost > 0 and pBuildingInfo.UnlockedByBelief and pBuildingInfo.Cost == -1) then
			local localizedText = Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_TOURISM", iTourism);
			table.insert(lines, localizedText);
		end
-- CBP FIX
		if(iTourism > 0 and pBuildingInfo.FaithCost > 0 and pBuildingInfo.PolicyType ~= nil and pBuildingInfo.Cost == -1) then
			local localizedText = Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_TOURISM", iTourism);
			table.insert(lines, localizedText);
		end
	end
	-- CBP
	if (pCity ~= nil) then
		local iTourism = pActivePlayer:GetExtraYieldWorldWonder(iBuildingID, YieldTypes.YIELD_TOURISM);
		if(iTourism > 0) then
			local localizedText = Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_TOURISM", iTourism);
			table.insert(lines, localizedText);
		end
	end
	-- END
	
	local iTechEnhancedTourism = pBuildingInfo.TechEnhancedTourism;
	local iEnhancingTech = GameInfoTypes[pBuildingInfo.EnhancedYieldTech];
	if(iTechEnhancedTourism > 0 and pActiveTeam:GetTeamTechs():HasTech(iEnhancingTech)) then
		local localizedText = Locale.ConvertTextKey("TXT_KEY_PRODUCTION_BUILDING_TOURISM", iTechEnhancedTourism);
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
function GetHelpTextForProject(iProjectID, bIncludeRequirementsInfo)
	local pProjectInfo = GameInfo.Projects[iProjectID];
	
	local pActivePlayer = Players[Game.GetActivePlayer()];
	local pActiveTeam = Teams[Game.GetActiveTeam()];
	
	local strHelpText = "";
	
	-- Name
	strHelpText = strHelpText .. Locale.ToUpper(Locale.ConvertTextKey( pProjectInfo.Description ));
	
	-- Cost
	local iCost = pActivePlayer:GetProjectProductionNeeded(iProjectID);
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
	strFoodToolTip = strFoodToolTip .. GetYieldTooltipHelper(pCity, iYieldType, "[ICON_FOOD]");
	
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
	
	strGoldToolTip = strGoldToolTip .. GetYieldTooltipHelper(pCity, iYieldType, "[ICON_GOLD]");
	
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
	
		strScienceToolTip = strScienceToolTip .. GetYieldTooltipHelper(pCity, iYieldType, "[ICON_RESEARCH]");
	end
	
	return strScienceToolTip;
end

-- PRODUCTION
function GetProductionTooltip(pCity)

	local iBaseProductionPT = pCity:GetBaseYieldRate(YieldTypes.YIELD_PRODUCTION);
	local iProductionPerTurn = pCity:GetCurrentProductionDifferenceTimes100(false, false) / 100;--pCity:GetYieldRate(YieldTypes.YIELD_PRODUCTION);
	local strCodeToolTip = pCity:GetYieldModifierTooltip(YieldTypes.YIELD_PRODUCTION);
	
	local strProductionBreakdown = GetYieldTooltip(pCity, YieldTypes.YIELD_PRODUCTION, iBaseProductionPT, iProductionPerTurn, "[ICON_PRODUCTION]", strCodeToolTip);
	
	-- Basic explanation of production
	local strProductionHelp = "";
	if (not OptionsManager.IsNoBasicHelp()) then
		strProductionHelp = strProductionHelp .. Locale.ConvertTextKey("TXT_KEY_PRODUCTION_HELP_INFO");
		strProductionHelp = strProductionHelp .. "[NEWLINE][NEWLINE]";
		--Controls.ProductionButton:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CITYVIEW_CHANGE_PROD_TT"));
	else
		--Controls.ProductionButton:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CITYVIEW_CHANGE_PROD"));
	end
	
	return strProductionHelp .. strProductionBreakdown;
end

-- CULTURE
function GetCultureTooltip(pCity)
	
	local strCultureToolTip = "";
	if (Game.IsOption(GameOptionTypes.GAMEOPTION_NO_POLICIES)) then
		strCultureToolTip = Locale.ConvertTextKey("TXT_KEY_TOP_PANEL_POLICIES_OFF_TOOLTIP");
	else
		if (not OptionsManager.IsNoBasicHelp()) then
			strCultureToolTip = strCultureToolTip .. Locale.ConvertTextKey("TXT_KEY_CULTURE_HELP_INFO");
			strCultureToolTip = strCultureToolTip .. "[NEWLINE][NEWLINE]";
		end
		
		local bFirst = true;
		
		-- Culture from Buildings
		local iCultureFromBuildings = pCity:GetJONSCulturePerTurnFromBuildings();
		if (iCultureFromBuildings ~= 0) then
			
			-- Spacing
			if (bFirst) then
				bFirst = false;
			else
				strCultureToolTip = strCultureToolTip .. "[NEWLINE]";
			end
			
			strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_FROM_BUILDINGS", iCultureFromBuildings);
		end
		
		-- Culture from Policies
		local iCultureFromPolicies = pCity:GetJONSCulturePerTurnFromPolicies();
		if (iCultureFromPolicies ~= 0) then
			
			-- Spacing
			if (bFirst) then
				bFirst = false;
			else
				strCultureToolTip = strCultureToolTip .. "[NEWLINE]";
			end
			
			strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_FROM_POLICIES", iCultureFromPolicies);
		end
		
		-- Culture from Specialists
		local iCultureFromSpecialists = pCity:GetJONSCulturePerTurnFromSpecialists();
		if (iCultureFromSpecialists ~= 0) then
			--CBP
			iCultureFromSpecialists = (iCultureFromSpecialists + pCity:GetBaseYieldRateFromSpecialists(YieldTypes.YIELD_CULTURE));
			--END
			-- Spacing
			if (bFirst) then
				bFirst = false;
			else
				strCultureToolTip = strCultureToolTip .. "[NEWLINE]";
			end
			
			strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_FROM_SPECIALISTS", iCultureFromSpecialists);
		end
		
		-- Culture from Religion
		local iCultureFromReligion = pCity:GetJONSCulturePerTurnFromReligion();
		if ( iCultureFromReligion ~= 0) then
			
			-- Spacing
			if (bFirst) then
				bFirst = false;
			else
				strCultureToolTip = strCultureToolTip .. "[NEWLINE]";
			end
			
			strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_FROM_RELIGION", iCultureFromReligion);
		end

-- CBP

		-- Base Yield from Pop
		local iYieldPerPop = pCity:GetYieldPerPopTimes100(YieldTypes.YIELD_CULTURE);
		if (iYieldPerPop ~= 0) then
			iYieldPerPop = iYieldPerPop * pCity:GetPopulation();
			iYieldPerPop = iYieldPerPop / 100;
			-- Spacing
			if (bFirst) then
				bFirst = false;
			else
				strCultureToolTip = strCultureToolTip .. "[NEWLINE]";
			end
			strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_FROM_POPULATION", iYieldPerPop);
		end
-- END

-- CBP -- Yield Increase from Piety
		local iYieldFromPiety = pCity:GetReligionYieldRateModifier(YieldTypes.YIELD_CULTURE);
		if ( iYieldFromPiety ~= 0) then
			
			-- Spacing
			if (bFirst) then
				bFirst = false;
			else
				strCultureToolTip = strCultureToolTip .. "[NEWLINE]";
			end
			
			strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_FROM_PIETY", iYieldFromPiety);
		end
-- END
-- CBP -- Yield Increase from CS Alliance
		local iYieldFromCSAlliance = pCity:GetBaseYieldRateFromCSAlliance(YieldTypes.YIELD_CULTURE);
		if ( iYieldFromCSAlliance ~= 0) then
			
			-- Spacing
			if (bFirst) then
				bFirst = false;
			else
				strCultureToolTip = strCultureToolTip .. "[NEWLINE]";
			end
			
			strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_FROM_CS_ALLIANCE", iYieldFromCSAlliance);
		end
-- END		
		-- Culture from Leagues
		local iCultureFromLeagues = pCity:GetJONSCulturePerTurnFromLeagues();
		if ( iCultureFromLeagues ~= 0) then
			
			-- Spacing
			if (bFirst) then
				bFirst = false;
			else
				strCultureToolTip = strCultureToolTip .. "[NEWLINE]";
			end
			
			strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_FROM_LEAGUES", iCultureFromLeagues);
		end
		
		-- Culture from Terrain
		local iCultureFromTerrain = pCity:GetBaseYieldRateFromTerrain(YieldTypes.YIELD_CULTURE);
		if (iCultureFromTerrain ~= 0) then
			
			-- Spacing
			if (bFirst) then
				bFirst = false;
			else
				strCultureToolTip = strCultureToolTip .. "[NEWLINE]";
			end
			
			strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_FROM_TERRAIN", iCultureFromTerrain);
		end

		-- Culture from Traits
		local iCultureFromTraits = pCity:GetJONSCulturePerTurnFromTraits();
		iCultureFromTraits = (iCultureFromTraits + pCity:GetYieldPerTurnFromTraits(YieldTypes.YIELD_CULTURE));
		if (iCultureFromTraits ~= 0) then
			
			-- Spacing
			if (bFirst) then
				bFirst = false;
			else
				strCultureToolTip = strCultureToolTip .. "[NEWLINE]";
			end
			
			strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_FROM_TRAITS", iCultureFromTraits);
		end

		-- CP Events
		-- Culture from Events
		local iCultureFromEvent = pCity:GetEventCityYield(YieldTypes.YIELD_CULTURE);
		if (iCultureFromEvent ~= 0) then
			
			-- Spacing
			if (bFirst) then
				bFirst = false;
			else
				strCultureToolTip = strCultureToolTip .. "[NEWLINE]";
			end
			
			strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_FROM_EVENTS", iCultureFromEvent);
		end
		-- END 
		
		-- Empire Culture modifier
		local iAmount = Players[pCity:GetOwner()]:GetCultureCityModifier();
		if (iAmount ~= 0) then
			strCultureToolTip = strCultureToolTip .. "[NEWLINE][NEWLINE]";
			strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_PLAYER_MOD", iAmount);
		end
		
		-- City Culture modifier
		local iAmount = pCity:GetCultureRateModifier();
		if (iAmount ~= 0) then
			strCultureToolTip = strCultureToolTip .. "[NEWLINE][NEWLINE]";
			strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_CITY_MOD", iAmount);
		end
		
		-- Culture Wonders modifier
		if (pCity:GetNumWorldWonders() > 0) then
			iAmount = Players[pCity:GetOwner()]:GetCultureWonderMultiplier();
			
			if (iAmount ~= 0) then
				strCultureToolTip = strCultureToolTip .. "[NEWLINE][NEWLINE]";
				strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_WONDER_BONUS", iAmount);
			end
		end

		-- CBP -- Resource Monopoly
		if (pCity:GetCityYieldModFromMonopoly(YieldTypes.YIELD_CULTURE) > 0) then
			iAmount = pCity:GetCityYieldModFromMonopoly(YieldTypes.YIELD_CULTURE);
			
			if (iAmount ~= 0) then
				strCultureToolTip = strCultureToolTip .. "[NEWLINE][NEWLINE]";
				strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_FROM_RESOURCE_MONOPOLY", iAmount);
			end
		end

		if (pCity:GetCorporationYieldModChange(YieldTypes.YIELD_CULTURE) > 0) then
			iAmount = pCity:GetCorporationYieldModChange(YieldTypes.YIELD_CULTURE);
			
			if (iAmount ~= 0) then
				strCultureToolTip = strCultureToolTip .. "[NEWLINE][NEWLINE]";
				strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_FROM_CORPORATION", iAmount);
			end
		end
		local iAmount = pCity:GetCultureModFromCarnaval();
		if (iAmount ~= 0) then
			strCultureToolTip = strCultureToolTip .. "[NEWLINE][NEWLINE]";
			strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_WLTKD_TRAIT", iAmount);
		end
		-- END

		-- CBP
		local iAmount = pCity:GetModFromWLTKD(YieldTypes.YIELD_CULTURE);
		if (iAmount ~= 0) then
			strCultureToolTip = strCultureToolTip .. "[NEWLINE][NEWLINE]";
			strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_GOLDEN_AGE", iAmount);
		end

		local iAmount = pCity:GetModFromGoldenAge(YieldTypes.YIELD_CULTURE);
		if (iAmount ~= 0) then
			strCultureToolTip = strCultureToolTip .. "[NEWLINE][NEWLINE]";
			strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_WLTKD", iAmount);
		end
		-- END
-- CBP Yield from Great Works
		local iYieldFromGreatWorks = pCity:GetBaseYieldRateFromGreatWorks(YieldTypes.YIELD_CULTURE);
		if (iYieldFromGreatWorks ~= 0) then
			strCultureToolTip = strCultureToolTip .. "[NEWLINE][NEWLINE]";
			strCultureToolTip = strCultureToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_ART_CBP_CULTURE", iYieldFromGreatWorks);
		end
-- END		
		-- Puppet modifier
		if (pCity:IsPuppet()) then
			iAmount = GameDefines.PUPPET_CULTURE_MODIFIER;
			
			if (iAmount ~= 0) then
				strCultureToolTip = strCultureToolTip .. "[NEWLINE]";
				strCultureToolTip = strCultureToolTip .. Locale.ConvertTextKey("TXT_KEY_PRODMOD_PUPPET", iAmount);
			end
		end
	end
	
	
	-- Tile growth
	local iCulturePerTurn = pCity:GetJONSCulturePerTurn();
	local iCultureStored = pCity:GetJONSCultureStored();
	local iCultureNeeded = pCity:GetJONSCultureThreshold();

	strCultureToolTip = strCultureToolTip .. "[NEWLINE][NEWLINE]";
	strCultureToolTip = strCultureToolTip .. Locale.ConvertTextKey("TXT_KEY_CULTURE_INFO", iCultureStored, iCultureNeeded);
	
	if iCulturePerTurn > 0 then
		local iCultureDiff = iCultureNeeded - iCultureStored;
		local iCultureTurns = math.ceil(iCultureDiff / iCulturePerTurn);
		strCultureToolTip = strCultureToolTip .. " " .. Locale.ConvertTextKey("TXT_KEY_CULTURE_TURNS", iCultureTurns);
	end
	
	return strCultureToolTip;
end

-- FAITH
function GetFaithTooltip(pCity)
	
	local faithTips = {};
	
	if (Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
		table.insert(faithTips, Locale.ConvertTextKey("TXT_KEY_TOP_PANEL_RELIGION_OFF_TOOLTIP"));
	else

		if (not OptionsManager.IsNoBasicHelp()) then
			table.insert(faithTips, Locale.ConvertTextKey("TXT_KEY_FAITH_HELP_INFO"));
		end
	
		-- Faith from Buildings
		local iFaithFromBuildings = pCity:GetFaithPerTurnFromBuildings();
		if (iFaithFromBuildings ~= 0) then
		
			table.insert(faithTips, "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_FAITH_FROM_BUILDINGS", iFaithFromBuildings));
		end
-- CBP

		-- Base Yield from Pop
		local iYieldPerPop = pCity:GetYieldPerPopTimes100(YieldTypes.YIELD_FAITH);
		if (iYieldPerPop ~= 0) then
			iYieldPerPop = iYieldPerPop * pCity:GetPopulation();
			iYieldPerPop = iYieldPerPop / 100;
			
			table.insert(faithTips, "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_FAITH_FROM_POP", iYieldPerPop));
		end
		-- Faith from Specialists
		local iYieldFromSpecialists = pCity:GetBaseYieldRateFromSpecialists(YieldTypes.YIELD_FAITH);
		if (iYieldFromSpecialists ~= 0) then
			table.insert(faithTips, "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_SPECIALISTS_FAITH", iYieldFromSpecialists));
		end
-- END
		-- Faith from Traits
		local iFaithFromTraits = pCity:GetFaithPerTurnFromTraits();
		iFaithFromTraits = (iFaithFromTraits + pCity:GetYieldPerTurnFromTraits(YieldTypes.YIELD_FAITH));
		if (iFaithFromTraits ~= 0) then
				
			table.insert(faithTips, "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_FAITH_FROM_TRAITS", iFaithFromTraits));
		end

-- CBP -- Yield Increase from Piety
		local iYieldFromPiety = pCity:GetReligionYieldRateModifier(YieldTypes.YIELD_FAITH);
		if (iYieldFromPiety ~= 0) then
			table.insert(faithTips, "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_FAITH_FROM_PIETY", iYieldFromPiety));
		end
-- END
-- CBP -- Yield Increase from CS Alliance
		local iYieldFromCSAlliance = pCity:GetBaseYieldRateFromCSAlliance(YieldTypes.YIELD_FAITH);
		if (iYieldFromCSAlliance ~= 0) then
			table.insert(faithTips, "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_FAITH_FROM_CS_ALLIANCE", iYieldFromCSAlliance));
		end
-- END		
		-- Faith from Terrain
		local iFaithFromTerrain = pCity:GetBaseYieldRateFromTerrain(YieldTypes.YIELD_FAITH);
		if (iFaithFromTerrain ~= 0) then
				
			table.insert(faithTips, "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_FAITH_FROM_TERRAIN", iFaithFromTerrain));
		end

		-- Faith from Policies
		local iFaithFromPolicies = pCity:GetFaithPerTurnFromPolicies();
		if (iFaithFromPolicies ~= 0) then
					
			table.insert(faithTips, "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_FAITH_FROM_POLICIES", iFaithFromPolicies));
		end

		-- Faith from Religion
		local iFaithFromReligion = pCity:GetFaithPerTurnFromReligion();
		if (iFaithFromReligion ~= 0) then
				
			table.insert(faithTips, "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_FAITH_FROM_RELIGION", iFaithFromReligion));
		end
		-- CP Events
		-- Faith from Events
		local iFaithFromEvent = pCity:GetEventCityYield(YieldTypes.YIELD_FAITH);
		if (iFaithFromEvent ~= 0) then
			
			table.insert(faithTips, "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_FAITH_FROM_EVENTS", iFaithFromEvent));
		end
		-- END 
		-- CBP
		local iFaithWLTKDMod = pCity:GetModFromWLTKD(YieldTypes.YIELD_FAITH);
		if (iFaithWLTKDMod ~= 0) then
			table.insert(faithTips, "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_FAITH_FROM_WLTKD", iFaithWLTKDMod));
		end

		local iFaithGoldenAgeMod = pCity:GetModFromGoldenAge(YieldTypes.YIELD_FAITH);
		if (iFaithGoldenAgeMod ~= 0) then
			table.insert(faithTips, "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_FAITH_FROM_GOLDEN_AGE", iFaithGoldenAgeMod));
		end
		-- END

-- CBP Yield from Great Works
		local iYieldFromGreatWorks = pCity:GetBaseYieldRateFromGreatWorks(YieldTypes.YIELD_FAITH);
		if (iYieldFromGreatWorks ~= 0) then
			table.insert(faithTips, "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_ART_CBP_FAITH", iYieldFromGreatWorks));
		end
-- END

		-- CBP -- Resource Monopoly
		if (pCity:GetCityYieldModFromMonopoly(YieldTypes.YIELD_FAITH) > 0) then
			iAmount = pCity:GetCityYieldModFromMonopoly(YieldTypes.YIELD_FAITH);
			
			if (iAmount ~= 0) then
				table.insert(faithTips, "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_FAITH_FROM_RESOURCE_MONOPOLY", iAmount));
			end
		end
		-- END
		
		-- Puppet modifier
		if (pCity:IsPuppet()) then
			iAmount = GameDefines.PUPPET_FAITH_MODIFIER;
		
			if (iAmount ~= 0) then
				table.insert(faithTips, Locale.ConvertTextKey("TXT_KEY_PRODMOD_PUPPET", iAmount));
			end
		end
	
		-- Citizens breakdown
		table.insert(faithTips, "----------------");

		table.insert(faithTips, GetReligionTooltip(pCity));
	end
	
	local strFaithToolTip = table.concat(faithTips, "[NEWLINE]");
	return strFaithToolTip;
end

-- TOURISM
function GetTourismTooltip(pCity)
	return pCity:GetTourismTooltip();
end

-- CBP
function GetCityHappinessTooltip(pCity)
	
	local strHappinessBreakdown = "";

	if(pCity) then
		local iStarvingUnhappiness = pCity:GetUnhappinessFromStarving();
		local iPillagedUnhappiness = pCity:GetUnhappinessFromPillaged();
		local iGoldUnhappiness = pCity:GetUnhappinessFromGold();
		local iDefenseUnhappiness = pCity:GetUnhappinessFromDefense();
		local iConnectionUnhappiness = pCity:GetUnhappinessFromConnection();
		local iMinorityUnhappiness = pCity:GetUnhappinessFromMinority();
		local iScienceUnhappiness = pCity:GetUnhappinessFromScience();
		local iCultureUnhappiness = pCity:GetUnhappinessFromCulture();
		local iResistanceUnhappiness = 0;
		if(pCity:IsRazing()) then
			iResistanceUnhappiness = (pCity:GetPopulation() / 2);
		elseif(pCity:IsResistance()) then
			iResistanceUnhappiness = (pCity:GetPopulation() / 2);
		end
		local iOccupationUnhappiness = 0;
		if(pCity:IsOccupied() and not pCity:IsNoOccupiedUnhappiness()) then
			iOccupationUnhappiness = (pCity:GetPopulation() * GameDefines.UNHAPPINESS_PER_OCCUPIED_POPULATION);
		end
			
		local iTotalUnhappiness = iScienceUnhappiness + iCultureUnhappiness + iDefenseUnhappiness + iGoldUnhappiness + iConnectionUnhappiness + iPillagedUnhappiness + iStarvingUnhappiness + iMinorityUnhappiness + iOccupationUnhappiness + iResistanceUnhappiness;

		local iPuppetMod = 0;
		if(pCity:IsPuppet()) then
			iPuppetMod = Players[pCity:GetOwner()]:GetPuppetUnhappinessMod();
		end

		local iCapitalMod = 0;
		if(pCity:IsCapital()) then
			iCapitalMod = Players[pCity:GetOwner()]:GetCapitalUnhappinessModCBP();
		end

		local iThresholdAdditions = (pCity:getThresholdAdditions() - iCapitalMod);
		local iThresholdSubtractionsGold = pCity:getThresholdSubtractions(YieldTypes.YIELD_GOLD);
		local iThresholdSubtractionsDefense = pCity:getThresholdSubtractions(YieldTypes.YIELD_PRODUCTION);
		local iThresholdSubtractionsScience = pCity:getThresholdSubtractions(YieldTypes.YIELD_SCIENCE);
		local iThresholdSubtractionsCulture = pCity:getThresholdSubtractions(YieldTypes.YIELD_CULTURE);

		iThresholdSubtractionsGold = iThresholdAdditions + (iThresholdSubtractionsGold + (iPuppetMod * -1));
		iThresholdSubtractionsDefense = iThresholdAdditions + (iThresholdSubtractionsDefense + (iPuppetMod * -1));
		iThresholdSubtractionsScience = iThresholdAdditions + (iThresholdSubtractionsScience + (iPuppetMod * -1));
		iThresholdSubtractionsCulture = iThresholdAdditions + (iThresholdSubtractionsCulture + (iPuppetMod * -1));

		local iCultureYield = pCity:GetUnhappinessFromCultureYield() / 100;
		local iDefenseYield = pCity:GetUnhappinessFromDefenseYield() / 100;
		local iGoldYield = pCity:GetUnhappinessFromGoldYield() / 100;
		local iCultureNeeded = pCity:GetUnhappinessFromCultureNeeded() / 100;
		local iDefenseNeeded = pCity:GetUnhappinessFromDefenseNeeded() / 100;
		local iGoldNeeded = pCity:GetUnhappinessFromGoldNeeded() / 100;
		local iScienceYield = pCity:GetUnhappinessFromScienceYield() / 100;
		local iScienceNeeded = pCity:GetUnhappinessFromScienceNeeded() / 100;
		
		local iCultureDeficit = pCity:GetUnhappinessFromCultureDeficit() / 100;
		local iDefenseDeficit = pCity:GetUnhappinessFromDefenseDeficit() / 100;
		local iGoldDeficit = pCity:GetUnhappinessFromGoldDeficit() / 100;
		local iScienceDeficit = pCity:GetUnhappinessFromScienceDeficit() / 100;


		local iTotalHappiness = pCity:GetLocalHappiness();

		strHappinessBreakdown = Locale.ConvertTextKey("TXT_KEY_EO_CITY_LOCAL_HAPPINESS", iTotalHappiness);

		if (not OptionsManager.IsNoBasicHelp()) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_EO_CITY_LOCAL_HAPPINESS_EXPLANATION");
		end

		strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_EO_CITY_LOCAL_UNHAPPINESS", iTotalUnhappiness);
		
		-- Occupation tooltip
		if (iOccupationUnhappiness ~= 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_OCCUPATION_UNHAPPINESS", iOccupationUnhappiness);
		end
		-- Resistance tooltip
		if (iResistanceUnhappiness ~= 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_RESISTANCE_UNHAPPINESS", iResistanceUnhappiness);
		end
		-- Starving tooltip
		if (iStarvingUnhappiness ~= 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_STARVING_UNHAPPINESS", iStarvingUnhappiness);
		end
		-- Pillaged tooltip
		if (iPillagedUnhappiness ~= 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_PILLAGED_UNHAPPINESS", iPillagedUnhappiness);
		end
		-- Gold tooltip
		if (iGoldUnhappiness > 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_GOLD_UNHAPPINESS", iGoldUnhappiness, iGoldYield, iGoldNeeded, iGoldDeficit);
		end
		if ((iGoldYield - iGoldNeeded) >= 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_GOLD_UNHAPPINESS_SURPLUS", (iGoldYield - iGoldNeeded));
		end
		-- Defense tooltip
		if (iDefenseUnhappiness > 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_DEFENSE_UNHAPPINESS", iDefenseUnhappiness, iDefenseYield, iDefenseNeeded, iDefenseDeficit);
		end
		if ((iDefenseYield - iDefenseNeeded) >= 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_DEFENSE_UNHAPPINESS_SURPLUS", (iDefenseYield - iDefenseNeeded));
		end
		-- Connection tooltip
		if (iConnectionUnhappiness ~= 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_CONNECTION_UNHAPPINESS", iConnectionUnhappiness);
		end
		-- Minority tooltip
		if (iMinorityUnhappiness ~= 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_MINORITY_UNHAPPINESS", iMinorityUnhappiness);
		end
		-- Science tooltip
		if (iScienceUnhappiness > 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_SCIENCE_UNHAPPINESS", iScienceUnhappiness, iScienceYield, iScienceNeeded, iScienceDeficit);
		end
		if ((iScienceYield - iScienceNeeded) >= 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_SCIENCE_UNHAPPINESS_SURPLUS", (iScienceYield - iScienceNeeded));
		end
		-- Culture tooltip
		if (iCultureUnhappiness > 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_UNHAPPINESS", iCultureUnhappiness, iCultureYield, iCultureNeeded, iCultureDeficit);
		end
		if ((iCultureYield - iCultureNeeded) >= 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_UNHAPPINESS_SURPLUS", (iCultureYield - iCultureNeeded));
		end

		if (not OptionsManager.IsNoBasicHelp()) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_EO_CITY_LOCAL_UNHAPPINESS_EXPLANATION");
		end

		strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_EO_CITY_GLOBAL_AVERAGE_MODS");

		if (iPuppetMod ~= 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_PUPPET_UNHAPPINESS_MOD", iPuppetMod);
		end

		if (iCapitalMod ~= 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_CAPITAL_UNHAPPINESS_MOD", iCapitalMod);
		end

		if(pCity:IsRazing()) then
			return strHappinessBreakdown;
		end
		if(pCity:IsOccupied() and not pCity:IsNoOccupiedUnhappiness()) then
			return strHappinessBreakdown;
		end

		if(iThresholdSubtractionsGold > 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_GLOBAL_AVERAGE_MOD_GOLD_POSITIVE", iThresholdSubtractionsGold);
		elseif(iThresholdSubtractionsGold < 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_GLOBAL_AVERAGE_MOD_GOLD", iThresholdSubtractionsGold);
		end

		if(iThresholdSubtractionsDefense > 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_GLOBAL_AVERAGE_MOD_DEFENSE_POSITIVE", iThresholdSubtractionsDefense);
		elseif(iThresholdSubtractionsDefense < 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_GLOBAL_AVERAGE_MOD_DEFENSE", iThresholdSubtractionsDefense);
		end

		if(iThresholdSubtractionsScience > 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_GLOBAL_AVERAGE_MOD_SCIENCE_POSITIVE", iThresholdSubtractionsScience);
		elseif(iThresholdSubtractionsScience < 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_GLOBAL_AVERAGE_MOD_SCIENCE", iThresholdSubtractionsScience);
		end

		if(iThresholdSubtractionsCulture > 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_GLOBAL_AVERAGE_MOD_CULTURE_POSITIVE", iThresholdSubtractionsCulture);
		elseif(iThresholdSubtractionsCulture < 0) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_GLOBAL_AVERAGE_MOD_CULTURE", iThresholdSubtractionsCulture);
		end

		if (not OptionsManager.IsNoBasicHelp()) then
			strHappinessBreakdown = strHappinessBreakdown .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_EO_CITY_GLOBAL_AVERAGE_MODS_EXPLANATION");
		end

	end

	return strHappinessBreakdown;
end
-- END

-- Yield Tooltip Helper
function GetYieldTooltipHelper(pCity, iYieldType, strIcon)
	
	local strModifiers = "";
	
	-- Base Yield
	local iBaseYield = pCity:GetBaseYieldRate(iYieldType);

	local iYieldPerPop = pCity:GetYieldPerPopTimes100(iYieldType);
	if (iYieldPerPop ~= 0) then
		iYieldPerPop = iYieldPerPop * pCity:GetPopulation();
		iYieldPerPop = iYieldPerPop / 100;
		
		iBaseYield = iBaseYield + iYieldPerPop;
	end

	-- Total Yield
	local iTotalYield;
	
	-- Food is special
	if (iYieldType == YieldTypes.YIELD_FOOD) then
		iTotalYield = pCity:FoodDifferenceTimes100() / 100;
	else
		iTotalYield = pCity:GetYieldRateTimes100(iYieldType) / 100;
	end
	
	-- Yield modifiers string
	strModifiers = strModifiers .. pCity:GetYieldModifierTooltip(iYieldType);
	
	-- Build tooltip
	local strYieldToolTip = GetYieldTooltip(pCity, iYieldType, iBaseYield, iTotalYield, strIcon, strModifiers);
	
	return strYieldToolTip;

end


------------------------------
-- Helper function to build yield tooltip string
function GetYieldTooltip(pCity, iYieldType, iBase, iTotal, strIconString, strModifiersString)
	
	local strYieldBreakdown = "";
	
	-- Base Yield from terrain
	local iYieldFromTerrain = pCity:GetBaseYieldRateFromTerrain(iYieldType);
	if (iYieldFromTerrain ~= 0) then
		strYieldBreakdown = strYieldBreakdown .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_TERRAIN", iYieldFromTerrain, strIconString);
		strYieldBreakdown = strYieldBreakdown .. "[NEWLINE]";
	end
	
	-- Base Yield from Buildings
	local iYieldFromBuildings = pCity:GetBaseYieldRateFromBuildings(iYieldType);
	if (iYieldFromBuildings ~= 0) then
		strYieldBreakdown = strYieldBreakdown .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_BUILDINGS", iYieldFromBuildings, strIconString);
		strYieldBreakdown = strYieldBreakdown .. "[NEWLINE]";
	end
	
	-- Base Yield from Specialists
	local iYieldFromSpecialists = pCity:GetBaseYieldRateFromSpecialists(iYieldType);
	if (iYieldFromSpecialists ~= 0) then
		strYieldBreakdown = strYieldBreakdown .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_SPECIALISTS", iYieldFromSpecialists, strIconString);
		strYieldBreakdown = strYieldBreakdown .. "[NEWLINE]";
	end
	
	-- Base Yield from Misc
	local iYieldFromMisc = pCity:GetBaseYieldRateFromMisc(iYieldType);
	if (iYieldFromMisc ~= 0) then
		if (iYieldType == YieldTypes.YIELD_SCIENCE) then
			strYieldBreakdown = strYieldBreakdown .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_POP", iYieldFromMisc, strIconString);
		else
			strYieldBreakdown = strYieldBreakdown .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_MISC", iYieldFromMisc, strIconString);
		end
		strYieldBreakdown = strYieldBreakdown .. "[NEWLINE]";
	end
	-- CP Events
	-- Base Yield from Events
	local iYieldFromEvents = pCity:GetEventCityYield(iYieldType);
	if (iYieldFromEvents ~= 0) then
		strYieldBreakdown = strYieldBreakdown .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_EVENTS", iYieldFromEvents, strIconString);
	end

	-- Yield Increase from City Yields
	if (iYieldType == YieldTypes.YIELD_SCIENCE) then
		local iYieldFromYields = pCity:GetScienceFromCityYield(iYieldType);
		if (iYieldFromYields ~= 0) then
			strYieldBreakdown = strYieldBreakdown .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_CITY_YIELDS", iYieldFromYields, strIconString);
			strYieldBreakdown = strYieldBreakdown .. "[NEWLINE]";
		end
	end

	-- CBP -- Yield Increase from CS Alliance (Germany)

	local iYieldFromCSAlliance = pCity:GetBaseYieldRateFromCSAlliance(iYieldType);
	if (iYieldFromCSAlliance ~= 0) then
		strYieldBreakdown = strYieldBreakdown .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_CS_ALLIANCE", iYieldFromCSAlliance, strIconString);
		strYieldBreakdown = strYieldBreakdown .. "[NEWLINE]";
	end

	-- CBP -- Yield Increase from Corporations
	local iYieldFromCorps = pCity:GetCorporationYieldChange(iYieldType);
	if (iYieldFromCorps ~= 0) then
		strYieldBreakdown = strYieldBreakdown .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_CORPORATIONS", iYieldFromCorps, strIconString);
		strYieldBreakdown = strYieldBreakdown .. "[NEWLINE]";
	end

	-- CBP -- Yield Increase from Piety
	local iYieldFromPiety = pCity:GetReligionYieldRateModifier(iYieldType);
	if (iYieldFromPiety ~= 0) then
		strYieldBreakdown = strYieldBreakdown .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_PIETY", iYieldFromPiety, strIconString);
		strYieldBreakdown = strYieldBreakdown .. "[NEWLINE]";
	end
	
	-- Base Yield from Pop
	local iYieldPerPop = pCity:GetYieldPerPopTimes100(iYieldType);
	if (iYieldPerPop ~= 0) then
		local iYieldFromPop = iYieldPerPop * pCity:GetPopulation();
		iYieldFromPop = iYieldFromPop / 100;
		
		strYieldBreakdown = strYieldBreakdown .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_POP_EXTRA", iYieldFromPop, strIconString);
		strYieldBreakdown = strYieldBreakdown .. "[NEWLINE]";
	end
	
	-- Base Yield from Religion
	local iYieldFromReligion = pCity:GetBaseYieldRateFromReligion(iYieldType);
	if (iYieldFromReligion ~= 0) then
		strYieldBreakdown = strYieldBreakdown .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_RELIGION", iYieldFromReligion, strIconString);
		strYieldBreakdown = strYieldBreakdown .. "[NEWLINE]";
	end
	
	if (iYieldType == YieldTypes.YIELD_FOOD) then
		local iYieldFromTrade = pCity:GetYieldRate(YieldTypes.YIELD_FOOD, false) - pCity:GetYieldRate(YieldTypes.YIELD_FOOD, true);
		if (iYieldFromTrade ~= 0) then
			strYieldBreakdown = strYieldBreakdown .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_FOOD_FROM_TRADE_ROUTES", iYieldFromTrade);
			strYieldBreakdown = strYieldBreakdown .. "[NEWLINE]";
		end
	end

	-- CBP Base Yield From City Connections
	local iYieldFromConnection = pCity:GetYieldChangeTradeRoute(iYieldType);
	if (iYieldFromConnection ~= 0) then
		strYieldBreakdown = strYieldBreakdown .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_CONNECTION", iYieldFromConnection, strIconString);
		strYieldBreakdown = strYieldBreakdown .. "[NEWLINE]";
	end

-- CBP Yield from Great Works
	if (iYieldType ~= YieldTypes.YIELD_CULTURE) then
		local iYieldFromGreatWorks = pCity:GetBaseYieldRateFromGreatWorks(iYieldType);
		if (iYieldFromGreatWorks ~= 0) then
			strYieldBreakdown = strYieldBreakdown .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_ART_CBP", iYieldFromGreatWorks, strIconString);
			strYieldBreakdown = strYieldBreakdown .. "[NEWLINE]";
		end
	end

	if (iYieldType ~= YieldTypes.YIELD_CULTURE) then
		local iYieldFromTraits = pCity:GetYieldPerTurnFromTraits(iYieldType);
		if (iYieldFromTraits ~= 0) then
			strYieldBreakdown = strYieldBreakdown .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_TRAIT_BONUS", iYieldFromTraits, strIconString);
			strYieldBreakdown = strYieldBreakdown .. "[NEWLINE]";
		end
	end

	-- WLTKD MOD
	local iYieldFromWLTKD = pCity:GetModFromWLTKD(iYieldType);
	if(iYieldFromWLTKD ~= 0) then
		strYieldBreakdown = strYieldBreakdown .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_WLTKD", iYieldFromWLTKD, strIconString);
		strYieldBreakdown = strYieldBreakdown .. "[NEWLINE]";
	end

	-- Golden Age MOD
	local iYieldFromGA = pCity:GetModFromGoldenAge(iYieldType);
	if(iYieldFromGA ~= 0) then
		strYieldBreakdown = strYieldBreakdown .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_FROM_GOLDEN_AGE", iYieldFromGA, strIconString);
		strYieldBreakdown = strYieldBreakdown .. "[NEWLINE]";
	end
-- END CBP

	local strExtraBaseString = "";

-- CBP Changes -- made YieldRateTimes100	
	-- Food eaten by pop
	local iYieldEaten = 0;
	if (iYieldType == YieldTypes.YIELD_FOOD) then
		local iFoodYield = (pCity:GetYieldRateTimes100(YieldTypes.YIELD_FOOD, false) / 100);
		iYieldEaten = pCity:FoodConsumption(true, 0);
		if (iYieldEaten ~= 0) then
			--strModifiers = strModifiers .. "[NEWLINE]";
			--strModifiers = strModifiers .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_YIELD_EATEN_BY_POP", iYieldEaten, "[ICON_FOOD]");
			--strModifiers = strModifiers .. "[NEWLINE]----------------[NEWLINE]";			
			strExtraBaseString = strExtraBaseString .. "   " .. Locale.ConvertTextKey("TXT_KEY_FOOD_USAGE", iFoodYield, iYieldEaten);
			
			local iFoodSurplus = iFoodYield - iYieldEaten;
			iBase = iFoodSurplus;
-- END
			
			--if (iFoodSurplus >= 0) then
				--strModifiers = strModifiers .. Locale.ConvertTextKey("TXT_KEY_YIELD_AFTER_EATEN", iFoodSurplus, "[ICON_FOOD]");
			--else
				--strModifiers = strModifiers .. Locale.ConvertTextKey("TXT_KEY_YIELD_AFTER_EATEN_NEGATIVE", iFoodSurplus, "[ICON_FOOD]");
			--end
		end
	end
	
	local strTotal;
	if (iTotal >= 0) then
		strTotal = Locale.ConvertTextKey("TXT_KEY_YIELD_TOTAL", iTotal, strIconString);
	else
		strTotal = Locale.ConvertTextKey("TXT_KEY_YIELD_TOTAL_NEGATIVE", iTotal, strIconString);
	end
	
	strYieldBreakdown = strYieldBreakdown .. "----------------";
	
	-- Build combined string
	if (iBase ~= iTotal or strExtraBaseString ~= "") then
		local strBase = Locale.ConvertTextKey("TXT_KEY_YIELD_BASE", iBase, strIconString) .. strExtraBaseString;
		strYieldBreakdown = strYieldBreakdown .. "[NEWLINE]" .. strBase;
	end
	
	-- Modifiers
	if (strModifiersString ~= "") then
		strYieldBreakdown = strYieldBreakdown .. "[NEWLINE]----------------" .. strModifiersString .. "[NEWLINE]----------------";
	end
	strYieldBreakdown = strYieldBreakdown .. "[NEWLINE]" .. strTotal;
	
	return strYieldBreakdown;

end


----------------------------------------------------------------        
-- MOOD INFO
----------------------------------------------------------------        
function GetMoodInfo(iOtherPlayer)
	
	local strInfo = "";
	
	-- Always war!
	if (Game.IsOption(GameOptionTypes.GAMEOPTION_ALWAYS_WAR)) then
		return "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_ALWAYS_WAR_TT");
	end
	
	local iActivePlayer = Game.GetActivePlayer();
	local pActivePlayer = Players[iActivePlayer];
	local pActiveTeam = Teams[pActivePlayer:GetTeam()];
	local pOtherPlayer = Players[iOtherPlayer];
	local iOtherTeam = pOtherPlayer:GetTeam();
	local pOtherTeam = Teams[iOtherTeam];
	
	--local iVisibleApproach = Players[iActivePlayer]:GetApproachTowardsUsGuess(iOtherPlayer);
	
	-- At war right now
	--[[if (pActiveTeam:IsAtWar(iOtherTeam)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_AT_WAR") .. "[NEWLINE]";
		
	-- Not at war right now
	else
		
		-- We've fought before
		if (pActivePlayer:GetNumWarsFought(iOtherPlayer) > 0) then
			-- They don't appear to be mad
			if (iVisibleApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_FRIENDLY or 
				iVisibleApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_NEUTRAL) then
				strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_PAST_WAR_NEUTRAL") .. "[NEWLINE]";
			-- They aren't happy with us
			else
				strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_PAST_WAR_BAD") .. "[NEWLINE]";
			end
		end
	end]]--
		
	-- Neutral things
	--[[if (iVisibleApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_AFRAID) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_AFRAID") .. "[NEWLINE]";
	end]]--
		
	-- Good things
	--[[if (pOtherPlayer:WasResurrectedBy(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_RESURRECTED") .. "[NEWLINE]";
	end]]--
	--[[if (pActivePlayer:IsDoF(iOtherPlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_DOF") .. "[NEWLINE]";
	end]]--
	--[[if (pActivePlayer:IsPlayerDoFwithAnyFriend(iOtherPlayer)) then		-- Human has a mutual friend with the AI
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_MUTUAL_DOF") .. "[NEWLINE]";
	end]]--
	--[[if (pActivePlayer:IsPlayerDenouncedEnemy(iOtherPlayer)) then		-- Human has denounced an enemy of the AI
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_MUTUAL_ENEMY") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:GetNumCiviliansReturnedToMe(iActivePlayer) > 0) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_CIVILIANS_RETURNED") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherTeam:HasEmbassyAtTeam(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_HAS_EMBASSY") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:GetNumTimesIntrigueSharedBy(iActivePlayer) > 0) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_SHARED_INTRIGUE") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerForgivenForSpying(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_FORGAVE_FOR_SPYING") .. "[NEWLINE]";
	end]]--
	
	-- Bad things
	--[[if (pOtherPlayer:IsFriendDeclaredWarOnUs(iActivePlayer)) then		-- Human was a friend and declared war on us
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_HUMAN_FRIEND_DECLARED_WAR") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsFriendDenouncedUs(iActivePlayer)) then			-- Human was a friend and denounced us
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_HUMAN_FRIEND_DENOUNCED") .. "[NEWLINE]";
	end]]--
	--[[if (pActivePlayer:GetWeDeclaredWarOnFriendCount() > 0) then		-- Human declared war on friends
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_HUMAN_DECLARED_WAR_ON_FRIENDS") .. "[NEWLINE]";
	end]]--
	--[[if (pActivePlayer:GetWeDenouncedFriendCount() > 0) then			-- Human has denounced his friends
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_HUMAN_DENOUNCED_FRIENDS") .. "[NEWLINE]";
	end]]--
	--[[if (pActivePlayer:GetNumFriendsDenouncedBy() > 0) then			-- Human has been denounced by friends
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_HUMAN_DENOUNCED_BY_FRIENDS") .. "[NEWLINE]";
	end]]--
	--[[if (pActivePlayer:IsDenouncedPlayer(iOtherPlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_DENOUNCED_BY_US") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsDenouncedPlayer(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_DENOUNCED_BY_THEM") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerDoFwithAnyEnemy(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_HUMAN_DOF_WITH_ENEMY") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerDenouncedFriend(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_HUMAN_DENOUNCED_FRIEND") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerNoSettleRequestEverAsked(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_SETTLE_ASKED") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerStopSpyingRequestEverAsked(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_STOP_SPYING_ASKED") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsDemandEverMade(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_TRADE_DEMAND") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:GetNumTimesRobbedBy(iActivePlayer) > 0) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_CAUGHT_STEALING") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:GetNumTimesCultureBombed(iActivePlayer) > 0) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_CULTURE_BOMB") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:GetNegativeReligiousConversionPoints(iActivePlayer) > 0) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_RELIGIOUS_CONVERSIONS") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:HasOthersReligionInMostCities(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_ADOPTING_MY_RELIGION") .. "[NEWLINE]";
	end]]--
	--[[if (pActivePlayer:HasOthersReligionInMostCities(iOtherPlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_ADOPTING_HIS_RELIGION") .. "[NEWLINE]";
	end]]--
	--[[local myLateGamePolicies = pActivePlayer:GetLateGamePolicyTree();
	local otherLateGamePolicies = pOtherPlayer:GetLateGamePolicyTree();
	if (myLateGamePolicies ~= PolicyBranchTypes.NO_POLICY_BRANCH_TYPE and otherLateGamePolicies ~= PolicyBranchTypes.NO_POLICY_BRANCH_TYPE) then
	    local myPoliciesStr = Locale.ConvertTextKey(GameInfo.PolicyBranchTypes[myLateGamePolicies].Description);
	    print (myPoliciesStr);
		if (myLateGamePolicies == otherLateGamePolicies) then
			strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_SAME_LATE_POLICY_TREES", myPoliciesStr) .. "[NEWLINE]";
		else
			local otherPoliciesStr = Locale.ConvertTextKey(GameInfo.PolicyBranchTypes[otherLateGamePolicies].Description);
			strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_DIFFERENT_LATE_POLICY_TREES", myPoliciesStr, otherPoliciesStr) .. "[NEWLINE]";
		end
	end]]--
	--[[if (pOtherPlayer:IsPlayerBrokenMilitaryPromise(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_MILITARY_PROMISE") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerIgnoredMilitaryPromise(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_MILITARY_PROMISE_IGNORED") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerBrokenExpansionPromise(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_EXPANSION_PROMISE") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerIgnoredExpansionPromise(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_EXPANSION_PROMISE_IGNORED") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerBrokenBorderPromise(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_BORDER_PROMISE") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerIgnoredBorderPromise(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_BORDER_PROMISE_IGNORED") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerBrokenAttackCityStatePromise(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_CITY_STATE_PROMISE") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerIgnoredAttackCityStatePromise(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_CITY_STATE_PROMISE_IGNORED") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerBrokenBullyCityStatePromise(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_BULLY_CITY_STATE_PROMISE_BROKEN") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerIgnoredBullyCityStatePromise(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_BULLY_CITY_STATE_PROMISE_IGNORED") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerBrokenSpyPromise(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_SPY_PROMISE_BROKEN") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerIgnoredSpyPromise(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_SPY_PROMISE_IGNORED") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerBrokenNoConvertPromise(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CONVERT_PROMISE_BROKEN") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerIgnoredNoConvertPromise(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CONVERT_PROMISE_IGNORED") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerBrokenCoopWarPromise(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_COOP_WAR_PROMISE") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsPlayerRecklessExpander(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_RECKLESS_EXPANDER") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:GetNumRequestsRefused(iActivePlayer) > 0) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_REFUSED_REQUESTS") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:GetRecentTradeValue(iActivePlayer) > 0) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_TRADE_PARTNER") .. "[NEWLINE]";	
	end]]--
	--[[if (pOtherPlayer:GetCommonFoeValue(iActivePlayer) > 0) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_COMMON_FOE") .. "[NEWLINE]";	
	end]]--
	--[[if (pOtherPlayer:GetRecentAssistValue(iActivePlayer) > 0) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_ASSISTANCE_TO_THEM") .. "[NEWLINE]";	
	end	]]--
	--[[if (pOtherPlayer:IsLiberatedCapital(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_LIBERATED_CAPITAL") .. "[NEWLINE]";	
	end]]--
	--[[if (pOtherPlayer:IsLiberatedCity(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_LIBERATED_CITY") .. "[NEWLINE]";	
	end	]]--
	--[[if (pOtherPlayer:IsGaveAssistanceTo(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_ASSISTANCE_FROM_THEM") .. "[NEWLINE]";	
	end	]]--	
	--[[if (pOtherPlayer:IsHasPaidTributeTo(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_PAID_TRIBUTE") .. "[NEWLINE]";	
	end	]]--
	--[[if (pOtherPlayer:IsNukedBy(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_NUKED") .. "[NEWLINE]";	
	end]]--	
	--[[if (pOtherPlayer:IsCapitalCapturedBy(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_CAPTURED_CAPITAL") .. "[NEWLINE]";	
	end	]]--

	-- Protected Minors
	--[[if (pOtherPlayer:IsAngryAboutProtectedMinorKilled(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_PROTECTED_MINORS_KILLED") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsAngryAboutProtectedMinorAttacked(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_PROTECTED_MINORS_ATTACKED") .. "[NEWLINE]";
	end]]--
	--[[if (pOtherPlayer:IsAngryAboutProtectedMinorBullied(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_PROTECTED_MINORS_BULLIED") .. "[NEWLINE]";
	end]]--
	
	-- Bullied Minors
	--[[if (pOtherPlayer:IsAngryAboutSidedWithTheirProtectedMinor(iActivePlayer)) then
		strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_SIDED_WITH_MINOR") .. "[NEWLINE]";
	end]]--
	
	--local iActualApproach = pOtherPlayer:GetMajorCivApproach(iActivePlayer)
	
	-- MOVED TO LUAPLAYER
	--[[
	-- Bad things we don't want visible if someone is friendly (acting or truthfully)
	if (iVisibleApproach ~= MajorCivApproachTypes.MAJOR_CIV_APPROACH_FRIENDLY) then-- and
		--iActualApproach ~= MajorCivApproachTypes.MAJOR_CIV_APPROACH_DECEPTIVE) then
		if (pOtherPlayer:GetLandDisputeLevel(iActivePlayer) > DisputeLevelTypes.DISPUTE_LEVEL_NONE) then
			strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_LAND_DISPUTE") .. "[NEWLINE]";
		end
		--if (pOtherPlayer:GetVictoryDisputeLevel(iActivePlayer) > DisputeLevelTypes.DISPUTE_LEVEL_NONE) then
			--strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_VICTORY_DISPUTE") .. "[NEWLINE]";
		--end
		if (pOtherPlayer:GetWonderDisputeLevel(iActivePlayer) > DisputeLevelTypes.DISPUTE_LEVEL_NONE) then
			strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_WONDER_DISPUTE") .. "[NEWLINE]";
		end
		if (pOtherPlayer:GetMinorCivDisputeLevel(iActivePlayer) > DisputeLevelTypes.DISPUTE_LEVEL_NONE) then
			strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_MINOR_CIV_DISPUTE") .. "[NEWLINE]";
		end
		if (pOtherPlayer:GetWarmongerThreat(iActivePlayer) > ThreatTypes.THREAT_NONE) then
			strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_WARMONGER_THREAT") .. "[NEWLINE]";
		end
	end
	]]--
	
	local aOpinion = pOtherPlayer:GetOpinionTable(iActivePlayer);
	--local aOpinionList = {};
	for i,v in ipairs(aOpinion) do
		--aOpinionList[i] = "[ICON_BULLET]" .. v .. "[NEWLINE]";
		strInfo = strInfo .. "[ICON_BULLET]" .. v .. "[NEWLINE]";
	end
	--strInfo = table.cat(aOpinionList, "[NEWLINE]");

	--  No specific events - let's see what string we should use
	if (strInfo == "") then
		
		-- Appears Friendly
		if (iVisibleApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_FRIENDLY) then
			strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_FRIENDLY");
		-- Appears Guarded
		elseif (iVisibleApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_GUARDED) then
			strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_GUARDED");
		-- Appears Hostile
		elseif (iVisibleApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_HOSTILE) then
			strInfo = strInfo .. "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_HOSTILE");
		-- Neutral - default string
		else
			strInfo = "[ICON_BULLET]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_DEFAULT_STATUS");
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
		iPressure, iNumTradeRoutesAddingPressure = city:GetPressurePerTurn(eReligion);
		if (iPressure > 0) then
			strPressure = Locale.ConvertTextKey("TXT_KEY_RELIGIOUS_PRESSURE_STRING", math.floor(iPressure/GameDefines["RELIGION_MISSIONARY_PRESSURE_MULTIPLIER"]));
		end
		
		local iFollowers = city:GetNumFollowers(eReligion)			
		if (not bFirst) then
			religionToolTip = religionToolTip .. "[NEWLINE]";
		else
			bFirst = false;
		end
		
		--local iNumTradeRoutesAddingPressure = city:GetNumTradeRoutesAddingPressure(eReligion);
		if (iNumTradeRoutesAddingPressure > 0) then
			religionToolTip = religionToolTip .. Locale.ConvertTextKey("TXT_KEY_RELIGION_TOOLTIP_LINE_WITH_TRADE", strIcon, iFollowers, strPressure, iNumTradeRoutesAddingPressure);
		else
			religionToolTip = religionToolTip .. Locale.ConvertTextKey("TXT_KEY_RELIGION_TOOLTIP_LINE", strIcon, iFollowers, strPressure);
		end
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
				
			local iPressure = city:GetPressurePerTurn(iReligionID);
			if (iPressure > 0) then
				strPressure = Locale.ConvertTextKey("TXT_KEY_RELIGIOUS_PRESSURE_STRING", math.floor(iPressure/GameDefines["RELIGION_MISSIONARY_PRESSURE_MULTIPLIER"]));
			end
			
			local iFollowers = city:GetNumFollowers(iReligionID)			
			if (not bFirst) then
				religionToolTip = religionToolTip .. "[NEWLINE]";
			else
				bFirst = false;
			end
			
			local iNumTradeRoutesAddingPressure = city:GetNumTradeRoutesAddingPressure(iReligionID);
			if (iNumTradeRoutesAddingPressure > 0) then
				religionToolTip = religionToolTip .. Locale.ConvertTextKey("TXT_KEY_RELIGION_TOOLTIP_LINE_WITH_TRADE", strIcon, iFollowers, strPressure, iNumTradeRoutesAddingPressure);
			else
				religionToolTip = religionToolTip .. Locale.ConvertTextKey("TXT_KEY_RELIGION_TOOLTIP_LINE", strIcon, iFollowers, strPressure);
			end
		end
	end
	
	if (not bFoundAFollower) then
		religionToolTip = religionToolTip .. Locale.ConvertTextKey("TXT_KEY_RELIGION_NO_FOLLOWERS");
	end
		
	return religionToolTip;
end