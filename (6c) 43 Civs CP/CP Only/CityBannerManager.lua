------------------------------------------------- 
-- CityBannerManager
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
include( "InfoTooltipInclude" );
include( "CityStateStatusHelper" );

local g_TeamIM  = InstanceManager:new( "TeamCityBanner",  "Anchor", Controls.CityBanners );
local g_OtherIM = InstanceManager:new( "OtherCityBanner", "Anchor", Controls.CityBanners  );
local g_SVStrikeIM = InstanceManager:new( "SVRangeStrikeButton", "Anchor", Controls.StrategicViewStrikeButtons );

-- Globals
local Instances = {};
local SVInstances = {};
local WorldPositionOffset = { x = 0, y = 0, z = 35 };

local BlackFog = 0; -- invisible
local GreyFog  = 1; -- once seen
local WhiteFog = 2; -- eyes on

local defaultErrorTextureSheet = "CityBannerProductionImage.dds";
local nullOffset = Vector2( 0, 0 );

-------------------------------------------------
-- Determines whether or not to show the Range Strike Button
-------------------------------------------------
function ShouldShowRangeStrikeButton(city) 
	if city == nil then
		return false;
	end
	
	if (city:GetOwner() ~= Game.GetActivePlayer()) then
		return false;
	end
		
	return city:CanRangeStrikeNow();
end

-------------------------------------------------
-- Show/hide the range strike icon
-------------------------------------------------
function UpdateRangeStrikeIcon(cityBanner)

	local player = Players[cityBanner.playerID];		
	if (player ~= nil) then
		
		local city = player:GetCityByID(cityBanner.cityID);
		local controls = cityBanner.SubControls;
	
		local svStrikeButton = nil;	
		if (SVInstances[cityBanner.playerID] ~= nil) then
			svStrikeButton = SVInstances[cityBanner.playerID][cityBanner.cityID];
		end
		if ShouldShowRangeStrikeButton(city) then
			if (controls.CityRangeStrikeButton ~= nil) then
				controls.CityRangeStrikeButton:SetHide(false);
				controls.CityRangeStrikeAnim:SetHide( false );
			end
			
			if svStrikeButton ~= nil then
				if (svStrikeButton.CityRangeStrikeButton ~= nil) then
					svStrikeButton.CityRangeStrikeButton:SetHide(false);
					svStrikeButton.CityRangeStrikeAnim:SetHide(false);
				end
			end
		else
			if (controls.CityRangeStrikeButton ~= nil) then
				controls.CityRangeStrikeButton:SetHide(true);
				controls.CityRangeStrikeAnim:SetHide( true );
			end
			
			if svStrikeButton ~= nil then
				if (svStrikeButton.CityRangeStrikeButton ~= nil) then
					svStrikeButton.CityRangeStrikeButton:SetHide(true);
					svStrikeButton.CityRangeStrikeAnim:SetHide(true);
				end
			end
		end
	end
end
-------------------------------------------------
-- Updates banner to reflect latest city info.
-------------------------------------------------
function RefreshCityBanner(cityBanner, iActiveTeam, iActivePlayer)
	if ( Instances[ cityBanner.playerID ] == nil or Instances[ cityBanner.playerID ][ cityBanner.cityID ] == nil ) then
	    return;
    end
		
	local strToolTip = "";
	local player = Players[cityBanner.playerID];
	
	local team = Players[cityBanner.playerID]:GetTeam();
	local isActivePlayerCity = (cityBanner.playerID == iActivePlayer);
	local isActiveTeamCity = false;
	if (iActiveTeam == team) then
		isActiveTeamCity = true;
	end	
		
	-- grab city using playerID and cityID
	local city = player:GetCityByID(cityBanner.cityID);
	-- for debugging purposes, we want to be able to create a city banner without a DLL-side city
	--assert(city);
	
	local bHasSpy = false;
	local bHasDiplomat = false;
	local strSpyName = nil;
	local strSpyRank = nil;
	
	if(city ~= nil) then
		local cityX = city:GetX();
		local cityY = city:GetY();
		
		local activePlayer = Players[iActivePlayer]
		local spies = activePlayer:GetEspionageSpies();
		for i,v in ipairs(spies) do
			if(v.CityX == cityX and v.CityY == cityY) then
				if (v.IsDiplomat) then
					bHasDiplomat = true;
				else
					bHasSpy = true;
				end
				strSpyName = Locale.Lookup(v.Name);
				strSpyRank = Locale.Lookup(v.Rank);
			end
		end
	end
	
	local controls = cityBanner.SubControls;
	
	-- Update colors
	local primaryColor, secondaryColor = player:GetPlayerColors();
	if player:IsMinorCiv() then
		primaryColor, secondaryColor = secondaryColor, primaryColor;
	end
	
	local backgroundColor = {x = secondaryColor.x, y = secondaryColor.y, z = secondaryColor.z, w = 0.7};
	
	controls.CityBannerBackground:SetColor(backgroundColor);
	if( isActiveTeamCity )then
		if (controls.CityBannerBGLeftHL ~= nil) then
			controls.CityBannerBGLeftHL:SetColor( backgroundColor );
		end
		if (controls.CityBannerBGRightHL ~= nil) then
			controls.CityBannerBGRightHL:SetColor( backgroundColor );
		end
		if (controls.CityBannerRightBackground ~= nill) then
			controls.CityBannerRightBackground:SetColor( backgroundColor );
		end
		if (controls.CityBannerLeftBackground ~= nil) then
			controls.CityBannerLeftBackground:SetColor( backgroundColor );
		end
	else
		--NOTE: If the active player were to ever change (such as during an auto play) these controls will not exist because
		--		the city banner is of the "active player" type and not the "other player" type.
		--		This fix is merely a fix to the Lua nil value error and not a proper solution to the problem.
		if(controls.RightBackground ~= nil and controls.LeftBackground ~= nil) then
			controls.RightBackground:SetColor( backgroundColor );
			controls.LeftBackground:SetColor( backgroundColor );
	    end
	end
	
	local textColor = {x = primaryColor.x, y = primaryColor.y, z = primaryColor.z, w = 1};
	local textColor200 = {x = primaryColor.x, y = primaryColor.y, z = primaryColor.z, w = 0.7};	
	local textColorShadow = {x = 0, y = 0, z = 0, w = 0.5};
	local textColorSoft = {x = 1, y = 1, z = 1, w = 0.5};
	if(controls.CityProductionName) then
		controls.CityProductionName:SetColor(textColor200, 0);
	end

	controls.CityName:SetColor(textColor, 0);
	controls.CityName:SetColor(textColorShadow, 1);
	controls.CityName:SetColor(textColorSoft, 2);
    
	--print("No city");
    	
	if city ~= nil then
		-- Update name
		local cityName = city:GetNameKey();
		local localizedCityName = Locale.ConvertTextKey(cityName);
		local convertedKey = Locale.ToUpper(localizedCityName);
		
		-- Update capital icon
		local isCapital = city:IsCapital() or Players[city:GetOriginalOwner()]:IsMinorCiv();
		
		if (city:IsCapital() and not player:IsMinorCiv()) then
			convertedKey = "[ICON_CAPITAL]" .. convertedKey;
		end
		
		controls.CityName:SetText(convertedKey);
		
		if (isActivePlayerCity) then
			if (city:IsPuppet() and not player:MayNotAnnex()) then
				strToolTip = Locale.ConvertTextKey("TXT_KEY_CITY_ANNEX_TT");
			else
				strToolTip = Locale.ConvertTextKey("TXT_KEY_CITY_ENTER_CITY_SCREEN");		
			end
		elseif (isActiveTeamCity) then
			strToolTip = Locale.ConvertTextKey("TXT_KEY_CITY_TEAMMATE");
		elseif (Game.IsOption( GameOptionTypes.GAMEOPTION_ALWAYS_WAR )) then
			strToolTip = Locale.ConvertTextKey("TXT_KEY_ALWAYS_AT_WAR_WITH_CITY");
		elseif (player:IsMinorCiv()) then
			local strStatusTT = GetCityStateStatusToolTip(iActivePlayer, cityBanner.playerID, false);
			strToolTip = strToolTip .. strStatusTT;	
			controls.StatusIconBG:SetToolTipString(strStatusTT);
			controls.StatusIcon:SetToolTipString(strStatusTT);
		elseif (not Teams[Game.GetActiveTeam()]:IsHasMet(player:GetTeam())) then
			strToolTip = Locale.ConvertTextKey("TXT_KEY_HAVENT_MET");
		else
			strToolTip = Locale.ConvertTextKey("TXT_KEY_TALK_TO_PLAYER");
		end
		
		if (Teams[Game.GetActiveTeam()]:IsAtWar(player:GetTeam())) then
			strToolTip = strToolTip .. "[NEWLINE]----------------[NEWLINE]"
			strToolTip = strToolTip .. player:GetWarmongerPreviewString(city:GetOwner(), city, Game.GetActivePlayer());
			if (city:GetOriginalOwner() ~= city:GetOwner()) then
				strToolTip = strToolTip .. "[NEWLINE]----------------[NEWLINE]"
				strToolTip = strToolTip .. player:GetLiberationPreviewString(city:GetOwner(), city, Game.GetActivePlayer());
			end
		end
		
		local eReligion = city:GetReligiousMajority();
		
		if (eReligion >= 0) then
			local religion = GameInfo.Religions[eReligion];
			IconHookup( religion.PortraitIndex, 32, religion.IconAtlas, controls.ReligiousIcon );
			IconHookup( religion.PortraitIndex, 32, religion.IconAtlas, controls.ReligiousIconShadow );
		end	
		
		local religionToolTip = "";
		if(GetReligionTooltip) then
			religionToolTip = GetReligionTooltip(city);
		end	
		
		if (religionToolTip ~= "") then
			strToolTip = strToolTip .. "[NEWLINE]----------------[NEWLINE]" .. religionToolTip;
		end
		controls.BannerButton:SetToolTipString(strToolTip);
	
		if (controls.ReligiousIcon ~= nil) then
			controls.ReligiousIcon:SetToolTipString(religionToolTip);
		end		
			
		local bHasReligion = (eReligion >= 0);
		if (controls.ReligiousIcon ~= nil) then
			controls.ReligiousIconContainer:SetHide(not bHasReligion);
		end
		
		DoResizeBanner(controls);

		-- Connected to capital?
		if (isActiveTeamCity) then
			if (not city:IsCapital() and player:IsCapitalConnectedToCity(city) and not city:IsBlockaded()) then
				controls.ConnectedIcon:SetHide(false);
				controls.ConnectedIcon:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CITY_CONNECTED"));
			else
				controls.ConnectedIcon:SetHide(true);
			end
		end
			
		-- Blockaded
		if (city:IsBlockaded()) then
			controls.BlockadedIcon:SetHide(false);
			controls.BlockadedIcon:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CITY_BLOCKADED"));
		else
			controls.BlockadedIcon:SetHide(true);
		end
		
		-- Being Razed
		if (city:IsRazing()) then
			controls.RazingIcon:SetHide(false);
			controls.RazingIcon:SetToolTipString(Locale.ConvertTextKey( "TXT_KEY_CITY_BURNING", tostring(city:GetRazingTurns()) ));
		else
			controls.RazingIcon:SetHide(true);
		end
		
		-- In Resistance
		if (city:IsResistance()) then
			controls.ResistanceIcon:SetHide(false);
			controls.ResistanceIcon:SetToolTipString(Locale.ConvertTextKey( "TXT_KEY_CITY_RESISTANCE", tostring(city:GetResistanceTurns()) ));
		else
			controls.ResistanceIcon:SetHide(true);
		end

		-- Puppet Status
		if (city:IsPuppet()) then
			controls.PuppetIcon:SetHide(false);
			
			if(isActivePlayerCity) then
				controls.PuppetIcon:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CITY_PUPPET"));
			else
				controls.PuppetIcon:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CITY_PUPPET_OTHER"));
			end
		else
			controls.PuppetIcon:SetHide(true);
		end
		
		-- Occupation Status
		if (city:IsOccupied() and not city:IsNoOccupiedUnhappiness()) then
			controls.OccupiedIcon:SetHide(false);
			controls.OccupiedIcon:SetToolTipString(Locale.ConvertTextKey( "TXT_KEY_CITY_OCCUPIED"));
		else
			controls.OccupiedIcon:SetHide(true);
		end
		
		if(bHasSpy) then
			controls.SpyIcon:SetHide(false);
			if (isActivePlayerCity) then
				controls.SpyIcon:LocalizeAndSetToolTip("TXT_KEY_CITY_SPY_YOUR_CITY_TT", strSpyRank, strSpyName, city:GetName(), strSpyRank, strSpyName);
			elseif (player:IsMinorCiv()) then
				controls.SpyIcon:LocalizeAndSetToolTip("TXT_KEY_CITY_SPY_CITY_STATE_TT", strSpyRank, strSpyName, city:GetName(), strSpyRank, strSpyName);			
			else
				controls.SpyIcon:LocalizeAndSetToolTip("TXT_KEY_CITY_SPY_OTHER_CIV_TT", strSpyRank, strSpyName, city:GetName(), strSpyRank, strSpyName, strSpyRank, strSpyName);
			end
		else
			controls.SpyIcon:SetHide(true);
		end

		if(bHasDiplomat) then
			controls.DiplomatIcon:SetHide(false);
			controls.DiplomatIcon:LocalizeAndSetToolTip("TXT_KEY_CITY_DIPLOMAT_OTHER_CIV_TT", strSpyRank, strSpyName, city:GetName(), strSpyRank, strSpyName, strSpyRank, strSpyName);
		else
			controls.DiplomatIcon:SetHide(true);
		end
		
		controls.IconsStack:ReprocessAnchoring();

		-- Update strength
		local cityStrengthStr = math.floor(city:GetStrengthValue() / 100);
		
		local garrisonedUnit = city:GetGarrisonedUnit();
		if garrisonedUnit == nil then
			if isActiveTeamCity then
				controls.GarrisonFrame:SetHide(true);
			end	
		end
		
		controls.CityStrength:SetText(cityStrengthStr);
		
    	if isActiveTeamCity then
			controls.EjectGarrison:SetHide(true);
		end

		UpdateRangeStrikeIcon(cityBanner);
		
		-- Update population
		local cityPopulation = math.floor(city:GetPopulation());
		controls.CityPopulation:SetText(cityPopulation);
		
		-- Update Growth Time
		if(controls.CityGrowth) then
			local cityGrowth = city:GetFoodTurnsLeft();
			
			if (city:IsFoodProduction() or city:FoodDifferenceTimes100() == 0) then
				cityGrowth = "-";
				controls.CityBannerRightBackground:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CITY_STOPPED_GROWING_TT", localizedCityName, cityPopulation));
			elseif city:FoodDifferenceTimes100() < 0 then
				cityGrowth = "[COLOR_WARNING_TEXT]-[ENDCOLOR]";
				controls.CityBannerRightBackground:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CITY_STARVING_TT",localizedCityName ));
			else
				controls.CityBannerRightBackground:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CITY_WILL_GROW_TT", localizedCityName, cityPopulation, cityPopulation+1, cityGrowth));
			end
			
			controls.CityGrowth:SetText(cityGrowth);
		end
		
		-- Update Production Time
		if(controls.BuildGrowth) then
			local buildGrowth = "-";
			
			if (city:IsProduction() and not city:IsProductionProcess()) then
				if (city:GetCurrentProductionDifferenceTimes100(false, false) > 0) then
					buildGrowth = city:GetProductionTurnsLeft();
				end
			end
			
			controls.BuildGrowth:SetText(buildGrowth);

		end
		
		-- Update Growth Meter
		if (controls.GrowthBar) then
			
			local iCurrentFood = city:GetFood();
			local iFoodNeeded = city:GrowthThreshold();
			local iFoodPerTurn = city:FoodDifference();
			local iCurrentFoodPlusThisTurn = iCurrentFood + iFoodPerTurn;
			
			local fGrowthProgressPercent = iCurrentFood / iFoodNeeded;
			local fGrowthProgressPlusThisTurnPercent = iCurrentFoodPlusThisTurn / iFoodNeeded;
			if (fGrowthProgressPlusThisTurnPercent > 1) then
				fGrowthProgressPlusThisTurnPercent = 1
			end
			
			controls.GrowthBar:SetPercent( fGrowthProgressPercent );
			controls.GrowthBarShadow:SetPercent( fGrowthProgressPlusThisTurnPercent );
			
		end
		
		-- Update Production Meter
		if (controls.ProductionBar) then
			
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
			
			controls.ProductionBar:SetPercent( fProductionProgressPercent );
			controls.ProductionBarShadow:SetPercent( fProductionProgressPlusThisTurnPercent );
		end

		-- Update Production Name
		local cityProductionName = city:GetProductionNameKey();
		if cityProductionName == nil or string.len(cityProductionName) == 0 then
			cityProductionName = "TXT_KEY_PRODUCTION_NO_PRODUCTION";
		end
		if (controls.CityProductionName) then
			convertedKey = Locale.ConvertTextKey(cityProductionName);
			controls.CityProductionName:SetText(convertedKey);
			if (controls.CityBannerLeftBackground) then
				if (city:IsProductionProcess()) then
					local cityProductionProcess = city:GetProductionProcess();
					local tooltipString = GetHelpTextForProcess(cityProductionProcess, false);
					controls.CityBannerLeftBackground:SetToolTipString(tooltipString);
				else
					if cityProductionName == "TXT_KEY_PRODUCTION_NO_PRODUCTION" then
						controls.CityBannerLeftBackground:SetToolTipString(Locale.ConvertTextKey( "TXT_KEY_CITY_NOT_PRODUCING", localizedCityName ));
					else
						local productionTurnsLeft = city:GetProductionTurnsLeft();
						local tooltipString;
						if productionTurnsLeft > 99 then
							tooltipString = Locale.ConvertTextKey("TXT_KEY_CITY_CURRENTLY_PRODUCING_99PLUS_TT", localizedCityName, cityProductionName);
						else
							tooltipString = Locale.ConvertTextKey("TXT_KEY_CITY_CURRENTLY_PRODUCING_TT", localizedCityName, cityProductionName, productionTurnsLeft);
						end
						controls.CityBannerLeftBackground:SetToolTipString(tooltipString);
					end
				end	
			end
		end
	
		-- Update Production icon
		if controls.CityBannerProductionImage then
			local unitProduction = city:GetProductionUnit();
			local buildingProduction = city:GetProductionBuilding();
			local projectProduction = city:GetProductionProject();
			local processProduction = city:GetProductionProcess();
			local noProduction = false;

			if unitProduction ~= -1 then
				local portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon(unitProduction, city:GetOwner());			
				if IconHookup( portraitOffset, 45, portraitAtlas, controls.CityBannerProductionImage ) then
					controls.CityBannerProductionImage:SetHide( false );
				else
					controls.CityBannerProductionImage:SetHide( true );
				end
			elseif buildingProduction ~= -1 then
				local thisBuildingInfo = GameInfo.Buildings[buildingProduction];
				if IconHookup( thisBuildingInfo.PortraitIndex, 45, thisBuildingInfo.IconAtlas, controls.CityBannerProductionImage ) then
					controls.CityBannerProductionImage:SetHide( false );
				else
					controls.CityBannerProductionImage:SetHide( true );
				end
			elseif projectProduction ~= -1 then
				local thisProjectInfo = GameInfo.Projects[projectProduction];
				if IconHookup( thisProjectInfo.PortraitIndex, 45, thisProjectInfo.IconAtlas, controls.CityBannerProductionImage ) then
					controls.CityBannerProductionImage:SetHide( false );
				else
					controls.CityBannerProductionImage:SetHide( true );
				end
			elseif processProduction ~= -1 then
				local thisProcessInfo = GameInfo.Processes[processProduction];
				if IconHookup( thisProcessInfo.PortraitIndex, 45, thisProcessInfo.IconAtlas, controls.CityBannerProductionImage ) then
					controls.CityBannerProductionImage:SetHide( false );
				else
					controls.CityBannerProductionImage:SetHide( true );
				end
			else -- really should have an error texture
				controls.CityBannerProductionImage:SetHide(true);
			end
			
			if isActivePlayerCity then
    			controls.CityBannerProductionButton:RegisterCallback( Mouse.eLClick, OnProdClick );
    			controls.CityBannerProductionButton:SetVoids( city:GetID(), nil );
    			controls.BannerButton:SetDisabled( false );
			end
			
		end
		
		-- This is another player's banner instance
		if( controls.MinorIndicator and controls.StatusIcon ) then
		
			controls.StatusIcon:SetColor( textColor );
			local civType = player:GetCivilizationType();
			local civInfo = GameInfo.Civilizations[civType];

			if( player:IsMinorCiv() ) then

				SetUpMinorMeter(iActivePlayer, cityBanner.playerID, controls, textColor );
				
				-- minor trait icon
				controls.StatusIcon:SetTexture( GameInfo.MinorCivTraits[ GameInfo.MinorCivilizations[ player:GetMinorCivType() ].MinorCivTrait ].TraitIcon );
				controls.StatusIcon:SetTextureOffsetVal( 0, 0 );

			else
				IconHookup( civInfo.PortraitIndex, 32, civInfo.AlphaIconAtlas, controls.StatusIcon );
				controls.StatusIcon:SetOffsetX( 0 );
				controls.StatusIconBG:SetHide( true );
            	controls.StatusMeterFrame:SetHide( true );
			end

			local pOriginalOwner = Players[ city:GetOriginalOwner() ];
			if( pOriginalOwner:IsMinorCiv() ) then
			
        		--if( city:IsPuppet() or city:IsOccupied() ) then
        		--if( pOriginalOwner ~= player ) then
			
            	local _, originalColor = pOriginalOwner:GetPlayerColors();
            	originalColor.w = 1;
            	
				civType = pOriginalOwner:GetCivilizationType();
				civInfo = GameInfo.Civilizations[civType];
				IconHookup( civInfo.PortraitIndex, 32, civInfo.AlphaIconAtlas, controls.MinorIndicator );
				
				controls.MinorIndicator:SetColor( originalColor );
				controls.MinorIndicator:SetHide( false );
			--    controls.MinorOccupiedSpacer:SetHide( false );
			--    controls.NameStack:SetOffsetX( -7 );
			else
			    controls.MinorIndicator:SetHide( true );
			 --   controls.MinorOccupiedSpacer:SetHide( true );
			 --   controls.NameStack:SetOffsetX( -3 );
			end
			
			controls.NameStack:CalculateSize();
			controls.NameStack:ReprocessAnchoring();
			
		end	
		
		-- Refresh the damage bar too
		RefreshCityDamage( cityBanner, city:GetDamage(), city:GetMaxHitPoints() );		
	end

	if(controls.NameStack)then
		controls.NameStack:CalculateSize();
		controls.NameStack:ReprocessAnchoring();
	end
	
	controls.IconsStack:CalculateSize();
	controls.IconsStack:ReprocessAnchoring();
end


----------------------------------------------------------------
----------------------------------------------------------------
function SetUpMinorMeter( iMajor, iMinor, controls, minorColor )
    
	controls.StatusMeterFrame:SetHide( false );
	controls.StatusIconBG:SetHide( false );
	controls.StatusIcon:SetOffsetX( 0 );
	controls.StatusIcon:SetColor( {x=1, y=1, z=1, w=1 } );
	
	-- If we're neutral, show the minor's own colors
	if (GetCityStateStatusType(iMajor, iMinor) == "MINOR_FRIENDSHIP_STATUS_NEUTRAL") then
		controls.StatusIcon:SetColor( minorColor );
		controls.StatusIconBG:SetHide( true );
    	controls.StatusIcon:SetOffsetX( -5 );
    end
	
	-- If INF is 0, don't bother showing the meter
	if (Players[iMinor]:GetMinorCivFriendshipWithMajor(iMajor) == 0) then
		controls.StatusMeterFrame:SetHide(true);
	end
	
	UpdateCityStateStatusUI(iMajor, iMinor, controls.PositiveStatusMeter, controls.NegativeStatusMeter, controls.StatusMeterMarker, controls.StatusIconBG);
end


-------------------------------------------------
-- On City Created
-------------------------------------------------
function OnCityCreated( hexPos, playerID, cityID, cultureType, eraType, continent, populationSize, size, fowState )
	local controlTable = {};
	local svStrikeButton = {};
	
	--If the player is on your team, display TeamCityBanner, otherwise display OtherCityBanner.
	local iActivePlayer = Game.GetActivePlayer();
	local iActiveTeam = Players[iActivePlayer]:GetTeam();
	local team = Players[playerID]:GetTeam();
	
	if( Instances[ playerID ] ~= nil and
	    Instances[ playerID ][ cityID ] ~= nil ) then
	    return;
    end
    
	if (SVInstances[playerID] == nil) then
		SVInstances[playerID] = {}
	end
    
    local gridPosX, gridPosY = ToGridFromHex( hexPos.x, hexPos.y );
		
	local isActiveType = false;
	if(iActiveTeam ~= team) then
	    controlTable = g_OtherIM:GetInstance();
	    controlTable.BannerButton:RegisterCallback( Mouse.eLClick, OnBannerClick );
	    controlTable.BannerButton:SetVoid1( gridPosX );
	    controlTable.BannerButton:SetVoid2( gridPosY );
	else
	    controlTable = g_TeamIM:GetInstance();
	    controlTable.BannerButton:RegisterCallback( Mouse.eLClick, OnBannerClick );
	    
	    controlTable.BannerButton:SetVoid1( gridPosX );
	    controlTable.BannerButton:SetVoid2( gridPosY );
	    
		controlTable.EjectGarrison:RegisterCallback( Mouse.eLClick, OnEjectGarrisonClick );		
		controlTable.EjectGarrison:SetVoid1(playerID);
		controlTable.EjectGarrison:SetVoid2(cityID);
		controlTable.CityRangeStrikeButton:RegisterCallback( Mouse.eLClick, OnCityRangeStrikeButtonClick );		
		controlTable.CityRangeStrikeButton:SetVoid1(playerID);
		controlTable.CityRangeStrikeButton:SetVoid2(cityID);
		
		svStrikeButton = g_SVStrikeIM:GetInstance();
		svStrikeButton.Anchor:SetWorldPosition( HexToWorld(hexPos) );
		svStrikeButton.CityRangeStrikeButton:RegisterCallback( Mouse.eLClick, OnCityRangeStrikeButtonClick );		
		svStrikeButton.CityRangeStrikeButton:SetVoid1(playerID);
		svStrikeButton.CityRangeStrikeButton:SetVoid2(cityID);
				
		SVInstances[playerID][cityID] = svStrikeButton;
		isActiveType = true;
	end
	
	local cityBanner = {
		playerID = playerID,
		cityID = cityID,
		IsActiveType = isActiveType,
		SubControls = controlTable,
		Hex = hexPos,
	};
	
	if (Instances[playerID] == nil) then
		Instances[playerID] = {}
	end
	
	Instances[playerID][cityID] = cityBanner;
	
	local HexPos = HexToWorld( hexPos );
	controlTable.Anchor:SetWorldPosition( VecAdd( HexPos, WorldPositionOffset ) );
	
	RefreshCityBanner(cityBanner, iActiveTeam, iActivePlayer);
	
	if fowState == BlackFog then
	    controlTable.Anchor:SetHide( true );
    else
	    controlTable.Anchor:SetHide( false );
	end
	
end
Events.SerialEventCityCreated.Add( OnCityCreated );

-------------------------------------------------------------------------------
-- Check the banner to see if it needs to be rebuilt (active player change)
-------------------------------------------------------------------------------
function CheckCityBannerRebuild( instance, iActiveTeam, iActivePlayer )
		
	local cityTeam = Players[instance.playerID]:GetTeam();		
	
    -- If the city banner was instanced for the active team and now its not or vice versa, rebuild the banner
	local bWantActive = cityTeam == iActiveTeam;	
	if (instance.IsActiveType ~= bWantActive) then
		-- rebuild the banner
		local controlTable = {};
		local gridPosX, gridPosY = ToGridFromHex( instance.Hex.x, instance.Hex.y );
		local worldPos = HexToWorld( instance.Hex );
	
		local bWasHidden = instance.SubControls.Anchor:IsHidden();
		-- print("Rebuilding banner for player: " .. tostring(instance.playerID) .. " city: " .. tostring(instance.cityID) .. " from active = " .. tostring(instance.IsActiveType) .. " to active = " .. tostring(bWantActive));
		
		if (not bWantActive) then
			-- If we don't want the 'active' style, then it must have been active.
			-- Release the old one
			if (instance.SubControls ~= nil) then
				g_TeamIM:ReleaseInstance( instance.SubControls );
			end
			if (SVInstances[instance.playerID] ~= nil) then
				local svInstance = SVInstances[instance.playerID][instance.cityID];
				if svInstance ~= nil then
					g_SVStrikeIM:ReleaseInstance( svInstance );
				end
				SVInstances[instance.playerID][instance.cityID] = nil;
			end
		    -- Create the new, non-active banner
			controlTable = g_OtherIM:GetInstance();
			controlTable.BannerButton:RegisterCallback( Mouse.eLClick, OnBannerClick );
			controlTable.BannerButton:SetVoid1( gridPosX );
			controlTable.BannerButton:SetVoid2( gridPosY );			
		else
			-- Release the old one
			if (instance.SubControls ~= nil) then
				g_OtherIM:ReleaseInstance( instance.SubControls );
			end
			
			-- Create the new active banner
			controlTable = g_TeamIM:GetInstance();
			controlTable.BannerButton:RegisterCallback( Mouse.eLClick, OnBannerClick );
	    
		    controlTable.BannerButton:SetVoid1( gridPosX );
		    controlTable.BannerButton:SetVoid2( gridPosY );
	    
			controlTable.EjectGarrison:RegisterCallback( Mouse.eLClick, OnEjectGarrisonClick );		
			controlTable.EjectGarrison:SetVoid1(instance.playerID);
			controlTable.EjectGarrison:SetVoid2(instance.cityID);
			controlTable.CityRangeStrikeButton:RegisterCallback( Mouse.eLClick, OnCityRangeStrikeButtonClick );		
			controlTable.CityRangeStrikeButton:SetVoid1(instance.playerID);
			controlTable.CityRangeStrikeButton:SetVoid2(instance.cityID);
		
			if (SVInstances[instance.playerID] == nil) then
				SVInstances[instance.playerID] = {}
			end
			
		    local svInstance = SVInstances[instance.playerID][instance.cityID];
		    if svInstance ~= nil then
				g_SVStrikeIM:ReleaseInstance( svInstance );
			end
					
			local svStrikeButton = {};
			svStrikeButton = g_SVStrikeIM:GetInstance();
			svStrikeButton.Anchor:SetWorldPosition( worldPos );
			svStrikeButton.CityRangeStrikeButton:RegisterCallback( Mouse.eLClick, OnCityRangeStrikeButtonClick );		
			svStrikeButton.CityRangeStrikeButton:SetVoid1(instance.playerID);
			svStrikeButton.CityRangeStrikeButton:SetVoid2(instance.cityID);
		
			SVInstances[instance.playerID][instance.cityID] = svStrikeButton;			
		end
		
		controlTable.Anchor:SetWorldPosition( VecAdd( worldPos, WorldPositionOffset ) );
	
		-- Attach
		instance.SubControls = controlTable;
		-- Set the new active type flag
		instance.IsActiveType = bWantActive;
		
		RefreshCityBanner(instance, iActiveTeam, iActivePlayer);

		-- Keep the hidden state	
		controlTable.Anchor:SetHide( bWasHidden );
	end
end

-------------------------------------------------
-- Change the width of the banner so it looks good with the length of the city name
-------------------------------------------------
function DoResizeBanner(BannerInstance)

	-- Just in case
	BannerInstance.NameStack:CalculateSize();
	BannerInstance.NameStack:ReprocessAnchoring();

	local iWidth = BannerInstance.NameStack:GetSizeX();
		
	-- If this control doesn't exist, then we're using the active player banner as opposed to the other player.
	-- NOTE:	There are rare instances when the active player will change (hotseat, autoplay) so just checking
	--			the active player is not good enough.
	if (BannerInstance.CityBannerBaseFrame == nil) then
		iWidth = iWidth + 60;	-- Offset for human player's banners

		BannerInstance.CityBannerBackgroundIcon:SetSizeX(iWidth);
		BannerInstance.CityBannerButtonGlow:SetSizeX(iWidth);
		BannerInstance.CityBannerButtonBase:SetSizeX(iWidth);
		
	else
		iWidth = iWidth + 10;	-- Offset for other player's banners
		BannerInstance.CityBannerBaseFrame:SetSizeX(iWidth);
	end

	BannerInstance.BannerButton:SetSizeX(iWidth);
	BannerInstance.CityBannerBackground:SetSizeX(iWidth);
	BannerInstance.CityBannerBackgroundHL:SetSizeX(iWidth);
	
	BannerInstance.BannerButton:ReprocessAnchoring();
	BannerInstance.NameStack:ReprocessAnchoring();
end


-------------------------------------------------
-- On City Update
-------------------------------------------------
function OnCityUpdate()
	-- Update all cities
	local iActivePlayer = Game.GetActivePlayer();
	local iActiveTeam = Players[iActivePlayer]:GetTeam();
	
	-- TODO: Refresh only visible city banners?
	for i, v in pairs(Instances) do
		for iCities, vCities in pairs(v) do
			RefreshCityBanner(vCities, iActiveTeam, iActivePlayer);
		end
	end
end
Events.SerialEventCityInfoDirty.Add(OnCityUpdate);

-------------------------------------------------
-- On City Destroyed
-------------------------------------------------
function OnCityDestroyed(hexPos, playerID, cityID, newPlayerID)
	
	local playerTable = Instances[ playerID ];
	local banner = playerTable[ cityID ];
	
	local active_team = Players[Game.GetActivePlayer()]:GetTeam();
	local team = Players[playerID]:GetTeam();
	
	if(active_team ~= team) 
	then
	    g_OtherIM:ReleaseInstance( banner.SubControls );
    else
	    g_TeamIM:ReleaseInstance( banner.SubControls );
	    
	    if (SVInstances[playerID] ~= nil) then
			local svInstance = SVInstances[playerID][cityID];
			if svInstance ~= nil then
				g_SVStrikeIM:ReleaseInstance( svInstance );
				SVInstances[playerID][cityID] = nil;
			end
		end
    end
	
	playerTable[cityID] = nil;
	
end
Events.SerialEventCityDestroyed.Add(OnCityDestroyed);
Events.SerialEventCityCaptured.Add(OnCityDestroyed);

-------------------------------------------------
-- Refresh the City Damage bar
-------------------------------------------------
function RefreshCityDamage(instance, iDamage, iMaxDamage)
	
	if instance == nil then
	    return;
    end
	
	local iHealthPercent = 1 - (iDamage / iMaxDamage);

    instance.SubControls.CityBannerHealthBar:SetPercent(iHealthPercent);
    
	---- Health bar color based on amount of damage
	local tBarColor = {};
	
    if iHealthPercent > 0.66 then
        tBarColor.x = 0;
        tBarColor.y = 1;
        tBarColor.z = 0;
        tBarColor.w = 1;
        instance.SubControls.CityBannerHealthBar:SetFGColor( tBarColor );
    elseif iHealthPercent > 0.33 then
        tBarColor.x = 1;
        tBarColor.y = 1;
        tBarColor.z = 0;
        tBarColor.w = 1;
        instance.SubControls.CityBannerHealthBar:SetFGColor( tBarColor );
    else
        tBarColor.x = 1;
        tBarColor.y = 0;
        tBarColor.z = 0;
        tBarColor.w = 1;
        instance.SubControls.CityBannerHealthBar:SetFGColor( tBarColor );
    end
    
    -- Show or hide the Health Bar as necessary
    if (iDamage == 0) then
		--print("Damage is 0, hiding health bar")
		instance.SubControls.CityBannerHealthBarBase:SetHide( true );
		instance.SubControls.CityBannerHealthBar:SetHide( true );
	else
		--print("Damage is NOT 0, showing health bar")
		instance.SubControls.CityBannerHealthBarBase:SetHide( false );
		instance.SubControls.CityBannerHealthBar:SetHide( false );
    end

end


-------------------------------------------------
-- On City Set Damage
-------------------------------------------------
function OnCitySetDamage(iPlayerID, iCityID, iDamage, iPreviousDamage)
	
	local playerTable = Instances[ iPlayerID ];
	local instance = playerTable[ iCityID ];
	local city = Players[iPlayerID]:GetCityByID(iCityID);
	
	if (instance ~= nil) then
		local iMaxHitPoints = 200;
		-- It is possible that the city is gone by now, but the banner still exists as messages are played back
		-- Get the max hit points from the city, but if it is not there get the global max.
		if (city ~= nil) then
			iMaxHitPoints = city:GetMaxHitPoints();
		else
			iMaxHitPoints = GameDefines["MAX_CITY_HIT_POINTS"];
		end
		 
		RefreshCityDamage(instance, iDamage, iMaxHitPoints);
	end
end
Events.SerialEventCitySetDamage.Add(OnCitySetDamage);


-------------------------------------------------
-- On Specific City changed
-------------------------------------------------
function OnSpecificCityInfoDirty(iPlayerID, iCityID, eUpdateType)
	
	if (eUpdateType == CityUpdateTypes.CITY_UPDATE_TYPE_BANNER or
	    eUpdateType == CityUpdateTypes.CITY_UPDATE_TYPE_ENEMY_IN_RANGE or
	    eUpdateType == CityUpdateTypes.CITY_UPDATE_TYPE_GARRISON) then
		
		local playerTable = Instances[ iPlayerID ];
		if playerTable == nil then
			return;
		end
		
		local instance = playerTable[ iCityID ];
		if instance == nil then
			return;
		end
						
		if (eUpdateType == CityUpdateTypes.CITY_UPDATE_TYPE_ENEMY_IN_RANGE) then
			UpdateRangeStrikeIcon(instance);
		else		
			local iActivePlayer = Game.GetActivePlayer();
			local iActiveTeam = Players[iActivePlayer]:GetTeam();
			RefreshCityBanner(instance, iActiveTeam, iActivePlayer);
		end		
	end
	
end
Events.SpecificCityInfoDirty.Add(OnSpecificCityInfoDirty);


------------------------------------------------------------
------------------------------------------------------------
function OnHexFogEvent( hexPos, fowType, bWholeMap )
    if( bWholeMap ) then
        for playerID,playerTable in pairs( Instances ) do
            for cityID,instance in pairs( playerTable ) do
                if( fowType == BlackFog ) then
                    instance.SubControls.Anchor:SetHide( true );
                elseif( fowType == GreyFog ) then
                    instance.SubControls.Anchor:SetHide( false );
                else
                    instance.SubControls.Anchor:SetHide( false );
                end
            end
        end
    else
        local gridPosX, gridPosY = ToGridFromHex( hexPos.x, hexPos.y );
		local plot = Map.GetPlot( gridPosX, gridPosY );
		if plot ~= nil then
			local city = plot:GetPlotCity();
			if city ~= nil then
				local cityID = city:GetID();
				local player = city:GetOwner();
				if player ~= -1 then
					local playerTable = Instances[ player ];
					if playerTable then
						local instance = playerTable[ cityID ];
						if instance then
							if fowType == BlackFog  then
								instance.SubControls.Anchor:SetHide( true );
							else
								local garrisonedUnit = city:GetGarrisonedUnit();
								if garrisonedUnit and not UnitMoving(garrisonedUnit:GetOwner(), garrisonedUnit:GetID()) then
									GarrisonComplete( instance, city );
								end
								instance.SubControls.Anchor:SetHide( false );
							end
						end
					end
				end
			end
		end
	end
end
Events.HexFOWStateChanged.Add( OnHexFogEvent );

-------------------------------------------------
-- On City Range Strike Button Selected
-------------------------------------------------
function OnCityRangeStrikeButtonClick( PlayerID, CityID )
	local player = Players[PlayerID];
	if (player == nil) then
		print("Invalid player");
		return;
	end
	
	local city = player:GetCityByID(CityID);

	if (player:GetID() ~= Game.GetActivePlayer()) then
		print("Not my player!");
		return;
	end

	if (city == nil) then
		print("No city!");
		return;
	end;
	
	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_CITY_RANGE_ATTACK);
	UI.ClearSelectionList();
	UI.SelectCity( city );

	Events.InitCityRangeStrike( PlayerID, CityID );
end


function OnInitCityRangeStrike( PlayerID, CityID )

	local player = Players[PlayerID];
	if (player == nil) then
		print("Invalid player");
		return;
	end
	
	local city = player:GetCityByID(CityID);

	if (player:GetID() ~= Game.GetActivePlayer()) then
		print("Not my player!");
		return;
	end

	if (city == nil) then
		print("No city!");
		return;
	end;

	UI.SelectCity( city );
	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_CITY_RANGE_ATTACK);
end
Events.InitCityRangeStrike.Add( OnInitCityRangeStrike );




------------------------------------------------------------
-- OnEjectGarrisonClick - kick out the city's garrison!
------------------------------------------------------------
function OnEjectGarrisonClick ( PlayerID, CityID )
	
	local player = Players[PlayerID];
	if (player == nil) then
		print("Invalid player");
		return;
	end
	
	local city = player:GetCityByID(CityID);

	if (player:GetID() ~= Game.GetActivePlayer()) then
		print("Not my player!");
		return;
	end

	if (city == nil) then
		print("No city!");
		return;
	end;

	local unit = city:GetGarrisonedUnit();
	if (unit == nil) then
		print("No unit!");
		return;
	end;
	
	UI.SetPlaceUnit(unit);
	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_PLACE_UNIT);
	OnCityUpdate();
	UI.HighlightCanPlacePlots(unit, city:Plot());
end

------------------------------------------------------------
------------------------------------------------------------
function LeaderSelected( ePlayer )

	if not Players[Game.GetActivePlayer()]:IsTurnActive() or Game.IsProcessingMessages() then
		return;
	end

	local player = Players[ePlayer];
    if player:IsHuman() then
        Events.OpenPlayerDealScreenEvent( ePlayer );
    else
        UI.SetRepeatActionPlayer(ePlayer);
        UI.ChangeStartDiploRepeatCount(1);
    	player:DoBeginDiploWithHuman();
	end
end

------------------------------------------------------------
------------------------------------------------------------
function OnBannerClick( x, y )
	local plot = Map.GetPlot( x, y );
	if plot then
		local playerID = plot:GetOwner();
		local player = Players[playerID];
		
		-- Active player city
		if playerID == Game.GetActivePlayer() then
			-- Puppets are special
			if (plot:GetPlotCity():IsPuppet() and not player:MayNotAnnex()) then
				local popupInfo = {
						Type = ButtonPopupTypes.BUTTONPOPUP_ANNEX_CITY,
						Data1 = plot:GetPlotCity():GetID(),
						Data2 = -1,
						Data3 = -1,
						Option1 = false,
						Option2 = false;
					}
				Events.SerialEventGameMessagePopup(popupInfo);
			else
				UI.DoSelectCityAtPlot( plot );
			end
			
		-- Other player, which has been met
		elseif (Teams[Game.GetActiveTeam()]:IsHasMet(player:GetTeam())) then
			
			if player:IsMinorCiv() then
				UI.DoSelectCityAtPlot( plot );
			else
				LeaderSelected( playerID );
			end
		end
	end
end

------------------------------------------------------------
------------------------------------------------------------
function GarrisonComplete( cityBanner, pCity )
	local active_team = Players[Game.GetActivePlayer()]:GetTeam();
	local team = Players[cityBanner.playerID]:GetTeam();
	
	local controls = cityBanner.SubControls;
	if active_team == team then
		controls.GarrisonFrame:SetHide(false);
	end	
end

-------------------------------------------------
-- On Unit Garrison
-------------------------------------------------
function OnUnitGarrison( playerID, unitID, bGarrisoned )
	if bGarrisoned and not UnitMoving(playerID, unitID) then
		local player = Players[ playerID ];
		local unit = player:GetUnitByID( unitID );
		local cityBanners = Instances[ playerID ];
		if unit ~= nil and cityBanners ~= nil then
			local city = unit:GetGarrisonedCity();
			if city ~= nil then
				local banner = cityBanners[city:GetID()];
				if banner ~= nil then
					GarrisonComplete( banner, city );
				end
			end
		end
	end
end
Events.UnitGarrison.Add( OnUnitGarrison );

-------------------------------------------------
-- On Unit Move Queue Changed
-------------------------------------------------
function OnUnitMoveQueueChanged( playerID, unitID, bRemainingMoves )
	if not bRemainingMoves then
		local player = Players[ playerID ];
		local unit = player:GetUnitByID( unitID );
		local cityBanners = Instances[ playerID ];
		if unit ~= nil and cityBanners ~= nil and unit:IsGarrisoned() then
			local city = unit:GetGarrisonedCity();
			if city ~= nil then
				local banner = cityBanners[city:GetID()];
				if banner ~= nil then
					GarrisonComplete( banner, city );
				end
			end
		end
	end
end
Events.UnitMoveQueueChanged.Add( OnUnitMoveQueueChanged );

-------------------------------------------------
-------------------------------------------------
function OnProdClick( cityID, prodName )
	local playerID = Game.GetActivePlayer();
	local activePlayer = Players[playerID]
	local city = activePlayer:GetCityByID(cityID);
	if city and not city:IsPuppet() then
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
end

------------------------------------------------------------
------------------------------------------------------------
function OnInterfaceModeChanged(oldInterfaceMode, newInterfaceMode)
	local disableBanners = newInterfaceMode ~= InterfaceModeTypes.INTERFACEMODE_SELECTION;
	for iPlayer, playerCityBanners in pairs(Instances) do
		for iCity, cityBanner in pairs(playerCityBanners) do
			cityBanner.SubControls.BannerButton:SetDisabled(disableBanners);
			cityBanner.SubControls.BannerButton:EnableToolTip(not disableBanners);
		end
	end
end
Events.InterfaceModeChanged.Add(OnInterfaceModeChanged);

------------------------------------------------------------
------------------------------------------------------------
function OnStrategicViewStateChanged(bStrategicView, bCityBanners)
	local bShowBanners = bCityBanners or not bStrategicView;
	Controls.CityBanners:SetHide(not bShowBanners);
	Controls.StrategicViewStrikeButtons:SetHide(bShowBanners);
end
Events.StrategicViewStateChanged.Add(OnStrategicViewStateChanged);

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnCityBannerActivePlayerChanged( iActivePlayer, iPrevActivePlayer )

	local iActiveTeam = Players[iActivePlayer]:GetTeam();
	-- Update all cities
	for i, v in pairs(Instances) do
		for iCities, vCities in pairs(v) do
			CheckCityBannerRebuild(vCities, iActiveTeam, iActivePlayer);
		end
	end
end
Events.GameplaySetActivePlayer.Add(OnCityBannerActivePlayerChanged);

------------------------------------------------------------
------------------------------------------------------------
function HideGarrisonRing(iX, iY, bHide)

	local pPlot = Map.GetPlot( iX, iY );
	if pPlot ~= nil then
		local pCity = pPlot:GetPlotCity();
		if pCity ~= nil then
			local cityID = pCity:GetID();
			local playerID = pCity:GetOwner();
			if playerID ~= -1 then
				-- Only the active team has a Garrison ring			
				local eActiveTeam = Players[Game.GetActivePlayer()]:GetTeam();
				local eCityTeam = Players[playerID]:GetTeam();
	
				if eActiveTeam == eCityTeam then			
					local playerTable = Instances[ playerID ];
					if playerTable then
						local pBannerInstance = playerTable[ cityID ];
						if pBannerInstance ~= nil then					
							if (bHide) then
								pBannerInstance.SubControls.GarrisonFrame:SetHide(true);
							else
								-- Only show it if we really need to
								local garrisonedUnit = pCity:GetGarrisonedUnit();
								if garrisonedUnit ~= nil then
									pBannerInstance.SubControls.GarrisonFrame:SetHide(false);
								end							
							end
						end
					end
				end
			end
		end
	end
end
------------------------------------------------------------
------------------------------------------------------------
function OnCombatBegin( attackerPlayerID,
                        attackerUnitID,
                        attackerUnitDamage,
                        attackerFinalUnitDamage,
                        attackerMaxHitPoints,
                        defenderPlayerID,
                        defenderUnitID,
                        defenderUnitDamage,
                        defenderFinalUnitDamage,
                        defenderMaxHitPoints, 
                        bContinuation,
                        attackerX,
                        attackerY,
                        defenderX,
                        defenderY )
    print( "CityBanner CombatBegin" );                        
				
	HideGarrisonRing(attackerX, attackerY, true);
	HideGarrisonRing(defenderX, defenderY, true);
end
Events.RunCombatSim.Add( OnCombatBegin );


------------------------------------------------------------
------------------------------------------------------------
function OnCombatEnd( attackerPlayerID,
                      attackerUnitID,
                      attackerUnitDamage,
                      attackerFinalUnitDamage,
                      attackerMaxHitPoints,
                      defenderPlayerOD,
                      defenderUnitID,
                      defenderUnitDamage,
                      defenderFinalUnitDamage,
                      defenderMaxHitPoints,
                      attackerX,
                      attackerY,
					  defenderX,
                      defenderY )
                         
    print( "CityBanner CombatEnd" );                        
    
	HideGarrisonRing(attackerX, attackerY, false);
	HideGarrisonRing(defenderX, defenderY, false);
end
Events.EndCombatSim.Add( OnCombatEnd );

--------------------------------------------------------------
-- The active player's turn has begin, make sure their range strike icons are correct
--------------------------------------------------------------
function OnActivePlayerTurnStart()

	local iPlayerID = Game.GetActivePlayer();
	local playerTable = Instances[ iPlayerID ];
	if playerTable == nil then
		return;
	end
	
    local pPlayer = Players[iPlayerID];
    if (pPlayer ~= nil) then
        if (pPlayer:IsAlive()) then
            for cityIndex = 0, pPlayer:GetNumCities() - 1, 1
            do
    			local pCity = pPlayer:GetCityByID( cityIndex );
    			if ( pCity ~= nil ) then
					local pBannerInstance = playerTable[ pCity:GetID() ];
					if (pBannerInstance ~= nil) then
						UpdateRangeStrikeIcon(pBannerInstance);
					end									
    			end
            end
        end
    end	
	
end
Events.ActivePlayerTurnStart.Add( OnActivePlayerTurnStart );

------------------------------------------------------------
-- scan for all cities when we are loaded
-- this keeps the banners from disappearing on hotload
------------------------------------------------------------

if( ContextPtr:IsHotLoad() ) then
    local i = 0;
    local player = Players[i];
    while player ~= nil 
    do
        if( player:IsAlive() )
        then
            for cityIndex = 0, player:GetNumCities() - 1, 1
            do
    			local city = player:GetCityByID( cityIndex );
    			if( city ~= nil )
    			then
    				OnCityCreated( ToHexFromGrid( Vector2( city:GetX(), city:GetY() ) ), player:GetID(), city:GetID() );
    			end
            end
        end

        i = i + 1;
        player = Players[i];
    end
end
	
