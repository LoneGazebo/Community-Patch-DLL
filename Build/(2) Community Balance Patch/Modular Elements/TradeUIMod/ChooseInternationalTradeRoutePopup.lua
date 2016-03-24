print("This is the modded ChooseInternationalTradeRoutePopup from 'UI - Trade Route Enhancements'")

-------------------------------------------------
-- Choose International Trade Route Popup
-------------------------------------------------

include( "IconSupport" );
include( "InstanceManager" );
include( "CommonBehaviors" );
include( "TradeRouteHelpers" );

-- Used for Piano Keys
local ltBlue = {19/255,32/255,46/255,120/255};
local dkBlue = {12/255,22/255,30/255,120/255};

local g_ItemManagers = {
	InstanceManager:new( "ItemInstance", "Button", Controls.YourCitiesStack ),
	InstanceManager:new( "ItemInstance", "Button", Controls.MajorCivsStack ),
	InstanceManager:new( "ItemInstance", "Button", Controls.CityStatesStack ),
}

g_iUnitIndex = -1;
g_iPlayer = -1;

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
--print(heightOffset);
--print(bpHeight);
bpHeight = bpHeight + heightOffset 
--print(bpHeight);

local pUnitPlot;

Controls.BottomPanel:SetSizeVal(bpWidth, bpHeight);
Controls.BottomPanel:ReprocessAnchoring();

local g_PopupInfo = nil;
-------------------------------------------------
-------------------------------------------------
function OnPopupMessage(popupInfo)
	local popupType = popupInfo.Type;
	if popupType ~= ButtonPopupTypes.BUTTONPOPUP_CHOOSE_INTERNATIONAL_TRADE_ROUTE then
		return;
	end	
	
	g_PopupInfo = popupInfo;
	
	g_iUnitIndex = popupInfo.Data2;
   	UIManager:QueuePopup( ContextPtr, PopupPriority.SocialPolicy );
   	Events.Event_ToggleTradeRouteDisplay(true);
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
			if(not Controls.ChooseConfirm:IsHidden()) then
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
function TradeOverview()
	local popupInfo = {
		Type = ButtonPopupTypes.BUTTONPOPUP_TRADE_ROUTE_OVERVIEW,
	}
	Events.SerialEventGameMessagePopup(popupInfo);
end
Controls.TradeOverviewButton:RegisterCallback( Mouse.eLClick, TradeOverview );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnClose()
    UIManager:DequeuePopup(ContextPtr);
	Events.Event_ToggleTradeRouteDisplay(OptionsManager.GetShowTradeOn());
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function RefreshData()
	
	local iActivePlayer = Game.GetActivePlayer();
	local pPlayer = Players[iActivePlayer];
	local pUnit = pPlayer:GetUnitByID(g_iUnitIndex);
	if (pUnit == nil) then
		return;
	end
	
	local map = Map;
	
	local pOriginPlot = pUnit:GetPlot();
	local pOriginCity = pOriginPlot:GetPlotCity();
	local eDomain = pUnit:GetDomainType();
		
	local BonusTips = 
	{
		[YieldTypes.YIELD_FOOD] = "TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_FOOD_SHORT",	
		[YieldTypes.YIELD_PRODUCTION] ="TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_PRODUCTION_SHORT",
		[YieldTypes.YIELD_GOLD] ="TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_GOLD_SHORT",
		[YieldTypes.YIELD_SCIENCE] ="TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_SCIENCE_SHORT",
		[YieldTypes.YIELD_CULTURE] = "TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_CULTURE_SHORT",
		[YieldTypes.YIELD_FAITH] = 	"TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_FAITH_SHORT",
	}
	
	
	g_Model = {};
	
	local potentialTradeSpots = pPlayer:GetPotentialInternationalTradeRouteDestinations(pUnit);
	for i,v in ipairs(potentialTradeSpots) do
		
		local tradeRoute = {
			PlotX = v.X,
			PlotY = v.Y,
			TradeConnectionType = v.TradeConnectionType
		};
		
		local pTargetCity = nil;
		local iTargetOwner = nil;
		local pLoopPlot = map.GetPlot(v.X, v.Y);
		if(pLoopPlot ~= nil) then
			pTargetCity = pLoopPlot:GetPlotCity();
			iTargetOwner = pTargetCity:GetOwner();
		end		
	
		local pTargetPlayer = Players[iTargetOwner];
