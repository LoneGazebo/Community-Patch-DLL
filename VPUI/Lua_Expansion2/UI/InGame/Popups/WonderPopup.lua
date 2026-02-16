if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Wonder popup
-------------------------------------------------

include( "IconSupport" );
include( "InfoTooltipInclude" );

local m_fAnimSeconds = 5;
local m_fScaleFactor = 0.5;
local m_fPct = 0;
local m_fOriginalSizeX = 0;
local m_fOriginalSizeY = 0;
local m_strAudioFile = "";
local m_PopupInfo = nil;

local lastBackgroundImage = "WonderConceptStonehedge.dds"

-------------------------------------------------
-- On Popup
-------------------------------------------------
function OnPopup( popupInfo )

	if( popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_WONDER_COMPLETED_ACTIVE_PLAYER ) then
		return;
	end
	
	if(Game == nil or Game.GetGameTurn() <= Game.GetStartTurn()) then
		return;
	end
	
	local iBuildingID = popupInfo.Data1;
	local thisBuilding = GameInfo.Buildings[ iBuildingID ];
	if( thisBuilding == nil ) then
    	return;
	end

	m_PopupInfo = popupInfo;

    if( thisBuilding.WonderSplashImage ~= nil ) then
    
        if( thisBuilding.WonderSplashAnchor ~= nil ) then
            Controls.WonderSplash:SetAnchor( thisBuilding.WonderSplashAnchor );
        else
            Controls.WonderSplash:SetAnchor( "T,R" );
        end
        Controls.WonderSplash:ReprocessAnchoring();
        lastBackgroundImage = thisBuilding.WonderSplashImage;
        Controls.WonderSplash:SetTextureAndResize( thisBuilding.WonderSplashImage );
    	
    	m_fOriginalSizeX, m_fOriginalSizeY = Controls.WonderSplash:GetSizeVal();
    	Controls.WonderSplash:SetSizeVal( m_fOriginalSizeX * (1 + m_fScaleFactor), m_fOriginalSizeY * (1 + m_fScaleFactor) );
    	
        ContextPtr:SetUpdate( OnUpdate );
    end

	IconHookup( thisBuilding.PortraitIndex, 128, thisBuilding.IconAtlas, Controls.WonderIcon)

    if( thisBuilding.Description ~= nil ) then
    	Controls.Title:SetText( Locale.ConvertTextKey( thisBuilding.Description ) );
    	Controls.LowerTitle:SetText( Locale.ConvertTextKey( thisBuilding.Description ) );
    	Controls.Title:SetHide( false );
    	Controls.LowerTitle:SetHide( false );
    else
    	Controls.Title:SetHide( true );
    	Controls.LowerTitle:SetHide( true );
	end
    if( thisBuilding.Quote ~= nil ) then
        Controls.Quote:SetText( Locale.ConvertTextKey( thisBuilding.Quote ) );
    	Controls.Quote:SetHide( false );
    else
    	Controls.Quote:SetHide( true );
    end

	-- Game Info
	local strGameInfo = thisBuilding.Help;

	if (strGameInfo == nil) then
		local bExcludeName = true;
		local bExcludeHeader = true;
		strGameInfo = GetHelpTextForBuilding(iBuildingID, bExcludeName, bExcludeHeader, false);
	end
    
    if( strGameInfo ~= nil ) then
        Controls.Stats:SetText( Locale.ConvertTextKey( strGameInfo ) );
    	Controls.Stats:SetHide( false );
    else
    	Controls.Stats:SetHide( true );
    end
    
	UIManager:QueuePopup( ContextPtr, PopupPriority.WonderPopup );
	
    m_fPct = 0;
    OnUpdate( 0 );
end
Events.SerialEventGameMessagePopup.Add( OnPopup );


----------------------------------------------------------------
----------------------------------------------------------------
function OnUpdate( fDTime )
    m_fPct = m_fPct + (fDTime / m_fAnimSeconds);
    
    if( m_fPct > 1 ) then
        ContextPtr:ClearUpdate();
    	--------------------------------------------------------------------------------------------
    	print( "setting to original size " .. m_fOriginalSizeX .. " " .. m_fOriginalSizeY );
    	
    	Controls.WonderSplash:SetSizeVal( m_fOriginalSizeX, m_fOriginalSizeY );
	else
	    local fScale = 1 + ((1 - m_fPct) * (1 - m_fPct) * m_fScaleFactor);
    	Controls.WonderSplash:SetSizeVal( m_fOriginalSizeX * fScale,
                                          m_fOriginalSizeY * fScale );
    end
end


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
    if( not bInitState ) then
        Controls.WonderSplash:UnloadTexture();
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
			Controls.WonderSplash:SetTextureAndResize( lastBackgroundImage );
        else
            UI.decTurnTimerSemaphore();
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_WONDER_COMPLETED_ACTIVE_PLAYER, 0);
            ContextPtr:ClearUpdate();
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(OnClose);
