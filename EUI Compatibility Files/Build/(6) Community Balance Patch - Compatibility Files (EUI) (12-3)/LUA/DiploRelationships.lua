-------------------------------------------------
-- Diplomatic
-------------------------------------------------
include( "IconSupport" );
include( "SupportFunctions"  );
include( "InstanceManager" );
include( "InfoTooltipInclude" );
include( "CityStateStatusHelper" );

local m_PlayerTable = Matchmaking.GetPlayerList();
local m_PlayerNames = {};
for i = 1, #m_PlayerTable do
    m_PlayerNames[ m_PlayerTable[i].playerID ] = m_PlayerTable[i].playerName;
end

local g_MajorCivButtonIM = InstanceManager:new( "MajorCivButtonInstance", "DiploButton", Controls.ButtonStack );
local g_MinorCivButtonIM = InstanceManager:new( "MinorCivButtonInstance", "LeaderButton", Controls.MinorButtonStack );
local g_MajorCivTradeRowIMList = {};

local g_iDealDuration = Game.GetDealDuration();

local g_Deal = UI.GetScratchDeal();
local g_iUs = -1; --Game.GetActivePlayer();
local g_pUs = -1;
local g_iTeam = -1;
local g_pTeam = -1;

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )
	g_iUs = Game.GetActivePlayer();
	g_pUs = Players[ g_iUs ];
	g_iTeam = g_pUs:GetTeam();
	g_pTeam = Teams[g_iTeam];
	
	InitMajorCivList();
	InitMinorCivList();
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

function InitMinorCivList()
	-- Clear buttons
	g_MinorCivButtonIM:ResetInstances();
	
	local iPlayer = Game.GetActivePlayer();
	local pPlayer = Players[ iPlayer ];
	local pTeam = Teams[pPlayer:GetTeam()];
	
    -------------------------------------------------
    -- Look for the CityStates we've met	
    -------------------------------------------------
	local iMinorMetCount = 0;
	local firstCityStateFound = nil;
	for iPlayerLoop = GameDefines.MAX_MAJOR_CIVS, GameDefines.MAX_CIV_PLAYERS-1, 1 do
		
		pOtherPlayer = Players[iPlayerLoop];
		iOtherTeam = pOtherPlayer:GetTeam();
		
		-- Valid player? - Can't be us, and has to be alive
		if (pPlayer:GetTeam() ~= iOtherTeam and pOtherPlayer:IsAlive()) then
			-- Met this player?
			if (pTeam:IsHasMet(iOtherTeam)) then
				if (pOtherPlayer:IsMinorCiv()) then
						iMinorMetCount = iMinorMetCount + 1;
						local controlTable = g_MinorCivButtonIM:GetInstance();
		
						local minorCivType = pOtherPlayer:GetMinorCivType();
						local civInfo = GameInfo.MinorCivilizations[minorCivType];
						
						local strDiploState = "";
				
						if (pTeam:IsAtWar(iOtherTeam)) then
							strDiploState = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_WAR" )
-- CBP
						elseif(pOtherPlayer:IsMarried(g_iUs)) then
							strDiploState = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_MARRIED" )
-- END
						end
						
						controlTable.LeaderName:SetText( Locale.ConvertTextKey( civInfo.Description ));
						controlTable.StatusText:SetText( strDiploState);
						
						-- Update colors
						local primaryColor, secondaryColor = pOtherPlayer:GetPlayerColors();
						primaryColor, secondaryColor = secondaryColor, primaryColor;
						local textColor = {x = primaryColor.x, y = primaryColor.y, z = primaryColor.z, w = 1};

						civType = pOtherPlayer:GetCivilizationType();
						civInfo = GameInfo.Civilizations[civType];
		
						local iconColor = textColor;		
						IconHookup( civInfo.PortraitIndex, 32, civInfo.AlphaIconAtlas, controlTable.LeaderPortrait );
						controlTable.LeaderPortrait:SetColor(iconColor);
						
						controlTable.LeaderButton:SetVoid1( iPlayerLoop);
						controlTable.LeaderButton:SetVoid2( city );
						controlTable.LeaderButton:RegisterCallback( Mouse.eLClick, MinorSelected );

						PopulateCityStateInfo(iPlayerLoop, controlTable);
						
						controlTable.CityStack:CalculateSize();
						controlTable.CityStack:ReprocessAnchoring();
	
						local buttonY = controlTable.CityStack:GetSizeY();
						controlTable.LeaderButton:SetSizeY(buttonY);
						controlTable.LeaderButtonHL:SetSizeY(buttonY);
				end
			end
		end
	end		

	if(iMinorMetCount > 0) then
		Controls.NoMinorCivs:SetHide(true);
	else
		Controls.NoMinorCivs:SetHide(false);
	end
	
	Controls.MinorButtonStack:CalculateSize();
	Controls.MinorButtonStack:ReprocessAnchoring();
	Controls.MinorCivScrollPanel:CalculateInternalSize();
end

function InitMajorCivList()
	-- Clear buttons
	g_MajorCivButtonIM:ResetInstances();
	
	for i, v in pairs(g_MajorCivTradeRowIMList) do
		v:ResetInstances();
	end
	g_MajorCivTradeRowIMList = {};
	
	local iPlayer = Game.GetActivePlayer();
	local pPlayer = Players[ iPlayer ];
	local pTeam = Teams[pPlayer:GetTeam()];
	local count = 0;
	
    --------------------------------------------------------------------
	-- Loop through all the Majors the active player knows
    --------------------------------------------------------------------
	for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
		
		pOtherPlayer = Players[iPlayerLoop];
		iOtherTeam = pOtherPlayer:GetTeam();
		
		-- Valid player? - Can't be us, and has to be alive
		if (iPlayerLoop ~= iPlayer and pOtherPlayer:IsAlive()) then

			-- Met this player?
			if (pTeam:IsHasMet(iOtherTeam)) then
				local controlTable = g_MajorCivButtonIM:GetInstance();
				count = count+1;
					-- Update colors
				local primaryColor, secondaryColor = pOtherPlayer:GetPlayerColors();
				--if pOtherPlayer:IsMinorCiv() then
					--primaryColor, secondaryColor = secondaryColor, primaryColor;
				--end					
				local backgroundColor = {x = secondaryColor.x, y = secondaryColor.y, z = secondaryColor.z, w = 1};
				local textColor = {x = primaryColor.x, y = primaryColor.y, z = primaryColor.z, w = 1};
				
				local civType = pOtherPlayer:GetCivilizationType();
				local civInfo = GameInfo.Civilizations[civType];
				local strCiv = Locale.ConvertTextKey(civInfo.ShortDescription);
				
				local leaderType = pOtherPlayer:GetLeaderType();
				local leaderInfo = GameInfo.Leaders[leaderType];

				controlTable.CivName:SetText(strCiv);

				CivIconHookup( iPlayerLoop, 32, controlTable.CivSymbol, controlTable.CivIconBG, controlTable.CivIconShadow, false, true );

				controlTable.CivIconBG:SetHide(false);
				
				IconHookup( leaderInfo.PortraitIndex, 64, leaderInfo.IconAtlas, controlTable.LeaderPortrait );

				local strDiploState;
				
				if (pTeam:IsAtWar(iOtherTeam)) then
					strDiploState = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_WAR" )	
				elseif (pOtherPlayer:IsDenouncingPlayer(iPlayer)) then
					strDiploState = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_DENOUNCING" );	
				elseif (pOtherPlayer:WasResurrectedThisTurnBy(iPlayer)) then
					strDiploState = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_LIBERATED" );					
				else
					local eApproachGuess = pPlayer:GetApproachTowardsUsGuess(iPlayerLoop);
					
					if( eApproachGuess == MajorCivApproachTypes.MAJOR_CIV_APPROACH_WAR ) then
						statusString = Locale.ConvertTextKey( "TXT_KEY_WAR_CAPS" );
					elseif( eApproachGuess == MajorCivApproachTypes.MAJOR_CIV_APPROACH_HOSTILE ) then
						statusString = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_HOSTILE", leaderInfo.Description  );
					elseif( eApproachGuess == MajorCivApproachTypes.MAJOR_CIV_APPROACH_GUARDED ) then
						statusString = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_GUARDED", leaderInfo.Description  );
					elseif( eApproachGuess == MajorCivApproachTypes.MAJOR_CIV_APPROACH_AFRAID ) then
						statusString = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_AFRAID", leaderInfo.Description  );
					elseif( eApproachGuess == MajorCivApproachTypes.MAJOR_CIV_APPROACH_FRIENDLY ) then
						statusString = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_FRIENDLY", leaderInfo.Description  );
					elseif( eApproachGuess == MajorCivApproachTypes.MAJOR_CIV_APPROACH_NEUTRAL ) then
						statusString = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_NEUTRAL", leaderInfo.Description );
					end
					
					PopulateTrade(iPlayerLoop, controlTable);
				end
				
				controlTable.DiploState:SetText(strDiploState);
				
				local strScore = pOtherPlayer:GetScore();
				controlTable.Score:SetText(strScore);
				controlTable.Score:SetHide(false);
				
				local textBoxSize = controlTable.LeaderNameStack:GetSizeX() - controlTable.DiploState:GetSizeX() -
									controlTable.Score:GetSizeX() - controlTable.CivIconBG:GetSizeX()- 74;

				
				--if(pOtherPlayer:GetNickName() ~= "" and Game:IsNetworkMultiPlayer()) then
				if(pOtherPlayer:GetNickName() ~= "" and pOtherPlayer:IsHuman()) then
					TruncateString(controlTable.LeaderName, textBoxSize, pOtherPlayer:GetNickName()); 
				else
					controlTable.LeaderName:LocalizeAndSetText( pOtherPlayer:GetNameKey() );
				end
				
				controlTable.DiploButton:SetVoid1( iPlayerLoop ); -- indicates type
				controlTable.DiploButton:RegisterCallback( Mouse.eLClick, LeaderSelected );
				
				controlTable.TradeStack:CalculateSize();
				controlTable.TradeStack:ReprocessAnchoring();
				
				local buttonY = controlTable.TradeStack:GetSizeY();
				controlTable.LeaderNameStack:SetSizeY(buttonY);
				
				controlTable.LeaderButton:CalculateSize();
				controlTable.LeaderButton:ReprocessAnchoring();
				
				buttonY = controlTable.LeaderButton:GetSizeY();
				controlTable.DiploButton:SetSizeY(buttonY);
				controlTable.DiploButtonHL:SetSizeY(buttonY);
			end
		end
	end

	if(count > 0) then
		Controls.NoMajorCivs:SetHide(true);
	else
		Controls.NoMajorCivs:SetHide(false);
	end
	
	Controls.ButtonStack:CalculateSize();
	Controls.ButtonStack:ReprocessAnchoring();
	Controls.MajorCivScrollPanel:CalculateInternalSize();
end

-------------------------------------------------
-- On Leader Selected
-------------------------------------------------
function LeaderSelected( ePlayer )

	if not Players[Game.GetActivePlayer()]:IsTurnActive() or Game.IsProcessingMessages() then
		return;
	end
	
    if( Players[ePlayer]:IsHuman() ) then
        Events.OpenPlayerDealScreenEvent( ePlayer );
    else
        
        UI.SetRepeatActionPlayer(ePlayer);
        UI.ChangeStartDiploRepeatCount(1);
    	Players[ePlayer]:DoBeginDiploWithHuman();    

	end
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function MinorSelected (PlayerID) 
	local popupInfo = {
		Type = ButtonPopupTypes.BUTTONPOPUP_CITY_STATE_DIPLO,
		Data1 = PlayerID;
		}
    
	Events.SerialEventGameMessagePopup( popupInfo );
end

function PopulateTrade(iPlayer, pStack)
	local g_MajorCivTradeRowIM = InstanceManager:new( "MajorCivTradeRowInstance", "Row", pStack.TradeStack );
	g_MajorCivTradeRowIMList[iPlayer] = g_MajorCivTradeRowIM;
	local iTeam = Players[iPlayer]:GetTeam();
	local pTeam = Teams[iTeam];
	
	local iItemToBeChanged = -1;	-- This is -1 because we're not changing anything right now
	local tradeRow = nil;
    ---------------------------------------------------------------------------------- 
    -- pocket Gold
    ---------------------------------------------------------------------------------- 
	-- Removed the negative check, as you can already see this in the diplo trade screen
	-- bGoldTradeAllowed = g_Deal:IsPossibleToTradeItem(iPlayer, g_iUs, TradeableItems.TRADE_ITEM_GOLD, 1);	-- 1 here is 1 Gold, which is the minimum possible
	
	-- if (bGoldTradeAllowed) then
		local iGold = g_Deal:GetGoldAvailable(iPlayer, iItemToBeChanged);
		local strGoldString = Locale.ConvertTextKey("TXT_KEY_DIPLO_GOLD") .." (" .. iGold .. "[ICON_GOLD])";
		tradeRow = AddTrade(strGoldString, tradeRow, g_MajorCivTradeRowIM);
	-- end
	---------------------------------------------------------------------------------- 
    -- pocket Gold Per Turn  
    ---------------------------------------------------------------------------------- 
	-- Removed the negative check, as you can already see this in the diplo trade screen
	-- local bGPTAllowed = g_Deal:IsPossibleToTradeItem(iPlayer, g_iUs, TradeableItems.TRADE_ITEM_GOLD_PER_TURN, 1, g_iDealDuration);	-- 1 here is 1 GPT, which is the minimum possible
	
    -- if (bGPTAllowed) then
		local iGoldPerTurn = Players[iPlayer]:CalculateGoldRate();
		local strGoldString = Locale.ConvertTextKey("TXT_KEY_DIPLO_GOLD_PER_TURN") .. " (" .. iGoldPerTurn .. "[ICON_GOLD])";
		tradeRow = AddTrade(strGoldString, tradeRow, g_MajorCivTradeRowIM);
	-- end
	---------------------------------------------------------------------------------- 	
    -- pocket Allow Embassy
    ----------------------------------------------------------------------------------
	local bEmbassyInYourCapital = pTeam:HasEmbassyAtTeam(g_iTeam);
	local bEmbassyInTheirCapital = g_pTeam:HasEmbassyAtTeam(iTeam);
    
    if (bEmbassyInYourCapital or bEmbassyInTheirCapital) then
		local strEmbassyTextKey = "";
		if (bEmbassyInYourCapital and bEmbassyInTheirCapital) then
			strEmbassyTextKey = "TXT_KEY_DIPLO_RELATION_EMBASSY_SHARED";
		elseif (bEmbassyInYourCapital) then
			strEmbassyTextKey = "TXT_KEY_DIPLO_RELATION_EMBASSY_YOUR";
		elseif (bEmbassyInTheirCapital) then
			strEmbassyTextKey = "TXT_KEY_DIPLO_RELATION_EMBASSY_THEIR";
		end
    
		tradeRow = AddTrade(Locale.ConvertTextKey(strEmbassyTextKey), tradeRow, g_MajorCivTradeRowIM);
    end
   
	---------------------------------------------------------------------------------- 
    -- pocket Open Borders
    ---------------------------------------------------------------------------------- 
    local bOpenBordersForMe = pTeam:IsAllowsOpenBordersToTeam(g_iTeam);
    local bOpenBordersForThem = g_pTeam:IsAllowsOpenBordersToTeam(iTeam);
    if (bOpenBordersForThem or bOpenBordersForMe) then
		local strOpenBordersKey = "";
		if (bOpenBordersForThem and bOpenBordersForMe) then
			strOpenBordersKey = "TXT_KEY_DIPLO_RELATION_OPEN_BORDERS_SHARED";
		elseif (bOpenBordersForMe) then	
			strOpenBordersKey = "TXT_KEY_DIPLO_RELATION_OPEN_BORDERS_THEIR";
		elseif (bOpenBordersForThem) then
			strOpenBordersKey = "TXT_KEY_DIPLO_RELATION_OPEN_BORDERS_YOUR";
		end
				
		tradeRow = AddTrade(Locale.ConvertTextKey(strOpenBordersKey), tradeRow, g_MajorCivTradeRowIM);
    end
	
	---------------------------------------------------------------------------------- 
    -- pocket Defensive Pact
    ---------------------------------------------------------------------------------- 
    if (g_pTeam:IsDefensivePact(iTeam)) then
		tradeRow = AddTrade(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEF_PACT"), tradeRow, g_MajorCivTradeRowIM);
    end
    ---------------------------------------------------------------------------------- 
    -- pocket Research Agreement
    ---------------------------------------------------------------------------------- 
    if (g_pTeam:IsHasResearchAgreement(iTeam)) then
		tradeRow = AddTrade(Locale.ConvertTextKey("TXT_KEY_DIPLO_RESCH_AGREEMENT"), tradeRow, g_MajorCivTradeRowIM);
    end
    ---------------------------------------------------------------------------------- 
    -- pocket Trade Agreement
    ---------------------------------------------------------------------------------- 
     bTradeAgreementAllowed = g_Deal:IsPossibleToTradeItem(iPlayer, g_iUs, TradeableItems.TRADE_ITEM_TRADE_AGREEMENT, g_iDealDuration);  
    -- Are they not allowed to give RA? (don't have tech, or are already providing it to us)
    if (bTradeAgreementAllowed) then
		tradeRow = AddTrade(Locale.ConvertTextKey("TXT_KEY_DIPLO_TRADE_AGREEMENT"), tradeRow, g_MajorCivTradeRowIM);
    end
    ---------------------------------------------------------------------------------- 
    -- pocket resources for them
    ---------------------------------------------------------------------------------- 
	local strategicCount = 0;
	local luxuryCount = 0;
	local strStrategic = Locale.ConvertTextKey("TXT_KEY_DIPLO_STRATEGIC_RESOURCES");
	local strLuxury = Locale.ConvertTextKey("TXT_KEY_DIPLO_LUXURY_RESOURCES");
	for pResource in GameInfo.Resources() do
		iResourceLoop = pResource.ID;
		local bCanTradeResource = g_Deal:IsPossibleToTradeItem(iPlayer, g_iUs, TradeableItems.TRADE_ITEM_RESOURCES, iResourceLoop, 1);	-- 1 here is 1 quanity of the Resource, which is the minimum possible

		if(bCanTradeResource) then
			v = GameInfo.Resources[iResourceLoop];
			local iResourceCount = Players[iPlayer]:GetNumResourceAvailable( iResourceLoop, false );

			if(v.ResourceClassType == "RESOURCECLASS_LUXURY") then
				strLuxury = strLuxury .. " " .. iResourceCount  ..v.IconString ;
				luxuryCount = luxuryCount + 1;
			else
				strStrategic = strStrategic .. " " .. iResourceCount  ..v.IconString;
				strategicCount = strategicCount + 1;
			end
		end
	end
	
	if(strategicCount > 0) then
		tradeRow = AddTrade(strStrategic, tradeRow, g_MajorCivTradeRowIM);
	end
	
	if(luxuryCount > 0) then
		tradeRow = AddTrade(strLuxury, tradeRow, g_MajorCivTradeRowIM);
	end
end

function AddTrade(tradeString, tradeRow, tradeMgr)
	if(tradeRow == nil) then
		tradeRow = tradeMgr:GetInstance();
		tradeRow.Item0:SetText(tradeString);
	else
		tradeRow.Item1:SetText(tradeString);
		tradeRow = nil;
	end	
	return tradeRow;
end

function PopulateCityStateInfo(iPlayer, pStack)
	local pPlayer = Players[iPlayer];
	
	local iActivePlayer = Game.GetActivePlayer();
	local strShortDescKey = pPlayer:GetCivilizationShortDescriptionKey();
	
	-- At war?
	local bWar = Teams[Game.GetActiveTeam()]:IsAtWar(pPlayer:GetTeam());
	
	local sMinorCivType = pPlayer:GetMinorCivType();
	local strStatusTT = GetCityStateStatusToolTip(iActivePlayer, iPlayer, true);
	local trait = GameInfo.MinorCivilizations[sMinorCivType].MinorCivTrait;
	pStack.StatusIcon:SetTexture(GameInfo.MinorCivTraits[trait].TraitIcon);
	UpdateCityStateStatusUI(iActivePlayer, iPlayer, pStack.PositiveStatusMeter, pStack.NegativeStatusMeter, pStack.StatusMeterMarker, pStack.StatusIconBG);
	pStack.StatusIconBG:SetToolTipString(strStatusTT);
	pStack.PositiveStatusMeter:SetToolTipString(strStatusTT);
	pStack.NegativeStatusMeter:SetToolTipString(strStatusTT);
	
	-- Trait
	local strTraitText = GetCityStateTraitText(iPlayer);
	local strTraitTT = GetCityStateTraitToolTip(iPlayer);
	
	strTraitText = "[COLOR_POSITIVE_TEXT]" .. strTraitText .. "[ENDCOLOR]";
	
	pStack.TraitInfo:SetText(strTraitText);
	pStack.TraitInfo:SetToolTipString(strTraitTT);
	pStack.TraitLabel:SetToolTipString(strTraitTT);
	
		-- Personality
	local strPersonalityText = "";
	local strPersonalityTT = "";
	local iPersonality = pPlayer:GetPersonality();
	if (iPersonality == MinorCivPersonalityTypes.MINOR_CIV_PERSONALITY_FRIENDLY) then
		strPersonalityText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_PERSONALITY_FRIENDLY");
		strPersonalityTT = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_PERSONALITY_FRIENDLY_TT");
	elseif (iPersonality == MinorCivPersonalityTypes.MINOR_CIV_PERSONALITY_NEUTRAL) then
		strPersonalityText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_PERSONALITY_NEUTRAL");
		strPersonalityTT = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_PERSONALITY_NEUTRAL_TT");
	elseif (iPersonality == MinorCivPersonalityTypes.MINOR_CIV_PERSONALITY_HOSTILE) then
		strPersonalityText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_PERSONALITY_HOSTILE");
		strPersonalityTT = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_PERSONALITY_HOSTILE_TT");
	elseif (iPersonality == MinorCivPersonalityTypes.MINOR_CIV_PERSONALITY_IRRATIONAL) then
		strPersonalityText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_PERSONALITY_IRRATIONAL");
		strPersonalityTT = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_PERSONALITY_IRRATIONAL_TT");
	end
	
	strPersonalityText = "[COLOR_POSITIVE_TEXT]" .. strPersonalityText .. "[ENDCOLOR]";
	
	pStack.PersonalityInfo:SetText(strPersonalityText);
	pStack.PersonalityInfo:SetToolTipString(strPersonalityTT);
	pStack.PersonalityLabel:SetToolTipString(strPersonalityTT);
	
	-- Allied with anyone?
	local strAlly = GetAllyText(iActivePlayer, iPlayer);
	pStack.AllyInfo:SetText(strAlly);
	local strAllyTT = GetAllyToolTip(iActivePlayer, iPlayer);
	pStack.AllyInfo:SetToolTipString(strAllyTT);
	pStack.AllyLabel:SetToolTipString(strAllyTT);
	
		-- Nearby Resources
	local pCapital = pPlayer:GetCapitalCity();
	
	local strResourceText = "";
	
	if (pCapital ~= nil) then
		
		local iNumResourcesFound = 0;
		
		local thisX = pCapital:GetX();
		local thisY = pCapital:GetY();
		
		local iRange = 5;
		local iCloseRange = 2;
		
		for iDX = -iRange, iRange, 1 do
			for iDY = -iRange, iRange, 1 do
				local pTargetPlot = Map.GetPlotXY(thisX, thisY, iDX, iDY);
				
				if pTargetPlot ~= nil then
					
					local iOwner = pTargetPlot:GetOwner();
					
					if (iOwner == iPlayer or iOwner == -1) then
						local plotX = pTargetPlot:GetX();
						local plotY = pTargetPlot:GetY();
						local plotDistance = Map.PlotDistance(thisX, thisY, plotX, plotY);
						
						if (plotDistance <= iRange and (plotDistance <= iCloseRange or iOwner == iPlayer)) then
							
							local iResourceType = pTargetPlot:GetResourceType(Game.GetActiveTeam());
							
							if (iResourceType ~= -1) then
								
								if (Game.GetResourceUsageType(iResourceType) ~= ResourceUsageTypes.RESOURCEUSAGE_BONUS) then
									
									-- Add spacing if we already have found an entry
									if (iNumResourcesFound > 0) then
										strResourceText = strResourceText .. ", ";
									end
									
									local pResource = GameInfo.Resources[iResourceType];
									strResourceText = strResourceText .. pResource.IconString .. " [COLOR_POSITIVE_TEXT]" .. Locale.ConvertTextKey(pResource.Description) .. " (" .. pTargetPlot:GetNumResource() .. ") [ENDCOLOR]";
									
									iNumResourcesFound = iNumResourcesFound + 1;
									
								end
							end
						end
					end
					
				end
			end
		end
		
		pStack.ResourcesInfo:SetText(strResourceText);
		local resourceY = pStack.ResourcesInfo:GetSizeY();
		pStack.ResourcesLabel:SetSizeY(resourceY);
		local strResourceTextTT = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_RESOURCES_TT");
		pStack.ResourcesInfo:SetToolTipString(strResourceTextTT);
		pStack.ResourcesLabel:SetToolTipString(strResourceTextTT);
	end
end