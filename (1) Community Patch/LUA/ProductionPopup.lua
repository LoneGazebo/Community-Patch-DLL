print("This is the modded ProductionPopup from Community Patch");

-------------------------------------------------
-- Production Chooser Popup
-------------------------------------------------
include("IconSupport");
include("InstanceManager");
include("InfoTooltipInclude");
include("VPUI_core");
include("CPK.lua");

local g_UnitInstanceManager = InstanceManager:new("ProductionButtonInstance", "ProductionButton", Controls.UnitButtonStack);
local g_BuildingInstanceManager = InstanceManager:new("ProductionButtonInstance", "ProductionButton", Controls.BuildingButtonStack);
local g_WonderInstanceManager = InstanceManager:new("ProductionButtonInstance", "ProductionButton", Controls.WonderButtonStack);
local g_ProcessInstanceManager = InstanceManager:new("ProductionButtonInstance", "ProductionButton", Controls.OtherButtonStack);

local tooltipInstance = {};
TTManager:GetTypeControlTable("CityViewTooltip", tooltipInstance);

local g_popupInfo = {};

local g_bHidden = true;

local L = Locale.Lookup;
local VP = VP or MapModData.VP;
local GameInfoCache = VP.GameInfoCache;
local IconHookupOrDefault = VP.IconHookupOrDefault;
local Hide = CPK.UI.Control.Hide;
local Show = CPK.UI.Control.Show;
local Refresh = CPK.UI.Control.Refresh;
local CustomModOptionEnabled = CPK.Misc.CustomModOptionEnabled;

local MOD_BALANCE_VP = CustomModOptionEnabled("BALANCE_VP");

local INFINITE_TURNS_STRING = L("TXT_KEY_PRODUCTION_HELP_INFINITE_TURNS");
local OFF_STRING = L("TXT_KEY_CITYVIEW_OFF");

--- @class OrderTypeHeaderDef
--- @field HeadingOpen boolean
--- @field Name string
--- @field Label string
--- @field LabelText string
--- @field Stack string

--- @class OrderTypeDef
--- @field TableName string
--- @field InstanceManager InstanceManager
--- @field Header OrderTypeHeaderDef
--- @field HelpTextFunc fun(eType: UnitType|BuildingType|ProjectType|ProcessType, pCity: City?): string
--- @field TurnsLeftFunc (fun(eType: UnitType|BuildingType|ProjectType|ProcessType, pCity: City, iQueueIndex: integer): integer)?
--- @field CostFunc (fun(eType: UnitType|BuildingType|ProjectType|ProcessType, pCity: City, eYield: YieldType): integer)?
--- @field CanProduceFunc fun(eType: UnitType|BuildingType|ProjectType|ProcessType, pCity: City, bTestVisible: boolean): boolean
--- @field CanPurchaseFunc fun(eType: UnitType|BuildingType|ProjectType|ProcessType, pCity: City, eYield: YieldType, bTestVisible: boolean): boolean
--- @field DisabledFunc fun(eType: UnitType|BuildingType|ProjectType|ProcessType, pCity: City, eYield: YieldType): string
--- @field PurchaseFunc fun(eType: UnitType|BuildingType|ProjectType|ProcessType, pCity: City, eYield: YieldType)?
--- @field IsRecommendedFunc fun(eType: UnitType|BuildingType|ProjectType|ProcessType, eAdvisor: AdvisorType): boolean

