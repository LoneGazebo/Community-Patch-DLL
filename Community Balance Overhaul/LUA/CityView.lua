print("This is the modded CityView from CBP")

-------------------------------------------------
-- Game View 
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
include( "SupportFunctions"  );
include( "TutorialPopupScreen" );
include( "InfoTooltipInclude" );


local g_BuildingIM   = InstanceManager:new( "BuildingInstance", "BuildingButton", Controls.BuildingStack );
local g_GPIM   = InstanceManager:new( "GPInstance", "GPBox", Controls.GPStack );
local g_SlackerIM   = InstanceManager:new( "SlackerInstance", "SlackerButton", Controls.BoxOSlackers );
local g_PlotButtonIM   = InstanceManager:new( "PlotButtonInstance", "PlotButtonAnchor", Controls.PlotButtonContainer );
local g_BuyPlotButtonIM   = InstanceManager:new( "BuyPlotButtonInstance", "BuyPlotButtonAnchor", Controls.PlotButtonContainer );

local WorldPositionOffset = { x = 0, y = 0, z = 30 };

local WorldPositionOffset2 = { x = 0, y = 35, z = 0 };

local g_iPortraitSize = Controls.ProductionPortrait:GetSize().x;

local screenSizeX, screenSizeY = UIManager:GetScreenSizeVal();

local pediaSearchStrings = {};

local gPreviousCity = nil;
local specialistTable = {};

local g_iBuildingToSell = -1;

local g_bRazeButtonDisabled = false;

-- Add any interface modes that need special processing to this table
local InterfaceModeMessageHandler = 
{
	[InterfaceModeTypes.INTERFACEMODE_SELECTION] = {},
	--[InterfaceModeTypes.INTERFACEMODE_CITY_PLOT_SELECTION] = {},
	[InterfaceModeTypes.INTERFACEMODE_PURCHASE_PLOT] = {}
}
-------------------------------------------------
-- Clear out the UI so that when a player changes
-- the next update doesn't show the previous player's
-- values for a frame
-------------------------------------------------
function ClearCityUIInfo()

	Controls.b1number:SetHide( true );
	Controls.b1down:SetHide( true );
	Controls.b1remove:SetHide( true );
	Controls.b2box:SetHide( true );
	Controls.b3box:SetHide( true );
	Controls.b4box:SetHide( true );
	Controls.b5box:SetHide( true );
	Controls.b6box:SetHide( true );

	Controls.ProductionItemName:SetText("");	
	Controls.ProductionPortraitButton:SetHide(true);		
	Controls.ProductionHelp:SetHide(true);

end

-----------------------------------------------------------------
-- CITY SCREEN CLOSED
-----------------------------------------------------------------
function CityScreenClosed()
	
	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
	OnCityViewUpdate();
	-- We may get here after a player change, clear the UI if this is not the active player's city
	local pCity = UI.GetHeadSelectedCity();
	if pCity ~= nil then
		if pCity:GetOwner() ~= Game.GetActivePlayer() then
			ClearCityUIInfo();
		end
	end
	UI.ClearSelectedCities();
	
	LuaEvents.TryDismissTutorial("CITY_SCREEN");
	
	g_iCurrentSpecialist = -1;
	if (not Controls.SellBuildingConfirm:IsHidden()) then 
		Controls.SellBuildingConfirm:SetHide(true);
	end
	g_iBuildingToSell = -1;
		
	-- Try and re-select the last unit selected		
	if (UI.GetHeadSelectedUnit() == nil and UI.GetLastSelectedUnit() ~= nil) then
		UI.SelectUnit(UI.GetLastSelectedUnit());
		UI.LookAtSelectionPlot();		
	end
	
	UI.SetCityScreenViewingMode(false);
end
Events.SerialEventExitCityScreen.Add(CityScreenClosed);

local DefaultMessageHandler = {};

DefaultMessageHandler[KeyEvents.KeyDown] =
function( wParam, lParam )
	
	local interfaceMode = UI.GetInterfaceMode();
	if (--	interfaceMode == InterfaceModeTypes.INTERFACEMODE_CITY_PLOT_SELECTION or
		interfaceMode == InterfaceModeTypes.INTERFACEMODE_PURCHASE_PLOT) then
		if ( wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN ) then
			UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
			return true;
		end	
	else
		if ( wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN ) then
			if(Controls.SellBuildingConfirm:IsHidden())then
				--CloseScreen();
				Events.SerialEventExitCityScreen();
				return true;
			else
				Controls.SellBuildingConfirm:SetHide(true);
				g_iBuildingToSell = -1;
				return true;
			end
		elseif wParam == Keys.VK_LEFT then
			Game.DoControl(GameInfoTypes.CONTROL_PREVCITY);
			return true;
		elseif wParam == Keys.VK_RIGHT then
			Game.DoControl(GameInfoTypes.CONTROL_NEXTCITY);
			return true;
		end
	end
	
    return false;
end


InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_SELECTION][MouseEvents.LButtonDown] = 
function( wParam, lParam )	
	if GameDefines.CITY_SCREEN_CLICK_WILL_EXIT == 1 then
		UI.ClearSelectedCities();
		return true;
	end

	return false;
end


--InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_PURCHASE_PLOT][MouseEvents.LButtonDown] = 
--function( wParam, lParam )
	--local hexX, hexY = UI.GetMouseOverHex();
	--local plot = Map.GetPlot( hexX, hexY );
	--local plotX = plot:GetX();
	--local plotY = plot:GetY();
	--local bShift = UIManager:GetShift();
	--local bAlt = UIManager:GetAlt();
	--local bCtrl = UIManager:GetControl();
	--local activePlayerID = Game.GetActivePlayer();
	--local pHeadSelectedCity = UI.GetHeadSelectedCity();
	--if pHeadSelectedCity then
		--if (plot:GetOwner() ~= activePlayerID) then
			--Events.AudioPlay2DSound("AS2D_INTERFACE_BUY_TILE");		
		--end
		--Network.SendCityBuyPlot(pHeadSelectedCity:GetID(), plotX, plotY);
	--end
	--return true;
--end
--
----------------------------------------------------------------        
----------------------------------------------------------------        
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_PURCHASE_PLOT][MouseEvents.RButtonUp] = 
function( wParam, lParam )
	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
end


----------------------------------------------------------------        
-- Input handling 
-- (this may be overkill for now because there is currently only 
-- one InterfaceMode on this display, but if we add some, which we did...)
----------------------------------------------------------------        
function InputHandler( uiMsg, wParam, lParam )
	local interfaceMode = UI.GetInterfaceMode();
	local currentInterfaceModeHandler = InterfaceModeMessageHandler[interfaceMode];
	if currentInterfaceModeHandler and currentInterfaceModeHandler[uiMsg] then
		return currentInterfaceModeHandler[uiMsg]( wParam, lParam );
	elseif DefaultMessageHandler[uiMsg] then
		return DefaultMessageHandler[uiMsg]( wParam, lParam );
	end
	return false;
end
ContextPtr:SetInputHandler( InputHandler );


local defaultErrorTextureSheet = "ProductionAtlas.dds";
local nullOffset = Vector2( 0, 0 );

local artistTexture = "citizenArtist.dds";
local engineerTexture = "citizenEngineer.dds";
local merchantTexture = "citizenMerchant.dds";
local scientistTexture = "citizenScientist.dds";
local unemployedTexture = "citizenUnemployed.dds";
local workerTexture = "citizenWorker.dds";
--CBP
local writerTexture =  "citizenwriter.dds";
local musicianTexture = "citizenmusician.dds";
--END
local emptySlotString = Locale.ConvertTextKey("TXT_KEY_CITYVIEW_EMPTY_SLOT");

---------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------

local otherSortedList = {};
local sortOrder = 0;

function CVSortFunction( a, b )

    local aVal = otherSortedList[ tostring( a ) ];
    local bVal = otherSortedList[ tostring( b ) ];
    
    if (aVal == nil) or (bVal == nil) then 
		if aVal and (bVal == nil) then
			return false;
		elseif (aVal == nil) and bVal then
			return true;
		else
			return tostring(a) < tostring(b); -- gotta do something deterministic
        end;
    else
        return aVal < bVal;
    end
end

---------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------

local workerHeadingOpen = OptionsManager.IsNoCitizenWarning();
local slackerHeadingOpen = true;
local GPHeadingOpen = true;
local wonderHeadingOpen = true;
local corpsHeadingOpen = true;
local greatWorkHeadingOpen = true;
local specialistBuildingHeadingOpen = true;
local buildingHeadingOpen = true;
local productionQueueOpen = false;

function OnSlackersSelected()
	if Players[Game.GetActivePlayer()]:IsTurnActive() then
		local pCity = UI.GetHeadSelectedCity();
		if pCity ~= nil then
			Network.SendDoTask(pCity:GetID(), TaskTypes.TASK_REMOVE_SLACKER, 0, -1, false, bAlt, bShift, bCtrl);
		end
	end
end

function OnWorkerHeaderSelected()
	workerHeadingOpen = not workerHeadingOpen;
	OnCityViewUpdate();
end

function OnSlackerHeaderSelected()
	slackerHeadingOpen = not slackerHeadingOpen;
	OnCityViewUpdate();
end

function OnGPHeaderSelected()
	GPHeadingOpen = not GPHeadingOpen;
	OnCityViewUpdate();
end

function OnGreatWorkHeaderSelected()
	greatWorkHeadingOpen = not greatWorkHeadingOpen;
	OnCityViewUpdate();
end

function OnWondersHeaderSelected()
	wonderHeadingOpen = not wonderHeadingOpen;
	OnCityViewUpdate();
end

function OnCorpsHeaderSelected()
	corpsHeadingOpen = not corpsHeadingOpen;
	OnCityViewUpdate();
end

function OnSpecialistBuildingsHeaderSelected()
	specialistBuildingHeadingOpen = not specialistBuildingHeadingOpen;
	OnCityViewUpdate();
end

function OnBuildingsHeaderSelected()
	buildingHeadingOpen = not buildingHeadingOpen;
	OnCityViewUpdate();
end

function GetPedia( void1, void2, button )
	local searchString = pediaSearchStrings[tostring(button)];
	if (searchString ~= nil) then
		Events.SearchForPediaEntry( searchString );
	end
end

-------------------------------------------------
-------------------------------------------------
function OnEditNameClick()
	if UI.GetHeadSelectedCity() then
		local popupInfo = {
				Type = ButtonPopupTypes.BUTTONPOPUP_RENAME_CITY,
				Data1 = UI.GetHeadSelectedCity():GetID(),
				Data2 = -1,
				Data3 = -1,
				Option1 = false,
				Option2 = false;
			}
		Events.SerialEventGameMessagePopup(popupInfo);
	end
end
Controls.EditButton:RegisterCallback( Mouse.eLClick, OnEditNameClick );


function AddBuildingButton( pCity, building )
	local buildingID= building.ID;
	if (pCity:IsHasBuilding(buildingID)) then
		
		local controlTable = g_BuildingIM:GetInstance();
		
		sortOrder = sortOrder + 1;
		otherSortedList[tostring( controlTable.BuildingButton )] = sortOrder;
		
		--controlTable.BuildingButton:RegisterCallback( Mouse.eLClick, OnBuildingClick );
		--controlTable.BuildingButton:SetVoid1( buildingID );

		if (pCity:GetNumFreeBuilding(buildingID) > 0) then
			bIsBuildingFree = true;
		else
			bIsBuildingFree = false;
		end
		
		-- Name
		local strBuildingName;
		
		-- Religious Buildings have special names
		if (building.IsReligious) then
			strBuildingName = Locale.ConvertTextKey("TXT_KEY_RELIGIOUS_BUILDING", building.Description, pPlayer:GetStateReligionKey());
		else
			strBuildingName = Locale.ConvertTextKey(building.Description);
		end

		-- Building is free, add an asterisk to the name
		if (bIsBuildingFree) then
			strBuildingName = strBuildingName .. " (" .. Locale.ConvertTextKey("TXT_KEY_FREE") .. ")";
		end
		
		controlTable.BuildingName:SetText(strBuildingName);

		pediaSearchStrings[tostring(controlTable.BuildingButton)] = Locale.ConvertTextKey(building.Description);
		controlTable.BuildingButton:RegisterCallback( Mouse.eRClick, GetPedia );
				
		-- Portrait
		if IconHookup( building.PortraitIndex, 64, building.IconAtlas, controlTable.BuildingImage ) then
			controlTable.BuildingImage:SetHide( false );
		else
			controlTable.BuildingImage:SetHide( true );
		end
		
		-- Great Work Slots		
		-- Hide all slots.
		local iMaxGreatWorkSlots = 4;
		for i = 1, iMaxGreatWorkSlots, 1 do
			local filledGreatWorkSlot = controlTable["BuildingFilledGreatWorkSlot" .. i];
			filledGreatWorkSlot:SetHide(true);
		end
		
		controlTable.ThemeBonus:SetHide(true);
		
		if(building.GreatWorkSlotType ~= nil and building.GreatWorkCount > 0) then
			local buildingGreatWorkSlotType = building.GreatWorkSlotType;
			local buildingGreatWorkSlot = GameInfo.GreatWorkSlots[buildingGreatWorkSlotType];
			local filledTexture = buildingGreatWorkSlot.FilledIcon;
			local emptyTexture = buildingGreatWorkSlot.EmptyIcon;
								
			local iBuildingClass = GameInfo.BuildingClasses[building.BuildingClass].ID;
			local iNumGreatWorks = building.GreatWorkCount;
			
			local themeBonus = pCity:GetThemingBonus(iBuildingClass);
			if(themeBonus > 0) then
				local themeBonusToolTip = pCity:GetThemingTooltip(iBuildingClass)
				controlTable.ThemeBonus:SetText("+" .. themeBonus);
				controlTable.ThemeBonus:SetToolTipString(themeBonusToolTip);
				controlTable.ThemeBonus:SetHide(false);
			end
			
			for i = 0, iNumGreatWorks - 1, 1 do
				local filledGreatWorkSlot = controlTable["BuildingFilledGreatWorkSlot" .. i + 1];
				
				local iGreatWorkIndex = pCity:GetBuildingGreatWork(iBuildingClass, i);
				if (iGreatWorkIndex >= 0) then
					filledGreatWorkSlot:SetHide(false);
					filledGreatWorkSlot:SetTexture(filledTexture);
					filledGreatWorkSlot:SetToolTipString(Game.GetGreatWorkTooltip(iGreatWorkIndex, pCity:GetOwner()));
					
					local greatWorkType = Game.GetGreatWorkType(iGreatWorkIndex);
					local greatWork = GameInfo.GreatWorks[greatWorkType];
					
					filledGreatWorkSlot:ClearCallback(Mouse.eLClick);
					
					if(greatWork.GreatWorkClassType ~= "GREAT_WORK_ARTIFACT") then
						filledGreatWorkSlot:RegisterCallback(Mouse.eLClick, function() 
							local popupInfo = {
								Type = ButtonPopupTypes.BUTTONPOPUP_GREAT_WORK_COMPLETED_ACTIVE_PLAYER,
								Data1 = iGreatWorkIndex,
								Priority = PopupPriority.Current
							}
							Events.SerialEventGameMessagePopup(popupInfo);
						end);
					end	
				else
					filledGreatWorkSlot:SetHide(false);
					filledGreatWorkSlot:SetTexture(emptyTexture);
					filledGreatWorkSlot:LocalizeAndSetToolTip(buildingGreatWorkSlot.EmptyToolTipText);
					filledGreatWorkSlot:ClearCallback(Mouse.eLClick);
				end
			end	
		end		
		
		-- Empty Specialist Slots
		iNumSpecialists = pCity:GetNumSpecialistsAllowedByBuilding(buildingID)
		
		controlTable.BuildingEmptySpecialistSlot1:SetHide(true);
		controlTable.BuildingEmptySpecialistSlot2:SetHide(true);
		controlTable.BuildingEmptySpecialistSlot3:SetHide(true);
		
		if (iNumSpecialists >= 1) then
			controlTable.BuildingEmptySpecialistSlot1:SetHide(false);
		end
		if (iNumSpecialists >= 2) then
			controlTable.BuildingEmptySpecialistSlot2:SetHide(false);
		end
		if (iNumSpecialists >= 3) then
			controlTable.BuildingEmptySpecialistSlot3:SetHide(false);
		end
		
		-- Filled Specialist Slots
		iNumAssignedSpecialists = pCity:GetNumSpecialistsInBuilding(buildingID)

		if specialistTable[buildingID] == nil then
			specialistTable[buildingID] = { false, false, false };
			if (iNumAssignedSpecialists >= 1) then
				specialistTable[buildingID][1] = true;
			end
			if (iNumAssignedSpecialists >= 2) then
				specialistTable[buildingID][2] = true;
			end
			if (iNumAssignedSpecialists >= 3) then
				specialistTable[buildingID][3] = true;
			end
		else
			local numSlotsIThinkAreFilled = 0;
			for i = 1, 3 do
				if specialistTable[buildingID][i] then
					numSlotsIThinkAreFilled = numSlotsIThinkAreFilled + 1;
				end
			end
			if numSlotsIThinkAreFilled ~= iNumAssignedSpecialists then
				specialistTable[buildingID] = { false, false, false };
				if (iNumAssignedSpecialists >= 1) then
					specialistTable[buildingID][1] = true;
				end
				if (iNumAssignedSpecialists >= 2) then
					specialistTable[buildingID][2] = true;
				end
				if (iNumAssignedSpecialists >= 3) then
					specialistTable[buildingID][3] = true;
				end
			end
		end
		
		controlTable.BuildingFilledSpecialistSlot1:SetHide(true);
		controlTable.BuildingFilledSpecialistSlot2:SetHide(true);
		controlTable.BuildingFilledSpecialistSlot3:SetHide(true);
		
		if (specialistTable[buildingID][1]) then
			controlTable.BuildingEmptySpecialistSlot1:SetHide(true);
			controlTable.BuildingFilledSpecialistSlot1:SetHide(false);
		end
		if (specialistTable[buildingID][2]) then
			controlTable.BuildingEmptySpecialistSlot2:SetHide(true);
			controlTable.BuildingFilledSpecialistSlot2:SetHide(false);
		end
		if (specialistTable[buildingID][3]) then
			controlTable.BuildingEmptySpecialistSlot3:SetHide(true);
			controlTable.BuildingFilledSpecialistSlot3:SetHide(false);
		end
		
		local specialistName = nil;
		if building.SpecialistType then
			local iSpecialistID = GameInfoTypes[building.SpecialistType];
			local pSpecialistInfo = GameInfo.Specialists[iSpecialistID];
			specialistName = Locale.ConvertTextKey(pSpecialistInfo.Description);
			local ToolTipString = specialistName .. " ";
					
			-- Culture
			local iCultureFromSpecialist = pCity:GetCultureFromSpecialist(iSpecialistID);
			if (iCultureFromSpecialist > 0) then
				--CBP
				local extraYield = (pCity:GetSpecialistYield( iSpecialistID, YieldTypes.YIELD_CULTURE ) + pCity:GetSpecialistYieldChange(iSpecialistID, YieldTypes.YIELD_CULTURE))
				iCultureFromSpecialist = (iCultureFromSpecialist + extraYield)
				-- END
				ToolTipString = ToolTipString .. " +" .. iCultureFromSpecialist .. "[ICON_CULTURE]";
			end
			-- Yield
			for pYieldInfo in GameInfo.Yields() do
				local iYieldID = pYieldInfo.ID;
				if(iYieldID ~= YieldTypes.YIELD_CULTURE) then
					local iYieldAmount = pCity:GetSpecialistYield(iSpecialistID, iYieldID);
-- CBP
					if(iYieldAmount > 0) then
						local iExtraYield = pCity:GetSpecialistYieldChange(iSpecialistID, iYieldID);
						iYieldAmount = (iYieldAmount + iExtraYield);
					end
-- End
				
					--Specialist Yield included in pCity:GetSpecialistYield();
					--iYieldAmount = iYieldAmount + Players[pCity:GetOwner()]:GetSpecialistExtraYield(iSpecialistID, iYieldID);
				
					if (iYieldAmount > 0) then
						ToolTipString = ToolTipString .. " +" .. iYieldAmount .. pYieldInfo.IconString;
					end
				end
			end
			if pSpecialistInfo.GreatPeopleRateChange > 0 then
				ToolTipString = ToolTipString .. " +" .. pSpecialistInfo.GreatPeopleRateChange .. "[ICON_GREAT_PEOPLE]";					
			end
			controlTable.BuildingFilledSpecialistSlot1:SetToolTipString(ToolTipString);
			controlTable.BuildingFilledSpecialistSlot2:SetToolTipString(ToolTipString);
			controlTable.BuildingFilledSpecialistSlot3:SetToolTipString(ToolTipString);
			ToolTipString = emptySlotString.."[NEWLINE]("..ToolTipString..")";
			controlTable.BuildingEmptySpecialistSlot1:SetToolTipString(ToolTipString);
			controlTable.BuildingEmptySpecialistSlot2:SetToolTipString(ToolTipString);
			controlTable.BuildingEmptySpecialistSlot3:SetToolTipString(ToolTipString);

			if building.SpecialistType == "SPECIALIST_SCIENTIST" then
				controlTable.BuildingFilledSpecialistSlot1:SetTexture(scientistTexture);
				controlTable.BuildingFilledSpecialistSlot2:SetTexture(scientistTexture);
				controlTable.BuildingFilledSpecialistSlot3:SetTexture(scientistTexture);
			elseif building.SpecialistType == "SPECIALIST_MERCHANT" then
				controlTable.BuildingFilledSpecialistSlot1:SetTexture(merchantTexture);
				controlTable.BuildingFilledSpecialistSlot2:SetTexture(merchantTexture);
				controlTable.BuildingFilledSpecialistSlot3:SetTexture(merchantTexture);
			elseif building.SpecialistType == "SPECIALIST_ARTIST" then
				controlTable.BuildingFilledSpecialistSlot1:SetTexture(artistTexture);
				controlTable.BuildingFilledSpecialistSlot2:SetTexture(artistTexture);
				controlTable.BuildingFilledSpecialistSlot3:SetTexture(artistTexture);
			elseif building.SpecialistType == "SPECIALIST_MUSICIAN" then
				controlTable.BuildingFilledSpecialistSlot1:SetTexture(musicianTexture);
				controlTable.BuildingFilledSpecialistSlot2:SetTexture(musicianTexture);
				controlTable.BuildingFilledSpecialistSlot3:SetTexture(musicianTexture);
			elseif building.SpecialistType == "SPECIALIST_WRITER" then
				controlTable.BuildingFilledSpecialistSlot1:SetTexture(writerTexture);
				controlTable.BuildingFilledSpecialistSlot2:SetTexture(writerTexture);
				controlTable.BuildingFilledSpecialistSlot3:SetTexture(writerTexture);
			elseif building.SpecialistType == "SPECIALIST_ENGINEER" then
				controlTable.BuildingFilledSpecialistSlot1:SetTexture(engineerTexture);
				controlTable.BuildingFilledSpecialistSlot2:SetTexture(engineerTexture);
				controlTable.BuildingFilledSpecialistSlot3:SetTexture(engineerTexture);
			else
				controlTable.BuildingFilledSpecialistSlot1:SetTexture(workerTexture);
				controlTable.BuildingFilledSpecialistSlot2:SetTexture(workerTexture);
				controlTable.BuildingFilledSpecialistSlot3:SetTexture(workerTexture);
			end
		end

		if (UI.IsCityScreenViewingMode()) then
			controlTable.BuildingFilledSpecialistSlot1:RegisterCallback( Mouse.eLClick, DisableSpecialist );
			controlTable.BuildingFilledSpecialistSlot2:RegisterCallback( Mouse.eLClick, DisableSpecialist );
			controlTable.BuildingFilledSpecialistSlot3:RegisterCallback( Mouse.eLClick, DisableSpecialist );
		else
			controlTable.BuildingFilledSpecialistSlot1:RegisterCallback( Mouse.eLClick, RemoveSpecialist );
			controlTable.BuildingFilledSpecialistSlot2:RegisterCallback( Mouse.eLClick, RemoveSpecialist );
			controlTable.BuildingFilledSpecialistSlot3:RegisterCallback( Mouse.eLClick, RemoveSpecialist );
		end

		if (specialistName ~= nil) then
			pediaSearchStrings[tostring(controlTable.BuildingFilledSpecialistSlot1)] = specialistName;
			controlTable.BuildingFilledSpecialistSlot1:RegisterCallback( Mouse.eRClick, GetPedia );
			pediaSearchStrings[tostring(controlTable.BuildingFilledSpecialistSlot2)] = specialistName;
			controlTable.BuildingFilledSpecialistSlot2:RegisterCallback( Mouse.eRClick, GetPedia );
			pediaSearchStrings[tostring(controlTable.BuildingFilledSpecialistSlot3)] = specialistName;
			controlTable.BuildingFilledSpecialistSlot3:RegisterCallback( Mouse.eRClick, GetPedia );
		end
		
		controlTable.BuildingFilledSpecialistSlot1:SetVoids( buildingID, 1 );
		controlTable.BuildingFilledSpecialistSlot2:SetVoids( buildingID, 2 );
		controlTable.BuildingFilledSpecialistSlot3:SetVoids( buildingID, 3 );

		if (UI.IsCityScreenViewingMode()) then
			controlTable.BuildingEmptySpecialistSlot1:RegisterCallback( Mouse.eLClick, DisableSpecialist );
			controlTable.BuildingEmptySpecialistSlot2:RegisterCallback( Mouse.eLClick, DisableSpecialist );
			controlTable.BuildingEmptySpecialistSlot3:RegisterCallback( Mouse.eLClick, DisableSpecialist );
		else
			controlTable.BuildingEmptySpecialistSlot1:RegisterCallback( Mouse.eLClick, AddSpecialist );
			controlTable.BuildingEmptySpecialistSlot2:RegisterCallback( Mouse.eLClick, AddSpecialist );
			controlTable.BuildingEmptySpecialistSlot3:RegisterCallback( Mouse.eLClick, AddSpecialist );
		end

		controlTable.BuildingEmptySpecialistSlot1:SetVoids( buildingID, 1 );
		controlTable.BuildingEmptySpecialistSlot2:SetVoids( buildingID, 2 );
		controlTable.BuildingEmptySpecialistSlot3:SetVoids( buildingID, 3 );


		-- Tool Tip
		local bExcludeHeader = false;
		local bExcludeName = false;
		local bNoMaintenance = bIsBuildingFree;
		local strToolTip = GetHelpTextForBuilding(buildingID, bExcludeName, bExcludeHeader, bNoMaintenance, pCity);
		--strToolTip = strToolTip .. Locale.ConvertTextKey(building.Help);
		
		--if (not bIsBuildingFree) then
			--local iMaintenance = building.GoldMaintenance;
			--if (iMaintenance ~= 0) then
				--strToolTip = strToolTip .. "[NEWLINE][NEWLINE]" .. tostring(iMaintenance) .. "[ICON_GOLD]" .. Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_MAINTENANCE" );
			--end
		--end
		
		if (iNumAssignedSpecialists > 0) then
			if(building.SpecialistType) then
				local pSpecialistInfo = GameInfo.Specialists[building.SpecialistType];
				local iSpecialistID = pSpecialistInfo.ID;
				
				strToolTip = strToolTip .. "[NEWLINE][NEWLINE]";
					
				local yields = {};
				
				-- Culture
				local iCultureFromSpecialist = 0; 
				
				-- Yield
				for pYieldInfo in GameInfo.Yields() do
					local iYieldID = pYieldInfo.ID;
-- CBP
					if(iYieldID == YieldTypes.YIELD_CULTURE)then
						iCultureFromSpecialist = pCity:GetCultureFromSpecialist(iSpecialistID) + pCity:GetSpecialistYield( iSpecialistID, YieldTypes.YIELD_CULTURE ) + pCity:GetSpecialistYieldChange( iSpecialistID, YieldTypes.YIELD_CULTURE);
						if(iCultureFromSpecialist > 0) then
							table.insert(yields, tostring(iCultureFromSpecialist) .. "[ICON_CULTURE]");
						end
--END
					elseif(iYieldID ~= YieldTypes.YIELD_CULTURE)then
						local iYieldAmount = pCity:GetSpecialistYield(iSpecialistID, iYieldID);
-- CBP
						if(iYieldAmount > 0) then
							local iExtraYield = pCity:GetSpecialistYieldChange(iSpecialistID, iYieldID);
							iYieldAmount = (iYieldAmount + iExtraYield);
						end
-- End
					
						if (iYieldAmount > 0) then
							table.insert(yields, tostring(iYieldAmount) .. pYieldInfo.IconString);
						end
					end
				end
				
				local strYield = table.concat(yields, " ");
				
				local str = Locale.Lookup("TXT_KEY_CITYVIEW_BUILDING_SPECIALIST_YIELD", iNumAssignedSpecialists, pSpecialistInfo.Description, strYield);
				
				strToolTip = strToolTip .. str;
			end				
		end
		
		-- Can we sell this thing?
		if (pCity:IsBuildingSellable(buildingID) and not pCity:IsPuppet()) then
			strToolTip = strToolTip .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey( "TXT_KEY_CLICK_TO_SELL" );
			controlTable.BuildingButton:RegisterCallback( Mouse.eLClick, OnBuildingClicked );
			controlTable.BuildingButton:SetVoid1( buildingID );
		-- We have to clear the data out here or else the instance manager will recycle it in other cities!
		else
			controlTable.BuildingButton:ClearCallback(Mouse.eLClick);
			controlTable.BuildingButton:SetVoid1( -1 );
		end
		
		controlTable.BuildingButton:SetToolTipString(strToolTip);
		
		controlTable.BuildingFilledSpecialistSlot1:SetDisabled( false );
		controlTable.BuildingFilledSpecialistSlot2:SetDisabled( false );
		controlTable.BuildingFilledSpecialistSlot3:SetDisabled( false );
		controlTable.BuildingEmptySpecialistSlot1:SetDisabled( false );
		controlTable.BuildingEmptySpecialistSlot2:SetDisabled( false );
		controlTable.BuildingEmptySpecialistSlot3:SetDisabled( false );

		-- Viewing Mode only
		if (UI.IsCityScreenViewingMode()) then
			controlTable.BuildingButton:SetDisabled( true );
		else
			controlTable.BuildingButton:SetDisabled( false );
		end
	end
end

function UpdateThisQueuedItem(city, queuedItemNumber, queueLength)
	local buttonPrefix = "b"..tostring(queuedItemNumber);
	local queuedOrderType;
	local queuedData1;
	local queuedData2;
	local queuedSave;
	local queuedRush;
	local controlBox = buttonPrefix.."box";
	local controlImage = buttonPrefix.."image";
	local controlName = buttonPrefix.."name";
	local controlTurns = buttonPrefix.."turns";
	local isMaint = false;
	
	local strToolTip = "";
	
	local bGeneratingProduction = false;
	if (city:GetCurrentProductionDifferenceTimes100(false, false) > 0) then
		bGeneratingProduction = true;
	end
	
	Controls[controlTurns]:SetHide( false );
	queuedOrderType, queuedData1, queuedData2, queuedSave, queuedRush = city:GetOrderFromQueue( queuedItemNumber-1 );
    if (queuedOrderType == OrderTypes.ORDER_TRAIN) then
		local thisUnitInfo = GameInfo.Units[queuedData1];
		local portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon(queuedData1, city:GetOwner());
		IconHookup( portraitOffset, 45, portraitAtlas, Controls[controlImage] );
		Controls[controlName]:SetText( Locale.ConvertTextKey( thisUnitInfo.Description ) );
		if (bGeneratingProduction) then
			Controls[controlTurns]:SetText(Locale.ConvertTextKey("TXT_KEY_PRODUCTION_HELP_NUM_TURNS", city:GetUnitProductionTurnsLeft(queuedData1, queuedItemNumber-1) ) );
		else
			Controls[controlTurns]:SetText(Locale.ConvertTextKey("TXT_KEY_PRODUCTION_HELP_INFINITE_TURNS"));
		end
		
		if (thisUnitInfo.Help ~= nil) then
			strToolTip = thisUnitInfo.Help;
		end
    elseif (queuedOrderType == OrderTypes.ORDER_CONSTRUCT) then
		local thisBuildingInfo = GameInfo.Buildings[queuedData1];
		IconHookup( thisBuildingInfo.PortraitIndex, 45, thisBuildingInfo.IconAtlas, Controls[controlImage] );
		Controls[controlName]:SetText( Locale.ConvertTextKey( thisBuildingInfo.Description ) );
		if (bGeneratingProduction) then
			Controls[controlTurns]:SetText(  Locale.ConvertTextKey("TXT_KEY_PRODUCTION_HELP_NUM_TURNS", city:GetBuildingProductionTurnsLeft(queuedData1, queuedItemNumber-1)) );
		else
			Controls[controlTurns]:SetText(Locale.ConvertTextKey("TXT_KEY_PRODUCTION_HELP_INFINITE_TURNS"));
		end
		
		if (thisBuildingInfo.Help ~= nil) then
			strToolTip = thisBuildingInfo.Help;
		end
    elseif (queuedOrderType == OrderTypes.ORDER_CREATE) then
		local thisProjectInfo = GameInfo.Projects[queuedData1];
		IconHookup( thisProjectInfo.PortraitIndex, 45, thisProjectInfo.IconAtlas, Controls[controlImage] );
		Controls[controlName]:SetText( Locale.ConvertTextKey( thisProjectInfo.Description ) );
		if (bGeneratingProduction) then
			Controls[controlTurns]:SetText(  Locale.ConvertTextKey("TXT_KEY_PRODUCTION_HELP_NUM_TURNS",city:GetProjectProductionTurnsLeft(queuedData1, queuedItemNumber-1)) );
		else
			Controls[controlTurns]:SetText(Locale.ConvertTextKey("TXT_KEY_PRODUCTION_HELP_INFINITE_TURNS"));
		end
		
		if (thisProjectInfo.Help ~= nil) then
			strToolTip = thisProjectInfo.Help;
		end
    elseif (queuedOrderType == OrderTypes.ORDER_MAINTAIN) then
		isMaint = true;
 		local thisProcessInfo = GameInfo.Processes[queuedData1];
		IconHookup( thisProcessInfo.PortraitIndex, 45, thisProcessInfo.IconAtlas, Controls[controlImage] );
		Controls[controlName]:SetText( Locale.ConvertTextKey( thisProcessInfo.Description ) );
		Controls[controlTurns]:SetHide( true );
		
		if (thisProcessInfo.Help ~= nil) then
			strToolTip = thisProcessInfo.Help;
		end
   end
   
	Controls[controlBox]:SetToolTipString(Locale.ConvertTextKey(strToolTip));
	return isMaint;
end

-------------------------------------------------
-- City View Update
-------------------------------------------------
function OnCityViewUpdate()
    if( ContextPtr:IsHidden() ) then
        return;
    end
        
	local pCity = UI.GetHeadSelectedCity();
	
	if gPreviousCity ~= pCity then
		gPreviousCity = pCity;
		specialistTable = {};
	end
	
	if (pCity ~= nil) then
	
		pediaSearchStrings = {};
		
		-- Auto Specialist checkbox
		local isNoAutoAssignSpecialists = pCity:IsNoAutoAssignSpecialists();
		Controls.NoAutoSpecialistCheckbox:SetCheck(isNoAutoAssignSpecialists);
		Controls.NoAutoSpecialistCheckbox2:SetCheck(isNoAutoAssignSpecialists);
	
		-- slewis - I'm showing this because when we're in espionage mode we hide this button
		Controls.EditButton:SetHide(false);
		Controls.PurchaseButton:SetDisabled(false);
		Controls.EndTurnText:SetText(Locale.ConvertTextKey("TXT_KEY_CITYVIEW_RETURN_TO_MAP"));
		
		-------------------------------------------
		-- City Banner
		-------------------------------------------
		local pPlayer = Players[pCity:GetOwner()];
		local isActiveTeamCity = true;
		
		-- Update capital icon
		local isCapital = pCity:IsCapital();
		Controls.CityCapitalIcon:SetHide(not isCapital);
		
		-- Connected to capital?
		if (isActiveTeamCity) then
			if (not isCapital and pPlayer:IsCapitalConnectedToCity(pCity) and not pCity:IsBlockaded()) then
				Controls.ConnectedIcon:SetHide(false);
				Controls.ConnectedIcon:LocalizeAndSetToolTip("TXT_KEY_CITY_CONNECTED");
			else
				Controls.ConnectedIcon:SetHide(true);
			end
		end
			
		-- Blockaded
		if (pCity:IsBlockaded()) then
			Controls.BlockadedIcon:SetHide(false);
			Controls.BlockadedIcon:LocalizeAndSetToolTip("TXT_KEY_CITY_BLOCKADED");
		else
			Controls.BlockadedIcon:SetHide(true);
		end
		
		-- Being Razed
		if (pCity:IsRazing()) then
			Controls.RazingIcon:SetHide(false);
			Controls.RazingIcon:LocalizeAndSetToolTip("TXT_KEY_CITY_BURNING", pCity:GetRazingTurns());
		else
			Controls.RazingIcon:SetHide(true);
		end
		
		-- In Resistance
		if (pCity:IsResistance()) then
			Controls.ResistanceIcon:SetHide(false);
			Controls.ResistanceIcon:LocalizeAndSetToolTip("TXT_KEY_CITY_RESISTANCE", pCity:GetResistanceTurns());
		else
			Controls.ResistanceIcon:SetHide(true);
		end

		-- Puppet Status
		if (pCity:IsPuppet()) then
			Controls.PuppetIcon:SetHide(false);
			Controls.PuppetIcon:LocalizeAndSetToolTip("TXT_KEY_CITY_PUPPET");
		else
			Controls.PuppetIcon:SetHide(true);
		end
		
		-- Occupation Status
		if (pCity:IsOccupied() and not pCity:IsNoOccupiedUnhappiness()) then
			Controls.OccupiedIcon:SetHide(false);
			Controls.OccupiedIcon:LocalizeAndSetToolTip("TXT_KEY_CITY_OCCUPIED");
		else
			Controls.OccupiedIcon:SetHide(true);
		end	
		
		local cityName = pCity:GetNameKey();
		local convertedKey = Locale.ConvertTextKey(cityName);
		
		if (pCity:IsRazing()) then
			convertedKey = convertedKey .. " (" .. Locale.ConvertTextKey("TXT_KEY_BURNING") .. ")";
		end
		
		if (pPlayer:GetNumCities() <= 1) then
			Controls.PrevCityButton:SetDisabled( true );
			Controls.NextCityButton:SetDisabled( true );
		else
			Controls.PrevCityButton:SetDisabled( false );
			Controls.NextCityButton:SetDisabled( false );
		end
		
		OnCitySetDamage(pCity:GetDamage(), pCity:GetMaxHitPoints());
		
		convertedKey = Locale.ToUpper(convertedKey);

		local cityNameSize = (math.abs(Controls.NextCityButton:GetOffsetX()) * 2) - (Controls.PrevCityButton:GetSizeX()); 
			         
		if(isCapital)then
			cityNameSize = cityNameSize - Controls.CityCapitalIcon:GetSizeX();
		end
		TruncateString(Controls.CityNameTitleBarLabel, cityNameSize, convertedKey); 

-- COMMUNITY PATCH
		Controls.CityNameTitleBarLabel:LocalizeAndSetToolTip(pCity:GetCityUnhappinessBreakdown(false));
-- END
		
		Controls.TitleStack:CalculateSize();
		Controls.TitleStack:ReprocessAnchoring();

	    Controls.Defense:SetText(  math.floor( pCity:GetStrengthValue() / 100 ) );
		Controls.Defense:LocalizeAndSetToolTip( Locale.ConvertTextKey("TXT_KEY_CITYVIEW_CITY_COMB_STRENGTH_TT") .. ": [ICON_RANGE_STRENGTH] " .. pCity:GetStrengthValue(true) / 100)

 		CivIconHookup( pPlayer:GetID(), 64, Controls.CivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true );
		
		-------------------------------------------
		-- Growth Meter
		-------------------------------------------
		local iCurrentFood = pCity:GetFood();
		local iFoodNeeded = pCity:GrowthThreshold();
		local iFoodPerTurn = pCity:FoodDifference();
		local iCurrentFoodPlusThisTurn = iCurrentFood + iFoodPerTurn;
		
		local fGrowthProgressPercent = iCurrentFood / iFoodNeeded;
		local fGrowthProgressPlusThisTurnPercent = iCurrentFoodPlusThisTurn / iFoodNeeded;
		if (fGrowthProgressPlusThisTurnPercent > 1) then
			fGrowthProgressPlusThisTurnPercent = 1
		end
		
		local iTurnsToGrowth = pCity:GetFoodTurnsLeft();
		
		local cityPopulation = math.floor(pCity:GetPopulation());
		Controls.CityPopulationLabel:SetText(tostring(cityPopulation));
		Controls.PeopleMeter:SetPercent( pCity:GetFood() / pCity:GrowthThreshold() );
		
		--Update suffix to use correct plurality.
		Controls.CityPopulationLabelSuffix:LocalizeAndSetText("TXT_KEY_CITYVIEW_CITIZENS_TEXT", cityPopulation);

		-------------------------------------------
		-- Deal with the production queue buttons
		-------------------------------------------
		local qLength = pCity:GetOrderQueueLength();
		if qLength > 0 then
			Controls.HideQueueButton:SetHide( false );
		else
			Controls.HideQueueButton:SetHide( true );
		end
		
		-- hide the queue buttons
		Controls.b1number:SetHide( true );
		Controls.b1down:SetHide( true );
		Controls.b1remove:SetHide( true );
		Controls.b2box:SetHide( true );
		Controls.b3box:SetHide( true );
		Controls.b4box:SetHide( true );
		Controls.b5box:SetHide( true );
		Controls.b6box:SetHide( true );
		
		local anyMaint = false;
		
		Controls.ProductionPortraitButton:SetHide( false );
		
		local panelSize = Controls.ProdQueueBackground:GetSize();
		if productionQueueOpen and qLength > 0 then
			panelSize.y = 470;
			Controls.ProductionButtonLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_QUEUE_PROD") );
			Controls.ProductionButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_QUEUE_PROD_TT") );
			
			-- show the queue buttons
			Controls.b1number:SetHide( false );
			Controls.b1remove:SetHide( false );
			if qLength > 1 then
				Controls.b1down:SetHide( false );
			end
			for i = 2, qLength, 1 do
				local isMaint = UpdateThisQueuedItem(pCity, i, qLength);
				local buttonName = "b"..tostring(i).."box";
				Controls[buttonName]:SetHide( false );
				--update the down buttons
				local buttonDown = "b"..tostring(i).."down";
				if qLength == i then
					Controls[buttonDown]:SetHide( true );
				else
					Controls[buttonDown]:SetHide( false );
				end
				local buttonUp = "b"..tostring(i).."up";
				if isMaint then
					anyMaint = true;
					Controls[buttonUp]:SetHide( true );
					buttonDown = "b"..tostring(i-1).."down";
					Controls[buttonDown]:SetHide( true );
				else
					Controls[buttonUp]:SetHide( false );
				end				
			end
		else
			if qLength == 0 then
				Controls.ProductionButtonLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_CHOOSE_PROD") );
				Controls.ProductionButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_CHOOSE_PROD_TT") );
			else
				Controls.ProductionButtonLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_CHANGE_PROD") );
				Controls.ProductionButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_CHANGE_PROD_TT") );
			end
			panelSize.y = 280;
		end
		Controls.ProdQueueBackground:SetSize(panelSize);
		if productionQueueOpen and (qLength >= 6 or anyMaint == true) then
			Controls.ProductionButton:SetDisabled( true );
		else
			Controls.ProductionButton:SetDisabled( false );
		end
		if qLength == 1 then
			Controls.b1remove:SetHide( true );
		end

		
		-------------------------------------------
		-- Item under Production
		-------------------------------------------
		local szItemName = Locale.ConvertTextKey(pCity:GetProductionNameKey());
		szItemName = Locale.ToUpper(szItemName);
		Controls.ProductionItemName:SetText(szItemName);
		
		-------------------------------------------
		-- Description and picture of Item under Production
		-------------------------------------------
		local szHelpText = "";
		local unitProduction = pCity:GetProductionUnit();
		local buildingProduction = pCity:GetProductionBuilding();
		local projectProduction = pCity:GetProductionProject();
		local processProduction = pCity:GetProductionProcess();
		local noProduction = false;

		if unitProduction ~= -1 then
			local thisUnitInfo = GameInfo.Units[unitProduction];
			local portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon(unitProduction, pCity:GetOwner());			
			szHelpText = Locale.ConvertTextKey(thisUnitInfo.Help);
			if IconHookup( portraitOffset, g_iPortraitSize, portraitAtlas, Controls.ProductionPortrait ) then
				Controls.ProductionPortrait:SetHide( false );
			else
				Controls.ProductionPortrait:SetHide( true );
			end
		elseif buildingProduction ~= -1 then
			local thisBuildingInfo = GameInfo.Buildings[buildingProduction];
			
			local bExcludeName = true;
			local bExcludeHeader = false;
			szHelpText = GetHelpTextForBuilding(buildingProduction, bExcludeName, bExcludeHeader, false, pCity);
			--szHelpText = thisBuildingInfo.Help;
			
			if IconHookup( thisBuildingInfo.PortraitIndex, g_iPortraitSize, thisBuildingInfo.IconAtlas, Controls.ProductionPortrait ) then
				Controls.ProductionPortrait:SetHide( false );
			else
				Controls.ProductionPortrait:SetHide( true );
			end
		elseif projectProduction ~= -1 then
			local thisProjectInfo = GameInfo.Projects[projectProduction];
			szHelpText = thisProjectInfo.Help;
			if IconHookup( thisProjectInfo.PortraitIndex, g_iPortraitSize, thisProjectInfo.IconAtlas, Controls.ProductionPortrait ) then
				Controls.ProductionPortrait:SetHide( false );
			else
				Controls.ProductionPortrait:SetHide( true );
			end
		elseif processProduction ~= -1 then
			local thisProcessInfo = GameInfo.Processes[processProduction];
			szHelpText = thisProcessInfo.Help;
			if IconHookup( thisProcessInfo.PortraitIndex, g_iPortraitSize, thisProcessInfo.IconAtlas, Controls.ProductionPortrait ) then
				Controls.ProductionPortrait:SetHide( false );
			else
				Controls.ProductionPortrait:SetHide( true );
			end
		else
			Controls.ProductionPortrait:SetHide(true);
			noProduction = true;
		end
		
		if szHelpText ~= nil and szHelpText ~= "" then
			Controls.ProductionHelp:SetText(Locale.ConvertTextKey(szHelpText));
			Controls.ProductionHelp:SetHide(false);
			Controls.ProductionHelpScroll:CalculateInternalSize();
		else
			Controls.ProductionHelp:SetHide(true);
		end

		-------------------------------------------
		-- Production
		-------------------------------------------
		
		DoUpdateProductionInfo( noProduction );
		
		-------------------------------------------
		-- Buildings (etc.) List
		-------------------------------------------		
		g_BuildingIM:ResetInstances();
		g_GPIM:ResetInstances();
		g_SlackerIM:ResetInstances();
		g_PlotButtonIM:ResetInstances();
		g_BuyPlotButtonIM:ResetInstances();
		
		local controlTable;
		local bIsFreeBuilding;		
		local iNumSpecialists;

		local slackerType = GameDefines.DEFAULT_SPECIALIST;
		local numSlackersInThisCity = pCity:GetSpecialistCount( slackerType );
		
		-- header
		if workerHeadingOpen then
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_CITIZEN_ALLOCATION" );
			Controls.WorkerHeaderLabel:SetText(localizedLabel);
			local focusType = pCity:GetFocusType();
			if focusType == CityAIFocusTypes.NO_CITY_AI_FOCUS_TYPE then
				Controls.BalancedFocusButton:SetCheck( true );
			elseif focusType == CityAIFocusTypes.CITY_AI_FOCUS_TYPE_FOOD then
				Controls.FoodFocusButton:SetCheck( true );
			elseif focusType == CityAIFocusTypes.CITY_AI_FOCUS_TYPE_PRODUCTION then
				Controls.ProductionFocusButton:SetCheck( true );
			elseif focusType == CityAIFocusTypes.CITY_AI_FOCUS_TYPE_GOLD then
				Controls.GoldFocusButton:SetCheck( true );
			elseif focusType == CityAIFocusTypes.CITY_AI_FOCUS_TYPE_SCIENCE then
				Controls.ResearchFocusButton:SetCheck( true );
			elseif focusType == CityAIFocusTypes.CITY_AI_FOCUS_TYPE_CULTURE then
				Controls.CultureFocusButton:SetCheck( true );
			elseif focusType == CityAIFocusTypes.CITY_AI_FOCUS_TYPE_GREAT_PEOPLE then
				Controls.GPFocusButton:SetCheck( true );
			elseif focusType == CityAIFocusTypes.CITY_AI_FOCUS_TYPE_FAITH then
				Controls.FaithFocusButton:SetCheck( true );
			else
				Controls.BalancedFocusButton:SetCheck( true );
			end
			Controls.AvoidGrowthButton:SetCheck( pCity:IsForcedAvoidGrowth() );
			if pCity:GetNumForcedWorkingPlots() > 0 or numSlackersInThisCity > 0 then
				Controls.ResetButton:SetHide( false );
				Controls.ResetFooter:SetHide( false );
			else
				Controls.ResetButton:SetHide( true );
				Controls.ResetFooter:SetHide( true );
			end
			Events.RequestYieldDisplay( YieldDisplayTypes.CITY_OWNED, pCity:GetX(), pCity:GetY() );
			Controls.WorkerManagementBox:SetHide( false );
		else
			local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_CITIZEN_ALLOCATION" );
			Controls.WorkerHeaderLabel:SetText(localizedLabel);
			Events.RequestYieldDisplay( YieldDisplayTypes.CITY_WORKED, pCity:GetX(), pCity:GetY() );
			Controls.WorkerManagementBox:SetHide( true );
		end
		Controls.WorkerHeader:RegisterCallback( Mouse.eLClick, OnWorkerHeaderSelected );
		
		-- add in the Great Person Meters
		local numGPs = 0;		
		for pSpecialistInfo in GameInfo.Specialists() do
			local iSpecialistIndex = pSpecialistInfo.ID;			
			local iProgress = pCity:GetSpecialistGreatPersonProgress(iSpecialistIndex);
			if (iProgress > 0) then		
				numGPs = numGPs + 1;		
			
				local unitClass = GameInfo.UnitClasses[pSpecialistInfo.GreatPeopleUnitClass];
				if(unitClass ~= nil) then

					local threshold = pCity:GetSpecialistUpgradeThreshold(unitClass.ID);			
					controlTable = g_GPIM:GetInstance();
					local percent = iProgress / threshold;
					controlTable.GPMeter:SetPercent( percent );

					local gp = GameInfo.Units[ unitClass.DefaultUnit ];
					local portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon(gp.ID, pCity:GetOwner());
					local labelText = Locale.ConvertTextKey(unitClass.Description);
					controlTable.GreatPersonLabel:SetText(labelText);
					pediaSearchStrings[tostring(controlTable.GPImage)] = labelText;
					controlTable.GPImage:RegisterCallback( Mouse.eRClick, GetPedia );
					
					local strToolTipText = Locale.ConvertTextKey("TXT_KEY_PROGRESS_TOWARDS",labelText);
					strToolTipText = strToolTipText .. ": " .. tostring(iProgress) .. "/" .. tostring(threshold);					
					local iCount = pCity:GetSpecialistCount( pSpecialistInfo.ID );
					local iGPPChange = pSpecialistInfo.GreatPeopleRateChange * iCount * 100;
					for building in GameInfo.Buildings{SpecialistType = pSpecialistInfo.Type} do
				        local buildingID = building.ID;
						if (pCity:IsHasBuilding(buildingID)) then
							iGPPChange = iGPPChange + building.GreatPeopleRateChange * 100;
						end
					end
					iGPPChange = iGPPChange + pCity:GetExtraSpecialistPoints(pSpecialistInfo.ID);
					iGPPChange = iGPPChange + pPlayer:GetMonopolyGreatPersonRateChange(pSpecialistInfo.ID);

					if iGPPChange > 0 then
						-- Generic GP mods
						local iPlayerMod = pPlayer:GetGreatPeopleRateModifier();
						local iPolicyMod = pPlayer:GetPolicyGreatPeopleRateModifier();
						local iWorldCongressMod = 0;
						local pWorldCongress = nil;
						if (Game.GetNumActiveLeagues() > 0) then
							pWorldCongress = Game.GetActiveLeague();
						end
						local iCityMod = pCity:GetGreatPeopleRateModifier();
						-- CBP
						iCityMod = iCityMod + pCity:GetSpecialistCityModifier(pSpecialistInfo.ID);
						local iMonopolyMod = pPlayer:GetMonopolyGreatPersonRateModifier(pSpecialistInfo.ID);
						--END
						local iGoldenAgeMod = 0;
						local bGoldenAge = (pPlayer:GetGoldenAgeTurns() > 0);
						
						-- GP mods by type		
						if (pSpecialistInfo.GreatPeopleUnitClass == "UNITCLASS_WRITER") then
							iPlayerMod = iPlayerMod + pPlayer:GetGreatWriterRateModifier();
							iPolicyMod = iPolicyMod + pPlayer:GetPolicyGreatWriterRateModifier();
							if (pWorldCongress ~= nil and pWorldCongress:GetArtsyGreatPersonRateModifier() ~= 0) then
								iWorldCongressMod = iWorldCongressMod + pWorldCongress:GetArtsyGreatPersonRateModifier();
							end
							if (bGoldenAge and pPlayer:GetGoldenAgeGreatWriterRateModifier() > 0) then
								iGoldenAgeMod = iGoldenAgeMod + pPlayer:GetGoldenAgeGreatWriterRateModifier();
							end
						elseif (pSpecialistInfo.GreatPeopleUnitClass == "UNITCLASS_ARTIST") then
							iPlayerMod = iPlayerMod + pPlayer:GetGreatArtistRateModifier();
							iPolicyMod = iPolicyMod + pPlayer:GetPolicyGreatArtistRateModifier();
							if (pWorldCongress ~= nil and pWorldCongress:GetArtsyGreatPersonRateModifier() ~= 0) then
								iWorldCongressMod = iWorldCongressMod + pWorldCongress:GetArtsyGreatPersonRateModifier();
							end
							if (bGoldenAge and pPlayer:GetGoldenAgeGreatArtistRateModifier() > 0) then
								iGoldenAgeMod = iGoldenAgeMod + pPlayer:GetGoldenAgeGreatArtistRateModifier();
							end
						elseif (pSpecialistInfo.GreatPeopleUnitClass == "UNITCLASS_MUSICIAN") then
							iPlayerMod = iPlayerMod + pPlayer:GetGreatMusicianRateModifier();
							iPolicyMod = iPolicyMod + pPlayer:GetPolicyGreatMusicianRateModifier();
							if (pWorldCongress ~= nil and pWorldCongress:GetArtsyGreatPersonRateModifier() ~= 0) then
								iWorldCongressMod = iWorldCongressMod + pWorldCongress:GetArtsyGreatPersonRateModifier();
							end
							if (bGoldenAge and pPlayer:GetGoldenAgeGreatMusicianRateModifier() > 0) then
								iGoldenAgeMod = iGoldenAgeMod + pPlayer:GetGoldenAgeGreatMusicianRateModifier();
							end
						elseif (pSpecialistInfo.GreatPeopleUnitClass == "UNITCLASS_SCIENTIST") then
							iPlayerMod = iPlayerMod + pPlayer:GetGreatScientistRateModifier();
							iPolicyMod = iPolicyMod + pPlayer:GetPolicyGreatScientistRateModifier();
							if (pWorldCongress ~= nil and pWorldCongress:GetScienceyGreatPersonRateModifier() ~= 0) then
								iWorldCongressMod = iWorldCongressMod + pWorldCongress:GetScienceyGreatPersonRateModifier();
							end
							--CBP
							if (bGoldenAge and pPlayer:GetGoldenAgeGreatScientistRateModifier() > 0) then
								iGoldenAgeMod = iGoldenAgeMod + pPlayer:GetGoldenAgeGreatScientistRateModifier();
							end
							--END
						elseif (pSpecialistInfo.GreatPeopleUnitClass == "UNITCLASS_MERCHANT") then
							iPlayerMod = iPlayerMod + pPlayer:GetGreatMerchantRateModifier();
							iPolicyMod = iPolicyMod + pPlayer:GetPolicyGreatMerchantRateModifier();
							if (pWorldCongress ~= nil and pWorldCongress:GetScienceyGreatPersonRateModifier() ~= 0) then
								iWorldCongressMod = iWorldCongressMod + pWorldCongress:GetScienceyGreatPersonRateModifier();
							end
							--CBP
							if (bGoldenAge and pPlayer:GetGoldenAgeGreatMerchantRateModifier() > 0) then
								iGoldenAgeMod = iGoldenAgeMod + pPlayer:GetGoldenAgeGreatMerchantRateModifier();
							end
							--END
						elseif (pSpecialistInfo.GreatPeopleUnitClass == "UNITCLASS_ENGINEER") then
							iPlayerMod = iPlayerMod + pPlayer:GetGreatEngineerRateModifier();
							iPolicyMod = iPolicyMod + pPlayer:GetPolicyGreatEngineerRateModifier();
							if (pWorldCongress ~= nil and pWorldCongress:GetScienceyGreatPersonRateModifier() ~= 0) then
								iWorldCongressMod = iWorldCongressMod + pWorldCongress:GetScienceyGreatPersonRateModifier();
							end
							--CBP
							if (bGoldenAge and pPlayer:GetGoldenAgeGreatEngineerRateModifier() > 0) then
								iGoldenAgeMod = iGoldenAgeMod + pPlayer:GetGoldenAgeGreatEngineerRateModifier();
							end
							--END
						end
						
						-- Player mod actually includes policy mod and World Congress mod, so separate them for tooltip
						iPlayerMod = iPlayerMod - iPolicyMod - iWorldCongressMod;
						
						local iMod = iPlayerMod + iPolicyMod + iWorldCongressMod + iCityMod + iGoldenAgeMod + iMonopolyMod;
						iGPPChange = (iGPPChange * (100 + iMod)) / 100;
-- Vox Populi
						local iProgress100 = pCity:GetSpecialistGreatPersonProgressTimes100(iSpecialistIndex);
						local iTurns = math.ceil( (threshold * 100 - iProgress100) / iGPPChange );
						--strToolTipText = strToolTipText .. " (+" .. math.floor(iGPPChange/100) .. "[ICON_GREAT_PEOPLE])";	
						strToolTipText = strToolTipText .. string.format(" (%+4.1f[ICON_GREAT_PEOPLE], ",iGPPChange/100) .. iTurns .. "[ICON_TURNS_REMAINING])";
-- Vox Populi end
						if (iPlayerMod > 0) then
							strToolTipText = strToolTipText .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_PLAYER_GP_MOD", iPlayerMod);
						end
						if (iMonopolyMod > 0) then
							strToolTipText = strToolTipText .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_MONOPOLY_GP_MOD", iMonopolyMod);
						end
						if (iPolicyMod > 0) then
							strToolTipText = strToolTipText .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_POLICY_GP_MOD", iPolicyMod);
						end
						if (iCityMod > 0) then
							strToolTipText = strToolTipText .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_CITY_GP_MOD", iCityMod);
						end
						if (iGoldenAgeMod > 0) then
							strToolTipText = strToolTipText .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_GOLDENAGE_GP_MOD", iGoldenAgeMod);
						end
						if (iWorldCongressMod ~= 0) then
							if (iWorldCongressMod < 0) then
								strToolTipText = strToolTipText .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_WORLD_CONGRESS_NEGATIVE_GP_MOD", iWorldCongressMod);
							else
								strToolTipText = strToolTipText .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_WORLD_CONGRESS_POSITIVE_GP_MOD", iWorldCongressMod);
							end
						end
					end
					controlTable.GPBox:SetToolTipString(strToolTipText);
					
					if IconHookup( portraitOffset, 64, portraitAtlas, controlTable.GPImage ) then
						controlTable.GPImage:SetHide( false );
					end
				end
			end			
		end
		-- header
		if GPHeadingOpen then
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_GREAT_PEOPLE_TEXT" );
			Controls.GPHeaderLabel:SetText(localizedLabel);
			Controls.GPStack:SetHide( false );
		else
			local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_GREAT_PEOPLE_TEXT" );
			Controls.GPHeaderLabel:SetText(localizedLabel);
			Controls.GPStack:SetHide( true );
		end
		if numGPs > 0 then
			Controls.GPHeader:SetHide( false );
		else
			Controls.GPHeader:SetHide( true );
			Controls.GPStack:SetHide( true );
		end
		Controls.GPHeader:RegisterCallback( Mouse.eLClick, OnGPHeaderSelected );

	
		-- add in the slackers
		local numberOfSlackersPerRow = 8;
		local slackerSize = 32;
		local slackerPadding = 2;
		-- header
		if slackerHeadingOpen then
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_UNEMPLOYED_TEXT" );
			Controls.SlackerHeaderLabel:SetText(localizedLabel);
		else
			local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_UNEMPLOYED_TEXT" );
			Controls.SlackerHeaderLabel:SetText(localizedLabel);
		end
		if numSlackersInThisCity > 0 then
			--if header is not hidden and is open
			Controls.SlackerHeader:SetHide( false );
			if slackerHeadingOpen then
				Controls.BoxOSlackers:SetHide( false );
				
				Controls.SlackerHeader:RegisterCallback( Mouse.eLClick, OnSlackerHeaderSelected );
				Controls.BoxOSlackers:RegisterCallback( Mouse.eLClick, OnSlackersSelected );

				-- build the tooltip for slackers
				local pSpecialistInfo = GameInfo.Specialists[slackerType];
				local specialistName = Locale.ConvertTextKey(pSpecialistInfo.Description);
				local ToolTipString = specialistName .. " ";						
				-- Culture
				local iCultureFromSpecialist = pCity:GetCultureFromSpecialist(iSpecialistID);
				if (iCultureFromSpecialist > 0) then
					--CBP
					local extraYield = (pCity:GetSpecialistYield( iSpecialistID, YieldTypes.YIELD_CULTURE ) + pCity:GetSpecialistYieldChange(iSpecialistID, YieldTypes.YIELD_CULTURE))
					iCultureFromSpecialist = (iCultureFromSpecialist + extraYield)
					-- END
					ToolTipString = ToolTipString .. " +" .. iCultureFromSpecialist .. "[ICON_CULTURE]";
				end
				-- Yield
				for pYieldInfo in GameInfo.Yields() do
					local iYieldID = pYieldInfo.ID;
					if(iYieldID ~= YieldTypes.YIELD_CULTURE) then
						local iYieldAmount = pCity:GetSpecialistYield(iSpecialistID, iYieldID);					
						--Specialist Yield included in pCity:GetSpecialistYield();
						--iYieldAmount = iYieldAmount + pPlayer:GetSpecialistExtraYield(iSpecialistID, iYieldID);
					
						if (iYieldAmount > 0) then
							--CBP
							local extraYield = pCity:GetSpecialistYieldChange(iSpecialistID, iYieldID);
							iYieldAmount = (iYieldAmount + extraYield);
							-- END
							ToolTipString = ToolTipString .. " +" .. iYieldAmount .. pYieldInfo.IconString;
						end
					end
				end
				if pSpecialistInfo.GreatPeopleRateChange > 0 then
					ToolTipString = ToolTipString .. " +" .. pSpecialistInfo.GreatPeopleRateChange .. "[ICON_GREAT_PEOPLE]";					
				end

				-- bunch-o-slackers
				local slackerAdded = 0;
				for i = 1, numSlackersInThisCity do
					controlTable = g_SlackerIM:GetInstance();
					controlTable.SlackerButton:SetOffsetVal( (slackerAdded % numberOfSlackersPerRow) * slackerSize + slackerPadding, math.floor(slackerAdded / numberOfSlackersPerRow) * slackerSize + slackerPadding );				
					controlTable.SlackerButton:SetToolTipString( ToolTipString );
					controlTable.SlackerButton:RegisterCallback( Mouse.eLClick, OnSlackersSelected );
					pediaSearchStrings[tostring(controlTable.SlackerButton)] = specialistName;
					controlTable.SlackerButton:RegisterCallback( Mouse.eRClick, GetPedia );
					slackerAdded = slackerAdded + 1;
				end
				if slackerAdded > 0 then
					local frameSize = {};
					local h = (math.floor((slackerAdded - 1) / numberOfSlackersPerRow) + 1) * slackerSize + (slackerPadding * 2);
					frameSize.x = 254;
					frameSize.y = h;
					Controls.BoxOSlackers:SetSize( frameSize );
				end
			else
				Controls.BoxOSlackers:SetHide( true );
			end
		else
			Controls.SlackerHeader:SetHide( true );
			Controls.BoxOSlackers:SetHide( true );
		end
		
		sortOrder = 0;
		otherSortedList = {};
		
		local iBuildingMaintenance = pCity:GetTotalBaseBuildingMaintenance();
		local strMaintenanceTT = Locale.ConvertTextKey("TXT_KEY_BUILDING_MAINTENANCE_TT", iBuildingMaintenance);
		Controls.SpecialBuildingsHeader:SetToolTipString(strMaintenanceTT);

		local freeSpecialists = pCity:GetRemainingFreeSpecialists();
		if(freeSpecialists > 0) then
			print("Free specialists!")
			local freeSpecialtxt = tostring(freeSpecialists);
			print(freeSpecialtxt);
			Controls.FreeSpecialistLabel:SetText(freeSpecialtxt);
			--Update suffix to use correct plurality.
			Controls.FreeSpecialistLabelSuffix:LocalizeAndSetText( "TXT_KEY_CITYVIEW_FREESPECIALIST_TEXT", freeSpecialists );
		else
			print("No free specialists...")
			local defSpecialist = tostring((GameDefines.BALANCE_UNHAPPINESS_PER_SPECIALIST / 100));
			Controls.FreeSpecialistLabel:SetText(defSpecialist);
			--Update suffix to use correct plurality.
			Controls.FreeSpecialistLabelSuffix:LocalizeAndSetText("TXT_KEY_CITYVIEW_NOFREESPECIALIST_TEXT");
			print(defSpecialist);
			print("TXT_KEY_CITYVIEW_NOFREESPECIALIST_TEXT");
		end

		Controls.BuildingsHeader:SetToolTipString(strMaintenanceTT);
		Controls.GreatWorkHeader:SetToolTipString(strMaintenanceTT);
		
		-- buildings that take specialists
		local numSpecialBuildingsInThisCity = 0;
		if specialistBuildingHeadingOpen then
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_SPECIAL_TEXT" );
			Controls.SpecialBuildingsHeaderLabel:SetText(localizedLabel);
		else
			local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_SPECIAL_TEXT" );
			Controls.SpecialBuildingsHeaderLabel:SetText(localizedLabel);
		end
		sortedList = {};
		thisId = 1;
		for building in GameInfo.Buildings() do
			local thisBuildingClass = GameInfo.BuildingClasses[building.BuildingClass];
			if thisBuildingClass.MaxGlobalInstances <= 0 and thisBuildingClass.MaxTeamInstances <= 0 then
				if not (building.IsCorporation == 1) then
					local buildingID= building.ID;
					if pCity:GetNumSpecialistsAllowedByBuilding(buildingID) > 0 then
						if (pCity:IsHasBuilding(buildingID)) then
							numSpecialBuildingsInThisCity = numSpecialBuildingsInThisCity + 1;
							local element = {};
							local name = Locale.ConvertTextKey( building.Description )
							element.name = name;
							element.ID = building.ID;
							sortedList[thisId] = element;
							thisId = thisId + 1;
						end
					end
				end
			end
		end
		table.sort(sortedList, function(a, b) return a.name < b.name end);
		if numSpecialBuildingsInThisCity > 0 then
			--if header is not hidden and is open
			Controls.SpecialBuildingsHeader:SetHide( false );
			Controls.SpecialistControlBox:SetHide( false );
			Controls.FreeSpecialistLabel:SetHide( false );
			Controls.FreeSpecialistLabelSuffix:SetHide( false );
			sortOrder = sortOrder + 1;
			otherSortedList[tostring( Controls.SpecialBuildingsHeader )] = sortOrder;
			sortOrder = sortOrder + 1;
			otherSortedList[tostring( Controls.SpecialistControlBox )] = sortOrder;
			if specialistBuildingHeadingOpen then
				Controls.SpecialBuildingsHeader:RegisterCallback( Mouse.eLClick, OnSpecialistBuildingsHeaderSelected );
				for i, v in ipairs(sortedList) do
					local building = GameInfo.Buildings[v.ID];
					AddBuildingButton( pCity, building );
				end
			else
				Controls.SpecialistControlBox:SetHide( true );
			end			
		else
			Controls.SpecialBuildingsHeader:SetHide( true );
			Controls.SpecialistControlBox:SetHide( true );
			Controls.FreeSpecialistLabel:SetHide( true );
			Controls.FreeSpecialistLabelSuffix:SetHide( true );
		end
		
		-- now add the wonders
		local numWondersInThisCity = 0;
		local numWondersWithSpecialistInThisCity = 0;
		if wonderHeadingOpen then
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_WONDERS_TEXT" );
			Controls.WondersHeaderLabel:SetText(localizedLabel);
		else
			local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_WONDERS_TEXT" );
			Controls.WondersHeaderLabel:SetText(localizedLabel);
		end
		local sortedList = {};
		local thisId = 1;
		for building in GameInfo.Buildings() do
			local thisBuildingClass = GameInfo.BuildingClasses[building.BuildingClass];
			if thisBuildingClass.MaxGlobalInstances > 0 or (thisBuildingClass.MaxPlayerInstances == 1 and building.SpecialistCount == 0) or thisBuildingClass.MaxTeamInstances > 0 then
				local buildingID= building.ID;
				if not (building.IsCorporation == 1) then
					if (pCity:IsHasBuilding(buildingID)) then
						numWondersInThisCity = numWondersInThisCity + 1;
						if(pCity:GetNumSpecialistsAllowedByBuilding(buildingID) > 0) then
							numWondersWithSpecialistInThisCity = numWondersWithSpecialistInThisCity + 1;
						end
					
						local element = {};
						local name = Locale.ConvertTextKey( building.Description )
						element.name = name;
						element.ID = building.ID;
						sortedList[thisId] = element;
						thisId = thisId + 1;
					end
				end
			end
		end
		table.sort(sortedList, function(a, b) return a.name < b.name end);
		
		Controls.SpecialistControlBox2:SetHide( true );
		if numWondersInThisCity > 0 then
			--if header is not hidden and is open
			Controls.WondersHeader:SetHide( false );
			sortOrder = sortOrder + 1;
			otherSortedList[tostring( Controls.WondersHeader )] = sortOrder;
			sortOrder = sortOrder + 1;
			otherSortedList[tostring( Controls.SpecialistControlBox2 )] = sortOrder;
			
			if wonderHeadingOpen then
				Controls.WondersHeader:RegisterCallback( Mouse.eLClick, OnWondersHeaderSelected );
				for i, v in ipairs(sortedList) do
					local building = GameInfo.Buildings[v.ID];
					AddBuildingButton( pCity, building );
				end
				
				if(numWondersWithSpecialistInThisCity > 0) then
					Controls.SpecialistControlBox2:SetHide( false );
				end
			end
		else
			Controls.WondersHeader:SetHide( true );
		end
			
		-- now add the Great Work buildings
		local numGreatWorkBuildingsInThisCity = 0;
		if greatWorkHeadingOpen then
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_GREAT_WORK_BUILDINGS_TEXT" );
			Controls.GreatWorkHeaderLabel:SetText(localizedLabel);
		else
			local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_GREAT_WORK_BUILDINGS_TEXT" );
			Controls.GreatWorkHeaderLabel:SetText(localizedLabel);
		end
		sortedList = {};
		thisId = 1;
		for building in GameInfo.Buildings() do
			local thisBuildingClass = GameInfo.BuildingClasses[building.BuildingClass];
			if thisBuildingClass.MaxGlobalInstances <= 0 and thisBuildingClass.MaxPlayerInstances ~= 1 and thisBuildingClass.MaxTeamInstances <= 0 then
				local thisBuilding = GameInfo.Buildings[building.ID];
				if thisBuilding.GreatWorkCount > 0 then
					if not (building.IsCorporation == 1) then
						if (pCity:IsHasBuilding(building.ID)) then
							numGreatWorkBuildingsInThisCity = numGreatWorkBuildingsInThisCity + 1;
							local element = {};
							local name = Locale.ConvertTextKey( building.Description )
							element.name = name;
							element.ID = building.ID;
							sortedList[thisId] = element;
							thisId = thisId + 1;
						end
					end
				end
			end
		end
		table.sort(sortedList, function(a, b) return a.name < b.name end);
		if numGreatWorkBuildingsInThisCity > 0 then
			--if header is not hidden and is open
			Controls.GreatWorkHeader:SetHide( false );
			sortOrder = sortOrder + 1;
			otherSortedList[tostring( Controls.GreatWorkHeader )] = sortOrder;
			if greatWorkHeadingOpen then
				Controls.GreatWorkHeader:RegisterCallback( Mouse.eLClick, OnGreatWorkHeaderSelected );
				for i, v in ipairs(sortedList) do
					local building = GameInfo.Buildings[v.ID];
					AddBuildingButton( pCity, building );
				end
			end
		else
			Controls.GreatWorkHeader:SetHide( true );
		end

		--Corps (CBO)
		local numCorpsInThisCity = 0;
		if corpsHeadingOpen then
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_CORPORATIONS_TEXT" );
			Controls.CorporationsHeaderLabel:SetText(localizedLabel);
		else
			local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_CORPORATIONS_TEXT" );
			Controls.CorporationsHeaderLabel:SetText(localizedLabel);
		end
		sortedList = {};
		thisId = 1;
		for building in GameInfo.Buildings() do
			local thisBuildingClass = GameInfo.BuildingClasses[building.BuildingClass];
			if (building.IsCorporation == 1) then
				local buildingID= building.ID;
				if (pCity:IsHasBuilding(buildingID)) then
					numCorpsInThisCity = numCorpsInThisCity + 1;				
					local element = {};
					local name = Locale.ConvertTextKey( building.Description )
					element.name = name;
					element.ID = building.ID;
					sortedList[thisId] = element;
					thisId = thisId + 1;
				end
			end
		end
		table.sort(sortedList, function(a, b) return a.name < b.name end);
		if numCorpsInThisCity > 0 then
			--if header is not hidden and is open
			Controls.CorporationsHeader:SetHide( false );
			sortOrder = sortOrder + 1;
			otherSortedList[tostring( Controls.CorporationsHeader )] = sortOrder;
			if corpsHeadingOpen then
				Controls.CorporationsHeader:RegisterCallback( Mouse.eLClick, OnCorpsHeaderSelected );
				for i, v in ipairs(sortedList) do
					local building = GameInfo.Buildings[v.ID];
					AddBuildingButton( pCity, building );
				end
			end
		else
			Controls.CorporationsHeader:SetHide( true );
		end
				
		-- the rest of the buildings
		local numBuildingsInThisCity = 0;
		if buildingHeadingOpen then
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_REGULARBUILDING_TEXT" );
			Controls.BuildingsHeaderLabel:SetText(localizedLabel);
		else
			local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_REGULARBUILDING_TEXT" );
			Controls.BuildingsHeaderLabel:SetText(localizedLabel);
		end
		sortedList = {};
		thisId = 1;
		for building in GameInfo.Buildings() do
			local thisBuildingClass = GameInfo.BuildingClasses[building.BuildingClass];
			if thisBuildingClass.MaxGlobalInstances <= 0 and thisBuildingClass.MaxPlayerInstances ~= 1 and thisBuildingClass.MaxTeamInstances <= 0 then
				if not (building.IsCorporation == 1) then
					local buildingID= building.ID;
					if pCity:GetNumSpecialistsAllowedByBuilding(buildingID) <= 0 then
						if (pCity:IsHasBuilding(buildingID) and GameInfo.Buildings[buildingID].GreatWorkCount == 0) then
							numBuildingsInThisCity = numBuildingsInThisCity + 1;
							local element = {};
							local name = Locale.ConvertTextKey( building.Description )
							element.name = name;
							element.ID = building.ID;
							sortedList[thisId] = element;
							thisId = thisId + 1;
						end
					end
				end
			end
		end
		table.sort(sortedList, function(a, b) return a.name < b.name end);
		if numBuildingsInThisCity > 0 then
			--if header is not hidden and is open
			Controls.BuildingsHeader:SetHide( false );
			sortOrder = sortOrder + 1;
			otherSortedList[tostring( Controls.BuildingsHeader )] = sortOrder;
			if buildingHeadingOpen then
				Controls.BuildingsHeader:RegisterCallback( Mouse.eLClick, OnBuildingsHeaderSelected );
				for i, v in ipairs(sortedList) do
					local building = GameInfo.Buildings[v.ID];
					AddBuildingButton( pCity, building );
				end
			end
		else
			Controls.BuildingsHeader:SetHide( true );
		end
		
		Controls.BuildingStack:SortChildren( CVSortFunction );
		
		Controls.BuildingStack:CalculateSize();
		Controls.BuildingStack:ReprocessAnchoring();
		
		Controls.WorkerManagementBox:CalculateSize();
		Controls.WorkerManagementBox:ReprocessAnchoring();
		
		Controls.GPStack:CalculateSize();
		Controls.GPStack:ReprocessAnchoring();
		
		RecalcPanelSize();
		
		-----------------------------------------
		-- Buying Plots
		-------------------------------------------
		szText = string.format( Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_BUY_TILE") );
	    Controls.BuyPlotButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_BUY_TILE_TT" ) );
		Controls.BuyPlotText:SetText(szText);
	    if (GameDefines["BUY_PLOTS_DISABLED"] ~= 0) then
			Controls.BuyPlotButton:SetDisabled(true);			
	    end
	    
		
		-------------------------------------------
		-- Resource Demanded
		-------------------------------------------
		
		local szResourceDemanded = "??? (Research Required)";
		
		if (pCity:GetResourceDemanded(true) ~= -1) then
			local pResourceInfo = GameInfo.Resources[pCity:GetResourceDemanded()];
			szResourceDemanded = Locale.ConvertTextKey(pResourceInfo.IconString) .. " " .. Locale.ConvertTextKey(pResourceInfo.Description);
			Controls.ResourceDemandedBox:SetHide(false);
			
		else
			Controls.ResourceDemandedBox:SetHide(true);
		end
				
		local iNumTurns = pCity:GetWeLoveTheKingDayCounter();
		if (iNumTurns > 0) then
			szText = Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_WLTKD_COUNTER", tostring(iNumTurns) );
			--- CBP
			if(pPlayer:IsGPWLTKD()) then
				szText = Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_WLTKD_COUNTER_UA", tostring(iNumTurns) );
				Controls.ResourceDemandedBox:SetToolTipString(Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_RESOURCE_FULFILLED_TT_UA" ) );
				Controls.ResourceDemandedBox:SetHide(false);
			elseif(pPlayer:IsCarnaval())then
				szText = Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_WLTKD_COUNTER_UA_CARNAVAL", tostring(iNumTurns) );
				Controls.ResourceDemandedBox:SetToolTipString(Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_RESOURCE_FULFILLED_TT_UA_CARNAVAL" ) );
				Controls.ResourceDemandedBox:SetHide(false);
			else
			-- END
			
			Controls.ResourceDemandedBox:SetToolTipString(Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_RESOURCE_FULFILLED_TT" ) );
			--CBP
			end
			-- END
		else
			szText = Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_RESOURCE_DEMANDED", szResourceDemanded );
			--CBP
			if(pPlayer:IsGPWLTKD()) then
				Controls.ResourceDemandedBox:SetToolTipString(Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_RESOURCE_DEMANDED_TT_UA" ) );
			elseif(pPlayer:IsCarnaval())then
				Controls.ResourceDemandedBox:SetToolTipString(Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_RESOURCE_DEMANDED_TT_UA_CARNAVAL" ) );
			else
				Controls.ResourceDemandedBox:SetToolTipString(Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_RESOURCE_DEMANDED_TT" ) );
			-- CBP
			end
			-- END
		end
		
		Controls.ResourceDemandedString:SetText(szText);
		Controls.ResourceDemandedBox:SetSizeX(Controls.ResourceDemandedString:GetSizeX() + 10);
		
		Controls.IconsStack:CalculateSize();
		Controls.IconsStack:ReprocessAnchoring();
		
		Controls.NotificationStack:CalculateSize();
		Controls.NotificationStack:ReprocessAnchoring();
		
		-------------------------------------------
		-- Raze City Button (Occupied Cities only)
		-------------------------------------------
		
		if (not pCity:IsOccupied() or pCity:IsRazing()) then		
			g_bRazeButtonDisabled = true;
			Controls.RazeCityButton:SetHide(true);
		else
			-- Can we not actually raze this city?
			if (not pPlayer:CanRaze(pCity, false)) then
				-- We COULD raze this city if it weren't a capital
				if (pPlayer:CanRaze(pCity, true)) then
					g_bRazeButtonDisabled = true;
					Controls.RazeCityButton:SetHide(false);
					Controls.RazeCityButton:SetDisabled(true);
					Controls.RazeCityButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_RAZE_BUTTON_DISABLED_BECAUSE_CAPITAL_TT" ) );
				-- Can't raze this city period
				else
					g_bRazeButtonDisabled = true;
					Controls.RazeCityButton:SetHide(true);
				end
			else
				g_bRazeButtonDisabled = false;
				Controls.RazeCityButton:SetHide(false);
				Controls.RazeCityButton:SetDisabled(false);		
				Controls.RazeCityButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_CITYVIEW_RAZE_BUTTON_TT" ) );
			end
		end

		-- Stop city razing
		if (pCity:IsRazing()) then
			g_bRazeButtonDisabled = false;
			Controls.UnrazeCityButton:SetHide(false);
		else
			g_bRazeButtonDisabled = true;
			Controls.UnrazeCityButton:SetHide(true);
		end
		
--		UpdateSpecialists(pCity);
		UpdateWorkingHexes();
		UpdateBuyPlotButton();

		-- Update left corner tooltips
		DoUpdateUpperLeftTooltips();
		
		-- display gold income
		local iGoldPerTurn = pCity:GetYieldRateTimes100(YieldTypes.YIELD_GOLD) / 100;
		Controls.GoldPerTurnLabel:SetText( Locale.ConvertTextKey("TXT_KEY_CITYVIEW_PERTURN_TEXT", iGoldPerTurn) );
		--Controls.ProdBox:SetToolTipString(strToolTip);
		
		-- display science income
		if (Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE)) then
			Controls.SciencePerTurnLabel:SetText( Locale.ConvertTextKey("TXT_KEY_CITYVIEW_OFF") );
		else
			local iSciencePerTurn = pCity:GetYieldRateTimes100(YieldTypes.YIELD_SCIENCE) / 100;
			Controls.SciencePerTurnLabel:SetText( Locale.ConvertTextKey("TXT_KEY_CITYVIEW_PERTURN_TEXT", iSciencePerTurn) );
		end
		--Controls.ScienceBox:SetToolTipString(strToolTip);
		
		local iCulturePerTurn = pCity:GetJONSCulturePerTurn();
		Controls.CulturePerTurnLabel:SetText( Locale.ConvertTextKey("TXT_KEY_CITYVIEW_PERTURN_TEXT", iCulturePerTurn) );
		--Controls.CultureBox:SetToolTipString(strToolTip);
		local cultureStored = pCity:GetJONSCultureStored();
		local cultureNext = pCity:GetJONSCultureThreshold();
		local cultureDiff = cultureNext - cultureStored;
		if iCulturePerTurn > 0 then
			local cultureTurns = math.ceil(cultureDiff / iCulturePerTurn);
			if (cultureTurns < 1) then
			   cultureTurns = 1
			end
			Controls.CultureTimeTillGrowthLabel:SetText( Locale.ConvertTextKey("TXT_KEY_CITYVIEW_TURNS_TILL_TILE_TEXT", cultureTurns) );
			Controls.CultureTimeTillGrowthLabel:SetHide( false );
		else
			Controls.CultureTimeTillGrowthLabel:SetHide( true );
		end
		local percentComplete = cultureStored / cultureNext;
		Controls.CultureMeter:SetPercent( percentComplete );
		
		if (Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
			Controls.FaithPerTurnLabel:SetText( Locale.ConvertTextKey("TXT_KEY_CITYVIEW_OFF") );
		else
			local iFaithPerTurn = pCity:GetFaithPerTurn();
			Controls.FaithPerTurnLabel:SetText( Locale.ConvertTextKey("TXT_KEY_CITYVIEW_PERTURN_TEXT", iFaithPerTurn) );
		end
		
		local iTourismPerTurn = pCity:GetBaseTourism() / 100;
		Controls.TourismPerTurnLabel:SetText( Locale.ConvertTextKey("TXT_KEY_CITYVIEW_PERTURN_TEXT", iTourismPerTurn) );
	
		-- CBP
		local iHappinessPerTurn = pCity:GetLocalHappiness();
		Controls.HappinessPerTurnLabel:SetText( Locale.ConvertTextKey("TXT_KEY_NET_HAPPINESS_TEXT", iHappinessPerTurn) );
		-- END
	
		-- CBP
		local iUnhappinessPerTurn = pCity:getUnhappinessAggregated();
		Controls.UnhappinessPerTurnLabel:SetText( Locale.ConvertTextKey("TXT_KEY_NET_UNHAPPINESS_TEXT", iUnhappinessPerTurn) );
		-- END

		local cityGrowth = pCity:GetFoodTurnsLeft();			
		if (pCity:IsFoodProduction() or pCity:FoodDifferenceTimes100() == 0) then
			Controls.CityGrowthLabel:SetText(Locale.ConvertTextKey("TXT_KEY_CITYVIEW_STAGNATION_TEXT"));
		elseif pCity:FoodDifference() < 0 then
			Controls.CityGrowthLabel:SetText(Locale.ConvertTextKey("TXT_KEY_CITYVIEW_STARVATION_TEXT"));
		else
			Controls.CityGrowthLabel:SetText(Locale.ConvertTextKey("TXT_KEY_CITYVIEW_TURNS_TILL_CITIZEN_TEXT", cityGrowth));
		end
		local iFoodPerTurn = pCity:FoodDifferenceTimes100() / 100;
		
		if (iFoodPerTurn >= 0) then
			Controls.FoodPerTurnLabel:SetText( Locale.ConvertTextKey("TXT_KEY_CITYVIEW_PERTURN_TEXT", iFoodPerTurn) );
		else
			Controls.FoodPerTurnLabel:SetText( Locale.ConvertTextKey("TXT_KEY_CITYVIEW_PERTURN_TEXT_NEGATIVE", iFoodPerTurn) );
		end

		local iCurrentFood = pCity:GetFood();
		local iFoodNeeded = pCity:GrowthThreshold();
		local iFoodDiff = pCity:FoodDifference();
		local iCurrentFoodPlusThisTurn = iCurrentFood + iFoodDiff;
			
		local fGrowthProgressPercent = iCurrentFood / iFoodNeeded;			
		
		-- Viewing mode only
		if (UI.IsCityScreenViewingMode()) then
			
			-- City Cycling
			Controls.PrevCityButton:SetDisabled( true );
			Controls.NextCityButton:SetDisabled( true );
			
			-- Governor
			Controls.BalancedFocusButton:SetDisabled( true );
			Controls.FoodFocusButton:SetDisabled( true );
			Controls.ProductionFocusButton:SetDisabled( true );
			Controls.GoldFocusButton:SetDisabled( true );
			Controls.ResearchFocusButton:SetDisabled( true );
			Controls.CultureFocusButton:SetDisabled( true );
			Controls.GPFocusButton:SetDisabled( true );
			Controls.FaithFocusButton:SetDisabled( true );
			Controls.AvoidGrowthButton:SetDisabled( true );
			Controls.ResetButton:SetDisabled( true );
			
			Controls.BoxOSlackers:SetDisabled( true );
			Controls.NoAutoSpecialistCheckbox:SetDisabled( true );
			Controls.NoAutoSpecialistCheckbox2:SetDisabled( true );
			
			-- Other
			Controls.RazeCityButton:SetDisabled( true );
			Controls.UnrazeCityButton:SetDisabled( true );
			
			Controls.BuyPlotButton:SetDisabled( true );
			-- Venice Edit (CBP)
			if(pCity:GetOwner() == Game.GetActivePlayer())then
				local bAnnex = Players[pCity:GetOwner()]:MayNotAnnex();
				if bAnnex then 
					Controls.BuyPlotButton:SetDisabled( false );
				end
			end
			--END
			
		else
			
			-- City Cycling
			Controls.PrevCityButton:SetDisabled( false );
			Controls.NextCityButton:SetDisabled( false );
			
			-- Governor
			Controls.BalancedFocusButton:SetDisabled( false );
			Controls.FoodFocusButton:SetDisabled( false );
			Controls.ProductionFocusButton:SetDisabled( false );
			Controls.GoldFocusButton:SetDisabled( false );
			Controls.ResearchFocusButton:SetDisabled( false );
			Controls.CultureFocusButton:SetDisabled( false );
			Controls.GPFocusButton:SetDisabled( false );
			Controls.FaithFocusButton:SetDisabled( false );
			Controls.AvoidGrowthButton:SetDisabled( false );
			Controls.ResetButton:SetDisabled( false );
			
			Controls.BoxOSlackers:SetDisabled( false );
			Controls.NoAutoSpecialistCheckbox:SetDisabled( false );
			Controls.NoAutoSpecialistCheckbox2:SetDisabled( false );
			
			-- Other
			if (not g_bRazeButtonDisabled) then
				Controls.RazeCityButton:SetDisabled( false );
				Controls.UnrazeCityButton:SetDisabled( false );
			end
			
			Controls.BuyPlotButton:SetDisabled( false );
		end
		
		if (pCity:GetOwner() ~= Game.GetActivePlayer()) then
			Controls.ProductionButton:SetDisabled(true);
			Controls.PurchaseButton:SetDisabled(true);
			Controls.EditButton:SetHide(true);
			Controls.EndTurnText:SetText(Locale.ConvertTextKey("TXT_KEY_CITYVIEW_RETURN_TO_ESPIONAGE"));
		end
		
		if(UI.IsCityScreenViewingMode()) then
			Controls.EditButton:SetHide(true);
		end
	end
end
Events.SerialEventCityScreenDirty.Add(OnCityViewUpdate);
Events.SerialEventCityInfoDirty.Add(OnCityViewUpdate);


-----------------------------------------------------------------
-----------------------------------------------------------------
function RecalcPanelSize()
	Controls.RightStack:CalculateSize();
	local size = math.min( screenSizeY + 30, Controls.RightStack:GetSizeY() + 85 );
	size = math.max( size, 160 );
    Controls.BuildingListBackground:SetSizeY( size );
    
	size = math.min( screenSizeY - 65, Controls.RightStack:GetSizeY() + 85 );
    Controls.ScrollPanel:SetSizeY( size );
	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end


-------------------------------------------------
-- On City Set Damage
-------------------------------------------------
function OnCitySetDamage(iDamage, iMaxDamage)
	
	local iHealthPercent = 1 - (iDamage / iMaxDamage);

    Controls.HealthMeter:SetPercent(iHealthPercent);
    
    if iHealthPercent > 0.66 then
        Controls.HealthMeter:SetTexture("CityNamePanelHealthBarGreen.dds");
    elseif iHealthPercent > 0.33 then
        Controls.HealthMeter:SetTexture("CityNamePanelHealthBarYellow.dds");
    else
        Controls.HealthMeter:SetTexture("CityNamePanelHealthBarRed.dds");
    end
    
    -- Show or hide the Health Bar as necessary
    if (iDamage == 0) then
		Controls.HealthFrame:SetHide(true);
	else
		Controls.HealthFrame:SetHide(false);
    end

end

-------------------------------------------------
-- Update Production Info
-------------------------------------------------
function DoUpdateProductionInfo( bNoProduction )
	
	local pCity = UI.GetHeadSelectedCity();
	local pPlayer = Players[pCity:GetOwner()];

	-- Production stored and needed
	local iStoredProduction = pCity:GetProductionTimes100() / 100;
	local iProductionNeeded = pCity:GetProductionNeeded();
	if (pCity:IsProductionProcess()) then
		iProductionNeeded = 0;
	end
	
	-- Base Production per turn
	local iProductionPerTurn = pCity:GetCurrentProductionDifferenceTimes100(false, false) / 100;--pCity:GetYieldRate(YieldTypes.YIELD_PRODUCTION);
	local iProductionModifier = pCity:GetProductionModifier() + 100;
	--iProductionPerTurn = iProductionPerTurn * iProductionModifier;
	--iProductionPerTurn = iProductionPerTurn / 100;
	
	-- Item being produced with food? (e.g. Settlers)
	--if (pCity:IsFoodProduction()) then
		--iProductionPerTurn = iProductionPerTurn + pCity:GetYieldRate(YieldTypes.YIELD_FOOD) - pCity:FoodConsumption(true);
	--end
	
	local strProductionPerTurn = Locale.ConvertTextKey("TXT_KEY_CITY_SCREEN_PROD_PER_TURN", iProductionPerTurn);
	Controls.ProductionOutput:SetText(strProductionPerTurn);
	
	-- Progress info for meter
	local iStoredProductionPlusThisTurn = iStoredProduction + iProductionPerTurn;
	
	local fProductionProgressPercent = iStoredProduction / iProductionNeeded;
	local fProductionProgressPlusThisTurnPercent = iStoredProductionPlusThisTurn / iProductionNeeded;
	if (fProductionProgressPlusThisTurnPercent > 1) then
		fProductionProgressPlusThisTurnPercent = 1
	end
	
	Controls.ProductionMeter:SetPercents( fProductionProgressPercent, fProductionProgressPlusThisTurnPercent );
	
	-- Turns left
	local productionTurnsLeft = pCity:GetProductionTurnsLeft();
	
	--if pCity:IsOccupation() then
		--Controls.ProductionTurnsLabel:SetText(" (City in unrest)");
	--else
	
	local strNumTurns;
	if(productionTurnsLeft > 99) then
		strNumTurns = Locale.ConvertTextKey("TXT_KEY_PRODUCTION_HELP_99PLUS_TURNS");
	else
		strNumTurns = Locale.ConvertTextKey("TXT_KEY_PRODUCTION_HELP_NUM_TURNS", productionTurnsLeft);
	end
	
	
	
	local bGeneratingProduction = pCity:IsProductionProcess() or pCity:GetCurrentProductionDifferenceTimes100(false, false) == 0;
	
	if (bGeneratingProduction) then
		strNumTurns = "";
	end
	
	-- Indicator for the fact that the empire is very unhappy
	if (pPlayer:IsEmpireVeryUnhappy()) then
		strNumTurns = strNumTurns .. " [ICON_HAPPINESS_4]";
	end
	
	if (not bGeneratingProduction) then
		Controls.ProductionTurnsLabel:SetText("(" .. strNumTurns .. ")");
	else
		Controls.ProductionTurnsLabel:SetText(strNumTurns);
	end
	
	--end
	
	if bNoProduction then
		Controls.ProductionTurnsLabel:SetHide(true);
	else
		Controls.ProductionTurnsLabel:SetHide(false);
	end
	
	-----------------------------
	-- TOOLTIP
	-----------------------------
	
	local strToolTip = "";

	-- What is being produced right now?
	if (bNoProduction) then
		strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_PRODUCTION_HELP_NOTHING");
	else
		if (not pCity:IsProductionProcess()) then
			strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_PRODUCTION_HELP_TEXT", pCity:GetProductionNameKey(), strNumTurns);
			strToolTip = strToolTip .. "[NEWLINE]----------------[NEWLINE]";
			strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_PRODUCTION_PROGRESS", iStoredProduction, iProductionNeeded);
		end
	end
	
	local iBaseProductionPT = pCity:GetBaseYieldRate(YieldTypes.YIELD_PRODUCTION);
	
	-- Output
	local strBase = Locale.ConvertTextKey("TXT_KEY_YIELD_BASE", iBaseProductionPT, "[ICON_PRODUCTION]");
	local strTotal = Locale.ConvertTextKey("TXT_KEY_YIELD_TOTAL", iProductionPerTurn, "[ICON_PRODUCTION]");
	local strOutput = strBase .. "[NEWLINE]" .. strTotal;
	strToolTip = strToolTip .. "[NEWLINE]";
	
	-- This builds the tooltip from C++
	local strCodeToolTip = pCity:GetYieldModifierTooltip(YieldTypes.YIELD_PRODUCTION);
	if (strCodeToolTip ~= "") then
		strOutput = strOutput .. "[NEWLINE]----------------" .. strCodeToolTip;
	end

	strToolTip = strToolTip .. strOutput;
	
	--Controls.ProductionDescriptionBox:SetToolTipString(strToolTip);
	Controls.ProductionPortraitButton:SetToolTipString(strToolTip);
	
	-- Info for the upper-left display
	Controls.ProdPerTurnLabel:SetText( Locale.ConvertTextKey("TXT_KEY_CITYVIEW_PERTURN_TEXT", iProductionPerTurn) );
	
	local strProductionHelp = GetProductionTooltip(pCity);
	
	Controls.ProdBox:SetToolTipString(strProductionHelp);
	
end


-------------------------------------------------
-- Update Tooltips in the upper-left part of the screen
-------------------------------------------------
function DoUpdateUpperLeftTooltips()
	
	local pCity = UI.GetHeadSelectedCity();
	
	local strFoodToolTip = GetFoodTooltip(pCity);
	Controls.FoodBox:SetToolTipString(strFoodToolTip);
	Controls.PopulationBox:SetToolTipString(strFoodToolTip);
	
	local strGoldToolTip = GetGoldTooltip(pCity);
	Controls.GoldBox:SetToolTipString(strGoldToolTip);
	
	local strScienceToolTip = GetScienceTooltip(pCity);
	Controls.ScienceBox:SetToolTipString(strScienceToolTip);
	
	local strCultureToolTip = GetCultureTooltip(pCity);
	Controls.CultureBox:SetToolTipString(strCultureToolTip);
	
	local strFaithToolTip = GetFaithTooltip(pCity);
	Controls.FaithBox:SetToolTipString(strFaithToolTip);
	
	local strTourismToolTip = GetTourismTooltip(pCity);
	Controls.TourismBox:SetToolTipString(strTourismToolTip);

-- CBP
	local strCityHappinessTooltip = GetCityHappinessTooltip(pCity);
	Controls.HappinessBox:SetToolTipString(strCityHappinessTooltip);

	local strCityUnhappinessTooltip = GetCityUnhappinessTooltip(pCity);
	Controls.UnhappinessBox:SetToolTipString(strCityUnhappinessTooltip);
-- END
end

-------------------------------------------------
-- Enter City Screen
-------------------------------------------------
function OnEnterCityScreen()
	
	local pCity = UI.GetHeadSelectedCity();
	
	if (pCity ~= nil) then
		Network.SendUpdateCityCitizens(pCity:GetID());
	end

	LuaEvents.TryQueueTutorial("CITY_SCREEN", true);
	
	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
end
Events.SerialEventEnterCityScreen.Add(OnEnterCityScreen);


-------------------------------------------------
-------------------------------------------------
function PlotButtonClicked( iPlotIndex )
	if Players[Game.GetActivePlayer()]:IsTurnActive() then
		if iPlotIndex > 0 then
			local pCity = UI.GetHeadSelectedCity();
			Network.SendDoTask(pCity:GetID(), TaskTypes.TASK_CHANGE_WORKING_PLOT, iPlotIndex, -1, false, bAlt, bShift, bCtrl);
		end
	end
end

-------------------------------------------------
-------------------------------------------------
function BuyPlotAnchorButtonClicked( iPlotIndex )

	if not Players[Game.GetActivePlayer()]:IsTurnActive() then
		return;
	end
	
	local activePlayerID = Game.GetActivePlayer();
	local pHeadSelectedCity = UI.GetHeadSelectedCity();
	if pHeadSelectedCity then
		local plot = pHeadSelectedCity:GetCityIndexPlot( iPlotIndex );
		local plotX = plot:GetX();
		local plotY = plot:GetY();
		Network.SendCityBuyPlot(pHeadSelectedCity:GetID(), plotX, plotY);
		UI.UpdateCityScreen();
		Events.AudioPlay2DSound("AS2D_INTERFACE_BUY_TILE");		
	end
	return true;
end


-------------------------------------------------
-------------------------------------------------
function UpdateWorkingHexes()
		
	local pCity = UI.GetHeadSelectedCity();
	
    if( pCity == nil ) then
        return;
    end
    
	if (UI.IsCityScreenUp()) then   
	
		-- display worked plots
		g_PlotButtonIM:ResetInstances();
		for i = 0, pCity:GetNumCityPlots() - 1, 1 do
			local plot = pCity:GetCityIndexPlot( i );
			if (plot ~= nil) then
				
				bNoHighlight = false;
				
				if ( plot:GetOwner() == pCity:GetOwner() ) then
				
					if workerHeadingOpen then
						local hexPos = ToHexFromGrid( Vector2( plot:GetX(), plot:GetY() ) );
						local worldPos = HexToWorld( hexPos );
					
						-- the city itself
						if ( i == 0 ) then
							local controlTable = g_PlotButtonIM:GetInstance();						
							controlTable.PlotButtonAnchor:SetWorldPosition( VecAdd( worldPos, WorldPositionOffset ) );
							IconHookup(	11, 45, "CITIZEN_ATLAS", controlTable.PlotButtonImage);
							controlTable.PlotButtonImage:SetToolTipString( Locale.ConvertTextKey("TXT_KEY_CITYVIEW_CITY_CENTER") );
							controlTable.PlotButtonImage:SetVoid1( -1 );
							controlTable.PlotButtonImage:RegisterCallback( Mouse.eLCLick, OnResetForcedTiles);
							
							DoTestViewingModeOnly(controlTable);
							
							--Events.SerialEventHexHighlight( ToHexFromGrid( Vector2( plot:GetX(), plot:GetY() ) ), true, Vector4( 1.0, 1.0, 1.0, 1 ) );
						-- FORCED worked plot
						elseif ( pCity:IsWorkingPlot( plot ) and pCity:IsForcedWorkingPlot( plot ) ) then
							local controlTable = g_PlotButtonIM:GetInstance();						
							controlTable.PlotButtonAnchor:SetWorldPosition( VecAdd( worldPos, WorldPositionOffset ) );
							IconHookup(	10, 45, "CITIZEN_ATLAS", controlTable.PlotButtonImage);
							controlTable.PlotButtonImage:SetToolTipString( Locale.ConvertTextKey("TXT_KEY_CITYVIEW_FORCED_WORK_TILE") );
							controlTable.PlotButtonImage:SetVoid1( i );
							controlTable.PlotButtonImage:RegisterCallback( Mouse.eLCLick, PlotButtonClicked);
							
							DoTestViewingModeOnly(controlTable);
							
							--Events.SerialEventHexHighlight( ToHexFromGrid( Vector2( plot:GetX(), plot:GetY() ) ), true, Vector4( 1.0, 1.0, 1.0, 1 ) );
						-- AI-picked worked plot
						elseif ( pCity:IsWorkingPlot( plot ) ) then						
							local controlTable = g_PlotButtonIM:GetInstance();						
							controlTable.PlotButtonAnchor:SetWorldPosition( VecAdd( worldPos, WorldPositionOffset ) );
							IconHookup(	0, 45, "CITIZEN_ATLAS", controlTable.PlotButtonImage);
							controlTable.PlotButtonImage:SetToolTipString( Locale.ConvertTextKey("TXT_KEY_CITYVIEW_GUVNA_WORK_TILE") );
							controlTable.PlotButtonImage:SetVoid1( i );
							controlTable.PlotButtonImage:RegisterCallback( Mouse.eLCLick, PlotButtonClicked);
							
							DoTestViewingModeOnly(controlTable);
							
							--Events.SerialEventHexHighlight( ToHexFromGrid( Vector2( plot:GetX(), plot:GetY() ) ), true, Vector4( 0.0, 1.0, 0.0, 1 ) );
						-- Owned by another one of our Cities
						elseif ( plot:GetWorkingCity():GetID() ~= pCity:GetID() and  plot:GetWorkingCity():IsWorkingPlot( plot ) ) then
							local controlTable = g_PlotButtonIM:GetInstance();						
							controlTable.PlotButtonAnchor:SetWorldPosition( VecAdd( worldPos, WorldPositionOffset ) );
							IconHookup(	12, 45, "CITIZEN_ATLAS", controlTable.PlotButtonImage);
							controlTable.PlotButtonImage:SetToolTipString( Locale.ConvertTextKey("TXT_KEY_CITYVIEW_NUTHA_CITY_TILE") );
							controlTable.PlotButtonImage:SetVoid1( i );
							controlTable.PlotButtonImage:RegisterCallback( Mouse.eLCLick, PlotButtonClicked);
							
							DoTestViewingModeOnly(controlTable);
							
							--Events.SerialEventHexHighlight( ToHexFromGrid( Vector2( plot:GetX(), plot:GetY() ) ), true, Vector4( 0.0, 0.0, 1.0, 1 ) );
						-- Blockaded water plot
						elseif ( plot:IsWater() and pCity:IsPlotBlockaded( plot ) ) then
							local controlTable = g_PlotButtonIM:GetInstance();						
							controlTable.PlotButtonAnchor:SetWorldPosition( VecAdd( worldPos, WorldPositionOffset ) );
							IconHookup(	13, 45, "CITIZEN_ATLAS", controlTable.PlotButtonImage);
							controlTable.PlotButtonImage:SetToolTipString( Locale.ConvertTextKey("TXT_KEY_CITYVIEW_BLOCKADED_CITY_TILE") );
							controlTable.PlotButtonImage:SetVoid1( -1 );
							controlTable.PlotButtonImage:RegisterCallback( Mouse.eLCLick, PlotButtonClicked);
							
							DoTestViewingModeOnly(controlTable);
							
							--Events.SerialEventHexHighlight( ToHexFromGrid( Vector2( plot:GetX(), plot:GetY() ) ), true, Vector4( 1.0, 0.0, 0.0, 1 ) );
						-- Enemy Unit standing here
						elseif ( plot:IsVisibleEnemyUnit(pCity:GetOwner()) ) then
							local controlTable = g_PlotButtonIM:GetInstance();						
							controlTable.PlotButtonAnchor:SetWorldPosition( VecAdd( worldPos, WorldPositionOffset ) );
							IconHookup(	13, 45, "CITIZEN_ATLAS", controlTable.PlotButtonImage);
							controlTable.PlotButtonImage:SetToolTipString( Locale.ConvertTextKey("TXT_KEY_CITYVIEW_ENEMY_UNIT_CITY_TILE") );
							controlTable.PlotButtonImage:SetVoid1( -1 );
							controlTable.PlotButtonImage:RegisterCallback( Mouse.eLCLick, PlotButtonClicked);
							
							DoTestViewingModeOnly(controlTable);
							
							--Events.SerialEventHexHighlight( ToHexFromGrid( Vector2( plot:GetX(), plot:GetY() ) ), true, Vector4( 1.0, 0.0, 0.0, 1 ) );
						-- Other: turn off highlight
						elseif ( pCity:CanWork( plot ) or plot:GetWorkingCity():GetID() ~= pCity:GetID() ) then
							local controlTable = g_PlotButtonIM:GetInstance();						
							controlTable.PlotButtonAnchor:SetWorldPosition( VecAdd( worldPos, WorldPositionOffset ) );
							controlTable.PlotButtonImage:SetToolTipString( Locale.ConvertTextKey("TXT_KEY_CITYVIEW_UNWORKED_CITY_TILE") );
							IconHookup(	9, 45, "CITIZEN_ATLAS", controlTable.PlotButtonImage);
							controlTable.PlotButtonImage:SetVoid1( i );
							controlTable.PlotButtonImage:RegisterCallback( Mouse.eLCLick, PlotButtonClicked);
							bNoHighlight = true;
							
							DoTestViewingModeOnly(controlTable);
							
						end
						
					else
						bNoHighlight = true;
					end
				end
				
				--if (bNoHighlight) then
					Events.SerialEventHexHighlight( ToHexFromGrid( Vector2( plot:GetX(), plot:GetY() ) ), false, Vector4( 0.0, 1.0, 0.0, 1 ) );
				--end
			end
		end
		
		-- Add buy plot buttons
		g_BuyPlotButtonIM:ResetInstances();
		if UI.GetInterfaceMode() == InterfaceModeTypes.INTERFACEMODE_PURCHASE_PLOT then
			Events.RequestYieldDisplay( YieldDisplayTypes.CITY_PURCHASABLE, pCity:GetX(), pCity:GetY() );
			for i = 0, pCity:GetNumCityPlots() - 1, 1 do
				local plot = pCity:GetCityIndexPlot( i );
				if (plot ~= nil) then
					local hexPos = ToHexFromGrid( Vector2( plot:GetX(), plot:GetY() ) );
					local worldPos = HexToWorld( hexPos );
					if (pCity:CanBuyPlotAt(plot:GetX(), plot:GetY(), false)) then
						local controlTable = g_BuyPlotButtonIM:GetInstance();						
						controlTable.BuyPlotButtonAnchor:SetWorldPosition( VecAdd( worldPos, WorldPositionOffset2 ) );
						local iPlotCost = pCity:GetBuyPlotCost( plot:GetX(), plot:GetY() );
						local strText = Locale.ConvertTextKey("TXT_KEY_CITYVIEW_CLAIM_NEW_LAND",iPlotCost);
						controlTable.BuyPlotAnchoredButton:SetToolTipString( strText );
						controlTable.BuyPlotAnchoredButtonLabel:SetText( tostring(iPlotCost) );
						controlTable.BuyPlotAnchoredButton:SetDisabled( false );
						controlTable.BuyPlotAnchoredButton:SetVoid1( i );
						controlTable.BuyPlotAnchoredButton:RegisterCallback( Mouse.eLCLick, BuyPlotAnchorButtonClicked);
					elseif (pCity:CanBuyPlotAt(plot:GetX(), plot:GetY(), true)) then
						local controlTable = g_BuyPlotButtonIM:GetInstance();						
						controlTable.BuyPlotButtonAnchor:SetWorldPosition( VecAdd( worldPos, WorldPositionOffset2 ) );
						local iPlotCost = pCity:GetBuyPlotCost( plot:GetX(), plot:GetY() );						
						local strText = Locale.ConvertTextKey("TXT_KEY_CITYVIEW_NEED_MONEY_BUY_TILE",iPlotCost);
						controlTable.BuyPlotAnchoredButton:SetToolTipString( strText );
						controlTable.BuyPlotAnchoredButton:SetDisabled( true );
						controlTable.BuyPlotAnchoredButtonLabel:SetText( "[COLOR_WARNING_TEXT]"..tostring(iPlotCost).."[ENDCOLOR]" );
					end
				end
			end
			local aPurchasablePlots = {pCity:GetBuyablePlotList()};
			for i = 1, #aPurchasablePlots, 1 do
				Events.SerialEventHexHighlight( ToHexFromGrid( Vector2( aPurchasablePlots[i]:GetX(), aPurchasablePlots[i]:GetY() ) ), true, Vector4( 1.0, 0.0, 1.0, 1 ) );
			end

		-- Standard mode - show plots that will be acquired by culture
		else
			local aPurchasablePlots = {pCity:GetBuyablePlotList()};
			for i = 1, #aPurchasablePlots, 1 do
				Events.SerialEventHexHighlight( ToHexFromGrid( Vector2( aPurchasablePlots[i]:GetX(), aPurchasablePlots[i]:GetY() ) ), true, Vector4( 1.0, 0.0, 1.0, 1 ) );
			end
		end
    end
end
Events.SerialEventCityHexHighlightDirty.Add(UpdateWorkingHexes);

-------------------------------------------------
function DoTestViewingModeOnly(controlTable)
	
	-- Viewing mode only?
	if (UI.IsCityScreenViewingMode()) then
		controlTable.PlotButtonImage:SetDisabled(true);
	else
		controlTable.PlotButtonImage:SetDisabled(false);
	end
	
end	



-------------------------------------------------
-------------------------------------------------
function OnProductionClick()
	
	local city = UI.GetHeadSelectedCity();
	local cityID = city:GetID();
	local popupInfo = {
		Type = ButtonPopupTypes.BUTTONPOPUP_CHOOSEPRODUCTION,
		Data1 = cityID,
		Data2 = -1,
		Data3 = -1,
		Option1 = (productionQueueOpen and city:GetOrderQueueLength() > 0),
		Option2 = false;
	}
	Events.SerialEventGameMessagePopup(popupInfo);
    -- send production popup message
end
Controls.ProductionButton:RegisterCallback( Mouse.eLClick, OnProductionClick );


-------------------------------------------------
-------------------------------------------------
function OnRemoveClick( num )	
	Game.SelectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_POP_ORDER, num);
end
Controls.b1remove:RegisterCallback( Mouse.eLClick, OnRemoveClick );
Controls.b1remove:SetVoid1( 0 );
Controls.b2remove:RegisterCallback( Mouse.eLClick, OnRemoveClick );
Controls.b2remove:SetVoid1( 1 );
Controls.b3remove:RegisterCallback( Mouse.eLClick, OnRemoveClick );
Controls.b3remove:SetVoid1( 2 );
Controls.b4remove:RegisterCallback( Mouse.eLClick, OnRemoveClick );
Controls.b4remove:SetVoid1( 3 );
Controls.b5remove:RegisterCallback( Mouse.eLClick, OnRemoveClick );
Controls.b5remove:SetVoid1( 4 );
Controls.b6remove:RegisterCallback( Mouse.eLClick, OnRemoveClick );
Controls.b6remove:SetVoid1( 5 );

-------------------------------------------------
-------------------------------------------------
function OnSwapClick( num )
	print()
	Game.SelectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_SWAP_ORDER, num);
end
Controls.b1down:RegisterCallback( Mouse.eLClick, OnSwapClick );
Controls.b1down:SetVoid1( 0 );

Controls.b2up:RegisterCallback( Mouse.eLClick, OnSwapClick );
Controls.b2up:SetVoid1( 0 );
Controls.b2down:RegisterCallback( Mouse.eLClick, OnSwapClick );
Controls.b2down:SetVoid1( 1 );

Controls.b3up:RegisterCallback( Mouse.eLClick, OnSwapClick );
Controls.b3up:SetVoid1( 1 );
Controls.b3down:RegisterCallback( Mouse.eLClick, OnSwapClick );
Controls.b3down:SetVoid1( 2 );

Controls.b4up:RegisterCallback( Mouse.eLClick, OnSwapClick );
Controls.b4up:SetVoid1( 2 );
Controls.b4down:RegisterCallback( Mouse.eLClick, OnSwapClick );
Controls.b4down:SetVoid1( 3 );

Controls.b5up:RegisterCallback( Mouse.eLClick, OnSwapClick );
Controls.b5up:SetVoid1( 3 );
Controls.b5down:RegisterCallback( Mouse.eLClick, OnSwapClick );
Controls.b5down:SetVoid1( 4 );

Controls.b6up:RegisterCallback( Mouse.eLClick, OnSwapClick );
Controls.b6up:SetVoid1( 4 );
--Controls.b6down:RegisterCallback( Mouse.eLClick, OnSwapClick );
--Controls.b6down:SetVoid1( 5 );


-------------------------------------------------
-------------------------------------------------

local g_iCurrentSpecialist = -1;
local g_bCurrentSpecialistGrowth = true;

---------------------------------------------------------------
-- Specialist Automation Checkbox
---------------------------------------------------------------
function OnNoAutoSpecialistCheckboxClick()
	local bValue = false;
	
	-- Checkbox was JUST turned on, 
	if (not UI.GetHeadSelectedCity():IsNoAutoAssignSpecialists()) then
		bValue = true;
	end
	
	Game.SelectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_DO_TASK, TaskTypes.TASK_NO_AUTO_ASSIGN_SPECIALISTS, -1, -1, bValue);
end
Controls.NoAutoSpecialistCheckbox:RegisterCallback(Mouse.eLClick, OnNoAutoSpecialistCheckboxClick);
Controls.NoAutoSpecialistCheckbox2:RegisterCallback(Mouse.eLClick, OnNoAutoSpecialistCheckboxClick);

---------------------------------------------------------------
-- Clicking on Building instances to add or remove Specialists
---------------------------------------------------------------
function OnBuildingClick(iBuilding)
	--local pCity = UI.GetHeadSelectedCity();
	--
	--local iNumSpecialistsAllowed = pCity:GetNumSpecialistsAllowedByBuilding(iBuilding)
	--local iNumSpecialistsAssigned = pCity:GetNumSpecialistsInBuilding(iBuilding);
	--
	--if (iNumSpecialistsAllowed > 0) then
		--
		---- If Specialists are automated then you can't change things with them
		--if (not pCity:IsNoAutoAssignSpecialists()) then
			--local bValue = true;
			--Game.SelectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_DO_TASK, TaskTypes.TASK_NO_AUTO_ASSIGN_SPECIALISTS, -1, -1, bValue);
			--Controls.NoAutoSpecialistCheckbox:SetCheck(true);
		--end
		--
		--local iSpecialist = GameInfoTypes[GameInfo.Buildings[iBuilding].SpecialistType];
		--
		---- Switched to a different specialist type, so clicking on the building will grow the count
		--if (iSpecialist ~= g_iCurrentSpecialist) then
			--g_bCurrentSpecialistGrowth = true;
		--end
		--
		---- Nobody assigned yet, so we must grow
		--if (iNumSpecialistsAssigned == 0) then
			--g_bCurrentSpecialistGrowth = true;
		--end
		--
		---- If we can add something, add it
		--if (g_bCurrentSpecialistGrowth and pCity:IsCanAddSpecialistToBuilding(iBuilding)) then
			--Game.SelectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_DO_TASK, TaskTypes.TASK_ADD_SPECIALIST, iSpecialist, iBuilding);
			--
		---- Can't add something, so remove what's here instead
		--elseif (iNumSpecialistsAssigned > 0) then
			--Game.SelectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_DO_TASK, TaskTypes.TASK_REMOVE_SPECIALIST, iSpecialist, iBuilding);
			--
			---- Start removing Specialists
			--g_bCurrentSpecialistGrowth = false;
		--end
		--
		--g_iCurrentSpecialist = iSpecialist;
	--end
	--
end


function DisableSpecialist(iBuilding, slot)
end


function AddSpecialist(iBuilding, slot)
	local pCity = UI.GetHeadSelectedCity();
				
	-- If Specialists are automated then you can't change things with them
	if (not pCity:IsNoAutoAssignSpecialists()) then
		Game.SelectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_DO_TASK, TaskTypes.TASK_NO_AUTO_ASSIGN_SPECIALISTS, -1, -1, true);
		Controls.NoAutoSpecialistCheckbox:SetCheck(true);
		Controls.NoAutoSpecialistCheckbox2:SetCheck(true);
	end
	
	local iSpecialist = GameInfoTypes[GameInfo.Buildings[iBuilding].SpecialistType];
	
	-- If we can add something, add it
	if (pCity:IsCanAddSpecialistToBuilding(iBuilding)) then
		Game.SelectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_DO_TASK, TaskTypes.TASK_ADD_SPECIALIST, iSpecialist, iBuilding);
	end
	
	--g_iCurrentSpecialist = iSpecialist;
	specialistTable[iBuilding][slot] = true;
	
