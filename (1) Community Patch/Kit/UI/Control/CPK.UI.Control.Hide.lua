local _lua_type = type

local EachArgs = CPK.Args.Each

--- @param control Control
local function ControlHide(control)
	if control ~= nil and _lua_type(control.SetHide) == 'function' then
		control:SetHide(true)
	end
end

--- Hides controls by calling `SetHide(true)` if available.
--- ```lua
--- -- Example
--- Hide(Controls.CloseButton)
--- Hide(Controls.B1, nil, Controls.B3) -- Hides B1, B3
--- ```
--- @param control Control
--- @param ... Control
local function Hide(control, ...)
	ControlHide(control)
	EachArgs(ControlHide, ...)
end

CPK.UI.Control.Hide = Hide
