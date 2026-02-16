if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
function ModeClicked( void1, Control )
	if Control == 0 then
		ContextPtr:SetHide( true );
		--ContextPtr:LookUpControl( "../DebugMenu"):SetHide(false);
	end
end

Controls.Exit_Button:RegisterCallback( Mouse.eLClick, ModeClicked );