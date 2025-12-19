if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
function OnExitGame()
	UI.ExitGame();
end
Events.UserRequestClose.Add( OnExitGame );