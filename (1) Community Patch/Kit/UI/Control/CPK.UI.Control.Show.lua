local IsTable = CPK.Type.IsTable
local IsCallable = CPK.Type.IsCallable

local ArgsEach = CPK.Args.Each

--- @param control Control
local function ControlShowOne(control)
	if IsTable(control) and IsCallable(control.SetHide) then
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
local function ControlShow(control, ...)
	ControlShowOne(control)
	ArgsEach(ControlShowOne, ...)
end

CPK.UI.Control.Show = ControlShow
