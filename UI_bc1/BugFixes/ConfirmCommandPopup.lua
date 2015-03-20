
-- CONFIRM COMMAND POPUP
-- This popup occurs when an action needs confirmation.
PopupLayouts[ButtonPopupTypes.BUTTONPOPUP_CONFIRMCOMMAND] = function(popupInfo)

	local iAction	= popupInfo.Data1;
	local bAlt		= popupInfo.Option1;

	local action = GameInfoActions[iAction];
	popupText = Locale.ConvertTextKey("TXT_KEY_POPUP_ARE_YOU_SURE_ACTION", action.TextKey);
	local unit = UI.GetHeadSelectedUnit()
	if unit then
		popupText = popupText .. " ("..unit:GetName()..")"
	end
	
	SetPopupText(popupText);
		
	-- Initialize 'yes' button.
	local OnYesClicked = function()
		
		-- Confirm action
		local gameMessageDoCommand = GameMessageTypes.GAMEMESSAGE_DO_COMMAND;
		Game.SelectionListGameNetMessage(gameMessageDoCommand, action.CommandType, action.CommandData, -1, 0, bAlt);
	end
	
	local buttonText = Locale.ConvertTextKey("TXT_KEY_POPUP_YES");
	AddButton(buttonText, OnYesClicked)
		
	-- Initialize 'no' button.
	local buttonText = Locale.ConvertTextKey("TXT_KEY_POPUP_NO");
	AddButton(buttonText, nil);

	Controls.CloseButton:SetHide( true );

end

----------------------------------------------------------------        
-- Key Down Processing
----------------------------------------------------------------        
PopupInputHandlers[ButtonPopupTypes.BUTTONPOPUP_CONFIRMCOMMAND] = function( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if( wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN ) then
			HideWindow();
            return true;
        end
    end
end

