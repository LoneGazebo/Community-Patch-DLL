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
	local pEventInfo = nil
	for row in GameInfo.Event_ParentEvents("EventChoiceType = '" .. pEventChoiceInfo.Type .. "'") do
		pEventInfo = GameInfo.Events[row.EventType]
		break
	end
	if pEventInfo then
		local pEventArt = pEventInfo.EventArt or "playereventdefaultbackground.dds"
		Controls.EventArt:SetTexture(pEventArt);
		Controls.EventArt:SetSizeVal(350,100);
		
		-- Event Audio
		local pEventAudio = pEventInfo.EventAudio
		if pEventAudio then
			Events.AudioPlay2DSound(pEventAudio)
		end
	end

	local playerID = Game.GetActivePlayer()
	local player = Players[Game.GetActivePlayer()];
		
	local szTitleString;
	local szHelpString;
	szTitleString = Locale.Lookup(pEventChoiceInfo.Description);
	szHelpString = Locale.ConvertTextKey(player:GetScaledEventChoiceValue(pEventChoiceInfo.ID));
	
	-- Test for any Override Strings

	tChoiceOverrideStrings = {}
	LuaEvents.EventChoice_OverrideTextStrings(playerID, nil, pEventChoiceInfo, tChoiceOverrideStrings)
	for _,str in ipairs(tChoiceOverrideStrings) do
		print(str.Help)
		szTitleString = str.Description or szTitleString
		szHelpString = str.Help or szHelpString
	end

	Controls.TitleLabel:SetText(szTitleString);
	Controls.TitleLabel:SetToolTipString(szTitleString);
	Controls.DescriptionLabel:SetText(szHelpString);
	
	-- Recalculate grid size
	local mainGridSizeY = 400
	local sizeYDiff = math.max((Controls.DescriptionLabel:GetSizeY()-Controls.EventBox:GetSizeY()),1)
	Controls.MainGrid:SetSizeY(mainGridSizeY + sizeYDiff)
		
	UIManager:QueuePopup( ContextPtr, PopupPriority.CityStateGreeting );
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