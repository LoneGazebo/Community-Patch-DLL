print("This is the City Event Popup Manager from CBP")
-------------------------------------------------
-- Choose Event Popup
-------------------------------------------------
include("FLuaVector.lua")
include("IconSupport");
include("InfoTooltipInclude");

-- This global table handles how items are populated based on an identifier.
g_NumberOfSelectedItems = 0;
local m_PopupInfo = nil;
PopulateItems = {};
CommitItems = {};

SelectedItems = {};

local g_pCity = nil
local tChoiceOverrideStrings = {}
local tEventOverrideStrings = {}
PopulateItems["CityEventChoices"] = function(stackControl, playerID, plotID, spyID)
	
	local count = 0;
	
	local plot = Map.GetPlotByIndex(plotID);
	local city = plot and plot:GetPlotCity()
	local player = Players[playerID];
	g_pCity = city

	if (g_pCity) then
		local plot = g_pCity:Plot();
		if plot then
			UI.LookAt(plot, 0);
			local hex = ToHexFromGrid(Vector2(plot:GetX(), plot:GetY()))
			Events.GameplayFX(hex.x, hex.y, -1) 
		end
	end

	SelectedItems = {};
	stackControl:DestroyAllChildren();
	----------------------------------------------------------------        
	-- build the buttons
	----------------------------------------------------------------
	--print("Getting choice list for city event")

	local iEventType = m_PopupInfo.Data2;
	if(iEventType ~= -1) then
		local pEventInfo = GameInfo.CityEvents[iEventType]

		if(pEventInfo) then
			--print("Found our city event " .. iEventType)
		
			-- Event Audio
			local pEventAudio = pEventInfo.CityEventAudio
			if pEventAudio then
				Events.AudioPlay2DSound(pEventAudio)
			end
		
			-- Top Text
			local cityName = city:GetNameKey();
			local localizedCityName = Locale.ConvertTextKey(cityName);

			local localizedDesc = Locale.ConvertTextKey(pEventInfo.Description);
			
			local szTypeString;
			local szHelpString;
			szTypeString = Locale.Lookup("TXT_KEY_CITY_EVENT_TITLE", localizedCityName, localizedDesc);

			if(player) then
				szHelpString = Locale.ConvertTextKey(player:GetEspionageValues(iEventType, spyID, playerID));
			end

			-- Test for any Override Strings
			tEventOverrideStrings = {}
			LuaEvents.Event_OverrideTextStrings(playerID, cityID, pEventInfo, tEventOverrideStrings)
			for _,str in ipairs(tEventOverrideStrings) do
				szTypeString = str.Description or szTypeString
				szHelpString = str.Help or szHelpString
			end
		
			Controls.TitleLabel:SetText(szTypeString);
			Controls.TitleLabel:SetToolTipString(szTypeString);
			Controls.DescriptionLabel:SetText(szHelpString);
		
			local buttonSizeY = 53
			for row in GameInfo.CityEvent_ParentEvents("CityEventType = '" .. pEventInfo.Type .. "'") do
			--print("Cycling through city event choices")
				local info = GameInfo.CityEventChoices[row.CityEventChoiceType]
				--print("Found an espionage event choice")

				local controlTable = {};
				ContextPtr:BuildInstanceForControl( "ItemInstance", controlTable, stackControl );
				local eventChoiceType = info.Type;

				local szDescString;
				local szHelpString;
				szDescString = Locale.Lookup(info.Description);
				szHelpString = Locale.ConvertTextKey(city:GetScaledEventChoiceValue(info.ID, false, spyID, playerID), localizedCityName);
		
				-- Test for any Override Strings
				tChoiceOverrideStrings = {}
				LuaEvents.EventChoice_OverrideTextStrings(playerID, cityID, info, tChoiceOverrideStrings)
				for _,str in ipairs(tChoiceOverrideStrings) do
					szDescString = str.Description or szDescString
					szHelpString = str.Help or szHelpString
				end
				controlTable.Name:SetText("[ICON_BULLET]" .. szDescString);
				controlTable.Button:SetToolTipString(szHelpString);
				
				-- Readjust the offset
				local sizeYDiff = math.max((controlTable.Name:GetSizeY()-buttonSizeY),1)
				if sizeYDiff > 1 then sizeYDiff = sizeYDiff + 20 end
				controlTable.Box:SetSizeY(buttonSizeY + sizeYDiff)
				controlTable.Button:SetSizeY(buttonSizeY + sizeYDiff)
				controlTable.MOSelectionAnim:SetSizeY(buttonSizeY + sizeYDiff)
				controlTable.MOSelectionAnimHL:SetSizeY(buttonSizeY + sizeYDiff)
				controlTable.SelectionAnim:SetSizeY(buttonSizeY + sizeYDiff)
				controlTable.SelectionAnimHL:SetSizeY(buttonSizeY + sizeYDiff)
				
				-- Disable invalid choices
				if(not city:IsCityEventChoiceValidEspionage(info.ID, iEventType, spyID, playerID)) then
					local szDisabledString = city:GetDisabledTooltip(info.ID, spyID, playerID );
					if(szDisabledString == "") then
						szDisabledString = "TXT_KEY_CANNOT_TAKE_TT";
					end
					szHelpString = szDisabledString
					controlTable.Button:SetDisabled(true)
					controlTable.Name:SetAlpha(0.2)
					controlTable.Button:SetToolTipString(szHelpString);
				else
					controlTable.Name:SetAlpha(1)
					controlTable.Button:SetDisabled(false)
				end
				
				local selectionAnim = controlTable.SelectionAnim;

				controlTable.Button:RegisterCallback(Mouse.eLClick, function()  
				
					local foundIndex = nil;
					for i,v in ipairs(SelectedItems) do
						if(v[1] == eventChoiceType) then
							foundIndex = i;
							break;
						end
					end
				
					if(foundIndex ~= nil) then
						if(g_NumberOfSelectedItems > 1) then
							selectionAnim:SetHide(true);
							table.remove(SelectedItems, foundIndex);
						end
					else
						if(g_NumberOfSelectedItems > 1) then
							if(#SelectedItems < g_NumberOfSelectedItems) then
								selectionAnim:SetHide(false);
								table.insert(SelectedItems, {eventChoiceType, controlTable});
							end
						else
							if(#SelectedItems > 0) then
								for i,v in ipairs(SelectedItems) do
									v[2].SelectionAnim:SetHide(true);	
								end
								SelectedItems = {};
							end
						
							selectionAnim:SetHide(false);
							table.insert(SelectedItems, {eventChoiceType, controlTable});
						end	
					end
				
					Controls.ConfirmButton:SetDisabled(g_NumberOfSelectedItems ~= #SelectedItems);
				
				end);

				--print("city event choice added")
			
				count = count + 1;
			end
		end
	end
	return count;
end

CommitItems["CityEventChoices"] = function(selection, playerID, plotID, spyID)
	
	local plot = Map.GetPlotByIndex(plotID);
	local city = plot and plot:GetPlotCity()

	if(city ~= nil) then
		for i,v in ipairs(selection) do
			local eventChoiceType = v[1];
			--print("Making Choice " .. eventChoiceType);
			local eventChoice = GameInfo.CityEventChoices[eventChoiceType];
			if(eventChoice ~= nil) then
				city:DoCityEventChoice(eventChoice.ID, spyID, playerID);
			end
		end
	end
end

function DisplayPopup(playerID, plotID, spyID, classType, numberOfChoices)
	
	if(numberOfChoices ~= 1) then
		error("This UI currently only supports 1 choice for the time being.");
	end
	
	local count = PopulateItems[classType](Controls.ItemStack, playerID, plotID, spyID);
	
	Controls.ItemStack:CalculateSize();
	Controls.ItemStack:ReprocessAnchoring();
	Controls.ItemScrollPanel:CalculateInternalSize();

	if (g_pCity) then
		local plot = g_pCity:Plot();
		if plot then
			Controls.GoToCity:RegisterCallback(Mouse.eLClick, function()
				UI.DoSelectCityAtPlot( plot );
				ContextPtr:SetHide(true);
			end);
		end
	end
	
	if(count > 0 and numberOfChoices > 0) then
		g_NumberOfSelectedItems = math.min(numberOfChoices, count);
	
		Controls.ConfirmButton:SetDisabled(true);
		ContextPtr:SetHide(false); 
	else
		ContextPtr:SetHide(true);
	end 
	
	Controls.ConfirmButton:RegisterCallback(Mouse.eLClick, function()
		CommitItems[classType](SelectedItems, playerID, plotID, spyID);
		 ContextPtr:SetHide(true);
	end);
end

function OnPopup( popupInfo )	
    if( popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_MODDER_12) then
		if(ContextPtr:IsHidden() == true ) then
			m_PopupInfo = popupInfo;
			print("Displaying City Event Selection Popup");
			DisplayPopup(m_PopupInfo.Data1, m_PopupInfo.Data3, m_PopupInfo.Data4, "CityEventChoices", 1);
		end
    end
end
Events.SerialEventGameMessagePopup.Add( OnPopup );


----------------------------------------------------------------        
-- Input processing
----------------------------------------------------------------        
function OnHideButtonClicked ()
    ContextPtr:SetHide(true);
	if (g_pCity) then
		local plot = g_pCity:Plot();
		if plot then
			UI.LookAt(plot, 0);
			local hex = ToHexFromGrid(Vector2(plot:GetX(), plot:GetY()))
			Events.GameplayFX(hex.x, hex.y, -1) 
		end
	end
end
Controls.HideButton:RegisterCallback( Mouse.eLClick, OnHideButtonClicked );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
            OnHideButtonClicked();
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
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_MODDER_12, 0);
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