--- @type table<OrderType, OrderTypeDef>
local tOrderTypeDetails = {
	[OrderTypes.ORDER_CONSTRUCT] = {
		TableName = "Buildings",
		InstanceManager = g_BuildingInstanceManager,
		Header = {
			HeadingOpen = true,
			Name = "BuildingButton",
			Label = "BuildingButtonLabel",
			LabelText = L("TXT_KEY_POP_BUILDINGS"),
			Stack = "BuildingButtonStack",
		},
		HelpTextFunc = function (eType, pCity)
			return GetHelpTextForBuilding(eType, false, nil, false, pCity, false, true);
		end,
		TurnsLeftFunc = function (eType, pCity, iQueueIndex)
			return pCity:GetBuildingProductionTurnsLeft(eType, iQueueIndex - 1);
		end,
		CostFunc = function (eType, pCity, eYield)
			if eYield == YieldTypes.YIELD_GOLD then
				return pCity:GetBuildingPurchaseCost(eType);
			elseif eYield == YieldTypes.YIELD_FAITH then
				return pCity:GetBuildingFaithPurchaseCost(eType);
			end
			print(string.format("ERROR: Invalid yield type %d is being used for purchasing buildings", eYield));
			return -1;
		end,
		CanProduceFunc = function (eType, pCity, bTestVisible)
			return pCity:CanConstruct(eType, 0, bTestVisible and 1 or 0);
		end,
		CanPurchaseFunc = function (eType, pCity, eYield, bTestVisible)
			return pCity:IsCanPurchase(not bTestVisible, not bTestVisible, -1, eType, -1, eYield);
		end,
		DisabledFunc = function (eType, pCity, eYield)
			if eYield == YieldTypes.NO_YIELD then
				return pCity:CanConstructTooltip(eType);
			elseif eYield == YieldTypes.YIELD_GOLD then
				return pCity:GetPurchaseBuildingTooltip(eType);
			elseif eYield == YieldTypes.YIELD_FAITH then
				return pCity:GetFaithPurchaseBuildingTooltip(eType);
			end
			print(string.format("ERROR: Invalid yield type %s is being used for purchasing buildings", GameInfo.Yields[eYield].Description));
			return "";
		end,
		PurchaseFunc = function (eType, pCity, eYield)
			return Game.CityPurchaseBuilding(pCity, eType, eYield);
		end,
		IsRecommendedFunc = function (eType, eAdvisor)
			return Game.IsBuildingRecommended(eType, eAdvisor);
		end
	},
	[OrderTypes.ORDER_CREATE] = {
		TableName = "Projects",
		InstanceManager = g_WonderInstanceManager,
		Header = {
			HeadingOpen = true,
			Name = "WonderButton",
			Label = "WonderButtonLabel",
			LabelText = L("TXT_KEY_POP_WONDERS"),
			Stack = "WonderButtonStack",
		},
		HelpTextFunc = function (eType, pCity)
			return GetHelpTextForProject(eType, pCity);
		end,
		TurnsLeftFunc = function (eType, pCity, iQueueIndex)
			return pCity:GetProjectProductionTurnsLeft(eType, iQueueIndex - 1);
		end,
		CostFunc = function (eType, pCity, eYield)
			if eYield == YieldTypes.YIELD_GOLD then
				return pCity:GetProjectPurchaseCost(eType);
			end
			print(string.format("ERROR: Invalid yield type %d is being used for purchasing projects", eYield));
			return -1;
		end,
		CanProduceFunc = function (eType, pCity, bTestVisible)
			return pCity:CanCreate(eType, 0, bTestVisible and 1 or 0);
		end,
		CanPurchaseFunc = function (eType, pCity, eYield, bTestVisible)
			return pCity:IsCanPurchase(not bTestVisible, not bTestVisible, -1, -1, eType, eYield);
		end,
		DisabledFunc = function (eType, pCity, eYield)
			if eYield == YieldTypes.NO_YIELD then
				return pCity:CanCreateTooltip(eType);
			elseif eYield == YieldTypes.YIELD_GOLD then
				-- Doesn't have a function for this
				return "";
			end
			print(string.format("ERROR: Invalid yield type %s is being used for purchasing projects", GameInfo.Yields[eYield].Description));
			return "";
		end,
		PurchaseFunc = function (eType, pCity, eYield)
			return Game.CityPurchaseProject(pCity, eType, eYield);
		end,
		IsRecommendedFunc = function (eType, eAdvisor)
			return Game.IsProjectRecommended(eType, eAdvisor);
		end
	},
	[OrderTypes.ORDER_MAINTAIN] = {
		TableName = "Processes",
		InstanceManager = g_ProcessInstanceManager,
		Header = {
			HeadingOpen = true,
			Name = "OtherButton",
			Label = "OtherButtonLabel",
			LabelText = L("TXT_KEY_CITYVIEW_OTHER"),
			Stack = "OtherButtonStack",
		},
		HelpTextFunc = function (eType)
			return GetHelpTextForProcess(eType);
		end,
		CanProduceFunc = function (eType, pCity)
			return pCity:CanMaintain(eType);
		end,
		CanPurchaseFunc = function ()
			return false;
		end,
		DisabledFunc = function ()
			return "";
		end,
		IsRecommendedFunc = function ()
			return false;
		end
	},
	[OrderTypes.ORDER_TRAIN] = {
		TableName = "Units",
		InstanceManager = g_UnitInstanceManager,
		Header = {
			HeadingOpen = true,
			Name = "UnitButton",
			Label = "UnitButtonLabel",
			LabelText = L("TXT_KEY_POP_UNITS"),
			Stack = "UnitButtonStack",
		},
		HelpTextFunc = function (eType, pCity)
			return GetHelpTextForUnit(eType, false, pCity);
		end,
		TurnsLeftFunc = function (eType, pCity, iQueueIndex)
			return pCity:GetUnitProductionTurnsLeft(eType, iQueueIndex - 1);
		end,
		CostFunc = function (eType, pCity, eYield)
			if eYield == YieldTypes.YIELD_GOLD then
				return pCity:GetUnitPurchaseCost(eType);
			elseif eYield == YieldTypes.YIELD_FAITH then
				return pCity:GetUnitFaithPurchaseCost(eType);
			end
			print(string.format("ERROR: Invalid yield type %d is being used for purchasing units", eYield));
			return -1;
		end,
		CanProduceFunc = function (eType, pCity, bTestVisible)
			return pCity:CanTrain(eType, 0, bTestVisible and 1 or 0);
		end,
		CanPurchaseFunc = function (eType, pCity, eYield, bTestVisible)
			return pCity:IsCanPurchase(not bTestVisible, not bTestVisible, eType, -1, -1, eYield);
		end,
		DisabledFunc = function (eType, pCity, eYield)
			if eYield == YieldTypes.NO_YIELD then
				return pCity:CanTrainTooltip(eType);
			elseif eYield == YieldTypes.YIELD_GOLD then
				return pCity:GetPurchaseUnitTooltip(eType);
			elseif eYield == YieldTypes.YIELD_FAITH then
				return pCity:GetFaithPurchaseUnitTooltip(eType);
			end
			print(string.format("ERROR: Invalid yield type %s is being used for purchasing units", GameInfo.Yields[eYield].Description));
			return "";
		end,
		PurchaseFunc = function (eType, pCity, eYield)
			return Game.CityPurchaseUnit(pCity, eType, eYield);
		end,
		IsRecommendedFunc = function (eType, eAdvisor)
			return Game.IsUnitRecommended(eType, eAdvisor);
		end
	},
};

local tAdvisorImages = {
	[AdvisorTypes.ADVISOR_MILITARY] = Controls.MilitaryRecommendation,
	[AdvisorTypes.ADVISOR_ECONOMIC] = Controls.EconomicRecommendation,
	[AdvisorTypes.ADVISOR_FOREIGN] = Controls.ForeignRecommendation,
	[AdvisorTypes.ADVISOR_SCIENCE] = Controls.ScienceRecommendation,
};

