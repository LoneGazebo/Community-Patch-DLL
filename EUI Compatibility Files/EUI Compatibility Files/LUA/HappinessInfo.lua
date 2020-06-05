-------------------------------------------------
-- Happiness Info
-------------------------------------------------
include( "InstanceManager" );

Controls.LocalCityStack:SetHide( false );
Controls.CityUnhappinessStack:SetHide( false );
Controls.ResourcesAvailableStack:SetHide( true );
Controls.ResourcesImportedStack:SetHide( true );
Controls.ResourcesExportedStack:SetHide( true );
Controls.ResourcesLocalStack:SetHide( true );

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
	
	-----------------------------------------------
	-- Happiness
	-----------------------------------------------
	
	local iTotalCityHappiness = 0;

	Controls.LocalCityStack:DestroyAllChildren();
	for pCity in pPlayer:Cities() do
		    
		local iLocalHappiness = pCity:GetLocalHappiness();
		iTotalCityHappiness = iTotalCityHappiness + iLocalHappiness;
			
		local instance = {};
		ContextPtr:BuildInstanceForControl( "TradeEntry", instance, Controls.LocalCityStack );
	        
		instance.CityName:SetText( pCity:GetName() );
		instance.TradeIncomeValue:SetText( iLocalHappiness );
		instance.TradeIncome:SetToolTipString(pCity:GetCityHappinessBreakdown());
	end
		
	Controls.LocalCityValue:SetText(iTotalCityHappiness);
    
    Controls.LocalCityToggle:SetHide( false );
    Controls.LocalCityStack:CalculateSize();
    Controls.LocalCityStack:ReprocessAnchoring();

	-----------------------------------------------
	-- Unhappiness
	-----------------------------------------------

    -- do not show empire totals as they are already included in the city breakdown
    -- also you can get them from the top bar happiness tooltip

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
		        
				instance.ResourceName:SetText( Locale.ConvertTextKey(pResource.Description) );
				instance.ResourceValue:SetText( iNum );
			end
		end
	end

	Controls.ResourcesAvailableValue:SetText(iTotalNumResources);

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
		local iNum = pPlayer:GetResourceImport(iResourceID) + pPlayer:GetResourceFromMinors(iResourceID);
		
		if (iNum > 0) then
			-- Only Luxuries or Strategic Resources
			if (Game.GetResourceUsageType(iResourceID) ~= ResourceUsageTypes.RESOURCEUSAGE_BONUS) then
				
				iTotalNumResources = iTotalNumResources + 1;
				
				local instance = {};
				ContextPtr:BuildInstanceForControl( "ResourceEntry", instance, Controls.ResourcesImportedStack );
		        
				instance.ResourceName:SetText( Locale.ConvertTextKey(pResource.Description) );
				instance.ResourceValue:SetText( iNum );
			end
		end
	end

	Controls.ResourcesImportedValue:SetText(iTotalNumResources);

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
		        
				instance.ResourceName:SetText( Locale.ConvertTextKey(pResource.Description) );
				instance.ResourceValue:SetText( iNum );
			end
		end
	end

	Controls.ResourcesExportedValue:SetText(iTotalNumResources);

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
		        
				instance.ResourceName:SetText( Locale.ConvertTextKey(pResource.Description) );
				instance.ResourceValue:SetText( iNum );
			end
		end
	end

	Controls.ResourcesLocalValue:SetText(iTotalNumResources);

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