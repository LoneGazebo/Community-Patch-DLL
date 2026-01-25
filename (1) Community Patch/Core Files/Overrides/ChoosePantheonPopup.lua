print("This is the modded ChoosePantheonPopup from Community Patch");

include("InstanceManager");
include("IconSupport");
include("VPUI_core");
include("CPK.lua");

local L = Locale.Lookup;
local Show = CPK.UI.Control.Show;
local Hide = CPK.UI.Control.Hide;
local Refresh = CPK.UI.Control.Refresh;
local Color = CPK.Text.Color;

local g_ItemInstanceManager = InstanceManager:new("ItemInstance", "Button", Controls.ItemStack);
local g_eBelief = BeliefTypes.NO_BELIEF;
local g_popupInfo;

-- Resize the UI to fit the screen
local _, iScreenY = UIManager:GetScreenSizeVal();
local iHeightOffset = iScreenY - 1020;

Controls.ItemScrollPanel:SetSizeY(Controls.ItemScrollPanel:GetSizeY() + iHeightOffset);
Controls.ItemScrollPanel:CalculateInternalSize();
Controls.ItemScrollPanel:ReprocessAnchoring();
Controls.BottomPanel:SetSizeY(Controls.BottomPanel:GetSizeY() + iHeightOffset);
Controls.BottomPanel:ReprocessAnchoring();

-------------------------------------------------------------------------------
--- @param popupInfo PopupInfo
Events.SerialEventGameMessagePopup.Add(function (popupInfo)
	if popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_FOUND_PANTHEON then
		return;
	end

	g_popupInfo = popupInfo;
	UIManager:QueuePopup(ContextPtr, PopupPriority.SocialPolicy);
end);

-------------------------------------------------------------------------------
local function Close()
	Hide(Controls.ChooseConfirm);
	UIManager:DequeuePopup(ContextPtr);
end

Controls.CloseButton:RegisterCallback(Mouse.eLClick, Close);

-------------------------------------------------------------------------------
--- @param uiMsg KeyEventType
--- @param wParam KeyType
--- @return boolean
ContextPtr:SetInputHandler(function (uiMsg, wParam)
	if uiMsg == KeyEvents.KeyDown then
		if wParam == Keys.VK_RETURN or wParam == Keys.VK_ESCAPE then
			if Controls.ChooseConfirm:IsHidden() then
				Close();
			else
				Hide(Controls.ChooseConfirm);
			end
			return true;
		end
	end
	return false;
end);

-------------------------------------------------------------------------------
--- @param eBelief BeliefType
local function SelectPantheon(eBelief)
	g_eBelief = eBelief;
	Controls.ConfirmText:LocalizeAndSetText("TXT_KEY_CONFIRM_PANTHEON", GameInfo.Beliefs[eBelief].ShortDescription);
	Show(Controls.ChooseConfirm);
end

-------------------------------------------------------------------------------
local function RefreshList()
	g_ItemInstanceManager:ResetInstances();

	local ePlayer = Game.GetActivePlayer();
	CivIconHookup(ePlayer, 64, Controls.CivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true);

	--- @type BeliefInfoWithScore[]
	local tAvailableBeliefs = {};
	local tAvailableBeliefTypes = (g_popupInfo.Data2 > 0) and Game.GetAvailablePantheonBeliefs() or Game.GetAvailableReformationBeliefs();
	local strTitleKey = (g_popupInfo.Data2 > 0) and "TXT_KEY_CHOOSE_PANTHEON_TITLE" or "TXT_KEY_CHOOSE_REFORMATION_BELIEF_TITLE";

	Controls.PanelTitle:LocalizeAndSetText(strTitleKey);
	for _, eBelief in ipairs(tAvailableBeliefTypes) do
		local kBeliefInfo = GameInfo.Beliefs[eBelief];
		table.insert(tAvailableBeliefs, {
			ID = kBeliefInfo.ID,
			Name = L(kBeliefInfo.ShortDescription),
			Description = L(kBeliefInfo.Description),
			Score = Game.ScoreBelief(ePlayer, kBeliefInfo.ID),
		});
	end

	-- Sort beliefs by their names
	table.sort(tAvailableBeliefs, function(a, b)
		return Locale.Compare(a.Name, b.Name) < 0;
	end);

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

	local bDebug = Game.IsDebugMode();
	for i, belief in ipairs(tAvailableBeliefs) do
		local instance = g_ItemInstanceManager:GetInstance();
		instance.Name:SetText(belief.Name);
		instance.Description:SetText(belief.Description);
		instance.Button:SetToolTipString("");

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
				instance.Button:SetToolTipString(L("TXT_KEY_RELIGION_CHOOSER_ADVISOR_RECOMMENDATION"));
				instance.Name:SetToolTipString(L("TXT_KEY_RELIGION_CHOOSER_ADVISOR_RECOMMENDATION"));
				instance.Description:SetToolTipString(L("TXT_KEY_RELIGION_CHOOSER_ADVISOR_RECOMMENDATION"));
			end
		end

		instance.Button:RegisterCallback(Mouse.eLClick, function ()
			SelectPantheon(belief.ID);
		end);

		-- Dynamically resize based on description height
		local iDescHeight = instance.Description:GetSizeY();
		instance.Button:SetSizeY(iDescHeight + 40);
		instance.Box:SetSizeY(iDescHeight + 40);
		instance.BounceAnim:SetSizeY(iDescHeight + 45);
		instance.BounceGrid:SetSizeY(iDescHeight + 45);

		-- Stripes
		if i % 2 == 1 then
			instance.Box:SetColor(Color.TableStripe1:Vec4());
		else
			instance.Box:SetColor(Color.TableStripe2:Vec4());
		end
	end

	Refresh(Controls.ItemStack, Controls.ItemScrollPanel);
end

Controls.Yes:RegisterCallback(Mouse.eLClick, function ()
	Network.SendFoundPantheon(Game.GetActivePlayer(), g_eBelief);
	Events.AudioPlay2DSound("AS2D_INTERFACE_POLICY");
	Close();
end);

Controls.No:RegisterCallback(Mouse.eLClick, function ()
	Hide(Controls.ChooseConfirm);
end);

-------------------------------------------------------------------------------
--- @param bIsHide boolean
---	@param bInitState boolean
ContextPtr:SetShowHideHandler(function (bIsHide, bInitState)
	if not bInitState then
		local unitPanelContext = ContextPtr:LookUpControl("/InGame/WorldView/UnitPanel/Base");
		local infoCornerContext = ContextPtr:LookUpControl("/InGame/WorldView/InfoCorner");
		if not bIsHide then
			UI.incTurnTimerSemaphore();
			Events.SerialEventGameMessagePopupShown(g_popupInfo);
			RefreshList();
			Hide(unitPanelContext, infoCornerContext);
		else
			Show(unitPanelContext, infoCornerContext);
			if g_popupInfo then
				Events.SerialEventGameMessagePopupProcessed.CallImmediate(g_popupInfo.Type, 0);
			end
			UI.decTurnTimerSemaphore();
		end
	end
end);

-------------------------------------------------------------------------------
-- 'Active' (local human) player has changed
Events.GameplaySetActivePlayer.Add(Close);

-------------------------------------------------------------------------------
-- If the user performed any action that changes selection states, just close this UI.
Events.UnitSelectionChanged.Add(Close);
