-------------------------------------------------
-- TECH PANEL
-------------------------------------------------
include("InstanceManager");
include("TechButtonInclude");
include("TechHelpInclude");
include("VPUI_core");
include("CPK.lua");

local NUM_SMALL_BUTTONS = 5;

local iPortraitSize = Controls.TechIcon:GetSize().x;

local eActivePlayer = Game.GetActivePlayer();
local eRecentTech = -1;
local eCurrentTech = -1;

local L = Locale.Lookup;
local Hide = CPK.UI.Control.Hide;
local Show = CPK.UI.Control.Show;
local StringBuilder = CPK.Util.StringBuilder;

local strChooseResearch = L("TXT_KEY_NOTIFICATION_SUMMARY_NEW_RESEARCH");
local strFinished = L("TXT_KEY_RESEARCH_FINISHED");

local function OnTechnologyButtonClicked()
	if Controls.TechPopup:IsHidden() then
		Events.SerialEventGameMessagePopup({
			Type = ButtonPopupTypes.BUTTONPOPUP_CHOOSETECH,
			Data1 = Game.GetActivePlayer(),
			Data3 = -1, -- this is to tell it that a tech was not just finished
		});
	else
		Hide(Controls.TechPopup);
	end
end

Controls.TechButton:RegisterCallback(Mouse.eLClick, OnTechnologyButtonClicked);
Controls.BigTechButton:RegisterCallback(Mouse.eLClick, OnTechnologyButtonClicked);
Controls.BigTechButton:RegisterCallback(Mouse.eRClick, function ()
	local eTech = (eCurrentTech == -1) and eRecentTech or eCurrentTech;
	if eTech ~= -1 then
		Events.SearchForPediaEntry(L(GameInfo.Technologies[eTech].Description));
	end
end);

local function OnTechPanelUpdated()
	-- Empty the UI when there's no active player (during CS or barbarian turn)
	if eActivePlayer < 0 then
		Hide(Controls.ActiveStyle);
		Controls.ActiveStyle:SetToolTipString(strChooseResearch);
		Controls.TechButton:SetToolTipString(strChooseResearch);
		Controls.BigTechButton:SetToolTipString(strChooseResearch);
		Controls.ResearchMeter:SetPercents(0, 0);
		Hide(Controls.TechIcon);
		Controls.TechText:SetText("");
		return;
	end

	local pPlayer = Players[eActivePlayer];
	local pTeam = Teams[pPlayer:GetTeam()];
	local pTeamTechs = pTeam:GetTeamTechs();

	eCurrentTech = pPlayer:GetCurrentResearch();
	eRecentTech = pTeamTechs:GetLastTechAcquired();

	local fResearchProgressPercent = 0;
	local fResearchProgressPlusThisTurnPercent = 0;

	local strResearchStatus;
	local strTech;
	local strTooltip;
	local kTechInfo;

	-- Are we researching something right now?
	if eCurrentTech ~= -1 then
		local iResearchTurnsLeft = pPlayer:GetResearchTurnsLeft(eCurrentTech, true);
		local iCurrentProgressTimes100 = pPlayer:GetResearchProgressTimes100(eCurrentTech);
		local iTechCost = pPlayer:GetResearchCost(eCurrentTech);
		local iNextTurnProgressTimes100 = iCurrentProgressTimes100 + pPlayer:GetScienceTimes100();

		fResearchProgressPercent = iCurrentProgressTimes100 / iTechCost;
		fResearchProgressPlusThisTurnPercent = math.min(100, iNextTurnProgressTimes100 / iTechCost);

		kTechInfo = GameInfo.Technologies[eCurrentTech];
		strTech = L(kTechInfo.Description);
		if iNextTurnProgressTimes100 > iCurrentProgressTimes100 then
			strTech = string.format("%s (%s)", strTech, iResearchTurnsLeft);
		end
		strTooltip = GetHelpTextForTech(eCurrentTech);
	elseif eRecentTech ~= -1 then -- maybe we just finished something
		kTechInfo = GameInfo.Technologies[eRecentTech];
		strTech = L(kTechInfo.Description);
		strResearchStatus = strFinished;
		strTooltip = strChooseResearch;
	else
		strResearchStatus = strChooseResearch;
		strTooltip = strChooseResearch;
		Hide(Controls.ActiveStyle);
	end

	if eCurrentTech ~= -1 or eRecentTech ~= -1 then
		-- Update the tech icon
		if IconHookup(kTechInfo.PortraitIndex, iPortraitSize, kTechInfo.IconAtlas, Controls.TechIcon) then
			Show(Controls.TechIcon);
		else
			Hide(Controls.TechIcon);
		end

		-- Update the small buttons
		local iNumSmallButtons = AddSmallButtonsToTechButton(Controls, kTechInfo, NUM_SMALL_BUTTONS, 64);
		AddCallbackToSmallButtons(Controls, NUM_SMALL_BUTTONS, -1, -1, Mouse.eLClick, OnTechnologyButtonClicked);
		if iNumSmallButtons > 0 then
			if OptionsManager.GetSmallUIAssets() then
				Controls.ActiveStyle:SetSizeVal(434, 126);
			else
				Controls.ActiveStyle:SetSizeVal(460, 126);
			end
			Show(Controls.ActiveStyle);
		else
			Hide(Controls.ActiveStyle);
		end
	end

	-- Research meter
	if eCurrentTech ~= -1 then
		Controls.ResearchMeter:SetPercents(fResearchProgressPercent / 100, fResearchProgressPlusThisTurnPercent / 100);
	else
		Controls.ResearchMeter:SetPercents(1, 0);
	end

	-- Text and tooltips
	Controls.TechText:SetText(StringBuilder.New():Append(strResearchStatus):Append(strTech):Concat(" "));
	Controls.ActiveStyle:SetToolTipString(strTooltip);
	Controls.TechButton:SetToolTipString(strTooltip);
	Controls.BigTechButton:SetToolTipString(strTooltip);
end

Events.SerialEventGameDataDirty.Add(OnTechPanelUpdated);

----------------------------------------------------------------
--- @param eInfoCorner InfoCornerId
Events.OpenInfoCorner.Add(function (eInfoCorner)
	-- Only show if it's our ID
	ContextPtr:SetHide(eInfoCorner ~= InfoCornerID.Tech);
end);

----------------------------------------------------------------
-- When a choose tech popup is triggered
--- @param popupInfo PopupInfo
Events.SerialEventGameMessagePopup.Add(function (popupInfo)
	if (popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_CHOOSETECH or popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_CHOOSE_TECH_TO_STEAL) and
	ContextPtr:IsHidden() then
		Events.OpenInfoCorner(InfoCornerID.Tech);
	end
end);

----------------------------------------------------------------
-- 'Active' (local human) player has changed
--- @param ePlayer PlayerId
Events.GameplaySetActivePlayer.Add(function (ePlayer)
	eActivePlayer = ePlayer;
	OnTechPanelUpdated();
end);

----------------------------------------------------------------
-- Observer mode? Another player's tech may be showed
--- @param ePlayer PlayerId
Events.AIProcessingStartedForPlayer.Add(function (ePlayer)
	local eOldActivePlayer = eActivePlayer;
	local pActivePlayer = Players[Game.GetActivePlayer()];
	if pActivePlayer:IsObserver() then
		local eObserverOverridePlayer = Game:GetObserverUIOverridePlayer();
		if eObserverOverridePlayer > -1 then
			eActivePlayer = eObserverOverridePlayer;
		else
			eActivePlayer = Players[ePlayer]:IsMajorCiv() and ePlayer or -1;
		end
	else
		eActivePlayer = Game.GetActivePlayer();
	end
	if eActivePlayer ~= eOldActivePlayer then
		OnTechPanelUpdated();
	end
end);

OnTechPanelUpdated();
