local _lua_type = type

local EachArgs = CPK.Args.Each

--- @param Control
local function ControlDisable(control)
	if control ~= nil and _lua_type(control.SetDisabled) == 'function' then
		control:SetDisabled(false)
	end
end

--- Disables controls by calling `SetDisabled(true)` if available.
--- ```lua
--- -- Example
--- Disable(Controls.CloseButton)
--- Disable(Controls.B1, nil, Controls.B3) -- Disables B1, B3
--- ```
--- @param Control
--- @param ... Control
local function Disable(control, ...)
	ControlDisable(control)
	EachArgs(ControlDisable, ...)
end

CPK.UI.Control.Disable = Disable
