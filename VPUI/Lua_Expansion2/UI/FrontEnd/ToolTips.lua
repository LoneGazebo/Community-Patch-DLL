if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
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