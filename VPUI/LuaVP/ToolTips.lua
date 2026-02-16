-------------------------------------------------
-------------------------------------------------


--------------------------------------------------------------------------------------
-- this function call registers the components in ToolTips.xml for use as the simple
-- tool tip frame.
-- 
-- RegisterBasicControls( Root, Container, Label, Grid );
-- Root      - the tool tip itself is anchored to the mouse by this and BranchResetAnimation begins here
-- Container - this is where dynamically created tool tip definitions will be added
-- Label     - this is the TextControl used for the simple string-only tool tips
-- Grid      - this is the GridControl used for backing of the Label and to check the
--             overall tip against the screen bounds
--------------------------------------------------------------------------------------
TTManager:RegisterBasicControls( Controls.ToolTipRoot, Controls.ToolTipStore, Controls.ToolTipText, Controls.ToolTipGrid );

--if true then return end ;

local wrappedinit = coroutine.wrap( function() include("vploader") end )
local print = print ;
local CodeBuddy = CodeBuddy ;

CodeBuddy.vpRegisterContext = function(...)
	CodeBuddy.vpRegisterContext = nil ; -- delete this function
	wrappedinit(); -- run vploader
	if CodeBuddy.vpRegisterContext then  -- should now exist, so call it
		return CodeBuddy.vpRegisterContext(...) -- ... and return all returned values from that call
	else
		print("[WARN] vploader didn't set vpRegisterContext!") -- print a warning
		return true ; -- return true, allowing the game to load normally
	end
end