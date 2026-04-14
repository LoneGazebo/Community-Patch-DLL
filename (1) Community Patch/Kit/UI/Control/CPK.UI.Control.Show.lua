local IsTable = CPK.Type.IsTable
local IsFunction = CPK.Type.IsFunction

local ArgsEach = CPK.Args.Each

--- @param control Control
local function ControlShowOne(control)
	if IsTable(control) and IsFunction(control.SetHide) then
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
	return ArgsEach(ControlShowOne, control, ...)
end

CPK.UI.Control.Show = ControlShow
