--
-- To display the Promotion Tree call either
--   LuaEvents.PromotionTreeDisplay(iUnit)
-- to display the tree for a specific unit, or
--   LuaEvents.PromotionTreeDisplay()
-- to display the tree with the Combat Class drop-down menu
--
-- For example, for a button on the UnitPanel screen
--   function OnPromotionTreeButton(iUnit)
--     LuaEvents.PromotionTreeDisplay(iUnit)
--   end
--   Controls.PromotionTreeButton:SetVoid1(UI.GetHeadSelectedUnit():GetID())
--   Controls.PromotionTreeButton:RegisterCallback(Mouse.eLClick, OnPromotionTreeButton)
--
-- For example, for a tab in the Additional Information list
--   function OnPromotionTreeView()
--     LuaEvents.PromotionTreeDisplay()
--   end
--   instance.Tab:RegisterCallback(Mouse.eLClick, OnPromotionTreeView)
--

print("This is the 'UI - Promotion Tree' mod script.")

include("IconSupport")
include("InstanceManager")
include("InfoTooltipInclude")

bDebug = false
ClassMode = false

include("DebugPrint")
include("ControlUtils")
include("ButtonManager")
include("PipeManager")
include("PromotionUtils")

local config = {
  normal  = {NAME="Normal",  PANEL=900, GAP=44, PIPE=16, BUTTON="ButtonInstance"},
--normal = {NAME="Normal", PANEL=740, GAP=50, PIPE=32, BUTTON="ButtonInstance"},   -- UndeadDevel: not tested by me!
  small  = {NAME="Small",  PANEL=768, GAP=44, PIPE=16, BUTTON="ButtonInstanceSmall"},
--  small  = {NAME="Small",  PANEL=840, GAP=44, PIPE=16, BUTTON="ButtonInstanceSmall"}   -- UndeadDevel: not tested by me!
}

-- These are set by Resize()
local iHeight = nil
local iGapY = nil
local iCentreLine = nil

local iLeftMargin = 20
local iTopMargin = 20

local iDropDownSizeX = 230
local iDropDownSizeY = 60
local iDropDownIconSize = 32
local iDefaultCombatClass = 0

local iUnitBoxSizeX = 230
local iUnitBoxSizeY = 60
local iUnitIconSize = 64

-- These will be determined by the ButtonManager
local iButtonSizeX = nil
local iButtonSizeY = nil

-- These are set by Resize()
local iPipeSizeX = nil
local iPipeSizeY = nil


local iSelectedUnit = nil
local bTreeVisible = false

-- UndeadDevel: we need to store this to check if we can extend the 3rd and 4th promo line
local line3DependentPromos = {}
-- UndeadDevel: coordinates for the dependent promo lines of line 3 and 4
local PipeHorizYUpper = 0
local PipeHorizYLower = 0
local PipeHorizXUpper = 0
local PipeHorizXLower = 0
-- UndeadDevel: need this to keep track of UI size for a small graphical fix
local largeUI = false
-- UndeadDevel: need this to keep track of tree size
local numPromosHoriz = 0
local numPromosLine3Base = 0
local numPromosLine4Base = 0

--
-- PlaceAbc(...) functions
--
-- Calculate locations of logical drawing elements in the tree
--

function PlaceDropDown()
  DrawPromotionDropDown(iLeftMargin, iCentreLine - (iDropDownSizeY / 2), sDefaultCombatClass)

  AdjustClassGroupWidth(iDropDownSizeX + iPipeSizeX/2)

  OnSelectCombatClass(iDefaultCombatClass)
end

function PlaceUnit(pUnit)
  local iUnitBoxX = iLeftMargin
  local iUnitBoxY = iCentreLine - (iUnitBoxSizeY / 2)
  DrawUnitBox(iUnitBoxX, iUnitBoxY, pUnit)

  AdjustClassGroupWidth(iUnitBoxSizeX + iPipeSizeX/2)

  -- If we have the DLL mod for promotion classes enabled, use them
  local sDisplayClass = nil
  local unitType = GameInfo.Units[pUnit:GetUnitType()]

  if unitType.PromotionClass then
    sDisplayClass =  unitType.PromotionClass
  elseif  pUnit:GetUnitCombatType() >= 0 then
    sDisplayClass = GameInfo.UnitCombatInfos[pUnit:GetUnitCombatType()].Type
  else
    print(string.format("Unit %i (%s) isn't eligible for any promotions", pUnit:GetID(), unitType.Type))
  end

-- need to convert the potentially "false" UnitCombat to our correct one using the specific unit type
-- in particular we need to check if we have a "Gunpowder unit" that is really an AA unit and
-- if we have an Archery unit, which could either be a helicopter or a mounted archer (or a regular one)
-- if the unit is a gunpowder unit, we should query if it has a relevant AirInterceptRange
-- if the unit is an archery unit, we should first query if it has the type UNIT_HELICOPTER_GUNSHIP and if not, if it has attack range 1
  if (sDisplayClass == "UNITCOMBAT_GUN" and pUnit:GetAirInterceptRange() > 0) then
    sDisplayClass = "UNITCOMBAT_AA"
  elseif (sDisplayClass == "UNITCOMBAT_ARCHER") then
    if (pUnit:GetUnitType() == GameInfoTypes.UNIT_HELICOPTER_GUNSHIP) then
      sDisplayClass = "UNITCOMBAT_HELICOPTER"
    elseif (pUnit:IsMounted()) then
      sDisplayClass = "UNITCOMBAT_MOUNTED_ARCHER"
    end
  end
  PlacePromotions(pUnit, sDisplayClass, iUnitBoxX + iUnitBoxSizeX, iCentreLine)
end

function PlacePromotions(pUnit, sCombatClass, iBaseX, iBaseY)
-- if we are here with the non-specific path, then so far we have the correct (as far as this UI is concerned) combat class from our own XML
-- if we are here from PlaceUnit() we have converted the CombatClass if it wasn't the correct one
  ButtonManagerReset()
  PipeManagerReset()
  
  line3DependentPromos = {}
  PipeHorizYUpper = 0
  PipeHorizYLower = 0
  PipeHorizXUpper = 0
  PipeHorizXLower = 0
  numPromosHoriz = 0  
  numPromosLine3Base = 0
  numPromosLine4Base = 0  

  dprint("   PlacePromotions() called with sCombatClass %s", sCombatClass)

  local iWidth = iButtonSizeX + 3*iPipeSizeX
  AdjustBaseGroupWidth(iWidth)

  basePromotions = GetBasePromotions(sCombatClass)

  if (basePromotions and #basePromotions > 0) then
    local iPipeHorizX = iBaseX
    local iPipeHorizY = iBaseY - (iPipeSizeY / 2)
    PipeManagerDrawHorizontalPipe(iPipeHorizX, iPipeHorizY, iPipeSizeX)

    PlaceBasePromotions(pUnit, sCombatClass, basePromotions[1], iPipeHorizX + iPipeSizeX, iBaseY, -1)

    if (#basePromotions > 1) then
      PlaceBasePromotions(pUnit, sCombatClass, basePromotions[2], iPipeHorizX + iPipeSizeX, iBaseY, 1)
    end
    if (#basePromotions > 3) then -- this only supports two (additional) non-branching promo lines, which can, however, intertwine after their base promos (e.g. Helicopter specials)
      PlaceThirdAndFourthPromotionLine(pUnit, sCombatClass, basePromotions[3], iPipeHorizX + iPipeSizeX, iBaseY, -1)
      PlaceThirdAndFourthPromotionLine(pUnit, sCombatClass, basePromotions[4], iPipeHorizX + iPipeSizeX, iBaseY, 1)
    elseif (#basePromotions > 2) then -- this only supports one (additional) straight, non-branching promo line, which can have up to two dependent promo lines (starting at the last base promo)
      PlaceThirdPromotionLine(pUnit, sCombatClass, basePromotions[3], iPipeHorizX + iPipeSizeX, iBaseY)
    end
  end

  Controls.GroupsStack:CalculateSize()
  Controls.GroupsStack:ReprocessAnchoring()
  Controls.ScrollPanel:CalculateInternalSize()
end

function PlaceThirdPromotionLine(pUnit, sCombatClass, basePromotions, iBaseX, iBaseY)
  local iPipeHorizX = 0
  local iPipeHorizY = 0

  for iPromotion = 1, #basePromotions, 1 do
    local sPromotion = basePromotions[iPromotion]
    local iButtonX = iBaseX + 2*iPipeSizeX + ((iPromotion - 1) * (2*iPipeSizeX + iButtonSizeX + iPipeSizeX))
    local iButtonY = iBaseY - (iButtonSizeY / 2)
    DrawPromotionButton(iButtonX, iButtonY, pUnit, sPromotion, sCombatClass)

    iPipeHorizX = iButtonX + iButtonSizeX
    iPipeHorizY = iButtonY + ((iButtonSizeY - iPipeSizeY) / 2)
    PipeManagerDrawHorizontalPipe(iButtonX - 3*iPipeSizeX, iPipeHorizY, 3*iPipeSizeX)
  end
  
  if (#basePromotions > numPromosHoriz) then
    numPromosHoriz = #basePromotions
  end
  local ThirdLineDependentPromos = GetDependentPromotions(sCombatClass, basePromotions[#basePromotions])
  if (#ThirdLineDependentPromos > 0) then
    PlaceThirdPromotionDependents(pUnit, sCombatClass, ThirdLineDependentPromos, iPipeHorizX, iPipeHorizY)
    for iPromoLine = 1, #ThirdLineDependentPromos, 1 do
      if (#basePromotions + #ThirdLineDependentPromos[iPromoLine] > numPromosHoriz) then
        numPromosHoriz = #basePromotions + #ThirdLineDependentPromos[iPromoLine]
      end
    end
  end
  local iWidth = math.max(Controls.BaseGroup:GetSize().x, (numPromosHoriz * (iButtonSizeX + 3*iPipeSizeX)))
  AdjustBaseGroupWidth(iWidth)
end

function PlaceThirdPromotionDependents(pUnit, sCombatClass, basePromotions, iBaseX, iBaseY)
  if (#basePromotions == 1) then
    PlaceStraightPromoLine(pUnit, sCombatClass, basePromotions[1], iBaseX, iBaseY, false)
  elseif (#basePromotions >= 2) then
    PipeManagerDrawHorizontalPipe(iBaseX, iBaseY, iPipeSizeX)
    DrawBottomPipe(iBaseX + iPipeSizeX, iBaseY, -1)
    DrawBottomPipe(iBaseX + iPipeSizeX, iBaseY, 1)
    if (largeUI) then
      PipeManagerDrawVerticalPipe(iBaseX + iPipeSizeX, iBaseY - 0.81*iPipeSizeY, 0.8*iPipeSizeY)   -- nasty patchwork...unfortunately necessary because of how bad the pipe elements are suited
      PipeManagerDrawVerticalPipe(iBaseX + iPipeSizeX, iBaseY + 1.9*iPipeSizeY, 0.65*iPipeSizeY)   -- for forking tightly
    end
    PlaceTwinPromoLine(pUnit, sCombatClass, basePromotions[1], iBaseX + iPipeSizeX, iBaseY + (iPipeSizeY / 2), -1)
    PlaceTwinPromoLine(pUnit, sCombatClass, basePromotions[2], iBaseX + iPipeSizeX, iBaseY + (iPipeSizeY / 2), 1)
  end
  if (#basePromotions > 2) then
    print("Cannot place more than 2 dependent promotion lines for the third base line!")
  end
end

function PlaceThirdAndFourthPromotionLine(pUnit, sCombatClass, basePromotions, iBaseX, iBaseY, iDirection)
  PlaceTwinPromoLine(pUnit, sCombatClass, basePromotions, iBaseX, iBaseY, iDirection)

  if (iDirection == -1) then  -- first of two executions of this function
    numPromosLine3Base = #basePromotions
    line3DependentPromos = GetDependentPromotions(sCombatClass, basePromotions[#basePromotions])
    if (#basePromotions > numPromosHoriz) then
      numPromosHoriz = #basePromotions
    end
  else
    numPromosLine4Base = #basePromotions
    if (#basePromotions > numPromosHoriz) then
      numPromosHoriz = #basePromotions
    end
    local line4DependentPromos = GetDependentPromotions(sCombatClass, basePromotions[#basePromotions])
    if (#line3DependentPromos > 0 or #line4DependentPromos > 0) then
    
      local crossConnections = PlaceMidLineDependencies(pUnit, sCombatClass, line3DependentPromos, line4DependentPromos, PipeHorizXUpper, PipeHorizXLower, PipeHorizYUpper, PipeHorizYLower, numPromosLine3Base, numPromosLine4Base)
      local promoLineComparator = 0
      
      if (crossConnections and numPromosLine4Base > numPromosLine3Base) then
        promoLineComparator = numPromosLine4Base - numPromosLine3Base
      end
      for iPromoLine = 1, #line3DependentPromos, 1 do
        if (numPromosLine3Base + #line3DependentPromos[iPromoLine] + promoLineComparator > numPromosHoriz) then
          numPromosHoriz = numPromosLine3Base + #line3DependentPromos[iPromoLine] + promoLineComparator
        end
      end
      promoLineComparator = 0
      if (crossConnections and numPromosLine3Base > numPromosLine4Base) then
        promoLineComparator = numPromosLine3Base - numPromosLine4Base
      end
      for iPromoLine = 1, #line4DependentPromos, 1 do
        if (#basePromotions + #line4DependentPromos[iPromoLine] + promoLineComparator > numPromosHoriz) then
          numPromosHoriz = #basePromotions + #line4DependentPromos[iPromoLine] + promoLineComparator
        end
      end
    end
  end
  local iWidth = math.max(Controls.BaseGroup:GetSize().x, (numPromosHoriz * (iButtonSizeX + 3*iPipeSizeX)))
  AdjustBaseGroupWidth(iWidth)  
end

-- returns whether there have been cross-connections found or not
function PlaceMidLineDependencies(pUnit, sCombatClass, line3Chains, line4Chains, iBaseXUpper, iBaseXLower, iBaseYUpper, iBaseYLower, numPromosThirdLine, numPromosFourthLine)
-- scenarios (numDependentPromoLines) 1, 1; 2, 0; 0, 2; 2, 1 (same as one of the two); 1 (same as one of the two), 2; 2, 2 (same)...everything else is incompatible
  local noMoreSpace = false
  local connectUP = false
  local lineLengthDifference = numPromosThirdLine - numPromosFourthLine
  local distanceStandard = iButtonSizeX + 3*iPipeSizeX
  
  local crossConnections = 0          -- doesn't store the number of cross-connections but the type (upper, lower); if it's == 1, then there's only 1, though
  if (#line4Chains ~= 0 and #line3Chains ~= 0 and #line3Chains <= 2) then  -- check for commonalities
    if (line3Chains[1][1] == line4Chains[1][1] or (not (#line4Chains < 2) and (line3Chains[1][1] == line4Chains[2][1]))) then
      crossConnections = 1
      connectUP = true          -- if this is true AND crossConnections == 2, then there are two cross-connections
    end
    if (#line3Chains == 2 and (line3Chains[2][1] == line4Chains[1][1] or (not (#line4Chains < 2) and (line3Chains[2][1] == line4Chains[2][1])))) then
      crossConnections = 2
    end
  end

  local lLDDistance = 0         -- lineLengthDifferenceDistance: the concrete form of the abstract lineLengthDifference
  if (#line3Chains <= 2) then
    if (#line3Chains == 2) then
      if (lineLengthDifference < 0) then
        lLDDistance = lineLengthDifference * -1 * distanceStandard
      end
      PlaceStraightPromoLine(pUnit, sCombatClass, line3Chains[1], iBaseXUpper + lLDDistance, iBaseYUpper, false)
      DrawTopPipe(iBaseXUpper + iPipeSizeX + lLDDistance, iBaseYLower, 1)
      DrawBottomPipe(iBaseXUpper + iPipeSizeX + lLDDistance, iBaseYUpper, 1)
      local iPipeVertY, iPipeVertLen
      iPipeVertY = iBaseYUpper + iPipeSizeY
      iPipeVertLen = iBaseYLower - iBaseYUpper - iPipeSizeY
      PipeManagerDrawVerticalPipe(iBaseXUpper + iPipeSizeX + lLDDistance, iPipeVertY, iPipeVertLen)
      PlaceStraightPromoLine(pUnit, sCombatClass, line3Chains[2], iBaseXUpper + lLDDistance, iBaseYLower, true)
      if (lLDDistance > 0) then
        PipeManagerDrawHorizontalPipe(iBaseXUpper, iBaseYUpper, lLDDistance)
      end
      noMoreSpace = true
    elseif (#line3Chains == 1) then
      if (lineLengthDifference < 0 and crossConnections ~= 0) then
        lLDDistance = lineLengthDifference * -1 * distanceStandard
        PipeManagerDrawHorizontalPipe(iBaseXUpper, iBaseYUpper, lLDDistance)
      end
      PlaceStraightPromoLine(pUnit, sCombatClass, line3Chains[1], iBaseXUpper + lLDDistance, iBaseYUpper, false)
    end
  else
    print("The third and fourth promotion lines of the Promotion Tree cannot handle any branching beyond the height of their 2 rows.")
  end

  if (#line4Chains == 0) then return crossConnections ~= 0 end
  
  if (lineLengthDifference > 0) then
    lLDDistance = lineLengthDifference * distanceStandard
  else lLDDistance = 0 end
  
  if (connectUP) then
    local iPipeVertY, iPipeVertLen
    iPipeVertY = iBaseYUpper + iPipeSizeY
    iPipeVertLen = iBaseYLower - iBaseYUpper - iPipeSizeY
    PipeManagerDrawVerticalPipe(iBaseXLower + iPipeSizeX + lLDDistance, iPipeVertY, iPipeVertLen)  
    DrawTopPipe(iBaseXLower + iPipeSizeX + lLDDistance, iBaseYUpper, -1)
    DrawBottomPipe(iBaseXLower + iPipeSizeX + lLDDistance, iBaseYLower, -1)
    PipeManagerDrawHorizontalPipe(iBaseXLower, iBaseYLower, iPipeSizeX + lLDDistance)
  end
  if (crossConnections == 2) then
    PipeManagerDrawHorizontalPipe(iBaseXLower, iBaseYLower, 3*iPipeSizeX + lLDDistance)
  end
  
  if (#line4Chains <= 2) then
    if ((crossConnections == 2 and connectUP and #line4Chains == 2) or ((crossConnections == 1 or crossConnections == 2 and not connectUP) and #line4Chains == 1)) then return true end
    if (noMoreSpace) then
      print("There was only enough space to draw the 3rd promotion line dependents.")
      return true
    elseif (crossConnections == 1) then  -- line 4 has 2 dependent promo lines, only one of which is intertwined with the one promo line of line 3
      if (line3Chains[1][1] == line4Chains[1][1]) then
        PlaceStraightPromoLine(pUnit, sCombatClass, line4Chains[2], iBaseXLower + lLDDistance, iBaseYLower, false)
      elseif (line3Chains[1][1] == line4Chains[2][1]) then
        PlaceStraightPromoLine(pUnit, sCombatClass, line4Chains[1], iBaseXLower + lLDDistance, iBaseYLower, false)
      end
    elseif (#line4Chains == 1) then -- no cross-connections
      PlaceStraightPromoLine(pUnit, sCombatClass, line4Chains[1], iBaseXLower, iBaseYLower, false)
    elseif (#line3Chains == 1) then  -- line 4 has 2 midline, dependent promo lines and line 3 has 1, but they are not intertwined, so the second dependent line of line 4 needs to be dropped
      print(string.format("Need to drop midline dependent promo line due to lack of space; promo line dropped starts with promotion %s", line4Chains[2][1]))
      PlaceStraightPromoLine(pUnit, sCombatClass, line4Chains[1], iBaseXLower, iBaseYLower, false)
    else  -- line 4 has 2 midline, dependent promo lines and line 3 has none
      PlaceStraightPromoLine(pUnit, sCombatClass, line4Chains[1], iBaseXLower + lLDDistance, iBaseYLower, false)
      DrawTopPipe(iBaseXLower + iPipeSizeX + lLDDistance, iBaseYUpper, -1)
      DrawBottomPipe(iBaseXLower + iPipeSizeX + lLDDistance, iBaseYLower, -1)
      local iPipeVertY, iPipeVertLen
      iPipeVertY = iBaseYUpper + iPipeSizeY
      iPipeVertLen = iBaseYLower - iBaseYUpper - iPipeSizeY
      PipeManagerDrawVerticalPipe(iBaseXLower + iPipeSizeX + lLDDistance, iPipeVertY, iPipeVertLen)
      PlaceStraightPromoLine(pUnit, sCombatClass, line4Chains[2], iBaseXLower + lLDDistance, iBaseYUpper, true)
    end
  else
    print("The third and fourth promotion lines of the Promotion Tree cannot handle any branching beyond the height of their 2 rows combined.")
  end
end

function PlaceStraightPromoLine(pUnit, sCombatClass, basePromotions, iBaseX, iBaseY, noStartingPipe)
  for iPromotion = 1, #basePromotions, 1 do
    local sPromotion = basePromotions[iPromotion]
    local iButtonX = iBaseX + (3*iPipeSizeX) + ((iPromotion - 1) * (3*iPipeSizeX + iButtonSizeX))
    local iButtonY = iBaseY - ((iButtonSizeY - iPipeSizeY) / 2)
    DrawPromotionButton(iButtonX, iButtonY, pUnit, sPromotion, sCombatClass)

    if (iPromotion ~= 1 or not noStartingPipe) then
      PipeManagerDrawHorizontalPipe(iButtonX - 3*iPipeSizeX, iBaseY, 3*iPipeSizeX)
    else
      PipeManagerDrawHorizontalPipe(iButtonX - iPipeSizeX, iBaseY, iPipeSizeX)
    end
  end
end

function PlaceTwinPromoLine(pUnit, sCombatClass, basePromotions, iBaseX, iBaseY, iDirection)
  local iPipeHorizX
  for iPromotion = 1, #basePromotions, 1 do
    local sPromotion = basePromotions[iPromotion]
    local iButtonX = iBaseX + (2*iPipeSizeX) + ((iPromotion-1) * (2*iPipeSizeX + iButtonSizeX + iPipeSizeX))
    local iButtonY = iBaseY + iDirection * (iButtonSizeY / 6)
    if (iDirection == -1) then
      iButtonY = iButtonY - iButtonSizeY
    end
    DrawPromotionButton(iButtonX, iButtonY, pUnit, sPromotion, sCombatClass)

    iPipeHorizX = iButtonX + iButtonSizeX
    local iPipeHorizY = iButtonY + ((iButtonSizeY - iPipeSizeY) / 2)
    if (iDirection == -1) then
      PipeHorizYUpper = iPipeHorizY
    else
      PipeHorizYLower = iPipeHorizY
    end
    if (iPromotion == 1) then
      DrawTopPipe(iButtonX - 2*iPipeSizeX, iPipeHorizY, iDirection)
      PipeManagerDrawHorizontalPipe(iButtonX - iPipeSizeX, iPipeHorizY, iPipeSizeX)
    else
      PipeManagerDrawHorizontalPipe(iButtonX - 3*iPipeSizeX, iPipeHorizY, 3*iPipeSizeX)
    end
  end
  if (iDirection == -1) then
    PipeHorizXUpper = iPipeHorizX       -- needed for the dependent lines of the third and fourth promo line
  else
    PipeHorizXLower = iPipeHorizX
  end
end

function PlaceBasePromotions(pUnit, sCombatClass, basePromotions, iBaseX, iBaseY, iDirection)
  local iGutterY = (iBaseY - (3 * (iButtonSizeY + iGapY))) / 2

  for iPromotion = 1, #basePromotions, 1 do
    local sPromotion = basePromotions[iPromotion]
    local iButtonX = iBaseX + (2*iPipeSizeX) + ((iPromotion-1) * (2*iPipeSizeX + iButtonSizeX + iPipeSizeX))
    local iButtonY = iBaseY + (iDirection * (iGutterY + ((iPromotion-1) * (iButtonSizeY + iGapY))))
    if (iDirection == -1) then
      iButtonY = iButtonY - iButtonSizeY
    end
    DrawPromotionButton(iButtonX, iButtonY, pUnit, sPromotion, sCombatClass)

    local iPipeHorizX = iButtonX + iButtonSizeX
    local iPipeHorizY = iButtonY + ((iButtonSizeY - iPipeSizeY) / 2)

    PipeManagerDrawHorizontalPipe(iButtonX - iPipeSizeX, iPipeHorizY, iPipeSizeX)

    local iPipeVertX = iButtonX - 2*iPipeSizeX
    DrawTopPipe(iPipeVertX, iPipeHorizY, iDirection)

    local iPipeUpY
    if (iPromotion == 1) then
      iPipeUpY = iBaseY - (iPipeSizeY / 2)
    else
      iPipeUpY = iPipeHorizY - (iDirection * (iButtonSizeY + iGapY))
    end
    DrawBottomPipe(iPipeVertX, iPipeUpY, iDirection)

    local iPipeVertY, iPipeVertLen
    if (iDirection == -1) then
      iPipeVertY = iPipeHorizY + iPipeSizeY
      iPipeVertLen = iPipeUpY - iPipeVertY
    else
      iPipeVertY = iPipeUpY + iPipeSizeY
      iPipeVertLen = iPipeHorizY - iPipeVertY
    end
    PipeManagerDrawVerticalPipe(iPipeVertX, iPipeVertY, iPipeVertLen)
    
    if (iPromotion > numPromosHoriz) then
      numPromosHoriz = iPromotion
    end

    local chainedPromotions = GetDependentPromotions(sCombatClass, sPromotion)
    if (#chainedPromotions > 0) then
      local offset = math.min(#basePromotions-iPromotion, 1)
      local lengthHistory = iPromotion + offset
      local iPipeHorizLen = (offset * (2*iPipeSizeX + iButtonSizeX + iPipeSizeX)) + iPipeSizeX
      PipeManagerDrawHorizontalPipe(iPipeHorizX, iPipeHorizY, iPipeHorizLen)

      PlaceDependentPromotions(pUnit, sCombatClass, chainedPromotions, iPipeHorizX + iPipeHorizLen, iButtonY, iDirection, lengthHistory)
    else
      -- There are no dependent promotions, but we need a short length of pipe to join the next base promotion onto
      if (iPromotion < #basePromotions) then
        PipeManagerDrawHorizontalPipe(iPipeHorizX, iPipeHorizY, iPipeSizeX)
      elseif (iPromotion == #basePromotions) then
        local iWidth = math.max(Controls.BaseGroup:GetSize().x, (numPromosHoriz * (iButtonSizeX + 3*iPipeSizeX)))
        AdjustBaseGroupWidth(iWidth)
      end
    end
  end
end

function PlaceDependentPromotions(pUnit, sCombatClass, chainedPromotions, iBaseX, iBaseY, iDirection, lengthHistory)
  local iPipeSpanY = iBaseY + ((iButtonSizeY - iPipeSizeY) / 2)
  local iPipeSpanAboveY = iPipeSpanY + (iDirection * (iButtonSizeY + iGapY))

  local iButtonX = iBaseX + 2*iPipeSizeX
  local iButtonY
  if (iDirection == -1) then
    iButtonY = iPipeSpanAboveY + iPipeSizeY + ((iPipeSpanY - (iPipeSpanAboveY + iPipeSizeY) - iButtonSizeY) / 2)
  else
    iButtonY = iPipeSpanY + iPipeSizeY + ((iPipeSpanAboveY - (iPipeSpanY + iPipeSizeY) - iButtonSizeY) / 2)
  end

  local iPipeJoinY = iButtonY + ((iButtonSizeY - iPipeSizeY) / 2)

  PipeManagerDrawHorizontalPipe(iBaseX - iPipeSizeX, iPipeSpanY, iPipeSizeX)

  local iSpanPromotions = 0
  for i = 1, #chainedPromotions - 1, 1 do
    iSpanPromotions = iSpanPromotions + #chainedPromotions[i]
  end

  if (iSpanPromotions > 0) then
    local iPipeSpanLen = iSpanPromotions * (2*iPipeSizeX + iButtonSizeX + iPipeSizeX)
    PipeManagerDrawHorizontalPipe(iBaseX, iPipeSpanY, iPipeSpanLen)
  end
  lengthHistory = lengthHistory + iSpanPromotions + #chainedPromotions[#chainedPromotions]
  if (lengthHistory > numPromosHoriz) then
    numPromosHoriz = lengthHistory
  end
  local iWidth = math.max(Controls.BaseGroup:GetSize().x, (numPromosHoriz * (iButtonSizeX + 3*iPipeSizeX)))
  AdjustBaseGroupWidth(iWidth)

  for i = 1, #chainedPromotions, 1 do
    for j = 1, #chainedPromotions[i], 1 do
      local sDependentPromotion = chainedPromotions[i][j]
      DrawPromotionButton(iButtonX, iButtonY, pUnit, sDependentPromotion, sCombatClass)

      if (j == 1) then
        local iPipeVertX = iButtonX - 2*iPipeSizeX
        local iPipeVertY, iPipeVertLen
        if (iDirection == -1) then
          iPipeVertY = iPipeJoinY + iPipeSizeY
          iPipeVertLen = iPipeSpanY - iPipeVertY
        else
          iPipeVertY = iPipeSpanY + iPipeSizeY
          iPipeVertLen = iPipeJoinY - iPipeVertY
        end

        PipeManagerDrawHorizontalPipe(iButtonX - iPipeSizeX, iPipeJoinY, iPipeSizeX)
        DrawTopPipe(iPipeVertX, iPipeJoinY, iDirection)
        PipeManagerDrawVerticalPipe(iPipeVertX, iPipeVertY, iPipeVertLen)
        DrawBottomPipe(iPipeVertX, iPipeSpanY, iDirection)
      else
        PipeManagerDrawHorizontalPipe(iButtonX - 3*iPipeSizeX, iPipeJoinY, 3*iPipeSizeX)
      end

      iButtonX = iButtonX + (2*iPipeSizeX + iButtonSizeX + iPipeSizeX)
    end
  end
end


--
-- AdjustAbc(iSize) functions
--

function AdjustPanelHeight(iHeight)
  local iPanelHeight = iHeight + 2*iTopMargin
  local iScrollHeight = iPanelHeight - iTopMargin
  local iGroupHeight = iPanelHeight
  local iDividerHeight = iScrollHeight - iTopMargin
  local iBlockHeight = iScrollHeight

  SetHeight(Controls.TreePanel, iPanelHeight)
  SetHeight(Controls.ScrollPanel, iScrollHeight)

  SetHeight(Controls.ClassGroup, iGroupHeight)
  SetHeight(Controls.ClassDivider, iDividerHeight)

  SetHeight(Controls.BaseGroup, iGroupHeight)
  SetHeight(Controls.BaseDivider, iDividerHeight)
  SetHeight(Controls.BaseBlock, iBlockHeight)

  Controls.DropDownBox:SetOffset({x=iLeftMargin, y=(iCentreLine - iDropDownSizeY/2)})
  Controls.UnitBox:SetOffset({x=iLeftMargin, y=(iCentreLine - iUnitBoxSizeY/2)})
  
  Controls.ScrollPanel:CalculateInternalSize()
  Controls.ScrollBar:SetSizeX(Controls.ScrollPanel:GetSizeX() - iLeftMargin - 15)
end

function AdjustClassGroupWidth(iWidth)
  SetWidth(Controls.ClassGroup, iWidth)
  SetWidth(Controls.ClassBar, iWidth)
end

function AdjustBaseGroupWidth(iWidth)
  SetWidth(Controls.BaseGroup, iWidth)
  SetWidth(Controls.BaseBlock, iWidth)
  SetWidth(Controls.BaseBar, iWidth)
end

--
-- DrawXyz(iX, iY, ...) functions
--
-- Create instance components and configure them
--
-- Note: The colour parameter in the pipe drawing functions is intended only for debugging!
--

function DrawPromotionDropDown(iX, iY)
  dprint("Place dropdown menu at (%i, %i)", iX, iY)

  Controls.ClassLabel:SetText(Locale.ConvertTextKey("TXT_KEY_PROMO_GROUP_CLASS"))

  Controls.DropDownBox:SetHide(false)
  Controls.UnitBox:SetHide(true)

  PopulateClassDropDown()

  Controls.Legend:SetHide(true)
end

function DrawUnitBox(iX, iY, pUnit)
  dprint("Place unit box for %s at (%i, %i)", pUnit:GetName(), iX, iY)

  Controls.ClassLabel:SetText(Locale.ConvertTextKey("TXT_KEY_PROMO_GROUP_UNIT"))

  Controls.UnitBox:SetHide(false)
  Controls.DropDownBox:SetHide(true)

  Controls.UnitName:SetText(pUnit:GetName())

  local unit = GameInfo.Units[pUnit:GetUnitType()]
  Controls.UnitPortraitFrame:SetHide(IconHookup(unit.PortraitIndex, iUnitIconSize, unit.IconAtlas, Controls.UnitPortrait) ~= true)

  Controls.UnitBox:SetToolTipString(GetPromotionsToolTip(pUnit))

  Controls.Legend:SetHide(false)
  OffsetAgain(Controls.Legend)
end

function DrawPromotionButton(iX, iY, pUnit, sPromotion, sCombatClass)
  dprint("Place %s at (%i, %i)", sPromotion, iX, iY)
  local promotion = GameInfo.UnitPromotions[sPromotion]

  local sName = Locale.ConvertTextKey(promotion.Description)
  local sToolTip = Locale.ConvertTextKey(promotion.Help)

  -- UndeadDevel: add tool tips for restrictions on the promotions only on relevant unit classes
  if (sPromotion:match("INTERCEPTION_[IV]+$") ~= nil and sCombatClass == "UNITCOMBAT_NAVALMELEE") then
    sToolTip = sToolTip .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_PROMO_INTERCEPTION_TOOL_TIP")
  elseif (sPromotion:match("EVASION_[I]+$") ~= nil and sCombatClass == "UNITCOMBAT_BOMBER") then
    sToolTip = sToolTip .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_PROMO_EVASION_TOOL_TIP")
  elseif (sCombatClass == "UNITCOMBAT_ARCHER" and (sPromotion == "PROMOTION_INDIRECT_FIRE" or sPromotion == "PROMOTION_RANGE")) then
    sToolTip = sToolTip .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_PROMO_RANGE_LIMITATION_TOOL_TIP")
  end

  local button = ButtonManagerGetButton(iX, iY, sToolTip, promotion.IconAtlas, promotion.PortraitIndex, promotion.TechPrereq)

  if (pUnit == nil or HasPromotion(pUnit, promotion.ID)) then
    button.EarntName:SetText(sName)
    button.Earnt:SetHide(false)
    button.Available:SetHide(true)
    button.Unavailable:SetHide(true)
  elseif (CanAcquirePromotion(pUnit, promotion.ID)) then
    button.AvailableName:SetText(sName)
    button.Available:SetHide(false)
    button.Earnt:SetHide(true)
    button.Unavailable:SetHide(true)

    button.Button:SetVoid1(promotion.ID)
    button.Button:RegisterCallback(Mouse.eLClick, OnSelectPromotion)
  else
    button.UnavailableName:SetText(sName)
    button.Unavailable:SetHide(false)
    button.Earnt:SetHide(true)
    button.Available:SetHide(true)
  end
end

function DrawBottomPipe(iX, iY, iDirection, colour)
  local sType
  if (iDirection == -1) then
    sType = "bottom-right"
  else
    sType = "top-right"
  end
  PipeManagerDrawQuadrantPipe(iX, iY, sType, colour)
end

function DrawTopPipe(iX, iY, iDirection, colour)
  local sType
  if (iDirection == -1) then
    sType = "top-left"
  else
    sType = "bottom-left"
  end
  PipeManagerDrawQuadrantPipe(iX, iY, sType, colour)
end


--
-- Helper functions
--

function GetPromotionsToolTip(pUnit)
  local sPromotions = ""

  local sRankedPromotions = ""
  local sRankedPrefix = ""
  local sPositivePromotions = ""
  local sPositivePrefix = ""
  local sNegativePromotions = ""
  local sNegativePrefix = ""

  for promotion in GameInfo.UnitPromotions() do
    if (pUnit:IsHasPromotion(promotion.ID)) then
      if (IsRankedPromotion(promotion.Type)) then
        sRankedPromotions = sRankedPromotions .. sRankedPrefix .. "[COLOR_YELLOW]" .. Locale.ConvertTextKey(promotion.Description) .. "[ENDCOLOR]: " .. Locale.ConvertTextKey(promotion.Help)
        sRankedPrefix = "[NEWLINE]"
      else
        if (promotion.PortraitIndex == 57) then
          sNegativePromotions = sNegativePromotions .. sNegativePrefix .. "[COLOR_RED]" .. Locale.ConvertTextKey(promotion.Description) .. "[ENDCOLOR]: " .. Locale.ConvertTextKey(promotion.Help)
          sNegativePrefix = "[NEWLINE]"
        else
          sPositivePromotions = sPositivePromotions .. sPositivePrefix .. "[COLOR_YELLOW]" .. Locale.ConvertTextKey(promotion.Description) .. "[ENDCOLOR]: " .. Locale.ConvertTextKey(promotion.Help)
          sPositivePrefix = "[NEWLINE]"
        end
      end
    end
  end

  if (sNegativePromotions ~= "") then
    if (sPositivePromotions ~= "") then
      sPositivePromotions = sPositivePromotions .. "[NEWLINE]" .. sNegativePromotions
    else
      sPositivePromotions = sNegativePromotions
    end
  end

  if (sRankedPromotions ~= "") then
    if (sPositivePromotions ~= "") then
      sPromotions = sRankedPromotions .. "[NEWLINE][NEWLINE]" .. sPositivePromotions
    else
      sPromotions = sRankedPromotions
    end
  elseif (sPositivePromotions ~= "") then
    sPromotions = sPositivePromotions
  end

  return sPromotions
end


--
-- UI functions
--

g_ClassList = nil

function PopulateClassDropDown()
  if (g_ClassList == nil) then
    PopulateClassList()

    for iIndex = 1, #g_ClassList, 1 do
      local controlTable = {}
      Controls.ClassDropDown:BuildEntry("InstanceOne", controlTable)

      controlTable.Button:SetVoid1(iIndex)
      controlTable.Button:SetText(Locale.ConvertTextKey(g_ClassList[iIndex].TextKey))
    end

    Controls.ClassDropDown:GetButton():SetText(Locale.ConvertTextKey(g_ClassList[iDefaultCombatClass].TextKey))

    Controls.ClassDropDown:CalculateInternals()
    Controls.ClassDropDown:RegisterSelectionCallback(OnSelectCombatClass)
  end
end

function PopulateClassList()
  g_ClassList = {}

  local sQuery = "SELECT x.CombatClass, x.IsDefault, x.Description, x.DefaultUnitType FROM UnitCombatInfosEx x"   -- UndeadDevel: we just use our own class table
  for row in DB.Query(sQuery) do
    table.insert(g_ClassList, {CombatClass=row.CombatClass, TextKey=row.Description, UnitType=row.DefaultUnitType})

    if (iDefaultCombatClass < 1 and row.IsDefault == true) then
      iDefaultCombatClass = #g_ClassList
    end
  end
end


--
-- UI handlers
--

function ShowHideHandler(bIsHide, bIsInit)
  if (not bIsHide and not bIsInit) then
    if (iSelectedUnit == nil) then
      PlaceDropDown()
    else
      PlaceUnit(Players[Game.GetActivePlayer()]:GetUnitByID(iSelectedUnit))
    end
  end

  bTreeVisible = (bIsHide == false)
end
ContextPtr:SetShowHideHandler(ShowHideHandler)

function OnDisplay(iUnit)
    iSelectedUnit = iUnit
    dprint("OnDisplay(%i)", iSelectedUnit or 0)

  UIManager:QueuePopup(ContextPtr, PopupPriority.BarbarianCamp)
end
LuaEvents.PromotionTreeDisplay.Add(OnDisplay)

function OnResize(bIsChecked)
  dprint("OnResize(%s)", (bIsChecked and "true" or "false"))

  if (bIsChecked) then
    Small()
  else
    Normal()
  end
end
Controls.ResizeButton:RegisterCheckHandler(OnResize);

function OnClose()
  Hide()
  UIManager:DequeuePopup(ContextPtr)
end
Controls.CloseButton:RegisterCallback(Mouse.eLClick, OnClose)
Events.GameplaySetActivePlayer.Add(OnClose)


function OnSwitchMode()
    if ClassMode then
        su = UI.GetHeadSelectedUnit()
        if (su) then
            LuaEvents.PromotionTreeDisplay(su:GetID())
            ClassMode = false
        end
    else
        LuaEvents.PromotionTreeDisplay()
        ClassMode = true
    end
end
Controls.SwitchModeButton:RegisterCallback(Mouse.eLClick, OnSwitchMode)

function InputHandler(uiMsg, wParam, lParam)
  if (uiMsg == KeyEvents.KeyDown) then
    if (wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN) then
      OnClose()
      return true
    elseif (wParam == Keys.S) then
        OnSwitchMode()
    elseif (wParam == Keys.D) then
        Controls.ResizeButton:SetCheck(largeUI == true)
        if (largeUI) then
          Small()
        else
          Normal()
        end
    end
  end
end
ContextPtr:SetInputHandler(InputHandler)

function OnSelectCombatClass(iIndex)
  if (iIndex == nil or iIndex < 1 or iIndex > #g_ClassList) then
    iIndex = iDefaultCombatClass or 1
  end

  iDefaultCombatClass = iIndex

  Controls.ClassDropDown:GetButton():SetText(Locale.ConvertTextKey(g_ClassList[iIndex].TextKey))

  local unit = GameInfo.Units[g_ClassList[iIndex].UnitType]
  Controls.ClassPortraitFrame:SetHide(IconHookup(unit.UnitFlagIconOffset, iDropDownIconSize, unit.UnitFlagAtlas, Controls.ClassPortrait) ~= true)

  PlacePromotions(nil, g_ClassList[iIndex].CombatClass, iLeftMargin + iDropDownSizeX, iCentreLine)
end

function OnSelectPromotion(iPromotion)
  local pUnit = UI.GetHeadSelectedUnit()
  dprint("Selected %s for %s", GameInfo.UnitPromotions[iPromotion].Type, pUnit:GetName())

  Events.AudioPlay2DSound("AS2D_INTERFACE_UNIT_PROMOTION")
  Game.HandleAction(GetActionForPromotion(iPromotion))
end

function OnUnitInfoDirty()
  if (bTreeVisible) then
    local pUnit = UI.GetHeadSelectedUnit()

    if (pUnit and pUnit:GetID() == iSelectedUnit) then
      PlaceUnit(pUnit)
    end
  end
end
Events.SerialEventUnitInfoDirty.Add(OnUnitInfoDirty)

function Hide()
  dprint("Hide()")

  ContextPtr:SetHide(true)
end

function Show()
  dprint("Show()")

  ContextPtr:SetHide(false)
end

function Small()
  dprint("Small()")

  Resize(config.small)
  Show()
end

function Normal()
  dprint("Normal()")

  Resize(config.normal)
  Show()
end

function Resize(config)
  dprint("Resize to %s", config.NAME)

  if (iHeight ~= config.PANEL) then
    ButtonManagerReset()
    PipeManagerReset()
  end

  largeUI = (config.NAME == "Normal")
  
  iHeight = config.PANEL
  iGapY = config.GAP
  iCentreLine = iTopMargin + (iHeight / 2)
  AdjustPanelHeight(iHeight)

  iButtonSizeX, iButtonSizeY = ButtonManagerInit(config.BUTTON, "Button", Controls.ScrollPanel)

  iPipeSizeX = config.PIPE
  iPipeSizeY = iPipeSizeX
  PipeManagerInit(iPipeSizeY, "PipeInstance", "PipeBox", Controls.ScrollPanel)
end

function Debug()
  bDebug = true
end

function NoDebug()
  bDebug = false
end

function Init()
  dprint("Init()")

  Resize(OptionsManager.GetSmallUIAssets() and config.small or config.normal)
  Controls.ResizeButton:SetCheck(OptionsManager.GetSmallUIAssets())
  Hide()
end

Init()

function ShowOnDemand()
    su = UI.GetHeadSelectedUnit()
    if (su) then
        LuaEvents.PromotionTreeDisplay(su:GetID())
        ClassMode = false
        Controls.SwitchModeButton:SetHide(false)
    else
        LuaEvents.PromotionTreeDisplay()
        ClassMode = true
        Controls.SwitchModeButton:SetHide(true)
    end
end

function OnAdditionalInformationDropdownGatherEntries(additionalEntries)
--  table.insert(additionalEntries, {text=Locale.ConvertTextKey("TXT_KEY_PROMO_DIPLO_CORNER_HOOK"), call=OnDisplay})
  table.insert(additionalEntries, {text=Locale.ConvertTextKey("TXT_KEY_PROMO_DIPLO_CORNER_HOOK"), call=ShowOnDemand})
end
LuaEvents.AdditionalInformationDropdownGatherEntries.Add(OnAdditionalInformationDropdownGatherEntries)
LuaEvents.RequestRefreshAdditionalInformationDropdownEntries()