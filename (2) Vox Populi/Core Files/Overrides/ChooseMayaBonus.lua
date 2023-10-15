print("This is the modded ChooseMayaBonuws from 'CBP'")
-------------------------------------------------
-- Choose Maya Bonus Popup
-------------------------------------------------
include("IconSupport");
include("InfoTooltipInclude");

-- This global table handles how items are populated based on an identifier.
g_NumberOfFreeItems = 0;
local m_PopupInfo = nil;

PopulateItems = {};
CommitItems = {};

SelectedItems = {};


PopulateItems["GreatPeople"] = function(stackControl, playerID)
	
	local count = 0;
	Controls.TitleLabel:LocalizeAndSetText("TXT_KEY_CHOOSE_LONG_COUNT");
	Controls.DescriptionLabel:LocalizeAndSetText("TXT_KEY_CHOOSE_LONG_COUNT_TT");
	
	local player = Players[playerID];
	SelectedItems = {};
	stackControl:DestroyAllChildren();
	----------------------------------------------------------------        
	-- build the buttons
	----------------------------------------------------------------        
	for info in GameInfo.Units{Special = "SPECIALUNIT_PEOPLE"} do
	
		if(player:CanTrain(info.ID, true, true, true, false)) then
			local controlTable = {};
			ContextPtr:BuildInstanceForControl( "ItemInstance", controlTable, stackControl );
			local unitType = info.Type;
			
			local portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon(info.ID, playerID);
			IconHookup( portraitOffset, 64, portraitAtlas, controlTable.Icon64 );
			controlTable.Help:LocalizeAndSetText(info.Strategy);
			controlTable.Name:LocalizeAndSetText(info.Description);

			local iEarlierBaktun = player:GetUnitBaktun(info.ID);
-- CBP
			if(info.ID == GameInfo.Units["UNIT_PROPHET"].ID and not player:IsProphetValid()) then
				controlTable.Button:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_INVALID_NO_RELIGION", iEarlierBaktun));
				controlTable.Button:SetDisabled(true);
-- CBP
-- END (edited elseif below as well)
			elseif (iEarlierBaktun > 0 and not player:IsFreeMayaGreatPersonChoice()) then
				controlTable.Button:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_SELECTED_EARLIER_BAKTUN", iEarlierBaktun));
				controlTable.Button:SetDisabled(true);
			else
				controlTable.Button:SetToolTipString(Locale.ConvertTextKey( info.Strategy ) );
			
				local selectionAnim = controlTable.SelectionAnim;

				controlTable.Button:RegisterCallback(Mouse.eLClick, function()  
				
					local foundIndex = nil;
					for i,v in ipairs(SelectedItems) do
						if(v[1] == unitType) then
							foundIndex = i;
							break;
						end
					end
				
					if(foundIndex ~= nil) then
						if(g_NumberOfFreeItems > 1) then
							selectionAnim:SetHide(true);
							table.remove(SelectedItems, foundIndex);
						end
					else
						if(g_NumberOfFreeItems > 1) then
							if(#SelectedItems < g_NumberOfFreeItems) then
								selectionAnim:SetHide(false);
								table.insert(SelectedItems, {unitType, controlTable});
							end
						else
							if(#SelectedItems > 0) then
								for i,v in ipairs(SelectedItems) do
									v[2].SelectionAnim:SetHide(true);	
								end
								SelectedItems = {};
							end
						
							selectionAnim:SetHide(false);
							table.insert(SelectedItems, {unitType, controlTable});
						end	
					end
				
					Controls.ConfirmButton:SetDisabled(g_NumberOfFreeItems ~= #SelectedItems);
				
				end);
			end
			
			count = count + 1;
		end
	end
	
	return count;
end

CommitItems["GreatPeople"] = function(selection, playerID)
	
	local activePlayer = Players[playerID];
	if(activePlayer ~= nil) then
		for i,v in ipairs(selection) do
			local unitType = v[1];
			print("Adding unit " .. unitType);
			local unit = GameInfo.Units[unitType];
			if(unit ~= nil) then
				if(activePlayer:GetNumMayaBoosts() > 0) then
					Network.SendMayaBonusChoice(playerID, unit.ID);
				end
				-- activePlayer:AddFreeUnit(unit.ID);
				-- activePlayer:ChangeNumMayaBoosts(-1);
			end
		end
	end
end
 
function DisplayPopup(playerID, classType, numberOfFreeItems)
	
	if(numberOfFreeItems ~= 1) then
		error("This UI currently only supports 1 free item for the time being.");
	end
	
	local count = PopulateItems[classType](Controls.ItemStack, playerID);
	
	Controls.ItemStack:CalculateSize();
	Controls.ItemStack:ReprocessAnchoring();
	Controls.ItemScrollPanel:CalculateInternalSize();
	
	if(count > 0 and numberOfFreeItems > 0) then
		g_NumberOfFreeItems = math.min(numberOfFreeItems, count);
	
		Controls.ConfirmButton:SetDisabled(true);
		ContextPtr:SetHide(false); 
	else
		ContextPtr:SetHide(true);
	end 
	
	Controls.ConfirmButton:RegisterCallback(Mouse.eLClick, function()
		CommitItems[classType](SelectedItems, playerID);
		 ContextPtr:SetHide(true);
	end);
end


function OnPopup( popupInfo )	
    if( popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_CHOOSE_MAYA_BONUS and
        ContextPtr:IsHidden() == true ) then
        
        m_PopupInfo = popupInfo;
        
        print("Displaying Choose Free Item Popup");
        DisplayPopup(popupInfo.Data1, "GreatPeople", 1);
    end
end
Events.SerialEventGameMessagePopup.Add( OnPopup );
  
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )

    if( not bInitState ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
        else
            UI.decTurnTimerSemaphore();
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_CHOOSE_MAYA_BONUS, 0);
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
