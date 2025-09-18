local lua_type = type

local ArgsEach = CPK.Args.Each

--- @param control Control
local function ControlDisableOne(control)
	if control ~= nil and lua_type(control.SetDisabled) == 'function' then
		control:SetDisabled(true)
	end
end

--- Disables controls by calling `SetDisabled(true)` if available.
--- ```lua
--- -- Example
--- Disable(Controls.CloseButton)
--- Disable(Controls.B1, nil, Controls.B3) -- Disables B1, B3
--- ```
--- @param control Control
--- @param ... Control
local function ControlDisable(control, ...)
	ControlDisableOne(control)
	ArgsEach(ControlDisableOne, ...)
end

CPK.UI.Control.Disable = ControlDisable
