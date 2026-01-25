print("This is the modded ChooseReligionPopup from Community Patch");

include("InstanceManager");
include("VPUI_core");
include("CPK.lua");

local L = Locale.Lookup;
local VP = MapModData.VP;
local IconHookupOrDefault = VP.IconHookupOrDefault;
local Show = CPK.UI.Control.Show;
local Hide = CPK.UI.Control.Hide;
local Disable = CPK.UI.Control.Disable;
local Enable = CPK.UI.Control.Enable;
local Refresh = CPK.UI.Control.Refresh;

--- @enum BeliefSlot
local BeliefSlots = {
	NONE = 0,
	PANTHEON = 1,
	FOUNDER = 2,
	FOLLOWER1 = 3,
	BONUS = 4,
	FOLLOWER2 = 5,
	ENHANCER = 6,
};

--- @class BeliefSlotDetail
--- @field SelectButton Control
--- @field AlphaAnim Control
--- @field NameLabel Control
--- @field DescriptionLabel Control
--- @field Lock Control?
--- @field AvailableListFunc fun(): BeliefType[]
--- @field DefaultName string
--- @field DefaultDescription string
--- @field BlockedDescription string?

--- @type table<BeliefSlot, BeliefSlotDetail>
local tBeliefSlotDetails = {
	[BeliefSlots.PANTHEON] = {
		SelectButton = Controls.PantheonBeliefButton,
		AlphaAnim = Controls.AlphaAnimPantheon,
		NameLabel = Controls.PantheonBeliefName,
		DescriptionLabel = Controls.PantheonBeliefDescription,
		AvailableListFunc = Game.GetAvailablePantheonBeliefs,
		DefaultName = L("TXT_KEY_CHOOSE_RELIGION_PANTHEON_BELIEF"),
		DefaultDescription = L("TXT_KEY_CHOOSE_RELIGION_SELECT_PANTHEON_BELIEF"),
	},
	[BeliefSlots.FOUNDER] = {
		SelectButton = Controls.FounderBeliefButton,
		AlphaAnim = Controls.AlphaAnimFounder,
		NameLabel = Controls.FounderBeliefName,
		DescriptionLabel = Controls.FounderBeliefDescription,
		AvailableListFunc = Game.GetAvailableFounderBeliefs,
		DefaultName = L("TXT_KEY_CHOOSE_RELIGION_FOUNDER_BELIEF"),
		DefaultDescription = L("TXT_KEY_CHOOSE_RELIGION_SELECT_FOUNDER_BELIEF"),
	},
	[BeliefSlots.FOLLOWER1] = {
		SelectButton = Controls.FollowerBeliefButton,
		AlphaAnim = Controls.AlphaAnimFollower,
		NameLabel = Controls.FollowerBeliefName,
		DescriptionLabel = Controls.FollowerBeliefDescription,
		AvailableListFunc = Game.GetAvailableFollowerBeliefs,
		DefaultName = L("TXT_KEY_CHOOSE_RELIGION_FOLLOWER_BELIEF"),
		DefaultDescription = L("TXT_KEY_CHOOSE_RELIGION_SELECT_FOLLOWER_BELIEF"),
	},
	[BeliefSlots.BONUS] = {
		SelectButton = Controls.BonusBeliefButton,
		AlphaAnim = Controls.AlphaAnimBonus,
		NameLabel = Controls.BonusBeliefName,
		DescriptionLabel = Controls.BonusBeliefDescription,
		Lock = Controls.BonusLock,
		AvailableListFunc = Game.GetAvailableBonusBeliefs,
		DefaultName = L("TXT_KEY_CHOOSE_RELIGION_BONUS_BELIEF"),
		DefaultDescription = L("TXT_KEY_CHOOSE_RELIGION_SELECT_BONUS_BELIEF"),
		BlockedDescription = L("TXT_KEY_CHOOSE_RELIGION_BYZANTINES_ONLY"),
	},
	[BeliefSlots.FOLLOWER2] = {
		SelectButton = Controls.FollowerBelief2Button,
		AlphaAnim = Controls.AlphaAnimFollower2,
		NameLabel = Controls.FollowerBelief2Name,
		DescriptionLabel = Controls.FollowerBelief2Description,
		Lock = Controls.Follower2Lock,
		AvailableListFunc = Game.GetAvailableFollowerBeliefs,
		DefaultName = L("TXT_KEY_CHOOSE_RELIGION_FOLLOWER_BELIEF2"),
		DefaultDescription = L("TXT_KEY_CHOOSE_RELIGION_SELECT_FOLLOWER_BELIEF2"),
		BlockedDescription = L("TXT_KEY_CHOOSE_RELIGION_AVAILABLE_LATER"),
	},
	[BeliefSlots.ENHANCER] = {
		SelectButton = Controls.EnhancerBeliefButton,
		AlphaAnim = Controls.AlphaAnimEnhancer,
		NameLabel = Controls.EnhancerBeliefName,
		DescriptionLabel = Controls.EnhancerBeliefDescription,
		Lock = Controls.EnhancerLock,
		AvailableListFunc = Game.GetAvailableEnhancerBeliefs,
		DefaultName = L("TXT_KEY_CHOOSE_RELIGION_SPREAD_BELIEF"),
		DefaultDescription = L("TXT_KEY_CHOOSE_RELIGION_SELECT_SPREAD_BELIEF"),
		BlockedDescription = L("TXT_KEY_CHOOSE_RELIGION_AVAILABLE_LATER"),
	},
};

