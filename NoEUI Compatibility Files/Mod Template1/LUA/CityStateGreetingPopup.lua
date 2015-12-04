print("This is the modded CityStateGreetingPopup from 'Global - CS Gifts'")

-------------------------------------------------
-- City State Greeting Popup
-------------------------------------------------
include( "IconSupport" );
include( "InfoTooltipInclude" );
include( "CityStateStatusHelper" );

local m_PopupInfo = nil;
local lastBackgroundImage = "citystatebackgroundculture.dds"

-------------------------------------------------
-------------------------------------------------
function OnPopup( popupInfo )
	
	local bGreeting = popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_CITY_STATE_GREETING;
	local bMessage = popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_CITY_STATE_MESSAGE;
	local bDiplo = popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_CITY_STATE_DIPLO;

	-- Current assignments of members of popupInfo
	--   Data1 is the player id
	--   Data2 is the gold gift value
	--   Data3 is the faith gift value
	--   Option1 is first met
	--   Option2 is nil
	--   Text is nil
	
	-- Updated assignments of members of popupInfo
    --   Data1 is the player id (unchanged)
	--   Data2 is the gift "value" (Gold/Culture/Faith amount, UnitId, CityId of population growth)
	--   Data3 is the friendship boost
	--   Option1 is first met (unchanged)
	--   Option2 is nil (unchanged)
	--   Text is suffix for the TXT_KEY_ to format with
	
	if (not bGreeting) then
		return;
	end
	
	--------------------------
	-- City State saying hi for the first time
	--------------------------
	
	m_PopupInfo = popupInfo;	
	
    local iPlayer = popupInfo.Data1;
    local pPlayer = Players[iPlayer];
	
	local strNameKey = pPlayer:GetCivilizationShortDescriptionKey();

	local strTitle = "";
	local strDescription = "";
	
	-- Set Title Icon
	local sMinorCivType = pPlayer:GetMinorCivType();
	local trait = GameInfo.MinorCivilizations[sMinorCivType].MinorCivTrait;
	Controls.TitleIcon:SetTexture(GameInfo.MinorCivTraits[trait].TraitTitleIcon);
	
	-- Set Background Image
	lastBackgroundImage = GameInfo.MinorCivTraits[trait].BackgroundImage;
	Controls.BackgroundImage:SetTexture(lastBackgroundImage);
	
	-- Update colors
	local primaryColor, secondaryColor = pPlayer:GetPlayerColors();
	primaryColor, secondaryColor = secondaryColor, primaryColor;
	local textColor = {x = primaryColor.x, y = primaryColor.y, z = primaryColor.z, w = 1};
	
	civType = pPlayer:GetCivilizationType();
	civInfo = GameInfo.Civilizations[civType];
	
	local iconColor = textColor;
	IconHookup( civInfo.PortraitIndex, 32, civInfo.AlphaIconAtlas, Controls.CivIcon );
	Controls.CivIcon:SetColor(iconColor);
	
	local strShortDescKey = pPlayer:GetCivilizationShortDescriptionKey();
	
	-- Title
	strTitle = Locale.ConvertTextKey("{" .. strShortDescKey.. ":upper}");

	local iActivePlayer = Game.GetActivePlayer();
	
	if (bMessage) then
		
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
		
		Controls.CityStateMeterThingy:SetHide(false);
		UpdateActiveQuests();
		Controls.QuestLabel:SetHide(false);
	
	-- Greeting popup - don't show status or quests here
	else
		Controls.CityStateMeterThingy:SetHide(true);
		Controls.QuestLabel:SetHide(true);
		
		-- Find the City State's ally or best friend
		local iMajorBestFriend = pPlayer:GetAlly()
		local iMajorFriendship = GameDefines.FRIENDSHIP_THRESHOLD_FRIENDS - 1

		if (iMajorBestFriend == -1) then
print(string.format("City State: Looking for a friend"))
			for iMajor = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
				local iFriendship = pPlayer:GetMinorCivFriendshipWithMajor(iMajor)
				if (iFriendship > iMajorFriendship) then
					iMajorBestFriend = iMajor
					iMajorFriendship = iFriendship
				end
			end
		else
print(string.format("City State: Has an ally"))
		end

		if (iMajorBestFriend ~= -1 and iMajorBestFriend ~= iActivePlayer) then
print(string.format("City State: Best friend is %i", iMajorBestFriend))
			if (not Teams[Game.GetActiveTeam()]:IsHasMet(Players[iMajorBestFriend]:GetTeam())) then
print(string.format("City State: Need to add a message and a quest"))
				-- Can we hijack the quest label to add the "Have you also met ..." text?
				-- AddMinorCivQuestIfAble(iActivePlayer, MinorCivQuestTypes.MINOR_CIV_QUEST_FIND_PLAYER)
			end
		end
	end
		
	-- Info on their Trait
	local strTraitText = GetCityStateTraitText(iPlayer);
	local strTraitTT = GetCityStateTraitToolTip(iPlayer);
	
	strTraitText = "[COLOR_POSITIVE_TEXT]" .. strTraitText .. "[ENDCOLOR]";
	
	Controls.TraitInfo:SetText(strTraitText);
	Controls.TraitInfo:SetToolTipString(strTraitTT);
	Controls.TraitLabel:SetToolTipString(strTraitTT);
	
	-- Personality
	local strPersonalityKey = "";
	local strPersonalityText = "";
	local strPersonalityTT = "";
	local iPersonality = pPlayer:GetPersonality();
	if (iPersonality == MinorCivPersonalityTypes.MINOR_CIV_PERSONALITY_FRIENDLY) then
		strPersonalityKey = "TXT_KEY_CITY_STATE_PERSONALITY_FRIENDLY"
	elseif (iPersonality == MinorCivPersonalityTypes.MINOR_CIV_PERSONALITY_NEUTRAL) then
		strPersonalityKey = "TXT_KEY_CITY_STATE_PERSONALITY_NEUTRAL"
	elseif (iPersonality == MinorCivPersonalityTypes.MINOR_CIV_PERSONALITY_HOSTILE) then
		strPersonalityKey = "TXT_KEY_CITY_STATE_PERSONALITY_HOSTILE"
	elseif (iPersonality == MinorCivPersonalityTypes.MINOR_CIV_PERSONALITY_IRRATIONAL) then
		strPersonalityKey = "TXT_KEY_CITY_STATE_PERSONALITY_IRRATIONAL"
	end
	
	strPersonalityText = "[COLOR_POSITIVE_TEXT]" .. Locale.ConvertTextKey(strPersonalityKey) .. "[ENDCOLOR]";

	strPersonalityTT = Locale.ConvertTextKey(strPersonalityKey .. "_TT");
	
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

	-- Gifts
	-- Updated assignments of members of popupInfo
    --   Data1 is the player id (unchanged)
	--   Data2 is the gift "value" (Gold/Culture/Faith amount, UnitId, CityId of population growth)
	--   Data3 is the friendship boost
	--   Option1 is first met (unchanged)
	--   Option2 is nil (unchanged)
	--   Text is suffix for the TXT_KEY_ to format with

--	local iGoldGift = popupInfo.Data2;
--	local iFaithGift = popupInfo.Data3;
	local bFirstMajorCiv = popupInfo.Option1;
	local strGiftString = "";
	
--	if (iGoldGift > 0) then
--		if (bFirstMajorCiv) then
--			strGiftString = strGiftString .. Locale.ConvertTextKey("TXT_KEY_CITY_STATE_GIFT_FIRST", iGoldGift);
--		else
--			strGiftString = strGiftString .. Locale.ConvertTextKey("TXT_KEY_CITY_STATE_GIFT_OTHER", iGoldGift);
--		end
--	end
--	
--	if (iFaithGift > 0) then
--		if (iGoldGift > 0) then
--			strGiftString = strGiftString .. " ";
--		end
--		
--		if (bFirstMajorCiv) then
--			strGiftString = strGiftString .. Locale.ConvertTextKey("TXT_KEY_CITY_STATE_GIFT_FAITH_FIRST", iFaithGift);
--		else
--			strGiftString = strGiftString .. Locale.ConvertTextKey("TXT_KEY_CITY_STATE_GIFT_FAITH_OTHER", iFaithGift);
--		end
--	end

	local strGiftTxtKey = string.format("TXT_KEY_MINOR_CIV_%sCONTACT_BONUS_%s", (bFirstMajorCiv and "FIRST_" or ""), popupInfo.Text)
	
	if (popupInfo.Data2 == 0) then
          if (popupInfo.Data3 == 0) then
            strGiftString = Locale.ConvertTextKey("TXT_KEY_MINOR_CIV_CONTACT_BONUS_NOTHING")
          else
            strGiftString = Locale.ConvertTextKey("TXT_KEY_MINOR_CIV_CONTACT_BONUS_FRIENDSHIP", popupInfo.Data3, strPersonalityKey)
          end
	else
	  if (popupInfo.Text == "UNIT") then
	    strGiftString = Locale.ConvertTextKey(strGiftTxtKey, GameInfo.Units[popupInfo.Data2].Description, strPersonalityKey)
	  else
	    strGiftString = Locale.ConvertTextKey(strGiftTxtKey, popupInfo.Data2, strPersonalityKey)
	  end
	end

	local strSpeakAgainString = Locale.ConvertTextKey("TXT_KEY_MINOR_SPEAK_AGAIN", strNameKey);
	
	strDescription = Locale.ConvertTextKey("TXT_KEY_CITY_STATE_MEETING", strNameKey, strGiftString, strSpeakAgainString);
	
	Controls.TitleLabel:SetText(strTitle);
	Controls.TitleLabel:SetColor(textColor, 0);
	Controls.DescriptionLabel:SetText(strDescription);
	
	UIManager:QueuePopup( ContextPtr, PopupPriority.CityStateGreeting );
end
Events.SerialEventGameMessagePopup.Add( OnPopup );

----------------------------------------------------------------
----------------------------------------------------------------
function getProtectingPlayers(iMinorCivID)
	local sProtecting = "";
	
	for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
		pOtherPlayer = Players[iPlayerLoop];

		if (iPlayerLoop ~= Game.GetActivePlayer()) then
			if (pOtherPlayer:IsAlive()) then
				if (pOtherPlayer:IsProtectingMinor(iMinorCivID)) then
					if (sProtecting ~= "") then
						sProtecting = sProtecting .. ", "
					end

					sProtecting = sProtecting .. Locale.ConvertTextKey(Players[iPlayerLoop]:GetCivilizationShortDescriptionKey());
				end
			end
		end
	end

	return sProtecting
end

function UpdateActiveQuests()
	local iActivePlayer = Game.GetActivePlayer();
    local iPlayer = m_PopupInfo.Data1;
	local sIconText = GetActiveQuestText(iActivePlayer, iPlayer);
	local sToolTipText = GetActiveQuestToolTip(iActivePlayer, iPlayer);
	
	Controls.QuestInfo:SetText(sIconText);
	Controls.QuestInfo:SetToolTipString(sToolTipText);
	Controls.QuestLabel:SetToolTipString(sToolTipText);
end

----------------------------------------------------------------        
-- Input processing
----------------------------------------------------------------        
function OnCloseButtonClicked ()
    UIManager:DequeuePopup( ContextPtr );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnCloseButtonClicked );
Controls.ScreenButton:RegisterCallback( Mouse.eRClick, OnCloseButtonClicked );

----------------------------------------------------------------
-- Find On Map
----------------------------------------------------------------
function OnFindOnMapButtonClicked ()
	local iPlayer = m_PopupInfo.Data1;
	local pPlayer = Players[iPlayer];
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
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
            OnCloseButtonClicked();
            return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )
    if( not bInitState ) then
		Controls.BackgroundImage:UnloadTexture();
        if( not bIsHide ) then
			Controls.BackgroundImage:SetTexture(lastBackgroundImage);
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
        else
            UI.decTurnTimerSemaphore();
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(m_PopupInfo.Type, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(OnCloseButtonClicked);
