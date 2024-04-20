print("This is the 'Squads' mod script.")

-- core
include( "IconSupport" );
include( "InstanceManager" );

-- third party
include( "PlotIterators.lua" );

-- project
include( "SquadsUtil.lua" );
include( "SquadNames.lua" );


local bHighlightSquadUnits = true;
local SquadsEndMovementType = 0;

SQUADS_MODE_NONE = 0;
SQUADS_MODE_MOVE = 1;
SQUADS_MODE_UNIT_MANAGEMENT = 2;

local currentPlot = nil;
local currentSquadNumber = 1;
local currentMode = SQUADS_MODE_NONE;

local bIsBoxSelecting = false;
local pSelectBoxStartPlot = nil;

-- Options Logic
function SquadsOptionChanged(optionKey, newValue)
    if optionKey == "HighlightSquadUnits" then
        bHighlightSquadUnits = newValue;
    elseif optionKey == "SquadsEndMovementType" then
        SquadsEndMovementType = newValue;

        -- In case squad movement is in progress
        local playerID = Game.GetActivePlayer();
        local pActivePlayer = Players[playerID];
        for unit in pActivePlayer:Units() do
            unit:SetSquadEndMovementType(SquadsEndMovementType);
        end
    end
end
LuaEvents.SQUADS_OPTIONS_CHANGED.Add(SquadsOptionChanged);

-- Mode Logic
function SetSquadsMode(mode)
    -- if mode == currentMode then return end
    currentMode = mode;

    local iPlayer = Game.GetActivePlayer();
    local pPlayer = Players[ iPlayer ];

    Events.SerialEventMouseOverHex.Remove(OnMouseMoveOverHex);

    if mode == SQUADS_MODE_NONE then 
        -- Display window again
        UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
        Events.ClearHexHighlights();

    elseif mode == SQUADS_MODE_UNIT_MANAGEMENT then
        if UI.GetInterfaceMode() ~= GameInfo.InterfaceModes.INTERFACEMODE_SQUAD_UNIT_MANAGEMENT.ID then
            UI.SetInterfaceMode(GameInfo.InterfaceModes.INTERFACEMODE_SQUAD_UNIT_MANAGEMENT.ID);
        end
        Events.SerialEventMouseOverHex.Add(OnMouseMoveOverHex);
        UpdateMouse();

    elseif mode == SQUADS_MODE_MOVE then
        if UI.GetInterfaceMode() ~= GameInfo.InterfaceModes.INTERFACEMODE_SQUAD_MOVEMENT.ID then
            UI.SetInterfaceMode(GameInfo.InterfaceModes.INTERFACEMODE_SQUAD_MOVEMENT.ID);
        end
        Events.SerialEventMouseOverHex.Add(OnMouseMoveOverHex);
        UpdateMouse();
    end  
end

function UpdateMouse(gridX, gridY)
    if gridX == nil then
        gridX, gridY = UI.GetMouseOverHex();
    end

    local plot = Map.GetPlot(gridX, gridY) or nil;
    currentPlot = plot;

    if currentMode == SQUADS_MODE_UNIT_MANAGEMENT then
        local iPlayer = Game.GetActivePlayer();
        local pPlayer = Players[ iPlayer ];

        
        -- Highlight all current units in the squad
        HighlightAllSquadStatus(pPlayer)

        -- Default color, overwrite later if unit logic applies
        HighlightPlot(plot, Vector4(0, 0, 1, 1));

        if plot ~= nil then
            local unitCount = currentPlot:GetNumUnits();
            if unitCount > 0 then
                -- Top Unit Only
                local unit = currentPlot:GetUnit(i);

                if unit:GetSquadNumber() == -1 then
                    -- Not currently in a squad (green)
                    HighlightPlot(plot, Vector4(0, 1, 0, 1));
                elseif unit:GetSquadNumber() == currentSquadNumber then
                    -- Removing from current squad (red)
                    HighlightPlot(plot, Vector4(1, 0, 0, 1));
                else
                    -- Is in other squad (yellow)
                    HighlightPlot(plot, Vector4(1, 1, 0, 1));
                end
            end


            if bIsBoxSelecting then
                --print(string.format("Start plot for box select: (%i,%i)", pSelectBoxStartPlot:GetX(), pSelectBoxStartPlot:GetY()))
                --print(string.format("End plot for box select: (%i,%i)", currentPlot:GetX(), currentPlot:GetY()))

                for i = 0, Map.GetNumPlots() - 1, 1 do
                    local pLoopPlot = Map.GetPlotByIndex(i);
                    -- print(string.format("Checking (%i,%i)", pLoopPlot:GetX(), pLoopPlot:GetY()))
                    if IsInBoxSelectArea(pLoopPlot) then
                        -- print("highlighting box select plot")
                        HighlightPlot(pLoopPlot, nil, "SquadsBoxSelect");
                    end
                end
            end
        end

    elseif currentMode == SQUADS_MODE_MOVE then
        local iPlayer = Game.GetActivePlayer();
        local pPlayer = Players[ iPlayer ];

        HighlightSquadUnits(pPlayer, currentSquadNumber);
        Events.ClearHexHighlightStyle("SquadsGroupMovement");
        -- Draw the movement preview shadow (2 rings worth)
        HighlightPlot(currentPlot, nil, "SquadsGroupMovement");
        for pLoopPlot in PlotAreaSpiralIterator(currentPlot, 2) do
            -- TODO: only highlight tiles current squad units can end on
            HighlightPlot(pLoopPlot, nil, "SquadsGroupMovement");
        end
    end
end

function OnMouseMoveOverHex(hexX, hexY)
    UpdateMouse(nil, hexX, hexY);
end

-- ========== Squads Panel Handlers ==========
function HandlePressAddToSquadButton ()
  print("add to squad button pressed");
  SetSquadsMode(SQUADS_MODE_UNIT_MANAGEMENT);
  OnClose();
end
Controls.AddToSquadButton:RegisterCallback(Mouse.eLClick, HandlePressAddToSquadButton);

function HandlePressMoveSquadButton ()
  SetSquadsMode(SQUADS_MODE_MOVE);
  OnClose();
end
Controls.MoveSquadButton:RegisterCallback(Mouse.eLClick, HandlePressMoveSquadButton);

function HandleResetSquadButton ()
    print("handle reset squad button clicked");
    local iPlayer = Game.GetActivePlayer();
    local pPlayer = Players[ iPlayer ];

    for unit in getSquadUnits(pPlayer, currentSquadNumber) do
        RemoveFromSquadWithEvent(unit);
    end

    BuildUnitList();
end
Controls.ResetSquadButton:RegisterCallback(Mouse.eLClick, HandleResetSquadButton);


function IsInBoxSelectArea(pPlotToCheck)
    if pSelectBoxStartPlot == nil or currentPlot == nil then
        return false
    end

    -- Box drawn from bottom left to top right
    if currentPlot:GetX() >= pSelectBoxStartPlot:GetX() and currentPlot:GetY() >= pSelectBoxStartPlot:GetY() then
        return pPlotToCheck:GetX() >= pSelectBoxStartPlot:GetX() and pPlotToCheck:GetX() <= currentPlot:GetX() and 
               pPlotToCheck:GetY() >= pSelectBoxStartPlot:GetY() and pPlotToCheck:GetY() <= currentPlot:GetY();
    -- Box drawn from top left to bottom right
    elseif currentPlot:GetX() >= pSelectBoxStartPlot:GetX() and currentPlot:GetY() <= pSelectBoxStartPlot:GetY() then
        return pPlotToCheck:GetX() >= pSelectBoxStartPlot:GetX() and pPlotToCheck:GetX() <= currentPlot:GetX() and 
               pPlotToCheck:GetY() <= pSelectBoxStartPlot:GetY() and pPlotToCheck:GetY() >= currentPlot:GetY();
    -- Box drawn from top right to bottom left
    elseif currentPlot:GetX() <= pSelectBoxStartPlot:GetX() and currentPlot:GetY() <= pSelectBoxStartPlot:GetY() then
        return pPlotToCheck:GetX() <= pSelectBoxStartPlot:GetX() and pPlotToCheck:GetX() >= currentPlot:GetX() and 
               pPlotToCheck:GetY() <= pSelectBoxStartPlot:GetY() and pPlotToCheck:GetY() >= currentPlot:GetY();
    -- Box drawn from bottom right to top left
    elseif currentPlot:GetX() <= pSelectBoxStartPlot:GetX() and currentPlot:GetY() >= pSelectBoxStartPlot:GetY() then
        return pPlotToCheck:GetX() <= pSelectBoxStartPlot:GetX() and pPlotToCheck:GetX() >= currentPlot:GetX() and 
               pPlotToCheck:GetY() >= pSelectBoxStartPlot:GetY() and pPlotToCheck:GetY() <= currentPlot:GetY();
    end
end


function AssignToSquadWithEvent(pUnit, squadNumber)
    pUnit:AssignToSquad(currentSquadNumber);
    LuaEvents.OnSquadChangeEvent(Game.GetActivePlayer(), pUnit:GetID());
end

function RemoveFromSquadWithEvent(pUnit)
    pUnit:RemoveFromSquad();
    LuaEvents.OnSquadChangeEvent(Game.GetActivePlayer(), pUnit:GetID());
end




-- TODO: move this somewhere organized
function DoBoxSelect()
    if not bIsBoxSelecting then return end
    bIsBoxSelecting = false;
    Events.ClearHexHighlightStyle("SquadsBoxSelect");

    bUnitsSelected = false;
    for i = 0, Map.GetNumPlots() - 1, 1 do
        local pLoopPlot = Map.GetPlotByIndex(i);
        if IsInBoxSelectArea(pLoopPlot) then
           
            local unitCount = pLoopPlot:GetNumUnits();
            for i = 0, unitCount do
                local pUnit = pLoopPlot:GetUnit(i);

                if pUnit ~= nil and pUnit:GetUnitClassType() ~= GameInfoTypes["UNITCLASS_WORKER"] and pUnit:GetUnitClassType() ~= GameInfoTypes["UNITCLASS_WORKBOAT"] then
                    AssignToSquadWithEvent(pUnit, currentSquadNumber);
                    bUnitsSelected = true;
                end
            end
        end
    end
    return bUnitsSelected;
end

-- ========== Escape Handlers
function HandleKeyDown(wParam)
    if wParam == Keys.VK_ESCAPE then
        SetSquadsMode(SQUADS_MODE_NONE);
    end
    
end
LuaEvents.SQUADS_ANY_MODE_KEY_DOWN.Add(HandleKeyDown);

-- ========== Manage Mode Handlers ==========
function HandleLeftClickUp(wParam, lParam)
    if currentPlot ~= nil then
        local unitCount = currentPlot:GetNumUnits();
        if unitCount == 1 then
            local unit = currentPlot:GetUnit(0);

            if unit:GetSquadNumber() == currentSquadNumber then
                RemoveFromSquadWithEvent(unit);
            else
                AssignToSquadWithEvent(unit, currentSquadNumber);
            end
        else
            for i = 0, unitCount do
                local pUnit = currentPlot:GetUnit(i);
                if pUnit ~= nil then
                    AssignToSquadWithEvent(pUnit, currentSquadNumber);
                end
            end
        end
    end
    
end
LuaEvents.SQUADS_MANAGE_UNITS_MODE_LEFT_CLICK_UP.Add(HandleLeftClickUp);

function HandleRightClickDown(wParam, lParam)
    bIsBoxSelecting = true;
    pSelectBoxStartPlot = currentPlot;
end

LuaEvents.SQUADS_MANAGE_UNITS_MODE_RIGHT_CLICK_DOWN.Add(HandleRightClickDown);


function HandleRightClickUp(wParam, lParam)
    bUnitsSelected = DoBoxSelect()
    
    if bUnitsSelected then
        SetSquadsMode(SQUADS_MODE_UNIT_MANAGEMENT)
    else
        SetSquadsMode(SQUADS_MODE_NONE);
    end
end
LuaEvents.SQUADS_MANAGE_UNITS_MODE_RIGHT_CLICK_UP.Add(HandleRightClickUp);


-- ========== Hotkey Handler ===========

local lastHotkeyTimeLut = {}
for i=1,10 do
    lastHotkeyTimeLut[i] = 0;
end

function HandleNumericHotkey(number, bCtrl)
    if number == 0 then
        number = 10
    end

    local lastPressTime = lastHotkeyTimeLut[number];
    lastHotkeyTimeLut[number] = os.clock();
    if bCtrl then
        lastHotkeyTimeLut[number] = 0;
        currentSquadNumber = number;
        SetSquadsMode(SQUADS_MODE_UNIT_MANAGEMENT);
    -- Double press
    elseif lastHotkeyTimeLut[number] - lastPressTime < 0.5 then
        lastHotkeyTimeLut[number] = 0;
        HandleFocusSquad(number);
    end
    

end
LuaEvents.SQUADS_NUMERIC_HOTKEY.Add(HandleNumericHotkey);

function HandleFocusSquad(number)
    local iPlayer = Game.GetActivePlayer();
    local pPlayer = Players[ iPlayer ];
    currentSquadNumber = number;

    for unit in getSquadUnits(pPlayer, currentSquadNumber) do
        -- Act on first unit in squad or do nothing if none exist
        Events.SerialEventUnitFlagSelected( iPlayer, unit:GetID() );
        SetSquadsMode(SQUADS_MODE_MOVE);
        break;
    end
end

-- ========== Move Mode Handlers ==========
function HandleMoveSquad(wParam, lParam)
    if currentPlot ~= nil then
        local iPlayer = Game.GetActivePlayer();
        local pPlayer = Players[ iPlayer ];

        local bMovedSquad = false;
        for unit in getSquadUnits(pPlayer, currentSquadNumber) do
            unit:SetSquadEndMovementType(SquadsEndMovementType);
            unit:DoSquadMovement(currentPlot);
            bMovedSquad = true;
            break;
        end

        if bMovedSquad == false then print("Squad is empty, nothing to move") end
    end

    SetSquadsMode(SQUADS_MODE_NONE);
end
LuaEvents.SQUADS_MOVE_MODE_RIGHT_CLICK.Add(HandleMoveSquad);

function HandleCancelMoveSquad(wParam, lParam)
    SetSquadsMode(SQUADS_MODE_NONE);
end
LuaEvents.SQUADS_MOVE_MODE_LEFT_CLICK.Add(HandleCancelMoveSquad);


-- ================ Highlight Logic =================
function HighlightPlot(plot, color, style)
    if plot then
        Events.SerialEventHexHighlight(ToHexFromGrid(Vector2(plot:GetX(), plot:GetY())), true, color, style);
    end
end


function OnUnitSelectionChange(iPlayerID, iUnitID, i, j, k, isSelected)
    if currentMode ~= SQUADS_MODE_NONE then

        if currentMode == SQUADS_MODE_UNIT_MANAGEMENT then
            local pPlayer = Players[iPlayerID];
            local pUnit = pPlayer:GetUnitByID(iUnitID);
            AssignToSquadWithEvent(pUnit, currentSquadNumber);
        end

        -- workaround for when flag is clicked, need to put the correct
        -- UI.SetInterfaceMode back into place
        SetSquadsMode(currentMode);
        return 
    end

    -- Events.ClearHexHighlights();
    if (isSelected) then
        local pPlayer = Players[iPlayerID];
        local pUnit = pPlayer:GetUnitByID(iUnitID);

        local bSquadNumChanged = (pUnit:GetSquadNumber() ~= currentSquadNumber);

        if (pUnit:GetSquadNumber() > -1) then
            _highlightSquadUnits(pPlayer, pUnit:GetSquadNumber(), bSquadNumChanged);
        end

        if pUnit:GetSquadNumber() > 0 then
            currentSquadNumber = pUnit:GetSquadNumber();
        end

        
    end
end
Events.UnitSelectionChanged.Add( OnUnitSelectionChange );


function HighlightSquadUnits(pPlayer, squadNumber)
    _highlightSquadUnits(pPlayer, squadNumber, false);
end

function _highlightSquadUnits(pPlayer, squadNumber, clearOldHighlights)
    if not bHighlightSquadUnits and currentMode ~= SQUADS_MODE_UNIT_MANAGEMENT then return end
    if clearOldHighlights then Events.ClearHexHighlights() end
    if squadNumber == -1 then return end
    for unit in getSquadUnits(pPlayer, squadNumber)
    do
        Events.SerialEventHexHighlight(ToHexFromGrid(Vector2(unit:GetX(), unit:GetY())), true, Vector4(1, 1, 1, 1));
    end
end

function HighlightAllSquadStatus(pPlayer)
    Events.ClearHexHighlights();
    for unit in pPlayer:Units() do
        if unit:GetSquadNumber() == -1 then
            -- do nothing
        elseif unit:GetSquadNumber() == currentSquadNumber then
            -- White if already in the current squad
            Events.SerialEventHexHighlight(ToHexFromGrid(Vector2(unit:GetX(), unit:GetY())), true, Vector4(1, 1, 1, 1));
        else
            -- Orange if already in a squad
            Events.SerialEventHexHighlight(ToHexFromGrid(Vector2(unit:GetX(), unit:GetY())), true, Vector4(1, 0.65, 0, 1));
        end
    end
end

-- ================ Util ================


function getSquadUnits(activePlayer, squadNumber)
    return activePlayer:UnitsInSquad(squadNumber);
end




-- ================ Squads Panel Management ================
local m_SquadsIM = InstanceManager:new( "SquadInstance", "Root", Controls.SquadStack );

function BuildSquadsList()
    local iPlayer = Game.GetActivePlayer();
    local pPlayer = Players[ iPlayer ];

    m_SquadsIM:ResetInstances();

    for i=1,10 do
        instance = m_SquadsIM:GetInstance();

        instance.Button:RegisterCallback( Mouse.eLClick, SquadEntryClicked );
        instance.Button:SetVoid1( i );

        instance.SquadName:SetText( GetSquadName(i) );

        instance.SelectionFrame:SetHide( not (currentSquadNumber == i) );
    end

    Controls.SquadNameInput:SetText( GetSquadName(currentSquadNumber) );
end


function SquadEntryClicked(squadNumber)
    currentSquadNumber = squadNumber;

    BuildSquadsList();
    BuildUnitList();
end


function SquadRenamed(newName)
    SetSquadName(currentSquadNumber, newName);
    BuildSquadsList();
end


function Validate(sValue, control, bFire)
    local bValid = false;

    if ValidateText(sValue) then
        bValid = true;
    end

    if bValid then
        print(Controls.SquadNameInputBorder)
        Controls.SquadNameInputBorder:SetColor({x=0.96,y=0.96,z=0.86,w=1})
        SquadRenamed(sValue);
    else
        Controls.SquadNameInputBorder:SetColor({x=1,y=0,z=0,w=1})
    end
end
Controls.SquadNameInput:RegisterCallback(Validate);


-- from military overview

local m_MilitaryIM = InstanceManager:new( "UnitInstance", "Root", Controls.MilitaryStack );
local m_CivilianIM = InstanceManager:new( "UnitInstance", "Root", Controls.CivilianStack );

local m_SortTable;
local eName     = 0;
local eStatus   = 1;
local eMovement = 2;
local eMoves    = 3;
local eStrength = 4;
local eRanged   = 5;
-- Infixo: ShowXPinMO/added
local eUnitXP   = 6;

local m_SortMode = eName;
local m_bSortReverse = false;

local m_PopupInfo = nil;
--ak added for healthbar calcs
local g_MaxDamage = GameDefines.MAX_HIT_POINTS or 100
-- Infixo
m_tGreatPeopleIcons = {
["UNIT_GREAT_ADMIRAL"] = "[ICON_GREAT_ADMIRAL]", 
["UNIT_GREAT_GENERAL"] = "[ICON_GREAT_GENERAL]", 
["UNIT_PROPHET"] = "[ICON_PROPHET]",
["UNIT_ARTIST"] = "[ICON_GREAT_ARTIST]", 
["UNIT_MUSICIAN"] = "[ICON_GREAT_MUSICIAN]",
["UNIT_WRITER"] = "[ICON_GREAT_WRITER]",
["UNIT_SCIENTIST"] = "[ICON_GREAT_SCIENTIST]", 
["UNIT_ENGINEER"] = "[ICON_GREAT_ENGINEER]", 
["UNIT_MERCHANT"] = "[ICON_GREAT_MERCHANT]", 
["UNIT_VENETIAN_MERCHANT"] = "[ICON_GREAT_MERCHANT_VENICE]", 
["UNIT_EXPLORER"] = "[ICON_GREAT_EXPLORER]", 
["UNIT_GREAT_DIPLOMAT"] = "[ICON_DIPLOMAT]" };

-------------------------------------------------
-- On Popup
-------------------------------------------------
-- function OnPopup( popupInfo )
-- 	if( popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_MILITARY_OVERVIEW ) then
--     	m_PopupInfo = popupInfo;
    	
-- 		if( m_PopupInfo.Data1 == 1 ) then
--         	if( ContextPtr:IsHidden() == false ) then
--         	    OnClose();
--             else
--             	UIManager:QueuePopup( ContextPtr, PopupPriority.InGameUtmost );
--         	end
--     	else
--         	UIManager:QueuePopup( ContextPtr, PopupPriority.MilitaryOverview );
--     	end
-- 	end
-- end
-- Events.SerialEventGameMessagePopup.Add( OnPopup );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------



-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function UnitClicked(unitID)
    local pSelectedUnit = UI:GetHeadSelectedUnit();
    if pSelectedUnit == nil then return end

    if UIManager:GetAlt() then
        RemoveUnitFromSquadList(unitID);
        return;
    end

    if pSelectedUnit:GetID() == unitID then
        UI.LookAtSelectionPlot(0);
    else
	    Events.SerialEventUnitFlagSelected( Game:GetActivePlayer(), unitID );
	    --ak update highlighted unit in UI when clicked
	    BuildUnitList(unitID);
    end
end
--ak have view follow unit selection
Events.SerialEventUnitFlagSelected.Add( function() UI.LookAtSelectionPlot(0) end)

function RemoveUnitFromSquadList(unitID)
    local iPlayer = Game.GetActivePlayer();
    local pPlayer = Players[ iPlayer ];
    local pUnit = pPlayer:GetUnitByID(unitID);
    if pUnit == nil then return end

    RemoveFromSquadWithEvent(pUnit);
    BuildUnitList();
end
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function UpdateScreen()

    local iPlayer = Game.GetActivePlayer();
    local pPlayer = Players[ iPlayer ];

    BuildSquadsList();
    -- Unit List

    --ak moved currently selected unit code out of BuildUnitList
    --ak so that we can pass it as a param
    local pSelectedUnit = UI:GetHeadSelectedUnit();
    local iSelectedUnit = -1;
    if( pSelectedUnit ~= nil ) then
        iSelectedUnit = pSelectedUnit:GetID();
    end
    
    BuildUnitList(iSelectedUnit);
end