local g_ReligionButtonInstanceManager = InstanceManager:new("ReligionButtonInstance", "Base", Controls.ReligionStack);
local g_BeliefInstanceManager = InstanceManager:new("BeliefItemInstance", "Button", Controls.BeliefStack);

-- The current religion selection, reset when switching players or someone else takes the religion
local g_eCurrentSlot = BeliefSlots.NONE;
local g_eCurrentReligion = ReligionTypes.NO_RELIGION;
local g_strCurrentReligionName;

local g_bFoundingReligion = true;
local g_popupInfo;

--- @type table<BeliefSlot, BeliefType>
local g_tSelectedBeliefs = {};

-------------------------------------------------------------------------------
--- @param popupInfo PopupInfo
Events.SerialEventGameMessagePopup.Add(function (popupInfo)
	local popupType = popupInfo.Type;
	if popupType ~= ButtonPopupTypes.BUTTONPOPUP_FOUND_RELIGION then
		return;
	end

	g_popupInfo = popupInfo;
	g_bFoundingReligion = popupInfo.Option1;

	UIManager:QueuePopup(ContextPtr, PopupPriority.SocialPolicy);
end);

-------------------------------------------------------------------------------
--- @param eSlot BeliefSlot
--- @return boolean
local function ToggleBeliefSlot(eSlot)
	if not Controls.BeliefPanel:IsHidden() and g_eCurrentSlot == eSlot or eSlot == BeliefSlots.NONE then
		Show(Controls.FoundReligion);
		Hide(Controls.BeliefPanel);
		Controls.CloseButton:LocalizeAndSetText("TXT_KEY_CLOSE");
		g_eCurrentSlot = BeliefSlots.NONE;
		return false;
	else
		Hide(Controls.FoundReligion);
		Show(Controls.BeliefPanel);
		Controls.CloseButton:LocalizeAndSetText("TXT_KEY_BACK_BUTTON");
		g_eCurrentSlot = eSlot;
		return true;
	end
end

-------------------------------------------------------------------------------
local function ValidateSelection()
	local pPlayer = Players[Game.GetActivePlayer()];

	if g_bFoundingReligion and g_tSelectedBeliefs[BeliefSlots.PANTHEON] and g_tSelectedBeliefs[BeliefSlots.FOUNDER] and g_tSelectedBeliefs[BeliefSlots.FOLLOWER1] then
		if pPlayer:IsTraitBonusReligiousBelief() then
			if g_tSelectedBeliefs[BeliefSlots.BONUS] then
				Enable(Controls.FoundReligion);
			else
				Disable(Controls.FoundReligion);
			end
		else
			Enable(Controls.FoundReligion);
		end
	elseif g_tSelectedBeliefs[BeliefSlots.FOLLOWER2] and g_tSelectedBeliefs[BeliefSlots.ENHANCER] then
		Enable(Controls.FoundReligion);
	else
		Disable(Controls.FoundReligion);
	end
end

-------------------------------------------------------------------------------
--- @param tAvailableBeliefs BeliefInfoWithScore[]
--- @param eSlot BeliefSlot
local function BuildBeliefList(tAvailableBeliefs, eSlot)
	g_BeliefInstanceManager:ResetInstances();

	local bDebug = Game.IsDebugMode();

	-- Get the top 3 scores
	local iRank1Score = 0;
	local iRank2Score = 0;
	local iRank3Score = 0;
	for _, belief in ipairs(tAvailableBeliefs) do
		local iScore = belief.Score;
		if iScore >= iRank1Score then
			iRank3Score = iRank2Score;
			iRank2Score = iRank1Score;
			iRank1Score = iScore;
		elseif iScore >= iRank2Score then
			iRank3Score = iRank2Score;
			iRank2Score = iScore;
		elseif iScore >= iRank3Score then
			iRank3Score = iScore;
		end
	end

	-- Start building the list
	for _, belief in ipairs(tAvailableBeliefs) do
		local instance = g_BeliefInstanceManager:GetInstance();
		instance.Name:SetText(belief.Name);
		instance.Name:SetToolTipString(belief.Tooltip);
		instance.Description:SetText(belief.Description);
		instance.Description:SetToolTipString(belief.Tooltip);
		instance.Button:SetToolTipString(belief.Tooltip);
		if not OptionsManager.IsNoBasicHelp() then
			local iScore = belief.Score;
			local strDebugScore = bDebug and L("TXT_KEY_DEBUG_SCORE", iScore) or "";
			if iScore >= iRank1Score then
				strDebugScore = string.format(" (%s[ICON_TROPHY_GOLD])", strDebugScore);
			elseif iScore >= iRank2Score then
				strDebugScore = string.format(" (%s[ICON_TROPHY_SILVER])", strDebugScore);
			elseif iScore >= iRank3Score then
				strDebugScore = string.format(" (%s[ICON_TROPHY_BRONZE])", strDebugScore);
			elseif bDebug then
				strDebugScore = string.format(" (%s)", strDebugScore);
			end
			instance.Name:SetText(belief.Name .. strDebugScore);

			if iScore >= iRank3Score then
				instance.Button:SetToolTipString(belief.Tooltip .. "[NEWLINE][NEWLINE]" .. L("TXT_KEY_RELIGION_CHOOSER_ADVISOR_RECOMMENDATION"));
				instance.Name:SetToolTipString(belief.Tooltip .. "[NEWLINE][NEWLINE]" .. L("TXT_KEY_RELIGION_CHOOSER_ADVISOR_RECOMMENDATION"));
				instance.Description:SetToolTipString(belief.Tooltip .. "[NEWLINE][NEWLINE]" .. L("TXT_KEY_RELIGION_CHOOSER_ADVISOR_RECOMMENDATION"));
			end
		end

		instance.Button:SetSizeY(instance.Description:GetSizeY() + 45);
		instance.AnimGrid:SetSizeY(instance.Description:GetSizeY() + 50);

		instance.Button:RegisterCallback(Mouse.eLClick, function ()
			tBeliefSlotDetails[eSlot].NameLabel:SetText(belief.Name);
			tBeliefSlotDetails[eSlot].NameLabel:SetToolTipString(belief.Tooltip);
			tBeliefSlotDetails[eSlot].DescriptionLabel:SetText(belief.Description);
			tBeliefSlotDetails[eSlot].DescriptionLabel:SetToolTipString(belief.Tooltip);
			g_tSelectedBeliefs[eSlot] = belief.ID;
			ValidateSelection();
			ToggleBeliefSlot(BeliefSlots.NONE);
		end);
	end

	Refresh(Controls.BeliefStack, Controls.BeliefScrollPanel);
	Controls.BeliefScrollPanel:SetScrollValue(0);
end

-------------------------------------------------------------------------------
local function Close()
	if g_eCurrentSlot ~= BeliefSlots.NONE then
		ToggleBeliefSlot(BeliefSlots.NONE);
	else
		UIManager:DequeuePopup(ContextPtr);
	end
end

Controls.CloseButton:RegisterCallback(Mouse.eLClick, Close);

-------------------------------------------------------------------------------
--- @param uiMsg KeyEventType
--- @param wParam KeyType
--- @return boolean
ContextPtr:SetInputHandler(function (uiMsg, wParam)
	if uiMsg == KeyEvents.KeyDown then
		if wParam == Keys.VK_ESCAPE then
			if not Controls.ChooseConfirm:IsHidden() then
				Hide(Controls.ChooseConfirm);
			elseif not Controls.ChangeNamePopup:IsHidden() then
				Hide(Controls.ChangeNamePopup);
			else
				Close();
			end
			return true;
		end

		-- Do nothing
		if wParam == Keys.VK_RETURN then
			return true;
		end
	end
	return false;
end);

-------------------------------------------------------------------------------
--- @type function
local SelectReligion;

local function RefreshReligions()
	g_ReligionButtonInstanceManager:ResetInstances();
	local eActivePlayer = Game.GetActivePlayer();
	local pActiveTeam = Teams[Game.GetActiveTeam()];
	local tReligionFounders = {};

	for ePlayer = 0, GameDefines.MAX_MAJOR_CIVS - 1 do
		local pPlayer = Players[ePlayer];
		if pPlayer:IsEverAlive() and pPlayer:OwnsReligion() then
			local eReligion = pPlayer:GetOwnedReligion();
			if eReligion > ReligionTypes.RELIGION_PANTHEON then
				tReligionFounders[eReligion] = pActiveTeam:IsHasMet(pPlayer:GetTeam()) and pPlayer:GetName() or L("TXT_KEY_CHOOSE_RELIGION_UNMET_PLAYER");
				if eActivePlayer == ePlayer then
					g_eCurrentReligion = eReligion;
				elseif g_eCurrentReligion == eReligion then
					-- Someone took the one we had selected. Start over.
					Hide(Controls.ReligionPanel);
					g_eCurrentReligion = ReligionTypes.NO_RELIGION;
					g_strCurrentReligionName = nil;
					g_tSelectedBeliefs = {};
				end
			end
		end
	end

	-- Refresh religion buttons
	local tReligions = {};
	for kReligionInfo in GameInfo.Religions("Type <> 'RELIGION_PANTHEON'") do
		local religion = {
			ID = kReligionInfo.ID,
			Description = L(kReligionInfo.Description),
			PortraitIndex = kReligionInfo.PortraitIndex,
			IconAtlas = kReligionInfo.IconAtlas,
			ToolTip = L(kReligionInfo.Description),
			Disabled = false,
		};

		if g_eCurrentReligion == religion.ID then
			religion.Disabled = true;
		elseif tReligionFounders[religion.ID] then
			religion.Disabled = true;
			religion.ToolTip = L("TXT_KEY_CHOOSE_RELIGION_ALREADY_FOUNDED", religion.Description, tReligionFounders[religion.ID]);
		end
		table.insert(tReligions, religion);
	end

	table.sort(tReligions, function(a, b)
		return Locale.Compare(a.Description, b.Description) < 0;
	end);

	for _, religion in ipairs(tReligions) do
		local instance = g_ReligionButtonInstanceManager:GetInstance();
		instance.Button:SetDisabled(religion.Disabled);
		instance.Button:SetToolTipString(religion.ToolTip);
		IconHookupOrDefault(religion.PortraitIndex, 64, religion.IconAtlas, instance.Icon);

		if g_bFoundingReligion then
			instance.Button:RegisterCallback(Mouse.eLClick, function ()
				SelectReligion(religion.ID, religion.Description, religion.IconAtlas, religion.PortraitIndex);
			end);
		else
			Disable(instance.Button);
			instance.Button:ClearCallback(Mouse.eLClick);
		end
	end

	if #tReligions > 13 then
		Controls.ReligionStack:SetAnchor("L,T");
	else
		Controls.ReligionStack:SetAnchor("C,T");
	end

	Refresh(Controls.ReligionStack, Controls.ReligionScrollPanel);
end

-------------------------------------------------------------------------------
local function ChangeReligionName()
	Controls.NewName:SetText(g_strCurrentReligionName);
	Hide(Controls.ChangeNameError);
	Show(Controls.ChangeNamePopup);
end

