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
	end
		
	-- Info on their Trait
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
	local iGoldGift = popupInfo.Data2;
	local iFaithGift = popupInfo.Data3;
	local bFirstMajorCiv = popupInfo.Option1;
	local strGiftString = "";
	
	if (iGoldGift > 0) then
		if (bFirstMajorCiv) then
			strGiftString = strGiftString .. Locale.ConvertTextKey("TXT_KEY_CITY_STATE_GIFT_FIRST", iGoldGift);
		else
			strGiftString = strGiftString .. Locale.ConvertTextKey("TXT_KEY_CITY_STATE_GIFT_OTHER", iGoldGift);
		end
	end
	
	if (iFaithGift > 0) then
		if (iGoldGift > 0) then
			strGiftString = strGiftString .. " ";
		end
		
		if (bFirstMajorCiv) then
			strGiftString = strGiftString .. Locale.ConvertTextKey("TXT_KEY_CITY_STATE_GIFT_FAITH_FIRST", iFaithGift);
		else
			strGiftString = strGiftString .. Locale.ConvertTextKey("TXT_KEY_CITY_STATE_GIFT_FAITH_OTHER", iFaithGift);
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
