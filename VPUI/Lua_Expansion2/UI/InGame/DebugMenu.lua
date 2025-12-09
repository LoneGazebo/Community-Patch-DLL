if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Debug Menu 
-------------------------------------------------
include( "FLuaVector" );

Controls.VersionNumber_Label:SetText(UI.GetVersionInfo());

Controls.SyncRandSeedLabel:SetText("SynchRandSeed="..tostring(Network.GetSynchRandSeed()));
Controls.MapRandSeedLabel:SetText("MapRandSeed="..tostring(Network.GetMapRandSeed()));

-------------------------------------------------
-- TogglePauseGameplay Button Handler
-------------------------------------------------
function TogglePauseGameplayClicked()
    bPauseGameplay = not bPauseGameplay;
    if bPauseGameplay == false 
    then
        Controls.TogglePauseGameplay_Button:SetText( "Pause Game" );
    else
        Controls.TogglePauseGameplay_Button:SetText( "Unpause Game" );
    end
	TogglePauseGameplay( bPauseGameplay );
end
bPauseGameplay = false;
Controls.TogglePauseGameplay_Button:RegisterCallback( Mouse.eLClick, TogglePauseGameplayClicked );

-------------------------------------------------
-- RevealAll Button Handler
-------------------------------------------------
function RevealAll()
    FOW_SetAll(0); -- corresponds to gameplay FOGOFWARMODE_OFF
    Game.UpdateFOW(false);
end
Controls.FOW_RevealAll_Button:RegisterCallback( Mouse.eLClick, RevealAll );

-------------------------------------------------
-- Reset Achievements Button Handler
-------------------------------------------------
function OnResetAchievementsButton()

	ResetAchievements(); -- NOP in Final Release

end
Controls.ResetAchievements_Button:RegisterCallback( Mouse.eLClick, OnResetAchievementsButton );

-------------------------------------------------
-- ExploredAll Button Handler
-------------------------------------------------

--Disabled for now, are they ever used? dbaker
function ExploredAll()
    FOW_SetAll(2); -- corresponds to gameplay FOGOFWARMODE_NOVIS
end
--Controls.FOW_ExploredAll_Button:RegisterCallback( Mouse.eLClick, ExploredAll );
-------------------------------------------------
-- HideAll Button Handler
-------------------------------------------------
function HideAll()
    FOW_SetAll(1); -- corresponds to gameplay FOGOFWARMODE_UNEXPLORED
end
--Controls.FOW_HideAll_Button:RegisterCallback( Mouse.eLClick, HideAll );


-------------------------------------------------
-- Generic mode button handler
-------------------------------------------------
function ModeClicked( void1, Control )


    --Enable the correct extended menus, if applicable
    Controls.DebugAudio_Panel:SetHide(		not ( Control == 14 ) );
    Controls.Terrain_Panel:SetHide(	not ( Control == 15 ) );
	Controls.Graphics_Panel:SetHide(        not ( Control == 17 ) );
	Controls.HexDebugText_Panel:SetHide(     not ( Control == 21 ) );
	Controls.ColorKey_Panel:SetHide( not (Control == 23 ) );
    --Controls.GlobalUnitData_Panel:SetHide(not ( Control == 18 ) );
    Controls.DebugMode_Panel:SetHide( not ( Control == 22 ) );
    Controls.FluidFOW_Panel:SetHide( not ( Control == 24 ) );
    Controls.DebugAdvisor_Panel:SetHide( not ( Control == 25 ) );
    
    if Control == 13 then
		ReloadTextures(0,0);
	end

    
    if Control == 16 then
		-- Entering PEffectMode.
		OnEnterPEffectMode();
	elseif g_iControl == 16 then
		-- Exiting PEffectMode.
		OnExitPEffectMode();
    end
    
    --Store the last state change
    g_iControl = Control;
end
g_iControl = -1;

Controls.ReloadTextures_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.DebugAudio_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.DebugTerrain_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.GraphicsPanel_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.HexDebugTextPanel_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.DebugModePanel_Button:RegisterCallback(Mouse.eLClick, ModeClicked );
Controls.ColorKeyPanel_Button:RegisterCallback(Mouse.eLClick, ModeClicked );
Controls.FluidFOWPanel_Button:RegisterCallback(Mouse.eLClick, ModeClicked );
Controls.DebugAdvisors_Button:RegisterCallback(Mouse.eLClick, ModeClicked );
--Controls.GlobalUnitData_Button:RegisterCallback( Mouse.eLClick, ModeClicked );

local iTutorialIndex = 1;
function DisplayNextTutorial()
	--LuaEvent.ForceAdvisorEvent( iTutorialIndex);
	print("Throw event");
	iTutorialIndex = iTutorialIndex + 1;
end
Controls.NextAdvisorPrompt_Button:RegisterCallback( Mouse.eLClick, DisplayNextTutorial );

function DisplayPrevTutorial()
	--LuaEvent.ForceAdvisorEvent( iTutorialIndex);
	print("Throw event");
	iTutorialIndex = iTutorialIndex - 1;
end
Controls.PrevAdvisorPrompt_Button:RegisterCallback( Mouse.eLClick, DisplayPrevTutorial );


-------------------------------------------------
-- Audio Debug Panel Buttons
-------------------------------------------------

function ChangeAudio( Opt )
	if( Opt == 0 ) then
		-- Play Next Song
		Events.AudioDebugChangeMusic(true,false,false);
	end
	if ( Opt == 1 ) then
		-- Toggle War
		Events.AudioDebugChangeMusic(true,true,false);
	end
	if ( Opt == 2 ) then
		-- Reset Audio System
		Events.AudioDebugChangeMusic(false,false,true);
	end
end
Controls.AudioNextSong_Button:RegisterCallback( Mouse.eLClick, ChangeAudio );
Controls.AudioToggleWar_Button:RegisterCallback( Mouse.eLClick, ChangeAudio );
Controls.AudioReset_Button:RegisterCallback( Mouse.eLClick, ChangeAudio );

-------------------------------------------------
-- Toggle City Banners
-------------------------------------------------
function ToggleCityBanners()
    bCityBanners = not bCityBanners;
	Controls.ToggleCityBanners_Checkbox:SetCheck( bCityBanners );
	ContextPtr:LookUpControl( "/InGame/WorldView/CityBannerManager" ):SetHide( not bCityBanners );
end
Controls.ToggleCityBanners_Button:RegisterCallback( Mouse.eLClick, ToggleCityBanners );
bCityBanners = true;

-------------------------------------------------
-- Toggle Unit Flags
-------------------------------------------------
function ToggleUnitFlags()
    bUnitFlags = not bUnitFlags;
	Controls.ToggleUnitFlags_Checkbox:SetCheck( bUnitFlags );
	ContextPtr:LookUpControl( "/InGame/WorldView/UnitFlagManager" ):SetHide( not bUnitFlags );
end
Controls.ToggleUnitFlags_Button:RegisterCallback( Mouse.eLClick, ToggleUnitFlags );
bUnitFlags = true;

-----------------------------------------------------
-- Begin: Particle Effect Debug Mode
-----------------------------------------------------
-- Table representing the Particle Effect Debug Mode
g_PEffectMode =
{
	m_bIsActive = false,
	m_bIsStatsVisible = false,
	m_fUpdateDelay = 1.0, -- This is a fixed value and shoul not be changing at runtime.
	m_fTimer = -1.0,
}

function OnEnterPEffectMode()
	g_PEffectMode.m_bIsActive = true;
    Controls.ParticleEffects_Panel:SetHide(false);
    Controls.ParticleEffectsShowStats_Button:SetText("Show Statistics");
end

function OnExitPEffectMode()
	g_PEffectMode.m_bIsActive = false;
	g_PEffectMode.m_bIsStatsVisible = false;
	g_PEffectMode.m_fTimer = -1.0;
    Controls.ParticleEffects_Panel:SetHide(true);
    Controls.ParticleEffectsStats_Panel:SetHide(true);
end

