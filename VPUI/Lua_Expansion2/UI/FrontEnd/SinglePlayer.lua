if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Main Menu
-------------------------------------------------

-------------------------------------------------
-- Script Body
-------------------------------------------------
local bHideUITest = true;
local bHideGridExamples = true;
local bHideLoadGame = true;
local bHidePreGame = true;
local fTime = 0;


function ShowHideHandler( bIsHide, bIsInit )
    if( not bIsHide ) then
		
		-- Reset some of the advanced settings for PreGame.
		-- This is to ensure that play now doesn't get borked or that Mutiplayer settings do not carry over.
		PreGame.SetPrivateGame(false);
		PreGame.SetGameType(GameTypes.GAME_SINGLE_PLAYER);
		PreGame.ResetSlots();
		PreGame.ResetGameOptions();
		PreGame.ResetMapOptions();
		PreGame.LoadPreGameSettings();
		    
        Controls.Civ5Logo:SetTexture( "CivilzationV_Logo.dds" );
        
        Controls.MainMenuScreenUI:SetHide( false );
        local str = Locale.ConvertTextKey( "TXT_KEY_PLAY_NOW_SETTINGS" ) .. "[NEWLINE]";
      
        local civIndex = PreGame.GetCivilization( 0 );
        if( civIndex ~= -1 ) then
            civ = GameInfo.Civilizations[ civIndex ];
            local leaderDescription = GameInfo.Leaders( "Type = '" .. GameInfo.Civilization_Leaders( "CivilizationType = '" .. civ.Type .. "'" )().LeaderheadType .. "'" )().Description;
            
            str = str .. Locale.ConvertTextKey( "TXT_KEY_RANDOM_LEADER_CIV", Locale.ConvertTextKey( leaderDescription ), Locale.ConvertTextKey( civ.ShortDescription )) .. "[NEWLINE]";
        else
            str = str .. Locale.ConvertTextKey( "TXT_KEY_RANDOM_LEADER" ) .. "[NEWLINE]";
        end
        
        str = str .. Locale.ConvertTextKey( "TXT_KEY_MAP_SCRIPT" ) .. ": ";
        if( not PreGame.IsRandomMapScript() ) then 
        	local savedMapScript = PreGame.GetMapScript();
        	local foundScript = false;
		
			for mapScript in GameInfo.MapScripts() do
				if(mapScript.FileName == savedMapScript) then
					str = str .. Locale.ConvertTextKey( mapScript.Name or mapScript.Description ) .. "[NEWLINE]";
					foundScript = true;
				end
			end
            
			if(not foundScript) then
				local mapData = UI.GetMapPreview(savedMapScript);
				if(mapData ~= nil) then
					str = str .. Locale.ConvertTextKey(mapData.Name) .. "[NEWLINE]";
				else
					PreGame.SetRandomMapScript(true);
				end
			end
        end
           
        local info = nil;
        if(PreGame.IsRandomMapScript()) then
            str = str .. Locale.ConvertTextKey( "TXT_KEY_RANDOM_MAP_SCRIPT" ) .. "[NEWLINE]";
        end

        if( not PreGame.IsRandomWorldSize() ) then
            info = GameInfo.Worlds[ PreGame.GetWorldSize() ];
            if(info ~= nil) then
				str = str .. Locale.ConvertTextKey( "TXT_KEY_MAP_SIZE_FORMAT", Locale.ConvertTextKey( info.Description )) .. "[NEWLINE]";
			end
        else
            str = str .. Locale.ConvertTextKey( "TXT_KEY_MAP_SIZE_FORMAT", Locale.ConvertTextKey( "TXT_KEY_RANDOM_MAP_SIZE" )) .. "[NEWLINE]";
        end
      
        info = GameInfo.HandicapInfos[ PreGame.GetHandicap( 0 ) ];
        if(info ~= nil) then
	        str = str .. Locale.ConvertTextKey( "TXT_KEY_AD_HANDICAP_SETTING", Locale.ConvertTextKey( info.Description )) .. "[NEWLINE]";
        end
        
        if(info ~= nil) then
			info = GameInfo.GameSpeeds[ PreGame.GetGameSpeed() ];
		end
        str = str .. Locale.ConvertTextKey( "TXT_KEY_AD_GAME_SPEED_SETTING", Locale.ConvertTextKey( info.Description ));
        
        Controls.StartGameButton:SetToolTipString( str );
    else
        Controls.Civ5Logo:UnloadTexture();
    end
    
    local bHideScenariosButton = true;
    for row in Modding.GetInstalledFiraxisScenarios() do
		bHideScenariosButton = false;
		break;
	end
	
	
    Controls.ScenariosButton:SetHide(bHideScenariosButton);
    
end
ContextPtr:SetShowHideHandler( ShowHideHandler );


-------------------------------------------------
-- StartGame Button Handler
-------------------------------------------------
function StartGameClick()
	Events.SerialEventStartGame();
	UIManager:SetUICursor( 1 );
end
Controls.StartGameButton:RegisterCallback( Mouse.eLClick, StartGameClick );


-------------------------------------------------
-- WorldPicker Button Handler
-------------------------------------------------
function SetupGameClicked()
    UIManager:QueuePopup( Controls.GameSetupScreen, PopupPriority.GameSetupScreen );
end
Controls.GameSetupButton:RegisterCallback( Mouse.eLClick, SetupGameClicked );

-------------------------------------------------
-- Scenarios Button Handler
-------------------------------------------------
function ScenariosClicked()
	UIManager:QueuePopup( Controls.ScenariosScreen, PopupPriority.GameSetupScreen );
end
Controls.ScenariosButton:RegisterCallback( Mouse.eLClick, ScenariosClicked );


-------------------------------------------------
-- LoadGame Button Handler
-------------------------------------------------
function LoadGameClick()
    UIManager:QueuePopup( Controls.LoadGameScreen, PopupPriority.LoadGameScreen );
end
Controls.LoadGameButton:RegisterCallback( Mouse.eLClick, LoadGameClick );

-------------------------------------------------
-- LoadMap Button Handler
-------------------------------------------------
function LoadTutorialClick()
    UIManager:QueuePopup( Controls.LoadTutorial, PopupPriority.LoadTutorial );
end
Controls.LoadTutorialButton:RegisterCallback( Mouse.eLClick, LoadTutorialClick );

-------------------------------------------------
-- Back Button Handler
-------------------------------------------------
function BackButtonClick()

    UIManager:DequeuePopup( ContextPtr );

end
Controls.BackButton:RegisterCallback( Mouse.eLClick, BackButtonClick );


----------------------------------------------------------------
-- Input processing
----------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE then
            BackButtonClick();
            return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );
