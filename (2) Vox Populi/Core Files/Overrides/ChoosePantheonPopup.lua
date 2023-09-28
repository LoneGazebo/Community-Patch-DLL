
-------------------------------------------------
-- Choose Pantheon Popup
-------------------------------------------------

include( "IconSupport" );
include( "InstanceManager" );

-- Used for Piano Keys
local ltBlue = {19/255,32/255,46/255,120/255};
local dkBlue = {12/255,22/255,30/255,120/255};

local g_ItemManager = InstanceManager:new( "ItemInstance", "Button", Controls.ItemStack );
local bHidden = true;
local g_bPantheons = true;
local bDebugScore = false; -- Use this to set to true to see scores for ALL beliefs

local screenSizeX, screenSizeY = UIManager:GetScreenSizeVal()
local spWidth, spHeight = Controls.ItemScrollPanel:GetSizeVal();

-- Original UI designed at 1050px 
local heightOffset = screenSizeY - 1020;

spHeight = spHeight + heightOffset;
Controls.ItemScrollPanel:SetSizeVal(spWidth, spHeight); 
Controls.ItemScrollPanel:CalculateInternalSize();
Controls.ItemScrollPanel:ReprocessAnchoring();

local bpWidth, bpHeight = Controls.BottomPanel:GetSizeVal();
--bpHeight = bpHeight * heightRatio;
print(heightOffset);
print(bpHeight);
bpHeight = bpHeight + heightOffset 
print(bpHeight);

Controls.BottomPanel:SetSizeVal(bpWidth, bpHeight);
Controls.BottomPanel:ReprocessAnchoring();
-------------------------------------------------
-------------------------------------------------
function OnPopupMessage(popupInfo)
	
	local popupType = popupInfo.Type;
	if popupType ~= ButtonPopupTypes.BUTTONPOPUP_FOUND_PANTHEON then
		return;
	end
	
	g_PopupInfo = popupInfo;
	g_bPantheons = popupInfo.Data2;

   	UIManager:QueuePopup( ContextPtr, PopupPriority.SocialPolicy );
end
Events.SerialEventGameMessagePopup.Add( OnPopupMessage );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnClose()
    UIManager:DequeuePopup(ContextPtr);
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    ----------------------------------------------------------------        
    -- Key Down Processing
    ----------------------------------------------------------------        
    if uiMsg == KeyEvents.KeyDown then
        if (wParam == Keys.VK_RETURN or wParam == Keys.VK_ESCAPE) then
			if(Controls.ChooseConfirm:IsHidden())then
	            OnClose();
	        else
				Controls.ChooseConfirm:SetHide(true);
           	end
			return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function RefreshList()
	g_ItemManager:ResetInstances();
		
	local pPlayer = Players[Game.GetActivePlayer()];
	CivIconHookup( pPlayer:GetID(), 64, Controls.CivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true );
	
	local availableBeliefs = {};
	
	if (g_bPantheons > 0) then		
		Controls.PanelTitle:LocalizeAndSetText("TXT_KEY_CHOOSE_PANTHEON_TITLE");
		for i,v in ipairs(Game.GetAvailablePantheonBeliefs()) do
			local belief = GameInfo.Beliefs[v];
			if(belief ~= nil) then
				table.insert(availableBeliefs, {
					ID = belief.ID,
					Name = Locale.Lookup(belief.ShortDescription),
					Description = Locale.Lookup(belief.Description),
				});
			end
		end	
	else
		Controls.PanelTitle:LocalizeAndSetText("TXT_KEY_CHOOSE_REFORMATION_BELIEF_TITLE");
		for i,v in ipairs(Game.GetAvailableReformationBeliefs()) do
			local belief = GameInfo.Beliefs[v];
			if(belief ~= nil) then
				table.insert(availableBeliefs, {
					ID = belief.ID,
					Name = Locale.Lookup(belief.ShortDescription),
					Description = Locale.Lookup(belief.Description),
				});
			end
		end		
	end

	-- Sort beliefs by their description.
	table.sort(availableBeliefs, function(a,b) return Locale.Compare(a.Name, b.Name) < 0; end);
	local iHighestScore = 0; --1st Prize
	local iHigherScore = 0; --2nd Prize
	local iHighScore = 0; --3rd Prize
	for i, belief in ipairs(availableBeliefs) do --Kinda disgusting to do it twice... but the Player will only see this once anyway.
		local iScore = Game.ScoreBelief(Game.GetActivePlayer(), belief.ID);
		if (iScore >= iHighestScore) then
			iHigherScore = iHighestScore;
			iHighestScore = iScore;
		elseif (iScore >= iHigherScore) then
			iHighScore = iHigherScore;
			iHigherScore = iScore;
		elseif (iScore >= iHighScore) then
			iHighScore = iScore;
		end
	end
	
	local bTickTock = false;
	for i, belief in ipairs(availableBeliefs) do
		local itemInstance = g_ItemManager:GetInstance();
		itemInstance.Name:SetText(belief.Name);
		itemInstance.Button:SetToolTipString(nil);
		if (not OptionsManager.IsNoBasicHelp()) then
			local iScore = Game.ScoreBelief(Game.GetActivePlayer(), belief.ID);
			if (iScore >= iHighestScore) then
				if (bDebugScore) then
					itemInstance.Name:SetText(belief.Name .. " (" .. Locale.Lookup("TXT_KEY_GR_SCORE") .. " [COLOR_CYAN]" .. iScore .. "[ENDCOLOR][ICON_TROPHY_GOLD])");
				else
					itemInstance.Name:SetText(belief.Name .. " (" .. "[ICON_TROPHY_GOLD])");
				end
				itemInstance.Button:SetToolTipString(Locale.Lookup("TXT_KEY_RELIGION_CHOOSER_ADVISOR_RECOMMENDATION"));
			elseif (iScore >= iHigherScore) then
				if (bDebugScore) then
					itemInstance.Name:SetText(belief.Name .. " (" .. Locale.Lookup("TXT_KEY_GR_SCORE") .. " [COLOR_CYAN]" .. iScore .. "[ENDCOLOR][ICON_TROPHY_SILVER])");
				else
					itemInstance.Name:SetText(belief.Name .. " (" .. "[ICON_TROPHY_SILVER])");
				end
				itemInstance.Button:SetToolTipString(Locale.Lookup("TXT_KEY_RELIGION_CHOOSER_ADVISOR_RECOMMENDATION"));
			elseif (iScore >= iHighScore) then
				if (bDebugScore) then
					itemInstance.Name:SetText(belief.Name .. " (" .. Locale.Lookup("TXT_KEY_GR_SCORE") .. " [COLOR_CYAN]" .. iScore .. "[ENDCOLOR][ICON_TROPHY_BRONZE])");
				else
					itemInstance.Name:SetText(belief.Name .. " (" .. "[ICON_TROPHY_BRONZE])");
				end
				itemInstance.Button:SetToolTipString(Locale.Lookup("TXT_KEY_RELIGION_CHOOSER_ADVISOR_RECOMMENDATION"));
			else
				if (bDebugScore) then
					itemInstance.Name:SetText(belief.Name .. " (" .. Locale.Lookup("TXT_KEY_GR_SCORE") .. " " .. iScore .. ")");
				end
			end
		end
		--itemInstance.Button:SetToolTipString(belief.Description);  
		itemInstance.Description:SetText(belief.Description);
		
		itemInstance.Button:RegisterCallback(Mouse.eLClick, function() SelectPantheon(belief.ID); end);
	
		if(bTickTock == false) then
			itemInstance.Box:SetColorVal(unpack(ltBlue));
		else
			itemInstance.Box:SetColorVal(unpack(dkBlue));
		end
		
		local buttonWidth, buttonHeight = itemInstance.Button:GetSizeVal();
		local descWidth, descHeight = itemInstance.Description:GetSizeVal();
		
		local newHeight = descHeight + 40;	
	
		
		itemInstance.Button:SetSizeVal(buttonWidth, newHeight);
		itemInstance.Box:SetSizeVal(buttonWidth + 20, newHeight);
		itemInstance.BounceAnim:SetSizeVal(buttonWidth + 20, newHeight + 5);
		itemInstance.BounceGrid:SetSizeVal(buttonWidth + 20, newHeight + 5);
		
				
		bTickTock = not bTickTock;
	end
	
	Controls.ItemStack:CalculateSize();
	Controls.ItemStack:ReprocessAnchoring();
	Controls.ItemScrollPanel:CalculateInternalSize();
end

function SelectPantheon(beliefID) 
	g_BeliefID = beliefID;
	local belief = GameInfo.Beliefs[beliefID];
	Controls.ConfirmText:LocalizeAndSetText("TXT_KEY_CONFIRM_PANTHEON", belief.ShortDescription);
	Controls.ChooseConfirm:SetHide(false);
end

function OnYes( )
	Controls.ChooseConfirm:SetHide(true);
	
	Network.SendFoundPantheon(Game.GetActivePlayer(), g_BeliefID);
	Events.AudioPlay2DSound("AS2D_INTERFACE_POLICY");	
	
	OnClose();	
end
Controls.Yes:RegisterCallback( Mouse.eLClick, OnYes );

function OnNo( )
	Controls.ChooseConfirm:SetHide(true);
end
Controls.No:RegisterCallback( Mouse.eLClick, OnNo );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )

	bHidden = bIsHide;
    if( not bInitState ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(g_PopupInfo);
        	
        	RefreshList();
        
			local unitPanel = ContextPtr:LookUpControl( "/InGame/WorldView/UnitPanel/Base" );
			if( unitPanel ~= nil ) then
				unitPanel:SetHide( true );
			end
			
			local infoCorner = ContextPtr:LookUpControl( "/InGame/WorldView/InfoCorner" );
			if( infoCorner ~= nil ) then
				infoCorner:SetHide( true );
			end
               	
        else
      
			local unitPanel = ContextPtr:LookUpControl( "/InGame/WorldView/UnitPanel/Base" );
			if( unitPanel ~= nil ) then
				unitPanel:SetHide(false);
			end
			
			local infoCorner = ContextPtr:LookUpControl( "/InGame/WorldView/InfoCorner" );
			if( infoCorner ~= nil ) then
				infoCorner:SetHide(false);
			end
			
			if(g_PopupInfo ~= nil) then
				Events.SerialEventGameMessagePopupProcessed.CallImmediate(g_PopupInfo.Type, 0);
			end
            UI.decTurnTimerSemaphore();
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnActivePlayerChanged()
	if (not Controls.ChooseConfirm:IsHidden()) then
		Controls.ChooseConfirm:SetHide(true);
    end
end
Events.GameplaySetActivePlayer.Add(OnActivePlayerChanged);

function OnDirty()
	-- If the user performed any action that changes selection states, just close this UI.
	if not bHidden then
		OnClose();
	end
end
Events.UnitSelectionChanged.Add( OnDirty );
