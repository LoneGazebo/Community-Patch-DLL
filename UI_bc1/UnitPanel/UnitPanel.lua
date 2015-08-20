------------------------------------------------------
-- Unit Panel Screen
-- modified by bc1 from Civ V 1.0.3.276 code
-- code is common using gk_mode and bnw_mode switches
------------------------------------------------------
Events.SequenceGameInitComplete.Add(function()
print("Loading EUI unit panel...",os.clock(),[[ 
 _   _       _ _   ____                  _ 
| | | |_ __ (_) |_|  _ \ __ _ _ __   ___| |
| | | | '_ \| | __| |_) / _` | '_ \ / _ \ |
| |_| | | | | | |_|  __/ (_| | | | |  __/ |
 \___/|_| |_|_|\__|_|   \__,_|_| |_|\___|_|
]])

local gk_mode = Game.GetReligionName ~= nil
local bnw_mode = Game.GetActiveLeague ~= nil

include( "EUI_utilities" )
local IconLookup = EUI.IconLookup
local IconHookup = EUI.IconHookup
local InstanceStackManager = EUI.InstanceStackManager
local Color = EUI.Color
local PrimaryColors = EUI.PrimaryColors
local BackgroundColors = EUI.BackgroundColors
local table = EUI.table
include( "EUI_tooltips" )

-------------------------------------------------
-- Minor lua optimizations
-------------------------------------------------
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
local tostring = tostring
--local type = type
--local unpack = unpack

local UI = UI
--local UIManager = UIManager
local Controls = Controls
local ContextPtr = ContextPtr
local Players = Players
local Teams = Teams
local GameInfo = EUI.GameInfoCache -- warning! use iterator ONLY with table field conditions, NOT string SQL query
local GameInfoActions = GameInfoActions
local GameInfoTypes = GameInfoTypes
local GameDefines = GameDefines
--local InterfaceDirtyBits = InterfaceDirtyBits
--local CityUpdateTypes = CityUpdateTypes
--local ButtonPopupTypes = ButtonPopupTypes
local YieldTypes = YieldTypes
--local GameOptionTypes = GameOptionTypes
local DomainTypes = DomainTypes
--local FeatureTypes = FeatureTypes
--local FogOfWarModeTypes = FogOfWarModeTypes
local OrderTypes = OrderTypes
--local PlotTypes = PlotTypes
--local TerrainTypes = TerrainTypes
--local InterfaceModeTypes = InterfaceModeTypes
--local NotificationTypes = NotificationTypes
local ActivityTypes = ActivityTypes
--local MissionTypes = MissionTypes
local ActionSubTypes = ActionSubTypes
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
--local MinorCivQuestTypes = MinorCivQuestTypes
--local CityAIFocusTypes = CityAIFocusTypes
--local AdvisorTypes = AdvisorTypes
--local GenericWorldAnchorTypes = GenericWorldAnchorTypes
--local GameStates = GameStates
--local GameplayGameStateTypes = GameplayGameStateTypes
--local CombatPredictionTypes = CombatPredictionTypes
--local ChatTargetTypes = ChatTargetTypes
local ReligionTypes = ReligionTypes
--local BeliefTypes = BeliefTypes
--local FaithPurchaseTypes = FaithPurchaseTypes
--local ResolutionDecisionTypes = ResolutionDecisionTypes
--local InfluenceLevelTypes = InfluenceLevelTypes
--local InfluenceLevelTrend = InfluenceLevelTrend
--local PublicOpinionTypes = PublicOpinionTypes
--local ControlTypes = ControlTypes

local PreGame = PreGame
local Game = Game
local Map = Map
--local OptionsManager = OptionsManager
local Events = Events
local Mouse = Mouse
--local MouseEvents = MouseEvents
--local MouseOverStrategicViewResource = MouseOverStrategicViewResource
local Locale = Locale
local L = Locale.ConvertTextKey
local IsGameCoreBusy = IsGameCoreBusy

local GameInfoMissions = GameInfo.Missions
local GameInfoAutomates = GameInfo.Automates
local GameInfoBuilds = GameInfo.Builds
local GameInfoUnits = GameInfo.Units

-------------------------------------------------
-- Globals
-------------------------------------------------

local g_tipControls = {}
TTManager:GetTypeControlTable( "EUI_UnitPanelItemTooltip", g_tipControls )

local g_activePlayerID, g_activePlayer, g_activeTeamID, g_activeTeam, g_activeTechs
local g_AllPlayerOptions = { UnitTypes = {}, HideUnitTypes = {}, UnitsInRibbon = {} }

local g_ActivePlayerUnitTypes
local g_ActivePlayerUnitsInRibbon = {}
local g_isHideCityList, g_isHideUnitList, g_isHideUnitTypes
local ActionToolTipHandler
--[[ 
 _   _       _ _          ___      ____ _ _   _             ____  _ _     _                 
| | | |_ __ (_) |_ ___   ( _ )    / ___(_) |_(_) ___  ___  |  _ \(_) |__ | |__   ___  _ __  
| | | | '_ \| | __/ __|  / _ \/\ | |   | | __| |/ _ \/ __| | |_) | | '_ \| '_ \ / _ \| '_ \ 
| |_| | | | | | |_\__ \ | (_>  < | |___| | |_| |  __/\__ \ |  _ <| | |_) | |_) | (_) | | | |
 \___/|_| |_|_|\__|___/  \___/\/  \____|_|\__|_|\___||___/ |_| \_\_|_.__/|_.__/ \___/|_| |_|
]]

-- NO_ACTIVITY, ACTIVITY_AWAKE, ACTIVITY_HOLD, ACTIVITY_SLEEP, ACTIVITY_HEAL, ACTIVITY_SENTRY, ACTIVITY_INTERCEPT, ACTIVITY_MISSION
local g_activityMissions = {
[ActivityTypes.ACTIVITY_AWAKE] = nil,
[ActivityTypes.ACTIVITY_HOLD] = GameInfoMissions.MISSION_SKIP,
[ActivityTypes.ACTIVITY_SLEEP] = GameInfoMissions.MISSION_SLEEP,
[ActivityTypes.ACTIVITY_HEAL] = GameInfoMissions.MISSION_HEAL,
[ActivityTypes.ACTIVITY_SENTRY] = GameInfoMissions.MISSION_ALERT,
[ActivityTypes.ACTIVITY_INTERCEPT] = GameInfoMissions.MISSION_AIRPATROL,
[ActivityTypes.ACTIVITY_MISSION] = GameInfoMissions.MISSION_MOVE_TO,
}
local g_MaxDamage = GameDefines.MAX_HIT_POINTS or 100
local g_rebaseRangeMultipler = GameDefines.AIR_UNIT_REBASE_RANGE_MULTIPLIER
local g_pressureMultiplier = GameDefines.RELIGION_MISSIONARY_PRESSURE_MULTIPLIER or 1
local g_moveDenominator = GameDefines.MOVE_DENOMINATOR

local g_colorBlack = Color( 0, 0, 0, 1 )
local g_colorWhite = Color( 1, 1, 1, 1 )
local g_colorRed = Color( 1, 0, 0, 1 )
local g_colorYellow = Color( 1, 1, 0.5, 1 )
local g_colorGreen = Color( 0.5, 1, 0.5, 1 )
local g_units, g_cities, g_unitTypes

local EUI_options = Modding.OpenUserData( "Enhanced User Interface Options", 1 )

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

-------------------------------------------------
-- Tooltip Utilities
-------------------------------------------------

local function ShowSimpleTip( ... )
	toolTip = ... and table.concat( {...}, "[NEWLINE]----------------[NEWLINE]" )
	if toolTip then
		g_tipControls.Text:SetText( toolTip )
		g_tipControls.PortraitFrame:SetHide( true )
		g_tipControls.Box:DoAutoSize()
	end
	g_tipControls.Box:SetHide( not toolTip )
end

local function lookAt( plot )
	UI.LookAt( plot )
	local hex = ToHexFromGrid{ x=plot:GetX(), y=plot:GetY() }
	Events.GameplayFX( hex.x, hex.y, -1 )
end

local function TextColor( c, s )
	return c..s.."[ENDCOLOR]"
end

local function UnitColor( s )
	return TextColor("[COLOR_UNIT_TEXT]", s)
end

local function BuildingColor( s )
	return TextColor("[COLOR_YIELD_FOOD]", s)
end

local function PolicyColor( s )
	return TextColor("[COLOR_MAGENTA]", s)
end

local function TechColor( s )
	return TextColor("[COLOR_CYAN]", s)
end

local function ReligionColor( s )
	return TextColor("[COLOR_WHITE]", s)
end

-------------------------------------------------
-- Ribbon Manager
-------------------------------------------------

local table_remove = table.remove
local function g_RibbonManager( name, stack, scrap, createAllItems, initItem, updateItem, callbacks, tooltips )
	local self
	local m=0
	local n=0
	local index = {}

	local function Create( itemID, item )
		if itemID then
			local instance
			n = n + 1
			if n <= #self then
				instance = self[ n ]
				if n > m then
--print("Recycle from scrap", name, m, n, #self, instance, "item", itemID, item and item:GetName() )
					instance.Button:ChangeParent( stack )
--else print("Recycle from stack", name, m, n, #self, instance, "item", itemID, item and item:GetName() )
				end
			else
				instance = {}
--print("Create new ", name, m, n, #self, instance, "item", itemID, item and item:GetName() )
				ContextPtr:BuildInstanceForControl( name, instance, stack )
				-- Setup Tootip Callbacks
				local control
				for name, callback in pairs( tooltips ) do
					control = instance[name]
					if control then
						control:SetToolTipCallback( callback )
						control:SetToolTipType( "EUI_UnitPanelItemTooltip" )
					end
				end
				-- Setup Callbacks
				for name, eventCallbacks in pairs( callbacks ) do
					control = instance[name]
					if control then
						for event, callback in pairs( eventCallbacks ) do
							control:RegisterCallback( event, callback )
						end
					end
				end
				self[n] = instance
			end
			index[ itemID ] = instance
			instance.Button:SetVoid1( itemID )
			instance[1] = itemID
			instance[2] = n
			return initItem( instance, item )
--else print( "Failed attempt to add an item to the list", itemID )
		end
	end

self = {
	Initialize = function( isHidden )
--print("Initializing ", name, " stack", m, n, #self, "hidden ?", isHidden )
		m = n
		n = 0
		index = {}
		if not isHidden then
--print("Initializing ", name, " stack contents" )
			createAllItems( Create )
		end
		for i = n+1, m do
			self[i].Button:ChangeParent( scrap )
		end
		m = 0
	end;

	Create = Create;
	Get = function( itemID )
		return index[ itemID ]
	end;

	Destroy = function( itemID )
		local instance = index[ itemID ]
--print( "Remove item from list", name, m, n, #self, "item", itemID, instance )
		if instance then
			index[ itemID ] = nil
			for i = instance[2] or 0, 1, -1 do
--print("looking for instance", instance, "ID", itemID, instance[1], instance[2], "self", self[i], self[i][1], self[i][2])
				if self[i] == instance then
--print( "Found item to remove at index", i )
					table_remove( self, i )
					instance.Button:ChangeParent( scrap )
					n = n-1
					self[#self+1] = instance
					return
				end
			end
		end
--print( "Failed attempt to remove item", name, itemID, instance )
--for k,v in pairs(index) do print("index[", k,"] = ", v) end
	end;

	Update = function( itemID )
		if itemID then
			local instance = index[ itemID ]
			if instance then
				updateItem( instance )
			end
		else
			for i = 1, n do
				updateItem( self[i] )
			end
		end
	end;

	FindInstance = function( control )
		local controlName = control:GetID()
		for i = 1, n do
			local instance = self[i]
			if instance[ controlName ] == control then
				return instance
			end
		end
	end;

	}
	return self
end

-------------------------------------------------
-- Item Functions
-------------------------------------------------

local function UpdateCity( instance )
	local city = instance and g_activePlayer:GetCityByID( instance[1] )
	if city then
		local itemInfo, portraitOffset, portraitAtlas, buildPercent
		local turnsRemaining = city:GetProductionTurnsLeft()
		local productionNeeded = city:GetProductionNeeded()
		local storedProduction = city:GetProduction() + city:GetOverflowProduction() + city:GetFeatureProduction()
		local orderID, itemID = city:GetOrderFromQueue()
		if orderID == OrderTypes.ORDER_TRAIN then
			itemInfo = GameInfo.Units
			portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon( itemID, g_activePlayerID )
		elseif orderID == OrderTypes.ORDER_CONSTRUCT then
			itemInfo = GameInfo.Buildings
		elseif orderID == OrderTypes.ORDER_CREATE then
			itemInfo = GameInfo.Projects
		elseif orderID == OrderTypes.ORDER_MAINTAIN then
			itemInfo = GameInfo.Processes
			turnsRemaining = nil
			productionNeeded = 0
		end
		if itemInfo then
			local item = itemInfo[ itemID ]
			itemInfo = IconHookup( portraitOffset or item.PortraitIndex, 64, portraitAtlas or item.IconAtlas, instance.BuildIcon )
			if productionNeeded > 0 then
				buildPercent = 1 - math.max( 0, storedProduction/productionNeeded )
			else
				buildPercent = 0
			end
			instance.BuildMeter:SetPercents( 0, buildPercent )
		else
			turnsRemaining = nil
		end
		instance.BuildIcon:SetHide( not itemInfo )
		instance.BuildGrowth:SetString( turnsRemaining )
		instance.Population:SetString( city:GetPopulation() )
		local foodPerTurnTimes100 = city:FoodDifferenceTimes100()
		if foodPerTurnTimes100 < 0 then
			instance.CityGrowth:SetString( " [COLOR_RED]" .. (math.floor( city:GetFoodTimes100() / -foodPerTurnTimes100 ) + 1) .. "[ENDCOLOR] " )
		elseif city:IsForcedAvoidGrowth() then
			instance.CityGrowth:SetString( "[ICON_LOCKED]" )
		elseif city:IsFoodProduction() or foodPerTurnTimes100 == 0 then
			instance.CityGrowth:SetString()
		else
			instance.CityGrowth:SetString( " "..city:GetFoodTurnsLeft().." " )
		end

		local isNotPuppet = not city:IsPuppet()
		local isNotRazing = not city:IsRazing()
		local isNotResistance = not city:IsResistance()
		local isCapital = city:IsCapital()

		instance.CapitalIcon:SetHide( not isCapital )
		instance.PuppetIcon:SetHide( isNotPuppet )
		local cityFocus = isNotRazing and isNotResistance and isNotPuppet and g_CityFocus[city:GetFocusType()]
		instance.FocusIcon:SetText( cityFocus and cityFocus[1] )
		local resource = GameInfo.Resources[city:GetResourceDemanded()]
		instance.QuestIcons:SetText( (city:GetWeLoveTheKingDayCounter() > 0 and "[ICON_HAPPINESS_1]") or (resource and resource.IconString) )
		instance.RazingIcon:SetHide( isNotRazing )
		instance.ResistanceIcon:SetHide( isNotResistance )
		instance.ConnectedIcon:SetHide( not g_activePlayer:IsCapitalConnectedToCity( city ) or isCapital )
		instance.BlockadedIcon:SetHide( not city:IsBlockaded() )
		instance.OccupiedIcon:SetHide( not city:IsOccupied() or city:IsNoOccupiedUnhappiness() )
		instance.Name:SetString( city:GetName() )

		local culturePerTurn = city:GetJONSCulturePerTurn()
		instance.BorderGrowth:SetString( culturePerTurn > 0 and math.ceil( (city:GetJONSCultureThreshold() - city:GetJONSCultureStored()) / culturePerTurn ) )
	end
end

local function FindCity( control )
	local instance = g_cities.FindInstance( control )
	return instance and g_activePlayer:GetCityByID( instance[1] )
end

local function UpdateUnit( instance )
	local unitID = instance[1]
	local unit = g_activePlayer:GetUnitByID( unitID )
	local isSelectedUnit = unitID == UI.GetSelectedUnitID()
	instance.Button:SetHide( isSelectedUnit )
	if unit and not isSelectedUnit then
		local info
		local text = ""
		local buildID = unit:GetBuildType()
		if buildID ~= -1 then -- unit is actively building something
			info = GameInfoBuilds[buildID]
			text = unit:GetPlot():GetBuildTurnsLeft( buildID, g_activePlayerID, 0, 0 )+1
			if text > 9 then text = "" end

		elseif unit:IsEmbarked() then
			info = GameInfoMissions.MISSION_EMBARK

		elseif unit:IsAutomated() then
			if unit:IsWork() then
				info = GameInfoAutomates.AUTOMATE_BUILD
			elseif bnw_mode and unit:IsTrade() then
				info = GameInfoMissions.MISSION_ESTABLISH_TRADE_ROUTE
			else
				info = GameInfoAutomates.AUTOMATE_EXPLORE
			end
		else
			local activityType = unit:GetActivityType()

			if activityType == ActivityTypes.ACTIVITY_SLEEP and unit:IsEverFortifyable() then
				info = GameInfoMissions.MISSION_FORTIFY
			else
				info = g_activityMissions[ activityType ]
			end
		end
		instance.Mission:SetHide(not( info and IconHookup( info.IconIndex, 45, info.IconAtlas, instance.Mission ) ) )
		instance.MissionText:SetText( text )

		local movesLeft = unit:MovesLeft()
		if movesLeft >= unit:MaxMoves() then
			instance.MovementPip:SetTextureOffsetVal( 0, 0 )
		elseif movesLeft > 0 then
			instance.MovementPip:SetTextureOffsetVal( 0, 32 )
		elseif unit:IsCanAttack() or not unit:IsCombatUnit() then
			instance.MovementPip:SetTextureOffsetVal( 0, 96 )
		else
			instance.MovementPip:SetTextureOffsetVal( 0, 64 )
		end
		local damage = unit:GetDamage()
		local color = Color( 1, 1, 1, 1 )
		if damage > 0 then
			local percent = 1 - damage / g_MaxDamage
--			instance.HealthMeter:SetHide(false)
--			instance.HealthMeter:SetPercent( percent )
			color.z = percent
			color.y = percent
--		else
--			instance.HealthMeter:SetHide(true)
		end
		instance.Button:SetColor( color )
	end
end
-- todo: flash text info

local function FilterUnit( unit )
	return unit and g_ActivePlayerUnitsInRibbon[ unit:GetUnitType() ]
end

local function FindUnit( control )
	local instance = g_units.FindInstance( control )
	return instance and g_activePlayer:GetUnitByID( instance[1] )
end

local function UnitToolTip( unit, ... )
	if unit then
		local unitTypeID = unit:GetUnitType()
		local unitInfo = unitTypeID and GameInfoUnits[unitTypeID]

		-- Unit XML stats
		local productionCost = g_activePlayer:GetUnitProductionNeeded( unitTypeID )
		local rangedStrength = unit:GetBaseRangedCombatStrength()
		local unitRange = unit:Range()
		local combatStrength = unitInfo.Combat or 0
		local movesLeft = unit:MovesLeft() / g_moveDenominator
		local maxMoves = unit:MaxMoves() / g_moveDenominator

		local thisUnitType = { UnitType = unitInfo.Type }

		-- Player data
		local city, activeCivilization, activeCivilizationType

		activeCivilization = GameInfo.Civilizations[ g_activePlayer:GetCivilizationType() ]
		activeCivilizationType = activeCivilization and activeCivilization.Type
		city = UI.GetHeadSelectedCity()
		if city and city:GetOwner() ~= g_activePlayerID then
			city = nil
		end
		city = city or g_activePlayer:GetCapitalCity() or g_activePlayer:Cities()(g_activePlayer)

		-- Name
		local combatClass = unitInfo.CombatClass and GameInfo.UnitCombatInfos[unitInfo.CombatClass]
		local tips = table( UnitColor( Locale.ToUpper( unit:GetNameKey() ) ) .. (combatClass and " ("..L(combatClass.Description)..")" or ""), "----------------", ... )

		-- Required Resources:
		local resources = table()
		for resource in GameInfo.Resources() do
			local numResourceRequired = Game.GetNumResourceRequiredForUnit( unitTypeID, resource.ID )
-- TODO				local numResourceAvailable = g_activePlayer:GetNumResourceAvailable(resource.ID, true)
			resources:insertIf( numResourceRequired > 0 and numResourceRequired .. resource.IconString ) -- .. L(resource.Description) )
		end
		tips:insertIf( #resources > 0 and L"TXT_KEY_PEDIA_REQ_RESRC_LABEL" .. " " .. resources:concat(", ") )

		-- Movement:
		tips:insertIf( unitInfo.Domain ~= "DOMAIN_AIR" and L("TXT_KEY_UPANEL_MOVEMENT") .. (" %.3g / %g[ICON_MOVES]"):format( movesLeft, maxMoves ) )

		-- Combat:
		tips:insertIf( combatStrength > 0 and L"TXT_KEY_UPANEL_STRENGTH" .. (" %g[ICON_STRENGTH]"):format( combatStrength ))

		-- Ranged Combat:
		tips:insertIf( rangedStrength > 0 and L"TXT_KEY_UPANEL_RANGED_ATTACK" .. " " .. rangedStrength .. "[ICON_RANGE_STRENGTH]" .. unitRange ) --TXT_KEY_PEDIA_RANGEDCOMBAT_LABEL

		-- Health
		local damage = unit:GetDamage()
		tips:insertIf( damage > 0 and L( "TXT_KEY_UPANEL_SET_HITPOINTS_TT", g_MaxDamage-damage, g_MaxDamage ) )

		-- XP
		tips:insertIf( (unit:IsCombatUnit() or unit:GetDomainType() == DomainTypes.DOMAIN_AIR) and L("TXT_KEY_UNIT_EXPERIENCE_INFO", unit:GetLevel(), unit:GetExperience(), unit:ExperienceNeeded() ) )
		--L( "TXT_KEY_EXPERIENCE_POPUP", unit:GetExperience().."/"..unit:ExperienceNeeded() ) )

		-- Abilities:	--TXT_KEY_PEDIA_FREEPROMOTIONS_LABEL
		for unitPromotion in GameInfo.UnitPromotions() do
			if unit:IsHasPromotion(unitPromotion.ID) and not (bnw_mode and unit:IsTrade()) then
				tips:insert( "[ICON_BULLET]"..L(unitPromotion.Description) )-- L(unitPromotion.Help) )
			end
		end

		-- Ability to create building in city (e.g. vanilla great general)
		for row in GameInfo.Unit_Buildings( thisUnitType ) do
			local building = row.BuildingType and GameInfo.Buildings[row.BuildingType]
			tips:insertIf( building and "[ICON_BULLET]"..L"TXT_KEY_MISSION_CONSTRUCT".." " .. BuildingColor( L(building.Description) ) )
		end

		-- Actions	--TXT_KEY_PEDIA_WORKER_ACTION_LABEL
		for row in GameInfo.Unit_Builds( thisUnitType ) do
			local build = row.BuildType and GameInfo.Builds[row.BuildType]
			if build then
				local buildImprovement = build.ImprovementType and GameInfo.Improvements[ build.ImprovementType ]
				local requiredCivilizationType = buildImprovement and buildImprovement.CivilizationType
				if not requiredCivilizationType or GameInfoTypes[ requiredCivilizationType ] == g_activePlayer:GetCivilizationType() then
					local prereqTech = build.PrereqTech and GameInfo.Technologies[build.PrereqTech]
					tips:insertIf( (not prereqTech or g_activeTechs:HasTech( prereqTech.ID or -1 ) ) and "[ICON_BULLET]" .. L(build.Description) )
				end
			end
		end

		-- Great Engineer
		tips:insertIf( unitInfo.BaseHurry > 0 and "[ICON_BULLET]" .. L"TXT_KEY_MISSION_HURRY_PRODUCTION" .. (" %g[ICON_PRODUCTION]%+g[ICON_PRODUCTION]/[ICON_CITIZEN]"):format( unitInfo.BaseHurry, unitInfo.HurryMultiplier or 0 ) )

		-- Great Merchant
		tips:insertIf( unitInfo.BaseGold > 0 and "[ICON_BULLET]" .. L"TXT_KEY_MISSION_CONDUCT_TRADE_MISSION" .. (" %g[ICON_GOLD]%+g[ICON_INFLUENCE]" ):format( unitInfo.BaseGold + ( unitInfo.NumGoldPerEra or 0 ) * ( Game and Teams[Game.GetActiveTeam()]:GetCurrentEra() or PreGame.GetEra() ), GameDefines.MINOR_FRIENDSHIP_FROM_TRADE_MISSION or 0 ) )

		-- Becomes Obsolete with:
		local obsoleteTech = unitInfo.ObsoleteTech and GameInfo.Technologies[unitInfo.ObsoleteTech]
		tips:insertIf( obsoleteTech and L"TXT_KEY_PEDIA_OBSOLETE_TECH_LABEL" .. " " .. TechColor( L(obsoleteTech.Description) ) )

		-- Upgrade unit
		local upgradeUnitTypeID = unit:GetUpgradeUnitType()
		local unitUpgradeInfo = upgradeUnitTypeID and GameInfo.Units[upgradeUnitTypeID]
		tips:insertIf( unitUpgradeInfo and L"TXT_KEY_COMMAND_UPGRADE" .. ": " .. UnitColor( L(unitUpgradeInfo.Description) ) .. " ("..unit:UpgradePrice(upgradeUnitTypeID).."[ICON_GOLD])" )

		g_tipControls.Text:SetText( tips:concat( "[NEWLINE]" ) )
		local portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon( unit )
		IconHookup( portraitOffset, g_tipControls.Portrait:GetSizeY(), portraitAtlas, g_tipControls.Portrait )
		g_tipControls.PortraitFrame:SetHide( false )
		g_tipControls.Box:DoAutoSize()
	end
	g_tipControls.Box:SetHide( not unit )
end

-------------------------------------------------
-- Unit Ribbon "Object"
-------------------------------------------------
g_units = g_RibbonManager( "UnitInstance", Controls.UnitStack, Controls.Scrap,
	function( Create ) -- CREATE ALL
		for unit in g_activePlayer:Units() do
			if FilterUnit( unit ) then
				Create( unit:GetID(), unit )
			end
		end
	end,
	function( instance, unit ) -- INIT
		local portrait, portraitOffset, portraitAtlas = instance.Portrait, UI.GetUnitPortraitIcon( unit )
		portrait:SetHide(not ( portraitOffset and portraitAtlas and IconHookup( portraitOffset, portrait:GetSizeX(), portraitAtlas, portrait ) ) )
		return UpdateUnit( instance )
	end,
	UpdateUnit, -- UPDATE
-------------------------------------------------------------------------------------------------
{-- the callback function table names need to match associated instance control ID defined in xml
	Button = {
		[Mouse.eLClick] = function( unitID )
			local unit = g_activePlayer:GetUnitByID( unitID )
			if unit then
				UI.SelectUnit( unit )
				UI.LookAtSelectionPlot( )
			end
		end,
		[Mouse.eRClick] = function( unitID )
			local unit = g_activePlayer:GetUnitByID( unitID )
			if unit then
				lookAt( unit:GetPlot() )
			end
		end,
	},--/Button
},--/unit callbacks
------------------------------------------------------------------------------------------
{-- the tooltip function names need to match associated instance control ID defined in xml
	Button = function( control )
		UnitToolTip( FindUnit( control ) )
	end;
	MovementPip = function( control )
		local unit = FindUnit( control )
		ShowSimpleTip( unit and L"TXT_KEY_UPANEL_MOVEMENT"..(" %.3g / %g [ICON_MOVES]"):format( unit:MovesLeft() / g_moveDenominator, unit:MaxMoves() / g_moveDenominator )
--[[
.." GetActivityType="..tostring(unit:GetActivityType())
.." GetFortifyTurns="..tostring(unit:GetFortifyTurns())
.." HasMoved="..tostring(unit:HasMoved())
.." IsReadyToMove="..tostring(unit:IsReadyToMove())
.." IsWaiting="..tostring(unit:IsWaiting())
.." IsAutomated="..tostring(unit:IsAutomated())
--]]
		)
	end;
	Mission = function( control )
		local unit = FindUnit( control )
		local status
		if unit then
			local buildID = unit:GetBuildType()
			local activityType = unit:GetActivityType()
			if buildID ~= -1 then -- this is a worker who is actively building something
				local info = GameInfoBuilds[buildID]
				status = L(info.Description).. " ("..(unit:GetPlot():GetBuildTurnsLeft( buildID, g_activePlayerID, 0, 0 )+1)..")"

			elseif unit:IsEmbarked() then
				status = "TXT_KEY_MISSION_EMBARK_HELP" --"TXT_KEY_UNIT_STATUS_EMBARKED"

			elseif unit:IsAutomated() then
				if unit:IsWork() then
					status = "TXT_KEY_ACTION_AUTOMATE_BUILD"
				elseif bnw_mode and unit:IsTrade() then
					status = "TXT_KEY_ACTION_AUTOMATE_TRADE"
				else
					status = "TXT_KEY_ACTION_AUTOMATE_EXPLORE"
				end

			elseif activityType == ActivityTypes.ACTIVITY_HEAL then
				status = "TXT_KEY_MISSION_HEAL_HELP"

			elseif activityType == ActivityTypes.ACTIVITY_SENTRY then
				status = "TXT_KEY_MISSION_ALERT_HELP"

			elseif activityType == ActivityTypes.ACTIVITY_SLEEP then -- sleep is either sleep or fortify
				if unit:IsGarrisoned() then
					status = "TXT_KEY_MISSION_GARRISON_HELP"
				elseif unit:IsEverFortifyable() then
					status = L"TXT_KEY_UNIT_STATUS_FORTIFIED" .. (" %+i"):format(unit:FortifyModifier()).."%[ICON_DEFENSE]"
				else
					status = "TXT_KEY_MISSION_SLEEP_HELP"
				end
			elseif activityType == ActivityTypes.ACTIVITY_INTERCEPT then
				status = "TXT_KEY_MISSION_INTERCEPT_HELP"

			elseif activityType == ActivityTypes.ACTIVITY_HOLD then
				status = "TXT_KEY_MISSION_SKIP_HELP"

			elseif activityType == ActivityTypes.ACTIVITY_MISSION then
				status = "TXT_KEY_INTERFACEMODE_MOVE_TO_HELP"
			else
				status = "Error - unkown unit state"
			end
		end
		ShowSimpleTip( unit and L(status) )
	end;
}--/units tooltips
)--/unit ribbon object

-------------------------------------------------
-- City Ribbon "Object"
-------------------------------------------------
local function ShowSimpleCityTip( control, city, tip, ... )
	return ShowSimpleTip( city:GetName() .. "[NEWLINE]" .. tip, ... )
end

g_cities = g_RibbonManager( "CityInstance", Controls.CityStack, Controls.Scrap,
	function( Create ) -- CREATE ALL
		for city in g_activePlayer:Cities() do
			Create( city:GetID(), city )
		end
	end,
	UpdateCity, -- INIT
	UpdateCity, -- UPDATE
-------------------------------------------------------------------------------------------------
{-- the callback function table names need to match associated instance control ID defined in xml
	Button = {
		[Mouse.eLClick] = function( cityID )
			local city = g_activePlayer:GetCityByID( cityID )
			if city then
				UI.DoSelectCityAtPlot( city:Plot() )
			end
		end,
		[Mouse.eRClick] = function( cityID )
			local city = g_activePlayer:GetCityByID( cityID )
			if city then
				lookAt( city:Plot() )
			end
		end,
	},--/Button
},--/city callbacks
------------------------------------------------------------------------------------------
{-- the tooltip function names need to match associated instance control ID defined in xml
	Button = function( control )
		local city = FindCity( control )
		local itemInfo, strToolTip, strDisabledInfo, portraitOffset, portraitAtlas
		if city then
			local orderID, itemID = city:GetOrderFromQueue()
			local cash, icon
			if orderID == OrderTypes.ORDER_TRAIN then
				itemInfo = GameInfo.Units
				portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon( itemID, g_activePlayerID )
				strToolTip = GetHelpTextForUnit( itemID, true )

			elseif orderID == OrderTypes.ORDER_CONSTRUCT then
				itemInfo = GameInfo.Buildings
				strToolTip = GetHelpTextForBuilding( itemID, false, false, city:GetNumFreeBuilding(itemID) > 0, city )

			elseif orderID == OrderTypes.ORDER_CREATE then
				itemInfo = GameInfo.Projects
				strToolTip = GetHelpTextForProject( itemID, true )
			elseif orderID == OrderTypes.ORDER_MAINTAIN then
				itemInfo = GameInfo.Processes
				strToolTip = GetHelpTextForProcess( itemID, true )
			end
			if strToolTip and city:GetOrderQueueLength() > 0 then
				strToolTip = L( "TXT_KEY_CITY_CURRENTLY_PRODUCING_TT", city:GetName(), city:GetProductionNameKey(), city:GetProductionTurnsLeft() ) .. "[NEWLINE]----------------[NEWLINE]" .. strToolTip
			else
				strToolTip = L( "TXT_KEY_CITY_NOT_PRODUCING", city:GetName() )
			end
			local item = itemInfo and itemInfo[itemID]
			item = item and IconHookup( portraitOffset or item.PortraitIndex, g_tipControls.Portrait:GetSizeY(), portraitAtlas or item.IconAtlas, g_tipControls.Portrait )
			g_tipControls.Text:SetText( strToolTip )
			g_tipControls.PortraitFrame:SetHide( not item )
			g_tipControls.Box:DoAutoSize()
		end
		return g_tipControls.Box:SetHide( not strToolTip )
	end,
	BuildGrowth = function( control )
		local city = FindCity( control )
		ShowSimpleTip( city and L( "TXT_KEY_CITY_CURRENTLY_PRODUCING_TT", city:GetName(), city:GetProductionNameKey(), city:GetProductionTurnsLeft() ), GetProductionTooltip( city ) )
	end,
	Population = function( control )
		local city = FindCity( control )
		ShowSimpleCityTip( control, city, city:GetPopulation() .. "[ICON_CITIZEN] ".. L( "TXT_KEY_CITYVIEW_CITIZENS_TEXT", city:GetPopulation() ), GetFoodTooltip( city ) )
	end,
	CityGrowth = function( control )
		local city = FindCity( control )
		if not city then
			return ShowSimpleTip()
		end
		local foodPerTurnTimes100 = city:FoodDifferenceTimes100()
		local tip
		if foodPerTurnTimes100 < 0 then
			tip = L( "TXT_KEY_NTFN_CITY_STARVING", city:GetName() )
		elseif city:IsForcedAvoidGrowth() then
			tip = L"TXT_KEY_CITYVIEW_FOCUS_AVOID_GROWTH_TEXT"
		elseif city:IsFoodProduction() or foodPerTurnTimes100 == 0 then
			tip = L"TXT_KEY_CITYVIEW_STAGNATION_TEXT"
		else
			tip = L( "TXT_KEY_CITYVIEW_TURNS_TILL_CITIZEN_TEXT", city:GetFoodTurnsLeft() )
		end
		ShowSimpleCityTip( control, city, tip, GetFoodTooltip( city ) )
	end,
	BorderGrowth = function( control )
		local city = FindCity( control )
		ShowSimpleCityTip( control, city, L("TXT_KEY_CITYVIEW_TURNS_TILL_TILE_TEXT", math.ceil( (city:GetJONSCultureThreshold() - city:GetJONSCultureStored()) / city:GetJONSCulturePerTurn() ) ), GetCultureTooltip( city ) )
	end,
	CapitalIcon = function( control )
		local city = FindCity( control )
		ShowSimpleCityTip( control, city, "[ICON_CAPITAL]" )
	end,
	PuppetIcon = function( control )
		local city = FindCity( control )
		ShowSimpleCityTip( control, city, L"TXT_KEY_CITY_PUPPET", L"TXT_KEY_CITY_ANNEX_TT" )
	end,
	FocusIcon = function( control )
		local city = FindCity( control )
		local cityFocus = g_CityFocus[city:GetFocusType()]
		ShowSimpleCityTip( control, city, cityFocus[2] )
	end,
	QuestIcons = function( control )
		local city = FindCity( control )
		local resource = GameInfo.Resources[city:GetResourceDemanded()]
		local weLoveTheKingDayCounter = city:GetWeLoveTheKingDayCounter()
		local tip
		-- We love the king
		if weLoveTheKingDayCounter > 0 then
			tip = L( "TXT_KEY_CITYVIEW_WLTKD_COUNTER", weLoveTheKingDayCounter )

		elseif resource then
			tip = L( "TXT_KEY_CITYVIEW_RESOURCE_DEMANDED", resource.IconString .. " " .. L(resource.Description) )
		end
		ShowSimpleCityTip( control, city, tip )
	end,
	RazingIcon = function( control )
		local city = FindCity( control )
		ShowSimpleCityTip( control, city, L( "TXT_KEY_CITY_BURNING", city:GetRazingTurns() ) )
	end,
	ResistanceIcon = function( control )
		local city = FindCity( control )
		ShowSimpleCityTip( control, city, L( "TXT_KEY_CITY_RESISTANCE", city:GetResistanceTurns() ) )
	end,
	ConnectedIcon = function( control )
		local city = FindCity( control )
		ShowSimpleCityTip( control, city, L("TXT_KEY_CITY_CONNECTED") .. (" (%+g[ICON_GOLD])"):format( ( bnw_mode and g_activePlayer:GetCityConnectionRouteGoldTimes100( city ) or g_activePlayer:GetRouteGoldTimes100( city ) ) / 100 ) ) -- stupid function renaming
	end,
	BlockadedIcon = function( control )
		local city = FindCity( control )
		ShowSimpleCityTip( control, city, L"TXT_KEY_CITY_BLOCKADED" )
	end,
	OccupiedIcon = function( control )
		local city = FindCity( control )
		ShowSimpleCityTip( control, city, L"TXT_KEY_CITY_OCCUPIED" )
	end,

}--/city tooltips
)--/city ribbon object

--[[ 
 _   _       _ _     ____                  _ 
| | | |_ __ (_) |_  |  _ \ __ _ _ __   ___| |
| | | | '_ \| | __| | |_) / _` | '_ \ / _ \ |
| |_| | | | | | |_  |  __/ (_| | | | |  __/ |
 \___/|_| |_|_|\__| |_|   \__,_|_| |_|\___|_|
]]

local g_screenWidth , g_screenHeight = UIManager:GetScreenSizeVal()
local g_topOffset0 = Controls.CityPanel:GetOffsetY()
local g_topOffset = g_topOffset0
local g_bottomOffset0 = Controls.UnitPanel:GetOffsetY()
local g_bottomOffset = g_bottomOffset0

local g_ActionIM    = InstanceStackManager( "UnitAction",  "UnitActionButton", Controls.ActionStack )
local g_BuildIM  = InstanceStackManager( "UnitAction",  "UnitActionButton", Controls.WorkerActionStack )
local g_EarnedPromotionIM   = InstanceStackManager( "EarnedPromotionInstance", "EarnedPromotion", Controls.EarnedPromotionStack )

local g_lastUnitID = -1		-- Used to determine if a different unit has been selected.
local g_isWorkerActionPanelOpen = false

local g_unitPortraitSize = Controls.UnitPortrait:GetSizeX()

local g_recommendedActionButton = {}
ContextPtr:BuildInstanceForControlAtIndex( "UnitAction", g_recommendedActionButton, Controls.WorkerActionStack, 1 )
--Controls.RecommendedActionLabel:ChangeParent( g_recommendedActionButton.UnitActionButton )
local g_actionIconSize = g_recommendedActionButton.UnitActionIcon:GetSizeX()

--------------------------------------------------------------------------------
-- this maps from the normal instance names to the build city control names
-- so we can use the same code to set it up
--------------------------------------------------------------------------------
local g_BuildCityControlMap = {
	UnitActionButton    = Controls.BuildCityButton,
	--UnitActionMouseover = Controls.BuildCityMouseover,
	--UnitActionText      = Controls.BuildCityText,
	--UnitActionHotKey    = Controls.BuildCityHotKey,
	--UnitActionHelp      = Controls.BuildCityHelp,
}

local g_directionTypes = {
	DirectionTypes.DIRECTION_NORTHEAST,
	DirectionTypes.DIRECTION_EAST,
	DirectionTypes.DIRECTION_SOUTHEAST,
	DirectionTypes.DIRECTION_SOUTHWEST,
	DirectionTypes.DIRECTION_WEST,
	DirectionTypes.DIRECTION_NORTHWEST
}

local g_actionIconIndexAndAtlas = {
	[ActionSubTypes.ACTIONSUBTYPE_PROMOTION] = function(action)
		local thisPromotion = GameInfo.UnitPromotions[action.CommandData]
		return thisPromotion.PortraitIndex, thisPromotion.IconAtlas
	end,

	[ActionSubTypes.ACTIONSUBTYPE_INTERFACEMODE] = function(action)
		local info = GameInfo.InterfaceModes[action.Type]
		return info.IconIndex, info.IconAtlas
	end,

	[ActionSubTypes.ACTIONSUBTYPE_MISSION] = function(action)
		local info = GameInfo.Missions[action.Type]
		return info.IconIndex, info.IconAtlas
	end,

	[ActionSubTypes.ACTIONSUBTYPE_COMMAND] = function(action)
		local info = GameInfo.Commands[action.Type]
		return info.IconIndex, info.IconAtlas
	end,

	[ActionSubTypes.ACTIONSUBTYPE_AUTOMATE] = function(action)
		local info = GameInfo.Automates[action.Type]
		return info.IconIndex, info.IconAtlas
	end,

	[ActionSubTypes.ACTIONSUBTYPE_BUILD] = function(action)
		local info = GameInfoBuilds[action.Type]
		return info.IconIndex, info.IconAtlas
	end,

	[ActionSubTypes.ACTIONSUBTYPE_CONTROL] = function(action)
		local info = GameInfo.Controls[action.Type]
		return info.IconIndex, info.IconAtlas
	end,
}
g_yieldString = {
[YieldTypes.YIELD_FOOD] = "TXT_KEY_BUILD_FOOD_STRING",
[YieldTypes.YIELD_PRODUCTION] = "TXT_KEY_BUILD_PRODUCTION_STRING",
[YieldTypes.YIELD_GOLD] = "TXT_KEY_BUILD_GOLD_STRING",
[YieldTypes.YIELD_SCIENCE] = "TXT_KEY_BUILD_SCIENCE_STRING",
}
if gk_mode then
	g_yieldString[YieldTypes.YIELD_CULTURE] = "TXT_KEY_BUILD_CULTURE_STRING"
	g_yieldString[YieldTypes.YIELD_FAITH] = "TXT_KEY_BUILD_FAITH_STRING"
end
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
local function OnUnitActionClicked( actionID )
	local action = GameInfoActions[actionID]
	if action and g_activePlayer:IsTurnActive() then
		if action.SubType == ActionSubTypes.ACTIONSUBTYPE_PROMOTION then
			Events.AudioPlay2DSound("AS2D_INTERFACE_UNIT_PROMOTION")
		end
		Game.HandleAction( actionID )
	end
end

--------------------------------------------------------------------------------
-- Refresh unit actions
--------------------------------------------------------------------------------
local function UpdateUnitActions( unit )

	g_ActionIM:ResetInstances()
	g_BuildIM:ResetInstances()
	Controls.WorkerActionPanel:SetHide(true)

	local hasMovesLeft = unit:MovesLeft() > 0
	local unitPlot = unit:GetPlot()

	local hasBuildAction, hasPromotion, recommendedBuild

	Controls.BackgroundCivFrame:SetHide( false )

	local hideCityButton = true
	local currentBuildID = unit:GetBuildType()

	-- loop over all the game actions
	local actions = {}
	for actionID = 0, #GameInfoActions do
		local action = GameInfoActions[ actionID ]
		-- test CanHandleAction w/ visible flag (ie COULD train if ... )
		if action.Visible
			and Game.CanHandleAction( actionID, unitPlot, true )
			-- We hide the Action buttons when Units are out of moves so new players aren't confused
			and ( hasMovesLeft
			or action.Type == "COMMAND_CANCEL"
			or action.Type == "COMMAND_STOP_AUTOMATION"
			or action.SubType == ActionSubTypes.ACTIONSUBTYPE_PROMOTION )
		then
			action.ID = actionID
			if action.SubType == ActionSubTypes.ACTIONSUBTYPE_BUILD
				or action.Type == "INTERFACEMODE_ROUTE_TO"
				or action.Type == "AUTOMATE_BUILD"
			then
				action.isBuild = true
			else
				action.isBuild = false
			end
			table.insert(actions, action)
			if action.SubType == ActionSubTypes.ACTIONSUBTYPE_PROMOTION then
				hasPromotion = true
			end
		end
	end

	-- loop over all the unit actions
	for i = 1, #actions do
		local action = actions[i]

		local button
		local extraXOffset = 0
--		if UI.IsTouchScreenEnabled() then
--			extraXOffset = 44
--		end
		if action.Type == "MISSION_FOUND" then
			button = Controls.BuildCityButton
			hideCityButton = false

		else
			local instance
			if action.SubType == ActionSubTypes.ACTIONSUBTYPE_PROMOTION then
				instance = g_BuildIM:GetInstance()

			elseif ( action.SubType == ActionSubTypes.ACTIONSUBTYPE_BUILD
				or action.Type == "INTERFACEMODE_ROUTE_TO"
				or action.Type == "AUTOMATE_BUILD" ) and not hasPromotion then

				hasBuildAction = true
				if not recommendedBuild and unit:IsActionRecommended( action.ID ) then

					recommendedBuild = action.ID
					instance = g_recommendedActionButton
					Controls.RecommendedActionLabel:SetText( (bnw_mode and (L("TXT_KEY_UPANEL_RECOMMENDED") .. "[NEWLINE]") or "") .. L( tostring( action.TextKey or action.Type ) ) )
				else
					instance = g_BuildIM:GetInstance()
				end
			else
				instance = g_ActionIM:GetInstance()
			end
			local f = g_actionIconIndexAndAtlas[action.SubType]
			if f then
				local iconIndex, iconAtlas = f(action)
				IconHookup( iconIndex, g_actionIconSize, iconAtlas, instance.UnitActionIcon )
			else
				print("Error - could not find method to obtain action icon", action.Type, action.SubType)
			end
			local buildInProgress
			if action.SubType == ActionSubTypes.ACTIONSUBTYPE_BUILD then
--				local build = GameInfoBuilds[action.Type]
--				local buildID = build.ID
				local buildID = action.MissionData
				local extraBuildRate = 0
				local turnsTotal
				if currentBuildID == -1 or buildID ~= currentBuildID then
					extraBuildRate = unit:WorkRate( true, buildID )
					turnsTotal = unitPlot:GetBuildTime( buildID, g_activePlayerID ) / extraBuildRate
				else
					turnsTotal = unitPlot:GetBuildTurnsTotal( buildID, g_activePlayerID )
				end
				local turnsLeft = unitPlot:GetBuildTurnsLeft( buildID, g_activePlayerID, extraBuildRate, extraBuildRate ) + 1
				if turnsLeft <= turnsTotal and turnsLeft > 0 then
					buildInProgress = turnsLeft
					instance.WorkerProgressBar:SetPercent( (turnsTotal - turnsLeft) / turnsTotal )
				end
			end
			instance.WorkerProgressBar:SetHide( not buildInProgress )
			instance.Text:SetText( buildInProgress )
			button = instance.UnitActionButton
		end

		-- test w/o visible flag (ie can train right now)
		if not Game.CanHandleAction( action.ID, unitPlot, false ) then
			button:SetAlpha( 0.6 )
			button:SetDisabled( true )
		else
			button:SetAlpha( 1.0 )
			button:SetDisabled( false )
		end

		button:RegisterCallback( Mouse.eLClick, OnUnitActionClicked )
		button:SetVoid1( action.ID )
		button:SetToolTipCallback( ActionToolTipHandler )

	end
	Controls.BuildCityButton:SetHide( hideCityButton )
	g_ActionIM:Commit()
	g_BuildIM:Commit()

	Controls.ActionStack:SetHide( false )
	Controls.ActionStack:CalculateSize()
	Controls.ActionStack:ReprocessAnchoring()

	Controls.PrimaryStack:CalculateSize()
	Controls.PrimaryStack:ReprocessAnchoring()


	--Controls.BuildStack:CalculateSize()
	--Controls.BuildStack:ReprocessAnchoring()
	if hasBuildAction or hasPromotion then
		Controls.WorkerActionPanel:SetHide( false )
		g_isWorkerActionPanelOpen = true

		Controls.RecommendedActionLabel:SetHide( not recommendedBuild )
		g_recommendedActionButton.UnitActionButton:SetHide( not recommendedBuild )
		Controls.RecommendedActionDivider:SetHide( not recommendedBuild )
		Controls.WorkerText:SetHide( hasPromotion )
		Controls.PromotionText:SetHide( not hasPromotion )
		Controls.PromotionAnimation:SetHide( not hasPromotion )
		Controls.EditButton:SetHide( not hasPromotion )

		Controls.WorkerActionStack:CalculateSize()
		local x, y = Controls.WorkerActionStack:GetSizeVal()
		Controls.WorkerActionPanel:SetSizeVal( math.max( x + 50, 290), y + 100 )
		Controls.WorkerActionStack:ReprocessAnchoring()
	else
		Controls.WorkerActionPanel:SetHide( true )
		g_isWorkerActionPanelOpen = false
	end
end

local defaultErrorTextureSheet = "TechAtlasSmall.dds"
local nullOffset = { x=0, y=0 }

--------------------------------------------------------------------------------
-- Refresh unit portrait and name
--------------------------------------------------------------------------------
local function UpdateUnitPortrait( unit )

	local name
	if unit:IsGreatPerson() then
		name = unit:GetNameNoDesc()
		if not name or #name == 0 then
			name = unit:GetName()
		end
	else
		name = unit:GetName()
	end

	name = Locale.ToUpper(name)

	--local name = unit:GetNameKey()
	local convertedKey = L(name)
	convertedKey = Locale.ToUpper(convertedKey)

	Controls.UnitName:SetText(convertedKey)
	Controls.UnitName:SetFontByName("TwCenMT24")

	local name_length = Controls.UnitName:GetSizeVal()
	local box_length = Controls.UnitNameButton:GetSizeVal()

	if name_length > (box_length - 50) then
		Controls.UnitName:SetFontByName("TwCenMT20")
	end

	name_length = Controls.UnitName:GetSizeVal()

	if name_length > (box_length - 50) then
		Controls.UnitName:SetFontByName("TwCenMT14")
	end

	-- XP
	if unit:IsCombatUnit() or unit:GetDomainType() == DomainTypes.DOMAIN_AIR then
		local iExperience = unit:GetExperience()

		local iLevel = unit:GetLevel()
		local iExperienceNeeded = unit:ExperienceNeeded()
		local xpString = L("TXT_KEY_UNIT_EXPERIENCE_INFO", iLevel, iExperience, iExperienceNeeded)
		Controls.XPMeter:SetToolTipString( xpString )
		Controls.XPMeter:SetPercent( iExperience / iExperienceNeeded )
		Controls.XPFrame:SetHide( false )
	else
		Controls.XPFrame:SetHide( true )
	end

	local thisUnitInfo = GameInfoUnits[unit:GetUnitType()]

	local flagOffset, flagAtlas = UI.GetUnitFlagIcon(unit)
	local textureOffset, textureAtlas = IconLookup( flagOffset, 32, flagAtlas )
	Controls.UnitIcon:SetTexture(textureAtlas)
	Controls.UnitIconShadow:SetTexture(textureAtlas)
	Controls.UnitIcon:SetTextureOffset(textureOffset)
	Controls.UnitIconShadow:SetTextureOffset(textureOffset)

	local unitOwnerID = unit:GetOwner()
	Controls.UnitIcon:SetColor( PrimaryColors[ unitOwnerID ] )
	Controls.UnitIconBackground:SetColor( BackgroundColors[ unitOwnerID ] )

	local portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon( unit )
	textureOffset, textureAtlas = IconLookup( portraitOffset, g_unitPortraitSize, portraitAtlas )
	Controls.UnitPortrait:SetTexture(textureAtlas or nullOffset)
	Controls.UnitPortrait:SetTextureOffset(textureOffset or defaultErrorTextureSheet)

	--These controls are potentially hidden if the previous selection was a city.
	Controls.UnitTypeFrame:SetHide(false)
	Controls.CycleLeft:SetHide(false)
	Controls.CycleRight:SetHide(false)
	Controls.UnitStatBox:SetHide(false)

end

local function UpdateCityPortrait(city)
	local name = city:GetName()
	name = Locale.ToUpper(name)
	--local name = unit:GetNameKey()
	local convertedKey = L(name)
	convertedKey = Locale.ToUpper(convertedKey)

	Controls.UnitName:SetText(convertedKey)
	Controls.UnitName:SetFontByName("TwCenMT24")

	local name_length = Controls.UnitName:GetSizeVal()
	local box_length = Controls.UnitNameButton:GetSizeVal()
	if name_length > (box_length - 50) then
		Controls.UnitName:SetFontByName("TwCenMT20")
		name_length = Controls.UnitName:GetSizeVal()
		if name_length > (box_length - 50) then
			Controls.UnitName:SetFontByName("TwCenMT14")
		end
	end

	Controls.UnitPortrait:SetToolTipString()

	local textureOffset, textureAtlas = IconLookup( 0, g_unitPortraitSize, "CITY_ATLAS" )
	Controls.UnitPortrait:SetTexture(textureAtlas or nullOffset)
	Controls.UnitPortrait:SetTextureOffset(textureOffset or defaultErrorTextureSheet)


	--Hide various aspects of Unit Panel since they don't apply to the city.
	--Clear promotions
	g_EarnedPromotionIM:ResetInstances()
	g_EarnedPromotionIM:Commit()

	Controls.UnitTypeFrame:SetHide(true)
	Controls.CycleLeft:SetHide(true)
	Controls.CycleRight:SetHide(true)
	Controls.XPFrame:SetHide( true )
	Controls.UnitStatBox:SetHide(true)
	Controls.WorkerActionPanel:SetHide(true)
	Controls.ActionStack:SetHide( true )
	g_isWorkerActionPanelOpen = false
end


--------------------------------------------------------------------------------
-- Refresh unit promotions
--------------------------------------------------------------------------------
local function UpdateUnitPromotions(unit)

	g_EarnedPromotionIM:ResetInstances()
	local controlTable

	--For each avail promotion, display the icon
	for unitPromotion in GameInfo.UnitPromotions() do
		local unitPromotionID = unitPromotion.ID

		if unit:IsHasPromotion(unitPromotionID) and not (bnw_mode and unit:IsTrade()) then

			controlTable = g_EarnedPromotionIM:GetInstance()
			IconHookup( unitPromotion.PortraitIndex, 32, unitPromotion.IconAtlas, controlTable.UnitPromotionImage )

			-- Tooltip
			controlTable.EarnedPromotion:SetToolTipString( L(unitPromotion.Description) .. "[NEWLINE][NEWLINE]" .. L(unitPromotion.Help) )
		end
	end
	g_EarnedPromotionIM:Commit()
end

---------------------------------------------------
---- Promotion Help
---------------------------------------------------
--local function PromotionHelpOpen(iPromotionID)
	--local pPromotionInfo = GameInfo.UnitPromotions[iPromotionID]
	--local promotionHelp = L(pPromotionInfo.Description)
	--Controls.HelpText:SetText(promotionHelp)
--end

--------------------------------------------------------------------------------
-- Refresh unit stats
--------------------------------------------------------------------------------
local function UpdateUnitStats(unit)

	-- update the background image (update this if we get icons for the minors)
	local civType = unit:GetCivilizationType()
	local civInfo = GameInfo.Civilizations[civType]
	local civPortraitIndex = civInfo.PortraitIndex
	if civPortraitIndex < 0 or civPortraitIndex > 21 then
		civPortraitIndex = 22
	end

	IconHookup( civPortraitIndex, 128, civInfo.IconAtlas, Controls.BackgroundCivSymbol )

	-- Movement
	if bnw_mode and unit:IsTrade() then
		Controls.UnitStatMovement:SetText()

	elseif unit:GetDomainType() == DomainTypes.DOMAIN_AIR then
		local unitRange = unit:Range()
		Controls.UnitStatMovement:SetText( unitRange .. "[ICON_MOVES]" )
		Controls.UnitStatMovement:LocalizeAndSetToolTip( "TXT_KEY_UPANEL_UNIT_MAY_STRIKE_REBASE", unitRange, unitRange * g_rebaseRangeMultipler / 100 )

	else
		local movesLeft = (" %.3g/%g"):format( unit:MovesLeft() / g_moveDenominator, unit:MaxMoves() / g_moveDenominator )

		Controls.UnitStatMovement:SetText( movesLeft.."[ICON_MOVES]" )

		Controls.UnitStatMovement:LocalizeAndSetToolTip( "TXT_KEY_UPANEL_UNIT_MAY_MOVE", movesLeft )
	end

	-- Strength
	local strength = 0
	if unit:GetDomainType() == DomainTypes.DOMAIN_AIR then
		strength = unit:GetBaseRangedCombatStrength()
	elseif not unit:IsEmbarked() then
		strength = unit:GetBaseCombatStrength()
	end
	if strength > 0 then
		Controls.UnitStatStrength:SetText( strength .. "[ICON_STRENGTH]" )
		Controls.UnitStatStrength:LocalizeAndSetToolTip( "TXT_KEY_UPANEL_STRENGTH_TT" )
	-- Religious units
	elseif gk_mode and unit:GetSpreadsLeft() > 0 then
		Controls.UnitStatStrength:SetText( math.floor(unit:GetConversionStrength()/g_pressureMultiplier) .. "[ICON_PEACE]" )
		Controls.UnitStatStrength:LocalizeAndSetToolTip( "TXT_KEY_UPANEL_RELIGIOUS_STRENGTH_TT" )
	elseif bnw_mode and unit:GetTourismBlastStrength() > 0 then
		Controls.UnitStatStrength:SetText( unit:GetTourismBlastStrength() .. "[ICON_TOURISM]" )
		Controls.UnitStatStrength:LocalizeAndSetToolTip( "TXT_KEY_UPANEL_TOURISM_STRENGTH_TT" )
	else
		Controls.UnitStatStrength:SetText()
	end

	-- Ranged Strength
	local rangedStrength = unit:GetDomainType() ~= DomainTypes.DOMAIN_AIR and unit:GetBaseRangedCombatStrength() or 0
	if rangedStrength > 0 then
		Controls.UnitStatRangedAttack:SetText( rangedStrength .. "[ICON_RANGE_STRENGTH]" .. unit:Range() )
		Controls.UnitStatRangedAttack:LocalizeAndSetToolTip( "TXT_KEY_UPANEL_RANGED_ATTACK_TT" )
	-- Religious unit
	elseif gk_mode and unit:GetSpreadsLeft() > 0 then
		local unitReligion = unit:GetReligion()
		local icon = (GameInfo.Religions[unitReligion] or {}).IconString
		Controls.UnitStatRangedAttack:SetText( icon and (unit:GetSpreadsLeft()..icon) )
		Controls.UnitStatRangedAttack:SetToolTipString( L(Game.GetReligionName(unitReligion))..": "..L"TXT_KEY_UPANEL_SPREAD_RELIGION_USES_TT" )
--	elseif gk_mode and GameInfoUnits[unit:GetUnitType()].RemoveHeresy then
--		Controls.UnitStatRangedAttack:LocalizeAndSetText( "TXT_KEY_UPANEL_REMOVE_HERESY_USES" )
--		Controls.UnitStatRangedAttack:LocalizeAndSetToolTip( "TXT_KEY_UPANEL_REMOVE_HERESY_USES_TT" )
	elseif bnw_mode and unit:CargoSpace() > 0 then
		Controls.UnitStatRangedAttack:SetText( L"TXT_KEY_UPANEL_CARGO_CAPACITY" .. " " .. unit:CargoSpace() )
		Controls.UnitStatRangedAttack:LocalizeAndSetToolTip( "TXT_KEY_UPANEL_CARGO_CAPACITY_TT", unit:GetName() )
	else
		Controls.UnitStatRangedAttack:SetText()
	end
	Controls.UnitStats:CalculateSize()
	Controls.UnitStats:ReprocessAnchoring()
end

--------------------------------------------------------------------------------
-- Refresh unit health bar
--------------------------------------------------------------------------------
local function UpdateUnitHealthBar(unit)
	-- note that this doesn't use the bar type
	local damage = unit:GetDamage()
	if damage == 0 then
		Controls.HealthBar:SetHide(true)
	else
		local healthPercent = 1.0 - (damage / g_MaxDamage)
		local healthTimes100 =  math.floor(100 * healthPercent + 0.5)
		local barSize = { x = 9, y = math.floor(123 * healthPercent) }
		if healthTimes100 <= 33 then
			Controls.RedBar:SetSize(barSize)
			Controls.RedAnim:SetSize(barSize)
			Controls.GreenBar:SetHide(true)
			Controls.YellowBar:SetHide(true)
			Controls.RedBar:SetHide(false)
		elseif healthTimes100 <= 66 then
			Controls.YellowBar:SetSize(barSize)
			Controls.GreenBar:SetHide(true)
			Controls.YellowBar:SetHide(false)
			Controls.RedBar:SetHide(true)
		else
			Controls.GreenBar:SetSize(barSize)
			Controls.GreenBar:SetHide(false)
			Controls.YellowBar:SetHide(true)
			Controls.RedBar:SetHide(true)
		end

		Controls.HealthBar:LocalizeAndSetToolTip( "TXT_KEY_UPANEL_SET_HITPOINTS_TT", g_MaxDamage-damage, g_MaxDamage )

		Controls.HealthBar:SetHide(false)
	end
end

--------------------------------------------------------------------------------
-- Event Handlers
--------------------------------------------------------------------------------

Controls.CycleLeft:RegisterCallback( Mouse.eLClick,
function()
	-- Cycle to next selection.
	Game.CycleUnits(true, true, false)
end)

Controls.CycleRight:RegisterCallback( Mouse.eLClick,
function()
	-- Cycle to previous selection.
	Game.CycleUnits(true, false, false)
end)

local function OnUnitNameClicked()
	-- go to this unit
	UI.LookAtSelectionPlot()
end
Controls.UnitNameButton:RegisterCallback( Mouse.eLClick, OnUnitNameClicked )

Controls.UnitPortraitButton:SetToolTipCallback(
function()
	UnitToolTip( UI.GetHeadSelectedUnit(), L("TXT_KEY_CURRENTLY_SELECTED_UNIT"), "----------------" )
end)

Controls.UnitPortraitButton:RegisterCallback( Mouse.eLClick, OnUnitNameClicked )

Controls.UnitPortraitButton:RegisterCallback( Mouse.eRClick,
function()
	local unit = UI.GetHeadSelectedUnit()
	Events.SearchForPediaEntry( unit and unit:GetNameKey() )
end)

--------------------------------------------------------------------------------
-- InfoPane is now dirty.
--------------------------------------------------------------------------------
local function sortUnitsByDistanceToSelectedUnit( instance1, instance2 )
	local unit1 = g_activePlayer:GetUnitByID( instance1:GetVoid1() )
	local unit2 = g_activePlayer:GetUnitByID( instance2:GetVoid1() )
	local unit = UI.GetHeadSelectedUnit()
	if unit and unit1 and unit2 then
		local x = unit:GetX()
		local y = unit:GetY()
		return Map.PlotDistance( x, y, unit1:GetX(), unit1:GetY() ) < Map.PlotDistance( x, y, unit2:GetX(), unit2:GetY() )
	end
end

local function OnInfoPaneDirty()

end
-------------------------------------------------
-- Unit rename
-------------------------------------------------
local function OnEditNameClick()

	if UI.GetHeadSelectedUnit() then
		Events.SerialEventGameMessagePopup{
				Type = ButtonPopupTypes.BUTTONPOPUP_RENAME_UNIT,
				Data1 = UI.GetHeadSelectedUnit():GetID(),
				Data2 = -1,
				Data3 = -1,
				Option1 = false,
				Option2 = false
			}
	end
end
Controls.EditButton:RegisterCallback( Mouse.eLClick, OnEditNameClick )
Controls.UnitNameButton:RegisterCallback( Mouse.eRClick, OnEditNameClick )

------------------------------------------------------
-- Action Tooltip Handler
------------------------------------------------------
local tipControlTable = {}
TTManager:GetTypeControlTable( "TypeUnitAction", tipControlTable )
function ActionToolTipHandler( control )

	local unit = UI.GetHeadSelectedUnit()
	if not unit then
		return
	end

	local actionID = control:GetVoid1()
	local action = GameInfoActions[actionID]
	local unitPlot = unit:GetPlot()

	-- Not able to perform action
	local gameCanHandleAction = not Game.CanHandleAction( actionID, unitPlot, false )

	-- Build data
	local isBuild = action.SubType == ActionSubTypes.ACTIONSUBTYPE_BUILD
	local buildID = action.MissionData
	local build = GameInfoBuilds[buildID]

	-- Improvement data
	local improvement = build and build.ImprovementType and build.ImprovementType ~= "NONE" and GameInfo.Improvements[build.ImprovementType]
	local improvementID = improvement and improvement.ID or -1

	-- Feature data
	local featureID = unitPlot:GetFeatureType()
	local feature = GameInfo.Features[featureID]

	-- Route data
	local route = build and build.RouteType and build.RouteType ~= "NONE" and GameInfo.Routes[build.RouteType]
	local routeID = route and route.ID or -1

	local strBuildTurnsString = ""
	local toolTip = table()
	local disabledTip = table()

	-- Upgrade has special help text
	if action.Type == "COMMAND_UPGRADE" then

		local upgradeUnitTypeID = unit:GetUpgradeUnitType()
		local unitUpgradePrice = unit:UpgradePrice(upgradeUnitTypeID)

		toolTip:insertLocalized( "TXT_KEY_UPGRADE_HELP", GameInfoUnits[upgradeUnitTypeID].Description, unitUpgradePrice )

		if gameCanHandleAction then

			-- Can't upgrade because we're outside our territory
			if unitPlot:GetOwner() ~= unit:GetOwner() then

				disabledTip:insertLocalized( "TXT_KEY_UPGRADE_HELP_DISABLED_TERRITORY" )
			end

			-- Can't upgrade because we're outside of a city
			if unit:GetDomainType() == DomainTypes.DOMAIN_AIR and not unitPlot:IsCity() then

				disabledTip:insertLocalized( "TXT_KEY_UPGRADE_HELP_DISABLED_CITY" )
			end

			-- Can't upgrade because we lack the Gold
			if unitUpgradePrice > g_activePlayer:GetGold() then

				disabledTip:insertLocalized( "TXT_KEY_UPGRADE_HELP_DISABLED_GOLD" )
			end

			-- Can't upgrade because we lack the Resources
			local resourcesNeeded = table()

			-- Loop through all resources to see how many we need. If it's > 0 then add to the string
			for pResource in GameInfo.Resources() do
				local iResourceLoop = pResource.ID

				local iNumResourceNeededToUpgrade = unit:GetNumResourceNeededToUpgrade(iResourceLoop)

				if iNumResourceNeededToUpgrade > 0 and iNumResourceNeededToUpgrade > g_activePlayer:GetNumResourceAvailable(iResourceLoop) then
					resourcesNeeded:insert( iNumResourceNeededToUpgrade .. " " .. pResource.IconString .. " " .. L(pResource.Description) )
				end
			end

			-- Build resources required string
			if #resourcesNeeded > 0 then

				disabledTip:insertLocalized( "TXT_KEY_UPGRADE_HELP_DISABLED_RESOURCES", resourcesNeeded:concat(", ") )
			end

				-- if we can't upgrade due to stacking
			if unitPlot:GetNumFriendlyUnitsOfType(unit) > 1 then

				disabledTip:insertLocalized( "TXT_KEY_UPGRADE_HELP_DISABLED_STACKING" )

			end
		end
	end

	if action.Type == "MISSION_ALERT" and not unit:IsEverFortifyable() then

		toolTip:insertLocalized( "TXT_KEY_MISSION_ALERT_NO_FORTIFY_HELP" )

	-- Golden Age has special help text
	elseif action.Type == "MISSION_GOLDEN_AGE" then

		toolTip:insertLocalized(  "TXT_KEY_MISSION_START_GOLDENAGE_HELP", unit:GetGoldenAgeTurns() )

	-- Spread Religion has special help text
	elseif gk_mode and action.Type == "MISSION_SPREAD_RELIGION" then

		local eMajorityReligion = unit:GetMajorityReligionAfterSpread()

		toolTip:insertLocalized( "TXT_KEY_MISSION_SPREAD_RELIGION_HELP" )
		toolTip:insert( "----------------" )
		toolTip:insert( L("TXT_KEY_MISSION_SPREAD_RELIGION_RESULT", Game.GetReligionName(unit:GetReligion()), unit:GetNumFollowersAfterSpread() ) .. " " )
		if eMajorityReligion < ReligionTypes.RELIGION_PANTHEON then
			toolTip:append( L("TXT_KEY_MISSION_MAJORITY_RELIGION_NONE") )
		else
			toolTip:append( L("TXT_KEY_MISSION_MAJORITY_RELIGION", Game.GetReligionName(eMajorityReligion) ) )
		end

	-- Create Great Work has special help text
	elseif bnw_mode and action.Type == "MISSION_CREATE_GREAT_WORK" then

		toolTip:insertLocalized( "TXT_KEY_MISSION_CREATE_GREAT_WORK_HELP" )
		toolTip:insert( "----------------" )

		if not gameCanHandleAction then
			local eGreatWorkSlotType = unit:GetGreatWorkSlotType()
			local iBuilding = g_activePlayer:GetBuildingOfClosestGreatWorkSlot(unit:GetX(), unit:GetY(), eGreatWorkSlotType)
			local pCity = g_activePlayer:GetCityOfClosestGreatWorkSlot(unit:GetX(), unit:GetY(), eGreatWorkSlotType)
			toolTip:insertLocalized( "TXT_KEY_MISSION_CREATE_GREAT_WORK_RESULT", GameInfo.Buildings[iBuilding].Description, pCity:GetNameKey() )
		end

	-- Paradrop
	elseif action.Type == "INTERFACEMODE_PARADROP" then
		toolTip:insertLocalized( "TXT_KEY_INTERFACEMODE_PARADROP_HELP_WITH_RANGE", unit:GetDropRange() )

	-- Sell Exotic Goods
	elseif bnw_mode and action.Type == "MISSION_SELL_EXOTIC_GOODS" then

		toolTip:insertLocalized( "TXT_KEY_MISSION_SELL_EXOTIC_GOODS_HELP" )

		if not gameCanHandleAction then
			toolTip:insert( "----------------" )
			toolTip:insert( "+" .. unit:GetExoticGoodsGoldAmount() .. "[ICON_GOLD]" )
			toolTip:insertLocalized( "TXT_KEY_EXPERIENCE_POPUP", unit:GetExoticGoodsXPAmount() )
		end

	-- Great Scientist
	elseif bnw_mode and action.Type == "MISSION_DISCOVER" then

		toolTip:insertLocalized( "TXT_KEY_MISSION_DISCOVER_TECH_HELP" )

		if not gameCanHandleAction then
			toolTip:insert( "----------------" )
			toolTip:insert( "+" .. unit:GetDiscoverAmount() .. "[ICON_RESEARCH]" )
		end

	-- Great Engineer
	elseif bnw_mode and action.Type == "MISSION_HURRY" then

		toolTip:insertLocalized( "TXT_KEY_MISSION_HURRY_PRODUCTION_HELP" )

		if not gameCanHandleAction then
			toolTip:insert( "----------------" )
			toolTip:insert( "+" .. unit:GetHurryProduction(unitPlot) .. "[ICON_PRODUCTION]" )
		end

	-- Great Merchant
	elseif bnw_mode and action.Type == "MISSION_TRADE" then

		toolTip:insertLocalized( "TXT_KEY_MISSION_CONDUCT_TRADE_MISSION_HELP" )

		if not gameCanHandleAction then
			toolTip:insert( "----------------" )
			toolTip:insert( "+" .. unit:GetTradeInfluence(unitPlot) .. "[ICON_INFLUENCE]" )
			toolTip:insert( "+" .. unit:GetTradeGold(unitPlot) .. "[ICON_GOLD]" )
		end

	-- Great Writer
	elseif bnw_mode and action.Type == "MISSION_GIVE_POLICIES" then

		toolTip:insertLocalized( "TXT_KEY_MISSION_GIVE_POLICIES_HELP" )

		if not gameCanHandleAction then
			toolTip:insert( "----------------" )
			toolTip:insert( "+" .. unit:GetGivePoliciesCulture() .. "[ICON_CULTURE]" )
		end

	-- Great Musician
	elseif bnw_mode and action.Type == "MISSION_ONE_SHOT_TOURISM" then

		toolTip:insertLocalized( "TXT_KEY_MISSION_ONE_SHOT_TOURISM_HELP" )

		if not gameCanHandleAction then
			toolTip:insert( "----------------" )
			toolTip:insert( "+" .. unit:GetBlastTourism() .. "[ICON_TOURISM]" )
		end

	-- Help text
	elseif action.Help and action.Help ~= "" then

		toolTip:insertLocalized(  action.Help )
	end

	-- Delete has special help text
	if action.Type == "COMMAND_DELETE" then

		toolTip:insertLocalized( "TXT_KEY_SCRAP_HELP", unit:GetScrapGold() )
	end

	-- Not able to perform action
	if gameCanHandleAction then

		-- Worker build
		if isBuild then

			-- Figure out what the name of the thing is that we're looking at
			local strImpRouteKey = ""
			if improvement then
				strImpRouteKey = improvement.Description
			elseif route then
				strImpRouteKey = route.Description
			end

			-- Don't have Tech for Build?
			if build.PrereqTech then
				local pPrereqTech = GameInfo.Technologies[build.PrereqTech]
				local iPrereqTech = pPrereqTech.ID
				if iPrereqTech ~= -1 and not g_activeTechs:HasTech(iPrereqTech) then

					-- Must not be a build which constructs something
					if improvement or route then

						disabledTip:insertLocalized( "TXT_KEY_BUILD_BLOCKED_PREREQ_TECH", pPrereqTech.Description, strImpRouteKey )
					end
				end
			end

			-- Trying to build something and are not adjacent to our territory?
			if gk_mode and improvement and improvement.InAdjacentFriendly then
				if unitPlot:GetTeam() ~= unit:GetTeam() then
					if not unitPlot:IsAdjacentTeam(unit:GetTeam(), true) then

						disabledTip:insertLocalized( "TXT_KEY_BUILD_BLOCKED_NOT_IN_ADJACENT_TERRITORY", strImpRouteKey )
					end
				end

			-- Trying to build something in a City-State's territory?
			elseif bnw_mode and improvement and improvement.OnlyCityStateTerritory then
				local bCityStateTerritory = false
				if unitPlot:IsOwned() then
					local unitPlotOwner = Players[unitPlot:GetOwner()]
					if unitPlotOwner and unitPlotOwner:IsMinorCiv() then
						bCityStateTerritory = true
					end
				end

				if not bCityStateTerritory then

					disabledTip:insertLocalized( "TXT_KEY_BUILD_BLOCKED_NOT_IN_CITY_STATE_TERRITORY", strImpRouteKey )
				end

			-- Trying to build something outside of our territory?
			elseif improvement and not improvement.OutsideBorders then
				if unitPlot:GetTeam() ~= unit:GetTeam() then

					disabledTip:insertLocalized( "TXT_KEY_BUILD_BLOCKED_OUTSIDE_TERRITORY", strImpRouteKey )
				end
			end

			-- Trying to build something that requires an adjacent luxury?
			if bnw_mode and improvement and improvement.AdjacentLuxury then
				local bAdjacentLuxury = false

				for loop, direction in ipairs( g_directionTypes ) do
					local adjacentPlot = Map.PlotDirection(unitPlot:GetX(), unitPlot:GetY(), direction)
					if adjacentPlot then
						local eResourceType = adjacentPlot:GetResourceType()
						if eResourceType ~= -1 then
							if Game.GetResourceUsageType(eResourceType) == ResourceUsageTypes.RESOURCEUSAGE_LUXURY then
								bAdjacentLuxury = true
							end
						end
					end
				end

				if not bAdjacentLuxury then

					disabledTip:insertLocalized( "TXT_KEY_BUILD_BLOCKED_NO_ADJACENT_LUXURY", strImpRouteKey )
				end
			end

			-- Trying to build something where we can't have two adjacent?
			if bnw_mode and improvement and improvement.NoTwoAdjacent then
				local bTwoAdjacent = false

				for loop, direction in ipairs( g_directionTypes ) do
					local adjacentPlot = Map.PlotDirection(unitPlot:GetX(), unitPlot:GetY(), direction)
					if adjacentPlot then
						local eImprovementType = adjacentPlot:GetImprovementType()
						if eImprovementType ~= -1 then
							if eImprovementType == improvementID then
								bTwoAdjacent = true
							end
						end
						local iBuildProgress = adjacentPlot:GetBuildProgress(buildID)
						if iBuildProgress > 0 then
							bTwoAdjacent = true
						end
					end
				end

				if bTwoAdjacent then

					disabledTip:insertLocalized( "TXT_KEY_BUILD_BLOCKED_CANNOT_BE_ADJACENT", strImpRouteKey )
				end
			end

			-- Build blocked by a feature here?
			if g_activePlayer:IsBuildBlockedByFeature(buildID, featureID) then

				for row in GameInfo.BuildFeatures{ BuildType = build.Type, FeatureType = feature.Type } do
					local pFeatureTech = GameInfo.Technologies[row.PrereqTech]
					disabledTip:insertLocalized( "TXT_KEY_BUILD_BLOCKED_BY_FEATURE", pFeatureTech.Description, feature.Description )
				end

			end

		-- Not a Worker build, use normal disabled help from XML
		else

			if action.Type == "MISSION_FOUND" and g_activePlayer:IsEmpireVeryUnhappy() then

				disabledTip :insertLocalized( "TXT_KEY_MISSION_BUILD_CITY_DISABLED_UNHAPPY" )

			elseif action.Type == "MISSION_CULTURE_BOMB" and g_activePlayer:GetCultureBombTimer() > 0 then

				disabledTip:insertLocalized( "TXT_KEY_MISSION_CULTURE_BOMB_DISABLED_COOLDOWN", g_activePlayer:GetCultureBombTimer() )

			elseif action.DisabledHelp and action.DisabledHelp ~= "" then

				disabledTip:insertLocalized( action.DisabledHelp )
			end
		end

		if #disabledTip > 0 then
			toolTip:insert( "[COLOR_WARNING_TEXT]" .. disabledTip:concat("[NEWLINE]") .. "[ENDCOLOR]" )
		end
	end

	-- Is this a Worker build?
	if isBuild then

		local extraBuildRate = 0

		-- Are we building anything right now?
		local currentBuildID = unit:GetBuildType()
		if currentBuildID == -1 or buildID ~= currentBuildID then
			extraBuildRate = unit:WorkRate( true, buildID )
		end

		local iBuildTurns = unitPlot:GetBuildTurnsLeft(buildID, g_activePlayerID, extraBuildRate, extraBuildRate) + 1
--print("iBuildTurns: " .. iBuildTurns)
		if iBuildTurns > 0 then
			strBuildTurnsString = " ... " .. L("TXT_KEY_STR_TURNS", iBuildTurns)
		end

		-- Extra Yield from this build

		for iYield = 0, YieldTypes.NUM_YIELD_TYPES-1 do
			local iYieldChange = unitPlot:GetYieldWithBuild( buildID, iYield, false, g_activePlayerID )
						- unitPlot:CalculateYield(iYield)

			if iYieldChange > 0 then
				toolTip:insert( "[COLOR_POSITIVE_TEXT]+" .. L( g_yieldString[iYield], iYieldChange) )
			elseif  iYieldChange < 0 then
				toolTip:insert( "[COLOR_NEGATIVE_TEXT]" .. L( g_yieldString[iYield], iYieldChange) )
			end
		end

		-- Resource connection
		if improvement then
			local iResourceID = unitPlot:GetResourceType(g_activeTeamID)
			if iResourceID ~= -1 then
				if unitPlot:IsResourceConnectedByImprovement(improvementID) then
					if Game.GetResourceUsageType(iResourceID) ~= ResourceUsageTypes.RESOURCEUSAGE_BONUS then
						local pResource = GameInfo.Resources[iResourceID]

						toolTip:insertLocalized( "TXT_KEY_BUILD_CONNECTS_RESOURCE", pResource.IconString, pResource.Description )

					end
				end
			end
		end

		-- Production for clearing a feature
		if feature then
			local bFeatureRemoved = unitPlot:IsBuildRemovesFeature(buildID)
			if bFeatureRemoved then

				toolTip:insertLocalized( "TXT_KEY_BUILD_FEATURE_CLEARED", feature.Description )

				local iFeatureProduction = unitPlot:GetFeatureProduction(buildID, g_activeTeamID)
				if iFeatureProduction > 0 then
					toolTip:append( L("TXT_KEY_BUILD_FEATURE_PRODUCTION", iFeatureProduction) )
				end
			end
		end
	end

	-- Tooltip
	tipControlTable.UnitActionHelp:SetText( toolTip:concat("[NEWLINE]") )

	-- Title
	local text = tostring( action.TextKey or action.Type )
	local strTitleString = "[COLOR_POSITIVE_TEXT]" .. L( text ) .. "[ENDCOLOR]".. strBuildTurnsString
	tipControlTable.UnitActionText:SetText( strTitleString )

	-- HotKey
	if action.SubType == ActionSubTypes.ACTIONSUBTYPE_PROMOTION then
		tipControlTable.UnitActionHotKey:SetText( "" )
	elseif action.HotKey and action.HotKey ~= "" then
		tipControlTable.UnitActionHotKey:SetText( "("..tostring(action.HotKey)..")" )
	else
		tipControlTable.UnitActionHotKey:SetText( "" )
	end

	-- Autosize tooltip
	tipControlTable.UnitActionMouseover:DoAutoSize()
	local mouseoverSizeX = tipControlTable.UnitActionMouseover:GetSizeX()
	if mouseoverSizeX < 350 then
		tipControlTable.UnitActionMouseover:SetSizeX( 350 )
	end

end

--[[----------------------------------------------------
 ___       _ _   _       _ _          _   _             
|_ _|_ __ (_) |_(_) __ _| (_)______ _| |_(_) ___  _ __  
 | || '_ \| | __| |/ _` | | |_  / _` | __| |/ _ \| '_ \ 
 | || | | | | |_| | (_| | | |/ / (_| | |_| | (_) | | | |
|___|_| |_|_|\__|_|\__,_|_|_/___\__,_|\__|_|\___/|_| |_|
]]------------------------------------------------------

local isUnit, isCity, isUpdateRequired

local function UpdateDisplayNow()
	if IsGameCoreBusy() then
		return
	end

	isUpdateRequired = false
	ContextPtr:ClearUpdate()

	if isUnit then
		isUnit = false
		g_units.Update()
		-- Retrieve the currently selected unit.
		local unit = UI.GetHeadSelectedUnit()
		local unitID = unit and unit:GetID() or -1

		-- Unit is different than last unit.
		if unitID ~= g_lastUnitID then
			local hexPosition = ToHexFromGrid{ x = unit and unit:GetX() or 0, y = unit and unit:GetY() or 0 }

			if g_lastUnitID ~= -1 then
				Events.UnitSelectionChanged( g_activePlayerID, g_lastUnitID, 0, 0, 0, false, false )
			end
			if unitID ~= -1 then
				Events.UnitSelectionChanged( g_activePlayerID, unitID, hexPosition.x, hexPosition.y, 0, true, false )
			end

			Controls.UnitStack:SortChildren( sortUnitsByDistanceToSelectedUnit )
			Controls.UnitScrollPanel:SetScrollValue(1)
		end
		g_lastUnitID = unitID

		g_bottomOffset = g_bottomOffset0
		local unitTypesPanelOffsetX = g_unitPortraitSize * 0.625
		local unitTypesPanelOffsetY = 120
		if unit then
			UpdateUnitActions(unit)
			UpdateUnitPortrait(unit)
			UpdateUnitPromotions(unit)
			UpdateUnitStats(unit)
			UpdateUnitHealthBar(unit)
			Controls.Panel:SetHide( false )
			Controls.Actions:SetHide( false )
		else
			-- Attempt to show currently selected city.
			local city = UI.GetHeadSelectedCity()
			if city then
				UpdateCityPortrait(city)
			else
				g_bottomOffset = 35
				unitTypesPanelOffsetX = 80
				unitTypesPanelOffsetY = -40
			end
			Controls.Panel:SetHide( not city )
			Controls.Actions:SetHide( not city )
		end
		Controls.ActionStack:SetHide( not unit )
		Controls.UnitPanel:SetOffsetY( g_bottomOffset )
		if not g_isHideUnitTypes then
			Controls.UnitTypesStack:CalculateSize()
			local x,y = Controls.UnitTypesStack:GetSizeVal()
			if y<64 then y=64 elseif y>320 then y=320 end
			Controls.UnitTypesPanel:SetOffsetVal( unitTypesPanelOffsetX, unitTypesPanelOffsetY )
			Controls.UnitTypesPanel:SetSizeVal( x+40, y+85 )
			Controls.UnitTypesScrollPanel:SetSizeVal( x, y )
			Controls.UnitTypesScrollPanel:CalculateInternalSize()
			Controls.UnitTypesScrollPanel:ReprocessAnchoring()
		end
	end
	if isCity then
		isCity = false
		g_cities.Update( isCity ~= true and isCity )
	end

	local maxTotalStackHeight = g_screenHeight - g_topOffset - g_bottomOffset
	local halfTotalStackHeight = math.floor(maxTotalStackHeight / 2)

	Controls.CityStack:CalculateSize()
	local cityStackHeight = Controls.CityStack:GetSizeY()
	Controls.UnitStack:CalculateSize()
	local unitStackHeight = Controls.UnitStack:GetSizeY()
	Controls.CityStack:CalculateSize()
	local cityStackHeight = Controls.CityStack:GetSizeY()
	Controls.UnitStack:CalculateSize()
	local unitStackHeight = Controls.UnitStack:GetSizeY()

	if unitStackHeight + cityStackHeight <= maxTotalStackHeight then
		unitStackHeight = false
		cityStackHeight = false
	elseif cityStackHeight <= halfTotalStackHeight then
		unitStackHeight = maxTotalStackHeight - cityStackHeight
		cityStackHeight = false
	elseif unitStackHeight <= halfTotalStackHeight then
		unitStackHeight = false
		cityStackHeight = maxTotalStackHeight - unitStackHeight
	else
		cityStackHeight = halfTotalStackHeight
		unitStackHeight = halfTotalStackHeight
	end

	Controls.CityScrollPanel:SetHide( not cityStackHeight )
	if cityStackHeight then
		Controls.CityStack:ChangeParent( Controls.CityScrollPanel )
		Controls.CityScrollPanel:SetSizeY( cityStackHeight )
		Controls.CityScrollPanel:CalculateInternalSize()
--			if resetCityPanelElevator then
--				Controls.CityScrollPanel:SetScrollValue( 0 )
--			end
	else
		Controls.CityStack:ChangeParent( Controls.CityPanel )
	end
	Controls.CityPanel:ReprocessAnchoring()

	Controls.UnitScrollPanel:SetHide( not unitStackHeight )
	if unitStackHeight then
		Controls.UnitStack:ChangeParent( Controls.UnitScrollPanel )
		Controls.UnitScrollPanel:SetSizeY( unitStackHeight )
		Controls.UnitScrollPanel:CalculateInternalSize()
--			if resetUnitPanelElevator then
--				Controls.UnitScrollPanel:SetScrollValue( 0 )
--			end
	else
		Controls.UnitStack:ChangeParent( Controls.UnitPanel )
	end
	return Controls.UnitPanel:ReprocessAnchoring()
end

local function UpdateDisplay()
	if not isUpdateRequired then
		isUpdateRequired = true
		return ContextPtr:SetUpdate( UpdateDisplayNow )
	end
end

local function UpdateCities()
	isCity = true
	return UpdateDisplay()
end

local function UpdateUnits()
	isUnit = true
	return UpdateDisplay()
end

local function UpdateSpecificCity( playerID, cityID )
	if playerID == g_activePlayerID then
		if isCity == false then
			isCity = cityID
			return UpdateDisplay()
		elseif isCity ~= cityID then
			return UpdateCities()
		end
	end
end

local function SelectUnitType( isChecked, unitTypeID ) -- Void2, control )
	g_ActivePlayerUnitsInRibbon[ unitTypeID ] = isChecked
	g_units.Initialize( g_isHideUnitList )
	UpdateUnits()
	-- only save player 0 preferences, not other hotseat player's
	if g_activePlayerID == 0 then
		EUI_options.SetValue( "RIBBON_"..GameInfoUnits[ unitTypeID ].Type, isChecked and 1 or 0 )
	end
end

local function addUnitType( unit )
	local unitTypeID = unit:GetUnitType()
	g_ActivePlayerUnitTypes[ unit:GetID() ] = unitTypeID
	local instance = g_unitTypes.Get( unitTypeID )
	if instance then
		local n = instance[3] + 1
		instance[3] = n
--print( "Add unit:", unit:GetID(), unit:GetName(), "type:", instance, unitTypeID, "count:", n )
		return instance.Count:SetText( n )
	else
--print( "Add unit:", unit:GetID(), unit:GetName(), "new type:", unitTypeID )
		return g_unitTypes.Create( unitTypeID, unit )
	end
end
-------------------------------------------------
-- Unit Options "Object"
-------------------------------------------------
g_unitTypes = g_RibbonManager( "UnitTypeInstance", Controls.UnitTypesStack, Controls.Scrap, 
	function( Create ) -- CREATE ALL
		g_ActivePlayerUnitTypes = {}
		for unit in g_activePlayer:Units() do
			addUnitType( unit )
		end
	end,
	function( instance, unit ) -- INIT
		local portrait, portraitOffset, portraitAtlas = instance.Portrait, UI.GetUnitPortraitIcon( unit )
		portrait:SetHide(not ( portraitOffset and portraitAtlas and IconHookup( portraitOffset, portrait:GetSizeX(), portraitAtlas, portrait ) ) )
		instance.CheckBox:RegisterCheckHandler( SelectUnitType )
		local unitTypeID = unit:GetUnitType()
		instance.CheckBox:SetCheck( g_ActivePlayerUnitsInRibbon[ unitTypeID ] )
		instance.CheckBox:SetVoid1( unitTypeID )
		instance.Count:SetText("1")
		instance[3] = 1
	end,
	function( instance ) -- UPDATE
		instance.Count:SetText( instance[3] )
	end,
-------------------------------------------------------------------------------------------------
{-- the callback function table names need to match associated instance control ID defined in xml
	Button = {
		[Mouse.eRClick] = function( unitTypeID )
			local unit = GameInfo.Units[ unitTypeID ]
			if unit then
				Events.SearchForPediaEntry( unit.Description )
			end
		end,
	},--/Button
},--/unit options callbacks
-------------------------------------------------------------------------------------------------
{-- the tooltip function names need to match associated instance control ID defined in xml
	Button = function( control )
		local itemID = control:GetVoid1()
		local portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon( itemID, g_activePlayerID )
		local item = GameInfo.Units[itemID]
		item = item and IconHookup( portraitOffset, g_tipControls.Portrait:GetSizeY(), portraitAtlas, g_tipControls.Portrait )
		local instance = g_unitTypes.Get( itemID )
		g_tipControls.Text:SetText( (instance and (instance[3].." ") or "")..GetHelpTextForUnit( itemID, true ) )
		g_tipControls.PortraitFrame:SetHide( not item )
		g_tipControls.Box:DoAutoSize()
	end,
}--/units options tooltips
)--/unit options object

local function CreateUnit( playerID, unitID ) --hexVec, unitType, cultureType, civID, primaryColor, secondaryColor, unitFlagIndex, fogState, selected, military, notInvisible )
--						table: 58C77688	83	0	30	table: 58C776B0	table: 58C776D8	0	2	false	false	true
	if playerID == g_activePlayerID then
		local unit = g_activePlayer:GetUnitByID( unitID )
--print("Create unit", unitID, unit and unit:GetName() )
		if unit then
			addUnitType( unit )
			if FilterUnit( unit ) then
				g_units.Create( unitID, unit )
				return UpdateDisplay()
			end
		end
	end
end

local function CreateCity( hexPos, playerID, cityID ) --, cultureType, eraType, continent, populationSize, size, fowState )
	if playerID == g_activePlayerID then
		g_cities.Create( cityID, g_activePlayer:GetCityByID( cityID ) )
		return UpdateDisplay()
	end
end

local function DestroyUnit( playerID, unitID )
	if playerID == g_activePlayerID then
		g_units.Destroy( unitID )
		local unitTypeID = g_ActivePlayerUnitTypes[ unitID ]
		local instance = g_unitTypes.Get( g_ActivePlayerUnitTypes[ unitID ] )
--print( "Destroy unit", unitID, "type:", g_ActivePlayerUnitTypes[ unitID ], instance, "previous count:", instance and instance[3] )
		g_ActivePlayerUnitTypes[ unitID ] = nil
		if instance then
			local n = instance[3] - 1
			if n == 0 then
				g_unitTypes.Destroy( unitTypeID )
			else
				instance[3] = n
				instance.Count:SetText( n )
			end
		end
		return UpdateDisplay()
	end
end

local function DestroyCity( hexPos, playerID, cityID )
	if playerID == g_activePlayerID then
		g_cities.Destroy( cityID )
		return UpdateDisplay()
	end
end

local eventAction = { [true] = "Remove", [false] = "Add" }
local function UpdateOptions()

	local flag = EUI_options.GetValue( "HideUnitRibbon" ) == 1
	if g_isHideUnitList ~= flag then
		g_isHideUnitList = flag
		Events.SerialEventUnitCreated[eventAction[flag]]( CreateUnit )
		Events.SerialEventUnitDestroyed[eventAction[flag]]( DestroyUnit )
	end
	isUnit = true
	g_units.Initialize( g_isHideUnitList )
	Controls.UnitPanel:SetHide( g_isHideUnitList )

	flag = EUI_options.GetValue( "HideCityRibbon" ) == 1
	if g_isHideCityList ~= flag then
		g_isHideCityList = flag
		Events.SerialEventCityCreated[eventAction[flag]]( CreateCity )
		Events.SerialEventCityDestroyed[eventAction[flag]]( DestroyCity )
		Events.SerialEventCityCaptured[eventAction[flag]]( DestroyCity )
		Events.SerialEventCityInfoDirty[eventAction[flag]]( UpdateCities )
		Events.SerialEventCitySetDamage[eventAction[flag]]( UpdateSpecificCity )
		Events.SpecificCityInfoDirty[eventAction[flag]]( UpdateSpecificCity )
	end
	isCity = not flag
	g_cities.Initialize( g_isHideCityList )
	Controls.CityPanel:SetHide( g_isHideCityList )

	flag = EUI_options.GetValue( "HideUnitTypes" ) == 1
	if g_isHideUnitTypes ~= flag then
		g_unitTypes.Initialize( flag )
		g_isHideUnitTypes = flag
		Controls.UnitTypesOpen:SetHide( not flag )
		Controls.UnitTypesClose:SetHide( flag )
	end
	Controls.UnitTypesPanel:SetHide( g_isHideUnitList or g_isHideUnitTypes )
	return UpdateDisplay()
end

Controls.UnitTypesButton:RegisterCallback( Mouse.eLClick,
function()
	EUI_options.SetValue( "HideUnitTypes", g_isHideUnitTypes and 0 or 1 )
	return UpdateOptions()
end)

Events.SerialEventEnterCityScreen.Add(
function()
	if g_lastUnitID ~= -1 then
		Events.UnitSelectionChanged( g_activePlayerID, g_lastUnitID, 0, 0, 0, false, false )
		g_lastUnitID = -1
		return UpdateUnits()
	end
end)

local function SetActivePlayer()-- activePlayerID, prevActivePlayerID )
	-- initialize globals
	if g_activePlayerID then
		g_AllPlayerOptions.UnitTypes[ g_activePlayerID ] = g_ActivePlayerUnitTypes
		g_AllPlayerOptions.UnitsInRibbon[ g_activePlayerID ] = g_ActivePlayerUnitsInRibbon
	end
	g_activePlayerID = Game.GetActivePlayer()
	g_activePlayer = Players[ g_activePlayerID ]
	g_activeTeamID = g_activePlayer:GetTeam()
	g_activeTeam = Teams[ g_activeTeamID ]
	g_activeTechs = g_activeTeam:GetTeamTechs()
	g_ActivePlayerUnitTypes = g_AllPlayerOptions.UnitTypes[ g_activePlayerID ] or {}
	g_ActivePlayerUnitsInRibbon = g_AllPlayerOptions.UnitsInRibbon[ g_activePlayerID ]
	if not g_ActivePlayerUnitsInRibbon then
		g_ActivePlayerUnitsInRibbon = {}
		for row in GameInfo.Units() do
			g_ActivePlayerUnitsInRibbon[ row.ID ] = EUI_options.GetValue( "RIBBON_"..row.Type ) ~= 0
		end
	end
	return UpdateOptions()
end

SetActivePlayer()
Events.GameplaySetActivePlayer.Add( SetActivePlayer )
Events.GameOptionsChanged.Add( UpdateOptions )
Events.SerialEventUnitInfoDirty.Add( UpdateUnits )
local g_infoCornerYmax = {
[InfoCornerID.None or -1] = g_topOffset0,
[InfoCornerID.Tech or -1] = OptionsManager.GetSmallUIAssets() and 150 or 225,
}
Events.OpenInfoCorner.Add( function( infoCornerID )
	g_topOffset = g_infoCornerYmax[infoCornerID] or 380
	Controls.CityPanel:SetOffsetY( g_topOffset )
	return UpdateOptions()
end)

--[[
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
			defenderMaxHitPoints )
	if attackerPlayerID == g_activePlayerID then
		local instance = g_units.Get( attackerUnitID )
		if instance then
			local toolTip = instance.Button:GetToolTipString()
			if toolTip then
				toolTip = toolTip .. "[NEWLINE]"
			else
				toolTip = ""
			end
			toolTip = toolTip
				.."Attack: "
				.. " / " .. tostring( attackerPlayerID )
				.. " / " .. tostring( attackerUnitID )
				.. " / " .. tostring( attackerUnitDamage )
				.. " / " .. tostring( attackerFinalUnitDamage )
				.. " / " .. tostring( attackerMaxHitPoints )
				.. " / " .. tostring( defenderPlayerID )
				.. " / " .. tostring( defenderUnitID )
				.. " / " .. tostring( defenderUnitDamage )
				.. " / " .. tostring( defenderFinalUnitDamage )
				.. " / " .. tostring( defenderMaxHitPoints )
			instance.Button:SetToolTipString( toolTip )
		end
	elseif defenderPlayerID == g_activePlayerID then
		local instance = g_units.Get( defenderUnitID )
		if instance then
			local toolTip = instance.Button:GetToolTipString()
			if toolTip then
				toolTip = toolTip .. "[NEWLINE]"
			else
				toolTip = ""
			end
			toolTip = toolTip
				.."Defense: "
				.. " / " .. tostring( attackerPlayerID )
				.. " / " .. tostring( attackerUnitID )
				.. " / " .. tostring( attackerUnitDamage )
				.. " / " .. tostring( attackerFinalUnitDamage )
				.. " / " .. tostring( attackerMaxHitPoints )
				.. " / " .. tostring( defenderPlayerID )
				.. " / " .. tostring( defenderUnitID )
				.. " / " .. tostring( defenderUnitDamage )
				.. " / " .. tostring( defenderFinalUnitDamage )
				.. " / " .. tostring( defenderMaxHitPoints )
			instance.Button:SetToolTipString( toolTip )
		end
	end
end)
--]]
-- Process request to hide enemy panel
LuaEvents.EnemyPanelHide.Add(
	function( isEnemyPanelHide )
		if g_isWorkerActionPanelOpen then
			Controls.WorkerActionPanel:SetHide( not isEnemyPanelHide )
		end
		if not g_isHideUnitTypes and not g_isHideUnitList then
			Controls.UnitTypesPanel:SetHide( not isEnemyPanelHide )
		end
	end)
local EnemyUnitPanel = LookUpControl( "/InGame/WorldView/EnemyUnitPanel" )
local isHidden = ContextPtr:IsHidden()
ContextPtr:SetShowHideHandler(
	function( isHide, isInit )
		if not isInit and isHidden ~= isHide then
			isHidden = isHide
			if isHide and EnemyUnitPanel then
				EnemyUnitPanel:SetHide( true )
			end
		end
	end)
ContextPtr:SetHide( false )

print("Finished loading EUI unit panel",os.clock())
end)