local eActivePlayer = Game.GetActivePlayer();
local pActivePlayer = Players[eActivePlayer];

-- We cannot assume that the city this popup is working on is UI.GetHeadSelectedCity(), so we need some way to store the current city.
-- A city pointer might be invalidated at any time so we save the city ID instead.
-- (We CAN assume the city is always owned by the player opening the popup)
-- This popup must be closed whenever we fail to retrieve the current city, or the active player has changed.
local eCurrentCity = -1;

local g_bQueueMode = false;
local g_bProductionMode = true;
local g_eOrder = -1;
local g_eFinishedItem = -1;

local _, iScreenSizeY = UIManager:GetScreenSizeVal();
Controls.ScrollBar:SetSizeY(iScreenSizeY - 400);
Controls.DownButton:SetOffsetY(iScreenSizeY - 414);
Controls.ScrollPanel:SetSizeY(iScreenSizeY - 396);
Refresh(Controls.ScrollPanel);
Controls.Backdrop:SetSizeY(iScreenSizeY - 300);

---------------------------------------------------------------
--- Shorthand for setting a custom tooltip and adjusting its size
--- @param tooltip TooltipInstance
--- @param strTooltip string
local function SetTooltip(tooltip, strTooltip)
	tooltip.CityViewTooltipText:SetText(strTooltip);
	tooltip.CityViewTooltipGrid:DoAutoSize();
end

----------------------------------------------------------------
-- Close the popup
local function Close()
	eCurrentCity = -1;
	Hide(ContextPtr);
end

Controls.CloseButton:RegisterCallback(Mouse.eLClick, Close);
Events.SerialEventExitCityScreen.Add(Close);

-------------------------------------------------
--- Get the current city the popup is working with. May return nil.
--- @return City?
local function GetCurrentCity()
	local pCity;
	if eCurrentCity ~= -1 then
		pCity = pActivePlayer:GetCityByID(eCurrentCity);
	end

	-- Failed to retrieve the city. It either means an invalid ID was used when opening this popup,
	-- or the city is removed while the player is in this popup. We close this popup either way.
	if not pCity then
		Close();
	end
	return pCity;
end

-------------------------------------------------
Controls.CityButton:RegisterCallback(Mouse.eLClick, function (x, y)
	Close();
	UI.DoSelectCityAtPlot(Map.GetPlot(x, y));
end);

----------------------------------------------------------------
--- Add a button based on the item info
--- @param eItem UnitType|BuildingType|ProjectType|ProcessType
--- @param eOrder OrderType
--- @param ePurchaseYield YieldType
--- @param strTurnsOrCost string
local function AddProductionButton(eItem, eOrder, ePurchaseYield, strTurnsOrCost)
	local pCity = GetCurrentCity();
	if not pCity then
		return;
	end

	local tOrder = tOrderTypeDetails[eOrder];

	local function OnProductionSelected()
		-- Don't use the upvalue pCity! That could somehow be invalid on the back end
		local pCurrentCity = GetCurrentCity();
		if not pCurrentCity then
			Close();
			return;
		end

		local eCity = pCurrentCity:GetID();
		if g_bProductionMode then
			if tOrder.CanProduceFunc(eItem, pCurrentCity, false) then
				Game.CityPushOrder(pCurrentCity, eOrder, eItem, false, not g_bQueueMode, true);
			end
		else
			assert(eOrder ~= OrderTypes.ORDER_MAINTAIN, "Attempting to purchase a process");
			if tOrder.CanPurchaseFunc(eItem, pCurrentCity, ePurchaseYield, false) then
				tOrder.PurchaseFunc(eItem, pCurrentCity, ePurchaseYield);
				Network.SendUpdateCityCitizens(eCity);
				if ePurchaseYield == YieldTypes.YIELD_GOLD then
					Events.AudioPlay2DSound("AS2D_INTERFACE_CITY_SCREEN_PURCHASE");
				elseif ePurchaseYield == YieldTypes.YIELD_FAITH then
					Events.AudioPlay2DSound("AS2D_INTERFACE_FAITH_PURCHASE");
				end
			end
		end

		Events.SpecificCityInfoDirty(eActivePlayer, eCity, CityUpdateTypes.CITY_UPDATE_TYPE_BANNER);
		Events.SpecificCityInfoDirty(eActivePlayer, eCity, CityUpdateTypes.CITY_UPDATE_TYPE_PRODUCTION);

		if not g_bQueueMode or not g_bProductionMode then
			Close();
		end
	end

	-- Create button
	local kInfo = GameInfo[tOrder.TableName][eItem];
	local instanceManager = tOrder.InstanceManager;
	if eOrder == OrderTypes.ORDER_CONSTRUCT then
		local kBuildingClassInfo = GameInfo.BuildingClasses[kInfo.BuildingClass];
		if kBuildingClassInfo.MaxGlobalInstances > -1 or kBuildingClassInfo.MaxPlayerInstances == 1 or kBuildingClassInfo.MaxTeamInstances > -1 then
			instanceManager = g_WonderInstanceManager;
		end
	end
	local instance = instanceManager:GetInstance();

	-- Portrait
	local iPortraitIndex;
	local strAtlas;
	if eOrder == OrderTypes.ORDER_TRAIN then
		iPortraitIndex, strAtlas = UI.GetUnitPortraitIcon(eItem, eActivePlayer);
	else
		iPortraitIndex = kInfo.PortraitIndex;
		strAtlas = kInfo.IconAtlas;
	end
	IconHookupOrDefault(iPortraitIndex, 45, strAtlas, instance.ProductionButtonImage);

	-- Tooltip
	local bDisabled;
	if g_bProductionMode then
		bDisabled = not tOrder.CanProduceFunc(eItem, pCity, false);
	else
		bDisabled = not tOrder.CanPurchaseFunc(eItem, pCity, ePurchaseYield, false);
	end
	instance.ProductionButton:SetToolTipCallback(function ()
		-- We can't be sure that the original city pointer is still valid here
		local pCurrentCity = GetCurrentCity();
		if not pCurrentCity then
			return;
		end

		local tLines = {};
		table.insert(tLines, tOrder.HelpTextFunc(eItem, pCurrentCity));
		if bDisabled then
			local strDisabled = tOrder.DisabledFunc(eItem, pCurrentCity, ePurchaseYield);
			if strDisabled ~= "" then
				table.insert(tLines, string.format("[COLOR_WARNING_TEXT]%s[ENDCOLOR]", strDisabled));
			end
		end
		SetTooltip(tooltipInstance, table.concat(tLines, "[NEWLINE]"));
	end);

	-- Name
	local strItemName = L(kInfo.Description);
	instance.UnitName:SetText(strItemName);
	local function GetProdHelp()
		Events.SearchForPediaEntry(strItemName);
	end

	-- Turns/purchase cost
	if g_bProductionMode then
		instance.NumTurns:SetText(strTurnsOrCost);
	else
		instance.NumTurns:SetText(string.format("%d %s", strTurnsOrCost, GameInfo.Yields[ePurchaseYield].IconString));
	end

	-- Advisors
	for eAdvisor, image in pairs(tAdvisorImages) do
		image:SetHide(not tOrder.IsRecommendedFunc(eItem, eAdvisor));
	end

	-- Button callbacks
	instance.ProductionButton:RegisterCallback(Mouse.eRClick, GetProdHelp);
	if bDisabled then
		instance.ProductionButton:ClearCallback(Mouse.eLClick);
		Show(instance.DisabledBox);
	else
		instance.ProductionButton:RegisterCallback(Mouse.eLClick, OnProductionSelected);
		Hide(instance.DisabledBox);
	end