function OnUpdatePEffectMode(fTimeDelta)
	if g_PEffectMode.m_bIsStatsVisible then
		-- Update timer.
		g_PEffectMode.m_fTimer = g_PEffectMode.m_fTimer - fTimeDelta;
		if g_PEffectMode.m_fTimer < 0.0 then
			-- Reset timer.
			g_PEffectMode.m_fTimer = g_PEffectMode.m_fUpdateDelay;
			
			-- Request statistics data.
			Events.ParticleEffectStatsRequested();
		end
	end
end

function OnShowPEffectStats(iVoid1, iVoid2)
	local bWasHidden = Controls.ParticleEffectsStats_Panel:IsHidden();
	
	-- Hide/Show the statistics panel.
	Controls.ParticleEffectsStats_Panel:SetHide(not bWasHidden);
	
	-- Update button text.
	if bWasHidden then
		Controls.ParticleEffectsShowStats_Button:SetText("Hide Statistics");
	else
		Controls.ParticleEffectsShowStats_Button:SetText("Show Statistics");
	end

	
	g_PEffectMode.m_bIsStatsVisible = bWasHidden;
	
	-- Reset timer so that the panel updates when you re-enter.
	g_PEffectMode.m_fTimer = -1.0;
end

OnPEffectStatsResponse = function(responseData)
	if g_PEffectMode.m_bIsStatsVisible then
		Controls.ParticleEffectsStats_Label0:SetText("Simulated Effects: "			.. tostring(responseData.NumSimulatedEffects));
		Controls.ParticleEffectsStats_Label1:SetText("Simulated Emitters: "		.. tostring(responseData.NumSimulatedEmitters));
		Controls.ParticleEffectsStats_Label2:SetText("Simulated Particles: "		.. tostring(responseData.NumSimulatedParticles));
		Controls.ParticleEffectsStats_Label3:SetText("Rendered Models: "			.. tostring(responseData.NumRenderedModels));
		Controls.ParticleEffectsStats_Label4:SetText("Rendered Particles: "		.. tostring(responseData.NumRenderedParticles));
		Controls.ParticleEffectsStats_Label5:SetText("Rendered Particle Polys:  "	.. tostring(responseData.NumRenderedParticlePolys));
	end
end
	
function OnReloadAllPEffects(iVoid1, iVoid2)
	-- Request for a reload of all particle effects.
	Events.ParticleEffectReloadRequested();
end

-- Register Mode Selection Callback
Controls.ParticleEffects_Button:RegisterCallback( Mouse.eLClick, ModeClicked );

-- Register for Button Click Callback for buttons on the particle effect mode main panel.
Controls.ParticleEffectsShowStats_Button:RegisterCallback( Mouse.eLClick, OnShowPEffectStats );
Controls.ParticleEffectsReloadAll_Button:RegisterCallback( Mouse.eLClick, OnReloadAllPEffects );

-- Register for Stats Response Event
Events.ParticleEffectStatsResponse.Add( OnPEffectStatsResponse );
-----------------------------------------------------
-- End: Particle Effect Debug Mode
-----------------------------------------------------

-------------------------------------------------
-------------------------------------------------
-- Handles updates for this lua context.
function UpdateHandler( fTimeDelta )
	if g_PEffectMode.m_bIsActive then
		OnUpdatePEffectMode(fTimeDelta)
	end
	
	function HumanReadableSize(bytes)
		if(bytes > 1073741824) then
			return string.format("%.2f gb", bytes / 1073741824);
		elseif(bytes > 1048576) then
			return string.format("%.2f mb", bytes / 1048576);
		elseif(bytes > 1024) then
			return string.format("%.2f kb", bytes / 1024);
		else
			return string.format("%d b", bytes);
		end
	end
	
	
	local count = collectgarbage("count");
	local gcmemory = string.format("Lua - %s", HumanReadableSize(count * 1024));	
	Controls.GCMemory:SetText(gcmemory);
	
	
	local dbmemory = DB.GetMemoryUsage();
	
	local currentUserAllocated = HumanReadableSize(dbmemory.CurrentUserAllocatedBytes);
	local maxUserAllocated = HumanReadableSize(dbmemory.MaxUserAllocatedBytes);
			
	Controls.DBMemoryAllocated:SetText(string.format("SQL Cur - %s", currentUserAllocated));
	Controls.DBMemoryMaxAllocated:SetText(string.format("SQL Max - %s", maxUserAllocated));
	
end
ContextPtr:SetUpdate( UpdateHandler );

-------------------------------------------------
-------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
     ----------------------------------------------------------------        
    -- Key Up Processing
    ----------------------------------------------------------------        
    if( uiMsg == KeyEvents.KeyUp )
    then
        if( wParam == Keys.S or
            wParam == Keys.VK_LEFT )
        then
    		Events.SerialEventCameraStopMovingLeft();
            return true;
        elseif( wParam == Keys.F or
                wParam == Keys.VK_RIGHT )
        then
    		Events.SerialEventCameraStopMovingRight();
            return true;
        elseif( wParam == Keys.E or
                wParam == Keys.VK_UP )
        then
    		Events.SerialEventCameraStopMovingForward();
            return true;
        elseif( wParam == Keys.D or
                wParam == Keys.VK_DOWN )
        then
    		Events.SerialEventCameraStopMovingBack();
            return true;
        elseif( wParam == Keys.VK_NUMPAD4 )
        then
			Events.CameraStopRotatingCCW();
			return true;
        elseif( wParam == Keys.VK_NUMPAD6 )
        then
			Events.CameraStopRotatingCW();
			return true;
		elseif( wParam == Keys.VK_NUMPAD8 )
        then
			Events.CameraStopPitchingUp();
			return true;
        elseif( wParam == Keys.VK_NUMPAD2 )
        then
			Events.CameraStopPitchingDown();
			return true;			
        end

    ----------------------------------------------------------------        
    -- Key Down Processing
    ----------------------------------------------------------------        
    elseif( uiMsg == KeyEvents.KeyDown )
    then
        if( wParam == Keys.VK_LEFT )
        then
    		Events.SerialEventCameraStartMovingLeft();
            return true;
        elseif( wParam == Keys.VK_RIGHT )
        then
    		Events.SerialEventCameraStartMovingRight();
            return true;
        elseif( wParam == Keys.VK_UP )
        then
    		Events.SerialEventCameraStartMovingForward();
            return true;
        elseif( wParam == Keys.VK_DOWN )
        then
    		Events.SerialEventCameraStartMovingBack();
            return true;
        elseif( wParam == Keys.VK_NUMPAD4 )
        then
			Events.CameraStartRotatingCCW();
			return true;
        elseif( wParam == Keys.VK_NUMPAD6 )
        then
			Events.CameraStartRotatingCW();
			return true;			
		elseif( wParam == Keys.VK_NUMPAD8 )
        then
			Events.CameraStartPitchingUp();
			return true;
        elseif( wParam == Keys.VK_NUMPAD2 )
        then
			Events.CameraStartPitchingDown();
			return true;			
        elseif( wParam == Keys.VK_NEXT or
                wParam == Keys.VK_OEM_PLUS )
        then
    		Events.SerialEventCameraOut( Vector2(0.5,0.5) );
            return true;
        elseif( wParam == Keys.VK_PRIOR or
                wParam == Keys.VK_OEM_MINUS )
        then
    		Events.SerialEventCameraIn( Vector2(0.5,0.5) );
            return true;
        end
        
        if( g_iControl == 3 ) then
			if( wParam == Keys.VK_OEM_PLUS ) then
				ChangeBuildingSize( 0.01 );
			end
			if( wparam == Keys.VS_OEM_MINUS ) then
				ChangeBuildingSize( -0.01 );
			end
        end
    end 
    UI.DebugKeyHandler( uiMsg, wParam, lParam );
end
ContextPtr:SetInputHandler( InputHandler );

-- Uncomment this section to disable the debug panel in MP games
function ShowHideHandler( isHide )
	if(not isHide and PreGame.IsMultiplayerGame()) then
		ContextPtr:SetHide( true );
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );


-------------------------------------------------
-- Script Body
-------------------------------------------------
