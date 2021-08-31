print("This is the modded CityStateDiploPopup from 'Global - City State Gifts'")

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-- City State Diplo Popup
--
-- Authors: Anton Strenger
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

include( "IconSupport" );
include( "InfoTooltipInclude" );
include( "CityStateStatusHelper" );

local g_iMinorCivID = -1;
local g_iMinorCivTeamID = -1;
local m_PopupInfo = nil;
local m_bNewQuestAvailable = false;
local lastBackgroundImage = "citystatebackgroundculture.dds"
local WordWrapOffset = 19;
local WordWrapAnimOffset = 3;

local kiNoAction = 0;
local kiMadePeace = 1;
local kiBulliedGold = 2;
local kiBulliedUnit = 3;
local kiGiftedGold = 4;
local kiPledgedToProtect = 5;
local kiDeclaredWar = 6;
local kiRevokedProtection = 7;
local m_iLastAction = kiNoAction;
local m_iPendingAction = kiNoAction; -- For bullying dialog popups

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-- HANDLERS AND HELPERS
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function SetButtonSize(textControl, buttonControl, animControl, buttonHL)

	--print(textControl:GetText());
	local sizeY = textControl:GetSizeY() + WordWrapOffset;
	buttonControl:SetSizeY(sizeY);
	animControl:SetSizeY(sizeY+WordWrapAnimOffset);
	buttonHL:SetSizeY(sizeY+WordWrapAnimOffset);
end

function UpdateButtonStack()
	Controls.GiveStack:CalculateSize();
    Controls.GiveStack:ReprocessAnchoring();
    
    Controls.TakeStack:CalculateSize();
    Controls.TakeStack:ReprocessAnchoring();
    
    Controls.ButtonStack:CalculateSize();
    Controls.ButtonStack:ReprocessAnchoring();
    
	Controls.ButtonScrollPanel:CalculateInternalSize();
end

function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
			if (Controls.BullyConfirm:IsHidden()) then
	            OnCloseButtonClicked();
			else
				m_iPendingAction = kiNoAction;
				Controls.BullyConfirm:SetHide(true);
            	Controls.BGBlock:SetHide(false);
			end
			return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );

function ShowHideHandler( bIsHide, bInitState )

    if( not bInitState ) then
 	   Controls.BackgroundImage:UnloadTexture();
       if( not bIsHide ) then
			Controls.BackgroundImage:SetTexture(lastBackgroundImage);
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
        else
            UI.decTurnTimerSemaphore();
            
            if(m_PopupInfo ~= nil) then
				Events.SerialEventGameMessagePopupProcessed.CallImmediate(m_PopupInfo.Type, 0);
			end
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );



-------------------------------------------------
-- On Event Received
-------------------------------------------------
function OnEventReceived( popupInfo )
	
	local bGreeting = popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_CITY_STATE_GREETING;
	local bMessage = popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_CITY_STATE_MESSAGE;
	local bDiplo = popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_CITY_STATE_DIPLO;
	
	if(not bMessage and not bDiplo) then
		return;
	end
	
	m_PopupInfo = popupInfo;	
	
    local iPlayer = popupInfo.Data1;
    local pPlayer = Players[iPlayer];
	local iTeam = pPlayer:GetTeam();
	local pTeam = Teams[iTeam];
	
	local iQuestFlags = popupInfo.Data2;
    
    g_iMinorCivID = iPlayer;
    g_iMinorCivTeamID = iTeam;
	
	m_iLastAction = kiNoAction;
	m_iPendingAction = kiNoAction;
	
	if (iQuestFlags == 1) then
		m_bNewQuestAvailable = true;
	else
		m_bNewQuestAvailable = false;
	end
	
	OnDisplay();
	
	UIManager:QueuePopup( ContextPtr, PopupPriority.CityStateDiplo );
end
Events.SerialEventGameMessagePopup.Add( OnEventReceived );

-------------------------------------------------
-- On Game Info Dirty
-------------------------------------------------
function OnGameDataDirty()

	if (ContextPtr:IsHidden()) then
		return;
	end
	
	OnDisplay();
	
end
Events.SerialEventGameDataDirty.Add(OnGameDataDirty);

-------------------------------------------------
-- On Display
-------------------------------------------------
function OnDisplay()
    
	-- print("Displaying City State Diplo Popup");
    
    local iActivePlayer = Game.GetActivePlayer();
    local pActivePlayer = Players[iActivePlayer];
    local iActiveTeam = Game.GetActiveTeam();
    local pActiveTeam = Teams[iActiveTeam];
    
    local iPlayer = g_iMinorCivID;
    local pPlayer = Players[iPlayer];
	local iTeam = g_iMinorCivTeamID;
	local pTeam = Teams[iTeam];
	local sMinorCivType = pPlayer:GetMinorCivType();
	
	local strShortDescKey = pPlayer:GetCivilizationShortDescriptionKey();
	
	local bAllies = pPlayer:IsAllies(iActivePlayer);
	local bFriends = pPlayer:IsFriends(iActivePlayer);
	
	-- At war?
	local bWar = pActiveTeam:IsAtWar(iTeam);

	-- Update colors
	local primaryColor, secondaryColor = pPlayer:GetPlayerColors();
	primaryColor, secondaryColor = secondaryColor, primaryColor;
	local textColor = {x = primaryColor.x, y = primaryColor.y, z = primaryColor.z, w = 1};

	-- Title
	strTitle = Locale.ConvertTextKey("{"..pPlayer:GetCivilizationShortDescriptionKey()..":upper}");
	Controls.TitleLabel:SetText(strTitle);
	Controls.TitleLabel:SetColor(textColor, 0);
	
	civType = pPlayer:GetCivilizationType();
	civInfo = GameInfo.Civilizations[civType];

	local trait = GameInfo.MinorCivilizations[sMinorCivType].MinorCivTrait;
	Controls.TitleIcon:SetTexture(GameInfo.MinorCivTraits[trait].TraitTitleIcon);
	
	-- Set Background Image
	lastBackgroundImage = GameInfo.MinorCivTraits[trait].BackgroundImage;
	Controls.BackgroundImage:SetTexture(lastBackgroundImage);
	
	local iconColor = textColor;
	IconHookup( civInfo.PortraitIndex, 32, civInfo.AlphaIconAtlas, Controls.CivIcon );
	Controls.CivIcon:SetColor(iconColor);
	
	local strStatusText = GetCityStateStatusText(iActivePlayer, iPlayer);
	local strStatusTT = GetCityStateStatusToolTip(iActivePlayer, iPlayer, true);
	Controls.StatusIcon:SetTexture(GameInfo.MinorCivTraits[trait].TraitIcon);
	UpdateCityStateStatusUI(iActivePlayer, iPlayer, Controls.PositiveStatusMeter, Controls.NegativeStatusMeter, Controls.StatusMeterMarker, Controls.StatusIconBG);
	Controls.StatusInfo:SetText(strStatusText);
	Controls.StatusInfo:SetToolTipString(strStatusTT);
	Controls.StatusLabel:SetToolTipString(strStatusTT);
	Controls.StatusIconBG:SetToolTipString(strStatusTT);
	Controls.PositiveStatusMeter:SetToolTipString(strStatusTT);
	Controls.NegativeStatusMeter:SetToolTipString(strStatusTT);
-- CBP
	Controls.MarriedButton:SetHide(true);
	if(pPlayer:IsMarried(iActivePlayer))then
		Controls.MarriedButton:SetHide(false);
		Controls.MarriedButton:SetText(Locale.ConvertTextKey("TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_MARRIED_SMALL"))
		Controls.MarriedButton:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_MARRIED_TT"))
	end
-- END
	-- Trait
	local strTraitText = GetCityStateTraitText(iPlayer);
	local strTraitTT = GetCityStateTraitToolTip(iPlayer);
	
	strTraitText = "[COLOR_POSITIVE_TEXT]" .. strTraitText .. "[ENDCOLOR]";
	
	Controls.TraitInfo:SetText(strTraitText);
	Controls.TraitInfo:SetToolTipString(strTraitTT);
	Controls.TraitLabel:SetToolTipString(strTraitTT);
	
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
	
	Controls.PersonalityInfo:SetText(strPersonalityText);
	Controls.PersonalityInfo:SetToolTipString(strPersonalityTT);
	Controls.PersonalityLabel:SetToolTipString(strPersonalityTT);
	
	-- Ally Status
	local iAlly = pPlayer:GetAlly();
	local bHideIcon = true;
	local bHideText = true;
	if (iAlly ~= nil and iAlly ~= -1) then
		if (iAlly ~= iActivePlayer) then
			if (Teams[Players[iAlly]:GetTeam()]:IsHasMet(Game.GetActiveTeam())) then
				bHideIcon = false;
				CivIconHookup(iAlly, 32, Controls.AllyIcon, Controls.AllyIconBG, Controls.AllyIconShadow, false, true);
			else
				bHideIcon = false;
				CivIconHookup(-1, 32, Controls.AllyIcon, Controls.AllyIconBG, Controls.AllyIconShadow, false, true);
			end
		else
			bHideText = false;
		end
	else
		bHideText = false;
	end
	local strAlly = GetAllyText(iActivePlayer, iPlayer);
	Controls.AllyText:SetText(strAlly);
	local strAllyTT = GetAllyToolTip(iActivePlayer, iPlayer);
	Controls.AllyIcon:SetToolTipString(strAllyTT);
	Controls.AllyIconBG:SetToolTipString(strAllyTT);
	Controls.AllyIconShadow:SetToolTipString(strAllyTT);
	Controls.AllyText:SetToolTipString(strAllyTT);
	Controls.AllyLabel:SetToolTipString(strAllyTT);
	Controls.AllyIconContainer:SetHide(bHideIcon);
	Controls.AllyText:SetHide(bHideText);
	
	-- Vox Populi - Contender info
	Controls.ContenderInfo:SetText( GetContenderInfo(activePlayerID, g_iMinorCivID) )
	
	-- Protected by anyone?
	local sProtectingPlayers = getProtectingPlayers(g_iMinorCivID);

	if (sProtectingPlayers ~= "") then
		Controls.ProtectInfo:SetText("[COLOR_POSITIVE_TEXT]" .. sProtectingPlayers .. "[ENDCOLOR]");
	else
		Controls.ProtectInfo:SetText(Locale.ConvertTextKey("TXT_KEY_CITY_STATE_NOBODY"));
	end
	
	Controls.ProtectInfo:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_POP_CSTATE_PROTECTED_BY_TT"));
	Controls.ProtectLabel:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_POP_CSTATE_PROTECTED_BY_TT"));
	
	-- Nearby Resources
	local pCapital = pPlayer:GetCapitalCity();
	if (pCapital ~= nil) then
		
		local strResourceText = "";
		
		local iNumResourcesFound = 0;
		
		local thisX = pCapital:GetX();
		local thisY = pCapital:GetY();
		
		local iRange = GameDefines["MINOR_CIV_RESOURCE_SEARCH_RADIUS"]; --5
		local iCloseRange = math.floor(iRange/2); --2
		local tResourceList = {};
		
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
									
									if (tResourceList[iResourceType] == nil) then
										tResourceList[iResourceType] = 0;
									end
									
									tResourceList[iResourceType] = tResourceList[iResourceType] + pTargetPlot:GetNumResource();
									
								end
							end
						end
					end
					
				end
			end
		end
		
		for iResourceType, iAmount in pairs(tResourceList) do
			if (iNumResourcesFound > 0) then
				strResourceText = strResourceText .. ", ";
			end
			local pResource = GameInfo.Resources[iResourceType];
			strResourceText = strResourceText .. pResource.IconString .. " [COLOR_POSITIVE_TEXT]" .. Locale.ConvertTextKey(pResource.Description) .. " (" .. iAmount .. ") [ENDCOLOR]";
			iNumResourcesFound = iNumResourcesFound + 1;
		end	
		
		Controls.ResourcesInfo:SetText(strResourceText);
		
		Controls.ResourcesLabel:SetHide(false);
		Controls.ResourcesInfo:SetHide(false);
		
		local strResourceTextTT = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_RESOURCES_TT");
		Controls.ResourcesInfo:SetToolTipString(strResourceTextTT);
		Controls.ResourcesLabel:SetToolTipString(strResourceTextTT);
		
	else
		Controls.ResourcesLabel:SetHide(true);
		Controls.ResourcesInfo:SetHide(true);
	end

	-- Body text
	local strText;
	
	-- Active Quests
	UpdateActiveQuests();
	
	-- Peace
	if (not bWar) then
		
		-- Were we sent here because we clicked a notification for a new quest?
		if (m_iLastAction == kiNoAction and m_bNewQuestAvailable) then
			strText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_DIPLO_HELLO_QUEST_MESSAGE");
		
		-- Did we just make peace?
		elseif (m_iLastAction == kiMadePeace) then
			strText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_DIPLO_PEACE_JUST_MADE");
			
		-- Did we just bully gold?
		elseif (m_iLastAction == kiBulliedGold) then
			strText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_DIPLO_JUST_BULLIED");
		
		-- Did we just bully a worker?
		elseif (m_iLastAction == kiBulliedUnit) then
			strText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_DIPLO_JUST_BULLIED_WORKER");
		
		-- Did we just give gold?
		elseif (m_iLastAction == kiGiftedGold) then
			strText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_DIPLO_JUST_SUPPORTED");
		
		-- Did we just PtP?
		elseif (m_iLastAction == kiPledgedToProtect) then
			strText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_PLEDGE_RESPONSE");
		
		-- Did we just revoke a PtP?
		elseif (m_iLastAction == kiRevokedProtection) then
			strText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_DIPLO_JUST_REVOKED_PROTECTION");
		
		-- Normal peaceful hello, with info about active quests
		else
			local iPersonality = pPlayer:GetPersonality();
			
			if (pPlayer:IsProtectedByMajor(iActivePlayer)) then
				strText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_DIPLO_HELLO_PEACE_PROTECTED");
			elseif (iPersonality == MinorCivPersonalityTypes.MINOR_CIV_PERSONALITY_FRIENDLY) then
				strText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_DIPLO_HELLO_PEACE_FRIENDLY");
			elseif (iPersonality == MinorCivPersonalityTypes.MINOR_CIV_PERSONALITY_NEUTRAL) then
				strText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_DIPLO_HELLO_PEACE_NEUTRAL");
			elseif (iPersonality == MinorCivPersonalityTypes.MINOR_CIV_PERSONALITY_HOSTILE) then
				strText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_DIPLO_HELLO_PEACE_HOSTILE");
			elseif (iPersonality == MinorCivPersonalityTypes.MINOR_CIV_PERSONALITY_IRRATIONAL) then
				strText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_DIPLO_HELLO_PEACE_IRRATIONAL");
			end
		
			local strQuestString = "";
			local strWarString = "";
			
			local iNumPlayersAtWar = 0;
			
			-- Loop through all the Majors the active player knows
			for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
				
				pOtherPlayer = Players[iPlayerLoop];
				iOtherTeam = pOtherPlayer:GetTeam();
				
				-- Don't test war with active player!
				if (iPlayerLoop ~= iActivePlayer) then
					if (pOtherPlayer:IsAlive()) then
						if (pTeam:IsAtWar(iOtherTeam)) then
							if (pPlayer:IsMinorWarQuestWithMajorActive(iPlayerLoop)) then
								if (iNumPlayersAtWar ~= 0) then
									strWarString = strWarString .. ", "
								end
								strWarString = strWarString .. Locale.ConvertTextKey(pOtherPlayer:GetNameKey());
								
								iNumPlayersAtWar = iNumPlayersAtWar + 1;
							end
						end
					end
				end
			end
		end
		
		-- Tell the City State to stop gifting us Units (if they are)
		Controls.NoUnitSpawningButton:SetHide(true);
		if (pPlayer:GetMinorCivTrait() == MinorCivTraitTypes.MINOR_CIV_TRAIT_MILITARISTIC) then
			if (bFriends) then
				Controls.NoUnitSpawningButton:SetHide(false);
				
				-- Player has said to turn it off
				local strSpawnText;
				if (pPlayer:IsMinorCivUnitSpawningDisabled(iActivePlayer)) then
					strSpawnText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_TURN_SPAWNING_ON");
				else
					strSpawnText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_TURN_SPAWNING_OFF");
				end
				
				Controls.NoUnitSpawningLabel:SetText(strSpawnText);
			end
		end
		
		Controls.GiveButton:SetHide(false);
		Controls.TakeButton:SetHide(false);
		Controls.PeaceButton:SetHide(true);
		Controls.WarButton:SetHide(false);
		
	-- War
	else
		
		-- Warmongering player
		if (pPlayer:IsPeaceBlocked(iActiveTeam)) then
			strText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_DIPLO_HELLO_WARMONGER");
			Controls.PeaceButton:SetHide(true);
			
		-- Normal War
		else
			strText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_DIPLO_HELLO_WAR");
			Controls.PeaceButton:SetHide(false);
		end
		
		Controls.GiveButton:SetHide(true);
		Controls.TakeButton:SetHide(true);
		Controls.WarButton:SetHide(true);
		Controls.NoUnitSpawningButton:SetHide(true);
		
	end
	
	-- Pledge to Protect
	local bShowPledgeButton = false;
	local bEnablePledgeButton = false;
	local bShowRevokeButton = false;
	local bEnableRevokeButton = false;
	local strProtectButton = Locale.Lookup("TXT_KEY_POP_CSTATE_PLEDGE_TO_PROTECT");
