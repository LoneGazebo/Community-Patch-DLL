-------------------------------------------------
-- New Era Popup
-------------------------------------------------

local m_PopupInfo = nil;
local lastBackgroundImage = "ERA_Medievel.dds"

-------------------------------------------------
-- On Popup
-------------------------------------------------
function OnPopup( popupInfo )

	if( popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_NEW_ERA ) then
		return;
	end

	m_PopupInfo = popupInfo;

    local iEra = popupInfo.Data1;

	if(iEra == 100) then
		Controls.DescriptionLabel:LocalizeAndSetText("TXT_KEY_POP_WORLD_VICTORY_TYPE_DIPLO");
		lastBackgroundImage = "DiploVictory.dds";
		Controls.TitleLabel:LocalizeAndSetText("TXT_KEY_POP_WORLD_VICTORY_SPLASH");
	elseif(iEra == 99) then
		Controls.DescriptionLabel:LocalizeAndSetText("TXT_KEY_POP_WORLD_VICTORY_TYPE_CULTURE");
		lastBackgroundImage = "CultureVictory.dds";
		Controls.TitleLabel:LocalizeAndSetText("TXT_KEY_POP_WORLD_VICTORY_SPLASH");
	elseif(iEra == 98) then
		Controls.DescriptionLabel:LocalizeAndSetText("TXT_KEY_POP_WORLD_VICTORY_TYPE_SPACERACE");
		lastBackgroundImage = "SpaceRaceVictory.dds";
		Controls.TitleLabel:LocalizeAndSetText("TXT_KEY_POP_WORLD_VICTORY_SPLASH");
	elseif(iEra == 97) then
		Controls.DescriptionLabel:LocalizeAndSetText("TXT_KEY_POP_WORLD_VICTORY_TYPE_CONQUEST");
		lastBackgroundImage = "ConquestVictory.dds";
		Controls.TitleLabel:LocalizeAndSetText("TXT_KEY_POP_WORLD_VICTORY_SPLASH");
	else
		Controls.DescriptionLabel:LocalizeAndSetText("TXT_KEY_POP_NEW_ERA_DESCRIPTION", GameInfo.Eras[iEra].Description);
	
		local strEra = GameInfo.Eras[iEra].Type;
	
		-- Change Image for Era
		if (strEra == "ERA_MEDIEVAL") then
			lastBackgroundImage = "ERA_Medievel.dds";
		elseif (strEra == "ERA_CLASSICAL") then
			lastBackgroundImage = "ERA_Classical.dds";
		elseif (strEra == "ERA_RENAISSANCE") then
			lastBackgroundImage = "ERA_Renissance.dds";
		elseif (strEra == "ERA_INDUSTRIAL") then
			lastBackgroundImage = "ERA_Industrial.dds";
		elseif (strEra == "ERA_MODERN") then
			lastBackgroundImage = "ERA_Modern.dds";
		elseif (strEra == "ERA_POSTMODERN") then
			lastBackgroundImage = "ERA_Atomic.dds";
		elseif (strEra == "ERA_FUTURE") then
			lastBackgroundImage = "ERA_Future.dds";
		end
	end
	Controls.EraImage:SetTexture(lastBackgroundImage);
	
	UIManager:QueuePopup( ContextPtr, PopupPriority.NewEraPopup );
end
Events.SerialEventGameMessagePopup.Add( OnPopup );


----------------------------------------------------------------        
-- Input processing
----------------------------------------------------------------        

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnClose()
    UIManager:DequeuePopup( ContextPtr );
    Controls.EraImage:UnloadTexture();
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose);

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
            OnClose();
            return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )

    if( not bInitState ) then
       Controls.EraImage:UnloadTexture();
       if( not bIsHide ) then
        	Controls.EraImage:SetTexture(lastBackgroundImage);
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
        else
            UI.decTurnTimerSemaphore();
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_NEW_ERA, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(OnClose);
