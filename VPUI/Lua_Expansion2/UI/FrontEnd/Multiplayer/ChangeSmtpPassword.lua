if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Change smtp Server Password
-------------------------------------------------

local g_EditSlot = 0;

local passwordsMatchString = Locale.ConvertTextKey( "TXT_KEY_OPSCREEN_TURN_NOTIFY_SMTP_PASSWORDS_MATCH" );
local passwordsMatchTooltipString = Locale.ConvertTextKey( "TXT_KEY_OPSCREEN_TURN_NOTIFY_SMTP_PASSWORDS_MATCH_TT" );
local passwordsNotMatchString = Locale.ConvertTextKey( "TXT_KEY_OPSCREEN_TURN_NOTIFY_SMTP_PASSWORDS_NOT_MATCH" );
local passwordsNotMatchTooltipString = Locale.ConvertTextKey( "TXT_KEY_OPSCREEN_TURN_NOTIFY_SMTP_PASSWORDS_NOT_MATCH_TT" );

-------------------------------------------------
-------------------------------------------------
function OnCancel()
    UIManager:PopModal( ContextPtr );
    ContextPtr:CallParentShowHideHandler( true );
    ContextPtr:SetHide( true );
end
Controls.CancelButton:RegisterCallback( Mouse.eLClick, OnCancel );


-------------------------------------------------
-------------------------------------------------
function OnAccept()
	if( ValidateSmtpPassword() ) then
		OptionsManager.CommitGameOptions();
		UIManager:PopModal( ContextPtr );
		ContextPtr:CallParentShowHideHandler( true );
		ContextPtr:SetHide( true );
	end
end
Controls.AcceptButton:RegisterCallback( Mouse.eLClick, OnAccept );

----------------------------------------------------------------
-- Input processing
----------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE then
            OnCancel();  
        end
        if wParam == Keys.VK_RETURN then
            OnAccept();  
        end
    end
    return true;
end
ContextPtr:SetInputHandler( InputHandler );

-------------------------------------------------
-------------------------------------------------
function ShowHideHandler( bIsHide, bIsInit )
    
	if( not bIsHide ) then
		Controls.TurnNotifySmtpPassEdit:SetText(OptionsManager.GetTurnNotifySmtpPassword_Cached());
		Controls.TurnNotifySmtpPassRetypeEdit:SetText(OptionsManager.GetTurnNotifySmtpPassword_Cached());
		ValidateSmtpPassword(); -- Update passwords match label
		Controls.TurnNotifySmtpPassEdit:TakeFocus();
				
		-- Adjust the overall size of the box
		--[[
		local iBoxHeight = 404;
		if ( bIsHotSeat ) then
			iBoxHeight = 584;
		end
		
		local frameSize = {};
		frameSize = Controls.MainBox:GetSize();
		frameSize.y = iBoxHeight;
		Controls.MainBox:SetSize( frameSize );
		frameSize = Controls.BackgroundBox:GetSize();
		frameSize.y = iBoxHeight;
		Controls.BackgroundBox:SetSize( frameSize );
		frameSize = Controls.FrameBox:GetSize();
		frameSize.y = iBoxHeight;
		Controls.FrameBox:SetSize( frameSize );
		]]--	     
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );


-------------------------------------------------
-------------------------------------------------
function ValidateSmtpPassword()
	if( Controls.TurnNotifySmtpPassEdit:GetText() and Controls.TurnNotifySmtpPassRetypeEdit:GetText() and
		Controls.TurnNotifySmtpPassEdit:GetText() == Controls.TurnNotifySmtpPassRetypeEdit:GetText() ) then
		-- password editboxes match.
		OptionsManager.SetTurnNotifySmtpPassword_Cached( Controls.TurnNotifySmtpPassEdit:GetText() );
		Controls.StmpPasswordMatchLabel:SetText(passwordsMatchString);
		Controls.StmpPasswordMatchLabel:SetToolTipString(passwordsMatchTooltipString);
		Controls.StmpPasswordMatchLabel:SetColorByName( "Green_Chat" );
		Controls.AcceptButton:SetDisabled(false);	
		return true;
	else
		-- password editboxes do not match.
		Controls.StmpPasswordMatchLabel:SetText(passwordsNotMatchString);
		Controls.StmpPasswordMatchLabel:SetToolTipString(passwordsNotMatchTooltipString);
		Controls.StmpPasswordMatchLabel:SetColorByName( "Magenta_Chat" );		
		Controls.AcceptButton:SetDisabled(true);	
		return false;
	end
end
Controls.TurnNotifySmtpPassEdit:RegisterCallback( ValidateSmtpPassword );
Controls.TurnNotifySmtpPassRetypeEdit:RegisterCallback( ValidateSmtpPassword );

