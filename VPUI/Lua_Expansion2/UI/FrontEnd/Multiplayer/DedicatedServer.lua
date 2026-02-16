if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end

----------------------------------------------------------------        
function ShowHideHandler( isHide )
	-- Check to make sure we are not launching, this menu can briefly get unhidden as the game launches
	if not Matchmaking.IsLaunchingGame() then 
		if not isHide then

			PreGame.SetInternetGame( true );
			PreGame.SetGameType(GameTypes.GAME_NETWORK_MULTIPLAYER);
			PreGame.ResetSlots();
			UIManager:QueuePopup( Controls.MPGameSetupScreen, PopupPriority.MPGameSetupScreen );
		end
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );
