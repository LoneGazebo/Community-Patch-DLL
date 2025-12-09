if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Select City Names
-------------------------------------------------

local m_PopupInfo = nil;

-------------------------------------------------
-------------------------------------------------
function OnCancel()
    UIManager:DequeuePopup( ContextPtr );
end
Controls.CancelButton:RegisterCallback( Mouse.eLClick, OnCancel );


-------------------------------------------------
-------------------------------------------------
function OnAccept()
	local pCity = UI.GetHeadSelectedCity();
	if pCity then
		Network.SendRenameCity(pCity:GetID(), Controls.EditCityName:GetText());
	end
	
    UIManager:DequeuePopup( ContextPtr );
end
Controls.AcceptButton:RegisterCallback( Mouse.eLClick, OnAccept );

----------------------------------------------------------------
-- Input processing
----------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE then
            OnCancel();  
        end
    end
    return true;
end
ContextPtr:SetInputHandler( InputHandler );


----------------------------------------------------------------
----------------------------------------------------------------
function ValidateText(text)

	local numNonWhiteSpace = 0;
	for i = 1, #text, 1 do
		if string.byte(text, i) ~= 32 then
			numNonWhiteSpace = numNonWhiteSpace + 1;
		end
	end
	
	if numNonWhiteSpace < 3 then
		return false;
	end
	
	-- don't allow % character
	for i = 1, #text, 1 do
		if string.byte(text, i) == 37 then
			return false;
		end
	end
	
	local invalidCharArray = { '\"', '<', '>', '|', '\b', '\0', '\t', '\n', '/', '\\', '*', '?', '%[', ']' };

	for i, ch in ipairs(invalidCharArray) do
		if string.find(text, ch) ~= nil then
			return false;
		end
	end
	
	-- don't allow control characters
	for i = 1, #text, 1 do
		if string.byte(text, i) < 32 then
			return false;
		end
	end
	
	return true;
end

function Validate(dummyString, dummyControl, enterPressedOrLostFocus)
	local bValid = false;

	if ValidateText(dummyString) then
		bValid = true;
	end
	
	Controls.AcceptButton:SetDisabled(not bValid);
end
Controls.EditCityName:RegisterCallback(Validate);

----------------------------------------------------------------
----------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )
    if( not bInitState ) then
        if( not bIsHide ) then
	     	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
        else
            UI.decTurnTimerSemaphore();
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_RENAME_CITY, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );


function OnPopup( popupInfo )
	if ( popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_RENAME_CITY ) then
		return;
	end
	
	m_PopupInfo = popupInfo;
	
	local pCity = UI.GetHeadSelectedCity();
		if pCity then
		local cityName = pCity:GetNameKey();
		local convertedKey = Locale.ConvertTextKey(cityName);
		
		Controls.EditCityName:SetText(convertedKey);
		Controls.AcceptButton:SetDisabled(true);
		
		UIManager:QueuePopup( ContextPtr, PopupPriority.Priority_GreatPersonReward );
	end
end
Events.SerialEventGameMessagePopup.Add( OnPopup );


----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(OnCancel);
