if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
include("TutorialChecks");

------------------------------------------------------------
------------------------------------------------------------

-- An extremely hacky way to determine which tutorial is to be run
if (GameDefines.TUTORIAL == 0) then
	Game.SetStaticTutorialActive(true);
	include("Tutorial_MainGame");
elseif (GameDefines.TUTORIAL == 1) then
	include("Tutorial_1_MovementAndExploration");
elseif (GameDefines.TUTORIAL == 2) then
	include("Tutorial_2_FoundCities");
elseif (GameDefines.TUTORIAL == 3) then
	include("Tutorial_3_ImprovingCities");
elseif (GameDefines.TUTORIAL == 4) then
	include("Tutorial_4_CombatAndConquest");
elseif (GameDefines.TUTORIAL == 5) then
	include("Tutorial_5_Diplomacy");
else
	include("Tutorial_MainGame");
end
------------------------------------------------------------
------------------------------------------------------------

include("TutorialEngine");