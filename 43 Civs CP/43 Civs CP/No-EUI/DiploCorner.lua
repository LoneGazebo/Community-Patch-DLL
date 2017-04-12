-------------------------------------------------
-- Diplomacy and Advisors Buttons that float out in the screen
-------------------------------------------------
include( "IconSupport" );
include( "SupportFunctions"  );
include( "InstanceManager" );
local g_ChatInstances = {};

local g_iChatTeam   = -1;
local g_iChatPlayer = -1;

local g_iLocalPlayer = Game.GetActivePlayer();
local g_pLocalPlayer = Players[ g_iLocalPlayer ];
local g_iLocalTeam = g_pLocalPlayer:GetTeam();
local g_pLocalTeam = Teams[ g_iLocalTeam ];      

local m_bChatOpen = not Controls.ChatPanel:IsHidden();


-------------------------------------------------
-------------------------------------------------
-- This method refreshes the entries that are in the additional information dropdown.
-- Modders can use the Lua event "AdditionalInformationDropdownGatherEntries" to 
-- add entries to the list.
function RefreshAdditionalInformationEntries()

	local function Popup(popupType, data1, data2)
		Events.SerialEventGameMessagePopup{ 
			Type = popupType,
			Data1 = data1,
			Data2 = data2
		};
	end

	local additionalEntries = {
		{ text = Locale.Lookup("TXT_KEY_ADVISOR_COUNSEL"),					call=function() Popup(ButtonPopupTypes.BUTTONPOPUP_ADVISOR_COUNSEL); end};
		{ text = Locale.Lookup("TXT_KEY_ADVISOR_SCREEN_TECH_TREE_DISPLAY"), call=function() Popup(ButtonPopupTypes.BUTTONPOPUP_TECH_TREE, nil, -1); end };
		{ text = Locale.Lookup("TXT_KEY_DIPLOMACY_OVERVIEW"),				call=function() Popup(ButtonPopupTypes.BUTTONPOPUP_DIPLOMATIC_OVERVIEW); end };
		{ text = Locale.Lookup("TXT_KEY_MILITARY_OVERVIEW"),				call=function() Popup(ButtonPopupTypes.BUTTONPOPUP_MILITARY_OVERVIEW); end };
		{ text = Locale.Lookup("TXT_KEY_ECONOMIC_OVERVIEW"),				call=function() Popup(ButtonPopupTypes.BUTTONPOPUP_ECONOMIC_OVERVIEW); end };
		{ text = Locale.Lookup("TXT_KEY_VP_TT"),							call=function() Popup(ButtonPopupTypes.BUTTONPOPUP_VICTORY_INFO); end };
		{ text = Locale.Lookup("TXT_KEY_DEMOGRAPHICS"),						call=function() Popup(ButtonPopupTypes.BUTTONPOPUP_DEMOGRAPHICS); end };
		{ text = Locale.Lookup("TXT_KEY_POP_NOTIFICATION_LOG"),				call=function() Popup(ButtonPopupTypes.BUTTONPOPUP_NOTIFICATION_LOG,Game.GetActivePlayer()); end };
		{ text = Locale.Lookup("TXT_KEY_TRADE_ROUTE_OVERVIEW"),				call=function() Popup(ButtonPopupTypes.BUTTONPOPUP_TRADE_ROUTE_OVERVIEW); end };
	};

	-- Obtain any modder/dlc entries.
	LuaEvents.AdditionalInformationDropdownGatherEntries(additionalEntries);
	
	-- Now that we have all entries, call methods to sort them
	LuaEvents.AdditionalInformationDropdownSortEntries(additionalEntries);

	 Controls.MultiPull:ClearEntries();

	Controls.MultiPull:RegisterSelectionCallback(function(id)
		local entry = additionalEntries[id];
		if(entry and entry.call ~= nil) then
			entry.call();
		end
	end);
		 
	for i,v in ipairs(additionalEntries) do
		local controlTable = {};
		Controls.MultiPull:BuildEntry( "InstanceOne", controlTable );

		controlTable.Button:SetText( v.text );
		controlTable.Button:LocalizeAndSetToolTip( v.tip );
		controlTable.Button:SetVoid1(i);
		
	end

	-- STYLE HACK
	-- The grid has a nice little footer that will overlap entries if it is not resized to be larger than everything else.
	Controls.MultiPull:CalculateInternals();
	local dropDown = Controls.MultiPull;
	local width, height = dropDown:GetGrid():GetSizeVal();
	dropDown:GetGrid():SetSizeVal(width, height+100);

end
LuaEvents.RequestRefreshAdditionalInformationDropdownEntries.Add(RefreshAdditionalInformationEntries);

function SortAdditionalInformationDropdownEntries(entries)
	table.sort(entries, function(a,b)
		return (Locale.Compare(a.text, b.text) == -1);
	end);
end
LuaEvents.AdditionalInformationDropdownSortEntries.Add(SortAdditionalInformationDropdownEntries);

-------------------------------------------------
-------------------------------------------------
function OnCultureOverview()
    local popupInfo = {
        Type = ButtonPopupTypes.BUTTONPOPUP_CULTURE_OVERVIEW,
    }
    Events.SerialEventGameMessagePopup(popupInfo);

end
Controls.CultureOverviewButton:RegisterCallback( Mouse.eLClick, OnCultureOverview );

function OnEspionageButton()
	Events.SerialEventGameMessagePopup{ 
		Type = ButtonPopupTypes.BUTTONPOPUP_ESPIONAGE_OVERVIEW,
	};
end
Controls.EspionageButton:RegisterCallback(Mouse.eLClick, OnEspionageButton);

-- C4DF
function OnVassalButton()
	Events.SerialEventGameMessagePopup{ 
		Type = ButtonPopupTypes.BUTTONPOPUP_MODDER_11,
	};
end
Controls.VassalButton:RegisterCallback(Mouse.eLClick, OnVassalButton);
--END
function OnCorpButton()
	Events.SerialEventGameMessagePopup{ 
		Type = ButtonPopupTypes.BUTTONPOPUP_MODDER_5,
	};
end
Controls.CorpButton:RegisterCallback(Mouse.eLClick, OnCorpButton);
-------------------------------------------------
-- On ChatToggle
-------------------------------------------------
function OnChatToggle()

    m_bChatOpen = not m_bChatOpen;

    if( m_bChatOpen ) then
        Controls.ChatPanel:SetHide( false );
        Controls.ChatToggle:SetTexture( "assets/UI/Art/Icons/MainChatOn.dds" );
        Controls.HLChatToggle:SetTexture( "assets/UI/Art/Icons/MainChatOffHL.dds" );
        Controls.MOChatToggle:SetTexture( "assets/UI/Art/Icons/MainChatOff.dds" );
    else
        Controls.ChatPanel:SetHide( true );
        Controls.ChatToggle:SetTexture( "assets/UI/Art/Icons/MainChatOff.dds" );
        Controls.HLChatToggle:SetTexture( "assets/UI/Art/Icons/MainChatOnHL.dds" );
        Controls.MOChatToggle:SetTexture( "assets/UI/Art/Icons/MainChatOn.dds" );
    end

    LuaEvents.ChatShow( m_bChatOpen );
end
Controls.ChatToggle:RegisterCallback( Mouse.eLClick, OnChatToggle );


