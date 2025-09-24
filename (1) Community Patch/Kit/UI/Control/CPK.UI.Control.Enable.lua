local lua_type = type

local ArgsEach = CPK.Args.Each

--- @param control Control
local function ControlEnableOne(control)
	if control ~= nil and lua_type(control.SetDisabled) == 'function' then
		control:SetDisabled(false)
	end
end

--- Enables controls by calling `SetDisabled(false)` if available.
--- ```lua
--- -- Example
--- Enable(Controls.CloseButton)
--- Enable(Controls.B1, nil, Controls.B3) -- Enables B1, B3
--- ```
--- @param control Control
--- @param ... Control
local function ControlEnable(control, ...)
	ControlEnableOne(control)
	ArgsEach(ControlEnableOne, ...)
end

CPK.UI.Control.Enable = ControlEnable
