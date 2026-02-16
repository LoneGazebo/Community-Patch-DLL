if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
----------------------------------------------------------------
----------------------------------------------------------------
include( "IconSupport" );
include( "CommonBehaviors" );

local m_strAudio = "";
local m_bAllowBack = true;

----------------------------------------------------------------
----------------------------------------------------------------
function OnMainMenu()
	UIManager:DequeuePopup( ContextPtr );

	if (Game.IsHotSeat() and Game.CountHumanPlayersAlive() > 0) then
		local iActivePlayer = Game.GetActivePlayer();
		local pPlayer = Players[iActivePlayer];

		-- If the player is not alive, but there are other humans in the game that are
		-- find the next human player alive and make them the active player
		if (not pPlayer:IsAlive()) then
			local bFound = false;
			
			local iNextPlayer = iActivePlayer + 1;
			for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do		
				if (iNextPlayer >= GameDefines.MAX_MAJOR_CIVS) then
					iNextPlayer = 0;
				end 
				pPlayer = Players[iNextPlayer];
				if (iActivePlayer ~= iNextPlayer and pPlayer:IsAlive() and pPlayer:IsHuman()) then
					Game.SetActivePlayer( iNextPlayer );
					-- Restore the game state
					Game.SetGameState(GameplayGameStateTypes.GAMESTATE_ON);
					bFound = true;
					break;
				end
				iNextPlayer = iNextPlayer + 1;
			end
							
			UIManager:DequeuePopup( ContextPtr );
			Controls.BackgroundImage:UnloadTexture();	
			
			if (not bFound) then
				-- Fail safe
				Events.ExitToMainMenu();
			end
		else
			Events.ExitToMainMenu();
		end		
	else
		Events.ExitToMainMenu();
	end
end
Controls.MainMenuButton:RegisterCallback( Mouse.eLClick, OnMainMenu );

----------------------------------------------------------------
----------------------------------------------------------------
function OnBack()
	if (m_bAllowBack) then
		Network.SendExtendedGame();
		UIManager:DequeuePopup( ContextPtr );
		Controls.BackgroundImage:UnloadTexture();	
	end
end
Controls.BackButton:RegisterCallback( Mouse.eLClick, OnBack );

function GetVictoryAudio (strName) 
	for v in GameInfo.Victories() do 
		if (v.Type == strName) then
			return v.Audio;
		end
	end
	
	return nil;
end

----------------------------------------------------------------
----------------------------------------------------------------
local m_fTime = 0;
local deferredDisplayTime = 0;

function OnUpdate( fDTime )
	
	-- Cap the delta time to eliminate overshooting because of delays in processing.
	local fMaxDTime = fDTime;
	if (fMaxDTime > 0.1) then
		fMaxDTime = 0.1;
	end

	-- Delaying the display so we can see some animation?	
	if (deferredDisplayTime > 0) then
		-- Yes, hide the UI
		Controls.BGBlock:SetHide(true);
		Controls.BGWin:SetHide(true);
		deferredDisplayTime = deferredDisplayTime - fMaxDTime;
		if (deferredDisplayTime <= 0) then	
			Controls.BGBlock:SetHide(false);
			Controls.BGWin:SetHide(false);
			deferredDisplayTime = 0;
		end
	else
		local fNewTime = m_fTime + fMaxDTime;
		if (m_strAudio and m_fTime < 2 and fNewTime >= 2) then
			-- print("Play audio: " .. m_strAudio);
			Events.AudioPlay2DSound(m_strAudio);
		end
		
		if (g_AnimUpdate ~= nil) then
			if (g_AnimUpdate(fMaxDTime)) then
				g_AnimUpdate = nil;
			end
		end
		
		m_fTime = fNewTime;
	end	
	
end
ContextPtr:SetUpdate( OnUpdate );


----------------------------------------------------------------
----------------------------------------------------------------
function OnDisplay( type, team )

	if(not ContextPtr:IsHidden()) then
		return;
	end
	
	UIManager:SetUICursor( 0 );
	m_strAudio = nil;
	m_bAllowBack = true;
	local bTutorialGame = false;
	local playerID = Game.GetActivePlayer();
	local player = Players[playerID];

	-- Hide the Beyond Earth button by default.
	if(Controls.BeyondButton ~= nil) then
		Controls.BeyondButton:SetHide(true);
		Controls.ButtonStack:CalculateSize();
		Controls.ButtonStack:ReprocessAnchoring();
	end

	if( team == Game.GetActiveTeam() ) then
    	Controls.EndGameText:SetText( Locale.ConvertTextKey( "TXT_KEY_VICTORY_BANG" ) );
		local victoryType = nil;
		if( type == EndGameTypes.Tutorial1 ) then
	    	Controls.EndGameText:SetText( Locale.ConvertTextKey( "TXT_KEY_TUTORIAL_1_COMPLETE" ) );
	    	bTutorialGame = true;
		elseif( type == EndGameTypes.Tutorial2 ) then
	    	Controls.EndGameText:SetText( Locale.ConvertTextKey( "TXT_KEY_TUTORIAL_2_COMPLETE" ) );
	    	bTutorialGame = true;
		elseif( type == EndGameTypes.Tutorial3 ) then
	    	Controls.EndGameText:SetText( Locale.ConvertTextKey( "TXT_KEY_TUTORIAL_3_COMPLETE" ) );
	    	bTutorialGame = true;
		elseif( type == EndGameTypes.Tutorial4 ) then
	    	Controls.EndGameText:SetText( Locale.ConvertTextKey( "TXT_KEY_TUTORIAL_4_COMPLETE" ) );
	    	bTutorialGame = true;
		elseif( type == EndGameTypes.Tutorial5 ) then
	    	Controls.EndGameText:SetText( Locale.ConvertTextKey( "TXT_KEY_TUTORIAL_5_COMPLETE" ) );
	    	bTutorialGame = true;
		elseif( type == EndGameTypes.Technology ) then
	    	Controls.EndGameText:SetText( Locale.ConvertTextKey( "TXT_KEY_VICTORY_FLAVOR_TECHNOLOGY" ) );
	    	victoryType = "VICTORY_SPACE_RACE";
	    	deferredDisplayTime = 7;
	    	
	    	-- Space victory, show BE button.
	    	if(Controls.BeyondButton ~= nil) then
	    		Controls.BeyondButton:SetHide(false);
	    		Controls.ButtonStack:CalculateSize();
				Controls.ButtonStack:ReprocessAnchoring();
	    	end
	    	
		elseif( type == EndGameTypes.Domination ) then
	    	Controls.EndGameText:SetText( Locale.ConvertTextKey( "TXT_KEY_VICTORY_FLAVOR_DOMINATION" ) );
	    	victoryType = "VICTORY_DOMINATION";
		elseif( type == EndGameTypes.Culture ) then
	    	Controls.EndGameText:SetText( Locale.ConvertTextKey( "TXT_KEY_VICTORY_FLAVOR_CULTURE" ) );
	    	victoryType = "VICTORY_CULTURAL";
		elseif( type == EndGameTypes.Diplomatic ) then
	    	Controls.EndGameText:SetText( Locale.ConvertTextKey( "TXT_KEY_VICTORY_FLAVOR_DIPLOMACY" ) );
	    	victoryType = "VICTORY_DIPLOMATIC";
		elseif( type == EndGameTypes.Time ) then
	    	Controls.EndGameText:SetText( Locale.ConvertTextKey( "TXT_KEY_VICTORY_FLAVOR_TIME" ) );
	    	victoryType = "VICTORY_TIME";
		end
		
		if(victoryType ~= nil and PreGame.GetGameOption("GAMEOPTION_NO_EXTENDED_PLAY") ~= 1)then
			Controls.BackButton:SetDisabled( false );
		else
			Controls.BackButton:SetDisabled( true );
			m_bAllowBack = false;
		end
			
		--print(victoryType);
		if(victoryType)then
			m_strAudio = GetVictoryAudio(victoryType);
			Controls.BackgroundImage:SetTexture(GameInfo.Victories[victoryType].VictoryBackground);    		
		else
			m_strAudio = GetVictoryAudio("VICTORY_TIME");
			Controls.BackgroundImage:SetTexture(GameInfo.Victories["VICTORY_TIME"].VictoryBackground); 		
		end
	else
		m_bAllowBack = false;
    	Controls.EndGameText:SetText( Locale.ConvertTextKey( "TXT_KEY_DEFEAT_BANG" ) );
    	Controls.EndGameText:SetText( Locale.ConvertTextKey( "TXT_KEY_VICTORY_FLAVOR_LOSS" ) );
    	Controls.BackgroundImage:UnloadTexture();
    	Controls.BackgroundImage:SetTexture( "Victory_Defeat.dds" );
		Controls.BackButton:SetDisabled( true );
		if (not Game:IsNetworkMultiPlayer() and player:IsAlive() and PreGame.GetGameOption("GAMEOPTION_NO_EXTENDED_PLAY") ~= 1) then
			if( type == EndGameTypes.Technology ) then
				Controls.BackButton:SetDisabled( false );
				m_bAllowBack = true;
			elseif( type == EndGameTypes.Domination ) then
				Controls.BackButton:SetDisabled( false );
				m_bAllowBack = true;
			elseif( type == EndGameTypes.Culture ) then
				Controls.BackButton:SetDisabled( false );
				m_bAllowBack = true;
			elseif( type == EndGameTypes.Diplomatic ) then
				Controls.BackButton:SetDisabled( false );
				m_bAllowBack = true;
			elseif( type == EndGameTypes.Time ) then
				Controls.BackButton:SetDisabled( false );
				m_bAllowBack = true;
			elseif( type == EndGameTypes.Loss ) then
				Controls.BackButton:SetDisabled( false );
				m_bAllowBack = true;
			end
		end

		m_strAudio = "AS2D_VICTORY_SPEECH_LOSS";
	end 

	Controls.RankingButton:SetHide( bTutorialGame );
	Controls.ReplayButton:SetHide( bTutorialGame );
	Controls.DemographicsButton:SetHide( bTutorialGame );
	
	local sizeY = Controls.EndGameText:GetSizeY();
	Controls.GameOverContainer:SetSizeY(sizeY + 30);
	
	g_AnimUpdate = ZoomOutEffect{
		SplashControl = Controls.BackgroundImage, 
		ScaleFactor = 0.5,
		AnimSeconds = 3
	};
	
	UIManager:QueuePopup( ContextPtr, PopupPriority.EndGameMenu );
	
end
Events.EndGameShow.Add( OnDisplay );


----------------------------------------------------------------
----------------------------------------------------------------
function OnGameOver()
    Controls.Ranking:SetHide( true );
    Controls.EndGameReplay:SetHide( true );
    Controls.EndGameDemographics:SetHide( true );
    Controls.GameOverContainer:SetHide( false );
    Controls.BackgroundImage:SetColor({x=1,y=1,z=1,w=1});
end
Controls.GameOverButton:RegisterCallback( Mouse.eLClick, OnGameOver );


----------------------------------------------------------------
----------------------------------------------------------------
function OnDemographics()
    Controls.Ranking:SetHide( true );
    Controls.EndGameReplay:SetHide( true );
    Controls.EndGameDemographics:SetHide( false );
    Controls.GameOverContainer:SetHide( true );
    Controls.BackgroundImage:SetColor({x=1,y=1,z=1,w=0.125});
end
Controls.DemographicsButton:RegisterCallback( Mouse.eLClick, OnDemographics );


----------------------------------------------------------------
----------------------------------------------------------------
function OnReplay()
    Controls.Ranking:SetHide( true );
    Controls.EndGameReplay:SetHide( false );
    Controls.EndGameDemographics:SetHide( true );
    Controls.GameOverContainer:SetHide( true );
    Controls.BackgroundImage:SetColor({x=1,y=1,z=1,w=0.125});
end
Controls.ReplayButton:RegisterCallback( Mouse.eLClick, OnReplay );


----------------------------------------------------------------
----------------------------------------------------------------
function OnRanking()
    Controls.Ranking:SetHide( false );
    Controls.EndGameReplay:SetHide( true );
    Controls.EndGameDemographics:SetHide( true );
    Controls.GameOverContainer:SetHide( true );
    Controls.BackgroundImage:SetColor({x=1,y=1,z=1,w=0.125});
end
Controls.RankingButton:RegisterCallback( Mouse.eLClick, OnRanking );


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
ContextPtr:SetInputHandler( InputHandler );


----------------------------------------------------------------        
----------------------------------------------------------------        
function ShowHideHandler( bIsHide, bIsInit )

	CivIconHookup( Game.GetActivePlayer(), 80, Controls.CivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true );
    if( not bIsInit ) then
        if( not bIsHide ) then
            UI.incTurnTimerSemaphore();
			-- Display a continue button if there are other players left in the game
			local pPlayer = Players[Game.GetActivePlayer()];
			if (Game.IsHotSeat() and Game.CountHumanPlayersAlive() > 0 and not pPlayer:IsAlive()) then
				Controls.MainMenuButton:SetText( Locale.ConvertTextKey( "TXT_KEY_MP_PLAYER_CHANGE_CONTINUE" ) );
			else
				Controls.MainMenuButton:SetText( Locale.ConvertTextKey( "TXT_KEY_MENU_EXIT_TO_MAIN" ) );
			end
            ContextPtr:SetUpdate( OnUpdate );
			m_fTime = 0;
        else
            UI.decTurnTimerSemaphore();
        end
    end

end
ContextPtr:SetShowHideHandler( ShowHideHandler );


function ShowBeyondEarthStorePage()
	Steam.ActivateGameOverlayToStore(65980);
end
Controls.BeyondButton:RegisterCallback( Mouse.eLClick, ShowBeyondEarthStorePage);