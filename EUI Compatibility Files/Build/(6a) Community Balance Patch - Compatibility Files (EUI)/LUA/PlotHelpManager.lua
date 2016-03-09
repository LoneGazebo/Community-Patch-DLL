-------------------------------
-- PlotHelpManager.lua
-- coded by bc1 from Civ V 1.0.3.276 and Civ BE code
-- without the caching optimizations (no longer applicable)
-- compatible with Gazebo's City-State Diplomacy Mod (CSD) for Brave New World v 21
-- compatible with GameInfo.Yields() iterator broken by Communitas
-- todo: resources hookup, maintenance cost & health, terraform
-- todo BE: don't show quest artifacts if there's already a (quest) improvement here
-------------------------------
include( "EUI_utilities" )

Events.SequenceGameInitComplete.Add(function()
print("Loading EUI plot help",ContextPtr,os.clock(),[[ 
 ____  _       _   _   _      _       __  __                                   
|  _ \| | ___ | |_| | | | ___| |_ __ |  \/  | __ _ _ __   __ _  __ _  ___ _ __ 
| |_) | |/ _ \| __| |_| |/ _ \ | '_ \| |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
|  __/| | (_) | |_|  _  |  __/ | |_) | |  | | (_| | | | | (_| | (_| |  __/ |   
|_|   |_|\___/ \__|_| |_|\___|_| .__/|_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|   
                               |_|                             |___/           
]])

include( "ResourceTooltipGenerator" )
local GenerateResourceToolTip = GenerateResourceToolTip
--EUI_utilities
local table = EUI.table
local YieldIcons = EUI.YieldIcons
local CountHexPlots = EUI.CountHexPlots
local IndexPlot = EUI.IndexPlot
local GameInfo = EUI.GameInfoCache -- warning! booleans are true, not 1, and use iterator ONLY with table field conditions, NOT string SQL query

-------------------------------
-- minor lua optimizations
-------------------------------

--local next = next
local pairs = pairs
--local ipairs = ipairs
--local pcall = pcall
--local print = print
--local select = select
--local setmetatable = setmetatable
--local string = string
local tonumber = tonumber
local tostring = tostring
--local type = type
--local unpack = unpack
--local table = table
--local table_insert = table.insert
--local table_remove = table.remove
--local table_concat = table.concat
--local os = os
--local os_time = os.time
--local os_date = os.date
--local math = math
local math_floor = math.floor
local math_ceil = math.ceil
--local math_min = math.min
local math_max = math.max
--local math_abs = math.abs
--local math_modf = math.modf
--local math_sqrt = math.sqrt
--local math_pi = math.pi
--local math_sin = math.sin
--local math_cos = math.cos
--local math_huge = math.huge

--local UI = UI
local UI_GetHeadSelectedUnit = UI.GetHeadSelectedUnit
local UI_GetMouseOverHex = UI.GetMouseOverHex
local UIManager = UIManager
local ToHexFromGrid = ToHexFromGrid
local IsGameCoreBusy = IsGameCoreBusy

--local Controls = Controls
local ContextPtr = ContextPtr
local Players = Players
local Teams = Teams
--local GameInfo = GameInfo
--local GameInfoActions = GameInfoActions
local GameInfoTypes = GameInfoTypes
local GameDefines = GameDefines
--local InterfaceDirtyBits = InterfaceDirtyBits
--local CityUpdateTypes = CityUpdateTypes
--local ButtonPopupTypes = ButtonPopupTypes
local YieldTypes = YieldTypes
local GameOptionTypes = GameOptionTypes
--local DomainTypes = DomainTypes
local FeatureTypes = FeatureTypes
--local FogOfWarModeTypes = FogOfWarModeTypes
--local OrderTypes = OrderTypes
--local PlotTypes = PlotTypes
--local TerrainTypes = TerrainTypes
--local InterfaceModeTypes = InterfaceModeTypes
--local NotificationTypes = NotificationTypes
--local ActivityTypes = ActivityTypes
--local MissionTypes = MissionTypes
--local ActionSubTypes = ActionSubTypes
--local GameMessageTypes = GameMessageTypes
--local TaskTypes = TaskTypes
--local CommandTypes = CommandTypes
--local DirectionTypes = DirectionTypes
--local DiploUIStateTypes = DiploUIStateTypes
--local FlowDirectionTypes = FlowDirectionTypes
--local PolicyBranchTypes = PolicyBranchTypes
--local FromUIDiploEventTypes = FromUIDiploEventTypes
--local CoopWarStates = CoopWarStates
--local ThreatTypes = ThreatTypes
--local DisputeLevelTypes = DisputeLevelTypes
--local LeaderheadAnimationTypes = LeaderheadAnimationTypes
--local TradeableItems = TradeableItems
--local EndTurnBlockingTypes = EndTurnBlockingTypes
local ResourceUsageTypes = ResourceUsageTypes
--local MajorCivApproachTypes = MajorCivApproachTypes
--local MinorCivTraitTypes = MinorCivTraitTypes
--local MinorCivPersonalityTypes = MinorCivPersonalityTypes
local MinorCivQuestTypes = MinorCivQuestTypes
--local CityAIFocusTypes = CityAIFocusTypes
--local AdvisorTypes = AdvisorTypes
--local GenericWorldAnchorTypes = GenericWorldAnchorTypes
--local GameStates = GameStates
--local GameplayGameStateTypes = GameplayGameStateTypes
--local CombatPredictionTypes = CombatPredictionTypes
--local ChatTargetTypes = ChatTargetTypes
--local ReligionTypes = ReligionTypes
--local BeliefTypes = BeliefTypes
--local FaithPurchaseTypes = FaithPurchaseTypes
--local ResolutionDecisionTypes = ResolutionDecisionTypes
--local InfluenceLevelTypes = InfluenceLevelTypes
--local InfluenceLevelTrend = InfluenceLevelTrend
--local PublicOpinionTypes = PublicOpinionTypes
--local ControlTypes = ControlTypes

local PreGame = PreGame
local Game = Game
--local Map = Map
local Map_GetPlot = Map.GetPlot
local OptionsManager = OptionsManager
local Events = Events
local Events_ClearHexHighlightStyle = Events.ClearHexHighlightStyle
local Events_SerialEventHexHighlight = Events.SerialEventHexHighlight
local Events_RequestYieldDisplay = Events.RequestYieldDisplay
--local Mouse = Mouse
--local MouseEvents = MouseEvents
local MouseOverStrategicViewResource = MouseOverStrategicViewResource
local Locale = Locale
local L = Locale.ConvertTextKey
local S = string.format
local YieldDisplayTypes_AREA = YieldDisplayTypes.AREA
--getmetatable("").__index.L = L

-------------------------------
-- Globals
-------------------------------
local GameInfoTechnologies = GameInfo.Technologies
local GameInfoPolicies = GameInfo.Policies
local GameInfoBeliefs = GameInfo.Beliefs

local civ5_mode = type( MouseOverStrategicViewResource ) == "function"
local gk_mode = type( Game.GetReligionName ) == "function"
local bnw_mode = type( Game.GetActiveLeague ) == "function"
local csd_mode = civ5_mode and type( Map_GetPlot(0,0).GetPlayerThatBuiltImprovement ) == "function"	-- Compatibility with Gazebo's City-State Diplomacy Mod (CSD) for Brave New World v23
local civ5_bnw_mode = bnw_mode and civ5_mode
--local civBE_mode = type( Game.GetAvailableBeliefs ) == "function"

local g_isGameDebugMode = Game.IsDebugMode()

local DomainTypes_DOMAIN_AIR = DomainTypes.DOMAIN_AIR
local MouseEvents_MouseMove = MouseEvents.MouseMove
local Controls_TheBox = Controls.TheBox

--local g_maxTipLength = 7 * Controls_TheBox:GetSizeY()
local g_tipTimerThreshold1, g_tipTimerThreshold2, g_isScienceEnabled, g_isPoliciesEnabled, g_isHappinessEnabled, g_isReligionEnabled, g_isOptionDebugMode, g_isNoob, g_isCivilianYields

local g_tipTimer = 0
local g_tipLevel = 0
local g_lastPlot = false
local g_isCityLimits

local g_specialFeatures = {
--	[FeatureTypes.FEATURE_JUNGLE or -1] = true,
--	[FeatureTypes.FEATURE_MARSH or -1] = true,
--	[FeatureTypes.FEATURE_OASIS or -1] = true,
	[FeatureTypes.FEATURE_ICE or -1] = true,
--	[FeatureTypes.FEATURE_FLOOD_PLAINS or -1] = true,
} g_specialFeatures[-1] = nil

local g_schemaQuirk = { CultureChange = YieldTypes.YIELD_CULTURE or "YIELD_CULTURE" }

local g_gameAvailableBeliefs = { Game.GetAvailablePantheonBeliefs, Game.GetAvailableFounderBeliefs, Game.GetAvailableFollowerBeliefs, Game.GetAvailableFollowerBeliefs, Game.GetAvailableEnhancerBeliefs, Game.GetAvailableBonusBeliefs }

local g_unitMouseOvers = table()
local g_lastTips = {}
local g_lastTip = false
local g_cityWorkingRadius = GameDefines.CITY_PLOTS_RADIUS
local g_moveDenominator = GameDefines.MOVE_DENOMINATOR
local g_defaultWorkRate
for row in GameInfo.Units() do
	g_defaultWorkRate = math_max( g_defaultWorkRate or 0, row.WorkRate )
end
print("found default work rate", g_defaultWorkRate)
g_defaultWorkRate = g_defaultWorkRate or 100

-------------------------------
-- Utilities
-------------------------------
local function ClearOverlays()
	for i = 1, #g_unitMouseOvers do
		Game.MouseoverUnit( g_unitMouseOvers:remove(), false )
	end
	if g_isCityLimits then
		g_isCityLimits = false
		Events_ClearHexHighlightStyle( "CityLimits" )
		Events_ClearHexHighlightStyle( "OwnedFill" )
		Events_ClearHexHighlightStyle( "OwnedOutline" )
		if g_isCivilianYields then
			Events_RequestYieldDisplay( YieldDisplayTypes_AREA, 0 )
		end
	end
end

local function ShowPlotTips( tip )
	if #(tip or "") > 0 then
		if g_lastTip ~= tip then
			g_lastTip = tip
			Controls_TheBox:SetToolTipString( tip )
		end
		Controls_TheBox:EnableToolTip( true )
	else
		Controls_TheBox:EnableToolTip( false )
	end
end

local function isMountainNearby( city, distance )
	for i = 0, (distance+1) * distance * 3 do
		local plot = city:GetCityIndexPlot(i)
		if plot and plot:IsMountain() then
			return true
		end
	end
end

local function isTerrainNearby( city, distance, terrainTypeID )
	for i = 0, (distance+1) * distance * 3 do
		local plot = city:GetCityIndexPlot(i)
		if plot and plot:GetTerrainType() == terrainTypeID then
			return true
		end
	end
end

-------------------------------
-- Plot Tool Tip
-------------------------------

local PlotToolTips = EUI.PlotToolTips or function( plot, isExtraTips )
	-- New Plot Tool Tip
	local activeTeamID = Game.GetActiveTeam()
	local activePlayerID = Game.GetActivePlayer()
	local activePlayer = Players[activePlayerID]
	local activeTeam = Teams[activeTeamID]
	local activeTeamTechs = activeTeam:GetTeamTechs()
	local activeCivilizationID = activePlayer:GetCivilizationType()
	local activeCivilizationType = (GameInfo.Civilizations[ activeCivilizationID ] or {}).Type
	local availableBeliefs = {}
	local activePlayerIdeologyType
	local plotOwner, plotCity, workingCity, owningCity
	local plotTechs = activeTeamTechs
	local plotBeliefs = {}

	local tips = table()

	if g_isReligionEnabled then
		if civ5_mode then
			local activePlayerBeliefs
			if activePlayer:HasCreatedReligion() then
				activePlayerBeliefs = Game.GetBeliefsInReligion( activePlayer:GetReligionCreatedByPlayer() )
			elseif activePlayer:HasCreatedPantheon() then
				activePlayerBeliefs = { activePlayer:GetBeliefInPantheon() }
			elseif activePlayer:CanCreatePantheon() then
				activePlayerBeliefs = {}
			end
			if activePlayerBeliefs then
				for i = 1, activePlayer:IsTraitBonusReligiousBelief() and 6 or 5 do
					local beliefID = activePlayerBeliefs[i]
					if beliefID and beliefID >= 0 then
						availableBeliefs[beliefID] = true -- because active player already has this belief in "i" belief class
					else
						for _,beliefID in pairs( g_gameAvailableBeliefs[i]() ) do
							availableBeliefs[beliefID] = true -- because available to active player in "i" belief class
						end
					end
				end
			end
		else
			for i, beliefID in pairs( Game.GetAvailableBeliefs() ) do
				availableBeliefs[ beliefID ] = true
			end
		end
	end
	if civ5_bnw_mode and g_isPoliciesEnabled then
		local activePlayerIdeologyID = activePlayer:GetLateGamePolicyTree()
		local activePlayerIdeology = GameInfo.PolicyBranchTypes[ activePlayerIdeologyID or -1 ]
		activePlayerIdeologyType = activePlayerIdeology and activePlayerIdeology.Type
	end

	------------------
	-- Tech Filter
	local function TechFilter( row )

		return row and plotTechs and not plotTechs:HasTech( row.ID )
	end

	------------------
	-- Policy Filter
	local function PolicyFilter( row )

		return row and not( plotOwner and plotOwner:HasPolicy( row.ID ) and not plotOwner:IsPolicyBlocked( row.ID ) )
			and not( activePlayerIdeologyType and activePlayerIdeologyType ~= row.PolicyBranchType )
	end

	------------------
	-- Belief Filter
	local function BeliefFilter(row)

		return row and not plotBeliefs[ row.ID ] and availableBeliefs[ row.ID ]
	end

	------------------
	-- Building Filter
	local function BuildingFilter( building )

		if building and activePlayer and activeTeam and activeCivilizationType then
			local buildingClass = building.BuildingClass
			if buildingClass then
				if owningCity then
					-- filter out building classes that city already has
					for row in GameInfo.Buildings{ BuildingClass = buildingClass } do
						if owningCity:IsHasBuilding(row.ID) then
							return false
						end
					end
					local owningCityPlot = owningCity:Plot()
					-- filter out buildings that city will never be able to build
					if ( owningCityPlot and owningCity:IsBuildingsMaxed() )
						or ( building.Water and not owningCity:IsCoastal( civ5_mode and building.MinAreaSize ) )
						or ( building.River and not owningCityPlot:IsRiver())
						or ( building.FreshWater and not owningCityPlot:IsFreshWater() )
-- CP
						or ( building.IsNoWater and owningCityPlot:IsFreshWater() )
-- END
						or ( building.Hill and not owningCityPlot:IsHills() )
						or ( building.Flat and owningCityPlot:IsHills() )
						or ( building.ProhibitedCityTerrain and owningCityPlot:GetTerrainType() == GameInfoTypes[building.ProhibitedCityTerrain] )
						or ( building.NearbyTerrainRequired and not isTerrainNearby( owningCity, 1, GameInfoTypes[building.NearbyTerrainRequired] ) )
						or ( building.Mountain and not isMountainNearby( owningCity, 1 ))
						or ( civ5_mode and building.NearbyMountainRequired and not isMountainNearby( owningCity, 2 ) )
					then
						return false
					end
				end

				-- filter out buildings which player will never be able to build
				if activePlayer:IsProductionMaxedBuildingClass( GameInfoTypes[buildingClass] )
-- redundant						or Game.IsBuildingClassMaxedOut( GameInfoTypes[buildingClass] )
-- redundant						or activeTeam:IsBuildingClassMaxedOut( GameInfoTypes[buildingClass] )
-- redundant						or activePlayer:IsBuildingClassMaxedOut( GameInfoTypes[buildingClass] )
					or activeTeam:IsObsoleteBuilding( building.ID )
					or ( civ5_mode and building.MaxStartEra and Game.GetStartEra() > GameInfoTypes[building.MaxStartEra] )
				then
					return false
				end

				-- special building ?
				local buildingClassOverride = GameInfo.Civilization_BuildingClassOverrides{ BuildingClassType = buildingClass, CivilizationType = activeCivilizationType }()
				if buildingClassOverride then
					return building.Type == buildingClassOverride.BuildingType
				else
					return building.Type == (GameInfo.BuildingClasses[ buildingClass ] or {}).DefaultBuilding
				end
			end
		end
	end
	-- Building Filter
	------------------

	------------------
	-- Yield Changes
	local function insertYieldChanges( tips, bullet, color, type, filter, info, ... )
		if tips and bullet and color and type and filter and info then
			local Yields = {}
			-- loop through tables to search
			for _, search in pairs{...} do
				-- scan table rows which match search
				if search then
					for row in search do
						-- get item which matches the type we're looking for
						local itemType, yieldType, yieldChange
						for k, v in pairs(row) do
							if k == type then
								itemType = v
							elseif k == "YieldType" then
								yieldType = YieldTypes[v]
							elseif tonumber(v) then
								yieldChange = v
								yieldType = g_schemaQuirk[k] or yieldType
							end
						end
						if itemType and yieldType and yieldChange then
							local itemYields = Yields[itemType]
							if not itemYields then
								itemYields = {}
								Yields[itemType] = itemYields
							end
							itemYields[yieldType] = (itemYields[yieldType] or 0) + yieldChange
						end
					end
				end
			end

			local row, yieldChange, text
			for itemType, itemYields in pairs(Yields) do
				text = ""
-- TODO: add culture for vanilla, happiness & resources
				for yieldID = 0, YieldTypes.NUM_YIELD_TYPES-1 do
					yieldChange = itemYields[ yieldID ]
					if yieldChange then
						if yieldChange > 0 then
							text = text .. S(" [COLOR_POSITIVE_TEXT]%+i[ENDCOLOR]", yieldChange ) .. (YieldIcons[yieldID] or "")
						elseif yieldChange < 0 then
							text = text .. S(" [COLOR_NEGATIVE_TEXT]%+i[ENDCOLOR]", yieldChange ) .. (YieldIcons[yieldID] or "")
						end
					end
				end
				if #text > 0 then
					row = info[itemType]
					if filter(row) then
						local name, tech, policy, belief, alreadyHave
						local canHave = true
						for k, v in pairs(row) do

							-- name: if it has a short description, use that, otherwise use the description
							if k == "ShortDescription" then
								name = v
							elseif k == "Description" then
								name = name or v

							-- does it have a tech requirement ?
							elseif k == "PrereqTech" and info ~= GameInfoTechnologies then
								tech = GameInfoTechnologies[v]
								if tech then
									alreadyHave = activeTeamTechs:HasTech( tech.ID )
									canHave = alreadyHave or g_isScienceEnabled
									tech = canHave and not alreadyHave
										and "[COLOR_CYAN]" .. L(tech.Description) .. "[ENDCOLOR] "
								end

							-- does it have a policy branch requirement ?
							elseif k == "PolicyBranchType" and info ~= GameInfoPolicies then
								policy = (GameInfo.PolicyBranchTypes[v] or {}).FreePolicy
								policy = GameInfoPolicies[ policy or -1 ]
								if policy then
									alreadyHave = activePlayer:HasPolicy( policy.ID )
									canHave = (alreadyHave or g_isPoliciesEnabled)
										and not activePlayer:IsPolicyBlocked( policy.ID )
										and not( activePlayerIdeologyType and activePlayerIdeologyType ~= policy.PolicyBranchType )
									policy = canHave and not alreadyHave
										and ("[COLOR_MAGENTA]" .. L(policy.Description) .. "[ENDCOLOR] ")
								end

							-- does building have a belief requirement ?
							elseif k == "UnlockedByBelief" and row.Cost < 2 then -- schema assumption: key exists only for bnw buildings
								if bnw_mode then
									belief = GameInfo.Belief_BuildingClassFaithPurchase{ BuildingClassType = row.BuildingClass }()
								else
									belief = GameInfo.Beliefs{ BuildingClassEnabled = row.BuildingClass }()
								end
								belief = belief and GameInfoBeliefs[ belief.BeliefType ]
								if belief then
									canHave = availableBeliefs[ belief.ID ]
									belief = canHave and not plotBeliefs[ belief.ID ]
										and ("[COLOR_WHITE]" .. L(belief.ShortDescription) .. "[ENDCOLOR] ")
								end
							end
						end
						if canHave and name then
							tips:insert( bullet .. (tech or "") .. (belief or "") .. (policy or "") .. color .. L(name) .. "[ENDCOLOR]" .. text )
						end
					end
				end
			end
		end
	end
	-- Yield Changes
	------------------

-- TODO: GameInfo.Improvement_ResourceTypes
-- TODO: GameInfo.Improvement_ResourceType_Yields

	-------------------------------------------------
	-- Improvement Yield Changes
	-------------------------------------------------
	local function insertImprovementYieldChanges( tips, improvement, bullet )
		if tips and improvement then
			bullet = bullet or ""
			local thisImprovementType = { ImprovementType = improvement.Type }
			if g_isScienceEnabled then
				insertYieldChanges( tips, bullet, "[COLOR_CYAN]", "TechType", TechFilter, GameInfoTechnologies, GameInfo.Improvement_TechYieldChanges( thisImprovementType ),
					( plot:IsFreshWater() and GameInfo.Improvement_TechFreshWaterYieldChanges or GameInfo.Improvement_TechNoFreshWaterYieldChanges)( thisImprovementType ) )
			end
			if g_isPoliciesEnabled then
				insertYieldChanges( tips, bullet, "[COLOR_MAGENTA]", "PolicyType", PolicyFilter, GameInfoPolicies, GameInfo.Policy_ImprovementYieldChanges( thisImprovementType ), GameInfo.Policy_ImprovementCultureChanges( thisImprovementType ) )
			end
			if g_isReligionEnabled then
				insertYieldChanges( tips, bullet, "[COLOR_WHITE]", "BeliefType", BeliefFilter, GameInfoBeliefs, GameInfo.Belief_ImprovementYieldChanges( thisImprovementType ) )
			end
		end
	end

	--------------------
	-- City State Quests
	local function insertCityStateQuest( tips, questID, textKey )
		for minorPlayerID = GameDefines.MAX_MAJOR_CIVS, GameDefines.MAX_CIV_PLAYERS-1 do
			local minorPlayer = Players[minorPlayerID]
			if minorPlayer then
				local minorTeamID = minorPlayer:GetTeam()
				if minorPlayer:IsMinorCiv() and activeTeamID ~= minorTeamID and minorPlayer:IsAlive() and activeTeam:IsHasMet( minorTeamID ) then
					-- Does the player have a quest corresponding to questID here?
					local isQuest
					if gk_mode then
						isQuest = minorPlayer:IsMinorCivActiveQuestForPlayer( activePlayerID, questID )
					else
						isQuest = minorPlayer:GetActiveQuestForPlayer( activePlayerID ) == questID
					end
					if isQuest
						and plot:GetX() == minorPlayer:GetQuestData1( activePlayerID, questID )
						and plot:GetY() == minorPlayer:GetQuestData2( activePlayerID, questID )
					then
						tips:insert( "[COLOR_POSITIVE_TEXT]" .. L( textKey, minorPlayer:GetCivilizationShortDescriptionKey() ) .. "[ENDCOLOR]" )
					end
				end
			end
		end
	end

	-------------------------------------------------
	-- Plot Help
	-------------------------------------------------

	if plot and plot:IsRevealed( activeTeamID, g_isGameDebugMode ) then

		local plotOwnerID = plot:GetRevealedOwner( activeTeamID, g_isGameDebugMode )
		plotOwner = plotOwnerID and Players[plotOwnerID]
		local plotTeamID = plot:GetRevealedTeam( activeTeamID, g_isGameDebugMode )
		local plotTeam = plotTeamID and Teams[plotTeamID]
		if plotTeam then
			plotTechs = plotTeam:GetTeamTechs()
		end
		workingCity = plot:GetWorkingCity()
		owningCity =  workingCity or ( plot.GetCityPurchaseID and plotOwner and plotOwner:GetCityByID( plot:GetCityPurchaseID() ) )
		plotCity = plot:GetPlotCity()
		local buildsInProgress = {}

		if g_isReligionEnabled and workingCity then
			if civ5_mode then
				local religionID = workingCity:GetReligiousMajority()
				if religionID > 0 then
					for _,belief in pairs( Game.GetBeliefsInReligion(religionID) ) do
						plotBeliefs[belief] = true
					end
				elseif religionID == 0 and plotOwner and plotOwner:HasCreatedPantheon() then --pantheon
					plotBeliefs[plotOwner:GetBeliefInPantheon()] = true
				end
				if bnw_mode then
					local pantheonID = workingCity:GetSecondaryReligionPantheonBelief()
					if pantheonID >= 0 then
						plotBeliefs[pantheonID] = true
					end
				end
			else
				for i, beliefID in pairs( Game.GetBeliefsAcquiredByPlayer( plotOwnerID ) ) do
					plotBeliefs[ beliefID ] = true
				end
			end
		end
		local workRate = math_max(0, (activePlayer:GetWorkerSpeedModifier() + 100) * g_defaultWorkRate ) / 100
		--------
		-- Units
		if plot:IsVisible( activeTeamID, g_isGameDebugMode ) then

			local units = {}
			-- Loop through all units in plot
			for i = 1, (bnw_mode and plot:GetNumLayerUnits() or plot:GetNumUnits()) do

				units[i] = bnw_mode and plot:GetLayerUnit(i-1) or plot:GetUnit(i-1)
			end
			if not civ5_mode then
				local unit = plot:GetOrbitalUnitInfluencingPlot()
				if unit and unit ~= units[#units] then
					units[#units+1] = unit
				end
			end
			for i=1, #units do
				local unit = units[i]
				if unit and not unit:IsInvisible( activeTeamID, g_isGameDebugMode ) then

					local unitOwnerID = unit:GetOwner()
					local unitOwner = Players[unitOwnerID]
					local unitTeamID = unit:GetTeam()

					local unitTip = ""
					local strCiv = unitOwner:GetCivilizationAdjective()
					local strNick = unitOwner:GetNickName()

					if activeTeamID == unitTeamID or (unitOwner:IsMinorCiv() and unitOwner:IsAllies(activePlayerID)) then
						unitTip = "[COLOR_POSITIVE_TEXT]"
					elseif activeTeam:IsAtWar(unitTeamID) then
						unitTip = "[COLOR_NEGATIVE_TEXT]"
					else
						unitTip = "[COLOR_WHITE]"
					end
					unitTip = unitTip .. L(unit:GetNameKey()) .. "[ENDCOLOR]"

					-- Player using nickname
					if PreGame.IsMultiplayerGame() and strNick and #strNick > 0 then

						unitTip = L( "TXT_KEY_MULTIPLAYER_UNIT_TT", strNick, strCiv, unitTip )

					elseif activeTeam:IsHasMet(unitTeamID) then

						unitTip = L( "TXT_KEY_PLOTROLL_UNIT_DESCRIPTION_CIV", strCiv, unitTip )
						if unit:HasName() then
							unitTip = L(unit:GetNameNoDesc()) .. " (" .. unitTip .. ")"
						end
					end

					local originalOwnerID = unit:GetOriginalOwner()
					local originalOwner = originalOwnerID and Players[originalOwnerID]
					if originalOwner and originalOwnerID ~= unitOwnerID and activeTeam:IsHasMet( originalOwner:GetTeam() ) then
						unitTip = unitTip .. " (" .. originalOwner:GetCivilizationAdjective() .. ")"
					end

					-- Debug stuff
					if g_isOptionDebugMode then
						unitTip = unitTip .. " (".. tostring(unitOwnerID) .." - " .. tostring(unit:GetID()) .. ")"
					end

					-- Moves & Combat Strength
					local unitMoves = 0
					local unitStrength = civ5_mode and unit:GetBaseCombatStrength() or unit:GetCombatStrength() --BE stupid function name change
					-- todo unit:GetMaxDefenseStrength()
					local rangedStrength = civ5_mode and unit:GetBaseRangedCombatStrength() or unit:GetRangedCombatStrength() --BE stupid function name change

					if unit:GetDomainType() == DomainTypes_DOMAIN_AIR then
						unitStrength = rangedStrength
						rangedStrength = 0
						unitMoves = unit:Range()
					else
						if unitOwnerID == activePlayerID then
							unitMoves = unit:MovesLeft()
						else
							unitMoves = unit:MaxMoves()
						end
						unitMoves = unitMoves / g_moveDenominator
					end

					-- In Orbit?
					if not civ5_mode and unit:IsInOrbit() then
						unitTip = unitTip .. " " .. "[COLOR_CYAN]" .. L( "TXT_KEY_PLOTROLL_ORBITING" ) .. "[ENDCOLOR]"

					elseif bnw_mode and unit:IsTrade() then
						-- show trade route
						g_unitMouseOvers:insert( unit )
						Game.MouseoverUnit( unit, true )
					else
						-- Moves
						if unitMoves > 0 then
							unitTip = S("%s %.3g[ICON_MOVES]", unitTip, unitMoves )
						end

						-- Strength
						if unitStrength > 0 then
							local adjustedUnitStrength = (math_max(100 + unit:GetStrategicResourceCombatPenalty(), 10) * unitStrength) / 100
							--todo other modifiers eg unhappy...
							if adjustedUnitStrength < unitStrength then
								adjustedUnitStrength = S(" [COLOR_NEGATIVE_TEXT]%.3g[ENDCOLOR]", adjustedUnitStrength )
							end
							unitTip = unitTip .. " " .. adjustedUnitStrength .. "[ICON_STRENGTH]"
						end

						-- Ranged Strength
						if rangedStrength > 0 then
							unitTip = unitTip .. " " .. rangedStrength .. "[ICON_RANGE_STRENGTH]"..unit:Range().." "
						end

						-- Religious Fervor
						if gk_mode then
							local spreadsLeft = unit:GetSpreadsLeft()
							if spreadsLeft > 0 then
								local icon = (GameInfo.Religions[unit:GetReligion()] or {}).IconString
								if icon then
									unitTip = unitTip .. " " .. spreadsLeft .. icon
								end
							end
						end

						-- Hit Points
						if unit:IsHurt() then
							unitTip = unitTip .. " " .. L( "TXT_KEY_PLOTROLL_UNIT_HP", unit:GetCurrHitPoints() )
						end
					end

					-- Embarked?
					if unit:IsEmbarked() then
						unitTip = unitTip .. " " .. L( "TXT_KEY_PLOTROLL_EMBARKED" )
					end

					tips:insert( unitTip )
					-- Can build something?
					if unitOwnerID == activePlayerID then
						local unitWorkRate = unit:WorkRate( true )
						if unitWorkRate > workRate then
							workRate = unitWorkRate
						end
					end

					-- Building something?
					local build = GameInfo.Builds[ unit:GetBuildType() ]
					if build then
						local buildTurnsLeft = plot:GetBuildTurnsLeft( build.ID, unitOwnerID, -unit:WorkRate() )
						buildsInProgress[ build.ID ] = buildTurnsLeft
						tips:insert( L( "TXT_KEY_WORKER_BUILD_PROGRESS", buildTurnsLeft, build.Description ) )
					end
				end
			end
--[[ remove = too much stuff
			-- Loop through trade routes
			if bnw_mode then
				local tradeRouteStrings = activePlayer:GetInternationalTradeRoutePlotToolTip(plot)	-- GetInternationalTradeRoutePlotMouseoverToolTip
				if #tradeRouteStrings > 0 then
					tips:insert( L"TXT_KEY_TRADE_ROUTE_TT_PLOT_HEADING" )
					for i,v in ipairs(tradeRouteStrings) do
						tips:insert( v.String )
					end
				end
			end
--]]
		end

		local selectedUnit = UI_GetHeadSelectedUnit()
		local isCombatUnitSelected
		if selectedUnit then
			isCombatUnitSelected = selectedUnit:IsCombatUnit()
			if selectedUnit:CanFound( plot ) then
				g_isCityLimits = true
				if g_isCivilianYields then
					Events_RequestYieldDisplay( YieldDisplayTypes_AREA, g_cityWorkingRadius, plot:GetX(), plot:GetY() )
				end
				for i=1, CountHexPlots( g_cityWorkingRadius ) do
					local p = IndexPlot( plot, i )
					if p then
						local hex = ToHexFromGrid{ x=p:GetX(), y=p:GetY() }
						Events_SerialEventHexHighlight( hex, true, nil, "CityLimits" )
						local ownerID = p:GetOwner()
						if ownerID >= 0 and ownerID ~= activePlayerID or p:IsPlayerCityRadius( activePlayerID ) then
--						if p:GetCityRadiusCount() > 0 then
							Events_SerialEventHexHighlight( hex, true, nil, "OwnedFill" )
							Events_SerialEventHexHighlight( hex, true, nil, "OwnedOutline" )
						end
					end
				end
			end
		end

		--------
		-- Owner
		local strOwner = ""

		if plotOwner then
			-- Met ?
			if activeTeam:IsHasMet( plotTeamID ) then
				if activeTeamID == plotTeamID or (plotOwner:IsMinorCiv() and plotOwner:IsAllies(activePlayerID))then
					strOwner = "[COLOR_POSITIVE_TEXT]"
				elseif activeTeam:IsAtWar( plotTeamID ) then
					strOwner = "[COLOR_NEGATIVE_TEXT]"
				elseif Teams[ plotTeamID ]:IsAllowsOpenBordersToTeam( activeTeamID ) then
					strOwner = "[COLOR_WHITE]"
				else
					strOwner = "[COLOR_YELLOW]"
				end
				-- Known city plot ?
				if owningCity and owningCity:Plot():IsRevealed( activeTeamID, g_isGameDebugMode ) then
					strOwner = strOwner .. owningCity:GetName() .. "[ENDCOLOR]"
					if g_isNoob then
						strOwner = L( "TXT_KEY_CITY_OF", plotOwner:IsMinorCiv() and "" or plotOwner:GetCivilizationAdjectiveKey(), strOwner )
					end
				-- No city, plot is just owned
				else
					strOwner = strOwner .. plotOwner:GetCivilizationShortDescription() .. "[ENDCOLOR]"
				end
			-- Not met
			else
				strOwner = L("TXT_KEY_UNMET_PLAYER")
			end
		end

		if #strOwner > 0 then
			tips:insert( strOwner )
		end

		------------
		-- Resources
		local resourceTip = ""
		local resourceID = plot:GetResourceType( activeTeamID )
		local resource, numResource, isResourceConnected, isResourceUsefull, resourceUsageType
		local isPillaged = plot:IsImprovementPillaged()
		local revealedImprovementID = plot:GetRevealedImprovementType( activeTeamID, g_isGameDebugMode )
		local actualImprovementID = plot:GetImprovementType()
		local revealedImprovement

		if resourceID >= 0 then
			resource = GameInfo.Resources[ resourceID ]
			resourceUsageType = Game.GetResourceUsageType( resourceID )
			isResourceUsefull = resourceUsageType ~= ResourceUsageTypes.RESOURCEUSAGE_BONUS
			isResourceConnected = plot:IsResourceConnectedByImprovement( revealedImprovementID ) and not isPillaged
			numResource = plot:GetNumResource()

			if resourceUsageType == ResourceUsageTypes.RESOURCEUSAGE_STRATEGIC then
				resourceTip = resourceTip .. numResource
			end
			resourceTip = resourceTip .. resource.IconString .. L( resource.Description )
-- CSD
			if csd_mode and resourceID == GameInfoTypes.RESOURCE_ARTIFACTS then
				insertCityStateQuest( tips, MinorCivQuestTypes.MINOR_CIV_QUEST_ARCHAEOLOGY, "TXT_KEY_CITY_STATE_ARCHAEOLOGY_QUEST_LONG" )
			end
-- CSD
			-- Resource Hookup info
			local resourceEnablerTechID = civ5_mode and GameInfoTypes[resource.TechCityTrade] -- todo
			if resourceEnablerTechID and resourceEnablerTechID ~= -1 and not activeTeamTechs:HasTech(resourceEnablerTechID) then

				local techName = GameInfoTechnologies[resourceEnablerTechID]
				techName = techName and techName.Description or "<unknown tech>"
				if g_isNoob then
					resourceTip = resourceTip .. " " .. L( "TXT_KEY_PLOTROLL_REQUIRES_TECH_TO_USE", techName )
				else
					resourceTip = resourceTip .. " " .. L( "TXT_KEY_PLOTROLL_REQUIRES_TECH", techName )
				end
			end
			if #resourceTip > 0 then
				if g_isNoob then
					tips:insert( L"TXT_KEY_RESOURCE" .. ": " .. resourceTip )
				else
					tips:insert( resourceTip )
				end
			end
		end

		------------------------
		-- Terrain type, feature
		local featureID = plot:GetFeatureType()
		local feature, isFeatureReplacesTerrain
		local terrain = GameInfo.Terrains[ plot:GetTerrainType() ]
		local featureTips = table()

		local isLakePlot = plot:IsLake()
		local isWaterPlot = plot:IsWater()
		local isSpecialFeature = g_specialFeatures[ featureID ] -- Some features are handled in a special manner, since they always have the same terrain type under it

		-- Miasma CIV BE only
		if not civ5_mode and plot:HasMiasma() then
			featureTips:insert( "[COLOR_NEGATIVE_TEXT]"..L((GameInfo.Features[FeatureTypes.FEATURE_MIASMA or -1] or {}).Description or "").."[ENDCOLOR]" )
		end

		-- Feature
		if featureID >= 0 then
			feature = GameInfo.Features[ featureID ]
			isFeatureReplacesTerrain = feature.YieldNotAdditive
			if civ5_mode and feature.NaturalWonder then
				isSpecialFeature = true
			end
			featureTips:insert( L( feature.Description ) )

		-- Mountain
		elseif plot:IsMountain() then
			isSpecialFeature = true
			featureTips:insert( L"TXT_KEY_PLOTROLL_MOUNTAIN" )

		-- Canyon CIV BE only
		elseif not civ5_mode and plot:IsCanyon() then
			isSpecialFeature = true
			featureTips:insert( L"TXT_KEY_PLOTROLL_CANYON" )
		end

		-- Terrain
		if not isSpecialFeature then
			-- Lake
			if isLakePlot then
				featureTips:insert( L"TXT_KEY_PLOTROLL_LAKE" )
			else
				featureTips:insert( L( terrain.Description ) )
			end
		end

		-- Hills
		if plot:IsHills() then
			featureTips:insert( L"TXT_KEY_PLOTROLL_HILL" )
		end

		-- River & Fresh Water
		if plot:IsRiver() then
			featureTips:insert( L"TXT_KEY_PLOTROLL_RIVER" )
		elseif plot:IsFreshWater() then
			featureTips:insert( L"TXT_KEY_PLOTROLL_FRESH_WATER" )
		end

		if #featureTips > 0 then
			if false and g_isNoob and not ( feature and feature.NaturalWonder ) then
				tips:insert( L"TXT_KEY_PEDIA_TERRAIN_LABEL" .. ": " .. featureTips:concat( ", " ) )
			else
				tips:insert( featureTips:concat( ", " ) )
			end
		end

		--------
		-- Yield
		local yieldTips = table()
		local yieldChange

		for yieldID = 0, YieldTypes.NUM_YIELD_TYPES-1 do -- GameInfo.Yields() iterator is broken by Communitas
			yieldChange = plot:CalculateYield( yieldID, true ) -- true = as shown to the active player
			if yieldChange ~= 0 then
				yieldTips:insert( yieldChange .. (YieldIcons[yieldID] or "") )
			end
		end

		if not civ5_mode then
			-- Health (local to plot)
			yieldChange = plot:GetHealth()
			if yieldChange ~= 0 then
				yieldTips:insert( yieldChange .. "[ICON_HEALTH_1]" )
			end

		elseif gk_mode then
			-- Happiness (should probably be calculated in CvPlayer)
			if feature and g_isHappinessEnabled then
				yieldChange = feature.InBorderHappiness
				yieldChange = math_floor( yieldChange * ( 100 + activePlayer:GetNaturalWonderYieldModifier() ) / 100 )

				if yieldChange ~= 0 then
					yieldTips:insert( yieldChange .. "[ICON_HAPPINESS_1]" )
				end
			end
		else
			yieldChange = plot:GetCulture()
			-- Only fudge in the additional culture if the owner is NOT the active player.
			if plotOwnerID ~= activePlayerID then
				if feature and feature.NaturalWonder then
					yieldChange = yieldChange + math_floor(feature.Culture * (100 + activePlayer:GetNaturalWonderYieldModifier()) / 100)
				end
			end
			if yieldChange ~= 0 then
				yieldTips:insert( yieldChange .. "[ICON_CULTURE]" )
			end
		end
		if resource and isResourceConnected and isResourceUsefull then
			yieldTips:insert( numResource .. resource.IconString )
		end

		-- Defense
		local defenseModifier = plot:DefenseModifier( activeTeamID, false, false )
		if defenseModifier ~= 0 then
			if g_isNoob then
				tips:insert( L"TXT_KEY_PEDIA_DEFENSE_LABEL" .. S(" %+i%%[ICON_STRENGTH]", defenseModifier) )
			else
				yieldTips:insert( S("%+i%%[ICON_STRENGTH]", defenseModifier) )
			end
		end
-- todo: moves required

		if #yieldTips > 0  then -- and (isExtraTips or g_isNoob or not isCombatUnitSelected) then
			if g_isNoob then
				tips:insert( L"TXT_KEY_OUTPUT" .. ": " .. yieldTips:concat( " " ) )
			else
				tips:insert( yieldTips:concat( " " ) )
			end
		end


		----------------------
		-- Improvement & Route
		local improvementTips = table()

		local function checkPillaged( row, flag )
			if row then
				local txt = L( row.Description )
				-- Compatibility with Gazebo's City-State Diplomacy Mod (CSD) for Brave New World
				if csd_mode and row.Type == "IMPROVEMENT_EMBASSY" then
					local player = Players[plot:GetPlayerThatBuiltImprovement()]
					if player then
						txt = txt .. " - ".. player:GetCivilizationShortDescription()
					end
				end
				if flag then
					return "[COLOR_NEGATIVE_TEXT]" .. txt .. " " .. L"TXT_KEY_PLOTROLL_PILLAGED" .. "[ENDCOLOR]"
				else
					return txt
				end
			else
				return ""
			end
		end

		if revealedImprovementID >= 0 then
			revealedImprovement = GameInfo.Improvements[revealedImprovementID]
			improvementTips:insert( "[COLOR_POSITIVE_TEXT]" .. checkPillaged( revealedImprovement, isPillaged ) .. "[ENDCOLOR]" )
		end

		local routeID = plot:GetRevealedRouteType( activeTeamID, g_isGameDebugMode )
		if routeID >= 0 then
			improvementTips:insert( checkPillaged(GameInfo.Routes[routeID], plot:IsRoutePillaged()) )
		end

		if g_isNoob and isExtraTips and plot:IsTradeRoute() then
			improvementTips:insert( L"TXT_KEY_PLOTROLL_TRADE_ROUTE" )
		end

		if #improvementTips > 0 then
			if g_isNoob then
				tips:insert( L"TXT_KEY_IMPROVEMENT" .. ": " .. improvementTips:concat(", ") )
			else
				tips:insert( improvementTips:concat(", ") )
			end
		end

		-- Maintenance (Improvement) CIVBE ONLY
		if not civ5_mode and plotOwnerID == activePlayerID then
			local energyMaintenance = plot:GetPlotMaintenance( activePlayerID )
			if energyMaintenance > 0 then
				tips:insert( "[COLOR_NEGATIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_CITYVIEW_MAINTENANCE") .. "[ENDCOLOR]" .. ": " .. energyMaintenance.."[ICON_ENERGY]" )
			end
		end

		if not isCombatUnitSelected or ( plotOwnerID == activePlayerID and isExtraTips ) then
			if not isPillaged then
				insertImprovementYieldChanges( tips, revealedImprovement, "[ICON_BULLET]" )
			end
			if not plotCity then
				local isSeaPlot = isWaterPlot and not isLakePlot
				insertYieldChanges(tips, "[ICON_BULLET]", "[COLOR_YIELD_FOOD]", "BuildingType", BuildingFilter, GameInfo.Buildings,
--TODO modifiers	GameInfo.Building_AreaYieldModifiers(),
--TODO modifiers	resource and GameInfo.Building_ResourceYieldModifiers{ ResourceType = resource.Type },
					resource and GameInfo.Building_ResourceCultureChanges{ ResourceType = resource.Type },
					plot:IsRiver() and GameInfo.Building_RiverPlotYieldChanges(),
					isSeaPlot and GameInfo.Building_SeaPlotYieldChanges(),
					isLakePlot and GameInfo.Building_LakePlotYieldChanges(),
					isSeaPlot and resource and GameInfo.Building_SeaResourceYieldChanges(),
					resource and GameInfo.Building_ResourceYieldChanges{ ResourceType = resource.Type },
					feature and GameInfo.Building_FeatureYieldChanges{ FeatureType = feature.Type },
					gk_mode and not isFeatureReplacesTerrain and GameInfo.Building_TerrainYieldChanges{ TerrainType = terrain.Type } )
			end
			if g_isPoliciesEnabled then
				insertYieldChanges( tips, "[ICON_BULLET]", "[COLOR_MAGENTA]", "PolicyType", PolicyFilter, GameInfoPolicies,
					plotCity and GameInfo.Policy_CityYieldChanges(),
					plotCity and plotCity:IsCoastal() and GameInfo.Policy_CoastalCityYieldChanges(),
					plotCity and plotCity:IsCapital() and plotOwner == activePlayer and GameInfo.Policy_CapitalYieldChanges(),
-- CP
					feature and plotOwner == activePlayer and GameInfo.Policy_FeatureYieldChanges{ FeatureType = feature.Type },
					feature and plotOwner == activePlayer and GameInfo.Policy_UnimprovedFeatureYieldChanges{ FeatureType = feature.Type },
					feature and feature.NaturalWonder and plotOwner == activePlayer and GameInfo.Policy_YieldChangesNaturalWonder(),
					resource and plotOwner == activePlayer and GameInfo.Policy_ResourceYieldChanges{ ResourceType = resource.Type },
					plotOwner == activePlayer and GameInfo.Policy_TerrainYieldChanges{ TerrainType = terrain.Type } )
-- END
--TODO modifiers	Policy_CapitalYieldModifiers
			end
			if g_isReligionEnabled then
				insertYieldChanges( tips, "[ICON_BULLET]", "[COLOR_WHITE]", "BeliefType", BeliefFilter, GameInfoBeliefs,
					plotCity and GameInfo.Belief_CityYieldChanges(),
					plotCity and plotCity:IsHolyCityAnyReligion() and GameInfo.Belief_HolyCityYieldChanges(),
					feature and GameInfo.Belief_FeatureYieldChanges{ FeatureType = feature.Type },
-- CP
					plot:IsLake() and GameInfo.Belief_LakePlotYield(),
					feature and plotOwner == activePlayer and GameInfo.Belief_UnimprovedFeatureYieldChanges{ FeatureType = feature.Type },
					feature and plotOwner == activePlayer and GameInfo.Belief_CityYieldFromUnimprovedFeature{ FeatureType = feature.Type },
					plotCity and plotCity:IsCoastal() and GameInfo.Belief_CoastalCityYieldChanges(),
					plotCity and plotCity:IsCapital() and plotOwner == activePlayer and GameInfo.Belief_CapitalYieldChanges(),					
-- END
					resource  and GameInfo.Belief_ResourceYieldChanges{ ResourceType = resource.Type },
					not isFeatureReplacesTerrain and GameInfo.Belief_TerrainYieldChanges{ TerrainType = terrain.Type },
					civ5_mode and feature and feature.NaturalWonder and GameInfo.Belief_YieldChangeNaturalWonder() )
--TODO modifiers	feature and feature.NaturalWonder and GameInfo.GetYieldModifierNaturalWonder()
			end

			for build in GameInfo.Builds() do
				local buildID = build.ID
				local buildTip = L(build.Description)

				local canBuild = plot:CanBuild( buildID, activePlayerID ) and build.ShowInPedia ~= false and isWaterPlot == build.Water -- filter duplicates, fix DLL bug can build roads in lakes
				local isBasicBuild = true
				local buildImprovement = GameInfo.Improvements[ build.ImprovementType or -1 ]

				local buildInProgress = buildsInProgress[ buildID ]

				if buildImprovement then
					buildTip = L(buildImprovement.Description)

					-- Work around unrevealed improvement game bug
					-- can always build unrevealed improvements (prevents "CanBuild" detection)
					if not (revealedImprovement or canBuild) then
						canBuild = actualImprovementID == buildImprovement.ID
					end

					-- case where improvement requires a specific civilization
					local civRequirement = buildImprovement.CivilizationType
					if civRequirement and canBuild then
						canBuild = GameInfoTypes[civRequirement] == activeCivilizationID
					end

					-- it's not basic to create a GP improvement
					if civ5_mode and buildImprovement.CreatedByGreatPerson then
						canBuild = canBuild and selectedUnit and selectedUnit:CanBuild( plot, buildID, 1, 0 )
						isBasicBuild = buildInProgress or canBuild
					end
				end

				-- does build have a tech requirement ?
				local tech1 = GameInfoTechnologies[ build.PrereqTech or -1 ]
				if tech1 and not activeTeamTechs:HasTech( tech1.ID ) then
					buildTip = "[COLOR_CYAN]" .. L(tech1.Description) .. "[ENDCOLOR] ".. buildTip
					canBuild = canBuild and g_isScienceEnabled
					isBasicBuild = isBasicBuild and (g_isNoob or isExtraTips)
				end

				-- does build remove a feature, and require a tech for doing so ?
				if feature then
					local row
					if civ5_mode then
						row = GameInfo.BuildFeatures{ BuildType = build.Type, FeatureType = feature.Type, Remove = true }() -- cache uses true / SQL uses 1
					else
						row = GameInfo.BuildsOnFeatures{ BuildType = build.Type, FeatureType = feature.Type, Remove = true }() -- stupid table rename by Firaxis
					end
					local tech2 = GameInfoTechnologies[ row and row.PrereqTech or -1 ]
					if tech2 and tech1 ~= tech2 and not activeTeamTechs:HasTech( tech2.ID ) then
						buildTip = "[COLOR_CYAN]" .. L(tech2.Description) .. "[ENDCOLOR] ".. buildTip
						canBuild = canBuild and g_isScienceEnabled
						isBasicBuild = isBasicBuild and (g_isNoob or isExtraTips)
					end
				end

				-- does build require time to build ?
				if plot:GetBuildTime(buildID) > 0 then
					local turnsRemaining = buildInProgress or math_ceil( ( plot:GetBuildTime( buildID, activePlayerID ) - math_max( workRate, plot:GetBuildProgress( buildID ) ) ) / workRate )
					if g_isNoob then
						buildTip = buildTip .. " (" .. Locale.ToLower( L( "TXT_KEY_STR_TURNS", turnsRemaining ) ) .. ")"
					else
						buildTip = buildTip .. " (" .. turnsRemaining .. ")"
					end
				end

				-- repair special case
				if build.Repair then
					buildImprovement = revealedImprovement
					isBasicBuild = true
					-- can always repair but not an unrevealed improvement
					canBuild = revealedImprovement
				end

				if canBuild or buildInProgress then
					-- Determine yield changes from this build
					canBuild = false
					for yieldID = 0, YieldTypes.NUM_YIELD_TYPES-1 do -- GameInfo.Yields() iterator is broken by Communitas
						local yieldChange
						-- Work around unrevealed improvement game bug
						if buildImprovement and revealedImprovementID ~= actualImprovementID then
							yieldChange = plot:CalculateImprovementYieldChange( buildImprovement.ID, yieldID, activePlayerID ) -- false = not optimal
						else
							yieldChange = plot:GetYieldWithBuild( buildID, yieldID, false, activePlayerID ) - plot:CalculateYield( yieldID, false ) -- false = without upgrade, false = actual
						end

						-- Positive or negative change?
						if yieldChange > 0 then
							buildTip = S( "%s [COLOR_POSITIVE_TEXT]%+i[ENDCOLOR]%s", buildTip, yieldChange, YieldIcons[yieldID] or "" )
							canBuild = true
						elseif yieldChange < 0 then
							buildTip = S( "%s [COLOR_NEGATIVE_TEXT]%+i[ENDCOLOR]%s", buildTip, yieldChange, YieldIcons[yieldID] or "" )
						end
					end
					-- Maintenance
					if buildImprovement then
						if civ5_mode then
							if (tonumber(buildImprovement.GoldMaintenance) or 0) > 0 then
								buildTip = S("%s [COLOR_NEGATIVE_TEXT]%+i[ENDCOLOR][ICON_GOLD]", buildTip, -buildImprovement.GoldMaintenance )
							end
						else
							if (tonumber(buildImprovement.Health) or 0) ~= 0 then
								buildTip = S("%s [COLOR_POSITIVE_TEXT]%+i[ENDCOLOR][ICON_HEALTH_1]", buildTip, buildImprovement.Health )
							end
							if (tonumber(buildImprovement.EnergyMaintenance) or 0) ~= 0 then
								buildTip = S("%s [COLOR_NEGATIVE_TEXT]%+i[ENDCOLOR][ICON_ENERGY]", buildTip, -buildImprovement.EnergyMaintenance )
							end
							if (tonumber(buildImprovement.Unhealth) or 0) ~= 0 then
								buildTip = S("%s [COLOR_NEGATIVE_TEXT]%+i[ENDCOLOR][ICON_HEALTH_4]", buildTip, -buildImprovement.Unhealth )
							end
						end
						if resource and isResourceUsefull then
							if plot:IsResourceConnectedByImprovement( buildImprovement.ID ) then
								if not isResourceConnected then
									buildTip = S("%s [COLOR_POSITIVE_TEXT]%+i[ENDCOLOR]%s", buildTip, numResource, resource.IconString )
								end
							elseif isResourceConnected then
								buildTip = S("%s [COLOR_NEGATIVE_TEXT]%+i[ENDCOLOR]%s", buildTip, -numResource, resource.IconString )
							end
						end
					end

					-- Determine possible further yields from this build
					if isExtraTips then
						local yieldTips = table()
						insertImprovementYieldChanges( yieldTips, buildImprovement, "[NEWLINE]      " )
						buildTip = "[ICON_MINUS]" .. buildTip .. yieldTips:concat()
					else
						buildTip = "[ICON_PLUS]" .. buildTip
					end
				end

				if buildInProgress or (canBuild and (isBasicBuild or (g_isNoob and isExtraTips))) then
					tips:insert( buildTip )
				end
			end
		end

		-- City state quest
		if civ5_mode and revealedImprovementID == GameInfoTypes.IMPROVEMENT_BARBARIAN_CAMP then
			insertCityStateQuest( tips, MinorCivQuestTypes.MINOR_CIV_QUEST_KILL_CAMP, "TXT_KEY_CITY_STATE_BARB_QUEST_LONG" )
		-- Compatibility with Gazebo's City-State Diplomacy Mod (CSD) for Brave New World
		elseif csd_mode and revealedImprovementID == GameInfoTypes.IMPROVEMENT_ARCHAEOLOGICAL_DIG then
			insertCityStateQuest( tips, MinorCivQuestTypes.MINOR_CIV_QUEST_ARCHAEOLOGY, "TXT_KEY_CITY_STATE_ARCHAEOLOGY_QUEST_LONG" )
		end
	end
	return tips:concat( "[NEWLINE]" )
end

-------------------------------------------------
-- Events & Initialisation
-------------------------------------------------

local function ResetTimer()
	g_tipTimer = 0
	if g_tipLevel ~= 0 then
		g_tipLevel = 0
		if g_tipTimerThreshold1 > 0 then
			Controls_TheBox:EnableToolTip( false )
			return ClearOverlays()
		end
	end
end

-------------------------------------------------
local function ResetAll()
	g_lastPlot = false
	ResetTimer()
end

-------------------------------------------------
local function DirtyAll()
	g_lastPlot = false
	g_tipLevel = -1
end

-------------------------------------------------
local function UpdateOptions()

	g_tipTimerThreshold1 = OptionsManager.GetTooltip1Seconds() / 100
	g_tipTimerThreshold2 = OptionsManager.GetTooltip2Seconds() / 100 + g_tipTimerThreshold1
	g_isCivilianYields = not OptionsManager.IsCivilianYields()
	g_isScienceEnabled = not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE)
	g_isPoliciesEnabled = not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_POLICIES)
	g_isHappinessEnabled = civ5_mode and not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_HAPPINESS)
	g_isReligionEnabled = civ5_mode and gk_mode and not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)
	g_isOptionDebugMode = OptionsManager.IsDebugMode()
	g_isNoob = civ5_mode and not OptionsManager.IsNoBasicHelp()
	ResetAll()
end
Events.GameOptionsChanged.Add( UpdateOptions )
UpdateOptions()

-------------------------------------------------
ContextPtr:SetInputHandler(
function( uiMsg ) --, wParam, lParam )
	if uiMsg == MouseEvents_MouseMove then
		local x, y = UIManager:GetMouseDelta()
		if x ~= 0 or y ~= 0 then
			return ResetTimer()
		end
	end
end)

-------------------------------------------------

ResetAll()
Events.CameraViewChanged.Add( ResetTimer )
if civ5_mode then
	Events.StrategicViewStateChanged.Add( ResetTimer )
end
Events.SerialEventUnitInfoDirty.Add( DirtyAll )
Events.WorldMouseOver( true )
ContextPtr:SetUpdate(
function( timeChange )
	if g_tipLevel > 1 then
		return
	else
		g_tipTimer = g_tipTimer + timeChange
		if IsGameCoreBusy() then
			return
		elseif g_tipTimer >= g_tipTimerThreshold2 then
			g_tipLevel = 2
		elseif g_tipTimer >= g_tipTimerThreshold1 then
			g_tipLevel = 1
		else
			return
		end
	end
	local plot = Map_GetPlot( UI_GetMouseOverHex() )
	local plotTips
	if plot == g_lastPlot then
		plotTips = g_lastTips[ g_tipLevel ]
	else
		g_lastTips = { false, false, false }
		g_lastPlot = plot
	end
	if civ5_mode and MouseOverStrategicViewResource() then
		-- Resource Tool Tip
		plotTips = g_lastTips[ 3 ]
		if not plotTips then
			plotTips = plot and plot:IsRevealed( Game.GetActiveTeam(), false ) and GenerateResourceToolTip( plot )
			g_lastTips[ 3 ] = plotTips
		end
	elseif not plotTips then
		ClearOverlays()
		plotTips = PlotToolTips( plot, g_tipLevel > 1 )
		g_lastTips[ g_tipLevel ] = plotTips
	end
	return ShowPlotTips( plotTips )
end)

print("Finished loading EUI plot help",os.clock())
end)
