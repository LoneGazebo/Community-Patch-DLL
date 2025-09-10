--- @meta

--- @class TTManager
TTManager = {}

--- @class TooltipInstance
--- @field [string] Control

--- Turn `tooltipInstance` into a table containing controls under `Controls[strTooltipType]`<br>
--- It's unknown what happens to the original content of `tooltipInstance`. The only known usages all have an empty table passed in.
--- @param strTooltipType string The ID of a ToolTipType
--- @param tooltipInstance TooltipInstance Will be altered
function TTManager:GetTypeControlTable(strTooltipType, tooltipInstance) end

--- Set the basic tooltip template (the one used in [SetToolTipString](lua://Control.SetToolTipString)) GLOBALLY. See ToolTips.lua for more details.<br>
--- Don't call this unless you know what you're doing.
--- @param root Control the tool tip itself is anchored to the mouse by this and BranchResetAnimation begins here
--- @param container Control this is where dynamically created tool tip definitions will be added
--- @param label Control this is the TextControl used for the simple string-only tool tips
--- @param grid Control this is the GridControl used for backing of the Label and to check the overall tip against the screen bounds
function TTManager:RegisterBasicControls(root, container, label, grid) end
