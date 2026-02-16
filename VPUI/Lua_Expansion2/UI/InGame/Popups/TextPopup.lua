if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Goody Hut Popup
-------------------------------------------------

local m_PopupInfo = nil;

-------------------------------------------------
-- On Display
-------------------------------------------------
function OnPopup( popupInfo )

	if( popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_TEXT ) then
		return;
	end

    if( popupInfo.Data1 ~= nil ) then
        print( "one" );
        Controls.DescriptionLabel:SetWrapWidth( popupInfo.Data1 );
    else
        print( "two" );
        Controls.DescriptionLabel:SetWrapWidth( 400 );
    end
   
    -- show/hide the flare 
    if( popupInfo.Option1 ) then
		if(Controls.TopImage ~= nil) then
			Controls.TopImage:SetHide( false );
		end
		
        Controls.BottomImage:SetHide( false );
    else
		if(Controls.TopImage ~= nil) then
			Controls.TopImage:SetHide( true );
        end
        Controls.BottomImage:SetHide( true );
    end
    
	Controls.DescriptionLabel:LocalizeAndSetText( popupInfo.Text );
	Controls.Grid:DoAutoSize();
	
	UIManager:QueuePopup( ContextPtr, PopupPriority.TextPopup );
end
Events.SerialEventGameMessagePopup.Add( OnPopup );


----------------------------------------------------------------        
-- Input processing
----------------------------------------------------------------        
function OnCloseButtonClicked ()
    UIManager:DequeuePopup( ContextPtr );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnCloseButtonClicked );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
            OnCloseButtonClicked();
            return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )

    if( not bInitState ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
        else
            UI.decTurnTimerSemaphore();
            Events.SerialEventGameMessagePopupProcessed.CallImmediate( ButtonPopupTypes.BUTTONPOPUP_TEXT, 0 );
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(OnCloseButtonClicked);
