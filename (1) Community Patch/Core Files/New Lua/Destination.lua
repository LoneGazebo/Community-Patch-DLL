print("This is the 'UI - Destination' mod script.")

include("FLuaVector")

local pathBorderStyle = "MovementRangeBorder"

local bClearPathStyle = false

local tHighlightedPathHexes = {}

local NavyBlue = Vector4(0.0, 0.0, 0.502, 1.0)
local White = Vector4(1.0, 1.0, 1.0, 1.0)

local function ClearPathColourHighlights()
  for _, hex in ipairs(tHighlightedPathHexes) do
    Events.SerialEventHexHighlight(hex, false)
  end
  tHighlightedPathHexes = {}
end

function OnUnitSelectionCleared()
  if bClearPathStyle then
    Events.ClearHexHighlightStyle(pathBorderStyle)
    bClearPathStyle = false
  end

  ClearPathColourHighlights()
end
Events.UnitSelectionCleared.Add(OnUnitSelectionCleared)

--- @param tPath PathNode[]
Events.UIPathFinderUpdate.Add(function(tPath)
  ClearPathColourHighlights()

  local prevNode
  for i, node in ipairs(tPath) do
    if i > 1 then
      if Map.PlotDistance(prevNode.x, prevNode.y, node.x, node.y) > 1 then
        local pHeadUnit = UI.GetHeadSelectedUnit();
        local color = pHeadUnit and pHeadUnit:GetDomainType() == DomainTypes.DOMAIN_SEA and NavyBlue or White
        local hexPrev = ToHexFromGrid({x = prevNode.x, y = prevNode.y})
        local hexNode = ToHexFromGrid({x = node.x, y = node.y})
        Events.SerialEventHexHighlight(hexPrev, true, color)
        Events.SerialEventHexHighlight(hexNode, true, color)
        tHighlightedPathHexes[#tHighlightedPathHexes + 1] = hexPrev
        tHighlightedPathHexes[#tHighlightedPathHexes + 1] = hexNode
      end
    end
    prevNode = node
  end
end)

-------------------------------------------------
Events.UnitSelectionChanged.Add(function(iPlayerID, iUnitID, i, j, k, isSelected)
  if isSelected then
    local pUnit = Players[iPlayerID]:GetUnitByID(iUnitID)
    if not pUnit.GetActivePath then
      local pMissionPlot = pUnit:LastMissionPlot()
      if pUnit:GetX() ~= pMissionPlot:GetX() or pUnit:GetY() ~= pMissionPlot:GetY() then
        local hex = ToHexFromGrid(Vector2(pMissionPlot:GetX(), pMissionPlot:GetY()));
        Events.SerialEventHexHighlight(hex, true, nil, pathBorderStyle)
        bClearPathStyle = true
        Events.GameplayFX(hex.x, hex.y, -1);
      end
    end
  end
end)

-------------------------------------------------
Events.DisplayMovementIndicator.Add(function(bShow)
  if not bShow then
    ClearPathColourHighlights()
  end
end)
