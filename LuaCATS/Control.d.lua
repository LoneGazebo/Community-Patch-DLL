--- @meta

--- @class Control
--- @field [string] function
Control = {}

--- @class Context: Control

--- @type Context
ContextPtr = {}

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

--- @param nAlpha number
function Control:SetAlpha(nAlpha) end

--- @param strText string
function Control:SetText(strText) end

--- @param strTooltip string
function Control:SetTooltipString(strTooltip) end

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
