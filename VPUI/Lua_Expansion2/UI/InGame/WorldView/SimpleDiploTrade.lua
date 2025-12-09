if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Diplomacy and Advisors Buttons that float out in the screen
-------------------------------------------------
include( "TradeLogic" );
Events.OpenPlayerDealScreenEvent.Add( OnOpenPlayerDealScreen );
Controls.ModifyButton:RegisterCallback( Mouse.eLClick, OnModify );



ContextPtr:SetInputHandler( InputHandler );

Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnBack );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnDiploActivePlayerChanged( iActivePlayer, iPrevActivePlayer )
	if (not ContextPtr:IsHidden()) then
		OnBack();
	end
end
Events.GameplaySetActivePlayer.Add(OnDiploActivePlayerChanged);

local sgPrevCursor = 0;
----------------------------------------------------------------        
----------------------------------------------------------------  
function OnShowHide( bHide )
	if(not bHide) then 
		sgPrevCursor = UIManager:SetUICursor(0);
		LuaEvents.OpenSimpleDiploTrade();
	else
		UIManager:SetUICursor(sgPrevCursor);
	end
end
ContextPtr:SetShowHideHandler( OnShowHide );


----------------------------------------------------------------        
----------------------------------------------------------------  
function OnOpenAILeaderDiploTrade()
	-- An AI leader trade was started while we're open, close ourselves.
	if (not ContextPtr:IsHidden()) then
		OnBack();
	end
end
LuaEvents.OpenAILeaderDiploTrade.Add(OnOpenAILeaderDiploTrade);
