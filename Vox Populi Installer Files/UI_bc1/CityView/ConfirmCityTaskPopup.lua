-- CONFIRM CITY TASK POPUP
-- This popup occurs when an action needs confirmation.

PopupLayouts[ButtonPopupTypes.BUTTONPOPUP_CONFIRM_CITY_TASK] = function(popupInfo)
	local cityID	= popupInfo.Data1;
	local taskID	= popupInfo.Data2;
	local popupText	= popupInfo.Text or "";

	if taskID == TaskTypes.TASK_RAZE then
		popupText = Locale.ConvertTextKey("TXT_KEY_POPUP_ARE_YOU_SURE_RAZE");
	else
		popupText = popupText.."[NEWLINE]"..Locale.ConvertTextKey("TXT_KEY_POPUP_ARE_YOU_SURE");
	end

	SetPopupText(popupText);

	-- Initialize 'yes' button.
	local OnYesClicked = function()

		-- Confirm action
		Network.SendDoTask( cityID, taskID, -1, -1, false, false, false, false );
	end

	AddButton(Locale.ConvertTextKey("TXT_KEY_POPUP_YES"), OnYesClicked)
	AddButton(Locale.ConvertTextKey("TXT_KEY_POPUP_NO"), nil)

end
