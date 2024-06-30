print("Loading MilitaryOverview.lua from 'VP-UI - Show XP in Military Overview'")
-------------------------------------------------
-- Military
-- v1.0, Feb 1, 2017: modified by Infixo for Show XP in Military Overview mod
-- v2.0, Feb 4, 2017: added detection of Community Patch - should be compatible with VP and BNW
-- v2.1, Mar 2, 2017: changes due to VP 2/27, corrected "Civlian"
-- v2.2, Dec 26, 2017: Implemented tweaks from ak, religious units and icons for GP
-- v2.3, Jan 14, 2018: Compatibility with VP 1-14 (obsolete Lua function changed into a new one)
-- v2.4 Oct 30, 2022: Integrated into VP, removed redundant IsUsingCP checks
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );

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
function OnPopup( popupInfo )
	if( popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_MILITARY_OVERVIEW ) then
    	m_PopupInfo = popupInfo;
    	
		if( m_PopupInfo.Data1 == 1 ) then
        	if( ContextPtr:IsHidden() == false ) then
        	    OnClose();
            else
            	UIManager:QueuePopup( ContextPtr, PopupPriority.InGameUtmost );
        	end
    	else
        	UIManager:QueuePopup( ContextPtr, PopupPriority.MilitaryOverview );
    	end
	end
end
Events.SerialEventGameMessagePopup.Add( OnPopup );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnClose()
    UIManager:DequeuePopup( ContextPtr );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose);


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
            OnClose();
            return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function UnitClicked(unitID)
    local pSelectedUnit = UI:GetHeadSelectedUnit();
    if( pSelectedUnit ~= nil and
        pSelectedUnit:GetID() == unitID ) then
        UI.LookAtSelectionPlot(0);
    else
	    Events.SerialEventUnitFlagSelected( Game:GetActivePlayer(), unitID );
	    --ak update highlighted unit in UI when clicked
	    BuildUnitList(unitID);
    end
end
--ak have view follow unit selection
Events.SerialEventUnitFlagSelected.Add( function() UI.LookAtSelectionPlot(0) end)


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function UpdateScreen()

    local iPlayer = Game.GetActivePlayer();
    local pPlayer = Players[ iPlayer ];
    
    --------------------------------------------------------
    -- Great General Progress
    local fThreshold = pPlayer:GreatGeneralThreshold();
    local fProgress  = pPlayer:GetCombatExperience();
    Controls.GPMeter:SetPercent( fProgress / fThreshold );
    Controls.GPBox:LocalizeAndSetToolTip( "TXT_KEY_MO_GENERAL_TT", fProgress, fThreshold );
    
    --------------------------------------------------------
    -- Great Admiral Progress
    fThreshold = pPlayer:GreatAdmiralThreshold();
    fProgress  = pPlayer:GetNavalCombatExperience();
    Controls.GAMeter:SetPercent( fProgress / fThreshold );
    Controls.GABox:LocalizeAndSetToolTip( "TXT_KEY_MO_ADMIRAL_TT", fProgress, fThreshold );
    
	--------------------------------------------------------
	-- Supply Details
	local iUnitSupplyMod = pPlayer:GetUnitProductionMaintenanceMod();
	local iUnitsSupplied = pPlayer:GetNumUnitsSupplied();
	local iUnitsTotal = pPlayer:GetNumUnitsToSupply();
	local iUnitsTotalMilitary = pPlayer:GetNumMilitaryUnits();
	local iPercentPerPop = pPlayer:GetNumUnitsSuppliedByPopulation();
	local iPerCity = pPlayer:GetNumUnitsSuppliedByCities();
	local iPerHandicap = pPlayer:GetNumUnitsSuppliedByHandicap();
	local iUnitsOver = pPlayer:GetNumUnitsOutOfSupply();
	local iTechReduction = pPlayer:GetTechSupplyReduction();
	local iEmpireSizeReduction = pPlayer:GetEmpireSizeSupplyReduction();
	local iWarWearinessReduction = pPlayer:GetSupplyReductionFromWarWeariness();
	local iSupplyFromGreatPeople = pPlayer:GetUnitSupplyFromExpendedGreatPeople();

	-- Bonuses from unlisted sources are added to the handicap value
	local iExtra = iUnitsSupplied - (iPerHandicap + iPerCity + iPercentPerPop + iSupplyFromGreatPeople - iTechReduction - iEmpireSizeReduction - iWarWearinessReduction);
	iPerHandicap = iPerHandicap + iExtra;

	Controls.HandicapSupplyValue:SetText(iPerHandicap);
	Controls.CitiesSupplyValue:SetText(iPerCity);
	Controls.PopulationSupplyValue:SetText(iPercentPerPop);
	Controls.GreatPeopleSupplyValue:SetText(iSupplyFromGreatPeople);
	Controls.WarWearinessReductionValue:SetText(-iWarWearinessReduction);
	Controls.TechReductionValue:SetText(-iTechReduction);
	Controls.EmpireSizeReductionValue:SetText(-iEmpireSizeReduction);
	Controls.SupplyCapValue:SetText(iUnitsSupplied);
	Controls.SupplyUseValue:SetText(iUnitsTotal);

	if (iUnitsOver ~= 0) then
		Controls.SupplyDeficit:SetHide(false);
		Controls.DeficitPenalty:SetHide(false);
		Controls.SupplyRemaining:SetHide(true);
		Controls.SupplyDeficitValue:SetText(iUnitsOver);
		Controls.SupplyDeficitPenaltyValue:SetText(iUnitSupplyMod .. "%");
	else
		Controls.SupplyDeficit:SetHide(true);
		Controls.DeficitPenalty:SetHide(true);
		Controls.SupplyRemaining:SetHide(false);
		Controls.SupplyRemainingValue:SetText(iUnitsSupplied - iUnitsTotal);
	end

    --------------------------------------------------------
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
    
    for unit in pPlayer:Units()
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
        	UI.incTurnTimerSemaphore();
			m_bSortReverse = not m_bSortReverse;
        	OnSort( m_SortMode );
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
        else
            UI.decTurnTimerSemaphore();
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_MILITARY_OVERVIEW, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(OnClose);

print("Loaded MilitaryOverview.lua from 'UI - Show XP in Military Overview'")