print("This is the modded EconomicGeneralInfo from CBP")
-------------------------------------------------
-- Economic
-------------------------------------------------
include( "IconSupport" );
include( "InfoTooltipInclude" );
include( "InstanceManager" );
include( "SupportFunctions" );
include( "CPK.lua" )

local FormatDecimal = CPK.Text.FormatDecimal
local FormatDecimalTimes100 = CPK.Text.FormatDecimalTimes100

local defaultErrorTextureSheet = "CityBannerProductionImage.dds";
local nullOffset = Vector2( 0, 0 );
local g_iPortraitSize = 45;

local m_SortTable;
local ePopulation     = 0;
local eName   = 1;
local eUnhappiness = 2;
local eLocalHappiness = 10;
local eProduction = 3;
local eFood = 5;
local eResearch = 6;
local eGold = 7;
local eCulture = 8;
local eFaith = 9;
local eReligion = 11;
local eFocus = 12;

local eTourism = 13;
local eStrength = 14;
local eSecurity = 15;

local m_SortMode = ePopulation;
local m_bSortReverse = false;

local pediaSearchStrings = {};

-- fix for DB cache issue, by merill
local DB_HandicapInfos = {};
if Game then
	for realHandicap in DB.Query("SELECT * FROM HandicapInfos") do
		DB_HandicapInfos[realHandicap["ID"]] = {};
		for key,val in pairs(realHandicap) do 
			DB_HandicapInfos[realHandicap["ID"]][key] = val;
		end
	end
end

-------------------------------------------------
-------------------------------------------------
-- Close the overview before selecting a city, so CityView opens in front.
local function OpenCityFromOverview(x, y)
    local plot = Map.GetPlot(x, y);
    if (plot) then
        Events.SerialEventGameMessagePopup({
            Type = ButtonPopupTypes.BUTTONPOPUP_ECONOMIC_OVERVIEW,
            Data1 = 1
        });
        UI.DoSelectCityAtPlot(plot);
    end
end

function OnProdClick(x, y)
    OpenCityFromOverview(x, y);
end

-------------------------------------------------
-------------------------------------------------
function OnProdRClick( cityID, void2, button )
	local searchString = pediaSearchStrings[tostring(button)];
	Events.SearchForPediaEntry( searchString );		
end
	
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

-- Display city-banner status indicators beside the city name.
local g_CityFocus = {
    [CityAIFocusTypes.CITY_AI_FOCUS_TYPE_FOOD] = { "[ICON_FOOD]", "TXT_KEY_CITYVIEW_FOCUS_FOOD_TEXT" };
    [CityAIFocusTypes.CITY_AI_FOCUS_TYPE_PRODUCTION] = { "[ICON_PRODUCTION]", "TXT_KEY_CITYVIEW_FOCUS_PROD_TEXT" };
    [CityAIFocusTypes.CITY_AI_FOCUS_TYPE_GOLD] = { "[ICON_GOLD]", "TXT_KEY_CITYVIEW_FOCUS_GOLD_TEXT" };
    [CityAIFocusTypes.CITY_AI_FOCUS_TYPE_SCIENCE] = { "[ICON_RESEARCH]", "TXT_KEY_CITYVIEW_FOCUS_RESEARCH_TEXT" };
    [CityAIFocusTypes.CITY_AI_FOCUS_TYPE_CULTURE] = { "[ICON_CULTURE]", "TXT_KEY_CITYVIEW_FOCUS_CULTURE_TEXT" };
    [CityAIFocusTypes.CITY_AI_FOCUS_TYPE_GREAT_PEOPLE] = { "[ICON_GREAT_PEOPLE]", "TXT_KEY_CITYVIEW_FOCUS_GREAT_PERSON_TEXT" };
    [CityAIFocusTypes.CITY_AI_FOCUS_TYPE_FAITH] = { "[ICON_PEACE]", "TXT_KEY_CITYVIEW_FOCUS_FAITH_TEXT" };
};

