if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
--------------------------------------------------------------
-- LeagueProjectPopup.lua
-- Author: Anton Strenger
-- DateCreated: 1/16/2012
--
-- Shows information about the results of a League Project
--------------------------------------------------------------

include( "InstanceManager" );
include( "IconSupport" );


-----------------------------
-- Globals --
-----------------------------
local m_bDebug = false;
local m_PopupInfo = nil;
local m_iLeague = nil;
local m_iProject = nil;

local m_PlayerRankInstanceManager = InstanceManager:new( "PlayerEntryInstance", "PlayerEntryBox", Controls.PlayerListStack );
local m_RewardTierInstanceManager = InstanceManager:new( "RewardTierInstance", "RewardTierBox", Controls.PlayerListStack );

local m_tContributions = {}; --[iLeague][iProject][iPlayer]


-------------------------------------------------
-- General UI Handlers
-------------------------------------------------
function OnPopup( popupInfo )

	if( popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_LEAGUE_PROJECT_COMPLETED ) then
		return;
	end
	
	m_PopupInfo = popupInfo;
	m_iLeague = popupInfo.Data1;
	m_iProject = popupInfo.Data2;
	
	UpdateContributions();
	UpdateAll();

	UIManager:QueuePopup( ContextPtr, PopupPriority.WhosWinning );
end
Events.SerialEventGameMessagePopup.Add( OnPopup );

function UpdateContributions()
	for iLeagueLoop = 0, Game.GetNumLeaguesEverFounded()-1, 1 do
		local pLeague = Game.GetLeague(iLeagueLoop);
		if (pLeague ~= nil) then
			if (m_tContributions[iLeagueLoop] == nil) then
				m_tContributions[iLeagueLoop] = {};
			end
			
			for tProjectLoop in GameInfo.LeagueProjects() do
				local iProjectLoop = tProjectLoop.ID;
				if (m_tContributions[iLeagueLoop][iProjectLoop] == nil) then
					m_tContributions[iLeagueLoop][iProjectLoop] = {};
				end
				
				if (pLeague:IsProjectActive(iProjectLoop) or pLeague:IsProjectComplete(iProjectLoop)) then
					for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
						if (m_tContributions[iLeagueLoop][iProjectLoop][iPlayerLoop] == nil) then
							m_tContributions[iLeagueLoop][iProjectLoop][iPlayerLoop] =
							{
								Contribution = 0,
								Tier = 0,
							};
						end
						
						local iContributionTimes100 = pLeague:GetMemberContribution(iPlayerLoop, iProjectLoop);
						m_tContributions[iLeagueLoop][iProjectLoop][iPlayerLoop].Contribution = iContributionTimes100 / 100;
						
						local iTier = pLeague:GetMemberContributionTier(iPlayerLoop, iProjectLoop);
						m_tContributions[iLeagueLoop][iProjectLoop][iPlayerLoop].Tier = iTier;
					end
				else
					m_tContributions[iLeagueLoop][iProjectLoop] = {};
				end
			end
		end
	end
end

function UpdateAll()
	if (m_iLeague ~= nil and m_iProject ~= nil) then
		local pLeague = Game.GetLeague(m_iLeague);
		if (pLeague ~= nil) then
			local tProject = GameInfo.LeagueProjects[m_iProject];
			m_PlayerRankInstanceManager:ResetInstances();
			m_RewardTierInstanceManager:ResetInstances();
			
			-- Background image
			if (tProject.ProjectSplashImage ~= nil) then
				Controls.BackgroundImage:SetTextureAndResize(tProject.ProjectSplashImage);
			end
			
			-- Project Info
			Controls.PresentsLabel:LocalizeAndSetText(GameInfo.Processes[tProject.Process].Description);
			Controls.ListNameLabel:LocalizeAndSetText("TXT_KEY_LEAGUE_PROJECT_POPUP_COMPLETE");
			
			local sProjectInfo = Locale.Lookup(tProject.Help);
			sProjectInfo = sProjectInfo .. "[NEWLINE][NEWLINE]";
			sProjectInfo = sProjectInfo .. pLeague:GetProjectDetails(tProject.ID);
			Controls.ProjectInfoLabel:SetText(sProjectInfo);
			
			-- Loop through all majors
			local tRankings = {};
			for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
				local pPlayerLoop = Players[iPlayerLoop];
				local iContribution = 0;
				local iTier = 0;
				if (pPlayerLoop ~= nil and pPlayerLoop:IsAlive()) then
					if (m_tContributions[m_iLeague] ~= nil) then
						if (m_tContributions[m_iLeague][m_iProject] ~= nil) then
							if (m_tContributions[m_iLeague][m_iProject][iPlayerLoop] ~= nil) then
								iContribution = m_tContributions[m_iLeague][m_iProject][iPlayerLoop].Contribution;
								iTier = m_tContributions[m_iLeague][m_iProject][iPlayerLoop].Tier;
							end
						end
					end
					table.insert(tRankings, {iPlayerLoop, iContribution, iTier});
				end
			end
			
			-- Sort based on contribution amount
			local f = function(a, b)
				if (a[3] == b[3]) then
					return a[2] > b[2];
				else
					return a[3] > b[3];
				end
			end
			table.sort(tRankings, f);
			
			-- Display sorted list
			local iDisplayTier = 4;
			for i, v in ipairs(tRankings) do
				local iPlayer = v[1];
				local iContribution = v[2];
				local iTier = v[3];
				if (m_bDebug) then print("==debug== Showing player rankings, found Player=" .. iPlayer .. ", Contribution=" .. iContribution .. ", Tier=" .. iTier); end
				AddPlayerEntry(iPlayer, iContribution, iTier, i);
			end
			
			Controls.PlayerListStack:CalculateSize();
			Controls.PlayerListStack:ReprocessAnchoring();
			Controls.PlayerListScrollPanel:CalculateInternalSize();
		end
	end
end

function OnClose ()
    UIManager:DequeuePopup( ContextPtr );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose );
Events.GameplaySetActivePlayer.Add(OnClose);

function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
            OnClose();
            return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );

function ShowHideHandler( bIsHide, bInitState )
    if( not bInitState ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
        else
            UI.decTurnTimerSemaphore();
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_LEAGUE_PROJECT_COMPLETED, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

-------------------------------------------------
-- AddRewardTier
-------------------------------------------------
function AddRewardTier (iRewardTier)
	local tProject = GameInfo.LeagueProjects[m_iProject];
	if (tProject ~= nil) then
		local controlTable = m_RewardTierInstanceManager:GetInstance();
		local sText = "";
		local sTooltip = "";
		
		if (iRewardTier > 3 or iRewardTier < 0) then
			return;
		elseif (iRewardTier == 0) then
			sText = Locale.Lookup("TXT_KEY_LEAGUE_PROJECT_REWARD_NONE");
			sTooltip = Locale.Lookup("TXT_KEY_LEAGUE_PROJECT_REWARD_NONE_HELP");
		else
			sText = Locale.Lookup(GameInfo.LeagueProjectRewards[tProject["RewardTier" .. iRewardTier]].Description);
			sTooltip = Locale.Lookup(GameInfo.LeagueProjectRewards[tProject["RewardTier" .. iRewardTier]].Description);
			-- Includes all previous tiers
			for i = 1, iRewardTier, 1 do
				sTooltip = sTooltip .. "[NEWLINE][ICON_BULLET]";
				sTooltip = sTooltip .. Locale.Lookup(GameInfo.LeagueProjectRewards[tProject["RewardTier" .. i]].Help);
			end
		end
		
		controlTable.RewardTierLabel:SetText(sText);
		controlTable.RewardTierBox:SetToolTipString(sTooltip);
	end
end

-------------------------------------------------
-- AddPlayerEntry
-------------------------------------------------
function AddPlayerEntry (iPlayerID, iScore, iTier, iRank)
	if (m_iLeague ~= nil and m_iProject ~= nil) then
		local pLeague = Game.GetLeague(m_iLeague);
		if (pLeague ~= nil) then
			local pPlayer = Players[iPlayerID];
			local iTeam = pPlayer:GetTeam();
			local pTeam = Teams[iTeam];

			local controlTable = m_PlayerRankInstanceManager:GetInstance();
			local leader = GameInfo.Leaders[pPlayer:GetLeaderType()];
			local strName = "";
			
			-- Active player
			if (pPlayer:GetID() == Game.GetActivePlayer()) then
				strName = Locale.ConvertTextKey( "TXT_KEY_POP_VOTE_RESULTS_YOU" );
				CivIconHookup( iPlayerID, 32, controlTable.CivIcon, controlTable.CivIconBG, controlTable.CivIconShadow, true, true, controlTable.CivIconHighlight);  
				IconHookup( leader.PortraitIndex, 64, leader.IconAtlas, controlTable.Portrait );
			-- Haven't yet met this player
			elseif (not pTeam:IsHasMet(Game.GetActiveTeam())) then
				strName = Locale.ConvertTextKey( "TXT_KEY_POP_VOTE_RESULTS_UNMET_PLAYER" );
				CivIconHookup( -1, 32, controlTable.CivIcon, controlTable.CivIconBG, controlTable.CivIconShadow, true, true, controlTable.CivIconHighlight);
				IconHookup( 22, 64, "LEADER_ATLAS", controlTable.Portrait );
			-- Met players
			else
				strName = Locale.ConvertTextKey(pPlayer:GetNameKey());
				CivIconHookup( iPlayerID, 32, controlTable.CivIcon, controlTable.CivIconBG, controlTable.CivIconShadow, true, true, controlTable.CivIconHighlight);  
				IconHookup( leader.PortraitIndex, 64, leader.IconAtlas, controlTable.Portrait );
			end 
			
			controlTable.PlayerNameText:SetText(strName);
			
			-- Formats the # and reduces it to X.XX
			local strPoints = Locale.ConvertTextKey("TXT_KEY_FORMAT_NUMBER", iScore) .. "[ICON_PRODUCTION]";
			controlTable.ScoreText:SetText(strPoints);
			
			-- Reward tier icons
			local strRewardIcons = "";
			local strRewardTT = "";
			if (iTier >= 1) then
				strRewardIcons = strRewardIcons .. "[ICON_TROPHY_BRONZE]";
				strRewardTT = pLeague:GetProjectRewardTierDetails(1, m_iProject) .. strRewardTT;
			end
			if (iTier >= 2) then
				strRewardIcons = strRewardIcons .. "[ICON_TROPHY_SILVER]";
				strRewardTT = pLeague:GetProjectRewardTierDetails(2, m_iProject) .. "[NEWLINE][NEWLINE]" .. strRewardTT;
			end
			if (iTier >= 3) then
				strRewardIcons = strRewardIcons .. "[ICON_TROPHY_GOLD]";
				strRewardTT = pLeague:GetProjectRewardTierDetails(3, m_iProject) .. "[NEWLINE][NEWLINE]" .. strRewardTT;
			end
			controlTable.RewardIconsText:SetText(strRewardIcons);
			controlTable.RewardIconsText:SetToolTipString(strRewardTT);
		end
	end
end
