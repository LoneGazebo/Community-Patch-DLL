if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end


-------------------------------------------------
-------------------------------------------------
function OnFrontEndPopup( string )
    UIManager:PushModal( ContextPtr );
    
    Controls.PopupText:SetText( Locale.ConvertTextKey( string ) );
end
Events.FrontEndPopup.Add( OnFrontEndPopup )


-------------------------------------------------
-------------------------------------------------
function OnBack()
    UIManager:PopModal( ContextPtr );
end
--Controls.BackButton:RegisterCallback( Mouse.eLClick, OnBack );
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnBack );


----------------------------------------------------------------
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