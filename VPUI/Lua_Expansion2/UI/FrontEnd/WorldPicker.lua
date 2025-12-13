if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Placeholder World Picker dialog
-------------------------------------------------

-------------------------------------------------
-- Duel Button Handler
-------------------------------------------------
function DuelWorldSizeButtonClick()
    ContextPtr:SetHide( true );
	Events.SerialEventStartGame( WorldSizeTypes.WORLDSIZE_DUEL );
	UIManager:SetUICursor( 1 );
end
Controls.DuelWorldSizeButton:RegisterCallback( eLClick, DuelWorldSizeButtonClick );

-------------------------------------------------
-- Tiny Button Handler
-------------------------------------------------
function TinyWorldSizeButtonClick()
    ContextPtr:SetHide( true );
	Events.SerialEventStartGame( WorldSizeTypes.WORLDSIZE_TINY );
	UIManager:SetUICursor( 1 );
end
Controls.TinyWorldSizeButton:RegisterCallback( eLClick, TinyWorldSizeButtonClick );

-------------------------------------------------
-- Small Button Handler
-------------------------------------------------
function SmallWorldSizeButtonClick()
    ContextPtr:SetHide( true );
	Events.SerialEventStartGame( WorldSizeTypes.WORLDSIZE_SMALL );
	UIManager:SetUICursor( 1 );
end
Controls.SmallWorldSizeButton:RegisterCallback( eLClick, SmallWorldSizeButtonClick );

-------------------------------------------------
-- Standard Button Handler
-------------------------------------------------
function StandardWorldSizeButtonClick()
    ContextPtr:SetHide( true );
	Events.SerialEventStartGame( WorldSizeTypes.WORLDSIZE_STANDARD );
	UIManager:SetUICursor( 1 );
end
Controls.StandardWorldSizeButton:RegisterCallback( eLClick, StandardWorldSizeButtonClick );

-------------------------------------------------
-- Large Button Handler
-------------------------------------------------
function LargeWorldSizeButtonClick()
    ContextPtr:SetHide( true );
	Events.SerialEventStartGame( WorldSizeTypes.WORLDSIZE_LARGE );
	UIManager:SetUICursor( 1 );
end
Controls.LargeWorldSizeButton:RegisterCallback( eLClick, LargeWorldSizeButtonClick );

-------------------------------------------------
-- HUge Button Handler
-------------------------------------------------
function HugeWorldSizeButtonClick()
    ContextPtr:SetHide( true );
	Events.SerialEventStartGame( WorldSizeTypes.WORLDSIZE_HUGE );
	UIManager:SetUICursor( 1 );
end
Controls.HugeWorldSizeButton:RegisterCallback( eLClick, HugeWorldSizeButtonClick );

-- remove screen 
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE then
            ContextPtr:SetHide( true );
            return true;
        end
    end
    return false;
end
ContextPtr:SetInputHandler( InputHandler );

