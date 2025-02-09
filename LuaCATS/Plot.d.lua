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

--- Gets plot owner id
--- @return PlayerId
--- @nodiscard
function Plot:GetOwner() end

--- TODO docs
--- @return AreaId
--- @nodiscard
function Plot:GetArea() end

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
