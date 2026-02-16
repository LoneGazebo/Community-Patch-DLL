print("This is the modded CityView from Community Patch");

include("IconSupport");
include("InstanceManager");
include("SupportFunctions");
include("TutorialPopupScreen");
include("InfoTooltipInclude");
include("FLuaVector");
include("VPUI_core");
include("CPK.lua");

local L = Locale.Lookup;
local VP = MapModData.VP;
local GameInfoCache = VP.GameInfoCache;
local GetGreatPersonInfoFromSpecialist = VP.GetGreatPersonInfoFromSpecialist;
local IconHookupOrDefault = VP.IconHookupOrDefault;
local Hide = CPK.UI.Control.Hide;
local Show = CPK.UI.Control.Show;
local Enable = CPK.UI.Control.Enable;
local Disable = CPK.UI.Control.Disable;
local Refresh = CPK.UI.Control.Refresh;
local CustomModOptionEnabled = CPK.Misc.CustomModOptionEnabled;

local MOD_BALANCE_VP = CustomModOptionEnabled("BALANCE_VP");

local buildingInstanceManager = InstanceManager:new("BuildingInstance", "BuildingButton", Controls.BuildingStack);
local gpInstanceManager = InstanceManager:new("GPInstance", "GPBox", Controls.GPStack);
local slackerInstanceManager = InstanceManager:new("SlackerInstance", "SlackerButton", Controls.SlackerStack);
local plotButtonInstanceManager = InstanceManager:new("PlotButtonInstance", "PlotButtonAnchor", Controls.PlotButtonContainer);
local buyPlotButtonInstanceManager = InstanceManager:new("BuyPlotButtonInstance", "BuyPlotButtonAnchor", Controls.PlotButtonContainer);

local tooltipInstance = {};
TTManager:GetTypeControlTable("CityViewTooltip", tooltipInstance);

local NORMAL_WORLD_POSITION_OFFSET = {x = 0, y = 0, z = 30};
local NORMAL_WORLD_POSITION_OFFSET2 = {x = 0, y = 35, z = 0};
local STRATEGIC_VIEW_WORLD_POSITION_OFFSET = {x = 0, y = 20, z = 0};
local MAX_SPECIALIST_SLOTS = 3;
local MAX_GREAT_WORK_SLOTS = 4;
local EMPTY_FUNCTION = function () end;

local FREE_STRING = L("TXT_KEY_FREE");
local SELL_BUILDING_STRING = L("TXT_KEY_CLICK_TO_SELL");
local BURNING_STRING = L("TXT_KEY_BURNING");
local SEPARATOR_STRING = "----------------";

local g_vWorldPositionOffset = NORMAL_WORLD_POSITION_OFFSET;
local g_vWorldPositionOffset2 = NORMAL_WORLD_POSITION_OFFSET2;

local iProductionPortraitSize = Controls.ProductionPortrait:GetSize().x;
local _, iScreenHeight = UIManager:GetScreenSizeVal();

local g_tPediaSearchStrings = {};

local g_pCurrentCity;
local g_tSpecialistAssignment = {};

local g_eBuildingToSell = -1;

---------------------------------------------------------------
--- Shorthand for setting a custom tooltip and adjusting its size
--- @param tooltip TooltipInstance
--- @param strTooltip string
local function SetTooltip(tooltip, strTooltip)
	tooltip.CityViewTooltipText:SetText(strTooltip);
	tooltip.CityViewTooltipGrid:DoAutoSize();
end

---------------------------------------------------------------
-- Set up update triggers
-- Instead of updating instantly, use SetUpdate to update every few frames when update is needed

--- @class UpdateType
--- @field UpdateFlag boolean
--- @field ScheduleFunc fun()
--- @field UpdateFunc fun()

--- @type {[string]: UpdateType}
local UpdateTypes = {
	FULLVIEW = {UpdateFlag = false, ScheduleFunc = EMPTY_FUNCTION, UpdateFunc = EMPTY_FUNCTION,},
	WORKERS = {UpdateFlag = false, ScheduleFunc = EMPTY_FUNCTION, UpdateFunc = EMPTY_FUNCTION,},
	SPECIALISTS = {UpdateFlag = false, ScheduleFunc = EMPTY_FUNCTION, UpdateFunc = EMPTY_FUNCTION,},
	UNEMPLOYED = {UpdateFlag = false, ScheduleFunc = EMPTY_FUNCTION, UpdateFunc = EMPTY_FUNCTION,},
	BUILDINGS = {UpdateFlag = false, ScheduleFunc = EMPTY_FUNCTION, UpdateFunc = EMPTY_FUNCTION,},
	HEXES = {UpdateFlag = false, ScheduleFunc = EMPTY_FUNCTION, UpdateFunc = EMPTY_FUNCTION,},
};

local function DoUpdate()
	ContextPtr:ClearUpdate();
	for _, t in pairs(UpdateTypes) do
		if t.UpdateFlag then
			t.UpdateFunc();
			t.UpdateFlag = false;
		end
	end
end

for _, t in pairs(UpdateTypes) do
	t.ScheduleFunc = function ()
		t.UpdateFlag = true;
		ContextPtr:SetUpdate(DoUpdate);
	end
end

-- Special case for full update
UpdateTypes.FULLVIEW.ScheduleFunc = function ()
	UpdateTypes.FULLVIEW.UpdateFlag = true;
	UpdateTypes.WORKERS.UpdateFlag = false;
	UpdateTypes.SPECIALISTS.UpdateFlag = false;
	UpdateTypes.UNEMPLOYED.UpdateFlag = false;
	UpdateTypes.BUILDINGS.UpdateFlag = false;
	ContextPtr:SetUpdate(DoUpdate);
end

---------------------------------------------------------------
--- @class OrderTypeDetail
--- @field TableName string
--- @field HelpTextFunc fun(eType: integer, pCity: City?): string
--- @field TurnsLeftFunc (fun(eType: integer, pCity: City, iQueueIndex: integer): integer)?

--- @type table<OrderType, OrderTypeDetail>
local tOrderTypeDetails = {
	[OrderTypes.ORDER_CONSTRUCT] = {
		TableName = "Buildings",
		HelpTextFunc = function (eType, pCity)
			return GetHelpTextForBuilding(eType, false, nil, false, pCity);
		end,
		TurnsLeftFunc = function (eType, pCity, iQueueIndex)
			return pCity:GetBuildingProductionTurnsLeft(eType, iQueueIndex - 1);
		end,
	},
	[OrderTypes.ORDER_CREATE] = {
		TableName = "Projects",
		HelpTextFunc = function (eType, pCity)
			return GetHelpTextForProject(eType, pCity);
		end,
		TurnsLeftFunc = function (eType, pCity, iQueueIndex)
			return pCity:GetProjectProductionTurnsLeft(eType, iQueueIndex - 1);
		end,
	},
	[OrderTypes.ORDER_MAINTAIN] = {
		TableName = "Processes",
		HelpTextFunc = function (eType)
			return GetHelpTextForProcess(eType);
		end,
	},
	[OrderTypes.ORDER_TRAIN] = {
		TableName = "Units",
		HelpTextFunc = function (eType, pCity)
			return GetHelpTextForUnit(eType, false, pCity);
		end,
		TurnsLeftFunc = function (eType, pCity, iQueueIndex)
			return pCity:GetUnitProductionTurnsLeft(eType, iQueueIndex - 1);
		end,
	},
};

--- @enum SpecialistState
local SpecialistStates = {
	EMPTY = 0,
	ASSIGNED = 1,
	LOCKED = 2,
};

--- @type table<SpecialistState, table<string, string>>
local tSpecialistTextures = {
	[SpecialistStates.EMPTY] = {
		SPECIALIST_SCIENTIST = "CitizenEmpty.dds",
		SPECIALIST_ENGINEER = "CitizenEmpty.dds",
		SPECIALIST_MERCHANT = "CitizenEmpty.dds",
		SPECIALIST_WRITER = "CitizenEmpty.dds",
		SPECIALIST_ARTIST = "CitizenEmpty.dds",
		SPECIALIST_MUSICIAN = "CitizenEmpty.dds",
		SPECIALIST_CIVIL_SERVANT = "CitizenEmpty.dds",
		SPECIALIST_CITIZEN = "CitizenEmpty.dds",
	},
	[SpecialistStates.ASSIGNED] = {
		SPECIALIST_SCIENTIST = "citizenScientist.dds",
		SPECIALIST_ENGINEER = "citizenEngineer.dds",
		SPECIALIST_MERCHANT = "citizenMerchant.dds",
		SPECIALIST_WRITER = "citizenwriter.dds",
		SPECIALIST_ARTIST = "citizenArtist.dds",
		SPECIALIST_MUSICIAN = "citizenmusician.dds",
		SPECIALIST_CIVIL_SERVANT = "citizenCivilServant.dds",
		SPECIALIST_CITIZEN = "citizenWorker.dds",
	},
	[SpecialistStates.LOCKED] = {
		SPECIALIST_SCIENTIST = "locked_scientist.dds",
		SPECIALIST_ENGINEER = "locked_engineer.dds",
		SPECIALIST_MERCHANT = "locked_merchant.dds",
		SPECIALIST_WRITER = "locked_writer.dds",
		SPECIALIST_ARTIST = "locked_artist.dds",
		SPECIALIST_MUSICIAN = "locked_musician.dds",
		SPECIALIST_CIVIL_SERVANT = "locked_servant.dds",
	},
};

--- @type table<CityAIFocusType, string>
local tFocusButtons = {
	[CityAIFocusTypes.NO_CITY_AI_FOCUS_TYPE] = "BalancedFocusButton",
	[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_FOOD] = "FoodFocusButton",
	[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_PRODUCTION] = "ProductionFocusButton",
	[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_GOLD] = "GoldFocusButton",
	[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_SCIENCE] = "ResearchFocusButton",
	[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_CULTURE] = "CultureFocusButton",
	[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_GREAT_PEOPLE] = "GPFocusButton",
	[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_FAITH] = "FaithFocusButton",
};

--- @class HeaderDef
--- @field Label string
--- @field LabelText string
--- @field OuterStack string?
--- @field InnerStack string?
--- @field HeadingOpen boolean
--- @field UpdateType UpdateType?

--- @type table<string, HeaderDef>
local tRightSideSections = {
	WorkerHeader = {
		Label = "WorkerHeaderLabel",
		LabelText = L("TXT_KEY_CITYVIEW_CITIZEN_ALLOCATION"),
		OuterStack = "WorkerManagementBox",
		HeadingOpen = OptionsManager.IsNoCitizenWarning(),
	},
	SlackerHeader = {
		Label = "SlackerHeaderLabel",
		LabelText = L("TXT_KEY_CITYVIEW_UNEMPLOYED_TEXT"),
		OuterStack = "BoxOSlackers",
		HeadingOpen = true,
	},
	GPHeader = {
		Label = "GPHeaderLabel",
		LabelText = L("TXT_KEY_CITYVIEW_GREAT_PEOPLE_TEXT"),
		OuterStack = "GPStack",
		HeadingOpen = true,
	},
	SpecialistBuildingHeader = {
		Label = "SpecialistBuildingHeaderLabel",
		LabelText = L("TXT_KEY_CITYVIEW_SPECIAL_TEXT"),
		InnerStack = "SpecialistStack",
		HeadingOpen = true,
	},
	WonderHeader = {
		Label = "WonderHeaderLabel",
		LabelText = L("TXT_KEY_CITYVIEW_WONDERS_TEXT"),
		InnerStack = "WonderStack",
		HeadingOpen = true,
	},
	GreatWorkBuildingHeader = {
		Label = "GreatWorkBuildingHeaderLabel",
		LabelText = L("TXT_KEY_CITYVIEW_GREAT_WORK_BUILDINGS_TEXT"),
		InnerStack = "GreatWorkStack",
		HeadingOpen = true,
	},
	CorporationHeader = {
		Label = "CorporationHeaderLabel",
		LabelText = L("TXT_KEY_CITYVIEW_CORPORATIONS_TEXT"),
		InnerStack = "CorporationStack",
		HeadingOpen = true,
	},
	OtherBuildingHeader = {
		Label = "OtherBuildingHeaderLabel",
		LabelText = L("TXT_KEY_CITYVIEW_REGULARBUILDING_TEXT"),
		InnerStack = "OtherBuildingStack",
		HeadingOpen = true,
	},
};

local eActivePlayer = Game.GetActivePlayer();
local pActivePlayer = Players[eActivePlayer];

local bProductionQueueOpen = false;

---------------------------------------------------------------
-- Clear out the UI so that when a player changes, the next update doesn't show the previous player's values for a frame
local function ClearCityUIInfo()
	Hide(Controls.b1number, Controls.b1down, Controls.b1remove);
	Hide(Controls.b2box, Controls.b3box, Controls.b4box, Controls.b5box, Controls.b6box);
	Hide(Controls.ProductionPortraitButton, Controls.ProductionHelp);
	Controls.ProductionItemName:SetText("");
end

---------------------------------------------------------------
local g_tButtonOrders = {};
local g_iSortOrder = 0;
local function CVSortFunction(a, b)
	local aVal = g_tButtonOrders[tostring(a)];
	local bVal = g_tButtonOrders[tostring(b)];

	if not aVal or not bVal then
		if aVal and not bVal then
			return false;
		elseif not aVal and bVal then
			return true;
		else
			return tostring(a) < tostring(b); -- gotta do something deterministic
		end
	else
		return aVal < bVal;
	end
end

local function SortByName(a, b)
	return a.Name < b.Name;
end

---------------------------------------------------------------
local function GetPedia(_, _, button)
	Events.SearchForPediaEntry(g_tPediaSearchStrings[tostring(button)]);
end

---------------------------------------------------------------
local function RefreshRightStack()
	Refresh(Controls.BuildingStack, Controls.WorkerManagementBox, Controls.SpecialistStack, Controls.GPStack, Controls.SlackerStack);
	Controls.BoxOSlackers:SetSizeY(Controls.SlackerStack:GetSizeY());
	Refresh(Controls.RightStack);

	Controls.BuildingListBackground:SetSizeY(math.max(math.min(iScreenHeight + 30, Controls.RightStack:GetSizeY() + 85), 160));

	Controls.RightScrollPanel:SetSizeY(math.min(iScreenHeight - 65, Controls.RightStack:GetSizeY() + 85));
	Controls.RightScrollPanel:CalculateInternalSize();
	Controls.RightScrollPanel:ReprocessAnchoring();

	-- Disable controls if in viewing only mode
	local bViewingMode = UI.IsCityScreenViewingMode();
	for _, strButtonName in pairs(tFocusButtons) do
		Controls[strButtonName]:SetDisabled(bViewingMode);
	end
	if bViewingMode then
		Disable(Controls.AvoidGrowthButton, Controls.ResetButton);
		Disable(Controls.BoxOSlackers, Controls.NoAutoSpecialistCheckbox, Controls.NoAutoSpecialistCheckbox2, Controls.ResetSpecialistsButton);
	else
		Enable(Controls.AvoidGrowthButton, Controls.ResetButton);
		Enable(Controls.BoxOSlackers, Controls.NoAutoSpecialistCheckbox, Controls.NoAutoSpecialistCheckbox2, Controls.ResetSpecialistsButton);
	end
end

---------------------------------------------------------------
--- @param pCity City
--- @param kBuildingInfo Info
local function AddBuildingButton(pCity, kBuildingInfo)
	local eBuilding = kBuildingInfo.ID;
	local instance = buildingInstanceManager:GetInstance();
	local bViewingMode = UI.IsCityScreenViewingMode();

	local tGreatWorkSlotControls = {
		instance.BuildingGreatWorkSlot1,
		instance.BuildingGreatWorkSlot2,
		instance.BuildingGreatWorkSlot3,
		instance.BuildingGreatWorkSlot4,
	};

	local tSpecialistSlotControls = {
		instance.BuildingSpecialistSlot1,
		instance.BuildingSpecialistSlot2,
		instance.BuildingSpecialistSlot3,
	};

	g_iSortOrder = g_iSortOrder + 1;
	g_tButtonOrders[tostring(instance.BuildingButton)] = g_iSortOrder;

	local bIsBuildingFree = pCity:GetNumFreeBuilding(eBuilding) > 0;

	-- Name
	local strBuildingName = L(kBuildingInfo.Description);
	g_tPediaSearchStrings[tostring(instance.BuildingButton)] = strBuildingName;
	instance.BuildingButton:RegisterCallback(Mouse.eRClick, GetPedia);

	-- Building is free, add an asterisk to the name
	if bIsBuildingFree then
		strBuildingName = string.format("%s (%s)", strBuildingName, FREE_STRING);
	end

	instance.BuildingName:SetText(strBuildingName);

	IconHookupOrDefault(kBuildingInfo.PortraitIndex, 64, kBuildingInfo.IconAtlas, instance.BuildingImage);

	-- Can it hold any great works?
	if kBuildingInfo.GreatWorkSlotType and kBuildingInfo.GreatWorkCount > 0 then
		local kGreatWorkSlotInfo = GameInfo.GreatWorkSlots[kBuildingInfo.GreatWorkSlotType];
		local eBuildingClass = GameInfoTypes[kBuildingInfo.BuildingClass];

		local iThemeBonus = pCity:GetThemingBonus(eBuildingClass);
		if iThemeBonus > 0 then
			instance.ThemeBonus:SetText("+" .. iThemeBonus);
			instance.ThemeBonus:SetToolTipString(pCity:GetThemingTooltip(eBuildingClass));
			Show(instance.ThemeBonus);
		else
			Hide(instance.ThemeBonus);
		end

		for i = 1, kBuildingInfo.GreatWorkCount do
			local slotButton = tGreatWorkSlotControls[i];
			slotButton:ClearCallback(Mouse.eLClick);
			local eGreatWork = pCity:GetBuildingGreatWork(eBuildingClass, i - 1);
			if eGreatWork ~= -1 then
				slotButton:SetToolTipString(Game.GetGreatWorkTooltip(eGreatWork, pCity:GetOwner()));
				local kGreatWorkInfo = GameInfo.GreatWorks[Game.GetGreatWorkType(eGreatWork)];
				if kGreatWorkInfo.GreatWorkClassType == "GREAT_WORK_ARTIFACT" then
					slotButton:SetTexture("GreatWorks_Artifact.dds");
				else
					slotButton:SetTexture(kGreatWorkSlotInfo.FilledIcon);
					slotButton:RegisterCallback(Mouse.eLClick, function()
						Events.SerialEventGameMessagePopup({
							Type = ButtonPopupTypes.BUTTONPOPUP_GREAT_WORK_COMPLETED_ACTIVE_PLAYER,
							Data1 = eGreatWork,
						});
					end);
				end
			else
				slotButton:SetTexture(kGreatWorkSlotInfo.EmptyIcon);
				slotButton:LocalizeAndSetToolTip(kGreatWorkSlotInfo.EmptyToolTipText);
			end
		end
	end

	-- Finally, show or hide great work slots
	for i = 1, MAX_GREAT_WORK_SLOTS do
		tGreatWorkSlotControls[i]:SetHide(i > kBuildingInfo.GreatWorkCount);
	end

	-- Can it have specialists?
	if kBuildingInfo.SpecialistType and kBuildingInfo.SpecialistCount > 0 then
		local iNumSpecialists = kBuildingInfo.SpecialistCount;
		if iNumSpecialists > MAX_SPECIALIST_SLOTS then
			print(string.format("Warning: More than %d specialists is not supported in the UI!", MAX_SPECIALIST_SLOTS));
			iNumSpecialists = MAX_SPECIALIST_SLOTS;
		end

		-- Populate specialist slot status
		local iNumAssignedSpecialists = pCity:GetNumSpecialistsInBuilding(eBuilding);
		local iNumLockedSpecialists = pCity:GetNumForcedSpecialistsInBuilding(eBuilding);

		g_tSpecialistAssignment[eBuilding] = {};
		for i = 1, iNumSpecialists do
			if i <= iNumLockedSpecialists then
				g_tSpecialistAssignment[eBuilding][i] = SpecialistStates.LOCKED;
			elseif i <= iNumAssignedSpecialists then
				g_tSpecialistAssignment[eBuilding][i] = SpecialistStates.ASSIGNED;
			else
				g_tSpecialistAssignment[eBuilding][i] = SpecialistStates.EMPTY;
			end
		end

		local eSpecialist = GameInfoTypes[kBuildingInfo.SpecialistType];
		local kSpecialistInfo = GameInfo.Specialists[eSpecialist];
		local strTooltip, strEmptyTooltip = GetHelpTextForSpecialist(eSpecialist, pCity);

		for iSlotIndex, eSpecialistState in ipairs(g_tSpecialistAssignment[eBuilding]) do
			local slotButton = tSpecialistSlotControls[iSlotIndex];
			slotButton:SetTexture(tSpecialistTextures[eSpecialistState][kBuildingInfo.SpecialistType]);

			if eSpecialistState == SpecialistStates.EMPTY then
				slotButton:SetToolTipString(strEmptyTooltip);
			else
				slotButton:SetToolTipString(strTooltip);
			end

			if bViewingMode then
				slotButton:ClearCallback(Mouse.eLClick);
			else
				slotButton:RegisterCallback(Mouse.eLClick, function ()
					if g_tSpecialistAssignment[eBuilding][iSlotIndex] == SpecialistStates.EMPTY and pCity:IsCanAddSpecialistToBuilding(eBuilding) then
						Game.SelectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_DO_TASK, TaskTypes.TASK_ADD_SPECIALIST, eSpecialist, eBuilding);
						g_tSpecialistAssignment[eBuilding][iSlotIndex] = SpecialistStates.LOCKED;
					elseif pCity:GetNumSpecialistsInBuilding(eBuilding) > 0 then
						local bLocked = (g_tSpecialistAssignment[eBuilding][iSlotIndex] == SpecialistStates.LOCKED);
						Game.SelectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_DO_TASK, TaskTypes.TASK_UNSLOT_SPECIALIST, eSpecialist, eBuilding, bLocked);
						g_tSpecialistAssignment[eBuilding][iSlotIndex] = SpecialistStates.EMPTY;
					end
				end);
			end

			g_tPediaSearchStrings[tostring(slotButton)] = L(kSpecialistInfo.Description);
			slotButton:RegisterCallback(Mouse.eRClick, GetPedia);
		end
	end

	-- Finally, show or hide specialist slots
	for i = 1, MAX_SPECIALIST_SLOTS do
		tSpecialistSlotControls[i]:SetHide(i > kBuildingInfo.SpecialistCount);
	end

	-- Can we sell this thing?
	local bSellable = false;
	if pCity:IsBuildingSellable(eBuilding) then
		bSellable = true;
		instance.BuildingButton:RegisterCallback(Mouse.eLClick, function ()
			if not pActivePlayer:IsTurnActive() then
				return;
			end

			-- Can this building even be sold?
			if not pCity:IsBuildingSellable(eBuilding) then
				return;
			end

			local iRefund = pCity:GetSellBuildingRefund(eBuilding);
			local iMaintenance = GameInfo.Buildings[eBuilding].GoldMaintenance;
			Controls.SellBuildingPopupText:LocalizeAndSetText("TXT_KEY_SELL_BUILDING_INFO", iRefund, iMaintenance);

			g_eBuildingToSell = eBuilding;
			Show(Controls.SellBuildingConfirm);
		end);
	-- We have to clear the click event here or the button may retain the callback from another city!
	else
		instance.BuildingButton:ClearCallback(Mouse.eLClick);
	end

	-- Set tooltip on mouseover
	instance.BuildingButton:SetToolTipCallback(function ()
		-- We can't be sure that the original city pointer is still valid here
		local pSelectedCity = UI.GetHeadSelectedCity();
		local strBuildingTooltip = GetHelpTextForBuilding(eBuilding, false, nil, bIsBuildingFree, pSelectedCity);
		if bSellable then
			strBuildingTooltip = string.format("%s[NEWLINE][NEWLINE]%s", strBuildingTooltip, SELL_BUILDING_STRING);
		end
		SetTooltip(tooltipInstance, strBuildingTooltip);
	end);

	-- Viewing Mode only
	instance.BuildingButton:SetDisabled(bViewingMode);
end

---------------------------------------------------------------
local function PlotButtonClicked(iPlotIndex)
	if pActivePlayer:IsTurnActive() then
		local pCity = UI.GetHeadSelectedCity();
		assert(pCity, "PlotButtonClicked: No city selected? How could this happen?");
		Network.SendDoTask(pCity:GetID(), TaskTypes.TASK_CHANGE_WORKING_PLOT, iPlotIndex, -1, false);
	end
end

-- Calling PlotButtonClicked with index 0 resets all forced tiles
Controls.ResetButton:SetVoid1(0);
Controls.ResetButton:RegisterCallback(Mouse.eLClick, PlotButtonClicked);

---------------------------------------------------------------
--- @param pCity City
--- @param ePlayer PlayerId
local function BuildProductionBox(pCity, ePlayer)

	---------------------------------------------------------------
	-- Automated production
	---------------------------------------------------------------
	Controls.AutomateProduction:SetCheck(pCity:IsProductionAutomated());
	Controls.AutomateProduction:SetHide(pCity:IsPuppet());

	---------------------------------------------------------------
	-- Current production
	---------------------------------------------------------------
	Controls.ProductionItemName:SetText(Locale.ToUpper(L(pCity:GetProductionNameKey())));

	local eUnitProduction = pCity:GetProductionUnit();
	local eBuildingProduction = pCity:GetProductionBuilding();
	local eProjectProduction = pCity:GetProductionProject();
	local eProcessProduction = pCity:GetProductionProcess();
	local strHelp;
	local iPortraitIndex;
	local strAtlas;

	if eUnitProduction ~= -1 then
		iPortraitIndex, strAtlas = UI.GetUnitPortraitIcon(eUnitProduction, ePlayer);
		strHelp = GetHelpTextForUnit(eUnitProduction, false, pCity);
	elseif eBuildingProduction ~= -1 then
		local kBuildingInfo = GameInfo.Buildings[eBuildingProduction];
		strHelp = GetHelpTextForBuilding(eBuildingProduction, true, _, false, pCity);
		iPortraitIndex = kBuildingInfo.PortraitIndex;
		strAtlas = kBuildingInfo.IconAtlas;
	elseif eProjectProduction ~= -1 then
		local kProjectInfo = GameInfo.Projects[eProjectProduction];
		strHelp = GetHelpTextForProject(eProjectProduction);
		iPortraitIndex = kProjectInfo.PortraitIndex;
		strAtlas = kProjectInfo.IconAtlas;
	elseif eProcessProduction ~= -1 then
		local kProcessInfo = GameInfo.Processes[eProcessProduction];
		strHelp = GetHelpTextForProcess(eProcessProduction);
		iPortraitIndex = kProcessInfo.PortraitIndex;
		strAtlas = kProcessInfo.IconAtlas;
	end

	if strHelp then
		IconHookupOrDefault(iPortraitIndex, iProductionPortraitSize, strAtlas, Controls.ProductionPortrait);
		Controls.ProductionHelp:SetText(strHelp);
		Show(Controls.ProductionPortrait, Controls.ProductionHelp);
		Refresh(Controls.ProductionHelpScroll);
	else
		Hide(Controls.ProductionPortrait, Controls.ProductionHelp);
	end

	---------------------------------------------------------------
	-- Production yield
	---------------------------------------------------------------

	-- Production stored and needed
	local fProductionStored = pCity:GetProductionTimes100() / 100 + pCity:GetTotalOverflowProductionTimes100() / 100;
	local iProductionNeeded = pCity:GetProductionNeeded();

	-- Production per turn
	local fProductionPerTurn = pCity:GetYieldRateTimes100(YieldTypes.YIELD_PRODUCTION) / 100;
	Controls.ProductionOutput:LocalizeAndSetText("TXT_KEY_CITY_SCREEN_PROD_PER_TURN", fProductionPerTurn);

	-- Progress info for meter
	local bIsProcess = pCity:IsProductionProcess();
	local fProductionProgress = bIsProcess and 0 or fProductionStored / iProductionNeeded;
	local fProductionProgressNextTurn = bIsProcess and 0 or (fProductionStored + fProductionPerTurn) / iProductionNeeded;
	fProductionProgressNextTurn = math.min(fProductionProgressNextTurn, 1);
	Controls.ProductionMeter:SetPercents(fProductionProgress, fProductionProgressNextTurn);

	-- Turns left
	local strTurnsLeft = "";
	if bIsProcess or fProductionPerTurn == 0 then
		Controls.ProductionTurnsLabel:SetText("");
	else
		local iTurnsLeft = pCity:GetProductionTurnsLeft();
		if iTurnsLeft > 99 then
			strTurnsLeft = L("TXT_KEY_PRODUCTION_HELP_99PLUS_TURNS");
		else
			strTurnsLeft = L("TXT_KEY_PRODUCTION_HELP_NUM_TURNS", iTurnsLeft);
		end
		Controls.ProductionTurnsLabel:SetText(string.format("(%s)", strTurnsLeft));
	end
	Controls.ProductionTurnsLabel:SetHide(not strHelp);

	-- Tooltips
	local strProductionTooltip = GetProductionTooltip(pCity);
	local tLines = {};

	if not strHelp then
		table.insert(tLines, L("TXT_KEY_PRODUCTION_HELP_NOTHING"));
	elseif not pCity:IsProductionProcess() then
		table.insert(tLines, L("TXT_KEY_PRODUCTION_HELP_TEXT", pCity:GetProductionNameKey(), strTurnsLeft));
		table.insert(tLines, SEPARATOR_STRING);
		table.insert(tLines, L("TXT_KEY_PRODUCTION_PROGRESS", fProductionStored, iProductionNeeded));
	end

	table.insert(tLines, strProductionTooltip);

	Controls.ProductionPortraitButton:SetToolTipString(table.concat(tLines, "[NEWLINE]"));
	Controls.ProdPerTurnLabel:LocalizeAndSetText("TXT_KEY_CITYVIEW_PERTURN_TEXT", math.floor(fProductionPerTurn));
	Controls.ProdBox:SetToolTipString(strProductionTooltip);

	---------------------------------------------------------------
	-- Production queue
	---------------------------------------------------------------

	--- @param iQueueIndex integer
	--- @return boolean
	local function UpdateThisQueuedItem(iQueueIndex)
		local strPrefix = "b" .. tostring(iQueueIndex);
		local box = Controls[strPrefix .. "box"];
		local image = Controls[strPrefix .. "image"];
		local nameLabel = Controls[strPrefix .. "name"];
		local turnsLabel = Controls[strPrefix .. "turns"];
		local bGeneratingProduction = (pCity:GetYieldRateTimes100(YieldTypes.YIELD_PRODUCTION) > 0);

		local eOrder, eItem = pCity:GetOrderFromQueue(iQueueIndex - 1);
		local tOrder = tOrderTypeDetails[eOrder];
		local kInfo = GameInfo[tOrder.TableName][eItem];

		local iItemPortraitIndex = kInfo.PortraitIndex;
		local strItemAtlas = kInfo.IconAtlas;
		if eOrder == OrderTypes.ORDER_TRAIN then
			iItemPortraitIndex, strItemAtlas = UI.GetUnitPortraitIcon(eItem, ePlayer);
		end
		IconHookupOrDefault(iItemPortraitIndex, 45, strItemAtlas, image);

		nameLabel:LocalizeAndSetText(kInfo.Description);

		if eOrder == OrderTypes.ORDER_MAINTAIN then
			Hide(turnsLabel);
		else
			Show(turnsLabel);
			if bGeneratingProduction then
				turnsLabel:LocalizeAndSetText("TXT_KEY_PRODUCTION_HELP_NUM_TURNS", tOrder.TurnsLeftFunc(eItem, pCity, iQueueIndex));
			else
				turnsLabel:LocalizeAndSetText("TXT_KEY_PRODUCTION_HELP_INFINITE_TURNS");
			end
		end

		box:SetToolTipCallback(function ()
			-- We can't be sure that the original city pointer is still valid here
			local pSelectedCity = UI.GetHeadSelectedCity();
			SetTooltip(tooltipInstance, tOrder.HelpTextFunc(eItem, pSelectedCity));
		end);

		Show(box);
		return (eOrder == OrderTypes.ORDER_MAINTAIN);
	end

	local iQueueLength = pCity:GetOrderQueueLength();
	Controls.HideQueueButton:SetHide(iQueueLength == 0);

	-- Hide the queue buttons
	Hide(Controls.b1number, Controls.b1down, Controls.b1remove);
	Hide(Controls.b2box, Controls.b3box, Controls.b4box, Controls.b5box, Controls.b6box);

	local bAnyProcess = false;

	Show(Controls.ProductionPortraitButton);

	local vGridSize = Controls.ProdQueueBackground:GetSize();
	if bProductionQueueOpen and iQueueLength > 0 then
		vGridSize.y = 470;
		Controls.ProductionButtonLabel:LocalizeAndSetText("TXT_KEY_CITYVIEW_QUEUE_PROD");
		Controls.ProductionButton:LocalizeAndSetToolTip("TXT_KEY_CITYVIEW_QUEUE_PROD_TT");

		-- Show the queue buttons
		Show(Controls.b1number, Controls.b1remove);
		if iQueueLength > 1 then
			Show(Controls.b1down);
		end
		for i = 2, iQueueLength do
			local bQueuedProcess = UpdateThisQueuedItem(i);

			-- Update the up and down buttons
			Controls["b" .. tostring(i) .. "down"]:SetHide(iQueueLength == 1);
			Controls["b" .. tostring(i) .. "up"]:SetHide(bQueuedProcess);

			if bQueuedProcess then -- process cannot be swapped
				bAnyProcess = true;
				Hide(Controls["b" .. tostring(i - 1) .. "down"]);
			end
		end
	else
		vGridSize.y = 280;
		if iQueueLength == 0 then
			Controls.ProductionButtonLabel:LocalizeAndSetText("TXT_KEY_CITYVIEW_CHOOSE_PROD");
			Controls.ProductionButton:LocalizeAndSetToolTip("TXT_KEY_CITYVIEW_CHOOSE_PROD_TT");
		else
			Controls.ProductionButtonLabel:LocalizeAndSetText("TXT_KEY_CITYVIEW_CHANGE_PROD");
			Controls.ProductionButton:LocalizeAndSetToolTip("TXT_KEY_CITYVIEW_CHANGE_PROD_TT");
		end
	end
	Controls.ProdQueueBackground:SetSize(vGridSize);
	Controls.ProductionButton:SetDisabled(bProductionQueueOpen and (iQueueLength >= 6 or bAnyProcess));
	if iQueueLength == 1 then
		Hide(Controls.b1remove);
	end

	if ePlayer == eActivePlayer then
		Enable(Controls.PurchaseButton);
	else
		Hide(Controls.AutomateProduction);
		Disable(Controls.ProductionButton, Controls.PurchaseButton);
	end
end

---------------------------------------------------------------
--- @param pCity City
local function BuildWorkerManagementBox(pCity)
	if tRightSideSections.WorkerHeader.HeadingOpen then
		Controls[tFocusButtons[pCity:GetFocusType()]]:SetCheck(true);
		Controls.AvoidGrowthButton:SetCheck(pCity:IsForcedAvoidGrowth());
		if pCity:GetNumForcedWorkingPlots() > 0 or pCity:GetSpecialistCount(GameDefines.DEFAULT_SPECIALIST) > 0 then
			Show(Controls.ResetButton, Controls.ResetFooter);
		else
			Hide(Controls.ResetButton, Controls.ResetFooter);
		end
		Events.RequestYieldDisplay(YieldDisplayTypes.CITY_OWNED, pCity:GetX(), pCity:GetY());
	else
		Events.RequestYieldDisplay(YieldDisplayTypes.CITY_WORKED, pCity:GetX(), pCity:GetY());
	end
end

UpdateTypes.WORKERS.UpdateFunc = RefreshRightStack;
tRightSideSections.WorkerHeader.UpdateType = UpdateTypes.WORKERS;

---------------------------------------------------------------
--- @param pCity City
--- @param ePlayer PlayerId
local function BuildGPMeter(pCity, ePlayer)
	gpInstanceManager:ResetInstances();
	local bHasGPProgress = false;
	local pPlayer = Players[ePlayer];

	if tRightSideSections.GPHeader.HeadingOpen then
		for eSpecialist, kSpecialistInfo in GameInfoCache("Specialists") do
			local iProgress = pCity:GetSpecialistGreatPersonProgress(eSpecialist);
			local iChange, strTooltip = pCity:GetSpecialistRate(eSpecialist, true);
			if iProgress > 0 or iChange > 0 then
				bHasGPProgress = true;

				local kGreatPersonInfo = GetGreatPersonInfoFromSpecialist(kSpecialistInfo.Type);
				local iNextGPCost = pPlayer:GetNextGreatPersonCost(kGreatPersonInfo.ID);
				local instance = gpInstanceManager:GetInstance();
				instance.GPMeter:SetPercent(iProgress / iNextGPCost);

				local strGreatPerson = L(kGreatPersonInfo.Description);
				instance.GreatPersonLabel:SetText(strGreatPerson);
				g_tPediaSearchStrings[tostring(instance.GPImage)] = strGreatPerson;
				instance.GPImage:RegisterCallback(Mouse.eRClick, GetPedia);

				instance.GPBox:SetToolTipString(strTooltip);

				local kUnitClassInfo = GameInfo.UnitClasses[kGreatPersonInfo.Class];
				local iPortraitOffset, strAtlas = UI.GetUnitPortraitIcon(GameInfoTypes[kUnitClassInfo.DefaultUnit], ePlayer);
				IconHookupOrDefault(iPortraitOffset, 64, strAtlas, instance.GPImage);
			end
		end
	end

	-- Hide the whole thing including header if there's no GPP in the city
	if bHasGPProgress then
		Show(Controls.GPHeader, Controls.GPStack);
	else
		Hide(Controls.GPHeader, Controls.GPStack);
	end
end

UpdateTypes.SPECIALISTS.UpdateFunc = RefreshRightStack;
tRightSideSections.GPHeader.UpdateType = UpdateTypes.SPECIALISTS;

---------------------------------------------------------------
--- @param pCity City
local function BuildUnemployedBox(pCity)
	slackerInstanceManager:ResetInstances();
	local eDefaultSpecialist = GameDefines.DEFAULT_SPECIALIST;
	local iNumUnemployed = pCity:GetSpecialistCount(eDefaultSpecialist);

	-- No unemployed citizens
	if iNumUnemployed == 0 then
		Hide(Controls.SlackerHeader, Controls.BoxOSlackers);
		return;
	end

	Show(Controls.SlackerHeader);
	if tRightSideSections.SlackerHeader.HeadingOpen then
		Show(Controls.BoxOSlackers);

		local function OnSlackersSelected()
			if pActivePlayer:IsTurnActive() then
				Network.SendDoTask(pCity:GetID(), TaskTypes.TASK_REMOVE_SLACKER, 0, -1, false);
			end
		end

		Controls.BoxOSlackers:RegisterCallback(Mouse.eLClick, OnSlackersSelected);

		local kSpecialistInfo = GameInfo.Specialists[eDefaultSpecialist];
		local strTooltip = GetHelpTextForSpecialist(eDefaultSpecialist, pCity);
		local strSpecialist = L(kSpecialistInfo.Description);

		-- Add unemployed instances
		for _ = 1, iNumUnemployed do
			local instance = slackerInstanceManager:GetInstance();
			instance.SlackerButton:SetToolTipString(strTooltip);
			instance.SlackerButton:RegisterCallback(Mouse.eLClick, OnSlackersSelected);
			g_tPediaSearchStrings[tostring(instance.SlackerButton)] = strSpecialist;
			instance.SlackerButton:RegisterCallback(Mouse.eRClick, GetPedia);
		end
	else
		Hide(Controls.BoxOSlackers);
	end
end

UpdateTypes.UNEMPLOYED.UpdateFunc = RefreshRightStack;
tRightSideSections.SlackerHeader.UpdateType = UpdateTypes.UNEMPLOYED;

---------------------------------------------------------------
--- @param pCity City
local function BuildBuildingBox(pCity)
	buildingInstanceManager:ResetInstances();

	-- Reset the building order list
	g_iSortOrder = 0;
	g_tButtonOrders = {};

	local strMaintenanceTooltip = L("TXT_KEY_BUILDING_MAINTENANCE_TT", pCity:GetTotalBaseBuildingMaintenance());

	-- Classify buildings in the city
	local tCorporationBuildings = {};
	local tWonders = {};
	local tSpecialistBuildings = {};
	local tGreatWorkBuildings = {};
	local tOtherBuildings = {};
	local bHasLockedSpecialists = false;
	local bHasSpecialistSlotsInWonders = false;
	for eBuilding, kBuildingInfo in GameInfoCache("Buildings") do
		if pCity:IsHasBuilding(eBuilding) then
			local kBuildingClassInfo = GameInfo.BuildingClasses[kBuildingInfo.BuildingClass];
			local building = {
				Name = L(kBuildingInfo.Description),
				ID = eBuilding,
			};
			-- Corporation buildings
			if kBuildingInfo.IsCorporation then
				table.insert(tCorporationBuildings, building);
			-- Wonders (excluding National Wonders with specialist slots)
			elseif kBuildingClassInfo.MaxGlobalInstances > 0
			or (kBuildingClassInfo.MaxPlayerInstances == 1 and kBuildingInfo.SpecialistCount == 0)
			or kBuildingClassInfo.MaxTeamInstances > 0 then
				table.insert(tWonders, building);
				if kBuildingInfo.SpecialistCount > 0 then
					bHasSpecialistSlotsInWonders = true;
				end
			-- Specialist buildings
			elseif kBuildingInfo.SpecialistCount > 0 then
				table.insert(tSpecialistBuildings, building);
				if not bHasLockedSpecialists and pCity:GetNumForcedSpecialistsInBuilding(eBuilding) > 0 then
					bHasLockedSpecialists = true;
				end
			-- Great Work buildings
			elseif kBuildingInfo.GreatWorkCount > 0 then
				table.insert(tGreatWorkBuildings, building);
			-- The rest of the visible buildings
			elseif kBuildingInfo.GreatWorkCount == 0 and not kBuildingInfo.IsDummy then
				table.insert(tOtherBuildings, building);
			-- Dummy buildings
			else
				-- Keep this block so other developers don't accidentally "fix" the above condition
				-- print("Dummy building detected:", L(kBuildingInfo.Description));
			end
		end
	end

	local tBuildingTables = {
		{HeaderName = "SpecialistBuildingHeader", BuildingList = tSpecialistBuildings},
		{HeaderName = "WonderHeader", BuildingList = tWonders},
		{HeaderName = "GreatWorkBuildingHeader", BuildingList = tGreatWorkBuildings},
		{HeaderName = "CorporationHeader", BuildingList = tCorporationBuildings},
		{HeaderName = "OtherBuildingHeader", BuildingList = tOtherBuildings},
	};

	-- Order is important
	for _, t in ipairs(tBuildingTables) do
		if next(t.BuildingList) then
			-- Header tooltip
			Controls[t.HeaderName]:SetToolTipString(strMaintenanceTooltip);

			-- Add header's corresponding stack to sorting list
			g_iSortOrder = g_iSortOrder + 1;
			g_tButtonOrders[tostring(Controls[tRightSideSections[t.HeaderName].InnerStack])] = g_iSortOrder;

			-- Show the stack and header
			Show(Controls[tRightSideSections[t.HeaderName].InnerStack]);

			-- Create building buttons only if the section is open
			if tRightSideSections[t.HeaderName].HeadingOpen then
				table.sort(t.BuildingList, SortByName);
				for _, building in ipairs(t.BuildingList) do
					local kBuildingInfo = GameInfo.Buildings[building.ID];
					AddBuildingButton(pCity, kBuildingInfo);
				end
			end
		else
			-- Hide the entire stack (which includes the header and everything)
			Hide(Controls[tRightSideSections[t.HeaderName].InnerStack]);
		end
	end

	-- Urbanization-free specialists
	if MOD_BALANCE_VP and next(tSpecialistBuildings) and tRightSideSections.SpecialistBuildingHeader.HeadingOpen then
		local iNumFreeSpecialists = pCity:GetRemainingFreeSpecialists();
		if iNumFreeSpecialists > 0 then
			Controls.FreeSpecialistLabel:SetText(iNumFreeSpecialists);
			Controls.FreeSpecialistLabelSuffix:LocalizeAndSetText("TXT_KEY_CITYVIEW_FREESPECIALIST_TEXT", iNumFreeSpecialists);
		else
			Controls.FreeSpecialistLabel:SetText(GameDefines.UNHAPPINESS_PER_SPECIALIST / 100);
			Controls.FreeSpecialistLabelSuffix:LocalizeAndSetText("TXT_KEY_CITYVIEW_NOFREESPECIALIST_TEXT");
		end
		Show(Controls.FreeSpecialistLabel, Controls.FreeSpecialistLabelSuffix);
	else
		Hide(Controls.FreeSpecialistLabel, Controls.FreeSpecialistLabelSuffix);
	end

	local bManualSpecialists = pCity:IsNoAutoAssignSpecialists();
	Controls.NoAutoSpecialistCheckbox:SetCheck(bManualSpecialists);
	Controls.NoAutoSpecialistCheckbox2:SetCheck(bManualSpecialists);

	-- Hide specialist controls when SpecialistBuildingHeader is closed
	if tRightSideSections.SpecialistBuildingHeader.HeadingOpen then
		Controls.ResetSpecialistsButton:SetHide(not bHasLockedSpecialists);
		Controls.ResetSpecialistsFooter:SetHide(not bHasLockedSpecialists);
		Show(Controls.SpecialistControlBox);
	else
		Hide(Controls.SpecialistControlBox, Controls.ResetSpecialistsButton, Controls.ResetSpecialistsFooter);
	end

	-- Hide wonder specialist controls when WonderHeader is closed
	if tRightSideSections.WonderHeader.HeadingOpen then
		Controls.SpecialistControlBox2:SetHide(not bHasSpecialistSlotsInWonders);
	else
		Hide(Controls.SpecialistControlBox2);
	end

	-- Reorder headers, extra buttons, and building buttons by sorting list
	Controls.BuildingStack:SortChildren(CVSortFunction);
end

-- Update function that rebuilds buildings and refreshes the right stack
local function UpdateBuildings()
	local pCity = UI.GetHeadSelectedCity();
	if pCity then
		BuildBuildingBox(pCity);
		RefreshRightStack();
	end
end

UpdateTypes.BUILDINGS.UpdateFunc = UpdateBuildings;
tRightSideSections.SpecialistBuildingHeader.UpdateType = UpdateTypes.BUILDINGS;
tRightSideSections.CorporationHeader.UpdateType = UpdateTypes.BUILDINGS;
tRightSideSections.WonderHeader.UpdateType = UpdateTypes.BUILDINGS;
tRightSideSections.OtherBuildingHeader.UpdateType = UpdateTypes.BUILDINGS;
tRightSideSections.GreatWorkBuildingHeader.UpdateType = UpdateTypes.BUILDINGS;

---------------------------------------------------------------
local function UpdateWorkingHexes()
	local pCity = UI.GetHeadSelectedCity();
	if not pCity then
		return;
	end

	if not UI.IsCityScreenUp() then
		return;
	end

	local iNumPlotsInWorkingRange = pCity:GetNumCityPlots();
	local ePlayer = pCity:GetOwner();
	local bPurchasePlot = (UI.GetInterfaceMode() == InterfaceModeTypes.INTERFACEMODE_PURCHASE_PLOT);
	if bPurchasePlot then
		Events.RequestYieldDisplay(YieldDisplayTypes.CITY_PURCHASABLE, pCity:GetX(), pCity:GetY());
	end

	plotButtonInstanceManager:ResetInstances();
	buyPlotButtonInstanceManager:ResetInstances();
	for iCityPlotIndex = 0, iNumPlotsInWorkingRange - 1 do
		local pPlot = pCity:GetCityIndexPlot(iCityPlotIndex);
		if pPlot then
			local vWorldPosition = HexToWorld(ToHexFromGrid(Vector2(pPlot:GetX(), pPlot:GetY())));

			-- Display worked plots
			if tRightSideSections.WorkerHeader.HeadingOpen and pPlot:GetOwner() == ePlayer then
				local iPortraitIndex;
				local strTooltipKey;
				if pCity:IsWorkingPlot(pPlot) then
					-- The city itself
					if iCityPlotIndex == 0 then
						iPortraitIndex = 11;
						strTooltipKey = "TXT_KEY_CITYVIEW_CITY_CENTER";
					-- FORCED worked plot
					elseif pCity:IsForcedWorkingPlot(pPlot) then
						iPortraitIndex = 10;
						strTooltipKey = "TXT_KEY_CITYVIEW_FORCED_WORK_TILE";
					-- AI-picked worked plot
					else
						iPortraitIndex = 0;
						strTooltipKey = "TXT_KEY_CITYVIEW_GUVNA_WORK_TILE";
					end
				else
					-- Owned and worked by another one of our cities
					if pPlot:GetWorkingCity():IsWorkingPlot(pPlot) then
						iPortraitIndex = 12;
						strTooltipKey = "TXT_KEY_CITYVIEW_NUTHA_CITY_TILE";
					-- Other workable plot
					elseif pPlot:GetWorkingCity():CanWork(pPlot) then
						iPortraitIndex = 9;
						strTooltipKey = "TXT_KEY_CITYVIEW_UNWORKED_CITY_TILE";
					-- Blockaded plot
					elseif pCity:IsPlotBlockaded(pPlot) then
						iPortraitIndex = 13;
						strTooltipKey = "TXT_KEY_CITYVIEW_BLOCKADED_CITY_TILE";
					-- Enemy unit standing here
					elseif pPlot:IsVisibleEnemyUnit(ePlayer) then
						iPortraitIndex = 13;
						strTooltipKey = "TXT_KEY_CITYVIEW_ENEMY_UNIT_CITY_TILE";
					end
				end

				if iPortraitIndex then
					local instance = plotButtonInstanceManager:GetInstance();
					instance.PlotButtonAnchor:SetWorldPosition(VecAdd(vWorldPosition, g_vWorldPositionOffset));
					IconHookupOrDefault(iPortraitIndex, 45, "CITIZEN_ATLAS", instance.PlotButtonImage);
					instance.PlotButtonImage:LocalizeAndSetToolTip(strTooltipKey);
					instance.PlotButtonImage:SetVoid1(iCityPlotIndex);
					instance.PlotButtonImage:RegisterCallback(Mouse.eLClick, PlotButtonClicked);
					instance.PlotButtonImage:SetDisabled(UI.IsCityScreenViewingMode());
				end
			end

			-- Add buy plot buttons
			if bPurchasePlot then
				local strTooltipKey;
				local bCanAfford = false;
				if pCity:CanBuyPlotAt(pPlot:GetX(), pPlot:GetY(), false) then
					bCanAfford = true;
					strTooltipKey = "TXT_KEY_CITYVIEW_CLAIM_NEW_LAND";
				elseif pCity:CanBuyPlotAt(pPlot:GetX(), pPlot:GetY(), true) then
					strTooltipKey = "TXT_KEY_CITYVIEW_NEED_MONEY_BUY_TILE";
				end

				if strTooltipKey then
					local instance = buyPlotButtonInstanceManager:GetInstance();
					instance.BuyPlotButtonAnchor:SetWorldPosition(VecAdd(vWorldPosition, g_vWorldPositionOffset2));
					local iPlotCost = pCity:GetBuyPlotCost(pPlot:GetX(), pPlot:GetY());
					instance.BuyPlotAnchoredButton:LocalizeAndSetToolTip(strTooltipKey, iPlotCost);
					instance.BuyPlotAnchoredButton:SetDisabled(not bCanAfford);
					if bCanAfford then
						instance.BuyPlotAnchoredButtonLabel:SetText(iPlotCost);
						instance.BuyPlotAnchoredButton:RegisterCallback(Mouse.eLClick, function ()
							if not pActivePlayer:IsTurnActive() then
								return;
							end
							Network.SendCityBuyPlot(pCity:GetID(), pPlot:GetX(), pPlot:GetY());
							UI.UpdateCityScreen();
							Events.AudioPlay2DSound("AS2D_INTERFACE_BUY_TILE");
						end);
					else
						instance.BuyPlotAnchoredButtonLabel:SetText("[COLOR_WARNING_TEXT]" .. iPlotCost .. "[ENDCOLOR]");
					end
				end
			end
		end
	end

	-- Highlight buyable plots
	for _, pBuyablePlot in pairs{pCity:GetBuyablePlotList()} do
		Events.SerialEventHexHighlight(ToHexFromGrid(Vector2(pBuyablePlot:GetX(), pBuyablePlot:GetY())), true, Vector4(1.0, 0.0, 1.0, 1));
	end
end

UpdateTypes.HEXES.UpdateFunc = UpdateWorkingHexes;
Events.SerialEventCityHexHighlightDirty.Add(UpdateTypes.HEXES.ScheduleFunc);

---------------------------------------------------------------
-- City View Update
---------------------------------------------------------------
local function UpdateViewFull()
	if ContextPtr:IsHidden() then
		return;
	end

	local pCity = UI.GetHeadSelectedCity();

	-- City has changed; reset specialist assignment table
	if g_pCurrentCity ~= pCity then
		g_pCurrentCity = pCity;
		g_tSpecialistAssignment = {};
	end

	if not pCity then
		return;
	end

	local bViewingMode = UI.IsCityScreenViewingMode();
	local ePlayer = pCity:GetOwner();
	local pPlayer = Players[pCity:GetOwner()];
	local bCapital = pCity:IsCapital();
	local bBlockaded = pCity:IsBlockaded();
	local bOccupied = pCity:IsOccupied();
	local bRazing = pCity:IsRazing();

	-- Clear it out to prevent memory leak
	g_tPediaSearchStrings = {};

	-------------------------------------------
	-- Top (city banner)
	-------------------------------------------

	-- City strength
	Controls.Defense:SetText(math.floor(pCity:GetStrengthValue() / 100));
	Controls.Defense:LocalizeAndSetToolTip("TXT_KEY_CITYVIEW_CITY_COMB_STRENGTH_TT", pCity:GetStrengthValue(true) / 100);

	-- Health bar
	local iDamage = pCity:GetDamage();
	if iDamage == 0 then
		Hide(Controls.HealthFrame);
	else
		local fHealthPercent = 1 - iDamage / pCity:GetMaxHitPoints();
		if fHealthPercent > 0.66 then
			Controls.HealthMeter:SetTexture("CityNamePanelHealthBarGreen.dds");
		elseif fHealthPercent > 0.33 then
			Controls.HealthMeter:SetTexture("CityNamePanelHealthBarYellow.dds");
		else
			Controls.HealthMeter:SetTexture("CityNamePanelHealthBarRed.dds");
		end
		Controls.HealthMeter:SetPercent(fHealthPercent);
		Show(Controls.HealthFrame);
	end

	-- City name
	local strCityName = L(pCity:GetNameKey());
	if bRazing then
		strCityName = string.format("%s (%s)", strCityName, BURNING_STRING);
	end
	strCityName = Locale.ToUpper(strCityName);
	local cityNameSize = math.abs(Controls.NextCityButton:GetOffsetX()) * 2 - Controls.PrevCityButton:GetSizeX();
	if bCapital then
		cityNameSize = cityNameSize - Controls.CityCapitalIcon:GetSizeX();
	end
	TruncateString(Controls.CityNameTitleBarLabel, cityNameSize, strCityName);

	if MOD_BALANCE_VP then
		Controls.CityNameTitleBarLabel:SetToolTipString(pCity:GetCityUnhappinessBreakdown(false));
	end

	Controls.CityCapitalIcon:SetHide(not bCapital);
	Refresh(Controls.TitleStack);

	if pPlayer:GetNumCities() <= 1 or bViewingMode then
		Disable(Controls.PrevCityButton, Controls.NextCityButton);
	else
		Enable(Controls.PrevCityButton, Controls.NextCityButton);
	end

	CivIconHookup(pPlayer:GetID(), 64, Controls.CivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true);

	Controls.EditButton:SetHide(bViewingMode or ePlayer ~= eActivePlayer);

	-- Icons
	if bCapital or bBlockaded then
		Hide(Controls.ConnectedIcon, Controls.IndustrialConnectedIcon);
	elseif pCity:IsIndustrialConnectedToCapital() then
		Show(Controls.IndustrialConnectedIcon);
		Hide(Controls.ConnectedIcon);
	elseif pCity:IsConnectedToCapital() then
		Show(Controls.ConnectedIcon);
		Hide(Controls.IndustrialConnectedIcon);
	else
		Hide(Controls.ConnectedIcon, Controls.IndustrialConnectedIcon);
	end

	if bBlockaded then
		local iSappedTurns = pCity:GetSappedTurns();
		if iSappedTurns > 0 then
			Controls.BlockadedIcon:SetText("[ICON_SAPPED]");
			Controls.BlockadedIcon:LocalizeAndSetToolTip("TXT_KEY_CITY_SAPPED", iSappedTurns);
		else
			Controls.BlockadedIcon:SetText("[ICON_BLOCKADED]");
			Controls.BlockadedIcon:LocalizeAndSetToolTip("TXT_KEY_CITY_BLOCKADED");
		end
		Show(Controls.BlockadedIcon);
	else
		Hide(Controls.BlockadedIcon);
	end

	if bRazing then
		Show(Controls.RazingIcon);
		Controls.RazingIcon:LocalizeAndSetToolTip("TXT_KEY_CITY_BURNING", pCity:GetRazingTurns());
	else
		Hide(Controls.RazingIcon);
	end

	if pCity:IsResistance() then
		Show(Controls.ResistanceIcon);
		Controls.ResistanceIcon:LocalizeAndSetToolTip("TXT_KEY_CITY_RESISTANCE", pCity:GetResistanceTurns());
	else
		Hide(Controls.ResistanceIcon);
	end

	Controls.PuppetIcon:SetHide(not pCity:IsPuppet());

	Controls.OccupiedIcon:SetHide(not bOccupied or pCity:IsNoOccupiedUnhappiness());

	-- Resource demanded
	local strResourceDemanded;
	local eResourceDemanded = pCity:GetResourceDemanded();
	if eResourceDemanded ~= -1 then
		local kResourceInfo = GameInfo.Resources[eResourceDemanded];
		strResourceDemanded = string.format("%s %s", L(kResourceInfo.IconString), L(kResourceInfo.Description));
		Show(Controls.ResourceDemandedBox);
	else
		eResourceDemanded = pCity:GetResourceDemanded(false);
		if eResourceDemanded ~= -1 then
			strResourceDemanded = L("TXT_KEY_CITYVIEW_RESOURCE_DEMANDED_UNKNOWN");
			Show(Controls.ResourceDemandedBox);
		else
			Hide(Controls.ResourceDemandedBox);
		end
	end

	local iNumTurns = pCity:GetWeLoveTheKingDayCounter();
	local strWLTKD;
	if iNumTurns > 0 then
		strWLTKD = L("TXT_KEY_CITYVIEW_WLTKD_COUNTER", iNumTurns);
		if pPlayer:IsGPWLTKD() then
			strWLTKD = L("TXT_KEY_CITYVIEW_WLTKD_COUNTER_UA_1", iNumTurns);
			Controls.ResourceDemandedBox:LocalizeAndSetToolTip("TXT_KEY_CITYVIEW_RESOURCE_FULFILLED_TT_UA_1");
			Show(Controls.ResourceDemandedBox);
		elseif pPlayer:IsCarnaval() then
			strWLTKD = L("TXT_KEY_CITYVIEW_WLTKD_COUNTER_UA_2", iNumTurns);
			Controls.ResourceDemandedBox:LocalizeAndSetToolTip("TXT_KEY_CITYVIEW_RESOURCE_FULFILLED_TT_UA_2");
			Show(Controls.ResourceDemandedBox);
		else
			Controls.ResourceDemandedBox:LocalizeAndSetToolTip("TXT_KEY_CITYVIEW_RESOURCE_FULFILLED_TT");
		end
	elseif eResourceDemanded ~= -1 then
		strWLTKD = L("TXT_KEY_CITYVIEW_RESOURCE_DEMANDED", strResourceDemanded);
		if pPlayer:IsGPWLTKD() then
			Controls.ResourceDemandedBox:LocalizeAndSetToolTip("TXT_KEY_CITYVIEW_RESOURCE_DEMANDED_TT_UA_1");
		elseif pPlayer:IsCarnaval() then
			Controls.ResourceDemandedBox:LocalizeAndSetToolTip("TXT_KEY_CITYVIEW_RESOURCE_DEMANDED_TT_UA_2");
		else
			Controls.ResourceDemandedBox:LocalizeAndSetToolTip("TXT_KEY_CITYVIEW_RESOURCE_DEMANDED_TT");
		end
	end

	if strWLTKD then
		Controls.ResourceDemandedString:SetText(strWLTKD);
		Controls.ResourceDemandedBox:SetSizeX(Controls.ResourceDemandedString:GetSizeX() + 10);
	end

	Refresh(Controls.IconsStack, Controls.NotificationStack);

	-------------------------------------------
	-- Bottom
	-------------------------------------------

	-- Buy tile button
	Controls.BuyPlotButton:LocalizeAndSetToolTip("TXT_KEY_CITYVIEW_BUY_TILE_TT");
	Controls.BuyPlotText:LocalizeAndSetText("TXT_KEY_CITYVIEW_BUY_TILE");
	if GameDefines.BUY_PLOTS_DISABLED ~= 0 then
		Disable(Controls.BuyPlotButton);
	-- Allow cities owned by the active player to always buy plots
	-- This is to enable tile purchase on Venetian puppets which are always on viewing mode
	-- The conditions properly guard it from buying tiles in non-Venetian puppets, foreign cities (espionage), or freshly conquered cities
	elseif not bViewingMode or (ePlayer == eActivePlayer and pCity:CanBuyAnyPlot()) then
		Enable(Controls.BuyPlotButton);
	else
		Disable(Controls.BuyPlotButton);
	end

	-- Raze/unraze button
	if not bOccupied or bRazing then
		Hide(Controls.RazeCityButton);
	-- Can we actually raze this city?
	elseif pPlayer:CanRaze(pCity, false) then
		Show(Controls.RazeCityButton);
		Enable(Controls.RazeCityButton);
		Controls.RazeCityButton:LocalizeAndSetToolTip("TXT_KEY_CITYVIEW_RAZE_BUTTON_TT");
	-- We COULD raze this city if it weren't a capital
	elseif pPlayer:CanRaze(pCity, true) then
		Show(Controls.RazeCityButton);
		Disable(Controls.RazeCityButton);
		Controls.RazeCityButton:LocalizeAndSetToolTip("TXT_KEY_CITYVIEW_RAZE_BUTTON_DISABLED_BECAUSE_CAPITAL_TT");
	-- Can't raze this city period
	else
		Hide(Controls.RazeCityButton);
	end

	if bRazing then
		Show(Controls.UnrazeCityButton);
		Enable(Controls.UnrazeCityButton);
	else
		Hide(Controls.UnrazeCityButton);
	end

	if bViewingMode then
		Disable(Controls.RazeCityButton, Controls.UnrazeCityButton);
	end

	-- Exit city
	if ePlayer ~= eActivePlayer then
		Controls.EndTurnText:LocalizeAndSetToolTip("TXT_KEY_CITYVIEW_RETURN_TO_ESPIONAGE");
	else
		Controls.EndTurnText:LocalizeAndSetToolTip("TXT_KEY_CITYVIEW_RETURN_TO_MAP");
	end

	-------------------------------------------
	-- Bottom left
	-------------------------------------------
	BuildProductionBox(pCity, ePlayer);

	-------------------------------------------
	-- Right side
	-------------------------------------------
	BuildWorkerManagementBox(pCity);
	BuildGPMeter(pCity, ePlayer);
	BuildUnemployedBox(pCity);
	BuildBuildingBox(pCity);
	RefreshRightStack();

	-------------------------------------------
	-- Worked tiles (and buy plot interface)
	-------------------------------------------
	UpdateWorkingHexes();

	-------------------------------------------
	-- Top left
	-------------------------------------------
	local strFoodTooltip = GetFoodTooltip(pCity);
	Controls.FoodBox:SetToolTipString(strFoodTooltip);
	Controls.PopulationBox:SetToolTipString(strFoodTooltip);
	local fFoodPerTurn = pCity:GetYieldRateTimes100(YieldTypes.YIELD_FOOD) / 100;
	if fFoodPerTurn > 0 then
		Controls.FoodPerTurnLabel:LocalizeAndSetText("TXT_KEY_CITYVIEW_PERTURN_TEXT", math.floor(fFoodPerTurn));
		Controls.CityGrowthLabel:LocalizeAndSetText("TXT_KEY_CITYVIEW_TURNS_TILL_CITIZEN_TEXT", pCity:GetFoodTurnsLeft());
	elseif fFoodPerTurn == 0 then
		Controls.FoodPerTurnLabel:LocalizeAndSetText("TXT_KEY_CITYVIEW_PERTURN_TEXT", math.floor(fFoodPerTurn));
		Controls.CityGrowthLabel:LocalizeAndSetText("TXT_KEY_CITYVIEW_STAGNATION_TEXT");
	else
		Controls.FoodPerTurnLabel:LocalizeAndSetText("TXT_KEY_CITYVIEW_PERTURN_TEXT_NEGATIVE", math.floor(fFoodPerTurn));
		Controls.CityGrowthLabel:LocalizeAndSetText("TXT_KEY_CITYVIEW_STARVATION_TEXT");
	end
	local iPopulation = pCity:GetPopulation();
	Controls.CityPopulationLabel:SetText(iPopulation);
	Controls.CityPopulationLabelSuffix:LocalizeAndSetText("TXT_KEY_CITYVIEW_CITIZENS_TEXT", iPopulation);
	Controls.PeopleMeter:SetPercent(pCity:GetFoodTimes100() / pCity:GrowthThreshold() / 100);

	Controls.GoldPerTurnLabel:LocalizeAndSetText("TXT_KEY_CITYVIEW_PERTURN_TEXT", math.floor(pCity:GetYieldRateTimes100(YieldTypes.YIELD_GOLD) / 100));
	Controls.GoldBox:SetToolTipString(GetGoldTooltip(pCity));

	if Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE) then
		Controls.SciencePerTurnLabel:LocalizeAndSetText("TXT_KEY_CITYVIEW_OFF");
	else
		Controls.SciencePerTurnLabel:LocalizeAndSetText("TXT_KEY_CITYVIEW_PERTURN_TEXT", math.floor(pCity:GetYieldRateTimes100(YieldTypes.YIELD_SCIENCE) / 100));
	end
	Controls.ScienceBox:SetToolTipString(GetScienceTooltip(pCity));

	if Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION) then
		Controls.FaithPerTurnLabel:LocalizeAndSetText("TXT_KEY_CITYVIEW_OFF");
	else
		Controls.FaithPerTurnLabel:LocalizeAndSetText("TXT_KEY_CITYVIEW_PERTURN_TEXT", math.floor(pCity:GetYieldRateTimes100(YieldTypes.YIELD_FAITH) / 100));
	end
	Controls.FaithBox:SetToolTipString(GetFaithTooltip(pCity));

	Controls.TourismPerTurnLabel:LocalizeAndSetText("TXT_KEY_CITYVIEW_PERTURN_TEXT", math.floor(pCity:GetYieldRateTimes100(YieldTypes.YIELD_TOURISM) / 100));
	Controls.TourismBox:SetToolTipString(GetTourismTooltip(pCity));

	if MOD_BALANCE_VP then
		Controls.HappinessBox:SetToolTipString(GetCityHappinessTooltip(pCity));
		Controls.HappinessPerTurnLabel:LocalizeAndSetText("TXT_KEY_NET_HAPPINESS_TEXT", pCity:GetLocalHappiness());
		Controls.UnhappinessBox:SetToolTipString(GetCityUnhappinessTooltip(pCity));
		Controls.UnhappinessPerTurnLabel:LocalizeAndSetText("TXT_KEY_NET_UNHAPPINESS_TEXT", pCity:GetUnhappinessAggregated());
	else
		Hide(Controls.HappinessBox, Controls.UnhappinessBox);
		Refresh(Controls.TopLeftStack);
	end

	Controls.CulturePerTurnLabel:LocalizeAndSetText("TXT_KEY_CITYVIEW_PERTURN_TEXT", math.floor(pCity:GetYieldRateTimes100(YieldTypes.YIELD_CULTURE) / 100));
	Controls.CultureBox:SetToolTipString(GetCultureTooltip(pCity));
	local fCultureStored = pCity:GetJONSCultureStoredTimes100() / 100;
	local iCultureNeeded = pCity:GetJONSCultureThreshold();
	local fBorderGrowth = pCity:GetYieldRateTimes100(YieldTypes.YIELD_CULTURE_LOCAL) / 100;
	local strBorderGrowthTooltip = GetBorderGrowthTooltip(pCity);
	if fBorderGrowth > 0 then
		local iTurnsToExpand = math.max(1, math.ceil((iCultureNeeded - fCultureStored) / fBorderGrowth));
		Controls.CultureTimeTillGrowthLabel:LocalizeAndSetText("TXT_KEY_CITYVIEW_TURNS_TILL_TILE_TEXT", iTurnsToExpand);
		Controls.CultureTimeTillGrowthLabel:SetToolTipString(strBorderGrowthTooltip);
		Show(Controls.CultureTimeTillGrowthLabel);
	else
		Hide(Controls.CultureTimeTillGrowthLabel);
	end
	Controls.CultureMeter:SetPercent(fCultureStored / iCultureNeeded);
	Controls.CultureMeter:SetToolTipString(strBorderGrowthTooltip);
