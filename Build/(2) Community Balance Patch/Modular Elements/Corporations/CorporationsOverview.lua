-------------------------------------------------
-- Corporations Overview
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
include("InfoTooltipInclude");

local g_AvailableCorporationsIM = InstanceManager:new( "AvailableCorporationInstance", "AvailableCorporationBox", Controls.AvailableCorporationsStack );
local g_EstablishedCorporationsIM = InstanceManager:new( "EstablishedCorporationInstance", "EstablishedCorporationBox", Controls.EstablishedCorporationsStack ); 

g_Tabs = {
	["Corporations"] = {
		Panel = Controls.CorporationsPanel,
		SelectHighlight = Controls.CorporationsSelectHighlight,
	},
	
	["Monopolies"] = {
		Panel = Controls.MonopoliesPanel,
		SelectHighlight = Controls.MonopoliesSelectHighlight,
	},
}

-- Corporation HQs
g_Headquarters = {
	[0] = {
		headquartersClass = "BUILDINGCLASS_TRADER_SIDS_HQ",
		officeClass = "BUILDINGCLASS_TRADER_SIDS",
		franchiseClass = "BUILDINGCLASS_TRADER_SIDS_FRANCHISE"
	},
	[1] = {
		headquartersClass = "BUILDINGCLASS_LANDSEA_EXTRACTORS_HQ",
		officeClass = "BUILDINGCLASS_LANDSEA_EXTRACTORS",
		franchiseClass = "BUILDINGCLASS_LANDSEA_EXTRACTORS_FRANCHISE"
	},
	[2] = {
		headquartersClass = "BUILDINGCLASS_HEXXON_REFINERY_HQ",
		officeClass = "BUILDINGCLASS_HEXXON_REFINERY",
		franchiseClass = "BUILDINGCLASS_HEXXON_REFINERY_FRANCHISE"
	},
	[3] = {
		headquartersClass = "BUILDINGCLASS_GIORIO_ARMEIER_HQ",
		officeClass = "BUILDINGCLASS_GIORIO_ARMEIER",
		franchiseClass = "BUILDINGCLASS_GIORIO_ARMEIER_FRANCHISE"
	},
	[4] = {
		headquartersClass = "BUILDINGCLASS_FIRAXITE_MATERIALS_HQ",
		officeClass = "BUILDINGCLASS_FIRAXITE_MATERIALS",
		franchiseClass = "BUILDINGCLASS_FIRAXITE_MATERIALS_FRANCHISE"
	},
	[5] = {
		headquartersClass = "BUILDINGCLASS_TWOKAY_FOODS_HQ",
		officeClass = "BUILDINGCLASS_TWOKAY_FOODS",
		franchiseClass = "BUILDINGCLASS_TWOKAY_FOODS_FRANCHISE"
	},
	[6] = {
		headquartersClass = "BUILDINGCLASS_CIVILIZED_JEWELERS_HQ",
		officeClass = "BUILDINGCLASS_CIVILIZED_JEWELERS",
		franchiseClass = "BUILDINGCLASS_CIVILIZED_JEWELERS_FRANCHISE"
	}
}

-------------------------------------------------
-- Global Variables
-------------------------------------------------
g_CurrentTab = nil;		-- The currently selected Tab.
g_iSelectedPlayerID = Game.GetActivePlayer();
g_pPlayer = Players[g_iSelectedPlayerID];
g_iTeam = g_pPlayer:GetTeam();
g_pTeam = Teams[g_iTeam];

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function SetBackground()
	-- background based on ideology
	local iIdeology = g_pPlayer:GetLateGamePolicyTree();
	local strBackgroundImage = "loading_9.dds";
	Controls.VassalBackgroundImage:SetTextureAndResize(strBackgroundImage);

	-- Can't find a method to fit texture to size, so this a hack way of doing it
	Controls.BackgroundImage:SetSizeX(926);
	Controls.BackgroundImage:SetSizeY(459);
end

function DisplayCorporations()
	local bHasCorporation = (g_pPlayer:GetCorpID() > 0);

--	if( not bHasCorporation ) then
		UpdateAvailableCorporations();
--	else
--		UpdateOurCorporation();
--	end
end

function DisplayMonopolies()
	print("DisplayMonopolies()");
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

