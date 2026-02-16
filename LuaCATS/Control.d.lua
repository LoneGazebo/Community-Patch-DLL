--- @meta

--- @class Control
--- @field [string] function
Control = {}

--- @class Context: Control

--- @type Context
ContextPtr = {}

--- TODO docs
--- @param instanceName string
--- @param controlTable table<string, Control>
--- @param parentControl Control
--- @return Control # Returns
function ContextPtr:BuildInstanceForControl(instanceName, controlTable, parentControl) end

--- TODO docs
--- @param instanceName string
--- @param controlTable table<string, Control>
function ContextPtr:BuildInstance(instanceName, controlTable) end

--- @type table<string, Control>
Controls = {}

--- @param eMouse MouseType
--- @param Callback fun(nVoid1: number, nVoid2: number)
function Control:RegisterCallback(eMouse, Callback) end

--- @param nVoid1 number
function Control:SetVoid1(nVoid1) end

--- @param nVoid2 number
function Control:SetVoid2(nVoid2) end

--- Set both Void1 and Void2 properties
--- @param nVoid1 number
--- @param nVoid2 number?
function Control:SetVoids(nVoid1, nVoid2) end

--- @param strTextureFileName string
function Control:SetTexture(strTextureFileName) end

--- Set texture offset (relative to the texture sheet) using vector of coordinates<br>
--- Positive coordinates mean right and down respectively<br>
--- @see Control.SetTextureOffsetVal
--- @param vOffset Vector2
function Control:SetTextureOffset(vOffset) end

--- Set texture offset (relative to the texture sheet) using X/Y coordinates<br>
--- Positive coordinates mean right and down respectively<br>
--- @see Control.SetTextureOffset
--- @param iX integer
--- @param iY integer
function Control:SetTextureOffsetVal(iX, iY) end

--- Set this control's offset using vector of coordinates<br>
--- Positive coordinates mean right and down respectively<br>
--- @see Control.SetOffsetVal
--- @param vOffset Vector2
function Control:SetOffset(vOffset) end

--- Set this control's offset using X/Y coordinates<br>
--- Positive coordinates mean right and down respectively<br>
--- @see Control.SetOffset
--- @param iX integer
--- @param iY integer
function Control:SetOffsetVal(iX, iY) end

--- @param iX integer
function Control:SetOffsetX(iX) end

--- @param iY integer
function Control:SetOffsetY(iY) end

--- All parameters should be in the range of 0-1. Cannot be used to set Color0, Color1, and Color2.<br>
--- @see Control.SetColor
--- @see Control.SetColorByName
--- @param r number
--- @param g number
--- @param b number
--- @param a number
function Control:SetColorVal(r, g, b, a) end

--- @see Control.SetColorVal
--- @see Control.SetColorByName
--- @param vColor Vector4 # The RGBA values of the color, in the range of 0-1
--- @param iLayer nil # Set Color if nil
--- |0 # Set Color0
--- |1 # Set Color1
--- |2 # Set Color2
function Control:SetColor(vColor, iLayer) end

--- @see Control.SetColorVal
--- @param strColor string # The name of the color or color set as defined in ColorAtlas.xml or the Colors table
function Control:SetColorByName(strColor) end

--- @param nAlpha number
function Control:SetAlpha(nAlpha) end

--- @param text string | number Seems to have an implicit tostring if a number is passed in
function Control:SetText(text) end

--- Equivalent to `Control:SetText(Locale.Lookup(strTextKey, ...))`
--- @param strTextKey string
--- @param ... string | number
function Control:LocalizeAndSetText(strTextKey, ...) end

--- @param strTooltip string
function Control:SetToolTipString(strTooltip) end

--- Equivalent to `Control:SetToolTipString(Locale.Lookup(strTextKey, ...))`
--- @param strTextKey string
--- @param ... string | number
function Control:LocalizeAndSetToolTip(strTextKey, ...) end

--- @return Control label # A text label at the same coordinates of the (topleft) of this control
function Control:GetTextControl() end

--- @return Vector2
function Control:GetSize() end

--- @param bHide boolean Whether the control should be hidden
--- @see CPK.UI.Control.Hide
--- @see CPK.UI.Control.Show
function Control:SetHide(bHide) end

--- @return boolean
function Control:IsHidden() end