end

UpdateTypes.FULLVIEW.UpdateFunc = UpdateViewFull;
Events.SerialEventCityScreenDirty.Add(UpdateTypes.FULLVIEW.ScheduleFunc);
Events.SerialEventCityInfoDirty.Add(UpdateTypes.FULLVIEW.ScheduleFunc);

---------------------------------------------------------------
-- City screen closed
Events.SerialEventExitCityScreen.Add(function ()
	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
	UpdateViewFull();

	-- We may get here after a player change, clear the UI if this is not the active player's city
	local pCity = UI.GetHeadSelectedCity();
	if pCity then
		if pCity:GetOwner() ~= eActivePlayer then
			ClearCityUIInfo();
		end
	end
	UI.ClearSelectedCities();

	LuaEvents.TryDismissTutorial("CITY_SCREEN");

	if not Controls.SellBuildingConfirm:IsHidden() then
		Hide(Controls.SellBuildingConfirm);
	end
	g_eBuildingToSell = -1;

	-- Try and re-select the last unit selected
	if not UI.GetHeadSelectedUnit() and UI.GetLastSelectedUnit() then
		UI.SelectUnit(UI.GetLastSelectedUnit());
		UI.LookAtSelectionPlot();
	end

	UI.SetCityScreenViewingMode(false);
end);

---------------------------------------------------------------
-- Input handling
---------------------------------------------------------------

local DefaultMessageHandler = {};

-- Add any interface modes that need special processing to this table
local InterfaceModeMessageHandler = {
	[InterfaceModeTypes.INTERFACEMODE_SELECTION] = {},
	[InterfaceModeTypes.INTERFACEMODE_PURCHASE_PLOT] = {},
};

--- @param wParam KeyType
DefaultMessageHandler[KeyEvents.KeyDown] = function (wParam)
	local eInterfaceMode = UI.GetInterfaceMode();
	if eInterfaceMode == InterfaceModeTypes.INTERFACEMODE_PURCHASE_PLOT then
		if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
			UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
			return true;
		end
	elseif eInterfaceMode == InterfaceModeTypes.INTERFACEMODE_SELECTION then
		if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
			if Controls.SellBuildingConfirm:IsHidden() then
				Events.SerialEventExitCityScreen();
				return true;
			else
				Hide(Controls.SellBuildingConfirm);
				g_eBuildingToSell = -1;
				return true;
			end
		elseif wParam == Keys.VK_LEFT then
			Game.DoControl(GameInfoTypes.CONTROL_PREVCITY);
			return true;
		elseif wParam == Keys.VK_RIGHT then
			Game.DoControl(GameInfoTypes.CONTROL_NEXTCITY);
			return true;
		end
	else
		error("Current interface mode is invalid");
	end
end

InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_SELECTION][MouseEvents.LButtonDown] = function ()
	if GameDefines.CITY_SCREEN_CLICK_WILL_EXIT == 1 then
		UI.ClearSelectedCities();
		return true;
	end
	return false;
end

InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_PURCHASE_PLOT][MouseEvents.RButtonUp] = function ()
	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
	return true;
end

--- @param uiMsg MouseEventType
--- @param wParam KeyType?
--- @return boolean
ContextPtr:SetInputHandler(function (uiMsg, wParam)
	local eInterfaceMode = UI.GetInterfaceMode();
	local currentInterfaceModeHandler = InterfaceModeMessageHandler[eInterfaceMode];
	if currentInterfaceModeHandler and currentInterfaceModeHandler[uiMsg] then
		return currentInterfaceModeHandler[uiMsg](wParam);
	elseif DefaultMessageHandler[uiMsg] then
		return DefaultMessageHandler[uiMsg](wParam);
	end
	return false;
end);

---------------------------------------------------------------
-- Change city name
Controls.EditButton:RegisterCallback(Mouse.eLClick, function ()
	local pCity = UI.GetHeadSelectedCity();
	assert(pCity, "Edit city name: No city selected? How could this happen?");

	Events.SerialEventGameMessagePopup({
		Type = ButtonPopupTypes.BUTTONPOPUP_RENAME_CITY,
		Data1 = pCity:GetID(),
	});
end);

---------------------------------------------------------------
-- Automated City Production
Controls.AutomateProduction:RegisterCheckHandler(function (isChecked)
	Game.SelectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_DO_TASK, TaskTypes.TASK_SET_AUTOMATED_PRODUCTION, -1, -1, isChecked, false);
end);

---------------------------------------------------------------
-- Enter city screen
Events.SerialEventEnterCityScreen.Add(function ()
	local pCity = UI.GetHeadSelectedCity();
	assert(pCity, "Enter city screen: No city selected? How could this happen?");

	if not pCity:IsPuppet() then
		Network.SendUpdateCityCitizens(pCity:GetID());
	end

	-- This calls TutorialEngine.lua
	LuaEvents.TryQueueTutorial("CITY_SCREEN", true);

	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
end);

---------------------------------------------------------------
-- Enter production screen
Controls.ProductionButton:RegisterCallback(Mouse.eLClick, function ()
	local pCity = UI.GetHeadSelectedCity();
	assert(pCity, "Production button: No city selected? How could this happen?");

	-- Send production popup message
	Events.SerialEventGameMessagePopup({
		Type = ButtonPopupTypes.BUTTONPOPUP_CHOOSEPRODUCTION,
		Data1 = pCity:GetID(),
		Data2 = -1,
		Data3 = -1,
		Option1 = (bProductionQueueOpen and pCity:GetOrderQueueLength() > 0),
		Option2 = false,
	});
end);

---------------------------------------------------------------
-- Enter purchase screen
Controls.PurchaseButton:RegisterCallback(Mouse.eLClick, function ()
	local pCity = UI.GetHeadSelectedCity();
	assert(pCity, "Purchase button: No city selected? How could this happen?");

	-- Send production popup message
	Events.SerialEventGameMessagePopup({
		Type = ButtonPopupTypes.BUTTONPOPUP_CHOOSEPRODUCTION,
		Data1 = pCity:GetID(),
		Data2 = -1,
		Data3 = -1,
		Option1 = (bProductionQueueOpen and pCity:GetOrderQueueLength() > 0),
		Option2 = true,
	});
end);

---------------------------------------------------------------
-- Remove queue items
--- @param iQueueOrder integer
local function OnRemoveClick(iQueueOrder)
	Game.SelectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_POP_ORDER, iQueueOrder);
end

local tRemoveButtons = {
	Controls.b1remove,
	Controls.b2remove,
	Controls.b3remove,
	Controls.b4remove,
	Controls.b5remove,
	Controls.b6remove,
};

for i, button in ipairs(tRemoveButtons) do
	button:SetVoid1(i - 1);
	button:RegisterCallback(Mouse.eLClick, OnRemoveClick);
end

---------------------------------------------------------------
-- Swap queue items
--- @param iPairOrder integer
local function OnSwapClick(iPairOrder)
	-- Swap the nth pair of queued items
	Game.SelectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_SWAP_ORDER, iPairOrder);
