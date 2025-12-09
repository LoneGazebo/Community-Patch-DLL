if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Choose Faith Great Person Popup
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
	Controls.TitleLabel:LocalizeAndSetText("TXT_KEY_CHOOSE_FAITH_GREAT_PERSON");
	Controls.DescriptionLabel:LocalizeAndSetText("TXT_KEY_CHOOSE_FAITH_GREAT_PERSON_TT");
	
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
			
			if (info.ID == GameInfo.Units["UNIT_PROPHET"].ID and (not player:HasCreatedPantheon() or (not player:HasCreatedReligion() and Game.GetNumReligionsStillToFound() == 0))) then	
				controlTable.Button:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_NO_PROPHET_FROM_FAITH"));
				controlTable.Button:SetDisabled(true);
			elseif (info.ID == GameInfo.Units["UNIT_MERCHANT"].ID and not player:IsPolicyBranchFinished(GameInfo.PolicyBranchTypes["POLICY_BRANCH_COMMERCE"].ID)) then	
				controlTable.Button:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_NO_MERCHANT_FROM_FAITH"));
				controlTable.Button:SetDisabled(true);
			elseif (info.ID == GameInfo.Units["UNIT_SCIENTIST"].ID and not player:IsPolicyBranchFinished(GameInfo.PolicyBranchTypes["POLICY_BRANCH_RATIONALISM"].ID)) then	
				controlTable.Button:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_NO_SCIENTIST_FROM_FAITH"));
				controlTable.Button:SetDisabled(true);
			elseif (info.ID == GameInfo.Units["UNIT_WRITER"].ID and not player:IsPolicyBranchFinished(GameInfo.PolicyBranchTypes["POLICY_BRANCH_AESTHETICS"].ID)) then	
				controlTable.Button:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_NO_ARTIST_FROM_FAITH"));
				controlTable.Button:SetDisabled(true);
			elseif (info.ID == GameInfo.Units["UNIT_ARTIST"].ID and not player:IsPolicyBranchFinished(GameInfo.PolicyBranchTypes["POLICY_BRANCH_AESTHETICS"].ID)) then	
				controlTable.Button:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_NO_ARTIST_FROM_FAITH"));
				controlTable.Button:SetDisabled(true);
			elseif (info.ID == GameInfo.Units["UNIT_MUSICIAN"].ID and not player:IsPolicyBranchFinished(GameInfo.PolicyBranchTypes["POLICY_BRANCH_AESTHETICS"].ID)) then	
				controlTable.Button:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_NO_ARTIST_FROM_FAITH"));
				controlTable.Button:SetDisabled(true);
			elseif (info.ID == GameInfo.Units["UNIT_GREAT_GENERAL"].ID and not player:IsPolicyBranchFinished(GameInfo.PolicyBranchTypes["POLICY_BRANCH_HONOR"].ID)) then	
				controlTable.Button:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_NO_GENERAL_FROM_FAITH"));
				controlTable.Button:SetDisabled(true);
			elseif (info.ID == GameInfo.Units["UNIT_GREAT_ADMIRAL"].ID and not player:IsPolicyBranchFinished(GameInfo.PolicyBranchTypes["POLICY_BRANCH_EXPLORATION"].ID)) then	
				controlTable.Button:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_NO_ADMIRAL_FROM_FAITH"));
				controlTable.Button:SetDisabled(true);
			elseif (info.ID == GameInfo.Units["UNIT_ENGINEER"].ID and not player:IsPolicyBranchFinished(GameInfo.PolicyBranchTypes["POLICY_BRANCH_TRADITION"].ID)) then	
				controlTable.Button:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_NO_ENGINEER_FROM_FAITH"));
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
				if(activePlayer:GetNumFaithGreatPeople() > 0) then
					Network.SendFaithGreatPersonChoice(playerID, unit.ID);
				end
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
    if( popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_CHOOSE_FAITH_GREAT_PERSON and
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
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_CHOOSE_FAITH_GREAT_PERSON, 0);
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
