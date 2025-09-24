local IsTable = CPK.Type.IsTable
local ArgsEach = CPK.Args.Each
local ControlKind = CPK.UI.Control.Kind

--- Control-specific refresh handlers.
--- Each function performs the required recalculation steps for
--- controls that need explicit updates.
--- @type table<ControlKind, fun(control: Control): nil>
local Refresh = {
	StackControl = function(c)
		c:CalculateSize()
		c:ReprocessAnchoring()
	end,
	ScrollPanelControl = function(c)
		c:CalculateInternalSize()
	end
}

--- Refreshes a single control if it has a known refresh handler.
---
--- Safely checks the control’s kind, and if it matches one of the
--- supported refresh types (`StackControl`, `ScrollPanelControl`, ...),
--- calls the appropriate recalculation method(s).
---
--- If the control is not a table, has no kind, or its kind has no
--- handler defined in `Refresh`, the function does nothing.
--- @param control Control # The control to refresh
local function ControlRefreshOne(control)
	if not IsTable(control) then
		return
	end

	local kind = ControlKind(control)

	if not kind then
		return
	end

	local refresh = Refresh[kind]

	if not refresh then
		return
	end

	refresh(control)
end

--- Refreshes one or more controls.
---
--- Calls `ControlRefreshOne` on the first argument and then on all
--- variadic arguments (`...`) using `ArgsEach`.
---
--- ```lua
--- local Refresh = CPK.UI.Control.Refresh
---
--- local stack = Controls.MyStack
--- local scroll = Controls.MyStackScroll
---
--- -- Refreshes both in specified order
--- Refresh(stack, scroll)
--- ```
---
--- @param control Control # The first control to refresh
--- @param ... Control # Additional controls to refresh
local function ControlRefresh(control, ...)
	ControlRefreshOne(control)
	ArgsEach(ControlRefreshOne, ...)
end

CPK.UI.Control.Refresh = ControlRefresh
