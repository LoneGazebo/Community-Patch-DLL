-------------------------------------------------
-- Diplomacy and Advisors Buttons that float out in the screen
-- modified by bc1 from 1.0.3.144 brave new world code
-- fix spinning globe bug
-------------------------------------------------
include( "TradeLogic" );
Events.OpenPlayerDealScreenEvent.Add( OnOpenPlayerDealScreen );
Controls.ModifyButton:RegisterCallback( Mouse.eLClick, OnModify );



ContextPtr:SetInputHandler( InputHandler );

Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnBack );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnDiploActivePlayerChanged()
	if not ContextPtr:IsHidden() then
		OnBack();
	end
end
Events.GameplaySetActivePlayer.Add(OnDiploActivePlayerChanged);


----------------------------------------------------------------        
----------------------------------------------------------------  
ContextPtr:SetShowHideHandler( function( isHide, isInit )
	OnShowHide( isHide, isInit );-- the one defined in TradeLogic which got overwritten by Firaxis, leading to the bug
	if not isInit and not isHide then
		LuaEvents.OpenSimpleDiploTrade();
	end
end);


----------------------------------------------------------------        
----------------------------------------------------------------  
function OnOpenAILeaderDiploTrade()
	-- An AI leader trade was started while we're open, close ourselves.
	if not ContextPtr:IsHidden() then
		OnBack();
	end
end
LuaEvents.OpenAILeaderDiploTrade.Add(OnOpenAILeaderDiploTrade);
