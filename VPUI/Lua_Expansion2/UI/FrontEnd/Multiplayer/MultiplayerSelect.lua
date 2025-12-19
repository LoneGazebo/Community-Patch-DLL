if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Multiplayer Select Screen
-------------------------------------------------
include( "MPGameDefaults" );

-------------------------------------------------
-- Globals
-------------------------------------------------
local bIsModding = (ContextPtr:GetID() == "ModMultiplayerSelectScreen");
local pitbossToggle = false;  -- Are we selecting the network mode for Pitboss or standard mp games?
local displayNetworkModes = false;	-- Are we displaying the network mode buttons?

-------------------------------------------------
-- Helper Functions
-------------------------------------------------
function ToggleNetworkModeDisplay(showNetworkMode)
		Controls.StandardButton:SetHide(showNetworkMode);
		Controls.HotSeatButton:SetHide(showNetworkMode);	
		Controls.PitbossButton:SetHide(showNetworkMode);	
		
		-- Show network mode buttons
		Controls.InternetButton:SetHide(not showNetworkMode);
		Controls.LANButton:SetHide(not showNetworkMode);
		
		displayNetworkModes = showNetworkMode;
end

-------------------------------------------------
-- Internet Game Button Handler
-------------------------------------------------
function InternetButtonClick()
	if(pitbossToggle) then
		UI.SetMultiplayerLobbyMode(MultiplayerLobbyMode.LOBBYMODE_PITBOSS_INTERNET);
	else
		UI.SetMultiplayerLobbyMode(MultiplayerLobbyMode.LOBBYMODE_STANDARD_INTERNET);
	end
	PreGame.SetInternetGame( true );
	PreGame.SetGameType(GameTypes.GAME_NETWORK_MULTIPLAYER);
	PreGame.ResetSlots();
	ResetMultiplayerOptions(); -- Apply multiplayer default settings.
	UIManager:QueuePopup( Controls.LobbyScreen, PopupPriority.LobbyScreen );
	
end
Controls.InternetButton:RegisterCallback( Mouse.eLClick, InternetButtonClick );


-------------------------------------------------
-- LAN Game Button Handler
-------------------------------------------------
function LANButtonClick()
	if(pitbossToggle) then
		UI.SetMultiplayerLobbyMode(MultiplayerLobbyMode.LOBBYMODE_PITBOSS_LAN);
	else
		UI.SetMultiplayerLobbyMode(MultiplayerLobbyMode.LOBBYMODE_STANDARD_LAN);
	end
	PreGame.SetInternetGame( false );
	PreGame.SetGameType(GameTypes.GAME_NETWORK_MULTIPLAYER);
	PreGame.ResetSlots();
	ResetMultiplayerOptions(); -- Apply multiplayer default settings.

	UIManager:QueuePopup( Controls.LobbyScreen, PopupPriority.LobbyScreen );

end
Controls.LANButton:RegisterCallback( Mouse.eLClick, LANButtonClick );

-------------------------------------------------
-- Standard Multiplayer Game Button Handler
-------------------------------------------------
function StandardButtonClick()
	pitbossToggle = false;
	
	ToggleNetworkModeDisplay(true);
end
Controls.StandardButton:RegisterCallback( Mouse.eLClick, StandardButtonClick );

-------------------------------------------------
-- Hotseat Game Button Handler
-------------------------------------------------
function HotSeatButtonClick()

	PreGame.SetGameType(GameTypes.GAME_HOTSEAT_MULTIPLAYER);
	ResetMultiplayerOptions(); -- Apply multiplayer default settings.

	UIManager:QueuePopup( Controls.MPGameSetupScreen, PopupPriority.MPGameSetupScreen );

end
Controls.HotSeatButton:RegisterCallback( Mouse.eLClick, HotSeatButtonClick );

-------------------------------------------------
-- Pitboss Game Button Handler
-------------------------------------------------
function PitbossButtonClick()
	pitbossToggle = true;
	
	ToggleNetworkModeDisplay(true);
end
Controls.PitbossButton:RegisterCallback( Mouse.eLClick, PitbossButtonClick );

-------------------------------------------------
-- LAN Game Button Handler
-------------------------------------------------
function ReconnectButtonClick()
	Network.Reconnect();
	--PreGame.SetInternetGame( false );

	--Controls.LobbyScreen:SetHide( false );
end
Controls.ReconnectButton:RegisterCallback( Mouse.eLClick, ReconnectButtonClick );



-------------------------------------------------
-- Back Button Handler
-------------------------------------------------
function BackButtonClick()
	if(displayNetworkModes) then
		ToggleNetworkModeDisplay(false);
	else
		PreGame.SetInternetGame( false );
		PreGame.SetGameType( GAME_SINGLE_PLAYER );
		PreGame.ResetSlots();
		UIManager:DequeuePopup( ContextPtr );
	end
end
Controls.BackButton:RegisterCallback( Mouse.eLClick, BackButtonClick );


-------------------------------------------------
-- Input Handler
-------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )

	if uiMsg == KeyEvents.KeyDown then

		if wParam == Keys.VK_ESCAPE then

			BackButtonClick();

		end

	end

	return true;

end
ContextPtr:SetInputHandler( InputHandler );


-------------------------------------------------
-- Show / Hide Handler
-------------------------------------------------
function ShowHideHandler( bIsHide )

	if( not Network.IsConnectedToSteam() ) then
		Controls.InternetButton:SetDisabled( true );
		Controls.InternetButton:LocalizeAndSetToolTip("TXT_KEY_STEAM_CONNECTED_NO");
	end

	if not bIsHide then
		
		if(bIsModding) then
			Controls.Title:LocalizeAndSetText("{TXT_KEY_MODS_SELECT_MULTIPLAYER_TYPE:upper}");
		else
			Controls.Title:LocalizeAndSetText("{TXT_KEY_MULTIPLAYER_SELECT_MULTIPLAYER_TYPE:upper}");
		end
		
		if Network.HasReconnectCache() then
			Controls.ReconnectButton:SetHide( false);
			Controls.ReconnectButton:SetDisabled( false );
		else
			Controls.ReconnectButton:SetHide( true );
			Controls.ReconnectButton:SetDisabled( true );
			
			-- To prevent settings getting carried over from scenarios and what not
			-- reset pregame here.
			if (not ContextPtr:IsHotLoad()) then
				UIManager:SetUICursor( 1 );
				Modding.ActivateDLC();
				PreGame.Reset();
				UIManager:SetUICursor( 0 );				
			end
		end
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

-------------------------------------------------
-- Event Handler: MultiplayerGameLaunched
-------------------------------------------------
function OnGameLaunched()

	UIManager:DequeuePopup( ContextPtr );

end
Events.MultiplayerGameLaunched.Add( OnGameLaunched );

-------------------------------------------------
-------------------------------------------------
function OnUpdateUI( type, tag, iData1, iData2, strData1 )
    if (type == SystemUpdateUIType.RestoreUI and tag == "MultiplayerSelect") then
		if (ContextPtr:IsHidden()) then
			UIManager:QueuePopup(ContextPtr, PopupPriority.MultiplayerSelectScreen );    
		end
    end
end
Events.SystemUpdateUI.Add( OnUpdateUI );
