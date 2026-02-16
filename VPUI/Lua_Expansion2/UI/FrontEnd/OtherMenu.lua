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
        Controls.Civ5Logo:SetTexture( "CivilzationV_Logo.dds" );
        
        Controls.MainMenuScreenUI:SetHide( false );
    else
        Controls.Civ5Logo:UnloadTexture();
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );


-------------------------------------------------
-- Latest News Button Handler
-------------------------------------------------
Controls.LatestNewsButton:RegisterCallback( Mouse.eLClick, function()

	Steam.ActivateGameOverlayToWebPage("http://store.steampowered.com/news/?appids=8930");
end);

-------------------------------------------------
-- Civilopedia Button Handler
-------------------------------------------------
Controls.CivilopediaButton:RegisterCallback( Mouse.eLClick, function()
	UIManager:QueuePopup(Controls.Civilopedia, PopupPriority.HallOfFame);
end);


-------------------------------------------------
-- HoF Button Handler
-------------------------------------------------
function HallOfFameClick()
	UIManager:QueuePopup( Controls.HallOfFame, PopupPriority.HallOfFame );
end
Controls.HallOfFameButton:RegisterCallback( Mouse.eLClick, HallOfFameClick );

-------------------------------------------------
-- View Replays Handler
-------------------------------------------------
function ViewReplaysButtonClick()
	UIManager:QueuePopup( Controls.LoadReplayMenu, PopupPriority.HallOfFame );
end
Controls.ViewReplaysButton:RegisterCallback( Mouse.eLClick, ViewReplaysButtonClick );

-------------------------------------------------
-- Credits Button Handler
-------------------------------------------------
function CreditsClicked()
    UIManager:QueuePopup( Controls.Credits, PopupPriority.Credits );
end
Controls.CreditsButton:RegisterCallback( Mouse.eLClick, CreditsClicked );

-------------------------------------------------
-- Leaderboard Button Handler
-------------------------------------------------
function LeaderboardClick()
	UIManager:QueuePopup( Controls.Leaderboard, PopupPriority.Leaderboard );
end
Controls.LeaderboardButton:RegisterCallback( Mouse.eLClick, LeaderboardClick );

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