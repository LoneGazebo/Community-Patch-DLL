if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Multiplayer Select Screen
-------------------------------------------------
include( "InstanceManager" );

g_InstanceManager = InstanceManager:new( "ModInstance", "Label", Controls.ModsStack );

-------------------------------------------------
-- Internet Game Button Handler
-------------------------------------------------
function InternetButtonClick()
	PreGame.SetInternetGame( true );
	PreGame.SetGameType(GameTypes.GAME_NETWORK_MULTIPLAYER);
	PreGame.ResetSlots();
	Matchmaking.RefreshInternetGameList();
	UIManager:QueuePopup( Controls.LobbyScreen, PopupPriority.LobbyScreen );
	
end
Controls.InternetButton:RegisterCallback( Mouse.eLClick, InternetButtonClick );


-------------------------------------------------
-- LAN Game Button Handler
-------------------------------------------------
function LANButtonClick()
	PreGame.SetInternetGame( false );
	PreGame.SetGameType(GameTypes.GAME_NETWORK_MULTIPLAYER);
	PreGame.ResetSlots();
	Matchmaking.RefreshLANGameList();

	UIManager:QueuePopup( Controls.LobbyScreen, PopupPriority.LobbyScreen );
end
Controls.LANButton:RegisterCallback( Mouse.eLClick, LANButtonClick );

-------------------------------------------------
-- Hotseat Game Button Handler
-------------------------------------------------
function HotSeatButtonClick()
	PreGame.SetGameType(GameTypes.GAME_HOTSEAT_MULTIPLAYER);
	
	UIManager:QueuePopup( Controls.MPGameSetupScreen, PopupPriority.MPGameSetupScreen );
end
Controls.HotSeatButton:RegisterCallback( Mouse.eLClick, HotSeatButtonClick );

-------------------------------------------------
-- LAN Game Button Handler
-------------------------------------------------
function ReconnectButtonClick()
	Network.Reconnect();
end
Controls.ReconnectButton:RegisterCallback( Mouse.eLClick, ReconnectButtonClick );



-------------------------------------------------
-- Back Button Handler
-------------------------------------------------
function BackButtonClick()
	PreGame.SetInternetGame( false );
	PreGame.SetGameType( GAME_SINGLE_PLAYER );
	PreGame.ResetSlots();
    UIManager:DequeuePopup( ContextPtr );

end
Controls.BackButton:RegisterCallback( Mouse.eLClick, BackButtonClick );


-------------------------------------------------
-- Input Handler
-------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )

	if (uiMsg == KeyEvents.KeyDown and wParam == Keys.VK_ESCAPE) then
		BackButtonClick();
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

	if (not bIsHide) then
		
		if Network.HasReconnectCache() then
			Controls.ReconnectButton:SetHide( false);
			Controls.ReconnectButton:SetDisabled( false );
		else
			Controls.ReconnectButton:SetHide( true );
			Controls.ReconnectButton:SetDisabled( true );
			
			-- To prevent settings getting carried over from scenarios and what not
			-- reset pregame here.
			UIManager:SetUICursor( 1 );
			PreGame.Reset();
			UIManager:SetUICursor( 0 );
		end
		
		g_InstanceManager:ResetInstances();
		
		local mods = Modding.GetEnabledModsByActivationOrder();
		
		if(#mods == 0) then
			Controls.ModsInUseLabel:SetHide(true);
		else
			Controls.ModsInUseLabel:SetHide(false);
			for i,v in ipairs(mods) do
				local displayName = Modding.GetModProperty(v.ModID, v.Version, "Name");
				local displayNameVersion = string.format("[ICON_BULLET] %s (v. %i)", displayName, v.Version);			
				local listing = g_InstanceManager:GetInstance();
				listing.Label:SetText(displayNameVersion);
				listing.Label:SetToolTipString(displayNameVersion);
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

