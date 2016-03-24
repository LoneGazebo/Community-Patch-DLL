-------------------------------------------------
-- Economic
-------------------------------------------------

include( "IconSupport" );

local m_CurrentPanel = Controls.EconomicGeneralInfo;

-------------------------------------------------
-- On Popup
-------------------------------------------------
function OnPopup( popupInfo )
	if( popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_ECONOMIC_OVERVIEW ) then
		m_PopupInfo = popupInfo;
		
		if( m_PopupInfo.Data1 == 1 ) then
        	if( ContextPtr:IsHidden() == false ) then
        	    OnClose();
        	    return;
    	    else
            	UIManager:QueuePopup( ContextPtr, PopupPriority.InGameUtmost );
        	end
    	else
        	UIManager:QueuePopup( ContextPtr, PopupPriority.EconOverview );
    	end
	end
end
Events.SerialEventGameMessagePopup.Add( OnPopup );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnClose()
    UIManager:DequeuePopup( ContextPtr );
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
    local pPlayer = Players[ Game.GetActivePlayer() ];
    if( pPlayer == nil ) then
        print( "Could not get player... huh?" );
        return;
    end
    
	-- Set Civ Icon
	CivIconHookup( Game.GetActivePlayer(), 64, Controls.CivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true );
	
    if( not bInitState ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        	
        	-- trigger the show/hide handler to update state
        	m_CurrentPanel:SetHide( false );
        	
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
        else
            UI.decTurnTimerSemaphore();
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_ECONOMIC_OVERVIEW, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnGeneralInfoButton()
	-- Set Tabs
	Controls.GeneralInfoSelectHighlight:SetHide(false);
	Controls.HappinessSelectHighlight:SetHide(true);
	--CBP--
	Controls.MonopoliesSelectHighlight:SetHide(true);

	-- Set Panels
    Controls.EconomicGeneralInfo:SetHide( false );
    Controls.HappinessInfo:SetHide( true );
	--CBP--
	Controls.MonopoliesInfo:SetHide(true);

    m_CurrentPanel = Controls.EconomicGeneralInfo;
end
Controls.GeneralInfoButton:RegisterCallback( Mouse.eLClick, OnGeneralInfoButton );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnHappinessButton()
	-- Set Tabs
	Controls.GeneralInfoSelectHighlight:SetHide(true);
	Controls.HappinessSelectHighlight:SetHide(false);
	--CBP--
	Controls.MonopoliesSelectHighlight:SetHide(true);
	
	-- Set Panels
    Controls.EconomicGeneralInfo:SetHide( true );
    Controls.HappinessInfo:SetHide( false );
	--CBP--
	Controls.MonopoliesInfo:SetHide(true);

    m_CurrentPanel = Controls.HappinessInfo;
end
Controls.HappinessButton:RegisterCallback( Mouse.eLClick, OnHappinessButton );

-- CBP
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnMonopoliesButton()
	-- Set Tabs
	Controls.GeneralInfoSelectHighlight:SetHide(true);
	Controls.HappinessSelectHighlight:SetHide(true);
	--CBP--
	Controls.MonopoliesSelectHighlight:SetHide(false);
	
	-- Set Panels
    Controls.EconomicGeneralInfo:SetHide( true );
    Controls.HappinessInfo:SetHide( true );
	--CBP--
	Controls.MonopoliesInfo:SetHide(false);

    m_CurrentPanel = Controls.MonopoliesInfo;
end
Controls.MonopoliesButton:RegisterCallback( Mouse.eLClick, OnMonopoliesButton );


OnGeneralInfoButton();

-- Disable Happiness section if Happiness is turned off.
if (Game.IsOption(GameOptionTypes.GAMEOPTION_NO_HAPPINESS)) then
	Controls.HappinessButton:SetDisabled(true);
	Controls.HappinessButton:LocalizeAndSetToolTip("TXT_KEY_TOP_PANEL_HAPPINESS_OFF_TOOLTIP");
end

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(OnClose);