-- CBP
	local strProtectTT = Locale.Lookup("TXT_KEY_POP_CSTATE_PLEDGE_TT", GameDefines.MINOR_FRIENDSHIP_ANCHOR_MOD_PROTECTED, GameDefines.BALANCE_MINOR_PROTECTION_MINIMUM_DURATION, GameDefines.BALANCE_INFLUENCE_BOOST_PROTECTION_MINOR, GameDefines.BALANCE_CS_PLEDGE_TO_PROTECT_DEFENSE_BONUS, GameDefines.BALANCE_CS_PLEDGE_TO_PROTECT_DEFENSE_BONUS_MAX);
	--local strProtectTT = Locale.Lookup("TXT_KEY_POP_CSTATE_PLEDGE_TT", GameDefines.MINOR_FRIENDSHIP_ANCHOR_MOD_PROTECTED, 10); --antonjs: todo: xml
--END
	local strRevokeProtectButton = Locale.Lookup("TXT_KEY_POP_CSTATE_REVOKE_PROTECTION");
	local strRevokeProtectTT = Locale.Lookup("TXT_KEY_POP_CSTATE_REVOKE_PROTECTION_TT");
	
	if (not bWar) then
		-- PtP in effect
		if (pPlayer:IsProtectedByMajor(iActivePlayer)) then
			bShowRevokeButton = true;
			-- Can we revoke it?
			if (pPlayer:CanMajorWithdrawProtection(iActivePlayer)) then
				bEnableRevokeButton = true;
			else
				bEnableRevokeButton = false;
				strRevokeProtectButton = "[COLOR_WARNING_TEXT]" .. strRevokeProtectButton .. "[ENDCOLOR]";
-- CBP
				local iTurnsCommitted = (pPlayer:GetTurnLastPledgedProtectionByMajor(iActivePlayer) + GameDefines.BALANCE_MINOR_PROTECTION_MINIMUM_DURATION) - Game.GetGameTurn();
				--local iTurnsCommitted = (pPlayer:GetTurnLastPledgedProtectionByMajor(iActivePlayer) + 10) - Game.GetGameTurn(); --antonjs: todo: xml
-- END
				strRevokeProtectTT = strRevokeProtectTT .. Locale.Lookup("TXT_KEY_POP_CSTATE_REVOKE_PROTECTION_DISABLED_COMMITTED_TT", iTurnsCommitted);
			end
		-- No PtP
		else
			bShowPledgeButton = true;
			-- Can we pledge?
			if (pPlayer:CanMajorStartProtection(iActivePlayer)) then
				bEnablePledgeButton = true;
			else
				bEnablePledgeButton = false;
				strProtectTT = strProtectTT .. pPlayer:GetPledgeProtectionInvalidReason(iActivePlayer);
				strProtectButton = "[COLOR_WARNING_TEXT]" .. strProtectButton .. "[ENDCOLOR]";
			end
		end
	end
	Controls.PledgeAnim:SetHide(not bEnablePledgeButton);
	Controls.PledgeButton:SetHide(not bShowPledgeButton);
	if (bShowPledgeButton) then
		Controls.PledgeLabel:SetText(strProtectButton);
		Controls.PledgeButton:SetToolTipString(strProtectTT);
	end
	Controls.RevokePledgeAnim:SetHide(not bEnableRevokeButton);
	Controls.RevokePledgeButton:SetHide(not bShowRevokeButton);
	if (bShowRevokeButton) then
		Controls.RevokePledgeLabel:SetText(strRevokeProtectButton);
		Controls.RevokePledgeButton:SetToolTipString(strRevokeProtectTT);
	end
	
	if (Game.IsOption(GameOptionTypes.GAMEOPTION_ALWAYS_WAR)) then
		Controls.PeaceButton:SetHide(true);
	end
	if (Game.IsOption(GameOptionTypes.GAMEOPTION_ALWAYS_PEACE)) then
		Controls.WarButton:SetHide(true);
	end
	if (Game.IsOption(GameOptionTypes.GAMEOPTION_NO_CHANGING_WAR_PEACE)) then
		Controls.PeaceButton:SetHide(true);
		Controls.WarButton:SetHide(true);
	end
	
	-- Buyout (Austria UA)
	local iBuyoutCost = pPlayer:GetBuyoutCost(iActivePlayer);
	local strButtonLabel = Locale.ConvertTextKey( "TXT_KEY_POP_CSTATE_BUYOUT");
	local strToolTip = Locale.ConvertTextKey( "TXT_KEY_POP_CSTATE_BUYOUT_TT", iBuyoutCost );
	if (pPlayer:CanMajorBuyout(iActivePlayer) and not bWar) then
		Controls.BuyoutButton:SetHide(false);
		Controls.BuyoutAnim:SetHide(false);
	elseif (pActivePlayer:IsAbleToAnnexCityStates() and not bWar) then
		if (pPlayer:GetAlly() == iActivePlayer) then
			local iAllianceTurns = pPlayer:GetAlliedTurns();
			strButtonLabel = "[COLOR_WARNING_TEXT]" .. strButtonLabel .. "[ENDCOLOR]";
			strToolTip = Locale.ConvertTextKey("TXT_KEY_POP_CSTATE_BUYOUT_DISABLED_ALLY_TT", GameDefines.MINOR_CIV_BUYOUT_TURNS, iAllianceTurns, iBuyoutCost);
		else
			strButtonLabel = "[COLOR_WARNING_TEXT]" .. strButtonLabel .. "[ENDCOLOR]";
			strToolTip = Locale.ConvertTextKey("TXT_KEY_POP_CSTATE_BUYOUT_DISABLED_TT", GameDefines.MINOR_CIV_BUYOUT_TURNS, iBuyoutCost);
		end
		--antonjs: todo: disable button entirely, in case bWar doesn't update in time for the callback to disallow buyout in wartime
		Controls.BuyoutButton:SetHide(false);
		Controls.BuyoutAnim:SetHide(true);
	else
		Controls.BuyoutButton:SetHide(true);
	end
	Controls.BuyoutLabel:SetText( strButtonLabel )
	Controls.BuyoutButton:SetToolTipString( strToolTip );
	-- CBP
	if(not pPlayer:IsMarried(iActivePlayer)) then
		iBuyoutCost = pPlayer:GetMarriageCost(iActivePlayer);
		local strButtonLabel = Locale.ConvertTextKey( "TXT_KEY_POP_CSTATE_BUYOUT");
		local strToolTip = Locale.ConvertTextKey( "TXT_KEY_POP_CSTATE_MARRIAGE_TT", iBuyoutCost );
		if(pPlayer:CanMajorMarry(iActivePlayer) and not bWar) then	
			Controls.MarriageButton:SetHide(false);
			Controls.MarriageAnim:SetHide(false);
		elseif (pActivePlayer:IsDiplomaticMarriage() and not bWar) then
			if (pPlayer:GetAlly() == iActivePlayer) then
				local iAllianceTurns = pPlayer:GetAlliedTurns();
				strButtonLabel = "[COLOR_WARNING_TEXT]" .. strButtonLabel .. "[ENDCOLOR]";
				strToolTip = Locale.ConvertTextKey("TXT_KEY_POP_CSTATE_MARRIAGE_DISABLED_ALLY_TT", GameDefines.MINOR_CIV_BUYOUT_TURNS, iAllianceTurns, iBuyoutCost);
			else
				strButtonLabel = "[COLOR_WARNING_TEXT]" .. strButtonLabel .. "[ENDCOLOR]";
				strToolTip = Locale.ConvertTextKey("TXT_KEY_POP_CSTATE_MARRIAGE_DISABLED_TT", GameDefines.MINOR_CIV_BUYOUT_TURNS, iBuyoutCost);
			end
			--antonjs: todo: disable button entirely, in case bWar doesn't update in time for the callback to disallow buyout in wartime
			Controls.MarriageButton:SetHide(false);
			Controls.MarriageAnim:SetHide(true);
		else
			Controls.MarriageButton:SetHide(true);
		end
		Controls.MarriageLabel:SetText( strButtonLabel );
		Controls.MarriageButton:SetToolTipString( strToolTip );
	else
		Controls.MarriageButton:SetHide(true);
	end
	-- END
	
	
	Controls.DescriptionLabel:SetText(strText);
	
	SetButtonSize(Controls.PeaceLabel, Controls.PeaceButton, Controls.PeaceAnim, Controls.PeaceButtonHL);
	SetButtonSize(Controls.GiveLabel, Controls.GiveButton, Controls.GiveAnim, Controls.GiveButtonHL);
	SetButtonSize(Controls.TakeLabel, Controls.TakeButton, Controls.TakeAnim, Controls.TakeButtonHL);
	SetButtonSize(Controls.WarLabel, Controls.WarButton, Controls.WarAnim, Controls.WarButtonHL);
	SetButtonSize(Controls.PledgeLabel, Controls.PledgeButton, Controls.PledgeAnim, Controls.PledgeButtonHL);
	SetButtonSize(Controls.RevokePledgeLabel, Controls.RevokePledgeButton, Controls.RevokePledgeAnim, Controls.RevokePledgeButtonHL);
	SetButtonSize(Controls.NoUnitSpawningLabel, Controls.NoUnitSpawningButton, Controls.NoUnitSpawningAnim, Controls.NoUnitSpawningButtonHL);
	SetButtonSize(Controls.BuyoutLabel, Controls.BuyoutButton, Controls.BuyoutAnim, Controls.BuyoutButtonHL);
	--CBP
	SetButtonSize(Controls.MarriageLabel, Controls.MarriageButton, Controls.MarriageAnim, Controls.MarriageButtonHL)
	-- END
	Controls.GiveStack:SetHide(true);
	Controls.TakeStack:SetHide(true);
	Controls.ButtonStack:SetHide(false);
	
	UpdateButtonStack();
end

----------------------------------------------------------------
----------------------------------------------------------------
function getProtectingPlayers(iMinorCivID)
	local sProtecting = "";
	
	for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
		pOtherPlayer = Players[iPlayerLoop];

		if (iPlayerLoop ~= Game.GetActivePlayer()) then
			if (pOtherPlayer:IsAlive()) then
				if(Teams[Game.GetActiveTeam()]:IsHasMet(pOtherPlayer:GetTeam())) then
					if (pOtherPlayer:IsProtectingMinor(iMinorCivID)) then
						if (sProtecting ~= "") then
							sProtecting = sProtecting .. ", "
						end

						sProtecting = sProtecting .. Locale.ConvertTextKey(Players[iPlayerLoop]:GetCivilizationShortDescriptionKey());
					end
				end
			end
		end
	end

	return sProtecting
end

function UpdateActiveQuests()
	local iActivePlayer = Game.GetActivePlayer();
	local sIconText = GetActiveQuestText(iActivePlayer, g_iMinorCivID);
	local sToolTipText = GetActiveQuestToolTip(iActivePlayer, g_iMinorCivID);
	
	Controls.QuestInfo:SetText(sIconText);
	Controls.QuestInfo:SetToolTipString(sToolTipText);
	Controls.QuestLabel:SetToolTipString(sToolTipText);
end

-------------------------------------------------
-- On Quest Info Clicked
-------------------------------------------------
function OnQuestInfoClicked()
	local iActivePlayer = Game.GetActivePlayer();
	local pMinor = Players[g_iMinorCivID];
	if (pMinor) then
		if (pMinor:IsMinorCivDisplayedQuestForPlayer(iActivePlayer, MinorCivQuestTypes.MINOR_CIV_QUEST_KILL_CAMP)) then
			local iQuestData1 = pMinor:GetQuestData1(iActivePlayer, MinorCivQuestTypes.MINOR_CIV_QUEST_KILL_CAMP);
			local iQuestData2 = pMinor:GetQuestData2(iActivePlayer, MinorCivQuestTypes.MINOR_CIV_QUEST_KILL_CAMP);
			local pPlot = Map.GetPlot(iQuestData1, iQuestData2);
			if (pPlot) then
				UI.LookAt(pPlot, 0);
				local hex = ToHexFromGrid(Vector2(pPlot:GetX(), pPlot:GetY()));
				Events.GameplayFX(hex.x, hex.y, -1);
			end
		end
		if (pMinor:IsMinorCivDisplayedQuestForPlayer(activePlayerID, MinorCivQuestTypes.MINOR_CIV_QUEST_DISCOVER_PLOT)) then
			local iQuestData1 = pMinor:GetQuestData1(iActivePlayer, MinorCivQuestTypes.MINOR_CIV_QUEST_DISCOVER_PLOT);
			local iQuestData2 = pMinor:GetQuestData2(iActivePlayer, MinorCivQuestTypes.MINOR_CIV_QUEST_DISCOVER_PLOT);
			local pPlot = Map.GetPlot(iQuestData1, iQuestData2);
			if (pPlot) then
				UI.LookAt(pPlot, 0);
				local hex = ToHexFromGrid(Vector2(pPlot:GetX(), pPlot:GetY()));
				Events.GameplayFX(hex.x, hex.y, -1);
			end
		end
		if (minorPlayer:IsMinorCivDisplayedQuestForPlayer(activePlayerID, MinorCivQuestTypes.MINOR_CIV_QUEST_UNIT_GET_CITY)) then
			local iQuestData1 = pMinor:GetQuestData1(iActivePlayer, MinorCivQuestTypes.MINOR_CIV_QUEST_UNIT_GET_CITY);
			local iQuestData2 = pMinor:GetQuestData2(iActivePlayer, MinorCivQuestTypes.MINOR_CIV_QUEST_UNIT_GET_CITY);
			local pPlot = Map.GetPlot(iQuestData1, iQuestData2);
			if (pPlot) then
				UI.LookAt(pPlot, 0);
				local hex = ToHexFromGrid(Vector2(pPlot:GetX(), pPlot:GetY()));
				Events.GameplayFX(hex.x, hex.y, -1);
			end
		end
	end
end
Controls.QuestInfo:RegisterCallback( Mouse.eLClick, OnQuestInfoClicked );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-- MAIN MENU
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

----------------------------------------------------------------
-- Pledge
----------------------------------------------------------------
function OnPledgeButtonClicked ()
	
	local iActivePlayer = Game.GetActivePlayer();
	local pPlayer = Players[g_iMinorCivID];
	
	if (pPlayer:CanMajorStartProtection(iActivePlayer)) then
		Game.DoMinorPledgeProtection(Game.GetActivePlayer(), g_iMinorCivID, true);
		m_iLastAction = kiPledgedToProtect;
	end
end
Controls.PledgeButton:RegisterCallback( Mouse.eLClick, OnPledgeButtonClicked );

----------------------------------------------------------------
-- Revoke Pledge
----------------------------------------------------------------
function OnRevokePledgeButtonClicked ()
	
	local iActivePlayer = Game.GetActivePlayer();
	local pPlayer = Players[g_iMinorCivID];
	
	if (pPlayer:CanMajorWithdrawProtection(iActivePlayer)) then
		Game.DoMinorPledgeProtection(iActivePlayer, g_iMinorCivID, false);
		m_iLastAction = kiRevokedProtection;
	end
end
Controls.RevokePledgeButton:RegisterCallback( Mouse.eLClick, OnRevokePledgeButtonClicked );

----------------------------------------------------------------
-- Buyout
----------------------------------------------------------------
function OnBuyoutButtonClicked()
	local iActivePlayer = Game.GetActivePlayer();
	local pMinor = Players[g_iMinorCivID];
	
	if (pMinor:CanMajorBuyout(iActivePlayer)) then
		UIManager:DequeuePopup( ContextPtr );
		Game.DoMinorBuyout(iActivePlayer, pMinor:GetID());
	end
end
Controls.BuyoutButton:RegisterCallback( Mouse.eLClick, OnBuyoutButtonClicked );
--CBP
----------------------------------------------------------------
-- Marriage
----------------------------------------------------------------
function OnMarriageButtonClicked()
	local iActivePlayer = Game.GetActivePlayer();
	local pMinor = Players[g_iMinorCivID];

	if (pMinor:CanMajorMarry(iActivePlayer)) then
		UIManager:DequeuePopup( ContextPtr );
		Game.DoMinorBuyout(iActivePlayer, pMinor:GetID());
	end
end
Controls.MarriageButton:RegisterCallback( Mouse.eLClick, OnMarriageButtonClicked );
--END

----------------------------------------------------------------
-- War
----------------------------------------------------------------
function OnWarButtonClicked ()
	UI.AddPopup{ Type = ButtonPopupTypes.BUTTONPOPUP_DECLAREWARMOVE, Data1 = g_iMinorCivTeamID, Option1 = true};
end
Controls.WarButton:RegisterCallback( Mouse.eLClick, OnWarButtonClicked );

----------------------------------------------------------------
-- Peace
----------------------------------------------------------------
function OnPeaceButtonClicked ()
    
	Network.SendChangeWar(g_iMinorCivTeamID, false);
	m_iLastAction = kiMadePeace;
end
Controls.PeaceButton:RegisterCallback( Mouse.eLClick, OnPeaceButtonClicked );

----------------------------------------------------------------
-- Stop/Start Unit Spawning
----------------------------------------------------------------
function OnStopStartSpawning()
    local pPlayer = Players[g_iMinorCivID];
    local iActivePlayer = Game.GetActivePlayer();
	
	local bSpawningDisabled = pPlayer:IsMinorCivUnitSpawningDisabled(iActivePlayer);
	
	-- Update the text based on what state we're changing to
	local strSpawnText;
	if (bSpawningDisabled) then
		strSpawnText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_TURN_SPAWNING_OFF");
	else
		strSpawnText = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_TURN_SPAWNING_ON");
	end
	
	Controls.NoUnitSpawningLabel:SetText(strSpawnText);
    
	Network.SendMinorNoUnitSpawning(g_iMinorCivID, not bSpawningDisabled);
end
Controls.NoUnitSpawningButton:RegisterCallback( Mouse.eLClick, OnStopStartSpawning );

----------------------------------------------------------------
-- Open Give Submenu
----------------------------------------------------------------
function OnGiveButtonClicked ()
	Controls.GiveStack:SetHide(false);
	Controls.TakeStack:SetHide(true);
	Controls.ButtonStack:SetHide(true);
	PopulateGiftChoices();
