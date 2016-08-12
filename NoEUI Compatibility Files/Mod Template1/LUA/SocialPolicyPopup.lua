print("This is the modded SocialPolicyPopup from CBP")
-------------------------------------------------
-- SocialPolicy Chooser Popup
-------------------------------------------------

include( "IconSupport" );
include( "InstanceManager" );
include( "CommonBehaviors" );

local m_PopupInfo = nil;

local g_LibertyPipeManager = InstanceManager:new( "ConnectorPipe", "ConnectorImage", Controls.LibertyPanel );
local g_TraditionPipeManager = InstanceManager:new( "ConnectorPipe", "ConnectorImage", Controls.TraditionPanel );
local g_HonorPipeManager = InstanceManager:new( "ConnectorPipe", "ConnectorImage", Controls.HonorPanel );
local g_PietyPipeManager = InstanceManager:new( "ConnectorPipe", "ConnectorImage", Controls.PietyPanel );
local g_AestheticsPipeManager = InstanceManager:new ( "ConnectorPipe", "ConnectorImage", Controls.AestheticsPanel );
local g_PatronagePipeManager = InstanceManager:new( "ConnectorPipe", "ConnectorImage", Controls.PatronagePanel );
local g_CommercePipeManager = InstanceManager:new( "ConnectorPipe", "ConnectorImage", Controls.CommercePanel );
local g_ExplorationPipeManager = InstanceManager:new ( "ConnectorPipe", "ConnectorImage", Controls.ExplorationPanel );
local g_RationalismPipeManager = InstanceManager:new( "ConnectorPipe", "ConnectorImage", Controls.RationalismPanel );

local g_LibertyInstanceManager = InstanceManager:new( "PolicyButton", "PolicyIcon", Controls.LibertyPanel );
local g_TraditionInstanceManager = InstanceManager:new( "PolicyButton", "PolicyIcon", Controls.TraditionPanel );
local g_HonorInstanceManager = InstanceManager:new( "PolicyButton", "PolicyIcon", Controls.HonorPanel );
local g_PietyInstanceManager = InstanceManager:new( "PolicyButton", "PolicyIcon", Controls.PietyPanel );
local g_AestheticsInstanceManager = InstanceManager:new ( "PolicyButton", "PolicyIcon", Controls.AestheticsPanel );
local g_PatronageInstanceManager = InstanceManager:new( "PolicyButton", "PolicyIcon", Controls.PatronagePanel );
local g_CommerceInstanceManager = InstanceManager:new( "PolicyButton", "PolicyIcon", Controls.CommercePanel );
local g_ExplorationInstanceManager = InstanceManager:new( "PolicyButton", "PolicyIcon", Controls.ExplorationPanel );
local g_RationalismInstanceManager = InstanceManager:new( "PolicyButton", "PolicyIcon", Controls.RationalismPanel );

local g_TenetInstanceManager = InstanceManager:new( "TenetChoice", "TenetButton", Controls.TenetStack );

include( "FLuaVector" );

local fullColor = {x = 1, y = 1, z = 1, w = 1};
local fadeColor = {x = 1, y = 1, z = 1, w = 0};
local fadeColorRV = {x = 1, y = 1, z = 1, w = 0.2};
local pinkColor = {x = 2, y = 0, z = 2, w = 1};
local lockTexture = "48Lock.dds";
local checkTexture = "48Checked.dds";

local hTexture = "Connect_H.dds";
local vTexture = "Connect_V.dds";

local topRightTexture =		"Connect_JonCurve_TopRight.dds"
local topLeftTexture =		"Connect_JonCurve_TopLeft.dds"
local bottomRightTexture =	"Connect_JonCurve_BottomRight.dds"
local bottomLeftTexture =	"Connect_JonCurve_BottomLeft.dds"

local policyIcons = {};

local g_PolicyXOffset = 28;
local g_PolicyYOffset = 68;

local g_PolicyPipeXOffset = 28;
local g_PolicyPipeYOffset = 68;

local m_gPolicyID;
local m_gAdoptingPolicy;

g_Tabs = {
	["SocialPolicies"] = {
		Button = Controls.TabButtonSocialPolicies,
		Panel = Controls.SocialPolicyPane,
		SelectHighlight = Controls.TabButtonSocialPoliciesHL,
	},
	
	["Ideologies"] = {
		Button = Controls.TabButtonIdeologies,
		Panel = Controls.IdeologyPane,
		SelectHighlight = Controls.TabButtonIdeologiesHL,
	},
}

local g_IdeologyBackgrounds = {
		[0] = "PolicyBranch_Ideology.dds",
		POLICY_BRANCH_AUTOCRACY = "SocialPoliciesAutocracy.dds",
		POLICY_BRANCH_FREEDOM = "SocialPoliciesFreedom.dds",
		POLICY_BRANCH_ORDER = "SocialPoliciesOrder.dds",
}

-------------------------------------------------
-- On Policy Selected
-------------------------------------------------
function PolicySelected( policyIndex )
    
    print("Clicked on Policy: " .. tostring(policyIndex));
    
	if policyIndex == -1 then
		return;
	end
    local player = Players[Game.GetActivePlayer()];   
    if player == nil then
		return;
    end
    
    local bHasPolicy = player:HasPolicy(policyIndex);
    local bCanAdoptPolicy = player:CanAdoptPolicy(policyIndex);
    
    print("bHasPolicy: " .. tostring(bHasPolicy));
    print("bCanAdoptPolicy: " .. tostring(bCanAdoptPolicy));
    print("Policy Blocked: " .. tostring(player:IsPolicyBlocked(policyIndex)));
    
    local bPolicyBlocked = false;
    
    -- If we can get this, OR we already have it, see if we can unblock it first
    if (bHasPolicy or bCanAdoptPolicy) then
    
		-- Policy blocked off right now? If so, try to activate
		if (player:IsPolicyBlocked(policyIndex)) then
			
			bPolicyBlocked = true;
			
			local strPolicyBranch = GameInfo.Policies[policyIndex].PolicyBranchType;
			local iPolicyBranch = GameInfoTypes[strPolicyBranch];
			
			print("Policy Branch: " .. tostring(iPolicyBranch));
			
			local popupInfo = {
				Type = ButtonPopupTypes.BUTTONPOPUP_CONFIRM_POLICY_BRANCH_SWITCH,
				Data1 = iPolicyBranch;
			}
			Events.SerialEventGameMessagePopup(popupInfo);
			
		end
    end
    
    -- Can adopt Policy right now - don't try this if we're going to unblock the Policy instead
    if (bCanAdoptPolicy and not bPolicyBlocked) then
		m_gPolicyID = policyIndex;
		m_gAdoptingPolicy = true;
		Controls.PolicyConfirm:SetHide(false);
		Controls.BGBlock:SetHide(true);
	end
	
end

-------------------------------------------------
-- On Policy Branch Selected
-------------------------------------------------
function PolicyBranchSelected( policyBranchIndex )
    
    --print("Clicked on PolicyBranch: " .. tostring(policyBranchIndex));
    
	if policyBranchIndex == -1 then
		return;
	end
    local player = Players[Game.GetActivePlayer()];   
    if player == nil then
		return;
    end
    
    local bHasPolicyBranch = player:IsPolicyBranchUnlocked(policyBranchIndex);
    local bCanAdoptPolicyBranch = player:CanUnlockPolicyBranch(policyBranchIndex);
    
    --print("bHasPolicyBranch: " .. tostring(bHasPolicyBranch));
    --print("bCanAdoptPolicyBranch: " .. tostring(bCanAdoptPolicyBranch));
   -- print("PolicyBranch Blocked: " .. tostring(player:IsPolicyBranchBlocked(policyBranchIndex)));
    
    local bUnblockingPolicyBranch = false;
    
    -- If we can get this, OR we already have it, see if we can unblock it first
    if (bHasPolicyBranch or bCanAdoptPolicyBranch) then
    
		-- Policy Branch blocked off right now? If so, try to activate
		if (player:IsPolicyBranchBlocked(policyBranchIndex)) then
			
			bUnblockingPolicyBranch = true;
			
			local popupInfo = {
				Type = ButtonPopupTypes.BUTTONPOPUP_CONFIRM_POLICY_BRANCH_SWITCH,
				Data1 = policyBranchIndex;
			}
			Events.SerialEventGameMessagePopup(popupInfo);
		end
    end
    
    -- Can adopt Policy Branch right now - don't try this if we're going to unblock the Policy Branch instead
    if (bCanAdoptPolicyBranch and not bUnblockingPolicyBranch) then
		m_gPolicyID = policyBranchIndex;
		m_gAdoptingPolicy = false;
		Controls.PolicyConfirm:SetHide(false);
		Controls.BGBlock:SetHide(true);
	end
end

-------------------------------------------------
-------------------------------------------------
function OnPopupMessage(popupInfo)
	
	local popupType = popupInfo.Type;
	if popupType ~= ButtonPopupTypes.BUTTONPOPUP_CHOOSEPOLICY then
		return;
	end
	
	m_PopupInfo = popupInfo;

	UpdateDisplay();
	
	if(m_PopupInfo.Data2 == 2) then
		TabSelect(g_Tabs["Ideologies"]);
	else
		TabSelect(g_Tabs["SocialPolicies"]);
	end
	
	if( m_PopupInfo.Data1 == 1 ) then
    	if( ContextPtr:IsHidden() == false ) then
    	    OnClose();
    	else
        	UIManager:QueuePopup( ContextPtr, PopupPriority.InGameUtmost );
    	end
	else
    	UIManager:QueuePopup( ContextPtr, PopupPriority.SocialPolicy );
	end
end
Events.SerialEventGameMessagePopup.Add( OnPopupMessage );

-------------------------------------------------
-------------------------------------------------
function UpdateDisplay()

    local player = Players[Game.GetActivePlayer()];
   	CivIconHookup( player:GetID(), 64, Controls.CivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true );

	print ("In UpdateDisplay()");

    local pTeam = Teams[player:GetTeam()];
    
    if player == nil then
		return;
    end
    
    local playerHas1City = player:GetNumCities() > 0;
    
    local bShowAll = OptionsManager.GetPolicyInfo();
    
    local szText = Locale.ConvertTextKey("TXT_KEY_NEXT_POLICY_COST_LABEL", player:GetNextPolicyCost());
    Controls.NextCost:SetText(szText);
    
    szText = Locale.ConvertTextKey("TXT_KEY_CURRENT_CULTURE_LABEL", player:GetJONSCulture());
    Controls.CurrentCultureLabel:SetText(szText);
    
    szText = Locale.ConvertTextKey("TXT_KEY_CULTURE_PER_TURN_LABEL", player:GetTotalJONSCulturePerTurn());
    Controls.CulturePerTurnLabel:SetText(szText);
    
    local iTurns;
    local iCultureNeeded = player:GetNextPolicyCost() - player:GetJONSCulture();
    if (iCultureNeeded <= 0) then
		iTurns = 0;
    else
		if (player:GetTotalJONSCulturePerTurn() == 0) then
			iTurns = "?";
		else
			iTurns = iCultureNeeded / player:GetTotalJONSCulturePerTurn();
			iTurns = iTurns + 1;
			iTurns = math.floor(iTurns);
		end
    end
    szText = Locale.ConvertTextKey("TXT_KEY_NEXT_POLICY_TURN_LABEL", iTurns);
    Controls.NextPolicyTurnLabel:SetText(szText);
    
    -- Player Title
    local iDominantBranch = player:GetDominantPolicyBranchForTitle();
    if (iDominantBranch ~= -1) then
		
		local strTextKey = GameInfo.PolicyBranchTypes[iDominantBranch].Title;
		
		local strText = Locale.ConvertTextKey(strTextKey, player:GetNameKey(), player:GetCivilizationShortDescriptionKey());
		
	    Controls.PlayerTitleLabel:SetHide(false);
	    Controls.PlayerTitleLabel:SetText(strText);
	else
	    Controls.PlayerTitleLabel:SetHide(true);
    end
    
    -- Free Policies
    local iNumFreePolicies = player:GetNumFreePolicies();
    if (iNumFreePolicies > 0) then
	    szText = Locale.ConvertTextKey("TXT_KEY_FREE_POLICIES_LABEL", iNumFreePolicies);
	    Controls.FreePoliciesLabel:SetText( szText );
	    Controls.FreePoliciesLabel:SetHide( false );
	else
	    Controls.FreePoliciesLabel:SetHide( true );
    end
    
	Controls.InfoStack:ReprocessAnchoring();
	Controls.InfoStack2:ReprocessAnchoring();
    
	--szText = Locale.ConvertTextKey( "TXT_KEY_SOCIAL_POLICY_DIRECTIONS" );
    --Controls.ReminderText:SetText( szText );

	local justLooking = true;
	if player:GetJONSCulture() >= player:GetNextPolicyCost() then
		justLooking = false;
	end
	
	-- Adjust Policy Branches
	local i = 0;
	local numUnlockedBranches = player:GetNumPolicyBranchesUnlocked();
--	if numUnlockedBranches > 0 then
		local policyBranchInfo = GameInfo.PolicyBranchTypes[i];
		while policyBranchInfo ~= nil and not policyBranchInfo.PurchaseByLevel do
			local numString = tostring( i );
			
			local buttonName = "BranchButton"..numString;
			local backName = "BranchBack"..numString;
			local DisabledBoxName = "DisabledBox"..numString;
			local LockedBoxName = "LockedBox"..numString;
			local ImageMaskName = "ImageMask"..numString;
			local DisabledMaskName = "DisabledMask"..numString;
			--local EraLabelName = "EraLabel"..numString;
			
			local thisButton = Controls[buttonName];
			local thisBack = Controls[backName];
			local thisDisabledBox = Controls[DisabledBoxName];
			local thisLockedBox = Controls[LockedBoxName];
			
			local thisImageMask = Controls[ImageMaskName];
			local thisDisabledMask = Controls[DisabledMaskName];
			
			
			if(thisImageMask == nil) then
				print(ImageMaskName);
			end
			--local thisEraLabel = Controls[EraLabelName];
			
			local strToolTip = Locale.ConvertTextKey(policyBranchInfo.Help);
			
			-- Era Prereq
			local iEraPrereq = GameInfoTypes[policyBranchInfo.EraPrereq]
			local bEraLock = false;
-- Change (CBP)
--			if (iEraPrereq ~= nil and pTeam:GetCurrentEra() < iEraPrereq) then
			if (iEraPrereq ~= nil and not player:CanUnlockPolicyBranch( i )) then
-- END
				bEraLock = true;
			else
				--thisEraLabel:SetHide(true);
			end
-- CBP
			if (iEraPrereq ~= nil and pTeam:GetCurrentEra() >= iEraPrereq) then
				bEraLock = false;
			elseif player:IsPolicyBranchUnlocked ( i ) then
				bEraLock = false;
			end
