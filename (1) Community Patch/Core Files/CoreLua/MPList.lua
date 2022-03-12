-------------------------------------------------
-- Diplomacy and Advisors Buttons that float out in the screen
-------------------------------------------------
include( "IconSupport" );
include( "SupportFunctions"  );
include( "InstanceManager" );

local g_pLocalTeam = Teams[ Players[ Game.GetActivePlayer() ]:GetTeam() ];
local g_bRebuildMPList = false;
local g_SortTable = {};
local g_PlayerEntries = {};
local g_TeamEntries = {};
local g_TeamData = {};

local TOP_COMPENSATION = 120;
local CHAT_COMPENSATION = 170;
local MINIMAP_GUESS = 250;
local _, screenY = UIManager:GetScreenSizeVal();
Controls.MPListScroll:SetSizeY( screenY - MINIMAP_GUESS - TOP_COMPENSATION - CHAT_COMPENSATION );

local SEPARATOR_MARGIN = 10;

local DiploRequestIncoming = Locale.ConvertTextKey( "TXT_KEY_DIPLO_REQUEST_INCOMING" );
local DiploRequestOutgoing = Locale.ConvertTextKey( "TXT_KEY_DIPLO_REQUEST_OUTGOING" );
local PlayerConnectedStr = Locale.ConvertTextKey( "TXT_KEY_MP_PLAYER_CONNECTED" );
local PlayerConnectingStr = Locale.ConvertTextKey( "TXT_KEY_MP_PLAYER_CONNECTING" );
local PlayerNotConnectedStr = Locale.ConvertTextKey( "TXT_KEY_MP_PLAYER_NOTCONNECTED" );
     
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function IsActiveHuman( pPlayer )
	-- Is this player really controlled by a human player?  
	-- This check handles the fact that pPlayer:IsHuman() returns true for unoccupied observer slots. DOH!
	return (Network.IsPlayerConnected(pPlayer:GetID()) or (pPlayer:IsHuman() and not pPlayer:IsObserver()));
end     
     

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function MPListSort( a, b )
     local aScore = g_SortTable[ tostring( a ) ];
     local bScore = g_SortTable[ tostring( b ) ];
        
     return aScore > bScore;
end


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function sortByPlayerID( pPlayer )
	-- Always sort by score now that multiplayer turn order is shown in the Turn Queue UI.
	return false;
	-- return (not pPlayer:IsSimultaneousTurns() and Game.IsNetworkMultiPlayer());
end


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function BuildStaticTeamsList()
     for iPlayer = 0, GameDefines.MAX_MAJOR_CIVS - 1 do
    
        local pPlayer = Players[ iPlayer ];
        
        local bIsObserver = (pPlayer:IsObserver() and (PreGame.GetSlotClaim(pPlayer:GetID()) == SlotClaim.SLOTCLAIM_ASSIGNED));
        if( pPlayer:IsAlive() or bIsObserver ) then
            local iTeam = pPlayer:GetTeam();
            local pTeam = Teams[ iTeam ];
            
            if( pTeam:GetNumMembers() == 1 and not bIsObserver) then
                g_TeamData[ iTeam ] = pPlayer;
            else
                if( g_TeamData[ iTeam ] == nil ) then
                    g_TeamData[ iTeam ] = {};
                end
                table.insert( g_TeamData[ iTeam ], pPlayer );
            end
        end
    end
end

local g_bShowPings = false;

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function UpdatePingTimeLabel( kControl, playerID )
	local iPingTime = Network.GetPingTime( playerID );
	if (iPingTime >= 0) then
		iPingTime = math.clamp(iPingTime, 0, 99999); --limit value to something that can be displayed in 5 chars.
		if (iPingTime == 0) then
			kControl:SetText( "<1ms" );
		else
			if (iPingTime < 1000) then
				kControl:SetText( tostring(iPingTime) .. "ms" );
			else
				kControl:SetText( string.format("%.2fs" , (iPingTime / 1000)) );
			end
		end
	else
		kControl:SetHide( true );
	end					
end

-------------------------------------------------
-------------------------------------------------
function OnPingTimesChanged()
    if( ContextPtr:IsHidden() == false and Game.IsNetworkMultiPlayer()) then

	    for playerID, controlTable in pairs( g_PlayerEntries ) do

			local pPlayer = Players[ playerID ];
			
			--if( playerID ~= Game.GetActivePlayer() ) then
				if (controlTable ~= nil and not controlTable.Ping:IsHidden()) then
					if ((pPlayer:IsAlive() or pPlayer:IsObserver()) and pPlayer:IsHuman()) then
						UpdatePingTimeLabel( controlTable.Ping, playerID );
					else
						controlTable.Ping:SetHide( true );
					end
				end
			--end	
		end
    end
end
Events.MultiplayerPingTimesChanged.Add( OnPingTimesChanged );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function UpdatePlayerData( pPlayer, controlTable )

  local iTeam = pPlayer:GetTeam();
  local pTeam = Teams[ iTeam ];
  local bMet = g_pLocalTeam:IsHasMet( iTeam );
	local bIsHuman = pPlayer:IsHuman();
	local bIsLocalPlayer = pPlayer:GetID() == Game.GetActivePlayer();
	local bIsNetworkMP = Game.IsNetworkMultiPlayer();
	local bIsPitboss = Game.IsOption("GAMEOPTION_PITBOSS");
	
	-- Hide dead players
	if(not pPlayer:IsAlive() and not pPlayer:IsObserver()) then
		controlTable.Root:SetHide(true);
	else
		controlTable.Root:SetHide(false);
	end

	if(bIsNetworkMP) then
		-- Networked multiplayer
		if(Matchmaking.IsHost()) then
			controlTable.KickBox:SetHide(false);
		else
			controlTable.KickBox:SetHide(true);
		end
		controlTable.HostBox:SetHide(false);
		controlTable.ConnectedBox:SetHide(false);
		controlTable.DiploBox:SetHide(false);
		if(g_bShowPings) then
			controlTable.PingBox:SetHide(false);	
			controlTable.Ping:SetHide( not bIsHuman or bIsLocalPlayer );
		else
			controlTable.PingBox:SetHide(true);	
		end
	elseif(PreGame.IsHotSeatGame()) then
		-- hotseat
		controlTable.KickBox:SetHide(true);
		controlTable.HostBox:SetHide(true);
		controlTable.ConnectedBox:SetHide(true);
		controlTable.DiploBox:SetHide(false);	
		controlTable.PingBox:SetHide(true);			
	else
		-- singleplayer 
		controlTable.KickBox:SetHide(true);
		controlTable.HostBox:SetHide(true);
		controlTable.ConnectedBox:SetHide(true);
		controlTable.DiploBox:SetHide(true);	
		controlTable.PingBox:SetHide(true);	
	end
		
	-- Determine the size of player entry
	controlTable.PlayerDataStack:CalculateSize();
	controlTable.Root:SetSizeX(controlTable.PlayerDataStack:GetSizeX());
	
	-- Update click entry.
	controlTable.ClickEntry:SetSizeX(controlTable.PlayerDataStack:GetSizeX());
	controlTable.ClickEntry:SetHide( not bIsNetworkMP );
	
	-- Update seperator size
	controlTable.Seperator:SetSizeX( controlTable.PlayerDataStack:GetSizeX() - SEPARATOR_MARGIN);
	
    -- name
    if( bIsHuman ) then
			local activePlayer = Game.GetActivePlayer();
			if( Game.IsGameMultiPlayer() == false and bIsLocalPlayer ) then         
				controlTable.Name:LocalizeAndSetText( "TXT_KEY_YOU" );
			elseif( pPlayer:IsObserver() and not Network.IsPlayerConnected(pPlayer:GetID()) ) then
				-- empty observer slot
				controlTable.Name:LocalizeAndSetText( "TXT_KEY_SLOTTYPE_OPEN" );
			else
				controlTable.Name:SetText(pPlayer:GetNickName());
      end
            
      if (g_bShowPings) then
				UpdatePingTimeLabel( controlTable.Ping, pPlayer:GetID() );
			end
    else
			if( bMet ) then
				controlTable.Name:SetText( pPlayer:GetName() );
       else
				controlTable.Name:LocalizeAndSetText( "TXT_KEY_POP_VOTE_RESULTS_UNMET_PLAYER" );
			end
    end
    
    -- icon
    CivIconHookup( bMet and pPlayer:GetID() or -1, 32, controlTable.Icon, controlTable.CivIconBG, controlTable.CivIconShadow, false, true );


    -- war color
    if( g_pLocalTeam:IsAtWar( pPlayer:GetTeam() ) ) then
        controlTable.Name:SetColorByName( "COLOR_NEGATIVE_TEXT" );
    else
        controlTable.Name:SetColorByName( "White" );
    end
    
    -- score
    controlTable.Score:SetText( pPlayer:GetScore() );
	if(sortByPlayerID(pPlayer)) then
		g_SortTable[ tostring( controlTable.Root ) ] = GameDefines.MAX_PLAYERS - pPlayer:GetID();
	else
    	g_SortTable[ tostring( controlTable.Root ) ] = pPlayer:GetScore();
	end
	
	if(pPlayer:IsObserver()) then
		controlTable.Score:SetHide(true);
	end

    -- turn finished
    if( not pPlayer:IsAlive()
			or pPlayer:HasReceivedNetTurnComplete() -- Human player finished with turn
			or (not pPlayer:IsHuman() and not pPlayer:IsTurnActive())  ) then -- AI player finished with turn
				controlTable.Name:SetAlpha( 0.5 );
        controlTable.IconBox:SetAlpha( 0.2 );
    else
        controlTable.Name:SetAlpha( 1 );
        controlTable.IconBox:SetAlpha( 1 );
    end
    
    -- Player connected indicator
    if(IsActiveHuman(pPlayer)) then
			controlTable.ConnectionStatus:SetHide(false);
			if(Network.IsPlayerHotJoining(pPlayer:GetID())) then
				-- Player is hot joining.
				controlTable.ConnectionStatus:SetTextureOffsetVal(0,32);
				controlTable.ConnectionStatus:SetToolTipString( PlayerConnectingStr );
			elseif(pPlayer.IsConnected ~= nil and pPlayer:IsConnected()) then
				-- fully connected
				controlTable.ConnectionStatus:SetTextureOffsetVal(0,0);
				controlTable.ConnectionStatus:SetToolTipString( PlayerConnectedStr );
			else
				-- Not connected
				controlTable.ConnectionStatus:SetTextureOffsetVal(0,96);
				controlTable.ConnectionStatus:SetToolTipString( PlayerNotConnectedStr );		
			end		
    else
				controlTable.ConnectionStatus:SetHide(true);
    end
    
		-- Update diplomatic request button.
		local localPlayer = Game.GetActivePlayer();
		if(UI.ProposedDealExists(localPlayer, pPlayer:GetID())) then
			-- We proposed something to them.
			controlTable.DiploWaiting:SetHide(false);
			controlTable.DiploWaiting:SetAlpha( 0.5 );
			controlTable.DiploWaiting:SetToolTipString( DiploRequestOutgoing );
		elseif(UI.ProposedDealExists(pPlayer:GetID(), localPlayer)) then
			--They proposed something to us.
			controlTable.DiploWaiting:SetHide(false);
			controlTable.DiploWaiting:SetAlpha( 1.0 );
			controlTable.DiploWaiting:SetToolTipString( DiploRequestIncoming );
		else
			-- No deals in progress.
			controlTable.DiploWaiting:SetHide(true);
		end
		
		controlTable.HostIcon:SetHide( not (Matchmaking.GetHostID() == pPlayer:GetID()) );	 
		
		local allowKick = Matchmaking.IsHost() and not bIsLocalPlayer and IsActiveHuman(pPlayer);
		controlTable.KickButton:SetHide(not allowKick);
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function MPListEntryClick()
	if PreGame.IsInternetGame() then		-- Pings only valid with the Steam transport
		g_bShowPings = not g_bShowPings;
		UpdateAndSort();
			
		Controls.MPListStack:CalculateSize();
		Controls.MPListStack:ReprocessAnchoring();
		Controls.MPListScroll:SetSizeX(Controls.MPListStack:GetSizeX());
		Controls.MPListScroll:CalculateInternalSize();
		if( Controls.MPListScroll:GetRatio() < 1 ) then
			Controls.MPListScroll:SetOffsetX( 15 );
		end
		Controls.MPListScroll:ReprocessAnchoring();   	
	end
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnKickPlayer( selectionIndex )
	local pPlayer = Players[selectionIndex];
	local playerName = pPlayer:GetName();
	UIManager:PushModal(Controls.ConfirmKick, true);	
	LuaEvents.SetKickPlayer(selectionIndex, playerName);
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnDiploWaiting( ePlayer )
	if not Players[Game.GetActivePlayer()]:IsTurnActive() or Game.IsProcessingMessages() then
		return;
	end
	if( Players[ePlayer]:IsHuman() ) then
		Events.OpenPlayerDealScreenEvent( ePlayer );
	else 
		UI.SetRepeatActionPlayer(ePlayer);
		UI.ChangeStartDiploRepeatCount(1);
		Players[ePlayer]:DoBeginDiploWithHuman();    
	end
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function TeamHasObservers(pTeam, teamData)
	-- check to see if this is a player instance since it might not be and will cause runtime error. there is probably an actual fix elsewhere if keen.
	if(getmetatable(teamData) ~= nil and pTeam:GetNumMembers() == 1 ) then
	
		-- single member team
		local pPlayer = teamData;
		
		if( pPlayer:IsObserver() and PreGame.GetSlotClaim(pPlayer:GetID()) == SlotClaim.SLOTCLAIM_ASSIGNED ) then
			return true;
		end
	else
		for _, pPlayer in pairs( teamData ) do	        
			if( pPlayer:IsObserver() and PreGame.GetSlotClaim(pPlayer:GetID()) == SlotClaim.SLOTCLAIM_ASSIGNED ) then
				return true;
			end
		end
	end
	
	return false;
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function BuildControls()

    for iTeam, teamData in pairs( g_TeamData ) do
        local pTeam = Teams[ iTeam ];

				local bObserverTeam = TeamHasObservers(pTeam, teamData);
        if( pTeam:GetNumMembers() == 1 and not bObserverTeam) then

            -- single member team
            local pPlayer = teamData;

            -- Make sure they're alive
            if( pPlayer:IsAlive() ) then
                local controlTable = {};
                ContextPtr:BuildInstanceForControl( "PlayerEntry", controlTable, Controls.MPListStack );
                g_PlayerEntries[ pPlayer:GetID() ] = controlTable;
								controlTable.ClickEntry:RegisterCallback( Mouse.eLClick, MPListEntryClick );
								controlTable.KickButton:RegisterCallback( Mouse.eLClick, OnKickPlayer );
								controlTable.KickButton:SetVoid1(pPlayer:GetID());
								controlTable.DiploWaiting:RegisterCallback( Mouse.eLClick, OnDiploWaiting );
								controlTable.DiploWaiting:SetVoid1(pPlayer:GetID());

                UpdatePlayerData( pPlayer, controlTable );
            end
        else
            local teamEntry = {};
            ContextPtr:BuildInstanceForControl( "TeamEntry", teamEntry, Controls.MPListStack );
			
            for _, pPlayer in pairs( teamData ) do
                
                if( pPlayer:IsEverAlive() or (pPlayer:IsObserver() and (PreGame.GetSlotClaim(pPlayer:GetID()) == SlotClaim.SLOTCLAIM_ASSIGNED)) ) then
                    local controlTable = {};
                    ContextPtr:BuildInstanceForControl( "PlayerEntry", controlTable, teamEntry.TeamStack );
                    g_PlayerEntries[ pPlayer:GetID() ] = controlTable;
                    controlTable.Seperator:SetHide( true );
										controlTable.ClickEntry:RegisterCallback( Mouse.eLClick, MPListEntryClick );
										controlTable.KickButton:RegisterCallback( Mouse.eLClick, OnKickPlayer );
										controlTable.KickButton:SetVoid1(pPlayer:GetID());
										controlTable.DiploWaiting:RegisterCallback( Mouse.eLClick, OnDiploWaiting );
										controlTable.DiploWaiting:SetVoid1(pPlayer:GetID());
																		
                    UpdatePlayerData( pPlayer, controlTable );
                end
            end
            
            if(bObserverTeam) then 
				-- OBSERVER_TEAM
				teamEntry.TeamName:LocalizeAndSetText( "TXT_KEY_MULTIPLAYER_OBSERVER_TEAM_NAME" );
				teamEntry.Score:SetHide(true);
			else			
				teamEntry.TeamName:LocalizeAndSetText( "TXT_KEY_MULTIPLAYER_DEFAULT_TEAM_NAME", iTeam + 1 );
			end
			teamEntry.TeamClickEntry:RegisterCallback( Mouse.eLClick, MPListEntryClick );

            teamEntry.TeamStack:CalculateSize();
            teamEntry.Root:SetSizeX( teamEntry.TeamStack:GetSizeX() );
            teamEntry.Root:SetSizeY( teamEntry.TeamStack:GetSizeY() + teamEntry.TeamName:GetSizeY() );

            g_TeamEntries[ iTeam ] = teamEntry;
            g_SortTable[ tostring( teamEntry.Root ) ] = pTeam:GetScore();
        end
    end
    
    Controls.MPListStack:CalculateSize();
    Controls.MPListScroll:SetSizeX(Controls.MPListStack:GetSizeX());
    Controls.MPListScroll:CalculateInternalSize();
    if( Controls.MPListScroll:GetRatio() < 1 ) then
        Controls.MPListScroll:SetOffsetX( 15 );
    end
    Controls.MPListScroll:ReprocessAnchoring();   

