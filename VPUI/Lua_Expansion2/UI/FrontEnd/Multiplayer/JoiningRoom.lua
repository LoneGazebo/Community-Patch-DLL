if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
----------------------------------------------------------------        
-- Globals
----------------------------------------------------------------   
local g_joinFailed = false;		-- Are we attempting to close this page after a join failure?
															-- Used to prevent the ActivateAllowedDLC mechanic in ShowHideHander 
															-- from restoring the page after a join failed.  This can occur
															-- due to the event system frame lag.


----------------------------------------------------------------        
-- Input Handler
----------------------------------------------------------------        
function InputHandler( uiMsg, wParam, lParam )

	if uiMsg == KeyEvents.KeyDown then

		if wParam == Keys.VK_ESCAPE then

			Matchmaking.LeaveMultiplayerGame();
			UIManager:DequeuePopup( ContextPtr );

		end

	end

	return true;

end
ContextPtr:SetInputHandler( InputHandler );


-------------------------------------------------
-- Event Handler: MultiplayerJoinRoomComplete
-------------------------------------------------
function OnJoinRoomComplete()

	if Matchmaking.IsHost() then

        UIManager:QueuePopup( Controls.StagingRoomScreen, PopupPriority.StagingScreen );
        UIManager:DequeuePopup( ContextPtr );
        
	else

		Controls.JoiningLabel:SetText( Locale.ConvertTextKey("TXT_KEY_MULTIPLAYER_JOINING_HOST" ));

	end

end


-------------------------------------------------
-- Event Handler: MultiplayerJoinRoomFailed
-------------------------------------------------
function OnJoinRoomFailed( iExtendedError, aExtendedErrorText )

	if iExtendedError == NetErrors.MISSING_REQUIRED_DATA then
		local szText = Locale.ConvertTextKey("TXT_KEY_MP_JOIN_FAILED_MISSING_RESOURCES");
		
		-- The aExtendedErrorText will contain an array of the IDs of the resources (DLC/Mods) needed by the game.
		local count = table.count(aExtendedErrorText);
		if(count > 0) then
			szText = szText .. "[NEWLINE]";
		    for index, value in pairs(aExtendedErrorText) do 		
				szText = szText .. "[NEWLINE] [ICON_BULLET]" .. Locale.ConvertTextKey(value);
			end
		end
		Events.FrontEndPopup.CallImmediate( szText );
	elseif iExtendedError == NetErrors.ROOM_FULL then
		Events.FrontEndPopup.CallImmediate( "TXT_KEY_MP_ROOM_FULL" );
	else
		Events.FrontEndPopup.CallImmediate( "TXT_KEY_MP_JOIN_FAILED" );
	end

	g_joinFailed = true;	
	Matchmaking.LeaveMultiplayerGame();
	UIManager:DequeuePopup( ContextPtr );
end

-------------------------------------------------
-- Event Handler: MultiplayerConnectionFailed
-------------------------------------------------
function OnMultiplayerConnectionFailed()

	-- We should only get this if we couldn't complete the connection to the host of the room	
	g_joinFailed = true;	
  Events.FrontEndPopup.CallImmediate( "TXT_KEY_MP_JOIN_FAILED" );
	Matchmaking.LeaveMultiplayerGame();
	UIManager:DequeuePopup( ContextPtr );
	
end

-------------------------------------------------
-- Event Handler: MultiplayerGameAbandoned
-------------------------------------------------
function OnMultiplayerGameAbandoned(eReason)

	if (eReason == NetKicked.BY_HOST) then
		Events.FrontEndPopup.CallImmediate( "TXT_KEY_MP_KICKED" );
	elseif (eReason == NetKicked.NO_ROOM) then
		Events.FrontEndPopup.CallImmediate( "TXT_KEY_MP_ROOM_FULL" );
	else
	    Events.FrontEndPopup.CallImmediate( "TXT_KEY_MP_JOIN_FAILED" );
	end
	g_joinFailed = true;	
	Matchmaking.LeaveMultiplayerGame();
	UIManager:DequeuePopup( ContextPtr );
	
end

-------------------------------------------------
-- Event Handler: ConnectedToNetworkHost
-------------------------------------------------
function OnHostConnect()
	Controls.JoiningLabel:SetText( Locale.ConvertTextKey("TXT_KEY_MULTIPLAYER_JOINING_PLAYERS" ));  
end

-------------------------------------------------
-- Event Handler: MultiplayerConnectionComplete
-------------------------------------------------
function OnConnectionCompete()

	if not Matchmaking.IsHost() then

		UIManager:QueuePopup( Controls.StagingRoomScreen, PopupPriority.StagingScreen );
        UIManager:DequeuePopup( ContextPtr );
	end
    
end

-------------------------------------------------
-- Event Handler: MultiplayerNetRegistered
-------------------------------------------------
function OnNetRegistered()
	Controls.JoiningLabel:SetText( Locale.ConvertTextKey("TXT_KEY_MULTIPLAYER_JOINING_GAMESTATE" ));    
end

-------------------------------------------------
-- Event Handler: PlayerVersionMismatchEvent
-------------------------------------------------
function OnVersionMismatch( iPlayerID, playerName, bIsHost )
	if( bIsHost ) then
		Events.FrontEndPopup.CallImmediate( Locale.ConvertTextKey( "TXT_KEY_MP_VERSION_MISMATCH_FOR_HOST", playerName ) );
		Matchmaking.KickPlayer( iPlayerID );
	else
		-- we mismatched with the host, exit the game.
		Events.FrontEndPopup.CallImmediate( Locale.ConvertTextKey( "TXT_KEY_MP_VERSION_MISMATCH_FOR_PLAYER" ) );
		g_joinFailed = true;	
		Matchmaking.LeaveMultiplayerGame();
		UIManager:DequeuePopup( ContextPtr );
	end
end
Events.PlayerVersionMismatchEvent.Add( OnVersionMismatch );

-------------------------------------------------
-- Show / Hide Handler
-------------------------------------------------
function ShowHideHandler( bIsHide, bIsInit )
	if( not bIsInit ) then
		if not bIsHide then
 			g_joinFailed = false;

			-- Activate only the DLC allowed for this MP game.  Mods will also deactivated/activate too.
			if (not ContextPtr:IsHotLoad()) then 
				local prevCursor = UIManager:SetUICursor( 1 );
				Modding.ActivateAllowedDLC();
				UIManager:SetUICursor( prevCursor );
				
				-- Send out an event to continue on, as the ActivateDLC may have swapped out the UI	
				Events.SystemUpdateUI( SystemUpdateUIType.RestoreUI, "JoiningRoom" );
			end
						
			Controls.JoiningLabel:LocalizeAndSetText( "TXT_KEY_MULTIPLAYER_JOINING_ROOM" );
			RegisterEvents();
    else
    	UnregisterEvents();
    end
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );


-------------------------------------------------
-------------------------------------------------
function RegisterEvents()
    Events.MultiplayerJoinRoomComplete.Add( OnJoinRoomComplete );
    Events.MultiplayerJoinRoomFailed.Add( OnJoinRoomFailed );
    Events.ConnectedToNetworkHost.Add ( OnHostConnect );
    Events.MultiplayerConnectionComplete.Add( OnConnectionCompete );
    Events.MultiplayerNetRegistered.Add( OnNetRegistered );   
    Events.MultiplayerConnectionFailed.Add( OnMultiplayerConnectionFailed );
    Events.MultiplayerGameAbandoned.Add( OnMultiplayerGameAbandoned );
end


-------------------------------------------------
-------------------------------------------------
function UnregisterEvents()
    Events.MultiplayerJoinRoomComplete.Remove( OnJoinRoomComplete );
    Events.MultiplayerJoinRoomFailed.Remove( OnJoinRoomFailed );
    Events.ConnectedToNetworkHost.Remove ( OnHostConnect );
    Events.MultiplayerConnectionComplete.Remove( OnConnectionCompete );
		Events.MultiplayerNetRegistered.Remove( OnNetRegistered );   
    Events.MultiplayerConnectionFailed.Remove( OnMultiplayerConnectionFailed );
    Events.MultiplayerGameAbandoned.Remove( OnMultiplayerGameAbandoned );
end


-------------------------------------------------
-------------------------------------------------
function AdjustScreenSize()
    local _, screenY = UIManager:GetScreenSizeVal();
    local TOP_COMPENSATION = 52 + ((screenY - 768) * 0.3 );

    Controls.MainGrid:SetSizeY( screenY - TOP_COMPENSATION );
    Controls.MainGrid:ReprocessAnchoring();

end


-------------------------------------------------
-------------------------------------------------
function OnUpdateUI( type, tag, iData1, iData2, strData1)
    if( type == SystemUpdateUIType.ScreenResize ) then
        AdjustScreenSize();
    elseif (not g_joinFailed and type == SystemUpdateUIType.RestoreUI and tag == "JoiningRoom") then
			if (ContextPtr:IsHidden()) then
				UIManager:QueuePopup(ContextPtr, PopupPriority.JoiningScreen );    
			end
    end
end
Events.SystemUpdateUI.Add( OnUpdateUI );

AdjustScreenSize();
