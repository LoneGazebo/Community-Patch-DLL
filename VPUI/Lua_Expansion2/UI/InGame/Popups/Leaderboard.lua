if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Leaderbroad Screen
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
include("SupportFunctions.lua");
include( "MapUtilities" );
----------------------------------------------------------------        
----------------------------------------------------------------      
local requestingScoresString = Locale.ConvertTextKey( "TXT_KEY_LB_REQUESTINGSCORES" ); 
local notSupportedByModString = Locale.ConvertTextKey( "TXT_KEY_LB_NOTSUPPORTEDBYMOD" );
local noLeaderboardString = Locale.ConvertTextKey( "TXT_KEY_LB_NOLEADERBOARD" ); 
local noScoresFriendsString = Locale.ConvertTextKey( "TXT_KEY_LB_NOSCORES_FRIENDS" ); 
local noScoresPersonalString = Locale.ConvertTextKey( "TXT_KEY_LB_NOSCORES_PERSONAL" ); 

local m_LeaderboardPulldownData = {};

local m_currentLeaderboardCategory = 1;		-- Current leaderboard category.  Cached in OnCategory.
local m_atLeaderboardTop = true;			-- Are we at the top of current leaderboard?
local m_atLeaderboardBottom = true;			-- Are we at the bottom of the current leaderboard.
  
local m_bIsEndGame = (ContextPtr:GetID() == "EndGameHallOfFame");

local g_GamesIM = InstanceManager:new( "GameInstance", "GameItem", Controls.GameStack );

local g_topDisplayedRank = -1;	-- The highest rank the highest rank in the current leaderboard data
local g_botDisplayedRank = -1;	-- The lowest rank the highest rank in the current leaderboard data

----------------------------------------------------------------    
-- Button Handlers    
----------------------------------------------------------------        
function OnBack()
	UIManager:DequeuePopup( ContextPtr );
end
Controls.BackButton:RegisterCallback( Mouse.eLClick, OnBack );

function OnCategory( which )
	-- Set Category Button hightlight
    Controls.FriendsHighlight:SetHide(  which ~= 1 );
    Controls.PersonalHighlight:SetHide( which ~= 2 );
    Controls.GlobalHighlight:SetHide( which ~= 3 );
    
    -- Set Leaderboard Category
    m_currentLeaderboardCategory = which;
    UI.SetLeaderboardCategory(which);
    g_topDisplayedRank = -1;
		g_botDisplayedRank = -1;
    RefreshScores();
end
Controls.FriendsButton:RegisterCallback( Mouse.eLClick, OnCategory );
Controls.PersonalButton:RegisterCallback( Mouse.eLClick, OnCategory );
Controls.GlobalButton:RegisterCallback( Mouse.eLClick, OnCategory );

Controls.FriendsButton:SetVoid1( 1 );
Controls.PersonalButton:SetVoid1( 2 );
Controls.GlobalButton:SetVoid1( 3 );

function OnLeaderboardUpEnd()
	-- Scrolled to top of current leaderboard data set.
	if(not m_atLeaderboardTop) then 
		-- We're not at the top of the current leaderboard, request the next higher set of leaderboard scores.
		UI.ScrollLeaderboardUp();
		RefreshScores();
	end
end
Controls.ScoreScrollPanel:RegisterUpEndCallback(OnLeaderboardUpEnd);

function OnLeaderboardDownEnd()
	-- Scrolled to bottom of current leaderboard data set.
	if(not m_atLeaderboardBottom) then 
		-- We're not at the bottom of the current leaderboard, request the next higher set of leaderboard scores.
		UI.ScrollLeaderboardDown();
		RefreshScores();
	end
end
Controls.ScoreScrollPanel:RegisterDownEndCallback(OnLeaderboardDownEnd);


----------------------------------------------------------------
-- Leaderboard Pulldown Selected
----------------------------------------------------------------
function OnLeaderboardPull( leaderboardIdx )
	Controls.LeaderboardPull:GetButton():LocalizeAndSetText( m_LeaderboardPulldownData[ leaderboardIdx ].Name );
	
	UI.SetLeaderboard(m_LeaderboardPulldownData[leaderboardIdx].ModID, m_LeaderboardPulldownData[leaderboardIdx].Name); -- Set new leaderboard
	RefreshScores(); -- Refresh scores for the new leaderboard
	
end
Controls.LeaderboardPull:RegisterSelectionCallback( OnLeaderboardPull );


---------------------------------------------------------------- 
-- Get the default leaderboard index
----------------------------------------------------------------
function GetDefaultLeaderboardIdx()
-- The default leaderboard is the leaderboard for the currently running dll.
	local curDllGUID = UI.GetDllGUID();
	for i, v in pairs(m_LeaderboardPulldownData) do
		if(curDllGUID == v.ModID) then
			return i;
		end
	end
	
	return 1;
end


----------------------------------------------------------------    
-- Builds the leaderboard pulldown options based on currently installed leaderboard dlc, mods, etc. 
----------------------------------------------------------------   
function BuildLeaderboardPulldown()
	m_LeaderboardPulldownData = UI.GetAvailableLeaderboards();
	
	Controls.LeaderboardPull:ClearEntries();
	local controlTable;
    for i, v in pairs(m_LeaderboardPulldownData) do
		print("Adding leaderboard to pulldown.  ModID: ", v.ModID, " Name: ", v.Name);
        controlTable = {};
        Controls.LeaderboardPull:BuildEntry( "InstanceOne", controlTable );
  
        controlTable.Button:LocalizeAndSetText( v.Name );
        controlTable.Button:SetVoid1( i );
    end
    Controls.LeaderboardPull:CalculateInternals();
    
	OnLeaderboardPull(GetDefaultLeaderboardIdx());
end


----------------------------------------------------------------        
-- Refresh Scores
----------------------------------------------------------------  
function RefreshScores()
	-- Request new leaderboard scores.  We will get a Leaderboard_ScoresDownloaded event when the request is finished (success or fail).
	UI.RequestLeaderboardScores();
	
	-- While refreshing scores, lock the scoll bar during the refresh by saying we're at the top and the bottom of the list.
	m_atLeaderboardTop = true;
	m_atLeaderboardBottom = true;	
		
	-- Show requesting data text.
	Controls.LeaderboardStatus:SetHide(false);
	Controls.LeaderboardStatus:SetText(requestingScoresString);
	
	-- Set leaderboard title name.
	--[[
	local leaderBoardTitle = UI.GetLeaderboardTitle();
	--print("leaderBoardTitle: " .. leaderBoardTitle);
	Controls.LeaderboardTitle:SetText(leaderBoardTitle);
	]]--
	
	g_GamesIM:ResetInstances();
end
Controls.RefreshButton:RegisterCallback( Mouse.eLClick, RefreshScores );


----------------------------------------------------------------        
---------------------------------------------------------------- 
function PopulateLeaderboardResults()
	g_GamesIM:ResetInstances();
	
	local gameEntries = {};
	local newTopRank = -1;
	local newBotRank = -1;
	
	local games = UI.GetLeaderboardScores();
	
	for i, v in pairs(games) do
	
		local controlTable = g_GamesIM:GetInstance();
		table.insert(gameEntries, controlTable);
		
		TruncateString( controlTable.LeaderName, 120, v.PlayerName );
		
		controlTable.GlobalRank:SetText(v.GlobalRank);
		if(newTopRank == -1 or v.GlobalRank < newTopRank) then
			newTopRank = v.GlobalRank;
		end
		if(newBotRank == -1 or v.GlobalRank > newBotRank) then
			newBotRank = v.GlobalRank;
		end
		
		controlTable.Score:SetText(v.Score);
		
		-- Setup civ and leader.
		local civ = GameInfo.Civilizations[v.Civ];
		if(civ ~= nil) then	
			controlTable.CivName:LocalizeAndSetText(civ.ShortDescription);
			IconHookup( civ.PortraitIndex, 32, civ.IconAtlas, controlTable.CivSymbol );
			
			local leader = GameInfo.Leaders[GameInfo.Civilization_Leaders( "CivilizationType = '" .. civ.Type .. "'" )().LeaderheadType];
			if(leader ~= nil) then		
				IconHookup( leader.PortraitIndex, 64, leader.IconAtlas, controlTable.LeaderPortrait );
			else
				IconHookup( 23, 64, "CIV_COLOR_ATLAS", controlTable.LeaderPortrait );
			end	
		else
			controlTable.CivName:LocalizeAndSetText("TXT_KEY_MISC_UNKNOWN");
			IconHookup( 23, 32, "CIV_COLOR_ATLAS", controlTable.CivSymbol );
			IconHookup( 23, 64, "CIV_COLOR_ATLAS", controlTable.LeaderPortrait );
		end
		
		-- Set game difficulty
		local info = GameInfo.HandicapInfos[ v.PlayerHandicapType ];
		if( info ~= nil) then
			IconHookup( info.PortraitIndex, 32, info.IconAtlas, controlTable.Difficulty );
			controlTable.Difficulty:SetToolTipString( Locale.ConvertTextKey( info.Description ) );	
		else
			controlTable.Difficulty:SetHide(true);
		end	
		
		-- Fill In Player Info
		if(v.PlayerTeamWon)then
			controlTable.DidWin:LocalizeAndSetText("TXT_KEY_VICTORY_BANG");
		else
			controlTable.DidWin:LocalizeAndSetText("TXT_KEY_DEFEAT_BANG");
		end
					
		-- Fill In Game Info
		local winnerCiv = nil;
		if(v.WinningTeamLeaderCivilizationType ~= nil) then
			winnerCiv = GameInfo.Civilizations[v.WinningTeamLeaderCivilizationType];
		end
		
		if(winnerCiv) then
			controlTable.WinningCivName:LocalizeAndSetText(winnerCiv.ShortDescription);
			IconHookup( winnerCiv.PortraitIndex, 64, winnerCiv.IconAtlas, controlTable.CivIconBG );
		else
			controlTable.WinningCivName:LocalizeAndSetText("TXT_KEY_MISC_UNKNOWN");
			IconHookup( 23, 64, "CIV_COLOR_ATLAS", controlTable.CivIconBG );
		end	
				
		if(v.VictoryType)then
			controlTable.WinType:LocalizeAndSetText(GameInfo.Victories[v.VictoryType].VictoryStatement);
		else
			controlTable.WinType:LocalizeAndSetText("");
		end
			
		for row in GameInfo.MapScripts() do
			if(row.FileName == v.MapName) then
				mapScript = row;
				break;
			end
		end
		
		local mapInfo = MapUtilities.GetBasicInfo(v.MapName);
        IconHookup( mapScript.IconIndex or 0, 32, mapInfo.IconAtlas, controlTable.MapType );        
		controlTable.MapType:SetToolTipString( Locale.ConvertTextKey( mapInfo.Name ) );
		
		info = GameInfo.Worlds[ v.WorldSize ];
		if ( info ~= nil ) then
			IconHookup( info.PortraitIndex, 32, info.IconAtlas, controlTable.MapSize );
			controlTable.MapSize:SetToolTipString( Locale.ConvertTextKey( info.Description ) );
		end
		
		info = GameInfo.GameSpeeds[ v.GameSpeed ];
		if ( info ~= nil ) then
			IconHookup( info.PortraitIndex, 32, info.IconAtlas, controlTable.SpeedIcon );
			controlTable.SpeedIcon:SetToolTipString( Locale.ConvertTextKey( info.Description ) );
		end
		
		--controlTable.MaxTurns:LocalizeAndSetText("TXT_KEY_MAX_TURN_FORMAT", v.WinningTurn);
		info = GameInfo.Eras[ v.StartEraType ];
		if ( info ~= nil ) then
			controlTable.StartEraTurns:LocalizeAndSetText("TXT_KEY_ERA_TURNS_FORMAT", Locale.ConvertTextKey(info.Description), v.WinningTurn);
		else
			controlTable.StartEraTurns:LocalizeAndSetText("TXT_KEY_ERA_TURNS_FORMAT", "TXT_KEY_MISC_UNKNOWN", v.WinningTurn);
		end
		
		controlTable.TimeFinished:SetText(v.GameEndTime);
		
		controlTable.IconStack:CalculateSize();
		controlTable.IconStack:ReprocessAnchoring();
				
		controlTable.SettingStack:CalculateSize();
		controlTable.SettingStack:ReprocessAnchoring();
				
		local itemY = controlTable.SettingStack:GetSizeY();
		controlTable.GameItem:SetSizeY(itemY);
	end
	
	--print("PopulateLeaderboardResults(): g_topDisplayedRank: " .. g_topDisplayedRank 
	--				.. " g_botDisplayedRank: " .. g_botDisplayedRank 
	--				.. " newTopRank: " .. newTopRank
	--				.. " newBotRank: " .. newBotRank);
	if(g_topDisplayedRank ~= -1 and g_botDisplayedRank ~= -1) then
		-- We're loading fresh data for an existing leaderboard.  
		-- Do some fancy math so it feels like the scroll bar is scrolling thru the whole leaderboard 
		-- (instead of just our current range).
		local curScrollRank = g_topDisplayedRank + ((g_botDisplayedRank - g_topDisplayedRank) * Controls.ScoreScrollPanel:GetScrollValue());
		--print("curScrollRank: " .. curScrollRank);
		if(curScrollRank < newTopRank) then
			--print("Setting Scroll to bottom");
			Controls.ScoreScrollPanel:SetScrollValue(0);
		elseif(curScrollRank > newBotRank) then
			--print("Setting Scroll to top");
			Controls.ScoreScrollPanel:SetScrollValue(1);
		else
			local newScrollValue = (curScrollRank - newTopRank)/(newBotRank - newTopRank);
			--print("newScrollValue: " .. newScrollValue);
			Controls.ScoreScrollPanel:SetScrollValue(newScrollValue);	
		end
	elseif(m_currentLeaderboardCategory == 2) then
		-- default scroll bar position to be positioned on local player if looking at the personal leaderboard.
		Controls.ScoreScrollPanel:SetScrollValue(0.5);
	end

	g_topDisplayedRank = newTopRank;
	g_botDisplayedRank = newBotRank;
	
	Controls.GameStack:CalculateSize();
	Controls.GameStack:ReprocessAnchoring();
	Controls.ScoreScrollPanel:CalculateInternalSize();
