print("This is the modded EspionageOverview from CBP")
-------------------------------------------------
-- Espionage Overview Popup
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
include( "CityStateStatusHelper" );


local g_AgentManager = GenerationalInstanceManager:new( "AgentInstance", "Base", Controls.AgentStack);
local g_MyCityManager = GenerationalInstanceManager:new( "MyCityInstance", "Base", Controls.MyCityStack);
local g_MyCityButtonManager = GenerationalInstanceManager:new("MyCityButtonInstance", "Base", Controls.MyCityStack);
local g_TheirCityManager = GenerationalInstanceManager:new( "TheirCityInstance", "Base", Controls.TheirCityStack);
local g_TheirCityButtonManager = GenerationalInstanceManager:new( "TheirCityButtonInstance", "Base", Controls.TheirCityStack);
local g_IntrigueManager = GenerationalInstanceManager:new( "IntrigueMessageInstance", "Base", Controls.IntrigueMessageStack);


local g_PreviousLocationBoxColor = "231,213,0,255";

-- Used for Piano Keys
local g_PianoKeys = {
	[0] = {19/255,32/255,46/255,120/255},
	[1] = {12/255,22/255,30/255,120/255},
}

-- Texture offsets based on rank.
local g_RankOffsets = {
		TXT_KEY_SPY_RANK_0 = {x = 0,y = 42},
		TXT_KEY_SPY_RANK_1 = {x = 0,y = 21},
		TXT_KEY_SPY_RANK_2 = {x = 0,y = 0}
};
	
-- Progressbar state information based on agent activity.
local g_ProgressBarStates = {
	TXT_KEY_SPY_STATE_TRAVELLING = {
		IconOffset = {x = 45,y = 45},
		ProgressBarTexture = "MeterBarGreatEspionageGreen.dds",
	},
	TXT_KEY_SPY_STATE_SURVEILLANCE = {
		IconOffset = {x = 45,y = 0},
		ProgressBarTexture = "MeterBarGreatEspionageBlue.dds",
	},
	TXT_KEY_SPY_STATE_GATHERING_INTEL = {
		IconOffset = {x = 45, y = 0},
		ProgressBarTexture = "MeterBarGreatPersonGold.dds",
	},
	TXT_KEY_SPY_STATE_COUNTER_INTEL = {
		IconOffset = {x = 45,y = 0},
		ProgressBarTexture = "MeterBarGreatPersonGold.dds",
	},
	TXT_KEY_SPY_STATE_RIGGING_ELECTION = {
		IconOffset = {x = 45, y = 90},
		ProgressBarTexture = "MeterBarGreatPersonGold.dds",
	},
	TXT_KEY_SPY_STATE_MAKING_INTRODUCTIONS = {
		IconOffset = {x = 45,y = 0},
		ProgressBarTexture = "MeterBarGreatEspionageBlue.dds",
	},
	TXT_KEY_SPY_STATE_PREPARING_HEIST = {
		IconOffset = {x = 45,y = 0},
		ProgressBarTexture = "MeterBarGreatEspionageGreen.dds",
	},
}
	
-- Agent text color based on agent activity.
local g_TextColors = {
	TXT_KEY_SPY_STATE_TRAVELLING           = {x =  94/255, y = 237/255, z = 105/255, w = 255/255},
	TXT_KEY_SPY_STATE_SURVEILLANCE         = {x = 128/255, y = 150/255, z = 228/255, w = 255/255},
	TXT_KEY_SPY_STATE_GATHERING_INTEL      = {x = 255/255, y = 222/255, z =   9/255, w = 255/255},
	TXT_KEY_SPY_STATE_COUNTER_INTEL		   = {x = 255/255, y = 222/255, z =   9/255, w = 255/255},
	TXT_KEY_SPY_STATE_RIGGING_ELECTION	   = {x = 255/255, y = 222/255, z =   9/255, w = 255/255},
	TXT_KEY_SPY_STATE_MAKING_INTRODUCTIONS = {x = 128/255, y = 150/255, z = 228/255, w = 255/255},
	TXT_KEY_SPY_STATE_SCHMOOZING		   = {x = 255/255, y = 222/255, z =   9/255, w = 255/255},
	TXT_KEY_SPY_STATE_PREPARING_HEIST	   = {x = 128/255, y = 150/255, z = 228/255, w = 255/255},
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

g_AgentsSortFunction = nil;
g_YourCitiesSortFunction = nil;
g_TheirCitiesSortFunction = nil;
g_IntrigueSortFunction = nil;

g_CurrentTab = nil;		-- The currently selected Tab.

-- slewis - added this so that we can sort while an agent is able to move. They are reset in refresh
g_SelectedAgentID = nil;
g_SelectedCity = nil;
g_CitiesAvailableToRelocate = nil;
-- end slewis

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
			if(Controls.ChooseConfirm:IsHidden())then
	            OnClose();
	        else
				Controls.ChooseConfirm:SetHide(true);
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
Controls.CancelButton:RegisterCallback(Mouse.eLClick, function() Refresh(); end);
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
	local rankOffsets = g_RankOffsets;
	local progressBarStates = g_ProgressBarStates;
		
		
	if (agent.State == "TXT_KEY_SPY_STATE_SURVEILLANCE" and agent.TurnsLeft < 0) then
		agent.AgentActivity = Locale.Lookup("TXT_KEY_SPY_STATE_ESTABLISHED_SURVEILLANCE");
	else
		agent.AgentActivity = Locale.Lookup(agent.State);	
	end		

	Controls.AgentRank:SetTextureOffset(rankOffsets[agent.Rank]);	
	Controls.AgentName:LocalizeAndSetText(agent.Name);
	Controls.AgentName:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_NAMEPLATE_TT", agent.Rank, agent.Name); 
	
	local szSpyRankTooltip = activePlayer:GetInfluenceSpyRankTooltip (agent.Name, agent.Rank, -1, OptionsManager.IsNoBasicHelp(), agent.AgentID);
	Controls.AgentRank:SetToolTipString(szSpyRankTooltip);
	Controls.AgentIcon:SetToolTipString(szSpyRankTooltip);
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
	
			szSpyRankTooltip = activePlayer:GetInfluenceSpyRankTooltip (agent.Name, agent.Rank, plot:GetOwner(), OptionsManager.IsNoBasicHelp(), agent.AgentID);
			Controls.AgentRank:SetToolTipString(szSpyRankTooltip);
			Controls.AgentIcon:SetToolTipString(szSpyRankTooltip);
		end
	end

	local bHideActivityInfo = progressBarStates[agent.State] == nil;

	Controls.AgentActivity:SetText(agent.AgentActivity);

	local strActivityTT = Locale.Lookup("TXT_KEY_EO_SPY_NEEDS_ASSIGNMENT_TT");
	local city = nil;
	if(plot ~= nil) then
		city = plot:GetPlotCity();
		if(city ~= nil) then
			strActivityTT = activePlayer:GetSpyChanceAtCity(city, agent.AgentID, OptionsManager.IsNoBasicHelp());
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
			local nextprogresspct = progresspct + (1 - progresspct)/agent.TurnsLeft;
			Controls.AgentActivityProgress:SetPercents( progresspct, nextprogresspct );						
		else 
			Controls.AgentActivityProgress:SetHide(true);
			Controls.AgentActivityProgressFrame:SetHide(true);
			Controls.AgentActivityProgressBack:SetHide(true);
		end
		
		if (agent.TurnsLeft >= 0) then
			Controls.AgentProgress:LocalizeAndSetText("TXT_KEY_STR_TURNS", agent.TurnsLeft);
		else
			if (agent.State == "TXT_KEY_SPY_STATE_SURVEILLANCE") then
				Controls.AgentProgress:LocalizeAndSetText("TXT_KEY_SPY_STATE_ESTABLISHED_SURVEILLANCE_PROGRESS_BAR");
			else
				Controls.AgentProgress:SetText("");			
			end
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
	local rankOffsets = g_RankOffsets;
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
		
		agentEntry.AgentRank:SetTextureOffset(rankOffsets[v.Rank]);	
		agentEntry.AgentName:LocalizeAndSetText(v.Name);
		agentEntry.AgentName:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_NAMEPLATE_TT", v.Rank, v.Name); 
		local szSpyRankTooltip = pActivePlayer:GetInfluenceSpyRankTooltip (v.Name, v.Rank, -1, OptionsManager.IsNoBasicHelp(), v.AgentID);
		agentEntry.AgentRank:SetToolTipString(szSpyRankTooltip);
		
		if (v.IsDiplomat) then
			agentEntry.DiplomatIcon:SetToolTipString(szSpyRankTooltip);
			agentEntry.DiplomatIcon:SetHide(false);
			agentEntry.AgentIcon:SetHide(true);
			agentEntry.ThiefIcon:SetHide(true);
		elseif(v.IsThief)then
			agentEntry.ThiefIcon:SetToolTipString(szSpyRankTooltip);
			agentEntry.ThiefIcon:SetHide(false);
			agentEntry.AgentIcon:SetHide(true);
			agentEntry.DiplomatIcon:SetHide(true);
		else
			agentEntry.AgentIcon:SetToolTipString(szSpyRankTooltip);
			agentEntry.AgentIcon:SetHide(false);
			agentEntry.DiplomatIcon:SetHide(true);
			agentEntry.ThiefIcon:SetHide(true);
		end
		
		
		if(v.State == "TXT_KEY_SPY_STATE_DEAD") then
			agentEntry.AgentLocationActionsPanel:SetHide(true);
			agentEntry.AgentActivityPanel:SetHide(true);
			agentEntry.AgentKIAPanel:SetHide(false);
			
			agentEntry.Base:SetColorVal(0,0,0,0);
			bTickTock = not bTickTock;
			
		else
			agentEntry.AgentLocationActionsPanel:SetHide(false);
			agentEntry.AgentActivityPanel:SetHide(false);
			agentEntry.AgentKIAPanel:SetHide(true);
			
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
		
					szSpyRankTooltip = pActivePlayer:GetInfluenceSpyRankTooltip (v.Name, v.Rank, plot:GetOwner(), OptionsManager.IsNoBasicHelp(), v.AgentID);
					agentEntry.AgentRank:SetToolTipString(szSpyRankTooltip);
					agentEntry.AgentIcon:SetToolTipString(szSpyRankTooltip);
				end
			end
		
			local bHideActivityInfo = progressBarStates[v.State] == nil;

			agentEntry.AgentActivity:SetText(v.AgentActivity);
			
			local strActivityTT = Locale.Lookup("TXT_KEY_EO_SPY_NEEDS_ASSIGNMENT_TT");
			local city = nil;
			if(plot ~= nil) then
				city = plot:GetPlotCity();
				if(city ~= nil) then
					strActivityTT = pActivePlayer:GetSpyChanceAtCity(city, v.AgentID, OptionsManager.IsNoBasicHelp());
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
					local nextprogresspct = progresspct + (1 - progresspct)/v.TurnsLeft;
					agentEntry.AgentActivityProgress:SetPercents( progresspct, nextprogresspct );						
				else 
					agentEntry.AgentActivityProgress:SetHide(true);
					agentEntry.AgentActivityProgressFrame:SetHide(true);
					agentEntry.AgentActivityProgressBack:SetHide(true);
				end
				
				
				if (v.TurnsLeft >= 0) then
					agentEntry.AgentProgress:LocalizeAndSetText("TXT_KEY_STR_TURNS", v.TurnsLeft);
				else
					if (v.State == "TXT_KEY_SPY_STATE_SURVEILLANCE") then
						agentEntry.AgentProgress:LocalizeAndSetText("TXT_KEY_SPY_STATE_ESTABLISHED_SURVEILLANCE_PROGRESS_BAR");
					else
						agentEntry.AgentProgress:SetText("");			
					end
				end		
			else
				agentEntry.AgentProgress:SetText("");
			end
			
			agentEntry.RelocateButton:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_MOVE_TT", v.Rank, v.Name);
			agentEntry.RelocateButton:SetDisabled(v.State == "TXT_KEY_SPY_STATE_DEAD");
			agentEntry.RelocateButton:RegisterCallback(Mouse.eLClick, function()
				RelocateAgent(v.AgentID, city);
			end);
			
			
			-- CITY SCREEN CLOSED - Don't look, Marc
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
		
		
			local OnCoupClicked =  function()		
				g_ConfirmAction = function()
					Network.SendStageCoup(Game.GetActivePlayer(), v.AgentID);
				end
				g_DenyAction = nil;
				
				if (city ~= nil) then
					local iCityAlly = Players[city:GetOwner()]:GetAlly();
					Controls.ConfirmText:LocalizeAndSetText("TXT_KEY_EO_STAGE_COUP_QUESTION", v.Rank, v.Name, city:GetNameKey(), Players[iCityAlly]:GetCivilizationAdjectiveKey(), Players[Game.GetActivePlayer()]:GetCoupChanceOfSuccess(city), city:GetNameKey(), Players[iCityAlly]:GetCivilizationDescriptionKey(), city:GetNameKey(), v.Rank, v.Name, city:GetNameKey());
					Controls.ConfirmContent:CalculateSize();
					local width, height = Controls.ConfirmContent:GetSizeVal();
					Controls.ConfirmFrame:SetSizeVal(width + 60, height + 120);
					Controls.ChooseConfirm:SetHide(false);
					Controls.YesString:LocalizeAndSetText("TXT_KEY_YES_BUTTON");
					Controls.NoString:LocalizeAndSetText("TXT_KEY_NO_BUTTON");
				end
			end;
			
			if (city and Players[city:GetOwner()]:IsMinorCiv()) then
				local bCoupDisabled = not pActivePlayer:CanSpyStageCoup(v.AgentID) or v.State == "TXT_KEY_SPY_STATE_DEAD";
				agentEntry.StageCoupButton:SetDisabled(bCoupDisabled);
				agentEntry.StageCoupButton:RegisterCallback(Mouse.eLClick, OnCoupClicked);
				local iCityAlly = Players[city:GetOwner()]:GetAlly();
-- CP
				local iCooldown = Players[city:GetOwner()]:GetCoupCooldown();
--END
				if (v.State == "TXT_KEY_SPY_STATE_DEAD") then
					agentEntry.StageCoupButton:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_BUTTON_DISABLED_SPY_DEAD_TT", v.Rank, v.Name);
				elseif (bCoupDisabled) then
					if (not pActivePlayer:HasSpyEstablishedSurveillance(v.AgentID)) then
						agentEntry.StageCoupButton:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_COUP_DISABLED_WAIT_TT", v.Rank, v.Name, city:GetNameKey());
					elseif (iCityAlly == -1) then
						agentEntry.StageCoupButton:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_COUP_DISABLED_NO_ALLY_TT", v.Rank, v.Name, city:GetNameKey(), city:GetNameKey());
-- CBP
					elseif(iCooldown > 0) then
						agentEntry.StageCoupButton:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_COUP_DISABLED_COOLDOWN_TT", v.Rank, v.Name, city:GetNameKey(), iCooldown);

					elseif(Players[city:GetOwner()]:GetPermanentAlly() ~= -1 and Players[city:GetOwner()]:GetPermanentAlly() ~= Game.GetActivePlayer()) then
						agentEntry.StageCoupButton:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_COUP_DISABLED_PERMA_TT", v.Rank, v.Name, city:GetNameKey());
-- END
					else
						agentEntry.StageCoupButton:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_COUP_DISABLED_YOU_ALLY_TT", v.Rank, v.Name, city:GetNameKey());
					end
				else
					agentEntry.StageCoupButton:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_COUP_ENABLED_TT", v.Rank, v.Name, city:GetNameKey(), Players[iCityAlly]:GetCivilizationAdjectiveKey(), Players[Game.GetActivePlayer()]:GetCoupChanceOfSuccess(city), v.Rank, v.Name, city:GetNameKey(), Players[iCityAlly]:GetCivilizationShortDescriptionKey(), v.Rank, v.Name, city:GetNameKey());
				end
			
				agentEntry.ViewCityButton:SetHide(true);
				agentEntry.StageCoupButton:SetHide(false);	
			else
				local bIsDiplomat = pActivePlayer:IsSpyDiplomat(v.AgentID);
				local bIsSchmoozing = pActivePlayer:IsSpySchmoozing(v.AgentID);
				local bViewDisabled = not pActivePlayer:HasSpyEstablishedSurveillance(v.AgentID) or v.State == "TXT_KEY_SPY_STATE_DEAD"
				agentEntry.ViewCityButton:SetDisabled(bViewDisabled and not bIsSchmoozing);
				agentEntry.ViewCityButton:RegisterCallback(Mouse.eLClick, OnViewCityClicked);	
				if (v.State == "TXT_KEY_SPY_STATE_DEAD") then
					agentEntry.ViewCityButton:LocalizeAndSetToolTip("TXT_KEY_EO_SPY_BUTTON_DISABLED_SPY_DEAD_TT", v.Rank, v.Name);
				elseif (city) then
					if (bIsDiplomat and not bIsSchmoozing) then
						agentEntry.ViewCityButton:LocalizeAndSetToolTip("TXT_KEY_EO_CITY_VIEW_DISABLED_DIPLOMAT_WAIT_TT", city:GetNameKey());
					elseif (city:GetOwner() == Game.GetActivePlayer()) then
						agentEntry.ViewCityButton:LocalizeAndSetToolTip("TXT_KEY_EO_CITY_VIEW_DISABLED_YOUR_CITY_TT", city:GetNameKey());						
					elseif (bViewDisabled and not bIsSchmoozing) then
						agentEntry.ViewCityButton:LocalizeAndSetToolTip("TXT_KEY_EO_CITY_VIEW_DISABLED_WAIT_TT", city:GetNameKey());
					elseif (bIsSchmoozing) then
						agentEntry.ViewCityButton:LocalizeAndSetToolTip("TXT_KEY_EO_CITY_VIEW_ENABLED_DIPOLOMAT_TT", city:GetNameKey());
					else
						agentEntry.ViewCityButton:LocalizeAndSetToolTip("TXT_KEY_EO_CITY_VIEW_ENABLED_TT", city:GetNameKey());
					end
				else
					agentEntry.ViewCityButton:LocalizeAndSetToolTip("TXT_KEY_EO_CITY_VIEW_DISABLED_NO_CITY_TT");
				end
				
				agentEntry.ViewCityButton:SetHide(false);
				agentEntry.StageCoupButton:SetHide(true);		
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

	print("Refreshing My Cities");
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
		
		local strPotentialToolTip = Locale.Lookup("TXT_KEY_EO_UNKNOWN_POTENTIAL_TT");
		
		if(cityInfo.BasePotential > 0) then
			strPotentialToolTip = pActivePlayer:GetCityPotentialInfo(pCity, OptionsManager.IsNoBasicHelp());
		
			local potentialMeter = entry.PotentialMeterFront;
			potentialMeter:SetHide(false);
			
			local pct = cityInfo.BasePotential/cityInfo.LargestBasePotential;
			potentialMeter:SetPercents(pct,pct);
		end
		
		entry.PotentialMeterBack:SetToolTipString(strPotentialToolTip);
		entry.PotentialMeterFront:SetToolTipString(strPotentialToolTip);
		

		entry.CivIcon:SetToolTipString(strCityCivToolTip);
		entry.CivilizationName:SetToolTipString(strCityCivToolTip);
		entry.CityName:SetToolTipString(strCityNameToolTip);		
		entry.CityPopulation:SetText(cityInfo.Population);
		entry.CityPopulation:SetAlpha(1.0);
		entry.CityPopulation:LocalizeAndSetToolTip("TXT_KEY_EO_CITY_POPULATION_TT", cityInfo.Name, cityInfo.Population);
		
		entry.ViewCityIcon:SetHide(true);

		if(agent ~= nil) then
			local textColor = textColors[agent.State];
			entry.CivilizationName:SetColor(textColor, 0);
			entry.CityName:SetColor(textColor, 0);
			entry.CityPopulation:SetColor(textColor, 0);
			entry.SpyInCityIcon:LocalizeAndSetToolTip("TXT_KEY_SPY_OCCUPYING_CITY", agent.Name);
			entry.SpyInCityIcon:SetHide(false);
-- CBP EDIT
			entry.CapitalCityIcon:SetHide(true);
			entry.CapitalCityIcon:SetToolTipString(nil);
-- END
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
						local iCityAlly = Players[pCity:GetOwner()]:GetAlly();
						if(iCityAlly ~= -1) then
							entry.AllyName:SetHide(false);
							entry.AllyIcon:SetHide(false);
							entry.AllyIconBG:SetHide(false);
							entry.AllyIconShadow:SetHide(false);
							entry.AllyIconHighlight:SetHide(false);
							CivIconHookup(iCityAlly, 32, entry.AllyIcon, entry.AllyIconBG, entry.AllyIconShadow, false, true, entry.AllyIconHighlight);
							strAllyToolTip = Locale.Lookup("TXT_KEY_ALLY_CBP_SPY", Players[iCityAlly]:GetCivilizationDescriptionKey());
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
			if(cityInfo.BasePotential > 0) then
				entry.PotentialMeterBack:SetHide(false);	
				local potentialMeter;
				if (agent ~= nil and cityInfo.Potential > 0) then
					potentialMeter = entry.PotentialMeterFront;
				else
					potentialMeter = entry.PotentialMeterGhost;
				end

				potentialMeter:SetHide(false);
				
				local pct = cityInfo.BasePotential/cityInfo.LargestBasePotential;
				potentialMeter:SetPercents(pct,pct);
			else
				entry.UnknownProgress:SetHide(false);
			end
		end
	
		local strCityNameToolTip = Locale.Lookup("TXT_KEY_EO_CITY_NAME_TT", cityInfo.Name, cityInfo.CivilizationNameKey);
		local strCityCivToolTip = Locale.Lookup("TXT_KEY_EO_CITY_CIV_TT", cityInfo.Name, cityInfo.CivilizationNameKey);
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
			--CBP
			if(cityInfo ~= nil) then
				local pPlayerCS = Players[cityInfo.PlayerID];
				if (pPlayerCS ~= -1 and pPlayerCS:IsMinorCiv()) then
					local iCooldown = pPlayerCS:GetCoupCooldown();
					if(iCooldown > 0) then
						strCityStateTT = strCityStateTT .. "[NEWLINE][NEWLINE]";
						strCityStateTT = strCityStateTT .. Locale.Lookup("TXT_KEY_EO_SPY_COUP_DISABLED_COOLDOWN_TT_EXTRA", iCooldown);
						entry.RigElectionEnabled:SetHide(true);
						entry.RigElectionDisabled:SetHide(true);
						entry.RigElectionCooldown:SetHide(false);
					end
				end
			end
			--END
			entry.RigElectionDisabled:SetToolTipString(strCityStateTT);
			entry.RigElectionEnabled:SetToolTipString(strCityStateTT);
			--CBP
			entry.RigElectionCooldown:SetToolTipString(strCityStateTT);
			--END
		else
			local strPotentialToolTip = Locale.Lookup("TXT_KEY_EO_UNKNOWN_POTENTIAL_TT");
			if(cityInfo.BasePotential > 0) then
				if (agent ~= nil and agent.EstablishedSurveillance) then
					if (cityInfo.Potential > 0) then
						local pPlayer = Players[cityInfo.PlayerID];
						local pCity = pPlayer:GetCityByID(cityInfo.CityID);	
						strPotentialToolTip = Players[Game.GetActivePlayer()]:GetCityPotentialInfo(pCity, OptionsManager.IsNoBasicHelp());
					else
						strPotentialToolTip = Locale.Lookup("TXT_KEY_EO_CITY_POTENTIAL_CANNOT_STEAL_TT", agent.Rank, agent.Name, cityInfo.Name, cityInfo.Name, cityInfo.BasePotential);
					end
				else
					if(not OptionsManager.IsNoBasicHelp()) then 
						strPotentialToolTip = Locale.Lookup("TXT_KEY_EO_CITY_ONCE_KNOWN_POTENTIAL_TT", cityInfo.Name, cityInfo.BasePotential);
					else
						strPotentialToolTip = Locale.Lookup("TXT_KEY_EO_CITY_ONCE_KNOWN_POTENTIAL_TT_SHORT", cityInfo.Name, cityInfo.BasePotential);
					end
				end
				entry.PotentialMeterBack:SetToolTipString(strPotentialToolTip);
				entry.PotentialMeterGhost:SetToolTipString(strPotentialToolTip);
				entry.PotentialMeterFront:SetToolTipString(strPotentialToolTip);
			else
				entry.UnknownProgress:SetToolTipString(strPotentialToolTip);
			end
		end

		entry.CivIcon:SetToolTipString(strCityCivToolTip);
		entry.CivilizationName:SetToolTipString(strCityCivToolTip);
		entry.CityName:SetToolTipString(strCityNameToolTip);
		entry.CityPopulation:SetText(cityInfo.Population);
		entry.CityPopulation:SetAlpha(1.0);
		entry.CityPopulation:LocalizeAndSetToolTip("TXT_KEY_EO_CITY_POPULATION_TT", cityInfo.Name, cityInfo.Population);
				
		if(agent ~= nil) then
			local textColor = textColors[agent.State];
			print(textColor);
			print(agent.State);
			entry.CivilizationName:SetColor(textColor, 0);
			entry.CityName:SetColor(textColor, 0);
			entry.CityPopulation:SetColor(textColor, 0);

			entry.DiplomatInCityIcon:SetHide(true);			
			entry.SpyInCityIcon:SetHide(true);
			
			if (agent.IsDiplomat) then
				entry.DiplomatInCityIcon:LocalizeAndSetToolTip("TXT_KEY_SPY_OCCUPYING_CITY", agent.Name);
				entry.DiplomatInCityIcon:SetHide(false);
-- CBP EDIT
				entry.CapitalCityIcon:SetHide(true);
				entry.CapitalCityIcon:SetToolTipString(nil);
				entry.AllyIcon:SetHide(true);
				entry.AllyIconBG:SetHide(true);
				entry.AllyIconShadow:SetHide(true);
				entry.AllyIconHighlight:SetHide(true);
				entry.AllyName:SetHide(true);

-- END		
			else
				entry.SpyInCityIcon:LocalizeAndSetToolTip("TXT_KEY_SPY_OCCUPYING_CITY", agent.Name);
				entry.SpyInCityIcon:SetHide(false);
-- CBP EDIT
				entry.CapitalCityIcon:SetHide(true);
				entry.CapitalCityIcon:SetToolTipString(nil);
				entry.AllyIcon:SetHide(true);
				entry.AllyIconBG:SetHide(true);
				entry.AllyIconShadow:SetHide(true);
				entry.AllyIconHighlight:SetHide(true);
				entry.AllyName:SetHide(true);
-- END
			end
			
			if (cityInfo.PlayerID == Game.GetActivePlayer()) then
				entry.DisabledViewCityIcon:SetHide(false);
				entry.ViewCityIcon:SetHide(true);
				entry.DisabledViewCityIcon:LocalizeAndSetToolTip("TXT_KEY_EO_CITY_VIEW_DISABLED_YOUR_CITY_TT", cityInfo.Name);				
			elseif (agent.EstablishedSurveillance) then
				entry.DisabledViewCityIcon:SetHide(true);
				entry.ViewCityIcon:SetHide(false);
				entry.ViewCityIcon:LocalizeAndSetToolTip("TXT_KEY_EO_CITY_VIEW_ENABLED_TT", cityInfo.Name);	
				
				local plot = Map.GetPlot(cityInfo.CityX, cityInfo.CityY);
				local city = plot:GetPlotCity();
				
				-- Initialize 'View City' button.
				-- CITY SCREEN CLOSED - Don't look, Marc
				local CityScreenClosed = function()
					UIManager:DequeuePopup(Controls.EmptyPopup);
					--print("This CityScreenClosed was called!");
					Events.SerialEventExitCityScreen.Remove(CityScreenClosed);
					UI.SetCityScreenViewingMode(false);
				end
				
				local OnViewCityClicked = function()
					
					if (city ~= nil) then
						--print("Attempting to show city screen");
						-- Queue up an empty popup at a higher priority so that it prevents other cities from appearing while we're looking at this one!
						UIManager:QueuePopup(Controls.EmptyPopup, PopupPriority.GenericPopup+1);
						Events.SerialEventExitCityScreen.Add(CityScreenClosed);
						UI.SetCityScreenViewingMode(true);
						UI.DoSelectCityAtPlot( city:Plot() );
					else
						print("city == null");
					end
				end
				
				entry.ViewCityIcon:RegisterCallback(Mouse.eLClick, OnViewCityClicked);
			else
				entry.DisabledViewCityIcon:SetHide(false);
				entry.ViewCityIcon:SetHide(true);
				entry.DisabledViewCityIcon:LocalizeAndSetToolTip("TXT_KEY_EO_CITY_VIEW_DISABLED_WAIT_TT", cityInfo.Name);	
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
			
			entry.DisabledViewCityIcon:LocalizeAndSetToolTip("TXT_KEY_EO_CITY_VIEW_DISABLED_TT", cityInfo.Name);
			entry.DisabledViewCityIcon:SetHide(false);
			entry.ViewCityIcon:SetHide(true);
		end
		
		if(bIsCityState == true) then
			entry.DisabledViewCityIcon:SetHide(true);
			entry.ViewCityIcon:SetHide(true);
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

			--CBP
			local bCheckThief = false;
			if (pActivePlayer:ValidHeistLocation(selectedAgentIndex, city)) then
				bCheckDiplomat = true;
				bCheckThief = true;
			end
			--END

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

					if(bCheckThief)then			
						Controls.ConfirmText:LocalizeAndSetText("TXT_KEY_SPY_BE_THIEF");
						Controls.YesString:LocalizeAndSetText("TXT_KEY_DIPLOMAT_PICKER_THIEF");
					else
						Controls.ConfirmText:LocalizeAndSetText("TXT_KEY_SPY_BE_DIPLOMAT");
						Controls.YesString:LocalizeAndSetText("TXT_KEY_DIPLOMAT_PICKER_DIPLOMAT");					
					end
					Controls.NoString:LocalizeAndSetText("TXT_KEY_DIPLOMAT_PICKER_SPY");
					Controls.ConfirmContent:CalculateSize();
					local width, height = Controls.ConfirmContent:GetSizeVal();
					Controls.ConfirmFrame:SetSizeVal(width + 60, height + 120);
					Controls.ChooseConfirm:SetHide(false);
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
	UpdateSortOptionsDisplay(g_YourCitiesSortOptions);
	UpdateSortOptionsDisplay(g_TheirCitiesSortOptions);
	UpdateSortOptionsDisplay(g_IntrigueSortOptions);
	
	g_AgentsSortFunction = GetSortFunction(g_AgentsSortOptions);
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

function IntrigueSortOptionSelected(option)
	local sortOptions = g_IntrigueSortOptions;
	UpdateSortOptionState(sortOptions, option);
	UpdateSortOptionsDisplay(sortOptions);
	g_IntrigueSortFunction = GetSortFunction(sortOptions);
	
	RefreshIntrigue();
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
		if(notificationType == NotificationTypes.NOTIFICATION_SPY_YOU_STAGE_COUP_FAILURE) then
		
			local text = summary .. "[NEWLINE][NEWLINE]" .. toolTip;
			Controls.NotificationText:SetText(text); 
			Controls.NotificationContent:CalculateSize();
			local width, height = Controls.NotificationContent:GetSizeVal();
			Controls.NotificationFrame:SetSizeVal(width + 60, height + 140);
			Controls.NotificationPopup:SetHide(false);
		
		elseif(notificationType == NotificationTypes.NOTIFICATION_SPY_YOU_STAGE_COUP_SUCCESS) then
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




-- Just in case :)
LuaEvents.RequestRefreshAdditionalInformationDropdownEntries();

RegisterSortOptions();
TabSelect("Overview");