-------------------------------------------------
-- Notification Log Popup
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
local g_RecentEventsInstanceManager = InstanceManager:new( "RecentEventsButton", "Button", Controls.RecentEventsButtonStack );

-------------------------------------------------
-- On Popup
-------------------------------------------------
function UpdateRecentEvents()
	
	g_RecentEventsInstanceManager:ResetInstances();
	local count = 0;
	local CityStr = "";
	local player = Players[Game.GetActivePlayer()];
	if player then
		for info in GameInfo.EventChoices() do
			if(player:IsEventChoiceActive(info.ID, true)) then
				local pEventInfo = nil
				for row in GameInfo.Event_ParentEvents("EventChoiceType = '" .. info.Type .. "'") do
					pEventInfo = GameInfo.Events[row.EventType]
					break
				end
				if pEventInfo then
					print("Found an event choice")
					count = count+1;
					local pCity = nil;
					local DescStr = "";				
					if(pEventInfo.Description ~= nil) then
						DescStr = Locale.Lookup(pEventInfo.Description)
					else
						DescStr = Locale.Lookup(info.Description)
					end
					local HelpStr = Locale.ConvertTextKey(player:GetScaledEventChoiceValue(info.ID))
					AddRecentEventsButton(DescStr, HelpStr, CityStr, pCity);
				end
			end
		end
		for pCity in player:Cities() do
			if(pCity ~= nil) then
				for info in GameInfo.CityEventChoices() do
					if pCity:IsCityEventChoiceActive(info.ID, true) then
						local pEventInfo = nil
						for row in GameInfo.CityEvent_ParentEvents("CityEventChoiceType = '" .. info.Type .. "'") do
							pEventInfo = GameInfo.CityEvents[row.CityEventType]
							break
						end
						if pEventInfo then
							print("Found an event choice")
							count = count+1;
							CityStr = Locale.ConvertTextKey(pCity:GetNameKey())
							local DescStr = "";			
							if(pEventInfo.Description ~= nil) then
								DescStr = Locale.Lookup(pEventInfo.Description)
							else
								DescStr = Locale.Lookup(info.Description)
							end
							local HelpStr = Locale.ConvertTextKey(pCity:GetScaledEventChoiceValue(info.ID))
							AddRecentEventsButton(DescStr, HelpStr, CityStr, pCity);
						end
					end
				end
			end
		end
	end
	if(count > 0) then
		Controls.NoRecentEvents:SetHide(true);
	else
		Controls.NoRecentEvents:SetHide(false);
	end
	
	Controls.RecentEventsButtonStack:CalculateSize();
	Controls.RecentEventsButtonStack:ReprocessAnchoring();
	Controls.RecentEventsScrollPanel:CalculateInternalSize();
end
Events.SerialEventGameMessagePopup.Add( OnPopup );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function AddRecentEventsButton( DescStr, HelpStr, CityStr, pCity)	

	local controlTable = g_RecentEventsInstanceManager:GetInstance();
	controlTable.RecentEventsHelpText:SetText(HelpStr);	
	controlTable.RecentEventsText:SetText("[COLOR_CYAN]" .. DescStr .. "[ENDCOLOR]");

	if(CityStr ~= "")then
		controlTable.CityName:SetText("[COLOR_POSITIVE_TEXT]" .. CityStr .. "[ENDCOLOR]");
	else
		controlTable.CityName:SetHide(true);
	end
	if(pCity ~= nil) then
		controlTable.Button:SetVoids( pCity:GetX(), pCity:GetY() );
		controlTable.Button:RegisterCallback( Mouse.eLClick, OnEventInfoClicked );
	end

    controlTable.TextStack:CalculateSize();
    controlTable.TextStack:ReprocessAnchoring();
    
    local sizeY = controlTable.TextStack:GetSizeY()
    controlTable.Button:SetSizeY(sizeY);
    controlTable.TextAnim:SetSizeY(sizeY);
    controlTable.TextHL:SetSizeY(sizeY);
end

function OnEventInfoClicked(x, y)
	local plot = Map.GetPlot( x, y );
	if( plot ~= nil ) then
		UI.LookAt(plot, 0);
		local hex = ToHexFromGrid(Vector2(plot:GetX(), plot:GetY()));
		Events.GameplayFX(hex.x, hex.y, -1);
	end
end
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )
    if( not bIsHide ) then
		UpdateRecentEvents();
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );
