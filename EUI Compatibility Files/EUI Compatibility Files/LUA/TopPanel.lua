-------------------------------
-- TopPanel.lua
-- coded by bc1 from Civ V 1.0.3.276 code
-- code is common using gk_mode and bnw_mode switches
-- compatible with Putmalk's Civ IV Diplomacy Features Mod v10
-- compatible with Gazebo's City-State Diplomacy Mod (CSD) for Brave New World v 23
-------------------------------
include( "EUI_utilities" )

Events.SequenceGameInitComplete.Add(function()
print( "Loading EUI top panel",ContextPtr,os.clock(), [[ 
 _____           ____                  _
|_   _|__  _ __ |  _ \ __ _ _ __   ___| |
  | |/ _ \| '_ \| |_) / _` | '_ \ / _ \ |
  | | (_) | |_) |  __/ (_| | | | |  __/ |
  |_|\___/| .__/|_|   \__,_|_| |_|\___|_|
          |_|
]])
local civ5_mode = InStrategicView ~= nil
local civBE_mode = not civ5_mode
local gk_mode = Game.GetReligionName ~= nil
local bnw_mode = Game.GetActiveLeague ~= nil
local civ5bnw_mode = civ5_mode and bnw_mode

-------------------------------
-- minor lua optimizations
-------------------------------
local math_ceil = math.ceil
local math_cos = math.cos
local math_floor = math.floor
local math_max = math.max
local math_min = math.min
local math_pi = math.pi
local math_sin = math.sin
local os_date = os.date
local os_time = os.time
local pairs = pairs
local tonumber = tonumber
local S = string.format

--EUI_utilities
local IconHookup = EUI.IconHookup
local CityPlots = EUI.CityPlots
local PopScratchDeal = EUI.PopScratchDeal
local PushScratchDeal = EUI.PushScratchDeal
local table = EUI.table
local YieldIcons = EUI.YieldIcons
local YieldNames = EUI.YieldNames
local GreatPeopleIcon = EUI.GreatPeopleIcon
local GameInfo = EUI.GameInfoCache -- warning! use iterator ONLY with table field conditions, NOT string SQL query
local GetHelpTextForAffinity
if civBE_mode then
	include( "EUI_tooltips" )
	GetHelpTextForAffinity = EUI.GetHelpTextForAffinity
end

local ButtonPopupTypes = ButtonPopupTypes
local ContextPtr = ContextPtr
local Controls = Controls
local DomainTypes = DomainTypes
local Events = Events
local FaithPurchaseTypes = FaithPurchaseTypes
local Game = Game
local GameDefines = GameDefines
local GameInfoTypes = GameInfoTypes
local GameOptionTypes = GameOptionTypes
local L = Locale.ConvertTextKey
local Locale = Locale
local Mouse = Mouse
local OptionsManager = OptionsManager
local OrderTypes = OrderTypes
local Players = Players
local PreGame = PreGame
local ResourceUsageTypes = ResourceUsageTypes
local Teams = Teams
local TradeableItems = TradeableItems
local UI = UI
local YieldTypes = YieldTypes

-------------------------------
-- Globals
-------------------------------

local g_activePlayerID, g_activePlayer, g_activeTeamID, g_activeTeam, g_activeCivilizationID, g_activeCivilization, g_activeTeamTechs

local g_isBasicHelp, g_isScienceEnabled, g_isPoliciesEnabled, g_isHappinessEnabled, g_isReligionEnabled, g_isHealthEnabled

local g_deal = UI.GetScratchDeal()
local g_resourceString = {}
local g_isSmallScreen = UIManager:GetScreenSizeVal() < (civ5bnw_mode and 1900 or 1600)
local g_isPopupUp = false
local g_requestTopPanelUpdate, g_requestToolTipControl, g_requestToolTipFunction
local g_toolTipHandler = {}

local g_options = Modding.OpenUserData( "Enhanced User Interface Options", 1)
local g_clockFormats = { "%H:%M", "%I:%M %p", "%X", "%c" }
local g_clockFormat, g_alarmTime
local g_startTurn = Game.GetStartTurn()

local g_scienceTextColor = civ5_mode and "[COLOR:33:190:247:255]" or "[COLOR_MENU_BLUE]"
local g_currencyIcon = civ5_mode and "[ICON_GOLD]" or "[ICON_ENERGY]"
local g_currencyString = civ5_mode and "GOLD" or "ENERGY"
--local g_happinessIcon = civ5_mode and "[ICON_HAPPY]" or "[ICON_HEALTH]"
local g_happinessString = civ5_mode and "HAPPINESS" or "HEALTH"

local textTipControls = {}
local tipControls = {}
TTManager:GetTypeControlTable( "TooltipTypeTopPanel", textTipControls )
TTManager:GetTypeControlTable( "EUI_TopPanelProgressTooltip", tipControls )
local g_luxuries = table()
for resource in GameInfo.Resources() do
	if Game.GetResourceUsageType(resource.ID) == ResourceUsageTypes.RESOURCEUSAGE_LUXURY then
		g_luxuries:insert( resource )
	end
end
g_luxuries:sort( function(a,b) return Locale.Compare( a.Description, b.Description ) == -1 end )
-------------------------------
-- Utilities
-------------------------------
local function GamePopup( popupType, data2 )
	Events.SerialEventGameMessagePopup{ Type = popupType, Data1 = 1, Data2 = data2 }
end
local GamePedia = Events.SearchForPediaEntry

local function Colorize( x )
	if x > 0 then
		return "[COLOR_POSITIVE_TEXT]" .. x .. "[ENDCOLOR]"
	elseif x < 0 then
		return "[COLOR_WARNING_TEXT]" .. x .. "[ENDCOLOR]"
	else
		return "0"
	end
end
local function ColorizeSigned( x )
	if x > 0 then
		return "[COLOR_POSITIVE_TEXT]+" .. x .. "[ENDCOLOR]"
	elseif x < 0 then
		return "[COLOR_WARNING_TEXT]" .. x .. "[ENDCOLOR]"
	else
		return "0"
	end
end
local function ColorizeAbs( x )
	if x > 0 then
		return "[COLOR_POSITIVE_TEXT]" .. x .. "[ENDCOLOR]"
	elseif x < 0 then
		return "[COLOR_WARNING_TEXT]" .. -x .. "[ENDCOLOR]"
	else
		return "0"
	end
end
local function requestToolTip( control )
	tipControls.Box:SetHide( false )
	g_requestToolTipControl = control
end
local function requestTextToolTip( control )
	textTipControls.TopPanelMouseover:SetHide( false )
	g_requestToolTipControl = control
end
local function setTextToolTip( tipText )
	textTipControls.TooltipLabel:SetText( tipText )
	return textTipControls.TopPanelMouseover:DoAutoSize()
end

-------------------------------------------------
-- Great People
-------------------------------------------------

-- TODO: optimize loop, add capability for several simultaneous gp's
local function ScanGP( player )
	local gp = nil
	for city in player:Cities() do

		for specialist in GameInfo.Specialists() do

			local gpuClass = specialist.GreatPeopleUnitClass	-- nil / UNITCLASS_ARTIST / UNITCLASS_SCIENTIST / UNITCLASS_MERCHANT / UNITCLASS_ENGINEER ...
			local unitClass = gpuClass and GameInfo.UnitClasses[gpuClass]
			if unitClass then
				local gpThreshold = city:GetSpecialistUpgradeThreshold(unitClass.ID)
				local gpProgress = city:GetSpecialistGreatPersonProgressTimes100(specialist.ID) / 100
				local gpChange = specialist.GreatPeopleRateChange * city:GetSpecialistCount( specialist.ID )
				for building in GameInfo.Buildings{ SpecialistType = specialist.Type } do
					if city:IsHasBuilding(building.ID) then
						gpChange = gpChange + building.GreatPeopleRateChange
					end
				end
				-- Vox Populi
				gpChange = gpChange + city:GetExtraSpecialistPoints(specialist.ID);
				gpChange = gpChange + player:GetMonopolyGreatPersonRateChange(specialist.ID);

				local gpChangePlayerMod = player:GetGreatPeopleRateModifier()
				local gpChangeCityMod = city:GetGreatPeopleRateModifier()
				-- CBP
				gpChangeCityMod = gpChangeCityMod + city:GetSpecialistCityModifier(specialist.ID);
				local gpChangeMonopolyMod = player:GetMonopolyGreatPersonRateModifier(specialist.ID);
				--END
				local gpChangePolicyMod = 0
				local gpChangeWorldCongressMod = 0
				local gpChangeGoldenAgeMod = 0
				local isGoldenAge = player:GetGoldenAgeTurns() > 0

				if bnw_mode then
					-- Generic GP mods

					gpChangePolicyMod = player:GetPolicyGreatPeopleRateModifier()

					local worldCongress = (Game.GetNumActiveLeagues() > 0) and Game.GetActiveLeague()

					-- GP mods by type
					if specialist.GreatPeopleUnitClass == "UNITCLASS_WRITER" then
						gpChangePlayerMod = gpChangePlayerMod + player:GetGreatWriterRateModifier()
						gpChangePolicyMod = gpChangePolicyMod + player:GetPolicyGreatWriterRateModifier()
						if worldCongress then
							gpChangeWorldCongressMod = gpChangeWorldCongressMod + worldCongress:GetArtsyGreatPersonRateModifier()
						end
						if isGoldenAge and player:GetGoldenAgeGreatWriterRateModifier() > 0 then
							gpChangeGoldenAgeMod = gpChangeGoldenAgeMod + player:GetGoldenAgeGreatWriterRateModifier()
						end
					elseif specialist.GreatPeopleUnitClass == "UNITCLASS_ARTIST" then
						gpChangePlayerMod = gpChangePlayerMod + player:GetGreatArtistRateModifier()
						gpChangePolicyMod = gpChangePolicyMod + player:GetPolicyGreatArtistRateModifier()
						if worldCongress then
							gpChangeWorldCongressMod = gpChangeWorldCongressMod + worldCongress:GetArtsyGreatPersonRateModifier()
						end
						if isGoldenAge and player:GetGoldenAgeGreatArtistRateModifier() > 0 then
							gpChangeGoldenAgeMod = gpChangeGoldenAgeMod + player:GetGoldenAgeGreatArtistRateModifier()
						end
					elseif specialist.GreatPeopleUnitClass == "UNITCLASS_MUSICIAN" then
						gpChangePlayerMod = gpChangePlayerMod + player:GetGreatMusicianRateModifier()
						gpChangePolicyMod = gpChangePolicyMod + player:GetPolicyGreatMusicianRateModifier()
						if worldCongress then
							gpChangeWorldCongressMod = gpChangeWorldCongressMod + worldCongress:GetArtsyGreatPersonRateModifier()
						end
						if isGoldenAge and player:GetGoldenAgeGreatMusicianRateModifier() > 0 then
							gpChangeGoldenAgeMod = gpChangeGoldenAgeMod + player:GetGoldenAgeGreatMusicianRateModifier()
						end
					elseif specialist.GreatPeopleUnitClass == "UNITCLASS_SCIENTIST" then
						gpChangePlayerMod = gpChangePlayerMod + player:GetGreatScientistRateModifier()
						gpChangePolicyMod = gpChangePolicyMod + player:GetPolicyGreatScientistRateModifier()
						if worldCongress then
							gpChangeWorldCongressMod = gpChangeWorldCongressMod + worldCongress:GetScienceyGreatPersonRateModifier()
						end
--CBP
						if isGoldenAge and player:GetGoldenAgeGreatScientistRateModifier() > 0 then
							gpChangeGoldenAgeMod = gpChangeGoldenAgeMod + player:GetGoldenAgeGreatScientistRateModifier()
						end
-- END
					elseif specialist.GreatPeopleUnitClass == "UNITCLASS_MERCHANT" then
						gpChangePlayerMod = gpChangePlayerMod + player:GetGreatMerchantRateModifier()
						gpChangePolicyMod = gpChangePolicyMod + player:GetPolicyGreatMerchantRateModifier()
						if worldCongress then
							gpChangeWorldCongressMod = gpChangeWorldCongressMod + worldCongress:GetScienceyGreatPersonRateModifier()
						end
--CBP
						if isGoldenAge and player:GetGoldenAgeGreatMerchantRateModifier() > 0 then
							gpChangeGoldenAgeMod = gpChangeGoldenAgeMod + player:GetGoldenAgeGreatMerchantRateModifier()
						end
-- END
					elseif specialist.GreatPeopleUnitClass == "UNITCLASS_ENGINEER" then
						gpChangePlayerMod = gpChangePlayerMod + player:GetGreatEngineerRateModifier()
						gpChangePolicyMod = gpChangePolicyMod + player:GetPolicyGreatEngineerRateModifier()
						if worldCongress then
							gpChangeWorldCongressMod = gpChangeWorldCongressMod + worldCongress:GetScienceyGreatPersonRateModifier()
						end
--CBP
						if isGoldenAge and player:GetGoldenAgeGreatEngineerRateModifier() > 0 then
							gpChangeGoldenAgeMod = gpChangeGoldenAgeMod + player:GetGoldenAgeGreatEngineerRateModifier()
						end
-- END
					-- Compatibility with Gazebo's City-State Diplomacy Mod (CSD) for Brave New World
					elseif player.GetGreatDiplomatRateModifier and specialist.GreatPeopleUnitClass == "UNITCLASS_GREAT_DIPLOMAT" then
						gpChangePlayerMod = gpChangePlayerMod + player:GetGreatDiplomatRateModifier()
--CBP
						if isGoldenAge and player:GetGoldenAgeGreatDiplomatRateModifier() > 0 then
							gpChangeGoldenAgeMod = gpChangeGoldenAgeMod + player:GetGoldenAgeGreatDiplomatRateModifier()
						end
-- END
					end

					-- Player mod actually includes policy mod and World Congress mod, so separate them for tooltip

					gpChangePlayerMod = gpChangePlayerMod - gpChangePolicyMod - gpChangeWorldCongressMod

				elseif gpuClass == "UNITCLASS_SCIENTIST" then

					gpChangePlayerMod = gpChangePlayerMod + player:GetTraitGreatScientistRateModifier()

				end

				local gpChangeMod = gpChangePlayerMod + gpChangePolicyMod + gpChangeWorldCongressMod + gpChangeCityMod + gpChangeGoldenAgeMod + gpChangeMonopolyMod;
				gpChange = (gpChangeMod / 100 + 1) * gpChange

				if gpChange > 0 then
					local gpTurns = math_ceil( (gpThreshold - gpProgress) / gpChange )
					if not gp or gpTurns < gp.Turns then
						gp = {
							Turns = gpTurns,
							City = city,
							Class = unitClass,
							Progress = gpProgress,
							Threshold = gpThreshold,
							Change = gpChange,
						}
					end
				end

			end -- unitClass
		end -- specialist
	end -- city
	return gp
end

-------------------------------------------------
-- Top Panel Update
-------------------------------------------------

local function UpdateTopPanelNow()

	g_requestTopPanelUpdate = false

	Controls.InstantYieldsIcon:SetText( S("[ICON_CAPITAL]") )
	-- my modification for Luxury Resources
	Controls.LuxuryResources:SetText( S("[ICON_GREAT_MERCHANT]") )
	-----------------------------
	-- Update science stats
	-----------------------------
	if g_isScienceEnabled then

		local sciencePerTurnTimes100 = g_activePlayer:GetScienceTimes100()

		local strSciencePerTurn = S( "+%.0f", sciencePerTurnTimes100 / 100 )

		-- Gold being deducted from our Science ?
		if g_activePlayer:GetScienceFromBudgetDeficitTimes100() == 0 then
			strSciencePerTurn = g_scienceTextColor .. strSciencePerTurn .. "[ENDCOLOR][ICON_RESEARCH]"	-- Normal Science state
		else
			strSciencePerTurn = "[COLOR:255:0:60:255]" .. strSciencePerTurn .. "[ENDCOLOR][ICON_RESEARCH]"	-- Science deductions
		end
		Controls.SciencePerTurn:SetText( strSciencePerTurn )

		if civ5_mode then
			local strScienceTurnsRemaining = ""
			local techID = g_activePlayer:GetCurrentResearch()

			if techID ~= -1 then
			-- research in progress

				local scienceProgress = g_activePlayer:GetResearchProgress(techID)
				local scienceNeeded = g_activePlayer:GetResearchCost(techID)

				if sciencePerTurnTimes100 > 0 then

					Controls.ScienceBar:SetPercent( scienceProgress / scienceNeeded )
					Controls.ScienceBarShadow:SetPercent( (scienceProgress*100 + sciencePerTurnTimes100) / scienceNeeded / 100 )
					if sciencePerTurnTimes100 > 0 then
						strScienceTurnsRemaining = g_activePlayer:GetResearchTurnsLeft(techID, true)
					end
					Controls.ScienceBox:SetHide( false )
				else
					Controls.ScienceBox:SetHide( true )
				end

			else
			-- not researching a tech

				Controls.ScienceBox:SetHide(true)
				techID = g_activeTeamTechs:GetLastTechAcquired()

			end

			-- if we have one, update the tech picture
			local techInfo = techID and techID ~= -1 and GameInfo.Technologies[ techID ]
			Controls.ScienceTurns:SetText( strScienceTurnsRemaining )
			Controls.TechIcon:SetHide(not (techInfo and IconHookup( techInfo.PortraitIndex, 45, techInfo.IconAtlas, Controls.TechIcon )) )
		end
	end

	-----------------------------
	-- Update Resources
	-----------------------------

	for resourceID, resourceInstance in pairs( g_resourceString ) do
		local resource = GameInfo.Resources[ resourceID ]

		local numResourceUsed = g_activePlayer:GetNumResourceUsed( resourceID )

		if numResourceUsed > 0
			or ( g_activeTeamTechs:HasTech(GameInfoTypes[resource.TechReveal])
			and ( civBE_mode or g_activeTeamTechs:HasTech(GameInfoTypes[resource.TechCityTrade]) ) )
		then
			resourceInstance:SetText( Colorize( g_activePlayer:GetNumResourceAvailable(resourceID, true) ) .. resource.IconString )
		else
			resourceInstance:SetText( "" )
		end
	end

	-----------------------------
	-- Update turn counter
	-----------------------------
	local gameTurn = Game.GetGameTurn()
	if g_startTurn > 0 then
		gameTurn = gameTurn .. "("..(gameTurn-g_startTurn)..")"
	end
	Controls.CurrentTurn:LocalizeAndSetText( "TXT_KEY_TP_TURN_COUNTER", gameTurn )

	local culturePerTurn, cultureProgress

	if civ5_mode then
		-- Clever Firaxis...
		culturePerTurn = g_activePlayer:GetTotalJONSCulturePerTurn()
		cultureProgress = g_activePlayer:GetJONSCulture()

		-----------------------------
		-- Update gold stats
		-----------------------------

		Controls.GoldPerTurn:LocalizeAndSetText( "TXT_KEY_TOP_PANEL_GOLD", g_activePlayer:GetGold(), g_activePlayer:CalculateGoldRate() )

		-----------------------------
		-- Update Happy & Golden Age
		-----------------------------

		local unhappyProductionModifier = 0
		local unhappyFoodModifier = 0
		local unhappyGoldModifier = 0

		if g_isHappinessEnabled then

			local happinessText
			local excessHappiness = g_activePlayer:GetHappinessForGAP()
			local turnsRemaining = ""

			local happypop = g_activePlayer:GetHappinessFromCitizenNeeds()
			local unhappypop = g_activePlayer:GetUnhappinessFromCitizenNeeds()
			local percent = g_activePlayer:GetExcessHappiness()

			happinessText = L( "TXT_KEY_HAPPINESS_TOP_PANEL_CBO", percent, unhappypop, happypop)
			Controls.HappinessString:SetText(happinessText)

			local goldenAgeTurns = g_activePlayer:GetGoldenAgeTurns()
			local happyProgress = g_activePlayer:GetGoldenAgeProgressMeter()
			local happyNeeded = g_activePlayer:GetGoldenAgeProgressThreshold()
			-- CBP
			local iGAPReligion = g_activePlayer:GetGAPFromReligion();
			local iGAPTrait = g_activePlayer:GetGAPFromTraits();
			local iGAPCities = g_activePlayer:GetGAPFromCities();
			-- END
			local happyProgressNext = (happyProgress + excessHappiness + iGAPReligion + iGAPTrait + iGAPCities);

			if goldenAgeTurns > 0 then
				Controls.GoldenAgeAnim:SetHide(false)
				Controls.HappyBox:SetHide(true)
				turnsRemaining = goldenAgeTurns
			else
				Controls.GoldenAgeAnim:SetHide(true)
				if happyNeeded > 0 then
					Controls.HappyBar:SetPercent( happyProgress / happyNeeded )
					Controls.HappyBarShadow:SetPercent( happyProgressNext / happyNeeded )
					if excessHappiness > 0 then
						turnsRemaining = math_ceil((happyNeeded - happyProgress) / (excessHappiness + iGAPReligion + iGAPTrait + iGAPCities))
					end
					Controls.HappyBox:SetHide(false)
				else
					Controls.HappyBox:SetHide(true)
				end
			end

			Controls.HappyTurns:SetText(turnsRemaining)
		end

		-----------------------------
		-- Update Faith
		-----------------------------
		if g_isReligionEnabled then
			local faithPerTurn = g_activePlayer:GetTotalFaithPerTurn()
			local faithProgress = g_activePlayer:GetFaith()
			local faithProgressNext = faithProgress + faithPerTurn

			local faithTarget, faithNeeded

			local iconSize = 45
			local faithPurchaseType = g_activePlayer:GetFaithPurchaseType()
			local faithPurchaseIndex = g_activePlayer:GetFaithPurchaseIndex()
			local capitalCity = g_activePlayer:GetCapitalCity()

			if faithPurchaseType == FaithPurchaseTypes.FAITH_PURCHASE_UNIT then

				faithTarget = GameInfo.Units[ faithPurchaseIndex ]
				faithNeeded = capitalCity and capitalCity:GetUnitFaithPurchaseCost(faithTarget.ID, true)

			elseif faithPurchaseType == FaithPurchaseTypes.FAITH_PURCHASE_BUILDING then

				faithTarget = GameInfo.Buildings[ faithPurchaseIndex ]
				faithNeeded = capitalCity and capitalCity:GetBuildingFaithPurchaseCost(faithTarget.ID, true)

			elseif faithPurchaseType == FaithPurchaseTypes.FAITH_PURCHASE_SAVE_PROPHET then

				faithTarget = GameInfo.Units.UNIT_PROPHET
				faithNeeded = g_activePlayer:GetMinimumFaithNextGreatProphet()

			elseif g_activePlayer:GetCurrentEra() < GameInfoTypes.ERA_INDUSTRIAL then
				if g_activePlayer:CanCreatePantheon(false) then

					faithTarget = GameInfo.Religions.RELIGION_PANTHEON
					iconSize = 48
					faithNeeded = Game.GetMinimumFaithNextPantheon()

				elseif Game.GetNumReligionsStillToFound(false, Game.GetActivePlayer()) > 0 then

					faithTarget = GameInfo.Units.UNIT_PROPHET
					faithNeeded = g_activePlayer:GetMinimumFaithNextGreatProphet()
				end
			end

			local turnsRemaining = ""
			if faithNeeded and faithNeeded > 0 then
				Controls.FaithBar:SetPercent( faithProgress / faithNeeded )
				Controls.FaithBarShadow:SetPercent( faithProgressNext / faithNeeded )
				if faithPerTurn > 0 then
					turnsRemaining = math_ceil((faithNeeded - faithProgress) / faithPerTurn )
				end
				Controls.FaithBox:SetHide(false)
				Controls.FaithString:SetText( S("+%i[ICON_PEACE]", faithPerTurn ) )
			else
				Controls.FaithBox:SetHide(true)
				Controls.FaithString:SetText( S("[ICON_PEACE]%i(+%i)", faithProgress, faithPerTurn ) )
			end

			Controls.FaithTurns:SetText( turnsRemaining )

			Controls.FaithIcon:SetHide( not (faithTarget and IconHookup(faithTarget.PortraitIndex, iconSize, faithTarget.IconAtlas, Controls.FaithIcon) ) )
		end

		-----------------------------
		-- Update Great People
		-----------------------------
		local gp = ScanGP( g_activePlayer )

		if gp then
			Controls.GpBar:SetPercent( gp.Progress / gp.Threshold )
			Controls.GpBarShadow:SetPercent( (gp.Progress+gp.Change) / gp.Threshold )
			Controls.GpTurns:SetText(gp.Turns)
			Controls.GpBox:SetHide(false)
			local gpUnit = GameInfo.Units[ gp.Class.DefaultUnit ]
			Controls.GpIcon:SetHide(not (gpUnit and IconHookup(gpUnit.PortraitIndex, 45, gpUnit.IconAtlas, Controls.GpIcon)))
		else
			Controls.GpBox:SetHide(true)
			Controls.GpIcon:SetHide(true)
			Controls.GpTurns:SetText("")
		end

		-----------------------------
		-- Update Military
		-----------------------------
		local iUnitsSupplied = g_activePlayer:GetNumUnitsSupplied();
		local iUnitsTotal = g_activePlayer:GetNumUnitsToSupply();

		if(iUnitsTotal > iUnitsSupplied)then
			Controls.UnitSupplyString:SetText( S("  [ICON_WAR] [COLOR_NEGATIVE_TEXT](%i/%i)[ENDCOLOR]", iUnitsTotal, iUnitsSupplied ) )
		else
			Controls.UnitSupplyString:SetText( S("  [ICON_WAR] (%i/%i)", iUnitsTotal, iUnitsSupplied ) )
		end

		Controls.UnitSupplyString:SetHide(false);
		Controls.UnitSupplyIcon:SetHide(false);

		-----------------------------
		-- Update Alerts
		-----------------------------

		Controls.WarningString:SetHide(true)

		-----------------------------
		-- Update date
		-----------------------------
		local date = Game.GetTurnString()

		if gk_mode and g_activePlayer:IsUsingMayaCalendar() then
			Controls.CurrentDate:LocalizeAndSetToolTip( "TXT_KEY_MAYA_DATE_TOOLTIP", g_activePlayer:GetMayaCalendarLongString(), date )
			date = g_activePlayer:GetMayaCalendarString()
		end

		Controls.CurrentDate:SetText( date )

		-----------------------------
		-- Update Tourism and
		-- International Trade Routes
		-----------------------------
		if bnw_mode then
			Controls.InternationalTradeRoutes:SetText( S( "%i/%i[ICON_INTERNATIONAL_TRADE]", g_activePlayer:GetNumInternationalTradeRoutesUsed(), g_activePlayer:GetNumInternationalTradeRoutesAvailable() ) )
			Controls.TourismString:SetText( S( "%+i[ICON_TOURISM]", g_activePlayer:GetTourism() / 100 ) )
		end
	else
		-----------------------------
		-- Update affinity status
		-----------------------------
		Controls.Purity:LocalizeAndSetText( "TXT_KEY_AFFINITY_STATUS", GameInfo.Affinity_Types.AFFINITY_TYPE_PURITY.IconString, g_activePlayer:GetAffinityLevel( GameInfoTypes.AFFINITY_TYPE_PURITY ) )
		local percentToNextPurityLevel = g_activePlayer:GetAffinityPercentTowardsNextLevel( GameInfoTypes.AFFINITY_TYPE_PURITY )
		if g_activePlayer:GetAffinityPercentTowardsMaxLevel( GameInfoTypes.AFFINITY_TYPE_PURITY ) >= 100 then
			percentToNextPurityLevel = 100
		end
		Controls.PurityProgressBar:Resize(5, math_floor((percentToNextPurityLevel/100)*30))

		Controls.Harmony:LocalizeAndSetText( "TXT_KEY_AFFINITY_STATUS", GameInfo.Affinity_Types.AFFINITY_TYPE_HARMONY.IconString, g_activePlayer:GetAffinityLevel( GameInfoTypes.AFFINITY_TYPE_HARMONY ) )
		local percentToNextHarmonyLevel = g_activePlayer:GetAffinityPercentTowardsNextLevel( GameInfoTypes.AFFINITY_TYPE_HARMONY )
		if g_activePlayer:GetAffinityPercentTowardsMaxLevel( GameInfoTypes.AFFINITY_TYPE_HARMONY ) >= 100 then
			percentToNextHarmonyLevel = 100
		end
		Controls.HarmonyProgressBar:Resize(5, math_floor((percentToNextHarmonyLevel/100)*30))

		Controls.Supremacy:LocalizeAndSetText( "TXT_KEY_AFFINITY_STATUS", GameInfo.Affinity_Types.AFFINITY_TYPE_SUPREMACY.IconString, g_activePlayer:GetAffinityLevel( GameInfoTypes.AFFINITY_TYPE_SUPREMACY ) )
		local percentToNextSupremacyLevel = g_activePlayer:GetAffinityPercentTowardsNextLevel( GameInfoTypes.AFFINITY_TYPE_SUPREMACY )
		if g_activePlayer:GetAffinityPercentTowardsMaxLevel( GameInfoTypes.AFFINITY_TYPE_SUPREMACY ) >= 100 then
			percentToNextSupremacyLevel = 100
		end
		Controls.SupremacyProgressBar:Resize(5, math_floor((percentToNextSupremacyLevel/100)*30))

		-----------------------------
		-- Update energy stats
		-----------------------------

		Controls.GoldPerTurn:LocalizeAndSetText( "TXT_KEY_TOP_PANEL_ENERGY", g_activePlayer:GetEnergy(), g_activePlayer:CalculateGoldRate() )

		-----------------------------
		-- Update Health
		-----------------------------
		if g_isHealthEnabled then
			local excessHealth = g_activePlayer:GetExcessHealth()
			if excessHealth < 0 then
				Controls.HealthString:SetText( S("[COLOR_RED]%i[ENDCOLOR][ICON_HEALTH_3]", -excessHealth) )
			else
				Controls.HealthString:SetText( S("[COLOR_GREEN]%i[ENDCOLOR][ICON_HEALTH_1]", excessHealth) )
			end
--				SetAutoWidthGridButton( Controls.HealthString, strHealth, BUTTON_PADDING )
		end

		-- Clever Firaxis...
		culturePerTurn = g_activePlayer:GetTotalCulturePerTurn()
		cultureProgress = g_activePlayer:GetCulture()
	end

	-----------------------------
	-- Update Culture
	-----------------------------

	if g_isPoliciesEnabled then

		local cultureTheshold = g_activePlayer:GetNextPolicyCost()
		local cultureProgressNext = cultureProgress + culturePerTurn
		local turnsRemaining = ""

		if cultureTheshold > 0 then
			Controls.CultureBar:SetPercent( cultureProgress / cultureTheshold )
			Controls.CultureBarShadow:SetPercent( cultureProgressNext / cultureTheshold )
			if culturePerTurn > 0 then
				turnsRemaining = math_ceil((cultureTheshold - cultureProgress) / culturePerTurn )
			end
			Controls.CultureBox:SetHide(false)
		else
			Controls.CultureBox:SetHide(true)
		end

		Controls.CultureTurns:SetText(turnsRemaining)
		Controls.CultureString:SetText( S("[COLOR_MAGENTA]+%i[ENDCOLOR][ICON_CULTURE]", culturePerTurn ) )
	end

	Controls.TopPanelInfoStack:CalculateSize()
	Controls.TopPanelDiploStack:CalculateSize()
	Controls.TopPanelInfoStack:ReprocessAnchoring()
	Controls.TopPanelDiploStack:ReprocessAnchoring()
	Controls.TopPanelBarL:SetSizeX( Controls.TopPanelInfoStack:GetSizeX() + 15 )
	Controls.TopPanelBarR:SetSizeX( Controls.TopPanelDiploStack:GetSizeX() + 15 )
end

---------------
-- Civilopedia
---------------
Controls.CivilopediaButton:RegisterCallback( Mouse.eLClick, function() GamePedia( "" ) end )

---------------
-- Menu
---------------
Controls.MenuButton:RegisterCallback( Mouse.eLClick,
function()
	return UIManager:QueuePopup( LookUpControl( "/InGame/GameMenu" ), PopupPriority.InGameMenu )
end)

-------------------------------------------------
-- Science Tooltip & Click Actions
-------------------------------------------------
local function OnTechLClick()
	GamePopup( ButtonPopupTypes.BUTTONPOPUP_TECH_TREE, -1 )
end
local function OnTechRClick()
	local techInfo = GameInfo.Technologies[ g_activePlayer:GetCurrentResearch() ] or GameInfo.Technologies[ g_activePlayer:GetCurrentResearch() ]
	GamePedia( techInfo and techInfo.Description or "TXT_KEY_TECH_HEADING1_TITLE" )	-- TXT_KEY_PEDIA_CATEGORY_3_LABEL
end

local function SetMark( line, size, percent, label, text )
	local r0 = size/2
	local r1 = size * 0.43
	local r2 = size * 0.47
	local angle = percent * math_pi * 2
	local x = math_sin( angle )
	local y = -math_cos( angle )
	line:SetEndVal( r1 * x + r0, r1 * y + r0 )
	label:SetOffsetVal( r2 * x, r2 * y )
	label:SetText( text )
end

g_toolTipHandler.SciencePerTurn = function()-- control )

	local tips = table()
	local tech, showLine1, showLine2, showBlankMeter, showLossMeter, showAnimMeter, showProgressMeter, showPortrait

	if not g_isScienceEnabled then
		tips:insert( L"TXT_KEY_TOP_PANEL_SCIENCE_OFF" .. ": " .. L"TXT_KEY_TOP_PANEL_SCIENCE_OFF_TOOLTIP" )
	else

		local sciencePerTurnTimes100 = g_activePlayer:GetScienceTimes100()
		local techID = g_activePlayer:GetCurrentResearch()
		local recentTechID = g_activeTeamTechs:GetLastTechAcquired()

		local size = 256

		if bnw_mode and g_activePlayer:IsAnarchy() then
			tips:insert( L( "TXT_KEY_TP_ANARCHY", g_activePlayer:GetAnarchyNumTurns() ) )
			tips:insert( "" )
		end

		if techID ~= -1 then
		-- Are we researching something right now?
			local researchTurnsLeft = g_activePlayer:GetResearchTurnsLeft( techID, true )
			tech = GameInfo.Technologies[ techID ]
			local researchCost = g_activePlayer:GetResearchCost( techID )
			local researchProgress = g_activePlayer:GetResearchProgress( techID )
			local tip = researchProgress .. "[ICON_RESEARCH]"
			if tech then
				tip = L( "TXT_KEY_PROGRESS_TOWARDS", g_scienceTextColor .. Locale.ToUpper( tech.Description ) .. "[ENDCOLOR]" ) .. " " .. tip .. "/ " .. researchCost .. "[ICON_RESEARCH]"
			end
			tips:insert( tip )

			if sciencePerTurnTimes100 > 0 then
				local scienceOverflowTimes100 = sciencePerTurnTimes100 * researchTurnsLeft + (researchProgress - researchCost) * 100
				local tip = g_scienceTextColor .. Locale.ToUpper( L( "TXT_KEY_STR_TURNS", researchTurnsLeft ) ) .. "[ENDCOLOR] " .. S( "%+g", scienceOverflowTimes100 / 100 ) .. "[ICON_RESEARCH]"
				if researchTurnsLeft > 1 then
					tip = L( "TXT_KEY_STR_TURNS", researchTurnsLeft -1 ) .. " " .. S( "%+g", (scienceOverflowTimes100 - sciencePerTurnTimes100) / 100 ) .. "[ICON_RESEARCH]  " .. tip
				end
				tips:insert( tip )
			end
-- todo: rationalize
			local cost = g_activePlayer:GetResearchCost(techID) * 100
			local progress = g_activePlayer:GetResearchProgress(techID) * 100
			local change = g_activePlayer:GetScienceTimes100()
			local loss = g_activePlayer:GetScienceFromBudgetDeficitTimes100()
			local turnsRemaining = g_activePlayer:GetResearchTurnsLeft(techID, true)

			local progressNext = progress + change

			if turnsRemaining < 2 then
				showAnimMeter = true
				if turnsRemaining > 0 then
					tipControls.ProgressMeter:SetPercents( progress / cost, 0 )
					showProgressMeter = true
				end
			else
				tipControls.ProgressMeter:SetPercents( math_min(1, progress / cost), progressNext / cost )
				showProgressMeter = true
			end

			-- Science LOSS from Budget Deficits
			if loss < 0 then
				showLossMeter = true
				showBlankMeter = true
				tipControls.BlankMeter:SetPercents( math_min(1, progressNext / cost ), 0 )
				tipControls.LossMeter:SetPercents( math_min(1, ( progressNext - loss ) / cost ), 0 )
			end

			if change ~= 0 then
				local overflow = change * turnsRemaining + progress - cost
				if turnsRemaining > 1 then
					SetMark( tipControls.Line1, size, ( overflow - change ) / cost, tipControls.Label1, turnsRemaining - 1 )
					showLine1 = true
				end
				if overflow < cost then
					SetMark( tipControls.Line2, size, overflow / cost, tipControls.Label2, turnsRemaining )
					showLine2 = true
				end
			end

		elseif recentTechID ~= -1 then
		-- maybe we just finished something
			showAnimMeter = true
			tech = GameInfo.Technologies[ recentTechID ]
			local tip = L"TXT_KEY_NOTIFICATION_SUMMARY_NEW_RESEARCH"
			if tech then
				tip = L"TXT_KEY_RESEARCH_FINISHED" .. " " .. g_scienceTextColor .. Locale.ToUpper( tech.Description ) .. "[ENDCOLOR], " .. tip
			end
			tips:insert( tip )
		end

		-- if we have one, update the tech picture
		showPortrait = tech and IconHookup( tech.PortraitIndex, size, tech.IconAtlas, tipControls.ItemPortrait )

		tips:insert( g_scienceTextColor )
		tips:insert( S( "%+g", sciencePerTurnTimes100 / 100 ) .. "[ENDCOLOR] " .. L"TXT_KEY_REPLAY_DATA_SCIENCEPERTURN" )

		-- Science LOSS from Budget Deficits
		tips:insertLocalizedIfNonZero( "TXT_KEY_TP_SCIENCE_FROM_BUDGET_DEFICIT", g_activePlayer:GetScienceFromBudgetDeficitTimes100() / 100 )

		-- Science from Cities
		tips:insertLocalizedIfNonZero( "TXT_KEY_TP_SCIENCE_FROM_CITIES", g_activePlayer:GetScienceFromCitiesTimes100(true) / 100 )

		-- Science from Trade Routes
		tips:insertLocalizedIfNonZero( "TXT_KEY_TP_SCIENCE_FROM_ITR", ( g_activePlayer:GetScienceFromCitiesTimes100(false) - g_activePlayer:GetScienceFromCitiesTimes100(true) ) / 100 )

		-- Science from Other Players
		tips:insertLocalizedIfNonZero( "TXT_KEY_TP_SCIENCE_FROM_MINORS", g_activePlayer:GetScienceFromOtherPlayersTimes100() / 100 )

		if civ5_mode then
			-- Science from Happiness
			tips:insertLocalizedIfNonZero( "TXT_KEY_TP_SCIENCE_FROM_HAPPINESS", g_activePlayer:GetScienceFromHappinessTimes100() / 100 )

			-- Science from Vassals / Compatibility with Putmalk's Civ IV Diplomacy Features Mod
			if g_activePlayer.GetYieldPerTurnFromVassals then
				tips:insertLocalizedIfNonZero( "TXT_KEY_TP_SCIENCE_VASSALS", g_activePlayer:GetYieldPerTurnFromVassals(YieldTypes.YIELD_SCIENCE) )
			end

			-- Compatibility with Gazebo's City-State Diplomacy Mod (CSD) for Brave New World v23
			if g_activePlayer.GetScienceRateFromMinorAllies and g_activePlayer.GetScienceRateFromLeagueAid then
				tips:insertLocalizedIfNonZero( "TXT_KEY_MINOR_SCIENCE_FROM_LEAGUE_ALLIES", g_activePlayer:GetScienceRateFromMinorAllies() )
				tips:insertLocalizedIfNonZero( "TXT_KEY_SCIENCE_FUNDING_FROM_LEAGUE", g_activePlayer:GetScienceRateFromLeagueAid() )
			end
