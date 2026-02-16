if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Change player bumper screen
-------------------------------------------------
include( "IconSupport" );

local g_bIsOpen = false;
-------------------------------------------------
-------------------------------------------------
function OnContinue()
	if (PreGame.TestPassword( Game.GetActivePlayer(), Controls.EnterPasswordEditBox:GetText() )) then
		g_bIsOpen = false;
		UIManager:PopModal( ContextPtr );
	end
end
Controls.ContinueButton:RegisterCallback( Mouse.eLClick, OnContinue );

-------------------------------------------------
-------------------------------------------------
function OnChangePassword()
	UIManager:PushModal(Controls.ChangePassword, true);		
end
Controls.ChangePasswordButton:RegisterCallback( Mouse.eLClick, OnChangePassword );

---------------------------------------------------------------
function Validate()

	local bValid = PreGame.TestPassword( Game.GetActivePlayer(), Controls.EnterPasswordEditBox:GetText() );	
	Controls.ContinueButton:SetDisabled(not bValid);
end
---------------------------------------------------------------
Controls.EnterPasswordEditBox:RegisterCallback(Validate);

----------------------------------------------------------------        
----------------------------------------------------------------        
function OnSave()
    UIManager:PushModal( Controls.SaveMenu, false );
end
Controls.SaveButton:RegisterCallback( Mouse.eLClick, OnSave );

-------------------------------------------------
-------------------------------------------------
function OnMainMenu()
	Controls.Message:SetText( Locale.ConvertTextKey("TXT_KEY_MENU_RETURN_MM_WARN") );
	Controls.ExitConfirm:SetHide( false );
	Controls.MainContainer:SetHide( true );
end
Controls.MainMenuButton:RegisterCallback( Mouse.eLClick, OnMainMenu );

----------------------------------------------------------------        
----------------------------------------------------------------
function OnYes( )
	Controls.ExitConfirm:SetHide( true );
   	
   	UIManager:SetUICursor( 1 );
	Events.ExitToMainMenu();
end
Controls.Yes:RegisterCallback( Mouse.eLClick, OnYes );

function OnNo( )
	Controls.ExitConfirm:SetHide( true );
	Controls.MainContainer:SetHide( false );
end
Controls.No:RegisterCallback( Mouse.eLClick, OnNo );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
	if ( not Controls.MainContainer:IsHidden() ) then
		if (Game.GetNumGameTurnActive() ~= 0) then
			if uiMsg == KeyEvents.KeyDown then
				if wParam == Keys.VK_RETURN then
					OnContinue();
					return true;
				end
			end
		end
	end
    return false;
end
ContextPtr:SetInputHandler( InputHandler );

-------------------------------------------------
-------------------------------------------------
function OnPasswordChanged( ePlayer )	
	local bShowPassword = PreGame.HasPassword( ePlayer );
	Controls.Stack:SetHide(not bShowPassword);
	
	-- Adjust the overall size of the box
	local iBoxHeight = 240;
	if ( bShowPassword ) then
		iBoxHeight = 320;
		Controls.ChangePasswordLabel:LocalizeAndSetText("TXT_KEY_MP_CHANGE_PASSWORD");
	else
		Controls.ChangePasswordLabel:LocalizeAndSetText("TXT_KEY_MP_ADD_PASSWORD");	
	end
	
	local frameSize = {};
	frameSize = Controls.MainBox:GetSize();
	frameSize.y = iBoxHeight;
	Controls.MainBox:SetSize( frameSize );
	frameSize.y = iBoxHeight;
	frameSize.y = iBoxHeight;
	
   	Controls.EnterPasswordEditBox:TakeFocus();
   	if (bShowPassword) then
   		local bValid = PreGame.TestPassword( Game.GetActivePlayer(), Controls.EnterPasswordEditBox:GetText());
		Controls.ContinueButton:SetDisabled( not bValid );   	
	else
		Controls.ContinueButton:SetDisabled( false );   	
	end
end

----------------------------------------------------------
----------------------------------------------------------
function UpdatePlayer()

	Controls.MainContainer:SetHide( false );
	local eActivePlayer = Game.GetActivePlayer();
	if (eActivePlayer ~= -1) then
		local szName = PreGame.GetNickName( eActivePlayer );		
		if (szName == "") then
			szName = PreGame.GetLeaderName( eActivePlayer );
			if (szName == "") then
				-- Get the player's leader type and get the leader description.
				local pPlayer = Players[ eActivePlayer ];
				local leaderType = pPlayer:GetLeaderType();			
				local leaderInfo = GameInfo.Leaders[leaderType];	
				szName = Locale.ConvertTextKey( leaderInfo.Description  );
			end			
		end
		Controls.Title:SetText( szName );
		
		local pPlayer = Players[eActivePlayer];
		if(pPlayer ~= nil) then
			local civ = GameInfo.Civilizations[pPlayer:GetCivilizationType()];
			if(civ ~= nil) then
				IconHookup( civ.PortraitIndex, 128, civ.IconAtlas, Controls.Icon );
			end
		end
		
	end
	Controls.EnterPasswordEditBox:SetText( "" );
	OnPasswordChanged( eActivePlayer );
end
-------------------------------------------------
-------------------------------------------------
function ShowHideHandler( bIsHide, bIsInit )
    
	if( not bIsHide ) then
		if (Game.GetNumGameTurnActive() ~= 0) then
			UpdatePlayer();
		end
	end	
	
    if( not bIsInit ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
			if (Game.GetNumGameTurnActive() ~= 0) then
				-- If this player is going to take a turn then pause
				Game.SetPausePlayer( Game.GetActivePlayer() );
		   		g_bIsOpen = true;
			else
				-- We coming in here and there is no player whose turn is active?
				Controls.MainContainer:SetHide( true );
				UIManager:PopModal( ContextPtr );	-- Close ourselves (ShowHideHandler will get called again with bIsHide==true)
			end
        else
            UI.decTurnTimerSemaphore();
			Game.SetPausePlayer( -1 );
			g_bIsOpen = false;
        end
        
    end
	
end
ContextPtr:SetShowHideHandler( ShowHideHandler );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
LuaEvents.PasswordChanged.Add( function(ePlayer)
	OnPasswordChanged( ePlayer );
end);

-------------------------------------------------
-- Player changed handler
-------------------------------------------------
function OnPlayerChangedEvent()
	if (g_bIsOpen) then
		Game.SetPausePlayer( Game.GetActivePlayer() );
		UpdatePlayer();
	end
end
Events.GameplaySetActivePlayer.Add( OnPlayerChangedEvent );
