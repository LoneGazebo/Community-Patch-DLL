if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- AdvisorInfo Popup
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );

local g_referencePlot = nil;
local g_bAttackingCity = false;

local m_PopupInfo = nil;

function HideAllAdvisors ()
	Controls.EconomicAdvisor:SetHide(true);
	Controls.ForeignAdvisor:SetHide(true);
	Controls.MilitaryAdvisor:SetHide(true);
	Controls.ScienceAdvisor:SetHide(true);
end

-------------------------------------------------
-- On Display
-------------------------------------------------
function OnPopup( popupInfo )

	if( popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_ADVISOR_MODAL ) then
		return;
	end

	m_PopupInfo = popupInfo;

	g_referencePlot = nil;
	g_bAttackingCity = false;
	
	g_bAttackingCity = popupInfo.Option1;

    HideAllAdvisors();

	Controls.DontShowAgainCheckbox:SetCheck(false);

    Controls.DescriptionLabel:SetText(Locale.ConvertTextKey(popupInfo.Text));	

	local imgCtrl = nil;
	local iAdvisor = popupInfo.Data1;
	local strAdvisorText;
	if (iAdvisor == AdvisorTypes.ADVISOR_MILITARY) then
		strAdvisorText = "TXT_KEY_ADVISOR_MILITARY_TITLE";
		imgCtrl = Controls.MilitaryAdvisor;	
	elseif (iAdvisor == AdvisorTypes.ADVISOR_ECONOMIC) then
		strAdvisorText = "TXT_KEY_ADVISOR_ECON_TITLE";
		imgCtrl = Controls.EconomicAdvisor;	
	elseif (iAdvisor == AdvisorTypes.ADVISOR_FOREIGN) then
	    strAdvisorText = "TXT_KEY_ADVISOR_FOREIGN_TITLE";
	    imgCtrl = Controls.ForeignAdvisor;	
	elseif (iAdvisor == AdvisorTypes.ADVISOR_SCIENCE) then
	    strAdvisorText = "TXT_KEY_ADVISOR_SCIENCE_TITLE";
	    imgCtrl = Controls.ScienceAdvisor;	
	end

    Controls.AdvisorLabel:SetText(Locale.ConvertTextKey(strAdvisorText));	

	local strTitle = "";
	if (g_bAttackingCity) then
		strTitle = "TXT_KEY_ADVISOR_CITY_ATTACK_DISPLAY";
	else
		strTitle = "TXT_KEY_ADVISOR_BAD_ATTACK_DISPLAY";
	end
	Controls.TitleLabel:SetText(Locale.ConvertTextKey(strTitle));
	Controls.PlayerTitleLabel:SetText(Locale.ConvertTextKey("TXT_KEY_ADVISOR_MODAL_TITLE"));    
    Controls.ConfirmButton:SetText(Locale.ConvertTextKey("TXT_KEY_ADVISOR_MODAL_CONFIRM"));
    Controls.CancelButton:SetText(Locale.ConvertTextKey("TXT_KEY_ADVISOR_MODAL_CANCEL"))
    
    -- Set Civ Icon
	CivIconHookup( Game.GetActivePlayer(), 64, Controls.Icon, Controls.CivIconBG, Controls.CivIconShadow, false, true );
    
    g_referencePlot = Map.GetPlotByIndex(popupInfo.Data2);
    
    if (imgCtrl) then	
		imgCtrl:SetHide(false);    
    end

	UIManager:QueuePopup( ContextPtr, PopupPriority.AdvisorModal );
end
Events.SerialEventGameMessagePopup.Add( OnPopup );


----------------------------------------------------------------        
----------------------------------------------------------------        
function Close ()
	if (Controls.DontShowAgainCheckbox:IsChecked()) then
		if (g_bAttackingCity) then
			Game.SetAdvisorCityAttackInterrupt(false);
		else
			Game.SetAdvisorBadAttackInterrupt(false);	
		end
	end

    UIManager:DequeuePopup( ContextPtr );
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnConfirmButtonClicked ()
	Close();
	
	-- Tell unit to move to position.
	if (g_referencePlot) then
		Game.SetCombatWarned(true);
		Game.SelectionListMove(g_referencePlot, false, false, false);	
	end
end
Controls.ConfirmButton:RegisterCallback( Mouse.eLClick, OnConfirmButtonClicked );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnCancelButtonClicked ()
	Close();
end
Controls.CancelButton:RegisterCallback( Mouse.eLClick, OnCancelButtonClicked );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
            Close();
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
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_ADVISOR_MODAL, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(Close);