end


----------------------------------------------------------------        
-- Key Down Processing
----------------------------------------------------------------        
function InputHandler( uiMsg, wParam, lParam )
    if( uiMsg == KeyEvents.KeyDown )
    then
        if( wParam == Keys.VK_RETURN or wParam == Keys.VK_ESCAPE ) then
			OnBack();
        end
    end
    return true;
end
if( not m_bIsEndGame ) then
	ContextPtr:SetInputHandler( InputHandler );
	Controls.Background:SetHide(false);
end

-------------------------------------------------
-------------------------------------------------
function ShowHideHandler( bIsHide )
	if(not bIsHide)then
		BuildLeaderboardPulldown();
		OnCategory(1);
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

-------------------------------------------------
-- If we hear a multiplayer game invite was sent, exit
-- so we don't interfere with the transition
-------------------------------------------------
function OnMultiplayerGameInvite()
   	if( ContextPtr:IsHidden() == false ) then
		OnBack();
	end
end

Events.MultiplayerGameLobbyInvite.Add( OnMultiplayerGameInvite );
Events.MultiplayerGameServerInvite.Add( OnMultiplayerGameInvite );

-------------------------------------------------
-- This function needs to be in sync with the latest Leaderboard_ScoresDownloadedData.
-- Refer to Leaderboard_ScoresDownloadedStatus for the different leaderboardStatus values
-------------------------------------------------
function OnLeaderboard_ScoresDownloaded( leaderboardStatus, atTop, atBottom )
	-- Update m_atLeaderboardTop and m_atLeaderboardBottom
	m_atLeaderboardTop = atTop;
	m_atLeaderboardBottom = atBottom;

	-- Update Scores
	if( leaderboardStatus == 0 ) then 
		-- kLeaderboard_ScoresDownloadedSuccess
		Controls.LeaderboardStatus:SetHide(true);
		PopulateLeaderboardResults();	
	elseif ( leaderboardStatus == 1 ) then
		-- kLeaderboard_NotSupportedByMod
		Controls.LeaderboardStatus:SetHide(false);
		Controls.LeaderboardStatus:SetText(notSupportedByModString);
	elseif ( leaderboardStatus == 2 ) then
		-- kLeaderboard_NoLeaderboard
		Controls.LeaderboardStatus:SetHide(false);
		Controls.LeaderboardStatus:SetText(noLeaderboardString);
	elseif ( leaderboardStatus == 3 ) then
		-- kLeaderboard_NoScores
		Controls.LeaderboardStatus:SetHide(false);
		
		-- text message depends on leaderboard category selected.
		if(m_currentLeaderboardCategory == 1) then 
			-- Viewing Friends Category
			Controls.LeaderboardStatus:SetText(noScoresFriendsString);
		elseif(m_currentLeaderboardCategory == 2) then
			-- Viewing Personal Category
			Controls.LeaderboardStatus:SetText(noScoresPersonalString);
		else
			-- We should only get kLeaderboard_NoScores for the Friends/Personal categories.
			-- If there are no scores in the global leaderboard, we should get kLeaderboard_NoLeaderboard
			-- Defaulting to noLeaderboardString just in cause.
			Controls.LeaderboardStatus:SetText(noLeaderboardString);
		end
	end
end

Events.Leaderboard_ScoresDownloaded.Add( OnLeaderboard_ScoresDownloaded );

----------------------------------------------------------------        
----------------------------------------------------------------        

if( m_bIsEndGame ) then
    Controls.FrontEndFrame:SetHide( true );
    Controls.Background:SetHide(true);
end