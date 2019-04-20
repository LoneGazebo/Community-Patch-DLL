print("This is the modded CityList from CBP")

-------------------------------------------------
-- City List
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );

local defaultErrorTextureSheet = "CityBannerProductionImage.dds";
local nullOffset = Vector2( 0, 0 );
local g_iPortraitSize = 45;

local m_SortTable;
local ePopulation     = 0;
local eName   = 1;
local eStrength = 2;
local eProduction = 3;

local m_SortMode = ePopulation;
local m_bSortReverse = false;

local pediaSearchStrings = {};

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
function OpenEcon()
	Events.SerialEventGameMessagePopup( { Type = ButtonPopupTypes.BUTTONPOPUP_ECONOMIC_OVERVIEW } );
end
Controls.OpenEconButton:RegisterCallback( Mouse.eLClick, OpenEcon );


-------------------------------------------------
-------------------------------------------------
function OnClose( )
    Events.OpenInfoCorner( InfoCornerID.None );
    ContextPtr:SetHide( true );
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
function OnChangeEvent()
    if( ContextPtr:IsHidden() == false ) then
        UpdateDisplay();
    end
end
Events.SerialEventCityInfoDirty.Add( OnChangeEvent );
Events.SerialEventCitySetDamage.Add( OnChangeEvent );
Events.SerialEventCityDestroyed.Add( OnChangeEvent );
Events.SerialEventCityCaptured.Add( OnChangeEvent );
Events.SerialEventCityCreated.Add( OnChangeEvent );
Events.SpecificCityInfoDirty.Add( OnChangeEvent );
Events.GameplaySetActivePlayer.Add( OnChangeEvent );

-------------------------------------------------
-- Determines whether or not to show the Range Strike Button
-------------------------------------------------
function ShouldShowRangeStrikeButton(city) 
	if city == nil then
		return false;
	end
		
	return city:CanRangeStrikeNow();
end

-------------------------------------------------
-------------------------------------------------
function UpdateDisplay()

	m_SortTable = {};
	
	pediaSearchStrings = {};

    local pPlayer = Players[ Game.GetActivePlayer() ];
    
    Controls.MainStack:DestroyAllChildren();
    
    for pCity in pPlayer:Cities() do
    
        local instance = {};
        ContextPtr:BuildInstanceForControl( "CityInstance", instance, Controls.MainStack );
        
        instance.Button:RegisterCallback( Mouse.eLClick, OnCityClick );
        instance.Button:RegisterCallback( Mouse.eRClick, OnCityRClick );
        instance.Button:SetVoids( pCity:GetX(), pCity:GetY() );
        
        local sortEntry = {};
		m_SortTable[ tostring( instance.Root ) ] = sortEntry;
		
		-- update range strike button (if it is the active player's city)
		sortEntry.RangeAttack = ShouldShowRangeStrikeButton(pCity);
		if  sortEntry.RangeAttack then
			instance.CityRangeStrikeButton:SetHide(false);
			instance.CityRangeStrikeAnim:SetHide( false );
		else
			instance.CityRangeStrikeButton:SetHide(true);
			instance.CityRangeStrikeAnim:SetHide( true );
		end

	
		--sortEntry.Strength = math.floor( pCity:GetStrengthValue() / 100 );
        --instance.Defense:SetText( sortEntry.Strength );
        
        sortEntry.Production = pCity:GetProductionNameKey();
        ProductionDetails( pCity, instance );

		sortEntry.CityName = pCity:GetName();
        instance.CityName:SetText( sortEntry.CityName );
   
-- COMMUNITY PATCH
		local iTotalUnhappiness = pCity:getHappinessDelta();

		sortEntry.Strength = math.floor(iTotalUnhappiness);
        instance.Defense:SetText( sortEntry.Strength );

		local TT = pCity:GetCityHappinessBreakdown();
		TT = TT .. pCity:GetCityUnhappinessBreakdown(false);
		instance.CityName:SetToolTipString(TT);
-- END   
        
        -- Support for Expansions.
        local MaxCityHitPoints;
        if(pCity.GetMaxHitPoints ~= nil) then
			MaxCityHitPoints = pCity:GetMaxHitPoints();
        else
			MaxCityHitPoints = GameDefines.MAX_CITY_HIT_POINTS;
        end
        
        local iHealthPercent = 1 - (pCity:GetDamage() / MaxCityHitPoints);
        if( iHealthPercent < 1 ) then
        	
            if iHealthPercent > 0.66 then
                instance.HealthBar:SetFGColor( { x = 0, y = 1, z = 0, w = 1 } );
            elseif iHealthPercent > 0.33 then
                instance.HealthBar:SetFGColor( { x = 1, y = 1, z = 0, w = 1 } );
            else
                instance.HealthBar:SetFGColor( { x = 1, y = 0, z = 0, w = 1 } );
            end
           
        	instance.HealthBarAnchor:SetHide( false );
        	instance.HealthBar:SetPercent( iHealthPercent );
    	else
        	instance.HealthBarAnchor:SetHide( true );
    	end
    
		if(pCity:IsCapital())then
			instance.IconCapital:SetText("[ICON_CAPITAL]");
			instance.IconCapital:SetHide( false );
		elseif(pCity:IsPuppet()) then
			instance.IconCapital:SetText("[ICON_PUPPET]");
			instance.IconCapital:SetHide(false);
		elseif(pCity:IsOccupied() and not pCity:IsNoOccupiedUnhappiness()) then
			print("city is occupied");
			instance.IconCapital:SetText("[ICON_OCCUPIED]");
			instance.IconCapital:SetHide(false);
		else
			instance.IconCapital:SetHide(true);
		end
		
		sortEntry.Population = pCity:GetPopulation();
        instance.Population:SetText( sortEntry.Population );

		-- Update Growth Meter
		if (instance.GrowthBar) then
			
			local iCurrentFood = pCity:GetFood();
			local iFoodNeeded = pCity:GrowthThreshold();
			local iFoodPerTurn = pCity:FoodDifference();
			local iCurrentFoodPlusThisTurn = iCurrentFood + iFoodPerTurn;
			
			local fGrowthProgressPercent = iCurrentFood / iFoodNeeded;
			local fGrowthProgressPlusThisTurnPercent = iCurrentFoodPlusThisTurn / iFoodNeeded;
			if (fGrowthProgressPlusThisTurnPercent > 1) then
				fGrowthProgressPlusThisTurnPercent = 1
			end
			
			instance.GrowthBar:SetPercent( fGrowthProgressPercent );
			instance.GrowthBarShadow:SetPercent( fGrowthProgressPlusThisTurnPercent );
		end
		
		-- Update Growth Time
		if(instance.CityGrowth) then
			local cityGrowth = pCity:GetFoodTurnsLeft();
			
			if (pCity:IsFoodProduction() or pCity:FoodDifferenceTimes100() == 0) then
				cityGrowth = "-";
				--instance.CityBannerRightBackground:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CITY_STOPPED_GROWING_TT", localizedCityName, cityPopulation));
			elseif pCity:FoodDifferenceTimes100() < 0 then
				cityGrowth = "[COLOR_WARNING_TEXT]-[ENDCOLOR]";
				--instance.CityBannerRightBackground:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CITY_STARVING_TT",localizedCityName ));
			else
				--instance.CityBannerRightBackground:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CITY_WILL_GROW_TT", localizedCityName, cityPopulation, cityPopulation+1, cityGrowth));
			end
			
			instance.CityGrowth:SetText(cityGrowth);
		end
    end

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
		if( m_SortMode == ePopulation ) then
			valueA = entryA.Population;
			valueB = entryB.Population;
		elseif( m_SortMode == eName ) then
			valueA = entryA.CityName;
			valueB = entryB.CityName;
		elseif( m_SortMode == eStrength ) then
			valueA = entryA.Strength;
			valueB = entryB.Strength;
		else -- SortProduction
			valueA = entryA.Production;
			valueB = entryB.Production;
		end
	    
		if( valueA == valueB ) then
			valueA = entryA.CityName;
			valueB = entryB.CityName;
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
    Controls.MainStack:SortChildren( SortFunction );
end
Controls.SortPopulation:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortCityName:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortStrength:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortProduction:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortPopulation:SetVoid1( ePopulation );
Controls.SortCityName:SetVoid1( eName );
Controls.SortStrength:SetVoid1( eStrength );
Controls.SortProduction:SetVoid1( eProduction );

-------------------------------------------------
-------------------------------------------------
function OnCityClick( x, y )
    local plot = Map.GetPlot( x, y );
    if( plot ~= nil ) then
    	UI.DoSelectCityAtPlot( plot );
	end
end

	
-------------------------------------------------
-------------------------------------------------
function OnCityRClick( x, y )
    local plot = Map.GetPlot( x, y );
    if( plot ~= nil ) then
    	UI.LookAt( plot );
	end
end

-------------------------------------------------
-------------------------------------------------
function OnProdClick( cityID, prodName )
	local popupInfo = {
			Type = ButtonPopupTypes.BUTTONPOPUP_CHOOSEPRODUCTION,
			Data1 = cityID,
			Data2 = -1,
			Data3 = -1,
			Option1 = false,
			Option2 = false;
		}
	Events.SerialEventGameMessagePopup(popupInfo);
    -- send production popup message
end

-------------------------------------------------
-------------------------------------------------
function OnProdRClick( cityID, void2, button )
	local searchString = pediaSearchStrings[tostring(button)];
	Events.SearchForPediaEntry( searchString );		
end
	
-------------------------------------------------
-------------------------------------------------
function OnOpenInfoCorner( iInfoType )
    if( iInfoType == InfoCornerID.Cities ) then
        ContextPtr:SetHide( false );
    else
        ContextPtr:SetHide( true );
    end
end
Events.OpenInfoCorner.Add( OnOpenInfoCorner );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ProductionDetails( city, instance )
	
	-- Update Production Meter
	if (instance.ProductionBar) then
		
		local iCurrentProduction = city:GetProduction();
		local iProductionNeeded = city:GetProductionNeeded();
		local iProductionPerTurn = city:GetYieldRate(YieldTypes.YIELD_PRODUCTION);
		if (city:IsFoodProduction()) then
			iProductionPerTurn = iProductionPerTurn + city:GetYieldRate(YieldTypes.YIELD_FOOD) - city:FoodConsumption(true);
		end
		local iCurrentProductionPlusThisTurn = iCurrentProduction + iProductionPerTurn;
		
		local fProductionProgressPercent = iCurrentProduction / iProductionNeeded;
		local fProductionProgressPlusThisTurnPercent = iCurrentProductionPlusThisTurn / iProductionNeeded;
		if (fProductionProgressPlusThisTurnPercent > 1) then
			fProductionProgressPlusThisTurnPercent = 1
		end
		
		instance.ProductionBar:SetPercent( fProductionProgressPercent );
		instance.ProductionBarShadow:SetPercent( fProductionProgressPlusThisTurnPercent );	
	end	
	
	-- Update Production Time
	if(instance.BuildGrowth) then
		local buildGrowth = "-";
		
		if (city:IsProduction() and not city:IsProductionProcess()) then
			if (city:GetCurrentProductionDifferenceTimes100(false, false) > 0) then
				buildGrowth = city:GetProductionTurnsLeft();
			end
		end
		
		instance.BuildGrowth:SetText(buildGrowth);

	end

	-- Update Production Name
	local cityProductionName = city:GetProductionNameKey();
	if cityProductionName == nil or string.len(cityProductionName) == 0 then
		cityProductionName = "TXT_KEY_PRODUCTION_NO_PRODUCTION";
	end
	instance.ProdImage:SetToolTipString( Locale.ConvertTextKey( cityProductionName ) );


	-- Update Production icon
	if instance.ProdImage then
		local unitProduction = city:GetProductionUnit();
		local buildingProduction = city:GetProductionBuilding();
		local projectProduction = city:GetProductionProject();
		local processProduction = city:GetProductionProcess();
		local noProduction = false;

		if unitProduction ~= -1 then
			local portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon(unitProduction, city:GetOwner());
			if IconHookup( portraitOffset, g_iPortraitSize, portraitAtlas, instance.ProdImage ) then
				instance.ProdImage:SetHide( false );
			else
				instance.ProdImage:SetHide( true );
			end
		elseif buildingProduction ~= -1 then
			local thisBuildingInfo = GameInfo.Buildings[buildingProduction];
			if IconHookup( thisBuildingInfo.PortraitIndex, g_iPortraitSize, thisBuildingInfo.IconAtlas, instance.ProdImage ) then
				instance.ProdImage:SetHide( false );
			else
				instance.ProdImage:SetHide( true );
			end
		elseif projectProduction ~= -1 then
			local thisProjectInfo = GameInfo.Projects[projectProduction];
			if IconHookup( thisProjectInfo.PortraitIndex, g_iPortraitSize, thisProjectInfo.IconAtlas, instance.ProdImage ) then
				instance.ProdImage:SetHide( false );
			else
				instance.ProdImage:SetHide( true );
			end
		elseif processProduction ~= -1 then
			local thisProcessInfo = GameInfo.Processes[processProduction];
			if IconHookup( thisProcessInfo.PortraitIndex, g_iPortraitSize, thisProcessInfo.IconAtlas, instance.ProdImage ) then
				instance.ProdImage:SetHide( false );
			else
				instance.ProdImage:SetHide( true );
			end
		else -- really should have an error texture
			instance.ProdImage:SetHide(true);
		end
	end
	
	-- hookup pedia and production popup to production button
	if (not city:IsPuppet()) then
		instance.ProdButton:RegisterCallback( Mouse.eLClick, OnProdClick );
	end
	pediaSearchStrings[tostring(instance.ProdButton)] = Locale.ConvertTextKey(cityProductionName);
	instance.ProdButton:RegisterCallback( Mouse.eRClick, OnProdRClick );
	instance.ProdButton:SetVoids( city:GetID(), nil );
	
end