end

-------------------------------------------------
--- @param tHeader OrderTypeHeaderDef
local function HandleAppearance(tHeader)
	if tHeader.HeadingOpen then
		Controls[tHeader.Label]:SetText("[ICON_MINUS] " .. tHeader.LabelText);
		Show(Controls[tHeader.Stack]);
		Refresh(Controls[tHeader.Stack]);
	else
		Controls[tHeader.Label]:SetText("[ICON_PLUS] " .. tHeader.LabelText);
		Hide(Controls[tHeader.Stack]);
	end
	Refresh(Controls.StackOStacks, Controls.ScrollPanel);
end

--- @param tHeader OrderTypeHeaderDef
--- @return fun()
local function MakeHeaderToggleCallback(tHeader)
	return function ()
		tHeader.HeadingOpen = not tHeader.HeadingOpen;
		HandleAppearance(tHeader);
	end
end

for _, tOrder in pairs(tOrderTypeDetails) do
	Controls[tOrder.Header.Name]:RegisterCallback(Mouse.eLClick, MakeHeaderToggleCallback(tOrder.Header));
end

-------------------------------------------------
-- Full update
--- @param pCity City?
local function UpdateWindow(pCity)
	pCity = pCity or GetCurrentCity();
	if not pCity then
		Close();
		return;
	end

	eCurrentCity = pCity:GetID();

	local pSelectedCity = UI.GetHeadSelectedCity();
	local eSelectedCity = pSelectedCity and pSelectedCity:GetID() or -1;

	-- Should we pan to this city?
	if eCurrentCity ~= eSelectedCity then
		UI.LookAt(pCity:Plot(), 0);
	end

	g_UnitInstanceManager:ResetInstances();
	g_BuildingInstanceManager:ResetInstances();
	g_WonderInstanceManager:ResetInstances();
	g_ProcessInstanceManager:ResetInstances();

	local fProductionPerTurn = pCity:GetYieldRateTimes100(YieldTypes.YIELD_PRODUCTION) / 100;
	local bGeneratingProduction = (fProductionPerTurn > 0);

	-------------------------------------------
	-- Bottom left
	-------------------------------------------

	-- Queue mode?
	if g_bQueueMode and g_bProductionMode then
		-- First, check if this popup should be closed immediately
		local iQueueLength = pCity:GetOrderQueueLength();
		if iQueueLength >= 6 then
			Close();
			return;
		end

		for i = 1, iQueueLength do
			local eOrder = pCity:GetOrderFromQueue(i - 1);
			if eOrder == OrderTypes.ORDER_MAINTAIN then
				Close();
				return;
			end
		end

		-- Now proceed to build the queue box
		Hide(Controls.ProductionBox, Controls.b1box, Controls.b2box, Controls.b3box, Controls.b4box, Controls.b5box, Controls.b6box);
		for i = 1, iQueueLength do
			local strPrefix = "b" .. tostring(i);
			local box = Controls[strPrefix .. "box"];
			local nameLabel = Controls[strPrefix .. "name"];
			local turnsLabel = Controls[strPrefix .. "turns"];

			local eOrder, eItem = pCity:GetOrderFromQueue(i - 1);
			local tOrder = tOrderTypeDetails[eOrder];
			local kInfo = GameInfo[tOrder.TableName][eItem];

			nameLabel:LocalizeAndSetText(kInfo.Description);

			if eOrder == OrderTypes.ORDER_MAINTAIN then
				Hide(turnsLabel);
			else
				Show(turnsLabel);
				if bGeneratingProduction then
					turnsLabel:LocalizeAndSetText("TXT_KEY_PRODUCTION_HELP_NUM_TURNS", tOrder.TurnsLeftFunc(eItem, pCity, i));
				else
					turnsLabel:SetText(INFINITE_TURNS_STRING);
				end
			end

			box:SetToolTipCallback(function ()
				-- We can't be sure that the original city pointer is still valid here
				local pCurrentCity = GetCurrentCity();
				if not pCurrentCity then
					return;
				end
				SetTooltip(tooltipInstance, tOrder.HelpTextFunc(eItem, pCurrentCity));
			end);
			Show(box);
		end
		Show(Controls.ProductionQueueBox);
	else
		Hide(Controls.ProductionQueueBox);

		local strItemName;
		local function GetProdHelp()
			Events.SearchForPediaEntry(strItemName);
		end

		local tFinishedOrder = tOrderTypeDetails[g_eOrder];
		if tFinishedOrder and g_eFinishedItem ~= -1 then
			-- We just finished something, so show THAT in the bottom left (but no icon)
			strItemName = L(GameInfo[tFinishedOrder.TableName][g_eFinishedItem].Description);
			Controls.ProductionItemName:LocalizeAndSetText("TXT_KEY_CITYVIEW_FINISHED", strItemName);
			Controls.ProductionPortraitButton:RegisterCallback(Mouse.eRClick, GetProdHelp);
			Hide(Controls.ProductionPortrait, Controls.ProductionOutput, Controls.ProductionTurnsLabel);
			Show(Controls.ProductionBox);
		else
			-- Is the city producing anything at all?
			local eUnitProduction = pCity:GetProductionUnit();
			local eBuildingProduction = pCity:GetProductionBuilding();
			local eProjectProduction = pCity:GetProductionProject();
			local eProcessProduction = pCity:GetProductionProcess();
			local eItem = -1;
			local eOrder = OrderTypes.NO_ORDER;

			if eUnitProduction ~= -1 then
				eOrder = OrderTypes.ORDER_TRAIN;
				eItem = eUnitProduction;
			elseif eBuildingProduction ~= -1 then
				eOrder = OrderTypes.ORDER_CONSTRUCT;
				eItem = eBuildingProduction;
			elseif eProjectProduction ~= -1 then
				eOrder = OrderTypes.ORDER_CREATE;
				eItem = eProjectProduction;
			elseif eProcessProduction ~= -1 then
				eOrder = OrderTypes.ORDER_MAINTAIN;
				eItem = eProcessProduction;
			end

			local tOrder = tOrderTypeDetails[eOrder];
			if tOrder then
				local kInfo = GameInfo[tOrder.TableName][eItem];

				-- Production item name
				strItemName = pCity:GetProductionNameKey();
				Controls.ProductionItemName:LocalizeAndSetText(strItemName);

				-- Portrait
				local iPortraitIndex;
				local strAtlas;
				if eOrder == OrderTypes.ORDER_TRAIN then
					iPortraitIndex, strAtlas = UI.GetUnitPortraitIcon(eItem, eActivePlayer);
				else
					iPortraitIndex = kInfo.PortraitIndex;
					strAtlas = kInfo.IconAtlas;
				end
				IconHookupOrDefault(iPortraitIndex, 128, strAtlas, Controls.ProductionPortrait);
				Controls.ProductionPortrait:SetToolTipCallback(function ()
					-- We can't be sure that the original city pointer is still valid here
					local pCurrentCity = GetCurrentCity();
					if not pCurrentCity then
						return;
					end
					SetTooltip(tooltipInstance, string.format("%s[NEWLINE][NEWLINE]%s", tOrder.HelpTextFunc(eItem, pCurrentCity), L("TXT_KEY_CITYVIEW_PROD_METER_TT")));
				end);
				Controls.ProductionPortraitButton:RegisterCallback(Mouse.eRClick, GetProdHelp);

				-- Production stored and needed
				local fProductionStored = pCity:GetProductionTimes100() / 100 + pCity:GetTotalOverflowProductionTimes100() / 100;
				local iProductionNeeded = pCity:GetProductionNeeded();

				-- Production per turn
				Controls.ProductionOutput:LocalizeAndSetText("TXT_KEY_CITY_SCREEN_PROD_PER_TURN", fProductionPerTurn);

				-- Progress info for meter
				local bIsProcess = pCity:IsProductionProcess();
				local fProductionProgress = bIsProcess and 0 or fProductionStored / iProductionNeeded;
				local fProductionProgressNextTurn = bIsProcess and 0 or (fProductionStored + fProductionPerTurn) / iProductionNeeded;
				fProductionProgressNextTurn = math.min(fProductionProgressNextTurn, 1);
				Controls.ProductionMeter:SetPercents(fProductionProgress, fProductionProgressNextTurn);

				-- Turns left
				local strNumTurns = "";
				if bIsProcess or fProductionPerTurn == 0 then
					Controls.ProductionTurnsLabel:SetText("");
				else
					local iTurnsLeft = pCity:GetProductionTurnsLeft();
					if iTurnsLeft > 99 then
						strNumTurns = L("TXT_KEY_PRODUCTION_HELP_99PLUS_TURNS");
					else
						strNumTurns = L("TXT_KEY_PRODUCTION_HELP_NUM_TURNS", iTurnsLeft);
					end
					Controls.ProductionTurnsLabel:SetText(string.format("(%s)", strNumTurns));
				end

				Show(Controls.ProductionPortrait, Controls.ProductionBox, Controls.ProductionOutput, Controls.ProductionTurnsLabel);
			else
				-- City is not producing anything
				Hide(Controls.ProductionBox);
			end
		end
	end

	if g_bProductionMode then
		Controls.PurchaseIcon:SetText("[ICON_GOLD]/[ICON_PEACE]");
		Controls.PurchaseString:LocalizeAndSetText("TXT_KEY_CITYVIEW_PURCHASE_BUTTON");
		Controls.PurchaseButton:LocalizeAndSetToolTip("TXT_KEY_CITYVIEW_PURCHASE_TT");
	else
		Controls.PurchaseIcon:SetText("[ICON_PRODUCTION]");
		Controls.PurchaseString:LocalizeAndSetText("TXT_KEY_CITYVIEW_PRODUCE_BUTTON");
		Controls.PurchaseButton:LocalizeAndSetToolTip(g_bQueueMode and "TXT_KEY_CITYVIEW_QUEUE_PROD_TT" or "TXT_KEY_CITYVIEW_CHOOSE_PROD_TT");
	end

	-------------------------------------------
	-- Top left
	-------------------------------------------
	Controls.CityName:SetText(pCity:GetName());
	Controls.CityButton:SetVoids(pCity:GetX(), pCity:GetY());

	local kCivInfo = GameInfo.Civilizations[pActivePlayer:GetCivilizationType()];
	IconHookupOrDefault(kCivInfo.PortraitIndex, 32, kCivInfo.IconAtlas, Controls.CivIcon);

	local iPopulation = pCity:GetPopulation();
	local fFoodPerTurn = pCity:GetYieldRateTimes100(YieldTypes.YIELD_FOOD) / 100;
	local strFoodTooltip = GetFoodTooltip(pCity);
	Controls.GrowthBar:SetPercent(pCity:GetFood() / pCity:GrowthThreshold());
	Controls.Food:SetText("[ICON_FOOD]" .. math.floor(fFoodPerTurn));
	Controls.PopulationSuffix:LocalizeAndSetText("TXT_KEY_CITYVIEW_CITIZENS_TEXT", iPopulation);
	Controls.Population:SetText(iPopulation);
	Controls.Food:SetToolTipString(strFoodTooltip);
	Controls.PopBox:SetToolTipString(strFoodTooltip);

	Controls.Production:SetText("[ICON_PRODUCTION]" .. math.floor(fProductionPerTurn));
	Controls.Production:SetToolTipString(GetProductionTooltip(pCity));

	Controls.Gold:SetText("[ICON_GOLD]" .. math.floor(pCity:GetYieldRateTimes100(YieldTypes.YIELD_GOLD) / 100));
	Controls.Gold:SetToolTipString(GetGoldTooltip(pCity));

	if Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE) then
		Controls.Science:SetText("[ICON_RESEARCH]" .. OFF_STRING);
	else
		Controls.Science:SetText("[ICON_RESEARCH]" .. math.floor(pCity:GetYieldRateTimes100(YieldTypes.YIELD_SCIENCE) / 100));
	end
	Controls.Science:SetToolTipString(GetScienceTooltip(pCity));

	Controls.Culture:SetText("[ICON_CULTURE]" .. pCity:GetYieldRateTimes100(YieldTypes.YIELD_CULTURE) / 100);
	Controls.Culture:SetToolTipString(GetCultureTooltip(pCity));

	if Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION) then
		Controls.Faith:SetText("[ICON_PEACE]" .. OFF_STRING);
	else
		Controls.Faith:SetText("[ICON_PEACE]" .. math.floor(pCity:GetYieldRateTimes100(YieldTypes.YIELD_FAITH) / 100));
	end
	Controls.Faith:SetToolTipString(GetFaithTooltip(pCity));

	if MOD_BALANCE_VP then
		Controls.Happiness:SetText("[ICON_HAPPINESS_1]" .. pCity:GetLocalHappiness());
		Controls.Unhappiness:SetText("[ICON_HAPPINESS_3]" .. pCity:GetUnhappinessAggregated());
		Controls.Happiness:SetToolTipString(GetCityHappinessTooltip(pCity));
		Controls.Unhappiness:SetToolTipString(GetCityUnhappinessTooltip(pCity));
	else
		Hide(Controls.HappinessDivider, Controls.HappinessGrid, Controls.UnhappinessDivider, Controls.UnhappinessGrid);
	end

	-- Set width of controls
	Refresh(Controls.YieldStack);
	local iStackWidth = Controls.YieldStack:GetSizeX();
	Controls.TopLeftContainer:SetSizeX(iStackWidth + 52);
	Controls.TopLeftGrid:SetSizeX(iStackWidth + 52);
	Controls.YieldDivider:SetSizeX(iStackWidth);
	Controls.CityDivider:SetSizeX(iStackWidth);
	Controls.YieldBox:SetSizeX(iStackWidth);
	Controls.TopLeftInnerGrid:SetSizeX(iStackWidth);
	Controls.CityButton:SetSizeX(iStackWidth);

	if fFoodPerTurn == 0 then
		Controls.CityGrowthLabel:SetText(L("TXT_KEY_CITYVIEW_STAGNATION_TEXT"));
	elseif fFoodPerTurn < 0 then
		Controls.CityGrowthLabel:SetText(L("TXT_KEY_CITYVIEW_STARVATION_TEXT"));
	else
		Controls.CityGrowthLabel:SetText(L("TXT_KEY_CITYVIEW_TURNS_TILL_CITIZEN_TEXT", pCity:GetFoodTurnsLeft()));
	end

	-------------------------------------------
	-- Production list
	-------------------------------------------
	Game.SetAdvisorRecommenderCity(pCity);

	local tItemLists = {};
	for eOrder in pairs(tOrderTypeDetails) do
		tItemLists[eOrder] = {};
	end

	--- @param eOrder OrderType
	--- @param eItem UnitType|BuildingType|ProjectType|ProcessType
	--- @param kInfo Info
	--- @return table
	local function AssembleItem(eOrder, eItem, kInfo, eYield)
		local tOrder = tOrderTypeDetails[eOrder];
		local strPrereqTechKey = ""
		if (eOrder == 0 or eOrder == 1) then
			-- Buildings, Units
			strPrereqTechKey = kInfo.PrereqTech;
		else
			-- Projects, Processes
			strPrereqTechKey = kInfo.TechPrereq;
		end
		local item = {
			ID = eItem,
			Name = L(kInfo.Description),
			OrderType = eOrder,
			YieldType = eYield,
		};

		item.PrereqTechX = strPrereqTechKey and GameInfo.Technologies[strPrereqTechKey].GridX or -1;

		if eYield == YieldTypes.NO_YIELD and bGeneratingProduction and eOrder ~= OrderTypes.ORDER_MAINTAIN then
			item.TurnsOrCost = L("TXT_KEY_STR_TURNS", tOrder.TurnsLeftFunc(eItem, pCity, 1));
		elseif eYield ~= YieldTypes.NO_YIELD and tOrder.CostFunc then
			item.TurnsOrCost = tOrder.CostFunc(eItem, pCity, eYield);
		else
			item.TurnsOrCost = INFINITE_TURNS_STRING;
		end

		if eOrder == OrderTypes.ORDER_TRAIN then
			item.IsCivilian = (kInfo.Combat == 0 and kInfo.RangedCombat == 0);
			item.IsAir = (kInfo.Domain == "DOMAIN_AIR");
			item.IsSea = (kInfo.Domain == "DOMAIN_SEA");
			item.IsLand = (kInfo.Domain == "DOMAIN_LAND");
		elseif eOrder == OrderTypes.ORDER_CONSTRUCT then
			local kBuildingClassInfo = GameInfo.BuildingClasses[kInfo.BuildingClass];
			item.IsNationalWonder = (kBuildingClassInfo.MaxPlayerInstances == 1);
			item.IsTeamWonder = (kBuildingClassInfo.MaxTeamInstances > -1);
			item.IsWorldWonder = (kBuildingClassInfo.MaxGlobalInstances > -1);
		end

		return item;
	end

	for eOrder, tOrder in pairs(tOrderTypeDetails) do
		for eItem, kInfo in GameInfoCache(tOrder.TableName) do
			-- Wonders belong with projects
			local tItemList = tItemLists[eOrder];
			if eOrder == OrderTypes.ORDER_CONSTRUCT then
				local kBuildingClassInfo = GameInfo.BuildingClasses[kInfo.BuildingClass];
				if kBuildingClassInfo.MaxGlobalInstances > -1 or kBuildingClassInfo.MaxPlayerInstances == 1 or kBuildingClassInfo.MaxTeamInstances > -1 then
					tItemList = tItemLists[OrderTypes.ORDER_CREATE];
				end
			end

			if g_bProductionMode then
				if tOrder.CanProduceFunc(eItem, pCity, true) then
					table.insert(tItemList, AssembleItem(eOrder, eItem, kInfo, YieldTypes.NO_YIELD));
				end
			else
				if tOrder.CanPurchaseFunc(eItem, pCity, YieldTypes.YIELD_GOLD, true) then
					table.insert(tItemList, AssembleItem(eOrder, eItem, kInfo, YieldTypes.YIELD_GOLD));
				end
				if tOrder.CanPurchaseFunc(eItem, pCity, YieldTypes.YIELD_FAITH, true) then
					table.insert(tItemList, AssembleItem(eOrder, eItem, kInfo, YieldTypes.YIELD_FAITH));
				end
			end
		end
	end

	-- Add section headers and item lists in order
	local tOrderOfOrders = {OrderTypes.ORDER_TRAIN, OrderTypes.ORDER_CONSTRUCT, OrderTypes.ORDER_CREATE, OrderTypes.ORDER_MAINTAIN};
	for _, eOrder in ipairs(tOrderOfOrders) do
		local tItemList = tItemLists[eOrder];
		table.sort(tItemList, function (item1, item2)
			if eOrder == OrderTypes.ORDER_TRAIN then
				-- Sort order: civilians, land units, sea units, air units
				if item1.IsCivilian ~= item2.IsCivilian then
					return item1.IsCivilian or not item2.IsCivilian;
				end
				if item1.IsLand ~= item2.IsLand then
					return item1.IsLand or not item2.IsLand;
				end
				if item1.IsSea ~= item2.IsSea then
					return item1.IsSea or not item2.IsSea;
				end
				if item1.IsAir ~= item2.IsAir then
					return item1.IsAir or not item2.IsAir;
				end
			elseif eOrder == OrderTypes.ORDER_CONSTRUCT then
				-- Sort order: national wonders, team wonders, world wonders, projects
				if item1.IsNationalWonder ~= item2.IsNationalWonder then
					return item1.IsNationalWonder or not item2.IsNationalWonder;
				end
				if item1.IsTeamWonder ~= item2.IsTeamWonder then
					return item1.IsTeamWonder or not item2.IsTeamWonder;
				end
				if item1.IsWorldWonder ~= item2.IsWorldWonder then
					return item1.IsWorldWonder or not item2.IsWorldWonder;
				end
			end

			-- Sort order: prereq tech column, name, ID (for tiebreak in case items share the same name), yield type
			if item1.PrereqTechX ~= item2.PrereqTechX then
				return item1.PrereqTechX < item2.PrereqTechX;
			end
			if item1.Name ~= item2.Name then
				return item1.Name < item2.Name;
			end
			if item1.ID ~= item2.ID then
				return item1.ID < item2.ID;
			end
			return item1.YieldType < item2.YieldType;
		end);

		for _, item in ipairs(tItemList) do
			AddProductionButton(item.ID, item.OrderType, item.YieldType, item.TurnsOrCost);
		end

		local tHeader = tOrderTypeDetails[eOrder].Header;
		HandleAppearance(tHeader);
		Controls[tHeader.Name]:SetHide(not next(tItemList));
	end

	-- Resize stacks and scrollbars
	Refresh(Controls.UnitButtonStack, Controls.BuildingButtonStack, Controls.WonderButtonStack, Controls.OtherButtonStack, Controls.StackOStacks, Controls.ScrollPanel);
end

-------------------------------------------------
--- Can this city purchase or produce anything right now?
--- @param pCity City
--- @return boolean
local function MayPurchaseOrProduceAnything(pCity)
	if not pCity:IsPuppet() then
		return true;
	end

	if g_bProductionMode then
		return false;
	end

	if pActivePlayer:MayNotAnnex() then
		return true;
	end

	-- IsCanPurchase already checks for everything, but we should avoid calling it for every building/unit ID
	for kBuildingInfo in GameInfo.Buildings("AllowsPuppetPurchase = 1 OR PuppetPurchaseOverride = 1") do
		if kBuildingInfo.AllowsPuppetPurchase and pCity:IsHasBuilding(kBuildingInfo.ID) then
			return true;
		elseif kBuildingInfo.PuppetPurchaseOverride then
			if pCity:IsCanPurchase(false, false, -1, kBuildingInfo.ID, -1, YieldTypes.YIELD_GOLD) then
				return true;
			elseif pCity:IsCanPurchase(false, false, -1, kBuildingInfo.ID, -1, YieldTypes.YIELD_FAITH) then
				return true;
			end
		end
	end

	for kUnitInfo in GameInfo.Units{PuppetPurchaseOverride = 1} do
		if pCity:IsCanPurchase(false, false, kUnitInfo.ID, -1, -1, YieldTypes.YIELD_GOLD) then
			return true;
		elseif pCity:IsCanPurchase(false, false, kUnitInfo.ID, -1, -1, YieldTypes.YIELD_FAITH) then
			return true;
		end
	end

	return false;
end

--- On display
--- @param popupInfo PopupInfo
Events.SerialEventGameMessagePopup.Add(function (popupInfo)
	if popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_CHOOSEPRODUCTION then
		return;
	end

	local pCity = pActivePlayer:GetCityByID(popupInfo.Data1);
	assert(pCity, "Invalid city ID!");

	-- Purchase mode?
	g_bProductionMode = not popupInfo.Option2;

	-- Don't waste time opening the popup if the city can't actually purchase/produce anything
	if not MayPurchaseOrProduceAnything(pCity) then
		return;
	end

	g_popupInfo = popupInfo;
	g_eOrder = popupInfo.Data2;
	g_eFinishedItem = popupInfo.Data3;
	g_bQueueMode = popupInfo.Option1;

	CivIconHookup(eActivePlayer, 32, Controls.CivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true);
	UpdateWindow(pCity);
	Show(ContextPtr);
end);

-------------------------------------------------
local function OnDirty()
	if not g_bHidden then
		UpdateWindow(UI.GetHeadSelectedCity() or GetCurrentCity());
	end
end

Events.SerialEventCityScreenDirty.Add(OnDirty);
Events.SerialEventCityInfoDirty.Add(OnDirty);
Events.SerialEventGameDataDirty.Add(OnDirty);