end


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function UpdateAndSort()
  for iPlayer, controlTable in pairs( g_PlayerEntries ) do
		UpdatePlayerData( Players[ iPlayer ], controlTable );
  end
    
	local bIsNetworkMP = Game.IsNetworkMultiPlayer();
	
  for iTeam, teamEntry in pairs( g_TeamEntries ) do
		local pTeam = Teams[ iTeam ];
    local score = pTeam:GetScore();
    teamEntry.Score:SetText( score );
		teamEntry.TeamClickEntry:SetHide( not bIsNetworkMP );
	
    teamEntry.TeamStack:CalculateSize();
		teamEntry.TeamBox:SetSizeX( teamEntry.TeamStack:GetSizeX() );
    teamEntry.Root:SetSizeX( teamEntry.TeamStack:GetSizeX() );
    teamEntry.TeamSeparator:SetSizeX( teamEntry.TeamStack:GetSizeX() - SEPARATOR_MARGIN);
    teamEntry.TeamHeaderSeparator:SetSizeX( teamEntry.TeamStack:GetSizeX() - SEPARATOR_MARGIN);
                  
    g_SortTable[ tostring( teamEntry.Root ) ] = score;
        
    -- sort all the players on the team
    teamEntry.TeamStack:SortChildren( MPListSort );
	end
    
  -- sort all of the teams
  Controls.MPListStack:SortChildren( MPListSort );
