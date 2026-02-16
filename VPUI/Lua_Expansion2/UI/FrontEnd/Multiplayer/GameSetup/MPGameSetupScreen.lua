if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Multiplayer Game Setup Screen
-------------------------------------------------
include( "IconSupport" );
include( "UniqueBonuses" );
include( "InstanceManager" );

-------------------------------------------------
-- The common Game Options handler code is in
-- MPGameOptions, which is used by the StagingRoom
-- as well as this UI
-------------------------------------------------

include( "MPGameOptions" );

-------------------------------------------------
-------------------------------------------------
function ShowHideExitButton()
	local bShow = Network.IsDedicatedServer();
	Controls.ExitButton:SetHide( not bShow );
end

-------------------------------------------------
-------------------------------------------------
function OnExitGame()
	Events.UserRequestClose();
end
Controls.ExitButton:RegisterCallback( Mouse.eLClick, OnExitGame );

-------------------------------------------------
-------------------------------------------------
function ShowHideBackButton()
	local bShow = not Network.IsDedicatedServer();
	Controls.BackButton:SetHide( not bShow );
end

-------------------------------------------------
-------------------------------------------------
function OnBack()
	if (not Network.IsDedicatedServer()) then
		if (not PreGame.IsHotSeatGame() ) then
			if PreGame.IsInternetGame() then
				Matchmaking.RefreshInternetGameList();
			else
				Matchmaking.RefreshLANGameList();
			end
		end
	end
	UIManager:DequeuePopup( ContextPtr );
end
Controls.BackButton:RegisterCallback( Mouse.eLClick, OnBack );


-------------------------------------------------
-------------------------------------------------
function OnStart()
	PreGame.SetPersistSettings(false); -- Do not save these settings out for the "Play Now" option.
	
	if( bIsModding and IsWBMap(PreGame.GetMapScript()) ) then
		PreGame.SetRandomMapScript(false);
		PreGame.SetLoadWBScenario(PreGame.GetLoadWBScenario());
	else
		PreGame.SetLoadWBScenario(false);
	end
    
	PreGame.SetLoadFileName( "" );
	PreGame.ResetSlots();

	local strGameName = Controls.NameBox:GetText();

	local worldInfo = GameInfo.Worlds[ PreGame.GetWorldSize() ];
	if (Network.IsDedicatedServer()) then
		PreGame.SetGameOption("GAMEOPTION_PITBOSS", true);
		local bResult, bPending = Matchmaking.HostServerGame( strGameName, worldInfo.DefaultPlayers, false );
	else	
		if (PreGame.IsInternetGame()) then			
			local bResult, bPending = Matchmaking.HostInternetGame( strGameName, worldInfo.DefaultPlayers );
		else
			if (PreGame.IsHotSeatGame()) then
				local bResult, bPending = Matchmaking.HostHotSeatGame( strGameName, worldInfo.DefaultPlayers );
			else
				local bResult, bPending = Matchmaking.HostLANGame( strGameName, worldInfo.DefaultPlayers );
			end
		end
	end
end
Controls.LaunchButton:RegisterCallback( Mouse.eLClick, OnStart );

-------------------------------------------------
-------------------------------------------------
function OnDefaultButton() 
	ResetMultiplayerOptions();
	
	-- Uncheck Everything
	RefreshGameOptions();
	RefreshDropDownOptions();

	UpdateGameOptionsDisplay();
end
Controls.DefaultButton:RegisterCallback( Mouse.eLClick, OnDefaultButton );

-------------------------------------------------
-------------------------------------------------
function OnLoadGame()
    UIManager:QueuePopup( Controls.MPLoadGameScreen, PopupPriority.MPLoadGameScreen );
end
Controls.LoadGameButton:RegisterCallback( Mouse.eLClick, OnLoadGame );


-------------------------------------------------
-------------------------------------------------
function OnPrivateGame()

	local bChecked = Controls.PrivateGameCheckbox:IsChecked();
	PreGame.SetPrivateGame( bChecked );

end
Controls.PrivateGameCheckbox:RegisterCallback( Mouse.eLClick, OnPrivateGame );


