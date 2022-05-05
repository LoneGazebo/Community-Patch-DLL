print("This is the 'UI - Destination' mod script.")

include("FLuaVector")

local pathBorderStyle = "MovementRangeBorder"

local bClearPathColour = false
local bClearPathStyle = false

local colours = { Green   = Vector4(0.0, 1.0, 0.0, 1.0),
                  Cyan    = Vector4(0.0, 1.0, 1.0, 1.0)}


local pathColours = {colours.Green, colours.Cyan}

function OnUnitSelectionChange(iPlayerID, iUnitID, i, j, k, isSelected)
  if (isSelected) then
    local pUnit = Players[iPlayerID]:GetUnitByID(iUnitID)
    local pMissionPlot = pUnit:LastMissionPlot()

    if (pUnit:GetX() ~= pMissionPlot:GetX() or pUnit:GetY() ~= pMissionPlot:GetY()) then
	  if (pUnit.GetActivePath) then
	    local path = pUnit:GetWaypointPath()

		for i = 2, #path, 1 do 		-- The first node in the path is our current location
			local node = path[i] 
			local nodeTurn = node.Turn
			if node.RemainingMovement == 0 then
				nodeTurn = nodeTurn - 1
			end
			Events.SerialEventHexHighlight(ToHexFromGrid({x = node.X, y = node.Y}), true, pathColours[math.fmod(nodeTurn, 2) + 1])
			bClearPathColour = true
		end
	  else
	    local hex = ToHexFromGrid(Vector2(pMissionPlot:GetX(), pMissionPlot:GetY()));
        Events.SerialEventHexHighlight(hex, true, nil, pathBorderStyle)
        bClearPathStyle = true
		Events.GameplayFX(hex.x, hex.y, -1);
	  end
    end
  end
end
Events.UnitSelectionChanged.Add(OnUnitSelectionChange)

function OnUnitSelectionCleared()
  if (bClearPathStyle) then
    Events.ClearHexHighlightStyle(pathBorderStyle)
    bClearPathStyle = false
  end
  
  if (bClearPathColour) then
    Events.ClearHexHighlights()
    bClearPathColour = false
  end
end
Events.UnitSelectionCleared.Add(OnUnitSelectionCleared)
