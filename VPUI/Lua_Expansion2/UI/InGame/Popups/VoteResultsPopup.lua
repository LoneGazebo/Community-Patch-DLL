if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Diplo Vote Results Popup
-------------------------------------------------
include( "IconSupport" );
include( "SupportFunctions"  );
include( "InstanceManager" );

local g_PlayerListInstanceManager = InstanceManager:new( "PlayerEntryInstance", "PlayerEntryBox", Controls.PlayerListStack );
local m_PopupInfo = nil;

g_bIsTeamGame = false;

-------------------------------------------------
-- AddPlayerEntry
-------------------------------------------------
function AddTeamEntry (iTeam, iVotes, iRank)
	
	local pTeam = Teams[iTeam];
	local pPlayer = Players[pTeam:GetLeaderID()];
	local activePlayer = Players[Game.GetActivePlayer()];
	
	-- Update colors
	--local primaryColor, secondaryColor = pPlayer:GetPlayerColors();
	local backgroundColor = nil; --{x = secondaryColor.x, y = secondaryColor.y, z = secondaryColor.z, w = 1};
	--local textColor = {x = primaryColor.x, y = primaryColor.y, z = primaryColor.z, w = 1};

	local controlTable = g_PlayerListInstanceManager:GetInstance();
	
    local leader = GameInfo.Leaders[pPlayer:GetLeaderType()];
    local leaderDescription = leader.Description;

	
	local strName = "";
	-- No rank for Minors
	if (not pTeam:IsMinorCiv()) then
		strName = Locale.ConvertTextKey("TXT_KEY_PROGRESS_SCREEN_NUMBERING", iRank);
	end
	
	local bColorizeBox = true;
	
	-- Rank
	
	-- Active player
	if ((pPlayer:GetID() == activePlayer:GetID()) or (activePlayer:GetTeam() == pTeam:GetID())) then
		strName = strName .. " " .. Locale.ConvertTextKey( g_bIsTeamGame and "TXT_KEY_POP_VOTE_RESULTS_YOUR_TEAM" or "TXT_KEY_POP_VOTE_RESULTS_YOU" );
		
		IconHookup( leader.PortraitIndex, 64, leader.IconAtlas, controlTable.Portrait );
		CivIconHookup( pPlayer:GetID(), 32, controlTable.Icon, controlTable.IconBG, controlTable.IconShadow, false, true );
		
	-- Haven't yet met this player
	elseif (not pTeam:IsHasMet(Game.GetActiveTeam())) then
		strName = strName .. " " .. Locale.ConvertTextKey( g_bIsTeamGame and "TXT_KEY_POP_VOTE_RESULTS_UNMET_TEAM" or "TXT_KEY_POP_VOTE_RESULTS_UNMET_PLAYER" );
		if(not pTeam:IsMinorCiv()) then
			IconHookup( 22, 64, "LEADER_ATLAS", controlTable.Portrait );
			CivIconHookup( -1, 32, controlTable.Icon, controlTable.IconBG, controlTable.IconShadow, false, true );
        else
			questionOffset, questionTextureSheet = IconLookup( 23, 64, "CIV_COLOR_ATLAS" );
			if questionOffset ~= nil then       
				controlTable.Portrait:SetTexture( questionTextureSheet );
				controlTable.Portrait:SetTextureOffset( questionOffset );
			end
			controlTable.Icon:SetHide(true);
			controlTable.IconBG:SetHide(true);
			controlTable.IconShadow:SetHide(true);
        end
		bColorizeBox = false;
		
	-- Met players
	else
		
		if(not g_bIsTeamGame or pTeam:IsMinorCiv()) then
			if(pPlayer:GetNickName() ~= "" and Game:IsNetworkMultiPlayer()) then
				strName = strName .. " " .. pPlayer:GetNickName();
			else
				strName = strName .. " " .. Locale.ConvertTextKey(pPlayer:GetNameKey());
			end
		else
			strName = strName .. " " .. Locale.ConvertTextKey("TXT_KEY_POP_UN_TEAM_LABEL", pTeam:GetID() + 1);
		end

		if(not pTeam:IsMinorCiv()) then
			IconHookup( leader.PortraitIndex, 64, leader.IconAtlas, controlTable.Portrait );
			CivIconHookup( pPlayer:GetID(), 32, controlTable.Icon, controlTable.IconBG, controlTable.IconShadow, false, true);
		else
			-- Update colors
			local primaryColor, secondaryColor = pPlayer:GetPlayerColors();
			primaryColor, secondaryColor = secondaryColor, primaryColor;
			local textColor = {x = primaryColor.x, y = primaryColor.y, z = primaryColor.z, w = 1};
			backgroundColor = textColor;
			
			local civType = pPlayer:GetCivilizationType();
			local civInfo = GameInfo.Civilizations[civType];
			IconHookup( civInfo.PortraitIndex, 64, civInfo.AlphaIconAtlas, controlTable.Portrait );
			controlTable.Portrait:SetColor(textColor);
			controlTable.Icon:SetHide(true);
			controlTable.IconBG:SetHide(true);
			controlTable.IconShadow:SetHide(true);
		end
	end
	
	---------------------
	-- Vote Cast
	---------------------
	
	local strVote;
	
	local iVoteCast = Game.GetVoteCast(iTeam);
	local pVoteCastTeam = Teams[iVoteCast];
	local iVoteCastPlayer = pVoteCastTeam:GetLeaderID();
	local pVoteCastPlayer = Players[iVoteCastPlayer];
	local strVoteCastTT = "";
	
	-- Minor voted for himself - make the field blank
	if (pTeam:IsMinorCiv() and iTeam == iVoteCast) then
		strVote = Locale.ConvertTextKey("TXT_KEY_ABSTAIN");
		controlTable.Icon:SetHide(true);
		controlTable.Icon:SetHide(true);
		controlTable.IconBG:SetHide(true);
		controlTable.IconShadow:SetHide(true);
		controlTable.BallotIcon:SetHide(true);
		controlTable.BallotIconBG:SetHide(true);
		controlTable.BallotIconShadow:SetHide(true);
		
	-- Active player was voted for
	elseif (pVoteCastPlayer:GetID() == Game.GetActivePlayer()) then
		strVote = Locale.ConvertTextKey( g_bIsTeamGame and "TXT_KEY_POP_VOTE_RESULTS_YOUR_TEAM" or "TXT_KEY_POP_VOTE_RESULTS_YOU" );
		CivIconHookup( pVoteCastTeam:GetLeaderID(), 32, controlTable.BallotIcon, controlTable.BallotIconBG, controlTable.BallotIconShadow, false, true );
		
		-- Tooltip
		if (Teams[Game.GetActiveTeam()]:IsHasMet(iTeam)) then
			if (pTeam:IsMinorCiv()) then	
				strVoteCastTT = Locale.ConvertTextKey("TXT_KEY_POP_UN_ELEC_VOTE_TT_CITYSTATE_ALT", pPlayer:GetCivilizationShortDescriptionKey(), pVoteCastPlayer:GetCivilizationShortDescriptionKey());
			else
				strVoteCastTT = Locale.ConvertTextKey("TXT_KEY_POP_UN_ELEC_VOTE_TT_CIV_ALT", pPlayer:GetCivilizationShortDescriptionKey(), pVoteCastPlayer:GetCivilizationShortDescriptionKey());
			end
		end
		
	-- Haven't yet met the team voted for
	elseif (not pVoteCastTeam:IsHasMet(Game.GetActiveTeam())) then
		strVote = Locale.ConvertTextKey( g_bIsTeamGame and "TXT_KEY_POP_VOTE_RESULTS_UNMET_TEAM" or "TXT_KEY_POP_VOTE_RESULTS_UNMET_PLAYER" );
		CivIconHookup( -1, 32, controlTable.BallotIcon, controlTable.BallotIconBG, controlTable.BallotIconShadow, false, true );
		
	-- Have met team voted for
	else
		if(not g_bIsTeamGame) then
			if(pPlayer:GetNickName() ~= "" and Game:IsNetworkMultiPlayer()) then
				strVote = pVoteCastPlayer:GetNickName();
			else
				strVote = Locale.ConvertTextKey(pVoteCastPlayer:GetNameKey());
			end
		else
				strVote = Locale.ConvertTextKey("TXT_KEY_POP_UN_TEAM_LABEL", pVoteCastTeam:GetID() + 1);
		end
		
		-- Tooltip
		if (Teams[Game.GetActiveTeam()]:IsHasMet(iTeam)) then
			if (pTeam:IsMinorCiv()) then	
				strVoteCastTT = Locale.ConvertTextKey("TXT_KEY_POP_UN_ELEC_VOTE_TT_CITYSTATE_ALT", pPlayer:GetCivilizationShortDescriptionKey(), pVoteCastPlayer:GetCivilizationShortDescriptionKey());
			else
				strVoteCastTT = Locale.ConvertTextKey("TXT_KEY_POP_UN_ELEC_VOTE_TT_CIV_ALT", pPlayer:GetCivilizationShortDescriptionKey(), pVoteCastPlayer:GetCivilizationShortDescriptionKey());
			end
		end
		
		-- Civ icon
		if(not pTeam:IsMinorCiv()) then
			CivIconHookup( pVoteCastTeam:GetLeaderID(), 32, controlTable.BallotIcon, controlTable.BallotIconBG, controlTable.BallotIconShadow, false, true );
		else
			local civType = pVoteCastPlayer:GetCivilizationType();
			local civInfo = GameInfo.Civilizations[civType];
			CivIconHookup( pVoteCastTeam:GetLeaderID(), 32, controlTable.BallotIcon, controlTable.BallotIconBG, controlTable.BallotIconShadow, false, true );
		end
	end
	
	local unSize = 0;
	-- Home of the UN? (asterisk after vote)
	controlTable.UNIcon:SetHide(true);
	--[[
	if (pTeam:IsHomeOfUnitedNations()) then
		controlTable.UNIcon:SetHide(false);
		unSize = controlTable.UNIcon:GetSizeX();
	end
	--]]

	-- Don't use Team Color unless we know this player
	if (bColorizeBox) then
		if(backgroundColor ~= nil) then
			controlTable.PlayerNameText:SetColor(backgroundColor, 0);
		end
	else
		controlTable.PlayerNameText:SetColorByName("Beige_Black_Alpha");
		backgroundColor = {x = 1, y = 1, z = 1, w = 1};
	end

	TruncateString(controlTable.PlayerNameText, controlTable.PlayerNameBox:GetSizeX(), strName);
	
	-- Clear out vote count for minors receiving 0 votes
	if (iVotes == 0 and pTeam:IsMinorCiv()) then
		iVotes = " ";
	end
	controlTable.VotesText:SetText(iVotes);
	
	TruncateString(controlTable.VoteCastText, controlTable.VoteCastBox:GetSizeX(), strVote);
	controlTable.VoteCastBox:EnableToolTip(false);
	if (strVoteCastTT ~= "" and not pTeam:IsHuman()) then
		controlTable.VoteCastBox:EnableToolTip(true);
		controlTable.VoteCastBox:SetToolTipString(strVoteCastTT);
	end
end


-------------------------------------------------
-- On Popup
-------------------------------------------------
function OnPopup( popupInfo )

	if( popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_VOTE_RESULTS ) then
		return;
	end
	
	g_PlayerListInstanceManager:ResetInstances();
	m_PopupInfo = popupInfo;
	
	-- Results of a preliminary election
	local bPreliminaryElection = popupInfo.Option1;

    -- Votes to Win Label
    Controls.VotesNeededLabel:SetHide(true);
    Controls.PrelimElectionLabel:SetHide(true);
    local iVotesToWin = Game.GetVotesNeededForDiploVictory();
	local strVotesToWinString = Locale.ConvertTextKey("TXT_KEY_VP_DIPLO_VOTES_NEEDED") .. " " .. iVotesToWin;
    if (not bPreliminaryElection) then	
		Controls.VotesNeededLabel:SetText(strVotesToWinString);
		Controls.VotesNeededLabel:SetHide(false);
	else
		local strPrelimMessage = Locale.ConvertTextKey("TXT_KEY_POP_UN_ELEC_PRELIMINARY_ALT") .. " " .. strVotesToWinString;
		Controls.PrelimElectionLabel:SetText(strPrelimMessage);
		Controls.PrelimElectionLabel:SetHide(false);
	end
    
    --local iPlayer = Game.GetActivePlayer();
	--local pPlayer = Players[iPlayer];
	--local pTeam = Teams[pPlayer:GetTeam()];
	
	local pLoopTeam;
	
	local iVotes;
	
	local tTeams = {};
	
	-- Loop through all players
	for iTeamLoop = 0, GameDefines.MAX_CIV_TEAMS-1, 1 do
		
		pLoopTeam = Teams[iTeamLoop];
		
		-- Valid Team?
		if (pLoopTeam:IsAlive()) then
			
			if(pLoopTeam:GetNumMembers() > 1) then
				g_bIsTeamGame = true;
			end
			
			iVotes = Game.GetNumVotesForTeam(iTeamLoop);
			
			-- If it's a minor then zero out their votes (they can't win anyways, and this only clutters up the screen)
			if (pLoopTeam:IsMinorCiv()) then
				iVotes = 0;
			end
			
			table.insert(tTeams, {iTeamLoop, iVotes});
		end
	end
	
	-- Sort our list of teams
	local f = function(a, b)
		-- For ties, major civ ranks higher in our display list
		if (a[2] == b[2]) then
			local iMajorStatusA = 1;
			local iMajorStatusB = 1;
			if (Teams[a[1]]:IsMinorCiv()) then
				iMajorStatusA = 0;
			end
			if (Teams[b[1]]:IsMinorCiv()) then
				iMajorStatusB = 0;
			end
			return iMajorStatusA > iMajorStatusB;
		end
		
		return a[2] > b[2];
	end
	
	table.sort(tTeams, f);
	
	if(g_bIsTeamGame) then
		Controls.PlayerHeader:LocalizeAndSetText("TXT_KEY_POP_UN_TEAM");
	else
		Controls.PlayerHeader:LocalizeAndSetText("TXT_KEY_POP_UN_PLAYER");
	end
	
	-- Popup sorted list
	for i, v in ipairs(tTeams) do
		AddTeamEntry(v[1], v[2], i);		-- TeamID, Votes, Rank
	end
	
	Controls.PlayerListStack:CalculateSize();
	Controls.PlayerListStack:ReprocessAnchoring();
	Controls.PlayerListScrollPanel:CalculateInternalSize();

	UIManager:QueuePopup( ContextPtr, PopupPriority.VoteResults );
end
Events.SerialEventGameMessagePopup.Add( OnPopup );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnClose()
    UIManager:DequeuePopup( ContextPtr );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose);


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
            OnClose();
            return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )

    if( not bInitState ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
        else
            UI.decTurnTimerSemaphore();
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_VOTE_RESULTS, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(OnClose);
