--
-- Control Utility functions
--

function SetWidth(control, iWidth)
  local size = control:GetSize()
  size.x = iWidth
  control:SetSize(size)
end

function SetHeight(control, iHeight)
  local size = control:GetSize()
  size.y = iHeight
  control:SetSize(size)
end

function OffsetAgain(control)
  local offsetX, offsetY = control:GetOffsetVal()
  control:SetOffsetVal(offsetX+10, offsetY+10)
  control:SetOffsetVal(offsetX, offsetY)
end