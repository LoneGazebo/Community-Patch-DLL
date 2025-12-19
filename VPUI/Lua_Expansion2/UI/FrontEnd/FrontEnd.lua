if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- FrontEnd
-------------------------------------------------

function ShowHideHandler( bIsHide, bIsInit )

		-- Check for game invites first.  If we have a game invite, we will have flipped 
		-- the Civ5App::eHasShownLegal and not show the legal/touch screens.
		UI:CheckForCommandLineInvitation();
		
    if( not UI:HasShownLegal() ) then
        UIManager:QueuePopup( Controls.LegalScreen, PopupPriority.LegalScreen );
    end

    if( not bIsHide ) then
        Controls.AtlasLogo:SetTexture( "CivilzationVAtlas.dds" );
    	UIManager:SetUICursor( 0 );
        UIManager:QueuePopup( Controls.MainMenu, PopupPriority.MainMenu );
    else
        Controls.AtlasLogo:UnloadTexture();
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );
