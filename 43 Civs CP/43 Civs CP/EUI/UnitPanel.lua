------------------------------------------------------
-- Unit Panel Screen
-- modified by bc1 from Civ V 1.0.3.276 code
-- code is common using gk_mode and bnw_mode switches
------------------------------------------------------

include( "EUI_tooltips" )
Events.SequenceGameInitComplete.Add(function()
print("Loading EUI unit panel",ContextPtr,os.clock(),[[ 
 _   _       _ _   ____                  _ 
| | | |_ __ (_) |_|  _ \ __ _ _ __   ___| |
| | | | '_ \| | __| |_) / _` | '_ \ / _ \ |
| |_| | | | | | |_|  __/ (_| | | | |  __/ |
 \___/|_| |_|_|\__|_|   \__,_|_| |_|\___|_|
]])

local gk_mode = Game.GetReligionName ~= nil
local bnw_mode = Game.GetActiveLeague ~= nil


-------------------------------------------------
-- Minor lua optimizations
-------------------------------------------------
local ipairs = ipairs
local math_ceil = math.ceil
local math_floor = math.floor
local math_max = math.max
local pairs = pairs
local print = print
local table_concat = table.concat
local table_insert = table.insert
local table_remove = table.remove
local tostring = tostring

--EUI_utilities
local StackInstanceManager = StackInstanceManager
local IconLookup = EUI.IconLookup
local IconHookup = EUI.IconHookup
local Color = EUI.Color
local PrimaryColors = EUI.PrimaryColors
local BackgroundColors = EUI.BackgroundColors
local GameInfo = EUI.GameInfoCache -- warning! use iterator ONLY with table field conditions, NOT string SQL query
local table = EUI.table

--EUI_tooltips
local GetHelpTextForUnit = EUI.GetHelpTextForUnit
local GetHelpTextForBuilding = EUI.GetHelpTextForBuilding
local GetHelpTextForProject = EUI.GetHelpTextForProject
local GetHelpTextForProcess = EUI.GetHelpTextForProcess
local GetFoodTooltip = EUI.GetFoodTooltip
local GetProductionTooltip = EUI.GetProductionTooltip
local GetCultureTooltip = EUI.GetCultureTooltip

local ActionSubTypes = ActionSubTypes
local ActivityTypes = ActivityTypes
local ContextPtr = ContextPtr
local Controls = Controls
local DomainTypes = DomainTypes
local Events = Events
local Game = Game
local GameDefines = GameDefines
local GameInfoActions = GameInfoActions
local GameInfoTypes = GameInfoTypes
local GameInfo_Automates = GameInfo.Automates
local GameInfo_Builds = GameInfo.Builds
local GameInfo_Missions = GameInfo.Missions
local GameInfo_Units = GameInfo.Units
local L = Locale.ConvertTextKey
local Locale_ToUpper = Locale.ToUpper
local Map_PlotDirection = Map.PlotDirection
local Map_PlotDistance = Map.PlotDistance
local Mouse = Mouse
local OrderTypes = OrderTypes
local Players = Players
local PreGame = PreGame
local ReligionTypes = ReligionTypes
local ResourceUsageTypes = ResourceUsageTypes
local Teams = Teams
local ToHexFromGrid = ToHexFromGrid
local UI = UI
local UI_GetHeadSelectedUnit = UI.GetHeadSelectedUnit
local UI_GetUnitPortraitIcon = UI.GetUnitPortraitIcon
local YieldTypes = YieldTypes

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
[ActivityTypes.ACTIVITY_HOLD] = GameInfo_Missions.MISSION_SKIP,
[ActivityTypes.ACTIVITY_SLEEP] = GameInfo_Missions.MISSION_SLEEP,
[ActivityTypes.ACTIVITY_HEAL] = GameInfo_Missions.MISSION_HEAL,
[ActivityTypes.ACTIVITY_SENTRY] = GameInfo_Missions.MISSION_ALERT,
[ActivityTypes.ACTIVITY_INTERCEPT] = GameInfo_Missions.MISSION_AIRPATROL,
[ActivityTypes.ACTIVITY_MISSION] = GameInfo_Missions.MISSION_MOVE_TO,
}
local g_MaxDamage = GameDefines.MAX_HIT_POINTS or 100
local g_rebaseRangeMultipler = GameDefines.AIR_UNIT_REBASE_RANGE_MULTIPLIER
local g_pressureMultiplier = GameDefines.RELIGION_MISSIONARY_PRESSURE_MULTIPLIER or 1
local g_moveDenominator = GameDefines.MOVE_DENOMINATOR

local g_units, g_cities, g_unitTypes

local EUI_options = Modding.OpenUserData( "Enhanced User Interface Options", 1 )

local g_cityFocusIcons = {
--[CityAIFocusTypes.NO_CITY_AI_FOCUS_TYPE or -1] = "",
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_FOOD or -1] = "[ICON_FOOD]",
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_PRODUCTION or -1] = "[ICON_PRODUCTION]",
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_GOLD or -1] = "[ICON_GOLD]",
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_SCIENCE or -1] = "[ICON_RESEARCH]",
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_CULTURE or -1] = "[ICON_CULTURE]",
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_GREAT_PEOPLE or -1] = "[ICON_GREAT_PEOPLE]",
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_FAITH or -1] = "[ICON_PEACE]",
} g_cityFocusIcons[-1] = nil

local g_cityFocusTooltips = {
[CityAIFocusTypes.NO_CITY_AI_FOCUS_TYPE or -1] = L"TXT_KEY_CITYVIEW_FOCUS_BALANCED_TEXT",
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_FOOD or -1] = L"TXT_KEY_CITYVIEW_FOCUS_FOOD_TEXT",
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_PRODUCTION or -1] = L"TXT_KEY_CITYVIEW_FOCUS_PROD_TEXT",
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_GOLD or -1] = L"TXT_KEY_CITYVIEW_FOCUS_GOLD_TEXT",
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_SCIENCE or -1] = L"TXT_KEY_CITYVIEW_FOCUS_RESEARCH_TEXT",
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_CULTURE or -1] = L"TXT_KEY_CITYVIEW_FOCUS_CULTURE_TEXT",
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_GREAT_PEOPLE or -1] = L"TXT_KEY_CITYVIEW_FOCUS_GREAT_PERSON_TEXT",
[CityAIFocusTypes.CITY_AI_FOCUS_TYPE_FAITH or -1] = L"TXT_KEY_CITYVIEW_FOCUS_FAITH_TEXT",
} g_cityFocusTooltips[-1] = nil

-------------------------------------------------
-- Tooltip Utilities
-------------------------------------------------

local function ShowSimpleTip( ... )
	local toolTip = ... and table_concat( {...}, "[NEWLINE]----------------[NEWLINE]" )
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

local function g_RibbonManager( name, stack, scrap, createAllItems, initItem, updateItem, callbacks, tooltips )
	local self
	local m=0
	local n=0
	local index = {}

	local function Create( itemID, item )
		if item then
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
			portraitOffset, portraitAtlas = UI_GetUnitPortraitIcon( itemID, g_activePlayerID )
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
				buildPercent = 1 - math_max( 0, storedProduction/productionNeeded )
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
			instance.CityGrowth:SetString( " [COLOR_RED]" .. (math_floor( city:GetFoodTimes100() / -foodPerTurnTimes100 ) + 1) .. "[ENDCOLOR] " )
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

		instance.CityIsCapital:SetHide( not isCapital )
		instance.CityIsPuppet:SetHide( isNotPuppet )
		instance.CityFocus:SetText( isNotRazing and isNotResistance and isNotPuppet and g_cityFocusIcons[city:GetFocusType()] )
		instance.CityQuests:SetText( city:GetWeLoveTheKingDayCounter() > 0 and "[ICON_HAPPINESS_1]" or (GameInfo.Resources[city:GetResourceDemanded()] or {}).IconString )
		instance.CityIsRazing:SetHide( isNotRazing )
		instance.CityIsResistance:SetHide( isNotResistance )
		instance.CityIsConnected:SetHide( not g_activePlayer:IsCapitalConnectedToCity( city ) or isCapital )
		instance.CityIsBlockaded:SetHide( not city:IsBlockaded() )
		instance.CityIsOccupied:SetHide( not city:IsOccupied() or city:IsNoOccupiedUnhappiness() )
		instance.Name:SetString( city:GetName() )

		local culturePerTurn = city:GetJONSCulturePerTurn()
		instance.BorderGrowth:SetString( culturePerTurn > 0 and math_ceil( (city:GetJONSCultureThreshold() - city:GetJONSCultureStored()) / culturePerTurn ) )

		local percent = 1 - city:GetDamage() / ( gk_mode and city:GetMaxHitPoints() or GameDefines.MAX_CITY_HIT_POINTS )
		instance.Button:SetColor( Color( 1, percent, percent, 1 ) )
	end
end

local function FindCity( control )
	local instance = g_cities.FindInstance( control )
	return instance and g_activePlayer:GetCityByID( instance[1] )
end

local function getUnitBuildProgressData( plot, buildID, unit )
	local buildProgress = plot:GetBuildProgress( buildID )
	local nominalWorkRate = unit:WorkRate( true )
	-- take into account unit.cpp "wipe out all build progress also" game bug
	local buildTime = plot:GetBuildTime( buildID, g_activePlayerID ) - nominalWorkRate
	local buildTurnsLeft
	if buildProgress == 0 then
		buildTurnsLeft = plot:GetBuildTurnsLeft( buildID, g_activePlayerID, nominalWorkRate - unit:WorkRate() )
	else
		buildProgress = buildProgress - nominalWorkRate
		buildTurnsLeft = plot:GetBuildTurnsLeft( buildID, g_activePlayerID, -unit:WorkRate() )
	end
	if buildTurnsLeft > 99999 then
		return math_ceil( ( buildTime - buildProgress ) / nominalWorkRate ), buildProgress, buildTime
	else
		return buildTurnsLeft, buildProgress, buildTime
	end
end

local function updateSmallUnitIcons( unit, instance )

	g_MaxDamage = unit:GetMaxHitPoints();
	local movesLeft = unit:MovesLeft()
	if movesLeft >= unit:MaxMoves() then
		instance.MovementPip:SetTextureOffsetVal( 0, 0 )-- cyan (green)
	elseif movesLeft > 0 then
		if unit:IsCombatUnit() and unit:IsOutOfAttacks() then
			instance.MovementPip:SetTextureOffsetVal( 0, 96 )-- orange (gray)
		else
			instance.MovementPip:SetTextureOffsetVal( 0, 32 )-- green (yellow)
		end
	else
		instance.MovementPip:SetTextureOffsetVal( 0, 64 )-- red
	end
	local damage = unit:GetDamage()
	local percent = 1
	if damage > 0 then
		if instance == Controls then
			percent = 1 - damage / g_MaxDamage / 3
		else
			percent = 1 - damage / g_MaxDamage
		end
--			instance.HealthMeter:SetHide(false)
--			instance.HealthMeter:SetPercent( percent )
--		else
--			instance.HealthMeter:SetHide(true)
	end
	instance.Button:SetColor( Color( 1, percent, percent, 1 ) )

	local info
	local text = ""
	local buildID = unit:GetBuildType()
	if buildID ~= -1 then -- unit is actively building something
		info = GameInfo_Builds[buildID]
		text = getUnitBuildProgressData( unit:GetPlot(), buildID, unit )
		if text > 99 then text = "" end

	elseif unit:IsEmbarked() then
		info = GameInfo_Missions.MISSION_EMBARK

	elseif unit:IsAutomated() then
		if unit:IsWork() then
			info = GameInfo_Automates.AUTOMATE_BUILD
		elseif bnw_mode and unit:IsTrade() then
			info = GameInfo_Missions.MISSION_ESTABLISH_TRADE_ROUTE
		else
			info = GameInfo_Automates.AUTOMATE_EXPLORE
		end
	else
		local activityType = unit:GetActivityType()

		if activityType == ActivityTypes.ACTIVITY_SLEEP and unit:IsEverFortifyable() then
			info = GameInfo_Missions.MISSION_FORTIFY
		else
			info = g_activityMissions[ activityType ]
		end
	end
	instance.Mission:SetHide(not( info and IconHookup( info.IconIndex, 45, info.IconAtlas, instance.Mission ) ) )
	instance.MissionText:SetText( text )
end

local function UpdateUnit( instance )
	local unitID = instance[1]
	local unit = g_activePlayer:GetUnitByID( unitID )
	local isSelectedUnit = unitID == UI.GetSelectedUnitID()
	instance.Button:SetHide( isSelectedUnit )
	if unit and not isSelectedUnit then
		updateSmallUnitIcons( unit, instance )
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
		g_MaxDamage = unit:GetMaxHitPoints();

		local unitTypeID = unit:GetUnitType()
		local unitInfo = unitTypeID and GameInfo_Units[unitTypeID]

		-- Unit XML stats
--		local productionCost = g_activePlayer:GetUnitProductionNeeded( unitTypeID )
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
		city = (city and city:GetOwner() ~= g_activePlayerID and city) or g_activePlayer:GetCapitalCity() or g_activePlayer:Cities()(g_activePlayer)

		-- Name
		local combatClass = unitInfo.CombatClass and GameInfo.UnitCombatInfos[unitInfo.CombatClass]
		local tips = table( UnitColor( Locale_ToUpper( unit:GetNameKey() ) ) .. (combatClass and " ("..L(combatClass.Description)..")" or ""), "----------------", ... )

		local cityName = unit:GetCityName();
		if(cityName ~= "") then
			tips:insertLocalized( "TXT_KEY_UNIT_ORIGIN_CITY", cityName )
		end

		-- Required Resources:
		local resources = table()
		for resource in GameInfo.Resources() do
			local numResourceRequired = Game.GetNumResourceRequiredForUnit( unitTypeID, resource.ID )
-- TODO				local numResourceAvailable = g_activePlayer:GetNumResourceAvailable(resource.ID, true)
			resources:insertIf( numResourceRequired > 0 and numResourceRequired .. resource.IconString ) -- .. L(resource.Description) )
		end
		tips:insertIf( #resources > 0 and L"TXT_KEY_PEDIA_REQ_RESRC_LABEL" .. " " .. resources:concat(", ") )

		-- Required Total Resources:
		if Game.IsCustomModOption("UNITS_RESOURCE_QUANTITY_TOTALS") then
			resources = table()
			for resource in GameInfo.Resources() do
				local numResourceRequired = Game.GetNumResourceTotalRequiredForUnit( unitTypeID, resource.ID )
-- TODO				local numResourceAvailable = g_activePlayer:GetNumResourceAvailable(resource.ID, true)
				resources:insertIf( numResourceRequired > 0 and numResourceRequired .. resource.IconString ) -- .. L(resource.Description) )
			end
			tips:insertIf( #resources > 0 and L"TXT_KEY_EUI_RESOURCES_QUANTITY_TOTAL_REQUIRED_UNIT_PANEL" .. ": " .. resources:concat(", ") )
		end

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
		local influence = unit:GetTradeInfluence(unit:GetPlot());
		tips:insertIf( influence > 0 and "[ICON_BULLET]" .. L"TXT_KEY_MISSION_CONDUCT_TRADE_MISSION" .. (" %g[ICON_GOLD]%+g[ICON_INFLUENCE]" ):format( unitInfo.BaseGold + ( unitInfo.NumGoldPerEra or 0 ) * ( Game and Teams[Game.GetActiveTeam()]:GetCurrentEra() or PreGame.GetEra() ), influence) )

		-- Becomes Obsolete with:
		local obsoleteTech = unitInfo.ObsoleteTech and GameInfo.Technologies[unitInfo.ObsoleteTech]
		tips:insertIf( obsoleteTech and L"TXT_KEY_PEDIA_OBSOLETE_TECH_LABEL" .. " " .. TechColor( L(obsoleteTech.Description) ) )

		-- Upgrade unit
		local upgradeUnitTypeID = unit:GetUpgradeUnitType()
		local unitUpgradeInfo = upgradeUnitTypeID and GameInfo.Units[upgradeUnitTypeID]
		tips:insertIf( unitUpgradeInfo and L"TXT_KEY_COMMAND_UPGRADE" .. ": " .. UnitColor( L(unitUpgradeInfo.Description) ) .. " ("..unit:UpgradePrice(upgradeUnitTypeID).."[ICON_GOLD])" )

		g_tipControls.Text:SetText( tips:concat( "[NEWLINE]" ) )
		local portraitOffset, portraitAtlas = UI_GetUnitPortraitIcon( unit )
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
		local portrait, portraitOffset, portraitAtlas = instance.Portrait, UI_GetUnitPortraitIcon( unit )
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
				status = L(GameInfo_Builds[buildID].Description).. " (".. getUnitBuildProgressData( unit:GetPlot(), buildID, unit ) ..")"

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
		local itemInfo, strToolTip, portraitOffset, portraitAtlas
		if city then
			local orderID, itemID = city:GetOrderFromQueue()
			if orderID == OrderTypes.ORDER_TRAIN then
				itemInfo = GameInfo.Units
				portraitOffset, portraitAtlas = UI_GetUnitPortraitIcon( itemID, g_activePlayerID )
				strToolTip = GetHelpTextForUnit( itemID, true )

			elseif orderID == OrderTypes.ORDER_CONSTRUCT then
				itemInfo = GameInfo.Buildings
				strToolTip = GetHelpTextForBuilding( itemID, false, false, city:GetNumFreeBuilding(itemID) > 0, city )

			elseif orderID == OrderTypes.ORDER_CREATE then
				itemInfo = GameInfo.Projects
				strToolTip = GetHelpTextForProject( itemID, city, true )
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
		ShowSimpleCityTip( control, city, L("TXT_KEY_CITYVIEW_TURNS_TILL_TILE_TEXT", math_ceil( (city:GetJONSCultureThreshold() - city:GetJONSCultureStored()) / city:GetJONSCulturePerTurn() ) ), GetCultureTooltip( city ) )
	end,
	CityIsCapital = function( control )
		local city = FindCity( control )
		ShowSimpleCityTip( control, city, "[ICON_CAPITAL]" )
	end,
	CityIsPuppet = function( control )
		local city = FindCity( control )
		ShowSimpleCityTip( control, city, L"TXT_KEY_CITY_PUPPET", L"TXT_KEY_CITY_ANNEX_TT" )
	end,
	CityFocus = function( control )
		local city = FindCity( control )
		ShowSimpleCityTip( control, city, city and g_cityFocusTooltips[city:GetFocusType()] )
	end,
	CityQuests = function( control )
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
	CityIsRazing = function( control )
		local city = FindCity( control )
		ShowSimpleCityTip( control, city, L( "TXT_KEY_CITY_BURNING", city:GetRazingTurns() ) )
	end,
	CityIsResistance = function( control )
		local city = FindCity( control )
		ShowSimpleCityTip( control, city, L( "TXT_KEY_CITY_RESISTANCE", city:GetResistanceTurns() ) )
	end,
	CityIsConnected = function( control )
		local city = FindCity( control )
		ShowSimpleCityTip( control, city, L("TXT_KEY_CITY_CONNECTED") .. (" (%+g[ICON_GOLD])"):format( ( bnw_mode and g_activePlayer:GetCityConnectionRouteGoldTimes100( city ) or g_activePlayer:GetRouteGoldTimes100( city ) ) / 100 ) ) -- stupid function renaming
	end,
	CityIsBlockaded = function( control )
		local city = FindCity( control )
		ShowSimpleCityTip( control, city, L"TXT_KEY_CITY_BLOCKADED" )
	end,
	CityIsOccupied = function( control )
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

local g_ActionIM = StackInstanceManager( "UnitAction",  "UnitActionButton", Controls.ActionStack )
local g_BuildIM = StackInstanceManager( "UnitAction",  "UnitActionButton", Controls.WorkerActionStack )
local g_EarnedPromotionIM = StackInstanceManager( "EarnedPromotionInstance", "EarnedPromotion", Controls.EarnedPromotionStack )

local g_lastUnitID = -1		-- Used to determine if a different unit has been selected.
local g_isWorkerActionPanelOpen = false

local g_unitPortraitSize = Controls.UnitPortrait:GetSizeX()

local g_recommendedActionButton = {}
ContextPtr:BuildInstanceForControlAtIndex( "UnitAction", g_recommendedActionButton, Controls.WorkerActionStack, 1 )
--Controls.RecommendedActionLabel:ChangeParent( g_recommendedActionButton.UnitActionButton )
local g_actionIconSize = g_recommendedActionButton.UnitActionIcon:GetSizeX()
local g_actionButtonSpacing = OptionsManager.GetSmallUIAssets() and 42 or 58

local g_existingBuild = {}
ContextPtr:BuildInstanceForControl( "UnitAction", g_existingBuild, Controls.WorkerActionStack )
g_existingBuild.WorkerProgressBar:SetPercent( 1 )
g_existingBuild.UnitActionButton:SetDisabled( true )
g_existingBuild.UnitActionButton:SetAlpha( 0.8 )


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
		local info = GameInfo_Builds[action.Type]
		return info.IconIndex, info.IconAtlas
	end,

	[ActionSubTypes.ACTIONSUBTYPE_CONTROL] = function(action)
		local info = GameInfo.Controls[action.Type]
		return info.IconIndex, info.IconAtlas
	end,
}
local g_yieldString = {
[YieldTypes.YIELD_FOOD or false] = "TXT_KEY_BUILD_FOOD_STRING",
[YieldTypes.YIELD_PRODUCTION or false] = "TXT_KEY_BUILD_PRODUCTION_STRING",
[YieldTypes.YIELD_GOLD or false] = "TXT_KEY_BUILD_GOLD_STRING",
[YieldTypes.YIELD_SCIENCE or false] = "TXT_KEY_BUILD_SCIENCE_STRING",
[YieldTypes.YIELD_CULTURE or false] = "TXT_KEY_BUILD_CULTURE_STRING",
[YieldTypes.YIELD_FAITH or false] = "TXT_KEY_BUILD_FAITH_STRING",
[YieldTypes.YIELD_TOURISM or false] = "TXT_KEY_BUILD_TOURISM_STRING",
[YieldTypes.YIELD_GOLDEN_AGE_POINTS or false] = "TXT_KEY_BUILD_GAP_STRING",
[YieldTypes.YIELD_GREAT_GENERAL_POINTS or false] = "TXT_KEY_BUILD_GGP_STRING",
[YieldTypes.YIELD_GREAT_ADMIRAL_POINTS or false] = "TXT_KEY_BUILD_GAP2_STRING",
[YieldTypes.YIELD_CULTURE_LOCAL or false] = "TXT_KEY_BUILD_CULTURE_LOCAL_STRING", 
} g_yieldString[false] = nil
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
	local plot = unit:GetPlot()

	local hasBuildAction, hasPromotion, recommendedBuild, canBuildSomething

	Controls.BackgroundCivFrame:SetHide( false )

	local hideCityButton = true
--	local currentBuildID = unit:GetBuildType()

	-- loop over all the game actions
	local actions = {}
	for actionID = 0, #GameInfoActions do
		local action = GameInfoActions[ actionID ]
		-- test CanHandleAction w/ visible flag (ie COULD train if ... )
		if action.Visible and Game.CanHandleAction( actionID, plot, true ) then
			local isPromotion = action.SubType == ActionSubTypes.ACTIONSUBTYPE_PROMOTION
			if isPromotion then
				hasPromotion = true
			end
			-- We hide the Action buttons when Units are out of moves so new players aren't confused
			if hasMovesLeft or isPromotion
				or action.Type == "COMMAND_CANCEL"
				or action.Type == "COMMAND_STOP_AUTOMATION"
			then
				action.ID = actionID
				local isBuild = action.SubType == ActionSubTypes.ACTIONSUBTYPE_BUILD
				if isBuild then
					canBuildSomething = true
				end
				action.isBuild = isBuild or action.Type == "INTERFACEMODE_ROUTE_TO" or action.Type == "AUTOMATE_BUILD"
				action.isPromotion = isPromotion
				table_insert(actions, action)
			end
		end
	end

	local numBuildActions = 0
	-- loop over all the unit actions
	for i = 1, #actions do
		local action = actions[i]

		local button
		if action.Type == "MISSION_FOUND" then
			button = Controls.BuildCityButton
			hideCityButton = false

		else
			local instance
			if action.isPromotion then
				instance = g_BuildIM:GetInstance()

			elseif action.isBuild and not hasPromotion then
				numBuildActions = numBuildActions + 1
				if hasMovesLeft and not recommendedBuild and unit:IsActionRecommended( action.ID ) then
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
			local buildTurnsLeft, buildProgress, buildTime
			if action.SubType == ActionSubTypes.ACTIONSUBTYPE_BUILD then
				buildTurnsLeft, buildProgress, buildTime = getUnitBuildProgressData( plot, action.MissionData, unit )
				instance.WorkerProgressBar:SetPercent( buildProgress / buildTime )
				if buildTurnsLeft < 1 then buildTurnsLeft = nil end
			end
			instance.WorkerProgressBar:SetHide( not buildProgress )
			instance.UnitActionText:SetText( buildTurnsLeft )
			button = instance.UnitActionButton
		end

		-- test w/o visible flag (ie can train right now)
		if not Game.CanHandleAction( action.ID, plot, false ) then
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

	--Controls.BuildStack:CalculateSize()
	--Controls.BuildStack:ReprocessAnchoring()
	if numBuildActions > 0 or hasPromotion then
		Controls.WorkerActionPanel:SetHide( false )
		g_isWorkerActionPanelOpen = true

		Controls.RecommendedActionLabel:SetHide( not recommendedBuild )
		g_recommendedActionButton.UnitActionButton:SetHide( not recommendedBuild )
		Controls.RecommendedActionDivider:SetHide( not recommendedBuild )
		local improvement = canBuildSomething and GameInfo.Improvements[ plot:GetImprovementType() ]
		local build = improvement and GameInfo_Builds{ ImprovementType = improvement.Type }()
		if build then
			numBuildActions = numBuildActions + 1
			IconHookup( build.IconIndex, g_actionIconSize, build.IconAtlas, g_existingBuild.UnitActionIcon )
		end
		g_existingBuild.UnitActionButton:SetHide( not build )
		Controls.WorkerText:SetHide( hasPromotion )
		Controls.PromotionText:SetHide( not hasPromotion )
		Controls.PromotionAnimation:SetHide( not hasPromotion )
		Controls.EditButton:SetHide( not hasPromotion )
		Controls.WorkerActionStack:SetWrapWidth( recommendedBuild and 232 or math_ceil( numBuildActions / math_ceil( numBuildActions / 5 ) ) * g_actionButtonSpacing )
		Controls.WorkerActionStack:CalculateSize()
		local x, y = Controls.WorkerActionStack:GetSizeVal()
		Controls.WorkerActionPanel:SetSizeVal( math_max( x, 200 ) + 50, y + 96 )
		Controls.WorkerActionStack:ReprocessAnchoring()
	else
		Controls.WorkerActionPanel:SetHide( true )
		g_isWorkerActionPanelOpen = false
	end

	Controls.ActionStack:SetHide( false )
	Controls.ActionStack:CalculateSize()
	Controls.ActionStack:ReprocessAnchoring()
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

	name = Locale_ToUpper(name)

	--local name = unit:GetNameKey()
	local convertedKey = L(name)
	convertedKey = Locale_ToUpper(convertedKey)

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

--	local thisUnitInfo = GameInfo_Units[unit:GetUnitType()]

	local flagOffset, flagAtlas = UI.GetUnitFlagIcon(unit)
	local textureOffset, textureAtlas = IconLookup( flagOffset, 32, flagAtlas )
	Controls.UnitIcon:SetTexture(textureAtlas)
	Controls.UnitIconShadow:SetTexture(textureAtlas)
	Controls.UnitIcon:SetTextureOffset(textureOffset)
	Controls.UnitIconShadow:SetTextureOffset(textureOffset)

	local unitOwnerID = unit:GetOwner()
	Controls.UnitIcon:SetColor( PrimaryColors[ unitOwnerID ] )
	Controls.UnitIconBackground:SetColor( BackgroundColors[ unitOwnerID ] )

	local portraitOffset, portraitAtlas = UI_GetUnitPortraitIcon( unit )
	textureOffset, textureAtlas = IconLookup( portraitOffset, g_unitPortraitSize, portraitAtlas )
	Controls.UnitPortrait:SetTexture(textureAtlas or nullOffset)
	Controls.UnitPortrait:SetTextureOffset(textureOffset or defaultErrorTextureSheet)

	--These controls are potentially hidden if the previous selection was a city.
	Controls.UnitTypeFrame:SetHide(false)
	Controls.CycleLeft:SetHide(false)
	Controls.CycleRight:SetHide(false)

end

local function UpdateCityPortrait(city)
	local name = city:GetName()
	name = Locale_ToUpper(name)
	--local name = unit:GetNameKey()
	local convertedKey = L(name)
	convertedKey = Locale_ToUpper(convertedKey)

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
local UpdateUnitPromotions = EUI.UpdateUnitPromotions or function(unit)

	g_EarnedPromotionIM:ResetInstances()
	local controlTable

	--For each avail promotion, display the icon
	for unitPromotion in GameInfo.UnitPromotions() do
		local unitPromotionID = unitPromotion.ID

		if unit:IsHasPromotion(unitPromotionID) and unitPromotion.ShowInUnitPanel ~= false then

			controlTable = g_EarnedPromotionIM:GetInstance()
			IconHookup( unitPromotion.PortraitIndex, 32, unitPromotion.IconAtlas, controlTable.UnitPromotionImage )

			-- Tooltip
			local sDurationTip = ""
			if unit:GetPromotionDuration(unitPromotionID) > 0 then
				sDurationTip = " (" .. Locale.ConvertTextKey("TXT_KEY_STR_TURNS", unit:GetPromotionDuration(unitPromotionID) - (Game.GetGameTurn() - unit:GetTurnPromotionGained(unitPromotionID))) .. ")"
			end
			controlTable.EarnedPromotion:SetToolTipString( L(unitPromotion.Description) .. sDurationTip .. "[NEWLINE][NEWLINE]" .. L(unitPromotion.Help) )
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

	updateSmallUnitIcons( unit, Controls )

	Controls.UnitStatBox:SetHide( bnw_mode and unit:IsTrade() )
--	Controls.UnitStatBox:SetHide( false )

	-- Movement
	if unit:GetDomainType() == DomainTypes.DOMAIN_AIR then
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
		Controls.UnitStatStrength:SetText( math_floor(unit:GetConversionStrength()/g_pressureMultiplier) .. "[ICON_PEACE]" )
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
	elseif (unit:GetChargesLeft() > 0) then
		Controls.UnitStatRangedAttack:SetText( "[ICON_GREAT_ADMIRAL]" ..unit:GetChargesLeft() )
		Controls.UnitStatRangedAttack:SetToolTipString( L"TXT_KEY_UPANEL_REPAIR_CHARGES_TT" )
--	elseif gk_mode and GameInfo_Units[unit:GetUnitType()].RemoveHeresy then
--		Controls.UnitStatRangedAttack:LocalizeAndSetText( "TXT_KEY_UPANEL_REMOVE_HERESY_USES" )
--		Controls.UnitStatRangedAttack:LocalizeAndSetToolTip( "TXT_KEY_UPANEL_REMOVE_HERESY_USES_TT" )
	elseif unit:GetBuilderStrength() > 0 then
		Controls.UnitStatRangedAttack:SetText( "[ICON_WORKER]" ..unit:GetBuilderStrength() )
		Controls.UnitStatRangedAttack:SetToolTipString( L"TXT_KEY_UPANEL_BUILDER_STRENGTH_TT" )
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
		g_MaxDamage = unit:GetMaxHitPoints();
		local healthPercent = 1.0 - (damage / g_MaxDamage)
		local healthTimes100 =  math_floor(100 * healthPercent + 0.5)
		local barSize = { x = 9, y = math_floor(123 * healthPercent) }
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
	UnitToolTip( UI_GetHeadSelectedUnit(), L("TXT_KEY_CURRENTLY_SELECTED_UNIT"), "----------------" )
end)

Controls.UnitPortraitButton:RegisterCallback( Mouse.eLClick, OnUnitNameClicked )

Controls.UnitPortraitButton:RegisterCallback( Mouse.eRClick,
function()
	local unit = UI_GetHeadSelectedUnit()
	Events.SearchForPediaEntry( unit and unit:GetNameKey() )
end)

local function sortUnitsByDistanceToSelectedUnit( instance1, instance2 )
	local unit1 = g_activePlayer:GetUnitByID( instance1:GetVoid1() )
	local unit2 = g_activePlayer:GetUnitByID( instance2:GetVoid1() )
	local unit = UI_GetHeadSelectedUnit()
	if unit and unit1 and unit2 then
		local x = unit:GetX()
		local y = unit:GetY()
		return Map_PlotDistance( x, y, unit1:GetX(), unit1:GetY() ) < Map_PlotDistance( x, y, unit2:GetX(), unit2:GetY() )
	end
end

-------------------------------------------------
-- Unit rename
-------------------------------------------------
local function OnEditNameClick()

	if UI_GetHeadSelectedUnit() then
		Events.SerialEventGameMessagePopup{
				Type = ButtonPopupTypes.BUTTONPOPUP_RENAME_UNIT,
				Data1 = UI_GetHeadSelectedUnit():GetID(),
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

	local unit = UI_GetHeadSelectedUnit()
	if not unit then
		tipControlTable.UnitActionMouseover:SetHide( true )
		return
	end

	local actionID = control:GetVoid1()
	local action = GameInfoActions[actionID]
	local plot = unit:GetPlot()
	local x = unit:GetX()
	local y = unit:GetY()

	if action.Type == "MISSION_FOUND" then
		tipControlTable.UnitActionIcon:SetTextureOffsetVal( 0, 0 )
		tipControlTable.UnitActionIcon:SetTexture( "BuildCity64.dds" )
	else
		local f = g_actionIconIndexAndAtlas[ action.SubType ]
		if f then
			local iconIndex, iconAtlas = f( action )
			IconHookup( iconIndex, 64, iconAtlas, tipControlTable.UnitActionIcon )
		else
			print("Error - could not find method to obtain action icon", action.Type, action.SubType)
		end
	end

	-- Able to perform action
	local gameCanHandleAction = Game.CanHandleAction( actionID, plot, false )

	-- Build data
	local isBuild = action.SubType == ActionSubTypes.ACTIONSUBTYPE_BUILD
	local buildID = action.MissionData
	local build = GameInfo_Builds[buildID]

	-- Improvement data
	local improvement = build and GameInfo.Improvements[build.ImprovementType]
	local improvementID = improvement and improvement.ID or -1

	-- Feature data
	local featureID = plot:GetFeatureType()
	local feature = GameInfo.Features[featureID]

	-- Route data
	local route = build and build.RouteType and build.RouteType ~= "NONE" and GameInfo.Routes[build.RouteType]

	local strBuildTurnsString = ""
	local toolTip = table()
	local disabledTip = table()

	-- Upgrade has special help text
	if action.Type == "COMMAND_UPGRADE" then

		local upgradeUnitTypeID = unit:GetUpgradeUnitType()
		local unitUpgradePrice = unit:UpgradePrice(upgradeUnitTypeID)

		toolTip:insertLocalized( "TXT_KEY_UPGRADE_HELP", GameInfo_Units[upgradeUnitTypeID].Description, unitUpgradePrice )
		toolTip:insert( "----------------" )
		toolTip:insert( GetHelpTextForUnit( upgradeUnitTypeID, true ) )

		if not gameCanHandleAction then
			toolTip:insert( "----------------" )

			-- Can't upgrade because we're outside our territory
			if plot:GetOwner() ~= unit:GetOwner() then

				disabledTip:insertLocalized( "TXT_KEY_UPGRADE_HELP_DISABLED_TERRITORY" )
			end

			-- Can't upgrade because we're outside of a city
			if unit:GetDomainType() == DomainTypes.DOMAIN_AIR and not plot:IsCity() then

				disabledTip:insertLocalized( "TXT_KEY_UPGRADE_HELP_DISABLED_CITY" )
			end

			-- Can't upgrade because we lack the Gold
			if unitUpgradePrice > g_activePlayer:GetGold() then

				disabledTip:insertLocalized( "TXT_KEY_UPGRADE_HELP_DISABLED_GOLD" )
			end

			-- Can't upgrade because we lack the Resources
			local resourcesNeeded = table()

			-- Loop through all resources to see how many we need. If it's > 0 then add to the string
			for resource in GameInfo.Resources() do
				local iResourceLoop = resource.ID

				local iNumResourceNeededToUpgrade = unit:GetNumResourceNeededToUpgrade(iResourceLoop)

				if iNumResourceNeededToUpgrade > 0 and iNumResourceNeededToUpgrade > g_activePlayer:GetNumResourceAvailable(iResourceLoop) then
					resourcesNeeded:insert( iNumResourceNeededToUpgrade .. " " .. resource.IconString .. " " .. L(resource.Description) )
				end
				
				if Game.IsCustomModOption("UNITS_RESOURCE_QUANTITY_TOTALS") then
					local iNumResourcesTotalNeeded = unit:GetNumResourceTotalNeededToUpgrade(iResourceLoop)
					
					if iNumResourcesTotalNeeded > 0 and iNumResourcesTotalNeeded > g_activePlayer:GetNumResourceTotal(iResourceLoop) then
						resourcesTotalNeeded:insert( iNumResourcesTotalNeeded .. " " .. resource.IconString .. " " .. L(resource.Description) )
					elseif iNumResourcesTotalNeeded > 0 and g_activePlayer:GetNumResourceAvailable(iResourceLoop) < 0 then
						resourcesPositiveNeeded:insert( resource.IconString .. " " .. L(resource.Description) )
					end
				end
			end

			-- Build resources required string
			if #resourcesNeeded > 0 then

				disabledTip:insertLocalized( "TXT_KEY_UPGRADE_HELP_DISABLED_RESOURCES", resourcesNeeded:concat(", ") )
			end
			
			-- Build resources total required string
			if Game.IsCustomModOption("UNITS_RESOURCE_QUANTITY_TOTALS") and #resourcesTotalNeeded > 0 then
				disabledTip:insertLocalized( "TXT_KEY_UPGRADE_HELP_DISABLED_GROSS_RESOURCES", resourcesTotalNeeded:concat(", ") )
			end
			if Game.IsCustomModOption("UNITS_RESOURCE_QUANTITY_TOTALS") and #resourcesPositiveNeeded > 0 then
				disabledTip:insertLocalized( "TXT_KEY_UPGRADE_HELP_DISABLED_RESOURCES_NET_NEGATIVE", resourcesPositiveNeeded:concat(", ") )
			end

				-- if we can't upgrade due to stacking
			if plot:GetNumFriendlyUnitsOfType(unit) > 1 then

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

		if gameCanHandleAction then
			local eGreatWorkSlotType = unit:GetGreatWorkSlotType()
			local iBuilding = g_activePlayer:GetBuildingOfClosestGreatWorkSlot(x, y, eGreatWorkSlotType)
			local pCity = g_activePlayer:GetCityOfClosestGreatWorkSlot(x, y, eGreatWorkSlotType)
			toolTip:insertLocalized( "TXT_KEY_MISSION_CREATE_GREAT_WORK_RESULT", GameInfo.Buildings[iBuilding].Description, pCity:GetNameKey() )
		end

	-- Paradrop
	elseif action.Type == "INTERFACEMODE_PARADROP" then
		toolTip:insertLocalized( "TXT_KEY_INTERFACEMODE_PARADROP_HELP_WITH_RANGE", unit:GetDropRange() )

	-- Sell Exotic Goods
	elseif bnw_mode and action.Type == "MISSION_SELL_EXOTIC_GOODS" then

		toolTip:insertLocalized( "TXT_KEY_MISSION_SELL_EXOTIC_GOODS_HELP" )

		if gameCanHandleAction then
			toolTip:insert( "----------------" )
			toolTip:insert( "+" .. unit:GetExoticGoodsGoldAmount() .. "[ICON_GOLD]" )
			toolTip:insertLocalized( "TXT_KEY_EXPERIENCE_POPUP", unit:GetExoticGoodsXPAmount() )
		end

	-- Great Scientist
	elseif bnw_mode and action.Type == "MISSION_DISCOVER" then

		toolTip:insertLocalized( "TXT_KEY_MISSION_DISCOVER_TECH_HELP" )

		if gameCanHandleAction then
			toolTip:insert( "----------------" )
			toolTip:insert( "+" .. unit:GetDiscoverAmount() .. "[ICON_RESEARCH]" )
		end

	-- Great Engineer
	elseif bnw_mode and action.Type == "MISSION_HURRY" then

		toolTip:insertLocalized( "TXT_KEY_MISSION_HURRY_PRODUCTION_HELP" )

		if gameCanHandleAction then
			toolTip:insert( "----------------" )
			toolTip:insert( "+" .. unit:GetHurryProduction(plot) .. "[ICON_PRODUCTION]" )
		end

	-- Great Merchant
	elseif bnw_mode and action.Type == "MISSION_TRADE" then

		toolTip:insertLocalized( "TXT_KEY_MISSION_CONDUCT_TRADE_MISSION_HELP" )

		if gameCanHandleAction then
			toolTip:insert( "----------------" )
			toolTip:insert( "+" .. unit:GetTradeInfluence(plot) .. "[ICON_INFLUENCE]" )
			toolTip:insert( "+" .. unit:GetTradeGold(plot) .. "[ICON_GOLD]" )
		end

	-- Great Writer
	elseif bnw_mode and action.Type == "MISSION_GIVE_POLICIES" then

		toolTip:insertLocalized( "TXT_KEY_MISSION_GIVE_POLICIES_HELP" )

		if gameCanHandleAction then
			toolTip:insert( "----------------" )
			toolTip:insert( "+" .. unit:GetGivePoliciesCulture() .. "[ICON_CULTURE]" )
		end

	-- Great Musician
	elseif bnw_mode and action.Type == "MISSION_ONE_SHOT_TOURISM" then

		toolTip:insertLocalized( "TXT_KEY_MISSION_ONE_SHOT_TOURISM_HELP" )

		if gameCanHandleAction then
			toolTip:insert( "----------------" )
			toolTip:insert( "+" .. unit:GetBlastTourism() .. "[ICON_TOURISM]" )
		end

	-- Help text
	elseif action.Help and action.Help ~= "" then

		toolTip:insertLocalized( action.Help )
	end

	-- Delete has special help text
	if action.Type == "COMMAND_DELETE" then

		toolTip:insertLocalized( "TXT_KEY_SCRAP_HELP", unit:GetScrapGold() )
	end

	-- Not able to perform action
	if not gameCanHandleAction then

		-- Worker build
		if isBuild then

			-- Figure out what the name of the thing is that we're looking at
			local strImpRouteKey = (improvement and improvement.Description) or (route and route.Description) or ""

			-- Don't have Tech for Build?
			if improvement or route then
				local prereqTech = GameInfo.Technologies[build.PrereqTech]
				if prereqTech and prereqTech.ID ~= -1 and not g_activeTechs:HasTech(prereqTech.ID) then

						disabledTip:insertLocalized( "TXT_KEY_BUILD_BLOCKED_PREREQ_TECH", prereqTech.Description, strImpRouteKey )
				end
			end

			-- Trying to build something and are not adjacent to our territory?
			if gk_mode and improvement and improvement.InAdjacentFriendly then
				if plot:GetTeam() ~= unit:GetTeam() and not plot:IsAdjacentTeam(unit:GetTeam(), true) then

						disabledTip:insertLocalized( "TXT_KEY_BUILD_BLOCKED_NOT_IN_ADJACENT_TERRITORY", strImpRouteKey )
				end

			-- Trying to build something in a City-State's territory?
			elseif bnw_mode and improvement and improvement.OnlyCityStateTerritory then
				if not plot:IsOwned() or not Players[plot:GetOwner()]:IsMinorCiv() then
					disabledTip:insertLocalized( "TXT_KEY_BUILD_BLOCKED_NOT_IN_CITY_STATE_TERRITORY", strImpRouteKey )
				end

			-- Trying to build something outside of our territory?
			elseif improvement and not improvement.OutsideBorders then
				if plot:GetTeam() ~= unit:GetTeam() then
					disabledTip:insertLocalized( "TXT_KEY_BUILD_BLOCKED_OUTSIDE_TERRITORY", strImpRouteKey )
				end
			end

			-- Trying to build something that requires an adjacent luxury?
			if bnw_mode and improvement and improvement.AdjacentLuxury then
				local bAdjacentLuxury = false

				for _, direction in ipairs( g_directionTypes ) do
					local adjacentPlot = Map_PlotDirection(x, y, direction)
					if adjacentPlot then
						local eResourceType = adjacentPlot:GetResourceType()
						if eResourceType ~= -1 then
							if Game.GetResourceUsageType(eResourceType) == ResourceUsageTypes.RESOURCEUSAGE_LUXURY then
								bAdjacentLuxury = true
								break
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

				for _, direction in ipairs( g_directionTypes ) do
					local adjacentPlot = Map_PlotDirection(x, y, direction)
					if adjacentPlot then
						if adjacentPlot:GetImprovementType() == improvementID or adjacentPlot:GetBuildProgress(buildID) > 0 then
							bTwoAdjacent = true
							break
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

		local turnsRemaining = getUnitBuildProgressData( plot, buildID, unit )
		if turnsRemaining > 0 then
			strBuildTurnsString = " ... " .. L("TXT_KEY_STR_TURNS", turnsRemaining )
		end

		-- Extra Yield from this build

		for yieldID = 0, YieldTypes.NUM_YIELD_TYPES-1 do
			local yieldChange = plot:GetYieldWithBuild( buildID, yieldID, false, g_activePlayerID ) - plot:CalculateYield(yieldID)

			if yieldChange > 0 then
				toolTip:insert( "[COLOR_POSITIVE_TEXT]+" .. L( g_yieldString[yieldID], yieldChange) )
			elseif  yieldChange < 0 then
				toolTip:insert( "[COLOR_NEGATIVE_TEXT]" .. L( g_yieldString[yieldID], yieldChange) )
			end
		end

		-- Resource connection
		if improvement then
			local resourceID = plot:GetResourceType(g_activeTeamID)
			if resourceID ~= -1
				and plot:IsResourceConnectedByImprovement(improvementID)
				and Game.GetResourceUsageType(resourceID) ~= ResourceUsageTypes.RESOURCEUSAGE_BONUS
			then
						local resource = GameInfo.Resources[resourceID]
						toolTip:insertLocalized( "TXT_KEY_BUILD_CONNECTS_RESOURCE", resource.IconString, resource.Description )
			end
		end

		-- Production for clearing a feature
		if feature and plot:IsBuildRemovesFeature(buildID) then
			toolTip:insertLocalized( "TXT_KEY_BUILD_FEATURE_CLEARED", feature.Description )

			local featureProduction = plot:GetFeatureProduction(buildID, g_activeTeamID)
			if featureProduction > 0 then
				toolTip:append( L("TXT_KEY_BUILD_FEATURE_PRODUCTION", featureProduction) )
				local city = plot:GetWorkingCity()
				if city then
					toolTip:append( " (".. city:GetName()..")" )
				end
			end
		end
	end

	-- Tooltip
	tipControlTable.UnitActionHelp:SetText( toolTip:concat("[NEWLINE]") )

	-- Title
	tipControlTable.UnitActionText:SetText( "[COLOR_POSITIVE_TEXT]" .. L( tostring( action.TextKey or action.Type ) ) .. "[ENDCOLOR]".. strBuildTurnsString )

	-- HotKey
	if action.SubType == ActionSubTypes.ACTIONSUBTYPE_PROMOTION then
		tipControlTable.UnitActionHotKey:SetText()
	elseif action.HotKey and action.HotKey ~= "" then
		tipControlTable.UnitActionHotKey:SetText( "("..tostring(action.HotKey)..")" )
	else
		tipControlTable.UnitActionHotKey:SetText()
	end

	-- Autosize tooltip
	tipControlTable.UnitActionMouseover:DoAutoSize()
	local mouseoverSizeX = tipControlTable.UnitActionMouseover:GetSizeX()
	if mouseoverSizeX < 350 then
		tipControlTable.UnitActionMouseover:SetSizeX( 350 )
	end

end
g_existingBuild.UnitActionButton:SetToolTipCallback(
function()-- control )
	local unit = UI_GetHeadSelectedUnit()
	if unit then
		local plot = unit:GetPlot()
		local improvementID = plot:GetImprovementType()
		local improvement = GameInfo.Improvements[ improvementID ]
		local build = improvement and GameInfo_Builds{ ImprovementType = improvement.Type }()
		if build then
			-- Icon
			IconHookup( build.IconIndex, 64, build.IconAtlas, tipControlTable.UnitActionIcon )
			-- Title
			tipControlTable.UnitActionText:SetText( "[COLOR_POSITIVE_TEXT]" .. L( improvement.Description ) .. "[ENDCOLOR]" )
			-- no HotKey
			tipControlTable.UnitActionHotKey:SetText()
			-- Yield from this improvement
			local toolTip = table()
			for yieldID = 0, YieldTypes.NUM_YIELD_TYPES-1 do
				local yieldChange = plot:CalculateImprovementYieldChange( improvementID, yieldID, plot:GetOwner(), false )
				--plot:CalculateYield( yieldID ) - plot:CalculateNatureYield( yieldID, g_activeTeamID )

				if yieldChange > 0 then
					toolTip:insert( "[COLOR_POSITIVE_TEXT]+" .. L( g_yieldString[yieldID], yieldChange) )
				elseif  yieldChange < 0 then
					toolTip:insert( "[COLOR_NEGATIVE_TEXT]" .. L( g_yieldString[yieldID], yieldChange) )
				end
			end
			tipControlTable.UnitActionHelp:SetText( toolTip:concat("[NEWLINE]") )
			-- Autosize tooltip
			tipControlTable.UnitActionMouseover:DoAutoSize()
			return
		end
	end
	tipControlTable.UnitActionMouseover:SetHide( true )
end)

--[[----------------------------------------------------
 ___       _ _   _       _ _          _   _             
|_ _|_ __ (_) |_(_) __ _| (_)______ _| |_(_) ___  _ __  
 | || '_ \| | __| |/ _` | | |_  / _` | __| |/ _ \| '_ \ 
 | || | | | | |_| | (_| | | |/ / (_| | |_| | (_) | | | |
|___|_| |_|_|\__|_|\__,_|_|_/___\__,_|\__|_|\___/|_| |_|
]]------------------------------------------------------

local g_unitUpdateRequired, g_cityUpdateRequired, g_isUpdateRequired

local function UpdateDisplayNow()
--	if IsGameCoreBusy() then
--		return
--	end

	g_isUpdateRequired = false
	ContextPtr:ClearUpdate()

	-- update all ribbon units
	if g_unitUpdateRequired then
		g_unitUpdateRequired = false
		g_units.Update()
		-- Retrieve the currently selected unit.
		local unit = UI_GetHeadSelectedUnit()
		local unitID = unit and unit:GetID() or -1

		-- Unit is different than last unit ?
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
			local n = Controls.UnitTypesStack:GetNumChildren()
			Controls.UnitTypesStack:SetWrapWidth( math_ceil( n / math_ceil( n / 5 ) ) * 64 )
			Controls.UnitTypesStack:CalculateSize()
			local x, y = Controls.UnitTypesStack:GetSizeVal()
			if y<64 then y=64 elseif y>320 then y=320 end
			Controls.UnitTypesPanel:SetOffsetVal( unitTypesPanelOffsetX, unitTypesPanelOffsetY )
			Controls.UnitTypesPanel:SetSizeVal( x+40, y+85 )
			Controls.UnitTypesScrollPanel:SetSizeVal( x, y )
			Controls.UnitTypesScrollPanel:CalculateInternalSize()
			Controls.UnitTypesScrollPanel:ReprocessAnchoring()
		end
	end
	-- update cities
	if g_cityUpdateRequired then
		g_cities.Update( g_cityUpdateRequired ~= true and g_cityUpdateRequired )
		g_cityUpdateRequired = false
	end

	local maxTotalStackHeight = g_screenHeight - g_topOffset - g_bottomOffset
	local halfTotalStackHeight = math_floor(maxTotalStackHeight / 2)

	Controls.CityStack:CalculateSize()
	local cityStackHeight = Controls.CityStack:GetSizeY()
	Controls.UnitStack:CalculateSize()
	local unitStackHeight = Controls.UnitStack:GetSizeY()

	if unitStackHeight + cityStackHeight <= maxTotalStackHeight then
		unitStackHeight = false
		halfTotalStackHeight = false
	elseif cityStackHeight <= halfTotalStackHeight then
		unitStackHeight = maxTotalStackHeight - cityStackHeight
		halfTotalStackHeight = false
	elseif unitStackHeight <= halfTotalStackHeight then
		cityStackHeight = maxTotalStackHeight - unitStackHeight
		unitStackHeight = false
	else
		cityStackHeight = halfTotalStackHeight
		unitStackHeight = halfTotalStackHeight
	end

	Controls.CityScrollPanel:SetHide( not halfTotalStackHeight )
	if halfTotalStackHeight then
		Controls.CityStack:ChangeParent( Controls.CityScrollPanel )
		Controls.CityScrollPanel:SetSizeY( cityStackHeight )
		Controls.CityScrollPanel:CalculateInternalSize()
	else
		Controls.CityStack:ChangeParent( Controls.CityPanel )
	end
	Controls.CityPanel:ReprocessAnchoring()
--	Controls.CityPanel:SetSizeY( cityStackHeight )

	Controls.UnitScrollPanel:SetHide( not unitStackHeight )
	if unitStackHeight then
		Controls.UnitStack:ChangeParent( Controls.UnitScrollPanel )
		Controls.UnitScrollPanel:SetSizeY( unitStackHeight )
		Controls.UnitScrollPanel:CalculateInternalSize()
	else
		Controls.UnitStack:ChangeParent( Controls.UnitPanel )
	end
	return Controls.UnitPanel:ReprocessAnchoring()
end

local function UpdateDisplay()
	if not g_isUpdateRequired then
		g_isUpdateRequired = true
		return ContextPtr:SetUpdate( UpdateDisplayNow )
	end
end

local function UpdateUnits()
	g_unitUpdateRequired = true
	return UpdateDisplay()
end

local function UpdateCities()
	g_cityUpdateRequired = true
	return UpdateDisplay()
end

local function UpdateSpecificCity( playerID, cityID )
	if playerID == g_activePlayerID then
		if g_cityUpdateRequired == false then
			g_cityUpdateRequired = cityID
			return UpdateDisplay()
		elseif g_cityUpdateRequired ~= cityID then
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
		EUI_options.SetValue( "RIBBON_"..GameInfo_Units[ unitTypeID ].Type, isChecked and 1 or 0 )
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
	function() -- CREATE ALL
		g_ActivePlayerUnitTypes = {}
		for unit in g_activePlayer:Units() do
			addUnitType( unit )
		end
	end,
	function( instance, unit ) -- INIT
		local portrait, portraitOffset, portraitAtlas = instance.Portrait, UI_GetUnitPortraitIcon( unit )
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
		local portraitOffset, portraitAtlas = UI_GetUnitPortraitIcon( itemID, g_activePlayerID )
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

local AddOrRemove = { [true] = "Remove", [false] = "Add" }
local function UpdateOptions()

	local flag = EUI_options.GetValue( "UnitRibbon" ) == 0
	if g_isHideUnitList ~= flag then
		g_isHideUnitList = flag
		Events.SerialEventUnitCreated[AddOrRemove[flag]]( CreateUnit )
		Events.SerialEventUnitDestroyed[AddOrRemove[flag]]( DestroyUnit )
	end
	g_unitUpdateRequired = true
	g_units.Initialize( g_isHideUnitList )
	Controls.UnitPanel:SetHide( g_isHideUnitList )

	flag = EUI_options.GetValue( "CityRibbon" ) == 0
	if g_isHideCityList ~= flag then
		g_isHideCityList = flag
		Events.SerialEventCityCreated[AddOrRemove[flag]]( CreateCity )
		Events.SerialEventCityDestroyed[AddOrRemove[flag]]( DestroyCity )
		Events.SerialEventCityCaptured[AddOrRemove[flag]]( DestroyCity )
		Events.SerialEventCityInfoDirty[AddOrRemove[flag]]( UpdateCities )
		Events.SerialEventCitySetDamage[AddOrRemove[flag]]( UpdateSpecificCity )
		Events.SpecificCityInfoDirty[AddOrRemove[flag]]( UpdateSpecificCity )
	end
	g_cityUpdateRequired = not flag
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