end

local tDownButtons = {
	Controls.b1down,
	Controls.b2down,
	Controls.b3down,
	Controls.b4down,
	Controls.b5down,
};

local tUpButtons = {
	Controls.b2up,
	Controls.b3up,
	Controls.b4up,
	Controls.b5up,
	Controls.b6up,
};

for i, button in ipairs(tDownButtons) do
	button:SetVoid1(i - 1);
	button:RegisterCallback(Mouse.eLClick, OnSwapClick);
end

for i, button in ipairs(tUpButtons) do
	button:SetVoid1(i - 1);
	button:RegisterCallback(Mouse.eLClick, OnSwapClick);
end

-----------------------------------------------------------------------------
-- Manual specialist control
local function OnNoAutoSpecialistCheckboxClick()
	-- Toggle to the opposite state
	local bValue = not UI.GetHeadSelectedCity():IsNoAutoAssignSpecialists();
	Game.SelectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_DO_TASK, TaskTypes.TASK_NO_AUTO_ASSIGN_SPECIALISTS, -1, -1, bValue);
end

Controls.NoAutoSpecialistCheckbox:RegisterCallback(Mouse.eLClick, OnNoAutoSpecialistCheckboxClick);
Controls.NoAutoSpecialistCheckbox2:RegisterCallback(Mouse.eLClick, OnNoAutoSpecialistCheckboxClick);

-----------------------------------------------------------------------------
-- Reset Specialists button
Controls.ResetSpecialistsButton:RegisterCallback(Mouse.eLClick, function ()
	Game.SelectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_DO_TASK, TaskTypes.TASK_RESET_SPECIALISTS, -1, -1);
end);

---------------------------------------------------------------
-- Go to next city
Controls.NextCityButton:RegisterCallback(Mouse.eLClick, function ()
	Game.DoControl(GameInfoTypes.CONTROL_NEXTCITY);
end);

---------------------------------------------------------------
-- Go to previous city
Controls.PrevCityButton:RegisterCallback(Mouse.eLClick, function ()
	Game.DoControl(GameInfoTypes.CONTROL_PREVCITY);
end);

---------------------------------------------------------------
-- Buy plot mode
Controls.BuyPlotButton:RegisterCallback(Mouse.eLClick, function ()
	if not pActivePlayer:IsTurnActive() then
		return;
	end

	local pCity = UI.GetHeadSelectedCity();
	assert(pCity, "Buy plot button: No city selected? How could this happen?");

	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_PURCHASE_PLOT);
	UpdateWorkingHexes();
end);

---------------------------------------------------------------
-- Plot moused over
--- @param iX integer
--- @param iY integer
Events.SerialEventMouseOverHex.Add(function (iX, iY)
	if UI.GetInterfaceMode() == InterfaceModeTypes.INTERFACEMODE_PURCHASE_PLOT then
		local pCity = UI.GetHeadSelectedCity();
		assert(pCity, "Buy plot interface: No city selected? How could this happen?");

		local strText = "---";

		-- Can buy this plot
		if pCity:CanBuyPlotAt(iX, iY, true) then
			local iPlotCost = pCity:GetBuyPlotCost(iX, iY);
			strText = "[ICON_GOLD] " .. iPlotCost;
		end

		Controls.BuyPlotText:SetText(strText);
	end
end);

---------------------------------------------------------------
-- Return to map
Controls.ReturnToMapButton:RegisterCallback(Mouse.eLClick, function ()
	Events.SerialEventExitCityScreen();
end);

---------------------------------------------------------------
-- Raze
Controls.RazeCityButton:RegisterCallback(Mouse.eLClick, function ()
	local pCity = UI.GetHeadSelectedCity();
	assert(pCity, "Raze: No city selected? How could this happen?");

	Events.SerialEventGameMessagePopup({
		Type = ButtonPopupTypes.BUTTONPOPUP_CONFIRM_CITY_TASK,
		Data1 = pCity:GetID(),
		Data2 = TaskTypes.TASK_RAZE,
	});
end);

---------------------------------------------------------------
-- Cancel raze
Controls.UnrazeCityButton:RegisterCallback(Mouse.eLClick, function ()
	if pActivePlayer:IsTurnActive() then
		local pCity = UI.GetHeadSelectedCity();
		assert(pCity, "Cancel raze: No city selected? How could this happen?");

		Network.SendDoTask(pCity:GetID(), TaskTypes.TASK_UNRAZE, -1, -1, false, false, false, false);
	end
end);

---------------------------------------------------------------
-- Right click on production queue item portrait
Controls.ProductionPortraitButton:RegisterCallback(Mouse.eRClick, function ()
	local pCity = UI.GetHeadSelectedCity();
	assert(pCity, "Production queue portrait: No city selected? How could this happen?");

	local strSearch;
	local eUnitProduction = pCity:GetProductionUnit();
	local eBuildingProduction = pCity:GetProductionBuilding();
	local eProjectProduction = pCity:GetProductionProject();
	local eProcessProduction = pCity:GetProductionProcess();

	if eUnitProduction ~= -1 then
		strSearch = L(GameInfo.Units[eUnitProduction].Description);
	elseif eBuildingProduction ~= -1 then
		strSearch = L(GameInfo.Buildings[eBuildingProduction].Description);
	elseif eProjectProduction ~= -1 then
		strSearch = L(GameInfo.Projects[eProjectProduction].Description);
	elseif eProcessProduction ~= -1 then
		strSearch = L(GameInfo.Processes[eProcessProduction].Description);
	end

	if strSearch then
		Events.SearchForPediaEntry(strSearch);
	end
end);

---------------------------------------------------------------
-- Hide queue
--- @param bIsChecked boolean
Controls.HideQueueButton:RegisterCheckHandler(function (bIsChecked)
	local pCity = UI.GetHeadSelectedCity();
	assert(pCity, "Hide queue button: No city selected? How could this happen?");
	bProductionQueueOpen = bIsChecked;
	BuildProductionBox(pCity, pCity:GetOwner());
end);

---------------------------------------------------------------
-- Focus buttons
for eFocus, strButtonName in pairs(tFocusButtons) do
	Controls[strButtonName]:RegisterCallback(Mouse.eLClick, function ()
		if pActivePlayer:IsTurnActive() then
			local pCity = UI.GetHeadSelectedCity();
			assert(pCity, "Switch governor focus: No city selected? How could this happen?");
			Network.SendSetCityAIFocus(pCity:GetID(), eFocus);
		end
	end);
end

---------------------------------------------------------------
-- Avoid growth
Controls.AvoidGrowthButton:RegisterCallback(Mouse.eLClick, function ()
	if pActivePlayer:IsTurnActive() then
		local pCity = UI.GetHeadSelectedCity();
		assert(pCity, "Avoid Growth button: No city selected? How could this happen?");
		Network.SendSetCityAvoidGrowth(pCity:GetID(), not pCity:IsForcedAvoidGrowth());
	end
end);

---------------------------------------------------------------
-- Support for add-in Lua
for addin in Modding.GetActivatedModEntryPoints("CityViewUIAddin") do
	local addinFile = Modding.GetEvaluatedFilePath(addin.ModID, addin.Version, addin.File);
	local strPath = addinFile.EvaluatedPath;

	-- Get the absolute path and filename without extension
	local strExtension = Path.GetExtension(strPath);
	local strPathWithoutExtension = string.sub(strPath, 1, #strPath - #strExtension);

	ContextPtr:LoadNewContext(strPathWithoutExtension);
end

---------------------------------------------------------------
--- Custom event called by ProductionPopup.lua
--- @param bIsHide boolean
LuaEvents.ProductionPopup.Add(function (bIsHide)
	Controls.TopLeft:SetHide(not bIsHide);
	Controls.ProdQueueBackground:SetHide(not bIsHide);
	Controls.LeftTrim:SetHide(not bIsHide);
	if OptionsManager.GetSmallUIAssets() then
		Controls.CivIconFrame:SetHide(not bIsHide);
		Controls.ProdQueueBackground:SetHide(not bIsHide);
	else
		Controls.InfoBG:SetHide(not bIsHide);
		Controls.CityInfo:SetHide(not bIsHide);
	end
end);

---------------------------------------------------------------
-- Sell building popup
---------------------------------------------------------------
Controls.YesButton:RegisterCallback(Mouse.eLClick, function ()
	Hide(Controls.SellBuildingConfirm);
	if pActivePlayer:IsTurnActive() then
		local pCity = UI.GetHeadSelectedCity();
		assert(pCity, "Confirm sell building: No city selected? How could this happen?");
		Network.SendSellBuilding(pCity:GetID(), g_eBuildingToSell);
	end
	g_eBuildingToSell = -1;
end);

Controls.NoButton:RegisterCallback(Mouse.eLClick, function ()
	Hide(Controls.SellBuildingConfirm);
	g_eBuildingToSell = -1;
end);

---------------------------------------------------------------
-- Strategic view toggle
--- @param bStrategicView boolean
Events.StrategicViewStateChanged.Add(function (bStrategicView)
	if bStrategicView then
		g_vWorldPositionOffset  = STRATEGIC_VIEW_WORLD_POSITION_OFFSET;
		g_vWorldPositionOffset2 = STRATEGIC_VIEW_WORLD_POSITION_OFFSET;
	else
		g_vWorldPositionOffset  = NORMAL_WORLD_POSITION_OFFSET;
		g_vWorldPositionOffset2 = NORMAL_WORLD_POSITION_OFFSET2;
	end
end);

---------------------------------------------------------------
-- Section headers
--- @param tSection HeaderDef
local function HandleAppearance(tSection)
	if tSection.HeadingOpen then
		Controls[tSection.Label]:SetText("[ICON_MINUS] " .. tSection.LabelText);
	else
		Controls[tSection.Label]:SetText("[ICON_PLUS] " .. tSection.LabelText);
	end
	if tSection.OuterStack then
		Controls[tSection.OuterStack]:SetHide(not tSection.HeadingOpen);
	end
	-- For InnerStack sections, we handle collapsing in BuildBuildingBox by:
	-- 1. Not adding building buttons when closed
	-- 2. Hiding static child controls when closed
end

--- @param strSectionName string
--- @param tSection HeaderDef
--- @return fun()
local function MakeHeaderToggleCallback(strSectionName, tSection)
	return function ()
		tSection.HeadingOpen = not tSection.HeadingOpen;
		HandleAppearance(tSection);

		-- Schedule an update
		tSection.UpdateType.ScheduleFunc();
	end
end

for strSectionName, tSection in pairs(tRightSideSections) do
	HandleAppearance(tSection);
	Controls[strSectionName]:RegisterCallback(Mouse.eLClick, MakeHeaderToggleCallback(strSectionName, tSection));
end

---------------------------------------------------------------
-- 'Active' (local human) player has changed
Events.GameplaySetActivePlayer.Add(function ()
	eActivePlayer = Game.GetActivePlayer();
	pActivePlayer = Players[eActivePlayer];
	if not ContextPtr:IsHidden() then
		Events.SerialEventExitCityScreen();
	end
end);
