-- UnitFlagManager
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
local g_MilitaryManager = InstanceManager:new( "NewUnitFlag", "Anchor", Controls.MilitaryFlags );
local g_CivilianManager = InstanceManager:new( "NewUnitFlag", "Anchor", Controls.CivilianFlags );
local g_AirCraftManager = InstanceManager:new( "NewUnitFlag", "Anchor", Controls.AirCraftFlags );

local g_MasterList = {};
local g_LastPlayerID;
local g_LastUnitID;
local g_PrintDebug = false;
local g_GarrisonedUnitFlagsInStrategicView = true;

local BlackFog = 0; -- invisible
local GreyFog  = 1; -- once seen
local WhiteFog = 2; -- eyes on
local g_DimAlpha = 0.45;
local g_DimAirAlpha = 0.6;

local GarrisonOffset = Vector2( -43, -39 );
local GarrisonOtherOffset = Vector2( -55, -34 );
local CityNonGarrisonOffset = Vector2( 45, -45 );
local CivilianOffset = Vector2( 0, -25 );

local g_AirFlagNormalOffset = -24;
local g_AirFlagHealthOffset = -19;

local g_CityFlags = {};
local g_CityFlagPlots = {};
local g_SelectedContainer = ContextPtr:LookUpControl( "../SelectedUnitContainer" );
local g_SelectedFlag = nil;
local CityWorldPositionOffset = { x = 0, y = 0, z = 35 };

local g_UnitFlagClass = 
{
    ------------------------------------------------------------------
    -- default values
    ------------------------------------------------------------------
    m_Instance = {},
    
    m_FlagType = 0,
    m_UnitType = 0,
    m_IsSelected = false,
    m_IsCurrentlyVisible = true,
    m_IsInvisible = false,
    m_IsGarrisoned = false,
    m_IsDimmed = false,
    m_IsForceHide = false,
    m_OverrideDimedFlag = false,
    m_HasCivilianSelectFlag = false,
    m_Health   = 1,
    
    m_Player   = -1,
    m_PlayerID = -1,
    m_CivID    = -1,
    m_UnitID   = -1,
    
    m_IsAirCraft = false,
    m_IsTrade = false,
    m_IsCivilian = false,
    m_CargoControls = nil,
    m_CarrierFlag = nil,
    m_CityFlag    = nil,
    m_CargoIsActive = false,
    
    m_Escort = nil;


    ------------------------------------------------------------------
    -- constructor
    ------------------------------------------------------------------
    new = function( self, playerID, unitID, fogState, invisible )
        local o = {};
        setmetatable( o, self );
        self.__index = self;
        o.m_Instance = {};
        
        if( playerID ~= -1 )
        then
			local pUnit = Players[ playerID ]:GetUnitByID( unitID );
			
		    if( pUnit:IsCombatUnit() and not pUnit:IsEmbarked()) then
                o.m_InstanceManager = g_MilitaryManager;
            else
                if( pUnit:GetDomainType() == DomainTypes.DOMAIN_AIR ) then
                    o.m_InstanceManager = g_AirCraftManager;
                    o.m_IsAirCraft = true;
                else
                    o.m_InstanceManager = g_CivilianManager;
                end
            end

            o.m_Instance = o.m_InstanceManager:GetInstance();
            o:Initialize( playerID, unitID, fogState, invisible );
           
            ---------------------------------------------------------
            -- Can carry units
            if( pUnit:CargoSpace() > 0 ) then
            
                --print( "unit has cargo space, building air button" );
                o.m_CargoControls = {};
                ContextPtr:BuildInstanceForControl( "AirButton", o.m_CargoControls, o.m_Instance.AirAnchor );
                o.m_CargoControls.PullDown:RegisterSelectionCallback( UnitFlagClicked );
                o.m_CargoControls.PullDown:ReprocessAnchoring();
               -- print( "creation cargo" );
                o:UpdateCargo();
            end

            ---------------------------------------------------------
            -- build the table for this player and store the flag
            local playerTable = g_MasterList[ playerID ];
            if playerTable == nil 
            then
                playerTable = {};
                g_MasterList[ playerID ] = playerTable
            end
            g_MasterList[ playerID ][ unitID ] = o;
            
            -- If the unit is cargo, link to the carrier, if it is already created.  If not, the carrier will create the link
            if (pUnit:IsCargo() and o.m_CarrierFlag == nil) then
				local pCarrier = pUnit:GetTransportUnit();
				if (pCarrier ~= nil) then
    				local pCarrierFlag = g_MasterList[ pCarrier:GetOwner() ][ pCarrier:GetID() ];
    				-- Check to see if it is there, it is possible it has yet to be created
    				if (pCarrierFlag ~= nil) then
    					o.m_CarrierFlag = pCarrierFlag;
    					pCarrierFlag:UpdateCargo();
    				end
    			end
    		end            
	        
			-- Threatening? (Disabled)
			--if (pUnit:IsThreateningAnyMinorCiv()) then
				--OnMarkThreateningEvent( playerID, unitID, true )
			--else
	            --o:SetFlash( false );
			--end
        end
        
        return o;
    end,
     
    
        
    ------------------------------------------------------------------
    -- constructor
    ------------------------------------------------------------------
    Initialize = function( o, playerID, unitID, fogState, invisible )
        o.m_Player = Players[ playerID ];
        o.m_PlayerID = playerID;
        o.m_UnitID = unitID;
        
        if( g_PrintDebug ) then print( string.format( "Creating UnitFlag for: Player[%i] Unit[%i]", playerID, unitID ) ); end
        
		local pUnit = Players[ playerID ]:GetUnitByID( unitID );
        if( pUnit == nil )
        then
            print( string.format( "Unit not found for UnitFlag: Player[%i] Unit[%i]", playerID, unitID ) );
            return nil;
        end
     
		o.m_IsTrade = pUnit:IsTrade();
        o.m_IsCivilian = not pUnit:IsCombatUnit() or pUnit:IsEmbarked();
        o.m_IsInvisible = invisible;
        
        -- Technically, we should get a UnitGarrisoned event after the creation event if
        -- the unit is garrisoned.  So IsGarrisoned should always be false at creation.
        -- In the interest of preserving behavior I'm allowing m_IsGarrisoned to be set
        -- using IsGarrisoned() on creation.  However, in the strategic view this causes
        -- a visibility error in some odd cases so there it always starts as false.
        if( InStrategicView() )
        then
			o.m_IsGarrisoned = false;
		else
			o.m_IsGarrisoned = pUnit:IsGarrisoned();
		end
      
        ---------------------------------------------------------
        -- Hook up the button
        local pPlayer = Players[Game.GetActivePlayer()];
        local active_team = pPlayer:GetTeam();
        local team = o.m_Player:GetTeam();
        			
		if (o.m_Player:IsHuman()) then
            o.m_Instance.NormalButton:SetVoid1( playerID );
            o.m_Instance.NormalButton:SetVoid2( unitID );
            o.m_Instance.NormalButton:RegisterCallback( Mouse.eLClick, UnitFlagClicked );
            o.m_Instance.NormalButton:RegisterCallback( Mouse.eMouseEnter, UnitFlagEnter );
            o.m_Instance.NormalButton:RegisterCallback( Mouse.eMouseExit, UnitFlagExit );
            
            o.m_Instance.HealthBarButton:SetVoid1( playerID );
            o.m_Instance.HealthBarButton:SetVoid2( unitID );
            o.m_Instance.HealthBarButton:RegisterCallback( Mouse.eLClick, UnitFlagClicked );
            o.m_Instance.HealthBarButton:RegisterCallback( Mouse.eMouseEnter, UnitFlagEnter );
            o.m_Instance.HealthBarButton:RegisterCallback( Mouse.eMouseExit, UnitFlagExit );
		end            		
		
        if( active_team == team ) then
            o.m_Instance.NormalButton:SetDisabled( false );
            o.m_Instance.NormalButton:SetConsumeMouseOver( true );

            o.m_Instance.HealthBarButton:SetDisabled( false );
            o.m_Instance.HealthBarButton:SetConsumeMouseOver( true );
            
		    local pPlot = pUnit:GetPlot();
		    if( pPlot:IsCity() ) then
			    o.m_CityFlag = UpdateCityCargo( pPlot );
		    end

        else
            o.m_Instance.NormalButton:SetDisabled( true );
            o.m_Instance.NormalButton:SetConsumeMouseOver( false );
            o.m_Instance.NormalButton:RegisterCallback( Mouse.eMouseEnter, function() 
				local pMouseOverUnit = Players[ playerID ]:GetUnitByID( unitID );
				if (pMouseOverUnit ~= nil) then
					Game.MouseoverUnit(pMouseOverUnit,true);
				end
				end);
            o.m_Instance.NormalButton:RegisterCallback( Mouse.eMouseExit, function() 
				local pMouseOverUnit = Players[ playerID ]:GetUnitByID( unitID );
				if (pMouseOverUnit ~= nil) then
					Game.MouseoverUnit(pMouseOverUnit,false);
				end
				end);
            o.m_Instance.HealthBarButton:SetDisabled( true );
            o.m_Instance.HealthBarButton:SetConsumeMouseOver( false );
        end


        ---------------------------------------------------------
        -- update all the info
        o:UpdateName();
        o:SetUnitColor();
        o:SetUnitType();
        o:UpdateFlagType();
        o:UpdateHealth();
        o:UpdateSelected();
        o:SetFogState( fogState );
        o:UpdateFlagOffset();
        o:UpdateVisibility();
    
    
        ---------------------------------------------------------
        -- Set the world position
        local worldPosX, worldPosY, worldPosZ = GridToWorld( pUnit:GetX(), pUnit:GetY() );
        worldPosZ = worldPosZ + 35;               
        
        o:UnitMove( worldPosX, worldPosY, worldPosZ );
        
    end,
        
       
    
    ------------------------------------------------------------------
    ------------------------------------------------------------------
    destroy = function( self )
        if( self.m_InstanceManager ~= nil )
        then
            -- tell our escort we're dead
            if( self.m_Escort ~= nil ) then
                self.m_Escort.m_IsForceHide = false;
                self.m_Escort:UpdateVisibility();
                self.m_Escort:SetEscort( nil );
            end
			self:SetEscort( nil );
            
            if( self.m_CargoControls ~= nil ) then
                self.m_Instance.AirAnchor:DestroyAllChildren();
                self.m_CargoIsActive = false;
                self.m_CargoControls = nil;
            end
           
            self:UpdateSelected( false );
                        
            if( self.m_CarrierFlag ~= nil ) then
    		    self.m_CarrierFlag:UpdateCargo();
		    end
		    
            if( self.m_CityFlag ~= nil ) then
		        UpdateCityCargo( g_CityFlagPlots[ self.m_CityFlag ] );
		    end
		    
            self.m_InstanceManager:ReleaseInstance( self.m_Instance );
            g_MasterList[ self.m_PlayerID ][ self.m_UnitID ] = nil;
        end
    end,
    


    ------------------------------------------------------------------
    ------------------------------------------------------------------
    SetUnitColor = function( self )
        local iconColor, flagColor = self.m_Player:GetPlayerColors();
        
        if( self.m_Player:IsMinorCiv() )
        then
            flagColor, iconColor = iconColor, flagColor;
        end

        self.m_Instance.FlagBase:SetColor( flagColor );
        self.m_Instance.UnitIcon:SetColor( iconColor );
        self.m_Instance.FlagBaseOutline:SetColor( iconColor );

    end,



    ------------------------------------------------------------------
    ------------------------------------------------------------------
    SetUnitType = function( self )
            
        local unit = Players[ self.m_PlayerID ]:GetUnitByID( self.m_UnitID );
        if unit == nil then
			return;
		end
				
		local thisUnitInfo = GameInfo.Units[unit:GetUnitType()];
		 
 		local textureOffset, textureSheet = IconLookup( thisUnitInfo.UnitFlagIconOffset, 32, thisUnitInfo.UnitFlagAtlas );				
		self.m_Instance.UnitIcon:SetTexture( textureSheet );
		self.m_Instance.UnitIconShadow:SetTexture( textureSheet );
		self.m_Instance.UnitIcon:SetTextureOffset( textureOffset );
		self.m_Instance.UnitIconShadow:SetTextureOffset( textureOffset );

    end,



    ------------------------------------------------------------------
    ------------------------------------------------------------------
    SetFogState = function( self, fogState )
        if( fogState ~= WhiteFog ) then
            self:SetHide( true );
        else
            self:SetHide( false );
        end
        
        self.m_FogState = fogState;
    end,
    
    ------------------------------------------------------------------
    ------------------------------------------------------------------
    SetHide = function( self, bHide )
		self.m_IsCurrentlyVisible = not bHide;
		self:UpdateVisibility();
    end,
    
    ------------------------------------------------------------
    ------------------------------------------------------------
    UpdateVisibility = function( self )
		local bVisible = self.m_IsCurrentlyVisible and not self.m_IsInvisible and not self.m_IsForceHide;
    	if InStrategicView() then
    		local bShowInStrategicView = bVisible and g_GarrisonedUnitFlagsInStrategicView and self.m_IsGarrisoned;
    		self.m_Instance.Anchor:SetHide(not bShowInStrategicView);
    	else
    		self.m_Instance.Anchor:SetHide(not bVisible);
    		-- Set the escorted unit too.
    		-- Do not change this in relation to the 'invisible' flag, that is a state of only that unit.  i.e. A sub does not hide the escorted unit
    		if( self.m_Escort ~= nil ) then
				local bEscortVisible = self.m_IsCurrentlyVisible and not self.m_IsForceHide;
        		self.m_Escort.m_Instance.Anchor:SetHide(not bEscortVisible);
        	end
        	if( self.m_CargoControls ~= nil and bVisible) then
        		self:UpdateCargo();
    		end
    	end
    end,
    
    ------------------------------------------------------------
    ------------------------------------------------------------
    GarrisonComplete = function( self, bGarrisoned )    
		self.m_IsGarrisoned = bGarrisoned;
		self:UpdateVisibility();
		self:UpdateFlagOffset();
    end,
 
    ------------------------------------------------------------------
    ------------------------------------------------------------------
    UpdateHealth = function( self )
    
		local pUnit = Players[ self.m_PlayerID ]:GetUnitByID( self.m_UnitID );
        if pUnit == nil then
			return;
		end	
			
        local healthPercent = math.max( math.min( pUnit:GetCurrHitPoints() / pUnit:GetMaxHitPoints(), 1 ), 0 );
        if( g_PrintDebug ) then print( "Setting health: " .. tostring( healthPercent ) .. " " .. tostring( self.m_PlayerID ) .. " " .. tostring( self.m_UnitID ) ); end
    
        -- going to damaged state
        if( healthPercent < 1 )
        then
            -- show the bar and the button anim
            self.m_Instance.HealthBarBG:SetHide( false );
            self.m_Instance.HealthBar:SetHide( false );
            self.m_Instance.HealthBarButton:SetHide( false );
            
            if( self.m_CargoControls ~= nil ) then
                self.m_CargoControls.PullDown:SetOffsetX( g_AirFlagHealthOffset );
            end
        
            -- hide the normal button
            self.m_Instance.NormalButton:SetHide( true );
            
            -- handle the selection indicator    
            if( self.m_IsSelected )
            then
                self.m_Instance.NormalSelect:SetHide( true );
                self.m_Instance.HealthBarSelect:SetHide( false );
            end
                    
            if( healthPercent > 0.66 )
            then
                self.m_Instance.HealthBar:SetFGColor( Vector4( 0, 1, 0, 1 ) );
            elseif( healthPercent > 0.33 )
            then
                self.m_Instance.HealthBar:SetFGColor( Vector4( 1, 1, 0, 1 ) );
            else
                self.m_Instance.HealthBar:SetFGColor( Vector4( 1, 0, 0, 1 ) );
            end
            
        --------------------------------------------------------------------    
        -- going to full health
        else
            self.m_Instance.HealthBar:SetFGColor( Vector4( 0, 1, 0, 1 ) );
            
            -- hide the bar and the button anim
            self.m_Instance.HealthBarBG:SetHide( true );
            self.m_Instance.HealthBar:SetHide( true );
            self.m_Instance.HealthBarButton:SetHide( true );
            if( self.m_CargoControls ~= nil ) then
                self.m_CargoControls.PullDown:SetOffsetX( g_AirFlagNormalOffset );
            end
        
            -- show the normal button
            self.m_Instance.NormalButton:SetHide( false );
        
            -- handle the selection indicator    
            if( self.m_IsSelected )
            then
                self.m_Instance.NormalSelect:SetHide( false );
                self.m_Instance.HealthBarSelect:SetHide( true );
            end
        end
        
        self.m_Instance.HealthBar:SetPercent( healthPercent );
        self.m_Health = healthPercent;
    end,
   
    
    

    ------------------------------------------------------------------
    ------------------------------------------------------------------
    UpdateSelected = function( self, isSelected )
        self.m_IsSelected = isSelected; --self.m_Unit:IsSelected();
        if( g_PrintDebug ) then print( "Setting selected: " .. tostring( self.m_IsSelected ) .. " " .. tostring( self.m_PlayerID ) .. " " .. tostring( self.m_UnitID ) ); end
        
        if( self.m_Health >= 1 )
        then
            self.m_Instance.NormalSelect:SetHide( not self.m_IsSelected );
            self.m_Instance.HealthBarSelect:SetHide( true );
        else
            self.m_Instance.HealthBarSelect:SetHide( not self.m_IsSelected );
            self.m_Instance.NormalSelect:SetHide( true );
        end
        
        if( self.m_IsSelected ) then
            self.m_Instance.Anchor:ChangeParent( g_SelectedContainer );
            g_SelectedFlag = self;
        else
            
            if( self.m_IsAirCraft ) then
                self.m_Instance.Anchor:ChangeParent( Controls.AirCraftFlags );
            elseif( self.m_IsCivilian ) then
                self.m_Instance.Anchor:ChangeParent( Controls.CivilianFlags );
            elseif( self.m_IsGarrisoned ) then
                self.m_Instance.Anchor:ChangeParent( Controls.GarrisonFlags );
            else
                self.m_Instance.Anchor:ChangeParent( Controls.MilitaryFlags );
            end
            
        end
        
        self:OverrideDimmedFlag( self.m_IsSelected );
        
    end,
    
    
    ------------------------------------------------------------------
    ------------------------------------------------------------------
    UpdateFlagType = function ( self )
    
        local textureName;
        local maskName;
        
		local pUnit = Players[ self.m_PlayerID ]:GetUnitByID( self.m_UnitID );
        if pUnit == nil then
			return;
		end	
		
		if self.m_IsGarrisoned and not pUnit:IsGarrisoned() then
			self.m_IsGarrisoned = false;
			if InStrategicView() then
				self.m_Instance.Anchor:SetHide(true);
			end
		end
			
        if( pUnit:IsEmbarked() ) then 
            textureName = "UnitFlagEmbark.dds";
            maskName = "UnitFlagEmbarkMask.dds";
            self.m_Instance.UnitIconShadow:SetOffsetVal( -1, -2 );
        elseif( pUnit:IsGarrisoned() ) then
            textureName = "UnitFlagBase.dds";
            maskName = "UnitFlagMask.dds";
            self.m_Instance.UnitIconShadow:SetOffsetVal( -1, 1 );
        elseif( pUnit:GetFortifyTurns() > 0 ) then
            --[[
            if( pUnit:isRanged() )
            then
                -- need art for this
                textureName = "UnitFlagRanged.dds";
                maskName = "UnitFlagRangedMask.dds";
                self.m_Instance.UnitIconShadow:SetOffsetVal( 0, 0 );
            else
            --]]
                textureName = "UnitFlagFortify.dds";
                maskName = "UnitFlagFortifyMask.dds";
                self.m_Instance.UnitIconShadow:SetOffsetVal( -1, 0 );
            --end

        elseif( self.m_IsTrade ) then
            textureName = "UnitFlagTrade.dds";
            maskName = "UnitFlagTradeMask.dds";
            self.m_Instance.UnitIconShadow:SetOffsetVal( -1, 0 );

            
        elseif( not self.m_IsCivilian ) then
            textureName = "UnitFlagBase.dds";
            maskName = "UnitFlagMask.dds";
            self.m_Instance.UnitIconShadow:SetOffsetVal( -1, 0 );
        else
            textureName = "UnitFlagCiv.dds";
            maskName = "UnitFlagCivMask.dds";
            self.m_Instance.UnitIconShadow:SetOffsetVal( -1, -3 );
        end
        
        self.m_Instance.UnitIconShadow:ReprocessAnchoring();
        
        
        self.m_Instance.FlagShadow:SetTexture( textureName );
        self.m_Instance.FlagBase:SetTexture( textureName );
        self.m_Instance.FlagBaseOutline:SetTexture( textureName );
        self.m_Instance.NormalSelect:SetTexture( textureName );
        self.m_Instance.HealthBarSelect:SetTexture( textureName );
        self.m_Instance.LightEffect:SetTexture( textureName );
        self.m_Instance.HealthBarBG:SetTexture( textureName );
        self.m_Instance.NormalAlphaAnim:SetTexture( textureName );
        self.m_Instance.HealthBarAlphaAnim:SetTexture( textureName );
        
        self.m_Instance.NormalScrollAnim:SetMask( maskName );
        self.m_Instance.HealthBarScrollAnim:SetMask( maskName );
    end,


    ------------------------------------------------------------------
    ------------------------------------------------------------------
    UpdateName = function ( self )
            
		local pUnit = Players[ self.m_PlayerID ]:GetUnitByID( self.m_UnitID );
        if pUnit == nil then
			return;
		end	

        local pPlayer = Players[Game.GetActivePlayer()];
        local active_team = pPlayer:GetTeam();
        local team = self.m_Player:GetTeam();

		local unitNameString;
		if(pUnit:HasName()) then
			local desc = Locale.ConvertTextKey("TXT_KEY_PLOTROLL_UNIT_DESCRIPTION_CIV",  self.m_Player:GetCivilizationAdjectiveKey(), pUnit:GetNameKey());
			unitNameString = string.format("%s[NEWLINE](%s)", Locale.Lookup(pUnit:GetNameNoDesc()), desc);
		else
			unitNameString = Locale.ConvertTextKey("TXT_KEY_PLOTROLL_UNIT_DESCRIPTION_CIV",  self.m_Player:GetCivilizationAdjectiveKey(), pUnit:GetNameKey());
		end

        if( active_team == team ) then
            local string;
            if(PreGame.IsMultiplayerGame() and self.m_Player:IsHuman()) then
                string = Locale.ConvertTextKey("TXT_KEY_MULTIPLAYER_UNIT_TT", self.m_Player:GetNickName(), self.m_Player:GetCivilizationAdjectiveKey(), pUnit:GetNameKey() );
			else
				string = unitNameString; 
            end
            
            local eReligion = pUnit:GetReligion();
			if (eReligion > ReligionTypes.RELIGION_PANTHEON) then
				string = string .. " - " .. Locale.Lookup(Game.GetReligionName(eReligion));
			end
			
			if( playerID == Game.GetActivePlayer() ) then
                string = string .. Locale.ConvertTextKey( "TXT_KEY_UPANEL_CLICK_TO_SELECT" );
			end
			self.m_Instance.UnitIcon:SetToolTipString( string );
        else
            if(PreGame.IsMultiplayerGame() and self.m_Player:IsHuman()) then
				self.m_Instance.UnitIcon:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_MULTIPLAYER_UNIT_TT", self.m_Player:GetNickName(), self.m_Player:GetCivilizationAdjectiveKey(), pUnit:GetNameKey()));
			else
				self.m_Instance.UnitIcon:SetToolTipString(unitNameString); 
            end
        end
		
    end,


    ------------------------------------------------------------------
    ------------------------------------------------------------------
    CheckEscort = function( self )
        --if( nil == nil ) then return; end
        
		local pUnit = Players[ self.m_PlayerID ]:GetUnitByID( self.m_UnitID );
        if pUnit == nil then
			return;
		end
		
		if (pUnit:GetDomainType() == DomainTypes.DOMAIN_AIR) then
			if (pUnit:IsImmobile()) then
		        if( g_PrintDebug ) then print( string.format( "Immobile air unit... not checking for escort!") ); end
		        
				return;
			end
		end
			
        local plot = pUnit:GetPlot();
        if( plot == nil )
        then 
            return
        end
        
        local escortUnit = nil;
        
        -- check current hex for other units
        if( plot:GetNumUnits() > 1 )
        then
            for i = 0, plot:GetNumUnits() - 1, 1
            do
		        if( g_PrintDebug ) then print( string.format( "Determining Escort for: Player[%i] Unit[%i] - %i", self.m_PlayerID, self.m_UnitID, i ) ); end
				
                local test = plot:GetUnit( i );
                if( test ~= nil and test:GetID() ~= self.m_UnitID and not test:IsGarrisoned() and not test:IsDead() and not test:IsDelayedDeath() )
                then
					escortUnit = test;
					break;
                end
            end
        
        end
        
       
        -- we have a new escort
        if( escortUnit ~= nil )
        then
            local playerTable = g_MasterList[ escortUnit:GetOwner() ];
            if( playerTable == nil )
            then
                if( g_PrintDebug ) then print( "Escort Unit found, but no flag built yet" ); end
                return;
            end
            
            local escortFlag = playerTable[ escortUnit:GetID() ];
            if( escortFlag == nil )
            then
                if( g_PrintDebug ) then print( "Escort Unit found, but no flag built yet" ); end
                return;
            end
            
            -- we had an old escort 
            if( self.m_Escort ~= nil and escortFlag ~= self.m_Escort )
            then
                if( g_PrintDebug ) then print( "informing old escort of our new plans " ); end
                self.m_Escort.m_IsForceHide = false;
                self.m_Escort:UpdateVisibility();
                self.m_Escort:SetEscort( nil );
            end
            
            if( g_PrintDebug ) then print( "Setting Escort to " .. tostring(escortFlag.m_PlayerID) .. " " .. tostring(escortFlag.m_UnitID)); end
            
            self:SetEscort( escortFlag );
            escortFlag:SetEscort( self );
         
        -- we do not have a new escort, but we had an old one
        elseif ( self.m_Escort ~= nil )
        then
            if( g_PrintDebug ) then print( "Clearing Both Escorts: " .. " " .. tostring( self.m_PlayerID ) .. " " .. tostring( self.m_UnitID ) ); end
            self.m_Escort.m_IsForceHide = false;
			self.m_Escort:UpdateVisibility();
            self.m_Escort:SetEscort( nil );
            self:SetEscort( nil );
        end   
    end,
   
    
    
    ------------------------------------------------------------------
    -- used by CheckEscort to maintain the Other Unit
    ------------------------------------------------------------------
    SetEscort = function( self, newEscort )
        
        if( newEscort ~= self.m_Escort ) then
			-- Not the same escort?  Break any link the escort has to us now.
			-- This keeps it clean and simple and prevents issues when there are 2+ units in a hex
			-- which can happen, before a forced stack move.
			if ( self.m_Escort and self.m_Escort.m_Escort == self ) then
				-- I don't want this to be recursive, so manually disconnect.
				self.m_Escort.m_Escort = nil;
		        self.m_Escort:UpdateFlagOffset();
			end
        end
       
        self.m_Escort = newEscort;
            
        self:UpdateFlagOffset()
    end,
    
    
    -----------------------------------------------------------------
    ------------------------------------------------------------------
    UpdateCargo = function( self )
        
		local pUnit = Players[ self.m_PlayerID ]:GetUnitByID( self.m_UnitID );
        if( pUnit == nil or pUnit:CargoSpace() <= 0 ) then
            return;
        end
        
        if( self.m_CargoControls == nil )  then
            print( "bad cargo controls... ? ? ?" );
            return;
        end
        
        
        if( not pUnit:HasCargo() ) then
            self.m_CargoControls.PullDown:SetHide( true );
        else
            self.m_CargoControls.PullDown:SetHide( false );
            self.m_CargoControls.PullDown:ClearEntries();
            
            if( self.m_Instance.HealthBar:IsHidden() == false ) then
                self.m_CargoControls.PullDown:SetOffsetX( g_AirFlagHealthOffset );
            else
                self.m_CargoControls.PullDown:SetOffsetX( g_AirFlagNormalOffset );
            end
            local cargoCount = 0;
            
            -----------------------------------------------
            -- button for the carrier
            controlTable = {};
            self.m_CargoControls.PullDown:BuildEntry( "UnitInstance", controlTable );
            controlTable.Button:SetText( pUnit:GetName() );
            controlTable.Button:SetVoids( self.m_playerID, self.m_UnitID );
            if( pUnit:MovesLeft() > 0 ) then
                controlTable.Button:SetAlpha( 1.0 );
            else
                controlTable.Button:SetAlpha( 0.6 );
            end

            self.m_CargoIsActive = false;
            local pPlot = pUnit:GetPlot();
            local numUnits = pPlot:GetNumUnits();
            for i = 0, numUnits - 1 do
            
                local pPlotUnit = pPlot:GetUnit( i );
                if( pPlotUnit:IsCargo() and pPlotUnit:GetTransportUnit():GetID() == self.m_UnitID ) then
                
                    -- we're carrying
                    controlTable = {};
                    self.m_CargoControls.PullDown:BuildEntry( "UnitInstance", controlTable );
                    
                    controlTable.Button:SetText( pPlotUnit:GetName() );
                    controlTable.Button:SetVoids( self.m_playerID, pPlotUnit:GetID() );
                    
                    if( pPlotUnit:MovesLeft() > 0 ) then
                        self.m_CargoIsActive = true;
                        controlTable.Button:SetAlpha( 1.0 );
                    else
                        controlTable.Button:SetAlpha( 0.6 );
                    end
                    cargoCount = cargoCount + 1;
                    -- Make sure the cargo flag has a link back to us
    				local pCargoFlag = g_MasterList[ pPlotUnit:GetOwner() ][ pPlotUnit:GetID() ];
    				-- Check to see if it is there, it is possible it has yet to be created
    				if (pCargoFlag ~= nil) then
    					pCargoFlag.m_CarrierFlag = self;
    				end                    
                end
            end
            
            self.m_CargoControls.Count:SetText( cargoCount );
			self.m_CargoControls.PullDown:SetToolTipString( pPlot:GetAirUnitsTooltip() )
            --self.m_CargoControls.PullDown:LocalizeAndSetToolTip( "TXT_KEY_STATIONED_AIRCRAFT", cargoCount );
            self.m_CargoControls.PullDown:CalculateInternals();
            
            if( self.m_CargoIsActive == false ) then
                self.m_CargoControls.PullDown:GetButton():SetAlpha( g_DimAirAlpha );
            end
        end
    end,



    -----------------------------------------------------------------
    ------------------------------------------------------------------
    SetFlash = function( self, bFlashOn )
        self.m_Instance.UnitIconAnim:SetToBeginning();
        
        if( bFlashOn ) then
            self.m_Instance.UnitIconAnim:Play();
        end
        
    end,
    
    
    -----------------------------------------------------------------
    ------------------------------------------------------------------
    SetDim = function( self, bDim )
		self.m_IsDimmed = bDim;
        self:UpdateDimmedState();
        
		local pUnit = Players[ self.m_PlayerID ]:GetUnitByID( self.m_UnitID );
        if( pUnit:IsCargo() and self.m_CarrierFlag ~= nil ) then
		    self.m_CarrierFlag:UpdateCargo();
	    elseif( self.m_CargoControls ~= nil ) then
	        self:UpdateCargo();
        end
        
        if( self.m_CityFlag ~= nil ) then
            UpdateCityCargo( pUnit:GetPlot() );
        end
    end,
   
    
    -----------------------------------------------------------------
    ------------------------------------------------------------------
    OverrideDimmedFlag = function( self, bOverride )
		self.m_OverrideDimmedFlag = bOverride;
        self:UpdateDimmedState();
    end,
    
    
    -----------------------------------------------------------------
    ------------------------------------------------------------------
    UpdateDimmedState = function( self )
		if( self.m_IsDimmed and not self.m_OverrideDimmedFlag ) then
            self.m_Instance.Anchor:SetAlpha( g_DimAlpha );
            self.m_Instance.HealthBar:SetAlpha( 1.0 / g_DimAlpha ); -- Health bar doesn't get dimmed (Hacky I know)
            
            if( self.m_CargoControls ~= nil ) then
                if( self.m_CargoIsActive ) then
                    self.m_CargoControls.PullDown:GetButton():SetAlpha( 1.0 / g_DimAlpha ); -- active cargo doesn't get dimmed (Hacky I know)
                    self.m_CargoControls.Count:SetAlpha( 1.0 / g_DimAlpha );
                else
                    self.m_CargoControls.PullDown:GetButton():SetAlpha( 1.0 );
                    self.m_CargoControls.Count:SetAlpha( 1.0 );
                end
            end
        else
            self.m_Instance.Anchor:SetAlpha( 1.0 );
            self.m_Instance.HealthBar:SetAlpha( 1.0 );
            
            if( self.m_CargoControls ~= nil ) then
                if( self.m_CargoIsActive ) then
                    self.m_CargoControls.PullDown:GetButton():SetAlpha( 1.0 );
                else
                    self.m_CargoControls.PullDown:GetButton():SetAlpha( g_DimAirAlpha );
                end
            end
        end
    end,


    -----------------------------------------------------------------
    ------------------------------------------------------------------
    UnitMove = function( self, posX, posY, posZ )
    
        self:CheckEscort();
        self:UpdateFlagOffset();
           
        ------------------------
        if( g_PrintDebug ) then print( "Setting flag position" ); end
        self.m_Instance.Anchor:SetWorldPositionVal( posX, posY, posZ );
        
        if( self.m_HasCivilianSelectFlag )
        then
            if( g_PrintDebug ) then print( "Updating select flag pos" ); end
            --g_CivilianSelectFlag.m_Instance.Anchor:SetWorldPositionVal( posX, posY, posZ );
        end
        
    end,



    ------------------------------------------------------------------
    ------------------------------------------------------------------
	UpdateFlagOffset = function( self )
	
		local pUnit = Players[ self.m_PlayerID ]:GetUnitByID( self.m_UnitID );
        if pUnit == nil then
			return;
		end	

        local plot = pUnit:GetPlot();
        
        if plot == nil then
			return;
        end
        
        local offset = Vector2( 0, 0 );
        	            
        if pUnit:IsGarrisoned() then
            if( Game.GetActiveTeam() == Players[ self.m_PlayerID ]:GetTeam() ) then
				offset = GarrisonOffset;
			else
				offset = GarrisonOtherOffset;
            end 
            
            -- When garrisoned, we want to line up the icon with the city banner.  Some units sit at different heights, so repostion the icon world position to match the city banner
        	local worldPos = Vector4( GridToWorld( pUnit:GetX(), pUnit:GetY() ) );
        	self.m_Instance.Anchor:SetWorldPosition( VecAdd( worldPos, CityWorldPositionOffset ) );
            
        elseif plot:IsCity() then
            offset = CityNonGarrisonOffset;
        end

        if( self.m_CarrierFlag ~= nil ) then
            offset = VecAdd( offset, CivilianOffset );
            
        -- if we're a civilian and there's someone else with us            
        elseif( self.m_IsCivilian and self.m_Escort ~= nil ) then
            if( not self.m_Escort.m_IsCivilian or -- if they are also a civilian try to only offset one of us
                self.m_UnitID < self.m_Escort.m_UnitID ) then
                offset = VecAdd( offset, CivilianOffset );
            end
        end
        
        -- set the ui offset
        self.m_Instance.FlagShadow:SetOffset( offset );
    end,
}

--g_CivilianSelectFlag = g_UnitFlagClass:new( -1 );
--ContextPtr:BuildInstance( "NewUnitFlag", g_CivilianSelectFlag.m_Instance );
--g_CivilianSelectFlag.m_Instance.Anchor:SetHide( true );

-------------------------------------------------
-- On Unit Created
-------------------------------------------------
function OnUnitCreated( playerID, unitID, hexVec, unitType, cultureType, civID, primaryColor, secondaryColor, unitFlagIndex, fogState, selected, military, notInvisible )
    if( Players[ playerID ] == nil or
        Players[ playerID ]:GetUnitByID( unitID ) == nil or
        Players[ playerID ]:GetUnitByID( unitID ):IsDead() )
    then
        return;
    end
    
    -- support for debug panel
    g_LastPlayerID = playerID;
    g_LastUnitID   = unitID;
    
    if( g_PrintDebug ) then print( "  Unit Created: " .. tostring( playerID ) .. " " .. tostring( unitID ) .. " " .. fogState ); end
    
    g_UnitFlagClass:new( playerID, unitID, fogState, not notInvisible );
end
Events.SerialEventUnitCreated.Add( OnUnitCreated );



-------------------------------------------------
-- On Unit Position Changed
-- sent by the engine while it walks a unit around
-------------------------------------------------
function OnUnitPositionChanged( playerID, unitID, unitPosition )
    if( Players[ playerID ] == nil or
		not Players[ playerID ]:IsAlive() )
        --Players[ playerID ]:GetUnitByID( unitID ) == nil or
        --Players[ playerID ]:GetUnitByID( unitID ):IsDead() )
    then
        return;
    end
    
    if( g_MasterList[ playerID ] == nil or
        g_MasterList[ playerID ][ unitID ] == nil ) 
    then
        --print( string.format( "Unit not found for OnUnitMove: Player[%i] Unit[%i]", playerID, unitID ) );
    else                
		local pUnit = Players[ playerID ]:GetUnitByID( unitID );
		if (pUnit ~= nil and pUnit:IsGarrisoned() ) then			
            -- When garrisoned, we want to line up the icon with the city banner.  
            -- Some units sit at different heights, so repostion the icon world position to match the city banner
        	local worldPosX, worldPosY, worldPosZ = GridToWorld( pUnit:GetX(), pUnit:GetY() );
        	unitPosition.z = worldPosZ;
        end
        
        local position = VecAdd( unitPosition, CityWorldPositionOffset );
                	
        g_MasterList[ playerID ][ unitID ]:UnitMove( position.x, position.y, position.z );
    end
end
Events.LocalMachineUnitPositionChanged.Add( OnUnitPositionChanged );



--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
function OnFlagTypeChange( playerID, unitID )
        
    if( Players[ playerID ] == nil or
		not Players[ playerID ]:IsAlive() or
        Players[ playerID ]:GetUnitByID( unitID ) == nil or
        Players[ playerID ]:GetUnitByID( unitID ):IsDead() )
    then
        return;
    end
    
    if( g_MasterList[ playerID ] == nil or
        g_MasterList[ playerID ][ unitID ] == nil ) 
    then
        --print( string.format( "Unit not found for OnFlagTypeChange: Player[%i] Unit[%i]", playerID, unitID ) );
    else
        local unit = g_MasterList[ playerID ][ unitID ];
        unit:UpdateFlagType();
        unit:UpdateFlagOffset();
    end
end
Events.UnitActionChanged.Add( OnFlagTypeChange );
Events.UnitGarrison.Add( OnFlagTypeChange );
Events.UnitEmbark.Add( OnFlagTypeChange );


-------------------------------------------------
-- nukes teleport instead of moving
-------------------------------------------------
function OnUnitTeleported( i, j, playerID, unitID )
    -- spoof out the move queue changed logic.
    OnUnitMoveQueueChanged( playerID, unitID, false );
end
Events.SerialEventUnitTeleportedToHex.Add( OnUnitTeleported );
	
	
-------------------------------------------------
-- On Unit Move Queue Changed
-------------------------------------------------
function OnUnitMoveQueueChanged( playerID, unitID, bRemainingMoves )

	if( not bRemainingMoves )then

		local thisUnit = Players[ playerID ]:GetUnitByID( unitID );
		if( thisUnit ~= nil and g_MasterList[ playerID ] ~= nil ) then

			local flag = g_MasterList[ playerID ][ unitID ];
			
			if( flag ~= nil ) then

   				flag:GarrisonComplete( thisUnit:IsGarrisoned() );

    			if( flag.m_CarrierFlag ~= nil ) then
    			    flag.m_CarrierFlag:UpdateCargo();
    		    end

    			if( thisUnit:IsCargo() ) then
    				flag.m_CarrierFlag = g_MasterList[ playerID ][ thisUnit:GetTransportUnit():GetID() ];
    			    
    			    if( flag.m_CarrierFlag ~= nil ) then
        			    flag.m_CarrierFlag:UpdateCargo();
    			    end
    		    else
    		        flag.m_CarrierFlag = nil;
    		    end


    		    local pPlot = thisUnit:GetPlot();
    			if( flag.m_CityFlag ~= nil ) then
    				UpdateCityCargo( g_CityFlagPlots[ flag.m_CityFlag ] );
    			    flag.m_CityFlag = nil;
    			end

    		    if( pPlot:IsCity() ) then
    			    flag.m_CityFlag = UpdateCityCargo( pPlot );
    			end

    		    flag:UpdateFlagOffset();
    		end
		end
	end
end
Events.UnitMoveQueueChanged.Add( OnUnitMoveQueueChanged );


-------------------------------------------------
-------------------------------------------------
function UpdateCityCargo( pPlot )
    --if( not pPlot:IsCity() ) then
        --return;
    --end
    
    local cargoCount = 0;
    local numUnits = pPlot:GetNumUnits();
    
    -- count the air units
    for i = 0, numUnits - 1 do
        local pPlotUnit = pPlot:GetUnit( i );
        if( pPlotUnit:GetDomainType() == DomainTypes.DOMAIN_AIR ) then
            cargoCount = cargoCount + 1;
        end
    end
    
    local cityFlagInstance = g_CityFlags[ pPlot:GetX() .. " " .. pPlot:GetY() ];
    if( cargoCount > 0 and pPlot:IsCity() ) then
        if( cityFlagInstance == nil ) then
            cityFlagInstance = {};
            ContextPtr:BuildInstanceForControl( "CityFlag", cityFlagInstance, Controls.CityContainer );
            cityFlagInstance.PullDown:RegisterSelectionCallback( UnitFlagClicked );
            g_CityFlags[ pPlot:GetX() .. " " .. pPlot:GetY() ] = cityFlagInstance;
            g_CityFlagPlots[ cityFlagInstance ] = pPlot;
        		        
        	local worldPos = Vector4( GridToWorld( pPlot:GetX(), pPlot:GetY() ) );
        	cityFlagInstance.Anchor:SetWorldPosition( VecAdd( worldPos, CityWorldPositionOffset ) );
	    end

        cityFlagInstance.PullDown:ClearEntries();
        
        -- actually add the air units
        for i = 0, numUnits - 1 do
            local pPlotUnit = pPlot:GetUnit( i );
            if( pPlotUnit:GetDomainType() == DomainTypes.DOMAIN_AIR ) then
            
                controlTable = {};
                cityFlagInstance.PullDown:BuildEntry( "UnitInstance", controlTable );
                
               -- 
				local unitName = pPlotUnit:GetName();
               
				local bw, bh = controlTable.Button:GetSizeVal();
                controlTable.Button:SetText(unitName);
                local tw, th = controlTable.Button:GetTextControl():GetSizeVal();
                
                local newWidth = math.max(150, tw+50);
                
                controlTable.Button:SetSizeVal(newWidth,bh);
                controlTable.Button:SetText(unitName);
              
                controlTable.Button:ReprocessAnchoring();
                controlTable.Button:SetVoids( pPlotUnit:GetOwner(), pPlotUnit:GetID() );
                
                if( pPlotUnit:MovesLeft() > 0 ) then
                    controlTable.Button:SetAlpha( 1.0 );
                else
                    controlTable.Button:SetAlpha( 0.6 );
                end
            end
        end
    
        cityFlagInstance.Count:SetText( cargoCount );
		cityFlagInstance.PullDown:SetToolTipString( pPlot:GetAirUnitsTooltip() )
        --cityFlagInstance.PullDown:LocalizeAndSetToolTip( "TXT_KEY_STATIONED_AIRCRAFT", cargoCount );
        cityFlagInstance.PullDown:CalculateInternals();
        cityFlagInstance.PullDown:SetHide( false );
        
        return cityFlagInstance;
    else
        if( cityFlagInstance ~= nil ) then
			cityFlagInstance.PullDown:SetHide( true );
        end
    end
end 


-------------------------------------------------
-------------------------------------------------
function OnCityCreated( hexPos, playerID, cityID, cultureType, eraType, continent, populationSize, size, fowState )
    local gridPosX, gridPosY = ToGridFromHex( hexPos.x, hexPos.y );
    --print( "gridPosX, gridPosY: "..tostring(gridPosX)..","..tostring(gridPosY) );
    local pCityFlag = UpdateCityCargo( Map.GetPlot( gridPosX, gridPosY ) );
    
    -- Set the visibility
    if (pCityFlag ~= nil) then
		local bInvisible = fowState ~= WhiteFog;
		
		pCityFlag.Anchor:SetHide( bInvisible );
   	end
    
end
Events.SerialEventCityCreated.Add( OnCityCreated );


-------------------------------------------------
-- On Unit Garrison
-------------------------------------------------
function OnUnitGarrison( playerID, unitID, bGarrisoned )
	if not UnitMoving(playerID, unitID) and g_MasterList[ playerID ] ~= nil then
		local flag = g_MasterList[ playerID ][ unitID ];
		if flag ~= nil then
			flag:GarrisonComplete(bGarrisoned);
		end
	end
end
Events.UnitGarrison.Add( OnUnitGarrison );


-------------------------------------------------
-- On Unit Destroyed
-------------------------------------------------
function OnUnitVisibility( playerID, unitID, visible, checkFlag, blendTime )
	if checkFlag then
		if( g_MasterList[ playerID ] ~= nil and
			g_MasterList[ playerID ][ unitID ] ~= nil ) then
	        
			local flag = g_MasterList[ playerID ][ unitID ];
			flag.m_IsInvisible = not visible;
			flag:UpdateVisibility();
		end
	end
end
Events.UnitVisibilityChanged.Add( OnUnitVisibility );


-------------------------------------------------
-- On Unit Destroyed
-------------------------------------------------
function OnUnitDestroyed( playerID, unitID )
    if( g_MasterList[ playerID ] == nil or
        g_MasterList[ playerID ][ unitID ] == nil ) 
    then
        --print( string.format( "Unit not found for OnUnitDestroyed: Player[%i] Unit[%i]", playerID, unitID ) );
    else
        g_MasterList[ playerID ][ unitID ]:destroy();
    end
end
Events.SerialEventUnitDestroyed.Add( OnUnitDestroyed );


-------------------------------------------------
-- On Flag Clicked
-------------------------------------------------
function UnitFlagClicked( playerID, unitID )
    Events.SerialEventUnitFlagSelected( playerID, unitID );
end


function UnitFlagEnter( playerID, unitID )
    if( g_MasterList[ playerID ] ~= nil and
        g_MasterList[ playerID ][ unitID ] ~= nil ) then
        
        g_MasterList[ playerID ][ unitID ]:OverrideDimmedFlag( true );
    end
end

function UnitFlagExit( playerID, unitID )
    if( g_MasterList[ playerID ] ~= nil and
        g_MasterList[ playerID ][ unitID ] ~= nil ) then
        
        local flag = g_MasterList[ playerID ][ unitID ];
        flag:OverrideDimmedFlag( flag.m_IsSelected );
    end
end



-------------------------------------------------
-------------------------------------------------
function OnUnitSelect( playerID, unitID, i, j, k, isSelected )

    if( Players[ playerID ] == nil or
		not Players[ playerID ]:IsAlive() or
        Players[ playerID ]:GetUnitByID( unitID ) == nil or
        Players[ playerID ]:GetUnitByID( unitID ):IsDead() )
    then
        return;
    end
    
    if( g_MasterList[ playerID ] == nil or
        g_MasterList[ playerID ][ unitID ] == nil ) 
    then
        print( string.format( "Unit not found for OnUnitSelect: Player[%i] Unit[%i]", playerID, unitID ) );
    else
        g_MasterList[ playerID ][ unitID ]:UpdateSelected( isSelected );
    end
end
Events.UnitSelectionChanged.Add( OnUnitSelect );



--------------------------------------------------------------------------------
-- Unit SetDamage was called - we only enter this function if the amount of damage actually changed
--------------------------------------------------------------------------------
function OnUnitSetDamage( playerID, unitID, iDamage, iPreviousDamage )
    if( Players[ playerID ] == nil or
		not Players[ playerID ]:IsAlive() or
        Players[ playerID ]:GetUnitByID( unitID ) == nil or
        Players[ playerID ]:GetUnitByID( unitID ):IsDead() )
    then
        return;
    end
    
    if( g_MasterList[ playerID ] == nil or
        g_MasterList[ playerID ][ unitID ] == nil ) 
    then
        --print( "Unit not found for OnUnitSetDamage: Player[" .. tostring( playerID ) .. "] Unit[" .. tostring( unitID ) .. "]" );
    else
        g_MasterList[ playerID ][ unitID ]:UpdateHealth();
    end
end
Events.SerialEventUnitSetDamage.Add( OnUnitSetDamage );

--------------------------------------------------------------------------------
-- A unit has changed its name, update the tool tip string
--------------------------------------------------------------------------------
function OnUnitNameChanged( playerID, unitID )
    if( Players[ playerID ] == nil or
		not Players[ playerID ]:IsAlive() or
        Players[ playerID ]:GetUnitByID( unitID ) == nil or
        Players[ playerID ]:GetUnitByID( unitID ):IsDead() )
    then
        return;
    end
    
    if( g_MasterList[ playerID ] ~= nil ) then
		local pFlag = g_MasterList[ playerID ][ unitID ];
		if (pFlag ~= nil) then
			pFlag:UpdateName();
		end
    end
end
Events.UnitNameChanged.Add( OnUnitNameChanged );


------------------------------------------------------------
-- this goes off when a hex is seen or unseen
------------------------------------------------------------
function OnHexFogEvent( hexPos, fogState, bWholeMap )
    if( bWholeMap ) 
    then
        for playerID,playerTable in pairs( g_MasterList ) 
        do
            for unitID,flag in pairs( playerTable ) 
            do
                flag:SetFogState( fogState );
            end
        end

		-- Do the city flags        
		local bInvisible = fogState ~= WhiteFog;
		for cityIndex, pCityFlag in pairs(g_CityFlags) do
			pCityFlag.Anchor:SetHide( bInvisible );
		end
    else
        local gridVecX, gridVecY = ToGridFromHex( hexPos.x, hexPos.y );
        local plot = Map.GetPlot( gridVecX, gridVecY );
        
        if( plot ~= nil ) then
        
            local unitCount = plot:GetNumLayerUnits();	-- Get all layers, so we update the trade units as well
            
            for i = 0, unitCount - 1, 1
            do
                local unit = plot:GetLayerUnit( i );	-- Get all layers, so we update the trade units as well
                if( unit ~= nil ) 
                then
                    local owner, unitID = unit:GetOwner(), unit:GetID();
                    if( g_MasterList[ owner ] ~= nil and
                        g_MasterList[ owner ][ unitID ] ~= nil )
                    then
                        --print( " FOG OF WAR'd!! " .. owner .. " " .. unitID .. " " .. fogState );
                        g_MasterList[ owner ][ unitID ]:SetFogState( fogState );
                    end
                end
            end
            
            -- Do city flag
			local bInvisible = fogState ~= WhiteFog;
			for cityIndex, pCityFlag in pairs( g_CityFlags )
			do				
				-- The flag doesn't seem to store its plot, so we have to search using the instance to plot map.
				local pCityFlagPlot = g_CityFlagPlots[ pCityFlag ];
				if (pCityFlagPlot == plot ) then
					pCityFlag.Anchor:SetHide( bInvisible );
					break;
				end				
			end
        end
    end
end
Events.HexFOWStateChanged.Add( OnHexFogEvent );




--------------------------------------------------------------------------------
-- Update the name of all unit flags.
--------------------------------------------------------------------------------
function UpdateAllFlagNames()
        for playerID,playerTable in pairs( g_MasterList ) do
            for unitID,flag in pairs( playerTable ) do
                flag:UpdateName( fogState );
            end
        end
end
-- We have to update all flags because we don't know which player was updated (disconnected/reconnected/etc).
Events.MultiplayerGamePlayerUpdated.Add(UpdateAllFlagNames);


------------------------------------------------------------
-- this goes off when a unit moves into or out of the fog
------------------------------------------------------------
function OnUnitFogEvent( playerID, unitID, fogState )
    if( Players[ playerID ] == nil or
		not Players[ playerID ]:IsAlive() or
        Players[ playerID ]:GetUnitByID( unitID ) == nil or
        Players[ playerID ]:GetUnitByID( unitID ):IsDead() )
    then
        return;
    end

    playerTable = g_MasterList[ playerID ];
    if( playerTable ~= nil ) 
    then
        local flag = playerTable[ unitID ];
        if( flag ~= nil ) 
        then
            flag:SetFogState( fogState );
        end
    end
end
Events.UnitStateChangeDetected.Add( OnUnitFogEvent );



------------------------------------------------------------
-- this goes off when gameplay decides a unit's flag should be dimmed or not
------------------------------------------------------------
function OnDimEvent( playerID, unitID, bDim )
    if( Players[ playerID ] == nil or
		not Players[ playerID ]:IsAlive() or
        Players[ playerID ]:GetUnitByID( unitID ) == nil or
        Players[ playerID ]:GetUnitByID( unitID ):IsDead() )
    then
        return;
    end

    playerTable = g_MasterList[ playerID ];
    if( playerTable ~= nil ) 
    then
        local flag = playerTable[ unitID ];
        if( flag ~= nil ) 
        then
        	local active_team = Players[Game.GetActivePlayer()]:GetTeam();
        	local team = Players[playerID]:GetTeam();
        	
        	if( active_team == team )
        	then
                --print( "  Unit dim: " .. tostring( playerID ) .. " " .. tostring( unitID ) .. " " .. iDim );
                flag:SetDim( bDim  );
        	end
        end
    end
end
Events.UnitShouldDimFlag.Add( OnDimEvent );



------------------------------------------------------------
-- this goes off when gameplay decides a unit is threatening and wants it marked
------------------------------------------------------------
function OnMarkThreateningEvent( playerID, unitID, bMark )
	
	--print("Marking Unit as Threatening: " .. tostring(playerID) .. ", " .. tostring(unitID) .. ", " .. tostring(bMark));
	
    if( Players[ playerID ] == nil or
		not Players[ playerID ]:IsAlive() or
        Players[ playerID ]:GetUnitByID( unitID ) == nil or
        Players[ playerID ]:GetUnitByID( unitID ):IsDead() )
    then
        return;
    end

    playerTable = g_MasterList[ playerID ];
    if( playerTable ~= nil ) 
    then
        local flag = playerTable[ unitID ];
        if( flag ~= nil ) 
        then
            --print( "  Unit mark threatening: " .. tostring( playerID ) .. " " .. tostring( unitID ) .. " " .. tostring(bMark) );
            flag:SetFlash(bMark);
        end
    end
end
Events.UnitMarkThreatening.Add( OnMarkThreateningEvent );










-- -------------------------------------------------------
-- temp to make updating units easier
-- -------------------------------------------------------
g_LastPlayerID = 0;
g_LastUnitID   = 0;



------------------------------------------------------------
-- scan for all cities when we are loaded
-- this keeps the banners from disappearing on hotload
------------------------------------------------------------
if( ContextPtr:IsHotLoad() )
then
    local i = 0;
    local player = Players[i];
    while player ~= nil 
    do
        if( player:IsAlive() )
        then
            for unit in player:Units() do
                local plot = Map.GetPlot( unit:GetX(), unit:GetY() );
                if( plot ~= nil ) then
                    if( plot:IsVisible( Game.GetActiveTeam() ) ) then
                        OnUnitCreated( player:GetID(), unit:GetID(), 0, 0, 0, 0, 0, 0, 0, WhiteFog, 0, 0, true );
                    end
                end
            end
        end

        i = i + 1;
        player = Players[i];
    end
end

------------------------------------------------------------
------------------------------------------------------------
function OnStrategicViewStateChanged( bStrategicView, bCityBanners )
	g_GarrisonedUnitFlagsInStrategicView = bStrategicView and bCityBanners;
	for playerID,playerTable in pairs( g_MasterList ) 
	do
		for unitID,flag in pairs( playerTable ) 
		do
			flag:UpdateVisibility();
		end
	end
end
Events.StrategicViewStateChanged.Add(OnStrategicViewStateChanged);


------------------------------------------------------------
------------------------------------------------------------
function OnCityDestroyed( hexPos, playerID, cityID )
    local pPlot = Map.GetPlot( ToGridFromHex( hexPos.x, hexPos.y ) );
    if( pPlot ~= nil ) then
        local count = pPlot:GetNumUnits();
        for i = 0, count - 1 do
            local pUnit = pPlot:GetUnit( i );
            if( pUnit ~= nil ) then
                local playerTable = g_MasterList[ pUnit:GetOwner() ];
                if( playerTable ~= nil ) then
                    local pFlag = playerTable[ pUnit:GetID() ];
                    if( pFlag ~= nil ) then
                        pFlag:UpdateFlagOffset();
                    end
                end
            end
        end
        
        UpdateCityCargo( pPlot );
    end
end
Events.SerialEventCityDestroyed.Add(OnCityDestroyed);
Events.SerialEventCityCaptured.Add(OnCityDestroyed);


------------------------------------------------------------
------------------------------------------------------------
function OnCombatBegin( m_AttackerPlayer,
                        m_AttackerUnitID,
                        m_AttackerUnitDamage,
                        m_AttackerFinalUnitDamage,
                        m_AttackerMaxHitPoints,
                        m_DefenderPlayer,
                        m_DefenderUnitID,
                        m_DefenderUnitDamage,
                        m_DefenderFinalUnitDamage,
                        m_DefenderMaxHitPoints, 
                        m_bContinuation )
                        
    local playerTable = g_MasterList[ m_AttackerPlayer ];
    if( playerTable ~= nil ) 
    then
        local flag = playerTable[ m_AttackerUnitID ];
        if( flag ~= nil ) 
        then
			flag.m_IsForceHide = true;
            flag:UpdateVisibility();
            if( flag.m_Escort ~= nil ) then
                flag.m_Escort.m_IsForceHide = true;
                flag.m_Escort:UpdateVisibility();
            end
        end
    end
    
	playerTable = g_MasterList[ m_DefenderPlayer ];
    if( playerTable ~= nil ) 
    then
        local flag = playerTable[ m_DefenderUnitID ];
        if( flag ~= nil) 
        then
            flag.m_IsForceHide = true;
            flag:UpdateVisibility();
            if( flag.m_Escort ~= nil ) then
                flag.m_Escort.m_IsForceHide = true;
                flag.m_Escort:UpdateVisibility();
            end
        end
    end
end
Events.RunCombatSim.Add( OnCombatBegin );


------------------------------------------------------------
------------------------------------------------------------
function OnCombatEnd( m_AttackerPlayer,
                      m_AttackerUnitID,
                      m_AttackerUnitDamage,
                      m_AttackerFinalUnitDamage,
                      m_AttackerMaxHitPoints,
                      m_DefenderPlayer,
                      m_DefenderUnitID,
                      m_DefenderUnitDamage,
                      m_DefenderFinalUnitDamage,
                      m_DefenderMaxHitPoints )
                         
    local playerTable = g_MasterList[ m_AttackerPlayer ];
    if( playerTable ~= nil ) 
    then
        local flag = playerTable[ m_AttackerUnitID ];
        if( flag ~= nil ) 
        then
			flag.m_IsForceHide = false;
            flag:UpdateVisibility();
            if( flag.m_Escort ~= nil ) then
                flag.m_Escort.m_IsForceHide = false;
                flag.m_Escort:UpdateVisibility();
            end
        end
    end
    
	playerTable = g_MasterList[ m_DefenderPlayer ];
    if( playerTable ~= nil ) 
    then
        local flag = playerTable[ m_DefenderUnitID ];
        if( flag ~= nil ) 
        then
			flag.m_IsForceHide = false;
            flag:UpdateVisibility();
            if( flag.m_Escort ~= nil ) then
                flag.m_Escort.m_IsForceHide = false;
                flag.m_Escort:UpdateVisibility();
            end
        end
    end           
end
Events.EndCombatSim.Add( OnCombatEnd );


----------------------------------------------------------------
-- on shutdown, we need to get the selected flag instance back, 
-- or we'll assert clearing the registered button handler
function OnShutdown()
    -- doesn't really matter where we put it, as long as we own it again.
    if( g_SelectedFlag ~= nil ) then
        g_SelectedFlag.m_Instance.Anchor:ChangeParent( Controls.MilitaryFlags );
    end
end
ContextPtr:SetShutdown( OnShutdown );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnActivePlayerChanged( iActivePlayer, iPrevActivePlayer )

	local iActivePlayerID = Game.GetActivePlayer();
	
    if( g_SelectedFlag ~= nil ) then
        g_SelectedFlag.m_Instance.Anchor:ChangeParent( Controls.MilitaryFlags );
        g_SelectedFlag = nil;
    end
	
	-- Rebuild all the tool tip strings.
	for playerID,playerTable in pairs( g_MasterList ) 
	do
		local pPlayer = Players[ playerID ];
		
		local bIsActivePlayer = ( playerID == iActivePlayer );
				
		-- Only need to do this for human players
		if (pPlayer:IsHuman()) then	
			for unitID, pFlag in pairs( playerTable ) 
			do
				local pUnit = pPlayer:GetUnitByID( unitID );
				if ( pUnit ~= nil ) then        	
					local toolTipString;
					if (PreGame.IsMultiplayerGame()) then
						toolTipString = Locale.ConvertTextKey("TXT_KEY_MULTIPLAYER_UNIT_TT", pPlayer:GetNickName(), pPlayer:GetCivilizationAdjectiveKey(), pUnit:GetNameKey());
					else
						if (pUnit:HasName()) then
							local desc = Locale.ConvertTextKey("TXT_KEY_PLOTROLL_UNIT_DESCRIPTION_CIV",  pPlayer:GetCivilizationAdjectiveKey(), pUnit:GetNameKey());
							toolTipString = string.format("%s (%s)", Locale.Lookup(pUnit:GetName()), desc);
						else
							toolTipString = Locale.ConvertTextKey("TXT_KEY_PLOTROLL_UNIT_DESCRIPTION_CIV",  pPlayer:GetCivilizationAdjectiveKey(), pUnit:GetNameKey());
						end
					end

					local eReligion = pUnit:GetReligion();
					if (eReligion > ReligionTypes.RELIGION_PANTHEON) then
						toolTipString = toolTipString .. " - " .. Locale.ConvertTextKey(GameInfo.Religions[eReligion].Description);
					end

					if ( bIsActivePlayer ) then
						toolTipString = toolTipString .. Locale.ConvertTextKey( "TXT_KEY_UPANEL_CLICK_TO_SELECT" );
						pFlag.m_Instance.NormalButton:SetDisabled( false );
						pFlag.m_Instance.NormalButton:SetConsumeMouseOver( true );
						pFlag.m_Instance.HealthBarButton:SetDisabled( false );
						pFlag.m_Instance.HealthBarButton:SetConsumeMouseOver( true );
					else
						pFlag.m_Instance.NormalButton:SetDisabled( true );
						pFlag.m_Instance.NormalButton:SetConsumeMouseOver( false );
						pFlag.m_Instance.HealthBarButton:SetDisabled( true );
						pFlag.m_Instance.HealthBarButton:SetConsumeMouseOver( false );					
					end
					
					pFlag.m_Instance.UnitIcon:SetToolTipString( toolTipString );			
					pFlag:UpdateFlagOffset();
				end
			end
		end
	end
end
Events.GameplaySetActivePlayer.Add(OnActivePlayerChanged);
