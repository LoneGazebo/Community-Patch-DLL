if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
--------------------------------------------------------------
-- LeagueSplash.lua
-- Author: Anton Strenger
-- DateCreated: 2/22/2013
--
-- Splash screen for when the World Congress is first 
-- established in the game.
--------------------------------------------------------------

-----------------------------
-- Globals --
-----------------------------
local m_PopupInfo = nil;

-----------------------------
-- On Popup --
-----------------------------
function OnPopup( popupInfo )

	if( popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_LEAGUE_SPLASH ) then
		return;
	end

	m_PopupInfo = popupInfo;

    local iLeague = popupInfo.Data1;
    local iFounder = popupInfo.Data2;
    local iGoverningSpecialSession = popupInfo.Data3;
    local bFirstSession = popupInfo.Option1;
    
    local sTitle = Locale.Lookup("TXT_KEY_LEAGUE_SPLASH_TITLE_FOUNDED");
    local sMessage = "";
    local sThisEra = "";
    local sNextEra = "";
    if (iLeague ~= nil and iGoverningSpecialSession ~= nil) then
		local pLeague = Game.GetLeague(iLeague);
		local tGoverningSpecialSession = GameInfo.LeagueSpecialSessions[iGoverningSpecialSession];
		if (pLeague ~= nil and tGoverningSpecialSession ~= nil) then
			sTitle = pLeague:GetLeagueSplashTitle(iGoverningSpecialSession, bFirstSession);
			sMessage = pLeague:GetLeagueSplashDescription(iGoverningSpecialSession, bFirstSession);
			sThisEra = pLeague:GetLeagueSplashThisEraDetails(iGoverningSpecialSession, bFirstSession);
			sNextEra = pLeague:GetLeagueSplashNextEraDetails(iGoverningSpecialSession, bFirstSession);
			Controls.LeagueImage:SetTexture(tGoverningSpecialSession.SessionSplashImage);
		end
	end
	Controls.TitleLabel:SetText(sTitle);
	Controls.DescriptionLabel:SetText(sMessage);
	Controls.ThisEraLabel:SetText(sThisEra .. "[NEWLINE]");
	Controls.NextEraLabel:SetText(sNextEra .. "[NEWLINE]");
	
	Controls.DescriptionScrollPanel:CalculateInternalSize();
	
	UIManager:QueuePopup( ContextPtr, PopupPriority.GenericPopup );
end
Events.SerialEventGameMessagePopup.Add( OnPopup );


-----------------------------
-- Input processing --
-----------------------------
function OnClose()
    Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_LEAGUE_SPLASH, 0);
    UIManager:DequeuePopup( ContextPtr );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose);

function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
            OnClose();
            return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );

function ShowHideHandler( bIsHide, bInitState )

    if( not bInitState ) then
       if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
        else
            UI.decTurnTimerSemaphore();
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(OnClose);