--END
			
			local lockName = "Lock"..numString;
			local thisLock = Controls[lockName];
			
			-- Branch is not yet unlocked
			if not player:IsPolicyBranchUnlocked( i ) then
				
				-- Cannot adopt this branch right now
				if (policyBranchInfo.LockedWithoutReligion and Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
					strToolTip = strToolTip .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_POLICY_BRANCH_CANNOT_UNLOCK_RELIGION");

				elseif (not player:CanUnlockPolicyBranch(i)) then
					
					local iNumPolicies = player:GetNumPolicies(true, true);
					local iNeed = policyBranchInfo.NumPolicyRequirement;
					local iNeeded = iNeed - iNumPolicies;
					if(iNeeded > 0) then
						strToolTip = strToolTip .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_POLICY_BRANCH_CANNOT_UNLOCK_NEED_MORE", iNeeded);
					else
						strToolTip = strToolTip .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_POLICY_BRANCH_CANNOT_UNLOCK");
					end
					
					-- Wrong Era and Need Policies
					if(bEraLock and iNeeded > 0) then
						local strEra = GameInfo.Eras[iEraPrereq].Description;
						strToolTip = strToolTip .. " " .. Locale.ConvertTextKey("TXT_KEY_POLICY_BRANCH_CANNOT_UNLOCK_ERA_ALT", strEra);
						local strEraTitle = Locale.ConvertTextKey(strEra);
						thisButton:SetText( strEraTitle );

					-- Not in prereq Era
					elseif (bEraLock and iNeeded <= 0) then
						local strEra = GameInfo.Eras[iEraPrereq].Description;
						strToolTip = strToolTip .. " " .. Locale.ConvertTextKey("TXT_KEY_POLICY_BRANCH_CANNOT_UNLOCK_ERA", strEra);
						
						-- Era Label
						--local strEraTitle = "[COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey(strEra) .. "[ENDCOLOR]";
						local strEraTitle = Locale.ConvertTextKey(strEra);
						thisButton:SetText( strEraTitle );
						--thisEraLabel:SetText(strEraTitle);
						--thisEraLabel:SetHide( true );
						
						--thisButton:SetHide( true );
						
					-- Don't have enough Culture Yet
					else
						strToolTip = strToolTip .. " " .. Locale.ConvertTextKey("TXT_KEY_POLICY_BRANCH_CANNOT_UNLOCK_CULTURE", player:GetNextPolicyCost());
						thisButton:SetHide( false );
						thisButton:SetText( Locale.ConvertTextKey( "TXT_KEY_POP_ADOPT_BUTTON" ) );
						--thisEraLabel:SetHide( true );
					end
					
					thisLock:SetHide( false );
					thisButton:SetDisabled( true );
				-- Can adopt this branch right now
				else
					strToolTip = strToolTip .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_POLICY_BRANCH_UNLOCK_SPEND", player:GetNextPolicyCost());
					thisLock:SetHide( true );
					--thisEraLabel:SetHide( true );
					thisButton:SetDisabled( false );
					thisButton:SetHide( false );
					thisButton:SetText( Locale.ConvertTextKey( "TXT_KEY_POP_ADOPT_BUTTON" ) );
				end
				
				if(not playerHas1City) then
					thisButton:SetDisabled(true);
				end
				
				thisBack:SetColor( fadeColor );
				thisLockedBox:SetHide(false);
				
				thisImageMask:SetHide(true);
				thisDisabledMask:SetHide(false);
				
			-- Branch is unlocked, but blocked by another branch
			elseif (player:IsPolicyBranchBlocked(i)) then
				thisButton:SetHide( false );
				thisBack:SetColor( fadeColor );
				thisLock:SetHide( false );
				thisLockedBox:SetHide(true);
				
				strToolTip = strToolTip .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_POLICY_BRANCH_BLOCKED");
				
			-- Branch is unlocked already
			else
				thisButton:SetHide( true );
				thisBack:SetColor( fullColor );
				thisLockedBox:SetHide(true);
				
				thisImageMask:SetHide(false);
				thisDisabledMask:SetHide(true);
			end
			
			-- Update tooltips
			thisButton:SetToolTipString(strToolTip);
			
			-- If the player doesn't have the era prereq, then dim out the branch
			if (bEraLock) then
				thisDisabledBox:SetHide(false);
				thisLockedBox:SetHide(true);
			else
				thisDisabledBox:SetHide(true);
			end
			
			if (bShowAll) then
				thisDisabledBox:SetHide(true);
				thisLockedBox:SetHide(true);
			end
			
			i = i + 1;
			policyBranchInfo = GameInfo.PolicyBranchTypes[i];
		end
	--else
		--local policyBranchInfo = GameInfo.PolicyBranchTypes[i];
		--while policyBranchInfo ~= nil do
			--local numString = tostring(i);
			--local buttonName = "BranchButton"..numString;
			--local backName = "BranchBack"..numString;
			--local thisButton = Controls[buttonName];
			--local thisBack = Controls[backName];
			--thisBack:SetColor( fullColor );
			--thisButton:SetHide( true );
			--i = i + 1;
			--policyBranchInfo = GameInfo.PolicyBranchTypes[i];
		--end
	--end
	
	-- Adjust Policy buttons
	
	
	i = 0;
	local policyInfo = GameInfo.Policies[i];
	while policyInfo ~= nil do
		
		local iBranch = policyInfo.PolicyBranchType;
		
		-- If this is nil it means the Policy is a freebie handed out with the Branch, so don't display it
		if (iBranch ~= nil and not GameInfo.PolicyBranchTypes[iBranch].PurchaseByLevel) then
			
			local thisPolicyIcon = policyIcons[i];
			
			-- Tooltip
			local strTooltip = Locale.ConvertTextKey( policyInfo.Help );
			
			-- Player already has Policy
			if player:HasPolicy( i ) then
				--thisPolicyIcon.Lock:SetTexture( checkTexture ); 
				--thisPolicyIcon.Lock:SetHide( true ); 
				thisPolicyIcon.MouseOverContainer:SetHide( true );
				thisPolicyIcon.PolicyIcon:SetDisabled( true );
				--thisPolicyIcon.PolicyIcon:SetVoid1( -1 );
				thisPolicyIcon.PolicyImage:SetColor( fullColor );
				IconHookup( policyInfo.PortraitIndex, 64, policyInfo.IconAtlasAchieved, thisPolicyIcon.PolicyImage );
			elseif(not playerHas1City) then
				--thisPolicyIcon.Lock:SetTexture( lockTexture ); 
				thisPolicyIcon.MouseOverContainer:SetHide( true );
				--thisPolicyIcon.Lock:SetHide( true ); 
				thisPolicyIcon.PolicyIcon:SetDisabled( true );
				--thisPolicyIcon.Lock:SetHide( false ); 
				--thisPolicyIcon.PolicyIcon:SetVoid1( -1 );
				thisPolicyIcon.PolicyImage:SetColor( fadeColorRV );
				IconHookup( policyInfo.PortraitIndex, 64, policyInfo.IconAtlas, thisPolicyIcon.PolicyImage );
				-- Tooltip
				strTooltip = strTooltip .. "[NEWLINE][NEWLINE]"
			
			-- Can adopt the Policy right now
			elseif player:CanAdoptPolicy( i ) then
				--thisPolicyIcon.Lock:SetHide( true ); 
				thisPolicyIcon.MouseOverContainer:SetHide( false );
				thisPolicyIcon.PolicyIcon:SetDisabled( false );
				if justLooking then
					--thisPolicyIcon.PolicyIcon:SetVoid1( -1 );
					thisPolicyIcon.PolicyImage:SetColor( fullColor );
				else
					thisPolicyIcon.PolicyIcon:SetVoid1( i ); -- indicates policy to be chosen
					thisPolicyIcon.PolicyImage:SetColor( fullColor );
				end
				IconHookup( policyInfo.PortraitIndex, 64, policyInfo.IconAtlas, thisPolicyIcon.PolicyImage );
				
			-- Policy is unlocked, but we lack culture
			elseif player:CanAdoptPolicy(i, true) then
				--thisPolicyIcon.Lock:SetTexture( lockTexture ); 
				thisPolicyIcon.MouseOverContainer:SetHide( true );
				--thisPolicyIcon.Lock:SetHide( true ); 
				thisPolicyIcon.PolicyIcon:SetDisabled( true );
				--thisPolicyIcon.Lock:SetHide( false ); 
				--thisPolicyIcon.PolicyIcon:SetVoid1( -1 );
				thisPolicyIcon.PolicyImage:SetColor( fadeColorRV );
				IconHookup( policyInfo.PortraitIndex, 64, policyInfo.IconAtlas, thisPolicyIcon.PolicyImage );
				-- Tooltip
				strTooltip = strTooltip .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_POLICY_BRANCH_CANNOT_UNLOCK_CULTURE", player:GetNextPolicyCost());
			else
				--thisPolicyIcon.Lock:SetTexture( lockTexture ); 
				thisPolicyIcon.MouseOverContainer:SetHide( true );
				--thisPolicyIcon.Lock:SetHide( true ); 
				thisPolicyIcon.PolicyIcon:SetDisabled( true );
				--thisPolicyIcon.Lock:SetHide( false ); 
				--thisPolicyIcon.PolicyIcon:SetVoid1( -1 );
				thisPolicyIcon.PolicyImage:SetColor( fadeColorRV );
				IconHookup( policyInfo.PortraitIndex, 64, policyInfo.IconAtlas, thisPolicyIcon.PolicyImage );
				-- Tooltip
				strTooltip = strTooltip .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_POLICY_CANNOT_UNLOCK");
			end
			
			-- Policy is Blocked
			if player:IsPolicyBlocked(i) then
				thisPolicyIcon.PolicyImage:SetColor( fadeColorRV );
				IconHookup( policyInfo.PortraitIndex, 64, policyInfo.IconAtlas, thisPolicyIcon.PolicyImage );
				
				-- Update tooltip if we have this Policy
				if player:HasPolicy( i ) then
					strTooltip = strTooltip .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_POLICY_BRANCH_BLOCKED");
				end
			end
				
			thisPolicyIcon.PolicyIcon:SetToolTipString( strTooltip );
		end
		
		i = i + 1;
		policyInfo = GameInfo.Policies[i];
	end
	
	-- Adjust Ideology
	local iIdeology = player:GetLateGamePolicyTree();
	local iLevel1Tenets = {};
	local iLevel2Tenets = {};
	local iLevel3Tenets = {};

	for i = 1, 7 do
		iLevel1Tenets[i] = player:GetTenet(iIdeology, 1, i);
		local buttonControl = Controls["IdeologyButton1" .. tostring(i)];
		local labelControl = Controls["IdeologyButtonLabel1" .. tostring(i)];
		local lockControl = Controls["Lock1" .. tostring(i)];
	
		buttonControl:SetDisabled(true);
		buttonControl:ClearCallback( Mouse.eLClick );
		buttonControl:SetToolTipString("");
		labelControl:SetString("");
		lockControl:SetHide(false);
		
		if (iLevel1Tenets[i] >= 0) then
		    local tenet = GameInfo.Policies[iLevel1Tenets[i]];
		 	labelControl:LocalizeAndSetText(tenet.Description);
		 	lockControl:SetHide(true);
     		if (tenet.Help ~= nil) then
     			local strToolTip = Locale.ConvertTextKey(tenet.Help);
     			buttonControl:SetToolTipString(strToolTip);
     		end
     	else
		    if ((i == 1 and iIdeology >= 0) or (i > 1 and iLevel1Tenets[i-1] >= 0)) then
    			labelControl:LocalizeAndSetText("TXT_KEY_POLICYSCREEN_ADD_TENET");
 				lockControl:SetHide(true);
 				if (player:GetJONSCulture() >= player:GetNextPolicyCost() or player:GetNumFreePolicies() > 0 or player:GetNumFreeTenets() > 0) then
      				buttonControl:RegisterCallback( Mouse.eLClick, function() TenetSelect(10 + i); end);	
					buttonControl:SetDisabled(false);
 				else
      			    buttonControl:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_POLICY_BRANCH_CANNOT_UNLOCK_CULTURE", player:GetNextPolicyCost()));				
 				end
 			end
		end
	end
	for i = 1, 4 do
		iLevel2Tenets[i] = player:GetTenet(iIdeology, 2, i);
		local buttonControl = Controls["IdeologyButton2" .. tostring(i)];
		local labelControl = Controls["IdeologyButtonLabel2" .. tostring(i)];
		local lockControl = Controls["Lock2" .. tostring(i)];
		
		buttonControl:SetDisabled(true);
		buttonControl:ClearCallback( Mouse.eLClick );
		buttonControl:SetToolTipString("");
		labelControl:SetString("");
		lockControl:SetHide(false);
	
		if (iLevel2Tenets[i] >= 0) then
		    local tenet = GameInfo.Policies[iLevel2Tenets[i]];
     		labelControl:LocalizeAndSetText(tenet.Description);
     		if (tenet.Help ~= nil) then
     			local strToolTip = Locale.ConvertTextKey(tenet.Help);
     			buttonControl:SetToolTipString(strToolTip);
      		end
    		lockControl:SetHide(true);
     	else
		    if (iLevel1Tenets[i+1] >= 0) then
    			labelControl:LocalizeAndSetText("TXT_KEY_POLICYSCREEN_ADD_TENET");
 				lockControl:SetHide(true);
 				if (player:GetJONSCulture() >= player:GetNextPolicyCost() or player:GetNumFreePolicies() > 0 or player:GetNumFreeTenets() > 0) then
      				buttonControl:RegisterCallback( Mouse.eLClick, function() TenetSelect(20 + i); end);	
				    buttonControl:SetDisabled(false);
 				else
      			    buttonControl:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_POLICY_BRANCH_CANNOT_UNLOCK_CULTURE", player:GetNextPolicyCost()));				
 				end			
			elseif (iLevel1Tenets[i] >= 0) then
				buttonControl:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_POLICYSCREEN_NEED_L1_TENETS_TOOLTIP"));
 			end
		end
	end
	
	for i = 1, 3 do
		iLevel3Tenets[i] = player:GetTenet(iIdeology, 3, i);
		local buttonControl = Controls["IdeologyButton3" .. tostring(i)];
		local labelControl = Controls["IdeologyButtonLabel3" .. tostring(i)];
		local lockControl = Controls["Lock3" .. tostring(i)];
		
		buttonControl:SetDisabled(true);
		buttonControl:ClearCallback( Mouse.eLClick );
		buttonControl:SetToolTipString("");
		labelControl:SetString("");
		lockControl:SetHide(false);
		
		if (iLevel3Tenets[i] >= 0) then
		    local tenet = GameInfo.Policies[iLevel3Tenets[i]];
     		labelControl:LocalizeAndSetText(tenet.Description);
     		lockControl:SetHide(true);
     		if (tenet.Help ~= nil) then
     			local strToolTip = Locale.ConvertTextKey(tenet.Help);
     			buttonControl:SetToolTipString(strToolTip);
      		end
     	else
		    if (iLevel2Tenets[i+1] >= 0) then
    			labelControl:LocalizeAndSetText("TXT_KEY_POLICYSCREEN_ADD_TENET");
 				lockControl:SetHide(true);
				
 				if (player:GetJONSCulture() >= player:GetNextPolicyCost() or player:GetNumFreePolicies() > 0 or player:GetNumFreeTenets() > 0) then
      				buttonControl:RegisterCallback( Mouse.eLClick, function() TenetSelect(30 + i); end);	
				    buttonControl:SetDisabled(false);
 				else
      			    buttonControl:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_POLICY_BRANCH_CANNOT_UNLOCK_CULTURE", player:GetNextPolicyCost()));				
 				end				
			elseif (iLevel2Tenets[i] >= 0) then
				buttonControl:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_POLICYSCREEN_NEED_L2_TENETS_TOOLTIP"));
			end
		end
	end
	
	if (iIdeology >= 0) then
	
	    -- Free Tenets
		local iNumFreeTenets = player:GetNumFreeTenets();
		if (iNumFreeTenets > 0) then
			szText = Locale.ConvertTextKey("TXT_KEY_FREE_TENETS_LABEL", iNumFreeTenets);
			Controls.FreeTenetsLabel:SetText( szText );
			Controls.FreeTenetsLabel:SetHide( false );
		else
			Controls.FreeTenetsLabel:SetHide( true );
		end
    
		local ideology = GameInfo.PolicyBranchTypes[iIdeology];
	    local ideologyName = Locale.Lookup("TXT_KEY_POLICYSCREEN_IDEOLOGY_TITLE", player:GetCivilizationAdjectiveKey(), ideology.Description); 
	    Controls.IdeologyHeader:SetText(ideologyName);
	    Controls.IdeologyImage1:SetTexture(g_IdeologyBackgrounds[ideology.Type]);
	    Controls.IdeologyImage2:SetTexture(g_IdeologyBackgrounds[ideology.Type]);
	    
	    local level1Count = 0;
	    local level2Count = 0;
	    local level3Count = 0;
	    
	    for i,v in ipairs(iLevel1Tenets) do
			if(v >= 0) then
				level1Count = level1Count + 1;
			end
	    end
	    
	      for i,v in ipairs(iLevel2Tenets) do
			if(v >= 0) then
				level2Count = level2Count + 1;
			end
	    end
	    
	      for i,v in ipairs(iLevel3Tenets) do
			if(v >= 0) then
				level3Count = level3Count + 1;
			end
	    end
	    
	    
	    Controls.Level1Tenets:LocalizeAndSetText("TXT_KEY_POLICYSCREEN_IDEOLOGY_L1TENETS", level1Count);
	    Controls.Level2Tenets:LocalizeAndSetText("TXT_KEY_POLICYSCREEN_IDEOLOGY_L2TENETS", level2Count);
	    Controls.Level3Tenets:LocalizeAndSetText("TXT_KEY_POLICYSCREEN_IDEOLOGY_L3TENETS", level3Count);
	    
	    Controls.TenetsStack:CalculateSize();
	    Controls.TenetsStack:ReprocessAnchoring();
	    
	    local ideologyTitle = Locale.ToUpper(ideologyName);
	    Controls.IdeologyTitle:SetText(ideologyTitle);
	    Controls.ChooseTenetTitle:SetText(ideologyTitle);
		Controls.NoIdeology:SetHide(true);
		--CBP
		Controls.PoliciesLeft:SetHide(true);
		--END
		Controls.DisabledIdeology:SetHide(true);
		Controls.HasIdeology:SetHide(false);
		
		Controls.IdeologyGenericHeader:SetHide(true);
		Controls.IdeologyDetails:SetHide(false);
		
		local szOpinionString;
		local iOpinion = player:GetPublicOpinionType();
		if (iOpinion == PublicOpinionTypes.PUBLIC_OPINION_DISSIDENTS) then
			szOpinionString = Locale.ConvertTextKey("TXT_KEY_CO_PUBLIC_OPINION_DISSIDENTS");
		elseif (iOpinion == PublicOpinionTypes.PUBLIC_OPINION_CIVIL_RESISTANCE) then
			szOpinionString = Locale.ConvertTextKey("TXT_KEY_CO_PUBLIC_OPINION_CIVIL_RESISTANCE");
		elseif (iOpinion == PublicOpinionTypes.PUBLIC_OPINION_REVOLUTIONARY_WAVE) then
			szOpinionString = Locale.ConvertTextKey("TXT_KEY_CO_PUBLIC_OPINION_REVOLUTIONARY_WAVE");
		else
			szOpinionString = Locale.ConvertTextKey("TXT_KEY_CO_PUBLIC_OPINION_CONTENT");
		end
		Controls.PublicOpinion:SetText(szOpinionString);
		Controls.PublicOpinion:SetToolTipString(player:GetPublicOpinionTooltip());
				
		local iUnhappiness = -1 * player:GetPublicOpinionUnhappiness();
		local strPublicOpinionUnhappiness = tostring(0);
		local strChangeIdeologyTooltip = "";
		if (iUnhappiness < 0) then
			strPublicOpinionUnhappiness = Locale.ConvertTextKey("TXT_KEY_CO_PUBLIC_OPINION_UNHAPPINESS", iUnhappiness);
			Controls.SwitchIdeologyButton:SetDisabled(false);	
			local ePreferredIdeology = player:GetPublicOpinionPreferredIdeology();
			local strPreferredIdeology = GameInfo.PolicyBranchTypes[ePreferredIdeology].Description;
			strChangeIdeologyTooltip = Locale.ConvertTextKey("TXT_KEY_POLICYSCREEN_CHANGE_IDEOLOGY_TT", strPreferredIdeology, (-1 * iUnhappiness), 2);
			Controls.SwitchIdeologyButton:RegisterCallback(Mouse.eLClick, function()
					ChooseChangeIdeology();
			end);
		else
			Controls.SwitchIdeologyButton:SetDisabled(true);	
		strChangeIdeologyTooltip = Locale.ConvertTextKey("TXT_KEY_POLICYSCREEN_CHANGE_IDEOLOGY_DISABLED_TT");
		end
		Controls.PublicOpinionUnhappiness:SetText(strPublicOpinionUnhappiness);
		Controls.PublicOpinionUnhappiness:SetToolTipString(player:GetPublicOpinionUnhappinessTooltip());
		Controls.SwitchIdeologyButton:SetToolTipString(strChangeIdeologyTooltip);
		
		Controls.PublicOpinionHeader:SetHide(false);
		Controls.PublicOpinion:SetHide(false);
		Controls.PublicOpinionUnhappinessHeader:SetHide(false);
		Controls.PublicOpinionUnhappiness:SetHide(false);
		Controls.SwitchIdeologyButton:SetHide(false);
	else
		Controls.IdeologyImage1:SetTexture(g_IdeologyBackgrounds[0]);
	    Controls.HasIdeology:SetHide(true);
		Controls.IdeologyGenericHeader:SetHide(false);
		Controls.IdeologyDetails:SetHide(true);
		Controls.PublicOpinionHeader:SetHide(true);
		Controls.PublicOpinion:SetHide(true);
		Controls.PublicOpinionUnhappinessHeader:SetHide(true);
		Controls.PublicOpinionUnhappiness:SetHide(true);
		Controls.SwitchIdeologyButton:SetHide(true);
		
		local bDisablePolicies = Game.IsOption(GameOptionTypes.GAMEOPTION_NO_POLICIES);
		Controls.NoIdeology:SetHide(bDisablePolicies);
	    Controls.DisabledIdeology:SetHide(not bDisablePolicies);
		--CBP
		local iNumPolicies = player:GetNumPolicies(true, true);
		local iNeed = GameDefines.BALANCE_MOD_POLICIES_NEEDED_IDEOLOGY;
		local iNeeded = iNeed - iNumPolicies;
		local strPoliciesLeft = Locale.ConvertTextKey("TXT_KEY_POLICYSCREEN_POLICIES_NEEDED_UNTIL_IDEOLOGY", iNeeded);
		Controls.PoliciesLeftLabel:SetText(strPoliciesLeft);
		Controls.PoliciesLeft:SetHide(bDisablePolicies);
		--END
	end
end
Events.EventPoliciesDirty.Add( UpdateDisplay );

-------------------------------------------------
-- On Tenet Selected
-------------------------------------------------
function TenetSelect(ideologyButtonNumber)

	if ideologyButtonNumber == nil then
		return;
	end

	local iLevel = math.floor(ideologyButtonNumber / 10);

    local player = Players[Game.GetActivePlayer()];   
    if player == nil then
		return;
	else
		Controls.ChooseTenet:SetHide(false);
		Controls.BGBlock:SetHide(true);
		
		g_TenetInstanceManager:ResetInstances();
		 
		local availableTenets = {};
		for i,v in ipairs(player:GetAvailableTenets(iLevel)) do
			local tenet = GameInfo.Policies[v];
			if (tenet ~= nil) then
				local entry = g_TenetInstanceManager:GetInstance();
				if (tenet.Help ~= nil) then
					entry.TenetLabel:LocalizeAndSetText(tenet.Help);
				else
					local szName = Locale.ConvertTextKey(tenet.Description);
					entry.TenetLabel:SetText("[COLOR_POSITIVE_TEXT]" .. szName .. "[ENDCOLOR]");
				end
				
				local tbW, tbH = entry.TenetButton:GetSizeVal();
				local tlW, tlH = entry.TenetLabel:GetSizeVal();
				
				local newHeight = tlH + 30;
				
				entry.TenetButton:SetSizeVal(tbW, newHeight);
				entry.Box:SetSizeVal(tbW, newHeight);
				
				entry.TenetButton:ReprocessAnchoring();
				
				entry.TenetButton:RegisterCallback(Mouse.eLClick, function()
					ChooseTenet(v, tenet.Description);
				end);
			end
		end
    end
end
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnCancel( )
	Controls.ChooseTenet:SetHide(true);
	Controls.BGBlock:SetHide(false);	
end
Controls.TenetCancelButton:RegisterCallback( Mouse.eLClick, OnCancel );
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnClose()
    UIManager:DequeuePopup( ContextPtr );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose );

----------------------------------------------------------------
----------------------------------------------------------------
function OnPolicyInfo( bIsChecked )
	local bUpdateScreen = false;
	
	if (bIsChecked ~= OptionsManager.GetPolicyInfo()) then
		bUpdateScreen = true;
	end
	
    OptionsManager.SetPolicyInfo_Cached( bIsChecked );
    OptionsManager.CommitGameOptions();
    
    if (bUpdateScreen) then
		Events.EventPoliciesDirty();
	end
end
Controls.PolicyInfo:RegisterCheckHandler( OnPolicyInfo );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    ----------------------------------------------------------------        
    -- Key Down Processing
    ----------------------------------------------------------------        
    if uiMsg == KeyEvents.KeyDown then
        if (wParam == Keys.VK_RETURN or wParam == Keys.VK_ESCAPE) then
			if(Controls.PolicyConfirm:IsHidden() and Controls.TenetConfirm:IsHidden() and Controls.ChooseTenet:IsHidden() and Controls.ChangeIdeologyConfirm:IsHidden())then
	            OnClose();
	        else
				if(Controls.TenetConfirm:IsHidden()) then
					Controls.ChooseTenet:SetHide(true);
					Controls.PolicyConfirm:SetHide(true);
					Controls.BGBlock:SetHide(false);
				
				else
					Controls.TenetConfirm:SetHide(true);
				end
			end
			return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );

function GetPipe(branchType)
	local controlTable = nil;
	-- decide which panel it goes on
	if branchType == "POLICY_BRANCH_LIBERTY" then
		controlTable = g_LibertyPipeManager:GetInstance();
	elseif branchType == "POLICY_BRANCH_TRADITION" then
		controlTable = g_TraditionPipeManager:GetInstance();
	elseif branchType == "POLICY_BRANCH_HONOR" then
		controlTable = g_HonorPipeManager:GetInstance();
	elseif branchType == "POLICY_BRANCH_PIETY" then
		controlTable = g_PietyPipeManager:GetInstance();
	elseif branchType == "POLICY_BRANCH_AESTHETICS" then
		controlTable = g_AestheticsPipeManager:GetInstance();
	elseif branchType == "POLICY_BRANCH_PATRONAGE" then
		controlTable = g_PatronagePipeManager:GetInstance();
	elseif branchType == "POLICY_BRANCH_COMMERCE" then
		controlTable = g_CommercePipeManager:GetInstance();
	elseif branchType == "POLICY_BRANCH_EXPLORATION" then
		controlTable = g_ExplorationPipeManager:GetInstance();
	elseif branchType == "POLICY_BRANCH_RATIONALISM" then
		controlTable = g_RationalismPipeManager:GetInstance();
	end
	return controlTable;
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function Init()

	local bDisablePolicies = Game.IsOption(GameOptionTypes.GAMEOPTION_NO_POLICIES);
	
	Controls.LabelPoliciesDisabled:SetHide(not bDisablePolicies);
	Controls.InfoStack:SetHide(bDisablePolicies);
	Controls.InfoStack2:SetHide(bDisablePolicies);
	
	-- Activate the Branch buttons
	local i = 0;
	local policyBranchInfo = GameInfo.PolicyBranchTypes[i];
	while policyBranchInfo ~= nil and not policyBranchInfo.PurchaseByLevel do
		local buttonName = "BranchButton"..tostring( i );
		local thisButton = Controls[buttonName];
		thisButton:SetVoid1( i ); -- indicates type
		thisButton:RegisterCallback( Mouse.eLClick, PolicyBranchSelected );
		i = i + 1;
		policyBranchInfo = GameInfo.PolicyBranchTypes[i];
	end

	-- add the pipes
	local policyPipes = {};
	for row in GameInfo.Policies() do
		policyPipes[row.Type] = 
		{
			upConnectionLeft = false;
			upConnectionRight = false;
			upConnectionCenter = false;
			upConnectionType = 0;
			downConnectionLeft = false;
			downConnectionRight = false;
			downConnectionCenter = false;
			downConnectionType = 0;
			yOffset = 0;
			policyType = row.Type;
		};
	end
	
	local cnxCenter = 1
	local cnxLeft = 2
	local cnxRight = 4

	-- Figure out which top and bottom adapters we need
	for row in GameInfo.Policy_PrereqPolicies() do
		local prereq = GameInfo.Policies[row.PrereqPolicy];
		local policy = GameInfo.Policies[row.PolicyType];
		if policy and prereq then
			if policy.GridX < prereq.GridX then
				policyPipes[policy.Type].upConnectionRight = true;
				policyPipes[prereq.Type].downConnectionLeft = true;
			elseif policy.GridX > prereq.GridX then
				policyPipes[policy.Type].upConnectionLeft = true;
				policyPipes[prereq.Type].downConnectionRight = true;
			else -- policy.GridX == prereq.GridX
				policyPipes[policy.Type].upConnectionCenter = true;
				policyPipes[prereq.Type].downConnectionCenter = true;
			end
			local yOffset = (policy.GridY - prereq.GridY) - 1;
			if yOffset > policyPipes[prereq.Type].yOffset then
				policyPipes[prereq.Type].yOffset = yOffset;
			end
		end
	end

	for pipeIndex, thisPipe in pairs(policyPipes) do
		if thisPipe.upConnectionLeft then
			thisPipe.upConnectionType = thisPipe.upConnectionType + cnxLeft;
		end 
		if thisPipe.upConnectionRight then
			thisPipe.upConnectionType = thisPipe.upConnectionType + cnxRight;
		end 
		if thisPipe.upConnectionCenter then
			thisPipe.upConnectionType = thisPipe.upConnectionType + cnxCenter;
		end 
		if thisPipe.downConnectionLeft then
			thisPipe.downConnectionType = thisPipe.downConnectionType + cnxLeft;
		end 
		if thisPipe.downConnectionRight then
			thisPipe.downConnectionType = thisPipe.downConnectionType + cnxRight;
		end 
		if thisPipe.downConnectionCenter then
			thisPipe.downConnectionType = thisPipe.downConnectionType + cnxCenter;
		end 
	end

	-- three passes down, up, connection
	-- connection
	for row in GameInfo.Policy_PrereqPolicies() do
		local prereq = GameInfo.Policies[row.PrereqPolicy];
		local policy = GameInfo.Policies[row.PolicyType];
		if policy and prereq then
		
			local thisPipe = policyPipes[row.PrereqPolicy];
		
			if policy.GridY - prereq.GridY > 1 or policy.GridY - prereq.GridY < -1 then
				local xOffset = (prereq.GridX-1)*g_PolicyPipeXOffset + 30;
				local pipe = GetPipe(policy.PolicyBranchType);
				pipe.ConnectorImage:SetOffsetVal( xOffset, (prereq.GridY-1)*g_PolicyPipeYOffset + 58 );
				pipe.ConnectorImage:SetTexture(vTexture);
				local size = { x = 19; y = g_PolicyPipeYOffset*(policy.GridY - prereq.GridY - 1); };
				pipe.ConnectorImage:SetSize(size);
			end
			
			if policy.GridX - prereq.GridX == 1 then
				local xOffset = (prereq.GridX-1)*g_PolicyPipeXOffset + 30;
				local pipe = GetPipe(policy.PolicyBranchType);
				pipe.ConnectorImage:SetOffsetVal( xOffset + 16, (prereq.GridY-1 + thisPipe.yOffset)*g_PolicyPipeYOffset + 58 );
				pipe.ConnectorImage:SetTexture(hTexture);
				local size = { x = 19; y = 19; };
				pipe.ConnectorImage:SetSize(size);
			end
			if policy.GridX - prereq.GridX == 2 then
				local xOffset = (prereq.GridX-1)*g_PolicyPipeXOffset + 30;
				local pipe = GetPipe(policy.PolicyBranchType);
				pipe.ConnectorImage:SetOffsetVal( xOffset + 16, (prereq.GridY-1 + thisPipe.yOffset)*g_PolicyPipeYOffset + 58 );
				pipe.ConnectorImage:SetTexture(hTexture);
				local size = { x = 40; y = 19; };
				pipe.ConnectorImage:SetSize(size);
			end
			if policy.GridX - prereq.GridX == -2 then
				local xOffset = (policy.GridX-1)*g_PolicyPipeXOffset + 30;
				local pipe = GetPipe(policy.PolicyBranchType);
				pipe.ConnectorImage:SetOffsetVal( xOffset + 16, (prereq.GridY-1 + thisPipe.yOffset)*g_PolicyPipeYOffset + 58 );
				pipe.ConnectorImage:SetTexture(hTexture);
				local size = { x = 40; y = 19; };
				pipe.ConnectorImage:SetSize(size);
			end
			if policy.GridX - prereq.GridX == -1 then
				local xOffset = (policy.GridX-1)*g_PolicyPipeXOffset + 30;
				local pipe = GetPipe(policy.PolicyBranchType);
				pipe.ConnectorImage:SetOffsetVal( xOffset + 16, (prereq.GridY-1 + thisPipe.yOffset)*g_PolicyPipeYOffset + 58 );
				pipe.ConnectorImage:SetTexture(hTexture);
				local size = { x = 20; y = 19; };
				pipe.ConnectorImage:SetSize(size);
			end
			
		end
	end
	
	-- Down	
	for pipeIndex, thisPipe in pairs(policyPipes) do
		local policy = GameInfo.Policies[thisPipe.policyType];
		local xOffset = (policy.GridX-1)*g_PolicyPipeXOffset + 30;
		if thisPipe.downConnectionType >= 1 then
			
			local startPipe = GetPipe(policy.PolicyBranchType);
			startPipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1)*g_PolicyPipeYOffset + 48 );
			startPipe.ConnectorImage:SetTexture(vTexture);
			
			local pipe = GetPipe(policy.PolicyBranchType);			
			if thisPipe.downConnectionType == 1 then
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1)*g_PolicyPipeYOffset + 58 );
				pipe.ConnectorImage:SetTexture(vTexture);
			elseif thisPipe.downConnectionType == 2 then
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1 + thisPipe.yOffset)*g_PolicyPipeYOffset + 58 );
				pipe.ConnectorImage:SetTexture(bottomRightTexture);
			elseif thisPipe.downConnectionType == 3 then
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1)*g_PolicyPipeYOffset + 58 );
				pipe.ConnectorImage:SetTexture(vTexture);
				pipe = GetPipe(policy.PolicyBranchType);			
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1 + thisPipe.yOffset)*g_PolicyPipeYOffset + 58 );
				pipe.ConnectorImage:SetTexture(bottomRightTexture);
			elseif thisPipe.downConnectionType == 4 then
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1)*g_PolicyPipeYOffset + 58 );
				pipe.ConnectorImage:SetTexture(bottomLeftTexture);
			elseif thisPipe.downConnectionType == 5 then
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1)*g_PolicyPipeYOffset + 58 );
				pipe.ConnectorImage:SetTexture(vTexture);
				pipe = GetPipe(policy.PolicyBranchType);			
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1 + thisPipe.yOffset)*g_PolicyPipeYOffset + 58 );
				pipe.ConnectorImage:SetTexture(bottomLeftTexture);
			elseif thisPipe.downConnectionType == 6 then
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1 + thisPipe.yOffset)*g_PolicyPipeYOffset + 58 );
				pipe.ConnectorImage:SetTexture(bottomRightTexture);
				pipe = GetPipe(policy.PolicyBranchType);		
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1 + thisPipe.yOffset)*g_PolicyPipeYOffset + 58 );
				pipe.ConnectorImage:SetTexture(bottomLeftTexture);
			else
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1)*g_PolicyPipeYOffset + 58 );
				pipe.ConnectorImage:SetTexture(vTexture);
				pipe = GetPipe(policy.PolicyBranchType);		
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1 + thisPipe.yOffset)*g_PolicyPipeYOffset + 58 );
				pipe.ConnectorImage:SetTexture(bottomRightTexture);
				pipe = GetPipe(policy.PolicyBranchType);
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1 + thisPipe.yOffset)*g_PolicyPipeYOffset + 58 );
				pipe.ConnectorImage:SetTexture(bottomLeftTexture);
			end
		end
	end

	-- Up
	for pipeIndex, thisPipe in pairs(policyPipes) do
		local policy = GameInfo.Policies[thisPipe.policyType];
		local xOffset = (policy.GridX-1)*g_PolicyPipeXOffset + 30;
		
		if thisPipe.upConnectionType >= 1 then
			
			local startPipe = GetPipe(policy.PolicyBranchType);
			startPipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1)*g_PolicyPipeYOffset + 0 );
			startPipe.ConnectorImage:SetTexture(vTexture);
			
			local pipe = GetPipe(policy.PolicyBranchType);			
			if thisPipe.upConnectionType == 1 then
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1)*g_PolicyPipeYOffset - 10 );
				pipe.ConnectorImage:SetTexture(vTexture);
			elseif thisPipe.upConnectionType == 2 then
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1)*g_PolicyPipeYOffset - 10 );
				pipe.ConnectorImage:SetTexture(topRightTexture);
			elseif thisPipe.upConnectionType == 3 then
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1)*g_PolicyPipeYOffset - 10 );
				pipe.ConnectorImage:SetTexture(vTexture);
				pipe = GetPipe(policy.PolicyBranchType);			
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1)*g_PolicyPipeYOffset - 10 );
				pipe.ConnectorImage:SetTexture(topRightTexture);
			elseif thisPipe.upConnectionType == 4 then
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1)*g_PolicyPipeYOffset - 10 );
				pipe.ConnectorImage:SetTexture(topLeftTexture);
			elseif thisPipe.upConnectionType == 5 then
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1)*g_PolicyPipeYOffset - 10 );
				pipe.ConnectorImage:SetTexture(vTexture);
				pipe = GetPipe(policy.PolicyBranchType);			
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1)*g_PolicyPipeYOffset - 10 );
				pipe.ConnectorImage:SetTexture(topLeftTexture);
			elseif thisPipe.upConnectionType == 6 then
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1)*g_PolicyPipeYOffset - 10 );
				pipe.ConnectorImage:SetTexture(topRightTexture);
				pipe = GetPipe(policy.PolicyBranchType);		
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1)*g_PolicyPipeYOffset - 10 );
				pipe.ConnectorImage:SetTexture(topLeftTexture);
			else
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1)*g_PolicyPipeYOffset - 10 );
				pipe.ConnectorImage:SetTexture(vTexture);
				pipe = GetPipe(policy.PolicyBranchType);		
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1)*g_PolicyPipeYOffset - 10 );
				pipe.ConnectorImage:SetTexture(topRightTexture);
				pipe = GetPipe(policy.PolicyBranchType);
				pipe.ConnectorImage:SetOffsetVal( xOffset, (policy.GridY-1)*g_PolicyPipeYOffset - 10 );
				pipe.ConnectorImage:SetTexture(topLeftTexture);
			end
		end
	end

	-- Add Policy buttons
	i = 0;
	policyInfo = GameInfo.Policies[i];
	while policyInfo ~= nil do
		
		local iBranch = policyInfo.PolicyBranchType;
		
		-- If this is nil it means the Policy is a freebie handed out with the Branch, so don't display it
		if (iBranch ~= nil) then
			
			local controlTable = nil;
			
			-- decide which panel it goes on
			if iBranch == "POLICY_BRANCH_LIBERTY" then
				controlTable = g_LibertyInstanceManager:GetInstance();
			elseif iBranch == "POLICY_BRANCH_TRADITION" then
				controlTable = g_TraditionInstanceManager:GetInstance();
			elseif iBranch == "POLICY_BRANCH_HONOR" then
				controlTable = g_HonorInstanceManager:GetInstance();
			elseif iBranch == "POLICY_BRANCH_PIETY" then
				controlTable = g_PietyInstanceManager:GetInstance();
			elseif iBranch == "POLICY_BRANCH_AESTHETICS" then
				controlTable = g_AestheticsInstanceManager:GetInstance();
			elseif iBranch == "POLICY_BRANCH_PATRONAGE" then
				controlTable = g_PatronageInstanceManager:GetInstance();
			elseif iBranch == "POLICY_BRANCH_COMMERCE" then
				controlTable = g_CommerceInstanceManager:GetInstance();
			elseif iBranch == "POLICY_BRANCH_EXPLORATION" then
				controlTable = g_ExplorationInstanceManager:GetInstance();
			elseif iBranch == "POLICY_BRANCH_RATIONALISM" then
				controlTable = g_RationalismInstanceManager:GetInstance();
			end
			
			if (controlTable ~= nil) then
				IconHookup( policyInfo.PortraitIndex, 64, policyInfo.IconAtlas, controlTable.PolicyImage );

				-- this math should match Russ's mocked up layout
				controlTable.PolicyIcon:SetOffsetVal((policyInfo.GridX-1)*g_PolicyXOffset+16,(policyInfo.GridY-1)*g_PolicyYOffset+12);
				controlTable.PolicyIcon:SetVoid1( i ); -- indicates which policy
				controlTable.PolicyIcon:RegisterCallback( Mouse.eLClick, PolicySelected );
			
				-- store this away for later
				policyIcons[i] = controlTable;
			end
		end
		
		i = i + 1;
		policyInfo = GameInfo.Policies[i];
	end
	
end

function OnYes( )
	Controls.PolicyConfirm:SetHide(true);
	Controls.BGBlock:SetHide(false);
	
	Network.SendUpdatePolicies(m_gPolicyID, m_gAdoptingPolicy, true);
	Events.AudioPlay2DSound("AS2D_INTERFACE_POLICY");		
	--Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DECLARES_WAR, g_iAIPlayer, 0, 0 );
end
Controls.Yes:RegisterCallback( Mouse.eLClick, OnYes );

function OnNo( )
	Controls.PolicyConfirm:SetHide(true);
	Controls.BGBlock:SetHide(false);
end
Controls.No:RegisterCallback( Mouse.eLClick, OnNo );


function ChooseTenet(tenet, name)
	g_SelectedTenet = tenet;
	Controls.LabelConfirmTenet:LocalizeAndSetText("TXT_KEY_POLICYSCREEN_CONFIRM_TENET", name);
	Controls.TenetConfirm:SetHide(false);
end

					
function OnTenetConfirmYes( )

	Controls.TenetConfirm:SetHide(true);
	Controls.ChooseTenet:SetHide(true);
	Controls.BGBlock:SetHide(false);
	
	Network.SendUpdatePolicies(g_SelectedTenet, true, true);
	Events.AudioPlay2DSound("AS2D_INTERFACE_POLICY");		
end
Controls.TenetConfirmYes:RegisterCallback( Mouse.eLClick, OnTenetConfirmYes );

function OnTenetConfirmNo( )
	Controls.TenetConfirm:SetHide(true);
	Controls.BGBlock:SetHide(false);
end
Controls.TenetConfirmNo:RegisterCallback( Mouse.eLClick, OnTenetConfirmNo );


function ChooseChangeIdeology()
    local player = Players[Game.GetActivePlayer()];
	local iAnarchyTurns = GameDefines["SWITCH_POLICY_BRANCHES_ANARCHY_TURNS"];
	local eCurrentIdeology = player:GetLateGamePolicyTree();
	local iCurrentIdeologyTenets = player:GetNumPoliciesInBranch(eCurrentIdeology);
	local iPreferredIdeologyTenets = iCurrentIdeologyTenets - GameDefines["SWITCH_POLICY_BRANCHES_TENETS_LOST"];
	if (iPreferredIdeologyTenets < 0) then
		iPreferredIdeologyTenets = 0;
	end
	local iUnhappiness = player:GetPublicOpinionUnhappiness();
	local strCurrentIdeology = GameInfo.PolicyBranchTypes[eCurrentIdeology].Description;	
	local ePreferredIdeology = player:GetPublicOpinionPreferredIdeology();
	local strPreferredIdeology = GameInfo.PolicyBranchTypes[ePreferredIdeology].Description;
	Controls.LabelConfirmChangeIdeology:LocalizeAndSetText("TXT_KEY_POLICYSCREEN_CONFIRM_CHANGE_IDEOLOGY", iAnarchyTurns, iCurrentIdeologyTenets, strCurrentIdeology, iPreferredIdeologyTenets, strPreferredIdeology, iUnhappiness);
	Controls.ChangeIdeologyConfirm:SetHide(false);
end

function OnChangeIdeologyConfirmYes( )

	Controls.ChangeIdeologyConfirm:SetHide(true);
	Network.SendChangeIdeology();
	Events.AudioPlay2DSound("AS2D_INTERFACE_POLICY");	
end
Controls.ChangeIdeologyConfirmYes:RegisterCallback( Mouse.eLClick, OnChangeIdeologyConfirmYes );

function OnChangeIdeologyConfirmNo( )
	Controls.ChangeIdeologyConfirm:SetHide(true);
end
Controls.ChangeIdeologyConfirmNo:RegisterCallback( Mouse.eLClick, OnChangeIdeologyConfirmNo );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )
    if( not bInitState ) then
        Controls.PolicyInfo:SetCheck( OptionsManager.GetPolicyInfo() );
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
        else
            UI.decTurnTimerSemaphore();
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_CHOOSEPOLICY, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnActivePlayerChanged()
	if (not Controls.PolicyConfirm:IsHidden() or not Controls.TenetConfirm:IsHidden() or not Controls.ChangeIdeologyConfirm:IsHidden()) then
		Controls.TenetConfirm:SetHide(true);
		Controls.ChangeIdeologyConfirm:SetHide(true);
		Controls.PolicyConfirm:SetHide(true);
    	Controls.BGBlock:SetHide(false);
	end
	OnClose();
end
Events.GameplaySetActivePlayer.Add(OnActivePlayerChanged);

Init();

-- Register tabbing behavior and assign global TabSelect routine.
TabSelect = RegisterTabBehavior(g_Tabs, g_Tabs["SocialPolicies"]);

Controls.ToIdeologyTab:RegisterCallback(Mouse.eLClick, function() TabSelect(g_Tabs["Ideologies"]) end);