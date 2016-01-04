print("This is the modded EconomicGeneralInfo from CBP")
-------------------------------------------------
-- Economic
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
local eFood = 5;
local eResearch = 6;
local eGold = 7;
local eCulture = 8;
local eFaith = 9;

local m_SortMode = ePopulation;
local m_bSortReverse = false;

local pediaSearchStrings = {};


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
	
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function UpdateDisplay()
    UpdateGPT();
    
    local pPlayer = Players[ Game.GetActivePlayer() ];
    if( pPlayer == nil ) then
        print( "Could not get player... huh?" );
        return;
    end
    
    m_SortTable = {};
	pediaSearchStrings = {};

    Controls.MainStack:DestroyAllChildren();
      
    for pCity in pPlayer:Cities() do
		local instance = {};
        ContextPtr:BuildInstanceForControl( "CityInstance", instance, Controls.MainStack );
        
        local sortEntry = {};
		m_SortTable[ tostring( instance.Root ) ] = sortEntry;

-- COMMUNITY PATCH
	
		local iStarvingUnhappiness = pCity:GetUnhappinessFromStarving();
		local iPillagedUnhappiness = pCity:GetUnhappinessFromPillaged();
		local iGoldUnhappiness = pCity:GetUnhappinessFromGold();
		local iDefenseUnhappiness = pCity:GetUnhappinessFromDefense();
		local iConnectionUnhappiness = pCity:GetUnhappinessFromConnection();
		local iMinorityUnhappiness = pCity:GetUnhappinessFromMinority();
		local iScienceUnhappiness = pCity:GetUnhappinessFromScience();
		local iCultureUnhappiness = pCity:GetUnhappinessFromCulture();
		
		local iResistanceUnhappiness = 0;
		if(pCity:IsRazing()) then
			iResistanceUnhappiness = (pCity:GetPopulation() / 2);
		elseif(pCity:IsResistance()) then
			iResistanceUnhappiness = (pCity:GetPopulation() / 2);
		end
		local iOccupationUnhappiness = 0;
		if(pCity:IsOccupied() and not pCity:IsNoOccupiedUnhappiness() and not pCity:IsResistance() and not pCity:IsRazing()) then
			iOccupationUnhappiness = (pCity:GetPopulation() * GameDefines.UNHAPPINESS_PER_OCCUPIED_POPULATION);
		end
		local iTotalUnhappiness = iScienceUnhappiness + iCultureUnhappiness + iDefenseUnhappiness	+ iGoldUnhappiness + iConnectionUnhappiness + iPillagedUnhappiness + iStarvingUnhappiness + iMinorityUnhappiness + iOccupationUnhappiness + iResistanceUnhappiness;
		sortEntry.Strength = math.floor(iTotalUnhappiness);
        instance.Defense:SetText( sortEntry.Strength );
-- END					
--		sortEntry.Strength = math.floor( pCity:GetStrengthValue() / 100 );
 --       instance.Defense:SetText( sortEntry.Strength );
        
        sortEntry.Production = pCity:GetProductionNameKey();
        ProductionDetails( pCity, instance );

		sortEntry.CityName = pCity:GetName();
        instance.CityName:SetText( sortEntry.CityName );

		local iPuppetMod = pPlayer:GetPuppetUnhappinessMod();
		local iCultureYield = pCity:GetUnhappinessFromCultureYield() / 100;
		local iDefenseYield = pCity:GetUnhappinessFromDefenseYield() / 100;
		local iGoldYield = pCity:GetUnhappinessFromGoldYield() / 100;
		local iCultureNeeded = pCity:GetUnhappinessFromCultureNeeded() / 100;
		local iDefenseNeeded = pCity:GetUnhappinessFromDefenseNeeded() / 100;
		local iGoldNeeded = pCity:GetUnhappinessFromGoldNeeded() / 100;
		local iScienceYield = pCity:GetUnhappinessFromScienceYield() / 100;
		local iScienceNeeded = pCity:GetUnhappinessFromScienceNeeded() / 100;

		local iCultureDeficit = pCity:GetUnhappinessFromCultureDeficit() / 100;
		local iDefenseDeficit = pCity:GetUnhappinessFromDefenseDeficit() / 100;
		local iGoldDeficit = pCity:GetUnhappinessFromGoldDeficit() / 100;
		local iScienceDeficit = pCity:GetUnhappinessFromScienceDeficit() / 100;


		strOccupationTT = Locale.ConvertTextKey("TXT_KEY_EO_CITY_LOCAL_UNHAPPINESS", iTotalUnhappiness);

		if(pCity:IsPuppet()) then
			if (iPuppetMod ~= 0) then
				strOccupationTT = strOccupationTT .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_PUPPET_UNHAPPINESS_MOD", iPuppetMod);
			end
		end
		
		-- Occupation tooltip
		if (iOccupationUnhappiness ~= 0) then
			strOccupationTT = strOccupationTT .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_OCCUPATION_UNHAPPINESS", iOccupationUnhappiness);
		end
		-- Resistance tooltip
		if (iResistanceUnhappiness ~= 0) then
			strOccupationTT = strOccupationTT .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_RESISTANCE_UNHAPPINESS", iResistanceUnhappiness);
		end
		-- Starving tooltip
		if (iStarvingUnhappiness ~= 0) then
			strOccupationTT = strOccupationTT .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_STARVING_UNHAPPINESS", iStarvingUnhappiness);
		end
		-- Pillaged tooltip
		if (iPillagedUnhappiness ~= 0) then
			strOccupationTT = strOccupationTT .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_PILLAGED_UNHAPPINESS", iPillagedUnhappiness);
		end
		-- Gold tooltip
		if (iGoldUnhappiness > 0) then
			strOccupationTT = strOccupationTT .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_GOLD_UNHAPPINESS", iGoldUnhappiness, iGoldYield, iGoldNeeded, iGoldDeficit);
		end
		if ((iGoldYield - iGoldNeeded) >= 0) then
			strOccupationTT = strOccupationTT .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_GOLD_UNHAPPINESS_SURPLUS", (iGoldYield - iGoldNeeded));
		end
		-- Defense tooltip
		if (iDefenseUnhappiness > 0) then
			strOccupationTT = strOccupationTT .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_DEFENSE_UNHAPPINESS", iDefenseUnhappiness, iDefenseYield, iDefenseNeeded, iDefenseDeficit);
		end
		if ((iDefenseYield - iDefenseNeeded) >= 0) then
			strOccupationTT = strOccupationTT .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_DEFENSE_UNHAPPINESS_SURPLUS", (iDefenseYield - iDefenseNeeded));
		end
		-- Connection tooltip
		if (iConnectionUnhappiness ~= 0) then
			strOccupationTT = strOccupationTT .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_CONNECTION_UNHAPPINESS", iConnectionUnhappiness);
		end
		-- Minority tooltip
		if (iMinorityUnhappiness ~= 0) then
			strOccupationTT = strOccupationTT .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_MINORITY_UNHAPPINESS", iMinorityUnhappiness);
		end
		-- Science tooltip
		if (iScienceUnhappiness > 0) then
			strOccupationTT = strOccupationTT .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_SCIENCE_UNHAPPINESS", iScienceUnhappiness, iScienceYield, iScienceNeeded, iScienceDeficit);
		end
		if ((iScienceYield - iScienceNeeded) >= 0) then
			strOccupationTT = strOccupationTT .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_SCIENCE_UNHAPPINESS_SURPLUS", (iScienceYield - iScienceNeeded));
		end
		-- Culture tooltip
		if (iCultureUnhappiness > 0) then
			strOccupationTT = strOccupationTT .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_UNHAPPINESS", iCultureUnhappiness, iCultureYield, iCultureNeeded, iCultureDeficit);
		end
		if ((iCultureYield - iCultureNeeded) >= 0) then
			strOccupationTT = strOccupationTT .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_CULTURE_UNHAPPINESS_SURPLUS", (iCultureYield - iCultureNeeded));
		end

		instance.CityName:SetToolTipString(strOccupationTT);
-- END  
        
        if(pCity:IsCapital())then
			instance.IconCapital:SetText("[ICON_CAPITAL]");
	        instance.IconCapital:SetHide( false );       
		elseif(pCity:IsPuppet()) then
			instance.IconCapital:SetText("[ICON_PUPPET]");
			instance.IconCapital:SetHide(false);
		elseif(pCity:IsOccupied() and not pCity:IsNoOccupiedUnhappiness()) then
			instance.IconCapital:SetText("[ICON_OCCUPIED]");
			instance.IconCapital:SetHide(false);
		else
			instance.IconCapital:SetHide(true);
		end
        
    	local pct = 1 - (pCity:GetDamage() / pCity:GetMaxHitPoints());
    	if( pct ~= 1 ) then
    	
            if pct > 0.66 then
                instance.HealthBar:SetFGColor( { x = 0, y = 1, z = 0, w = 1 } );
            elseif pct > 0.33 then
                instance.HealthBar:SetFGColor( { x = 1, y = 1, z = 0, w = 1 } );
            else
                instance.HealthBar:SetFGColor( { x = 1, y = 0, z = 0, w = 1 } );
            end
            
        	instance.HealthBar:SetPercent( pct );
        	instance.HealthBarBox:SetHide( false );
    	else
        	instance.HealthBarBox:SetHide( true );
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
		
		sortEntry.Food = pCity:FoodDifference();
        instance.Food:SetText( sortEntry.Food );
       
		local productionYield = pCity:GetYieldRate( YieldTypes.YIELD_PRODUCTION );
		local totalProductionPerTurn = math.floor(productionYield + (productionYield * (pCity:GetProductionModifier() / 100)));
       
		sortEntry.Production = totalProductionPerTurn;
        instance.Production:SetText( sortEntry.Production );
        
        
        if(Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE)) then
			instance.ScienceBox:SetHide(true);
        else
			sortEntry.Science = pCity:GetYieldRate( YieldTypes.YIELD_SCIENCE );
			instance.Science:SetText( sortEntry.Science );
			instance.ScienceBox:SetHide(false);
        end
        
        sortEntry.Gold = pCity:GetYieldRate( YieldTypes.YIELD_GOLD );
        instance.Gold:SetText( sortEntry.Gold );
        
        sortEntry.Culture = pCity:GetJONSCulturePerTurn();
        instance.Culture:SetText( sortEntry.Culture );
        
        if(Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
			instance.FaithBox:SetHide(true);
        else
			sortEntry.Faith = pCity:GetFaithPerTurn();
			instance.Faith:SetText(sortEntry.Faith);
			instance.FaithBox:SetHide(false);   
        end
    end
    
    Controls.MainStack:CalculateSize();
    Controls.MainStack:ReprocessAnchoring();
    Controls.MainScroll:CalculateInternalSize();
end


-------------------------------------------------
-------------------------------------------------
function UpdateGPT()

    local pPlayer = Players[ Game.GetActivePlayer() ];
    
    local iHandicap = Players[Game.GetActivePlayer()]:GetHandicapType();

    Controls.TotalGoldValue:SetText( Locale.ToNumber( pPlayer:GetGold(), "#.##" ) );
    
    local netGPT = pPlayer:CalculateGoldRateTimes100() / 100;
    Controls.NetGoldValue:SetText( Locale.ToNumber( netGPT, "#.##" ) );
    
    if( netGPT < 0 ) then
        Controls.ScienceLost:SetHide( false );
        Controls.ScienceLostValue:SetText( Locale.ToNumber( pPlayer:GetScienceFromBudgetDeficitTimes100() / 100, "#.##" ) );
    else
        Controls.ScienceLost:SetHide( true );
    end
    
    Controls.GrossGoldValue:SetText( "[COLOR_POSITIVE_TEXT]" .. Locale.ToNumber( pPlayer:CalculateGrossGoldTimes100() / 100, "#.##" ) .. "[ENDCOLOR]" );
    
    Controls.TotalExpenseValue:SetText( "[COLOR_NEGATIVE_TEXT]" .. Locale.ToNumber( pPlayer:CalculateInflatedCosts(), "#.##" ) .. "[ENDCOLOR]" );

	-- Cities
    Controls.CityIncomeValue:SetText( Locale.ToNumber( pPlayer:GetGoldFromCitiesTimes100() / 100, "#.##" ) );
    
    local bFoundCity = false;
    Controls.CityStack:DestroyAllChildren();
    for pCity in pPlayer:Cities() do
    
        local CityIncome = pCity:GetYieldRateTimes100(YieldTypes.YIELD_GOLD) / 100;
    
        if( CityIncome > 0 ) then
            bFoundCity = true;
    		local instance = {};
            ContextPtr:BuildInstanceForControl( "TradeEntry", instance, Controls.CityStack );
            
            instance.CityName:SetText( pCity:GetName() );
            instance.TradeIncomeValue:SetText( Locale.ToNumber( CityIncome, "#.##" ) );
        end
    end
    
    if( bFoundCity ) then
        Controls.CityToggle:SetDisabled( false );
        Controls.CityToggle:SetAlpha( 1.0 );
    else
        Controls.CityToggle:SetDisabled( true );
        Controls.CityToggle:SetAlpha( 0.5 );
    end
    Controls.CityStack:CalculateSize();
    Controls.CityStack:ReprocessAnchoring();
    
    -- Diplomacy
    local diploGPT = pPlayer:GetGoldPerTurnFromDiplomacy();
    if( diploGPT > 0 ) then
        Controls.DiploIncomeValue:SetText( Locale.ToNumber( diploGPT, "#.##" ) );
    else
        Controls.DiploIncomeValue:SetText( 0 );
    end
    
    -- Religion
    local religionGPT = pPlayer:GetGoldPerTurnFromReligion();
    if( religionGPT > 0 ) then
        Controls.ReligionIncomeValue:SetText( Locale.ToNumber( religionGPT, "#.##" ) );
    else
        Controls.ReligionIncomeValue:SetText( 0 );
    end

    Controls.TradeIncomeValue:SetText( Locale.ToNumber( pPlayer:GetCityConnectionGoldTimes100() / 100, "#.##" ) );
    
    -- Trade income breakdown tooltip
    local iBaseGold = GameDefines.TRADE_ROUTE_BASE_GOLD / 100;
    local iGoldPerPop = GameDefines.TRADE_ROUTE_CITY_POP_GOLD_MULTIPLIER / 100;
    local strTooltip = Locale.ConvertTextKey("TXT_KEY_EO_INCOME_TRADE");
    strTooltip = strTooltip .. "[NEWLINE][NEWLINE]";
	local iTradeRouteGoldModifier = pPlayer:GetCityConnectionTradeRouteGoldModifier();
	if (iTradeRouteGoldModifier ~= 0) then
		strTooltip = strTooltip .. Locale.ConvertTextKey("TXT_KEY_EGI_TRADE_ROUTE_MOD_INFO", iTradeRouteGoldModifier);
		strTooltip = strTooltip .. "[NEWLINE]";
	end
    strTooltip = strTooltip .. Locale.ConvertTextKey("TXT_KEY_TRADE_ROUTE_INCOME_INFO", iBaseGold, iGoldPerPop);
    Controls.TradeIncomeValue:SetToolTipString( strTooltip );
    
    local bFoundTrade = false;
    Controls.TradeStack:DestroyAllChildren();
    for pCity in pPlayer:Cities() do
    
        if( pPlayer:IsCapitalConnectedToCity( pCity ) ) then
            local tradeIncome = pPlayer:GetCityConnectionRouteGoldTimes100( pCity ) / 100;
        
            if( tradeIncome > 0 ) then
                bFoundTrade = true;
        		local instance = {};
                ContextPtr:BuildInstanceForControl( "TradeEntry", instance, Controls.TradeStack );
                
                instance.CityName:SetText( pCity:GetName() );
                instance.TradeIncomeValue:SetText( Locale.ToNumber( tradeIncome, "#.##" ) );
                
                local strPopInfo = " (" .. pCity:GetPopulation() .. ")"; 
                instance.CityName:SetToolTipString( strTooltip .. strPopInfo );
                instance.TradeIncomeValue:SetToolTipString( strTooltip .. strPopInfo );
                instance.TradeIncome:SetToolTipString( strTooltip .. strPopInfo );
            end 
        end 
    end
    
    if( bFoundTrade ) then
        Controls.TradeToggle:SetDisabled( false );
        Controls.TradeToggle:SetAlpha( 1.0 );
    else
        Controls.TradeToggle:SetDisabled( true );
        Controls.TradeToggle:SetAlpha( 0.5 );
    end
    Controls.TradeStack:CalculateSize();
    Controls.TradeStack:ReprocessAnchoring();

	-- Units
	
	local iTotalUnitMaintenance = pPlayer:CalculateUnitCost();
	
    Controls.UnitExpenseValue:SetText( Locale.ToNumber( iTotalUnitMaintenance , "#.##" ) );
    
    local iTotalUnits = pPlayer:GetNumUnits();
    
	print("Total Units - " .. iTotalUnits);
    local iMaintenanceFreeUnits = pPlayer:GetNumMaintenanceFreeUnits(DomainTypes.NO_DOMAIN, false);
    
    
	print("Maint Free Units - " .. iMaintenanceFreeUnits);
    
    local iPaidUnits = iTotalUnits - iMaintenanceFreeUnits;
    
    print("Paid Units - " .. iPaidUnits);
    
    local fCostPer = Locale.ToNumber( iTotalUnitMaintenance / iPaidUnits , "#.##" );
    
    local strUnitTT = Locale.ConvertTextKey("TXT_KEY_EO_EX_UNITS", fCostPer, iPaidUnits);
    
    -- Maintenance free units
    if (iMaintenanceFreeUnits ~= 0) then
		strUnitTT = strUnitTT .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_EO_EX_UNITS_NO_MAINT", iMaintenanceFreeUnits);
    end
    
    -- Maintenance mod (handicap)
    local iUnitMaintMod = GameInfo.HandicapInfos[iHandicap].UnitCostPercent;
    if (iUnitMaintMod ~= 100) then
		strUnitTT = strUnitTT .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_HANDICAP_MAINTENANCE_MOD", iUnitMaintMod);
	end
    
    Controls.UnitExpense:SetToolTipString( strUnitTT );
    
    -- Buildings
    local iBuildingMaintMod = GameInfo.HandicapInfos[iHandicap].BuildingCostPercent;
    
    local strBuildingsTT = Locale.ConvertTextKey("TXT_KEY_EO_EX_BUILDINGS");
    local strBuildingsModTT = "";
    
    if (iBuildingMaintMod ~= 100) then
		strBuildingsModTT = Locale.ConvertTextKey("TXT_KEY_HANDICAP_MAINTENANCE_MOD", iBuildingMaintMod);
		strBuildingsTT = strBuildingsTT .. "[NEWLINE][NEWLINE]" .. strBuildingsModTT;
		strBuildingsModTT = Locale.ConvertTextKey("TXT_KEY_EO_EX_BASE_BUILDINGS") .. "[NEWLINE][NEWLINE]" .. strBuildingsModTT;
	end
    
    Controls.BuildingsToggle:SetToolTipString( strBuildingsTT );
    
    Controls.BuildingExpenseValue:SetText( Locale.ToNumber( pPlayer:GetBuildingGoldMaintenance(), "#.##" ) );
   
    bFoundCity = false;
    Controls.BuildingsStack:DestroyAllChildren();
    for pCity in pPlayer:Cities() do
		
        local BuildingCost = pCity:GetTotalBaseBuildingMaintenance();
		
        if( BuildingCost > 0 ) then
            bFoundCity = true;
    		local instance = {};
            ContextPtr:BuildInstanceForControl( "TradeEntry", instance, Controls.BuildingsStack );
            
            instance.CityName:SetText( pCity:GetName() );
            instance.TradeIncomeValue:SetText( Locale.ToNumber( BuildingCost, "#.##" ) );
            instance.TradeIncome:SetToolTipString( strBuildingsModTT );
        end
    end
    
    if( bFoundCity ) then
        Controls.BuildingsToggle:SetDisabled( false );
        Controls.BuildingsToggle:SetAlpha( 1.0 );
    else
        Controls.BuildingsToggle:SetDisabled( true );
        Controls.BuildingsToggle:SetAlpha( 0.5 );
    end
    Controls.BuildingsStack:CalculateSize();
    Controls.BuildingsStack:ReprocessAnchoring();
    
    -- Routes
    local strRoutesTT = Locale.ConvertTextKey("TXT_KEY_EO_EX_IMPROVEMENTS");
    
    local iRouteMaintMod = GameInfo.HandicapInfos[iHandicap].RouteCostPercent;
    local strRoutesModTT = "";
    
    if (iRouteMaintMod ~= 100) then
		strRoutesModTT = Locale.ConvertTextKey("TXT_KEY_HANDICAP_MAINTENANCE_MOD", iRouteMaintMod);
		strRoutesTT = strRoutesTT .. "[NEWLINE][NEWLINE]" .. strRoutesModTT;
	end
    
    Controls.TileExpense:SetToolTipString(strRoutesTT);
    Controls.TileExpenseValue:SetText( Locale.ToNumber( pPlayer:GetImprovementGoldMaintenance(), "#.##" ) );
    
    -- Diplo
    local diploGPT = pPlayer:GetGoldPerTurnFromDiplomacy();
    if( diploGPT < 0 ) then
        Controls.DiploExpenseValue:SetText( Locale.ToNumber( -diploGPT, "#.##" ) );
    else
        Controls.DiploExpenseValue:SetText( 0 );
    end
    
    Controls.GoldScroll:CalculateInternalSize();
end

-- Start hidden
Controls.CityStack:SetHide( true );
Controls.TradeStack:SetHide( true );
Controls.BuildingsStack:SetHide( true );

-------------------------------------------------
-------------------------------------------------
function OnCityToggle()
    local bWasHidden = Controls.CityStack:IsHidden();
    Controls.CityStack:SetHide( not bWasHidden );
    if( bWasHidden ) then
        Controls.CityToggle:LocalizeAndSetText("TXT_KEY_EO_INCOME_CITIES_COLLAPSE");
    else
        Controls.CityToggle:LocalizeAndSetText("TXT_KEY_EO_INCOME_CITIES");
    end
    Controls.GoldStack:CalculateSize();
    Controls.GoldStack:ReprocessAnchoring();
    Controls.GoldScroll:CalculateInternalSize();
end
Controls.CityToggle:RegisterCallback( Mouse.eLClick, OnCityToggle );

-------------------------------------------------
-------------------------------------------------
function OnTradeToggle()
    local bWasHidden = Controls.TradeStack:IsHidden();
    Controls.TradeStack:SetHide( not bWasHidden );
    if( bWasHidden ) then
        Controls.TradeToggle:LocalizeAndSetText("TXT_KEY_EO_INCOME_TRADE_DETAILS_COLLAPSE");
    else
        Controls.TradeToggle:LocalizeAndSetText("TXT_KEY_EO_INCOME_TRADE_DETAILS");
    end
    Controls.GoldStack:CalculateSize();
    Controls.GoldStack:ReprocessAnchoring();
    Controls.GoldScroll:CalculateInternalSize();
end
Controls.TradeToggle:RegisterCallback( Mouse.eLClick, OnTradeToggle );

-------------------------------------------------
-------------------------------------------------
function OnBuildingsToggle()
    local bWasHidden = Controls.BuildingsStack:IsHidden();
    Controls.BuildingsStack:SetHide( not bWasHidden );
    if( bWasHidden ) then
        Controls.BuildingsToggle:LocalizeAndSetText("TXT_KEY_EO_BUILDINGS_COLLAPSE");
    else
        Controls.BuildingsToggle:LocalizeAndSetText("TXT_KEY_EO_BUILDINGS");
    end
    Controls.GoldStack:CalculateSize();
    Controls.GoldStack:ReprocessAnchoring();
    Controls.GoldScroll:CalculateInternalSize();
end
Controls.BuildingsToggle:RegisterCallback( Mouse.eLClick, OnBuildingsToggle );


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
		elseif( m_SortMode == eFood ) then
			valueA = entryA.Food;
			valueB = entryB.Food;
		elseif( m_SortMode == eResearch ) then
			valueA = entryA.Science;
			valueB = entryB.Science;
		elseif( m_SortMode == eGold ) then
			valueA = entryA.Gold;
			valueB = entryB.Gold;
		elseif( m_SortMode == eCulture ) then
			valueA = entryA.Culture;
			valueB = entryB.Culture;
		elseif( m_SortMode == eFaith ) then
			valueA = entryA.Faith;
			valueB = entryB.Faith;
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
Controls.SortFood:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortGold:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortCulture:RegisterCallback( Mouse.eLClick, OnSort );

Controls.SortPopulation:SetVoid1( ePopulation );
Controls.SortCityName:SetVoid1( eName );
Controls.SortStrength:SetVoid1( eStrength );
Controls.SortProduction:SetVoid1( eProduction );
Controls.SortFood:SetVoid1( eFood );
Controls.SortGold:SetVoid1( eGold );
Controls.SortCulture:SetVoid1( eCulture );

if(Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE)) then
	Controls.SortResearch:SetHide(true);
	Controls.ScienceLost:SetHide(true);
else
	Controls.SortResearch:RegisterCallback(Mouse.eLClick, OnSort);
	Controls.SortResearch:SetVoid1(eResearch);
	Controls.SortResearch:SetHide(false);
	Controls.ScienceLost:SetHide(false);
end

if(Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
	Controls.SortFaith:SetHide(true);
	Controls.ReligionIncome:SetHide(true);
else
	Controls.SortFaith:RegisterCallback(Mouse.eLClick, OnSort);
	Controls.SortFaith:SetVoid1(eFaith);
	Controls.SortFaith:SetHide(false);
	Controls.ReligionIncome:SetHide(false);
end

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
			local thisUnitInfo = GameInfo.Units[unitProduction];
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
	instance.ProdButton:RegisterCallback( Mouse.eLClick, OnProdClick );
	pediaSearchStrings[tostring(instance.ProdButton)] = Locale.ConvertTextKey(cityProductionName);
	instance.ProdButton:RegisterCallback( Mouse.eRClick, OnProdRClick );
	instance.ProdButton:SetVoids( city:GetID(), nil );

end


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )
    if( not bIsHide ) then
    	UpdateDisplay();
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );