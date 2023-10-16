print("This is the modded TradeRouteHelpers from CBP")
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function BuildTradeRouteGoldToolTipString (pOriginCity, pTargetCity, eDomain)

	local iPlayer = pOriginCity:GetOwner();
	local pPlayer = Players[iPlayer];
	local iOtherPlayer = pTargetCity:GetOwner();
	local pOtherPlayer = Players[iOtherPlayer];
	local strOtherLeaderName;
	if(pOtherPlayer:GetNickName() ~= "" and Game:IsNetworkMultiPlayer()) then
		strOtherLeaderName = pOtherPlayer:GetNickName();
	else
		strOtherLeaderName = pOtherPlayer:GetName();
	end

	local strResult = "";
	local strBaseValue = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_BASE", pPlayer:GetInternationalTradeRouteBaseBonus(pOriginCity, pTargetCity, true) / 100);
	local strYourGPTValue = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_GPT_YOURS", pOriginCity:GetNameKey(), pPlayer:GetInternationalTradeRouteGPTBonus(pOriginCity, pTargetCity, true) / 100);
	--local strTheirGPTValue = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_GPT_YOURS", pTargetCity:GetNameKey(), pPlayer:GetInternationalTradeRouteGPTBonus(pOriginCity, pTargetCity, false) / 100);
-- COMMUNITY PATCH
	local iTheirInfluenceGold = pOtherPlayer:GetInfluenceTradeRouteGoldBonus(iPlayer);
	local strTheirGPTValue = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_GPT_YOURS", pTargetCity:GetNameKey(), pPlayer:GetInternationalTradeRouteGPTBonus(pOriginCity, pTargetCity, false) / 100 );
	local iInfluenceGold = pPlayer:GetInfluenceTradeRouteGoldBonus(iOtherPlayer);
--END
	local iPolicyBonus = pPlayer:GetInternationalTradeRoutePolicyBonus(pOriginCity, pTargetCity, eDomain);
	local strPolicyValue = "";
	if (iPolicyBonus ~= 0) then
		strPolicyValue = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_POLICIES", iPolicyBonus / 100);
	end

	local iYourBuildingBonus = pPlayer:GetInternationalTradeRouteYourBuildingBonus(pOriginCity, pTargetCity, eDomain, true);
	local strYourBuildingValue = "";
	if (iYourBuildingBonus ~= 0) then
		strYourBuildingValue = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_BUILDING", pOriginCity:GetNameKey(), iYourBuildingBonus / 100);
	end

	--CBP
	local iMinorBonus = pPlayer:GetMinorCivGoldBonus(pOriginCity, pTargetCity, eDomain, true);
	local strMinorBonus = "";
	if (iMinorBonus ~= 0) then
		if (pOtherPlayer:IsMinorCiv()) then
			strMinorBonus = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_MINOR_CS_TT_BONUS", pTargetCity:GetNameKey(), iMinorBonus / 100);
		elseif (iPlayer == iOtherPlayer) then
			strMinorBonus = Locale.ConvertTextKey("TXT_KEY_CHOOSE_GOLD_INTERNAL_TRADE_ROUTE_MINOR_CS_TT_BONUS", pTargetCity:GetNameKey(), iMinorBonus / 100);
		end
	end
	--END
	
	local iTheirBuildingBonus = pPlayer:GetInternationalTradeRouteTheirBuildingBonus(pOriginCity, pTargetCity, eDomain, true);
	local strTheirBuildingValue = "";
	if (iTheirBuildingBonus ~= 0) then
		strTheirBuildingValue = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_BUILDING", pTargetCity:GetNameKey(), iTheirBuildingBonus / 100);
	end
	
	local strResourceList = "";
	local iDiversityBonus = pPlayer:GetInternationalTradeRouteResourceBonus(pOriginCity, pTargetCity, true);
	if (iDiversityBonus ~= 0) then
		local iOurDiversity = pPlayer:GetCityResourceBonus(pOriginCity, pTargetCity, true);
		local iTheirDiversity = pPlayer:GetCityResourceBonus(pOriginCity, pTargetCity, false);
		strResourceList = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_RESOURCE_DIFFERENT", iDiversityBonus, iOurDiversity, iTheirDiversity);
	end

	local strExclusiveValue = "";
	local iExclusiveBonus = pPlayer:GetInternationalTradeRouteExclusiveBonus(pOriginCity, pTargetCity);
	if (iExclusiveBonus ~= 0) then
		strExclusiveValue = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_EXCLUSIVE_CONNECTION", iExclusiveBonus / 100);
	end

	local strOtherTraitValue = "";
	local iOtherTraitBonus = pPlayer:GetInternationalTradeRouteOtherTraitBonus(pOriginCity, pTargetCity, eDomain, true);
	if (iOtherTraitBonus ~= 0) then
		strOtherTraitValue = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_OTHER_TRAIT", pOtherPlayer:GetCivilizationAdjectiveKey(), iOtherTraitBonus / 100);
	end

	local strRiverModifier = "";
	local iRiverModifier = pPlayer:GetInternationalTradeRouteRiverModifier(pOriginCity, pTargetCity, eDomain, true);
	if (iRiverModifier ~= 0) then
		strRiverModifier = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_RIVER_MODIFIER", iRiverModifier);
	end
-- CBP
	local strCorporateModifier = "";
	local iCorporateModifier = pPlayer:GetInternationalTradeRouteCorporationModifier(pOriginCity, pTargetCity, true);
	if (iCorporateModifier ~= 0) then
		strCorporateModifier = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_CORPORATION_MODIFIER_GOLD", iCorporateModifier);
	end

	local strOpenBordersModifier = "";
	local iOpenBordersModifier = pPlayer:GetTradeConnectionOpenBordersModifierTimes100(pOriginCity, pTargetCity, true);
	if (iOpenBordersModifier ~= 0) then
		strOpenBordersModifier = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_OPEN_BORDERS_MOD", iOpenBordersModifier);
	end

	local strDistanceModifier = "";
	local iDistanceModifier = pPlayer:GetTradeConnectionDistanceValueModifierTimes100(pOriginCity, pTargetCity, eDomain);
	if (iDistanceModifier ~= 0) then
		strDistanceModifier = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_DISTANCE_MOD", -iDistanceModifier);
	end
-- END

-- COMMUNITY PATCH
	local strInfluenceValue = "";
	if (iInfluenceGold > 0) then
		strInfluenceValue = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_GOLD_EXPLAINED", strOtherLeaderName, iInfluenceGold / 100);
	end
	local strHolyCapitalValue = "";
	local iHolyCapitalYield = pPlayer:GetTradeRouteYieldModifier(pOriginCity, pTargetCity);
	if (iHolyCapitalYield ~= 0) then
		strHolyCapitalValue = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YIELD_MODIFIER", iHolyCapitalYield);
	end
-- END

	local strDomainModifier = "";
	local iDomainModifier = pPlayer:GetInternationalTradeRouteDomainModifier(eDomain);
	if (iDomainModifier ~= 0) then
		if (eDomain == DomainTypes.DOMAIN_SEA) then
			strDomainModifier = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_DOMAIN_SEA_MODIFIER", (iDomainModifier + 100) / 100);
-- CBP change
		else
			strDomainModifier = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_DOMAIN_LAND_MODIFIER", (iDomainModifier + 100) / 100);
-- end
		end
	end

	local strTotal = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_TOTAL", pPlayer:GetInternationalTradeRouteTotal(pOriginCity, pTargetCity, eDomain, true) / 100);
	
	local strOtherTotal = "";
	local iTradeeAmount = pOtherPlayer:GetInternationalTradeRouteTotal(pOriginCity, pTargetCity, eDomain, false);

	if (iTradeeAmount ~= 0) then
		local strOtherRevenueHeader;
		if (iPlayer == Game.GetActivePlayer()) then
			strOtherRevenueHeader = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_REVENUE");
		else
			strOtherRevenueHeader = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_REVENUE");
		end
		 
		strOtherTotal = strOtherTotal .. strOtherRevenueHeader;
		strOtherTotal = strOtherTotal .. "[NEWLINE]";
		
		local iOtherBase = pOtherPlayer:GetInternationalTradeRouteBaseBonus(pOriginCity, pTargetCity, false);
		if (iOtherBase ~= 0) then
			strOtherTotal = strOtherTotal .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_BASE", iOtherBase / 100);
			strOtherTotal = strOtherTotal .. "[NEWLINE]";
		end
		
		local iRiverModifier = pPlayer:GetInternationalTradeRouteRiverModifier(pOriginCity, pTargetCity, eDomain, false);
		if (iRiverModifier ~= 0) then
			strOtherTotal = strOtherTotal .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_RIVER_MODIFIER", iRiverModifier);
			strOtherTotal = strOtherTotal .. "[NEWLINE]";
		end
		
		if (strDomainModifier ~= "") then
			strOtherTotal = strOtherTotal .. strDomainModifier;
			strOtherTotal = strOtherTotal .. "[NEWLINE]";
		end
		
		local iOtherBuildingBonus = pOtherPlayer:GetInternationalTradeRouteTheirBuildingBonus(pOriginCity, pTargetCity, eDomain, false);
		if (iOtherBuildingBonus ~= 0) then
			strOtherTotal = strOtherTotal .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_BUILDING", pTargetCity:GetNameKey(), iOtherBuildingBonus / 100);
			strOtherTotal = strOtherTotal .. "[NEWLINE]";
		end


-- COMMUNITY PATCH
		if(iTheirInfluenceGold > 0) then
			if (iPlayer == Game.GetActivePlayer()) then
				strOtherTotal = strOtherTotal .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_GOLD_EXPLAINED", strOtherLeaderName, iTheirInfluenceGold / 100);
				strOtherTotal = strOtherTotal .. "[NEWLINE]";
			else
				strOtherTotal = strOtherTotal .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_GOLD_EXPLAINED", strOtherLeaderName, iInfluenceGold / 100);
				strOtherTotal = strOtherTotal .. "[NEWLINE]";
			end
		end
		local strOpenBordersModifier = "";
		local iOpenBordersModifier = pOtherPlayer:GetTradeConnectionOpenBordersModifierTimes100(pOriginCity, pTargetCity, false);
		if (iOpenBordersModifier ~= 0) then
			strOtherTotal = strOtherTotal .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_OPEN_BORDERS_MOD", iOpenBordersModifier);
			strOtherTotal = strOtherTotal .. "[NEWLINE]";
		end

		local strDistanceModifier = "";
		local iDistanceModifier = pPlayer:GetTradeConnectionDistanceValueModifierTimes100(pOriginCity, pTargetCity, eDomain);
		if (iDistanceModifier ~= 0) then
			strOtherTotal = strOtherTotal .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_DISTANCE_MOD", -iDistanceModifier);
			strOtherTotal = strOtherTotal .. "[NEWLINE]";
		end
-- END
		
		strOtherTotal = strOtherTotal .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_TRADEE_TOTAL", strOtherLeaderName, iTradeeAmount / 100);
	end
	
	if (iPlayer ~= Game.GetActivePlayer()) then
		strResult = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_REVENUE");
	else
		strResult = Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_REVENUE");
	end
	strResult = strResult .. "[NEWLINE]";
	strResult = strResult .. strBaseValue;
	strResult = strResult .. "[NEWLINE]";
	strResult = strResult .. strYourGPTValue;
	strResult = strResult .. "[NEWLINE]";
	strResult = strResult .. strTheirGPTValue;
	strResult = strResult .. "[NEWLINE]";
	
	if (strPolicyValue ~= "") then
		strResult = strResult .. strPolicyValue;
		strResult = strResult .. "[NEWLINE]";
	end
	
	if (strYourBuildingValue ~= "") then
		strResult = strResult .. strYourBuildingValue;
		strResult = strResult .. "[NEWLINE]";
	end

	if (strTheirBuildingValue ~= "") then
		strResult = strResult .. strTheirBuildingValue;
		strResult = strResult .. "[NEWLINE]";
	end
	
	if (strResourceList ~= "") then
		strResult = strResult .. strResourceList;
		strResult = strResult .. "[NEWLINE]";
	end
	
	if (strExclusiveBonus ~= "") then
		strResult = strResult .. strExclusiveValue;	
		strResult = strResult .. "[NEWLINE]";
	end
	
	if (strOtherTraitValue ~= "") then
		strResult = strResult .. strOtherTraitValue;
		strResult = strResult .. "[NEWLINE]";
	end
	
	if (strRiverModifier ~= "") then
		strResult = strResult .. strRiverModifier;
		strResult = strResult .. "[NEWLINE]";
	end

	--CBP
	if (strMinorBonus ~= "") then
		strResult = strResult .. strMinorBonus;
		strResult = strResult .. "[NEWLINE]";
	end
	--END

-- Community Patch
	if (strInfluenceValue ~= "") then
		strResult = strResult .. strInfluenceValue;
		strResult = strResult .. "[NEWLINE]";
	end