-- CBP
		local strTraitText = "";
		if (pTargetPlayer ~= nil and pTargetPlayer:IsMinorCiv()) then		
			local iTrait = pTargetPlayer:GetMinorCivTrait();
			if (iTrait == MinorCivTraitTypes.MINOR_CIV_TRAIT_CULTURED) then
				strTraitText = " (" .. Locale.Lookup("TXT_KEY_CITY_STATE_CULTURED_ADJECTIVE") .. ") ";
			elseif (iTrait == MinorCivTraitTypes.MINOR_CIV_TRAIT_MILITARISTIC) then
				strTraitText = " (" .. Locale.Lookup("TXT_KEY_CITY_STATE_MILITARISTIC_ADJECTIVE") .. ") ";
			elseif (iTrait == MinorCivTraitTypes.MINOR_CIV_TRAIT_MARITIME) then
				strTraitText = " (" .. Locale.Lookup("TXT_KEY_CITY_STATE_MARITIME_ADJECTIVE") .. ") ";
			elseif (iTrait == MinorCivTraitTypes.MINOR_CIV_TRAIT_MERCANTILE) then
				strTraitText = " (" .. Locale.Lookup("TXT_KEY_CITY_STATE_MERCANTILE_ADJECTIVE") .. ") ";
			elseif (iTrait == MinorCivTraitTypes.MINOR_CIV_TRAIT_RELIGIOUS) then
				strTraitText = " (" .. Locale.Lookup("TXT_KEY_CITY_STATE_RELIGIOUS_ADJECTIVE") .. ") ";
			end
		end
		tradeRoute.CityName = (pTargetCity:GetName() .. strTraitText);
--END		
--EDITED		tradeRoute.CityName = pTargetCity:GetName();

		tradeRoute.CityIcons = "";
		tradeRoute.CivName = pTargetPlayer:GetCivilizationDescription();	
		
		if(iTargetOwner == iActivePlayer) then
			tradeRoute.Category = 1;
		elseif(iTargetOwner < GameDefines.MAX_MAJOR_CIVS) then
			tradeRoute.Category = 2;
-- CBP
			if (pTargetCity ~= nil and pTargetCity:IsFranchised(iActivePlayer)) then
				tradeRoute.CityIcons = " [ICON_INVEST]"
			end
-- END
		else
			tradeRoute.Category = 3;	-- City States

			if (pTargetPlayer:IsMinorCivActiveQuestForPlayer(iActivePlayer, MinorCivQuestTypes.MINOR_CIV_QUEST_TRADE_ROUTE)) then
				tradeRoute.CityIcons = " [ICON_INTERNATIONAL_TRADE]"
			end
-- CBP
			if (pTargetCity ~= nil and pTargetCity:IsFranchised(iActivePlayer)) then
				tradeRoute.CityIcons = " [ICON_INVEST]"
			end
-- END
		end
		
		local myBonuses = "";
		local theirBonuses = "";
		
		for j,u in ipairs(v.Yields) do
			local iYield = j - 1;
			
			if(iYield == YieldTypes.YIELD_GOLD) then
				tradeRoute.Gold = u.Mine;
				tradeRoute.GoldDelta = u.Mine - u.Theirs;
			elseif (iYield == YieldTypes.YIELD_SCIENCE) then
				tradeRoute.Science = u.Mine;
				tradeRoute.ScienceDelta = u.Mine - u.Theirs;
--			end
-- CBP
			elseif (iYield == YieldTypes.YIELD_PRODUCTION) then
				tradeRoute.Production = u.Theirs;
			elseif (iYield == YieldTypes.YIELD_FOOD) then
				tradeRoute.Food = u.Theirs;
			end
-- END
			
			local entry = BonusTips[iYield];
			if(entry ~= nil) then
				if(u.Mine ~= 0) then
					myBonuses = myBonuses .. "  " .. Locale.Lookup(entry, u.Mine / 100);
				end
				
				if(u.Theirs ~= 0) then
					theirBonuses = theirBonuses .. "  " .. Locale.Lookup(entry, u.Theirs / 100);
				end
			end
		end
		
		if (v.TradeConnectionType == TradeConnectionTypes.TRADE_CONNECTION_WONDER_RESOURCE) then
			theirBonuses = theirBonuses .. "  [ICON_RES_MARBLE]";
		end
		
		if (v.FromReligion ~= 0 and v.FromPressureAmount ~= 0) then
			local religion = GameInfo.Religions[v.FromReligion];
			myBonuses = myBonuses .. "  " .. Locale.Lookup("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_RELIGIOUS_PRESSURE_SHORT", religion.IconString, v.FromPressureAmount);
		end
		
		if (v.ToReligion ~= 0 and v.ToPressureAmount ~= 0) then
			local religion = GameInfo.Religions[v.ToReligion];
			theirBonuses = theirBonuses .. "  " .. Locale.Lookup("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_RELIGIOUS_PRESSURE_SHORT", religion.IconString, v.ToPressureAmount);
		end
		
		if (myBonuses ~= "") then
			myBonuses = "[ICON_ARROW_LEFT]" .. myBonuses;
			
			if (theirBonuses ~= "") then
				myBonuses = myBonuses .. "[NEWLINE]";
			end
		end
		
		if (theirBonuses ~= "") then
			theirBonuses = "[ICON_ARROW_RIGHT]" .. theirBonuses;
		end
-- CBP
		local otherBonuses = "";
		tradeRoute.OtherIsolation = 0;
		if (pTargetCity ~= nil and pPlayer ~= nil and tradeRoute.Category == 1) then
			local iIsolation = pTargetCity:GetUnhappinessFromConnection();
			if(iIsolation > 0 and pOriginCity:IsConnectedToCapital()) then
				tradeRoute.OtherIsolation = iIsolation;
				otherBonuses = otherBonuses .. Locale.Lookup("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_ISOLATION_OTHER", -iIsolation);
			end
		end
		tradeRoute.OtherInfluence = 0;
		if (pTargetCity ~= nil and pTargetPlayer ~= nil and tradeRoute.Category == 2) then
			local iInfluence = pPlayer:GetInfluenceOn(iTargetOwner);
			local iCulture = pTargetPlayer:GetJONSCultureEverGenerated();
			local iPercent = 0;
			if (iCulture > 0) then
				iPercent = iInfluence / iCulture;
			end
			if(iPercent > 0) then
				tradeRoute.OtherInfluence = (iPercent * 100);
				if(otherBonuses ~= "") then
					otherBonuses = otherBonuses .. ", ";
				end
				otherBonuses = otherBonuses .. Locale.Lookup("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_INFLUENCE_OTHER", Locale.ToNumber((iPercent * 100), "#.#"));
			end
		end
		if (pTargetCity ~= nil and tradeRoute.Category ~= 1) then
			if(pPlayer:IsConnectedToPlayer(iTargetOwner)) then
				if(otherBonuses ~= "") then
					otherBonuses = otherBonuses .. ", ";
				end
				otherBonuses = otherBonuses .. Locale.Lookup("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_CONNECTED");
			end
		end
		if (myBonuses ~= "" or theirBonuses ~= "") then
			if(otherBonuses ~= "") then
				otherBonuses = "[NEWLINE]" .. "[ICON_INTERNATIONAL_TRADE] " .. otherBonuses;
			end
		end

		tradeRoute.Bonuses = myBonuses .. theirBonuses .. otherBonuses;
-- END		
-- EDITED tradeRoute.Bonuses = myBonuses .. theirBonuses;
		tradeRoute.TargetPlayerId = pTargetPlayer:GetID();
		tradeRoute.ToolTip = BuildTradeRouteToolTipString(pPlayer, pOriginCity, pTargetCity, eDomain);
		tradeRoute.eDomain = eDomain;
		
		if (v.OldTradeRoute) then
			tradeRoute.PrevRoute = Locale.Lookup("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_PREV_ROUTE");
		else
			tradeRoute.PrevRoute = "";
		end
		
		table.insert(g_Model, tradeRoute);
	end
end

function SortByCityName(a, b)
	return Locale.Compare(a.CityName, b.CityName) == -1;
end

function SortByCivName(a, b)
	local result = Locale.Compare(a.CivName, b.CivName);
	if(result == 0) then
		return SortByCityName(a,b);
	else
		return result == -1;
	end
end

function SortByMaxGold(a, b)
	return a.Gold > b.Gold;
end

function SortByGoldDelta(a, b)
	return a.GoldDelta > b.GoldDelta;
end

function SortByMaxScience(a, b)
	return a.Science > b.Science;
end

function SortByScienceDelta(a, b)
	return a.ScienceDelta > b.ScienceDelta;
end

--CBP
function SortByOtherIsolation(a, b)
	return a.OtherIsolation > b.OtherIsolation;
end
function SortByOtherInfluence(a, b)
	return a.OtherInfluence > b.OtherInfluence;
end
function SortByFood(a, b)
	return a.Food > b.Food;
end
function SortByProduction(a, b)
	return a.Production > b.Production;
end
--END

g_SortOptions = {
	{"TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_SORT_CIVNAME", SortByCivName},
	{"TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_SORT_CITYNAME", SortByCityName},
	{"TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_SORT_MAX_GOLD", SortByMaxGold},
	{"TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_SORT_MAX_GOLD_DELTA", SortByGoldDelta}, 
	{"TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_SORT_MAX_SCIENCE", SortByMaxScience},
	{"TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_SORT_MAX_SCIENCE_DELTA", SortByScienceDelta},
	--CBP
	{"TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_SORT_ISOLATION_OTHER", SortByOtherIsolation},
	{"TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_SORT_INFLUENCE_OTHER", SortByOtherInfluence},
	{"TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_SORT_PRODUCTION", SortByProduction},
	{"TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_SORT_FOOD", SortByFood},
	--END
}
g_CurrentSortOption = 1;

function SortData()
	table.sort(g_Model, g_SortOptions[g_CurrentSortOption][2]);
end


function Initialize()
	local pPlayer = Players[Game.GetActivePlayer()];
	local pUnit = pPlayer:GetUnitByID(g_iUnitIndex);
	if (pUnit == nil) then
		return;
	end
	
	local pOriginPlot = pUnit:GetPlot();
	local pOriginCity = pOriginPlot:GetPlotCity();
	
	local eDomain = pUnit:GetDomainType();
	local iRange = pPlayer:GetTradeRouteRange(eDomain, pOriginCity);

	CivIconHookup( pPlayer:GetID(), 64, Controls.CivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true );
		
	local unitType = pUnit:GetUnitType();
	local unitInfo = GameInfo.Units[unitType];
	
	local portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon(unitType, pPlayer:GetID());
	IconHookup(portraitOffset, 64, portraitAtlas, Controls.TradeUnitIcon);
	
	Controls.StartingCity:LocalizeAndSetText("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_STARTING_CITY", pOriginCity:GetName());
	Controls.UnitInfo:LocalizeAndSetText("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_TRADE_UNIT", pUnit:GetName());
	Controls.UnitRange:LocalizeAndSetText("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_RANGE", iRange);
	
	
	local sortByPulldown = Controls.SortByPullDown;
	sortByPulldown:ClearEntries();
	for i, v in ipairs(g_SortOptions) do
		local controlTable = {};
		sortByPulldown:BuildEntry( "InstanceOne", controlTable );
		controlTable.Button:LocalizeAndSetText(v[1]);
		
		controlTable.Button:RegisterCallback(Mouse.eLClick, function()
			sortByPulldown:GetButton():LocalizeAndSetText(v[1]);
			g_CurrentSortOption = i;
			
			SortData();
			DisplayData();
		end);
	end
	sortByPulldown:CalculateInternals();
	sortByPulldown:GetButton():LocalizeAndSetText(g_SortOptions[g_CurrentSortOption][1]);
end

function DisplayData()
	
	for _, itemManager in ipairs(g_ItemManagers) do
		itemManager:ResetInstances();
	end
		
	for _, tradeRoute in ipairs(g_Model) do
		
		local itemInstance = g_ItemManagers[tradeRoute.Category]:GetInstance();
--CBP
		if(tradeRoute.CityIcons == " [ICON_INVEST]") then
			local pPlayer = Players[Game.GetActivePlayer()];
			if(pPlayer ~= nil) then
				local strCorpInfo = Locale.Lookup("TXT_KEY_CORP_HERE");
				itemInstance.CityName:SetText(tradeRoute.CityName .. strCorpInfo);
			else
				itemInstance.CityName:SetText(tradeRoute.CityName .. tradeRoute.CityIcons);		
			end
		else
			itemInstance.CityName:SetText(tradeRoute.CityName .. tradeRoute.CityIcons);		
		end
--END
		itemInstance.Bonuses:SetText(tradeRoute.Bonuses);
		itemInstance.PrevRoute:SetText(tradeRoute.PrevRoute);

		CivIconHookup(tradeRoute.TargetPlayerId, 32, itemInstance.CivIcon, itemInstance.CivIconBG, itemInstance.CivIconShadow, false, true );
		itemInstance.Button:RegisterCallback(Mouse.eLClick, function() SelectTradeDestinationChoice(tradeRoute.PlotX, tradeRoute.PlotY, tradeRoute.TradeConnectionType); end);
		itemInstance.Button:SetToolTipString(tradeRoute.ToolTip);
		itemInstance.Box:SetColorVal(unpack(ltBlue));
		local buttonWidth, buttonHeight = itemInstance.Button:GetSizeVal();
		local descWidth, descHeight = itemInstance.Bonuses:GetSizeVal();

		
		-- local newHeight = math.max(80, descHeight + 40);	
		local newHeight = math.max(40, descHeight + 35);	
		
		itemInstance.Button:SetSizeVal(buttonWidth, newHeight);
		itemInstance.Box:SetSizeVal(buttonWidth, newHeight);
		itemInstance.BounceAnim:SetSizeVal(buttonWidth, newHeight + 5);
		itemInstance.BounceGrid:SetSizeVal(buttonWidth, newHeight + 5);
		
		itemInstance.GoToCity:RegisterCallback(Mouse.eLClick, function() 
			local plot = Map.GetPlot(tradeRoute.PlotX, tradeRoute.PlotY);
			UI.LookAt(plot, 0);  
		end);
		
		itemInstance.Button:RegisterCallback(Mouse.eMouseEnter, function() Game.SelectedUnit_SpeculativePopupTradeRoute_Display(tradeRoute.PlotX, tradeRoute.PlotY, tradeRoute.TradeConnectionType, tradeRoute.eDomain); end);
		itemInstance.Button:RegisterCallback(Mouse.eMouseExit, function() Game.SelectedUnit_SpeculativePopupTradeRoute_Hide(tradeRoute.PlotX, tradeRoute.PlotY, tradeRoute.TradeConnectionType); end);
	end

	Controls.YourCitiesStack:CalculateSize();
	Controls.YourCitiesStack:ReprocessAnchoring();
	Controls.MajorCivsStack:CalculateSize();
	Controls.MajorCivsStack:ReprocessAnchoring();
	Controls.CityStatesStack:CalculateSize();
	Controls.CityStatesStack:ReprocessAnchoring();
	Controls.ItemStack:CalculateSize();
	Controls.ItemStack:ReprocessAnchoring();
	Controls.ItemScrollPanel:CalculateInternalSize();

end

function SelectTradeDestinationChoice(iPlotX, iPlotY, iTradeConnectionType) 
	g_selectedPlotX = iPlotX;
	g_selectedPlotY = iPlotY;
	g_selectedTradeType = iTradeConnectionType;
	
	Controls.ChooseConfirm:SetHide(false);
end

function OnConfirmYes( )
	Controls.ChooseConfirm:SetHide(true);
	
	local kPlot = Map.GetPlot( g_selectedPlotX, g_selectedPlotY );	
	Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_ESTABLISH_TRADE_ROUTE, kPlot:GetPlotIndex(), g_selectedTradeType, 0, false, nil);
		
	Events.AudioPlay2DSound("AS2D_INTERFACE_CARAVAN_TRADE_ROUTE");	
	
	OnClose();	
end
Controls.ConfirmYes:RegisterCallback( Mouse.eLClick, OnConfirmYes );

function OnConfirmNo( )
	Controls.ChooseConfirm:SetHide(true);
end
Controls.ConfirmNo:RegisterCallback( Mouse.eLClick, OnConfirmNo );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )

	bHidden = bIsHide;
    if( not bInitState ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(g_PopupInfo);
        	
        	Initialize();
        	RefreshData();
        	SortData();
        	DisplayData();
        
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


function OnCollapseExpand()
	Controls.YourCitiesStack:CalculateSize();
	Controls.YourCitiesStack:ReprocessAnchoring();
	Controls.MajorCivsStack:CalculateSize();
	Controls.MajorCivsStack:ReprocessAnchoring();
	Controls.CityStatesStack:CalculateSize();
	Controls.CityStatesStack:ReprocessAnchoring();
	Controls.ItemStack:CalculateSize();
	Controls.ItemStack:ReprocessAnchoring();
	Controls.ItemScrollPanel:CalculateInternalSize();
end

RegisterCollapseBehavior{	
	Header = Controls.YourCitiesHeader, 
	HeaderLabel = Controls.YourCitiesHeaderLabel, 
	HeaderExpandedLabel = Locale.Lookup("TXT_KEY_CHOOSE_TRADE_ROUTE_YOUR_CITIES"),
	HeaderCollapsedLabel = Locale.Lookup("TXT_KEY_CHOOSE_TRADE_ROUTE_YOUR_CITIES_COLLAPSED"),
	Panel = Controls.YourCitiesStack,
	Collapsed = false,
	OnCollapse = OnCollapseExpand,
	OnExpand = OnCollapseExpand,
};
							
RegisterCollapseBehavior{
	Header = Controls.MajorCivsHeader,
	HeaderLabel = Controls.MajorCivsHeaderLabel,
	HeaderExpandedLabel = Locale.Lookup("TXT_KEY_CHOOSE_TRADE_ROUTE_MAJOR_CIVS"),
	HeaderCollapsedLabel = Locale.Lookup("TXT_KEY_CHOOSE_TRADE_ROUTE_MAJOR_CIVS_COLLAPSED"),
	Panel = Controls.MajorCivsStack,
	Collapsed = false,
	OnCollapse = OnCollapseExpand,
	OnExpand = OnCollapseExpand,
};

RegisterCollapseBehavior{
	Header = Controls.CityStatesHeader,
	HeaderLabel = Controls.CityStatesHeaderLabel,
	HeaderExpandedLabel = Locale.Lookup("TXT_KEY_CHOOSE_TRADE_ROUTE_CITY_STATES"),
	HeaderCollapsedLabel = Locale.Lookup("TXT_KEY_CHOOSE_TRADE_ROUTE_CITY_STATES_COLLAPSED"),
	Panel = Controls.CityStatesStack,
	Collapsed = false,
	OnCollapse = OnCollapseExpand,
	OnExpand = OnCollapseExpand,
};

