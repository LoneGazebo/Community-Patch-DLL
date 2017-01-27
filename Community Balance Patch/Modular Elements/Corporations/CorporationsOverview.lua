
print("This is the Corporations Overview from CBP")
-------------------------------------------------
-- Corporations Overview
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
include("InfoTooltipInclude");

local g_AvailableCorporationsIM = InstanceManager:new( "AvailableCorporationInstance", "AvailableCorporationBox", Controls.AvailableCorporationsStack );

local g_CorporationMonopolyResourceIMList = {};
			
local g_CorporationBenefitIM = InstanceManager:new( "CorporationBenefitInstance", "Base", Controls.CorporationBenefitStack );
local g_YourOfficesIM = InstanceManager:new( "OfficeCityInstance", "Base", Controls.YourOfficesStack );
local g_YourFranchisesIM = InstanceManager:new( "FranchiseCityInstance", "Base", Controls.YourFranchisesStack );

local g_WorldCorporationsIM = InstanceManager:new( "WorldCorporationInstance", "Base", Controls.WorldCorporationsStack );
local g_WorldFranchisesIM = InstanceManager:new( "WorldFranchiseInstance", "Base", Controls.WorldFranchisesStack );
local g_WorldMonopolyResourcesIM = InstanceManager:new( "WorldMonopolyResourceInstance", "Base", Controls.MonopolyResourcesStack );

local pediaSearchStrings = {};

g_Tabs = {
	["Monopolies"] = {
		Panel = Controls.MonopoliesPanel,
		SelectHighlight = Controls.MonopoliesSelectHighlight,
	},

	["Corporations"] = {
		Panel = Controls.CorporationsPanel,
		SelectHighlight = Controls.CorporationsSelectHighlight,
	},

	["WorldCorporations"] = {
		Panel = Controls.WorldCorporationsPanel,
		SelectHighlight = Controls.WorldCorporationsSelectHighlight,
	},
}

g_YourOfficesSortOptions = {
	{
		Button = Controls.OOSortByCivilization,
		ImageControl = Controls.OOSortByCivilizationImage,
		Column = "Civilization",
		DefaultDirection = "asc",
		CurrentDirection = nil,
	},
	{
		Button = Controls.OOSortByName,
		ImageControl = Controls.OOSortByNameImage,
		Column = "Name",
		DefaultDirection = "asc",
		CurrentDirection = "asc",
	},
	{
		Button = Controls.OOSortByTR,
		ImageControl = Controls.OOSortByTRImage,
		Column = "TradeRoutes",
		DefaultDirection = "desc",
		SortType = "numeric",
		CurrentDirection = nil,
	},
}

g_YourFranchisesSortOptions = {
	{
		Button = Controls.OFSortByCivilization,
		ImageControl = Controls.OFSortByCivilizationImage,
		Column = "Civilization",
		DefaultDirection = "asc",
		CurrentDirection = nil,
	},
	{
		Button = Controls.OFSortByName,
		ImageControl = Controls.OFSortByNameImage,
		Column = "Name",
		DefaultDirection = "asc",
		CurrentDirection = "asc",
	},
}

g_WorldCorporationsSortOptions = {
	{
		Button = Controls.WCSortByCorpName,
		ImageControl = Controls.WCSortByCorpNameImage,
		Column = "CorpName",
		DefaultDirection = "asc",
		CurrentDirection = "asc",
	},
	{
		Button = Controls.WCSortByCivilization,
		ImageControl = Controls.WCSortByCivilizationImage,
		Column = "CivName",
		DefaultDirection = "asc",
		CurrentDirection = nil,
	},
	{
		Button = Controls.WCSortByHQ,
		ImageControl = Controls.WCSortByHQImage,
		Column = "CityName",
		DefaultDirection = "asc",
		CurrentDirection = nil,
	},
	{
		Button = Controls.WCSortByDateFounded,
		ImageControl = Controls.WCSortByDateFoundedImage,
		Column = "DateFounded",
		DefaultDirection = "desc",
		SortType = "numeric",
		CurrentDirection = nil,
	},
	{
		Button = Controls.WCSortByNumOffices,
		ImageControl = Controls.WCSortByNumOfficesImage,
		Column = "NumOffices",
		DefaultDirection = "desc",
		SortType = "numeric",
		CurrentDirection = nil,
	},
	{
		Button = Controls.WCSortByNumFranchises,
		ImageControl = Controls.WCSortByNumFranchisesImage,
		Column = "NumFranchises",
		DefaultDirection = "desc",
		SortType = "numeric",
		CurrentDirection = nil,
	},
}

g_WorldFranchisesSortOptions = {
	{
		Button = Controls.WFSortByCivilization,
		ImageControl = Controls.WFSortByCivilizationImage,
		Column = "FromCivName",
		DefaultDirection = "asc",
		CurrentDirection = "asc",
	},
	{
		Button = Controls.WFSortByCorp,
		ImageControl = Controls.WFSortByCorpImage,
		Column = "CorpName",
		DefaultDirection = "asc",
		CurrentDirection = nil,
	},
	{
		Button = Controls.WFSortByCivTo,
		ImageControl = Controls.WFSortByCivToImage,
		Column = "ToCivName",
		DefaultDirection = "asc",
		CurrentDirection = nil,
	},
	{
		Button = Controls.WFSortByCity,
		ImageControl = Controls.WFSortByCityImage,
		Column = "CityName",
		DefaultDirection = "asc",
		CurrentDirection = nil,
	},
}

g_MonopolyResourcesSortOptions = {
	{
		Button = Controls.MPSortByResource,
		ImageControl = Controls.MPSortByResourceImage,
		Column = "ResourceName",
		DefaultDirection = "asc",
		CurrentDirection = nil,
	},
	{
		Button = Controls.MPSortByCivilization,
		ImageControl = Controls.MPSortByCivilizationImage,
		Column = "CivName",
		DefaultDirection = "asc",
		CurrentDirection = nil,
	},
	{
		Button = Controls.MPSortByPercent,
		ImageControl = Controls.MPSortByPercentImage,
		Column = "MonopolyPercent",
		DefaultDirection = "desc",
		SortType = "numeric",
		CurrentDirection = "desc",
	},
}
-------------------------------------------------
-- Global Variables
-------------------------------------------------
g_CurrentTab = nil;		-- The currently selected Tab.
g_iSelectedPlayerID = Game.GetActivePlayer();
g_pPlayer = Players[g_iSelectedPlayerID];
g_iTeam = g_pPlayer:GetTeam();
g_pTeam = Teams[g_iTeam];

-- When we have a Corporation, this will change
g_Office = nil;
g_Franchise = nil;

g_MonopolyResourcePlayer = -1;

g_YourOfficesSortFunction = nil;
g_YourFranchisesSortFunction = nil;
g_WorldCorporationsSortFunction = nil;
g_WorldFranchisesSortFunction = nil;
g_MonopolyResourcesSortFunction = nil;
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

function GetPedia( void1, void2, button )
	local searchString = pediaSearchStrings[tostring(button)];
	if (searchString ~= nil) then
		Events.SearchForPediaEntry( searchString );
	end
end

function SetBackground()
	local strBackgroundImage = "loading_9.dds";
	Controls.BackgroundImage:SetTextureAndResize(strBackgroundImage);

	-- Can't find a method to fit texture to size, so this a hack way of doing it
	Controls.BackgroundImage:SetSizeX(956);
	Controls.BackgroundImage:SetSizeY(459);
end

function DisplayCorporations()
	local bHasCorpTech = g_pTeam:IsCorporationsEnabled();

	Controls.YourCorporationsNotReady:SetHide( bHasCorpTech );
	Controls.AvailableCorporationBox:SetHide( not bHasCorpTech );

	if(bHasCorpTech) then
		local bHasCorporation = (g_pPlayer:GetCorporation() ~= -1);

		Controls.AvailableCorporationBox:SetHide( bHasCorporation );
		Controls.YourCorporationBox:SetHide( not bHasCorporation );
	
		if( not bHasCorporation ) then
			UpdateAvailableCorporations();
		else
			UpdateYourCorporation();
		end
	end
end

function DisplayWorldCorporations()
	RefreshWorldCorporations();
	RefreshWorldFranchises();
end

function RefreshWorldCorporations()
	--print("RefreshWorldCorporations()");

	g_WorldCorporationsIM:ResetInstances();

	local instances = {};
	for iPlayerLoop=0, GameDefines.MAX_MAJOR_CIVS, 1 do
		local pLoopPlayer = Players[iPlayerLoop];
		if(pLoopPlayer:IsAlive()) then
			local ePlayerCorporation = pLoopPlayer:GetCorporation();
			if(ePlayerCorporation ~= -1) then
				--print("ePlayerCorporation=" .. ePlayerCorporation);

				local pCorporation = GameInfo.Corporations[ePlayerCorporation];
				if(pCorporation ~= nil) then
					local info = {
						CivPlayer = iPlayerLoop,	-- int
						CivName = Locale.ConvertTextKey(GetPlayerCiv(iPlayerLoop).Description),
						CorpName = Locale.ConvertTextKey(pCorporation.Description),
						CorpTooltip = Locale.ConvertTextKey(pCorporation.Help),
						CityName = Game.GetCorporationHeadquarters(ePlayerCorporation),	-- city
						DateFounded = pLoopPlayer:GetCorporationFoundedTurn(),
						NumOffices = pLoopPlayer:GetNumberofOffices(),
						NumFranchises = pLoopPlayer:GetNumberofGlobalFranchises(),
					};
					table.insert(instances, info);
				else
					print("WARNING: pCorporation is nil! RefreshWorldCorporations()!");
				end
			end
		end
	end

	table.sort(instances, g_WorldCorporationsSortFunction);

	for i,v in pairs(instances) do
		InsertCorporation(v);
	end

	Controls.WorldCorporationsNone:SetHide( Game.GetNumCorporationsFounded() > 0 );

	Controls.WorldCorporationsStack:CalculateSize();
	Controls.WorldCorporationsStack:ReprocessAnchoring();
	Controls.WorldCorporationsScrollPanel:CalculateInternalSize();
end

function InsertCorporation(info)
	local instance = g_WorldCorporationsIM:GetInstance();
	local pPlayer = Players[info.CivPlayer];

	instance.Corporation:LocalizeAndSetText(info.CorpName);
	instance.Corporation:LocalizeAndSetToolTip(info.CorpTooltip);
	instance.DateFounded:SetText(Game.GetDateString(info.DateFounded));
	instance.Headquarters:SetText(info.CityName:GetName());
	instance.NumOffices:SetText(info.NumOffices);
	instance.NumFranchises:SetText(info.NumFranchises);

	HookupCivControl(info.CivPlayer, 32, instance.CivName, instance.CivIcon, instance.CivIconBG, instance.CivIconShadow);
end

function RefreshWorldFranchises()
	--print("RefreshWorldFranchises()");

	g_WorldFranchisesIM:ResetInstances();

	local franchises = {};
	local iNumFranchises = 0;

	-- Look at all player's cities (too many for-loops, this is ridiculously expensive!)
	for iPlayerLoop=0, GameDefines.MAX_CIV_PLAYERS-1, 1 do
		local pLoopPlayer = Players[iPlayerLoop];
		if( pLoopPlayer:IsEverAlive() and pLoopPlayer:IsAlive() ) then
			for city in pLoopPlayer:Cities() do
				for eFromPlayer=0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
					-- City has this franchise?
					if( city:IsFranchised(eFromPlayer) ) then
						local pFromPlayer = Players[eFromPlayer];
						local pCorporation = nil;

						local szCorporationString = "Undefined";

						local eFromCorporation = pFromPlayer:GetCorporation();
						if(eFromCorporation ~= -1) then
							pCorporation = GameInfo.Corporations[eFromCorporation];
							szCorporationString = Locale.ConvertTextKey(pCorporation.Description);
						end

						local info = {
							FromCiv = eFromPlayer,
							FromCivName = Locale.ConvertTextKey(GetPlayerCiv( eFromPlayer ).Description),
							CorpName = szCorporationString,
							ToCiv = iPlayerLoop,
							ToCivName = Locale.ConvertTextKey(GetPlayerCiv( iPlayerLoop ).Description),
							CityName = city:GetName(),
						};
						table.insert(franchises, info);
						iNumFranchises = iNumFranchises + 1;
					end
				end
			end
		end
	end

	table.sort(franchises, g_WorldFranchisesSortFunction);

	for i,v in ipairs(franchises) do
		InsertFranchise(v);
	end

	Controls.WorldFranchisesNone:SetHide( iNumFranchises ~= 0 );

	Controls.WorldFranchisesStack:CalculateSize();
	Controls.WorldFranchisesStack:ReprocessAnchoring();
	Controls.WorldFranchisesScrollPanel:CalculateInternalSize();