-------------------------------------------------
--- @param ePlayer PlayerId
--- @param eCity CityId
local function OnCityDestroyed(_, ePlayer, eCity)
	if ePlayer == eActivePlayer and eCity == eCurrentCity then
		Close();
	end
end

Events.SerialEventCityDestroyed.Add(OnCityDestroyed);
Events.SerialEventCityCaptured.Add(OnCityDestroyed);

----------------------------------------------------------------
-- Input handling
--- @param uiMsg KeyEventType
--- @param wParam KeyType
ContextPtr:SetInputHandler(function (uiMsg, wParam)
	if uiMsg == KeyEvents.KeyDown then
		if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
			Close();
			return true;
		end
	end
end);

----------------------------------------------------------------
--- @param bIsHide boolean
--- @param bInitState boolean
ContextPtr:SetShowHideHandler(function (bIsHide, bInitState)
	if not bInitState then
		if not bIsHide then
			Events.SerialEventGameMessagePopupShown(g_popupInfo);
		else
			Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_CHOOSEPRODUCTION, 0);
		end
		g_bHidden = bIsHide;

		-- Call CityView.lua
		LuaEvents.ProductionPopup(bIsHide);
	end
end);

----------------------------------------------------------------
Controls.PurchaseButton:RegisterCallback(Mouse.eLClick, function ()
	g_bProductionMode = not g_bProductionMode;
	UpdateWindow();
end);

----------------------------------------------------------------
-- 'Active' (local human) player has changed
Events.GameplaySetActivePlayer.Add(function ()
	eActivePlayer = Game.GetActivePlayer();
	pActivePlayer = Players[eActivePlayer];
	if not ContextPtr:IsHidden() then
		Close();
	end
end);
