-- VET_Events
-- Author: Thalassicus
-- DateCreated: 3/20/2012 1:12:12 AM
--------------------------------------------------------------

include("MT_Events.lua")
include("FLuaVector")

local log = Events.LuaLogger:New()
log:SetLevel("INFO")


function UpdatePlotYields()
	print("UpdatePlotYields Start")
	--
	if UI:IsLoadedGame() then
		return
	end
	--]]
	for plotID, plot in Plots() do
		local x				= plot:GetX()
		local y				= plot:GetY()
		
		if plot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT and not plot:IsHills() and plot:GetFeatureType() == -1 then
			local resInfo = GameInfo.Resources[plot:GetResourceType()]
			if plot:IsFreshWater() then
				Game.SetPlotExtraYield( x, y, YieldTypes.YIELD_FOOD, 1)
			elseif resInfo then
				if resInfo.ResourceClassType == "RESOURCECLASS_BONUS" and resInfo.Type ~= "RESOURCE_STONE" then
					Game.SetPlotExtraYield( x, y, YieldTypes.YIELD_FOOD, 1)
				elseif resInfo.Happiness > 0 then
					Game.SetPlotExtraYield( x, y, YieldTypes.YIELD_GOLD, 1)
				elseif not resInfo.TechReveal then
					Game.SetPlotExtraYield( x, y, YieldTypes.YIELD_PRODUCTION, 1)
				end
			end
		end
	end
	print("UpdatePlotYields Done")
end
Events.SequenceGameInitComplete.Add(function() return SafeCall(UpdatePlotYields) end)


function MoveIncanMountainResources()
	--[[
	print("MoveIncanMountainResources Start")
	if UI:IsLoadedGame() then
		return
	end
	local settlers = {}
	for playerID, player in pairs(Players) do
		if player:IsAliveCiv() and not player:IsMinorCiv() then
			
		end
	end
	
	print("MoveIncanMountainResources Done")
	--]]
end
if GameInfo.Builds.BUILD_TERRACE_FARM then
	Events.SequenceGameInitComplete.Add(function() return SafeCall(MoveIncanMountainResources) end)
end


function DoResourceDiscovered(playerID, techID)
	local player = Players[playerID]
	local techInfo = GameInfo.Technologies[techID]
	--log:Debug("player=%s tech=%s", player:GetName(), techInfo.Type)
	for resourceInfo in GameInfo.Resources(string.format("TechReveal = '%s'", techInfo.Type)) do
		for plotID, plot in Plots() do
			if Plot_IsFlatDesert(plot) and plot:GetResourceType() == resourceInfo.ID and plot:GetOwner() == playerID then
				Plot_ChangeYield(plot, YieldTypes.YIELD_GOLD, 1)
			end
		end
	end
end
--Events.TechAcquired.Add(DoResourceDiscovered)

function CreateWorkboats(player, techID, changeID)
	local playerID = player:GetID()
	if player:IsHuman() then
		return
	end
	local techType = GameInfo.Technologies[techID].Type
	for buildInfo in GameInfo.Builds(string.format("ImprovementType IS NOT NULL AND ShowInTechTree AND PrereqTech = '%s'", techType)) do
		local improveInfo = GameInfo.Improvements[buildInfo.ImprovementType]
		if improveInfo.Water then
			log:Info("Checking for workboats %s %s %s", player:GetName(), techType, improveInfo.Type)
			for city in player:Cities() do
				if city:IsCoastal() then
					for plot in Plot_GetPlotsInCircle(city:Plot(), 1, 2) do
						local resInfo = GameInfo.Resources[plot:GetResourceType()]
						if resInfo and plot:GetPlotType() == PlotTypes.PLOT_OCEAN and plot:GetImprovementType() == -1 then
							if Game.HasValue( {ImprovementType=improveInfo.Type, ResourceType=resInfo.Type}, GameInfo.Improvement_ResourceTypes ) then
								log:Info("  %s spawned for %s", city:GetName(), resInfo.Type)
								player:InitUnitClass("UNITCLASS_WORKBOAT", city:Plot())
							end
						end
					end
				end
			end
		end
	end
end
LuaEvents.NewTech.Add(CreateWorkboats)

---------------------------------------------------------------------
---------------------------------------------------------------------

function FloodplainCity(hexPos, playerID, cityID, newPlayerID)
	local plot = Map.GetPlot(ToGridFromHex(hexPos.x, hexPos.y))
	if plot:GetTerrainType() == TerrainTypes.TERRAIN_DESERT and not plot:IsHills() then
		plot:SetFeatureType(FeatureTypes.FEATURE_FLOOD_PLAINS, -1)
	end
end
LuaEvents.NewCity.Add(FloodplainCity)

---------------------------------------------------------------------
---------------------------------------------------------------------

function GreatImprovementEraBonus(hexX, hexY, cultureArtID, continentArtID, playerID, engineImprovementTypeDoNotUse, improvementID, engineResourceTypeDoNotUse, resID, eraID, improvementState)
	--print("OnImprovementCreated");
	--print("hexX: " .. hexX);
	--print("hexY: " .. hexY);
	--print("cultureArtID: " .. cultureArtID);
	--print("playerID: " .. playerID);
	--print("improvementID: " .. improvementID);
	--print("resID: " .. resID);
	--print("eraID: " .. eraID);
	--print("improvementState: " .. improvementState);
	--print("------------------");

	local impInfo	= GameInfo.Improvements[improvementID]
	local player	= Players[playerID]
	local plot		= Map.GetPlot(ToGridFromHex(hexX, hexY))
	if not impInfo.CreatedByGreatPerson or eraID == 0 then
		return
	end

	for yieldInfo in GameInfo.Yields() do
		local extraYield = GetImprovementExtraYield(impInfo.ID, yieldInfo.ID, player)

		if extraYield > 0 then
			log:Info("%s + %s %s", impInfo.Type, extraYield, yieldInfo.Type)
			Plot_ChangeYield(plot, yieldInfo.ID, extraYield)
		end
	end
end
--Events.SerialEventImprovementCreated.Add(GreatImprovementEraBonus)
