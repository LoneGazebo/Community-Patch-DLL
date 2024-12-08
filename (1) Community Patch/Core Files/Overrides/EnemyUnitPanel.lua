-------------------------------------------------
-- Enemy Unit Panel Screen + Combat Simulator
-------------------------------------------------
include("IconSupport");
include("InstanceManager");

local g_MyCombatDataIM = InstanceManager:new("UsCombatInfo", "Text", Controls.MyCombatResultsStack);
local g_TheirCombatDataIM = InstanceManager:new("ThemCombatInfo", "Text", Controls.TheirCombatResultsStack);

local g_iPortraitSize = Controls.UnitPortrait:GetSize().x;

local g_bWorldMouseOver = true;
local g_bShowPanel = false;

-- Maximum number of bonuses that can be displayed on the combat simulator. Rest are displayed as misc. bonus.
local g_iMaxBonusDisplay = 7;

--------------------------------------------------------------------------------
-- Update unit/city display name
--------------------------------------------------------------------------------
function SetName(name)
	name = Locale.ToUpper(name);
	local iNameLength = Locale.Length(name);

	if iNameLength < 18 then
		Controls.UnitName:SetText(name);
		Controls.UnitName:SetHide(false);
		Controls.LongUnitName:SetHide(true);
		Controls.ReallyLongUnitName:SetHide(true);
	elseif iNameLength < 23 then
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
-- Refresh city portrait and name
--------------------------------------------------------------------------------
function UpdateCityPortrait(pCity)
	if not pCity then
		return;
	end

	SetName(pCity:GetName());
	local ePlayer = pCity:GetOwner();
	local pPlayer = Players[pCity:GetOwner()];
	local eCiv = PreGame.GetCivilization(ePlayer);
	local pCiv = GameInfo.Civilizations[eCiv];

	-- print("pCiv.AlphaIconAtlas: " .. tostring(pCiv.AlphaIconAtlas));

	local textureOffset, textureAtlas = IconLookup(pCiv.PortraitIndex, 32, pCiv.AlphaIconAtlas);
	Controls.UnitIcon:SetTexture(textureAtlas);
	Controls.UnitIconShadow:SetTexture(textureAtlas);
	Controls.UnitIcon:SetTextureOffset(textureOffset);
	Controls.UnitIconShadow:SetTextureOffset(textureOffset);

	local iconColor, flagColor = pPlayer:GetPlayerColors();
	if pPlayer:IsMinorCiv() then
		flagColor, iconColor = iconColor, flagColor;
	end
	Controls.UnitBackColor:SetColor(flagColor);
	Controls.UnitIcon:SetColor(iconColor);

	IconHookup(0, g_iPortraitSize, "ENEMY_CITY_ATLAS", Controls.UnitPortrait);
end

--------------------------------------------------------------------------------
-- Refresh unit portrait and name
--------------------------------------------------------------------------------
function UpdateUnitPortrait(pUnit)
	if not pUnit then
		return;
	end

	SetName(pUnit:GetName());
	local flagOffset, flagAtlas = UI.GetUnitFlagIcon(pUnit);
	local textureOffset, textureSheet = IconLookup(flagOffset, 32, flagAtlas);
	Controls.UnitIcon:SetTexture(textureSheet);
	Controls.UnitIconShadow:SetTexture(textureSheet);
	Controls.UnitIcon:SetTextureOffset(textureOffset);
	Controls.UnitIconShadow:SetTextureOffset(textureOffset);

	local pPlayer = Players[pUnit:GetOwner()];
	local iconColor, flagColor = pPlayer:GetPlayerColors();
	if pPlayer:IsMinorCiv() then
		flagColor, iconColor = iconColor, flagColor;
	end
	Controls.UnitBackColor:SetColor(flagColor);
	Controls.UnitIcon:SetColor(iconColor);

	local portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon(pUnit);
	textureOffset, textureSheet = IconLookup(portraitOffset, g_iPortraitSize, portraitAtlas);
	if not textureOffset then
		textureSheet = "TechAtlasSmall.dds";
		textureOffset = Vector2(0, 0);
	end
	Controls.UnitPortrait:SetTexture(textureSheet);
	Controls.UnitPortrait:SetTextureOffset(textureOffset);
end

--------------------------------------------------------------------------------
-- Refresh promotions
--------------------------------------------------------------------------------
function UpdateUnitPromotions(pUnit)
	local UnitPromotionKey = "UnitPromotion";

	-- Clear promotions
	local i = 1;
	while Controls[UnitPromotionKey .. i] do
		local promotionIcon = Controls[UnitPromotionKey .. i];
		promotionIcon:SetHide(true);
		i = i + 1;
	end

	if pUnit then
		-- For each available promotion, display the icon
		for pPromotion in GameInfo.UnitPromotions() do
			local ePromotion = pPromotion.ID;

			if pUnit:IsHasPromotion(ePromotion) then
				-- Get next available promotion button
				local j = 1;
				local promotionIcon;
				repeat
					promotionIcon = Controls[UnitPromotionKey .. j];
					j = j + 1;
				until not promotionIcon or promotionIcon:IsHidden();

				if promotionIcon then
					IconHookup(pPromotion.PortraitIndex, 32, pPromotion.IconAtlas, promotionIcon);
					promotionIcon:SetHide(false);
				end
			end
		end
	end
end

--------------------------------------------------------------------------------
-- Refresh city stats
--------------------------------------------------------------------------------
function UpdateCityStats(pCity)
	-- Strength
	local sStrength = tostring(math.floor(pCity:GetStrengthValue() / 100)) .. " [ICON_STRENGTH]";
	Controls.UnitStatStrength:SetText(sStrength);
	Controls.UnitStrengthBox:SetHide(false);
	Controls.UnitMovementBox:SetHide(true);
	Controls.UnitRangedAttackBox:SetHide(true);
end

--------------------------------------------------------------------------------
-- Refresh unit stats
--------------------------------------------------------------------------------
function UpdateUnitStats(pUnit)
	-- Movement
	local iMaxMoves = pUnit:MaxMoves() / GameDefines.MOVE_DENOMINATOR;
	local sMaxMoves = tostring(iMaxMoves) .. " [ICON_MOVES]";
	Controls.UnitStatMovement:SetText(sMaxMoves);
	Controls.UnitMovementBox:SetHide(false);

	-- Strength
	local iStrength = 0;
	if pUnit:GetDomainType() == DomainTypes.DOMAIN_AIR then
		iStrength = pUnit:GetBaseRangedCombatStrength();
	else
		iStrength = pUnit:GetBaseCombatStrength();
	end
	if iStrength > 0 then
		local sStrength = tostring(iStrength) .. " [ICON_STRENGTH]";
		Controls.UnitStrengthBox:SetHide(false);
		Controls.UnitStatStrength:SetText(sStrength);
	else
		Controls.UnitStrengthBox:SetHide(true);
	end

	-- Ranged Strength
	local iRangedStrength = 0;
	if pUnit:GetDomainType() ~= DomainTypes.DOMAIN_AIR then
		iRangedStrength = pUnit:GetBaseRangedCombatStrength();
	else
		iRangedStrength = 0;
	end
	if iRangedStrength > 0 then
		local sRangedStrength = iRangedStrength .. " [ICON_RANGE_STRENGTH]";
		Controls.UnitRangedAttackBox:SetHide(false);
		Controls.UnitStatRangedAttack:SetText(sRangedStrength);
	else
		Controls.UnitRangedAttackBox:SetHide(true);
	end
end

--------------------------------------------------------------------------------
-- Format combat modifier
--------------------------------------------------------------------------------
function GetFormattedText(iValue, bForMe, bPercent, sOptionalColor)
	if not iValue then
		return "";
	end

	local sNumberPart = Locale.ToNumber(iValue, "#.#");

	if bPercent then
		sNumberPart = sNumberPart .. "%";
	end

	local sPrefix;
	local sSuffix = "[ENDCOLOR]";
	if sOptionalColor then
		sPrefix = sOptionalColor;
	elseif bForMe then
		if iValue > 0 then
			sPrefix = "[COLOR_POSITIVE_TEXT]+";
		elseif iValue < 0 then
			sPrefix = "[COLOR_NEGATIVE_TEXT]";
		end
	else
		if iValue < 0 then
			sPrefix = "[COLOR_POSITIVE_TEXT]";
		elseif iValue > 0 then
			sPrefix = "[COLOR_NEGATIVE_TEXT]+";
		end
	end

	local sResult;
	if sPrefix then
		sResult = sPrefix .. sNumberPart .. sSuffix;
	end

	if bForMe then
		sResult = " :  " .. sResult .. "[]";
	else
		sResult = "[]" .. sResult .. "  : ";
	end

	return sResult;
end

--------------------------------------------------------------------------------
-- Add row to combat simulator, or add to misc. modifier
-- Return total number of modifiers and the sum of misc. modifiers
-- optionalParam can be nil, boolean, number or string
--------------------------------------------------------------------------------
function ProcessModifier(iModifier, sTextKey, nBonus, iMiscModifier, bForMe, bPercent, sOptionalColor, optionalParam)
	if not iModifier or iModifier ~= 0 then
		if nBonus < g_iMaxBonusDisplay then
			local controlTable;
			if bForMe then
				controlTable = g_MyCombatDataIM:GetInstance();
			else
				controlTable = g_TheirCombatDataIM:GetInstance();
			end
			if optionalParam then
				controlTable.Text:LocalizeAndSetText(sTextKey, optionalParam);
			else
				controlTable.Text:LocalizeAndSetText(sTextKey);
			end
			controlTable.Value:SetText(GetFormattedText(iModifier, bForMe, bPercent, sOptionalColor));
		elseif iModifier then
			iMiscModifier = iMiscModifier + iModifier;
		end
		nBonus = nBonus + 1;
	end
	return nBonus, iMiscModifier;
end

--------------------------------------------------------------------------------
-- Common function for combat simulator (unit vs unit, city vs unit, unit vs city)
-- This assumes pMyUnit and pTheirUnit cannot be both nil
--------------------------------------------------------------------------------
function UpdateCombatSimulator(pMyUnit, pTheirUnit, pMyCity, pTheirCity)
	g_MyCombatDataIM:ResetInstances();
	g_TheirCombatDataIM:ResetInstances();

	local eMyPlayer, eTheirPlayer;
	local pMyPlayer, pTheirPlayer;
	local iMyStrength, iTheirStrength;
	local iMyMaxHP, iTheirMaxHP;
	local iMyDamage, iTheirDamage;
	local pFromPlot, pToPlot;

	local bRanged = false;

	local iMyDamageInflicted = 0;
	local iTheirDamageInflicted = 0;
	local iMyRangedSupportDamageInflicted = 0;
	local iWithdrawChance = 0;
	local iNumVisibleAAUnits = 0;
	local bInterceptPossible = false;

	local controlTable;
	local iModifier;
	local nBonus = 0; -- counting how many bonuses the unit/city has
	local iMiscModifier = 0; -- where all the extra bonuses are added
	local sDescription;

	if pTheirUnit then
		eTheirPlayer = pTheirUnit:GetOwner();
		pToPlot = pTheirUnit:GetPlot();
		iTheirMaxHP = pTheirUnit:GetMaxHitPoints();
		iTheirDamage = pTheirUnit:GetDamage();
	elseif pTheirCity then
		eTheirPlayer = pTheirCity:GetOwner();
		pToPlot = pTheirCity:Plot();
		iTheirStrength = pTheirCity:GetStrengthValue(false, pMyUnit:IgnoreBuildingDefense());
		iTheirMaxHP = pTheirCity:GetMaxHitPoints();
		iTheirDamage = pTheirCity:GetDamage();
	end

	local pBattlePlot = pToPlot;

	if pMyUnit then
		eMyPlayer = pMyUnit:GetOwner();
		pFromPlot = pMyUnit:GetPlot();
		iMyMaxHP = pMyUnit:GetMaxHitPoints();
		iMyDamage = pMyUnit:GetDamage();
		if pMyUnit:IsCanAttackRanged() then
			-- Ranged unit
			iMyStrength = pMyUnit:GetMaxRangedCombatStrength(pTheirUnit, pTheirCity, true);
			bRanged = true;
			pBattlePlot = pFromPlot;
		else
			-- Melee unit
			pFromPlot = pMyUnit:GetMeleeAttackFromPlot(pToPlot);
			iMyStrength = pMyUnit:GetMaxAttackStrength(pFromPlot, pToPlot, pTheirUnit);
		end
	elseif pMyCity then
		bRanged = true;
		eMyPlayer = pMyCity:GetOwner();
		pFromPlot = pMyCity:Plot();
		iMyMaxHP = pMyCity:GetMaxHitPoints();
		iMyDamage = pMyCity:GetDamage();
		iMyStrength = pMyCity:GetStrengthValue(true, false, pTheirUnit);
		iTheirStrength = pMyCity:RangeCombatUnitDefense(pTheirUnit);
		iMyDamageInflicted = pMyCity:RangeCombatDamage(pTheirUnit);
	end

	-- Skip everything if my strength isn't positive (can't attack!)
	if iMyStrength <= 0 then
		RecalculateSize();
		return;
	end

	pMyPlayer = Players[eMyPlayer];
	pTheirPlayer = Players[eTheirPlayer];

	-- Highlight the combat plots
	if pMyUnit then
		local v2FromHex = ToHexFromGrid(Vector2(pFromPlot:GetX(), pFromPlot:GetY()));
		Events.SerialEventHexHighlight(v2FromHex, true, Vector4(0.7, 0, 0, 1), "ValidFireTargetBorder");

		local v2ToHex = ToHexFromGrid(Vector2(pToPlot:GetX(), pToPlot:GetY()));
		Events.SerialEventHexHighlight(v2ToHex, true, Vector4(0.7, 0, 0, 1), "ValidFireTargetBorder");
	end

	-- For unit vs unit/city, we do combat estimation here
	if pMyUnit then
		if bRanged then
			-- Different units use different values to defend against ranged
			if pTheirUnit then
				if pTheirUnit:IsEmbarked() then
					iTheirStrength = pTheirUnit:GetEmbarkedUnitDefense();
				elseif pTheirUnit:IsCanAttackRanged() then
					iTheirStrength = pTheirUnit:GetMaxRangedCombatStrength(pMyUnit, nil, false);
				else
					iTheirStrength = pTheirUnit:GetMaxDefenseStrength(pToPlot, pMyUnit, pFromPlot, true);
				end
			end

			iMyDamageInflicted = pMyUnit:GetRangeCombatDamage(pTheirUnit, pTheirCity, false);

			-- Air strike is ranged but the attacker takes damage
			if pMyUnit:GetDomainType() == DomainTypes.DOMAIN_AIR then
				if pMyUnit:GetUnitAIType() == GameInfoTypes.UNITAI_MISSILE_AIR then
					-- Suicide missile attack
					iTheirDamageInflicted = pMyUnit:GetCurrHitPoints();
				else
					-- Regular air attack
					if pTheirUnit then
						iTheirDamageInflicted = pTheirUnit:GetAirStrikeDefenseDamage(pMyUnit, false);
					elseif pTheirCity then
						iTheirDamageInflicted = pTheirCity:GetAirStrikeDefenseDamage(pMyUnit, false);
					end
				end
				iNumVisibleAAUnits = pMyUnit:GetInterceptorCount(pToPlot, pTheirUnit, false, true);
				bInterceptPossible = true;
			end
		else
			-- Normal melee combat
			if pTheirUnit then
				if pMyUnit:IsRangedSupportFire() then
					-- This already accounts for halved base combat strength
					iMyRangedSupportDamageInflicted = pMyUnit:GetRangeCombatDamage(pTheirUnit, nil, false);
				end

				-- This already checks for embarkation
				iTheirStrength = pTheirUnit:GetMaxDefenseStrength(pToPlot, pMyUnit, pFromPlot, false, iMyRangedSupportDamageInflicted);

				iWithdrawChance = pTheirUnit:GetWithdrawChance(pMyUnit);
			end

			iMyDamageInflicted = pMyUnit:GetCombatDamage(iMyStrength, iTheirStrength, nil, false, false, not not pTheirCity, pTheirCity);
			iMyDamageInflicted = iMyDamageInflicted + iMyRangedSupportDamageInflicted;

			-- Embarked unit cannot deal damage
			if not (pTheirUnit and pTheirUnit:IsEmbarked()) then
				iTheirDamageInflicted = pMyUnit:GetCombatDamage(iTheirStrength, iMyStrength, nil, false, not not pTheirCity, false);
			end
		end
	end

	-- Update health bars
	DoUpdateHealthBars(iMyMaxHP, iTheirMaxHP, iMyDamage, iTheirDamage, iMyDamageInflicted, iTheirDamageInflicted);

	-------------------------
	-- Combat prediction
	-------------------------

	-- Hide everything first
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

	if pMyUnit and pTheirUnit then
		local eCombatPrediction;
		if pTheirUnit:IsEmbarked() then
			eCombatPrediction = CombatPredictionTypes.COMBAT_PREDICTION_TOTAL_VICTORY;
		else
			eCombatPrediction = Game.GetCombatPrediction(pMyUnit, pTheirUnit);
		end

		if eCombatPrediction == CombatPredictionTypes.COMBAT_PREDICTION_RANGED then
			Controls.RangedAttackIndicator:SetHide(false);
			Controls.RangedAttackButtonLabel:SetText(Locale.ToUpper(Locale.ConvertTextKey("TXT_KEY_INTERFACEMODE_RANGE_ATTACK")));
		elseif eCombatPrediction == CombatPredictionTypes.COMBAT_PREDICTION_STALEMATE then
			Controls.StalemateIndicator:SetHide(false);
		elseif eCombatPrediction == CombatPredictionTypes.COMBAT_PREDICTION_TOTAL_DEFEAT then
			Controls.TotalDefeatIndicator:SetHide(false);
		elseif eCombatPrediction == CombatPredictionTypes.COMBAT_PREDICTION_TOTAL_VICTORY then
			Controls.TotalVictoryIndicator:SetHide(false);
		elseif eCombatPrediction == CombatPredictionTypes.COMBAT_PREDICTION_MAJOR_VICTORY then
			Controls.MajorVictoryIndicator:SetHide(false);
		elseif eCombatPrediction == CombatPredictionTypes.COMBAT_PREDICTION_SMALL_VICTORY then
			Controls.SmallVictoryIndicator:SetHide(false);
		elseif eCombatPrediction == CombatPredictionTypes.COMBAT_PREDICTION_MAJOR_DEFEAT then
			Controls.MajorDefeatIndicator:SetHide(false);
		elseif eCombatPrediction == CombatPredictionTypes.COMBAT_PREDICTION_SMALL_DEFEAT then
			Controls.SmallDefeatIndicator:SetHide(false);
		else
			Controls.StalemateIndicator:SetHide(false);
		end
	elseif bRanged then
		Controls.RangedAttackIndicator:SetHide(false);
		Controls.RangedAttackButtonLabel:SetText(Locale.ToUpper(Locale.ConvertTextKey("TXT_KEY_INTERFACEMODE_RANGE_ATTACK")));
	else
		-- Unit melee attacking a city
		if iMyDamage > iMyMaxHP / 2 and iTheirDamageInflicted > 0 then
			-- Our unit is weak (less than half health)
			Controls.RiskyAttackIndicator:SetHide(false);
		elseif iTheirDamageInflicted >= iMyDamageInflicted then
			-- They are doing at least as much damage to us as we're doing to them
			Controls.RiskyAttackIndicator:SetHide(false);
		else
			-- Safe (?) attack
			Controls.SafeAttackIndicator:SetHide(false);
		end
	end

	-------------------------
	-- My side
	-------------------------

	-- My Damage
	Controls.MyDamageValue:SetText("[COLOR_GREEN]" .. iMyDamageInflicted .. "[ENDCOLOR]");

	-- My Strength
	Controls.MyStrengthValue:SetText(Locale.ToNumber(iMyStrength / 100, "#.#"));

	if pMyCity then

		-- Garrison bonus
		if pMyCity:GetGarrisonedUnit() then
			iModifier = pMyPlayer:GetGarrisonedCityRangeStrikeModifier();
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_GARRISONED_CITY_RANGE_BONUS", nBonus, iMiscModifier, true, true);
		end

		-- Counterspy bonus
		iModifier = pMyCity:GetRangeStrikeModifierFromEspionage();
		nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_COUNTERSPY_DEFENSE", nBonus, iMiscModifier, true, true);

		-- Trait attack bonus against the same target
		iModifier = pMyCity:GetMultiAttackBonusCity(pTheirUnit);
		nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_MULTI_ATTACK_BONUS", nBonus, iMiscModifier, true, true);

		-- Religion bonus
		iModifier = pMyCity:GetReligionCityRangeStrikeModifier();
		nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_RELIGIOUS_BELIEF", nBonus, iMiscModifier, true, true);

		-- VS barbarian modifier
		if pTheirUnit:IsBarbarian() then
			iModifier = pMyPlayer:GetBarbarianCombatBonus();
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_VS_BARBARIANS_BONUS", nBonus, iMiscModifier, true, true);
		end

	elseif pMyUnit then

		if pTheirUnit then
			-- Ranged Support Fire
			if pMyUnit:IsRangedSupportFire() then
				nBonus = ProcessModifier(nil, "TXT_KEY_EUPANEL_RANGED_SUPPORT", nBonus, nil, true);
			end

			-- Plague (only applied if melee attacking a melee unit; don't add to misc. bonus)
			local sPlague, iChance;
			if not bRanged and not pTheirUnit:IsCanAttackRanged() then
				sPlague, iChance = pMyUnit:GetMovementRules(pTheirUnit);
				if sPlague ~= "" then
					if iChance >= 0 then
						nBonus = ProcessModifier(iChance, sPlague, nBonus, nil, true, true, "[COLOR_CYAN]");
					else
						nBonus = ProcessModifier(nil, sPlague, nBonus, nil, true);
					end
				end
			end

			-- Ignore Zone of Control
			local sZOC = pMyUnit:GetZOCStatus();
			if sZOC ~= "" then
				nBonus = ProcessModifier(nil, sZOC, nBonus, nil, true);
			end
		end

		-- Damaged unit
		iModifier = pMyUnit:GetDamageCombatModifier();
		nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_UNITCOMBAT_DAMAGE_MODIFIER", nBonus, iMiscModifier, true, true);

		-- Empire unhappy
		iModifier = pMyUnit:GetUnhappinessCombatPenalty();
		if pMyPlayer:IsEmpireVeryUnhappy() then
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_EMPIRE_VERY_UNHAPPY_PENALTY", nBonus, iMiscModifier, true, true);
		elseif pMyPlayer:IsEmpireUnhappy() then
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_EMPIRE_UNHAPPY_PENALTY", nBonus, iMiscModifier, true, true);
		end

		-- Anti-warmonger fervor
		if pTheirUnit then
			iModifier = pMyUnit:GetResistancePower(pTheirUnit);
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_RESISTANCE_POWER", nBonus, iMiscModifier, true, true);
		end

		-- Lacking strategic resources
		iModifier = pMyUnit:GetStrategicResourceCombatPenalty();
		nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_STRATEGIC_RESOURCE", nBonus, iMiscModifier, true, true);

		-- Great General bonus
		if pMyUnit:IsNearGreatGeneral() then
			iModifier = pMyPlayer:GetGreatGeneralCombatBonus() + pMyPlayer:GetTraitGreatGeneralExtraBonus() + pMyUnit:GetGreatGeneralAuraBonus();

			if pMyUnit:GetDomainType() == DomainTypes.DOMAIN_LAND then
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_GG_NEAR", nBonus, iMiscModifier, true, true);
			elseif pMyUnit:GetDomainType() == DomainTypes.DOMAIN_SEA then
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_GA_NEAR", nBonus, iMiscModifier, true, true);
			end

			-- Ignoring Great General aura (extra row)
			if pMyUnit:IsIgnoreGreatGeneralBenefit() then
				nBonus, iMiscModifier = ProcessModifier(-iModifier, "TXT_KEY_EUPANEL_IGG", nBonus, iMiscModifier, true, true);
			end
		end

		-- Great General stacking bonus
		if pMyUnit:IsStackedGreatGeneral() then
			iModifier = pMyUnit:GetGreatGeneralCombatModifier();
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_GG_STACKED", nBonus, iMiscModifier, true, true);
		end

		-- Near terrifying opponent
		iModifier = pMyUnit:GetReverseGreatGeneralModifier();
		nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_REVERSE_GG_NEAR", nBonus, iMiscModifier, true, true);

		-- Attacking blockaded city
		if pTheirCity and pTheirCity:IsBlockadedTest() then
			iModifier = GameDefines.BLOCKADED_CITY_ATTACK_MODIFIER;
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_CITY_BLOCKADED", nBonus, iMiscModifier, true, true);
		end

		-- Policy attack bonus
		if not bRanged then
			local iTurns = pMyPlayer:GetAttackBonusTurns();
			if iTurns > 0 then
				iModifier = GameDefines.POLICY_ATTACK_BONUS_MOD;
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_POLICY_ATTACK_BONUS", nBonus, iMiscModifier, true, true, nil, iTurns);
			end
		end

		-- Near Capital
		iModifier = pMyUnit:CapitalDefenseModifier();
		if iModifier > 0 then
			-- Compute distance to Capital
			local pCapital = pMyPlayer:GetCapitalCity();
			if pCapital then
				local iDistance = Map.PlotDistance(pCapital:GetX(), pCapital:GetY(), pBattlePlot:GetX(), pBattlePlot:GetY());
				iModifier = iModifier + iDistance * pMyUnit:CapitalDefenseFalloff();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_CAPITAL_DEFENSE_BONUS", nBonus, iMiscModifier, true, true);
			end
		end

		-- Attack bonus against the same target
		if pTheirUnit then
			iModifier = pMyUnit:GetMultiAttackBonus(pTheirUnit);
		elseif pTheirCity then
			iModifier = pMyUnit:GetMultiAttackBonusCity(pTheirCity);
		end
		nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_MULTI_ATTACK_BONUS", nBonus, iMiscModifier, true, true);

		-- Trait VS major civ city bonus during Golden Age
		if pTheirCity then
			iModifier = pMyPlayer:GetTraitConquestOfTheWorldCityAttackMod(pToPlot);
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_CONQUEST_OF_WORLD", nBonus, iMiscModifier, true, true);
		end

		-- Trait bonus during Golden Age
		if pMyPlayer:IsGoldenAge() then
			iModifier = pMyPlayer:GetTraitGoldenAgeCombatModifier();
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_GOLDEN_AGE", nBonus, iMiscModifier, true, true);
		end

		-- Trait attack bonus against City States
		if pTheirPlayer:IsMinorCiv() then
			iModifier = pMyPlayer:GetTraitCityStateCombatModifier();
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_CITY_STATE", nBonus, iMiscModifier, true, true);
		end

		-- Trait bonus for allying with City States
		iModifier = pMyUnit:GetAllianceCSStrength();
		nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ATTACK_CS_ALLIANCE_STRENGTH", nBonus, iMiscModifier, true, true);

		if pTheirUnit then
			-- Trait bonus against more advanced enemy in owned territory
			if pBattlePlot:GetOwner() == eMyPlayer and pTheirUnit:IsHigherTechThan(pMyUnit:GetUnitType()) then
				iModifier = pMyPlayer:GetCombatBonusVsHigherTech();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_TRAIT_LOW_TECH_BONUS", nBonus, iMiscModifier, true, true);
			end

			-- Trait bonus against enemy with more cities
			if pTheirUnit:IsLargerCivThan(pMyUnit) then
				iModifier = pMyPlayer:GetCombatBonusVsLargerCiv();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_TRAIT_SMALL_SIZE_BONUS", nBonus, iMiscModifier, true, true);
			end
		end

		-- Monopoly attack bonus
		iModifier = pMyUnit:GetMonopolyAttackBonus();
		nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_MONOPOLY_POWER_ATTACK", nBonus, iMiscModifier, true, true);

		-- Flanking bonus
		if pTheirUnit then
			iModifier = 0;
			if not bRanged then
				iModifier = pFromPlot:GetEffectiveFlankingBonus(pMyUnit, pTheirUnit, pToPlot);
			elseif bRanged and pMyUnit:IsRangedFlankAttack() then
				iModifier = pToPlot:GetEffectiveFlankingBonusAtRange(pMyUnit, pTheirUnit);
			end
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_FLANKING_BONUS", nBonus, iMiscModifier, true, true);
		end

		-- Fighting in friendly lands (technically still possible when attacking a city, if ranged)
		if pBattlePlot:IsFriendlyTerritory(eMyPlayer) then
			-- General modifier
			iModifier = pMyUnit:GetFriendlyLandsModifier();
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_FIGHT_AT_HOME_BONUS", nBonus, iMiscModifier, true, true);

			-- Attack modifier
			iModifier = pMyUnit:GetFriendlyLandsAttackModifier();
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ATTACK_IN_FRIEND_LANDS", nBonus, iMiscModifier, true, true);

			-- Religion modifier
			iModifier = pMyPlayer:GetFoundedReligionFriendlyCityCombatMod(pBattlePlot);
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_FRIENDLY_CITY_BELIEF_BONUS", nBonus, iMiscModifier, true, true);
		else
			-- General modifier
			iModifier = pMyUnit:GetOutsideFriendlyLandsModifier();
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_OUTSIDE_HOME_BONUS", nBonus, iMiscModifier, true, true);

			-- Religion modifier
			iModifier = pMyPlayer:GetFoundedReligionEnemyCityCombatMod(pBattlePlot);
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ENEMY_CITY_BELIEF_BONUS", nBonus, iMiscModifier, true, true);
		end

		if pTheirUnit then
			-- Target is in friendly lands (+ following a different religion)
			iModifier = pMyUnit:GetCombatVersusOtherReligionOwnLands(pTheirUnit);
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_FRIENDLY_CITY_BELIEF_BONUS_CBP", nBonus, iMiscModifier, true, true);

			-- Target is in lands friendly to them (+ following a different religion)
			iModifier = pMyUnit:GetCombatVersusOtherReligionTheirLands(pTheirUnit);
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ENEMY_CITY_BELIEF_BONUS_CBP", nBonus, iMiscModifier, true, true);
		end

		if pTheirCity then
			-- City attack bonus
			iModifier = pMyUnit:CityAttackModifier();
			if iModifier > 0 then
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ATTACK_CITIES", nBonus, iMiscModifier, true, true);
			else
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ATTACK_CITIES_PENALTY", nBonus, iMiscModifier, true, true);
			end

			-- Sapper unit modifier
			iModifier = pMyUnit:GetSapperAreaEffectBonus(pTheirCity);
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_CITY_SAPPED", nBonus, iMiscModifier, true, true);
		end

		-- Adjacent to X unit combat modifier
		iModifier = pMyUnit:PerAdjacentUnitCombatModifier() + pMyUnit:PerAdjacentUnitCombatAttackMod();
		nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_PER_ADJACENT_UNIT_COMBAT", nBonus, iMiscModifier, true, true);

		if not bRanged then
			-- Attacking across a River
			if not pMyUnit:IsRiverCrossingNoPenalty() and pFromPlot:IsRiverCrossingToPlot(pToPlot) then
				iModifier = GameDefines.RIVER_ATTACK_MODIFIER;
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ATTACK_OVER_RIVER", nBonus, iMiscModifier, true, true);
			end

			-- Amphibious landing
			if not pMyUnit:IsAmphib() and not pToPlot:IsWater() and pFromPlot:IsWater() and pMyUnit:GetDomainType() == DomainTypes.DOMAIN_LAND then
				iModifier = GameDefines.AMPHIB_ATTACK_MODIFIER;
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_AMPHIBIOUS_ATTACK", nBonus, iMiscModifier, true, true);
			end
		end

		-- Nearby unit modifier
		iModifier = pMyUnit:GetGiveCombatModToUnit();
		nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_NEARBYPROMOTION_COMBAT_BONUS", nBonus, iMiscModifier, true, true);

		-- Near friendly/hostile/any city modifier
		iModifier = pMyUnit:GetNearbyCityBonusCombatMod();
		nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_NEARBYPROMOTION_CITY_COMBAT_BONUS", nBonus, iMiscModifier, true, true);

		-- Near improvement modifier
		iModifier = pMyUnit:GetNearbyImprovementModifier(pFromPlot);
		nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_IMPROVEMENT_NEAR", nBonus, iMiscModifier, true, true);

		-- Near unit class modifier
		iModifier = pMyUnit:GetNearbyUnitClassModifierFromUnitClass(pFromPlot);
		nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_UNITCLASS_NEAR", nBonus, iMiscModifier, true, true);

		-- Adjacent to owned unit modifier
		if pMyUnit:IsFriendlyUnitAdjacent(true) then
			iModifier = pMyUnit:GetAdjacentModifier();
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ADJACENT_FRIEND_UNIT_BONUS", nBonus, iMiscModifier, true, true);
		else
			iModifier = pMyUnit:GetNoAdjacentUnitModifier();
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_NO_FRIENDLY_UNIT_ADJACENT_BONUS", nBonus, iMiscModifier, true, true);
		end

		if pTheirUnit then
			-- VS unit class modifier
			local eTheirUnitClass = pTheirUnit:GetUnitClassType();
			iModifier = pMyUnit:GetUnitClassModifier(eTheirUnitClass) + pMyUnit:UnitClassAttackModifier(eTheirUnitClass);
			sDescription = Locale.ConvertTextKey(GameInfo.UnitClasses[eTheirUnitClass].Description);
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_VS_CLASS", nBonus, iMiscModifier, true, true, nil, sDescription);

			-- VS unit combat modifier
			local eTheirUnitCombat = pTheirUnit:GetUnitCombatType();
			if eTheirUnitCombat ~= -1 then
				iModifier = pMyUnit:UnitCombatModifier(eTheirUnitCombat);
				sDescription = Locale.ConvertTextKey(GameInfo.UnitCombatInfos[eTheirUnitCombat].Description);
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_VS_CLASS", nBonus, iMiscModifier, true, true, nil, sDescription);

				-- Skirmishers count as both Archery and Mounted Units
				if pTheirUnit:IsMounted() then
					iModifier = pMyUnit:UnitCombatModifier(GameInfoTypes.UNITCOMBAT_MOUNTED);
					sDescription = Locale.ConvertTextKey(GameInfo.UnitCombatInfos.UNITCOMBAT_MOUNTED.Description);
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_VS_CLASS", nBonus, iMiscModifier, true, true, nil, sDescription);
				end
			end

			-- VS domain modifier
			iModifier = pMyUnit:DomainModifier(pTheirUnit:GetDomainType());
			if iModifier > 0 then
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_VS_DOMAIN", nBonus, iMiscModifier, true, true);
			else
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_PENALTY_VS_DOMAIN", nBonus, iMiscModifier, true, true);
			end

			-- VS domain attack modifier
			iModifier = pMyUnit:DomainAttackPercent(pTheirUnit:GetDomainType());
			if iModifier > 0 then
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ATTACK_BONUS_VS_DOMAIN", nBonus, iMiscModifier, true, true);
			else
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ATTACK_PENALTY_VS_DOMAIN", nBonus, iMiscModifier, true, true);
			end

			-- VS barbarian modifier
			if pTheirUnit:IsBarbarian() then
				iModifier = pMyPlayer:GetBarbarianCombatBonus() + pMyUnit:BarbarianCombatBonus();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_VS_BARBARIANS_BONUS", nBonus, iMiscModifier, true, true);
			end

			-- Target is fortified
			if pTheirUnit:GetFortifyTurns() > 0 then
				iModifier = pMyUnit:AttackFortifiedModifier();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_VS_FORT_UNITS", nBonus, iMiscModifier, true, true);
			end

			if pTheirUnit:GetDamage() > 0 then
				-- Target is wounded
				iModifier = pMyUnit:AttackWoundedModifier();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_VS_WOUND_UNITS", nBonus, iMiscModifier, true, true);
			else
				-- Target is at full health
				iModifier = pMyUnit:AttackFullyHealedModifier();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_VS_FULLY_HEALED_UNITS", nBonus, iMiscModifier, true, true);
			end

			if pTheirUnit:GetDamage() < iTheirMaxHP / 2 then
				-- Target is above half health
				iModifier = pMyUnit:AttackAbove50Modifier();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_VS_MORE_50_HP_UNITS", nBonus, iMiscModifier, true, true);
			else
				-- Target is at or below half health
				iModifier = pMyUnit:AttackBelow50Modifier();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_VS_LESS_50_HP_UNITS", nBonus, iMiscModifier, true, true);
			end

			-- Target is on open terrain
			if pToPlot:IsOpenGround() then
				if not bRanged then
					iModifier = pMyUnit:OpenAttackModifier();
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_OPEN_TERRAIN_BONUS", nBonus, iMiscModifier, true, true);
				else
					iModifier = pMyUnit:OpenRangedAttackModifier();
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_OPEN_TERRAIN_RANGE_BONUS", nBonus, iMiscModifier, true, true);
				end
			end

			-- Target is on rough terrain
			if pToPlot:IsRoughGround() then
				if not bRanged then
					iModifier = pMyUnit:RoughAttackModifier();
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ROUGH_TERRAIN_BONUS", nBonus, iMiscModifier, true, true);
				else
					iModifier = pMyUnit:RoughRangedAttackModifier();
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ROUGH_TERRAIN_RANGED_BONUS", nBonus, iMiscModifier, true, true);
				end
			end

			-- Target is on hill
			if pToPlot:IsHills() then
				iModifier = pMyUnit:HillsAttackModifier();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_HILL_ATTACK_BONUS", nBonus, iMiscModifier, true, true);
			end

			local eToFeature = pToPlot:GetFeatureType();
			local eToTerrain = pToPlot:GetTerrainType();
			if eToFeature ~= FeatureTypes.NO_FEATURE then
				-- Target is on featured tile
				iModifier = pMyUnit:FeatureAttackModifier(eToFeature);
				sDescription = Locale.ConvertTextKey(GameInfo.Features[eToFeature].Description);
			else
				-- Target is on featureless tile
				iModifier = pMyUnit:TerrainAttackModifier(eToTerrain);
				sDescription = Locale.ConvertTextKey(GameInfo.Terrains[eToTerrain].Description);
			end
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ATTACK_INTO_BONUS", nBonus, iMiscModifier, true, true, nil, sDescription);

			-- Target is on featureless hill
			if eToFeature == FeatureTypes.NO_FEATURE and pToPlot:IsHills() then
				iModifier = pMyUnit:TerrainAttackModifier(GameInfoTypes.TERRAIN_HILL);
				sDescription = Locale.ConvertTextKey(GameInfo.Terrains.TERRAIN_HILL.Description);
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ATTACK_INTO_BONUS", nBonus, iMiscModifier, true, true, nil, sDescription);
			end
		end

		-- Fighting in rough/open terrain
		if pBattlePlot:IsRoughGround() then
			iModifier = pMyUnit:RoughFromModifier();
		else
			iModifier = pMyUnit:OpenFromModifier();
		end
		nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_FROM_TERRAIN_BONUS", nBonus, iMiscModifier, true, true);

		-- Ranged attack modifier
		if bRanged then
			iModifier = pMyUnit:GetRangedAttackModifier();
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_RANGED_ATTACK_MODIFIER", nBonus, iMiscModifier, true, true);
			
			iModifier = pMyUnit:GetGarrisonRangedAttackModifier();
			nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_GARRISON_RANGED_MODIFIER", nBonus, iMiscModifier, true, true);
		end

		-- Generic promotion attack modifier
		iModifier = pMyUnit:GetAttackModifier();
		nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ATTACK_MOD_BONUS", nBonus, iMiscModifier, true, true);

		-- Generic promotion modifier + difficulty modifier
		iModifier = pMyUnit:GetExtraCombatPercent();
		nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_EXTRA_PERCENT", nBonus, iMiscModifier, true, true);
	end

	-- Display misc. bonus here if there are more than 4 bonuses
	if nBonus > g_iMaxBonusDisplay then
		controlTable = g_MyCombatDataIM:GetInstance();
		controlTable.Text:LocalizeAndSetText("TXT_KEY_MISC_BONUS");
		controlTable.Value:SetText(GetFormattedText(iMiscModifier, true, true));
	end

	-------------------------
	-- Their side
	-------------------------

	-- Their Damage
	Controls.TheirDamageValue:SetText("[COLOR_RED]" .. iTheirDamageInflicted .. "[ENDCOLOR]");

	-- Their Strength
	Controls.TheirStrengthValue:SetText(Locale.ToNumber(iTheirStrength / 100, "#.#"));

	-- Reset for their bonuses
	nBonus = 0;
	iMiscModifier = 0;

	-- Air interception
	if bInterceptPossible then
		nBonus = ProcessModifier(nil, "TXT_KEY_EUPANEL_AIR_INTERCEPT_WARNING1", nBonus, nil, false);
		nBonus = ProcessModifier(nil, "TXT_KEY_EUPANEL_AIR_INTERCEPT_WARNING2", nBonus, nil, false);
	end
	if iNumVisibleAAUnits > 0 then
		nBonus = ProcessModifier(nil, "TXT_KEY_EUPANEL_VISIBLE_AA_UNITS", nBonus, nil, false, false, nil, iNumVisibleAAUnits);
	end

	if pTheirUnit then
		-- Plague (don't add to misc. bonus)
		if pMyUnit then
			local sPlague, iChance = pTheirUnit:GetMovementRules(pMyUnit);
			if sPlague ~= "" then
				if iChance >= 0 then
					nBonus = ProcessModifier(iChance, sPlague, nBonus, nil, false, true, "[COLOR_CYAN]");
				else
					nBonus = ProcessModifier(nil, sPlague, nBonus, nil, false);
				end
			end
		end

		if not bRanged then
			-- Withdraw chance (don't add to misc. bonus)
			if iWithdrawChance == 100 then
				nBonus = ProcessModifier(nil, "TXT_KEY_EUPANEL_WITHDRAW_CHANCE_100_PERCENT", nBonus, nil, false);
			elseif iWithdrawChance > 0 then
				nBonus = ProcessModifier(iWithdrawChance, "TXT_KEY_EUPANEL_WITHDRAW_CHANCE", nBonus, nil, false, true, "[COLOR_CYAN]");
			end
		end

		-- Below modifiers only apply for combat units (air units also get modifiers but you can't directly attack them)
		if pTheirUnit:IsCombatUnit() then
			if pTheirUnit:IsEmbarked() then
				-- Embarked unit only has one possible combat strength modifier
				iModifier = pTheirUnit:GetEmbarkDefensiveModifier();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_EMBARKATION_DEFENSE", nBonus, iMiscModifier, false, true);
			else
				-- Capture chance (don't add to misc. bonus)
				if not bRanged and iWithdrawChance < 100 then
					local iChance = pMyUnit:GetCaptureChance(pTheirUnit);
					if iChance == 100 then
						nBonus = ProcessModifier(nil, "TXT_KEY_EUPANEL_CAPTURE_CHANCE_100_PERCENT", nBonus, nil, false);
					elseif iChance > 0 then
						nBonus = ProcessModifier(iChance, "TXT_KEY_EUPANEL_CAPTURE_CHANCE", nBonus, nil, false, true, "[COLOR_CYAN]");
					end
				end

				-- Damaged unit
				iModifier = pTheirUnit:GetDamageCombatModifier(bRanged);
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_UNITCOMBAT_DAMAGE_MODIFIER", nBonus, iMiscModifier, false, true);

				-- Empire unhappy
				iModifier = pTheirUnit:GetUnhappinessCombatPenalty();
				if pTheirPlayer:IsEmpireVeryUnhappy() then
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_EMPIRE_VERY_UNHAPPY_PENALTY", nBonus, iMiscModifier, false, true);
				elseif pTheirPlayer:IsEmpireUnhappy() then
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_EMPIRE_UNHAPPY_PENALTY", nBonus, iMiscModifier, false, true);
				end

				-- Anti-warmonger fervor
				if pMyUnit then
					iModifier = pTheirUnit:GetResistancePower(pMyUnit);
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_RESISTANCE_POWER", nBonus, iMiscModifier, false, true);
				end

				-- Lacking strategic resources
				iModifier = pTheirUnit:GetStrategicResourceCombatPenalty();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_STRATEGIC_RESOURCE", nBonus, iMiscModifier, false, true);

				-- Great General bonus
				if pTheirUnit:IsNearGreatGeneral() then
					iModifier = pTheirPlayer:GetGreatGeneralCombatBonus() + pTheirPlayer:GetTraitGreatGeneralExtraBonus() + pTheirUnit:GetGreatGeneralAuraBonus();

					if pTheirUnit:GetDomainType() == DomainTypes.DOMAIN_LAND then
						nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_GG_NEAR", nBonus, iMiscModifier, false, true);
					elseif pTheirUnit:GetDomainType() == DomainTypes.DOMAIN_SEA then
						nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_GA_NEAR", nBonus, iMiscModifier, false, true);
					end

					-- Ignoring Great General aura (extra row)
					if pTheirUnit:IsIgnoreGreatGeneralBenefit() then
						nBonus, iMiscModifier = ProcessModifier(-iModifier, "TXT_KEY_EUPANEL_IGG", nBonus, iMiscModifier, false, true);
					end
				end

				-- Great General stacking bonus
				if pTheirUnit:IsStackedGreatGeneral() then
					iModifier = pTheirUnit:GetGreatGeneralCombatModifier();
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_GG_STACKED", nBonus, iMiscModifier, false, true);
				end

				-- Near terrifying opponent
				iModifier = pTheirUnit:GetReverseGreatGeneralModifier();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_REVERSE_GG_NEAR", nBonus, iMiscModifier, false, true);

				-- Near Capital
				iModifier = pTheirUnit:CapitalDefenseModifier();
				if iModifier > 0 then
					-- Compute distance to Capital
					local pCapital = pTheirPlayer:GetCapitalCity();
					if pCapital then
						local iDistance = Map.PlotDistance(pCapital:GetX(), pCapital:GetY(), pTheirUnit:GetX(), pTheirUnit:GetY());
						iModifier = iModifier + iDistance * pTheirUnit:CapitalDefenseFalloff();
						nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_CAPITAL_DEFENSE_BONUS", nBonus, iMiscModifier, false, true);
					end
				end

				-- Trait bonus during Golden Age
				if pTheirPlayer:IsGoldenAge() then
					iModifier = pTheirPlayer:GetTraitGoldenAgeCombatModifier();
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_GOLDEN_AGE", nBonus, iMiscModifier, false, true);
				end

				-- Trait VS major civ city bonus during Golden Age
				if pMyCity then
					iModifier = pTheirPlayer:GetTraitConquestOfTheWorldCityAttackMod(pFromPlot);
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_CONQUEST_OF_WORLD", nBonus, iMiscModifier, false, true);
				end

				-- Trait bonus for allying with City States
				iModifier = pTheirUnit:GetAllianceCSStrength();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ATTACK_CS_ALLIANCE_STRENGTH", nBonus, iMiscModifier, false, true);

				if pMyUnit then
					-- Trait bonus against more advanced enemy in owned territory
					if pToPlot:GetOwner() == eTheirPlayer and pMyUnit:IsHigherTechThan(pTheirUnit:GetUnitType()) then
						iModifier = pTheirPlayer:GetCombatBonusVsHigherTech();
						nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_TRAIT_LOW_TECH_BONUS", nBonus, iMiscModifier, false, true);
					end

					-- Trait bonus against enemy with more cities
					if pMyUnit:IsLargerCivThan(pTheirUnit) then
						iModifier = pTheirPlayer:GetCombatBonusVsLargerCiv();
						nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_TRAIT_SMALL_SIZE_BONUS", nBonus, iMiscModifier, false, true);
					end
				end

				-- Monopoly defense bonus
				iModifier = pTheirUnit:GetMonopolyDefenseBonus();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_MONOPOLY_POWER_DEFENSE", nBonus, iMiscModifier, false, true);

				-- Flanking bonus
				if pMyUnit and not bRanged then
					iModifier = pToPlot:GetEffectiveFlankingBonus(pTheirUnit, pMyUnit, pFromPlot);
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_FLANKING_BONUS", nBonus, iMiscModifier, false, true);
				end

				-- Fighting in friendly lands
				if pToPlot:IsFriendlyTerritory(eTheirPlayer) then
					-- General modifier
					iModifier = pTheirUnit:GetFriendlyLandsModifier();
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_FIGHT_AT_HOME_BONUS", nBonus, iMiscModifier, false, true);

					-- Religion modifier
					iModifier = pTheirPlayer:GetFoundedReligionFriendlyCityCombatMod(pToPlot);
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_FRIENDLY_CITY_BELIEF_BONUS", nBonus, iMiscModifier, false, true);

					-- Defending in friendly lands (+ following a different religion)
					if pMyUnit then
						iModifier = pTheirUnit:GetCombatVersusOtherReligionOwnLands(pMyUnit);
						nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_FRIENDLY_CITY_BELIEF_BONUS_CBP", nBonus, iMiscModifier, false, true);
					end
				else
					-- General modifier
					iModifier = pTheirUnit:GetOutsideFriendlyLandsModifier();
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_OUTSIDE_HOME_BONUS", nBonus, iMiscModifier, false, true);

					-- Religion modifier
					iModifier = pTheirPlayer:GetFoundedReligionEnemyCityCombatMod(pToPlot);
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ENEMY_CITY_BELIEF_BONUS", nBonus, iMiscModifier, false, true);
				end

				-- Defending in lands friendly to the attacker (+ following a different religion)
				if pMyUnit and pToPlot:IsFriendlyTerritory(eMyPlayer) then
					iModifier = pTheirUnit:GetCombatVersusOtherReligionTheirLands(pMyUnit);
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ENEMY_CITY_BELIEF_BONUS_CBP", nBonus, iMiscModifier, false, true);
				end

				-- Adjacent to X unit combat modifier
				iModifier = pTheirUnit:PerAdjacentUnitCombatModifier() + pTheirUnit:PerAdjacentUnitCombatDefenseMod();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_PER_ADJACENT_UNIT_COMBAT", nBonus, iMiscModifier, false, true);

				-- Nearby unit modifier
				iModifier = pTheirUnit:GetGiveCombatModToUnit();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_NEARBYPROMOTION_COMBAT_BONUS", nBonus, iMiscModifier, false, true);

				-- Near friendly/hostile/any city modifier
				iModifier = pTheirUnit:GetNearbyCityBonusCombatMod();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_NEARBYPROMOTION_CITY_COMBAT_BONUS", nBonus, iMiscModifier, false, true);

				-- Near improvement modifier
				iModifier = pTheirUnit:GetNearbyImprovementModifier(pToPlot);
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_IMPROVEMENT_NEAR", nBonus, iMiscModifier, false, true);

				-- Near unit class modifier
				iModifier = pTheirUnit:GetNearbyUnitClassModifierFromUnitClass(pToPlot);
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_UNITCLASS_NEAR", nBonus, iMiscModifier, false, true);

				-- Adjacent to owned unit modifier
				if pTheirUnit:IsFriendlyUnitAdjacent(true) then
					iModifier = pTheirUnit:GetAdjacentModifier();
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ADJACENT_FRIEND_UNIT_BONUS", nBonus, iMiscModifier, false, true);
				else
					iModifier = pTheirUnit:GetNoAdjacentUnitModifier();
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_NO_FRIENDLY_UNIT_ADJACENT_BONUS", nBonus, iMiscModifier, false, true);
				end

				if pMyUnit then
					-- VS unit class modifier
					local eMyUnitClass = pMyUnit:GetUnitClassType();
					iModifier = pTheirUnit:GetUnitClassModifier(eMyUnitClass) + pTheirUnit:UnitClassDefenseModifier(eMyUnitClass);
					sDescription = Locale.ConvertTextKey(GameInfo.UnitClasses[eMyUnitClass].Description);
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_VS_CLASS", nBonus, iMiscModifier, false, true, nil, sDescription);

					-- VS unit combat modifier
					local eMyUnitCombat = pMyUnit:GetUnitCombatType();
					if eMyUnitCombat ~= -1 then
						iModifier = pTheirUnit:UnitCombatModifier(pMyUnit:GetUnitCombatType());
						sDescription = Locale.ConvertTextKey(GameInfo.UnitCombatInfos[eMyUnitCombat].Description);
						nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_VS_CLASS", nBonus, iMiscModifier, false, true, nil, sDescription);

						-- Skirmishers count as both Archery and Mounted Units
						if pMyUnit:IsMounted() then
							iModifier = pTheirUnit:UnitCombatModifier(GameInfoTypes.UNITCOMBAT_MOUNTED);
							sDescription = Locale.ConvertTextKey(GameInfo.UnitCombatInfos.UNITCOMBAT_MOUNTED.Description);
							nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_VS_CLASS", nBonus, iMiscModifier, false, true, nil, sDescription);
						end
					end

					-- VS domain modifier
					iModifier = pTheirUnit:DomainModifier(pMyUnit:GetDomainType());
					if iModifier > 0 then
						nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_VS_DOMAIN", nBonus, iMiscModifier, false, true);
					else
						nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_PENALTY_VS_DOMAIN", nBonus, iMiscModifier, false, true);
					end

					-- VS domain defense modifier
					iModifier = pTheirUnit:DomainDefensePercent(pMyUnit:GetDomainType());
					if iModifier > 0 then
						nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_DEFENSE_BONUS_VS_DOMAIN", nBonus, iMiscModifier, false, true);
					else
						nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_DEFENSE_PENALTY_VS_DOMAIN", nBonus, iMiscModifier, false, true);
					end
				end

				-- Defending on open terrain
				if pToPlot:IsOpenGround() then
					iModifier = pTheirUnit:OpenDefenseModifier() + pTheirUnit:OpenFromModifier();
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_OPEN_TERRAIN_DEF_BONUS", nBonus, iMiscModifier, false, true);
				end

				-- Defending on rough terrain
				if pToPlot:IsRoughGround() then
					iModifier = pTheirUnit:RoughDefenseModifier() + pTheirUnit:RoughFromModifier();
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_ROUGH_TERRAIN_DEF_BONUS", nBonus, iMiscModifier, false, true);
				end

				-- Defending on hill
				if pToPlot:IsHills() then
					iModifier = pTheirUnit:HillsDefenseModifier();
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_HILL_DEFENSE_BONUS", nBonus, iMiscModifier, false, true);
				end

				local eToFeature = pToPlot:GetFeatureType();
				local eToTerrain = pToPlot:GetTerrainType();
				if eToFeature ~= FeatureTypes.NO_FEATURE then
					-- Defending on featured tile
					iModifier = pTheirUnit:FeatureDefenseModifier(eToFeature);
					sDescription = Locale.ConvertTextKey(GameInfo.Features[eToFeature].Description);
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_DEFENSE_TERRAIN", nBonus, iMiscModifier, false, true, nil, sDescription);
				else
					-- Defending on featureless tile
					iModifier = pTheirUnit:TerrainDefenseModifier(eToTerrain);
					sDescription = Locale.ConvertTextKey(GameInfo.Terrains[eToTerrain].Description);
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_DEFENSE_TERRAIN", nBonus, iMiscModifier, false, true, nil, sDescription);
				end

				-- Defending on featureless hill
				if eToFeature == FeatureTypes.NO_FEATURE and pToPlot:IsHills() then
					iModifier = pTheirUnit:TerrainAttackModifier(GameInfoTypes.TERRAIN_HILL);
					sDescription = Locale.ConvertTextKey(GameInfo.Terrains.TERRAIN_HILL.Description);
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_BONUS_DEFENSE_TERRAIN", nBonus, iMiscModifier, false, true, nil, sDescription);
				end

				-- Plot defense
				iModifier = pToPlot:DefenseModifier(pTheirUnit:GetTeam(), false, false);
				if iModifier > 0 and pTheirUnit:NoDefensiveBonus() then
					-- Only improvements count
					iModifier = iModifier - pToPlot:DefenseModifier(pTheirUnit:GetTeam(), true, false);
				end
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_TERRAIN_MODIFIER", nBonus, iMiscModifier, false, true);

				-- Fortify bonus
				iModifier = pTheirUnit:FortifyModifier();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_FORTIFICATION_BONUS", nBonus, iMiscModifier, false, true);

				-- Ranged defense modifier
				if bRanged then
					iModifier = pTheirUnit:GetRangedDefenseModifier();
					nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_RANGED_DEFENSE_MODIFIER", nBonus, iMiscModifier, false, true);
				end

				-- Generic promotion modifier + difficulty modifier
				iModifier = pTheirUnit:GetExtraCombatPercent();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_EXTRA_PERCENT", nBonus, iMiscModifier, false, true);

				-- Generic promotion defense modifier
				iModifier = pTheirUnit:GetDefenseModifier();
				nBonus, iMiscModifier = ProcessModifier(iModifier, "TXT_KEY_EUPANEL_DEFENSE_BONUS", nBonus, iMiscModifier, false, true);
			end
		end
	end

	-- Display misc. bonus here if there are more than 4 bonuses
	if nBonus > g_iMaxBonusDisplay then
		controlTable = g_TheirCombatDataIM:GetInstance();
		controlTable.Text:LocalizeAndSetText("TXT_KEY_MISC_BONUS");
		controlTable.Value:SetText(GetFormattedText(iMiscModifier, false, true));
	end

	RecalculateSize();
end

--------------------------------------------------------------------------------
-- Set size
--------------------------------------------------------------------------------
function RecalculateSize()
	Controls.MyCombatResultsStack:CalculateSize();
	Controls.TheirCombatResultsStack:CalculateSize();

	local sizeX = Controls.DetailsGrid:GetSizeX();
	Controls.DetailsGrid:DoAutoSize();
	Controls.DetailsGrid:SetSizeX(sizeX);
	Controls.DetailsSeperator:SetSizeY(Controls.DetailsGrid:GetSizeY());
	Controls.DetailsGrid:ReprocessAnchoring();
end

--------------------------------------------------------------------------------
-- Update health bars in combat simulator
--------------------------------------------------------------------------------
function DoUpdateHealthBars(iMyMaxHP, iTheirMaxHP, iMyCurrentDamage, iTheirCurrentDamage, iMyDamageInflicted, iTheirDamageInflicted)
	local iHealthBarWidth = 8;
	local iHealthBarHeight = 160;

	-------------------------
	-- My health bar
	-------------------------
	local iMyCurrentHP = iMyMaxHP - iMyCurrentDamage;
	if iTheirDamageInflicted > iMyCurrentHP then
		iTheirDamageInflicted = iMyCurrentHP;
	end
	iMyCurrentHP = iMyCurrentHP - iTheirDamageInflicted;

	-- Show the remaining health bar
	local fHealthPercent = iMyCurrentHP / iMyMaxHP;
	local tHealthBarSize = {
		x = iHealthBarWidth,
		y = math.floor(iHealthBarHeight * fHealthPercent),
	};

	Controls.MyRedBar:SetHide(true);
	Controls.MyYellowBar:SetHide(true);
	Controls.MyGreenBar:SetHide(true);
	if fHealthPercent <= 0.30 then
		Controls.MyRedBar:SetSize(tHealthBarSize);
		Controls.MyRedBar:SetHide(false);
	elseif fHealthPercent <= 0.50 then
		Controls.MyYellowBar:SetSize(tHealthBarSize);
		Controls.MyYellowBar:SetHide(false);
	else
		Controls.MyGreenBar:SetSize(tHealthBarSize);
		Controls.MyGreenBar:SetHide(false);
	end

	-- Show the flashing damage bar
	if iTheirDamageInflicted > 0 then
		local fDamagePercent = iTheirDamageInflicted / iMyMaxHP;
		local tDamageBarSize = {
			x = iHealthBarWidth,
			y = math.floor(iHealthBarHeight * fDamagePercent),
		};
		if tHealthBarSize.y > 0 then
			Controls.MyDeltaBar:SetOffsetVal(0, tHealthBarSize.y + 4);
		else
			Controls.MyDeltaBar:SetOffsetVal(0, 2);
		end
		Controls.MyDeltaBar:SetHide(false);
		Controls.MyDeltaBar:SetSize(tDamageBarSize);
		Controls.MyDeltaBarFlash:SetSize(tDamageBarSize);
	else
		Controls.MyDeltaBar:SetHide(true);
	end

	-------------------------
	-- Their health bar
	-------------------------
	local iTheirCurrentHP = iTheirMaxHP - iTheirCurrentDamage;
	if iMyDamageInflicted > iTheirCurrentHP then
		iMyDamageInflicted = iTheirCurrentHP;
	end
	iTheirCurrentHP = iTheirCurrentHP - iMyDamageInflicted;

	-- Show the remaining health bar
	fHealthPercent = iTheirCurrentHP / iTheirMaxHP;
	tHealthBarSize = {
		x = iHealthBarWidth,
		y = math.floor(iHealthBarHeight * fHealthPercent),
	};

	Controls.TheirRedBar:SetHide(true);
	Controls.TheirGreenBar:SetHide(true);
	Controls.TheirYellowBar:SetHide(true);
	if fHealthPercent <= 0.30 then
		Controls.TheirRedBar:SetSize(tHealthBarSize);
		Controls.TheirRedBar:SetHide(false);
	elseif fHealthPercent <= 0.50 then
		Controls.TheirYellowBar:SetSize(tHealthBarSize);
		Controls.TheirYellowBar:SetHide(false);
	else
		Controls.TheirGreenBar:SetSize(tHealthBarSize);
		Controls.TheirGreenBar:SetHide(false);
	end

	-- Show the flashing damage bar
	if iMyDamageInflicted > 0 then
		local fDamagePercent = iMyDamageInflicted / iTheirMaxHP;
		local tDamageBarSize = {
			x = iHealthBarWidth,
			y = math.floor(iHealthBarHeight * fDamagePercent),
		};
		if tHealthBarSize.y > 0 then
			Controls.TheirDeltaBar:SetOffsetVal(0, tHealthBarSize.y + 4);
		else
			Controls.TheirDeltaBar:SetOffsetVal(0, 2);
		end
		Controls.TheirDeltaBar:SetHide(false);
		Controls.TheirDeltaBar:SetSize(tDamageBarSize);
		Controls.TheirDeltaBarFlash:SetSize(tDamageBarSize);
	else
		Controls.TheirDeltaBar:SetHide(true);
	end
end

--------------------------------------------------------------------------------
function OnWorldMouseOver(bWorldHasMouseOver)
	g_bWorldMouseOver = bWorldHasMouseOver;

	if g_bShowPanel and g_bWorldMouseOver then
		ContextPtr:SetHide(false);
	else
		ContextPtr:SetHide(true);
	end
end
Events.WorldMouseOver.Add(OnWorldMouseOver);

--------------------------------------------------------------------------------
function OnMouseOverHex(hexX, hexY)
	g_bShowPanel = false;

	Controls.MyCombatResultsStack:SetHide(true);
	Controls.TheirCombatResultsStack:SetHide(true);
	Events.ClearHexHighlightStyle("ValidFireTargetBorder");

	local pPlot = Map.GetPlot(hexX, hexY);

	if pPlot then
		local pHeadUnit = UI.GetHeadSelectedUnit();
		local pHeadCity = UI.GetHeadSelectedCity();

		-- Air units are not combat units; need to check combat strength explicitly
		-- Also suppress the popup if the unit can be promoted to avoid overlap
		if pHeadUnit and (pHeadUnit:GetBaseCombatStrength() > 0 or pHeadUnit:GetBaseRangedCombatStrength() > 0) and not pHeadUnit:IsPromotionReady() then
			if not pHeadUnit:IsRanged() then
				-- Melee attacks and air strikes
				local eTeam = Game.GetActiveTeam();
				local pTeam = Teams[eTeam];

				-- Don't show info for targets we can't see
				if pPlot:IsRevealed(eTeam, false) then
					if pPlot:IsCity() then
						local pCity = pPlot:GetPlotCity();
						if pTeam:IsAtWar(pCity:GetTeam()) or (UIManager:GetAlt() and pCity:GetOwner() ~= eTeam) then
							UpdateCityStats(pCity);
							UpdateCombatSimulator(pHeadUnit, nil, nil, pCity);
							UpdateUnitPromotions(nil);
							UpdateCityPortrait(pCity);

							Controls.MyCombatResultsStack:SetHide(false);
							Controls.TheirCombatResultsStack:SetHide(false);
							g_bShowPanel = true;
						end
					else
						-- Can see this plot right now
						if pPlot:IsVisible(eTeam, false) and not pHeadUnit:IsCityAttackOnly() then
							local eImprovement = pPlot:GetImprovementType();
							local bValidLandAttack = pHeadUnit:GetDomainType() == DomainTypes.DOMAIN_LAND and (not pPlot:IsWater() or (eImprovement ~= -1 and GameInfo.Improvements[eImprovement].AllowsWalkWater));
							local bValidSeaAttack = pHeadUnit:GetDomainType() == DomainTypes.DOMAIN_SEA and pPlot:IsWater();
							local bValidAirAttack = pHeadUnit:GetDomainType() == DomainTypes.DOMAIN_AIR;

							if bValidLandAttack or bValidSeaAttack or bValidAirAttack then
								local ePlayer = Game.GetActivePlayer();
								local pUnit = pPlot:GetBestDefender(-1, ePlayer, pHeadUnit, 1);

								if pUnit then
									UpdateUnitPortrait(pUnit);
									UpdateUnitPromotions(pUnit);
									UpdateUnitStats(pUnit);
									UpdateCombatSimulator(pHeadUnit, pUnit);
									Controls.MyCombatResultsStack:SetHide(false);
									Controls.TheirCombatResultsStack:SetHide(false);
									g_bShowPanel = true;
								else
									pUnit = pPlot:GetBestDefender(-1, ePlayer, pHeadUnit);
									if pUnit then
										UpdateUnitPortrait(pUnit);
										UpdateUnitPromotions(pUnit);
										UpdateUnitStats(pUnit);
									end
								end
							end
						end
					end
				end
			elseif pHeadUnit:CanEverRangeStrikeAt(pPlot:GetX(), pPlot:GetY()) and not pHeadUnit:IsEmbarked() then
				-- Ranged attack, need to check correct domain etc.
				local eTeam = Game.GetActiveTeam();
				local pTeam = Teams[eTeam];

				-- Don't show info for targets we can't see
				if pPlot:IsRevealed(eTeam, false) then
					if pPlot:IsCity() and pHeadUnit:GetUnitAIType() ~= GameInfoTypes.UNITAI_MISSILE_AIR then
						-- Target is a city
						local pCity = pPlot:GetPlotCity();

						if pTeam:IsAtWar(pCity:GetTeam()) or (UIManager:GetAlt() and pCity:GetOwner() ~= eTeam) then
							UpdateCityStats(pCity);
							UpdateCombatSimulator(pHeadUnit, nil, nil, pCity);
							UpdateUnitPromotions(nil);
							UpdateCityPortrait(pCity);

							Controls.MyCombatResultsStack:SetHide(false);
							Controls.TheirCombatResultsStack:SetHide(false);
							g_bShowPanel = true;
						end
					elseif pPlot:IsVisible(eTeam, false) and not pHeadUnit:IsCityAttackOnly() then
						local ePlayer = Game.GetActivePlayer();
						local pUnit = pPlot:GetBestDefender(-1, ePlayer, pHeadUnit, 1);

						if pUnit then
							UpdateUnitPortrait(pUnit);
							UpdateUnitPromotions(pUnit);
							UpdateUnitStats(pUnit);
							UpdateCombatSimulator(pHeadUnit, pUnit);
							Controls.MyCombatResultsStack:SetHide(false);
							Controls.TheirCombatResultsStack:SetHide(false);
							g_bShowPanel = true;
						else
							pUnit = pPlot:GetBestDefender(-1, ePlayer, pHeadUnit);
							if pUnit then
								UpdateUnitPortrait(pUnit);
								UpdateUnitPromotions(pUnit);
								UpdateUnitStats(pUnit);
							end
						end
					end
				end
			end
		elseif pHeadCity and pHeadCity:CanRangeStrikeNow() then
			-- No combat unit selected, what about a city?
			local eTeam = Game.GetActiveTeam();

			-- Don't show info for targets we can't see
			if pPlot:IsVisible(eTeam, false) then
				local ePlayer = Game.GetActivePlayer();
				local pUnit = pPlot:GetBestDefender(-1, ePlayer, nil, 1);

				if pUnit then
					UpdateUnitPortrait(pUnit);
					UpdateUnitPromotions(pUnit);
					UpdateUnitStats(pUnit);
					UpdateCombatSimulator(nil, pUnit, pHeadCity);
					Controls.MyCombatResultsStack:SetHide(false);
					Controls.TheirCombatResultsStack:SetHide(false);
					g_bShowPanel = true;
				end
			end
		end
	end

	if g_bShowPanel and g_bWorldMouseOver then
		ContextPtr:SetHide(false);
	else
		ContextPtr:SetHide(true);
	end
end
Events.SerialEventMouseOverHex.Add(OnMouseOverHex);

--------------------------------------------------------------------------------
function ShowHideHandler(bIsHide, bIsInit)
	if not bIsInit then
		LuaEvents.EnemyPanelHide(bIsHide);
	end
end
ContextPtr:SetShowHideHandler(ShowHideHandler);
