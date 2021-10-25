-------------------------------------------------
-- Natural Wonder Popup
-------------------------------------------------
include( "IconSupport" );

local m_PopupInfo = nil;

-------------------------------------------------
-- On Display
-------------------------------------------------
function OnPopup( popupInfo )

	if( popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_NATURAL_WONDER_REWARD ) then
		return;
	end
	
	local x = popupInfo.Data1;
	local y = popupInfo.Data2;
	local iFinderGold = popupInfo.Data3;
	local bFirstFinder = popupInfo.Option1;
	
	SetIcon(x, y);
	
	m_PopupInfo = popupInfo;
	
	local pNaturalWonderPlot = Map.GetPlot(x, y);
	local feature = pNaturalWonderPlot:GetFeatureType();
	local info = GameInfo.Features[feature];
	local condition = "FeatureType = '" .. info.Type .. "'"
				
	local yieldString = Locale.ConvertTextKey("TXT_KEY_POP_NATURAL_WONDER_FOUND_TT", info.Description);
	local numYields = 0;
	for row in GameInfo.Feature_YieldChanges( condition ) do
	    if (row.Yield > 0) then
	        numYields = numYields + 1;
			yieldString = yieldString .. " " .. tostring(row.Yield) .. " ";
			yieldString = yieldString .. GameInfo.Yields[row.YieldType].IconString .. " ";
		end
	end	
	if (numYields == 0) then
		yieldString = yieldString .. " " .. Locale.ConvertTextKey( "TXT_KEY_PEDIA_NO_YIELD" );
	end

	numYields = 0;
	for row in GameInfo.Feature_EraYieldChanges( condition ) do
	    if (row.Yield > 0) then
			if(yieldString ~= "" and numYields == 0)then
				yieldString = yieldString .. "[NEWLINE]" .. Locale.ConvertTextKey( "TXT_KEY_PEDIA_ERA_YIELD" )
			end
	        numYields = numYields + 1;
			yieldString = yieldString .. " " .. tostring(row.Yield) .. " ";
			yieldString = yieldString .. GameInfo.Yields[row.YieldType].IconString .. " ";
		end
	end	
	
	if (info.InBorderHappiness > 0) then
	    yieldString = yieldString .. Locale.ConvertTextKey("TXT_KEY_POP_NATURAL_WONDER_FOUND_HAPPY", info.InBorderHappiness);
	end
	
	if (info.AdjacentUnitFreePromotion) then
		local thisPromotion = GameInfo.UnitPromotions[info.AdjacentUnitFreePromotion];
		yieldString = yieldString .. Locale.ConvertTextKey("TXT_KEY_POP_NATURAL_WONDER_FOUND_PROMOTE", Locale.ConvertTextKey(thisPromotion.Description));
	end

	if (info.FreePromotionIfOwned) then
		local thisPromotion = GameInfo.UnitPromotions[info.FreePromotionIfOwned];
		yieldString = yieldString .. Locale.ConvertTextKey("TXT_KEY_POP_NATURAL_WONDER_FOUND_PROMOTE_OWNED", Locale.ConvertTextKey(thisPromotion.Description));
	end
	
	if (iFinderGold > 0) then
		if (bFirstFinder) then
			yieldString = yieldString .. Locale.ConvertTextKey("TXT_KEY_POP_NATURAL_WONDER_FIRST_FOUND_GOLD", iFinderGold);
		else
			yieldString = yieldString .. Locale.ConvertTextKey("TXT_KEY_POP_NATURAL_WONDER_SUBSEQUENT_FOUND_GOLD", iFinderGold);
		end
	end
	
	Controls.DescriptionLabel:SetText(yieldString);	

	UIManager:QueuePopup( ContextPtr, PopupPriority.NaturalWonderPopup );
end
Events.SerialEventGameMessagePopup.Add( OnPopup );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function SetIcon(x, y)
	local pNaturalWonderPlot = Map.GetPlot(x, y);
	local feature = pNaturalWonderPlot:GetFeatureType();
	--print(feature);
	local info = GameInfo.Features[feature];
	if(info.NaturalWonder or info.PseudoNaturalWonder)then
		IconHookup(info.PortraitIndex, 128, info.IconAtlas, Controls.Icon);
		Controls.WonderLabel:LocalizeAndSetText(info.Description);
	end
end

----------------------------------------------------------------        
-- Input processing
----------------------------------------------------------------        

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnCloseButtonClicked ()
    UIManager:DequeuePopup( ContextPtr );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnCloseButtonClicked );

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
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
        else
            UI.decTurnTimerSemaphore();
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_NATURAL_WONDER_REWARD, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(OnClose);
