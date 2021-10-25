
-- CONFIRM COMMAND POPUP
-- This popup occurs when an action needs confirmation.
PopupLayouts[ButtonPopupTypes.BUTTONPOPUP_CONFIRMCOMMAND] = function(popupInfo)

	local bAlt		= popupInfo.Option1;

	local action = GameInfoActions[popupInfo.Data1] or {};
	local popupText = Locale.ConvertTextKey("TXT_KEY_POPUP_ARE_YOU_SURE_ACTION", action.TextKey or "" );
	local unit = UI.GetHeadSelectedUnit()
	if unit and popupText then
		popupText = popupText .. " ("..unit:GetName()..")"
	end

	SetPopupText(popupText);

	-- Initialize 'yes' button.
	local buttonText = Locale.ConvertTextKey("TXT_KEY_POPUP_YES");
	AddButton(buttonText, function()
		-- Confirm action
		Game.SelectionListGameNetMessage( GameMessageTypes.GAMEMESSAGE_DO_COMMAND, action.CommandType, action.CommandData, -1, 0, bAlt );
	end )

	-- Initialize 'no' button.
	AddButton( Locale.ConvertTextKey("TXT_KEY_POPUP_NO") )

	Controls.CloseButton:SetHide( true );

end

----------------------------------------------------------------
-- Key Down Processing
----------------------------------------------------------------
PopupInputHandlers[ButtonPopupTypes.BUTTONPOPUP_CONFIRMCOMMAND] = function( uiMsg, wParam )--, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
			HideWindow();
            return true;
        end
    end
end

