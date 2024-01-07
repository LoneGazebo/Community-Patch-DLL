-------------------------------------------------
-- Help text for techs
-------------------------------------------------
function GetHelpTextForTech(eTech, bShort, playerIDOverride)
	local pTech = GameInfo.Technologies[eTech];

	local strHelpText = "";

	if not bShort then
		if Game then
			local pActiveTeam = Teams[Game.GetActiveTeam()];
			local pActivePlayer = Players[Game.GetActivePlayer()];
			if(playerIDOverride ~= nil) then
				pActivePlayer = Players[playerIDOverride];
				pActiveTeam = Teams[pActivePlayer:GetTeam()];
			end
			local pTeamTechs = pActiveTeam:GetTeamTechs();
			local iTechCost = pActivePlayer:GetResearchCost(eTech);

			-- Name
			strHelpText = strHelpText .. Locale.ToUpper(Locale.ConvertTextKey(pTech.Description));

			-- Do we have this tech?
			if pTeamTechs:HasTech(eTech) then
				strHelpText = strHelpText .. " [COLOR_POSITIVE_TEXT](" .. Locale.ConvertTextKey("TXT_KEY_RESEARCHED") .. ")[ENDCOLOR]";
			end

			-- Cost/Progress
			strHelpText = strHelpText .. "[NEWLINE]-------------------------[NEWLINE]";

			local iProgress = pActivePlayer:GetResearchProgress(eTech);

			-- Don't show progress if we have 0 or we're done with the tech
			if iProgress == 0 or pTeamTechs:HasTech(eTech) then
				strHelpText = strHelpText .. " " .. Locale.ConvertTextKey("TXT_KEY_TECH_HELP_COST", iTechCost);
			else
				strHelpText = strHelpText .. " " .. Locale.ConvertTextKey("TXT_KEY_TECH_HELP_COST_WITH_PROGRESS", iProgress, iTechCost);
			end
		end

		-- Leads to...
		local strLeadsTo = "";
		local bFirstLeadsTo = true;
		for row in GameInfo.Technology_PrereqTechs() do
			local pPrereqTech = GameInfo.Technologies[row.PrereqTech];
			local pLeadsToTech = GameInfo.Technologies[row.TechType];

			if pPrereqTech and pLeadsToTech then
				if pTech.ID == pPrereqTech.ID then
					-- If this isn't the first leads-to tech, then add a comma to separate
					if bFirstLeadsTo then
						bFirstLeadsTo = false;
					else
						strLeadsTo = strLeadsTo .. ", ";
					end

					strLeadsTo = strLeadsTo .. "[COLOR_POSITIVE_TEXT]" .. Locale.ConvertTextKey( pLeadsToTech.Description ) .. "[ENDCOLOR]";
				end
			end
		end

		if strLeadsTo ~= "" then
			strHelpText = strHelpText .. "[NEWLINE]";
			strHelpText = strHelpText .. " " .. Locale.ConvertTextKey("TXT_KEY_TECH_HELP_LEADS_TO", strLeadsTo);
		end
	end

	-- Unlocks:
	local strUnlocks = strHelpText;

	local techType = pTech.Type;
	local condition = {TechType = techType};
	local prereqCondition = {PrereqTech = techType};
	local otherPrereqCondition = {TechPrereq = techType};
	local revealCondition = {TechReveal = techType};

	-- Update the units unlocked
	local strUnits = Locale.ConvertTextKey("TXT_KEY_TECH_HELP_UNITS_UNLOCKED");
	local iNumUnits = 0;
	for row in GameInfo.Units(prereqCondition) do
		if row.ShowInPedia then
			if iNumUnits > 0 then
				strUnits = strUnits .. "[NEWLINE]";
			end
			strUnits = strUnits .. " [ICON_BULLET] " .. Locale.ConvertTextKey(row.Description);
			iNumUnits = iNumUnits + 1;
		end
	end
	if iNumUnits > 0 then
		strUnlocks = strUnlocks .. strUnits;
	end

	-- Update the buildings and wonders unlocked
	local strBuildings = Locale.ConvertTextKey("TXT_KEY_TECH_HELP_BUILDINGS_UNLOCKED");
	local strWonders = Locale.ConvertTextKey("TXT_KEY_TECH_HELP_WONDERS_UNLOCKED");
	local iNumBuildings = 0;
	local iNumWonders = 0;
	for row in GameInfo.Buildings(prereqCondition) do
		if row.ShowInPedia then
			local pBuildingClass = GameInfo.BuildingClasses[row.BuildingClass];
			if pBuildingClass.MaxGlobalInstances <= 0 then
				if iNumBuildings > 0 then
					strBuildings = strBuildings .. "[NEWLINE]";
				end
				strBuildings = strBuildings .. " [ICON_BULLET] " .. Locale.ConvertTextKey(row.Description);
				iNumBuildings = iNumBuildings + 1;
			else
				if iNumWonders > 0 then
					strWonders = strWonders .. "[NEWLINE]";
				end
				strWonders = strWonders .. " [ICON_BULLET] " .. Locale.ConvertTextKey(row.Description);
				iNumWonders = iNumWonders + 1;
			end
		end
	end
	if iNumBuildings > 0 then
		strUnlocks = strUnlocks .. strBuildings;
	end
	if iNumWonders > 0 then
		strUnlocks = strUnlocks .. strWonders;
	end

	-- Update the projects unlocked
	local strProjects = Locale.ConvertTextKey("TXT_KEY_TECH_HELP_PROJECTS_UNLOCKED");
	local iNumProjects = 0;
	for row in GameInfo.Projects(otherPrereqCondition) do
		if iNumProjects > 0 then
			strProjects = strProjects .. "[NEWLINE]";
		end
		strProjects = strProjects .. " [ICON_BULLET] " .. Locale.ConvertTextKey(row.Description);
		iNumProjects = iNumProjects + 1;
	end
	if iNumProjects > 0 then
		strUnlocks = strUnlocks .. strProjects;
	end

	-- Update the processes unlocked
	local strProcesses = Locale.ConvertTextKey("TXT_KEY_TECH_HELP_PROCESSES_UNLOCKED");
	local iNumProcesses = 0;
	for row in GameInfo.Processes(otherPrereqCondition) do
		if iNumProcesses > 0 then
			strProcesses = strProcesses .. "[NEWLINE]";
		end
		strProcesses = strProcesses .. " [ICON_BULLET] " .. Locale.ConvertTextKey(row.Description);
		iNumProcesses = iNumProcesses + 1;
	end
	if iNumProcesses > 0 then
		strUnlocks = strUnlocks .. strProcesses;
	end

	-- Update the corporations unlocked
	if pTech.CorporationsEnabled then
		local strCorporations = Locale.ConvertTextKey("TXT_KEY_TECH_HELP_CORPS_UNLOCKED");
		local iNumCorporations = 0;
		for row in GameInfo.Corporations() do
			if iNumCorporations > 0 then
				strCorporations = strCorporations .. "[NEWLINE]";
			end
			strCorporations = strCorporations .. " [ICON_BULLET] " .. Locale.ConvertTextKey(row.Description);
			iNumCorporations = iNumCorporations + 1;
		end
		if iNumCorporations > 0 then
			strUnlocks = strUnlocks .. strCorporations;
		end
	end

	-- Update the resources revealed
	local strResources = Locale.ConvertTextKey("TXT_KEY_TECH_HELP_RESOURCES_UNLOCKED");
	local iNumResources = 0;
	for row in GameInfo.Resources(revealCondition) do
		if iNumResources == 0 then
			strResources = strResources .. " [ICON_BULLET] ";
		else
			strResources = strResources .. ", ";
		end
		strResources = strResources .. row.IconString .. " " .. Locale.ConvertTextKey(row.Description);
		iNumResources = iNumResources + 1;
	end
	if iNumResources > 0 then
		strUnlocks = strUnlocks .. strResources;
	end

	-- Update the builds and time reductions unlocked
	local strBuilds = Locale.ConvertTextKey("TXT_KEY_TECH_HELP_BUILDS_UNLOCKED");
	local iNumBuilds = 0;
	for row in GameInfo.Builds{PrereqTech = techType, ShowInPedia = 1} do
		if iNumBuilds > 0 then
			strBuilds = strBuilds .. "[NEWLINE]";
		end
		strBuilds = strBuilds .. " [ICON_BULLET] " .. Locale.ConvertTextKey(row.Description);
		iNumBuilds = iNumBuilds + 1;
	end
	for row in GameInfo.Build_TechTimeChanges{TechType = techType} do
		if iNumBuilds > 0 then
			strBuilds = strBuilds .. "[NEWLINE]";
		end
		local pBuild = GameInfo.Builds[row.BuildType];
		local iBuildTime = pBuild.Time;
		local pBuildFeature = GameInfo.BuildFeatures{BuildType = row.BuildType}();
		if pBuildFeature and pBuildFeature.RemoveOnly and pBuildFeature.Time then
			iBuildTime = pBuildFeature.Time;
		end
		strBuilds = strBuilds .. " [ICON_BULLET] " ..
			Locale.ConvertTextKey("TXT_KEY_TECH_HELP_BUILD_REDUCTION", pBuild.Description, DisplayPercentage(row.TimeChange / iBuildTime));
		iNumBuilds = iNumBuilds + 1;
	end
	if iNumBuilds > 0 then
		strUnlocks = strUnlocks .. strBuilds;
	end

	-- Update the special abilites
	local strAbilities = Locale.ConvertTextKey("TXT_KEY_TECH_HELP_ACTIONS_UNLOCKED") .. " [ICON_BULLET] ";
	local iNumAbilities = 0;

	for row in GameInfo.Route_TechMovementChanges(condition) do
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_MOVEMENT", GameInfo.Routes[row.RouteType].Description);
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.EmbarkedMoveChange > 0 then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ABLTY_FAST_EMBARK_STRING");
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.FeatureProductionModifier > 0 then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		local iGameSpeedBuildPercent = GameInfo.GameSpeeds[Game.GetGameSpeedType()].BuildPercent;
		local buildFeatureInfo = GameInfo.BuildFeatures{BuildType = 'BUILD_REMOVE_FOREST'}();
		local iBaseChopYield = buildFeatureInfo and buildFeatureInfo.Production or 0;
		local iGameSpeedChopYield = iBaseChopYield * iGameSpeedBuildPercent / 100;
		local iBonusChopYield = math.floor(iGameSpeedChopYield * pTech.FeatureProductionModifier / 100);
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ABLTY_TECH_BOOST_CHOP", iBonusChopYield);
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.AllowsEmbarking then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ALLOWS_EMBARKING");
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.AllowsDefensiveEmbarking then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ABLTY_DEFENSIVE_EMBARK_STRING");
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.EmbarkedAllWaterPassage then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ABLTY_OCEAN_EMBARK_STRING");
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.Happiness > 0 then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ABLTY_HAPPINESS_BUMP", pTech.Happiness);
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.BombardRange > 0 then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ABLTY_CITY_RANGE_INCREASE");
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.BombardIndirect > 0 then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ABLTY_CITY_INDIRECT_INCREASE");
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.CityLessEmbarkCost then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ABLTY_CITY_LESS_EMBARK_COST_STRING");
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.CityNoEmbarkCost then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ABLTY_CITY_NO_EMBARK_COST_STRING");
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.AllowEmbassyTradingAllowed then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ABLTY_ALLOW_EMBASSY_STRING");
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.OpenBordersTradingAllowed then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ABLTY_OPEN_BORDER_STRING");
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.DefensivePactTradingAllowed then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ABLTY_D_PACT_STRING");
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.BridgeBuilding then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ABLTY_BRIDGE_STRING");
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.MapVisible then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_REVEALS_ENTIRE_MAP");
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.InternationalTradeRoutesChange > 0 then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ADDITIONAL_INTERNATIONAL_TRADE_ROUTE");
		iNumAbilities = iNumAbilities + 1;
	end

	for row in GameInfo.Technology_TradeRouteDomainExtraRange(condition) do
		if row.Range and row.Range > 0 then
			if iNumAbilities > 0 then
				strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
			end
			if GameInfo.Domains[row.DomainType].ID == DomainTypes.DOMAIN_LAND then
				strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_EXTENDS_LAND_TRADE_ROUTE_RANGE");
			elseif GameInfo.Domains[row.DomainType].ID == DomainTypes.DOMAIN_SEA then
				strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_EXTENDS_SEA_TRADE_ROUTE_RANGE");
			end
			iNumAbilities = iNumAbilities + 1;
		end
	end

	if pTech.InfluenceSpreadModifier > 0 then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_DOUBLE_TOURISM");
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.AllowsWorldCongress then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ALLOWS_WORLD_CONGRESS");
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.ExtraVotesPerDiplomat > 0 then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_EXTRA_VOTES_FROM_DIPLOMATS", pTech.ExtraVotesPerDiplomat);
		iNumAbilities = iNumAbilities + 1;
	end

	for row in GameInfo.Technology_FreePromotions(condition) do
		local pPromotion = GameInfo.UnitPromotions[row.PromotionType];
		if pPromotion then
			if iNumAbilities > 0 then
				strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
			end
			strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_FREE_PROMOTION_FROM_TECH", pPromotion.Description, pPromotion.Help);
			iNumAbilities = iNumAbilities + 1;
		end
	end

	if pTech.ResearchAgreementTradingAllowed then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ABLTY_R_PACT_STRING");
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.MapTrading then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ABLTY_MAP_TRADING_STRING");
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.TechTrading then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ABLTY_TECH_TRADING_STRING");
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.VassalageTradingAllowed then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ABLTY_VASSALAGE_STRING");
		iNumAbilities = iNumAbilities + 1;
	end

	if pTech.CorporationsEnabled then
		if iNumAbilities > 0 then
			strAbilities = strAbilities .. "[NEWLINE] [ICON_BULLET] ";
		end
		strAbilities = strAbilities .. Locale.ConvertTextKey("TXT_KEY_ABLTY_ENABLES_CORPORATIONS");
		iNumAbilities = iNumAbilities + 1;
	end

	if iNumAbilities > 0 then
		strUnlocks = strUnlocks .. strAbilities;
	end

	local strYields = Locale.ConvertTextKey("TXT_KEY_TECH_HELP_YIELDS_UNLOCKED") .. " [ICON_BULLET] ";
	local iNumYields = 0;
	for row in GameInfo.Tech_SpecialistYieldChanges(condition) do
		if iNumYields > 0 then
			strYields = strYields .. "[NEWLINE] [ICON_BULLET] ";
		end
		strYields = strYields .. Locale.ConvertTextKey("TXT_KEY_SPECIALIST_YIELD_CHANGE",
			GameInfo.Specialists[row.SpecialistType].Description, GameInfo.Yields[row.YieldType].Description, row.Yield, GameInfo.Yields[row.YieldType].IconString);
		iNumYields = iNumYields + 1;
	end

	for row in GameInfo.Improvement_TechYieldChanges(condition) do
		if iNumYields > 0 then
			strYields = strYields .. "[NEWLINE] [ICON_BULLET] ";
		end
		strYields = strYields .. Locale.ConvertTextKey("TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_YIELDCHANGES",
			GameInfo.Improvements[row.ImprovementType].Description, GameInfo.Yields[row.YieldType].Description, row.Yield, GameInfo.Yields[row.YieldType].IconString);
		iNumYields = iNumYields + 1;
	end

	for row in GameInfo.Improvement_TechNoFreshWaterYieldChanges(condition) do
		if iNumYields > 0 then
			strYields = strYields .. "[NEWLINE] [ICON_BULLET] ";
		end
		strYields = strYields .. Locale.ConvertTextKey("TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_NOFRESHWATERYIELDCHANGES",
			GameInfo.Improvements[row.ImprovementType].Description, GameInfo.Yields[row.YieldType].Description, row.Yield, GameInfo.Yields[row.YieldType].IconString);
		iNumYields = iNumYields + 1;
	end

	for row in GameInfo.Improvement_TechFreshWaterYieldChanges(condition) do
		if iNumYields > 0 then
			strYields = strYields .. "[NEWLINE] [ICON_BULLET] ";
		end
		strYields = strYields .. Locale.ConvertTextKey("TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_FRESHWATERYIELDCHANGES",
			GameInfo.Improvements[row.ImprovementType].Description, GameInfo.Yields[row.YieldType].Description, row.Yield, GameInfo.Yields[row.YieldType].IconString);
		iNumYields = iNumYields + 1;
	end

	if iNumYields > 0 then
		strUnlocks = strUnlocks .. strYields;
	end

	return strUnlocks;
end

-- Used in trade screen
function GetShortHelpTextForTech(eTech)
	return GetHelpTextForTech(eTech, true);
end

-- Display percentage up to 1 decimal place
function DisplayPercentage(fNumber)
	if math.floor(fNumber * 1000) / 10 % 1 == 0 then
		return math.floor(fNumber * 100);
	else
		return math.floor(fNumber * 1000) / 10;
	end
end
