-------------------------------------------------
-- CityBannerManager
-- modified by bc1 from 1.0.3.144 brave new world code
-- code is common using gk_mode and bnw_mode switches
-------------------------------------------------
Events.SequenceGameInitComplete.Add(function()
print("Loading EUI city banners...",os.clock(),[[ 
  ____ _ _         ____                              __  __                                   
 / ___(_) |_ _   _| __ )  __ _ _ __  _ __   ___ _ __|  \/  | __ _ _ __   __ _  __ _  ___ _ __ 
| |   | | __| | | |  _ \ / _` | '_ \| '_ \ / _ \ '__| |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
| |___| | |_| |_| | |_) | (_| | | | | | | |  __/ |  | |  | | (_| | | | | (_| | (_| |  __/ |   
 \____|_|\__|\__, |____/ \__,_|_| |_|_| |_|\___|_|  |_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|   
             |___/                                                            |___/           
]])
include( "EUI_utilities" )
local IconHookup = EUI.IconHookup
local CivIconHookup = EUI.CivIconHookup
local CityPlots = EUI.CityPlots
local Color = EUI.Color
local GameInfo = EUI.GameInfoCache -- warning! use iterator ONLY with table field conditions, NOT string SQL query
include( "CityStateStatusHelper" )
local GetCityStateStatusRow = GetCityStateStatusRow
local GetCityStateStatusType = GetCityStateStatusType
local UpdateCityStateStatusUI = UpdateCityStateStatusUI
local GetCityStateStatusToolTip = GetCityStateStatusToolTip
local GetAllyToolTip = GetAllyToolTip
local GetActiveQuestText = GetActiveQuestText
local GetActiveQuestToolTip = GetActiveQuestToolTip
include( "EUI_tooltips" )
local GetHelpTextForUnit = GetHelpTextForUnit
local GetHelpTextForBuilding = GetHelpTextForBuilding
local GetHelpTextForProject = GetHelpTextForProject
local GetHelpTextForProcess = GetHelpTextForProcess
local GetMoodInfo = GetMoodInfo
local GetReligionTooltip = GetReligionTooltip

-------------------------------
-- minor lua optimizations
-------------------------------

local math = math
--local os = os
local pairs = pairs
local ipairs = ipairs
--local pcall = pcall
--local print = print
--local select = select
--local string = string
--local table = table
--local tonumber = tonumber
--local tostring = tostring
--local type = type
--local unpack = unpack

local UI = UI
--local UIManager = UIManager
local Controls = Controls
local ContextPtr = ContextPtr
local Players = Players
local Teams = Teams
--local GameInfo = GameInfo
--local GameInfoActions = GameInfoActions
--local GameInfoTypes = GameInfoTypes
local GameDefines = GameDefines
--local InterfaceDirtyBits = InterfaceDirtyBits
local CityUpdateTypes = CityUpdateTypes
local ButtonPopupTypes = ButtonPopupTypes
local YieldTypes = YieldTypes
local GameOptionTypes = GameOptionTypes
--local DomainTypes = DomainTypes
--local FeatureTypes = FeatureTypes
--local FogOfWarModeTypes = FogOfWarModeTypes
--local OrderTypes = OrderTypes
--local PlotTypes = PlotTypes
--local TerrainTypes = TerrainTypes
local InterfaceModeTypes = InterfaceModeTypes
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
local CityAIFocusTypes = CityAIFocusTypes
--local AdvisorTypes = AdvisorTypes
--local GenericWorldAnchorTypes = GenericWorldAnchorTypes
--local GameStates = GameStates
--local GameplayGameStateTypes = GameplayGameStateTypes
--local CombatPredictionTypes = CombatPredictionTypes
--local ChatTargetTypes = ChatTargetTypes
--local ReligionTypes = ReligionTypes
--local BeliefTypes = BeliefTypes
local FaithPurchaseTypes = FaithPurchaseTypes
--local ResolutionDecisionTypes = ResolutionDecisionTypes
--local InfluenceLevelTypes = InfluenceLevelTypes
--local InfluenceLevelTrend = InfluenceLevelTrend
--local PublicOpinionTypes = PublicOpinionTypes
--local ControlTypes = ControlTypes

local PreGame = PreGame
local Game = Game
local Map = Map
local OptionsManager = OptionsManager
local Events = Events
local Mouse = Mouse
--local MouseEvents = MouseEvents
--local MouseOverStrategicViewResource = MouseOverStrategicViewResource
local Locale = Locale
local L = Locale.ConvertTextKey
local S = string.format
--getmetatable("").__index.L = L
local InStrategicView = InStrategicView

-------------------------------
-- Globals
-------------------------------
local gk_mode = Game.GetReligionName ~= nil
local bnw_mode = Game.GetActiveLeague ~= nil
local civ5_mode = InStrategicView ~= nil
local civBE_mode = Game.GetAvailableBeliefs ~= nil

local g_activePlayerID = Game.GetActivePlayer()
local g_activePlayer = Players[ g_activePlayerID ]
local g_activeTeamID = Game.GetActiveTeam()
local g_activeTeam = Teams[ g_activeTeamID ]

local g_cityBanners = {}
local g_outpostBanners = {}
local g_stationBanners = {}
local g_svStrikeButtons = {}

local g_scrapTeamBanners = {}
local g_scrapOtherBanners = {}
local g_scrapSVStrikeButtons = {}

local g_WorldPositionOffsetZ = civ5_mode and 35 or 55

local BlackFog = 0	-- invisible
local GreyFog = 1	-- once seen
local WhiteFog = 2	-- eyes on

local g_colorWhite = Color( 1, 1, 1, 1 )
local g_colorGreen = Color( 0, 1, 0, 1 )
local g_colorYellow = Color( 1, 1, 0, 1 )
local g_colorRed = Color( 1, 0, 0, 1 )
local g_colorCulture = Color( 1, 0, 1, 1 )
local g_colorShadowBlack = Color( 0, 0, 0, 0.7 )
local g_primaryColors = EUI.PrimaryColors
local g_backgroundColors = EUI.BackgroundColors

local g_cityHexHighlight, g_toolTipControl, g_toolTipPlotIndex
local g_toolTipHandler = {}

local g_dirtyCityBanners = {}
local g_missingCityBanners = {}

local defaultErrorTextureSheet = "CityBannerProductionImage.dds"
local nullOffset = Vector2( 0, 0 )

local g_CovertOpsBannerContainer = civBE_mode and ContextPtr:LookUpControl( "../CovertOpsBannerContainer" )
local g_CovertOpsIntelReportContainer = civBE_mode and ContextPtr:LookUpControl( "../CovertOpsIntelReportContainer" )

local g_CityFocus = {
[CityAIFocusTypes.NO_CITY_AI_FOCUS_TYPE] =		{"",			L("TXT_KEY_CITYVIEW_FOCUS_BALANCED_TEXT")},
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_FOOD] =		{"[ICON_FOOD]",		L("TXT_KEY_CITYVIEW_FOCUS_FOOD_TEXT")},
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_PRODUCTION] =	{"[ICON_PRODUCTION]",	L("TXT_KEY_CITYVIEW_FOCUS_PROD_TEXT")},
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_GOLD] =		{"[ICON_GOLD]",		L("TXT_KEY_CITYVIEW_FOCUS_GOLD_TEXT")},
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_SCIENCE] =		{"[ICON_RESEARCH]",	L("TXT_KEY_CITYVIEW_FOCUS_RESEARCH_TEXT")},
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_CULTURE] =		{"[ICON_CULTURE]",	L("TXT_KEY_CITYVIEW_FOCUS_CULTURE_TEXT")},
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_GREAT_PEOPLE] =	{"[ICON_GREAT_PEOPLE]",	L("TXT_KEY_CITYVIEW_FOCUS_GREAT_PERSON_TEXT")}
}
if gk_mode then
	g_CityFocus[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_FAITH] = {"[ICON_PEACE]",	L("TXT_KEY_CITYVIEW_FOCUS_FAITH_TEXT")}
end

local function IsTurnActive( player )
	return player and player:IsTurnActive() and not Game.IsProcessingMessages()
end

local function BannerError( n, arg )
	if Game.IsDebugMode() then
		local txt = ""
		if arg and arg.PlotIndex then
			txt = "city banner"
			arg = arg and Map.GetPlotByIndex(arg.PlotIndex)
		end
		if arg and arg.GetPlotCity then
			txt = "plot " .. (arg:IsCity() and "with" or "without") .. " city"
			arg = arg:GetPlotCity()
		end
		if arg and arg.GetCityPlotIndex then
			txt = "city " .. arg:GetName()
		end
		print( "glitch", n, txt, debug and debug.traceback and debug.traceback() )
	end
end

local RefreshCityBanner

------------------------------------------------------------
-- CityBanner ToolTip
------------------------------------------------------------
function g_toolTipHandler.BannerButton( control )
	local plotIndex = control:GetVoid1()
	local plot = Map.GetPlotByIndex( plotIndex )
	local city = plot and plot:GetPlotCity()
	if city then
		local tipText = ""
		local cityOwnerID = city:GetOwner()
		local cityOwner = Players[ cityOwnerID ]
		if cityOwner then
			local cityTeamID = cityOwner:GetTeam()

			-- city resources
			resources = {}
			for plot in CityPlots( city ) do
				if plot	and plot:IsRevealed( g_activeTeamID )
				then
					local resourceID = plot:GetResourceType( g_activeTeamID )
					local numResource = plot:GetNumResource()
					if numResource > 0 then
						if not plot:IsCity() and (plot:IsImprovementPillaged() or not plot:IsResourceConnectedByImprovement( plot:GetImprovementType() )) then
							numResource = numResource / 65536
						end
						resources[ resourceID ] = ( resources[ resourceID ] or 0 ) + numResource
					end
				end
			end
			for resourceID, numResource in pairs(resources) do
				local resource = GameInfo.Resources[ resourceID ]
				if resource then
					local numConnected, numNotConnected = math.modf( numResource )
					numNotConnected = numNotConnected * 65536
					local usageID = Game.GetResourceUsageType( resourceID )
					if usageID == ResourceUsageTypes.RESOURCEUSAGE_STRATEGIC
						or usageID == ResourceUsageTypes.RESOURCEUSAGE_LUXURY
					then
						if numConnected > 0 then
							tipText = tipText .. " [COLOR_POSITIVE_TEXT]" .. numConnected .. "[ENDCOLOR]" .. resource.IconString
						end
						if numNotConnected > 0 then
							tipText = tipText .. " [COLOR_WARNING_TEXT]" .. numNotConnected .. "[ENDCOLOR]" .. resource.IconString
						end
					end
				end
			end

			if cityTeamID == g_activeTeamID then

				local cultureStored = city:GetJONSCultureStored()
				local cultureNeeded = city:GetJONSCultureThreshold()
				local culturePerTurn = city:GetJONSCulturePerTurn()
				local turnsRemaining
				if culturePerTurn > 0 then
					turnsRemaining = math.max(math.ceil((cultureNeeded - cultureStored ) / culturePerTurn), 1)
					tipText = city:GetName() .. tipText .. "[NEWLINE][COLOR_MAGENTA]" .. L("TXT_KEY_CITYVIEW_TURNS_TILL_TILE_TEXT", turnsRemaining ) .. "[ENDCOLOR]"
				else
					tipText = city:GetName() .. tipText .. " 0 [ICON_CULTURE]"
					turnsRemaining = 99999
				end

--[[ too much stuff ?
				-- City happiness
				if not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_HAPPINESS) then
					local cityHappiness = city:GetHappiness() + cityOwner:GetExtraHappinessPerCity()
					if cityOwner:IsCapitalConnectedToCity(city) and not city:IsCapital() then
						cityHappiness = cityHappiness + cityOwner:GetHappinessPerTradeRoute() / 100	-- workaround vanilla Civ bug: have to divide by 100 (now in gk code)
					end
					if gk_mode then
						cityHappiness = cityHappiness + city:GetLocalHappiness()
					elseif city:GetGarrisonedUnit() then
						cityHappiness = cityHappiness + cityOwner:GetHappinessPerGarrisonedUnit()
					end
					tipText = tipText .. S(" %g[ICON_HAPPINESS_1] %+g[ICON_HAPPINESS_4]", cityHappiness, -cityOwner:GetUnhappinessFromCityForUI(city) / 100 )
				end

				-- City Culture
				tipText = tipText .. "[NEWLINE][NEWLINE][COLOR_MAGENTA]" .. L("TXT_KEY_CITYVIEW_CULTURE_TEXT")
					.. "[ENDCOLOR] " .. L("TXT_KEY_CITYVIEW_TURNS_TILL_TILE_TEXT", turnsRemaining )
					.. S("[NEWLINE]%g [COLOR_MAGENTA]%+g[ICON_CULTURE][ENDCOLOR]x%g = %g / %g", cultureStored, culturePerTurn, turnsRemaining, turnsRemaining * culturePerTurn +cultureStored, cultureNeeded)	--:231:0:231:255

				-- City Culture Empire modifier
				local iAmount = cityOwner:GetCultureCityModifier()
				if iAmount ~= 0 then
					tipText = tipText .. "[NEWLINE][ICON_BULLET]" .. L("TXT_KEY_CULTURE_PLAYER_MOD", iAmount)
				end

				-- City Culture modifier
				local iAmount = city:GetCultureRateModifier()
				if iAmount ~= 0 then
					tipText = tipText .. L("TXT_KEY_PRODMOD_YIELD", iAmount)
				end

				-- City Culture Wonders modifier
				if city:GetNumWorldWonders() > 0 then
					local iAmount = cityOwner:GetCultureWonderMultiplier()
					if iAmount ~= 0 then
						tipText = tipText .. "[NEWLINE][ICON_BULLET]" .. L("TXT_KEY_CULTURE_WONDER_BONUS", iAmount)
					end
				end

				-- City Gold
				tipText = tipText .. "[NEWLINE][NEWLINE][COLOR_YIELD_GOLD]" .. L("TXT_KEY_CITYVIEW_GOLD_TEXT")
					.. S(" %+g[ICON_GOLD][ENDCOLOR]", city:GetYieldRateTimes100(YieldTypes.YIELD_GOLD) / 100) --:231:213:0:255
				if not city:IsBlockaded() and not city:IsCapital() and cityOwner:IsCapitalConnectedToCity(city) then
					if cityOwner.GetRouteGoldTimes100 then
						tipText = tipText .. S(" %+g[ICON_CONNECTED]", cityOwner:GetRouteGoldTimes100( city ) / 100 )
					elseif cityOwner.GetCityConnectionRouteGoldTimes100 then
						tipText = tipText .. S(" %+g[ICON_CONNECTED]", cityOwner:GetCityConnectionRouteGoldTimes100( city ) / 100 )
					end
				end

				local BuildingCost = city:GetTotalBaseBuildingMaintenance()
				if BuildingCost ~= 0 then
					tipText = tipText .. S(" %+g ", - BuildingCost ) .. L("TXT_KEY_REPLAY_DATA_BUILDINGMAINTENANCE")
				end

				tipText = tipText .. city:GetYieldModifierTooltip(YieldTypes.YIELD_GOLD)

				-- City Science
				if not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE) then
					tipText = tipText .. "[NEWLINE][NEWLINE][COLOR_CYAN]" .. L("TXT_KEY_CITYVIEW_RESEARCH_TEXT")
						.. S(" %+g[ICON_RESEARCH][ENDCOLOR]", city:GetYieldRateTimes100(YieldTypes.YIELD_SCIENCE) / 100) --:33:190:247:255
						.. city:GetYieldModifierTooltip(YieldTypes.YIELD_SCIENCE)
				end
--]]
				if gk_mode and city:GetReligiousMajority() < 0 then
					tipText = tipText .. "[NEWLINE]" .. GetReligionTooltip(city)
				end

				if not OptionsManager.IsNoBasicHelp() then
					if cityOwnerID == g_activePlayerID then
						tipText = tipText .. "[NEWLINE]" .. L("TXT_KEY_CITY_ENTER_CITY_SCREEN")
					else
						tipText = tipText .. "[NEWLINE]" .. L("TXT_KEY_CITY_TEAMMATE")
					end
				end

			elseif g_activeTeam:IsHasMet( cityTeamID ) then

				-- Other player we have met

				if gk_mode and city:GetReligiousMajority() < 0 then
					tipText = tipText .. "[NEWLINE]" .. GetReligionTooltip(city)
				end

				if cityOwner:IsMinorCiv() then
					tipText = L("TXT_KEY_CITY_OF", "", city:GetName() ) .. tipText .. "[NEWLINE][NEWLINE]"
						.. GetCityStateStatusToolTip( g_activePlayerID, cityOwnerID, true )
				else
					tipText = L("TXT_KEY_CITY_OF", cityOwner:GetCivilizationAdjectiveKey(), city:GetName() )
						.. tipText .. "[NEWLINE][NEWLINE]" .. GetMoodInfo(cityOwnerID, true)
				end

				if bnw_mode and g_activeTeam:IsAtWar(cityTeamID) then
					tipText = tipText .. "[NEWLINE]----------------[NEWLINE]"
							.. cityOwner:GetWarmongerPreviewString(city:GetOwner())
					if city:GetOriginalOwner() ~= city:GetOwner() then
						tipText = tipText .. "[NEWLINE]----------------[NEWLINE]"
								.. cityOwner:GetLiberationPreviewString(city:GetOriginalOwner())
					end
				elseif Game.IsOption( GameOptionTypes.GAMEOPTION_ALWAYS_WAR ) then
					tipText = tipText .. "[NEWLINE]" .. L("TXT_KEY_ALWAYS_AT_WAR_WITH_CITY")
				end

				if not OptionsManager.IsNoBasicHelp() then
					tipText = tipText .. "[NEWLINE][NEWLINE]" .. L("TXT_KEY_TALK_TO_PLAYER")
				end
			else
				-- Players we have not met
				tipText = L("TXT_KEY_HAVENT_MET")
			end
		end
		control:SetToolTipString( tipText )
	else
		BannerError( 354, plot )
	end
end

------------------------------------------------------------
-- City Production ToolTip
------------------------------------------------------------
function g_toolTipHandler.CityBannerProductionButton( control )
	local plotIndex = control:GetVoid1()
	local plot = Map.GetPlotByIndex( plotIndex )
	local city = plot and plot:GetPlotCity()

	if city then
		local cityName = city:GetName()
		local isNoob = not OptionsManager.IsNoBasicHelp()
		local cityProductionName = city:GetProductionNameKey()
		local tipText = ""
		local isProducingSomething = true

		local productionPerTurn100 = city:GetCurrentProductionDifferenceTimes100(false, false)	-- food = false, overflow = false
		local productionStored100 = city:GetProductionTimes100() + city:GetCurrentProductionDifferenceTimes100(false, true) - productionPerTurn100
		local productionNeeded = 0
		local productionTurnsLeft = 1

		local unitProductionID = city:GetProductionUnit()
		local buildingProductionID = city:GetProductionBuilding()
		local projectProductionID = city:GetProductionProject()
		local processProductionID = city:GetProductionProcess()

		if unitProductionID ~= -1 then
--			tipText = GetHelpTextForUnit( unitProductionID, false )

		elseif buildingProductionID ~= -1 then
--			tipText = GetHelpTextForBuilding( buildingProductionID, true, true, true, city )

		elseif projectProductionID ~= -1 then
--			tipText = GetHelpTextForProject( projectProductionID, false )

		elseif processProductionID ~= -1 then
			tipText = GetHelpTextForProcess( processProductionID, false )
			isProducingSomething = false
		else
			if isNoob then
				tipText = L( "TXT_KEY_CITY_NOT_PRODUCING", cityName )
			else
				tipText = L"TXT_KEY_PRODUCTION_NO_PRODUCTION"
			end
			isProducingSomething = false
		end

		if isProducingSomething then
			productionNeeded = city:GetProductionNeeded()
			productionTurnsLeft = city:GetProductionTurnsLeft()
			tipText = "[COLOR_YIELD_FOOD]" .. Locale.ToUpper( cityProductionName ) .. "[ENDCOLOR]"
			if isNoob then
				tipText = L( "TXT_KEY_PROGRESS_TOWARDS", tipText )
			end
			tipText = tipText .. "  " .. productionStored100 / 100 .. "[ICON_PRODUCTION]/ " .. productionNeeded .. "[ICON_PRODUCTION]"
			if productionPerTurn100 > 0 then

				tipText = tipText .. "[NEWLINE]"

				local productionOverflow100 = productionPerTurn100 * productionTurnsLeft + productionStored100 - productionNeeded * 100
				if productionTurnsLeft > 1 then
					tipText = tipText .. L( "TXT_KEY_STR_TURNS", productionTurnsLeft -1 )
						.. S( " %+g[ICON_PRODUCTION]  ", ( productionOverflow100 - productionPerTurn100 ) / 100 )
				end
				tipText = tipText .. "[COLOR_YIELD_FOOD]" .. Locale.ToUpper( L( "TXT_KEY_STR_TURNS", productionTurnsLeft ) ) .. "[ENDCOLOR]"
						.. S( " %+g[ICON_PRODUCTION]", productionOverflow100 / 100 )
			end
		end

		control:SetToolTipString( tipText )
	else
		BannerError( 431, plot )
	end
end

------------------------------------------------------------
-- City Growth ToolTip
------------------------------------------------------------
function g_toolTipHandler.PopulationBox( control )
	local plotIndex = control:GetVoid1()
	local plot = Map.GetPlotByIndex( plotIndex )
	local city = plot and plot:GetPlotCity()

	if city then
		local tipText
		local isNoob = not OptionsManager.IsNoBasicHelp()
		local cityName = city:GetName()
		local cityPopulation = math.floor(city:GetPopulation())

		local foodStoredTimes100 = city:GetFoodTimes100()
		local foodPerTurnTimes100 = city:FoodDifferenceTimes100( true )	-- true means size 1 city cannot starve
		local foodThreshold = city:GrowthThreshold()

		local turnsToCityGrowth = city:GetFoodTurnsLeft()

		if foodPerTurnTimes100 < 0 then
			foodThreshold = 0
			turnsToCityGrowth = math.floor( foodStoredTimes100 / -foodPerTurnTimes100 ) + 1
			tipText = "[COLOR_WARNING_TEXT]" .. cityPopulation - 1 .. "[ENDCOLOR][ICON_CITIZEN]"
			if isNoob then
				tipText = L( "TXT_KEY_CITY_STARVING_TT", cityName ) .. "[NEWLINE]"
				.. L( "TXT_KEY_PROGRESS_TOWARDS", tipText )
			end
		elseif city:IsForcedAvoidGrowth() then
			tipText = "[ICON_LOCKED]".. cityPopulation .."[ICON_CITIZEN]"
			if isNoob then
				tipText = L"TXT_KEY_CITYVIEW_FOCUS_AVOID_GROWTH_TEXT" .. " " .. tipText
			end
			foodPerTurnTimes100 = 0
		elseif foodPerTurnTimes100 == 0 then
			tipText = "[COLOR_YELLOW]" .. cityPopulation .. "[ENDCOLOR][ICON_CITIZEN]"
			if isNoob then
				tipText = L( "TXT_KEY_PROGRESS_TOWARDS", tipText )
			end
		else
			tipText = "[COLOR_POSITIVE_TEXT]" .. cityPopulation +1 .. "[ENDCOLOR][ICON_CITIZEN]"
			if isNoob then
				tipText = L( "TXT_KEY_PROGRESS_TOWARDS", tipText )
			end
		end

		tipText = tipText .. "  " .. foodStoredTimes100 / 100 .. "[ICON_FOOD]/ " .. foodThreshold .. "[ICON_FOOD][NEWLINE]"

		if foodPerTurnTimes100 == 0 then

			tipText = tipText .. L"TXT_KEY_CITYVIEW_STAGNATION_TEXT"
		else
			local foodOverflowTimes100 = foodPerTurnTimes100 * turnsToCityGrowth + foodStoredTimes100 - foodThreshold * 100

			if turnsToCityGrowth > 1 then
				tipText = tipText .. L( "TXT_KEY_STR_TURNS", turnsToCityGrowth -1 )
						.. S( " %+g[ICON_FOOD]  ", ( foodOverflowTimes100 - foodPerTurnTimes100 ) / 100 )
			end
			tipText = tipText .. ( foodPerTurnTimes100 < 0 and "[COLOR_WARNING_TEXT]" or "[COLOR_POSITIVE_TEXT]" )
					.. Locale.ToUpper( L( "TXT_KEY_STR_TURNS", turnsToCityGrowth ) ) .. "[ENDCOLOR]"
					.. S( " %+g[ICON_FOOD]", foodOverflowTimes100 / 100 )
		end

		control:SetToolTipString( tipText )
	else
		BannerError( 503, plot )
	end
end

-------------------------------------------------
-- City Religion Tooltip
-------------------------------------------------

function g_toolTipHandler.ReligiousIcon( control )
	local plotIndex = control:GetVoid1()
	local plot = Map.GetPlotByIndex( plotIndex )
	local city = plot and plot:GetPlotCity()

	if city then
		control:SetToolTipString( GetReligionTooltip(city) )
	else
		BannerError( 522, plot )
	end
end

-------------------------------------------------
-- Clear Hex Highlighting
-------------------------------------------------
local function ClearHexHighlights()
	Events.ClearHexHighlightStyle( "Culture" )
	Events.ClearHexHighlightStyle( "WorkedFill" )
	Events.ClearHexHighlightStyle( "WorkedOutline" )
	Events.ClearHexHighlightStyle( "OwnedFill")
	Events.ClearHexHighlightStyle( "OwnedOutline" )
	Events.ClearHexHighlightStyle( "CityLimits" )
	g_cityHexHighlight = false
end

-------------------------------------------------
-- Show/hide the garrison frame icon
-------------------------------------------------
local function HideGarrisonFrame( cityBanner, isHide )
	-- Only the active team has a Garrison ring
	if cityBanner and cityBanner[1] then
		cityBanner.GarrisonFrame:SetHide( isHide )
	end
end

-------------------------------------------------
-- Show/hide the range strike icon
-------------------------------------------------
local function UpdateRangeIcons( plotIndex, city, cityBanner )

	if city and cityBanner then

		local hideRangeStrikeButton = city:GetOwner() ~= g_activePlayerID or not city:CanRangeStrikeNow()
		if cityBanner.CityRangeStrikeButton then
			cityBanner.CityRangeStrikeButton:SetHide( hideRangeStrikeButton )
		end

		local svStrikeButton = g_svStrikeButtons[ plotIndex ]
		if svStrikeButton then
			svStrikeButton.CityRangeStrikeButton:SetHide( hideRangeStrikeButton )
		end
	end
end

-------------------------------------------------
-- Refresh the City Damage bar
-------------------------------------------------
local function RefreshCityDamage( city, cityBanner, cityDamage )

	if cityBanner then

		local maxCityHitPoints = GameDefines.MAX_CITY_HIT_POINTS
		if gk_mode and city then
			maxCityHitPoints = city:GetMaxHitPoints()
		end

		local iHealthPercent = 1 - cityDamage / maxCityHitPoints

		cityBanner.CityBannerHealthBar:SetPercent(iHealthPercent)
		cityBanner.CityBannerHealthBar:SetToolTipString( S("%g / %g", maxCityHitPoints - cityDamage, maxCityHitPoints) )

		---- Health bar color based on amount of damage
		local barColor = {}

		if iHealthPercent > 0.66 then
			barColor = g_colorGreen
		elseif iHealthPercent > 0.33 then
			barColor = g_colorYellow
		else
			barColor = g_colorRed
		end
		cityBanner.CityBannerHealthBar:SetFGColor( barColor )

		-- Show or hide the Health Bar as necessary
		cityBanner.CityBannerHealthBarBase:SetHide( cityDamage == 0 )
	end
end

-------------------------------------------------
-- Click On City State Quest Info
-------------------------------------------------
local questKillCamp = MinorCivQuestTypes.MINOR_CIV_QUEST_KILL_CAMP
local IsActiveQuestKillCamp
if bnw_mode then
	IsActiveQuestKillCamp = function( minorPlayer )
		return minorPlayer and minorPlayer:IsMinorCivDisplayedQuestForPlayer( g_activePlayerID, questKillCamp )
	end
elseif gk_mode then
	IsActiveQuestKillCamp = function( minorPlayer )
		return minorPlayer and minorPlayer:IsMinorCivActiveQuestForPlayer( g_activePlayerID, questKillCamp )
	end
else
	IsActiveQuestKillCamp = function( minorPlayer )
		return minorPlayer and minorPlayer:GetActiveQuestForPlayer( g_activePlayerID ) == questKillCamp
	end
end

local function OnQuestInfoClicked( plotIndex )
	local plot = Map.GetPlotByIndex( plotIndex )
	local city = plot and plot:GetPlotCity()
	local cityOwner = city and Players[ city:GetOwner() ]
	if cityOwner and cityOwner:IsMinorCiv() and IsActiveQuestKillCamp( cityOwner ) then
		local questData1 = cityOwner:GetQuestData1( g_activePlayerID, questKillCamp )
		local questData2 = cityOwner:GetQuestData2( g_activePlayerID, questKillCamp )
		local plot = Map.GetPlot( questData1, questData2 )
		if plot then
			UI.LookAt( plot, 0 )
			local hex = ToHexFromGrid{ x=plot:GetX(), y=plot:GetY() }
			Events.GameplayFX( hex.x, hex.y, -1 )
		end
	end
end

local function AnnexPopup( plotIndex )
	local plot = Map.GetPlotByIndex( plotIndex )
	local city = plot and plot:GetPlotCity()
	if city and city:GetOwner() == g_activePlayerID and not( bnw_mode and g_activePlayer:MayNotAnnex() ) then
		Events.SerialEventGameMessagePopup{
			Type = ButtonPopupTypes.BUTTONPOPUP_ANNEX_CITY,
			Data1 = city:GetID(),
			Data2 = -1,
			Data3 = -1,
			Option1 = false,
			Option2 = false
		}
	end
end

local function EspionagePopup()
	Events.SerialEventGameMessagePopup{ 
		Type = ButtonPopupTypes.BUTTONPOPUP_ESPIONAGE_OVERVIEW,
	}
end

-------------------------------------------------
-- Click on City Range Strike Button
-------------------------------------------------
local function OnCityRangeStrikeButtonClick( plotIndex )
	local plot = Map.GetPlotByIndex( plotIndex )
	local city = plot and plot:GetPlotCity()

	if city and city:GetOwner() == g_activePlayerID then
		UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_CITY_RANGE_ATTACK)
		UI.ClearSelectionList()
		UI.SelectCity( city )

		Events.InitCityRangeStrike( city:GetOwner(), city:GetID() )
	end
end

-------------------------------------------------
-- Right Click on production button
-------------------------------------------------

local function OnProdRClick( plotIndex )
	local plot = Map.GetPlotByIndex( plotIndex )
	local city = plot and plot:GetPlotCity()

	if city and city:GetTeam() == g_activeTeamID then
		Events.SearchForPediaEntry( city:GetProductionNameKey() )
	else
		BannerError( 737, plot )
	end
end

-------------------------------------------------
-- Left Click on city banner
-------------------------------------------------
local function OnBannerClick( plotIndex )
	local plot = Map.GetPlotByIndex( plotIndex )
	local city = plot and plot:GetPlotCity()
	if city then
		local cityOwnerID = city:GetOwner()
		local cityOwner = Players[ cityOwnerID ]

		-- Active player city
		if cityOwnerID == g_activePlayerID then

			-- always open city screen, puppets are not that special
			ClearHexHighlights()
			UI.DoSelectCityAtPlot( plot )

		-- Observers get to look at anything
		elseif g_activePlayer:IsObserver() then
			UI.SelectCity( city )
			UI.LookAt( plot )
			UI.SetCityScreenUp( true )

		-- Other player, which has been met
		elseif g_activeTeam:IsHasMet( city:GetTeam() ) then

			if cityOwner:IsMinorCiv() then

				UI.DoSelectCityAtPlot( plot )
			else
				local hasSpy = false
				if bnw_mode then
					local x, y = plot:GetX(), plot:GetY()
					for i, spy in ipairs( g_activePlayer:GetEspionageSpies() ) do
						if spy.CityX == x and spy.CityY == y then
							hasSpy = spy.EstablishedSurveillance
							break
						end
					end
				end

				if hasSpy then
					ClearHexHighlights()
					UI.DoSelectCityAtPlot( plot )

				elseif IsTurnActive( g_activePlayer ) then
					if cityOwner:IsHuman() then

						Events.OpenPlayerDealScreenEvent( cityOwnerID )
					elseif not cityOwner:IsBarbarian() then
						UI.SetRepeatActionPlayer( cityOwnerID )
						UI.ChangeStartDiploRepeatCount(1)
						cityOwner:DoBeginDiploWithHuman()
					end
				end
			end
		end
	else
		BannerError( 795, plot )
	end
end

-------------------------------------------------
-- City banner mouseover
-------------------------------------------------
local function OnBannerMouseExit()
	if not UI.IsCityScreenUp() then

		ClearHexHighlights()
		-- duplicate code from InGame.lua function RequestYieldDisplay()

		local isDisplayCivilianYields = OptionsManager.IsCivilianYields()
		local unit = UI.GetHeadSelectedUnit()

		if isDisplayCivilianYields and UI.CanSelectionListWork() and not( unit and (GameInfo.Units[unit:GetUnitType()] or {}).DontShowYields ) then
			Events.RequestYieldDisplay( YieldDisplayTypes.EMPIRE )

		elseif isDisplayCivilianYields and UI.CanSelectionListFound() and unit then
			Events.RequestYieldDisplay( YieldDisplayTypes.AREA, 2, unit:GetX(), unit:GetY() )
		else
			Events.RequestYieldDisplay( YieldDisplayTypes.AREA, 0 )
		end
	end
end

local function OnMouseEnter( plotIndex, _, control )
	g_toolTipControl = control
	g_toolTipPlotIndex = plotIndex
	return RefreshCityBanner( plotIndex )
end

local function OnBannerMouseEnter( ... )
	g_cityHexHighlight = ...
	return OnMouseEnter( ... )
end
-------------------------------------------------
-- Destroy City Banner
-------------------------------------------------

local function DestroyCityBanner( plotIndex, cityBanner )

	-- Release sv strike button
	local svStrikeButton = g_svStrikeButtons[ plotIndex ]
	if svStrikeButton then
		svStrikeButton.Anchor:ChangeParent( Controls.Scrap )
		table.insert( g_scrapSVStrikeButtons, svStrikeButton )
		g_svStrikeButtons[ plotIndex ] = nil
	end

	-- Release city banner
	if cityBanner then
		table.insert( cityBanner[1] and g_scrapTeamBanners or g_scrapOtherBanners, cityBanner )
		g_cityBanners[ plotIndex or -1 ] = nil
		return cityBanner.Anchor:ChangeParent( Controls.Scrap )
	end
end

local function DestroyCityBannerAtPlot( plot )
	if plot then
		local plotIndex = plot:GetPlotIndex()
		return DestroyCityBanner( plotIndex, g_cityBanners[ plotIndex ] )
	end
end

local function InitBannerCallbacks( cityBanner )
	cityBanner.BannerButton:RegisterCallback( Mouse.eLClick, OnBannerClick )
	cityBanner.BannerButton:RegisterCallback( Mouse.eMouseEnter, OnBannerMouseEnter )
	cityBanner.BannerButton:RegisterCallback( Mouse.eMouseExit, OnBannerMouseExit )
	cityBanner.ReligiousIcon:RegisterCallback( Mouse.eLClick, OnBannerClick )
	cityBanner.ReligiousIcon:RegisterCallback( Mouse.eMouseEnter, OnMouseEnter )
	cityBanner.DiplomatIcon:RegisterCallback( Mouse.eLClick, EspionagePopup )
	cityBanner.SpyIcon:RegisterCallback( Mouse.eLClick, EspionagePopup )
end

-------------------------------------------------
-- Update banners to reflect latest city info
-------------------------------------------------
local function RefreshCityBannersNow()
	if IsGameCoreBusy() then
		return
	end
	ContextPtr:ClearUpdate()

	------------------------------
	-- Loop all dirty city banners
	for plotIndex in pairs( g_dirtyCityBanners ) do

		local cityBanner = g_cityBanners[ plotIndex ]
		local plot = Map.GetPlotByIndex( plotIndex )
		local city = plot and plot:GetPlotCity()
		if city then
			local isActiveType = city:GetTeam() == g_activeTeamID

			-- Incompatible banner type ? Destroy !
			if cityBanner and isActiveType ~= cityBanner[1] then
				DestroyCityBanner( plotIndex, cityBanner )
				cityBanner = nil
			end

			---------------------
			-- Create City Banner
			if not cityBanner then
				local worldX, worldY, worldZ = GridToWorld( plot:GetX(), plot:GetY() )
				if isActiveType then
					-- create a strike button for stategic view
					cityBanner = table.remove( g_scrapSVStrikeButtons )
					if cityBanner then
						cityBanner.Anchor:ChangeParent( Controls.StrategicViewStrikeButtons )
					else
						cityBanner = {}
						ContextPtr:BuildInstanceForControl( "SVRangeStrikeButton", cityBanner, Controls.StrategicViewStrikeButtons )
						cityBanner.CityRangeStrikeButton:RegisterCallback( Mouse.eLClick, OnCityRangeStrikeButtonClick )
					end
					cityBanner.Anchor:SetWorldPositionVal( worldX, worldY, worldZ )
					cityBanner.CityRangeStrikeButton:SetVoid1( plotIndex )
					g_svStrikeButtons[ plotIndex ] = cityBanner

					-- create a team type city banner
					cityBanner = table.remove( g_scrapTeamBanners )
					if cityBanner then
						cityBanner.Anchor:ChangeParent( Controls.CityBanners )
					else
						cityBanner = {}
						ContextPtr:BuildInstanceForControl( "TeamCityBanner", cityBanner, Controls.CityBanners )
						cityBanner.PopulationBox:RegisterCallback( Mouse.eMouseEnter, OnMouseEnter )
						cityBanner.PopulationBox:RegisterCallback( Mouse.eLClick, OnBannerClick )
						cityBanner.CityBannerProductionButton:RegisterCallback( Mouse.eMouseEnter, OnMouseEnter )
						cityBanner.CityBannerProductionButton:RegisterCallback( Mouse.eLClick, OnBannerClick )
						cityBanner.CityBannerProductionButton:RegisterCallback( Mouse.eRClick, OnProdRClick )
						cityBanner.CityRangeStrikeButton:RegisterCallback( Mouse.eLClick, OnCityRangeStrikeButtonClick )
						cityBanner.PuppetIcon:RegisterCallback( Mouse.eLClick, AnnexPopup )
						InitBannerCallbacks( cityBanner )
					end
					cityBanner.PuppetIcon:SetVoid1( plotIndex )
					cityBanner.PopulationBox:SetVoid1( plotIndex )
					cityBanner.CityRangeStrikeButton:SetVoid1( plotIndex )
					cityBanner.CityBannerProductionButton:SetVoid1( plotIndex )
					cityBanner.CityBannerProductionButton:SetDisabled( true )
				else
					-- create a foreign type city banner
					cityBanner = table.remove( g_scrapOtherBanners )
					if cityBanner then
						cityBanner.Anchor:ChangeParent( Controls.CityBanners )
					else
						cityBanner = {}
						ContextPtr:BuildInstanceForControl( "OtherCityBanner", cityBanner, Controls.CityBanners )
						cityBanner.QuestIcons:RegisterCallback( Mouse.eLClick, OnQuestInfoClicked )
						InitBannerCallbacks( cityBanner )
					end
					cityBanner.QuestIcons:SetVoid1( plotIndex )
				end

				cityBanner.BannerButton:SetVoid1( plotIndex )
				cityBanner.ReligiousIcon:SetVoid1( plotIndex )
				cityBanner.Anchor:SetWorldPositionVal( worldX, worldY, worldZ + g_WorldPositionOffsetZ )

				cityBanner[1] = isActiveType
				g_cityBanners[ plotIndex ] = cityBanner
			end
			-- Create City Banner
			---------------------

			local cityOwnerID = city:GetOwner()
			local cityOwner = Players[ cityOwnerID ]


			local isActivePlayerCity = cityOwnerID == g_activePlayerID

			-- Refresh the damage bar
			RefreshCityDamage( city, cityBanner, city:GetDamage() )

			-- Colors
			local color = g_primaryColors[ cityOwnerID ]
			local backgroundColor = g_backgroundColors[ cityOwnerID ]

			-- Update name
			local cityName = city:GetName()
			local upperCaseCityName = Locale.ToUpper( cityName )

			local originalCityOwnerID = city:GetOriginalOwner()
			local originalCityOwner = Players[ originalCityOwnerID ]
			local otherCivID, otherCivAlpha

			-- Update capital icon
			cityBanner.CapitalIcon:SetHide( not city:IsCapital() or cityOwner:IsMinorCiv() )
			cityBanner.OriginalCapitalIcon:SetHide( city:IsCapital() or not city:IsOriginalCapital() )

			cityBanner.CityName:SetText( upperCaseCityName )

			cityBanner.CityName:SetColor( color, 0 )			-- #0 = main color
			cityBanner.CityName:SetColor( Color( 0, 0, 0, 0.5 ), 1 )	-- #1 = shadow color
			cityBanner.CityName:SetColor( Color( 1, 1, 1, 0.5 ), 2 )	-- #2 = soft color

			-- Update strength
			cityBanner.CityStrength:SetText(math.floor(city:GetStrengthValue() / 100))

			-- Update population
			local cityPopulation = math.floor(city:GetPopulation())
			cityBanner.CityPopulation:SetText(cityPopulation)

			-- Being Razed ?
			if city:IsRazing() then
				cityBanner.RazingIcon:SetHide( false )
				cityBanner.RazingIcon:LocalizeAndSetToolTip( "TXT_KEY_CITY_BURNING", city:GetRazingTurns() )
			else
				cityBanner.RazingIcon:SetHide( true )
			end

			-- In Resistance ?
			if city:IsResistance() then
				cityBanner.ResistanceIcon:SetHide( false )
				cityBanner.ResistanceIcon:LocalizeAndSetToolTip( "TXT_KEY_CITY_RESISTANCE", city:GetResistanceTurns() )
			else
				cityBanner.ResistanceIcon:SetHide( true )
			end

			-- Puppet ?
			if city:IsPuppet() then
				cityBanner.PuppetIcon:SetHide( false )
				if isActivePlayerCity then
					if bnw_mode and g_activePlayer:MayNotAnnex() then
						cityBanner.PuppetIcon:LocalizeAndSetToolTip( "TXT_KEY_CITY_PUPPET" )
					else
						cityBanner.PuppetIcon:SetToolTipString( L"TXT_KEY_CITY_PUPPET".."[NEWLINE][NEWLINE]"..L"TXT_KEY_CITY_ANNEX_TT" )
					end
				end
			else
				cityBanner.PuppetIcon:SetHide( true )
			end

			-- Occupied ?
			cityBanner.OccupiedIcon:SetHide( not city:IsOccupied() or city:IsNoOccupiedUnhappiness() )

			-- Blockaded ?
			cityBanner.BlockadedIcon:SetHide( not city:IsBlockaded() )

			cityBanner.CityBannerBackground:SetColor( backgroundColor )
			cityBanner.CityBannerRightBackground:SetColor( backgroundColor )
			cityBanner.CityBannerLeftBackground:SetColor( backgroundColor )

			if isActiveType then

				cityBanner.CityBannerBGLeftHL:SetColor( backgroundColor )
				cityBanner.CityBannerBGRightHL:SetColor( backgroundColor )
				cityBanner.CityBannerBackgroundHL:SetColor( backgroundColor )

				-- Update Growth
				local foodStored100 = city:GetFoodTimes100()
				local foodThreshold100 = city:GrowthThreshold() * 100
				local foodPerTurn100 = city:FoodDifferenceTimes100( true )
				local foodStoredPercent = 0
				local foodStoredNextTurnPercent = 0
				if foodThreshold100 > 0 then
					foodStoredPercent = foodStored100 / foodThreshold100
					foodStoredNextTurnPercent = ( foodStored100 + foodPerTurn100 ) / foodThreshold100
					if foodPerTurn100 < 0 then
						foodStoredPercent, foodStoredNextTurnPercent = foodStoredNextTurnPercent, foodStoredPercent
					end
				end

				-- Update Growth Meter
				cityBanner.GrowthBar:SetPercent( math.max(math.min( foodStoredPercent, 1),0))
				cityBanner.GrowthBarShadow:SetPercent( math.max(math.min( foodStoredNextTurnPercent, 1),0))
				cityBanner.GrowthBarStarve:SetHide( foodPerTurn100 >= 0 )

				-- Update Growth Time
				local turnsToCityGrowth = city:GetFoodTurnsLeft()
				local growthText

				if foodPerTurn100 < 0 then
					turnsToCityGrowth = math.floor( foodStored100 / -foodPerTurn100 ) + 1
					growthText = "[COLOR_WARNING_TEXT]" .. turnsToCityGrowth .. "[ENDCOLOR]"
				elseif city:IsForcedAvoidGrowth() then
					growthText = "[ICON_LOCKED]"
				elseif foodPerTurn100 == 0 then
					growthText = "-"
				else
					growthText = math.min(turnsToCityGrowth,99)
				end

				cityBanner.CityGrowth:SetText( growthText )

				local productionPerTurn100 = city:GetCurrentProductionDifferenceTimes100(false, false)	-- food = false, overflow = false
				local productionStored100 = city:GetProductionTimes100() + city:GetCurrentProductionDifferenceTimes100(false, true) - productionPerTurn100
				local productionNeeded100 = city:GetProductionNeeded() * 100
				local productionStoredPercent = 0
				local productionStoredNextTurnPercent = 0

				if productionNeeded100 > 0 then
					productionStoredPercent = productionStored100 / productionNeeded100
					productionStoredNextTurnPercent = (productionStored100 + productionPerTurn100) / productionNeeded100
				end

				cityBanner.ProductionBar:SetPercent( math.max(math.min( productionStoredPercent, 1),0))
				cityBanner.ProductionBarShadow:SetPercent( math.max(math.min( productionStoredNextTurnPercent, 1),0))

				-- Update Production Time
				if city:IsProduction()
					and not city:IsProductionProcess()
					and productionPerTurn100 > 0
				then
					cityBanner.BuildGrowth:SetText( city:GetProductionTurnsLeft() )
				else
					cityBanner.BuildGrowth:SetText( "-" )
				end

				-- Update Production icon
				local unitProductionID = city:GetProductionUnit()
				local buildingProductionID = city:GetProductionBuilding()
				local projectProductionID = city:GetProductionProject()
				local processProductionID = city:GetProductionProcess()
				local item = nil

				if unitProductionID ~= -1 then
					item = GameInfo.Units[unitProductionID]
				elseif buildingProductionID ~= -1 then
					item = GameInfo.Buildings[buildingProductionID]
				elseif projectProductionID ~= -1 then
					item = GameInfo.Projects[projectProductionID]
				elseif processProductionID ~= -1 then
					item = GameInfo.Processes[processProductionID]
				end
				-- really should have an error texture

				cityBanner.CityBannerProductionImage:SetHide( not( item and
					IconHookup( item.PortraitIndex, 45, item.IconAtlas, cityBanner.CityBannerProductionImage )))
				cityBanner.CityBannerProductionButton:SetDisabled( not isActivePlayerCity )

				-- Focus?
				local cityFocus = g_CityFocus[city:GetFocusType()]
				if not cityFocus or city:IsRazing() or city:IsResistance() or city:IsPuppet() then
					cityBanner.FocusIcon:SetHide( true )
				else
					cityBanner.FocusIcon:SetText( cityFocus[1] )
					cityBanner.FocusIcon:SetToolTipString( cityFocus[2] )
					cityBanner.FocusIcon:SetHide( false )
--todo focus menu
				end

				-- Connected to capital?
				if cityOwner:IsCapitalConnectedToCity(city) and not city:IsCapital() then -- and not city:IsBlockaded()
					local connectionTip = L("TXT_KEY_CITY_CONNECTED")
					if cityOwner.GetRouteGoldTimes100 then
						connectionTip = connectionTip .. S(" (%+g[ICON_GOLD])", cityOwner:GetRouteGoldTimes100( city ) / 100 )
					elseif cityOwner.GetCityConnectionRouteGoldTimes100 then
						connectionTip = connectionTip .. S(" (%+g[ICON_GOLD])", cityOwner:GetCityConnectionRouteGoldTimes100( city ) / 100 )
					end
					cityBanner.ConnectedIcon:SetHide( false )
					cityBanner.ConnectedIcon:SetToolTipString( connectionTip )
				else
					cityBanner.ConnectedIcon:SetHide( true )
				end

				-- Demand resource / King day ?
				local resource = GameInfo.Resources[city:GetResourceDemanded()]
				local weLoveTheKingDayCounter = city:GetWeLoveTheKingDayCounter()
				-- We love the king
				if weLoveTheKingDayCounter > 0 then
					cityBanner.QuestIcons:SetText( "[ICON_HAPPINESS_1]" )
					cityBanner.QuestIcons:LocalizeAndSetToolTip( "TXT_KEY_CITYVIEW_WLTKD_COUNTER", weLoveTheKingDayCounter )
					cityBanner.QuestIcons:SetHide( false )

				elseif resource then
					cityBanner.QuestIcons:SetText( resource.IconString )
					cityBanner.QuestIcons:LocalizeAndSetToolTip( "TXT_KEY_CITYVIEW_RESOURCE_DEMANDED", resource.IconString .. " " .. L(resource.Description) )
					cityBanner.QuestIcons:SetHide( false )
				else
					cityBanner.QuestIcons:SetHide( true )
				end

				-- update range strike button (if it is the active player's city)

				UpdateRangeIcons( plotIndex, city, cityBanner )

				cityBanner.GarrisonFrame:SetHide( not city:GetGarrisonedUnit() )

			-- not active team city
			else
				local isMinorCiv = cityOwner:IsMinorCiv()
				local allyID, ally

				if isMinorCiv then
					-- Update Quests
					cityBanner.QuestIcons:SetText( GetActiveQuestText( g_activePlayerID, cityOwnerID ) )
					cityBanner.QuestIcons:SetToolTipString( GetActiveQuestToolTip( g_activePlayerID, cityOwnerID ) )
					cityBanner.QuestIcons:SetHide( false )

					color = g_colorWhite

					local info = GetCityStateStatusRow( g_activePlayerID, cityOwnerID )
					cityBanner.StatusIconBG:SetTexture( info and info.StatusIcon or "Blank.dds" )
					cityBanner.StatusIconBG:SetTextureOffsetVal( 0, 0 )
					cityBanner.StatusIcon:SetTexture( (GameInfo.MinorCivTraits[ (GameInfo.MinorCivilizations[ cityOwner:GetMinorCivType() ] or {}).MinorCivTrait or -1 ] or {}).TraitIcon or "Blank.dds" )
					-- Update Pledge
					if gk_mode then
						local pledge = g_activePlayer:IsProtectingMinor( cityOwnerID )
						local free = pledge and cityOwner:CanMajorWithdrawProtection( g_activePlayerID )
						cityBanner.Pledge1:SetHide( not pledge or free )
						cityBanner.Pledge2:SetHide( not free )
					end
					-- Update Allies
					allyID = cityOwner:GetAlly()
					ally = Players[ allyID ]
				else
					cityBanner.QuestIcons:SetHide( true )
					local civInfo = GameInfo.Civilizations[ cityOwner:GetCivilizationType() ]

					IconHookup( civInfo.PortraitIndex, 32, civInfo.AlphaIconAtlas, cityBanner.StatusIconBG )
				end

				cityBanner.StatusIconBG:SetColor( color )
				cityBanner.StatusIcon:SetHide( not isMinorCiv )

				if ally then
				-- Set left banner icon to ally flag
					otherCivAlpha = 1
					otherCivID = g_activeTeam:IsHasMet( ally:GetTeam() ) and allyID or -1
					cityBanner.CivIndicator:SetToolTipString( GetAllyToolTip( g_activePlayerID, cityOwnerID ) )
				end
			end
			if not otherCivID and originalCityOwner and (originalCityOwnerID ~= cityOwnerID) then
			-- Set left banner icon to city state flag
				if originalCityOwner:IsMinorCiv() then
					otherCivAlpha = 4 --hack
					cityBanner.MinorTraitIcon:SetTexture( (GameInfo.MinorCivTraits[ (GameInfo.MinorCivilizations[ originalCityOwner:GetMinorCivType() ] or {}).MinorCivTrait or -1 ] or {}).TraitIcon or "Blank.dds" )
					cityBanner.OriginalCapitalIcon:SetHide( true )
				else
					otherCivAlpha = 0.5
					otherCivID = g_activeTeam:IsHasMet( originalCityOwner:GetTeam() ) and originalCityOwnerID
					cityBanner.CivIndicator:LocalizeAndSetToolTip( "TXT_KEY_POPUP_CITY_CAPTURE_INFO_LIBERATE", originalCityOwner:GetCivilizationShortDescription() )
				end
			end
			if otherCivID then
				CivIconHookup( otherCivID, 32, cityBanner.CivIcon, cityBanner.CivIconBG, cityBanner.CivIconShadow, false, true )
				cityBanner.CivIndicator:SetAlpha( otherCivAlpha )
			end
			cityBanner.MinorCivIndicator:SetHide( otherCivAlpha ~= 4 ) -- hack
			cityBanner.CivIndicator:SetHide( not otherCivID )

			-- Spy & Religion
			if gk_mode then
				local spy
				local x = city:GetX()
				local y = city:GetY()

				for i, s in ipairs( g_activePlayer:GetEspionageSpies() ) do
					if s.CityX == x and s.CityY == y then
						spy = s
						break
					end
				end

				if spy then
					if spy.IsDiplomat then
						cityBanner.SpyIcon:SetHide( true )
						cityBanner.DiplomatIcon:SetHide( false )
						cityBanner.SpyIcon:LocalizeAndSetToolTip( "TXT_KEY_CITY_DIPLOMAT_OTHER_CIV_TT", spy.Rank, spy.Name, cityName, spy.Rank, spy.Name, spy.Rank, spy.Name )
					else
						cityBanner.SpyIcon:SetHide( false )
						cityBanner.DiplomatIcon:SetHide( true )
						if isActivePlayerCity then
							cityBanner.SpyIcon:LocalizeAndSetToolTip( "TXT_KEY_CITY_SPY_YOUR_CITY_TT", spy.Rank, spy.Name, cityName, spy.Rank, spy.Name )
						elseif cityOwner:IsMinorCiv() then
							cityBanner.SpyIcon:LocalizeAndSetToolTip( "TXT_KEY_CITY_SPY_CITY_STATE_TT", spy.Rank, spy.Name, cityName, spy.Rank, spy.Name)
						else
							cityBanner.SpyIcon:LocalizeAndSetToolTip( "TXT_KEY_CITY_SPY_OTHER_CIV_TT", spy.Rank, spy.Name, cityName, spy.Rank, spy.Name, spy.Rank, spy.Name)
						end
					end
				else
					cityBanner.SpyIcon:SetHide( true )
					cityBanner.DiplomatIcon:SetHide( true )
				end

				local religionID = city:GetReligiousMajority()
				if religionID >= 0 then
					local religion = GameInfo.Religions[religionID]
					IconHookup( religion.PortraitIndex, 32, religion.IconAtlas, cityBanner.ReligiousIcon )
					IconHookup( religion.PortraitIndex, 32, religion.IconAtlas, cityBanner.ReligiousIconShadow )
					cityBanner.ReligiousIconContainer:SetHide( false )
				else
					cityBanner.ReligiousIconContainer:SetHide( true )
				end
			end

			-- Change the width of the banner so it looks good with the length of the city name

			cityBanner.NameStack:CalculateSize()
			local bannerWidth = cityBanner.NameStack:GetSizeX() - 64
			cityBanner.BannerButton:SetSizeX( bannerWidth )
			cityBanner.CityBannerBackground:SetSizeX( bannerWidth )
			cityBanner.CityBannerBackgroundHL:SetSizeX( bannerWidth )
			if isActiveType then
				cityBanner.CityBannerBackgroundIcon:SetSizeX( bannerWidth )
				cityBanner.CityBannerButtonGlow:SetSizeX( bannerWidth )
				cityBanner.CityBannerButtonBase:SetSizeX( bannerWidth )
			else
				cityBanner.CityBannerBaseFrame:SetSizeX( bannerWidth )
			end

			cityBanner.BannerButton:ReprocessAnchoring()
			cityBanner.NameStack:ReprocessAnchoring()
			cityBanner.IconsStack:CalculateSize()
			cityBanner.IconsStack:ReprocessAnchoring()

		-- No city on this plot ? Destroy !
		else
			DestroyCityBanner( plotIndex, cityBanner )
		end
	end
	-- Loop all dirty city banners
	------------------------------
	g_dirtyCityBanners = {}

	if g_cityHexHighlight then
		local plot = Map.GetPlotByIndex( g_cityHexHighlight )
		local city = plot and plot:GetPlotCity()
		if city then

			local cityOwnerID = city:GetOwner()

			if city:GetTeam() == g_activeTeamID or g_activePlayer:IsObserver() then

				if cityOwnerID == g_activePlayerID and not( Game.IsNetworkMultiPlayer() and g_activePlayer:HasReceivedNetTurnComplete() ) then -- required to prevent turn interrupt
					Network.SendUpdateCityCitizens( city:GetID() )
				end

				local normalView = not (civ5_mode and InStrategicView())
				-- Show plots that will be acquired by culture
				local purchasablePlots = {city:GetBuyablePlotList()}
				for i = 1, #purchasablePlots do
					local hexPos = ToHexFromGrid{ x= purchasablePlots[i]:GetX(), y= purchasablePlots[i]:GetY() }
					Events.SerialEventHexHighlight( hexPos , true, g_colorCulture, "Culture" )
				end

				-- Show city plots

				for i = 0, city:GetNumCityPlots()-1 do
					local plot = city:GetCityIndexPlot( i )
					-- worked city plots
					if plot then
						local hexPos = ToHexFromGrid{ x=plot:GetX(), y=plot:GetY() }
						if city:IsWorkingPlot( plot ) then
							Events.SerialEventHexHighlight( hexPos , true, nil, "WorkedFill" )
							Events.SerialEventHexHighlight( hexPos , true, nil, "WorkedOutline" )
						end
						if normalView then
							Events.SerialEventHexHighlight( hexPos , true, nil, "CityLimits" )
						end
					end
				end
				for plot in CityPlots( city ) do
					-- city plots that are owned but not worked
					if not city:IsWorkingPlot( plot ) then
						local hexPos = ToHexFromGrid{ x=plot:GetX(), y=plot:GetY() }
						Events.SerialEventHexHighlight( hexPos , true, nil, "OwnedFill" )
						Events.SerialEventHexHighlight( hexPos , true, nil, "OwnedOutline" )
					end
				end
			else
				for plot in CityPlots( city ) do
					local hexPos = ToHexFromGrid{ x=plot:GetX(), y=plot:GetY() }
					Events.SerialEventHexHighlight( hexPos , true, nil, "OwnedFill" )
					Events.SerialEventHexHighlight( hexPos , true, nil, "OwnedOutline" )
				end
			end
			Events.RequestYieldDisplay( YieldDisplayTypes.CITY_OWNED, city:GetX(), city:GetY() )
		end
		g_cityHexHighlight = false
	end
	if g_toolTipControl then
		local toolTipHandler = g_toolTipHandler[g_toolTipControl:GetID()]
		if toolTipHandler then
			toolTipHandler( g_toolTipControl )
		end
		g_toolTipControl = nil
	end
end

RefreshCityBanner = function( plotIndex )
	if plotIndex then
		g_dirtyCityBanners[ plotIndex ] = true
		return ContextPtr:SetUpdate( RefreshCityBannersNow )
	end
end

local function RefreshCityBannerAtPlot( plot )
	return RefreshCityBanner( plot and plot:GetPlotIndex() )
end

-------------------------------------------------
-- Register Events
-------------------------------------------------

------------------
-- On City Created
Events.SerialEventCityCreated.Add( function( hexPos, cityOwnerID, cityID, cultureType, eraType, continent, populationSize, size, fowState )

	return RefreshCityBannerAtPlot( fowState ~= BlackFog and Map.GetPlot( ToGridFromHex( hexPos.x, hexPos.y ) ) )

end)

------------------
-- On City Updated
Events.SerialEventCityInfoDirty.Add( function()
	-- Update all visible city banners
	for plotIndex in pairs( g_cityBanners ) do
		RefreshCityBanner( plotIndex )
	end
end)

--------------------
-- On City Destroyed
Events.SerialEventCityDestroyed.Add( 
function( hexPos, cityOwnerID, cityID, newPlayerID )
	local plot = Map.GetPlot( ToGridFromHex( hexPos.x, hexPos.y ) )
	if plot then
		local plotIndex = plot:GetPlotIndex()
		return DestroyCityBanner( plotIndex, g_cityBanners[ plotIndex ] )
	end
end)

---------------------
-- On City Set Damage
Events.SerialEventCitySetDamage.Add( function( cityOwnerID, cityID, cityDamage, previousDamage )
	local cityOwner = Players[ cityOwnerID ]
	local city = cityOwner and cityOwner:GetCityByID( cityID )
	return RefreshCityDamage( city, city and g_cityBanners[ city:Plot():GetPlotIndex() ], cityDamage )
end)

---------------------------
-- On Specific City changed
Events.SpecificCityInfoDirty.Add( function( cityOwnerID, cityID, updateType )
	local cityOwner = Players[ cityOwnerID ]
	local city = cityOwner and cityOwner:GetCityByID( cityID )
	local plotIndex = city and city:Plot():GetPlotIndex()
	local cityBanner = g_cityBanners[ plotIndex ]
	if plotIndex and cityBanner then
		if updateType == CityUpdateTypes.CITY_UPDATE_TYPE_ENEMY_IN_RANGE then
			return UpdateRangeIcons( plotIndex, city, cityBanner )
		elseif updateType == CityUpdateTypes.CITY_UPDATE_TYPE_BANNER or updateType == CityUpdateTypes.CITY_UPDATE_TYPE_GARRISON then
			return RefreshCityBanner( plotIndex )
		end
	end

end)

-------------------------
-- On Improvement Created
Events.SerialEventImprovementCreated.Add( function( hexX, hexY, cultureID, continentID, playerID )--, improvementID, rawResourceID, improvementEra, improvementState )
	if playerID == g_activePlayerID then
		local plot = Map.GetPlot( ToGridFromHex( hexX, hexY ) )
		local city = plot and plot:GetWorkingCity()
		return RefreshCityBanner( city and city:Plot():GetPlotIndex() )
	end
end)

---------------------------
-- On Road/Railroad Created
Events.SerialEventRoadCreated.Add( function( hexX, hexY, playerID, roadID )
	if playerID == g_activePlayerID then
		for city in g_activePlayer:Cities() do
			RefreshCityBanner( city:Plot():GetPlotIndex() )
		end
	end
end)

-----------------------
-- On city range strike
Events.InitCityRangeStrike.Add( function( cityOwnerID, cityID )

	if cityOwnerID == g_activePlayerID then

		local city = g_activePlayer:GetCityByID( cityID )
		if city then
			UI.SelectCity( city )
			UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_CITY_RANGE_ATTACK)
		else
			BannerError( 1634, city )
		end
	end
end)

-------------------
-- On Unit Garrison
Events.UnitGarrison.Add( function( unitOwnerID, unitID, isGarrisoned )
	if isGarrisoned then
		local unitOwner = Players[ unitOwnerID ]
		local unit = unitOwner and unitOwner:GetUnitByID( unitID )
		local city = unit and unit:GetGarrisonedCity()
		return HideGarrisonFrame( city and g_cityBanners[ city:Plot():GetPlotIndex() ], UnitMoving( unitOwnerID, unitID ) )
	end
end)

-----------------------------
-- On Unit Move Queue Changed
Events.UnitMoveQueueChanged.Add( function( unitOwnerID, unitID, hasRemainingMoves )
	local unitOwner = Players[ unitOwnerID ]
	local unit = unitOwner and unitOwner:GetUnitByID( unitID )
	local city = unit and unit:GetGarrisonedCity()
	return HideGarrisonFrame( city and g_cityBanners[ city:Plot():GetPlotIndex() ], not hasRemainingMoves )
end)

---------------------------
-- On interface mode change
Events.InterfaceModeChanged.Add( function(oldInterfaceMode, newInterfaceMode)
	local disableBanners = newInterfaceMode ~= InterfaceModeTypes.INTERFACEMODE_SELECTION
	for plotIndex, cityBanner in pairs( g_cityBanners ) do
		cityBanner.BannerButton:SetDisabled(disableBanners)
		cityBanner.BannerButton:EnableToolTip(not disableBanners)
	end
end)

Events.StrategicViewStateChanged.Add( function(isStrategicView, showCityBanners)
	local showBanners = showCityBanners or not isStrategicView
	Controls.CityBanners:SetHide( not showBanners )
	return Controls.StrategicViewStrikeButtons:SetHide( showBanners )
end)

-----------------------
-- On fog of war change
Events.HexFOWStateChanged.Add( function( hexPos, fowType, isWholeMap )
	if isWholeMap then
		if fowType == BlackFog then
			for plotIndex, cityBanner in pairs( g_cityBanners ) do
				DestroyCityBanner( plotIndex, cityBanner )
			end
		else
			for playerID = 0, GameDefines.MAX_CIV_PLAYERS-1 do
				local player = Players[ playerID ]
				if player and player:IsAlive() then
					for city in player:Cities() do
						RefreshCityBannerAtPlot( city:Plot() )
					end
				end
			end
		end
	else
		local plot = Map.GetPlot( ToGridFromHex( hexPos.x, hexPos.y ) )
		if plot and plot:IsCity() then
			local plotIndex = plot:GetPlotIndex()
			if fowType == BlackFog then
				DestroyCityBanner( plotIndex, g_cityBanners[ plotIndex ] )
			else
				return RefreshCityBanner( plotIndex )
			end
		end
	end
end)

-------------------------------------------------
-- 'Active' (local human) player has changed:
-- Check for City Banner Active Type change
-------------------------------------------------
Events.GameplaySetActivePlayer.Add( function( activePlayerID, previousActivePlayerID )
	-- update globals

	g_activePlayerID = Game.GetActivePlayer()
	g_activePlayer = Players[ g_activePlayerID ]
	g_activeTeamID = Game.GetActiveTeam()
	g_activeTeam = Teams[ g_activeTeamID ]
	ClearHexHighlights()
	-- Update all city banners
	for playerID = 0, GameDefines.MAX_CIV_PLAYERS-1 do
		local player = Players[ playerID ]
		if player and player:IsAlive() then
			for city in player:Cities() do
				local plot = city:Plot()
				local isRevealed = plot:IsRevealed( g_activeTeamID )
				local plotIndex = plot:GetPlotIndex()
				local cityBanner = g_cityBanners[ plotIndex ]
				local isActiveType = city:GetTeam() == g_activeTeamID

				-- If city banner is hidden or type no longer matches, destroy the banner
				if cityBanner and not(isRevealed and isActiveType == cityBanner[1]) then
					DestroyCityBanner( plotIndex, cityBanner )
				end
				RefreshCityBanner( isRevealed and plotIndex )
			end
		end
	end
end)

-------------------------------------------------
-- Hide Garrisson Ring during Animated Combat
-------------------------------------------------
if gk_mode then

	local function HideGarrisonRing( x, y, hideGarrisonRing )

		local plot = Map.GetPlot( x, y )
		local city = plot and plot:GetPlotCity()
		local cityBanner = city and g_cityBanners[ plot:GetPlotIndex() ]
		return cityBanner and HideGarrisonFrame( cityBanner, hideGarrisonRing or not city:GetGarrisonedUnit() )
	end

	Events.RunCombatSim.Add( function(
				attackerPlayerID,
				attackerUnitID,
				attackerUnitDamage,
				attackerFinalUnitDamage,
				attackerMaxHitPoints,
				defenderPlayerID,
				defenderUnitID,
				defenderUnitDamage,
				defenderFinalUnitDamage,
				defenderMaxHitPoints,
				attackerX,
				attackerY,
				defenderX,
				defenderY,
				bContinuation)
--print( "CityBanner CombatBegin", attackerX, attackerY, defenderX, defenderY )

		HideGarrisonRing(attackerX, attackerY, true)
		HideGarrisonRing(defenderX, defenderY, true)
	end)

	Events.EndCombatSim.Add( function(
				attackerPlayerID,
				attackerUnitID,
				attackerUnitDamage,
				attackerFinalUnitDamage,
				attackerMaxHitPoints,
				defenderPlayerID,
				defenderUnitID,
				defenderUnitDamage,
				defenderFinalUnitDamage,
				defenderMaxHitPoints,
				attackerX,
				attackerY,
				defenderX,
				defenderY )

--print( "CityBanner CombatEnd", attackerX, attackerY, defenderX, defenderY )

		HideGarrisonRing(attackerX, attackerY, false)
		HideGarrisonRing(defenderX, defenderY, false)
	end)

end -- gk_mode

--------------------------------------------------------------
-- The active player's turn has begun, make sure their range strike icons are correct
Events.ActivePlayerTurnStart.Add( function()

	for plotIndex, cityBanner in pairs( g_cityBanners ) do
		UpdateRangeIcons( plotIndex, Map.GetPlotByIndex( plotIndex ):GetPlotCity(), cityBanner )
	end
end)

Events.SerialEventUnitDestroyed.Add( function( unitOwnerID, unitID )
	local unitOwner = Players[ unitOwnerID ]
	if unitOwner and g_activeTeam:IsAtWar( unitOwner:GetTeam() ) then
		for city in g_activePlayer:Cities() do
			local plotIndex = city:Plot():GetPlotIndex()
			UpdateRangeIcons( plotIndex, city, g_cityBanners[ plotIndex ] )
		end
	end
end)

-------------------------------------------------
-- Initialize all Visible City Banners
-------------------------------------------------
for playerID = 0, GameDefines.MAX_CIV_PLAYERS-1 do
	local player = Players[ playerID ]
	if player then
		if player:IsAlive() then
			for city in player:Cities() do
				local plot = city:Plot()
				RefreshCityBannerAtPlot( plot:IsRevealed( g_activeTeamID ) and plot )
			end
		end
	end
end

print("Finished loading EUI city banners", os.clock())
end)