--------------------------------------------------------
--------------------------------------------------------
function BuildUnitList(iSelectedUnit)

    m_SortTable = {};
    
    local iPlayer = Game.GetActivePlayer();
    local pPlayer = Players[ iPlayer ];
    
    local bFoundMilitary = false;
    local bFoundCivilian = false;
    
    
    m_MilitaryIM:ResetInstances();
    m_CivilianIM:ResetInstances();
    
    for unit in getSquadUnits(pPlayer, currentSquadNumber)
    do
        local instance;
        local iUnit = unit:GetID();
        
        if( unit:GetUnitCombatType() ~= -1 or unit:CanNuke()) then
            instance = m_MilitaryIM:GetInstance();
            bFoundMilitary = true;
        else
            instance = m_CivilianIM:GetInstance();
            bFoundCivilian = true;
        end
        
        local sortEntry = {};
        m_SortTable[ tostring( instance.Root ) ] = sortEntry;
        
        instance.Button:RegisterCallback( Mouse.eLClick, UnitClicked );
        instance.Button:RegisterCallback( Mouse.eRClick, RemoveUnitFromSquadList );
        instance.Button:SetVoid1( unit:GetID() );
        
		-- Infixo: ShowXPinMO/get unit's Name or Type
		-- name's returned as "real_name (type)" so it has to be truncated
		local unittype = Locale.Lookup(unit:GetNameKey());
		local unitname = unit:GetName();
		if unitname ~= unittype then -- unique name
			unitname = string.sub(unitname,1,#unitname - #unittype - 3);
		end
		sortEntry.DisplayName = unitname;
		instance.UnitName:SetText( unitname );
		-- religious units
		--if unit:GetReligion() ~= -1 then -- Infixo: Merchant can have a religion!
		if GameInfo.Units[unit:GetUnitType()].ReligiousStrength > 0 then
			if unit:GetSpreadsLeft() > 0 then -- Great Prophet and Missionary
				instance.UnitName:SetText( unitname.." "..GameInfo.Religions[unit:GetReligion()].IconString..tostring(unit:GetSpreadsLeft()));
			else -- Inquisitor
				instance.UnitName:SetText( unitname.." "..GameInfo.Religions[unit:GetReligion()].IconString);
			end
		-- great persons
		elseif unit:IsGreatPerson() then 
			local sUnitType = GameInfo.Units[unit:GetUnitType()].Type;
			instance.UnitName:SetText( unitname.." "..(m_tGreatPeopleIcons[sUnitType] or "(?)") );
		end
		-- Infixo: ShowXPinMO/end

		--ak added unitType as tooltip, useful for named units
		instance.UnitName:SetToolTipString(unittype);

		--ak begin healthbar calcs
		instance.HealthBar:SetAlpha (0.7);
		if unit:GetDamage() == 0 then
			instance.HealthBar:SetHide(true)
		else
			local healthPercent = unit:GetCurrHitPoints() / unit:GetMaxHitPoints();
			local healthTimes100 =  math.floor(100 * healthPercent + 0.5)
			local barSize = { y = 3, x = math.floor(138 * healthPercent) }
			-- green
			instance.GreenBar:SetSize(barSize)
			instance.GreenBar:SetHide(healthTimes100<=70);
			-- yellow
			instance.YellowBar:SetSize(barSize)
			instance.YellowBar:SetHide(healthTimes100<=33 or healthTimes100>70);
			-- red
			instance.RedBar:SetSize(barSize)
			instance.RedAnim:SetSize(barSize)
			instance.RedBar:SetHide(healthTimes100>33);
			-- show bar
			instance.HealthBar:SetHide(false)
		end
		--ak end healthbar calcs

        if( unit:MovesLeft() > 0 ) then
            instance.Button:SetAlpha( 1.0 );
        else
            instance.Button:SetAlpha( 0.6 );
        end
        
        instance.SelectionFrame:SetHide( not (iSelectedUnit == iUnit) );
       
		--ak fixed promotion available check (unit:promote always seems to return false in VP)
		--sortEntry.hasPromotion = unit:CanPromote(); 
		sortEntry.hasPromotion = unit:GetExperience() >= unit:ExperienceNeeded() 
        instance.PromotionIndicator:SetHide( not sortEntry.hasPromotion );

        ---------------------------------------------------------
        -- Status field
        local buildType = unit:GetBuildType();
        local activityType = unit:GetActivityType();
        if( unit:IsEmbarked() ) then
            sortEntry.status = "TXT_KEY_UNIT_STATUS_EMBARKED";
            --instance.Status:SetHide( false ); -- Infixo
                        
        elseif( unit:IsGarrisoned()) then
			sortEntry.status = Locale.Lookup("TXT_KEY_MISSION_GARRISON").." "..unit:GetGarrisonedCity():GetName();
            --instance.Status:SetHide( false ); -- Infixo
			
        elseif( buildType ~= -1) then -- this is a worker who is actively building something
			--[[ Infixo
    		local thisBuild = GameInfo.Builds[buildType];
    		local civilianUnitStr = Locale.ConvertTextKey(thisBuild.Description);
    		local iTurnsLeft = unit:GetPlot():GetBuildTurnsLeft(buildType,Game.GetActivePlayer(), 0, 0);	
    		local iTurnsTotal = unit:GetPlot():GetBuildTurnsTotal(buildType);
    		if (iTurnsLeft < 4000 and iTurnsLeft > 0) then
				civilianUnitStr = civilianUnitStr.." ("..tostring(iTurnsLeft)..")";
    		end
            sortEntry.status = civilianUnitStr;
            instance.Status:SetHide( false );
			-- Infixo ]]
			sortEntry.status = Locale.Lookup(GameInfo.Builds[buildType].Description).." ("..tostring( unit:GetPlot():GetBuildTurnsLeft(buildType,Game.GetActivePlayer(), 0, 0) )..")";
            
        elseif( unit:IsAutomated()) then
			if(unit:IsWork()) then
				sortEntry.status = "TXT_KEY_ACTION_AUTOMATE_BUILD";
			elseif unit:IsTrade() then
				sortEntry.status = "TXT_KEY_ACTION_AUTOMATE_TRADE";
			else
				sortEntry.status = "TXT_KEY_ACTION_AUTOMATE_EXPLORE";
			end
			--instance.Status:SetHide( false ); -- Infixo
				
		elseif( activityType == ActivityTypes.ACTIVITY_HEAL ) then
			sortEntry.status = "TXT_KEY_MISSION_HEAL";
			--instance.Status:SetHide( false ); -- Infixo
			
		elseif( activityType == ActivityTypes.ACTIVITY_SENTRY ) then
			sortEntry.status = "TXT_KEY_MISSION_ALERT";
			--instance.Status:SetHide( false ); -- Infixo
			if unit:FortifyModifier() > 0 then sortEntry.status = Locale.Lookup("TXT_KEY_MISSION_ALERT").." +"..tostring(unit:FortifyModifier()).."%[ICON_STRENGTH]" end
			
        elseif( unit:GetFortifyTurns() > 0 ) then
            sortEntry.status = "TXT_KEY_UNIT_STATUS_FORTIFIED";
            --instance.Status:SetHide( false ); -- Infixo
			if unit:FortifyModifier() > 0 then sortEntry.status = Locale.Lookup("TXT_KEY_UNIT_STATUS_FORTIFIED").." +"..tostring(unit:FortifyModifier()).."%[ICON_STRENGTH]" end
            
        elseif( activityType == ActivityTypes.ACTIVITY_SLEEP ) then
			sortEntry.status = "TXT_KEY_MISSION_SLEEP";
			--instance.Status:SetHide( false ); -- Infixo
		-- Infixo
		elseif activityType == ActivityTypes.ACTIVITY_INTERCEPT then
			status = "TXT_KEY_MISSION_INTERCEPT_HELP";
		elseif activityType == ActivityTypes.ACTIVITY_HOLD then
			status = "TXT_KEY_MISSION_SKIP_HELP";
		elseif activityType == ActivityTypes.ACTIVITY_MISSION then
			status = "TXT_KEY_INTERFACEMODE_MOVE_TO_HELP";
        -- Infixo    
    	else
            sortEntry.status = nil;
            --instance.Status:SetHide( true ); -- Infixo
        end
        if( sortEntry.status ~= nil ) then
            instance.Status:LocalizeAndSetText( sortEntry.status );
			instance.Status:SetHide( false );
        else
            instance.Status:SetText( "" );
            instance.Status:SetHide( true );
        end
        
		-- Infixo: ShowXPinMO/start
		local unitxp = unit:GetExperience();
		local unitlevel = unit:GetLevel();
		if bFoundMilitary then
			sortEntry.unitxplev = unitxp;
			instance.UnitXP:SetText( unitxp .. "/" .. unitlevel );
		elseif bFoundCivilian then
			sortEntry.unitxplev = 0;
			instance.UnitXP:SetText("-");
		else
			sortEntry.unitxplev = 0;
			instance.UnitXP:SetText("error");
		end
		-- Infixo: ShowXPinMO/end

	    local move_denominator = GameDefines["MOVE_DENOMINATOR"];
	    local moves_left = math.floor(unit:MovesLeft() / move_denominator);
	    local max_moves = math.floor(unit:MaxMoves() / move_denominator);
	    
        if( moves_left == max_moves ) then
            instance.MovementPip:SetTextureOffsetVal( 0, 0 );
        elseif( moves_left == 0 ) then
            instance.MovementPip:SetTextureOffsetVal( 0, 96 );
        else
            instance.MovementPip:SetTextureOffsetVal( 0, 32 );
        end
	    
        sortEntry.movement = moves_left;
        sortEntry.moves = max_moves;
        instance.Movement:SetText( moves_left .. "/" .. max_moves );
        
        sortEntry.strength = unit:GetBaseCombatStrength();
        if( sortEntry.strength == 0 ) then
            instance.Strength:SetText( "-" );
        else
            instance.Strength:SetText( sortEntry.strength );
        end
        
        sortEntry.ranged = unit:GetBaseRangedCombatStrength();
        if( sortEntry.ranged == 0 ) then
            instance.RangedAttack:SetText( "-" );
        else
            instance.RangedAttack:SetText( sortEntry.ranged );
        end
        
        sortEntry.unit = unit;
    end

    if( bFoundMilitary and bFoundCivilian ) then
        Controls.CivilianSeperator:SetHide( false );
    else
        Controls.CivilianSeperator:SetHide( true );
    end
    
    Controls.MilitaryStack:CalculateSize();
    Controls.CivilianStack:CalculateSize();
    
    Controls.MainStack:CalculateSize();
    Controls.ScrollPanel:CalculateInternalSize();
    
    Controls.ScrollPanel:ReprocessAnchoring();
end


-------------------------------------------------
-------------------------------------------------
function SortFunction( a, b )
    local valueA, valueB;
    local entryA = m_SortTable[ tostring( a ) ];
    local entryB = m_SortTable[ tostring( b ) ];

    if (entryA == nil) or (entryB == nil) then 
		if entryA and (entryB == nil) then
			return false;
		elseif (entryA == nil) and entryB then
			return true;
		else
			if( m_bSortReverse ) then
				return tostring(a) > tostring(b); -- gotta do something deterministic
			else
				return tostring(a) < tostring(b); -- gotta do something deterministic
			end
        end;
    else
		if( m_SortMode == eName ) then
			valueA = entryA.DisplayName;
			valueB = entryB.DisplayName;
		elseif( m_SortMode == eStatus ) then
			valueA = entryA.status or "";
			valueB = entryB.status or "";
			
			if(valueA ~= valueB) then
				if(m_bSortReverse) then
					return Locale.Compare(valueA, valueB) == -1;
				else
					return Locale.Compare(valueA, valueB) == 1;
				end
			end
			
		elseif( m_SortMode == eMovement ) then
			valueA = entryA.movement;
			valueB = entryB.movement;
		elseif( m_SortMode == eMoves ) then
			valueA = entryA.moves;
			valueB = entryB.moves;
		elseif( m_SortMode == eStrength ) then
			valueA = entryA.strength;
			valueB = entryB.strength;
		elseif( m_SortMode == eRanged ) then
			valueA = entryA.ranged;
			valueB = entryB.ranged;
		-- Infixo: ShowXPinMO/start
		elseif( m_SortMode == eUnitXP ) then
			valueA = entryA.unitxplev;
			valueB = entryB.unitxplev;
		-- Infixo: ShowXPinMO/end
		end
	    
		if( valueA == valueB ) then
			valueA = entryA.unit:GetID();
			valueB = entryB.unit:GetID();
		end
	   
		if( m_bSortReverse ) then
			return valueA > valueB;
		else
			return valueA < valueB;
		end
	end
end


-------------------------------------------------
-------------------------------------------------
function OnSort( type )
    if( m_SortMode == type ) then
        m_bSortReverse = not m_bSortReverse;
    else
        m_bSortReverse = false;
    end
    
    m_SortMode = type;
    Controls.MilitaryStack:SortChildren( SortFunction );
    Controls.CivilianStack:SortChildren( SortFunction );
end
Controls.SortName:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortStatus:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortMovement:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortMoves:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortStrength:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortRanged:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortName:SetVoid1( eName );
Controls.SortStatus:SetVoid1( eStatus );
Controls.SortMovement:SetVoid1( eMovement );
Controls.SortMoves:SetVoid1( eMoves );
Controls.SortStrength:SetVoid1( eStrength );
Controls.SortRanged:SetVoid1( eRanged );
-- ShowXPinMO/start
Controls.SortXP:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortXP:SetVoid1( eUnitXP );
-- ShowXPinMO/end

        
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )

	-- Set player icon at top of screen
	CivIconHookup( 0, 64, Controls.Icon, Controls.CivIconBG, Controls.CivIconShadow, false, true );
    UpdateScreen();

    if( not bInitState ) then
        if( not bIsHide ) then
			m_bSortReverse = not m_bSortReverse;
        	OnSort( m_SortMode );
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

ContextPtr:SetHide(true);


function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
            OnClose();
            return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );

function OnClose ()
  ContextPtr:SetHide(true)
end
Controls.CloseButton:RegisterCallback(Mouse.eLClick, OnClose)

function Open()
    ContextPtr:SetHide(false)
    SetSquadsMode(SQUADS_MODE_NONE);
end

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
-- Boilerplate
function OnDiploCornerPopup()
    Open();
  -- ContextPtr:SetHide(false)
end

function OnAdditionalInformationDropdownGatherEntries(additionalEntries)
  table.insert(additionalEntries, {text=Locale.ConvertTextKey("TXT_KEY_SQUADS_TITLE"), call=OnDiploCornerPopup, art="SquadsIcon.dds"})
end

LuaEvents.AdditionalInformationDropdownGatherEntries.Add(OnAdditionalInformationDropdownGatherEntries)
LuaEvents.RequestRefreshAdditionalInformationDropdownEntries()

Events.GameplaySetActivePlayer.Add(OnClose)

-- Load last so listeners in this file are ready
include( "SquadsOptions.lua" );
print("Loaded Squads.lua from 'Squads for VP'")