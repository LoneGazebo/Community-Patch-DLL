if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end

-------------------------------------------------
-- Choose Ideology Popup
-------------------------------------------------

include( "IconSupport" );
include( "InstanceManager" );
include( "CommonBehaviors" );

-- Used for Piano Keys
local ltBlue = {19/255,32/255,46/255,120/255};
local dkBlue = {12/255,22/255,30/255,120/255};

local g_ItemManager = InstanceManager:new( "ItemInstance", "Button", Controls.ItemStack );

local g_ViewTenetsLevel1Manager = InstanceManager:new( "ViewTenetsInstance", "Button", Controls.Level1TenetsStack );
local g_ViewTenetsLevel2Manager = InstanceManager:new( "ViewTenetsInstance", "Button", Controls.Level2TenetsStack );
local g_ViewTenetsLevel3Manager = InstanceManager:new( "ViewTenetsInstance", "Button", Controls.Level3TenetsStack );

local bHidden = true;

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

local g_PopupInfo = nil;
-------------------------------------------------
-------------------------------------------------
function OnPopupMessage(popupInfo)
	  
	local popupType = popupInfo.Type;
	
	g_iUnitIndex = popupInfo.Data2;
	
	if popupType ~= ButtonPopupTypes.BUTTONPOPUP_CHOOSE_IDEOLOGY then
		return;
	end	
	
	g_PopupInfo = popupInfo;
	
   	UIManager:QueuePopup( ContextPtr, PopupPriority.SocialPolicy );
end
Events.SerialEventGameMessagePopup.Add( OnPopupMessage );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    ----------------------------------------------------------------        
    -- Key Down Processing
    ----------------------------------------------------------------        
    if uiMsg == KeyEvents.KeyDown then
        if (wParam == Keys.VK_ESCAPE) then
			if(not Controls.ViewTenetsPopup:IsHidden()) then
				Controls.ViewTenetsPopup:SetHide(true);
			elseif(not Controls.ChooseConfirm:IsHidden()) then
				Controls.ChooseConfirm:SetHide(true);
			else
				OnClose();
			end
        	return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnClose()
    UIManager:DequeuePopup(ContextPtr);
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function RefreshList()
	g_ItemManager:ResetInstances();
		
	local pPlayer = Players[Game.GetActivePlayer()];
	CivIconHookup( pPlayer:GetID(), 64, Controls.CivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true );
	   
	local ideologies = {
		{
			PolicyBranchType = "POLICY_BRANCH_AUTOCRACY",
			VictoryTypes = {"CultureVictory", "DiplomaticVictory", "DominationVictory"},
			Image = "SocialPoliciesAutocracy.dds",
			ImageOffsetX = 0,
			ImageOffsetY = 20,
		},
		{
			PolicyBranchType = "POLICY_BRANCH_FREEDOM",
			VictoryTypes = {"CultureVictory", "DiplomaticVictory", "ScienceVictory"},
			Image = "SocialPoliciesFreedom.dds",
			ImageOffsetX = 0,
			ImageOffsetY = 0,
		},
		{
			PolicyBranchType = "POLICY_BRANCH_ORDER",
			VictoryTypes = {"CultureVictory", "DominationVictory", "ScienceVictory"},
			Image = "SocialPoliciesOrder.dds",
			ImageOffsetX = 0,
			ImageOffsetY = 0,
		}			
	}	
	
	local adoptedIdeologies = {};
	
	for player_num = 0, GameDefines.MAX_CIV_PLAYERS - 1 do
		local player = Players[player_num];
		if(player and player:IsAlive()) then
			local branchId = player:GetLateGamePolicyTree();
			local adoptedIdeology = adoptedIdeologies[branchId] or {};
			table.insert(adoptedIdeology, player:GetID());
		
			adoptedIdeologies[branchId] = adoptedIdeology;
		end
	end
	
	local activePlayerTeam = Teams[pPlayer:GetTeam()];
			
	local tick = true;
	for _, ideology in ipairs(ideologies) do
	
		    local branch = GameInfo.PolicyBranchTypes[ideology.PolicyBranchType];
		    local branchID = branch.ID;
			local itemInstance = g_ItemManager:GetInstance();
			itemInstance.Name:LocalizeAndSetText(branch.Description);
			
			local iFreePolicies = Game.GetNumFreePolicies(branch.ID);
			local szFreePolicyInfo = Locale.Lookup("TXT_KEY_CHOOSE_IDEOLOGY_NUM_FREE_POLICIES", iFreePolicies);
			if (iFreePolicies > 0) then
				szFreePolicyInfo = "[COLOR_POSITIVE_TEXT]" .. szFreePolicyInfo .. "[ENDCOLOR]";
			end
			itemInstance.FreePolicies:SetText(szFreePolicyInfo);	
			
			itemInstance.CultureVictory:SetHide(true);
			itemInstance.DiplomaticVictory:SetHide(true);
			itemInstance.DominationVictory:SetHide(true);
			itemInstance.ScienceVictory:SetHide(true);
        
			for _, victoryControl in ipairs(ideology.VictoryTypes) do
				print(victoryControl);
				itemInstance[victoryControl]:SetHide(false);
			end
			
			itemInstance.AdoptedByStack1:DestroyAllChildren();
			itemInstance.AdoptedByStack2:DestroyAllChildren();
			
			local adoptedIdeology = adoptedIdeologies[branchID];
			
			
			
			if(adoptedIdeology ~= nil and #adoptedIdeology > 0) then
			
				local adoptedIdeologyCount = #adoptedIdeology;
				
				for i = 1, math.min(adoptedIdeologyCount, 12), 1 do
					local control = {};
					ContextPtr:BuildInstanceForControl( "CivInstance", control, itemInstance.AdoptedByStack1);
					
					local playerID = adoptedIdeology[i];
					
					local player = Players[playerID];
					if(activePlayerTeam:IsHasMet(player:GetTeam())) then
						CivIconHookup(playerID, 32, control.CivIcon, control.CivIconBG, control.CivIconShadow, true, true, control.CivIconHighlight);
					else
						CivIconHookup(-1, 32, control.CivIcon, control.CivIconBG, control.CivIconShadow, true, true, control.CivIconHighlight);
					end
				end
				
				if(adoptedIdeologyCount > 12) then
					for i = 13, math.min(adoptedIdeologyCount, 24), 1 do
						local control = {};
						ContextPtr:BuildInstanceForControl( "CivInstance", control, itemInstance.AdoptedByStack2);
						
						local playerID = adoptedIdeology[i];
						
						local player = Players[playerID];
						if(activePlayerTeam:IsHasMet(player:GetTeam())) then
							CivIconHookup(playerID, 32, control.CivIcon, control.CivIconBG, control.CivIconShadow, true, true, control.CivIconHighlight);
						else
							CivIconHookup(-1, 32, control.CivIcon, control.CivIconBG, control.CivIconShadow, true, true, control.CivIconHighlight);
						end
					end
				end
			
				itemInstance.AdoptedByNobody:SetHide(true);
			else
				itemInstance.AdoptedByNobody:SetHide(false);
			end
			
			itemInstance.AdoptedByStack1:CalculateSize();
			itemInstance.AdoptedByStack1:ReprocessAnchoring();
			itemInstance.AdoptedByStack2:CalculateSize();
			itemInstance.AdoptedByStack2:ReprocessAnchoring();
			itemInstance.AdoptedByStacks:CalculateSize();
			itemInstance.AdoptedByStacks:ReprocessAnchoring();
		 
			itemInstance.Button:RegisterCallback(Mouse.eLClick, function() SelectIdeologyChoice(branch.ID); end);
			
			local color = tick and ltBlue or dkBlue;
			tick = not tick;
			
			itemInstance.Box:SetColorVal(unpack(color));
			local buttonWidth, buttonHeight = itemInstance.Button:GetSizeVal();
			local stackWidth, stackHeight = itemInstance.AdoptedByStacks:GetSizeVal();
			
			local newHeight = 170 + stackHeight;	
			
			itemInstance.Button:SetSizeVal(buttonWidth, newHeight);
			itemInstance.Box:SetSizeVal(buttonWidth, newHeight);
			
			itemInstance.BounceAnim:SetSizeVal(buttonWidth, newHeight + 5);
			itemInstance.BounceGrid:SetSizeVal(buttonWidth, newHeight + 5);
			
			itemInstance.IdeologyImage:SetTexture(ideology.Image);
			itemInstance.IdeologyImage:SetTextureOffsetVal(ideology.ImageOffsetX, ideology.ImageOffsetY);
			
			local ideologyImageWidth, ideologyImageHeight = itemInstance.IdeologyImage:GetSizeVal();
			itemInstance.IdeologyImage:SetSizeVal(ideologyImageWidth, newHeight);
			itemInstance.IdeologyImage:SetTextureSizeVal(ideologyImageWidth, newHeight);
			itemInstance.IdeologyImage:NormalizeTexture();
			
			itemInstance.ShowTenets:RegisterCallback(Mouse.eLClick, function()
				ViewTenets(ideology.PolicyBranchType);
			end);
				
	end
	
	Controls.ItemStack:CalculateSize();
	Controls.ItemStack:ReprocessAnchoring();
	Controls.ItemScrollPanel:CalculateInternalSize();
end

function SelectIdeologyChoice(iChoice) 
	g_iChoice = iChoice;
	
	local branch = GameInfo.PolicyBranchTypes[iChoice];
	Controls.ConfirmText:LocalizeAndSetText("TXT_KEY_CHOOSE_IDEOLOGY_CONFIRM", branch.Description); 
	Controls.ChooseConfirm:SetHide(false);
end

function OnConfirmYes( )
	Controls.ChooseConfirm:SetHide(true);
	
	Network.SendIdeologyChoice(Game.GetActivePlayer(), g_iChoice);
	Events.AudioPlay2DSound("AS2D_INTERFACE_POLICY");	
	OnClose();
end
Controls.ConfirmYes:RegisterCallback( Mouse.eLClick, OnConfirmYes );

function OnConfirmNo( )
	Controls.ChooseConfirm:SetHide(true);
end
Controls.ConfirmNo:RegisterCallback( Mouse.eLClick, OnConfirmNo );


function ViewTenets(branchType)
	g_ViewTenetsLevel1Manager:ResetInstances();
	g_ViewTenetsLevel2Manager:ResetInstances();
	g_ViewTenetsLevel3Manager:ResetInstances();
	
	local tick = true;	
	
	function PopulateInstance(instance, text)
		instance.TenetDescription:LocalizeAndSetText(text);
		instance.Button:SetDisabled(true);
		
		local width,height = instance.TenetDescription:GetSizeVal();
		local newHeight = height + 30;
		instance.Button:SetSizeVal(width, newHeight);
		
		local boxWidth, boxHeight = instance.Box:GetSizeVal();
		instance.Box:SetSizeVal(boxWidth, newHeight);
		
		local color = tick and ltBlue or dkBlue;
		tick = not tick;
			
		instance.Box:SetColorVal(unpack(color));
	end
	
	for row in GameInfo.Policies{PolicyBranchType = branchType, Level = 1} do
		local instance = g_ViewTenetsLevel1Manager:GetInstance();
		PopulateInstance(instance, row.Help);
	end
	
	for row in GameInfo.Policies{PolicyBranchType = branchType, Level = 2} do
		local instance = g_ViewTenetsLevel2Manager:GetInstance();
		PopulateInstance(instance, row.Help);
	end
	
	for row in GameInfo.Policies{PolicyBranchType = branchType, Level = 3} do
		local instance = g_ViewTenetsLevel3Manager:GetInstance();
		PopulateInstance(instance, row.Help);
	end
	
	Controls.Level1TenetsStack:CalculateSize();
	Controls.Level1TenetsStack:ReprocessAnchoring();
	Controls.Level2TenetsStack:CalculateSize();
	Controls.Level2TenetsStack:ReprocessAnchoring();
	Controls.Level3TenetsStack:CalculateSize();
	Controls.Level3TenetsStack:ReprocessAnchoring();
	Controls.ViewTenetsStack:CalculateSize();
	Controls.ViewTenetsStack:ReprocessAnchoring();
	
	Controls.ViewTenetsScrollPanel:CalculateInternalSize();
	
	Controls.ViewTenetsPopup:SetHide(false);
end

function OnViewTenetsPopupCloseButton()
	Controls.ViewTenetsPopup:SetHide(true);
end
Controls.ViewTenetsCloseButton:RegisterCallback(Mouse.eLClick, OnViewTenetsPopupCloseButton);


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

function OnDirty()
	-- If the user performed any action that changes selection states, just close this UI.
	if not bHidden then
		OnClose();
	end
end
Events.UnitSelectionChanged.Add( OnDirty );

-------------------------------
-- Collapse/Expand Behaviors --
-------------------------------
function OnCollapseExpand()
	Controls.Level1TenetsStack:CalculateSize();
	Controls.Level1TenetsStack:ReprocessAnchoring();
	Controls.Level2TenetsStack:CalculateSize();
	Controls.Level2TenetsStack:ReprocessAnchoring();
	Controls.Level3TenetsStack:CalculateSize();
	Controls.Level3TenetsStack:ReprocessAnchoring();
	Controls.ViewTenetsStack:CalculateSize();
	Controls.ViewTenetsStack:ReprocessAnchoring();
	
	Controls.ViewTenetsScrollPanel:CalculateInternalSize();
end

local ViewTenetsLevel1HeaderText = Locale.Lookup("TXT_KEY_POLICYSCREEN_L1_TENET");
RegisterCollapseBehavior{	
	Header = Controls.Level1TenetsHeader, 
	HeaderLabel = Controls.Level1TenetsHeaderLabel, 
	HeaderExpandedLabel = "[ICON_MINUS] " .. ViewTenetsLevel1HeaderText,
	HeaderCollapsedLabel = "[ICON_PLUS] " .. ViewTenetsLevel1HeaderText,
	Panel = Controls.Level1TenetsStack,
	Collapsed = false,
	OnCollapse = OnCollapseExpand,
	OnExpand = OnCollapseExpand,
};

local ViewTenetsLevel2HeaderText = Locale.Lookup("TXT_KEY_POLICYSCREEN_L2_TENET");
RegisterCollapseBehavior{	
	Header = Controls.Level2TenetsHeader, 
	HeaderLabel = Controls.Level2TenetsHeaderLabel, 
	HeaderExpandedLabel = "[ICON_MINUS] " .. ViewTenetsLevel2HeaderText,
	HeaderCollapsedLabel = "[ICON_PLUS] " .. ViewTenetsLevel2HeaderText,
	Panel = Controls.Level2TenetsStack,
	Collapsed = false,
	OnCollapse = OnCollapseExpand,
	OnExpand = OnCollapseExpand,
};

local ViewTenetsLevel3HeaderText = Locale.Lookup("TXT_KEY_POLICYSCREEN_L3_TENET");
RegisterCollapseBehavior{	
	Header = Controls.Level3TenetsHeader, 
	HeaderLabel = Controls.Level3TenetsHeaderLabel, 
	HeaderExpandedLabel = "[ICON_MINUS] " .. ViewTenetsLevel3HeaderText,
	HeaderCollapsedLabel = "[ICON_PLUS] " .. ViewTenetsLevel3HeaderText,
	Panel = Controls.Level3TenetsStack,
	Collapsed = false,
	OnCollapse = OnCollapseExpand,
	OnExpand = OnCollapseExpand,
};