-------------------------------------------------
-- Include file that has handy stuff for the tech tree and other screens that need to show a tech button
-------------------------------------------------
include("IconSupport");
include("InfoTooltipInclude");
include("VPUI_core");

-- Default icon
local strDefaultTexture = "UnitActions360.dds";
local vNullOffset = Vector2(0, 0);

local bResearchAgreements = Game.IsOption("GAMEOPTION_RESEARCH_AGREEMENTS");
local bNoTechTrading = Game.IsOption("GAMEOPTION_NO_TECH_TRADING");
local bNoVassalage = Game.IsOption("GAMEOPTION_NO_VASSALAGE");

local MOD_EVENTS_CITY_BOMBARD = Game.IsCustomModOption("EVENTS_CITY_BOMBARD")
	and not Game.IsCustomModOption("CORE_NO_RANGED_ATTACK_FROM_CITIES")
	and not Game.IsCustomModOption("BALANCE_CORE_BOMBARD_RANGE_BUILDINGS");

local L = Locale.Lookup;
local VP = MapModData and MapModData.VP or VP;
local GetInfoFromId = VP.GetInfoFromId;
local GetInfoFromType = VP.GetInfoFromType;
local GameInfoCache = VP.GameInfoCache;
local GetCivsFromTrait = VP.GetCivsFromTrait;

-- This has to stay until we override TechTree.lua
techPediaSearchStrings = {};
function GetTechPedia(_, _, button)
	Events.SearchForPediaEntry(techPediaSearchStrings[tostring(button)]);
end

-- These have to stay until we override TechPopup.lua and TechTree.lua
turnsString = L("TXT_KEY_TURNS");
freeString = L("TXT_KEY_FREE");
lockedString = "[ICON_LOCKED]";

-- VP/bal: gamespeed is currently only used to calculate chop yields, possibly can be applied in other places too
-- Assume forest and jungle have the same base chop production
local iBuildPercent = GameInfo.GameSpeeds[Game.GetGameSpeedType()].BuildPercent;
local iBaseChopYield = GameInfo.BuildFeatures{BuildType = 'BUILD_REMOVE_FOREST'}().Production;

--- Remove the sign of the first parameter and append it as the second parameter<br>
--- More flexible than using the "number" construct
--- @param strTextKey string
--- @param nFirstParameter number
--- @param ... string|number
--- @return string # The localized string
local function GetSignedTooltip(strTextKey, nFirstParameter, ...)
	local strSign = nFirstParameter >= 0 and "+" or "-";
	nFirstParameter = math.abs(nFirstParameter);
	return L(strTextKey, nFirstParameter, strSign, ...);
end

--- Short hand for building a yield boost string
--- @param kYieldInfo table
--- @param nYield number
--- @return string # The localized string
local function GetYieldBoostString(kYieldInfo, nYield)
	return L("TXT_KEY_TECH_HELP_YIELD_CHANGE", nYield, kYieldInfo.IconString);
end

--- Given a yield table, generate the yield icons on the button and the localized yield boost string
--- @param tYieldBoosts table<integer, number>
--- @return string # The button yield icons
--- @return string # The string containing all the yield boosts
local function GetYieldBoostsString(tYieldBoosts)
	local strButtonText = "";
	local tYieldStrings = {};
	for eYield, kYieldInfo in GameInfoCache("Yields") do
		if tYieldBoosts[eYield] and tYieldBoosts[eYield] ~= 0 then
			table.insert(tYieldStrings, GetYieldBoostString(kYieldInfo, tYieldBoosts[eYield]));
			strButtonText = strButtonText .. kYieldInfo.IconString;
		end
	end
	return strButtonText, table.concat(tYieldStrings, " ");
end

--- Populate small buttons on a given tech<br>
--- The small buttons need to be named "B1", "B2", "B3", ...
--- @param buttonStack Instance UI instance containing the small buttons
--- @param kTechInfo Info Database entry of the tech
--- @param iButtonCount integer Number of small buttons
--- @param iTextureSize integer Size of the small button icons
--- @param ePlayerOverride PlayerId? View in this player's perspective instead of the active player's
--- @param bSkipTooltips boolean? Whether unit, building, improvement, project, process tooltips (potentially time-consuming) should be skipped
--- @return integer # The number of small buttons on the tech **+ 1**
function AddSmallButtonsToTechButton(buttonStack, kTechInfo, iButtonCount, iTextureSize, ePlayerOverride, bSkipTooltips)
	-- Gather info we need
	local ePlayer = ePlayerOverride or Game.GetActivePlayer();
	local pPlayer = Players[ePlayer];
	local strCivType;
	local eCiv = -1;
	if pPlayer then
		eCiv = pPlayer:GetCivilizationType();
		strCivType = GetInfoFromId("Civilizations", pPlayer:GetCivilizationType()).Type;
	end

	--- Given a database table with TechType, <X>Type, YieldType and Yield, extract all entries of the given tech/yield pair.
	--- Duplicate rows are overwritten.
	--- @param tYieldTable table<integer, table<integer, integer?>?> The yield table, could be empty or partially filled
	--- @param strDatabaseTable string The name of the database table to be extracted
	--- @param kYieldInfo table The yield entry
	--- @param strOtherColumn string The name of the other column (in the form of <X>Type)
	--- @param strYieldColumnName string|nil The name of the yield column (default "Yield" if nil)
	local function ExtractYieldTable(tYieldTable, strDatabaseTable, kYieldInfo, strOtherColumn, strYieldColumnName)
		local eYield = kYieldInfo.ID;
		strYieldColumnName = strYieldColumnName or "Yield";
		for row in GameInfo[strDatabaseTable]{TechType = kTechInfo.Type, YieldType = kYieldInfo.Type} do
			local eOtherType = GameInfoTypes[row[strOtherColumn]];
			tYieldTable[eOtherType] = tYieldTable[eOtherType] or {};
			tYieldTable[eOtherType][eYield] = row[strYieldColumnName];
		end
	end

	local function CanPlayerEverBuildImprovement(strImprovementType)
		if not strImprovementType or not pPlayer then
			return false;
		end

		local kImprovementInfo = GameInfo.Improvements[strImprovementType];
		if kImprovementInfo.GraphicalOnly then
			return false;
		end

		if kImprovementInfo.CivilizationType and kImprovementInfo.CivilizationType ~= strCivType then
			return false;
		end

		return true;
	end

	local tCanEverBuildImprovementCache = {};
	local function CanPlayerEverBuildImprovementCached(strImprovementType)
		if tCanEverBuildImprovementCache[strImprovementType] == nil then
			tCanEverBuildImprovementCache[strImprovementType] = CanPlayerEverBuildImprovement(strImprovementType);
		end
		return tCanEverBuildImprovementCache[strImprovementType];
	end

	local function CanPlayerEverBuild(eBuilding)
		if not pPlayer then
			return false;
		end

		local kBuildingInfo = GetInfoFromId("Buildings", eBuilding);
		if kBuildingInfo.CivilizationRequired and kBuildingInfo.CivilizationRequired ~= strCivType then
			return false;
		end

		local eCheckBuildingClass = GameInfoTypes[kBuildingInfo.BuildingClass];
		if pPlayer:GetCivilizationBuilding(eCheckBuildingClass) ~= kBuildingInfo.ID then
			return false;
		end

		return true;
	end

	local tCanEverBuildCache = {};
	local function CanPlayerEverBuildCached(eBuilding)
		if tCanEverBuildCache[eBuilding] == nil then
			tCanEverBuildCache[eBuilding] = CanPlayerEverBuild(eBuilding);
		end
		return tCanEverBuildCache[eBuilding];
	end

	local tPediaSearchStrings = {};
	local function GetTechPedia(_, _, button)
		Events.SearchForPediaEntry(tPediaSearchStrings[tostring(button)]);
	end

	--- @param button Control
	--- @param bRClick boolean
	--- @param iIconIndex integer
	--- @param strAtlas string
	--- @param strTooltip string
	--- @param strButtonText string?
	local function SetCommonButtonProperties(button, bRClick, iIconIndex, strAtlas, strTooltip, strButtonText)
		button:SetToolTipString(strTooltip);
		if not IconHookup(iIconIndex, iTextureSize, strAtlas, button) then
			button:SetTexture(strDefaultTexture);
			button:SetTextureOffset(vNullOffset);
		end
		if strButtonText and strButtonText ~= "" then
			button:GetTextControl():SetOffsetY(iTextureSize / 3);
			button:GetTextControl():SetAlpha(0.8);
			button:SetText(strButtonText);
		end
		if bRClick then
			button:RegisterCallback(Mouse.eRClick, GetTechPedia);
		end
		button:SetHide(false);
	end

	--- @param button Control
	--- @param kUnitInfo Info
	--- @param strCustomTooltip string?
	--- @param strButtonText string?
	local function SetupUnitButton(button, kUnitInfo, strCustomTooltip, strButtonText)
		local strTooltip = strCustomTooltip or (not bSkipTooltips and GetHelpTextForUnit(kUnitInfo.ID, true) or "");
		tPediaSearchStrings[tostring(button)] = L(kUnitInfo.Description);
		local iIconIndex, strAtlas = UI.GetUnitPortraitIcon(kUnitInfo.ID, ePlayer);
		SetCommonButtonProperties(button, true, iIconIndex, strAtlas, strTooltip, strButtonText);
	end

	--- @param button Control
	--- @param kCorporationInfo Info
	--- @param strCustomTooltip string?
	--- @param strButtonText string?
	local function SetupCorporationButton(button, kCorporationInfo, strCustomTooltip, strButtonText)
		local strTooltip = strCustomTooltip or
			string.format("[COLOR_YIELD_GOLD]%s[ENDCOLOR][NEWLINE]----------------[NEWLINE]%s", L(kCorporationInfo.Description), L(kCorporationInfo.Help));
		tPediaSearchStrings[tostring(button)] = L(kCorporationInfo.Description);
		SetCommonButtonProperties(button, true, kCorporationInfo.PortraitIndex, kCorporationInfo.IconAtlas, strTooltip, strButtonText);
		-- Dim Corporations already taken
		if not strCustomTooltip and Game.IsCorporationFounded(kCorporationInfo.ID) then
			button:SetAlpha(0.33);
		end
	end

	--- @param button Control
	--- @param kBuildingInfo Info
	--- @param strCustomTooltip string?
	--- @param strButtonText string?
	local function SetupBuildingButton(button, kBuildingInfo, strCustomTooltip, strButtonText)
		local strTooltip = strCustomTooltip or GetHelpTextForBuilding(kBuildingInfo.ID, false, nil, false);
		tPediaSearchStrings[tostring(button)] = L(kBuildingInfo.Description);
		SetCommonButtonProperties(button, true, kBuildingInfo.PortraitIndex, kBuildingInfo.IconAtlas, strTooltip, strButtonText);
		-- Dim Wonders already taken
		if not strCustomTooltip and (Game.AnyoneHasWonder(kBuildingInfo.ID) and GetInfoFromType("BuildingClasses", kBuildingInfo.BuildingClass).MaxGlobalInstances == 1) then
			button:SetAlpha(0.33);
		end
	end

	--- @param button Control
	--- @param kProjectInfo Info
	--- @param strCustomTooltip string?
	--- @param strButtonText string?
	local function SetupProjectButton(button, kProjectInfo, strCustomTooltip, strButtonText)
		local strTooltip = strCustomTooltip or GetHelpTextForProject(kProjectInfo.ID, true);
		tPediaSearchStrings[tostring(button)] = L(kProjectInfo.Description);
		SetCommonButtonProperties(button, true, kProjectInfo.PortraitIndex, kProjectInfo.IconAtlas, strTooltip, strButtonText);
	end

	--- @param button Control
	--- @param kResourceInfo Info
	--- @param strCustomTooltip string?
	--- @param strButtonText string?
	local function SetupResourceButton(button, kResourceInfo, strCustomTooltip, strButtonText)
		local strTooltip = strCustomTooltip or L("TXT_KEY_REVEALS_RESOURCE_ON_MAP", kResourceInfo.Description);
		tPediaSearchStrings[tostring(button)] = L(kResourceInfo.Description);
		SetCommonButtonProperties(button, true, kResourceInfo.PortraitIndex, kResourceInfo.IconAtlas, strTooltip, strButtonText);
	end

	--- @param button Control
	--- @param kProcessInfo Info
	--- @param strCustomTooltip string?
	--- @param strButtonText string?
	local function SetupProcessButton(button, kProcessInfo, strCustomTooltip, strButtonText)
		local strTooltip = strCustomTooltip or GetHelpTextForProcess(kProcessInfo.ID);
		tPediaSearchStrings[tostring(button)] = L(kProcessInfo.Description);
		SetCommonButtonProperties(button, true, kProcessInfo.PortraitIndex, kProcessInfo.IconAtlas, strTooltip, strButtonText);
	end

	--- @param button Control
	--- @param kBuildInfo Info
	--- @param strCustomTooltip string?
	--- @param strButtonText string?
	local function SetupBuildButton(button, kBuildInfo, strCustomTooltip, strButtonText)
		local strTooltip;
		if kBuildInfo.RouteType then
			strTooltip = strCustomTooltip or L(kBuildInfo.Description);
			tPediaSearchStrings[tostring(button)] = L(GameInfo.Routes[kBuildInfo.RouteType].Description);
		elseif kBuildInfo.ImprovementType then
			local kImprovementInfo = GetInfoFromType("Improvements", kBuildInfo.ImprovementType);
			strTooltip = strCustomTooltip or GetHelpTextForImprovement(kImprovementInfo.ID, false, false);
			tPediaSearchStrings[tostring(button)] = L(kImprovementInfo.Description);
		else -- we are a choppy thing
			strTooltip = strCustomTooltip or L(kBuildInfo.Description);
			tPediaSearchStrings[tostring(button)] = L(GameInfo.Concepts["CONCEPT_WORKERS_CLEARINGLAND"].Description);
		end
		SetCommonButtonProperties(button, true, kBuildInfo.IconIndex, kBuildInfo.IconAtlas, strTooltip, strButtonText);
	end

	--- Unlike the other setup functions, a tooltip must be provided here<br>
	--- bIconIndex must be true if kInfo is a mission (has IconIndex column instead of PortraitIndex)
	--- @param button table
	--- @param kInfo table
	--- @param strCustomTooltip string
	--- @param strButtonText string?
	--- @param bIconIndex boolean?
	local function SetupGenericButton(button, kInfo, strCustomTooltip, strButtonText, bIconIndex)
		local iIconIndex;
		if bIconIndex then
			iIconIndex = kInfo.IconIndex;
		else
			iIconIndex = kInfo.PortraitIndex;
		end
		tPediaSearchStrings[tostring(button)] = L(kInfo.Description);
		SetCommonButtonProperties(button, true, iIconIndex, kInfo.IconAtlas, strCustomTooltip, strButtonText);
	end

	-- First, hide all buttons by default
	for iButtonIndex = 1, iButtonCount do
		local strButtonName = "B" .. tostring(iButtonIndex);
		buttonStack[strButtonName]:SetHide(true);
		buttonStack[strButtonName]:SetAlpha(1);
	end

	local iButtonIndex = 1;

	--- @param SetupFunc fun(button: Control, kInfo: Info, strCustomTooltip: string?, strButtonText: string?, bIconIndex: boolean?)
	--- @param kInfo Info
	--- @param strCustomTooltip string?
	--- @param strButtonText string?
	--- @param bIconIndex boolean?
	local function GenerateNextButtonFromInfo(SetupFunc, kInfo, strCustomTooltip, strButtonText, bIconIndex)
		local strButtonName = "B" .. iButtonIndex;
		local button = buttonStack[strButtonName];
		if button then
			SetupFunc(button, kInfo, strCustomTooltip, strButtonText, bIconIndex);
			iButtonIndex = iButtonIndex + 1;
		end
	end

	--- @param strTooltip string
	--- @param strAtlas string?
	--- @param iIconIndex integer?
	--- @param strButtonText string?
	local function GenerateNextButtonCustom(strTooltip, strAtlas, iIconIndex, strButtonText)
		local strButtonName = "B" .. iButtonIndex;
		local button = buttonStack[strButtonName];
		strAtlas = strAtlas or "GENERIC_FUNC_ATLAS";
		iIconIndex = iIconIndex or 0;
		if button then
			SetCommonButtonProperties(button, false, iIconIndex, strAtlas, strTooltip, strButtonText);
			iButtonIndex = iButtonIndex + 1;
		end
	end

	for kUnitInfo in GameInfo.Units{PreReqTech = kTechInfo.Type} do
		if pPlayer and not kUnitInfo.MinorCivGift and pPlayer:GetSpecificUnitType(kUnitInfo.Class, true) == kUnitInfo.ID then
			GenerateNextButtonFromInfo(SetupUnitButton, kUnitInfo);
			if iButtonIndex > iButtonCount then return iButtonCount end
		end
	end

	for kBuildingInfo in GameInfo.Buildings{PreReqTech = kTechInfo.Type} do
		if kBuildingInfo.ShowInPedia and CanPlayerEverBuildCached(kBuildingInfo.ID) then
			GenerateNextButtonFromInfo(SetupBuildingButton, kBuildingInfo);
			if iButtonIndex > iButtonCount then return iButtonCount end
		end
	end

	for kResourceInfo in GameInfo.Resources{TechReveal = kTechInfo.Type} do
		-- Does this player have an alternate unlock tech on this resource?
		local bAltTech = false;
		if pPlayer then
			for row in GameInfo.Trait_AlternateResourceTechs{ResourceType = kResourceInfo.Type} do
				for _, eCheckCiv in pairs(GetCivsFromTrait(row.TraitType)) do
					if eCiv == eCheckCiv then
						bAltTech = true;
						break;
					end
				end
				if bAltTech then
					break;
				end
			end
		end

		if not bAltTech then
			GenerateNextButtonFromInfo(SetupResourceButton, kResourceInfo);
			if iButtonIndex > iButtonCount then return iButtonCount end
		end
	end

	-- Is this tech an alternate unlock tech on a resource?
	if pPlayer then
		for row in GameInfo.Trait_AlternateResourceTechs{TechReveal = kTechInfo.Type} do
			for _, eCheckCiv in pairs(GetCivsFromTrait(row.TraitType)) do
				if eCiv == eCheckCiv then
					GenerateNextButtonFromInfo(SetupResourceButton, GetInfoFromType("Resources", row.ResourceType));
					if iButtonIndex > iButtonCount then return iButtonCount end
					break;
				end
			end
		end
	end

	for kProjectInfo in GameInfo.Projects{TechPrereq = kTechInfo.Type} do
		GenerateNextButtonFromInfo(SetupProjectButton, kProjectInfo);
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	for kBuildInfo in GameInfo.Builds{PrereqTech = kTechInfo.Type, ShowInTechTree = 1} do
		if not kBuildInfo.ImprovementType or CanPlayerEverBuildImprovementCached(kBuildInfo.ImprovementType) then
			GenerateNextButtonFromInfo(SetupBuildButton, kBuildInfo);
			if iButtonIndex > iButtonCount then return iButtonCount end
		end
	end

	for kProcessInfo in GameInfo.Processes{TechPrereq = kTechInfo.Type} do
		GenerateNextButtonFromInfo(SetupProcessButton, kProcessInfo);
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.CorporationsEnabled then
		GenerateNextButtonCustom(L("TXT_KEY_ABLTY_ENABLES_CORPORATIONS"), "UNIT_ACTION_BUY_CITYSTATE", 0);
		if iButtonIndex > iButtonCount then return iButtonCount end

		for _, kCorporationInfo in GameInfoCache("Corporations") do
			-- Get the headquarters building and check if this player can build it
			local eHeadquarters = pPlayer:GetCivilizationBuilding(GameInfoTypes[kCorporationInfo.HeadquartersBuildingClass]);
			if eHeadquarters ~= -1 then
				local strRequiredCivType = GetInfoFromId("Buildings", eHeadquarters).CivilizationRequired;
				if not strRequiredCivType or strRequiredCivType == strCivType then
					GenerateNextButtonFromInfo(SetupCorporationButton, kCorporationInfo);
					if iButtonIndex > iButtonCount then return iButtonCount end
				end
			end
		end
	end

	for row in GameInfo.Route_TechMovementChanges{TechType = kTechInfo.Type} do
		local strTooltip = L("TXT_KEY_TECH_HELP_FASTER_ROUTE_MOVEMENT", row.MovementChange / GameDefines.MOVE_DENOMINATOR, GameInfo.Routes[row.RouteType].Description);
		if row.RouteType == "ROUTE_ROAD" then
			GenerateNextButtonCustom(strTooltip, "UNIT_ACTION_GOLD_ATLAS", 24);
		elseif row.RouteType == "ROUTE_RAILROAD" then
			GenerateNextButtonCustom(strTooltip, "UNIT_ACTION_GOLD_ATLAS", 25);
		else
			GenerateNextButtonCustom(strTooltip);
		end
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	for row in GameInfo.Build_TechTimeChanges{TechType = kTechInfo.Type} do
		local kBuildInfo = GameInfo.Builds[row.BuildType];
		if kBuildInfo.Time and kBuildInfo.Time > 0 then
			local strTooltip = L("TXT_KEY_TECH_HELP_BUILD_COST_REDUCTION", row.TimeChange * 100 / kBuildInfo.Time, kBuildInfo.Description);
			if row.BuildType == "BUILD_REMOVE_JUNGLE" or row.BuildType == "BUILD_REMOVE_FOREST" then
				GenerateNextButtonCustom(strTooltip, "UNIT_ACTION_GOLD_ATLAS", 31);
			elseif row.BuildType == "BUILD_REMOVE_MARSH" then
				GenerateNextButtonCustom(strTooltip, "UNIT_ACTION_GOLD_ATLAS", 38);
			else
				GenerateNextButtonCustom(strTooltip, kBuildInfo.IconAtlas, kBuildInfo.IconIndex);
			end
			if iButtonIndex > iButtonCount then return iButtonCount end
		end
	end

	if kTechInfo.FeatureProductionModifier ~= 0 then
		local iAdjChopYield = math.floor(iBaseChopYield * kTechInfo.FeatureProductionModifier / 100 * iBuildPercent / 100);
		GenerateNextButtonCustom(GetSignedTooltip("TXT_KEY_TECH_HELP_CHOP_BOOST", iAdjChopYield), "UNIT_ACTION_GOLD_ATLAS", 31, "[ICON_PRODUCTION]");
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.EmbarkedMoveChange ~= 0 then
		GenerateNextButtonCustom(GetSignedTooltip("TXT_KEY_TECH_HELP_FASTER_EMBARKED_MOVEMENT", kTechInfo.EmbarkedMoveChange), "UNIT_ACTION_GOLD_ATLAS", 14);
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.CityLessEmbarkCost then
		GenerateNextButtonCustom(L("TXT_KEY_TECH_HELP_REDUCED_EMBARK_COST"), "UNIT_ACTION_GOLD_ATLAS", 14);
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.CityNoEmbarkCost then
		GenerateNextButtonCustom(L("TXT_KEY_TECH_HELP_NO_EMBARK_COST"), "UNIT_ACTION_GOLD_ATLAS", 14);
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.AllowsEmbarking then
		GenerateNextButtonFromInfo(SetupGenericButton, GetInfoFromType("Missions", "MISSION_EMBARK"), L("TXT_KEY_ALLOWS_EMBARKING"), nil, true);
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.AllowsDefensiveEmbarking then
		GenerateNextButtonFromInfo(SetupGenericButton, GetInfoFromType("Missions", "MISSION_EMBARK"), L("TXT_KEY_ABLTY_DEFENSIVE_EMBARK_STRING"), nil, true);
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.EmbarkedAllWaterPassage then
		GenerateNextButtonCustom(L("TXT_KEY_ALLOWS_CROSSING_OCEANS"));
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.AllowEmbassyTradingAllowed then
		GenerateNextButtonCustom(L("TXT_KEY_ALLOWS_EMBASSY"));
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.OpenBordersTradingAllowed then
		GenerateNextButtonCustom(L("TXT_KEY_ALLOWS_OPEN_BORDERS"));
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.DefensivePactTradingAllowed then
		GenerateNextButtonCustom(L("TXT_KEY_ALLOWS_DEFENSIVE_PACTS"));
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.MapTrading then
		GenerateNextButtonCustom(L("TXT_KEY_ALLOWS_MAP_TRADING"));
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.TechTrading and not bNoTechTrading then
		GenerateNextButtonCustom(L("TXT_KEY_ALLOWS_TECH_TRADING"));
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.ResearchAgreementTradingAllowed and bResearchAgreements then
		GenerateNextButtonCustom(L("TXT_KEY_ALLOWS_RESEARCH_AGREEMENTS"));
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.VassalageTradingAllowed and not bNoVassalage then
		GenerateNextButtonCustom(L("TXT_KEY_ALLOWS_VASSALAGE"));
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.BridgeBuilding then
		GenerateNextButtonCustom(L("TXT_KEY_ALLOWS_BRIDGES"));
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.AllowsWorldCongress then
		GenerateNextButtonCustom(L("TXT_KEY_ALLOWS_WORLD_CONGRESS"));
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.MapVisible then
		GenerateNextButtonCustom(L("TXT_KEY_REVEALS_ENTIRE_MAP"));
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.Happiness ~= 0 then
		GenerateNextButtonCustom(GetSignedTooltip("TXT_KEY_TECH_HELP_HAPPINESS_GLOBAL", kTechInfo.Happiness));
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if MOD_EVENTS_CITY_BOMBARD then
		if kTechInfo.BombardRange > 0 then
			GenerateNextButtonFromInfo(SetupGenericButton, GetInfoFromType("Missions", "MISSION_RANGE_ATTACK"), L("TXT_KEY_ABLTY_CITY_RANGE_INCREASE"), nil, true);
			if iButtonIndex > iButtonCount then return iButtonCount end
		end

		if kTechInfo.BombardIndirect > 0 then
			GenerateNextButtonFromInfo(SetupGenericButton, GetInfoFromType("Missions", "MISSION_RANGE_ATTACK"), L("TXT_KEY_ABLTY_CITY_INDIRECT_INCREASE"), nil, true);
			if iButtonIndex > iButtonCount then return iButtonCount end
		end
	end

	if kTechInfo.UnitFortificationModifier ~= 0 then
		GenerateNextButtonCustom(GetSignedTooltip("TXT_KEY_TECH_HELP_FORTIFICATION_MODIFIER", kTechInfo.UnitFortificationModifier));
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.UnitBaseHealModifier ~= 0 then
		GenerateNextButtonCustom(GetSignedTooltip("TXT_KEY_TECH_HELP_HEAL_MODIFIER", kTechInfo.UnitBaseHealModifier));
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.InternationalTradeRoutesChange ~= 0 then
		local strTooltip = L("TXT_KEY_TECH_HELP_EXTRA_TRADE_ROUTES", kTechInfo.InternationalTradeRoutesChange);
		local strText = L("TXT_KEY_TECH_HELP_NUMBER_CHANGE", kTechInfo.InternationalTradeRoutesChange);
		GenerateNextButtonFromInfo(SetupGenericButton, GetInfoFromType("Missions", "MISSION_ESTABLISH_TRADE_ROUTE"), strTooltip, strText, true);
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.InfluenceSpreadModifier ~= 0 then
		GenerateNextButtonCustom(GetSignedTooltip("TXT_KEY_TECH_HELP_TOURISM_MODIFIER", kTechInfo.InfluenceSpreadModifier));
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	if kTechInfo.ExtraVotesPerDiplomat ~= 0 then
		GenerateNextButtonCustom(GetSignedTooltip("TXT_KEY_TECH_HELP_DELEGATES_FROM_DIPLOMATS", kTechInfo.ExtraVotesPerDiplomat));
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	for row in GameInfo.Technology_TradeRouteDomainExtraRange{TechType = kTechInfo.Type} do
		if row.Range ~= 0 then
			local eDomain = GameInfoTypes[row.DomainType];
			if eDomain == DomainTypes.DOMAIN_LAND then
				GenerateNextButtonCustom(GetSignedTooltip("TXT_KEY_TECH_HELP_LAND_TRADE_ROUTE_EXTENSION", row.Range));
				if iButtonIndex > iButtonCount then return iButtonCount end
			elseif eDomain == DomainTypes.DOMAIN_SEA then
				GenerateNextButtonCustom(GetSignedTooltip("TXT_KEY_TECH_HELP_SEA_TRADE_ROUTE_EXTENSION", row.Range));
				if iButtonIndex > iButtonCount then return iButtonCount end
			end
		end
	end

	for row in GameInfo.Technology_FreePromotions{TechType = kTechInfo.Type} do
		local kPromotionInfo = GetInfoFromType("UnitPromotions", row.PromotionType);
		local strTooltip = L("TXT_KEY_FREE_PROMOTION_FROM_TECH", kPromotionInfo.Description, kPromotionInfo.Help);
		GenerateNextButtonFromInfo(SetupGenericButton, kPromotionInfo, strTooltip);
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	-- Yield-related
	local tSpecialistBoosts = {};
	local tTerrainBoosts = {};
	local tFeatureBoosts = {};
	local tImprovementBoosts = {};
	local tImprovementFreshWaterBoosts = {};
	local tImprovementNoFreshWaterBoosts = {};
	local tBuildingBoosts = {};
	for _, kYieldInfo in GameInfoCache("Yields") do
		ExtractYieldTable(tSpecialistBoosts, "Tech_SpecialistYieldChanges", kYieldInfo, "SpecialistType");
		ExtractYieldTable(tTerrainBoosts, "Terrain_TechYieldChanges", kYieldInfo, "TerrainType");
		ExtractYieldTable(tFeatureBoosts, "Feature_TechYieldChanges", kYieldInfo, "FeatureType");
		ExtractYieldTable(tImprovementBoosts, "Improvement_TechYieldChanges", kYieldInfo, "ImprovementType");
		ExtractYieldTable(tImprovementFreshWaterBoosts, "Improvement_TechFreshWaterYieldChanges", kYieldInfo, "ImprovementType");
		ExtractYieldTable(tImprovementNoFreshWaterBoosts, "Improvement_TechNoFreshWaterYieldChanges", kYieldInfo, "ImprovementType");
		ExtractYieldTable(tBuildingBoosts, "Building_TechEnhancedYieldChanges", kYieldInfo, "BuildingType");
	end

	-- TechType column may be NULL in Building_TechEnhancedYieldChanges, in which case it takes the building's EnhancedYieldTech column
	for kBuildingInfo in GameInfo.Buildings{EnhancedYieldTech = kTechInfo.Type} do
		if not tBuildingBoosts[kBuildingInfo.ID] then
			tBuildingBoosts[kBuildingInfo.ID] = {};
			for row in GameInfo.Building_TechEnhancedYieldChanges{BuildingType = kBuildingInfo.Type} do
				if not row.TechType then
					tBuildingBoosts[kBuildingInfo.ID][GameInfoTypes[row.YieldType]] = row.Yield;
				end
			end
		end

		-- Also add TechEnhancedTourism
		tBuildingBoosts[kBuildingInfo.ID][YieldTypes.YIELD_TOURISM] = tBuildingBoosts[kBuildingInfo.ID][YieldTypes.YIELD_TOURISM] or 0;
		tBuildingBoosts[kBuildingInfo.ID][YieldTypes.YIELD_TOURISM] = tBuildingBoosts[kBuildingInfo.ID][YieldTypes.YIELD_TOURISM] + kBuildingInfo.TechEnhancedTourism;
	end

	for eSpecialist, tYieldBoosts in pairs(tSpecialistBoosts) do
		local kSpecialistInfo = GetInfoFromId("Specialists", eSpecialist);
		local strButtonText, strYieldBoosts = GetYieldBoostsString(tYieldBoosts);
		local strTooltip = L("TXT_KEY_TECH_HELP_SPECIALIST_BOOST", kSpecialistInfo.Description, strYieldBoosts);
		GenerateNextButtonFromInfo(SetupGenericButton, kSpecialistInfo, strTooltip, strButtonText);
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	for eTerrain, tYieldBoosts in pairs(tTerrainBoosts) do
		local kTerrainInfo = GetInfoFromId("Terrains", eTerrain);
		local strButtonText, strYieldBoosts = GetYieldBoostsString(tYieldBoosts);
		local strTooltip = L("TXT_KEY_TECH_HELP_TERRAIN_BOOST", kTerrainInfo.Description, strYieldBoosts);
		GenerateNextButtonFromInfo(SetupGenericButton, kTerrainInfo, strTooltip, strButtonText);
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	for eFeature, tYieldBoosts in pairs(tFeatureBoosts) do
		local kFeatureInfo = GetInfoFromId("Features", eFeature);
		local strButtonText, strYieldBoosts = GetYieldBoostsString(tYieldBoosts);
		local strTooltip = L("TXT_KEY_TECH_HELP_FEATURE_BOOST", kFeatureInfo.Description, strYieldBoosts);
		GenerateNextButtonFromInfo(SetupGenericButton, kFeatureInfo, strTooltip, strButtonText);
		if iButtonIndex > iButtonCount then return iButtonCount end
	end

	for eImprovement, tYieldBoosts in pairs(tImprovementBoosts) do
		local kImprovementInfo = GetInfoFromId("Improvements", eImprovement);
		if CanPlayerEverBuildImprovementCached(eImprovement) then
			local strButtonText, strYieldBoosts = GetYieldBoostsString(tYieldBoosts);
			local strTooltip = L("TXT_KEY_TECH_HELP_IMPROVEMENT_BOOST", kImprovementInfo.Description, strYieldBoosts);
			local kBuildInfo = GameInfo.Builds{ImprovementType = kImprovementInfo.Type}();
			GenerateNextButtonFromInfo(SetupBuildButton, kBuildInfo, strTooltip, strButtonText);
			if iButtonIndex > iButtonCount then return iButtonCount end
		end
	end

	for eImprovement, tYieldBoosts in pairs(tImprovementFreshWaterBoosts) do
		local kImprovementInfo = GetInfoFromId("Improvements", eImprovement);
		if CanPlayerEverBuildImprovementCached(eImprovement) then
			local strButtonText, strYieldBoosts = GetYieldBoostsString(tYieldBoosts);
			local strTooltip = L("TXT_KEY_TECH_HELP_IMPROVEMENT_BOOST_FRESH_WATER", kImprovementInfo.Description, strYieldBoosts);
			local kBuildInfo = GameInfo.Builds{ImprovementType = kImprovementInfo.Type}();
			GenerateNextButtonFromInfo(SetupBuildButton, kBuildInfo, strTooltip, strButtonText);
			if iButtonIndex > iButtonCount then return iButtonCount end
		end
	end

	for eImprovement, tYieldBoosts in pairs(tImprovementNoFreshWaterBoosts) do
		local kImprovementInfo = GetInfoFromId("Improvements", eImprovement);
		if CanPlayerEverBuildImprovementCached(eImprovement) then
			local strButtonText, strYieldBoosts = GetYieldBoostsString(tYieldBoosts);
			local strTooltip = L("TXT_KEY_TECH_HELP_IMPROVEMENT_BOOST_NO_FRESH_WATER", kImprovementInfo.Description, strYieldBoosts);
			local kBuildInfo = GameInfo.Builds{ImprovementType = kImprovementInfo.Type}();
			GenerateNextButtonFromInfo(SetupBuildButton, kBuildInfo, strTooltip, strButtonText);
			if iButtonIndex > iButtonCount then return iButtonCount end
		end
	end

	for eBuilding, tYieldBoosts in pairs(tBuildingBoosts) do
		if CanPlayerEverBuildCached(eBuilding) then
			local kBuildingInfo = GetInfoFromId("Buildings", eBuilding);
			local strButtonText, strYieldBoosts = GetYieldBoostsString(tYieldBoosts);
			local strTooltip = L("TXT_KEY_TECH_HELP_BUILDING_BOOST", kBuildingInfo.Description, strYieldBoosts);
			GenerateNextButtonFromInfo(SetupGenericButton, kBuildingInfo, strTooltip, strButtonText);
			if iButtonIndex > iButtonCount then return iButtonCount end
		end
	end

	return iButtonIndex;
end

--- @param buttonStack Instance
--- @param iButtonCount integer
--- @param nVoid1 number
--- @param nVoid2 number
--- @param eMouse MouseType
--- @param CallbackFunc function
function AddCallbackToSmallButtons(buttonStack, iButtonCount, nVoid1, nVoid2, eMouse, CallbackFunc)
	for buttonNum = 1, iButtonCount do
		local strButtonName = "B" .. tostring(buttonNum);
		buttonStack[strButtonName]:SetVoids(nVoid1, nVoid2);
		buttonStack[strButtonName]:RegisterCallback(eMouse, CallbackFunc);
	end
end

function GatherInfoAboutUniqueStuff( civType )

	validUnitBuilds = {};
	validBuildingBuilds = {};
	validImprovementBuilds = {};

	-- put in the default units for any civ
	for thisUnitClass in GameInfo.UnitClasses() do
		validUnitBuilds[thisUnitClass.Type]	= thisUnitClass.DefaultUnit;	
	end

	-- put in my overrides
	for thisOverride in GameInfo.Civilization_UnitClassOverrides() do
 		if thisOverride.CivilizationType == civType then
			validUnitBuilds[thisOverride.UnitClassType]	= thisOverride.UnitType;
 		end
	end

	-- put in the default buildings for any civ
	for thisBuildingClass in GameInfo.BuildingClasses() do
		validBuildingBuilds[thisBuildingClass.Type]	= thisBuildingClass.DefaultBuilding;	
	end

	-- put in my overrides
	for thisOverride in GameInfo.Civilization_BuildingClassOverrides() do
 		if thisOverride.CivilizationType == civType then
			validBuildingBuilds[thisOverride.BuildingClassType]	= thisOverride.BuildingType;	
 		end
	end
	
	-- add in support for unique improvements
	for thisImprovement in GameInfo.Improvements() do
		if thisImprovement.CivilizationType == civType or thisImprovement.CivilizationType == nil then
			validImprovementBuilds[thisImprovement.Type] = thisImprovement.Type;	
		else
			validImprovementBuilds[thisImprovement.Type] = nil;	
		end
	end
	
end