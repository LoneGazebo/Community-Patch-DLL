if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Great Person Popup
-------------------------------------------------

include( "IconSupport" );
local m_PopupInfo = nil;

-------------------------------------------------
-------------------------------------------------
function OnPopup( popupInfo )

	if( popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_GREAT_PERSON_REWARD ) then
		return;
	end

	m_PopupInfo = popupInfo;

    local iUnitType = popupInfo.Data1;
    local pGreatPersonInfo = GameInfo.Units[iUnitType];
    
    local iPlayer = Game.GetActivePlayer();
    local iCityID = popupInfo.Data2;
    local pCity = Players[iPlayer]:GetCityByID(iCityID);
	
	Controls.DescriptionLabel:SetText(Locale.ConvertTextKey("TXT_KEY_GREAT_PERSON_REWARD", pGreatPersonInfo.Description, pCity:GetNameKey()));
	
	local portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon(iUnitType, iPlayer);
	if IconHookup( portraitOffset, 256, portraitAtlas, Controls.Portrait ) then
		Controls.Portrait:SetHide( false );
	else
		Controls.Portrait:SetHide( true );
	end

	UIManager:QueuePopup( ContextPtr, PopupPriority.GreatPersonReward );
end
Events.SerialEventGameMessagePopup.Add( OnPopup );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnCloseButtonClicked ()
	Controls.Portrait:UnloadTexture();
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
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_GREAT_PERSON_REWARD, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(OnCloseButtonClicked);
