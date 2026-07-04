--- @meta

--- @class Map
Map = {}

--- Get the width and height of the map
--- @return integer, integer
--- @nodiscard
function Map.GetGridSize() end

--- Get the plot on the specified grid coordinates. May return nil.
--- @param x integer
--- @param y integer
--- @return Plot
--- @nodiscard
function Map.GetPlot(x, y) end

--- Get the plot with the specified index.<br>
--- `iPlotId = w * y + x`, where `w` is the width of the map, and `x, y` are the grid coordinates.<br>
--- May return nil.
--- @param iPlotId PlotId
--- @return Plot
--- @nodiscard
function Map.GetPlotByIndex(iPlotId) end

--- @param iAreaID AreaId
--- @return Area
--- @nodiscard
function Map.GetArea(iAreaID) end

--- Whether the map wraps on the X-axis
--- @return boolean
function Map.IsWrapX() end

--- Whether the map wraps on the Y-axis
--- @return boolean
function Map.IsWrapY() end

--- @param iOption integer
--- @return integer
function Map.GetCustomOption(iOption) end

--- Get a random integer between `0` and `iRange - 1` inclusive
--- @param iRange integer
--- @param strLogMsg string
--- @return integer
function Map.Rand(iRange, strLogMsg) end

--- Get the plot to the `eDirection` of (x, y). May return nil.
--- @param x integer # Must be valid coordinate
--- @param y integer # Must be valid coordinate
--- @param eDirection DirectionType
--- @return Plot
function Map.PlotDirection(x, y, eDirection) end

--- Get the number of plots in the map
--- @return integer
function Map.GetNumPlots() end

--- Get the distance between (x1, y1) and (x2, y2)
--- @param x1 integer # Must be valid coordinate
--- @param y1 integer # Must be valid coordinate
--- @param x2 integer # Must be valid coordinate
--- @param y2 integer # Must be valid coordinate
--- @return integer
function Map.PlotDistance(x1, y1, x2, y2) end

--- Get the maximum plot distance between any two plots on the map,
--- factoring in map wraps
--- @return integer
function Map.MaxPlotDistance() end

--- Get the world size ID of the map
--- @return WorldSizeType
function Map.GetWorldSize() end

--- Get the ID of the biggest landmass/watermass
--- @param bWater boolean
--- @return LandmassId
function Map.FindBiggestLandmassID(bWater) end

--- Get the number of tiles of the specified landmass
--- @param iLandmassID LandmassId
--- @return integer
function Map.GetNumTilesOfLandmass(iLandmassID) end

--- Recalculate areas, landmasses, and continents
function Map.RecalculateAreas() end
