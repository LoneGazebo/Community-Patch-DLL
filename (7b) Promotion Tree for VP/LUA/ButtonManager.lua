--
-- Button Manager functions
--

local m_ButtonManager = nil
local iButtonIconSize = nil

function ButtonManagerInit(sInstanceName, rootControl, parentControl)
  dprint("ButtonManagerInit(%s)", sInstanceName)

  m_ButtonManager = InstanceManager:new(sInstanceName, rootControl, parentControl)

  local button = m_ButtonManager:GetInstance()
  local size = button.ButtonBox:GetSize()
  iButtonIconSize = button.Portrait:GetSizeX()

  ButtonManagerReset()

  return size.x, size.y
end

function ButtonManagerReset()
  dprint("ButtonManagerReset()")

  if (m_ButtonManager ~= nil) then
    m_ButtonManager:ResetInstances()
  end
end

function ButtonManagerGetButton(iX, iY, sToolTip, sAtlas, iAtlasIndex, sReqTech)
  local button = m_ButtonManager:GetInstance()
  button.Button:SetOffsetVal(iX, iY)
  button.Button:SetToolTipString(sToolTip)

  button.PortraitFrame:SetHide(IconHookup(iAtlasIndex, iButtonIconSize, sAtlas, button.Portrait) ~= true)

  if (sReqTech) then
    button.ReqTech:SetHide(false)
    button.ReqTech:SetToolTipString(Locale.ConvertTextKey(GameInfo.Technologies[sReqTech].Description))
  else
    button.ReqTech:SetHide(true)
  end

  return button
end