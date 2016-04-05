-------------------------------------------------
-- City Events Info Log Popup
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
local g_CityEventsInstanceManager = InstanceManager:new( "CityEventsButton", "Button", Controls.CityEventsButtonStack );
-------------------------------------------------
-- On Popup
-------------------------------------------------
function UpdateCityEvents()
			
	g_CityEventsInstanceManager:ResetInstances();

	local player = Players[Game.GetActivePlayer()];
	for pCity in player:Cities() do
		if(pCity ~= nil) then
			for info in GameInfo.CityEventChoices() do
				if pCity:IsCityEventChoiceActive(info.ID) then
					local pEventInfo = nil
					for row in GameInfo.CityEvent_ParentEvents("CityEventChoiceType = '" .. info.Type .. "'") do
						pEventInfo = GameInfo.CityEvents[row.CityEventType]
						break
					end
					if pEventInfo then
						print("Found an event choice")
			
						local CityStr = Locale.ConvertTextKey(pCity:GetNameKey())
						local DescStr = ""
						
						if(pEventInfo.Description ~= nil) then
							DescStr = Locale.Lookup(pEventInfo.Description)
						else
							DescStr = Locale.Lookup(info.Description)
						end
						local HelpStr = Locale.ConvertTextKey(info.Help)
						local duration = pCity:GetCityEventChoiceCooldown(info.ID)
						AddCityEventsButton(DescStr, HelpStr, duration, CityStr, iCityX, iCityY);
					end
				end
			end
		end
	end
	
	Controls.CityEventsButtonStack:CalculateSize();
	Controls.CityEventsButtonStack:ReprocessAnchoring();
	Controls.CityEventsScrollPanel:CalculateInternalSize();
end
Events.SerialEventGameMessagePopup.Add( OnPopup );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function AddCityEventsButton( DescStr, HelpStr, duration, CityStr, iCityX, iCityY )	

	local controlTable = g_CityEventsInstanceManager:GetInstance();
	controlTable.CityEventsHelpText:SetText(HelpStr);	
	if(duration > 0) then
		controlTable.CityEventsText:SetText(DescStr .. " (" .. Locale.ConvertTextKey("TXT_KEY_TP_TURNS_REMAINING", duration) .. ")");
	else
		controlTable.CityEventsText:SetText(DescStr);
	end
	controlTable.CityName:SetText("[COLOR_POSITIVE_TEXT]" .. CityStr .. "[ENDCOLOR]");
    
    controlTable.TextStack:CalculateSize();
    controlTable.TextStack:ReprocessAnchoring();
    
    local sizeY = controlTable.TextStack:GetSizeY()
    controlTable.Button:SetSizeY(sizeY);
    controlTable.TextAnim:SetSizeY(sizeY);
    controlTable.TextHL:SetSizeY(sizeY);

	controlTable.Button:RegisterCallback( Mouse.eLClick, OnEventInfoClicked );
end

function OnEventInfoClicked(iCityX, iCityY)
	local pPlot = Map.GetPlot(iCityX, iCityY);
	if (pPlot) then
		UI.LookAt(pPlot, 0);
		local hex = ToHexFromGrid(Vector2(pPlot:GetX(), pPlot:GetY()));
		Events.GameplayFX(hex.x, hex.y, -1);
	end
end
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )
    if( not bIsHide ) then
		UpdateCityEvents();
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );