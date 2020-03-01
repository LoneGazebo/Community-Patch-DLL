-------------------------------------------------
-- Happiness Info
-------------------------------------------------
include( "InstanceManager" );

Controls.TradeRouteStack:SetHide( true );
Controls.LocalCityStack:SetHide( true );
Controls.CityUnhappinessStack:SetHide( true );
Controls.ResourcesAvailableStack:SetHide( true );
Controls.ResourcesImportedStack:SetHide( true );
Controls.ResourcesExportedStack:SetHide( true );
Controls.ResourcesLocalStack:SetHide( true );

if(Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
	Controls.Religion:SetHide(true);
else
	Controls.Religion:SetHide(false);
end

-------------------------------------------------
-------------------------------------------------
function OnTradeRouteToggle()
    local bWasHidden = Controls.TradeRouteStack:IsHidden();
    Controls.TradeRouteStack:SetHide( not bWasHidden );
    if( bWasHidden ) then
        Controls.TradeRouteToggle:LocalizeAndSetText("TXT_KEY_EO_TRADE_ROUTES_COLLAPSE");
    else
        Controls.TradeRouteToggle:LocalizeAndSetText("TXT_KEY_EO_TRADE_ROUTES");
    end
    Controls.HappinessStack:CalculateSize();
    Controls.HappinessStack:ReprocessAnchoring();
    Controls.HappinessScroll:CalculateInternalSize();
end
Controls.TradeRouteToggle:RegisterCallback( Mouse.eLClick, OnTradeRouteToggle );

-------------------------------------------------
-------------------------------------------------
function OnLocalCityToggle()
    local bWasHidden = Controls.LocalCityStack:IsHidden();
    Controls.LocalCityStack:SetHide( not bWasHidden );
    if( bWasHidden ) then
        Controls.LocalCityToggle:LocalizeAndSetText("TXT_KEY_EO_LOCAL_CITY_COLLAPSE");
    else
        Controls.LocalCityToggle:LocalizeAndSetText("TXT_KEY_EO_LOCAL_CITY");
    end
    Controls.HappinessStack:CalculateSize();
    Controls.HappinessStack:ReprocessAnchoring();
    Controls.HappinessScroll:CalculateInternalSize();
end
Controls.LocalCityToggle:RegisterCallback( Mouse.eLClick, OnLocalCityToggle );

-------------------------------------------------
-------------------------------------------------
function OnCityUnhappinessToggle()
    local bWasHidden = Controls.CityUnhappinessStack:IsHidden();
    Controls.CityUnhappinessStack:SetHide( not bWasHidden );
    if( bWasHidden ) then
        Controls.CityUnhappinessToggle:LocalizeAndSetText("TXT_KEY_EO_CITY_UNHAPPINESS_COLLAPSE");
    else
        Controls.CityUnhappinessToggle:LocalizeAndSetText("TXT_KEY_EO_CITY_UNHAPPINESS");
    end
    Controls.UnhappinessStack:CalculateSize();
    Controls.UnhappinessStack:ReprocessAnchoring();
    Controls.UnhappinessScroll:CalculateInternalSize();
end
Controls.CityUnhappinessToggle:RegisterCallback( Mouse.eLClick, OnCityUnhappinessToggle );

-------------------------------------------------
-------------------------------------------------
function OnResourcesAvailableToggle()
    local bWasHidden = Controls.ResourcesAvailableStack:IsHidden();
    Controls.ResourcesAvailableStack:SetHide( not bWasHidden );
    local strString = Locale.ConvertTextKey("TXT_KEY_EO_RESOURCES_AVAILBLE");
    if( bWasHidden ) then
        Controls.ResourcesAvailableToggle:SetText("[ICON_MINUS]" .. strString);
    else
        Controls.ResourcesAvailableToggle:SetText("[ICON_PLUS]" .. strString);
    end
    Controls.ResourcesStack:CalculateSize();
    Controls.ResourcesStack:ReprocessAnchoring();
    Controls.ResourcesScroll:CalculateInternalSize();
end
Controls.ResourcesAvailableToggle:RegisterCallback( Mouse.eLClick, OnResourcesAvailableToggle );

-------------------------------------------------
-------------------------------------------------
function OnResourcesImportedToggle()
    local bWasHidden = Controls.ResourcesImportedStack:IsHidden();
    Controls.ResourcesImportedStack:SetHide( not bWasHidden );
    local strString = Locale.ConvertTextKey("TXT_KEY_RESOURCES_IMPORTED");
    if( bWasHidden ) then
        Controls.ResourcesImportedToggle:SetText("[ICON_MINUS]" .. strString);
    else
        Controls.ResourcesImportedToggle:SetText("[ICON_PLUS]" .. strString);
    end
    Controls.ResourcesStack:CalculateSize();
    Controls.ResourcesStack:ReprocessAnchoring();
    Controls.ResourcesScroll:CalculateInternalSize();
end
Controls.ResourcesImportedToggle:RegisterCallback( Mouse.eLClick, OnResourcesImportedToggle );

-------------------------------------------------
-------------------------------------------------
function OnResourcesExportedToggle()
    local bWasHidden = Controls.ResourcesExportedStack:IsHidden();
    Controls.ResourcesExportedStack:SetHide( not bWasHidden );
    local strString = Locale.ConvertTextKey("TXT_KEY_RESOURCES_EXPORTED");
    if( bWasHidden ) then
        Controls.ResourcesExportedToggle:SetText("[ICON_MINUS]" .. strString);
    else
        Controls.ResourcesExportedToggle:SetText("[ICON_PLUS]" .. strString);
    end
    Controls.ResourcesStack:CalculateSize();
    Controls.ResourcesStack:ReprocessAnchoring();
    Controls.ResourcesScroll:CalculateInternalSize();
end
Controls.ResourcesExportedToggle:RegisterCallback( Mouse.eLClick, OnResourcesExportedToggle );

-------------------------------------------------
-------------------------------------------------
function OnResourcesLocalToggle()
    local bWasHidden = Controls.ResourcesLocalStack:IsHidden();
    Controls.ResourcesLocalStack:SetHide( not bWasHidden );
    local strString = Locale.ConvertTextKey("TXT_KEY_EO_LOCAL_RESOURCES");
    if( bWasHidden ) then
        Controls.ResourcesLocalToggle:SetText("[ICON_MINUS]" .. strString);
    else
        Controls.ResourcesLocalToggle:SetText("[ICON_PLUS]" .. strString);
    end
    Controls.ResourcesStack:CalculateSize();
    Controls.ResourcesStack:ReprocessAnchoring();
    Controls.ResourcesScroll:CalculateInternalSize();
end
Controls.ResourcesLocalToggle:RegisterCallback( Mouse.eLClick, OnResourcesLocalToggle );

-------------------------------------------------
-------------------------------------------------
function UpdateScreen()

	local iPlayerID = Game.GetActivePlayer();
    local pPlayer = Players[iPlayerID];
	local pTeam = Teams[pPlayer:GetTeam()];
	local pCity = UI.GetHeadSelectedCity();
    
    local iHandicap = Players[Game.GetActivePlayer()]:GetHandicapType();
	
	-----------------------------------------------
	-- Happiness
	-----------------------------------------------
	
	local iEmpireHappiness = pPlayer:GetHappiness();

	Controls.TotalHappinessValue:SetText("[COLOR_POSITIVE_TEXT]" .. iEmpireHappiness .. "[ENDCOLOR]");

	local iCityHappiness = pPlayer:GetHappinessFromCities();
	local iTradeRouteHappiness = pPlayer:GetHappinessFromTradeRoutes();
	local iReligionHappiness = 0;
	iReligionHappiness = pPlayer:GetHappinessFromReligion();
	local iNaturalWonderHappiness = pPlayer:GetHappinessFromNaturalWonders();
	local iMinorCivHappiness = pPlayer:GetHappinessFromMinorCivs();
	local iEvent = 0;
	iEvent = pPlayer:GetEventHappiness();
	local iLeagueHappiness = pPlayer:GetHappinessFromLeagues();
	local handicaphappiness = pPlayer:GetHandicapHappiness();
	local luxuryHappiness = pPlayer:GetBonusHappinessFromLuxuriesFlat();
	-- C4DF
	local iHappinessFromVassal = pPlayer:GetHappinessFromVassals();
	-- END
	
	local iTotalHappiness = iHappiness;
	
	-- Trade Routes
	
	if (iTradeRouteHappiness ~= 0) then
		Controls.TradeRouteStack:DestroyAllChildren();
		for pCity in pPlayer:Cities() do
		    
		    -- Capitals cannot be connected to themselves
		    if (not pCity:IsCapital()) then
			    
			    local strTradeConnection = "-";
			    
				if (pPlayer:IsCapitalConnectedToCity(pCity)) then
					strTradeConnection = pPlayer:GetHappinessPerTradeRoute() / 100;
				end
				
				local instance = {};
				ContextPtr:BuildInstanceForControl( "TradeEntry", instance, Controls.TradeRouteStack );
		        
				instance.CityName:SetText( pCity:GetName() );
				instance.TradeIncomeValue:SetText( strTradeConnection );
			end
		end
		
		Controls.TradeRouteValue:SetText(iTradeRouteHappiness);
	end
    
    if( iTradeRouteHappiness > 0 ) then
        Controls.TradeRouteToggle:SetHide( false );
    else
        Controls.TradeRouteToggle:SetHide( true );
    end
    Controls.TradeRouteStack:CalculateSize();
    Controls.TradeRouteStack:ReprocessAnchoring();
	
	-- LocalCityHappiness
	
	local iCityHappiness = 0;

	Controls.LocalCityStack:DestroyAllChildren();
	for pCity in pPlayer:Cities() do
		    
		local strLocalCity = "-";
		    
		strLocalCity = pCity:GetLocalHappiness();
		iCityHappiness = iCityHappiness + pCity:GetLocalHappiness()
			
		local instance = {};
		ContextPtr:BuildInstanceForControl( "TradeEntry", instance, Controls.LocalCityStack );
	        
		instance.CityName:SetText( pCity:GetName() );
		instance.TradeIncomeValue:SetText( strLocalCity );
		instance.TradeIncome:SetToolTipString(pCity:GetCityHappinessBreakdown());
	end
		
	Controls.LocalCityValue:SetText(iCityHappiness);
    
    Controls.LocalCityToggle:SetHide( false );
    Controls.LocalCityStack:CalculateSize();
    Controls.LocalCityStack:ReprocessAnchoring();
	
	
	-- Handicap
	Controls.HandicapHappinessValue:SetText(handicaphappiness);

	-- Luxuries
	Controls.LuxuryHappinessValue:SetText(luxuryHappiness);

	-- City-States
	Controls.MinorCivHappinessValue:SetText(iMinorCivHappiness);

	-- Events
	Controls.EventHappinessValue:SetText(iEvent);


	-- Religion
	Controls.ReligionHappinessValue:SetText(iReligionHappiness);
	
	-- Natural Wonders
	if (iNaturalWonderHappiness > 0) then
		Controls.NaturalWondersValue:SetText(iNaturalWonderHappiness);
		Controls.NaturalWonders:SetHide(false);
	else
		Controls.NaturalWonders:SetHide(true);
	end
	
	
	-- Leagues
	if (iLeagueHappiness ~= 0) then
		Controls.LeagueHappinessValue:SetText(iLeagueHappiness);
		Controls.LeagueHappiness:SetHide(false);
	else
		Controls.LeagueHappiness:SetHide(true);
	end

	-- END
	-- C4DF
	if (iHappinessFromVassal ~= 0) then
		Controls.VassalHappinessValue:SetText(iHappinessFromVassal);
		Controls.VassalHappiness:SetHide(false);
	else
		Controls.VassalHappiness:SetHide(true);
	end

	-----------------------------------------------
	-- Unhappiness
	-----------------------------------------------

	local iEmpireUnhappiness = pPlayer:GetUnhappiness();
	
	-- Total Unhappiness
	Controls.TotalUnhappinessValue:SetText("[COLOR_NEGATIVE_TEXT]" .. iEmpireUnhappiness .. "[ENDCOLOR]");
	
	-- Public Opinion
	local iUnhappinessPublicOpinion =  pPlayer:GetUnhappinessFromPublicOpinion();
	if (iUnhappinessPublicOpinion ~= 0) then
		Controls.PoblicOpinionUnhappinessValue:SetText(iUnhappinessPublicOpinion);
		Controls.PoblicOpinionUnhappinessValue:LocalizeAndSetToolTip("TXT_KEY_TP_UNHAPPINESS_PUBLIC_OPINION", iUnhappinessPublicOpinion);
		Controls.PublicOpinionUnhappiness:SetHide(false);
	else
		Controls.PublicOpinionUnhappiness:SetHide(true);
	end	

	local iUnhappinessWar =  pPlayer:GetUnhappinessFromWarWeariness();
	if (iUnhappinessWar ~= 0) then
		Controls.PublicOpinionWarValue:SetText(iUnhappinessWar);
		Controls.PublicOpinionWarValue:LocalizeAndSetToolTip("TXT_KEY_TP_UNHAPPINESS_WAR_WEARINESS", iUnhappinessWar);
		Controls.PublicOpinionWar:SetHide(false);
	else
		Controls.PublicOpinionWar:SetHide(true);
	end	

	-- City Breakdown

	
	
	local iTotalUnhappiness = 0;
    Controls.CityUnhappinessStack:DestroyAllChildren();
    for pCity in pPlayer:Cities() do
	    
	    local iUnhappiness = pCity:getUnhappinessAggregated();
		local instance = {};
		ContextPtr:BuildInstanceForControl( "TradeEntry", instance, Controls.CityUnhappinessStack );
        
        instance.CityName:SetText( pCity:GetName() );
		instance.TradeIncomeValue:SetText( iUnhappiness );

		iTotalUnhappiness = iTotalUnhappiness + iUnhappiness;
		
		-- Occupation tooltip
		local strOccupationTT = Locale.ConvertTextKey("TXT_KEY_EO_CITY_IS_NOT_OCCUPIED");
		if (pCity:IsOccupied() and not pCity:IsNoOccupiedUnhappiness()) then
			strOccupationTT = Locale.ConvertTextKey("TXT_KEY_EO_CITY_IS_OCCUPIED");
		end

-- COMMUNITY CHANGE
		strOccupationTT = strOccupationTT .. pCity:GetCityUnhappinessBreakdown(false);
-- END CHANGE
		
		instance.TradeIncome:SetToolTipString(strOccupationTT);
	end

	Controls.CityUnhappinessValue:SetText(iTotalUnhappiness);
    
    if( pPlayer:GetNumCities() > 0 ) then
        Controls.CityUnhappinessToggle:SetDisabled( false );
        Controls.CityUnhappinessToggle:SetAlpha( 1.0 );
    else
        Controls.CityUnhappinessToggle:SetDisabled( true );
        Controls.CityUnhappinessToggle:SetAlpha( 0.5 );
    end
    Controls.CityUnhappinessStack:CalculateSize();
    Controls.CityUnhappinessStack:ReprocessAnchoring();




	-----------------------------------------------
	-- Resources
	-----------------------------------------------
	
	-- Resources Available
	
    Controls.ResourcesAvailableToggle:SetText("[ICON_PLUS]" .. Locale.ConvertTextKey("TXT_KEY_EO_RESOURCES_AVAILBLE"));
	
	local iTotalNumResources = 0;

	print("iTotalNumResources: " .. iTotalNumResources);
	
    Controls.ResourcesAvailableStack:DestroyAllChildren();
	for pResource in GameInfo.Resources() do
		local iResourceID = pResource.ID;
		local iNum = pPlayer:GetNumResourceTotal(iResourceID, true);
		
		if (iNum > 0) then
			-- Only Luxuries or Strategic Resources
			if (Game.GetResourceUsageType(iResourceID) ~= ResourceUsageTypes.RESOURCEUSAGE_BONUS) then
				
				iTotalNumResources = iTotalNumResources + 1;
				
				local instance = {};
				ContextPtr:BuildInstanceForControl( "ResourceEntry", instance, Controls.ResourcesAvailableStack );
		        
				-- Make it a dash instead of a zero, so it stands out more
				if (iNum == 0) then
					iNum = "-";
				end
		        
				instance.ResourceName:SetText( Locale.ConvertTextKey(pResource.Description) );
				instance.ResourceValue:SetText( iNum );
			end
		end
	end

	print("iTotalNumResources: " .. iTotalNumResources);

    if ( iTotalNumResources > 0 ) then
        Controls.ResourcesAvailableToggle:SetDisabled( false );
        Controls.ResourcesAvailableToggle:SetAlpha( 1.0 );
    else
        Controls.ResourcesAvailableToggle:SetDisabled( true );
        Controls.ResourcesAvailableToggle:SetAlpha( 0.5 );
    end
    Controls.ResourcesAvailableStack:CalculateSize();
    Controls.ResourcesAvailableStack:ReprocessAnchoring();
	
	-- Resources Imported
	
    Controls.ResourcesImportedToggle:SetText("[ICON_PLUS]" .. Locale.ConvertTextKey("TXT_KEY_RESOURCES_IMPORTED"));
	
	iTotalNumResources = 0;
	
    Controls.ResourcesImportedStack:DestroyAllChildren();
	for pResource in GameInfo.Resources() do
		local iResourceID = pResource.ID;
		local iNum = pPlayer:GetResourceImport(iResourceID);
		
		if (iNum > 0) then
			-- Only Luxuries or Strategic Resources
			if (Game.GetResourceUsageType(iResourceID) ~= ResourceUsageTypes.RESOURCEUSAGE_BONUS) then
				
				iTotalNumResources = iTotalNumResources + 1;
				
				local instance = {};
				ContextPtr:BuildInstanceForControl( "ResourceEntry", instance, Controls.ResourcesImportedStack );
		        
				-- Make it a dash instead of a zero, so it stands out more
				if (iNum == 0) then
					iNum = "-";
				end
		        
				instance.ResourceName:SetText( Locale.ConvertTextKey(pResource.Description) );
				instance.ResourceValue:SetText( iNum );
			end
		end
	end

    if ( iTotalNumResources > 0 ) then
        Controls.ResourcesImportedToggle:SetDisabled( false );
        Controls.ResourcesImportedToggle:SetAlpha( 1.0 );
    else
        Controls.ResourcesImportedToggle:SetDisabled( true );
        Controls.ResourcesImportedToggle:SetAlpha( 0.5 );
    end
    Controls.ResourcesImportedStack:CalculateSize();
    Controls.ResourcesImportedStack:ReprocessAnchoring();
	
	
	-- Resources Exported
	
    Controls.ResourcesExportedToggle:SetText("[ICON_PLUS]" .. Locale.ConvertTextKey("TXT_KEY_RESOURCES_EXPORTED"));
	
	iTotalNumResources = 0;
	
    Controls.ResourcesExportedStack:DestroyAllChildren();
	for pResource in GameInfo.Resources() do
		local iResourceID = pResource.ID;
		local iNum = pPlayer:GetResourceExport(iResourceID);
		
		if (iNum > 0) then
			-- Only Luxuries or Strategic Resources
			if (Game.GetResourceUsageType(iResourceID) ~= ResourceUsageTypes.RESOURCEUSAGE_BONUS) then
				
				iTotalNumResources = iTotalNumResources + 1;
				
				local instance = {};
				ContextPtr:BuildInstanceForControl( "ResourceEntry", instance, Controls.ResourcesExportedStack );
		        
				-- Make it a dash instead of a zero, so it stands out more
				if (iNum == 0) then
					iNum = "-";
				end
		        
				instance.ResourceName:SetText( Locale.ConvertTextKey(pResource.Description) );
				instance.ResourceValue:SetText( iNum );
			end
		end
	end

    if ( iTotalNumResources > 0 ) then
        Controls.ResourcesExportedToggle:SetDisabled( false );
        Controls.ResourcesExportedToggle:SetAlpha( 1.0 );
    else
        Controls.ResourcesExportedToggle:SetDisabled( true );
        Controls.ResourcesExportedToggle:SetAlpha( 0.5 );
    end
    Controls.ResourcesExportedStack:CalculateSize();
    Controls.ResourcesExportedStack:ReprocessAnchoring();
    
	-- Resources Local
	
    Controls.ResourcesLocalToggle:SetText("[ICON_PLUS]" .. Locale.ConvertTextKey("TXT_KEY_EO_LOCAL_RESOURCES"));
	
	iTotalNumResources = 0;
	
    Controls.ResourcesLocalStack:DestroyAllChildren();
	for pResource in GameInfo.Resources() do
		local iResourceID = pResource.ID;
		local iNum = pPlayer:GetNumResourceTotal(iResourceID, false) + pPlayer:GetResourceExport(iResourceID);
		
		if (iNum > 0) then
			-- Only Luxuries or Strategic Resources
			if (Game.GetResourceUsageType(iResourceID) ~= ResourceUsageTypes.RESOURCEUSAGE_BONUS) then
				
				iTotalNumResources = iTotalNumResources + 1;
				
				local instance = {};
				ContextPtr:BuildInstanceForControl( "ResourceEntry", instance, Controls.ResourcesLocalStack );
		        
				-- Make it a dash instead of a zero, so it stands out more
				if (iNum == 0) then
					iNum = "-";
				end
		        
				instance.ResourceName:SetText( Locale.ConvertTextKey(pResource.Description) );
				instance.ResourceValue:SetText( iNum );
			end
		end
	end

    if ( iTotalNumResources > 0 ) then
        Controls.ResourcesLocalToggle:SetDisabled( false );
        Controls.ResourcesLocalToggle:SetAlpha( 1.0 );
    else
        Controls.ResourcesLocalToggle:SetDisabled( true );
        Controls.ResourcesLocalToggle:SetAlpha( 0.5 );
    end
    Controls.ResourcesLocalStack:CalculateSize();
    Controls.ResourcesLocalStack:ReprocessAnchoring();
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )
    if( not bIsHide ) then
    	UpdateScreen();
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );