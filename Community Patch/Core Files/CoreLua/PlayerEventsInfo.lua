-------------------------------------------------
-- Notification Log Popup
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
local g_PlayerEventsInstanceManager = InstanceManager:new( "PlayerEventsButton", "Button", Controls.PlayerEventsButtonStack );

-------------------------------------------------
-- On Popup
-------------------------------------------------
function UpdatePlayerEvents()
	
	g_PlayerEventsInstanceManager:ResetInstances();
	local count = 0;
	local player = Players[Game.GetActivePlayer()];
	for info in GameInfo.EventChoices() do
		if(player:IsEventChoiceActive(info.ID)) then
			local pEventInfo = nil
			for row in GameInfo.Event_ParentEvents("EventChoiceType = '" .. info.Type .. "'") do
				pEventInfo = GameInfo.Events[row.EventType]
				break
			end
			if pEventInfo then
				print("Found an event choice")
				count = count+1;
				local DescStr = ""				
				if(pEventInfo.Description ~= nil) then
					DescStr = Locale.Lookup(pEventInfo.Description)
				else
					DescStr = Locale.Lookup(info.Description)
				end
				local HelpStr = Locale.ConvertTextKey(player:GetScaledEventChoiceValue(info.ID))
				local duration = player:GetEventChoiceCooldown(info.ID)
				AddPlayerEventsButton(DescStr, HelpStr, duration);
			end
		end
	end
	if(count > 0) then
		Controls.NoPlayerEvents:SetHide(true);
	else
		Controls.NoPlayerEvents:SetHide(false);
	end
	
	Controls.PlayerEventsButtonStack:CalculateSize();
	Controls.PlayerEventsButtonStack:ReprocessAnchoring();
	Controls.PlayerEventsScrollPanel:CalculateInternalSize();
end
Events.SerialEventGameMessagePopup.Add( OnPopup );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function AddPlayerEventsButton( DescStr, HelpStr, duration )	

	local controlTable = g_PlayerEventsInstanceManager:GetInstance();
	controlTable.PlayerEventsHelpText:SetText(HelpStr);	
	if(duration > 0) then
		controlTable.PlayerEventsText:SetText("[COLOR_CYAN]" .. DescStr .. "[ENDCOLOR]" .. " (" .. Locale.ConvertTextKey("TXT_KEY_TP_TURNS_REMAINING", duration) .. ")");
	else
		controlTable.PlayerEventsText:SetText("[COLOR_CYAN]" .. DescStr .. "[ENDCOLOR]");
	end
    
    controlTable.TextStack:CalculateSize();
    controlTable.TextStack:ReprocessAnchoring();
    
    local sizeY = controlTable.TextStack:GetSizeY()
    controlTable.Button:SetSizeY(sizeY);
    controlTable.TextAnim:SetSizeY(sizeY);
    controlTable.TextHL:SetSizeY(sizeY);
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )
    if( not bIsHide ) then
		UpdatePlayerEvents();
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );
