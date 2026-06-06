--- @meta

--- @class Plot
local Plot = {}

--- TODO docs
--- @return integer
--- @nodiscard
function Plot:GetX() end

--- TODO docs
--- @return integer
--- @nodiscard
function Plot:GetY() end

--- TODO docs
--- @return PlotId
--- @nodiscard
function Plot:GetIndex() end

--- Gets plot owner id
--- @return PlayerId
--- @nodiscard
function Plot:GetOwner() end

--- TODO docs
--- @return AreaId
--- @nodiscard
function Plot:GetArea() end

--- TODO docs
--- @return LandmassId
--- @nodiscard
function Plot:GetLandmass() end

--- TODO docs
--- @return ContinentId
--- @nodiscard
function Plot:GetContinent() end

--- TODO docs
--- @return TeamId
--- @nodiscard
function Plot:GetTeam() end

--- Gets the number of units stationed in specified plot.
--- @return integer
--- @nodiscard
function Plot:GetNumUnits() end

--- Determines the amount of culture yielded by working the plot.
--- @return number
--- @nodiscard
function Plot:GetCulture() end

--- TOOD docs
--- @return boolean
--- @nodiscard
function Plot:IsCity() end

--- TODO docs
--- @return City | nil
--- @nodiscard
function Plot:GetPlotCity() end

--- TODO docs
--- @return boolean
--- @nodiscard
function Plot:IsWater() end

--- TODO docs
--- @return boolean
--- @nodiscard
function Plot:IsRiver() end

--- TODO docs
--- @return boolean
--- @nodiscard
function Plot:IsRiverSide() end

--- TODO docs
--- @return boolean
--- @nodiscard
function Plot:IsLake() end

--- TODO docs
--- @return boolean
--- @nodiscard
function Plot:IsHills() end

--- TODO docs
--- @return boolean
--- @nodiscard
function Plot:IsOwned() end

--- TODO docs
--- @return boolean
--- @nodiscard
function Plot:IsMountain() end

--- TODO docs
--- @param bIncludePseudo boolean? # Default true
--- @return boolean
--- @nodiscard
function Plot:IsNaturalWonder(bIncludePseudo) end

--- TODO docs
--- @return boolean
--- @nodiscard
function Plot:IsFreshWater() end

--- TODO docs
--- @return boolean
--- @nodiscard
function Plot:IsFlatlands() end

--- TODO docs
--- @return boolean
--- @nodiscard
function Plot:IsCoastalLand() end

--- TODO docs
--- @return boolean
--- @nodiscard
function Plot:IsAdjacentToLand() end

--- TODO docs
--- @param area Area
--- @return boolean
--- @nodiscard
function Plot:IsAdjacentToArea(area) end

--- TODO docs
--- @param iLandmassID LandmassId
--- @return boolean
--- @nodiscard
function Plot:IsAdjacentToLandmass(iLandmassID) end

--- TODO docs
--- @return boolean
--- @nodiscard
function Plot:IsRoutePillaged() end

--- TODO docs
--- @return boolean
--- @nodiscard
function Plot:IsImprovementEmbassy() end

--- TODO docs
--- @return boolean
--- @nodiscard
function Plot:IsImprovementPillaged() end

--- TODO docs
--- @return boolean
--- @nodiscard
function Plot:IsImpassable() end

--- Checks if plot is revealed by specified team.
--- @param teamId TeamId
--- @param debug boolean
--- @return boolean
--- @nodiscard
function Plot:IsRevealed(teamId, debug) end

--- @return PlotType
--- @nodiscard
function Plot:GetPlotType() end

--- @param ePlot PlotType
--- @param bRecalculate boolean? # Default true
--- @param bRebuildGraphics boolean? # Default true
--- @param bEraseUnitsIfWater boolean? # Default true
function Plot:SetPlotType(ePlot, bRecalculate, bRebuildGraphics, bEraseUnitsIfWater) end

--- @return TerrainType
--- @nodiscard
function Plot:GetTerrainType() end

--- @param eTerrain TerrainType
--- @param bRecalculate boolean? # Default false
--- @param bRebuildGraphics boolean? # Default false
function Plot:SetTerrainType(eTerrain, bRecalculate, bRebuildGraphics) end

--- @return FeatureType
--- @nodiscard
function Plot:GetFeatureType() end

--- @param eFeature FeatureType
function Plot:SetFeatureType(eFeature) end

--- @return ResourceType
--- @nodiscard
function Plot:GetResourceType() end

--- @param eResource ResourceType
--- @param iQuantity integer
--- @param bForMinorCivPlot boolean? # Default false
function Plot:SetResourceType(eResource, iQuantity, bForMinorCivPlot) end

--- @return boolean
--- @nodiscard
function Plot:IsWOfRiver() end

--- @param bNewValue boolean
--- @param eRiverDir FlowDirectionType
function Plot:SetWOfRiver(bNewValue, eRiverDir) end

--- @return boolean
--- @nodiscard
function Plot:IsNWOfRiver() end

--- @param bNewValue boolean
--- @param eRiverDir FlowDirectionType
function Plot:SetNWOfRiver(bNewValue, eRiverDir) end

--- @return boolean
--- @nodiscard
function Plot:IsNEOfRiver() end

--- @param bNewValue boolean
--- @param eRiverDir FlowDirectionType
function Plot:SetNEOfRiver(bNewValue, eRiverDir) end

--- @param eYield YieldType
--- @param bDisplay boolean? # Whether the result is for display (factors in active player knowledge), default false
--- @return integer
function Plot:CalculateYield(eYield, bDisplay) end

--- @return integer
--- @nodiscard
function Plot:GetNumResource() end
