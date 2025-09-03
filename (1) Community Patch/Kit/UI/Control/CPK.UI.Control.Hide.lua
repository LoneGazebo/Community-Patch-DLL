local IsTable = CPK.Type.IsTable
local IsCallable = CPK.Type.IsCallable

local ArgsEach = CPK.Args.Each

--- @param control Control
local function ControlHideOne(control)
	if IsTable(control) and IsCallable(control.SetHide) then
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
local function ControlHide(control, ...)
	ControlHideOne(control)
	ArgsEach(ControlHideOne, ...)
end

CPK.UI.Control.Hide = ControlHide
