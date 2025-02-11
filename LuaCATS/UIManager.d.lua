--- @meta

--- @class UIManager
UIManager = {}

--- TODO docs
--- @param context Context
function UIManager:DequeuePopup(context) end

--- TODO docs
--- @param context Context
--- @param popupPriority PopupPriority
function UIManager:QueuePopup(context, popupPriority) end

--- Checks if `ALT` key is being pressed.
--- @return boolean
--- @see UI.AltKeyDown
function UIManager:GetAlt() end

--- Checks if `SHIFT` key is being pressed.
--- @return boolean
--- @see UI.ShiftKeyDown
function UIManager:GetShift() end

--- Checks if `CTRL` key is being pressed.
--- @return boolean
--- @see UI.CtrlKeyDown
function UIManager:GetControl() end

--- TODO docs
--- @return number, number
function UIManager:GetMouseDelta() end

--- Gets LATEST mouse cursor position within game screen window.
--- Result is within the result of `UIManager:GetScreenSizeVal()`
--- @return integer # X
--- @return integer # Y
function UIManager:GetMousePos() end

--- TODO docs
--- @param arg0 string
--- @param arg1 integer # TODO should be PopupPriority?
function UIManager:GetNamedContextByIndex(arg0, arg1) end

--- TODO docs
--- @param arg0 string
--- @return integer
function UIManager:GetNamedContextCount(arg0) end

--- TODO docs
--- @return integer
function UIManager:GetNumPointers() end

--- Gets the number of available resolution modes.
--- ```lua
--- -- Example for display 3440x1440
--- UIManager:GetResInfo(UIManager:GetResCount()) -- Output: 0, 0, 0, 0, 0, 0
--- UIManager:GetResInfo(UIManager:GetResCount() - 1) -- Output: 3440, 1440, 144, 0, 0, 0
--- ```
--- @return integer
function UIManager:GetResCount() end

--- Gets available resolution paramaters by specified resolution mode index.
--- ```lua
--- -- Example for display 3440x1440
--- UIManager:GetResInfo(UIManager:GetResCount()) -- Output: 0, 0, 0, 0, 0, 0
--- UIManager:GetResInfo(UIManager:GetResCount() - 1) -- Output: 3440, 1440, 144, 0, 0, 0
--- ```
--- @param resolutionModeIdx integer # Should be in range `[0..UIManager:GetResCount()]`
--- @see UIManager.GetResCount
--- @return integer # Screen Width
--- @return integer # Screen Height
--- @return integer # Screen Refresh Rate
--- @return integer # TODO
--- @return integer # TODO
--- @return integer # TODO
function UIManager:GetResInfo(resolutionModeIdx) end

--- Gets current game screen width and height.
--- ```lua
--- -- Example for game running in windowed 2560x1440
--- UIManager:GetScreenSizeVal() -- Output: 2554, 1405
--- ```
--- @return integer # Width
--- @return integer # Height
function UIManager:GetScreenSizeVal() end

--- TODO docs
--- @param arg0 string
--- @return unknown
function UIManager:GetVisibleNamedContext(arg0) end

--- TODO docs
--- @param context Context
--- @return boolean
function UIManager:IsModal(context) end

--- TODO docs
--- @param context Context
--- @return boolean
function UIManager:IsTopModal(context) end

--- TODO docs
--- @param context Context
function UIManager:PopModal(context) end

--- TODO docs
--- @param context Context
--- @param arg1? boolean
function UIManager:PushModal(context, arg1) end

--- Sets cursor icon by specified idx (TODO verify if there is an enum for this?)
--- ```lua
--- -- Example 1
--- UIManager:SetUICursor(0) -- Default
--- UIManager:SetUICursor(1) -- Loading
--- UIManager:SetUICursor(24) -- Gift
--- UIManager:SetUICursor(25) -- Anything above 24 makes cursor system default
---
--- -- Example 2
--- local previousCursorIdx = UIManager:SetUICursor(1) -- Make cursor loading
--- -- ... do some stuff here
--- UIManager:SetUICursor(previousCursorIdx) -- Make cursor previous
--- ```
--- @param cursorIdx integer # 0 is default cursor, 1 is loading cursor, should be within `[0, 24]`
--- @return integer # Previous cursor index. If previous cursor index was greater than 24, returns 63
function UIManager:SetUICursor(cursorIdx) end

--- TODO docs
--- @return boolean
function UIManager:IsExtendedFont() end

--- TODO docs
--- @param arg0 integer
--- @return { x: integer, y: integer }
function UIManager:GetGameGridTopLeft(arg0) end

--- TODO docs
--- @param arg0 integer
--- @return { x: integer, y: integer }
function UIManager:GetGameGridBottomRight(arg0) end

--- TODO docs
--- @param arg0 unknown
function UIManager:ApplyResolution(arg0) end

--- TODO docs
function UIManager:FlushInputMessages() end

--- TODO docs
function UIManager:PopAllInterrupts() end

--- TODO docs
function UIManager:PrintAllocationInfo() end

--- TODO docs
function UIManager:PrintTextureInfo() end

--- TODO docs
--- @param arg0 unknown
function UIManager:PrintWindowInfo(arg0) end

--- TODO docs
--- @param arg0 unknown
function UIManager:ReleaseAndUnloadTextureByName(arg0) end

--- TODO docs
--- @param arg0 unknown
function UIManager:ReleaseTextureByName(arg0) end

--- TODO docs, breaks UI if called, each consecutive call breaks UI in another way
function UIManager:RequestReloadUI() end

--- TODO docs
function UIManager:RequestReprocessAnchoring() end

--- Changes control bounds visibility. (Useful for debugging?)
--- If true control bounds are being drawn.
--- See [screenshot](./docs/screenshots/UIManager_SetDrawControlBounds.png)
--- @param areControlBoundsVisible boolean
function UIManager:SetDrawControlBounds(areControlBoundsVisible) end

--- Changes world anchors visibility.
--- If true hides yields, resource icons and etc.
--- @param areWorldAnchorsHidden boolean
function UIManager:SetHideWorldAnchors(areWorldAnchorsHidden) end