-- CBP 
			-- Putmalk
			local g_bAllowResearchAgreements = Game.IsOption("GAMEOPTION_RESEARCH_AGREEMENTS")
			-- Science from Religion
			tips:insertLocalizedIfNonZero( "TXT_KEY_SCIENCE_FROM_RELIGION", g_activePlayer:GetYieldPerTurnFromReligion(YieldTypes.YIELD_SCIENCE))
			
			-- Science % lost from unhappiness
			local iScienceMinors = g_activePlayer:GetSciencePerTurnFromMinorCivs();
			tips:insertLocalizedIfNonZero( "TXT_KEY_SCIENCE_FROM_MINORS", iScienceMinors)
--END
			-- Science from Research Agreements
			tips:insertLocalizedIfNonZero( "TXT_KEY_TP_SCIENCE_FROM_RESEARCH_AGREEMENTS", g_activePlayer:GetScienceFromResearchAgreementsTimes100() / 100 )

			-- Show Research Agreements
-- CBP
			-- No research if using C4DF
			if(g_bAllowResearchAgreements) then
-- END
				local itemType, duration, finalTurn, data1, data2, data3, flag1, fromPlayerID
				local gameTurn = Game.GetGameTurn() - 1
				local researchAgreementCounters = {}
	
				PushScratchDeal()
				for i = 0, UI.GetNumCurrentDeals( g_activePlayerID ) - 1 do
					UI.LoadCurrentDeal( g_activePlayerID, i )
					g_deal:ResetIterator()
					repeat
						if bnw_mode then
							itemType, duration, finalTurn, data1, data2, data3, flag1, fromPlayerID = g_deal:GetNextItem()
						else
							itemType, duration, finalTurn, data1, data2, fromPlayerID = g_deal:GetNextItem()
						end
	--local itemKey for k,v in pairs( TradeableItems ) do if itemType == v then itemKey = k break end end
	--print( "Deal #", i, "item type", itemType, itemKey, "duration", duration, "finalTurn", finalTurn, "data1", data1, "data2", data2, "fromPlayerID", fromPlayerID)
						if itemType == TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT and fromPlayerID ~= g_activePlayerID then
							researchAgreementCounters[fromPlayerID] = finalTurn - gameTurn
							break
						end
					until not itemType
				end
				PopScratchDeal()
	
				local tipIndex = #tips
	
				for playerID = 0, GameDefines.MAX_MAJOR_CIVS-1 do
	
					local player = Players[playerID]
					local teamID = player:GetTeam()
	
					if playerID ~= g_activePlayerID and player:IsAlive() and g_activeTeam:IsHasMet(teamID) then
	
						-- has reseach agreement ?
						if g_activeTeam:IsHasResearchAgreement(teamID) then
							tips:insert( "[ICON_BULLET][COLOR_POSITIVE_TEXT]" .. player:GetName() .. "[ENDCOLOR]" )
							if researchAgreementCounters[playerID] then
								tips:append( " " .. g_scienceTextColor .. Locale.ToUpper( L( "TXT_KEY_STR_TURNS", researchAgreementCounters[playerID] ) ) .. "[ENDCOLOR]" )
							end
						else
							tips:insert( "[ICON_BULLET][COLOR_WARNING_TEXT]" .. player:GetName() .. "[ENDCOLOR]" )
						end
					end
				end
	
				if #tips > tipIndex then
					tips:insert( tipIndex+1, "" )
					tips:insert( tipIndex+2, L"TXT_KEY_DO_RESEARCH_AGREEMENT" )
				end
-- CBP
			end
-- END
		else
			-- Science from Health
			tips:insertLocalizedIfNonZero( "TXT_KEY_TP_SCIENCE_FROM_HEALTH", g_activePlayer:GetScienceFromHealthTimes100() / 100 )

			-- Science from Culture Rate
			tips:insertLocalizedIfNonZero( "TXT_KEY_TP_SCIENCE_FROM_CULTURE", g_activePlayer:GetScienceFromCultureTimes100() / 100 )

			-- Science from Diplomacy Rate
			local scienceFromDiplomacy = g_activePlayer:GetScienceFromDiplomacyTimes100() / 100
			tips:insertLocalizedIf( scienceFromDiplomacy > 0 and "TXT_KEY_TP_SCIENCE_FROM_DIPLOMACY", scienceFromDiplomacy )
			tips:insertLocalizedIf( scienceFromDiplomacy < 0 and "TXT_KEY_TP_NEGATIVE_SCIENCE_FROM_DIPLOMACY", -scienceFromDiplomacy )
		end

		-- Let people know that building more cities makes techs harder to get
		if bnw_mode and g_isBasicHelp then
			tips:insert( "" )
			tips:insert( L( "TXT_KEY_TP_TECH_CITY_COST", Game.GetNumCitiesTechCostMod() * ( 100 + ( civBE_mode and g_activePlayer:GetNumCitiesResearchCostDiscount() or 0 ) ) / 100 ) )
		end
	end

	tipControls.Text:SetText( tips:concat( "[NEWLINE]" ) )
	tipControls.Box:DoAutoSize()

	tipControls.Line1:SetHide( not showLine1 )
	tipControls.Label1:SetHide( not showLine1 )
	tipControls.Line2:SetHide( not showLine2 )
	tipControls.Label2:SetHide( not showLine2 )
	tipControls.BlankMeter:SetHide( not showBlankMeter )
	tipControls.LossMeter:SetHide( not showLossMeter )
	tipControls.AnimMeter:SetHide( not showAnimMeter )
	tipControls.ProgressMeter:SetHide( not showProgressMeter )
	tipControls.ItemPortrait:SetHide( not showPortrait )
end
Controls.SciencePerTurn:SetToolTipCallback( requestToolTip )
Controls.SciencePerTurn:RegisterCallback( Mouse.eLClick, OnTechLClick )
Controls.SciencePerTurn:RegisterCallback( Mouse.eRClick, OnTechRClick )
if civ5_mode then
	g_toolTipHandler.TechIcon = g_toolTipHandler.SciencePerTurn
	Controls.TechIcon:SetToolTipCallback( requestToolTip )
	Controls.TechIcon:RegisterCallback( Mouse.eLClick, OnTechLClick )
	Controls.TechIcon:RegisterCallback( Mouse.eRClick, OnTechRClick )
end

-------------------------------------------------
-- Gold Tooltip & Click Actions
-------------------------------------------------
g_toolTipHandler.GoldPerTurn = function()-- control )
	local tips = table()

	local goldPerTurnFromDiplomacy = g_activePlayer:GetGoldPerTurnFromDiplomacy()
	local goldPerTurnFromOtherPlayers = math_max(0,goldPerTurnFromDiplomacy)
	local goldPerTurnToOtherPlayers = -math_min(0,goldPerTurnFromDiplomacy)

	local goldPerTurnFromReligion = gk_mode and g_activePlayer:GetGoldPerTurnFromReligion() or 0
	local goldPerTurnFromCities = g_activePlayer:GetGoldFromCitiesTimes100() / 100;
	local cityConnectionGold = g_activePlayer:GetCityConnectionGoldTimes100() / 100;
-- C4DF
	-- Gold from Vassals
	local iGoldFromVassals = g_activePlayer:GetYieldPerTurnFromVassals(YieldTypes.YIELD_GOLD);
	local iGoldFromVassalTax = math.floor(g_activePlayer:GetMyShareOfVassalTaxes() / 100);
-- END
	local playerTraitGold = 0
	local tradeRouteGold = 0
	local goldPerTurnFromPolicies = 0

	local unitCost = g_activePlayer:CalculateUnitCost()
	local unitSupply = g_activePlayer:CalculateUnitSupply()
	local buildingMaintenance = g_activePlayer:GetBuildingGoldMaintenance()
	local improvementMaintenance = g_activePlayer:GetImprovementGoldMaintenance()
-- BEGIN C4DF
	local iExpenseFromVassalTaxes = g_activePlayer:GetExpensePerTurnFromVassalTaxes();
	local iVassalMaintenance = g_activePlayer:GetVassalGoldMaintenance();
-- END C4DF
	local routeMaintenance = 0
	local beaconEnergyDelta = 0

	if bnw_mode then
		tradeRouteGold = g_activePlayer:GetGoldFromCitiesMinusTradeRoutesTimes100() / 100;
		goldPerTurnFromCities, tradeRouteGold = tradeRouteGold, goldPerTurnFromCities - tradeRouteGold
		playerTraitGold = g_activePlayer:GetGoldPerTurnFromTraits()
		if g_activePlayer:IsAnarchy() then
			tips:insert( L("TXT_KEY_TP_ANARCHY", g_activePlayer:GetAnarchyNumTurns() ) )
			tips:insert( "" )
		end
	end

	-- Total gold
-- CBP
		-- Gold gained from happiness
	local iInternalRouteGold = g_activePlayer:GetInternalTradeRouteGoldBonus();
	local iGoldFromMinors = g_activePlayer:GetGoldPerTurnFromMinorCivs()

	local totalIncome, totalWealth
	local explicitIncome = goldPerTurnFromCities + goldPerTurnFromOtherPlayers + cityConnectionGold + goldPerTurnFromReligion + tradeRouteGold + playerTraitGold + iGoldFromMinors + iInternalRouteGold -- C4DF
-- C4DF CHANGE
	if (iGoldFromVassals > 0) then
		explicitIncome = explicitIncome + iGoldFromVassals;
	end
	if (iGoldFromVassalTax > 0) then
		explicitIncome = explicitIncome + iGoldFromVassalTax;
	end
-- C4DF END CHANGE
	if civ5_mode then
		totalWealth = g_activePlayer:GetGold()
		totalIncome = explicitIncome
	else
		totalWealth = g_activePlayer:GetEnergy()
		totalIncome = g_activePlayer:CalculateGrossGoldTimes100() + goldPerTurnToOtherPlayers * 100
		goldPerTurnFromPolicies = g_activePlayer:GetGoldPerTurnFromPolicies()
		explicitIncome = explicitIncome + goldPerTurnFromPolicies
		routeMaintenance = g_activePlayer:GetRouteEnergyMaintenance()
		beaconEnergyDelta = g_activePlayer:GetBeaconEnergyCostPerTurn()
	end
	tips:insert( L( "TXT_KEY_TP_AVAILABLE_GOLD", totalWealth ) )
	local totalExpenses = unitCost + unitSupply + buildingMaintenance + improvementMaintenance + goldPerTurnToOtherPlayers + routeMaintenance + beaconEnergyDelta
-- BEGIN C4DF
	if (iVassalMaintenance > 0) then
		totalExpenses = totalExpenses + iVassalMaintenance;
	end
	if (iExpenseFromVassalTaxes > 0) then
		totalExpenses = totalExpenses + iExpenseFromVassalTaxes;
	end
-- END C4DF
	tips:insert( "" )

	-- Gold per turn

	tips:insert( S( "[COLOR_YELLOW]%+g[ENDCOLOR] ", g_activePlayer:CalculateGoldRateTimes100() / 100 ) .. L(S("TXT_KEY_REPLAY_DATA_%sPERTURN", g_currencyString)) )

	-- Science LOSS from Budget Deficits

	tips:insertLocalizedIfNonZero( "TXT_KEY_TP_SCIENCE_FROM_BUDGET_DEFICIT", g_activePlayer:GetScienceFromBudgetDeficitTimes100() / 100 )

	-- Income

	tips:insert( "[COLOR_WHITE]" )

	-- EDIT CBP
	tips:insert( L("TXT_KEY_TP_TOTAL_INCOME", math.floor(totalIncome) ) )
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_CITY_OUTPUT", math.floor(goldPerTurnFromCities) )

	if bnw_mode then
		tips:insertLocalizedBulletIfNonZero( S("TXT_KEY_TP_%s_FROM_CITY_CONNECTIONS", g_currencyString), math.floor(cityConnectionGold) )
		tips:insertLocalizedBulletIfNonZero( civ5_mode and "TXT_KEY_TP_GOLD_FROM_ITR" or "TXT_KEY_TP_ENERGY_FROM_TRADE_ROUTES", math.floor(tradeRouteGold) )
		tips:insertLocalizedBulletIfNonZero( S("TXT_KEY_TP_%s_FROM_TRAITS", g_currencyString), playerTraitGold )
		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_ENERGY_FROM_POLICIES", goldPerTurnFromPolicies )
	else
		tips:insertLocalizedBulletIfNonZero( S("TXT_KEY_TP_%s_FROM_TR", g_currencyString), math.floor(cityConnectionGold) )
	end