end

function InsertFranchise(info)
	local instance = g_WorldFranchisesIM:GetInstance();
	local eFromPlayer = -1;

	instance.CityName:SetText( info.CityName );
	instance.CorporationName:LocalizeAndSetText( info.CorpName );

	HookupCivControl( info.FromCiv, 32, instance.FromCivName, instance.FromCivIcon, instance.FromCivIconBG, instance.FromCivIconShadow);
	HookupCivControl( info.ToCiv, 32, instance.ToCivName, instance.ToCivIcon, instance.ToCivIconBG, instance.ToCivIconShadow);
end

function DisplayMonopolies()
	RefreshMonopolyPulldowns();
	RefreshMonopolies();
end

function RefreshMonopolies()
	--print("RefreshMonopolies()");

	g_WorldMonopolyResourcesIM:ResetInstances();
	
	local resources = {};
	for pResource in GameInfo.Resources() do
		local eResource = pResource.ID;
		if(pResource.IsMonopoly == 1) then
			local ePlayer = ChooseMonopolyPlayer(eResource);

			local iMonopolyPercent = 0;
			local szResourceName = Locale.ConvertTextKey( pResource.Description );
			local szCivName = 0;
			local bValid = IsResourceValid(eResource, ePlayer);

			if( ePlayer ~= -1 ) then
				iMonopolyPercent = Players[ePlayer]:GetMonopolyPercent(eResource);
				szCivName = Locale.ConvertTextKey(GetPlayerCiv(ePlayer).Description);
			end

			if(bValid) then
				-- Build control for hookup
				local info = {
					ResourceIcon = pResource.IconString,
					ResourceName = szResourceName,
					ResourceID = eResource,
					Resource = pResource,
					CivName = szCivName,
					PlayerID = ePlayer,
					MonopolyPercent = iMonopolyPercent,
				};
				table.insert(resources, info);
			end
		end
	end

	table.sort(resources, g_MonopolyResourcesSortFunction);

	for i,v in pairs(resources) do
		HookupResourceInstance(v);
	end

	Controls.MonopolyResourcesStack:CalculateSize();
	Controls.MonopolyResourcesStack:ReprocessAnchoring();
	Controls.MonopolyResourcesScrollPanel:CalculateInternalSize();
end

function IsResourceValid(eResource, ePlayer)

	local iTotal = Map.GetNumResources(eResource);

	if(iTotal <= 0) then
		return false;
	end

	if(g_MonopolyResourceType == "All") then	
		--print( "Looking at all resources" );
		if(ePlayer ~= -1) then
			local iNumResources = Players[ePlayer]:GetNumResourceTotal(eResource, false) + Players[ePlayer]:GetResourceExport(eResource);
			return iTotal > 0 or iNumResources > 0;
		else
		-- If player does not exist, then check if it's on the map only
			return iTotal > 0;
		end
	elseif(g_MonopolyResourceType == "Global") then
		--print( "Looking at global resources" );
		if(ePlayer ~= -1) then
			return Players[ePlayer]:HasGlobalMonopoly(eResource);
		else
			return false;
		end
	elseif(g_MonopolyResourceType == "Strategic") then
		--print( "Looking at strategic resources" );
		if(ePlayer ~= -1) then
			return Players[ePlayer]:HasStrategicMonopoly(eResource);
		else
			return false;
		end
	elseif(g_MonopolyResourceType == "Potential") then
		--print( "Looking at potential resources" );
		if(ePlayer ~= -1) then
			if(not Players[ePlayer]:HasGlobalMonopoly(eResource) and not Players[ePlayer]:HasStrategicMonopoly(eResource)) then
				return Players[ePlayer]:GetMonopolyPercent(eResource) > 0;
			end
		else
			return false;
		end
	end

	return false;
	--end
end

-- Get the player we want to display on a monopoly instance
function ChooseMonopolyPlayer( eResource )
	if(g_MonopolyResourcePlayer ~= -1) then
		return g_MonopolyResourcePlayer;
	end
	
	-- Greatest
	local eGreatest = Game.GetGreatestPlayerResourceMonopoly( eResource );
	return eGreatest;
end

function HookupResourceInstance(info)
	local instance = g_WorldMonopolyResourcesIM:GetInstance();

	local szResourceString = "";
	szResourceString = szResourceString .. info.ResourceIcon;
	szResourceString = szResourceString .. " " .. info.ResourceName;

	instance.ResourceLabel:SetText( szResourceString );
	instance.ResourceLabel:LocalizeAndSetToolTip( info.Resource.Help );

	-- Hide civ icon box if no one
	instance.CivIconBox:SetHide( info.PlayerID == -1 );

	if(info.PlayerID == -1) then
		instance.CivName:LocalizeAndSetText( "TXT_KEY_CPO_MP_NO_CIVILIZATION" );
	else
		instance.CivName:SetText(info.CivName);
		HookupCivControl( info.PlayerID, 32, instance.CivName, instance.CivIcon, instance.CivIconBG, instance.CivIconShadow );
	end

	
	local iNumResources = 0;
	
	if(info.PlayerID ~= -1) then
		iNumResources = Players[info.PlayerID]:GetNumResourceTotal(info.ResourceID, false) + Players[info.PlayerID]:GetResourceExport(info.ResourceID);
	end
	
	local iNumTotal = Map.GetNumResources(info.ResourceID);

	instance.PercentLabel:LocalizeAndSetToolTip("TXT_KEY_RESOURCE_BREAKDOWN", iNumResources, iNumTotal);

	instance.PercentLabel:LocalizeAndSetText("TXT_KEY_CPO_MP_MONOPOLY_PERCENT", info.MonopolyPercent);
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

-- Helper function to hookup a name + icon control
function HookupCivControl( playerID, size, nameControl, icon, iconBG, iconShadow )
	local pPlayer = Players[playerID];
	local civType = pPlayer:GetCivilizationType();
	local civInfo = GameInfo.Civilizations[civType];
	
	local playerIDDisplay = playerID;
	if (g_pTeam:IsHasMet(pPlayer:GetTeam()) ) then
		str = Locale.ConvertTextKey( civInfo.ShortDescription );
	else
		str = Locale.ConvertTextKey( "TXT_KEY_UNMET_PLAYER" );
		playerIDDisplay = -1;
	end

	if(nameControl ~= nil) then
		nameControl:LocalizeAndSetText( str );
	end

	CivIconHookup( playerIDDisplay, size, icon, iconBG, iconShadow, false, true);
end

-- Helper function to return civilization table for player
function GetPlayerCiv( ePlayer )
	local pPlayer = Players[ ePlayer ];
	local civType = pPlayer:GetCivilizationType();
	local civInfo = GameInfo.Civilizations[civType];
	return civInfo;
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

function UpdateAvailableCorporations()
	g_AvailableCorporationsIM:ResetInstances();

	for i, v in pairs(g_CorporationMonopolyResourceIMList) do
		v:ResetInstances();
	end
	g_CorporationMonopolyResourceIMList = {};

	pediaSearchStrings = {};

	for row in GameInfo.Corporations() do
		local HQClass = GameInfo.BuildingClasses[row.HeadquartersBuildingClass];
		local HQ = GameInfo.Buildings[HQClass.DefaultBuilding];
		
		local OfficeClass = GameInfo.BuildingClasses[row.OfficeBuildingClass];
		local Office = GameInfo.Buildings[OfficeClass.DefaultBuilding];

		local FranchiseClass = GameInfo.BuildingClasses[row.FranchiseBuildingClass];
		local Franchise = GameInfo.Buildings[FranchiseClass.DefaultBuilding];

		local founderTooltip = "";
		local pFoundingCity = nil;
		local pFoundingPlayer = nil;

		local bAvailable = true;
		local eFounder = Game.GetCorporationFounder(row.ID);
		
		local instance = g_AvailableCorporationsIM:GetInstance();
		
		-- do we need this?
		if(eFounder ~= -1) then
			bAvailable = false;
			HookupCivControl( eFounder, 64, nil, instance.CivIcon, instance.CivIconBG, instance.CivIconShadow );
		end
		
		instance.CorporationFounderBox:SetHide( bAvailable );

		-- Corporation Icon Hookup
		IconHookup( row.PortraitIndex, 128, row.IconAtlas, instance.CorporationPortrait );
		instance.CorporationName:LocalizeAndSetText(row.Description);

		-- Add hook so that right-clicking the Resource brings up the Civilopedia page for it
		-- search by text string - works, while the locale version doesn't (this smells like technical debt to me)
		pediaSearchStrings[tostring(instance.PortraitButton)] = row.Description;
		instance.PortraitButton:RegisterCallback( Mouse.eRClick, GetPedia );

		local g_CorporationMonopolyResourceIM = InstanceManager:new( "MonopolyResourceInstance", "MonopolyResourceBox", instance.MonopolyResourceStack );
		-- Get the resource monopoly list for this Corporation
		local resourceString = "";
		for resource in GameInfo.Corporation_ResourceMonopolyOrs( "CorporationType = '" .. row.Type .. "'" ) do
			local requiredResource = GameInfo.Resources[resource.ResourceType];
			if requiredResource then
				g_CorporationMonopolyResourceIMList[row.ID] = g_CorporationMonopolyResourceIM;
				local resourceInstance = g_CorporationMonopolyResourceIM:GetInstance();
				resourceInstance.ResourcesLabel:SetText( requiredResource.IconString );
				resourceInstance.ResourcesLabel:LocalizeAndSetToolTip( requiredResource.Description );
			end		
		end
		
		-- darken the box if it is not available
		if( not bAvailable ) then
			instance.AvailableCorporationBox:SetAlpha( 0.4 );
		else
			instance.AvailableCorporationBox:SetAlpha( 1.0 );
		end
		
		if( row.Help ~= nil) then
			local str = Locale.ConvertTextKey( row.Help ) .. " " .. Locale.ConvertTextKey( "TXT_KEY_CPO_RIGHT_CLICK_MORE_INFO" );
			instance.CorporationBonus:LocalizeAndSetText( str );
		end
	end

	if(Game.GetNumAvailableCorporations() > 0) then
		Controls.FoundCorporationStatus:LocalizeAndSetText( "TXT_KEY_CPO_FOUND_CORPORATION_AVAILABLE" );
	else
		Controls.FoundCorporationStatus:LocalizeAndSetText( "TXT_KEY_CPO_FOUND_CORPORATION_UNAVAILABLE" );
	end

	RecalcPanelSize();
end

function UpdateYourCorporation()
	--print( "In UpdateYourCorporation()" );

	local eCorporation = g_pPlayer:GetCorporation();

	if ( eCorporation == -1 ) then
		print( "ERROR: Got to UpdateYourCorporation() with no Corporation!");
		return;
	end

	local pCorporation = GameInfo.Corporations[eCorporation];
	if ( pCorporation == nil ) then
		print( "ERROR: pCorporation is nil!" );
		return;
	end

	local pHeadquartersClass = GameInfo.BuildingClasses[pCorporation.HeadquartersBuildingClass];
	local pHeadquarters = GameInfo.Buildings[pHeadquartersClass.DefaultBuilding];
	if( pHeadquarters ) then
		-- nothing?
	end
	
	local pOfficeClass = GameInfo.BuildingClasses[pCorporation.OfficeBuildingClass];
	local pOffice = GameInfo.Buildings[pOfficeClass.DefaultBuilding];
	if( pOffice ) then
		IconHookup( pOffice.PortraitIndex, 80, pOffice.IconAtlas, Controls.OfficePortrait );
		Controls.YourCorporationOfficeBox:LocalizeAndSetToolTip( pOffice.Help );
		
		pediaSearchStrings[tostring(Controls.YourCorporationOfficeBox)] = pOffice.Description;
		Controls.YourCorporationOfficeBox:RegisterCallback( Mouse.eRClick, GetPedia );
	end
	
	local pFranchiseClass = GameInfo.BuildingClasses[pCorporation.FranchiseBuildingClass];
	local pFranchise = GameInfo.Buildings[pFranchiseClass.DefaultBuilding];
	if( pFranchise ) then
		IconHookup( pFranchise.PortraitIndex, 80, pFranchise.IconAtlas, Controls.FranchisePortrait );
		Controls.YourCorporationFranchiseBox:LocalizeAndSetToolTip( pFranchise.Help );
		Controls.OOSortByTR:LocalizeAndSetToolTip( "TXT_KEY_CPO_OFFICE_TRADE_ROUTES_SORT_TT", pFranchise.Description );
		
		pediaSearchStrings[tostring(Controls.YourCorporationFranchiseBox)] = pFranchise.Description;
		Controls.YourCorporationFranchiseBox:RegisterCallback( Mouse.eRClick, GetPedia );
	end

	IconHookup( pCorporation.PortraitIndex, 128, pCorporation.IconAtlas, Controls.YourCorporationPortrait );
	pediaSearchStrings[tostring(Controls.YourCorporationButton)] = pCorporation.Description;
	Controls.YourCorporationButton:RegisterCallback( Mouse.eRClick, GetPedia );
	
	Controls.YourCorporationName:LocalizeAndSetText( pCorporation.Description );
	Controls.YourCorporationOffices:LocalizeAndSetText( "TXT_KEY_CPO_NUM_OFFICES", g_pPlayer:GetNumberofOffices(), g_pPlayer:GetNumCities() );
	Controls.YourCorporationOffices:LocalizeAndSetToolTip( "TXT_KEY_NUM_OFFICES_TT" );

	Controls.YourCorporationFranchises:LocalizeAndSetText( "TXT_KEY_CPO_NUM_FRANCHISES", g_pPlayer:GetNumberofGlobalFranchises(), g_pPlayer:GetMaxFranchises() );
	Controls.YourCorporationFranchises:LocalizeAndSetToolTip( "TXT_KEY_CORP_NUM_FRANCHISE_TT" );
	
	Controls.YourCorporationCurrentBenefit:SetText( g_pPlayer:GetCurrentOfficeBenefit() );

	-- Update date
	local date;
	local foundedTurn = g_pPlayer:GetCorporationFoundedTurn();
	
	-- note: cannnot a fixed date	
	--if (g_pPlayer:IsUsingMayaCalendar()) then
	--	date = g_pPlayer:GetMayaCalendarString();
	--	local toolTipString = Locale.ConvertTextKey("TXT_KEY_MAYA_DATE_TOOLTIP", g_pPlayer:GetMayaCalendarLongString(), traditionalDate);
	--	Controls.YourCorporationDate:SetToolTipString(toolTipString);
	--else
	--	date = traditionalDate;
	--end

	local pHeadquartersCity = Game.GetCorporationHeadquarters(eCorporation);
	if(pHeadquartersCity) then
		Controls.YourCorporationHQ:LocalizeAndSetText( "TXT_KEY_CPO_YOUR_CORPORATION_HQ", pHeadquartersCity:GetName() );
	end

	Controls.YourCorporationDate:LocalizeAndSetText("TXT_KEY_CPO_YOUR_CORPORATION_ESTABLISHED", Game.GetDateString(foundedTurn), foundedTurn);

	RefreshCorporationBenefits( pCorporation );
	RefreshOffices();
	RefreshFranchises();
end

function RefreshCorporationBenefits( pCorporation )
	--print("RefreshCorporationBenefits()");
	g_CorporationBenefitIM:ResetInstances();

	if(pCorporation == nil) then
		print("pCorporation is nil!");
		return;
	end

	if( pCorporation.ResourceBonusHelp ~= nil ) then
		InsertBenefit( "TXT_KEY_CPO_RESOURCE_BONUS", pCorporation.ResourceBonusHelp );
	end
	if( pCorporation.OfficeBonusHelp ~= nil ) then
		InsertBenefit( "TXT_KEY_CPO_OFFICE_BONUS", pCorporation.OfficeBonusHelp );
	end
	if( pCorporation.TradeRouteBonusHelp ~= nil ) then
		InsertBenefit( "TXT_KEY_CPO_TRADE_ROUTE_BONUS", pCorporation.TradeRouteBonusHelp );
	end

	Controls.CorporationBenefitStack:CalculateSize();
	Controls.CorporationBenefitStack:ReprocessAnchoring();
end

function InsertBenefit( name, desc )
	local instance = g_CorporationBenefitIM:GetInstance();
	instance.BenefitName:LocalizeAndSetText( name );
	instance.BenefitDescription:LocalizeAndSetText( desc );

	local bw,bh = instance.Base:GetSizeVal();
	local bdw,bdh = instance.BenefitDescription:GetSizeVal();
	instance.Base:SetSizeVal(bw, bdh + 18);
end

function RefreshOffices( )
	-- Clear buttons
	g_YourOfficesIM:ResetInstances();

	-- actual array holding city instances
	local myCities = {};

	local eCorporation = g_pPlayer:GetCorporation();
	if ( eCorporation == -1 ) then
		print( "ERROR: Got to RefreshOffices() with no Corporation!");
		return;
	end

	local pCorporation = GameInfo.Corporations[eCorporation];
	if ( pCorporation == nil ) then
		print( "ERROR: pCorporation is nil!" );
		return;
	end

	local pOfficeClass = GameInfo.BuildingClasses[pCorporation.OfficeBuildingClass];
	local pOffice = GameInfo.Buildings[pOfficeClass.DefaultBuilding];
	if( pOffice == nil ) then
		print( "ERROR: No defined office. Exiting RefreshOffices()" );
		return;
	end

	-- Find offices in our cities
	for city in g_pPlayer:Cities() do
		-- City has office?
		if(city:HasOffice()) then			
			local civType = g_pPlayer:GetCivilizationType();
			local civInfo = GameInfo.Civilizations[civType];
			local civName = Locale.ConvertTextKey( civInfo.ShortDescription );

			local iForeignTradeRoutes = 0;
			local strTradeCitiesTooltip = "";

			-- look through trade routes
			for i,v in ipairs(g_pPlayer:GetTradeRoutes()) do

				if( v.FromCity == city and v.ToCity:GetCivilizationType() ~= g_pPlayer:GetCivilizationType() ) then 
					iForeignTradeRoutes = iForeignTradeRoutes + 1;

					local strFranchised = "";
					if(not v.ToCity:IsFranchised(g_iSelectedPlayerID)) then
						strFranchised = Locale.ConvertTextKey("TXT_KEY_CPO_NO_FRANCHISE");
					else
						strFranchised = Locale.ConvertTextKey("TXT_KEY_CPO_FRANCHISED");
					end

					strTradeCitiesTooltip = strTradeCitiesTooltip .. Locale.ConvertTextKey("TXT_KEY_CPO_OFFICE_TRADE_ROUTES_INSTANCE_TT", v.FromCity:GetName(), v.ToCity:GetName(), strFranchised) .. "[NEWLINE]";
				end
			end
			
			local strTradeTooltip = "";
			if(iForeignTradeRoutes > 0) then
				strTradeTooltip = Locale.ConvertTextKey( "TXT_KEY_CPO_OFFICE_TRADE_ROUTES_TT" );
				strTradeTooltip = strTradeTooltip .. "[NEWLINE]" .. strTradeCitiesTooltip;
			end

			local info = {
				Civilization = civName,
				City = city,
				Name = city:GetName(),
				Player = g_iSelectedPlayerID,
				TradeToolTip = strTradeTooltip,
				TradeRoutes = iForeignTradeRoutes
			}
			table.insert(myCities, info);
		end
	end

	table.sort(myCities, g_YourOfficesSortFunction);

	-- Loop through sorted city
	for i,v in ipairs(myCities) do
		local officeCity = g_YourOfficesIM:GetInstance();
		
		CivIconHookup( v.Player, 32, officeCity.CivIcon, officeCity.CivIconBG, officeCity.CivIconShadow, false, true);
		
		local bCapital = Players[v.Player]:GetCapitalCity() == v.City;
		officeCity.CapitalIcon:SetHide( not bCapital );

		officeCity.CityTradeRoutes:SetToolTipString( v.TradeToolTip );
		officeCity.CityTradeRoutes:SetText( v.TradeRoutes );
		officeCity.CivilizationName:SetText( v.Civilization );
		officeCity.CityName:SetText( v.Name );
	end

	Controls.YourOfficesStack:CalculateSize();
	Controls.YourOfficesStack:ReprocessAnchoring();
	Controls.YourOfficesScrollPanel:CalculateInternalSize();
end

function RefreshFranchises( )
	g_YourFranchisesIM:ResetInstances();

	-- actual array holding city instances
	local foreignCities = {};

	-- Find our franchise in other player's cities
	for iPlayerLoop = 0, GameDefines.MAX_CIV_PLAYERS-1, 1 do
		local pOtherPlayer = Players[iPlayerLoop];
		if pOtherPlayer:IsEverAlive() then
			-- look at player's cities now
			for city in pOtherPlayer:Cities() do
				-- City has franchise?
				if(city:IsFranchised(g_iSelectedPlayerID)) then
					local civType = pOtherPlayer:GetCivilizationType();
					local civInfo = GameInfo.Civilizations[civType];
					local civName = Locale.ConvertTextKey( civInfo.ShortDescription );
			
					local info = {
						Civilization = civName,
						City = city,
						Name = city:GetName(),
						Player = iPlayerLoop
					}
					table.insert(foreignCities, info);
				end
			end
		end
	end

	table.sort(foreignCities, g_YourFranchisesSortFunction);

	-- Loop through sorted city
	for i,v in ipairs(foreignCities) do
		local franchiseCity = g_YourFranchisesIM:GetInstance();
		
		CivIconHookup( v.Player, 32, franchiseCity.CivIcon, franchiseCity.CivIconBG, franchiseCity.CivIconShadow, false, true);

		franchiseCity.CivilizationName:SetText( v.Civilization );
		franchiseCity.CityName:SetText( v.Name );
	end

	Controls.YourFranchisesStack:CalculateSize();
	Controls.YourFranchisesStack:ReprocessAnchoring();
	Controls.YourFranchisesScrollPanel:CalculateInternalSize();
end

-----------------------------------------------------------------
-----------------------------------------------------------------
function RecalcPanelSize()
	Controls.AvailableCorporationsStack:CalculateSize();

	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-- Pulldowns: note, functions must exist before we assign it to a variable

g_YourCorporationPulldownViews = {
	{
		Label = "TXT_KEY_CPO_YC_PULLDOWN_TYPE_OVERVIEW",
		Box = Controls.YourCorporationOverviewBox,
	},
	{
		Label = "TXT_KEY_CPO_YC_PULLDOWN_TYPE_OFFICES",
		Box = Controls.YourCorporationOfficesBox,
	}
}
g_YourCorporationSelectedPulldown = nil;

g_WorldCorporationsPulldownViews = {
	{
		Label = "TXT_KEY_CPO_WC_PULLDOWN_CORPORATIONS",
		Box = Controls.WorldCorporationsBox,
		Callback = RefreshWorldCorporations,
	},
	{
		Label = "TXT_KEY_CPO_WC_PULLDOWN_FRANCHISES",
		Box = Controls.WorldFranchisesBox,
		Callback = RefreshWorldFranchises,
	},
}
g_WorldCorporationsSelectedPulldown = nil;

g_MonopolyTypePulldownViews = {
	{
		Label = "TXT_KEY_CPO_MP_PULLDOWN_TYPE_ALL",
		Type = "All",
	},
	{
		Label = "TXT_KEY_CPO_MP_PULLDOWN_TYPE_GLOBAL",
		Type = "Global",
	},
	{
		Label = "TXT_KEY_CPO_MP_PULLDOWN_TYPE_STRATEGIC",
		Type = "Strategic",
	},
	{
		Label = "TXT_KEY_CPO_MP_PULLDOWN_TYPE_POTENTIAL",
		Type = "Potential",
	},
}

g_MonopolyResourceType = nil;

function BuildYourCorporationPulldown()
	local pullDown = Controls.YourCorporationPulldown;
	local controlTable;
	for i,v in pairs(g_YourCorporationPulldownViews) do
		controlTable = {};
		pullDown:BuildEntry( "InstanceOne", controlTable );
		controlTable.Button:LocalizeAndSetText( v.Label );
		controlTable.Button:SetVoid1( i );
	end
    pullDown:CalculateInternals();
end

function BuildWorldCorporationPulldown()
	local controlTable;
	for i,v in pairs(g_WorldCorporationsPulldownViews) do
		controlTable = {};
		Controls.WorldCorporationsPulldown:BuildEntry( "InstanceOne", controlTable );
		controlTable.Button:LocalizeAndSetText( v.Label );
		controlTable.Button:SetVoid1( i );
	end
    Controls.WorldCorporationsPulldown:CalculateInternals();
end

function BuildMonopolyCivPulldown()
	local pullDown = Controls.MonopolyCivPulldown;

	-- Greatest
	local instance = {};
	pullDown:BuildEntry( "InstanceOne", instance );
	instance.Button:LocalizeAndSetText( "TXT_KEY_CPO_MP_DROPDOWN_GREATEST" );
	instance.Button:SetVoid1( -1 );

	for iPlayerLoop=0, GameDefines.MAX_MAJOR_CIVS-1,1 do
		local pLoopPlayer = Players[iPlayerLoop];
		if(pLoopPlayer:IsAlive()) then
			local eTeam = pLoopPlayer:GetTeam();
			if(g_pTeam:IsHasMet(eTeam)) then
				local instance = {};
				pullDown:BuildEntry( "InstanceOne", instance );
				instance.Button:LocalizeAndSetText( GetPlayerCiv(iPlayerLoop).ShortDescription );
				instance.Button:SetVoid1( iPlayerLoop );
			end
		end
	end

	pullDown:CalculateInternals();
	pullDown:RegisterSelectionCallback( OnMonopolyCivPulldown );
end

function BuildMonopolyTypePulldown()
	local pullDown = Controls.MonopolyTypePulldown;

	for i,v in pairs(g_MonopolyTypePulldownViews) do
		local instance = {};
		pullDown:BuildEntry( "InstanceOne", instance );
		instance.Button:LocalizeAndSetText( v.Label );
		instance.Button:SetVoid1( i );
	end

	pullDown:CalculateInternals();
	pullDown:RegisterSelectionCallback( OnMonopolyTypePulldown );
end

function BuildPulldowns()
	BuildYourCorporationPulldown();
	BuildWorldCorporationPulldown();
	BuildMonopolyCivPulldown();
	BuildMonopolyTypePulldown();
	
	OnYourCorporationPulldown( 1 );
	OnWorldCorporationsPulldown( 1 );
	OnMonopolyCivPulldown( -1 );
	OnMonopolyTypePulldown( 1 );
end

function RefreshMonopolyPulldowns()
	Controls.MonopolyCivPulldown:ClearEntries();
	Controls.MonopolyTypePulldown:ClearEntries();

	BuildMonopolyCivPulldown();
	BuildMonopolyTypePulldown();
end

function OnYourCorporationPulldown( index )
	--print("OnYourCorporationPulldown()= " .. index);
	if(g_YourCorporationSelectedPulldown ~= nil) then
		g_YourCorporationSelectedPulldown.Box:SetHide( true );
	end

	g_YourCorporationSelectedPulldown = g_YourCorporationPulldownViews[index];
	g_YourCorporationSelectedPulldown.Box:SetHide( false );
	Controls.YourCorporationPulldown:GetButton():LocalizeAndSetText( g_YourCorporationSelectedPulldown.Label );

	DisplayCorporations();
end
Controls.YourCorporationPulldown:RegisterSelectionCallback( OnYourCorporationPulldown );

function OnWorldCorporationsPulldown( index )
	--print("OnWorldCorporationsPulldown()= " .. index);
	if(g_WorldCorporationsSelectedPulldown ~= nil) then
		g_WorldCorporationsSelectedPulldown.Box:SetHide( true );
	end

	g_WorldCorporationsSelectedPulldown = g_WorldCorporationsPulldownViews[index];
	g_WorldCorporationsSelectedPulldown.Box:SetHide( false );
	Controls.WorldCorporationsPulldown:GetButton():LocalizeAndSetText( g_WorldCorporationsSelectedPulldown.Label );

	g_WorldCorporationsSelectedPulldown.Callback();
end
Controls.WorldCorporationsPulldown:RegisterSelectionCallback( OnWorldCorporationsPulldown );

function OnMonopolyCivPulldown( ePlayer )
	--print("OnMonopolyCivPulldown()=" .. ePlayer);

	g_MonopolyResourcePlayer = ePlayer;
	
	if(ePlayer == -1) then
		Controls.MonopolyCivPulldown:GetButton():LocalizeAndSetText( "TXT_KEY_CPO_MP_DROPDOWN_GREATEST" );
	else
		Controls.MonopolyCivPulldown:GetButton():LocalizeAndSetText( GetPlayerCiv(ePlayer).ShortDescription );
	end

	RefreshMonopolies();
end

function OnMonopolyTypePulldown( index )
	--print("OnMonopolyTypePulldown()=" .. index);

	Controls.MonopolyTypePulldown:GetButton():LocalizeAndSetText(g_MonopolyTypePulldownViews[index].Label);

	g_MonopolyResourceType = g_MonopolyTypePulldownViews[index].Type;

	RefreshMonopolies();
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
Controls.TabButtonWorldCorporations:RegisterCallback( Mouse.eLClick, function() TabSelect("WorldCorporations"); end);
Controls.TabButtonMonopolies:RegisterCallback( Mouse.eLClick, function() TabSelect("Monopolies"); end);

-------------------------------------------------------------------------------
-- Sorting Support
-------------------------------------------------------------------------------
function AlphabeticalSortFunction(field, direction)
	if(direction == "asc") then
		return function(a,b)
			return Locale.Compare(a[field], b[field]) == -1;
		end
	elseif(direction == "desc") then
		return function(a,b)
			return Locale.Compare(a[field], b[field]) == 1;
		end
	end
end

function NumericSortFunction(field, direction)
	if(direction == "asc") then
		return function(a,b)
			local va = (a ~= nil and a[field] ~= nil) and a[field] or -1;
			local vb = (b ~= nil and b[field] ~= nil) and b[field] or -1;

			return tonumber(va) < tonumber(vb);
		end
	elseif(direction == "desc") then
		return function(a,b)
			local va = (a ~= nil and a[field] ~= nil) and a[field] or -1;
			local vb = (b ~= nil and b[field] ~= nil) and b[field] or -1;

			return tonumber(vb) < tonumber(va);
		end
	end
end

-- Registers the sort option controls click events
function RegisterSortOptions()
	for i,v in ipairs(g_YourOfficesSortOptions) do
		if(v.Button ~= nil) then
			v.Button:RegisterCallback(Mouse.eLClick, function() YourOfficesSortOptionSelected(v); end);
		end
	end
	
	for i,v in ipairs(g_YourFranchisesSortOptions) do
		if(v.Button ~= nil) then
			v.Button:RegisterCallback(Mouse.eLClick, function() YourFranchisesSortOptionSelected(v); end);
		end
	end

	for i,v in ipairs(g_WorldCorporationsSortOptions) do
		if(v.Button ~= nil) then
			v.Button:RegisterCallback(Mouse.eLClick, function() WorldCorporationsSortOptionSelected(v); end);
		end
	end

	for i,v in ipairs(g_WorldFranchisesSortOptions) do
		if(v.Button ~= nil) then
			v.Button:RegisterCallback(Mouse.eLClick, function() WorldFranchisesSortOptionSelected(v); end);
		end
	end

	for i,v in ipairs(g_MonopolyResourcesSortOptions) do
		if(v.Button ~= nil) then
			v.Button:RegisterCallback(Mouse.eLClick, function() MonopolyResourcesSortOptionSelected(v); end);
		end
	end

	UpdateSortOptionsDisplay(g_YourOfficesSortOptions);
	UpdateSortOptionsDisplay(g_YourFranchisesSortOptions);
	UpdateSortOptionsDisplay(g_WorldCorporationsSortOptions);
	UpdateSortOptionsDisplay(g_WorldFranchisesSortOptions);
	UpdateSortOptionsDisplay(g_MonopolyResourcesSortOptions);

	g_YourOfficesSortFunction = GetSortFunction(g_YourOfficesSortOptions);
	g_YourFranchisesSortFunction = GetSortFunction(g_YourFranchisesSortOptions);
	g_WorldCorporationsSortFunction = GetSortFunction(g_WorldCorporationsSortOptions);
	g_WorldFranchisesSortFunction = GetSortFunction(g_WorldFranchisesSortOptions);
	g_MonopolyResourcesSortFunction = GetSortFunction(g_MonopolyResourcesSortOptions);
end

function GetSortFunction(sortOptions)
	local orderBy = nil;
	for i,v in ipairs(sortOptions) do
		if(v.CurrentDirection ~= nil) then
			if(v.SortType == "numeric") then
				return NumericSortFunction(v.Column, v.CurrentDirection);
			else
				return AlphabeticalSortFunction(v.Column, v.CurrentDirection);
			end
		end
	end
	
	return nil;
end

-- Updates the sort option structure
function UpdateSortOptionState(sortOptions, selectedOption)
	-- Current behavior is to only have 1 sort option enabled at a time 
	-- though the rest of the structure is built to support multiple in the future.
	-- If a sort option was selected that wasn't already selected, use the default 
	-- direction.  Otherwise, toggle to the other direction.
	for i,v in ipairs(sortOptions) do
		if(v == selectedOption) then
			if(v.CurrentDirection == nil) then			
				v.CurrentDirection = v.DefaultDirection;
			else
				if(v.CurrentDirection == "asc") then
					v.CurrentDirection = "desc";
				else
					v.CurrentDirection = "asc";
				end
			end
		else
			v.CurrentDirection = nil;
		end
	end
end

-- Updates the control states of sort options
function UpdateSortOptionsDisplay(sortOptions)
	for i,v in ipairs(sortOptions) do
		local imageControl = v.ImageControl;
		if(imageControl ~= nil) then
			if(v.CurrentDirection == nil) then
				imageControl:SetHide(true);
			else
				local imageToUse = "SelectedUp.dds";
				if(v.CurrentDirection == "desc") then
					imageToUse = "SelectedDown.dds";
				end
				imageControl:SetTexture(imageToUse);
				
				imageControl:SetHide(false);
			end
		end
	end
end

-- Callback for when sort options are selected.
function YourOfficesSortOptionSelected(option)
	local sortOptions = g_YourOfficesSortOptions;
	UpdateSortOptionState(sortOptions, option);
	UpdateSortOptionsDisplay(sortOptions);
	g_YourOfficesSortFunction = GetSortFunction(sortOptions);
	
	RefreshOffices( );
end

-- Callback for when sort options are selected.
function YourFranchisesSortOptionSelected(option)
	local sortOptions = g_YourFranchisesSortOptions;
	UpdateSortOptionState(sortOptions, option);
	UpdateSortOptionsDisplay(sortOptions);
	g_YourFranchisesSortFunction = GetSortFunction(sortOptions);
	
	RefreshFranchises( ); 
end

-- Callback for when sort options are selected.
function WorldCorporationsSortOptionSelected(option)
	local sortOptions = g_WorldCorporationsSortOptions;
	UpdateSortOptionState(sortOptions, option);
	UpdateSortOptionsDisplay(sortOptions);
	g_WorldCorporationsSortFunction = GetSortFunction(sortOptions);

	RefreshWorldCorporations( ); 
end

-- Callback for when sort options are selected.
function WorldFranchisesSortOptionSelected(option)
	local sortOptions = g_WorldFranchisesSortOptions;
	UpdateSortOptionState(sortOptions, option);
	UpdateSortOptionsDisplay(sortOptions);
	g_WorldFranchisesSortFunction = GetSortFunction(sortOptions);
	
	RefreshWorldFranchises( ); 
end

-- Callback for when sort options are selected.
function MonopolyResourcesSortOptionSelected(option)
	local sortOptions = g_MonopolyResourcesSortOptions;
	UpdateSortOptionState(sortOptions, option);
	UpdateSortOptionsDisplay(sortOptions);
	g_MonopolyResourcesSortFunction = GetSortFunction(sortOptions);
	
	RefreshMonopolies( ); 
end

-------------------------------------------------
-- On Popup
-------------------------------------------------
function OnPopup( popupInfo )
	if( popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_MODDER_5 ) then
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
				Type = ButtonPopupTypes.BUTTONPOPUP_MODDER_5,
			};
		end,
	});
end);

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )

	g_iSelectedPlayerID = Game.GetActivePlayer();
	g_pPlayer = Players[g_iSelectedPlayerID];
	g_iTeam = g_pPlayer:GetTeam();
	g_pTeam = Teams[g_iTeam];

	-- Set player icon at top of screen
	CivIconHookup( Game.GetActivePlayer(), 64, Controls.CivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true );
   
    if( not bInitState ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
			TabSelect(g_CurrentTab);
        else
            UI.decTurnTimerSemaphore();
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_MODDER_5, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
g_Tabs["Corporations"].RefreshContent = function()
	DisplayCorporations();
end

g_Tabs["WorldCorporations"].RefreshContent = function()
	DisplayWorldCorporations();
end

g_Tabs["Monopolies"].RefreshContent = function()
	DisplayMonopolies();
end

-- Just in case :)
LuaEvents.RequestRefreshAdditionalInformationDropdownEntries();

RegisterSortOptions();
TabSelect("Monopolies");

-- Default pulldown selections
BuildPulldowns( );

SetBackground();
ContextPtr:SetHide(true);