--END
-- CBP
	if (strCorporateModifier ~= "") then
		strResult = strResult .. strCorporateModifier;
		strResult = strResult .. "[NEWLINE]";
	end

	if (strOpenBordersModifier ~= "") then
		strResult = strResult .. strOpenBordersModifier;
		strResult = strResult .. "[NEWLINE]";
	end

	if (strDistanceModifier ~= "") then
		strResult = strResult .. strDistanceModifier;
		strResult = strResult .. "[NEWLINE]";
	end

	if (strHolyCapitalValue ~= "") then
		strResult = strResult .. strHolyCapitalValue;
		strResult = strResult .. "[NEWLINE]";
	end
-- END
	
	if (strDomainModifier ~= "") then
		strResult = strResult .. strDomainModifier;
	end
	strResult = strResult .. "[NEWLINE]";
	strResult = strResult .. strTotal;
	strResult = strResult .. "[NEWLINE]";
	
	if (strOtherTotal ~= "") then
		strResult = strResult .. "[NEWLINE]";
		strResult = strResult .. strOtherTotal;
	end
	
	return strResult;
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function BuildTradeRouteScienceToolTipString (pOriginCity, pTargetCity, eDomain)

	local strResult = "";
	local iPlayer = pOriginCity:GetOwner();
	local pOriginPlayer = Players[iPlayer];
	local iOtherPlayer = pTargetCity:GetOwner();
	local pOtherPlayer = Players[iOtherPlayer];
	
	local strOriginLeaderName;
	if(pOriginPlayer:GetNickName() ~= "" and Game:IsNetworkMultiPlayer()) then
		strOriginLeaderName = pOriginPlayer:GetNickName();
	else
		strOriginLeaderName = pOriginPlayer:GetName();
	end
	
	local strOtherLeaderName;
	if(pOtherPlayer:GetNickName() ~= "" and Game:IsNetworkMultiPlayer()) then
		strOtherLeaderName = pOtherPlayer:GetNickName();
	else
		strOtherLeaderName = pOtherPlayer:GetName();
	end

	local iOriginScience  = pOriginPlayer:GetInternationalTradeRouteScience(pOriginCity, pTargetCity, eDomain, true) / 100;
	local iDestScience = pOtherPlayer:GetInternationalTradeRouteScience(pOriginCity, pTargetCity, eDomain, false) / 100;

	if (iOriginScience > 0) then
		if (pOtherPlayer:IsMinorCiv()) then
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_SCIENCE_GAIN_CS", iOriginScience);
		elseif (iPlayer == iOtherPlayer) then
			strResult = strResult .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_GOLD_INTERNAL_TRADE_ROUTE_ITEM_TT_YOUR_SCIENCE_GAIN", iOriginScience);
		else	
			local iNumTechs = pOriginPlayer:GetNumTechDifference(iOtherPlayer);
			local iInfluenceScience = pOriginPlayer:GetInfluenceTradeRouteScienceBonus(iOtherPlayer);
			local delta = iOriginScience - iInfluenceScience;
			if (iPlayer == Game.GetActivePlayer()) then	
				strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_SCIENCE_GAIN");
				strResult = strResult .. "[NEWLINE]";
				strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_SCIENCE_EXPLAINED", strOtherLeaderName, iNumTechs, iInfluenceScience, delta);
				strResult = strResult .. "[NEWLINE]";
				strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_SCIENCE_TOTAL", iOriginScience);
			else
				strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_SCIENCE_GAIN");
				strResult = strResult .. "[NEWLINE]";
				strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_SCIENCE_EXPLAINED", iNumTechs, strOriginLeaderName, iInfluenceScience, delta);
				strResult = strResult .. "[NEWLINE]";
				strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_SCIENCE_TOTAL", strOriginLeaderName, iOriginScience);				
			end
		end

		-- CBP
		local strCorporateModifier = "";
		local iCorporateModifier = pOriginPlayer:GetInternationalTradeRouteCorporationModifierScience(pOriginCity, pTargetCity, eDomain, true);
		if (iCorporateModifier ~= 0) then
			strResult = strResult .. "[NEWLINE]";
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_CORPORATION_MODIFIER_SCIENCE", iCorporateModifier);		
		end

		local strHolyCapitalValue = "";
		local iHolyCapitalYield = pOriginPlayer:GetTradeRouteYieldModifier(pOriginCity, pTargetCity);
		if (iHolyCapitalYield ~= 0) then
			strResult = strResult .. "[NEWLINE]";
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YIELD_MODIFIER", iHolyCapitalYield);
		end
	-- END
	end
	
	if (iDestScience > 0) then
		if (strResult ~= "") then
			strResult = strResult .. "[NEWLINE][NEWLINE]";
		end
	
		local iNumTechs = pOtherPlayer:GetNumTechDifference(iPlayer);
		local iInfluenceScience = pOtherPlayer:GetInfluenceTradeRouteScienceBonus(iPlayer);
		local delta = iDestScience - iInfluenceScience;
		if (iPlayer == Game.GetActivePlayer()) then
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_SCIENCE_GAIN");
			strResult = strResult .. "[NEWLINE]";
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_SCIENCE_EXPLAINED", iNumTechs, strOtherLeaderName, iInfluenceScience, delta);
			strResult = strResult .. "[NEWLINE]";
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_SCIENCE_TOTAL", strOtherLeaderName, iDestScience);		
		else
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_SCIENCE_GAIN");
			strResult = strResult .. "[NEWLINE]";
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_SCIENCE_EXPLAINED", strOriginLeaderName, iNumTechs, iInfluenceScience, delta);
			strResult = strResult .. "[NEWLINE]";
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_SCIENCE_TOTAL", iDestScience);		
		end

	end

	return strResult;
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function BuildTradeRouteCultureToolTipString (pOriginCity, pTargetCity, eDomain)

	local strResult = "";
	local iPlayer = pOriginCity:GetOwner();
	local pOriginPlayer = Players[iPlayer];
	local iOtherPlayer = pTargetCity:GetOwner();
	local pOtherPlayer = Players[iOtherPlayer];
	
	local strOriginLeaderName;
	if(pOriginPlayer:GetNickName() ~= "" and Game:IsNetworkMultiPlayer()) then
		strOriginLeaderName = pOriginPlayer:GetNickName();
	else
		strOriginLeaderName = pOriginPlayer:GetName();
	end
	
	local strOtherLeaderName;
	if(pOtherPlayer:GetNickName() ~= "" and Game:IsNetworkMultiPlayer()) then
		strOtherLeaderName = pOtherPlayer:GetNickName();
	else
		strOtherLeaderName = pOtherPlayer:GetName();
	end

	local iOriginCulture = pOriginPlayer:GetInternationalTradeRouteCulture(pOriginCity, pTargetCity, eDomain, true) / 100;
	local iDestCulture = pOtherPlayer:GetInternationalTradeRouteCulture(pOriginCity, pTargetCity, eDomain, false) / 100;

	if (iOriginCulture > 0) then
		if(pOtherPlayer:IsMinorCiv()) then
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_CULTURE_GAIN_CS", iOriginCulture);
		elseif (iPlayer == iOtherPlayer) then
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_GOLD_INTERNAL_TRADE_ROUTE_ITEM_TT_YOUR_CULTURE_GAIN", iOriginCulture);
		else	
			local iNumPolicies = pOriginPlayer:GetNumPolicyDifference(iOtherPlayer);
			if(iNumPolicies < 0) then
				iTotal = (iTotal * -1);
			end
			if (iPlayer == Game.GetActivePlayer()) then	
				strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_CULTURE_GAIN");
				strResult = strResult .. "[NEWLINE]";
				strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_CULTURE_EXPLAINED", strOtherLeaderName, iNumPolicies);
				strResult = strResult .. "[NEWLINE]";
				strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_CULTURE_TOTAL", iOriginCulture);
			else
				strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_CULTURE_GAIN");
				strResult = strResult .. "[NEWLINE]";
				strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_CULTURE_EXPLAINED", iNumPolicies, strOriginLeaderName);
				strResult = strResult .. "[NEWLINE]";
				strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_CULTURE_TOTAL", strOriginLeaderName, iOriginCulture);				
			end
		end

		local strHolyCapitalValue = "";
		local iHolyCapitalYield = pOriginPlayer:GetTradeRouteYieldModifier(pOriginCity, pTargetCity);
		if (iHolyCapitalYield ~= 0) then
			strResult = strResult .. "[NEWLINE]";
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YIELD_MODIFIER", iHolyCapitalYield);
		end
	end
	
	if (iDestCulture > 0) then
		if (strResult ~= "") then
			strResult = strResult .. "[NEWLINE][NEWLINE]";
		end
	
		local iNumPolicies = pOtherPlayer:GetNumPolicyDifference(iPlayer);
		if(iNumPolicies < 0) then
			iNumPolicies = (iNumPolicies * -1);
		end
		if (iPlayer == Game.GetActivePlayer()) then
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_CULTURE_GAIN");
			strResult = strResult .. "[NEWLINE]";
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_CULTURE_EXPLAINED", iNumPolicies, strOtherLeaderName);
			strResult = strResult .. "[NEWLINE]";
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_CULTURE_TOTAL", strOtherLeaderName, iDestCulture);		
		else
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_CULTURE_GAIN");
			strResult = strResult .. "[NEWLINE]";
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_CULTURE_EXPLAINED", strOriginLeaderName, iNumPolicies);
			strResult = strResult .. "[NEWLINE]";
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_CULTURE_TOTAL", iDestCulture);		
		end

	end

	return strResult;
end
--CBP
function BuildTradeRouteProductionToolTipString (pOriginCity, pTargetCity, eDomain)

	local strResult = "";
	local iPlayer = pOriginCity:GetOwner();
	local pOriginPlayer = Players[iPlayer];

	local iOriginProduction  = pOriginPlayer:GetInternationalTradeRouteProduction(pOriginCity, pTargetCity, eDomain, true) / 100;

	if (iOriginProduction > 0) then
		strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_PRODUCTION", iOriginProduction);
		
		local strHolyCapitalValue = "";
		local iHolyCapitalYield = pOriginPlayer:GetTradeRouteYieldModifier(pOriginCity, pTargetCity);
		if (iHolyCapitalYield ~= 0) then
			strResult = strResult .. "[NEWLINE]";
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNAL_TRADE_ROUTE_ITEM_TT_YIELD_MODIFIER_PROD", iHolyCapitalYield);
		end
	end

	return strResult;
end
function BuildTradeRouteFoodToolTipString (pOriginCity, pTargetCity, eDomain)

	local strResult = "";
	local iPlayer = pOriginCity:GetOwner();
	local pOriginPlayer = Players[iPlayer];

	local iOriginFood  = pOriginPlayer:GetInternationalTradeRouteFood(pOriginCity, pTargetCity, eDomain, true) / 100;

	if (iOriginFood > 0) then
		strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_FOOD", iOriginFood);

		local strHolyCapitalValue = "";
		local iHolyCapitalYield = pOriginPlayer:GetTradeRouteYieldModifier(pOriginCity, pTargetCity);
		if (iHolyCapitalYield ~= 0) then
			strResult = strResult .. "[NEWLINE]";
			strResult = strResult .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_INTERNAL_TRADE_ROUTE_ITEM_TT_YIELD_MODIFIER_FOOD", iHolyCapitalYield);
		end
	end

	return strResult;
end

--END
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function BuildTradeRouteToolTipString (pPlayer, pOriginCity, pTargetCity, eDomain, eTradeConnectionType)
	local strResult;

	local strGoldToolTip = BuildTradeRouteGoldToolTipString(pOriginCity, pTargetCity, eDomain);
	local strScienceToolTip = BuildTradeRouteScienceToolTipString(pOriginCity, pTargetCity, eDomain);
	-- CBP
	local strCultureToolTip = BuildTradeRouteCultureToolTipString(pOriginCity, pTargetCity, eDomain);
	local strProductionToolTip = BuildTradeRouteProductionToolTipString(pOriginCity, pTargetCity, eDomain);
	local strFoodToolTip = BuildTradeRouteFoodToolTipString(pOriginCity, pTargetCity, eDomain);
	--END
	strResult = strGoldToolTip;
	if (strScienceToolTip ~= "") then
		if (strResult ~= "") then
			strResult = strResult .. "[NEWLINE]";
		end
		
		strResult = strResult .. strScienceToolTip;
	end
	--CBP
	if (strCultureToolTip ~= "") then
		if (strResult ~= "") then
			strResult = strResult .. "[NEWLINE]";
		end
		
		strResult = strResult .. strCultureToolTip;
	end	
	if (strProductionToolTip ~= "") then
		if (strResult ~= "") then
			strResult = strResult .. "[NEWLINE]";
		end
		
		strResult = strResult .. strProductionToolTip;
	end
	if (strFoodToolTip ~= "") then
		if (strResult ~= "") then
			strResult = strResult .. "[NEWLINE]";
		end
		
		strResult = strResult .. strFoodToolTip;
	end
	--END
	
	return strResult;
end
