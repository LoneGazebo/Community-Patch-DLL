print("This is the modded EspionageOverview from CBP")
-------------------------------------------------
-- Espionage Overview Popup
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
include( "CityStateStatusHelper" );
include("FLuaVector.lua")
include("InfoTooltipInclude");
include("TradeLogic");

local g_AgentManager = GenerationalInstanceManager:new( "AgentInstance", "Base", Controls.AgentStack);
local g_MyCityManager = GenerationalInstanceManager:new( "MyCityInstance", "Base", Controls.MyCityStack);
local g_UnitListManager = GenerationalInstanceManager:new( "UnitListInstance", "Base", Controls.UnitListStack);
local g_MyCityButtonManager = GenerationalInstanceManager:new("MyCityButtonInstance", "Base", Controls.MyCityStack);
local g_TheirCityManager = GenerationalInstanceManager:new( "TheirCityInstance", "Base", Controls.TheirCityStack);
local g_TheirCityButtonManager = GenerationalInstanceManager:new( "TheirCityButtonInstance", "Base", Controls.TheirCityStack);
local g_IntrigueManager = GenerationalInstanceManager:new( "IntrigueMessageInstance", "Base", Controls.IntrigueMessageStack);
local g_MissionSelectionManager = GenerationalInstanceManager:new( "MissionSelectionInstance", "Base", Controls.MissionSelectionStack);
local g_PassiveBonusesManager = GenerationalInstanceManager:new( "PassiveBonusesInstance", "Base", Controls.PassiveBonusesStack);
local g_DiplomatBonusesManager = GenerationalInstanceManager:new( "DiplomatBonusesInstance", "Base", Controls.DiplomatBonusesStack);
local g_CounterspyFocusManager = GenerationalInstanceManager:new( "CounterspyFocusInstance", "Base", Controls.CounterspyFocusStack);

local m_Deal = UI.GetScratchDeal();

-- Mission Selection
SelectedItems = {};

local g_PreviousLocationBoxColor = "231,213,0,255";

-- Used for Piano Keys
local g_PianoKeys = {
	[0] = {19/255,32/255,46/255,120/255},
	[1] = {12/255,22/255,30/255,120/255},
}

-- Progressbar state information based on agent activity.
local g_ProgressBarStates = {
	TXT_KEY_SPY_STATE_TRAVELLING = {
		IconOffset = {x = 45,y = 45},
		ProgressBarTexture = "MeterBarGreatEspionageGreen.dds",
	},
	TXT_KEY_SPY_STATE_GATHERING_INTEL = {
		IconOffset = {x = 45, y = 0},
		ProgressBarTexture = "MeterBarGreatEspionageBlue.dds",
	},
	TXT_KEY_SPY_STATE_COUNTER_INTEL = {
		IconOffset = {x = 45,y = 0},
		ProgressBarTexture = "MeterBarGreatEspionageBlue.dds",
	},
	TXT_KEY_SPY_STATE_SCHMOOZING = {
		IconOffset = {x = 45,y = 0},
		ProgressBarTexture = "MeterBarGreatEspionageBlue.dds",
	},
	TXT_KEY_SPY_STATE_RIGGING_ELECTION = {
		IconOffset = {x = 45, y = 90},
		ProgressBarTexture = "MeterBarGreatPersonGold.dds",
	},
}

-- Agent text color based on agent activity.
local g_TextColors = {
	TXT_KEY_SPY_STATE_TRAVELLING           = {x =  94/255, y = 237/255, z = 105/255, w = 255/255},
	TXT_KEY_SPY_STATE_GATHERING_INTEL      = {x = 128/255, y = 150/255, z = 228/255, w = 255/255},
	TXT_KEY_SPY_STATE_COUNTER_INTEL		   = {x = 128/255, y = 150/255, z = 228/255, w = 255/255},
	TXT_KEY_SPY_STATE_SCHMOOZING		   = {x = 128/255, y = 150/255, z = 228/255, w = 255/255},
	TXT_KEY_SPY_STATE_RIGGING_ELECTION	   = {x = 255/255, y = 222/255, z =   9/255, w = 255/255},
}

g_Tabs = {
	["Overview"] = {
		Panel = Controls.OverviewPanel,
		SelectHighlight = Controls.OverviewSelectHighlight,
	},

	["Intrigue"] = {
		Panel = Controls.IntriguePanel,
		SelectHighlight = Controls.IntrigueSelectHighlight,
	},
}

g_AgentsSortOptions = {
	{
		Button = Controls.ASortByNameRank,
		ImageControl = Controls.ASortByNameRankImage,
		Column = "AgentName",
		DefaultDirection = "asc",
		CurrentDirection = "asc",
	},
	{
		Button = Controls.ASortByLocation,
		ImageControl = Controls.ASortByLocationImage,
		Column = "AgentLocation",
		DefaultDirection = "asc",
		CurrentDirection = nil,
	},
	{
		Button = Controls.ASortByActivity,
		ImageControl = Controls.ASortByActivityImage,
		Column = "AgentActivity",
		DefaultDirection = "asc",
		CurrentDirection = nil,
	},
};

g_UnitListSortOptions = {
	{
		Button = Controls.ULSortByName,
		ImageControl = Controls.ULSortByNameImage,
		Column = "Name",
		DefaultDirection = "asc",
		CurrentDirection = nil,
	},
	{
		Button = Controls.ULSortByStrength,
		ImageControl = Controls.ULSortByStrengthImage,
		Column = "Strength",
		SortType = "numeric",
		DefaultDirection = "desc",
		CurrentDirection = nil,
	},
	{
		Button = Controls.ULSortByRangedStrength,
		ImageControl = Controls.ULSortByRangedStrengthImage,
		Column = "RangedStrength",
		SortType = "numeric",
		DefaultDirection = "desc",
		CurrentDirection = nil,
	},
	{
		Button = Controls.ULSortByCount,
		ImageControl = Controls.ULSortByCountImage,
		Column = "Count",
		SortType = "numeric",
		DefaultDirection = "desc",
		CurrentDirection = "desc",
	},
};

g_YourCitiesSortOptions = {
	{
		Button = Controls.YCSortByCivilization,
		ImageControl = Controls.YCSortByCivilizationImage,
		Column = "Civilization",
		DefaultDirection = "asc",
		CurrentDirection = nil,
	},
	{
		Button = Controls.YCSortByName,
		ImageControl = Controls.YCSortByNameImage,
		Column = "Name",
		DefaultDirection = "asc",
		CurrentDirection = "asc",
	},
	{
		Button = Controls.YCSortByPotential,
		ImageControl = Controls.YCSortByPotentialImage,
		Column = "Potential",
		SortType = "numeric",
		DefaultDirection = "desc",
		CurrentDirection = nil,
	},
	{
		Button = Controls.YCSortByPopulation,
		ImageControl = Controls.YCSortByPopulationImage,
		Column = "Population",
		DefaultDirection = "desc",
		SortType = "numeric",
		CurrentDirection = nil,
	},
};

g_TheirCitiesSortOptions = {
	{
		Button = Controls.OCSortByCivilization,
		ImageControl = Controls.OCSortByCivilizationImage,
		Column = "CivilizationName",
		DefaultDirection = "asc",
		CurrentDirection = "asc",
	},
	{
		Button = Controls.OCSortByName,
		ImageControl = Controls.OCSortByNameImage,
		Column = "Name",
		DefaultDirection = "asc",
		CurrentDirection = nil,
	},
	{
		Button = Controls.OCSortByPotential,
		ImageControl = Controls.OCSortByPotentialImage,
		Column = "Potential",
		DefaultDirection = "desc",
		CurrentDirection = nil,
		SortType = "numeric",
	},
	{
		Button = Controls.OtherCitiesSortByPopulation,
		ImageControl = Controls.OCSortByPopulationImage,
		Column = "Population",
		DefaultDirection = "desc",
		SortType = "numeric",
		CurrentDirection = nil,
	},
};

g_IntrigueSortOptions = {
	{
		Button = Controls.IntrigueSortByTurn,
		ImageControl = Controls.IntrigueSortByTurnImg,
		Column = "Turn",
		DefaultDirection = "desc",
		CurrentDirection = "desc",
		SortType = "numeric",
	},
	{
		Button = Controls.IntrigueSortByFrom,
		ImageControl = Controls.IntrigueSortByFromImg,
		Column = "From",
		DefaultDirection = "asc",
		CurrentDirection = nil,
	},
	{
		Button = Controls.IntrigueSortByMsg,
		ImageControl = Controls.IntrigueSortByMsgImg,
		Column = "Message",
		DefaultDirection = "asc",
		CurrentDirection = nil,
	},
};
_AgentsSortFunction = nil;
g_YourCitiesSortFunction = nil;
g_UnitListSortFunction = nil;
g_TheirCitiesSortFunction = nil;
g_IntrigueSortFunction = nil;

g_CurrentTab = nil;		-- The currently selected Tab.

-- slewis - added this so that we can sort while an agent is able to move. They are reset in refresh
g_SelectedAgentID = nil;
g_SelectedCity = nil;
g_CitiesAvailableToRelocate = nil;
g_UnitList = nil;
-- end slewis

function GetNetworkPointsScaled(info)
	local iNPScaled = info.NetworkPointsNeeded;
	if (info.NetworkPointsScaling == true) then
		iNPScaled = math.floor(iNPScaled * GameInfo.GameSpeeds[Game.GetGameSpeedType()].TrainPercent / 100);
	end
	return iNPScaled;
end
-------------------------------------------------
-------------------------------------------------
function OnPopupMessage(popupInfo)

	if (not Game.IsOption("GAMEOPTION_NO_ESPIONAGE")) then
		local popupType = popupInfo.Type;
		if popupType ~= ButtonPopupTypes.BUTTONPOPUP_ESPIONAGE_OVERVIEW then
			return;
		end

		g_PopupInfo = popupInfo;

		if( g_PopupInfo.Data1 == 1 ) then
    		if( ContextPtr:IsHidden() == false ) then
    			OnClose();
			else
        		UIManager:QueuePopup( ContextPtr, PopupPriority.InGameUtmost );
    		end
		else
			UIManager:QueuePopup( ContextPtr, PopupPriority.SocialPolicy );
		end
    end

end
Events.SerialEventGameMessagePopup.Add( OnPopupMessage );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnClose()
	UIManager:DequeuePopup(ContextPtr);
end
Controls.CloseButton:RegisterCallback(Mouse.eLClick, OnClose);


function IgnoreLeftClick( Id )
	-- just swallow it
end
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    ----------------------------------------------------------------
    -- Key Down Processing
    ----------------------------------------------------------------
    if uiMsg == KeyEvents.KeyDown then
        if (wParam == Keys.VK_ESCAPE) then
			if(not Controls.ChooseConfirm:IsHidden()) then
				Controls.ChooseConfirm:SetHide(true);
			elseif (not Controls.MissionSelectionPopup:IsHidden()) then
				Controls.MissionSelectionPopup:SetHide(true);
			elseif (not Controls.DiplomatTradeDealsPopup:IsHidden()) then
				Controls.DiplomatTradeDealsPopup:SetHide(true);
			elseif (not Controls.UnitListPopup:IsHidden()) then
				Controls.UnitListPopup:SetHide(true);
			elseif (not Controls.DiplomatPopup:IsHidden()) then
				Controls.DiplomatPopup:SetHide(true);
			elseif (not Controls.CounterspyFocusPopup:IsHidden()) then
				Controls.CounterspyFocusPopup:SetHide(true);
			elseif (not Controls.NotificationPopup:IsHidden()) then
				Controls.NotificationPopup:SetHide(true);
			else
				OnClose();
           	end
			return true;
        end

        -- Do Nothing.
        if(wParam == Keys.VK_RETURN) then
			return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

local g_RenaissanceEraType;
local renaissanceEra = GameInfo.Eras["ERA_RENAISSANCE"];
if(renaissanceEra ~= nil) then
	g_RenaissanceEraType = renaissanceEra.ID;
end

function ShowHideHandler( bIsHide, bInitState )
    if( not bInitState ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();

			if(g_RenaissanceEraType ~= nil) then
				local activePlayer = Players[Game.GetActivePlayer()];
				local bUseModern = activePlayer:GetCurrentEra() > g_RenaissanceEraType;
				Controls.ModernBackground:SetHide(not bUseModern);
				Controls.EarlyBackground:SetHide(bUseModern);
			end

        	Events.SerialEventGameMessagePopupShown(g_PopupInfo);
        	TabSelect(g_CurrentTab);
        else
			if(g_PopupInfo ~= nil) then
				Events.SerialEventGameMessagePopupProcessed.CallImmediate(g_PopupInfo.Type, 0);
			end
            UI.decTurnTimerSemaphore();
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

function TabSelect(tab)
	for i,v in pairs(g_Tabs) do
		local bHide = i ~= tab;
		v.Panel:SetHide(bHide);
		v.SelectHighlight:SetHide(bHide);
	end
	g_CurrentTab = tab;
	Refresh();
end
Controls.TabButtonOverview:RegisterCallback( Mouse.eLClick, function() TabSelect("Overview"); end);
Controls.TabButtonIntrigue:RegisterCallback( Mouse.eLClick, function() TabSelect("Intrigue"); end );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
Controls.CancelRelocationButton:RegisterCallback(Mouse.eLClick, function() Refresh(); end);
function RelocateAgent(agentID, city)
	local activePlayer = Players[Game.GetActivePlayer()];
	local availableCitiesToRelocate = activePlayer:GetAvailableSpyRelocationCities(agentID);

	g_SelectedAgentID = agentID;
	g_SelectedCity = city;
	g_CitiesAvailableToRelocate = availableCitiesToRelocate;

	RefreshMyCities(g_SelectedAgentID, g_SelectedCity and g_SelectedCity:GetOwner() or nil, g_SelectedCity and g_SelectedCity:GetID() or nil, g_CitiesAvailableToRelocate);
	RefreshTheirCities(g_SelectedAgentID, g_SelectedCity and g_SelectedCity:GetOwner() or nil, g_SelectedCity and g_SelectedCity:GetID() or nil, g_CitiesAvailableToRelocate);

	Controls.UnassignButton:RegisterCallback(Mouse.eLClick, function()
		g_ConfirmAction = function()
			Network.SendMoveSpy(Game.GetActivePlayer(), g_SelectedAgentID, -1, -1, false);
			Refresh();
		end
		g_DenyAction = nil;

		local agent = nil;
		local agents = activePlayer:GetEspionageSpies();
		for i,v in ipairs(agents) do
			if(v.AgentID == g_SelectedAgentID) then
				agent = v;
			end
		end

		Controls.ConfirmText:LocalizeAndSetText("TXT_KEY_EO_MOVE_SPY_TO_HIDEOUT_CHECK", agent.Rank, agent.Name);
		Controls.ConfirmContent:CalculateSize();
		local width, height = Controls.ConfirmContent:GetSizeVal();
		Controls.ConfirmFrame:SetSizeVal(width + 60, height + 120);
		Controls.ChooseConfirm:SetHide(false);
		Controls.YesString:LocalizeAndSetText("TXT_KEY_YES_BUTTON");
		Controls.NoString:LocalizeAndSetText("TXT_KEY_NO_BUTTON");
	end);


	local agent = nil;
	local agents = activePlayer:GetEspionageSpies();

	for i,v in ipairs(agents) do
		if(v.AgentID == g_SelectedAgentID) then
			agent = v;
		end
	end

	local map = Map;
	local progressBarStates = g_ProgressBarStates;


	if (agent.State == "TXT_KEY_SPY_STATE_SURVEILLANCE" and agent.TurnsLeft < 0) then
		agent.AgentActivity = Locale.Lookup("TXT_KEY_SPY_STATE_ESTABLISHED_SURVEILLANCE");
	else
		agent.AgentActivity = Locale.Lookup(agent.State);
	end

	Controls.AgentName:LocalizeAndSetText(agent.Name);
	Controls.AgentName:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_NAMEPLATE_TT", agent.Rank, agent.Name);

	Controls.AgentLocationActionsPanel:SetHide(false);
	Controls.AgentActivityPanel:SetHide(false);

	Controls.AgentLocation:LocalizeAndSetText("TXT_KEY_SPY_LOCATION_UNASSIGNED");
	Controls.AgentLocation:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_NEEDS_ASSIGNMENT_TT");

	Controls.AgentLocationIcon:SetHide(true);
	Controls.AgentLocationIconBG:SetHide(true);
	Controls.AgentLocationIconShadow:SetHide(true);
	Controls.CivIconHighlight:SetHide(true);

	local plot = map.GetPlot(agent.CityX, agent.CityY);
	local city = nil;
	if(plot ~= nil) then
		city = plot:GetPlotCity();
		if(city ~= nil) then

			Controls.AgentLocationIcon:SetHide(false);
			Controls.AgentLocationIconBG:SetHide(false);
			Controls.AgentLocationIconShadow:SetHide(false);
			Controls.CivIconHighlight:SetHide(false);

			CivIconHookup(city:GetOwner(), 32, Controls.AgentLocationIcon, Controls.AgentLocationIconBG, Controls.AgentLocationIconShadow, false, true, Controls.CivIconHighlight);

			Controls.AgentLocation:LocalizeAndSetText(city:GetName());

			Controls.AgentLocationIcon:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_LOCATION_TT", agent.Rank, agent.Name, city:GetName());
			Controls.AgentLocation:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_LOCATION_TT", agent.Rank, agent.Name, city:GetName());
		end
	end

	local bHideActivityInfo = progressBarStates[agent.State] == nil;

	Controls.AgentActivity:SetText(agent.AgentActivity);

	local strActivityTT = Locale.Lookup("TXT_KEY_EO_SPY_NEEDS_ASSIGNMENT_TT");
	local city = nil;
	if(plot ~= nil) then
		city = plot:GetPlotCity();
		if(city ~= nil) then
			strActivityTT = activePlayer:GetSpyMissionTooltip(city, agent.AgentID);
		end
	end

	Controls.AgentActivity:SetToolTipString(strActivityTT);
	Controls.AgentActivityProgressFrame:SetHide(bHideActivityInfo);
	Controls.AgentActivityProgressFrame:SetToolTipString(strActivityTT);
	Controls.AgentActivityProgress:SetHide(bHideActivityInfo);
	Controls.AgentActivityProgress:SetToolTipString(strActivityTT);
	Controls.AgentActivityProgressBack:SetHide(bHideActivityInfo);
	Controls.AgentActivityProgressBack:SetToolTipString(strActivityTT);
	Controls.AgentActivityIcon:SetHide(bHideActivityInfo);
	Controls.AgentActivityIcon:SetToolTipString(strActivityTT);

	Controls.AgentActivityPanel:SetHide(bHideActivityInfo);
	if(not bHideActivityInfo) then

		local progressBarState = progressBarStates[agent.State];
		Controls.AgentActivityIcon:SetTextureOffset(progressBarState.IconOffset);

		if (progressBarState.ProgressBarTexture) then
			Controls.AgentActivityProgress:SetHide(false);
			Controls.AgentActivityProgressBack:SetHide(false);
			local progress = agent.PercentComplete;
			if (progress < 0) then
				progress = 0;
			end
			Controls.AgentActivityProgress:SetTexture(progressBarState.ProgressBarTexture);
			local progresspct = progress/100;
			local nextprogresspct = 1;
			if (agent.TurnsLeft > 0) then
				nextprogresspct = progresspct + (1 - progresspct)/agent.TurnsLeft;
			end
			Controls.AgentActivityProgress:SetPercents( progresspct, nextprogresspct );
		else
			Controls.AgentActivityProgress:SetHide(true);
			Controls.AgentActivityProgressFrame:SetHide(true);
			Controls.AgentActivityProgressBack:SetHide(true);
		end
	
		if (agent.TurnsLeft >= 0) then
			Controls.AgentProgress:LocalizeAndSetText("TXT_KEY_STR_TURNS", agent.TurnsLeft);
		elseif (agent.SpyFocus >= 0) then
			local pMissionInfo = GameInfo.CityEventChoices[agent.SpyFocus];
			Controls.AgentProgress:LocalizeAndSetText(pMissionInfo.MissionTooltip);
		elseif (agent.NetworkPointsStored >= 0) then
			Controls.AgentProgress:LocalizeAndSetText("TXT_KEY_NETWORK_POINTS", agent.NetworkPointsStored, agent.NetworkPointsPerTurn);
		else
			Controls.AgentProgress:SetText("");
		end
	else
		Controls.AgentProgress:SetText("");
	end

	Controls.RelocateInstructions:LocalizeAndSetText("TXT_KEY_MOVE_SPY_INSTRUCTIONS", agent.Rank, agent.Name);
	Controls.AgentMovePanel:SetHide(false);
	Controls.AgentsListBox:SetHide(true);
	Controls.CloseButton:SetHide(true);
end

-------------------------------------------------------------------------------
function RefreshAgents()

	--print("Refreshing Agents");
	g_AgentManager:ResetInstances();

	local map = Map;
	local progressBarStates = g_ProgressBarStates;

	local bTickTock = true;
	local pActivePlayer = Players[Game.GetActivePlayer()];
	local agents = pActivePlayer:GetEspionageSpies();

	local bHasAgents = #agents ~= 0;
	Controls.NoMoreSpiesLabel:SetHide(bHasAgents);

	for i,v in ipairs(agents) do
		if (v.State == "TXT_KEY_SPY_STATE_SURVEILLANCE" and v.TurnsLeft < 0) then
			v.AgentActivity = Locale.Lookup("TXT_KEY_SPY_STATE_ESTABLISHED_SURVEILLANCE");
		else
			v.AgentActivity = Locale.Lookup(v.State);
		end

		v.AgentName = Locale.Lookup(v.Name);

		v.AgentLocation = Locale.Lookup("TXT_KEY_SPY_LOCATION_UNASSIGNED");
		local plot = map.GetPlot(v.CityX, v.CityY);
		if(plot ~= nil) then
			local city = plot:GetPlotCity();
			if(city ~= nil) then
				v.AgentLocation = Locale.Lookup(city:GetName());
			end
		end
	end

	table.sort(agents, g_AgentsSortFunction);

	for i,v in ipairs(agents) do

		local agentEntry = g_AgentManager:GetInstance();

		agentEntry.AgentName:LocalizeAndSetText(v.Name);
		agentEntry.AgentName:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_NAMEPLATE_TT", v.Rank, v.Name);

		if (v.IsDiplomat) then
			agentEntry.DiplomatIcon:SetHide(false);
			agentEntry.AgentIcon:SetHide(true);
		else
			agentEntry.AgentIcon:SetHide(false);
			agentEntry.DiplomatIcon:SetHide(true);
		end


		if(v.State == "TXT_KEY_SPY_STATE_DEAD") then
			agentEntry.AgentLocationActionsPanel:SetHide(true);
			agentEntry.AgentActivityPanel:SetHide(true);
			agentEntry.AgentKIAPanel:SetHide(false);

			agentEntry.Base:SetColorVal(0,0,0,0);
			bTickTock = not bTickTock;

			agentEntry.RelocateButton:SetHide(true);
		else
			agentEntry.AgentLocationActionsPanel:SetHide(false);
			agentEntry.AgentActivityPanel:SetHide(false);
			agentEntry.AgentKIAPanel:SetHide(true);
			
			agentEntry.RelocateButton:SetHide(false);

			agentEntry.AgentLocation:SetText(v.AgentLocation);
			agentEntry.AgentLocation:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_NEEDS_ASSIGNMENT_TT");

			agentEntry.Base:SetColorVal(unpack(bTickTock and g_PianoKeys[0] or g_PianoKeys[1]));
			bTickTock = not bTickTock;

			agentEntry.AgentLocationIcon:SetHide(true);
			agentEntry.AgentLocationIconBG:SetHide(true);
			agentEntry.AgentLocationIconShadow:SetHide(true);
			agentEntry.CivIconHighlight:SetHide(true);

			local plot = map.GetPlot(v.CityX, v.CityY);
			local city = nil;
			if(plot ~= nil) then
				city = plot:GetPlotCity();
				if(city ~= nil) then
					local cityOwner = Players[city:GetOwner()];

					agentEntry.AgentLocationIcon:SetHide(false);
					agentEntry.AgentLocationIconBG:SetHide(false);
					agentEntry.AgentLocationIconShadow:SetHide(false);
					agentEntry.CivIconHighlight:SetHide(false);

					CivIconHookup(city:GetOwner(), 32, agentEntry.AgentLocationIcon, agentEntry.AgentLocationIconBG, agentEntry.AgentLocationIconShadow, false, true, agentEntry.CivIconHighlight);

					agentEntry.AgentLocationIcon:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_LOCATION_TT", v.Rank, v.Name, city:GetName());
					agentEntry.AgentLocation:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_LOCATION_TT", v.Rank, v.Name, city:GetName());
				end
			end

			local bHideActivityInfo = progressBarStates[v.State] == nil;

			agentEntry.AgentActivity:SetText(v.AgentActivity);

			local strActivityTT = Locale.Lookup("TXT_KEY_EO_SPY_NEEDS_ASSIGNMENT_TT");
			local city = nil;
			if(plot ~= nil) then
				city = plot:GetPlotCity();
				if(city ~= nil) then
					strActivityTT = pActivePlayer:GetSpyMissionTooltip(city, v.AgentID);
				end
			end

			agentEntry.AgentActivity:SetToolTipString(strActivityTT);
			agentEntry.AgentActivityProgressFrame:SetHide(bHideActivityInfo);
			agentEntry.AgentActivityProgressFrame:SetToolTipString(strActivityTT);
			agentEntry.AgentActivityProgress:SetHide(bHideActivityInfo);
			agentEntry.AgentActivityProgress:SetToolTipString(strActivityTT);
			agentEntry.AgentActivityProgressBack:SetHide(bHideActivityInfo);
			agentEntry.AgentActivityProgressBack:SetToolTipString(strActivityTT);
			agentEntry.AgentActivityIcon:SetHide(bHideActivityInfo);
			agentEntry.AgentActivityIcon:SetToolTipString(strActivityTT);

			if(not bHideActivityInfo) then

				local progressBarState = progressBarStates[v.State];
				agentEntry.AgentActivityIcon:SetTextureOffset(progressBarState.IconOffset);

				if (progressBarState.ProgressBarTexture) then
					agentEntry.AgentActivityProgress:SetHide(false);
					agentEntry.AgentActivityProgressBack:SetHide(false);
					local progress = v.PercentComplete;
					if (progress < 0) then
						progress = 0;
					end
					agentEntry.AgentActivityProgress:SetTexture(progressBarState.ProgressBarTexture);
					local progresspct = progress/100;
					local nextprogresspct = 1;
					if(v.TurnsLeft > 0) then
						 nextprogresspct = progresspct + (1 - progresspct)/v.TurnsLeft;
					end
					agentEntry.AgentActivityProgress:SetPercents( progresspct, nextprogresspct );
				else
					agentEntry.AgentActivityProgress:SetHide(true);
					agentEntry.AgentActivityProgressFrame:SetHide(true);
					agentEntry.AgentActivityProgressBack:SetHide(true);
				end


				if (v.TurnsLeft >= 0) then
					agentEntry.AgentProgress:LocalizeAndSetText("TXT_KEY_STR_TURNS", v.TurnsLeft);
				elseif (v.SpyFocus >= 0 and GameInfo.CityEventChoices[v.SpyFocus].MissionTooltip) then
					local pMissionInfo = GameInfo.CityEventChoices[v.SpyFocus];
					agentEntry.AgentProgress:LocalizeAndSetText(pMissionInfo.MissionTooltip);
				elseif (v.NetworkPointsStored >= 0) then
					agentEntry.AgentProgress:LocalizeAndSetText("TXT_KEY_NETWORK_POINTS", v.NetworkPointsStored, v.NetworkPointsPerTurn);
				else
					agentEntry.AgentProgress:SetText("");
				end
			else
				agentEntry.AgentProgress:SetText("");
			end

			if (v.NumTurnsMovementBlocked > 0) then
				agentEntry.RelocateButton:LocalizeAndSetToolTip("TXT_KEY_EO_COUNTERSPY_CANNOT_MOVE_TT", v.Rank, v.Name,v.NumTurnsMovementBlocked);
				agentEntry.RelocateButton:SetDisabled(true);
			elseif (v.VassalDiplomatPlayer >= 0) then
				agentEntry.RelocateButton:LocalizeAndSetToolTip("TXT_KEY_EO_VASSAL_DIPLOMAT_CANNOT_MOVE_TT", v.Rank, v.Name, Players[v.VassalDiplomatPlayer]:GetCivilizationShortDescription());
				agentEntry.RelocateButton:SetDisabled(true);
			else
				agentEntry.RelocateButton:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_MOVE_TT", v.Rank, v.Name);
				agentEntry.RelocateButton:SetDisabled(false);
				agentEntry.RelocateButton:RegisterCallback(Mouse.eLClick, function()
					RelocateAgent(v.AgentID, city);
				end);
			end
			
			local OnSpyMissionSelectionClicked = function()

				if (city ~= nil) then
					if(city:GetOwner() ~= Game.GetActivePlayer()) then
					  -- spy in enemy city
						local countpassive = PopulatePassiveBonusList(Controls.PassiveBonusesStack, Game.GetActivePlayer(), city, v);
						
						local count = PopulateSelectionList(Controls.MissionSelectionStack, Game.GetActivePlayer(), city, v);
							
						if(countpassive > 0 or count > 0) then					
							Controls.ConfirmMissionSelectionButton:SetDisabled(true);
							Controls.ConfirmMissionSelectionButton:RegisterCallback(Mouse.eLClick, function()
								CommitMissionSelection(SelectedItems, Game.GetActivePlayer(), city, v.AgentID);
								Controls.MissionSelectionPopup:SetHide(true);
								RefreshAgents();
							end);
							
							Controls.MissionSelectionPopup:SetHide(false);
						end
					end
				else
					print("city == null");
				end
			end
			
			-- Initialize 'Change Counterspy Focus' button.
			local OnChangeCounterspyFocusClicked = function()

				if (city ~= nil) then
					if(city:GetOwner() == Game.GetActivePlayer()) then
						local count = PopulateSelectionList(Controls.CounterspyFocusStack, Game.GetActivePlayer(), city, v);
							
						if(count > 0) then					
							Controls.ConfirmCounterspyFocusButton:SetDisabled(true);
							Controls.ConfirmCounterspyFocusButton:RegisterCallback(Mouse.eLClick, function()
								CommitCounterspyFocus(SelectedItems, Game.GetActivePlayer(), city, v.AgentID);
								Controls.CounterspyFocusPopup:SetHide(true);
								RefreshAgents();
							end);
							Controls.CounterspyFocusContent:CalculateSize();
							local width, height = Controls.CounterspyFocusContent:GetSizeVal();
							Controls.CounterspyFocusFrame:SetSizeVal(width, height);
							Controls.CounterspyFocusPopup:SetHide(false);
						end
					end
				else
					print("city == null");
				end
			end
			
			-- Initialize Diplomat Button
			local OnDiplomatButtonClicked = function()
				if (city ~= nil) then
				  -- spy in enemy city
					local countpassive = PopulateDiplomatBonusList(Controls.DiplomatBonusesStack, Game.GetActivePlayer(), city, v);
						
					if(countpassive > 0) then				
						Controls.DiplomatPopup:SetHide(false);
					end
				else
					print("city == null");
				end
			end
	

			agentEntry.SpyMissionButton:SetHide(true);
			agentEntry.CounterspyButton:SetHide(true);
			agentEntry.DiplomatButton:SetHide(true);
			
			if (city and city:GetOwner() == Game.GetActivePlayer()) then
				-- Counterspy
				agentEntry.CounterspyButton:SetHide(false);
				if(v.State == "TXT_KEY_SPY_STATE_TRAVELLING") then
					agentEntry.CounterspyButton:SetDisabled(true);
					agentEntry.CounterspyButton:LocalizeAndSetToolTip("TXT_KEY_EO_COUNTERSPY_TRAVELING_TOOLTIP", v.Rank, v.Name, city:GetName());
				elseif (v.NumTurnsMovementBlocked > 0) then
					agentEntry.CounterspyButton:SetDisabled(true);
					agentEntry.CounterspyButton:LocalizeAndSetToolTip("TXT_KEY_EO_COUNTERSPY_CANNOT_CHANGE_FOCUS_TT", v.Rank, v.Name, v.NumTurnsMovementBlocked);
				else
					agentEntry.CounterspyButton:LocalizeAndSetToolTip("TXT_KEY_EO_COUNTERSPY_TOOLTIP", v.Rank, v.Name, GameDefines["ESPIONAGE_COUNTERSPY_CHANGE_FOCUS_COOLDOWN"]);
					agentEntry.CounterspyButton:SetDisabled(false);
					agentEntry.CounterspyButton:RegisterCallback(Mouse.eLClick, OnChangeCounterspyFocusClicked);
				end
			elseif (city and not Players[city:GetOwner()]:IsMinorCiv()) then
				if (pActivePlayer:IsSpyDiplomat(v.AgentID)) then
					agentEntry.DiplomatButton:SetHide(false);
					if(v.State == "TXT_KEY_SPY_STATE_TRAVELLING") then
						agentEntry.DiplomatButton:SetDisabled(true);
						agentEntry.DiplomatButton:LocalizeAndSetToolTip("TXT_KEY_EO_DIPLOMAT_TRAVELING_TOOLTIP", v.Rank, v.Name, city:GetName());
					else
						agentEntry.DiplomatButton:LocalizeAndSetToolTip("TXT_KEY_EO_DIPLOMAT_TOOLTIP", v.Rank, v.Name);
						agentEntry.DiplomatButton:SetDisabled(true);
						agentEntry.DiplomatButton:SetDisabled(false);
						agentEntry.DiplomatButton:RegisterCallback(Mouse.eLClick, OnDiplomatButtonClicked);
					end
				else
					agentEntry.SpyMissionButton:SetHide(false);
					if(v.State == "TXT_KEY_SPY_STATE_TRAVELLING") then
						agentEntry.SpyMissionButton:SetDisabled(true);
						agentEntry.SpyMissionButton:LocalizeAndSetToolTip("TXT_KEY_EO_MISSIONS_TRAVELING_TOOLTIP", v.Rank, v.Name, city:GetName());
					else
						agentEntry.SpyMissionButton:LocalizeAndSetToolTip("TXT_KEY_EO_MISSIONS_TOOLTIP", v.Rank, v.Name, city:GetName());
						agentEntry.SpyMissionButton:SetDisabled(false);
						agentEntry.SpyMissionButton:RegisterCallback(Mouse.eLClick, OnSpyMissionSelectionClicked);
					end
				end
			end

			agentEntry.AgentActivitiesStack:CalculateSize();
			agentEntry.AgentActivitiesStack:ReprocessAnchoring();
		end
	end

	Controls.AgentStack:CalculateSize();
	Controls.AgentStack:ReprocessAnchoring();
	Controls.AgentScrollPanel:CalculateInternalSize();
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

function RefreshMyCities(selectedAgentIndex, selectedAgentCurrentCityPlayerID, selectedAgentCurrentCityID, citiesAvailableForRelocate)

	--print("Refreshing My Cities");
	local map = Map;
	local pActivePlayer = Players[Game.GetActivePlayer()];
	local eActiveTeam = pActivePlayer:GetTeam();
	local textColors = g_TextColors;
	local agents = pActivePlayer:GetEspionageSpies();

	local agentsByPlayer = {};
	for i,v in ipairs(agents) do
		local plot = map.GetPlot(v.CityX, v.CityY);
		if(plot ~= nil) then
			local city = plot:GetPlotCity();

			if(city ~= nil) then
				local ownerID = city:GetOwner();
				local cityID = city:GetID();
				local playerCities = agentsByPlayer[ownerID];

				if(playerCities == nil) then
					playerCities = {};
					agentsByPlayer[ownerID] = playerCities;
				end

				playerCities[cityID] = v;
			end
		end
	end

	function GetAgentForCity(playerID, cityID)
		local playerCities = agentsByPlayer[playerID];
		if(playerCities ~= nil) then
			return playerCities[cityID];
		end
	end

	g_MyCityManager:ResetInstances();
	g_MyCityButtonManager:ResetInstances();

	function CityAvailableForRelocate(city)
		if(citiesAvailableForRelocate ~= nil) then
			for i,v in ipairs(citiesAvailableForRelocate) do
				if(v.PlayerID == city.PlayerID and v.CityID == city.CityID) then
					return true;
				end
			end
		end

		return false;
	end

	local cityStatus = pActivePlayer:GetEspionageCityStatus();

	local civilization = GameInfo.Civilizations[pActivePlayer:GetCivilizationType()];
	local myCivilizationNameKey = pActivePlayer:GetCivilizationShortDescriptionKey();
	local myCivilizationName = Locale.Lookup(myCivilizationNameKey);
	local myCivIconAtlas = civilization.IconAtlas;
	local myCivPortraitIndex = civilization.PortraitIndex;

	local myCityStatus = {};
	for i,v in ipairs(cityStatus) do
		if(v.PlayerID == Game.GetActivePlayer()) then
			v.Civilization = myCivilizationName;
			table.insert(myCityStatus, v);
		end
	end

	table.sort(myCityStatus, g_YourCitiesSortFunction);

	local bTickTock = true;

	function ApplyGenericEntrySettings(entry, cityInfo, agent, bTickTock)
		entry.Base:SetColorVal(unpack(bTickTock and g_PianoKeys[0] or g_PianoKeys[1]));

		CivIconHookup(Game.GetActivePlayer(), 32, entry.CivIcon, entry.CivIconBG, entry.CivIconShadow, false, true, entry.CivIconHighlight);

		entry.CivilizationName:SetText(myCivilizationName);
		entry.CivilizationName:SetAlpha(1.0);
-- CBP EDIT
		local pPlayer = Players[cityInfo.PlayerID];
		local pCity = pPlayer:GetCityByID(cityInfo.CityID);
		entry.CapitalCityIcon:SetHide(true);
		if(pCity ~= nil) then
			if(pCity:IsCapital())then
				if (GameInfo.Civilizations[cityInfo.CivilizationType].Type ~= "CIVILIZATION_MINOR") then
					entry.CapitalCityIcon:SetHide(false);
					entry.CapitalCityIcon:LocalizeAndSetToolTip("TXT_KEY_YOUR_CAPITAL_CBP_SPY", cityInfo.Name);
				end
			end
		end
-- END
		entry.CityName:LocalizeAndSetText(cityInfo.Name);
		entry.CityName:SetAlpha(1.0);

		local strCityNameToolTip = Locale.Lookup("TXT_KEY_EO_CITY_NAME_TT", cityInfo.Name, myCivilizationNameKey);
		local strCityCivToolTip = Locale.Lookup("TXT_KEY_EO_CITY_CIV_TT", cityInfo.Name, myCivilizationNameKey);

		entry.PotentialMeterFront:SetHide(true);

		local width, height = entry.PotentialMeterBack:GetSizeVal();
		
		local iPercentReduction = math.floor(cityInfo.Potential * GameDefines["ESPIONAGE_NP_REDUCTION_PER_SECURITY_POINT"] / 100);
		strPotentialToolTip = Locale.Lookup("TXT_KEY_EO_OWN_CITY_POTENTIAL_TT", cityInfo.Name, cityInfo.Potential, iPercentReduction, pActivePlayer:GetCitySecurityTooltip(pCity));

		local potentialMeter = entry.PotentialMeterFront;
		potentialMeter:SetHide(false);

		local pct = cityInfo.Potential/GameDefines["ESPIONAGE_MAX_NUM_SECURITY_POINTS"];
		potentialMeter:SetPercents(pct,pct);

		entry.PotentialMeterBack:SetToolTipString(strPotentialToolTip);
		entry.PotentialMeterFront:SetToolTipString(strPotentialToolTip);


		entry.CivIcon:SetToolTipString(strCityCivToolTip);
		entry.CivilizationName:SetToolTipString(strCityCivToolTip);
		entry.CityName:SetToolTipString(strCityNameToolTip);
		entry.CityPopulation:SetText(cityInfo.Population);
		entry.CityPopulation:SetAlpha(1.0);
		entry.CityPopulation:LocalizeAndSetToolTip("TXT_KEY_EO_CITY_POPULATION_TT", cityInfo.Name, cityInfo.Population);

		if(agent ~= nil) then
			local textColor = textColors[agent.State];
			entry.CivilizationName:SetColor(textColor, 0);
			entry.CityName:SetColor(textColor, 0);
			entry.CityPopulation:SetColor(textColor, 0);
			entry.SpyInCityIcon:LocalizeAndSetToolTip("TXT_KEY_SPY_OCCUPYING_CITY", agent.Name);
			entry.SpyInCityIcon:SetHide(false);
		else
			entry.SpyInCityIcon:SetToolTipString(nil);
			entry.SpyInCityIcon:SetHide(true);

			local colorSet = "Beige_Black_Alpha";
			entry.CivilizationName:SetColorByName(colorSet);
			entry.CityName:SetColorByName(colorSet);
			entry.CityPopulation:SetColorByName(colorSet);
		end
	end

	for i,v in ipairs(myCityStatus) do
		if(selectedAgentIndex == nil) then
			local cityEntry = g_MyCityManager:GetInstance();
			local agent = GetAgentForCity(v.PlayerID, v.CityID);

			ApplyGenericEntrySettings(cityEntry, v, agent, bTickTock);

		elseif(CityAvailableForRelocate(v)) then
			local cityEntry = g_MyCityButtonManager:GetInstance();
			local agent = GetAgentForCity(v.PlayerID, v.CityID);

			ApplyGenericEntrySettings(cityEntry, v, agent, bTickTock);

			cityEntry.CitySelectButton:RegisterCallback(Mouse.eLClick, function()
				Network.SendMoveSpy(Game.GetActivePlayer(), selectedAgentIndex, v.PlayerID, v.CityID, false);
				Refresh();
			end);

		else
			local cityEntry = g_MyCityManager:GetInstance();
			local agent = GetAgentForCity(v.PlayerID, v.CityID);

			ApplyGenericEntrySettings(cityEntry, v, agent, bTickTock);

			-- Dim Text
			cityEntry.CivilizationName:SetAlpha(0.4);
			cityEntry.CityName:SetAlpha(0.4);
			cityEntry.CityPopulation:SetAlpha(0.4);
		end

		bTickTock = not bTickTock;
	end

	Controls.MyCityStack:CalculateSize();
	Controls.MyCityStack:ReprocessAnchoring();
	Controls.MyCityScrollPanel:CalculateInternalSize();
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

function RefreshTheirCities(selectedAgentIndex, selectedAgentCurrentCityPlayerID, selectedAgentCurrentCityID, citiesAvailableForRelocate)

	--print("Refreshing their Cities");
	g_TheirCityManager:ResetInstances();
	g_TheirCityButtonManager:ResetInstances();

	local map = Map;
	local textColors = g_TextColors;
	local pActivePlayer = Players[Game.GetActivePlayer()];
	local eActiveTeam = pActivePlayer:GetTeam();

	local agents = pActivePlayer:GetEspionageSpies();
	local agentsByCity = {};
	for i,v in ipairs(agents) do
		local plot = map.GetPlot(v.CityX, v.CityY);

		if(plot ~= nil) then
			local city = plot:GetPlotCity();

			if(city ~= nil) then
				local ownerID = city:GetOwner();
				local cityID = city:GetID();
				local playerCities = agentsByCity[ownerID];

				if(playerCities == nil) then
					playerCities = {};
					agentsByCity[ownerID] = playerCities;
				end

				playerCities[cityID] = v;
			end
		end
	end

	function GetAgentForCity(playerID, cityID)
		local playerCities = agentsByCity[playerID];
		if(playerCities ~= nil) then
			return playerCities[cityID];
		end
	end

	function CityAvailableForRelocate(city)
		if(citiesAvailableForRelocate ~= nil) then
			for i,v in ipairs(citiesAvailableForRelocate) do
				if(v.PlayerID == city.PlayerID and v.CityID == city.CityID) then
					return true;
				end
			end
		end

		return false;
	end

	function ApplyGenericEntrySettings(entry, cityInfo, agent, bTickTock)
		entry.Base:SetColorVal(unpack(bTickTock and g_PianoKeys[0] or g_PianoKeys[1]));

		CivIconHookup( cityInfo.PlayerID, 32, entry.CivIcon, entry.CivIconBG, entry.CivIconShadow, true, true, nil);
		entry.CivIconHighlight:SetHide(false);

		entry.CivilizationName:SetText(cityInfo.CivilizationName);
		entry.CivilizationName:SetAlpha(1.0);
-- CBP EDIT
		entry.CapitalCityIcon:SetHide(true);
		entry.AllyIcon:SetHide(true);
		entry.AllyIconBG:SetHide(true);
		entry.AllyIconShadow:SetHide(true);
		entry.AllyIconHighlight:SetHide(true);
		entry.AllyName:SetHide(true);
		--ENGINSEER EDIT
		entry.SphereOfInfluenceIcon:SetHide(true);
		entry.SphereOfInfluenceMiniIcon:SetHide(true);
		--END
		local strAllyToolTip = "";
		if(cityInfo ~= nil) then
			local pPlayer = Players[cityInfo.PlayerID];
			if (pPlayer ~= nil) then
				local pCity = pPlayer:GetCityByID(cityInfo.CityID);
				if(pCity ~= nil and pCity:IsCapital()) then
					if (GameInfo.Civilizations[cityInfo.CivilizationType].Type ~= "CIVILIZATION_MINOR") then
						entry.CapitalCityIcon:LocalizeAndSetToolTip("TXT_KEY_CAPITAL_CBP_SPY", cityInfo.Name, cityInfo.CivilizationNameKey);
						entry.CapitalCityIcon:SetHide(false);
					elseif(pPlayer:IsMinorCiv()) then
						--ENGINSEER EDIT
						if (pPlayer:GetPermanentAlly() > -1) then
							entry.SphereOfInfluenceIcon:SetHide(false);
							entry.SphereOfInfluenceIcon:SetText("[ICON_PUPPET]");
							entry.SphereOfInfluenceIcon:LocalizeAndSetToolTip("TXT_KEY_CITY_STATE_PERMANENT_ALLY_TT", Players[pPlayer:GetPermanentAlly()]:GetCivilizationShortDescriptionKey());
						elseif (pPlayer:IsNoAlly()) then
							entry.SphereOfInfluenceIcon:SetHide(false);
							entry.SphereOfInfluenceIcon:SetText("[ICON_FLOWER]");
							entry.SphereOfInfluenceIcon:LocalizeAndSetToolTip("TXT_KEY_CITY_STATE_ALLY_NOBODY_PERMA");
						else
							entry.SphereOfInfluenceIcon:SetText("");
							entry.SphereOfInfluenceIcon:LocalizeAndSetToolTip(nil);
							entry.SphereOfInfluenceIcon:SetHide(true);
							local iCityAlly = Players[pCity:GetOwner()]:GetAlly();
							if(iCityAlly ~= -1) then
								entry.AllyName:SetHide(false);
								entry.AllyIcon:SetHide(false);
								entry.AllyIconBG:SetHide(false);
								entry.AllyIconShadow:SetHide(false);
								entry.AllyIconHighlight:SetHide(false);
								--ENGINSEER AND AXATIN EDIT
								if iCityAlly ~= Game.GetActivePlayer() then
									local bHasMet = Teams[Game.GetActiveTeam()]:IsHasMet(Players[iCityAlly]:GetTeam());
									if (bHasMet == true) then
										strAllyToolTip = Locale.Lookup("TXT_KEY_CITY_STATE_ALLY_TT", Players[iCityAlly]:GetCivilizationShortDescription(), pPlayer:GetMinorCivFriendshipWithMajor(iCityAlly) - pPlayer:GetMinorCivFriendshipWithMajor(Game.GetActivePlayer()) + 1);
										CivIconHookup(iCityAlly, 32, entry.AllyIcon, entry.AllyIconBG, entry.AllyIconShadow, false, true, entry.AllyIconHighlight);
									else
										strAllyToolTip = Locale.Lookup("TXT_KEY_CITY_STATE_ALLY_UNKNOWN_TT", pPlayer:GetMinorCivFriendshipWithMajor(iCityAlly) - pPlayer:GetMinorCivFriendshipWithMajor(Game.GetActivePlayer()) + 1);
										CivIconHookup(-1, 32, entry.AllyIcon, entry.AllyIconBG, entry.AllyIconShadow, false, true, entry.AllyIconHighlight);
									end
								else
									strAllyToolTip = Locale.Lookup("TXT_KEY_ALLY_CBP_SPY", Players[iCityAlly]:GetCivilizationDescriptionKey());
									CivIconHookup(iCityAlly, 32, entry.AllyIcon, entry.AllyIconBG, entry.AllyIconShadow, false, true, entry.AllyIconHighlight);
								end
								--END
								entry.AllyName:SetToolTipString(strAllyToolTip);
								entry.AllyIconHighlight:SetToolTipString(strAllyToolTip);
								entry.AllyIconShadow:SetToolTipString(strAllyToolTip);
								entry.AllyIconBG:SetToolTipString(strAllyToolTip);
								entry.AllyIcon:SetToolTipString(strAllyToolTip);
							end
						end
					end
				end
			end
		end
-- END
		entry.CityName:LocalizeAndSetText(cityInfo.Name);
		entry.CityName:SetAlpha(1.0);

		entry.PotentialMeterBack:SetHide(true);
		entry.PotentialMeterGhost:SetHide(true);
		entry.PotentialMeterFront:SetHide(true);
		entry.RigElectionDisabled:SetHide(true);
		entry.RigElectionEnabled:SetHide(true);

		--CBP
		entry.RigElectionCooldown:SetHide(true);
		--END
		--ENGINSEER EDIT
		entry.PledgeToProtect:SetHide(true);
		--END
		entry.UnknownProgress:SetHide(true);

		local bIsCityState = false;
		if (GameInfo.Civilizations[cityInfo.CivilizationType].Type == "CIVILIZATION_MINOR") then
			bIsCityState = true;

			if(agent ~= nil and agent.State == "TXT_KEY_SPY_STATE_RIGGING_ELECTION") then
				entry.RigElectionEnabled:SetHide(false);
			else
				entry.RigElectionDisabled:SetHide(false);
			end
		else
			--local width, height = entry.PotentialMeterBack:GetSizeVal();
			if(cityInfo.Potential >= 0) then
				entry.PotentialMeterBack:SetHide(false);
				local potentialMeter;
				if (agent ~= nil) then
					potentialMeter = entry.PotentialMeterFront;
				else
					potentialMeter = entry.PotentialMeterGhost;
				end

				potentialMeter:SetHide(false);

				local pct = cityInfo.Potential/GameDefines["ESPIONAGE_MAX_NUM_SECURITY_POINTS"];
				potentialMeter:SetPercents(pct,pct);
			else
				entry.UnknownProgress:SetHide(false);
			end
		end

		local strCityNameToolTip = Locale.Lookup("TXT_KEY_EO_CITY_NAME_TT", cityInfo.Name, cityInfo.CivilizationNameKey);
		local strCityCivToolTip = Locale.Lookup("TXT_KEY_EO_CITY_CIV_TT", cityInfo.Name, cityInfo.CivilizationNameKey);

		if(cityInfo ~= nil and selectedAgentIndex ~= nil) then
			local pPlayer = Players[cityInfo.PlayerID];
			if (pPlayer ~= nil) then
				local pCity = pPlayer:GetCityByID(cityInfo.CityID);
				if(pCity) then
					if(not Players[Game.GetActivePlayer()]:CanMoveSpyTo(pCity, selectedAgentIndex)) then
						strCityNameToolTip = Locale.Lookup("TXT_KEY_EO_NO_ACTIONS_POSSIBLE");
						strCityCivToolTip = Locale.Lookup("TXT_KEY_EO_NO_ACTIONS_POSSIBLE");
					end
				end
			end
		end

		if (bIsCityState) then
			local strTraitText = GetCityStateTraitText(cityInfo.PlayerID);
			local strTraitTT = GetCityStateTraitToolTip(cityInfo.PlayerID);

			strCityNameToolTip = Locale.Lookup("TXT_KEY_EO_CITY_NAME_CITY_STATE_TT", cityInfo.Name, strTraitText);
			strCityNameToolTip = strCityNameToolTip .. "[NEWLINE][NEWLINE]";
			strCityNameToolTip = strCityNameToolTip .. strTraitTT;
			strCityCivToolTip = Locale.Lookup("TXT_KEY_EO_CITY_CIV_CITY_STATE_TT", cityInfo.Name, strTraitText);
			strCityCivToolTip = strCityCivToolTip .. "[NEWLINE][NEWLINE]";
			strCityCivToolTip = strCityCivToolTip .. strTraitTT;

			local strCityStateTT = Locale.Lookup("TXT_KEY_EO_CITY_STATE_POTENTIAL_TT");
			strCityStateTT = strCityStateTT .. "[NEWLINE][NEWLINE]";
			strCityStateTT = strCityStateTT .. Locale.Lookup("TXT_KEY_EO_CITY_STATE_ELECTION", Game.GetTurnsBetweenMinorCivElections(), Game.GetTurnsUntilMinorCivElection());
			entry.RigElectionDisabled:SetToolTipString(strCityStateTT);
			entry.RigElectionEnabled:SetToolTipString(strCityStateTT);
			if (getProtectingPlayers(cityInfo.PlayerID) ~= "") then
				entry.PledgeToProtect:SetHide(false);
				entry.PledgeToProtect:SetToolTipString(Locale.Lookup("TXT_KEY_POP_CSTATE_PROTECTED_BY_TT") .. "[NEWLINE][NEWLINE]" .. Locale.Lookup("TXT_KEY_POP_CSTATE_PROTECTED_BY") .. " " .. getProtectingPlayers(cityInfo.PlayerID));
			end
		else
			local iPercentReduction = math.floor(cityInfo.Potential * GameDefines["ESPIONAGE_NP_REDUCTION_PER_SECURITY_POINT"] / 100);
			strPotentialToolTip = Locale.Lookup("TXT_KEY_EO_CITY_POTENTIAL_TT",cityInfo.Name, cityInfo.Potential, iPercentReduction);
			entry.PotentialMeterBack:SetToolTipString(strPotentialToolTip);
			entry.PotentialMeterGhost:SetToolTipString(strPotentialToolTip);
			entry.PotentialMeterFront:SetToolTipString(strPotentialToolTip);
		end

		entry.CivIcon:SetToolTipString(strCityCivToolTip);
		entry.CivilizationName:SetToolTipString(strCityCivToolTip);
		entry.CityName:SetToolTipString(strCityNameToolTip);
		entry.CityPopulation:SetText(cityInfo.Population);
		entry.CityPopulation:SetAlpha(1.0);
		entry.CityPopulation:LocalizeAndSetToolTip("TXT_KEY_EO_CITY_POPULATION_TT", cityInfo.Name, cityInfo.Population);

		if(agent ~= nil) then
			local textColor = textColors[agent.State];
			entry.CivilizationName:SetColor(textColor, 0);
			entry.CityName:SetColor(textColor, 0);
			entry.CityPopulation:SetColor(textColor, 0);

			entry.DiplomatInCityIcon:SetHide(true);
			entry.SpyInCityIcon:SetHide(true);

			if (agent.IsDiplomat) then
				entry.DiplomatInCityIcon:LocalizeAndSetToolTip("TXT_KEY_SPY_OCCUPYING_CITY", agent.Name);
				entry.DiplomatInCityIcon:SetHide(false);
			else
				entry.SpyInCityIcon:LocalizeAndSetToolTip("TXT_KEY_SPY_OCCUPYING_CITY", agent.Name);
				entry.SpyInCityIcon:SetHide(false);
			end

		else
			entry.SpyInCityIcon:SetToolTipString(nil);
			entry.SpyInCityIcon:SetHide(true);
			entry.DiplomatInCityIcon:SetToolTipString(nil);
			entry.DiplomatInCityIcon:SetHide(true);

			local colorSet = "Beige_Black_Alpha";
			entry.CivilizationName:SetColorByName(colorSet);
			entry.CityName:SetColorByName(colorSet);
			entry.CityPopulation:SetColorByName(colorSet);
		end

	end

	local cityStatus = pActivePlayer:GetEspionageCityStatus();
	local theirCityStatus = {};

	for i,v in ipairs(cityStatus) do
		if(v.Team ~= eActiveTeam) then

			local theirCivilization = GameInfo.Civilizations[v.CivilizationType];

			if(theirCivilization.Type == "CIVILIZATION_MINOR") then
				local minorCivPlayer = Players[v.PlayerID];
				local minorCivType = minorCivPlayer:GetMinorCivType();
				local minorCiv = GameInfo.MinorCivilizations[minorCivType];
				local primary,secondary = minorCivPlayer:GetPlayerColors();

				v.CivilizationPortraitIndex = 22;
				v.CivilizationIconAtlas = "CIV_ALPHA_ATLAS";
				v.CivilizationIconColor = secondary;
				v.CivilizationNameKey = minorCiv.ShortDescription;
				v.CivilizationName = Locale.Lookup(minorCiv.ShortDescription);
			else
				v.CivilizationPortraitIndex = theirCivilization.PortraitIndex;
				v.CivilizationIconAtlas = theirCivilization.IconAtlas;
				v.CivilizationIconColor = {x = 1.0, y = 1.0, z = 1.0, w = 1.0};
				v.CivilizationNameKey = theirCivilization.ShortDescription;
				v.CivilizationName = Locale.Lookup(theirCivilization.ShortDescription);
			end


			table.insert(theirCityStatus, v);
		end
	end

	table.sort(theirCityStatus, g_TheirCitiesSortFunction);

	for i,v in ipairs(theirCityStatus) do

		if(selectedAgentIndex == nil) then
			local cityEntry = g_TheirCityManager:GetInstance();
			local agent = GetAgentForCity(v.PlayerID, v.CityID);

			ApplyGenericEntrySettings(cityEntry, v, agent, bTickTock)


		elseif(CityAvailableForRelocate(v)) then
			local cityEntry = g_TheirCityButtonManager:GetInstance();
			local agent = GetAgentForCity(v.PlayerID, v.CityID);

			local plot = map.GetPlot(v.CityX, v.CityY);
			local city = plot:GetPlotCity();

			local pMyTeam = Teams[Game.GetActiveTeam()]

			local bCheckDiplomat = false;
			if (city:IsCapital() and (not Players[v.PlayerID]:IsMinorCiv()) and (not pMyTeam:IsAtWar(city:GetTeam()))) then
				bCheckDiplomat = true;
			end

			ApplyGenericEntrySettings(cityEntry, v, agent, bTickTock)

			if (bCheckDiplomat) then
				cityEntry.CitySelectButton:RegisterCallback(Mouse.eLClick, function()
					g_ConfirmAction = function()
						Network.SendMoveSpy(Game.GetActivePlayer(), selectedAgentIndex, v.PlayerID, v.CityID, true);
						Refresh();
					end
					g_DenyAction = function()
						Network.SendMoveSpy(Game.GetActivePlayer(), selectedAgentIndex, v.PlayerID, v.CityID, false);
						Refresh();
					end

					Controls.ConfirmText:LocalizeAndSetText("TXT_KEY_SPY_BE_DIPLOMAT");
					Controls.ConfirmContent:CalculateSize();
					local width, height = Controls.ConfirmContent:GetSizeVal();
					Controls.ConfirmFrame:SetSizeVal(width + 60, height + 120);
					Controls.ChooseConfirm:SetHide(false);
					Controls.YesString:LocalizeAndSetText("TXT_KEY_DIPLOMAT_PICKER_DIPLOMAT");
					Controls.NoString:LocalizeAndSetText("TXT_KEY_DIPLOMAT_PICKER_SPY");
				end);
			else
				cityEntry.CitySelectButton:RegisterCallback(Mouse.eLClick, function()
					Network.SendMoveSpy(Game.GetActivePlayer(), selectedAgentIndex, v.PlayerID, v.CityID, false);
					Refresh();
				end);
			end
		else
			local cityEntry = g_TheirCityManager:GetInstance();
			local agent = GetAgentForCity(v.PlayerID, v.CityID);

			ApplyGenericEntrySettings(cityEntry, v, agent, bTickTock)

			cityEntry.CivilizationName:SetAlpha(0.4);
			cityEntry.CityName:SetAlpha(0.4);
			cityEntry.CityPopulation:SetAlpha(0.4);
		end

		bTickTock = not bTickTock;
	end

	Controls.TheirCityStack:CalculateSize();
	Controls.TheirCityStack:ReprocessAnchoring();
	Controls.TheirCityScrollPanel:CalculateInternalSize();
end

function RefreshUnitList()

	g_UnitListManager:ResetInstances();
	
	unitList = g_UnitList
	table.sort(unitList, g_UnitListSortFunction);
	
	for i,v in ipairs(unitList) do
		local unitEntry = g_UnitListManager:GetInstance();

		unitEntry.UnitName:SetText(v.Name);
		if(v.Strength > 0) then
			unitEntry.UnitStrength:SetText(v.Strength);
		else
			unitEntry.UnitStrength:SetText("-");
		end
		if(v.RangedStrength > 0) then
			unitEntry.UnitRangedStrength:SetText(v.RangedStrength);
		else
			unitEntry.UnitRangedStrength:SetText("-");
		end
		unitEntry.UnitCount:SetText(v.Count);

		-- Dim Text
		--cityEntry.CivilizationName:SetAlpha(0.4);
		--cityEntry.CityName:SetAlpha(0.4);
		--cityEntry.CityPopulation:SetAlpha(0.4);

		bTickTock = not bTickTock;
	end

	Controls.UnitListStack:CalculateSize();
	Controls.UnitListStack:ReprocessAnchoring();
	Controls.UnitListScrollPanel:CalculateInternalSize();
end

function Refresh()

	function TestEspionageStarted()
		local player = Players[Game.GetActivePlayer()];
		return player:GetNumSpies() > 0;
	end

	local bEspionageStarted = TestEspionageStarted();

	Controls.LabelEspionageNotStartedYet:SetHide(bEspionageStarted);
	Controls.TabPanel:SetHide(not bEspionageStarted);
	Controls.AgentsListBox:SetHide(not bEspionageStarted);
	Controls.AgentMovePanel:SetHide(true);
	Controls.CitiesListBox:SetHide(not bEspionageStarted);
	Controls.CloseButton:SetHide(false);

	if(bEspionageStarted) then
		g_SelectedAgentID = nil;
		g_SelectedCity = nil;
		g_CitiesAvailableToRelocate = nil;

		RefreshAgents();
		RefreshMyCities();
		RefreshTheirCities();
		RefreshIntrigue();
	end
end
Events.SerialEventEspionageScreenDirty.Add(Refresh);

function OnYes( )
	Controls.ChooseConfirm:SetHide(true);

	if(g_ConfirmAction ~= nil) then
		g_ConfirmAction();
	end
end
Controls.Yes:RegisterCallback( Mouse.eLClick, OnYes );

function OnNo( )
	Controls.ChooseConfirm:SetHide(true);
	if (g_DenyAction ~= nil) then
		g_DenyAction();
	end
end
Controls.No:RegisterCallback( Mouse.eLClick, OnNo );

function PendingDealButtonHandler( iPlayer, index )

    UI.LoadCurrentDeal( iPlayer, index );
    
    local iBeginTurn = m_Deal:GetStartTurn();
    local iDuration  = m_Deal:GetDuration();
    if( iDuration ~= 0 ) then
        Controls.TurnStart:SetText( Locale.ConvertTextKey( "TXT_KEY_DO_DEAL_BEGAN", iBeginTurn ) );
        Controls.TurnEnd:SetHide( false );
        Controls.TurnEnd:SetText( Locale.ConvertTextKey( "TXT_KEY_DO_DEAL_DURATION", iDuration ) );
    else
        Controls.TurnStart:SetText( Locale.ConvertTextKey( "TXT_KEY_DO_ON_TURN", iBeginTurn ) );
        Controls.TurnEnd:SetHide( true );
    end
    
    OpenDealReview(iPlayer);
end

function BuildDealButton( iPlayer, controlTable )    

    local iOtherPlayer = m_Deal:GetOtherPlayer( iPlayer );
    local pOtherPlayer = Players[ iOtherPlayer ];
    
    controlTable.TurnsLabel:LocalizeAndSetText( "TXT_KEY_DO_ON_TURN", (m_Deal:GetStartTurn() + m_Deal:GetDuration()) );
	CivIconHookup( iOtherPlayer, 32, controlTable.CivIcon, controlTable.CivIconBG, controlTable.CivIconShadow, false, true );
   
    local civName = Locale.ConvertTextKey( GameInfo.Civilizations[ pOtherPlayer:GetCivilizationType() ].ShortDescription );
    
    --if( m_bIsMulitplayer and pOtherPlayer:IsHuman() ) then
    if( pOtherPlayer:GetNickName() ~= "" and pOtherPlayer:IsHuman() ) then
	
        controlTable.PlayerLabel:SetText( pOtherPlayer:GetNickName() );
        controlTable.CivLabel:SetText( civName );
    else
    
        controlTable.PlayerLabel:SetText( pOtherPlayer:GetName() );
        controlTable.CivLabel:SetText( civName );
	end
    
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function BuildCurrentDealButton( iPlayer, controlTable )    

    local iOtherPlayer = m_Deal:GetOtherPlayer( iPlayer );
    local pOtherPlayer = Players[ iOtherPlayer ];
    
    controlTable.TurnsLabel:LocalizeAndSetText( "TXT_KEY_DO_ENDS_ON", (m_Deal:GetStartTurn() + m_Deal:GetDuration()) );
	CivIconHookup( iOtherPlayer, 32, controlTable.CivIcon, controlTable.CivIconBG, controlTable.CivIconShadow, false, true );
    
   
    local civName = Locale.ConvertTextKey( GameInfo.Civilizations[ pOtherPlayer:GetCivilizationType() ].ShortDescription );
    
    --if( m_bIsMulitplayer and pOtherPlayer:IsHuman() ) then
    if( pOtherPlayer:GetNickName() ~= "" and pOtherPlayer:IsHuman() ) then
	
        controlTable.PlayerLabel:SetText( pOtherPlayer:GetNickName() );
        controlTable.CivLabel:SetText( civName );
    else
    
        controlTable.PlayerLabel:SetText( pOtherPlayer:GetName() );
        controlTable.CivLabel:SetText( civName );
	end
    
end


function PopulateDealChooserDiplomat(iPlayer)
	m_Deal:ClearItems();
	DoClearTable();
    DisplayDeal();
    Controls.CurrentDealsStack:DestroyAllChildren();
        
    local iNumCurrentDeals = UI.GetNumCurrentDeals( iPlayer );    
    if( iNumCurrentDeals > 0 ) then
        
        Controls.CurrentDealsStack:SetHide( false );
        for i = 0, iNumCurrentDeals - 1 do
        
            controlTable = {};
            ContextPtr:BuildInstanceForControl( "DealButtonInstance", controlTable, Controls.CurrentDealsStack );
            
            controlTable.DealButton:SetVoids( iPlayer, i);
            controlTable.DealButton:RegisterCallback( Mouse.eLClick, PendingDealButtonHandler );
            
            UI.LoadCurrentDeal( iPlayer, i );
            BuildDealButton( iPlayer, controlTable );
            BuildCurrentDealButton( iPlayer, controlTable );
        end
		Controls.NoDealsText:SetHide( true );
    else
        Controls.CurrentDealsStack:SetHide( true );
		Controls.NoDealsText:SetHide( false );
    end
    
    Controls.CurrentDealsStack:CalculateSize();
    Controls.AllDealsStack:CalculateSize();
    Controls.ListScrollPanel:CalculateInternalSize();
    Controls.AllDealsStack:ReprocessAnchoring();
end


PopulateSelectionList = function(stackControl, playerID, city, spy)
	-- used for choosing a spy mission or a counterspy focus
	
	local count = 0;
	local player = Players[playerID];

	SelectedItems = {};
	stackControl:DestroyAllChildren();
	
	local cityName = city:GetNameKey();
	local localizedCityName = Locale.ConvertTextKey(cityName);

	local iEventType;
	local iEventID;
	local instance;
	if (playerID == city:GetOwner()) then
		-- counterspy
		Controls.CounterspyFocusDescription:LocalizeAndSetText("TXT_KEY_EO_COUNTERSPY_TOOLTIP", spy.Rank, spy.Name, GameDefines["ESPIONAGE_COUNTERSPY_CHANGE_FOCUS_COOLDOWN"]);
		for row in GameInfo.CityEvents("IsCounterSpy") do
			iEventType = row.Type;
			iEventID = row.ID;
		end
	else
		-- spy in enemy city
		Controls.MissionSelectionDescription:LocalizeAndSetText("TXT_KEY_EO_MISSIONS_HELP");
		for row in GameInfo.CityEvents("EspionageSetup") do
			iEventType = row.Type;
			iEventID = row.ID;
		end
	end
	
	----------------------------------------------------------------        
	-- build the buttons
	----------------------------------------------------------------
	
	if(iEventType ~= nil) then
		local buttonSizeY = 53
		for row in GameInfo.CityEvent_ParentEvents("CityEventType = '" .. iEventType .. "'") do
			local info = GameInfo.CityEventChoices[row.CityEventChoiceType]

			local eventChoiceType = info.Type;
			local instance;
			if (playerID == city:GetOwner()) then
				instance = g_CounterspyFocusManager:GetInstance();
			else
				instance = g_MissionSelectionManager:GetInstance();
			end

			local szDescString;
			local szHelpString;
			szDescString = Locale.Lookup(info.Description);
			szHelpString = Locale.ConvertTextKey(city:GetScaledEventChoiceValue(info.ID, false, spyID, playerID), localizedCityName);
			
			if (playerID ~= city:GetOwner()) then
				szHelpString = szHelpString .. "[NEWLINE][NEWLINE]";
				szHelpString = szHelpString .. Locale.Lookup("TXT_KEY_EO_ID_KILL_CHANCE", info.SpyIDChance, info.SpyKillChance);
			end
	
			if(info.NetworkPointsNeeded > 0) then
				local iNPScaled = GetNetworkPointsScaled(info);
				instance.Name:LocalizeAndSetText("TXT_KEY_EO_MISSION_COST", iNPScaled, szDescString);
			else
				instance.Name:SetText(szDescString);
			end
			instance.Button:SetToolTipString(szHelpString);
			
			-- Readjust the offset
			local sizeYDiff = math.max((instance.Name:GetSizeY()-buttonSizeY),1)
			if sizeYDiff > 1 then sizeYDiff = sizeYDiff + 20 end
			instance.Base:SetSizeY(buttonSizeY + sizeYDiff)
			instance.Button:SetSizeY(buttonSizeY + sizeYDiff)
			instance.MOSelectionAnim:SetSizeY(buttonSizeY + sizeYDiff)
			instance.MOSelectionAnimHL:SetSizeY(buttonSizeY + sizeYDiff)
			instance.SelectionAnim:SetSizeY(buttonSizeY + sizeYDiff)
			instance.SelectionAnimHL:SetSizeY(buttonSizeY + sizeYDiff)
			
			-- Disable invalid choices
			if(not city:IsCityEventChoiceValidEspionage(info.ID, iEventID, spy.AgentID, playerID)) then
				local szDisabledString = "";
				if (playerID == city:GetOwner()) then
					szDisabledString = Locale.Lookup("TXT_KEY_EO_COUNTERSPY_CANNOT_CHANGE_ACTIVE_FOCUS_TT");
				else
					szDisabledString = city:GetDisabledTooltip(info.ID, spy.AgentID, playerID );
				end
				if(szDisabledString == "") then
					szDisabledString = "TXT_KEY_CANNOT_TAKE_TT";
				end
				szHelpString = szHelpString .. "[NEWLINE][NEWLINE]" .. szDisabledString;
				instance.Button:SetDisabled(true)
				instance.Name:SetAlpha(0.2)
				instance.Button:SetToolTipString(szHelpString);
			else
				instance.Name:SetAlpha(1)
				instance.Button:SetDisabled(false)
			end
			
			local selectionAnim = instance.SelectionAnim;

			instance.Button:RegisterCallback(Mouse.eLClick, function()  
				local foundIndex = nil;
				for i,v in ipairs(SelectedItems) do
					if(v[1] == eventChoiceType) then
						foundIndex = i;
						break;
					end
				end
			
				if(foundIndex == nil) then
					if(#SelectedItems > 0) then
						for i,v in ipairs(SelectedItems) do
							v[2].SelectionAnim:SetHide(true);	
						end
						SelectedItems = {};
					end
				
					selectionAnim:SetHide(false);
					table.insert(SelectedItems, {eventChoiceType, instance});
				end
			
				if (playerID == city:GetOwner()) then
					Controls.ConfirmCounterspyFocusButton:SetDisabled(#SelectedItems ~= 1);
				else
					Controls.ConfirmMissionSelectionButton:SetDisabled(#SelectedItems ~= 1);
				end
			
			end);
		
			count = count + 1;
		end
	end
	return count;
end


PopulatePassiveBonusList = function(stackControl, playerID, city, spy)
	-- passive bonuses for offensive spies
	
	local count = 0;
	local player = Players[playerID];

	SelectedItems = {};
	stackControl:DestroyAllChildren();
	
	local cityName = city:GetNameKey();
	local localizedCityName = Locale.ConvertTextKey(cityName);

	local instance;
	
	local strUnlockedPassiveBonusText = "";
	local iNetworkPointsOfBestUnlockedBonus = -1;
	for row in GameInfo.SpyPassiveBonuses() do
		local iNPScaled = GetNetworkPointsScaled(row);
		if (iNPScaled <= spy.MaxNetworkPointsStored and iNPScaled > iNetworkPointsOfBestUnlockedBonus) then
			iNetworkPointsOfBestUnlockedBonus = iNPScaled;
			strUnlockedPassiveBonusText = Locale.Lookup(row.Help);
		end
	end
	
		
	if (strUnlockedPassiveBonusText ~= "") then
		if(spy.MaxNetworkPointsStored == spy.NetworkPointsStored) then
			Controls.PassiveBonusesDescription:LocalizeAndSetText("TXT_KEY_EO_PASSIVE_BONUSES_UNLOCKED", spy.MaxNetworkPointsStored, strUnlockedPassiveBonusText);
		else
			Controls.PassiveBonusesDescription:LocalizeAndSetText("TXT_KEY_EO_PASSIVE_BONUSES_UNLOCKED_FROM_EARLIER", spy.MaxNetworkPointsStored, strUnlockedPassiveBonusText);
		end
	else
		Controls.PassiveBonusesDescription:LocalizeAndSetText("TXT_KEY_EO_PASSIVE_BONUSES_UNLOCKED_NOTHING_YET", spy.NetworkPointsStored);
	end
	
	-- check if we can view the city screen
	local iNetworkPointsNeededToViewCityScreen = 999999;
	for row in GameInfo.SpyPassiveBonuses("RevealCityScreen") do
		local iNPScaled = GetNetworkPointsScaled(row);
		iNetworkPointsNeededToViewCityScreen = math.min(iNetworkPointsNeededToViewCityScreen, iNPScaled);
	end;
	
	
	-- City Screen Closed
	local CityScreenClosed = function()
		UIManager:DequeuePopup(Controls.EmptyPopup);
		--print("This CityScreenClosed was called!");
		Events.SerialEventExitCityScreen.Remove(CityScreenClosed);
		UI.SetCityScreenViewingMode(false);
	end
			
	-- Initialize 'View City' button.
	local OnViewCityClicked = function()

		if (city ~= nil) then
			print("Attempting to show city screen");
			-- Queue up an empty popup at a higher priority so that it prevents other cities from appearing while we're looking at this one!
			UIManager:QueuePopup(Controls.EmptyPopup, PopupPriority.GenericPopup+1);
			Events.SerialEventExitCityScreen.Add(CityScreenClosed);
			UI.SetCityScreenViewingMode(true);
			UI.DoSelectCityAtPlot( city:Plot() );
		else
			print("city == null");
		end
	end
	
	if(iNetworkPointsNeededToViewCityScreen == 999999) then
		-- no passive bonus unlocks the city screen, hide the button
		Controls.ViewCityButton:SetHide(true);
	elseif (spy.MaxNetworkPointsStored >= iNetworkPointsNeededToViewCityScreen) then
		Controls.ViewCityButton:SetDisabled(false);
		Controls.ViewCityButton:SetToolTipString("");
		Controls.ViewCityButton:RegisterCallback(Mouse.eLClick, OnViewCityClicked);
	else
		Controls.ViewCityButton:SetDisabled(true);
		Controls.ViewCityButton:SetToolTipString(Locale.Lookup("TXT_KEY_EO_CITY_SCREEN_NOT_ENOUGH_NP", iNetworkPointsNeededToViewCityScreen));
	end
	
	----------------------------------------------------------------        
	-- build the passive bonuses list
	----------------------------------------------------------------
	
	local buttonSizeY = 53
	for row in GameInfo.SpyPassiveBonuses() do

		local instance = g_PassiveBonusesManager:GetInstance();

		local szDescString;
		local szHelpString;
		local iNPScaled = GetNetworkPointsScaled(row);
		szDescString = Locale.Lookup("TXT_KEY_EO_PASSIVE_BONUS_NP_REQUIRED", iNPScaled, Locale.Lookup(row.Description));
		szHelpString = Locale.Lookup(row.Help);

		instance.Button:SetToolTipString(Locale.Lookup("TXT_KEY_EO_PASSIVE_BONUS_THRESHOLD_TT", szHelpString));
		
		-- Readjust the offset
		local sizeYDiff = math.max((instance.Name:GetSizeY()-buttonSizeY),1)
		if sizeYDiff > 1 then sizeYDiff = sizeYDiff + 20 end
		instance.Base:SetSizeY(buttonSizeY + sizeYDiff)
		instance.Button:SetSizeY(buttonSizeY + sizeYDiff)
		instance.Button:SetDisabled(true);
		
		if(iNPScaled > spy.MaxNetworkPointsStored) then
			instance.Name:SetText(szDescString);
			instance.Name:SetAlpha(0.2)
		else
			instance.Name:SetText(szDescString);
			instance.Name:SetAlpha(1)
		end
	
		count = count + 1;
	end
	return count;
end

PopulateDiplomatBonusList = function(stackControl, playerID, city, spy)
	-- passive bonuses for offensive spies
	
	local count = 0;
	local player = Players[playerID];

	SelectedItems = {};
	stackControl:DestroyAllChildren();
	
	local cityName = city:GetNameKey();
	local localizedCityName = Locale.ConvertTextKey(cityName);

	local instance;
	
	local strUnlockedPassiveBonusText = "";
	local iNetworkPointsOfBestUnlockedBonus = -1;
	for row in GameInfo.SpyPassiveBonusesDiplomat() do
		local iNPScaled = GetNetworkPointsScaled(row);
		if (iNPScaled <= spy.MaxNetworkPointsStored and iNPScaled > iNetworkPointsOfBestUnlockedBonus) then
			iNetworkPointsOfBestUnlockedBonus = iNPScaled;
			strUnlockedPassiveBonusText = Locale.Lookup(row.Help);
		end
	end
	
	Controls.DiplomatDescription:LocalizeAndSetText("TXT_KEY_EO_PASSIVE_BONUSES_UNLOCKED_DIPLOMAT", spy.MaxNetworkPointsStored, strUnlockedPassiveBonusText);
	
	-- initialize buttons
			
	local OnViewTechScreenClicked = function()

		if (city ~= nil) then
			-- print("Attempting to show city screen");			
			Events.SerialEventGameMessagePopup( { Type = ButtonPopupTypes.BUTTONPOPUP_TECH_TREE , Data2 = -1, Data4 = 1, Data5 = city:GetOwner()} );
		else
			print("city == null");
		end
	end
	
	local OnViewPolicyScreenClicked = function()

		if (city ~= nil) then
			-- print("Attempting to show city screen");			
			Events.SerialEventGameMessagePopup( { Type = ButtonPopupTypes.BUTTONPOPUP_CHOOSEPOLICY, Data3 = 1, Data4 = city:GetOwner() } );
			print(city:GetOwner());
		else
			print("city == null");
		end
	end	
	
	local OnViewUnitListClicked = function()
		if (city ~= nil) then
			local iPlayer = city:GetOwner();
			local pPlayer = Players[iPlayer];
			local unitID, count;
			local UnitList = {}
			
			for row in GameInfo.UnitClasses() do
				count = pPlayer:GetUnitClassCount(row.ID);
				if count > 0 then
					unitID = pPlayer:GetSpecificUnitType(row.Type);
					-- only military units
					if (GameInfo.Units[unitID].Combat > 0 or GameInfo.Units[unitID].RangedCombat > 0) then
						local entry = {}
						entry["Name"] = Locale.Lookup(GameInfo.Units[unitID].Description);
						entry["Strength"] = GameInfo.Units[unitID].Combat
						entry["RangedStrength"] = GameInfo.Units[unitID].RangedCombat
						entry["Count"] = count;
						table.insert(UnitList, entry);
					end
				end
			end
			
			Controls.UnitListPopup:SetHide(false);
			g_UnitList = UnitList
			RefreshUnitList();
		else
			print("city == null");
		end
	end	
	
	local OnViewTradeDealsClicked = function()
		if (city ~= nil) then
			local iPlayer = city:GetOwner();
			Controls.DiplomatTradeDealsPopup:SetHide(false);
			Controls.DealsPanel:SetHide( false );
			PopulateDealChooserDiplomat(iPlayer);
			Controls.TradeDetails:SetHide( true )
			Controls.CloseDiplomatTradeButton:RegisterCallback(Mouse.eLClick, OnCloseDiplomatTradeButton);
		else
			print("city == null");
		end
	end
	
	-- check which buttons are active
	local iNetworkPointsNeededToViewTechTree = 999999;
	local iNetworkPointsNeededToViewPolicyTree = 999999;
	local iNetworkPointsNeededToViewUnitList = 999999;
	local iNetworkPointsNeededToViewTradeDeals = 999999;
	for row in GameInfo.SpyPassiveBonusesDiplomat("RevealTechTree") do
		local iNPScaled = GetNetworkPointsScaled(row);
		iNetworkPointsNeededToViewTechTree = math.min(iNetworkPointsNeededToViewTechTree, iNPScaled);
	end;
	for row in GameInfo.SpyPassiveBonusesDiplomat("RevealPolicyTree") do
		local iNPScaled = GetNetworkPointsScaled(row);
		iNetworkPointsNeededToViewPolicyTree = math.min(iNetworkPointsNeededToViewPolicyTree, iNPScaled);
	end;
	for row in GameInfo.SpyPassiveBonusesDiplomat("RevealMilitaryUnitCount") do
		local iNPScaled = GetNetworkPointsScaled(row);
		iNetworkPointsNeededToViewUnitList = math.min(iNetworkPointsNeededToViewUnitList, iNPScaled);
	end;
	for row in GameInfo.SpyPassiveBonusesDiplomat("RevealTradeDeals") do
		local iNPScaled = GetNetworkPointsScaled(row);
		iNetworkPointsNeededToViewTradeDeals = math.min(iNetworkPointsNeededToViewTradeDeals, iNPScaled);
	end;
	
	if(iNetworkPointsNeededToViewTechTree == 999999) then
		Controls.ButtonDiplomatViewTechScreen:SetHide(true);
	elseif (spy.MaxNetworkPointsStored >= iNetworkPointsNeededToViewTechTree) then
		Controls.ButtonDiplomatViewTechScreen:SetDisabled(false);
		Controls.ButtonDiplomatViewTechScreen:SetToolTipString("");
		Controls.ButtonDiplomatViewTechScreen:RegisterCallback(Mouse.eLClick, OnViewTechScreenClicked);
	else
		Controls.ButtonDiplomatViewTechScreen:SetDisabled(true);
		Controls.ButtonDiplomatViewTechScreen:SetToolTipString(Locale.Lookup("TXT_KEY_VIEW_TECH_SCREEN_NOT_ENOUGH_NP", iNetworkPointsNeededToViewTechTree));
	end
	if(iNetworkPointsNeededToViewPolicyTree == 999999) then
		Controls.ButtonDiplomatViewPolicyScreen:SetHide(true);
	elseif (spy.MaxNetworkPointsStored >= iNetworkPointsNeededToViewPolicyTree) then
		Controls.ButtonDiplomatViewPolicyScreen:SetDisabled(false);
		Controls.ButtonDiplomatViewPolicyScreen:SetToolTipString("");
		Controls.ButtonDiplomatViewPolicyScreen:RegisterCallback(Mouse.eLClick, OnViewPolicyScreenClicked);
	else
		Controls.ButtonDiplomatViewPolicyScreen:SetDisabled(true);
		Controls.ButtonDiplomatViewPolicyScreen:SetToolTipString(Locale.Lookup("TXT_KEY_EO_VIEW_POLICY_SCREEN_NOT_ENOUGH_NP", iNetworkPointsNeededToViewPolicyTree));
	end
	if(iNetworkPointsNeededToViewUnitList == 999999) then
		Controls.ButtonDiplomatViewUnitList:SetHide(true);
	elseif (spy.MaxNetworkPointsStored >= iNetworkPointsNeededToViewUnitList) then
		Controls.ButtonDiplomatViewUnitList:SetDisabled(false);
		Controls.ButtonDiplomatViewUnitList:SetToolTipString("");
		Controls.ButtonDiplomatViewUnitList:RegisterCallback(Mouse.eLClick, OnViewUnitListClicked);
	else
		Controls.ButtonDiplomatViewUnitList:SetDisabled(true);
		Controls.ButtonDiplomatViewUnitList:SetToolTipString(Locale.Lookup("TXT_KEY_EO_VIEW_UNIT_LIST_NOT_ENOUGH_NP", iNetworkPointsNeededToViewUnitList));
	end
	if(iNetworkPointsNeededToViewTradeDeals == 999999) then
		Controls.ButtonDiplomatViewTradeDeals:SetHide(true);
	elseif (spy.MaxNetworkPointsStored >= iNetworkPointsNeededToViewTradeDeals) then
		Controls.ButtonDiplomatViewTradeDeals:SetDisabled(false);
		Controls.ButtonDiplomatViewTradeDeals:SetToolTipString("");
		Controls.ButtonDiplomatViewTradeDeals:RegisterCallback(Mouse.eLClick, OnViewTradeDealsClicked);
	else
		Controls.ButtonDiplomatViewTradeDeals:SetDisabled(true);
		Controls.ButtonDiplomatViewTradeDeals:SetToolTipString(Locale.Lookup("TXT_KEY_EO_VIEW_TRADE_DEALS_NOT_ENOUGH_NP", iNetworkPointsNeededToViewTradeDeals));
	end
	
	----------------------------------------------------------------        
	-- build the passive bonuses list
	----------------------------------------------------------------
	
	local buttonSizeY = 53
	for row in GameInfo.SpyPassiveBonusesDiplomat() do

		local instance = g_DiplomatBonusesManager:GetInstance();

		local szDescString;
		local szHelpString;
		local iNPScaled = GetNetworkPointsScaled(row);
		szDescString = Locale.Lookup("TXT_KEY_EO_PASSIVE_BONUS_NP_REQUIRED", iNPScaled, Locale.Lookup(row.Description));
		szHelpString = Locale.Lookup(row.Help);

		instance.Button:SetToolTipString(Locale.Lookup("TXT_KEY_EO_PASSIVE_BONUS_THRESHOLD_TT", szHelpString));
		
		-- Readjust the offset
		local sizeYDiff = math.max((instance.Name:GetSizeY()-buttonSizeY),1)
		if sizeYDiff > 1 then sizeYDiff = sizeYDiff + 20 end
		instance.Base:SetSizeY(buttonSizeY + sizeYDiff)
		instance.Button:SetSizeY(buttonSizeY + sizeYDiff)
		instance.Button:SetDisabled(true);
		
		if(iNPScaled > spy.MaxNetworkPointsStored) then
			instance.Name:SetText(szDescString);
			instance.Name:SetAlpha(0.2)
		else
			instance.Name:SetText(szDescString);
			instance.Name:SetAlpha(1)
		end
	
		count = count + 1;
	end
	return count;
end

CommitCounterspyFocus = function(selection, playerID, pCity, spyID)
	if(pCity ~= nil) then
		for i,v in ipairs(selection) do
			local eventChoiceType = v[1];
			local eventChoice = GameInfo.CityEventChoices[eventChoiceType];
			if(eventChoice ~= nil) then
				local pPlayer = Players[playerID];
				pCity:DoCityEventChoice(eventChoice.ID, spyID, playerID);
				break;
			end
		end
	end
end

CommitMissionSelection = function(selection, playerID, pCity, spyID)
	if(pCity ~= nil) then
		for i,v in ipairs(selection) do
			local eventChoiceType = v[1];
			local eventChoice = GameInfo.CityEventChoices[eventChoiceType];
			if(eventChoice ~= nil) then
				local pPlayer = Players[playerID];
				pCity:DoCityEventChoice(eventChoice.ID, spyID, playerID);
				Controls.MissionSelectionPopup:SetHide(true);
				RefreshAgents();
				break;
			end
		end
	end
end
----------------------------------------------------------------
function RefreshIntrigue()
	local pActivePlayer = Players[Game.GetActivePlayer()];
	local intrigueMessages = pActivePlayer:GetIntrigueMessages();

	g_IntrigueManager:ResetInstances();

	if(#intrigueMessages == 0) then
		Controls.IntrigueMessageHeaders:SetHide(true);
		Controls.IntrigueMessageScrollPanel:SetHide(true);
		Controls.NoIntrigueAvailable:SetHide(false);
	else

		local sortedMessages = {};

		for i,v in ipairs(intrigueMessages) do
			local strLeaderName = "Unknown player";
			local pPlayer = Players[v.DiscoveringPlayer];
			local iPortraitIndex = -1;
			local strIconAtlas = "";

			if (pPlayer) then
				local dpCivType = pPlayer:GetCivilizationType();
				local dpCivInfo = GameInfo.Civilizations[dpCivType];
				iPortraitIndex = dpCivInfo.PortraitIndex;
				strIconAtlas = dpCivInfo.IconAtlas;
				if(pPlayer:GetNickName() ~= "" and Game:IsNetworkMultiPlayer()) then
					strLeaderName = pPlayer:GetNickName();
				else
					strLeaderName = pPlayer:GetName();
				end
			end

			sortedMessages[i] = {
				DiscoveringPlayer = v.DiscoveringPlayer;
				Turn = v.Turn,
				From = strLeaderName,
				FromPortraitIndex = iPortraitIndex;
				FromIconAtlas = strIconAtlas,
				Message = v.String,
				SpyName = v.SpyName;
			}
		end

		table.sort(sortedMessages, g_IntrigueSortFunction);

		Controls.IntrigueMessageHeaders:SetHide(false);
		Controls.IntrigueMessageScrollPanel:SetHide(false);
		Controls.NoIntrigueAvailable:SetHide(true);

		local bTickTock = true;
		for i,v in ipairs(sortedMessages) do

			local instance = g_IntrigueManager:GetInstance();
			instance.Base:SetColorVal(unpack(bTickTock and g_PianoKeys[0] or g_PianoKeys[1]));
			instance.TurnNum:SetText(v.Turn);
			instance.Message:SetString(v.Message);

			local width, height = instance.Message:GetSizeVal();
			local baseWidth, baseHeight = instance.Base:GetSizeVal();
			instance.Base:SetSizeVal(baseWidth, height + 22);
			if (v.DiscoveringPlayer == Game.GetActivePlayer()) then
				instance.CivIcon:SetHide(true);
				instance.CivilizationName:LocalizeAndSetText(v.SpyName);
				instance.SpyIcon:SetHide(false);
			else
				IconHookup(v.FromPortraitIndex, 32, v.FromIconAtlas, instance.CivIcon);
				instance.CivIcon:SetHide(false);
				instance.CivilizationName:SetText(v.From);
				instance.SpyIcon:SetHide(true);
			end

			bTickTock = not bTickTock;
		end
	end

	Controls.IntrigueMessageStack:CalculateSize();
	Controls.IntrigueMessageStack:ReprocessAnchoring();
	Controls.IntrigueMessageScrollPanel:CalculateInternalSize();
end
-------------------------------------------------------------------------------
-- Sorting Support
-------------------------------------------------------------------------------
function AlphabeticalSortFunction(field, direction)
	if(direction == "asc") then
		return function(a,b)
			return Locale.Compare(a[field], b[field]) == -1;
		end
	elseif(direction == "desc") then
		return function(a,b)
			return Locale.Compare(a[field], b[field]) == 1;
		end
	end
end

function NumericSortFunction(field, direction)
	if(direction == "asc") then
		return function(a,b)
			local va = (a ~= nil and a[field] ~= nil) and a[field] or -1;
			local vb = (b ~= nil and b[field] ~= nil) and b[field] or -1;

			return tonumber(va) < tonumber(vb);
		end
	elseif(direction == "desc") then
		return function(a,b)
			local va = (a ~= nil and a[field] ~= nil) and a[field] or -1;
			local vb = (b ~= nil and b[field] ~= nil) and b[field] or -1;

			return tonumber(vb) < tonumber(va);
		end
	end
end

-- Registers the sort option controls click events
function RegisterSortOptions()

	for i,v in ipairs(g_AgentsSortOptions) do
		if(v.Button ~= nil) then
			v.Button:RegisterCallback(Mouse.eLClick, function() AgentsSortOptionSelected(v); end);
		end
	end
	
	for i,v in ipairs(g_UnitListSortOptions) do
		if(v.Button ~= nil) then
			v.Button:RegisterCallback(Mouse.eLClick, function() UnitListSortOptionSelected(v); end);
		end
	end

	for i,v in ipairs(g_YourCitiesSortOptions) do
		if(v.Button ~= nil) then
			v.Button:RegisterCallback(Mouse.eLClick, function() YourCitiesSortOptionSelected(v); end);
		end
	end

	for i,v in ipairs(g_TheirCitiesSortOptions) do
		if(v.Button ~= nil) then
			v.Button:RegisterCallback(Mouse.eLClick, function() TheirCitiesSortOptionSelected(v); end);
		end
	end

	for i,v in ipairs(g_IntrigueSortOptions) do
		if(v.Button ~= nil) then
			v.Button:RegisterCallback(Mouse.eLClick, function() IntrigueSortOptionSelected(v); end);
		end
	end
		
	UpdateSortOptionsDisplay(g_AgentsSortOptions);
	UpdateSortOptionsDisplay(g_UnitListSortOptions);
	UpdateSortOptionsDisplay(g_YourCitiesSortOptions);
	UpdateSortOptionsDisplay(g_TheirCitiesSortOptions);
	UpdateSortOptionsDisplay(g_IntrigueSortOptions);

	g_AgentsSortFunction = GetSortFunction(g_AgentsSortOptions);
	g_UnitListSortFunction = GetSortFunction(g_UnitListSortOptions);
	g_YourCitiesSortFunction = GetSortFunction(g_YourCitiesSortOptions);
	g_TheirCitiesSortFunction = GetSortFunction(g_TheirCitiesSortOptions);
	g_IntrigueSortFunction = GetSortFunction(g_IntrigueSortOptions);

end

function GetSortFunction(sortOptions)
	local orderBy = nil;
	for i,v in ipairs(sortOptions) do
		if(v.CurrentDirection ~= nil) then
			if(v.SortType == "numeric") then
				return NumericSortFunction(v.Column, v.CurrentDirection);
			else
				return AlphabeticalSortFunction(v.Column, v.CurrentDirection);
			end
		end
	end

	return nil;
end

-- Updates the sort option structure
function UpdateSortOptionState(sortOptions, selectedOption)
	-- Current behavior is to only have 1 sort option enabled at a time
	-- though the rest of the structure is built to support multiple in the future.
	-- If a sort option was selected that wasn't already selected, use the default
	-- direction.  Otherwise, toggle to the other direction.
	for i,v in ipairs(sortOptions) do
		if(v == selectedOption) then
			if(v.CurrentDirection == nil) then
				v.CurrentDirection = v.DefaultDirection;
			else
				if(v.CurrentDirection == "asc") then
					v.CurrentDirection = "desc";
				else
					v.CurrentDirection = "asc";
				end
			end
		else
			v.CurrentDirection = nil;
		end
	end
end

-- Updates the control states of sort options
function UpdateSortOptionsDisplay(sortOptions)
	for i,v in ipairs(sortOptions) do
		local imageControl = v.ImageControl;
		if(imageControl ~= nil) then
			if(v.CurrentDirection == nil) then
				imageControl:SetHide(true);
			else
				local imageToUse = "SelectedUp.dds";
				if(v.CurrentDirection == "desc") then
					imageToUse = "SelectedDown.dds";
				end
				imageControl:SetTexture(imageToUse);

				imageControl:SetHide(false);
			end
		end
	end
end

-- Callback for when sort options are selected.
function AgentsSortOptionSelected(option)
	local sortOptions = g_AgentsSortOptions;
	UpdateSortOptionState(sortOptions, option);
	UpdateSortOptionsDisplay(sortOptions);
	g_AgentsSortFunction = GetSortFunction(sortOptions);

	RefreshAgents();
end

-- Callback for when sort options are selected.
function TheirCitiesSortOptionSelected(option)
	local sortOptions = g_TheirCitiesSortOptions;
	UpdateSortOptionState(sortOptions, option);
	UpdateSortOptionsDisplay(sortOptions);
	g_TheirCitiesSortFunction = GetSortFunction(sortOptions);

	RefreshTheirCities(g_SelectedAgentID, g_SelectedCity and g_SelectedCity:GetOwner() or nil, g_SelectedCity and g_SelectedCity:GetID() or nil, g_CitiesAvailableToRelocate);
end

-- Callback for when sort options are selected.
function YourCitiesSortOptionSelected(option)
	local sortOptions = g_YourCitiesSortOptions;
	UpdateSortOptionState(sortOptions, option);
	UpdateSortOptionsDisplay(sortOptions);
	g_YourCitiesSortFunction = GetSortFunction(sortOptions);


	RefreshMyCities(g_SelectedAgentID, g_SelectedCity and g_SelectedCity:GetOwner() or nil, g_SelectedCity and g_SelectedCity:GetID() or nil, g_CitiesAvailableToRelocate);
end

-- Callback for when sort options are selected.
function UnitListSortOptionSelected(option)
	local sortOptions = g_UnitListSortOptions;
	UpdateSortOptionState(sortOptions, option);
	UpdateSortOptionsDisplay(sortOptions);
	g_UnitListSortFunction = GetSortFunction(sortOptions);

	RefreshUnitList();
end

function IntrigueSortOptionSelected(option)
	local sortOptions = g_IntrigueSortOptions;
	UpdateSortOptionState(sortOptions, option);
	UpdateSortOptionsDisplay(sortOptions);
	g_IntrigueSortFunction = GetSortFunction(sortOptions);

	RefreshIntrigue();
end

----------------------------------------------------------------
-- Copied from CityState LUA
----------------------------------------------------------------
function getProtectingPlayers(iMinorCivID)
	local sProtecting = "";

	for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
		pOtherPlayer = Players[iPlayerLoop];

		--if (iPlayerLoop ~= Game.GetActivePlayer()) then
			if (pOtherPlayer:IsAlive()) then
				if (pOtherPlayer:IsProtectingMinor(iMinorCivID)) then
					if (sProtecting ~= "") then
						sProtecting = sProtecting .. ", "
					end

					sProtecting = sProtecting .. Locale.ConvertTextKey(Players[iPlayerLoop]:GetCivilizationShortDescriptionKey());
				end
			end
		--end
	end

	return sProtecting
end

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(OnClose);

-----------------------------------------------------------------
-- Add Espionage Overview to Dropdown
-----------------------------------------------------------------
LuaEvents.AdditionalInformationDropdownGatherEntries.Add(function(entries)
	if (not Game.IsOption("GAMEOPTION_NO_ESPIONAGE")) then
		table.insert(entries, {
			text = Locale.Lookup("TXT_KEY_EO_TITLE"),
			call = function()
				Events.SerialEventGameMessagePopup{
					Type = ButtonPopupTypes.BUTTONPOPUP_ESPIONAGE_OVERVIEW,
				};
			end,
		});

	end
end);

--------------------------------------------------------------------
function HandleNotificationAdded(notificationId, notificationType, toolTip, summary, gameValue, extraGameData)
	--If this UI is displayed and the notification is related to staging a coup, popup the status.
	if(ContextPtr:IsHidden() == false) then
		if(notificationType == NotificationTypes.NOTIFICATION_SPY_YOU_STAGE_COUP_FAILURE or notificationType == NotificationTypes.NOTIFICATION_SPY_YOU_STAGE_COUP_SUCCESS) then

			local text = summary .. "[NEWLINE][NEWLINE]" .. toolTip;
			Controls.NotificationText:SetText(text);
			Controls.NotificationContent:CalculateSize();
			local width, height = Controls.NotificationContent:GetSizeVal();
			Controls.NotificationFrame:SetSizeVal(width + 60, height + 140);
			Controls.NotificationPopup:SetHide(false);
		end
	end
end
Events.NotificationAdded.Add(HandleNotificationAdded);

function OnOK()
	Controls.NotificationPopup:SetHide(true);
	TabSelect(g_CurrentTab);
end
Controls.OK:RegisterCallback(Mouse.eLClick, OnOK);

function OnCloseCounterspyFocusButton()
	Controls.CounterspyFocusPopup:SetHide(true);
	TabSelect(g_CurrentTab);
end
Controls.CloseCounterspyFocusButton:RegisterCallback(Mouse.eLClick, OnCloseCounterspyFocusButton);

function OnCloseUnitListButton()
	Controls.UnitListPopup:SetHide(true);
end
Controls.CloseUnitListButton:RegisterCallback(Mouse.eLClick, OnCloseUnitListButton);

function OnCloseDiplomatButton()
	Controls.DiplomatPopup:SetHide(true);
	TabSelect(g_CurrentTab);
end
Controls.CloseDiplomatButton:RegisterCallback(Mouse.eLClick, OnCloseDiplomatButton);

function OnCloseMissionSelectionButton()
	Controls.MissionSelectionPopup:SetHide(true);
	TabSelect(g_CurrentTab);
end

function OnCloseDiplomatTradeButton()
	Controls.DiplomatTradeDealsPopup:SetHide(true);
	Controls.DealsPanel:SetHide( true );
end

Controls.CloseMissionSelectionButton:RegisterCallback(Mouse.eLClick, OnCloseMissionSelectionButton);


-- Just in case :)
LuaEvents.RequestRefreshAdditionalInformationDropdownEntries();

RegisterSortOptions();
TabSelect("Overview");