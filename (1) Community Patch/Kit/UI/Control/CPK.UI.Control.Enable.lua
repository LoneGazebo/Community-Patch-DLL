local _lua_type = type

local EachArgs = CPK.Args.Each

--- @param Control
local function ControlEnable(control)
	if control ~= nil and _lua_type(control.SetDisabled) == 'function' then
		control:SetDisabled(false)
	end
end

--- Enables controls by calling `SetDisabled(false)` if available.
--- ```lua
--- -- Example
--- Enable(Controls.CloseButton)
--- Enable(Controls.B1, nil, Controls.B3) -- Enables B1, B3
--- ```
--- @param Control
--- @param ... Control
local function Enable(control, ...)
	ControlEnable(control)
	EachArgs(ControlEnable, ...)
end

CPK.UI.Control.Enable = Enable
