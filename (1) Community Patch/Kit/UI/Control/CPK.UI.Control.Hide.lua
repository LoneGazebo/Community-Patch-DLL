local IsTable = CPK.Type.IsTable
local IsFunction = CPK.Type.IsFunction

local ArgsEach = CPK.Args.Each

--- @param control Control
local function ControlHideOne(control)
	if IsTable(control) and IsFunction(control.SetHide) then
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
--- @return Control ...
local function ControlHide(control, ...)
	return ArgsEach(ControlHideOne, control, ...)
end

CPK.UI.Control.Hide = ControlHide