-------------------------------------------------
-------------------------------------------------
local bFlipper = false;
function OnChat( fromPlayer, toPlayer, text, eTargetType )

    local controlTable = {};
    ContextPtr:BuildInstanceForControl( "ChatEntry", controlTable, Controls.ChatStack );
  
    table.insert( g_ChatInstances, controlTable );
    if( #g_ChatInstances > 100 ) then
        Controls.ChatStack:ReleaseChild( g_ChatInstances[ 1 ].Box );
        table.remove( g_ChatInstances, 1 );
    end
    
    TruncateString( controlTable.String, 200, Players[fromPlayer]:GetNickName() );
    local fromName = controlTable.String:GetText();
    
    if( eTargetType == ChatTargetTypes.CHATTARGET_TEAM ) then
        controlTable.String:SetColorByName( "Green_Chat" );
        controlTable.String:SetText( fromName .. ": " .. text ); 
        
    elseif( eTargetType == ChatTargetTypes.CHATTARGET_PLAYER ) then
    
        local toName;
        if( toPlayer == g_iLocalPlayer ) then
            toName = Locale.ConvertTextKey( "TXT_KEY_YOU" );
        else
            TruncateString( controlTable.String, 200, Players[toPlayer]:GetNickName() );
            toName = Locale.ConvertTextKey( "TXT_KEY_DIPLO_TO_PLAYER", controlTable.String:GetText() );
        end
        controlTable.String:SetText( fromName .. " (" .. toName .. "): " .. text ); 
        controlTable.String:SetColorByName( "Magenta_Chat" );
        
    elseif( fromPlayer == g_iLocalPlayer ) then
        controlTable.String:SetColorByName( "Gray_Chat" );
        
        controlTable.String:SetText( fromName .. ": " .. text ); 
    else
        controlTable.String:SetText( fromName .. ": " .. text ); 
    end
      
    controlTable.Box:SetSizeY( controlTable.String:GetSizeY() + 8 );
    controlTable.Box:ReprocessAnchoring();

    if( bFlipper ) then
        controlTable.Box:SetColorChannel( 3, 0.4 );
    end
    bFlipper = not bFlipper;
    
	Events.AudioPlay2DSound( "AS2D_IF_MP_CHAT_DING" );		

    Controls.ChatStack:CalculateSize();
    Controls.ChatScroll:CalculateInternalSize();
    Controls.ChatScroll:SetScrollValue( 1 );
end
Events.GameMessageChat.Add( OnChat );


-------------------------------------------------
-------------------------------------------------
function SendChat( text )
    if( string.len( text ) > 0 ) then
        Network.SendChat( text, g_iChatTeam, g_iChatPlayer );
    end
    Controls.ChatEntry:ClearString();
end
Controls.ChatEntry:RegisterCallback( SendChat );

-------------------------------------------------
-------------------------------------------------
function ShowHideInviteButton()
	local bShow = PreGame.IsInternetGame();
	Controls.MPInvite:SetHide( not bShow );
end

-------------------------------------------------
-- On MPInvite
-------------------------------------------------
function OnMPInvite()
    Steam.ActivateInviteOverlay();	
end
Controls.MPInvite:RegisterCallback( Mouse.eLClick, OnMPInvite );

----------------------------------------------------------------
----------------------------------------------------------------
function OnPlayerDisconnect( playerID )
    if( ContextPtr:IsHidden() == false ) then
    	ShowHideInviteButton();
	end
end
Events.MultiplayerGamePlayerDisconnected.Add( OnPlayerDisconnect );

-------------------------------------------------
-------------------------------------------------
function ShowHideHandler( bIsHide )
    Controls.CornerAnchor:SetHide( false );
    
    if(not bIsHide) then
		ShowHideInviteButton();
		LuaEvents.RequestRefreshAdditionalInformationDropdownEntries();
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

-------------------------------------------------
-------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    if( m_bChatOpen 
        and uiMsg == KeyEvents.KeyUp
        and wParam == Keys.VK_TAB ) then
        Controls.ChatEntry:TakeFocus();
        return true;
    end
end
ContextPtr:SetInputHandler( InputHandler );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnSocialPoliciesClicked()
    Events.SerialEventGameMessagePopup( { Type = ButtonPopupTypes.BUTTONPOPUP_CHOOSEPOLICY } );
end
Controls.SocialPoliciesButton:RegisterCallback( Mouse.eLClick, OnSocialPoliciesClicked );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnDiploClicked()
    Controls.DiploList:SetHide( not Controls.DiploList:IsHidden() );
    -- Events.SerialEventGameMessagePopup( { Type = ButtonPopupTypes.BUTTONPOPUP_DIPLOMACY } );
end
Controls.DiploButton:RegisterCallback( Mouse.eLClick, OnDiploClicked );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnOpenPlayerDealScreen( iOtherPlayer )
    local iUs = Game.GetActivePlayer();
    local iUsTeam = Players[ iUs ]:GetTeam();
    local pUsTeam = Teams[ iUsTeam ];

    -- any time we're legitimately opening the pvp deal screen, make sure we hide the diplolist.
    local iOtherTeam = Players[iOtherPlayer]:GetTeam();
    local iProposalTo = UI.HasMadeProposal( iUs );
   
	  -- this logic should match OnOpenPlayerDealScreen in TradeLogic.lua, DiploCorner.lua, and DiploList.lua  
    if( (pUsTeam:IsAtWar( iOtherTeam ) and (g_bAlwaysWar or g_bNoChangeWar) ) or																					 -- Always at War
	    (iProposalTo ~= -1 and iProposalTo ~= iOtherPlayer and not UI.ProposedDealExists(iOtherPlayer, iUs)) ) then -- Only allow one proposal from us at a time.
	    -- do nothing
	    return;
    else
        Controls.CornerAnchor:SetHide( true );
    end

end
Events.OpenPlayerDealScreenEvent.Add( OnOpenPlayerDealScreen );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnChatTarget( iTeam, iPlayer )
    g_iChatTeam = iTeam;
    g_iChatPlayer = iPlayer;

    if( iTeam ~= -1 ) then
		TruncateString( Controls.LengthTest, Controls.ChatPull:GetSizeX(), Locale.ConvertTextKey("TXT_KEY_DIPLO_TO_TEAM"));
        Controls.ChatPull:GetButton():SetText( Controls.LengthTest:GetText() );
    else
        if( iPlayer ~= -1 ) then
			TruncateString( Controls.LengthTest, Controls.ChatPull:GetSizeX(), Locale.ConvertTextKey("TXT_KEY_DIPLO_TO_PLAYER", Players[ iPlayer ]:GetNickName()));
            Controls.ChatPull:GetButton():SetText( Controls.LengthTest:GetText() );
        else
            Controls.ChatPull:GetButton():LocalizeAndSetText( "TXT_KEY_DIPLO_TO_ALL" );
        end
    end
end
Controls.ChatPull:RegisterSelectionCallback( OnChatTarget );


-------------------------------------------------------
-------------------------------------------------------
function PopulateChatPull()

    Controls.ChatPull:ClearEntries();

    -------------------------------------------------------
    -- Add All Entry
    local controlTable = {};
    Controls.ChatPull:BuildEntry( "InstanceOne", controlTable );
    controlTable.Button:SetVoids( -1, -1 );
    local textControl = controlTable.Button:GetTextControl();
    textControl:LocalizeAndSetText( "TXT_KEY_DIPLO_TO_ALL" );


    -------------------------------------------------------
    -- See if Team has more than 1 other human member
    local iTeamCount = 0;
    for iPlayer = 0, GameDefines.MAX_PLAYERS do
        local pPlayer = Players[iPlayer];

        if( iPlayer ~= g_iLocalPlayer and pPlayer ~= nil and pPlayer:IsHuman() and pPlayer:GetTeam() == g_iLocalTeam ) then
            iTeamCount = iTeamCount + 1;
        end
    end

    if( iTeamCount > 0 ) then
        local controlTable = {};
        Controls.ChatPull:BuildEntry( "InstanceOne", controlTable );
        controlTable.Button:SetVoids( g_iLocalTeam, -1 );
        local textControl = controlTable.Button:GetTextControl();
        textControl:LocalizeAndSetText( "TXT_KEY_DIPLO_TO_TEAM" );
    end


    -------------------------------------------------------
    -- Humans
    for iPlayer = 0, GameDefines.MAX_PLAYERS do
        local pPlayer = Players[iPlayer];

        if( iPlayer ~= g_iLocalPlayer and pPlayer ~= nil and pPlayer:IsHuman() ) then

            controlTable = {};
            Controls.ChatPull:BuildEntry( "InstanceOne", controlTable );
            controlTable.Button:SetVoids( -1, iPlayer );
            textControl = controlTable.Button:GetTextControl();
			TruncateString( textControl, Controls.ChatPull:GetSizeX()-20, Locale.ConvertTextKey("TXT_KEY_DIPLO_TO_PLAYER", pPlayer:GetNickName()));
        end
    end
    
    Controls.ChatPull:GetButton():LocalizeAndSetText( "TXT_KEY_DIPLO_TO_ALL" );
    Controls.ChatPull:CalculateInternals();
end
Events.MultiplayerGamePlayerUpdated.Add(PopulateChatPull);


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function DoUpdateLeagueCountdown()
	local bHide = true;
	local sTooltip = Locale.ConvertTextKey("TXT_KEY_EO_DIPLOMACY");
				
	if (Game.GetNumActiveLeagues() > 0) then
		local pLeague = Game.GetActiveLeague();
		if (pLeague ~= nil) then
			local iCountdown = pLeague:GetTurnsUntilSession();
			if (iCountdown ~= 0 and not pLeague:IsInSession()) then
				bHide = false;
				if (PreGame.IsVictory(GameInfo.Victories["VICTORY_DIPLOMATIC"].ID) and Game.IsUnitedNationsActive() and Game.GetGameState() == GameplayGameStateTypes.GAMESTATE_ON) then
					local iCountdownToVictorySession = pLeague:GetTurnsUntilVictorySession();
					if (iCountdownToVictorySession <= iCountdown) then
						Controls.UNTurnsLabel:SetText("[COLOR_POSITIVE_TEXT]" .. iCountdownToVictorySession .. "[ENDCOLOR]");
					else
						Controls.UNTurnsLabel:SetText(iCountdown);
					end
					sTooltip = Locale.ConvertTextKey("TXT_KEY_EO_DIPLOMACY_AND_VICTORY_SESSION", iCountdown, iCountdownToVictorySession);
				else
					Controls.UNTurnsLabel:SetText(iCountdown);
					sTooltip = Locale.ConvertTextKey("TXT_KEY_EO_DIPLOMACY_AND_LEAGUE_SESSION", iCountdown);
				end
			end
		end
	end
	
	Controls.UNTurnsLabel:SetHide(bHide);
	Controls.DiploButton:SetToolTipString(sTooltip);
end
Events.SerialEventGameDataDirty.Add(DoUpdateLeagueCountdown);

-- Also call it once so it starts correct - surprisingly enough, GameData isn't dirtied as we're loading a game
DoUpdateLeagueCountdown();

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function DoUpdateEspionageButton()
	local iLocalPlayer = Game.GetActivePlayer();
	local pLocalPlayer = Players[iLocalPlayer];
	local iNumUnassignedSpies = pLocalPlayer:GetNumUnassignedSpies();
	
	local strToolTip = Locale.ConvertTextKey("TXT_KEY_EO_TITLE");
	
	if (iNumUnassignedSpies > 0) then
		strToolTip = strToolTip .. "[NEWLINE][NEWLINE]";
		strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_EO_UNASSIGNED_SPIES_TT", iNumUnassignedSpies);
		Controls.UnassignedSpiesLabel:SetHide(false);
		Controls.UnassignedSpiesLabel:SetText(iNumUnassignedSpies);
	else
		Controls.UnassignedSpiesLabel:SetHide(true);
	end
	
	Controls.EspionageButton:SetToolTipString(strToolTip);
end
Events.SerialEventEspionageScreenDirty.Add(DoUpdateEspionageButton);

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function DoUpdateVassalButton()
	local iLocalPlayer = Game.GetActivePlayer();
	local pLocalPlayer = Players[iLocalPlayer];
	local pLocalTeam = Teams[pLocalPlayer:GetTeam()];
	local iNumVassalTaxesAvailable = 0;
	
	local strToolTip = Locale.ConvertTextKey("TXT_KEY_VO");
	
	-- calculate number of vassal taxes available
	for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
		local pOtherPlayer = Players[iPlayerLoop];
		if pOtherPlayer:IsEverAlive() then
			if (iPlayerLoop ~= iLocalPlayer and pOtherPlayer:IsAlive()) then
				if (pLocalTeam:CanSetVassalTax(iPlayerLoop)) then
					iNumVassalTaxesAvailable = iNumVassalTaxesAvailable + 1;
				end
			end
		end
	end

	if (iNumVassalTaxesAvailable > 0) then
		strToolTip = strToolTip .. "[NEWLINE][NEWLINE]";
		strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_VO_TAXES_AVAILABLE_TT", iNumVassalTaxesAvailable);
		Controls.VassalTaxesAvailableLabel:SetHide(false);
		Controls.VassalTaxesAvailableLabel:SetText(iNumVassalTaxesAvailable);
	else
		Controls.VassalTaxesAvailableLabel:SetHide(true);
	end
	
	Controls.VassalButton:SetToolTipString(strToolTip);
end
Events.SerialEventEspionageScreenDirty.Add(DoUpdateVassalButton); -- not a typo! do not change!

--------------------------------------------------------------------
function HandleNotificationAdded(notificationId, notificationType, toolTip, summary, gameValue, extraGameData)
	
	-- In the event we receive a new spy, make sure the large button is displayed.
	if(ContextPtr:IsHidden() == false) then
		if(notificationType == NotificationTypes.NOTIFICATION_SPY_CREATED_ACTIVE_PLAYER) then
			CheckEspionageStarted();
		end
		-- C4DF
		if(notificationType == NotificationTypes.NOTIFICATION_GENERIC) then
			CheckVassalageStarted();
		end
	end
end
Events.NotificationAdded.Add(HandleNotificationAdded);

DoUpdateEspionageButton();
-- C4DF
DoUpdateVassalButton();
-- END

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
if( Game.IsGameMultiPlayer() ) then
    PopulateChatPull();

	if ( not Game.IsHotSeat() ) then
		Controls.ChatToggle:SetHide( false );
		OnChatToggle();
	end
end


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnEndGameButton()

    local which = math.random( 1, 6 );
    
    if( which == 1 ) then Events.EndGameShow( EndGameTypes.Technology, Game.GetActivePlayer() ); 
    elseif( which == 2 ) then Events.EndGameShow( EndGameTypes.Domination, Game.GetActivePlayer() );
    elseif( which == 3 ) then Events.EndGameShow( EndGameTypes.Culture, Game.GetActivePlayer() );
    elseif( which == 4 ) then Events.EndGameShow( EndGameTypes.Diplomatic, Game.GetActivePlayer() );
    elseif( which == 5 ) then Events.EndGameShow( EndGameTypes.Time, Game.GetActivePlayer() );
    elseif( which == 6 ) then Events.EndGameShow( EndGameTypes.Time, Game.GetActivePlayer() + 1 ); 
    end
end
Controls.EndGameButton:RegisterCallback( Mouse.eLClick, OnEndGameButton );

local g_PerPlayerState = {};
----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnDiploCornerActivePlayerChanged( iActivePlayer, iPrevActivePlayer )
	-- Restore the state per player
	local bIsHidden = Controls.DiploList:IsHidden() == true;
	-- Save the state per player
	if (iPrevActivePlayer ~= -1) then
		g_PerPlayerState[ iPrevActivePlayer + 1 ] = bIsHidden;
	end
	
	if (iActivePlayer ~= -1) then
		if (g_PerPlayerState[ iActivePlayer + 1 ] == nil or g_PerPlayerState[ iActivePlayer + 1 ] == -1) then
			Controls.DiploList:SetHide( true );
		else
			local bWantHidden = g_PerPlayerState[ iActivePlayer + 1 ];
			if ( bWantHidden ~= Controls.DiploList:IsHidden()) then
				Controls.DiploList:SetHide( bWantHidden );
			end
		end
	end

	g_iLocalPlayer = Game.GetActivePlayer();
	g_pLocalPlayer = Players[ g_iLocalPlayer ];
	g_iLocalTeam = g_pLocalPlayer:GetTeam();
	g_pLocalTeam = Teams[ g_iLocalTeam ];
	PopulateChatPull();
end
Events.GameplaySetActivePlayer.Add(OnDiploCornerActivePlayerChanged);


function CheckEspionageStarted()
	function TestEspionageStarted()
		local player = Players[Game.GetActivePlayer()];
		return player:GetNumSpies() > 0;
	end

	local bEspionageStarted = TestEspionageStarted();
	Controls.CornerAnchor:SetHide(bEspionageStarted);
	Controls.CornerAnchor_Espionage:SetHide(not bEspionageStarted);
	Controls.EspionageButton:SetHide(not bEspionageStarted);
	if(bEspionageStarted) then
		DoUpdateEspionageButton();
	end
end

function CheckVassalageStarted()
	function TestVassalageStarted()
		local player = Players[Game.GetActivePlayer()];
		local team = Teams[player:GetTeam()];
		return (team:GetCurrentEra() >= Game.GetVassalageEnabledEra()) or team:IsVassalOfSomeone();
	end

	local bVassalStarted = TestVassalageStarted();
	Controls.VassalButton:SetHide(not bVassalStarted);
	if(bVassalStarted) then
		DoUpdateVassalButton();
	end
end

function OnActivePlayerTurnStart()
	CheckEspionageStarted();
	CheckVassalageStarted();
end
Events.ActivePlayerTurnStart.Add(OnActivePlayerTurnStart);


OnActivePlayerTurnStart();

--Hide CultureOverview, if disabled.
if(Game.IsOption("GAMEOPTION_NO_CULTURE_OVERVIEW_UI")) then
	Controls.CultureOverviewButton:SetHide(true);
end