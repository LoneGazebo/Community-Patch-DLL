if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Confirm Player Kick
-------------------------------------------------

local g_kickIdx = 0;
local g_kickName = "";

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
	print("OnAccept: g_kickIdx: " .. tostring(g_kickIdx));
	Matchmaking.KickPlayer( g_kickIdx );
	UIManager:PopModal( ContextPtr );
	ContextPtr:CallParentShowHideHandler( true );
	ContextPtr:SetHide( true );
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
		-- Set player name in popup
		UpdateKickLabel();
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

-------------------------------------------------
-------------------------------------------------
function UpdateKickLabel()
	Controls.KickDialog:LocalizeAndSetText("TXT_KEY_CONFIRM_KICK_PLAYER_DESC", g_kickName);
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
LuaEvents.SetKickPlayer.Add(function(playerID, playerName)
	g_kickIdx = playerID;
	g_kickName = playerName;
	UpdateKickLabel();
end);

