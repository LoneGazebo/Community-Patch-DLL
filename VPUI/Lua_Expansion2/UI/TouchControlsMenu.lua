if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Touch Controls Menu
-------------------------------------------------

function InputHandler( uiMsg, wParam, lParam )
    if( uiMsg == KeyEvents.WM_KEYDOWN )
    then
        if( wParam == Keys.VK_ESCAPE )
        then
            ContextPtr:SetHide( true );
            return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );


function OnClose()
    ContextPtr:SetHide( true );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose );


function OnCheck( bIsChecked )
    OptionsManager.SetHideTouchHelp_Cached( not bIsChecked );
	OptionsManager.CommitGameOptions();
end
Controls.DontShowAgainCheckbox:RegisterCheckHandler( OnCheck );

Controls.DontShowAgainCheckbox:SetCheck( not OptionsManager.GetHideTouchHelp() );