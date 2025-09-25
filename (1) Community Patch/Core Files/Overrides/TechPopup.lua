-------------------------------------------------
-- Technology Chooser Popup
-------------------------------------------------

include("InstanceManager");
include("TechButtonInclude");
include("TechHelpInclude");
include("VPUI_core");
include("CPK.lua");

local instanceManager = InstanceManager:new("TechButtonInstance", "TechButtonContainer", Controls.ButtonStack);

local NUM_SMALL_BUTTONS = 5;

local eActivePlayer = Game.GetActivePlayer();
local pActivePlayer = Players[eActivePlayer];
local eActiveTeam = Game.GetActiveTeam();
local pActiveTeam = Teams[eActiveTeam];

local g_eStealTechTargetPlayer = -1;
local g_ePopup = -1;

local L = Locale.Lookup;
local Hide = CPK.UI.Control.Hide;
local Show = CPK.UI.Control.Show;

local TURNS_STRING = L("TXT_KEY_TURNS");
local FREE_STRING = L("TXT_KEY_FREE");

------------------------------------------------------------------
-- Close this popup
local function ClosePopup()
	Hide(ContextPtr);
	Events.SerialEventGameMessagePopupProcessed(g_ePopup, 0);
end

------------------------------------------------------------------
--- Add a button based on the tech info
--- @param kTechInfo Info
--- @param iNumTech integer
local function AddTechButton(kTechInfo, iNumTech)
	local instance = instanceManager:GetInstance();

	-- On tech selected
	local function TechSelected()
		if g_eStealTechTargetPlayer ~= -1 then
			Network.SendResearch(kTechInfo.ID, 0, g_eStealTechTargetPlayer, false);
		else
			Network.SendResearch(kTechInfo.ID, iNumTech, -1, false);
		end
		ClosePopup();
	end

	instance.TechButton:RegisterCallback(Mouse.eLClick, TechSelected);

	local strSearch = L(kTechInfo.Description);
	instance.TechButton:RegisterCallback(Mouse.eRClick, function ()
		Events.SearchForPediaEntry(strSearch);
	end);

	local strTech = L(kTechInfo.Description);
	instance.CurrentlyResearchingTechName:SetText(strTech);
	instance.AvailableTechName:SetText(strTech);
	instance.FreeTechName:SetText(strTech);

	local strTurns = string.format("%s %s", pActivePlayer:GetResearchTurnsLeft(kTechInfo.ID, true), TURNS_STRING);
	local bFreeTech = (iNumTech > 0 or g_eStealTechTargetPlayer ~= -1);
	local bCurrentResearch = (pActivePlayer:GetCurrentResearch() == kTechInfo.ID);

	if bCurrentResearch or pActivePlayer:CanResearch(kTechInfo.ID) then
		if bCurrentResearch then
			Hide(instance.Available);
		else
			Hide(instance.CurrentlyResearching);
		end

		local researchGrid = bCurrentResearch and instance.CurrentlyResearching or instance.Available;
		if bFreeTech then
			Hide(researchGrid);
			instance.TechQueueLabel:SetText(FREE_STRING);
			Show(instance.FreeTech, instance.TechQueue);
		else
			Show(researchGrid);
			Hide(instance.FreeTech);

			-- Update queue number if needed
			local iPosition = bCurrentResearch and -1 or pActivePlayer:GetQueuePosition(kTechInfo.ID);
			if iPosition ~= -1 then
				instance.TechQueueLabel:SetText(tostring(iPosition - 1));
				Show(instance.TechQueue);
			else
				Hide(instance.TechQueue);
			end
		end

		local turnLabel = bFreeTech and instance.FreeTurns or (bCurrentResearch and instance.CurrentlyResearchingTurns or instance.AvailableTurns);
		if pActivePlayer:GetScienceTimes100() > 0 and g_eStealTechTargetPlayer == -1 then
			turnLabel:SetText(strTurns);
			Show(turnLabel);
		else
			Hide(turnLabel);
		end

		if bCurrentResearch then
			-- Turn on the meter
			local pTeamTechs = pActiveTeam:GetTeamTechs();
			local iCurrentProgressTimes100 = pTeamTechs:GetResearchProgressTimes100(kTechInfo.ID);
			local iNextTurnProgressTimes100 = iCurrentProgressTimes100 + pActivePlayer:GetScienceTimes100();
			local iTechCost = pTeamTechs:GetResearchCost(kTechInfo.ID);
			local fCurrentProgressPercent = iCurrentProgressTimes100 / iTechCost;
			local fNextTurnProgressPercent = math.min(100, iNextTurnProgressTimes100 / iTechCost);
			instance.ProgressMeterInternal:SetPercents(fCurrentProgressPercent / 100, fNextTurnProgressPercent / 100);
			Show(instance.ProgressMeter);
		else
			-- Turn off the meter
			Hide(instance.ProgressMeter);
		end
	end

	instance.TechButton:SetToolTipString(GetHelpTextForTech(kTechInfo.ID));

	-- Update the picture
	if IconHookup(kTechInfo.PortraitIndex, 64, kTechInfo.IconAtlas, instance.TechPortrait) then
		Show(instance.TechPortrait);
	else
		Hide(instance.TechPortrait);
	end

	-- Advisors
	instance.MilitaryRecommendation:SetHide(not Game.IsTechRecommended(kTechInfo.ID, AdvisorTypes.ADVISOR_MILITARY));
	instance.EconomicRecommendation:SetHide(not Game.IsTechRecommended(kTechInfo.ID, AdvisorTypes.ADVISOR_ECONOMIC));
	instance.ForeignRecommendation:SetHide(not Game.IsTechRecommended(kTechInfo.ID, AdvisorTypes.ADVISOR_FOREIGN));
	instance.ScienceRecommendation:SetHide(not Game.IsTechRecommended(kTechInfo.ID, AdvisorTypes.ADVISOR_SCIENCE));
	instance.AdvisorStack:CalculateSize();
	instance.AdvisorStack:ReprocessAnchoring();

	-- Add the small buttons and their tooltips
	AddSmallButtonsToTechButton(instance, kTechInfo, NUM_SMALL_BUTTONS, 45);

	-- Treat left clicking on small buttons as clicking on this tech
	AddCallbackToSmallButtons(instance, NUM_SMALL_BUTTONS, 0, 0, Mouse.eLClick, TechSelected);
end

------------------------------------------------------------------
-- On update
local function OnTechPanelUpdated()
	if ContextPtr:IsHidden() then
		return;
	end

	-- Update advisors for this player
	Game.SetAdvisorRecommenderTech(eActivePlayer);

	local iNumTech = pActivePlayer:GetNumFreeTechs();

	-- Clear buttons
	instanceManager:ResetInstances();

	if not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE) then
		for kTechInfo in GameInfo.Technologies() do
			if pActivePlayer:CanResearch(kTechInfo.ID) then
				-- No espionage - choosing a tech
				if g_eStealTechTargetPlayer == -1 then
					-- Normal research or free tech available
					if iNumTech == 0 or pActivePlayer:CanResearchForFree(kTechInfo.ID) then
						AddTechButton(kTechInfo, iNumTech);
					end
				-- Espionage - stealing a tech
				elseif Players[g_eStealTechTargetPlayer]:HasTech(kTechInfo.ID) then
					AddTechButton(kTechInfo, 0);
				end
			end
		end
	end

	local _, iY = UIManager:GetScreenSizeVal();

	Controls.ScrollPanelBlackFrame:SetSizeY(iY);
	Controls.ScrollPanelFrame:SetSizeY(iY);
	if OptionsManager.GetSmallUIAssets() then
		Controls.ScrollPanel:SetSizeY(iY - 244);
	else
		Controls.ScrollPanel:SetSizeY(iY - 300);
	end

	Controls.ButtonStack:CalculateSize();
	Controls.ButtonStack:ReprocessAnchoring();
	Controls.ScrollPanel:CalculateInternalSize();
end

Events.SerialEventGameDataDirty.Add(OnTechPanelUpdated);

------------------------------------------------------------------
-- On display

--- **BUTTONPOPUP_CHOOSETECH**: *Data1*: active player ID, *Data2*: number of free techs, *Data3*: last researched tech ID<br>
--- **BUTTONPOPUP_CHOOSE_TECH_TO_STEAL**: *Data1*: active player ID, *Data2*: target player ID, *Data3*: -1
--- @param popupInfo PopupInfo
Events.SerialEventGameMessagePopup.Add(function (popupInfo)
	-- If popup type isn't choose tech, exit
	if not (popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_CHOOSETECH or popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_CHOOSE_TECH_TO_STEAL) then
		return;
	end

	g_ePopup = popupInfo.Type;

	-- print("popupInfo.Data1: " .. popupInfo.Data1);
	-- print("popupInfo.Data2: " .. popupInfo.Data2);
	-- print("popupInfo.Data3: " .. popupInfo.Data3);

	-- Only display popup if the ID is the active player.
	if popupInfo.Data1 ~= Game.GetActivePlayer() then
		return;
	end

	Show(ContextPtr);

	Events.SerialEventGameMessagePopupShown(popupInfo);

	g_eStealTechTargetPlayer = -1;
	if g_ePopup == ButtonPopupTypes.BUTTONPOPUP_CHOOSE_TECH_TO_STEAL then
		-- print("Setting g_eStealTechTargetPlayer value");
		g_eStealTechTargetPlayer = popupInfo.Data2;
	end

	OnTechPanelUpdated();
end);

------------------------------------------------------------------
-- Keyboard events
--- @param uiMsg integer
--- @param wParam any
--- @return true?
ContextPtr:SetInputHandler(function (uiMsg, wParam)
	if uiMsg == KeyEvents.KeyDown then
		if wParam == Keys.VK_ESCAPE then
			ClosePopup();
			return true;
		end
	end
end);

------------------------------------------------------------------
-- 'Active' (local human) player has changed
Events.GameplaySetActivePlayer.Add(function ()
	eActivePlayer = Game.GetActivePlayer();
	pActivePlayer = Players[eActivePlayer];
	eActiveTeam = Game.GetActiveTeam();
	pActiveTeam = Teams[eActiveTeam];

	OnTechPanelUpdated();

	if not ContextPtr:IsHidden() then
		ClosePopup();
	end
end);

------------------------------------------------------------------
-- Open the tech tree
Controls.OpenTTButton:RegisterCallback(Mouse.eLClick, function ()
	ClosePopup();
	Events.SerialEventGameMessagePopup{Type = ButtonPopupTypes.BUTTONPOPUP_TECH_TREE, Data2 = g_eStealTechTargetPlayer};
end);
