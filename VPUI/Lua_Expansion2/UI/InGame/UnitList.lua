if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- UnitList
-------------------------------------------------
include( "InstanceManager" );
include( "SupportFunctions" );
local m_MilitaryIM = InstanceManager:new( "UnitInstance", "Root", Controls.MilitaryStack );
local m_CivilianIM = InstanceManager:new( "UnitInstance", "Root", Controls.CivilianStack );

local m_SortTable;
local eName     = 0;
local eStatus   = 1;
local eMovement = 2;

local m_SortMode = eName;
local m_bSortReverse = false;


-------------------------------------------------
-------------------------------------------------
function ShowHideHandler( bIsHide )
    if( not bIsHide ) then
        UpdateDisplay();
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );


-------------------------------------------------
-------------------------------------------------
function OnClose( )
    ContextPtr:SetHide( true );
    Events.OpenInfoCorner( InfoCornerID.None );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose );


----------------------------------------------------------------
-- Key Down Processing
----------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
	if uiMsg == KeyEvents.KeyDown then
		if wParam == Keys.VK_ESCAPE then --or wParam == Keys.VK_RETURN then
			OnClose();
			return true;
		end
    end
end
ContextPtr:SetInputHandler( InputHandler );


-------------------------------------------------
-------------------------------------------------
function UnitClicked( unitID )
    local pSelectedUnit = UI:GetHeadSelectedUnit();
    if( pSelectedUnit ~= nil and
        pSelectedUnit:GetID() == unitID ) then
        UI.LookAtSelectionPlot(0);
    else
        Events.SerialEventUnitFlagSelected( Game:GetActivePlayer(), unitID );
    end
end


-------------------------------------------------
-------------------------------------------------
function OpenOverview()
	Events.SerialEventGameMessagePopup( { Type = ButtonPopupTypes.BUTTONPOPUP_MILITARY_OVERVIEW } );
end
Controls.OpenOverviewButton:RegisterCallback( Mouse.eLClick, OpenOverview );


-------------------------------------------------
-------------------------------------------------
function OnChangeEvent()
    if( ContextPtr:IsHidden() == false ) then
        UpdateDisplay();
    end
end
Events.SerialEventUnitDestroyed.Add( OnChangeEvent );
Events.SerialEventUnitSetDamage.Add( OnChangeEvent );
Events.UnitStateChangeDetected.Add( OnChangeEvent );
Events.SerialEventUnitCreated.Add( OnChangeEvent );
Events.UnitSelectionChanged.Add( OnChangeEvent );
Events.UnitActionChanged.Add( OnChangeEvent );
Events.UnitFlagUpdated.Add( OnChangeEvent );
Events.UnitGarrison.Add( OnChangeEvent );
Events.UnitEmbark.Add( OnChangeEvent );
Events.SerialEventUnitMoveToHexes.Add( OnChangeEvent );
Events.SerialEventUnitMove.Add( OnChangeEvent );
Events.SerialEventUnitTeleportedToHex.Add( OnChangeEvent );
Events.GameplaySetActivePlayer.Add(OnChangeEvent);

-------------------------------------------------
-------------------------------------------------
function UpdateDisplay()

    m_SortTable = {};

    local pPlayer = Players[ Game.GetActivePlayer() ];
    
    local bFoundMilitary = false;
    local bFoundCivilian = false;
    
    local pSelectedUnit = UI:GetHeadSelectedUnit();
    local iSelectedUnit = -1;
    if( pSelectedUnit ~= nil ) then
        iSelectedUnit = pSelectedUnit:GetID();
    end
    
    m_MilitaryIM:ResetInstances();
    m_CivilianIM:ResetInstances();
   
    for unit in pPlayer:Units()
    do
        local instance;
        local iUnit = unit:GetID();
        
        if( unit:IsCombatUnit() ) then
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
        
        sortEntry.name = Locale.ConvertTextKey( unit:GetNameKey() );
        TruncateString( instance.UnitName, 110, sortEntry.name );
        
        if( unit:MovesLeft() > 0 ) then
            instance.Button:SetAlpha( 1.0 );
        else
            instance.Button:SetAlpha( 0.6 );
        end
        
        instance.SelectionFrame:SetHide( not (iSelectedUnit == iUnit) );
       
        sortEntry.hasPromotion = unit:CanPromote(); 
        instance.PromotionIndicator:SetHide( not sortEntry.hasPromotion );

        ---------------------------------------------------------
        -- Status field
        local buildType = unit:GetBuildType();
        local activityType = unit:GetActivityType();
        if( unit:IsEmbarked() ) then
            sortEntry.status = "TXT_KEY_UNIT_STATUS_EMBARKED";
            instance.Status:SetHide( false );
            
        elseif( unit:IsGarrisoned()) then
			sortEntry.status = "TXT_KEY_MISSION_GARRISON";
            instance.Status:SetHide( false );
            
        elseif( unit:IsAutomated()) then
			if(unit:IsWork()) then
				sortEntry.status = "TXT_KEY_ACTION_AUTOMATE_BUILD";
				instance.Status:SetHide( false );
			elseif(unit:IsTrade()) then
				sortEntry.status = "TXT_KEY_ACTION_AUTOMATE_TRADE";
				instance.Status:SetHide( false );
			else
				sortEntry.status = "TXT_KEY_ACTION_AUTOMATE_EXPLORE";
				instance.Status:SetHide( false );
			end
			
		elseif( activityType == ActivityTypes.ACTIVITY_HEAL ) then
			sortEntry.status = "TXT_KEY_MISSION_HEAL";
			instance.Status:SetHide( false );
			
		elseif( activityType == ActivityTypes.ACTIVITY_SENTRY ) then
			sortEntry.status = "TXT_KEY_MISSION_ALERT";
			instance.Status:SetHide( false );
			
        elseif( unit:GetFortifyTurns() > 0 ) then
            sortEntry.status = "TXT_KEY_UNIT_STATUS_FORTIFIED";
            instance.Status:SetHide( false );
            
        elseif( activityType == ActivityTypes.ACTIVITY_SLEEP ) then
			sortEntry.status = "TXT_KEY_MISSION_SLEEP";
			instance.Status:SetHide( false );
			
        elseif( buildType ~= -1) then -- this is a worker who is actively building something
    		local thisBuild = GameInfo.Builds[buildType];
    		local civilianUnitStr = Locale.ConvertTextKey(thisBuild.Description);
    		local iTurnsLeft = unit:GetPlot():GetBuildTurnsLeft(buildType, 0, 0);	
    		local iTurnsTotal = unit:GetPlot():GetBuildTurnsTotal(buildType);	
    		if (iTurnsLeft < 4000 and iTurnsLeft > 0) then
    			civilianUnitStr = civilianUnitStr.." ("..tostring(iTurnsLeft)..")";
    		end
            sortEntry.status = civilianUnitStr;
            instance.Status:SetHide( false );
            
    	else
            sortEntry.status = "";
            instance.Status:SetHide( true );
        end
        
        if( sortEntry.status ~= "" ) then
		    instance.Status:LocalizeAndSetText( sortEntry.status );
	    else
		    instance.Status:SetText( "" );
	    end
	    
	    local statusY = instance.Status:GetSizeY();
	    statusY = statusY + 10;
	    --print(statusY);
	    if(statusY > 24)then
		   instance.StatusBox:SetSizeY(statusY); 
		   instance.SelectionFrame:SetSizeY(statusY);
		   instance.Root:SetSizeY(statusY);
		   instance.SelectHL:SetSizeY(statusY);
		   instance.SelectAnim:SetSizeY(statusY);
        end
	    local move_denominator = GameDefines["MOVE_DENOMINATOR"];
	    local moves_left = unit:MovesLeft() / move_denominator;
	    local max_moves = unit:MaxMoves() / move_denominator;
        sortEntry.movement = moves_left;
        
        if( moves_left == max_moves ) then
            instance.MovementPip:SetTextureOffsetVal( 0, 0 );
        elseif( moves_left == 0 ) then
            instance.MovementPip:SetTextureOffsetVal( 0, 96 );
        else
            instance.MovementPip:SetTextureOffsetVal( 0, 32 );
        end  
        
		instance.UnitStack:CalculateSize();
		instance.UnitStack:ReprocessAnchoring();
        
        sortEntry.unit = unit;
    end


    if( bFoundMilitary and bFoundCivilian ) then
        Controls.CivilianSeperator:SetHide( false );
    else
        Controls.CivilianSeperator:SetHide( true );
    end
     
    Controls.MilitaryStack:SortChildren( SortFunction );
    Controls.CivilianStack:SortChildren( SortFunction );
    
    Controls.MilitaryStack:CalculateSize();
    Controls.MilitaryStack:ReprocessAnchoring();
    Controls.CivilianStack:CalculateSize();
    Controls.CivilianStack:ReprocessAnchoring();
    
    Controls.MainStack:CalculateSize();
    Controls.MainStack:ReprocessAnchoring();
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
			valueA = entryA.name;
			valueB = entryB.name;
		elseif( m_SortMode == eStatus ) then
			valueA = entryA.status;
			valueB = entryB.status;
		else -- movement
			valueA = entryA.movement;
			valueB = entryB.movement;
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
Controls.SortName:SetVoid1( eName );
Controls.SortStatus:SetVoid1( eStatus );
Controls.SortMovement:SetVoid1( eMovement );

	
-------------------------------------------------
-------------------------------------------------
function OnOpenInfoCorner( iInfoType )
    if( iInfoType == InfoCornerID.Units ) then
        ContextPtr:SetHide( false );
        OnSort( m_SortMode );
    else
        ContextPtr:SetHide( true );
    end
end
Events.OpenInfoCorner.Add( OnOpenInfoCorner );
