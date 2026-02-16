if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Mods Error Popup
-------------------------------------------------
include("IconSupport");
include("InstanceManager");

local g_BackButton = {};

-------------------------------------------------
-------------------------------------------------
function OnOK()
    UIManager:PopModal( ContextPtr );
    ContextPtr:CallParentShowHideHandler( true );
    ContextPtr:SetHide( true );
end
Controls.OKButton:RegisterCallback( Mouse.eLClick, OnOK );

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

----------------------------------------------------------------        
-- ShowHideHandler
----------------------------------------------------------------
function ShowHideHandler( bIsHide )

end
ContextPtr:SetShowHideHandler( ShowHideHandler );
