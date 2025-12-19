if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-------------------------------------------------
function ResetMultiplayerOptions()

	-- Default custom civ names
	PreGame.SetLeaderName( 0, "");
	PreGame.SetCivilizationDescription( 0, "");
	PreGame.SetCivilizationShortDescription( 0, "");
	PreGame.SetCivilizationAdjective( 0, "");
	
		-- Default Map Size
	local worldSize = GameInfo.Worlds["WORLDSIZE_SMALL"];
	if(worldSize == nil) then
		worldSize = GameInfo.Worlds()(); -- Get first world size found.
	end
	PreGame.SetRandomWorldSize( false );
	PreGame.SetWorldSize( worldSize.ID );
	PreGame.SetNumMinorCivs( worldSize.DefaultMinorCivs );
	
	-- Default Map Type
	PreGame.SetLoadWBScenario(false);
	PreGame.SetRandomMapScript(false);
	local mapScript = GameInfo.MapScripts{FileName = "Assets\\Maps\\Continents.lua"}();
	if(mapScript ~= nil) then
		PreGame.SetMapScript(mapScript.FileName);
	else
		-- This shouldn't happen.  The backup plan is to find the first map script that supports multiplayer.
		for row in GameInfo.MapScripts{SupportsMultiplayer = 1} do
			PreGame.SetMapScript(row.FileName);
			break;
		end
	end

	-- Default Game Pace
	PreGame.SetGameSpeed(3);

	-- Default Start Era
	PreGame.SetEra(0);

	-- Reset Max Turns
	PreGame.SetMaxTurns(0);

	PreGame.ResetGameOptions();
	PreGame.ResetMapOptions();
	
	-- Default Game Options
	if (PreGame.IsHotSeatGame()) then
		PreGame.SetGameOption("GAMEOPTION_DYNAMIC_TURNS", false);
		PreGame.SetGameOption("GAMEOPTION_SIMULTANEOUS_TURNS", false);
	else
		PreGame.SetGameOption("GAMEOPTION_DYNAMIC_TURNS", true);
		PreGame.SetGameOption("GAMEOPTION_SIMULTANEOUS_TURNS", false);
	end
end

-------------------------------------------------
-------------------------------------------------

