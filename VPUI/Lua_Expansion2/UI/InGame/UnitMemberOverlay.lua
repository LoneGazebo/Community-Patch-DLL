if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-- UnitMemberOverlay
-------------------------------------------------
include( "FLuaVector" );
include( "InstanceManager" );
local g_CombatManager = InstanceManager:new( "UnitMemberOverlay", "Anchor", Controls.CombatOverlays );


local g_MasterList = {};
local g_PrintDebug = false;


local BlackFog = 0; -- invisible
local GreyFog  = 1; -- once seen
local WhiteFog = 2; -- eyes on
local g_DimAlpha = 0.45;

local g_TargetColors = {};

local g_UnitMemberOverlayClass = 
{
    ------------------------------------------------------------------
    -- default values
    ------------------------------------------------------------------
    m_Instance = {},

    m_IsSelected = false,
    
    m_Player   = -1,
    m_PlayerID = -1,
    m_UnitID   = -1,
    m_MemberID = -1,
        
    m_Unit = {},
    m_Escort = nil;
    
    ------------------------------------------------------------------
    -- constructor
    ------------------------------------------------------------------
    new = function( self, playerID, unitID, memberID, fogState )
        local o = {};
        setmetatable( o, self );
        self.__index = self;
        o.m_Instance = {};
        
        if( playerID ~= -1 )
		then
			o.m_InstanceManager = g_CombatManager;
			o.m_Instance = o.m_InstanceManager:GetInstance();
                        
            o:Initialize( playerID, unitID, memberID, fogState );
                        
            local playerTable = g_MasterList[ playerID ];
            if playerTable == nil 
            then
                playerTable = {};
                g_MasterList[ playerID ] = playerTable
            end
            
            local unitTable = playerTable[ unitID ];
            if unitTable == nil
            then
            	unitTable = {};
            	playerTable[ unitID ] = unitTable;
            end
            
            unitTable[ memberID ] = o;
        end
        
        return o;
    end,
     
            
    ------------------------------------------------------------------
    -- constructor
    ------------------------------------------------------------------
    Initialize = function( o, playerID, unitID, memberID, fogState )
        o.m_Player = Players[ playerID ];
        o.m_PlayerID = playerID;
        o.m_Unit = o.m_Player:GetUnitByID( unitID );
        o.m_UnitID = unitID;
        o.m_MemberID = memberID;
        
        if( o.m_Unit == nil )
        then
            print( string.format( "Unit not found for UnitFlag: Player[%i] Unit[%i]", playerID, unitID ) );
            return nil;
        end
         

		o.m_Instance.InfoTxt1:SetHide(false);
		o.m_Instance.InfoTxt2:SetHide(false);
		o.m_Instance.TargetColor:SetHide(false);
        o.m_Instance.TargetColor:SetPercent( 100 );
		o.m_Instance.Anchor:SetHide(false);
		
		if( g_PrintDebug ) 
		then 
			print( "Instance members:" );
		
			for itrMemberID, itrMember in pairs( o.m_Instance ) 
			do
   				print( tostring( itrMemberID ) );
   			end

			print( "InstanceManager members:" );
		
			for itrMemberID, itrMember in pairs( o.m_InstanceManager ) 
			do
   				print ( tostring( itrMemberID ) );
   			end

   			print ( "m_RootControlName = " .. tostring( o.m_InstanceManager.m_RootControlName ) );
   			print ( "m_ParentControl is hidden = " .. tostring( o.m_InstanceManager.m_ParentControl:IsHidden() ) );
   			print ( "m_ParentControl.GetID = " .. tostring( o.m_InstanceManager.m_ParentControl:GetID() ) );
   			
			print( "Controls.CombatOverlays members:" );
		
			for itrMemberID, itrMember in pairs( Controls.CombatOverlays ) 
			do
   				print ( tostring( itrMemberID ) );
   			end

   			print ( "Controls.CombatOverlays is hidden = " .. tostring( Controls.CombatOverlays:IsHidden() ) );
		end

		Controls.CombatOverlays:SetHide(false);
		
        ---------------------------------------------------------
        -- update all the info
        o:SetTargetColor( Vector4(0.5, 0.5, 0.5, 1.0) );
        o:SetFogState( fogState );
        
        ---------------------------------------------------------
        -- Set the world position
        local worldPosX, worldPosY, worldPosZ = GridToWorld( o.m_Unit:GetX(), o.m_Unit:GetY() );
        -- Optional offset.  For now, 0,0,0 seems fine (might want to get rid of this then)
        local worldOffset = Vector3( 0, 0, 0 );
        local position = VecAdd( Vector3( worldPosX, worldPosY, worldPosZ ), worldOffset );
        if( g_PrintDebug ) then print( "Initial position: " .. tostring( position.x ) .. ", " .. tostring( position.y ) .. ", " .. tostring( position.z ) ); end
        
        o:SetPosition( position );
        
    end,
               
    
    ------------------------------------------------------------------
    ------------------------------------------------------------------
    destroy = function( self )
        if( self.m_InstanceManager ~= nil )
        then
			if( g_PrintDebug ) then print( "Releasing overlay instance "); end
            self.m_InstanceManager:ReleaseInstance( self.m_Instance );
            g_MasterList[ self.m_PlayerID ][ self.m_UnitID ][ self.m_MemberID ] = nil;            
        end
    end,
    

    ------------------------------------------------------------------
    ------------------------------------------------------------------
    SetTargetColor = function( self, color )
        
        if( g_PrintDebug ) then print( "Setting target color to: " .. tostring( color.x ) .. " " .. tostring( color.y ) .. " " .. tostring( color.x ) .. " " .. tostring( color.w )); end
        color.w = 1.0;
        self.m_Instance.TargetColor:SetFGColor( color );
    end,

    ------------------------------------------------------------------
    ------------------------------------------------------------------
    SetFogState = function( self, fogState )
        if( fogState ~= WhiteFog ) then
	        if( g_PrintDebug ) then print( "Hiding anchor " ); end
            self.m_Instance.Anchor:SetHide( true );
        else
	        if( g_PrintDebug ) then print( "Showing anchor " ); end
            self.m_Instance.Anchor:SetHide( false );
        end
        
        self.m_FogState = fogState;
    end,
    
    ------------------------------------------------------------------
    ------------------------------------------------------------------
    UpdateSelected = function( self )
        self.m_IsSelected = self.m_Unit:IsSelected();
        if( g_PrintDebug ) then print( "Setting selected: " .. tostring( self.m_IsSelected ) .. " " .. tostring( self.m_PlayerID ) .. " " .. tostring( self.m_UnitID ) ); end
                       
    end,
    
    -----------------------------------------------------------------
    ------------------------------------------------------------------
    SetDimState = function( self, isDim )
        if( isDim )
        then
            self.m_Instance.Anchor:SetAlpha( g_DimAlpha );
        else
            self.m_Instance.Anchor:SetAlpha( 1.0 );
        end
        
        self.m_IsDim = isDim;
    end,


    -----------------------------------------------------------------
    ------------------------------------------------------------------
    SetPosition = function( self, position )
               
        if( g_PrintDebug ) then print( "Setting position: " .. tostring( position.x ) .. ", " .. tostring( position.y ) .. ", " .. tostring( position.z ) ); end
        self.m_Instance.Anchor:SetWorldPosition( position );
               
    end,

    -----------------------------------------------------------------
    ------------------------------------------------------------------
    SetMessage1 = function( self, msg )
               
        if( g_PrintDebug ) then print( "Setting message: " .. tostring( msg ) ); end
        self.m_Instance.InfoTxt1:SetText( tostring( msg ) );
               
    end,

    -----------------------------------------------------------------
    ------------------------------------------------------------------
    SetMessage2 = function( self, msg )
               
        if( g_PrintDebug ) then print( "Setting message: " .. tostring( msg ) ); end
        self.m_Instance.InfoTxt2:SetText( tostring( msg ) );
               
    end,
    
    ------------------------------------------------------------------
    ------------------------------------------------------------------
    ShowHide = function( self, bShow )

        if( g_PrintDebug ) then print( "Setting visibility: " .. tostring( bShow ) ); end
		self.m_Instance.Anchor:SetHide( bShow ~= true );
			        
    end,
    
}


-------------------------------------------------
-- On UnitOverlayAdd
-------------------------------------------------
function OnUnitOverlayAdd( playerID, unitID, memberID, position )

	if( g_PrintDebug ) then print( "Handling remove: " .. tostring( playerID ) .. " " .. tostring( unitID ) .. " " .. tostring( memberID ) .. ":" .. tostring( position.x ).. "," .. tostring( position.y ) .. "," .. tostring( position.z ) ); end

	local playerUnitList = Players[ playerID ];
	if playerUnitList == nil then return end
	
	local unit = playerUnitList:GetUnitByID( unitID );
    if ( unit == nil or unit:IsDead() ) then return end
    
	local unit = g_UnitMemberOverlayClass:new( playerID, unitID, memberID, WhiteFog );
	unit:ShowHide(true);
	unit:SetMessage1( memberID );
	unit:SetPosition( position );
end
Events.UnitMemberOverlayAdd.Add( OnUnitOverlayAdd );

-------------------------------------------------
-- On UnitOverlayRemove
-------------------------------------------------
function OnUnitOverlayRemove( playerID, unitID, memberID )

	if( g_PrintDebug ) then print( "Handling remove: " .. tostring( playerID ) .. " " .. tostring( unitID ) .. " " .. tostring( memberID ) ); end

	local playerUnitList = Players[ playerID ];
	if playerUnitList == nil then return end
	
	local unit = playerUnitList:GetUnitByID( unitID );
    if ( unit == nil or unit:IsDead() ) then return end
    
    local playerList = g_MasterList[ playerID ];
    if playerList ~= nil
    then
    	local unitList = playerList[ unitID ];
    	if unitList ~= nil
    	then
    		local unit = unitList[ memberID ];
    		if unit ~= nil
    		then
	   			unit:destroy();
		        unitList[ memberID ] = nil;
			end
		end
	end
end
Events.UnitMemberOverlayRemove.Add( OnUnitOverlayRemove );

-------------------------------------------------
-- On UnitOverlayShowHide
-------------------------------------------------
function OnUnitOverlayShowHide( playerID, unitID, memberID, bShow )

	local playerUnitList = Players[ playerID ];
	if playerUnitList == nil then return end
	
	local unit = playerUnitList:GetUnitByID( unitID );
    if ( unit == nil or unit:IsDead() ) then return end
    
    local playerList = g_MasterList[ playerID ];
    if playerList ~= nil
    then
    	local unitList = playerList[ unitID ];
    	if unitList ~= nil
    	then
    		local unit = unitList[ memberID ];
    		if unit ~= nil
    		then
	   			unit:ShowHide(bShow);
			end
		end
	end
end
Events.UnitMemberOverlayShowHide.Add( OnUnitOverlayShowHide );

-------------------------------------------------
-- On Unit Position Changed
-- sent by the engine while it walks a unit around
-------------------------------------------------
function OnUnitPositionChanged( playerID, unitID, unitPosition )

	local playerUnitList = Players[ playerID ];
	if playerUnitList == nil then return end
	
	local unit = playerUnitList:GetUnitByID( unitID );
    if ( unit == nil or unit:IsDead() ) then return end
    
    local playerList = g_MasterList[ playerID ];
    if playerList ~= nil
    then
    	local unitList = playerList[ unitID ];
    	if unitList ~= nil
    	then
    		for itrMemberID, itrMember in pairs(unitList)
    		do
    			itrMember:SetPosition( unitPosition );
        	end
        end
    end
end
-- Events.LocalMachineUnitPositionChanged.Add( OnUnitPositionChanged );

-------------------------------------------------
-- Combat Target has changed
-------------------------------------------------
function OnUnitMemberCombatTargetChanged( playerID, unitID, memberID, targetPlayerID, targetUnitID, targetMemberID )

	local playerUnitList = Players[ playerID ];
	if playerUnitList == nil then return end
	
	local unit = playerUnitList:GetUnitByID( unitID );
    if ( unit == nil or unit:IsDead() ) then return end
    
    local playerList = g_MasterList[ playerID ];
    if playerList ~= nil
    then
    	local unitList = playerList[ unitID ];
    	if unitList ~= nil
    	then
    		local member = unitList[ memberID ];
    		if member ~= nil
    		then    			
    			if ((targetMemberID % 256) == 255)
    			then
    				member:SetTargetColor( Vector4(0.5, 0.5, 0.5, 1.0) );
    				member:SetMessage1( tostring( memberID ) );
    			else
	    			member:SetTargetColor( g_TargetColors[ targetMemberID % 16] );
    				member:SetMessage1( tostring( memberID ) .. ", T:" .. tostring( targetMemberID % 256 ) );
				end	    			
        	end
        end
    end
end
Events.UnitMemberCombatTargetChanged.Add( OnUnitMemberCombatTargetChanged );

-------------------------------------------------
-- Unit members position has changed
-------------------------------------------------
function OnUnitMemberPositionChanged( playerID, unitID, memberID, unitPosition )

	local playerUnitList = Players[ playerID ];
	if playerUnitList == nil then return end
	
	local unit = playerUnitList:GetUnitByID( unitID );
    if ( unit == nil or unit:IsDead() ) then return end
    
    local playerList = g_MasterList[ playerID ];
    if playerList ~= nil
    then
    	local unitList = playerList[ unitID ];
    	if unitList ~= nil
    	then
    		local member = unitList[ memberID ];
    		if member ~= nil
    		then
    			member:SetPosition( unitPosition );
        	end
        end
    end
end
Events.UnitMemberPositionChanged.Add( OnUnitMemberPositionChanged );

-------------------------------------------------
-- Combat Target state has changed
-------------------------------------------------
function OnUnitMemberCombatStateChanged( playerID, unitID, memberID, stateID )

	if( g_PrintDebug ) then print( "Handling state change: " .. tostring( playerID ) .. " " .. tostring( unitID ) .. " " .. tostring( memberID ) .. " " .. tostring( stateID ) ); end

	local playerUnitList = Players[ playerID ];
	if playerUnitList == nil then return end
	
	local unit = playerUnitList:GetUnitByID( unitID );
    if ( unit == nil or unit:IsDead() ) then return end
    
    local playerList = g_MasterList[ playerID ];
    if playerList ~= nil
    then
    	local unitList = playerList[ unitID ];
    	if unitList ~= nil
    	then
    		local member = unitList[ memberID ];
    		if member ~= nil
    		then
    			member:SetMessage2( stateID );
        	end
        end
    end
end
Events.UnitMemberCombatStateChanged.Add( OnUnitMemberCombatStateChanged );


-------------------------------------------------
-- On Unit Destroyed
-------------------------------------------------
function OnUnitDestroyed( playerID, unitID )

	local playerList = g_MasterList[ playerID ];
	if playerList ~= nil
	then
    	local unitTable = playerList[ unitID ];
		if unitTable ~= nil
		then
			for itrUnitID, itrUnitMember in pairs( unitTable ) 
			do
   				itrUnitMember:destroy();
   			end   		
			playerList[ unitID ] = nil;
		end
    end
end
Events.SerialEventUnitDestroyed.Add( OnUnitDestroyed );

-------------------------------------------------
-------------------------------------------------
function OnUnitSelect( playerID, unitId, i, j, k, isSelected )

	local playerUnitList = Players[ playerID ];
	if playerUnitList == nil then return end
	
	local unit = playerUnitList:GetUnitByID( unitID );
    if ( unit == nil or unit:IsDead() ) then return end

	local playerList = g_MasterList[ playerID ];
	if playerList ~= nil
	then
		local unitList = playerList[ unitID ];
		if unitList ~= nil
		then			
			local unitMember = unitList[ memberID ];
			if unitMember ~= nil
			then			
				unitMember:UpdateSelected();
			end
		end
    end
end
Events.UnitSelectionChanged.Add( OnUnitSelect );

------------------------------------------------------------
-- this goes off when a hex is seen or unseen
------------------------------------------------------------
function OnHexFogEvent( hexPos, fogState, bWholeMap )
    if( bWholeMap ) 
    then
        for playerID,playerTable in pairs( g_MasterList ) 
        do
            for unitID,unitTable in pairs( playerTable ) 
            do
	            for memberID,member in pairs( unitTable ) 
    	        do
                	member:SetFogState( fogState );
                end
            end
        end
    else
        local gridVecX, gridVecY = ToGridFromHex( hexPos.x, hexPos.y );
        local plot = Map.GetPlot( gridVecX, gridVecY );
        
        if( plot ~= nil ) then
        
            local unitCount = plot:GetNumUnits();
            
            for i = 0, unitCount - 1, 1
            do
                local unit = plot:GetUnit( i );
                if( unit ~= nil ) 
                then
                    local owner, unitID = unit:GetOwner(), unit:GetID();
                    local playerTable = g_MasterList[ owner ];
                    if playerTable ~= nil
                    then
                    	local unitTable = playerTable[ unitID ];
                    	if unitTable ~= nil
                    	then
				            for memberID,member in pairs( unitTable ) 
			    	        do
			                	member:SetFogState( fogState );
			                end
						end
                    end
                end
            end
        end
    end
end
Events.HexFOWStateChanged.Add( OnHexFogEvent );

------------------------------------------------------------
-- this goes off when a unit moves into or out of the fog
------------------------------------------------------------
function OnUnitFogEvent( playerID, unitID, fogState )
	local playersUnitList = Players[ playerID ];
    if playersUnitList == nil then return end
    
    local unit = playersUnitList:GetUnitByID( unitID );    
    if (unit == nil or unit:IsDead() ) then return end

    playerTable = g_MasterList[ playerID ];
    if( playerTable ~= nil ) 
    then
        local unitTable = playerTable[ unitID ];
        if( unitTable ~= nil ) 
        then
            for memberID,member in pairs( unitTable ) 
	        do
            	member:SetFogState( fogState );
            end
        end
    end
end
Events.UnitStateChangeDetected.Add( OnUnitFogEvent );


------------------------------------------------------------
-- this goes off when a unit moves into or out of the fog
------------------------------------------------------------
function OnDimEvent( playerID, unitID, isDim )

	local playersUnitList = Players[ playerID ];
    if playersUnitList == nil then return end
    
    local unit = playersUnitList:GetUnitByID( unitID );    
    if (unit == nil or unit:IsDead() ) then return end

    playerTable = g_MasterList[ playerID ];
    if( playerTable ~= nil ) 
    then
        local unitTable = playerTable[ unitID ];
        if( unitTable ~= nil )
        then
        	local active_team = Players[Game.GetActivePlayer()]:GetTeam();
        	local team = Players[playerID]:GetTeam();
        	
        	if( active_team == team )
        	then
                --print( "  Unit dim: " .. tostring( playerID ) .. " " .. tostring( unitID ) .. " " .. isDim );
	            for memberID,member in pairs( unitTable ) 
		        do
	            	member:SetDimState( (isDim == 1) );
	            end                
        	end
        end
    end
end
Events.UnitShouldDimFlag.Add( OnDimEvent );

-----------------------------------------------------------------------------------------
-- Initialization
-----------------------------------------------------------------------------------------

g_TargetColors[0] = Vector4(1.0, 0.0, 0.0, 1.0);
g_TargetColors[1] = Vector4(1.0, 1.0, 0.0, 1.0);
g_TargetColors[2] = Vector4(0.5, 1.0, 0.0, 1.0);
g_TargetColors[3] = Vector4(0.0, 1.0, 1.0, 1.0);
g_TargetColors[4] = Vector4(0.5, 0.5, 1.0, 1.0);
g_TargetColors[5] = Vector4(1.0, 0.5, 0.0, 1.0);
g_TargetColors[6] = Vector4(0.0, 1.0, 0.0, 1.0);
g_TargetColors[7] = Vector4(0.0, 0.0, 1.0, 1.0);
g_TargetColors[8] = Vector4(0.5, 0.0, 1.0, 1.0);
g_TargetColors[9] = Vector4(0.5, 0.0, 0.5, 1.0);
g_TargetColors[10] = Vector4(0.5, 0.25, 0.0, 1.0);
g_TargetColors[11] = Vector4(0.0, 0.25, 0.0, 1.0);
g_TargetColors[12] = Vector4(0.5, 0.0, 0.0, 1.0);
g_TargetColors[13] = Vector4(0.25, 0.5, 0.5, 1.0);
g_TargetColors[14] = Vector4(0.0, 0.0, 0.5, 1.0);
g_TargetColors[15] = Vector4(0.5, 0.0, 0.25, 1.0);