-------------------------------------------------
--UpdateDisplay()
-- This method refreshes UI elements based on PreGame Data.
-- This function should only read from PreGame and not set items.
-- The one exception is the hack for ensuring all Civs are valid.
-------------------------------------------------
function UpdateDisplay()

	UpdateGameOptionsDisplay(true);
	ShowHideExitButton();
	ShowHideBackButton();

	-- Update private game checkbox
	Controls.PrivateGameCheckbox:SetHide( not PreGame.IsInternetGame() );
	Controls.PrivateGameCheckbox:SetCheck( PreGame.IsPrivateGame() );
	
	Controls.GameNameBox:SetHide( PreGame.IsHotSeatGame() );
	Controls.GameNameDivider:SetHide( PreGame.IsHotSeatGame() );
	
	if(PreGame.GetLoadWBScenario()) then
		Controls.LaunchButton:LocalizeAndSetText( "TXT_KEY_HOST_SCENARIO" );
	else
		Controls.LaunchButton:LocalizeAndSetText( "TXT_KEY_HOST_GAME" );
	end	
end

----------------------------------------------------------------        
----------------------------------------------------------------        
function ShowHideHandler( isHide, bInit )
	-- Check to make sure we are not launching, this menu can briefly get unhidden as the game launches
	if not Matchmaking.IsLaunchingGame() and not isHide then 
		bIsModding = (ContextPtr:LookUpControl("../.."):GetID() == "ModMultiplayerSelectScreen");
		if(bIsModding) then
			Controls.TitleLabel:LocalizeAndSetText( "TXT_KEY_MOD_MP_GAME_SETUP_HEADER" );
			Controls.ModsButton:SetHide( false );
		else
			Controls.TitleLabel:LocalizeAndSetText( "TXT_KEY_MULTIPLAYER_GAME_SETUP_HEADER" );
			Controls.ModsButton:SetHide( true );
		end
		
		SetInLobby( false );
		
		if PreGame.GetLoadFileName() == "" then
			-- Default these to their state in the options manager
			PreGame.SetQuickCombat( OptionsManager.GetMultiplayerQuickCombatEnabled() );
			PreGame.SetQuickMovement( OptionsManager.GetMultiplayerQuickMovementEnabled() );		
			if (Network.IsDedicatedServer()) then
				PreGame.SetGameOption("GAMEOPTION_PITBOSS", true);
			end
		end
		
		PopulateMapSizePulldown();

		RefreshMapScripts();	
		PreGame.SetRandomMapScript(false);	-- Random map scripts is not supported in multiplayer
		UpdateDisplay();	
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );


----------------------------------------------------------------        
-- Input processing
----------------------------------------------------------------        
function InputHandler( uiMsg, wParam, lParam )
	if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE then
			OnBack();
            return true;
		end
	end
end
ContextPtr:SetInputHandler( InputHandler );

-----------------------------------------------------------------
-- Adjust for resolution
-----------------------------------------------------------------
function AdjustScreenSize()
    local _, screenY = UIManager:GetScreenSizeVal();
    
    local TOP_COMPENSATION = 52 + ((screenY - 768) * 0.3 );
    local BOTTOM_COMPENSATION = 222;
    if (PreGame.IsHotSeatGame()) then
	    Controls.OptionsScrollPanel:SetOffsetVal( 40, 44 );
	    BOTTOM_COMPENSATION = 160;
	end
    local SIZE = screenY - (TOP_COMPENSATION + BOTTOM_COMPENSATION);

    Controls.MainGrid:SetSizeY( screenY - TOP_COMPENSATION );
    Controls.OptionsScrollPanel:SetSizeY( SIZE );
    Controls.OptionsScrollPanel:CalculateInternalSize();
end


-------------------------------------------------
-------------------------------------------------
function OnUpdateUI( type )
    if( type == SystemUpdateUIType.ScreenResize ) then
        AdjustScreenSize();
    end
end
Events.SystemUpdateUI.Add( OnUpdateUI );

AdjustScreenSize();