end

function RemoveSpecialist(iBuilding, slot)
	local pCity = UI.GetHeadSelectedCity();
	
	local iNumSpecialistsAssigned = pCity:GetNumSpecialistsInBuilding(iBuilding);
				
	-- If Specialists are automated then you can't change things with them
	if (not pCity:IsNoAutoAssignSpecialists()) then
		Game.SelectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_DO_TASK, TaskTypes.TASK_NO_AUTO_ASSIGN_SPECIALISTS, -1, -1, true);
		Controls.NoAutoSpecialistCheckbox:SetCheck(true);
		Controls.NoAutoSpecialistCheckbox2:SetCheck(true);
	end
	
	local iSpecialist = GameInfoTypes[GameInfo.Buildings[iBuilding].SpecialistType];
	
	-- If we can remove something, remove it
	if (iNumSpecialistsAssigned > 0) then
		Game.SelectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_DO_TASK, TaskTypes.TASK_REMOVE_SPECIALIST, iSpecialist, iBuilding);
	end
	
	--g_iCurrentSpecialist = iSpecialist;
	
	specialistTable[iBuilding][slot] = false;
end

-------------------------------------------------
-------------------------------------------------
function OnNextCityButton()
	Game.DoControl(GameInfoTypes.CONTROL_NEXTCITY)
