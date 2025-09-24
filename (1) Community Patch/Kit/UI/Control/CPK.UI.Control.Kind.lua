local lua_getmetatable = getmetatable
local AssertIsTable = CPK.Assert.IsTable

--- The kinds of UI controls available in the game.
--- This hierarchy is incomplete; contribute if you discover more.
--- (`->` means "extends from")
---
--- @alias ControlKind
--- | 'ControlBase'             #
--- | 'LuaContext'              # -> ControlBase
--- | 'ButtonControlBase'       # -> ControlBase
--- | 'AnimControl'             # -> ControlBase
--- | 'FlipAnim'                # -> AnimControl
--- | 'AlphaAnim'               # -> AnimControl
--- | 'ScrollAnim'              # -> AnimControl
--- | 'BarControl'              # -> ControlBase
--- | 'GridControl'             # -> ControlBase
--- | 'TextControl'             # -> ControlBase
--- | 'ImageControl'            # -> ControlBase
--- | 'StackControl'            # -> ControlBase
--- | 'MeterControl'            # -> ControlBase
--- | 'ColorBoxControl'         # -> ControlBase
--- | 'ReplayMapControl'        # -> ControlBase
--- | 'TextureBarControl'       # -> ControlBase
--- | 'ScrollPanelControl'      # -> ControlBase
--- | 'ButtonControl'           # -> ButtonControlBase
--- | 'SliderControl'           # -> ButtonControlBase
--- | 'EditBoxControl'          # -> ButtonControlBase
--- | 'TextButtonControl'       # -> ButtonControlBase
--- | 'GridButtonControl'       # -> ButtonControlBase
--- | 'PullDownControl'         # -> ButtonControlBase
--- | 'CheckBoxControl'         # -> ButtonControlBase
--- | 'RadioButtonControl'      # -> CheckBoxControl
---
--- Retrieves the kind (class name) of the given UI control.
--- This reads the `CTypeName` field from the control's metatable.
---
--- ```lua
--- local Kind = CPK.UI.Control.Kind
---
--- local btn = Controls.CloseButton
--- print(Kind(btn)) -- "ButtonControl"
---
--- local slider = ContextPtr:LookUpControl("/Some/Path/To/VolumeSlider")
--- print(Kind(slider)) -- "SliderControl"
--- ```
---
--- @param control Control # A valid UI control instance
--- @return ControlKind # The kind of control
--- @nodiscard
local function ControlKind(control)
	AssertIsTable(control)

	local meta = lua_getmetatable(control)
	return meta.CTypeName
end

CPK.UI.Control.Kind = ControlKind
