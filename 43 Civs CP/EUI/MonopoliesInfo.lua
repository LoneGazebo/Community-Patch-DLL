print("This is the modded MonopoliesInfo from CBP")
-------------------------------------------------
-- Monopolies Info
-------------------------------------------------
include( "InstanceManager" );
include( "IconSupport" );

Controls.ResourcesControlledStack:SetHide( true );
Controls.ResourcesLocalStack:SetHide( true );
Controls.StrategicResourcesLocalStack:SetHide( true );
Controls.ResourcesForeignStack:SetHide( true );
Controls.ForeignStrategicResourcesStack:SetHide( true );

-------------------------------------------------
-------------------------------------------------
function OnResourcesLocalToggle()
    local bWasHidden = Controls.ResourcesLocalStack:IsHidden();
    Controls.ResourcesLocalStack:SetHide( not bWasHidden );
    local strString = Locale.ConvertTextKey("TXT_KEY_EO_MONOPOLIES");
    if( bWasHidden ) then
        Controls.ResourcesLocalToggle:SetText("[ICON_MINUS]" .. strString);
    else
        Controls.ResourcesLocalToggle:SetText("[ICON_PLUS]" .. strString);
    end
    Controls.MonopoliesStack:CalculateSize();
    Controls.MonopoliesStack:ReprocessAnchoring();
    Controls.MonopoliesScroll:CalculateInternalSize();
end

Controls.ResourcesLocalToggle:RegisterCallback( Mouse.eLClick, OnResourcesLocalToggle );

-------------------------------------------------
-------------------------------------------------
function OnStrategicResourcesLocalToggle()
    local bWasHidden = Controls.StrategicResourcesLocalStack:IsHidden();
    Controls.StrategicResourcesLocalStack:SetHide( not bWasHidden );
    local strString = Locale.ConvertTextKey("TXT_KEY_EO_MONOPOLIES_STRATEGIC");
    if( bWasHidden ) then
        Controls.StrategicResourcesLocalToggle:SetText("[ICON_MINUS]" .. strString);
    else
        Controls.StrategicResourcesLocalToggle:SetText("[ICON_PLUS]" .. strString);
    end
    Controls.MonopoliesStack:CalculateSize();
    Controls.MonopoliesStack:ReprocessAnchoring();
    Controls.MonopoliesScroll:CalculateInternalSize();
end

Controls.StrategicResourcesLocalToggle:RegisterCallback( Mouse.eLClick, OnStrategicResourcesLocalToggle );

-------------------------------------------------
-------------------------------------------------
function OnResourcesControlledToggle()
    local bWasHidden = Controls.ResourcesControlledStack:IsHidden();
    Controls.ResourcesControlledStack:SetHide( not bWasHidden );
    local strString = Locale.ConvertTextKey("TXT_KEY_EO_LOCAL_RESOURCES_CONTROLLED");
    if( bWasHidden ) then
        Controls.ResourcesControlledToggle:SetText("[ICON_MINUS]" .. strString);
    else
        Controls.ResourcesControlledToggle:SetText("[ICON_PLUS]" .. strString);
    end
    Controls.MonopoliesStack:CalculateSize();
    Controls.MonopoliesStack:ReprocessAnchoring();
    Controls.MonopoliesScroll:CalculateInternalSize();
end

Controls.ResourcesControlledToggle:RegisterCallback( Mouse.eLClick, OnResourcesControlledToggle );


-------------------------------------------------
-------------------------------------------------
function OnResourcesForeignToggle()
    local bWasHidden = Controls.ResourcesForeignStack:IsHidden();
    Controls.ResourcesForeignStack:SetHide( not bWasHidden );
    local strString = Locale.ConvertTextKey("TXT_KEY_EO_LOCAL_RESOURCES_FOREIGN");
    if( bWasHidden ) then
        Controls.ResourcesForeignToggle:SetText("[ICON_MINUS]" .. strString);
    else
        Controls.ResourcesForeignToggle:SetText("[ICON_PLUS]" .. strString);
    end
    Controls.MonopoliesStack:CalculateSize();
    Controls.MonopoliesStack:ReprocessAnchoring();
    Controls.MonopoliesScroll:CalculateInternalSize();
end

Controls.ResourcesForeignToggle:RegisterCallback( Mouse.eLClick, OnResourcesForeignToggle );

-------------------------------------------------
-------------------------------------------------
function OnForeignStrategicResourcesToggle()
    local bWasHidden = Controls.ForeignStrategicResourcesStack:IsHidden();
    Controls.ForeignStrategicResourcesStack:SetHide( not bWasHidden );
    local strString = Locale.ConvertTextKey("TXT_KEY_EO_LOCAL_RESOURCES_FOREIGN_STRATEGIC");
    if( bWasHidden ) then
        Controls.ForeignStrategicResourcesToggle:SetText("[ICON_MINUS]" .. strString);
    else
        Controls.ForeignStrategicResourcesToggle:SetText("[ICON_PLUS]" .. strString);
    end
    Controls.MonopoliesStack:CalculateSize();
    Controls.MonopoliesStack:ReprocessAnchoring();
    Controls.MonopoliesScroll:CalculateInternalSize();
end

Controls.ForeignStrategicResourcesToggle:RegisterCallback( Mouse.eLClick, OnForeignStrategicResourcesToggle );

--CBP
-------------------------------------------------
-------------------------------------------------
function UpdateCorporations()

    local pPlayer = Players[ Game.GetActivePlayer() ];
	local g_iUs = Game.GetActivePlayer();
	g_iUsTeam = pPlayer:GetTeam();
	g_pUsTeam = Teams[ g_iUsTeam ];
	local strCorpName = pPlayer:GetCorporationName();
	local iFranchises = pPlayer:GetNumberofGlobalFranchises();
	local CorpDetails = Locale.ConvertTextKey("TXT_KEY_CORP_EX_DESC");
	if(strCorpName ~= nil) then
		for building in GameInfo.Buildings() do
			local thisBuilding = GameInfo.Buildings[building.ID];
			if(thisBuilding.CorporationHQID == pPlayer:GetCorpID()) then
				IconHookup( thisBuilding.PortraitIndex, 128, thisBuilding.IconAtlas, Controls.BuildingImage )
			end
		end
		CorpDetails = pPlayer:GetCorporationHelper();
		Controls.CorporationName:SetText("[COLOR_POSITIVE_TEXT]" .. strCorpName .. "[ENDCOLOR]");
		Controls.CorporationName:SetToolTipString((Locale.ConvertTextKey(CorpDetails)));
		local iMaxFranchises = pPlayer:GetMaxFranchises();
		local strMaxFran = Locale.ConvertTextKey("TXT_KEY_CORP_MAX_FRANCHISE", iFranchises, iMaxFranchises);
		local strMaxFranTT = Locale.ConvertTextKey("TXT_KEY_CORP_MAX_FRANCHISE_TT", iMaxFranchises);
		Controls.MaxFranchises:SetText(strMaxFran);	
		Controls.MaxFranchises:SetToolTipString(strMaxFranTT);
		Controls.FounderIconBG:SetHide( true );
		Controls.FounderIcon:SetHide( true );
		Controls.FounderIconShadow:SetHide( true );
		Controls.FounderIconHighlight:SetHide( true );
		Controls.BuildingImage:SetHide( false );
		Controls.BuildingImage:SetToolTipString((Locale.ConvertTextKey(CorpDetails)));
		if (not OptionsManager.IsNoBasicHelp()) then
			Controls.HoverInfo:SetHide( false );
			Controls.HoverInfo:SetText(Locale.ConvertTextKey("TXT_KEY_HOVER_INFO"));	
		end
	else
		CivIconHookup(pPlayer:GetID(), 80, Controls.FounderIcon, Controls.FounderIconBG, Controls.FounderIconShadow, true, true );
		strCorpName = Locale.ConvertTextKey("TXT_KEY_CORP_NO_CORP");
		Controls.CorporationName:SetText(strCorpName);	
		Controls.BuildingImage:SetHide( true );
		Controls.FounderIconBG:SetHide( false );
		Controls.FounderIcon:SetHide( false );
		Controls.FounderIconShadow:SetHide( false );
		Controls.FounderIconHighlight:SetHide( false );
		Controls.HoverInfo:SetHide( true );
	end


	-- Franchises
    if( iFranchises > 0 ) then
        Controls.NumFranchises:SetText( Locale.ToNumber(iFranchises, "#") );
    else
        Controls.NumFranchises:SetText( 0 );
    end

	-- Offices
    local iOffices = pPlayer:GetNumberofOffices();
    if( iOffices > 0 ) then
        Controls.NumOffices:SetText( Locale.ToNumber(iOffices, "#") );
    else
        Controls.NumOffices:SetText( 0 );
    end

	-- FRANCHISE LIST
	local bFoundForeignCity = false;
	Controls.FranchiseStack:DestroyAllChildren();
	for iOtherPlayer = 0, GameDefines.MAX_CIV_PLAYERS - 1 do		
		local pOtherPlayer = Players[ iOtherPlayer ];
		g_iThemTeam = pOtherPlayer:GetTeam();
		g_pThemTeam = Teams[ g_iThemTeam ]; 
		-- Valid player? - Can't be us, and has to be alive
		if (iOtherPlayer ~= g_iUs and pOtherPlayer:IsAlive()) then
			g_iThemTeam = pOtherPlayer:GetTeam();
			g_pThemTeam = Teams[ g_iThemTeam ];
			if(g_pThemTeam:IsHasMet( g_iUsTeam )) then
				for pOtherCity in pOtherPlayer:Cities() do
					if(pOtherCity:IsFranchised(g_iUs)) then
						bFoundForeignCity = true;
						local instance = {};
						ContextPtr:BuildInstanceForControl( "FranchiseEntry", instance, Controls.FranchiseStack );
						instance.CityName:SetText( pOtherCity:GetName() );
						local strCivName = pOtherPlayer:GetCivilizationShortDescription()
						instance.CityName:SetToolTipString(strCivName);
						instance.FranchisePresent:SetText("[ICON_INVEST]");
					end
				end
			end
		end
	end
	if( bFoundForeignCity ) then
		Controls.FranchiseToggle:SetDisabled( false );
		Controls.FranchiseToggle:SetAlpha( 1.0 );
	else
		Controls.FranchiseToggle:SetDisabled( true );
		Controls.FranchiseToggle:SetAlpha( 0.5 );
	end

	Controls.FranchiseStack:CalculateSize();
	Controls.FranchiseStack:ReprocessAnchoring();

	-- OFFICE LIST
	local bFoundCity = false;
    Controls.OfficeStack:DestroyAllChildren();
    for pCity in pPlayer:Cities() do
		if(pCity:HasOffice()) then
			bFoundCity = true;
			local instance = {};
			ContextPtr:BuildInstanceForControl( "OfficeEntry", instance, Controls.OfficeStack );
			instance.CityName:SetText( pCity:GetName() );
			instance.OfficePresent:SetText("[ICON_CHECKBOX]");
		end
	end
	if( bFoundCity ) then
		Controls.OfficeToggle:SetDisabled( false );
		Controls.OfficeToggle:SetAlpha( 1.0 );
	else
		Controls.OfficeToggle:SetDisabled( true );
		Controls.OfficeToggle:SetAlpha( 0.5 );
	end

	Controls.OfficeStack:CalculateSize();
	Controls.OfficeStack:ReprocessAnchoring();

	-- FOREIGN CORPS
	local bFoundForeignCorps = false;
	Controls.ForeignCorpsStack:DestroyAllChildren();
	for iOtherPlayer = 0, GameDefines.MAX_CIV_PLAYERS - 1 do		
		local pOtherPlayer = Players[ iOtherPlayer ];
		g_iThemTeam = pOtherPlayer:GetTeam();
		g_pThemTeam = Teams[ g_iThemTeam ]; 
		-- Valid player? - Can't be us, and has to be alive
		if (iOtherPlayer ~= g_iUs and pOtherPlayer ~= nil and pOtherPlayer:IsAlive()) then
			g_iThemTeam = pOtherPlayer:GetTeam();
			g_pThemTeam = Teams[ g_iThemTeam ];
			if(g_pThemTeam:IsHasMet( g_iUsTeam )) then
				local strCorpName = pOtherPlayer:GetCorporationName();
				if(strCorpName ~= nil) then
					bFoundForeignCorps = true;	
					local iFranchises = pOtherPlayer:GetNumberofGlobalFranchises();		
					local instance = {};
					ContextPtr:BuildInstanceForControl( "ForeignCorpsEntry", instance, Controls.ForeignCorpsStack );
					instance.CorpName:SetText( strCorpName );
					local strCivName = pOtherPlayer:GetCivilizationShortDescription();
					local strCorpNameTT = pOtherPlayer:GetCorporationHelper();
					instance.CorpName:SetToolTipString("[COLOR_POSITIVE_TEXT]" .. strCivName .. "[ENDCOLOR]" .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey(strCorpNameTT));
					if( iFranchises > 0 ) then
						instance.ForeignCorpsSize:SetText( Locale.ToNumber(iFranchises, "#") );
					else
						instance.ForeignCorpsSize:SetText( 0 );
					end
				end
			end
		end
	end
	if( bFoundForeignCorps ) then
		Controls.ForeignCorpsToggle:SetDisabled( false );
		Controls.ForeignCorpsToggle:SetAlpha( 1.0 );
	else
		Controls.ForeignCorpsToggle:SetDisabled( true );
		Controls.ForeignCorpsToggle:SetAlpha( 0.5 );
	end

	Controls.ForeignCorpsStack:CalculateSize();
	Controls.ForeignCorpsStack:ReprocessAnchoring();

	Controls.CorpScroll:CalculateInternalSize();