local function UpdateCityIndicators(pCity, instance, spiesByCity)
    local icons = {};
    local descriptions = {};
    local function Add(icon, description)
        table.insert(icons, icon);
        table.insert(descriptions, description);
    end

    local pPlayer = Players[pCity:GetOwner()];

    if (pCity:IsForcedAvoidGrowth()) then
        Add("[ICON_LOCKED]", Locale.ConvertTextKey("TXT_KEY_CITYVIEW_FOCUS_AVOID_GROWTH_TEXT"));
    end
    if (pCity:IsResistance()) then
        Add("[ICON_RESISTANCE]", Locale.ConvertTextKey("TXT_KEY_CITY_RESISTANCE", pCity:GetResistanceTurns()));
    end
    if (pCity:IsRazing()) then
        Add("[ICON_RAZING]", Locale.ConvertTextKey("TXT_KEY_CITY_BURNING", pCity:GetRazingTurns()));
    end
    if (pCity:IsBlockaded()) then
        if (pCity:GetSappedTurns() > 0) then
            Add("[ICON_SAPPED]", Locale.ConvertTextKey("TXT_KEY_CITY_SAPPED", pCity:GetSappedTurns()));
        else
            Add("[ICON_BLOCKADED]", Locale.ConvertTextKey("TXT_KEY_CITY_BLOCKADED"));
        end
    end

    -- Keep city connections in sync with the CityBannerManager rules.
    if (not pCity:IsCapital() and not pCity:IsBlockaded()) then
        if (pCity:IsIndustrialConnectedToCapital()) then
            Add("[ICON_INDUSTRIAL_CONNECTED]", Locale.ConvertTextKey("TXT_KEY_CITY_INDUSTRIAL_CONNECTED"));
        elseif (pCity:IsConnectedToCapital()) then
            Add("[ICON_CONNECTED]", Locale.ConvertTextKey("TXT_KEY_CITY_CONNECTED"));
        end
    end

    if (pCity:GetWeLoveTheKingDayCounter() > 0) then
        Add("[ICON_HAPPINESS_1]", Locale.ConvertTextKey("TXT_KEY_CITYVIEW_WLTKD_COUNTER", pCity:GetWeLoveTheKingDayCounter()));
    else
        local resource = GameInfo.Resources[pCity:GetResourceDemanded(true)];
        if (resource) then
            Add(resource.IconString, Locale.ConvertTextKey("TXT_KEY_CITYVIEW_RESOURCE_DEMANDED", Locale.ConvertTextKey(resource.Description)));
        end
    end
    if (pCity:IsMined()) then
        Add("[ICON_MINEFIELD]", Locale.ConvertTextKey("TXT_KEY_CITY_MINED"));
    end

    local spy = spiesByCity[pCity:GetX() .. "," .. pCity:GetY()];
    if (spy) then
        local spyName = Locale.Lookup(spy.Name);
        local spyRank = Locale.Lookup(spy.Rank);
        if (spy.IsDiplomat) then
            Add("[ICON_DIPLOMAT]", Locale.ConvertTextKey("TXT_KEY_CITY_DIPLOMAT_OTHER_CIV_TT", spyRank, spyName, pCity:GetName(), spyRank, spyName, spyRank, spyName));
        else
            Add("[ICON_SPY]", Locale.ConvertTextKey("TXT_KEY_CITY_SPY_YOUR_CITY_TT", spyRank, spyName, pCity:GetName(), spyRank, spyName));
        end
    end
    -- The following status icons are supplied by VP's CityBannerManager.
    if (pCity:HasOffice()) then
        local corporation = GameInfo.Corporations[pPlayer:GetCorporation()];
        if (corporation) then
            local tooltipKey = Game.GetCorporationHeadquarters(corporation.ID) == pCity
                and "TXT_KEY_CORPORATION_OFFICEHQ_TT" or "TXT_KEY_CORPORATION_OFFICE_TT";
            Add("[ICON_OFFICE]", Locale.ConvertTextKey(tooltipKey, corporation.Description));
        end
    end
    if (Game.GetNumCorporationsFounded() > 0) then
        local franchises = {};
        for corporation in GameInfo.Corporations() do
            local founder = Game.GetCorporationFounder(corporation.ID);
            if (founder ~= -1 and pCity:IsFranchised(founder)) then
                local building = Players[founder]:GetSpecificBuildingType(corporation.FranchiseBuildingClass);
                if (building ~= -1 and GameInfo.Buildings[building]) then
                    table.insert(franchises, "[ICON_BULLET][COLOR_POSITIVE_TEXT]" .. Locale.ConvertTextKey(GameInfo.Buildings[building].Description) .. "[ENDCOLOR]: " .. Locale.ConvertTextKey(GameInfo.Buildings[building].Help));
                end
            end
        end
        if (#franchises > 0) then
            Add("[ICON_FRANCHISE]", table.concat(franchises, "[NEWLINE]"));
        end
    end
    if (pCity:CanAirlift()) then
        Add("[ICON_AIRPORT]", Locale.ConvertTextKey("TXT_KEY_CITY_HAS_AIRLIFT"));
    end
    if (pCity:CanSealift()) then
        Add("[ICON_CARGO_SHIP]", Locale.ConvertTextKey("TXT_KEY_CITY_HAS_SEALIFT"));
    end
    if (pCity:IsBorderObstacleLand()) then
        Add("[ICON_BORDER_OBSTRUCTION]", Locale.ConvertTextKey("TXT_KEY_CITY_HAS_OBSTACLE"));
    end
    if (pCity:IsProductionAutomated() and not pCity:IsPuppet()) then
        Add("[ICON_AUTOMATED]", Locale.ConvertTextKey("TXT_KEY_CITY_PRODUCTION_AUTOMATED"));
    end
    if (pCity:getHappinessDelta() < 0) then
        local unhappy = -pCity:getHappinessDelta();
        Add("[ICON_HAPPINESS_3]", Locale.ConvertTextKey("TXT_KEY_CITY_UNHAPPY", unhappy));
    end
    -- VP can grant yields from annexed city-state traits.
    if (Players[pCity:GetOriginalOwner()]:IsMinorCiv() and pPlayer:IsAnnexedCityStatesGiveYields()) then
        local traitKeys = {
            [MinorCivTraitTypes.MINOR_CIV_TRAIT_CULTURED] = "TXT_KEY_CITY_STATE_CULTURED_TT_ANNEXED",
            [MinorCivTraitTypes.MINOR_CIV_TRAIT_MILITARISTIC] = "TXT_KEY_CITY_STATE_MILITARISTIC_NO_UU_TT_ANNEXED",
            [MinorCivTraitTypes.MINOR_CIV_TRAIT_MARITIME] = "TXT_KEY_CITY_STATE_MARITIME_TT_ANNEXED",
            [MinorCivTraitTypes.MINOR_CIV_TRAIT_MERCANTILE] = "TXT_KEY_CITY_STATE_MERCANTILE_TT_ANNEXED",
            [MinorCivTraitTypes.MINOR_CIV_TRAIT_RELIGIOUS] = "TXT_KEY_CITY_STATE_RELIGIOUS_TT_ANNEXED"
        };
        local tooltipKey = traitKeys[Players[pCity:GetOriginalOwner()]:GetMinorCivTrait()];
        Add("[ICON_CITY_STATE]", tooltipKey and Locale.ConvertTextKey(tooltipKey) or "");
    end

    instance.CityStatus:SetText(table.concat(icons, ""));
    -- Do not truncate extra status icons at the old 90px cap.
    instance.CityStatus:SetSizeX(#icons * 18);
    local capitalWidth = (pCity:IsCapital() or pCity:IsPuppet() or
        (pCity:IsOccupied() and not pCity:IsNoOccupiedUnhappiness())) and 18 or 0;
    TruncateString(instance.CityName, math.min(132, math.max(64, 260 - #icons * 18 - capitalWidth)), pCity:GetName());
    instance.CityStatus:SetToolTipString(table.concat(descriptions, "[NEWLINE]"));

    local focus = g_CityFocus[pCity:GetFocusType()];
    instance.CityFocus:SetText(focus and focus[1] or "[ICON_RANGE_STRENGTH]");
    instance.CityFocus:SetToolTipString(focus and Locale.ConvertTextKey(focus[2]) or Locale.ConvertTextKey("TXT_KEY_CITYVIEW_FOCUS_BALANCED_TEXT"));

    local majority = pCity:GetReligiousMajority();
    local religion = GameInfo.Religions[majority];
    local followers = 0;
    if (religion) then
        followers = pCity:GetNumFollowers(majority);
        local name = Locale.ConvertTextKey(religion.Description);
        instance.ReligionFollowers:SetText(followers);
        instance.ReligionFollowers:SetToolTipString(name);
        IconHookup(religion.PortraitIndex, 32, religion.IconAtlas, instance.ReligionIcon);
        instance.ReligionIcon:SetToolTipString(name);
        instance.ReligionIcon:SetHide(false);
    else
        instance.ReligionFollowers:SetText("");
        instance.ReligionIcon:SetHide(true);
    end
    -- CityBannerManager uses the same followers/pressure-only helper.
    local religionTooltip = GetReligionTooltip(pCity);
    instance.ReligionBox:SetToolTipString(religionTooltip);
    instance.ReligionFollowers:SetToolTipString(religionTooltip);
    instance.ReligionIcon:SetToolTipString(religionTooltip);
    instance.ReligionContents:CalculateSize();
    instance.ReligionContents:ReprocessAnchoring();
    return followers;
end

-- Use the CityView tooltip template, not the shared TopPanel happiness tooltip.
-- The DLL yield breakdown is generated only when a city yield is hovered.
local g_YieldTooltipControls = {};
TTManager:GetTypeControlTable("CityViewTooltip", g_YieldTooltipControls);

local function SetCityYieldTooltip(instance, field, playerID, cityID, yieldType)
    local function ShowTooltip()
        local pCity = Players[playerID]:GetCityByID(cityID);
        g_YieldTooltipControls.CityViewTooltipText:SetText(
            pCity and pCity:GetYieldRateTooltip(yieldType) or "");
        g_YieldTooltipControls.CityViewTooltipGrid:DoAutoSize();
    end
    instance[field .. "Box"]:SetToolTipCallback(ShowTooltip);
    instance[field]:SetToolTipCallback(ShowTooltip);
end

local function FormatCityYield(value)
    local hundredths = math.floor(value * 100 + 0.5);
    if math.abs(hundredths) < 10000 then
        return string.format("%.1f", math.floor(hundredths / 10) / 10);
    end
    return tostring(math.floor(hundredths / 100));
end

local g_DisplayYieldColumns = {
    Food = true, Production = true, Gold = true, Science = true,
    Culture = true, Faith = true, Tourism = true
};

local g_EconomicColumns = {
    { "Defense", "SortStrength", "Strength" },
    { "Security", "SortSecurity" },
    { "Food", "SortFood" },
    { "Gold", "SortGold" },
    { "Science", "SortResearch" },
    { "Culture", "SortCulture" },
    { "Faith", "SortFaith" },
    { "Tourism", "SortTourism" },
    { "LocalHappiness", "SortLocalHappiness", "Happiness" },
    { "Unhappiness", "SortUnhappiness" },
    { "Production", "SortProduction" },
};

function UpdateDisplay()
    UpdateGPT();
    
    local pPlayer = Players[ Game.GetActivePlayer() ];
    if( pPlayer == nil ) then
        print( "Could not get player... huh?" );
        return;
    end
    
    m_SortTable = {};
	pediaSearchStrings = {};
    local cityRows = {};
    local spiesByCity = {};
    for _, spy in ipairs(pPlayer:GetEspionageSpies() or {}) do
        if (spy.CityX and spy.CityY and spy.CityX >= 0 and spy.CityY >= 0) then
            spiesByCity[spy.CityX .. "," .. spy.CityY] = spy;
        end
    end

    -- The espionage overview exposes city security as Potential in VP.
    local securityByCity = {};
    for _, status in ipairs(pPlayer:GetEspionageCityStatus() or {}) do
        if status.PlayerID == pPlayer:GetID() then
            securityByCity[status.CityID] = status.Potential;
        end
    end

    Controls.MainStack:DestroyAllChildren();
      
    for pCity in pPlayer:Cities() do
		local instance = {};
        ContextPtr:BuildInstanceForControl( "CityInstance", instance, Controls.MainStack );
        
        local sortEntry = {};
		m_SortTable[ tostring( instance.Root ) ] = sortEntry;

        sortEntry.Strength = math.floor(pCity:GetStrengthValue() / 100);
        instance.Defense:SetText(sortEntry.Strength);
        instance.Defense:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_EO_SORT_STRENGTH"));
        local security = securityByCity[pCity:GetID()];
        sortEntry.Security = security or 0;
        instance.Security:SetText(security or "-");
        instance.Security:SetToolTipString(pPlayer:GetCitySecurityTooltip(pCity));

        sortEntry.Happiness = pCity:GetLocalHappiness();
        instance.LocalHappiness:SetText(sortEntry.Happiness);
        instance.LocalHappiness:SetToolTipString(pCity:GetCityHappinessBreakdown());

        sortEntry.Unhappiness = pCity:GetUnhappinessAggregated();
        instance.Unhappiness:SetText(sortEntry.Unhappiness);
        instance.Unhappiness:SetToolTipString(pCity:GetCityUnhappinessBreakdown(false));

        sortEntry.Production = pCity:GetProductionNameKey();
        ProductionDetails( pCity, instance );

		sortEntry.CityName = pCity:GetName();
        TruncateString(instance.CityName, 132, sortEntry.CityName);
        instance.CityName:SetToolTipString(sortEntry.CityName);
        instance.CityButton:SetVoids(pCity:GetX(), pCity:GetY());
        instance.CityButton:RegisterCallback(Mouse.eLClick, OpenCityFromOverview);

		-- Unhappiness breakdown now belongs to its dedicated column.
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
        
        sortEntry.ReligionFollowers = UpdateCityIndicators(pCity, instance, spiesByCity);
        sortEntry.Focus = pCity:GetFocusType();
        instance.CityInfoStack:CalculateSize();
        instance.CityInfoStack:ReprocessAnchoring();

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
			local iFoodPerTurn = pCity:GetYieldRateTimes100(YieldTypes.YIELD_FOOD) / 100;
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
			
			if (pCity:GetYieldRateTimes100(YieldTypes.YIELD_FOOD) == 0) then
				cityGrowth = "-";
				--instance.CityBannerRightBackground:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CITY_STOPPED_GROWING_TT", localizedCityName, cityPopulation));
			elseif pCity:GetYieldRateTimes100(YieldTypes.YIELD_FOOD) < 0 then
				cityGrowth = "[COLOR_WARNING_TEXT]-[ENDCOLOR]";
				--instance.CityBannerRightBackground:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CITY_STARVING_TT",localizedCityName ));
			else
				--instance.CityBannerRightBackground:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CITY_WILL_GROW_TT", localizedCityName, cityPopulation, cityPopulation+1, cityGrowth));
			end
			
			instance.CityGrowth:SetText(cityGrowth);
		end
		
		sortEntry.Food = pCity:GetYieldRateTimes100(YieldTypes.YIELD_FOOD) / 100;
        instance.Food:SetText( FormatCityYield(sortEntry.Food) );
        
		sortEntry.Production =pCity:GetYieldRateTimes100( YieldTypes.YIELD_PRODUCTION ) / 100;
        instance.Production:SetText( FormatCityYield(sortEntry.Production) );
        
        
        if(Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE)) then
			instance.Science:SetText("-");
            instance.ScienceBox:SetHide(false);
        else
			sortEntry.Science = pCity:GetYieldRateTimes100( YieldTypes.YIELD_SCIENCE ) / 100;
			instance.Science:SetText( FormatCityYield(sortEntry.Science) );
			instance.ScienceBox:SetHide(false);
        end
        
        sortEntry.Gold = pCity:GetYieldRateTimes100( YieldTypes.YIELD_GOLD ) / 100;
        instance.Gold:SetText( FormatCityYield(sortEntry.Gold) );
        
        sortEntry.Culture = pCity:GetYieldRateTimes100(YieldTypes.YIELD_CULTURE) / 100;
        instance.Culture:SetText( FormatCityYield(sortEntry.Culture) );
        
        if(Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
			instance.Faith:SetText("-");
            instance.FaithBox:SetHide(false);
        else
			sortEntry.Faith = pCity:GetYieldRateTimes100(YieldTypes.YIELD_FAITH) / 100;
			instance.Faith:SetText( FormatCityYield(sortEntry.Faith) );
			instance.FaithBox:SetHide(false);   
        end
        sortEntry.Tourism = pCity:GetYieldRateTimes100(YieldTypes.YIELD_TOURISM) / 100;
        instance.Tourism:SetText(FormatCityYield(sortEntry.Tourism));

        -- Generate the native yield breakdown on hover, not on every overview refresh.
        local playerID, cityID = pCity:GetOwner(), pCity:GetID();
        SetCityYieldTooltip(instance, "Food", playerID, cityID, YieldTypes.YIELD_FOOD);
        SetCityYieldTooltip(instance, "Production", playerID, cityID, YieldTypes.YIELD_PRODUCTION);
        SetCityYieldTooltip(instance, "Gold", playerID, cityID, YieldTypes.YIELD_GOLD);
        SetCityYieldTooltip(instance, "Science", playerID, cityID, YieldTypes.YIELD_SCIENCE);
        SetCityYieldTooltip(instance, "Culture", playerID, cityID, YieldTypes.YIELD_CULTURE);
        SetCityYieldTooltip(instance, "Tourism", playerID, cityID, YieldTypes.YIELD_TOURISM);
        SetCityYieldTooltip(instance, "Faith", playerID, cityID, YieldTypes.YIELD_FAITH);

        cityRows[#cityRows + 1] = { instance = instance, values = sortEntry };
    end
    
    -- Keep 12 pixels beyond the widest displayed number in each yield column.
    local totalWidth = 60 + 270 + 64 + 36 + 16 + 94;
    for _, column in ipairs(g_EconomicColumns) do
        local field, header = column[1], column[2];
        local maxChars = 1;
        for _, row in ipairs(cityRows) do
            local value = row.values[column[3] or field];
            if (value) then
                local display = g_DisplayYieldColumns[field] and FormatCityYield(value)
                    or tostring(math.floor(value));
                maxChars = math.max(maxChars, #display);
            end
        end
        local width = math.max(32, maxChars * 10 + 12);
        totalWidth = totalWidth + width;
        Controls[header]:SetSizeX(width);
        for _, row in ipairs(cityRows) do
            row.instance[field .. "Box"]:SetSizeX(width);
        end
    end

    for _, row in ipairs(cityRows) do
        row.instance.Root:SetSizeX(totalWidth);
        row.instance.RowDivider:SetSizeX(totalWidth);
        row.instance.CityRowStack:CalculateSize();
        row.instance.CityRowStack:ReprocessAnchoring();
    end
    Controls.ColumnHeaderStack:CalculateSize();
    Controls.ColumnHeaderStack:ReprocessAnchoring();

    Controls.MainStack:CalculateSize();
    Controls.MainStack:ReprocessAnchoring();
    Controls.MainScroll:CalculateInternalSize();
end


-------------------------------------------------
-------------------------------------------------
function UpdateGPT()

    local pPlayer = Players[ Game.GetActivePlayer() ];
    
    local iHandicap = Players[Game.GetActivePlayer()]:GetHandicapType();

    Controls.TotalGoldValue:SetText( FormatDecimalTimes100( pPlayer:GetGoldTimes100() ) );
    
    local netGPT = pPlayer:CalculateGoldRateTimes100() / 100;
    local netText = FormatDecimal(netGPT);
    Controls.NetGoldValue:SetText((netGPT >= 0 and "[COLOR_POSITIVE_TEXT]" or "[COLOR_NEGATIVE_TEXT]") .. netText .. "[ENDCOLOR]");
    
    if( netGPT < 0 ) then
        Controls.ScienceLost:SetHide( false );
        Controls.ScienceLostValue:SetText( FormatDecimalTimes100( pPlayer:GetScienceFromBudgetDeficitTimes100() ) );
    else
        Controls.ScienceLost:SetHide( true );
    end
    
    Controls.GrossGoldValue:SetText( FormatDecimalTimes100( pPlayer:CalculateGrossGoldTimes100() ) );
    
    Controls.TotalExpenseValue:SetText( "[COLOR_NEGATIVE_TEXT]" .. FormatDecimal( pPlayer:CalculateInflatedCosts() ) .. "[ENDCOLOR]" );

	-- Cities
    Controls.CityIncomeValue:SetText( FormatDecimalTimes100( pPlayer:GetGoldFromCitiesTimes100()) );
    
    local bFoundCity = false;
    Controls.CityStack:DestroyAllChildren();
    for pCity in pPlayer:Cities() do
    
        local CityIncome = pCity:GetYieldRateTimes100(YieldTypes.YIELD_GOLD) / 100;
    
        if( CityIncome > 0 ) then
            bFoundCity = true;
    		local instance = {};
            ContextPtr:BuildInstanceForControl( "TradeEntry", instance, Controls.CityStack );
            
            instance.CityName:SetText( pCity:GetName() );
            instance.TradeIncomeValue:SetText( FormatDecimal( CityIncome) );
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
        Controls.DiploIncomeValue:SetText( FormatDecimal( diploGPT) );
    else
        Controls.DiploIncomeValue:SetText( FormatDecimal(0) );
    end
    
    -- Religion
    local religionGPT = pPlayer:GetGoldPerTurnFromReligion();
    if( religionGPT > 0 ) then
        Controls.ReligionIncomeValue:SetText( FormatDecimal( religionGPT ) );
    else
        Controls.ReligionIncomeValue:SetText( FormatDecimal(0) );
    end

    Controls.TradeIncomeValue:SetText( FormatDecimalTimes100( pPlayer:GetCityConnectionGoldTimes100() ) );
    
    -- Trade income breakdown tooltip
    local iBaseGold = GameDefines.TRADE_ROUTE_BASE_GOLD / 100;
	local iGoldPerCapitalPop = GameDefines.TRADE_ROUTE_CAPITAL_POP_GOLD_MULTIPLIER / 100
	local iCapitalPop = 0;
    local iGoldPerPop = GameDefines.TRADE_ROUTE_CITY_POP_GOLD_MULTIPLIER / 100;
    local strTooltip = Locale.ConvertTextKey("TXT_KEY_EO_INCOME_TRADE");
    strTooltip = strTooltip .. "[NEWLINE][NEWLINE]";
	local iTradeRouteGoldModifier = pPlayer:GetCityConnectionTradeRouteGoldModifier();
	if (iTradeRouteGoldModifier ~= 0) then
		if (iTradeRouteGoldModifier > 0) then
			strTooltip = strTooltip .. "+" .. Locale.ConvertTextKey("TXT_KEY_EGI_TRADE_ROUTE_MOD_INFO", iTradeRouteGoldModifier);
		else
			strTooltip = strTooltip .. "-" .. Locale.ConvertTextKey("TXT_KEY_EGI_TRADE_ROUTE_MOD_INFO", iTradeRouteGoldModifier);
		end
		strTooltip = strTooltip .. "[NEWLINE]";
	end
	if pPlayer:GetCapitalCity() ~= nil then
		iCapitalPop = pPlayer:GetCapitalCity():GetPopulation();
	end
    strTooltip = strTooltip .. Locale.ConvertTextKey("TXT_KEY_TRADE_ROUTE_INCOME_INFO", iBaseGold, iGoldPerCapitalPop, iCapitalPop, iGoldPerPop);
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
                instance.TradeIncomeValue:SetText( FormatDecimal( tradeIncome ) );
                
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
	
    Controls.UnitExpenseValue:SetText( FormatDecimal( iTotalUnitMaintenance ) );
    
    local iTotalUnits = pPlayer:GetNumUnits();
    
	print("Total Units - " .. iTotalUnits);
    local iMaintenanceFreeUnits = math.min(pPlayer:GetNumMaintenanceFreeUnits(DomainTypes.NO_DOMAIN, false), iTotalUnits);
    
    
	print("Maint Free Units - " .. iMaintenanceFreeUnits);
    
    local iPaidUnits = iTotalUnits - iMaintenanceFreeUnits;
    
    print("Paid Units - " .. iPaidUnits);
    
    local fCostPer = FormatDecimal( iTotalUnitMaintenance / iPaidUnits );
    
    local strUnitTT = Locale.ConvertTextKey("TXT_KEY_EO_EX_UNITS", fCostPer, iPaidUnits);
    
    -- Maintenance free units
    if (iMaintenanceFreeUnits ~= 0) then
		strUnitTT = strUnitTT .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_EO_EX_UNITS_NO_MAINT", iMaintenanceFreeUnits);
    end
    
    -- Maintenance mod (handicap)
    local iUnitMaintMod = DB_HandicapInfos[iHandicap].UnitCostPercent;
    if (iUnitMaintMod ~= 100) then
		strUnitTT = strUnitTT .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_HANDICAP_MAINTENANCE_MOD", iUnitMaintMod);
	end
    
    Controls.UnitExpense:SetToolTipString( strUnitTT );
    
    -- Buildings
    local iBuildingMaintMod = DB_HandicapInfos[iHandicap].BuildingCostPercent;
    
    local strBuildingsTT = Locale.ConvertTextKey("TXT_KEY_EO_EX_BUILDINGS");
    local strBuildingsModTT = "";
    
    if (iBuildingMaintMod ~= 100) then
		strBuildingsModTT = Locale.ConvertTextKey("TXT_KEY_HANDICAP_MAINTENANCE_MOD", iBuildingMaintMod);
		strBuildingsTT = strBuildingsTT .. "[NEWLINE][NEWLINE]" .. strBuildingsModTT;
		strBuildingsModTT = Locale.ConvertTextKey("TXT_KEY_EO_EX_BASE_BUILDINGS") .. "[NEWLINE][NEWLINE]" .. strBuildingsModTT;
	end
    
    Controls.BuildingsToggle:SetToolTipString( strBuildingsTT );
    
    Controls.BuildingExpenseValue:SetText( FormatDecimal( pPlayer:GetBuildingGoldMaintenance() ) );
   
    bFoundCity = false;
    Controls.BuildingsStack:DestroyAllChildren();
    for pCity in pPlayer:Cities() do
		
        local BuildingCost = pCity:GetTotalBaseBuildingMaintenance();
		
        if( BuildingCost > 0 ) then
            bFoundCity = true;
    		local instance = {};
            ContextPtr:BuildInstanceForControl( "TradeEntry", instance, Controls.BuildingsStack );
            
            instance.CityName:SetText( pCity:GetName() );
            instance.TradeIncomeValue:SetText( FormatDecimal( BuildingCost ) );
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
    
    local iRouteMaintMod = DB_HandicapInfos[iHandicap].ImprovementCostPercent;
    local strRoutesModTT = "";
    
    if (iRouteMaintMod ~= 100) then
		strRoutesModTT = Locale.ConvertTextKey("TXT_KEY_HANDICAP_MAINTENANCE_MOD", iRouteMaintMod);
		strRoutesTT = strRoutesTT .. "[NEWLINE][NEWLINE]" .. strRoutesModTT;
	end
    
    Controls.TileExpense:SetToolTipString(strRoutesTT);
    Controls.TileExpenseValue:SetText( FormatDecimal( pPlayer:GetImprovementGoldMaintenance() ) );
    
    -- Diplo
    local diploGPT = pPlayer:GetGoldPerTurnFromDiplomacy();
    if( diploGPT < 0 ) then
        Controls.DiploExpenseValue:SetText( FormatDecimal( -diploGPT) );
    else
        Controls.DiploExpenseValue:SetText( FormatDecimal(0) );
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
		elseif( m_SortMode == eUnhappiness ) then
			valueA = entryA.Unhappiness;
			valueB = entryB.Unhappiness;
		elseif( m_SortMode == eLocalHappiness ) then
			valueA = entryA.Happiness;
			valueB = entryB.Happiness;
		elseif( m_SortMode == eStrength ) then
			valueA = entryA.Strength;
			valueB = entryB.Strength;
		elseif( m_SortMode == eSecurity ) then
			valueA = entryA.Security;
			valueB = entryB.Security;
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
		elseif( m_SortMode == eTourism ) then
			valueA = entryA.Tourism;
			valueB = entryB.Tourism;
		elseif( m_SortMode == eReligion ) then
			valueA = entryA.ReligionFollowers;
			valueB = entryB.ReligionFollowers;
		elseif( m_SortMode == eFocus ) then
			valueA = entryA.Focus;
			valueB = entryB.Focus;
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
Controls.SortReligion:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortFocus:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortStrength:RegisterCallback(Mouse.eLClick, OnSort);
Controls.SortSecurity:RegisterCallback(Mouse.eLClick, OnSort);
Controls.SortUnhappiness:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortLocalHappiness:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortTourism:RegisterCallback(Mouse.eLClick, OnSort);
Controls.SortProduction:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortFood:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortGold:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortCulture:RegisterCallback( Mouse.eLClick, OnSort );

Controls.SortPopulation:SetVoid1( ePopulation );
Controls.SortCityName:SetVoid1( eName );
Controls.SortReligion:SetVoid1( eReligion );
Controls.SortFocus:SetVoid1( eFocus );
Controls.SortStrength:SetVoid1(eStrength);
Controls.SortSecurity:SetVoid1(eSecurity);
Controls.SortUnhappiness:SetVoid1( eUnhappiness );
Controls.SortLocalHappiness:SetVoid1( eLocalHappiness );
Controls.SortTourism:SetVoid1(eTourism);
Controls.SortProduction:SetVoid1( eProduction );
Controls.SortFood:SetVoid1( eFood );
Controls.SortGold:SetVoid1( eGold );
Controls.SortCulture:SetVoid1( eCulture );

if(Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE)) then
	Controls.SortResearch:SetDisabled(true);
	Controls.SortResearch:SetHide(false);
	Controls.ScienceLost:SetHide(true);
else
	Controls.SortResearch:RegisterCallback(Mouse.eLClick, OnSort);
	Controls.SortResearch:SetVoid1(eResearch);
	Controls.SortResearch:SetHide(false);
	Controls.ScienceLost:SetHide(false);
end

if(Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
	Controls.SortFaith:SetDisabled(true);
	Controls.SortFaith:SetHide(false);
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
		
		local iCurrentProduction = city:GetProductionTimes100() / 100;
		local iProductionNeeded = city:GetProductionNeeded();
		local iProductionPerTurn = city:GetYieldRateTimes100(YieldTypes.YIELD_PRODUCTION) / 100;
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
			if (city:GetYieldRateTimes100(YieldTypes.YIELD_PRODUCTION) > 0) then
				buildGrowth = city:GetProductionTurnsLeft();
			end
		end
		
		instance.BuildGrowth:SetText(buildGrowth);
        local turnWidth = math.max(8, #tostring(buildGrowth) * 8);
        instance.BuildGrowth:SetSizeX(turnWidth);
        -- The portrait artwork has inner padding; shift its control 9px left.
        instance.ProdImage:SetOffsetX(8 + turnWidth - 3);

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
	instance.ProdButton:SetVoids( city:GetX(), city:GetY() );

end


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )
    if( not bIsHide ) then
    	UpdateDisplay();
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );