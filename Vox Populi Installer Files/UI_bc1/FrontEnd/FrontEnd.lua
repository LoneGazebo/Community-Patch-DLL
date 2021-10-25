-- modified by Temudjin from 1.0.3.142 code
-------------------------------------------------
-- FrontEnd
-------------------------------------------------

function ShowHideHandler( bIsHide, bIsInit )

		-- Check for game invites first.  If we have a game invite, we will have flipped 
		-- the Civ5App::eHasShownLegal and not show the legal/touch screens.
		UI:CheckForCommandLineInvitation();

---------- Temudjin START
--    if not UI:HasShownLegal() then
--        UIManager:QueuePopup( Controls.LegalScreen, PopupPriority.LegalScreen );
--    end
---------- Temudjin END

    if not bIsHide then
        Controls.AtlasLogo:SetTexture( "CivilzationVAtlas.dds" );
    	UIManager:SetUICursor( 0 );
        UIManager:QueuePopup( Controls.MainMenu, PopupPriority.MainMenu );
    else
        Controls.AtlasLogo:UnloadTexture();
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );
