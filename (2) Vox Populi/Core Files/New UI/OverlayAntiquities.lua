include("FLuaVector")

local highlights = { Red     = Vector4(1.0, 0.0, 0.0, 1.0), 
                     Green   = Vector4(0.0, 1.0, 0.0, 1.0), 
                     Blue    = Vector4(0.0, 0.0, 1.0, 1.0),
                     Cyan    = Vector4(0.0, 1.0, 1.0, 1.0),
                     Yellow  = Vector4(1.0, 1.0, 0.0 ,1.0),
                     Magenta = Vector4(1.0, 0.0, 1.0, 1.0),
                     Black   = Vector4(0.5, 0.5, 0.5, 1.0)}             

local iExplorationFinisher = GameInfoTypes.POLICY_AESTHETICS_FINISHER
local iSiteHut = GameInfoTypes.IMPROVEMENT_GOODY_HUT
local iSiteDig = GameInfoTypes.RESOURCE_ARTIFACTS
local iHiddenDig = GameInfoTypes.RESOURCE_HIDDEN_ARTIFACTS

local colourGoodyHut = highlights["Yellow"]
local colourDigSite = highlights["Green"]
local colourHiddenSite = highlights["Cyan"]

local siteInfo = {
    {Name="TXT_KEY_ANTIQUITY_LEGEND_GOODY_HUT", Color={R=colourGoodyHut.x, G=colourGoodyHut.y, B=colourGoodyHut.z}},
    {Name="TXT_KEY_ANTIQUITY_LEGEND_DIG_SITE", Color={R=colourDigSite.x, G=colourDigSite.y, B=colourDigSite.z}},
    {Name="TXT_KEY_ANTIQUITY_LEGEND_HIDDEN_SITE", Color={R=colourHiddenSite.x, G=colourHiddenSite.y, B=colourHiddenSite.z}}
}

local pSitePlots = {}
local iSiteIndex = 0

function lookatSite(iPlayer, iOffset)
  if (iSiteIndex > 0) then
    seekSiteIndex(iPlayer, iSiteIndex, iOffset)
  end

  if (iSiteIndex >= 1 and iSiteIndex <= #pSitePlots) then
    local pPlot = pSitePlots[iSiteIndex]

    if (pPlot ~= nil) then
      UI.LookAt(pPlot)
    end
  end
end

function seekSiteIndex(iPlayer, iStart, iOffset)
  repeat
    iSiteIndex = iSiteIndex + iOffset

    if (iSiteIndex < 1) then
      iSiteIndex = #pSitePlots
    elseif (iSiteIndex > #pSitePlots) then
      iSiteIndex = 1
    end
  until (iSiteIndex == iStart or isVisibleSite(iPlayer, pSitePlots[iSiteIndex]))
end

function isVisibleSite(iPlayer, pPlot)
  local pPlayer = Players[iPlayer]
  local iTeam = pPlayer:GetTeam()

  if (pPlot:IsRevealed(iTeam)) then
    -- Any revealed goody hut
    if (pPlot:GetImprovementType() == iSiteHut) then
      return true
    end

    -- Any revealed dig site
    if (pPlot:GetResourceType() == iSiteDig) then
      return true
    end

    -- Any revealed hidden site
    if (pPlayer:HasPolicy(iExplorationFinisher) and pPlot:GetResourceType() == iHiddenDig) then
      return true
    end
  end

  return false
end

function clearSiteCache()
  pSitePlots = {}
  iSiteIndex = 0
end

function cacheSitePlots(iPlayer)
  pSitePlots = {}
  iSiteIndex = 1

  local iPlotCount = 1
  local iIndexDistance = 999
  local pCentrePlot = Map.GetPlot(UI.GetMouseOverHex())

  for iPlot = 0, Map.GetNumPlots()-1, 1 do
    local pPlot = Map.GetPlotByIndex(iPlot)

    if (isVisibleSite(iPlayer, pPlot)) then
      table.insert(pSitePlots, pPlot)
      iPlotCount = iPlotCount + 1

      if (pCentrePlot ~= nil) then
        local iDistance = Map.PlotDistance(pCentrePlot:GetX(), pCentrePlot:GetY(), pPlot:GetX(), pPlot:GetY())

        if (iDistance < iIndexDistance) then
          iSiteIndex = iPlotCount
          iIndexDistance = iDistance
        end
      end
    end
  end

  iSiteIndex = math.min(iSiteIndex, #pSitePlots)

  LuaEvents.MiniMapOverlayHideLegendNav(iSiteIndex == 0)
end

function highlightNone()
  Events.ClearHexHighlights()
end

function highlightAllSites(iPlayer)
  local iTeam = Players[iPlayer]:GetTeam()

  highlightGoodyHuts(iTeam)
  highlightDigSites(iTeam)
end

function highlightGoodyHuts(iTeam)
  for _, pPlot in ipairs(pSitePlots) do
    if (pPlot:GetImprovementType() == iSiteHut) then
      highlightPlot(iTeam, pPlot, colourGoodyHut)
    end
  end
end

function highlightDigSites(iTeam)
  for _, pPlot in ipairs(pSitePlots) do
    if (pPlot:GetResourceType() == iSiteDig) then
      highlightPlot(iTeam, pPlot, colourDigSite)
	elseif (pPlot:GetResourceType() == iHiddenDig) then
      highlightPlot(iTeam, pPlot, colourHiddenSite)
    end
  end
end

function highlightPlot(iTeam, pPlot, highlight)
  Events.SerialEventHexHighlight(ToHexFromGrid(Vector2(pPlot:GetX(), pPlot:GetY())), true, highlight)
end


function OnAntiquitiesDisplay(bShow)
  if (bShow) then
    cacheSitePlots(Game.GetActivePlayer())
    highlightAllSites(Game.GetActivePlayer())

    LuaEvents.MiniMapOverlayLegend("TXT_KEY_ANTIQUITY_OVERLAY_ANTIQUITIES", siteInfo)
  else
    highlightNone()
  end

end
LuaEvents.AntiquitiesDisplay.Add(OnAntiquitiesDisplay) 

function OnNextSite()
  lookatSite(Game.GetActivePlayer(), 1)
end
LuaEvents.AntiquitiesDisplayNext.Add(OnNextSite) 

function OnPreviousSite()
  lookatSite(Game.GetActivePlayer(), -1)
end
LuaEvents.AntiquitiesDisplayPrevious.Add(OnPreviousSite) 