end

Controls.FranchiseStack:SetHide( true );
Controls.OfficeStack:SetHide( true );

function UpdateMonopolies()
	local pPlayer = Players[ Game.GetActivePlayer() ];
	local g_iUs = Game.GetActivePlayer();
	g_iUsTeam = pPlayer:GetTeam();
	g_pUsTeam = Teams[ g_iUsTeam ];
	-- Our Monopolies
	Controls.ResourcesLocalToggle:SetText("[ICON_PLUS]" .. Locale.ConvertTextKey("TXT_KEY_EO_MONOPOLIES"));
	iTotalNumResources = 0;
    Controls.ResourcesLocalStack:DestroyAllChildren();
	for pResource in GameInfo.Resources() do
		local iResourceID = pResource.ID;
		if (Game.GetResourceUsageType(iResourceID) == ResourceUsageTypes.RESOURCEUSAGE_LUXURY) then	
			local iNum = pPlayer:GetNumResourceTotal(iResourceID, false) + pPlayer:GetResourceExport(iResourceID);
			local iMonopoly = 0;
			local iTotal = 0;
			if(iNum > 0) then
				iTotal = Map.GetNumResources(iResourceID);
				if(iTotal > 0) then
					iMonopoly = (iNum * 100) / iTotal;
				end
			end
			if (iMonopoly > 50) then
				iTotalNumResources = iTotalNumResources + 1;

				local instance = {};
				ContextPtr:BuildInstanceForControl( "ResourceEntry", instance, Controls.ResourcesLocalStack );

				instance.ResourceName:SetText(Locale.ConvertTextKey(pResource.IconString) .. " " .. Locale.ConvertTextKey(pResource.Description));
				instance.ResourceValue:SetText("[COLOR_POSITIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_RESOURCE_MONOPOLY", Locale.ToNumber( iMonopoly, "#" )) .. "[ENDCOLOR]");

				local strTooltip = Locale.ConvertTextKey("TXT_KEY_RESOURCE_YOU_GAIN_GLOBAL") .. Locale.Lookup(pResource.Help);
				instance.ResourceName:SetToolTipString(strTooltip);
				local strTooltip2 = Locale.ConvertTextKey("TXT_KEY_RESOURCE_BREAKDOWN", iNum, iTotal)
				instance.ResourceValue:SetToolTipString(strTooltip2);
			end
		elseif (Game.GetResourceUsageType(iResourceID) == ResourceUsageTypes.RESOURCEUSAGE_STRATEGIC) then	
			local iNum = pPlayer:GetNumResourceTotal(iResourceID, false) + pPlayer:GetResourceExport(iResourceID);
			local iMonopoly = 0;
			local iTotal = 0;
			if(iNum > 0) then
				iTotal = Map.GetNumResources(iResourceID);
				if(iTotal > 0) then
					iMonopoly = (iNum * 100) / iTotal;
				end
			end
			if (iMonopoly > 50) then
				iTotalNumResources = iTotalNumResources + 1;

				local instance = {};
				ContextPtr:BuildInstanceForControl( "ResourceEntry", instance, Controls.ResourcesLocalStack );

				instance.ResourceName:SetText(Locale.ConvertTextKey(pResource.IconString) .. " " .. Locale.ConvertTextKey(pResource.Description));
				instance.ResourceValue:SetText("[COLOR_POSITIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_RESOURCE_MONOPOLY", Locale.ToNumber( iMonopoly, "#" )) .. "[ENDCOLOR]");

				local strTooltip = Locale.ConvertTextKey("TXT_KEY_RESOURCE_YOU_GAIN_GLOBAL_AND_STRATEGIC") .. Locale.Lookup(pResource.Help);
				instance.ResourceName:SetToolTipString(strTooltip);
				local strTooltip2 = Locale.ConvertTextKey("TXT_KEY_RESOURCE_BREAKDOWN", iNum, iTotal)
				instance.ResourceValue:SetToolTipString(strTooltip2);
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

	-- Our Strategic Monopolies
	Controls.StrategicResourcesLocalToggle:SetText("[ICON_PLUS]" .. Locale.ConvertTextKey("TXT_KEY_EO_MONOPOLIES_STRATEGIC"));
	iTotalNumResources = 0;
    Controls.StrategicResourcesLocalStack:DestroyAllChildren();
	for pResource in GameInfo.Resources() do
		local iResourceID = pResource.ID;
		if (Game.GetResourceUsageType(iResourceID) == ResourceUsageTypes.RESOURCEUSAGE_STRATEGIC) then	
			local iNum = pPlayer:GetNumResourceTotal(iResourceID, false) + pPlayer:GetResourceExport(iResourceID);
			local iMonopoly = 0;
			local iTotal = 0;
			if(iNum > 0) then
				iTotal = Map.GetNumResources(iResourceID);
				if(iTotal > 0) then
					iMonopoly = (iNum * 100) / iTotal;
				end
			end
			if (iMonopoly > 25 and iMonopoly <= 50) then
				iTotalNumResources = iTotalNumResources + 1;

				local instance = {};
				ContextPtr:BuildInstanceForControl( "ResourceEntry", instance, Controls.StrategicResourcesLocalStack );

				instance.ResourceName:SetText(Locale.ConvertTextKey(pResource.IconString) .. " " .. Locale.ConvertTextKey(pResource.Description));
				instance.ResourceValue:SetText("[COLOR_POSITIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_RESOURCE_MONOPOLY", Locale.ToNumber( iMonopoly, "#" )) .. "[ENDCOLOR]");

				local strTooltip = Locale.ConvertTextKey("TXT_KEY_RESOURCE_YOU_GAIN_STRATEGIC") .. Locale.Lookup(pResource.StrategicHelp);
				instance.ResourceName:SetToolTipString(strTooltip);
				local strTooltip2 = Locale.ConvertTextKey("TXT_KEY_RESOURCE_BREAKDOWN", iNum, iTotal)
				instance.ResourceValue:SetToolTipString(strTooltip2);
			end
		end
	end
	
    if ( iTotalNumResources > 0 ) then
        Controls.StrategicResourcesLocalToggle:SetDisabled( false );
        Controls.StrategicResourcesLocalToggle:SetAlpha( 1.0 );
    else
        Controls.StrategicResourcesLocalToggle:SetDisabled( true );
        Controls.StrategicResourcesLocalToggle:SetAlpha( 0.5 );
    end
    Controls.StrategicResourcesLocalStack:CalculateSize();
    Controls.StrategicResourcesLocalStack:ReprocessAnchoring();

	-- Potential Monopolies
	
    Controls.ResourcesControlledToggle:SetText("[ICON_PLUS]" .. Locale.ConvertTextKey("TXT_KEY_EO_LOCAL_RESOURCES_CONTROLLED"));
	
	iTotalNumResources = 0;
	
    Controls.ResourcesControlledStack:DestroyAllChildren();
	for pResource in GameInfo.Resources() do
		local iResourceID = pResource.ID;
		if (Game.GetResourceUsageType(iResourceID) == ResourceUsageTypes.RESOURCEUSAGE_LUXURY) then
			local iNum = pPlayer:GetNumResourceTotal(iResourceID, false) + pPlayer:GetResourceExport(iResourceID);
			local iMonopoly = 0;
			local iTotal = 0;
			if(iNum > 0) then
				iTotal = Map.GetNumResources(iResourceID);
				if(iTotal > 0) then
					iMonopoly = (iNum * 100) / iTotal;
				end
			end
			if(iMonopoly > 0 and iMonopoly <= 50) then
				iTotalNumResources = iTotalNumResources + 1;
				
				local instance = {};
				ContextPtr:BuildInstanceForControl( "ResourceEntry", instance, Controls.ResourcesControlledStack );
		        
				instance.ResourceName:SetText(Locale.ConvertTextKey(pResource.IconString) .. " " .. Locale.ConvertTextKey(pResource.Description));
				instance.ResourceValue:SetText(Locale.ConvertTextKey("TXT_KEY_RESOURCE_CONTROLLED", Locale.ToNumber( iMonopoly, "#" )));
				local strTooltip = Locale.ConvertTextKey("TXT_KEY_RESOURCE_POTENTIAL") .. Locale.Lookup(pResource.Help);
				instance.ResourceName:SetToolTipString(strTooltip);
				local strTooltip2 = Locale.ConvertTextKey("TXT_KEY_RESOURCE_BREAKDOWN", iNum, iTotal)
				instance.ResourceValue:SetToolTipString(strTooltip2);
			end
		elseif (Game.GetResourceUsageType(iResourceID) == ResourceUsageTypes.RESOURCEUSAGE_STRATEGIC) then
			local iNum = pPlayer:GetNumResourceTotal(iResourceID, false) + pPlayer:GetResourceExport(iResourceID);
			local iMonopoly = 0;
			local iTotal = 0;
			if(iNum > 0) then
				iTotal = Map.GetNumResources(iResourceID);
				if(iTotal > 0) then
					iMonopoly = (iNum * 100) / iTotal;
				end
			end
			if(iMonopoly > 0 and iMonopoly <= 25) then
				iTotalNumResources = iTotalNumResources + 1;
				
				local instance = {};
				ContextPtr:BuildInstanceForControl( "ResourceEntry", instance, Controls.ResourcesControlledStack );
		        
				instance.ResourceName:SetText(Locale.ConvertTextKey(pResource.IconString) .. " " .. Locale.ConvertTextKey(pResource.Description));
				instance.ResourceValue:SetText(Locale.ConvertTextKey("TXT_KEY_RESOURCE_CONTROLLED", Locale.ToNumber( iMonopoly, "#" )));
				local strTooltip = Locale.ConvertTextKey("TXT_KEY_RESOURCE_POTENTIAL_STRATEGIC") .. Locale.Lookup(pResource.Help);
				instance.ResourceName:SetToolTipString(strTooltip);
				local strTooltip2 = Locale.ConvertTextKey("TXT_KEY_RESOURCE_BREAKDOWN", iNum, iTotal)
				instance.ResourceValue:SetToolTipString(strTooltip2);
			end
		end
	end
    if ( iTotalNumResources > 0 ) then
        Controls.ResourcesControlledToggle:SetDisabled( false );
        Controls.ResourcesControlledToggle:SetAlpha( 1.0 );
    else
        Controls.ResourcesControlledToggle:SetDisabled( true );
        Controls.ResourcesControlledToggle:SetAlpha( 0.5 );
    end
    Controls.ResourcesControlledStack:CalculateSize();
    Controls.ResourcesControlledStack:ReprocessAnchoring();

	-- Foreign Monopolies
	Controls.ResourcesForeignToggle:SetText("[ICON_PLUS]" .. Locale.ConvertTextKey("TXT_KEY_EO_LOCAL_RESOURCES_FOREIGN"));
	iTotalNumResources = 0;
    Controls.ResourcesForeignStack:DestroyAllChildren();
	for iOtherPlayer = 0, GameDefines.MAX_CIV_PLAYERS - 1 do		
		local pOtherPlayer = Players[ iOtherPlayer ];
		g_iThemTeam = pOtherPlayer:GetTeam();
		g_pThemTeam = Teams[ g_iThemTeam ]; 
		-- Valid player? - Can't be us, and has to be alive
		if (iOtherPlayer ~= g_iUs and pOtherPlayer ~= nil and pOtherPlayer:IsAlive()) then
			g_iThemTeam = pOtherPlayer:GetTeam();
			g_pThemTeam = Teams[ g_iThemTeam ];
			if(g_pThemTeam:IsHasMet( g_iUsTeam )) then
				for pResource in GameInfo.Resources() do
					local iResourceID = pResource.ID;
					if (Game.GetResourceUsageType(iResourceID) == ResourceUsageTypes.RESOURCEUSAGE_LUXURY) then
						if(pOtherPlayer:GetResourceMonopolyPlayer(iResourceID)) then
							local iMonopoly = 0;
							local iOtherNum = pOtherPlayer:GetNumResourceTotal(iResourceID, false) + pOtherPlayer:GetResourceExport(iResourceID);
							local iOtherTotal = Map.GetNumResources(iResourceID);
							if(iOtherTotal > 0) then
								iMonopoly = (iOtherNum * 100) / iOtherTotal;
							end
							if(iMonopoly > 50) then
								iTotalNumResources = iTotalNumResources + 1;
				
								local instance = {};
								ContextPtr:BuildInstanceForControl( "ResourceEntry", instance, Controls.ResourcesForeignStack );
		        
								instance.ResourceName:SetText(Locale.ConvertTextKey(pResource.IconString) .. " " .. Locale.ConvertTextKey(pResource.Description));
								instance.ResourceValue:SetText("[COLOR_NEGATIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_RESOURCE_MONOPOLY_OTHER", Locale.ToNumber( iMonopoly, "#" ), pOtherPlayer:GetCivilizationShortDescriptionKey()) .. "[ENDCOLOR]");

								local strTooltip = Locale.ConvertTextKey("TXT_KEY_RESOURCE_THEY_GAIN_GLOBAL") .. Locale.Lookup(pResource.Help);
								instance.ResourceName:SetToolTipString(strTooltip);
								local strTooltip2 = Locale.ConvertTextKey("TXT_KEY_RESOURCE_BREAKDOWN", iOtherNum, iOtherTotal)
								instance.ResourceValue:SetToolTipString(strTooltip2);
							end
						end
					elseif(Game.GetResourceUsageType(iResourceID) == ResourceUsageTypes.RESOURCEUSAGE_STRATEGIC) then
						if(pOtherPlayer:GetResourceMonopolyPlayer(iResourceID)) then
							local iMonopoly = 0;
							local iOtherNum = pOtherPlayer:GetNumResourceTotal(iResourceID, false) + pOtherPlayer:GetResourceExport(iResourceID);
							local iOtherTotal = Map.GetNumResources(iResourceID);
							if(iOtherTotal > 0) then
								iMonopoly = (iOtherNum * 100) / iOtherTotal;
							end
							if(iMonopoly > 50) then
								iTotalNumResources = iTotalNumResources + 1;
				
								local instance = {};
								ContextPtr:BuildInstanceForControl( "ResourceEntry", instance, Controls.ResourcesForeignStack );
		        
								instance.ResourceName:SetText(Locale.ConvertTextKey(pResource.IconString) .. " " .. Locale.ConvertTextKey(pResource.Description));
								instance.ResourceValue:SetText("[COLOR_NEGATIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_RESOURCE_MONOPOLY_OTHER", Locale.ToNumber( iMonopoly, "#" ), pOtherPlayer:GetCivilizationShortDescriptionKey()) .. "[ENDCOLOR]");

								local strTooltip = Locale.ConvertTextKey("TXT_KEY_RESOURCE_THEY_GAIN_BOTH") .. Locale.Lookup(pResource.Help);
								instance.ResourceName:SetToolTipString(strTooltip);
								local strTooltip2 = Locale.ConvertTextKey("TXT_KEY_RESOURCE_BREAKDOWN", iOtherNum, iOtherTotal)
								instance.ResourceValue:SetToolTipString(strTooltip2);
							end
						end
					end
				end
			end
		end
	end

    if ( iTotalNumResources > 0 ) then
        Controls.ResourcesForeignToggle:SetDisabled( false );
        Controls.ResourcesForeignToggle:SetAlpha( 1.0 );
    else
        Controls.ResourcesForeignToggle:SetDisabled( true );
        Controls.ResourcesForeignToggle:SetAlpha( 0.5 );
    end
    Controls.ResourcesForeignStack:CalculateSize();
    Controls.ResourcesForeignStack:ReprocessAnchoring();

	-- Strategic Foreign Monopolies
	Controls.ForeignStrategicResourcesToggle:SetText("[ICON_PLUS]" .. Locale.ConvertTextKey("TXT_KEY_EO_LOCAL_RESOURCES_FOREIGN_STRATEGIC"));
	iTotalNumResources = 0;
    Controls.ForeignStrategicResourcesStack:DestroyAllChildren();
	for iOtherPlayer = 0, GameDefines.MAX_CIV_PLAYERS - 1 do		
		local pOtherPlayer = Players[ iOtherPlayer ];
		-- Valid player? - Can't be us, and has to be alive
		if (iOtherPlayer ~= g_iUs and pOtherPlayer ~= nil and pOtherPlayer:IsAlive()) then
			g_iThemTeam = pOtherPlayer:GetTeam();
			g_pThemTeam = Teams[ g_iThemTeam ];
			if(g_pThemTeam:IsHasMet( g_iUsTeam )) then
				for pResource in GameInfo.Resources() do
					local iResourceID = pResource.ID;
					if (Game.GetResourceUsageType(iResourceID) == ResourceUsageTypes.RESOURCEUSAGE_STRATEGIC) then
						local iMonopoly = 0;
						local iOtherNum = pOtherPlayer:GetNumResourceTotal(iResourceID, false) + pOtherPlayer:GetResourceExport(iResourceID);
						local iOtherTotal = Map.GetNumResources(iResourceID);
						if(iOtherTotal > 0) then
							iMonopoly = (iOtherNum * 100) / iOtherTotal;
						end
						if(iMonopoly > 25 and iMonopoly <= 50) then
							iTotalNumResources = iTotalNumResources + 1;
				
							local instance = {};
							ContextPtr:BuildInstanceForControl( "ResourceEntry", instance, Controls.ForeignStrategicResourcesStack );
		        
							instance.ResourceName:SetText(Locale.ConvertTextKey(pResource.IconString) .. " " .. Locale.ConvertTextKey(pResource.Description));
							instance.ResourceValue:SetText("[COLOR_NEGATIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_RESOURCE_MONOPOLY_OTHER", Locale.ToNumber( iMonopoly, "#" ), pOtherPlayer:GetCivilizationShortDescriptionKey()) .. "[ENDCOLOR]");

							local strTooltip = Locale.ConvertTextKey("TXT_KEY_RESOURCE_THEY_GAIN_STRATEGIC") .. Locale.Lookup(pResource.StrategicHelp);
							instance.ResourceName:SetToolTipString(strTooltip);
							local strTooltip2 = Locale.ConvertTextKey("TXT_KEY_RESOURCE_BREAKDOWN", iOtherNum, iOtherTotal)
							instance.ResourceValue:SetToolTipString(strTooltip2);
						end
					end
				end
			end
		end
	end

    if ( iTotalNumResources > 0 ) then
        Controls.ForeignStrategicResourcesToggle:SetDisabled( false );
        Controls.ForeignStrategicResourcesToggle:SetAlpha( 1.0 );
    else
        Controls.ForeignStrategicResourcesToggle:SetDisabled( true );
        Controls.ForeignStrategicResourcesToggle:SetAlpha( 0.5 );
    end
    Controls.ForeignStrategicResourcesStack:CalculateSize();
    Controls.ForeignStrategicResourcesStack:ReprocessAnchoring();

	Controls.MonopoliesScroll:CalculateInternalSize();
end
-------------------------------------------------
-------------------------------------------------
function OnFranchiseToggle()
    local bWasHidden = Controls.FranchiseStack:IsHidden();
    Controls.FranchiseStack:SetHide( not bWasHidden );
    if( bWasHidden ) then
        Controls.FranchiseToggle:LocalizeAndSetText("TXT_KEY_EO_FRANCHISE_DETAILS_COLLAPSE");
    else
        Controls.FranchiseToggle:LocalizeAndSetText("TXT_KEY_EO_FRANCHISE_DETAILS");
    end
    Controls.CorpStack:CalculateSize();
    Controls.CorpStack:ReprocessAnchoring();
    Controls.CorpScroll:CalculateInternalSize();
end
Controls.FranchiseToggle:RegisterCallback( Mouse.eLClick, OnFranchiseToggle );
-------------------------------------------------
-------------------------------------------------
function OnOfficeToggle()
    local bWasHidden = Controls.OfficeStack:IsHidden();
    Controls.OfficeStack:SetHide( not bWasHidden );
    if( bWasHidden ) then
        Controls.OfficeToggle:LocalizeAndSetText("TXT_KEY_EO_OFFICE_DETAILS_COLLAPSE");
    else
        Controls.OfficeToggle:LocalizeAndSetText("TXT_KEY_EO_OFFICE_DETAILS");
    end
    Controls.CorpStack:CalculateSize();
    Controls.CorpStack:ReprocessAnchoring();
    Controls.CorpScroll:CalculateInternalSize();
end
Controls.OfficeToggle:RegisterCallback( Mouse.eLClick, OnOfficeToggle );
-------------------------------------------------
-------------------------------------------------
function OnForeignCorpsToggle()
    local bWasHidden = Controls.ForeignCorpsStack:IsHidden();
    Controls.ForeignCorpsStack:SetHide( not bWasHidden );
    if( bWasHidden ) then
        Controls.ForeignCorpsToggle:LocalizeAndSetText("TXT_KEY_EO_FOREIGN_CORPS_DETAILS_COLLAPSE");
    else
        Controls.ForeignCorpsToggle:LocalizeAndSetText("TXT_KEY_EO_FOREIGN_CORPS_DETAILS");
    end
    Controls.CorpStack:CalculateSize();
    Controls.CorpStack:ReprocessAnchoring();
    Controls.CorpScroll:CalculateInternalSize();
end
Controls.ForeignCorpsToggle:RegisterCallback( Mouse.eLClick, OnForeignCorpsToggle );

-- END

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )
    if( not bIsHide ) then
		UpdateCorporations();
		UpdateMonopolies();
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );