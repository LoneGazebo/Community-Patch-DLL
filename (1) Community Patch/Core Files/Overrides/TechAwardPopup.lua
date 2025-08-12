-------------------------------------------------
-- Technology Award Popup
-------------------------------------------------
include("TechButtonInclude");
include("TechHelpInclude");

local NUM_SMALL_BUTTONS = 5;

local g_popupInfo = {};

local L = Locale.Lookup;

-------------------------------------------------------------------------------
-- On display
Events.SerialEventGameMessagePopup.Add(function (popupInfo)
	if popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_TECH_AWARD then
		return;
	end

	if popupInfo.Data3 == -1 then
		return;
	end

	g_popupInfo = popupInfo;

	local kTechInfo = GameInfo.Technologies[popupInfo.Data3];
	assert(kTechInfo, "Invalid tech ID from popupInfo");

	Controls.TechName:SetText(L(kTechInfo.Description));
	Controls.TechQuote:SetText(L(kTechInfo.Quote));
	Controls.TechHelp:SetText(string.format("[NEWLINE]%s[NEWLINE]", GetShortHelpTextForTech(kTechInfo.ID)));

	-- Update the tech icon
	if IconHookup(kTechInfo.PortraitIndex, 128, kTechInfo.IconAtlas, Controls.TechIcon) then
		Controls.TechIcon:SetHide(false);
	else
		Controls.TechIcon:SetHide(true);
	end

	-- Update the small buttons
	AddSmallButtonsToTechButton(Controls, kTechInfo, NUM_SMALL_BUTTONS, 64);
	Controls.BonusStack:CalculateSize();
	Controls.BonusStack:ReprocessAnchoring();

	Controls.OuterGrid:DoAutoSize();

	UIManager:QueuePopup(ContextPtr, PopupPriority.TechAward);
end);

-------------------------------------------------------------------------------
-- Close this popup
local function Close()
	UIManager:DequeuePopup(ContextPtr);
end

-------------------------------------------------------------------------------
-- The Continue button is pressed
Controls.ContinueButton:RegisterCallback(Mouse.eLClick, Close);

-------------------------------------------------------------------------------
-- Keyboard events
--- @param uiMsg integer
--- @param wParam any
--- @return true?
ContextPtr:SetInputHandler(function (uiMsg, wParam)
	if uiMsg == KeyEvents.KeyDown then
		if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
			Close();
			return true;
		end
	end
end);

-------------------------------------------------------------------------------
--- @param bIsHide boolean
--- @param bInitState boolean
ContextPtr:SetShowHideHandler(function (bIsHide, bInitState)
	if not bInitState then
		if not bIsHide then
			UI.incTurnTimerSemaphore();
			Events.SerialEventGameMessagePopupShown(g_popupInfo);
		else
			UI.decTurnTimerSemaphore();
			Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_TECH_AWARD, 0);
		end
	end
end);

----------------------------------------------------------------
-- 'Active' (local human) player has changed
-- Make sure it's closed so the next player doesn't see the previous player's popup.
Events.GameplaySetActivePlayer.Add(Close);
