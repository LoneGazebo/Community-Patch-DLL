-------------------------------------------------
-- Goody Hut Popup
-------------------------------------------------

local m_PopupInfo = nil;

-------------------------------------------------
-- On Display
-------------------------------------------------
local tChoiceOverrideStrings = {}
function OnPopup( popupInfo )

	if( popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_MODDER_9 ) then
		return;
	end

	m_PopupInfo = popupInfo;

    local iEventChoiceType = popupInfo.Data1;
    local pEventChoiceInfo = GameInfo.EventChoices[iEventChoiceType];

	-- Top Art
	local pEventInfo = GameInfo.Events[pEventChoiceInfo.ParentEvent];
	local pEventArt = pEventInfo.EventArt
	if pEventArt then
		Controls.EventArt:SetTexture(pEventArt);
		Controls.EventArt:SetSizeVal(350,100);
		Controls.EventArt:SetAlpha(0.2);
		Controls.EventArt:SetHide(false);
		Controls.EventArtFrame:SetHide(false);
	else
		Controls.EventArt:SetHide(true);
		Controls.EventArtFrame:SetHide(true);
	end
	
	-- Event Audio
	local pEventAudio = pEventInfo.EventAudio
	if pEventAudio then
		Events.AudioPlay2DSound(pEventAudio)
	end
	
	local szTitleString;
	local szHelpString;
	szTitleString = Locale.Lookup(pEventChoiceInfo.Description);
	szHelpString = Locale.Lookup(pEventChoiceInfo.Help);
	
	-- Test for any Override Strings
	local playerID = Game.GetActivePlayer()
	local player = Players[Game.GetActivePlayer()];
	tEventOverrideStrings = {}
	LuaEvents.EventChoice_OverrideTextStrings(playerID, nil, pEventChoiceInfo, tChoiceOverrideStrings)
	for _,str in ipairs(tChoiceOverrideStrings) do
		szTitleString = str.Description or szTitleString
		szHelpString = str.Help or szHelpString
	end

	Controls.TitleLabel:SetText(szTitleString);
	Controls.DescriptionLabel:SetText(szHelpString);
		
	UIManager:QueuePopup( ContextPtr, PopupPriority.GoodyHut );
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
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_MODDER_9, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnActivePlayerChanged( iActivePlayer, iPrevActivePlayer )
	if (not ContextPtr:IsHidden()) then
		ContextPtr:SetHide(true);
	end
end
Events.GameplaySetActivePlayer.Add(OnActivePlayerChanged);