-- C4DF
	-- Gold from Vassals / Compatibility with Putmalk's Civ IV Diplomacy Features Mod
	tips:insertLocalizedBulletIfNonZero( S("TXT_KEY_TP_GOLD_VASSALS", g_currencyString), iGoldFromVassals)
	tips:insertLocalizedBulletIfNonZero( S("TXT_KEY_TP_GOLD_VASSAL_TAX", g_currencyString), iGoldFromVassalTax)
-- END
--CBP
	tips:insertLocalizedBulletIfNonZero( S("TXT_KEY_TP_GOLD_FROM_INTERNAL_TRADE", g_currencyString), iInternalRouteGold)
	tips:insertLocalizedBulletIfNonZero( S("TXT_KEY_TP_%s_FROM_OTHERS", g_currencyString), goldPerTurnFromOtherPlayers )
	tips:insertLocalizedBulletIfNonZero( S("TXT_KEY_TP_%s_FROM_RELIGION", g_currencyString), goldPerTurnFromReligion )
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_YIELD_FROM_UNCATEGORIZED", math.floor(totalIncome - explicitIncome) )
-- CBP
	tips:insertLocalizedBulletIfNonZero( S("TXT_KEY_TP_GOLD_FROM_MINORS", g_currencyString), iGoldFromMinors)
--END
	tips:insert( "[ENDCOLOR]" )

	-- Spending

--END
	tips:insert( "[COLOR:255:150:150:255]" .. L("TXT_KEY_TP_TOTAL_EXPENSES", totalExpenses ) )
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNIT_MAINT", unitCost )
	tips:insertLocalizedBulletIfNonZero( S("TXT_KEY_TP_%s_UNIT_SUPPLY", g_currencyString), unitSupply )
	tips:insertLocalizedBulletIfNonZero( S("TXT_KEY_TP_%s_BUILDING_MAINT", g_currencyString), buildingMaintenance )
	tips:insertLocalizedBulletIfNonZero( S("TXT_KEY_TP_%s_TILE_MAINT", g_currencyString), improvementMaintenance )
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_ENERGY_ROUTE_MAINT", routeMaintenance )
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_GOLD_VASSAL_MAINT", iVassalMaintenance )	-- Compatibility with Putmalk's Civ IV Diplomacy Features Mod
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_GOLD_VASSAL_TAX", iExpenseFromVassalTaxes )	-- Compatibility with Putmalk's Civ IV Diplomacy Features Mod
	tips:insertLocalizedBulletIfNonZero( S("TXT_KEY_TP_%s_TO_OTHERS", g_currencyString), goldPerTurnToOtherPlayers )
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_ENERGY_TO_BEACON", beaconEnergyDelta )

	tips:insert( "[ENDCOLOR]" )

	-- show gold available for trade to the active player
	local tipIndex = #tips

	for playerID = 0, GameDefines.MAX_MAJOR_CIVS-1 do

		local player = Players[playerID]

		-- Valid player? - Can't be us, has to be alive, and has to be met
		if playerID ~= g_activePlayerID and player:IsAlive() and g_activeTeam:IsHasMet( player:GetTeam() ) then
			tips:insert( "[ICON_BULLET]" .. player:GetName() .. S("  %i%s(%+i)",
					g_deal:GetGoldAvailable(playerID, -1), g_currencyIcon, player:CalculateGoldRate() ) )
		end
	end

	if #tips > tipIndex then
		tips:insert( tipIndex+1, "" )
		tips:insert( tipIndex+2, L"TXT_KEY_EO_RESOURCES_AVAILBLE" )
	end

	-- Basic explanation

	if g_isBasicHelp then
		tips:insert( "" )
		tips:insertLocalized( S("TXT_KEY_TP_%s_EXPLANATION", g_currencyString) )
	end

	return setTextToolTip( tips:concat( "[NEWLINE]" ) )
end
Controls.GoldPerTurn:RegisterCallback( Mouse.eLClick, function() GamePopup( ButtonPopupTypes.BUTTONPOPUP_ECONOMIC_OVERVIEW ) end )
Controls.GoldPerTurn:RegisterCallback( Mouse.eRClick, function() GamePedia( S("TXT_KEY_%s_HEADING1_TITLE", g_currencyString) ) end )
Controls.GoldPerTurn:SetToolTipCallback( requestTextToolTip )

if civ5_mode then
	-------------------------------------------------
	-- Great People Tooltip & Click Actions
	-------------------------------------------------
	Controls.GpIcon:RegisterCallback( Mouse.eLClick,
	function()
		local gp = ScanGP( Players[Game.GetActivePlayer()] )
		if gp then
			return UI.DoSelectCityAtPlot( gp.City:Plot() )
		end
	end)
	Controls.GpIcon:RegisterCallback( Mouse.eRClick,
	function()
		local gp = ScanGP( Players[Game.GetActivePlayer()] )
		if gp then
			return GamePedia( GameInfo.Units[ gp.Class.DefaultUnit ].Description )
		end
	end)
	g_toolTipHandler.GpIcon = function()-- control )
		local tipText = ""
		local gp = ScanGP( Players[Game.GetActivePlayer()] )
		if gp then
			local icon = GreatPeopleIcon( gp.Class.Type )
			tipText = L( "TXT_KEY_PROGRESS_TOWARDS", "[COLOR_YIELD_FOOD]" .. Locale.ToUpper( gp.Class.Description ) .. "[ENDCOLOR]" )
				.. " " .. gp.Progress .. icon .. " / " .. gp.Threshold .. icon .. "[NEWLINE]"
				.. gp.City:GetName() .. S( " %+g", gp.Change ) .. icon .. " " .. L"TXT_KEY_GOLD_PERTURN_HEADING4_TITLE"
				.. " [COLOR_YIELD_FOOD]" .. Locale.ToUpper( L( "TXT_KEY_STR_TURNS", gp.Turns ) ) .. "[ENDCOLOR]"
		else
			tipText = "No GP..."
		end
		return setTextToolTip( tipText )
	end
	Controls.GpIcon:SetToolTipCallback( requestTextToolTip )

	-------------------------------------------------
	-- Happiness Tooltip & Click Actions
	-------------------------------------------------
	g_toolTipHandler.HappinessString = function()-- control )

		if g_isHappinessEnabled then
			local tips = table()

		-- CBP EDITS HERE

			if (g_activePlayer:IsEmpireSuperUnhappy() and not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS)) then
					tips:insert( "[COLOR:255:60:60:255]" .. L("TXT_KEY_TP_EMPIRE_SUPER_UNHAPPY")  .. "[ENDCOLOR]" )
			elseif (g_activePlayer:IsEmpireSuperUnhappy() and Game.IsOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS)) then
				tips:insert( "[COLOR:255:60:60:255]" .. L("TXT_KEY_TP_EMPIRE_SUPER_UNHAPPY_NO_REBELS")  .. "[ENDCOLOR]" )
			elseif (g_activePlayer:IsEmpireVeryUnhappy() and not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS)) then	
				tips:insert("[COLOR:255:60:60:255]" .. L("TXT_KEY_TP_EMPIRE_VERY_UNHAPPY") .. "[ENDCOLOR]" )
			elseif (g_activePlayer:IsEmpireVeryUnhappy() and Game.IsOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS)) then
				tips:insert("[COLOR:255:60:60:255]" .. L("TXT_KEY_TP_EMPIRE_VERY_UNHAPPY_NO_REBELS") .. "[ENDCOLOR]" )
			elseif g_activePlayer:IsEmpireUnhappy() then
				tips:insert( "[COLOR:255:60:60:255]" .. L("TXT_KEY_TP_EMPIRE_UNHAPPY") .. "[ENDCOLOR]" )
			else
				tips:insert( "[COLOR:150:255:150:255]" .. L("TXT_KEY_TP_TOTAL_HAPPINESS") .. "[ENDCOLOR]" )
			-- Basic explanation of Happiness
			end

			if g_isBasicHelp then
				tips:insert( L"TXT_KEY_TP_HAPPINESS_EXPLANATION" )
				tips:insert( "" )
			end

			
			
			local empireUnhappiness = g_activePlayer:GetEmpireUnhappinessForCity();
	
			--------------
			-- Unhappiness
			local unhappinessFromPupetCities = g_activePlayer:GetUnhappinessFromPuppetCityPopulation() * 100
			local unhappinessFromSpecialists = g_activePlayer:GetUnhappinessFromCitySpecialists()
--CBP
			local unhappinessFromPop = (g_activePlayer:GetUnhappinessFromCityPopulation() - unhappinessFromPupetCities)
			if(unhappinessFromPop < 0)then
				unhappinessFromPop = 0
			end

			local iUnhappinessPublicOpinion = g_activePlayer:GetUnhappinessFromPublicOpinion();
			local iUnhappinessFromWar = g_activePlayer:GetUnhappinessFromWarWeariness();
			local totalunhappiness = iUnhappinessFromWar + iUnhappinessPublicOpinion;
--END	
			tips:insert( "[COLOR:255:150:150:255]" )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHAPPINESS_TOTAL", empireUnhappiness,totalunhappiness )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHAPPINESS_CITY_COUNT", g_activePlayer:GetUnhappinessFromCityCount() / 100 )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHAPPINESS_CAPTURED_CITY_COUNT", g_activePlayer:GetUnhappinessFromCapturedCityCount() / 100 )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHAPPINESS_POPULATION", unhappinessFromPop / 100 )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHAPPINESS_UNITS", g_activePlayer:GetUnhappinessFromUnits() / 100 )

			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHAPPINESS_PUBLIC_OPINION", iUnhappinessPublicOpinion)
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHAPPINESS_WAR_WEARINESS", iUnhappinessFromWar)	
		

			local empireHappiness = g_activePlayer:GetEmpireHappinessForCity();

			local religionHappiness = 0
			religionhappiness = g_activePlayer:GetHappinessFromReligion();
			local naturalwonderhappiness = g_activePlayer:GetHappinessFromNaturalWonders();
			local minorcivhappiness = g_activePlayer:GetHappinessFromMinorCivs();
			local leaguehappiness = g_activePlayer:GetHappinessFromLeagues();
			local vassalhappiness = g_activePlayer:GetHappinessFromVassals();
			local eventhappiness = g_activePlayer:GetEventHappiness();
			local tradehappiness = g_activePlayer:GetHappinessFromTradeRoutes();
			local resourcehappiness = g_activePlayer:GetBonusHappinessFromLuxuriesFlat();
			local perresourcehappiness  = g_activePlayer:GetBonusHappinessFromLuxuriesFlatForUI();
			local handicaphappiness = g_activePlayer:GetHandicapHappiness();

			------------
			-- Happiness
			tips:insert( "[ENDCOLOR][COLOR:150:255:150:255]" )
			
			local htotal = g_activePlayer:GetHappiness();
			if(htotal ~= 0) then
				tips:insert( L("TXT_KEY_TP_HAPPINESS_SOURCES", empireHappiness, htotal ) )
			end
	
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HAPPINESS_DIFFICULTY_LEVEL", handicaphappiness )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HAPPINESS_NATURAL_WONDERS", naturalwonderhappiness )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HAPPINESS_CITY_STATE_FRIENDSHIP", minorcivhappiness )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HAPPINESS_LEAGUES", leaguehappiness )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HAPPINESS_VASSALS", vassalhappiness )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HAPPINESS_EVENT", eventhappiness )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HAPPINESS_CONNECTED_CITIES", tradehappiness )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HAPPINESS_STATE_RELIGION_CBO", religionhappiness )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HAPPINESS_RESOURCE_CITY", resourcehappiness, perresourcehappiness )

			tips:insert( "[ENDCOLOR]" )
			
			tips:insert( "[COLOR:255:150:150:255]" )
			local iUnhappinessFromStarving = g_activePlayer:GetUnhappinessFromCityStarving();
			local iUnhappinessFromPillaged = g_activePlayer:GetUnhappinessFromCityPillaged();
			local iUnhappinessFromGold = g_activePlayer:GetUnhappinessFromCityGold();
			local iUnhappinessFromDefense = g_activePlayer:GetUnhappinessFromCityDefense();
			local iUnhappinessFromConnection = g_activePlayer:GetUnhappinessFromCityConnection();
			local iUnhappinessFromMinority = g_activePlayer:GetUnhappinessFromCityMinority();
			local iUnhappinessFromScience = g_activePlayer:GetUnhappinessFromCityScience();
			local iUnhappinessFromCulture = g_activePlayer:GetUnhappinessFromCityCulture();

			local total = iUnhappinessFromStarving+iUnhappinessFromPillaged+iUnhappinessFromGold+iUnhappinessFromDefense+iUnhappinessFromConnection+iUnhappinessFromMinority+iUnhappinessFromScience+iUnhappinessFromCulture;

			if(total ~= 0)then
				tips:insert( L"TXT_KEY_TP_UNHAPPINESS_NEEDS" )
			end

-- COMMUNITY PATCH CHANGES BELOW		
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHAPPINESS_PUPPET_CITIES", unhappinessFromPupetCities / 100 )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHAPPINESS_OCCUPIED_POPULATION", g_activePlayer:GetUnhappinessFromOccupiedCities() / 100 )
			
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHAPPINESS_STARVING", iUnhappinessFromStarving )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHAPPINESS_PILLAGED", iUnhappinessFromPillaged )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHAPPINESS_GOLD", iUnhappinessFromGold )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHAPPINESS_DEFENSE", iUnhappinessFromDefense )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHAPPINESS_CONNECTION", iUnhappinessFromConnection )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHAPPINESS_MINORITY", iUnhappinessFromMinority )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHAPPINESS_SCIENCE", iUnhappinessFromScience )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHAPPINESS_CULTURE", iUnhappinessFromCulture )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHAPPINESS_SPECIALISTS", unhappinessFromSpecialists / 100 )
			tips:insert( "[ENDCOLOR]" )
--END CHANGES
			return setTextToolTip( tips:concat( "[NEWLINE]" ) )
		else
			return setTextToolTip( L"TXT_KEY_TOP_PANEL_HAPPINESS_OFF_TOOLTIP" )
		end
	end
	Controls.HappinessString:RegisterCallback( Mouse.eLClick, function() GamePopup( ButtonPopupTypes.BUTTONPOPUP_ECONOMIC_OVERVIEW, 2 ) end )
	Controls.HappinessString:RegisterCallback( Mouse.eRClick, function() GamePedia( "TXT_KEY_GOLD_HEADING1_TITLE" ) end )
	Controls.HappinessString:SetToolTipCallback( requestTextToolTip )

	-------------------------------------------------
	-- Golden Age Tooltip
	-------------------------------------------------
	g_toolTipHandler.GoldenAgeString = function()-- control )

		if g_isHappinessEnabled then

			local tips = table()
			local excessHappiness = g_activePlayer:GetHappinessForGAP()
			local goldenAgeTurns = g_activePlayer:GetGoldenAgeTurns()
			local happyProgress = g_activePlayer:GetGoldenAgeProgressMeter()
			local happyNeeded = g_activePlayer:GetGoldenAgeProgressThreshold()
			-- CBP
			local iGAPReligion = g_activePlayer:GetGAPFromReligion();
			local iGAPTrait = g_activePlayer:GetGAPFromTraits();
			local iGAPCities = g_activePlayer:GetGAPFromCities();
			--END
			if goldenAgeTurns > 0 then
				if bnw_mode and g_activePlayer:GetGoldenAgeTourismModifier() > 0 then
					tips:insert( Locale.ToUpper"TXT_KEY_UNIQUE_GOLDEN_AGE_ANNOUNCE" )
				else
					tips:insert( Locale.ToUpper"TXT_KEY_GOLDEN_AGE_ANNOUNCE" )
				end
				tips:insert( L( "TXT_KEY_TP_GOLDEN_AGE_NOW", goldenAgeTurns ) )
			end
			tips:insert( L( "TXT_KEY_PROGRESS_TOWARDS", "[COLOR_YELLOW]"
				.. Locale.ToUpper( "TXT_KEY_SPECIALISTSANDGP_GOLDENAGE_HEADING4_TITLE" )
				.. "[ENDCOLOR]" ) .. " " .. happyProgress .. " / " .. happyNeeded )
			if excessHappiness > 0 then
				tips:insert( L("TXT_KEY_MISSION_START_GOLDENAGE") .. ": [COLOR_YELLOW]"
					.. Locale.ToUpper( L( "TXT_KEY_STR_TURNS", math_ceil((happyNeeded - happyProgress) / (excessHappiness + iGAPReligion + iGAPTrait + iGAPCities)) ) )
					.. "[ENDCOLOR]"	.. "[NEWLINE][NEWLINE]" .. L("TXT_KEY_TP_GOLDEN_AGE_ADDITION", excessHappiness) )
			elseif excessHappiness < 0 then
				tips:insert( "[COLOR_WARNING_TEXT]" .. L("TXT_KEY_TP_GOLDEN_AGE_LOSS", -excessHappiness) .. "[ENDCOLOR]" )
			end
			-- CBP
			local iGAPReligion = g_activePlayer:GetGAPFromReligion();
			if (iGAPReligion > 0) then
				tips:insert( "[NEWLINE]" .. L("TXT_KEY_TP_GOLDEN_AGE_ADDITION_RELIGION", iGAPReligion));
			end
				
			if (iGAPTrait > 0) then
				tips:insert( "[NEWLINE]" .. L("TXT_KEY_TP_GOLDEN_AGE_ADDITION_TRAIT", iGAPTrait));
			end
				
			if (iGAPCities > 0) then
				tips:insert( "[NEWLINE]" .. L("TXT_KEY_TP_GOLDEN_AGE_ADDITION_CITIES", iGAPCities));
			end
			-- END
			
			if g_isBasicHelp then
				tips:insert( "" )
				if gk_mode and g_activePlayer:IsGoldenAgeCultureBonusDisabled() then
					tips:insert( L"TXT_KEY_TP_GOLDEN_AGE_EFFECT_NO_CULTURE" )
				else
					tips:insert( L"TXT_KEY_TP_GOLDEN_AGE_EFFECT" )
				end
				if bnw_mode and g_activePlayer:GetGoldenAgeTurns() > 0 and g_activePlayer:GetGoldenAgeTourismModifier() > 0 then
					tips:insert( "" )
					tips:insert( L"TXT_KEY_TP_CARNIVAL_EFFECT" )
				end
			end

			return setTextToolTip( tips:concat( "[NEWLINE]" ) )
		else
			return setTextToolTip( L"TXT_KEY_TOP_PANEL_HAPPINESS_OFF_TOOLTIP" )
		end
	end
	Controls.GoldenAgeString:SetToolTipCallback( requestTextToolTip )

	-------------------------------------------------
	-- Tourism Tooltip & Click Actions
	-------------------------------------------------
	if bnw_mode then
		g_toolTipHandler.TourismString = function()-- control )

			local totalGreatWorks = g_activePlayer:GetNumGreatWorks()
			local totalSlots = g_activePlayer:GetNumGreatWorkSlots()

			local tipText = L( "TXT_KEY_TOP_PANEL_TOURISM_TOOLTIP_1", totalGreatWorks )
					.. "[NEWLINE]"
					.. L( "TXT_KEY_TOP_PANEL_TOURISM_TOOLTIP_2", totalSlots - totalGreatWorks )

			local cultureVictory = GameInfo.Victories.VICTORY_CULTURAL
			if cultureVictory and PreGame.IsVictory(cultureVictory.ID) then
				local numInfluential = g_activePlayer:GetNumCivsInfluentialOn()
				local numToBeInfluential = g_activePlayer:GetNumCivsToBeInfluentialOn()
				tipText = tipText .. "[NEWLINE][NEWLINE]"
					.. L( "TXT_KEY_TOP_PANEL_TOURISM_TOOLTIP_3", L("TXT_KEY_CO_VICTORY_INFLUENTIAL_OF", numInfluential, numToBeInfluential) )
			end

			--CBP
			local iTourismPenalty = g_activePlayer:GetTourismPenalty();

			tipText = tipText .. "[NEWLINE][NEWLINE]"
					.. L( "TXT_KEY_TOP_PANEL_TOURISM_TOOLTIP_CONQUEST_WARNING", iTourismPenalty )
			--END

			return setTextToolTip( tipText )
		end
		Controls.TourismString:SetHide(false)
		Controls.TourismString:RegisterCallback( Mouse.eLClick, function() GamePopup( ButtonPopupTypes.BUTTONPOPUP_CULTURE_OVERVIEW, 4 ) end )
		Controls.TourismString:RegisterCallback( Mouse.eRClick, function() GamePedia( "TXT_KEY_CULTURE_TOURISM_HEADING2_TITLE" ) end )	-- TXT_KEY_CULTURE_TOURISM_AND_CULTURE_HEADING2_TITLE
		Controls.TourismString:SetToolTipCallback( requestTextToolTip )

		-------------------------------------------------
		-- International Trade Routes Tooltip & Click Actions
		-------------------------------------------------
		g_toolTipHandler.InternationalTradeRoutes = function()-- control )

			local tipText = ""

			local numAvailableTradeUnits = g_activePlayer:GetNumAvailableTradeUnits(DomainTypes.DOMAIN_LAND)
			if numAvailableTradeUnits > 0 then
				local tradeUnitType = g_activePlayer:GetTradeUnitType(DomainTypes.DOMAIN_LAND)
				tipText = tipText .. L("TXT_KEY_TOP_PANEL_INTERNATIONAL_TRADE_ROUTES_TT_UNASSIGNED", numAvailableTradeUnits, GameInfo.Units[ tradeUnitType ].Description) .. "[NEWLINE]"
			end

			local numAvailableTradeUnits = g_activePlayer:GetNumAvailableTradeUnits(DomainTypes.DOMAIN_SEA)
			if numAvailableTradeUnits > 0 then
				local tradeUnitType = g_activePlayer:GetTradeUnitType(DomainTypes.DOMAIN_SEA)
				tipText = tipText .. L("TXT_KEY_TOP_PANEL_INTERNATIONAL_TRADE_ROUTES_TT_UNASSIGNED", numAvailableTradeUnits, GameInfo.Units[ tradeUnitType ].Description) .. "[NEWLINE]"
			end

			local usedTradeRoutes = g_activePlayer:GetNumInternationalTradeRoutesUsed()
			local availableTradeRoutes = g_activePlayer:GetNumInternationalTradeRoutesAvailable()

			if #tipText > 0 then
				tipText = tipText .. "[NEWLINE]"
			end
			tipText = L("TXT_KEY_TOP_PANEL_INTERNATIONAL_TRADE_ROUTES_TT", usedTradeRoutes, availableTradeRoutes)

			local strYourTradeRoutes = g_activePlayer:GetTradeYourRoutesTTString()
			if #strYourTradeRoutes > 0 then
				tipText = tipText .. "[NEWLINE][NEWLINE]"
						.. L("TXT_KEY_TOP_PANEL_ITR_ESTABLISHED_BY_PLAYER_TT")
						.. "[NEWLINE]"
						.. strYourTradeRoutes
			end

			local strToYouTradeRoutes = g_activePlayer:GetTradeToYouRoutesTTString()
			if #strToYouTradeRoutes > 0 then
				tipText = tipText .. "[NEWLINE][NEWLINE]"
						.. L("TXT_KEY_TOP_PANEL_ITR_ESTABLISHED_BY_OTHER_TT")
						.. "[NEWLINE]"
						.. strToYouTradeRoutes
			end

			return setTextToolTip( tipText )
		end
		Controls.InternationalTradeRoutes:SetHide(false)
		Controls.InternationalTradeRoutes:RegisterCallback( Mouse.eLClick, function() GamePopup( ButtonPopupTypes.BUTTONPOPUP_TRADE_ROUTE_OVERVIEW ) end )
		Controls.InternationalTradeRoutes:RegisterCallback( Mouse.eRClick, function() GamePedia( "TXT_KEY_TRADE_ROUTES_HEADING2_TITLE" ) end )	-- TXT_KEY_TRADE_ROUTES_HEADING2_TITLE
		Controls.InternationalTradeRoutes:SetToolTipCallback( requestTextToolTip )
	end
else
	-- ===========================================================================
	-- Health Tooltip
	-- ===========================================================================
	g_toolTipHandler.HealthString = function()-- control )
		if g_isHealthEnabled then

			local excessHealth = g_activePlayer:GetExcessHealth()
			local healthLevel = g_activePlayer:GetCurrentHealthLevel()
			local healthLevelInfo = GameInfo.HealthLevels[healthLevel]
			local colorPrefixText = "[COLOR_GREEN]"
			local iconStringText = "[ICON_HEALTH]"
			local rangeFactor = 1
			if excessHealth < 0 then
				colorPrefixText = "[COLOR_RED]"
				iconStringText = "[ICON_UNHEALTH]"
				rangeFactor = -1
			end
			local tips = table( L("TXT_KEY_TP_HEALTH_SUMMARY", iconStringText, colorPrefixText, excessHealth * rangeFactor) )

			tips:insertLocalizedIf( healthLevelInfo.Help )

			tips:insert( g_activePlayer:IsEmpireUnhealthy() and "[COLOR_WARNING_TEXT]" or "[COLOR_POSITIVE_TEXT]" )
			local cityYieldMods = {}
			local combatMod = 0
			local cityGrowthMod = 0
			local outpostGrowthMod = 0
			local cityIntrigueMod = 0
			for info in GameInfo.HealthLevels() do
				local healthLevelID = info.ID
				if g_activePlayer:IsAffectedByHealthLevel(healthLevelID) then
					for yieldID = 0, YieldTypes.NUM_YIELD_TYPES-1 do
						cityYieldMods[yieldID] = (cityYieldMods[yieldID] or 0) + Game.GetHealthLevelCityYieldModifier(healthLevelID, excessHealth, yieldID)
					end
					combatMod = combatMod + (info.CombatModifier or 0)
					cityGrowthMod = cityGrowthMod + Game.GetHealthLevelCityGrowthModifier(healthLevelID, excessHealth)
					outpostGrowthMod = outpostGrowthMod + Game.GetHealthLevelCityGrowthModifier(healthLevelID, excessHealth)
					cityIntrigueMod = cityIntrigueMod + Game.GetHealthLevelCityIntrigueModifier(healthLevelID, excessHealth)
				end
			end
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HEALTH_LEVEL_EFFECT_COMBAT_MODIFIER", combatMod )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HEALTH_LEVEL_EFFECT_CITY_GROWTH_MODIFIER", cityGrowthMod )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HEALTH_LEVEL_EFFECT_OUTPOST_GROWTH_MODIFIER", outpostGrowthMod )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HEALTH_LEVEL_EFFECT_CITY_INTRIGUE_MODIFIER", cityIntrigueMod )
			for yieldID = 0, YieldTypes.NUM_YIELD_TYPES-1 do
				tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HEALTH_LEVEL_EFFECT_CITY_YIELD_MODIFIER", cityYieldMods[yieldID] or 0, YieldIcons[yieldID] or "???", YieldNames[yieldID] or "???" )
			end
--			tips:insert( "[ENDCOLOR]" )

			--*** HEALTH Breakdown ***--
			local totalHealth		= g_activePlayer:GetHealth()
			local handicapInfo		= GameInfo.HandicapInfos[g_activePlayer:GetHandicapType()]
			local handicapHealth		= handicapInfo.BaseHealthRate
			local healthFromCities		= g_activePlayer:GetHealthFromCities()
			local extraCityHealth		= g_activePlayer:GetExtraHealthPerCity() * g_activePlayer:GetNumCities()
			local healthFromPolicies	= g_activePlayer:GetHealthFromPolicies()
			local healthFromTradeRoutes	= g_activePlayer:GetHealthFromTradeRoutes()
			--local healthFromNationalSecurityProject	= g_activePlayer:GetHealthFromNationalSecurityProject(); WRM: Add this in when we have a text string for it

			tips:insert( "[COLOR_WHITE]" )
			tips:insertLocalized( "TXT_KEY_TP_HEALTH_SOURCES", totalHealth )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HEALTH_CITIES", healthFromCities )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HEALTH_POLICIES", healthFromPolicies )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HEALTH_CONNECTED_CITIES", healthFromTradeRoutes )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HEALTH_CITY_COUNT", extraCityHealth )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HEALTH_DIFFICULTY_LEVEL", handicapHealth )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_HEALTH_OTHER_SOURCES", totalHealth - handicapHealth - healthFromPolicies - healthFromCities - healthFromTradeRoutes - extraCityHealth )
			tips:insert( "[ENDCOLOR]" )

			--*** UNHEALTH Breakdown ***--
			local totalUnhealth			= g_activePlayer:GetUnhealth()
			local unhealthFromCities		= g_activePlayer:GetUnhealthFromCities()
			local unhealthFromUnits			= g_activePlayer:GetUnhealthFromUnits()
			local unhealthFromCityCount		= g_activePlayer:GetUnhealthFromCityCount()
			local unhealthFromConqueredCityCount	= g_activePlayer:GetUnhealthFromConqueredCityCount()
			local unhealthFromPupetCities		= g_activePlayer:GetUnhealthFromPuppetCityPopulation()
			local unhealthFromSpecialists		= g_activePlayer:GetUnhealthFromCitySpecialists()
			local unhealthFromPop			= g_activePlayer:GetUnhealthFromCityPopulation() - unhealthFromSpecialists - unhealthFromPupetCities
			local unhealthFromConqueredCities	= g_activePlayer:GetUnhealthFromConqueredCities()

			tips:insert( "[COLOR:255:150:150:255]" )
			tips:insertLocalized( "TXT_KEY_TP_UNHEALTH_TOTAL", totalUnhealth )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHEALTH_CITIES", unhealthFromCities / 100 )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHEALTH_CITY_COUNT", unhealthFromCityCount / 100 )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHEALTH_CAPTURED_CITY_COUNT", unhealthFromConqueredCityCount / 100 )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHEALTH_POPULATION", unhealthFromPop / 100 )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHEALTH_PUPPET_CITIES", unhealthFromPupetCities / 100 )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHEALTH_SPECIALISTS", unhealthFromSpecialists / 100 )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHEALTH_OCCUPIED_POPULATION", unhealthFromConqueredCities / 100 )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHEALTH_UNITS", unhealthFromUnits / 100 )
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_YIELD_FROM_UNCATEGORIZED", totalUnhealth - ( unhealthFromCities + unhealthFromCityCount + unhealthFromConqueredCityCount + unhealthFromPop + unhealthFromPupetCities + unhealthFromSpecialists + unhealthFromConqueredCities + unhealthFromUnits ) / 100 )
			tips:insert( "[ENDCOLOR]" )

			-- Overall Unhealth Mod
			local unhealthMod = g_activePlayer:GetUnhealthMod()
			if unhealthMod > 0 then -- Positive mod means more Unhealth - this is a bad thing!
				tips:append( "[COLOR:255:150:150:255]" )
			end
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_TP_UNHEALTH_MOD", unhealthMod )

			-- Basic explanation of Health
			tips:insert( "[ENDCOLOR]" )
			tips:insertLocalized( "TXT_KEY_TP_HEALTH_EXPLANATION", totalUnhealth )

			return setTextToolTip( tips:concat( "[NEWLINE]" ) )
		else
			return setTextToolTip( L"TXT_KEY_TOP_PANEL_HEALTH_OFF_TOOLTIP" )
		end
	end
	Controls.HealthString:RegisterCallback( Mouse.eLClick, function() GamePopup( ButtonPopupTypes.BUTTONPOPUP_ECONOMIC_OVERVIEW, 2 ) end )
	Controls.HealthString:RegisterCallback( Mouse.eRClick, function() GamePedia( "TXT_KEY_GOLD_HEADING1_TITLE" ) end )
	Controls.HealthString:SetToolTipCallback( requestTextToolTip )

	-- ===========================================================================
	-- Affinity Tooltips
	-- ===========================================================================

	g_toolTipHandler.Harmony = function() return setTextToolTip( GetHelpTextForAffinity( GameInfoTypes.AFFINITY_TYPE_HARMONY, g_activePlayer ) ) end
	g_toolTipHandler.Purity = function() return setTextToolTip( GetHelpTextForAffinity( GameInfoTypes.AFFINITY_TYPE_PURITY, g_activePlayer ) ) end
	g_toolTipHandler.Supremacy = function() return setTextToolTip( GetHelpTextForAffinity( GameInfoTypes.AFFINITY_TYPE_SUPREMACY, g_activePlayer ) ) end

	Controls.Harmony:SetToolTipCallback( requestTextToolTip )
	Controls.Purity:SetToolTipCallback( requestTextToolTip )
	Controls.Supremacy:SetToolTipCallback( requestTextToolTip )
	Controls.Harmony:RegisterCallback( Mouse.eLClick, function() GamePopup( ButtonPopupTypes.BUTTONPOPUP_ECONOMIC_OVERVIEW, 2 ) end )
	Controls.Harmony:RegisterCallback( Mouse.eRClick, function() GamePedia( GameInfo.Affinity_Types.AFFINITY_TYPE_HARMONY.Description ) end )
	Controls.Purity:RegisterCallback( Mouse.eLClick, function() GamePopup( ButtonPopupTypes.BUTTONPOPUP_ECONOMIC_OVERVIEW, 2 ) end )
	Controls.Purity:RegisterCallback( Mouse.eRClick, function() GamePedia( GameInfo.Affinity_Types.AFFINITY_TYPE_PURITY.Description ) end )
	Controls.Supremacy:RegisterCallback( Mouse.eLClick, function() GamePopup( ButtonPopupTypes.BUTTONPOPUP_ECONOMIC_OVERVIEW, 2 ) end )
	Controls.Supremacy:RegisterCallback( Mouse.eRClick, function() GamePedia( GameInfo.Affinity_Types.AFFINITY_TYPE_SUPREMACY.Description ) end )
end
-------------------------------------------------
-- Culture Tooltip & Click Actions
-------------------------------------------------
g_toolTipHandler.CultureString = function()-- control )

	local tips = table()

	if not g_isPoliciesEnabled then
		tips:insert( L"TXT_KEY_TOP_PANEL_POLICIES_OFF_TOOLTIP" )
	else
		local turnsRemaining = 1
		local cultureProgress, culturePerTurn, culturePerTurnForFree, culturePerTurnFromCities, culturePerTurnFromExcessHappiness, culturePerTurnFromTraits
		-- Firaxis Cleverness...
		if civ5_mode then
			cultureProgress = g_activePlayer:GetJONSCulture()
			culturePerTurn = g_activePlayer:GetTotalJONSCulturePerTurn()
			culturePerTurnForFree = g_activePlayer:GetJONSCulturePerTurnForFree()
			culturePerTurnFromCities = g_activePlayer:GetJONSCulturePerTurnFromCities()
			culturePerTurnFromExcessHappiness = g_activePlayer:GetJONSCulturePerTurnFromExcessHappiness()
			culturePerTurnFromTraits = bnw_mode and g_activePlayer:GetJONSCulturePerTurnFromTraits() or 0
		else
			cultureProgress = g_activePlayer:GetCulture()
			culturePerTurn = g_activePlayer:GetTotalCulturePerTurn()
			culturePerTurnForFree = g_activePlayer:GetCulturePerTurnForFree()
			culturePerTurnFromCities = g_activePlayer:GetCulturePerTurnFromCities()
			culturePerTurnFromExcessHappiness = g_activePlayer:GetCulturePerTurnFromExcessHealth()
			culturePerTurnFromTraits = g_activePlayer:GetCulturePerTurnFromTraits()
		end
		local cultureTheshold = g_activePlayer:GetNextPolicyCost()
		if cultureTheshold > cultureProgress then
			if culturePerTurn > 0 then
				turnsRemaining = math_ceil( (cultureTheshold - cultureProgress) / culturePerTurn)
			else
				turnsRemaining = "?"
			end
		end

		if bnw_mode and g_activePlayer:IsAnarchy() then
			tips:insert( L("TXT_KEY_TP_ANARCHY", g_activePlayer:GetAnarchyNumTurns()) )
			tips:insert( "" )
		end

		tips:insert( L( "TXT_KEY_PROGRESS_TOWARDS", "[COLOR_MAGENTA]" .. Locale.ToUpper"TXT_KEY_ADVISOR_SCREEN_SOCIAL_POLICY_DISPLAY" .. "[ENDCOLOR]" )
				.. " " .. cultureProgress .. "[ICON_CULTURE]/ " .. cultureTheshold .. "[ICON_CULTURE]" )

		if culturePerTurn > 0 then
			local cultureOverflow = culturePerTurn * turnsRemaining + cultureProgress - cultureTheshold
			local tip = "[COLOR_MAGENTA]" .. Locale.ToUpper( L( "TXT_KEY_STR_TURNS", turnsRemaining ) )
					.. "[ENDCOLOR]"	.. S( " %+g[ICON_CULTURE]", cultureOverflow )
			if turnsRemaining > 1 then
				tip = L( "TXT_KEY_STR_TURNS", turnsRemaining -1 )
					.. S( " %+g[ICON_CULTURE]  ", cultureOverflow - culturePerTurn )
					.. tip
			end
			tips:insert( tip )
		end

		tips:insert( "" )
		tips:insert( "[COLOR_MAGENTA]" .. S( "%+g", culturePerTurn )
				.. "[ENDCOLOR] " .. L"TXT_KEY_REPLAY_DATA_CULTUREPERTURN" )

		-- Culture for Free
		tips:insertLocalizedIfNonZero( "TXT_KEY_TP_CULTURE_FOR_FREE", culturePerTurnForFree )

		-- Culture from Cities
		tips:insertLocalizedIfNonZero( "TXT_KEY_TP_CULTURE_FROM_CITIES", culturePerTurnFromCities )

		-- Culture from Excess Happiness / Health
		tips:insertLocalizedIfNonZero( "TXT_KEY_TP_CULTURE_FROM_" .. g_happinessString, culturePerTurnFromExcessHappiness )

		-- Culture from Traits
		tips:insertLocalizedIfNonZero( "TXT_KEY_TP_CULTURE_FROM_TRAITS", culturePerTurnFromTraits )

		if civ5_mode then
			-- Culture from Minor Civs
			local culturePerTurnFromMinorCivs = g_activePlayer:GetJONSCulturePerTurnFromMinorCivs()
			tips:insertLocalizedIfNonZero( "TXT_KEY_TP_CULTURE_FROM_MINORS", culturePerTurnFromMinorCivs )

			-- Culture from Religion
			local culturePerTurnFromReligion = gk_mode and g_activePlayer:GetCulturePerTurnFromReligion() or 0
			tips:insertLocalizedIfNonZero( "TXT_KEY_TP_CULTURE_FROM_RELIGION", culturePerTurnFromReligion )

			-- Culture from bonus turns (League Project)
			local culturePerTurnFromBonusTurns = 0
			if bnw_mode then
				culturePerTurnFromBonusTurns = g_activePlayer:GetCulturePerTurnFromBonusTurns()
				tips:insertLocalizedIfNonZero( "TXT_KEY_TP_CULTURE_FROM_BONUS_TURNS", culturePerTurnFromBonusTurns, g_activePlayer:GetCultureBonusTurns() )
			end

			-- Culture from Vassals / Compatibility with Putmalk's Civ IV Diplomacy Features Mod
-- C4DF
			local culturePerTurnFromVassals = 0;
			if g_activePlayer.GetYieldPerTurnFromVassals then
				culturePerTurnFromVassals = g_activePlayer:GetYieldPerTurnFromVassals(YieldTypes.YIELD_CULTURE)
				tips:insertLocalizedIfNonZero( "TXT_KEY_TP_CULTURE_VASSALS", culturePerTurnFromVassals )
			end
-- END
			-- Culture from Golden Age
-- CBP
	
			local iCultureFromGoldenAge = (culturePerTurn - culturePerTurnForFree - culturePerTurnFromCities - culturePerTurnFromExcessHappiness - culturePerTurnFromMinorCivs - culturePerTurnFromReligion - culturePerTurnFromTraits - culturePerTurnFromBonusTurns - culturePerTurnFromVassals)
			
			tips:insertLocalizedIfNonZero( "TXT_KEY_TP_CULTURE_FROM_GOLDEN_AGE", iCultureFromGoldenAge)
-- END
		
		else
			-- Uncategorized Culture
			tips:insertLocalizedIfNonZero( "TXT_KEY_TP_YIELD_FROM_UNCATEGORIZED", culturePerTurn - culturePerTurnForFree - culturePerTurnFromCities - culturePerTurnFromExcessHappiness - culturePerTurnFromTraits )
		end

		-- CBP
		if(g_activePlayer:GetTechsToFreePolicy() >= 0)then
			tips:insert( "[NEWLINE][NEWLINE]" )
			tips:insert( L("TXT_KEY_TP_TECHS_NEEDED_FOR_NEXT_FREE_POLICY", g_activePlayer:GetTechsToFreePolicy()) )
		end
--END 

		-- Let people know that building more cities makes policies harder to get

		if g_isBasicHelp then
			tips:insert( "" )
			tips:insert( L("TXT_KEY_TP_CULTURE_CITY_COST", Game.GetNumCitiesPolicyCostMod() * ( 100 + ( civBE_mode and g_activePlayer:GetNumCitiesPolicyCostDiscount() or 0 ) ) / 100 ) )
		end
	end

	return setTextToolTip( tips:concat( "[NEWLINE]" ) )
end
Controls.CultureString:RegisterCallback( Mouse.eLClick, function() GamePopup( ButtonPopupTypes.BUTTONPOPUP_CHOOSEPOLICY ) end )
Controls.CultureString:RegisterCallback( Mouse.eRClick, function() GamePedia( "TXT_KEY_CULTURE_HEADING1_TITLE" ) end )	-- TXT_KEY_PEDIA_CATEGORY_8_LABEL
Controls.CultureString:SetToolTipCallback( requestTextToolTip )

-------------------------------------------------
-- Faith Tooltip & Click Actions
-------------------------------------------------
if civ5_mode and gk_mode then
	g_toolTipHandler.FaithString = function()-- control )


		if g_isReligionEnabled then
			local tips = table()
			local faithPerTurn = g_activePlayer:GetTotalFaithPerTurn()

			if bnw_mode and g_activePlayer:IsAnarchy() then
				tips:insert( L( "TXT_KEY_TP_ANARCHY", g_activePlayer:GetAnarchyNumTurns() ) )
				tips:insert( "" )
			end

			tips:insert( L("TXT_KEY_TP_FAITH_ACCUMULATED", g_activePlayer:GetFaith()) )
			tips:insert( "" )
			tips:insert( "[COLOR_WHITE]" .. S("%+g", faithPerTurn ) .. "[ENDCOLOR] "
				.. L"TXT_KEY_YIELD_FAITH" .. "[ICON_PEACE] " .. L"TXT_KEY_GOLD_PERTURN_HEADING4_TITLE" )

			-- Faith from Cities
			tips:insertLocalizedIfNonZero( "TXT_KEY_TP_FAITH_FROM_CITIES", g_activePlayer:GetFaithPerTurnFromCities() )

			-- Faith from Outposts
			tips:insertLocalizedIfNonZero( "TXT_KEY_TP_FAITH_FROM_OUTPOSTS", civBE_mode and g_activePlayer:GetFaithPerTurnFromOutposts() or 0 )

			-- Faith from Minor Civs
			tips:insertLocalizedIfNonZero( "TXT_KEY_TP_FAITH_FROM_MINORS", g_activePlayer:GetFaithPerTurnFromMinorCivs() )

			-- Faith from Religion
			tips:insertLocalizedIfNonZero( "TXT_KEY_TP_FAITH_FROM_RELIGION", g_activePlayer:GetFaithPerTurnFromReligion() )

-- C4DF
			if g_activePlayer.GetYieldPerTurnFromVassals then
				tips:insertLocalizedIfNonZero( "TXT_KEY_TP_FAITH_VASSALS", g_activePlayer:GetYieldPerTurnFromVassals(YieldTypes.YIELD_FAITH) )
			end
-- END			
-- COMMUNITY PATCH CHANGE

--END

			-- New World Deluxe Scenario ( you still need to delete TopPanel.lua from ...\Steam\SteamApps\common\sid meier's civilization v\assets\DLC\DLC_07\Scenarios\Conquest of the New World Deluxe\UI )
			if EUI.deluxe_scenario then
				tips:insertLocalized( "TXT_KEY_NEWWORLD_SCENARIO_TP_RELIGION_TOOLTIP" )
			else
				if g_activePlayer:HasCreatedPantheon() then
					if (Game.GetNumReligionsStillToFound(false, Game.GetActivePlayer()) > 0 or g_activePlayer:HasCreatedReligion())
						and (g_activePlayer:GetCurrentEra() < GameInfoTypes.ERA_INDUSTRIAL)
					then
						tips:insertLocalizedIfNonZero( "TXT_KEY_TP_FAITH_NEXT_PROPHET", g_activePlayer:GetMinimumFaithNextGreatProphet() )
					end
				else
					if g_activePlayer:CanCreatePantheon(false) then
						tips:insertLocalizedIfNonZero( "TXT_KEY_TP_FAITH_NEXT_PANTHEON", Game.GetMinimumFaithNextPantheon() )
					else
						tips:insert( L"TXT_KEY_TP_FAITH_PANTHEONS_LOCKED" )
					end
				end

				tips:insert( "" )
				tips:insert( L( "TXT_KEY_TP_FAITH_RELIGIONS_LEFT", math_max( Game.GetNumReligionsStillToFound(false, Game.GetActivePlayer()), 0 ) ) )

				if g_activePlayer:GetCurrentEra() >= GameInfoTypes.ERA_INDUSTRIAL then
					tips:insert( "" )
					tips:insert( L( "TXT_KEY_TP_FAITH_NEXT_GREAT_PERSON", g_activePlayer:GetMinimumFaithNextGreatProphet() ) )
					local numTips = #tips
					local capitalCity = g_activePlayer:GetCapitalCity()
					if capitalCity then
						for unit in GameInfo.Units{Special = "SPECIALUNIT_PEOPLE"} do
							local unitID = unit.ID
							if capitalCity:GetUnitFaithPurchaseCost(unitID, true) > 0
								and g_activePlayer:IsCanPurchaseAnyCity(false, true, unitID, -1, YieldTypes.YIELD_FAITH)
								and g_activePlayer:DoesUnitPassFaithPurchaseCheck(unitID)
							then
								tips:insert( "[ICON_BULLET]" .. L(unit.Description) )
							end
						end
					end

					if numTips == #tips then
						tips:insert( "[ICON_BULLET]" .. L("TXT_KEY_RO_YR_NO_GREAT_PEOPLE") )
					end
				end
			end
			return setTextToolTip( tips:concat( "[NEWLINE]" ) )
		else
			return setTextToolTip( L"TXT_KEY_TOP_PANEL_RELIGION_OFF_TOOLTIP" )	--TXT_KEY_TOP_PANEL_RELIGION_OFF
		end
	end
	g_toolTipHandler.FaithIcon = g_toolTipHandler.FaithString

	local function OnFaithLClick()
		return GamePopup( ButtonPopupTypes.BUTTONPOPUP_RELIGION_OVERVIEW )
	end
	local function OnFaithRClick()
		return GamePedia( "TXT_KEY_CONCEPT_RELIGION_FAITH_EARNING_DESCRIPTION" )	-- TXT_KEY_PEDIA_CATEGORY_15_LABEL
	end
	Controls.FaithString:RegisterCallback( Mouse.eLClick, OnFaithLClick )
	Controls.FaithString:RegisterCallback( Mouse.eRClick, OnFaithRClick )
	Controls.FaithString:SetToolTipCallback( requestTextToolTip )
	Controls.FaithString:SetHide( false )
	Controls.FaithTurns:SetHide( false )
	Controls.FaithIcon:RegisterCallback( Mouse.eLClick, OnFaithLClick )
	Controls.FaithIcon:RegisterCallback( Mouse.eRClick, OnFaithRClick )
	Controls.FaithIcon:SetToolTipCallback( requestTextToolTip )
	Controls.FaithIcon:SetHide( false )
end

if civ5_mode and gk_mode then 
	g_toolTipHandler.InstantYieldsIcon = function()-- control )
		local iPlayerID = Game.GetActivePlayer();
		local pPlayer = Players[iPlayerID];

		local strInstantYieldToolTip = pPlayer:GetInstantYieldHistoryTooltip(10);

		local tips = table()

		tips:insert( strInstantYieldToolTip )	

		return setTextToolTip( tips:concat( "[NEWLINE]" ) )
	end

	Controls.InstantYieldsIcon:SetToolTipCallback( requestTextToolTip )
	Controls.InstantYieldsIcon:SetHide( false )
end

-- my modification for Luxury Resources
if civ5_mode and gk_mode then 
	g_toolTipHandler.LuxuryResources = function()-- control )
			local tips = table()
		----------------------------
			-- Local Resources in Cities
			----------------------------
			local tip = ""
			for _, resource in pairs( g_luxuries) do
				local resourceID = resource.ID
				local quantity = g_activePlayer:GetNumResourceTotal( resourceID, false ) + g_activePlayer:GetResourceExport( resourceID )
				if quantity > 0 then
					tip = tip .. " " .. ColorizeAbs( quantity ) .. resource.IconString
				end
			end
			tips:insert( L"TXT_KEY_EO_LOCAL_RESOURCES_CBP" .. (#tip > 0 and tip or (" : "..L"TXT_KEY_TP_NO_RESOURCES_DISCOVERED")) )

			-- Resources from city terrain
			for city in g_activePlayer:Cities() do
				local numConnectedResource = {}
				local numUnconnectedResource = {}
				for plot in CityPlots( city ) do
					local resourceID = plot:GetResourceType( g_activeTeamID )
					local numResource = plot:GetNumResource()
					if numResource > 0
						and Game.GetResourceUsageType( resourceID ) == ResourceUsageTypes.RESOURCEUSAGE_LUXURY
					then
						if plot:IsCity() or (not plot:IsImprovementPillaged() and plot:IsResourceConnectedByImprovement( plot:GetImprovementType() )) then
							numConnectedResource[resourceID] = (numConnectedResource[resourceID] or 0) + numResource
						else
							numUnconnectedResource[resourceID] = (numUnconnectedResource[resourceID] or 0) + numResource
						end
					end
				end
				local tip = ""
				for _, resource in pairs( g_luxuries) do
					local resourceID = resource.ID
					if (numConnectedResource[resourceID] or 0) > 0 then
						tip = tip .. " " .. ColorizeAbs( numConnectedResource[resourceID] ) .. resource.IconString
					end
					if (numUnconnectedResource[resourceID] or 0) > 0 then
						tip = tip .. " " .. ColorizeAbs( -numUnconnectedResource[resourceID] ) .. resource.IconString
					end
				end
				if #tip > 0 then
					tips:insert( "[ICON_BULLET]" .. city:GetName() .. tip )
				end
			end

			----------------------------
			-- Import & Export Breakdown
			----------------------------
			local itemType, duration, finalTurn, data1, data2, data3, flag1, fromPlayerID
			local gameTurn = Game.GetGameTurn()-1
			local Exports = {}
			local Imports = {}
			for playerID = 0, GameDefines.MAX_MAJOR_CIVS-1 do
				Exports[ playerID ] = {}
				Imports[ playerID ] = {}
			end
			PushScratchDeal()
			for i = 0, UI.GetNumCurrentDeals( g_activePlayerID ) - 1 do
				UI.LoadCurrentDeal( g_activePlayerID, i )
				local otherPlayerID = g_deal:GetOtherPlayer( g_activePlayerID )
				g_deal:ResetIterator()
				repeat
					if bnw_mode then
						itemType, duration, finalTurn, data1, data2, data3, flag1, fromPlayerID = g_deal:GetNextItem()
					else
						itemType, duration, finalTurn, data1, data2, fromPlayerID = g_deal:GetNextItem()
					end
					-- data1 is resourceID, data2 is quantity

					if data2 and itemType == TradeableItems.TRADE_ITEM_RESOURCES and Game.GetResourceUsageType( data1 ) == ResourceUsageTypes.RESOURCEUSAGE_LUXURY then
						local trade
						if fromPlayerID == g_activePlayerID then
							trade = Exports[otherPlayerID]
						else
							trade = Imports[fromPlayerID]
						end
						local resourceTrade = trade[ data1 ]
						if not resourceTrade then
							resourceTrade = {}
							trade[ data1 ] = resourceTrade
						end
						resourceTrade[finalTurn] = (resourceTrade[finalTurn] or 0) + data2
					end
				until not itemType
			end
			PopScratchDeal()

			----------------------------
			-- Imports
			----------------------------
			local tip = ""
			for _, resource in pairs( g_luxuries) do
				local resourceID = resource.ID
				local quantity = g_activePlayer:GetResourceImport( resourceID ) + g_activePlayer:GetResourceFromMinors( resourceID )
				if quantity > 0 then
					tip = tip .. " " .. ColorizeAbs( quantity ) .. resource.IconString
				end
			end
			if #tip > 0 then
				tips:insert( "" )
				tips:insert( L"TXT_KEY_RESOURCES_IMPORTED" .. tip )
				for playerID, array in pairs( Imports ) do
					local tip = ""
					for resourceID, row in pairs( array ) do
						for turn, quantity in pairs(row) do
							if quantity > 0 then
								tip = tip .. " " .. quantity .. GameInfo.Resources[ resourceID ].IconString .. "(" .. turn - gameTurn .. ")"
							end
						end
					end
					if #tip > 0 then
						tips:insert( "[ICON_BULLET]" .. Players[ playerID ]:GetCivilizationShortDescription() .. tip )
					end
				end
				for minorID = GameDefines.MAX_MAJOR_CIVS, GameDefines.MAX_CIV_PLAYERS-1 do
					local minor = Players[ minorID ]
					if minor and minor:IsAlive() and minor:GetAlly() == g_activePlayerID then
						local tip = ""
						for _, resource in pairs( g_luxuries) do
							local quantity = minor:GetResourceExport(resource.ID)
							if quantity > 0 then
								tip = tip .. " " .. quantity .. resource.IconString
							end
						end
						if #tip > 0 then
							tips:insert( "[ICON_BULLET]" .. minor:GetCivilizationShortDescription() .. tip )
						end
					end
				end
			end

			----------------------------
			-- Exports
			----------------------------
			local tip = ""
			for _, resource in pairs( g_luxuries) do
				local resourceID = resource.ID
				local quantity = g_activePlayer:GetResourceExport( resourceID )
				if quantity > 0 then
					tip = tip .. " " .. ColorizeAbs( quantity ) .. resource.IconString
				end
			end
			if #tip > 0 then
				tips:insert( "" )
				tips:insert( L"TXT_KEY_RESOURCES_EXPORTED" .. tip )
				for playerID, array in pairs( Exports ) do
					local tip = ""
					for resourceID, row in pairs( array ) do
						for turn, quantity in pairs(row) do
							if quantity > 0 then
								tip = tip .. " " .. quantity .. GameInfo.Resources[ resourceID ].IconString .. "(" .. turn - gameTurn .. ")"
							end
						end
					end
					if #tip > 0 then
						tips:insert( "[ICON_BULLET]" .. Players[ playerID ]:GetCivilizationShortDescription() .. tip )
					end
				end
			end
			-- show resources available for trade to the active player

--			tips:insert( L"TXT_KEY_DIPLO_ITEMS_LUXURY_RESOURCES" )
--			tips:insert( missingResources )
			tips:insert( "" )
			tips:insert( L"TXT_KEY_EO_RESOURCES_AVAILBLE" )

			----------------------------
			-- Available for Import
			----------------------------
			for _, resource in pairs( g_luxuries) do
				local resourceID = resource.ID
				local resources = table()
				for playerID = 0, GameDefines.MAX_CIV_PLAYERS - 1 do

					local player = Players[playerID]
					local isMinorCiv = player:IsMinorCiv()

					-- Valid player? - Can't be us, has to be alive and met, can't be allied city state
					if playerID ~= g_activePlayerID
						and player:IsAlive()
						and g_activeTeam:IsHasMet( player:GetTeam() )
						and not (isMinorCiv and player:IsAllies( g_activePlayerID ))
					then


						local numResource = ( isMinorCiv and player:GetNumResourceTotal(resourceID, false) + player:GetResourceExport( resourceID ) )
							or ( g_deal:IsPossibleToTradeItem(playerID, g_activePlayerID, TradeableItems.TRADE_ITEM_RESOURCES, resourceID, 1) and player:GetNumResourceAvailable(resourceID, false) )
							or 0
						if numResource > 0 then
							resources:insert( player:GetCivilizationShortDescription() .. " " .. numResource .. resource.IconString )
						end
					end
				end
				if #resources > 0 then
					tips:insert( "[ICON_BULLET]" .. L(resource.Description) .. ": " .. resources:concat(", ") )
				end
			end

	return setTextToolTip( tips:concat( "[NEWLINE]" ) )
		
	end

	Controls.LuxuryResources:SetToolTipCallback( requestTextToolTip )
	Controls.LuxuryResources:SetHide( false )
end
-------------------------------------------------
-- Military Tooltip & Click Actions
-------------------------------------------------
if civ5_mode and gk_mode then
	g_toolTipHandler.UnitSupplyString = function()-- control )

		local iPlayerID = Game.GetActivePlayer();
		local pPlayer = Players[iPlayerID];

		local iUnitSupplyMod = pPlayer:GetUnitProductionMaintenanceMod();
		local iUnitsSupplied = pPlayer:GetNumUnitsSupplied();
		local iUnitsTotal = pPlayer:GetNumUnitsToSupply();
		local iPercentPerPop = pPlayer:GetNumUnitsSuppliedByPopulation();
		local iPerCity = pPlayer:GetNumUnitsSuppliedByCities();
		local iPerHandicap = pPlayer:GetNumUnitsSuppliedByHandicap();
		local iWarWearinessReduction = pPlayer:GetWarWeariness();
		local iUnitsOver = pPlayer:GetNumUnitsOutOfSupply();
		local iWarWearinessActualReduction = pPlayer:GetWarWearinessSupplyReduction();
		local iTechReduction = pPlayer:GetTechSupplyReduction();
	
		local strUnitSupplyToolTip = "";
		if(iUnitsOver > 0) then
			strUnitSupplyToolTip = "[COLOR_NEGATIVE_TEXT]";
			strUnitSupplyToolTip = strUnitSupplyToolTip .. Locale.ConvertTextKey("TXT_KEY_UNIT_SUPPLY_REACHED_TOOLTIP", iUnitsSupplied, iUnitsOver, -iUnitSupplyMod);
			strUnitSupplyToolTip = strUnitSupplyToolTip .. "[ENDCOLOR]";
		end

		local strUnitSupplyToolUnderTip = Locale.ConvertTextKey("TXT_KEY_UNIT_SUPPLY_REMAINING_TOOLTIP", iUnitsSupplied, iUnitsTotal, iPercentPerPop, iPerCity, iPerHandicap, (iWarWearinessReduction / 2), iWarWearinessActualReduction, iTechReduction, iWarWearinessReduction);

		if(strUnitSupplyToolTip ~= "") then
			strUnitSupplyToolTip = strUnitSupplyToolTip .. "[NEWLINE][NEWLINE]" .. strUnitSupplyToolUnderTip;
		else
			strUnitSupplyToolTip = strUnitSupplyToolUnderTip;
		end

		local tips = table()

		tips:insert( strUnitSupplyToolTip )	

		return setTextToolTip( tips:concat( "[NEWLINE]" ) )
	end

	g_toolTipHandler.UnitSupplyIcon = g_toolTipHandler.UnitSupplyString

	local function OnUnitSupplyLClick()
		return GamePopup( ButtonPopupTypes.BUTTONPOPUP_MILITARY_OVERVIEW )
	end

	Controls.UnitSupplyString:RegisterCallback( Mouse.eLClick, OnUnitSupplyLClick )
	Controls.UnitSupplyString:SetToolTipCallback( requestTextToolTip )
	Controls.UnitSupplyString:SetHide( false )
	Controls.UnitSupplyIcon:RegisterCallback( Mouse.eLClick, OnUnitSupplyLClick )
	Controls.UnitSupplyIcon:SetToolTipCallback( requestTextToolTip )
	Controls.UnitSupplyIcon:SetHide( false )
end

-------------------------------------------------
-- Strategic Resources Tooltips & Click Actions
-------------------------------------------------
local function ResourcesToolTip( control )

	local tips = table()

	-- show resources available to the active player

	local resource = GameInfo.Resources[ control:GetVoid1() ]
	local resourceID = resource and resource.ID
	if resourceID and Game.GetResourceUsageType(resourceID) == ResourceUsageTypes.RESOURCEUSAGE_STRATEGIC then

		local numResourceUsed = g_activePlayer:GetNumResourceUsed( resourceID )

		if numResourceUsed > 0 or
			( g_activeTeamTechs:HasTech( GameInfoTypes[ resource.TechReveal ] ) and
			g_activeTeamTechs:HasTech( GameInfoTypes[ resource.TechCityTrade ] ) )
		then
--			local numResourceTotal = g_activePlayer:GetNumResourceTotal( resourceID, true )	-- true means includes both imports & minors - but exports are deducted regardless
			local numResourceAvailable = g_activePlayer:GetNumResourceAvailable( resourceID, true )	-- same as (total - used)
			local numResourceExport = g_activePlayer:GetResourceExport( resourceID )
			local numResourceImport = g_activePlayer:GetResourceImport( resourceID ) + g_activePlayer:GetResourceFromMinors( resourceID )
			local numResourceMisc = g_activePlayer:GetResourcesMisc(resourceID)
			local numResourceLocal = g_activePlayer:GetNumResourceTotal( resourceID, false ) + numResourceExport - numResourceMisc

			tips:insert( ColorizeAbs(numResourceAvailable) .. resource.IconString .. " " .. Locale.ToUpper(resource.Description) )
			tips:insert( "----------------" )

			----------------------------
			-- Local Resources in Cities
			----------------------------
			tips:insert( "" )
			tips:insert( Colorize(numResourceLocal) .. " " .. L"TXT_KEY_EO_LOCAL_RESOURCES_CBP" )

			-- Resources from city terrain
			for city in g_activePlayer:Cities() do
				local numConnectedResource = 0
				local numUnconnectedResource = 0
				for plot in CityPlots( city ) do
					local numResource = plot:GetNumResource()
					if numResource > 0  and resourceID == plot:GetResourceType( g_activeTeamID ) then
						if plot:IsCity() or (not plot:IsImprovementPillaged() and plot:IsResourceConnectedByImprovement( plot:GetImprovementType() )) then
							numConnectedResource = numConnectedResource + numResource
						else
							numUnconnectedResource = numUnconnectedResource + numResource
						end
					end
				end
				local tip = ""
				if numConnectedResource > 0 then
					tip = " " .. ColorizeAbs( numConnectedResource ) .. resource.IconString
				end
				if numUnconnectedResource > 0 then
					tip = tip .. " " .. ColorizeAbs( -numUnconnectedResource ) .. resource.IconString
				end
				if #tip > 0 then
					tips:insert( "[ICON_BULLET]" .. city:GetName() .. tip )
				end
			end
			if gk_mode then
				-- Resources from buildings
				local tipIndex = #tips
				for row in GameInfo.Building_ResourceQuantity{ ResourceType = resource.Type } do
					local building = GameInfo.Buildings[ row.BuildingType ]
					local numResource = row.Quantity
					if building and numResource and numResource > 0 then
						local buildingID = building.ID
						-- count how many such buildings player has
						local numExisting = g_activePlayer:CountNumBuildings( buildingID )
						-- count how many such units player is building
						local numBuilds = 0
						for city in g_activePlayer:Cities() do
							if city:GetProductionBuilding() == buildingID then
								numBuilds = numBuilds + 1
							end
						end
						-- can player build this building someday ?
						local canBuildSomeday
						-- check whether this Unit has been blocked out by the civ XML
						local buildingOverride = GameInfo.Civilization_BuildingClassOverrides{ CivilizationType = g_activeCivilization.Type, BuildingClassType = building.BuildingClass }()
						if buildingOverride then
							canBuildSomeday = buildingOverride.BuildingType == building.Type
						else
							canBuildSomeday = GameInfo.BuildingClasses[ building.BuildingClass ].DefaultBuilding == building.Type
						end
						canBuildSomeday = canBuildSomeday and not (
							-- no espionage buildings for a non-espionage game
							( Game.IsOption(GameOptionTypes.GAMEOPTION_NO_ESPIONAGE) and building.IsEspionage )
							-- Has obsolete tech?
							or ( building.ObsoleteTech and g_activeTeamTechs:HasTech( GameInfoTypes[building.ObsoleteTech] ) )
						)
						if canBuildSomeday or numExisting > 0 or numBuilds > 0 then
							local totalResource = (numExisting + numBuilds) * numResource
							local tip = "[COLOR_YIELD_FOOD]" .. L( building.Description ) .. "[ENDCOLOR]"
							if canBuildSomeday then
								local tech = building.PrereqTech and GameInfo.Technologies[ building.PrereqTech ]
								if tech and not g_activeTeamTechs:HasTech( tech.ID ) then
									tip = tip .. " [COLOR_CYAN]" .. L(tech.Description) .. "[ENDCOLOR]"
								end
								local policyBranch = building.PolicyBranchType and GameInfo.PolicyBranchTypes[ building.PolicyBranchType ]
								if policyBranch and not g_activePlayer:GetPolicyBranchChosen( policyBranch.ID ) then
									tip = tip .. " [COLOR_MAGENTA]" .. L(policyBranch.Description) .. "[ENDCOLOR]"
								end
							end
							if totalResource > 0 then
								tipIndex = tipIndex+1
								tips:insert( tipIndex, "[ICON_BULLET]" .. totalResource .. resource.IconString .. " = " ..  numExisting .. " (+" .. numBuilds .. ") " .. tip )
							else
								tips:insert( "[ICON_BULLET] (" .. numResource .. "/" .. tip .. ")" )
							end
						end
					end
				end
			end

			local tip2 = ""
			if numResourceMisc > 0 then
				tip2 = " " .. ColorizeAbs( numResourceMisc ) .. resource.IconString
			end
			if #tip2 > 0 then
				tips:insert( "" )
				tips:insert( Colorize(numResourceMisc) .. " " .. L"TXT_KEY_EO_MISC_RESOURCES" )
				tips:insert( "[ICON_BULLET]" .. tip2 )
			end
			----------------------------
			-- Import & Export Breakdown
			----------------------------

			-- Get specified resource traded with the active player

			local itemType, duration, finalTurn, data1, data2, data3, flag1, fromPlayerID
			local gameTurn = Game.GetGameTurn()-1
			local Exports = {}
			local Imports = {}
			for playerID = 0, GameDefines.MAX_MAJOR_CIVS-1 do
				Exports[ playerID ] = {}
				Imports[ playerID ] = {}
			end
			PushScratchDeal()
			for i = 0, UI.GetNumCurrentDeals( g_activePlayerID ) - 1 do
				UI.LoadCurrentDeal( g_activePlayerID, i )
				local otherPlayerID = g_deal:GetOtherPlayer( g_activePlayerID )
				g_deal:ResetIterator()
				repeat
					if bnw_mode then
						itemType, duration, finalTurn, data1, data2, data3, flag1, fromPlayerID = g_deal:GetNextItem()
					else
						itemType, duration, finalTurn, data1, data2, fromPlayerID = g_deal:GetNextItem()
					end
					-- data1 is resourceID, data2 is quantity

					if itemType == TradeableItems.TRADE_ITEM_RESOURCES and data1 == resourceID and data2 then
						if fromPlayerID == g_activePlayerID then
							Exports[otherPlayerID][finalTurn] = (Exports[otherPlayerID][finalTurn] or 0) + data2
						else
							Imports[fromPlayerID][finalTurn] = (Imports[fromPlayerID][finalTurn] or 0) + data2
						end
					end
				until not itemType
			end
			PopScratchDeal()

			----------------------------
			-- Resource Imports
			----------------------------
			if numResourceImport > 0 then
				tips:insert( "" )
				tips:insert( Colorize(numResourceImport) .. " " .. L"TXT_KEY_RESOURCES_IMPORTED" )
				for playerID, row in pairs( Imports ) do
					local tip = ""
					for turn, quantity in pairs(row) do
						if quantity > 0 then
							tip = tip .. " " .. quantity .. resource.IconString .. "(" .. turn - gameTurn .. ")"
						end
					end
					if #tip > 0 then
						tips:insert( "[ICON_BULLET]" .. Players[ playerID ]:GetCivilizationShortDescription() .. tip )
					end
				end
				for minorID = GameDefines.MAX_MAJOR_CIVS, GameDefines.MAX_CIV_PLAYERS-1 do
					local minor = Players[ minorID ]
					if minor and minor:IsAlive() and minor:GetAlly() == g_activePlayerID then
						local quantity = minor:GetResourceExport(resourceID)
						if quantity > 0 then
							tips:insert( "[ICON_BULLET]" .. minor:GetCivilizationShortDescription() .. " " .. quantity .. resource.IconString )
						end
					end
				end
			end
			----------------------------
			-- Resource Exports
			----------------------------
			if numResourceExport > 0 then
				tips:insert( "" )
				tips:insert( Colorize(-numResourceExport) .. " " .. L"TXT_KEY_RESOURCES_EXPORTED" )
				for playerID, row in pairs( Exports ) do
					local tip = ""
					for turn, quantity in pairs(row) do
						if quantity > 0 then
							tip = tip .. " " .. quantity .. resource.IconString .. "(" .. turn - gameTurn .. ")"
						end
					end
					if #tip > 0 then
						tips:insert( "[ICON_BULLET]" .. Players[ playerID ]:GetCivilizationShortDescription() .. tip )
					end
				end
			end

			----------------------------
			-- Resource Useage Breakdown
			----------------------------
			tips:insert( "" )
			tips:insert( Colorize(-numResourceUsed) .. " " .. L"TXT_KEY_PEDIA_REQ_RESRC_LABEL" )
			local tipIndex = #tips

			for unit in GameInfo.Units() do
				local unitID = unit.ID
				local numResource = Game.GetNumResourceRequiredForUnit( unitID, resourceID )
				if numResource > 0 then
					-- count how many such units player has
					local numExisting = 0
					for unit in g_activePlayer:Units() do
						if unit:GetUnitType() == unitID then
							numExisting = numExisting + 1
						end
					end
					-- count how many such units player is building
					local numBuilds = 0
					for city in g_activePlayer:Cities() do
						for i=0, city:GetOrderQueueLength()-1 do
							local queuedOrderType, queuedItemType = city:GetOrderFromQueue( i )
							if queuedOrderType == OrderTypes.ORDER_TRAIN and queuedItemType == unitID then
								numBuilds = numBuilds + 1
							end
						end
					end
					-- can player build this unit someday ?
					local canBuildSomeday = true
					if bnw_mode then
						-- does player trait prohibits training this unit ?
						local leader = GameInfo.Leaders[ g_activePlayer:GetLeaderType() ]
						for leaderTrait in GameInfo.Leader_Traits{ LeaderType = leader.Type } do
							if GameInfo.Trait_NoTrain{ UnitClassType = unit.Class, TraitType = leaderTrait.TraitType }() then
								canBuildSomeday = false
								break
							end
						end
					end
					if canBuildSomeday then
						-- check whether this Unit has been blocked out by the civ XML unit override
						local unitOverride = GameInfo.Civilization_UnitClassOverrides{ CivilizationType = g_activeCivilization.Type, UnitClassType = unit.Class }()
						if unitOverride then
							canBuildSomeday = unitOverride.UnitType == unit.Type
						else
							canBuildSomeday = GameInfo.UnitClasses[ unit.Class ].DefaultUnit == unit.Type
						end
					end
					canBuildSomeday = canBuildSomeday and not (
						-- one City Challenge?
						( Game.IsOption(GameOptionTypes.GAMEOPTION_ONE_CITY_CHALLENGE) and (unit.Found or unit.FoundAbroad) )
						-- Faith Requirements?
						or ( g_isReligionEnabled and (unit.FoundReligion or unit.SpreadReligion or unit.RemoveHeresy) )
						-- obsolete by tech?
						or ( unit.ObsoleteTech and g_activeTeamTechs:HasTech( GameInfoTypes[unit.ObsoleteTech] ) )
					)
					if canBuildSomeday or numExisting > 0 or numBuilds > 0 then
						local totalResource = (numExisting + numBuilds) * numResource
						local tip = "[COLOR_YELLOW]" .. L( unit.Description ) .. "[ENDCOLOR]"
						if canBuildSomeday then
							-- Tech requirements
							local tech = unit.PrereqTech and GameInfo.Technologies[ unit.PrereqTech ]
							if tech and not g_activeTeamTechs:HasTech( tech.ID ) then
								tip = S( "%s [COLOR_CYAN]%s[ENDCOLOR]", tip, L(tech.Description) )
							end
							-- Policy Requirement
							local policy = civ5bnw_mode and unit.PolicyType and GameInfo.Policies[ unit.PolicyType ]
							if policy and not g_activePlayer:HasPolicy( policy.ID ) then
								tip = S( "%s [COLOR_MAGENTA]%s[ENDCOLOR]", tip, L(policy.Description) )
							end
							if civBE_mode then
								-- Affinity Level Requirements
								for affinityPrereq in GameInfo.Unit_AffinityPrereqs{ UnitType = unit.Type } do
									local affinityInfo = (tonumber( affinityPrereq.Level) or 0 ) > 0 and GameInfo.Affinity_Types[ affinityPrereq.AffinityType ]
									if affinityInfo and g_activePlayer:GetAffinityLevel( affinityInfo.ID ) < affinityPrereq.Level then
										tip = S("%s [%s]%i%s%s[ENDCOLOR]", tip, affinityInfo.ColorType, affinityPrereq.Level, affinityInfo.IconString or "???", L(affinityInfo.Description) )
									end
								end
							end
						end
						if totalResource > 0 then
							tipIndex = tipIndex+1
							tips:insert( tipIndex, "[ICON_BULLET]" .. totalResource .. resource.IconString .. " = " ..  numExisting .. " (+" .. numBuilds .. ") " .. tip )
						else
							tips:insert( "[ICON_BULLET] (" .. numResource .. "/" .. tip .. ")" )
						end
					end
				end
			end
			for building in GameInfo.Buildings() do
				local buildingID = building.ID
				local numResource = Game.GetNumResourceRequiredForBuilding( buildingID, resourceID )
				if numResource > 0 then
					-- count how many such buildings player has
					local numExisting = g_activePlayer:CountNumBuildings( buildingID )
					-- count how many such units player is building
					local numBuilds = 0
					for city in g_activePlayer:Cities() do
						for i=0, city:GetOrderQueueLength()-1 do
							local queuedOrderType, queuedItemType = city:GetOrderFromQueue( i )
							if queuedOrderType == OrderTypes.ORDER_CONSTRUCT and queuedItemType == buildingID then
								numBuilds = numBuilds + 1
							end
						end
					end
					-- can player build this building someday ?
					local canBuildSomeday
					-- check whether this Unit has been blocked out by the civ XML
					local buildingOverride = GameInfo.Civilization_BuildingClassOverrides{ CivilizationType = g_activeCivilization.Type, BuildingClassType = building.BuildingClass }()
					if buildingOverride then
						canBuildSomeday = buildingOverride.BuildingType == building.Type
					else
						canBuildSomeday = GameInfo.BuildingClasses[ building.BuildingClass ].DefaultBuilding == building.Type
					end
					canBuildSomeday = canBuildSomeday and not (
						-- no espionage buildings for a non-espionage game
						( Game.IsOption(GameOptionTypes.GAMEOPTION_NO_ESPIONAGE) and building.IsEspionage )
						-- Has obsolete tech?
						or ( civ5_mode and building.ObsoleteTech and g_activeTeamTechs:HasTech( GameInfoTypes[building.ObsoleteTech] ) )
					)
					if canBuildSomeday or numExisting > 0 or numBuilds > 0 then
						local totalResource = (numExisting + numBuilds) * numResource
						local tip = "[COLOR_YIELD_FOOD]" .. L( building.Description ) .. "[ENDCOLOR]"
						if canBuildSomeday then
							local tech = GameInfo.Technologies[ building.PrereqTech ]
							if tech and not g_activeTeamTechs:HasTech( tech.ID ) then
								tip = S( "%s [COLOR_CYAN]%s[ENDCOLOR]", tip, L(tech.Description) )
							end
							local policyBranch = civ5bnw_mode and building.PolicyBranchType and GameInfo.PolicyBranchTypes[ building.PolicyBranchType ]
							if policyBranch and not g_activePlayer:GetPolicyBranchChosen( policyBranch.ID ) then
								tip = S( "%s [COLOR_MAGENTA]%s[ENDCOLOR]", tip, L(policyBranch.Description) )
							end
							if civBE_mode then
								-- Affinity Level Requirements
								for affinityPrereq in GameInfo.Building_AffinityPrereqs{ BuildingType = building.Type } do
									local affinityInfo = (tonumber( affinityPrereq.Level) or 0 ) > 0 and GameInfo.Affinity_Types[ affinityPrereq.AffinityType ]
									if affinityInfo and g_activePlayer:GetAffinityLevel( affinityInfo.ID ) < affinityPrereq.Level then
										tip = S("%s [%s]%i%s%s[ENDCOLOR]", tip, affinityInfo.ColorType, affinityPrereq.Level, affinityInfo.IconString or "???", L(affinityInfo.Description) )
									end
								end
							end
						end
						if totalResource > 0 then
							tipIndex = tipIndex+1
							tips:insert( tipIndex, "[ICON_BULLET]" .. totalResource .. resource.IconString .. " = " ..  numExisting .. " (+" .. numBuilds .. ") " .. tip )
						else
							tips:insert( "[ICON_BULLET] (" .. numResource .. "/" .. tip .. ")" )
						end
					end
				end
			end
		end
	end

	-- show foreign strategic resources available for trade to the active player

	local tipIndex = #tips
	local totalResource = 0
	----------------------------
	-- Available for Import
	----------------------------
	for playerID = 0, GameDefines.MAX_CIV_PLAYERS - 1 do

		local player = Players[playerID]
		local isMinorCiv = player:IsMinorCiv()

		-- Valid player? - Can't be us, has to be alive, and has to be met
		if playerID ~= g_activePlayerID
			and player:IsAlive()
			and g_activeTeam:IsHasMet( player:GetTeam() )
			and not (isMinorCiv and player:IsAllies(g_activePlayerID))
		then
			local numResource = Game.GetResourceUsageType(resourceID) == ResourceUsageTypes.RESOURCEUSAGE_STRATEGIC
				and ( ( isMinorCiv and player:GetNumResourceTotal(resourceID, false) + player:GetResourceExport( resourceID ) )
				or ( g_deal:IsPossibleToTradeItem(playerID, g_activePlayerID, TradeableItems.TRADE_ITEM_RESOURCES, resourceID, 1) and player:GetNumResourceAvailable(resourceID, false) ) )
			if numResource and numResource > 0 then
				totalResource = totalResource + numResource
				tips:insert( "[ICON_BULLET]" .. player:GetCivilizationShortDescription() .. " " .. numResource .. resource.IconString )
			end
		end
	end
	if totalResource > 0 then
		tips:insert( tipIndex+1, "" )
		tips:insert( tipIndex+2, "----------------")
		tips:insert( tipIndex+3, totalResource .. " " .. L"TXT_KEY_EO_RESOURCES_AVAILBLE" )
	end

	return setTextToolTip( tips:concat( "[NEWLINE]" ) )
end
local function ResourcesTipHandler( control )
	g_requestToolTipFunction = ResourcesToolTip
	g_requestToolTipControl = control
end

local function OnResourceLClick()-- resourceID )
	return GamePopup( ButtonPopupTypes.BUTTONPOPUP_ECONOMIC_OVERVIEW )
end
local function OnResourceRClick( resourceID )
	return GamePedia( GameInfo.Resources[ resourceID ].Description )
end

for resource in GameInfo.Resources() do
	local resourceID = resource.ID
	if Game.GetResourceUsageType( resourceID ) == ResourceUsageTypes.RESOURCEUSAGE_STRATEGIC then
		local instance = {}
		ContextPtr:BuildInstanceForControlAtIndex( "ResourceInstance", instance, Controls.TopPanelDiploStack, 8 )
		instance = instance.ResourceItem
		g_resourceString[ resourceID ] = instance
		instance:SetVoid1( resourceID )
		instance:SetToolTipCallback( ResourcesTipHandler )
		instance:RegisterCallback( Mouse.eLClick, OnResourceLClick )
		instance:RegisterCallback( Mouse.eRClick, OnResourceRClick )
	end
end

-------------------------------------------------
-- Initialization
-------------------------------------------------

local function UpdateTopPanel()
	g_requestTopPanelUpdate = true
end

local function UpdateOptions()
	g_clockFormat = g_options
				and g_options.GetValue
				and g_options.GetValue( "Clock" ) == 1
				and g_clockFormats[ g_options.GetValue("ClockMode") or 1 ]
	Controls.CurrentTime:SetHide( not g_clockFormat )
	g_isBasicHelp = civBE_mode or not OptionsManager.IsNoBasicHelp()
	g_isScienceEnabled = not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE)
	g_isPoliciesEnabled = not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_POLICIES)
	g_isHappinessEnabled = civ5_mode and not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_HAPPINESS)
	g_isReligionEnabled = civ5_mode and gk_mode and not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)
	g_isHealthEnabled = not (civ5_mode or Game.IsOption(GameOptionTypes.GAMEOPTION_NO_HEALTH) )
	UpdateTopPanel()
end

local function SetActivePlayer()
	g_activePlayerID = Game.GetActivePlayer()
	g_activePlayer = Players[g_activePlayerID]
	g_activeTeamID = g_activePlayer:GetTeam()
	g_activeTeam = Teams[g_activeTeamID]
	g_activeCivilizationID = g_activePlayer:GetCivilizationType()
	g_activeCivilization = GameInfo.Civilizations[ g_activeCivilizationID ]
	g_activeTeamTechs = g_activeTeam:GetTeamTechs()
	UpdateOptions()
end
SetActivePlayer()

Controls.TopPanelBar:SetHide( not g_isSmallScreen )
Controls.TopPanelBarL:SetHide( g_isSmallScreen )
Controls.TopPanelBarR:SetHide( g_isSmallScreen )
Controls.TopPanelMask:SetHide( true )

ContextPtr:SetUpdate(
function()
--	if IsGameCoreBusy() then
--		return
--	end

	if g_alarmTime and os_time() >= g_alarmTime then
		g_alarmTime = nil
		UI.AddPopup{ Type = ButtonPopupTypes.BUTTONPOPUP_TEXT,
			Data1 = 800,	-- WrapWidth
			Option1 = true, -- show TopImage
			Text = os_date( g_clockFormat ) }
	end

	if g_clockFormat then
		Controls.CurrentTime:SetText( os_date( g_clockFormat ) )
	end

	g_activePlayerID = Game.GetActivePlayer()
	g_activePlayer = Players[g_activePlayerID]

	if g_isPopupUp ~= UI.IsPopupUp() then
		Controls.TopPanelMask:SetHide( g_isPopupUp or g_isSmallScreen )
		g_isPopupUp = not g_isPopupUp
		UpdateTopPanelNow()

	elseif g_requestTopPanelUpdate then
		UpdateTopPanelNow()
	end

	if g_requestToolTipControl then
		local toolTipHandler = g_requestToolTipFunction or g_toolTipHandler[g_requestToolTipControl:GetID()]
		if toolTipHandler then
			toolTipHandler( g_requestToolTipControl )
		end
		g_requestToolTipControl = nil
		g_requestToolTipFunction = nil
	end
end)

Events.SerialEventGameDataDirty.Add( UpdateTopPanel )
Events.SerialEventTurnTimerDirty.Add( UpdateTopPanel )
Events.SerialEventCityInfoDirty.Add( UpdateTopPanel )
Events.SerialEventImprovementCreated.Add( UpdateTopPanel )	-- required to update happiness & resources if a resource got hooked up
Events.GameplaySetActivePlayer.Add( SetActivePlayer )
Events.GameOptionsChanged.Add( UpdateOptions )

-------------------------------------------------
-- Alarm Clock
-------------------------------------------------

for clockFormatIndex, clockFormat in ipairs( g_clockFormats ) do
	local instance = {}
	ContextPtr:BuildInstanceForControl( "ClockOptionInstance", instance, Controls.ClockOptions )
	instance = instance.ClockOption
	instance:GetTextButton():SetText( os_date( clockFormat ) )
	instance:SetCheck( g_clockFormat == clockFormat )
	instance:RegisterCheckHandler(
	function( isChecked )
		if isChecked then
			g_options.SetValue( "ClockMode", clockFormatIndex )
			UpdateOptions()
		end
	end)
end
local function GetAlarmOptions()
	g_alarmTime = nil
	local time = tonumber( g_options.GetValue( "AlarmTime" ) ) or 0
	local t = os_date( "*t", time )
	if t then
		Controls.AlarmHours:SetText( S( "%2d", t.hour ) )
		Controls.AlarmMinutes:SetText( S( "%2d", t.min ) )
		if time > os_time() + 1 then

			g_alarmTime = g_options.GetValue( "AlarmIsOn" ) == 1 and time
		end
	end
	Controls.AlarmCheckBox:SetCheck( g_alarmTime )
end
GetAlarmOptions()
Controls.ClockOptions:CalculateSize()
Controls.ClockOptionsPanel:SetSizeY( Controls.ClockOptions:GetSizeY() + 88 )

Controls.CurrentTime:RegisterCallback( Mouse.eLClick,
function()
	Controls.ClockOptionsPanel:SetHide( not Controls.ClockOptionsPanel:IsHidden() )
end)

Controls.ClockOptionsPanelClose:RegisterCallback( Mouse.eLClick,
function()
	Controls.ClockOptionsPanel:SetHide( true )
end)

local function SetAlarmOptions()
	local t = os_date("*t")
	t.hour = tonumber( Controls.AlarmHours:GetText() ) or 0
	t.min = tonumber( Controls.AlarmMinutes:GetText() ) or 0
	local time = os_time(t)

	if time < os_time()+2 then
		time = time + 86400	-- 1 day in seconds
	end
	g_options.SetValue( "AlarmTime", time )
	g_options.SetValue( "AlarmIsOn", Controls.AlarmCheckBox:IsChecked() )

	GetAlarmOptions()
end

Controls.AlarmHours:RegisterCallback( SetAlarmOptions )
Controls.AlarmMinutes:RegisterCallback( SetAlarmOptions )
Controls.AlarmCheckBox:RegisterCheckHandler( SetAlarmOptions )

print("Finished loading EUI top panel",os.clock())
end)
