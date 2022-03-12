-------------------------------------------------
-- Events
-------------------------------------------------

include( "IconSupport" );

local m_CurrentPanel = Controls.PlayerEventsInfo;

-------------------------------------------------
-- On Popup
-------------------------------------------------
function OnPopup( popupInfo )
	if( popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_MODDER_6 ) then
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
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_MODDER_6, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnPlayerEventsInfoButton()
	-- Set Tabs
	Controls.PlayerEventsInfoHighlight:SetHide(false);
	Controls.CityEventsInfoHighlight:SetHide(true);
	Controls.RecentEventsInfoHighlight:SetHide(true);
	Controls.CityRecentEventsInfoHighlight:SetHide(true);

	-- Set Panels
    Controls.PlayerEventsInfo:SetHide( false );
    Controls.CityEventsInfo:SetHide( true );
	Controls.RecentEventsInfo:SetHide(true);
	Controls.CityRecentEventsInfo:SetHide(true);

    m_CurrentPanel = Controls.PlayerEventsInfo;
end
Controls.PlayerEvents:RegisterCallback( Mouse.eLClick, OnPlayerEventsInfoButton );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnCityEventsInfoButton()
	-- Set Tabs
	Controls.PlayerEventsInfoHighlight:SetHide(true);
	Controls.CityEventsInfoHighlight:SetHide(false);
	Controls.RecentEventsInfoHighlight:SetHide(true);
	Controls.CityRecentEventsInfoHighlight:SetHide(true);

	-- Set Panels
    Controls.PlayerEventsInfo:SetHide( true );
    Controls.CityEventsInfo:SetHide( false );
	Controls.RecentEventsInfo:SetHide(true);
	Controls.CityRecentEventsInfo:SetHide(true);


    m_CurrentPanel = Controls.CityEventsInfo;
end
Controls.CityEvents:RegisterCallback( Mouse.eLClick, OnCityEventsInfoButton );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnRecentEventsInfoButton()
	-- Set Tabs
	Controls.PlayerEventsInfoHighlight:SetHide(true);
	Controls.CityEventsInfoHighlight:SetHide(true);
	Controls.RecentEventsInfoHighlight:SetHide(false);
	Controls.CityRecentEventsInfoHighlight:SetHide(true);

	-- Set Panels
    Controls.PlayerEventsInfo:SetHide( true );
    Controls.CityEventsInfo:SetHide( true );
	Controls.RecentEventsInfo:SetHide(false);
	Controls.CityRecentEventsInfo:SetHide(true);

    m_CurrentPanel = Controls.RecentEventsInfo;
end
Controls.RecentEvents:RegisterCallback( Mouse.eLClick, OnRecentEventsInfoButton );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnCityRecentEventsInfoButton()
	-- Set Tabs
	Controls.PlayerEventsInfoHighlight:SetHide(true);
	Controls.CityEventsInfoHighlight:SetHide(true);
	Controls.RecentEventsInfoHighlight:SetHide(true);
	Controls.CityRecentEventsInfoHighlight:SetHide(false);

	-- Set Panels
    Controls.PlayerEventsInfo:SetHide( true );
    Controls.CityEventsInfo:SetHide( true );
	Controls.RecentEventsInfo:SetHide(true);
	Controls.CityRecentEventsInfo:SetHide(false);

    m_CurrentPanel = Controls.CityRecentEventsInfo;
end
Controls.CityRecentEvents:RegisterCallback( Mouse.eLClick, OnCityRecentEventsInfoButton );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnActivePlayerChanged( iActivePlayer, iPrevActivePlayer )
	if (not ContextPtr:IsHidden()) then
		ContextPtr:SetHide(true);
	end
end
Events.GameplaySetActivePlayer.Add(OnActivePlayerChanged);

-----------------------------------------------------------------
-- Add Event Overview to Dropdown
-----------------------------------------------------------------
if Game.IsOption("GAMEOPTION_EVENTS") then
	LuaEvents.AdditionalInformationDropdownGatherEntries.Add(function(entries)
		table.insert(entries, {
			text = Locale.Lookup("TXT_KEY_EVENT_OVERVIEW"),
			call = function() 
				Events.SerialEventGameMessagePopup{ 
					Type = ButtonPopupTypes.BUTTONPOPUP_MODDER_6,
				};
			end,
		});
	end);
	-- Just in case :)
	LuaEvents.RequestRefreshAdditionalInformationDropdownEntries();
end