end
Controls.GiveButton:RegisterCallback( Mouse.eLClick, OnGiveButtonClicked );

----------------------------------------------------------------
-- Open Take Submenu
----------------------------------------------------------------
function OnTakeButtonClicked ()
	Controls.GiveStack:SetHide(true);
	Controls.TakeStack:SetHide(false);
	Controls.ButtonStack:SetHide(true);
	PopulateTakeChoices();
end
Controls.TakeButton:RegisterCallback( Mouse.eLClick, OnTakeButtonClicked );

----------------------------------------------------------------
-- Close
----------------------------------------------------------------
function OnCloseButtonClicked ()
	UIManager:DequeuePopup( ContextPtr );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnCloseButtonClicked );

----------------------------------------------------------------
-- Find On Map
----------------------------------------------------------------
function OnFindOnMapButtonClicked ()
	local pPlayer = Players[g_iMinorCivID];
	if (pPlayer) then
		local pCity = pPlayer:GetCapitalCity();
		if (pCity) then
			local pPlot = pCity:Plot();
			if (pPlot) then
				UI.LookAt(pPlot, 0);
			end
		end
	end
end
Controls.FindOnMapButton:RegisterCallback( Mouse.eLClick, OnFindOnMapButtonClicked );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-- GIFT MENU
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
local iGoldGiftLarge = GameDefines["MINOR_GOLD_GIFT_LARGE"];
local iGoldGiftMedium = GameDefines["MINOR_GOLD_GIFT_MEDIUM"];
local iGoldGiftSmall = GameDefines["MINOR_GOLD_GIFT_SMALL"];

function PopulateGiftChoices()	
	local pPlayer = Players[g_iMinorCivID];
	
	local iActivePlayer = Game.GetActivePlayer();
	local pActivePlayer = Players[iActivePlayer];
	
	-- Small Gold
	local iNumGoldPlayerHas = pActivePlayer:GetGold();
	
	iGold = iGoldGiftSmall;
	iLowestGold = iGold;
	iFriendshipAmount = pPlayer:GetFriendshipFromGoldGift(iActivePlayer, iGold);
	local buttonText = Locale.ConvertTextKey("TXT_KEY_POPUP_MINOR_GOLD_GIFT_AMOUNT", iGold, iFriendshipAmount);
	if (iNumGoldPlayerHas < iGold) then
		buttonText = "[COLOR_WARNING_TEXT]" .. buttonText .. "[ENDCOLOR]";
		Controls.SmallGiftAnim:SetHide(true);
	else
		Controls.SmallGiftAnim:SetHide(false);
	end
	Controls.SmallGift:SetText(buttonText);
	SetButtonSize(Controls.SmallGift, Controls.SmallGiftButton, Controls.SmallGiftAnim, Controls.SmallGiftButtonHL);
	
	-- Medium Gold
	iGold = iGoldGiftMedium;
	iFriendshipAmount = pPlayer:GetFriendshipFromGoldGift(iActivePlayer, iGold);
	local buttonText = Locale.ConvertTextKey("TXT_KEY_POPUP_MINOR_GOLD_GIFT_AMOUNT", iGold, iFriendshipAmount);
	if (iNumGoldPlayerHas < iGold) then
		buttonText = "[COLOR_WARNING_TEXT]" .. buttonText .. "[ENDCOLOR]";
		Controls.MediumGiftAnim:SetHide(true);
	else
		Controls.MediumGiftAnim:SetHide(false);
	end
	Controls.MediumGift:SetText(buttonText);
	SetButtonSize(Controls.MediumGift, Controls.MediumGiftButton, Controls.MediumGiftAnim, Controls.MediumGiftButtonHL);
	
	-- Large Gold
	iGold = iGoldGiftLarge;
	iFriendshipAmount = pPlayer:GetFriendshipFromGoldGift(iActivePlayer, iGold);
	local buttonText = Locale.ConvertTextKey("TXT_KEY_POPUP_MINOR_GOLD_GIFT_AMOUNT", iGold, iFriendshipAmount);
	if (iNumGoldPlayerHas < iGold) then
		buttonText = "[COLOR_WARNING_TEXT]" .. buttonText .. "[ENDCOLOR]";
		Controls.LargeGiftAnim:SetHide(true);
	else
		Controls.LargeGiftAnim:SetHide(false);
	end
	Controls.LargeGift:SetText(buttonText);
	SetButtonSize(Controls.LargeGift, Controls.LargeGiftButton, Controls.LargeGiftAnim, Controls.LargeGiftButtonHL);
	
	-- Unit
	local iInfluence = pPlayer:GetFriendshipFromUnitGift(iActivePlayer, false, true);
	local iTravelTurns = GameDefines.MINOR_UNIT_GIFT_TRAVEL_TURNS;
	local buttonText = Locale.ConvertTextKey("TXT_KEY_POP_CSTATE_GIFT_UNIT", iInfluence);
	local tooltipText = Locale.ConvertTextKey("TXT_KEY_POP_CSTATE_GIFT_UNIT_TT", iTravelTurns, iInfluence);
	if (pPlayer:GetIncomingUnitCountdown(iActivePlayer) >= 0) then
		buttonText = "[COLOR_WARNING_TEXT]" .. buttonText .. "[ENDCOLOR]";
		Controls.UnitGiftAnim:SetHide(true);
		Controls.UnitGiftButton:ClearCallback( Mouse.eLClick );
	else
		Controls.UnitGiftAnim:SetHide(false);
		Controls.UnitGiftButton:RegisterCallback( Mouse.eLClick, OnGiftUnit );
	end
	Controls.UnitGift:SetText(buttonText);
	Controls.UnitGiftButton:SetToolTipString(tooltipText);
	SetButtonSize(Controls.UnitGift, Controls.UnitGiftButton, Controls.UnitGiftAnim, Controls.UnitGiftButtonHL);
	
	-- Tile Improvement
	-- Only allowed for allies
	iGold = pPlayer:GetGiftTileImprovementCost(iActivePlayer);
	local buttonText = Locale.ConvertTextKey("TXT_KEY_POPUP_MINOR_GIFT_TILE_IMPROVEMENT", iGold);
	if (not pPlayer:CanMajorGiftTileImprovement(iActivePlayer)) then
		buttonText = "[COLOR_WARNING_TEXT]" .. buttonText .. "[ENDCOLOR]";
		Controls.TileImprovementGiftAnim:SetHide(true);
	else
		Controls.TileImprovementGiftAnim:SetHide(false);
	end
	Controls.TileImprovementGift:SetText(buttonText);
	SetButtonSize(Controls.TileImprovementGift, Controls.TileImprovementGiftButton, Controls.TileImprovementGiftAnim, Controls.TileImprovementGiftButtonHL);
	
	-- Tooltip info
	local iFriendsAmount = GameDefines["FRIENDSHIP_THRESHOLD_FRIENDS"];
	local iAlliesAmount = GameDefines["FRIENDSHIP_THRESHOLD_ALLIES"];
    local iFriendship = pPlayer:GetMinorCivFriendshipWithMajor(iActivePlayer);
	local strInfoTT = Locale.ConvertTextKey("TXT_KEY_POP_CSTATE_GOLD_STATUS_TT", iFriendsAmount, iAlliesAmount, iFriendship);
	strInfoTT = strInfoTT .. "[NEWLINE][NEWLINE]";
	strInfoTT = strInfoTT .. Locale.ConvertTextKey("TXT_KEY_POP_CSTATE_GOLD_TT");
	Controls.SmallGiftButton:SetToolTipString(strInfoTT);
	Controls.MediumGiftButton:SetToolTipString(strInfoTT);
	Controls.LargeGiftButton:SetToolTipString(strInfoTT);
	
	UpdateButtonStack();
end

----------------------------------------------------------------
-- Gold Gifts
----------------------------------------------------------------
function OnSmallGold ()
	local iActivePlayer = Game.GetActivePlayer();
	local pActivePlayer = Players[iActivePlayer];
	local iNumGoldPlayerHas = pActivePlayer:GetGold();
	
	if (iNumGoldPlayerHas >= iGoldGiftSmall) then
		Game.DoMinorGoldGift(g_iMinorCivID, iGoldGiftSmall);
		m_iLastAction = kiGiftedGold;
		OnCloseGive();
	end
end
Controls.SmallGiftButton:RegisterCallback( Mouse.eLClick, OnSmallGold );

function OnMediumGold ()
	local iActivePlayer = Game.GetActivePlayer();
	local pActivePlayer = Players[iActivePlayer];
	local iNumGoldPlayerHas = pActivePlayer:GetGold();
	
	if (iNumGoldPlayerHas >= iGoldGiftMedium) then
		Game.DoMinorGoldGift(g_iMinorCivID, iGoldGiftMedium);
		m_iLastAction = kiGiftedGold;
		OnCloseGive();
	end
end
Controls.MediumGiftButton:RegisterCallback( Mouse.eLClick, OnMediumGold );

function OnBigGold ()
	local iActivePlayer = Game.GetActivePlayer();
	local pActivePlayer = Players[iActivePlayer];
	local iNumGoldPlayerHas = pActivePlayer:GetGold();
	
	if (iNumGoldPlayerHas >= iGoldGiftLarge) then
		Game.DoMinorGoldGift(g_iMinorCivID, iGoldGiftLarge);
		m_iLastAction = kiGiftedGold;
		OnCloseGive();
	end
end
Controls.LargeGiftButton:RegisterCallback( Mouse.eLClick, OnBigGold );

----------------------------------------------------------------
-- Gift Unit
----------------------------------------------------------------
function OnGiftUnit()
    UIManager:DequeuePopup( ContextPtr );

	local interfaceModeSelection = InterfaceModeTypes.INTERFACEMODE_GIFT_UNIT;
	
	UI.SetInterfaceMode(interfaceModeSelection);
	UI.SetInterfaceModeValue(g_iMinorCivID);
end
Controls.UnitGiftButton:RegisterCallback( Mouse.eLClick, OnGiftUnit );

----------------------------------------------------------------
-- Gift Improvement
----------------------------------------------------------------
function OnGiftTileImprovement()
	local pMinor = Players[g_iMinorCivID];
	local iActivePlayer = Game.GetActivePlayer();
    
    if (pMinor:CanMajorGiftTileImprovement(iActivePlayer)) then
		UIManager:DequeuePopup( ContextPtr );

		local interfaceModeSelection = InterfaceModeTypes.INTERFACEMODE_GIFT_TILE_IMPROVEMENT;
		
		UI.SetInterfaceMode(interfaceModeSelection);
		UI.SetInterfaceModeValue(g_iMinorCivID);
	end
end
Controls.TileImprovementGiftButton:RegisterCallback( Mouse.eLClick, OnGiftTileImprovement );

----------------------------------------------------------------
-- Close Give Submenu
----------------------------------------------------------------
function OnCloseGive()
	Controls.GiveStack:SetHide(true);
	Controls.TakeStack:SetHide(true);
	Controls.ButtonStack:SetHide(false);
	UpdateButtonStack();
end
Controls.ExitGiveButton:RegisterCallback( Mouse.eLClick, OnCloseGive );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-- TAKE MENU
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
local iBullyGoldInfluenceLost = (GameDefines["MINOR_FRIENDSHIP_DROP_BULLY_GOLD_SUCCESS"] / 100) * -1; -- Since XML value is times 100 for fidelity, and negative
local iBullyUnitInfluenceLost = (GameDefines["MINOR_FRIENDSHIP_DROP_BULLY_WORKER_SUCCESS"] / 100) * -1; -- Since XML value is times 100 for fidelity, and negative
local iBullyUnitMinimumPop = 4; --antonjs: todo: XML

function PopulateTakeChoices()	
	local pPlayer = Players[g_iMinorCivID];
	local iActivePlayer = Game.GetActivePlayer();
	local buttonText = "";
	local ttText = "";
	
	local iBullyGold = pPlayer:GetMinorCivBullyGoldAmount(iActivePlayer);
	buttonText = Locale.Lookup("TXT_KEY_POPUP_MINOR_BULLY_GOLD_AMOUNT", iBullyGold, iBullyGoldInfluenceLost);
	ttText = pPlayer:GetMajorBullyGoldDetails(iActivePlayer);
	if (not pPlayer:CanMajorBullyGold(iActivePlayer)) then
		buttonText = "[COLOR_WARNING_TEXT]" .. buttonText .. "[ENDCOLOR]";
		Controls.GoldTributeAnim:SetHide(true);
	else
		Controls.GoldTributeAnim:SetHide(false);
	end
	Controls.GoldTributeLabel:SetText(buttonText);
	Controls.GoldTributeButton:SetToolTipString(ttText);
	SetButtonSize(Controls.GoldTributeLabel, Controls.GoldTributeButton, Controls.GoldTributeAnim, Controls.GoldTributeButtonHL);
-- CBP
	local iTheftValue = 0;
	local pMajor = Players[iActivePlayer];
	local sBullyUnit = GameInfo.Units[pPlayer:GetBullyUnit()].Description; --antonjs: todo: XML or fn
	if(pMajor:IsBullyAnnex()) then
		buttonText = Locale.Lookup("TXT_KEY_POPUP_MINOR_BULLY_UNIT_AMOUNT_ANNEX");
	else
		if(pPlayer:GetMinorCivTrait() == MinorCivTraitTypes.MINOR_CIV_TRAIT_MARITIME) then
			iTheftValue = pPlayer:GetYieldTheftAmount(iActivePlayer, YieldTypes.YIELD_FOOD);
			buttonText = Locale.Lookup("TXT_KEY_POPUP_MINOR_BULLY_FOOD_AMOUNT", iTheftValue, iBullyUnitInfluenceLost);
		elseif(pPlayer:GetMinorCivTrait() == MinorCivTraitTypes.MINOR_CIV_TRAIT_CULTURED) then
			iTheftValue = pPlayer:GetYieldTheftAmount(iActivePlayer, YieldTypes.YIELD_CULTURE);
			buttonText = Locale.Lookup("TXT_KEY_POPUP_MINOR_BULLY_CULTURE_AMOUNT", iTheftValue, iBullyUnitInfluenceLost);
		elseif(pPlayer:GetMinorCivTrait() == MinorCivTraitTypes.MINOR_CIV_TRAIT_MILITARISTIC) then
			iTheftValue = pPlayer:GetYieldTheftAmount(iActivePlayer, YieldTypes.YIELD_SCIENCE);
			buttonText = Locale.Lookup("TXT_KEY_POPUP_MINOR_BULLY_SCIENCE_AMOUNT", iTheftValue, iBullyUnitInfluenceLost);
		elseif(pPlayer:GetMinorCivTrait() == MinorCivTraitTypes.MINOR_CIV_TRAIT_MERCANTILE) then
			iTheftValue = pPlayer:GetYieldTheftAmount(iActivePlayer, YieldTypes.YIELD_PRODUCTION);
			buttonText = Locale.Lookup("TXT_KEY_POPUP_MINOR_BULLY_PRODUCTION_AMOUNT", iTheftValue, iBullyUnitInfluenceLost);
		elseif(pPlayer:GetMinorCivTrait() == MinorCivTraitTypes.MINOR_CIV_TRAIT_RELIGIOUS) then
			iTheftValue = pPlayer:GetYieldTheftAmount(iActivePlayer, YieldTypes.YIELD_FAITH);
			buttonText = Locale.Lookup("TXT_KEY_POPUP_MINOR_BULLY_FAITH_AMOUNT", iTheftValue, iBullyUnitInfluenceLost);
		else
			buttonText = Locale.Lookup("TXT_KEY_POPUP_MINOR_BULLY_UNIT_AMOUNT", sBullyUnit, iBullyUnitInfluenceLost);
		end	
	end
-- END
	ttText = pPlayer:GetMajorBullyUnitDetails(iActivePlayer);
	if (not pPlayer:CanMajorBullyUnit(iActivePlayer)) then
		buttonText = "[COLOR_WARNING_TEXT]" .. buttonText .. "[ENDCOLOR]";
		Controls.UnitTributeAnim:SetHide(true);
	else
		Controls.UnitTributeAnim:SetHide(false);
	end
	Controls.UnitTributeLabel:SetText(buttonText);
	Controls.UnitTributeButton:SetToolTipString(ttText);
	SetButtonSize(Controls.UnitTributeLabel, Controls.UnitTributeButton, Controls.UnitTributeAnim, Controls.UnitTributeButtonHL);
	
	UpdateButtonStack();
end

----------------------------------------------------------------
-- Bully confirmation
----------------------------------------------------------------
function OnBullyButtonClicked ()

	local listofProtectingCivs = {};
	for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
			
		pOtherPlayer = Players[iPlayerLoop];
			
		-- Don't test protection status with active player!
		if (iPlayerLoop ~= Game.GetActivePlayer()) then
			if (pOtherPlayer:IsAlive()) then
				if(Teams[Game.GetActiveTeam()]:IsHasMet(pOtherPlayer:GetTeam())) then
					if (pOtherPlayer:IsProtectingMinor(g_iMinorCivID)) then
						table.insert(listofProtectingCivs, Players[iPlayerLoop]:GetCivilizationShortDescriptionKey()); 
					end
				end
			end
		end
	end
	
	local pMinor = Players[g_iMinorCivID];
	local cityStateName = Locale.Lookup(pMinor:GetCivilizationShortDescriptionKey());
	
	local bullyConfirmString = Locale.ConvertTextKey("TXT_KEY_CONFIRM_BULLY", cityStateName);
	local numProtectingCivs = #listofProtectingCivs;
	if(numProtectingCivs > 0) then
		if(numProtectingCivs == 1) then
			bullyConfirmString = Locale.ConvertTextKey("TXT_KEY_CONFIRM_BULLY_PROTECTED_CITY_STATE", cityStateName, listofProtectingCivs[1]);
		else
			local translatedCivs = {};
			for i,v in ipairs(listofProtectingCivs) do
				translatedCivs[i] = Locale.Lookup(v);
			end
		
			bullyConfirmString = Locale.ConvertTextKey("TXT_KEY_CONFIRM_BULLY_PROTECTED_CITY_STATE_MULTIPLE", cityStateName, table.concat(translatedCivs, ", "));
		end
	end
	
	Controls.BullyConfirmLabel:SetText( bullyConfirmString );
	Controls.BullyConfirm:SetHide(false);
	Controls.BGBlock:SetHide(true);
end

----------------------------------------------------------------
-- Take Gold
----------------------------------------------------------------
function OnGoldTributeButtonClicked()
	local pPlayer = Players[g_iMinorCivID];
	local iActivePlayer = Game.GetActivePlayer();
	
	if (pPlayer:CanMajorBullyGold(iActivePlayer)) then
		m_iPendingAction = kiBulliedGold;
		OnBullyButtonClicked();
		OnCloseTake();
	end
end
Controls.GoldTributeButton:RegisterCallback( Mouse.eLClick, OnGoldTributeButtonClicked );

----------------------------------------------------------------
-- Take Unit
----------------------------------------------------------------
function OnUnitTributeButtonClicked()
	local pPlayer = Players[g_iMinorCivID];
	local iActivePlayer = Game.GetActivePlayer();
	
	if (pPlayer:CanMajorBullyUnit(iActivePlayer)) then
		m_iPendingAction = kiBulliedUnit;
		OnBullyButtonClicked();
		OnCloseTake();
	end
end
Controls.UnitTributeButton:RegisterCallback( Mouse.eLClick, OnUnitTributeButtonClicked );

----------------------------------------------------------------
-- Close Take Submenu
----------------------------------------------------------------
function OnCloseTake()
	Controls.GiveStack:SetHide(true);
	Controls.TakeStack:SetHide(true);
	Controls.ButtonStack:SetHide(false);
	UpdateButtonStack();
end
Controls.ExitTakeButton:RegisterCallback( Mouse.eLClick, OnCloseTake );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-- BULLY CONFIRMATION POPUP
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnYesBully( )
	local iActivePlayer = Game.GetActivePlayer();
	if (m_iPendingAction == kiBulliedGold) then
		Game.DoMinorBullyGold(iActivePlayer, g_iMinorCivID);
		m_iPendingAction = kiNoAction;
		m_iLastAction = kiBulliedGold;
	elseif (m_iPendingAction == kiBulliedUnit) then
-- CBP
		OnCloseButtonClicked();
		m_iPendingAction = kiNoAction;
		m_iLastAction = kiBulliedUnit;
-- END
		Game.DoMinorBullyUnit(iActivePlayer, g_iMinorCivID);
	else
		print("Scripting error - Selected Yes for bully confrirmation dialog, but invalid PendingAction type");
	end

	Controls.BullyConfirm:SetHide(true);
	Controls.BGBlock:SetHide(false);
    UIManager:DequeuePopup( ContextPtr );
end
Controls.YesBully:RegisterCallback( Mouse.eLClick, OnYesBully );

function OnNoBully( )
	Controls.BullyConfirm:SetHide(true);
	Controls.BGBlock:SetHide(false);
end
Controls.NoBully:RegisterCallback( Mouse.eLClick, OnNoBully );

-------------------------------------------------
-- 'Active' (local human) player has changed
-------------------------------------------------
Events.GameplaySetActivePlayer.Add(OnCloseButtonClicked);