--- @param eReligion ReligionType
--- @param strReligionName string
--- @param strIconAtlas string
--- @param iPortraitIndex integer
SelectReligion = function (eReligion, strReligionName, strIconAtlas, iPortraitIndex)
	local eActivePlayer = Game.GetActivePlayer();

	g_eCurrentReligion = eReligion;
	RefreshReligions();

	if g_eCurrentReligion ~= ReligionTypes.NO_RELIGION then
		IconHookupOrDefault(iPortraitIndex, 80, strIconAtlas, Controls.ReligionIcon);
		Hide(Controls.LabelPleaseSelectAReligion);
		g_strCurrentReligionName = strReligionName;

		if Players[eActivePlayer]:OwnsReligion() then
			Controls.LabelReligionName:SetText(strReligionName);
			Controls.LabelReligionName:ClearCallback(Mouse.eLClick);
			Controls.FoundReligion:LocalizeAndSetText("TXT_KEY_CHOOSE_RELIGION_OK_BUTTON_ENHANCE");
			Controls.FoundReligionHeading:LocalizeAndSetText("TXT_KEY_CHOOSE_RELIGION_TITLE_ENHANCE");
		else
			Controls.LabelReligionName:LocalizeAndSetText("TXT_KEY_CHOOSE_RELIGION_NAME_LABEL", strReligionName);
			Controls.LabelReligionName:RegisterCallback(Mouse.eLClick, ChangeReligionName);
			Controls.FoundReligion:LocalizeAndSetText("TXT_KEY_CHOOSE_RELIGION_OK_BUTTON");
			Controls.FoundReligionHeading:LocalizeAndSetText("TXT_KEY_CHOOSE_RELIGION_TITLE");
		end

		local iYear = Game.GetGameTurnYear();
		local strYear;
		if iYear < 0 then
			strYear = L("TXT_KEY_TIME_BC", math.abs(iYear));
		else
			strYear = L("TXT_KEY_TIME_AD", math.abs(iYear));
		end

		Controls.LabelReligionFoundedIn:LocalizeAndSetText("TXT_KEY_CHOOSE_RELIGION_FOUNDED_IN", strYear);
		Show(Controls.ReligionPanel);
	end
end

-------------------------------------------------------------------------------
Controls.Yes:RegisterCallback(Mouse.eLClick, function ()
	local ePlayer = Game.GetActivePlayer();
	local pPlayer = Players[ePlayer];
	local kReligionInfo = GameInfo.Religions[g_eCurrentReligion];

	local strCustomName;
	if L(kReligionInfo.Description) ~= g_strCurrentReligionName then
		strCustomName = g_strCurrentReligionName;
	end

	if g_bFoundingReligion then
		Network.SendFoundReligion(ePlayer, g_eCurrentReligion, strCustomName,
			g_tSelectedBeliefs[BeliefSlots.FOUNDER],
			g_tSelectedBeliefs[BeliefSlots.FOLLOWER1],
			not pPlayer:HasCreatedPantheon() and g_tSelectedBeliefs[BeliefSlots.PANTHEON] or BeliefTypes.NO_BELIEF,
			g_tSelectedBeliefs[BeliefSlots.BONUS] or BeliefTypes.NO_BELIEF,
			g_popupInfo.Data1, g_popupInfo.Data2);
	else
		Network.SendEnhanceReligion(ePlayer, g_eCurrentReligion, nil,
			g_tSelectedBeliefs[BeliefSlots.FOLLOWER2],
			g_tSelectedBeliefs[BeliefSlots.ENHANCER],
			g_popupInfo.Data1, g_popupInfo.Data2);
	end
	Events.AudioPlay2DSound("AS2D_INTERFACE_POLICY");

	Hide(Controls.ChooseConfirm);
	Close();
end);

Controls.No:RegisterCallback(Mouse.eLClick, function ()
	Hide(Controls.ChooseConfirm);
end);

Controls.ChangeNameOKButton:RegisterCallback(Mouse.eLClick, function ()
	local strReligionName = Controls.NewName:GetText();
	if Locale.IsNilOrEmpty(strReligionName) then
		Show(Controls.ChangeNameError);
	else
		g_strCurrentReligionName = strReligionName;
		Controls.LabelReligionName:LocalizeAndSetText("TXT_KEY_CHOOSE_RELIGION_NAME_LABEL", g_strCurrentReligionName);
		Hide(Controls.ChangeNamePopup);
	end
end);

Controls.ChangeNameDefaultButton:RegisterCallback(Mouse.eLClick, function ()
	Controls.NewName:LocalizeAndSetText(GameInfo.Religions[g_eCurrentReligion].Description);
	Hide(Controls.ChangeNameError);
end);

Controls.ChangeNameCancelButton:RegisterCallback(Mouse.eLClick, function ()
	Hide(Controls.ChangeNamePopup);
end);

Controls.FoundReligion:RegisterCallback(Mouse.eLClick, function ()
	if g_bFoundingReligion then
		Controls.LabelConfirmFoundReligion:LocalizeAndSetText("TXT_KEY_CHOOSE_RELIGION_CONFIRM", g_strCurrentReligionName);
	else
		Controls.LabelConfirmFoundReligion:LocalizeAndSetText("TXT_KEY_CHOOSE_RELIGION_CONFIRM_ENHANCE", g_strCurrentReligionName);
	end
	Show(Controls.ChooseConfirm);
end);

-------------------------------------------------------------------------------
--- @param eSlot BeliefSlot
local function OnSelectButtonClick(eSlot)
	if ToggleBeliefSlot(eSlot) then
		--- @type BeliefInfoWithScore[]
		local tAvailableBeliefs = {};
		local ePlayer = Game.GetActivePlayer();
		for _, eBelief in ipairs(tBeliefSlotDetails[eSlot].AvailableListFunc()) do
			local kBeliefInfo = GameInfo.Beliefs[eBelief];
			if (eSlot == BeliefSlots.BONUS and eBelief ~= g_tSelectedBeliefs[BeliefSlots.PANTHEON] and
				eBelief ~= g_tSelectedBeliefs[BeliefSlots.FOLLOWER1] and
				eBelief ~= g_tSelectedBeliefs[BeliefSlots.FOUNDER]) or
			(eSlot ~= BeliefSlots.BONUS and eBelief ~= g_tSelectedBeliefs[BeliefSlots.BONUS]) then
				table.insert(tAvailableBeliefs, {
					ID = kBeliefInfo.ID,
					Name = L(kBeliefInfo.ShortDescription),
					Description = L(kBeliefInfo.Description),
					Tooltip = kBeliefInfo.Tooltip and L(kBeliefInfo.Tooltip) or L(kBeliefInfo.Description),
					Score = Game.ScoreBelief(ePlayer, kBeliefInfo.ID),
				});
			end
		end

		table.sort(tAvailableBeliefs, function (a, b)
			return Locale.Compare(a.Name, b.Name) < 0;
		end);

		BuildBeliefList(tAvailableBeliefs, eSlot);
	end
end

--- @param tSlotList BeliefSlot[]
local function EnableBeliefButtons(tSlotList)
	for _, eSlot in pairs(tSlotList) do
		local tSlotDetail = tBeliefSlotDetails[eSlot];
		tSlotDetail.SelectButton:RegisterCallback(Mouse.eLClick, function ()
			OnSelectButtonClick(eSlot);
		end);
		tSlotDetail.DescriptionLabel:SetText(tSlotDetail.DefaultDescription);
		tSlotDetail.DescriptionLabel:SetOffsetVal(24, 25);
		Show(tSlotDetail.AlphaAnim);
		if tSlotDetail.Lock then
			Hide(tSlotDetail.Lock);
		end
	end
end

--- @param tSlotList BeliefSlot[]
local function DisableBeliefButtons(tSlotList)
	for _, eSlot in pairs(tSlotList) do
		local tSlotDetail = tBeliefSlotDetails[eSlot];
		tSlotDetail.SelectButton:ClearCallback(Mouse.eLClick);
		tSlotDetail.DescriptionLabel:SetText(tSlotDetail.BlockedDescription or "");
		tSlotDetail.DescriptionLabel:SetOffsetVal(52, 30);
		Hide(tSlotDetail.AlphaAnim);
		if tSlotDetail.Lock then
			Show(tSlotDetail.Lock);
		end
	end
end

--- @param tSlotBeliefs table<BeliefSlot, BeliefType>
local function SetupExistingBeliefButtons(tSlotBeliefs)
	for eSlot, eBelief in ipairs(tSlotBeliefs) do
		local kBeliefInfo = GameInfo.Beliefs[eBelief];
		assert(kBeliefInfo, "Savegame or database could be corrupt. If this issue persists, consider reinstalling the mod.");
		if kBeliefInfo.Reformation then 
			assert( eSlot == #tSlotBeliefs , "Did you reform before founding?!" );
			break;
		end
		tBeliefSlotDetails[eSlot].NameLabel:LocalizeAndSetText(kBeliefInfo.ShortDescription);
		tBeliefSlotDetails[eSlot].NameLabel:LocalizeAndSetToolTip(kBeliefInfo.Description);
		tBeliefSlotDetails[eSlot].DescriptionLabel:LocalizeAndSetText(kBeliefInfo.Description);
		tBeliefSlotDetails[eSlot].DescriptionLabel:LocalizeAndSetToolTip(kBeliefInfo.Description);
		tBeliefSlotDetails[eSlot].DescriptionLabel:SetOffsetVal(24, 25);
		g_tSelectedBeliefs[eSlot] = eBelief;
	end
end

local function RefreshExistingBeliefs()
	local pPlayer = Players[Game.GetActivePlayer()];
	local bEnhancing = pPlayer:OwnsReligion();
	local bBonusBelief = pPlayer:IsTraitBonusReligiousBelief();

	-- Handle bonus belief first
	if bBonusBelief and not bEnhancing then
		EnableBeliefButtons{BeliefSlots.BONUS};
	else
		DisableBeliefButtons{BeliefSlots.BONUS};
		if bBonusBelief then -- Already has the bonus belief by now!
			Hide(Controls.BonusLock);
			Hide(Controls.AlphaAnimBonus)
		end
	end

	-- Config the UI based on the player's religion status
	if bEnhancing then
		DisableBeliefButtons{BeliefSlots.PANTHEON, BeliefSlots.FOLLOWER1, BeliefSlots.FOUNDER};
		EnableBeliefButtons{BeliefSlots.FOLLOWER2, BeliefSlots.ENHANCER};
		-- The case where the player already founded and has a bonus belief is handled here
		SetupExistingBeliefButtons(Game.GetBeliefsInReligion(pPlayer:GetOwnedReligion()));
	elseif pPlayer:HasCreatedPantheon() then
		DisableBeliefButtons{BeliefSlots.PANTHEON, BeliefSlots.FOLLOWER2, BeliefSlots.ENHANCER};
		EnableBeliefButtons{BeliefSlots.FOLLOWER1, BeliefSlots.FOUNDER};
		SetupExistingBeliefButtons{[BeliefSlots.PANTHEON] = pPlayer:GetBeliefInPantheon()};
	else
		DisableBeliefButtons{BeliefSlots.FOLLOWER2, BeliefSlots.ENHANCER};
		EnableBeliefButtons{BeliefSlots.PANTHEON, BeliefSlots.FOLLOWER1, BeliefSlots.FOUNDER};
	end
end

--- @param bIsHide boolean
--- @param bInitState boolean
ContextPtr:SetShowHideHandler(function (bIsHide, bInitState)
	if not bInitState then
		if not bIsHide then
			UI.incTurnTimerSemaphore();
			Events.SerialEventGameMessagePopupShown(g_popupInfo);

			RefreshReligions();
			RefreshExistingBeliefs();

			if g_eCurrentReligion ~= ReligionTypes.NO_RELIGION then
				local kReligionInfo = GameInfo.Religions[g_eCurrentReligion];
				SelectReligion(g_eCurrentReligion, L(Game.GetReligionName(g_eCurrentReligion)), kReligionInfo.IconAtlas, kReligionInfo.PortraitIndex);
			else
				Show(Controls.LabelPleaseSelectAReligion);
			end

			ValidateSelection();

			ToggleBeliefSlot(BeliefSlots.NONE);
		else
			if g_popupInfo then
				Events.SerialEventGameMessagePopupProcessed.CallImmediate(g_popupInfo.Type, 0);
			end
			UI.decTurnTimerSemaphore();
		end
	end
end);

----------------------------------------------------------------
-- 'Active' (local human) player has changed
Events.GameplaySetActivePlayer.Add(function ()
	Hide(Controls.ReligionPanel, Controls.ChooseConfirm);
	g_eCurrentReligion = ReligionTypes.NO_RELIGION;
	g_strCurrentReligionName = nil;
	g_tSelectedBeliefs = {};
end);
