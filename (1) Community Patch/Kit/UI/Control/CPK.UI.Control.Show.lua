local _lua_type = type

local ArgsEach = CPK.Args.Each

--- @param control Control
local function ControlShow(control)
	if control ~= nil and _lua_type(control.SetHide) == 'function' then
		control:SetHide(false)
	end
end

--- Shows controls by calling `SetHide(false)` if available.
--- ```lua
--- -- Example
--- Show(Controls.CloseButton)
--- Show(Controls.B1, nil, Controls.B3) -- Shows B1, B3
--- ```
--- @param control Control
--- @param ... Control
local function Show(control, ...)
	ControlShow(control)
	ArgsEach(ControlShow, ...)
end

CPK.UI.Control.Show = Show