end

Events.MultiplayerGamePlayerDisconnected.Add( UpdateAndSort );
Events.MultiplayerGamePlayerUpdated.Add( UpdateAndSort );
Events.MultiplayerHotJoinStarted.Add( UpdateAndSort );
Events.MultiplayerHotJoinCompleted.Add( UpdateAndSort );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnUpdate()
    if( g_bRebuildMPList ) then
        UpdateAndSort();
        g_bRebuildMPList = false;
    end
end


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnScoreDirty()
    g_bRebuildMPList = true;
end
Events.SerialEventScoreDirty.Add( OnScoreDirty );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnRemotePlayerTurnStart()
    g_bRebuildMPList = true;
end
Events.RemotePlayerTurnStart.Add( OnRemotePlayerTurnStart );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnRemotePlayerTurnEnd()
    g_bRebuildMPList = true;
end
Events.RemotePlayerTurnEnd.Add( OnRemotePlayerTurnEnd );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnLocalTurnStart()
    g_bRebuildMPList = true;
end
Events.ActivePlayerTurnStart.Add( OnLocalTurnStart );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnSerialEventGameDataDirty()
		-- We need to rebuild the mp list because it's possible that 
		-- we're involved in a multiplayer diplomacy proposal.
    g_bRebuildMPList = true;
end
Events.SerialEventGameDataDirty.Add( OnSerialEventGameDataDirty );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnChatToggle( bChatOpen )
    if( bChatOpen ) then
        Controls.MPListScroll:SetOffsetY( TOP_COMPENSATION + CHAT_COMPENSATION );
    else
        Controls.MPListScroll:SetOffsetY( TOP_COMPENSATION );
    end
end
LuaEvents.ChatShow.Add( OnChatToggle );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide )
    if( not bIsHide ) then
        ContextPtr:SetUpdate( OnUpdate );
    else
        ContextPtr:ClearUpdate();
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

    
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnOptionsChanged()
    if( (Game.IsGameMultiPlayer() and OptionsManager.GetMPScoreList()) 
    or (not Game.IsGameMultiPlayer() and OptionsManager.GetScoreList())) then
        ContextPtr:SetHide( false );
    else
        ContextPtr:SetHide( true );
    end
end
Events.GameOptionsChanged.Add( OnOptionsChanged );


function OnShowScoreList(bShow)
	if(bShow) then
		ContextPtr:SetHide( false );
	else
		ContextPtr:SetHide( true );
	end
end

LuaEvents.ShowScoreList.Add( OnShowScoreList );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnMPListActivePlayerChanged( iActivePlayer, iPrevActivePlayer )
	g_pLocalTeam = Teams[ Players[ Game.GetActivePlayer() ]:GetTeam() ];
	g_bRebuildMPList = true;
end
Events.GameplaySetActivePlayer.Add(OnMPListActivePlayerChanged);


-------------------------------------------------------------------------------
-- Hot Join Notice Handling
-------------------------------------------------------------------------------
function OnHotJoinStart()
	Controls.HotJoinNotice:SetHide(false);
end
Events.MultiplayerHotJoinStarted.Add(OnHotJoinStart);

function OnHotJoinCompleted()
	Controls.HotJoinNotice:SetHide(true);
end
Events.MultiplayerHotJoinCompleted.Add(OnHotJoinCompleted);


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
BuildStaticTeamsList();
BuildControls();
g_bRebuildMPList = true;
OnOptionsChanged();

