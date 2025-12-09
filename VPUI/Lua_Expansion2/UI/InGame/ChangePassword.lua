if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Change password screen
-------------------------------------------------
include( "IconSupport" );

-------------------------------------------------
-------------------------------------------------
function OnOK()
	local ePlayer = Game.GetActivePlayer();
	if (PreGame.TestPassword( ePlayer, Controls.OldPasswordEditBox:GetText() ) ) then
		if ( Controls.NewPasswordEditBox:GetText() == Controls.RetypeNewPasswordEditBox:GetText() ) then
			PreGame.SetPassword( ePlayer, Controls.NewPasswordEditBox:GetText(), Controls.OldPasswordEditBox:GetText() );
			
			UIManager:PopModal( ContextPtr );
			ContextPtr:SetHide( true );
			LuaEvents.PasswordChanged( Game.GetActivePlayer() );
		end
	end
end
Controls.OKButton:RegisterCallback( Mouse.eLClick, OnOK );

-------------------------------------------------
-------------------------------------------------
function OnCancel()
	UIManager:PopModal( ContextPtr );
	ContextPtr:SetHide( true );
end
Controls.CancelButton:RegisterCallback( Mouse.eLClick, OnCancel );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_RETURN then
            OnOK();
            return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );

----------------------------------------------------------------
----------------------------------------------------------------
function ValidateText(text)
	local isAllWhiteSpace = true;
	for i = 1, #text, 1 do
		if(string.byte(text, i) ~= 32) then
			isAllWhiteSpace = false;
			break;
		end
	end
	
	if(isAllWhiteSpace) then
		return false;
	end
	
	-- don't allow % character
	for i = 1, #text, 1 do
		if string.byte(text, i) == 37 then
			return false;
		end
	end
	
	local invalidCharArray = { '\"', '<', '>', '|', '\b', '\0', '\t', '\n', '/', '\\', '*', '?' };

	for i, ch in ipairs(invalidCharArray) do
		if(string.find(text, ch) ~= nil) then
			return false;
		end
	end
	
	-- don't allow control characters
	for i = 1, #text, 1 do
		if (string.byte(text, i) < 32) then
			return false;
		end
	end

	return true;
end

---------------------------------------------------------------
function Validate()
	local bValid = false;

	if(	ValidateText(Controls.NewPasswordEditBox:GetText()) and
		ValidateText(Controls.RetypeNewPasswordEditBox:GetText()) and
		Controls.NewPasswordEditBox:GetText() == Controls.RetypeNewPasswordEditBox:GetText() and
		PreGame.TestPassword( Game.GetActivePlayer(), Controls.OldPasswordEditBox:GetText() ) ) then		
		bValid = true;
	end
	
	
	Controls.OKButton:SetDisabled(not bValid);
end
---------------------------------------------------------------
Controls.OldPasswordEditBox:RegisterCallback(Validate);
Controls.NewPasswordEditBox:RegisterCallback(Validate);
Controls.RetypeNewPasswordEditBox:RegisterCallback(Validate);

-------------------------------------------------
-------------------------------------------------
function ShowHideHandler( bIsHide, bIsInit )
    
	if( not bIsHide ) then
		
		local bHasPassword = PreGame.HasPassword( Game.GetActivePlayer() );
		Controls.OldPasswordStack:SetHide( not bHasPassword );
		Controls.OldPasswordEditBox:SetText( "" );
		Controls.NewPasswordEditBox:SetText( "" );
		Controls.RetypeNewPasswordEditBox:SetText( "" );		
		
        Controls.OKButton:SetDisabled(true);
        if (bHasPassword) then
       		Controls.OldPasswordEditBox:TakeFocus();
       	else
       		Controls.NewPasswordEditBox:TakeFocus();
       	end
       	
      	local pPlayer = Players[Game.GetActivePlayer()];
		if(pPlayer ~= nil) then
			local civ = GameInfo.Civilizations[pPlayer:GetCivilizationType()];
			if(civ ~= nil) then
				IconHookup( civ.PortraitIndex, 128, civ.IconAtlas, Controls.Icon );
			end
		end
	end		
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

