-------------------------------------------------
-- City Events Info Log Popup
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
local g_CityEventsInstanceManager = InstanceManager:new( "CityEventsButton", "Button", Controls.CityEventsButtonStack );
local m_pCity = nil;
-------------------------------------------------
-- On Popup
-------------------------------------------------
function UpdateCityEvents()
			
	g_CityEventsInstanceManager:ResetInstances();
	local count = 0;
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
						count = count+1;
						local CityStr = Locale.ConvertTextKey(pCity:GetNameKey())
						local DescStr = ""
						
						if(pEventInfo.Description ~= nil) then
							DescStr = Locale.Lookup(pEventInfo.Description)
						else
							DescStr = Locale.Lookup(info.Description)
						end
						local HelpStr = Locale.ConvertTextKey(pCity:GetScaledEventChoiceValue(info.ID))
						local duration = pCity:GetCityEventChoiceCooldown(info.ID)
						AddCityEventsButton(DescStr, HelpStr, duration, CityStr, pCity);
					end
				end
			end
		end
	end
	if(count > 0) then
		Controls.NoCityEvents:SetHide(true);
	else
		Controls.NoCityEvents:SetHide(false);
	end
	
	Controls.CityEventsButtonStack:CalculateSize();
	Controls.CityEventsButtonStack:ReprocessAnchoring();
	Controls.CityEventsScrollPanel:CalculateInternalSize();
end
Events.SerialEventGameMessagePopup.Add( OnPopup );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function AddCityEventsButton( DescStr, HelpStr, duration, CityStr, pCity )	

	local controlTable = g_CityEventsInstanceManager:GetInstance();
	controlTable.CityEventsHelpText:SetText(HelpStr);	
	if(duration > 0) then
		controlTable.CityEventsText:SetText("[COLOR_CYAN]" .. DescStr .. "[ENDCOLOR]" .. " (" .. Locale.ConvertTextKey("TXT_KEY_TP_TURNS_REMAINING", duration) .. ")");
	else
		controlTable.CityEventsText:SetText("[COLOR_CYAN]" .. DescStr .. "[ENDCOLOR]");
	end
	controlTable.CityName:SetText("[COLOR_POSITIVE_TEXT]" .. CityStr .. "[ENDCOLOR]");
    
    controlTable.TextStack:CalculateSize();
    controlTable.TextStack:ReprocessAnchoring();
    
    local sizeY = controlTable.TextStack:GetSizeY()
    controlTable.Button:SetSizeY(sizeY);
    controlTable.TextAnim:SetSizeY(sizeY);
    controlTable.TextHL:SetSizeY(sizeY);

	controlTable.Button:SetVoids( pCity:GetX(), pCity:GetY() );

	controlTable.Button:RegisterCallback( Mouse.eLClick, OnEventInfoClicked );
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
		UpdateCityEvents();
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );