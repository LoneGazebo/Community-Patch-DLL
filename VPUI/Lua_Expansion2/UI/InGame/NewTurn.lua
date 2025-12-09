if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- New Turn Popup
-------------------------------------------------
include( "IconSupport" );
include( "SupportFunctions" );
include( "GameplayUtilities" );

------------------------------------------------------------
------------------------------------------------------------
-- utility functions
function GetPlayer ()
	local iPlayerID = Game.GetActivePlayer();
	if (iPlayerID < 0) then
		print("Error - player index not correct");
		return nil;
	end

	if (not Players[iPlayerID]:IsHuman()) then
		return nil;
	end;

	return Players[iPlayerID];
end

-------------------------------------------------
-- OnTurnStart
-------------------------------------------------
function OnTurnStart ()

	-- if this is not the human player, ignore the turn ending
	local player = GetPlayer();
	if (player == nil) then
		return;	
	end

	if (not player:IsTurnActive()) then
		return;
	end

	-- Set Civ Icon
	CivIconHookup(  Game.GetActivePlayer(), 64, Controls.CivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true); 
		
	-- Update date
	local year = Game.GetGameTurnYear();
	local strDate;
	if(year < 0) then
		strDate = Locale.ConvertTextKey("TXT_KEY_TIME_BC", math.abs(year));
	else
		strDate = Locale.ConvertTextKey("TXT_KEY_TIME_AD", math.abs(year));
	end

	local player = Players[Game.GetActivePlayer()];
	local strInfo = GameplayUtilities.GetLocalizedLeaderTitle(player);
	
	Controls.Anim:SetHide( false );
	Controls.Anim:BranchResetAnimation();
	Controls.NewTurn:SetText(strDate);
	Controls.NewTurnInfo:SetText(strInfo);

	UIManager:SetUICursor( 0 );

end
Events.ActivePlayerTurnStart.Add( OnTurnStart );