end
Controls.NextCityButton:RegisterCallback( Mouse.eLClick, OnNextCityButton );

-------------------------------------------------
-------------------------------------------------
function OnPrevCityButton()
	Game.DoControl(GameInfoTypes.CONTROL_PREVCITY)
end
Controls.PrevCityButton:RegisterCallback( Mouse.eLClick, OnPrevCityButton );

-------------------------------------------------
-------------------------------------------------
function UpdateBuyPlotButton()

end

-------------------------------------------------
-------------------------------------------------
function OnBuyPlotClick()

	if not Players[Game.GetActivePlayer()]:IsTurnActive() then
		return;
	end
	
	local city = UI.GetHeadSelectedCity();
	
	if (city == nil) then
		return;
	end;
	
	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_PURCHASE_PLOT);
	UpdateWorkingHexes();
	
	--UpdateBuyPlotButton();
end
Controls.BuyPlotButton:RegisterCallback( Mouse.eLClick, OnBuyPlotClick );

-------------------------------------------------
-- Plot moused over
-------------------------------------------------
function OnMouseOverHex( hexX, hexY )
	
	if UI.GetInterfaceMode() == InterfaceModeTypes.INTERFACEMODE_PURCHASE_PLOT then
		
		local city = UI.GetHeadSelectedCity();
		
		if (city == nil) then
			return;
		end;

		local strText = "---";
		
		-- Can buy this plot
		if (city:CanBuyPlotAt( hexX, hexY, true)) then
			local iPlotCost = city:GetBuyPlotCost( hexX, hexY );
			strText = "[ICON_GOLD] " .. iPlotCost;
		end
		
		Controls.BuyPlotText:SetText(strText);
	end
	
end
Events.SerialEventMouseOverHex.Add( OnMouseOverHex );

-------------------------------------------------
-------------------------------------------------
function OnReturnToMapButton()
	--CloseScreen();
	Events.SerialEventExitCityScreen();
end
Controls.ReturnToMapButton:RegisterCallback( Mouse.eLClick, OnReturnToMapButton);

-------------------------------------------------
-------------------------------------------------
function OnRazeButton()

	local pCity = UI.GetHeadSelectedCity();
	
	if (pCity == nil) then
		return;
	end;
	
	local popupInfo = {
		Type = ButtonPopupTypes.BUTTONPOPUP_CONFIRM_CITY_TASK,
		Data1 = pCity:GetID(),
		Data2 = TaskTypes.TASK_RAZE,
		}
    
	Events.SerialEventGameMessagePopup( popupInfo );
end
Controls.RazeCityButton:RegisterCallback( Mouse.eLClick, OnRazeButton);

-------------------------------------------------
-------------------------------------------------
function OnUnrazeButton()

	if Players[Game.GetActivePlayer()]:IsTurnActive() then
		local pCity = UI.GetHeadSelectedCity();
		
		if (pCity == nil) then
			return;
		end;
		
		Network.SendDoTask(pCity:GetID(), TaskTypes.TASK_UNRAZE, -1, -1, false, false, false, false);
	end
end
Controls.UnrazeCityButton:RegisterCallback( Mouse.eLClick, OnUnrazeButton);

-------------------------------------------------
-------------------------------------------------
function OnPurchaseButton()
	local city = UI.GetHeadSelectedCity();
	local cityID = city:GetID();
	local popupInfo = {
		Type = ButtonPopupTypes.BUTTONPOPUP_CHOOSEPRODUCTION,
		Data1 = cityID,
		Data2 = -1,
		Data3 = -1,
		Option1 = (productionQueueOpen and city:GetOrderQueueLength() > 0),
		Option2 = true;
	}
	Events.SerialEventGameMessagePopup(popupInfo);
    -- send production popup message

end
Controls.PurchaseButton:RegisterCallback( Mouse.eLClick, OnPurchaseButton);


function OnPortraitRClicked()
	local city = UI.GetHeadSelectedCity();
	local cityID = city:GetID();

	local searchString = "";
	local unitProduction = city:GetProductionUnit();
	local buildingProduction = city:GetProductionBuilding();
	local projectProduction = city:GetProductionProject();
	local processProduction = city:GetProductionProcess();
	local noProduction = false;

	if unitProduction ~= -1 then
		local thisUnitInfo = GameInfo.Units[unitProduction];
		searchString = Locale.ConvertTextKey( thisUnitInfo.Description );
	elseif buildingProduction ~= -1 then
		local thisBuildingInfo = GameInfo.Buildings[buildingProduction];
		searchString = Locale.ConvertTextKey( thisBuildingInfo.Description );
	elseif projectProduction ~= -1 then
		local thisProjectInfo = GameInfo.Projects[projectProduction];
		searchString = Locale.ConvertTextKey( thisProjectInfo.Description );
	elseif processProduction ~= -1 then
		local pProcessInfo = GameInfo.Processes[processProduction];
		searchString = Locale.ConvertTextKey( pProcessInfo.Description );
	else
		noProduction = true;
	end
		
	if noProduction == false then
	
		--CloseScreen();

		-- search by name
		Events.SearchForPediaEntry( searchString );		
	end
		
end
Controls.ProductionPortraitButton:RegisterCallback( Mouse.eRClick, OnPortraitRClicked );


----------------------------------------------------------------
----------------------------------------------------------------
function OnHideQueue( bIsChecked )
	productionQueueOpen = bIsChecked;
	OnCityViewUpdate();
end
Controls.HideQueueButton:RegisterCheckHandler( OnHideQueue );


----------------------------------------------------------------
----------------------------------------------------------------

function FocusChanged( focus )
	if Players[Game.GetActivePlayer()]:IsTurnActive() then
		local pCity = UI.GetHeadSelectedCity();
		Network.SendSetCityAIFocus( pCity:GetID(), focus );
	end
end
Controls.BalancedFocusButton:SetVoid1( CityAIFocusTypes.NO_CITY_AI_FOCUS_TYPE )
Controls.BalancedFocusButton:RegisterCallback( Mouse.eLClick, FocusChanged );

Controls.FoodFocusButton:SetVoid1( CityAIFocusTypes.CITY_AI_FOCUS_TYPE_FOOD )
Controls.FoodFocusButton:RegisterCallback( Mouse.eLClick, FocusChanged );

Controls.ProductionFocusButton:SetVoid1( CityAIFocusTypes.CITY_AI_FOCUS_TYPE_PRODUCTION )
Controls.ProductionFocusButton:RegisterCallback( Mouse.eLClick, FocusChanged );

Controls.GoldFocusButton:SetVoid1( CityAIFocusTypes.CITY_AI_FOCUS_TYPE_GOLD )
Controls.GoldFocusButton:RegisterCallback( Mouse.eLClick, FocusChanged );

Controls.ResearchFocusButton:SetVoid1( CityAIFocusTypes.CITY_AI_FOCUS_TYPE_SCIENCE )
Controls.ResearchFocusButton:RegisterCallback( Mouse.eLClick, FocusChanged );

Controls.CultureFocusButton:SetVoid1( CityAIFocusTypes.CITY_AI_FOCUS_TYPE_CULTURE )
Controls.CultureFocusButton:RegisterCallback( Mouse.eLClick, FocusChanged );

Controls.GPFocusButton:SetVoid1( CityAIFocusTypes.CITY_AI_FOCUS_TYPE_GREAT_PEOPLE )
Controls.GPFocusButton:RegisterCallback( Mouse.eLClick, FocusChanged );

Controls.FaithFocusButton:SetVoid1( CityAIFocusTypes.CITY_AI_FOCUS_TYPE_FAITH );
Controls.FaithFocusButton:RegisterCallback( Mouse.eLClick, FocusChanged );

----------------------------------------------------------------
----------------------------------------------------------------

function OnAvoidGrowth( )
	if Players[Game.GetActivePlayer()]:IsTurnActive() then
		local pCity = UI.GetHeadSelectedCity();
		Network.SendSetCityAvoidGrowth( pCity:GetID(), not pCity:IsForcedAvoidGrowth() );
	end		
end
Controls.AvoidGrowthButton:RegisterCallback( Mouse.eLClick, OnAvoidGrowth );

----------------------------------------------------------------
----------------------------------------------------------------

function OnResetForcedTiles( )
	if Players[Game.GetActivePlayer()]:IsTurnActive() then
		local pCity = UI.GetHeadSelectedCity();
		if pCity ~= nil then
			-- calling this with the city center (0 in the third param) causes it to reset all forced tiles
			Network.SendDoTask(pCity:GetID(), TaskTypes.TASK_CHANGE_WORKING_PLOT, 0, -1, false, bAlt, bShift, bCtrl);
		end
	end
end
Controls.ResetButton:RegisterCallback( Mouse.eLClick, OnResetForcedTiles );

---------------------------------------------------------------------------------------
-- Support for Modded Add-in UI's
---------------------------------------------------------------------------------------
g_uiAddins = {};
for addin in Modding.GetActivatedModEntryPoints("CityViewUIAddin") do
	local addinFile = Modding.GetEvaluatedFilePath(addin.ModID, addin.Version, addin.File);
	local addinPath = addinFile.EvaluatedPath;
	
	-- Get the absolute path and filename without extension.
	local extension = Path.GetExtension(addinPath);
	local path = string.sub(addinPath, 1, #addinPath - #extension);
	
	table.insert(g_uiAddins, ContextPtr:LoadNewContext(path));
end


---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
function OnProductionPopup( bIsHide )
	if OptionsManager.GetSmallUIAssets() then
		Controls.TopLeft:SetHide( not bIsHide );
		Controls.CivIconFrame:SetHide( not bIsHide );
		Controls.ProdQueueBackground:SetHide( not bIsHide );
		Controls.LeftTrim:SetHide( not bIsHide );
    else
		Controls.TopLeft:SetHide( not bIsHide );
		Controls.InfoBG:SetHide( not bIsHide );
		Controls.CityInfo:SetHide( not bIsHide );
		Controls.ProdQueueBackground:SetHide( not bIsHide );
		Controls.LeftTrim:SetHide( not bIsHide );
    end
end
LuaEvents.ProductionPopup.Add( OnProductionPopup );


------------------------------------------------------------
-- Selling Buildings
------------------------------------------------------------
	
function OnBuildingClicked(iBuildingID)

	if (not Players[Game.GetActivePlayer()]:IsTurnActive()) then
		return;
	end

	local pCity = UI.GetHeadSelectedCity();
	
	-- Can this building even be sold?
	if (not pCity:IsBuildingSellable(iBuildingID)) then
		return;
	end
	
	-- Build info string
	local pBuilding = GameInfo.Buildings[iBuildingID];
	
	local iRefund = pCity:GetSellBuildingRefund(iBuildingID);
	local iMaintenance = pBuilding.GoldMaintenance;
	
	local localizedLabel = Locale.ConvertTextKey( "TXT_KEY_SELL_BUILDING_INFO", iRefund, iMaintenance );
	Controls.SellBuildingPopupText:SetText(localizedLabel);
	
	g_iBuildingToSell = iBuildingID;
	
	Controls.SellBuildingConfirm:SetHide(false);
end

function OnYes( )
	Controls.SellBuildingConfirm:SetHide(true);
	if Players[Game.GetActivePlayer()]:IsTurnActive() then
		local pCity = UI.GetHeadSelectedCity();
		Network.SendSellBuilding(pCity:GetID(), g_iBuildingToSell);
	end
	g_iBuildingToSell = -1;
end
Controls.YesButton:RegisterCallback( Mouse.eLClick, OnYes );

function OnNo( )
	Controls.SellBuildingConfirm:SetHide(true);
	g_iBuildingToSell = -1;
end
Controls.NoButton:RegisterCallback( Mouse.eLClick, OnNo );


------------------------------------------------------------
------------------------------------------------------------
local NormalWorldPositionOffset  = WorldPositionOffset;
local NormalWorldPositionOffset2 = WorldPositionOffset2;
local StrategicViewWorldPositionOffset = { x = 0, y = 20, z = 0 };
function OnStrategicViewStateChanged( bStrategicView )
	if bStrategicView then
		WorldPositionOffset  = StrategicViewWorldPositionOffset;
		WorldPositionOffset2 = StrategicViewWorldPositionOffset;
	else
		WorldPositionOffset  = NormalWorldPositionOffset;
		WorldPositionOffset2 = NormalWorldPositionOffset2;
	end
end
Events.StrategicViewStateChanged.Add(OnStrategicViewStateChanged);

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnEventActivePlayerChanged( iActivePlayer, iPrevActivePlayer )
	ClearCityUIInfo();
    if( not ContextPtr:IsHidden() ) then
		Events.SerialEventExitCityScreen();	
	end
end
Events.GameplaySetActivePlayer.Add(OnEventActivePlayerChanged);