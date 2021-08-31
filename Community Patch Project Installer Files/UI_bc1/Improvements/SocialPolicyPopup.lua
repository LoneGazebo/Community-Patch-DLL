-------------------------------------------------
-- SocialPolicy Chooser Popup
-------------------------------------------------

include( "IconSupport" );
local IconHookup = IconHookup
local CivIconHookup = CivIconHookup
local GameInfo = EUI and EUI.GameInfoCache or GameInfo

include( "InstanceManager" );
local InstanceManager = InstanceManager

local ipairs = ipairs
local math_ceil = math.ceil
local math_huge = math.huge
local pairs = pairs
local print = print
local tostring = tostring

local ButtonPopupTypes = ButtonPopupTypes
local ContextPtr = ContextPtr
local Controls = Controls
local Events = Events
local Game = Game
local GameDefines = GameDefines
local GameInfoTypes = GameInfoTypes
local GameOptionTypes = GameOptionTypes
local KeyEvents = KeyEvents
local Keys = Keys
local Locale = Locale
local Mouse = Mouse
local Network = Network
local OptionsManager = OptionsManager
local Players = Players
local PopupPriority = PopupPriority
local PublicOpinionTypes = PublicOpinionTypes
local Teams = Teams
local UI = UI
local UIManager = UIManager

local bnw_mode = Game.GetActiveLeague ~= nil

local m_PopupInfo

local g_pipeManagers = {
	POLICY_BRANCH_LIBERTY = InstanceManager:new( "ConnectorPipe", "ConnectorImage", Controls.LibertyPanel ),
	POLICY_BRANCH_TRADITION = InstanceManager:new( "ConnectorPipe", "ConnectorImage", Controls.TraditionPanel ),
	POLICY_BRANCH_HONOR = InstanceManager:new( "ConnectorPipe", "ConnectorImage", Controls.HonorPanel ),
	POLICY_BRANCH_PIETY = InstanceManager:new( "ConnectorPipe", "ConnectorImage", Controls.PietyPanel ),
	POLICY_BRANCH_AESTHETICS = InstanceManager:new ( "ConnectorPipe", "ConnectorImage", Controls.AestheticsPanel ),
	POLICY_BRANCH_PATRONAGE = InstanceManager:new( "ConnectorPipe", "ConnectorImage", Controls.PatronagePanel ),
	POLICY_BRANCH_COMMERCE = InstanceManager:new( "ConnectorPipe", "ConnectorImage", Controls.CommercePanel ),
	POLICY_BRANCH_EXPLORATION = InstanceManager:new ( "ConnectorPipe", "ConnectorImage", Controls.ExplorationPanel ),
	POLICY_BRANCH_RATIONALISM = InstanceManager:new( "ConnectorPipe", "ConnectorImage", Controls.RationalismPanel ),
	POLICY_BRANCH_FREEDOM = InstanceManager:new( "ConnectorPipe", "ConnectorImage", Controls.FreedomPanel ),
	POLICY_BRANCH_ORDER = InstanceManager:new( "ConnectorPipe", "ConnectorImage", Controls.OrderPanel ),
	POLICY_BRANCH_AUTOCRACY = InstanceManager:new( "ConnectorPipe", "ConnectorImage", Controls.AutocracyPanel ),
}
local g_instanceManagers = {
	POLICY_BRANCH_LIBERTY = InstanceManager:new( "PolicyButton", "PolicyIcon", Controls.LibertyPanel ),
	POLICY_BRANCH_TRADITION = InstanceManager:new( "PolicyButton", "PolicyIcon", Controls.TraditionPanel ),
	POLICY_BRANCH_HONOR = InstanceManager:new( "PolicyButton", "PolicyIcon", Controls.HonorPanel ),
	POLICY_BRANCH_PIETY = InstanceManager:new( "PolicyButton", "PolicyIcon", Controls.PietyPanel ),
	POLICY_BRANCH_AESTHETICS = InstanceManager:new ( "PolicyButton", "PolicyIcon", Controls.AestheticsPanel ),
	POLICY_BRANCH_PATRONAGE = InstanceManager:new( "PolicyButton", "PolicyIcon", Controls.PatronagePanel ),
	POLICY_BRANCH_COMMERCE = InstanceManager:new( "PolicyButton", "PolicyIcon", Controls.CommercePanel ),
	POLICY_BRANCH_EXPLORATION = InstanceManager:new( "PolicyButton", "PolicyIcon", Controls.ExplorationPanel ),
	POLICY_BRANCH_RATIONALISM = InstanceManager:new( "PolicyButton", "PolicyIcon", Controls.RationalismPanel ),
	POLICY_BRANCH_FREEDOM = InstanceManager:new( "PolicyButton", "PolicyIcon", Controls.FreedomPanel ),
	POLICY_BRANCH_ORDER = InstanceManager:new( "PolicyButton", "PolicyIcon", Controls.OrderPanel ),
	POLICY_BRANCH_AUTOCRACY = InstanceManager:new( "PolicyButton", "PolicyIcon", Controls.AutocracyPanel ),
}
local g_TenetInstanceManager = InstanceManager:new( "TenetChoice", "TenetButton", Controls.TenetStack )

local fullColor = {x = 1, y = 1, z = 1, w = 1};
local fadeColor = {x = 1, y = 1, z = 1, w = 0};
local fadeColorRV = {x = 1, y = 1, z = 1, w = 0.2};

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
local g_SelectedTenet
local TabSelect
local g_Tabs = {
	SocialPolicies = {
		Button = Controls.TabButtonSocialPolicies,
		Panel = Controls.SocialPolicyPane,
		SelectHighlight = Controls.TabButtonSocialPoliciesHL,
	},

	Ideologies = {
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

	--print("Clicked on Policy: " .. tostring(policyIndex));

	if policyIndex == -1 then
		return;
	end
	local player = Players[Game.GetActivePlayer()];
	if player == nil then
		return;
	end

	local bHasPolicy = player:HasPolicy(policyIndex);
	local bCanAdoptPolicy = player:CanAdoptPolicy(policyIndex);

	--print("bHasPolicy: " .. tostring(bHasPolicy));
	--print("bCanAdoptPolicy: " .. tostring(bCanAdoptPolicy));
	--print("Policy Blocked: " .. tostring(player:IsPolicyBlocked(policyIndex)));

	local bPolicyBlocked = false;

	-- If we can get this, OR we already have it, see if we can unblock it first
	if bHasPolicy or bCanAdoptPolicy then

		-- Policy blocked off right now? If so, try to activate
		if player:IsPolicyBlocked(policyIndex) then

			bPolicyBlocked = true;

			local strPolicyBranch = GameInfo.Policies[policyIndex].PolicyBranchType;
			local iPolicyBranch = GameInfoTypes[strPolicyBranch];

			--print("Policy Branch: " .. tostring(iPolicyBranch));

			local popupInfo = {
				Type = ButtonPopupTypes.BUTTONPOPUP_CONFIRM_POLICY_BRANCH_SWITCH,
				Data1 = iPolicyBranch;
			}
			Events.SerialEventGameMessagePopup(popupInfo);

		end
	end

	-- Can adopt Policy right now - don't try this if we're going to unblock the Policy instead
	if bCanAdoptPolicy and not bPolicyBlocked then
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
	--print("PolicyBranch Blocked: " .. tostring(player:IsPolicyBranchBlocked(policyBranchIndex)));

	local bUnblockingPolicyBranch = false;

	-- If we can get this, OR we already have it, see if we can unblock it first
	if bHasPolicyBranch or bCanAdoptPolicyBranch then

		-- Policy Branch blocked off right now? If so, try to activate
		if player:IsPolicyBranchBlocked(policyBranchIndex) then

			bUnblockingPolicyBranch = true;

			local popupInfo = {
				Type = ButtonPopupTypes.BUTTONPOPUP_CONFIRM_POLICY_BRANCH_SWITCH,
				Data1 = policyBranchIndex;
			}
			Events.SerialEventGameMessagePopup(popupInfo);
		end
	end

	-- Can adopt Policy Branch right now - don't try this if we're going to unblock the Policy Branch instead
	if bCanAdoptPolicyBranch and not bUnblockingPolicyBranch then
		m_gPolicyID = policyBranchIndex;
		m_gAdoptingPolicy = false;
		Controls.PolicyConfirm:SetHide(false);
		Controls.BGBlock:SetHide(true);
	end
end

-------------------------------------------------
-------------------------------------------------
function OnPopupMessage(popupInfo)

	if popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_CHOOSEPOLICY then
		return;
	end

	m_PopupInfo = popupInfo;

	UpdateDisplay();

	if bnw_mode then
		local player = Players[Game.GetActivePlayer()]
		if m_PopupInfo.Data2 == 2 or (player and player:GetNumFreeTenets() > 0) then
			TabSelect(g_Tabs["Ideologies"]);
		else
			TabSelect(g_Tabs["SocialPolicies"]);
		end
	end

	if m_PopupInfo.Data1 == 1 then
		if ContextPtr:IsHidden() == false then
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

	if not player then return end

	local pTeam = Teams[player:GetTeam()];


	local playerHas1City = player:GetNumCities() > 0;


	local bShowAll = OptionsManager.GetPolicyInfo();

	Controls.NextCost:LocalizeAndSetText( "TXT_KEY_NEXT_POLICY_COST_LABEL", player:GetNextPolicyCost() )
	Controls.CurrentCultureLabel:LocalizeAndSetText( "TXT_KEY_CURRENT_CULTURE_LABEL", player:GetJONSCulture() )
	Controls.CulturePerTurnLabel:LocalizeAndSetText( "TXT_KEY_CULTURE_PER_TURN_LABEL", player:GetTotalJONSCulturePerTurn() )

	local cultureNeeded = player:GetNextPolicyCost() - player:GetJONSCulture()
	local culturePerTurn = player:GetTotalJONSCulturePerTurn()
	Controls.NextPolicyTurnLabel:LocalizeAndSetText( "TXT_KEY_NEXT_POLICY_TURN_LABEL", cultureNeeded <= 0 and 0 or ( culturePerTurn <= 0 and "?" or math_ceil( cultureNeeded / culturePerTurn ) ) )

	-- Player Title
	local dominantBranch = GameInfo.PolicyBranchTypes[ player:GetDominantPolicyBranchForTitle() ]
	if dominantBranch then
		Controls.PlayerTitleLabel:SetHide(false);
		Controls.PlayerTitleLabel:LocalizeAndSetText( dominantBranch.Title, player:GetNameKey(), player:GetCivilizationShortDescriptionKey() );
	else
		Controls.PlayerTitleLabel:SetHide(true);
	end

	-- Free Policies
	local iNumFreePolicies = player:GetNumFreePolicies();
	if iNumFreePolicies > 0 then
		Controls.FreePoliciesLabel:LocalizeAndSetText( "TXT_KEY_FREE_POLICIES_LABEL", iNumFreePolicies );
		Controls.FreePoliciesLabel:SetHide( false );
	else
		Controls.FreePoliciesLabel:SetHide( true );
	end


	local justLooking = player:GetJONSCulture() < player:GetNextPolicyCost()

	-- Adjust Policy Branches
	local policyBranchID = 0;
	local policyBranchInfo = GameInfo.PolicyBranchTypes[ policyBranchID ];
	while policyBranchInfo and not (bnw_mode and policyBranchInfo.PurchaseByLevel) do

		local thisButton = Controls[ "BranchButton"..policyBranchID ]
		local thisBack = Controls[ "BranchBack"..policyBranchID ]
		local thisDisabledBox = Controls[ "DisabledBox"..policyBranchID ]
		local thisLockedBox = Controls[ "LockedBox"..policyBranchID ]
		local thisImageMask = Controls[ "ImageMask"..policyBranchID ]
		local thisDisabledMask = Controls[ "DisabledMask"..policyBranchID ]
		local thisLock = Controls[ "Lock"..policyBranchID ]

		local strToolTip = Locale.ConvertTextKey(policyBranchInfo.Help)

		-- Era Prereq
		local iEraPrereq = GameInfoTypes[ policyBranchInfo.EraPrereq ]
		local bEraLock = iEraPrereq and pTeam:GetCurrentEra() < iEraPrereq



		-- Branch is not yet unlocked
		if not player:IsPolicyBranchUnlocked( policyBranchID ) then

			-- Cannot adopt this branch right now
			if bnw_mode and policyBranchInfo.LockedWithoutReligion and Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION) then
				strToolTip = strToolTip .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_POLICY_BRANCH_CANNOT_UNLOCK_RELIGION");

			elseif not player:CanUnlockPolicyBranch( policyBranchID ) then

				strToolTip = strToolTip .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_POLICY_BRANCH_CANNOT_UNLOCK");

				-- Not in prereq Era
				if bEraLock then
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

			if not playerHas1City then
				thisButton:SetDisabled(true);
			end

			thisBack:SetColor( fadeColor );
			thisLockedBox:SetHide(false);

			thisImageMask:SetHide(true);
			thisDisabledMask:SetHide(false);

		-- Branch is unlocked, but blocked by another branch
		elseif player:IsPolicyBranchBlocked( policyBranchID ) then
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
		if bEraLock then
			thisDisabledBox:SetHide(false);
			thisLockedBox:SetHide(true);
		else
			thisDisabledBox:SetHide(true);
		end

		if bShowAll then
			thisDisabledBox:SetHide(true);
			thisLockedBox:SetHide(true);
		end

		policyBranchID = policyBranchID + 1;
		policyBranchInfo = GameInfo.PolicyBranchTypes[ policyBranchID ];
	end

	-- Adjust Policy buttons
	local policyID = 0;
	local policyInfo = GameInfo.Policies[policyID];
	while policyInfo do

		local iBranch = policyInfo.PolicyBranchType;

		-- If this is nil it means the Policy is a freebie handed out with the Branch, so don't display it
		if iBranch and not (bnw_mode and GameInfo.PolicyBranchTypes[iBranch].PurchaseByLevel) then

			local thisPolicyIcon = policyIcons[policyID];

			-- Tooltip
			local strTooltip = Locale.ConvertTextKey( policyInfo.Help );

			-- Player already has Policy
			if player:HasPolicy( policyID ) then
				--thisPolicyIcon.Lock:SetTexture( checkTexture );
				--thisPolicyIcon.Lock:SetHide( true );
				thisPolicyIcon.MouseOverContainer:SetHide( true );
				thisPolicyIcon.PolicyIcon:SetDisabled( true );
				--thisPolicyIcon.PolicyIcon:SetVoid1( -1 );
				thisPolicyIcon.PolicyImage:SetColor( fullColor );
				IconHookup( policyInfo.PortraitIndex, 64, policyInfo.IconAtlasAchieved, thisPolicyIcon.PolicyImage );
			elseif not playerHas1City then
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
			elseif player:CanAdoptPolicy( policyID ) then
				--thisPolicyIcon.Lock:SetHide( true );
				thisPolicyIcon.MouseOverContainer:SetHide( false );
				thisPolicyIcon.PolicyIcon:SetDisabled( false );
				if justLooking then
					--thisPolicyIcon.PolicyIcon:SetVoid1( -1 );
					thisPolicyIcon.PolicyImage:SetColor( fullColor );
				else
					thisPolicyIcon.PolicyIcon:SetVoid1( policyID ); -- indicates policy to be chosen
					thisPolicyIcon.PolicyImage:SetColor( fullColor );
				end
				IconHookup( policyInfo.PortraitIndex, 64, policyInfo.IconAtlas, thisPolicyIcon.PolicyImage );

			-- Policy is unlocked, but we lack culture
			elseif player:CanAdoptPolicy( policyID, true ) then
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
			if player:IsPolicyBlocked( policyID ) then
				thisPolicyIcon.PolicyImage:SetColor( fadeColorRV );
				IconHookup( policyInfo.PortraitIndex, 64, policyInfo.IconAtlas, thisPolicyIcon.PolicyImage );

				-- Update tooltip if we have this Policy
				if player:HasPolicy( policyID ) then
					strTooltip = strTooltip .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_POLICY_BRANCH_BLOCKED");
				end
			end

			thisPolicyIcon.PolicyIcon:SetToolTipString( strTooltip );
		end

		policyID = policyID + 1;
		policyInfo = GameInfo.Policies[ policyID ];
	end

	-- Adjust Ideology
	if bnw_mode then
		CivIconHookup( player:GetID(), 64, Controls.CivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true );
		Controls.InfoStack2:ReprocessAnchoring();

		local ideologyID = player:GetLateGamePolicyTree();
		local upperLevelCount = ideologyID >=0 and math_huge or 0
		local tenetsPerLevel = { 7, 4, 3 }
		for j = 1, 3 do
			local levelCount = 0
			local thisLevelTenets = {}
			local levelTips = ""
			for _,v in ipairs( player:GetAvailableTenets(j) ) do
				local tenet = GameInfo.Policies[v]
				if tenet then
					levelTips = levelTips .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey( tenet.Help or tenet.Description or "" )
				end
			end
			for i = 1, tenetsPerLevel[j] do
				local tenetID = player:GetTenet( ideologyID, j, i )
				thisLevelTenets[i] = tenetID
				local buttonControl = Controls["IdeologyButton"..j..i]
				local labelControl = Controls["IdeologyButtonLabel"..j..i]
				local lockControl = Controls["Lock"..j..i]

				buttonControl:SetDisabled( true )
				buttonControl:ClearCallback( Mouse.eLClick )

				if tenetID >= 0 then
					levelCount = levelCount + 1
					local tenet = GameInfo.Policies[ tenetID ]
					labelControl:LocalizeAndSetText( tenet.Description )
					buttonControl:LocalizeAndSetToolTip( tenet.Help )
					lockControl:SetHide(true)
				else
					local tip
					if upperLevelCount > i and i == levelCount+1 then
						tip = Locale.ConvertTextKey( "TXT_KEY_POLICYSCREEN_ADD_TENET" )
						labelControl:SetText( tip )
						lockControl:SetHide( true )
						if player:GetJONSCulture() >= player:GetNextPolicyCost() or player:GetNumFreePolicies() > 0 or player:GetNumFreeTenets() > 0 then
							buttonControl:RegisterCallback( Mouse.eLClick, function() TenetSelect(j) end)
							buttonControl:SetDisabled( false )
						else
							tip = Locale.ConvertTextKey( "TXT_KEY_POLICY_BRANCH_CANNOT_UNLOCK_CULTURE", player:GetNextPolicyCost() )
						end
					else
						lockControl:SetHide( false )
						labelControl:SetString()
						if upperLevelCount == i then
							tip = "[ICON_LOCKED]" .. Locale.ConvertTextKey( "TXT_KEY_POLICYSCREEN_NEED_L"..(j-1).."_TENETS_TOOLTIP" )
						else
							tip = "[ICON_LOCKED][COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey( "TXT_KEY_POLICYSCREEN_IDEOLOGY_LEVEL"..j ) .. "[ENDCOLOR]"
						end
					end
					buttonControl:SetToolTipString( tip..levelTips )
				end
			end
			Controls["Level"..j.."Tenets"]:LocalizeAndSetText( "TXT_KEY_POLICYSCREEN_IDEOLOGY_L"..j.."TENETS", levelCount )
			upperLevelCount = levelCount
		end

		if ideologyID >= 0 then

			-- Free Tenets
			local iNumFreeTenets = player:GetNumFreeTenets();
			if iNumFreeTenets > 0 then
				Controls.FreeTenetsLabel:LocalizeAndSetText( "TXT_KEY_FREE_TENETS_LABEL", iNumFreeTenets )
				Controls.FreeTenetsLabel:SetHide( false );
			else
				Controls.FreeTenetsLabel:SetHide( true );
			end

			local ideology = GameInfo.PolicyBranchTypes[ideologyID];
			local ideologyName = Locale.Lookup("TXT_KEY_POLICYSCREEN_IDEOLOGY_TITLE", player:GetCivilizationAdjectiveKey(), ideology.Description);
			Controls.IdeologyHeader:SetText(ideologyName);
			Controls.IdeologyImage1:SetTexture(g_IdeologyBackgrounds[ideology.Type]);
			Controls.IdeologyImage2:SetTexture(g_IdeologyBackgrounds[ideology.Type]);


			Controls.TenetsStack:CalculateSize();
			Controls.TenetsStack:ReprocessAnchoring();

			local ideologyTitle = Locale.ToUpper(ideologyName);
			Controls.IdeologyTitle:SetText(ideologyTitle);
			Controls.ChooseTenetTitle:SetText(ideologyTitle);
			Controls.NoIdeology:SetHide(true);
			Controls.DisabledIdeology:SetHide(true);
			Controls.HasIdeology:SetHide(false);

			Controls.IdeologyGenericHeader:SetHide(true);
			Controls.IdeologyDetails:SetHide(false);

			local szOpinionString;
			local iOpinion = player:GetPublicOpinionType();
			if iOpinion == PublicOpinionTypes.PUBLIC_OPINION_DISSIDENTS then
				szOpinionString = Locale.ConvertTextKey("TXT_KEY_CO_PUBLIC_OPINION_DISSIDENTS");
			elseif iOpinion == PublicOpinionTypes.PUBLIC_OPINION_CIVIL_RESISTANCE then
				szOpinionString = Locale.ConvertTextKey("TXT_KEY_CO_PUBLIC_OPINION_CIVIL_RESISTANCE");
			elseif iOpinion == PublicOpinionTypes.PUBLIC_OPINION_REVOLUTIONARY_WAVE then
				szOpinionString = Locale.ConvertTextKey("TXT_KEY_CO_PUBLIC_OPINION_REVOLUTIONARY_WAVE");
			else
				szOpinionString = Locale.ConvertTextKey("TXT_KEY_CO_PUBLIC_OPINION_CONTENT");
			end
			Controls.PublicOpinion:SetText(szOpinionString);
			Controls.PublicOpinion:SetToolTipString(player:GetPublicOpinionTooltip());

			local iUnhappiness = -1 * player:GetPublicOpinionUnhappiness();
			local strPublicOpinionUnhappiness = tostring(0);
			local strChangeIdeologyTooltip = "";
			if iUnhappiness < 0 then
				strPublicOpinionUnhappiness = Locale.ConvertTextKey("TXT_KEY_CO_PUBLIC_OPINION_UNHAPPINESS", iUnhappiness);
				Controls.SwitchIdeologyButton:SetDisabled(false);
				local ePreferredIdeology = player:GetPublicOpinionPreferredIdeology();
				local strPreferredIdeology = tostring((GameInfo.PolicyBranchTypes[ePreferredIdeology] or {}).Description);
				strChangeIdeologyTooltip = Locale.ConvertTextKey("TXT_KEY_POLICYSCREEN_CHANGE_IDEOLOGY_TT", strPreferredIdeology, (-1 * iUnhappiness), 2);

				Controls.SwitchIdeologyButton:RegisterCallback(Mouse.eLClick, ChooseChangeIdeology );
			else
				Controls.SwitchIdeologyButton:SetDisabled( true );
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

		end
	else
		Controls.AnarchyBlock:SetHide( not player:IsAnarchy() );
	end
	Controls.InfoStack:ReprocessAnchoring();
end
Events.EventPoliciesDirty.Add( UpdateDisplay );

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

	if bIsChecked ~= OptionsManager.GetPolicyInfo() then
		bUpdateScreen = true;
	end

	OptionsManager.SetPolicyInfo_Cached( bIsChecked );
	OptionsManager.CommitGameOptions();

	if bUpdateScreen then
		Events.EventPoliciesDirty();
	end
end
Controls.PolicyInfo:RegisterCheckHandler( OnPolicyInfo );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function InputHandler( uiMsg, wParam )
	----------------------------------------------------------------
	-- Key Down Processing
	----------------------------------------------------------------
	if uiMsg == KeyEvents.KeyDown then
		if wParam == Keys.VK_RETURN or wParam == Keys.VK_ESCAPE then
			if bnw_mode then
				if Controls.PolicyConfirm:IsHidden() and Controls.TenetConfirm:IsHidden() and Controls.ChooseTenet:IsHidden() and Controls.ChangeIdeologyConfirm:IsHidden() then
					OnClose();
				elseif Controls.TenetConfirm:IsHidden() then
					Controls.ChooseTenet:SetHide(true);
					Controls.PolicyConfirm:SetHide(true);
					Controls.BGBlock:SetHide(false);
				else
					Controls.TenetConfirm:SetHide(true);
				end
			elseif Controls.PolicyConfirm:IsHidden() then
				OnClose();
			else
				Controls.PolicyConfirm:SetHide(true);
				Controls.BGBlock:SetHide(false);
			end
			return true;
		end
	end
end
ContextPtr:SetInputHandler( InputHandler );

function GetPipe( branchType )
	local pipeManager = g_pipeManagers[ branchType ]
	return pipeManager and pipeManager:GetInstance()
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
	while policyBranchInfo and not (bnw_mode and policyBranchInfo.PurchaseByLevel) do
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

	for _, thisPipe in pairs( policyPipes ) do
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
	for _, thisPipe in pairs( policyPipes ) do
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
	for _, thisPipe in pairs( policyPipes ) do
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
	local policyInfo = GameInfo.Policies[i];
	while policyInfo do

		local instanceManager = g_instanceManagers[ policyInfo.PolicyBranchType ]

		-- If this is nil it means the Policy is a freebie handed out with the Branch, so don't display it
		local controlTable = instanceManager and instanceManager:GetInstance()

		if controlTable then
			IconHookup( policyInfo.PortraitIndex, 64, policyInfo.IconAtlas, controlTable.PolicyImage );

			-- this math should match Russ's mocked up layout
			controlTable.PolicyIcon:SetOffsetVal((policyInfo.GridX-1)*g_PolicyXOffset+16,(policyInfo.GridY-1)*g_PolicyYOffset+12);
			controlTable.PolicyIcon:SetVoid1( i ); -- indicates which policy
			controlTable.PolicyIcon:RegisterCallback( Mouse.eLClick, PolicySelected );

			-- store this away for later
			policyIcons[i] = controlTable;
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

-------------------------------------------------
-- Ideology Selection
-------------------------------------------------
if bnw_mode then
	include( "CommonBehaviors" );

	function TenetSelect(iLevel)

		local player = Players[Game.GetActivePlayer()];
		if player and iLevel then
			Controls.ChooseTenet:SetHide(false);
			Controls.BGBlock:SetHide(true);

			g_TenetInstanceManager:ResetInstances();

			for _,v in ipairs(player:GetAvailableTenets(iLevel)) do
				local tenet = GameInfo.Policies[v];
				if tenet then
					local entry = g_TenetInstanceManager:GetInstance();
					entry.TenetLabel:LocalizeAndSetText(tenet.Help or tenet.Description or "");

					local newHeight = entry.TenetLabel:GetSizeY() + 30;

					entry.TenetButton:SetSizeY( newHeight );
					entry.Box:SetSizeY( newHeight );

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
		if iPreferredIdeologyTenets < 0 then
			iPreferredIdeologyTenets = 0;
		end
		local iUnhappiness = player:GetPublicOpinionUnhappiness();
		local strCurrentIdeology = GameInfo.PolicyBranchTypes[eCurrentIdeology].Description;
		local ePreferredIdeology = player:GetPublicOpinionPreferredIdeology();
		local strPreferredIdeology = tostring((GameInfo.PolicyBranchTypes[ePreferredIdeology] or {}).Description);
		Controls.LabelConfirmChangeIdeology:LocalizeAndSetText("TXT_KEY_POLICYSCREEN_CONFIRM_CHANGE_IDEOLOGY", iAnarchyTurns, iCurrentIdeologyTenets, strCurrentIdeology, iPreferredIdeologyTenets, strPreferredIdeology, iUnhappiness);
		Controls.ChangeIdeologyConfirm:SetHide(false);
	end

	function OnChangeIdeologyConfirmYes( )
		local player = Players[Game.GetActivePlayer()];
		local ePreferredIdeology = player:GetPublicOpinionPreferredIdeology();
		Controls.ChangeIdeologyConfirm:SetHide(true);
		Network.SendChangeIdeology();
		Events.AudioPlay2DSound("AS2D_INTERFACE_POLICY");
		if ePreferredIdeology < 0 then
			Events.SerialEventGameMessagePopup{Type = ButtonPopupTypes.BUTTONPOPUP_CHOOSE_IDEOLOGY}
			OnClose()
		end
	end
	Controls.ChangeIdeologyConfirmYes:RegisterCallback( Mouse.eLClick, OnChangeIdeologyConfirmYes );

	function OnChangeIdeologyConfirmNo( )
		Controls.ChangeIdeologyConfirm:SetHide(true);
	end
	Controls.ChangeIdeologyConfirmNo:RegisterCallback( Mouse.eLClick, OnChangeIdeologyConfirmNo );

	-- Register tabbing behavior and assign global TabSelect routine.
	TabSelect = RegisterTabBehavior(g_Tabs, g_Tabs["SocialPolicies"]);

	Controls.ToIdeologyTab:RegisterCallback(Mouse.eLClick, function() TabSelect(g_Tabs["Ideologies"]) end);
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )
	if not bInitState then
		Controls.PolicyInfo:SetCheck( OptionsManager.GetPolicyInfo() );
		if not bIsHide then
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
	if bnw_mode then
		if not Controls.PolicyConfirm:IsHidden() or not Controls.TenetConfirm:IsHidden() or not Controls.ChangeIdeologyConfirm:IsHidden() then
			Controls.TenetConfirm:SetHide(true);
			Controls.ChangeIdeologyConfirm:SetHide(true);
			Controls.PolicyConfirm:SetHide(true);
			Controls.BGBlock:SetHide(false);
		end
	elseif not Controls.PolicyConfirm:IsHidden() then
		Controls.PolicyConfirm:SetHide(true);
		Controls.BGBlock:SetHide(false);
	end
	OnClose();
end
Events.GameplaySetActivePlayer.Add(OnActivePlayerChanged);

Init();