function UpdateAvailableCorporations()
	local BuildingClass;
	local building;

	local OfficeClass;
	local office;

	local FranchiseClass;
	local franchise;

	local corpID;

	-- Clear buttons
	 g_AvailableCorporationsIM:ResetInstances();
	 g_EstablishedCorporationsIM:ResetInstances();

	for i,v in pairs(g_Headquarters) do

		BuildingClass = GameInfo.BuildingClasses[v.headquartersClass];
		building = GameInfo.Buildings[BuildingClass.DefaultBuilding];

		OfficeClass = GameInfo.BuildingClasses[v.officeClass];
		office = GameInfo.Buildings[OfficeClass.DefaultBuilding];

		FranchiseClass = GameInfo.BuildingClasses[v.franchiseClass];
		franchise = GameInfo.Buildings[FranchiseClass.DefaultBuilding];

		corpID = building.CorporationHQID;
		print( i .. ": " .. corpID );

		local iFounderID = -1;
		local founderTooltip = "";
		local pFoundingCity = "";
		local pFoundingPlayer = nil;

		-- Get the founder 
		for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
			local pFounder = Players[iPlayerLoop];
			if pFounder:IsEverAlive() then
				local playerCorpID = pFounder:GetCorpID();
				if(playerCorpID == corpID) then
					-- We found the founder of this corporation
					iFounderID = iPlayerLoop;

					local civType = pFounder:GetCivilizationType();
					local civInfo = GameInfo.Civilizations[civType];
					
					if (g_pTeam:IsHasMet(Players[iFounderID]:GetTeam()) ) then
						founderTooltip = Locale.ConvertTextKey( civInfo.ShortDescription );
					else
						founderTooltip = Locale.ConvertTextKey( "TXT_KEY_UNMET_PLAYER" );
					end

					-- Look through their cities and get the headquarters
					for city in pFounder:Cities() do
						if(city:IsHasBuilding(building.ID)) then
							pFoundingCity = city;
							pFoundingPlayer = Players[iFounderID];
							break;
						end
					end

					break;
				end
			end
		end

		local controlTable;
		local bAvailable = (iFounderID == -1);

		-- If this corporation is available
		if( bAvailable ) then
			controlTable = g_AvailableCorporationsIM:GetInstance();

		-- If this corporation is already established by another civilization
		else 
			controlTable = g_EstablishedCorporationsIM:GetInstance();
			if(g_pTeam:IsHasMet(Players[iFounderID]:GetTeam())) then
				CivIconHookup( iFounderID, 32, controlTable.FounderIcon, controlTable.FounderIconBG, controlTable.FounderIconShadow, false, true);
			else
				CivIconHookup( -1, 32, controlTable.FounderIcon, controlTable.FounderIconBG, controlTable.FounderIconShadow, false, true);
			end
			controlTable.FounderBox:LocalizeAndSetToolTip(founderTooltip);
		end

		-- Corporation Icon Hookup
		IconHookup( building.PortraitIndex, 80, building.IconAtlas, controlTable.CorporationPortrait );
		controlTable.CorporationName:LocalizeAndSetText(building.Description);

		-- Construct tooltip
		local strTooltip = "";
		strTooltip = strTooltip .. Locale.ConvertTextKey( "TXT_KEY_CPO_OFFICE_BENEFIT") .. "[NEWLINE]";
		strTooltip = strTooltip .. Locale.ConvertTextKey( "-------------------") .. "[NEWLINE]";
		strTooltip = strTooltip .. Locale.ConvertTextKey( office.Help ) .. "[NEWLINE][NEWLINE]" ;
		strTooltip = strTooltip .. Locale.ConvertTextKey( "TXT_KEY_CPO_FRANCHISE_BENEFIT" ) .. "[NEWLINE]";
		strTooltip = strTooltip .. Locale.ConvertTextKey( "-------------------" .. "[NEWLINE]");
		strTooltip = strTooltip .. Locale.ConvertTextKey( franchise.Help);

		controlTable.NameBox:SetToolTipString( strTooltip );

		if( bAvailable ) then
			-- Get the resource monopoly list for this Corporation
			local resourceString = "";
			for row in GameInfo.Building_ResourceMonopolyOrs( "BuildingType = '" .. building.Type .. "'" ) do
				local requiredResource = GameInfo.Resources[row.ResourceType];
				if requiredResource then
					resourceString = resourceString .. requiredResource.IconString .. " ";
				end		
			end
			controlTable.ResourcesRequired:SetText(resourceString);

			-- Progress Bar Hookup
			IconHookup( building.PortraitIndex, 64, building.IconAtlas, controlTable.ProgressImage );
		  
			if (building.Help ~= nil) then
				controlTable.ProgressBox:LocalizeAndSetToolTip( GetHelpTextForBuilding(building.ID, false, false, false, nil) );
			end

			-- Reset controls if we are not building it
			controlTable.ProgressTurns:SetText( "" );
			controlTable.ProgressMeter:SetPercent( 0 );

			-- iterate through all of our cities and see if we're building it
			for city in g_pPlayer:Cities() do
				if city:GetProductionBuilding() == building.ID then
					--------------------------------------------------------
					-- Building Production Progress
					local iProductionNeeded = pPlayer:GetBuildingProductionNeeded(building.ID);
					local iProductionGained = city:GetProductionTimes100() / 100;
					controlTable.ProgressMeter:SetPercent( iProductionGained / iProductionNeeded );
					
					local bGeneratingProduction = city:GetCurrentProductionDifferenceTimes100(false, false) > 0;
				
					if (bGeneratingProduction) then
						controlTable.ProgressTurns:SetText( Locale.ConvertTextKey("TXT_KEY_PRODUCTION_HELP_NUM_TURNS", city:GetBuildingProductionTurnsLeft(building.ID)) );
					else
						controlTable.ProgressTurns:SetText( Locale.ConvertTextKey("TXT_KEY_PRODUCTION_HELP_INFINITE_TURNS") );
					end
				end
			end
		else
			controlTable.FounderCity:SetText( pFoundingCity:GetName() );
			controlTable.OfficesLabel:SetText( pFoundingPlayer:GetNumberofOffices() );
			controlTable.FranchisesLabel:SetText( pFoundingPlayer:GetNumberofGlobalFranchises() );
		end
	end

	RecalcPanelSize();
end

function UpdateOurCorporation()

end

-----------------------------------------------------------------
-----------------------------------------------------------------
function OnAvailableCorporationsButton()
    if( Controls.AvailableCorporationsStack:IsHidden() ) then
        Controls.AvailableCorporationsButton:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "{TXT_KEY_CPO_AVAILABLE_CORPORATIONS:upper}" ) );
        Controls.AvailableCorporationsStack:SetHide( false );
    else
        Controls.AvailableCorporationsButton:SetText( "[ICON_PLUS]" .. Locale.ConvertTextKey( "{TXT_KEY_CPO_AVAILABLE_CORPORATIONS:upper}" ) );
        Controls.AvailableCorporationsStack:SetHide( true );
    end
    
    RecalcPanelSize();
end
Controls.AvailableCorporationsButton:RegisterCallback( Mouse.eLClick, OnAvailableCorporationsButton );

function OnEstablishedCorporationsButton()
    if( Controls.EstablishedCorporationsStack:IsHidden() ) then
        Controls.EstablishedCorporationsButton:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "{TXT_KEY_CPO_ESTABLISHED_CORPORATIONS:upper}" ) );
        Controls.EstablishedCorporationsStack:SetHide( false );
    else
        Controls.EstablishedCorporationsButton:SetText( "[ICON_PLUS]" .. Locale.ConvertTextKey( "{TXT_KEY_CPO_ESTABLISHED_CORPORATIONS:upper}" ) );
        Controls.EstablishedCorporationsStack:SetHide( true );
    end
    
    RecalcPanelSize();
end
Controls.EstablishedCorporationsButton:RegisterCallback( Mouse.eLClick, OnEstablishedCorporationsButton );

function RecalcPanelSize()
	Controls.OuterCorporationsStack:CalculateSize();
	Controls.AvailableCorporationsStack:CalculateSize();
	Controls.EstablishedCorporationsStack:CalculateSize();

	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
g_Tabs["Corporations"].RefreshContent = function()
	DisplayCorporations();
end

g_Tabs["Monopolies"].RefreshContent = function()
	DisplayMonopolies();
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function TabSelect(tab)

	for i,v in pairs(g_Tabs) do
		local bHide = i ~= tab;
		v.Panel:SetHide(bHide);
		v.SelectHighlight:SetHide(bHide);
	end
	g_CurrentTab = tab;
	g_Tabs[tab].RefreshContent();	
end
Controls.TabButtonCorporations:RegisterCallback( Mouse.eLClick, function() TabSelect("Corporations"); end);
Controls.TabButtonMonopolies:RegisterCallback( Mouse.eLClick, function() TabSelect("Monopolies"); end);

-------------------------------------------------
-- On Popup
-------------------------------------------------
function OnPopup( popupInfo )
	if( popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_MODDER_10 ) then
    	m_PopupInfo = popupInfo;
		if( m_PopupInfo.Data1 == 1 ) then
        	if( ContextPtr:IsHidden() == false ) then
        		OnClose();
			else
            	UIManager:QueuePopup( ContextPtr, PopupPriority.InGameUtmost );
        	end
    	else
        	UIManager:QueuePopup( ContextPtr, PopupPriority.LoadGameScreen );
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

-----------------------------------------------------------------
-- Add Corporations Overview to Dropdown (if enabled)
-----------------------------------------------------------------
LuaEvents.AdditionalInformationDropdownGatherEntries.Add( function(entries)
	table.insert(entries, {
		text = Locale.Lookup("TXT_KEY_CPO"),
		call = function()
			Events.SerialEventGameMessagePopup {
				Type = ButtonPopupTypes.BUTTONPOPUP_MODDER_10,
			};
		end,
	});
end);

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )

	-- Set player icon at top of screen
	CivIconHookup( Game.GetActivePlayer(), 64, Controls.CivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true );
   
    if( not bInitState ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
			TabSelect(g_CurrentTab);
        else
            UI.decTurnTimerSemaphore();
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_MODDER_10, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

OnAvailableCorporationsButton();
OnEstablishedCorporationsButton();

-- Just in case :)
LuaEvents.RequestRefreshAdditionalInformationDropdownEntries();
TabSelect("Corporations");
SetBackground();
ContextPtr:SetHide(true);