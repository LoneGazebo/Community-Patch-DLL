-------------------------------------------------
-- Happiness Info
-------------------------------------------------
include( "InstanceManager" );

Controls.LuxuryHappinessStack:SetHide( true );
Controls.CityBuildingStack:SetHide( true );
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
function OnLuxuryHappinessToggle()
    local bWasHidden = Controls.LuxuryHappinessStack:IsHidden();
    Controls.LuxuryHappinessStack:SetHide( not bWasHidden );
    if( bWasHidden ) then
        Controls.LuxuryHappinessToggle:LocalizeAndSetText("TXT_KEY_EO_LUXURY_HAPPINESS_COLLAPSE");
    else
        Controls.LuxuryHappinessToggle:LocalizeAndSetText("TXT_KEY_EO_LUXURY_HAPPINESS");
    end
    Controls.HappinessStack:CalculateSize();
    Controls.HappinessStack:ReprocessAnchoring();
    Controls.HappinessScroll:CalculateInternalSize();
end
Controls.LuxuryHappinessToggle:RegisterCallback( Mouse.eLClick, OnLuxuryHappinessToggle );

-------------------------------------------------
-------------------------------------------------
function OnCityBuildingToggle()
    local bWasHidden = Controls.CityBuildingStack:IsHidden();
    Controls.CityBuildingStack:SetHide( not bWasHidden );
    if( bWasHidden ) then
        Controls.CityBuildingToggle:LocalizeAndSetText("TXT_KEY_EO_CITY_BUILDINGS_COLLAPSE");
    else
        Controls.CityBuildingToggle:LocalizeAndSetText("TXT_KEY_EO_CITY_BUILDINGS");
    end
    Controls.HappinessStack:CalculateSize();
    Controls.HappinessStack:ReprocessAnchoring();
    Controls.HappinessScroll:CalculateInternalSize();
end
Controls.CityBuildingToggle:RegisterCallback( Mouse.eLClick, OnCityBuildingToggle );

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
	
	Controls.TotalHappinessValue:SetText("[COLOR_POSITIVE_TEXT]" .. pPlayer:GetHappiness() .. "[ENDCOLOR]");
	
	local iHappiness = pPlayer:GetExcessHappiness();

	local iPoliciesHappiness = pPlayer:GetHappinessFromPolicies();
	local iResourcesHappiness = pPlayer:GetHappinessFromResources();
	local iExtraLuxuryHappiness = pPlayer:GetExtraHappinessPerLuxury();
	local iBuildingHappiness = pPlayer:GetHappinessFromBuildings();
	local iCityHappiness = pPlayer:GetHappinessFromCities();
	local iTradeRouteHappiness = pPlayer:GetHappinessFromTradeRoutes();
	local iReligionHappiness = pPlayer:GetHappinessFromReligion();
	local iNaturalWonderHappiness = pPlayer:GetHappinessFromNaturalWonders();
	local iExtraHappinessPerCity = pPlayer:GetExtraHappinessPerCity() * pPlayer:GetNumCities();
	local iMinorCivHappiness = pPlayer:GetHappinessFromMinorCivs();
	local iLeagueHappiness = pPlayer:GetHappinessFromLeagues();
	-- CBP
	local iHappinessFromMonopoly = pPlayer:GetHappinessFromResourceMonopolies();
	local iHappinessFromBonusResources = pPlayer:GetBonusHappinessFromLuxuries();
	-- END
	-- C4DF
	local iHappinessFromVassal = pPlayer:GetHappinessFromVassals();
	-- END
	
	-- EDIT
	local iHandicapHappiness = pPlayer:GetHappiness() - iPoliciesHappiness - iHappinessFromMonopoly - iHappinessFromBonusResources - iResourcesHappiness - iBuildingHappiness - iCityHappiness - iTradeRouteHappiness - iReligionHappiness - iNaturalWonderHappiness - iMinorCivHappiness - iExtraHappinessPerCity - iLeagueHappiness - iHappinessFromVassal;

	local iTotalHappiness = iHappiness;
	
	-- Luxury Resource Details

	local iBaseHappinessFromResources = 0;
	local iNumHappinessResources = 0;
	
    Controls.LuxuryHappinessStack:DestroyAllChildren();

	for resource in GameInfo.Resources() do
		local resourceID = resource.ID;
		local iHappiness = pPlayer:GetHappinessFromLuxury(resourceID);
		if (iHappiness > 0) then
			iNumHappinessResources = iNumHappinessResources + 1;
			iBaseHappinessFromResources = iBaseHappinessFromResources + iHappiness;
				
			local instance = {};
			ContextPtr:BuildInstanceForControl( "TradeEntry", instance, Controls.LuxuryHappinessStack );
	            
			instance.CityName:SetText( Locale.ConvertTextKey(resource.Description));
			instance.TradeIncomeValue:SetText( Locale.ToNumber( iHappiness, "#.##" ) );
		end
	end
	
	Controls.LuxuryHappinessValue:SetText(iBaseHappinessFromResources);
    
    if( iBaseHappinessFromResources > 0 ) then
        Controls.LuxuryHappinessToggle:SetDisabled( false );
        Controls.LuxuryHappinessToggle:SetAlpha( 1.0 );
    else
        Controls.LuxuryHappinessToggle:SetDisabled( true );
        Controls.LuxuryHappinessToggle:SetAlpha( 0.5 );
    end
    Controls.LuxuryHappinessStack:CalculateSize();
    Controls.LuxuryHappinessStack:ReprocessAnchoring();
	
	-- Happiness from Luxury Variety
	local iHappinessFromExtraResources = pPlayer:GetHappinessFromResourceVariety();
	if (iHappinessFromExtraResources > 0) then
		Controls.LuxuryVarietyValue:SetText(iHappinessFromExtraResources);
		Controls.LuxuryVariety:SetHide(false);
	else
		Controls.LuxuryVariety:SetHide(true);
	end
	
	-- Extra Happiness from each Luxury
	if (iExtraLuxuryHappiness >= 1) then
		Controls.LuxuryBonusValue:SetText(iExtraLuxuryHappiness);
		Controls.LuxuryBonus:SetHide(false);
	else
		Controls.LuxuryBonus:SetHide(true);
	end
	
	-- Misc Happiness from Resources
	local iMiscHappiness = iResourcesHappiness - iBaseHappinessFromResources - iHappinessFromExtraResources - (iExtraLuxuryHappiness * iNumHappinessResources);
	if (iMiscHappiness > 0) then
		Controls.LuxuryMiscValue:SetText(iMiscHappiness);
		Controls.LuxuryMisc:SetHide(false);
	else
		Controls.LuxuryMisc:SetHide(true);
	end
	
	-- City Buildings
    Controls.CityBuildingStack:DestroyAllChildren();
    for pCity in pPlayer:Cities() do
	    
	    local iCityHappiness = pCity:GetHappiness();
	    
		local instance = {};
		ContextPtr:BuildInstanceForControl( "TradeEntry", instance, Controls.CityBuildingStack );
        
        -- Make it a dash instead of a zero, so it stands out more
        if (iCityHappiness == 0) then
			iCityHappiness = "-";
        end
        
        instance.CityName:SetText( pCity:GetName() );
		instance.TradeIncomeValue:SetText( iCityHappiness );
	end
	
	Controls.CityBuildingValue:SetText(iBuildingHappiness);
    
    if( iBuildingHappiness > 0 ) then
        Controls.CityBuildingToggle:SetDisabled( false );
        Controls.CityBuildingToggle:SetAlpha( 1.0 );
    else
        Controls.CityBuildingToggle:SetDisabled( true );
        Controls.CityBuildingToggle:SetAlpha( 0.5 );
    end
    Controls.CityBuildingStack:CalculateSize();
    Controls.CityBuildingStack:ReprocessAnchoring();
	
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
	
	if (iCityHappiness > 0) then
		Controls.LocalCityStack:DestroyAllChildren();
		for pCity in pPlayer:Cities() do
		    
		    local strLocalCity = "-";
		    
			if (pCity:GetLocalHappiness() > 0) then
				strLocalCity = pCity:GetLocalHappiness();
			end
			
			local instance = {};
			ContextPtr:BuildInstanceForControl( "TradeEntry", instance, Controls.LocalCityStack );
	        
			instance.CityName:SetText( pCity:GetName() );
			instance.TradeIncomeValue:SetText( strLocalCity );
		end
		
		Controls.LocalCityValue:SetText(iCityHappiness);
	end
    
    if( iCityHappiness > 0 ) then
        Controls.LocalCityToggle:SetHide( false );
    else
        Controls.LocalCityToggle:SetHide( true );
    end
    Controls.LocalCityStack:CalculateSize();
    Controls.LocalCityStack:ReprocessAnchoring();
	
	-- City-States
	Controls.MinorCivHappinessValue:SetText(iMinorCivHappiness);
	
	-- Policies
	Controls.PoliciesHappinessValue:SetText(iPoliciesHappiness);
	
	-- Religion
	Controls.ReligionHappinessValue:SetText(iReligionHappiness);
	
	-- Natural Wonders
	if (iNaturalWonderHappiness > 0) then
		Controls.NaturalWondersValue:SetText(iNaturalWonderHappiness);
		Controls.NaturalWonders:SetHide(false);
	else
		Controls.NaturalWonders:SetHide(true);
	end
	
	-- Free Happiness per City
	if (iExtraHappinessPerCity > 0) then
		Controls.FreeCityHappinessValue:SetText(iExtraHappinessPerCity);
		Controls.FreeCityHappiness:SetHide(false);
	else
		Controls.FreeCityHappiness:SetHide(true);
	end
	
	-- Leagues
	if (iLeagueHappiness ~= 0) then
		Controls.LeagueHappinessValue:SetText(iLeagueHappiness);
		Controls.LeagueHappiness:SetHide(false);
	else
		Controls.LeagueHappiness:SetHide(true);
	end

	-- CBP
	if (iHappinessFromMonopoly ~= 0) then
		Controls.MonopolyHappinessValue:SetText(iHappinessFromMonopoly);
		Controls.MonopolyHappiness:SetHide(false);
	else
		Controls.MonopolyHappiness:SetHide(true);
	end

	if (iHappinessFromBonusResources ~= 0) then
		Controls.ExtraLuxuryHappinessValue:SetText(iHappinessFromBonusResources);
		Controls.ExtraLuxuryHappiness:SetHide(false);
	else
		Controls.ExtraLuxuryHappiness:SetHide(true);
	end

	-- END
	-- C4DF
	if (iHappinessFromVassal ~= 0) then
		Controls.VassalHappinessValue:SetText(iHappinessFromVassal);
		Controls.VassalHappiness:SetHide(false);
	else
		Controls.VassalHappiness:SetHide(true);
	end
	
	-- END
	-- iHandicapHappiness
	Controls.HandicapHappinessValue:SetText(iHandicapHappiness);



	-----------------------------------------------
	-- Unhappiness
	-----------------------------------------------
	
	local pHandicap = GameInfo.HandicapInfos[iHandicap];
	
	local iTotalUnhappiness = pPlayer:GetUnhappiness();
	local iUnhappinessFromUnits = Locale.ToNumber( pPlayer:GetUnhappinessFromUnits() / 100, "#.##" );
	local iUnhappinessFromCityCount = Locale.ToNumber( pPlayer:GetUnhappinessFromCityCount() / 100, "#.##" );
	local iUnhappinessFromCapturedCityCount = Locale.ToNumber( pPlayer:GetUnhappinessFromCapturedCityCount() / 100, "#.##" );
	local iUnhappinessFromPop = Locale.ToNumber( pPlayer:GetUnhappinessFromCityPopulation() / 100, "#.##" );
	local iUnhappinessFromOccupiedCities = Locale.ToNumber( pPlayer:GetUnhappinessFromOccupiedCities() / 100, "#.##" );
	
	local iOccupiedPop = 0;
	local iNumOccupiedCities = 0;
	for pCity in pPlayer:Cities() do
		if (pCity:IsOccupied() and not pCity:IsNoOccupiedUnhappiness()) then
			iNumOccupiedCities = iNumOccupiedCities + 1;
			iOccupiedPop = iOccupiedPop + pCity:GetPopulation();
		end
	end
	
	local iNumNormalCities = pPlayer:GetNumCities() - iNumOccupiedCities;
	local iNormalPop = pPlayer:GetTotalPopulation() - iOccupiedPop;
	
	-- Total Unhappiness
	Controls.TotalUnhappinessValue:SetText("[COLOR_NEGATIVE_TEXT]" .. iTotalUnhappiness .. "[ENDCOLOR]");
	
	-- Normal City Count
	Controls.CityCountUnhappinessTitle:SetText(Locale.ConvertTextKey("TXT_KEY_NUMBER_OF_CITIES", iNumNormalCities));
	Controls.CityCountUnhappinessValue:SetText(iUnhappinessFromCityCount);
	
	local strTTExtraInfo = "";
	
	local iCityCountMod = pHandicap.NumCitiesUnhappinessMod;	-- Handicap
	if (iCityCountMod ~= 100) then
		strTTExtraInfo = strTTExtraInfo .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_NUMBER_OF_CITIES_HANDICAP_TT", 100 - iCityCountMod);
	end
	
	iCityCountMod = pPlayer:GetCityCountUnhappinessMod();		-- Player Mod
	if (iCityCountMod ~= 0) then
		strTTExtraInfo = strTTExtraInfo .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_UNHAPPINESS_MOD_PLAYER", iCityCountMod);
	end
	
	iCityCountMod = pPlayer:GetTraitCityUnhappinessMod();		-- Trait Mod
	if (iCityCountMod ~= 0) then
		strTTExtraInfo = strTTExtraInfo .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_UNHAPPINESS_MOD_TRAIT", iCityCountMod);
	end
	
	local iCityCountMod = Game:GetWorldNumCitiesUnhappinessPercent();	-- World Size
	if (iCityCountMod ~= 100) then
		strTTExtraInfo = strTTExtraInfo .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_UNHAPPINESS_MOD_MAP", 100 - iCityCountMod);
	end
	
	local strTooltip;
	if (strTTExtraInfo~= "") then
		strTooltip = Locale.Lookup("TXT_KEY_NUMBER_OF_CITIES_TT_NORMALLY") .. strTTExtraInfo;
	else
		strTooltip = Locale.Lookup("TXT_KEY_NUMBER_OF_CITIES_TT");
	end
	
	Controls.CityCountUnhappiness:SetToolTipString(strTooltip);
	
	-- Occupied City Count
	if (iUnhappinessFromCapturedCityCount ~= "0") then
		Controls.OCityCountUnhappinessTitle:SetText(Locale.ConvertTextKey("TXT_KEY_NUMBER_OF_OCCUPIED_CITIES", iNumOccupiedCities));
		Controls.OCityCountUnhappinessValue:SetText(iUnhappinessFromCapturedCityCount);
		
		strTooltip = Locale.ConvertTextKey("TXT_KEY_NUMBER_OF_OCCUPIED_CITIES_TT");
		strTTExtraInfo = "";
		
		local iCityCountMod = pHandicap.NumCitiesUnhappinessMod;	-- Handicap
		if (iCityCountMod ~= 100) then
			strTTExtraInfo = strTTExtraInfo .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_NUMBER_OF_CITIES_HANDICAP_TT", 100 - iCityCountMod);
		end
		
		iCityCountMod = pPlayer:GetCityCountUnhappinessMod();		-- Player Mod
		if (iCityCountMod ~= 0) then
			strTTExtraInfo = strTTExtraInfo .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_UNHAPPINESS_MOD_PLAYER", iCityCountMod);
		end
		
		iCityCountMod = pPlayer:GetTraitCityUnhappinessMod();		-- Trait Mod
		if (iCityCountMod ~= 0) then
			strTTExtraInfo = strTTExtraInfo .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_UNHAPPINESS_MOD_TRAIT", iCityCountMod);
		end
		
		local iCityCountMod = Game:GetWorldNumCitiesUnhappinessPercent();	-- World Size
		if (iCityCountMod ~= 100) then
			strTTExtraInfo = strTTExtraInfo .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_UNHAPPINESS_MOD_MAP", 100 - iCityCountMod);
		end
		
		if (strTTExtraInfo~= "") then
			strTooltip = strTooltip .. " " .. Locale.ConvertTextKey("TXT_KEY_NORMALLY") .. "." .. strTTExtraInfo;
		else
			strTooltip = strTooltip .. ".";
		end
			
		Controls.OCityCountUnhappiness:SetToolTipString(strTooltip);
		Controls.OCityCountUnhappiness:SetHide(false);
	else
		Controls.OCityCountUnhappiness:SetHide(true);
	end
	
	-- Normal Population
	Controls.PopulationUnhappinessTitle:SetText(Locale.ConvertTextKey("TXT_KEY_POP_UNHAPPINESS", iNormalPop));
	Controls.PopulationUnhappinessValue:SetText(iUnhappinessFromPop);
	
	strTooltip = Locale.ConvertTextKey("TXT_KEY_POP_UNHAPPINESS_TT");
	strTTExtraInfo = "";
	
	local iPopulationMod = pHandicap.PopulationUnhappinessMod;	-- Handicap
	if (iPopulationMod ~= 100) then
		strTTExtraInfo = strTTExtraInfo .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_NUMBER_OF_CITIES_HANDICAP_TT", 100 - iPopulationMod);
	end
	
	iCityCountMod = pPlayer:GetUnhappinessMod();		-- Player Mod
	if (iCityCountMod ~= 0) then
		strTTExtraInfo = strTTExtraInfo .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_UNHAPPINESS_MOD_PLAYER", iCityCountMod);
	end
	
	iCityCountMod = pPlayer:GetTraitPopUnhappinessMod();		-- Trait Mod
	if (iCityCountMod ~= 0) then
		strTTExtraInfo = strTTExtraInfo .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_UNHAPPINESS_MOD_TRAIT", iCityCountMod);
	end
	
	iCityCountMod = pPlayer:GetCapitalUnhappinessMod();		-- Capital Mod
	if (iCityCountMod ~= 0) then
		strTTExtraInfo = strTTExtraInfo .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_UNHAPPINESS_MOD_CAPITAL", iCityCountMod);
	end
	
	if (pPlayer:IsHalfSpecialistUnhappiness()) then		-- Specialist Happiness
		strTTExtraInfo = strTTExtraInfo .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_UNHAPPINESS_MOD_SPECIALIST");
	end
	
	if (strTTExtraInfo ~= "") then
		strTooltip = strTooltip .. " " .. Locale.ConvertTextKey("TXT_KEY_NORMALLY") .. "." .. strTTExtraInfo;
	else
		strTooltip = strTooltip .. ".";
	end
	
	Controls.PopulationUnhappiness:SetToolTipString(strTooltip);
	
	-- Occupied City Count
	if (iOccupiedPop ~= 0) then
		Controls.OPopulationUnhappinessTitle:SetText(Locale.ConvertTextKey("TXT_KEY_OCCUPIED_POP_UNHAPPINESS", iOccupiedPop));
		Controls.OPopulationUnhappinessValue:SetText(iUnhappinessFromOccupiedCities);
		
		strTooltip = Locale.ConvertTextKey("TXT_KEY_OCCUPIED_POP_UNHAPPINESS_TT");
		strTTExtraInfo = "";
		
		if (iPopulationMod ~= 100) then		-- Handicap
			strTTExtraInfo = strTTExtraInfo .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_NUMBER_OF_CITIES_HANDICAP_TT", 100 - iPopulationMod);
		end
		
		iCityCountMod = pPlayer:GetOccupiedPopulationUnhappinessMod();		-- Player Mod
		if (iCityCountMod ~= 0) then
			strTTExtraInfo = strTTExtraInfo .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_UNHAPPINESS_MOD_PLAYER", iCityCountMod);
		end
		
		if (pPlayer:IsHalfSpecialistUnhappiness()) then		-- Specialist Happiness
			strTTExtraInfo = strTTExtraInfo .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_UNHAPPINESS_MOD_SPECIALIST");
		end
		
		if (strTTExtraInfo ~= "") then
			strTooltip = strTooltip .. " " .. Locale.ConvertTextKey("TXT_KEY_NORMALLY") .. "." .. strTTExtraInfo;
		else
			strTooltip = strTooltip .. ".";
		end
		
		Controls.OPopulationUnhappiness:SetToolTipString(strTooltip);
		Controls.OPopulationUnhappiness:SetHide(false);
	else
		Controls.OPopulationUnhappiness:SetHide(true);
	end
	
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
    Controls.CityUnhappinessStack:DestroyAllChildren();
    for pCity in pPlayer:Cities() do
	    
	    local fUnhappinessTimes100 = pPlayer:GetUnhappinessFromCityForUI(pCity);
		-- COMMUNITY CHANGE
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
		if(pCity:IsOccupied() and not pCity:IsNoOccupiedUnhappiness()) then
			iOccupationUnhappiness = (pCity:GetPopulation() * GameDefines.UNHAPPINESS_PER_OCCUPIED_POPULATION);
		end

		local iTotalUnhappyCP = (iScienceUnhappiness + iCultureUnhappiness + iDefenseUnhappiness + iGoldUnhappiness + iConnectionUnhappiness + iPillagedUnhappiness + iStarvingUnhappiness + iMinorityUnhappiness + iResistanceUnhappiness + iOccupationUnhappiness) * 100;
		fUnhappinessTimes100 = fUnhappinessTimes100 + iTotalUnhappyCP;
		--END
		local instance = {};
		ContextPtr:BuildInstanceForControl( "TradeEntry", instance, Controls.CityUnhappinessStack );
        
        -- Make it a dash instead of a zero, so it stands out more
        if (fUnhappinessTimes100 == 0) then
			fUnhappinessTimes100 = "-";
		else
			fUnhappinessTimes100 = fUnhappinessTimes100 / 100;
        end
        
        instance.CityName:SetText( pCity:GetName() );
		instance.TradeIncomeValue:SetText( Locale.ToNumber( fUnhappinessTimes100, "#.##" ) );
		
		-- Occupation tooltip
		strOccupationTT = Locale.ConvertTextKey("TXT_KEY_EO_CITY_IS_NOT_OCCUPIED");
		if (pCity:IsOccupied() and not pCity:IsNoOccupiedUnhappiness()) then
			strOccupationTT = Locale.ConvertTextKey("TXT_KEY_EO_CITY_IS_OCCUPIED");
		end

-- COMMUNITY CHANGE
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

-- END CHANGE


		
		instance.TradeIncome:SetToolTipString(strOccupationTT);
	end
    
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