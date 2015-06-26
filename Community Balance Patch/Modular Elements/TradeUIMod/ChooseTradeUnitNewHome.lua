print("This is the modded ChooseTradeUnitNewHome from 'UI - Trade Route Enhancements'")

-------------------------------------------------
-- Choose Trade Unit Home City Popup
-------------------------------------------------

include( "IconSupport" );
include( "InstanceManager" );

-- Used for Piano Keys
local ltBlue = {19/255,32/255,46/255,120/255};
local dkBlue = {12/255,22/255,30/255,120/255};

local g_ItemManager = InstanceManager:new( "ItemInstance", "Button", Controls.ItemStack );

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
bpHeight = bpHeight + heightOffset 

local pUnitPlot;

Controls.BottomPanel:SetSizeVal(bpWidth, bpHeight);
Controls.BottomPanel:ReprocessAnchoring();

local g_PopupInfo = nil;
-------------------------------------------------
-------------------------------------------------
function OnPopupMessage(popupInfo)
	  
	local popupType = popupInfo.Type;
	if popupType ~= ButtonPopupTypes.BUTTONPOPUP_CHOOSE_TRADE_UNIT_NEW_HOME then
		return;
	end	

	g_PopupInfo = popupInfo;
		
	g_iUnitIndex = popupInfo.Data1;
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
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function RefreshList()
	g_ItemManager:ResetInstances();
	
	local pPlayer = Players[Game.GetActivePlayer()];
	local pUnit = pPlayer:GetUnitByID(g_iUnitIndex);
	if (pUnit == nil) then
		return;
	end
	
	local pOriginPlot = pUnit:GetPlot();
	local pOriginCity = pOriginPlot:GetPlotCity();
	
	local eDomain = pUnit:GetDomainType();

	CivIconHookup( pPlayer:GetID(), 64, Controls.CivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true );
	
	
	local unitType = pUnit:GetUnitType();
	local unitInfo = GameInfo.Units[unitType];
	
	local portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon(unitType, pPlayer:GetID());
	IconHookup(unitInfo.PortraitIndex, 64, unitInfo.IconAtlas, Controls.TradeUnitIcon);
	
	Controls.StartingCity:LocalizeAndSetText("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_STARTING_CITY", pOriginCity:GetName());
	Controls.UnitInfo:LocalizeAndSetText("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_TRADE_UNIT", pUnit:GetName());
	
	local map = Map;
	
	local potentialNewHomes = pPlayer:GetPotentialTradeUnitNewHomeCity(pUnit);
	for i,v in ipairs(potentialNewHomes) do
		local pTargetCity = nil;
		local iTargetOwner = nil;
		local pLoopPlot = map.GetPlot(v.X, v.Y);
		if(pLoopPlot ~= nil) then
			pTargetCity = pLoopPlot:GetPlotCity();
			iTargetOwner = pTargetCity:GetOwner();
		end
		
		local pTargetPlayer = Players[iTargetOwner];
		
		local itemInstance = g_ItemManager:GetInstance();
		CivIconHookup(pTargetPlayer:GetID(), 45, itemInstance.CivIcon, itemInstance.CivIconBG, itemInstance.CivIconShadow, false, true );
		local sCityDetails = Locale.Lookup("TXT_KEY_CHANGE_TRADE_UNIT_HOME_CITY_ITEM_CITY", pTargetCity:GetName());
		if (pTargetCity.IsHasBuildingClass and pTargetCity:IsHasBuildingClass(GameInfoTypes.BUILDINGCLASS_CARAVANSARY)) then
		  sCityDetails = sCityDetails .. " [ICON_MOVES]";
		end
		if (pTargetCity.IsHasBuildingClass and pTargetCity:IsHasBuildingClass(GameInfoTypes.BUILDINGCLASS_HARBOR)) then
		  sCityDetails = sCityDetails .. " [ICON_TRADE_WHITE]";
		end
		itemInstance.CityName:SetText(sCityDetails);

		local potentialRoutes = pPlayer:GetPotentialInternationalTradeRouteDestinationsFrom(pUnit, pTargetCity)
		local sTT = Locale.Lookup("TXT_KEY_CHANGE_TRADE_UNIT_HOME_CITY_ITEM_CITY_TT", #potentialRoutes, pTargetCity:GetName())
		for _,r in ipairs(potentialRoutes) do
		  local pDestCity = Map.GetPlot(r.X, r.Y):GetPlotCity()
		  if (pDestCity) then
		    sTT = sTT .. "[NEWLINE][ICON_BULLET] " .. pDestCity:GetName()
			local pDestPlayer = Players[pDestCity:GetOwner()]
			if (pDestPlayer:IsMinorCiv()) then
		      sTT = sTT .. " (City State)"

              if (pDestPlayer:IsMinorCivActiveQuestForPlayer(iPlayer, MinorCivQuestTypes.MINOR_CIV_QUEST_TRADE_ROUTE)) then
                sTT = sTT .. " [ICON_INTERNATIONAL_TRADE]"
              end
			else
		      sTT = sTT .. " (" .. pDestPlayer:GetCivilizationDescription() .. ")"
			end
		  end
		end
		itemInstance.CityName:LocalizeAndSetToolTip(sTT);

		itemInstance.Button:RegisterCallback(Mouse.eLClick, function() SelectNewHome(v.X, v.Y); end);
		-- Leave the definition of the instance to the XML!!!
		-- itemInstance.Box:SetColorVal(unpack(ltBlue));
		-- local buttonWidth, buttonHeight = itemInstance.Button:GetSizeVal();
		-- local newHeight = 80;	
		-- itemInstance.Button:SetSizeVal(buttonWidth, newHeight);
		-- itemInstance.Box:SetSizeVal(buttonWidth, newHeight);
		-- itemInstance.BounceAnim:SetSizeVal(buttonWidth, newHeight + 5);
		-- itemInstance.BounceGrid:SetSizeVal(buttonWidth, newHeight + 5);
		
		itemInstance.GoToCity:RegisterCallback(Mouse.eLClick, function() 
			UI.LookAt(pLoopPlot, 0);  
		end);
	end

	Controls.ItemStack:CalculateSize();
	Controls.ItemStack:ReprocessAnchoring();
	Controls.ItemScrollPanel:CalculateInternalSize();
end

function SelectNewHome(iPlotX, iPlotY) 

	g_selectedPlotX = iPlotX;
	g_selectedPlotY = iPlotY;
	
	Controls.ChooseConfirm:SetHide(false);
end

function OnConfirmYes( )
	Controls.ChooseConfirm:SetHide(true);
	
	Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_CHANGE_TRADE_UNIT_HOME_CITY, g_selectedPlotX, g_selectedPlotY, 0, false, bShift);
		
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
