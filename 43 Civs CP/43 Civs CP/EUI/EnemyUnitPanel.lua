print("This is the modded EnemyUnitPanel from CBP")
-------------------------------------------------
-- Enemy Unit Panel Screen 
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );

local g_MyCombatDataIM = InstanceManager:new( "UsCombatInfo", "Text", Controls.MyCombatResultsStack );
local g_TheirCombatDataIM = InstanceManager:new( "ThemCombatInfo", "Text", Controls.TheirCombatResultsStack );

local g_NumButtons = 12;
local g_lastUnitID = -1;		-- Used to determine if a different pUnit has been selected.

local maxUnitHitPoints = GameDefines["MAX_HIT_POINTS"];

local defaultErrorTextureSheet = "TechAtlasSmall.dds";
local nullOffset = Vector2( 0, 0 );

local g_iPortraitSize = Controls.UnitPortrait:GetSize().x;

local g_bWorldMouseOver = true;
local g_bShowPanel = false;

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

function SetName(name)
	
	name = Locale.ToUpper(name);

    local iNameLength = Locale.Length(name);
    if (iNameLength < 18) then
	    Controls.UnitName:SetText(name);
	    
	    Controls.UnitName:SetHide(false);
	    Controls.LongUnitName:SetHide(true);
	    Controls.ReallyLongUnitName:SetHide(true);
	    
    elseif (iNameLength < 23) then
	    Controls.LongUnitName:SetText(name);
	    
	    Controls.UnitName:SetHide(true);
	    Controls.LongUnitName:SetHide(false);
	    Controls.ReallyLongUnitName:SetHide(true);
	    
    else
	    Controls.ReallyLongUnitName:SetText(name);
	    
	    Controls.UnitName:SetHide(true);
	    Controls.LongUnitName:SetHide(true);
	    Controls.ReallyLongUnitName:SetHide(false);
    end
end
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
function UpdateCityPortrait( pCity )

	if pCity == nil then
		return;
	end

	SetName(pCity:GetName());
	local playerID = pCity:GetOwner();
	local pPlayer = Players[playerID];
    local thisCivType = PreGame.GetCivilization( playerID );
    local thisCiv = GameInfo.Civilizations[thisCivType];

	--print("thisCiv.AlphaIconAtlas:"..tostring(thisCiv.AlphaIconAtlas))

	local textureOffset, textureAtlas = IconLookup( thisCiv.PortraitIndex, 32, thisCiv.AlphaIconAtlas );
	Controls.UnitIcon:SetTexture( textureAtlas );
	Controls.UnitIconShadow:SetTexture( textureAtlas );
	Controls.UnitIcon:SetTextureOffset( textureOffset );
	Controls.UnitIconShadow:SetTextureOffset( textureOffset );

    local iconColor, flagColor = pPlayer:GetPlayerColors();
	if pPlayer:IsMinorCiv() then
		flagColor, iconColor = iconColor, flagColor;
	end
	Controls.UnitBackColor:SetColor( flagColor );
	Controls.UnitIcon:SetColor( iconColor );

	IconHookup( 0, g_iPortraitSize, "ENEMY_CITY_ATLAS", Controls.UnitPortrait );				
end


--------------------------------------------------------------------------------
-- Refresh Unit portrait and name
--------------------------------------------------------------------------------
function UpdateUnitPortrait( pUnit )

	if pUnit == nil then
		return;
	end

	local name = pUnit:GetName();
	SetName(name);
		
	local flagOffset, flagAtlas = UI.GetUnitFlagIcon(pUnit);

	local textureOffset, textureSheet = IconLookup( flagOffset, 32, flagAtlas );				
	Controls.UnitIcon:SetTexture( textureSheet );
	Controls.UnitIconShadow:SetTexture( textureSheet );
	Controls.UnitIcon:SetTextureOffset( textureOffset );
	Controls.UnitIconShadow:SetTextureOffset( textureOffset );

	local pPlayer = Players[pUnit:GetOwner()];
    local iconColor, flagColor = pPlayer:GetPlayerColors();
	if pPlayer:IsMinorCiv() then
		flagColor, iconColor = iconColor, flagColor;
	end
	Controls.UnitBackColor:SetColor( flagColor );
	Controls.UnitIcon:SetColor( iconColor );

	local portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon(pUnit);
	textureOffset, textureSheet = IconLookup( portraitOffset, g_iPortraitSize, portraitAtlas );				
	if textureOffset == nil then
		textureSheet = defaultErrorTextureSheet;
		textureOffset = nullOffset;
	end				
	Controls.UnitPortrait:SetTexture(textureSheet);
	Controls.UnitPortrait:SetTextureOffset(textureOffset);
end


--------------------------------------------------------------------------------
-- Refresh Unit promotions
--------------------------------------------------------------------------------
function UpdateUnitPromotions(pUnit)
	local UnitPromotionKey = "UnitPromotion";
	
	--Clear Unit Promotions
	local i = 1;
	while(Controls[UnitPromotionKey..i] ~= nil) do
		local promotionIcon = Controls[UnitPromotionKey..i];
		promotionIcon:SetHide(true);
	
		i = i + 1;
	end
	
	if pUnit then
		--For each avail promotion, display the icon
		for unitPromotion in GameInfo.UnitPromotions() do
			local unitPromotionID = unitPromotion.ID;
			if(pUnit:IsHasPromotion(unitPromotionID)) then
				
				-- Get next available promotion button
				local idx = 1;
				local promotionIcon;
				repeat
					promotionIcon = Controls[UnitPromotionKey..idx];
					idx = idx + 1;
					
				until(promotionIcon == nil or promotionIcon:IsHidden() == true);
				
				if promotionIcon ~= nil then
					IconHookup( unitPromotion.PortraitIndex, 32, unitPromotion.IconAtlas, promotionIcon );				
					promotionIcon:SetHide(false);
				end
			end
		end
	end
end


--------------------------------------------------------------------------------
-- Refresh City stats
--------------------------------------------------------------------------------
function UpdateCityStats(pCity)
	
	-- Strength
	local strength = math.floor(pCity:GetStrengthValue() / 100);
	
	strength = strength .. " [ICON_STRENGTH]";
	Controls.UnitStrengthBox:SetHide(false);
	Controls.UnitStatStrength:SetText(strength);
	
	Controls.UnitMovementBox:SetHide(true);
	Controls.UnitRangedAttackBox:SetHide(true);	
	
end


--------------------------------------------------------------------------------
-- Refresh Unit stats
--------------------------------------------------------------------------------
function UpdateUnitStats(pUnit)
	
	-- Movement
	local move_denominator = GameDefines["MOVE_DENOMINATOR"];
	local max_moves = pUnit:MaxMoves() / move_denominator;
	local szMoveStr = max_moves .. " [ICON_MOVES]";
	
	Controls.UnitStatMovement:SetText(szMoveStr);
	Controls.UnitMovementBox:SetHide(false);
	
	-- Strength
    local strength = 0;
    if(pUnit:GetDomainType() == DomainTypes.DOMAIN_AIR) then
        strength = pUnit:GetBaseRangedCombatStrength();
    else
        strength = pUnit:GetBaseCombatStrength();
    end
	if(strength > 0) then
		strength = strength .. " [ICON_STRENGTH]";
		Controls.UnitStrengthBox:SetHide(false);
		Controls.UnitStatStrength:SetText(strength);
	else
		Controls.UnitStrengthBox:SetHide(true);
	end		
	
	-- Ranged Strength
	local iRangedStrength = 0;
	if(pUnit:GetDomainType() ~= DomainTypes.DOMAIN_AIR) then
		iRangedStrength = pUnit:GetBaseRangedCombatStrength();
	else
		iRangedStrength = 0;
	end
	if(iRangedStrength > 0) then
		local szRangedStrength = iRangedStrength .. " [ICON_RANGE_STRENGTH]";
		Controls.UnitRangedAttackBox:SetHide(false);
		Controls.UnitStatRangedAttack:SetText(szRangedStrength);
	else
		Controls.UnitRangedAttackBox:SetHide(true);
	end		
	
end


--------------------------------------------------------------------------------
-- Format our text please!
--------------------------------------------------------------------------------
function GetFormattedText(strLocalizedText, iValue, bForMe, bPercent, strOptionalColor)
	
	local strTextToReturn = "";
	local strNumberPart = Locale.ToNumber(iValue, "#.##");
	
	if (bPercent) then
		strNumberPart = strNumberPart .. "%";
		
		if (bForMe) then
			if (iValue > 0) then
				strNumberPart = "[COLOR_POSITIVE_TEXT]+" .. strNumberPart .. "[ENDCOLOR]";
			elseif (iValue < 0) then
				strNumberPart = "[COLOR_NEGATIVE_TEXT]" .. strNumberPart .. "[ENDCOLOR]";
			end
		else
			if (iValue < 0) then
				strNumberPart = "[COLOR_POSITIVE_TEXT]" .. strNumberPart .. "[ENDCOLOR]";
			elseif (iValue > 0) then
				strNumberPart = "[COLOR_NEGATIVE_TEXT]+" .. strNumberPart .. "[ENDCOLOR]";
			end
		end
		
		-- Bullet for my side
		if (bForMe) then
			strNumberPart = strNumberPart .. "[]";
		-- Bullet for their side
		else
			strNumberPart = "[]" .. strNumberPart;
		end
	end
	
	if (strOptionalColor ~= nil) then
		strNumberPart = strOptionalColor .. strNumberPart .. "[ENDCOLOR]";
	end
	
	-- Formatting for my side
	if (bForMe) then
		strTextToReturn = " :  " .. strNumberPart;
	-- Formatting for their side
	else
		strTextToReturn = strNumberPart .. "  : ";
	end
	
	return strTextToReturn;
	
end


--------------------------------------------------------------------------------
-- Refresh Combat Odds
--------------------------------------------------------------------------------
function UpdateCombatOddsUnitVsCity(pMyUnit, pCity)
	
	--print("Updating city combat odds");
	
	g_MyCombatDataIM:ResetInstances();
	g_TheirCombatDataIM:ResetInstances();
	
	if (pMyUnit ~= nil) then
		
		local iMyPlayer = pMyUnit:GetOwner();
		local iTheirPlayer = pCity:GetOwner();
		local pMyPlayer = Players[iMyPlayer];
		local pTheirPlayer = Players[iTheirPlayer];
		
		local iMyStrength = 0;
		local iTheirStrength = 0;
		local bRanged = false;
		local iNumVisibleAAUnits = 0;
		local bInterceptPossible = false;
		
		local pFromPlot = pMyUnit:GetPlot();
		local pToPlot = pCity:Plot();
		
		--JFD begins
		--Find actual attacking plot (pathfinding will fail for ranged units)
		for _, v in pairs(pMyUnit:GeneratePath(pToPlot, 3)) do
			local pPlot = Map.GetPlot(v.X,v.Y)
			if pPlot ~= pToPlot then 
				pFromPlot = pPlot
			end
		end
		local hexID = ToHexFromGrid( Vector2( pFromPlot:GetX(), pFromPlot:GetY()) );
		Events.SerialEventHexHighlight( hexID, true, Vector4( 0.7, 0, 0, 1 ), "ValidFireTargetBorder");
		
		local hexID = ToHexFromGrid( Vector2( pToPlot:GetX(), pToPlot:GetY()) );
		Events.SerialEventHexHighlight( hexID, true, Vector4( 0.7, 0, 0, 1 ), "ValidFireTargetBorder");
		--JFD ends
		
		-- Ranged Unit
		if (pMyUnit:IsRangedSupportFire() == false and pMyUnit:GetBaseRangedCombatStrength() > 0) then
			iMyStrength = pMyUnit:GetMaxRangedCombatStrength(nil, pCity, true, true);
			bRanged = true;
			
		-- Melee Unit
		else
			iMyStrength = pMyUnit:GetMaxAttackStrength(pFromPlot, pToPlot, nil);
		end
		
		iTheirStrength = pCity:GetStrengthValue(false,pMyUnit:IgnoreBuildingDefense());
		
		if (iMyStrength > 0) then
			
			local pPlot = pCity:Plot();
			
			-- Start with logic of combat estimation
			local iMyDamageInflicted = 0;
			local iTheirDamageInflicted = 0;
			local iTheirFireSupportCombatDamage = 0;
			
			-- Ranged Strike
			if (bRanged) then
				iMyDamageInflicted = pMyUnit:GetRangeCombatDamage(nil, pCity, false);
				
				if (pPlot ~= nil and pCity ~= nil and pMyUnit ~= nil and pMyUnit:GetDomainType() == DomainTypes.DOMAIN_AIR) then
					iTheirDamageInflicted = pCity:GetAirStrikeDefenseDamage(pMyUnit, false);	
					iNumVisibleAAUnits = pMyUnit:GetInterceptorCount(pPlot, nil, true, true);		
					bInterceptPossible = true;	
				end
				
			-- Normal Melee Combat
			else								
				local pFireSupportUnit = pMyUnit:GetFireSupportUnit(pCity:GetOwner(), pPlot:GetX(), pPlot:GetY());
				if (pFireSupportUnit ~= nil) then
					iTheirFireSupportCombatDamage = pFireSupportUnit:GetRangeCombatDamage(pMyUnit, nil, false);
				end
				
				iMyDamageInflicted = pMyUnit:GetCombatDamage(iMyStrength, iTheirStrength, pMyUnit:GetDamage() + iTheirFireSupportCombatDamage, false, false, true, pCity);
				iTheirDamageInflicted = pMyUnit:GetCombatDamage(iTheirStrength, iMyStrength, pCity:GetDamage(), false, true, false, NULL);
				iTheirDamageInflicted = iTheirDamageInflicted + iTheirFireSupportCombatDamage;
				
			end
			
			-- City's max HP
			local maxCityHitPoints = pCity:GetMaxHitPoints();
			if (iMyDamageInflicted > maxCityHitPoints) then
				iMyDamageInflicted = maxCityHitPoints;
			end
			-- Unit's max HP
			local maxUnitHitPoints = pMyUnit:GetMaxHitPoints();
			if (iTheirDamageInflicted > maxUnitHitPoints) then
				iTheirDamageInflicted = maxUnitHitPoints;
			end
			
			local bTheirCityLoss = false;
			local bMyUnitLoss = false;
			-- Will their City be captured in combat?
			if (pCity:GetDamage() + iMyDamageInflicted >= maxCityHitPoints) then
				bCityLoss = true;
			end
			-- Will my Unit die in combat?
			if (pMyUnit:GetDamage() + iTheirDamageInflicted >= maxUnitHitPoints) then
				bMyUnitLoss = true;
			end
			
			-- now do the health bars
			
			DoUpdateHealthBars(maxUnitHitPoints, maxCityHitPoints, pMyUnit:GetDamage(), pCity:GetDamage(), iMyDamageInflicted, iTheirDamageInflicted)
			
			-- Now do text stuff
			
			local controlTable;
			local strText;
			
			Controls.RangedAttackIndicator:SetHide(true);
			Controls.SafeAttackIndicator:SetHide(true);
			Controls.RiskyAttackIndicator:SetHide(true);
			Controls.TotalVictoryIndicator:SetHide(true);
			Controls.MajorVictoryIndicator:SetHide(true);
			Controls.SmallVictoryIndicator:SetHide(true);
			Controls.SmallDefeatIndicator:SetHide(true);
			Controls.MajorDefeatIndicator:SetHide(true);
			Controls.TotalDefeatIndicator:SetHide(true);
			Controls.StalemateIndicator:SetHide(true);
			
			local strMyDamageTextColor = "White_Black";
			local strTheirDamageTextColor = "White_Black";
			-- Ranged attack
			if (bRanged) then
				Controls.RangedAttackIndicator:SetHide(false);
				Controls.RangedAttackButtonLabel:SetText(Locale.ToUpper(Locale.ConvertTextKey("TXT_KEY_INTERFACEMODE_RANGE_ATTACK")));
			-- Our unit is weak
			elseif (pMyUnit:GetDamage() > (maxUnitHitPoints / 2) and iTheirDamageInflicted > 0) then
				Controls.RiskyAttackIndicator:SetHide(false);
			-- They are doing at least as much damage to us as we're doing to them
			elseif (iTheirDamageInflicted >= iMyDamageInflicted) then
				Controls.RiskyAttackIndicator:SetHide(false);
			-- Safe (?) attack
			else
				Controls.SafeAttackIndicator:SetHide(false);
			end
				
			-- Ranged fire support
			if (iTheirFireSupportCombatDamage > 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_SUPPORT_DMG" );
				controlTable.Value:SetText( GetFormattedText( "", iTheirFireSupportCombatDamage, true, false ) );

				-- Also add an entry in their stack, so that the gaps match up
				controlTable = g_TheirCombatDataIM:GetInstance();
			end
			
			-- My Damage
			Controls.MyDamageValue:SetText("[COLOR_GREEN]" .. iMyDamageInflicted .. "[ENDCOLOR]");
			-- My Strength
			Controls.MyStrengthValue:SetText( Locale.ToNumber(iMyStrength / 100, "#.##"));
			
			-- Their Damage
			Controls.TheirDamageValue:SetText("[COLOR_RED]" .. iTheirDamageInflicted .. "[ENDCOLOR]");
			
			--if (bRanged) then
				--Controls.TheirDamageValue:SetHide(true);
				--Controls.TheirDamage:SetHide(true);
			--else
				--Controls.TheirDamageValue:SetHide(false);
				--Controls.TheirDamage:SetHide(false);
			--end
			
			-- Their Strength
			Controls.TheirStrengthValue:SetText( Locale.ToNumber(iTheirStrength / 100, "#.##") );
			
			-- City Attack bonus
			local iModifier = pMyUnit:CityAttackModifier();
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				
				local textKey;
				if(iModifier >= 0) then
					textKey = "TXT_KEY_EUPANEL_ATTACK_CITIES";
				else
					textKey = "TXT_KEY_EUPANEL_ATTACK_CITIES_PENALTY";
				end
				
				controlTable.Text:LocalizeAndSetText( textKey );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end

			-- ExtraCombatPercent (CBP)
			iModifier = pMyUnit:GetExtraCombatPercent();
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_EXTRA_PERCENT" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			
			iModifier = pMyPlayer:GetFoundedReligionEnemyCityCombatMod(pPlot);
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ENEMY_CITY_BELIEF_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			
			-- Sapper unit modifier
			if (pMyUnit:GetSapperAreaEffectBonus(pCity) ~= 0) then
				iModifier = pMyUnit:GetSapperAreaEffectBonus(pCity);
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_CITY_SAPPED" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			-- Conquest of the World
			iModifier = pMyPlayer:GetTraitConquestOfTheWorldCityAttackMod(pPlot);
			if (iModifier ~= 0 and pMyPlayer:IsGoldenAge()) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_BONUS_CONQUEST_OF_WORLD" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			-- Blockaded
			if (pCity:IsBlockadedTest()) then
				iModifier = (GameDefines["SAPPED_CITY_ATTACK_MODIFIER"] / 2);
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_CITY_BLOCKADED" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			iModifier = pMyUnit:GetAllianceCSStrength();
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ATTACK_CS_ALLIANCE_STRENGTH" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end

			-- COMMUNITY (Bushido)
			if (pMyPlayer:GetWoundedUnitDamageMod() ~= 0) then
				iModifier = pMyUnit:GetDamage() / 5;
			    if (iModifier ~= 0) then
				   controlTable = g_MyCombatDataIM:GetInstance();
				   controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BUSHIDO" );
				   controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			end
			if (pMyUnit:IsStrongerDamaged()) then
				iModifier = pMyUnit:GetDamage() / 3;
			    if (iModifier ~= 0) then
				   controlTable = g_MyCombatDataIM:GetInstance();
				   controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BUSHIDO" );
				   controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			end

			-- Civ Trait Bonus
			iModifier = pMyUnit:GetMultiAttackBonusCity(pCity);
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_BONUS_MULTI_ATTACK_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			-- END
						
			-- Civ Trait Bonus
			iModifier = pMyPlayer:GetTraitGoldenAgeCombatModifier();
			if (iModifier ~= 0 and pMyPlayer:IsGoldenAge()) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_BONUS_GOLDEN_AGE" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end

			iModifier = pMyPlayer:GetTraitCityStateCombatModifier();
			if (iModifier ~= 0 and pTheirPlayer:IsMinorCiv()) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_BONUS_CITY_STATE" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
				
			if (not bRanged) then
			
				-- Crossing a River
				if (not pMyUnit:IsRiverCrossingNoPenalty()) then
					if (pMyUnit:GetPlot():IsRiverCrossingToPlot(pToPlot)) then
						iModifier = GameDefines["RIVER_ATTACK_MODIFIER"];

						if (iModifier ~= 0) then
							controlTable = g_MyCombatDataIM:GetInstance();
							controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ATTACK_OVER_RIVER" );
							controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
						end
					end
				end
				

				-- Amphibious landing
				if (not pMyUnit:IsAmphib()) then
					if (not pToPlot:IsWater() and pMyUnit:GetPlot():IsWater() and pMyUnit:GetDomainType() == DomainTypes.DOMAIN_LAND) then
						iModifier = GameDefines["AMPHIB_ATTACK_MODIFIER"];

						if (iModifier ~= 0) then
							controlTable = g_MyCombatDataIM:GetInstance();
							controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_AMPHIBIOUS_ATTACK" );
							controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
						end
					end
				end
				
			else
				iModifier = pMyUnit:GetRangedAttackModifier();
				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_RANGED_ATTACK_MODIFIER" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			end

			-- Attack Modifier
			iModifier = pMyUnit:GetAttackModifier();
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ATTACK_MOD_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			--NearbyPromtoion Unit that Gives a Combat bonus
			if (pMyUnit:GetGiveCombatModToUnit() ~= 0) then
				iModifier = pMyUnit:GetGiveCombatModToUnit();
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_NEARBYPROMOTION_COMBAT_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			--NearbyPromtoion Unit that gets a bonus near cities?
			if (pMyUnit:GetNearbyCityBonusCombatMod() ~= 0) then
				iModifier = pMyUnit:GetNearbyCityBonusCombatMod();
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_NEARBYPROMOTION_CITY_COMBAT_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			-- Great General bonus
			if (pMyUnit:IsNearGreatGeneral()) then
				iModifier = pMyPlayer:GetGreatGeneralCombatBonus() + pMyUnit:GetGreatGeneralAuraBonus();
				iModifier = iModifier + pMyPlayer:GetTraitGreatGeneralExtraBonus();
				controlTable = g_MyCombatDataIM:GetInstance();
				if (pMyUnit:GetDomainType() == DomainTypes.DOMAIN_LAND) then
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_GG_NEAR" );
				else
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_GA_NEAR" );
				end
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				
				-- Ignores Great General penalty
				if (pMyUnit:IsIgnoreGreatGeneralBenefit()) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText("TXT_KEY_EUPANEL_IGG");
					controlTable.Value:SetText(GetFormattedText(strText, -iModifier, true, true));
				end
			end
			
			-- Great General stacking bonus
			if (pMyUnit:GetGreatGeneralCombatModifier() ~= 0 and pMyUnit:IsStackedGreatGeneral()) then
				iModifier = pMyUnit:GetGreatGeneralCombatModifier();
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_GG_STACKED" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			
			-- Reverse Great General modifier
			if (pMyUnit:GetReverseGreatGeneralModifier() ~= 0) then
				iModifier = pMyUnit:GetReverseGreatGeneralModifier();
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_REVERSE_GG_NEAR" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			
			-- Nearby improvement modifier
			if (pMyUnit:GetNearbyImprovementModifier(pFromPlot) ~= 0) then
				iModifier = pMyUnit:GetNearbyImprovementModifier(pFromPlot);
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_IMPROVEMENT_NEAR" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			-- Nearby UnitClass modifier
			if (pMyUnit:GetNearbyUnitClassModifierFromUnitClass(pFromPlot) ~= 0) then
				iModifier = pMyUnit:GetNearbyUnitClassModifierFromUnitClass(pFromPlot);
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_UNITCLASS_NEAR" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			-- Empire Unhappy
			iModifier = pMyUnit:GetUnhappinessCombatPenalty();
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				
				if(pMyPlayer:IsEmpireVeryUnhappy()) then
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_EMPIRE_VERY_UNHAPPY_PENALTY" );
				else
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_EMPIRE_UNHAPPY_PENALTY" );
				
				end
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end

			-- Lack Strategic Resources
			iModifier = pMyUnit:GetStrategicResourceCombatPenalty();
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_STRATEGIC_RESOURCE" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			
			-- Adjacent Modifier
			iModifier = pMyUnit:GetAdjacentModifier();
			if (iModifier ~= 0) then
				local bCombatUnit = true;
				if (pMyUnit:IsFriendlyUnitAdjacent(bCombatUnit)) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ADJACENT_FRIEND_UNIT_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			end

-- CBP
			-- PerAdjacentUnitCombatModifier
			iModifier = pMyUnit:PerAdjacentUnitCombatModifier() + pMyUnit:PerAdjacentUnitCombatAttackMod();
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				--local unitClassType = Locale.ConvertTextKey(GameInfo.UnitClasses[pTheirUnit:GetUnitClassType()].Description);
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BONUS_PER_ADJACENT_UNIT_COMBAT" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
-- END
			
			-- Policy Attack bonus
			local iTurns = pMyPlayer:GetAttackBonusTurns();
			if (iTurns > 0) then
				iModifier = GameDefines["POLICY_ATTACK_BONUS_MOD"];
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_POLICY_ATTACK_BONUS", iTurns );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end

			---------------------------
			-- AIR INTERCEPT PREVIEW --
			---------------------------
			if (bInterceptPossible) then
				controlTable = g_TheirCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText("TXT_KEY_EUPANEL_AIR_INTERCEPT_WARNING1");
				controlTable.Value:SetText("");
				controlTable = g_TheirCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText("TXT_KEY_EUPANEL_AIR_INTERCEPT_WARNING2");
				controlTable.Value:SetText("");
			end
			if (iNumVisibleAAUnits > 0) then
				controlTable = g_TheirCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText("TXT_KEY_EUPANEL_VISIBLE_AA_UNITS", iNumVisibleAAUnits);
				controlTable.Value:SetText("");
			end
		end
	end
	
	
    Controls.MyCombatResultsStack:CalculateSize();
    Controls.TheirCombatResultsStack:CalculateSize();
    
    local sizeX = Controls.DetailsGrid:GetSizeX();
    Controls.DetailsGrid:DoAutoSize();
    Controls.DetailsGrid:SetSizeX( sizeX );
    Controls.DetailsSeperator:SetSizeY( Controls.DetailsGrid:GetSizeY() );
    Controls.DetailsGrid:ReprocessAnchoring();
end

function UpdateCombatOddsUnitVsUnit(pMyUnit, pTheirUnit)

	g_MyCombatDataIM:ResetInstances();
	g_TheirCombatDataIM:ResetInstances();
	
	if (pMyUnit ~= nil) then
		
		local iMyPlayer = pMyUnit:GetOwner();
		local iTheirPlayer = pTheirUnit:GetOwner();
		local pMyPlayer = Players[iMyPlayer];
		local pTheirPlayer = Players[iTheirPlayer];
		
		local iMyStrength = 0;
		local iTheirStrength = 0;
		local bRanged = false;
		local iNumVisibleAAUnits = 0;
		local bInterceptPossible = false;
		
		local pFromPlot = pMyUnit:GetPlot();
		local pToPlot = pTheirUnit:GetPlot();
		
		--JFD begins
		--Find actual attacking plot (pathfinding will fail for ranged units) 
		for _, v in pairs(pMyUnit:GeneratePath(pToPlot, 3)) do
			local pPlot = Map.GetPlot(v.X,v.Y)
			if pPlot ~= pToPlot then 
				pFromPlot = pPlot
			end
		end
		local hexID = ToHexFromGrid( Vector2( pFromPlot:GetX(), pFromPlot:GetY()) );
		Events.SerialEventHexHighlight( hexID, true, Vector4( 0.7, 0, 0, 1 ), "ValidFireTargetBorder");
		
		local hexID = ToHexFromGrid( Vector2( pToPlot:GetX(), pToPlot:GetY()) );
		Events.SerialEventHexHighlight( hexID, true, Vector4( 0.7, 0, 0, 1 ), "ValidFireTargetBorder");
		--JFD ends
		
		-- Ranged Unit
		if (pMyUnit:GetBaseRangedCombatStrength() > 0) then
			iMyStrength = pMyUnit:GetMaxRangedCombatStrength(pTheirUnit, nil, true, true);
			bRanged = true;
				
		-- Melee Unit
		else
			iMyStrength = pMyUnit:GetMaxAttackStrength(pFromPlot, pToPlot, pTheirUnit);
		end
		
		if (iMyStrength > 0) then
			
			-- Start with logic of combat estimation
			local iMyDamageInflicted = 0;
			local iTheirDamageInflicted = 0;
			local iTheirFireSupportCombatDamage = 0;
			
			-- Ranged Strike
			if (bRanged) then
				
				iMyDamageInflicted = pMyUnit:GetRangeCombatDamage(pTheirUnit, nil, false);
				
				if (pTheirUnit:IsEmbarked()) then
					iTheirStrength = pTheirUnit:GetEmbarkedUnitDefense();
				else
					iTheirStrength = pTheirUnit:GetMaxRangedCombatStrength(pMyUnit, nil, false, true);
				end
				
				if (iTheirStrength == 0 or pTheirUnit:GetDomainType() == DomainTypes.DOMAIN_SEA or pTheirUnit:IsRangedSupportFire()) then
					iTheirStrength = pTheirUnit:GetMaxDefenseStrength(pToPlot, pMyUnit, pFromPlot, true);
				end
				
				if (pMyUnit:GetDomainType() == DomainTypes.DOMAIN_AIR) then
					if (pMyUnit:GetUnitAIType() ~= 30) then
						-- regular air attack
						iTheirDamageInflicted = pTheirUnit:GetAirStrikeDefenseDamage(pMyUnit, false);
					else
						-- suicide missile attack
						iTheirDamageInflicted = pMyUnit:GetCurrHitPoints();
					end
					iNumVisibleAAUnits = pMyUnit:GetInterceptorCount(pToPlot, pTheirUnit, true, true);		
					bInterceptPossible = true;
				end
				
			-- Normal Melee Combat
			else
				
				iTheirStrength = pTheirUnit:GetMaxDefenseStrength(pToPlot, pMyUnit, pFromPlot);
				
				local pFireSupportUnit = pMyUnit:GetFireSupportUnit(pTheirUnit:GetOwner(), pToPlot:GetX(), pToPlot:GetY());
				if (pFireSupportUnit ~= nil) then
					iTheirFireSupportCombatDamage = pFireSupportUnit:GetRangeCombatDamage(pMyUnit, nil, false);
				end
				
				iMyDamageInflicted = pMyUnit:GetCombatDamage(iMyStrength, iTheirStrength, pMyUnit:GetDamage() + iTheirFireSupportCombatDamage, false, false, false, NULL);
				iTheirDamageInflicted = pTheirUnit:GetCombatDamage(iTheirStrength, iMyStrength, pTheirUnit:GetDamage(), false, false, false, NULL);
				iTheirDamageInflicted = iTheirDamageInflicted + iTheirFireSupportCombatDamage;
				
			end
			
			-- Don't give numbers greater than a Unit's max HP
			if (iMyDamageInflicted > maxUnitHitPoints) then
				iMyDamageInflicted = maxUnitHitPoints;
			end
			if (iTheirDamageInflicted > maxUnitHitPoints) then
				iTheirDamageInflicted = maxUnitHitPoints;
			end
			
			-- now do the health bars
			
			DoUpdateHealthBars(maxUnitHitPoints, maxUnitHitPoints, pMyUnit:GetDamage(), pTheirUnit:GetDamage(), iMyDamageInflicted, iTheirDamageInflicted)

			-- Now do text stuff
			
			local controlTable;
			local strText;
			
			--local strMyDamageTextColor = "White_Black";
			--local strTheirDamageTextColor = "White_Black";
			--if (iMyDamageInflicted > iTheirDamageInflicted) then
				--strMyDamageTextColor = "Green_Black";
				--strTheirDamageTextColor = "Red_Black";
			--elseif (iMyDamageInflicted < iTheirDamageInflicted) then
				--strMyDamageTextColor = "Red_Black";
				--strTheirDamageTextColor = "Green_Black";
			--end
			--
			---- Damage I will suffer
			--controlTable = g_MyCombatDataIM:GetInstance();
			--if (bRanged) then
				--strText = ".";
			--else
				--strText = "DAMAGE Taken: ~" .. iTheirDamageInflicted .. "/" .. maxUnitHitPoints;
				--controlTable.MyCombatInfoInstanceText:SetColorByName(strMyDamageTextColor);
			--end
			--controlTable.MyCombatInfoInstanceText:SetText(strText);
			--
			---- Damage they will suffer
			--controlTable = g_TheirCombatDataIM:GetInstance();
			--strText = "DAMAGE Taken: ~" .. iMyDamageInflicted .. "/" .. maxUnitHitPoints;
			
			
			Controls.RangedAttackIndicator:SetHide(true);
			Controls.SafeAttackIndicator:SetHide(true);
			Controls.RiskyAttackIndicator:SetHide(true);
			Controls.TotalVictoryIndicator:SetHide(true);
			Controls.MajorVictoryIndicator:SetHide(true);
			Controls.SmallVictoryIndicator:SetHide(true);
			Controls.SmallDefeatIndicator:SetHide(true);
			Controls.MajorDefeatIndicator:SetHide(true);
			Controls.TotalDefeatIndicator:SetHide(true);
			Controls.StalemateIndicator:SetHide(true);
			
			--print("iMyDamageInflicted: " .. tostring(iMyDamageInflicted));
			--print("iTheirDamageInflicted: " .. tostring(iTheirDamageInflicted));
			
			local strMyDamageTextColor = "White_Black";
			local strTheirDamageTextColor = "White_Black";
			
			local eCombatPrediction = Game.GetCombatPrediction(pMyUnit, pTheirUnit);
			if (eCombatPrediction == CombatPredictionTypes.COMBAT_PREDICTION_RANGED) then
				Controls.RangedAttackIndicator:SetHide(false);
				Controls.RangedAttackButtonLabel:SetText(Locale.ToUpper(Locale.ConvertTextKey("TXT_KEY_INTERFACEMODE_RANGE_ATTACK")));
			elseif (eCombatPrediction == CombatPredictionTypes.COMBAT_PREDICTION_STALEMATE) then
				Controls.StalemateIndicator:SetHide(false);
			elseif (eCombatPrediction == CombatPredictionTypes.COMBAT_PREDICTION_TOTAL_DEFEAT) then
				Controls.TotalDefeatIndicator:SetHide(false);
			elseif (eCombatPrediction == CombatPredictionTypes.COMBAT_PREDICTION_TOTAL_VICTORY) then
				Controls.TotalVictoryIndicator:SetHide(false);
			elseif (eCombatPrediction == CombatPredictionTypes.COMBAT_PREDICTION_MAJOR_VICTORY) then
				Controls.MajorVictoryIndicator:SetHide(false);
			elseif (eCombatPrediction == CombatPredictionTypes.COMBAT_PREDICTION_SMALL_VICTORY) then
				Controls.SmallVictoryIndicator:SetHide(false);
			elseif (eCombatPrediction == CombatPredictionTypes.COMBAT_PREDICTION_MAJOR_DEFEAT) then
				Controls.MajorDefeatIndicator:SetHide(false);
			elseif (eCombatPrediction == CombatPredictionTypes.COMBAT_PREDICTION_SMALL_DEFEAT) then
				Controls.SmallDefeatIndicator:SetHide(false);
			else
				Controls.StalemateIndicator:SetHide(false);
			end

			-- Ranged fire support
			if (iTheirFireSupportCombatDamage > 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText("TXT_KEY_EUPANEL_SUPPORT_DMG");
				controlTable.Value:SetText( GetFormattedText(strText, iTheirFireSupportCombatDamage, true, false) );

				-- Also add an entry in their stack, so that the gaps match up
				controlTable = g_TheirCombatDataIM:GetInstance();
			end
			
			-- Combat Strength
			--if (not bRanged) then
				--controlTable = g_MyCombatDataIM:GetInstance();
				--strText = "STRENGTH: " .. iMyStrength / 100;
				--
				--controlTable = g_TheirCombatDataIM:GetInstance();
				--strText = "STRENGTH: " .. iTheirStrength / 100;
			--end
			
			-- My Damage
			--controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_UNIT_APPROXIMATE_DAMAGE" );
			--controlTable.Value:SetText( GetFormattedText(strText, iTheirDamageInflicted, true, false, "[COLOR_RED]");
			--Controls.MyDamage:SetText(strText);
			Controls.MyDamageValue:SetText("[COLOR_GREEN]" .. iMyDamageInflicted .. "[ENDCOLOR]");
			
			-- My Strength
			--controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_UNIT_STRENGTH" ) .. " [ICON_STRENGTH]";
			--controlTable.Value:SetText( GetFormattedText(strText, iMyStrength / 100, true, false, "[COLOR_CYAN]");
			--Controls.MyStrength:SetText(strText);
			Controls.MyStrengthValue:SetText( Locale.ToNumber(iMyStrength / 100, "#.##"));
			
			----------------------------------------------------------------------------
			-- BONUSES MY UNIT GETS
			----------------------------------------------------------------------------

			-------------------------
			-- Movement Immunity --
			-------------------------
			local movementRules = pMyUnit:GetMovementRules(pTheirUnit);
			if(movementRules ~= "") then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(movementRules);
				controlTable.Value:SetText("");
			end
			movementRules = pMyUnit:GetZOCStatus();
			if(movementRules ~= "") then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(movementRules);
				controlTable.Value:SetText("");
			end

			local iModifier;
			
			if (not bRanged) then
				
				-- Crossing a River
				if (not pMyUnit:IsRiverCrossingNoPenalty()) then
					if (pMyUnit:GetPlot():IsRiverCrossingToPlot(pToPlot)) then
						iModifier = GameDefines["RIVER_ATTACK_MODIFIER"];

						if (iModifier ~= 0) then
							controlTable = g_MyCombatDataIM:GetInstance();
							controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ATTACK_OVER_RIVER" );
							controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
						end
					end
				end
				
				-- Amphibious landing
				if (not pMyUnit:IsAmphib()) then
					if (not pToPlot:IsWater() and pMyUnit:GetPlot():IsWater() and pMyUnit:GetDomainType() == DomainTypes.DOMAIN_LAND) then
						iModifier = GameDefines["AMPHIB_ATTACK_MODIFIER"];

						if (iModifier ~= 0) then
							controlTable = g_MyCombatDataIM:GetInstance();
							controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_AMPHIBIOUS_ATTACK" );
							controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
						end
					end
				end
				
			end
			-- NearbyPromotion Unit Bonus
			if (pMyUnit:GetGiveCombatModToUnit() ~= 0) then
				iModifier = pMyUnit:GetGiveCombatModToUnit();
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_NEARBYPROMOTION_COMBAT_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			--NearbyPromtoion Unit that gets a bonus near cities?
			if (pMyUnit:GetNearbyCityBonusCombatMod() ~= 0) then
				iModifier = pMyUnit:GetNearbyCityBonusCombatMod();
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_NEARBYPROMOTION_CITY_COMBAT_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			-- Great General bonus
			if (pMyUnit:IsNearGreatGeneral()) then
				iModifier = pMyPlayer:GetGreatGeneralCombatBonus() + pMyUnit:GetGreatGeneralAuraBonus();
				iModifier = iModifier + pMyPlayer:GetTraitGreatGeneralExtraBonus();
				controlTable = g_MyCombatDataIM:GetInstance();
				if (pMyUnit:GetDomainType() == DomainTypes.DOMAIN_LAND) then
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_GG_NEAR" );
				else
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_GA_NEAR" );
				end
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				
				-- Ignores Great General penalty
				if (pMyUnit:IsIgnoreGreatGeneralBenefit()) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText("TXT_KEY_EUPANEL_IGG");
					controlTable.Value:SetText(GetFormattedText(strText, -iModifier, true, true));
				end
			end
			
			-- Great General stacked bonus
			if (pMyUnit:GetGreatGeneralCombatModifier() ~= 0 and pMyUnit:IsStackedGreatGeneral()) then
				iModifier = pMyUnit:GetGreatGeneralCombatModifier();
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_GG_STACKED" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
				
			-- Reverse Great General modifier
			if (pMyUnit:GetReverseGreatGeneralModifier() ~= 0) then
				iModifier = pMyUnit:GetReverseGreatGeneralModifier();
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_REVERSE_GG_NEAR" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			
			-- Nearby improvement modifier
			if (pMyUnit:GetNearbyImprovementModifier(pFromPlot) ~= 0) then
				iModifier = pMyUnit:GetNearbyImprovementModifier(pFromPlot);
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_IMPROVEMENT_NEAR" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			-- Nearby UnitClass modifier
			if (pMyUnit:GetNearbyUnitClassModifierFromUnitClass(pFromPlot) ~= 0) then
				iModifier = pMyUnit:GetNearbyUnitClassModifierFromUnitClass(pFromPlot);
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_UNITCLASS_NEAR" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			
			-- Policy Attack bonus
			local iTurns = pMyPlayer:GetAttackBonusTurns();
			if (iTurns > 0) then
				iModifier = GameDefines["POLICY_ATTACK_BONUS_MOD"];
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_POLICY_ATTACK_BONUS", iTurns );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			
			-- Flanking bonus
			if (not bRanged) then
				iModifier = pFromPlot:GetEffectiveFlankingBonus(pMyUnit,pTheirUnit,pToPlot);
				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_FLANKING_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			end
			
			-- Extra Combat Percent
			iModifier = pMyUnit:GetExtraCombatPercent();
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_EXTRA_PERCENT" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end

			-- COMMUNITY (Bushido)
			if (pMyPlayer:GetWoundedUnitDamageMod() ~= 0) then
			   iModifier =  pMyUnit:GetDamage() / 5;
			    if (iModifier ~= 0) then
				   controlTable = g_MyCombatDataIM:GetInstance();
				   controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BUSHIDO" );
				   controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			end
			if (pMyUnit:IsStrongerDamaged()) then
			   iModifier = pMyUnit:GetDamage() / 3;
			    if (iModifier ~= 0) then
				   controlTable = g_MyCombatDataIM:GetInstance();
				   controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BUSHIDO" );
				   controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			end

			if (not bRanged) then
				iModifier = pMyUnit:GetExtraWithdrawal();
			    if (iModifier ~= 0) then
				   controlTable = g_MyCombatDataIM:GetInstance();
				   controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_WITHDRAW_CHANCE" );
				   controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end		
			end

			-- CBP (Monopoly)
			iModifier = pMyUnit:GetMonopolyAttackBonus();
			if(iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_MONOPOLY_POWER_ATTACK" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end

			-- COMMUNITY (Resistance)
			iModifier = pMyUnit:GetResistancePower(pTheirUnit);
			if(iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_RESISTANCE_POWER" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
				

			-- Bonus for fighting in one's lands
			if (pToPlot:IsFriendlyTerritory(c)) then
				
				-- General combat mod
				iModifier = pMyUnit:GetFriendlyLandsModifier();
				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_FIGHT_AT_HOME_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
				
				-- Attack mod
				iModifier = pMyUnit:GetFriendlyLandsAttackModifier();
				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ATTACK_IN_FRIEND_LANDS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
				
				iModifier = pMyPlayer:GetFoundedReligionFriendlyCityCombatMod(pToPlot);
				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_FRIENDLY_CITY_BELIEF_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
-- COMMUNITY PATCH CHANGE
				iModifier = pMyUnit:GetCombatVersusOtherReligionOwnLands(pTheirUnit);
				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_FRIENDLY_CITY_BELIEF_BONUS_CBP" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
--END
			end

-- COMMUNITY PATCH CHANGE	
		
			-- CombatBonusVsHigherPop
			iModifier = pMyPlayer:GetCombatBonusVsHigherPop();
			if (iModifier ~= 0 and pTheirUnit:IsHigherPopThan(pMyUnit)) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_TRAIT_LOW_POP_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			iModifier = pMyUnit:GetAllianceCSStrength();
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ATTACK_CS_ALLIANCE_STRENGTH" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
--END		
			
			-- CombatBonusVsHigherTech
			if (pToPlot:GetOwner() == iMyPlayer) then
				iModifier = pMyPlayer:GetCombatBonusVsHigherTech();

				if (iModifier ~= 0 and pTheirUnit:IsHigherTechThan(pMyUnit:GetUnitType())) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_TRAIT_LOW_TECH_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			end
					
			-- CombatBonusVsLargerCiv
			iModifier = pMyPlayer:GetCombatBonusVsLargerCiv();
			if (iModifier ~= 0 and pTheirUnit:IsLargerCivThan(pMyUnit)) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_TRAIT_SMALL_SIZE_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
					
			-- CapitalDefenseModifier
			iModifier = pMyUnit:CapitalDefenseModifier();
			if (iModifier > 0) then
				
				-- Compute distance to capital
				local pCapital = pMyPlayer:GetCapitalCity();
					
				if (pCapital ~= nil) then

					local plotDistance = Map.PlotDistance(pCapital:GetX(), pCapital:GetY(), pMyUnit:GetX(), pMyUnit:GetY());
					iModifier = iModifier + (plotDistance * pMyUnit:CapitalDefenseFalloff());

					if (iModifier > 0) then
						controlTable = g_MyCombatDataIM:GetInstance();
						controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_CAPITAL_DEFENSE_BONUS" );
						controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
					end
				end
			end
			
			-- Bonus for fighting outside one's lands
			if (not pToPlot:IsFriendlyTerritory(iMyPlayer)) then
				
				-- General combat mod
				iModifier = pMyUnit:GetOutsideFriendlyLandsModifier();
				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_OUTSIDE_HOME_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
				
				iModifier = pMyPlayer:GetFoundedReligionEnemyCityCombatMod(pToPlot);
				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ENEMY_CITY_BELIEF_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			end

-- COMMUNITY PATCH CHANGE
			if (pToPlot:IsFriendlyTerritory(iTheirPlayer)) then
				iModifier = pMyUnit:GetCombatVersusOtherReligionTheirLands(pTheirUnit);
				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ENEMY_CITY_BELIEF_BONUS_CBP" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			end
--END
			-- Empire Unhappy
			iModifier = pMyUnit:GetUnhappinessCombatPenalty();
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				if(pMyPlayer:IsEmpireVeryUnhappy()) then
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_EMPIRE_VERY_UNHAPPY_PENALTY" );
				else
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_EMPIRE_UNHAPPY_PENALTY" );
				end
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end

			-- Lack Strategic Resources
			iModifier = pMyUnit:GetStrategicResourceCombatPenalty();
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_STRATEGIC_RESOURCE" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
			
			-- Adjacent Modifier
			iModifier = pMyUnit:GetAdjacentModifier();
			if (iModifier ~= 0) then
				local bCombatUnit = true;
				if (pMyUnit:IsFriendlyUnitAdjacent(bCombatUnit)) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ADJACENT_FRIEND_UNIT_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			end
			
			-- Attack Modifier
			iModifier = pMyUnit:GetAttackModifier();
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ATTACK_MOD_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end

			-- UnitClassModifier
			iModifier = pMyUnit:GetUnitClassModifier(pTheirUnit:GetUnitClassType());
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				local unitClassType = Locale.ConvertTextKey(GameInfo.UnitClasses[pTheirUnit:GetUnitClassType()].Description);
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BONUS_VS_CLASS", unitClassType );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end

			-- UnitClassAttackModifier
			iModifier = pMyUnit:UnitClassAttackModifier(pTheirUnit:GetUnitClassType());
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				local unitClassType = Locale.ConvertTextKey(GameInfo.UnitClasses[pTheirUnit:GetUnitClassType()].Description);
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BONUS_VS_CLASS" , unitClassType );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end

			-- UnitCombatModifier
			if (pTheirUnit:GetUnitCombatType() ~= -1) then
				iModifier = pMyUnit:UnitCombatModifier(pTheirUnit:GetUnitCombatType());
-- CBP
				if(pTheirUnit:IsMounted()) then
					iModifier = (iModifier + pMyUnit:UnitCombatModifier(GameInfo.UnitCombatInfos["UNITCOMBAT_MOUNTED"].ID));
				end
-- END

				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					local unitClassType = Locale.ConvertTextKey(GameInfo.UnitCombatInfos[pTheirUnit:GetUnitCombatType()].Description);
-- CBP
					if(pTheirUnit:IsMounted()) then
						controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BONUS_VS_CLASS_CBP", unitClassType );
						controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
					else
-- END
						controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BONUS_VS_CLASS", unitClassType );
						controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
-- CBP
					end
-- END
				end
			end

-- CBP
			-- PerAdjacentUnitCombatModifier
			iModifier = pMyUnit:PerAdjacentUnitCombatModifier() + pMyUnit:PerAdjacentUnitCombatAttackMod();
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				--local unitClassType = Locale.ConvertTextKey(GameInfo.UnitClasses[pTheirUnit:GetUnitClassType()].Description);
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BONUS_PER_ADJACENT_UNIT_COMBAT" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
-- END
	
			-- DomainModifier
			iModifier = pMyUnit:DomainModifier(pTheirUnit:GetDomainType());
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BONUS_VS_DOMAIN" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );

				--CvBaseInfo kDomainInfo;
				--Database::SingleResult kResult;
				--DB.SelectAt(kResult, "Domains", pTheirUnit:getDomainType());
				--kDomainInfo.CacheResult(kResult);

				--strString.append(GetLocalizedText("TXT_KEY_COMBAT_PLOT_MOD_VS_TYPE", iModifier, kDomainInfo.GetDescription()));
			end
			
			-- attackFortifiedMod
			if (pTheirUnit:GetFortifyTurns() > 0) then
				iModifier = pMyUnit:AttackFortifiedModifier();

				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BONUS_VS_FORT_UNITS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			end
			
			-- AttackWoundedMod
			if (pTheirUnit:GetDamage() > 0) then
				iModifier = pMyUnit:AttackWoundedModifier();

				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BONUS_VS_WOUND_UNITS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			else
				iModifier = pMyUnit:AttackFullyHealedModifier();

				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BONUS_VS_FULLY_HEALED_UNITS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			end

			if (pTheirUnit:GetDamage() < (pTheirUnit:GetMaxHitPoints() / 2)) then
				iModifier = pMyUnit:AttackAbove50Modifier();

				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BONUS_VS_MORE_50_HP_UNITS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			else
				iModifier = pMyUnit:AttackBelow50Modifier();

				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BONUS_VS_LESS_50_HP_UNITS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			end

			-- HillsAttackModifier
			if (pToPlot:IsHills()) then
				iModifier = pMyUnit:HillsAttackModifier();

				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_HILL_ATTACK_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			end
			
			if (pToPlot:IsOpenGround()) then
			
				-- OpenAttackModifier
				iModifier = pMyUnit:OpenAttackModifier();

				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_OPEN_TERRAIN_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
				
				-- OpenRangedAttackModifier
				iModifier = pMyUnit:OpenRangedAttackModifier();

				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_OPEN_TERRAIN_RANGE_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			end
			
			if(bRanged) then
				iModifier = pMyUnit:GetRangedAttackModifier();
				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_RANGED_ATTACK_MODIFIER" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			end
			
			if (pToPlot:IsRoughGround()) then
			
				-- RoughAttackModifier
				iModifier = pMyUnit:RoughAttackModifier();

				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ROUGH_TERRAIN_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			
				-- RoughRangedAttackModifier
				iModifier = pMyUnit:RoughRangedAttackModifier();

				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ROUGH_TERRAIN_RANGED_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			end
			
			if (pToPlot:GetFeatureType() ~= -1) then
			
				-- FeatureAttackModifier
				iModifier = pMyUnit:FeatureAttackModifier(pToPlot:GetFeatureType());
				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					local featureTypeBonus = Locale.ConvertTextKey(GameInfo.Features[pToPlot:GetFeatureType()].Description);
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ATTACK_INTO_BONUS", featureTypeBonus );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			else
			
				-- TerrainAttackModifier		
				iModifier = pMyUnit:TerrainAttackModifier(pToPlot:GetTerrainType());
				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					local terrainTypeBonus = Locale.ConvertTextKey( GameInfo.Terrains[pToPlot:GetTerrainType()].Description );
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ATTACK_INTO_BONUS", terrainTypeBonus  );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
				
				if (pToPlot:IsHills()) then
					iModifier = pMyUnit:TerrainAttackModifier(GameInfo.Terrains["TERRAIN_HILL"].ID);
					if (iModifier ~= 0) then
						controlTable = g_MyCombatDataIM:GetInstance();
						local terrainTypeBonus = Locale.ConvertTextKey( GameInfo.Terrains["TERRAIN_HILL"].Description );
						controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ATTACK_INTO_BONUS", terrainTypeBonus  );
						controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
					end
				end
			end
			
			-- BarbarianBonuses
			if (pTheirUnit:IsBarbarian()) then
				--iModifier = GameInfo.HandicapInfos[Game:GetHandicapType()].BarbarianBonus;
				
				iModifier = DB_HandicapInfos[Game:GetHandicapType()].BarbarianBonus;
				iModifier = iModifier + Players[pMyUnit:GetOwner()]:GetBarbarianCombatBonus();

				iModifier = iModifier + pMyUnit:BarbarianCombatBonus();

				if (iModifier ~= 0) then
					controlTable = g_MyCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_VS_BARBARIANS_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
				end
			end

			-- Civ Trait Bonus
			iModifier = pMyUnit:GetMultiAttackBonus(pTheirUnit);
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_BONUS_MULTI_ATTACK_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end

			-- Civ Trait Bonus
			iModifier = pMyPlayer:GetTraitGoldenAgeCombatModifier();
			if (iModifier ~= 0 and pMyPlayer:IsGoldenAge()) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_BONUS_GOLDEN_AGE" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end

			iModifier = pMyPlayer:GetTraitCityStateCombatModifier();
			if (iModifier ~= 0 and pTheirPlayer:IsMinorCiv()) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_BONUS_CITY_STATE" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end

			----------------------------------------------------------------------------
			-- BONUSES THEIR UNIT GETS
			----------------------------------------------------------------------------
			
			-- Their Damage
			--controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_UNIT_APPROXIMATE_DAMAGE" );
			--controlTable.Value:SetText( GetFormattedText(strText, iMyDamageInflicted, false, false, "[COLOR_RED]") );
			--Controls.TheirDamage:SetText(strText);
			Controls.TheirDamageValue:SetText("[COLOR_RED]" .. iTheirDamageInflicted .. "[ENDCOLOR]");
			
			-- Their Strength
			--strText =  "[ICON_STRENGTH] " .. Locale.ConvertTextKey( "TXT_KEY_EUPANEL_UNIT_STRENGTH" );
			--controlTable.Value:SetText( GetFormattedText(strText, iTheirStrength / 100, false, false, "[COLOR_CYAN]") );
			--Controls.TheirStrength:SetText(strText);
			Controls.TheirStrengthValue:SetText( Locale.ToNumber(iTheirStrength / 100, "#.##"));
			
			--print("iTheirStrength: " .. iTheirStrength);
			
			-- Show or hide based on whether this unit can fight back
			--if (not pTheirUnit:IsCombatUnit() or bRanged) then
				--Controls.TheirDamageValue:SetHide(true);
				--Controls.TheirDamage:SetHide(true);
			--else
				--Controls.TheirDamageValue:SetHide(false);
				--Controls.TheirDamage:SetHide(false);
			--end

			--------------------------
			-- AIR INTERCEPT PREVIEW --
			--------------------------
			if (bInterceptPossible) then
				controlTable = g_TheirCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText("TXT_KEY_EUPANEL_AIR_INTERCEPT_WARNING1");
				controlTable.Value:SetText("");
				controlTable = g_TheirCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText("TXT_KEY_EUPANEL_AIR_INTERCEPT_WARNING2");
				controlTable.Value:SetText("");
			end
			if (iNumVisibleAAUnits > 0) then
				controlTable = g_TheirCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText("TXT_KEY_EUPANEL_VISIBLE_AA_UNITS", iNumVisibleAAUnits);
				controlTable.Value:SetText("");
			end
			
			-------------------------
			-- Movement Immunity --
			-------------------------
			local movementRules = pTheirUnit:GetMovementRules(pMyUnit);
			if(movementRules ~= "") then
				controlTable = g_TheirCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(movementRules);
				controlTable.Value:SetText("");
			end
			movementRules = pTheirUnit:GetZOCStatus();
			if(movementRules ~= "") then
				controlTable = g_TheirCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(movementRules);
				controlTable.Value:SetText("");
			end
			-------------------------
			-- PRIZE SHIPS PREVIEW --
			-------------------------
			if (not bRanged) then
				local iChance;
				iChance = pMyUnit:GetCaptureChance(pTheirUnit);
				if (iChance > 0) then
						controlTable = g_TheirCombatDataIM:GetInstance();
						controlTable.Text:LocalizeAndSetText("TXT_KEY_EUPANEL_CAPTURE_CHANCE", iChance);
						controlTable.Value:SetText("");
				end
			end

			if (pTheirUnit:IsCombatUnit()) then

				-- Empire Unhappy
				iModifier = pTheirUnit:GetUnhappinessCombatPenalty();
				if (iModifier ~= 0) then
					controlTable = g_TheirCombatDataIM:GetInstance();
					if(pTheirPlayer:IsEmpireVeryUnhappy()) then
						controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_EMPIRE_VERY_UNHAPPY_PENALTY" );
					else
						controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_EMPIRE_UNHAPPY_PENALTY" );
					end
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end

			-- COMMUNITY (Bushido)
				if (pTheirPlayer:GetWoundedUnitDamageMod() ~= 0) then
				   iModifier = pTheirUnit:GetDamage() / 5;
				   if (iModifier ~= 0) then
					   controlTable = g_TheirCombatDataIM:GetInstance();
					   controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BUSHIDO" );
					   controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
					end
				end
				if (pTheirUnit:IsStrongerDamaged()) then
				   iModifier =  pTheirUnit:GetDamage() / 3;
					if (iModifier ~= 0) then
					   controlTable = g_TheirCombatDataIM:GetInstance();
					   controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BUSHIDO" );
					   controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
					end
				end

				if (not bRanged) then
					iModifier = pTheirUnit:GetExtraWithdrawal();
					if (iModifier ~= 0) then
					   controlTable = g_TheirCombatDataIM:GetInstance();
					   controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_WITHDRAW_CHANCE" );
					   controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
					end		
				end

				-- CBP (Monopoly)
				iModifier = pTheirUnit:GetMonopolyDefenseBonus();
				if(iModifier ~= 0) then
					controlTable = g_TheirCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_MONOPOLY_POWER_DEFENSE" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end

			-- COMMUNITY (Resistance)
				iModifier = pTheirUnit:GetResistancePower(pMyUnit);
				if(iModifier ~= 0) then
					controlTable = g_TheirCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_RESISTANCE_POWER" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end
			--END

				-- Lack Strategic Resources
				iModifier = pTheirUnit:GetStrategicResourceCombatPenalty();
				if (iModifier ~= 0) then
					controlTable = g_TheirCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_STRATEGIC_RESOURCE" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end
				
				-- Adjacent Modifier
				iModifier = pTheirUnit:GetAdjacentModifier();
				if (iModifier ~= 0) then
					local bCombatUnit = true;
					if (pTheirUnit:IsFriendlyUnitAdjacent(bCombatUnit)) then
						controlTable = g_TheirCombatDataIM:GetInstance();
						controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_ADJACENT_FRIEND_UNIT_BONUS" );
						controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
					end
				end

				-- Plot Defense
				iModifier = pToPlot:DefenseModifier(pTheirUnit:GetTeam(), false, false);

				-- special treatment for mobile units
				if (pTheirUnit:NoDefensiveBonus() and iModifier>0) then
					-- only improvements (forts) count
					iModifier = iModifier - pToPlot:DefenseModifier(pTheirUnit:GetTeam(), true, false);
				end

				if (iModifier ~= 0) then
					controlTable = g_TheirCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_TERRAIN_MODIFIER" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end	

				-- FortifyModifier
				iModifier = pTheirUnit:FortifyModifier();
				if (iModifier ~= 0) then
					controlTable = g_TheirCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_FORTIFICATION_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
	--				strString.append(GetLocalizedText("TXT_KEY_COMBAT_PLOT_FORTIFY_MOD", iModifier));
				end
				-- NearbyPromotion Unit Bonus
				if (pTheirUnit:GetGiveCombatModToUnit() ~= 0) then
					iModifier = pTheirUnit:GetGiveCombatModToUnit();
					controlTable = g_TheirCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_NEARBYPROMOTION_COMBAT_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end
				--NearbyPromtoion Unit that gets a bonus near cities?
				if (pTheirUnit:GetNearbyCityBonusCombatMod() ~= 0) then
					iModifier = pTheirUnit:GetNearbyCityBonusCombatMod();
					controlTable = g_TheirCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_NEARBYPROMOTION_CITY_COMBAT_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end
				-- Great General bonus
				if (pTheirUnit:IsNearGreatGeneral()) then
					iModifier = pTheirPlayer:GetGreatGeneralCombatBonus() + pTheirUnit:GetGreatGeneralAuraBonus();
					iModifier = iModifier + pTheirPlayer:GetTraitGreatGeneralExtraBonus();
					controlTable = g_TheirCombatDataIM:GetInstance();
					if (pTheirUnit:GetDomainType() == DomainTypes.DOMAIN_LAND) then
						controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_GG_NEAR" );
					else
						controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_GA_NEAR" );
					end
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
					
					-- Ignores Great General penalty
					if (pTheirUnit:IsIgnoreGreatGeneralBenefit()) then
						controlTable = g_TheirCombatDataIM:GetInstance();
						controlTable.Text:LocalizeAndSetText("TXT_KEY_EUPANEL_IGG");
						controlTable.Value:SetText(GetFormattedText(strText, -iModifier, false, true));
					end
				end
				
				-- Great General stack bonus
				if (pTheirUnit:GetGreatGeneralCombatModifier() ~= 0 and pTheirUnit:IsStackedGreatGeneral()) then
					iModifier = pTheirUnit:GetGreatGeneralCombatModifier();
					controlTable = g_TheirCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_GG_STACKED" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end
				
				-- Reverse Great General bonus
				if (pTheirUnit:GetReverseGreatGeneralModifier() ~= 0) then
					iModifier = pTheirUnit:GetReverseGreatGeneralModifier();
					controlTable = g_TheirCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_REVERSE_GG_NEAR" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end 
				
				-- Nearby improvement modifier
				if (pTheirUnit:GetNearbyImprovementModifier(pToPlot) ~= 0) then
					iModifier = pTheirUnit:GetNearbyImprovementModifier(pToPlot);
					controlTable = g_TheirCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_IMPROVEMENT_NEAR" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end
				-- Nearby UnitClass modifier
				if (pTheirUnit:GetNearbyUnitClassModifierFromUnitClass(pToPlot) ~= 0) then
					iModifier = pTheirUnit:GetNearbyUnitClassModifierFromUnitClass(pToPlot);
					controlTable = g_TheirCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_UNITCLASS_NEAR" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end
				
				-- Flanking bonus
				if (not bRanged) then
					iModifier = pToPlot:GetEffectiveFlankingBonus(pTheirUnit,pMyUnit,pFromPlot);
					if (iModifier ~= 0) then
						controlTable = g_TheirCombatDataIM:GetInstance();
						controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_FLANKING_BONUS" );
						controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
					end
				end
				
				-- ExtraCombatPercent
				iModifier = pTheirUnit:GetExtraCombatPercent();
				if (iModifier ~= 0) then
					controlTable = g_TheirCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_EXTRA_PERCENT" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
	--				strString.append(GetLocalizedText("TXT_KEY_COMBAT_PLOT_EXTRA_STRENGTH", iModifier));
				end

				-- Bonus for fighting in one's lands
				if (pToPlot:IsFriendlyTerritory(iTheirPlayer)) then
					iModifier = pTheirUnit:GetFriendlyLandsModifier();
					if (iModifier ~= 0) then
						controlTable = g_TheirCombatDataIM:GetInstance();
						controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_FIGHT_AT_HOME_BONUS" );
						controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
					end
					
					iModifier = pTheirPlayer:GetFoundedReligionFriendlyCityCombatMod(pToPlot);
					if (iModifier ~= 0) then
						controlTable = g_TheirCombatDataIM:GetInstance();
						controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_FRIENDLY_CITY_BELIEF_BONUS" );
						controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
					end
-- COMMUNITY PATCH CHANGE
					iModifier = pTheirUnit:GetCombatVersusOtherReligionOwnLands(pMyUnit);
					if (iModifier ~= 0) then
						controlTable = g_TheirCombatDataIM:GetInstance();
						controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_FRIENDLY_CITY_BELIEF_BONUS_CBP" );
						controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
					end
--END
				end
				
				-- Bonus for fighting outside one's lands
				if (not pToPlot:IsFriendlyTerritory(iTheirPlayer)) then
					
					-- General combat mod
					iModifier = pTheirUnit:GetOutsideFriendlyLandsModifier();
					if (iModifier ~= 0) then
						controlTable = g_TheirCombatDataIM:GetInstance();
						controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_OUTSIDE_HOME_BONUS" );
						controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
					end
					
					iModifier = pTheirPlayer:GetFoundedReligionEnemyCityCombatMod(pToPlot);
					if (iModifier ~= 0) then
						controlTable = g_TheirCombatDataIM:GetInstance();
						controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ENEMY_CITY_BELIEF_BONUS" );
						controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
					end
				end

-- COMMUNITY PATCH CHANGE
				if (pToPlot:IsFriendlyTerritory(iMyPlayer)) then
					iModifier = pTheirUnit:GetCombatVersusOtherReligionTheirLands(pMyUnit);
					if (iModifier ~= 0) then
						controlTable = g_TheirCombatDataIM:GetInstance();
						controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ENEMY_CITY_BELIEF_BONUS_CBP" );
						controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
					end
				end
--END
				
				-- Defense Modifier
				local iModifier = pTheirUnit:GetDefenseModifier();
				if (iModifier ~= 0) then
					controlTable = g_TheirCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_DEFENSE_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end

				-- UnitClassDefenseModifier
				iModifier = pTheirUnit:UnitClassDefenseModifier(pMyUnit:GetUnitClassType());
				if (iModifier ~= 0) then
					controlTable = g_TheirCombatDataIM:GetInstance();
					local unitClassBonus = Locale.ConvertTextKey(GameInfo.UnitClasses[pMyUnit:GetUnitClassType()].Description);
					controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_BONUS_VS_CLASS", unitClassBonus );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end

				---- ClassDefenseModifier
				--iModifier = pTheirUnit:unitClassDefenseModifier(pMyUnit:getUnitClassType());
				--if (iModifier ~= 0) then
					--controlTable = g_TheirCombatDataIM:GetInstance();
					--strText = "Defense Bonus: " .. iModifier .. "%";
	----				strString.append(GetLocalizedText("TXT_KEY_COMBAT_PLOT_MOD_VS_TYPE", iModifier, GC.getUnitClassInfo(pMyUnit:getUnitClassType()).GetTextKey()));
				--end

				-- UnitCombatModifier
				if (pMyUnit:GetUnitCombatType() ~= -1) then
					iModifier = pTheirUnit:UnitCombatModifier(pMyUnit:GetUnitCombatType());

					if (iModifier ~= 0) then
						controlTable = g_TheirCombatDataIM:GetInstance();
						local unitClassType = Locale.ConvertTextKey(GameInfo.UnitCombatInfos[pMyUnit:GetUnitCombatType()].Description);
						controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BONUS_VS_CLASS", unitClassType );
						controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
					end
				end

				---- unitCombatModifier
				--if (pMyUnit:getUnitCombatType() ~= NO_UNITCOMBAT)
				--then
					--iModifier = pTheirUnit:unitCombatModifier(pMyUnit:getUnitCombatType());
					--
					--if (iModifier ~= 0)
					--then
						--strString.append(NEWLINE);
						--strString.append(GetLocalizedText("TXT_KEY_COMBAT_PLOT_MOD_VS_TYPE", iModifier, GC.getUnitCombatClassInfo(pMyUnit:getUnitCombatType()).GetTextKey()));
					--end
				--end

-- CBP
				-- PerAdjacentUnitCombatModifier
				iModifier = pTheirUnit:PerAdjacentUnitCombatModifier() + pTheirUnit:PerAdjacentUnitCombatDefenseMod();
				if (iModifier ~= 0) then
					controlTable = g_TheirCombatDataIM:GetInstance();
					--local unitClassType = Locale.ConvertTextKey(GameInfo.UnitClasses[pTheirUnit:GetUnitClassType()].Description);
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BONUS_PER_ADJACENT_UNIT_COMBAT" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end
-- END

				-- DomainModifier
				iModifier = pTheirUnit:DomainModifier(pMyUnit:GetDomainType());
				if (iModifier ~= 0) then
					controlTable = g_TheirCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_BONUS_VS_DOMAIN" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );

					--CvBaseInfo kDomainInfo;
					--Database::SingleResult kResult;
					--DB.SelectAt(kResult, "Domains", pTheirUnit:getDomainType());
					--kDomainInfo.CacheResult(kResult);

					--strString.append(GetLocalizedText("TXT_KEY_COMBAT_PLOT_MOD_VS_TYPE", iModifier, kDomainInfo.GetDescription()));
				end
				
				-- HillsDefenseModifier
				if (pToPlot:IsHills()) then
					iModifier = pTheirUnit:HillsDefenseModifier();

					if (iModifier ~= 0) then
						controlTable = g_TheirCombatDataIM:GetInstance();
						controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_HILL_DEFENSE_BONUS" );
						controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
					end
				end
				
				-- OpenDefenseModifier
				if (pToPlot:IsOpenGround()) then
					iModifier = pTheirUnit:OpenDefenseModifier();

					if (iModifier ~= 0) then
						controlTable = g_TheirCombatDataIM:GetInstance();
						controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_OPEN_TERRAIN_DEF_BONUS" );
						controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
					end
				end
				
				-- RoughDefenseModifier
				if (pToPlot:IsRoughGround()) then
					iModifier = pTheirUnit:RoughDefenseModifier();

					if (iModifier ~= 0) then
						controlTable = g_TheirCombatDataIM:GetInstance();
						controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_ROUGH_TERRAIN_DEF_BONUS" );
						controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
					end
				end
	
-- COMMUNITY PATCH CHANGE	
		
				-- CombatBonusVsHigherPop
				iModifier = pTheirPlayer:GetCombatBonusVsHigherPop();
				if (iModifier ~= 0 and pMyUnit:IsHigherPopThan(pTheirUnit)) then
					controlTable = g_TheirCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_TRAIT_LOW_POP_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end
				iModifier = pTheirUnit:GetAllianceCSStrength();
				if (iModifier ~= 0) then
					controlTable = g_TheirCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ATTACK_CS_ALLIANCE_STRENGTH" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end
--END
				-- CombatBonusVsHigherTech
				if (pToPlot:GetOwner() == iTheirPlayer) then
					iModifier = pTheirPlayer:GetCombatBonusVsHigherTech();

					if (iModifier ~= 0 and pMyUnit:IsHigherTechThan(pTheirUnit:GetUnitType())) then
						controlTable = g_TheirCombatDataIM:GetInstance();
						controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_TRAIT_LOW_TECH_BONUS" );
						controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
					end
				end
				
				-- CombatBonusVsLargerCiv
				iModifier = pTheirPlayer:GetCombatBonusVsLargerCiv();
				if (iModifier ~= 0 and pMyUnit:IsLargerCivThan(pTheirUnit)) then
					controlTable = g_TheirCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_TRAIT_SMALL_SIZE_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end
								
				-- CapitalDefenseModifier
				iModifier = pTheirUnit:CapitalDefenseModifier();
				if (iModifier > 0) then
				
					-- Compute distance to capital
					local pCapital = pTheirPlayer:GetCapitalCity();
					
					if (pCapital ~= nil) then

						local plotDistance = Map.PlotDistance(pCapital:GetX(), pCapital:GetY(), pTheirUnit:GetX(), pTheirUnit:GetY());
						iModifier = iModifier + (plotDistance * pTheirUnit:CapitalDefenseFalloff());

						if (iModifier > 0) then
							controlTable = g_TheirCombatDataIM:GetInstance();
							controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_CAPITAL_DEFENSE_BONUS" );
							controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
						end
					end
				end
		
				if (pToPlot:GetFeatureType() ~= -1) then
				
					-- FeatureDefenseModifier
					iModifier = pTheirUnit:FeatureDefenseModifier(pToPlot:GetFeatureType());
					if (iModifier ~= 0) then
						controlTable = g_TheirCombatDataIM:GetInstance();
						local typeBonus = Locale.ConvertTextKey(GameInfo.Features[pToPlot:GetFeatureType()].Description);
						controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_BONUS_DEFENSE_TERRAIN", typeBonus );
						controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
					end
				else
				
					-- TerrainDefenseModifier		
					iModifier = pTheirUnit:TerrainDefenseModifier(pToPlot:GetTerrainType());
					if (iModifier ~= 0) then
						controlTable = g_TheirCombatDataIM:GetInstance();
						local typeBonus = Locale.ConvertTextKey(GameInfo.Terrains[pToPlot:GetTerrainType()].Description);
						controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_BONUS_DEFENSE_TERRAIN", typeBonus );
						controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
					end
					
					if (pToPlot:IsHills()) then
						iModifier = pTheirUnit:TerrainDefenseModifier(GameInfo.Terrains["TERRAIN_HILL"].ID);
						if (iModifier ~= 0) then
							controlTable = g_TheirCombatDataIM:GetInstance();
							local terrainTypeBonus = Locale.ConvertTextKey( GameInfo.Terrains["TERRAIN_HILL"].Description );
							controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BONUS_DEFENSE_TERRAIN", terrainTypeBonus  );
							controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
						end
					end
				end
				
				-- Civ Trait Bonus
				iModifier = pTheirPlayer:GetTraitGoldenAgeCombatModifier();
				if (iModifier ~= 0 and pTheirPlayer:IsGoldenAge()) then
					controlTable = g_TheirCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_BONUS_GOLDEN_AGE" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end
			end
		end
	end
	
    Controls.MyCombatResultsStack:CalculateSize();
    Controls.TheirCombatResultsStack:CalculateSize();
    
    local sizeX = Controls.DetailsGrid:GetSizeX();
    Controls.DetailsGrid:DoAutoSize();
    Controls.DetailsGrid:SetSizeX( sizeX );
    Controls.DetailsSeperator:SetSizeY( Controls.DetailsGrid:GetSizeY() );
    Controls.DetailsGrid:ReprocessAnchoring();
end

function UpdateCombatOddsCityVsUnit(myCity, theirUnit)
	
	-- Reset bonuses
	g_MyCombatDataIM:ResetInstances();
	g_TheirCombatDataIM:ResetInstances();
	
	--Set Initial Values
	local myCityMaxHP = myCity:GetMaxHitPoints();
	local myCityCurHP = myCity:GetDamage();
	local myCityDamageInflicted = myCity:RangeCombatDamage(theirUnit, nil);
	local myCityStrength = myCity:GetStrengthValue(true);
	
	local theirUnitMaxHP = theirUnit:GetMaxHitPoints();
	local theirUnitCurHP = theirUnit:GetDamage();
	local theirUnitDamageInflicted = 0;
	local theirUnitStrength = myCity:RangeCombatUnitDefense(theirUnit);
	local iTheirPlayer = theirUnit:GetOwner();
	local pTheirPlayer = Players[iTheirPlayer];

	if (myCityDamageInflicted > theirUnitMaxHP) then
		myCityDamageInflicted = theirUnitMaxHP;
	end
				
	-- City vs Unit is ranged attack
	Controls.RangedAttackIndicator:SetHide(false);
	Controls.RangedAttackButtonLabel:SetText(Locale.ToUpper(Locale.ConvertTextKey("TXT_KEY_INTERFACEMODE_RANGE_ATTACK")));
		
	-- Their Damage
	Controls.TheirDamageValue:SetText("[COLOR_RED]" .. theirUnitDamageInflicted .. "[ENDCOLOR]");
	
	-- Their Strength
	Controls.TheirStrengthValue:SetText( Locale.ToNumber(theirUnitStrength / 100, "#.##"));

	-- My Damage
	Controls.MyDamageValue:SetText("[COLOR_GREEN]" .. myCityDamageInflicted .. "[ENDCOLOR]");
	
	-- My Strength
	Controls.MyStrengthValue:SetText( Locale.ToNumber(myCityStrength / 100, "#.##"));
	
	DoUpdateHealthBars(myCityMaxHP, theirUnitMaxHP, myCityCurHP, theirUnitCurHP, myCityDamageInflicted, theirUnitDamageInflicted);
					
	Controls.RangedAttackIndicator:SetHide(false);
	Controls.SafeAttackIndicator:SetHide(true);
	Controls.RiskyAttackIndicator:SetHide(true);
	Controls.TotalVictoryIndicator:SetHide(true);
	Controls.MajorVictoryIndicator:SetHide(true);
	Controls.SmallVictoryIndicator:SetHide(true);
	Controls.SmallDefeatIndicator:SetHide(true);
	Controls.MajorDefeatIndicator:SetHide(true);
	Controls.TotalDefeatIndicator:SetHide(true);
	Controls.StalemateIndicator:SetHide(true);
					
	-- Show some bonuses
	if (theirUnit:IsCombatUnit()) then

		local myPlayerID = myCity:GetOwner();
		local myPlayer = Players[myPlayerID];
		
		local theirPlayerID = theirUnit:GetOwner();
		local theirPlayer = Players[theirPlayerID];
		local myPlot = myCity:Plot();
		local theirPlot = theirUnit:GetPlot();
		
		-- Empire Unhappy
		iModifier = theirUnit:GetUnhappinessCombatPenalty();
		if (iModifier ~= 0) then
			controlTable = g_TheirCombatDataIM:GetInstance();
			if(theirPlayer:IsEmpireVeryUnhappy()) then
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_EMPIRE_VERY_UNHAPPY_PENALTY" );
			else
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_EMPIRE_UNHAPPY_PENALTY" );
			end
			controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
		end

		-- Lack Strategic Resources
		iModifier = theirUnit:GetStrategicResourceCombatPenalty();
		if (iModifier ~= 0) then
			controlTable = g_TheirCombatDataIM:GetInstance();
			controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_STRATEGIC_RESOURCE" );
			controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
		end
		
		-- Adjacent Modifier
		iModifier = theirUnit:GetAdjacentModifier();
		if (iModifier ~= 0) then
			local bCombatUnit = true;
			if (theirUnit:IsFriendlyUnitAdjacent(bCombatUnit)) then
				controlTable = g_TheirCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_ADJACENT_FRIEND_UNIT_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
			end
		end

-- CBP
		-- PerAdjacentUnitCombatModifier
		iModifier = theirUnit:PerAdjacentUnitCombatModifier() + theirUnit:PerAdjacentUnitCombatDefenseMod();
		if (iModifier ~= 0) then
			controlTable = g_TheirCombatDataIM:GetInstance();
			controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BONUS_PER_ADJACENT_UNIT_COMBAT" );
			controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
		end
-- END
		
		-- Plot Defense
		iModifier = theirPlot:DefenseModifier(theirUnit:GetTeam(), false, false);

		-- special treatment for mobile units
		if (theirUnit:NoDefensiveBonus() and iModifier>0) then
			-- only improvements (forts) count
			iModifier = iModifier - theirPlot:DefenseModifier(theirUnit:GetTeam(), true, false);
		end

		if (iModifier ~= 0) then
			controlTable = g_TheirCombatDataIM:GetInstance();
			controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_TERRAIN_MODIFIER" );
			controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
		end

		-- FortifyModifier
		iModifier = theirUnit:FortifyModifier();
		if (iModifier ~= 0) then
			controlTable = g_TheirCombatDataIM:GetInstance();
			controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_FORTIFICATION_BONUS" );
			controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
--				strString.append(GetLocalizedText("TXT_KEY_COMBAT_PLOT_FORTIFY_MOD", iModifier));
		end
		
		-- Great General bonus
		if (theirUnit:IsNearGreatGeneral()) then
			iModifier = theirPlayer:GetGreatGeneralCombatBonus() + theirUnit:GetGreatGeneralAuraBonus();
			iModifier = iModifier + theirPlayer:GetTraitGreatGeneralExtraBonus();
			controlTable = g_TheirCombatDataIM:GetInstance();
			if (theirUnit:GetDomainType() == DomainTypes.DOMAIN_LAND) then
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_GG_NEAR" );
			else
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_GA_NEAR" );
			end
			controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
			
			-- Ignores Great General penalty
			if (theirUnit:IsIgnoreGreatGeneralBenefit()) then
				controlTable = g_TheirCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText("TXT_KEY_EUPANEL_IGG");
				controlTable.Value:SetText(GetFormattedText(strText, -iModifier, false, true));
			end
		end
		
		-- Great General stacked bonus
		if (theirUnit:GetGreatGeneralCombatModifier() ~= 0 and theirUnit:IsStackedGreatGeneral()) then
			iModifier = theirUnit:GetGreatGeneralCombatModifier();
			controlTable = g_TheirCombatDataIM:GetInstance();
			controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_GG_STACKED" );
			controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
		end
		
		-- ExtraCombatPercent
		iModifier = theirUnit:GetExtraCombatPercent();
		if (iModifier ~= 0) then
			controlTable = g_TheirCombatDataIM:GetInstance();
			controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_EXTRA_PERCENT" );
			controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
--				strString.append(GetLocalizedText("TXT_KEY_COMBAT_PLOT_EXTRA_STRENGTH", iModifier));
		end

		-- Bonus for fighting in one's lands
		if (theirPlot:IsFriendlyTerritory(iTheirPlayer)) then
			iModifier = theirUnit:GetFriendlyLandsModifier();
			if (iModifier ~= 0) then
				controlTable = g_TheirCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_FIGHT_AT_HOME_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
			end
	
			iModifier = pTheirPlayer:GetFoundedReligionFriendlyCityCombatMod(theirPlot);
			if (iModifier ~= 0) then
				controlTable = g_TheirCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_FRIENDLY_CITY_BELIEF_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
			end
		end
		
		-- Bonus for fighting outside one's lands
		if (not theirPlot:IsFriendlyTerritory(iTheirPlayer)) then
			
			-- General combat mod
			iModifier = theirUnit:GetOutsideFriendlyLandsModifier();
			if (iModifier ~= 0) then
				controlTable = g_TheirCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_OUTSIDE_HOME_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
			end
			
			iModifier = pTheirPlayer:GetFoundedReligionEnemyCityCombatMod(theirPlot);
			if (iModifier ~= 0) then
				controlTable = g_TheirCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ENEMY_CITY_BELIEF_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
			end
		end

-- COMMUNITY PATCH CHANGE
		iModifier = theirUnit:GetAllianceCSStrength();
		if (iModifier ~= 0) then
			controlTable = g_TheirCombatDataIM:GetInstance();
			controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_ATTACK_CS_ALLIANCE_STRENGTH" );
			controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
		end
--END
		-- Defense Modifier
		local iModifier = theirUnit:GetDefenseModifier();
		if (iModifier ~= 0) then
			controlTable = g_TheirCombatDataIM:GetInstance();
			controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_DEFENSE_BONUS" );
			controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
		end
		
		-- HillsDefenseModifier
		if (theirPlot:IsHills()) then
			iModifier = theirUnit:HillsDefenseModifier();

			if (iModifier ~= 0) then
				controlTable = g_TheirCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_HILL_DEFENSE_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
			end
		end
		
		-- OpenDefenseModifier
		if (theirPlot:IsOpenGround()) then
			iModifier = theirUnit:OpenDefenseModifier();

			if (iModifier ~= 0) then
				controlTable = g_TheirCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_OPEN_TERRAIN_DEF_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
			end
		end
		
		-- RoughDefenseModifier
		if (theirPlot:IsRoughGround()) then
			iModifier = theirUnit:RoughDefenseModifier();

			if (iModifier ~= 0) then
				controlTable = g_TheirCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_ROUGH_TERRAIN_DEF_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
			end
		end

		-- CapitalDefenseModifier
		iModifier = theirUnit:CapitalDefenseModifier();
		if (iModifier > 0) then

			-- Compute distance to capital
			local pCapital = theirPlayer:GetCapitalCity();
			
			if (pCapital ~= nil) then
				local plotDistance = Map.PlotDistance(pCapital:GetX(), pCapital:GetY(), theirUnit:GetX(), theirUnit:GetY());
				iModifier = iModifier + (plotDistance * theirUnit:CapitalDefenseFalloff());
						
				if (iModifier > 0) then
					controlTable = g_TheirCombatDataIM:GetInstance();
					controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_CAPITAL_DEFENSE_BONUS" );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end
			end
		end
		
		if (theirPlot:GetFeatureType() ~= -1) then
		
			-- FeatureDefenseModifier
			iModifier = theirUnit:FeatureDefenseModifier(theirPlot:GetFeatureType());
			if (iModifier ~= 0) then
				controlTable = g_TheirCombatDataIM:GetInstance();
				local typeBonus = Locale.ConvertTextKey(GameInfo.Features[theirPlot:GetFeatureType()].Description);
				controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_BONUS_DEFENSE_TERRAIN", typeBonus );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
			end
		else
		
			-- TerrainDefenseModifier		
			iModifier = theirUnit:TerrainDefenseModifier(theirPlot:GetTerrainType());
			if (iModifier ~= 0) then
				controlTable = g_TheirCombatDataIM:GetInstance();
				local typeBonus = Locale.ConvertTextKey(GameInfo.Terrains[theirPlot:GetTerrainType()].Description);
				controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_BONUS_DEFENSE_TERRAIN", typeBonus );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
			end
			
			if (theirPlot:IsHills()) then
				iModifier = theirUnit:TerrainDefenseModifier(GameInfo.Terrains["TERRAIN_HILL"].ID);
				if (iModifier ~= 0) then
					controlTable = g_TheirCombatDataIM:GetInstance();
					local terrainTypeBonus = Locale.ConvertTextKey( GameInfo.Terrains["TERRAIN_HILL"].Description );
					controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BONUS_DEFENSE_TERRAIN", terrainTypeBonus  );
					controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
				end
			end
		end
		
		-- BarbarianBonuses
		if (theirUnit:IsBarbarian()) then
			--iModifier = GameInfo.HandicapInfos[Game:GetHandicapType()].BarbarianBonus;
			
			iModifier = DB_HandicapInfos[Game:GetHandicapType()].BarbarianBonus;
			iModifier = iModifier + myPlayer:GetBarbarianCombatBonus();

			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_VS_BARBARIANS_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
		end
-- CBP
		if(theirUnit:GetDomainType() == DomainTypes.DOMAIN_SEA) then
			iModifier = 100 - GameDefines["BALANCE_NAVAL_DEFENSE_CITY_STRIKE_MODIFIER"];
			iModifier = iModifier * -1; 
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_NAVAL_DEFENSE_CITY" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
		end
	
		-- Civ Trait Bonus
		iModifier = myCity:GetMultiAttackBonusCity(theirUnit);
		if (iModifier ~= 0) then
			controlTable = g_MyCombatDataIM:GetInstance();
			controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_BONUS_MULTI_ATTACK_BONUS" );
			controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
		end
-- END		
		if (myCity:GetGarrisonedUnit() ~= nil) then		
			iModifier = myPlayer:GetGarrisonedCityRangeStrikeModifier();
			if (iModifier ~= 0) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_GARRISONED_CITY_RANGE_BONUS" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
		end
		
		-- Religion Bonus
		iModifier = myCity:GetReligionCityRangeStrikeModifier();
		if (iModifier ~= 0) then
			controlTable = g_MyCombatDataIM:GetInstance();
			controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_BONUS_RELIGIOUS_BELIEF" );
			controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
		end
		
		-- Sapper unit modifier
		if (theirUnit:GetSapperAreaEffectBonus(myCity) ~= 0) then
			iModifier = theirUnit:GetSapperAreaEffectBonus(myCity);
			controlTable = g_TheirCombatDataIM:GetInstance();
			controlTable.Text:LocalizeAndSetText( "TXT_KEY_EUPANEL_CITY_SAPPED" );
			controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
		end
		-- Conquest of the World
		if myPlot ~= nil then
			iModifier = theirPlayer:GetTraitConquestOfTheWorldCityAttackMod(myPlot);
			if (iModifier ~= 0 and theirPlayer:IsGoldenAge()) then
				controlTable = g_MyCombatDataIM:GetInstance();
				controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_BONUS_CONQUEST_OF_WORLD" );
				controlTable.Value:SetText( GetFormattedText(strText, iModifier, true, true) );
			end
		end
		-- Civ Trait Bonus
		iModifier = theirPlayer:GetTraitGoldenAgeCombatModifier();
		if (iModifier ~= 0 and theirPlayer:IsGoldenAge()) then
			controlTable = g_TheirCombatDataIM:GetInstance();
			controlTable.Text:LocalizeAndSetText(  "TXT_KEY_EUPANEL_BONUS_GOLDEN_AGE" );
			controlTable.Value:SetText( GetFormattedText(strText, iModifier, false, true) );
		end
	end
	
	-- Some UI processing.
	Controls.MyCombatResultsStack:CalculateSize();
    Controls.TheirCombatResultsStack:CalculateSize();
    
    local sizeX = Controls.DetailsGrid:GetSizeX();
    Controls.DetailsGrid:DoAutoSize();
    Controls.DetailsGrid:SetSizeX( sizeX );
    Controls.DetailsSeperator:SetSizeY( Controls.DetailsGrid:GetSizeY() );
    Controls.DetailsGrid:ReprocessAnchoring();
end

--------------------------------------------------------------------------------
-- Update Health Bar combat preview
--------------------------------------------------------------------------------

function DoUpdateHealthBars(iMaxMyHP, iTheirMaxHP, myCurrentDamage, theirCurrentDamage, iMyDamageInflicted, iTheirDamageInflicted)

	local myDamageTaken = iTheirDamageInflicted;
	if (myDamageTaken > iMaxMyHP - myCurrentDamage) then
		myDamageTaken = iMaxMyHP - myCurrentDamage;
	end
	myCurrentDamage = myCurrentDamage + myDamageTaken;
	
	-- show the remaining health bar
	local healthPercent = (iMaxMyHP - myCurrentDamage) / iMaxMyHP;
	local healthTimes100 =  math.floor(100 * healthPercent + 0.5);
	local healthBarSize = { x = 8, y = math.floor(115 * healthPercent) };
	if healthTimes100 <= 30 then
		Controls.MyRedBar:SetSize(healthBarSize);
		Controls.MyGreenBar:SetHide(true);
		Controls.MyYellowBar:SetHide(true);
		Controls.MyRedBar:SetHide(false);
	elseif healthTimes100 <= 50 then
		Controls.MyYellowBar:SetSize(healthBarSize);
		Controls.MyGreenBar:SetHide(true);
		Controls.MyYellowBar:SetHide(false);
		Controls.MyRedBar:SetHide(true);
	else
		Controls.MyGreenBar:SetSize(healthBarSize);
		Controls.MyGreenBar:SetHide(false);
		Controls.MyYellowBar:SetHide(true);
		Controls.MyRedBar:SetHide(true);
	end

	-- show the flashing damage bar for my unit
	if myDamageTaken > 0 then
		local damagePercent = myDamageTaken / iMaxMyHP;
		local damageBarSize = { x = 8, y = math.floor(115 * damagePercent) };
		Controls.MyDeltaBar:SetHide(false);
		if healthBarSize.y > 0 then
			Controls.MyDeltaBar:SetOffsetVal( 0, healthBarSize.y + 4 );
		else
			Controls.MyDeltaBar:SetOffsetVal( 0, 2 );
		end
		Controls.MyDeltaBar:SetSize(damageBarSize);
		Controls.MyDeltaBarFlash:SetSize(damageBarSize);
	else
		Controls.MyDeltaBar:SetHide(true);
	end

	-- Now do their health bar
	
	local theirDamageTaken = iMyDamageInflicted;
	if (theirDamageTaken > iTheirMaxHP - theirCurrentDamage) then
		theirDamageTaken = iTheirMaxHP - theirCurrentDamage;
	end
	theirCurrentDamage = theirCurrentDamage + theirDamageTaken;

	-- show the remaining health bar
	healthPercent = (iTheirMaxHP - theirCurrentDamage) / iTheirMaxHP;
	healthTimes100 =  math.floor(100 * healthPercent + 0.5);
	healthBarSize = { x = 8, y = math.floor(115 * healthPercent) };
	if healthTimes100 <= 30 then
		Controls.TheirRedBar:SetSize(healthBarSize);
		Controls.TheirGreenBar:SetHide(true);
		Controls.TheirYellowBar:SetHide(true);
		Controls.TheirRedBar:SetHide(false);
	elseif healthTimes100 <= 50 then
		Controls.TheirYellowBar:SetSize(healthBarSize);
		Controls.TheirGreenBar:SetHide(true);
		Controls.TheirYellowBar:SetHide(false);
		Controls.TheirRedBar:SetHide(true);
	else
		Controls.TheirGreenBar:SetSize(healthBarSize);
		Controls.TheirGreenBar:SetHide(false);
		Controls.TheirYellowBar:SetHide(true);
		Controls.TheirRedBar:SetHide(true);
	end

	-- show the flashing damage bar for my unit
	if theirDamageTaken > 0 then
		local damagePercent = theirDamageTaken / iTheirMaxHP;
		local damageBarSize = { x = 8, y = math.floor(115 * damagePercent) };
		Controls.TheirDeltaBar:SetHide(false);
		if healthBarSize.y > 0 then
			Controls.TheirDeltaBar:SetOffsetVal( 0, healthBarSize.y + 4 );
		else
			Controls.TheirDeltaBar:SetOffsetVal( 0, 2 );
		end
		Controls.TheirDeltaBar:SetSize(damageBarSize);
		Controls.TheirDeltaBarFlash:SetSize(damageBarSize);
	else
		Controls.TheirDeltaBar:SetHide(true);
	end
	
end


--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
function OnWorldMouseOver( bWorldHasMouseOver )
    g_bWorldMouseOver = bWorldHasMouseOver;
    
	if( g_bShowPanel and g_bWorldMouseOver ) then
		ContextPtr:SetHide( false );
	else
		ContextPtr:SetHide( true );
    end
end
Events.WorldMouseOver.Add( OnWorldMouseOver );



--------------------------------------------------------------------------------
-- Hex has been moused over
--------------------------------------------------------------------------------
function OnMouseOverHex( hexX, hexY )

	g_bShowPanel = false;
	
	Controls.MyCombatResultsStack:SetHide(true);
	Controls.TheirCombatResultsStack:SetHide(true);
	Events.ClearHexHighlightStyle("ValidFireTargetBorder") --JFD
	
	local pPlot = Map.GetPlot( hexX, hexY );
	
	if (pPlot ~= nil) then
		local pHeadUnit = UI.GetHeadSelectedUnit();
		local pHeadCity = UI.GetHeadSelectedCity();
				
		if (pHeadUnit ~= nil) then
			
			-- melee attack
			if (pHeadUnit:IsCombatUnit() and (pHeadUnit:IsRanged() and pHeadUnit:IsEmbarked()) == false) and ((pHeadUnit:IsRanged() and pHeadUnit:IsRangeAttackOnlyInDomain() and not pPlot:IsWater()) == false) then
				
				local iTeam = Game.GetActiveTeam()
				local pTeam = Teams[iTeam]
				
				-- Don't show info for stuff we can't see
				if (pPlot:IsRevealed(iTeam, false)) then
					
					-- City
					if (pPlot:IsCity()) then
						
						local pCity = pPlot:GetPlotCity();
						
						if (pTeam:IsAtWar(pCity:GetTeam()) or (UIManager:GetAlt() and pCity:GetOwner() ~= iTeam)) then
							UpdateCityStats(pCity);
							UpdateCombatOddsUnitVsCity(pHeadUnit, pCity);
							UpdateUnitPromotions(nil);
							UpdateCityPortrait(pCity);
							
							Controls.MyCombatResultsStack:SetHide(false);
							Controls.TheirCombatResultsStack:SetHide(false);
							g_bShowPanel = true;
						end
						
					-- No City Here
					else
						
						-- Can see this plot right now
						if (pPlot:IsVisible(iTeam, false) and not pHeadUnit:IsCityAttackOnly()) then
							
							local iNumUnits = pPlot:GetNumUnits();
							local pUnit;
							
							-- Loop through all Units
							for i = 0, iNumUnits do
								pUnit = pPlot:GetUnit(i);
								if (pUnit ~= nil and not pUnit:IsInvisible(iTeam, false)) then
									
									-- No air units
									--if (pUnit:GetDomainType() ~= DomainTypes.DOMAIN_AIR) then
										
										-- Other guy must be same domain, OR we must be ranged OR we must be naval and he is embarked
										if (pHeadUnit:GetDomainType() == pUnit:GetDomainType() or pHeadUnit:IsRanged() or (pHeadUnit:GetDomainType() == DomainTypes.DOMAIN_SEA and pUnit:IsEmbarked())) then
										
											 if (pUnit:GetBaseCombatStrength() > 0 or pHeadUnit:IsRanged()) then
												UpdateUnitPortrait(pUnit);
												UpdateUnitPromotions(pUnit);
												UpdateUnitStats(pUnit);
												
												if (pTeam:IsAtWar(pUnit:GetTeam()) or (UIManager:GetAlt() and pUnit:GetOwner() ~= iTeam)) then
													UpdateCombatOddsUnitVsUnit(pHeadUnit, pUnit);
													Controls.MyCombatResultsStack:SetHide(false);
													Controls.TheirCombatResultsStack:SetHide(false);
													
													g_bShowPanel = true;
													break;
												end
											end
										end
									--end
								end
							end
						end
					end
				end
			elseif(pHeadUnit:IsRanged() == true and pHeadUnit:IsEmbarked() == false) then
				
				local iTeam = Game.GetActiveTeam()
				local pTeam = Teams[iTeam]
				
				-- Don't show info for stuff we can't see
				if (pPlot:IsRevealed(iTeam, false)) then
					
					-- City and not a missile attack (30 is unitai_missile_air)
					if (pPlot:IsCity() and pHeadUnit:GetUnitAIType() ~= 30) then
						
						local pCity = pPlot:GetPlotCity();
						
						if (pTeam:IsAtWar(pCity:GetTeam()) or (UIManager:GetAlt() and pCity:GetOwner() ~= iTeam)) then
							UpdateCityStats(pCity);
							UpdateCombatOddsUnitVsCity(pHeadUnit, pCity);
							UpdateUnitPromotions(nil);
							UpdateCityPortrait(pCity);
							
							Controls.MyCombatResultsStack:SetHide(false);
							Controls.TheirCombatResultsStack:SetHide(false);
							g_bShowPanel = true;
						end
						
					-- No City Here
					else
						
						-- Can see this plot right now
						if (pPlot:IsVisible(iTeam, false)) then
							
							local iNumUnits = pPlot:GetNumUnits();
							local pUnit;
							
							-- Loop through all Units
							for i = 0, iNumUnits do
								pUnit = pPlot:GetUnit(i);
								if (pUnit ~= nil and not pUnit:IsInvisible(iTeam, false)) then
									 if (pUnit:GetBaseCombatStrength() > 0 or pHeadUnit:IsRanged()) then
										UpdateUnitPortrait(pUnit);
										UpdateUnitPromotions(pUnit);
										UpdateUnitStats(pUnit);
												
										if (pTeam:IsAtWar(pUnit:GetTeam()) or (UIManager:GetAlt() and pUnit:GetOwner() ~= iTeam)) then
											UpdateCombatOddsUnitVsUnit(pHeadUnit, pUnit);
											Controls.MyCombatResultsStack:SetHide(false);
											Controls.TheirCombatResultsStack:SetHide(false);
											
											g_bShowPanel = true;
											break;
										end
									end
								end
							end
						end
					end
				end
				
			end
		
		elseif(pHeadCity ~= nil and pHeadCity:CanRangeStrikeNow()) then	-- no unit selected, what about a city?
			
			local myTeamID = Game.GetActiveTeam();
			
			-- Don't show info for stuff we can't see
			if (pPlot:IsVisible(myTeamID, false)) then
							
				local numUnitsOnPlot = pPlot:GetNumUnits();
				
				local myTeam = Teams[myTeamID];
				
				-- Loop through all Units
				for i = 0, numUnitsOnPlot - 1 do
					local theirUnit = pPlot:GetUnit(i);
					
					
					if (theirUnit ~= nil and not theirUnit:IsInvisible(myTeamID, false)) then
														
						-- No air units
						if (theirUnit:GetDomainType() ~= DomainTypes.DOMAIN_AIR) then
							
							if (myTeam:IsAtWar(theirUnit:GetTeam()) or (UIManager:GetAlt() and theirUnit:GetOwner() ~= myTeamID)) then
								
								-- Enemy Unit info
								UpdateUnitPortrait(theirUnit);
								UpdateUnitPromotions(theirUnit);
								UpdateUnitStats(theirUnit);
							
								UpdateCombatOddsCityVsUnit(pHeadCity, theirUnit);
								
								Controls.MyCombatResultsStack:SetHide(false);
								Controls.TheirCombatResultsStack:SetHide(false);
								
								g_bShowPanel = true;
								
								break;
							end
						end
					end
				end
			end
		end
	end
	
	if( g_bShowPanel and g_bWorldMouseOver ) then
		ContextPtr:SetHide( false );
	else
		ContextPtr:SetHide( true );
	end
end
Events.SerialEventMouseOverHex.Add( OnMouseOverHex );


--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bIsInit )
    if( not bIsInit ) then
        LuaEvents.EnemyPanelHide( bIsHide );
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );