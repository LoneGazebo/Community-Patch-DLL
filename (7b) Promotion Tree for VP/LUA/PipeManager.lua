--
-- Pipe Manager functions
--
--
-- Include the following in the xml context
--
--	<Instance Name="PipeInstance" >
--		<Box Anchor="L,T" Size="32,32" Color="0,0,0,0" ID="PipeBox">
--			<Image Anchor="L,T" Color="White,255" Texture="TechBranches.dds" Size="32,32" TextureOffset="0,0" ID="PipeIcon"/>
--		</Box>
--	</Instance>
--
-- then in the associated lua file
--
--  include("PipeManager")
--  PipeManagerInit(iPipeSize, "PipeInstance", "PipeBox", Controls.ScrollPanel)
--    iPipeSize is 16, 24, 32 or 42
--      16, 24 and 32 give pipes centered in a square box
--      42 is special and draws the 32x42 rectangular pipes as used by the Tech Tree code
--
--  PipeManagerDrawHorizontalPipe(iX, iY, iLength, colour)
--	PipeManagerDrawVerticalPipe(iX, iY, iHeight, colour)
--  PipeManagerDrawQuadrantPipe(iX, iY, sType, colour)
--    sType is "top-left", "top-right", "bottom-left" or "bottom-right"
--    colour is omitted (ie nil), "red", "blue", "green", "white", "black", "clear" or Color(r,g,b,a) (default is "clear")
--
--  PipeManagerReset()
--
-- Note: PipeManagerGetPipe(iX, iY, colour) is not intended for general use
--

include("FLuaVector")

local m_PipeColours = {
  red   = Color(255,   0,   0, 255),
  green = Color(  0, 255,   0, 255),
  blue  = Color(  0,   0, 255, 255),
  white = Color(255, 255, 255, 255),
  black = Color(  0,   0,   0, 255),
  clear = Color(  0,   0,   0,   0),
}

local m_PipeSize = 32
local m_PipeSizes = {}
      m_PipeSizes[16] = {x=16, y=16}
      m_PipeSizes[24] = {x=24, y=24}
      m_PipeSizes[32] = {x=32, y=32}
      m_PipeSizes[42] = {x=32, y=42}
local m_PipeOffsets = {}
      m_PipeOffsets[16] = {h={x=0, y=3},  v={x=3, y=0},  tl={x=3, y=3},   tr={x=87, y=3},  bl={x=3, y=90},  br={x=87, y=90}}
      m_PipeOffsets[24] = {h={x=0, y=-1}, v={x=-1, y=0}, tl={x=-1, y=-1}, tr={x=83, y=-1}, bl={x=-1, y=86}, br={x=83, y=86}}
      m_PipeOffsets[32] = {h={x=0, y=-5}, v={x=-5, y=0}, tl={x=-5, y=-5}, tr={x=79, y=-5}, bl={x=-5, y=82}, br={x=79, y=82}}
      m_PipeOffsets[42] = {h={x=0, y=0},  v={x=0, y=0},  tl={x=0, y=0},   tr={x=72, y=0},  bl={x=0, y=72},  br={x=72, y=72}}

local m_PipeManager = nil

function PipeManagerInit(iSize, sInstanceName, rootControl, parentControl)
  dprint("PipeManagerInit(%d, %s)", iSize, sInstanceName)

  if (m_PipeOffsets[iSize] ~= nil) then
    m_PipeSize = iSize
  else
    m_PipeSize = 32
  end

  m_PipeManager = InstanceManager:new(sInstanceName, rootControl, parentControl)
end

function PipeManagerReset()
  dprint("PipeManagerReset()")

  if (m_PipeManager ~= nil) then
    m_PipeManager:ResetInstances()
  end
end

function PipeManagerGetPipe(iX, iY, colour)
  local pipe = m_PipeManager:GetInstance()
  pipe.PipeBox:SetOffsetVal(iX, iY)

  if (type(colour) == "string") then
    colour = m_PipeColours[colour]
  end

  if (type(colour) == "table") then
	pipe.PipeBox:SetColor(colour)
  else
	pipe.PipeBox:SetColor(Color(0,0,0,0))
  end

  return pipe
end

function PipeManagerDrawHorizontalPipe(iX, iY, iLength, colour)
  dprint("  draw horizontal pipe at (%i, %i) of length %i", iX, iY, iLength)
  if (iLength > 0) then
    local pipe = PipeManagerGetPipe(iX, iY, colour)
    pipe.PipeBox:SetSize({x=iLength, y=m_PipeSizes[m_PipeSize].y})
    pipe.PipeIcon:SetSize({x=iLength, y=32})

	pipe.PipeIcon:SetTexture("TechBranchH.dds")
	pipe.PipeIcon:SetTextureOffset(m_PipeOffsets[m_PipeSize].h)
  end
end

function PipeManagerDrawVerticalPipe(iX, iY, iHeight, colour)
  dprint("  draw vertical pipe at (%i, %i) of height %i", iX, iY, iHeight)
  if (iHeight > 0) then
    local pipe = PipeManagerGetPipe(iX, iY, colour)
    pipe.PipeBox:SetSize({x=m_PipeSizes[m_PipeSize].x, y=iHeight})
    pipe.PipeIcon:SetSize({x=32, y=iHeight})

	pipe.PipeIcon:SetTexture("TechBranchV.dds")
	pipe.PipeIcon:SetTextureOffset(m_PipeOffsets[m_PipeSize].v)
  end
end

function PipeManagerDrawQuadrantPipe(iX, iY, sType, colour)
  dprint("  draw %s quadrant pipe at (%i, %i)", sType, iX, iY)
  local pipe = PipeManagerGetPipe(iX, iY, colour)
  pipe.PipeBox:SetSize(m_PipeSizes[m_PipeSize])
  pipe.PipeIcon:SetSize({x=32, y=32})

  pipe.PipeIcon:SetTexture("TechBranches.dds")

  if (sType == 'top-right') then
    pipe.PipeIcon:SetTextureOffset(m_PipeOffsets[m_PipeSize].tr)
  elseif (sType == 'bottom-right') then
    pipe.PipeIcon:SetTextureOffset(m_PipeOffsets[m_PipeSize].br)
  elseif (sType == 'top-left') then
    pipe.PipeIcon:SetTextureOffset(m_PipeOffsets[m_PipeSize].tl)
  elseif (sType == 'bottom-left') then
    pipe.PipeIcon:SetTextureOffset(m_PipeOffsets[m_PipeSize].bl)
  end
end